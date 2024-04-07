#!/bin/python3

from ast import ast


class AstNode:
    name = ""
    fields = []
    children = []

    def __init__(self, name, fields, children):
        self.name = name
        self.fields = fields
        self.children = children


def to_string(field_type, field_name):
    if field_type in ["TIdentifier"]:
        return field_name
    else:
        return f"std::to_string({field_name})"


def print_ast_case(node):
    print(f"        case AST_T::{node.name}_t: {{")
    print(f"            field(\"{node.name}\", \"\", ++t);")
    
    if node.fields or node.children:
        print(f"            {node.name}* p_node = static_cast<{node.name}*>(node);")
    
    for field in node.fields:
        if field[1][0] == "[":
            print(f"            field(\"List[\" + std::to_string(p_node->{field[1][1:]}.size()) + \"]\", \"\", t+1);")
            print(f"            for(const auto& item: p_node->{field[1][1:]}) {{")
            print(f"                field(\"{field[0]().name}\", {to_string(field[0]().name, 'item')}, t+2);")
            print(f"            }}")
        else:
            print(f"            field(\"{field[0]().name}\", {to_string(field[0]().name, 'p_node->' + field[1])}, t+1);")
    
    for child in node.children:
        if child[0] == "[":
            print(f"            field(\"List[\" + std::to_string(p_node->{child[1:]}.size()) + \"]\", \"\", t+1);")
            print(f"            for(const auto& item: p_node->{child[1:]}) {{")
            print(f"                print_ast(item.get(), t+1);")
            print(f"            }}")
        else:
            print(f"            print_ast(p_node->{child}.get(), t);")
    
    print(f"            break;")
    print(f"        }}")


if __name__ == "__main__":
    print(R"""#include "util/pprint.hpp"
#ifndef __NDEBUG__
#include "util/error.hpp"
#include "ast/ast.hpp"
#include "ast/front_symt.hpp"
#include "ast/back_symt.hpp"
#include "ast/front_ast.hpp"
#include "ast/interm_ast.hpp"
#include "ast/back_ast.hpp"
#include "frontend/parser/lexer.hpp"

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
    switch(node->type()) {""")
    for node in ast:
        print_ast_case(AstNode(node[0], node[1], node[2]))
    print(R"""        default:
            RAISE_INTERNAL_ERROR;
    }
}

void pretty_print_ast(Ast* node, const std::string& name) {
    header_string(name);
    print_ast(node, 0);
    std::cout << std::endl;
}

void pretty_print_symbol_table() {
    header_string("Symbol Table");
    std::cout << "\nDict(" + std::to_string(symbol_table.size()) + "):";
    for(const auto& symbol: symbol_table) {
        field("[" + symbol.first + "]", "", 2);
        print_ast(symbol.second.get(), 2);
    }
    std::cout << std::endl;
}

void pretty_print_struct_type_table() {
    header_string("Structure Type Table");
    std::cout << "\nDict(" + std::to_string(struct_type_table.size()) + "):";
    for(const auto& struct_type: struct_type_table) {
        field("[" + struct_type.first + "]", "", 2);
        print_ast(struct_type.second.get(), 2);
    }
    std::cout << std::endl;
}

void pretty_print_backend_symbol_table() {
    header_string("Backend Symbol Table");
    std::cout << "\nDict(" + std::to_string(backend_symbol_table.size()) + "):";
    for(const auto& symbol: backend_symbol_table) {
        field("[" + symbol.first + "]", "", 2);
        print_ast(symbol.second.get(), 2);
    }
    std::cout << std::endl;
}

void pretty_print_asm_code() {
    header_string("ASM Code");
    std::cout << std::endl;
}
#endif""")
