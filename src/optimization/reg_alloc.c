#include <string.h>

#include "util/c_std.h"
#include "util/throw.h"

#include "ast/back_ast.h"
#include "ast/back_symt.h"
#include "ast/front_symt.h"
#include "ast_t.h" // ast

#include "backend/assembly/registers.h"

#include "optimization/reg_alloc.h"

typedef TULong mask_t;

typedef struct ControlFlowGraph ControlFlowGraph;
typedef struct DataFlowAnalysis DataFlowAnalysis;
typedef struct DataFlowAnalysisO2 DataFlowAnalysisO2;

typedef struct InferenceRegister {
    REGISTER_KIND color;
    REGISTER_KIND reg_kind;
    size_t degree;
    size_t spill_cost;
    mask_t linked_hard_mask;
    vector_t(TIdentifier) linked_pseudo_names;
} InferenceRegister;

PairKeyValue(TIdentifier, InferenceRegister);

typedef struct InferenceGraph {
    size_t k;
    size_t offset;
    mask_t hard_reg_mask;
    vector_t(size_t) unpruned_hard_mask_bits;
    vector_t(TIdentifier) unpruned_pseudo_names;
    hashmap_t(TIdentifier, InferenceRegister) pseudo_reg_map;
} InferenceGraph;

typedef struct RegAllocContext {
    BackEndContext* backend;
    FrontEndContext* frontend;
    // Register allocation
    mask_t callee_saved_reg_mask;
    BackendFun* p_backend_fun;
    InferenceGraph* p_infer_graph;
    REGISTER_KIND reg_color_map[26];
    InferenceRegister hard_regs[26];
    unique_ptr_t(ControlFlowGraph) cfg;
    unique_ptr_t(DataFlowAnalysis) dfa;
    unique_ptr_t(DataFlowAnalysisO2) dfa_o2;
    unique_ptr_t(InferenceGraph) infer_graph;
    unique_ptr_t(InferenceGraph) sse_infer_graph;
    vector_t(unique_ptr_t(AsmInstruction)) * p_instrs;
    // Register coalescing
    bool is_with_coal;
} RegAllocContext;

static void free_InferenceGraph(unique_ptr_t(InferenceGraph) * self) {
    uptr_delete(*self);
    vec_delete((*self)->unpruned_hard_mask_bits);
    vec_delete((*self)->unpruned_pseudo_names);
    for (size_t i = 0; i < map_size((*self)->pseudo_reg_map); ++i) {
        vec_delete(pair_second((*self)->pseudo_reg_map[i]).linked_pseudo_names);
    }
    map_delete((*self)->pseudo_reg_map);
    uptr_free(*self);
}

