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
// Result:
// The number of superpixels is 2537 

#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc/slic.hpp>

#include <Eigen/Dense>


struct superpixel
{
	superpixel()
	{
		rgb[0] = rgb[1] = rgb[2] = 0;
		label_number = 0;
		index.clear();
	}

	int rgb[3];

	std::vector<int> index;

	int label_number;
};

struct Index
{
	int label;

	int pixel_index;
};


int main(int argc, char** argv)
{
	cv::Mat image = cv::imread("demo.jpg", 1);
	cv::resize(image, image, cv::Size(), 0.5, 0.5);

	int width = image.cols;
	int height = image.rows;


	cv::Mat mask;
	cv::Mat labels;

	// create a static class object 
	cv::Ptr<cv::ximgproc::SuperpixelSLIC> slic = cv::ximgproc::createSuperpixelSLIC(image, cv::ximgproc::SLIC, 60, 60.0f);

	slic->iterate(10);
	slic->enforceLabelConnectivity();
	slic->getLabelContourMask(mask);

	slic->getLabels(labels);


	// get label of every pixel
	std::vector<std::vector<int> > pixel_arrays;
	std::vector<superpixel> superpixels;
	std::vector<Index> indeces;
	std::vector<int> labels_index;


	int nr = labels.rows;
	int nc = labels.cols * labels.channels();

	for (int i = 0; i < nr; i++)
	{
		int* data = labels.ptr<int>(i);

		for (int j = 0; j < nc; j++)
		{
			//superpixel sp;
			Index id;

			//std::cout << data[j] << std::endl;
			labels_index.push_back(data[j]);

			int r = image.at<cv::Vec3b>(i, j)[0];
			int g = image.at<cv::Vec3b>(i, j)[1];
			int b = image.at<cv::Vec3b>(i, j)[2];
			//std::cout << "The R G B are: " << r << " " << r << " " << b << std::endl;
			std::vector<int> one_pixel;
			one_pixel.push_back(r);
			one_pixel.push_back(g);
			one_pixel.push_back(b);
			pixel_arrays.push_back(one_pixel);


			//sp.label_number = data[j];
			//int index = i*image.rows + j;
			//sp.index.push_back(index);
			//superpixels.push_back(sp);

			id.label = data[j];
			id.pixel_index = i*image.rows + j;

			indeces.push_back(id);

		}
	}

	// output labels_index
	std::ofstream out;
	out.open("labels.txt");

	for (int i = 0; i < labels_index.size(); i++)
	{
		out << labels_index[i] << std::endl;
	}
	out.close();



	int number = slic->getNumberOfSuperpixels();
	std::cout << "The number of superpixels is: " << number << std::endl;

	image.setTo(cv::Scalar(255, 255, 255), mask);

	cv::namedWindow("SLIC", cv::WINDOW_NORMAL);
	cv::imshow("SLIC", image);

	cv::waitKey(0);



	// Get superpixel value
	for (int i = 0; i < number; i++)
	{
		superpixel sp;
		sp.label_number = i;

		for (int j = 0; j < indeces.size(); j++)
		{
			Index id = indeces[j];

			if (id.label == i)
			{
				sp.index.push_back(id.pixel_index);
			}

		}

		superpixels.push_back(sp);
	}



	// the mean pixel value of every superpixel
	//
	for (int i = 0; i < superpixels.size(); i++)
	{
		//superpixel sp = superpixels[i];
		
		double r = 0.0;
		double g = 0.0;
		double b = 0.0; 

		for (int j = 0; j < superpixels[i].index.size(); j++)
		{
			r += pixel_arrays[superpixels[i].index[j]][0];
			g += pixel_arrays[superpixels[i].index[j]][1];
			b += pixel_arrays[superpixels[i].index[j]][2];

		}

		superpixels[i].rgb[0] = r / superpixels[i].index.size();
		superpixels[i].rgb[1] = g / superpixels[i].index.size();
		superpixels[i].rgb[2] = b / superpixels[i].index.size();

	}




	// Iteration to solve equation 


	// Observations 
	Eigen::Matrix<double, Eigen::Dynamic, 3> Cl(256*256, 3);


	for (int i = 0; i < pixel_arrays.size(); i++)
	{
		Cl.row(i) << pixel_arrays[i][0], pixel_arrays[i][1], pixel_arrays[i][2];

		//std::cout << Cl.row(i) << std::endl;
	}



	// Unknowns
	Eigen::Matrix<double, Eigen::Dynamic, 3> Cd(256*256, 3);
	//Eigen::Matrix<double, Eigen::Dynamic, 1> Md(256*256, 1);
	std::vector<double> Md;
	Md.resize(256 * 256);
	Eigen::Matrix<double, 1, 3> Cs;
	//Eigen::Matrix<double, Eigen::Dynamic, 1> Ms(256 * 256, 1);
	std::vector<double> Ms;
	Ms.resize(256 * 256);


	// Intial Values
	Eigen::Matrix<double, Eigen::Dynamic, 3> Cd0(256*256, 3);
	//Eigen::Matrix<double, Eigen::Dynamic, 1> Md0(256*256, 1);
	std::vector<double> Md0;
	Md0.resize(256 * 256);
	Eigen::Matrix<double, 1, 3> Cs0;
	//Eigen::Matrix<double, Eigen::Dynamic, 1> Ms0(256*256, 1);
	std::vector<double> Ms0;
	Ms0.resize(256 * 256);



	for (int i = 0; i < height*width; i++)
	{
		
		Cd0.row(i) << superpixels[labels_index[i]].rgb[0],
			superpixels[labels_index[i]].rgb[1],
			superpixels[labels_index[i]].rgb[2];

		//std::cout << Cd0.row(i) << std::endl;
	}


	for (int i = 0; i < 256*256; i++)
	{
		Md0[i] = 0.1;
		Ms0[i] = 0.1;
	}

	Cs0 << 50, 50, 50;




	int iteration = 50;
	for (int i = 0; i < iteration; i++)
	{

		for (int j = 0; j < height*width; j++)
		{
			Cd.row(j) = Md0[j] * (Cl.row(j) - Ms0[j] * Cs0.row(0)) / (Md0[j] * Md0[j]);

			Cs.row(0) = Ms0[j] * (Cl.row(j) - Md0[j] * Cd0.row(j)) / (Ms0[j] * Ms0[j]);


			Md[j] = Cd0.row(j).dot((Cl.row(j) - Ms0[j] * Cs0.row(0))) / Cd0.row(j).dot(Cd0.row(j));

			Ms[j] = Cs0.row(0).dot((Cl.row(j) - Md0[j] * Cd0.row(j))) / Cs0.row(0).dot(Cs0.row(0));

			//std::cout << Cd.row(j) << "\n"
			//	<< Cs.row(0) << "\n"
			//	<< Md[j] << "\n"
			//	<< Ms[j] << std::endl;
		}



		// x0 = x update x0 and iterate
		// Key: how to adjust Cd, because we need to make pixel value in superpixel equal
		for (int k = 0; k < Md.size(); k++)
		{
			Md0[k] = Md[k];
			Ms0[k] = Ms[k];

			//std::cout << Md0[k] << " " << Ms0[k] << std::endl;
		}

		Cs0 = Cs;


		// update superpixel value
		for (int p = 0; p < superpixels.size(); p++)
		{
			//superpixel sp = superpixels[i];

			double r = 0.0;
			double g = 0.0;
			double b = 0.0;

			for (int q = 0; q < superpixels[p].index.size(); q++)
			{
				r += Cd.row(superpixels[p].index[q])[0];
				g += Cd.row(superpixels[p].index[q])[1];
				b += Cd.row(superpixels[p].index[q])[2];

			}

			superpixels[p].rgb[0] = r / superpixels[p].index.size();
			superpixels[p].rgb[1] = g / superpixels[p].index.size();
			superpixels[p].rgb[2] = b / superpixels[p].index.size();
		}



		for (int r = 0; r < height*width; r++)
		{

			Cd0.row(r) << superpixels[labels_index[r]].rgb[0],
				superpixels[labels_index[r]].rgb[1],
				superpixels[labels_index[r]].rgb[2];

			//std::cout << Cd0.row(r) << std::endl;
			//std::cout << Cs0.row(0) << std::endl;

		}


		std::cout << "Iterate: " << i 
			<< "\n" << std::endl;

	}


	for (int i = 0; i < number; i++)
	{
		std::cout << Cd.row(i) << "\n"
			<< Cs.row(0) << "\n"
			<< Md[i] << "\n"
			<< Ms[i] << std::endl;
	}




	return 0;
}