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
        case AST_T::CExpression_t: {
            field("CExpression", "", ++t);
            CExpression* p_node = static_cast<CExpression*>(node);
            print_ast(p_node->exp.get(), t);
            break;
        }
        case AST_T::CS_t: {
            field("CS", "", ++t);
            CS* p_node = static_cast<CS*>(node);
            print_ast(p_node->statement.get(), t);
            break;
        }
        case AST_T::CB_t: {
            field("CB", "", ++t);
            CB* p_node = static_cast<CB*>(node);
            field("List", "", t+1);
            for(const auto& item: p_node->block_items) {
                print_ast(item.get(), t+1);
            }
            break;
        }
        case AST_T::CLabel_t: {
            field("CLabel", "", ++t);
            CLabel* p_node = static_cast<CLabel*>(node);
            field("TIdentifier", p_node->target, t+1);
            break;
        }
        case AST_T::CGoto_t: {
            field("CGoto", "", ++t);
            break;
        }
        case AST_T::CNull_t: {
            field("CNull", "", ++t);
            break;
        }
        case AST_T::Int_t: {
            field("Int", "", ++t);
            break;
        }
        case AST_T::CStatic_t: {
            field("CStatic", "", ++t);
            break;
        }
        case AST_T::CFunctionDeclaration_t: {
            field("CFunctionDeclaration", "", ++t);
            CFunctionDeclaration* p_node = static_cast<CFunctionDeclaration*>(node);
            field("TIdentifier", p_node->target, t+1);
            field("List", "", t+1);
            for(const auto& item: p_node->params) {
                field("TIdentifier", item, t+2);
            }
            print_ast(p_node->body.get(), t);
            print_ast(p_node->fun_type.get(), t);
            print_ast(p_node->storage_class.get(), t);
            break;
        }
        default:
            raise_runtime_error("Ast pretty print not implemented for node type " + em(std::to_string(node->type())));
    }
}

void pretty_print_ast(Ast* node, const std::string& name) {
    header_string(name);
    print_ast(node, 0);
    std::cout << std::endl;
}
