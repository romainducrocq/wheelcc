#include <memory>

#include "util/throw.hpp"

#include "ast/back_ast.hpp"
#include "ast/back_symt.hpp"
#include "ast/front_symt.hpp"
#include "ast_t.hpp" // ast

#include "assembly/regs.hpp" // backend
#include "backend/assembly/symt_cvt.hpp"

struct SymtCvtContext {
    TIdentifier symbol;
};

static std::unique_ptr<SymtCvtContext> ctx;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Symbol table conversion

static TInt get_scalar_alignment(Type* type) {
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

static TInt get_arr_alignment(Array* arr_type, TLong& size) {
    size = arr_type->size;
    while (arr_type->elem_type->type() == AST_Array_t) {
        arr_type = static_cast<Array*>(arr_type->elem_type.get());
        size *= arr_type->size;
    }
    TInt alignment;
    {
        alignment = gen_type_alignment(arr_type->elem_type.get());
        if (arr_type->elem_type->type() == AST_Structure_t) {
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

static TInt get_struct_alignment(Structure* struct_type) {
    return frontend->struct_typedef_table[struct_type->tag]->alignment;
}

TInt gen_type_alignment(Type* type) {
    switch (type->type()) {
        case AST_Array_t: {
            TLong size;
            return get_arr_alignment(static_cast<Array*>(type), size);
        }
        case AST_Structure_t:
            return get_struct_alignment(static_cast<Structure*>(type));
        default:
            return get_scalar_alignment(type);
    }
}

static std::shared_ptr<ByteArray> arr_asm_type(Array* arr_type) {
    TLong size;
    TInt alignment = get_arr_alignment(arr_type, size);
    return std::make_shared<ByteArray>(std::move(size), std::move(alignment));
}

static std::shared_ptr<ByteArray> struct_asm_type(Structure* struct_type) {
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

std::shared_ptr<AssemblyType> cvt_backend_asm_type(TIdentifier name) {
    switch (frontend->symbol_table[name]->type_t->type()) {
        case AST_Char_t:
        case AST_SChar_t:
        case AST_UChar_t:
            return std::make_shared<Byte>();
        case AST_Int_t:
        case AST_UInt_t:
            return std::make_shared<LongWord>();
        case AST_Long_t:
        case AST_ULong_t:
        case AST_Pointer_t:
            return std::make_shared<QuadWord>();
        case AST_Double_t:
            return std::make_shared<BackendDouble>();
        case AST_Array_t:
            return arr_asm_type(static_cast<Array*>(frontend->symbol_table[name]->type_t.get()));
        case AST_Structure_t:
            return struct_asm_type(static_cast<Structure*>(frontend->symbol_table[name]->type_t.get()));
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void cvt_backend_symbol(std::unique_ptr<BackendSymbol>&& node) {
    backend->symbol_table[ctx->symbol] = std::move(node);
}

static void dbl_static_const() {
    std::shared_ptr<AssemblyType> asm_type = std::make_shared<BackendDouble>();
    cvt_backend_symbol(std::make_unique<BackendObj>(true, true, std::move(asm_type)));
}

static void string_static_const(Array* arr_type) {
    std::shared_ptr<AssemblyType> asm_type = arr_asm_type(arr_type);
    cvt_backend_symbol(std::make_unique<BackendObj>(true, true, std::move(asm_type)));
}

static void cvt_static_const_toplvl(AsmStaticConstant* node) {
    ctx->symbol = node->name;
    switch (node->static_init->type()) {
        case AST_DoubleInit_t:
            dbl_static_const();
            break;
        case AST_StringInit_t:
            string_static_const(static_cast<Array*>(frontend->symbol_table[node->name]->type_t.get()));
            break;
        default:
            RAISE_INTERNAL_ERROR;
    }
}

static void cvt_toplvl(AsmTopLevel* node) {
    if (node->type() == AST_AsmStaticConstant_t) {
        cvt_static_const_toplvl(static_cast<AsmStaticConstant*>(node));
    }
    else {
        RAISE_INTERNAL_ERROR;
    }
}

static void cvt_fun_type(FunAttr* node, FunType* fun_type) {
    if (fun_type->param_reg_mask == NULL_REGISTER_MASK) {
        ABORT_IF(node->is_def);
        fun_type->param_reg_mask = REGISTER_MASK_FALSE;
    }
    if (fun_type->ret_reg_mask == NULL_REGISTER_MASK) {
        fun_type->ret_reg_mask = REGISTER_MASK_FALSE;
    }
    bool is_def = node->is_def;
    cvt_backend_symbol(std::make_unique<BackendFun>(std::move(is_def)));
}

static void cvt_obj_type(IdentifierAttr* node) {
    if (node->type() != AST_ConstantAttr_t) {
        std::shared_ptr<AssemblyType> asm_type = cvt_backend_asm_type(ctx->symbol);
        bool is_static = node->type() == AST_StaticAttr_t;
        cvt_backend_symbol(std::make_unique<BackendObj>(std::move(is_static), false, std::move(asm_type)));
    }
}

static void cvt_program(AsmProgram* node) {
    backend->symbol_table.reserve(frontend->symbol_table.size());
    for (const auto& symbol : frontend->symbol_table) {
        ctx->symbol = symbol.first;
        if (symbol.second->type_t->type() == AST_FunType_t) {
            cvt_fun_type(
                static_cast<FunAttr*>(symbol.second->attrs.get()), static_cast<FunType*>(symbol.second->type_t.get()));
        }
        else {
            cvt_obj_type(symbol.second->attrs.get());
        }
    }

    for (const auto& top_level : node->static_const_toplvls) {
        cvt_toplvl(top_level.get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void convert_symbol_table(AsmProgram* node) {
    ctx = std::make_unique<SymtCvtContext>();
    cvt_program(node);
    ctx.reset();
}
