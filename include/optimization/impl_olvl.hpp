#ifndef _OPTIMIZATION_IMPL_OLVL_HPP
#define _OPTIMIZATION_IMPL_OLVL_HPP

#ifdef __OPTIM_LEVEL__
#if __OPTIM_LEVEL__ >= 1 && __OPTIM_LEVEL__ <= 2

#define GET_INSTRUCTION(X) (*context->p_instructions)[X]
#define GET_CFG_BLOCK(X) context->control_flow_graph->blocks[X]

#ifndef __MASK_TYPE__
using mask_t = TULong;
#endif
using AstInstruction =
#if __OPTIM_LEVEL__ == 1
    TacInstruction
#elif __OPTIM_LEVEL__ == 2
    AsmInstruction
#endif
    ;

struct ControlFlowBlock {
    size_t size;
    size_t instructions_front_index;
    size_t instructions_back_index;
    std::vector<size_t> predecessor_ids;
    std::vector<size_t> successor_ids;
};

struct ControlFlowGraph {
    size_t entry_id;
    size_t exit_id;
    std::vector<size_t> entry_successor_ids;
    std::vector<size_t> exit_predecessor_ids;
    std::vector<bool> reaching_code;
    std::vector<ControlFlowBlock> blocks;
    std::unordered_map<TIdentifier, size_t> identifier_id_map;
};

struct DataFlowAnalysis {
    size_t set_size;
    size_t mask_size;
    size_t incoming_index;
    std::vector<size_t> open_block_ids;
    std::vector<size_t> instruction_index_map;
    std::vector<mask_t> blocks_mask_sets;
    std::vector<mask_t> instructions_mask_sets;
    std::unordered_set<TIdentifier> alias_set;
#if __OPTIM_LEVEL__ == 1
    // Copy propagation
    std::vector<size_t> data_index_map;
    std::vector<std::unique_ptr<TacInstruction>> bak_instructions;
    // Dead store elimination
#elif __OPTIM_LEVEL__ == 2
    // Register allocation
    std::vector<TIdentifier> data_name_map;
#endif
    size_t static_index;
    size_t addressed_index;
};

