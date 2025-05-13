#include <algorithm>
#include <array>
#include <inttypes.h>
#include <memory>
#include <unordered_map>
#include <vector>

#include "util/throw.hpp"

#include "ast/back_ast.hpp"
#include "ast/back_symt.hpp"
#include "ast/front_symt.hpp"
#include "ast_t.hpp" // ast

#include "backend/assembly/registers.hpp"

#include "optimization/reg_alloc.hpp"

typedef TULong mask_t;

struct ControlFlowGraph;
struct DataFlowAnalysis;
struct DataFlowAnalysisO2;

struct InferenceRegister {
    REGISTER_KIND color;
    REGISTER_KIND reg_kind;
    size_t degree;
    size_t spill_cost;
    mask_t linked_hard_mask;
    std::vector<TIdentifier> linked_pseudo_names;
};

struct InferenceGraph {
    size_t k;
    size_t offset;
    mask_t hard_reg_mask;
    std::vector<size_t> unpruned_hard_mask_bits;
    std::vector<TIdentifier> unpruned_pseudo_names;
    std::unordered_map<TIdentifier, InferenceRegister> pseudo_reg_map;
};

struct RegAllocContext {
    BackEndContext* backend;
    FrontEndContext* frontend;
    // Register allocation
    mask_t callee_saved_reg_mask;
    BackendFun* p_backend_fun;
    InferenceGraph* p_infer_graph;
    std::array<REGISTER_KIND, 26> reg_color_map;
    std::array<InferenceRegister, 26> hard_regs;
    std::unique_ptr<ControlFlowGraph> cfg;
    std::unique_ptr<DataFlowAnalysis> dfa;
    std::unique_ptr<DataFlowAnalysisO2> dfa_o2;
    std::unique_ptr<InferenceGraph> infer_graph;
    std::unique_ptr<InferenceGraph> sse_infer_graph;
    std::vector<std::unique_ptr<AsmInstruction>>* p_instrs;
    // Register coalescing
    bool is_with_coal;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register allocation

#ifndef __OPTIM_LEVEL__
#define __OPTIM_LEVEL__ 2
#undef _OPTIMIZATION_IMPL_OLVL_H
#include "impl_olvl.hpp" // optimization
#undef __OPTIM_LEVEL__
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Inference graph

static bool is_bitshift_cl(AsmBinary* node) {
    switch (node->binop->type()) {
        case AST_AsmBitShiftLeft_t:
        case AST_AsmBitShiftRight_t:
        case AST_AsmBitShrArithmetic_t:
            return node->src->type() != AST_AsmImm_t;
        default:
            return false;
    }
}

static void infer_transfer_used_reg(Ctx ctx, REGISTER_KIND reg_kind, size_t next_instr_idx) {
    SET_DFA_INSTR_SET_AT(next_instr_idx, register_mask_bit(reg_kind), true);
}

static void infer_transfer_used_name(Ctx ctx, TIdentifier name, size_t next_instr_idx) {
    if (!is_aliased_name(ctx, name)) {
        size_t i = ctx->cfg->identifier_id_map[name];
        SET_DFA_INSTR_SET_AT(next_instr_idx, i, true);
    }
}

static void infer_transfer_used_op(Ctx ctx, AsmOperand* node, size_t next_instr_idx) {
    switch (node->type()) {
        case AST_AsmRegister_t: {
            REGISTER_KIND reg_kind = register_mask_kind(static_cast<AsmRegister*>(node)->reg.get());
            if (reg_kind != REG_Sp) {
                infer_transfer_used_reg(ctx, reg_kind, next_instr_idx);
            }
            break;
        }
        case AST_AsmPseudo_t:
            infer_transfer_used_name(ctx, static_cast<AsmPseudo*>(node)->name, next_instr_idx);
            break;
        case AST_AsmMemory_t: {
            REGISTER_KIND reg_kind = register_mask_kind(static_cast<AsmMemory*>(node)->reg.get());
            if (reg_kind != REG_Sp) {
                infer_transfer_used_reg(ctx, reg_kind, next_instr_idx);
            }
            break;
        }
        case AST_AsmIndexed_t: {
            AsmIndexed* p_node = static_cast<AsmIndexed*>(node);
            {
                REGISTER_KIND reg_kind = register_mask_kind(p_node->reg_base.get());
                infer_transfer_used_reg(ctx, reg_kind, next_instr_idx);
            }
            {
                REGISTER_KIND reg_kind = register_mask_kind(p_node->reg_index.get());
                infer_transfer_used_reg(ctx, reg_kind, next_instr_idx);
            }
            break;
        }
        default:
            break;
    }
}

static void infer_transfer_used_call(Ctx ctx, AsmCall* node, size_t next_instr_idx) {
    FunType* fun_type = static_cast<FunType*>(ctx->frontend->symbol_table[node->name]->type_t.get());
    GET_DFA_INSTR_SET_MASK(next_instr_idx, 0) |= fun_type->param_reg_mask;
}

static void infer_transfer_updated_reg(Ctx ctx, REGISTER_KIND reg_kind, size_t next_instr_idx) {
    SET_DFA_INSTR_SET_AT(next_instr_idx, register_mask_bit(reg_kind), false);
}

static void infer_transfer_updated_name(Ctx ctx, TIdentifier name, size_t next_instr_idx) {
    if (!is_aliased_name(ctx, name)) {
        size_t i = ctx->cfg->identifier_id_map[name];
        SET_DFA_INSTR_SET_AT(next_instr_idx, i, false);
    }
}

static void infer_transfer_updated_op(Ctx ctx, AsmOperand* node, size_t next_instr_idx) {
    switch (node->type()) {
        case AST_AsmRegister_t: {
            REGISTER_KIND reg_kind = register_mask_kind(static_cast<AsmRegister*>(node)->reg.get());
            if (reg_kind != REG_Sp) {
                infer_transfer_updated_reg(ctx, reg_kind, next_instr_idx);
            }
            break;
        }
        case AST_AsmPseudo_t:
            infer_transfer_updated_name(ctx, static_cast<AsmPseudo*>(node)->name, next_instr_idx);
            break;
        case AST_AsmMemory_t: {
            REGISTER_KIND reg_kind = register_mask_kind(static_cast<AsmMemory*>(node)->reg.get());
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
    AsmInstruction* node = GET_INSTR(instr_idx).get();
    switch (node->type()) {
        case AST_AsmMov_t: {
            AsmMov* p_node = static_cast<AsmMov*>(node);
            infer_transfer_updated_op(ctx, p_node->dst.get(), next_instr_idx);
            infer_transfer_used_op(ctx, p_node->src.get(), next_instr_idx);
            break;
        }
        case AST_AsmMovSx_t: {
            AsmMovSx* p_node = static_cast<AsmMovSx*>(node);
            infer_transfer_updated_op(ctx, p_node->dst.get(), next_instr_idx);
            infer_transfer_used_op(ctx, p_node->src.get(), next_instr_idx);
            break;
        }
        case AST_AsmMovZeroExtend_t: {
            AsmMovZeroExtend* p_node = static_cast<AsmMovZeroExtend*>(node);
            infer_transfer_updated_op(ctx, p_node->dst.get(), next_instr_idx);
            infer_transfer_used_op(ctx, p_node->src.get(), next_instr_idx);
            break;
        }
        case AST_AsmLea_t: {
            AsmLea* p_node = static_cast<AsmLea*>(node);
            infer_transfer_updated_op(ctx, p_node->dst.get(), next_instr_idx);
            infer_transfer_used_op(ctx, p_node->src.get(), next_instr_idx);
            break;
        }
        case AST_AsmCvttsd2si_t: {
            AsmCvttsd2si* p_node = static_cast<AsmCvttsd2si*>(node);
            infer_transfer_updated_op(ctx, p_node->dst.get(), next_instr_idx);
            infer_transfer_used_op(ctx, p_node->src.get(), next_instr_idx);
            break;
        }
        case AST_AsmCvtsi2sd_t: {
            AsmCvtsi2sd* p_node = static_cast<AsmCvtsi2sd*>(node);
            infer_transfer_updated_op(ctx, p_node->dst.get(), next_instr_idx);
            infer_transfer_used_op(ctx, p_node->src.get(), next_instr_idx);
            break;
        }
        case AST_AsmUnary_t:
            infer_transfer_used_op(ctx, static_cast<AsmUnary*>(node)->dst.get(), next_instr_idx);
            break;
        case AST_AsmBinary_t: {
            AsmBinary* p_node = static_cast<AsmBinary*>(node);
            infer_transfer_used_op(ctx, p_node->src.get(), next_instr_idx);
            infer_transfer_used_op(ctx, p_node->dst.get(), next_instr_idx);
            if (is_bitshift_cl(p_node)) {
                infer_transfer_used_reg(ctx, REG_Cx, next_instr_idx);
            }
            break;
        }
        case AST_AsmCmp_t: {
            AsmCmp* p_node = static_cast<AsmCmp*>(node);
            infer_transfer_used_op(ctx, p_node->src.get(), next_instr_idx);
            infer_transfer_used_op(ctx, p_node->dst.get(), next_instr_idx);
            break;
        }
        case AST_AsmIdiv_t:
            infer_transfer_used_op(ctx, static_cast<AsmIdiv*>(node)->src.get(), next_instr_idx);
            infer_transfer_used_reg(ctx, REG_Ax, next_instr_idx);
            infer_transfer_used_reg(ctx, REG_Dx, next_instr_idx);
            break;
        case AST_AsmDiv_t:
            infer_transfer_used_op(ctx, static_cast<AsmDiv*>(node)->src.get(), next_instr_idx);
            infer_transfer_used_reg(ctx, REG_Ax, next_instr_idx);
            break;
        case AST_AsmCdq_t:
            infer_transfer_updated_reg(ctx, REG_Dx, next_instr_idx);
            infer_transfer_used_reg(ctx, REG_Ax, next_instr_idx);
            break;
        case AST_AsmSetCC_t:
            infer_transfer_updated_op(ctx, static_cast<AsmSetCC*>(node)->dst.get(), next_instr_idx);
            break;
        case AST_AsmPush_t:
            infer_transfer_used_op(ctx, static_cast<AsmPush*>(node)->src.get(), next_instr_idx);
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
            infer_transfer_used_call(ctx, static_cast<AsmCall*>(node), next_instr_idx);
            break;
        default:
            THROW_ABORT;
    }
}

static void set_p_infer_graph(Ctx ctx, bool is_dbl) {
    ctx->p_infer_graph = is_dbl ? ctx->sse_infer_graph.get() : ctx->infer_graph.get();
}

static void infer_add_pseudo_edges(Ctx ctx, TIdentifier name_1, TIdentifier name_2) {
    {
        InferenceRegister& infer = ctx->p_infer_graph->pseudo_reg_map[name_1];
        if (std::find(infer.linked_pseudo_names.begin(), infer.linked_pseudo_names.end(), name_2)
            == infer.linked_pseudo_names.end()) {
            infer.linked_pseudo_names.push_back(name_2);
            infer.degree++;
        }
    }
    {
        InferenceRegister& infer = ctx->p_infer_graph->pseudo_reg_map[name_2];
        if (std::find(infer.linked_pseudo_names.begin(), infer.linked_pseudo_names.end(), name_1)
            == infer.linked_pseudo_names.end()) {
            infer.linked_pseudo_names.push_back(name_1);
            infer.degree++;
        }
    }
}

static void infer_add_reg_edge(Ctx ctx, REGISTER_KIND reg_kind, TIdentifier name) {
    {
        InferenceRegister& infer = ctx->p_infer_graph->pseudo_reg_map[name];
        if (!register_mask_get(infer.linked_hard_mask, reg_kind)) {
            register_mask_set(infer.linked_hard_mask, reg_kind, true);
            infer.degree++;
        }
    }
    {
        InferenceRegister& infer = ctx->hard_regs[register_mask_bit(reg_kind)];
        if (std::find(infer.linked_pseudo_names.begin(), infer.linked_pseudo_names.end(), name)
            == infer.linked_pseudo_names.end()) {
            infer.linked_pseudo_names.push_back(name);
            infer.degree++;
        }
    }
}

static void infer_rm_pseudo_edge(InferenceRegister& infer, TIdentifier name) {
    for (size_t i = infer.linked_pseudo_names.size(); i-- > 0;) {
        if (infer.linked_pseudo_names[i] == name) {
            std::swap(infer.linked_pseudo_names[i], infer.linked_pseudo_names.back());
            infer.linked_pseudo_names.pop_back();
            infer.degree--;
            return;
        }
    }
    THROW_ABORT;
}

static void infer_rm_unpruned_pseudo_name(Ctx ctx, TIdentifier name) {
    for (size_t i = ctx->p_infer_graph->unpruned_pseudo_names.size(); i-- > 0;) {
        if (ctx->p_infer_graph->unpruned_pseudo_names[i] == name) {
            std::swap(ctx->p_infer_graph->unpruned_pseudo_names[i], ctx->p_infer_graph->unpruned_pseudo_names.back());
            ctx->p_infer_graph->unpruned_pseudo_names.pop_back();
            return;
        }
    }
    THROW_ABORT;
}

static void infer_init_used_name_edges(Ctx ctx, TIdentifier name) {
    if (!is_aliased_name(ctx, name)) {
        set_p_infer_graph(ctx, ctx->frontend->symbol_table[name]->type_t->type() == AST_Double_t);
        ctx->p_infer_graph->pseudo_reg_map[name].spill_cost++;
    }
}

static void infer_init_used_op_edges(Ctx ctx, AsmOperand* node) {
    if (node->type() == AST_AsmPseudo_t) {
        infer_init_used_name_edges(ctx, static_cast<AsmPseudo*>(node)->name);
    }
}

static void infer_init_updated_regs_edges(
    Ctx ctx, REGISTER_KIND* reg_kinds, size_t instr_idx, size_t reg_kinds_size, bool is_dbl) {

    size_t mov_mask_bit = ctx->dfa->set_size;
    bool is_mov = GET_INSTR(instr_idx)->type() == AST_AsmMov_t;
    if (is_mov) {
        AsmMov* mov = static_cast<AsmMov*>(GET_INSTR(instr_idx).get());
        if (mov->src->type() == AST_AsmPseudo_t) {
            TIdentifier src_name = static_cast<AsmPseudo*>(mov->src.get())->name;
            if (is_aliased_name(ctx, src_name)) {
                is_mov = false;
            }
            else {
                bool is_src_dbl = ctx->frontend->symbol_table[src_name]->type_t->type() == AST_Double_t;
                set_p_infer_graph(ctx, is_src_dbl);
                ctx->p_infer_graph->pseudo_reg_map[src_name].spill_cost++;
                mov_mask_bit = ctx->cfg->identifier_id_map[src_name];
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
                if (is_dbl == (ctx->frontend->symbol_table[pseudo_name]->type_t->type() == AST_Double_t)) {
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
                if (is_dbl == (ctx->frontend->symbol_table[pseudo_name]->type_t->type() == AST_Double_t)) {
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
    bool is_dbl = ctx->frontend->symbol_table[name]->type_t->type() == AST_Double_t;

    size_t mov_mask_bit = ctx->dfa->set_size;
    bool is_mov = GET_INSTR(instr_idx)->type() == AST_AsmMov_t;
    if (is_mov) {
        AsmMov* mov = static_cast<AsmMov*>(GET_INSTR(instr_idx).get());
        switch (mov->src->type()) {
            case AST_AsmRegister_t: {
                REGISTER_KIND src_reg_kind = register_mask_kind(static_cast<AsmRegister*>(mov->src.get())->reg.get());
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
                TIdentifier src_name = static_cast<AsmPseudo*>(mov->src.get())->name;
                if (is_aliased_name(ctx, src_name)) {
                    is_mov = false;
                }
                else {
                    bool is_src_dbl = ctx->frontend->symbol_table[src_name]->type_t->type() == AST_Double_t;
                    set_p_infer_graph(ctx, is_src_dbl);
                    ctx->p_infer_graph->pseudo_reg_map[src_name].spill_cost++;
                    mov_mask_bit = ctx->cfg->identifier_id_map[src_name];
                    is_mov = is_dbl == is_src_dbl;
                }
                break;
            }
            case AST_AsmMemory_t: {
                REGISTER_KIND src_reg_kind = register_mask_kind(static_cast<AsmMemory*>(mov->src.get())->reg.get());
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
    ctx->p_infer_graph->pseudo_reg_map[name].spill_cost++;

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
                    && is_dbl == (ctx->frontend->symbol_table[pseudo_name]->type_t->type() == AST_Double_t)) {
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
                    && is_dbl == (ctx->frontend->symbol_table[pseudo_name]->type_t->type() == AST_Double_t)) {
                    infer_add_pseudo_edges(ctx, name, pseudo_name);
                }
            }
        }
    }
}

static void infer_init_updated_op_edges(Ctx ctx, AsmOperand* node, size_t instr_idx) {
    switch (node->type()) {
        case AST_AsmRegister_t: {
            REGISTER_KIND reg_kinds[1] = {register_mask_kind(static_cast<AsmRegister*>(node)->reg.get())};
            if (reg_kinds[0] != REG_Sp) {
                bool is_dbl = register_mask_bit(reg_kinds[0]) > 11;
                infer_init_updated_regs_edges(ctx, reg_kinds, instr_idx, 1, is_dbl);
            }
            break;
        }
        case AST_AsmPseudo_t:
            infer_init_updated_name_edges(ctx, static_cast<AsmPseudo*>(node)->name, instr_idx);
            break;
        default:
            break;
    }
}

static void infer_init_edges(Ctx ctx, size_t instr_idx) {
    AsmInstruction* node = GET_INSTR(instr_idx).get();
    switch (node->type()) {
        case AST_AsmMov_t:
            infer_init_updated_op_edges(ctx, static_cast<AsmMov*>(node)->dst.get(), instr_idx);
            break;
        case AST_AsmMovSx_t:
            infer_init_updated_op_edges(ctx, static_cast<AsmMovSx*>(node)->dst.get(), instr_idx);
            break;
        case AST_AsmMovZeroExtend_t:
            infer_init_updated_op_edges(ctx, static_cast<AsmMovZeroExtend*>(node)->dst.get(), instr_idx);
            break;
        case AST_AsmLea_t:
            infer_init_updated_op_edges(ctx, static_cast<AsmLea*>(node)->dst.get(), instr_idx);
            break;
        case AST_AsmCvttsd2si_t:
            infer_init_updated_op_edges(ctx, static_cast<AsmCvttsd2si*>(node)->dst.get(), instr_idx);
            break;
        case AST_AsmCvtsi2sd_t:
            infer_init_updated_op_edges(ctx, static_cast<AsmCvtsi2sd*>(node)->dst.get(), instr_idx);
            break;
        case AST_AsmUnary_t:
            infer_init_updated_op_edges(ctx, static_cast<AsmUnary*>(node)->dst.get(), instr_idx);
            break;
        case AST_AsmBinary_t: {
            AsmBinary* p_node = static_cast<AsmBinary*>(node);
            if (is_bitshift_cl(p_node)) {
                REGISTER_KIND reg_kinds[1] = {REG_Cx};
                infer_init_updated_regs_edges(ctx, reg_kinds, instr_idx, 1, false);
            }
            infer_init_updated_op_edges(ctx, p_node->dst.get(), instr_idx);
            infer_init_used_op_edges(ctx, p_node->src.get());
            break;
        }
        case AST_AsmCmp_t: {
            AsmCmp* p_node = static_cast<AsmCmp*>(node);
            infer_init_used_op_edges(ctx, p_node->src.get());
            infer_init_used_op_edges(ctx, p_node->dst.get());
            break;
        }
        case AST_AsmIdiv_t: {
            REGISTER_KIND reg_kinds[2] = {REG_Ax, REG_Dx};
            infer_init_updated_regs_edges(ctx, reg_kinds, instr_idx, 2, false);
            infer_init_used_op_edges(ctx, static_cast<AsmIdiv*>(node)->src.get());
            break;
        }
        case AST_AsmDiv_t: {
            REGISTER_KIND reg_kinds[1] = {REG_Ax};
            infer_init_updated_regs_edges(ctx, reg_kinds, instr_idx, 1, false);
            infer_init_used_op_edges(ctx, static_cast<AsmDiv*>(node)->src.get());
            break;
        }
        case AST_AsmCdq_t: {
            REGISTER_KIND reg_kinds[1] = {REG_Dx};
            infer_init_updated_regs_edges(ctx, reg_kinds, instr_idx, 1, false);
            break;
        }
        case AST_AsmSetCC_t:
            infer_init_updated_op_edges(ctx, static_cast<AsmSetCC*>(node)->dst.get(), instr_idx);
            break;
        case AST_AsmPush_t:
            infer_init_used_op_edges(ctx, static_cast<AsmPush*>(node)->src.get());
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
    if (ctx->cfg->identifier_id_map.empty()) {
        return false;
    }

    ctx->callee_saved_reg_mask = REGISTER_MASK_FALSE;
    ctx->infer_graph->unpruned_pseudo_names.clear();
    ctx->infer_graph->pseudo_reg_map.clear();
    ctx->sse_infer_graph->unpruned_pseudo_names.clear();
    ctx->sse_infer_graph->pseudo_reg_map.clear();
    for (const auto& name_id : ctx->cfg->identifier_id_map) {
        TIdentifier name = name_id.first;
        InferenceRegister infer = {REG_Sp, REG_Sp, 0, 0, REGISTER_MASK_FALSE, {}};
        if (ctx->frontend->symbol_table[name_id.first]->type_t->type() == AST_Double_t) {
            ctx->sse_infer_graph->unpruned_pseudo_names.push_back(name);
            ctx->sse_infer_graph->pseudo_reg_map[name] = std::move(infer);
        }
        else {
            ctx->infer_graph->unpruned_pseudo_names.push_back(name);
            ctx->infer_graph->pseudo_reg_map[name] = std::move(infer);
        }
    }

    if (!ctx->infer_graph->pseudo_reg_map.empty()) {
        if (ctx->infer_graph->unpruned_hard_mask_bits.size() < 12) {
            ctx->infer_graph->unpruned_hard_mask_bits.resize(12);
        }

        mask_t hard_reg_mask = ctx->infer_graph->hard_reg_mask;
        for (size_t i = 0; i < 12; ++i) {
            ctx->reg_color_map[i] = REG_Sp;
            ctx->hard_regs[i].color = REG_Sp;
            ctx->hard_regs[i].degree = 11;
            ctx->hard_regs[i].spill_cost = 0;
            ctx->hard_regs[i].linked_hard_mask = hard_reg_mask;
            ctx->hard_regs[i].linked_pseudo_names.clear();
            ctx->infer_graph->unpruned_hard_mask_bits[i] = i;
        }
    }
    if (!ctx->sse_infer_graph->pseudo_reg_map.empty()) {
        if (ctx->sse_infer_graph->unpruned_hard_mask_bits.size() < 14) {
            ctx->sse_infer_graph->unpruned_hard_mask_bits.resize(14);
        }

        mask_t hard_reg_mask = ctx->sse_infer_graph->hard_reg_mask;
        for (size_t i = 12; i < 26; ++i) {
            ctx->reg_color_map[i] = REG_Sp;
            ctx->hard_regs[i].color = REG_Sp;
            ctx->hard_regs[i].degree = 13;
            ctx->hard_regs[i].spill_cost = 0;
            ctx->hard_regs[i].linked_hard_mask = hard_reg_mask;
            ctx->hard_regs[i].linked_pseudo_names.clear();
            ctx->sse_infer_graph->unpruned_hard_mask_bits[i - 12] = i;
        }
    }

    for (size_t block_id = 0; block_id < ctx->cfg->blocks.size(); ++block_id) {
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
        std::swap(
            ctx->p_infer_graph->unpruned_pseudo_names[pruned_idx], ctx->p_infer_graph->unpruned_pseudo_names.back());
        ctx->p_infer_graph->unpruned_pseudo_names.pop_back();
    }
    else {
        std::swap(ctx->p_infer_graph->unpruned_hard_mask_bits[pruned_idx],
            ctx->p_infer_graph->unpruned_hard_mask_bits.back());
        ctx->p_infer_graph->unpruned_hard_mask_bits.pop_back();
    }
    if (infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < ctx->p_infer_graph->k; ++i) {
            InferenceRegister& linked_infer = ctx->hard_regs[i + ctx->p_infer_graph->offset];
            if (register_mask_get(infer->linked_hard_mask, linked_infer.reg_kind)) {
                linked_infer.degree--;
            }
        }
    }
    for (TIdentifier name : infer->linked_pseudo_names) {
        ctx->p_infer_graph->pseudo_reg_map[name].degree--;
    }
}

static void alloc_unprune_infer_reg(Ctx ctx, InferenceRegister* infer, TIdentifier pruned_name) {
    if (infer->reg_kind == REG_Sp) {
        THROW_ABORT_IF(std::find(ctx->p_infer_graph->unpruned_pseudo_names.begin(),
                           ctx->p_infer_graph->unpruned_pseudo_names.end(), pruned_name)
                       != ctx->p_infer_graph->unpruned_pseudo_names.end());
        ctx->p_infer_graph->unpruned_pseudo_names.push_back(pruned_name);
    }
    else {
        size_t pruned_mask_bit = register_mask_bit(infer->reg_kind);
        THROW_ABORT_IF(std::find(ctx->p_infer_graph->unpruned_hard_mask_bits.begin(),
                           ctx->p_infer_graph->unpruned_hard_mask_bits.end(), pruned_mask_bit)
                       != ctx->p_infer_graph->unpruned_hard_mask_bits.end());
        ctx->p_infer_graph->unpruned_hard_mask_bits.push_back(pruned_mask_bit);
    }
    if (infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < ctx->p_infer_graph->k; ++i) {
            InferenceRegister& linked_infer = ctx->hard_regs[i + ctx->p_infer_graph->offset];
            if (register_mask_get(infer->linked_hard_mask, linked_infer.reg_kind)) {
                linked_infer.degree++;
            }
        }
    }
    for (TIdentifier name : infer->linked_pseudo_names) {
        ctx->p_infer_graph->pseudo_reg_map[name].degree++;
    }
}

static void alloc_color_infer_graph(Ctx ctx);

static void alloc_next_color_infer_graph(Ctx ctx) {
    if (!ctx->p_infer_graph->unpruned_hard_mask_bits.empty() || !ctx->p_infer_graph->unpruned_pseudo_names.empty()) {
        alloc_color_infer_graph(ctx);
    }
}

static InferenceRegister* alloc_prune_infer_graph(Ctx ctx, TIdentifier& pruned_name) {
    size_t pruned_idx;
    InferenceRegister* infer = nullptr;
    for (size_t i = 0; i < ctx->p_infer_graph->unpruned_pseudo_names.size(); ++i) {
        pruned_name = ctx->p_infer_graph->unpruned_pseudo_names[i];
        infer = &ctx->p_infer_graph->pseudo_reg_map[pruned_name];
        if (infer->degree < ctx->p_infer_graph->k) {
            pruned_idx = i;
            break;
        }
        infer = nullptr;
    }
    if (!infer) {
        for (size_t i = 0; i < ctx->p_infer_graph->unpruned_hard_mask_bits.size(); ++i) {
            size_t pruned_mask_bit = ctx->p_infer_graph->unpruned_hard_mask_bits[i];
            infer = &ctx->hard_regs[pruned_mask_bit];
            if (infer->degree < ctx->p_infer_graph->k) {
                pruned_idx = i;
                break;
            }
            infer = nullptr;
        }
    }
    if (!infer) {
        size_t i = 0;
        for (; i < ctx->p_infer_graph->unpruned_pseudo_names.size(); ++i) {
            pruned_name = ctx->p_infer_graph->unpruned_pseudo_names[i];
            infer = &ctx->p_infer_graph->pseudo_reg_map[pruned_name];
            if (infer->degree > 0) {
                pruned_idx = i;
                break;
            }
            infer = nullptr;
        }
        THROW_ABORT_IF(!infer);
        double min_spill_metric = static_cast<double>(infer->spill_cost) / infer->degree;
        for (; i < ctx->p_infer_graph->unpruned_pseudo_names.size(); ++i) {
            TIdentifier spill_name = ctx->p_infer_graph->unpruned_pseudo_names[i];
            InferenceRegister& spill_infer = ctx->p_infer_graph->pseudo_reg_map[spill_name];
            if (spill_infer.degree > 0) {
                double spill_metric = static_cast<double>(spill_infer.spill_cost) / spill_infer.degree;
                if (spill_metric < min_spill_metric) {
                    pruned_idx = i;
                    pruned_name = spill_name;
                    infer = &spill_infer;
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
            InferenceRegister& linked_infer = ctx->hard_regs[i + ctx->p_infer_graph->offset];
            if (register_mask_get(infer->linked_hard_mask, linked_infer.reg_kind)) {
                if (linked_infer.color != REG_Sp) {
                    register_mask_set(color_reg_mask, linked_infer.color, false);
                }
            }
        }
    }
    for (TIdentifier name : infer->linked_pseudo_names) {
        InferenceRegister& linked_infer = ctx->p_infer_graph->pseudo_reg_map[name];
        if (linked_infer.color != REG_Sp) {
            register_mask_set(color_reg_mask, linked_infer.color, false);
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
    TIdentifier pruned_name;
    InferenceRegister* infer = alloc_prune_infer_graph(ctx, pruned_name);
    alloc_next_color_infer_graph(ctx);
    alloc_unprune_infer_graph(ctx, infer, pruned_name);
}

static void alloc_color_reg_map(Ctx ctx) {
    for (size_t i = 0; i < ctx->p_infer_graph->k; ++i) {
        InferenceRegister& infer = ctx->hard_regs[i + ctx->p_infer_graph->offset];
        if (infer.color != REG_Sp) {
            ctx->reg_color_map[register_mask_bit(infer.color)] = infer.reg_kind;
        }
    }
}

static std::shared_ptr<AsmRegister> alloc_hard_reg(Ctx ctx, TIdentifier name) {
    if (is_aliased_name(ctx, name)) {
        return nullptr;
    }
    set_p_infer_graph(ctx, ctx->frontend->symbol_table[name]->type_t->type() == AST_Double_t);
    REGISTER_KIND color = ctx->p_infer_graph->pseudo_reg_map[name].color;
    if (color != REG_Sp) {
        REGISTER_KIND reg_kind = ctx->reg_color_map[register_mask_bit(color)];
        std::shared_ptr<AsmRegister> hard_reg = gen_register(reg_kind);
        if (is_reg_callee_saved(reg_kind) && !register_mask_get(ctx->callee_saved_reg_mask, reg_kind)) {
            register_mask_set(ctx->callee_saved_reg_mask, reg_kind, true);
            std::shared_ptr<AsmOperand> callee_saved_reg = hard_reg;
            ctx->p_backend_fun->callee_saved_regs.push_back(std::move(callee_saved_reg));
        }
        return hard_reg;
    }
    else {
        return nullptr;
    }
}

static REGISTER_KIND get_op_reg_kind(Ctx ctx, AsmOperand* node) {
    switch (node->type()) {
        case AST_AsmRegister_t:
            return register_mask_kind(static_cast<AsmRegister*>(node)->reg.get());
        case AST_AsmPseudo_t: {
            TIdentifier name = static_cast<AsmPseudo*>(node)->name;
            if (is_aliased_name(ctx, name)) {
                return REG_Sp;
            }
            set_p_infer_graph(ctx, ctx->frontend->symbol_table[name]->type_t->type() == AST_Double_t);
            REGISTER_KIND color = ctx->p_infer_graph->pseudo_reg_map[name].color;
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
    AsmOperand* src_op = static_cast<AsmPseudo*>(node->src.get());
    AsmOperand* dst_op = static_cast<AsmPseudo*>(node->dst.get());
    REGISTER_KIND src_reg_kind = get_op_reg_kind(ctx, src_op);
    REGISTER_KIND dst_reg_kind = get_op_reg_kind(ctx, dst_op);
    if (src_reg_kind != REG_Sp && src_reg_kind == dst_reg_kind) {
        set_instr(ctx, nullptr, instr_idx);
    }
    else {
        if (src_op->type() == AST_AsmPseudo_t) {
            std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(src_op)->name);
            if (hard_reg) {
                node->src = std::move(hard_reg);
            }
        }
        if (dst_op->type() == AST_AsmPseudo_t) {
            std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(dst_op)->name);
            if (hard_reg) {
                node->dst = std::move(hard_reg);
            }
        }
    }
}

static void alloc_mov_sx_instr(Ctx ctx, AsmMovSx* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_reg) {
            node->src = std::move(hard_reg);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_reg) {
            node->dst = std::move(hard_reg);
        }
    }
}

static void alloc_zero_extend_instr(Ctx ctx, AsmMovZeroExtend* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_reg) {
            node->src = std::move(hard_reg);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_reg) {
            node->dst = std::move(hard_reg);
        }
    }
}

static void alloc_lea_instr(Ctx ctx, AsmLea* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_reg) {
            node->src = std::move(hard_reg);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_reg) {
            node->dst = std::move(hard_reg);
        }
    }
}

static void alloc_cvttsd2si_instr(Ctx ctx, AsmCvttsd2si* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_reg) {
            node->src = std::move(hard_reg);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_reg) {
            node->dst = std::move(hard_reg);
        }
    }
}

static void alloc_cvtsi2sd_instr(Ctx ctx, AsmCvtsi2sd* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_reg) {
            node->src = std::move(hard_reg);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_reg) {
            node->dst = std::move(hard_reg);
        }
    }
}

