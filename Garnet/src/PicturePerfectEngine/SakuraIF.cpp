#define NOMINMAX // Suppres max(a,b) and min(a,b) macros by windows.h

#include <Common/Synchronize.h>

#include <PicturePerfectEngine/SakuraExecutionUnits.h>

namespace
{

	class Thin
	{
	private :
		cv::Mat four_neighbor;
	public :
		Thin() {
			four_neighbor = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
		}

		void operator()(cv::Mat& dst, cv::Mat& src) {
			// Image
			//const int dx[] = { 1,  1,  0, -1, -1, -1,  0,  1 };
			//const int dy[] = { 0, -1, -1, -1,  0,  1,  1,  1 };

			src.copyTo(dst);
			cv::Mat& I = dst;
			for (;;) {
				cv::Mat candidate;
				cv::erode(I, candidate, four_neighbor);
				candidate = I - candidate;

				int num_delete = 0;
				for (int y = 1; y < I.rows - 1; y++) {
					uchar* IdataN = I.ptr<uchar>(y - 1);
					uchar* IdataC = I.ptr<uchar>(y);
					uchar* IdataP = I.ptr<uchar>(y + 1);
					uchar* cdata = candidate.ptr<uchar>(y);
					for (int x = 1; x < I.cols - 1; x++) {
						if ( cdata[x] == 0 ) {
							continue;
						}

						uchar n[8];
						int num = 0;
						n[0] = IdataP[x + 0] > 0 ? 1 : 0;
						n[1] = IdataP[x - 1] > 0 ? 1 : 0;
						n[2] = IdataC[x - 1] > 0 ? 1 : 0;
						n[3] = IdataN[x - 1] > 0 ? 1 : 0;
						n[4] = IdataN[x + 0] > 0 ? 1 : 0;
						n[5] = IdataN[x + 1] > 0 ? 1 : 0;
						n[6] = IdataC[x + 1] > 0 ? 1 : 0;
						n[7] = IdataP[x + 1] > 0 ? 1 : 0;
						num = n[0] + n[1] + n[2] + n[3] + n[4] + n[5] + n[6] + n[7];
						if ( num < 3 ) continue;

						// check connection count.
						int m = 0;
	#if 1
						m += n[0] * (1 - n[1] * n[2]);
						m += n[2] * (1 - n[3] * n[4]);
						m += n[4] * (1 - n[5] * n[6]);
						m += n[6] * (1 - n[7] * n[0]);
	#else
						for (int i = 0; i < 4; i++) {
							int k0 = i * 2;
							int k1 = (k0 + 1) % 8;
							int k2 = (k1 + 1) % 8;
							m += n[k0] * (1 - n[k1] * n[k2]);
						}
	#endif
						if ( m == 1 ) {
							IdataC[x] = 0;
							num_delete++;
						}
					}
				}

				if ( num_delete == 0 ) break;
			}
		}
	};
	static Thin _thin;

	/**
	 *  Summation of all elements should be 1.0 or 0.0.
	 *  If it is larger than 1.0, the image will be brigetened.
	 */

	#define DECLARE_KERNEL(name, ...)\
			static float _data_##name [9] = { __VA_ARGS__ };\
			static cv::Mat _kernel_##name(3, 3, CV_32FC1, _data_##name)

	DECLARE_KERNEL(Sharpen3x3,
		-1, -1, -1,
		-1,  9, -1,
		-1, -1, -1,
	);
	DECLARE_KERNEL(SobelX3x3,
		-1,  0,  1,
		-2,  0,  2,
		-1,  0,  1,
	);
	DECLARE_KERNEL(SobelY3x3,
		-1, -2, -1,
		 0,  0,  0,
		 1,  2,  1,
	);
	DECLARE_KERNEL(Laplacian3x3,
		 0,  1,  0,
		 1, -4,  1,
		 0,  1,  0,
	);
	DECLARE_KERNEL(Laplacian2nd3x3,
		 1,  1,  1,
		 1, -8,  1,
		 1,  1,  1,
	);
	DECLARE_KERNEL(PrewittX3x3,
		-1,  0,  1,
		-1,  0,  1,
		-1,  0,  1,
	);
	DECLARE_KERNEL(PrewittY3x3,
		-1, -1, -1,
		 0,  0,  0,
		 1,  1,  1,
	);
	DECLARE_KERNEL(LightDarkEdge3x3,
		 -1.f/3, -1.f/3, -1.f/3,
		 -1.f/3,  8.f/3, -1.f/3,
		 -1.f/3, -1.f/3, -1.f/3,
	);

