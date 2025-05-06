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

#define __MASK_TYPE__
using mask_t = TULong;

struct ControlFlowGraph;
struct DataFlowAnalysis;
struct DataFlowAnalysisO2;

struct InferenceRegister {
    REGISTER_KIND color;
    REGISTER_KIND register_kind;
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
    std::unordered_map<TIdentifier, InferenceRegister> pseudo_register_map;
};

struct RegAllocContext {
    RegAllocContext(uint8_t optim_2_code);

    // Register allocation
    mask_t callee_saved_reg_mask;
    BackendFun* p_backend_fun_top_level;
    InferenceGraph* p_inference_graph;
    std::array<REGISTER_KIND, 26> register_color_map;
    std::array<InferenceRegister, 26> hard_registers;
    std::unique_ptr<ControlFlowGraph> control_flow_graph;
    std::unique_ptr<DataFlowAnalysis> data_flow_analysis;
    std::unique_ptr<DataFlowAnalysisO2> data_flow_analysis_o2;
    std::unique_ptr<InferenceGraph> inference_graph;
    std::unique_ptr<InferenceGraph> sse_inference_graph;
    std::vector<std::unique_ptr<AsmInstruction>>* p_instructions;
    // Register coalescing
    bool is_with_coalescing;
};

RegAllocContext::RegAllocContext(uint8_t optim_2_code) :
    is_with_coalescing(optim_2_code > 1u) // With register coalescing
{}

static std::unique_ptr<RegAllocContext> context;

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
    switch (node->binary_op->type()) {
        case AST_AsmBitShiftLeft_t:
        case AST_AsmBitShiftRight_t:
        case AST_AsmBitShrArithmetic_t:
            return node->src->type() != AST_AsmImm_t;
        default:
            return false;
    }
}

static void infer_transfer_used_reg(REGISTER_KIND register_kind, size_t next_instruction_index) {
    SET_DFA_INSTR_SET_AT(next_instruction_index, register_mask_bit(register_kind), true);
}

static void infer_transfer_used_name(TIdentifier name, size_t next_instruction_index) {
    if (!is_aliased_name(name)) {
        size_t i = context->control_flow_graph->identifier_id_map[name];
        SET_DFA_INSTR_SET_AT(next_instruction_index, i, true);
    }
}

static void infer_transfer_used_op(AsmOperand* node, size_t next_instruction_index) {
    switch (node->type()) {
        case AST_AsmRegister_t: {
            REGISTER_KIND register_kind = register_mask_kind(static_cast<AsmRegister*>(node)->reg.get());
            if (register_kind != REG_Sp) {
                infer_transfer_used_reg(register_kind, next_instruction_index);
            }
            break;
        }
        case AST_AsmPseudo_t:
            infer_transfer_used_name(static_cast<AsmPseudo*>(node)->name, next_instruction_index);
            break;
        case AST_AsmMemory_t: {
            REGISTER_KIND register_kind = register_mask_kind(static_cast<AsmMemory*>(node)->reg.get());
            if (register_kind != REG_Sp) {
                infer_transfer_used_reg(register_kind, next_instruction_index);
            }
            break;
        }
        case AST_AsmIndexed_t: {
            AsmIndexed* p_node = static_cast<AsmIndexed*>(node);
            {
                REGISTER_KIND register_kind = register_mask_kind(p_node->reg_base.get());
                infer_transfer_used_reg(register_kind, next_instruction_index);
            }
            {
                REGISTER_KIND register_kind = register_mask_kind(p_node->reg_index.get());
                infer_transfer_used_reg(register_kind, next_instruction_index);
            }
            break;
        }
        default:
            break;
    }
}

static void infer_transfer_used_call(AsmCall* node, size_t next_instruction_index) {
    FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[node->name]->type_t.get());
    GET_DFA_INSTR_SET_MASK(next_instruction_index, 0) |= fun_type->param_reg_mask;
}

static void infer_transfer_updated_reg(REGISTER_KIND register_kind, size_t next_instruction_index) {
    SET_DFA_INSTR_SET_AT(next_instruction_index, register_mask_bit(register_kind), false);
}

static void infer_transfer_updated_name(TIdentifier name, size_t next_instruction_index) {
    if (!is_aliased_name(name)) {
        size_t i = context->control_flow_graph->identifier_id_map[name];
        SET_DFA_INSTR_SET_AT(next_instruction_index, i, false);
    }
}

static void infer_transfer_updated_op(AsmOperand* node, size_t next_instruction_index) {
    switch (node->type()) {
        case AST_AsmRegister_t: {
            REGISTER_KIND register_kind = register_mask_kind(static_cast<AsmRegister*>(node)->reg.get());
            if (register_kind != REG_Sp) {
                infer_transfer_updated_reg(register_kind, next_instruction_index);
            }
            break;
        }
        case AST_AsmPseudo_t:
            infer_transfer_updated_name(static_cast<AsmPseudo*>(node)->name, next_instruction_index);
            break;
        case AST_AsmMemory_t: {
            REGISTER_KIND register_kind = register_mask_kind(static_cast<AsmMemory*>(node)->reg.get());
            if (register_kind != REG_Sp) {
                infer_transfer_used_reg(register_kind, next_instruction_index);
            }
            break;
        }
        case AST_AsmIndexed_t:
            RAISE_INTERNAL_ERROR;
        default:
            break;
    }
}

