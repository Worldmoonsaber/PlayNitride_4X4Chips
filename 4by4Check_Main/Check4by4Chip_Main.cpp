#include "Check4by4Chip_lib.h"



int main()
{
	SettingP_ chipsetting;
	thresP_ thresParm;
	ImgP_ imageParm;
	sizeTD_ target;

	imageParm.cols = 5320; //800 ;900-1600
	imageParm.rows = 4600;

	chipsetting.interval[0] = 500;
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

	int boolflag = 0;//11

	Mat Grayimg;


	Mat markimg_simu;

	// Image source input: IMG format:RGB
	try
	{
		//std::tie(picorder, rawimg) = Inputfunction();

		rawimg = imread("C:\\Users\\Playuser\\Downloads\\1004\\1004\\BadImg.bmp");

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

	//rawimg = imread();

	target.TDmaxW = 1.3;
	target.TDminW = 0.7;


	target.TDmaxH = 1.3;
	target.TDminH = 0.7;

	target.TDwidth = 192;
	target.TDheight = 350;

	//if (picorder > 132800 && picorder < 132899)
	//{


		thresParm = { 3,{99999,99999,99999},{99999,99999,99999} ,{3,99999,99999}, {99999,99999,99999} };//pic24052202

		thresParm.thresmode = 3;
		thresParm.bgmax[0] = 100;
		thresParm.fgmax[0] = 1;
		chipsetting.xpitch[0] = 318;
		chipsetting.ypitch[0] = 552;
	//}
	//else if (picorder > 153000 && picorder < 153099)
	//{

	//	target.TDwidth = 135;
	//	target.TDheight = 120;
	//	//thresParm = { 4,{111,99999,99999},{99999,99999,99999} ,{13,99999,99999}, {99999,99999,99999} };//pic24052202
	//	thresParm = { 0,{99999,99999,99999},{0,99999,99999} ,{255,99999,99999}, {180,99999,99999} };//pic153002
	//	chipsetting.xpitch[0] = 508;
	//	chipsetting.ypitch[0] = 340;
	//}
	//else if (picorder > 183600 && picorder < 183699)
	//{

	//	target.TDwidth = 301;
	//	target.TDheight = 153;
	//	//thresParm = { 4,{111,99999,99999},{99999,99999,99999} ,{13,99999,99999}, {99999,99999,99999} };//pic24052202
	//	thresParm = { 3,{99999,99999,99999},{0,99999,99999} ,{255,99999,99999}, {180,99999,99999} };//pic153002
	//	chipsetting.xpitch[0] = 513;
	//	chipsetting.ypitch[0] = 300;
	//}

	if (imageParm.cols != rawimg.cols || imageParm.rows != rawimg.rows)
	{
		boolflag == 7;
		rawimg.copyTo(markimg_simu);
		Grayimg = Mat::zeros(Size(500, 500), CV_8UC1);
	}

	if (boolflag == 0)
	{
		funCheck4x4(rawimg, cropedRImg, thresParm, chipsetting, target, boolflag, Grayimg, markimg_simu);
	}



	rawimg.release();
	cropedRImg.release();
	std::cout << "check img state:: " << boolflag << endl;
	return 0;
}

























