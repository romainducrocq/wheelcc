#include "intermediate/tac_gen.hpp"

// https://stackoverflow.com/questions/201718/concatenating-two-stdvectors

/** TODO
cdef TacBinaryOp represent_binary_op(CBinaryOp node):
    # binary_operator = Add | Subtract | Multiply | Divide | Remainder | BitAnd | BitOr | BitXor
    #                 | BitShiftLeft | BitShiftRight | Equal | NotEqual | LessThan | LessOrEqual
    #                 | GreaterThan | GreaterOrEqual
    if isinstance(node, CAdd):
        return TacAdd()
    elif isinstance(node, CSubtract):
        return TacSubtract()
    elif isinstance(node, CMultiply):
        return TacMultiply()
    elif isinstance(node, CDivide):
        return TacDivide()
    elif isinstance(node, CRemainder):
        return TacRemainder()
    elif isinstance(node, CBitAnd):
        return TacBitAnd()
    elif isinstance(node, CBitOr):
        return TacBitOr()
    elif isinstance(node, CBitXor):
        return TacBitXor()
    elif isinstance(node, CBitShiftLeft):
        return TacBitShiftLeft()
    elif isinstance(node, CBitShiftRight):
        return TacBitShiftRight()
    elif isinstance(node, CEqual):
        return TacEqual()
    elif isinstance(node, CNotEqual):
        return TacNotEqual()
    elif isinstance(node, CLessThan):
        return TacLessThan()
    elif isinstance(node, CLessOrEqual):
        return TacLessOrEqual()
    elif isinstance(node, CGreaterThan):
        return TacGreaterThan()
    elif isinstance(node, CGreaterOrEqual):
        return TacGreaterOrEqual()
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/

/** TODO
cdef TacUnaryOp represent_unary_op(CUnaryOp node):
    # unary_operator = Complement | Negate | Not
    if isinstance(node, CComplement):
        return TacComplement()
    elif isinstance(node, CNegate):
        return TacNegate()
    elif isinstance(node, CNot):
        return TacNot()
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/

/** TODO
cdef TacVariable represent_variable_value(CVar node):
    cdef TIdentifier name
    name = copy_identifier(node.name)
    return TacVariable(name)
*/

/** TODO
cdef TacConstant represent_constant_value(CConstant node):
    cdef CConst constant = node.constant
    return TacConstant(constant)
*/

/** TODO
cdef TacVariable represent_inner_exp_value(CExp node):
    cdef TIdentifier inner_name = represent_variable_identifier(node)
    cdef Type inner_type = node.exp_type
    cdef IdentifierAttr inner_attrs = LocalAttr()
    symbol_table[inner_name.str_t] = Symbol(inner_type, inner_attrs)
    return TacVariable(inner_name)
*/

/** TODO
cdef TacValue represent_inner_value(CExp node):
    return represent_inner_exp_value(node)
*/

/** TODO
cdef TacValue represent_value(CExp node):
    # val = Constant(int) | Var(identifier)
    if isinstance(node, CVar):
        return represent_variable_value(node)
    elif isinstance(node, CConstant):
        return represent_constant_value(node)
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/

/** TODO
cdef list[TacInstruction] instructions = []
*/

/** TODO
cdef TacConstant represent_exp_constant_instructions(CConstant node):
    return represent_value(node)
*/

/** TODO
cdef TacVariable represent_exp_var_instructions(CVar node):
    return represent_value(node)
*/

/** TODO
cdef TacValue represent_exp_fun_call_instructions(CFunctionCall node):
    cdef TIdentifier name = copy_identifier(node.name)
    cdef Py_ssize_t i
    cdef list[TacValue] args = []
    for i in range(len(node.args)):
        args.append(represent_exp_instructions(node.args[i]))
    cdef TacValue dst = represent_inner_value(node)
    instructions.append(TacFunCall(name, args, dst))
    return dst
*/

/** TODO
cdef TacValue represent_exp_cast_instructions(CCast node):
    cdef TacValue src = represent_exp_instructions(node.exp)
    if is_same_type(node.target_type, node.exp.exp_type):
        return src
    cdef TacValue dst = represent_inner_value(node)
    if isinstance(node.exp.exp_type, Double):
        if is_type_signed(node.target_type):
            instructions.append(TacDoubleToInt(src, dst))
        else:
            instructions.append(TacDoubleToUInt(src, dst))
        return dst
    elif isinstance(node.target_type, Double):
        if is_type_signed(node.exp.exp_type):
            instructions.append(TacIntToDouble(src, dst))
        else:
            instructions.append(TacUIntToDouble(src, dst))
        return dst
    cdef int32 target_type_size = get_type_size(node.target_type)
    cdef int32 inner_type_size = get_type_size(node.exp.exp_type)
    if target_type_size == inner_type_size:
        instructions.append(TacCopy(src, dst))
    elif target_type_size < inner_type_size:
        instructions.append(TacTruncate(src, dst))
    elif is_type_signed(node.exp.exp_type):
        instructions.append(TacSignExtend(src, dst))
    else:
        instructions.append(TacZeroExtend(src, dst))
    return dst
*/