static void infer_transfer_live_regs(size_t instruction_index, size_t next_instruction_index) {
    AsmInstruction* node = GET_INSTR(instruction_index).get();
    switch (node->type()) {
        case AST_AsmMov_t: {
            AsmMov* p_node = static_cast<AsmMov*>(node);
            infer_transfer_updated_op(p_node->dst.get(), next_instruction_index);
            infer_transfer_used_op(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_AsmMovSx_t: {
            AsmMovSx* p_node = static_cast<AsmMovSx*>(node);
            infer_transfer_updated_op(p_node->dst.get(), next_instruction_index);
            infer_transfer_used_op(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_AsmMovZeroExtend_t: {
            AsmMovZeroExtend* p_node = static_cast<AsmMovZeroExtend*>(node);
            infer_transfer_updated_op(p_node->dst.get(), next_instruction_index);
            infer_transfer_used_op(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_AsmLea_t: {
            AsmLea* p_node = static_cast<AsmLea*>(node);
            infer_transfer_updated_op(p_node->dst.get(), next_instruction_index);
            infer_transfer_used_op(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_AsmCvttsd2si_t: {
            AsmCvttsd2si* p_node = static_cast<AsmCvttsd2si*>(node);
            infer_transfer_updated_op(p_node->dst.get(), next_instruction_index);
            infer_transfer_used_op(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_AsmCvtsi2sd_t: {
            AsmCvtsi2sd* p_node = static_cast<AsmCvtsi2sd*>(node);
            infer_transfer_updated_op(p_node->dst.get(), next_instruction_index);
            infer_transfer_used_op(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_AsmUnary_t:
            infer_transfer_used_op(static_cast<AsmUnary*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_AsmBinary_t: {
            AsmBinary* p_node = static_cast<AsmBinary*>(node);
            infer_transfer_used_op(p_node->src.get(), next_instruction_index);
            infer_transfer_used_op(p_node->dst.get(), next_instruction_index);
            if (is_bitshift_cl(p_node)) {
                infer_transfer_used_reg(REG_Cx, next_instruction_index);
            }
            break;
        }
        case AST_AsmCmp_t: {
            AsmCmp* p_node = static_cast<AsmCmp*>(node);
            infer_transfer_used_op(p_node->src.get(), next_instruction_index);
            infer_transfer_used_op(p_node->dst.get(), next_instruction_index);
            break;
        }
        case AST_AsmIdiv_t:
            infer_transfer_used_op(static_cast<AsmIdiv*>(node)->src.get(), next_instruction_index);
            infer_transfer_used_reg(REG_Ax, next_instruction_index);
            infer_transfer_used_reg(REG_Dx, next_instruction_index);
            break;
        case AST_AsmDiv_t:
            infer_transfer_used_op(static_cast<AsmDiv*>(node)->src.get(), next_instruction_index);
            infer_transfer_used_reg(REG_Ax, next_instruction_index);
            break;
        case AST_AsmCdq_t:
            infer_transfer_updated_reg(REG_Dx, next_instruction_index);
            infer_transfer_used_reg(REG_Ax, next_instruction_index);
            break;
        case AST_AsmSetCC_t:
            infer_transfer_updated_op(static_cast<AsmSetCC*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_AsmPush_t:
            infer_transfer_used_op(static_cast<AsmPush*>(node)->src.get(), next_instruction_index);
            break;
        case AST_AsmCall_t:
            infer_transfer_updated_reg(REG_Ax, next_instruction_index);
            infer_transfer_updated_reg(REG_Cx, next_instruction_index);
            infer_transfer_updated_reg(REG_Dx, next_instruction_index);
            infer_transfer_updated_reg(REG_Di, next_instruction_index);
            infer_transfer_updated_reg(REG_Si, next_instruction_index);
            infer_transfer_updated_reg(REG_R8, next_instruction_index);
            infer_transfer_updated_reg(REG_R9, next_instruction_index);
            infer_transfer_updated_reg(REG_Xmm0, next_instruction_index);
            infer_transfer_updated_reg(REG_Xmm1, next_instruction_index);
            infer_transfer_updated_reg(REG_Xmm2, next_instruction_index);
            infer_transfer_updated_reg(REG_Xmm3, next_instruction_index);
            infer_transfer_updated_reg(REG_Xmm4, next_instruction_index);
            infer_transfer_updated_reg(REG_Xmm5, next_instruction_index);
            infer_transfer_updated_reg(REG_Xmm6, next_instruction_index);
            infer_transfer_updated_reg(REG_Xmm7, next_instruction_index);
            infer_transfer_updated_reg(REG_Xmm8, next_instruction_index);
            infer_transfer_updated_reg(REG_Xmm9, next_instruction_index);
            infer_transfer_updated_reg(REG_Xmm10, next_instruction_index);
            infer_transfer_updated_reg(REG_Xmm11, next_instruction_index);
            infer_transfer_updated_reg(REG_Xmm12, next_instruction_index);
            infer_transfer_updated_reg(REG_Xmm13, next_instruction_index);
            infer_transfer_used_call(static_cast<AsmCall*>(node), next_instruction_index);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void set_p_infer_graph(bool is_double) {
    context->p_inference_graph = is_double ? context->sse_inference_graph.get() : context->inference_graph.get();
}

static void infer_add_pseudo_edges(TIdentifier name_1, TIdentifier name_2) {
    {
        InferenceRegister& infer = context->p_inference_graph->pseudo_register_map[name_1];
        if (std::find(infer.linked_pseudo_names.begin(), infer.linked_pseudo_names.end(), name_2)
            == infer.linked_pseudo_names.end()) {
            infer.linked_pseudo_names.push_back(name_2);
            infer.degree++;
        }
    }
    {
        InferenceRegister& infer = context->p_inference_graph->pseudo_register_map[name_2];
        if (std::find(infer.linked_pseudo_names.begin(), infer.linked_pseudo_names.end(), name_1)
            == infer.linked_pseudo_names.end()) {
            infer.linked_pseudo_names.push_back(name_1);
            infer.degree++;
        }
    }
}

static void infer_add_reg_edge(REGISTER_KIND register_kind, TIdentifier name) {
    {
        InferenceRegister& infer = context->p_inference_graph->pseudo_register_map[name];
        if (!register_mask_get(infer.linked_hard_mask, register_kind)) {
            register_mask_set(infer.linked_hard_mask, register_kind, true);
            infer.degree++;
        }
    }
    {
        InferenceRegister& infer = context->hard_registers[register_mask_bit(register_kind)];
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
    RAISE_INTERNAL_ERROR;
}

static void infer_rm_unpruned_pseudo_name(TIdentifier name) {
    for (size_t i = context->p_inference_graph->unpruned_pseudo_names.size(); i-- > 0;) {
        if (context->p_inference_graph->unpruned_pseudo_names[i] == name) {
            std::swap(context->p_inference_graph->unpruned_pseudo_names[i],
                context->p_inference_graph->unpruned_pseudo_names.back());
            context->p_inference_graph->unpruned_pseudo_names.pop_back();
            return;
        }
    }
    RAISE_INTERNAL_ERROR;
}

static void infer_init_used_name_edges(TIdentifier name) {
    if (!is_aliased_name(name)) {
        set_p_infer_graph(frontend->symbol_table[name]->type_t->type() == AST_Double_t);
        context->p_inference_graph->pseudo_register_map[name].spill_cost++;
    }
}

static void infer_init_used_op_edges(AsmOperand* node) {
    if (node->type() == AST_AsmPseudo_t) {
        infer_init_used_name_edges(static_cast<AsmPseudo*>(node)->name);
    }
}

static void infer_init_updated_regs_edges(
    REGISTER_KIND* register_kinds, size_t instruction_index, size_t register_kinds_size, bool is_double) {

    size_t mov_mask_bit = context->data_flow_analysis->set_size;
    bool is_mov = GET_INSTR(instruction_index)->type() == AST_AsmMov_t;
    if (is_mov) {
        AsmMov* mov = static_cast<AsmMov*>(GET_INSTR(instruction_index).get());
        if (mov->src->type() == AST_AsmPseudo_t) {
            TIdentifier src_name = static_cast<AsmPseudo*>(mov->src.get())->name;
            if (is_aliased_name(src_name)) {
                is_mov = false;
            }
            else {
                bool is_src_double = frontend->symbol_table[src_name]->type_t->type() == AST_Double_t;
                set_p_infer_graph(is_src_double);
                context->p_inference_graph->pseudo_register_map[src_name].spill_cost++;
                mov_mask_bit = context->control_flow_graph->identifier_id_map[src_name];
                is_mov = is_double == is_src_double;
            }
        }
        else {
            is_mov = false;
        }
    }
    set_p_infer_graph(is_double);

    if (GET_DFA_INSTR_SET_MASK(instruction_index, 0) != MASK_FALSE) {
        for (size_t i = context->data_flow_analysis->set_size < 64 ? context->data_flow_analysis->set_size : 64;
             i-- > REGISTER_MASK_SIZE;) {
            if (GET_DFA_INSTR_SET_AT(instruction_index, i) && !(is_mov && i == mov_mask_bit)) {
                TIdentifier pseudo_name = context->data_flow_analysis_o2->data_name_map[i - REGISTER_MASK_SIZE];
                if (is_double == (frontend->symbol_table[pseudo_name]->type_t->type() == AST_Double_t)) {
                    for (size_t j = 0; j < register_kinds_size; ++j) {
                        infer_add_reg_edge(register_kinds[j], pseudo_name);
                    }
                }
            }
        }
    }
    size_t i = 64;
    for (size_t j = 1; j < context->data_flow_analysis->mask_size; ++j) {
        if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > context->data_flow_analysis->set_size) {
            mask_set_size = context->data_flow_analysis->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(instruction_index, i) && !(is_mov && i == mov_mask_bit)) {
                TIdentifier pseudo_name = context->data_flow_analysis_o2->data_name_map[i - REGISTER_MASK_SIZE];
                if (is_double == (frontend->symbol_table[pseudo_name]->type_t->type() == AST_Double_t)) {
                    for (size_t k = 0; k < register_kinds_size; ++k) {
                        infer_add_reg_edge(register_kinds[k], pseudo_name);
                    }
                }
            }
        }
    }
}

static void infer_init_updated_name_edges(TIdentifier name, size_t instruction_index) {
    if (is_aliased_name(name)) {
        return;
    }
    bool is_double = frontend->symbol_table[name]->type_t->type() == AST_Double_t;

    size_t mov_mask_bit = context->data_flow_analysis->set_size;
    bool is_mov = GET_INSTR(instruction_index)->type() == AST_AsmMov_t;
    if (is_mov) {
        AsmMov* mov = static_cast<AsmMov*>(GET_INSTR(instruction_index).get());
        switch (mov->src->type()) {
            case AST_AsmRegister_t: {
                REGISTER_KIND src_register_kind =
                    register_mask_kind(static_cast<AsmRegister*>(mov->src.get())->reg.get());
                if (src_register_kind == REG_Sp) {
                    is_mov = false;
                }
                else {
                    mov_mask_bit = register_mask_bit(src_register_kind);
                    is_mov = is_double == (mov_mask_bit > 11);
                }
                break;
            }
            case AST_AsmPseudo_t: {
                TIdentifier src_name = static_cast<AsmPseudo*>(mov->src.get())->name;
                if (is_aliased_name(src_name)) {
                    is_mov = false;
                }
                else {
                    bool is_src_double = frontend->symbol_table[src_name]->type_t->type() == AST_Double_t;
                    set_p_infer_graph(is_src_double);
                    context->p_inference_graph->pseudo_register_map[src_name].spill_cost++;
                    mov_mask_bit = context->control_flow_graph->identifier_id_map[src_name];
                    is_mov = is_double == is_src_double;
                }
                break;
            }
            case AST_AsmMemory_t: {
                REGISTER_KIND src_register_kind =
                    register_mask_kind(static_cast<AsmMemory*>(mov->src.get())->reg.get());
                if (src_register_kind == REG_Sp) {
                    is_mov = false;
                }
                else {
                    mov_mask_bit = register_mask_bit(src_register_kind);
                    is_mov = is_double == (mov_mask_bit > 11);
                }
                break;
            }
            case AST_AsmIndexed_t:
                RAISE_INTERNAL_ERROR;
            default: {
                is_mov = false;
                break;
            }
        }
    }
    set_p_infer_graph(is_double);
    context->p_inference_graph->pseudo_register_map[name].spill_cost++;

    if (GET_DFA_INSTR_SET_MASK(instruction_index, 0) != MASK_FALSE) {
        size_t i = context->p_inference_graph->offset;
        size_t mask_set_size = i + context->p_inference_graph->k;
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(instruction_index, i) && !(is_mov && i == mov_mask_bit)) {
                REGISTER_KIND register_kind = context->hard_registers[i].register_kind;
                infer_add_reg_edge(register_kind, name);
            }
        }
        i = REGISTER_MASK_SIZE;
        mask_set_size = context->data_flow_analysis->set_size < 64 ? context->data_flow_analysis->set_size : 64;
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(instruction_index, i) && !(is_mov && i == mov_mask_bit)) {
                TIdentifier pseudo_name = context->data_flow_analysis_o2->data_name_map[i - REGISTER_MASK_SIZE];
                if (name != pseudo_name
                    && is_double == (frontend->symbol_table[pseudo_name]->type_t->type() == AST_Double_t)) {
                    infer_add_pseudo_edges(name, pseudo_name);
                }
            }
        }
    }
    size_t i = 64;
    for (size_t j = 1; j < context->data_flow_analysis->mask_size; ++j) {
        if (GET_DFA_INSTR_SET_MASK(instruction_index, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > context->data_flow_analysis->set_size) {
            mask_set_size = context->data_flow_analysis->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTR_SET_AT(instruction_index, i) && !(is_mov && i == mov_mask_bit)) {
                TIdentifier pseudo_name = context->data_flow_analysis_o2->data_name_map[i - REGISTER_MASK_SIZE];
                if (name != pseudo_name
                    && is_double == (frontend->symbol_table[pseudo_name]->type_t->type() == AST_Double_t)) {
                    infer_add_pseudo_edges(name, pseudo_name);
                }
            }
        }
    }
}

static void infer_init_updated_op_edges(AsmOperand* node, size_t instruction_index) {
    switch (node->type()) {
        case AST_AsmRegister_t: {
            REGISTER_KIND register_kinds[1] = {register_mask_kind(static_cast<AsmRegister*>(node)->reg.get())};
            if (register_kinds[0] != REG_Sp) {
                bool is_double = register_mask_bit(register_kinds[0]) > 11;
                infer_init_updated_regs_edges(register_kinds, instruction_index, 1, is_double);
            }
            break;
        }
        case AST_AsmPseudo_t:
            infer_init_updated_name_edges(static_cast<AsmPseudo*>(node)->name, instruction_index);
            break;
        default:
            break;
    }
}

static void infer_init_edges(size_t instruction_index) {
    AsmInstruction* node = GET_INSTR(instruction_index).get();
    switch (node->type()) {
        case AST_AsmMov_t:
            infer_init_updated_op_edges(static_cast<AsmMov*>(node)->dst.get(), instruction_index);
            break;
        case AST_AsmMovSx_t:
            infer_init_updated_op_edges(static_cast<AsmMovSx*>(node)->dst.get(), instruction_index);
            break;
        case AST_AsmMovZeroExtend_t:
            infer_init_updated_op_edges(static_cast<AsmMovZeroExtend*>(node)->dst.get(), instruction_index);
            break;
        case AST_AsmLea_t:
            infer_init_updated_op_edges(static_cast<AsmLea*>(node)->dst.get(), instruction_index);
            break;
        case AST_AsmCvttsd2si_t:
            infer_init_updated_op_edges(static_cast<AsmCvttsd2si*>(node)->dst.get(), instruction_index);
            break;
        case AST_AsmCvtsi2sd_t:
            infer_init_updated_op_edges(static_cast<AsmCvtsi2sd*>(node)->dst.get(), instruction_index);
            break;
        case AST_AsmUnary_t:
            infer_init_updated_op_edges(static_cast<AsmUnary*>(node)->dst.get(), instruction_index);
            break;
        case AST_AsmBinary_t: {
            AsmBinary* p_node = static_cast<AsmBinary*>(node);
            if (is_bitshift_cl(p_node)) {
                REGISTER_KIND register_kinds[1] = {REG_Cx};
                infer_init_updated_regs_edges(register_kinds, instruction_index, 1, false);
            }
            infer_init_updated_op_edges(p_node->dst.get(), instruction_index);
            infer_init_used_op_edges(p_node->src.get());
            break;
        }
        case AST_AsmCmp_t: {
            AsmCmp* p_node = static_cast<AsmCmp*>(node);
            infer_init_used_op_edges(p_node->src.get());
            infer_init_used_op_edges(p_node->dst.get());
            break;
        }
        case AST_AsmIdiv_t: {
            REGISTER_KIND register_kinds[2] = {REG_Ax, REG_Dx};
            infer_init_updated_regs_edges(register_kinds, instruction_index, 2, false);
            infer_init_used_op_edges(static_cast<AsmIdiv*>(node)->src.get());
            break;
        }
        case AST_AsmDiv_t: {
            REGISTER_KIND register_kinds[1] = {REG_Ax};
            infer_init_updated_regs_edges(register_kinds, instruction_index, 1, false);
            infer_init_used_op_edges(static_cast<AsmDiv*>(node)->src.get());
            break;
        }
        case AST_AsmCdq_t: {
            REGISTER_KIND register_kinds[1] = {REG_Dx};
            infer_init_updated_regs_edges(register_kinds, instruction_index, 1, false);
            break;
        }
        case AST_AsmSetCC_t:
            infer_init_updated_op_edges(static_cast<AsmSetCC*>(node)->dst.get(), instruction_index);
            break;
        case AST_AsmPush_t:
            infer_init_used_op_edges(static_cast<AsmPush*>(node)->src.get());
            break;
        case AST_AsmCall_t: {
            {
                REGISTER_KIND register_kinds[7] = {REG_Ax, REG_Cx, REG_Dx, REG_Di, REG_Si, REG_R8, REG_R9};
                infer_init_updated_regs_edges(register_kinds, instruction_index, 7, false);
            }
            {
                REGISTER_KIND register_kinds[14] = {REG_Xmm0, REG_Xmm1, REG_Xmm2, REG_Xmm3, REG_Xmm4, REG_Xmm5,
                    REG_Xmm6, REG_Xmm7, REG_Xmm8, REG_Xmm9, REG_Xmm10, REG_Xmm11, REG_Xmm12, REG_Xmm13};
                infer_init_updated_regs_edges(register_kinds, instruction_index, 14, true);
            }
            break;
        }
        default:
            break;
    }
}

static bool init_inference_graph(TIdentifier function_name) {
    if (!init_data_flow_analysis(function_name)) {
        return false;
    }
    dfa_iter_alg();
    if (context->control_flow_graph->identifier_id_map.empty()) {
        return false;
    }

    context->callee_saved_reg_mask = REGISTER_MASK_FALSE;
    context->inference_graph->unpruned_pseudo_names.clear();
    context->inference_graph->pseudo_register_map.clear();
    context->sse_inference_graph->unpruned_pseudo_names.clear();
    context->sse_inference_graph->pseudo_register_map.clear();
    for (const auto& name_id : context->control_flow_graph->identifier_id_map) {
        TIdentifier name = name_id.first;
        InferenceRegister infer = {REG_Sp, REG_Sp, 0, 0, REGISTER_MASK_FALSE, {}};
        if (frontend->symbol_table[name_id.first]->type_t->type() == AST_Double_t) {
            context->sse_inference_graph->unpruned_pseudo_names.push_back(name);
            context->sse_inference_graph->pseudo_register_map[name] = std::move(infer);
        }
        else {
            context->inference_graph->unpruned_pseudo_names.push_back(name);
            context->inference_graph->pseudo_register_map[name] = std::move(infer);
        }
    }

    if (!context->inference_graph->pseudo_register_map.empty()) {
        if (context->inference_graph->unpruned_hard_mask_bits.size() < 12) {
            context->inference_graph->unpruned_hard_mask_bits.resize(12);
        }

        mask_t hard_reg_mask = context->inference_graph->hard_reg_mask;
        for (size_t i = 0; i < 12; ++i) {
            context->register_color_map[i] = REG_Sp;
            context->hard_registers[i].color = REG_Sp;
            context->hard_registers[i].degree = 11;
            context->hard_registers[i].spill_cost = 0;
            context->hard_registers[i].linked_hard_mask = hard_reg_mask;
            context->hard_registers[i].linked_pseudo_names.clear();
            context->inference_graph->unpruned_hard_mask_bits[i] = i;
        }
    }
    if (!context->sse_inference_graph->pseudo_register_map.empty()) {
        if (context->sse_inference_graph->unpruned_hard_mask_bits.size() < 14) {
            context->sse_inference_graph->unpruned_hard_mask_bits.resize(14);
        }

        mask_t hard_reg_mask = context->sse_inference_graph->hard_reg_mask;
        for (size_t i = 12; i < 26; ++i) {
            context->register_color_map[i] = REG_Sp;
            context->hard_registers[i].color = REG_Sp;
            context->hard_registers[i].degree = 13;
            context->hard_registers[i].spill_cost = 0;
            context->hard_registers[i].linked_hard_mask = hard_reg_mask;
            context->hard_registers[i].linked_pseudo_names.clear();
            context->sse_inference_graph->unpruned_hard_mask_bits[i - 12] = i;
        }
    }

    for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
        if (GET_CFG_BLOCK(block_id).size > 0) {
            for (size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
                 instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
                if (GET_INSTR(instruction_index)) {
                    infer_init_edges(instruction_index);
                }
            }
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register allocation

static bool is_reg_callee_saved(REGISTER_KIND register_kind) {
    switch (register_kind) {
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

static void alloc_prune_infer_reg(InferenceRegister* infer, size_t pruned_index) {
    if (infer->register_kind == REG_Sp) {
        std::swap(context->p_inference_graph->unpruned_pseudo_names[pruned_index],
            context->p_inference_graph->unpruned_pseudo_names.back());
        context->p_inference_graph->unpruned_pseudo_names.pop_back();
    }
    else {
        std::swap(context->p_inference_graph->unpruned_hard_mask_bits[pruned_index],
            context->p_inference_graph->unpruned_hard_mask_bits.back());
        context->p_inference_graph->unpruned_hard_mask_bits.pop_back();
    }
    if (infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < context->p_inference_graph->k; ++i) {
            InferenceRegister& linked_infer = context->hard_registers[i + context->p_inference_graph->offset];
            if (register_mask_get(infer->linked_hard_mask, linked_infer.register_kind)) {
                linked_infer.degree--;
            }
        }
    }
    for (TIdentifier name : infer->linked_pseudo_names) {
        context->p_inference_graph->pseudo_register_map[name].degree--;
    }
}

static void alloc_unprune_infer_reg(InferenceRegister* infer, TIdentifier pruned_name) {
    if (infer->register_kind == REG_Sp) {
        if (std::find(context->p_inference_graph->unpruned_pseudo_names.begin(),
                context->p_inference_graph->unpruned_pseudo_names.end(), pruned_name)
            != context->p_inference_graph->unpruned_pseudo_names.end()) {
            RAISE_INTERNAL_ERROR;
        }
        context->p_inference_graph->unpruned_pseudo_names.push_back(pruned_name);
    }
    else {
        size_t pruned_mask_bit = register_mask_bit(infer->register_kind);
        if (std::find(context->p_inference_graph->unpruned_hard_mask_bits.begin(),
                context->p_inference_graph->unpruned_hard_mask_bits.end(), pruned_mask_bit)
            != context->p_inference_graph->unpruned_hard_mask_bits.end()) {
            RAISE_INTERNAL_ERROR;
        }
        context->p_inference_graph->unpruned_hard_mask_bits.push_back(pruned_mask_bit);
    }
    if (infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < context->p_inference_graph->k; ++i) {
            InferenceRegister& linked_infer = context->hard_registers[i + context->p_inference_graph->offset];
            if (register_mask_get(infer->linked_hard_mask, linked_infer.register_kind)) {
                linked_infer.degree++;
            }
        }
    }
    for (TIdentifier name : infer->linked_pseudo_names) {
        context->p_inference_graph->pseudo_register_map[name].degree++;
    }
}

static void alloc_color_infer_graph();

static void alloc_next_color_infer_graph() {
    if (!context->p_inference_graph->unpruned_hard_mask_bits.empty()
        || !context->p_inference_graph->unpruned_pseudo_names.empty()) {
        alloc_color_infer_graph();
    }
}

static InferenceRegister* alloc_prune_infer_graph(TIdentifier& pruned_name) {
    size_t pruned_index;
    InferenceRegister* infer = nullptr;
    for (size_t i = 0; i < context->p_inference_graph->unpruned_pseudo_names.size(); ++i) {
        pruned_name = context->p_inference_graph->unpruned_pseudo_names[i];
        infer = &context->p_inference_graph->pseudo_register_map[pruned_name];
        if (infer->degree < context->p_inference_graph->k) {
            pruned_index = i;
            break;
        }
        infer = nullptr;
    }
    if (!infer) {
        for (size_t i = 0; i < context->p_inference_graph->unpruned_hard_mask_bits.size(); ++i) {
            size_t pruned_mask_bit = context->p_inference_graph->unpruned_hard_mask_bits[i];
            infer = &context->hard_registers[pruned_mask_bit];
            if (infer->degree < context->p_inference_graph->k) {
                pruned_index = i;
                break;
            }
            infer = nullptr;
        }
    }
    if (!infer) {
        size_t i = 0;
        for (; i < context->p_inference_graph->unpruned_pseudo_names.size(); ++i) {
            pruned_name = context->p_inference_graph->unpruned_pseudo_names[i];
            infer = &context->p_inference_graph->pseudo_register_map[pruned_name];
            if (infer->degree > 0) {
                pruned_index = i;
                break;
            }
            infer = nullptr;
        }
        if (!infer) {
            RAISE_INTERNAL_ERROR;
        }
        double min_spill_metric = static_cast<double>(infer->spill_cost) / infer->degree;
        for (; i < context->p_inference_graph->unpruned_pseudo_names.size(); ++i) {
            TIdentifier spill_name = context->p_inference_graph->unpruned_pseudo_names[i];
            InferenceRegister& spill_infer = context->p_inference_graph->pseudo_register_map[spill_name];
            if (spill_infer.degree > 0) {
                double spill_metric = static_cast<double>(spill_infer.spill_cost) / spill_infer.degree;
                if (spill_metric < min_spill_metric) {
                    pruned_index = i;
                    pruned_name = spill_name;
                    infer = &spill_infer;
                    min_spill_metric = spill_metric;
                }
            }
        }
    }
    alloc_prune_infer_reg(infer, pruned_index);
    return infer;
}

static void alloc_unprune_infer_graph(InferenceRegister* infer, TIdentifier pruned_name) {
    mask_t color_reg_mask = context->p_inference_graph->hard_reg_mask;
    if (infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < context->p_inference_graph->k; ++i) {
            InferenceRegister& linked_infer = context->hard_registers[i + context->p_inference_graph->offset];
            if (register_mask_get(infer->linked_hard_mask, linked_infer.register_kind)) {
                if (linked_infer.color != REG_Sp) {
                    register_mask_set(color_reg_mask, linked_infer.color, false);
                }
            }
        }
    }
    for (TIdentifier name : infer->linked_pseudo_names) {
        InferenceRegister& linked_infer = context->p_inference_graph->pseudo_register_map[name];
        if (linked_infer.color != REG_Sp) {
            register_mask_set(color_reg_mask, linked_infer.color, false);
        }
    }
    if (color_reg_mask != REGISTER_MASK_FALSE) {
        if (is_reg_callee_saved(infer->register_kind)) {
            for (size_t i = context->p_inference_graph->k; i-- > 0;) {
                REGISTER_KIND color = context->hard_registers[i + context->p_inference_graph->offset].register_kind;
                if (register_mask_get(color_reg_mask, color)) {
                    infer->color = color;
                    break;
                }
            }
        }
        else {
            for (size_t i = 0; i < context->p_inference_graph->k; ++i) {
                REGISTER_KIND color = context->hard_registers[i + context->p_inference_graph->offset].register_kind;
                if (register_mask_get(color_reg_mask, color)) {
                    infer->color = color;
                    break;
                }
            }
        }
        alloc_unprune_infer_reg(infer, pruned_name);
    }
}

static void alloc_color_infer_graph() {
    TIdentifier pruned_name;
    InferenceRegister* infer = alloc_prune_infer_graph(pruned_name);
    alloc_next_color_infer_graph();
    alloc_unprune_infer_graph(infer, pruned_name);
}

static void alloc_color_reg_map() {
    for (size_t i = 0; i < context->p_inference_graph->k; ++i) {
        InferenceRegister& infer = context->hard_registers[i + context->p_inference_graph->offset];
        if (infer.color != REG_Sp) {
            context->register_color_map[register_mask_bit(infer.color)] = infer.register_kind;
        }
    }
}

static std::shared_ptr<AsmRegister> alloc_hard_reg(TIdentifier name) {
    if (is_aliased_name(name)) {
        return nullptr;
    }
    set_p_infer_graph(frontend->symbol_table[name]->type_t->type() == AST_Double_t);
    REGISTER_KIND color = context->p_inference_graph->pseudo_register_map[name].color;
    if (color != REG_Sp) {
        REGISTER_KIND register_kind = context->register_color_map[register_mask_bit(color)];
        std::shared_ptr<AsmRegister> hard_register = gen_register(register_kind);
        if (is_reg_callee_saved(register_kind) && !register_mask_get(context->callee_saved_reg_mask, register_kind)) {
            register_mask_set(context->callee_saved_reg_mask, register_kind, true);
            std::shared_ptr<AsmOperand> callee_saved_register = hard_register;
            context->p_backend_fun_top_level->callee_saved_registers.push_back(std::move(callee_saved_register));
        }
        return hard_register;
    }
    else {
        return nullptr;
    }
}

static REGISTER_KIND get_op_reg_kind(AsmOperand* node) {
    switch (node->type()) {
        case AST_AsmRegister_t:
            return register_mask_kind(static_cast<AsmRegister*>(node)->reg.get());
        case AST_AsmPseudo_t: {
            TIdentifier name = static_cast<AsmPseudo*>(node)->name;
            if (is_aliased_name(name)) {
                return REG_Sp;
            }
            set_p_infer_graph(frontend->symbol_table[name]->type_t->type() == AST_Double_t);
            REGISTER_KIND color = context->p_inference_graph->pseudo_register_map[name].color;
            if (color == REG_Sp) {
                return REG_Sp;
            }
            else {
                return context->register_color_map[register_mask_bit(color)];
            }
        }
        case AST_AsmMemory_t:
            return REG_Sp;
        case AST_AsmIndexed_t:
            RAISE_INTERNAL_ERROR;
        default:
            return REG_Sp;
    }
}

static void alloc_mov_instr(AsmMov* node, size_t instruction_index) {
    AsmOperand* src_operand = static_cast<AsmPseudo*>(node->src.get());
    AsmOperand* dst_operand = static_cast<AsmPseudo*>(node->dst.get());
    REGISTER_KIND src_register_kind = get_op_reg_kind(src_operand);
    REGISTER_KIND dst_register_kind = get_op_reg_kind(dst_operand);
    if (src_register_kind != REG_Sp && src_register_kind == dst_register_kind) {
        set_instr(nullptr, instruction_index);
    }
    else {
        if (src_operand->type() == AST_AsmPseudo_t) {
            std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(src_operand)->name);
            if (hard_register) {
                node->src = std::move(hard_register);
            }
        }
        if (dst_operand->type() == AST_AsmPseudo_t) {
            std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(dst_operand)->name);
            if (hard_register) {
                node->dst = std::move(hard_register);
            }
        }
    }
}

static void alloc_mov_sx_instr(AsmMovSx* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = std::move(hard_register);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = std::move(hard_register);
        }
    }
}

static void alloc_zero_extend_instr(AsmMovZeroExtend* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = std::move(hard_register);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = std::move(hard_register);
        }
    }
}

static void alloc_lea_instr(AsmLea* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = std::move(hard_register);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = std::move(hard_register);
        }
    }
}

static void alloc_cvttsd2si_instr(AsmCvttsd2si* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = std::move(hard_register);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = std::move(hard_register);
        }
    }
}

static void alloc_cvtsi2sd_instr(AsmCvtsi2sd* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = std::move(hard_register);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = std::move(hard_register);
        }
    }
}

