#include "StdAfx.h"
#include "ImageProcess.h"


CImageProcess::CImageProcess(void)
{
}


CImageProcess::~CImageProcess(void)
{
}

unsigned char * CImageProcess::ImageMinus(LPBYTE destimag,LPBYTE backgroundimage,int width,int height)
{
	int nWidthBytes = ((width*8) + 31) / 32 * 4;
	unsigned char * data=new unsigned char [height*nWidthBytes];
// 	int **GrayMat=new int *[height];
// 	for(int i=0;i<height;i++)
// 	{
// 		GrayMat[i]=new int [nWidthBytes];
// 		memset(GrayMat[i],0,nWidthBytes);
// 	}
// 	//最大、最小灰度和值
// 	int nMax = -255;
// 	int nMin = 255; 
// 
// 	for(int i=0;i<height;i++)
// 	{
// 		for(int j=0;j<nWidthBytes;j++)
// 		{
// 			GrayMat[i][j]=destimag[i*nWidthBytes+j]-backgroundimage[i*nWidthBytes+j];
// 			if( GrayMat[i][j] > nMax)
// 				nMax = GrayMat[i][j];
// 			if( GrayMat[i][j] < nMin)
// 				nMin = GrayMat[i][j];
// 		}
// 	}
// 	//将GrayMat的取值范围重新归一化到[0, 255]
// 	int nSpan = nMax - nMin;
// 	BYTE bt;
// 	for(int i=0; i<height; i++)
// 	{
// 		for(int j=0; j<nWidthBytes; j++)
// 		{
// 			
// 			if(nSpan > 0)
// 				bt = ((BYTE)(abs(GrayMat[i][j])));//(GrayMat[i][j] - nMin)*255/nSpan;
// 			else if(GrayMat[i][j] <= 255)
// 				bt = GrayMat[i][j] ;
// 			else
// 				bt = 255;
// 
// 			data[i*nWidthBytes+j]=bt;		
// 
// 
// 		}// for j
// 	}// for i
// 	for(int i=0;i<height;i++)
// 		delete[](GrayMat[i]);
// 	delete[]GrayMat;

	BYTE bt;
	for(int i=0; i<height; i++)
	{
		for(int j=0; j<nWidthBytes; j++)
		{
			
			int span=((abs(destimag[i*nWidthBytes+j]-backgroundimage[i*nWidthBytes+j])));
			if (span<=255)
				bt = span;//(GrayMat[i][j] - nMin)*255/nSpan;
			else
				bt = 255;

			data[i*nWidthBytes+j]=bt;		


		}// for j
	}// for i
	return data;
}


unsigned char* CImageProcess::Gray2BW(unsigned char *imgData, int width, int height)
{

	if (imgData==NULL)
	{
		return NULL;
	}
	int rows=height;
	int cols=width;
	int x0=0,y0=0;
	int dx=width,dy=height;
	unsigned char *np;     // 图像指针
	int threshold=1; // 阈值
	int ihist[256];         // 图像直方图，256个点

	int i, j, k;       // various counters
	int  n1, n2, gmin, gmax;
	double m1, m2, sum, csum, fmax, sb;
	int n;
	// 对直方图置零...
	memset(ihist, 0, sizeof(ihist));

	gmin=255; gmax=0;
	// 生成直方图
	for (i = y0 + 1; i < y0 + dy - 1; i++) {
		np = &imgData[i*cols+x0+1];
		for (j = x0 + 1; j < x0 + dx - 1; j++) {
			ihist[*np]++;			//本身的统计量加1
			if(*np > gmax) gmax=*np;	//gmax，gmin统计最大和最小的像素值
			if(*np < gmin) gmin=*np;
			np++; /* next pixel */
		}
	}

	// set up everything
	sum = csum = 0.0;
	n = 0;

	for (k = 0; k <= 255; k++) {
		sum += (double) k * (double) ihist[k];   /* x*f(x) 质量矩*/
		n   += ihist[k];                           /* f(x)   质量   */
	}

	if (!n) {
		// if n has no value, there is problems...
		fprintf (stderr, "NOT NORMAL thresholdValue = 160\n");

	}

	// do the otsu global thresholding method
	fmax = -1.0;	//方差不可能为负，故意设定负的初始值
	n1 = 0;
	for (k = 0; k < 255; k++) {
		n1 += ihist[k];
		if (!n1) { continue; }
		n2 = n - n1;
		if (n2 == 0) { break; }
		csum += (double) k *ihist[k];
		m1 = csum / n1;
		m2 = (sum - csum) / n2;
		sb = (double) n1 *(double) n2 *(m1 - m2) * (m1 - m2);
		/* bbg: note: can be optimized. */
		if (sb > fmax) {
			fmax = sb;
			threshold = k;
		}
	}

	int ret;
	unsigned char temp;
	unsigned char r;
	int widthStep=(int)((width*8+31)/32)*4;



	threshold=128;
	//为dst图像分配数据空间
	unsigned char* dst =new unsigned char[widthStep*height];
	//(unsigned char*)malloc(widthStep*height*sizeof(unsigned char));
	memset(dst,0,widthStep*height);

	//计算灰度数据, 0表示纯黑，255表示纯白
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++,n++)
		{
			r=*(imgData+widthStep*(height-1-i)+j);
			if(r>threshold)
			{
				temp=(unsigned char)255;
			}
			else
			{
				temp=(unsigned char)0;
			}
			*(dst+widthStep*(height-1-i)+j)=temp;
		}
	}

	return dst;
}

