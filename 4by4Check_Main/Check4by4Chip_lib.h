#pragma once
#pragma once
#pragma once

#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp> //mophorlogical operation
#include<opencv2/core.hpp>
#include <numeric>
#include <chrono>

using namespace cv;
using namespace std;

#pragma region structure_declare
typedef struct
{
	int interval[4]; //x interval, y interval
	int xpitch[3]; //in unit of pixel 
	int ypitch[3];
	int carx;
	int cary;
}SettingP_;

typedef struct
{

	double TDwidth;
	double TDmaxW;
	double TDminW;
	double TDheight;
	double TDmaxH;
	double TDminH;

}sizeTD_;

typedef struct
{
	int thresmode; //0:gray ; 1:RGB ; 2:HSV
	int bgmax[3];
	int bgmin[3];
	int fgmax[3];
	int fgmin[3];

}thresP_;

typedef struct
{
	int PICmode; //0: read pic from path ; 1: read pic from pointer address
	int Outputmode;
	int cols; //
	int rows; //
	double correctTheta;
}ImgP_;

#pragma endregion



#pragma region function_declare
/*Input function*/
std::tuple<int, Mat> Inputfunction(); //picture order, bg coodinate, picture
std::tuple < int, int> dict_masksize(int picorder);


/*general operation*/
Point find_piccenter(Mat src);
Mat CropIMG(Mat img, Rect size);

Mat KmeanOP(int knum, Mat src);


std::tuple<Point, int,Mat, vector<Point>> potentialchipSearch_V1(Mat cropedRImg, double resizeTDwidth, double resizeTDheight, sizeTD_ target, thresP_ thresParm, int flag, double distPX);
std::tuple< int, Mat, Mat> check4by4_V1(Mat src, Mat thresimg, int boolflag, Point Potchip, SettingP_ chipsetting, vector<Point> TDcnt);

void funCheck4x4(Mat rawimg, Mat cropedRImg, thresP_ thresParm, SettingP_ chipsetting, sizeTD_ target, int& boolflag, Mat& Grayimg, Mat& markimg_simu);
#pragma endregion