/** TODO
cdef TacValue represent_exp_assignment_instructions(CAssignment node):
    cdef TacValue src = represent_exp_instructions(node.exp_right)
    cdef TacValue dst = represent_exp_instructions(node.exp_left)
    instructions.append(TacCopy(src, dst))
    return dst
*/

/** TODO
cdef TacValue represent_exp_assignment_compound_instructions(CAssignmentCompound node):
    cdef TacValue src1 = represent_exp_instructions(node.exp_left)
    cdef TacValue src2 = represent_exp_instructions(node.exp_right)
    cdef TacValue dst = represent_inner_value(node)
    cdef TacBinaryOp binary_op = represent_binary_op(node.binary_op)
    instructions.append(TacBinary(binary_op, src1, src2, dst))
    return dst
*/

/** TODO
cdef TacValue represent_exp_conditional_instructions(CConditional node):
    cdef TIdentifier target_else = represent_label_identifier("ternary_else")
    cdef TacValue condition = represent_exp_instructions(node.condition)
    instructions.append(TacJumpIfZero(condition, target_else))
    cdef TacValue src_middle = represent_exp_instructions(node.exp_middle)
    cdef TacValue dst = represent_inner_value(node)
    instructions.append(TacCopy(src_middle, dst))
    cdef TIdentifier target_false = represent_label_identifier("ternary_false")
    instructions.append(TacJump(target_false))
    instructions.append(TacLabel(target_else))
    cdef TacValue src_right = represent_exp_instructions(node.exp_right)
    instructions.append(TacCopy(src_right, dst))
    instructions.append(TacLabel(target_false))
    return dst
*/

/** TODO
cdef TacValue represent_exp_unary_instructions(CUnary node):
    cdef TacValue src = represent_exp_instructions(node.exp)
    cdef TacValue dst = represent_inner_value(node)
    cdef TacUnaryOp unary_op = represent_unary_op(node.unary_op)
    instructions.append(TacUnary(unary_op, src, dst))
    return dst
*/

/** TODO
cdef TacValue represent_exp_binary_and_instructions(CBinary node):
    cdef TIdentifier target_false = represent_label_identifier("and_false")
    cdef TacValue condition_left = represent_exp_instructions(node.exp_left)
    instructions.append(TacJumpIfZero(condition_left, target_false))
    cdef TacValue condition_right = represent_exp_instructions(node.exp_right)
    instructions.append(TacJumpIfZero(condition_right, target_false))
    cdef TacValue src_true = TacConstant(CConstInt(TInt(1)))
    cdef TacValue src_false = TacConstant(CConstInt(TInt(0)))
    cdef TIdentifier target_true = represent_label_identifier("and_true")
    cdef TacValue dst = represent_inner_value(node)
    instructions.append(TacCopy(src_true, dst))
    instructions.append(TacJump(target_true))
    instructions.append(TacLabel(target_false))
    instructions.append(TacCopy(src_false, dst))
    instructions.append(TacLabel(target_true))
    return dst
*/

/** TODO
cdef TacValue represent_exp_binary_or_instructions(CBinary node):
    cdef TIdentifier target_true = represent_label_identifier("or_true")
    cdef TacValue condition_left = represent_exp_instructions(node.exp_left)
    instructions.append(TacJumpIfNotZero(condition_left, target_true))
    cdef TacValue condition_right = represent_exp_instructions(node.exp_right)
    instructions.append(TacJumpIfNotZero(condition_right, target_true))
    cdef TacValue src_true = TacConstant(CConstInt(TInt(1)))
    cdef TacValue src_false = TacConstant(CConstInt(TInt(0)))
    cdef TIdentifier target_false = represent_label_identifier("or_false")
    cdef TacValue dst = represent_inner_value(node)
    instructions.append(TacCopy(src_false, dst))
    instructions.append(TacJump(target_false))
    instructions.append(TacLabel(target_true))
    instructions.append(TacCopy(src_true, dst))
    instructions.append(TacLabel(target_false))
    return dst
*/

