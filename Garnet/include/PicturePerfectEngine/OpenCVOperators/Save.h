
class Save_I1B_S : public Operator
{
public :
	virtual ResultType  execute(ProgramPointer& program, RegisterFile& regs) {
		IplImage*     src1 = regs[*program++].get<IplImage*>();
		std::wstring& src2 = regs[*program++].get<std::wstring>();

		std::string filename = Text::toMultiByte(src2);

		cvSaveImage(filename.c_str(), src1);

		return RES_NEXT;
	}
};

class Save_I3B_S : public Operator
{
public :
	virtual ResultType  execute(ProgramPointer& program, RegisterFile& regs) {
		IplImage*     src1 = regs[*program++].get<IplImage*>();
		std::wstring& src2 = regs[*program++].get<std::wstring>();

		std::string filename = Text::toMultiByte(src2);

		cvSaveImage(filename.c_str(), src1);

		return RES_NEXT;
	}
};
