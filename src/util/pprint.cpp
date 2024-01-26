#include "util/pprint.hpp"
#include "parser/lexer.hpp"
#include "ast/ast.hpp"
#include "ast/symbol_table.hpp"
#include "ast/c_ast.hpp"

#include <vector>
#include <iostream>

static void header_string(const std::string& header) {
    std::cout << "+\n+\n@@ " + header + " @@";
}

void pretty_print_tokens(const std::vector<Token>& tokens) {
    header_string("Tokens");
    for(size_t token = 0; token < tokens.size(); token++) {
        std::cout << "\n" + std::to_string(token) + ": (\"" + tokens[token].token + "\", " +
                     std::to_string(tokens[token].token_kind) + ")";
    }
    std::cout << std::endl;
}

static void field(const std::string& name, const std::string& value, size_t t) {
    std::cout << "\n";
    for(size_t i = 0; i < t-1; i++) {
        std::cout << "  ";
    }
    std::cout << name << ": ";
    if(!value.empty()) {
        std::cout << value;
    }
}

// TODO
static void print_ast(Ast* node, size_t t) {
    if(!node) {
        field("None", "", ++t);
        return;
    }
    switch(node->type()) {
        case AST_T::CAdd_t: {
            field("CAdd", "", ++t);
            break;
        }
        case AST_T::CConstInt_t: {
            field("CConstInt", "", ++t);
            CConstInt* p_node = static_cast<CConstInt*>(node);
            field("TInt", std::to_string(p_node->value), t+1);
            break;
        }
        case AST_T::CConstLong_t: {
            field("CConstLong", "", ++t);
            CConstLong* p_node = static_cast<CConstLong*>(node);
            field("TLong", std::to_string(p_node->value), t+1);
            break;
        }
        case AST_T::CConstant_t: {
            field("CConstant", "", ++t);
            CConstant* p_node = static_cast<CConstant*>(node);
            print_ast(p_node->constant.get(), t);
            break;
        }
        case AST_T::CBinary_t: {
            field("CBinary", "", ++t);
            CBinary* p_node = static_cast<CBinary*>(node);
            print_ast(p_node->binary_op.get(), t);
            print_ast(p_node->exp_left.get(), t);
            print_ast(p_node->exp_right.get(), t);
            break;
        }
        default:
            break; // TODO raise runtime exception
    }
}

void pretty_print_ast(Ast* node, const std::string& name) {
    header_string(name);
    print_ast(node, 0);
    std::cout << std::endl;
}

// TODO rm
// int main(void) {
//     std::unique_ptr<CBinaryOp> bin_op = std::make_unique<CAdd>();

//     std::unique_ptr<CConst> const_int = std::make_unique<CConstInt>(32);
//     std::unique_ptr<CExp> constant_1 = std::make_unique<CConstant>(std::move(const_int));

//     std::unique_ptr<CConst> const_long = std::make_unique<CConstLong>(64);
//     std::unique_ptr<CExp> constant_2 = std::make_unique<CConstant>(std::move(const_long));

//     std::unique_ptr<CExp> binary = std::make_unique<CBinary>(std::move(bin_op), nullptr,
//                                                              std::move(constant_2));

//     pretty_print_ast(binary.get(), "C AST");

//     return 0;
// }