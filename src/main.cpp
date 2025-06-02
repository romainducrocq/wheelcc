#ifndef __NDEBUG__
#include "util/pprint.hpp"
#endif
#include <inttypes.h>
#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>

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
    std::string filename;
    std::vector<std::string> includedirs;
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
static void debug_toks(Ctx ctx, const std::vector<Token>& tokens) {
    if (ctx->is_verbose) {
        pprint_toks(tokens);
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

static void compile(Ctx ctx, ErrorsContext* errors, FileIoContext* fileio) {

#ifdef _WIN32
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_os, "Windows"));
#elif defined(__i386__)
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_arch, "x86"));
#elif defined(__APPLE__)

#if !defined(__arm__) || !defined(__x86_64__)
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_arch, "unknown"));
#elif defined(__GNUC__) && !defined(__clang__)
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_compiler, "gcc"));
#elif !defined(__clang__)
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_compiler, "unknown"));
#elif __clang_major__ < 5
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_cc_ver, CLANG_VERSION));
#endif

#elif defined(__linux__)

#ifdef __arm__
    THROW_INIT(GET_FATAL_MSG(MSG_unsupported_arch, "arm"));
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

    if (ctx->debug_code > 0
#ifdef __NDEBUG__
        && ctx->debug_code <= 127
#endif
    ) {
        ctx->is_verbose = true;
    }

    verbose(ctx, "-- Lexing ... ");
    std::vector<Token> tokens = lex_c_code(ctx->filename, std::move(ctx->includedirs), errors, fileio);
    verbose(ctx, "OK\n");
#ifndef __NDEBUG__
    if (ctx->debug_code == 255) {
        debug_toks(ctx, tokens);
        return;
    }
#endif

    IdentifierContext identifiers;
    {
        identifiers.label_count = 0u;
        identifiers.var_count = 0u;
        identifiers.struct_count = 0u;
    }
#ifndef __NDEBUG__
    ctx->identifiers = &identifiers;
#endif

    verbose(ctx, "-- Parsing ... ");
    std::unique_ptr<CProgram> c_ast = parse_tokens(std::move(tokens), errors, &identifiers);
    verbose(ctx, "OK\n");
#ifndef __NDEBUG__
    if (ctx->debug_code == 254) {
        debug_ast(ctx, c_ast.get(), "C AST");
        return;
    }
#endif

    FrontEndContext frontend;
#ifndef __NDEBUG__
    ctx->frontend = &frontend;
#endif

    verbose(ctx, "-- Semantic analysis ... ");
    analyze_semantic(c_ast.get(), errors, &frontend, &identifiers);
    verbose(ctx, "OK\n");
#ifndef __NDEBUG__
    if (ctx->debug_code == 253) {
        debug_ast(ctx, c_ast.get(), "C AST");
        debug_string_const_table(ctx);
        debug_struct_typedef_table(ctx);
        debug_symbol_table(ctx);
        return;
    }
#endif

    verbose(ctx, "-- TAC representation ... ");
    std::unique_ptr<TacProgram> tac_ast = represent_three_address_code(std::move(c_ast), &frontend, &identifiers);
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
        return;
    }
#endif

    BackEndContext backend;
#ifndef __NDEBUG__
    ctx->backend = &backend;
#endif

    verbose(ctx, "-- Assembly generation ... ");
    std::unique_ptr<AsmProgram> asm_ast = generate_assembly(std::move(tac_ast), &frontend, &identifiers);
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
        return;
    }
#endif

    verbose(ctx, "-- Code emission ... ");
    ctx->filename += ".s";
    emit_gas_code(std::move(asm_ast), std::move(ctx->filename), &backend, fileio, &identifiers);
    verbose(ctx, "OK\n");
}

static bool arg_parse_uint8(const char* arg, uint8_t& value) {
    char* end_ptr = nullptr;
    errno = 0;
    value = (uint8_t)strtol(arg, &end_ptr, 10);
    return end_ptr == arg;
}

static void arg_parse(Ctx ctx, char** argv) {
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
    ctx->filename = std::string(argv[i]);

    if (!argv[++i]) {
        THROW_INIT(GET_ARG_MSG_0(MSG_no_include_dir_arg));
    }
    do {
        ctx->includedirs.emplace_back(std::string(argv[i]));
    }
    while (argv[++i]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int, char** argv) {
    ErrorsContext errors;
    FileIoContext fileio;
    MainContext ctx;
    {
        errors.errors = &errors;
        errors.fileio = &fileio;

        fileio.errors = &errors;
        fileio.fd_write = nullptr;

        ctx.errors = &errors;
        ctx.is_verbose = false;
    }

    try {
        arg_parse(&ctx, argv);
        compile(&ctx, &errors, &fileio);
    }
    catch (const std::runtime_error& err) {
        if (ctx.is_verbose) {
            printf("\n");
            fflush(stdout);
        }
        fprintf(stderr, "%s\n", err.what());
        return 1;
    }

    return 0;
}