static unique_ptr_t(InferenceGraph) make_InferenceGraph(bool is_sse) {
    unique_ptr_t(InferenceGraph) self = uptr_new();
    uptr_alloc(InferenceGraph, self);
    self->hard_reg_mask = REGISTER_MASK_FALSE;
    self->unpruned_hard_mask_bits = vec_new();
    self->unpruned_pseudo_names = vec_new();
    self->pseudo_reg_map = map_new();
    if (is_sse) {
        self->k = 14;
        self->offset = 12;
        register_mask_set(&self->hard_reg_mask, REG_Xmm0, true);
        register_mask_set(&self->hard_reg_mask, REG_Xmm1, true);
        register_mask_set(&self->hard_reg_mask, REG_Xmm2, true);
        register_mask_set(&self->hard_reg_mask, REG_Xmm3, true);
        register_mask_set(&self->hard_reg_mask, REG_Xmm4, true);
        register_mask_set(&self->hard_reg_mask, REG_Xmm5, true);
        register_mask_set(&self->hard_reg_mask, REG_Xmm6, true);
        register_mask_set(&self->hard_reg_mask, REG_Xmm7, true);
        register_mask_set(&self->hard_reg_mask, REG_Xmm8, true);
        register_mask_set(&self->hard_reg_mask, REG_Xmm9, true);
        register_mask_set(&self->hard_reg_mask, REG_Xmm10, true);
        register_mask_set(&self->hard_reg_mask, REG_Xmm11, true);
        register_mask_set(&self->hard_reg_mask, REG_Xmm12, true);
        register_mask_set(&self->hard_reg_mask, REG_Xmm13, true);
    }
    else {
        self->k = 12;
        self->offset = 0;
        register_mask_set(&self->hard_reg_mask, REG_Ax, true);
        register_mask_set(&self->hard_reg_mask, REG_Bx, true);
        register_mask_set(&self->hard_reg_mask, REG_Cx, true);
        register_mask_set(&self->hard_reg_mask, REG_Dx, true);
        register_mask_set(&self->hard_reg_mask, REG_Di, true);
        register_mask_set(&self->hard_reg_mask, REG_Si, true);
        register_mask_set(&self->hard_reg_mask, REG_R8, true);
        register_mask_set(&self->hard_reg_mask, REG_R9, true);
        register_mask_set(&self->hard_reg_mask, REG_R12, true);
        register_mask_set(&self->hard_reg_mask, REG_R13, true);
        register_mask_set(&self->hard_reg_mask, REG_R14, true);
        register_mask_set(&self->hard_reg_mask, REG_R15, true);
    }
    return self;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register allocation

#ifndef __OPTIM_LEVEL__
#define __OPTIM_LEVEL__ 2
#undef _OPTIMIZATION_IMPL_OLVL_H
#include "impl_olvl.h" // optimization
#undef __OPTIM_LEVEL__
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Inference graph

static bool is_bitshift_cl(AsmBinary* node) {
    switch (node->binop->type) {
        case AST_AsmBitShiftLeft_t:
        case AST_AsmBitShiftRight_t:
        case AST_AsmBitShrArithmetic_t:
            return node->src->type != AST_AsmImm_t;
        default:
            return false;
    }
}

static bool find_identifier(const vector_t(TIdentifier) xs, TIdentifier x) {
    for (size_t i = 0; i < vec_size(xs); ++i) {
        if (xs[i] == x) {
            return true;
        }
    }
    return false;
}

static void infer_transfer_used_reg(Ctx ctx, REGISTER_KIND reg_kind, size_t next_instr_idx) {
    SET_DFA_INSTR_SET_AT(next_instr_idx, register_mask_bit(reg_kind), true);
}

static void infer_transfer_used_name(Ctx ctx, TIdentifier name, size_t next_instr_idx) {
    if (!is_aliased_name(ctx, name)) {
        size_t i = map_get(ctx->cfg->identifier_id_map, name);
        SET_DFA_INSTR_SET_AT(next_instr_idx, i, true);
    }
}

static void infer_transfer_used_op(Ctx ctx, AsmOperand* node, size_t next_instr_idx) {
    switch (node->type) {
        case AST_AsmRegister_t: {
            REGISTER_KIND reg_kind = register_mask_kind(&node->get._AsmRegister.reg);
            if (reg_kind != REG_Sp) {
                infer_transfer_used_reg(ctx, reg_kind, next_instr_idx);
            }
            break;
        }
        case AST_AsmPseudo_t:
            infer_transfer_used_name(ctx, node->get._AsmPseudo.name, next_instr_idx);
            break;
        case AST_AsmMemory_t: {
            REGISTER_KIND reg_kind = register_mask_kind(&node->get._AsmMemory.reg);
            if (reg_kind != REG_Sp) {
                infer_transfer_used_reg(ctx, reg_kind, next_instr_idx);
            }
            break;
        }
        case AST_AsmIndexed_t: {
            AsmIndexed* p_node = &node->get._AsmIndexed;
            {
                REGISTER_KIND reg_kind = register_mask_kind(&p_node->reg_base);
                infer_transfer_used_reg(ctx, reg_kind, next_instr_idx);
            }
            {
                REGISTER_KIND reg_kind = register_mask_kind(&p_node->reg_index);
                infer_transfer_used_reg(ctx, reg_kind, next_instr_idx);
            }
            break;
        }
        default:
            break;
    }
}

static void infer_transfer_used_call(Ctx ctx, AsmCall* node, size_t next_instr_idx) {
    FunType* fun_type = &map_get(ctx->frontend->symbol_table, node->name)->type_t->get._FunType;
    GET_DFA_INSTR_SET_MASK(next_instr_idx, 0) |= fun_type->param_reg_mask;
}

static void infer_transfer_updated_reg(Ctx ctx, REGISTER_KIND reg_kind, size_t next_instr_idx) {
    SET_DFA_INSTR_SET_AT(next_instr_idx, register_mask_bit(reg_kind), false);
}

static void infer_transfer_updated_name(Ctx ctx, TIdentifier name, size_t next_instr_idx) {
    if (!is_aliased_name(ctx, name)) {
        size_t i = map_get(ctx->cfg->identifier_id_map, name);
        SET_DFA_INSTR_SET_AT(next_instr_idx, i, false);
    }
}

static void infer_transfer_updated_op(Ctx ctx, AsmOperand* node, size_t next_instr_idx) {
    switch (node->type) {
        case AST_AsmRegister_t: {
            REGISTER_KIND reg_kind = register_mask_kind(&node->get._AsmRegister.reg);
            if (reg_kind != REG_Sp) {
                infer_transfer_updated_reg(ctx, reg_kind, next_instr_idx);
            }
            break;
        }
        case AST_AsmPseudo_t:
            infer_transfer_updated_name(ctx, node->get._AsmPseudo.name, next_instr_idx);
            break;
        case AST_AsmMemory_t: {
            REGISTER_KIND reg_kind = register_mask_kind(&node->get._AsmMemory.reg);
            if (reg_kind != REG_Sp) {
                infer_transfer_used_reg(ctx, reg_kind, next_instr_idx);
            }
            break;
        }
        case AST_AsmIndexed_t:
            THROW_ABORT;
        default:
            break;
    }
}

static void infer_transfer_live_regs(Ctx ctx, size_t instr_idx, size_t next_instr_idx) {
    AsmInstruction* node = GET_INSTR(instr_idx);
    switch (node->type) {
        case AST_AsmMov_t: {
            AsmMov* p_node = &node->get._AsmMov;
            infer_transfer_updated_op(ctx, p_node->dst, next_instr_idx);
            infer_transfer_used_op(ctx, p_node->src, next_instr_idx);
            break;
        }
        case AST_AsmMovSx_t: {
            AsmMovSx* p_node = &node->get._AsmMovSx;
            infer_transfer_updated_op(ctx, p_node->dst, next_instr_idx);
            infer_transfer_used_op(ctx, p_node->src, next_instr_idx);
            break;
        }
        case AST_AsmMovZeroExtend_t: {
            AsmMovZeroExtend* p_node = &node->get._AsmMovZeroExtend;
            infer_transfer_updated_op(ctx, p_node->dst, next_instr_idx);
            infer_transfer_used_op(ctx, p_node->src, next_instr_idx);
            break;
        }
        case AST_AsmLea_t: {
            AsmLea* p_node = &node->get._AsmLea;
            infer_transfer_updated_op(ctx, p_node->dst, next_instr_idx);
            infer_transfer_used_op(ctx, p_node->src, next_instr_idx);
            break;
        }
        case AST_AsmCvttsd2si_t: {
            AsmCvttsd2si* p_node = &node->get._AsmCvttsd2si;
            infer_transfer_updated_op(ctx, p_node->dst, next_instr_idx);
            infer_transfer_used_op(ctx, p_node->src, next_instr_idx);
            break;
        }
        case AST_AsmCvtsi2sd_t: {
            AsmCvtsi2sd* p_node = &node->get._AsmCvtsi2sd;
            infer_transfer_updated_op(ctx, p_node->dst, next_instr_idx);
            infer_transfer_used_op(ctx, p_node->src, next_instr_idx);
            break;
        }
        case AST_AsmUnary_t:
            infer_transfer_used_op(ctx, node->get._AsmUnary.dst, next_instr_idx);
            break;
        case AST_AsmBinary_t: {
            AsmBinary* p_node = &node->get._AsmBinary;
            infer_transfer_used_op(ctx, p_node->src, next_instr_idx);
            infer_transfer_used_op(ctx, p_node->dst, next_instr_idx);
            if (is_bitshift_cl(p_node)) {
                infer_transfer_used_reg(ctx, REG_Cx, next_instr_idx);
            }
            break;
        }
        case AST_AsmCmp_t: {
            AsmCmp* p_node = &node->get._AsmCmp;
            infer_transfer_used_op(ctx, p_node->src, next_instr_idx);
            infer_transfer_used_op(ctx, p_node->dst, next_instr_idx);
            break;
        }
        case AST_AsmIdiv_t:
            infer_transfer_used_op(ctx, node->get._AsmIdiv.src, next_instr_idx);
            infer_transfer_used_reg(ctx, REG_Ax, next_instr_idx);
            infer_transfer_used_reg(ctx, REG_Dx, next_instr_idx);
            break;
        case AST_AsmDiv_t:
            infer_transfer_used_op(ctx, node->get._AsmDiv.src, next_instr_idx);
            infer_transfer_used_reg(ctx, REG_Ax, next_instr_idx);
            break;
        case AST_AsmCdq_t:
            infer_transfer_updated_reg(ctx, REG_Dx, next_instr_idx);
            infer_transfer_used_reg(ctx, REG_Ax, next_instr_idx);
            break;
        case AST_AsmSetCC_t:
            infer_transfer_updated_op(ctx, node->get._AsmSetCC.dst, next_instr_idx);
            break;
        case AST_AsmPush_t:
            infer_transfer_used_op(ctx, node->get._AsmPush.src, next_instr_idx);
            break;
        case AST_AsmCall_t:
            infer_transfer_updated_reg(ctx, REG_Ax, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Cx, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Dx, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Di, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Si, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_R8, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_R9, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Xmm0, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Xmm1, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Xmm2, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Xmm3, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Xmm4, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Xmm5, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Xmm6, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Xmm7, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Xmm8, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Xmm9, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Xmm10, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Xmm11, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Xmm12, next_instr_idx);
            infer_transfer_updated_reg(ctx, REG_Xmm13, next_instr_idx);
            infer_transfer_used_call(ctx, &node->get._AsmCall, next_instr_idx);
            break;
        default:
            THROW_ABORT;
    }
}

static void set_p_infer_graph(Ctx ctx, bool is_dbl) {
    ctx->p_infer_graph = is_dbl ? ctx->sse_infer_graph : ctx->infer_graph;
}

static void infer_add_pseudo_edges(Ctx ctx, TIdentifier name_1, TIdentifier name_2) {
    {
        InferenceRegister* infer = &map_get(ctx->p_infer_graph->pseudo_reg_map, name_1);
        if (!find_identifier(infer->linked_pseudo_names, name_2)) {
            vec_push_back(infer->linked_pseudo_names, name_2);
            infer->degree++;
        }
    }
    {
        InferenceRegister* infer = &map_get(ctx->p_infer_graph->pseudo_reg_map, name_2);
        if (!find_identifier(infer->linked_pseudo_names, name_1)) {
            vec_push_back(infer->linked_pseudo_names, name_1);
            infer->degree++;
        }
    }
}

static void infer_add_reg_edge(Ctx ctx, REGISTER_KIND reg_kind, TIdentifier name) {
    {
        InferenceRegister* infer = &map_get(ctx->p_infer_graph->pseudo_reg_map, name);
        if (!register_mask_get(infer->linked_hard_mask, reg_kind)) {
            register_mask_set(&infer->linked_hard_mask, reg_kind, true);
            infer->degree++;
        }
    }
    {
        InferenceRegister* infer = &ctx->hard_regs[register_mask_bit(reg_kind)];
        if (!find_identifier(infer->linked_pseudo_names, name)) {
            vec_push_back(infer->linked_pseudo_names, name);
            infer->degree++;
        }
    }
}

static void infer_rm_pseudo_edge(InferenceRegister* infer, TIdentifier name) {
    for (size_t i = vec_size(infer->linked_pseudo_names); i-- > 0;) {
        if (infer->linked_pseudo_names[i] == name) {
            vec_remove_swap(infer->linked_pseudo_names, i);
            infer->degree--;
            return;
        }
    }
    THROW_ABORT;
}

static void infer_rm_unpruned_pseudo_name(Ctx ctx, TIdentifier name) {
    for (size_t i = vec_size(ctx->p_infer_graph->unpruned_pseudo_names); i-- > 0;) {
        if (ctx->p_infer_graph->unpruned_pseudo_names[i] == name) {
            vec_remove_swap(ctx->p_infer_graph->unpruned_pseudo_names, i);
            return;
        }
    }
    THROW_ABORT;
}

static void infer_init_used_name_edges(Ctx ctx, TIdentifier name) {
    if (!is_aliased_name(ctx, name)) {
        set_p_infer_graph(ctx, map_get(ctx->frontend->symbol_table, name)->type_t->type == AST_Double_t);
        map_get(ctx->p_infer_graph->pseudo_reg_map, name).spill_cost++;
    }
}

static void infer_init_used_op_edges(Ctx ctx, AsmOperand* node) {
    if (node->type == AST_AsmPseudo_t) {
        infer_init_used_name_edges(ctx, node->get._AsmPseudo.name);
    }
}

static void infer_init_updated_regs_edges(
    Ctx ctx, REGISTER_KIND* reg_kinds, size_t instr_idx, size_t reg_kinds_size, bool is_dbl) {

    size_t mov_mask_bit = ctx->dfa->set_size;
    bool is_mov = GET_INSTR(instr_idx)->type == AST_AsmMov_t;
    if (is_mov) {
        AsmMov* mov = &GET_INSTR(instr_idx)->get._AsmMov;
        if (mov->src->type == AST_AsmPseudo_t) {
            TIdentifier src_name = mov->src->get._AsmPseudo.name;
            if (is_aliased_name(ctx, src_name)) {
                is_mov = false;
            }
            else {
                bool is_src_dbl = map_get(ctx->frontend->symbol_table, src_name)->type_t->type == AST_Double_t;
                set_p_infer_graph(ctx, is_src_dbl);
                map_get(ctx->p_infer_graph->pseudo_reg_map, src_name).spill_cost++;
                mov_mask_bit = map_get(ctx->cfg->identifier_id_map, src_name);
                is_mov = is_dbl == is_src_dbl;
            }
        }
        else {
            is_mov = false;
        }
    }
    set_p_infer_graph(ctx, is_dbl);

    if (GET_DFA_INSTR_SET_MASK(instr_idx, 0) != MASK_FALSE) {
        for (size_t i = ctx->dfa->set_size < 64 ? ctx->dfa->set_size : 64; i-- > REGISTER_MASK_SIZE;) {
            if (GET_DFA_INSTR_SET_AT(instr_idx, i) && !(is_mov && i == mov_mask_bit)) {
                TIdentifier pseudo_name = ctx->dfa_o2->data_name_map[i - REGISTER_MASK_SIZE];
                if (is_dbl == (map_get(ctx->frontend->symbol_table, pseudo_name)->type_t->type == AST_Double_t)) {
                    for (size_t j = 0; j < reg_kinds_size; ++j) {
                        infer_add_reg_edge(ctx, reg_kinds[j], pseudo_name);
                    }
                }
            }
        }
    }
    size_t i = 64;
    for (size_t j = 1; j < ctx->dfa->mask_size; ++j) {
        if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > ctx->dfa->set_size) {
            mask_set_size = ctx->dfa->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(instr_idx, i) && !(is_mov && i == mov_mask_bit)) {
                TIdentifier pseudo_name = ctx->dfa_o2->data_name_map[i - REGISTER_MASK_SIZE];
                if (is_dbl == (map_get(ctx->frontend->symbol_table, pseudo_name)->type_t->type == AST_Double_t)) {
                    for (size_t k = 0; k < reg_kinds_size; ++k) {
                        infer_add_reg_edge(ctx, reg_kinds[k], pseudo_name);
                    }
                }
            }
        }
    }
}

