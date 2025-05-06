#include "util/pprint.hpp"
#ifndef __NDEBUG__
#include <cstring>
#include <iostream>
#include <vector>

#include "util/str2t.hpp"
#include "util/throw.hpp"

#include "ast/ast.hpp"
#include "ast/back_ast.hpp"
#include "ast/back_symt.hpp"
#include "ast/front_ast.hpp"
#include "ast/front_symt.hpp"
#include "ast/interm_ast.hpp"

#include "frontend/parser/errors.hpp"
#include "frontend/parser/lexer.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Pretty print

static void print_title(const std::string& header) { std::cout << "+\n+\n@@ " << header << " @@"; }

void pprint_toks(const std::vector<Token>& tokens) {
    print_title("Tokens");
    std::cout << "\nList[" << std::to_string(tokens.size()) << "]:";
    for (const auto& tok : tokens) {
        std::cout << "\n  ";
        if (tok.tok.compare(get_tok_kind_fmt(tok.tok_kind)) == 0) {
            std::cout << tok.tok;
            continue;
        }
        std::cout << get_tok_kind_fmt(tok.tok_kind) << "(" << tok.tok << ")";
    }
    std::cout << std::endl;
}

static void print_field(const std::string& name, const std::string& value, size_t t) {
    std::cout << "\n";
    for (size_t i = 0; i < t - 1; ++i) {
        std::cout << "  ";
    }
    std::cout << name << ": ";
    if (!value.empty()) {
        std::cout << value;
    }
}

