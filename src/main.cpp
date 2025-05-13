#ifndef __NDEBUG__
#include "util/pprint.hpp"
#endif
#include <inttypes.h>
#include <iostream>
#include <memory>
#include <stdexcept>
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
    MainContext();

    ErrorsContext* errors;
    // Main
    bool is_verbose;
    uint8_t debug_code;
    uint8_t optim_1_mask;
    uint8_t optim_2_code;
    std::string filename;
    std::vector<std::string> includedirs;
    std::vector<std::string> args;
};

MainContext::MainContext() : is_verbose(false) {}

static std::unique_ptr<MainContext> ctx;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Main

static void verbose(std::string&& out, bool end) {
    if (ctx->is_verbose) {
        std::cout << out;
        if (end) {
            std::cout << std::endl;
        }
    }
}

#ifndef __NDEBUG__
static void debug_toks(const std::vector<Token>& tokens) {
    if (ctx->is_verbose) {
        pprint_toks(tokens);
    }
}

static void debug_ast(Ast* node, std::string&& name) {
    if (ctx->is_verbose) {
        pprint_ast(node, name);
    }
}

static void debug_addressed_set() {
    if (ctx->is_verbose) {
        pprint_addressed_set();
    }
}

static void debug_string_const_table() {
    if (ctx->is_verbose) {
        pprint_string_const_table();
    }
}

static void debug_struct_typedef_table() {
    if (ctx->is_verbose) {
        pprint_struct_typedef_table();
    }
}

static void debug_symbol_table() {
    if (ctx->is_verbose) {
        pprint_symbol_table();
    }
}

static void debug_backend_symbol_table() {
    if (ctx->is_verbose) {
        pprint_backend_symbol_table();
    }
}

static void debug_asm_code() {
    if (ctx->is_verbose) {
        pprint_asm_code();
    }
}
#endif

static void compile() {
    if (ctx->debug_code > 0
#ifdef __NDEBUG__
        && ctx->debug_code <= 127
#endif
    ) {
        ctx->is_verbose = true;
    }

    verbose("-- Lexing ... ", false);
    std::vector<Token> tokens = lex_c_code(ctx->filename, std::move(ctx->includedirs));
    verbose("OK", true);
#ifndef __NDEBUG__
    if (ctx->debug_code == 255) {
        debug_toks(tokens);
        return;
    }
#endif

    INIT_IDENTIFIER_CTX;

    verbose("-- Parsing ... ", false);
    std::unique_ptr<CProgram> c_ast = parse_tokens(std::move(tokens));
    verbose("OK", true);
#ifndef __NDEBUG__
    if (ctx->debug_code == 254) {
        debug_ast(c_ast.get(), "C AST");
        return;
    }
#endif

    INIT_FRONTEND_CTX;

    verbose("-- Semantic analysis ... ", false);
    analyze_semantic(c_ast.get());
    verbose("OK", true);
#ifndef __NDEBUG__
    if (ctx->debug_code == 253) {
        debug_ast(c_ast.get(), "C AST");
        debug_string_const_table();
        debug_struct_typedef_table();
        debug_symbol_table();
        return;
    }
#endif

    FREE_ERRORS_CTX;

    verbose("-- TAC representation ... ", false);
    std::unique_ptr<TacProgram> tac_ast = represent_three_address_code(std::move(c_ast));
    if (ctx->optim_1_mask > 0) {
        verbose("OK", true);
        verbose("-- Level 1 optimization ... ", false);
        optimize_three_address_code(tac_ast.get(), ctx->optim_1_mask);
    }
    verbose("OK", true);
#ifndef __NDEBUG__
    if (ctx->debug_code == 252) {
        debug_ast(tac_ast.get(), "TAC AST");
        debug_string_const_table();
        debug_struct_typedef_table();
        debug_symbol_table();
        return;
    }
#endif

    INIT_BACKEND_CTX;

    verbose("-- Assembly generation ... ", false);
    std::unique_ptr<AsmProgram> asm_ast = generate_assembly(std::move(tac_ast));
    convert_symbol_table(asm_ast.get());
    if (ctx->optim_2_code > 0) {
        verbose("OK", true);
        verbose("-- Level 2 optimization ... ", false);
        allocate_registers(asm_ast.get(), ctx->optim_2_code);
    }
    fix_stack(asm_ast.get());
    verbose("OK", true);
#ifndef __NDEBUG__
    if (ctx->debug_code == 251) {
        debug_ast(asm_ast.get(), "ASM AST");
        debug_addressed_set();
        debug_string_const_table();
        debug_struct_typedef_table();
        debug_symbol_table();
        debug_backend_symbol_table();
        return;
    }
#endif

    FREE_FRONTEND_CTX;

    verbose("-- Code emission ... ", false);
    ctx->filename += ".s";
    emit_gas_code(std::move(asm_ast), std::move(ctx->filename));
    verbose("OK", true);
#ifndef __NDEBUG__
    if (ctx->debug_code == 250) {
        debug_asm_code();
        return;
    }
#endif

    FREE_BACKEND_CTX;

    FREE_IDENTIFIER_CTX;

    FREE_FILEIO_CTX;
}

