class V1D_Zero : public Operator
{
public :
	virtual ResultType  execute(ProgramPointer& program, RegisterFile& regs) {
		double& dst = regs[*program++].get<double>();
		dst = 0;
		return RES_NEXT;
	}
};

class I1B_Zero : public Operator
{
public :
	virtual ResultType  execute(ProgramPointer& program, RegisterFile& regs) {
		IplImage* dst = regs[*program++].get<IplImage*>();
		cvSetZero(dst);
		return RES_NEXT;
	}
};

class I3B_Zero : public Operator
{
public :
	virtual ResultType  execute(ProgramPointer& program, RegisterFile& regs) {
		IplImage* dst = regs[*program++].get<IplImage*>();
		cvSetZero(dst);
		return RES_NEXT;
	}
};
