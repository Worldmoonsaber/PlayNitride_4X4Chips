#include "Check4by4Chip_lib.h"



#pragma region STEP1_roughlysearch 

std::tuple<Point, int, Mat, vector<Point>> potentialchipSearch_V1(Mat cropedRImg, double resizeTDwidth, double resizeTDheight, sizeTD target, thresP thresParm, int flag, double distPX)
{
	Point potentialchip = Point(0, 0);

	Mat thresimg;
	funcCreateKmeanThresImg(thresParm, cropedRImg, thresimg);
	cropedRImg.release();
	vector<Point> TDCNT;
	vector<BlobInfo> vChips = RegionPartition(thresimg, resizeTDwidth * resizeTDheight * 1.4, resizeTDwidth * resizeTDheight * 0.5);
	Point2f piccenter = find_piccenter(thresimg);

	try
	{
		if (vChips.size() == 0)
		{
			flag = 1;
			return{ potentialchip,flag,thresimg,TDCNT };
		}
		else
		{
			std::sort(vChips.begin(), vChips.end(), [&, piccenter](BlobInfo& a, BlobInfo& b)
				{
					norm(a.Center() - piccenter);
					return norm(a.Center() - piccenter) < norm(b.Center() - piccenter);
				});

			if (vChips.size() <16) //4*4=16
			{
				flag = 1;
				return{ potentialchip,flag,thresimg,TDCNT };
			}
			else
			{
				TDCNT = vChips[0].contour();
				Point potentialchipFullfield = Point2i(vChips[0].Center().x*12, vChips[0].Center().y*12);
				cout << "check tdpt(full img) is : " << potentialchipFullfield << endl;

				if (norm(potentialchipFullfield - Point2i(thresimg.cols * 6, thresimg.rows * 6)) > distPX)
				{
					flag = 6;
					potentialchip = vChips[0].Center();
				}
				else
				{
					flag = 0;
					potentialchip = vChips[0].Center();
				}
			}
		}

	}
	catch (const char* message)
	{
		std::cout << message << std::endl;
	}

	vChips.clear();

	return{ potentialchip,flag,thresimg,TDCNT };
}

#pragma endregion STEP1_roughlysearch 



#pragma region STEP2_4by4check 

