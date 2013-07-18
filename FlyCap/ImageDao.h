#pragma once
#include "vector"
#include "KeyLocation.h"
class CImageDao
{
public:
	CImageDao(void);
	virtual ~CImageDao(void);
	virtual bool CaptureBackgroundImage()=0;			//≤∂◊Ω±≥æ∞Õº∆¨
	virtual	Image *GetImage()=0;
	virtual bool DeepCopy(Image *image)=0;
	virtual bool SetImage(Image *image)=0;
// 	virtual bool Convert2BImagePixels()=0;
//  	virtual bool ConvertGRAYPixels()=0;
// 	virtual bool ConvertProcessedPixels()=0;
//  	virtual bool GetShowPixels()=0;
 	virtual bool GetImageSize(int *width,int *height)=0;
	virtual bool SaveImage(char *)=0;

	virtual bool ConvertGray(Image *image)=0;
	virtual bool ImageMinus(Image *backgroundImage)=0;
	virtual bool AdaptiveMedianFilter(int nFilterH, int nFilterW, int nFilterMY, int nFilterMX)=0;
	virtual	bool RGBtoGray()=0;
	virtual bool ConvertGray2BW()=0;
//	virtual bool DrawRectangle(int x, int y,int step)=0;

	virtual void Open(int se[3][3])=0;//ø™‘ÀÀ„
	virtual void Close(int se[3][3])=0;//±’‘ÀÀ„



	//another algorithm
	virtual void SkinDetect()=0;


	//////////////////////////////////////////////////////////////////////////
	//.....................used for deal with the un-private object............
	//....................helper......................................
	virtual bool SearchFingerPoint(unsigned char *lpdata,int nTempW, int nTempH,int nTempMX, int nTempMY,vector<CKeyLocation> *keyVector)=0;
	//virtual bool IsFingerSkin(unsigned char *lpdata,int i,int j)=0;
	virtual bool SetImagePointTrace(unsigned char *)=0;

	virtual CString GetHasClickedKey()=0;

	//virtual vector<CKeyLocation> GetAllKeyPisition()=0;

	virtual bool ClearHasClickedKey()=0;



//	virtual bool ResetCountImageUpdatePixels()=0;

	virtual bool GetGrayAndBackgroundPTR(unsigned char *curImage,unsigned char *backgroundImage)=0;

	virtual CString Getfront_background_Pixelstatics()=0;

	virtual bool BackgroundUpdateOperate(unsigned char *curImageData,unsigned char *backgroundImageData)=0;
};

