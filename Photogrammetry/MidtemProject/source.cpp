// 
// CIVILEN 6451 Photogrammetry
// For Midterm Project
// Name: Bing Zha
// Email: zha.44@osu.edu
// The Ohio State University
//
// Process:
// 1. Superpixel algorithm to segment image
// 2. Dichromatic Reflectance Model
// 3. Least Square estimation (Gauss-Seidel iterations)
//

#include <iostream>
#include <ctime>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc/slic.hpp>


int main(int argc, char** argv)
{
	int a = 1;

	cv::Mat image = cv::imread("demo.jpg", 1);
	cv::Mat mask;
	cv::Mat labels;

	// create a static class object 
	cv::Ptr<cv::ximgproc::SuperpixelSLIC> slic = cv::ximgproc::createSuperpixelSLIC(image, cv::ximgproc::SLIC, 10, 10.0f);

	slic->iterate(10);
	slic->enforceLabelConnectivity();
	slic->getLabelContourMask(mask);

	slic->getLabels(labels);

	// get label of every pixel
	int nr = labels.rows;
	int nc = labels.cols * labels.channels();

	for (int i = 0; i < nr; i++)
	{
		int* data = labels.ptr<int>(i);
		for (int j = 0; j < nc; j++)
		{
			std::cout << data[j] << std::endl;
		}
	}



	int number = slic->getNumberOfSuperpixels();
	std::cout << "The number of superpixels is: " << number << std::endl;

	image.setTo(cv::Scalar(255, 255, 255), mask);

	cv::namedWindow("SLIC", cv::WINDOW_NORMAL);
	cv::imshow("SLIC", image);

	cv::waitKey(0);

	return 0;
}