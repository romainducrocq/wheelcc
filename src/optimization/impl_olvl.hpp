#ifndef _OPTIMIZATION_IMPL_OLVL_H
#define _OPTIMIZATION_IMPL_OLVL_H

#ifdef __OPTIM_LEVEL__
#if __OPTIM_LEVEL__ >= 1 && __OPTIM_LEVEL__ <= 2

#define GET_INSTR(X) (*ctx->p_instrs)[X]
#define GET_CFG_BLOCK(X) ctx->cfg->blocks[X]

#if __OPTIM_LEVEL__ == 1
typedef TULong mask_t;
typedef TacInstruction AstInstruction;
typedef OptimTacContext* Ctx;
#elif __OPTIM_LEVEL__ == 2
typedef AsmInstruction AstInstruction;
typedef RegAllocContext* Ctx;
#endif

struct ControlFlowBlock {
    size_t size;
    size_t instrs_front_idx;
    size_t instrs_back_idx;
    vector_t(size_t) pred_ids;
    vector_t(size_t) succ_ids;
};

struct ControlFlowGraph {
    size_t entry_id;
    size_t exit_id;
    vector_t(size_t) entry_succ_ids;
    vector_t(size_t) exit_pred_ids;
    vector_t(bool) reaching_code;
    vector_t(ControlFlowBlock) blocks;
    std::unordered_map<TIdentifier, size_t> identifier_id_map;
};

