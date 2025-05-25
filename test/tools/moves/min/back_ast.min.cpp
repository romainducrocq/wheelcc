AsmImm(TULong value, bool is_byte, bool is_quad, bool is_neg)+
AsmRegister(std::unique_ptr<AsmReg>@@ reg)+
AsmPseudo(TIdentifier name)+
AsmMemory(TLong value, std::unique_ptr<AsmReg>@@ reg)+
AsmData(TIdentifier name, TLong offset)+
AsmPseudoMem(TIdentifier name, TLong offset)+
AsmIndexed(TLong scale, std::unique_ptr<AsmReg>@@ reg_base, std::unique_ptr<AsmReg>@@ reg_index)+
AsmMov(std::shared_ptr<AssemblyType>@@ asm_type, std::shared_ptr<AsmOperand>@@ src, std::shared_ptr<AsmOperand>@@ dst)+
AsmMovSx(std::shared_ptr<AssemblyType>@@ asm_type_src, std::shared_ptr<AssemblyType>@@ asm_type_dst, std::shared_ptr<AsmOperand>@@ src, std::shared_ptr<AsmOperand>@@ dst)+
AsmMovZeroExtend(std::shared_ptr<AssemblyType>@@ asm_type_src, std::shared_ptr<AssemblyType>@@ asm_type_dst, std::shared_ptr<AsmOperand>@@ src, std::shared_ptr<AsmOperand>@@ dst)+
AsmLea(std::shared_ptr<AsmOperand>@@ src, std::shared_ptr<AsmOperand>@@ dst)+
AsmCvttsd2si(std::shared_ptr<AssemblyType>@@ asm_type, std::shared_ptr<AsmOperand>@@ src, std::shared_ptr<AsmOperand>@@ dst)+
AsmCvtsi2sd(std::shared_ptr<AssemblyType>@@ asm_type, std::shared_ptr<AsmOperand>@@ src, std::shared_ptr<AsmOperand>@@ dst)+
AsmUnary(std::unique_ptr<AsmUnaryOp>@@ unop, std::shared_ptr<AssemblyType>@@ asm_type, std::shared_ptr<AsmOperand>@@ dst)+
AsmBinary(std::unique_ptr<AsmBinaryOp>@@ binop, std::shared_ptr<AssemblyType>@@ asm_type, std::shared_ptr<AsmOperand>@@ src, std::shared_ptr<AsmOperand>@@ dst)+
AsmCmp(std::shared_ptr<AssemblyType>@@ asm_type, std::shared_ptr<AsmOperand>@@ src, std::shared_ptr<AsmOperand>@@ dst)+
AsmIdiv(std::shared_ptr<AssemblyType>@@ asm_type, std::shared_ptr<AsmOperand>@@ src)+
AsmDiv(std::shared_ptr<AssemblyType>@@ asm_type, std::shared_ptr<AsmOperand>@@ src)+
AsmCdq(std::shared_ptr<AssemblyType>@@ asm_type)+
AsmJmp(TIdentifier target)+
AsmJmpCC(TIdentifier target, std::unique_ptr<AsmCondCode>@@ cond_code)+
AsmSetCC(std::unique_ptr<AsmCondCode>@@ cond_code, std::shared_ptr<AsmOperand>@@ dst)+
AsmLabel(TIdentifier name)+
AsmPush(std::shared_ptr<AsmOperand>@@ src)+
AsmPop(std::unique_ptr<AsmReg>@@ reg)+
AsmCall(TIdentifier name)+
AsmFunction(TIdentifier name, bool is_glob, bool is_ret_memory, std::vector<std::unique_ptr<AsmInstruction>>@@ instructions)+
AsmStaticVariable(TIdentifier name, TInt alignment, bool is_glob, std::vector<std::shared_ptr<StaticInit>@@>@@ static_inits)+
AsmStaticConstant(TIdentifier name, TInt alignment, std::shared_ptr<StaticInit>@@ static_init)+
AsmProgram(std::vector<std::unique_ptr<AsmTopLevel>>@@ static_const_toplvls, std::vector<std::unique_ptr<AsmTopLevel>>@@ top_levels)+
