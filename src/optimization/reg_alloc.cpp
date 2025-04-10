#include <algorithm>
#include <array>
#include <inttypes.h>
#include <memory>
#include <unordered_map>
#include <vector>

#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/back_ast.hpp"
#include "ast/back_symt.hpp"
#include "ast/front_symt.hpp"

#include "backend/assembly/registers.hpp"

#include "optimization/reg_alloc.hpp"

#define __MASK_TYPE__
using mask_t = TULong;

struct ControlFlowGraph;
struct DataFlowAnalysis;

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
#undef _OPTIMIZATION_IMPL_OLVL_HPP
#include "optimization/impl_olvl.hpp"
#undef __OPTIM_LEVEL__
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Inference graph

static bool is_binary_bitshift_cl(AsmBinary* node) {
    switch (node->binary_op->type()) {
        case AST_T::AsmBitShiftLeft_t:
        case AST_T::AsmBitShiftRight_t:
        case AST_T::AsmBitShrArithmetic_t:
            return node->src->type() != AST_T::AsmImm_t;
        default:
            return false;
    }
}

static void inference_graph_transfer_used_reg_live_registers(
    REGISTER_KIND register_kind, size_t next_instruction_index) {
    SET_DFA_INSTRUCTION_SET_AT(next_instruction_index, register_mask_bit(register_kind), true);
}

static void inference_graph_transfer_used_name_live_registers(TIdentifier name, size_t next_instruction_index) {
    if (!is_aliased_name(name)) {
        size_t i = context->control_flow_graph->identifier_id_map[name];
        SET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i, true);
    }
}

static void inference_graph_transfer_used_operand_live_registers(AsmOperand* node, size_t next_instruction_index) {
    switch (node->type()) {
        case AST_T::AsmRegister_t: {
            REGISTER_KIND register_kind = register_mask_kind(static_cast<AsmRegister*>(node)->reg.get());
            if (register_kind != REGISTER_KIND::Sp) {
                inference_graph_transfer_used_reg_live_registers(register_kind, next_instruction_index);
            }
            break;
        }
        case AST_T::AsmPseudo_t:
            inference_graph_transfer_used_name_live_registers(
                static_cast<AsmPseudo*>(node)->name, next_instruction_index);
            break;
        case AST_T::AsmMemory_t: {
            REGISTER_KIND register_kind = register_mask_kind(static_cast<AsmMemory*>(node)->reg.get());
            if (register_kind != REGISTER_KIND::Sp) {
                inference_graph_transfer_used_reg_live_registers(register_kind, next_instruction_index);
            }
            break;
        }
        case AST_T::AsmIndexed_t: {
            AsmIndexed* p_node = static_cast<AsmIndexed*>(node);
            {
                REGISTER_KIND register_kind = register_mask_kind(p_node->reg_base.get());
                inference_graph_transfer_used_reg_live_registers(register_kind, next_instruction_index);
            }
            {
                REGISTER_KIND register_kind = register_mask_kind(p_node->reg_index.get());
                inference_graph_transfer_used_reg_live_registers(register_kind, next_instruction_index);
            }
            break;
        }
        default:
            break;
    }
}

static void inference_graph_transfer_used_call_live_registers(AsmCall* node, size_t next_instruction_index) {
    FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[node->name]->type_t.get());
    GET_DFA_INSTRUCTION_SET_MASK(next_instruction_index, 0) |= fun_type->param_reg_mask;
}

static void inference_graph_transfer_updated_reg_live_registers(
    REGISTER_KIND register_kind, size_t next_instruction_index) {
    SET_DFA_INSTRUCTION_SET_AT(next_instruction_index, register_mask_bit(register_kind), false);
}

static void inference_graph_transfer_updated_name_live_registers(TIdentifier name, size_t next_instruction_index) {
    if (!is_aliased_name(name)) {
        size_t i = context->control_flow_graph->identifier_id_map[name];
        SET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i, false);
    }
}

static void inference_graph_transfer_updated_operand_live_registers(AsmOperand* node, size_t next_instruction_index) {
    switch (node->type()) {
        case AST_T::AsmRegister_t: {
            REGISTER_KIND register_kind = register_mask_kind(static_cast<AsmRegister*>(node)->reg.get());
            if (register_kind != REGISTER_KIND::Sp) {
                inference_graph_transfer_updated_reg_live_registers(register_kind, next_instruction_index);
            }
            break;
        }
        case AST_T::AsmPseudo_t:
            inference_graph_transfer_updated_name_live_registers(
                static_cast<AsmPseudo*>(node)->name, next_instruction_index);
            break;
        case AST_T::AsmMemory_t: {
            REGISTER_KIND register_kind = register_mask_kind(static_cast<AsmMemory*>(node)->reg.get());
            if (register_kind != REGISTER_KIND::Sp) {
                inference_graph_transfer_used_reg_live_registers(register_kind, next_instruction_index);
            }
            break;
        }
        case AST_T::AsmIndexed_t:
            RAISE_INTERNAL_ERROR;
        default:
            break;
    }
}

