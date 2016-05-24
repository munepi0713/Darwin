
#include <string>
#include <iostream>

#include <opencv2/opencv.hpp>

int main(int argc, const char* argv[])
{
    cv::Mat img;

    // load image
    std::string infilename(argv[1]);
    img = cv::imread(infilename, 1); // read as color image

    // put cross mark
    int x = img.cols * strtod(argv[2], 0);
    int y = img.rows * strtod(argv[3], 0);
    cv::line(img, cv::Point(x, y - 8), cv::Point(x, y + 8), cv::Scalar(255), 3);
    cv::line(img, cv::Point(x - 8, y), cv::Point(x + 8, y), cv::Scalar(255), 3);

    // put diagonal cross.
    if ( argc > 5 ) {
        int cx = img.cols * strtod(argv[5], 0);
        int cy = img.rows * strtod(argv[6], 0);
        cv::line(img, cv::Point(cx - 8, cy - 8), cv::Point(cx + 8, cy + 8), cv::Scalar(255), 3);
        cv::line(img, cv::Point(cx - 8, cy + 8), cv::Point(cx + 8, cy - 8), cv::Scalar(255), 3);
    }

    // save image
    std::string outfilename;
    if ( argc > 4 ) {
        outfilename = argv[4];
    }
    else {
        std::string::size_type pos = infilename.find_last_of('.');
        outfilename = infilename.substr(0, pos) + "_out.png";
    }
    cv::imwrite(outfilename, img);

    return 0;
}
