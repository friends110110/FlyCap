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


	//curImageData��ǰ�Ҷ�ͼ��backgroundImageData�����Ҷ�ͼ��
	bool UpdateBackgroundPixels(unsigned char *curImageData,unsigned char *backgrodImageData);
	bool ImageCountAddOne();
	bool ImageCountClearZero();
public:
	//��ͼƬ��ÿ�����ص��Ӧ������������ʱ��仯
	int **m_PixelsTimes;
	int m_count;
private:
	//����ÿ100��ͼƬ������±���
	const static int m_UpdateImageCount=100; 
	//����ϵ��
	 float a;
private:
	unsigned char *m_curGrayImagePtr;
	unsigned char *m_backgroundImagePtr;
	int m_nheight;
	int m_nwidth;
};