static void inference_graph_transfer_live_registers(size_t instruction_index, size_t next_instruction_index) {
    AsmInstruction* node = GET_INSTRUCTION(instruction_index).get();
    switch (node->type()) {
        case AST_T::AsmMov_t: {
            AsmMov* p_node = static_cast<AsmMov*>(node);
            inference_graph_transfer_updated_operand_live_registers(p_node->dst.get(), next_instruction_index);
            inference_graph_transfer_used_operand_live_registers(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_T::AsmMovSx_t: {
            AsmMovSx* p_node = static_cast<AsmMovSx*>(node);
            inference_graph_transfer_updated_operand_live_registers(p_node->dst.get(), next_instruction_index);
            inference_graph_transfer_used_operand_live_registers(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_T::AsmMovZeroExtend_t: {
            AsmMovZeroExtend* p_node = static_cast<AsmMovZeroExtend*>(node);
            inference_graph_transfer_updated_operand_live_registers(p_node->dst.get(), next_instruction_index);
            inference_graph_transfer_used_operand_live_registers(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_T::AsmLea_t: {
            AsmLea* p_node = static_cast<AsmLea*>(node);
            inference_graph_transfer_updated_operand_live_registers(p_node->dst.get(), next_instruction_index);
            inference_graph_transfer_used_operand_live_registers(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_T::AsmCvttsd2si_t: {
            AsmCvttsd2si* p_node = static_cast<AsmCvttsd2si*>(node);
            inference_graph_transfer_updated_operand_live_registers(p_node->dst.get(), next_instruction_index);
            inference_graph_transfer_used_operand_live_registers(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_T::AsmCvtsi2sd_t: {
            AsmCvtsi2sd* p_node = static_cast<AsmCvtsi2sd*>(node);
            inference_graph_transfer_updated_operand_live_registers(p_node->dst.get(), next_instruction_index);
            inference_graph_transfer_used_operand_live_registers(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_T::AsmUnary_t:
            inference_graph_transfer_used_operand_live_registers(
                static_cast<AsmUnary*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_T::AsmBinary_t: {
            AsmBinary* p_node = static_cast<AsmBinary*>(node);
            inference_graph_transfer_used_operand_live_registers(p_node->src.get(), next_instruction_index);
            inference_graph_transfer_used_operand_live_registers(p_node->dst.get(), next_instruction_index);
            if (is_binary_bitshift_cl(p_node)) {
                inference_graph_transfer_used_reg_live_registers(REGISTER_KIND::Cx, next_instruction_index);
            }
            break;
        }
        case AST_T::AsmCmp_t: {
            AsmCmp* p_node = static_cast<AsmCmp*>(node);
            inference_graph_transfer_used_operand_live_registers(p_node->src.get(), next_instruction_index);
            inference_graph_transfer_used_operand_live_registers(p_node->dst.get(), next_instruction_index);
            break;
        }
        case AST_T::AsmIdiv_t:
            inference_graph_transfer_used_operand_live_registers(
                static_cast<AsmIdiv*>(node)->src.get(), next_instruction_index);
            inference_graph_transfer_used_reg_live_registers(REGISTER_KIND::Ax, next_instruction_index);
            inference_graph_transfer_used_reg_live_registers(REGISTER_KIND::Dx, next_instruction_index);
            break;
        case AST_T::AsmDiv_t:
            inference_graph_transfer_used_operand_live_registers(
                static_cast<AsmDiv*>(node)->src.get(), next_instruction_index);
            inference_graph_transfer_used_reg_live_registers(REGISTER_KIND::Ax, next_instruction_index);
            break;
        case AST_T::AsmCdq_t:
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::Dx, next_instruction_index);
            inference_graph_transfer_used_reg_live_registers(REGISTER_KIND::Ax, next_instruction_index);
            break;
        case AST_T::AsmSetCC_t:
            inference_graph_transfer_updated_operand_live_registers(
                static_cast<AsmSetCC*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_T::AsmPush_t:
            inference_graph_transfer_used_operand_live_registers(
                static_cast<AsmPush*>(node)->src.get(), next_instruction_index);
            break;
        case AST_T::AsmCall_t:
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::Ax, next_instruction_index);
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::Cx, next_instruction_index);
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::Dx, next_instruction_index);
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::Di, next_instruction_index);
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::Si, next_instruction_index);
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::R8, next_instruction_index);
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::R9, next_instruction_index);
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::Xmm0, next_instruction_index);
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::Xmm1, next_instruction_index);
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::Xmm2, next_instruction_index);
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::Xmm3, next_instruction_index);
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::Xmm4, next_instruction_index);
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::Xmm5, next_instruction_index);
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::Xmm6, next_instruction_index);
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::Xmm7, next_instruction_index);
            inference_graph_transfer_used_call_live_registers(static_cast<AsmCall*>(node), next_instruction_index);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void inference_graph_set_p(bool is_double) {
    context->p_inference_graph = is_double ? context->sse_inference_graph.get() : context->inference_graph.get();
}

