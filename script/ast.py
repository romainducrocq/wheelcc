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

    # /include/ast/symbol_table.hpp
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

    # /include/ast/c_ast.hpp
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
    ["CFunctionDeclaration", [(TIdentifier, "name"), (TIdentifier, "[params")], ["body", "fun_type", "storage_class"]],
    ["CVariableDeclaration", [(TIdentifier, "name")], ["init", "var_type", "storage_class"]],
    ["CDeclaration", [], []],
    ["CFunDecl", [], ["function_decl"]],
    ["CVarDecl", [], ["variable_decl"]],
    ["CProgram", [], ["[declarations"]],

    # /include/ast/tac_ast.hpp
    ["TacUnaryOp", [], []],
    ["TacComplement", [], []],
    ["TacNegate", [], []],
    ["TacNot", [], []],
    ["TacBinaryOp", [], []],
    ["TacAdd", [], []],
    ["TacSubtract", [], []],
    ["TacMultiply", [], []],
    ["TacDivide", [], []],
    ["TacRemainder", [], []],
    ["TacBitAnd", [], []],
    ["TacBitOr", [], []],
    ["TacBitXor", [], []],
    ["TacBitShiftLeft", [], []],
    ["TacBitShiftRight", [], []],
    ["TacEqual", [], []],
    ["TacNotEqual", [], []],
    ["TacLessThan", [], []],
    ["TacLessOrEqual", [], []],
    ["TacGreaterThan", [], []],
    ["TacGreaterOrEqual", [], []],
    ["TacValue", [], []],
    ["TacConstant", [], ["constant"]],
    ["TacVariable", [(TIdentifier, "name")], []],
    ["TacInstruction", [], []],
    ["TacReturn", [], ["val"]],
    ["TacSignExtend", [], ["src", "dst"]],
    ["TacTruncate", [], ["src", "dst"]],
    ["TacZeroExtend", [], ["src", "dst"]],
    ["TacDoubleToInt", [], ["src", "dst"]],
    ["TacDoubleToUInt", [], ["src", "dst"]],
    ["TacIntToDouble", [], ["src", "dst"]],
    ["TacUIntToDouble", [], ["src", "dst"]],
    ["TacFunCall", [(TIdentifier, "name")], ["[args", "dst"]],
    ["TacUnary", [], ["unary_op", "src", "dst"]],
    ["TacBinary", [], ["binary_op", "src1", "src2", "dst"]],
    ["TacCopy", [], ["src", "dst"]],
    ["TacJump", [(TIdentifier, "target")], []],
    ["TacJumpIfZero", [(TIdentifier, "target")], ["condition"]],
    ["TacJumpIfNotZero", [(TIdentifier, "target")], ["condition"]],
    ["TacLabel", [(TIdentifier, "name")], []],
    ["TacTopLevel", [], []],
    ["TacFunction", [(TIdentifier, "name"), (Bool, "is_global"), (TIdentifier, "[params")], ["[body"]],
    ["TacStaticVariable", [(TIdentifier, "name"), (Bool, "is_global")], ["static_init_type", "initial_value"]],
    ["TacProgram", [], ["[static_variable_top_levels", "[function_top_levels"]],
]