static void set_instruction(std::unique_ptr<AstInstruction>&& instruction, size_t instruction_index) {
    if (instruction) {
        GET_INSTRUCTION(instruction_index) = std::move(instruction);
    }
    else {
        GET_INSTRUCTION(instruction_index).reset();
    }
#if __OPTIM_LEVEL__ == 1
    context->is_fixed_point = false;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Control flow graph

static void control_flow_graph_add_edge(std::vector<size_t>& successor_ids, std::vector<size_t>& predecessor_ids,
    size_t successor_id, size_t predecessor_id) {
    if (std::find(successor_ids.begin(), successor_ids.end(), successor_id) == successor_ids.end()) {
        successor_ids.push_back(successor_id);
    }
    if (std::find(predecessor_ids.begin(), predecessor_ids.end(), predecessor_id) == predecessor_ids.end()) {
        predecessor_ids.push_back(predecessor_id);
    }
}

static void control_flow_graph_add_successor_edge(size_t block_id, size_t successor_id) {
    if (successor_id < context->control_flow_graph->exit_id) {
        control_flow_graph_add_edge(
            GET_CFG_BLOCK(block_id).successor_ids, GET_CFG_BLOCK(successor_id).predecessor_ids, successor_id, block_id);
    }
    else if (successor_id == context->control_flow_graph->exit_id) {
        control_flow_graph_add_edge(GET_CFG_BLOCK(block_id).successor_ids,
            context->control_flow_graph->exit_predecessor_ids, successor_id, block_id);
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static void control_flow_graph_add_predecessor_edge(size_t block_id, size_t predecessor_id) {
    if (predecessor_id < context->control_flow_graph->exit_id) {
        control_flow_graph_add_edge(GET_CFG_BLOCK(predecessor_id).successor_ids,
            GET_CFG_BLOCK(block_id).predecessor_ids, block_id, predecessor_id);
    }
    else if (predecessor_id == context->control_flow_graph->entry_id) {
        control_flow_graph_add_edge(context->control_flow_graph->entry_successor_ids,
            GET_CFG_BLOCK(block_id).predecessor_ids, block_id, predecessor_id);
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

#if __OPTIM_LEVEL__ == 1
static void control_flow_graph_remove_edge(std::vector<size_t>& successor_ids, std::vector<size_t>& predecessor_ids,
    size_t successor_id, size_t predecessor_id, bool is_reachable) {
    if (is_reachable) {
        for (size_t i = successor_ids.size(); i-- > 0;) {
            if (successor_ids[i] == successor_id) {
                std::swap(successor_ids[i], successor_ids.back());
                successor_ids.pop_back();
                break;
            }
        }
    }
    for (size_t i = predecessor_ids.size(); i-- > 0;) {
        if (predecessor_ids[i] == predecessor_id) {
            std::swap(predecessor_ids[i], predecessor_ids.back());
            predecessor_ids.pop_back();
            break;
        }
    }
}

static void control_flow_graph_remove_successor_edge(size_t block_id, size_t successor_id, bool is_reachable) {
    if (successor_id < context->control_flow_graph->exit_id) {
        control_flow_graph_remove_edge(GET_CFG_BLOCK(block_id).successor_ids,
            GET_CFG_BLOCK(successor_id).predecessor_ids, successor_id, block_id, is_reachable);
    }
    else if (successor_id == context->control_flow_graph->exit_id) {
        control_flow_graph_remove_edge(GET_CFG_BLOCK(block_id).successor_ids,
            context->control_flow_graph->exit_predecessor_ids, successor_id, block_id, is_reachable);
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static void control_flow_graph_remove_predecessor_edge(size_t block_id, size_t predecessor_id) {
    if (predecessor_id < context->control_flow_graph->exit_id) {
        control_flow_graph_remove_edge(GET_CFG_BLOCK(predecessor_id).successor_ids,
            GET_CFG_BLOCK(block_id).predecessor_ids, block_id, predecessor_id, true);
    }
    else if (predecessor_id == context->control_flow_graph->entry_id) {
        control_flow_graph_remove_edge(context->control_flow_graph->entry_successor_ids,
            GET_CFG_BLOCK(block_id).predecessor_ids, block_id, predecessor_id, true);
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static void control_flow_graph_remove_empty_block(size_t block_id, bool is_reachable) {
    for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
        if (is_reachable) {
            for (size_t predecessor_id : GET_CFG_BLOCK(block_id).predecessor_ids) {
                if (predecessor_id == context->control_flow_graph->entry_id) {
                    control_flow_graph_add_predecessor_edge(successor_id, predecessor_id);
                }
                else {
                    control_flow_graph_add_successor_edge(predecessor_id, successor_id);
                }
            }
        }
        control_flow_graph_remove_successor_edge(block_id, successor_id, is_reachable);
    }
    if (is_reachable) {
        for (size_t predecessor_id : GET_CFG_BLOCK(block_id).predecessor_ids) {
            control_flow_graph_remove_predecessor_edge(block_id, predecessor_id);
        }
    }
    GET_CFG_BLOCK(block_id).instructions_front_index = context->control_flow_graph->exit_id;
    GET_CFG_BLOCK(block_id).instructions_back_index = context->control_flow_graph->exit_id;
}

static void control_flow_graph_remove_block_instruction(size_t instruction_index, size_t block_id) {
    if (GET_INSTRUCTION(instruction_index)) {
        set_instruction(nullptr, instruction_index);
        GET_CFG_BLOCK(block_id).size--;
        if (GET_CFG_BLOCK(block_id).size == 0) {
            control_flow_graph_remove_empty_block(block_id, true);
        }
        else if (instruction_index == GET_CFG_BLOCK(block_id).instructions_front_index) {
            for (; instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
                if (GET_INSTRUCTION(instruction_index)) {
                    GET_CFG_BLOCK(block_id).instructions_front_index = instruction_index;
                    break;
                }
            }
        }
        else if (instruction_index == GET_CFG_BLOCK(block_id).instructions_back_index) {
            instruction_index++;
            for (; instruction_index-- > GET_CFG_BLOCK(block_id).instructions_front_index;) {
                if (GET_INSTRUCTION(instruction_index)) {
                    GET_CFG_BLOCK(block_id).instructions_back_index = instruction_index;
                    break;
                }
            }
        }
    }
}
#endif

#if __OPTIM_LEVEL__ == 1
static void control_flow_graph_initialize_label_block(TacLabel* node) {
    context->control_flow_graph->identifier_id_map[node->name] = context->control_flow_graph->blocks.size() - 1;
}
#elif __OPTIM_LEVEL__ == 2
static void control_flow_graph_initialize_label_block(AsmLabel* node) {
    context->control_flow_graph->identifier_id_map[node->name] = context->control_flow_graph->blocks.size() - 1;
}
#endif

static void control_flow_graph_initialize_block(size_t instruction_index, size_t& instructions_back_index) {
    AstInstruction* node = GET_INSTRUCTION(instruction_index).get();
    switch (node->type()) {
#if __OPTIM_LEVEL__ == 1
        case AST_T::TacLabel_t:
#elif __OPTIM_LEVEL__ == 2
        case AST_T::AsmLabel_t:
#endif
        {
            if (instructions_back_index != context->p_instructions->size()) {
                context->control_flow_graph->blocks.back().instructions_back_index = instructions_back_index;
                ControlFlowBlock block = {0, instruction_index, 0, {}, {}};
                context->control_flow_graph->blocks.emplace_back(std::move(block));
            }
#if __OPTIM_LEVEL__ == 1
            control_flow_graph_initialize_label_block(static_cast<TacLabel*>(node));
#elif __OPTIM_LEVEL__ == 2
            control_flow_graph_initialize_label_block(static_cast<AsmLabel*>(node));
#endif
            instructions_back_index = instruction_index;
            break;
        }
#if __OPTIM_LEVEL__ == 1
        case AST_T::TacReturn_t:
        case AST_T::TacJump_t:
        case AST_T::TacJumpIfZero_t:
        case AST_T::TacJumpIfNotZero_t:
#elif __OPTIM_LEVEL__ == 2
        case AST_T::AsmJmp_t:
        case AST_T::AsmJmpCC_t:
        case AST_T::AsmRet_t:
#endif
        {
            context->control_flow_graph->blocks.back().instructions_back_index = instruction_index;
            instructions_back_index = context->p_instructions->size();
            break;
        }
        default: {
            instructions_back_index = instruction_index;
            break;
        }
    }
}

#if __OPTIM_LEVEL__ == 1
static void control_flow_graph_initialize_jump_edges(TacJump* node, size_t block_id) {
    control_flow_graph_add_successor_edge(block_id, context->control_flow_graph->identifier_id_map[node->target]);
}

static void control_flow_graph_initialize_jump_if_zero_edges(TacJumpIfZero* node, size_t block_id) {
    control_flow_graph_add_successor_edge(block_id, context->control_flow_graph->identifier_id_map[node->target]);
    control_flow_graph_add_successor_edge(block_id, block_id + 1);
}

static void control_flow_graph_initialize_jump_if_not_zero_edges(TacJumpIfNotZero* node, size_t block_id) {
    control_flow_graph_add_successor_edge(block_id, context->control_flow_graph->identifier_id_map[node->target]);
    control_flow_graph_add_successor_edge(block_id, block_id + 1);
}
#elif __OPTIM_LEVEL__ == 2
static void control_flow_graph_initialize_jmp_edges(AsmJmp* node, size_t block_id) {
    control_flow_graph_add_successor_edge(block_id, context->control_flow_graph->identifier_id_map[node->target]);
}

static void control_flow_graph_initialize_jmp_cc_edges(AsmJmpCC* node, size_t block_id) {
    control_flow_graph_add_successor_edge(block_id, context->control_flow_graph->identifier_id_map[node->target]);
    control_flow_graph_add_successor_edge(block_id, block_id + 1);
}
#endif

static void control_flow_graph_initialize_edges(size_t block_id) {
    AstInstruction* node = GET_INSTRUCTION(GET_CFG_BLOCK(block_id).instructions_back_index).get();
    switch (node->type()) {
#if __OPTIM_LEVEL__ == 1
        case AST_T::TacReturn_t:
#elif __OPTIM_LEVEL__ == 2
        case AST_T::AsmRet_t:
#endif
            control_flow_graph_add_successor_edge(block_id, context->control_flow_graph->exit_id);
            break;
#if __OPTIM_LEVEL__ == 1
        case AST_T::TacJump_t:
            control_flow_graph_initialize_jump_edges(static_cast<TacJump*>(node), block_id);
            break;
        case AST_T::TacJumpIfZero_t:
            control_flow_graph_initialize_jump_if_zero_edges(static_cast<TacJumpIfZero*>(node), block_id);
            break;
        case AST_T::TacJumpIfNotZero_t:
            control_flow_graph_initialize_jump_if_not_zero_edges(static_cast<TacJumpIfNotZero*>(node), block_id);
            break;
#elif __OPTIM_LEVEL__ == 2
        case AST_T::AsmJmp_t:
            control_flow_graph_initialize_jmp_edges(static_cast<AsmJmp*>(node), block_id);
            break;
        case AST_T::AsmJmpCC_t:
            control_flow_graph_initialize_jmp_cc_edges(static_cast<AsmJmpCC*>(node), block_id);
            break;
#endif
        default:
            control_flow_graph_add_successor_edge(block_id, block_id + 1);
            break;
    }
}

static void control_flow_graph_initialize() {
    context->control_flow_graph->blocks.clear();
    context->control_flow_graph->identifier_id_map.clear();
    {
        size_t instructions_back_index = context->p_instructions->size();
        for (size_t instruction_index = 0; instruction_index < context->p_instructions->size(); ++instruction_index) {
            if (GET_INSTRUCTION(instruction_index)) {
                if (instructions_back_index == context->p_instructions->size()) {
                    ControlFlowBlock block = {0, instruction_index, 0, {}, {}};
                    context->control_flow_graph->blocks.emplace_back(std::move(block));
                }
                control_flow_graph_initialize_block(instruction_index, instructions_back_index);
                context->control_flow_graph->blocks.back().size++;
            }
        }
        if (instructions_back_index != context->p_instructions->size()) {
            context->control_flow_graph->blocks.back().instructions_back_index = instructions_back_index;
        }
    }

    context->control_flow_graph->exit_id = context->control_flow_graph->blocks.size();
    context->control_flow_graph->entry_id = context->control_flow_graph->exit_id + 1;
    context->control_flow_graph->entry_successor_ids.clear();
    context->control_flow_graph->exit_predecessor_ids.clear();
    if (!context->control_flow_graph->blocks.empty()) {
        control_flow_graph_add_predecessor_edge(0, context->control_flow_graph->entry_id);
        for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
            control_flow_graph_initialize_edges(block_id);
        }
    }
}

// // TODO rm
// #include "util/pprint.hpp"
// static void print_control_flow_graph() {
//     printf("\n\n----------------------------------------\nControlFlowGraph[%lu]:\n",
//         context->control_flow_graph->blocks.size());
//     printf("entry_id: %lu\n"
//            "exit_id: %lu\n"
//            "entry_successor_ids: ",
//         context->control_flow_graph->entry_id, context->control_flow_graph->exit_id);
//     for (size_t successor_id : context->control_flow_graph->entry_successor_ids) {
//         if (successor_id == context->control_flow_graph->entry_id) {
//             printf("ENTRY, ");
//         }
//         else if (successor_id == context->control_flow_graph->exit_id) {
//             printf("EXIT, ");
//         }
//         else {
//             printf("%lu, ", successor_id);
//         }
//     }
//     printf("\nexit_predecessor_ids: ");
//     for (size_t predecessor_id : context->control_flow_graph->exit_predecessor_ids) {
//         if (predecessor_id == context->control_flow_graph->entry_id) {
//             printf("ENTRY, ");
//         }
//         else if (predecessor_id == context->control_flow_graph->exit_id) {
//             printf("EXIT, ");
//         }
//         else {
//             printf("%lu, ", predecessor_id);
//         }
//     }
//     printf("\nblocks: \n");
//     for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
//         printf("\n--- ControlFlowBlock[%lu]\n"
//                "    size: %lu\n"
//                "    instructions_front_index: %lu\n"
//                "    instructions_back_index: %lu\n"
//                "    predecessor_ids: ",
//             block_id, GET_CFG_BLOCK(block_id).size, GET_CFG_BLOCK(block_id).instructions_front_index,
//             GET_CFG_BLOCK(block_id).instructions_back_index);
//         for (size_t predecessor_id : GET_CFG_BLOCK(block_id).predecessor_ids) {
//             if (predecessor_id == context->control_flow_graph->entry_id) {
//                 printf("ENTRY, ");
//             }
//             else if (predecessor_id == context->control_flow_graph->exit_id) {
//                 printf("EXIT, ");
//             }
//             else {
//                 printf("%lu, ", predecessor_id);
//             }
//         }
//         printf("\n    successor_ids: ");
//         for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
//             if (successor_id == context->control_flow_graph->entry_id) {
//                 printf("ENTRY, ");
//             }
//             else if (successor_id == context->control_flow_graph->exit_id) {
//                 printf("EXIT, ");
//             }
//             else {
//                 printf("%lu, ", successor_id);
//             }
//         }
//         printf("\n    instructions: \n");
//         std::unique_ptr<TacTopLevel> print_ast;
//         {
//             std::vector<std::unique_ptr<AstInstruction>> print_instructions;
//             print_instructions.reserve(
//                 GET_CFG_BLOCK(block_id).instructions_back_index - GET_CFG_BLOCK(block_id).instructions_front_index +
//                 1);
//             for (size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
//                  instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
//                 print_instructions.push_back(std::move(GET_INSTRUCTION(instruction_index)));
//             }
//             print_ast = std::make_unique<TacFunction>("CFG BLOCK ID " + std::to_string(block_id), false,
//                 std::vector<std::string>(0), std::move(print_instructions));
//         }
//         pretty_print_ast(print_ast.get(), std::to_string(block_id));
//         {
//             std::vector<std::unique_ptr<AstInstruction>>& print_instructions =
//                 static_cast<TacFunction*>(print_ast.get())->body;
//             for (size_t instruction_index = 0; instruction_index < print_instructions.size(); ++instruction_index) {
//                 GET_INSTRUCTION(GET_CFG_BLOCK(block_id).instructions_front_index + instruction_index) =
//                     std::move(print_instructions[instruction_index]);
//             }
//         }
//     }
//     // RAISE_INTERNAL_ERROR;
// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Data flow analysis

static bool mask_get(TULong mask, size_t bit) {
    if (bit > 63) {
        bit %= 64;
    }
    return (mask & (static_cast<TULong>(1ul) << bit)) > 0;
}

static void mask_set(TULong& mask, size_t bit, bool value) {
    if (bit > 63) {
        bit %= 64;
    }
    if (value) {
        mask |= static_cast<TULong>(1ul) << bit;
    }
    else {
        mask &= ~(static_cast<TULong>(1ul) << bit);
    }
}

#define MASK_FALSE 0ul
#if __OPTIM_LEVEL__ == 1
#define MASK_TRUE 18446744073709551615ul
#endif
#define MASK_OFFSET(X) X > 63 ? X / 64 : 0

#define GET_DFA_BLOCK_SET_INDEX(X, Y) (X) * context->data_flow_analysis->mask_size + (Y)
#define GET_DFA_INSTRUCTION_SET_INDEX(X, Y) \
    context->data_flow_analysis->instruction_index_map[X] * context->data_flow_analysis->mask_size + (Y)

#define GET_DFA_BLOCK_SET_MASK(X, Y) context->data_flow_analysis->blocks_mask_sets[GET_DFA_BLOCK_SET_INDEX(X, Y)]
#define GET_DFA_INSTRUCTION_SET_MASK(X, Y) \
    context->data_flow_analysis->instructions_mask_sets[GET_DFA_INSTRUCTION_SET_INDEX(X, Y)]

#define GET_DFA_BLOCK_SET_AT(X, Y) mask_get(GET_DFA_BLOCK_SET_MASK(X, MASK_OFFSET(Y)), Y)
#define GET_DFA_INSTRUCTION_SET_AT(X, Y) mask_get(GET_DFA_INSTRUCTION_SET_MASK(X, MASK_OFFSET(Y)), Y)

#define SET_DFA_INSTRUCTION_SET_AT(X, Y, Z) mask_set(GET_DFA_INSTRUCTION_SET_MASK(X, MASK_OFFSET(Y)), Y, Z)

#if __OPTIM_LEVEL__ == 1
#define GET_DFA_INSTRUCTION(X) GET_INSTRUCTION(context->data_flow_analysis->data_index_map[X])
#endif

static bool is_transfer_instruction(size_t instruction_index
#if __OPTIM_LEVEL__ == 1
    ,
    bool is_dead_store_elimination
#endif
) {
    switch (GET_INSTRUCTION(instruction_index)->type()) {
#if __OPTIM_LEVEL__ == 1
        case AST_T::TacSignExtend_t:
        case AST_T::TacTruncate_t:
        case AST_T::TacZeroExtend_t:
        case AST_T::TacDoubleToInt_t:
        case AST_T::TacDoubleToUInt_t:
        case AST_T::TacIntToDouble_t:
        case AST_T::TacUIntToDouble_t:
        case AST_T::TacFunCall_t:
        case AST_T::TacUnary_t:
        case AST_T::TacBinary_t:
        case AST_T::TacCopy_t:
        case AST_T::TacGetAddress_t:
        case AST_T::TacLoad_t:
        case AST_T::TacStore_t:
        case AST_T::TacAddPtr_t:
        case AST_T::TacCopyToOffset_t:
        case AST_T::TacCopyFromOffset_t:
            return true;
        case AST_T::TacReturn_t:
        case AST_T::TacJumpIfZero_t:
        case AST_T::TacJumpIfNotZero_t:
            return is_dead_store_elimination;
#elif __OPTIM_LEVEL__ == 2
        case AST_T::AsmMov_t:
        case AST_T::AsmUnary_t:
        case AST_T::AsmBinary_t:
        case AST_T::AsmCmp_t:
        case AST_T::AsmIdiv_t:
        case AST_T::AsmCdq_t:
        case AST_T::AsmSetCC_t:
        case AST_T::AsmPush_t:
        case AST_T::AsmCall_t:
            return true;
#endif
        default:
            return false;
    }
}

#if __OPTIM_LEVEL__ == 1
static size_t get_dfa_data_index(size_t instruction_index) {
    for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
        if (context->data_flow_analysis->data_index_map[i] == instruction_index) {
            return i;
        }
    }
    RAISE_INTERNAL_ERROR;
}

static TacInstruction* get_dfa_bak_instruction(size_t i) {
    if (context->control_flow_graph->reaching_code[i]) {
        if (context->data_flow_analysis->bak_instructions[i]) {
            return context->data_flow_analysis->bak_instructions[i].get();
        }
        else {
            RAISE_INTERNAL_ERROR;
        }
    }
    else if (GET_DFA_INSTRUCTION(i)) {
        return GET_DFA_INSTRUCTION(i).get();
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static bool set_dfa_bak_instruction(size_t instruction_index, size_t& i) {
    i = get_dfa_data_index(instruction_index);
    if (!context->control_flow_graph->reaching_code[i]) {
        context->control_flow_graph->reaching_code[i] = true;
        return true;
    }
    else {
        return false;
    }
}
#endif

#if __OPTIM_LEVEL__ == 1
static bool copy_propagation_transfer_reaching_copies(size_t instruction_index, size_t next_instruction_index);
static void eliminate_dead_store_transfer_live_values(size_t instruction_index, size_t next_instruction_index);
#elif __OPTIM_LEVEL__ == 2
static void inference_graph_transfer_live_registers(size_t instruction_index, size_t next_instruction_index);
#endif

#if __OPTIM_LEVEL__ == 1
static size_t data_flow_analysis_forward_transfer_block(size_t instruction_index, size_t block_id) {
    for (size_t next_instruction_index = instruction_index + 1;
         next_instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++next_instruction_index) {
        if (GET_INSTRUCTION(next_instruction_index) && is_transfer_instruction(next_instruction_index, false)) {
            for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
                GET_DFA_INSTRUCTION_SET_MASK(next_instruction_index, i) =
                    GET_DFA_INSTRUCTION_SET_MASK(instruction_index, i);
            }
            if (!copy_propagation_transfer_reaching_copies(instruction_index, next_instruction_index)) {
                for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
                    GET_DFA_INSTRUCTION_SET_MASK(next_instruction_index, i) =
                        GET_DFA_INSTRUCTION_SET_MASK(instruction_index, i);
                }
            }
            instruction_index = next_instruction_index;
        }
    }
    for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
        GET_DFA_INSTRUCTION_SET_MASK(context->data_flow_analysis->incoming_index, i) =
            GET_DFA_INSTRUCTION_SET_MASK(instruction_index, i);
    }
    if (!copy_propagation_transfer_reaching_copies(instruction_index, context->data_flow_analysis->incoming_index)) {
        for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
            GET_DFA_INSTRUCTION_SET_MASK(context->data_flow_analysis->incoming_index, i) =
                GET_DFA_INSTRUCTION_SET_MASK(instruction_index, i);
        }
    }
    return instruction_index;
}
#endif

static size_t data_flow_analysis_backward_transfer_block(size_t instruction_index, size_t block_id) {
    if (instruction_index > 0) {
        for (size_t next_instruction_index = instruction_index;
             next_instruction_index-- > GET_CFG_BLOCK(block_id).instructions_front_index;) {
            if (GET_INSTRUCTION(next_instruction_index)
                && is_transfer_instruction(next_instruction_index
#if __OPTIM_LEVEL__ == 1
                    ,
                    true
#endif
                    )) {
                for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
                    GET_DFA_INSTRUCTION_SET_MASK(next_instruction_index, i) =
                        GET_DFA_INSTRUCTION_SET_MASK(instruction_index, i);
                }
#if __OPTIM_LEVEL__ == 1
                eliminate_dead_store_transfer_live_values
#elif __OPTIM_LEVEL__ == 2
                inference_graph_transfer_live_registers
#endif
                    (instruction_index, next_instruction_index);
                instruction_index = next_instruction_index;
            }
        }
    }
    for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
        GET_DFA_INSTRUCTION_SET_MASK(context->data_flow_analysis->incoming_index, i) =
            GET_DFA_INSTRUCTION_SET_MASK(instruction_index, i);
    }
#if __OPTIM_LEVEL__ == 1
    eliminate_dead_store_transfer_live_values
#elif __OPTIM_LEVEL__ == 2
    inference_graph_transfer_live_registers
#endif
        (instruction_index, context->data_flow_analysis->incoming_index);
    return instruction_index;
}

static bool data_flow_analysis_after_meet_block(size_t block_id) {
    bool is_fixed_point = true;
    {
        size_t i = 0;
        for (; i < context->data_flow_analysis->mask_size; ++i) {
            if (GET_DFA_BLOCK_SET_MASK(block_id, i)
                != GET_DFA_INSTRUCTION_SET_MASK(context->data_flow_analysis->incoming_index, i)) {
                is_fixed_point = false;
                break;
            }
        }
        for (; i < context->data_flow_analysis->mask_size; ++i) {
            GET_DFA_BLOCK_SET_MASK(block_id, i) =
                GET_DFA_INSTRUCTION_SET_MASK(context->data_flow_analysis->incoming_index, i);
        }
    }
    return is_fixed_point;
}

#if __OPTIM_LEVEL__ == 1
static bool data_flow_analysis_forward_meet_block(size_t block_id) {
    size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
    for (; instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
        if (GET_INSTRUCTION(instruction_index) && is_transfer_instruction(instruction_index, false)) {
            goto Lelse;
        }
    }
    instruction_index = context->data_flow_analysis->incoming_index;
Lelse:
    for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
        GET_DFA_INSTRUCTION_SET_MASK(instruction_index, i) = MASK_TRUE;
    }

    for (size_t predecessor_id : GET_CFG_BLOCK(block_id).predecessor_ids) {
        if (predecessor_id < context->control_flow_graph->exit_id) {
            for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
                GET_DFA_INSTRUCTION_SET_MASK(instruction_index, i) &= GET_DFA_BLOCK_SET_MASK(predecessor_id, i);
            }
        }
        else if (predecessor_id == context->control_flow_graph->entry_id) {
            for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
                GET_DFA_INSTRUCTION_SET_MASK(instruction_index, i) = MASK_FALSE;
            }
            break;
        }
        else {
            RAISE_INTERNAL_ERROR;
        }
    }

    if (instruction_index < context->data_flow_analysis->incoming_index) {
        data_flow_analysis_forward_transfer_block(instruction_index, block_id);
    }
    else if (instruction_index != context->data_flow_analysis->incoming_index) {
        RAISE_INTERNAL_ERROR;
    }

    return data_flow_analysis_after_meet_block(block_id);
}
#endif

