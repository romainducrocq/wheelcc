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

struct InferenceRegister {
    REGISTER_KIND register_kind;
    size_t color;
    size_t degree;
    size_t spill_cost;
    uint64_t linked_hard_mask;
    std::vector<TIdentifier> linked_pseudo_names;
};

struct InferenceGraph {
    size_t k;
    size_t offset;
    uint64_t hard_reg_mask;
    std::array<InferenceRegister, 14> hard_registers;
    std::vector<size_t> unpruned_hard_mask_bits;
    std::vector<TIdentifier> unpruned_pseudo_names;
    std::unordered_map<TIdentifier, InferenceRegister> pseudo_register_map;
};

struct RegAllocContext {
    RegAllocContext(uint8_t optim_2_code);

    // Register allocation
    std::unique_ptr<ControlFlowGraph> control_flow_graph;
    std::unique_ptr<DataFlowAnalysis> data_flow_analysis;
    std::unique_ptr<InferenceGraph> inference_graph;
    std::unique_ptr<InferenceGraph> sse_inference_graph;
    InferenceGraph* p_inference_graph;
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

static void inference_graph_transfer_used_name_live_registers(TIdentifier name, size_t next_instruction_index) {
    if (frontend->symbol_table[name]->attrs->type() != AST_T::StaticAttr_t) {
        size_t i = context->control_flow_graph->identifier_id_map[name];
        SET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i, true);
    }
}

static void inference_graph_transfer_used_operand_live_registers(AsmOperand* node, size_t next_instruction_index) {
    if (node->type() == AST_T::AsmPseudo_t) {
        inference_graph_transfer_used_name_live_registers(static_cast<AsmPseudo*>(node)->name, next_instruction_index);
    }
}

static void inference_graph_transfer_used_call_live_registers(AsmCall* node, size_t next_instruction_index) {
    FunType* fun_type = static_cast<FunType*>(frontend->symbol_table[node->name]->type_t.get());
    GET_DFA_INSTRUCTION_SET_MASK(next_instruction_index, 0) |= fun_type->param_reg_mask;
}

static void inference_graph_transfer_used_reg_live_registers(
    REGISTER_KIND register_kind, size_t next_instruction_index) {
    SET_DFA_INSTRUCTION_SET_AT(next_instruction_index, register_mask_bit(register_kind), true);
}

static void inference_graph_transfer_updated_name_live_registers(TIdentifier name, size_t next_instruction_index) {
    if (frontend->symbol_table[name]->attrs->type() != AST_T::StaticAttr_t) {
        size_t i = context->control_flow_graph->identifier_id_map[name];
        SET_DFA_INSTRUCTION_SET_AT(next_instruction_index, i, false);
    }
}

static void inference_graph_transfer_updated_operand_live_registers(AsmOperand* node, size_t next_instruction_index) {
    if (node->type() == AST_T::AsmPseudo_t) {
        inference_graph_transfer_updated_name_live_registers(
            static_cast<AsmPseudo*>(node)->name, next_instruction_index);
    }
}

