#include "util/c_std.h"
#include "util/throw.h"

#include "ast/ast.h"
#include "ast/front_ast.h"
#include "ast/front_symt.h"

#include "frontend/parser/errors.h"
#include "frontend/parser/lexer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Errors

const char* get_tok_kind_fmt(TOKEN_KIND tok_kind) {
    switch (tok_kind) {
        case TOK_assign_shiftleft:
            return "<<=";
        case TOK_assign_shiftright:
            return ">>=";
        case TOK_unop_incr:
            return "++";
        case TOK_unop_decr:
            return "--";
        case TOK_binop_shiftleft:
            return "<<";
        case TOK_binop_shiftright:
            return ">>";
        case TOK_binop_and:
            return "&&";
        case TOK_binop_or:
            return "||";
        case TOK_binop_eq:
            return "==";
        case TOK_binop_ne:
            return "!=";
        case TOK_binop_le:
            return "<=";
        case TOK_binop_ge:
            return ">=";
        case TOK_assign_add:
            return "+=";
        case TOK_assign_subtract:
            return "-=";
        case TOK_assign_multiply:
            return "*=";
        case TOK_assign_divide:
            return "/=";
        case TOK_assign_remainder:
            return "%=";
        case TOK_assign_bitand:
            return "&=";
        case TOK_assign_bitor:
            return "|=";
        case TOK_assign_xor:
            return "^=";
        case TOK_structop_ptr:
            return "->";
        case TOK_open_paren:
            return "(";
        case TOK_close_paren:
            return ")";
        case TOK_open_brace:
            return "{";
        case TOK_close_brace:
            return "}";
        case TOK_open_bracket:
            return "[";
        case TOK_close_bracket:
            return "]";
        case TOK_semicolon:
            return ";";
        case TOK_unop_complement:
            return "~";
        case TOK_unop_neg:
            return "-";
        case TOK_unop_not:
            return "!";
        case TOK_binop_add:
            return "+";
        case TOK_binop_multiply:
            return "*";
        case TOK_binop_divide:
            return "/";
        case TOK_binop_remainder:
            return "%";
        case TOK_binop_bitand:
            return "&";
        case TOK_binop_bitor:
            return "|";
        case TOK_binop_xor:
            return "^";
        case TOK_binop_lt:
            return "<";
        case TOK_binop_gt:
            return ">";
        case TOK_assign:
            return "=";
        case TOK_ternary_if:
            return "?";
        case TOK_ternary_else:
            return ":";
        case TOK_comma_separator:
            return ",";
        case TOK_structop_member:
            return ".";
        case TOK_key_char:
            return "char";
        case TOK_key_int:
            return "int";
        case TOK_key_long:
            return "long";
        case TOK_key_double:
            return "double";
        case TOK_key_signed:
            return "signed";
        case TOK_key_unsigned:
            return "unsigned";
        case TOK_key_void:
            return "void";
        case TOK_key_struct:
            return "struct";
        case TOK_key_union:
            return "union";
        case TOK_key_sizeof:
            return "sizeof";
        case TOK_key_return:
            return "return";
        case TOK_key_if:
            return "if";
        case TOK_key_else:
            return "else";
        case TOK_key_goto:
            return "goto";
        case TOK_key_do:
            return "do";
        case TOK_key_while:
            return "while";
        case TOK_key_for:
            return "for";
        case TOK_key_switch:
            return "switch";
        case TOK_key_case:
            return "case";
        case TOK_key_default:
            return "default";
        case TOK_key_break:
            return "break";
        case TOK_key_continue:
            return "continue";
        case TOK_key_static:
            return "static";
        case TOK_key_extern:
            return "extern";
        case TOK_identifier:
            return "identifier";
        case TOK_string_literal:
            return "string literal";
        case TOK_char_const:
            return "const char";
        case TOK_int_const:
            return "const int";
        case TOK_long_const:
            return "const long";
        case TOK_uint_const:
            return "const unsigned int";
        case TOK_ulong_const:
            return "const unsigned long";
        case TOK_dbl_const:
            return "const double";
        default:
            THROW_ABORT;
    }
}

const char* get_tok_fmt(IdentifierContext* ctx, Token* token) {
    switch (token->tok_kind) {
        case TOK_identifier:
        case TOK_string_literal:
        case TOK_char_const:
        case TOK_int_const:
        case TOK_long_const:
        case TOK_uint_const:
        case TOK_ulong_const:
        case TOK_dbl_const:
            return map_get(ctx->hash_table, token->tok);
        default:
            return get_tok_kind_fmt(token->tok_kind);
    }
}