void CImageProcess::DrawRectangle(unsigned char *data,int x, int y, int step,int width,int height)
{
// 
// 	for(int j = y - step; j < y + step + 1; j++)
// 	{
// 		for(int i = x - step; i < x + step + 1; i++)
// 		{
// 			if(j>0&&j<width&&i>0&&i<height)
// 				if(j == y - r || j == y + r || i == x - r || i == x + r)
// 				{
// 					data[i*width+j]=0;
// 				}
// 		}
// 	}
	int nWidth=((width*8)+31)/32*4;
	int x1=x-step;
	int y1=y-step;
	int x2=x+step;
	int y2=y+step;

	x1=x1<0?0:x1;
	x2=x2>=nWidth?nWidth-1:x2;
	y1=y1<0?0:y1;
	y2=y2>=height?height-1:y2;
// 	for (int i=x1;i<=x2;i++)
// 	{
// 		//data[y1*nWidth+i]=0;
// 		data[(height-y2)*nWidth+i]=0;
// 	}
// 	for(int i=y1;i<=y2;i++)
// 	{
// 		data[i*nWidth+x1]=0;
// 		data[i*nWidth+x2]=0;
// 	}
	for(int i=y1;i<=y2;i++)
		for (int j=x1;j<=x2;j++)
		{
			data[i*nWidth+j]=255;				//把框弄成白色
		}
}

void CImageProcess::SearchFingerPoint(unsigned char *data,int nTempW, int nTempH , 
	int nTempMX, int nTempMY, int width,int height,vector<CPoint> *pointArray)
{
	
	int nWidth=((width*8)+31)/32*4;
	int i,j;
	//扫描图像进行模板操作
	
		for(j=nTempMX; j<nWidth - (nTempW - nTempMX) + 1; j++)
		{
			for(i=nTempMY; i<height - (nTempH - nTempMY)+1; i++)						//先按列扫描
			{
			// (j,i)为中心点
			int fResult = 0;
			for(int l=0; l<nTempW; l++)
			{
				for(int k=0; k<nTempH; k++)
				{
					//计算加权和
					fResult += data[(i+k-nTempMY)*nWidth+j + l - nTempMX] ;
				}
			}

			if(fResult>=5*255)										//判断区域白点个数
			{
				DrawRectangle(data,j,i,50,width,height);
				pointArray->push_back(CPoint(j,i));
				return;
			}
		}//for j
	}//for i
	
}



bool CImageProcess::SetImagePointTrace(unsigned char * data, int width, int height, vector<CPoint> pointArray,BYTE value)
{
	int nWidth=((width*8)+31)/32*4;
	vector<CPoint>::iterator ite;
// 	for(int i=0;i<1024;i++)
// 	{
// 		data[512*nWidth+i]=value;
// 		data[513*nWidth+i]=value;
// 		data[514*nWidth+i]=value;
// 	}
	for(ite=pointArray.begin();ite<pointArray.end();ite++)
	{
		//data[ite->y*nWidth+ite->x]=value;
		DrawRectangle(data,ite->x,ite->y,10,width,height);
	}
	////////////////调试、、、、、、、、、、、、、、、、、、、、、、
	//////////////////////////////////////////////////////////////////////////
// 	 	vector<vector<int>> GrayMat;
// 	 
// 	 	for(int i=0;i<height;i++)
// 	 	{
// 	 		vector<int> a;	
// 	 		for(int j=0;j<nWidth;j++)
// 	 		{
// 	 			a.push_back(data[i*nWidth+j]);
// 	 
// 	 		}
// 	 		GrayMat.push_back(a);
// 	 	}
	//////////////////////////////////////////////////////////////////////////
//	DrawRectangle(data,200,200,50,width,height);
	
	return true;
}


bool CImageProcess::SetQUADPoint(unsigned char * data, int width, int height, CPoint point, byte value)
{
// 	int nWidth=((width*8)+31)/32*4;
// 	int x1=x-step;
// 	int y1=y-step;
// 	int x2=x+step;
// 	int y2=y+step;
// 
// 	x1=x1<0?0:x1;
// 	x2=x2>=nWidth?nWidth-1:x2;
// 	y1=y1<0?0:y1;
// 	y2=y2>=height?height-1:y2;
// 	for(int i=y1;i<=y2;i++)
// 		for (int j=x1;j<=x2;j++)
// 		{
// 			data[i*nWidth+j]=255;				//把框弄成白色
// 		}
	return false;
}


