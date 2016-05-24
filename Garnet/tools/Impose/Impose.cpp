
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

void showUsage();

int main(int argc, const char* argv[])
{
	std::string srcfilename;
	std::string impfilename;
	std::string dstfilename;
	
	int mode = 0;
	for (auto i = 1u; i < argc; i++) {
		if ( argv[i][0] == '-' ) {
		}
		else {
			switch ( mode ) {
			case 0 :
				srcfilename = argv[i];
				mode++;
				break;
			case 1 :
				impfilename = argv[i];
				mode++;
				break;
			case 2 :
				dstfilename = argv[i];
				mode++;
				break;
			}
		}
	}
	if ( srcfilename.empty() || impfilename.empty() || dstfilename.empty() ) {
		showUsage();
		return 0;
	}
	
	cv::Mat src, imp, dst;
	
	src = cv::imread(srcfilename);
	imp = cv::imread(impfilename);
	dst = cv::Mat(src);
	
	cv::add(src, imp, dst);
	
	cv::imwrite(dstfilename, dst);
	
	return 0;
}

void showUsage()
{
	std::cout << "\n[usage]";
	std::cout << "\n  Impose <src> <impose> <dst>";
	std::cout << std::endl;
}

