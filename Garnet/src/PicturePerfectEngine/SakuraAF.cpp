
#define NOMINMAX

#include <algorithm>

#include <PicturePerfectEngine/Vector.h>
#include <PicturePerfectEngine/SakuraExecutionUnits.h>

PicturePerfect::ProgramIterator Sakura::AF::operator()(uint32_t index, PicturePerfect::ProgramIterator program, RegisterMap& regs)
{
	using namespace PicturePerfect;

	Vector<double>& out = regs[*program++]->at<Vector<double> >();
	struct {
		const PicturePerfect::DataSlot* data;
		double                d() { return data->at<double>(); }
		const Vector<double>& a() { return data->at<Vector<double> >(); }
	} in0, in1;
	#define _0v  (in0.d())
	#define _1v  (in1.d())
	#define _0a  (in0.a())
	#define _1a  (in1.a())

	switch ( index ) {
	// A1D_Zero_V1D
	case 0 :
		out.clear();
		break;
	// A1D_Move_V1D
	case 1 :
		out = regs[*program++]->at<Vector<double> >();
		break;
	// A1D_AAbsolute_A1D
	case 2 :
		in0.data = regs[*program++];
		out.resize(_0a.size());
		for (auto i = 0u; i < out.size(); i++) {
			out[i] = abs(_0a[i]);
		}
		break;
	// A1D_AAddV_A1D_V1D
	case 3 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		out.resize(_0a.size());
		for (auto i = 0u; i < out.size(); i++) {
			out[i] = _0a[i] + _1v;
		}
		break;
	// A1D_ADivV_A1D_V1D
	case 4 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		out.resize(_0a.size());
		for (auto i = 0u; i < out.size(); i++) {
			out[i] = (_1v != 0) ? (_0a[i] / _1v) : std::numeric_limits<double>::infinity();
		}
		break;
	// A1D_ADivV_V1D_A1D
	case 5 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		out.resize(_1a.size());
		for (auto i = 0u; i < out.size(); i++) {
			out[i] = (_1a[i] != 0) ? (_0v / _1a[i]) : std::numeric_limits<double>::infinity();
		}
		break;
	// A1D_ASubtractV_A1D_V1D
	case 6 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		out.resize(_0a.size());
		for (auto i = 0u; i < out.size(); i++) {
			out[i] = _0a[i] - _1v;
		}
		break;
	// A1D_ASubtractV_V1D_A1D
	case 7 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		out.resize(_1a.size());
		for (auto i = 0u; i < out.size(); i++) {
			out[i] = _0v - _1a[i];
		}
		break;
	}

	return program;
}
