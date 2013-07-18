#pragma once
class CBackgroundUpdateDao
{
public:
	CBackgroundUpdateDao(void);
	~CBackgroundUpdateDao(void);
	virtual bool CountImageUpdatePixels(int i,int j,int x)=0;
	//curImageData��ǰ�Ҷ�ͼ��backgroundImageData�����Ҷ�ͼ��

	virtual bool SetWidth_Height(int width,int height)=0;

//	virtual bool ResetImageCountUpdatePixels()=0;
	virtual bool GetGrayAndBackgroundPTR(unsigned char *curImage,unsigned char *backgroundImage)=0;

	
	virtual bool UpdateBackgroundPixels(unsigned char *curImageData,unsigned char *backgroundImageData)=0;
	virtual bool ImageCountAddOne()=0;
	virtual bool ImageCountClearZero()=0;
};

