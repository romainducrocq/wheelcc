#include "assembly/st_conv.hpp"
#include "util/error.hpp"
#include "ast/ast.hpp"
#include "ast/symbol_table.hpp"
#include "ast/backend_st.hpp"
#include "ast/asm_ast.hpp"

#include <memory>

std::shared_ptr<AssemblyType> convert_backend_assembly_type(const TIdentifier& name) {
    switch(symbol_table[name]->type_t->type()) {
        case AST_T::Int_t:
        case AST_T::UInt_t:
            return std::make_shared<LongWord>();
        case AST_T::Double_t:
            return std::make_shared<BackendDouble>();
        case AST_T::Long_t:
        case AST_T::ULong_t:
            return std::make_shared<QuadWord>();
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static const TIdentifier* p_symbol;

static void add_backend_symbol(std::unique_ptr<BackendSymbol>&& node) {
    backend_symbol_table[*p_symbol] = std::move(node);
}

static void convert_double_static_constant() {
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<BackendDouble>();
    add_backend_symbol(std::make_unique<BackendObj>(true, true, std::move(assembly_type)));
}

static void convert_static_constant_top_level(AsmStaticConstant* node) {
    p_symbol = &node->name;
    switch(node->initial_value->type()) {
        case AST_T::DoubleInit_t:
            convert_double_static_constant();
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void convert_top_level(AsmTopLevel* node) {
    switch(node->type()) {
        case AST_T::AsmStaticConstant_t:
            convert_static_constant_top_level(static_cast<AsmStaticConstant*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void convert_fun_type(FunAttr* node) {
    bool is_defined = node->is_defined;
    add_backend_symbol(std::make_unique<BackendFun>(std::move(is_defined)));
}

static void convert_obj_type(IdentifierAttr* node) {
    std::shared_ptr<AssemblyType> assembly_type =  convert_backend_assembly_type(*p_symbol);
    bool is_static = node->type() == AST_T::StaticAttr_t;
    add_backend_symbol(std::make_unique<BackendObj>(std::move(is_static), false,
                                                          std::move(assembly_type)));
}

static void convert_program(AsmProgram* node) {
    for(size_t top_level = 0; top_level < node->static_constant_top_levels.size(); top_level++) {
        convert_top_level(node->static_constant_top_levels[top_level].get());
    }

    for(const auto& symbol: symbol_table) {
        p_symbol = &symbol.first;
        if(symbol.second->type_t->type() == AST_T::FunType_t) {
            convert_fun_type(static_cast<FunAttr*>(symbol.second->attrs.get()));
        }
        else {
            convert_obj_type(symbol.second->attrs.get());
        }
    }
    p_symbol = nullptr;
}

void convert_symbol_table(AsmProgram* node) {
    convert_program(node);
}
