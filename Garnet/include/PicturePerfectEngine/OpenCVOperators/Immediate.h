
class V1D_Immediate : public Operator
{
	virtual ResultType  execute(ProgramPointer& program, RegisterFile& regs) {
		double&   dst  = regs[*program++].get<double>();

		union {
			uint16_t  v[4];
			double    d;
		}         value;
		for (int i = 0; i < 4; i++) {
			value.v[i] = *program++;
		}

		dst = value.d;

		return RES_NEXT;
	}
};

class S_Immediate : public Operator
{
	virtual ResultType  execute(ProgramPointer& program, RegisterFile& regs) {
		std::wstring& dst = regs[*program++].get<std::wstring>();

		dst.assign(L"");

		wchar_t c;
		while ( (c = (wchar_t)*program++) != 0 ) {
			dst.push_back(c);
		}

		return RES_NEXT;
	}
};
