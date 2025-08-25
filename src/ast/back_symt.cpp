#include <memory>

#include "util/c_std.h"

#include "ast/ast.h"
#include "ast/back_symt.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Backend symbol table

AST_T AssemblyType::type() { return AST_AssemblyType_t; }
AST_T Byte::type() { return AST_Byte_t; }
AST_T LongWord::type() { return AST_LongWord_t; }
AST_T QuadWord::type() { return AST_QuadWord_t; }
AST_T BackendDouble::type() { return AST_BackendDouble_t; }
AST_T ByteArray::type() { return AST_ByteArray_t; }
AST_T BackendSymbol::type() { return AST_BackendSymbol_t; }
AST_T BackendObj::type() { return AST_BackendObj_t; }
AST_T BackendFun::type() { return AST_BackendFun_t; }

ByteArray::ByteArray(TLong size, TInt alignment) : size(size), alignment(alignment) {}

BackendObj::BackendObj(bool is_static, bool is_const, std::shared_ptr<AssemblyType>&& asm_type) :
    is_static(is_static), is_const(is_const), asm_type(std::move(asm_type)) {}

BackendFun::BackendFun() : callee_saved_regs(vec_new()) {}
BackendFun::BackendFun(bool is_def) : is_def(is_def), callee_saved_regs(vec_new()) {}
BackendFun::~BackendFun() {
    for (size_t i = 0; i < vec_size(this->callee_saved_regs); ++i) {
        this->callee_saved_regs[i].reset();
    }
    vec_delete(this->callee_saved_regs);
}
