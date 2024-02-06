#include "assembly/st_conv.hpp"
#include "util/error.hpp"
#include "ast/ast.hpp"
#include "ast/symbol_table.hpp"
#include "ast/backend_st.hpp"
#include "ast/asm_ast.hpp"

#include <memory>
#include <vector>

/**
cdef static_constant_top_levels = []
*/
std::vector<std::unique_ptr<AsmTopLevel>> static_constant_top_levels;

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
            raise_internal_error("An error occurred in backend symbol table conversion, "
                                 "not all nodes were visited");
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
    add_backend_symbol(std::make_unique<BackendObj>(is_static, is_constant, std::move(assembly_type)));
}

/**
cdef void convert_static_constant_top_levels():
    global symbol

    cdef int static_constant
    for static_constant in range(len(static_constant_top_levels)):
        symbol = static_constant_top_levels[static_constant].name.str_t
        if isinstance(static_constant_top_levels[static_constant].initial_value, DoubleInit):
            convert_double_static_constant()
        else:

            raise RuntimeError(
                "An error occurred in backend symbol table conversion, not all nodes were visited")
*/
static void convert_static_constant_top_levels() {
    for(size_t static_constant = 0; static_constant < static_constant_top_levels.size(); static_constant++) {
        AsmStaticConstant* p_node = static_cast<AsmStaticConstant*>(static_constant_top_levels[static_constant].get());
        p_symbol = &p_node->name;
        switch(p_node->initial_value->type()) {
            case AST_T::DoubleInit_t:
                convert_double_static_constant();
                break;
            default:
                raise_internal_error("An error occurred in backend symbol table conversion, "
                                     "not all nodes were visited");
        }
    }
    p_symbol = nullptr;
}

/**
cdef void convert_fun_type(FunAttr node):
    cdef bint is_defined = node.is_defined
    add_backend_symbol(BackendFun(is_defined))
*/
static void convert_fun_type(FunAttr* node) {
    bool is_defined = node->is_defined;
    add_backend_symbol(std::make_unique<BackendFun>(is_defined));
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
    add_backend_symbol(std::make_unique<BackendObj>(is_static, is_constant, std::move(assembly_type)));
}

/**
cdef void convert_symbol_table():
    global symbol

    convert_static_constant_top_levels()

    for symbol in symbol_table:
        if isinstance(symbol_table[symbol].type_t, FunType):
            convert_fun_type(symbol_table[symbol].attrs)
        else:
            convert_obj_type(symbol_table[symbol].attrs)
*/
void convert_symbol_table() {
    convert_static_constant_top_levels();
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
