#include "assembly/st_conv.hpp"
#include "util/error.hpp"
#include "ast/ast.hpp"
#include "ast/symbol_table.hpp"
#include "ast/backend_st.hpp"
#include "ast/asm_ast.hpp"

#include <memory>

/**
cdef AssemblyType convert_backend_assembly_type(str name_str):
    if isinstance(symbol_table[name_str].type_t, (Int, UInt)):
        return LongWord()
    elif isinstance(symbol_table[name_str].type_t, Double):
        return BackendDouble()
    elif isinstance(symbol_table[name_str].type_t, (Long, ULong)):
        return QuadWord()
    else:

        raise RuntimeError(
            "An error occurred in backend symbol table conversion, not all nodes were visited")
*/
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

/**
cdef str symbol = ""
*/
static const TIdentifier* p_symbol;

/**
cdef void add_backend_symbol(BackendSymbol node):
    backend_symbol_table[symbol] = node
*/
static void add_backend_symbol(std::unique_ptr<BackendSymbol>&& node) {
    backend_symbol_table[*p_symbol] = std::move(node);
}

/**
cdef void convert_double_static_constant():
    cdef AssemblyType assembly_type = BackendDouble()
    cdef bint is_static = True
    cdef bint is_constant = True
    add_backend_symbol(BackendObj(assembly_type, is_static, is_constant))
*/
static void convert_double_static_constant() {
    bool is_static = true;
    bool is_constant = true;
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<BackendDouble>();
    add_backend_symbol(std::make_unique<BackendObj>(std::move(is_static), std::move(is_constant),
                                                          std::move(assembly_type)));
}

/**
cdef void convert_static_constant_top_level(AsmStaticConstant node):
    global symbol

    symbol = node.name.str_t
    if isinstance(node.initial_value, DoubleInit):
        convert_double_static_constant()
    else:

        raise RuntimeError(
            "An error occurred in backend symbol table conversion, not all nodes were visited")
*/
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

/**
cdef void convert_top_level(AsmTopLevel node):
    if isinstance(node, AsmStaticConstant):
        convert_static_constant_top_level(node)
    else:

        raise RuntimeError(
            "An error occurred in stack management, not all nodes were visited")
*/
static void convert_top_level(AsmTopLevel* node) {
    switch(node->type()) {
        case AST_T::AsmStaticConstant_t:
            convert_static_constant_top_level(static_cast<AsmStaticConstant*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

/**
cdef void convert_fun_type(FunAttr node):
    cdef bint is_defined = node.is_defined
    add_backend_symbol(BackendFun(is_defined))
*/
static void convert_fun_type(FunAttr* node) {
    bool is_defined = node->is_defined;
    add_backend_symbol(std::make_unique<BackendFun>(std::move(is_defined)));
}

/**
cdef void convert_obj_type(IdentifierAttr node):
    cdef AssemblyType assembly_type = convert_backend_assembly_type(symbol)
    cdef bint is_static = isinstance(node, StaticAttr)
    cdef bint is_constant = False
    add_backend_symbol(BackendObj(assembly_type, is_static, is_constant))
*/
static void convert_obj_type(IdentifierAttr* node) {
    std::shared_ptr<AssemblyType> assembly_type =  convert_backend_assembly_type(*p_symbol);
    bool is_static = node->type() == AST_T::StaticAttr_t;
    bool is_constant = false;
    add_backend_symbol(std::make_unique<BackendObj>(std::move(is_static), std::move(is_constant),
                                                          std::move(assembly_type)));
}

/**
cdef void convert_backend_symbol_table(AsmProgram node):
    global symbol

    cdef Py_ssize_t top_level
    for top_level in range(len(node.static_constant_top_levels)):
        convert_top_level(node.static_constant_top_levels[top_level])

    for symbol in symbol_table:
        if isinstance(symbol_table[symbol].type_t, FunType):
            convert_fun_type(symbol_table[symbol].attrs)
        else:
            convert_obj_type(symbol_table[symbol].attrs)
*/
static void convert_backend_symbol_table(AsmProgram* node) {
    for(size_t top_level = 0; top_level < node->static_constant_top_levels.size(); top_level++) {
        convert_top_level(node->static_constant_top_levels[top_level].get());
    }

    for(const auto& symbol: symbol_table) {
        p_symbol = &symbol.first;
        if(symbol.second->type() == AST_T::FunType_t) {
            convert_fun_type(static_cast<FunAttr*>(symbol.second->attrs.get()));
        }
        else {
            convert_obj_type(symbol.second->attrs.get());
        }
    }
    p_symbol = nullptr;
}

/**
cdef void convert_symbol_table(AsmProgram asm_ast):
    convert_backend_symbol_table(asm_ast)
*/
void convert_symbol_table(AsmProgram* node) {
    convert_backend_symbol_table(node);
}