static void inference_graph_add_pseudo_edges(TIdentifier name_1, TIdentifier name_2) {
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

static void inference_graph_add_reg_edge(TIdentifier name, REGISTER_KIND register_kind) {
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

static void inference_graph_initialize_used_name_edges(TIdentifier name) {
    if (!is_aliased_name(name)) {
        inference_graph_set_p(frontend->symbol_table[name]->type_t->type() == AST_T::Double_t);
        context->p_inference_graph->pseudo_register_map[name].spill_cost++;
    }
}

static void inference_graph_initialize_used_operand_edges(AsmOperand* node) {
    if (node->type() == AST_T::AsmPseudo_t) {
        inference_graph_initialize_used_name_edges(static_cast<AsmPseudo*>(node)->name);
    }
}

static void inference_graph_initialize_updated_regs_edges(
    REGISTER_KIND* register_kinds, size_t instruction_index, size_t register_kinds_size, bool is_double) {
    inference_graph_set_p(is_double);

    size_t mov_mask_bit = context->data_flow_analysis->set_size;
    bool is_mov = GET_INSTRUCTION(instruction_index)->type() == AST_T::AsmMov_t;
    if (is_mov) {
        AsmMov* mov = static_cast<AsmMov*>(GET_INSTRUCTION(instruction_index).get());
        if (mov->src->type() == AST_T::AsmPseudo_t) {
            TIdentifier src_name = static_cast<AsmPseudo*>(mov->src.get())->name;
            if (is_aliased_name(src_name)) {
                is_mov = false;
            }
            else {
                mov_mask_bit = context->control_flow_graph->identifier_id_map[src_name];
                context->p_inference_graph->pseudo_register_map[src_name].spill_cost++;
            }
        }
        else {
            is_mov = false;
        }
    }

    if (GET_DFA_INSTRUCTION_SET_MASK(instruction_index, 0) != MASK_FALSE) {
        for (size_t i = context->data_flow_analysis->set_size < 64 ? context->data_flow_analysis->set_size : 64;
             i-- > REGISTER_MASK_SIZE;) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i) && !(is_mov && i == mov_mask_bit)) {
                TIdentifier pseudo_name = context->data_flow_analysis->data_name_map[i - REGISTER_MASK_SIZE];
                if (is_double == (frontend->symbol_table[pseudo_name]->type_t->type() == AST_T::Double_t)) {
                    for (size_t j = 0; j < register_kinds_size; ++j) {
                        inference_graph_add_reg_edge(pseudo_name, register_kinds[j]);
                    }
                }
            }
        }
    }
    size_t i = 64;
    for (size_t j = 1; j < context->data_flow_analysis->mask_size; ++j) {
        if (GET_DFA_INSTRUCTION_SET_MASK(instruction_index, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > context->data_flow_analysis->set_size) {
            mask_set_size = context->data_flow_analysis->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i) && !(is_mov && i == mov_mask_bit)) {
                TIdentifier pseudo_name = context->data_flow_analysis->data_name_map[i - REGISTER_MASK_SIZE];
                if (is_double == (frontend->symbol_table[pseudo_name]->type_t->type() == AST_T::Double_t)) {
                    for (size_t k = 0; k < register_kinds_size; ++k) {
                        inference_graph_add_reg_edge(pseudo_name, register_kinds[k]);
                    }
                }
            }
        }
    }
}

