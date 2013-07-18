#pragma once
class CPicAlgorithm
{
public:
	CPicAlgorithm(void);
	~CPicAlgorithm(void);
	unsigned char* RGB2Gray(unsigned char* imgData, int width, int height, int bitCount);
	unsigned char * Gray2BW(unsigned char *imgData, int width, int height);
	unsigned char* drawEmptyRect(unsigned char *imgData, int width, int height, int x, int y, int dx, int dy,int t);
	int getRect(unsigned char *imgData, int width, int height, int x, int y, int dx, int dy);




};

