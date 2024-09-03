


#include "Check4by4Chip_lib.h"



int main()
{
	SettingP chipsetting;
	thresP thresParm;
	ImgP imageParm;
	sizeTD target;



	imageParm.cols = 5320; //800 ;900-1600
	imageParm.rows = 4600;

	chipsetting.interval[0] = 200;
	//chipsetting.interval[1] = 176; //490
	//chipsetting.interval[2] = 114; //273 


	imageParm.Outputmode = 0; //0:center coord ; 1: multiple mode
	imageParm.PICmode = 0;  // 0=B or L¡B1=G¡B2=R
	//chipsetting.interval[0] = 0; //2

	chipsetting.carx = 0;
	chipsetting.cary = 0;

	//Tell AOI how many angles should rotate : positive: counterclockwise   /negative:clockwise
	//imageParm.correctTheta = 2.6; //8280402
	imageParm.correctTheta = 0; // rotate angle=3
	//imageParm.correctTheta = -0.247; //L4_1020
	/////////////////////////////////////////
	Mat rawimg, cropedRImg;
	int picorder;
	Point piccenter;
	Point IMGoffset;
	Point2f creteriaPoint;




	//output parameters::
	Mat ReqIMG, marksize;
	Point simupt;
	int boolflag = 0;//11

	Point Finechip;
	Mat Grayimg, markimg;


	Mat markimg_simu;


	//operating mode
	int mode = 1;
	if (mode == 1)
	{
		// Image source input: IMG format:RGB
		try
		{
			std::tie(picorder, rawimg) = Inputfunction();

			if (rawimg.empty())
			{
				boolflag = 8;
				throw "something wrong::input image failure";
			} //check if image is empty

		} //try loop
		catch (const char* message)
		{

			std::cout << "check catch state:: " << boolflag << endl;


		}//catch loop


		target.TDmaxW = 1.3;
		target.TDminW = 0.7;


		target.TDmaxH = 1.3;
		target.TDminH = 0.7;



		if (picorder > 132800 && picorder < 132899)
		{

			target.TDwidth = 265;
			target.TDheight = 124;
			thresParm = {3,{99999,99999,99999},{99999,99999,99999} ,{3,99999,99999}, {99999,99999,99999} };//pic24052202
			chipsetting.xpitch[0] = 343;
			chipsetting.ypitch[0] = 230;
		}
		else if (picorder > 153000 && picorder < 153099)
		{

			target.TDwidth = 135;
			target.TDheight = 120;
			//thresParm = { 4,{111,99999,99999},{99999,99999,99999} ,{13,99999,99999}, {99999,99999,99999} };//pic24052202
			thresParm = { 0,{99999,99999,99999},{0,99999,99999} ,{255,99999,99999}, {180,99999,99999} };//pic153002
			chipsetting.xpitch[0] = 508;
			chipsetting.ypitch[0] = 340;
		}
		else if (picorder > 183600 && picorder < 183699)
		{

			target.TDwidth = 301;
			target.TDheight = 153;
			//thresParm = { 4,{111,99999,99999},{99999,99999,99999} ,{13,99999,99999}, {99999,99999,99999} };//pic24052202
			thresParm = { 3,{99999,99999,99999},{0,99999,99999} ,{255,99999,99999}, {180,99999,99999} };//pic153002
			chipsetting.xpitch[0] = 513;
			chipsetting.ypitch[0] = 300;
		}
		




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
			std::tie(Potchip, boolflag, thresimg, TDcnt) = potentialchipSearch_V1(cropedRImg, resizeTDwidth, resizeTDheight, target, thresParm, boolflag, chipsetting.interval[0]);

			auto t_end2 = std::chrono::high_resolution_clock::now();
			double elapsed_time_ms2 = std::chrono::duration<double, std::milli>(t_end2 - t_start2).count();
			std::cout << "calculate roughly-search-op time is:: " << elapsed_time_ms2 << endl;

		
			if (boolflag == 0)
			{

				std::tie( boolflag, Grayimg, markimg_simu) = check4by4_V1(cropedRImg,thresimg, boolflag, Potchip, chipsetting, TDcnt);
			
			}

			else
			{
				cv::resize(rawimg, markimg_simu, Size(500,500), INTER_NEAREST);
				cv::resize(thresimg, Grayimg, Size(500, 500), INTER_NEAREST);
				//Grayimg = Mat::zeros(Size(500, 500), CV_8UC1);
			}

		}
		
		std::cout << "check img state:: " << boolflag << endl;
		std::cout << "check center is ::" << simupt << endl;
	}



	return 0;
}

























