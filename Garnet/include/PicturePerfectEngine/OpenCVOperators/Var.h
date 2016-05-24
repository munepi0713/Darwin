template <uint32_t T>
class X_Var : public Operator
{
public :
	virtual ResultType  execute(ProgramPointer& program, RegisterFile& regs) {
		regs.create(*program++, T);
		return RES_NEXT;
	}
};

typedef class X_Var<DATA_V1D>  V1D_Var;
typedef class X_Var<DATA_I1B>  I1B_Var;
typedef class X_Var<DATA_I3B>  I3B_Var;
typedef class X_Var<DATA_S>    S_Var;
