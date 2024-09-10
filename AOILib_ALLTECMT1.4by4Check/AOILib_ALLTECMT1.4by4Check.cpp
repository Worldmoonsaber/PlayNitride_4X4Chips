#include "AOILib_ALLTECMT1.4by4Check.h"
#include "../4by4Check_Main/Check4by4Chip_lib.h"




void Uchips_4by4check(thresP thresParm, ImgP imageParm, SettingP chipsetting, sizeTD target, unsigned int* imageIN, unsigned int* imageOUT, unsigned char* imageGray, float boolResult[], float outputLEDX[], float outputLEDY[])
{
	
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
		Grayimg = Mat::zeros(Size(500, 500), CV_8UC1);
	}

	if (boolflag == 0)
	{
		funCheck4x4(rawimg, cropedRImg, thresParm, chipsetting, target, boolflag, Grayimg, markimg_simu);
	}

	std::cout << "check img state:: " << boolflag << endl;

	Grayimg.copyTo(thres_output);
	markimg_simu.copyTo(image_output);
	Grayimg.release();
	markimg_simu.release();

	boolResult[0] = boolflag;
}