const char* get_const_fmt(CConst* node) {
    switch (node->type) {
        case AST_CConstInt_t:
            return "int";
        case AST_CConstLong_t:
            return "long";
        case AST_CConstUInt_t:
            return "unsigned int";
        case AST_CConstULong_t:
            return "unsigned long";
        case AST_CConstDouble_t:
            return "double";
        case AST_CConstChar_t:
            return "char";
        case AST_CConstUChar_t:
            return "unsigned char";
        default:
            THROW_ABORT;
    }
}

const char* get_storage_class_fmt(CStorageClass* node) {
    switch (node->type) {
        case AST_CStatic_t:
            return "static";
        case AST_CExtern_t:
            return "extern";
        default:
            THROW_ABORT;
    }
}

const char* get_unop_fmt(CUnaryOp* node) {
    switch (node->type) {
        case AST_CComplement_t:
            return "~";
        case AST_CNegate_t:
            return "-";
        case AST_CNot_t:
            return "!";
        default:
            THROW_ABORT;
    }
}

const char* get_binop_fmt(CBinaryOp* node) {
    switch (node->type) {
        case AST_CAdd_t:
            return "+";
        case AST_CSubtract_t:
            return "-";
        case AST_CMultiply_t:
            return "*";
        case AST_CDivide_t:
            return "/";
        case AST_CRemainder_t:
            return "%";
        case AST_CBitAnd_t:
            return "&";
        case AST_CBitOr_t:
            return "|";
        case AST_CBitXor_t:
            return "^";
        case AST_CBitShiftLeft_t:
            return "<<";
        case AST_CBitShiftRight_t:
            return ">>";
        case AST_CBitShrArithmetic_t:
            return ">>";
        case AST_CAnd_t:
            return "&&";
        case AST_COr_t:
            return "||";
        case AST_CEqual_t:
            return "==";
        case AST_CNotEqual_t:
            return "!=";
        case AST_CLessThan_t:
            return "<";
        case AST_CLessOrEqual_t:
            return "<=";
        case AST_CGreaterThan_t:
            return ">";
        case AST_CGreaterOrEqual_t:
            return ">=";
        default:
            THROW_ABORT;
    }
}

const char* get_assign_fmt(CBinaryOp* node, CUnaryOp* unop) {
    if (!node) {
        return "=";
    }
    else if (unop->type != AST_CUnaryOp_t) { // TODO
        switch (unop->type) {
            case AST_CPrefix_t: {
                switch (node->type) {
                    case AST_CAdd_t:
                        return "prefix ++";
                    case AST_CSubtract_t:
                        return "prefix --";
                    default:
                        THROW_ABORT;
                }
            }
            case AST_CPostfix_t: {
                switch (node->type) {
                    case AST_CAdd_t:
                        return "postfix ++";
                    case AST_CSubtract_t:
                        return "postfix --";
                    default:
                        THROW_ABORT;
                }
            }
            default:
                THROW_ABORT;
        }
    }
    else {
        switch (node->type) {
            case AST_CAdd_t:
                return "+=";
            case AST_CSubtract_t:
                return "-=";
            case AST_CMultiply_t:
                return "*=";
            case AST_CDivide_t:
                return "/=";
            case AST_CRemainder_t:
                return "%=";
            case AST_CBitAnd_t:
                return "&=";
            case AST_CBitOr_t:
                return "|=";
            case AST_CBitXor_t:
                return "^=";
            case AST_CBitShiftLeft_t:
                return "<<=";
            case AST_CBitShiftRight_t:
                return ">>=";
            case AST_CBitShrArithmetic_t:
                return ">>=";
            default:
                THROW_ABORT;
        }
    }
}

const char* get_name_fmt(IdentifierContext* ctx, TIdentifier name, string_t* name_fmt) {
    string_t value = map_get(ctx->hash_table, name);
    str_copy(value, *name_fmt);
    for (size_t i = str_size(*name_fmt); i-- > 0;) {
        if ((*name_fmt)[i] == UID_SEPARATOR[0]) {
            str_substr(*name_fmt, 0, i - 1);
            break;
        }
    }
    return *name_fmt;
}

