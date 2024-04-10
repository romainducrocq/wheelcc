#include "backend/assembly/symt_cvt.hpp"
#include "util/error.hpp"
#include "ast/ast.hpp"
#include "ast/front_symt.hpp"
#include "ast/back_symt.hpp"
#include "ast/back_ast.hpp"

#include <memory>

TInt generate_scalar_type_alignment(Type* type_1) {
    switch(type_1->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::UChar_t:
            return 1;
        case AST_T::Int_t:
        case AST_T::UInt_t:
            return 4;
        case AST_T::Long_t:
        case AST_T::Double_t:
        case AST_T::ULong_t:
        case AST_T::Pointer_t:
            return 8;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

TInt generate_type_alignment(Type* type_1);

static TInt generate_array_aggregate_type_alignment(Array* arr_type, TLong& size) {
    size = arr_type->size;
    while(arr_type->elem_type->type() == AST_T::Array_t) {
        arr_type = static_cast<Array*>(arr_type->elem_type.get());
        size *= arr_type->size;
    }
    TInt alignment;
    {
        alignment = generate_type_alignment(arr_type->elem_type.get());
        size *= alignment;
        if(size >= 16l) {
            alignment = 16;
        }
    }
    return alignment;
}

static TInt generate_array_aggregate_type_alignment(Array* arr_type) {
    TLong size;
    return generate_array_aggregate_type_alignment(arr_type, size);
}

TInt generate_type_alignment(Type* type_1) {
    switch(type_1->type()) {
        case AST_T::Array_t:
            return generate_array_aggregate_type_alignment(static_cast<Array*>(type_1));
        default:
            return generate_scalar_type_alignment(type_1);
    }
}

static std::shared_ptr<ByteArray> convert_array_aggregate_assembly_type(Array* arr_type) {
    TLong size;
    TInt alignment = generate_array_aggregate_type_alignment(arr_type, size);
    return std::make_shared<ByteArray>(std::move(size), std::move(alignment));
}

std::shared_ptr<AssemblyType> convert_backend_assembly_type(const TIdentifier& name) {
    switch(symbol_table[name]->type_t->type()) {
        case AST_T::Char_t:
        case AST_T::SChar_t:
        case AST_T::UChar_t:
            return std::make_shared<Byte>();
        case AST_T::Int_t:
        case AST_T::UInt_t:
            return std::make_shared<LongWord>();
        case AST_T::Long_t:
        case AST_T::ULong_t:
        case AST_T::Pointer_t:
            return std::make_shared<QuadWord>();
        case AST_T::Double_t:
            return std::make_shared<BackendDouble>();
        case AST_T::Array_t:
            return convert_array_aggregate_assembly_type(static_cast<Array*>(symbol_table[name]->type_t.get()));
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

static void convert_string_static_constant(Array* arr_type) {
    std::shared_ptr<AssemblyType> assembly_type = convert_array_aggregate_assembly_type(arr_type);
    add_backend_symbol(std::make_unique<BackendObj>(true, true, std::move(assembly_type)));
}

static void convert_static_constant_top_level(AsmStaticConstant* node) {
    p_symbol = &node->name;
    switch(node->static_init->type()) {
        case AST_T::DoubleInit_t:
            convert_double_static_constant();
            break;
        case AST_T::StringInit_t:
            convert_string_static_constant(static_cast<Array*>(symbol_table[node->name]->type_t.get()));
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
    if(node->type() != AST_T::ConstantAttr_t) {
        std::shared_ptr<AssemblyType> assembly_type = convert_backend_assembly_type(*p_symbol);
        bool is_static = node->type() == AST_T::StaticAttr_t;
        add_backend_symbol(std::make_unique<BackendObj>(std::move(is_static), false,
                                                              std::move(assembly_type)));
    }
}

static void convert_program(AsmProgram* node) {
    for(const auto& symbol: symbol_table) {
        p_symbol = &symbol.first;
        if(symbol.second->type_t->type() == AST_T::FunType_t) {
            convert_fun_type(static_cast<FunAttr*>(symbol.second->attrs.get()));
        }
        else {
            convert_obj_type(symbol.second->attrs.get());
        }
    }

    for(size_t top_level = 0; top_level < node->static_constant_top_levels.size(); top_level++) {
        convert_top_level(node->static_constant_top_levels[top_level].get());
    }
    p_symbol = nullptr;
    symbol_table.clear();
}

void convert_symbol_table(AsmProgram* node) {
    convert_program(node);
}
