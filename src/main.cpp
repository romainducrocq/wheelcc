#ifndef __NDEBUG__
#include "util/pprint.hpp"
#endif
#include <inttypes.h>
#include <memory>
#include <stdio.h>

#include "util/c_std.hpp"
#include "util/fileio.hpp"
#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/back_ast.hpp"
#include "ast/back_symt.hpp"
#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"
#include "ast/interm_ast.hpp"

#include "frontend/parser/errors.hpp"
#include "frontend/parser/lexer.hpp"
#include "frontend/parser/parser.hpp"

#include "frontend/intermediate/semantic.hpp"
#include "frontend/intermediate/tac_repr.hpp"

#include "backend/assembly/asm_gen.hpp"
#include "backend/assembly/stack_fix.hpp"
#include "backend/assembly/symt_cvt.hpp"

#include "backend/emitter/gas_code.hpp"

#include "optimization/optim_tac.hpp"
#include "optimization/reg_alloc.hpp"

struct MainContext {
    ErrorsContext* errors;
#ifndef __NDEBUG__
    BackEndContext* backend;
    FrontEndContext* frontend;
    IdentifierContext* identifiers;
#endif
    // Main
    bool is_verbose;
    uint8_t debug_code;
    uint8_t optim_1_mask;
    uint8_t optim_2_code;
    string_t filename;
    vector_t(const char*) includedirs;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Main

typedef MainContext* Ctx;

static void verbose(Ctx ctx, const char* msg) {
    if (ctx->is_verbose) {
        printf("%s", msg);
    }
}

#ifndef __NDEBUG__
static void debug_toks(Ctx ctx, vector_t(Token) tokens) {
    if (ctx->is_verbose) {
        pprint_toks(ctx->identifiers, tokens);
    }
}

static void debug_ast(Ctx ctx, Ast* node, const char* name) {
    if (ctx->is_verbose) {
        pprint_ast(ctx->identifiers, node, name);
    }
}

static void debug_addressed_set(Ctx ctx) {
    if (ctx->is_verbose) {
        pprint_addressed_set(ctx->identifiers, ctx->frontend);
    }
}

static void debug_string_const_table(Ctx ctx) {
    if (ctx->is_verbose) {
        pprint_string_const_table(ctx->identifiers, ctx->frontend);
    }
}

static void debug_struct_typedef_table(Ctx ctx) {
    if (ctx->is_verbose) {
        pprint_struct_typedef_table(ctx->identifiers, ctx->frontend);
    }
}

static void debug_symbol_table(Ctx ctx) {
    if (ctx->is_verbose) {
        pprint_symbol_table(ctx->identifiers, ctx->frontend);
    }
}

static void debug_backend_symbol_table(Ctx ctx) {
    if (ctx->is_verbose) {
        pprint_backend_symbol_table(ctx->identifiers, ctx->backend);
    }
}
#endif

static void set_filename_ext(Ctx ctx, const char* ext) {
    for (size_t i = str_size(ctx->filename); i-- > 0;) {
        if (ctx->filename[i] == '.') {
            str_substr(ctx->filename, 0, i);
            str_append(ctx->filename, ext);
            return;
        }
    }
    THROW_ABORT;
}

static error_t compile(Ctx ctx, ErrorsContext* errors, FileIoContext* fileio) {
    IdentifierContext identifiers;
    FrontEndContext frontend;
    BackEndContext backend;
    vector_t(Token) tokens = vec_new();
    std::unique_ptr<CProgram> c_ast;
    std::unique_ptr<TacProgram> tac_ast;
    std::unique_ptr<AsmProgram> asm_ast;
    {
#ifndef __NDEBUG__
        ctx->identifiers = &identifiers;
        ctx->frontend = &frontend;
        ctx->backend = &backend;
#endif

        if (ctx->debug_code > 0
#ifdef __NDEBUG__
            && ctx->debug_code <= 127
#endif
        ) {
            ctx->is_verbose = true;
            errors->is_stdout = true;
        }

        identifiers.label_count = 0u;
        identifiers.var_count = 0u;
        identifiers.struct_count = 0u;
        identifiers.hash_table = map_new();

        frontend.string_const_table = map_new();
        frontend.struct_typedef_table = map_new();
        frontend.symbol_table = map_new();
        frontend.addressed_set = set_new();

        backend.symbol_table = map_new();
    }

    CATCH_ENTER;

#ifdef _WIN32
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_os, "Windows"));