static void inference_graph_initialize_updated_name_edges(TIdentifier name, size_t instruction_index) {
    if (is_aliased_name(name)) {
        return;
    }
    bool is_double = frontend->symbol_table[name]->type_t->type() == AST_T::Double_t;
    inference_graph_set_p(is_double);
    context->p_inference_graph->pseudo_register_map[name].spill_cost++;

    size_t mov_mask_bit = context->data_flow_analysis->set_size;
    bool is_mov = GET_INSTRUCTION(instruction_index)->type() == AST_T::AsmMov_t;
    if (is_mov) {
        AsmMov* mov = static_cast<AsmMov*>(GET_INSTRUCTION(instruction_index).get());
        switch (mov->src->type()) {
            case AST_T::AsmRegister_t: {
                REGISTER_KIND src_register_kind =
                    register_mask_kind(static_cast<AsmRegister*>(mov->src.get())->reg.get());
                if (src_register_kind == REGISTER_KIND::Sp) {
                    is_mov = false;
                }
                else {
                    mov_mask_bit = register_mask_bit(src_register_kind);
                }
                break;
            }
            case AST_T::AsmPseudo_t: {
                TIdentifier src_name = static_cast<AsmPseudo*>(mov->src.get())->name;
                if (is_aliased_name(src_name)) {
                    is_mov = false;
                }
                else {
                    mov_mask_bit = context->control_flow_graph->identifier_id_map[src_name];
                    context->p_inference_graph->pseudo_register_map[src_name].spill_cost++;
                }
                break;
            }
            case AST_T::AsmMemory_t: {
                REGISTER_KIND src_register_kind =
                    register_mask_kind(static_cast<AsmMemory*>(mov->src.get())->reg.get());
                if (src_register_kind == REGISTER_KIND::Sp) {
                    is_mov = false;
                }
                else {
                    mov_mask_bit = register_mask_bit(src_register_kind);
                }
                break;
            }
            case AST_T::AsmIndexed_t:
                RAISE_INTERNAL_ERROR;
            default: {
                is_mov = false;
                break;
            }
        }
    }

    if (GET_DFA_INSTRUCTION_SET_MASK(instruction_index, 0) != MASK_FALSE) {
        size_t i = context->p_inference_graph->offset;
        size_t mask_set_size = i + context->p_inference_graph->k;
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i) && !(is_mov && i == mov_mask_bit)) {
                REGISTER_KIND register_kind = context->hard_registers[i].register_kind;
                inference_graph_add_reg_edge(name, register_kind);
            }
        }
        i = REGISTER_MASK_SIZE;
        mask_set_size = context->data_flow_analysis->set_size < 64 ? context->data_flow_analysis->set_size : 64;
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i) && !(is_mov && i == mov_mask_bit)) {
                TIdentifier pseudo_name = context->data_flow_analysis->data_name_map[i - REGISTER_MASK_SIZE];
                if (name != pseudo_name
                    && is_double == (frontend->symbol_table[pseudo_name]->type_t->type() == AST_T::Double_t)) {
                    inference_graph_add_pseudo_edges(name, pseudo_name);
                }
            }
        }
    }
    size_t i = 64;
    for (size_t j = 1; j < context->data_flow_analysis->mask_size; ++j) {
        if (GET_DFA_INSTRUCTION_SET_MASK(instruction_index, j) == MASK_FALSE) {
            i += 64;
            continue;
        }
        size_t mask_set_size = i + 64;
        if (mask_set_size > context->data_flow_analysis->set_size) {
            mask_set_size = context->data_flow_analysis->set_size;
        }
        for (; i < mask_set_size; ++i) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i) && !(is_mov && i == mov_mask_bit)) {
                TIdentifier pseudo_name = context->data_flow_analysis->data_name_map[i - REGISTER_MASK_SIZE];
                if (name != pseudo_name
                    && is_double == (frontend->symbol_table[pseudo_name]->type_t->type() == AST_T::Double_t)) {
                    inference_graph_add_pseudo_edges(name, pseudo_name);
                }
            }
        }
    }
}

static void inference_graph_initialize_updated_operand_edges(AsmOperand* node, size_t instruction_index) {
    switch (node->type()) {
        case AST_T::AsmRegister_t: {
            REGISTER_KIND register_kinds[1] = {register_mask_kind(static_cast<AsmRegister*>(node)->reg.get())};
            if (register_kinds[0] != REGISTER_KIND::Sp) {
                bool is_double = register_mask_bit(register_kinds[0]) > 11;
                inference_graph_initialize_updated_regs_edges(register_kinds, instruction_index, 1, is_double);
            }
            break;
        }
        case AST_T::AsmPseudo_t:
            inference_graph_initialize_updated_name_edges(static_cast<AsmPseudo*>(node)->name, instruction_index);
            break;
        default:
            break;
    }
}

