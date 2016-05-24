
#include <Common/InlineSprintf.h>

namespace PicturePerfectOpenCVOperators
{

// Reinhard05. Global TMO.
// COMMAND
//	 pfstmo_reinhard05 [--brightness <val>] [--chromatic <val>] [--light <val>] [--verbose] [--help]
// OPTIONS
//   --brightness <val>, -b <val>
//           Brightness correction (-8..8), parameter 'f' in paper.  Default value: 0.0
//   --chromatic <val>, -c <val>
//           Amount of chromatic adaptation (von Kries model) (0..1), parameter 'c' in paper.  Default value: 0.0
//   --light <val>, -l <val>
//           Amount of light adaptation adaptation (0..1), parameter 'l' in paper.  Default value: 1.0
class TmoReinhard05 : public Operator
{
    public :
		virtual ResultType  process(const strings& tokens, PacketStore& packets)
		{
			static unsigned int _counter = 0;
			assertNumberOfArgs(tokens, 3);

			// 
			Packet& dst  = packets[tokens[1]]; // Output Image
			Packet& src1 = packets[tokens[2]]; // Source Image
			Packet& src2 = packets[tokens[3]]; // f
			Packet& src3 = packets[tokens[4]]; // c
			Packet& src4 = packets[tokens[5]]; // l

			//
			std::string randomName1 = inline_sprintf("TmoReinhard05_%08u.png", _counter++);
			std::string randomName2 = inline_sprintf("TmoReinhard05_%08u.png", _counter++);
			cvSaveImage(randomName1.c_str(), src1.image);

			//
			char buf[256];
			sprintf(buf, "pfsin %s | pfstmo_reinhard05 --brightness %f --chromatic %f --light %f | pfsout %s",
					randomName1.c_str(),
					src2.value,
					src3.value,
					src4.value,
					randomName2.c_str());
			system(buf);

			// Image
			IplImage* temp;
			temp = cvLoadImage(randomName2.c_str(), CV_LOAD_IMAGE_UNCHANGED);
			cvCopyImage(temp, dst.image);
			cvReleaseImage(&temp);

			// Value
			dst.value = src1.value;

			return RES_NEXT;
		}
};

}