#elif defined(__APPLE__)
#if defined(__GNUC__) && !defined(__clang__)
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_compiler, "gcc"));
#elif !defined(__clang__)
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_compiler, "unknown"));
#elif __clang_major__ < 5
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_cc_ver, CLANG_VERSION));
#endif

#elif defined(__linux__)
#ifdef __arm__
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_arch, "arm"));
#elif defined(__i386__)
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_arch, "x86"));
#elif !defined(__x86_64__)
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_arch, "unknown"));
#elif defined(__clang__)
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_compiler, "clang"));
#elif !defined(__GNUC__)
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_compiler, "unknown"));
#elif __GNUC__ < 8 || (__GNUC__ == 8 && __GNUC_MINOR__ == 0)
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_cc_ver, GCC_VERSION));
#endif

#else
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_os, "unknown"));
#endif

    verbose(ctx, "-- Lexing ... ");
    TRY(lex_c_code(ctx->filename, &ctx->includedirs, errors, fileio, &identifiers, &tokens));
    verbose(ctx, "OK\n");
#ifndef __NDEBUG__
    if (ctx->debug_code == 255) {
        debug_toks(ctx, tokens);
        EARLY_EXIT;
    }
#endif

    verbose(ctx, "-- Parsing ... ");
    TRY(parse_tokens(&tokens, errors, &identifiers, &c_ast));
    verbose(ctx, "OK\n");
#ifndef __NDEBUG__
    if (ctx->debug_code == 254) {
        debug_ast(ctx, c_ast.get(), "C AST");
        EARLY_EXIT;
    }
#endif

    verbose(ctx, "-- Semantic analysis ... ");
    TRY(analyze_semantic(c_ast.get(), errors, &frontend, &identifiers));
    verbose(ctx, "OK\n");
#ifndef __NDEBUG__
    if (ctx->debug_code == 253) {
        debug_ast(ctx, c_ast.get(), "C AST");
        debug_string_const_table(ctx);
        debug_struct_typedef_table(ctx);
        debug_symbol_table(ctx);
        EARLY_EXIT;
    }
#endif

    verbose(ctx, "-- TAC representation ... ");
    tac_ast = represent_three_address_code(std::move(c_ast), &frontend, &identifiers);
    if (ctx->optim_1_mask > 0) {
        verbose(ctx, "OK\n-- Level 1 optimization ... ");
        optimize_three_address_code(tac_ast.get(), &frontend, ctx->optim_1_mask);
    }
    verbose(ctx, "OK\n");
#ifndef __NDEBUG__
    if (ctx->debug_code == 252) {
        debug_ast(ctx, tac_ast.get(), "TAC AST");
        debug_string_const_table(ctx);
        debug_struct_typedef_table(ctx);
        debug_symbol_table(ctx);
        EARLY_EXIT;
    }
#endif

    verbose(ctx, "-- Assembly generation ... ");
    asm_ast = generate_assembly(std::move(tac_ast), &frontend, &identifiers);
    convert_symbol_table(asm_ast.get(), &backend, &frontend);
    if (ctx->optim_2_code > 0) {
        verbose(ctx, "OK\n-- Level 2 optimization ... ");
        allocate_registers(asm_ast.get(), &backend, &frontend, ctx->optim_2_code);
    }
    fix_stack(asm_ast.get(), &backend);
    verbose(ctx, "OK\n");
#ifndef __NDEBUG__
    if (ctx->debug_code == 251) {
        debug_ast(ctx, asm_ast.get(), "ASM AST");
        debug_addressed_set(ctx);
        debug_string_const_table(ctx);
        debug_struct_typedef_table(ctx);
        debug_symbol_table(ctx);
        debug_backend_symbol_table(ctx);
        EARLY_EXIT;
    }
