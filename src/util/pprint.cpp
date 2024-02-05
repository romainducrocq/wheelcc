#include "util/pprint.hpp"

#ifndef __NDEBUG__

#include "util/error.hpp"
#include "parser/lexer.hpp"
#include "ast/ast.hpp"
#include "ast/symbol_table.hpp"
#include "ast/backend_st.hpp"
#include "ast/c_ast.hpp"
#include "ast/tac_ast.hpp"

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
        case AST_T::Type_t: {
            field("Type", "", ++t);
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
        case AST_T::Double_t: {
            field("Double", "", ++t);
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
        case AST_T::DoubleInit_t: {
            field("DoubleInit", "", ++t);
            DoubleInit* p_node = static_cast<DoubleInit*>(node);
            field("TDouble", std::to_string(p_node->value), t+1);
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
            print_ast(p_node->static_init.get(), t);
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
        case AST_T::AssemblyType_t: {
            field("AssemblyType", "", ++t);
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
        case AST_T::CConstDouble_t: {
            field("CConstDouble", "", ++t);
            CConstDouble* p_node = static_cast<CConstDouble*>(node);
            field("TDouble", std::to_string(p_node->value), t+1);
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
            print_ast(p_node->initial_value.get(), t);
            break;
        }
        case AST_T::TacProgram_t: {
            field("TacProgram", "", ++t);
            TacProgram* p_node = static_cast<TacProgram*>(node);
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
        default:
            raise_internal_error("Pretty print not implemented for ast node type " +
                                 em(std::to_string(node->type())));
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
        print_ast(symbol.second->type_t.get(), 2);
        print_ast(symbol.second->attrs.get(), 2);
    }
    std::cout << std::endl;
}

#endif