static bool data_flow_analysis_backward_meet_block(size_t block_id) {
    size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_back_index + 1;
    while (instruction_index-- > GET_CFG_BLOCK(block_id).instructions_front_index) {
        if (GET_INSTRUCTION(instruction_index)
            && is_transfer_instruction(instruction_index
#if __OPTIM_LEVEL__ == 1
                ,
                true
#endif
                )) {
            goto Lelse;
        }
    }
    instruction_index = context->data_flow_analysis->incoming_index;
Lelse:
    for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
        GET_DFA_INSTRUCTION_SET_MASK(instruction_index, i) = MASK_FALSE;
    }

    for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
        if (successor_id < context->control_flow_graph->exit_id) {
            for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
                GET_DFA_INSTRUCTION_SET_MASK(instruction_index, i) |= GET_DFA_BLOCK_SET_MASK(successor_id, i);
            }
        }
        else if (successor_id == context->control_flow_graph->exit_id) {
            for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
                GET_DFA_INSTRUCTION_SET_MASK(instruction_index, i) =
                    GET_DFA_INSTRUCTION_SET_MASK(context->data_flow_analysis->static_index, i);
            }
            break;
        }
        else {
            RAISE_INTERNAL_ERROR;
        }
    }

    if (instruction_index < context->data_flow_analysis->incoming_index) {
        data_flow_analysis_backward_transfer_block(instruction_index, block_id);
    }
    else if (instruction_index != context->data_flow_analysis->incoming_index) {
        RAISE_INTERNAL_ERROR;
    }

    return data_flow_analysis_after_meet_block(block_id);
}

