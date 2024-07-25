#ifndef __NDEBUG__
#include "util/pprint.hpp"
#endif
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

#include "backend/emitter/att_code.hpp"

static std::unique_ptr<MainContext> context;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Main

static void verbose(const std::string& out, bool end) {
    if (context->VERBOSE) {
        std::cout << out;
        if (end) {
            std::cout << std::endl;
        }
    }
}

#ifndef __NDEBUG__
static void debug_tokens(const std::vector<Token>& tokens) {
    if (context->VERBOSE) {
        pretty_print_tokens(tokens);
    }
}

static void debug_ast(Ast* node, const std::string& name) {
    if (context->VERBOSE) {
        pretty_print_ast(node, name);
    }
}

static void debug_symbol_table() {
    if (context->VERBOSE) {
        pretty_print_symbol_table();
    }
}

static void debug_static_constant_table() {
    if (context->VERBOSE) {
        pretty_print_static_constant_table();
    }
}

static void debug_struct_typedef_table() {
    if (context->VERBOSE) {
        pretty_print_struct_typedef_table();
    }
}

static void debug_backend_symbol_table() {
    if (context->VERBOSE) {
        pretty_print_backend_symbol_table();
    }
}

static void debug_asm_code() {
    if (context->VERBOSE) {
        pretty_print_asm_code();
    }
}
#endif

static void compile() {
    if (context->opt_code > 0
#ifdef __NDEBUG__
        && context->opt_code <= 127
#endif
    ) {
        context->VERBOSE = true;
    }

    INIT_UTIL_CONTEXT;

    verbose("-- Lexing ... ", false);
    std::unique_ptr<std::vector<Token>> tokens = lexing(context->filename);
    verbose("OK", true);
#ifndef __NDEBUG__
    if (context->opt_code == 255) {
        debug_tokens(*tokens);
        return;
    }
#endif

    verbose("-- Parsing ... ", false);
    std::unique_ptr<CProgram> c_ast = parsing(std::move(tokens));
    verbose("OK", true);
#ifndef __NDEBUG__
    if (context->opt_code == 254) {
        debug_ast(c_ast.get(), "C AST");
        return;
    }
#endif

    INIT_FRONT_END_CONTEXT;

    verbose("-- Semantic analysis ... ", false);
    analyze_semantic(c_ast.get());
    verbose("OK", true);
#ifndef __NDEBUG__
    if (context->opt_code == 253) {
        debug_ast(c_ast.get(), "C AST");
        debug_symbol_table();
        debug_static_constant_table();
        debug_struct_typedef_table();
        return;
    }
#endif

    verbose("-- TAC representation ... ", false);
    std::unique_ptr<TacProgram> tac_ast = three_address_code_representation(std::move(c_ast));
    verbose("OK", true);
#ifndef __NDEBUG__
    if (context->opt_code == 252) {
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
    verbose("OK", true);
#ifndef __NDEBUG__
    if (context->opt_code == 251) {
        debug_ast(asm_ast.get(), "ASM AST");
        debug_symbol_table();
        debug_static_constant_table();
        debug_struct_typedef_table();
        debug_backend_symbol_table();
        return;
    }
#endif

    FREE_FRONT_END_CONTEXT;

    verbose("-- Code emission ... ", false);
    context->filename = context->filename.substr(0, context->filename.size() - 2) + ".s";
    code_emission(std::move(asm_ast), std::move(context->filename));
    verbose("OK", true);
#ifndef __NDEBUG__
    if (context->opt_code == 250) {
        debug_asm_code();
        return;
    }
#endif

    FREE_BACK_END_CONTEXT;

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

static void arg_parse() {
    std::string arg;
    shift_args(arg);

    shift_args(arg);
    if (arg.empty()) {
        raise_argument_error(GET_ERROR_MESSAGE(ERROR_MESSAGE::no_option_code_in_argument));
    }
    {
        std::vector<char> buffer(arg.begin(), arg.end());
        buffer.push_back('\0');
        char* end_ptr = nullptr;
        errno = 0;
        context->opt_code = static_cast<int>(strtol(&buffer[0], &end_ptr, 10));

        if (end_ptr == &buffer[0] || context->opt_code < 0 || context->opt_code > 255) {
            raise_argument_error(GET_ERROR_MESSAGE(ERROR_MESSAGE::invalid_option_code_in_argument, arg));
        }
    }

    shift_args(arg);
    if (arg.empty()) {
        raise_argument_error(GET_ERROR_MESSAGE(ERROR_MESSAGE::no_input_files_in_argument));
    }
    context->filename = std::move(arg);

    context->opt_s_code = 0; // TODO
    context->args.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    context = std::make_unique<MainContext>();
    for (size_t i = static_cast<size_t>(argc); i-- > 0;) {
        context->args.push_back(argv[i]);
    }
    arg_parse();

    compile();
    context.reset();

    return 0;
}