static void inference_graph_transfer_updated_reg_live_registers(
    REGISTER_KIND register_kind, size_t next_instruction_index) {
    SET_DFA_INSTRUCTION_SET_AT(next_instruction_index, register_mask_bit(register_kind), false);
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
        case AST_T::AsmUnary_t: {
            AsmUnary* p_node = static_cast<AsmUnary*>(node);
            inference_graph_transfer_updated_operand_live_registers(p_node->dst.get(), next_instruction_index);
            inference_graph_transfer_used_operand_live_registers(p_node->dst.get(), next_instruction_index);
            break;
        }
        case AST_T::AsmBinary_t: {
            AsmBinary* p_node = static_cast<AsmBinary*>(node);
            inference_graph_transfer_updated_operand_live_registers(p_node->dst.get(), next_instruction_index);
            inference_graph_transfer_used_operand_live_registers(p_node->src.get(), next_instruction_index);
            inference_graph_transfer_used_operand_live_registers(p_node->dst.get(), next_instruction_index);
            break;
        }
        case AST_T::AsmCmp_t: {
            AsmCmp* p_node = static_cast<AsmCmp*>(node);
            inference_graph_transfer_used_operand_live_registers(p_node->src.get(), next_instruction_index);
            inference_graph_transfer_used_operand_live_registers(p_node->dst.get(), next_instruction_index);
            break;
        }
        case AST_T::AsmIdiv_t:
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::Ax, next_instruction_index);
            inference_graph_transfer_updated_reg_live_registers(REGISTER_KIND::Dx, next_instruction_index);
            inference_graph_transfer_used_operand_live_registers(
                static_cast<AsmIdiv*>(node)->src.get(), next_instruction_index);
            inference_graph_transfer_used_reg_live_registers(REGISTER_KIND::Ax, next_instruction_index);
            inference_graph_transfer_used_reg_live_registers(REGISTER_KIND::Dx, next_instruction_index);
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
        size_t i = register_mask_bit(register_kind) - context->p_inference_graph->offset;
        InferenceRegister& infer = context->p_inference_graph->hard_registers[i];
        if (std::find(infer.linked_pseudo_names.begin(), infer.linked_pseudo_names.end(), name)
            == infer.linked_pseudo_names.end()) {
            infer.linked_pseudo_names.push_back(name);
            infer.degree++;
        }
    }
}

static void inference_graph_initialize_used_name_edges(TIdentifier name) {
    inference_graph_set_p(frontend->symbol_table[name]->type_t->type() == AST_T::Double_t);
    context->p_inference_graph->pseudo_register_map[name].spill_cost++;
}

static void inference_graph_initialize_used_operand_edges(AsmOperand* node) {
    if (node->type() == AST_T::AsmPseudo_t) {
        inference_graph_initialize_used_name_edges(static_cast<AsmPseudo*>(node)->name);
    }
}