	#undef DECLARE_KERNEL

	
	class DFT
	{
	private :
		bool     _isPrepared;
		cv::Mat  _planes[2];
		cv::Mat  _fbuffer;
		cv::Mat  _mask;
		double   _lastEllipse;

	public :
		DFT() {
			_isPrepared = false;
			_lastEllipse = -1;
		}

		void prepare(const cv::Size& dftSize) {
			_planes[0] = cv::Mat::zeros(dftSize, CV_32F);
			_planes[1] = cv::Mat::zeros(dftSize, CV_32F);
			_fbuffer = cv::Mat::zeros(dftSize, CV_32FC2);
			_mask = cv::Mat(dftSize, CV_8U);
			_isPrepared = true;
		}

		void executeHighPassFilter(cv::Mat& out, const cv::Mat& in1, double in2) {
			cv::Mat I(in1);

			cv::Size dftSize;
			dftSize.width  = cv::getOptimalDFTSize(I.cols);
			dftSize.height = cv::getOptimalDFTSize(I.rows);

			if ( !_isPrepared ) {
				_planes[0] = cv::Mat::zeros(dftSize, CV_32F);
				_planes[1] = cv::Mat::zeros(dftSize, CV_32F);
				_fbuffer = cv::Mat::zeros(dftSize, CV_32FC2);
				_isPrepared = true;
			}

			// copy src to padded float buffer.
			_planes[0] = cv::Scalar::all(0);
			_planes[1] = cv::Scalar::all(0);
			cv::Mat real(_planes[0], cv::Rect(0, 0, I.cols, I.rows));
			I.convertTo(real, real.type());

			cv::merge(_planes, 2, _fbuffer); // interleave 2 planes into a single plane.
		
        
			cv::dft(_fbuffer, _fbuffer);

			// swap quadrants to locate lower freq to center.
			int c2 = _fbuffer.cols / 2;
			int r2 = _fbuffer.rows / 2;
			cv::Mat q1, q2,  q3, q4, qt;
			q1 = _fbuffer(cv::Rect( 0,  0, c2, r2));
			q2 = _fbuffer(cv::Rect(c2, r2, c2, r2));
			q3 = _fbuffer(cv::Rect(c2,  0, c2, r2));
			q4 = _fbuffer(cv::Rect( 0, r2, c2, r2));
			q1.copyTo(qt);
			q2.copyTo(q1);
			qt.copyTo(q2);
			q3.copyTo(qt);
			q4.copyTo(q3);
			qt.copyTo(q4);

			// Cut lower freq elements.
			int radX = static_cast<int>(c2 * in2);
			int radY = static_cast<int>(r2 * in2);
			cv::ellipse(_fbuffer, cv::Point(c2, r2), cv::Size(radX, radY), 0, 0, 360, cv::Scalar(0), -1);

			// Re-swap quadrants.
			q1.copyTo(qt);
			q2.copyTo(q1);
			qt.copyTo(q2);
			q3.copyTo(qt);
			q4.copyTo(q3);
			qt.copyTo(q4);

			cv::idft(_fbuffer, _fbuffer);

			cv::split(_fbuffer, _planes); // de-interleave

			cv::Mat roi = _planes[0](cv::Rect(0, 0, I.cols, I.rows));

			double minVal, maxVal;
			cv::minMaxLoc(roi, &minVal, &maxVal);
			if ( maxVal < 255.0 ) maxVal = 255.0;

			cv::Mat J(out);
			roi.convertTo(J, J.type(), 255.0 / maxVal);
		}

