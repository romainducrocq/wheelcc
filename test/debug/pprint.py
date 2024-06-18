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
        elif child[0] == "(":
            print(f"            field(\"Dict[\" + std::to_string(p_node->{child[1:]}.size()) + \"]\", \"\", t+1);")
            print(f"            for(const auto& item: p_node->{child[1:]}) {{")
            print(f"                field(\"[\" + item.first + \"]\", \"\", t+2);")
            print(f"                print_ast(item.second.get(), t+2);")
            print(f"            }}")
        else:
            print(f"            print_ast(p_node->{child}.get(), t);")
    
    print(f"            break;")
    print(f"        }}")


if __name__ == "__main__":
    print(R"""#include "util/pprint.hpp"
#ifndef __NDEBUG__
#include "util/str2t.hpp"
#include "util/throw.hpp"
#include "ast/ast.hpp"
#include "ast/front_symt.hpp"
#include "ast/back_symt.hpp"
#include "ast/front_ast.hpp"
#include "ast/interm_ast.hpp"
#include "ast/back_ast.hpp"
#include "frontend/parser/errors.hpp"
#include "frontend/parser/lexer.hpp"

#include <cstring>
#include <vector>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pretty print

static void header_string(const std::string& header) {
    std::cout << "+\n+\n@@ " << header << " @@";
}

void pretty_print_tokens(const std::vector<Token>& tokens) {
    header_string("Tokens");
    std::cout << "\nList[" << std::to_string(tokens.size()) << "]:";
    for(const auto& token: tokens) {
        std::cout << "\n  ";
        if(token.token.compare(get_token_kind_hr(token.token_kind)) == 0) {
            std::cout << token.token;
            continue;
        }
        std::cout << get_token_kind_hr(token.token_kind) << "(" << token.token << ")";
    }
    std::cout << std::endl;
}

static void field(const std::string& name, const std::string& value, size_t t) {
    std::cout << "\n";
    for(size_t i = 0; i < t - 1; ++i) {
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
    std::cout << "\nDict(" << std::to_string(frontend->symbol_table.size()) << "):";
    for(const auto& symbol: frontend->symbol_table) {
        field("[" + symbol.first + "]", "", 2);
        print_ast(symbol.second.get(), 2);
    }
    std::cout << std::endl;
}

void pretty_print_static_constant_table() {
    header_string("Static Constant Table");
    std::cout << "\nDict(" << std::to_string(frontend->static_constant_table.size()) << "):";
    for(const auto& static_constant: frontend->static_constant_table) {
        field("[" + static_constant.first + "]", "", 2);
        if(frontend->symbol_table.find(static_constant.second) != frontend->symbol_table.end() &&
           frontend->symbol_table[static_constant.second]->attrs->type() == AST_T::ConstantAttr_t) {
            ConstantAttr* constant_attr = static_cast<ConstantAttr*>(frontend->symbol_table[static_constant.second]->attrs.get());
            if(constant_attr->static_init->type() == AST_T::StringInit_t) {
                std::cout << "\n    string: \"";
                for(const TChar& byte : static_cast<StringInit*>(constant_attr->static_init.get())->literal.get()->value) {
                    switch(byte) {
                        case 39:
                            std::cout << "\\'";
                            break;
                        case 34:
                            std::cout << "\\\"";
                            break;
                        case 63:
                            std::cout << "\\?";
                            break;
                        case 92:
                            std::cout << "\\\\";
                            break;
                        case 7:
                            std::cout << "\\a";
                            break;
                        case 8:
                            std::cout << "\\b";
                            break;
                        case 12:
                            std::cout << "\\f";
                            break;
                        case 10:
                            std::cout << "\\n";
                            break;
                        case 13:
                            std::cout << "\\r";
                            break;
                        case 9:
                            std::cout << "\\t";
                            break;
                        case 11:
                            std::cout << "\\v";
                            break;
                        default:
                            std::cout << byte;
                            break;
                    }
                }
                std::cout << "\"";
                continue;
            }
        }
        else if(backend->backend_symbol_table.find(static_constant.second) != backend->backend_symbol_table.end() &&
                backend->backend_symbol_table[static_constant.second]->type() == AST_T::BackendObj_t){
            BackendObj* backend_obj = static_cast<BackendObj*>(backend->backend_symbol_table[static_constant.second].get());
            if(backend_obj->is_constant &&
               backend_obj->assembly_type->type() == AST_T::BackendDouble_t) {
                double decimal;
                uint64_t binary = string_to_uint64(static_constant.first);
                std::memcpy(&decimal, &binary, sizeof(double));
                std::cout << "\n    double: " << std::to_string(decimal);
                continue;
            }
        }
        RAISE_INTERNAL_ERROR;
    }
    std::cout << std::endl;
}

void pretty_print_struct_typedef_table() {
    header_string("Structure Typedef Table");
    std::cout << "\nDict(" << std::to_string(frontend->struct_typedef_table.size()) << "):";
    for(const auto& struct_typedef: frontend->struct_typedef_table) {
        field("[" + struct_typedef.first + "]", "", 2);
        print_ast(struct_typedef.second.get(), 2);
    }
    std::cout << std::endl;
}

void pretty_print_backend_symbol_table() {
    header_string("Backend Symbol Table");
    std::cout << "\nDict(" << std::to_string(backend->backend_symbol_table.size()) << "):";
    for(const auto& backend_symbol: backend->backend_symbol_table) {
        field("[" + backend_symbol.first + "]", "", 2);
        print_ast(backend_symbol.second.get(), 2);
    }
    std::cout << std::endl;
}

void pretty_print_asm_code() {
    header_string("ASM Code");
    std::cout << std::endl;
}
#endif""")