static void infer_init_updated_name_edges(Ctx ctx, TIdentifier name, size_t instr_idx) {
    if (is_aliased_name(ctx, name)) {
        return;
    }
    bool is_dbl = map_get(ctx->frontend->symbol_table, name)->type_t->type == AST_Double_t;

    size_t mov_mask_bit = ctx->dfa->set_size;
    bool is_mov = GET_INSTR(instr_idx)->type == AST_AsmMov_t;
    if (is_mov) {
        AsmMov* mov = &GET_INSTR(instr_idx)->get._AsmMov;
        switch (mov->src->type) {
            case AST_AsmRegister_t: {
                REGISTER_KIND src_reg_kind = register_mask_kind(&mov->src->get._AsmRegister.reg);
                if (src_reg_kind == REG_Sp) {
                    is_mov = false;
                }
                else {
                    mov_mask_bit = register_mask_bit(src_reg_kind);
                    is_mov = is_dbl == (mov_mask_bit > 11);
                }
                break;
            }
            case AST_AsmPseudo_t: {
                TIdentifier src_name = mov->src->get._AsmPseudo.name;
                if (is_aliased_name(ctx, src_name)) {
                    is_mov = false;
                }
                else {
                    bool is_src_dbl = map_get(ctx->frontend->symbol_table, src_name)->type_t->type == AST_Double_t;
                    set_p_infer_graph(ctx, is_src_dbl);
                    map_get(ctx->p_infer_graph->pseudo_reg_map, src_name).spill_cost++;
                    mov_mask_bit = map_get(ctx->cfg->identifier_id_map, src_name);
                    is_mov = is_dbl == is_src_dbl;
                }
                break;
            }
            case AST_AsmMemory_t: {
                REGISTER_KIND src_reg_kind = register_mask_kind(&mov->src->get._AsmMemory.reg);
                if (src_reg_kind == REG_Sp) {
                    is_mov = false;
                }
                else {
                    mov_mask_bit = register_mask_bit(src_reg_kind);
                    is_mov = is_dbl == (mov_mask_bit > 11);
                }
                break;
            }
            case AST_AsmIndexed_t:
                THROW_ABORT;
            default: {
                is_mov = false;
                break;
            }
        }
    }
    set_p_infer_graph(ctx, is_dbl);
    map_get(ctx->p_infer_graph->pseudo_reg_map, name).spill_cost++;

    if (GET_DFA_INSTR_SET_MASK(instr_idx, 0) != MASK_FALSE) {
        size_t i = ctx->p_infer_graph->offset;
        size_t mask_set_size = i + ctx->p_infer_graph->k;
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(instr_idx, i) && !(is_mov && i == mov_mask_bit)) {
                REGISTER_KIND reg_kind = ctx->hard_regs[i].reg_kind;
                infer_add_reg_edge(ctx, reg_kind, name);
            }
        }
        i = REGISTER_MASK_SIZE;
        mask_set_size = ctx->dfa->set_size < 64 ? ctx->dfa->set_size : 64;
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(instr_idx, i) && !(is_mov && i == mov_mask_bit)) {
                TIdentifier pseudo_name = ctx->dfa_o2->data_name_map[i - REGISTER_MASK_SIZE];
                if (name != pseudo_name
                    && is_dbl == (map_get(ctx->frontend->symbol_table, pseudo_name)->type_t->type == AST_Double_t)) {
                    infer_add_pseudo_edges(ctx, name, pseudo_name);
                }
            }
        }
    }
    size_t i = 64;
    for (size_t j = 1; j < ctx->dfa->mask_size; ++j) {
        if (GET_DFA_INSTR_SET_MASK(instr_idx, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > ctx->dfa->set_size) {
            mask_set_size = ctx->dfa->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(instr_idx, i) && !(is_mov && i == mov_mask_bit)) {
                TIdentifier pseudo_name = ctx->dfa_o2->data_name_map[i - REGISTER_MASK_SIZE];
                if (name != pseudo_name
                    && is_dbl == (map_get(ctx->frontend->symbol_table, pseudo_name)->type_t->type == AST_Double_t)) {
                    infer_add_pseudo_edges(ctx, name, pseudo_name);
                }
            }
        }
    }
}

static void infer_init_updated_op_edges(Ctx ctx, AsmOperand* node, size_t instr_idx) {
    switch (node->type) {
        case AST_AsmRegister_t: {
            REGISTER_KIND reg_kinds[1] = {register_mask_kind(&node->get._AsmRegister.reg)};
            if (reg_kinds[0] != REG_Sp) {
                bool is_dbl = register_mask_bit(reg_kinds[0]) > 11;
                infer_init_updated_regs_edges(ctx, reg_kinds, instr_idx, 1, is_dbl);
            }
            break;
        }
        case AST_AsmPseudo_t:
            infer_init_updated_name_edges(ctx, node->get._AsmPseudo.name, instr_idx);
            break;
        default:
            break;
    }
}

static void infer_init_edges(Ctx ctx, size_t instr_idx) {
    AsmInstruction* node = GET_INSTR(instr_idx);
    switch (node->type) {
        case AST_AsmMov_t:
            infer_init_updated_op_edges(ctx, node->get._AsmMov.dst, instr_idx);
            break;
        case AST_AsmMovSx_t:
            infer_init_updated_op_edges(ctx, node->get._AsmMovSx.dst, instr_idx);
            break;
        case AST_AsmMovZeroExtend_t:
            infer_init_updated_op_edges(ctx, node->get._AsmMovZeroExtend.dst, instr_idx);
            break;
        case AST_AsmLea_t:
            infer_init_updated_op_edges(ctx, node->get._AsmLea.dst, instr_idx);
            break;
        case AST_AsmCvttsd2si_t:
            infer_init_updated_op_edges(ctx, node->get._AsmCvttsd2si.dst, instr_idx);
            break;
        case AST_AsmCvtsi2sd_t:
            infer_init_updated_op_edges(ctx, node->get._AsmCvtsi2sd.dst, instr_idx);
            break;
        case AST_AsmUnary_t:
            infer_init_updated_op_edges(ctx, node->get._AsmUnary.dst, instr_idx);
            break;
        case AST_AsmBinary_t: {
            AsmBinary* p_node = &node->get._AsmBinary;
            if (is_bitshift_cl(p_node)) {
                REGISTER_KIND reg_kinds[1] = {REG_Cx};
                infer_init_updated_regs_edges(ctx, reg_kinds, instr_idx, 1, false);
            }
            infer_init_updated_op_edges(ctx, p_node->dst, instr_idx);
            infer_init_used_op_edges(ctx, p_node->src);
            break;
        }
        case AST_AsmCmp_t: {
            AsmCmp* p_node = &node->get._AsmCmp;
            infer_init_used_op_edges(ctx, p_node->src);
            infer_init_used_op_edges(ctx, p_node->dst);
            break;
        }
        case AST_AsmIdiv_t: {
            REGISTER_KIND reg_kinds[2] = {REG_Ax, REG_Dx};
            infer_init_updated_regs_edges(ctx, reg_kinds, instr_idx, 2, false);
            infer_init_used_op_edges(ctx, node->get._AsmIdiv.src);
            break;
        }
        case AST_AsmDiv_t: {
            REGISTER_KIND reg_kinds[1] = {REG_Ax};
            infer_init_updated_regs_edges(ctx, reg_kinds, instr_idx, 1, false);
            infer_init_used_op_edges(ctx, node->get._AsmDiv.src);
            break;
        }
        case AST_AsmCdq_t: {
            REGISTER_KIND reg_kinds[1] = {REG_Dx};
            infer_init_updated_regs_edges(ctx, reg_kinds, instr_idx, 1, false);
            break;
        }
        case AST_AsmSetCC_t:
            infer_init_updated_op_edges(ctx, node->get._AsmSetCC.dst, instr_idx);
            break;
        case AST_AsmPush_t:
            infer_init_used_op_edges(ctx, node->get._AsmPush.src);
            break;
        case AST_AsmCall_t: {
            {
                REGISTER_KIND reg_kinds[7] = {REG_Ax, REG_Cx, REG_Dx, REG_Di, REG_Si, REG_R8, REG_R9};
                infer_init_updated_regs_edges(ctx, reg_kinds, instr_idx, 7, false);
            }
            {
                REGISTER_KIND reg_kinds[14] = {REG_Xmm0, REG_Xmm1, REG_Xmm2, REG_Xmm3, REG_Xmm4, REG_Xmm5, REG_Xmm6,
                    REG_Xmm7, REG_Xmm8, REG_Xmm9, REG_Xmm10, REG_Xmm11, REG_Xmm12, REG_Xmm13};
                infer_init_updated_regs_edges(ctx, reg_kinds, instr_idx, 14, true);
            }
            break;
        }
        default:
            break;
    }
}

