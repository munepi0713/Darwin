class SetImageSize : public Operator
{
public :
	virtual ResultType  execute(ProgramPointer& program, RegisterFile& regs) {
		Program::value_type width  = *program++;
		Program::value_type height = *program++;

		regs.setImageSize(cv::Size(width, height));

		return RES_NEXT;
	}
};
