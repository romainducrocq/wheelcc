#include <algorithm>
#include <array>
#include <inttypes.h>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/back_ast.hpp"
#include "ast/front_symt.hpp"

#include "backend/assembly/registers.hpp"

#include "optimization/reg_alloc.hpp"

struct ControlFlowGraph;
struct DataFlowAnalysis;

struct InferenceNode {
    REGISTER_KIND hard_register;
    bool is_pruned;
    double spill_cost;
    size_t color;
    uint64_t linked_reg_mask;
    std::vector<TIdentifier> linked_pseudo_nodes;
};
#define HARD_REGISTER_NODE(X)    \
    {                            \
        X, false, 0., 0, 0ul, {} \
    }

struct RegAllocContext {
    RegAllocContext(uint8_t optim_2_code, uint64_t hard_reg_mask, uint64_t hard_sse_reg_mask);

    // Register allocation
    uint64_t HARD_REG_MASK;
    uint64_t HARD_SSE_REG_MASK;
    std::array<InferenceNode, 12> reg_inference_graph;
    std::array<InferenceNode, 14> sse_reg_inference_graph;
    std::vector<size_t> unpruned_reg_nodes;
    std::vector<size_t> unpruned_sse_reg_nodes;
    std::vector<TIdentifier> unpruned_pseudo_nodes;
    std::vector<TIdentifier> unpruned_sse_pseudo_nodes;
    std::unordered_map<TIdentifier, InferenceNode> pseudo_inference_graph;
    std::unordered_map<TIdentifier, InferenceNode> sse_pseudo_inference_graph;
    std::unique_ptr<ControlFlowGraph> control_flow_graph;
    std::unique_ptr<DataFlowAnalysis> data_flow_analysis;
    std::vector<std::unique_ptr<AsmInstruction>>* p_instructions;
    // Register coalescing
    bool is_with_coalescing;
};

RegAllocContext::RegAllocContext(uint8_t optim_2_code, uint64_t hard_reg_mask, uint64_t hard_sse_reg_mask) :
    HARD_REG_MASK(hard_reg_mask), HARD_SSE_REG_MASK(hard_sse_reg_mask),
    reg_inference_graph({HARD_REGISTER_NODE(REGISTER_KIND::Ax), HARD_REGISTER_NODE(REGISTER_KIND::Bx),
        HARD_REGISTER_NODE(REGISTER_KIND::Cx), HARD_REGISTER_NODE(REGISTER_KIND::Dx),
        HARD_REGISTER_NODE(REGISTER_KIND::Di), HARD_REGISTER_NODE(REGISTER_KIND::Si),
        HARD_REGISTER_NODE(REGISTER_KIND::R8), HARD_REGISTER_NODE(REGISTER_KIND::R9),
        HARD_REGISTER_NODE(REGISTER_KIND::R12), HARD_REGISTER_NODE(REGISTER_KIND::R13),
        HARD_REGISTER_NODE(REGISTER_KIND::R14), HARD_REGISTER_NODE(REGISTER_KIND::R15)}),
    sse_reg_inference_graph({HARD_REGISTER_NODE(REGISTER_KIND::Xmm0), HARD_REGISTER_NODE(REGISTER_KIND::Xmm1),
        HARD_REGISTER_NODE(REGISTER_KIND::Xmm2), HARD_REGISTER_NODE(REGISTER_KIND::Xmm3),
        HARD_REGISTER_NODE(REGISTER_KIND::Xmm4), HARD_REGISTER_NODE(REGISTER_KIND::Xmm5),
        HARD_REGISTER_NODE(REGISTER_KIND::Xmm6), HARD_REGISTER_NODE(REGISTER_KIND::Xmm7),
        HARD_REGISTER_NODE(REGISTER_KIND::Xmm8), HARD_REGISTER_NODE(REGISTER_KIND::Xmm9),
        HARD_REGISTER_NODE(REGISTER_KIND::Xmm10), HARD_REGISTER_NODE(REGISTER_KIND::Xmm11),
        HARD_REGISTER_NODE(REGISTER_KIND::Xmm12), HARD_REGISTER_NODE(REGISTER_KIND::Xmm13)}),
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