static void alloc_unary_instr(Ctx ctx, AsmUnary* node) {
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_reg) {
            node->dst = std::move(hard_reg);
        }
    }
}

static void alloc_binary_instr(Ctx ctx, AsmBinary* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_reg) {
            node->src = std::move(hard_reg);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_reg) {
            node->dst = std::move(hard_reg);
        }
    }
}

static void alloc_cmp_instr(Ctx ctx, AsmCmp* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_reg) {
            node->src = std::move(hard_reg);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_reg) {
            node->dst = std::move(hard_reg);
        }
    }
}

static void alloc_idiv_instr(Ctx ctx, AsmIdiv* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_reg) {
            node->src = std::move(hard_reg);
        }
    }
}

static void alloc_div_instr(Ctx ctx, AsmDiv* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_reg) {
            node->src = std::move(hard_reg);
        }
    }
}

static void alloc_set_cc_instr(Ctx ctx, AsmSetCC* node) {
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_reg) {
            node->dst = std::move(hard_reg);
        }
    }
}

static void alloc_push_instr(Ctx ctx, AsmPush* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_reg = alloc_hard_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_reg) {
            node->src = std::move(hard_reg);
        }
    }
}

static void alloc_instr(Ctx ctx, size_t instr_idx) {
    AsmInstruction* node = GET_INSTR(instr_idx).get();
    switch (node->type()) {
        case AST_AsmMov_t:
            alloc_mov_instr(ctx, static_cast<AsmMov*>(node), instr_idx);
            break;
        case AST_AsmMovSx_t:
            alloc_mov_sx_instr(ctx, static_cast<AsmMovSx*>(node));
            break;
        case AST_AsmMovZeroExtend_t:
            alloc_zero_extend_instr(ctx, static_cast<AsmMovZeroExtend*>(node));
            break;
        case AST_AsmLea_t:
            alloc_lea_instr(ctx, static_cast<AsmLea*>(node));
            break;
        case AST_AsmCvttsd2si_t:
            alloc_cvttsd2si_instr(ctx, static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_AsmCvtsi2sd_t:
            alloc_cvtsi2sd_instr(ctx, static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_AsmUnary_t:
            alloc_unary_instr(ctx, static_cast<AsmUnary*>(node));
            break;
        case AST_AsmBinary_t:
            alloc_binary_instr(ctx, static_cast<AsmBinary*>(node));
            break;
        case AST_AsmCmp_t:
            alloc_cmp_instr(ctx, static_cast<AsmCmp*>(node));
            break;
        case AST_AsmIdiv_t:
            alloc_idiv_instr(ctx, static_cast<AsmIdiv*>(node));
            break;
        case AST_AsmDiv_t:
            alloc_div_instr(ctx, static_cast<AsmDiv*>(node));
            break;
        case AST_AsmSetCC_t:
            alloc_set_cc_instr(ctx, static_cast<AsmSetCC*>(node));
            break;
        case AST_AsmPush_t:
            alloc_push_instr(ctx, static_cast<AsmPush*>(node));
            break;
        case AST_AsmCdq_t:
        case AST_AsmCall_t:
            break;
        default:
            break;
    }
}

static void reallocate_registers(Ctx ctx) {
    if (!ctx->infer_graph->unpruned_pseudo_names.empty()) {
        set_p_infer_graph(ctx, false);
        alloc_color_infer_graph(ctx);
        alloc_color_reg_map(ctx);
    }
    if (!ctx->sse_infer_graph->unpruned_pseudo_names.empty()) {
        set_p_infer_graph(ctx, true);
        alloc_color_infer_graph(ctx);
        alloc_color_reg_map(ctx);
    }
    for (size_t instr_idx = 0; instr_idx < ctx->p_instrs->size(); ++instr_idx) {
        if (GET_INSTR(instr_idx)) {
            alloc_instr(ctx, instr_idx);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register coalescing

static TInt get_type_size(Type* type) {
    switch (type->type()) {
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
    switch (node->type()) {
        case AST_AsmRegister_t: {
            REGISTER_KIND reg_kind = register_mask_kind(static_cast<AsmRegister*>(node)->reg.get());
            if (reg_kind != REG_Sp) {
                coalesced_idx = register_mask_bit(reg_kind);
            }
            break;
        }
        case AST_AsmPseudo_t: {
            TIdentifier name = static_cast<AsmPseudo*>(node)->name;
            if (!is_aliased_name(ctx, name)) {
                coalesced_idx = ctx->cfg->identifier_id_map[name];
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
    Ctx ctx, InferenceRegister*& src_infer, InferenceRegister*& dst_infer, size_t src_idx, size_t dst_idx) {
    if (src_idx != dst_idx && (src_idx >= REGISTER_MASK_SIZE || dst_idx >= REGISTER_MASK_SIZE)
        && src_idx < ctx->dfa->set_size && dst_idx < ctx->dfa->set_size) {
        if (src_idx < REGISTER_MASK_SIZE) {
            TIdentifier dst_name = ctx->dfa_o2->data_name_map[dst_idx - REGISTER_MASK_SIZE];
            bool is_dbl = ctx->frontend->symbol_table[dst_name]->type_t->type() == AST_Double_t;
            if (is_dbl == (src_idx > 11)) {
                set_p_infer_graph(ctx, is_dbl);
                src_infer = &ctx->hard_regs[src_idx];
                dst_infer = &ctx->p_infer_graph->pseudo_reg_map[dst_name];
                return !register_mask_get(dst_infer->linked_hard_mask, src_infer->reg_kind);
            }
        }
        else if (dst_idx < REGISTER_MASK_SIZE) {
            TIdentifier src_name = ctx->dfa_o2->data_name_map[src_idx - REGISTER_MASK_SIZE];
            bool is_dbl = ctx->frontend->symbol_table[src_name]->type_t->type() == AST_Double_t;
            if (is_dbl == (dst_idx > 11)) {
                set_p_infer_graph(ctx, is_dbl);
                src_infer = &ctx->p_infer_graph->pseudo_reg_map[src_name];
                dst_infer = &ctx->hard_regs[dst_idx];
                return !register_mask_get(src_infer->linked_hard_mask, dst_infer->reg_kind);
            }
        }
        else {
            TIdentifier src_name = ctx->dfa_o2->data_name_map[src_idx - REGISTER_MASK_SIZE];
            TIdentifier dst_name = ctx->dfa_o2->data_name_map[dst_idx - REGISTER_MASK_SIZE];
            bool is_dbl = ctx->frontend->symbol_table[src_name]->type_t->type() == AST_Double_t;
            if (is_dbl == (ctx->frontend->symbol_table[dst_name]->type_t->type() == AST_Double_t)
                && get_type_size(ctx->frontend->symbol_table[src_name]->type_t.get())
                       == get_type_size(ctx->frontend->symbol_table[dst_name]->type_t.get())) {
                set_p_infer_graph(ctx, is_dbl);
                src_infer = &ctx->p_infer_graph->pseudo_reg_map[src_name];
                dst_infer = &ctx->p_infer_graph->pseudo_reg_map[dst_name];
                return std::find(dst_infer->linked_pseudo_names.begin(), dst_infer->linked_pseudo_names.end(), src_name)
                       == dst_infer->linked_pseudo_names.end();
            }
        }
    }
    return false;
}

static bool coal_briggs_test(Ctx ctx, InferenceRegister* src_infer, InferenceRegister* dst_infer) {
    size_t degree = 0;

    if (src_infer->linked_hard_mask != REGISTER_MASK_FALSE || dst_infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < ctx->p_infer_graph->k; ++i) {
            InferenceRegister& linked_infer = ctx->hard_regs[i + ctx->p_infer_graph->offset];
            if (register_mask_get(src_infer->linked_hard_mask, linked_infer.reg_kind)) {
                if (register_mask_get(dst_infer->linked_hard_mask, linked_infer.reg_kind)) {
                    if (linked_infer.degree > ctx->p_infer_graph->k) {
                        degree++;
                    }
                }
                else if (linked_infer.degree >= ctx->p_infer_graph->k) {
                    degree++;
                }
            }
            else if (register_mask_get(dst_infer->linked_hard_mask, linked_infer.reg_kind)
                     && linked_infer.degree >= ctx->p_infer_graph->k) {
                degree++;
            }
        }
    }

    for (size_t i = 0; i < ctx->dfa->mask_size; ++i) {
        GET_DFA_INSTR_SET_MASK(ctx->dfa->incoming_idx, i) = MASK_FALSE;
    }
    for (TIdentifier name : dst_infer->linked_pseudo_names) {
        size_t i = ctx->cfg->identifier_id_map[name];
        SET_DFA_INSTR_SET_AT(ctx->dfa->incoming_idx, i, true);
    }
    for (TIdentifier name : src_infer->linked_pseudo_names) {
        size_t i = ctx->cfg->identifier_id_map[name];
        InferenceRegister& linked_infer = ctx->p_infer_graph->pseudo_reg_map[name];
        if (GET_DFA_INSTR_SET_AT(ctx->dfa->incoming_idx, i)) {
            if (linked_infer.degree > ctx->p_infer_graph->k) {
                degree++;
            }
            SET_DFA_INSTR_SET_AT(ctx->dfa->incoming_idx, i, false);
        }
        else if (linked_infer.degree >= ctx->p_infer_graph->k) {
            degree++;
        }
    }
    for (TIdentifier name : dst_infer->linked_pseudo_names) {
        size_t i = ctx->cfg->identifier_id_map[name];
        if (GET_DFA_INSTR_SET_AT(ctx->dfa->incoming_idx, i)) {
            InferenceRegister& linked_infer = ctx->p_infer_graph->pseudo_reg_map[name];
            if (linked_infer.degree >= ctx->p_infer_graph->k) {
                degree++;
            }
        }
    }

    return degree < ctx->p_infer_graph->k;
}

static bool coal_george_test(Ctx ctx, REGISTER_KIND reg_kind, InferenceRegister* infer) {
    for (TIdentifier name : infer->linked_pseudo_names) {
        InferenceRegister& linked_infer = ctx->p_infer_graph->pseudo_reg_map[name];
        if (!register_mask_get(linked_infer.linked_hard_mask, reg_kind)
            && linked_infer.degree >= ctx->p_infer_graph->k) {
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
            InferenceRegister& linked_infer = ctx->hard_regs[i + ctx->p_infer_graph->offset];
            if (register_mask_get(infer->linked_hard_mask, linked_infer.reg_kind)) {
                infer_rm_pseudo_edge(linked_infer, merge_name);
                infer_add_reg_edge(ctx, linked_infer.reg_kind, keep_name);
            }
        }
    }
    for (TIdentifier name : infer->linked_pseudo_names) {
        InferenceRegister& linked_infer = ctx->p_infer_graph->pseudo_reg_map[name];
        infer_rm_pseudo_edge(linked_infer, merge_name);
        infer_add_pseudo_edges(ctx, keep_name, name);
    }
    infer_rm_unpruned_pseudo_name(ctx, merge_name);
}

static void coal_hard_infer_reg(Ctx ctx, REGISTER_KIND reg_kind, InferenceRegister* infer, size_t merge_idx) {
    TIdentifier merge_name = ctx->dfa_o2->data_name_map[merge_idx - REGISTER_MASK_SIZE];
    if (infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < ctx->p_infer_graph->k; ++i) {
            InferenceRegister& linked_infer = ctx->hard_regs[i + ctx->p_infer_graph->offset];
            if (register_mask_get(infer->linked_hard_mask, linked_infer.reg_kind)) {
                infer_rm_pseudo_edge(linked_infer, merge_name);
            }
        }
    }
    for (TIdentifier name : infer->linked_pseudo_names) {
        InferenceRegister& linked_infer = ctx->p_infer_graph->pseudo_reg_map[name];
        infer_rm_pseudo_edge(linked_infer, merge_name);
        infer_add_reg_edge(ctx, reg_kind, name);
    }
    infer_rm_unpruned_pseudo_name(ctx, merge_name);
}

static bool coal_infer_regs(Ctx ctx, AsmMov* node) {
    InferenceRegister* src_infer = nullptr;
    InferenceRegister* dst_infer = nullptr;
    size_t src_idx = get_coalesced_idx(ctx, node->src.get());
    size_t dst_idx = get_coalesced_idx(ctx, node->dst.get());
    if (get_coalescable_infer_regs(ctx, src_infer, dst_infer, src_idx, dst_idx)
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

static std::shared_ptr<AsmOperand> coal_op_reg(Ctx ctx, TIdentifier name, size_t coalesced_idx) {
    if (coalesced_idx < ctx->dfa->set_size && coalesced_idx != ctx->cfg->identifier_id_map[name]) {
        if (coalesced_idx < REGISTER_MASK_SIZE) {
            REGISTER_KIND reg_kind = ctx->hard_regs[coalesced_idx].reg_kind;
            return gen_register(reg_kind);
        }
        else {
            set_p_infer_graph(ctx, ctx->frontend->symbol_table[name]->type_t->type() == AST_Double_t);
            name = ctx->dfa_o2->data_name_map[coalesced_idx - REGISTER_MASK_SIZE];
            ctx->p_infer_graph->pseudo_reg_map[name].spill_cost++;
            return std::make_shared<AsmPseudo>(std::move(name));
        }
    }
    else {
        return nullptr;
    }
}

static void coal_mov_instr(Ctx ctx, AsmMov* node, size_t instr_idx, size_t block_id) {
    size_t src_idx = get_coalesced_idx(ctx, node->src.get());
    size_t dst_idx = get_coalesced_idx(ctx, node->dst.get());
    if (src_idx < ctx->dfa->set_size && src_idx == dst_idx) {
        cfg_rm_block_instr(ctx, instr_idx, block_id);
    }
    else {
        if (node->src->type() == AST_AsmPseudo_t) {
            std::shared_ptr<AsmOperand> op_reg =
                coal_op_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name, src_idx);
            if (op_reg) {
                node->src = std::move(op_reg);
            }
        }
        if (node->dst->type() == AST_AsmPseudo_t) {
            std::shared_ptr<AsmOperand> op_reg =
                coal_op_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name, dst_idx);
            if (op_reg) {
                node->dst = std::move(op_reg);
            }
        }
    }
}

static void coal_mov_sx_instr(Ctx ctx, AsmMovSx* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name, src_idx);
        if (op_reg) {
            node->src = std::move(op_reg);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name, dst_idx);
        if (op_reg) {
            node->dst = std::move(op_reg);
        }
    }
}

static void coal_zero_extend_instr(Ctx ctx, AsmMovZeroExtend* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name, src_idx);
        if (op_reg) {
            node->src = std::move(op_reg);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name, dst_idx);
        if (op_reg) {
            node->dst = std::move(op_reg);
        }
    }
}

static void coal_lea_instr(Ctx ctx, AsmLea* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name, src_idx);
        if (op_reg) {
            node->src = std::move(op_reg);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name, dst_idx);
        if (op_reg) {
            node->dst = std::move(op_reg);
        }
    }
}

static void coal_cvttsd2si_instr(Ctx ctx, AsmCvttsd2si* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name, src_idx);
        if (op_reg) {
            node->src = std::move(op_reg);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name, dst_idx);
        if (op_reg) {
            node->dst = std::move(op_reg);
        }
    }
}

