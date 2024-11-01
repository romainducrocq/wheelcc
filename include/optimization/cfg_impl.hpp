#if defined(__LEVEL_1__) && defined(__LEVEL_2__)
#define _OPTIMIZATION_CFG_IMPL_HPP
#endif

#ifndef _OPTIMIZATION_CFG_IMPL_HPP
#define _OPTIMIZATION_CFG_IMPL_HPP

#if defined(__LEVEL_1__) || defined(__LEVEL_2__)

#ifdef __LEVEL_1__
#define GET_INSTRUCTION(X) (*context->p_instructions)[X]
#define GET_CFG_BLOCK(X) context->control_flow_graph->blocks[X]
#endif

#ifdef __LEVEL_1__
#define AST_INSTRUCTION TacInstruction
#endif

static void set_instruction(std::unique_ptr<AST_INSTRUCTION>&& instruction, size_t instruction_index);

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

#ifdef __LEVEL_1__
static void control_flow_graph_initialize_label_block(TacLabel* node) {
    context->control_flow_graph->label_id_map[node->name] = context->control_flow_graph->blocks.size() - 1;
}
#endif

static void control_flow_graph_initialize_block(size_t instruction_index, size_t& instructions_back_index) {
    AST_INSTRUCTION* node = GET_INSTRUCTION(instruction_index).get();
    switch (node->type()) {
#ifdef __LEVEL_1__
        case AST_T::TacLabel_t:
#endif
        {
            if (instructions_back_index != context->p_instructions->size()) {
                context->control_flow_graph->blocks.back().instructions_back_index = instructions_back_index;
                ControlFlowBlock block {0, instruction_index, 0, {}, {}};
                context->control_flow_graph->blocks.emplace_back(std::move(block));
            }
#ifdef __LEVEL_1__
            control_flow_graph_initialize_label_block(static_cast<TacLabel*>(node));
#endif
            instructions_back_index = instruction_index;
            break;
        }
#ifdef __LEVEL_1__
        case AST_T::TacReturn_t:
        case AST_T::TacJump_t:
        case AST_T::TacJumpIfZero_t:
        case AST_T::TacJumpIfNotZero_t:
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

#ifdef __LEVEL_1__
static void control_flow_graph_initialize_jump_edges(TacJump* node, size_t block_id) {
    control_flow_graph_add_successor_edge(block_id, context->control_flow_graph->label_id_map[node->target]);
}

static void control_flow_graph_initialize_jump_if_zero_edges(TacJumpIfZero* node, size_t block_id) {
    control_flow_graph_add_successor_edge(block_id, context->control_flow_graph->label_id_map[node->target]);
    control_flow_graph_add_successor_edge(block_id, block_id + 1);
}

static void control_flow_graph_initialize_jump_if_not_zero_edges(TacJumpIfNotZero* node, size_t block_id) {
    control_flow_graph_add_successor_edge(block_id, context->control_flow_graph->label_id_map[node->target]);
    control_flow_graph_add_successor_edge(block_id, block_id + 1);
}
#endif

static void control_flow_graph_initialize_edges(size_t block_id) {
    AST_INSTRUCTION* node = GET_INSTRUCTION(GET_CFG_BLOCK(block_id).instructions_back_index).get();
    switch (node->type()) {
#ifdef __LEVEL_1__
        case AST_T::TacReturn_t:
#endif
            control_flow_graph_add_successor_edge(block_id, context->control_flow_graph->exit_id);
            break;
#ifdef __LEVEL_1__
        case AST_T::TacJump_t:
            control_flow_graph_initialize_jump_edges(static_cast<TacJump*>(node), block_id);
            break;
        case AST_T::TacJumpIfZero_t:
            control_flow_graph_initialize_jump_if_zero_edges(static_cast<TacJumpIfZero*>(node), block_id);
            break;
        case AST_T::TacJumpIfNotZero_t:
            control_flow_graph_initialize_jump_if_not_zero_edges(static_cast<TacJumpIfNotZero*>(node), block_id);
            break;
#endif
        default:
            control_flow_graph_add_successor_edge(block_id, block_id + 1);
            break;
    }
}

static void control_flow_graph_initialize() {
    context->control_flow_graph->blocks.clear();
    context->control_flow_graph->label_id_map.clear();
    {
        size_t instructions_back_index = context->p_instructions->size();
        for (size_t instruction_index = 0; instruction_index < context->p_instructions->size(); ++instruction_index) {
            if (GET_INSTRUCTION(instruction_index)) {
                if (instructions_back_index == context->p_instructions->size()) {
                    ControlFlowBlock block {0, instruction_index, 0, {}, {}};
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

#endif
#endif