static void alloc_unary_instr(AsmUnary* node) {
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = std::move(hard_register);
        }
    }
}

static void alloc_binary_instr(AsmBinary* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = std::move(hard_register);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = std::move(hard_register);
        }
    }
}

static void alloc_cmp_instr(AsmCmp* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = std::move(hard_register);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = std::move(hard_register);
        }
    }
}

static void alloc_idiv_instr(AsmIdiv* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = std::move(hard_register);
        }
    }
}

static void alloc_div_instr(AsmDiv* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = std::move(hard_register);
        }
    }
}

static void alloc_set_cc_instr(AsmSetCC* node) {
    if (node->dst->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = std::move(hard_register);
        }
    }
}

static void alloc_push_instr(AsmPush* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register = alloc_hard_reg(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = std::move(hard_register);
        }
    }
}

static void alloc_instr(size_t instruction_index) {
    AsmInstruction* node = GET_INSTR(instruction_index).get();
    switch (node->type()) {
        case AST_AsmMov_t:
            alloc_mov_instr(static_cast<AsmMov*>(node), instruction_index);
            break;
        case AST_AsmMovSx_t:
            alloc_mov_sx_instr(static_cast<AsmMovSx*>(node));
            break;
        case AST_AsmMovZeroExtend_t:
            alloc_zero_extend_instr(static_cast<AsmMovZeroExtend*>(node));
            break;
        case AST_AsmLea_t:
            alloc_lea_instr(static_cast<AsmLea*>(node));
            break;
        case AST_AsmCvttsd2si_t:
            alloc_cvttsd2si_instr(static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_AsmCvtsi2sd_t:
            alloc_cvtsi2sd_instr(static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_AsmUnary_t:
            alloc_unary_instr(static_cast<AsmUnary*>(node));
            break;
        case AST_AsmBinary_t:
            alloc_binary_instr(static_cast<AsmBinary*>(node));
            break;
        case AST_AsmCmp_t:
            alloc_cmp_instr(static_cast<AsmCmp*>(node));
            break;
        case AST_AsmIdiv_t:
            alloc_idiv_instr(static_cast<AsmIdiv*>(node));
            break;
        case AST_AsmDiv_t:
            alloc_div_instr(static_cast<AsmDiv*>(node));
            break;
        case AST_AsmSetCC_t:
            alloc_set_cc_instr(static_cast<AsmSetCC*>(node));
            break;
        case AST_AsmPush_t:
            alloc_push_instr(static_cast<AsmPush*>(node));
            break;
        case AST_AsmCdq_t:
        case AST_AsmCall_t:
            break;
        default:
            break;
    }
}

static void reallocate_registers() {
    if (!context->inference_graph->unpruned_pseudo_names.empty()) {
        set_p_infer_graph(false);
        alloc_color_infer_graph();
        alloc_color_reg_map();
    }
    if (!context->sse_inference_graph->unpruned_pseudo_names.empty()) {
        set_p_infer_graph(true);
        alloc_color_infer_graph();
        alloc_color_reg_map();
    }
    for (size_t instruction_index = 0; instruction_index < context->p_instructions->size(); ++instruction_index) {
        if (GET_INSTR(instruction_index)) {
            alloc_instr(instruction_index);
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
            RAISE_INTERNAL_ERROR;
    }
}

static size_t get_coalesced_idx(AsmOperand* node) {
    size_t coalesced_index = context->data_flow_analysis->set_size;
    switch (node->type()) {
        case AST_AsmRegister_t: {
            REGISTER_KIND register_kind = register_mask_kind(static_cast<AsmRegister*>(node)->reg.get());
            if (register_kind != REG_Sp) {
                coalesced_index = register_mask_bit(register_kind);
            }
            break;
        }
        case AST_AsmPseudo_t: {
            TIdentifier name = static_cast<AsmPseudo*>(node)->name;
            if (!is_aliased_name(name)) {
                coalesced_index = context->control_flow_graph->identifier_id_map[name];
            }
            break;
        }
        default:
            break;
    }

    if (coalesced_index < context->data_flow_analysis->set_size) {
        while (coalesced_index >= REGISTER_MASK_SIZE
               && coalesced_index != context->data_flow_analysis->open_data_map[coalesced_index - REGISTER_MASK_SIZE]) {
            coalesced_index = context->data_flow_analysis->open_data_map[coalesced_index - REGISTER_MASK_SIZE];
        }
    }
    return coalesced_index;
}

static bool get_coalescable_infer_regs(
    InferenceRegister*& src_infer, InferenceRegister*& dst_infer, size_t src_index, size_t dst_index) {
    if (src_index != dst_index && (src_index >= REGISTER_MASK_SIZE || dst_index >= REGISTER_MASK_SIZE)
        && src_index < context->data_flow_analysis->set_size && dst_index < context->data_flow_analysis->set_size) {
        if (src_index < REGISTER_MASK_SIZE) {
            TIdentifier dst_name = context->data_flow_analysis_o2->data_name_map[dst_index - REGISTER_MASK_SIZE];
            bool is_double = frontend->symbol_table[dst_name]->type_t->type() == AST_Double_t;
            if (is_double == (src_index > 11)) {
                set_p_infer_graph(is_double);
                src_infer = &context->hard_registers[src_index];
                dst_infer = &context->p_inference_graph->pseudo_register_map[dst_name];
                return !register_mask_get(dst_infer->linked_hard_mask, src_infer->register_kind);
            }
        }
        else if (dst_index < REGISTER_MASK_SIZE) {
            TIdentifier src_name = context->data_flow_analysis_o2->data_name_map[src_index - REGISTER_MASK_SIZE];
            bool is_double = frontend->symbol_table[src_name]->type_t->type() == AST_Double_t;
            if (is_double == (dst_index > 11)) {
                set_p_infer_graph(is_double);
                src_infer = &context->p_inference_graph->pseudo_register_map[src_name];
                dst_infer = &context->hard_registers[dst_index];
                return !register_mask_get(src_infer->linked_hard_mask, dst_infer->register_kind);
            }
        }
        else {
            TIdentifier src_name = context->data_flow_analysis_o2->data_name_map[src_index - REGISTER_MASK_SIZE];
            TIdentifier dst_name = context->data_flow_analysis_o2->data_name_map[dst_index - REGISTER_MASK_SIZE];
            bool is_double = frontend->symbol_table[src_name]->type_t->type() == AST_Double_t;
            if (is_double == (frontend->symbol_table[dst_name]->type_t->type() == AST_Double_t)
                && get_type_size(frontend->symbol_table[src_name]->type_t.get())
                       == get_type_size(frontend->symbol_table[dst_name]->type_t.get())) {
                set_p_infer_graph(is_double);
                src_infer = &context->p_inference_graph->pseudo_register_map[src_name];
                dst_infer = &context->p_inference_graph->pseudo_register_map[dst_name];
                return std::find(dst_infer->linked_pseudo_names.begin(), dst_infer->linked_pseudo_names.end(), src_name)
                       == dst_infer->linked_pseudo_names.end();
            }
        }
    }
    return false;
}

static bool coal_briggs_test(InferenceRegister* src_infer, InferenceRegister* dst_infer) {
    size_t degree = 0;

    if (src_infer->linked_hard_mask != REGISTER_MASK_FALSE || dst_infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < context->p_inference_graph->k; ++i) {
            InferenceRegister& linked_infer = context->hard_registers[i + context->p_inference_graph->offset];
            if (register_mask_get(src_infer->linked_hard_mask, linked_infer.register_kind)) {
                if (register_mask_get(dst_infer->linked_hard_mask, linked_infer.register_kind)) {
                    if (linked_infer.degree > context->p_inference_graph->k) {
                        degree++;
                    }
                }
                else if (linked_infer.degree >= context->p_inference_graph->k) {
                    degree++;
                }
            }
            else if (register_mask_get(dst_infer->linked_hard_mask, linked_infer.register_kind)
                     && linked_infer.degree >= context->p_inference_graph->k) {
                degree++;
            }
        }
    }

    for (size_t i = 0; i < context->data_flow_analysis->mask_size; ++i) {
        GET_DFA_INSTR_SET_MASK(context->data_flow_analysis->incoming_index, i) = MASK_FALSE;
    }
    for (TIdentifier name : dst_infer->linked_pseudo_names) {
        size_t i = context->control_flow_graph->identifier_id_map[name];
        SET_DFA_INSTR_SET_AT(context->data_flow_analysis->incoming_index, i, true);
    }
    for (TIdentifier name : src_infer->linked_pseudo_names) {
        size_t i = context->control_flow_graph->identifier_id_map[name];
        InferenceRegister& linked_infer = context->p_inference_graph->pseudo_register_map[name];
        if (GET_DFA_INSTR_SET_AT(context->data_flow_analysis->incoming_index, i)) {
            if (linked_infer.degree > context->p_inference_graph->k) {
                degree++;
            }
            SET_DFA_INSTR_SET_AT(context->data_flow_analysis->incoming_index, i, false);
        }
        else if (linked_infer.degree >= context->p_inference_graph->k) {
            degree++;
        }
    }
    for (TIdentifier name : dst_infer->linked_pseudo_names) {
        size_t i = context->control_flow_graph->identifier_id_map[name];
        if (GET_DFA_INSTR_SET_AT(context->data_flow_analysis->incoming_index, i)) {
            InferenceRegister& linked_infer = context->p_inference_graph->pseudo_register_map[name];
            if (linked_infer.degree >= context->p_inference_graph->k) {
                degree++;
            }
        }
    }

    return degree < context->p_inference_graph->k;
}

static bool coal_george_test(REGISTER_KIND register_kind, InferenceRegister* infer) {
    for (TIdentifier name : infer->linked_pseudo_names) {
        InferenceRegister& linked_infer = context->p_inference_graph->pseudo_register_map[name];
        if (!register_mask_get(linked_infer.linked_hard_mask, register_kind)
            && linked_infer.degree >= context->p_inference_graph->k) {
            return false;
        }
    }
    return true;
}

static bool coal_conservative_tests(InferenceRegister* src_infer, InferenceRegister* dst_infer) {
    if (coal_briggs_test(src_infer, dst_infer)) {
        return true;
    }
    else if (src_infer->register_kind != REG_Sp) {
        return coal_george_test(src_infer->register_kind, dst_infer);
    }
    else if (dst_infer->register_kind != REG_Sp) {
        return coal_george_test(dst_infer->register_kind, src_infer);
    }
    else {
        return false;
    }
}

static void coal_pseudo_infer_reg(InferenceRegister* infer, size_t merge_index, size_t keep_index) {
    TIdentifier merge_name = context->data_flow_analysis_o2->data_name_map[merge_index - REGISTER_MASK_SIZE];
    TIdentifier keep_name = context->data_flow_analysis_o2->data_name_map[keep_index - REGISTER_MASK_SIZE];
    if (infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < context->p_inference_graph->k; ++i) {
            InferenceRegister& linked_infer = context->hard_registers[i + context->p_inference_graph->offset];
            if (register_mask_get(infer->linked_hard_mask, linked_infer.register_kind)) {
                infer_rm_pseudo_edge(linked_infer, merge_name);
                infer_add_reg_edge(linked_infer.register_kind, keep_name);
            }
        }
    }
    for (TIdentifier name : infer->linked_pseudo_names) {
        InferenceRegister& linked_infer = context->p_inference_graph->pseudo_register_map[name];
        infer_rm_pseudo_edge(linked_infer, merge_name);
        infer_add_pseudo_edges(keep_name, name);
    }
    infer_rm_unpruned_pseudo_name(merge_name);
}

static void coal_hard_infer_reg(REGISTER_KIND register_kind, InferenceRegister* infer, size_t merge_index) {
    TIdentifier merge_name = context->data_flow_analysis_o2->data_name_map[merge_index - REGISTER_MASK_SIZE];
    if (infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < context->p_inference_graph->k; ++i) {
            InferenceRegister& linked_infer = context->hard_registers[i + context->p_inference_graph->offset];
            if (register_mask_get(infer->linked_hard_mask, linked_infer.register_kind)) {
                infer_rm_pseudo_edge(linked_infer, merge_name);
            }
        }
    }
    for (TIdentifier name : infer->linked_pseudo_names) {
        InferenceRegister& linked_infer = context->p_inference_graph->pseudo_register_map[name];
        infer_rm_pseudo_edge(linked_infer, merge_name);
        infer_add_reg_edge(register_kind, name);
    }
    infer_rm_unpruned_pseudo_name(merge_name);
}