		void executeLowPassFilter(cv::Mat& out, const cv::Mat& in1, double in2) {
			cv::Mat I(in1);
        
			cv::Size dftSize;
			dftSize.width  = cv::getOptimalDFTSize(I.cols);
			dftSize.height = cv::getOptimalDFTSize(I.rows);
        
			if ( !_isPrepared ) {
				_planes[0] = cv::Mat::zeros(dftSize, CV_32F);
				_planes[1] = cv::Mat::zeros(dftSize, CV_32F);
				_fbuffer = cv::Mat::zeros(dftSize, CV_32FC2);
				_isPrepared = true;
			}

			// copy src to padded float buffer.
			_planes[0] = cv::Scalar::all(0);
			_planes[1] = cv::Scalar::all(0);
			cv::Mat real(_planes[0], cv::Rect(0, 0, I.cols, I.rows));
			I.convertTo(real, real.type());
			cv::merge(_planes, 2, _fbuffer); // interleave 2 planes into a single plane.
		
			cv::dft(_fbuffer, _fbuffer);

			// swap quadrants to locate lower freq to center.
			int c2 = _fbuffer.cols / 2;
			int r2 = _fbuffer.rows / 2;
			cv::Mat q1, q2,  q3, q4, qt;
			q1 = _fbuffer(cv::Rect( 0,  0, c2, r2));
			q2 = _fbuffer(cv::Rect(c2, r2, c2, r2));
			q3 = _fbuffer(cv::Rect(c2,  0, c2, r2));
			q4 = _fbuffer(cv::Rect( 0, r2, c2, r2));
			q1.copyTo(qt);
			q2.copyTo(q1);
			qt.copyTo(q2);
			q3.copyTo(qt);
			q4.copyTo(q3);
			qt.copyTo(q4);

			// Cut lower freq elements.
			if ( _lastEllipse != in2 ) {
				_mask = cv::Scalar(255);
				int radX = static_cast<int>(c2 * in2);
				int radY = static_cast<int>(r2 * in2);
				cv::ellipse(_mask, cv::Point(c2, r2), cv::Size(radX, radY), 0, 0, 360, cv::Scalar(0), -1);
				_lastEllipse = in2;
			}
			_fbuffer.setTo(cv::Scalar::all(0), _mask);
			//cv::imwrite("mask.png", _mask);

	#if 1
			// Re-swap quadrants.
			q1 = _fbuffer(cv::Rect( 0,  0, c2, r2));
			q2 = _fbuffer(cv::Rect(c2, r2, c2, r2));
			q3 = _fbuffer(cv::Rect(c2,  0, c2, r2));
			q4 = _fbuffer(cv::Rect( 0, r2, c2, r2));
			q1.copyTo(qt);
			q2.copyTo(q1);
			qt.copyTo(q2);
			q3.copyTo(qt);
			q4.copyTo(q3);
			qt.copyTo(q4);
	#endif

			cv::idft(_fbuffer, _fbuffer);

			cv::split(_fbuffer, _planes); // de-interleave

			cv::Mat roi = _planes[0](cv::Rect(0, 0, I.cols, I.rows));

			double minVal, maxVal;
			cv::minMaxLoc(roi, &minVal, &maxVal);
			if ( maxVal < 255.0 ) maxVal = 255.0;

			cv::Mat J(out);
			roi.convertTo(J, J.type(), 255.0 / maxVal);
		}
	};
	static DFT _dft;

}//end of local namespace

