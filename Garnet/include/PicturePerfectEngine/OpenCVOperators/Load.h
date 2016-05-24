
class I1B_Load_S : public Operator
{
public :
	virtual ResultType  execute(ProgramPointer& program, RegisterFile& regs) {
		IplImage*    dst  = regs[*program++].get<IplImage*>();
		std::wstring& src  = regs[*program++].get<std::wstring>();

		std::string  filename = Text::toMultiByte(src);

		IplImage* temp = cvLoadImage(filename.c_str(), 0); // Grayscale
		cvResize(temp, dst, CV_INTER_LINEAR);
		cvReleaseImage(&temp);

		return RES_NEXT;
	}
};

class I3B_Load_S : public Operator
{
public :
	virtual ResultType  execute(ProgramPointer& program, RegisterFile& regs) {
		IplImage*     dst  = regs[*program++].get<IplImage*>();
		std::wstring& src  = regs[*program++].get<std::wstring>();

		std::string  filename = Text::toMultiByte(src);

		IplImage* temp = cvLoadImage(filename.c_str(), 1); // Color
		cvResize(temp, dst, CV_INTER_LINEAR);
		cvReleaseImage(&temp);

		return RES_NEXT;
	}
};