static void inference_graph_initialize_updated_name_edges(TIdentifier name, size_t instruction_index, bool is_mov) {
    bool is_double = frontend->symbol_table[name]->type_t->type() == AST_T::Double_t;
    inference_graph_set_p(is_double);
    context->p_inference_graph->pseudo_register_map[name].spill_cost++;

    size_t mov_mask_bit = REGISTER_MASK_SIZE;
    if (is_mov) {
        if (GET_INSTRUCTION(instruction_index)->type() != AST_T::AsmMov_t) {
            RAISE_INTERNAL_ERROR;
        }
        AsmMov* mov = static_cast<AsmMov*>(GET_INSTRUCTION(instruction_index).get());
        switch (mov->src->type()) {
            case AST_T::AsmRegister_t: {
                REGISTER_KIND src_register_kind = register_mask_kind(static_cast<AsmRegister*>(mov->src.get()));
                mov_mask_bit = register_mask_bit(src_register_kind);
                break;
            }
            case AST_T::AsmPseudo_t: {
                TIdentifier src_name = static_cast<AsmPseudo*>(mov->src.get())->name;
                mov_mask_bit = context->control_flow_graph->identifier_id_map[src_name];
                break;
            }
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
                size_t j = i - context->p_inference_graph->offset;
                REGISTER_KIND register_kind = context->p_inference_graph->hard_registers[j].register_kind;
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

static void inference_graph_initialize_updated_operand_edges(AsmOperand* node, size_t instruction_index, bool is_mov) {
    if (node->type() == AST_T::AsmPseudo_t) {
        inference_graph_initialize_updated_name_edges(static_cast<AsmPseudo*>(node)->name, instruction_index, is_mov);
    }
}

static void inference_graph_initialize_updated_regs_edges(
    REGISTER_KIND* register_kinds, size_t instruction_index, size_t register_kinds_size, bool is_double) {
    inference_graph_set_p(is_double);

    if (GET_DFA_INSTRUCTION_SET_MASK(instruction_index, 0) != MASK_FALSE) {
        for (size_t i = context->data_flow_analysis->set_size < 64 ? context->data_flow_analysis->set_size : 64;
             i-- > REGISTER_MASK_SIZE;) {
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
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
            if (GET_DFA_INSTRUCTION_SET_AT(instruction_index, i)) {
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

static void inference_graph_initialize_edges(size_t instruction_index) {
    AsmInstruction* node = GET_INSTRUCTION(instruction_index).get();
    switch (node->type()) {
        case AST_T::AsmMov_t:
            // | Mov(src, dst) -> updated = [dst]
            // std::shared_ptr<AsmOperand> src;
            // std::shared_ptr<AsmOperand> dst;
            inference_graph_initialize_updated_operand_edges(
                static_cast<AsmMov*>(node)->dst.get(), instruction_index, true);
            break;
        case AST_T::AsmUnary_t:
            // | Unary(op, dst) -> updated = [dst]
            // std::shared_ptr<AsmOperand> dst;
            inference_graph_initialize_updated_operand_edges(
                static_cast<AsmUnary*>(node)->dst.get(), instruction_index, false);
            break;
        case AST_T::AsmBinary_t:
            // | Binary(op, src, dst) -> updated = [dst]
            // std::shared_ptr<AsmOperand> src;
            // std::shared_ptr<AsmOperand> dst;
            inference_graph_initialize_updated_operand_edges(
                static_cast<AsmBinary*>(node)->dst.get(), instruction_index, false);
            break;
        case AST_T::AsmIdiv_t: {
            // | Idiv(divisor)->updated = [ Reg(AX), Reg(DX) ]
            // std::shared_ptr<AsmOperand> src;
            REGISTER_KIND register_kinds[2] = {REGISTER_KIND::Ax, REGISTER_KIND::Dx};
            inference_graph_initialize_updated_regs_edges(register_kinds, instruction_index, 2, false);
            break;
        }
        case AST_T::AsmCdq_t: {
            // | Cdq->updated = [Reg(DX)]
            REGISTER_KIND register_kinds[1] = {REGISTER_KIND::Dx};
            inference_graph_initialize_updated_regs_edges(register_kinds, instruction_index, 1, false);
            break;
        }
        case AST_T::AsmSetCC_t:
            // | SetCC(cond, dst)->updated = [dst]
            // std::shared_ptr<AsmOperand> dst;
            inference_graph_initialize_updated_operand_edges(
                static_cast<AsmSetCC*>(node)->dst.get(), instruction_index, false);
            break;
        case AST_T::AsmCall_t: {
            // | Call(f)->updated = [ Reg(DI), Reg(SI), Reg(DX), Reg(CX), Reg(R8), Reg(R9), Reg(AX) ]
            REGISTER_KIND register_kinds[7] = {REGISTER_KIND::Ax, REGISTER_KIND::Cx, REGISTER_KIND::Dx,
                REGISTER_KIND::Di, REGISTER_KIND::Si, REGISTER_KIND::R8, REGISTER_KIND::R9};
            inference_graph_initialize_updated_regs_edges(register_kinds, instruction_index, 7, false);
            break;
        }
        case AST_T::AsmCmp_t:
            // | Cmp(v1, v2)-> updated = []
            // std::shared_ptr<AsmOperand> src;
            // std::shared_ptr<AsmOperand> dst;
        case AST_T::AsmPush_t:
            // | Push(v)->updated = []
            // std::shared_ptr<AsmOperand> src;
            break;
        default:
            break;
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

    context->inference_graph->unpruned_pseudo_names.clear();
    context->inference_graph->pseudo_register_map.clear();
    context->sse_inference_graph->unpruned_pseudo_names.clear();
    context->sse_inference_graph->pseudo_register_map.clear();
    for (const auto& name_id : context->control_flow_graph->identifier_id_map) {
        TIdentifier name = name_id.first;
        InferenceRegister infer = {REGISTER_KIND::Sp, 0, 0, 0, MASK_FALSE, {}};
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

        uint64_t hard_reg_mask = context->inference_graph->hard_reg_mask;
        for (size_t i = 0; i < 12; ++i) {
            context->inference_graph->unpruned_hard_mask_bits[i] = i;
            context->inference_graph->hard_registers[i].color = 0;
            context->inference_graph->hard_registers[i].degree = 12;
            context->inference_graph->hard_registers[i].spill_cost = 0;
            context->inference_graph->hard_registers[i].linked_hard_mask = hard_reg_mask;
            context->inference_graph->hard_registers[i].linked_pseudo_names.clear();
        }
    }
    if (!context->sse_inference_graph->pseudo_register_map.empty()) {
        if (context->sse_inference_graph->unpruned_hard_mask_bits.size() < 14) {
            context->sse_inference_graph->unpruned_hard_mask_bits.resize(14);
        }

        uint64_t hard_reg_mask = context->sse_inference_graph->hard_reg_mask;
        for (size_t i = 0; i < 14; ++i) {
            context->sse_inference_graph->unpruned_hard_mask_bits[i] = i;
            context->sse_inference_graph->hard_registers[i].color = 0;
            context->sse_inference_graph->hard_registers[i].degree = 14;
            context->sse_inference_graph->hard_registers[i].spill_cost = 0;
            context->sse_inference_graph->hard_registers[i].linked_hard_mask = hard_reg_mask;
            context->sse_inference_graph->hard_registers[i].linked_pseudo_names.clear();
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
    context = std::make_unique<RegAllocContext>(std::move(optim_2_code));
    context->control_flow_graph = std::make_unique<ControlFlowGraph>();
    context->data_flow_analysis = std::make_unique<DataFlowAnalysis>();
    context->inference_graph = std::make_unique<InferenceGraph>();
    {
        context->inference_graph->k = 12;
        context->inference_graph->offset = 0;

        context->inference_graph->hard_reg_mask = MASK_FALSE;
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

        context->inference_graph->hard_registers[0].register_kind = REGISTER_KIND::Ax;
        context->inference_graph->hard_registers[1].register_kind = REGISTER_KIND::Bx;
        context->inference_graph->hard_registers[2].register_kind = REGISTER_KIND::Cx;
        context->inference_graph->hard_registers[3].register_kind = REGISTER_KIND::Dx;
        context->inference_graph->hard_registers[4].register_kind = REGISTER_KIND::Di;
        context->inference_graph->hard_registers[5].register_kind = REGISTER_KIND::Si;
        context->inference_graph->hard_registers[6].register_kind = REGISTER_KIND::R8;
        context->inference_graph->hard_registers[7].register_kind = REGISTER_KIND::R9;
        context->inference_graph->hard_registers[8].register_kind = REGISTER_KIND::R12;
        context->inference_graph->hard_registers[9].register_kind = REGISTER_KIND::R13;
        context->inference_graph->hard_registers[10].register_kind = REGISTER_KIND::R14;
        context->inference_graph->hard_registers[11].register_kind = REGISTER_KIND::R15;
    }
    context->sse_inference_graph = std::make_unique<InferenceGraph>();
    {
        context->sse_inference_graph->k = 14;
        context->sse_inference_graph->offset = 12;

        context->sse_inference_graph->hard_reg_mask = MASK_FALSE;
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

        context->sse_inference_graph->hard_registers[0].register_kind = REGISTER_KIND::Xmm0;
        context->sse_inference_graph->hard_registers[1].register_kind = REGISTER_KIND::Xmm1;
        context->sse_inference_graph->hard_registers[2].register_kind = REGISTER_KIND::Xmm2;
        context->sse_inference_graph->hard_registers[3].register_kind = REGISTER_KIND::Xmm3;
        context->sse_inference_graph->hard_registers[4].register_kind = REGISTER_KIND::Xmm4;
        context->sse_inference_graph->hard_registers[5].register_kind = REGISTER_KIND::Xmm5;
        context->sse_inference_graph->hard_registers[6].register_kind = REGISTER_KIND::Xmm6;
        context->sse_inference_graph->hard_registers[7].register_kind = REGISTER_KIND::Xmm7;
        context->sse_inference_graph->hard_registers[8].register_kind = REGISTER_KIND::Xmm8;
        context->sse_inference_graph->hard_registers[9].register_kind = REGISTER_KIND::Xmm9;
        context->sse_inference_graph->hard_registers[10].register_kind = REGISTER_KIND::Xmm10;
        context->sse_inference_graph->hard_registers[11].register_kind = REGISTER_KIND::Xmm11;
        context->sse_inference_graph->hard_registers[12].register_kind = REGISTER_KIND::Xmm12;
        context->sse_inference_graph->hard_registers[13].register_kind = REGISTER_KIND::Xmm13;
    }
    regalloc_program(node);
    context.reset();
}