static bool coal_infer_regs(AsmMov* node) {
    InferenceRegister* src_infer = nullptr;
    InferenceRegister* dst_infer = nullptr;
    size_t src_index = get_coalesced_idx(node->src.get());
    size_t dst_index = get_coalesced_idx(node->dst.get());
    if (get_coalescable_infer_regs(src_infer, dst_infer, src_index, dst_index)
        && coal_conservative_tests(src_infer, dst_infer)) {
        if (src_index < REGISTER_MASK_SIZE) {
            coal_hard_infer_reg(src_infer->register_kind, dst_infer, dst_index);
            context->data_flow_analysis->open_data_map[dst_index - REGISTER_MASK_SIZE] = src_index;
        }
        else {
            if (dst_index < REGISTER_MASK_SIZE) {
                coal_hard_infer_reg(dst_infer->register_kind, src_infer, src_index);
            }
            else {
                coal_pseudo_infer_reg(src_infer, src_index, dst_index);
            }
            context->data_flow_analysis->open_data_map[src_index - REGISTER_MASK_SIZE] = dst_index;
        }
        return true;
    }
    else {
        return false;
    }
}

static std::shared_ptr<AsmOperand> coal_op_reg(TIdentifier name, size_t coalesced_index) {
    if (coalesced_index < context->data_flow_analysis->set_size
        && coalesced_index != context->control_flow_graph->identifier_id_map[name]) {
        if (coalesced_index < REGISTER_MASK_SIZE) {
            REGISTER_KIND register_kind = context->hard_registers[coalesced_index].register_kind;
            return gen_register(register_kind);
        }
        else {
            set_p_infer_graph(frontend->symbol_table[name]->type_t->type() == AST_Double_t);
            name = context->data_flow_analysis_o2->data_name_map[coalesced_index - REGISTER_MASK_SIZE];
            context->p_inference_graph->pseudo_register_map[name].spill_cost++;
            return std::make_shared<AsmPseudo>(std::move(name));
        }
    }
    else {
        return nullptr;
    }
}

