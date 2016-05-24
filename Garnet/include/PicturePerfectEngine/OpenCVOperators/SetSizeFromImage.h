
class SetSizeFromImage : public Operator
{
public :
	virtual ResultType  execute(ProgramPointer& program, RegisterFile& regs) {
		std::wstring& src = regs[*program++].get<std::wstring>();

		std::string name = Text::toMultiByte(src);

		cv::Mat temp = cv::imread(name);
		if ( temp.empty() ) {
			throw "Failed to load image";
		}
		regs.setImageSize(cv::Size(temp.cols, temp.rows));

		return RES_NEXT;
	}
};
