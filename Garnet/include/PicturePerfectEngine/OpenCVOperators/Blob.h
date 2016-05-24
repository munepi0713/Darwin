#include <cassert>

#include <Labeling.h>

namespace PicturePerfectOpenCVOperators
{
class BigBlob4 : public Operator
{
private :
    short*     _label;
    LabelingBS _labeling;
public :
    BigBlob4() {
        _label = 0;
    }
    virtual ~BigBlob4() {
        delete[] _label;
    }
	virtual ResultType  process(const strings& tokens, PacketStore& packets)
	{
		assertNumberOfArgs(tokens, 4);

		// 
		Packet& dst  = packets[tokens[1]];
		Packet& src1 = packets[tokens[2]];
		Packet& src2 = packets[tokens[3]];

        // Image
#if 1
        if ( _label == 0 ) {
            _label = new short[src1.image->width * src1.image->height];
        }
#else
        short* label = new short[src1.image->width * src1.image->height];
#endif
        assert(src1.image->width == src1.image->widthStep);

        int th = static_cast<int>(src2.value * src1.image->width * src1.image->height);
        //printf("%s threshold = %d\n", __FUNCTION__, th);
            
#if 1
        _labeling.Exec((uint8_t*)src1.image->imageData, _label, src1.image->width, src1.image->height, true, th);
            
        cv::Mat I(src1.image);
        cv::Mat J(dst.image);
        for (int y = 0; y < I.rows; y++) {
            uchar* jdata = J.ptr<uchar>(y);
            uchar* idata = I.ptr<uchar>(y);
	        for (int x = 0; x < I.cols; x++) {
	            jdata[x] = _label[x + y * I.cols] == 0 ? 0 : idata[x];
	        }
	    }
#else
        LabelingBS labeling;
        labeling.Exec((uint8_t*)src1.image->imageData, label, src1.image->width, src1.image->height, true, th);
            
        cv::Mat I(src1.image);
        cv::Mat J(dst.image);
        for (int y = 0; y < I.rows; y++) {
	        for (int x = 0; x < I.cols; x++) {
	            J.at<uchar>(y, x) = label[x + y * I.cols] == 0 ? 0 : I.at<uchar>(y, x);
	        }
	    }
        delete[] label;
#endif
            
		// Value
		dst.value = src1.value;

		return RES_NEXT;
	}
};

class BigBlob8 : public Operator
{
private :
    short*     _label;
    LabelingBS _labeling;
public :
    BigBlob8() {
        _label = 0;
    }
    virtual ~BigBlob8() {
        delete[] _label;
    }
	virtual ResultType  process(const strings& tokens, PacketStore& packets)
	{
		assertNumberOfArgs(tokens, 4);

		// 
		Packet& dst  = packets[tokens[1]];
		Packet& src1 = packets[tokens[2]];
		Packet& src2 = packets[tokens[3]];

        // Image
        if ( _label == 0 ) {
            _label = new short[src1.image->width * src1.image->height];
        }
            
        _labeling.Exec((uint8_t*)src1.image->imageData, _label, src1.image->width, src1.image->height, true, static_cast<int>(src2.value * src1.image->width * src1.image->height));
            
        cv::Mat I(src1.image);
        cv::Mat J(dst.image);
        for (int y = 0; y < I.rows; y++) {
	        for (int x = 0; x < I.cols; x++) {
	            J.at<uchar>(y, x) = _label[x + y * I.cols] == 0 ? 0 : I.at<uchar>(y, x);
	        }
	    }
            
		// Value
		dst.value = src1.value;

		return RES_NEXT;
	}
};

}