static bool init_inference_graph(Ctx ctx, TIdentifier fun_name) {
    if (!init_data_flow_analysis(ctx, fun_name)) {
        return false;
    }
    dfa_iter_alg(ctx);
    if (map_empty(ctx->cfg->identifier_id_map)) {
        return false;
    }

    ctx->callee_saved_reg_mask = REGISTER_MASK_FALSE;

    vec_clear(ctx->infer_graph->unpruned_pseudo_names);
    for (size_t i = 0; i < map_size(ctx->infer_graph->pseudo_reg_map); ++i) {
        vec_delete(pair_second(ctx->infer_graph->pseudo_reg_map[i]).linked_pseudo_names);
    }
    map_clear(ctx->infer_graph->pseudo_reg_map);

    vec_clear(ctx->sse_infer_graph->unpruned_pseudo_names);
    for (size_t i = 0; i < map_size(ctx->sse_infer_graph->pseudo_reg_map); ++i) {
        vec_delete(pair_second(ctx->sse_infer_graph->pseudo_reg_map[i]).linked_pseudo_names);
    }
    map_clear(ctx->sse_infer_graph->pseudo_reg_map);

    for (size_t i = 0; i < map_size(ctx->cfg->identifier_id_map); ++i) {
        TIdentifier name = pair_first(ctx->cfg->identifier_id_map[i]);
        InferenceRegister infer = {REG_Sp, REG_Sp, 0, 0, REGISTER_MASK_FALSE, vec_new()};
        if (map_get(ctx->frontend->symbol_table, name)->type_t->type == AST_Double_t) {
            vec_push_back(ctx->sse_infer_graph->unpruned_pseudo_names, name);
            map_add(ctx->sse_infer_graph->pseudo_reg_map, name, infer);
        }
        else {
            vec_push_back(ctx->infer_graph->unpruned_pseudo_names, name);
            map_add(ctx->infer_graph->pseudo_reg_map, name, infer);
        }
    }

    if (!map_empty(ctx->infer_graph->pseudo_reg_map)) {
        if (vec_size(ctx->infer_graph->unpruned_hard_mask_bits) < 12) {
            vec_resize(ctx->infer_graph->unpruned_hard_mask_bits, 12);
        }

        mask_t hard_reg_mask = ctx->infer_graph->hard_reg_mask;
        for (size_t i = 0; i < 12; ++i) {
            ctx->reg_color_map[i] = REG_Sp;
            ctx->hard_regs[i].color = REG_Sp;
            ctx->hard_regs[i].degree = 11;
            ctx->hard_regs[i].spill_cost = 0;
            ctx->hard_regs[i].linked_hard_mask = hard_reg_mask;
            vec_clear(ctx->hard_regs[i].linked_pseudo_names);
            ctx->infer_graph->unpruned_hard_mask_bits[i] = i;
        }
    }
    if (!map_empty(ctx->sse_infer_graph->pseudo_reg_map)) {
        if (vec_size(ctx->sse_infer_graph->unpruned_hard_mask_bits) < 14) {
            vec_resize(ctx->sse_infer_graph->unpruned_hard_mask_bits, 14);
        }

        mask_t hard_reg_mask = ctx->sse_infer_graph->hard_reg_mask;
        for (size_t i = 12; i < 26; ++i) {
            ctx->reg_color_map[i] = REG_Sp;
            ctx->hard_regs[i].color = REG_Sp;
            ctx->hard_regs[i].degree = 13;
            ctx->hard_regs[i].spill_cost = 0;
            ctx->hard_regs[i].linked_hard_mask = hard_reg_mask;
            vec_clear(ctx->hard_regs[i].linked_pseudo_names);
            ctx->sse_infer_graph->unpruned_hard_mask_bits[i - 12] = i;
        }
    }

    for (size_t block_id = 0; block_id < vec_size(ctx->cfg->blocks); ++block_id) {
        if (GET_CFG_BLOCK(block_id).size > 0) {
            for (size_t instr_idx = GET_CFG_BLOCK(block_id).instrs_front_idx;
                 instr_idx <= GET_CFG_BLOCK(block_id).instrs_back_idx; ++instr_idx) {
                if (GET_INSTR(instr_idx)) {
                    infer_init_edges(ctx, instr_idx);
                }
            }
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register allocation

static bool is_reg_callee_saved(REGISTER_KIND reg_kind) {
    switch (reg_kind) {
        case REG_Bx:
        case REG_R12:
        case REG_R13:
        case REG_R14:
        case REG_R15:
            return true;
        default:
            return false;
    }
}

static void alloc_prune_infer_reg(Ctx ctx, InferenceRegister* infer, size_t pruned_idx) {
    if (infer->reg_kind == REG_Sp) {
        vec_remove_swap(ctx->p_infer_graph->unpruned_pseudo_names, pruned_idx);
    }
    else {
        vec_remove_swap(ctx->p_infer_graph->unpruned_hard_mask_bits, pruned_idx);
    }
    if (infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < ctx->p_infer_graph->k; ++i) {
            InferenceRegister* linked_infer = &ctx->hard_regs[i + ctx->p_infer_graph->offset];
            if (register_mask_get(infer->linked_hard_mask, linked_infer->reg_kind)) {
                linked_infer->degree--;
            }
        }
    }
    for (size_t i = 0; i < vec_size(infer->linked_pseudo_names); ++i) {
        map_get(ctx->p_infer_graph->pseudo_reg_map, infer->linked_pseudo_names[i]).degree--;
    }
}

static void alloc_unprune_infer_reg(Ctx ctx, InferenceRegister* infer, TIdentifier pruned_name) {
    if (infer->reg_kind == REG_Sp) {
        THROW_ABORT_IF(find_identifier(ctx->p_infer_graph->unpruned_pseudo_names, pruned_name));
        vec_push_back(ctx->p_infer_graph->unpruned_pseudo_names, pruned_name);
    }
    else {
        size_t pruned_mask_bit = register_mask_bit(infer->reg_kind);
        THROW_ABORT_IF(find_size_t(ctx->p_infer_graph->unpruned_hard_mask_bits, pruned_mask_bit));
        vec_push_back(ctx->p_infer_graph->unpruned_hard_mask_bits, pruned_mask_bit);
    }
    if (infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < ctx->p_infer_graph->k; ++i) {
            InferenceRegister* linked_infer = &ctx->hard_regs[i + ctx->p_infer_graph->offset];
            if (register_mask_get(infer->linked_hard_mask, linked_infer->reg_kind)) {
                linked_infer->degree++;
            }
        }
    }
    for (size_t i = 0; i < vec_size(infer->linked_pseudo_names); ++i) {
        map_get(ctx->p_infer_graph->pseudo_reg_map, infer->linked_pseudo_names[i]).degree++;
    }
}

static void alloc_color_infer_graph(Ctx ctx);

static void alloc_next_color_infer_graph(Ctx ctx) {
    if (!vec_empty(ctx->p_infer_graph->unpruned_hard_mask_bits)
        || !vec_empty(ctx->p_infer_graph->unpruned_pseudo_names)) {
        alloc_color_infer_graph(ctx);
    }
}

static InferenceRegister* alloc_prune_infer_graph(Ctx ctx, TIdentifier* pruned_name) {
    size_t pruned_idx;
    InferenceRegister* infer = NULL;
    for (size_t i = 0; i < vec_size(ctx->p_infer_graph->unpruned_pseudo_names); ++i) {
        *pruned_name = ctx->p_infer_graph->unpruned_pseudo_names[i];
        infer = &map_get(ctx->p_infer_graph->pseudo_reg_map, *pruned_name);
        if (infer->degree < ctx->p_infer_graph->k) {
            pruned_idx = i;
            break;
        }
        infer = NULL;
    }
    if (!infer) {
        for (size_t i = 0; i < vec_size(ctx->p_infer_graph->unpruned_hard_mask_bits); ++i) {
            size_t pruned_mask_bit = ctx->p_infer_graph->unpruned_hard_mask_bits[i];
            infer = &ctx->hard_regs[pruned_mask_bit];
            if (infer->degree < ctx->p_infer_graph->k) {
                pruned_idx = i;
                break;
            }
            infer = NULL;
        }
    }
    if (!infer) {
        size_t i = 0;
        for (; i < vec_size(ctx->p_infer_graph->unpruned_pseudo_names); ++i) {
            *pruned_name = ctx->p_infer_graph->unpruned_pseudo_names[i];
            infer = &map_get(ctx->p_infer_graph->pseudo_reg_map, *pruned_name);
            if (infer->degree > 0) {
                pruned_idx = i;
                break;
            }
            infer = NULL;
        }
        THROW_ABORT_IF(!infer);
        double min_spill_metric = ((double)infer->spill_cost) / infer->degree;
        for (; i < vec_size(ctx->p_infer_graph->unpruned_pseudo_names); ++i) {
            TIdentifier spill_name = ctx->p_infer_graph->unpruned_pseudo_names[i];
            InferenceRegister* spill_infer = &map_get(ctx->p_infer_graph->pseudo_reg_map, spill_name);
            if (spill_infer->degree > 0) {
                double spill_metric = ((double)spill_infer->spill_cost) / spill_infer->degree;
                if (spill_metric < min_spill_metric) {
                    pruned_idx = i;
                    *pruned_name = spill_name;
                    infer = spill_infer;
                    min_spill_metric = spill_metric;
                }
            }
        }
    }
    alloc_prune_infer_reg(ctx, infer, pruned_idx);
    return infer;
}

static void alloc_unprune_infer_graph(Ctx ctx, InferenceRegister* infer, TIdentifier pruned_name) {
    mask_t color_reg_mask = ctx->p_infer_graph->hard_reg_mask;
    if (infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < ctx->p_infer_graph->k; ++i) {
            InferenceRegister* linked_infer = &ctx->hard_regs[i + ctx->p_infer_graph->offset];
            if (register_mask_get(infer->linked_hard_mask, linked_infer->reg_kind)) {
                if (linked_infer->color != REG_Sp) {
                    register_mask_set(&color_reg_mask, linked_infer->color, false);
                }
            }
        }
    }
    for (size_t i = 0; i < vec_size(infer->linked_pseudo_names); ++i) {
        InferenceRegister* linked_infer = &map_get(ctx->p_infer_graph->pseudo_reg_map, infer->linked_pseudo_names[i]);
        if (linked_infer->color != REG_Sp) {
            register_mask_set(&color_reg_mask, linked_infer->color, false);
        }
    }
    if (color_reg_mask != REGISTER_MASK_FALSE) {
        if (is_reg_callee_saved(infer->reg_kind)) {
            for (size_t i = ctx->p_infer_graph->k; i-- > 0;) {
                REGISTER_KIND color = ctx->hard_regs[i + ctx->p_infer_graph->offset].reg_kind;
                if (register_mask_get(color_reg_mask, color)) {
                    infer->color = color;
                    break;
                }
            }
        }
        else {
            for (size_t i = 0; i < ctx->p_infer_graph->k; ++i) {
                REGISTER_KIND color = ctx->hard_regs[i + ctx->p_infer_graph->offset].reg_kind;
                if (register_mask_get(color_reg_mask, color)) {
                    infer->color = color;
                    break;
                }
            }
        }
        alloc_unprune_infer_reg(ctx, infer, pruned_name);
    }
}