static void coal_mov_instr(AsmMov* node, size_t instruction_index, size_t block_id) {
    size_t src_index = get_coalesced_idx(node->src.get());
    size_t dst_index = get_coalesced_idx(node->dst.get());
    if (src_index < context->data_flow_analysis->set_size && src_index == dst_index) {
        cfg_rm_block_instr(instruction_index, block_id);
    }
    else {
        if (node->src->type() == AST_AsmPseudo_t) {
            std::shared_ptr<AsmOperand> operand_register =
                coal_op_reg(static_cast<AsmPseudo*>(node->src.get())->name, src_index);
            if (operand_register) {
                node->src = std::move(operand_register);
            }
        }
        if (node->dst->type() == AST_AsmPseudo_t) {
            std::shared_ptr<AsmOperand> operand_register =
                coal_op_reg(static_cast<AsmPseudo*>(node->dst.get())->name, dst_index);
            if (operand_register) {
                node->dst = std::move(operand_register);
            }
        }
    }
}

static void coal_mov_sx_instr(AsmMovSx* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_index = get_coalesced_idx(node->src.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->src.get())->name, src_index);
        if (operand_register) {
            node->src = std::move(operand_register);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_index = get_coalesced_idx(node->dst.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->dst.get())->name, dst_index);
        if (operand_register) {
            node->dst = std::move(operand_register);
        }
    }
}

