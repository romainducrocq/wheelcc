#include <memory>

#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/back_ast.hpp"
#include "ast/back_symt.hpp"
#include "ast/front_symt.hpp"

#include "backend/assembly/symt_cvt.hpp"

struct SymtCvtContext {
    TIdentifier symbol;
};

static std::unique_ptr<SymtCvtContext> context;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Symbol table conversion

static TInt generate_scalar_type_alignment(Type* type) {
    switch (type->type()) {
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

static TInt generate_array_aggregate_type_alignment(Array* arr_type, TLong& size) {
    size = arr_type->size;
    while (arr_type->elem_type->type() == AST_T::Array_t) {
        arr_type = static_cast<Array*>(arr_type->elem_type.get());
        size *= arr_type->size;
    }
    TInt alignment;
    {
        alignment = generate_type_alignment(arr_type->elem_type.get());
        if (arr_type->elem_type->type() == AST_T::Structure_t) {
            Structure* struct_type = static_cast<Structure*>(arr_type->elem_type.get());
            size *= frontend->struct_typedef_table[struct_type->tag]->size;
        }
        else {
            size *= alignment;
        }
        if (size >= 16l) {
            alignment = 16;
        }
    }
    return alignment;
}

static TInt generate_structure_aggregate_type_alignment(Structure* struct_type) {
    return frontend->struct_typedef_table[struct_type->tag]->alignment;
}

TInt generate_type_alignment(Type* type) {
    switch (type->type()) {
        case AST_T::Array_t: {
            TLong size;
            return generate_array_aggregate_type_alignment(static_cast<Array*>(type), size);
        }
        case AST_T::Structure_t:
            return generate_structure_aggregate_type_alignment(static_cast<Structure*>(type));
        default:
            return generate_scalar_type_alignment(type);
    }
}

static std::shared_ptr<ByteArray> convert_array_aggregate_assembly_type(Array* arr_type) {
    TLong size;
    TInt alignment = generate_array_aggregate_type_alignment(arr_type, size);
    return std::make_shared<ByteArray>(std::move(size), std::move(alignment));
}

static std::shared_ptr<ByteArray> convert_structure_aggregate_assembly_type(Structure* struct_type) {
    TLong size;
    TInt alignment;
    if (frontend->struct_typedef_table.find(struct_type->tag) != frontend->struct_typedef_table.end()) {
        size = frontend->struct_typedef_table[struct_type->tag]->size;
        alignment = frontend->struct_typedef_table[struct_type->tag]->alignment;
    }
    else {
        size = -1l;
        alignment = -1;
    }
    return std::make_shared<ByteArray>(std::move(size), std::move(alignment));
}

std::shared_ptr<AssemblyType> convert_backend_assembly_type(TIdentifier name) {
    switch (frontend->symbol_table[name]->type_t->type()) {
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
            return convert_array_aggregate_assembly_type(
                static_cast<Array*>(frontend->symbol_table[name]->type_t.get()));
        case AST_T::Structure_t:
            return convert_structure_aggregate_assembly_type(
                static_cast<Structure*>(frontend->symbol_table[name]->type_t.get()));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void convert_backend_symbol(std::unique_ptr<BackendSymbol>&& node) {
    backend->backend_symbol_table[context->symbol] = std::move(node);
}

static void convert_double_static_constant() {
    std::shared_ptr<AssemblyType> assembly_type = std::make_shared<BackendDouble>();
    convert_backend_symbol(std::make_unique<BackendObj>(true, true, std::move(assembly_type)));
}

static void convert_string_static_constant(Array* arr_type) {
    std::shared_ptr<AssemblyType> assembly_type = convert_array_aggregate_assembly_type(arr_type);
    convert_backend_symbol(std::make_unique<BackendObj>(true, true, std::move(assembly_type)));
}

static void convert_static_constant_top_level(AsmStaticConstant* node) {
    context->symbol = node->name;
    switch (node->static_init->type()) {
        case AST_T::DoubleInit_t:
            convert_double_static_constant();
            break;
        case AST_T::StringInit_t:
            convert_string_static_constant(static_cast<Array*>(frontend->symbol_table[node->name]->type_t.get()));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void convert_top_level(AsmTopLevel* node) {
    switch (node->type()) {
        case AST_T::AsmStaticConstant_t:
            convert_static_constant_top_level(static_cast<AsmStaticConstant*>(node));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void convert_fun_type(FunAttr* node, FunType* fun_type) {
    if (node->is_defined && fun_type->param_reg_mask == NULL_REGISTER_MASK) {
        RAISE_INTERNAL_ERROR;
    }
    TULong param_reg_mask = fun_type->param_reg_mask == NULL_REGISTER_MASK ? 0ul : fun_type->param_reg_mask;
    TULong ret_reg_mask = fun_type->ret_reg_mask == NULL_REGISTER_MASK ? 0ul : fun_type->ret_reg_mask;
    bool is_defined = node->is_defined;
    convert_backend_symbol(std::make_unique<BackendFun>(std::move(is_defined)));
}

static void convert_obj_type(IdentifierAttr* node) {
    if (node->type() != AST_T::ConstantAttr_t) {
        std::shared_ptr<AssemblyType> assembly_type = convert_backend_assembly_type(context->symbol);
        bool is_static = node->type() == AST_T::StaticAttr_t;
        convert_backend_symbol(std::make_unique<BackendObj>(std::move(is_static), false, std::move(assembly_type)));
    }
}

static void convert_program(AsmProgram* node) {
    backend->backend_symbol_table.reserve(frontend->symbol_table.size());
    for (const auto& symbol : frontend->symbol_table) {
        context->symbol = symbol.first;
        if (symbol.second->type_t->type() == AST_T::FunType_t) {
            convert_fun_type(
                static_cast<FunAttr*>(symbol.second->attrs.get()), static_cast<FunType*>(symbol.second->type_t.get()));
        }
        else {
            convert_obj_type(symbol.second->attrs.get());
        }
    }

    for (const auto& top_level : node->static_constant_top_levels) {
        convert_top_level(top_level.get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void convert_symbol_table(AsmProgram* node) {
    context = std::make_unique<SymtCvtContext>();
    convert_program(node);
    context.reset();
}
