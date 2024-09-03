#pragma once



typedef struct
{
	int interval[4];
	int xpitch[3]; 
	int ypitch[3];
	int carx;
	int cary;

}SettingP;

typedef struct
{

	double TDwidth; //135
	double TDmaxW;  //1.25
	double TDminW;  //0.9
	double TDheight; //75
	double TDmaxH;   //1.5
	double TDminH;   //0.7

}sizeTD;


typedef struct 
{
	int thresmode; //0,3,4
	int bgmax[3];   //[99999,99999,99999]
	int bgmin[3];   //[99999,99999,99999]
	int fgmax[3];   //[99999,99999,99999]
	int fgmin[3];   //[99999,99999,99999]
}thresP;

typedef struct
{
	int PICmode; 
	int Outputmode; 
	int cols; //5320
	int rows; //4600   
	double correctTheta;
}ImgP;



__declspec(dllexport)  void Uchips_4by4check(thresP thresParm, ImgP imageParm, SettingP chipsetting, sizeTD target, unsigned int* imageIN,
						unsigned int* imageOUT, unsigned char* imageGray, float boolResult[], float outputLEDX[], float outputLEDY[]);