static void shift_args(std::string& arg) {
    if (!ctx->args.empty()) {
        arg = std::move(ctx->args.back());
        ctx->args.pop_back();
        return;
    }
    arg = "";
}

static bool arg_parse_uint8(std::string& arg, uint8_t& code) {
    std::vector<char> buf(arg.begin(), arg.end());
    buf.push_back('\0');
    char* end_ptr = nullptr;
    errno = 0;
    code = static_cast<uint8_t>(strtol(&buf[0], &end_ptr, 10));
    return end_ptr == &buf[0];
}

static void arg_parse() {
    std::string arg;
    shift_args(arg);

    shift_args(arg);
    if (arg.empty()) {
        THROW_INIT(GET_ARG_MSG_0(MSG_no_debug_arg));
    }
    else if (arg_parse_uint8(arg, ctx->debug_code)) {
        THROW_INIT(GET_ARG_MSG(MSG_invalid_debug_arg, arg.c_str()));
    }

    shift_args(arg);
    if (arg.empty()) {
        THROW_INIT(GET_ARG_MSG_0(MSG_no_optim_1_arg));
    }
    else if (arg_parse_uint8(arg, ctx->optim_1_mask) || ctx->optim_1_mask > 15) {
        THROW_INIT(GET_ARG_MSG(MSG_invalid_optim_1_arg, arg.c_str()));
    }

    shift_args(arg);
    if (arg.empty()) {
        THROW_INIT(GET_ARG_MSG_0(MSG_no_optim_2_arg));
    }
    else if (arg_parse_uint8(arg, ctx->optim_2_code) || ctx->optim_2_code > 2) {
        THROW_INIT(GET_ARG_MSG(MSG_invalid_optim_2_arg, arg.c_str()));
    }

    shift_args(arg);
    if (arg.empty()) {
        THROW_INIT(GET_ARG_MSG_0(MSG_no_input_files_arg));
    }
    ctx->filename = arg;

    shift_args(arg);
    if (arg.empty()) {
        THROW_INIT(GET_ARG_MSG_0(MSG_no_include_dir_arg));
    }
    do {
        std::string includedir = arg;
        ctx->includedirs.emplace_back(std::move(includedir));
        shift_args(arg);
    }
    while (!arg.empty());

    arg = "";
    ctx->args.clear();
    std::vector<std::string>().swap(ctx->args);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    try {
        ctx = std::make_unique<MainContext>();
        {
            size_t i = static_cast<size_t>(argc);
            ctx->args.reserve(i);
            for (; i-- > 0;) {
                std::string arg = argv[i];
                ctx->args.emplace_back(std::move(arg));
            }
        }

        INIT_FILEIO_CTX;

        INIT_ERRORS_CTX;

        {
            errors->errors = errors.get();
            errors->fileio = fileio.get();
            fileio->errors = errors.get();
            ctx->errors = errors.get();
        }

#ifdef _WIN32
        THROW_INIT(GET_FATAL_MSG(MSG_unsupported_os, "Windows"));
#elif defined(__APPLE__)
        THROW_INIT(GET_FATAL_MSG(MSG_unsupported_os, "MacOS"));
#elif !defined(__linux__)
        THROW_INIT(GET_FATAL_MSG(MSG_unsupported_os, "unknown"));
#elif defined(__arm__)
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
        THROW_INIT(GET_FATAL_MSG(MSG_unsupported_gcc_ver, GCC_VERSION));
#endif

        arg_parse();

        compile();

        ctx.reset();
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    return 0;
}
