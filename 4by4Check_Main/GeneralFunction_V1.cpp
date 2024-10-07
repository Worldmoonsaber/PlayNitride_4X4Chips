
#include "Check4by4Chip_lib.h"
#pragma once
#pragma region General_function


Point find_piccenter(Mat src) {
	int piccenterx = int(src.size().width * 0.5);
	int piccentery = int(src.size().height * 0.5);
	Point piccenter = Point(piccenterx, piccentery);
	return piccenter;
}

Mat CropIMG(Mat img, Rect size)
{
	Mat croppedIMG;
	img(size).copyTo(croppedIMG);
	return croppedIMG;

}


Mat KmeanOP(int knum, Mat src)
{
	TermCriteria criteria;
	Mat labels, centeres;
	Mat pixelVal, segmentedIMG;

	//define stopping criteria
	criteria = TermCriteria(cv::TermCriteria::EPS + TermCriteria::MAX_ITER, 100, 0.2);
	int attempts = 10;
	int KmeanFlag = cv::KMEANS_RANDOM_CENTERS;



	Mat samples(src.rows * src.cols, src.channels(), CV_32F); //change to float

	Mat clonesample;


	for (int y = 0; y < src.rows; y++)
	{
		for (int x = 0; x < src.cols; x++)
		{
			for (int z = 0; z < src.channels(); z++)
			{
				if (src.channels() == 3)
				{
					samples.at<float>(y + x * src.rows, z) = src.at<Vec3b>(y, x)[z];
				}

				else
				{
					samples.at<float>(y + x * src.rows, z) = src.at<uchar>(y, x); //for gray img
				}

			}
		}
	}



	cv::kmeans(samples, knum, labels, criteria, attempts, KmeanFlag, centeres);

	Mat NewIMG(src.size(), src.type()); //CV8U3

	for (int y = 0; y < src.rows; y++)
	{
		for (int x = 0; x < src.cols; x++)
		{
			int cluster_idx = labels.at<int>(y + x * src.rows, 0);
			if (src.channels() == 3)
			{
				for (int z = 0; z < src.channels(); z++) //3 channels
				{
					NewIMG.at<Vec3b>(y, x)[z] = (centeres.at<float>(cluster_idx, z)); //CV32F


				}
			}
			else
			{
				NewIMG.at<uchar>(y, x) = centeres.at<float>(cluster_idx, 0); //for gray img
			}


		}
	}



	//std::cout << "finish Kop" << endl;

	return NewIMG;
}

# pragma endregion




