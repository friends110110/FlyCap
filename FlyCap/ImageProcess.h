#pragma once
#include "vector"
using namespace std;
class CImageProcess
{
public:
	CImageProcess(void);
	~CImageProcess(void);
	unsigned char * ImageMinus(LPBYTE destimag,LPBYTE backgroundimage,int width,int height);
	unsigned char* Gray2BW(unsigned char *imgData, int width, int height);
	void DrawRectangle(unsigned char *data,int x, int y, int step,int width,int height);
	void SearchFingerPoint(unsigned char *data,int nTempW, int nTempH , 
		int nTempMX, int nTempMY, int width,int height,vector<CPoint> *pointArray);
	
	void SearchFingerThining(unsigned char *imgData,int nWidth,int nHeight);		//hou jia

	bool SetImagePointTrace(unsigned char * data, int width, int height, vector<CPoint> pointArray,BYTE value);
	bool SetQUADPoint(unsigned char * data, int width, int height, CPoint point, byte value);
	
};

