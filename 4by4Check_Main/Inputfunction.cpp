#include "Check4by4Chip_lib.h"


#pragma region Input_function



std::tuple<int, Mat> Inputfunction()
{
	int picorder, picmode;

	bool valid = false;
	Mat Rawpic;

	do
	{

		std::cout << "Load picture from 1328 (0) or 1530 (1) or 1836(2) :" << endl;
		//std::cout << "Load picture from L4-AUO(6) / L4-0411(7)  /1328(8) / 1530(9) / 0613_L5(10) / 0624(11)" << endl;
		cin >> picmode;
		if (picmode > -1 && picmode < 20)
		{
			valid = true;
			if (picmode == 0)
			{
				std::cout << "Enter the number of the picture:";
				cin >> picorder;
				//L5 basler camera::
				//Rawpic = imread("D:/AOI_Project/L5-1AOI/L5-1Offset/pic/Testpic/" + string(std::to_string(picorder)) + ".bmp");//loading the image
				//L5-1 lucid camera::

				Rawpic = imread(R"x(D:\AOI_Project\L5-1AOI\MT1_4by4chips\pic\1328\)x" + string(std::to_string(picorder)) + ".bmp");

				///cv::imwrite(R"x(D:\AOI_Project\L0_MassTrimming\CPchip\pic\20240614\704102.bmp)x", Rawpic);

				//Rawpic = imread(R"x(E:\出差相關用程式\20240701\LS01\pic\20240614\)x" + string(std::to_string(picorder)) + ".bmp");



			}
			else if (picmode == 1)
			{
				std::cout << "Enter the number of the picture:";
				cin >> picorder;
				//L5 basler camera::
				//Rawpic = imread("D:/AOI_Project/L5-1AOI/L5-1Offset/pic/Testpic/" + string(std::to_string(picorder)) + ".bmp");//loading the image
				//L5-1 lucid camera::

				Rawpic = imread(R"x(D:\AOI_Project\L5-1AOI\MT1_4by4chips\pic\1530\)x" + string(std::to_string(picorder)) + ".bmp");

				///cv::imwrite(R"x(D:\AOI_Project\L0_MassTrimming\CPchip\pic\20240614\704102.bmp)x", Rawpic);

				//Rawpic = imread(R"x(E:\出差相關用程式\20240701\LS01\pic\20240614\)x" + string(std::to_string(picorder)) + ".bmp");



			}
			else if (picmode == 2)
			{
				std::cout << "Enter the number of the picture:";
				cin >> picorder;
				//L5 basler camera::
				//Rawpic = imread("D:/AOI_Project/L5-1AOI/L5-1Offset/pic/Testpic/" + string(std::to_string(picorder)) + ".bmp");//loading the image
				//L5-1 lucid camera::

				Rawpic = imread(R"x(D:\AOI_Project\L5-1AOI\MT1_4by4chips\pic\1836\)x" + string(std::to_string(picorder)) + ".bmp");

				



			}


		}
		else
		{
			std::cout << "Load picture from 1328 (0) or 1530 (1)  :";
			cin >> picmode;
		}
	} while (!valid);

	std::cout << "end-----------------------------------" << endl;
	return { picorder ,Rawpic };

}


std::tuple < int, int> dict_masksize(int picorder)
{


	////82800:::
	//int TDwidth = 281;//275:2576/2301 //280
	//int TDheight = 170;//140

	//1019xx:::
	//int TDwidth = 236;//275:2576/2301 //280
	//int TDheight = 145;//140

	//828016::small
	//int TDwidth = 236;//275:2576/2301
	//int TDheight = 149;//140

	//1229::
	int TDwidth = 278;//275:2576/2301 //280
	int TDheight = 156;//140

	if (picorder > 118 && picorder < 2450264)
	{
		TDwidth = 513;
		TDheight = 295;

	}
	else if (picorder > 10200 && picorder < 10275)
	{
		TDwidth = 230;//275:2576/2301 //280
		TDheight = 143;//140
	}


	else //828
	{
		TDwidth = 284;//278
		TDheight = 156;//156

		/*828024
		TDwidth = 235;
		TDheight = 156;
		*/

		/*828034
		TDwidth = 284;//278
		TDheight = 156;//156
		*/

	}

	return{ TDwidth,TDheight };
}





#pragma endregion