/** TODO
cdef TacValue represent_exp_binary_instructions(CBinary node):
    cdef TacValue src1 = represent_exp_instructions(node.exp_left)
    cdef TacValue src2 = represent_exp_instructions(node.exp_right)
    cdef TacValue dst = represent_inner_value(node)
    cdef TacBinaryOp binary_op = represent_binary_op(node.binary_op)
    instructions.append(TacBinary(binary_op, src1, src2, dst))
    return dst
*/

/** TODO
cdef TacValue represent_exp_instructions(CExp node):
    if isinstance(node, CFunctionCall):
        return represent_exp_fun_call_instructions(node)
    elif isinstance(node, CVar):
        return represent_exp_var_instructions(node)
    elif isinstance(node, CConstant):
        return represent_exp_constant_instructions(node)
    elif isinstance(node, CCast):
        return represent_exp_cast_instructions(node)
    elif isinstance(node, CAssignment):
        return represent_exp_assignment_instructions(node)
    elif isinstance(node, CAssignmentCompound):
        return represent_exp_assignment_compound_instructions(node)
    elif isinstance(node, CConditional):
        return represent_exp_conditional_instructions(node)
    elif isinstance(node, CUnary):
        return represent_exp_unary_instructions(node)
    elif isinstance(node, CBinary):
        if isinstance(node.binary_op, CAnd):
            return represent_exp_binary_and_instructions(node)
        elif isinstance(node.binary_op, COr):
            return represent_exp_binary_or_instructions(node)
        elif isinstance(node.binary_op, (CEqual, CNotEqual, CLessThan, CLessOrEqual, CGreaterThan, CGreaterOrEqual,
                                       CAdd, CSubtract, CMultiply, CDivide, CRemainder, CBitAnd, CBitOr, CBitXor,
                                       CBitShiftLeft, CBitShiftRight)):
            return represent_exp_binary_instructions(node)
        else:

            raise RuntimeError(
                "An error occurred in three address code representation, not all nodes were visited")

    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/

/** TODO
cdef void represent_statement_null_instructions(CNull node):
    pass
*/

/** TODO
cdef void represent_statement_return_instructions(CReturn node):
    cdef TacValue val = represent_exp_instructions(node.exp)
    instructions.append(TacReturn(val))
*/

/** TODO
cdef void represent_statement_compound_instructions(CCompound node):
    represent_block(node.block)
*/

/** TODO
cdef void represent_statement_expression_instructions(CExpression node):
    _ = represent_exp_instructions(node.exp)
*/

/** TODO
cdef void represent_statement_if_instructions(CIf node):
    cdef TIdentifier target_false = represent_label_identifier("if_false")
    cdef TacValue condition = represent_exp_instructions(node.condition)
    instructions.append(TacJumpIfZero(condition, target_false))
    represent_statement_instructions(node.then)
    instructions.append(TacLabel(target_false))
*/

/** TODO
cdef void represent_statement_if_else_instructions(CIf node):
    cdef TIdentifier target_else = represent_label_identifier("if_else")
    cdef TacValue condition = represent_exp_instructions(node.condition)
    instructions.append(TacJumpIfZero(condition, target_else))
    represent_statement_instructions(node.then)
    cdef TIdentifier target_false = represent_label_identifier("if_false")
    instructions.append(TacJump(target_false))
    instructions.append(TacLabel(target_else))
    represent_statement_instructions(node.else_fi)
    instructions.append(TacLabel(target_false))
*/

/** TODO
cdef void represent_statement_while_instructions(CWhile node):
    cdef TIdentifier target_continue = TIdentifier("continue_" + node.target.str_t)
    instructions.append(TacLabel(target_continue))
    cdef TacValue condition = represent_exp_instructions(node.condition)
    cdef TIdentifier target_break = TIdentifier("break_" + node.target.str_t)
    instructions.append(TacJumpIfZero(condition, target_break))
    represent_statement_instructions(node.body)
    instructions.append(TacJump(target_continue))
    instructions.append(TacLabel(target_break))
*/

/** TODO
cdef void represent_statement_do_while_instructions(CDoWhile node):
    cdef TIdentifier target_do_while_start = represent_label_identifier("do_while_start")
    instructions.append(TacLabel(target_do_while_start))
    represent_statement_instructions(node.body)
    cdef TIdentifier target_continue = TIdentifier("continue_" + node.target.str_t)
    instructions.append(TacLabel(target_continue))
    cdef TacValue condition = represent_exp_instructions(node.condition)
    instructions.append(TacJumpIfNotZero(condition, target_do_while_start))
    cdef TIdentifier target_break = TIdentifier("break_" + node.target.str_t)
    instructions.append(TacLabel(target_break))
*/

