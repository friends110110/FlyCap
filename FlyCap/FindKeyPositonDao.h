#pragma once
#include <vector>
#include "KeyLocation.h"
class CFindKeyPositonDao
{
public:
	CFindKeyPositonDao(void);
	~CFindKeyPositonDao(void);
	virtual bool RegionGrow(unsigned char *srcImg,int nwidth,int nheight)=0;
	virtual vector<CKeyLocation> *getLocationPosition()=0;
};

