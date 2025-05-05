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

    bool is_verbose;
    uint8_t debug_code;
    uint8_t optim_1_mask;
    uint8_t optim_2_code;
    std::string filename;
    std::vector<std::string> includedirs;
    std::vector<std::string> args;
};

MainContext::MainContext() : is_verbose(false) {}

static std::unique_ptr<MainContext> context;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Main

static void verbose(std::string&& out, bool end) {
    if (context->is_verbose) {
        std::cout << out;
        if (end) {
            std::cout << std::endl;
        }
    }
}

#ifndef __NDEBUG__
static void debug_toks(const std::vector<Token>& tokens) {
    if (context->is_verbose) {
        pprint_toks(tokens);
    }
}

static void debug_ast(Ast* node, std::string&& name) {
    if (context->is_verbose) {
        pprint_ast(node, name);
    }
}

static void debug_addressed_set() {
    if (context->is_verbose) {
        pprint_addressed_set();
    }
}

static void debug_string_const_table() {
    if (context->is_verbose) {
        pprint_string_const_table();
    }
}

static void debug_struct_typedef_table() {
    if (context->is_verbose) {
        pprint_struct_typedef_table();
    }
}

static void debug_symbol_table() {
    if (context->is_verbose) {
        pprint_symbol_table();
    }
}

static void debug_backend_symbol_table() {
    if (context->is_verbose) {
        pprint_backend_symbol_table();
    }
}

static void debug_asm_code() {
    if (context->is_verbose) {
        pprint_asm_code();
    }
}
#endif

static void compile() {
    if (context->debug_code > 0
#ifdef __NDEBUG__
        && context->debug_code <= 127
#endif
    ) {
        context->is_verbose = true;
    }

    verbose("-- Lexing ... ", false);
    std::vector<Token> tokens = lex_c_code(context->filename, std::move(context->includedirs));
    verbose("OK", true);
#ifndef __NDEBUG__
    if (context->debug_code == 255) {
        debug_toks(tokens);
        return;
    }
#endif

    INIT_IDENTIFIER_CONTEXT;

    verbose("-- Parsing ... ", false);
    std::unique_ptr<CProgram> c_ast = parse_tokens(std::move(tokens));
    verbose("OK", true);
#ifndef __NDEBUG__
    if (context->debug_code == 254) {
        debug_ast(c_ast.get(), "C AST");
        return;
    }
#endif

    INIT_FRONT_END_CONTEXT;

    verbose("-- Semantic analysis ... ", false);
    analyze_semantic(c_ast.get());
    verbose("OK", true);
#ifndef __NDEBUG__
    if (context->debug_code == 253) {
        debug_ast(c_ast.get(), "C AST");
        debug_string_const_table();
        debug_struct_typedef_table();
        debug_symbol_table();
        return;
    }
#endif

    FREE_ERRORS_CONTEXT;

    verbose("-- TAC representation ... ", false);
    std::unique_ptr<TacProgram> tac_ast = represent_three_address_code(std::move(c_ast));
    if (context->optim_1_mask > 0) {
        verbose("OK", true);
        verbose("-- Level 1 optimization ... ", false);
        optimize_three_address_code(tac_ast.get(), context->optim_1_mask);
    }
    verbose("OK", true);
#ifndef __NDEBUG__
    if (context->debug_code == 252) {
        debug_ast(tac_ast.get(), "TAC AST");
        debug_string_const_table();
        debug_struct_typedef_table();
        debug_symbol_table();
        return;
    }
#endif

    INIT_BACK_END_CONTEXT;

    verbose("-- Assembly generation ... ", false);
    std::unique_ptr<AsmProgram> asm_ast = generate_assembly(std::move(tac_ast));
    convert_symbol_table(asm_ast.get());
    if (context->optim_2_code > 0) {
        verbose("OK", true);
        verbose("-- Level 2 optimization ... ", false);
        allocate_registers(asm_ast.get(), context->optim_2_code);
    }
    fix_stack(asm_ast.get());
    verbose("OK", true);
#ifndef __NDEBUG__
    if (context->debug_code == 251) {
        debug_ast(asm_ast.get(), "ASM AST");
        debug_addressed_set();
        debug_string_const_table();
        debug_struct_typedef_table();
        debug_symbol_table();
        debug_backend_symbol_table();
        return;
    }
#endif

    FREE_FRONT_END_CONTEXT;

    verbose("-- Code emission ... ", false);
    context->filename += ".s";
    emit_gas_code(std::move(asm_ast), std::move(context->filename));
    verbose("OK", true);
#ifndef __NDEBUG__
    if (context->debug_code == 250) {
        debug_asm_code();
        return;
    }
#endif

    FREE_BACK_END_CONTEXT;

    FREE_IDENTIFIER_CONTEXT;

    FREE_FILEIO_CONTEXT;
}

static void shift_args(std::string& arg) {
    if (!context->args.empty()) {
        arg = std::move(context->args.back());
        context->args.pop_back();
        return;
    }
    arg = "";
}