static void coal_cvtsi2sd_instr(Ctx ctx, AsmCvtsi2sd* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name, src_idx);
        if (op_reg) {
            node->src = std::move(op_reg);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name, dst_idx);
        if (op_reg) {
            node->dst = std::move(op_reg);
        }
    }
}

static void coal_unary_instr(Ctx ctx, AsmUnary* node) {
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name, dst_idx);
        if (op_reg) {
            node->dst = std::move(op_reg);
        }
    }
}

static void coal_binary_instr(Ctx ctx, AsmBinary* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name, src_idx);
        if (op_reg) {
            node->src = std::move(op_reg);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name, dst_idx);
        if (op_reg) {
            node->dst = std::move(op_reg);
        }
    }
}

static void coal_cmp_instr(Ctx ctx, AsmCmp* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name, src_idx);
        if (op_reg) {
            node->src = std::move(op_reg);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name, dst_idx);
        if (op_reg) {
            node->dst = std::move(op_reg);
        }
    }
}

static void coal_idiv_instr(Ctx ctx, AsmIdiv* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name, src_idx);
        if (op_reg) {
            node->src = std::move(op_reg);
        }
    }
}

static void coal_div_instr(Ctx ctx, AsmDiv* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name, src_idx);
        if (op_reg) {
            node->src = std::move(op_reg);
        }
    }
}

