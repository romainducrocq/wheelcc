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
    for (const auto& token : tokens) {
        std::cout << "\n  ";
        if (token.token.compare(fmt_tok_kind(token.token_kind)) == 0) {
            std::cout << token.token;
            continue;
        }
        std::cout << fmt_tok_kind(token.token_kind) << "(" << token.token << ")";
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
        case AST_T::CConst_t: {
            print_field("CConst", "", ++t);
            break;
        }
        case AST_T::CConstInt_t: {
            print_field("CConstInt", "", ++t);
            CConstInt* p_node = static_cast<CConstInt*>(node);
            print_field("TInt", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_T::CConstLong_t: {
            print_field("CConstLong", "", ++t);
            CConstLong* p_node = static_cast<CConstLong*>(node);
            print_field("TLong", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_T::CConstUInt_t: {
            print_field("CConstUInt", "", ++t);
            CConstUInt* p_node = static_cast<CConstUInt*>(node);
            print_field("TUInt", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_T::CConstULong_t: {
            print_field("CConstULong", "", ++t);
            CConstULong* p_node = static_cast<CConstULong*>(node);
            print_field("TULong", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_T::CConstDouble_t: {
            print_field("CConstDouble", "", ++t);
            CConstDouble* p_node = static_cast<CConstDouble*>(node);
            print_field("TDouble", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_T::CConstChar_t: {
            print_field("CConstChar", "", ++t);
            CConstChar* p_node = static_cast<CConstChar*>(node);
            print_field("TChar", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_T::CConstUChar_t: {
            print_field("CConstUChar", "", ++t);
            CConstUChar* p_node = static_cast<CConstUChar*>(node);
            print_field("TUChar", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_T::CStringLiteral_t: {
            print_field("CStringLiteral", "", ++t);
            CStringLiteral* p_node = static_cast<CStringLiteral*>(node);
            print_field("List[" + std::to_string(p_node->value.size()) + "]", "", t + 1);
            for (const auto& item : p_node->value) {
                print_field("TChar", std::to_string(item), t + 2);
            }
            break;
        }
        case AST_T::Type_t: {
            print_field("Type", "", ++t);
            break;
        }
        case AST_T::Char_t: {
            print_field("Char", "", ++t);
            break;
        }
        case AST_T::SChar_t: {
            print_field("SChar", "", ++t);
            break;
        }
        case AST_T::UChar_t: {
            print_field("UChar", "", ++t);
            break;
        }
        case AST_T::Int_t: {
            print_field("Int", "", ++t);
            break;
        }
        case AST_T::Long_t: {
            print_field("Long", "", ++t);
            break;
        }
        case AST_T::UInt_t: {
            print_field("UInt", "", ++t);
            break;
        }
        case AST_T::ULong_t: {
            print_field("ULong", "", ++t);
            break;
        }
        case AST_T::Double_t: {
            print_field("Double", "", ++t);
            break;
        }
        case AST_T::Void_t: {
            print_field("Void", "", ++t);
            break;
        }
        case AST_T::FunType_t: {
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
        case AST_T::Pointer_t: {
            print_field("Pointer", "", ++t);
            Pointer* p_node = static_cast<Pointer*>(node);
            print_ast(p_node->ref_type.get(), t);
            break;
        }
        case AST_T::Array_t: {
            print_field("Array", "", ++t);
            Array* p_node = static_cast<Array*>(node);
            print_field("TLong", std::to_string(p_node->size), t + 1);
            print_ast(p_node->elem_type.get(), t);
            break;
        }
        case AST_T::Structure_t: {
            print_field("Structure", "", ++t);
            Structure* p_node = static_cast<Structure*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->tag], t + 1);
            print_field("Bool", std::to_string(p_node->is_union), t + 1);
            break;
        }
        case AST_T::StaticInit_t: {
            print_field("StaticInit", "", ++t);
            break;
        }
        case AST_T::IntInit_t: {
            print_field("IntInit", "", ++t);
            IntInit* p_node = static_cast<IntInit*>(node);
            print_field("TInt", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_T::LongInit_t: {
            print_field("LongInit", "", ++t);
            LongInit* p_node = static_cast<LongInit*>(node);
            print_field("TLong", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_T::UIntInit_t: {
            print_field("UIntInit", "", ++t);
            UIntInit* p_node = static_cast<UIntInit*>(node);
            print_field("TUInt", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_T::ULongInit_t: {
            print_field("ULongInit", "", ++t);
            ULongInit* p_node = static_cast<ULongInit*>(node);
            print_field("TULong", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_T::CharInit_t: {
            print_field("CharInit", "", ++t);
            CharInit* p_node = static_cast<CharInit*>(node);
            print_field("TChar", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_T::UCharInit_t: {
            print_field("UCharInit", "", ++t);
            UCharInit* p_node = static_cast<UCharInit*>(node);
            print_field("TUChar", std::to_string(p_node->value), t + 1);
            break;
        }
        case AST_T::DoubleInit_t: {
            print_field("DoubleInit", "", ++t);
            DoubleInit* p_node = static_cast<DoubleInit*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->double_constant], t + 1);
            break;
        }
        case AST_T::ZeroInit_t: {
            print_field("ZeroInit", "", ++t);
            ZeroInit* p_node = static_cast<ZeroInit*>(node);
            print_field("TLong", std::to_string(p_node->byte), t + 1);
            break;
        }
        case AST_T::StringInit_t: {
            print_field("StringInit", "", ++t);
            StringInit* p_node = static_cast<StringInit*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->string_constant], t + 1);
            print_field("Bool", std::to_string(p_node->is_null_terminated), t + 1);
            print_ast(p_node->literal.get(), t);
            break;
        }
        case AST_T::PointerInit_t: {
            print_field("PointerInit", "", ++t);
            PointerInit* p_node = static_cast<PointerInit*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            break;
        }
        case AST_T::InitialValue_t: {
            print_field("InitialValue", "", ++t);
            break;
        }
        case AST_T::Tentative_t: {
            print_field("Tentative", "", ++t);
            break;
        }
        case AST_T::Initial_t: {
            print_field("Initial", "", ++t);
            Initial* p_node = static_cast<Initial*>(node);
            print_field("List[" + std::to_string(p_node->static_inits.size()) + "]", "", t + 1);
            for (const auto& item : p_node->static_inits) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_T::NoInitializer_t: {
            print_field("NoInitializer", "", ++t);
            break;
        }
        case AST_T::IdentifierAttr_t: {
            print_field("IdentifierAttr", "", ++t);
            break;
        }
        case AST_T::FunAttr_t: {
            print_field("FunAttr", "", ++t);
            FunAttr* p_node = static_cast<FunAttr*>(node);
            print_field("Bool", std::to_string(p_node->is_defined), t + 1);
            print_field("Bool", std::to_string(p_node->is_global), t + 1);
            break;
        }
        case AST_T::StaticAttr_t: {
            print_field("StaticAttr", "", ++t);
            StaticAttr* p_node = static_cast<StaticAttr*>(node);
            print_field("Bool", std::to_string(p_node->is_global), t + 1);
            print_ast(p_node->init.get(), t);
            break;
        }
        case AST_T::ConstantAttr_t: {
            print_field("ConstantAttr", "", ++t);
            ConstantAttr* p_node = static_cast<ConstantAttr*>(node);
            print_ast(p_node->static_init.get(), t);
            break;
        }
        case AST_T::LocalAttr_t: {
            print_field("LocalAttr", "", ++t);
            break;
        }
        case AST_T::Symbol_t: {
            print_field("Symbol", "", ++t);
            Symbol* p_node = static_cast<Symbol*>(node);
            print_ast(p_node->type_t.get(), t);
            print_ast(p_node->attrs.get(), t);
            break;
        }
        case AST_T::StructMember_t: {
            print_field("StructMember", "", ++t);
            StructMember* p_node = static_cast<StructMember*>(node);
            print_field("TLong", std::to_string(p_node->offset), t + 1);
            print_ast(p_node->member_type.get(), t);
            break;
        }
        case AST_T::StructTypedef_t: {
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
        case AST_T::AssemblyType_t: {
            print_field("AssemblyType", "", ++t);
            break;
        }
        case AST_T::Byte_t: {
            print_field("Byte", "", ++t);
            break;
        }
        case AST_T::LongWord_t: {
            print_field("LongWord", "", ++t);
            break;
        }
        case AST_T::QuadWord_t: {
            print_field("QuadWord", "", ++t);
            break;
        }
        case AST_T::BackendDouble_t: {
            print_field("BackendDouble", "", ++t);
            break;
        }
        case AST_T::ByteArray_t: {
            print_field("ByteArray", "", ++t);
            ByteArray* p_node = static_cast<ByteArray*>(node);
            print_field("TLong", std::to_string(p_node->size), t + 1);
            print_field("TInt", std::to_string(p_node->alignment), t + 1);
            break;
        }
        case AST_T::BackendSymbol_t: {
            print_field("BackendSymbol", "", ++t);
            break;
        }
        case AST_T::BackendObj_t: {
            print_field("BackendObj", "", ++t);
            BackendObj* p_node = static_cast<BackendObj*>(node);
            print_field("Bool", std::to_string(p_node->is_static), t + 1);
            print_field("Bool", std::to_string(p_node->is_constant), t + 1);
            print_ast(p_node->assembly_type.get(), t);
            break;
        }
        case AST_T::BackendFun_t: {
            print_field("BackendFun", "", ++t);
            BackendFun* p_node = static_cast<BackendFun*>(node);
            print_field("Bool", std::to_string(p_node->is_defined), t + 1);
            print_field("List[" + std::to_string(p_node->callee_saved_registers.size()) + "]", "", t + 1);
            for (const auto& item : p_node->callee_saved_registers) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_T::CUnaryOp_t: {
            print_field("CUnaryOp", "", ++t);
            break;
        }
        case AST_T::CComplement_t: {
            print_field("CComplement", "", ++t);
            break;
        }
        case AST_T::CNegate_t: {
            print_field("CNegate", "", ++t);
            break;
        }
        case AST_T::CNot_t: {
            print_field("CNot", "", ++t);
            break;
        }
        case AST_T::CPrefix_t: {
            print_field("CPrefix", "", ++t);
            break;
        }
        case AST_T::CPostfix_t: {
            print_field("CPostfix", "", ++t);
            break;
        }
        case AST_T::CBinaryOp_t: {
            print_field("CBinaryOp", "", ++t);
            break;
        }
        case AST_T::CAdd_t: {
            print_field("CAdd", "", ++t);
            break;
        }
        case AST_T::CSubtract_t: {
            print_field("CSubtract", "", ++t);
            break;
        }
        case AST_T::CMultiply_t: {
            print_field("CMultiply", "", ++t);
            break;
        }
        case AST_T::CDivide_t: {
            print_field("CDivide", "", ++t);
            break;
        }
        case AST_T::CRemainder_t: {
            print_field("CRemainder", "", ++t);
            break;
        }
        case AST_T::CBitAnd_t: {
            print_field("CBitAnd", "", ++t);
            break;
        }
        case AST_T::CBitOr_t: {
            print_field("CBitOr", "", ++t);
            break;
        }
        case AST_T::CBitXor_t: {
            print_field("CBitXor", "", ++t);
            break;
        }
        case AST_T::CBitShiftLeft_t: {
            print_field("CBitShiftLeft", "", ++t);
            break;
        }
        case AST_T::CBitShiftRight_t: {
            print_field("CBitShiftRight", "", ++t);
            break;
        }
        case AST_T::CBitShrArithmetic_t: {
            print_field("CBitShrArithmetic", "", ++t);
            break;
        }
        case AST_T::CAnd_t: {
            print_field("CAnd", "", ++t);
            break;
        }
        case AST_T::COr_t: {
            print_field("COr", "", ++t);
            break;
        }
        case AST_T::CEqual_t: {
            print_field("CEqual", "", ++t);
            break;
        }
        case AST_T::CNotEqual_t: {
            print_field("CNotEqual", "", ++t);
            break;
        }
        case AST_T::CLessThan_t: {
            print_field("CLessThan", "", ++t);
            break;
        }
        case AST_T::CLessOrEqual_t: {
            print_field("CLessOrEqual", "", ++t);
            break;
        }
        case AST_T::CGreaterThan_t: {
            print_field("CGreaterThan", "", ++t);
            break;
        }
        case AST_T::CGreaterOrEqual_t: {
            print_field("CGreaterOrEqual", "", ++t);
            break;
        }
        case AST_T::CAbstractDeclarator_t: {
            print_field("CAbstractDeclarator", "", ++t);
            break;
        }
        case AST_T::CAbstractPointer_t: {
            print_field("CAbstractPointer", "", ++t);
            CAbstractPointer* p_node = static_cast<CAbstractPointer*>(node);
            print_ast(p_node->abstract_declarator.get(), t);
            break;
        }
        case AST_T::CAbstractArray_t: {
            print_field("CAbstractArray", "", ++t);
            CAbstractArray* p_node = static_cast<CAbstractArray*>(node);
            print_field("TLong", std::to_string(p_node->size), t + 1);
            print_ast(p_node->abstract_declarator.get(), t);
            break;
        }
        case AST_T::CAbstractBase_t: {
            print_field("CAbstractBase", "", ++t);
            break;
        }
        case AST_T::CParam_t: {
            print_field("CParam", "", ++t);
            CParam* p_node = static_cast<CParam*>(node);
            print_ast(p_node->declarator.get(), t);
            print_ast(p_node->param_type.get(), t);
            break;
        }
        case AST_T::CDeclarator_t: {
            print_field("CDeclarator", "", ++t);
            break;
        }
        case AST_T::CIdent_t: {
            print_field("CIdent", "", ++t);
            CIdent* p_node = static_cast<CIdent*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            break;
        }
        case AST_T::CPointerDeclarator_t: {
            print_field("CPointerDeclarator", "", ++t);
            CPointerDeclarator* p_node = static_cast<CPointerDeclarator*>(node);
            print_ast(p_node->declarator.get(), t);
            break;
        }
        case AST_T::CArrayDeclarator_t: {
            print_field("CArrayDeclarator", "", ++t);
            CArrayDeclarator* p_node = static_cast<CArrayDeclarator*>(node);
            print_field("TLong", std::to_string(p_node->size), t + 1);
            print_ast(p_node->declarator.get(), t);
            break;
        }
        case AST_T::CFunDeclarator_t: {
            print_field("CFunDeclarator", "", ++t);
            CFunDeclarator* p_node = static_cast<CFunDeclarator*>(node);
            print_field("List[" + std::to_string(p_node->param_list.size()) + "]", "", t + 1);
            for (const auto& item : p_node->param_list) {
                print_ast(item.get(), t + 1);
            }
            print_ast(p_node->declarator.get(), t);
            break;
        }
        case AST_T::CExp_t: {
            print_field("CExp", "", ++t);
            CExp* p_node = static_cast<CExp*>(node);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CConstant_t: {
            print_field("CConstant", "", ++t);
            CConstant* p_node = static_cast<CConstant*>(node);
            print_ast(p_node->constant.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CString_t: {
            print_field("CString", "", ++t);
            CString* p_node = static_cast<CString*>(node);
            print_ast(p_node->literal.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CVar_t: {
            print_field("CVar", "", ++t);
            CVar* p_node = static_cast<CVar*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CCast_t: {
            print_field("CCast", "", ++t);
            CCast* p_node = static_cast<CCast*>(node);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->target_type.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CUnary_t: {
            print_field("CUnary", "", ++t);
            CUnary* p_node = static_cast<CUnary*>(node);
            print_ast(p_node->unary_op.get(), t);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CBinary_t: {
            print_field("CBinary", "", ++t);
            CBinary* p_node = static_cast<CBinary*>(node);
            print_ast(p_node->binary_op.get(), t);
            print_ast(p_node->exp_left.get(), t);
            print_ast(p_node->exp_right.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CAssignment_t: {
            print_field("CAssignment", "", ++t);
            CAssignment* p_node = static_cast<CAssignment*>(node);
            print_ast(p_node->unary_op.get(), t);
            print_ast(p_node->exp_left.get(), t);
            print_ast(p_node->exp_right.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CConditional_t: {
            print_field("CConditional", "", ++t);
            CConditional* p_node = static_cast<CConditional*>(node);
            print_ast(p_node->condition.get(), t);
            print_ast(p_node->exp_middle.get(), t);
            print_ast(p_node->exp_right.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CFunctionCall_t: {
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
        case AST_T::CDereference_t: {
            print_field("CDereference", "", ++t);
            CDereference* p_node = static_cast<CDereference*>(node);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CAddrOf_t: {
            print_field("CAddrOf", "", ++t);
            CAddrOf* p_node = static_cast<CAddrOf*>(node);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CSubscript_t: {
            print_field("CSubscript", "", ++t);
            CSubscript* p_node = static_cast<CSubscript*>(node);
            print_ast(p_node->primary_exp.get(), t);
            print_ast(p_node->subscript_exp.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CSizeOf_t: {
            print_field("CSizeOf", "", ++t);
            CSizeOf* p_node = static_cast<CSizeOf*>(node);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CSizeOfT_t: {
            print_field("CSizeOfT", "", ++t);
            CSizeOfT* p_node = static_cast<CSizeOfT*>(node);
            print_ast(p_node->target_type.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CDot_t: {
            print_field("CDot", "", ++t);
            CDot* p_node = static_cast<CDot*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->member], t + 1);
            print_ast(p_node->structure.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CArrow_t: {
            print_field("CArrow", "", ++t);
            CArrow* p_node = static_cast<CArrow*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->member], t + 1);
            print_ast(p_node->pointer.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CStatement_t: {
            print_field("CStatement", "", ++t);
            break;
        }
        case AST_T::CReturn_t: {
            print_field("CReturn", "", ++t);
            CReturn* p_node = static_cast<CReturn*>(node);
            print_ast(p_node->exp.get(), t);
            break;
        }
        case AST_T::CExpression_t: {
            print_field("CExpression", "", ++t);
            CExpression* p_node = static_cast<CExpression*>(node);
            print_ast(p_node->exp.get(), t);
            break;
        }
        case AST_T::CIf_t: {
            print_field("CIf", "", ++t);
            CIf* p_node = static_cast<CIf*>(node);
            print_ast(p_node->condition.get(), t);
            print_ast(p_node->then.get(), t);
            print_ast(p_node->else_fi.get(), t);
            break;
        }
        case AST_T::CGoto_t: {
            print_field("CGoto", "", ++t);
            CGoto* p_node = static_cast<CGoto*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            break;
        }
        case AST_T::CLabel_t: {
            print_field("CLabel", "", ++t);
            CLabel* p_node = static_cast<CLabel*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->jump_to.get(), t);
            break;
        }
        case AST_T::CCompound_t: {
            print_field("CCompound", "", ++t);
            CCompound* p_node = static_cast<CCompound*>(node);
            print_ast(p_node->block.get(), t);
            break;
        }
        case AST_T::CWhile_t: {
            print_field("CWhile", "", ++t);
            CWhile* p_node = static_cast<CWhile*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->condition.get(), t);
            print_ast(p_node->body.get(), t);
            break;
        }
        case AST_T::CDoWhile_t: {
            print_field("CDoWhile", "", ++t);
            CDoWhile* p_node = static_cast<CDoWhile*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->condition.get(), t);
            print_ast(p_node->body.get(), t);
            break;
        }
        case AST_T::CFor_t: {
            print_field("CFor", "", ++t);
            CFor* p_node = static_cast<CFor*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->init.get(), t);
            print_ast(p_node->condition.get(), t);
            print_ast(p_node->post.get(), t);
            print_ast(p_node->body.get(), t);
            break;
        }
        case AST_T::CSwitch_t: {
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
        case AST_T::CCase_t: {
            print_field("CCase", "", ++t);
            CCase* p_node = static_cast<CCase*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->value.get(), t);
            print_ast(p_node->jump_to.get(), t);
            break;
        }
        case AST_T::CDefault_t: {
            print_field("CDefault", "", ++t);
            CDefault* p_node = static_cast<CDefault*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->jump_to.get(), t);
            break;
        }
        case AST_T::CBreak_t: {
            print_field("CBreak", "", ++t);
            CBreak* p_node = static_cast<CBreak*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            break;
        }
        case AST_T::CContinue_t: {
            print_field("CContinue", "", ++t);
            CContinue* p_node = static_cast<CContinue*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            break;
        }
        case AST_T::CNull_t: {
            print_field("CNull", "", ++t);
            break;
        }
        case AST_T::CForInit_t: {
            print_field("CForInit", "", ++t);
            break;
        }
        case AST_T::CInitDecl_t: {
            print_field("CInitDecl", "", ++t);
            CInitDecl* p_node = static_cast<CInitDecl*>(node);
            print_ast(p_node->init.get(), t);
            break;
        }
        case AST_T::CInitExp_t: {
            print_field("CInitExp", "", ++t);
            CInitExp* p_node = static_cast<CInitExp*>(node);
            print_ast(p_node->init.get(), t);
            break;
        }
        case AST_T::CBlock_t: {
            print_field("CBlock", "", ++t);
            break;
        }
        case AST_T::CB_t: {
            print_field("CB", "", ++t);
            CB* p_node = static_cast<CB*>(node);
            print_field("List[" + std::to_string(p_node->block_items.size()) + "]", "", t + 1);
            for (const auto& item : p_node->block_items) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_T::CBlockItem_t: {
            print_field("CBlockItem", "", ++t);
            break;
        }
        case AST_T::CS_t: {
            print_field("CS", "", ++t);
            CS* p_node = static_cast<CS*>(node);
            print_ast(p_node->statement.get(), t);
            break;
        }
        case AST_T::CD_t: {
            print_field("CD", "", ++t);
            CD* p_node = static_cast<CD*>(node);
            print_ast(p_node->declaration.get(), t);
            break;
        }
        case AST_T::CStorageClass_t: {
            print_field("CStorageClass", "", ++t);
            break;
        }
        case AST_T::CStatic_t: {
            print_field("CStatic", "", ++t);
            break;
        }
        case AST_T::CExtern_t: {
            print_field("CExtern", "", ++t);
            break;
        }
        case AST_T::CInitializer_t: {
            print_field("CInitializer", "", ++t);
            CInitializer* p_node = static_cast<CInitializer*>(node);
            print_ast(p_node->init_type.get(), t);
            break;
        }
        case AST_T::CSingleInit_t: {
            print_field("CSingleInit", "", ++t);
            CSingleInit* p_node = static_cast<CSingleInit*>(node);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->init_type.get(), t);
            break;
        }
        case AST_T::CCompoundInit_t: {
            print_field("CCompoundInit", "", ++t);
            CCompoundInit* p_node = static_cast<CCompoundInit*>(node);
            print_field("List[" + std::to_string(p_node->initializers.size()) + "]", "", t + 1);
            for (const auto& item : p_node->initializers) {
                print_ast(item.get(), t + 1);
            }
            print_ast(p_node->init_type.get(), t);
            break;
        }
        case AST_T::CMemberDeclaration_t: {
            print_field("CMemberDeclaration", "", ++t);
            CMemberDeclaration* p_node = static_cast<CMemberDeclaration*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->member_name], t + 1);
            print_ast(p_node->member_type.get(), t);
            break;
        }
        case AST_T::CStructDeclaration_t: {
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
        case AST_T::CFunctionDeclaration_t: {
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
        case AST_T::CVariableDeclaration_t: {
            print_field("CVariableDeclaration", "", ++t);
            CVariableDeclaration* p_node = static_cast<CVariableDeclaration*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_ast(p_node->init.get(), t);
            print_ast(p_node->var_type.get(), t);
            print_ast(p_node->storage_class.get(), t);
            break;
        }
        case AST_T::CDeclaration_t: {
            print_field("CDeclaration", "", ++t);
            break;
        }
        case AST_T::CFunDecl_t: {
            print_field("CFunDecl", "", ++t);
            CFunDecl* p_node = static_cast<CFunDecl*>(node);
            print_ast(p_node->function_decl.get(), t);
            break;
        }
        case AST_T::CVarDecl_t: {
            print_field("CVarDecl", "", ++t);
            CVarDecl* p_node = static_cast<CVarDecl*>(node);
            print_ast(p_node->variable_decl.get(), t);
            break;
        }
        case AST_T::CStructDecl_t: {
            print_field("CStructDecl", "", ++t);
            CStructDecl* p_node = static_cast<CStructDecl*>(node);
            print_ast(p_node->struct_decl.get(), t);
            break;
        }
        case AST_T::CProgram_t: {
            print_field("CProgram", "", ++t);
            CProgram* p_node = static_cast<CProgram*>(node);
            print_field("List[" + std::to_string(p_node->declarations.size()) + "]", "", t + 1);
            for (const auto& item : p_node->declarations) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_T::TacUnaryOp_t: {
            print_field("TacUnaryOp", "", ++t);
            break;
        }
        case AST_T::TacComplement_t: {
            print_field("TacComplement", "", ++t);
            break;
        }
        case AST_T::TacNegate_t: {
            print_field("TacNegate", "", ++t);
            break;
        }
        case AST_T::TacNot_t: {
            print_field("TacNot", "", ++t);
            break;
        }
        case AST_T::TacBinaryOp_t: {
            print_field("TacBinaryOp", "", ++t);
            break;
        }
        case AST_T::TacAdd_t: {
            print_field("TacAdd", "", ++t);
            break;
        }
        case AST_T::TacSubtract_t: {
            print_field("TacSubtract", "", ++t);
            break;
        }
        case AST_T::TacMultiply_t: {
            print_field("TacMultiply", "", ++t);
            break;
        }
        case AST_T::TacDivide_t: {
            print_field("TacDivide", "", ++t);
            break;
        }
        case AST_T::TacRemainder_t: {
            print_field("TacRemainder", "", ++t);
            break;
        }
        case AST_T::TacBitAnd_t: {
            print_field("TacBitAnd", "", ++t);
            break;
        }
        case AST_T::TacBitOr_t: {
            print_field("TacBitOr", "", ++t);
            break;
        }
        case AST_T::TacBitXor_t: {
            print_field("TacBitXor", "", ++t);
            break;
        }
        case AST_T::TacBitShiftLeft_t: {
            print_field("TacBitShiftLeft", "", ++t);
            break;
        }
        case AST_T::TacBitShiftRight_t: {
            print_field("TacBitShiftRight", "", ++t);
            break;
        }
        case AST_T::TacBitShrArithmetic_t: {
            print_field("TacBitShrArithmetic", "", ++t);
            break;
        }
        case AST_T::TacEqual_t: {
            print_field("TacEqual", "", ++t);
            break;
        }
        case AST_T::TacNotEqual_t: {
            print_field("TacNotEqual", "", ++t);
            break;
        }
        case AST_T::TacLessThan_t: {
            print_field("TacLessThan", "", ++t);
            break;
        }
        case AST_T::TacLessOrEqual_t: {
            print_field("TacLessOrEqual", "", ++t);
            break;
        }
        case AST_T::TacGreaterThan_t: {
            print_field("TacGreaterThan", "", ++t);
            break;
        }
        case AST_T::TacGreaterOrEqual_t: {
            print_field("TacGreaterOrEqual", "", ++t);
            break;
        }
        case AST_T::TacValue_t: {
            print_field("TacValue", "", ++t);
            break;
        }
        case AST_T::TacConstant_t: {
            print_field("TacConstant", "", ++t);
            TacConstant* p_node = static_cast<TacConstant*>(node);
            print_ast(p_node->constant.get(), t);
            break;
        }
        case AST_T::TacVariable_t: {
            print_field("TacVariable", "", ++t);
            TacVariable* p_node = static_cast<TacVariable*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            break;
        }
        case AST_T::TacExpResult_t: {
            print_field("TacExpResult", "", ++t);
            break;
        }
        case AST_T::TacPlainOperand_t: {
            print_field("TacPlainOperand", "", ++t);
            TacPlainOperand* p_node = static_cast<TacPlainOperand*>(node);
            print_ast(p_node->val.get(), t);
            break;
        }
        case AST_T::TacDereferencedPointer_t: {
            print_field("TacDereferencedPointer", "", ++t);
            TacDereferencedPointer* p_node = static_cast<TacDereferencedPointer*>(node);
            print_ast(p_node->val.get(), t);
            break;
        }
        case AST_T::TacSubObject_t: {
            print_field("TacSubObject", "", ++t);
            TacSubObject* p_node = static_cast<TacSubObject*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->base_name], t + 1);
            print_field("TLong", std::to_string(p_node->offset), t + 1);
            break;
        }
        case AST_T::TacInstruction_t: {
            print_field("TacInstruction", "", ++t);
            break;
        }
        case AST_T::TacReturn_t: {
            print_field("TacReturn", "", ++t);
            TacReturn* p_node = static_cast<TacReturn*>(node);
            print_ast(p_node->val.get(), t);
            break;
        }
        case AST_T::TacSignExtend_t: {
            print_field("TacSignExtend", "", ++t);
            TacSignExtend* p_node = static_cast<TacSignExtend*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacTruncate_t: {
            print_field("TacTruncate", "", ++t);
            TacTruncate* p_node = static_cast<TacTruncate*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacZeroExtend_t: {
            print_field("TacZeroExtend", "", ++t);
            TacZeroExtend* p_node = static_cast<TacZeroExtend*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacDoubleToInt_t: {
            print_field("TacDoubleToInt", "", ++t);
            TacDoubleToInt* p_node = static_cast<TacDoubleToInt*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacDoubleToUInt_t: {
            print_field("TacDoubleToUInt", "", ++t);
            TacDoubleToUInt* p_node = static_cast<TacDoubleToUInt*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacIntToDouble_t: {
            print_field("TacIntToDouble", "", ++t);
            TacIntToDouble* p_node = static_cast<TacIntToDouble*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacUIntToDouble_t: {
            print_field("TacUIntToDouble", "", ++t);
            TacUIntToDouble* p_node = static_cast<TacUIntToDouble*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacFunCall_t: {
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
        case AST_T::TacUnary_t: {
            print_field("TacUnary", "", ++t);
            TacUnary* p_node = static_cast<TacUnary*>(node);
            print_ast(p_node->unary_op.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacBinary_t: {
            print_field("TacBinary", "", ++t);
            TacBinary* p_node = static_cast<TacBinary*>(node);
            print_ast(p_node->binary_op.get(), t);
            print_ast(p_node->src1.get(), t);
            print_ast(p_node->src2.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacCopy_t: {
            print_field("TacCopy", "", ++t);
            TacCopy* p_node = static_cast<TacCopy*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacGetAddress_t: {
            print_field("TacGetAddress", "", ++t);
            TacGetAddress* p_node = static_cast<TacGetAddress*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacLoad_t: {
            print_field("TacLoad", "", ++t);
            TacLoad* p_node = static_cast<TacLoad*>(node);
            print_ast(p_node->src_ptr.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacStore_t: {
            print_field("TacStore", "", ++t);
            TacStore* p_node = static_cast<TacStore*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst_ptr.get(), t);
            break;
        }
        case AST_T::TacAddPtr_t: {
            print_field("TacAddPtr", "", ++t);
            TacAddPtr* p_node = static_cast<TacAddPtr*>(node);
            print_field("TLong", std::to_string(p_node->scale), t + 1);
            print_ast(p_node->src_ptr.get(), t);
            print_ast(p_node->index.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacCopyToOffset_t: {
            print_field("TacCopyToOffset", "", ++t);
            TacCopyToOffset* p_node = static_cast<TacCopyToOffset*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->dst_name], t + 1);
            print_field("TLong", std::to_string(p_node->offset), t + 1);
            print_ast(p_node->src.get(), t);
            break;
        }
        case AST_T::TacCopyFromOffset_t: {
            print_field("TacCopyFromOffset", "", ++t);
            TacCopyFromOffset* p_node = static_cast<TacCopyFromOffset*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->src_name], t + 1);
            print_field("TLong", std::to_string(p_node->offset), t + 1);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacJump_t: {
            print_field("TacJump", "", ++t);
            TacJump* p_node = static_cast<TacJump*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            break;
        }
        case AST_T::TacJumpIfZero_t: {
            print_field("TacJumpIfZero", "", ++t);
            TacJumpIfZero* p_node = static_cast<TacJumpIfZero*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->condition.get(), t);
            break;
        }
        case AST_T::TacJumpIfNotZero_t: {
            print_field("TacJumpIfNotZero", "", ++t);
            TacJumpIfNotZero* p_node = static_cast<TacJumpIfNotZero*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->condition.get(), t);
            break;
        }
        case AST_T::TacLabel_t: {
            print_field("TacLabel", "", ++t);
            TacLabel* p_node = static_cast<TacLabel*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            break;
        }
        case AST_T::TacTopLevel_t: {
            print_field("TacTopLevel", "", ++t);
            break;
        }
        case AST_T::TacFunction_t: {
            print_field("TacFunction", "", ++t);
            TacFunction* p_node = static_cast<TacFunction*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("Bool", std::to_string(p_node->is_global), t + 1);
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
        case AST_T::TacStaticVariable_t: {
            print_field("TacStaticVariable", "", ++t);
            TacStaticVariable* p_node = static_cast<TacStaticVariable*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("Bool", std::to_string(p_node->is_global), t + 1);
            print_ast(p_node->static_init_type.get(), t);
            print_field("List[" + std::to_string(p_node->static_inits.size()) + "]", "", t + 1);
            for (const auto& item : p_node->static_inits) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_T::TacStaticConstant_t: {
            print_field("TacStaticConstant", "", ++t);
            TacStaticConstant* p_node = static_cast<TacStaticConstant*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_ast(p_node->static_init_type.get(), t);
            print_ast(p_node->static_init.get(), t);
            break;
        }
        case AST_T::TacProgram_t: {
            print_field("TacProgram", "", ++t);
            TacProgram* p_node = static_cast<TacProgram*>(node);
            print_field("List[" + std::to_string(p_node->static_constant_top_levels.size()) + "]", "", t + 1);
            for (const auto& item : p_node->static_constant_top_levels) {
                print_ast(item.get(), t + 1);
            }
            print_field("List[" + std::to_string(p_node->static_variable_top_levels.size()) + "]", "", t + 1);
            for (const auto& item : p_node->static_variable_top_levels) {
                print_ast(item.get(), t + 1);
            }
            print_field("List[" + std::to_string(p_node->function_top_levels.size()) + "]", "", t + 1);
            for (const auto& item : p_node->function_top_levels) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_T::AsmReg_t: {
            print_field("AsmReg", "", ++t);
            break;
        }
        case AST_T::AsmAx_t: {
            print_field("AsmAx", "", ++t);
            break;
        }
        case AST_T::AsmBx_t: {
            print_field("AsmBx", "", ++t);
            break;
        }
        case AST_T::AsmCx_t: {
            print_field("AsmCx", "", ++t);
            break;
        }
        case AST_T::AsmDx_t: {
            print_field("AsmDx", "", ++t);
            break;
        }
        case AST_T::AsmDi_t: {
            print_field("AsmDi", "", ++t);
            break;
        }
        case AST_T::AsmSi_t: {
            print_field("AsmSi", "", ++t);
            break;
        }
        case AST_T::AsmR8_t: {
            print_field("AsmR8", "", ++t);
            break;
        }
        case AST_T::AsmR9_t: {
            print_field("AsmR9", "", ++t);
            break;
        }
        case AST_T::AsmR10_t: {
            print_field("AsmR10", "", ++t);
            break;
        }
        case AST_T::AsmR11_t: {
            print_field("AsmR11", "", ++t);
            break;
        }
        case AST_T::AsmR12_t: {
            print_field("AsmR12", "", ++t);
            break;
        }
        case AST_T::AsmR13_t: {
            print_field("AsmR13", "", ++t);
            break;
        }
        case AST_T::AsmR14_t: {
            print_field("AsmR14", "", ++t);
            break;
        }
        case AST_T::AsmR15_t: {
            print_field("AsmR15", "", ++t);
            break;
        }
        case AST_T::AsmSp_t: {
            print_field("AsmSp", "", ++t);
            break;
        }
        case AST_T::AsmBp_t: {
            print_field("AsmBp", "", ++t);
            break;
        }
        case AST_T::AsmXMM0_t: {
            print_field("AsmXMM0", "", ++t);
            break;
        }
        case AST_T::AsmXMM1_t: {
            print_field("AsmXMM1", "", ++t);
            break;
        }
        case AST_T::AsmXMM2_t: {
            print_field("AsmXMM2", "", ++t);
            break;
        }
        case AST_T::AsmXMM3_t: {
            print_field("AsmXMM3", "", ++t);
            break;
        }
        case AST_T::AsmXMM4_t: {
            print_field("AsmXMM4", "", ++t);
            break;
        }
        case AST_T::AsmXMM5_t: {
            print_field("AsmXMM5", "", ++t);
            break;
        }
        case AST_T::AsmXMM6_t: {
            print_field("AsmXMM6", "", ++t);
            break;
        }
        case AST_T::AsmXMM7_t: {
            print_field("AsmXMM7", "", ++t);
            break;
        }
        case AST_T::AsmXMM8_t: {
            print_field("AsmXMM8", "", ++t);
            break;
        }
        case AST_T::AsmXMM9_t: {
            print_field("AsmXMM9", "", ++t);
            break;
        }
        case AST_T::AsmXMM10_t: {
            print_field("AsmXMM10", "", ++t);
            break;
        }
        case AST_T::AsmXMM11_t: {
            print_field("AsmXMM11", "", ++t);
            break;
        }
        case AST_T::AsmXMM12_t: {
            print_field("AsmXMM12", "", ++t);
            break;
        }
        case AST_T::AsmXMM13_t: {
            print_field("AsmXMM13", "", ++t);
            break;
        }
        case AST_T::AsmXMM14_t: {
            print_field("AsmXMM14", "", ++t);
            break;
        }
        case AST_T::AsmXMM15_t: {
            print_field("AsmXMM15", "", ++t);
            break;
        }
        case AST_T::AsmCondCode_t: {
            print_field("AsmCondCode", "", ++t);
            break;
        }
        case AST_T::AsmE_t: {
            print_field("AsmE", "", ++t);
            break;
        }
        case AST_T::AsmNE_t: {
            print_field("AsmNE", "", ++t);
            break;
        }
        case AST_T::AsmG_t: {
            print_field("AsmG", "", ++t);
            break;
        }
        case AST_T::AsmGE_t: {
            print_field("AsmGE", "", ++t);
            break;
        }
        case AST_T::AsmL_t: {
            print_field("AsmL", "", ++t);
            break;
        }
        case AST_T::AsmLE_t: {
            print_field("AsmLE", "", ++t);
            break;
        }
        case AST_T::AsmA_t: {
            print_field("AsmA", "", ++t);
            break;
        }
        case AST_T::AsmAE_t: {
            print_field("AsmAE", "", ++t);
            break;
        }
        case AST_T::AsmB_t: {
            print_field("AsmB", "", ++t);
            break;
        }
        case AST_T::AsmBE_t: {
            print_field("AsmBE", "", ++t);
            break;
        }
        case AST_T::AsmP_t: {
            print_field("AsmP", "", ++t);
            break;
        }
        case AST_T::AsmOperand_t: {
            print_field("AsmOperand", "", ++t);
            break;
        }
        case AST_T::AsmImm_t: {
            print_field("AsmImm", "", ++t);
            AsmImm* p_node = static_cast<AsmImm*>(node);
            print_field("TULong", std::to_string(p_node->value), t + 1);
            print_field("Bool", std::to_string(p_node->is_byte), t + 1);
            print_field("Bool", std::to_string(p_node->is_quad), t + 1);
            print_field("Bool", std::to_string(p_node->is_neg), t + 1);
            break;
        }
        case AST_T::AsmRegister_t: {
            print_field("AsmRegister", "", ++t);
            AsmRegister* p_node = static_cast<AsmRegister*>(node);
            print_ast(p_node->reg.get(), t);
            break;
        }
        case AST_T::AsmPseudo_t: {
            print_field("AsmPseudo", "", ++t);
            AsmPseudo* p_node = static_cast<AsmPseudo*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            break;
        }
        case AST_T::AsmMemory_t: {
            print_field("AsmMemory", "", ++t);
            AsmMemory* p_node = static_cast<AsmMemory*>(node);
            print_field("TLong", std::to_string(p_node->value), t + 1);
            print_ast(p_node->reg.get(), t);
            break;
        }
        case AST_T::AsmData_t: {
            print_field("AsmData", "", ++t);
            AsmData* p_node = static_cast<AsmData*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("TLong", std::to_string(p_node->offset), t + 1);
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            print_field("AsmPseudoMem", "", ++t);
            AsmPseudoMem* p_node = static_cast<AsmPseudoMem*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("TLong", std::to_string(p_node->offset), t + 1);
            break;
        }
        case AST_T::AsmIndexed_t: {
            print_field("AsmIndexed", "", ++t);
            AsmIndexed* p_node = static_cast<AsmIndexed*>(node);
            print_field("TLong", std::to_string(p_node->scale), t + 1);
            print_ast(p_node->reg_base.get(), t);
            print_ast(p_node->reg_index.get(), t);
            break;
        }
        case AST_T::AsmBinaryOp_t: {
            print_field("AsmBinaryOp", "", ++t);
            break;
        }
        case AST_T::AsmAdd_t: {
            print_field("AsmAdd", "", ++t);
            break;
        }
        case AST_T::AsmSub_t: {
            print_field("AsmSub", "", ++t);
            break;
        }
        case AST_T::AsmMult_t: {
            print_field("AsmMult", "", ++t);
            break;
        }
        case AST_T::AsmDivDouble_t: {
            print_field("AsmDivDouble", "", ++t);
            break;
        }
        case AST_T::AsmBitAnd_t: {
            print_field("AsmBitAnd", "", ++t);
            break;
        }
        case AST_T::AsmBitOr_t: {
            print_field("AsmBitOr", "", ++t);
            break;
        }
        case AST_T::AsmBitXor_t: {
            print_field("AsmBitXor", "", ++t);
            break;
        }
        case AST_T::AsmBitShiftLeft_t: {
            print_field("AsmBitShiftLeft", "", ++t);
            break;
        }
        case AST_T::AsmBitShiftRight_t: {
            print_field("AsmBitShiftRight", "", ++t);
            break;
        }
        case AST_T::AsmBitShrArithmetic_t: {
            print_field("AsmBitShrArithmetic", "", ++t);
            break;
        }
        case AST_T::AsmUnaryOp_t: {
            print_field("AsmUnaryOp", "", ++t);
            break;
        }
        case AST_T::AsmNot_t: {
            print_field("AsmNot", "", ++t);
            break;
        }
        case AST_T::AsmNeg_t: {
            print_field("AsmNeg", "", ++t);
            break;
        }
        case AST_T::AsmShr_t: {
            print_field("AsmShr", "", ++t);
            break;
        }
        case AST_T::AsmInstruction_t: {
            print_field("AsmInstruction", "", ++t);
            break;
        }
        case AST_T::AsmMov_t: {
            print_field("AsmMov", "", ++t);
            AsmMov* p_node = static_cast<AsmMov*>(node);
            print_ast(p_node->assembly_type.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmMovSx_t: {
            print_field("AsmMovSx", "", ++t);
            AsmMovSx* p_node = static_cast<AsmMovSx*>(node);
            print_ast(p_node->assembly_type_src.get(), t);
            print_ast(p_node->assembly_type_dst.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmMovZeroExtend_t: {
            print_field("AsmMovZeroExtend", "", ++t);
            AsmMovZeroExtend* p_node = static_cast<AsmMovZeroExtend*>(node);
            print_ast(p_node->assembly_type_src.get(), t);
            print_ast(p_node->assembly_type_dst.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmLea_t: {
            print_field("AsmLea", "", ++t);
            AsmLea* p_node = static_cast<AsmLea*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmCvttsd2si_t: {
            print_field("AsmCvttsd2si", "", ++t);
            AsmCvttsd2si* p_node = static_cast<AsmCvttsd2si*>(node);
            print_ast(p_node->assembly_type.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmCvtsi2sd_t: {
            print_field("AsmCvtsi2sd", "", ++t);
            AsmCvtsi2sd* p_node = static_cast<AsmCvtsi2sd*>(node);
            print_ast(p_node->assembly_type.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmUnary_t: {
            print_field("AsmUnary", "", ++t);
            AsmUnary* p_node = static_cast<AsmUnary*>(node);
            print_ast(p_node->unary_op.get(), t);
            print_ast(p_node->assembly_type.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmBinary_t: {
            print_field("AsmBinary", "", ++t);
            AsmBinary* p_node = static_cast<AsmBinary*>(node);
            print_ast(p_node->binary_op.get(), t);
            print_ast(p_node->assembly_type.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmCmp_t: {
            print_field("AsmCmp", "", ++t);
            AsmCmp* p_node = static_cast<AsmCmp*>(node);
            print_ast(p_node->assembly_type.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmIdiv_t: {
            print_field("AsmIdiv", "", ++t);
            AsmIdiv* p_node = static_cast<AsmIdiv*>(node);
            print_ast(p_node->assembly_type.get(), t);
            print_ast(p_node->src.get(), t);
            break;
        }
        case AST_T::AsmDiv_t: {
            print_field("AsmDiv", "", ++t);
            AsmDiv* p_node = static_cast<AsmDiv*>(node);
            print_ast(p_node->assembly_type.get(), t);
            print_ast(p_node->src.get(), t);
            break;
        }
        case AST_T::AsmCdq_t: {
            print_field("AsmCdq", "", ++t);
            AsmCdq* p_node = static_cast<AsmCdq*>(node);
            print_ast(p_node->assembly_type.get(), t);
            break;
        }
        case AST_T::AsmJmp_t: {
            print_field("AsmJmp", "", ++t);
            AsmJmp* p_node = static_cast<AsmJmp*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            break;
        }
        case AST_T::AsmJmpCC_t: {
            print_field("AsmJmpCC", "", ++t);
            AsmJmpCC* p_node = static_cast<AsmJmpCC*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->target], t + 1);
            print_ast(p_node->cond_code.get(), t);
            break;
        }
        case AST_T::AsmSetCC_t: {
            print_field("AsmSetCC", "", ++t);
            AsmSetCC* p_node = static_cast<AsmSetCC*>(node);
            print_ast(p_node->cond_code.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmLabel_t: {
            print_field("AsmLabel", "", ++t);
            AsmLabel* p_node = static_cast<AsmLabel*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            break;
        }
        case AST_T::AsmPush_t: {
            print_field("AsmPush", "", ++t);
            AsmPush* p_node = static_cast<AsmPush*>(node);
            print_ast(p_node->src.get(), t);
            break;
        }
        case AST_T::AsmPop_t: {
            print_field("AsmPop", "", ++t);
            AsmPop* p_node = static_cast<AsmPop*>(node);
            print_ast(p_node->reg.get(), t);
            break;
        }
        case AST_T::AsmCall_t: {
            print_field("AsmCall", "", ++t);
            AsmCall* p_node = static_cast<AsmCall*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            break;
        }
        case AST_T::AsmRet_t: {
            print_field("AsmRet", "", ++t);
            break;
        }
        case AST_T::AsmTopLevel_t: {
            print_field("AsmTopLevel", "", ++t);
            break;
        }
        case AST_T::AsmFunction_t: {
            print_field("AsmFunction", "", ++t);
            AsmFunction* p_node = static_cast<AsmFunction*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("Bool", std::to_string(p_node->is_global), t + 1);
            print_field("Bool", std::to_string(p_node->is_return_memory), t + 1);
            print_field("List[" + std::to_string(p_node->instructions.size()) + "]", "", t + 1);
            for (const auto& item : p_node->instructions) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_T::AsmStaticVariable_t: {
            print_field("AsmStaticVariable", "", ++t);
            AsmStaticVariable* p_node = static_cast<AsmStaticVariable*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("TInt", std::to_string(p_node->alignment), t + 1);
            print_field("Bool", std::to_string(p_node->is_global), t + 1);
            print_field("List[" + std::to_string(p_node->static_inits.size()) + "]", "", t + 1);
            for (const auto& item : p_node->static_inits) {
                print_ast(item.get(), t + 1);
            }
            break;
        }
        case AST_T::AsmStaticConstant_t: {
            print_field("AsmStaticConstant", "", ++t);
            AsmStaticConstant* p_node = static_cast<AsmStaticConstant*>(node);
            print_field("TIdentifier", identifiers->hash_table[p_node->name], t + 1);
            print_field("TInt", std::to_string(p_node->alignment), t + 1);
            print_ast(p_node->static_init.get(), t);
            break;
        }
        case AST_T::AsmProgram_t: {
            print_field("AsmProgram", "", ++t);
            AsmProgram* p_node = static_cast<AsmProgram*>(node);
            print_field("List[" + std::to_string(p_node->static_constant_top_levels.size()) + "]", "", t + 1);
            for (const auto& item : p_node->static_constant_top_levels) {
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
    std::cout << "\nDict(" << std::to_string(frontend->string_constant_table.size()) << "):";
    for (const auto& static_constant : frontend->string_constant_table) {
        print_field("[" + identifiers->hash_table[static_constant.first] + "]", "", 2);
        if (frontend->symbol_table.find(static_constant.second) != frontend->symbol_table.end()
            && frontend->symbol_table[static_constant.second]->attrs->type() == AST_T::ConstantAttr_t) {
            ConstantAttr* constant_attr =
                static_cast<ConstantAttr*>(frontend->symbol_table[static_constant.second]->attrs.get());
            if (constant_attr->static_init->type() == AST_T::StringInit_t) {
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
    std::cout << "\nDict(" << std::to_string(backend->backend_symbol_table.size()) << "):";
    for (const auto& backend_symbol : backend->backend_symbol_table) {
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
