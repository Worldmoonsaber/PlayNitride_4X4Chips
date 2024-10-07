#include "Check4by4Chip_lib.h"
#include "OpenCV_Extension_Tool.h"


void funcCreateKmeanThresImg(thresP_ thresParm,Mat cropedRImg ,Mat& thresimg)
{
	Mat gauBGR, EnHBGR;
	cv::cvtColor(cropedRImg, cropedRImg, cv::COLOR_BGR2GRAY);
	cropedRImg.convertTo(cropedRImg, -1, 1.2, 0);
	cv::GaussianBlur(cropedRImg, gauBGR, Size(0, 0), 13);
	cv::addWeighted(cropedRImg, 1.5, gauBGR, -0.7, 0.0, EnHBGR); //(1.5, -0.7)

	Mat Kop;

	if (thresParm.thresmode == 3)
	{
		Kop = KmeanOP(2, EnHBGR);
		double minVal, maxVal; //maxVal: frequency
		Point minLoc, maxLoc; //maxLoc.y: pixel value
		minMaxLoc(Kop, &minVal, &maxVal, &minLoc, &maxLoc);
		//std::cout << "calculate min Loc is:: " << minLoc.y << " / " << maxLoc.y << " / " << minVal << " / " << maxVal << endl;
		threshold(Kop, thresimg, minVal + 1, 255, THRESH_BINARY_INV);
		cv::medianBlur(thresimg, thresimg, 5);
	}

	else if (thresParm.thresmode == 4)
	{
		Kop = KmeanOP(2, EnHBGR);
		double minVal, maxVal; //maxVal: frequency
		Point minLoc, maxLoc; //maxLoc.y: pixel value
		minMaxLoc(Kop, &minVal, &maxVal, &minLoc, &maxLoc);
		threshold(Kop, thresimg, maxVal - 1, 255, THRESH_BINARY);
		cv::medianBlur(thresimg, thresimg, 5);
	}
	else if (thresParm.thresmode == 0)
	{
		Mat drakfiled, brightfield;
		Kop = KmeanOP(3, EnHBGR);
		double minVal, maxVal; //maxVal: frequency
		Point minLoc, maxLoc; //maxLoc.y: pixel value
		minMaxLoc(Kop, &minVal, &maxVal, &minLoc, &maxLoc);
		threshold(Kop, brightfield, maxVal - 1, 255, THRESH_BINARY);
		cv::medianBlur(brightfield, thresimg, 5);

		drakfiled.release();
		brightfield.release();
	}
	else
	{
		Kop = KmeanOP(2, EnHBGR);
		double minVal, maxVal; //maxVal: frequency
		Point minLoc, maxLoc; //maxLoc.y: pixel value
		minMaxLoc(Kop, &minVal, &maxVal, &minLoc, &maxLoc);
		threshold(Kop, thresimg, minVal + 1, 255, THRESH_BINARY_INV);
		cv::medianBlur(thresimg, thresimg, 5);
	}

	Kop.release();
	gauBGR.release();
	EnHBGR.release();
}


#pragma region STEP1_roughlysearch 