/** TODO
cdef void represent_for_init_decl_instructions(CInitDecl node):
    represent_variable_declaration_instructions(node.init)
*/

/** TODO
cdef void represent_for_init_exp_instructions(CInitExp node):
    if node.init:
        _ = represent_exp_instructions(node.init)
*/

/** TODO
cdef void represent_statement_for_init_instructions(CForInit node):
    if isinstance(node, CInitDecl):
        represent_for_init_decl_instructions(node)
    elif isinstance(node, CInitExp):
        represent_for_init_exp_instructions(node)
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/

/** TODO
cdef void represent_statement_for_instructions(CFor node):
    represent_statement_for_init_instructions(node.init)
    cdef TIdentifier target_for_start = represent_label_identifier("for_start")
    instructions.append(TacLabel(target_for_start))
    cdef TIdentifier target_break = TIdentifier("break_" + node.target.str_t)
    cdef TacValue condition
    if node.condition:
        condition = represent_exp_instructions(node.condition)
        instructions.append(TacJumpIfZero(condition, target_break))
    represent_statement_instructions(node.body)
    cdef TIdentifier target_continue = TIdentifier("continue_" + node.target.str_t)
    instructions.append(TacLabel(target_continue))
    if node.post:
        _ = represent_exp_instructions(node.post)
    instructions.append(TacJump(target_for_start))
    instructions.append(TacLabel(target_break))
*/

/** TODO
cdef void represent_statement_break_instructions(CBreak node):
    cdef TIdentifier target_break = TIdentifier("break_" + node.target.str_t)
    instructions.append(TacJump(target_break))
*/

/** TODO
cdef void represent_statement_continue_instructions(CContinue node):
    cdef TIdentifier target_continue = TIdentifier("continue_" + node.target.str_t)
    instructions.append(TacJump(target_continue))
*/

/** TODO
cdef void represent_statement_goto_instructions(CGoto node):
    cdef TIdentifier target_label = node.target
    instructions.append(TacJump(target_label))
*/

/** TODO
cdef void represent_statement_label_instructions(CLabel node):
    cdef TIdentifier target_label = node.target
    instructions.append(TacLabel(target_label))
    represent_statement_instructions(node.jump_to)
*/

/** TODO
cdef void represent_statement_instructions(CStatement node):
    if isinstance(node, CNull):
        represent_statement_null_instructions(node)
    elif isinstance(node, CReturn):
        represent_statement_return_instructions(node)
    elif isinstance(node, CCompound):
        represent_statement_compound_instructions(node)
    elif isinstance(node, CExpression):
        represent_statement_expression_instructions(node)
    elif isinstance(node, CIf):
        if node.else_fi:
            represent_statement_if_else_instructions(node)
        else:
            represent_statement_if_instructions(node)
    elif isinstance(node, CWhile):
        represent_statement_while_instructions(node)
    elif isinstance(node, CDoWhile):
        represent_statement_do_while_instructions(node)
    elif isinstance(node, CFor):
        represent_statement_for_instructions(node)
    elif isinstance(node, CBreak):
        represent_statement_break_instructions(node)
    elif isinstance(node, CContinue):
        represent_statement_continue_instructions(node)
    elif isinstance(node, CGoto):
        represent_statement_goto_instructions(node)
    elif isinstance(node, CLabel):
        represent_statement_label_instructions(node)
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/

/** TODO
cdef void represent_variable_declaration_instructions(CVariableDeclaration node):
    cdef TacValue src = represent_exp_instructions(node.init)
    cdef TacValue dst = represent_value(CVar(node.name))
    instructions.append(TacCopy(src, dst))
*/

/** TODO
cdef void represent_declaration_var_decl_instructions(CVarDecl node):
    if isinstance(symbol_table[node.variable_decl.name.str_t].attrs, StaticAttr):
        return
    if node.variable_decl.init:
        represent_variable_declaration_instructions(node.variable_decl)
*/

/** TODO
cdef void represent_declaration_fun_decl_instructions(CFunDecl node):
    pass
*/

