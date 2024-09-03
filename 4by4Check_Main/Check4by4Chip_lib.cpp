#include "Check4by4Chip_lib.h"


#pragma region STEP1_roughlysearch 

std::tuple<Point, int, Mat, vector<Point>> potentialchipSearch_V1(Mat cropedRImg, double resizeTDwidth, double resizeTDheight, sizeTD_ target, thresP_ thresParm, int flag, double distPX)
{
	Point potentialchip = Point(0, 0);


	/*sub-function start*/
			//Input: cropedRImg

	Mat gauBGR, EnHBGR, thresimg;
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
		threshold(Kop, thresimg, minVal+1, 255, THRESH_BINARY_INV);
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
		threshold(Kop, brightfield, maxVal-1, 255, THRESH_BINARY);
		cv::medianBlur(brightfield, thresimg, 5);
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


	vector<vector<Point>> contthres;
	vector<vector<Point>> potCNT;
	vector<Point> TDCNT;
	vector<Point2f> potPT;
	vector<double>potDist;


	Mat upscaleImg = Mat::zeros(thresimg.size(), CV_8UC1);
	Mat potentialIMG = Mat::zeros(thresimg.size(), CV_8UC1);

	

	cv::findContours(thresimg, contthres, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point());
	Point2f piccenter = find_piccenter(thresimg);

	try
	{
		if (contthres.size() == 0)
		{
			flag = 1;
			thresimg.copyTo(potentialIMG);
			throw "something wrong::thresholdmode value issue";
		}
		else
		{


			for (int i = 0; i < contthres.size(); i++)
			{
				Rect bx = boundingRect(contthres[i]);
														
				if (bx.width * bx.height< (resizeTDwidth * resizeTDheight * 1.4) &&
					bx.width * bx.height >(resizeTDwidth * resizeTDheight * 0.5) ) //theta=3
				{
										

					Moments M = (moments(contthres[i], false));
					Point2f Mpt = (Point2f((M.m10 / M.m00), (M.m01 / M.m00)));
					potCNT.push_back(contthres[i]);
					potPT.push_back(Mpt);
					potDist.push_back((norm(piccenter - potPT[potPT.size() - 1])));
					cv::drawContours(potentialIMG, contthres, i, Scalar::all(255), -1);
					//cout << "check potential center"<<endl;
				}
			}


			cout << "check potCNT.size() is : " << potCNT.size() << endl;


			if (potDist.size() <16) //4*4=16
			{
				flag = 1;
				thresimg.copyTo(potentialIMG);
				throw "something wrong::thresholdmode value issue";
			}
			else
			{
				auto chipiter = std::min_element(potDist.begin(), potDist.end());
				int chipIndex = std::distance(potDist.begin(), chipiter);
				TDCNT = potCNT[chipIndex];
				//cout << "check potential center is : " << potPT[chipIndex]  << "[ "<< piccenter<<" ]" << endl;

				circle(upscaleImg, potPT[chipIndex], 3, Scalar::all(255), -1);
				//circle(thresimg, piccenter, 1, Scalar::all(150), -1);
				cv::resize(upscaleImg, upscaleImg, Size(int(thresimg.cols * 12), int(thresimg.rows * 12)), INTER_NEAREST);

				cv::findContours(upscaleImg, contthres, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point());

				Moments M1 = (moments(contthres[0], false));
				Point potentialchipFullfield = Point2i((Point2f((M1.m10 / M1.m00), (M1.m01 / M1.m00))));
				cout << "check tdpt(full img) is : " << potentialchipFullfield << endl;

				if (norm(potentialchipFullfield - Point2i(upscaleImg.cols * 0.5, upscaleImg.rows * 0.5)) > distPX)
				{
					flag = 6;
					potentialchip = potPT[chipIndex];
				}
				else
				{
					flag = 0;
					potentialchip = potPT[chipIndex];
				}
			}
		}

	}

	catch (const char* message)
	{
		std::cout << message << std::endl;

	}

	
	return{ potentialchip,flag,potentialIMG,TDCNT };
}

#pragma endregion STEP1_roughlysearch 



#pragma region STEP2_4by4check 


std::tuple< int, Mat, Mat> check4by4_V1(Mat src,Mat thresimg, int boolflag, Point Potchip, SettingP_ chipsetting, vector<Point> TDcnt)
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
	Point leftopPt = Potchip - Point(2 * resizedpitchX, 2 * resizedpitchY);

	
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
				cv::circle(markimg, Potchip, 3, Scalar(255, 0, 0), -1);
				cv::resize(markimg, markimg, Size(500, 500));
				cv::resize(thresimg, Grayimg, Size(500, 500));
			}
			else
			{
				if (Cntchip.size() > 16)
				{
					boolflag = 2;
					
					cv::circle(markimg, Potchip, 3, Scalar(255, 0, 0), -1);
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
					cv::circle(markimg, Potchip, 3, Scalar(255, 0, 0), -1);
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