PicturePerfect::ProgramIterator Sakura::IF::operator()(uint32_t index, PicturePerfect::ProgramIterator program, RegisterMap& regs)
{
	cv::Mat& out = regs[*program++]->at<cv::Mat>();
	PicturePerfect::DataSlot* in1;
	PicturePerfect::DataSlot* in2;
	PicturePerfect::DataSlot* in3;
	PicturePerfect::DataSlot* in4;
	//double    vtemp;
	//cv::Mat* itemp;
	//std::cout << " IF" << index;

	switch ( index ) {
	// I1B_Zero_I1B
	case 0 :
		out = cv::Mat::zeros(out.size(), CV_8UC1);
		break;
	// I3B_Zero_I3B
	case 1 :
		out = cv::Mat::zeros(out.size(), CV_8UC3);
		break;
	// I1B_Move_I1B
	case 2 :
	// I3B_Move_I3B
	case 3 :
		in1 = regs[*program++];
		in1->at<cv::Mat>().copyTo(out);
		break;
	// I1B_Split1st_I3B
	case 4 :
		assert(out.channels() == 1);
		in1 = regs[*program++];
		{
			int fromTo[2] = { 0, 0 };
			cv::mixChannels(&in1->at<cv::Mat>(), 1, &out, 1, fromTo, 1);
		}
		break;
	// I1B_Split2nd_I3B
	case 5 :
		assert(out.channels() == 1);
		in1 = regs[*program++];
		{
			int fromTo[2] = { 1, 0 };
			cv::mixChannels(&in1->at<cv::Mat>(), 1, &out, 1, fromTo, 1);
		}
		break;
	// I1B_Split3rd_I3B
	case 6 :
		assert(out.channels() == 1);
		in1 = regs[*program++];
		{
			int fromTo[2] = { 2, 0 };
			cv::mixChannels(&in1->at<cv::Mat>(), 1, &out, 1, fromTo, 1);
		}
		break;
	// I3B_BGR2HSV_I3B
	case 7 :
		in1 = regs[*program++];
		cv::cvtColor(in1->at<cv::Mat>(), out, CV_BGR2HSV);
		break;
	// I3B_BGR2YUV_I3B
	case 8 :
		in1 = regs[*program++];
        cv::cvtColor(in1->at<cv::Mat>(), out, CV_BGR2YCrCb);
		break;
	// I1B_BGR2Gray_I3B
	case 9 :
		in1 = regs[*program++];
        cv::cvtColor(in1->at<cv::Mat>(), out, CV_BGR2GRAY);
		break;
	// I1B_Dilate3x3_I1B
	case 10 :
		in1 = regs[*program++];
		cv::dilate(in1->at<cv::Mat>(), out, cv::Mat());
		break;
	// I1B_Erode3x3_I1B
	case 11 :
		in1 = regs[*program++];
		cv::erode(in1->at<cv::Mat>(), out, cv::Mat());
		break;
	// I1B_Thin_I1B
	case 12 :
		in1 = regs[*program++];
		_thin(out, in1->at<cv::Mat>());
		break;
	// I1B_Laplacian3x3_I1B
	case 13 :
		in1 = regs[*program++];
        cv::filter2D(in1->at<cv::Mat>(), out, -1, _kernel_Laplacian3x3);
		break;
	// I1B_Laplacian2nd3x3_I1B
	case 14 :
		in1 = regs[*program++];
		cv::filter2D(in1->at<cv::Mat>(), out, -1, _kernel_Laplacian2nd3x3);
		break;
	// I1B_Median3x3_I1B
	case 15 :
		in1 = regs[*program++];
		cv::medianBlur(in1->at<cv::Mat>(), out, 3);
		break;
	// I1B_Sharpen3x3_I1B
	case 16 :
		in1 = regs[*program++];
        cv::filter2D(in1->at<cv::Mat>(), out, -1, _kernel_Sharpen3x3);
		break;
	// I1B_Smooth3x3_I1B
	case 17 :
		in1 = regs[*program++];
		cv::boxFilter(in1->at<cv::Mat>(), out, -1, cv::Size(3, 3));
		break;
	// I1B_Sobel3x3_I1B
	case 18 :
		in1   = regs[*program++];
		{
			cv::Mat gradX, gradY, absGradX, absGradY, grad;

			Sobel(in1->at<cv::Mat>(), gradX, CV_16S, 1, 0, 3);
			convertScaleAbs(gradX, absGradX);

			Sobel(in1->at<cv::Mat>(), gradY, CV_16S, 0, 1, 3);
			convertScaleAbs(gradY, absGradY);

			/// Total Gradient (approximate)
			addWeighted(absGradX, 0.5, absGradY, 0.5, 0, grad);

			grad.convertTo(out, CV_8U);
		}
		break;
	// I1B_SobelX3x3_I1B
	case 19 :
		in1 = regs[*program++];
		cv::Sobel(in1->at<cv::Mat>(), out, -1, 0, 1, 3);
		break;
	// I1B_SobelY3x3_I1B
	case 20 :
		in1 = regs[*program++];
		cv::Sobel(in1->at<cv::Mat>(), out, -1, 1, 0, 3);
		break;
	// I1B_Negate_I1B
	case 21 :
		in1 = regs[*program++];
		cv::subtract(cv::Scalar(255), in1->at<cv::Mat>(), out);
		break;
	// I1B_ProjectionX_I1B
	case 22 :
		in1 = regs[*program++];
		{
			cv::Mat& I = in1->at<cv::Mat>();
			cv::Mat& J = out;

			int64_t* proj = new int64_t[I.cols];
			memset(proj, 0, I.cols * sizeof(int64_t));
			for (int y = 0; y < I.rows; y++) {
				uchar* data;
				data = I.ptr<uchar>(y);
				for (int x = 0; x < I.cols; x++) {
            		proj[x] += data[x];
				}
			}
			for (int x = 0; x < I.cols; x++) {
        		proj[x] /= I.rows;
				if ( proj[x] > 255 ) proj[x] = 255;
			}
			for (int y = 0; y < I.rows; y++) {
				uchar* data;
				data = J.ptr<uchar>(y);
				for (int x = 0; x < I.cols; x++) {
        			data[x] = static_cast<uchar>(proj[x]);
        		}
			}
			delete[] proj;
		}
		break;
	// I1B_ProjectionY_I1B
	case 23 :
		in1 = regs[*program++];
		{
			// Image
			cv::Mat& I = in1->at<cv::Mat>();
			cv::Mat& J = out;
        
			for (int y = 0; y < I.rows; y++) {
        		uint64_t proj = 0;
				uchar*   data;
				data = I.ptr<uchar>(y);
        		for (int x = 0; x < I.cols; x++) {
        			//proj += I.at<uchar>(y, x);
					proj += data[x];
        		}
        		proj /= I.rows;
				data =J.ptr<uchar>(y);
        		for (int x = 0; x < I.cols; x++) {
        			//J.at<uchar>(y, x) = proj > 255 ? 255 : static_cast<uchar>(proj);
					data[x] = proj > 255 ? 255 : static_cast<uchar>(proj);
        		}
			}
		}
		break;
	// I1B_Connection4_I1B
	case 24 :
		in1 = regs[*program++];
		{
			#define UPDATE_IF_INSIDE(X, Y) if ( (X) >=0 && (X) < I.cols && (Y) >= 0 && (Y) < I.rows ) flag = flag || (I.at<uchar>((Y), (X)) > 0 ? true : false)
			cv::Mat& I = in1->at<cv::Mat>();
			cv::Mat& J = out;
        
       		for (int y = 0; y < I.rows; y++) {
				for (int x = 0; x < I.cols; x++) {
            		bool flag = false;
					UPDATE_IF_INSIDE(x    , y - 1);
					UPDATE_IF_INSIDE(x - 1, y    );
					UPDATE_IF_INSIDE(x    , y    );
					UPDATE_IF_INSIDE(x + 1, y    );
					UPDATE_IF_INSIDE(x    , y + 1);
					J.at<uchar>(y, x) = flag ? 255 : 0;
				}
			}
			#undef UPDATE_IF_INSIDE
		}
		break;
	// I1B_Connection8_I1B
	case 25 :
		in1 = regs[*program++];
		{
			#define UPDATE_IF_INSIDE(X, Y) if ( (X) >=0 && (X) < I.cols && (Y) >= 0 && (Y) < I.rows ) flag = flag || (I.at<uchar>((Y), (X)) > 0 ? true : false)
			cv::Mat& I = in1->at<cv::Mat>();
			cv::Mat& J = out;
        
       		for (int y = 0; y < I.rows; y++) {
				for (int x = 0; x < I.cols; x++) {
            		bool flag = false;
					UPDATE_IF_INSIDE(x - 1, y - 1);
					UPDATE_IF_INSIDE(x    , y - 1);
					UPDATE_IF_INSIDE(x + 1, y - 1);
					UPDATE_IF_INSIDE(x - 1, y    );
					UPDATE_IF_INSIDE(x    , y    );
					UPDATE_IF_INSIDE(x + 1, y    );
					UPDATE_IF_INSIDE(x - 1, y + 1);
					UPDATE_IF_INSIDE(x    , y + 1);
					UPDATE_IF_INSIDE(x + 1, y + 1);
					J.at<uchar>(y, x) = flag ? 255 : 0;
				}
			}
			#undef UPDATE_IF_INSIDE
		}
		break;
	// I1B_Outline4_I1B
	case 26 :
		PICTUREPERFECT_THROW(NotImplementedException, "SakuraIF::I1B_Outline4_I1B");
		break;
	// I1B_Outline8_I1B
	case 27 :
		PICTUREPERFECT_THROW(NotImplementedException, "SakuraIF::I1B_Outline8_I1B");
		break;
	// I1B_Segment_I1B
	case 28 :
		PICTUREPERFECT_THROW(NotImplementedException, "SakuraIF::I1B_Segment_I1B");
		break;
	// I1B_Add_I1B_I1B
	case 29 :
		in1 = regs[*program++];
		in2 = regs[*program++];
		cv::add(in1->at<cv::Mat>(), in2->at<cv::Mat>(), out);
		break;
	// I1B_Diff_I1B_I1B
	case 30 :
		in1 = regs[*program++];
		in2 = regs[*program++];
		cv::absdiff(in1->at<cv::Mat>(), in2->at<cv::Mat>(), out);
		break;
	// I1B_AlgebraicProd_I1B_I1B
	case 31 :
		in1 = regs[*program++];
		in2 = regs[*program++];
        for (int y = 0; y < out.rows; y++) {
            unsigned char* p0 = (unsigned char*)out.ptr(y);
            unsigned char* p1 = (unsigned char*)in1->at<cv::Mat>().ptr(y);
            unsigned char* p2 = (unsigned char*)in2->at<cv::Mat>().ptr(y);
            for (int x = 0; x < out.cols; x++) {
                p0[x] = ((unsigned int)p1[x] * p2[x] + 1) / 255;
            }
        }
		break;
	// I1B_AlgebraicSum_I1B_I1B
	case 32 :
		in1 = regs[*program++];
		in2 = regs[*program++];
        for (int y = 0; y < out.rows; y++) {
            unsigned char* p0 = (unsigned char*)out.ptr(y);
            unsigned char* p1 = (unsigned char*)in1->at<cv::Mat>().ptr(y);
            unsigned char* p2 = (unsigned char*)in2->at<cv::Mat>().ptr(y);
            for (int x = 0; x < out.cols; x++) {
                p0[x] = p1[x] + p2[x] - (p1[x] * p2[x] + 1) / 255;
            }
        }
		break;
	// I1B_BoundedProd_I1B_I1B
	case 33 :
		in1 = regs[*program++];
		in2 = regs[*program++];
        for (int y = 0; y < out.rows; y++) {
            unsigned char* p0 = (unsigned char*)out.ptr(y);
            unsigned char* p1 = (unsigned char*)in1->at<cv::Mat>().ptr(y);
            unsigned char* p2 = (unsigned char*)in2->at<cv::Mat>().ptr(y);
            for (int x = 0; x < out.cols; x++) {
                int temp = p1[x] + p2[x];
                p0[x] = (unsigned char)(temp > 255 ? temp - 255 : 0);
            }
        }
		break;
	// I1B_LogicalProd_I1B_I1B
	case 34 :
		in1 = regs[*program++];
		in2 = regs[*program++];
        cv::min(in1->at<cv::Mat>(), in2->at<cv::Mat>(), out);
		break;
	// I1B_LogicalSum_I1B_I1B
	case 35 :
		in1 = regs[*program++];
		in2 = regs[*program++];
        cv::max(in1->at<cv::Mat>(), in2->at<cv::Mat>(), out);
		break;
	// I1B_Subtract_I1B_I1B
	case 36 :
		in1 = regs[*program++];
		in2 = regs[*program++];
		cv::subtract(in1->at<cv::Mat>(), in2->at<cv::Mat>(), out);
		break;
	// I1B_HighPass_I1B_V1D
	case 37 :
		in1 = regs[*program++];
		in2 = regs[*program++];
		_dft.executeHighPassFilter(out, in1->at<cv::Mat>(), in2->at<double>());
		break;
	// I1B_LowPass_I1B_V1D
	case 38 :
		in1 = regs[*program++];
		in2 = regs[*program++];
		_dft.executeLowPassFilter(out, in1->at<cv::Mat>(), in2->at<double>());
		break;
	// I1B_Threshold_I1B
	case 39 :
		in1 = regs[*program++];
		cv::threshold(in1->at<cv::Mat>(), out, 0, 255, CV_THRESH_TOZERO | CV_THRESH_OTSU);
		break;
	// I1B_ThresholdInv_I1B
	case 40 :
		in1 = regs[*program++];
		cv::threshold(in1->at<cv::Mat>(), out, 0, 255, CV_THRESH_TOZERO_INV | CV_THRESH_OTSU);
		break;
	// I1B_ThresholdV_I1B_V1D
	case 41 :
		in1 = regs[*program++];
		in2 = regs[*program++];
		cv::threshold(in1->at<cv::Mat>(), out, in2->at<double>() * 255, 255, CV_THRESH_TOZERO);
		break;
	// I1B_ThresholdInvV_I1B_V1D
	case 42 :
		in1 = regs[*program++];
		in2 = regs[*program++];
		cv::threshold(in1->at<cv::Mat>(), out, in2->at<double>() * 255, 255, CV_THRESH_TOZERO_INV);
		break;
	// I1B_Binarize_I1B
	case 43 :
		in1 = regs[*program++];
		cv::threshold(in1->at<cv::Mat>(), out, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
		break;
	// I1B_BinarizeInv_I1B
	case 44 :
		in1 = regs[*program++];
		cv::threshold(in1->at<cv::Mat>(), out, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
		break;
	// I1B_BinarizeV_I1B_V1D
	case 45 :
		in1 = regs[*program++];
		in2 = regs[*program++];
		cv::threshold(in1->at<cv::Mat>(), out, in2->at<double>() * 255, 255, CV_THRESH_BINARY);
		break;
	// I1B_BinarizeInvV_I1B_V1D
	case 46 :
		in1 = regs[*program++];
		in2 = regs[*program++];
		cv::threshold(in1->at<cv::Mat>(), out, in2->at<double>() * 255, 255, CV_THRESH_BINARY_INV);
		break;
	// I1B_BigBlob4_I1B_V1D
	case 47 :
		PICTUREPERFECT_THROW(NotImplementedException, "SakuraIF::I1B_BigBlob4_I1B_V1D");
		break;
	// I1B_BigBlob8_I1B_V1D
	case 48 :
		PICTUREPERFECT_THROW(NotImplementedException, "SakuraIF::I1B_BigBlob8_I1B_V1D");
		break;
	// I1B_AddV_I1B_V1D
	case 49 :
		in1 = regs[*program++];
		in2 = regs[*program++];
		cv::add(in1->at<cv::Mat>(), cv::Scalar(in2->at<double>() * 255), out);
		break;
	// I1B_FillV_V1D
	case 50 :
		in1 = regs[*program++];
		out = cv::Scalar(in1->at<double>() * 255);
		break;
	// I1B_ModAvgV_I1B_V1D
	case 51 :
		in1 = regs[*program++];
		in2 = regs[*program++];
		{
			int avg, prop;
			avg  = (int)cv::mean(in1->at<cv::Mat>()).val[0];
			prop = (int)(in2->at<double>() * 255);

			if ( avg > prop )
			{
				cv::Scalar val;
				val.val[0] = (avg - prop);
				val.val[1] = (avg - prop);
				val.val[2] = (avg - prop);
				val.val[3] = (avg - prop);
				cv::subtract(in1->at<cv::Mat>(), val, out);
			}
			else
			{
				cv::Scalar val;
				val.val[0] = (prop - avg);
				val.val[1] = (prop - avg);
				val.val[2] = (prop - avg);
				val.val[3] = (prop - avg);
				cv::add(in1->at<cv::Mat>(), val, out);
			}
		}
		break;
	// I1B_MultiplyV_I1B_V1D
	case 52 :
		in1 = regs[*program++];
		in2 = regs[*program++];
		cv::scaleAdd(in1->at<cv::Mat>(), in2->at<double>() * 255, cv::Mat::zeros(out.rows, out.cols, out.type()), out);
		break;
	// I1B_SubtractV_I1B_V1D
	case 53 :
		in1 = regs[*program++];
		in2 = regs[*program++];
		cv::subtract(in1->at<cv::Mat>(), cv::Scalar(in2->at<double>() * 255), out);
		break;
	// I1B_NoiseV_V1D
	case 54 :
		PICTUREPERFECT_THROW(NotImplementedException, "SakuraIF::I1B_NoiseV_V1D");
		break;
	// I1B_Select_I1B_I1B_V1D_V1D
	case 55 :
		in1 = regs[*program++];
		in2 = regs[*program++];
		in3 = regs[*program++];
		in4 = regs[*program++];
		if ( in3->at<double>() >= in4->at<double>() ) {
			in1->at<cv::Mat>().copyTo(out);
		}
		else {
			in2->at<cv::Mat>().copyTo(out);
		}
		break;
	// I1B_Mean3x3_I1B
	case 56 :
		PICTUREPERFECT_THROW(NotImplementedException, "SakuraIF::I1B_Mean3x3_I1B");
		break;
	// I1B_LightEdge3x3_I1B
	case 57 :
		in1 = regs[*program++];
		cv::filter2D(in1->at<cv::Mat>(), out, -1, _kernel_LightDarkEdge3x3);
		break;
	// I1B_DarkEdge3x3_I1B
	case 58 :
		in1 = regs[*program++];
		cv::filter2D(in1->at<cv::Mat>(), out, -1, _kernel_LightDarkEdge3x3, cv::Point(-1, -1), 255);
		break;
	// I1B_LargeArea_I1B
	case 59 :
		in1 = regs[*program++];
		out = in1->at<cv::Mat>();
		break;
	// I1B_SmallArea_I1B
	case 60 :
		in1 = regs[*program++];
		out = in1->at<cv::Mat>();
		break;
	// I1B_Variance3x3_I1B
	case 61 :
		in1 = regs[*program++];
		out = in1->at<cv::Mat>();
		break;
	// I1B_Gamma_I1B_V1D
	case 62 :
		in1 = regs[*program++];
		in2 = regs[*program++];
		{
			double x = (in2->at<double>() - 0.5) * 0.3;
			double gamma = x >= 0.0 ? x + 1.0 : x - 1.0;
			cv::Mat temp;
			in1->at<cv::Mat>().convertTo(temp, CV_32F, 1.0 / 255);
			cv::pow(temp, gamma, temp);
			temp.convertTo(out, CV_8U, 255.0);
		}
		break;
	// I1B_DrasticSum_I1B_I1B
	case 63 :
		in1 = regs[*program++];
		in2 = regs[*program++];
        for (int y = 0; y < out.rows; y++) {
            unsigned char* p0 = (unsigned char*)out.ptr(y);
            unsigned char* p1 = (unsigned char*)in1->at<cv::Mat>().ptr(y);
            unsigned char* p2 = (unsigned char*)in2->at<cv::Mat>().ptr(y);
            for (int x = 0; x < out.cols; x++) {
                p0[x] = (unsigned char)(p1[x] == 0 ? p2[x] : p2[x] == 0 ? p1[x] : 1);
            }
        }
		break;
	// I1B_DrasticProd_I1B_I1B
	case 64 :
		in1 = regs[*program++];
		in2 = regs[*program++];
        for (int y = 0; y < out.rows; y++) {
            unsigned char* p0 = (unsigned char*)out.ptr(y);
            unsigned char* p1 = (unsigned char*)in1->at<cv::Mat>().ptr(y);
            unsigned char* p2 = (unsigned char*)in2->at<cv::Mat>().ptr(y);
            for (int x = 0; x < out.cols; x++) {
                p0[x] = (unsigned char)(p1[x] == 255 ? p2[x] : p2[x] == 255 ? p1[x] : 0);
            }
        }
		break;
	// I1B_Prewitt3x3_I1B
	case 65 :
		in1   = regs[*program++];
		{
			cv::Mat orig, gradX, gradY, absGradX, absGradY, grad;

			// We need value range between [-255, 255] so we use CV_16S.
			in1->at<cv::Mat>().convertTo(orig, CV_16S);

			cv::filter2D(orig, gradX, -1, _kernel_PrewittX3x3);
			convertScaleAbs(gradX, absGradX);

			cv::filter2D(orig, gradY, -1, _kernel_PrewittY3x3);
			convertScaleAbs(gradY, absGradY);

			/// Total Gradient (approximate)
			cv::addWeighted(absGradX, 0.5, absGradY, 0.5, 0, grad);

			grad.convertTo(out, CV_8U);
		}
		break;
	}

	return program;
}