const char* get_struct_name_fmt(IdentifierContext* ctx, TIdentifier name, bool is_union, string_t* struct_fmt) {
    *struct_fmt = is_union ? str_new("union ") : str_new("struct ");
    {
        string_t name_fmt = str_new(NULL);
        str_append(*struct_fmt, get_name_fmt(ctx, name, &name_fmt));
        str_delete(name_fmt);
    }
    return *struct_fmt;
}

const char* get_fun_fmt(IdentifierContext* ctx, FunType* fun_type, string_t* fun_fmt) {
    *fun_fmt = str_new("(");
    {
        string_t type_fmt = str_new(NULL);
        str_append(*fun_fmt, get_type_fmt(ctx, fun_type->ret_type, &type_fmt));
        str_delete(type_fmt);
    }
    str_append(*fun_fmt, ")(");
    for (size_t i = 0; i < vec_size(fun_type->param_types); ++i) {
        {
            string_t type_fmt = str_new(NULL);
            str_append(*fun_fmt, get_type_fmt(ctx, fun_type->param_types[i], &type_fmt));
            str_delete(type_fmt);
        }
        str_append(*fun_fmt, ", ");
    }
    if (!vec_empty(fun_type->param_types)) {
        str_pop_back(*fun_fmt);
        str_pop_back(*fun_fmt);
    }
    str_append(*fun_fmt, ")");
    return *fun_fmt;
}

const char* get_ptr_fmt(IdentifierContext* ctx, Pointer* ptr_type, string_t* ptr_fmt) {
    *ptr_fmt = str_new("");
    string_t decltor_fmt = str_new("*");
    while (ptr_type->ref_type->type == AST_Pointer_t) {
        ptr_type = &ptr_type->ref_type->get._Pointer;
        str_append(decltor_fmt, "*");
    }
    {
        string_t type_fmt = str_new(NULL);
        str_append(*ptr_fmt, get_type_fmt(ctx, ptr_type->ref_type, &type_fmt));
        str_delete(type_fmt);
    }
    str_append(*ptr_fmt, decltor_fmt);
    str_delete(decltor_fmt);
    return *ptr_fmt;
}

const char* get_arr_fmt(IdentifierContext* ctx, Array* arr_type, string_t* arr_fmt) {
    *arr_fmt = str_new("");
    string_t decltor_fmt = str_new("[");
    {
        string_t strto_size = str_to_string(arr_type->size);
        str_append(decltor_fmt, strto_size);
        str_delete(strto_size);
    }
    str_append(decltor_fmt, "]");
    while (arr_type->elem_type->type == AST_Array_t) {
        arr_type = &arr_type->elem_type->get._Array;
        str_append(decltor_fmt, "[");
        {
            string_t strto_size = str_to_string(arr_type->size);
            str_append(decltor_fmt, strto_size);
            str_delete(strto_size);
        }
        str_append(decltor_fmt, "]");
    }
    {
        string_t type_fmt = str_new(NULL);
        str_append(*arr_fmt, get_type_fmt(ctx, arr_type->elem_type, &type_fmt));
        str_delete(type_fmt);
    }
    str_append(*arr_fmt, decltor_fmt);
    str_delete(decltor_fmt);
    return *arr_fmt;
}

const char* get_struct_fmt(IdentifierContext* ctx, Structure* struct_type, string_t* struct_fmt) {
    return get_struct_name_fmt(ctx, struct_type->tag, struct_type->is_union, struct_fmt);
}