static void alloc_color_infer_graph(Ctx ctx) {
    TIdentifier pruned_name = 0;
    InferenceRegister* infer = alloc_prune_infer_graph(ctx, &pruned_name);
    alloc_next_color_infer_graph(ctx);
    alloc_unprune_infer_graph(ctx, infer, pruned_name);
}

static void alloc_color_reg_map(Ctx ctx) {
    for (size_t i = 0; i < ctx->p_infer_graph->k; ++i) {
        InferenceRegister* infer = &ctx->hard_regs[i + ctx->p_infer_graph->offset];
        if (infer->color != REG_Sp) {
            ctx->reg_color_map[register_mask_bit(infer->color)] = infer->reg_kind;
        }
    }
}

static shared_ptr_t(AsmOperand) alloc_hard_reg(Ctx ctx, TIdentifier name) {
    if (is_aliased_name(ctx, name)) {
        return sptr_new();
    }
    set_p_infer_graph(ctx, map_get(ctx->frontend->symbol_table, name)->type_t->type == AST_Double_t);
    REGISTER_KIND color = map_get(ctx->p_infer_graph->pseudo_reg_map, name).color;
    if (color != REG_Sp) {
        REGISTER_KIND reg_kind = ctx->reg_color_map[register_mask_bit(color)];
        shared_ptr_t(AsmOperand) hard_reg = gen_register(reg_kind);
        if (is_reg_callee_saved(reg_kind) && !register_mask_get(ctx->callee_saved_reg_mask, reg_kind)) {
            register_mask_set(&ctx->callee_saved_reg_mask, reg_kind, true);
            shared_ptr_t(AsmOperand) callee_saved_reg = sptr_new();
            sptr_copy(AsmOperand, hard_reg, callee_saved_reg);
            vec_move_back(ctx->p_backend_fun->callee_saved_regs, callee_saved_reg);
        }
        return hard_reg;
    }
    else {
        return sptr_new();
    }
}

static REGISTER_KIND get_op_reg_kind(Ctx ctx, AsmOperand* node) {
    switch (node->type) {
        case AST_AsmRegister_t:
            return register_mask_kind(&node->get._AsmRegister.reg);
        case AST_AsmPseudo_t: {
            TIdentifier name = node->get._AsmPseudo.name;
            if (is_aliased_name(ctx, name)) {
                return REG_Sp;
            }
            set_p_infer_graph(ctx, map_get(ctx->frontend->symbol_table, name)->type_t->type == AST_Double_t);
            REGISTER_KIND color = map_get(ctx->p_infer_graph->pseudo_reg_map, name).color;
            if (color == REG_Sp) {
                return REG_Sp;
            }
            else {
                return ctx->reg_color_map[register_mask_bit(color)];
            }
        }
        case AST_AsmMemory_t:
            return REG_Sp;
        case AST_AsmIndexed_t:
            THROW_ABORT;
        default:
            return REG_Sp;
    }
}

static void alloc_mov_instr(Ctx ctx, AsmMov* node, size_t instr_idx) {
    REGISTER_KIND src_reg_kind = get_op_reg_kind(ctx, node->src);
    REGISTER_KIND dst_reg_kind = get_op_reg_kind(ctx, node->dst);
    if (src_reg_kind != REG_Sp && src_reg_kind == dst_reg_kind) {
        set_instr(ctx, uptr_new(), instr_idx);
    }
    else {
        if (node->src->type == AST_AsmPseudo_t) {
            shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->src->get._AsmPseudo.name);
            if (hard_reg) {
                sptr_move(AsmOperand, hard_reg, node->src);
            }
        }
        if (node->dst->type == AST_AsmPseudo_t) {
            shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->dst->get._AsmPseudo.name);
            if (hard_reg) {
                sptr_move(AsmOperand, hard_reg, node->dst);
            }
        }
    }
}

static void alloc_mov_sx_instr(Ctx ctx, AsmMovSx* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->src->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->src);
        }
    }
    if (node->dst->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->dst->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->dst);
        }
    }
}

static void alloc_zero_extend_instr(Ctx ctx, AsmMovZeroExtend* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->src->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->src);
        }
    }
    if (node->dst->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->dst->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->dst);
        }
    }
}

static void alloc_lea_instr(Ctx ctx, AsmLea* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->src->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->src);
        }
    }
    if (node->dst->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->dst->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->dst);
        }
    }
}

static void alloc_cvttsd2si_instr(Ctx ctx, AsmCvttsd2si* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->src->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->src);
        }
    }
    if (node->dst->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->dst->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->dst);
        }
    }
}

static void alloc_cvtsi2sd_instr(Ctx ctx, AsmCvtsi2sd* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->src->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->src);
        }
    }
    if (node->dst->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->dst->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->dst);
        }
    }
}

static void alloc_unary_instr(Ctx ctx, AsmUnary* node) {
    if (node->dst->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->dst->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->dst);
        }
    }
}

static void alloc_binary_instr(Ctx ctx, AsmBinary* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->src->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->src);
        }
    }
    if (node->dst->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->dst->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->dst);
        }
    }
}

static void alloc_cmp_instr(Ctx ctx, AsmCmp* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->src->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->src);
        }
    }
    if (node->dst->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->dst->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->dst);
        }
    }
}

static void alloc_idiv_instr(Ctx ctx, AsmIdiv* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->src->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->src);
        }
    }
}

static void alloc_div_instr(Ctx ctx, AsmDiv* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->src->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->src);
        }
    }
}

static void alloc_set_cc_instr(Ctx ctx, AsmSetCC* node) {
    if (node->dst->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->dst->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->dst);
        }
    }
}

