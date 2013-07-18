#pragma once

#include "imagedao.h"
#include "iostream"
#include "vector"
#include "map"
#include "string"
#include "KeyLocation.h"

#include "BackgroundUpdateDao.h"
#include "BackgroundUpdateDaoImpl.h"
using namespace std;

class CImageDaoImpl :	public CImageDao
{
public:
	CImageDaoImpl(void);
	virtual ~CImageDaoImpl(void);
	CImageDaoImpl(Image *image);
	virtual	Image *GetImage();
	virtual bool DeepCopy(Image *image);
	bool SetImage(Image *image);
	virtual bool CaptureBackgroundImage();			//捕捉背景图片
// 	virtual bool Get2BImagePixels()=0;
// 	virtual bool GetGRAYPixels()=0;
// 	virtual bool GetProcessedPixels()=0;
//	virtual bool GetShowPixels()=0;
	virtual bool GetImageSize(int *width,int *height);
	
	virtual bool SaveImage(char *);

	virtual bool ConvertGray(Image *image);
	virtual bool ImageMinus(Image *backgroundImage);
	virtual bool RGBtoGray();
	virtual bool ConvertGray2BW();



	// 	功能：改进的中值滤波
	// 
	// 		注：在标准中值滤波的基础上，当处理每一个象素时，判断该象素是否是滤波窗口所覆盖下邻域象素的极大或者极小值。
	// 		如果是，则采用正常的中值滤波处理该象素；如果不是，则不予处理。
	// 
	// 参数:
	// 	CImgProcess* pTo：目标图像的 CImgProcess 指针
	// 		int   nFilterH：滤波器的高度
	// 		int   nFilterW：滤波器的宽度
	// 		int   nFilterMX：滤波器的中心元素Y坐标
	// 		int   nFilterMY：滤波器的中心元素X坐标
	bool AdaptiveMedianFilter(int nFilterH, int nFilterW, int nFilterMY, int nFilterMX);

	void Open( int se[3][3]);//开运算
	void Close(int se[3][3]);//闭运算
private:
	//速度太慢了  尽管 代码更加清晰 但是反复堆栈调用影响速率
// 	inline unsigned char  getPixels(int x,int y)
// 	{
// 		unsigned char *bitdata=m_processimage->GetData();
// 		
// 		return *(bitdata+x*nWidthBytes +y);
// 	}
// 	inline void setPixels(int x,int y,unsigned char gray)
// 	{
// 		unsigned char *bitdata=m_processimage->GetData();
// 		*(bitdata+x*nWidthBytes+y)=gray;
// 	}
	int BytesWide(unsigned int num);				//convert byte to 4^n


	int GetMedianValue(int * pAryGray, int nFilterLen);
	int DetectPixelRoundThreshold(unsigned char *srcImage,int i,int j,int tempx,int tempy);
	int DetectThreshold(int nMaxIter, int &nDiffRet,int &numMax,int &numMin);			//牛顿迭代法 最大迭代次数和 亮暗差值
	int DetectOTSUMethod_OneDimension();
	int DetectThreshOTSUMethod_TwoDimension();
	void Erode(int se[3][3]); //腐蚀算法
	void Dilate(int se[3][3]); //膨胀算法

private:
	Image *m_processimage;					//used for minus ,in order to get the information we want
	int nWidthBytes;
	int nHeightBytes;
	int m_imagecount;
	vector<CPoint> *pointArray;
	unsigned char *m_imagedata;
	bool showImageDrawRECflag;			//a flag used for draw rect on the showImage
	
	//标记 用于检测是否一直按着某个键
	char m_IsKeyRepeatFlag;
private:   
	static const int m_threshold=64;		//+64				//基准 二值化基准
	static  bool isfirstFlag;								//use for solving the problem which the FLYCAP SDK causes
	static const unsigned char backgroundcolor=0;
	static const unsigned char frontcolor=255;				
	static const int thredFingerCount=13;		//5 for 3*3   1,1			//number for detect the finger location
	static const int pointColor=RGB(255,0,0);				//set a value related with the bitmap
	static const int Rectanglstep=50;

// 	bool CImageDaoImpl::isfirstFlag;
// 	unsigned char CImageDaoImpl::backgroundcolor;
// 	unsigned char CImageDaoImpl::frontcolor;
// 	int CImageDaoImpl::thredFingerCount;
public:
	void SkinDetect();

public:
	//////////////////////////////////////////////////////////////////////////
	//.....................used for deal with the un-private object............
	//....................helper......................................


	
	CString GetHasClickedKey();

	 bool ClearHasClickedKey();
   // map<char,CRect> *mapArray;

	//存放已按键信息;  非常重要的
	vector<char> *keyclickedArray;
	//要等按了Y之后才能把上次那键放入keyclickedArray数组，所以之前就先放keyclickedtemp中
	static char keyclickedtemp;


	bool IsFingerSkin(unsigned char *lpdata,int i,int j);

	//图片上哪个点被按了
	char O_IskeyClicked(int x,int y,vector<CKeyLocation> *keyArray);

	//解析按键的要怎么做
	bool O_resolution(char key);
	virtual bool SearchFingerPoint(unsigned char * lpdata,int nTempW, int nTempH,int nTempMX, int nTempMY,vector<CKeyLocation> *keyVector);




	//set point trace on the pic
	virtual bool SetImagePointTrace(unsigned char *);
	//Bresenham算法 --图形学算法   图片上画直线
	void BresenhamDrawline(unsigned char *lpdata,int x1,int y1,int x2,int y2,int bitcount,int color=(int)backgroundcolor);		
	//对 lpdata图像的数据 画外框
	bool DrawRectangle(unsigned char *lpdata,int x, int y,int step,COLORREF color);

private:

	static long o_startCount;
	static long o_endCount;
	static char o_lastkey;

private:
	CBackgroundUpdateDao *m_backgroundUpdateDao;
public:

	int m_frontColorCount;
	int m_backgroundColorCount;
//	bool ResetCountImageUpdatePixels();
	
	bool GetGrayAndBackgroundPTR(unsigned char *curImage,unsigned char *backgroundImage);

	bool BackgroundUpdateOperate(unsigned char *curImageData,unsigned char *backgroundImageData);
	CString m_front_background_Pixelstatics;
	CString Getfront_background_Pixelstatics(void);
};