#endif

    verbose(ctx, "-- Code emission ... ");
    set_filename_ext(ctx, "s");
    TRY(open_fwrite(fileio, ctx->filename));
    emit_gas_code(std::move(asm_ast), &backend, fileio, &identifiers);
    close_fwrite(fileio);
    verbose(ctx, "OK\n");

    FINALLY;
    for (size_t i = 0; i < map_size(identifiers.hash_table); ++i) {
        str_delete(pair_second(identifiers.hash_table[i]));
    }
    map_delete(identifiers.hash_table);

    map_delete(frontend.string_const_table);
    for (size_t i = 0; i < map_size(frontend.struct_typedef_table); ++i) {
        pair_second(frontend.struct_typedef_table[i]).reset();
    }
    map_delete(frontend.struct_typedef_table);
    for (size_t i = 0; i < map_size(frontend.symbol_table); ++i) {
        pair_second(frontend.symbol_table[i]).reset();
    }
    map_delete(frontend.symbol_table);
    set_delete(frontend.addressed_set);

    for (size_t i = 0; i < map_size(backend.symbol_table); ++i) {
        pair_second(backend.symbol_table[i]).reset();
    }
    map_delete(backend.symbol_table);

    vec_delete(tokens);
    CATCH_EXIT;
}

static bool arg_parse_uint8(const char* arg, uint8_t& value) {
    char* end_ptr = NULL;
    value = (uint8_t)strtol(arg, &end_ptr, 10);
    return end_ptr == arg;
}

static error_t arg_parse(Ctx ctx, char** argv) {
    CATCH_ENTER;
    size_t i = 0;

    if (!argv[++i]) {
        THROW_INIT(GET_ARG_MSG_0(MSG_no_debug_arg));
    }
    else if (arg_parse_uint8(argv[i], ctx->debug_code)) {
        THROW_INIT(GET_ARG_MSG(MSG_invalid_debug_arg, argv[i]));
    }

    if (!argv[++i]) {
        THROW_INIT(GET_ARG_MSG_0(MSG_no_optim_1_arg));
    }
    else if (arg_parse_uint8(argv[i], ctx->optim_1_mask) || ctx->optim_1_mask > 15) {
        THROW_INIT(GET_ARG_MSG(MSG_invalid_optim_1_arg, argv[i]));
    }

    if (!argv[++i]) {
        THROW_INIT(GET_ARG_MSG_0(MSG_no_optim_2_arg));
    }
    else if (arg_parse_uint8(argv[i], ctx->optim_2_code) || ctx->optim_2_code > 2) {
        THROW_INIT(GET_ARG_MSG(MSG_invalid_optim_2_arg, argv[i]));
    }

    if (!argv[++i]) {
        THROW_INIT(GET_ARG_MSG_0(MSG_no_input_files_arg));
    }
    ctx->filename = str_new(argv[i]);

    if (!argv[++i]) {
        THROW_INIT(GET_ARG_MSG_0(MSG_no_include_dir_arg));
    }
    do {
        vec_push_back(ctx->includedirs, (const char*)argv[i]);
    }
    while (argv[++i]);
    FINALLY;
    CATCH_EXIT;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

error_t main(int, char** argv) {
    ErrorsContext errors;
    FileIoContext fileio;
    MainContext ctx;
    {
        errors.errors = &errors;
        errors.fileio = &fileio;
        errors.is_stdout = false;
        errors.linebuf_map = map_new();
        errors.fopen_lines = vec_new();

        fileio.errors = &errors;
        fileio.fd_write = NULL;
        fileio.write_buf = str_new(NULL);
        fileio.filename = str_new(NULL);
        fileio.file_reads = vec_new();

        ctx.errors = &errors;
        ctx.is_verbose = false;
        ctx.filename = str_new(NULL);
        ctx.includedirs = vec_new();
    }
    CATCH_ENTER;
    TRY(arg_parse(&ctx, argv));
    TRY(compile(&ctx, &errors, &fileio));

    FINALLY;
    for (size_t i = 0; i < vec_size(errors.fopen_lines); ++i) {
        str_delete(errors.fopen_lines[i].filename);
    }
    map_delete(errors.linebuf_map);
    vec_delete(errors.fopen_lines);

    str_delete(fileio.write_buf);
    str_delete(fileio.filename);
    for (size_t i = 0; i < vec_size(fileio.file_reads); ++i) {
        str_delete(fileio.file_reads[i].filename);
    }
    vec_delete(fileio.file_reads);

    str_delete(ctx.filename);
    vec_delete(ctx.includedirs);
    CATCH_EXIT;
}