#if __OPTIM_LEVEL__ == 1
static void data_flow_analysis_forward_iterative_algorithm() {
    size_t open_block_ids_size = context->control_flow_graph->blocks.size();
    for (size_t i = 0; i < open_block_ids_size; ++i) {
        size_t block_id = context->data_flow_analysis->open_block_ids[i];
        if (block_id == context->control_flow_graph->exit_id) {
            continue;
        }

        bool is_fixed_point = data_flow_analysis_forward_meet_block(block_id);
        if (!is_fixed_point) {
            for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
                if (successor_id < context->control_flow_graph->exit_id) {
                    for (size_t j = i + 1; j < open_block_ids_size; ++j) {
                        if (successor_id == context->data_flow_analysis->open_block_ids[j]) {
                            goto Lelse;
                        }
                    }
                    if (open_block_ids_size < context->data_flow_analysis->open_block_ids.size()) {
                        context->data_flow_analysis->open_block_ids[open_block_ids_size] = successor_id;
                    }
                    else {
                        context->data_flow_analysis->open_block_ids.push_back(successor_id);
                    }
                    open_block_ids_size++;
                Lelse:;
                }
                else if (successor_id != context->control_flow_graph->exit_id) {
                    RAISE_INTERNAL_ERROR;
                }
            }
        }
    }
}
#endif

