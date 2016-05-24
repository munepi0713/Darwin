
#define NOMINMAX

#include <algorithm>

#include <PicturePerfectEngine/Vector.h>
#include <PicturePerfectEngine/SakuraExecutionUnits.h>

PicturePerfect::ProgramIterator Sakura::VF::operator()(uint32_t index, PicturePerfect::ProgramIterator program, RegisterMap& regs)
{
	using namespace PicturePerfect;

	double& out = regs[*program++]->at<double>();
	struct {
		const PicturePerfect::DataSlot* data;
		double                v() { return data->at<double>(); }
		const cv::Mat&        i() { return data->at<cv::Mat>(); }
		const Vector<double>& a() { return data->at<Vector<double> >(); }
	} in0, in1, in2, in3;
	#define _0v  (in0.v())
	#define _1v  (in1.v())
	#define _2v  (in2.v())
	#define _3v  (in3.v())
	#define _0i  (in0.i())
	#define _1i  (in1.i())
	#define _2i  (in2.i())
	#define _0a  (in0.a())

	//std::cout << " VF" << index;

	switch ( index ) {
	// V1D_Zero
	case 0 :
		out = 0.0;
		break;
	// V1D_Move_V1D
	case 1 :
		out = regs[*program++]->at<double>();
		break;
	// V1D_AvgPixel_I1B
	case 2 :
		in0.data = regs[*program++];
		out = (double)cv::mean(_0i).val[0] / 255.0;
		break;
	// V1D_MaxPixel_I1B
	case 3 :
		in0.data = regs[*program++];
		cv::minMaxLoc(_0i, NULL, &out);
		out = out / 255.0;
		break;
	// V1D_MinPixel_I1B
	case 4 :
		in0.data = regs[*program++];
		cv::minMaxLoc(_0i, &out, NULL);
		out = out / 255.0;
		break;
	// V1D_FGArea_I1B
	case 5 :
		in0.data = regs[*program++];
		{
			cv::Mat temp;
			cv::threshold(_0i, temp, 0, 1, CV_THRESH_BINARY | CV_THRESH_OTSU);
			out = (double)cv::countNonZero(temp) / (temp.rows * temp.cols);
		}
		break;
	// V1D_BGArea_I1B
	case 6 :
		in0.data = regs[*program++];
		{
			cv::Mat temp;
			cv::threshold(_0i, temp, 0, 1, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
			out = (double)cv::countNonZero(temp) / (temp.rows * temp.cols);
		}
		break;
	// V1D_FGAreaV_I1B_V1D
	case 7 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		{
			cv::Mat temp;
			cv::threshold(_0i, temp, _1v, 1, CV_THRESH_BINARY);
			out = (double)cv::countNonZero(temp) / (temp.rows * temp.cols);
		}
		break;
	// V1D_BGAreaV_I1B_V1D
	case 8 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		{
			cv::Mat temp;
			cv::threshold(_0i, temp, _1v, 1, CV_THRESH_BINARY_INV);
			out = (double)cv::countNonZero(temp) / (temp.rows * temp.cols);
		}
		break;
	// V1D_MassCenterBX_I1B
	case 9 :
		{
			in0.data = regs[*program++];
			cv::Moments moments = cv::moments(_0i, true);
			double gX = moments.m10 / moments.m00;
			//double gY = moments.m01 / moments.m00;
			out = gX / _0i.rows;
		}
		break;
	// V1D_MassCenterBY_I1B
	case 10 :
		{
			in0.data = regs[*program++];
			cv::Moments moments = cv::moments(_0i, true);
			//double gX = moments.m10 / moments.m00;
			double gY = moments.m01 / moments.m00;
			out = gY / _0i.cols;
		}
		break;
	// V1D_PSNR_I1B_I1B
	case 11 :
		{
			in0.data = regs[*program++];
			in1.data = regs[*program++];
			unsigned long sqsum = 0;
			for (int y = 0; y < _0i.rows; y++) {
				unsigned char* p1 = (unsigned char*)_0i.ptr(y);
				unsigned char* p2 = (unsigned char*)_1i.ptr(y);
				for (int x = 0; x < _0i.cols; x++) {
					long temp = ((int)p1[x] - (int)p2[x]);
					sqsum += temp * temp;
				}
			}
			if ( sqsum == 0 ) {
				out = 1.0;
			}
			else {
				double temp = 0.2 * log(255.0 / sqrt((double)sqsum / (_0i.rows * _0i.cols)));
				if ( temp > 1.0 ) temp = 1.0;
				if ( temp < 0.0 ) temp = 0.0;
				out = temp;
			}
		}
		break;
	// V1D_VNegate_V1D
	case 12 :
		in0.data = regs[*program++];
		out = 1.0 - _0v;
		break;
	// V1D_VAdd_V1D_V1D
	case 13 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		out = std::min(_0v + _1v, 1.0);
		break;
	// V1D_VAlgebraicProd_V1D_V1D
	case 14 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		out = _0v * _1v;
		break;
	// V1D_VAlgebraicSum_V1D_V1D
	case 15 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		out = _0v + _1v - _0v * _1v;
		break;
	// V1D_VDiff_V1D_V1D
	case 16 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		out = std::abs(_0v - _1v);
		break;
	// V1D_VLogicalProd_V1D_V1D
	case 17 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		out = std::min(_0v, _1v);
		break;
	// V1D_VLogicalSum_V1D_V1D
	case 18 :
	// V1D_Select_V1D_V1D
	case 21 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		out = std::max(_0v, _1v);
		break;
	// V1D_VSubtract_V1D_V1D
	case 19 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		out = std::max(_0v - _1v, 0.0);
		break;
	// V1D_Distance_V1D_V1D_V1D_V1D
	case 20 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		in2.data = regs[*program++];
		in3.data = regs[*program++];
		{
			double w, h;
			double dx, dy;
			w = _0i.cols;
			h = _0i.rows;
			dx = (_0v * w) - (_2v * w);
			dy = (_1v * h) - (_3v * h);
			out = sqrt(dx * dx + dy * dy) / sqrt(w * w + h * h);
			break;
		}
		break;
	// V1D_Select_V1D_V1D_V1D_V1D
	case 22 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		in2.data = regs[*program++];
		in3.data = regs[*program++];
		out = (_2v >= _3v) ? _0v : _1v;
		break;
	//V1D_Adjust_V1D
	case 23 :
		in0.data = regs[*program++];
		out = 1.0 / (1.0 + _0v);
		break;
	//V1D_AverageA_A1D
	case 24 :
		in0.data = regs[*program++];
		out = _0a.average();
		break;
	//V1D_Invert_V1D
	case 25 :
		in0.data = regs[*program++];
		out = _0v != 0 ? 1.0 / _0v : std::numeric_limits<double>::infinity();
		break;
	//V1D_MaxA_A1D
	case 26 :
		in0.data = regs[*program++];
		out = _0a.max();
		break;
	//V1D_MinA_A1D
	case 27 :
		in0.data = regs[*program++];
		out = _0a.min();
		break;
	//V1D_MSEA_A1D_V1D
	case 28 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		{
			double sqsum = 0;
			for (auto i = 0u; i < _0a.size(); i++) {
				double x = (_0a[i] - _1v);
				sqsum += x * x;
			}
			out = sqsum / _0a.size();
		}
		break;
	//V1D_NumA_A1D
	case 29 :
		in0.data = regs[*program++];
		out = static_cast<double>(_0a.size());
		break;
	//V1D_NumEqA_A1D_V1D
	case 30 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		{
			uint32_t count = 0;
			for (auto i = 0u; i < _0a.size(); i++) {
				if ( _0a[i] == _1v ) count++;
			}
			out = count;
		}
		break;
	//V1D_NumGreaterA_A1D_V1D
	case 31 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		{
			uint32_t count = 0;
			for (auto i = 0u; i < _0a.size(); i++) {
				if ( _0a[i] > _1v ) count++;
			}
			out = count;
		}
		break;
	//V1D_NumGreaterEqA_A1D_V1D
	case 32 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		{
			uint32_t count = 0;
			for (auto i = 0u; i < _0a.size(); i++) {
				if ( _0a[i] >= _1v ) count++;
			}
			out = count;
		}
		break;
	//V1D_NumLessA_A1D_V1D
	case 33 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		{
			uint32_t count = 0;
			for (auto i = 0u; i < _0a.size(); i++) {
				if ( _0a[i] < _1v ) count++;
			}
			out = count;
		}
		break;
	//V1D_NumLessEqA_A1D_V1D
	case 34 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		{
			uint32_t count = 0;
			for (auto i = 0u; i < _0a.size(); i++) {
				if ( _0a[i] <= _1v ) count++;
			}
			out = count;
		}
		break;
	//V1D_PercentV_V1D_V1D
	case 35 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		out = _0v * _1v / 100.0;
		break;
	//V1D_RMSA_A1D
	case 36 :
		in0.data = regs[*program++];
		{
			double sqsum = 0;
			for (auto i = 0u; i < _0a.size(); i++) {
				sqsum += _0a[i] * _0a[i];
			}
			out = sqrt(sqsum / _0a.size());
		}
		break;
	//V1D_Sqrt_V1D
	case 37 :
		in0.data = regs[*program++];
		out = sqrt(_0v);
		break;
	//V1D_StdevA_A1D
	case 38 :
		in0.data = regs[*program++];
		out = _0a.stdev();
		break;
	//V1D_SumA_A1D
	case 39 :
		in0.data = regs[*program++];
		out = _0a.sum();
		break;
	//V1D_VAddV_V1D_V1D
	case 40 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		out = _0v + _1v;
		break;
	//V1D_VDivV_V1D_V1D
	case 41 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		out = (_1v != 0) ? (_0v / _1v) : std::numeric_limits<double>::infinity();
		break;
	//V1D_VMultiplyV_V1D_V1D
	case 42 :
		in0.data = regs[*program++];
		in1.data = regs[*program++];
		out = _0v * _1v;
		break;
	//V1D_VSignFlip_V1D
	case 43 :
		in0.data = regs[*program++];
		out = -_0v;
		break;
	//V1D_VExp_V1D
	case 44 :
		in0.data = regs[*program++];
		out = exp(_0v);
		break;
	//V1D_VInvertC_V1D
	case 45 :
		in0.data = regs[*program++];
		out = 1.0 / (_0v + 1.0e-6);
		break;
	// V1D_MaskPSNR_I1B_I1B_I1B
	case 46 :
		{
			in0.data = regs[*program++];
			in1.data = regs[*program++];
			in2.data = regs[*program++];
			unsigned long sqsum = 0;
			unsigned long pixels = 0;
			for (int y = 0; y < _0i.rows; y++) {
				unsigned char* p1 = (unsigned char*)_0i.ptr(y);
				unsigned char* p2 = (unsigned char*)_1i.ptr(y);
				unsigned char* p3 = (unsigned char*)_2i.ptr(y);
				for (int x = 0; x < _0i.cols; x++) {
					if ( p3[x] > 0 ) {
						long temp = ((int)p1[x] - (int)p2[x]);
						sqsum += temp * temp;
						pixels++;
					}
				}
			}
			if ( sqsum == 0 || pixels == 0 ) {
				out = 1.0;
			}
			else {
				double temp = 0.2 * log(255.0 / sqrt((double)sqsum / pixels));
				if ( temp > 1.0 ) temp = 1.0;
				if ( temp < 0.0 ) temp = 0.0;
				out = temp;
			}
		}
		break;
	// V1D_NoExtremeMaskPSNR_I1B_I1B_I1B
	case 47 :
		{
			in0.data = regs[*program++];
			in1.data = regs[*program++];
			in2.data = regs[*program++];
			unsigned long sqsum = 0;
			unsigned long pixels = 0;
			unsigned long p1sum = 0;
			for (int y = 0; y < _0i.rows; y++) {
				unsigned char* p1 = (unsigned char*)_0i.ptr(y);
				unsigned char* p2 = (unsigned char*)_1i.ptr(y);
				unsigned char* p3 = (unsigned char*)_2i.ptr(y);
				for (int x = 0; x < _0i.cols; x++) {
					if (p3[x] > 0) {
						long temp = ((int)p1[x] - (int)p2[x]);
						sqsum += temp * temp;
						p1sum += p1[x];
						pixels++;
					}
				}
			}
			if (pixels == 0) {
				out = 1.0;
			}
			else if (sqsum == 0 || p1sum == 0 || p1sum == 255 * pixels) {
				// Assign 0 if the all pixels match or are black or are white.
				out = 0.0;
			}
			else {
				double temp = 0.2 * log(255.0 / sqrt((double)sqsum / pixels));
				if (temp > 1.0) temp = 1.0;
				if (temp < 0.0) temp = 0.0;
				out = temp;
			}
		}
		break;
	}

	return program;
}
