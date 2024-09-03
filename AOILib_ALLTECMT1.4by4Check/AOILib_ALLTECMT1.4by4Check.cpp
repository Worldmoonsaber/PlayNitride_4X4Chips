#include "AOILib_ALLTECMT1.4by4Check.h"
#include "../4by4Check_Main/Check4by4Chip_lib.h"




void Uchips_4by4check(thresP thresParm, ImgP imageParm, SettingP chipsetting, sizeTD target, unsigned int* imageIN, unsigned int* imageOUT, unsigned char* imageGray, float boolResult[], float outputLEDX[], float outputLEDY[])
{
#pragma region ®æ¦¡Âà´«
	thresP_ _thresParm;

	_thresParm.thresmode = thresParm.thresmode;

	for (int i = 0; i < 3; i++)
	{
		_thresParm.bgmax[i] = thresParm.bgmax[i];
		_thresParm.bgmin[i] = thresParm.bgmin[i];
		_thresParm.fgmax[i] = thresParm.fgmax[i];
		_thresParm.fgmin[i] = thresParm.fgmin[i];
	}

	ImgP_ _imageParm;

	_imageParm.correctTheta = imageParm.correctTheta;
	_imageParm.cols = imageParm.cols;
	_imageParm.rows = imageParm.rows;
	_imageParm.Outputmode = imageParm.Outputmode;
	_imageParm.PICmode = imageParm.PICmode;

	SettingP_ _chipsetting;

	_chipsetting.carx = chipsetting.carx;
	_chipsetting.cary = chipsetting.cary;

	for (int i = 0; i < 4; i++)
		_chipsetting.interval[i] = chipsetting.interval[i];

	for (int i = 0; i < 3; i++)
	{
		_chipsetting.xpitch[i] = chipsetting.xpitch[i];
		_chipsetting.ypitch[i] = chipsetting.ypitch[i];
	}

	sizeTD_ _target;

	_target.TDheight = target.TDheight;
	_target.TDmaxH = target.TDmaxH;
	_target.TDmaxW = target.TDmaxW;
	_target.TDminH = target.TDminH;
	_target.TDminW = target.TDminW;
	_target.TDwidth = target.TDwidth;
#pragma endregion

	Mat rawimg, cropedRImg;
	Point piccenter;
	Point IMGoffset;
	Point2f creteriaPoint;

	//output parameters::
	Mat ReqIMG, marksize;
	int boolflag = 0;//11
	Point Finechip;
	Mat Grayimg, markimg;
	Mat markimg_simu;

	Mat image_input(imageParm.rows, imageParm.cols, CV_8UC4, &imageIN[0]); // THIS IS THE INPUT IMAGE, POINTER TO DATA			
	image_input.copyTo(rawimg);

	Mat image_output(500, 500, CV_8UC4, &imageOUT[0]);
	Mat thres_output(500, 500, CV_8UC1, &imageGray[0]);

	try
	{
		if (rawimg.empty())
		{
			boolflag = 8;
			throw "something wrong::input image failure";
		} //check if image is empty

	} //try loop
	catch (const char* message)
	{
		std::cout << "check catch state:: " << boolflag << endl;
		return;

	}//catch loop


	if (imageParm.cols != rawimg.cols || imageParm.rows != rawimg.rows)
	{
		boolflag == 7;
		rawimg.copyTo(markimg_simu);
		Grayimg = Mat::zeros(Size(600, 500), CV_8UC1);
	}

	if (boolflag == 0)
	{
		creteriaPoint = find_piccenter(rawimg);
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
		std::tie(Potchip, boolflag, thresimg, TDcnt) = potentialchipSearch_V1(cropedRImg, resizeTDwidth, resizeTDheight, _target, _thresParm, boolflag, chipsetting.interval[0]);

		auto t_end2 = std::chrono::high_resolution_clock::now();
		double elapsed_time_ms2 = std::chrono::duration<double, std::milli>(t_end2 - t_start2).count();
		std::cout << "calculate roughly-search-op time is:: " << elapsed_time_ms2 << endl;

		if (boolflag == 0)
		{
			std::tie(boolflag, Grayimg, markimg_simu) = check4by4_V1(cropedRImg, thresimg, boolflag, Potchip, _chipsetting, TDcnt);
		}
		else
		{
			cv::resize(rawimg, markimg_simu, Size(500, 500), INTER_NEAREST);
			cv::resize(thresimg, Grayimg, Size(500, 500), INTER_NEAREST);
		}

	}

	std::cout << "check img state:: " << boolflag << endl;

	Grayimg.copyTo(thres_output);
	markimg_simu.copyTo(image_output);
	Grayimg.release();
	markimg_simu.release();

	boolResult[0] = boolflag;
}
