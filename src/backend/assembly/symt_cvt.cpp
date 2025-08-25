#include <memory>

#include "util/c_std.hpp"
#include "util/throw.hpp"

#include "ast/back_ast.hpp"
#include "ast/back_symt.hpp"
#include "ast/front_symt.hpp"
#include "ast_t.hpp" // ast

#include "assembly/regs.hpp" // backend
#include "backend/assembly/symt_cvt.hpp"

struct SymtCvtContext {
    BackEndContext* backend;
    FrontEndContext* frontend;
    // Symbol table conversion
    TIdentifier symbol;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Symbol table conversion

typedef SymtCvtContext* Ctx;

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
            THROW_ABORT;
    }
}

static TInt get_arr_alignment(FrontEndContext* ctx, Array* arr_type, TLong* size) {
    *size = arr_type->size;
    while (arr_type->elem_type->type() == AST_Array_t) {
        arr_type = static_cast<Array*>(arr_type->elem_type.get());
        *size *= arr_type->size;
    }
    TInt alignment;
    {
        alignment = gen_type_alignment(ctx, arr_type->elem_type.get());
        if (arr_type->elem_type->type() == AST_Structure_t) {
            Structure* struct_type = static_cast<Structure*>(arr_type->elem_type.get());
            *size *= map_get(ctx->struct_typedef_table, struct_type->tag)->size;
        }
        else {
            *size *= alignment;
        }
        if (*size >= 16l) {
            alignment = 16;
        }
    }
    return alignment;
}

static TInt get_struct_alignment(FrontEndContext* ctx, Structure* struct_type) {
    return map_get(ctx->struct_typedef_table, struct_type->tag)->alignment;
}

TInt gen_type_alignment(FrontEndContext* ctx, Type* type) {
    switch (type->type()) {
        case AST_Array_t: {
            TLong size;
            return get_arr_alignment(ctx, static_cast<Array*>(type), &size);
        }
        case AST_Structure_t:
            return get_struct_alignment(ctx, static_cast<Structure*>(type));
        default:
            return get_scalar_alignment(type);
    }
}

static std::shared_ptr<ByteArray> arr_asm_type(FrontEndContext* ctx, Array* arr_type) {
    TLong size;
    TInt alignment = get_arr_alignment(ctx, arr_type, &size);
    return std::make_shared<ByteArray>(size, alignment);
}

static std::shared_ptr<ByteArray> struct_asm_type(FrontEndContext* ctx, Structure* struct_type) {
    TLong size;
    TInt alignment;
    if (map_find(ctx->struct_typedef_table, struct_type->tag) != map_end()) {
        size = map_get(ctx->struct_typedef_table, struct_type->tag)->size;
        alignment = map_get(ctx->struct_typedef_table, struct_type->tag)->alignment;
    }
    else {
        size = -1l;
        alignment = -1;
    }
    return std::make_shared<ByteArray>(size, alignment);
}

std::shared_ptr<AssemblyType> cvt_backend_asm_type(FrontEndContext* ctx, TIdentifier name) {
    switch (map_get(ctx->symbol_table, name)->type_t->type()) {
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
            return arr_asm_type(ctx, static_cast<Array*>(map_get(ctx->symbol_table, name)->type_t.get()));
        case AST_Structure_t:
            return struct_asm_type(ctx, static_cast<Structure*>(map_get(ctx->symbol_table, name)->type_t.get()));
        default:
            THROW_ABORT;
    }
}

static void cvt_backend_symbol(Ctx ctx, std::unique_ptr<BackendSymbol>&& node) {
    map_move_add(ctx->backend->symbol_table, ctx->symbol, node);
}

static void dbl_static_const(Ctx ctx) {
    std::shared_ptr<AssemblyType> asm_type = std::make_shared<BackendDouble>();
    cvt_backend_symbol(ctx, std::make_unique<BackendObj>(true, true, std::move(asm_type)));
}

static void string_static_const(Ctx ctx, Array* arr_type) {
    std::shared_ptr<AssemblyType> asm_type = arr_asm_type(ctx->frontend, arr_type);
    cvt_backend_symbol(ctx, std::make_unique<BackendObj>(true, true, std::move(asm_type)));
}

static void cvt_static_const_toplvl(Ctx ctx, AsmStaticConstant* node) {
    ctx->symbol = node->name;
    switch (node->static_init->type()) {
        case AST_DoubleInit_t:
            dbl_static_const(ctx);
            break;
        case AST_StringInit_t:
            string_static_const(
                ctx, static_cast<Array*>(map_get(ctx->frontend->symbol_table, node->name)->type_t.get()));
            break;
        default:
            THROW_ABORT;
    }
}

static void cvt_toplvl(Ctx ctx, AsmTopLevel* node) {
    if (node->type() == AST_AsmStaticConstant_t) {
        cvt_static_const_toplvl(ctx, static_cast<AsmStaticConstant*>(node));
    }
    else {
        THROW_ABORT;
    }
}

static void cvt_fun_type(Ctx ctx, FunAttr* node, FunType* fun_type) {
    if (fun_type->param_reg_mask == NULL_REGISTER_MASK) {
        THROW_ABORT_IF(node->is_def);
        fun_type->param_reg_mask = REGISTER_MASK_FALSE;
    }
    if (fun_type->ret_reg_mask == NULL_REGISTER_MASK) {
        fun_type->ret_reg_mask = REGISTER_MASK_FALSE;
    }
    bool is_def = node->is_def;
    cvt_backend_symbol(ctx, std::make_unique<BackendFun>(is_def));
}

static void cvt_obj_type(Ctx ctx, IdentifierAttr* node) {
    if (node->type() != AST_ConstantAttr_t) {
        std::shared_ptr<AssemblyType> asm_type = cvt_backend_asm_type(ctx->frontend, ctx->symbol);
        bool is_static = node->type() == AST_StaticAttr_t;
        cvt_backend_symbol(ctx, std::make_unique<BackendObj>(is_static, false, std::move(asm_type)));
    }
}

static void cvt_program(Ctx ctx, AsmProgram* node) {
    for (size_t i = 0; i < map_size(ctx->frontend->symbol_table); ++i) {
        const pair_t(TIdentifier, UPtrSymbol)* symbol = &ctx->frontend->symbol_table[i];
        ctx->symbol = pair_first(*symbol);
        if (pair_second(*symbol)->type_t->type() == AST_FunType_t) {
            cvt_fun_type(ctx, static_cast<FunAttr*>(pair_second(*symbol)->attrs.get()),
                static_cast<FunType*>(pair_second(*symbol)->type_t.get()));
        }
        else {
            cvt_obj_type(ctx, pair_second(*symbol)->attrs.get());
        }
    }

    for (size_t i = 0; i < vec_size(node->static_const_toplvls); ++i) {
        cvt_toplvl(ctx, node->static_const_toplvls[i].get());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void convert_symbol_table(AsmProgram* node, BackEndContext* backend, FrontEndContext* frontend) {
    SymtCvtContext ctx;
    {
        ctx.backend = backend;
        ctx.frontend = frontend;
    }
    cvt_program(&ctx, node);
}
