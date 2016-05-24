class V1D_Copy_V1D : public Operator
{
public :
	virtual ResultType  execute(ProgramPointer& program, RegisterFile& regs) {
		double& dst = regs[*program++].get<double>();
		double& src = regs[*program++].get<double>();
		dst = src;
		return RES_NEXT;
	}
};

class I1B_Copy_I1B : public Operator
{
public :
	virtual ResultType  execute(ProgramPointer& program, RegisterFile& regs) {
		IplImage* dst = regs[*program++].get<IplImage*>();
		IplImage* src = regs[*program++].get<IplImage*>();
		cvCopy(src, dst);
		return RES_NEXT;
	}
};

class I3B_Copy_I3B : public Operator
{
public :
	virtual ResultType  execute(ProgramPointer& program, RegisterFile& regs) {
		IplImage* dst = regs[*program++].get<IplImage*>();
		IplImage* src = regs[*program++].get<IplImage*>();
		cvCopy(src, dst);
		return RES_NEXT;
	}
};