static void coal_zero_extend_instr(AsmMovZeroExtend* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_index = get_coalesced_idx(node->src.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->src.get())->name, src_index);
        if (operand_register) {
            node->src = std::move(operand_register);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_index = get_coalesced_idx(node->dst.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->dst.get())->name, dst_index);
        if (operand_register) {
            node->dst = std::move(operand_register);
        }
    }
}

static void coal_lea_instr(AsmLea* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_index = get_coalesced_idx(node->src.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->src.get())->name, src_index);
        if (operand_register) {
            node->src = std::move(operand_register);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_index = get_coalesced_idx(node->dst.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->dst.get())->name, dst_index);
        if (operand_register) {
            node->dst = std::move(operand_register);
        }
    }
}

static void coal_cvttsd2si_instr(AsmCvttsd2si* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_index = get_coalesced_idx(node->src.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->src.get())->name, src_index);
        if (operand_register) {
            node->src = std::move(operand_register);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_index = get_coalesced_idx(node->dst.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->dst.get())->name, dst_index);
        if (operand_register) {
            node->dst = std::move(operand_register);
        }
    }
}

static void coal_cvtsi2sd_instr(AsmCvtsi2sd* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_index = get_coalesced_idx(node->src.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->src.get())->name, src_index);
        if (operand_register) {
            node->src = std::move(operand_register);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_index = get_coalesced_idx(node->dst.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->dst.get())->name, dst_index);
        if (operand_register) {
            node->dst = std::move(operand_register);
        }
    }
}

