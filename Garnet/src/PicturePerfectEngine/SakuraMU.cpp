
#include <boost/format.hpp>

#include <Common/MD5.h>
#include <Common/Text.h>
#include <Common/PrintRange.h>

#include <PicturePerfectEngine/Vector.h>
#include <PicturePerfectEngine/SakuraExecutionUnits.h>

PicturePerfect::ProgramIterator Sakura::MU::operator()(uint32_t index, PicturePerfect::ProgramIterator program, RegisterMap& regs)
{
	const PicturePerfect::DataSlot* in1;

	switch ( index ) {
	// End
	case 0 :
		PICTUREPERFECT_THROW(NotImplementedException, "SakuraIF::I1B_BigBlob4_I1B_V1D");
		break;
	// Start
	case 1 :
		break;
	// PrintHash_V1D
	case 2 :
		in1 = regs[*program++];
		std::cout << inline_sprintf("%16" PRIx64 "\n", *((uint64_t*)&in1->at<double>()));
		break;
	// PrintHash_I1B
	case 3 :
	// PrintHash_I3B
	case 4 :
		in1 = regs[*program++];
		{
			MD5 md5;
			md5.write((unsigned char*)in1->at<cv::Mat>().data, in1->at<cv::Mat>().step * in1->at<cv::Mat>().rows);
			md5.finalize();
			std::cout << md5.get() << "\n";
		}
		break;
	// PrintHash_A1D
	case 5 :
		in1 = regs[*program++];
		{
			const PicturePerfect::Vector<double>& data = in1->at<PicturePerfect::Vector<double> >();
			MD5 md5;
			md5.write((unsigned char*)&*data.begin(), data.size() * sizeof(double));
			md5.finalize();
			std::cout << md5.get() << "\n";
		}
		break;
	// PrintValue_V1D
	case 6 :
		in1 = regs[*program++];
		std::cout << boost::format("%f\n") % in1->at<double>();
		break;
	// PrintValue_S
	case 7 :
		in1 = regs[*program++];
		std::cout << Text::toMultiByte(in1->at<std::wstring>()) << "\n";
		break;
	// PrintValue_A1D
	case 8 :
		in1 = regs[*program++];
		std::cout << print_range(in1->at<PicturePerfect::Vector<double> >().begin(), in1->at<PicturePerfect::Vector<double> >().end(), ", ") << "\n";
		break;
	}

	return program;
}