static void data_flow_analysis_backward_iterative_algorithm() {
    size_t open_block_ids_size = context->control_flow_graph->blocks.size();
    for (size_t i = 0; i < open_block_ids_size; ++i) {
        size_t block_id = context->data_flow_analysis->open_block_ids[i];
        if (block_id == context->control_flow_graph->exit_id) {
            continue;
        }

        bool is_fixed_point = data_flow_analysis_backward_meet_block(block_id);
        if (!is_fixed_point) {
            for (size_t predecessor_id : GET_CFG_BLOCK(block_id).predecessor_ids) {
                if (predecessor_id < context->control_flow_graph->exit_id) {
                    for (size_t j = i + 1; j < open_block_ids_size; ++j) {
                        if (predecessor_id == context->data_flow_analysis->open_block_ids[j]) {
                            goto Lelse;
                        }
                    }
                    if (open_block_ids_size < context->data_flow_analysis->open_block_ids.size()) {
                        context->data_flow_analysis->open_block_ids[open_block_ids_size] = predecessor_id;
                    }
                    else {
                        context->data_flow_analysis->open_block_ids.push_back(predecessor_id);
                    }
                    open_block_ids_size++;
                Lelse:;
                }
                else if (predecessor_id != context->control_flow_graph->entry_id) {
                    RAISE_INTERNAL_ERROR;
                }
            }
        }
    }
}

#if __OPTIM_LEVEL__ == 1
static void data_flow_analysis_forward_open_block(size_t block_id, size_t& i);
#endif
static void data_flow_analysis_backward_open_block(size_t block_id, size_t& i);

#if __OPTIM_LEVEL__ == 1
static void data_flow_analysis_forward_successor_open_block(size_t block_id, size_t& i) {
    for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
        data_flow_analysis_forward_open_block(successor_id, i);
    }
}
#endif

static void data_flow_analysis_backward_successor_open_block(size_t block_id, size_t& i) {
    for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
        data_flow_analysis_backward_open_block(successor_id, i);
    }
}

#if __OPTIM_LEVEL__ == 1
static void data_flow_analysis_forward_open_block(size_t block_id, size_t& i) {
    if (block_id < context->control_flow_graph->exit_id && !context->control_flow_graph->reaching_code[block_id]) {
        context->control_flow_graph->reaching_code[block_id] = true;
        data_flow_analysis_forward_successor_open_block(block_id, i);
        i--;
        context->data_flow_analysis->open_block_ids[i] = block_id;
    }
}
#endif

static void data_flow_analysis_backward_open_block(size_t block_id, size_t& i) {
    if (block_id < context->control_flow_graph->exit_id && !context->control_flow_graph->reaching_code[block_id]) {
        context->control_flow_graph->reaching_code[block_id] = true;
        data_flow_analysis_backward_successor_open_block(block_id, i);
        context->data_flow_analysis->open_block_ids[i] = block_id;
        i++;
    }
}

#if __OPTIM_LEVEL__ == 1
static void data_flow_analysis_add_alias_value(TacValue* node) {
    if (node->type() == AST_T::TacVariable_t) {
        context->data_flow_analysis->alias_set.insert(static_cast<TacVariable*>(node)->name);
    }
}