static void alloc_push_instr(Ctx ctx, AsmPush* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        shared_ptr_t(AsmOperand) hard_reg = alloc_hard_reg(ctx, node->src->get._AsmPseudo.name);
        if (hard_reg) {
            sptr_move(AsmOperand, hard_reg, node->src);
        }
    }
}

static void alloc_instr(Ctx ctx, size_t instr_idx) {
    AsmInstruction* node = GET_INSTR(instr_idx);
    switch (node->type) {
        case AST_AsmMov_t:
            alloc_mov_instr(ctx, &node->get._AsmMov, instr_idx);
            break;
        case AST_AsmMovSx_t:
            alloc_mov_sx_instr(ctx, &node->get._AsmMovSx);
            break;
        case AST_AsmMovZeroExtend_t:
            alloc_zero_extend_instr(ctx, &node->get._AsmMovZeroExtend);
            break;
        case AST_AsmLea_t:
            alloc_lea_instr(ctx, &node->get._AsmLea);
            break;
        case AST_AsmCvttsd2si_t:
            alloc_cvttsd2si_instr(ctx, &node->get._AsmCvttsd2si);
            break;
        case AST_AsmCvtsi2sd_t:
            alloc_cvtsi2sd_instr(ctx, &node->get._AsmCvtsi2sd);
            break;
        case AST_AsmUnary_t:
            alloc_unary_instr(ctx, &node->get._AsmUnary);
            break;
        case AST_AsmBinary_t:
            alloc_binary_instr(ctx, &node->get._AsmBinary);
            break;
        case AST_AsmCmp_t:
            alloc_cmp_instr(ctx, &node->get._AsmCmp);
            break;
        case AST_AsmIdiv_t:
            alloc_idiv_instr(ctx, &node->get._AsmIdiv);
            break;
        case AST_AsmDiv_t:
            alloc_div_instr(ctx, &node->get._AsmDiv);
            break;
        case AST_AsmSetCC_t:
            alloc_set_cc_instr(ctx, &node->get._AsmSetCC);
            break;
        case AST_AsmPush_t:
            alloc_push_instr(ctx, &node->get._AsmPush);
            break;
        case AST_AsmCdq_t:
        case AST_AsmCall_t:
            break;
        default:
            break;
    }
}

static void reallocate_registers(Ctx ctx) {
    if (!vec_empty(ctx->infer_graph->unpruned_pseudo_names)) {
        set_p_infer_graph(ctx, false);
        alloc_color_infer_graph(ctx);
        alloc_color_reg_map(ctx);
    }
    if (!vec_empty(ctx->sse_infer_graph->unpruned_pseudo_names)) {
        set_p_infer_graph(ctx, true);
        alloc_color_infer_graph(ctx);
        alloc_color_reg_map(ctx);
    }
    for (size_t instr_idx = 0; instr_idx < vec_size(*ctx->p_instrs); ++instr_idx) {
        if (GET_INSTR(instr_idx)) {
            alloc_instr(ctx, instr_idx);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register coalescing

static TInt get_type_size(Type* type) {
    switch (type->type) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t:
            return 1;
        case AST_Int_t:
        case AST_UInt_t:
            return 4;
        case AST_Long_t:
        case AST_Double_t:
        case AST_ULong_t:
        case AST_Pointer_t:
            return 8;
        default:
            THROW_ABORT;
    }
}

static size_t get_coalesced_idx(Ctx ctx, AsmOperand* node) {
    size_t coalesced_idx = ctx->dfa->set_size;
    switch (node->type) {
        case AST_AsmRegister_t: {
            REGISTER_KIND reg_kind = register_mask_kind(&node->get._AsmRegister.reg);
            if (reg_kind != REG_Sp) {
                coalesced_idx = register_mask_bit(reg_kind);
            }
            break;
        }
        case AST_AsmPseudo_t: {
            TIdentifier name = node->get._AsmPseudo.name;
            if (!is_aliased_name(ctx, name)) {
                coalesced_idx = map_get(ctx->cfg->identifier_id_map, name);
            }
            break;
        }
        default:
            break;
    }

    if (coalesced_idx < ctx->dfa->set_size) {
        while (coalesced_idx >= REGISTER_MASK_SIZE
               && coalesced_idx != ctx->dfa->open_data_map[coalesced_idx - REGISTER_MASK_SIZE]) {
            coalesced_idx = ctx->dfa->open_data_map[coalesced_idx - REGISTER_MASK_SIZE];
        }
    }
    return coalesced_idx;
}

static bool get_coalescable_infer_regs(
    Ctx ctx, InferenceRegister** src_infer, InferenceRegister** dst_infer, size_t src_idx, size_t dst_idx) {
    if (src_idx != dst_idx && (src_idx >= REGISTER_MASK_SIZE || dst_idx >= REGISTER_MASK_SIZE)
        && src_idx < ctx->dfa->set_size && dst_idx < ctx->dfa->set_size) {
        if (src_idx < REGISTER_MASK_SIZE) {
            TIdentifier dst_name = ctx->dfa_o2->data_name_map[dst_idx - REGISTER_MASK_SIZE];
            bool is_dbl = map_get(ctx->frontend->symbol_table, dst_name)->type_t->type == AST_Double_t;
            if (is_dbl == (src_idx > 11)) {
                set_p_infer_graph(ctx, is_dbl);
                *src_infer = &ctx->hard_regs[src_idx];
                *dst_infer = &map_get(ctx->p_infer_graph->pseudo_reg_map, dst_name);
                return !register_mask_get((*dst_infer)->linked_hard_mask, (*src_infer)->reg_kind);
            }
        }
        else if (dst_idx < REGISTER_MASK_SIZE) {
            TIdentifier src_name = ctx->dfa_o2->data_name_map[src_idx - REGISTER_MASK_SIZE];
            bool is_dbl = map_get(ctx->frontend->symbol_table, src_name)->type_t->type == AST_Double_t;
            if (is_dbl == (dst_idx > 11)) {
                set_p_infer_graph(ctx, is_dbl);
                *src_infer = &map_get(ctx->p_infer_graph->pseudo_reg_map, src_name);
                *dst_infer = &ctx->hard_regs[dst_idx];
                return !register_mask_get((*src_infer)->linked_hard_mask, (*dst_infer)->reg_kind);
            }
        }
        else {
            TIdentifier src_name = ctx->dfa_o2->data_name_map[src_idx - REGISTER_MASK_SIZE];
            TIdentifier dst_name = ctx->dfa_o2->data_name_map[dst_idx - REGISTER_MASK_SIZE];
            Type* src_type = map_get(ctx->frontend->symbol_table, src_name)->type_t;
            Type* dst_type = map_get(ctx->frontend->symbol_table, dst_name)->type_t;
            bool is_dbl = src_type->type == AST_Double_t;
            if (is_dbl == (dst_type->type == AST_Double_t) && get_type_size(src_type) == get_type_size(dst_type)) {
                set_p_infer_graph(ctx, is_dbl);
                *src_infer = &map_get(ctx->p_infer_graph->pseudo_reg_map, src_name);
                *dst_infer = &map_get(ctx->p_infer_graph->pseudo_reg_map, dst_name);
                return !find_identifier((*dst_infer)->linked_pseudo_names, src_name);
            }
        }
    }
    return false;
}

static bool coal_briggs_test(Ctx ctx, InferenceRegister* src_infer, InferenceRegister* dst_infer) {
    size_t degree = 0;

    if (src_infer->linked_hard_mask != REGISTER_MASK_FALSE || dst_infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < ctx->p_infer_graph->k; ++i) {
            InferenceRegister* linked_infer = &ctx->hard_regs[i + ctx->p_infer_graph->offset];
            if (register_mask_get(src_infer->linked_hard_mask, linked_infer->reg_kind)) {
                if (register_mask_get(dst_infer->linked_hard_mask, linked_infer->reg_kind)) {
                    if (linked_infer->degree > ctx->p_infer_graph->k) {
                        degree++;
                    }
                }
                else if (linked_infer->degree >= ctx->p_infer_graph->k) {
                    degree++;
                }
            }
            else if (register_mask_get(dst_infer->linked_hard_mask, linked_infer->reg_kind)
                     && linked_infer->degree >= ctx->p_infer_graph->k) {
                degree++;
            }
        }
    }

    for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
        GET_DFA_INSTR_SET_MASK(ctx->dfa->incoming_idx, i) = MASK_FALSE;
    }
    for (size_t i = 0; i < vec_size(dst_infer->linked_pseudo_names); ++i) {
        size_t j = map_get(ctx->cfg->identifier_id_map, dst_infer->linked_pseudo_names[i]);
        SET_DFA_INSTR_SET_AT(ctx->dfa->incoming_idx, j, true);
    }
    for (size_t i = 0; i < vec_size(src_infer->linked_pseudo_names); ++i) {
        size_t j = map_get(ctx->cfg->identifier_id_map, src_infer->linked_pseudo_names[i]);
        InferenceRegister* linked_infer =
            &map_get(ctx->p_infer_graph->pseudo_reg_map, src_infer->linked_pseudo_names[i]);
        if (GET_DFA_INSTR_SET_AT(ctx->dfa->incoming_idx, j)) {
            if (linked_infer->degree > ctx->p_infer_graph->k) {
                degree++;
            }
            SET_DFA_INSTR_SET_AT(ctx->dfa->incoming_idx, j, false);
        }
        else if (linked_infer->degree >= ctx->p_infer_graph->k) {
            degree++;
        }
    }
    for (size_t i = 0; i < vec_size(dst_infer->linked_pseudo_names); ++i) {
        size_t j = map_get(ctx->cfg->identifier_id_map, dst_infer->linked_pseudo_names[i]);
        if (GET_DFA_INSTR_SET_AT(ctx->dfa->incoming_idx, j)) {
            InferenceRegister* linked_infer =
                &map_get(ctx->p_infer_graph->pseudo_reg_map, dst_infer->linked_pseudo_names[i]);
            if (linked_infer->degree >= ctx->p_infer_graph->k) {
                degree++;
            }
        }
    }

    return degree < ctx->p_infer_graph->k;
}