std::tuple< int, Mat, Mat> check4by4_V1(Mat src,Mat thresimg, int boolflag, Point Ref_chip_Pos, SettingP chipsetting, vector<Point> TDcnt)
{
	Point finechip = Point(0, 0);
	Point IMGoffset = Point(0, 0);
	Mat Grayimg, markimg;
	src.copyTo(markimg);

	/*start......*/
	double resizedpitchX = chipsetting.xpitch[0] / 12;
	double resizedpitchY = chipsetting.ypitch[0] / 12;

	Mat creteriaImg = Mat::zeros(thresimg.size(), CV_8UC1);
	Rect patternmodel = boundingRect(TDcnt);
	Mat bitAndImg;
	vector<vector<Point>> Cntchip;
	Point leftopPt = Ref_chip_Pos - Point(2 * resizedpitchX, 2 * resizedpitchY);

	
	try
	{

		if (leftopPt.x<0 || leftopPt.y < 0)
		{
			boolflag = 2;
			throw "something wrong::X-Y pitch issue";
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					Point rectcenter = leftopPt + Point(j * resizedpitchX, i * resizedpitchY);
					//cout << "check center is : " << rectcenter << endl;
					Rect pattendraw = Rect(Point(rectcenter.x - 0.5 * patternmodel.width, rectcenter.y - 0.5 * patternmodel.height),
						Point(rectcenter.x + 0.5 * patternmodel.width, rectcenter.y + 0.5 * patternmodel.height));
					cv::rectangle(creteriaImg, pattendraw, Scalar::all(255), -1);
				}
			}
			bitwise_and(creteriaImg, thresimg, bitAndImg);

			cv::findContours(bitAndImg, Cntchip, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point());

			std::cout << "check Cntchip size() is :: " << Cntchip.size() << endl;
		
			
			
			if (Cntchip.size() == 16)
			{
				boolflag = 9;
				for (int i = 0; i < Cntchip.size(); i++)
				{
					Rect bx=cv::boundingRect(Cntchip[i]);
					cv::circle(markimg, Point(bx.x + 0.5 * bx.width, bx.y + bx.height * 0.5), 3, Scalar(0, 0, 255), -1);
				}
				cv::circle(markimg, Ref_chip_Pos, 3, Scalar(255, 0, 0), -1);
				cv::resize(markimg, markimg, Size(500, 500));
				cv::resize(thresimg, Grayimg, Size(500, 500));
			}
			else
			{
				if (Cntchip.size() > 16)
				{
					boolflag = 2;
					
					cv::circle(markimg, Ref_chip_Pos, 3, Scalar(255, 0, 0), -1);
					cv::resize(markimg, markimg, Size(500, 500));
					cv::resize(thresimg, Grayimg, Size(500, 500));
					throw "please Tune pitch";
				}
				else
				{
					boolflag = 3;
					for (int i = 0; i < Cntchip.size(); i++)
					{
						Rect bx = cv::boundingRect(Cntchip[i]);
						cv::circle(markimg, Point(bx.x + 0.5 * bx.width, bx.y + bx.height * 0.5), 3, Scalar(0, 0, 255), -1);
					}
					cv::circle(markimg, Ref_chip_Pos, 3, Scalar(255, 0, 0), -1);
					cv::resize(markimg, markimg, Size(500, 500));
					cv::resize(thresimg, Grayimg, Size(500, 500));
					throw "please select another area";
				}
				
			}
			

			std::cout << "check flag is :: " << boolflag << endl;
			std::cout << endl;

		}
	}
	catch (const char* message)
	{
		std::cout << message << std::endl;

	}

	return{ boolflag ,Grayimg,markimg };
}

#pragma endregion STEP2_4by4check 

#pragma region «Ê¸Ë

void funCheck4x4(Mat& rawimg, Mat& cropedRImg, thresP thresParm, SettingP chipsetting,sizeTD target,int& boolflag,Mat& Grayimg,Mat& markimg_simu)
{

	/*****Step.1 roughly search chip:::*/
	/*Resize image to speed up::start*/
	double resizeTDwidth = target.TDwidth / 12;
	double resizeTDheight = target.TDheight / 12;
	std::cout << "calculate resize TD dimension is:: " << resizeTDwidth << " / " << resizeTDheight << endl;
	cv::resize(rawimg, cropedRImg, Size(int(rawimg.cols / 12), int(rawimg.rows / 12)), INTER_NEAREST);
	auto t_start2 = std::chrono::high_resolution_clock::now();

	Point Potchip;
	Mat thresimg;
	vector<Point> TDcnt;
	std::tie(Potchip, boolflag, thresimg, TDcnt) = potentialchipSearch_V1(cropedRImg, resizeTDwidth, resizeTDheight, target, thresParm, boolflag, chipsetting.interval[0]);

	auto t_end2 = std::chrono::high_resolution_clock::now();
	double elapsed_time_ms2 = std::chrono::duration<double, std::milli>(t_end2 - t_start2).count();
	std::cout << "calculate roughly-search-op time is:: " << elapsed_time_ms2 << endl;

	if (boolflag == 0)
	{
		std::tie(boolflag, Grayimg, markimg_simu) = check4by4_V1(cropedRImg, thresimg, boolflag, Potchip, chipsetting, TDcnt);
	}
	else
	{
		cv::resize(rawimg, markimg_simu, Size(500, 500), INTER_NEAREST);
		cv::resize(thresimg, Grayimg, Size(500, 500), INTER_NEAREST);
	}

}

#pragma endregion