static bool is_same_value(TacValue* node_1, TacValue* node_2);

static bool propagate_copies_add_data_index(TacCopy* node, size_t instruction_index, size_t block_id) {
    if (node->dst->type() != AST_T::TacVariable_t) {
        RAISE_INTERNAL_ERROR;
    }
    else if (is_same_value(node->src.get(), node->dst.get())) {
        control_flow_graph_remove_block_instruction(instruction_index, block_id);
        return false;
    }
    else {
        if (context->data_flow_analysis->set_size < context->data_flow_analysis->data_index_map.size()) {
            context->data_flow_analysis->data_index_map[context->data_flow_analysis->set_size] = instruction_index;
        }
        else {
            context->data_flow_analysis->data_index_map.push_back(instruction_index);
        }
        context->data_flow_analysis->set_size++;
        return true;
    }
}

static void eliminate_dead_store_add_data_name(TIdentifier name) {
    if (context->control_flow_graph->identifier_id_map.find(name)
        == context->control_flow_graph->identifier_id_map.end()) {
        context->control_flow_graph->identifier_id_map[name] = context->data_flow_analysis->set_size;
        context->data_flow_analysis->set_size++;
    }
}

static void eliminate_dead_store_add_data_value(TacValue* node) {
    if (node->type() == AST_T::TacVariable_t) {
        eliminate_dead_store_add_data_name(static_cast<TacVariable*>(node)->name);
    }
}
#elif __OPTIM_LEVEL__ == 2
static void inference_graph_add_data_name(TIdentifier name) {
    if (frontend->symbol_table[name]->attrs->type() != AST_T::StaticAttr_t
        && context->control_flow_graph->identifier_id_map.find(name)
               == context->control_flow_graph->identifier_id_map.end()) {
        context->control_flow_graph->identifier_id_map[name] =
            REGISTER_MASK_SIZE + context->data_flow_analysis->set_size;
        context->data_flow_analysis->set_size++;
    }
}

static void inference_graph_add_data_operand(AsmOperand* node) {
    if (node->type() == AST_T::AsmPseudo_t) {
        inference_graph_add_data_name(static_cast<AsmPseudo*>(node)->name);
    }
}
#endif

