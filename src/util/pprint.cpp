#include "util/pprint.hpp"
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
        case AST_T::CConst_t: {
            field("CConst", "", ++t);
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
        case AST_T::CConstUInt_t: {
            field("CConstUInt", "", ++t);
            CConstUInt* p_node = static_cast<CConstUInt*>(node);
            field("TUInt", std::to_string(p_node->value), t+1);
            break;
        }
        case AST_T::CConstULong_t: {
            field("CConstULong", "", ++t);
            CConstULong* p_node = static_cast<CConstULong*>(node);
            field("TULong", std::to_string(p_node->value), t+1);
            break;
        }
        case AST_T::CConstDouble_t: {
            field("CConstDouble", "", ++t);
            CConstDouble* p_node = static_cast<CConstDouble*>(node);
            field("TDouble", std::to_string(p_node->value), t+1);
            break;
        }
        case AST_T::CConstChar_t: {
            field("CConstChar", "", ++t);
            CConstChar* p_node = static_cast<CConstChar*>(node);
            field("TChar", std::to_string(p_node->value), t+1);
            break;
        }
        case AST_T::CConstUChar_t: {
            field("CConstUChar", "", ++t);
            CConstUChar* p_node = static_cast<CConstUChar*>(node);
            field("TUChar", std::to_string(p_node->value), t+1);
            break;
        }
        case AST_T::CStringLiteral_t: {
            field("CStringLiteral", "", ++t);
            CStringLiteral* p_node = static_cast<CStringLiteral*>(node);
            field("List[" + std::to_string(p_node->value.size()) + "]", "", t+1);
            for(const auto& item: p_node->value) {
                field("TChar", std::to_string(item), t+2);
            }
            break;
        }
        case AST_T::Type_t: {
            field("Type", "", ++t);
            break;
        }
        case AST_T::Char_t: {
            field("Char", "", ++t);
            break;
        }
        case AST_T::SChar_t: {
            field("SChar", "", ++t);
            break;
        }
        case AST_T::UChar_t: {
            field("UChar", "", ++t);
            break;
        }
        case AST_T::Int_t: {
            field("Int", "", ++t);
            break;
        }
        case AST_T::Long_t: {
            field("Long", "", ++t);
            break;
        }
        case AST_T::UInt_t: {
            field("UInt", "", ++t);
            break;
        }
        case AST_T::ULong_t: {
            field("ULong", "", ++t);
            break;
        }
        case AST_T::Double_t: {
            field("Double", "", ++t);
            break;
        }
        case AST_T::Void_t: {
            field("Void", "", ++t);
            break;
        }
        case AST_T::FunType_t: {
            field("FunType", "", ++t);
            FunType* p_node = static_cast<FunType*>(node);
            field("List[" + std::to_string(p_node->param_types.size()) + "]", "", t+1);
            for(const auto& item: p_node->param_types) {
                print_ast(item.get(), t+1);
            }
            print_ast(p_node->ret_type.get(), t);
            break;
        }
        case AST_T::Pointer_t: {
            field("Pointer", "", ++t);
            Pointer* p_node = static_cast<Pointer*>(node);
            print_ast(p_node->ref_type.get(), t);
            break;
        }
        case AST_T::Array_t: {
            field("Array", "", ++t);
            Array* p_node = static_cast<Array*>(node);
            field("TLong", std::to_string(p_node->size), t+1);
            print_ast(p_node->elem_type.get(), t);
            break;
        }
        case AST_T::Structure_t: {
            field("Structure", "", ++t);
            Structure* p_node = static_cast<Structure*>(node);
            field("TIdentifier", p_node->tag, t+1);
            break;
        }
        case AST_T::StaticInit_t: {
            field("StaticInit", "", ++t);
            break;
        }
        case AST_T::IntInit_t: {
            field("IntInit", "", ++t);
            IntInit* p_node = static_cast<IntInit*>(node);
            field("TInt", std::to_string(p_node->value), t+1);
            break;
        }
        case AST_T::LongInit_t: {
            field("LongInit", "", ++t);
            LongInit* p_node = static_cast<LongInit*>(node);
            field("TLong", std::to_string(p_node->value), t+1);
            break;
        }
        case AST_T::UIntInit_t: {
            field("UIntInit", "", ++t);
            UIntInit* p_node = static_cast<UIntInit*>(node);
            field("TUInt", std::to_string(p_node->value), t+1);
            break;
        }
        case AST_T::ULongInit_t: {
            field("ULongInit", "", ++t);
            ULongInit* p_node = static_cast<ULongInit*>(node);
            field("TULong", std::to_string(p_node->value), t+1);
            break;
        }
        case AST_T::CharInit_t: {
            field("CharInit", "", ++t);
            CharInit* p_node = static_cast<CharInit*>(node);
            field("TChar", std::to_string(p_node->value), t+1);
            break;
        }
        case AST_T::UCharInit_t: {
            field("UCharInit", "", ++t);
            UCharInit* p_node = static_cast<UCharInit*>(node);
            field("TUChar", std::to_string(p_node->value), t+1);
            break;
        }
        case AST_T::DoubleInit_t: {
            field("DoubleInit", "", ++t);
            DoubleInit* p_node = static_cast<DoubleInit*>(node);
            field("TDouble", std::to_string(p_node->value), t+1);
            field("TULong", std::to_string(p_node->binary), t+1);
            break;
        }
        case AST_T::ZeroInit_t: {
            field("ZeroInit", "", ++t);
            ZeroInit* p_node = static_cast<ZeroInit*>(node);
            field("TLong", std::to_string(p_node->byte), t+1);
            break;
        }
        case AST_T::StringInit_t: {
            field("StringInit", "", ++t);
            StringInit* p_node = static_cast<StringInit*>(node);
            field("Bool", std::to_string(p_node->is_null_terminated), t+1);
            field("TIdentifier", p_node->string_constant, t+1);
            print_ast(p_node->literal.get(), t);
            break;
        }
        case AST_T::PointerInit_t: {
            field("PointerInit", "", ++t);
            PointerInit* p_node = static_cast<PointerInit*>(node);
            field("TIdentifier", p_node->name, t+1);
            break;
        }
        case AST_T::InitialValue_t: {
            field("InitialValue", "", ++t);
            break;
        }
        case AST_T::Tentative_t: {
            field("Tentative", "", ++t);
            break;
        }
        case AST_T::Initial_t: {
            field("Initial", "", ++t);
            Initial* p_node = static_cast<Initial*>(node);
            field("List[" + std::to_string(p_node->static_inits.size()) + "]", "", t+1);
            for(const auto& item: p_node->static_inits) {
                print_ast(item.get(), t+1);
            }
            break;
        }
        case AST_T::NoInitializer_t: {
            field("NoInitializer", "", ++t);
            break;
        }
        case AST_T::IdentifierAttr_t: {
            field("IdentifierAttr", "", ++t);
            break;
        }
        case AST_T::FunAttr_t: {
            field("FunAttr", "", ++t);
            FunAttr* p_node = static_cast<FunAttr*>(node);
            field("Bool", std::to_string(p_node->is_defined), t+1);
            field("Bool", std::to_string(p_node->is_global), t+1);
            break;
        }
        case AST_T::StaticAttr_t: {
            field("StaticAttr", "", ++t);
            StaticAttr* p_node = static_cast<StaticAttr*>(node);
            field("Bool", std::to_string(p_node->is_global), t+1);
            print_ast(p_node->init.get(), t);
            break;
        }
        case AST_T::ConstantAttr_t: {
            field("ConstantAttr", "", ++t);
            ConstantAttr* p_node = static_cast<ConstantAttr*>(node);
            print_ast(p_node->static_init.get(), t);
            break;
        }
        case AST_T::LocalAttr_t: {
            field("LocalAttr", "", ++t);
            break;
        }
        case AST_T::Symbol_t: {
            field("Symbol", "", ++t);
            Symbol* p_node = static_cast<Symbol*>(node);
            print_ast(p_node->type_t.get(), t);
            print_ast(p_node->attrs.get(), t);
            break;
        }
        case AST_T::StructMember_t: {
            field("StructMember", "", ++t);
            StructMember* p_node = static_cast<StructMember*>(node);
            field("TLong", std::to_string(p_node->offset), t+1);
            print_ast(p_node->member_type.get(), t);
            break;
        }
        case AST_T::StructTypedef_t: {
            field("StructTypedef", "", ++t);
            StructTypedef* p_node = static_cast<StructTypedef*>(node);
            field("TInt", std::to_string(p_node->alignment), t+1);
            field("TLong", std::to_string(p_node->size), t+1);
            field("List[" + std::to_string(p_node->member_names.size()) + "]", "", t+1);
            for(const auto& item: p_node->member_names) {
                field("TIdentifier", item, t+2);
            }
            field("Dict[" + std::to_string(p_node->members.size()) + "]", "", t+1);
            for(const auto& item: p_node->members) {
                field("[" + item.first + "]", "", t+2);
                print_ast(item.second.get(), t+2);
            }
            break;
        }
        case AST_T::AssemblyType_t: {
            field("AssemblyType", "", ++t);
            break;
        }
        case AST_T::Byte_t: {
            field("Byte", "", ++t);
            break;
        }
        case AST_T::LongWord_t: {
            field("LongWord", "", ++t);
            break;
        }
        case AST_T::QuadWord_t: {
            field("QuadWord", "", ++t);
            break;
        }
        case AST_T::BackendDouble_t: {
            field("BackendDouble", "", ++t);
            break;
        }
        case AST_T::ByteArray_t: {
            field("ByteArray", "", ++t);
            ByteArray* p_node = static_cast<ByteArray*>(node);
            field("TLong", std::to_string(p_node->size), t+1);
            field("TInt", std::to_string(p_node->alignment), t+1);
            break;
        }
        case AST_T::BackendSymbol_t: {
            field("BackendSymbol", "", ++t);
            break;
        }
        case AST_T::BackendObj_t: {
            field("BackendObj", "", ++t);
            BackendObj* p_node = static_cast<BackendObj*>(node);
            field("Bool", std::to_string(p_node->is_static), t+1);
            field("Bool", std::to_string(p_node->is_constant), t+1);
            print_ast(p_node->assembly_type.get(), t);
            break;
        }
        case AST_T::BackendFun_t: {
            field("BackendFun", "", ++t);
            BackendFun* p_node = static_cast<BackendFun*>(node);
            field("Bool", std::to_string(p_node->is_defined), t+1);
            break;
        }
        case AST_T::CUnaryOp_t: {
            field("CUnaryOp", "", ++t);
            break;
        }
        case AST_T::CComplement_t: {
            field("CComplement", "", ++t);
            break;
        }
        case AST_T::CNegate_t: {
            field("CNegate", "", ++t);
            break;
        }
        case AST_T::CNot_t: {
            field("CNot", "", ++t);
            break;
        }
        case AST_T::CBinaryOp_t: {
            field("CBinaryOp", "", ++t);
            break;
        }
        case AST_T::CAdd_t: {
            field("CAdd", "", ++t);
            break;
        }
        case AST_T::CSubtract_t: {
            field("CSubtract", "", ++t);
            break;
        }
        case AST_T::CMultiply_t: {
            field("CMultiply", "", ++t);
            break;
        }
        case AST_T::CDivide_t: {
            field("CDivide", "", ++t);
            break;
        }
        case AST_T::CRemainder_t: {
            field("CRemainder", "", ++t);
            break;
        }
        case AST_T::CBitAnd_t: {
            field("CBitAnd", "", ++t);
            break;
        }
        case AST_T::CBitOr_t: {
            field("CBitOr", "", ++t);
            break;
        }
        case AST_T::CBitXor_t: {
            field("CBitXor", "", ++t);
            break;
        }
        case AST_T::CBitShiftLeft_t: {
            field("CBitShiftLeft", "", ++t);
            break;
        }
        case AST_T::CBitShiftRight_t: {
            field("CBitShiftRight", "", ++t);
            break;
        }
        case AST_T::CAnd_t: {
            field("CAnd", "", ++t);
            break;
        }
        case AST_T::COr_t: {
            field("COr", "", ++t);
            break;
        }
        case AST_T::CEqual_t: {
            field("CEqual", "", ++t);
            break;
        }
        case AST_T::CNotEqual_t: {
            field("CNotEqual", "", ++t);
            break;
        }
        case AST_T::CLessThan_t: {
            field("CLessThan", "", ++t);
            break;
        }
        case AST_T::CLessOrEqual_t: {
            field("CLessOrEqual", "", ++t);
            break;
        }
        case AST_T::CGreaterThan_t: {
            field("CGreaterThan", "", ++t);
            break;
        }
        case AST_T::CGreaterOrEqual_t: {
            field("CGreaterOrEqual", "", ++t);
            break;
        }
        case AST_T::CAbstractDeclarator_t: {
            field("CAbstractDeclarator", "", ++t);
            break;
        }
        case AST_T::CAbstractPointer_t: {
            field("CAbstractPointer", "", ++t);
            CAbstractPointer* p_node = static_cast<CAbstractPointer*>(node);
            print_ast(p_node->abstract_declarator.get(), t);
            break;
        }
        case AST_T::CAbstractArray_t: {
            field("CAbstractArray", "", ++t);
            CAbstractArray* p_node = static_cast<CAbstractArray*>(node);
            field("TLong", std::to_string(p_node->size), t+1);
            print_ast(p_node->abstract_declarator.get(), t);
            break;
        }
        case AST_T::CAbstractBase_t: {
            field("CAbstractBase", "", ++t);
            break;
        }
        case AST_T::CParam_t: {
            field("CParam", "", ++t);
            CParam* p_node = static_cast<CParam*>(node);
            print_ast(p_node->declarator.get(), t);
            print_ast(p_node->param_type.get(), t);
            break;
        }
        case AST_T::CDeclarator_t: {
            field("CDeclarator", "", ++t);
            break;
        }
        case AST_T::CIdent_t: {
            field("CIdent", "", ++t);
            CIdent* p_node = static_cast<CIdent*>(node);
            field("TIdentifier", p_node->name, t+1);
            break;
        }
        case AST_T::CPointerDeclarator_t: {
            field("CPointerDeclarator", "", ++t);
            CPointerDeclarator* p_node = static_cast<CPointerDeclarator*>(node);
            print_ast(p_node->declarator.get(), t);
            break;
        }
        case AST_T::CArrayDeclarator_t: {
            field("CArrayDeclarator", "", ++t);
            CArrayDeclarator* p_node = static_cast<CArrayDeclarator*>(node);
            field("TLong", std::to_string(p_node->size), t+1);
            print_ast(p_node->declarator.get(), t);
            break;
        }
        case AST_T::CFunDeclarator_t: {
            field("CFunDeclarator", "", ++t);
            CFunDeclarator* p_node = static_cast<CFunDeclarator*>(node);
            field("List[" + std::to_string(p_node->param_list.size()) + "]", "", t+1);
            for(const auto& item: p_node->param_list) {
                print_ast(item.get(), t+1);
            }
            print_ast(p_node->declarator.get(), t);
            break;
        }
        case AST_T::CExp_t: {
            field("CExp", "", ++t);
            CExp* p_node = static_cast<CExp*>(node);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CConstant_t: {
            field("CConstant", "", ++t);
            CConstant* p_node = static_cast<CConstant*>(node);
            print_ast(p_node->constant.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CString_t: {
            field("CString", "", ++t);
            CString* p_node = static_cast<CString*>(node);
            print_ast(p_node->literal.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CVar_t: {
            field("CVar", "", ++t);
            CVar* p_node = static_cast<CVar*>(node);
            field("TIdentifier", p_node->name, t+1);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CCast_t: {
            field("CCast", "", ++t);
            CCast* p_node = static_cast<CCast*>(node);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->target_type.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CUnary_t: {
            field("CUnary", "", ++t);
            CUnary* p_node = static_cast<CUnary*>(node);
            print_ast(p_node->unary_op.get(), t);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CBinary_t: {
            field("CBinary", "", ++t);
            CBinary* p_node = static_cast<CBinary*>(node);
            print_ast(p_node->binary_op.get(), t);
            print_ast(p_node->exp_left.get(), t);
            print_ast(p_node->exp_right.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CAssignment_t: {
            field("CAssignment", "", ++t);
            CAssignment* p_node = static_cast<CAssignment*>(node);
            print_ast(p_node->exp_left.get(), t);
            print_ast(p_node->exp_right.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CConditional_t: {
            field("CConditional", "", ++t);
            CConditional* p_node = static_cast<CConditional*>(node);
            print_ast(p_node->condition.get(), t);
            print_ast(p_node->exp_middle.get(), t);
            print_ast(p_node->exp_right.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CFunctionCall_t: {
            field("CFunctionCall", "", ++t);
            CFunctionCall* p_node = static_cast<CFunctionCall*>(node);
            field("TIdentifier", p_node->name, t+1);
            field("List[" + std::to_string(p_node->args.size()) + "]", "", t+1);
            for(const auto& item: p_node->args) {
                print_ast(item.get(), t+1);
            }
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CDereference_t: {
            field("CDereference", "", ++t);
            CDereference* p_node = static_cast<CDereference*>(node);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CAddrOf_t: {
            field("CAddrOf", "", ++t);
            CAddrOf* p_node = static_cast<CAddrOf*>(node);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CSubscript_t: {
            field("CSubscript", "", ++t);
            CSubscript* p_node = static_cast<CSubscript*>(node);
            print_ast(p_node->primary_exp.get(), t);
            print_ast(p_node->subscript_exp.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CSizeOf_t: {
            field("CSizeOf", "", ++t);
            CSizeOf* p_node = static_cast<CSizeOf*>(node);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CSizeOfT_t: {
            field("CSizeOfT", "", ++t);
            CSizeOfT* p_node = static_cast<CSizeOfT*>(node);
            print_ast(p_node->target_type.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CDot_t: {
            field("CDot", "", ++t);
            CDot* p_node = static_cast<CDot*>(node);
            field("TIdentifier", p_node->member, t+1);
            print_ast(p_node->structure.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CArrow_t: {
            field("CArrow", "", ++t);
            CArrow* p_node = static_cast<CArrow*>(node);
            field("TIdentifier", p_node->member, t+1);
            print_ast(p_node->pointer.get(), t);
            print_ast(p_node->exp_type.get(), t);
            break;
        }
        case AST_T::CStatement_t: {
            field("CStatement", "", ++t);
            break;
        }
        case AST_T::CReturn_t: {
            field("CReturn", "", ++t);
            CReturn* p_node = static_cast<CReturn*>(node);
            print_ast(p_node->exp.get(), t);
            break;
        }
        case AST_T::CExpression_t: {
            field("CExpression", "", ++t);
            CExpression* p_node = static_cast<CExpression*>(node);
            print_ast(p_node->exp.get(), t);
            break;
        }
        case AST_T::CIf_t: {
            field("CIf", "", ++t);
            CIf* p_node = static_cast<CIf*>(node);
            print_ast(p_node->condition.get(), t);
            print_ast(p_node->then.get(), t);
            print_ast(p_node->else_fi.get(), t);
            break;
        }
        case AST_T::CGoto_t: {
            field("CGoto", "", ++t);
            CGoto* p_node = static_cast<CGoto*>(node);
            field("TIdentifier", p_node->target, t+1);
            break;
        }
        case AST_T::CLabel_t: {
            field("CLabel", "", ++t);
            CLabel* p_node = static_cast<CLabel*>(node);
            field("TIdentifier", p_node->target, t+1);
            print_ast(p_node->jump_to.get(), t);
            break;
        }
        case AST_T::CCompound_t: {
            field("CCompound", "", ++t);
            CCompound* p_node = static_cast<CCompound*>(node);
            print_ast(p_node->block.get(), t);
            break;
        }
        case AST_T::CWhile_t: {
            field("CWhile", "", ++t);
            CWhile* p_node = static_cast<CWhile*>(node);
            field("TIdentifier", p_node->target, t+1);
            print_ast(p_node->condition.get(), t);
            print_ast(p_node->body.get(), t);
            break;
        }
        case AST_T::CDoWhile_t: {
            field("CDoWhile", "", ++t);
            CDoWhile* p_node = static_cast<CDoWhile*>(node);
            field("TIdentifier", p_node->target, t+1);
            print_ast(p_node->condition.get(), t);
            print_ast(p_node->body.get(), t);
            break;
        }
        case AST_T::CFor_t: {
            field("CFor", "", ++t);
            CFor* p_node = static_cast<CFor*>(node);
            field("TIdentifier", p_node->target, t+1);
            print_ast(p_node->init.get(), t);
            print_ast(p_node->condition.get(), t);
            print_ast(p_node->post.get(), t);
            print_ast(p_node->body.get(), t);
            break;
        }
        case AST_T::CBreak_t: {
            field("CBreak", "", ++t);
            CBreak* p_node = static_cast<CBreak*>(node);
            field("TIdentifier", p_node->target, t+1);
            break;
        }
        case AST_T::CContinue_t: {
            field("CContinue", "", ++t);
            CContinue* p_node = static_cast<CContinue*>(node);
            field("TIdentifier", p_node->target, t+1);
            break;
        }
        case AST_T::CNull_t: {
            field("CNull", "", ++t);
            break;
        }
        case AST_T::CForInit_t: {
            field("CForInit", "", ++t);
            break;
        }
        case AST_T::CInitDecl_t: {
            field("CInitDecl", "", ++t);
            CInitDecl* p_node = static_cast<CInitDecl*>(node);
            print_ast(p_node->init.get(), t);
            break;
        }
        case AST_T::CInitExp_t: {
            field("CInitExp", "", ++t);
            CInitExp* p_node = static_cast<CInitExp*>(node);
            print_ast(p_node->init.get(), t);
            break;
        }
        case AST_T::CBlock_t: {
            field("CBlock", "", ++t);
            break;
        }
        case AST_T::CB_t: {
            field("CB", "", ++t);
            CB* p_node = static_cast<CB*>(node);
            field("List[" + std::to_string(p_node->block_items.size()) + "]", "", t+1);
            for(const auto& item: p_node->block_items) {
                print_ast(item.get(), t+1);
            }
            break;
        }
        case AST_T::CBlockItem_t: {
            field("CBlockItem", "", ++t);
            break;
        }
        case AST_T::CS_t: {
            field("CS", "", ++t);
            CS* p_node = static_cast<CS*>(node);
            print_ast(p_node->statement.get(), t);
            break;
        }
        case AST_T::CD_t: {
            field("CD", "", ++t);
            CD* p_node = static_cast<CD*>(node);
            print_ast(p_node->declaration.get(), t);
            break;
        }
        case AST_T::CStorageClass_t: {
            field("CStorageClass", "", ++t);
            break;
        }
        case AST_T::CStatic_t: {
            field("CStatic", "", ++t);
            break;
        }
        case AST_T::CExtern_t: {
            field("CExtern", "", ++t);
            break;
        }
        case AST_T::CInitializer_t: {
            field("CInitializer", "", ++t);
            CInitializer* p_node = static_cast<CInitializer*>(node);
            print_ast(p_node->init_type.get(), t);
            break;
        }
        case AST_T::CSingleInit_t: {
            field("CSingleInit", "", ++t);
            CSingleInit* p_node = static_cast<CSingleInit*>(node);
            print_ast(p_node->exp.get(), t);
            print_ast(p_node->init_type.get(), t);
            break;
        }
        case AST_T::CCompoundInit_t: {
            field("CCompoundInit", "", ++t);
            CCompoundInit* p_node = static_cast<CCompoundInit*>(node);
            field("List[" + std::to_string(p_node->initializers.size()) + "]", "", t+1);
            for(const auto& item: p_node->initializers) {
                print_ast(item.get(), t+1);
            }
            print_ast(p_node->init_type.get(), t);
            break;
        }
        case AST_T::CMemberDeclaration_t: {
            field("CMemberDeclaration", "", ++t);
            CMemberDeclaration* p_node = static_cast<CMemberDeclaration*>(node);
            field("TIdentifier", p_node->member_name, t+1);
            print_ast(p_node->member_type.get(), t);
            break;
        }
        case AST_T::CStructDeclaration_t: {
            field("CStructDeclaration", "", ++t);
            CStructDeclaration* p_node = static_cast<CStructDeclaration*>(node);
            field("TIdentifier", p_node->tag, t+1);
            field("List[" + std::to_string(p_node->members.size()) + "]", "", t+1);
            for(const auto& item: p_node->members) {
                print_ast(item.get(), t+1);
            }
            break;
        }
        case AST_T::CFunctionDeclaration_t: {
            field("CFunctionDeclaration", "", ++t);
            CFunctionDeclaration* p_node = static_cast<CFunctionDeclaration*>(node);
            field("TIdentifier", p_node->name, t+1);
            field("List[" + std::to_string(p_node->params.size()) + "]", "", t+1);
            for(const auto& item: p_node->params) {
                field("TIdentifier", item, t+2);
            }
            print_ast(p_node->body.get(), t);
            print_ast(p_node->fun_type.get(), t);
            print_ast(p_node->storage_class.get(), t);
            break;
        }
        case AST_T::CVariableDeclaration_t: {
            field("CVariableDeclaration", "", ++t);
            CVariableDeclaration* p_node = static_cast<CVariableDeclaration*>(node);
            field("TIdentifier", p_node->name, t+1);
            print_ast(p_node->init.get(), t);
            print_ast(p_node->var_type.get(), t);
            print_ast(p_node->storage_class.get(), t);
            break;
        }
        case AST_T::CDeclaration_t: {
            field("CDeclaration", "", ++t);
            break;
        }
        case AST_T::CFunDecl_t: {
            field("CFunDecl", "", ++t);
            CFunDecl* p_node = static_cast<CFunDecl*>(node);
            print_ast(p_node->function_decl.get(), t);
            break;
        }
        case AST_T::CVarDecl_t: {
            field("CVarDecl", "", ++t);
            CVarDecl* p_node = static_cast<CVarDecl*>(node);
            print_ast(p_node->variable_decl.get(), t);
            break;
        }
        case AST_T::CStructDecl_t: {
            field("CStructDecl", "", ++t);
            CStructDecl* p_node = static_cast<CStructDecl*>(node);
            print_ast(p_node->struct_decl.get(), t);
            break;
        }
        case AST_T::CProgram_t: {
            field("CProgram", "", ++t);
            CProgram* p_node = static_cast<CProgram*>(node);
            field("List[" + std::to_string(p_node->declarations.size()) + "]", "", t+1);
            for(const auto& item: p_node->declarations) {
                print_ast(item.get(), t+1);
            }
            break;
        }
        case AST_T::TacUnaryOp_t: {
            field("TacUnaryOp", "", ++t);
            break;
        }
        case AST_T::TacComplement_t: {
            field("TacComplement", "", ++t);
            break;
        }
        case AST_T::TacNegate_t: {
            field("TacNegate", "", ++t);
            break;
        }
        case AST_T::TacNot_t: {
            field("TacNot", "", ++t);
            break;
        }
        case AST_T::TacBinaryOp_t: {
            field("TacBinaryOp", "", ++t);
            break;
        }
        case AST_T::TacAdd_t: {
            field("TacAdd", "", ++t);
            break;
        }
        case AST_T::TacSubtract_t: {
            field("TacSubtract", "", ++t);
            break;
        }
        case AST_T::TacMultiply_t: {
            field("TacMultiply", "", ++t);
            break;
        }
        case AST_T::TacDivide_t: {
            field("TacDivide", "", ++t);
            break;
        }
        case AST_T::TacRemainder_t: {
            field("TacRemainder", "", ++t);
            break;
        }
        case AST_T::TacBitAnd_t: {
            field("TacBitAnd", "", ++t);
            break;
        }
        case AST_T::TacBitOr_t: {
            field("TacBitOr", "", ++t);
            break;
        }
        case AST_T::TacBitXor_t: {
            field("TacBitXor", "", ++t);
            break;
        }
        case AST_T::TacBitShiftLeft_t: {
            field("TacBitShiftLeft", "", ++t);
            break;
        }
        case AST_T::TacBitShiftRight_t: {
            field("TacBitShiftRight", "", ++t);
            break;
        }
        case AST_T::TacEqual_t: {
            field("TacEqual", "", ++t);
            break;
        }
        case AST_T::TacNotEqual_t: {
            field("TacNotEqual", "", ++t);
            break;
        }
        case AST_T::TacLessThan_t: {
            field("TacLessThan", "", ++t);
            break;
        }
        case AST_T::TacLessOrEqual_t: {
            field("TacLessOrEqual", "", ++t);
            break;
        }
        case AST_T::TacGreaterThan_t: {
            field("TacGreaterThan", "", ++t);
            break;
        }
        case AST_T::TacGreaterOrEqual_t: {
            field("TacGreaterOrEqual", "", ++t);
            break;
        }
        case AST_T::TacValue_t: {
            field("TacValue", "", ++t);
            break;
        }
        case AST_T::TacConstant_t: {
            field("TacConstant", "", ++t);
            TacConstant* p_node = static_cast<TacConstant*>(node);
            print_ast(p_node->constant.get(), t);
            break;
        }
        case AST_T::TacVariable_t: {
            field("TacVariable", "", ++t);
            TacVariable* p_node = static_cast<TacVariable*>(node);
            field("TIdentifier", p_node->name, t+1);
            break;
        }
        case AST_T::TacExpResult_t: {
            field("TacExpResult", "", ++t);
            break;
        }
        case AST_T::TacPlainOperand_t: {
            field("TacPlainOperand", "", ++t);
            TacPlainOperand* p_node = static_cast<TacPlainOperand*>(node);
            print_ast(p_node->val.get(), t);
            break;
        }
        case AST_T::TacDereferencedPointer_t: {
            field("TacDereferencedPointer", "", ++t);
            TacDereferencedPointer* p_node = static_cast<TacDereferencedPointer*>(node);
            print_ast(p_node->val.get(), t);
            break;
        }
        case AST_T::TacInstruction_t: {
            field("TacInstruction", "", ++t);
            break;
        }
        case AST_T::TacReturn_t: {
            field("TacReturn", "", ++t);
            TacReturn* p_node = static_cast<TacReturn*>(node);
            print_ast(p_node->val.get(), t);
            break;
        }
        case AST_T::TacSignExtend_t: {
            field("TacSignExtend", "", ++t);
            TacSignExtend* p_node = static_cast<TacSignExtend*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacTruncate_t: {
            field("TacTruncate", "", ++t);
            TacTruncate* p_node = static_cast<TacTruncate*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacZeroExtend_t: {
            field("TacZeroExtend", "", ++t);
            TacZeroExtend* p_node = static_cast<TacZeroExtend*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacDoubleToInt_t: {
            field("TacDoubleToInt", "", ++t);
            TacDoubleToInt* p_node = static_cast<TacDoubleToInt*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacDoubleToUInt_t: {
            field("TacDoubleToUInt", "", ++t);
            TacDoubleToUInt* p_node = static_cast<TacDoubleToUInt*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacIntToDouble_t: {
            field("TacIntToDouble", "", ++t);
            TacIntToDouble* p_node = static_cast<TacIntToDouble*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacUIntToDouble_t: {
            field("TacUIntToDouble", "", ++t);
            TacUIntToDouble* p_node = static_cast<TacUIntToDouble*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacFunCall_t: {
            field("TacFunCall", "", ++t);
            TacFunCall* p_node = static_cast<TacFunCall*>(node);
            field("TIdentifier", p_node->name, t+1);
            field("List[" + std::to_string(p_node->args.size()) + "]", "", t+1);
            for(const auto& item: p_node->args) {
                print_ast(item.get(), t+1);
            }
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacUnary_t: {
            field("TacUnary", "", ++t);
            TacUnary* p_node = static_cast<TacUnary*>(node);
            print_ast(p_node->unary_op.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacBinary_t: {
            field("TacBinary", "", ++t);
            TacBinary* p_node = static_cast<TacBinary*>(node);
            print_ast(p_node->binary_op.get(), t);
            print_ast(p_node->src1.get(), t);
            print_ast(p_node->src2.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacCopy_t: {
            field("TacCopy", "", ++t);
            TacCopy* p_node = static_cast<TacCopy*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacGetAddress_t: {
            field("TacGetAddress", "", ++t);
            TacGetAddress* p_node = static_cast<TacGetAddress*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacLoad_t: {
            field("TacLoad", "", ++t);
            TacLoad* p_node = static_cast<TacLoad*>(node);
            print_ast(p_node->src_ptr.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacStore_t: {
            field("TacStore", "", ++t);
            TacStore* p_node = static_cast<TacStore*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst_ptr.get(), t);
            break;
        }
        case AST_T::TacAddPtr_t: {
            field("TacAddPtr", "", ++t);
            TacAddPtr* p_node = static_cast<TacAddPtr*>(node);
            field("TLong", std::to_string(p_node->scale), t+1);
            print_ast(p_node->src_ptr.get(), t);
            print_ast(p_node->index.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::TacCopyToOffset_t: {
            field("TacCopyToOffset", "", ++t);
            TacCopyToOffset* p_node = static_cast<TacCopyToOffset*>(node);
            field("TIdentifier", p_node->dst_name, t+1);
            field("TLong", std::to_string(p_node->offset), t+1);
            print_ast(p_node->src.get(), t);
            break;
        }
        case AST_T::TacJump_t: {
            field("TacJump", "", ++t);
            TacJump* p_node = static_cast<TacJump*>(node);
            field("TIdentifier", p_node->target, t+1);
            break;
        }
        case AST_T::TacJumpIfZero_t: {
            field("TacJumpIfZero", "", ++t);
            TacJumpIfZero* p_node = static_cast<TacJumpIfZero*>(node);
            field("TIdentifier", p_node->target, t+1);
            print_ast(p_node->condition.get(), t);
            break;
        }
        case AST_T::TacJumpIfNotZero_t: {
            field("TacJumpIfNotZero", "", ++t);
            TacJumpIfNotZero* p_node = static_cast<TacJumpIfNotZero*>(node);
            field("TIdentifier", p_node->target, t+1);
            print_ast(p_node->condition.get(), t);
            break;
        }
        case AST_T::TacLabel_t: {
            field("TacLabel", "", ++t);
            TacLabel* p_node = static_cast<TacLabel*>(node);
            field("TIdentifier", p_node->name, t+1);
            break;
        }
        case AST_T::TacTopLevel_t: {
            field("TacTopLevel", "", ++t);
            break;
        }
        case AST_T::TacFunction_t: {
            field("TacFunction", "", ++t);
            TacFunction* p_node = static_cast<TacFunction*>(node);
            field("TIdentifier", p_node->name, t+1);
            field("Bool", std::to_string(p_node->is_global), t+1);
            field("List[" + std::to_string(p_node->params.size()) + "]", "", t+1);
            for(const auto& item: p_node->params) {
                field("TIdentifier", item, t+2);
            }
            field("List[" + std::to_string(p_node->body.size()) + "]", "", t+1);
            for(const auto& item: p_node->body) {
                print_ast(item.get(), t+1);
            }
            break;
        }
        case AST_T::TacStaticVariable_t: {
            field("TacStaticVariable", "", ++t);
            TacStaticVariable* p_node = static_cast<TacStaticVariable*>(node);
            field("TIdentifier", p_node->name, t+1);
            field("Bool", std::to_string(p_node->is_global), t+1);
            print_ast(p_node->static_init_type.get(), t);
            field("List[" + std::to_string(p_node->static_inits.size()) + "]", "", t+1);
            for(const auto& item: p_node->static_inits) {
                print_ast(item.get(), t+1);
            }
            break;
        }
        case AST_T::TacStaticConstant_t: {
            field("TacStaticConstant", "", ++t);
            TacStaticConstant* p_node = static_cast<TacStaticConstant*>(node);
            field("TIdentifier", p_node->name, t+1);
            print_ast(p_node->static_init_type.get(), t);
            print_ast(p_node->static_init.get(), t);
            break;
        }
        case AST_T::TacProgram_t: {
            field("TacProgram", "", ++t);
            TacProgram* p_node = static_cast<TacProgram*>(node);
            field("List[" + std::to_string(p_node->static_constant_top_levels.size()) + "]", "", t+1);
            for(const auto& item: p_node->static_constant_top_levels) {
                print_ast(item.get(), t+1);
            }
            field("List[" + std::to_string(p_node->static_variable_top_levels.size()) + "]", "", t+1);
            for(const auto& item: p_node->static_variable_top_levels) {
                print_ast(item.get(), t+1);
            }
            field("List[" + std::to_string(p_node->function_top_levels.size()) + "]", "", t+1);
            for(const auto& item: p_node->function_top_levels) {
                print_ast(item.get(), t+1);
            }
            break;
        }
        case AST_T::AsmReg_t: {
            field("AsmReg", "", ++t);
            break;
        }
        case AST_T::AsmAx_t: {
            field("AsmAx", "", ++t);
            break;
        }
        case AST_T::AsmCx_t: {
            field("AsmCx", "", ++t);
            break;
        }
        case AST_T::AsmDx_t: {
            field("AsmDx", "", ++t);
            break;
        }
        case AST_T::AsmDi_t: {
            field("AsmDi", "", ++t);
            break;
        }
        case AST_T::AsmSi_t: {
            field("AsmSi", "", ++t);
            break;
        }
        case AST_T::AsmR8_t: {
            field("AsmR8", "", ++t);
            break;
        }
        case AST_T::AsmR9_t: {
            field("AsmR9", "", ++t);
            break;
        }
        case AST_T::AsmR10_t: {
            field("AsmR10", "", ++t);
            break;
        }
        case AST_T::AsmR11_t: {
            field("AsmR11", "", ++t);
            break;
        }
        case AST_T::AsmSp_t: {
            field("AsmSp", "", ++t);
            break;
        }
        case AST_T::AsmBp_t: {
            field("AsmBp", "", ++t);
            break;
        }
        case AST_T::AsmXMM0_t: {
            field("AsmXMM0", "", ++t);
            break;
        }
        case AST_T::AsmXMM1_t: {
            field("AsmXMM1", "", ++t);
            break;
        }
        case AST_T::AsmXMM2_t: {
            field("AsmXMM2", "", ++t);
            break;
        }
        case AST_T::AsmXMM3_t: {
            field("AsmXMM3", "", ++t);
            break;
        }
        case AST_T::AsmXMM4_t: {
            field("AsmXMM4", "", ++t);
            break;
        }
        case AST_T::AsmXMM5_t: {
            field("AsmXMM5", "", ++t);
            break;
        }
        case AST_T::AsmXMM6_t: {
            field("AsmXMM6", "", ++t);
            break;
        }
        case AST_T::AsmXMM7_t: {
            field("AsmXMM7", "", ++t);
            break;
        }
        case AST_T::AsmXMM14_t: {
            field("AsmXMM14", "", ++t);
            break;
        }
        case AST_T::AsmXMM15_t: {
            field("AsmXMM15", "", ++t);
            break;
        }
        case AST_T::AsmCondCode_t: {
            field("AsmCondCode", "", ++t);
            break;
        }
        case AST_T::AsmE_t: {
            field("AsmE", "", ++t);
            break;
        }
        case AST_T::AsmNE_t: {
            field("AsmNE", "", ++t);
            break;
        }
        case AST_T::AsmG_t: {
            field("AsmG", "", ++t);
            break;
        }
        case AST_T::AsmGE_t: {
            field("AsmGE", "", ++t);
            break;
        }
        case AST_T::AsmL_t: {
            field("AsmL", "", ++t);
            break;
        }
        case AST_T::AsmLE_t: {
            field("AsmLE", "", ++t);
            break;
        }
        case AST_T::AsmA_t: {
            field("AsmA", "", ++t);
            break;
        }
        case AST_T::AsmAE_t: {
            field("AsmAE", "", ++t);
            break;
        }
        case AST_T::AsmB_t: {
            field("AsmB", "", ++t);
            break;
        }
        case AST_T::AsmBE_t: {
            field("AsmBE", "", ++t);
            break;
        }
        case AST_T::AsmP_t: {
            field("AsmP", "", ++t);
            break;
        }
        case AST_T::AsmOperand_t: {
            field("AsmOperand", "", ++t);
            break;
        }
        case AST_T::AsmImm_t: {
            field("AsmImm", "", ++t);
            AsmImm* p_node = static_cast<AsmImm*>(node);
            field("Bool", std::to_string(p_node->is_byte), t+1);
            field("Bool", std::to_string(p_node->is_quad), t+1);
            field("TIdentifier", p_node->value, t+1);
            break;
        }
        case AST_T::AsmRegister_t: {
            field("AsmRegister", "", ++t);
            AsmRegister* p_node = static_cast<AsmRegister*>(node);
            print_ast(p_node->reg.get(), t);
            break;
        }
        case AST_T::AsmPseudo_t: {
            field("AsmPseudo", "", ++t);
            AsmPseudo* p_node = static_cast<AsmPseudo*>(node);
            field("TIdentifier", p_node->name, t+1);
            break;
        }
        case AST_T::AsmMemory_t: {
            field("AsmMemory", "", ++t);
            AsmMemory* p_node = static_cast<AsmMemory*>(node);
            field("TLong", std::to_string(p_node->value), t+1);
            print_ast(p_node->reg.get(), t);
            break;
        }
        case AST_T::AsmData_t: {
            field("AsmData", "", ++t);
            AsmData* p_node = static_cast<AsmData*>(node);
            field("TIdentifier", p_node->name, t+1);
            break;
        }
        case AST_T::AsmPseudoMem_t: {
            field("AsmPseudoMem", "", ++t);
            AsmPseudoMem* p_node = static_cast<AsmPseudoMem*>(node);
            field("TIdentifier", p_node->name, t+1);
            field("TLong", std::to_string(p_node->offset), t+1);
            break;
        }
        case AST_T::AsmIndexed_t: {
            field("AsmIndexed", "", ++t);
            AsmIndexed* p_node = static_cast<AsmIndexed*>(node);
            field("TLong", std::to_string(p_node->scale), t+1);
            print_ast(p_node->reg_base.get(), t);
            print_ast(p_node->reg_index.get(), t);
            break;
        }
        case AST_T::AsmBinaryOp_t: {
            field("AsmBinaryOp", "", ++t);
            break;
        }
        case AST_T::AsmAdd_t: {
            field("AsmAdd", "", ++t);
            break;
        }
        case AST_T::AsmSub_t: {
            field("AsmSub", "", ++t);
            break;
        }
        case AST_T::AsmMult_t: {
            field("AsmMult", "", ++t);
            break;
        }
        case AST_T::AsmDivDouble_t: {
            field("AsmDivDouble", "", ++t);
            break;
        }
        case AST_T::AsmBitAnd_t: {
            field("AsmBitAnd", "", ++t);
            break;
        }
        case AST_T::AsmBitOr_t: {
            field("AsmBitOr", "", ++t);
            break;
        }
        case AST_T::AsmBitXor_t: {
            field("AsmBitXor", "", ++t);
            break;
        }
        case AST_T::AsmBitShiftLeft_t: {
            field("AsmBitShiftLeft", "", ++t);
            break;
        }
        case AST_T::AsmBitShiftRight_t: {
            field("AsmBitShiftRight", "", ++t);
            break;
        }
        case AST_T::AsmUnaryOp_t: {
            field("AsmUnaryOp", "", ++t);
            break;
        }
        case AST_T::AsmNot_t: {
            field("AsmNot", "", ++t);
            break;
        }
        case AST_T::AsmNeg_t: {
            field("AsmNeg", "", ++t);
            break;
        }
        case AST_T::AsmShr_t: {
            field("AsmShr", "", ++t);
            break;
        }
        case AST_T::AsmInstruction_t: {
            field("AsmInstruction", "", ++t);
            break;
        }
        case AST_T::AsmMov_t: {
            field("AsmMov", "", ++t);
            AsmMov* p_node = static_cast<AsmMov*>(node);
            print_ast(p_node->assembly_type.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmMovSx_t: {
            field("AsmMovSx", "", ++t);
            AsmMovSx* p_node = static_cast<AsmMovSx*>(node);
            print_ast(p_node->assembly_type_src.get(), t);
            print_ast(p_node->assembly_type_dst.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmMovZeroExtend_t: {
            field("AsmMovZeroExtend", "", ++t);
            AsmMovZeroExtend* p_node = static_cast<AsmMovZeroExtend*>(node);
            print_ast(p_node->assembly_type_src.get(), t);
            print_ast(p_node->assembly_type_dst.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmLea_t: {
            field("AsmLea", "", ++t);
            AsmLea* p_node = static_cast<AsmLea*>(node);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmCvttsd2si_t: {
            field("AsmCvttsd2si", "", ++t);
            AsmCvttsd2si* p_node = static_cast<AsmCvttsd2si*>(node);
            print_ast(p_node->assembly_type.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmCvtsi2sd_t: {
            field("AsmCvtsi2sd", "", ++t);
            AsmCvtsi2sd* p_node = static_cast<AsmCvtsi2sd*>(node);
            print_ast(p_node->assembly_type.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmUnary_t: {
            field("AsmUnary", "", ++t);
            AsmUnary* p_node = static_cast<AsmUnary*>(node);
            print_ast(p_node->unary_op.get(), t);
            print_ast(p_node->assembly_type.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmBinary_t: {
            field("AsmBinary", "", ++t);
            AsmBinary* p_node = static_cast<AsmBinary*>(node);
            print_ast(p_node->binary_op.get(), t);
            print_ast(p_node->assembly_type.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmCmp_t: {
            field("AsmCmp", "", ++t);
            AsmCmp* p_node = static_cast<AsmCmp*>(node);
            print_ast(p_node->assembly_type.get(), t);
            print_ast(p_node->src.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmIdiv_t: {
            field("AsmIdiv", "", ++t);
            AsmIdiv* p_node = static_cast<AsmIdiv*>(node);
            print_ast(p_node->assembly_type.get(), t);
            print_ast(p_node->src.get(), t);
            break;
        }
        case AST_T::AsmDiv_t: {
            field("AsmDiv", "", ++t);
            AsmDiv* p_node = static_cast<AsmDiv*>(node);
            print_ast(p_node->assembly_type.get(), t);
            print_ast(p_node->src.get(), t);
            break;
        }
        case AST_T::AsmCdq_t: {
            field("AsmCdq", "", ++t);
            AsmCdq* p_node = static_cast<AsmCdq*>(node);
            print_ast(p_node->assembly_type.get(), t);
            break;
        }
        case AST_T::AsmJmp_t: {
            field("AsmJmp", "", ++t);
            AsmJmp* p_node = static_cast<AsmJmp*>(node);
            field("TIdentifier", p_node->target, t+1);
            break;
        }
        case AST_T::AsmJmpCC_t: {
            field("AsmJmpCC", "", ++t);
            AsmJmpCC* p_node = static_cast<AsmJmpCC*>(node);
            field("TIdentifier", p_node->target, t+1);
            print_ast(p_node->cond_code.get(), t);
            break;
        }
        case AST_T::AsmSetCC_t: {
            field("AsmSetCC", "", ++t);
            AsmSetCC* p_node = static_cast<AsmSetCC*>(node);
            print_ast(p_node->cond_code.get(), t);
            print_ast(p_node->dst.get(), t);
            break;
        }
        case AST_T::AsmLabel_t: {
            field("AsmLabel", "", ++t);
            AsmLabel* p_node = static_cast<AsmLabel*>(node);
            field("TIdentifier", p_node->name, t+1);
            break;
        }
        case AST_T::AsmPush_t: {
            field("AsmPush", "", ++t);
            AsmPush* p_node = static_cast<AsmPush*>(node);
            print_ast(p_node->src.get(), t);
            break;
        }
        case AST_T::AsmCall_t: {
            field("AsmCall", "", ++t);
            AsmCall* p_node = static_cast<AsmCall*>(node);
            field("TIdentifier", p_node->name, t+1);
            break;
        }
        case AST_T::AsmRet_t: {
            field("AsmRet", "", ++t);
            break;
        }
        case AST_T::AsmTopLevel_t: {
            field("AsmTopLevel", "", ++t);
            break;
        }
        case AST_T::AsmFunction_t: {
            field("AsmFunction", "", ++t);
            AsmFunction* p_node = static_cast<AsmFunction*>(node);
            field("TIdentifier", p_node->name, t+1);
            field("Bool", std::to_string(p_node->is_global), t+1);
            field("List[" + std::to_string(p_node->instructions.size()) + "]", "", t+1);
            for(const auto& item: p_node->instructions) {
                print_ast(item.get(), t+1);
            }
            break;
        }
        case AST_T::AsmStaticVariable_t: {
            field("AsmStaticVariable", "", ++t);
            AsmStaticVariable* p_node = static_cast<AsmStaticVariable*>(node);
            field("TIdentifier", p_node->name, t+1);
            field("TInt", std::to_string(p_node->alignment), t+1);
            field("Bool", std::to_string(p_node->is_global), t+1);
            field("List[" + std::to_string(p_node->static_inits.size()) + "]", "", t+1);
            for(const auto& item: p_node->static_inits) {
                print_ast(item.get(), t+1);
            }
            break;
        }
        case AST_T::AsmStaticConstant_t: {
            field("AsmStaticConstant", "", ++t);
            AsmStaticConstant* p_node = static_cast<AsmStaticConstant*>(node);
            field("TIdentifier", p_node->name, t+1);
            field("TInt", std::to_string(p_node->alignment), t+1);
            print_ast(p_node->static_init.get(), t);
            break;
        }
        case AST_T::AsmProgram_t: {
            field("AsmProgram", "", ++t);
            AsmProgram* p_node = static_cast<AsmProgram*>(node);
            field("List[" + std::to_string(p_node->static_constant_top_levels.size()) + "]", "", t+1);
            for(const auto& item: p_node->static_constant_top_levels) {
                print_ast(item.get(), t+1);
            }
            field("List[" + std::to_string(p_node->top_levels.size()) + "]", "", t+1);
            for(const auto& item: p_node->top_levels) {
                print_ast(item.get(), t+1);
            }
            break;
        }
        default:
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
    std::cout << "\nDict(" << std::to_string(symbol_table->size()) << "):";
    for(const auto& symbol: *symbol_table) {
        field("[" + symbol.first + "]", "", 2);
        print_ast(symbol.second.get(), 2);
    }
    std::cout << std::endl;
}

void pretty_print_static_constant_table() {
    header_string("Static Constant Table");
    std::cout << "\nDict(" << std::to_string(static_constant_table->size()) << "):";
    for(const auto& static_constant: *static_constant_table) {
        field("[" + static_constant.first + "]", "", 2);
        if(symbol_table->find(static_constant.second) != symbol_table->end() &&
           (*symbol_table)[static_constant.second]->attrs->type() == AST_T::ConstantAttr_t) {
            ConstantAttr* const_attr = static_cast<ConstantAttr*>((*symbol_table)[static_constant.second]->attrs.get());
            if(const_attr->static_init->type() == AST_T::StringInit_t) {
                std::cout << "\n    string: \"";
                for(const TChar& byte : static_cast<StringInit*>(const_attr->static_init.get())->literal.get()->value) {
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
        else if(backend_symbol_table->find(static_constant.second) != backend_symbol_table->end() &&
                (*backend_symbol_table)[static_constant.second]->type() == AST_T::BackendObj_t){
            BackendObj* backend_obj = static_cast<BackendObj*>((*backend_symbol_table)[static_constant.second].get());
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
    std::cout << "\nDict(" << std::to_string(struct_typedef_table->size()) << "):";
    for(const auto& struct_typedef: *struct_typedef_table) {
        field("[" + struct_typedef.first + "]", "", 2);
        print_ast(struct_typedef.second.get(), 2);
    }
    std::cout << std::endl;
}

void pretty_print_backend_symbol_table() {
    header_string("Backend Symbol Table");
    std::cout << "\nDict(" << std::to_string(backend_symbol_table->size()) << "):";
    for(const auto& symbol: *backend_symbol_table) {
        field("[" + symbol.first + "]", "", 2);
        print_ast(symbol.second.get(), 2);
    }
    std::cout << std::endl;
}

void pretty_print_asm_code() {
    header_string("ASM Code");
    std::cout << std::endl;
}
#endif
