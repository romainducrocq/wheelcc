#ifndef _OPTIMIZATION_IMPL_OLVL_H
#define _OPTIMIZATION_IMPL_OLVL_H

#ifdef __OPTIM_LEVEL__
#if __OPTIM_LEVEL__ >= 1 && __OPTIM_LEVEL__ <= 2

#define GET_INSTR(X) (*ctx->p_instrs)[X]
#define GET_CFG_BLOCK(X) ctx->cfg->blocks[X]

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
    size_t instrs_front_idx;
    size_t instrs_back_idx;
    std::vector<size_t> pred_ids;
    std::vector<size_t> succ_ids;
};

struct ControlFlowGraph {
    size_t entry_id;
    size_t exit_id;
    std::vector<size_t> entry_succ_ids;
    std::vector<size_t> exit_pred_ids;
    std::vector<bool> reaching_code;
    std::vector<ControlFlowBlock> blocks;
    std::unordered_map<TIdentifier, size_t> identifier_id_map;
};

struct DataFlowAnalysis {
    size_t set_size;
    size_t mask_size;
    size_t incoming_idx;
    size_t static_idx;
    std::vector<size_t> open_data_map;
    std::vector<size_t> instr_idx_map;
    std::vector<mask_t> blocks_mask_sets;
    std::vector<mask_t> instrs_mask_sets;
};

#if __OPTIM_LEVEL__ == 1
struct DataFlowAnalysisO1 {
    // Copy propagation
    std::vector<size_t> data_idx_map;
    std::vector<std::unique_ptr<TacInstruction>> bak_instrs;
    // Dead store elimination
    size_t addressed_idx;
};
#elif __OPTIM_LEVEL__ == 2
struct DataFlowAnalysisO2 {
    // Register allocation
    std::vector<TIdentifier> data_name_map;
};
#endif