static void coal_unary_instr(AsmUnary* node) {
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_index = get_coalesced_idx(node->dst.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->dst.get())->name, dst_index);
        if (operand_register) {
            node->dst = std::move(operand_register);
        }
    }
}

static void coal_binary_instr(AsmBinary* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_index = get_coalesced_idx(node->src.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->src.get())->name, src_index);
        if (operand_register) {
            node->src = std::move(operand_register);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_index = get_coalesced_idx(node->dst.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->dst.get())->name, dst_index);
        if (operand_register) {
            node->dst = std::move(operand_register);
        }
    }
}

static void coal_cmp_instr(AsmCmp* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_index = get_coalesced_idx(node->src.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->src.get())->name, src_index);
        if (operand_register) {
            node->src = std::move(operand_register);
        }
    }
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_index = get_coalesced_idx(node->dst.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->dst.get())->name, dst_index);
        if (operand_register) {
            node->dst = std::move(operand_register);
        }
    }
}

static void coal_idiv_instr(AsmIdiv* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_index = get_coalesced_idx(node->src.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->src.get())->name, src_index);
        if (operand_register) {
            node->src = std::move(operand_register);
        }
    }
}

static void coal_div_instr(AsmDiv* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_index = get_coalesced_idx(node->src.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->src.get())->name, src_index);
        if (operand_register) {
            node->src = std::move(operand_register);
        }
    }
}

static void coal_set_cc_instr(AsmSetCC* node) {
    if (node->dst->type() == AST_AsmPseudo_t) {
        size_t dst_index = get_coalesced_idx(node->dst.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->dst.get())->name, dst_index);
        if (operand_register) {
            node->dst = std::move(operand_register);
        }
    }
}

