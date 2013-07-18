#pragma once
#include "backgroundupdatedao.h"
class CBackgroundUpdateDaoImpl :public CBackgroundUpdateDao
{
public:
	CBackgroundUpdateDaoImpl(void);
//	CBackgroundUpdateDaoImpl(int nwidth,int nheight);
	~CBackgroundUpdateDaoImpl(void);

	bool CountImageUpdatePixels(int i,int j,int x);

	bool SetWidth_Height(int width,int height);

//	bool ResetImageCountUpdatePixels();
	bool GetGrayAndBackgroundPTR(unsigned char *curImage,unsigned char *backgroundImage);


	//curImageData当前灰度图像，backgroundImageData背景灰度图像
	bool UpdateBackgroundPixels(unsigned char *curImageData,unsigned char *backgrodImageData);
	bool ImageCountAddOne();
	bool ImageCountClearZero();
public:
	//与图片的每个像素点对应，计数像素随时间变化
	int **m_PixelsTimes;
	int m_count;
private:
	//处理每100张图片则更新下背景
	const static int m_UpdateImageCount=100; 
	//更新系数
	 float a;
private:
	unsigned char *m_curGrayImagePtr;
	unsigned char *m_backgroundImagePtr;
	int m_nheight;
	int m_nwidth;
};

