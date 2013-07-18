#pragma once
#include <vector>
#include "KeyLocation.h"
#include "FindKeyPositonDao.h"

class CFindKeyPositionDaoImpl:public CFindKeyPositonDao
{
public:
	CFindKeyPositionDaoImpl(void);
	~CFindKeyPositionDaoImpl(void);


	virtual vector<CKeyLocation> *getLocationPosition();
	virtual bool RegionGrow(unsigned char *srcImg,int nwidth,int nheight);

	static const int Blank_keyNum=16;
	static char keystrArray[Blank_keyNum];
	int keystrCount;


	//阈值颜色 64
	static const int m_thread=64;

	//存放15个按键的位置以及信息

private:
	bool RecFindSeed4RegionGrow(unsigned char *,unsigned char * pto,int nwidth,int nheight,int pxy[2],unsigned char nType);
	bool RecFindRegionWithSeed(unsigned char *,unsigned char *,int nwidth,int nheight,int pxy[2],unsigned char nType);



	bool O_Histeq(unsigned char *srcImage,int nwidth,int nheight);

	int O_DetectPixelRoundThreshold(unsigned char *srcImage,int i,int j,int tempx,int tempy);
	int O_DetectThreshold(unsigned char *srcImage,int nMaxIter, int &nDiffRet,int &numMax,int &numMin);			//牛顿迭代法 最大迭代次数和 亮暗差值
	int O_DetectOTSUMethod_OneDimension(unsigned char *srcImage,int nwidth,int nheight);
	bool O_Grayto2B(unsigned char *srcImage,unsigned char * srcImage2Bpto,int nwidth,int nheight);

	void O_Erode(unsigned char *,int se[3][3],int nwidth,int nheigh); //腐蚀算法
	void O_Dilate(unsigned char *,int se[3][3],int nwidth,int nheigh); //膨胀算法
	void O_Open( unsigned char *,int se[3][3],int nwidth,int nheigh);//开运算
	void O_Close(unsigned char *,int se[3][3],int nwidth,int nheigh);//闭运算

	inline int BytesWide(unsigned int num)
	{
		return((num*8) + 31) / 32 * 4;
	}


	vector<CKeyLocation> *m_keypositionArray;

	int 	keyfindlocationx;
	int  keyfindlocationy;
	//外边框不处理	
	static const int KeyOutsideBorderX=5;
	static const int KeyOutsideBorderY=8;

	//当找到一个种子点时，让此点的y步长加10再找 以防止  后面的按键y先被找到 而遗漏前面的按键
	static const int keyySearchStep=10;


	static const unsigned char backgroundcolor=0;
	static const unsigned char frontcolor=255;	
};

