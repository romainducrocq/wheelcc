CAbstractPointer(std::unique_ptr<CAbstractDeclarator>@@ abstract_decltor)+
CAbstractArray(TLong size, std::unique_ptr<CAbstractDeclarator>@@ abstract_decltor)+
CParam(std::unique_ptr<CDeclarator>@@ decltor, std::shared_ptr<Type> param_type)+
CIdent(TIdentifier name)+
CPointerDeclarator(std::unique_ptr<CDeclarator>@@ decltor)+
CArrayDeclarator(TLong size, std::unique_ptr<CDeclarator>@@ decltor)+
CFunDeclarator(std::vector<std::unique_ptr<CParam>> param_list, std::unique_ptr<CDeclarator>@@ decltor)+
CExp(size_t line)+
CConstant(std::shared_ptr<CConst> constant, size_t line)+
CString(std::shared_ptr<CStringLiteral> literal, size_t line)+
CVar(TIdentifier name, size_t line)+
CCast(std::unique_ptr<CExp>@@ exp, std::shared_ptr<Type> target_type, size_t line)+
CUnary(std::unique_ptr<CUnaryOp>@@ unop, std::unique_ptr<CExp>@@ exp, size_t line)+
CBinary(std::unique_ptr<CBinaryOp>@@ binop, std::unique_ptr<CExp>@@ exp_left, std::unique_ptr<CExp>@@ exp_right, size_t line)+
CAssignment(std::unique_ptr<CUnaryOp>@@ unop, std::unique_ptr<CExp>@@ exp_left, std::unique_ptr<CExp>@@ exp_right, size_t line)+
CConditional(std::unique_ptr<CExp>@@ condition, std::unique_ptr<CExp>@@ exp_middle, std::unique_ptr<CExp>@@ exp_right, size_t line)+
CFunctionCall(TIdentifier name, std::vector<std::unique_ptr<CExp>> args, size_t line)+
CDereference(std::unique_ptr<CExp>@@ exp, size_t line)+
CAddrOf(std::unique_ptr<CExp>@@ exp, size_t line)+
CSubscript(std::unique_ptr<CExp>@@ primary_exp, std::unique_ptr<CExp>@@ subscript_exp, size_t line)+
CSizeOf(std::unique_ptr<CExp>@@ exp, size_t line)+
CSizeOfT(std::shared_ptr<Type> target_type, size_t line)+
CDot(TIdentifier member, std::unique_ptr<CExp>@@ structure, size_t line)+
CArrow(TIdentifier member, std::unique_ptr<CExp>@@ pointer, size_t line)+
CReturn(std::unique_ptr<CExp>@@ exp, size_t line)+
CExpression(std::unique_ptr<CExp>@@ exp)+
CIf(std::unique_ptr<CExp>@@ condition, std::unique_ptr<CStatement>@@ then, std::unique_ptr<CStatement>@@ else_fi)+
CGoto(TIdentifier target, size_t line)+
CLabel(TIdentifier target, std::unique_ptr<CStatement>@@ jump_to, size_t line)+
CCompound(std::unique_ptr<CBlock>@@ block)+
CWhile(std::unique_ptr<CExp>@@ condition, std::unique_ptr<CStatement>@@ body)+
CDoWhile(std::unique_ptr<CExp>@@ condition, std::unique_ptr<CStatement>@@ body)+
CFor(std::unique_ptr<CForInit>@@ init, std::unique_ptr<CExp>@@ condition, std::unique_ptr<CExp>@@ post, std::unique_ptr<CStatement>@@ body)+
CSwitch(std::unique_ptr<CExp>@@ match, std::unique_ptr<CStatement>@@ body)+
CCase(std::unique_ptr<CExp>@@ value, std::unique_ptr<CStatement>@@ jump_to)+
CDefault(std::unique_ptr<CStatement>@@ jump_to, size_t line)+
CBreak(size_t line)+
CContinue(size_t line)+
CInitDecl(std::unique_ptr<CVariableDeclaration>@@ init)+
CInitExp(std::unique_ptr<CExp>@@ init)+
CB(std::vector<std::unique_ptr<CBlockItem>> block_items)+
CS(std::unique_ptr<CStatement>@@ statement)+
CD(std::unique_ptr<CDeclaration>@@ declaration)+
CSingleInit(std::unique_ptr<CExp>@@ exp)+
CCompoundInit(std::vector<std::unique_ptr<CInitializer>> initializers)+
CMemberDeclaration(TIdentifier member_name, std::shared_ptr<Type> member_type, size_t line)+
CStructDeclaration(TIdentifier tag, bool is_union, std::vector<std::unique_ptr<CMemberDeclaration>> members, size_t line)+
CFunctionDeclaration(TIdentifier name, std::vector<TIdentifier> params, std::unique_ptr<CBlock>@@ body, std::shared_ptr<Type> fun_type, std::unique_ptr<CStorageClass>@@ storage_class, size_t line)+
CVariableDeclaration(TIdentifier name, std::unique_ptr<CInitializer>@@ init, std::shared_ptr<Type> var_type, std::unique_ptr<CStorageClass>@@ storage_class, size_t line)+
CFunDecl(std::unique_ptr<CFunctionDeclaration>@@ fun_decl)+
CVarDecl(std::unique_ptr<CVariableDeclaration>@@ var_decl)+
CStructDecl(std::unique_ptr<CStructDeclaration>@@ struct_decl)+
CProgram(std::vector<std::unique_ptr<CDeclaration>> declarations)+
