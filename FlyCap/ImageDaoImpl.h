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
	virtual bool CaptureBackgroundImage();			//��׽����ͼƬ
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



	// 	���ܣ��Ľ�����ֵ�˲�
	// 
	// 		ע���ڱ�׼��ֵ�˲��Ļ����ϣ�������ÿһ������ʱ���жϸ������Ƿ����˲��������������������صļ�����߼�Сֵ��
	// 		����ǣ��������������ֵ�˲���������أ�������ǣ����账��
	// 
	// ����:
	// 	CImgProcess* pTo��Ŀ��ͼ��� CImgProcess ָ��
	// 		int   nFilterH���˲����ĸ߶�
	// 		int   nFilterW���˲����Ŀ��
	// 		int   nFilterMX���˲���������Ԫ��Y����
	// 		int   nFilterMY���˲���������Ԫ��X����
	bool AdaptiveMedianFilter(int nFilterH, int nFilterW, int nFilterMY, int nFilterMX);

	void Open( int se[3][3]);//������
	void Close(int se[3][3]);//������
private:
	//�ٶ�̫����  ���� ����������� ���Ƿ�����ջ����Ӱ������
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
	int DetectThreshold(int nMaxIter, int &nDiffRet,int &numMax,int &numMin);			//ţ�ٵ����� ������������ ������ֵ
	int DetectOTSUMethod_OneDimension();
	int DetectThreshOTSUMethod_TwoDimension();
	void Erode(int se[3][3]); //��ʴ�㷨
	void Dilate(int se[3][3]); //�����㷨

private:
	Image *m_processimage;					//used for minus ,in order to get the information we want
	int nWidthBytes;
	int nHeightBytes;
	int m_imagecount;
	vector<CPoint> *pointArray;
	unsigned char *m_imagedata;
	bool showImageDrawRECflag;			//a flag used for draw rect on the showImage
	
	//��� ���ڼ���Ƿ�һֱ����ĳ����
	char m_IsKeyRepeatFlag;
private:   
	static const int m_threshold=64;		//+64				//��׼ ��ֵ����׼
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

	//����Ѱ�����Ϣ;  �ǳ���Ҫ��
	vector<char> *keyclickedArray;
	//Ҫ�Ȱ���Y֮����ܰ��ϴ��Ǽ�����keyclickedArray���飬����֮ǰ���ȷ�keyclickedtemp��
	static char keyclickedtemp;


	bool IsFingerSkin(unsigned char *lpdata,int i,int j);

	//ͼƬ���ĸ��㱻����
	char O_IskeyClicked(int x,int y,vector<CKeyLocation> *keyArray);

	//����������Ҫ��ô��
	bool O_resolution(char key);
	virtual bool SearchFingerPoint(unsigned char * lpdata,int nTempW, int nTempH,int nTempMX, int nTempMY,vector<CKeyLocation> *keyVector);




	//set point trace on the pic
	virtual bool SetImagePointTrace(unsigned char *);
	//Bresenham�㷨 --ͼ��ѧ�㷨   ͼƬ�ϻ�ֱ��
	void BresenhamDrawline(unsigned char *lpdata,int x1,int y1,int x2,int y2,int bitcount,int color=(int)backgroundcolor);		
	//�� lpdataͼ������� �����
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

