TacConstant(std::shared_ptr<CConst>@@ constant)+
TacVariable(TIdentifier name)+
TacPlainOperand(std::shared_ptr<TacValue>@@ val)+
TacDereferencedPointer(std::shared_ptr<TacValue>@@ val)+
TacSubObject(TIdentifier base_name, TLong offset)+
TacReturn(std::shared_ptr<TacValue>@@ val)+
TacSignExtend(std::shared_ptr<TacValue>@@ src, std::shared_ptr<TacValue>@@ dst)+
TacTruncate(std::shared_ptr<TacValue>@@ src, std::shared_ptr<TacValue>@@ dst)+
TacZeroExtend(std::shared_ptr<TacValue>@@ src, std::shared_ptr<TacValue>@@ dst)+
TacDoubleToInt(std::shared_ptr<TacValue>@@ src, std::shared_ptr<TacValue>@@ dst)+
TacDoubleToUInt(std::shared_ptr<TacValue>@@ src, std::shared_ptr<TacValue>@@ dst)+
TacIntToDouble(std::shared_ptr<TacValue>@@ src, std::shared_ptr<TacValue>@@ dst)+
TacUIntToDouble(std::shared_ptr<TacValue>@@ src, std::shared_ptr<TacValue>@@ dst)+
TacFunCall(TIdentifier name, std::vector<std::shared_ptr<TacValue>@@>@@ args, std::shared_ptr<TacValue>@@ dst)+
TacUnary(std::unique_ptr<TacUnaryOp>@@ unop, std::shared_ptr<TacValue>@@ src, std::shared_ptr<TacValue>@@ dst)+
TacBinary(std::unique_ptr<TacBinaryOp>@@ binop, std::shared_ptr<TacValue>@@ src1, std::shared_ptr<TacValue>@@ src2, std::shared_ptr<TacValue>@@ dst)+
TacCopy(std::shared_ptr<TacValue>@@ src, std::shared_ptr<TacValue>@@ dst)+
TacGetAddress(std::shared_ptr<TacValue>@@ src, std::shared_ptr<TacValue>@@ dst)+
TacLoad(std::shared_ptr<TacValue>@@ src_ptr, std::shared_ptr<TacValue>@@ dst)+
TacStore(std::shared_ptr<TacValue>@@ src, std::shared_ptr<TacValue>@@ dst_ptr)+
TacAddPtr(TLong scale, std::shared_ptr<TacValue>@@ src_ptr, std::shared_ptr<TacValue>@@ idx, std::shared_ptr<TacValue>@@ dst)+
TacCopyToOffset(TIdentifier dst_name, TLong offset, std::shared_ptr<TacValue>@@ src)+
TacCopyFromOffset(TIdentifier src_name, TLong offset, std::shared_ptr<TacValue>@@ dst)+
TacJump(TIdentifier target)+
TacJumpIfZero(TIdentifier target, std::shared_ptr<TacValue>@@ condition)+
TacJumpIfNotZero(TIdentifier target, std::shared_ptr<TacValue>@@ condition)+
TacLabel(TIdentifier name)+
TacFunction(TIdentifier name, bool is_glob, std::vector<TIdentifier>@@ params, std::vector<std::unique_ptr<TacInstruction>>@@ body)+
TacStaticVariable(TIdentifier name, bool is_glob, std::shared_ptr<Type>@@ static_init_type, std::vector<std::shared_ptr<StaticInit>@@>@@ static_inits)+
TacStaticConstant(TIdentifier name, std::shared_ptr<Type>@@ static_init_type, std::shared_ptr<StaticInit>@@ static_init)+
TacProgram(std::vector<std::unique_ptr<TacTopLevel>>@@ static_const_toplvls, std::vector<std::unique_ptr<TacTopLevel>>@@ static_var_toplvls, std::vector<std::unique_ptr<TacTopLevel>>@@ fun_toplvls)+