static void print_ast(Ast* node, size_t t) {
    if (!node) {
        print_field("None", "", ++t);
        return;
    }
    switch (node->type()) {
        case AST_CConst_t: {
            print_field("CConst", "", ++t);
            break;
        }
        case AST_CConstInt_t: {
            print_field("CConstInt", "", ++t);
            CConstInt* p_node = static_cast<CConstInt*>(node);
            print_field("TInt", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_CConstLong_t: {
            print_field("CConstLong", "", ++t);
            CConstLong* p_node = static_cast<CConstLong*>(node);
            print_field("TLong", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_CConstUInt_t: {
            print_field("CConstUInt", "", ++t);
            CConstUInt* p_node = static_cast<CConstUInt*>(node);
            print_field("TUInt", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_CConstULong_t: {
            print_field("CConstULong", "", ++t);
            CConstULong* p_node = static_cast<CConstULong*>(node);
            print_field("TULong", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_CConstDouble_t: {
            print_field("CConstDouble", "", ++t);
            CConstDouble* p_node = static_cast<CConstDouble*>(node);
            print_field("TDouble", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_CConstChar_t: {
            print_field("CConstChar", "", ++t);
            CConstChar* p_node = static_cast<CConstChar*>(node);
            print_field("TChar", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_CConstUChar_t: {
            print_field("CConstUChar", "", ++t);
            CConstUChar* p_node = static_cast<CConstUChar*>(node);
            print_field("TUChar", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_CStringLiteral_t: {
            print_field("CStringLiteral", "", ++t);
            CStringLiteral* p_node = static_cast<CStringLiteral*>(node);
            print_field("List[" + std::to_string(p_node->value.size()) + "]", "", t + 1);
            for (const auto& item : p_node->value) {
                print_field("TChar", std::to_string(item), t + 2);
            }
            break;
        }
        case AST_Type_t: {
            print_field("Type", "", ++t);
            break;
        }
        case AST_Char_t: {
            print_field("Char", "", ++t);
            break;
        }
        case AST_SChar_t: {
            print_field("SChar", "", ++t);
            break;
        }
        case AST_UChar_t: {
            print_field("UChar", "", ++t);
            break;
        }
        case AST_Int_t: {
            print_field("Int", "", ++t);
            break;
        }
        case AST_Long_t: {
            print_field("Long", "", ++t);
            break;
        }
        case AST_UInt_t: {
            print_field("UInt", "", ++t);
            break;
        }
        case AST_ULong_t: {
            print_field("ULong", "", ++t);
            break;
        }
        case AST_Double_t: {
            print_field("Double", "", ++t);
            break;
        }
        case AST_Void_t: {
            print_field("Void", "", ++t);
            break;
        }
        case AST_FunType_t: {
            print_field("FunType", "", ++t);
            FunType* p_node = static_cast<FunType*>(node);
            print_field("TULong", std::to_string(p_node->param_reg_mask), t + 1);
            print_field("TULong", std::to_string(p_node->ret_reg_mask), t + 1);
            print_field("List[" + std::to_string(p_node->param_types.size()) + "]", "", t + 1);
            for (const auto& item : p_node->param_types) {
                print_ast(item.get(), t + 1);
            }
            print_ast(p_node->ret_type.get(), t);
            break;
        }
        case AST_Pointer_t: {
            print_field("Pointer", "", ++t);
            Pointer* p_node = static_cast<Pointer*>(node);
            print_ast(p_node->ref_type.get(), t);
            break;
        }
        case AST_Array_t: {
            print_field("Array", "", ++t);
            Array* p_node = static_cast<Array*>(node);
            print_field("TLong", std::to_string(p_node->size), t + 1);
            print_ast(p_node->elem_type.get(), t);
            break;
        }
        case AST_Structure_t: {
            print_field("Structure", "", ++t);
            Structure* p_node = static_cast<Structure*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->tag], t + 1);
            print_field("Bool", std::to_string(p_node->is_union), t + 1);
            break;
        }
        case AST_StaticInit_t: {
            print_field("StaticInit", "", ++t);
            break;
        }
        case AST_IntInit_t: {
            print_field("IntInit", "", ++t);
            IntInit* p_node = static_cast<IntInit*>(node);
            print_field("TInt", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_LongInit_t: {
            print_field("LongInit", "", ++t);
            LongInit* p_node = static_cast<LongInit*>(node);
            print_field("TLong", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_UIntInit_t: {
            print_field("UIntInit", "", ++t);
            UIntInit* p_node = static_cast<UIntInit*>(node);
            print_field("TUInt", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_ULongInit_t: {
            print_field("ULongInit", "", ++t);
            ULongInit* p_node = static_cast<ULongInit*>(node);
            print_field("TULong", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_CharInit_t: {
            print_field("CharInit", "", ++t);
            CharInit* p_node = static_cast<CharInit*>(node);
            print_field("TChar", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_UCharInit_t: {
            print_field("UCharInit", "", ++t);
            UCharInit* p_node = static_cast<UCharInit*>(node);
            print_field("TUChar", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_DoubleInit_t: {
            print_field("DoubleInit", "", ++t);
            DoubleInit* p_node = static_cast<DoubleInit*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->dbl_const], t + 1);
            break;
        }
        case AST_ZeroInit_t: {
            print_field("ZeroInit", "", ++t);
            ZeroInit* p_node = static_cast<ZeroInit*>(node);
            print_field("TLong", std::to_string(p_node->byte), t + 1);
            break;
        }
        case AST_StringInit_t: {
            print_field("StringInit", "", ++t);
            StringInit* p_node = static_cast<StringInit*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->string_const], t + 1);
            print_field("Bool", std::to_string(p_node->is_null_term), t + 1);
            print_ast(p_node->literal.get(), t);
            break;
        }
        case AST_PointerInit_t: {
            print_field("PointerInit", "", ++t);
            PointerInit* p_node = static_cast<PointerInit*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            break;
        }
        case AST_InitialValue_t: {
            print_field("InitialValue", "", ++t);
            break;
        }
        case AST_Tentative_t: {
            print_field("Tentative", "", ++t);
            break;
        }
        case AST_Initial_t: {
            print_field("Initial", "", ++t);
            Initial* p_node = static_cast<Initial*>(node);
            print_field("List[" + std::to_string(p_node->static_inits.size()) + "]", "", t + 1);
            for (const auto& item : p_node->static_inits) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_NoInitializer_t: {
            print_field("NoInitializer", "", ++t);
            break;
        }
        case AST_IdentifierAttr_t: {
            print_field("IdentifierAttr", "", ++t);
            break;
        }
        case AST_FunAttr_t: {
            print_field("FunAttr", "", ++t);
            FunAttr* p_node = static_cast<FunAttr*>(node);
            print_field("Bool", std::to_string(p_node->is_def), t + 1);
            print_field("Bool", std::to_string(p_node->is_glob), t + 1);
            break;
        }
        case AST_StaticAttr_t: {
            print_field("StaticAttr", "", ++t);
            StaticAttr* p_node = static_cast<StaticAttr*>(node);
            print_field("Bool", std::to_string(p_node->is_glob), t + 1);
            print_ast(p_node->init.get(), t);
            break;
        }
        case AST_ConstantAttr_t: {
            print_field("ConstantAttr", "", ++t);
            ConstantAttr* p_node = static_cast<ConstantAttr*>(node);
            print_ast(p_node->static_init.get(), t);
            break;
        }
        case AST_LocalAttr_t: {
            print_field("LocalAttr", "", ++t);
            break;
        }
        case AST_Symbol_t: {
            print_field("Symbol", "", ++t);
            Symbol* p_node = static_cast<Symbol*>(node);
            print_ast(p_node->type_t.get(), t);
            print_ast(p_node->attrs.get(), t);
            break;
        }
        case AST_StructMember_t: {
            print_field("StructMember", "", ++t);
            StructMember* p_node = static_cast<StructMember*>(node);
            print_field("TLong", std::to_string(p_node->offset), t + 1);
            print_ast(p_node->member_type.get(), t);
            break;
        }
        case AST_StructTypedef_t: {
            print_field("StructTypedef", "", ++t);
            StructTypedef* p_node = static_cast<StructTypedef*>(node);
            print_field("TInt", std::to_string(p_node->alignment), t + 1);
            print_field("TLong", std::to_string(p_node->size), t + 1);
            print_field("List[" + std::to_string(p_node->member_names.size()) + "]", "", t + 1);
            for (const auto& item : p_node->member_names) {
                print_field("TIdentifier", identifiers->hash_table[item], t + 2);
            }
            print_field("Dict[" + std::to_string(p_node->members.size()) + "]", "", t + 1);
            for (const auto& item : p_node->members) {
                print_field("[" + identifiers->hash_table[item.first] + "]", "", t + 2);
                print_ast(item.second.get(), t + 2);
            }
            break;
        }
        case AST_AssemblyType_t: {
            print_field("AssemblyType", "", ++t);
            break;
        }
        case AST_Byte_t: {
            print_field("Byte", "", ++t);
            break;
        }
        case AST_LongWord_t: {
            print_field("LongWord", "", ++t);
            break;
        }
        case AST_QuadWord_t: {
            print_field("QuadWord", "", ++t);
            break;
        }
        case AST_BackendDouble_t: {
            print_field("BackendDouble", "", ++t);
            break;
        }
        case AST_ByteArray_t: {
            print_field("ByteArray", "", ++t);
            ByteArray* p_node = static_cast<ByteArray*>(node);
            print_field("TLong", std::to_string(p_node->size), t + 1);
            print_field("TInt", std::to_string(p_node->alignment), t + 1);
            break;
        }
        case AST_BackendSymbol_t: {
            print_field("BackendSymbol", "", ++t);
            break;
        }
        case AST_BackendObj_t: {
            print_field("BackendObj", "", ++t);
            BackendObj* p_node = static_cast<BackendObj*>(node);
            print_field("Bool", std::to_string(p_node->is_static), t + 1);
            print_field("Bool", std::to_string(p_node->is_const), t + 1);
            print_ast(p_node->asm_type.get(), t);
            break;
        }
        case AST_BackendFun_t: {
            print_field("BackendFun", "", ++t);
            BackendFun* p_node = static_cast<BackendFun*>(node);
            print_field("Bool", std::to_string(p_node->is_def), t + 1);
            print_field("List[" + std::to_string(p_node->callee_saved_regs.size()) + "]", "", t + 1);
            for (const auto& item : p_node->callee_saved_regs) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_CUnaryOp_t: {
            print_field("CUnaryOp", "", ++t);
            break;
        }
        case AST_CComplement_t: {
            print_field("CComplement", "", ++t);
            break;
        }
        case AST_CNegate_t: {
            print_field("CNegate", "", ++t);
            break;
        }
        case AST_CNot_t: {
            print_field("CNot", "", ++t);
            break;
        }
        case AST_CPrefix_t: {
            print_field("CPrefix", "", ++t);
            break;
        }
        case AST_CPostfix_t: {
            print_field("CPostfix", "", ++t);
            break;
        }
        case AST_CBinaryOp_t: {
            print_field("CBinaryOp", "", ++t);
            break;
        }
        case AST_CAdd_t: {
            print_field("CAdd", "", ++t);
            break;
        }
        case AST_CSubtract_t: {
            print_field("CSubtract", "", ++t);
            break;
        }
        case AST_CMultiply_t: {
            print_field("CMultiply", "", ++t);
            break;
        }
        case AST_CDivide_t: {
            print_field("CDivide", "", ++t);
            break;
        }
        case AST_CRemainder_t: {
            print_field("CRemainder", "", ++t);
            break;
        }
        case AST_CBitAnd_t: {
            print_field("CBitAnd", "", ++t);
            break;
        }
        case AST_CBitOr_t: {
            print_field("CBitOr", "", ++t);
            break;
        }
        case AST_CBitXor_t: {
            print_field("CBitXor", "", ++t);
            break;
        }
        case AST_CBitShiftLeft_t: {
            print_field("CBitShiftLeft", "", ++t);
            break;
        }
        case AST_CBitShiftRight_t: {
            print_field("CBitShiftRight", "", ++t);
            break;
        }
        case AST_CBitShrArithmetic_t: {
            print_field("CBitShrArithmetic", "", ++t);
            break;
        }
        case AST_CAnd_t: {
            print_field("CAnd", "", ++t);
            break;
        }
        case AST_COr_t: {
            print_field("COr", "", ++t);
            break;
        }
        case AST_CEqual_t: {
            print_field("CEqual", "", ++t);
            break;
        }
        case AST_CNotEqual_t: {
            print_field("CNotEqual", "", ++t);
            break;
        }
        case AST_CLessThan_t: {
            print_field("CLessThan", "", ++t);
            break;
        }
        case AST_CLessOrEqual_t: {
            print_field("CLessOrEqual", "", ++t);
            break;
        }
        case AST_CGreaterThan_t: {
            print_field("CGreaterThan", "", ++t);
            break;
        }
        case AST_CGreaterOrEqual_t: {
            print_field("CGreaterOrEqual", "", ++t);
            break;
        }
        case AST_CAbstractDeclarator_t: {
            print_field("CAbstractDeclarator", "", ++t);
            break;
        }
        case AST_CAbstractPointer_t: {
            print_field("CAbstractPointer", "", ++t);
            CAbstractPointer* p_node = static_cast<CAbstractPointer*>(node);
            print_ast(p_node->abstract_decltor.get(), t);
            break;
        }
        case AST_CAbstractArray_t: {
            print_field("CAbstractArray", "", ++t);
            CAbstractArray* p_node = static_cast<CAbstractArray*>(node);
            print_field("TLong", std::to_string(p_node->size), t + 1);
            print_ast(p_node->abstract_decltor.get(), t);
            break;
        }
        case AST_CAbstractBase_t: {
            print_field("CAbstractBase", "", ++t);
            break;
        }
        case AST_CParam_t: {
            print_field("CParam", "", ++t);
            CParam* p_node = static_cast<CParam*>(node);
            print_ast(p_node->decltor.get(), t);
            print_ast(p_node->param_type.get(), t);
            break;
        }
        case AST_CDeclarator_t: {
            print_field("CDeclarator", "", ++t);
            break;
        }
        case AST_CIdent_t: {
            print_field("CIdent", "", ++t);
            CIdent* p_node = static_cast<CIdent*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            break;
        }
        case AST_CPointerDeclarator_t: {
            print_field("CPointerDeclarator", "", ++t);
            CPointerDeclarator* p_node = static_cast<CPointerDeclarator*>(node);
            print_ast(p_node->decltor.get(), t);
            break;
        }
        case AST_CArrayDeclarator_t: {
            print_field("CArrayDeclarator", "", ++t);
            CArrayDeclarator* p_node = static_cast<CArrayDeclarator*>(node);
            print_field("TLong", std::to_string(p_node->size), t + 1);
            print_ast(p_node->decltor.get(), t);
            break;
        }
        case AST_CFunDeclarator_t: {
            print_field("CFunDeclarator", "", ++t);
            CFunDeclarator* p_node = static_cast<CFunDeclarator*>(node);
            print_field("List[" + std::to_string(p_node->param_list.size()) + "]", "", t + 1);
            for (const auto& item : p_node->param_list) {
                print_ast(item.get(), t + 1);
            }
            print_ast(p_node->decltor.get(), t);
            break;
        }
        case AST_CExp_t: {
            print_field("CExp", "", ++t);
            CExp* p_node = static_cast<CExp*>(node);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CConstant_t: {
            print_field("CConstant", "", ++t);
            CConstant* p_node = static_cast<CConstant*>(node);
            print_ast(p_node->constant.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CString_t: {
            print_field("CString", "", ++t);
            CString* p_node = static_cast<CString*>(node);
            print_ast(p_node->literal.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CVar_t: {
            print_field("CVar", "", ++t);
            CVar* p_node = static_cast<CVar*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CCast_t: {
            print_field("CCast", "", ++t);
            CCast* p_node = static_cast<CCast*>(node);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->target_type.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CUnary_t: {
            print_field("CUnary", "", ++t);
            CUnary* p_node = static_cast<CUnary*>(node);
            print_ast(p_node->unop.get(), t);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CBinary_t: {
            print_field("CBinary", "", ++t);
            CBinary* p_node = static_cast<CBinary*>(node);
            print_ast(p_node->binop.get(), t);
            print_ast(p_node->exp_left.get(), t);
            print_ast(p_node->exp_right.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CAssignment_t: {
            print_field("CAssignment", "", ++t);
            CAssignment* p_node = static_cast<CAssignment*>(node);
            print_ast(p_node->unop.get(), t);
            print_ast(p_node->exp_left.get(), t);
            print_ast(p_node->exp_right.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CConditional_t: {
            print_field("CConditional", "", ++t);
            CConditional* p_node = static_cast<CConditional*>(node);
            print_ast(p_node->condition.get(), t);
            print_ast(p_node->exp_middle.get(), t);
            print_ast(p_node->exp_right.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CFunctionCall_t: {
            print_field("CFunctionCall", "", ++t);
            CFunctionCall* p_node = static_cast<CFunctionCall*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("List[" + std::to_string(p_node->args.size()) + "]", "", t + 1);
            for (const auto& item : p_node->args) {
                print_ast(item.get(), t + 1);
            }
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CDereference_t: {
            print_field("CDereference", "", ++t);
            CDereference* p_node = static_cast<CDereference*>(node);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CAddrOf_t: {
            print_field("CAddrOf", "", ++t);
            CAddrOf* p_node = static_cast<CAddrOf*>(node);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CSubscript_t: {
            print_field("CSubscript", "", ++t);
            CSubscript* p_node = static_cast<CSubscript*>(node);
            print_ast(p_node->primary_exp.get(), t);
            print_ast(p_node->subscript_exp.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CSizeOf_t: {
            print_field("CSizeOf", "", ++t);
            CSizeOf* p_node = static_cast<CSizeOf*>(node);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CSizeOfT_t: {
            print_field("CSizeOfT", "", ++t);
            CSizeOfT* p_node = static_cast<CSizeOfT*>(node);
            print_ast(p_node->target_type.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CDot_t: {
            print_field("CDot", "", ++t);
            CDot* p_node = static_cast<CDot*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->member], t + 1);
            print_ast(p_node->structure.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CArrow_t: {
            print_field("CArrow", "", ++t);
            CArrow* p_node = static_cast<CArrow*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->member], t + 1);
            print_ast(p_node->pointer.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_CStatement_t: {
            print_field("CStatement", "", ++t);
            break;
        }
        case AST_CReturn_t: {
            print_field("CReturn", "", ++t);
            CReturn* p_node = static_cast<CReturn*>(node);
            print_ast(p_node->exp.get(), t);
            break;
        }
        case AST_CExpression_t: {
            print_field("CExpression", "", ++t);
            CExpression* p_node = static_cast<CExpression*>(node);
            print_ast(p_node->exp.get(), t);
            break;
        }
        case AST_CIf_t: {
            print_field("CIf", "", ++t);
            CIf* p_node = static_cast<CIf*>(node);
            print_ast(p_node->condition.get(), t);
            print_ast(p_node->then.get(), t);
            print_ast(p_node->else_fi.get(), t);
            break;
        }
        case AST_CGoto_t: {
            print_field("CGoto", "", ++t);
            CGoto* p_node = static_cast<CGoto*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            break;
        }
        case AST_CLabel_t: {
            print_field("CLabel", "", ++t);
            CLabel* p_node = static_cast<CLabel*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->jump_to.get(), t);
            break;
        }
        case AST_CCompound_t: {
            print_field("CCompound", "", ++t);
            CCompound* p_node = static_cast<CCompound*>(node);
            print_ast(p_node->block.get(), t);
            break;
        }
        case AST_CWhile_t: {
            print_field("CWhile", "", ++t);
            CWhile* p_node = static_cast<CWhile*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->condition.get(), t);
            print_ast(p_node->body.get(), t);
            break;
        }
        case AST_CDoWhile_t: {
            print_field("CDoWhile", "", ++t);
            CDoWhile* p_node = static_cast<CDoWhile*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->condition.get(), t);
            print_ast(p_node->body.get(), t);
            break;
        }
        case AST_CFor_t: {
            print_field("CFor", "", ++t);
            CFor* p_node = static_cast<CFor*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->init.get(), t);
            print_ast(p_node->condition.get(), t);
            print_ast(p_node->post.get(), t);
            print_ast(p_node->body.get(), t);
            break;
        }
        case AST_CSwitch_t: {
            print_field("CSwitch", "", ++t);
            CSwitch* p_node = static_cast<CSwitch*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_field("Bool", std::to_string(p_node->is_default), t + 1);
            print_ast(p_node->match.get(), t);
            print_ast(p_node->body.get(), t);
            print_field("List[" + std::to_string(p_node->cases.size()) + "]", "", t + 1);
            for (const auto& item : p_node->cases) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_CCase_t: {
            print_field("CCase", "", ++t);
            CCase* p_node = static_cast<CCase*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->value.get(), t);
            print_ast(p_node->jump_to.get(), t);
            break;
        }
        case AST_CDefault_t: {
            print_field("CDefault", "", ++t);
            CDefault* p_node = static_cast<CDefault*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->jump_to.get(), t);
            break;
        }
        case AST_CBreak_t: {
            print_field("CBreak", "", ++t);
            CBreak* p_node = static_cast<CBreak*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            break;
        }
        case AST_CContinue_t: {
            print_field("CContinue", "", ++t);
            CContinue* p_node = static_cast<CContinue*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            break;
        }
        case AST_CNull_t: {
            print_field("CNull", "", ++t);
            break;
        }
        case AST_CForInit_t: {
            print_field("CForInit", "", ++t);
            break;
        }
        case AST_CInitDecl_t: {
            print_field("CInitDecl", "", ++t);
            CInitDecl* p_node = static_cast<CInitDecl*>(node);
            print_ast(p_node->init.get(), t);
            break;
        }
        case AST_CInitExp_t: {
            print_field("CInitExp", "", ++t);
            CInitExp* p_node = static_cast<CInitExp*>(node);
            print_ast(p_node->init.get(), t);
            break;
        }
        case AST_CBlock_t: {
            print_field("CBlock", "", ++t);
            break;
        }
        case AST_CB_t: {
            print_field("CB", "", ++t);
            CB* p_node = static_cast<CB*>(node);
            print_field("List[" + std::to_string(p_node->block_items.size()) + "]", "", t + 1);
            for (const auto& item : p_node->block_items) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_CBlockItem_t: {
            print_field("CBlockItem", "", ++t);
            break;
        }
        case AST_CS_t: {
            print_field("CS", "", ++t);
            CS* p_node = static_cast<CS*>(node);
            print_ast(p_node->statement.get(), t);
            break;
        }
        case AST_CD_t: {
            print_field("CD", "", ++t);
            CD* p_node = static_cast<CD*>(node);
            print_ast(p_node->declaration.get(), t);
            break;
        }
        case AST_CStorageClass_t: {
            print_field("CStorageClass", "", ++t);
            break;
        }
        case AST_CStatic_t: {
            print_field("CStatic", "", ++t);
            break;
        }
        case AST_CExtern_t: {
            print_field("CExtern", "", ++t);
            break;
        }
        case AST_CInitializer_t: {
            print_field("CInitializer", "", ++t);
            CInitializer* p_node = static_cast<CInitializer*>(node);
            print_ast(p_node->init_type.get(), t);
            break;
        }
        case AST_CSingleInit_t: {
            print_field("CSingleInit", "", ++t);
            CSingleInit* p_node = static_cast<CSingleInit*>(node);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->init_type.get(), t);
            break;
        }
        case AST_CCompoundInit_t: {
            print_field("CCompoundInit", "", ++t);
            CCompoundInit* p_node = static_cast<CCompoundInit*>(node);
            print_field("List[" + std::to_string(p_node->initializers.size()) + "]", "", t + 1);
            for (const auto& item : p_node->initializers) {
                print_ast(item.get(), t + 1);
            }
            print_ast(p_node->init_type.get(), t);
            break;
        }
        case AST_CMemberDeclaration_t: {
            print_field("CMemberDeclaration", "", ++t);
            CMemberDeclaration* p_node = static_cast<CMemberDeclaration*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->member_name], t + 1);
            print_ast(p_node->member_type.get(), t);
            break;
        }
        case AST_CStructDeclaration_t: {
            print_field("CStructDeclaration", "", ++t);
            CStructDeclaration* p_node = static_cast<CStructDeclaration*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->tag], t + 1);
            print_field("Bool", std::to_string(p_node->is_union), t + 1);
            print_field("List[" + std::to_string(p_node->members.size()) + "]", "", t + 1);
            for (const auto& item : p_node->members) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_CFunctionDeclaration_t: {
            print_field("CFunctionDeclaration", "", ++t);
            CFunctionDeclaration* p_node = static_cast<CFunctionDeclaration*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("List[" + std::to_string(p_node->params.size()) + "]", "", t + 1);
            for (const auto& item : p_node->params) {
                print_field("TIdentifier", identifiers->hash_table[item], t + 2);
            }
            print_ast(p_node->body.get(), t);
            print_ast(p_node->fun_type.get(), t);
            print_ast(p_node->storage_class.get(), t);
            break;
        }
        case AST_CVariableDeclaration_t: {
            print_field("CVariableDeclaration", "", ++t);
            CVariableDeclaration* p_node = static_cast<CVariableDeclaration*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_ast(p_node->init.get(), t);
            print_ast(p_node->var_type.get(), t);
            print_ast(p_node->storage_class.get(), t);
            break;
        }
        case AST_CDeclaration_t: {
            print_field("CDeclaration", "", ++t);
            break;
        }
        case AST_CFunDecl_t: {
            print_field("CFunDecl", "", ++t);
            CFunDecl* p_node = static_cast<CFunDecl*>(node);
            print_ast(p_node->fun_decl.get(), t);
            break;
        }
        case AST_CVarDecl_t: {
            print_field("CVarDecl", "", ++t);
            CVarDecl* p_node = static_cast<CVarDecl*>(node);
            print_ast(p_node->var_decl.get(), t);
            break;
        }
        case AST_CStructDecl_t: {
            print_field("CStructDecl", "", ++t);
            CStructDecl* p_node = static_cast<CStructDecl*>(node);
            print_ast(p_node->struct_decl.get(), t);
            break;
        }
        case AST_CProgram_t: {
            print_field("CProgram", "", ++t);
            CProgram* p_node = static_cast<CProgram*>(node);
            print_field("List[" + std::to_string(p_node->declarations.size()) + "]", "", t + 1);
            for (const auto& item : p_node->declarations) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_TacUnaryOp_t: {
            print_field("TacUnaryOp", "", ++t);
            break;
        }
        case AST_TacComplement_t: {
            print_field("TacComplement", "", ++t);
            break;
        }
        case AST_TacNegate_t: {
            print_field("TacNegate", "", ++t);
            break;
        }
        case AST_TacNot_t: {
            print_field("TacNot", "", ++t);
            break;
        }
        case AST_TacBinaryOp_t: {
            print_field("TacBinaryOp", "", ++t);
            break;
        }
        case AST_TacAdd_t: {
            print_field("TacAdd", "", ++t);
            break;
        }
        case AST_TacSubtract_t: {
            print_field("TacSubtract", "", ++t);
            break;
        }
        case AST_TacMultiply_t: {
            print_field("TacMultiply", "", ++t);
            break;
        }
        case AST_TacDivide_t: {
            print_field("TacDivide", "", ++t);
            break;
        }
        case AST_TacRemainder_t: {
            print_field("TacRemainder", "", ++t);
            break;
        }
        case AST_TacBitAnd_t: {
            print_field("TacBitAnd", "", ++t);
            break;
        }
        case AST_TacBitOr_t: {
            print_field("TacBitOr", "", ++t);
            break;
        }
        case AST_TacBitXor_t: {
            print_field("TacBitXor", "", ++t);
            break;
        }
        case AST_TacBitShiftLeft_t: {
            print_field("TacBitShiftLeft", "", ++t);
            break;
        }
        case AST_TacBitShiftRight_t: {
            print_field("TacBitShiftRight", "", ++t);
            break;
        }
        case AST_TacBitShrArithmetic_t: {
            print_field("TacBitShrArithmetic", "", ++t);
            break;
        }
        case AST_TacEqual_t: {
            print_field("TacEqual", "", ++t);
            break;
        }
        case AST_TacNotEqual_t: {
            print_field("TacNotEqual", "", ++t);
            break;
        }
        case AST_TacLessThan_t: {
            print_field("TacLessThan", "", ++t);
            break;
        }
        case AST_TacLessOrEqual_t: {
            print_field("TacLessOrEqual", "", ++t);
            break;
        }
        case AST_TacGreaterThan_t: {
            print_field("TacGreaterThan", "", ++t);
            break;
        }
        case AST_TacGreaterOrEqual_t: {
            print_field("TacGreaterOrEqual", "", ++t);
            break;
        }
        case AST_TacValue_t: {
            print_field("TacValue", "", ++t);
            break;
        }
        case AST_TacConstant_t: {
            print_field("TacConstant", "", ++t);
            TacConstant* p_node = static_cast<TacConstant*>(node);
            print_ast(p_node->constant.get(), t);
            break;
        }
        case AST_TacVariable_t: {
            print_field("TacVariable", "", ++t);
            TacVariable* p_node = static_cast<TacVariable*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            break;
        }
        case AST_TacExpResult_t: {
            print_field("TacExpResult", "", ++t);
            break;
        }
        case AST_TacPlainOperand_t: {
            print_field("TacPlainOperand", "", ++t);
            TacPlainOperand* p_node = static_cast<TacPlainOperand*>(node);
            print_ast(p_node->val.get(), t);
            break;
        }
        case AST_TacDereferencedPointer_t: {
            print_field("TacDereferencedPointer", "", ++t);
            TacDereferencedPointer* p_node = static_cast<TacDereferencedPointer*>(node);
            print_ast(p_node->val.get(), t);
            break;
        }
        case AST_TacSubObject_t: {
            print_field("TacSubObject", "", ++t);
            TacSubObject* p_node = static_cast<TacSubObject*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->base_name], t + 1);
            print_field("TLong", std::to_string(p_node->offset), t + 1);
            break;
        }
        case AST_TacInstruction_t: {
            print_field("TacInstruction", "", ++t);
            break;
        }
        case AST_TacReturn_t: {
            print_field("TacReturn", "", ++t);
            TacReturn* p_node = static_cast<TacReturn*>(node);
            print_ast(p_node->val.get(), t);
            break;
        }
        case AST_TacSignExtend_t: {
            print_field("TacSignExtend", "", ++t);
            TacSignExtend* p_node = static_cast<TacSignExtend*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_TacTruncate_t: {
            print_field("TacTruncate", "", ++t);
            TacTruncate* p_node = static_cast<TacTruncate*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_TacZeroExtend_t: {
            print_field("TacZeroExtend", "", ++t);
            TacZeroExtend* p_node = static_cast<TacZeroExtend*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_TacDoubleToInt_t: {
            print_field("TacDoubleToInt", "", ++t);
            TacDoubleToInt* p_node = static_cast<TacDoubleToInt*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_TacDoubleToUInt_t: {
            print_field("TacDoubleToUInt", "", ++t);
            TacDoubleToUInt* p_node = static_cast<TacDoubleToUInt*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_TacIntToDouble_t: {
            print_field("TacIntToDouble", "", ++t);
            TacIntToDouble* p_node = static_cast<TacIntToDouble*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_TacUIntToDouble_t: {
            print_field("TacUIntToDouble", "", ++t);
            TacUIntToDouble* p_node = static_cast<TacUIntToDouble*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_TacFunCall_t: {
            print_field("TacFunCall", "", ++t);
            TacFunCall* p_node = static_cast<TacFunCall*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("List[" + std::to_string(p_node->args.size()) + "]", "", t + 1);
            for (const auto& item : p_node->args) {
                print_ast(item.get(), t + 1);
            }
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_TacUnary_t: {
            print_field("TacUnary", "", ++t);
            TacUnary* p_node = static_cast<TacUnary*>(node);
            print_ast(p_node->unop.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_TacBinary_t: {
            print_field("TacBinary", "", ++t);
            TacBinary* p_node = static_cast<TacBinary*>(node);
            print_ast(p_node->binop.get(), t);
            print_ast(p_node->src1.get(), t);
            print_ast(p_node->src2.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_TacCopy_t: {
            print_field("TacCopy", "", ++t);
            TacCopy* p_node = static_cast<TacCopy*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_TacGetAddress_t: {
            print_field("TacGetAddress", "", ++t);
            TacGetAddress* p_node = static_cast<TacGetAddress*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_TacLoad_t: {
            print_field("TacLoad", "", ++t);
            TacLoad* p_node = static_cast<TacLoad*>(node);
            print_ast(p_node->src_ptr.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_TacStore_t: {
            print_field("TacStore", "", ++t);
            TacStore* p_node = static_cast<TacStore*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst_ptr.get(), t);
            break;
        }
        case AST_TacAddPtr_t: {
            print_field("TacAddPtr", "", ++t);
            TacAddPtr* p_node = static_cast<TacAddPtr*>(node);
            print_field("TLong", std::to_string(p_node->scale), t + 1);
            print_ast(p_node->src_ptr.get(), t);
            print_ast(p_node->idx.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_TacCopyToOffset_t: {
            print_field("TacCopyToOffset", "", ++t);
            TacCopyToOffset* p_node = static_cast<TacCopyToOffset*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->dst_name], t + 1);
            print_field("TLong", std::to_string(p_node->offset), t + 1);
            print_ast(p_node->src.get(), t);
            break;
        }
        case AST_TacCopyFromOffset_t: {
            print_field("TacCopyFromOffset", "", ++t);
            TacCopyFromOffset* p_node = static_cast<TacCopyFromOffset*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->src_name], t + 1);
            print_field("TLong", std::to_string(p_node->offset), t + 1);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_TacJump_t: {
            print_field("TacJump", "", ++t);
            TacJump* p_node = static_cast<TacJump*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            break;
        }
        case AST_TacJumpIfZero_t: {
            print_field("TacJumpIfZero", "", ++t);
            TacJumpIfZero* p_node = static_cast<TacJumpIfZero*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->condition.get(), t);
            break;
        }
        case AST_TacJumpIfNotZero_t: {
            print_field("TacJumpIfNotZero", "", ++t);
            TacJumpIfNotZero* p_node = static_cast<TacJumpIfNotZero*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->condition.get(), t);
            break;
        }
        case AST_TacLabel_t: {
            print_field("TacLabel", "", ++t);
            TacLabel* p_node = static_cast<TacLabel*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            break;
        }
        case AST_TacTopLevel_t: {
            print_field("TacTopLevel", "", ++t);
            break;
        }
        case AST_TacFunction_t: {
            print_field("TacFunction", "", ++t);
            TacFunction* p_node = static_cast<TacFunction*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("Bool", std::to_string(p_node->is_glob), t + 1);
            print_field("List[" + std::to_string(p_node->params.size()) + "]", "", t + 1);
            for (const auto& item : p_node->params) {
                print_field("TIdentifier", identifiers->hash_table[item], t + 2);
            }
            print_field("List[" + std::to_string(p_node->body.size()) + "]", "", t + 1);
            for (const auto& item : p_node->body) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_TacStaticVariable_t: {
            print_field("TacStaticVariable", "", ++t);
            TacStaticVariable* p_node = static_cast<TacStaticVariable*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("Bool", std::to_string(p_node->is_glob), t + 1);
            print_ast(p_node->static_init_type.get(), t);
            print_field("List[" + std::to_string(p_node->static_inits.size()) + "]", "", t + 1);
            for (const auto& item : p_node->static_inits) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_TacStaticConstant_t: {
            print_field("TacStaticConstant", "", ++t);
            TacStaticConstant* p_node = static_cast<TacStaticConstant*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_ast(p_node->static_init_type.get(), t);
            print_ast(p_node->static_init.get(), t);
            break;
        }
        case AST_TacProgram_t: {
            print_field("TacProgram", "", ++t);
            TacProgram* p_node = static_cast<TacProgram*>(node);
            print_field("List[" + std::to_string(p_node->static_const_toplvls.size()) + "]", "", t + 1);
            for (const auto& item : p_node->static_const_toplvls) {
                print_ast(item.get(), t + 1);
            }
            print_field("List[" + std::to_string(p_node->static_var_toplvls.size()) + "]", "", t + 1);
            for (const auto& item : p_node->static_var_toplvls) {
                print_ast(item.get(), t + 1);
            }
            print_field("List[" + std::to_string(p_node->fun_toplvls.size()) + "]", "", t + 1);
            for (const auto& item : p_node->fun_toplvls) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_AsmReg_t: {
            print_field("AsmReg", "", ++t);
            break;
        }
        case AST_AsmAx_t: {
            print_field("AsmAx", "", ++t);
            break;
        }
        case AST_AsmBx_t: {
            print_field("AsmBx", "", ++t);
            break;
        }
        case AST_AsmCx_t: {
            print_field("AsmCx", "", ++t);
            break;
        }
        case AST_AsmDx_t: {
            print_field("AsmDx", "", ++t);
            break;
        }
        case AST_AsmDi_t: {
            print_field("AsmDi", "", ++t);
            break;
        }
        case AST_AsmSi_t: {
            print_field("AsmSi", "", ++t);
            break;
        }
        case AST_AsmR8_t: {
            print_field("AsmR8", "", ++t);
            break;
        }
        case AST_AsmR9_t: {
            print_field("AsmR9", "", ++t);
            break;
        }
        case AST_AsmR10_t: {
            print_field("AsmR10", "", ++t);
            break;
        }
        case AST_AsmR11_t: {
            print_field("AsmR11", "", ++t);
            break;
        }
        case AST_AsmR12_t: {
            print_field("AsmR12", "", ++t);
            break;
        }
        case AST_AsmR13_t: {
            print_field("AsmR13", "", ++t);
            break;
        }
        case AST_AsmR14_t: {
            print_field("AsmR14", "", ++t);
            break;
        }
        case AST_AsmR15_t: {
            print_field("AsmR15", "", ++t);
            break;
        }
        case AST_AsmSp_t: {
            print_field("AsmSp", "", ++t);
            break;
        }
        case AST_AsmBp_t: {
            print_field("AsmBp", "", ++t);
            break;
        }
        case AST_AsmXMM0_t: {
            print_field("AsmXMM0", "", ++t);
            break;
        }
        case AST_AsmXMM1_t: {
            print_field("AsmXMM1", "", ++t);
            break;
        }
        case AST_AsmXMM2_t: {
            print_field("AsmXMM2", "", ++t);
            break;
        }
        case AST_AsmXMM3_t: {
            print_field("AsmXMM3", "", ++t);
            break;
        }
        case AST_AsmXMM4_t: {
            print_field("AsmXMM4", "", ++t);
            break;
        }
        case AST_AsmXMM5_t: {
            print_field("AsmXMM5", "", ++t);
            break;
        }
        case AST_AsmXMM6_t: {
            print_field("AsmXMM6", "", ++t);
            break;
        }
        case AST_AsmXMM7_t: {
            print_field("AsmXMM7", "", ++t);
            break;
        }
        case AST_AsmXMM8_t: {
            print_field("AsmXMM8", "", ++t);
            break;
        }
        case AST_AsmXMM9_t: {
            print_field("AsmXMM9", "", ++t);
            break;
        }
        case AST_AsmXMM10_t: {
            print_field("AsmXMM10", "", ++t);
            break;
        }
        case AST_AsmXMM11_t: {
            print_field("AsmXMM11", "", ++t);
            break;
        }
        case AST_AsmXMM12_t: {
            print_field("AsmXMM12", "", ++t);
            break;
        }
        case AST_AsmXMM13_t: {
            print_field("AsmXMM13", "", ++t);
            break;
        }
        case AST_AsmXMM14_t: {
            print_field("AsmXMM14", "", ++t);
            break;
        }
        case AST_AsmXMM15_t: {
            print_field("AsmXMM15", "", ++t);
            break;
        }
        case AST_AsmCondCode_t: {
            print_field("AsmCondCode", "", ++t);
            break;
        }
        case AST_AsmE_t: {
            print_field("AsmE", "", ++t);
            break;
        }
        case AST_AsmNE_t: {
            print_field("AsmNE", "", ++t);
            break;
        }
        case AST_AsmG_t: {
            print_field("AsmG", "", ++t);
            break;
        }
        case AST_AsmGE_t: {
            print_field("AsmGE", "", ++t);
            break;
        }
        case AST_AsmL_t: {
            print_field("AsmL", "", ++t);
            break;
        }
        case AST_AsmLE_t: {
            print_field("AsmLE", "", ++t);
            break;
        }
        case AST_AsmA_t: {
            print_field("AsmA", "", ++t);
            break;
        }
        case AST_AsmAE_t: {
            print_field("AsmAE", "", ++t);
            break;
        }
        case AST_AsmB_t: {
            print_field("AsmB", "", ++t);
            break;
        }
        case AST_AsmBE_t: {
            print_field("AsmBE", "", ++t);
            break;
        }
        case AST_AsmP_t: {
            print_field("AsmP", "", ++t);
            break;
        }
        case AST_AsmOperand_t: {
            print_field("AsmOperand", "", ++t);
            break;
        }
        case AST_AsmImm_t: {
            print_field("AsmImm", "", ++t);
            AsmImm* p_node = static_cast<AsmImm*>(node);
            print_field("TULong", std::to_string(p_node->value), t + 1);
            print_field("Bool", std::to_string(p_node->is_byte), t + 1);
            print_field("Bool", std::to_string(p_node->is_quad), t + 1);
            print_field("Bool", std::to_string(p_node->is_neg), t + 1);
            break;
        }
        case AST_AsmRegister_t: {
            print_field("AsmRegister", "", ++t);
            AsmRegister* p_node = static_cast<AsmRegister*>(node);
            print_ast(p_node->reg.get(), t);
            break;
        }
        case AST_AsmPseudo_t: {
            print_field("AsmPseudo", "", ++t);
            AsmPseudo* p_node = static_cast<AsmPseudo*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            break;
        }
        case AST_AsmMemory_t: {
            print_field("AsmMemory", "", ++t);
            AsmMemory* p_node = static_cast<AsmMemory*>(node);
            print_field("TLong", std::to_string(p_node->value), t + 1);
            print_ast(p_node->reg.get(), t);
            break;
        }
        case AST_AsmData_t: {
            print_field("AsmData", "", ++t);
            AsmData* p_node = static_cast<AsmData*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("TLong", std::to_string(p_node->offset), t + 1);
            break;
        }
        case AST_AsmPseudoMem_t: {
            print_field("AsmPseudoMem", "", ++t);
            AsmPseudoMem* p_node = static_cast<AsmPseudoMem*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("TLong", std::to_string(p_node->offset), t + 1);
            break;
        }
        case AST_AsmIndexed_t: {
            print_field("AsmIndexed", "", ++t);
            AsmIndexed* p_node = static_cast<AsmIndexed*>(node);
            print_field("TLong", std::to_string(p_node->scale), t + 1);
            print_ast(p_node->reg_base.get(), t);
            print_ast(p_node->reg_index.get(), t);
            break;
        }
        case AST_AsmBinaryOp_t: {
            print_field("AsmBinaryOp", "", ++t);
            break;
        }
        case AST_AsmAdd_t: {
            print_field("AsmAdd", "", ++t);
            break;
        }
        case AST_AsmSub_t: {
            print_field("AsmSub", "", ++t);
            break;
        }
        case AST_AsmMult_t: {
            print_field("AsmMult", "", ++t);
            break;
        }
        case AST_AsmDivDouble_t: {
            print_field("AsmDivDouble", "", ++t);
            break;
        }
        case AST_AsmBitAnd_t: {
            print_field("AsmBitAnd", "", ++t);
            break;
        }
        case AST_AsmBitOr_t: {
            print_field("AsmBitOr", "", ++t);
            break;
        }
        case AST_AsmBitXor_t: {
            print_field("AsmBitXor", "", ++t);
            break;
        }
        case AST_AsmBitShiftLeft_t: {
            print_field("AsmBitShiftLeft", "", ++t);
            break;
        }
        case AST_AsmBitShiftRight_t: {
            print_field("AsmBitShiftRight", "", ++t);
            break;
        }
        case AST_AsmBitShrArithmetic_t: {
            print_field("AsmBitShrArithmetic", "", ++t);
            break;
        }
        case AST_AsmUnaryOp_t: {
            print_field("AsmUnaryOp", "", ++t);
            break;
        }
        case AST_AsmNot_t: {
            print_field("AsmNot", "", ++t);
            break;
        }
        case AST_AsmNeg_t: {
            print_field("AsmNeg", "", ++t);
            break;
        }
        case AST_AsmShr_t: {
            print_field("AsmShr", "", ++t);
            break;
        }
        case AST_AsmInstruction_t: {
            print_field("AsmInstruction", "", ++t);
            break;
        }
        case AST_AsmMov_t: {
            print_field("AsmMov", "", ++t);
            AsmMov* p_node = static_cast<AsmMov*>(node);
            print_ast(p_node->asm_type.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_AsmMovSx_t: {
            print_field("AsmMovSx", "", ++t);
            AsmMovSx* p_node = static_cast<AsmMovSx*>(node);
            print_ast(p_node->asm_type_src.get(), t);
            print_ast(p_node->asm_type_dst.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_AsmMovZeroExtend_t: {
            print_field("AsmMovZeroExtend", "", ++t);
            AsmMovZeroExtend* p_node = static_cast<AsmMovZeroExtend*>(node);
            print_ast(p_node->asm_type_src.get(), t);
            print_ast(p_node->asm_type_dst.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_AsmLea_t: {
            print_field("AsmLea", "", ++t);
            AsmLea* p_node = static_cast<AsmLea*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_AsmCvttsd2si_t: {
            print_field("AsmCvttsd2si", "", ++t);
            AsmCvttsd2si* p_node = static_cast<AsmCvttsd2si*>(node);
            print_ast(p_node->asm_type.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_AsmCvtsi2sd_t: {
            print_field("AsmCvtsi2sd", "", ++t);
            AsmCvtsi2sd* p_node = static_cast<AsmCvtsi2sd*>(node);
            print_ast(p_node->asm_type.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_AsmUnary_t: {
            print_field("AsmUnary", "", ++t);
            AsmUnary* p_node = static_cast<AsmUnary*>(node);
            print_ast(p_node->unop.get(), t);
            print_ast(p_node->asm_type.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_AsmBinary_t: {
            print_field("AsmBinary", "", ++t);
            AsmBinary* p_node = static_cast<AsmBinary*>(node);
            print_ast(p_node->binop.get(), t);
            print_ast(p_node->asm_type.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_AsmCmp_t: {
            print_field("AsmCmp", "", ++t);
            AsmCmp* p_node = static_cast<AsmCmp*>(node);
            print_ast(p_node->asm_type.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_AsmIdiv_t: {
            print_field("AsmIdiv", "", ++t);
            AsmIdiv* p_node = static_cast<AsmIdiv*>(node);
            print_ast(p_node->asm_type.get(), t);
            print_ast(p_node->src.get(), t);
            break;
        }
        case AST_AsmDiv_t: {
            print_field("AsmDiv", "", ++t);
            AsmDiv* p_node = static_cast<AsmDiv*>(node);
            print_ast(p_node->asm_type.get(), t);
            print_ast(p_node->src.get(), t);
            break;
        }
        case AST_AsmCdq_t: {
            print_field("AsmCdq", "", ++t);
            AsmCdq* p_node = static_cast<AsmCdq*>(node);
            print_ast(p_node->asm_type.get(), t);
            break;
        }
        case AST_AsmJmp_t: {
            print_field("AsmJmp", "", ++t);
            AsmJmp* p_node = static_cast<AsmJmp*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            break;
        }
        case AST_AsmJmpCC_t: {
            print_field("AsmJmpCC", "", ++t);
            AsmJmpCC* p_node = static_cast<AsmJmpCC*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->cond_code.get(), t);
            break;
        }
        case AST_AsmSetCC_t: {
            print_field("AsmSetCC", "", ++t);
            AsmSetCC* p_node = static_cast<AsmSetCC*>(node);
            print_ast(p_node->cond_code.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_AsmLabel_t: {
            print_field("AsmLabel", "", ++t);
            AsmLabel* p_node = static_cast<AsmLabel*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            break;
        }
        case AST_AsmPush_t: {
            print_field("AsmPush", "", ++t);
            AsmPush* p_node = static_cast<AsmPush*>(node);
            print_ast(p_node->src.get(), t);
            break;
        }
        case AST_AsmPop_t: {
            print_field("AsmPop", "", ++t);
            AsmPop* p_node = static_cast<AsmPop*>(node);
            print_ast(p_node->reg.get(), t);
            break;
        }
        case AST_AsmCall_t: {
            print_field("AsmCall", "", ++t);
            AsmCall* p_node = static_cast<AsmCall*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            break;
        }
        case AST_AsmRet_t: {
            print_field("AsmRet", "", ++t);
            break;
        }
        case AST_AsmTopLevel_t: {
            print_field("AsmTopLevel", "", ++t);
            break;
        }
        case AST_AsmFunction_t: {
            print_field("AsmFunction", "", ++t);
            AsmFunction* p_node = static_cast<AsmFunction*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("Bool", std::to_string(p_node->is_glob), t + 1);
            print_field("Bool", std::to_string(p_node->is_ret_memory), t + 1);
            print_field("List[" + std::to_string(p_node->instructions.size()) + "]", "", t + 1);
            for (const auto& item : p_node->instructions) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_AsmStaticVariable_t: {
            print_field("AsmStaticVariable", "", ++t);
            AsmStaticVariable* p_node = static_cast<AsmStaticVariable*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("TInt", std::to_string(p_node->alignment), t + 1);
            print_field("Bool", std::to_string(p_node->is_glob), t + 1);
            print_field("List[" + std::to_string(p_node->static_inits.size()) + "]", "", t + 1);
            for (const auto& item : p_node->static_inits) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_AsmStaticConstant_t: {
            print_field("AsmStaticConstant", "", ++t);
            AsmStaticConstant* p_node = static_cast<AsmStaticConstant*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("TInt", std::to_string(p_node->alignment), t + 1);
            print_ast(p_node->static_init.get(), t);
            break;
        }
        case AST_AsmProgram_t: {
            print_field("AsmProgram", "", ++t);
            AsmProgram* p_node = static_cast<AsmProgram*>(node);
            print_field("List[" + std::to_string(p_node->static_const_toplvls.size()) + "]", "", t + 1);
            for (const auto& item : p_node->static_const_toplvls) {
                print_ast(item.get(), t + 1);
            }
            print_field("List[" + std::to_string(p_node->top_levels.size()) + "]", "", t + 1);
            for (const auto& item : p_node->top_levels) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        default:
            RAISE_INTERNAL_ERROR;
    }
}

void pprint_ast(Ast* node, const std::string& name) {
    print_title(name);
    print_ast(node, 0);
    std::cout << std::endl;
}

void pprint_addressed_set() {
    print_title("Addressed Set");
    std::cout << "\nSet(" << std::to_string(frontend->addressed_set.size()) << "):";
    for (const TIdentifier& name : frontend->addressed_set) {
        print_field("", identifiers->hash_table[name], 2);
    }
    std::cout << std::endl;
}

void pprint_string_const_table() {
    print_title("String Constant Table");
    std::cout << "\nDict(" << std::to_string(frontend->string_const_table.size()) << "):";
    for (const auto& static_constant : frontend->string_const_table) {
        print_field("[" + identifiers->hash_table[static_constant.first] + "]", "", 2);
        if (frontend->symbol_table.find(static_constant.second) != frontend->symbol_table.end()
            && frontend->symbol_table[static_constant.second]->attrs->type() == AST_ConstantAttr_t) {
            ConstantAttr* constant_attr =
                static_cast<ConstantAttr*>(frontend->symbol_table[static_constant.second]->attrs.get());
            if (constant_attr->static_init->type() == AST_StringInit_t) {
                std::cout << "\n    String: \"";
                for (const TChar& byte :
                    static_cast<StringInit*>(constant_attr->static_init.get())->literal.get()->value) {
                    switch (byte) {
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
        RAISE_INTERNAL_ERROR;
    }
    std::cout << std::endl;
}

void pprint_struct_typedef_table() {
    print_title("Structure Typedef Table");
    std::cout << "\nDict(" << std::to_string(frontend->struct_typedef_table.size()) << "):";
    for (const auto& struct_typedef : frontend->struct_typedef_table) {
        print_field("[" + identifiers->hash_table[struct_typedef.first] + "]", "", 2);
        print_ast(struct_typedef.second.get(), 2);
    }
    std::cout << std::endl;
}

void pprint_symbol_table() {
    print_title("Symbol Table");
    std::cout << "\nDict(" << std::to_string(frontend->symbol_table.size()) << "):";
    for (const auto& symbol : frontend->symbol_table) {
        print_field("[" + identifiers->hash_table[symbol.first] + "]", "", 2);
        print_ast(symbol.second.get(), 2);
    }
    std::cout << std::endl;
}

void pprint_backend_symbol_table() {
    print_title("Backend Symbol Table");
    std::cout << "\nDict(" << std::to_string(backend->symbol_table.size()) << "):";
    for (const auto& backend_symbol : backend->symbol_table) {
        print_field("[" + identifiers->hash_table[backend_symbol.first] + "]", "", 2);
        print_ast(backend_symbol.second.get(), 2);
    }
    std::cout << std::endl;
}

void pprint_asm_code() {
    print_title("ASM Code");
    std::cout << std::endl;
}
#endif
