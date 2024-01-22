#include "parser/lexer.hpp"
#include "parser/parser.hpp"
#include "util/error.hpp"
#include "util/pprint.hpp"

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

static void debug_tokens(const std::vector<Token>& tokens) {
    if(VERBOSE) {
        pretty_print_tokens(tokens);
    }
}

/** TODO
cdef void debug_ast(AST ast): #
    if VERBOSE: #
        pretty_print_ast(ast) #
*/

/**  TODO
cdef void debug_symbol_table(): #
    if VERBOSE: #
        pretty_print_symbol_table() #
*/

/** TODO
cdef void debug_backend_symbol_table(): #
    if VERBOSE: #
        pretty_print_backend_symbol_table() #
*/

/** TODO
cdef void debug_asm_code(list[str] asm_code): #
    verbose("OK") #
    if VERBOSE: #
        pretty_print_asm_code(asm_code) #
*/

/** TODO
cdef void do_compile(str filename, int32 opt_code, int32 opt_s_code):

    verbose("-- Lexing ... ", end="")
    cdef list[Token] tokens = lexing(filename)
    verbose("OK")
    if opt_code == 255:
        debug_tokens(tokens) #
        return

    verbose("-- Parsing ... ", end="")
    cdef CProgram c_ast = parsing(tokens)
    verbose("OK")
    if opt_code == 254:
        debug_ast(c_ast) #
        return

    verbose("-- Semantic analysis ... ", end="")
    analyze_semantic(c_ast)
    verbose("OK")
    if opt_code == 253:
        debug_ast(c_ast) #
        debug_symbol_table() #
        return

    verbose("-- TAC representation ... ", end="")
    cdef TacProgram tac_ast = three_address_code_representation(c_ast)
    verbose("OK")
    if opt_code == 252:
        debug_ast(tac_ast) #
        debug_symbol_table() #
        return

    verbose("-- Assembly generation ... ", end="")
    cdef AsmProgram asm_ast = assembly_generation(tac_ast)
    verbose("OK")
    if opt_code == 251:
        debug_ast(asm_ast) #
        debug_symbol_table() #
        debug_backend_symbol_table() #
        return

    verbose("-- Code emission ... ", end="")
    if opt_code == 250: #
        debug_asm_code(code_emission_print(asm_ast)) #
        return #

    filename = f"{filename.rsplit('.', 1)[0]}.s"
    code_emission(asm_ast, filename)
    verbose("OK")
*/

static void do_compile(const std::string& filename, int opt_code, int /*opt_s_code*/) {

    verbose("-- Lexing ... ", false);
    std::vector<Token> tokens;
    lexing(filename, tokens);
    verbose("OK", true);
    if(opt_code == 255) {
        debug_tokens(tokens);
        return;
    }

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
        raise_runtime_error("No option code passed in args(0)");
    }
    opt_code = std::stoi(arg);

    shift_args(arg);
    if(arg.empty()) {
        raise_runtime_error("No file name passed in args(1)");
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
    if(opt_code > 0) {
        VERBOSE = true;
    }

    do_compile(filename, opt_code, opt_s_code);

    return 0;
}