static void inference_graph_initialize_edges(size_t instruction_index) {
    AsmInstruction* node = GET_INSTRUCTION(instruction_index).get();
    switch (node->type()) {
        case AST_T::AsmMov_t:
            inference_graph_initialize_updated_operand_edges(static_cast<AsmMov*>(node)->dst.get(), instruction_index);
            break;
        case AST_T::AsmMovSx_t:
            inference_graph_initialize_updated_operand_edges(
                static_cast<AsmMovSx*>(node)->dst.get(), instruction_index);
            break;
        case AST_T::AsmMovZeroExtend_t:
            inference_graph_initialize_updated_operand_edges(
                static_cast<AsmMovZeroExtend*>(node)->dst.get(), instruction_index);
            break;
        case AST_T::AsmLea_t:
            inference_graph_initialize_updated_operand_edges(static_cast<AsmLea*>(node)->dst.get(), instruction_index);
            break;
        case AST_T::AsmCvttsd2si_t:
            inference_graph_initialize_updated_operand_edges(
                static_cast<AsmCvttsd2si*>(node)->dst.get(), instruction_index);
            break;
        case AST_T::AsmCvtsi2sd_t:
            inference_graph_initialize_updated_operand_edges(
                static_cast<AsmCvtsi2sd*>(node)->dst.get(), instruction_index);
            break;
        case AST_T::AsmUnary_t:
            inference_graph_initialize_updated_operand_edges(
                static_cast<AsmUnary*>(node)->dst.get(), instruction_index);
            break;
        case AST_T::AsmBinary_t: {
            AsmBinary* p_node = static_cast<AsmBinary*>(node);
            if (is_binary_bitshift_cl(p_node)) {
                REGISTER_KIND register_kinds[1] = {REGISTER_KIND::Cx};
                inference_graph_initialize_updated_regs_edges(register_kinds, instruction_index, 1, false);
            }
            inference_graph_initialize_updated_operand_edges(p_node->dst.get(), instruction_index);
            inference_graph_initialize_used_operand_edges(p_node->src.get());
            break;
        }
        case AST_T::AsmCmp_t: {
            AsmCmp* p_node = static_cast<AsmCmp*>(node);
            inference_graph_initialize_used_operand_edges(p_node->src.get());
            inference_graph_initialize_used_operand_edges(p_node->dst.get());
            break;
        }
        case AST_T::AsmIdiv_t: {
            REGISTER_KIND register_kinds[2] = {REGISTER_KIND::Ax, REGISTER_KIND::Dx};
            inference_graph_initialize_updated_regs_edges(register_kinds, instruction_index, 2, false);
            inference_graph_initialize_used_operand_edges(static_cast<AsmIdiv*>(node)->src.get());
            break;
        }
        case AST_T::AsmDiv_t: {
            REGISTER_KIND register_kinds[1] = {REGISTER_KIND::Ax};
            inference_graph_initialize_updated_regs_edges(register_kinds, instruction_index, 1, false);
            inference_graph_initialize_used_operand_edges(static_cast<AsmDiv*>(node)->src.get());
            break;
        }
        case AST_T::AsmCdq_t: {
            REGISTER_KIND register_kinds[1] = {REGISTER_KIND::Dx};
            inference_graph_initialize_updated_regs_edges(register_kinds, instruction_index, 1, false);
            break;
        }
        case AST_T::AsmSetCC_t:
            inference_graph_initialize_updated_operand_edges(
                static_cast<AsmSetCC*>(node)->dst.get(), instruction_index);
            break;
        case AST_T::AsmPush_t:
            inference_graph_initialize_used_operand_edges(static_cast<AsmPush*>(node)->src.get());
            break;
        case AST_T::AsmCall_t: {
            {
                REGISTER_KIND register_kinds[7] = {REGISTER_KIND::Ax, REGISTER_KIND::Cx, REGISTER_KIND::Dx,
                    REGISTER_KIND::Di, REGISTER_KIND::Si, REGISTER_KIND::R8, REGISTER_KIND::R9};
                inference_graph_initialize_updated_regs_edges(register_kinds, instruction_index, 7, false);
            }
            {
                REGISTER_KIND register_kinds[8] = {REGISTER_KIND::Xmm0, REGISTER_KIND::Xmm1, REGISTER_KIND::Xmm2,
                    REGISTER_KIND::Xmm3, REGISTER_KIND::Xmm4, REGISTER_KIND::Xmm5, REGISTER_KIND::Xmm6,
                    REGISTER_KIND::Xmm7};
                inference_graph_initialize_updated_regs_edges(register_kinds, instruction_index, 8, true);
            }
            break;
        }
        default:
            break;
    }
}