static bool coal_george_test(Ctx ctx, REGISTER_KIND reg_kind, InferenceRegister* infer) {
    for (size_t i = 0; i < vec_size(infer->linked_pseudo_names); ++i) {
        InferenceRegister* linked_infer = &map_get(ctx->p_infer_graph->pseudo_reg_map, infer->linked_pseudo_names[i]);
        if (!register_mask_get(linked_infer->linked_hard_mask, reg_kind)
            && linked_infer->degree >= ctx->p_infer_graph->k) {
            return false;
        }
    }
    return true;
}

static bool coal_conservative_tests(Ctx ctx, InferenceRegister* src_infer, InferenceRegister* dst_infer) {
    if (coal_briggs_test(ctx, src_infer, dst_infer)) {
        return true;
    }
    else if (src_infer->reg_kind != REG_Sp) {
        return coal_george_test(ctx, src_infer->reg_kind, dst_infer);
    }
    else if (dst_infer->reg_kind != REG_Sp) {
        return coal_george_test(ctx, dst_infer->reg_kind, src_infer);
    }
    else {
        return false;
    }
}

static void coal_pseudo_infer_reg(Ctx ctx, InferenceRegister* infer, size_t merge_idx, size_t keep_idx) {
    TIdentifier merge_name = ctx->dfa_o2->data_name_map[merge_idx - REGISTER_MASK_SIZE];
    TIdentifier keep_name = ctx->dfa_o2->data_name_map[keep_idx - REGISTER_MASK_SIZE];
    if (infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < ctx->p_infer_graph->k; ++i) {
            InferenceRegister* linked_infer = &ctx->hard_regs[i + ctx->p_infer_graph->offset];
            if (register_mask_get(infer->linked_hard_mask, linked_infer->reg_kind)) {
                infer_rm_pseudo_edge(linked_infer, merge_name);
                infer_add_reg_edge(ctx, linked_infer->reg_kind, keep_name);
            }
        }
    }
    for (size_t i = 0; i < vec_size(infer->linked_pseudo_names); ++i) {
        InferenceRegister* linked_infer = &map_get(ctx->p_infer_graph->pseudo_reg_map, infer->linked_pseudo_names[i]);
        infer_rm_pseudo_edge(linked_infer, merge_name);
        infer_add_pseudo_edges(ctx, keep_name, infer->linked_pseudo_names[i]);
    }
    infer_rm_unpruned_pseudo_name(ctx, merge_name);
}

static void coal_hard_infer_reg(Ctx ctx, REGISTER_KIND reg_kind, InferenceRegister* infer, size_t merge_idx) {
    TIdentifier merge_name = ctx->dfa_o2->data_name_map[merge_idx - REGISTER_MASK_SIZE];
    if (infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < ctx->p_infer_graph->k; ++i) {
            InferenceRegister* linked_infer = &ctx->hard_regs[i + ctx->p_infer_graph->offset];
            if (register_mask_get(infer->linked_hard_mask, linked_infer->reg_kind)) {
                infer_rm_pseudo_edge(linked_infer, merge_name);
            }
        }
    }
    for (size_t i = 0; i < vec_size(infer->linked_pseudo_names); ++i) {
        InferenceRegister* linked_infer = &map_get(ctx->p_infer_graph->pseudo_reg_map, infer->linked_pseudo_names[i]);
        infer_rm_pseudo_edge(linked_infer, merge_name);
        infer_add_reg_edge(ctx, reg_kind, infer->linked_pseudo_names[i]);
    }
    infer_rm_unpruned_pseudo_name(ctx, merge_name);
}

static bool coal_infer_regs(Ctx ctx, AsmMov* node) {
    InferenceRegister* src_infer = NULL;
    InferenceRegister* dst_infer = NULL;
    size_t src_idx = get_coalesced_idx(ctx, node->src);
    size_t dst_idx = get_coalesced_idx(ctx, node->dst);
    if (get_coalescable_infer_regs(ctx, &src_infer, &dst_infer, src_idx, dst_idx)
        && coal_conservative_tests(ctx, src_infer, dst_infer)) {
        if (src_idx < REGISTER_MASK_SIZE) {
            coal_hard_infer_reg(ctx, src_infer->reg_kind, dst_infer, dst_idx);
            ctx->dfa->open_data_map[dst_idx - REGISTER_MASK_SIZE] = src_idx;
        }
        else {
            if (dst_idx < REGISTER_MASK_SIZE) {
                coal_hard_infer_reg(ctx, dst_infer->reg_kind, src_infer, src_idx);
            }
            else {
                coal_pseudo_infer_reg(ctx, src_infer, src_idx, dst_idx);
            }
            ctx->dfa->open_data_map[src_idx - REGISTER_MASK_SIZE] = dst_idx;
        }
        return true;
    }
    else {
        return false;
    }
}

static shared_ptr_t(AsmOperand) coal_op_reg(Ctx ctx, TIdentifier name, size_t coalesced_idx) {
    if (coalesced_idx < ctx->dfa->set_size && coalesced_idx != map_get(ctx->cfg->identifier_id_map, name)) {
        if (coalesced_idx < REGISTER_MASK_SIZE) {
            REGISTER_KIND reg_kind = ctx->hard_regs[coalesced_idx].reg_kind;
            return gen_register(reg_kind);
        }
        else {
            set_p_infer_graph(ctx, map_get(ctx->frontend->symbol_table, name)->type_t->type == AST_Double_t);
            name = ctx->dfa_o2->data_name_map[coalesced_idx - REGISTER_MASK_SIZE];
            map_get(ctx->p_infer_graph->pseudo_reg_map, name).spill_cost++;
            return make_AsmPseudo(name);
        }
    }
    else {
        return sptr_new();
    }
}

static void coal_mov_instr(Ctx ctx, AsmMov* node, size_t instr_idx, size_t block_id) {
    size_t src_idx = get_coalesced_idx(ctx, node->src);
    size_t dst_idx = get_coalesced_idx(ctx, node->dst);
    if (src_idx < ctx->dfa->set_size && src_idx == dst_idx) {
        cfg_rm_block_instr(ctx, instr_idx, block_id);
    }
    else {
        if (node->src->type == AST_AsmPseudo_t) {
            shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->src->get._AsmPseudo.name, src_idx);
            if (op_reg) {
                sptr_move(AsmOperand, op_reg, node->src);
            }
        }
        if (node->dst->type == AST_AsmPseudo_t) {
            shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->dst->get._AsmPseudo.name, dst_idx);
            if (op_reg) {
                sptr_move(AsmOperand, op_reg, node->dst);
            }
        }
    }
}

static void coal_mov_sx_instr(Ctx ctx, AsmMovSx* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->src->get._AsmPseudo.name, src_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->src);
        }
    }
    if (node->dst->type == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->dst->get._AsmPseudo.name, dst_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->dst);
        }
    }
}

static void coal_zero_extend_instr(Ctx ctx, AsmMovZeroExtend* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->src->get._AsmPseudo.name, src_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->src);
        }
    }
    if (node->dst->type == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->dst->get._AsmPseudo.name, dst_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->dst);
        }
    }
}

static void coal_lea_instr(Ctx ctx, AsmLea* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->src->get._AsmPseudo.name, src_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->src);
        }
    }
    if (node->dst->type == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->dst->get._AsmPseudo.name, dst_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->dst);
        }
    }
}

static void coal_cvttsd2si_instr(Ctx ctx, AsmCvttsd2si* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->src->get._AsmPseudo.name, src_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->src);
        }
    }
    if (node->dst->type == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->dst->get._AsmPseudo.name, dst_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->dst);
        }
    }
}

static void coal_cvtsi2sd_instr(Ctx ctx, AsmCvtsi2sd* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->src->get._AsmPseudo.name, src_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->src);
        }
    }
    if (node->dst->type == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->dst->get._AsmPseudo.name, dst_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->dst);
        }
    }
}

static void coal_unary_instr(Ctx ctx, AsmUnary* node) {
    if (node->dst->type == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->dst->get._AsmPseudo.name, dst_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->dst);
        }
    }
}

