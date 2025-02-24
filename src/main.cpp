#ifndef __NDEBUG__
#include "util/pprint.hpp"
#endif
#include <inttypes.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "util/throw.hpp"
#include "util/util.hpp"

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

// #include "optimization/optim_tac.hpp"
// #include "optimization/reg_alloc.hpp"

static std::unique_ptr<MainContext> context;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Main

static void verbose(const std::string& out, bool end) {
    if (context->is_verbose) {
        std::cout << out;
        if (end) {
            std::cout << std::endl;
        }
    }
}

#ifndef __NDEBUG__
static void debug_tokens(const std::vector<Token>& tokens) {
    if (context->is_verbose) {
        pretty_print_tokens(tokens);
    }
}

static void debug_ast(Ast* node, const std::string& name) {
    if (context->is_verbose) {
        pretty_print_ast(node, name);
    }
}

static void debug_symbol_table() {
    if (context->is_verbose) {
        pretty_print_symbol_table();
    }
}

static void debug_static_constant_table() {
    if (context->is_verbose) {
        pretty_print_static_constant_table();
    }
}

static void debug_struct_typedef_table() {
    if (context->is_verbose) {
        pretty_print_struct_typedef_table();
    }
}

static void debug_backend_symbol_table() {
    if (context->is_verbose) {
        pretty_print_backend_symbol_table();
    }
}

static void debug_asm_code() {
    if (context->is_verbose) {
        pretty_print_asm_code();
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

    INIT_UTIL_CONTEXT;

    INIT_ERRORS_CONTEXT;

    verbose("-- Lexing ... ", false);
    std::unique_ptr<std::vector<Token>> tokens = lexing(context->filename, std::move(context->includedirs));
    verbose("OK", true);
#ifndef __NDEBUG__
    if (context->debug_code == 255) {
        debug_tokens(*tokens);
        return;
    }
#endif

    INIT_IDENTIFIER_CONTEXT;

    verbose("-- Parsing ... ", false);
    std::unique_ptr<CProgram> c_ast = parsing(std::move(tokens));
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
        debug_symbol_table();
        debug_static_constant_table();
        debug_struct_typedef_table();
        return;
    }
#endif

    FREE_ERRORS_CONTEXT;

    verbose("-- TAC representation ... ", false);
    std::unique_ptr<TacProgram> tac_ast = three_address_code_representation(std::move(c_ast));
    // if (context->optim_1_mask > 0) {
    //     verbose("OK", true);
    //     verbose("-- Level 1 optimization ... ", false);
    //     three_address_code_optimization(tac_ast.get(), context->optim_1_mask);
    // }
    verbose("OK", true);
#ifndef __NDEBUG__
    if (context->debug_code == 252) {
        debug_ast(tac_ast.get(), "TAC AST");
        debug_symbol_table();
        debug_static_constant_table();
        debug_struct_typedef_table();
        return;
    }
#endif

    INIT_BACK_END_CONTEXT;

    verbose("-- Assembly generation ... ", false);
    std::unique_ptr<AsmProgram> asm_ast = assembly_generation(std::move(tac_ast));
    // if (context->optim_2_code > 0) {
    //     verbose("OK", true);
    //     verbose("-- Level 2 optimization ... ", false);
    //     register_allocation(asm_ast.get(), context->optim_2_code);
    // }
    convert_symbol_table(asm_ast.get());
    fix_stack(asm_ast.get());
    verbose("OK", true);
#ifndef __NDEBUG__
    if (context->debug_code == 251) {
        debug_ast(asm_ast.get(), "ASM AST");
        debug_symbol_table();
        debug_static_constant_table();
        debug_struct_typedef_table();
        debug_backend_symbol_table();
        return;
    }
#endif

    FREE_FRONT_END_CONTEXT;

    //     verbose("-- Code emission ... ", false);
    //     context->filename += ".s";
    //     gas_code_emission(std::move(asm_ast), std::move(context->filename));
    //     verbose("OK", true);
    // #ifndef __NDEBUG__
    //     if (context->debug_code == 250) {
    //         debug_asm_code();
    //         return;
    //     }
    // #endif

    FREE_BACK_END_CONTEXT;

    FREE_IDENTIFIER_CONTEXT;

    FREE_UTIL_CONTEXT;
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
        raise_argument_error(GET_ERROR_MESSAGE(ERROR_MESSAGE_ARGUMENT::no_debug_code_in_argument));
    }
    else if (arg_parse_uint8(arg, context->debug_code)) {
        raise_argument_error(GET_ERROR_MESSAGE(ERROR_MESSAGE_ARGUMENT::invalid_debug_code_in_argument, arg));
    }

    shift_args(arg);
    if (arg.empty()) {
        raise_argument_error(GET_ERROR_MESSAGE(ERROR_MESSAGE_ARGUMENT::no_optim_1_mask_in_argument));
    }
    else if (arg_parse_uint8(arg, context->optim_1_mask) || context->optim_1_mask > 15) {
        raise_argument_error(GET_ERROR_MESSAGE(ERROR_MESSAGE_ARGUMENT::invalid_optim_1_mask_in_argument, arg));
    }

    shift_args(arg);
    if (arg.empty()) {
        raise_argument_error(GET_ERROR_MESSAGE(ERROR_MESSAGE_ARGUMENT::no_optim_2_code_in_argument));
    }
    else if (arg_parse_uint8(arg, context->optim_2_code) || context->optim_2_code > 2) {
        raise_argument_error(GET_ERROR_MESSAGE(ERROR_MESSAGE_ARGUMENT::invalid_optim_2_code_in_argument, arg));
    }

    shift_args(arg);
    if (arg.empty()) {
        raise_argument_error(GET_ERROR_MESSAGE(ERROR_MESSAGE_ARGUMENT::no_input_files_in_argument));
    }
    context->filename = arg;

    shift_args(arg);
    if (arg.empty()) {
        raise_argument_error(GET_ERROR_MESSAGE(ERROR_MESSAGE_ARGUMENT::no_include_directories_in_argument));
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
    context = std::make_unique<MainContext>();
    {
        size_t i = static_cast<size_t>(argc);
        context->args.reserve(i);
        for (; i-- > 0;) {
            std::string arg = argv[i];
            context->args.emplace_back(std::move(arg));
        }
    }

    arg_parse();
    compile();

    context.reset();

    return 0;
}