static bool inference_graph_initialize(TIdentifier function_name) {
    control_flow_graph_initialize();
    if (!data_flow_analysis_initialize(function_name)) {
        return false;
    }
    data_flow_analysis_backward_iterative_algorithm();
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
        InferenceRegister infer = {REGISTER_KIND::Sp, REGISTER_KIND::Sp, 0, 0, REGISTER_MASK_FALSE, {}};
        if (frontend->symbol_table[name_id.first]->type_t->type() == AST_T::Double_t) {
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
            context->register_color_map[i] = REGISTER_KIND::Sp;
            context->hard_registers[i].color = REGISTER_KIND::Sp;
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
            context->register_color_map[i] = REGISTER_KIND::Sp;
            context->hard_registers[i].color = REGISTER_KIND::Sp;
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
                if (GET_INSTRUCTION(instruction_index)) {
                    inference_graph_initialize_edges(instruction_index);
                }
            }
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register allocation

static bool is_register_callee_saved(REGISTER_KIND register_kind) {
    switch (register_kind) {
        case REGISTER_KIND::Bx:
        case REGISTER_KIND::R12:
        case REGISTER_KIND::R13:
        case REGISTER_KIND::R14:
        case REGISTER_KIND::R15:
            return true;
        default:
            return false;
    }
}

static void regalloc_prune_inference_register(InferenceRegister* infer, size_t pruned_index) {
    if (infer->register_kind == REGISTER_KIND::Sp) {
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

static void regalloc_unprune_inference_register(InferenceRegister* infer, TIdentifier pruned_name) {
    if (infer->register_kind == REGISTER_KIND::Sp) {
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

static void regalloc_color_inference_graph();

static void regalloc_next_color_inference_graph() {
    if (!context->p_inference_graph->unpruned_hard_mask_bits.empty()
        || !context->p_inference_graph->unpruned_pseudo_names.empty()) {
        regalloc_color_inference_graph();
    }
}

static InferenceRegister* regalloc_prune_inference_graph(TIdentifier& pruned_name) {
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
    regalloc_prune_inference_register(infer, pruned_index);
    return infer;
}

static void regalloc_unprune_inference_graph(InferenceRegister* infer, TIdentifier pruned_name) {
    mask_t color_reg_mask = context->inference_graph->hard_reg_mask;
    if (infer->linked_hard_mask != REGISTER_MASK_FALSE) {
        for (size_t i = 0; i < context->p_inference_graph->k; ++i) {
            InferenceRegister& linked_infer = context->hard_registers[i + context->p_inference_graph->offset];
            if (register_mask_get(infer->linked_hard_mask, linked_infer.register_kind)) {
                if (linked_infer.color != REGISTER_KIND::Sp) {
                    register_mask_set(color_reg_mask, linked_infer.color, false);
                }
            }
        }
    }
    for (TIdentifier name : infer->linked_pseudo_names) {
        InferenceRegister& linked_infer = context->p_inference_graph->pseudo_register_map[name];
        if (linked_infer.color != REGISTER_KIND::Sp) {
            register_mask_set(color_reg_mask, linked_infer.color, false);
        }
    }
    if (color_reg_mask != REGISTER_MASK_FALSE) {
        if (is_register_callee_saved(infer->register_kind)) {
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
        regalloc_unprune_inference_register(infer, pruned_name);
    }
}

static void regalloc_color_inference_graph() {
    TIdentifier pruned_name;
    InferenceRegister* infer = regalloc_prune_inference_graph(pruned_name);
    regalloc_next_color_inference_graph();
    regalloc_unprune_inference_graph(infer, pruned_name);
}

static void regalloc_color_register_map() {
    for (size_t i = 0; i < context->p_inference_graph->k; ++i) {
        InferenceRegister& infer = context->hard_registers[i + context->p_inference_graph->offset];
        if (infer.color != REGISTER_KIND::Sp) {
            context->register_color_map[register_mask_bit(infer.color)] = infer.register_kind;
        }
    }
}

static std::shared_ptr<AsmRegister> regalloc_hard_register(TIdentifier name) {
    if (is_aliased_name(name)) {
        return nullptr;
    }
    inference_graph_set_p(frontend->symbol_table[name]->type_t->type() == AST_T::Double_t);
    REGISTER_KIND color = context->p_inference_graph->pseudo_register_map[name].color;
    if (color != REGISTER_KIND::Sp) {
        REGISTER_KIND register_kind = context->register_color_map[register_mask_bit(color)];
        std::shared_ptr<AsmRegister> hard_register = generate_register(register_kind);
        if (is_register_callee_saved(register_kind)
            && !register_mask_get(context->callee_saved_reg_mask, register_kind)) {
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

static REGISTER_KIND regalloc_operand_register_kind(AsmOperand* node) {
    switch (node->type()) {
        case AST_T::AsmRegister_t:
            return register_mask_kind(static_cast<AsmRegister*>(node)->reg.get());
        case AST_T::AsmPseudo_t: {
            TIdentifier name = static_cast<AsmPseudo*>(node)->name;
            if (is_aliased_name(name)) {
                return REGISTER_KIND::Sp;
            }
            inference_graph_set_p(frontend->symbol_table[name]->type_t->type() == AST_T::Double_t);
            REGISTER_KIND color = context->p_inference_graph->pseudo_register_map[name].color;
            if (color == REGISTER_KIND::Sp) {
                return REGISTER_KIND::Sp;
            }
            else {
                return context->register_color_map[register_mask_bit(color)];
            }
        }
        case AST_T::AsmMemory_t:
            return register_mask_kind(static_cast<AsmMemory*>(node)->reg.get());
        case AST_T::AsmIndexed_t:
            RAISE_INTERNAL_ERROR;
        default:
            return REGISTER_KIND::Sp;
    }
}

static void regalloc_mov_instructions(AsmMov* node, size_t instruction_index) {
    AsmOperand* src_operand = static_cast<AsmPseudo*>(node->src.get());
    AsmOperand* dst_operand = static_cast<AsmPseudo*>(node->dst.get());
    REGISTER_KIND src_register_kind = regalloc_operand_register_kind(src_operand);
    REGISTER_KIND dst_register_kind = regalloc_operand_register_kind(dst_operand);
    if (src_register_kind != REGISTER_KIND::Sp && src_register_kind == dst_register_kind) {
        set_instruction(nullptr, instruction_index);
    }
    else {
        if (src_operand->type() == AST_T::AsmPseudo_t) {
            std::shared_ptr<AsmOperand> hard_register =
                regalloc_hard_register(static_cast<AsmPseudo*>(src_operand)->name);
            if (hard_register) {
                node->src = hard_register;
            }
        }
        if (node->dst->type() == AST_T::AsmPseudo_t) {
            std::shared_ptr<AsmOperand> hard_register =
                regalloc_hard_register(static_cast<AsmPseudo*>(dst_operand)->name);
            if (hard_register) {
                node->dst = hard_register;
            }
        }
    }
}

static void regalloc_mov_sx_instructions(AsmMovSx* node) {
    if (node->src->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = hard_register;
        }
    }
    if (node->dst->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = hard_register;
        }
    }
}

static void regalloc_mov_zero_extend_instructions(AsmMovZeroExtend* node) {
    if (node->src->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = hard_register;
        }
    }
    if (node->dst->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = hard_register;
        }
    }
}

static void regalloc_lea_instructions(AsmLea* node) {
    if (node->src->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = hard_register;
        }
    }
    if (node->dst->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = hard_register;
        }
    }
}

static void regalloc_cvttsd2si_instructions(AsmCvttsd2si* node) {
    if (node->src->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = hard_register;
        }
    }
    if (node->dst->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = hard_register;
        }
    }
}

static void regalloc_cvtsi2sd_instructions(AsmCvtsi2sd* node) {
    if (node->src->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = hard_register;
        }
    }
    if (node->dst->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = hard_register;
        }
    }
}