static void coal_binary_instr(Ctx ctx, AsmBinary* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->src->get._AsmPseudo.name, src_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->src);
        }
    }
    if (node->dst->type == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->dst->get._AsmPseudo.name, dst_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->dst);
        }
    }
}

static void coal_cmp_instr(Ctx ctx, AsmCmp* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->src->get._AsmPseudo.name, src_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->src);
        }
    }
    if (node->dst->type == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->dst->get._AsmPseudo.name, dst_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->dst);
        }
    }
}

static void coal_idiv_instr(Ctx ctx, AsmIdiv* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->src->get._AsmPseudo.name, src_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->src);
        }
    }
}

static void coal_div_instr(Ctx ctx, AsmDiv* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->src->get._AsmPseudo.name, src_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->src);
        }
    }
}

static void coal_set_cc_instr(Ctx ctx, AsmSetCC* node) {
    if (node->dst->type == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->dst->get._AsmPseudo.name, dst_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->dst);
        }
    }
}

static void coal_push_instr(Ctx ctx, AsmPush* node) {
    if (node->src->type == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src);
        shared_ptr_t(AsmOperand) op_reg = coal_op_reg(ctx, node->src->get._AsmPseudo.name, src_idx);
        if (op_reg) {
            sptr_move(AsmOperand, op_reg, node->src);
        }
    }
}

static void coal_instr(Ctx ctx, size_t instr_idx, size_t block_id) {
    AsmInstruction* node = GET_INSTR(instr_idx);
    switch (node->type) {
        case AST_AsmMov_t:
            coal_mov_instr(ctx, &node->get._AsmMov, instr_idx, block_id);
            break;
        case AST_AsmMovSx_t:
            coal_mov_sx_instr(ctx, &node->get._AsmMovSx);
            break;
        case AST_AsmMovZeroExtend_t:
            coal_zero_extend_instr(ctx, &node->get._AsmMovZeroExtend);
            break;
        case AST_AsmLea_t:
            coal_lea_instr(ctx, &node->get._AsmLea);
            break;
        case AST_AsmCvttsd2si_t:
            coal_cvttsd2si_instr(ctx, &node->get._AsmCvttsd2si);
            break;
        case AST_AsmCvtsi2sd_t:
            coal_cvtsi2sd_instr(ctx, &node->get._AsmCvtsi2sd);
            break;
        case AST_AsmUnary_t:
            coal_unary_instr(ctx, &node->get._AsmUnary);
            break;
        case AST_AsmBinary_t:
            coal_binary_instr(ctx, &node->get._AsmBinary);
            break;
        case AST_AsmCmp_t:
            coal_cmp_instr(ctx, &node->get._AsmCmp);
            break;
        case AST_AsmIdiv_t:
            coal_idiv_instr(ctx, &node->get._AsmIdiv);
            break;
        case AST_AsmDiv_t:
            coal_div_instr(ctx, &node->get._AsmDiv);
            break;
        case AST_AsmSetCC_t:
            coal_set_cc_instr(ctx, &node->get._AsmSetCC);
            break;
        case AST_AsmPush_t:
            coal_push_instr(ctx, &node->get._AsmPush);
            break;
        case AST_AsmCdq_t:
        case AST_AsmCall_t:
            break;
        default:
            break;
    }
}

static bool coalesce_registers(Ctx ctx) {
    {
        size_t open_data_map_size = ctx->dfa->set_size - REGISTER_MASK_SIZE;
        if (vec_size(ctx->dfa->open_data_map) < open_data_map_size) {
            vec_resize(ctx->dfa->open_data_map, open_data_map_size);
        }
    }
    for (size_t i = REGISTER_MASK_SIZE; i < ctx->dfa->set_size; ++i) {
        ctx->dfa->open_data_map[i - REGISTER_MASK_SIZE] = i;
    }

    {
        bool is_fixed_point = true;
        for (size_t instr_idx = 0; instr_idx < vec_size(*ctx->p_instrs); ++instr_idx) {
            if (GET_INSTR(instr_idx) && GET_INSTR(instr_idx)->type == AST_AsmMov_t
                && coal_infer_regs(ctx, &GET_INSTR(instr_idx)->get._AsmMov)) {
                is_fixed_point = false;
            }
        }
        if (is_fixed_point) {
            return false;
        }
    }

    for (size_t block_id = 0; block_id < vec_size(ctx->cfg->blocks); ++block_id) {
        if (GET_CFG_BLOCK(block_id).size > 0) {
            for (size_t instr_idx = GET_CFG_BLOCK(block_id).instrs_front_idx;
                 instr_idx <= GET_CFG_BLOCK(block_id).instrs_back_idx; ++instr_idx) {
                if (GET_INSTR(instr_idx)) {
                    coal_instr(ctx, instr_idx, block_id);
                }
            }
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void alloc_fun_toplvl(Ctx ctx, AsmFunction* node) {
    ctx->p_instrs = &node->instructions;
    init_control_flow_graph(ctx);
Ldowhile:
    if (init_inference_graph(ctx, node->name)) {
        if (ctx->is_with_coal && coalesce_registers(ctx)) {
            if (vec_empty(ctx->infer_graph->unpruned_pseudo_names)
                && vec_empty(ctx->sse_infer_graph->unpruned_pseudo_names)) {
                goto Lbreak;
            }
            goto Ldowhile;
        }
        {
            BackendFun* backend_fun = &map_get(ctx->backend->symbol_table, node->name)->get._BackendFun;
            ctx->p_backend_fun = backend_fun;
        }
        reallocate_registers(ctx);
        ctx->p_backend_fun = NULL;
    }
Lbreak:
    ctx->p_infer_graph = NULL;
    ctx->p_instrs = NULL;
}

static void alloc_toplvl(Ctx ctx, AsmTopLevel* node) {
    switch (node->type) {
        case AST_AsmFunction_t:
            alloc_fun_toplvl(ctx, &node->get._AsmFunction);
            break;
        case AST_AsmStaticVariable_t:
            break;
        default:
            THROW_ABORT;
    }
}

static void alloc_program(Ctx ctx, AsmProgram* node) {
    for (size_t i = 0; i < vec_size(node->top_levels); ++i) {
        alloc_toplvl(ctx, node->top_levels[i]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void allocate_registers(AsmProgram* node, BackEndContext* backend, FrontEndContext* frontend, uint8_t optim_2_code) {
    RegAllocContext ctx;
    {
        ctx.backend = backend;
        ctx.frontend = frontend;
        ctx.is_with_coal = optim_2_code > 1u;

        ctx.hard_regs[0].reg_kind = REG_Ax;
        ctx.hard_regs[1].reg_kind = REG_Bx;
        ctx.hard_regs[2].reg_kind = REG_Cx;
        ctx.hard_regs[3].reg_kind = REG_Dx;
        ctx.hard_regs[4].reg_kind = REG_Di;
        ctx.hard_regs[5].reg_kind = REG_Si;
        ctx.hard_regs[6].reg_kind = REG_R8;
        ctx.hard_regs[7].reg_kind = REG_R9;
        ctx.hard_regs[8].reg_kind = REG_R12;
        ctx.hard_regs[9].reg_kind = REG_R13;
        ctx.hard_regs[10].reg_kind = REG_R14;
        ctx.hard_regs[11].reg_kind = REG_R15;

        ctx.hard_regs[12].reg_kind = REG_Xmm0;
        ctx.hard_regs[13].reg_kind = REG_Xmm1;
        ctx.hard_regs[14].reg_kind = REG_Xmm2;
        ctx.hard_regs[15].reg_kind = REG_Xmm3;
        ctx.hard_regs[16].reg_kind = REG_Xmm4;
        ctx.hard_regs[17].reg_kind = REG_Xmm5;
        ctx.hard_regs[18].reg_kind = REG_Xmm6;
        ctx.hard_regs[19].reg_kind = REG_Xmm7;
        ctx.hard_regs[20].reg_kind = REG_Xmm8;
        ctx.hard_regs[21].reg_kind = REG_Xmm9;
        ctx.hard_regs[22].reg_kind = REG_Xmm10;
        ctx.hard_regs[23].reg_kind = REG_Xmm11;
        ctx.hard_regs[24].reg_kind = REG_Xmm12;
        ctx.hard_regs[25].reg_kind = REG_Xmm13;

        for (size_t i = 0; i < 26; ++i) {
            ctx.hard_regs[i].linked_pseudo_names = vec_new();
        }

        ctx.cfg = make_ControlFlowGraph();
        ctx.dfa = make_DataFlowAnalysis();
        ctx.dfa_o2 = make_DataFlowAnalysisO2();
        ctx.infer_graph = make_InferenceGraph(false);
        ctx.sse_infer_graph = make_InferenceGraph(true);
    }
    alloc_program(&ctx, node);

    for (size_t i = 0; i < 26; ++i) {
        vec_delete(ctx.hard_regs[i].linked_pseudo_names);
    }

    free_ControlFlowGraph(&ctx.cfg);
    free_DataFlowAnalysis(&ctx.dfa);
    free_DataFlowAnalysisO2(&ctx.dfa_o2);
    free_InferenceGraph(&ctx.infer_graph);
    free_InferenceGraph(&ctx.sse_infer_graph);
}