void CImageProcess::SearchFingerThining(unsigned char *imgData,int nWidth,int nHeight)
{
	//四个条件
	BOOL bCondition1;
	BOOL bCondition2;
	BOOL bCondition3;
	BOOL bCondition4;

	nWidth=((nWidth*8+31)/32)*4;  //行占的字节数
	//5×5相邻区域像素值
	unsigned char neighbour[5][5];

	int i, j;
	int m, n;

	BOOL bModified = TRUE;


	while(bModified)
	{		
		bModified = FALSE;
		unsigned char * pic = new unsigned char[nWidth*nHeight];
		memset(pic,0,sizeof(char)*nWidth*nHeight);

		for(j=2; j<nHeight-2; j++)
		{
			for(i=2; i<nWidth-2; i++)
			{				
				bCondition1 = FALSE;
				bCondition2 = FALSE;
				bCondition3 = FALSE;
				bCondition4 = FALSE;

				BYTE data = imgData[j+i*nWidth];
				if(data == 255)
					continue;


				// 获得当前点相邻的5×5区域内像素值，白色用0代表，黑色用1代表
				for (m = 0;m < 5;m++ )
				{
					for (n = 0;n < 5;n++)
					{
						neighbour[m][n] = (imgData[(i + n - 2)*nWidth+ j + m - 2] == 0);
					}
				}
				//				neighbour[][]
				//逐个判断条件。
				//判断2<=NZ(P1)<=6
				int nCount =  neighbour[1][1] + neighbour[1][2] + neighbour[1][3] \
					+ neighbour[2][1] + neighbour[2][3] + \
					+ neighbour[3][1] + neighbour[3][2] + neighbour[3][3];
				if ( nCount >= 2 && nCount <=6)
					bCondition1 = TRUE;

				//判断Z0(P1)=1
				nCount = 0;
				if (neighbour[1][2] == 0 && neighbour[1][1] == 1)
					nCount++;
				if (neighbour[1][1] == 0 && neighbour[2][1] == 1)
					nCount++;
				if (neighbour[2][1] == 0 && neighbour[3][1] == 1)
					nCount++;
				if (neighbour[3][1] == 0 && neighbour[3][2] == 1)
					nCount++;
				if (neighbour[3][2] == 0 && neighbour[3][3] == 1)
					nCount++;
				if (neighbour[3][3] == 0 && neighbour[2][3] == 1)
					nCount++;
				if (neighbour[2][3] == 0 && neighbour[1][3] == 1)
					nCount++;
				if (neighbour[1][3] == 0 && neighbour[1][2] == 1)
					nCount++;
				if (nCount == 1)
					bCondition2 = TRUE;

				//判断P2*P4*P8=0 or Z0(p2)!=1
				if (neighbour[1][2]*neighbour[2][1]*neighbour[2][3] == 0)
					bCondition3 = TRUE;
				else
				{
					nCount = 0;
					if (neighbour[0][2] == 0 && neighbour[0][1] == 1)
						nCount++;
					if (neighbour[0][1] == 0 && neighbour[1][1] == 1)
						nCount++;
					if (neighbour[1][1] == 0 && neighbour[2][1] == 1)
						nCount++;
					if (neighbour[2][1] == 0 && neighbour[2][2] == 1)
						nCount++;
					if (neighbour[2][2] == 0 && neighbour[2][3] == 1)
						nCount++;
					if (neighbour[2][3] == 0 && neighbour[1][3] == 1)
						nCount++;
					if (neighbour[1][3] == 0 && neighbour[0][3] == 1)
						nCount++;
					if (neighbour[0][3] == 0 && neighbour[0][2] == 1)
						nCount++;
					if (nCount != 1)
						bCondition3 = TRUE;
				}

				//判断P2*P4*P6=0 or Z0(p4)!=1
				if (neighbour[1][2]*neighbour[2][1]*neighbour[3][2] == 0)
					bCondition4 = TRUE;
				else
				{
					nCount = 0;
					if (neighbour[1][1] == 0 && neighbour[1][0] == 1)
						nCount++;
					if (neighbour[1][0] == 0 && neighbour[2][0] == 1)
						nCount++;
					if (neighbour[2][0] == 0 && neighbour[3][0] == 1)
						nCount++;
					if (neighbour[3][0] == 0 && neighbour[3][1] == 1)
						nCount++;
					if (neighbour[3][1] == 0 && neighbour[3][2] == 1)
						nCount++;
					if (neighbour[3][2] == 0 && neighbour[2][2] == 1)
						nCount++;
					if (neighbour[2][2] == 0 && neighbour[1][2] == 1)
						nCount++;
					if (neighbour[1][2] == 0 && neighbour[1][1] == 1)
						nCount++;
					if (nCount != 1)
						bCondition4 = TRUE;
				}

				if(bCondition1 && bCondition2 && bCondition3 && bCondition4)
				{
					pic[i*nWidth+ j]=255;
					bModified = TRUE;
				}
				else
				{
					pic[i*nWidth+j]=0;
				}

			} //for i
		} //for j
		delete []imgData;
		imgData=pic;
	}

}

