
#include <string>
#include <iostream>

#include <opencv2/opencv.hpp>

#include <RandomGenerator.h>

RandomSFMT19937 randgen;

void  gen_random(double ratio, const std::string& filename)
{
    int64_t count = 0;
	cv::Mat img = cv::Mat::zeros(256, 256, CV_8UC3);
	for (int y = 0; y < 256; y++) {
		for (int x = 0; x < 256; x++) {
			if ( randgen.generate() >= ratio ) {
                // Red
				img.at<cv::Vec3b>(y, x)[0] = 0;
				img.at<cv::Vec3b>(y, x)[1] = 0;
				img.at<cv::Vec3b>(y, x)[2] = 255;
			}
			else {
                // Blue
				img.at<cv::Vec3b>(y, x)[0] = 255;
				img.at<cv::Vec3b>(y, x)[1] = 0;
				img.at<cv::Vec3b>(y, x)[2] = 0;
                count++;
			}
		}
	}
    cv::imwrite(filename, img);
    std::cout << filename << "," << count << "," << (double)count/(256*256) << std::endl;
}

void  gen_random_binary(double ratio, const std::string& filename)
{
    int64_t count = 0;
	cv::Mat img = cv::Mat::zeros(128, 128, CV_8UC3);
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			if ( randgen.generate() >= ratio ) {
                // Black
				img.at<cv::Vec3b>(y, x)[0] = 0;
				img.at<cv::Vec3b>(y, x)[1] = 0;
				img.at<cv::Vec3b>(y, x)[2] = 0;
			}
			else {
                // White
				img.at<cv::Vec3b>(y, x)[0] = 255;
				img.at<cv::Vec3b>(y, x)[1] = 255;
				img.at<cv::Vec3b>(y, x)[2] = 255;
                count++;
			}
		}
	}
    cv::imwrite(filename, img);
    std::cout << filename << "," << count << "," << (double)count/(img.cols*img.rows) << std::endl;
}

int main()
{
    randgen.initialize(12345678u);

    gen_random_binary(160 / 255.0, "Binarize_Answer_1.png");
    gen_random_binary(144 / 255.0, "Binarize_Answer_2.png");
    gen_random_binary(192 / 255.0, "Binarize_Answer_3.png");
    gen_random_binary( 68 / 255.0, "Binarize_Answer_4.png");
    gen_random_binary(208 / 255.0, "Binarize_Answer_5.png");
    gen_random_binary(168 / 255.0, "Binarize_Answer_6.png");
    gen_random_binary(192 / 255.0, "Binarize_Answer_7.png");
    gen_random_binary(208 / 255.0, "Binarize_Answer_8.png");
    gen_random_binary( 96 / 255.0, "Binarize_Answer_9.png");

    /*
    gen_random(0.0, "RedBlue_Random0.0a.png");
    gen_random(0.0, "RedBlue_Random0.0b.png");
    gen_random(0.0, "RedBlue_Random0.0c.png");
    gen_random(0.2, "RedBlue_Random0.2a.png");
    gen_random(0.2, "RedBlue_Random0.2b.png");
    gen_random(0.2, "RedBlue_Random0.2c.png");
    gen_random(0.4, "RedBlue_Random0.4a.png");
    gen_random(0.4, "RedBlue_Random0.4b.png");
    gen_random(0.4, "RedBlue_Random0.4c.png");
    gen_random(0.6, "RedBlue_Random0.6a.png");
    gen_random(0.6, "RedBlue_Random0.6b.png");
    gen_random(0.6, "RedBlue_Random0.6c.png");
    gen_random(0.8, "RedBlue_Random0.8a.png");
    gen_random(0.8, "RedBlue_Random0.8b.png");
    gen_random(0.8, "RedBlue_Random0.8c.png");
    gen_random(1.0, "RedBlue_Random1.0a.png");
    gen_random(1.0, "RedBlue_Random1.0b.png");
    gen_random(1.0, "RedBlue_Random1.0c.png");
    */

    return 0;
}