/** TODO
cdef void represent_declaration_instructions(CDeclaration node):
    if isinstance(node, CFunDecl):
        represent_declaration_fun_decl_instructions(node)
    elif isinstance(node, CVarDecl):
        represent_declaration_var_decl_instructions(node)
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/

/** TODO
cdef void represent_list_instructions(list[CBlockItem] list_node):
    # instruction = Return(val) | FunCall(identifier fun_name, val* args, val dst)
    #             | Unary(unary_operator, val src, val dst) | Binary(binary_operator, val src1, val src2, val dst)
    #             | Copy(val src, val dst) | Jump(identifier target) | JumpIfZero(val condition, identifier target)
    #             | JumpIfNotZero(val condition, identifier target) | Label(identifier name)

    cdef Py_ssize_t block_item
    for block_item in range(len(list_node)):
        if isinstance(list_node[block_item], CS):
            represent_statement_instructions(list_node[block_item].statement)
        elif isinstance(list_node[block_item], CD):
            represent_declaration_instructions(list_node[block_item].declaration)
        else:

            raise RuntimeError(
                "An error occurred in three address code representation, not all nodes were visited")
*/

/** TODO
cdef void represent_block(CBlock node):
    # block = Block(block_item* block_items)
    if isinstance(node, CB):
        represent_list_instructions(node.block_items)
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/

/** TODO
cdef TacFunction represent_function_top_level(CFunctionDeclaration node):
    global instructions

    cdef TIdentifier name = copy_identifier(node.name)
    cdef bint is_global = symbol_table[node.name.str_t].attrs.is_global
    cdef Py_ssize_t param
    cdef list[TIdentifier] params = []
    for param in range(len(node.params)):
        params.append(copy_identifier(node.params[param]))
    cdef list[TacInstruction] body = []
    instructions = body
    represent_block(node.body)
    instructions.append(TacReturn(TacConstant(CConstInt(TInt(0)))))
    return TacFunction(name, is_global, params, body)
*/

/** TODO
cdef list[TacTopLevel] function_top_levels = []
*/

/** TODO
cdef void represent_fun_decl_top_level(CFunDecl node):
    if node.function_decl.body:
        function_top_levels.append(represent_function_top_level(node.function_decl))
*/

/** TODO
cdef void represent_var_decl_top_level(CVarDecl node):
    pass
*/

/** TODO
cdef void represent_declaration_top_level(CDeclaration node):
    # top_level = Function(identifier, bool global, identifier* params, instruction* body)
    if isinstance(node, CFunDecl):
        represent_fun_decl_top_level(node)
    elif isinstance(node, CVarDecl):
        represent_var_decl_top_level(node)
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/

/** TODO
cdef list[TacTopLevel] static_variable_top_levels = []
*/

/** TODO
cdef StaticInit represent_tentative_static_init(Type static_init_type):
    if isinstance(static_init_type, Int):
        return IntInit(TInt(0))
    elif isinstance(static_init_type, Long):
        return LongInit(TLong(0))
    elif isinstance(static_init_type, Double):
        return DoubleInit(TDouble(0.0))
    elif isinstance(static_init_type, UInt):
        return UIntInit(TUInt(0))
    elif isinstance(static_init_type, ULong):
        return ULongInit(TULong(0))
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, not all nodes were visited")
*/

/** TODO
cdef void represent_static_variable_top_level(StaticAttr node, Type static_init_type, str symbol):
    if isinstance(node.init, NoInitializer):
        return

    cdef TIdentifier name = TIdentifier(symbol)
    cdef bint is_global = node.is_global
    cdef StaticInit initial_value
    if isinstance(node.init, Initial):
        initial_value = node.init.static_init
    elif isinstance(node.init, Tentative):
        initial_value = represent_tentative_static_init(static_init_type)
    else:

        raise RuntimeError(
            "An error occurred in three address code representation, top level variable has invalid initializer")

    static_variable_top_levels.append(TacStaticVariable(name, is_global, static_init_type, initial_value))
*/

/** TODO
cdef void represent_symbol_top_level(Symbol node, str symbol):
    # top_level = StaticVariable(identifier, bool global, int init)
    if isinstance(node.attrs, StaticAttr):
        represent_static_variable_top_level(node.attrs, node.type_t, symbol)
*/

/** TODO
cdef TacProgram represent_program(CProgram node):
    # program = Program(top_level*)
    global function_top_levels

    cdef list[TacTopLevel] top_levels = []
    function_top_levels = top_levels
    cdef Py_ssize_t declaration
    for declaration in range(len(node.declarations)):
        represent_declaration_top_level(node.declarations[declaration])

    static_variable_top_levels.clear()
    cdef str symbol
    for symbol in symbol_table:
        represent_symbol_top_level(symbol_table[symbol], symbol)
    top_levels = static_variable_top_levels + top_levels

    return TacProgram(top_levels)
*/

/** TODO
cdef TacProgram three_address_code_representation(CProgram c_ast):

    cdef TacProgram tac_ast = represent_program(c_ast)

    if not tac_ast:
        raise RuntimeError(
            "An error occurred in three address code representation, Asm was not generated")

    return tac_ast
*/