struct DataFlowAnalysis {
    size_t set_size;
    size_t mask_size;
    size_t incoming_idx;
    size_t static_idx;
    vector_t(size_t) open_data_map;
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

static void set_instr(Ctx ctx, std::unique_ptr<AstInstruction>&& instr, size_t instr_idx) {
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

static bool find_size_t(const vector_t(size_t) xs, size_t x) {
    for (size_t i = 0; i < vec_size(xs); ++i) {
        if (xs[i] == x) {
            return true;
        }
    }
    return false;
}

static void cfg_add_edge(vector_t(size_t) * succ_ids, vector_t(size_t) * pred_ids, size_t succ_id, size_t pred_id) {
    if (!find_size_t(*succ_ids, succ_id)) {
        vec_push_back(*succ_ids, succ_id);
    }
    if (!find_size_t(*pred_ids, pred_id)) {
        vec_push_back(*pred_ids, pred_id);
    }
}

static void cfg_add_succ_edge(Ctx ctx, size_t block_id, size_t succ_id) {
    if (succ_id < ctx->cfg->exit_id) {
        cfg_add_edge(&GET_CFG_BLOCK(block_id).succ_ids, &GET_CFG_BLOCK(succ_id).pred_ids, succ_id, block_id);
    }
    else if (succ_id == ctx->cfg->exit_id) {
        cfg_add_edge(&GET_CFG_BLOCK(block_id).succ_ids, &ctx->cfg->exit_pred_ids, succ_id, block_id);
    }
    else {
        THROW_ABORT;
    }
}

static void cfg_add_pred_edge(Ctx ctx, size_t block_id, size_t pred_id) {
    if (pred_id < ctx->cfg->exit_id) {
        cfg_add_edge(&GET_CFG_BLOCK(pred_id).succ_ids, &GET_CFG_BLOCK(block_id).pred_ids, block_id, pred_id);
    }
    else if (pred_id == ctx->cfg->entry_id) {
        cfg_add_edge(&ctx->cfg->entry_succ_ids, &GET_CFG_BLOCK(block_id).pred_ids, block_id, pred_id);
    }
    else {
        THROW_ABORT;
    }
}

static void cfg_rm_edge(
    vector_t(size_t) * succ_ids, vector_t(size_t) * pred_ids, size_t succ_id, size_t pred_id, bool is_reachable) {
    if (is_reachable) {
        for (size_t i = vec_size(*succ_ids); i-- > 0;) {
            if ((*succ_ids)[i] == succ_id) {
                vec_remove_swap(*succ_ids, i);
                break;
            }
        }
    }
    for (size_t i = vec_size(*pred_ids); i-- > 0;) {
        if ((*pred_ids)[i] == pred_id) {
            vec_remove_swap(*pred_ids, i);
            break;
        }
    }
}

static void cfg_rm_succ_edge(Ctx ctx, size_t block_id, size_t succ_id, bool is_reachable) {
    if (succ_id < ctx->cfg->exit_id) {
        cfg_rm_edge(
            &GET_CFG_BLOCK(block_id).succ_ids, &GET_CFG_BLOCK(succ_id).pred_ids, succ_id, block_id, is_reachable);
    }
    else if (succ_id == ctx->cfg->exit_id) {
        cfg_rm_edge(&GET_CFG_BLOCK(block_id).succ_ids, &ctx->cfg->exit_pred_ids, succ_id, block_id, is_reachable);
    }
    else {
        THROW_ABORT;
    }
}

static void cfg_rm_pred_edge(Ctx ctx, size_t block_id, size_t pred_id) {
    if (pred_id < ctx->cfg->exit_id) {
        cfg_rm_edge(&GET_CFG_BLOCK(pred_id).succ_ids, &GET_CFG_BLOCK(block_id).pred_ids, block_id, pred_id, true);
    }
    else if (pred_id == ctx->cfg->entry_id) {
        cfg_rm_edge(&ctx->cfg->entry_succ_ids, &GET_CFG_BLOCK(block_id).pred_ids, block_id, pred_id, true);
    }
    else {
        THROW_ABORT;
    }
}

static void cfg_rm_empty_block(Ctx ctx, size_t block_id, bool is_reachable) {
    for (size_t i = 0; i < vec_size(GET_CFG_BLOCK(block_id).succ_ids); ++i) {
        size_t succ_id = GET_CFG_BLOCK(block_id).succ_ids[i];
        if (is_reachable) {
            for (size_t j = 0; j < vec_size(GET_CFG_BLOCK(block_id).pred_ids); ++j) {
                size_t pred_id = GET_CFG_BLOCK(block_id).pred_ids[j];
                if (pred_id == ctx->cfg->entry_id) {
                    cfg_add_pred_edge(ctx, succ_id, pred_id);
                }
                else {
                    cfg_add_succ_edge(ctx, pred_id, succ_id);
                }
            }
        }
        cfg_rm_succ_edge(ctx, block_id, succ_id, is_reachable);
    }
    if (is_reachable) {
        for (size_t i = 0; i < vec_size(GET_CFG_BLOCK(block_id).pred_ids); ++i) {
            size_t pred_id = GET_CFG_BLOCK(block_id).pred_ids[i];
            cfg_rm_pred_edge(ctx, block_id, pred_id);
        }
    }
    GET_CFG_BLOCK(block_id).instrs_front_idx = ctx->cfg->exit_id;
    GET_CFG_BLOCK(block_id).instrs_back_idx = ctx->cfg->exit_id;
}

static void cfg_rm_block_instr(Ctx ctx, size_t instr_idx, size_t block_id) {
    if (GET_INSTR(instr_idx)) {
        set_instr(ctx, nullptr, instr_idx);
        GET_CFG_BLOCK(block_id).size--;
        if (GET_CFG_BLOCK(block_id).size == 0) {
            cfg_rm_empty_block(ctx, block_id, true);
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
static void cfg_init_label_block(Ctx ctx, TacLabel* node) {
    ctx->cfg->identifier_id_map[node->name] = vec_size(ctx->cfg->blocks) - 1;
}
#elif __OPTIM_LEVEL__ == 2
static void cfg_init_label_block(Ctx ctx, AsmLabel* node) {
    ctx->cfg->identifier_id_map[node->name] = vec_size(ctx->cfg->blocks) - 1;
}
#endif

static void cfg_init_block(Ctx ctx, size_t instr_idx, size_t& instrs_back_idx) {
    AstInstruction* node = GET_INSTR(instr_idx).get();
    switch (node->type()) {
#if __OPTIM_LEVEL__ == 1
        case AST_TacLabel_t:
#elif __OPTIM_LEVEL__ == 2
        case AST_AsmLabel_t:
#endif
        {
            if (instrs_back_idx != vec_size(*ctx->p_instrs)) {
                vec_back(ctx->cfg->blocks).instrs_back_idx = instrs_back_idx;
                ControlFlowBlock block = {0, instr_idx, 0, vec_new(), vec_new()};
                vec_push_back(ctx->cfg->blocks, block);
            }
#if __OPTIM_LEVEL__ == 1
            cfg_init_label_block(ctx, static_cast<TacLabel*>(node));
#elif __OPTIM_LEVEL__ == 2
            cfg_init_label_block(ctx, static_cast<AsmLabel*>(node));
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
            vec_back(ctx->cfg->blocks).instrs_back_idx = instr_idx;
            instrs_back_idx = vec_size(*ctx->p_instrs);
            break;
        }
        default: {
            instrs_back_idx = instr_idx;
            break;
        }
    }
}

#if __OPTIM_LEVEL__ == 1
static void cfg_init_jump_edges(Ctx ctx, TacJump* node, size_t block_id) {
    cfg_add_succ_edge(ctx, block_id, ctx->cfg->identifier_id_map[node->target]);
}

static void cfg_init_jmp_eq_0_edges(Ctx ctx, TacJumpIfZero* node, size_t block_id) {
    cfg_add_succ_edge(ctx, block_id, ctx->cfg->identifier_id_map[node->target]);
    cfg_add_succ_edge(ctx, block_id, block_id + 1);
}

static void cfg_init_jmp_ne_0_edges(Ctx ctx, TacJumpIfNotZero* node, size_t block_id) {
    cfg_add_succ_edge(ctx, block_id, ctx->cfg->identifier_id_map[node->target]);
    cfg_add_succ_edge(ctx, block_id, block_id + 1);
}
#elif __OPTIM_LEVEL__ == 2
static void cfg_init_jmp_edges(Ctx ctx, AsmJmp* node, size_t block_id) {
    cfg_add_succ_edge(ctx, block_id, ctx->cfg->identifier_id_map[node->target]);
}

static void cfg_init_jmp_cc_edges(Ctx ctx, AsmJmpCC* node, size_t block_id) {
    cfg_add_succ_edge(ctx, block_id, ctx->cfg->identifier_id_map[node->target]);
    cfg_add_succ_edge(ctx, block_id, block_id + 1);
}
#endif

static void cfg_init_edges(Ctx ctx, size_t block_id) {
    AstInstruction* node = GET_INSTR(GET_CFG_BLOCK(block_id).instrs_back_idx).get();
    switch (node->type()) {
#if __OPTIM_LEVEL__ == 1
        case AST_TacReturn_t:
#elif __OPTIM_LEVEL__ == 2
        case AST_AsmRet_t:
#endif
            cfg_add_succ_edge(ctx, block_id, ctx->cfg->exit_id);
            break;
#if __OPTIM_LEVEL__ == 1
        case AST_TacJump_t:
            cfg_init_jump_edges(ctx, static_cast<TacJump*>(node), block_id);
            break;
        case AST_TacJumpIfZero_t:
            cfg_init_jmp_eq_0_edges(ctx, static_cast<TacJumpIfZero*>(node), block_id);
            break;
        case AST_TacJumpIfNotZero_t:
            cfg_init_jmp_ne_0_edges(ctx, static_cast<TacJumpIfNotZero*>(node), block_id);
            break;
#elif __OPTIM_LEVEL__ == 2
        case AST_AsmJmp_t:
            cfg_init_jmp_edges(ctx, static_cast<AsmJmp*>(node), block_id);
            break;
        case AST_AsmJmpCC_t:
            cfg_init_jmp_cc_edges(ctx, static_cast<AsmJmpCC*>(node), block_id);
            break;
#endif
        default:
            cfg_add_succ_edge(ctx, block_id, block_id + 1);
            break;
    }
}

static void init_control_flow_graph(Ctx ctx) {
    for (size_t block_id = 0; block_id < vec_size(ctx->cfg->blocks); ++block_id) {
        vec_delete(GET_CFG_BLOCK(block_id).pred_ids);
        vec_delete(GET_CFG_BLOCK(block_id).succ_ids);
    }
    vec_clear(ctx->cfg->blocks);
    ctx->cfg->identifier_id_map.clear();
    {
        size_t instrs_back_idx = vec_size(*ctx->p_instrs);
        for (size_t instr_idx = 0; instr_idx < vec_size(*ctx->p_instrs); ++instr_idx) {
            if (GET_INSTR(instr_idx)) {
                if (instrs_back_idx == vec_size(*ctx->p_instrs)) {
                    ControlFlowBlock block = {0, instr_idx, 0, vec_new(), vec_new()};
                    vec_push_back(ctx->cfg->blocks, block);
                }
                cfg_init_block(ctx, instr_idx, instrs_back_idx);
                vec_back(ctx->cfg->blocks).size++;
            }
        }
        if (instrs_back_idx != vec_size(*ctx->p_instrs)) {
            vec_back(ctx->cfg->blocks).instrs_back_idx = instrs_back_idx;
        }
    }

    ctx->cfg->exit_id = vec_size(ctx->cfg->blocks);
    ctx->cfg->entry_id = ctx->cfg->exit_id + 1;
    vec_clear(ctx->cfg->entry_succ_ids);
    vec_clear(ctx->cfg->exit_pred_ids);
    if (!vec_empty(ctx->cfg->blocks)) {
        cfg_add_pred_edge(ctx, 0, ctx->cfg->entry_id);
        for (size_t block_id = 0; block_id < vec_size(ctx->cfg->blocks); ++block_id) {
            cfg_init_edges(ctx, block_id);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Data flow analysis

static bool mask_get(TULong mask, size_t bit) {
    if (bit > 63) {
        bit %= 64;
    }
    return (mask & (((TULong)1ul) << bit)) > 0;
}

static void mask_set(TULong& mask, size_t bit, bool value) {
    if (bit > 63) {
        bit %= 64;
    }
    if (value) {
        mask |= ((TULong)1ul) << bit;
    }
    else {
        mask &= ~(((TULong)1ul) << bit);
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

static bool is_transfer_instr(Ctx ctx, size_t instr_idx
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
static size_t get_dfa_data_idx(Ctx ctx, size_t instr_idx) {
    for (size_t i = 0; i < ctx->dfa->set_size; ++i) {
        if (ctx->dfa_o1->data_idx_map[i] == instr_idx) {
            return i;
        }
    }
    THROW_ABORT;
}

static TacInstruction* get_dfa_bak_instr(Ctx ctx, size_t i) {
    if (ctx->cfg->reaching_code[i]) {
        if (ctx->dfa_o1->bak_instrs[i]) {
            return ctx->dfa_o1->bak_instrs[i].get();
        }
        else {
            THROW_ABORT;
        }
    }
    else if (GET_DFA_INSTR(i)) {
        return GET_DFA_INSTR(i).get();
    }
    else {
        THROW_ABORT;
    }
}

static bool set_dfa_bak_instr(Ctx ctx, size_t instr_idx, size_t& i) {
    i = get_dfa_data_idx(ctx, instr_idx);
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
static bool prop_transfer_reach_copies(Ctx ctx, size_t instr_idx, size_t next_instr_idx);
static void elim_transfer_live_values(Ctx ctx, size_t instr_idx, size_t next_instr_idx);
#elif __OPTIM_LEVEL__ == 2
static void infer_transfer_live_regs(Ctx ctx, size_t instr_idx, size_t next_instr_idx);
#endif

#if __OPTIM_LEVEL__ == 1
static size_t dfa_forward_transfer_block(Ctx ctx, size_t instr_idx, size_t block_id) {
    for (size_t next_instr_idx = instr_idx + 1; next_instr_idx <= GET_CFG_BLOCK(block_id).instrs_back_idx;
         ++next_instr_idx) {
        if (GET_INSTR(next_instr_idx) && is_transfer_instr(ctx, next_instr_idx, false)) {
            for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
                GET_DFA_INSTR_SET_MASK(next_instr_idx, i) = GET_DFA_INSTR_SET_MASK(instr_idx, i);
            }
            if (!prop_transfer_reach_copies(ctx, instr_idx, next_instr_idx)) {
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
    if (!prop_transfer_reach_copies(ctx, instr_idx, ctx->dfa->incoming_idx)) {
        for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
            GET_DFA_INSTR_SET_MASK(ctx->dfa->incoming_idx, i) = GET_DFA_INSTR_SET_MASK(instr_idx, i);
        }
    }
    return instr_idx;
}
#endif

static size_t dfa_backward_transfer_block(Ctx ctx, size_t instr_idx, size_t block_id) {
    if (instr_idx > 0) {
        for (size_t next_instr_idx = instr_idx; next_instr_idx-- > GET_CFG_BLOCK(block_id).instrs_front_idx;) {
            if (GET_INSTR(next_instr_idx)
                && is_transfer_instr(ctx, next_instr_idx
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
                    (ctx, instr_idx, next_instr_idx);
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
        (ctx, instr_idx, ctx->dfa->incoming_idx);
    return instr_idx;
}

static bool dfa_after_meet_block(Ctx ctx, size_t block_id) {
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
static bool dfa_forward_meet_block(Ctx ctx, size_t block_id) {
    size_t instr_idx = GET_CFG_BLOCK(block_id).instrs_front_idx;
    for (; instr_idx <= GET_CFG_BLOCK(block_id).instrs_back_idx; ++instr_idx) {
        if (GET_INSTR(instr_idx) && is_transfer_instr(ctx, instr_idx, false)) {
            goto Lelse;
        }
    }
    instr_idx = ctx->dfa->incoming_idx;
Lelse:
    for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
        GET_DFA_INSTR_SET_MASK(instr_idx, i) = MASK_TRUE;
    }

    for (size_t i = 0; i < vec_size(GET_CFG_BLOCK(block_id).pred_ids); ++i) {
        size_t pred_id = GET_CFG_BLOCK(block_id).pred_ids[i];
        if (pred_id < ctx->cfg->exit_id) {
            for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
                GET_DFA_INSTR_SET_MASK(instr_idx, j) &= GET_DFA_BLOCK_SET_MASK(pred_id, j);
            }
        }
        else if (pred_id == ctx->cfg->entry_id) {
            for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
                GET_DFA_INSTR_SET_MASK(instr_idx, j) = MASK_FALSE;
            }
            break;
        }
        else {
            THROW_ABORT;
        }
    }

    if (instr_idx < ctx->dfa->incoming_idx) {
        dfa_forward_transfer_block(ctx, instr_idx, block_id);
    }
    else {
        THROW_ABORT_IF(instr_idx != ctx->dfa->incoming_idx);
    }

    return dfa_after_meet_block(ctx, block_id);
}
#endif

static bool dfa_backward_meet_block(Ctx ctx, size_t block_id) {
    size_t instr_idx = GET_CFG_BLOCK(block_id).instrs_back_idx + 1;
    while (instr_idx-- > GET_CFG_BLOCK(block_id).instrs_front_idx) {
        if (GET_INSTR(instr_idx)
            && is_transfer_instr(ctx, instr_idx
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

    for (size_t i = 0; i < vec_size(GET_CFG_BLOCK(block_id).succ_ids); ++i) {
        size_t succ_id = GET_CFG_BLOCK(block_id).succ_ids[i];
        if (succ_id < ctx->cfg->exit_id) {
            for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
                GET_DFA_INSTR_SET_MASK(instr_idx, j) |= GET_DFA_BLOCK_SET_MASK(succ_id, j);
            }
        }
        else if (succ_id == ctx->cfg->exit_id) {
            for (size_t j = 0; j < ctx->dfa->mask_size; ++j) {
                GET_DFA_INSTR_SET_MASK(instr_idx, j) = GET_DFA_INSTR_SET_MASK(ctx->dfa->static_idx, j);
            }
            break;
        }
        else {
            THROW_ABORT;
        }
    }

    if (instr_idx < ctx->dfa->incoming_idx) {
        dfa_backward_transfer_block(ctx, instr_idx, block_id);
    }
    else {
        THROW_ABORT_IF(instr_idx != ctx->dfa->incoming_idx);
    }

    return dfa_after_meet_block(ctx, block_id);
}

#if __OPTIM_LEVEL__ == 1
static void dfa_forward_iter_alg(Ctx ctx) {
    size_t open_data_map_size = vec_size(ctx->cfg->blocks);
    for (size_t i = 0; i < open_data_map_size; ++i) {
        size_t block_id = ctx->dfa->open_data_map[i];
        if (block_id == ctx->cfg->exit_id) {
            continue;
        }

        bool is_fixed_point = dfa_forward_meet_block(ctx, block_id);
        if (!is_fixed_point) {
            for (size_t j = 0; j < vec_size(GET_CFG_BLOCK(block_id).succ_ids); ++j) {
                size_t succ_id = GET_CFG_BLOCK(block_id).succ_ids[j];
                if (succ_id < ctx->cfg->exit_id) {
                    for (size_t k = i + 1; k < open_data_map_size; ++k) {
                        if (succ_id == ctx->dfa->open_data_map[k]) {
                            goto Lelse;
                        }
                    }
                    if (open_data_map_size < vec_size(ctx->dfa->open_data_map)) {
                        ctx->dfa->open_data_map[open_data_map_size] = succ_id;
                    }
                    else {
                        vec_push_back(ctx->dfa->open_data_map, succ_id);
                    }
                    open_data_map_size++;
                Lelse:;
                }
                else {
                    THROW_ABORT_IF(succ_id != ctx->cfg->exit_id);
                }
            }
        }
    }
}
#endif

static void dfa_iter_alg(Ctx ctx) {
    size_t open_data_map_size = vec_size(ctx->cfg->blocks);
    for (size_t i = 0; i < open_data_map_size; ++i) {
        size_t block_id = ctx->dfa->open_data_map[i];
        if (block_id == ctx->cfg->exit_id) {
            continue;
        }

        bool is_fixed_point = dfa_backward_meet_block(ctx, block_id);
        if (!is_fixed_point) {
            for (size_t j = 0; j < vec_size(GET_CFG_BLOCK(block_id).pred_ids); ++j) {
                size_t pred_id = GET_CFG_BLOCK(block_id).pred_ids[j];
                if (pred_id < ctx->cfg->exit_id) {
                    for (size_t k = i + 1; k < open_data_map_size; ++k) {
                        if (pred_id == ctx->dfa->open_data_map[k]) {
                            goto Lelse;
                        }
                    }
                    if (open_data_map_size < vec_size(ctx->dfa->open_data_map)) {
                        ctx->dfa->open_data_map[open_data_map_size] = pred_id;
                    }
                    else {
                        vec_push_back(ctx->dfa->open_data_map, pred_id);
                    }
                    open_data_map_size++;
                Lelse:;
                }
                else {
                    THROW_ABORT_IF(pred_id != ctx->cfg->entry_id);
                }
            }
        }
    }
}

#if __OPTIM_LEVEL__ == 1
static void dfa_forward_open_block(Ctx ctx, size_t block_id, size_t& i);
#endif
static void dfa_backward_open_block(Ctx ctx, size_t block_id, size_t& i);

#if __OPTIM_LEVEL__ == 1
static void dfa_forward_succ_open_block(Ctx ctx, size_t block_id, size_t& i) {
    for (size_t j = 0; j < vec_size(GET_CFG_BLOCK(block_id).succ_ids); ++j) {
        dfa_forward_open_block(ctx, GET_CFG_BLOCK(block_id).succ_ids[j], i);
    }
}
#endif

static void dfa_backward_succ_open_block(Ctx ctx, size_t block_id, size_t& i) {
    for (size_t j = 0; j < vec_size(GET_CFG_BLOCK(block_id).succ_ids); ++j) {
        dfa_backward_open_block(ctx, GET_CFG_BLOCK(block_id).succ_ids[j], i);
    }
}

#if __OPTIM_LEVEL__ == 1
static void dfa_forward_open_block(Ctx ctx, size_t block_id, size_t& i) {
    if (block_id < ctx->cfg->exit_id && !ctx->cfg->reaching_code[block_id]) {
        ctx->cfg->reaching_code[block_id] = true;
        dfa_forward_succ_open_block(ctx, block_id, i);
        i--;
        ctx->dfa->open_data_map[i] = block_id;
    }
}
#endif

static void dfa_backward_open_block(Ctx ctx, size_t block_id, size_t& i) {
    if (block_id < ctx->cfg->exit_id && !ctx->cfg->reaching_code[block_id]) {
        ctx->cfg->reaching_code[block_id] = true;
        dfa_backward_succ_open_block(ctx, block_id, i);
        ctx->dfa->open_data_map[i] = block_id;
        i++;
    }
}

static bool is_aliased_name(Ctx ctx, TIdentifier name) {
    return ctx->frontend->symbol_table[name]->attrs->type() == AST_StaticAttr_t
           || ctx->frontend->addressed_set.find(name) != ctx->frontend->addressed_set.end();
}

#if __OPTIM_LEVEL__ == 1
static void dfa_add_aliased_value(Ctx ctx, TacValue* node) {
    if (node->type() == AST_TacVariable_t) {
        ctx->frontend->addressed_set.insert(static_cast<TacVariable*>(node)->name);
    }
}

static bool is_same_value(TacValue* node_1, TacValue* node_2);

static bool prop_add_data_idx(Ctx ctx, TacCopy* node, size_t instr_idx, size_t block_id) {
    THROW_ABORT_IF(node->dst->type() != AST_TacVariable_t);
    if (is_same_value(node->src.get(), node->dst.get())) {
        cfg_rm_block_instr(ctx, instr_idx, block_id);
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

static void elim_add_data_name(Ctx ctx, TIdentifier name) {
    if (ctx->cfg->identifier_id_map.find(name) == ctx->cfg->identifier_id_map.end()) {
        ctx->cfg->identifier_id_map[name] = ctx->dfa->set_size;
        ctx->dfa->set_size++;
    }
}

static void elim_add_data_value(Ctx ctx, TacValue* node) {
    if (node->type() == AST_TacVariable_t) {
        elim_add_data_name(ctx, static_cast<TacVariable*>(node)->name);
    }
}
#elif __OPTIM_LEVEL__ == 2
static void infer_add_data_name(Ctx ctx, TIdentifier name) {
    if (!is_aliased_name(ctx, name) && ctx->cfg->identifier_id_map.find(name) == ctx->cfg->identifier_id_map.end()) {
        ctx->cfg->identifier_id_map[name] = REGISTER_MASK_SIZE + ctx->dfa->set_size;
        ctx->dfa->set_size++;
    }
}

static void infer_add_data_op(Ctx ctx, AsmOperand* node) {
    if (node->type() == AST_AsmPseudo_t) {
        infer_add_data_name(ctx, static_cast<AsmPseudo*>(node)->name);
    }
}
#endif

static bool init_data_flow_analysis(Ctx ctx,
#if __OPTIM_LEVEL__ == 1
    bool is_store_elim, bool is_addressed_set
#elif __OPTIM_LEVEL__ == 2
    TIdentifier fun_name
#endif
) {
    ctx->dfa->set_size = 0;
    ctx->dfa->incoming_idx = vec_size(*ctx->p_instrs);

    if (vec_size(ctx->dfa->open_data_map) < vec_size(ctx->cfg->blocks)) {
        vec_resize(ctx->dfa->open_data_map, vec_size(ctx->cfg->blocks));
    }
    {
        size_t i;
#if __OPTIM_LEVEL__ == 1
        i = is_store_elim ? 3 : 1;
#elif __OPTIM_LEVEL__ == 2
        i = 2;
#endif
        if (ctx->dfa->instr_idx_map.size() < vec_size(*ctx->p_instrs) + i) {
            ctx->dfa->instr_idx_map.resize(vec_size(*ctx->p_instrs) + i);
        }
    }
    if (vec_size(ctx->cfg->reaching_code) < vec_size(ctx->cfg->blocks)) {
        vec_resize(ctx->cfg->reaching_code, vec_size(ctx->cfg->blocks));
    }
    memset(ctx->cfg->reaching_code, false, sizeof(bool) * vec_size(ctx->cfg->blocks));

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
        ctx->frontend->addressed_set.clear();
    }
#endif
    for (size_t block_id = 0; block_id < vec_size(ctx->cfg->blocks); ++block_id) {
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
                                elim_add_data_value(ctx, p_node->val.get());
                            }
                            break;
                        }
                        case AST_TacSignExtend_t: {
                            if (is_store_elim) {
                                TacSignExtend* p_node = static_cast<TacSignExtend*>(node);
                                elim_add_data_value(ctx, p_node->src.get());
                                elim_add_data_value(ctx, p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacTruncate_t: {
                            if (is_store_elim) {
                                TacTruncate* p_node = static_cast<TacTruncate*>(node);
                                elim_add_data_value(ctx, p_node->src.get());
                                elim_add_data_value(ctx, p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacZeroExtend_t: {
                            if (is_store_elim) {
                                TacZeroExtend* p_node = static_cast<TacZeroExtend*>(node);
                                elim_add_data_value(ctx, p_node->src.get());
                                elim_add_data_value(ctx, p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacDoubleToInt_t: {
                            if (is_store_elim) {
                                TacDoubleToInt* p_node = static_cast<TacDoubleToInt*>(node);
                                elim_add_data_value(ctx, p_node->src.get());
                                elim_add_data_value(ctx, p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacDoubleToUInt_t: {
                            if (is_store_elim) {
                                TacDoubleToUInt* p_node = static_cast<TacDoubleToUInt*>(node);
                                elim_add_data_value(ctx, p_node->src.get());
                                elim_add_data_value(ctx, p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacIntToDouble_t: {
                            if (is_store_elim) {
                                TacIntToDouble* p_node = static_cast<TacIntToDouble*>(node);
                                elim_add_data_value(ctx, p_node->src.get());
                                elim_add_data_value(ctx, p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacUIntToDouble_t: {
                            if (is_store_elim) {
                                TacUIntToDouble* p_node = static_cast<TacUIntToDouble*>(node);
                                elim_add_data_value(ctx, p_node->src.get());
                                elim_add_data_value(ctx, p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacFunCall_t: {
                            if (is_store_elim) {
                                TacFunCall* p_node = static_cast<TacFunCall*>(node);
                                for (size_t i = 0; i < vec_size(p_node->args); ++i) {
                                    elim_add_data_value(ctx, p_node->args[i].get());
                                }
                                if (p_node->dst) {
                                    elim_add_data_value(ctx, p_node->dst.get());
                                }
                            }
                            break;
                        }
                        case AST_TacUnary_t: {
                            if (is_store_elim) {
                                TacUnary* p_node = static_cast<TacUnary*>(node);
                                elim_add_data_value(ctx, p_node->src.get());
                                elim_add_data_value(ctx, p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacBinary_t: {
                            if (is_store_elim) {
                                TacBinary* p_node = static_cast<TacBinary*>(node);
                                elim_add_data_value(ctx, p_node->src1.get());
                                elim_add_data_value(ctx, p_node->src2.get());
                                elim_add_data_value(ctx, p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacCopy_t: {
                            TacCopy* p_node = static_cast<TacCopy*>(node);
                            if (is_copy_prop) {
                                if (!prop_add_data_idx(ctx, p_node, instr_idx, block_id)) {
                                    goto Lcontinue;
                                }
                            }
                            else {
                                elim_add_data_value(ctx, p_node->src.get());
                                elim_add_data_value(ctx, p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacGetAddress_t: {
                            TacGetAddress* p_node = static_cast<TacGetAddress*>(node);
                            if (is_store_elim) {
                                elim_add_data_value(ctx, p_node->src.get());
                                elim_add_data_value(ctx, p_node->dst.get());
                            }
                            if (is_addressed_set) {
                                dfa_add_aliased_value(ctx, p_node->src.get());
                            }
                            break;
                        }
                        case AST_TacLoad_t: {
                            if (is_store_elim) {
                                TacLoad* p_node = static_cast<TacLoad*>(node);
                                elim_add_data_value(ctx, p_node->src_ptr.get());
                                elim_add_data_value(ctx, p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacStore_t: {
                            if (is_store_elim) {
                                TacStore* p_node = static_cast<TacStore*>(node);
                                elim_add_data_value(ctx, p_node->src.get());
                                elim_add_data_value(ctx, p_node->dst_ptr.get());
                            }
                            break;
                        }
                        case AST_TacAddPtr_t: {
                            if (is_store_elim) {
                                TacAddPtr* p_node = static_cast<TacAddPtr*>(node);
                                elim_add_data_value(ctx, p_node->src_ptr.get());
                                elim_add_data_value(ctx, p_node->idx.get());
                                elim_add_data_value(ctx, p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacCopyToOffset_t: {
                            if (is_store_elim) {
                                TacCopyToOffset* p_node = static_cast<TacCopyToOffset*>(node);
                                elim_add_data_name(ctx, p_node->dst_name);
                                elim_add_data_value(ctx, p_node->src.get());
                            }
                            break;
                        }
                        case AST_TacCopyFromOffset_t: {
                            if (is_store_elim) {
                                TacCopyFromOffset* p_node = static_cast<TacCopyFromOffset*>(node);
                                elim_add_data_name(ctx, p_node->src_name);
                                elim_add_data_value(ctx, p_node->dst.get());
                            }
                            break;
                        }
                        case AST_TacJumpIfZero_t: {
                            if (is_copy_prop) {
                                goto Lcontinue;
                            }
                            elim_add_data_value(ctx, static_cast<TacJumpIfZero*>(node)->condition.get());
                            break;
                        }
                        case AST_TacJumpIfNotZero_t: {
                            if (is_copy_prop) {
                                goto Lcontinue;
                            }
                            elim_add_data_value(ctx, static_cast<TacJumpIfNotZero*>(node)->condition.get());
                            break;
                        }
#elif __OPTIM_LEVEL__ == 2
                        case AST_AsmMov_t: {
                            AsmMov* p_node = static_cast<AsmMov*>(node);
                            infer_add_data_op(ctx, p_node->src.get());
                            infer_add_data_op(ctx, p_node->dst.get());
                            break;
                        }
                        case AST_AsmMovSx_t: {
                            AsmMovSx* p_node = static_cast<AsmMovSx*>(node);
                            infer_add_data_op(ctx, p_node->src.get());
                            infer_add_data_op(ctx, p_node->dst.get());
                            break;
                        }
                        case AST_AsmMovZeroExtend_t: {
                            AsmMovZeroExtend* p_node = static_cast<AsmMovZeroExtend*>(node);
                            infer_add_data_op(ctx, p_node->src.get());
                            infer_add_data_op(ctx, p_node->dst.get());
                            break;
                        }
                        case AST_AsmLea_t: {
                            AsmLea* p_node = static_cast<AsmLea*>(node);
                            infer_add_data_op(ctx, p_node->src.get());
                            infer_add_data_op(ctx, p_node->dst.get());
                            break;
                        }
                        case AST_AsmCvttsd2si_t: {
                            AsmCvttsd2si* p_node = static_cast<AsmCvttsd2si*>(node);
                            infer_add_data_op(ctx, p_node->src.get());
                            infer_add_data_op(ctx, p_node->dst.get());
                            break;
                        }
                        case AST_AsmCvtsi2sd_t: {
                            AsmCvtsi2sd* p_node = static_cast<AsmCvtsi2sd*>(node);
                            infer_add_data_op(ctx, p_node->src.get());
                            infer_add_data_op(ctx, p_node->dst.get());
                            break;
                        }
                        case AST_AsmUnary_t:
                            infer_add_data_op(ctx, static_cast<AsmUnary*>(node)->dst.get());
                            break;
                        case AST_AsmBinary_t: {
                            AsmBinary* p_node = static_cast<AsmBinary*>(node);
                            infer_add_data_op(ctx, p_node->src.get());
                            infer_add_data_op(ctx, p_node->dst.get());
                            break;
                        }
                        case AST_AsmCmp_t: {
                            AsmCmp* p_node = static_cast<AsmCmp*>(node);
                            infer_add_data_op(ctx, p_node->src.get());
                            infer_add_data_op(ctx, p_node->dst.get());
                            break;
                        }
                        case AST_AsmIdiv_t:
                            infer_add_data_op(ctx, static_cast<AsmIdiv*>(node)->src.get());
                            break;
                        case AST_AsmDiv_t:
                            infer_add_data_op(ctx, static_cast<AsmDiv*>(node)->src.get());
                            break;
                        case AST_AsmSetCC_t:
                            infer_add_data_op(ctx, static_cast<AsmSetCC*>(node)->dst.get());
                            break;
                        case AST_AsmPush_t:
                            infer_add_data_op(ctx, static_cast<AsmPush*>(node)->src.get());
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
    size_t blocks_mask_sets_size = ctx->dfa->mask_size * vec_size(ctx->cfg->blocks);

    if (ctx->dfa->blocks_mask_sets.size() < blocks_mask_sets_size) {
        ctx->dfa->blocks_mask_sets.resize(blocks_mask_sets_size);
    }
    if (ctx->dfa->instrs_mask_sets.size() < instrs_mask_sets_size) {
        ctx->dfa->instrs_mask_sets.resize(instrs_mask_sets_size);
    }

#if __OPTIM_LEVEL__ == 1
    if (is_copy_prop) {
        size_t i = vec_size(ctx->cfg->blocks);
        for (size_t j = 0; j < vec_size(ctx->cfg->entry_succ_ids); ++j) {
            size_t succ_id = ctx->cfg->entry_succ_ids[j];
            if (!ctx->cfg->reaching_code[succ_id]) {
                dfa_forward_open_block(ctx, succ_id, i);
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

        if (vec_size(ctx->cfg->reaching_code) < ctx->dfa->set_size) {
            vec_resize(ctx->cfg->reaching_code, ctx->dfa->set_size);
        }
        if (ctx->dfa_o1->bak_instrs.size() < ctx->dfa->set_size) {
            ctx->dfa_o1->bak_instrs.resize(ctx->dfa->set_size);
        }

        memset(ctx->cfg->reaching_code, false, sizeof(bool) * ctx->dfa->set_size);

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
            for (size_t j = 0; j < blocks_mask_sets_size; ++j) {
                ctx->dfa->blocks_mask_sets[j] = mask_true_back;
            }
        }
    }
    else {
#endif
        size_t i = 0;
        for (size_t j = 0; j < vec_size(ctx->cfg->entry_succ_ids); ++j) {
            size_t succ_id = ctx->cfg->entry_succ_ids[j];
            if (!ctx->cfg->reaching_code[succ_id]) {
                dfa_backward_open_block(ctx, succ_id, i);
            }
        }
        for (; i < vec_size(ctx->cfg->blocks); ++i) {
            ctx->dfa->open_data_map[i] = ctx->cfg->exit_id;
        }

#if __OPTIM_LEVEL__ == 1
        GET_DFA_INSTR_SET_MASK(ctx->dfa->static_idx, 0) = MASK_FALSE;
        GET_DFA_INSTR_SET_MASK(ctx->dfa_o1->addressed_idx, 0) = MASK_FALSE;
#elif __OPTIM_LEVEL__ == 2
    {
        FunType* fun_type = static_cast<FunType*>(ctx->frontend->symbol_table[fun_name]->type_t.get());
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
            if (ctx->frontend->symbol_table[name_id.first]->attrs->type() == AST_StaticAttr_t) {
                SET_DFA_INSTR_SET_AT(ctx->dfa->static_idx, name_id.second, true);
            }
            if (ctx->frontend->addressed_set.find(name_id.first) != ctx->frontend->addressed_set.end()) {
                SET_DFA_INSTR_SET_AT(ctx->dfa_o1->addressed_idx, name_id.second, true);
            }
#elif __OPTIM_LEVEL__ == 2
        ctx->dfa_o2->data_name_map[name_id.second - REGISTER_MASK_SIZE] = name_id.first;
#endif
        }

        memset(ctx->dfa->blocks_mask_sets.data(), MASK_FALSE, sizeof(mask_t) * blocks_mask_sets_size);
#if __OPTIM_LEVEL__ == 1
    }
#endif

    return true;
}

#endif
#endif
#endif