static void set_instruction(std::unique_ptr<AsmInstruction>&& instruction, size_t instruction_index) {
    if (instruction) {
        GET_INSTRUCTION(instruction_index) = std::move(instruction);
    }
    else {
        GET_INSTRUCTION(instruction_index).reset();
    }
}

static void regalloc_transfer_used_name_live_registers(TIdentifier name, size_t next_instruction_index) {
    if (frontend->symbol_table[name]->attrs->type() != AST_T::StaticAttr_t) {
        size_t i = context->control_flow_graph->identifier_id_map[name];
        SET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i, true);
    }
}

static void regalloc_transfer_used_operand_live_registers(AsmOperand* node, size_t next_instruction_index) {
    if (node->type() == AST_T::AsmPseudo_t) {
        regalloc_transfer_used_name_live_registers(static_cast<AsmPseudo*>(node)->name, next_instruction_index);
    }
}

static void regalloc_transfer_used_call_live_registers(AsmCall* node, size_t next_instruction_index) {
    FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[node->name]->type_t.get());
    GET_DFA_INSTRUCTION_SET_MASK(next_instruction_index, 0) |= fun_type->param_reg_mask;
}

static void regalloc_transfer_used_reg_live_registers(REGISTER_KIND register_kind, size_t next_instruction_index) {
    SET_DFA_INSTRUCTION_SET_AT(next_instruction_index, register_mask_bit(register_kind), true);
}

static void regalloc_transfer_updated_name_live_registers(TIdentifier name, size_t next_instruction_index) {
    if (frontend->symbol_table[name]->attrs->type() != AST_T::StaticAttr_t) {
        size_t i = context->control_flow_graph->identifier_id_map[name];
        SET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i, false);
    }
}

static void regalloc_transfer_updated_operand_live_registers(AsmOperand* node, size_t next_instruction_index) {
    if (node->type() == AST_T::AsmPseudo_t) {
        regalloc_transfer_updated_name_live_registers(static_cast<AsmPseudo*>(node)->name, next_instruction_index);
    }
}

static void regalloc_transfer_updated_reg_live_registers(REGISTER_KIND register_kind, size_t next_instruction_index) {
    SET_DFA_INSTRUCTION_SET_AT(next_instruction_index, register_mask_bit(register_kind), false);
}