const char* get_type_fmt(IdentifierContext* ctx, Type* type, string_t* type_fmt) {
    switch (type->type) {
        case AST_Char_t:
            return "char";
        case AST_SChar_t:
            return "signed char";
        case AST_UChar_t:
            return "unsigned char";
        case AST_Int_t:
            return "int";
        case AST_Long_t:
            return "long";
        case AST_UInt_t:
            return "unsigned int";
        case AST_ULong_t:
            return "unsigned long";
        case AST_Double_t:
            return "double";
        case AST_Void_t:
            return "void";
        case AST_FunType_t:
            return get_fun_fmt(ctx, &type->get._FunType, type_fmt);
        case AST_Pointer_t:
            return get_ptr_fmt(ctx, &type->get._Pointer, type_fmt);
        case AST_Array_t:
            return get_arr_fmt(ctx, &type->get._Array, type_fmt);
        case AST_Structure_t:
            return get_struct_fmt(ctx, &type->get._Structure, type_fmt);
        default:
            THROW_ABORT;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define EM_CSTR(X) "\033[1m‘" X "’\033[0m"
#define EM_VARG "\033[1m‘%s’\033[0m"
#define RET_ERRNO return "(no. %i) "

const char* get_fatal_msg(MESSAGE_FATAL msg) {
    switch (msg) {
        case MSG_unsupported_os:
            RET_ERRNO EM_VARG " operating system is not supported, requires " EM_CSTR("GNU/Linux") " (x86_64) or " //
                EM_CSTR("MacOS");
        case MSG_unsupported_arch:
#ifdef __APPLE__
            RET_ERRNO EM_VARG " architecture is not supported, requires " EM_CSTR("x86_64") " or " EM_CSTR("arm");
#else
            RET_ERRNO EM_VARG " architecture is not supported, requires " EM_CSTR("x86_64");
#endif
        case MSG_unsupported_compiler:
#ifdef __APPLE__
            RET_ERRNO EM_VARG " compiler is not supported, requires " EM_CSTR("clang") " >= 5.0.0";
#else
            RET_ERRNO EM_VARG " compiler is not supported, requires " EM_CSTR("gcc") " >= 8.1.0";
#endif
        case MSG_unsupported_cc_ver:
#ifdef __APPLE__
            RET_ERRNO EM_CSTR("clang") " %i.%i.%i is not supported, requires " EM_CSTR("clang") " >= 5.0.0";
#else
            RET_ERRNO EM_CSTR("gcc") " %i.%i.%i is not supported, requires " EM_CSTR("gcc") " >= 8.1.0";
#endif
        default:
            THROW_ABORT;
    }
}

const char* get_arg_msg(MESSAGE_ARG msg) {
    switch (msg) {
        case MSG_print_help:
            RET_ERRNO "Usage: %s [--help] Debug OptimL1 OptimL2 FILE SourceDir [IncludeDir...]\n"
                      "    [--help]:         print help and exit\n"
                      "    Debug:            print debug info (0..1"
#ifndef __NDEBUG__
                      "|251..255"
#endif
                      ")\n"
                      "    OptimL1:          optimization level 1 mask (0..15)\n"
                      "    OptimL2:          optimization level 2 enum (0..2)\n"
                      "    FILE:             source file to compile\n"
                      "    SourceDir:        source file include path\n"
                      "    [IncludeDir...]:  add a list of paths to include path\n"
                      "see " EM_CSTR("driver.sh");
        case MSG_no_debug_arg:
            RET_ERRNO "no debug code passed in first argument, see " EM_CSTR("--help");
        case MSG_invalid_debug_arg:
            RET_ERRNO "invalid debug code " EM_VARG " passed in first argument, see " EM_CSTR("--help");
        case MSG_no_optim_1_arg:
            RET_ERRNO "no level 1 optimization mask passed in second argument, see " EM_CSTR("--help");
        case MSG_invalid_optim_1_arg:
            RET_ERRNO "invalid level 1 optimization mask " EM_VARG " passed in second argument, see " EM_CSTR("--help");
        case MSG_no_optim_2_arg:
            RET_ERRNO "no level 2 optimization code passed in third argument, see " EM_CSTR("--help");
        case MSG_invalid_optim_2_arg:
            RET_ERRNO "invalid level 2 optimization code " EM_VARG " passed in third argument, see " EM_CSTR("--help");
        case MSG_no_input_files_arg:
            RET_ERRNO "no input file passed in fourth argument, see " EM_CSTR("--help");
        case MSG_no_include_dir_arg:
            RET_ERRNO "no include directories passed in fifth argument, see " EM_CSTR("--help");
        default:
            THROW_ABORT;
    }
}

const char* get_util_msg(MESSAGE_UTIL msg) {
    switch (msg) {
        case MSG_failed_fread:
            RET_ERRNO "cannot read input file " EM_VARG;
        case MSG_failed_fwrite:
            RET_ERRNO "cannot write output file " EM_VARG;
        case MSG_failed_strtoi:
            RET_ERRNO "cannot interpret string " EM_VARG " to an integer value";
        case MSG_failed_strtou:
            RET_ERRNO "cannot interpret string " EM_VARG " to an unsigned integer value";
        case MSG_failed_strtod:
            RET_ERRNO "cannot interpret string " EM_VARG " to a floating point value";
        default:
            THROW_ABORT;
    }
}

const char* get_lexer_msg(MESSAGE_LEXER msg) {
    switch (msg) {
        case MSG_invalid_tok:
            RET_ERRNO "found invalid token " EM_VARG;
        case MSG_failed_include:
            RET_ERRNO "cannot find " EM_VARG " header file in " EM_CSTR("include") " directive search";
        default:
            THROW_ABORT;
    }
}

const char* get_parser_msg(MESSAGE_PARSER msg) {
    switch (msg) {
        case MSG_unexpected_next_tok:
            RET_ERRNO "found token " EM_VARG ", but expected " EM_VARG " next";
        case MSG_reached_eof:
            RET_ERRNO "reached end of file, but expected declaration or statement next";
        case MSG_overflow_long_const:
            RET_ERRNO "cannot represent " EM_VARG " as a 64 bits signed integer constant, very large number";
        case MSG_overflow_ulong_const:
            RET_ERRNO "cannot represent " EM_VARG " as a 64 bits unsigned integer constant, very large number";
        case MSG_arr_size_not_int_const:
            RET_ERRNO "illegal array size " EM_VARG ", requires a constant integer";
        case MSG_case_value_not_int_const:
            RET_ERRNO "illegal " EM_CSTR("case") " value " EM_VARG ", requires a constant integer";
        case MSG_expect_unop:
            RET_ERRNO "found token " EM_VARG ", but expected " EM_CSTR("~") ", " EM_CSTR("-") " or " EM_CSTR("!") //
                " next";
        case MSG_expect_binop:
            RET_ERRNO "found token " EM_VARG ", but expected " EM_CSTR("+") ", " EM_CSTR("+=") ", " EM_CSTR("++")  //
                ", " EM_CSTR("-") ", " EM_CSTR("-=") ", " EM_CSTR("--") ", " EM_CSTR("*") ", " EM_CSTR("*=") ", "  //
                EM_CSTR("/") ", " EM_CSTR("/=") ", " EM_CSTR("%") ", " EM_CSTR("%=") ", " EM_CSTR("&") ", "        //
                EM_CSTR("&=") ", " EM_CSTR("|") ", " EM_CSTR("|=") ", " EM_CSTR("^") ", " EM_CSTR("^=") ", "       //
                EM_CSTR("<<") ", " EM_CSTR("<<=") ", " EM_CSTR(">>") ", " EM_CSTR(">>=") ", " EM_CSTR("&&")        //
                ", " EM_CSTR("||") ", " EM_CSTR("==") ", " EM_CSTR("!=") ", " EM_CSTR("<") ", " EM_CSTR("<=") ", " //
                EM_CSTR(">") " or " EM_CSTR(">=") " next";
        case MSG_expect_abstract_decltor:
            RET_ERRNO "found token " EM_VARG ", but expected " EM_CSTR("*") ", " EM_CSTR("(") " or " EM_CSTR("[") //
                " next";
        case MSG_expect_ptr_unary_factor:
            RET_ERRNO "found token " EM_VARG ", but expected " EM_CSTR("*") " or " EM_CSTR("&") " next";
        case MSG_expect_primary_exp_factor:
            RET_ERRNO "found token " EM_VARG ", but expected " EM_CSTR("const int") ", " EM_CSTR("const long") ", " //
                EM_CSTR("const char") ", " EM_CSTR("const double") ", " EM_CSTR("const unsigned int") ", "          //
                EM_CSTR("const unsigned long") ", " EM_CSTR("identifier") ", " EM_CSTR("identifier(") ", "          //
                EM_CSTR("string literal") " or " EM_CSTR("(") " next";
        case MSG_expect_exp:
            RET_ERRNO "found token " EM_VARG ", but expected " EM_CSTR("+") ", " EM_CSTR("-") ", " EM_CSTR("*") ", " //
                EM_CSTR("/") ", " EM_CSTR("%") ", " EM_CSTR("&") ", " EM_CSTR("|") ", " EM_CSTR("^") ", "            //
                EM_CSTR("<<") ", " EM_CSTR(">>") ", " EM_CSTR("<") ", " EM_CSTR("<=") ", " EM_CSTR(">") ", "         //
                EM_CSTR(">=") ", " EM_CSTR("==") ", " EM_CSTR("!=") ", " EM_CSTR("&&") ", " EM_CSTR("||") ", "       //
                EM_CSTR("=") ", " EM_CSTR("+=") ", " EM_CSTR("-=") ", " EM_CSTR("*=") ", " EM_CSTR("/=") ", "        //
                EM_CSTR("%=") ", " EM_CSTR("&=") ", " EM_CSTR("|=") ", " EM_CSTR("^=") ", " EM_CSTR("<<=") ", "      //
                EM_CSTR(">>=") " or " EM_CSTR("?") " next";
        case MSG_for_init_decl_as_fun:
            RET_ERRNO "function " EM_VARG " declared in " EM_CSTR("for") " loop initial declaration";
        case MSG_expect_specifier:
            RET_ERRNO "found token " EM_VARG ", but expected " EM_CSTR("identifier") ", " EM_CSTR(")") ", " //
                EM_CSTR("char") ", " EM_CSTR("int") ", " EM_CSTR("long") ", " EM_CSTR("double") ", "        //
                EM_CSTR("unsigned") ", " EM_CSTR("signed") ", " EM_CSTR("void") ", " EM_CSTR("struct") ", " //
                EM_CSTR("union") ", " EM_CSTR("static") ", " EM_CSTR("extern") ", " EM_CSTR("*") ", "       //
                EM_CSTR("(") " or " EM_CSTR("[") " next";
        case MSG_expect_specifier_list:
            RET_ERRNO "found tokens " EM_VARG ", but expected valid list of unique type specifiers next";
        case MSG_expect_storage_class:
            RET_ERRNO "found token " EM_VARG ", but expected " EM_CSTR("static") " or " EM_CSTR("extern") " next";
        case MSG_empty_compound_init:
            RET_ERRNO "empty compound initializer requires at least one initializer";
        case MSG_derived_fun_decl:
            RET_ERRNO "cannot apply further type derivation to function declaration";
        case MSG_expect_simple_decltor:
            RET_ERRNO "found token " EM_VARG ", but expected " EM_CSTR("identifier") " or " EM_CSTR("(") " next";
        case MSG_expect_param_list:
            RET_ERRNO "found token " EM_VARG ", but expected " EM_CSTR("void") ", " EM_CSTR("char") ", " //
                EM_CSTR("int") ", " EM_CSTR("long") ", " EM_CSTR("double") ", " EM_CSTR("unsigned") ", " //
                EM_CSTR("signed") ", " EM_CSTR("struct") " or " EM_CSTR("union") " next";
        case MSG_member_decl_not_auto:
            RET_ERRNO "data structure type declared with member " EM_VARG " with " EM_VARG " storage class";
        case MSG_member_decl_as_fun:
            RET_ERRNO "data structure type declared with member " EM_VARG " as a function";
        default:
            THROW_ABORT;
    }
}

const char* get_semantic_msg(MESSAGE_SEMANTIC msg) {
    switch (msg) {
        case MSG_incomplete_arr:
            RET_ERRNO "array type " EM_VARG " of incomplete type " EM_VARG ", requires a complete type";
        case MSG_joint_ptr_mismatch:
            RET_ERRNO "pointer type mismatch " EM_VARG " and " EM_VARG " in operator";
        case MSG_fun_used_as_var:
            RET_ERRNO "function " EM_VARG " used as a variable";
        case MSG_illegal_cast:
            RET_ERRNO "illegal cast, cannot convert expression from type " EM_VARG " to " EM_VARG;
        case MSG_invalid_unary_op:
            RET_ERRNO "cannot apply unary operator " EM_VARG " on operand type " EM_VARG;
        case MSG_invalid_binary_op:
            RET_ERRNO "cannot apply binary operator " EM_VARG " on operand type " EM_VARG;
        case MSG_invalid_binary_ops:
            RET_ERRNO "cannot apply binary operator " EM_VARG " on operand types " EM_VARG " and " EM_VARG;
        case MSG_assign_to_void:
            RET_ERRNO "cannot assign " EM_CSTR("=") " to left operand type " EM_CSTR("void");
        case MSG_assign_to_rvalue:
            RET_ERRNO "assignment " EM_VARG " requires lvalue left operand, but got rvalue";
        case MSG_invalid_condition:
            RET_ERRNO "cannot apply conditional " EM_CSTR("?") " on condition operand type " EM_VARG;
        case MSG_invalid_ternary_op:
            RET_ERRNO "cannot apply ternary operator " EM_CSTR(":") " on operand types " EM_VARG " and " EM_VARG;
        case MSG_var_used_as_fun:
            RET_ERRNO "variable " EM_VARG " used as a function";
        case MSG_call_with_wrong_argc:
            RET_ERRNO "function " EM_VARG " called with " EM_VARG " arguments instead of " EM_VARG;
        case MSG_deref_not_ptr:
            RET_ERRNO "cannot apply dereference operator " EM_CSTR("*") " on non-pointer type " EM_VARG;
        case MSG_addrof_rvalue:
            RET_ERRNO "addresssing " EM_CSTR("&") " requires lvalue operand, but got rvalue";
        case MSG_invalid_subscript:
            RET_ERRNO "cannot subscript array with operand types " EM_VARG " and " EM_VARG
                      ", requires a complete pointer and an integer types";
        case MSG_sizeof_incomplete:
            RET_ERRNO "cannot get size with " EM_CSTR("sizeof") " operator on incomplete type " EM_VARG;
        case MSG_dot_not_struct:
            RET_ERRNO "cannot access data structure member " EM_VARG
                      " with dot operator " EM_CSTR(".") " on non-data structure type " EM_VARG;
        case MSG_member_not_in_struct:
            RET_ERRNO "data structure type " EM_VARG " has no member named " EM_VARG;
        case MSG_arrow_not_struct_ptr:
            RET_ERRNO "cannot access data structure member " EM_VARG
                      " with arrow operator " EM_CSTR("->") " on non-pointer-to-data structure type " EM_VARG;
        case MSG_arrow_incomplete:
            RET_ERRNO "cannot access data structure member " EM_VARG
                      " with arrow operator " EM_CSTR("->") " on incomplete data structure type " EM_VARG;
        case MSG_exp_incomplete:
            RET_ERRNO "incomplete data structure type " EM_VARG " in expression";
        case MSG_ret_value_in_void_fun:
            RET_ERRNO "found " EM_CSTR("return") " value in function " EM_VARG " returning type " EM_CSTR("void");
        case MSG_no_ret_value_in_fun:
            RET_ERRNO "found " EM_CSTR("return") " with no value in function " EM_VARG " returning type " EM_VARG;
        case MSG_invalid_if:
            RET_ERRNO "cannot use " EM_CSTR("if") " statement with condition expression type " EM_VARG;
        case MSG_invalid_while:
            RET_ERRNO "cannot use " EM_CSTR("while") " loop statement with condition expression type " EM_VARG;
        case MSG_invalid_do_while:
            RET_ERRNO "cannot use " EM_CSTR("do while") " loop statement with condition expression type " EM_VARG;
        case MSG_invalid_for:
            RET_ERRNO "cannot use " EM_CSTR("for") " loop statement with condition expression type " EM_VARG;
        case MSG_invalid_switch:
            RET_ERRNO "cannot use " EM_CSTR("switch") " statement with match expression type " EM_VARG
                                                      ", requires an integer type";
        case MSG_duplicate_case_value:
            RET_ERRNO "found duplicate " EM_CSTR("case") " value " EM_VARG " in " EM_CSTR("switch") " statement";
        case MSG_string_init_not_char_arr:
            RET_ERRNO "non-character array type " EM_VARG " initialized from string literal";
        case MSG_string_init_overflow:
            RET_ERRNO "size " EM_VARG " string literal initialized with " EM_VARG " characters";
        case MSG_arr_init_overflow:
            RET_ERRNO "size " EM_VARG " array type " EM_VARG " initialized with " EM_VARG " initializers";
        case MSG_struct_init_overflow:
            RET_ERRNO "data structure type " EM_VARG " initialized with " EM_VARG " members instead of " EM_VARG;
        case MSG_ret_arr:
            RET_ERRNO "function " EM_VARG " returns array type " EM_VARG ", instead of pointer type";
        case MSG_ret_incomplete:
            RET_ERRNO "function " EM_VARG " returns incomplete data structure type " EM_VARG;
        case MSG_void_param:
            RET_ERRNO "function " EM_VARG " declared with parameter " EM_VARG " with type " EM_CSTR("void");
        case MSG_incomplete_param:
            RET_ERRNO "function " EM_VARG " defined with parameter " EM_VARG
                      " with incomplete data structure type " EM_VARG;
        case MSG_redecl_fun_conflict:
            RET_ERRNO "function " EM_VARG " redeclared with function type " EM_VARG
                      ", but previous declaration has function type " EM_VARG;
        case MSG_redef_fun:
            RET_ERRNO "function " EM_VARG " already defined with function type " EM_VARG;
        case MSG_redecl_static_conflict:
            RET_ERRNO "function " EM_VARG " with " EM_CSTR("static") " storage class already declared non-static";
        case MSG_static_ptr_init_not_int:
            RET_ERRNO "cannot statically initialize pointer type " EM_VARG " from constant " EM_VARG
                      ", requires a constant integer";
        case MSG_static_ptr_init_not_null:
            RET_ERRNO "cannot statically initialize pointer type " EM_VARG " from non-null value " EM_VARG;
        case MSG_agg_init_with_single:
            RET_ERRNO "aggregate type " EM_VARG " statically initialized with single initializer";
        case MSG_static_ptr_init_string:
            RET_ERRNO "non-character pointer type " EM_VARG " statically initialized from string literal";
        case MSG_static_init_not_const:
            RET_ERRNO "cannot statically initialize variable from non-constant type " EM_VARG ", requires a constant";
        case MSG_scalar_init_with_compound:
            RET_ERRNO "cannot initialize scalar type " EM_VARG " with compound initializer";
        case MSG_void_var_decl:
            RET_ERRNO "variable " EM_VARG " declared with type " EM_CSTR("void");
        case MSG_incomplete_var_decl:
            RET_ERRNO "variable " EM_VARG " declared with incomplete data structure type " EM_VARG;
        case MSG_redecl_var_conflict:
            RET_ERRNO "variable " EM_VARG " redeclared with conflicting type " EM_VARG
                      ", but previously declared with type " EM_VARG;
        case MSG_redecl_var_storage:
            RET_ERRNO "variable " EM_VARG " redeclared with conflicting storage class";
        case MSG_redef_extern_var:
            RET_ERRNO "illegal initializer, can only declare variable " EM_VARG
                      " with " EM_CSTR("extern") " storage class";
        case MSG_duplicate_member_decl:
            RET_ERRNO "data structure type " EM_VARG " declared with duplicate member name " EM_VARG;
        case MSG_incomplete_member_decl:
            RET_ERRNO "data structure type " EM_VARG " declared with member " EM_VARG " with incomplete type " EM_VARG;
        case MSG_redecl_struct_in_scope:
            RET_ERRNO "data structure type " EM_VARG " already declared in this scope";
        case MSG_case_out_of_switch:
            RET_ERRNO "found " EM_CSTR("case") " statement outside of " EM_CSTR("switch");
        case MSG_default_out_of_switch:
            RET_ERRNO "found " EM_CSTR("default") " statement outside of " EM_CSTR("switch");
        case MSG_multiple_default:
            RET_ERRNO "found more than one " EM_CSTR("default") " statement in " EM_CSTR("switch");
        case MSG_break_out_of_loop:
            RET_ERRNO "found " EM_CSTR("break") " statement outside of loop";
        case MSG_continue_out_of_loop:
            RET_ERRNO "found " EM_CSTR("continue") " statement outside of loop";
        case MSG_undef_goto_target:
            RET_ERRNO "found " EM_CSTR("goto") " statement, but target label " EM_VARG
                                               " not defined in function " EM_VARG;
        case MSG_redecl_struct_conflict:
            RET_ERRNO EM_VARG " conflicts with data structure type " EM_VARG
                              " previously declared or defined in this scope";
        case MSG_undef_struct_in_scope:
            RET_ERRNO "data structure type " EM_VARG " not defined in this scope";
        case MSG_undecl_var_in_scope:
            RET_ERRNO "variable " EM_VARG " not declared in this scope";
        case MSG_undecl_fun_in_scope:
            RET_ERRNO "function " EM_VARG " not declared in this scope";
        case MSG_for_init_decl_not_auto:
            RET_ERRNO "variable " EM_VARG " declared with " EM_VARG
                      " storage class in " EM_CSTR("for") " loop initial declaration";
        case MSG_redef_label_in_scope:
            RET_ERRNO "label " EM_VARG " already defined in this scope";
        case MSG_redecl_var_in_scope:
            RET_ERRNO "variable " EM_VARG " already declared in this scope";
        case MSG_def_nested_fun:
            RET_ERRNO "function " EM_VARG
                      " defined inside another function, but nested function definition are not permitted";
        case MSG_decl_nested_static_fun:
            RET_ERRNO "cannot declare nested function " EM_VARG
                      " in another function with " EM_CSTR("static") " storage class";
        case MSG_redecl_fun_in_scope:
            RET_ERRNO "function " EM_VARG " already declared in this scope";
        default:
            THROW_ABORT;
    }
}