static bool data_flow_analysis_initialize(
#if __OPTIM_LEVEL__ == 1
    bool is_dead_store_elimination, bool init_alias_set
#endif
) {
    context->data_flow_analysis->set_size = 0;
    context->data_flow_analysis->incoming_index = context->p_instructions->size();

    if (context->data_flow_analysis->open_block_ids.size() < context->control_flow_graph->blocks.size()) {
        context->data_flow_analysis->open_block_ids.resize(context->control_flow_graph->blocks.size());
    }
    if (context->data_flow_analysis->instruction_index_map.size() < context->p_instructions->size() + 3) {
        context->data_flow_analysis->instruction_index_map.resize(context->p_instructions->size() + 3);
    }
    if (context->control_flow_graph->reaching_code.size() < context->control_flow_graph->blocks.size()) {
        context->control_flow_graph->reaching_code.resize(context->control_flow_graph->blocks.size());
    }
    std::fill(context->control_flow_graph->reaching_code.begin(),
        context->control_flow_graph->reaching_code.begin() + context->control_flow_graph->blocks.size(), false);

    size_t instructions_mask_sets_size = 0;
#if __OPTIM_LEVEL__ == 1
    bool is_copy_propagation = !is_dead_store_elimination;
    if (is_dead_store_elimination) {
#endif
        context->control_flow_graph->identifier_id_map.clear();
        context->data_flow_analysis->static_index = context->data_flow_analysis->incoming_index + 1;
        context->data_flow_analysis->addressed_index = context->data_flow_analysis->static_index + 1;
#if __OPTIM_LEVEL__ == 1
    }
    if (init_alias_set) {
#endif
        context->data_flow_analysis->alias_set.clear();
#if __OPTIM_LEVEL__ == 1
    }
#endif
    for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
        if (GET_CFG_BLOCK(block_id).size > 0) {
            for (size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
                 instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
                if (GET_INSTRUCTION(instruction_index)) {
                    AstInstruction* node = GET_INSTRUCTION(instruction_index).get();
                    switch (node->type()) {
#if __OPTIM_LEVEL__ == 1
                        case AST_T::TacReturn_t: {
                            if (is_copy_propagation) {
                                goto Lcontinue;
                            }
                            TacReturn* p_node = static_cast<TacReturn*>(node);
                            if (p_node->val) {
                                eliminate_dead_store_add_data_value(p_node->val.get());
                            }
                            break;
                        }
                        case AST_T::TacSignExtend_t: {
                            if (is_dead_store_elimination) {
                                TacSignExtend* p_node = static_cast<TacSignExtend*>(node);
                                eliminate_dead_store_add_data_value(p_node->src.get());
                                eliminate_dead_store_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_T::TacTruncate_t: {
                            if (is_dead_store_elimination) {
                                TacTruncate* p_node = static_cast<TacTruncate*>(node);
                                eliminate_dead_store_add_data_value(p_node->src.get());
                                eliminate_dead_store_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_T::TacZeroExtend_t: {
                            if (is_dead_store_elimination) {
                                TacZeroExtend* p_node = static_cast<TacZeroExtend*>(node);
                                eliminate_dead_store_add_data_value(p_node->src.get());
                                eliminate_dead_store_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_T::TacDoubleToInt_t: {
                            if (is_dead_store_elimination) {
                                TacDoubleToInt* p_node = static_cast<TacDoubleToInt*>(node);
                                eliminate_dead_store_add_data_value(p_node->src.get());
                                eliminate_dead_store_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_T::TacDoubleToUInt_t: {
                            if (is_dead_store_elimination) {
                                TacDoubleToUInt* p_node = static_cast<TacDoubleToUInt*>(node);
                                eliminate_dead_store_add_data_value(p_node->src.get());
                                eliminate_dead_store_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_T::TacIntToDouble_t: {
                            if (is_dead_store_elimination) {
                                TacIntToDouble* p_node = static_cast<TacIntToDouble*>(node);
                                eliminate_dead_store_add_data_value(p_node->src.get());
                                eliminate_dead_store_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_T::TacUIntToDouble_t: {
                            if (is_dead_store_elimination) {
                                TacUIntToDouble* p_node = static_cast<TacUIntToDouble*>(node);
                                eliminate_dead_store_add_data_value(p_node->src.get());
                                eliminate_dead_store_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_T::TacFunCall_t: {
                            if (is_dead_store_elimination) {
                                TacFunCall* p_node = static_cast<TacFunCall*>(node);
                                for (const auto& arg : p_node->args) {
                                    eliminate_dead_store_add_data_value(arg.get());
                                }
                                if (p_node->dst) {
                                    eliminate_dead_store_add_data_value(p_node->dst.get());
                                }
                            }
                            break;
                        }
                        case AST_T::TacUnary_t: {
                            if (is_dead_store_elimination) {
                                TacUnary* p_node = static_cast<TacUnary*>(node);
                                eliminate_dead_store_add_data_value(p_node->src.get());
                                eliminate_dead_store_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_T::TacBinary_t: {
                            if (is_dead_store_elimination) {
                                TacBinary* p_node = static_cast<TacBinary*>(node);
                                eliminate_dead_store_add_data_value(p_node->src1.get());
                                eliminate_dead_store_add_data_value(p_node->src2.get());
                                eliminate_dead_store_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_T::TacCopy_t: {
                            TacCopy* p_node = static_cast<TacCopy*>(node);
                            if (is_copy_propagation) {
                                if (!propagate_copies_add_data_index(p_node, instruction_index, block_id)) {
                                    goto Lcontinue;
                                }
                            }
                            else {
                                eliminate_dead_store_add_data_value(p_node->src.get());
                                eliminate_dead_store_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_T::TacGetAddress_t: {
                            TacGetAddress* p_node = static_cast<TacGetAddress*>(node);
                            if (is_dead_store_elimination) {
                                eliminate_dead_store_add_data_value(p_node->src.get());
                                eliminate_dead_store_add_data_value(p_node->dst.get());
                            }
                            if (init_alias_set) {
                                data_flow_analysis_add_alias_value(p_node->src.get());
                            }
                            break;
                        }
                        case AST_T::TacLoad_t: {
                            if (is_dead_store_elimination) {
                                TacLoad* p_node = static_cast<TacLoad*>(node);
                                eliminate_dead_store_add_data_value(p_node->src_ptr.get());
                                eliminate_dead_store_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_T::TacStore_t: {
                            if (is_dead_store_elimination) {
                                TacStore* p_node = static_cast<TacStore*>(node);
                                eliminate_dead_store_add_data_value(p_node->src.get());
                                eliminate_dead_store_add_data_value(p_node->dst_ptr.get());
                            }
                            break;
                        }
                        case AST_T::TacAddPtr_t: {
                            if (is_dead_store_elimination) {
                                TacAddPtr* p_node = static_cast<TacAddPtr*>(node);
                                eliminate_dead_store_add_data_value(p_node->src_ptr.get());
                                eliminate_dead_store_add_data_value(p_node->index.get());
                                eliminate_dead_store_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_T::TacCopyToOffset_t: {
                            if (is_dead_store_elimination) {
                                TacCopyToOffset* p_node = static_cast<TacCopyToOffset*>(node);
                                eliminate_dead_store_add_data_name(p_node->dst_name);
                                eliminate_dead_store_add_data_value(p_node->src.get());
                            }
                            break;
                        }
                        case AST_T::TacCopyFromOffset_t: {
                            if (is_dead_store_elimination) {
                                TacCopyFromOffset* p_node = static_cast<TacCopyFromOffset*>(node);
                                eliminate_dead_store_add_data_name(p_node->src_name);
                                eliminate_dead_store_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_T::TacJumpIfZero_t: {
                            if (is_copy_propagation) {
                                goto Lcontinue;
                            }
                            eliminate_dead_store_add_data_value(static_cast<TacJumpIfZero*>(node)->condition.get());
                            break;
                        }
                        case AST_T::TacJumpIfNotZero_t: {
                            if (is_copy_propagation) {
                                goto Lcontinue;
                            }
                            eliminate_dead_store_add_data_value(static_cast<TacJumpIfNotZero*>(node)->condition.get());
                            break;
                        }
#elif __OPTIM_LEVEL__ == 2
                        case AST_T::AsmMov_t: {
                            AsmMov* p_node = static_cast<AsmMov*>(node);
                            inference_graph_add_data_operand(p_node->src.get());
                            inference_graph_add_data_operand(p_node->dst.get());
                            break;
                        }
                        case AST_T::AsmUnary_t:
                            inference_graph_add_data_operand(static_cast<AsmUnary*>(node)->dst.get());
                            break;
                        case AST_T::AsmBinary_t: {
                            AsmBinary* p_node = static_cast<AsmBinary*>(node);
                            inference_graph_add_data_operand(p_node->src.get());
                            inference_graph_add_data_operand(p_node->dst.get());
                            break;
                        }
                        case AST_T::AsmCmp_t: {
                            AsmCmp* p_node = static_cast<AsmCmp*>(node);
                            inference_graph_add_data_operand(p_node->src.get());
                            inference_graph_add_data_operand(p_node->dst.get());
                            break;
                        }
                        case AST_T::AsmIdiv_t:
                            inference_graph_add_data_operand(static_cast<AsmIdiv*>(node)->src.get());
                            break;
                        case AST_T::AsmSetCC_t:
                            inference_graph_add_data_operand(static_cast<AsmSetCC*>(node)->dst.get());
                            break;
                        case AST_T::AsmPush_t:
                            inference_graph_add_data_operand(static_cast<AsmPush*>(node)->src.get());
                            break;
                        case AST_T::AsmCdq_t:
                        case AST_T::AsmCall_t:
                            break;
#endif
                        default:
                            goto Lcontinue;
                    }
                    context->data_flow_analysis->instruction_index_map[instruction_index] = instructions_mask_sets_size;
                    instructions_mask_sets_size++;
                Lcontinue:;
                }
            }
        }
        else {
            context->control_flow_graph->reaching_code[block_id] = true;
        }
    }
    if (context->data_flow_analysis->set_size == 0) {
        return false;
    }

#if __OPTIM_LEVEL__ == 2
    if (context->data_flow_analysis->data_name_map.size() < context->data_flow_analysis->set_size) {
        context->data_flow_analysis->data_name_map.resize(context->data_flow_analysis->set_size);
    }
    context->data_flow_analysis->set_size += REGISTER_MASK_SIZE;
#endif

    context->data_flow_analysis->instruction_index_map[context->data_flow_analysis->incoming_index] =
        instructions_mask_sets_size;
    instructions_mask_sets_size++;
#if __OPTIM_LEVEL__ == 1
    if (is_dead_store_elimination) {
#endif
        context->data_flow_analysis->instruction_index_map[context->data_flow_analysis->static_index] =
            instructions_mask_sets_size;
        instructions_mask_sets_size++;
        context->data_flow_analysis->instruction_index_map[context->data_flow_analysis->addressed_index] =
            instructions_mask_sets_size;
        instructions_mask_sets_size++;
#if __OPTIM_LEVEL__ == 1
    }
#endif
    context->data_flow_analysis->mask_size = (context->data_flow_analysis->set_size + 63) / 64;
    instructions_mask_sets_size *= context->data_flow_analysis->mask_size;
    size_t blocks_mask_sets_size = context->data_flow_analysis->mask_size * context->control_flow_graph->blocks.size();

    if (context->data_flow_analysis->blocks_mask_sets.size() < blocks_mask_sets_size) {
        context->data_flow_analysis->blocks_mask_sets.resize(blocks_mask_sets_size);
    }
    if (context->data_flow_analysis->instructions_mask_sets.size() < instructions_mask_sets_size) {
        context->data_flow_analysis->instructions_mask_sets.resize(instructions_mask_sets_size);
    }

#if __OPTIM_LEVEL__ == 1
    if (is_copy_propagation) {
        size_t i = context->control_flow_graph->blocks.size();
        for (size_t successor_id : context->control_flow_graph->entry_successor_ids) {
            if (!context->control_flow_graph->reaching_code[successor_id]) {
                data_flow_analysis_forward_open_block(successor_id, i);
            }
        }
        while (i-- > 0) {
            context->data_flow_analysis->open_block_ids[i] = context->control_flow_graph->exit_id;
        }

        mask_t mask_true_back = MASK_TRUE;
        i = context->data_flow_analysis->set_size - (context->data_flow_analysis->mask_size - 1) * 64;
        if (i > 0) {
            for (; i < 64; ++i) {
                mask_set(mask_true_back, i, false);
            }
        }

        if (context->control_flow_graph->reaching_code.size() < context->data_flow_analysis->set_size) {
            context->control_flow_graph->reaching_code.resize(context->data_flow_analysis->set_size);
        }
        if (context->data_flow_analysis->bak_instructions.size() < context->data_flow_analysis->set_size) {
            context->data_flow_analysis->bak_instructions.resize(context->data_flow_analysis->set_size);
        }

        std::fill(context->control_flow_graph->reaching_code.begin(),
            context->control_flow_graph->reaching_code.begin() + context->data_flow_analysis->set_size, false);

        if (context->data_flow_analysis->mask_size > 1) {
            i = 0;
            do {
                for (size_t j = context->data_flow_analysis->mask_size - 1; j-- > 0;) {
                    context->data_flow_analysis->blocks_mask_sets[i] = MASK_TRUE;
                    i++;
                }
                context->data_flow_analysis->blocks_mask_sets[i] = mask_true_back;
                i++;
            }
            while (i < blocks_mask_sets_size);
        }
        else {
            std::fill(context->data_flow_analysis->blocks_mask_sets.begin(),
                context->data_flow_analysis->blocks_mask_sets.begin() + blocks_mask_sets_size, mask_true_back);
        }
    }
    else {
#endif
        size_t i = 0;
        for (size_t successor_id : context->control_flow_graph->entry_successor_ids) {
            if (!context->control_flow_graph->reaching_code[successor_id]) {
                data_flow_analysis_backward_open_block(successor_id, i);
            }
        }
        for (; i < context->control_flow_graph->blocks.size(); i++) {
            context->data_flow_analysis->open_block_ids[i] = context->control_flow_graph->exit_id;
        }

        for (i = 0; i < context->data_flow_analysis->mask_size; ++i) {
            GET_DFA_INSTRUCTION_SET_MASK(context->data_flow_analysis->static_index, i) = MASK_FALSE;
            GET_DFA_INSTRUCTION_SET_MASK(context->data_flow_analysis->addressed_index, i) = MASK_FALSE;
        }

        for (const auto& name_id : context->control_flow_graph->identifier_id_map) {
#if __OPTIM_LEVEL__ == 1
            if (frontend->symbol_table[name_id.first]->attrs->type() == AST_T::StaticAttr_t) {
                SET_DFA_INSTRUCTION_SET_AT(context->data_flow_analysis->static_index, name_id.second, true);
            }
#elif __OPTIM_LEVEL__ == 2
        context->data_flow_analysis->data_name_map[name_id.second - REGISTER_MASK_SIZE] = name_id.first;
#endif
            if (context->data_flow_analysis->alias_set.find(name_id.first)
                != context->data_flow_analysis->alias_set.end()) {
                SET_DFA_INSTRUCTION_SET_AT(context->data_flow_analysis->addressed_index, name_id.second, true);
            }
        }

#if __OPTIM_LEVEL__ == 2
        SET_DFA_INSTRUCTION_SET_AT(
            context->data_flow_analysis->static_index, register_mask_bit(REGISTER_KIND::Ax), true);
#endif

        std::fill(context->data_flow_analysis->blocks_mask_sets.begin(),
            context->data_flow_analysis->blocks_mask_sets.begin() + blocks_mask_sets_size, MASK_FALSE);
#if __OPTIM_LEVEL__ == 1
    }
#endif

    return true;
}

// // TODO rm
// #include <stdio.h>
// static void print_data_flow_analysis(bool is_dead_store_elimination) {
//     size_t blocks_mask_sets_size = 0;
//     size_t instructions_mask_sets_size = 0;
//     for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
//         if (GET_CFG_BLOCK(block_id).size > 0) {
//             blocks_mask_sets_size++;
//             for (size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
//                  instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
//                 if (GET_INSTRUCTION(instruction_index)) {
//                     switch (GET_INSTRUCTION(instruction_index)->type()) {
//                         case AST_T::TacSignExtend_t:
//                         case AST_T::TacTruncate_t:
//                         case AST_T::TacZeroExtend_t:
//                         case AST_T::TacDoubleToInt_t:
//                         case AST_T::TacDoubleToUInt_t:
//                         case AST_T::TacIntToDouble_t:
//                         case AST_T::TacUIntToDouble_t:
//                         case AST_T::TacFunCall_t:
//                         case AST_T::TacUnary_t:
//                         case AST_T::TacBinary_t:
//                         case AST_T::TacCopy_t:
//                         case AST_T::TacGetAddress_t:
//                         case AST_T::TacLoad_t:
//                         case AST_T::TacStore_t:
//                         case AST_T::TacAddPtr_t:
//                         case AST_T::TacCopyToOffset_t:
//                         case AST_T::TacCopyFromOffset_t:
//                             instructions_mask_sets_size++;
//                             break;
//                         case AST_T::TacReturn_t:
//                         case AST_T::TacJumpIfZero_t:
//                         case AST_T::TacJumpIfNotZero_t: {
//                             if (is_dead_store_elimination) {
//                                 instructions_mask_sets_size++;
//                             }
//                             break;
//                         }
//                         default:
//                             break;
//                     }
//                 }
//             }
//         }
//     }

//     printf("\n\n----------------------------------------\nDataFlowAnalysis:\n");
//     printf("blocks_mask_sets_size: (%zu, %zu)\n", blocks_mask_sets_size, context->data_flow_analysis->set_size);
//     for (size_t i = 0; i < blocks_mask_sets_size; ++i) {
//         for (size_t j = 0; j < context->data_flow_analysis->set_size; ++j) {
//             printf("%i ", mask_get(context->data_flow_analysis
//                                        ->blocks_mask_sets[i * context->data_flow_analysis->mask_size +
//                                        MASK_OFFSET(j)],
//                               j) ?
//                               1 :
//                               0);
//         }
//         printf("\n");
//     }
//     printf("instructions_mask_sets_size: (%zu, %zu)\n", instructions_mask_sets_size,
//         context->data_flow_analysis->set_size);
//     for (size_t i = 0; i < instructions_mask_sets_size; ++i) {
//         for (size_t j = 0; j < context->data_flow_analysis->set_size; ++j) {
//             printf("%i ",
//                 mask_get(context->data_flow_analysis
//                              ->instructions_mask_sets[i * context->data_flow_analysis->mask_size + MASK_OFFSET(j)],
//                     j) ?
//                     1 :
//                     0);
//         }
//         printf("\n");
//     }
// }

#endif
#endif
#endif