static bool arg_parse_uint8(std::string& arg, uint8_t& code) {
    std::vector<char> buffer(arg.begin(), arg.end());
    buffer.push_back('\0');
    char* end_ptr = nullptr;
    errno = 0;
    code = static_cast<uint8_t>(strtol(&buffer[0], &end_ptr, 10));
    return end_ptr == &buffer[0];
}

static void arg_parse() {
    std::string arg;
    shift_args(arg);

    shift_args(arg);
    if (arg.empty()) {
        RAISE_ARGUMENT_ERROR(GET_ARGUMENT_MESSAGE_0(MESSAGE_ARGUMENT::no_debug_code_in_argument));
    }
    else if (arg_parse_uint8(arg, context->debug_code)) {
        RAISE_ARGUMENT_ERROR(GET_ARGUMENT_MESSAGE(MESSAGE_ARGUMENT::invalid_debug_code_in_argument, arg.c_str()));
    }

    shift_args(arg);
    if (arg.empty()) {
        RAISE_ARGUMENT_ERROR(GET_ARGUMENT_MESSAGE_0(MESSAGE_ARGUMENT::no_optim_1_mask_in_argument));
    }
    else if (arg_parse_uint8(arg, context->optim_1_mask) || context->optim_1_mask > 15) {
        RAISE_ARGUMENT_ERROR(GET_ARGUMENT_MESSAGE(MESSAGE_ARGUMENT::invalid_optim_1_mask_in_argument, arg.c_str()));
    }

    shift_args(arg);
    if (arg.empty()) {
        RAISE_ARGUMENT_ERROR(GET_ARGUMENT_MESSAGE_0(MESSAGE_ARGUMENT::no_optim_2_code_in_argument));
    }
    else if (arg_parse_uint8(arg, context->optim_2_code) || context->optim_2_code > 2) {
        RAISE_ARGUMENT_ERROR(GET_ARGUMENT_MESSAGE(MESSAGE_ARGUMENT::invalid_optim_2_code_in_argument, arg.c_str()));
    }

    shift_args(arg);
    if (arg.empty()) {
        RAISE_ARGUMENT_ERROR(GET_ARGUMENT_MESSAGE_0(MESSAGE_ARGUMENT::no_input_files_in_argument));
    }
    context->filename = arg;

    shift_args(arg);
    if (arg.empty()) {
        RAISE_ARGUMENT_ERROR(GET_ARGUMENT_MESSAGE_0(MESSAGE_ARGUMENT::no_include_directories_in_argument));
    }
    do {
        std::string includedir = arg;
        context->includedirs.emplace_back(std::move(includedir));
        shift_args(arg);
    }
    while (!arg.empty());

    arg = "";
    context->args.clear();
    std::vector<std::string>().swap(context->args);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    try {
        context = std::make_unique<MainContext>();
        {
            size_t i = static_cast<size_t>(argc);
            context->args.reserve(i);
            for (; i-- > 0;) {
                std::string arg = argv[i];
                context->args.emplace_back(std::move(arg));
            }
        }

        INIT_FILEIO_CONTEXT;

        INIT_ERRORS_CONTEXT;

#ifdef _WIN32
        RAISE_FATAL_ERROR(GET_FATAL_MESSAGE(MESSAGE_FATAL::operating_system_not_supported, "Windows"));
#elif defined(__APPLE__)
        RAISE_FATAL_ERROR(GET_FATAL_MESSAGE(MESSAGE_FATAL::operating_system_not_supported, "MacOS"));
#elif !defined(__linux__)
        RAISE_FATAL_ERROR(GET_FATAL_MESSAGE(MESSAGE_FATAL::operating_system_not_supported, "unknown"));
#elif defined(__arm__)
        RAISE_FATAL_ERROR(GET_FATAL_MESSAGE(MESSAGE_FATAL::architecture_not_supported, "arm"));
#elif defined(__i386__)
        RAISE_FATAL_ERROR(GET_FATAL_MESSAGE(MESSAGE_FATAL::architecture_not_supported, "x86"));
#elif !defined(__x86_64__)
        RAISE_FATAL_ERROR(GET_FATAL_MESSAGE(MESSAGE_FATAL::architecture_not_supported, "unknown"));
#elif defined(__clang__)
        RAISE_FATAL_ERROR(GET_FATAL_MESSAGE(MESSAGE_FATAL::compiler_not_supported, "clang"));
#elif !defined(__GNUC__)
        RAISE_FATAL_ERROR(GET_FATAL_MESSAGE(MESSAGE_FATAL::compiler_not_supported, "unknown"));
#elif __GNUC__ < 8 || (__GNUC__ == 8 && __GNUC_MINOR__ == 0)
        RAISE_FATAL_ERROR(GET_FATAL_MESSAGE(MESSAGE_FATAL::gcc_version_not_supported, GCC_VERSION));
#endif

        arg_parse();

        compile();

        context.reset();
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }

    return 0;
}