static void coal_set_cc_instr(Ctx ctx, AsmSetCC* node) {
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_idx = get_coalesced_idx(ctx, node->dst.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->dst.get())->name, dst_idx);
        if (op_reg) {
            node->dst = std::move(op_reg);
        }
    }
}

static void coal_push_instr(Ctx ctx, AsmPush* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_idx = get_coalesced_idx(ctx, node->src.get());
        std::shared_ptr<AsmOperand> op_reg = coal_op_reg(ctx, static_cast<AsmPseudo*>(node->src.get())->name, src_idx);
        if (op_reg) {
            node->src = std::move(op_reg);
        }
    }
}

static void coal_instr(Ctx ctx, size_t instr_idx, size_t block_id) {
    AsmInstruction* node = GET_INSTR(instr_idx).get();
    switch (node->type()) {
        case AST_AsmMov_t:
            coal_mov_instr(ctx, static_cast<AsmMov*>(node), instr_idx, block_id);
            break;
        case AST_AsmMovSx_t:
            coal_mov_sx_instr(ctx, static_cast<AsmMovSx*>(node));
            break;
        case AST_AsmMovZeroExtend_t:
            coal_zero_extend_instr(ctx, static_cast<AsmMovZeroExtend*>(node));
            break;
        case AST_AsmLea_t:
            coal_lea_instr(ctx, static_cast<AsmLea*>(node));
            break;
        case AST_AsmCvttsd2si_t:
            coal_cvttsd2si_instr(ctx, static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_AsmCvtsi2sd_t:
            coal_cvtsi2sd_instr(ctx, static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_AsmUnary_t:
            coal_unary_instr(ctx, static_cast<AsmUnary*>(node));
            break;
        case AST_AsmBinary_t:
            coal_binary_instr(ctx, static_cast<AsmBinary*>(node));
            break;
        case AST_AsmCmp_t:
            coal_cmp_instr(ctx, static_cast<AsmCmp*>(node));
            break;
        case AST_AsmIdiv_t:
            coal_idiv_instr(ctx, static_cast<AsmIdiv*>(node));
            break;
        case AST_AsmDiv_t:
            coal_div_instr(ctx, static_cast<AsmDiv*>(node));
            break;
        case AST_AsmSetCC_t:
            coal_set_cc_instr(ctx, static_cast<AsmSetCC*>(node));
            break;
        case AST_AsmPush_t:
            coal_push_instr(ctx, static_cast<AsmPush*>(node));
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
        if (ctx->dfa->open_data_map.size() < open_data_map_size) {
            ctx->dfa->open_data_map.resize(open_data_map_size);
        }
    }
    for (size_t i = REGISTER_MASK_SIZE; i < ctx->dfa->set_size; ++i) {
        ctx->dfa->open_data_map[i - REGISTER_MASK_SIZE] = i;
    }

    {
        bool is_fixed_point = true;
        for (size_t instr_idx = 0; instr_idx < ctx->p_instrs->size(); ++instr_idx) {
            if (GET_INSTR(instr_idx) && GET_INSTR(instr_idx)->type() == AST_AsmMov_t
                && coal_infer_regs(ctx, static_cast<AsmMov*>(GET_INSTR(instr_idx).get()))) {
                is_fixed_point = false;
            }
        }
        if (is_fixed_point) {
            return false;
        }
    }

    for (size_t block_id = 0; block_id < ctx->cfg->blocks.size(); ++block_id) {
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
            if (ctx->infer_graph->unpruned_pseudo_names.empty()
                && ctx->sse_infer_graph->unpruned_pseudo_names.empty()) {
                goto Lbreak;
            }
            goto Ldowhile;
        }
        {
            BackendFun* backend_fun = static_cast<BackendFun*>(ctx->backend->symbol_table[node->name].get());
            ctx->p_backend_fun = backend_fun;
        }
        reallocate_registers(ctx);
        ctx->p_backend_fun = nullptr;
    }
Lbreak:
    ctx->p_infer_graph = nullptr;
    ctx->p_instrs = nullptr;
}

static void alloc_toplvl(Ctx ctx, AsmTopLevel* node) {
    switch (node->type()) {
        case AST_AsmFunction_t:
            alloc_fun_toplvl(ctx, static_cast<AsmFunction*>(node));
            break;
        case AST_AsmStaticVariable_t:
            break;
        default:
            THROW_ABORT;
    }
}

static void alloc_program(Ctx ctx, AsmProgram* node) {
    for (const auto& top_level : node->top_levels) {
        alloc_toplvl(ctx, top_level.get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void allocate_registers(AsmProgram* node, FrontEndContext* frontend, uint8_t optim_2_code) {
    RegAllocContext ctx;
    {
        ctx.backend = backend.get();
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

        ctx.cfg = std::make_unique<ControlFlowGraph>();
        ctx.dfa = std::make_unique<DataFlowAnalysis>();
        ctx.dfa_o2 = std::make_unique<DataFlowAnalysisO2>();
        ctx.infer_graph = std::make_unique<InferenceGraph>();
        {
            ctx.infer_graph->k = 12;
            ctx.infer_graph->offset = 0;

            ctx.infer_graph->hard_reg_mask = REGISTER_MASK_FALSE;
            register_mask_set(ctx.infer_graph->hard_reg_mask, REG_Ax, true);
            register_mask_set(ctx.infer_graph->hard_reg_mask, REG_Bx, true);
            register_mask_set(ctx.infer_graph->hard_reg_mask, REG_Cx, true);
            register_mask_set(ctx.infer_graph->hard_reg_mask, REG_Dx, true);
            register_mask_set(ctx.infer_graph->hard_reg_mask, REG_Di, true);
            register_mask_set(ctx.infer_graph->hard_reg_mask, REG_Si, true);
            register_mask_set(ctx.infer_graph->hard_reg_mask, REG_R8, true);
            register_mask_set(ctx.infer_graph->hard_reg_mask, REG_R9, true);
            register_mask_set(ctx.infer_graph->hard_reg_mask, REG_R12, true);
            register_mask_set(ctx.infer_graph->hard_reg_mask, REG_R13, true);
            register_mask_set(ctx.infer_graph->hard_reg_mask, REG_R14, true);
            register_mask_set(ctx.infer_graph->hard_reg_mask, REG_R15, true);
        }
        ctx.sse_infer_graph = std::make_unique<InferenceGraph>();
        {
            ctx.sse_infer_graph->k = 14;
            ctx.sse_infer_graph->offset = 12;

            ctx.sse_infer_graph->hard_reg_mask = REGISTER_MASK_FALSE;
            register_mask_set(ctx.sse_infer_graph->hard_reg_mask, REG_Xmm0, true);
            register_mask_set(ctx.sse_infer_graph->hard_reg_mask, REG_Xmm1, true);
            register_mask_set(ctx.sse_infer_graph->hard_reg_mask, REG_Xmm2, true);
            register_mask_set(ctx.sse_infer_graph->hard_reg_mask, REG_Xmm3, true);
            register_mask_set(ctx.sse_infer_graph->hard_reg_mask, REG_Xmm4, true);
            register_mask_set(ctx.sse_infer_graph->hard_reg_mask, REG_Xmm5, true);
            register_mask_set(ctx.sse_infer_graph->hard_reg_mask, REG_Xmm6, true);
            register_mask_set(ctx.sse_infer_graph->hard_reg_mask, REG_Xmm7, true);
            register_mask_set(ctx.sse_infer_graph->hard_reg_mask, REG_Xmm8, true);
            register_mask_set(ctx.sse_infer_graph->hard_reg_mask, REG_Xmm9, true);
            register_mask_set(ctx.sse_infer_graph->hard_reg_mask, REG_Xmm10, true);
            register_mask_set(ctx.sse_infer_graph->hard_reg_mask, REG_Xmm11, true);
            register_mask_set(ctx.sse_infer_graph->hard_reg_mask, REG_Xmm12, true);
            register_mask_set(ctx.sse_infer_graph->hard_reg_mask, REG_Xmm13, true);
        }
    }
    alloc_program(&ctx, node);
}