static void regalloc_unary_instructions(AsmUnary* node) {
    if (node->dst->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = hard_register;
        }
    }
}

static void regalloc_binary_instructions(AsmBinary* node) {
    if (node->src->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = hard_register;
        }
    }
    if (node->dst->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = hard_register;
        }
    }
}

static void regalloc_cmp_instructions(AsmCmp* node) {
    if (node->src->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = hard_register;
        }
    }
    if (node->dst->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = hard_register;
        }
    }
}

static void regalloc_idiv_instructions(AsmIdiv* node) {
    if (node->src->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = hard_register;
        }
    }
}

static void regalloc_div_instructions(AsmDiv* node) {
    if (node->src->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = hard_register;
        }
    }
}

static void regalloc_set_cc_instructions(AsmSetCC* node) {
    if (node->dst->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->dst.get())->name);
        if (hard_register) {
            node->dst = hard_register;
        }
    }
}

static void regalloc_push_instructions(AsmPush* node) {
    if (node->src->type() == AST_T::AsmPseudo_t) {
        std::shared_ptr<AsmOperand> hard_register =
            regalloc_hard_register(static_cast<AsmPseudo*>(node->src.get())->name);
        if (hard_register) {
            node->src = hard_register;
        }
    }
}

static void regalloc_instructions(size_t instruction_index) {
    AsmInstruction* node = GET_INSTRUCTION(instruction_index).get();
    switch (node->type()) {
        case AST_T::AsmMov_t:
            regalloc_mov_instructions(static_cast<AsmMov*>(node), instruction_index);
            break;
        case AST_T::AsmMovSx_t:
            regalloc_mov_sx_instructions(static_cast<AsmMovSx*>(node));
            break;
        case AST_T::AsmMovZeroExtend_t:
            regalloc_mov_zero_extend_instructions(static_cast<AsmMovZeroExtend*>(node));
            break;
        case AST_T::AsmLea_t:
            regalloc_lea_instructions(static_cast<AsmLea*>(node));
            break;
        case AST_T::AsmCvttsd2si_t:
            regalloc_cvttsd2si_instructions(static_cast<AsmCvttsd2si*>(node));
            break;
        case AST_T::AsmCvtsi2sd_t:
            regalloc_cvtsi2sd_instructions(static_cast<AsmCvtsi2sd*>(node));
            break;
        case AST_T::AsmUnary_t:
            regalloc_unary_instructions(static_cast<AsmUnary*>(node));
            break;
        case AST_T::AsmBinary_t:
            regalloc_binary_instructions(static_cast<AsmBinary*>(node));
            break;
        case AST_T::AsmCmp_t:
            regalloc_cmp_instructions(static_cast<AsmCmp*>(node));
            break;
        case AST_T::AsmIdiv_t:
            regalloc_idiv_instructions(static_cast<AsmIdiv*>(node));
            break;
        case AST_T::AsmDiv_t:
            regalloc_div_instructions(static_cast<AsmDiv*>(node));
            break;
        case AST_T::AsmSetCC_t:
            regalloc_set_cc_instructions(static_cast<AsmSetCC*>(node));
            break;
        case AST_T::AsmPush_t:
            regalloc_push_instructions(static_cast<AsmPush*>(node));
            break;
        case AST_T::AsmCdq_t:
        case AST_T::AsmCall_t:
            break;
        default:
            break;
    }
}