std::tuple<Point, int, Mat, vector<Point>> potentialchipSearch_V1(Mat cropedRImg, double resizeTDwidth, double resizeTDheight, sizeTD_ target, thresP_ thresParm, int flag, double distPX)
{
	float ratio = 10;
	Point potentialchip = Point(0, 0);

	Mat thresimg;

	funcCreateKmeanThresImg(thresParm, cropedRImg, thresimg);


	//----------使用色彩過濾
	vector<Point> TDCNT;
	vector<BlobInfo> vChipPossible = RegionPartitionTopology(thresimg);
	vector<BlobInfo> vChips;
	vector<BlobInfo> vChipsNeedFilterSecondTime;


	Point2f piccenter = find_piccenter(thresimg);

	thresimg = Mat::zeros(cropedRImg.size(), CV_8UC1);
	//cropedRImg.release();

	vector<vector<Point>> vContour;

	float std_Aspect = 0;

	if (target.TDheight > target.TDwidth)
		std_Aspect = target.TDheight / target.TDwidth;
	else
		std_Aspect = target.TDwidth / target.TDheight;

	float max_MeasuredH = -1, max_MeasuredW = -1, min_MeasuredH = 5000, min_MeasuredW = 5000;

	for (int i = 0; i < vChipPossible.size(); i++)
	{
		if (std_Aspect * 2 < vChipPossible[i].AspectRatio())
			continue;

		if (std_Aspect * 0.5 > vChipPossible[i].AspectRatio())
			continue;


		if (vChipPossible[i].Center().x< resizeTDwidth * 0.5 || vChipPossible[i].Center().x>cropedRImg.size().width - resizeTDwidth * 0.5)
			continue;

		if (vChipPossible[i].Center().y< resizeTDheight * 0.5 || vChipPossible[i].Center().y>cropedRImg.size().height - resizeTDheight * 0.5)
			continue;

		if (vChipPossible[i].Width() > resizeTDwidth * target.TDmaxW)
			continue;

		if (vChipPossible[i].Width() < resizeTDwidth * target.TDminW)
			continue;


		if (vChipPossible[i].Height() > resizeTDheight * target.TDmaxH)
			continue;

		if (vChipPossible[i].Height() < resizeTDheight * target.TDminH)
			continue;


		if (vChipPossible[i].Rectangularity() < 0.5)
			continue;//絕對不可能是Chip

		if (vChipPossible[i].Rectangularity() >= 0.8) //方正度高 絕對是Chip 沒問題
		{
			if (vChipPossible[i].Width() > max_MeasuredW)
				max_MeasuredW = vChipPossible[i].Width();

			if (vChipPossible[i].Width() < min_MeasuredW)
				min_MeasuredW = vChipPossible[i].Width();

			if (vChipPossible[i].Height() > max_MeasuredH)
				max_MeasuredH = vChipPossible[i].Height();

			if (vChipPossible[i].Height() < min_MeasuredH)
				min_MeasuredH = vChipPossible[i].Height();


			vChips.push_back(vChipPossible[i]);
			vContour.push_back(vChipPossible[i].contourMain());
			continue;
		}
		else
			vChipsNeedFilterSecondTime.push_back(vChipPossible[i]); //有可能是Chip 需要更多條件卡控


	}

	//----統計平均方正度

	vector<BlobInfo> vChipsNeedFilterThirdTime;


	for (int i = 0; i < vChipsNeedFilterSecondTime.size(); i++)
	{
		if (vChipsNeedFilterSecondTime[i].Width() > max_MeasuredW || vChipsNeedFilterSecondTime[i].Width() < min_MeasuredW
			|| vChipsNeedFilterSecondTime[i].Height() > max_MeasuredH || vChipsNeedFilterSecondTime[i].Height() < min_MeasuredH)
			continue;

		vChips.push_back(vChipsNeedFilterSecondTime[i]);
		vContour.push_back(vChipsNeedFilterSecondTime[i].contourMain());
	}


	drawContours(thresimg, vContour, -1, Scalar(255, 255, 255), -1);

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
				Point potentialchipFullfield = Point2i(vChips[0].Center().x* ratio, vChips[0].Center().y* ratio);
				cout << "check tdpt(full img) is : " << potentialchipFullfield << endl;

				if (norm(potentialchipFullfield - Point2i(thresimg.cols * ratio/2, thresimg.rows * ratio /2)) > distPX)
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


std::tuple< int, Mat, Mat> check4by4_V1(Mat src,Mat thresimg, int boolflag, Point Ref_chip_Pos, SettingP_ chipsetting, vector<Point> TDcnt)
{
	float ratio = 10;
	Point finechip = Point(0, 0);
	Point IMGoffset = Point(0, 0);
	Mat Grayimg, markimg;
	src.copyTo(markimg);

	/*start......*/
	double resizedpitchX = chipsetting.xpitch[0] / ratio;
	double resizedpitchY = chipsetting.ypitch[0] / ratio;

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



#pragma region 封裝

void funCheck4x4(Mat& rawimg, Mat& cropedRImg, thresP_ thresParm, SettingP_ chipsetting,sizeTD_ target,int& boolflag,Mat& Grayimg,Mat& markimg_simu)
{


	float ratio=10;
	/*****Step.1 roughly search chip:::*/
	/*Resize image to speed up::start*/
	double resizeTDwidth = target.TDwidth / ratio;
	double resizeTDheight = target.TDheight / ratio;
	std::cout << "calculate resize TD dimension is:: " << resizeTDwidth << " / " << resizeTDheight << endl;
	cv::resize(rawimg, cropedRImg, Size(int(rawimg.cols / ratio), int(rawimg.rows / ratio)), INTER_NEAREST);
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
