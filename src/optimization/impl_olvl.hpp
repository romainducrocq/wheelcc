#ifndef _OPTIMIZATION_IMPL_OLVL_H
#define _OPTIMIZATION_IMPL_OLVL_H

#ifdef __OPTIM_LEVEL__
#if __OPTIM_LEVEL__ >= 1 && __OPTIM_LEVEL__ <= 2

#define GET_INSTR(X) (*context->p_instructions)[X]
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
    size_t static_index;
    std::vector<size_t> open_data_map;
    std::vector<size_t> instruction_index_map;
    std::vector<mask_t> blocks_mask_sets;
    std::vector<mask_t> instructions_mask_sets;
};

#if __OPTIM_LEVEL__ == 1
struct DataFlowAnalysisO1 {
    // Copy propagation
    std::vector<size_t> data_index_map;
    std::vector<std::unique_ptr<TacInstruction>> bak_instructions;
    // Dead store elimination
    size_t addressed_index;
};
#elif __OPTIM_LEVEL__ == 2
struct DataFlowAnalysisO2 {
    // Register allocation
    std::vector<TIdentifier> data_name_map;
};
#endif

static void set_instr(std::unique_ptr<AstInstruction>&& instruction, size_t instruction_index) {
    if (instruction) {
        GET_INSTR(instruction_index) = std::move(instruction);
    }
    else {
        GET_INSTR(instruction_index).reset();
    }
#if __OPTIM_LEVEL__ == 1
    context->is_fixed_point = false;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Control flow graph

static void cfg_add_edge(std::vector<size_t>& successor_ids, std::vector<size_t>& predecessor_ids, size_t successor_id,
    size_t predecessor_id) {
    if (std::find(successor_ids.begin(), successor_ids.end(), successor_id) == successor_ids.end()) {
        successor_ids.push_back(successor_id);
    }
    if (std::find(predecessor_ids.begin(), predecessor_ids.end(), predecessor_id) == predecessor_ids.end()) {
        predecessor_ids.push_back(predecessor_id);
    }
}

static void cfg_add_succ_edge(size_t block_id, size_t successor_id) {
    if (successor_id < context->control_flow_graph->exit_id) {
        cfg_add_edge(
            GET_CFG_BLOCK(block_id).successor_ids, GET_CFG_BLOCK(successor_id).predecessor_ids, successor_id, block_id);
    }
    else if (successor_id == context->control_flow_graph->exit_id) {
        cfg_add_edge(GET_CFG_BLOCK(block_id).successor_ids, context->control_flow_graph->exit_predecessor_ids,
            successor_id, block_id);
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static void cfg_add_pred_edge(size_t block_id, size_t predecessor_id) {
    if (predecessor_id < context->control_flow_graph->exit_id) {
        cfg_add_edge(GET_CFG_BLOCK(predecessor_id).successor_ids, GET_CFG_BLOCK(block_id).predecessor_ids, block_id,
            predecessor_id);
    }
    else if (predecessor_id == context->control_flow_graph->entry_id) {
        cfg_add_edge(context->control_flow_graph->entry_successor_ids, GET_CFG_BLOCK(block_id).predecessor_ids,
            block_id, predecessor_id);
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static void cfg_rm_edge(std::vector<size_t>& successor_ids, std::vector<size_t>& predecessor_ids, size_t successor_id,
    size_t predecessor_id, bool is_reachable) {
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

static void cfg_rm_succ_edge(size_t block_id, size_t successor_id, bool is_reachable) {
    if (successor_id < context->control_flow_graph->exit_id) {
        cfg_rm_edge(GET_CFG_BLOCK(block_id).successor_ids, GET_CFG_BLOCK(successor_id).predecessor_ids, successor_id,
            block_id, is_reachable);
    }
    else if (successor_id == context->control_flow_graph->exit_id) {
        cfg_rm_edge(GET_CFG_BLOCK(block_id).successor_ids, context->control_flow_graph->exit_predecessor_ids,
            successor_id, block_id, is_reachable);
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static void cfg_rm_pred_edge(size_t block_id, size_t predecessor_id) {
    if (predecessor_id < context->control_flow_graph->exit_id) {
        cfg_rm_edge(GET_CFG_BLOCK(predecessor_id).successor_ids, GET_CFG_BLOCK(block_id).predecessor_ids, block_id,
            predecessor_id, true);
    }
    else if (predecessor_id == context->control_flow_graph->entry_id) {
        cfg_rm_edge(context->control_flow_graph->entry_successor_ids, GET_CFG_BLOCK(block_id).predecessor_ids, block_id,
            predecessor_id, true);
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static void cfg_rm_empty_block(size_t block_id, bool is_reachable) {
    for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
        if (is_reachable) {
            for (size_t predecessor_id : GET_CFG_BLOCK(block_id).predecessor_ids) {
                if (predecessor_id == context->control_flow_graph->entry_id) {
                    cfg_add_pred_edge(successor_id, predecessor_id);
                }
                else {
                    cfg_add_succ_edge(predecessor_id, successor_id);
                }
            }
        }
        cfg_rm_succ_edge(block_id, successor_id, is_reachable);
    }
    if (is_reachable) {
        for (size_t predecessor_id : GET_CFG_BLOCK(block_id).predecessor_ids) {
            cfg_rm_pred_edge(block_id, predecessor_id);
        }
    }
    GET_CFG_BLOCK(block_id).instructions_front_index = context->control_flow_graph->exit_id;
    GET_CFG_BLOCK(block_id).instructions_back_index = context->control_flow_graph->exit_id;
}

static void cfg_rm_block_instr(size_t instruction_index, size_t block_id) {
    if (GET_INSTR(instruction_index)) {
        set_instr(nullptr, instruction_index);
        GET_CFG_BLOCK(block_id).size--;
        if (GET_CFG_BLOCK(block_id).size == 0) {
            cfg_rm_empty_block(block_id, true);
        }
        else if (instruction_index == GET_CFG_BLOCK(block_id).instructions_front_index) {
            for (; instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
                if (GET_INSTR(instruction_index)) {
                    GET_CFG_BLOCK(block_id).instructions_front_index = instruction_index;
                    break;
                }
            }
        }
        else if (instruction_index == GET_CFG_BLOCK(block_id).instructions_back_index) {
            instruction_index++;
            for (; instruction_index-- > GET_CFG_BLOCK(block_id).instructions_front_index;) {
                if (GET_INSTR(instruction_index)) {
                    GET_CFG_BLOCK(block_id).instructions_back_index = instruction_index;
                    break;
                }
            }
        }
    }
}

#if __OPTIM_LEVEL__ == 1
static void cfg_init_label_block(TacLabel* node) {
    context->control_flow_graph->identifier_id_map[node->name] = context->control_flow_graph->blocks.size() - 1;
}
#elif __OPTIM_LEVEL__ == 2
static void cfg_init_label_block(AsmLabel* node) {
    context->control_flow_graph->identifier_id_map[node->name] = context->control_flow_graph->blocks.size() - 1;
}
#endif

static void cfg_init_block(size_t instruction_index, size_t& instructions_back_index) {
    AstInstruction* node = GET_INSTR(instruction_index).get();
    switch (node->type()) {
#if __OPTIM_LEVEL__ == 1
        case AST_TacLabel_t:
#elif __OPTIM_LEVEL__ == 2
        case AST_AsmLabel_t:
#endif
        {
            if (instructions_back_index != context->p_instructions->size()) {
                context->control_flow_graph->blocks.back().instructions_back_index = instructions_back_index;
                ControlFlowBlock block = {0, instruction_index, 0, {}, {}};
                context->control_flow_graph->blocks.emplace_back(std::move(block));
            }
#if __OPTIM_LEVEL__ == 1
            cfg_init_label_block(static_cast<TacLabel*>(node));
#elif __OPTIM_LEVEL__ == 2
            cfg_init_label_block(static_cast<AsmLabel*>(node));
#endif
            instructions_back_index = instruction_index;
            break;
        }
#if __OPTIM_LEVEL__ == 1
        case AST_TacReturn_t:
        case AST_TacJump_t:
        case AST_TacJumpIfZero_t:
        case AST_TacJumpIfNotZero_t:
#elif __OPTIM_LEVEL__ == 2
        case AST_AsmJmp_t:
        case AST_AsmJmpCC_t:
        case AST_AsmRet_t:
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
static void cfg_init_jump_edges(TacJump* node, size_t block_id) {
    cfg_add_succ_edge(block_id, context->control_flow_graph->identifier_id_map[node->target]);
}

static void cfg_init_jmp_eq_0_edges(TacJumpIfZero* node, size_t block_id) {
    cfg_add_succ_edge(block_id, context->control_flow_graph->identifier_id_map[node->target]);
    cfg_add_succ_edge(block_id, block_id + 1);
}

static void cfg_init_jmp_ne_0_edges(TacJumpIfNotZero* node, size_t block_id) {
    cfg_add_succ_edge(block_id, context->control_flow_graph->identifier_id_map[node->target]);
    cfg_add_succ_edge(block_id, block_id + 1);
}
#elif __OPTIM_LEVEL__ == 2
static void cfg_init_jmp_edges(AsmJmp* node, size_t block_id) {
    cfg_add_succ_edge(block_id, context->control_flow_graph->identifier_id_map[node->target]);
}

static void cfg_init_jmp_cc_edges(AsmJmpCC* node, size_t block_id) {
    cfg_add_succ_edge(block_id, context->control_flow_graph->identifier_id_map[node->target]);
    cfg_add_succ_edge(block_id, block_id + 1);
}
#endif

static void cfg_init_edges(size_t block_id) {
    AstInstruction* node = GET_INSTR(GET_CFG_BLOCK(block_id).instructions_back_index).get();
    switch (node->type()) {
#if __OPTIM_LEVEL__ == 1
        case AST_TacReturn_t:
#elif __OPTIM_LEVEL__ == 2
        case AST_AsmRet_t:
#endif
            cfg_add_succ_edge(block_id, context->control_flow_graph->exit_id);
            break;
#if __OPTIM_LEVEL__ == 1
        case AST_TacJump_t:
            cfg_init_jump_edges(static_cast<TacJump*>(node), block_id);
            break;
        case AST_TacJumpIfZero_t:
            cfg_init_jmp_eq_0_edges(static_cast<TacJumpIfZero*>(node), block_id);
            break;
        case AST_TacJumpIfNotZero_t:
            cfg_init_jmp_ne_0_edges(static_cast<TacJumpIfNotZero*>(node), block_id);
            break;
#elif __OPTIM_LEVEL__ == 2
        case AST_AsmJmp_t:
            cfg_init_jmp_edges(static_cast<AsmJmp*>(node), block_id);
            break;
        case AST_AsmJmpCC_t:
            cfg_init_jmp_cc_edges(static_cast<AsmJmpCC*>(node), block_id);
            break;
#endif
        default:
            cfg_add_succ_edge(block_id, block_id + 1);
            break;
    }
}

static void init_control_flow_graph() {
    context->control_flow_graph->blocks.clear();
    context->control_flow_graph->identifier_id_map.clear();
    {
        size_t instructions_back_index = context->p_instructions->size();
        for (size_t instruction_index = 0; instruction_index < context->p_instructions->size(); ++instruction_index) {
            if (GET_INSTR(instruction_index)) {
                if (instructions_back_index == context->p_instructions->size()) {
                    ControlFlowBlock block = {0, instruction_index, 0, {}, {}};
                    context->control_flow_graph->blocks.emplace_back(std::move(block));
                }
                cfg_init_block(instruction_index, instructions_back_index);
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
        cfg_add_pred_edge(0, context->control_flow_graph->entry_id);
        for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
            cfg_init_edges(block_id);
        }
    }
}

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

#define GET_DFA_BLOCK_SET_IDX(X, Y) (X) * context->data_flow_analysis->mask_size + (Y)
#define GET_DFA_INSTR_SET_IDX(X, Y) \
    context->data_flow_analysis->instruction_index_map[X] * context->data_flow_analysis->mask_size + (Y)

#define GET_DFA_BLOCK_SET_MASK(X, Y) context->data_flow_analysis->blocks_mask_sets[GET_DFA_BLOCK_SET_IDX(X, Y)]
#define GET_DFA_INSTR_SET_MASK(X, Y) context->data_flow_analysis->instructions_mask_sets[GET_DFA_INSTR_SET_IDX(X, Y)]

#define GET_DFA_BLOCK_SET_AT(X, Y) mask_get(GET_DFA_BLOCK_SET_MASK(X, MASK_OFFSET(Y)), Y)
#define GET_DFA_INSTR_SET_AT(X, Y) mask_get(GET_DFA_INSTR_SET_MASK(X, MASK_OFFSET(Y)), Y)

#define SET_DFA_INSTR_SET_AT(X, Y, Z) mask_set(GET_DFA_INSTR_SET_MASK(X, MASK_OFFSET(Y)), Y, Z)

#if __OPTIM_LEVEL__ == 1
#define GET_DFA_INSTR(X) GET_INSTR(context->data_flow_analysis_o1->data_index_map[X])
#endif

static bool is_transfer_instr(size_t instruction_index
#if __OPTIM_LEVEL__ == 1
    ,
    bool is_dead_store_elimination
#endif
) {
    switch (GET_INSTR(instruction_index)->type()) {
#if __OPTIM_LEVEL__ == 1
        case AST_TacSignExtend_t:
        case AST_TacTruncate_t:
        case AST_TacZeroExtend_t:
        case AST_TacDoubleToInt_t:
        case AST_TacDoubleToUInt_t:
        case AST_TacIntToDouble_t:
        case AST_TacUIntToDouble_t:
        case AST_TacFunCall_t:
        case AST_TacUnary_t:
        case AST_TacBinary_t:
        case AST_TacCopy_t:
        case AST_TacGetAddress_t:
        case AST_TacLoad_t:
        case AST_TacStore_t:
        case AST_TacAddPtr_t:
        case AST_TacCopyToOffset_t:
        case AST_TacCopyFromOffset_t:
            return true;
        case AST_TacReturn_t:
        case AST_TacJumpIfZero_t:
        case AST_TacJumpIfNotZero_t:
            return is_dead_store_elimination;
#elif __OPTIM_LEVEL__ == 2
        case AST_AsmMov_t:
        case AST_AsmMovSx_t:
        case AST_AsmMovZeroExtend_t:
        case AST_AsmLea_t:
        case AST_AsmCvttsd2si_t:
        case AST_AsmCvtsi2sd_t:
        case AST_AsmUnary_t:
        case AST_AsmBinary_t:
        case AST_AsmCmp_t:
        case AST_AsmIdiv_t:
        case AST_AsmDiv_t:
        case AST_AsmCdq_t:
        case AST_AsmSetCC_t:
        case AST_AsmPush_t:
        case AST_AsmCall_t:
            return true;
#endif
        default:
            return false;
    }
}

#if __OPTIM_LEVEL__ == 1
static size_t get_dfa_data_idx(size_t instruction_index) {
    for (size_t i = 0; i < context->data_flow_analysis->set_size; ++i) {
        if (context->data_flow_analysis_o1->data_index_map[i] == instruction_index) {
            return i;
        }
    }
    RAISE_INTERNAL_ERROR;
}

static TacInstruction* get_dfa_bak_instr(size_t i) {
    if (context->control_flow_graph->reaching_code[i]) {
        if (context->data_flow_analysis_o1->bak_instructions[i]) {
            return context->data_flow_analysis_o1->bak_instructions[i].get();
        }
        else {
            RAISE_INTERNAL_ERROR;
        }
    }
    else if (GET_DFA_INSTR(i)) {
        return GET_DFA_INSTR(i).get();
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static bool set_dfa_bak_instr(size_t instruction_index, size_t& i) {
    i = get_dfa_data_idx(instruction_index);
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
static bool prop_transfer_reach_copies(size_t instruction_index, size_t next_instruction_index);
static void elim_transfer_live_values(size_t instruction_index, size_t next_instruction_index);
#elif __OPTIM_LEVEL__ == 2
static void infer_transfer_live_regs(size_t instruction_index, size_t next_instruction_index);
#endif

#if __OPTIM_LEVEL__ == 1
static size_t dfa_forward_transfer_block(size_t instruction_index, size_t block_id) {
    for (size_t next_instruction_index = instruction_index + 1;
         next_instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++next_instruction_index) {
        if (GET_INSTR(next_instruction_index) && is_transfer_instr(next_instruction_index, false)) {
            for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
                GET_DFA_INSTR_SET_MASK(next_instruction_index, i) = GET_DFA_INSTR_SET_MASK(instruction_index, i);
            }
            if (!prop_transfer_reach_copies(instruction_index, next_instruction_index)) {
                for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
                    GET_DFA_INSTR_SET_MASK(next_instruction_index, i) = GET_DFA_INSTR_SET_MASK(instruction_index, i);
                }
            }
            instruction_index = next_instruction_index;
        }
    }
    for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
        GET_DFA_INSTR_SET_MASK(context->data_flow_analysis->incoming_index, i) =
            GET_DFA_INSTR_SET_MASK(instruction_index, i);
    }
    if (!prop_transfer_reach_copies(instruction_index, context->data_flow_analysis->incoming_index)) {
        for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
            GET_DFA_INSTR_SET_MASK(context->data_flow_analysis->incoming_index, i) =
                GET_DFA_INSTR_SET_MASK(instruction_index, i);
        }
    }
    return instruction_index;
}
#endif

static size_t dfa_backward_transfer_block(size_t instruction_index, size_t block_id) {
    if (instruction_index > 0) {
        for (size_t next_instruction_index = instruction_index;
             next_instruction_index-- > GET_CFG_BLOCK(block_id).instructions_front_index;) {
            if (GET_INSTR(next_instruction_index)
                && is_transfer_instr(next_instruction_index
#if __OPTIM_LEVEL__ == 1
                    ,
                    true
#endif
                    )) {
                for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
                    GET_DFA_INSTR_SET_MASK(next_instruction_index, i) = GET_DFA_INSTR_SET_MASK(instruction_index, i);
                }
#if __OPTIM_LEVEL__ == 1
                elim_transfer_live_values
#elif __OPTIM_LEVEL__ == 2
                infer_transfer_live_regs
#endif
                    (instruction_index, next_instruction_index);
                instruction_index = next_instruction_index;
            }
        }
    }
    for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
        GET_DFA_INSTR_SET_MASK(context->data_flow_analysis->incoming_index, i) =
            GET_DFA_INSTR_SET_MASK(instruction_index, i);
    }
#if __OPTIM_LEVEL__ == 1
    elim_transfer_live_values
#elif __OPTIM_LEVEL__ == 2
    infer_transfer_live_regs
#endif
        (instruction_index, context->data_flow_analysis->incoming_index);
    return instruction_index;
}

static bool dfa_after_meet_block(size_t block_id) {
    bool is_fixed_point = true;
    {
        size_t i = 0;
        for (; i < context->data_flow_analysis->mask_size; ++i) {
            if (GET_DFA_BLOCK_SET_MASK(block_id, i)
                != GET_DFA_INSTR_SET_MASK(context->data_flow_analysis->incoming_index, i)) {
                is_fixed_point = false;
                break;
            }
        }
        for (; i < context->data_flow_analysis->mask_size; ++i) {
            GET_DFA_BLOCK_SET_MASK(block_id, i) =
                GET_DFA_INSTR_SET_MASK(context->data_flow_analysis->incoming_index, i);
        }
    }
    return is_fixed_point;
}

#if __OPTIM_LEVEL__ == 1
static bool dfa_forward_meet_block(size_t block_id) {
    size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
    for (; instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
        if (GET_INSTR(instruction_index) && is_transfer_instr(instruction_index, false)) {
            goto Lelse;
        }
    }
    instruction_index = context->data_flow_analysis->incoming_index;
Lelse:
    for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
        GET_DFA_INSTR_SET_MASK(instruction_index, i) = MASK_TRUE;
    }

    for (size_t predecessor_id : GET_CFG_BLOCK(block_id).predecessor_ids) {
        if (predecessor_id < context->control_flow_graph->exit_id) {
            for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
                GET_DFA_INSTR_SET_MASK(instruction_index, i) &= GET_DFA_BLOCK_SET_MASK(predecessor_id, i);
            }
        }
        else if (predecessor_id == context->control_flow_graph->entry_id) {
            for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
                GET_DFA_INSTR_SET_MASK(instruction_index, i) = MASK_FALSE;
            }
            break;
        }
        else {
            RAISE_INTERNAL_ERROR;
        }
    }

    if (instruction_index < context->data_flow_analysis->incoming_index) {
        dfa_forward_transfer_block(instruction_index, block_id);
    }
    else if (instruction_index != context->data_flow_analysis->incoming_index) {
        RAISE_INTERNAL_ERROR;
    }

    return dfa_after_meet_block(block_id);
}
#endif

static bool dfa_backward_meet_block(size_t block_id) {
    size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_back_index + 1;
    while (instruction_index-- > GET_CFG_BLOCK(block_id).instructions_front_index) {
        if (GET_INSTR(instruction_index)
            && is_transfer_instr(instruction_index
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
        GET_DFA_INSTR_SET_MASK(instruction_index, i) = MASK_FALSE;
    }

    for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
        if (successor_id < context->control_flow_graph->exit_id) {
            for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
                GET_DFA_INSTR_SET_MASK(instruction_index, i) |= GET_DFA_BLOCK_SET_MASK(successor_id, i);
            }
        }
        else if (successor_id == context->control_flow_graph->exit_id) {
            for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
                GET_DFA_INSTR_SET_MASK(instruction_index, i) =
                    GET_DFA_INSTR_SET_MASK(context->data_flow_analysis->static_index, i);
            }
            break;
        }
        else {
            RAISE_INTERNAL_ERROR;
        }
    }

    if (instruction_index < context->data_flow_analysis->incoming_index) {
        dfa_backward_transfer_block(instruction_index, block_id);
    }
    else if (instruction_index != context->data_flow_analysis->incoming_index) {
        RAISE_INTERNAL_ERROR;
    }

    return dfa_after_meet_block(block_id);
}

#if __OPTIM_LEVEL__ == 1
static void dfa_forward_iter_alg() {
    size_t open_data_map_size = context->control_flow_graph->blocks.size();
    for (size_t i = 0; i < open_data_map_size; ++i) {
        size_t block_id = context->data_flow_analysis->open_data_map[i];
        if (block_id == context->control_flow_graph->exit_id) {
            continue;
        }

        bool is_fixed_point = dfa_forward_meet_block(block_id);
        if (!is_fixed_point) {
            for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
                if (successor_id < context->control_flow_graph->exit_id) {
                    for (size_t j = i + 1; j < open_data_map_size; ++j) {
                        if (successor_id == context->data_flow_analysis->open_data_map[j]) {
                            goto Lelse;
                        }
                    }
                    if (open_data_map_size < context->data_flow_analysis->open_data_map.size()) {
                        context->data_flow_analysis->open_data_map[open_data_map_size] = successor_id;
                    }
                    else {
                        context->data_flow_analysis->open_data_map.push_back(successor_id);
                    }
                    open_data_map_size++;
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

static void dfa_iter_alg() {
    size_t open_data_map_size = context->control_flow_graph->blocks.size();
    for (size_t i = 0; i < open_data_map_size; ++i) {
        size_t block_id = context->data_flow_analysis->open_data_map[i];
        if (block_id == context->control_flow_graph->exit_id) {
            continue;
        }

        bool is_fixed_point = dfa_backward_meet_block(block_id);
        if (!is_fixed_point) {
            for (size_t predecessor_id : GET_CFG_BLOCK(block_id).predecessor_ids) {
                if (predecessor_id < context->control_flow_graph->exit_id) {
                    for (size_t j = i + 1; j < open_data_map_size; ++j) {
                        if (predecessor_id == context->data_flow_analysis->open_data_map[j]) {
                            goto Lelse;
                        }
                    }
                    if (open_data_map_size < context->data_flow_analysis->open_data_map.size()) {
                        context->data_flow_analysis->open_data_map[open_data_map_size] = predecessor_id;
                    }
                    else {
                        context->data_flow_analysis->open_data_map.push_back(predecessor_id);
                    }
                    open_data_map_size++;
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
static void dfa_forward_open_block(size_t block_id, size_t& i);
#endif
static void dfa_backward_open_block(size_t block_id, size_t& i);

#if __OPTIM_LEVEL__ == 1
static void dfa_forward_succ_open_block(size_t block_id, size_t& i) {
    for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
        dfa_forward_open_block(successor_id, i);
    }
}
#endif

static void dfa_backward_succ_open_block(size_t block_id, size_t& i) {
    for (size_t successor_id : GET_CFG_BLOCK(block_id).successor_ids) {
        dfa_backward_open_block(successor_id, i);
    }
}

#if __OPTIM_LEVEL__ == 1
static void dfa_forward_open_block(size_t block_id, size_t& i) {
    if (block_id < context->control_flow_graph->exit_id && !context->control_flow_graph->reaching_code[block_id]) {
        context->control_flow_graph->reaching_code[block_id] = true;
        dfa_forward_succ_open_block(block_id, i);
        i--;
        context->data_flow_analysis->open_data_map[i] = block_id;
    }
}
#endif

static void dfa_backward_open_block(size_t block_id, size_t& i) {
    if (block_id < context->control_flow_graph->exit_id && !context->control_flow_graph->reaching_code[block_id]) {
        context->control_flow_graph->reaching_code[block_id] = true;
        dfa_backward_succ_open_block(block_id, i);
        context->data_flow_analysis->open_data_map[i] = block_id;
        i++;
    }
}

static bool is_aliased_name(TIdentifier name) {
    return frontend->symbol_table[name]->attrs->type() == AST_StaticAttr_t
           || frontend->addressed_set.find(name) != frontend->addressed_set.end();
}

#if __OPTIM_LEVEL__ == 1
static void dfa_add_aliased_value(TacValue* node) {
    if (node->type() == AST_TacVariable_t) {
        frontend->addressed_set.insert(static_cast<TacVariable*>(node)->name);
    }
}

static bool is_same_value(TacValue* node_1, TacValue* node_2);

static bool prop_add_data_idx(TacCopy* node, size_t instruction_index, size_t block_id) {
    if (node->dst->type() != AST_TacVariable_t) {
        RAISE_INTERNAL_ERROR;
    }
    else if (is_same_value(node->src.get(), node->dst.get())) {
        cfg_rm_block_instr(instruction_index, block_id);
        return false;
    }
    else {
        if (context->data_flow_analysis->set_size < context->data_flow_analysis_o1->data_index_map.size()) {
            context->data_flow_analysis_o1->data_index_map[context->data_flow_analysis->set_size] = instruction_index;
        }
        else {
            context->data_flow_analysis_o1->data_index_map.push_back(instruction_index);
        }
        context->data_flow_analysis->set_size++;
        return true;
    }
}

static void elim_add_data_name(TIdentifier name) {
    if (context->control_flow_graph->identifier_id_map.find(name)
        == context->control_flow_graph->identifier_id_map.end()) {
        context->control_flow_graph->identifier_id_map[name] = context->data_flow_analysis->set_size;
        context->data_flow_analysis->set_size++;
    }
}

static void elim_add_data_value(TacValue* node) {
    if (node->type() == AST_TacVariable_t) {
        elim_add_data_name(static_cast<TacVariable*>(node)->name);
    }
}
#elif __OPTIM_LEVEL__ == 2
static void infer_add_data_name(TIdentifier name) {
    if (!is_aliased_name(name)
        && context->control_flow_graph->identifier_id_map.find(name)
               == context->control_flow_graph->identifier_id_map.end()) {
        context->control_flow_graph->identifier_id_map[name] =
            REGISTER_MASK_SIZE + context->data_flow_analysis->set_size;
        context->data_flow_analysis->set_size++;
    }
}

static void infer_add_data_op(AsmOperand* node) {
    if (node->type() == AST_AsmPseudo_t) {
        infer_add_data_name(static_cast<AsmPseudo*>(node)->name);
    }
}
#endif

static bool init_data_flow_analysis(
#if __OPTIM_LEVEL__ == 1
    bool is_dead_store_elimination, bool is_addressed_set
#elif __OPTIM_LEVEL__ == 2
    TIdentifier function_name
#endif
) {
    context->data_flow_analysis->set_size = 0;
    context->data_flow_analysis->incoming_index = context->p_instructions->size();

    if (context->data_flow_analysis->open_data_map.size() < context->control_flow_graph->blocks.size()) {
        context->data_flow_analysis->open_data_map.resize(context->control_flow_graph->blocks.size());
    }
    {
        size_t i =
#if __OPTIM_LEVEL__ == 1
            is_dead_store_elimination ? 3 : 1
#elif __OPTIM_LEVEL__ == 2
            2
#endif
            ;
        if (context->data_flow_analysis->instruction_index_map.size() < context->p_instructions->size() + i) {
            context->data_flow_analysis->instruction_index_map.resize(context->p_instructions->size() + i);
        }
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
#if __OPTIM_LEVEL__ == 1
        context->data_flow_analysis_o1->addressed_index = context->data_flow_analysis->static_index + 1;
    }
    if (is_addressed_set) {
        frontend->addressed_set.clear();
    }
#endif
    for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
        if (GET_CFG_BLOCK(block_id).size > 0) {
            for (size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
                 instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
                if (GET_INSTR(instruction_index)) {
                    AstInstruction* node = GET_INSTR(instruction_index).get();
                    switch (node->type()) {
#if __OPTIM_LEVEL__ == 1
                        case AST_TacReturn_t: {
                            if (is_copy_propagation) {
                                goto Lcontinue;
                            }
                            TacReturn* p_node = static_cast<TacReturn*>(node);
                            if (p_node->val) {
                                elim_add_data_value(p_node->val.get());
                            }
                            break;
                        }
                        case AST_TacSignExtend_t: {
                            if (is_dead_store_elimination) {
                                TacSignExtend* p_node = static_cast<TacSignExtend*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacTruncate_t: {
                            if (is_dead_store_elimination) {
                                TacTruncate* p_node = static_cast<TacTruncate*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacZeroExtend_t: {
                            if (is_dead_store_elimination) {
                                TacZeroExtend* p_node = static_cast<TacZeroExtend*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacDoubleToInt_t: {
                            if (is_dead_store_elimination) {
                                TacDoubleToInt* p_node = static_cast<TacDoubleToInt*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacDoubleToUInt_t: {
                            if (is_dead_store_elimination) {
                                TacDoubleToUInt* p_node = static_cast<TacDoubleToUInt*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacIntToDouble_t: {
                            if (is_dead_store_elimination) {
                                TacIntToDouble* p_node = static_cast<TacIntToDouble*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacUIntToDouble_t: {
                            if (is_dead_store_elimination) {
                                TacUIntToDouble* p_node = static_cast<TacUIntToDouble*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacFunCall_t: {
                            if (is_dead_store_elimination) {
                                TacFunCall* p_node = static_cast<TacFunCall*>(node);
                                for (const auto& arg : p_node->args) {
                                    elim_add_data_value(arg.get());
                                }
                                if (p_node->dst) {
                                    elim_add_data_value(p_node->dst.get());
                                }
                            }
                            break;
                        }
                        case AST_TacUnary_t: {
                            if (is_dead_store_elimination) {
                                TacUnary* p_node = static_cast<TacUnary*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacBinary_t: {
                            if (is_dead_store_elimination) {
                                TacBinary* p_node = static_cast<TacBinary*>(node);
                                elim_add_data_value(p_node->src1.get());
                                elim_add_data_value(p_node->src2.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacCopy_t: {
                            TacCopy* p_node = static_cast<TacCopy*>(node);
                            if (is_copy_propagation) {
                                if (!prop_add_data_idx(p_node, instruction_index, block_id)) {
                                    goto Lcontinue;
                                }
                            }
                            else {
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacGetAddress_t: {
                            TacGetAddress* p_node = static_cast<TacGetAddress*>(node);
                            if (is_dead_store_elimination) {
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            if (is_addressed_set) {
                                dfa_add_aliased_value(p_node->src.get());
                            }
                            break;
                        }
                        case AST_TacLoad_t: {
                            if (is_dead_store_elimination) {
                                TacLoad* p_node = static_cast<TacLoad*>(node);
                                elim_add_data_value(p_node->src_ptr.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacStore_t: {
                            if (is_dead_store_elimination) {
                                TacStore* p_node = static_cast<TacStore*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst_ptr.get());
                            }
                            break;
                        }
                        case AST_TacAddPtr_t: {
                            if (is_dead_store_elimination) {
                                TacAddPtr* p_node = static_cast<TacAddPtr*>(node);
                                elim_add_data_value(p_node->src_ptr.get());
                                elim_add_data_value(p_node->index.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacCopyToOffset_t: {
                            if (is_dead_store_elimination) {
                                TacCopyToOffset* p_node = static_cast<TacCopyToOffset*>(node);
                                elim_add_data_name(p_node->dst_name);
                                elim_add_data_value(p_node->src.get());
                            }
                            break;
                        }
                        case AST_TacCopyFromOffset_t: {
                            if (is_dead_store_elimination) {
                                TacCopyFromOffset* p_node = static_cast<TacCopyFromOffset*>(node);
                                elim_add_data_name(p_node->src_name);
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacJumpIfZero_t: {
                            if (is_copy_propagation) {
                                goto Lcontinue;
                            }
                            elim_add_data_value(static_cast<TacJumpIfZero*>(node)->condition.get());
                            break;
                        }
                        case AST_TacJumpIfNotZero_t: {
                            if (is_copy_propagation) {
                                goto Lcontinue;
                            }
                            elim_add_data_value(static_cast<TacJumpIfNotZero*>(node)->condition.get());
                            break;
                        }
#elif __OPTIM_LEVEL__ == 2
                        case AST_AsmMov_t: {
                            AsmMov* p_node = static_cast<AsmMov*>(node);
                            infer_add_data_op(p_node->src.get());
                            infer_add_data_op(p_node->dst.get());
                            break;
                        }
                        case AST_AsmMovSx_t: {
                            AsmMovSx* p_node = static_cast<AsmMovSx*>(node);
                            infer_add_data_op(p_node->src.get());
                            infer_add_data_op(p_node->dst.get());
                            break;
                        }
                        case AST_AsmMovZeroExtend_t: {
                            AsmMovZeroExtend* p_node = static_cast<AsmMovZeroExtend*>(node);
                            infer_add_data_op(p_node->src.get());
                            infer_add_data_op(p_node->dst.get());
                            break;
                        }
                        case AST_AsmLea_t: {
                            AsmLea* p_node = static_cast<AsmLea*>(node);
                            infer_add_data_op(p_node->src.get());
                            infer_add_data_op(p_node->dst.get());
                            break;
                        }
                        case AST_AsmCvttsd2si_t: {
                            AsmCvttsd2si* p_node = static_cast<AsmCvttsd2si*>(node);
                            infer_add_data_op(p_node->src.get());
                            infer_add_data_op(p_node->dst.get());
                            break;
                        }
                        case AST_AsmCvtsi2sd_t: {
                            AsmCvtsi2sd* p_node = static_cast<AsmCvtsi2sd*>(node);
                            infer_add_data_op(p_node->src.get());
                            infer_add_data_op(p_node->dst.get());
                            break;
                        }
                        case AST_AsmUnary_t:
                            infer_add_data_op(static_cast<AsmUnary*>(node)->dst.get());
                            break;
                        case AST_AsmBinary_t: {
                            AsmBinary* p_node = static_cast<AsmBinary*>(node);
                            infer_add_data_op(p_node->src.get());
                            infer_add_data_op(p_node->dst.get());
                            break;
                        }
                        case AST_AsmCmp_t: {
                            AsmCmp* p_node = static_cast<AsmCmp*>(node);
                            infer_add_data_op(p_node->src.get());
                            infer_add_data_op(p_node->dst.get());
                            break;
                        }
                        case AST_AsmIdiv_t:
                            infer_add_data_op(static_cast<AsmIdiv*>(node)->src.get());
                            break;
                        case AST_AsmDiv_t:
                            infer_add_data_op(static_cast<AsmDiv*>(node)->src.get());
                            break;
                        case AST_AsmSetCC_t:
                            infer_add_data_op(static_cast<AsmSetCC*>(node)->dst.get());
                            break;
                        case AST_AsmPush_t:
                            infer_add_data_op(static_cast<AsmPush*>(node)->src.get());
                            break;
                        case AST_AsmCdq_t:
                        case AST_AsmCall_t:
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
    if (context->data_flow_analysis_o2->data_name_map.size() < context->data_flow_analysis->set_size) {
        context->data_flow_analysis_o2->data_name_map.resize(context->data_flow_analysis->set_size);
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
#if __OPTIM_LEVEL__ == 1
        context->data_flow_analysis->instruction_index_map[context->data_flow_analysis_o1->addressed_index] =
            instructions_mask_sets_size;
        instructions_mask_sets_size++;
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
                dfa_forward_open_block(successor_id, i);
            }
        }
        while (i-- > 0) {
            context->data_flow_analysis->open_data_map[i] = context->control_flow_graph->exit_id;
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
        if (context->data_flow_analysis_o1->bak_instructions.size() < context->data_flow_analysis->set_size) {
            context->data_flow_analysis_o1->bak_instructions.resize(context->data_flow_analysis->set_size);
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
                dfa_backward_open_block(successor_id, i);
            }
        }
        for (; i < context->control_flow_graph->blocks.size(); i++) {
            context->data_flow_analysis->open_data_map[i] = context->control_flow_graph->exit_id;
        }

#if __OPTIM_LEVEL__ == 1
        GET_DFA_INSTR_SET_MASK(context->data_flow_analysis->static_index, 0) = MASK_FALSE;
        GET_DFA_INSTR_SET_MASK(context->data_flow_analysis_o1->addressed_index, 0) = MASK_FALSE;
#elif __OPTIM_LEVEL__ == 2
    {
        FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[function_name]->type_t.get());
        GET_DFA_INSTR_SET_MASK(context->data_flow_analysis->static_index, 0) = fun_type->ret_reg_mask;
    }
#endif
        for (i = 1; i < context->data_flow_analysis->mask_size; ++i) {
            GET_DFA_INSTR_SET_MASK(context->data_flow_analysis->static_index, i) = MASK_FALSE;
#if __OPTIM_LEVEL__ == 1
            GET_DFA_INSTR_SET_MASK(context->data_flow_analysis_o1->addressed_index, i) = MASK_FALSE;
#endif
        }

        for (const auto& name_id : context->control_flow_graph->identifier_id_map) {
#if __OPTIM_LEVEL__ == 1
            if (frontend->symbol_table[name_id.first]->attrs->type() == AST_StaticAttr_t) {
                SET_DFA_INSTR_SET_AT(context->data_flow_analysis->static_index, name_id.second, true);
            }
            if (frontend->addressed_set.find(name_id.first) != frontend->addressed_set.end()) {
                SET_DFA_INSTR_SET_AT(context->data_flow_analysis_o1->addressed_index, name_id.second, true);
            }
#elif __OPTIM_LEVEL__ == 2
        context->data_flow_analysis_o2->data_name_map[name_id.second - REGISTER_MASK_SIZE] = name_id.first;
#endif
        }

        std::fill(context->data_flow_analysis->blocks_mask_sets.begin(),
            context->data_flow_analysis->blocks_mask_sets.begin() + blocks_mask_sets_size, MASK_FALSE);
#if __OPTIM_LEVEL__ == 1
    }
#endif

    return true;
}

#endif
#endif
#endif