static void regalloc_inference_graph() {
    if (!context->inference_graph->pseudo_register_map.empty()) {
        inference_graph_set_p(false);
        regalloc_color_inference_graph();
        regalloc_color_register_map();
    }
    if (!context->sse_inference_graph->pseudo_register_map.empty()) {
        inference_graph_set_p(true);
        regalloc_color_inference_graph();
        regalloc_color_register_map();
    }
    for (size_t instruction_index = 0; instruction_index < context->p_instructions->size(); ++instruction_index) {
        if (GET_INSTRUCTION(instruction_index)) {
            regalloc_instructions(instruction_index);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register coalescing

static void coalesce_inference_graph() {
    // TODO
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void regalloc_function_top_level(AsmFunction* node) {
    context->p_instructions = &node->instructions;
    if (inference_graph_initialize(node->name)) {
        if (context->is_with_coalescing) {
            coalesce_inference_graph();
        }
        {
            BackendFun* backend_fun = static_cast<BackendFun*>(backend->backend_symbol_table[node->name].get());
            context->p_backend_fun_top_level = backend_fun;
        }
        regalloc_inference_graph();
        context->p_backend_fun_top_level = nullptr;
        context->p_inference_graph = nullptr;
    }
    context->p_instructions = nullptr;
}

static void regalloc_top_level(AsmTopLevel* node) {
    switch (node->type()) {
        case AST_T::AsmFunction_t:
            regalloc_function_top_level(static_cast<AsmFunction*>(node));
            break;
        case AST_T::AsmStaticVariable_t:
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void regalloc_program(AsmProgram* node) {
    for (const auto& top_level : node->top_levels) {
        regalloc_top_level(top_level.get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void register_allocation(AsmProgram* node, uint8_t optim_2_code) {
    context = std::make_unique<RegAllocContext>(std::move(optim_2_code));
    context->control_flow_graph = std::make_unique<ControlFlowGraph>();
    context->data_flow_analysis = std::make_unique<DataFlowAnalysis>();
    {
        context->hard_registers[0].register_kind = REGISTER_KIND::Ax;
        context->hard_registers[1].register_kind = REGISTER_KIND::Bx;
        context->hard_registers[2].register_kind = REGISTER_KIND::Cx;
        context->hard_registers[3].register_kind = REGISTER_KIND::Dx;
        context->hard_registers[4].register_kind = REGISTER_KIND::Di;
        context->hard_registers[5].register_kind = REGISTER_KIND::Si;
        context->hard_registers[6].register_kind = REGISTER_KIND::R8;
        context->hard_registers[7].register_kind = REGISTER_KIND::R9;
        context->hard_registers[8].register_kind = REGISTER_KIND::R12;
        context->hard_registers[9].register_kind = REGISTER_KIND::R13;
        context->hard_registers[10].register_kind = REGISTER_KIND::R14;
        context->hard_registers[11].register_kind = REGISTER_KIND::R15;

        context->hard_registers[12].register_kind = REGISTER_KIND::Xmm0;
        context->hard_registers[13].register_kind = REGISTER_KIND::Xmm1;
        context->hard_registers[14].register_kind = REGISTER_KIND::Xmm2;
        context->hard_registers[15].register_kind = REGISTER_KIND::Xmm3;
        context->hard_registers[16].register_kind = REGISTER_KIND::Xmm4;
        context->hard_registers[17].register_kind = REGISTER_KIND::Xmm5;
        context->hard_registers[18].register_kind = REGISTER_KIND::Xmm6;
        context->hard_registers[19].register_kind = REGISTER_KIND::Xmm7;
        context->hard_registers[20].register_kind = REGISTER_KIND::Xmm8;
        context->hard_registers[21].register_kind = REGISTER_KIND::Xmm9;
        context->hard_registers[22].register_kind = REGISTER_KIND::Xmm10;
        context->hard_registers[23].register_kind = REGISTER_KIND::Xmm11;
        context->hard_registers[24].register_kind = REGISTER_KIND::Xmm12;
        context->hard_registers[25].register_kind = REGISTER_KIND::Xmm13;
    }
    context->inference_graph = std::make_unique<InferenceGraph>();
    {
        context->inference_graph->k = 12;
        context->inference_graph->offset = 0;

        context->inference_graph->hard_reg_mask = REGISTER_MASK_FALSE;
        register_mask_set(context->inference_graph->hard_reg_mask, REGISTER_KIND::Ax, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REGISTER_KIND::Bx, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REGISTER_KIND::Cx, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REGISTER_KIND::Dx, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REGISTER_KIND::Di, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REGISTER_KIND::Si, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REGISTER_KIND::R8, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REGISTER_KIND::R9, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REGISTER_KIND::R12, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REGISTER_KIND::R13, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REGISTER_KIND::R14, true);
        register_mask_set(context->inference_graph->hard_reg_mask, REGISTER_KIND::R15, true);
    }
    context->sse_inference_graph = std::make_unique<InferenceGraph>();
    {
        context->sse_inference_graph->k = 14;
        context->sse_inference_graph->offset = 12;

        context->sse_inference_graph->hard_reg_mask = REGISTER_MASK_FALSE;
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REGISTER_KIND::Xmm0, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REGISTER_KIND::Xmm1, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REGISTER_KIND::Xmm2, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REGISTER_KIND::Xmm3, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REGISTER_KIND::Xmm4, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REGISTER_KIND::Xmm5, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REGISTER_KIND::Xmm6, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REGISTER_KIND::Xmm7, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REGISTER_KIND::Xmm8, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REGISTER_KIND::Xmm9, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REGISTER_KIND::Xmm10, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REGISTER_KIND::Xmm11, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REGISTER_KIND::Xmm12, true);
        register_mask_set(context->sse_inference_graph->hard_reg_mask, REGISTER_KIND::Xmm13, true);
    }
    regalloc_program(node);
    context.reset();
}
