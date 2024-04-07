#include "util/error.hpp"
#ifndef __NDEBUG__
#include "util/pprint.hpp"
#endif
#include "ast/ast.hpp"
#include "ast/front_ast.hpp"
#include "ast/interm_ast.hpp"
#include "ast/back_ast.hpp"
#include "frontend/parser/lexer.hpp"
#include "frontend/parser/parser.hpp"
#include "frontend/intermediate/semantic.hpp"
#include "frontend/intermediate/tac_repr.hpp"
#include "backend/assembly/asm_gen.hpp"
#include "backend/emitter/att_code.hpp"

#include <string>
#include <vector>
#include <iostream>

static bool VERBOSE = false;

static void verbose(const std::string& out, bool end) {
    if(VERBOSE) {
        std::cout << out;
        if(end) {
            std::cout << std::endl;
        }
    }
}

#ifndef __NDEBUG__
static void debug_tokens(const std::vector<Token>& tokens) {
    if(VERBOSE) {
        pretty_print_tokens(tokens);
    }
}

static void debug_ast(Ast* node, const std::string& name) {
    if(VERBOSE) {
        pretty_print_ast(node, name);
    }
}

static void debug_symbol_table() {
    if(VERBOSE) {
        pretty_print_symbol_table();
    }
}

static void debug_struct_type_table() {
    if(VERBOSE) {
        pretty_print_struct_type_table();
    }
}

static void debug_backend_symbol_table() {
    if(VERBOSE) {
        pretty_print_backend_symbol_table();
    }
}

static void debug_asm_code() {
    if(VERBOSE) {
        pretty_print_asm_code();
    }
}
#endif

static void do_compile(std::string& filename, int opt_code, int /*opt_s_code*/) {
    if(opt_code > 0
#ifdef __NDEBUG__
       && opt_code < 200
#endif
    ) {
        VERBOSE = true;
    }

    verbose("-- Lexing ... ", false);
    std::vector<Token> tokens = lexing(filename);
    verbose("OK", true);
#ifndef __NDEBUG__
    if(opt_code == 255) {
        debug_tokens(tokens);
        return;
    }
#endif

    verbose("-- Parsing ... ", false);
    std::unique_ptr<CProgram> c_ast = parsing(std::move(tokens));
    verbose("OK", true);
#ifndef __NDEBUG__
    if(opt_code == 254) {
        debug_ast(c_ast.get(), "C AST");
        return;
    }
#endif

    verbose("-- Semantic analysis ... ", false);
    analyze_semantic(c_ast.get());
    verbose("OK", true);
#ifndef __NDEBUG__
    if(opt_code == 253) {
        debug_ast(c_ast.get(), "C AST");
        debug_symbol_table();
        debug_struct_type_table();
        return;
    }
#endif

    verbose("-- TAC representation ... ", false);
    std::unique_ptr<TacProgram> tac_ast = three_address_code_representation(std::move(c_ast));
    verbose("OK", true);
#ifndef __NDEBUG__
    if(opt_code == 252) {
        debug_ast(tac_ast.get(), "TAC AST");
        debug_symbol_table();
        debug_struct_type_table();
        return;
    }
#endif

    verbose("-- Assembly generation ... ", false);
    std::unique_ptr<AsmProgram> asm_ast = assembly_generation(std::move(tac_ast));
    verbose("OK", true);
#ifndef __NDEBUG__
    if(opt_code == 251) {
        debug_ast(asm_ast.get(), "ASM AST");
        debug_symbol_table();
        debug_struct_type_table();
        debug_backend_symbol_table();
        return;
    }
#endif

    verbose("-- Code emission ... ", false);
    filename = filename.substr(0, filename.size()-2) + ".s";
    code_emission(std::move(asm_ast), std::move(filename));
    verbose("OK", true);
#ifndef __NDEBUG__
    if(opt_code == 250) {
        debug_asm_code();
        return;
    }
#endif

    return;
}

static std::vector<std::string> args;

static void shift_args(std::string& arg) {
    if(!args.empty()) {
        arg = std::move(args.back());
        args.pop_back();
        return;
    }
    arg = "";
}

static void arg_parse(std::string& filename, int& opt_code, int& opt_s_code) {
    std::string arg;
    shift_args(arg);

    shift_args(arg);
    if(arg.empty()) {
        raise_runtime_error("No option code passed in " + em("args[0]"));
    }
    opt_code = std::stoi(arg);

    shift_args(arg);
    if(arg.empty()) {
        raise_runtime_error("No file name passed in " + em("args[1]"));
    }
    filename = std::move(arg);

    opt_s_code = 0;
}

int main(int argc, char **argv) {

    for(size_t i = static_cast<size_t>(argc); i-- > 0 ;){
        args.push_back(argv[i]);
    }

    std::string filename;
    int opt_code;
    int opt_s_code;
    arg_parse(filename, opt_code, opt_s_code);

    do_compile(filename, opt_code, opt_s_code);

    return 0;
}