static void regalloc_transfer_live_registers(AsmInstruction* node, size_t next_instruction_index) {
    switch (node->type()) {
        case AST_T::AsmMov_t: {
            AsmMov* p_node = static_cast<AsmMov*>(node);
            regalloc_transfer_updated_operand_live_registers(p_node->dst.get(), next_instruction_index);
            regalloc_transfer_used_operand_live_registers(p_node->src.get(), next_instruction_index);
            break;
        }
        case AST_T::AsmUnary_t: {
            AsmUnary* p_node = static_cast<AsmUnary*>(node);
            regalloc_transfer_updated_operand_live_registers(p_node->dst.get(), next_instruction_index);
            regalloc_transfer_used_operand_live_registers(p_node->dst.get(), next_instruction_index);
            break;
        }
        case AST_T::AsmBinary_t: {
            AsmBinary* p_node = static_cast<AsmBinary*>(node);
            regalloc_transfer_updated_operand_live_registers(p_node->dst.get(), next_instruction_index);
            regalloc_transfer_used_operand_live_registers(p_node->src.get(), next_instruction_index);
            regalloc_transfer_used_operand_live_registers(p_node->dst.get(), next_instruction_index);
            break;
        }
        case AST_T::AsmCmp_t: {
            AsmCmp* p_node = static_cast<AsmCmp*>(node);
            regalloc_transfer_used_operand_live_registers(p_node->src.get(), next_instruction_index);
            regalloc_transfer_used_operand_live_registers(p_node->dst.get(), next_instruction_index);
            break;
        }
        case AST_T::AsmIdiv_t:
            regalloc_transfer_updated_reg_live_registers(REGISTER_KIND::Ax, next_instruction_index);
            regalloc_transfer_updated_reg_live_registers(REGISTER_KIND::Dx, next_instruction_index);
            regalloc_transfer_used_operand_live_registers(
                static_cast<AsmIdiv*>(node)->src.get(), next_instruction_index);
            regalloc_transfer_used_reg_live_registers(REGISTER_KIND::Ax, next_instruction_index);
            regalloc_transfer_used_reg_live_registers(REGISTER_KIND::Dx, next_instruction_index);
            break;
        case AST_T::AsmCdq_t:
            regalloc_transfer_updated_reg_live_registers(REGISTER_KIND::Dx, next_instruction_index);
            regalloc_transfer_used_reg_live_registers(REGISTER_KIND::Ax, next_instruction_index);
            break;
        case AST_T::AsmSetCC_t:
            regalloc_transfer_updated_operand_live_registers(
                static_cast<AsmSetCC*>(node)->dst.get(), next_instruction_index);
            break;
        case AST_T::AsmPush_t:
            regalloc_transfer_used_operand_live_registers(
                static_cast<AsmPush*>(node)->src.get(), next_instruction_index);
            break;
        case AST_T::AsmCall_t:
            regalloc_transfer_updated_reg_live_registers(REGISTER_KIND::Ax, next_instruction_index);
            regalloc_transfer_updated_reg_live_registers(REGISTER_KIND::Cx, next_instruction_index);
            regalloc_transfer_updated_reg_live_registers(REGISTER_KIND::Dx, next_instruction_index);
            regalloc_transfer_updated_reg_live_registers(REGISTER_KIND::Di, next_instruction_index);
            regalloc_transfer_updated_reg_live_registers(REGISTER_KIND::Si, next_instruction_index);
            regalloc_transfer_updated_reg_live_registers(REGISTER_KIND::R8, next_instruction_index);
            regalloc_transfer_updated_reg_live_registers(REGISTER_KIND::R9, next_instruction_index);
            regalloc_transfer_used_call_live_registers(static_cast<AsmCall*>(node), next_instruction_index);
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static bool inference_graph_initialize() {
    control_flow_graph_initialize();
    if (!data_flow_analysis_initialize()) {
        return false;
    }
    data_flow_analysis_backward_iterative_algorithm();
    if (context->control_flow_graph->identifier_id_map.empty()) {
        return false;
    }

    if (context->unpruned_reg_nodes.size() < 12) {
        context->unpruned_reg_nodes.resize(12);
    }
    if (context->unpruned_sse_reg_nodes.size() < 14) {
        context->unpruned_sse_reg_nodes.resize(14);
    }
    for (size_t i = 0; i < 12; ++i) {
        context->unpruned_reg_nodes[i] = i;
        context->reg_inference_graph[i].is_pruned = false;
        context->reg_inference_graph[i].spill_cost = -1.;
        context->reg_inference_graph[i].color = 0;
        context->reg_inference_graph[i].linked_reg_mask = context->HARD_REG_MASK;
        context->reg_inference_graph[i].linked_pseudo_nodes.clear();

        context->unpruned_sse_reg_nodes[i] = i;
        context->sse_reg_inference_graph[i].is_pruned = false;
        context->sse_reg_inference_graph[i].spill_cost = -1.;
        context->sse_reg_inference_graph[i].color = 0;
        context->sse_reg_inference_graph[i].linked_reg_mask = context->HARD_SSE_REG_MASK;
        context->sse_reg_inference_graph[i].linked_pseudo_nodes.clear();
    }
    for (size_t i = 12; i < 14; ++i) {
        context->unpruned_sse_reg_nodes[i] = i;
        context->sse_reg_inference_graph[i].is_pruned = false;
        context->sse_reg_inference_graph[i].spill_cost = -1.;
        context->sse_reg_inference_graph[i].color = 0;
        context->sse_reg_inference_graph[i].linked_reg_mask = context->HARD_SSE_REG_MASK;
        context->sse_reg_inference_graph[i].linked_pseudo_nodes.clear();
    }

    context->unpruned_pseudo_nodes.clear();
    context->unpruned_sse_pseudo_nodes.clear();
    context->pseudo_inference_graph.clear();
    context->sse_pseudo_inference_graph.clear();
    for (const auto& name_id : context->control_flow_graph->identifier_id_map) {
        TIdentifier name = name_id.first;
        InferenceNode node = {REGISTER_KIND::Sp, false, 0., 0, MASK_FALSE, {}};
        if (frontend->symbol_table[name_id.first]->type_t->type() == AST_T::Double_t) {
            context->unpruned_sse_pseudo_nodes.push_back(name);
            context->sse_pseudo_inference_graph[name] = std::move(node);
        }
        else {
            context->unpruned_pseudo_nodes.push_back(name);
            context->pseudo_inference_graph[name] = std::move(node);
        }
    }

    // TODO add_edges
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register allocation

static void regalloc_inference_graph() {
    // TODO
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Register coalescing

static void coalesce_list_instructions() {
    // TODO
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void regalloc_function_top_level(AsmFunction* node) {
    context->p_instructions = &node->instructions;
    if (inference_graph_initialize()) {
        regalloc_inference_graph();
        if (context->is_with_coalescing) {
            coalesce_list_instructions();
        }
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
    {
        uint64_t hard_reg_mask = MASK_FALSE;
        {
            register_mask_set(hard_reg_mask, REGISTER_KIND::Ax, true);
            register_mask_set(hard_reg_mask, REGISTER_KIND::Bx, true);
            register_mask_set(hard_reg_mask, REGISTER_KIND::Cx, true);
            register_mask_set(hard_reg_mask, REGISTER_KIND::Dx, true);
            register_mask_set(hard_reg_mask, REGISTER_KIND::Di, true);
            register_mask_set(hard_reg_mask, REGISTER_KIND::Si, true);
            register_mask_set(hard_reg_mask, REGISTER_KIND::R8, true);
            register_mask_set(hard_reg_mask, REGISTER_KIND::R9, true);
            register_mask_set(hard_reg_mask, REGISTER_KIND::R12, true);
            register_mask_set(hard_reg_mask, REGISTER_KIND::R13, true);
            register_mask_set(hard_reg_mask, REGISTER_KIND::R14, true);
            register_mask_set(hard_reg_mask, REGISTER_KIND::R15, true);
        }
        uint64_t hard_sse_reg_mask = MASK_FALSE;
        {
            register_mask_set(hard_sse_reg_mask, REGISTER_KIND::Xmm0, true);
            register_mask_set(hard_sse_reg_mask, REGISTER_KIND::Xmm1, true);
            register_mask_set(hard_sse_reg_mask, REGISTER_KIND::Xmm2, true);
            register_mask_set(hard_sse_reg_mask, REGISTER_KIND::Xmm3, true);
            register_mask_set(hard_sse_reg_mask, REGISTER_KIND::Xmm4, true);
            register_mask_set(hard_sse_reg_mask, REGISTER_KIND::Xmm5, true);
            register_mask_set(hard_sse_reg_mask, REGISTER_KIND::Xmm6, true);
            register_mask_set(hard_sse_reg_mask, REGISTER_KIND::Xmm7, true);
            register_mask_set(hard_sse_reg_mask, REGISTER_KIND::Xmm8, true);
            register_mask_set(hard_sse_reg_mask, REGISTER_KIND::Xmm9, true);
            register_mask_set(hard_sse_reg_mask, REGISTER_KIND::Xmm10, true);
            register_mask_set(hard_sse_reg_mask, REGISTER_KIND::Xmm11, true);
            register_mask_set(hard_sse_reg_mask, REGISTER_KIND::Xmm12, true);
            register_mask_set(hard_sse_reg_mask, REGISTER_KIND::Xmm13, true);
        }
        context = std::make_unique<RegAllocContext>(
            std::move(optim_2_code), std::move(hard_reg_mask), std::move(hard_sse_reg_mask));
    }
    context->control_flow_graph = std::make_unique<ControlFlowGraph>();
    context->data_flow_analysis = std::make_unique<DataFlowAnalysis>();
    regalloc_program(node);
    context.reset();
}
