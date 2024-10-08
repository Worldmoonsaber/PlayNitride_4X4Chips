#include <opencv2/core/types.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <format>
#include <opencv2/photo.hpp>
using namespace std;
using namespace cv;
#pragma comment(lib,"AOILib_Common.lib") 

#pragma region 資料結構

typedef struct
{
	int interval[4];
	int xpitch[3]; //in unit of pixel  xpitch[0]=250pixel
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
	int thresmode;
	int bgmax[3];
	int bgmin[3];
	int fgmax[3];
	int fgmin[3];
}thresP;

typedef struct
{
	int PICmode;
	int Outputmode;
	int cols; //1500
	int rows; //1500    
	double correctTheta;
}ImgP;

#pragma endregion


#pragma region Blob 切割

class BlobInfo;

_declspec(dllexport) vector<BlobInfo> RegionPartitionTopology(Mat ImgBinary);

_declspec(dllexport) vector<BlobInfo> RegionPartition(Mat ImgBinary, int maxArea, int minArea);

#pragma endregion

#pragma region 影像切割與尋找邊緣

_declspec(dllexport) Point find_piccenter(Mat src);

_declspec(dllexport) Mat CropIMG(Mat img, Rect size);

_declspec(dllexport) int findBoundary(Mat creteriaIMG, Rect inirect, char direction);

_declspec(dllexport) std::tuple<Rect, Point>FindMaxInnerRect(Mat src, Mat colorSRC, sizeTD target, Point TDcenter);


#pragma endregion

#pragma region 影像強化處理

_declspec(dllexport) Mat ContrastEnhancement(Mat srcimg, float alpha, float beta);

_declspec(dllexport) void gammaCorrection(const Mat& src, Mat& dst, const float gamma);

_declspec(dllexport) Mat KmeanOP(int knum, Mat src);

_declspec(dllexport) void funcCreateKmeanThresImg(thresP thresParm, Mat cropedRImg, Mat& thresImgOut);

#pragma endregion

_declspec(dllexport) void funcRotatePoint(vector<Point> vPt, vector<Point>& vPtOut, Mat& marksize, float correctTheta, Point IMGoffset);

_declspec(dllexport) Mat RotatecorrectImg(double Rtheta, Mat src);

_declspec(dllexport) void funcThreshold(Mat ImgInput, Mat& ImgThres, thresP thresParm, ImgP imageParm, sizeTD target);

_declspec(dllexport) std::tuple<int, Point_<int>> FindMF_pixel(Mat histImg);

_declspec(dllexport) std::tuple<Mat, Mat, Mat>Histplotting(Mat src, int hist_w, int hist_h, int histsize);


#pragma region 一定要放在這裡 呼叫dll時才不會出錯 還在研究有沒有比較好的方法

_declspec(dllexport) class BlobInfo
{
public:
    BlobInfo(vector<Point> vArea, vector<Point> vContour);
    BlobInfo();
    BlobInfo(Mat ImgRegion);
    BlobInfo(vector<Point> vContour);
    BlobInfo(vector<Point> vMainContour, vector<vector<Point>> vHollowContour);

    _declspec(dllexport) void CaculateBlob(vector<Point> vArea, vector<Point> vContour);
    _declspec(dllexport) void Release();
    _declspec(dllexport) int Area();

    /// <summary>
    ///  1 ~ 0.0   1:完美圓形
    /// </summary>
    /// <returns></returns>
    _declspec(dllexport) float Circularity();
    _declspec(dllexport) Point2f Center();
    /// <summary>
    ///  1 ~ 0.0   1:完美矩形
    /// </summary>
    /// <returns></returns>
    _declspec(dllexport) float Rectangularity();
    _declspec(dllexport) float minRectHeight();
    _declspec(dllexport) float minRectWidth();
    _declspec(dllexport) float Angle();
    /// <summary>
    /// 長寬比
    /// </summary>
    /// <returns></returns>
    _declspec(dllexport) float AspectRatio();
    _declspec(dllexport) vector<Point> Points();
    _declspec(dllexport) vector<Point> contour();

    /// <summary>
    /// 長軸長度
    /// </summary>
    /// <returns></returns>
    _declspec(dllexport) float Ra();

    /// <summary>
    /// 短軸長度
    /// </summary>
    /// <returns></returns>
    _declspec(dllexport) float Rb();

    _declspec(dllexport) int Xmin();
    _declspec(dllexport) int Ymin();
    _declspec(dllexport) int Xmax();
    _declspec(dllexport) int Ymax();
    _declspec(dllexport) int Width();
    _declspec(dllexport) int Height();
    /// <summary>
    /// 蓬鬆度
    /// </summary>
    /// <returns></returns>
    _declspec(dllexport) float Bulkiness();

    /// <summary>
    /// 緊緻度
    /// </summary>
    /// <returns></returns>
    _declspec(dllexport) float Compactness();

    /// <summary>
    /// 與 Circularity 有定義上的差別 這個屬性更適合偵測 中空圓環的圓環 圓環的 Roundness趨近於1 Circularity 0.1 左右
    /// </summary>
    /// <returns></returns>
    _declspec(dllexport) float Roundness();

    _declspec(dllexport) float Sides();


    /// <summary>
    /// Topology 才有用的屬性
    /// </summary>
    /// <returns></returns>
    _declspec(dllexport) vector<vector<Point>> contourHollow();

    /// <summary>
    /// Topology 才有用的屬性
    /// </summary>
    /// <returns></returns>
    _declspec(dllexport) vector<Point> contourMain();

private:

    int _area = -1;
    float _circularity = -1;
    float _rectangularity = -1;

    Point2f _center;

    vector<Point> _points{};
    vector<Point> _contour{};

    int _XminBound = -1;
    int _YminBound = -1;
    int _XmaxBound = -1;
    int _YmaxBound = -1;
    float _minRectWidth = -1;
    float _minRectHeight = -1;
    float _Angle = -1;
    float _AspectRatio = -1;
    float _Ra = -1;
    float _Rb = -1;
    float _bulkiness = -1;
    float _compactness = -1;
    float _roundness = -1;
    float _sides = -1;
    float _Width = -1;
    float _Height = -1;

    vector<Point> _contourMain{};
    vector<vector<Point>> _contourHollow{};

};


#pragma endregion