static void coal_push_instr(AsmPush* node) {
    if (node->src->type() == AST_AsmPseudo_t) {
        size_t src_index = get_coalesced_idx(node->src.get());
        std::shared_ptr<AsmOperand> operand_register =
            coal_op_reg(static_cast<AsmPseudo*>(node->src.get())->name, src_index);
        if (operand_register) {
            node->src = std::move(operand_register);
        }
    }
}

static void coal_instr(size_t instruction_index, size_t block_id) {
    AsmInstruction* node = GET_INSTR(instruction_index).get();
    switch (node->type()) {
        case AST_AsmMov_t:
            coal_mov_instr(static_cast<AsmMov*>(node), instruction_index, block_id);
            break;
        case AST_AsmMovSx_t:
            coal_mov_sx_instr(static_cast<AsmMovSx*>(node));
            break;
        case AST_AsmMovZeroExtend_t:
            coal_zero_extend_instr(static_cast<AsmMovZeroExtend*>(node));
            break;
        case AST_AsmLea_t:
            coal_lea_instr(static_cast<AsmLea*>(node));
            break;
        case AST_AsmCvttsd2si_t:
            coal_cvttsd2si_instr(static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_AsmCvtsi2sd_t:
            coal_cvtsi2sd_instr(static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_AsmUnary_t:
            coal_unary_instr(static_cast<AsmUnary*>(node));
            break;
        case AST_AsmBinary_t:
            coal_binary_instr(static_cast<AsmBinary*>(node));
            break;
        case AST_AsmCmp_t:
            coal_cmp_instr(static_cast<AsmCmp*>(node));
            break;
        case AST_AsmIdiv_t:
            coal_idiv_instr(static_cast<AsmIdiv*>(node));
            break;
        case AST_AsmDiv_t:
            coal_div_instr(static_cast<AsmDiv*>(node));
            break;
        case AST_AsmSetCC_t:
            coal_set_cc_instr(static_cast<AsmSetCC*>(node));
            break;
        case AST_AsmPush_t:
            coal_push_instr(static_cast<AsmPush*>(node));
            break;
        case AST_AsmCdq_t:
        case AST_AsmCall_t:
            break;
        default:
            break;
    }
}

static bool coalesce_registers() {
    {
        size_t open_data_map_size = context->data_flow_analysis->set_size - REGISTER_MASK_SIZE;
        if (context->data_flow_analysis->open_data_map.size() < open_data_map_size) {
            context->data_flow_analysis->open_data_map.resize(open_data_map_size);
        }
    }
    for (size_t i = REGISTER_MASK_SIZE; i < context->data_flow_analysis->set_size; ++i) {
        context->data_flow_analysis->open_data_map[i - REGISTER_MASK_SIZE] = i;
    }

    {
        bool is_fixed_point = true;
        for (size_t instruction_index = 0; instruction_index < context->p_instructions->size(); ++instruction_index) {
            if (GET_INSTR(instruction_index) && GET_INSTR(instruction_index)->type() == AST_AsmMov_t
                && coal_infer_regs(static_cast<AsmMov*>(GET_INSTR(instruction_index).get()))) {
                is_fixed_point = false;
            }
        }
        if (is_fixed_point) {
            return false;
        }
    }

    for (size_t block_id = 0; block_id < context->control_flow_graph->blocks.size(); ++block_id) {
        if (GET_CFG_BLOCK(block_id).size > 0) {
            for (size_t instruction_index = GET_CFG_BLOCK(block_id).instructions_front_index;
                 instruction_index <= GET_CFG_BLOCK(block_id).instructions_back_index; ++instruction_index) {
                if (GET_INSTR(instruction_index)) {
                    coal_instr(instruction_index, block_id);
                }
            }
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void alloc_fun_toplvl(AsmFunction* node) {
    context->p_instructions = &node->instructions;
    init_control_flow_graph();
Ldowhile:
    if (init_inference_graph(node->name)) {
        if (context->is_with_coalescing && coalesce_registers()) {
            if (context->inference_graph->unpruned_pseudo_names.empty()
                && context->sse_inference_graph->unpruned_pseudo_names.empty()) {
                goto Lbreak;
            }
            goto Ldowhile;
        }
        {
            BackendFun* backend_fun = static_cast<BackendFun*>(backend->backend_symbol_table[node->name].get());
            context->p_backend_fun_top_level = backend_fun;
        }
        reallocate_registers();
        context->p_backend_fun_top_level = nullptr;
    }
Lbreak:
    context->p_inference_graph = nullptr;
    context->p_instructions = nullptr;
}

static void alloc_toplvl(AsmTopLevel* node) {
    switch (node->type()) {
        case AST_AsmFunction_t:
            alloc_fun_toplvl(static_cast<AsmFunction*>(node));
            break;
        case AST_AsmStaticVariable_t:
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void alloc_program(AsmProgram* node) {
    for (const auto& top_level : node->top_levels) {
        alloc_toplvl(top_level.get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void allocate_registers(AsmProgram* node, uint8_t optim_2_code) {
    context = std::make_unique<RegAllocContext>(std::move(optim_2_code));
    context->control_flow_graph = std::make_unique<ControlFlowGraph>();
    context->data_flow_analysis = std::make_unique<DataFlowAnalysis>();
    context->data_flow_analysis_o2 = std::make_unique<DataFlowAnalysisO2>();
    {
        context->hard_registers[0].register_kind = REG_Ax;
        context->hard_registers[1].register_kind = REG_Bx;
        context->hard_registers[2].register_kind = REG_Cx;
        context->hard_registers[3].register_kind = REG_Dx;
        context->hard_registers[4].register_kind = REG_Di;
        context->hard_registers[5].register_kind = REG_Si;
        context->hard_registers[6].register_kind = REG_R8;
        context->hard_registers[7].register_kind = REG_R9;
        context->hard_registers[8].register_kind = REG_R12;
        context->hard_registers[9].register_kind = REG_R13;
        context->hard_registers[10].register_kind = REG_R14;
        context->hard_registers[11].register_kind = REG_R15;

        context->hard_registers[12].register_kind = REG_Xmm0;
        context->hard_registers[13].register_kind = REG_Xmm1;
        context->hard_registers[14].register_kind = REG_Xmm2;
        context->hard_registers[15].register_kind = REG_Xmm3;
        context->hard_registers[16].register_kind = REG_Xmm4;
        context->hard_registers[17].register_kind = REG_Xmm5;
        context->hard_registers[18].register_kind = REG_Xmm6;
        context->hard_registers[19].register_kind = REG_Xmm7;
        context->hard_registers[20].register_kind = REG_Xmm8;
        context->hard_registers[21].register_kind = REG_Xmm9;
        context->hard_registers[22].register_kind = REG_Xmm10;
        context->hard_registers[23].register_kind = REG_Xmm11;
        context->hard_registers[24].register_kind = REG_Xmm12;
        context->hard_registers[25].register_kind = REG_Xmm13;
    }
    context->inference_graph = std::make_unique<InferenceGraph>();
    {
        context->inference_graph->k = 12;
        context->inference_graph->offset = 0;

        context->inference_graph->hard_reg_mask = REGISTER_MASK_FALSE;
        register_mask_set(context->inference_graph->hard_reg_mask, REG_Ax, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REG_Bx, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REG_Cx, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REG_Dx, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REG_Di, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REG_Si, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REG_R8, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REG_R9, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REG_R12, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REG_R13, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REG_R14, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REG_R15, true);
    }
    context->sse_inference_graph = std::make_unique<InferenceGraph>();
    {
        context->sse_inference_graph->k = 14;
        context->sse_inference_graph->offset = 12;

        context->sse_inference_graph->hard_reg_mask = REGISTER_MASK_FALSE;
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REG_Xmm0, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REG_Xmm1, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REG_Xmm2, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REG_Xmm3, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REG_Xmm4, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REG_Xmm5, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REG_Xmm6, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REG_Xmm7, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REG_Xmm8, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REG_Xmm9, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REG_Xmm10, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REG_Xmm11, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REG_Xmm12, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REG_Xmm13, true);
    }
    alloc_program(node);
    context.reset();
}