static void set_instr(std::unique_ptr<AstInstruction>&& instr, size_t instr_idx) {
    if (instr) {
        GET_INSTR(instr_idx) = std::move(instr);
    }
    else {
        GET_INSTR(instr_idx).reset();
    }
#if __OPTIM_LEVEL__ == 1
    ctx->is_fixed_point = false;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Control flow graph

static void cfg_add_edge(std::vector<size_t>& succ_ids, std::vector<size_t>& pred_ids, size_t succ_id, size_t pred_id) {
    if (std::find(succ_ids.begin(), succ_ids.end(), succ_id) == succ_ids.end()) {
        succ_ids.push_back(succ_id);
    }
    if (std::find(pred_ids.begin(), pred_ids.end(), pred_id) == pred_ids.end()) {
        pred_ids.push_back(pred_id);
    }
}

static void cfg_add_succ_edge(size_t block_id, size_t succ_id) {
    if (succ_id < ctx->cfg->exit_id) {
        cfg_add_edge(GET_CFG_BLOCK(block_id).succ_ids, GET_CFG_BLOCK(succ_id).pred_ids, succ_id, block_id);
    }
    else if (succ_id == ctx->cfg->exit_id) {
        cfg_add_edge(GET_CFG_BLOCK(block_id).succ_ids, ctx->cfg->exit_pred_ids, succ_id, block_id);
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static void cfg_add_pred_edge(size_t block_id, size_t pred_id) {
    if (pred_id < ctx->cfg->exit_id) {
        cfg_add_edge(GET_CFG_BLOCK(pred_id).succ_ids, GET_CFG_BLOCK(block_id).pred_ids, block_id, pred_id);
    }
    else if (pred_id == ctx->cfg->entry_id) {
        cfg_add_edge(ctx->cfg->entry_succ_ids, GET_CFG_BLOCK(block_id).pred_ids, block_id, pred_id);
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static void cfg_rm_edge(
    std::vector<size_t>& succ_ids, std::vector<size_t>& pred_ids, size_t succ_id, size_t pred_id, bool is_reachable) {
    if (is_reachable) {
        for (size_t i = succ_ids.size(); i-- > 0;) {
            if (succ_ids[i] == succ_id) {
                std::swap(succ_ids[i], succ_ids.back());
                succ_ids.pop_back();
                break;
            }
        }
    }
    for (size_t i = pred_ids.size(); i-- > 0;) {
        if (pred_ids[i] == pred_id) {
            std::swap(pred_ids[i], pred_ids.back());
            pred_ids.pop_back();
            break;
        }
    }
}

static void cfg_rm_succ_edge(size_t block_id, size_t succ_id, bool is_reachable) {
    if (succ_id < ctx->cfg->exit_id) {
        cfg_rm_edge(GET_CFG_BLOCK(block_id).succ_ids, GET_CFG_BLOCK(succ_id).pred_ids, succ_id, block_id, is_reachable);
    }
    else if (succ_id == ctx->cfg->exit_id) {
        cfg_rm_edge(GET_CFG_BLOCK(block_id).succ_ids, ctx->cfg->exit_pred_ids, succ_id, block_id, is_reachable);
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static void cfg_rm_pred_edge(size_t block_id, size_t pred_id) {
    if (pred_id < ctx->cfg->exit_id) {
        cfg_rm_edge(GET_CFG_BLOCK(pred_id).succ_ids, GET_CFG_BLOCK(block_id).pred_ids, block_id, pred_id, true);
    }
    else if (pred_id == ctx->cfg->entry_id) {
        cfg_rm_edge(ctx->cfg->entry_succ_ids, GET_CFG_BLOCK(block_id).pred_ids, block_id, pred_id, true);
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static void cfg_rm_empty_block(size_t block_id, bool is_reachable) {
    for (size_t succ_id : GET_CFG_BLOCK(block_id).succ_ids) {
        if (is_reachable) {
            for (size_t pred_id : GET_CFG_BLOCK(block_id).pred_ids) {
                if (pred_id == ctx->cfg->entry_id) {
                    cfg_add_pred_edge(succ_id, pred_id);
                }
                else {
                    cfg_add_succ_edge(pred_id, succ_id);
                }
            }
        }
        cfg_rm_succ_edge(block_id, succ_id, is_reachable);
    }
    if (is_reachable) {
        for (size_t pred_id : GET_CFG_BLOCK(block_id).pred_ids) {
            cfg_rm_pred_edge(block_id, pred_id);
        }
    }
    GET_CFG_BLOCK(block_id).instrs_front_idx = ctx->cfg->exit_id;
    GET_CFG_BLOCK(block_id).instrs_back_idx = ctx->cfg->exit_id;
}

static void cfg_rm_block_instr(size_t instr_idx, size_t block_id) {
    if (GET_INSTR(instr_idx)) {
        set_instr(nullptr, instr_idx);
        GET_CFG_BLOCK(block_id).size--;
        if (GET_CFG_BLOCK(block_id).size == 0) {
            cfg_rm_empty_block(block_id, true);
        }
        else if (instr_idx == GET_CFG_BLOCK(block_id).instrs_front_idx) {
            for (; instr_idx <= GET_CFG_BLOCK(block_id).instrs_back_idx; ++instr_idx) {
                if (GET_INSTR(instr_idx)) {
                    GET_CFG_BLOCK(block_id).instrs_front_idx = instr_idx;
                    break;
                }
            }
        }
        else if (instr_idx == GET_CFG_BLOCK(block_id).instrs_back_idx) {
            instr_idx++;
            for (; instr_idx-- > GET_CFG_BLOCK(block_id).instrs_front_idx;) {
                if (GET_INSTR(instr_idx)) {
                    GET_CFG_BLOCK(block_id).instrs_back_idx = instr_idx;
                    break;
                }
            }
        }
    }
}

#if __OPTIM_LEVEL__ == 1
static void cfg_init_label_block(TacLabel* node) {
    ctx->cfg->identifier_id_map[node->name] = ctx->cfg->blocks.size() - 1;
}
#elif __OPTIM_LEVEL__ == 2
static void cfg_init_label_block(AsmLabel* node) {
    ctx->cfg->identifier_id_map[node->name] = ctx->cfg->blocks.size() - 1;
}
#endif

static void cfg_init_block(size_t instr_idx, size_t& instrs_back_idx) {
    AstInstruction* node = GET_INSTR(instr_idx).get();
    switch (node->type()) {
#if __OPTIM_LEVEL__ == 1
        case AST_TacLabel_t:
#elif __OPTIM_LEVEL__ == 2
        case AST_AsmLabel_t:
#endif
        {
            if (instrs_back_idx != ctx->p_instrs->size()) {
                ctx->cfg->blocks.back().instrs_back_idx = instrs_back_idx;
                ControlFlowBlock block = {0, instr_idx, 0, {}, {}};
                ctx->cfg->blocks.emplace_back(std::move(block));
            }
#if __OPTIM_LEVEL__ == 1
            cfg_init_label_block(static_cast<TacLabel*>(node));
#elif __OPTIM_LEVEL__ == 2
            cfg_init_label_block(static_cast<AsmLabel*>(node));
#endif
            instrs_back_idx = instr_idx;
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
            ctx->cfg->blocks.back().instrs_back_idx = instr_idx;
            instrs_back_idx = ctx->p_instrs->size();
            break;
        }
        default: {
            instrs_back_idx = instr_idx;
            break;
        }
    }
}

#if __OPTIM_LEVEL__ == 1
static void cfg_init_jump_edges(TacJump* node, size_t block_id) {
    cfg_add_succ_edge(block_id, ctx->cfg->identifier_id_map[node->target]);
}

static void cfg_init_jmp_eq_0_edges(TacJumpIfZero* node, size_t block_id) {
    cfg_add_succ_edge(block_id, ctx->cfg->identifier_id_map[node->target]);
    cfg_add_succ_edge(block_id, block_id + 1);
}

static void cfg_init_jmp_ne_0_edges(TacJumpIfNotZero* node, size_t block_id) {
    cfg_add_succ_edge(block_id, ctx->cfg->identifier_id_map[node->target]);
    cfg_add_succ_edge(block_id, block_id + 1);
}
#elif __OPTIM_LEVEL__ == 2
static void cfg_init_jmp_edges(AsmJmp* node, size_t block_id) {
    cfg_add_succ_edge(block_id, ctx->cfg->identifier_id_map[node->target]);
}

static void cfg_init_jmp_cc_edges(AsmJmpCC* node, size_t block_id) {
    cfg_add_succ_edge(block_id, ctx->cfg->identifier_id_map[node->target]);
    cfg_add_succ_edge(block_id, block_id + 1);
}
#endif

static void cfg_init_edges(size_t block_id) {
    AstInstruction* node = GET_INSTR(GET_CFG_BLOCK(block_id).instrs_back_idx).get();
    switch (node->type()) {
#if __OPTIM_LEVEL__ == 1
        case AST_TacReturn_t:
#elif __OPTIM_LEVEL__ == 2
        case AST_AsmRet_t:
#endif
            cfg_add_succ_edge(block_id, ctx->cfg->exit_id);
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
    ctx->cfg->blocks.clear();
    ctx->cfg->identifier_id_map.clear();
    {
        size_t instrs_back_idx = ctx->p_instrs->size();
        for (size_t instr_idx = 0; instr_idx < ctx->p_instrs->size(); ++instr_idx) {
            if (GET_INSTR(instr_idx)) {
                if (instrs_back_idx == ctx->p_instrs->size()) {
                    ControlFlowBlock block = {0, instr_idx, 0, {}, {}};
                    ctx->cfg->blocks.emplace_back(std::move(block));
                }
                cfg_init_block(instr_idx, instrs_back_idx);
                ctx->cfg->blocks.back().size++;
            }
        }
        if (instrs_back_idx != ctx->p_instrs->size()) {
            ctx->cfg->blocks.back().instrs_back_idx = instrs_back_idx;
        }
    }

    ctx->cfg->exit_id = ctx->cfg->blocks.size();
    ctx->cfg->entry_id = ctx->cfg->exit_id + 1;
    ctx->cfg->entry_succ_ids.clear();
    ctx->cfg->exit_pred_ids.clear();
    if (!ctx->cfg->blocks.empty()) {
        cfg_add_pred_edge(0, ctx->cfg->entry_id);
        for (size_t block_id = 0; block_id < ctx->cfg->blocks.size(); ++block_id) {
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

#define GET_DFA_BLOCK_SET_IDX(X, Y) (X) * ctx->dfa->mask_size + (Y)
#define GET_DFA_INSTR_SET_IDX(X, Y) ctx->dfa->instr_idx_map[X] * ctx->dfa->mask_size + (Y)

#define GET_DFA_BLOCK_SET_MASK(X, Y) ctx->dfa->blocks_mask_sets[GET_DFA_BLOCK_SET_IDX(X, Y)]
#define GET_DFA_INSTR_SET_MASK(X, Y) ctx->dfa->instrs_mask_sets[GET_DFA_INSTR_SET_IDX(X, Y)]

#define GET_DFA_BLOCK_SET_AT(X, Y) mask_get(GET_DFA_BLOCK_SET_MASK(X, MASK_OFFSET(Y)), Y)
#define GET_DFA_INSTR_SET_AT(X, Y) mask_get(GET_DFA_INSTR_SET_MASK(X, MASK_OFFSET(Y)), Y)

#define SET_DFA_INSTR_SET_AT(X, Y, Z) mask_set(GET_DFA_INSTR_SET_MASK(X, MASK_OFFSET(Y)), Y, Z)

#if __OPTIM_LEVEL__ == 1
#define GET_DFA_INSTR(X) GET_INSTR(ctx->dfa_o1->data_idx_map[X])
#endif

static bool is_transfer_instr(size_t instr_idx
#if __OPTIM_LEVEL__ == 1
    ,
    bool is_store_elim
#endif
) {
    switch (GET_INSTR(instr_idx)->type()) {
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
            return is_store_elim;
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
static size_t get_dfa_data_idx(size_t instr_idx) {
    for (size_t i = 0; i < ctx->dfa->set_size; ++i) {
        if (ctx->dfa_o1->data_idx_map[i] == instr_idx) {
            return i;
        }
    }
    RAISE_INTERNAL_ERROR;
}

static TacInstruction* get_dfa_bak_instr(size_t i) {
    if (ctx->cfg->reaching_code[i]) {
        if (ctx->dfa_o1->bak_instrs[i]) {
            return ctx->dfa_o1->bak_instrs[i].get();
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

static bool set_dfa_bak_instr(size_t instr_idx, size_t& i) {
    i = get_dfa_data_idx(instr_idx);
    if (!ctx->cfg->reaching_code[i]) {
        ctx->cfg->reaching_code[i] = true;
        return true;
    }
    else {
        return false;
    }
}
#endif

#if __OPTIM_LEVEL__ == 1
static bool prop_transfer_reach_copies(size_t instr_idx, size_t next_instr_idx);
static void elim_transfer_live_values(size_t instr_idx, size_t next_instr_idx);
#elif __OPTIM_LEVEL__ == 2
static void infer_transfer_live_regs(size_t instr_idx, size_t next_instr_idx);
#endif

#if __OPTIM_LEVEL__ == 1
static size_t dfa_forward_transfer_block(size_t instr_idx, size_t block_id) {
    for (size_t next_instr_idx = instr_idx + 1; next_instr_idx <= GET_CFG_BLOCK(block_id).instrs_back_idx;
         ++next_instr_idx) {
        if (GET_INSTR(next_instr_idx) && is_transfer_instr(next_instr_idx, false)) {
            for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
                GET_DFA_INSTR_SET_MASK(next_instr_idx, i) = GET_DFA_INSTR_SET_MASK(instr_idx, i);
            }
            if (!prop_transfer_reach_copies(instr_idx, next_instr_idx)) {
                for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
                    GET_DFA_INSTR_SET_MASK(next_instr_idx, i) = GET_DFA_INSTR_SET_MASK(instr_idx, i);
                }
            }
            instr_idx = next_instr_idx;
        }
    }
    for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
        GET_DFA_INSTR_SET_MASK(ctx->dfa->incoming_idx, i) = GET_DFA_INSTR_SET_MASK(instr_idx, i);
    }
    if (!prop_transfer_reach_copies(instr_idx, ctx->dfa->incoming_idx)) {
        for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
            GET_DFA_INSTR_SET_MASK(ctx->dfa->incoming_idx, i) = GET_DFA_INSTR_SET_MASK(instr_idx, i);
        }
    }
    return instr_idx;
}
#endif

static size_t dfa_backward_transfer_block(size_t instr_idx, size_t block_id) {
    if (instr_idx > 0) {
        for (size_t next_instr_idx = instr_idx; next_instr_idx-- > GET_CFG_BLOCK(block_id).instrs_front_idx;) {
            if (GET_INSTR(next_instr_idx)
                && is_transfer_instr(next_instr_idx
#if __OPTIM_LEVEL__ == 1
                    ,
                    true
#endif
                    )) {
                for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
                    GET_DFA_INSTR_SET_MASK(next_instr_idx, i) = GET_DFA_INSTR_SET_MASK(instr_idx, i);
                }
#if __OPTIM_LEVEL__ == 1
                elim_transfer_live_values
#elif __OPTIM_LEVEL__ == 2
                infer_transfer_live_regs
#endif
                    (instr_idx, next_instr_idx);
                instr_idx = next_instr_idx;
            }
        }
    }
    for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
        GET_DFA_INSTR_SET_MASK(ctx->dfa->incoming_idx, i) = GET_DFA_INSTR_SET_MASK(instr_idx, i);
    }
#if __OPTIM_LEVEL__ == 1
    elim_transfer_live_values
#elif __OPTIM_LEVEL__ == 2
    infer_transfer_live_regs
#endif
        (instr_idx, ctx->dfa->incoming_idx);
    return instr_idx;
}

static bool dfa_after_meet_block(size_t block_id) {
    bool is_fixed_point = true;
    {
        size_t i = 0;
        for (; i < ctx->dfa->mask_size; ++i) {
            if (GET_DFA_BLOCK_SET_MASK(block_id, i) != GET_DFA_INSTR_SET_MASK(ctx->dfa->incoming_idx, i)) {
                is_fixed_point = false;
                break;
            }
        }
        for (; i < ctx->dfa->mask_size; ++i) {
            GET_DFA_BLOCK_SET_MASK(block_id, i) = GET_DFA_INSTR_SET_MASK(ctx->dfa->incoming_idx, i);
        }
    }
    return is_fixed_point;
}

#if __OPTIM_LEVEL__ == 1
static bool dfa_forward_meet_block(size_t block_id) {
    size_t instr_idx = GET_CFG_BLOCK(block_id).instrs_front_idx;
    for (; instr_idx <= GET_CFG_BLOCK(block_id).instrs_back_idx; ++instr_idx) {
        if (GET_INSTR(instr_idx) && is_transfer_instr(instr_idx, false)) {
            goto Lelse;
        }
    }
    instr_idx = ctx->dfa->incoming_idx;
Lelse:
    for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
        GET_DFA_INSTR_SET_MASK(instr_idx, i) = MASK_TRUE;
    }

    for (size_t pred_id : GET_CFG_BLOCK(block_id).pred_ids) {
        if (pred_id < ctx->cfg->exit_id) {
            for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
                GET_DFA_INSTR_SET_MASK(instr_idx, i) &= GET_DFA_BLOCK_SET_MASK(pred_id, i);
            }
        }
        else if (pred_id == ctx->cfg->entry_id) {
            for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
                GET_DFA_INSTR_SET_MASK(instr_idx, i) = MASK_FALSE;
            }
            break;
        }
        else {
            RAISE_INTERNAL_ERROR;
        }
    }

    if (instr_idx < ctx->dfa->incoming_idx) {
        dfa_forward_transfer_block(instr_idx, block_id);
    }
    else if (instr_idx != ctx->dfa->incoming_idx) {
        RAISE_INTERNAL_ERROR;
    }

    return dfa_after_meet_block(block_id);
}
#endif

static bool dfa_backward_meet_block(size_t block_id) {
    size_t instr_idx = GET_CFG_BLOCK(block_id).instrs_back_idx + 1;
    while (instr_idx-- > GET_CFG_BLOCK(block_id).instrs_front_idx) {
        if (GET_INSTR(instr_idx)
            && is_transfer_instr(instr_idx
#if __OPTIM_LEVEL__ == 1
                ,
                true
#endif
                )) {
            goto Lelse;
        }
    }
    instr_idx = ctx->dfa->incoming_idx;
Lelse:
    for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
        GET_DFA_INSTR_SET_MASK(instr_idx, i) = MASK_FALSE;
    }

    for (size_t succ_id : GET_CFG_BLOCK(block_id).succ_ids) {
        if (succ_id < ctx->cfg->exit_id) {
            for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
                GET_DFA_INSTR_SET_MASK(instr_idx, i) |= GET_DFA_BLOCK_SET_MASK(succ_id, i);
            }
        }
        else if (succ_id == ctx->cfg->exit_id) {
            for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
                GET_DFA_INSTR_SET_MASK(instr_idx, i) = GET_DFA_INSTR_SET_MASK(ctx->dfa->static_idx, i);
            }
            break;
        }
        else {
            RAISE_INTERNAL_ERROR;
        }
    }

    if (instr_idx < ctx->dfa->incoming_idx) {
        dfa_backward_transfer_block(instr_idx, block_id);
    }
    else if (instr_idx != ctx->dfa->incoming_idx) {
        RAISE_INTERNAL_ERROR;
    }

    return dfa_after_meet_block(block_id);
}

#if __OPTIM_LEVEL__ == 1
static void dfa_forward_iter_alg() {
    size_t open_data_map_size = ctx->cfg->blocks.size();
    for (size_t i = 0; i < open_data_map_size; ++i) {
        size_t block_id = ctx->dfa->open_data_map[i];
        if (block_id == ctx->cfg->exit_id) {
            continue;
        }

        bool is_fixed_point = dfa_forward_meet_block(block_id);
        if (!is_fixed_point) {
            for (size_t succ_id : GET_CFG_BLOCK(block_id).succ_ids) {
                if (succ_id < ctx->cfg->exit_id) {
                    for (size_t j = i + 1; j < open_data_map_size; ++j) {
                        if (succ_id == ctx->dfa->open_data_map[j]) {
                            goto Lelse;
                        }
                    }
                    if (open_data_map_size < ctx->dfa->open_data_map.size()) {
                        ctx->dfa->open_data_map[open_data_map_size] = succ_id;
                    }
                    else {
                        ctx->dfa->open_data_map.push_back(succ_id);
                    }
                    open_data_map_size++;
                Lelse:;
                }
                else if (succ_id != ctx->cfg->exit_id) {
                    RAISE_INTERNAL_ERROR;
                }
            }
        }
    }
}
#endif

static void dfa_iter_alg() {
    size_t open_data_map_size = ctx->cfg->blocks.size();
    for (size_t i = 0; i < open_data_map_size; ++i) {
        size_t block_id = ctx->dfa->open_data_map[i];
        if (block_id == ctx->cfg->exit_id) {
            continue;
        }

        bool is_fixed_point = dfa_backward_meet_block(block_id);
        if (!is_fixed_point) {
            for (size_t pred_id : GET_CFG_BLOCK(block_id).pred_ids) {
                if (pred_id < ctx->cfg->exit_id) {
                    for (size_t j = i + 1; j < open_data_map_size; ++j) {
                        if (pred_id == ctx->dfa->open_data_map[j]) {
                            goto Lelse;
                        }
                    }
                    if (open_data_map_size < ctx->dfa->open_data_map.size()) {
                        ctx->dfa->open_data_map[open_data_map_size] = pred_id;
                    }
                    else {
                        ctx->dfa->open_data_map.push_back(pred_id);
                    }
                    open_data_map_size++;
                Lelse:;
                }
                else if (pred_id != ctx->cfg->entry_id) {
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
    for (size_t succ_id : GET_CFG_BLOCK(block_id).succ_ids) {
        dfa_forward_open_block(succ_id, i);
    }
}
#endif

static void dfa_backward_succ_open_block(size_t block_id, size_t& i) {
    for (size_t succ_id : GET_CFG_BLOCK(block_id).succ_ids) {
        dfa_backward_open_block(succ_id, i);
    }
}

#if __OPTIM_LEVEL__ == 1
static void dfa_forward_open_block(size_t block_id, size_t& i) {
    if (block_id < ctx->cfg->exit_id && !ctx->cfg->reaching_code[block_id]) {
        ctx->cfg->reaching_code[block_id] = true;
        dfa_forward_succ_open_block(block_id, i);
        i--;
        ctx->dfa->open_data_map[i] = block_id;
    }
}
#endif

static void dfa_backward_open_block(size_t block_id, size_t& i) {
    if (block_id < ctx->cfg->exit_id && !ctx->cfg->reaching_code[block_id]) {
        ctx->cfg->reaching_code[block_id] = true;
        dfa_backward_succ_open_block(block_id, i);
        ctx->dfa->open_data_map[i] = block_id;
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

static bool prop_add_data_idx(TacCopy* node, size_t instr_idx, size_t block_id) {
    if (node->dst->type() != AST_TacVariable_t) {
        RAISE_INTERNAL_ERROR;
    }
    else if (is_same_value(node->src.get(), node->dst.get())) {
        cfg_rm_block_instr(instr_idx, block_id);
        return false;
    }
    else {
        if (ctx->dfa->set_size < ctx->dfa_o1->data_idx_map.size()) {
            ctx->dfa_o1->data_idx_map[ctx->dfa->set_size] = instr_idx;
        }
        else {
            ctx->dfa_o1->data_idx_map.push_back(instr_idx);
        }
        ctx->dfa->set_size++;
        return true;
    }
}

static void elim_add_data_name(TIdentifier name) {
    if (ctx->cfg->identifier_id_map.find(name) == ctx->cfg->identifier_id_map.end()) {
        ctx->cfg->identifier_id_map[name] = ctx->dfa->set_size;
        ctx->dfa->set_size++;
    }
}

static void elim_add_data_value(TacValue* node) {
    if (node->type() == AST_TacVariable_t) {
        elim_add_data_name(static_cast<TacVariable*>(node)->name);
    }
}
#elif __OPTIM_LEVEL__ == 2
static void infer_add_data_name(TIdentifier name) {
    if (!is_aliased_name(name) && ctx->cfg->identifier_id_map.find(name) == ctx->cfg->identifier_id_map.end()) {
        ctx->cfg->identifier_id_map[name] = REGISTER_MASK_SIZE + ctx->dfa->set_size;
        ctx->dfa->set_size++;
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
    bool is_store_elim, bool is_addressed_set
#elif __OPTIM_LEVEL__ == 2
    TIdentifier fun_name
#endif
) {
    ctx->dfa->set_size = 0;
    ctx->dfa->incoming_idx = ctx->p_instrs->size();

    if (ctx->dfa->open_data_map.size() < ctx->cfg->blocks.size()) {
        ctx->dfa->open_data_map.resize(ctx->cfg->blocks.size());
    }
    {
        size_t i =
#if __OPTIM_LEVEL__ == 1
            is_store_elim ? 3 : 1
#elif __OPTIM_LEVEL__ == 2
            2
#endif
            ;
        if (ctx->dfa->instr_idx_map.size() < ctx->p_instrs->size() + i) {
            ctx->dfa->instr_idx_map.resize(ctx->p_instrs->size() + i);
        }
    }
    if (ctx->cfg->reaching_code.size() < ctx->cfg->blocks.size()) {
        ctx->cfg->reaching_code.resize(ctx->cfg->blocks.size());
    }
    std::fill(ctx->cfg->reaching_code.begin(), ctx->cfg->reaching_code.begin() + ctx->cfg->blocks.size(), false);

    size_t instrs_mask_sets_size = 0;
#if __OPTIM_LEVEL__ == 1
    bool is_copy_prop = !is_store_elim;
    if (is_store_elim) {
#endif
        ctx->cfg->identifier_id_map.clear();
        ctx->dfa->static_idx = ctx->dfa->incoming_idx + 1;
#if __OPTIM_LEVEL__ == 1
        ctx->dfa_o1->addressed_idx = ctx->dfa->static_idx + 1;
    }
    if (is_addressed_set) {
        frontend->addressed_set.clear();
    }
#endif
    for (size_t block_id = 0; block_id < ctx->cfg->blocks.size(); ++block_id) {
        if (GET_CFG_BLOCK(block_id).size > 0) {
            for (size_t instr_idx = GET_CFG_BLOCK(block_id).instrs_front_idx;
                 instr_idx <= GET_CFG_BLOCK(block_id).instrs_back_idx; ++instr_idx) {
                if (GET_INSTR(instr_idx)) {
                    AstInstruction* node = GET_INSTR(instr_idx).get();
                    switch (node->type()) {
#if __OPTIM_LEVEL__ == 1
                        case AST_TacReturn_t: {
                            if (is_copy_prop) {
                                goto Lcontinue;
                            }
                            TacReturn* p_node = static_cast<TacReturn*>(node);
                            if (p_node->val) {
                                elim_add_data_value(p_node->val.get());
                            }
                            break;
                        }
                        case AST_TacSignExtend_t: {
                            if (is_store_elim) {
                                TacSignExtend* p_node = static_cast<TacSignExtend*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacTruncate_t: {
                            if (is_store_elim) {
                                TacTruncate* p_node = static_cast<TacTruncate*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacZeroExtend_t: {
                            if (is_store_elim) {
                                TacZeroExtend* p_node = static_cast<TacZeroExtend*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacDoubleToInt_t: {
                            if (is_store_elim) {
                                TacDoubleToInt* p_node = static_cast<TacDoubleToInt*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacDoubleToUInt_t: {
                            if (is_store_elim) {
                                TacDoubleToUInt* p_node = static_cast<TacDoubleToUInt*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacIntToDouble_t: {
                            if (is_store_elim) {
                                TacIntToDouble* p_node = static_cast<TacIntToDouble*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacUIntToDouble_t: {
                            if (is_store_elim) {
                                TacUIntToDouble* p_node = static_cast<TacUIntToDouble*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacFunCall_t: {
                            if (is_store_elim) {
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
                            if (is_store_elim) {
                                TacUnary* p_node = static_cast<TacUnary*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacBinary_t: {
                            if (is_store_elim) {
                                TacBinary* p_node = static_cast<TacBinary*>(node);
                                elim_add_data_value(p_node->src1.get());
                                elim_add_data_value(p_node->src2.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacCopy_t: {
                            TacCopy* p_node = static_cast<TacCopy*>(node);
                            if (is_copy_prop) {
                                if (!prop_add_data_idx(p_node, instr_idx, block_id)) {
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
                            if (is_store_elim) {
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            if (is_addressed_set) {
                                dfa_add_aliased_value(p_node->src.get());
                            }
                            break;
                        }
                        case AST_TacLoad_t: {
                            if (is_store_elim) {
                                TacLoad* p_node = static_cast<TacLoad*>(node);
                                elim_add_data_value(p_node->src_ptr.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacStore_t: {
                            if (is_store_elim) {
                                TacStore* p_node = static_cast<TacStore*>(node);
                                elim_add_data_value(p_node->src.get());
                                elim_add_data_value(p_node->dst_ptr.get());
                            }
                            break;
                        }
                        case AST_TacAddPtr_t: {
                            if (is_store_elim) {
                                TacAddPtr* p_node = static_cast<TacAddPtr*>(node);
                                elim_add_data_value(p_node->src_ptr.get());
                                elim_add_data_value(p_node->idx.get());
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacCopyToOffset_t: {
                            if (is_store_elim) {
                                TacCopyToOffset* p_node = static_cast<TacCopyToOffset*>(node);
                                elim_add_data_name(p_node->dst_name);
                                elim_add_data_value(p_node->src.get());
                            }
                            break;
                        }
                        case AST_TacCopyFromOffset_t: {
                            if (is_store_elim) {
                                TacCopyFromOffset* p_node = static_cast<TacCopyFromOffset*>(node);
                                elim_add_data_name(p_node->src_name);
                                elim_add_data_value(p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacJumpIfZero_t: {
                            if (is_copy_prop) {
                                goto Lcontinue;
                            }
                            elim_add_data_value(static_cast<TacJumpIfZero*>(node)->condition.get());
                            break;
                        }
                        case AST_TacJumpIfNotZero_t: {
                            if (is_copy_prop) {
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
                    ctx->dfa->instr_idx_map[instr_idx] = instrs_mask_sets_size;
                    instrs_mask_sets_size++;
                Lcontinue:;
                }
            }
        }
        else {
            ctx->cfg->reaching_code[block_id] = true;
        }
    }
    if (ctx->dfa->set_size == 0) {
        return false;
    }

#if __OPTIM_LEVEL__ == 2
    if (ctx->dfa_o2->data_name_map.size() < ctx->dfa->set_size) {
        ctx->dfa_o2->data_name_map.resize(ctx->dfa->set_size);
    }
    ctx->dfa->set_size += REGISTER_MASK_SIZE;
#endif

    ctx->dfa->instr_idx_map[ctx->dfa->incoming_idx] = instrs_mask_sets_size;
    instrs_mask_sets_size++;
#if __OPTIM_LEVEL__ == 1
    if (is_store_elim) {
#endif
        ctx->dfa->instr_idx_map[ctx->dfa->static_idx] = instrs_mask_sets_size;
        instrs_mask_sets_size++;
#if __OPTIM_LEVEL__ == 1
        ctx->dfa->instr_idx_map[ctx->dfa_o1->addressed_idx] = instrs_mask_sets_size;
        instrs_mask_sets_size++;
    }
#endif
    ctx->dfa->mask_size = (ctx->dfa->set_size + 63) / 64;
    instrs_mask_sets_size *= ctx->dfa->mask_size;
    size_t blocks_mask_sets_size = ctx->dfa->mask_size * ctx->cfg->blocks.size();

    if (ctx->dfa->blocks_mask_sets.size() < blocks_mask_sets_size) {
        ctx->dfa->blocks_mask_sets.resize(blocks_mask_sets_size);
    }
    if (ctx->dfa->instrs_mask_sets.size() < instrs_mask_sets_size) {
        ctx->dfa->instrs_mask_sets.resize(instrs_mask_sets_size);
    }

#if __OPTIM_LEVEL__ == 1
    if (is_copy_prop) {
        size_t i = ctx->cfg->blocks.size();
        for (size_t succ_id : ctx->cfg->entry_succ_ids) {
            if (!ctx->cfg->reaching_code[succ_id]) {
                dfa_forward_open_block(succ_id, i);
            }
        }
        while (i-- > 0) {
            ctx->dfa->open_data_map[i] = ctx->cfg->exit_id;
        }

        mask_t mask_true_back = MASK_TRUE;
        i = ctx->dfa->set_size - (ctx->dfa->mask_size - 1) * 64;
        if (i > 0) {
            for (; i < 64; ++i) {
                mask_set(mask_true_back, i, false);
            }
        }

        if (ctx->cfg->reaching_code.size() < ctx->dfa->set_size) {
            ctx->cfg->reaching_code.resize(ctx->dfa->set_size);
        }
        if (ctx->dfa_o1->bak_instrs.size() < ctx->dfa->set_size) {
            ctx->dfa_o1->bak_instrs.resize(ctx->dfa->set_size);
        }

        std::fill(ctx->cfg->reaching_code.begin(), ctx->cfg->reaching_code.begin() + ctx->dfa->set_size, false);

        if (ctx->dfa->mask_size > 1) {
            i = 0;
            do {
                for (size_t j = ctx->dfa->mask_size - 1; j-- > 0;) {
                    ctx->dfa->blocks_mask_sets[i] = MASK_TRUE;
                    i++;
                }
                ctx->dfa->blocks_mask_sets[i] = mask_true_back;
                i++;
            }
            while (i < blocks_mask_sets_size);
        }
        else {
            std::fill(ctx->dfa->blocks_mask_sets.begin(), ctx->dfa->blocks_mask_sets.begin() + blocks_mask_sets_size,
                mask_true_back);
        }
    }
    else {
#endif
        size_t i = 0;
        for (size_t succ_id : ctx->cfg->entry_succ_ids) {
            if (!ctx->cfg->reaching_code[succ_id]) {
                dfa_backward_open_block(succ_id, i);
            }
        }
        for (; i < ctx->cfg->blocks.size(); i++) {
            ctx->dfa->open_data_map[i] = ctx->cfg->exit_id;
        }

#if __OPTIM_LEVEL__ == 1
        GET_DFA_INSTR_SET_MASK(ctx->dfa->static_idx, 0) = MASK_FALSE;
        GET_DFA_INSTR_SET_MASK(ctx->dfa_o1->addressed_idx, 0) = MASK_FALSE;
#elif __OPTIM_LEVEL__ == 2
    {
        FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[fun_name]->type_t.get());
        GET_DFA_INSTR_SET_MASK(ctx->dfa->static_idx, 0) = fun_type->ret_reg_mask;
    }
#endif
        for (i = 1; i < ctx->dfa->mask_size; ++i) {
            GET_DFA_INSTR_SET_MASK(ctx->dfa->static_idx, i) = MASK_FALSE;
#if __OPTIM_LEVEL__ == 1
            GET_DFA_INSTR_SET_MASK(ctx->dfa_o1->addressed_idx, i) = MASK_FALSE;
#endif
        }

        for (const auto& name_id : ctx->cfg->identifier_id_map) {
#if __OPTIM_LEVEL__ == 1
            if (frontend->symbol_table[name_id.first]->attrs->type() == AST_StaticAttr_t) {
                SET_DFA_INSTR_SET_AT(ctx->dfa->static_idx, name_id.second, true);
            }
            if (frontend->addressed_set.find(name_id.first) != frontend->addressed_set.end()) {
                SET_DFA_INSTR_SET_AT(ctx->dfa_o1->addressed_idx, name_id.second, true);
            }
#elif __OPTIM_LEVEL__ == 2
        ctx->dfa_o2->data_name_map[name_id.second - REGISTER_MASK_SIZE] = name_id.first;
#endif
        }

        std::fill(
            ctx->dfa->blocks_mask_sets.begin(), ctx->dfa->blocks_mask_sets.begin() + blocks_mask_sets_size, MASK_FALSE);
#if __OPTIM_LEVEL__ == 1
    }
#endif

    return true;
}

#endif
#endif
#endif
