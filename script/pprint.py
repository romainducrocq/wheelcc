#!/bin/python3

class TInt: name = "TInt"
class TLong: name = "TLong"
class TDouble: name = "TDouble"
class TUInt: name = "TUInt"
class TULong: name = "TULong"
class TIdentifier: name = "TIdentifier"
class Bool: name = "Bool"
class List: name = "List"

""" AST """ ############################################################################################################

ast = [
    # SYMBOL TABLE
    ["Type", [], []],
    ["Int", [], []],
    ["Long", [], []],
    ["Double", [], []],
    ["UInt", [], []],
    ["ULong", [], []],
    ["FunType", [], ["[param_types", "ret_type"]],
    ["StaticInit", [], []],
    ["IntInit", [(TInt, "value")], []],
    ["LongInit", [(TLong, "value")], []],
    ["DoubleInit", [(TDouble, "value")], []],
    ["UIntInit", [(TUInt, "value")], []],
    ["ULongInit", [(TULong, "value")], []],
    ["InitialValue", [], []],
    ["Tentative", [], []],
    ["Initial", [], ["static_init"]],
    ["NoInitializer", [], []],
    ["IdentifierAttr", [], []],
    ["FunAttr", [(Bool, "is_defined"), (Bool, "is_global")], []],
    ["StaticAttr", [(Bool, "is_global")], ["init"]],
    ["LocalAttr", [], []],
    ["Symbol", [], ["type_t", "attrs"]],

    # C AST
    ["CConst", [], []],
    ["CConstInt", [(TInt, "value")], []],
    ["CConstLong", [(TLong, "value")], []],
    ["CConstDouble", [(TDouble, "value")], []],
    ["CConstUInt", [(TUInt, "value")], []],
    ["CConstULong", [(TULong, "value")], []],
    ["CUnaryOp", [], []],
    ["CComplement", [], []],
    ["CNegate", [], []],
    ["CNot", [], []],
    ["CBinaryOp", [], []],
    ["CAdd", [], []],
    ["CSubtract", [], []],
    ["CMultiply", [], []],
    ["CDivide", [], []],
    ["CRemainder", [], []],
    ["CBitAnd", [], []],
    ["CBitOr", [], []],
    ["CBitXor", [], []],
    ["CBitShiftLeft", [], []],
    ["CBitShiftRight", [], []],
    ["CAnd", [], []],
    ["COr", [], []],
    ["CEqual", [], []],
    ["CNotEqual", [], []],
    ["CLessThan", [], []],
    ["CLessOrEqual", [], []],
    ["CGreaterThan", [], []],
    ["CGreaterOrEqual", [], []],
    ["CExp", [], ["exp_type"]],
    ["CConstant", [], ["constant", "exp_type"]],
    ["CVar", [(TIdentifier, "name")], ["exp_type"]],
    ["CCast", [], ["exp", "target_type", "exp_type"]],
    ["CUnary", [], ["unary_op", "exp", "exp_type"]],
    ["CBinary", [], ["binary_op", "exp_left", "exp_right", "exp_type"]],
    ["CAssignment", [], ["exp_left", "exp_right", "exp_type"]],
    ["CConditional", [], ["condition", "exp_middle", "exp_right", "exp_type"]],
    ["CAssignmentCompound", [], ["binary_op", "exp_left", "exp_right", "exp_type"]],
    ["CFunctionCall", [(TIdentifier, "name")], ["[args", "exp_type"]],
    ["CStatement", [], []],
    ["CReturn", [], ["exp"]],
    ["CExpression", [], ["exp"]],
    ["CIf", [], ["condition", "then", "else_fi"]],
    ["CGoto", [(TIdentifier, "target")], []],
    ["CLabel", [(TIdentifier, "target")], ["jump_to"]],
    ["CCompound", [], ["block"]],
    ["CWhile", [(TIdentifier, "target")], ["condition", "body"]],
    ["CDoWhile", [(TIdentifier, "target")], ["condition", "body"]],
    ["CFor", [(TIdentifier, "target")], ["init", "condition", "post", "body"]],
    ["CBreak", [(TIdentifier, "target")], []],
    ["CContinue", [(TIdentifier, "target")], []],
    ["CNull", [], []],
    ["CForInit", [], []],
    ["CInitDecl", [], ["init"]],
    ["CInitExp", [], ["init"]],
    ["CBlock", [], []],
    ["CB", [], ["[block_items"]],
    ["CBlockItem", [], []],
    ["CS", [], ["statement"]],
    ["CD", [], ["declaration"]],
    ["CStorageClass", [], []],
    ["CStatic", [], []],
    ["CExtern", [], []],
    ["CFunctionDeclaration", [(TIdentifier, "target"), (TIdentifier, "[params")], ["body", "fun_type", "storage_class"]],
    ["CVariableDeclaration", [(TIdentifier, "target")], ["init", "var_type", "storage_class"]],
    ["CDeclaration", [], []],
    ["CFunDecl", [], ["function_decl"]],
    ["CVarDecl", [], ["variable_decl"]],
    ["CProgram", [], ["[declarations"]],
]

########################################################################################################################


class AstNode:
    name = ""
    fields = []
    children = []

    def __init__(self, name, fields, children):
        self.name = name
        self.fields = fields
        self.children = children


def to_string(field_type, field_name):
    if field_type == "TIdentifier":
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
            print(f"            field(\"List\", \"\", t+1);")
            print(f"            for(const auto& item: p_node->{field[1][1:]}) {{")
            print(f"                field(\"{field[0]().name}\", {to_string(field[0]().name, 'item')}, t+2);")
            print(f"            }}")
        else:
            print(f"            field(\"{field[0]().name}\", {to_string(field[0]().name, 'p_node->' + field[1])}, t+1);")
    
    for child in node.children:
        if child[0] == "[":
            print(f"            field(\"List\", \"\", t+1);")
            print(f"            for(const auto& item: p_node->{child[1:]}) {{")
            print(f"                print_ast(item.get(), t+1);")
            print(f"            }}")
        else:
            print(f"            print_ast(p_node->{child}.get(), t);")
    
    print(f"            break;")
    print(f"        }}")


if __name__ == "__main__":
    print(R"""#include "util/pprint.hpp"
#include "util/error.hpp"
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
    switch(node->type()) {""")
    for node in ast:
        print_ast_case(AstNode(node[0], node[1], node[2]))
    print(R"""        default:
            raise_runtime_error("Pretty print not implemented for ast node type " + em(std::to_string(node->type())));
    }
}

void pretty_print_ast(Ast* node, const std::string& name) {
    header_string(name);
    print_ast(node, 0);
    std::cout << std::endl;
}""")    
