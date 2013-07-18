#include "StdAfx.h"
#include "ImageDaoImpl.h"

 bool CImageDaoImpl::isfirstFlag=false;


long CImageDaoImpl::o_startCount=0;
long CImageDaoImpl::o_endCount=0;
char CImageDaoImpl::o_lastkey=0;
char CImageDaoImpl::keyclickedtemp=0;
//map<string,CRect> CImageDaoImpl::*mapArray=0;
CImageDaoImpl::CImageDaoImpl(void)
	: m_front_background_Pixelstatics(_T(""))
{
	showImageDrawRECflag=false;
	m_processimage=new Image;
	nWidthBytes=0;
	nHeightBytes=0;
	pointArray=new vector<CPoint>();
	//mapArray=new map<char,CRect>;
	keyclickedArray=new vector<char>;
	m_IsKeyRepeatFlag=0;
	m_backgroundUpdateDao=new CBackgroundUpdateDaoImpl;
}


CImageDaoImpl::~CImageDaoImpl(void)
{
	if (NULL!=m_processimage)
	{
		delete m_processimage;
	}
	if(0!=pointArray->size())
	{
		pointArray->empty();
		pointArray->~vector();

	}
// 	if (0!=mapArray->size())
// 	{
// 		mapArray->empty();
// 		mapArray->~map();
// 	}

	if(0!=keyclickedArray->size())
	{
		keyclickedArray->empty();
		keyclickedArray->~vector();
	}
	if (0!=m_backgroundUpdateDao)
	{
		delete m_backgroundUpdateDao;
	}
}

CImageDaoImpl::CImageDaoImpl(Image *image)
{
	m_processimage=new Image(*image);
	nWidthBytes=((image->GetCols()*8) + 31) / 32 * 4;
	nHeightBytes=image->GetRows();
	pointArray=new vector<CPoint>();
	//mapArray=new map<char,CRect>;
	keyclickedArray=new vector<char>;
	m_IsKeyRepeatFlag=0;
	
}
Image * CImageDaoImpl::GetImage()
{
	return m_processimage;
}
bool CImageDaoImpl::DeepCopy(Image *image)
{
	nWidthBytes=((image->GetCols()*8) + 31) / 32 * 4;
	nHeightBytes=image->GetRows();
	m_processimage->DeepCopy(image);
	m_imagedata=m_processimage->GetData();
	m_IsKeyRepeatFlag=0;

	return true;
}
bool CImageDaoImpl::SetImage(Image *image)
{
	
	m_processimage=image;
	nWidthBytes=((image->GetCols()*8) + 31) / 32 * 4;
	nHeightBytes=image->GetRows();
	m_IsKeyRepeatFlag=0;
	return true;
}
bool CImageDaoImpl::CaptureBackgroundImage()			//捕捉背景图片
{
	return true;
}
// bool CImageDaoImpl::Get2BImagePixels()
// {
// 		return true;
// }
// bool CImageDaoImpl::GetGRAYPixels()
// {
// 		return true;
// }
// bool CImageDaoImpl::GetProcessedPixels()
// {
// 		return true;
// }
// bool CImageDaoImpl::GetShowPixels()
// {
// 		return true;
// }
bool CImageDaoImpl::GetImageSize(int *width,int *height)
{
		*width=m_processimage->GetCols();
		*height=m_processimage->GetRows();
		return true;
}
bool CImageDaoImpl::SaveImage(char * filepath)
{
		m_processimage->Save(filepath);
		return true;
}
bool CImageDaoImpl::ConvertGray(Image *image)
{
	image->Convert(PIXEL_FORMAT_MONO8 ,m_processimage);
	nHeightBytes=m_processimage->GetRows();
	nWidthBytes=m_processimage->GetCols();
	//用于背景更新确立的图像长宽
	m_backgroundUpdateDao->SetWidth_Height(nWidthBytes,nHeightBytes);
	return true;
}
bool CImageDaoImpl::ImageMinus(Image *backgroundImage)
{
	if (backgroundImage->GetRows()!=nHeightBytes||BytesWide(backgroundImage->GetCols())!=nWidthBytes)
	{
		return false;
	}
	unsigned char *bacakgrounddata=backgroundImage->GetData();
	int processpixel,backgroundpixel,span;
	m_imagedata=this->m_processimage->GetData();
#ifdef WRITE_BACKGROUND_MINUS
	FILE *fp=fopen("backminus.txt","w+");
#endif	
	for (int i=0;i<nHeightBytes;i++)
	{
		for(int j=0;j<nWidthBytes;j++)
		{
			processpixel=*(m_imagedata+i*nWidthBytes+j);//getPixels(i,j);
			backgroundpixel=*(bacakgrounddata+i*nWidthBytes+j);
			span=processpixel-backgroundpixel;
// 			if (span<0)
// 			{
// 				//setPixels(i,j,0);
// 				*(m_imagedata+i*nWidthBytes+j)=0;
// 			}
// 			else
// 			{
// 				//setPixels(i,j,byte-*(bacakgrounddata+i*nWidthBytes+j));
// 				*(m_imagedata+i*nWidthBytes+j)=(byte)span;
// 			}
#ifdef WRITE_BACKGROUND_MINUS
			fprintf(fp,"%c",(byte)(abs(span)));
#endif
			*(m_imagedata+i*nWidthBytes+j)=(byte)(abs(span));
			//os<<(span)<<"  ";
		}
		// char str='\n';
		//fprintf(fp,"\n");
	}

#ifdef WRITE_BACKGROUND_MINUS
	fclose(fp);
#endif
	return true;
}
//传入裸数据图像，以及宽、长，和位深（24），返回灰度数据指针
bool  CImageDaoImpl::RGBtoGray()
{
	int n=0;
	int i,j;
	unsigned char r,g,b,gray;
	m_imagedata=m_processimage->GetData();
	unsigned char * dst =new unsigned char [nWidthBytes*nHeightBytes];
	memset(dst,0,nWidthBytes*nHeightBytes);

	unsigned int row,col,stride;
	PixelFormat ppixel;
	BayerTileFormat pbay;
	m_processimage->GetDimensions(&row,&col,&stride,&ppixel,&pbay);
	//计算灰度数据
	for(i=0;i<nHeightBytes;i++)
	{
		n=0;
		for(j=0;j<nWidthBytes*3;j++,n++)
		{
			b=*(m_imagedata+nWidthBytes*(nHeightBytes-1-i)+j);  //bmp中顺序是BGR
			j++;
			g=*(m_imagedata+nWidthBytes*(nHeightBytes-1-i)+j);
			j++;
			r=*(m_imagedata+nWidthBytes*(nHeightBytes-1-i)+j);
			gray=(r*19595 + g*38469 + b*7472) >> 16;
			*(dst+nWidthBytes*(nHeightBytes-1-i)+n)=gray;
		}
	}
	unsigned int x=m_processimage->GetBitsPerPixel(); 
	unsigned char *vv=m_processimage->GetData();
	m_processimage->ReleaseBuffer();

	if (vv!=NULL&&true==isfirstFlag)
	{
		delete []vv; 
	}
	isfirstFlag=true;
// 	m_processimage->~Image();
// 	m_processimage=new Image(row,col,0,dst,nWidthBytes*nHeightBytes,PIXEL_FORMAT_MONO8) ;
// 	m_processimage->Convert(PIXEL_FORMAT_MONO8 ,m_processimage);
// 	Error error=m_processimage->SetData(dst,nWidthBytes*nHeightBytes);

	m_imagedata=m_processimage->GetData();
	
	//m_processimage->SetDimensions(row,col,0,PIXEL_FORMAT_MONO8,NONE );

	unsigned int y=m_processimage->GetBitsPerPixel(); 
	PixelFormat dd=m_processimage->GetPixelFormat();
	//unsigned z=m_processimage->get
	m_processimage->Save("dafasdf.bmp");
	return true;
}
/**************************************************
int DetectThreshold(int nMaxIter, int &nDiffRet)

功能：
	利用迭代法自动确定阈值

参数：
	int nMaxIter：最大迭代次数
	int &nDiffRet：	使用给定阈值确定的的亮区与暗区平均灰度的差异值
返回值：
	int类型，算法所确定的阈值
***************************************************/

int CImageDaoImpl::DetectThreshold(int nMaxIter, int &nDiffRet,int &numMax,int &numMin)
{
	int nThreshold;

	nDiffRet = 0;

	// 直方图数组
	int nHistogram[256] = { 0 };
	int i, j;

	BYTE bt;
	
	int nMin = 255;
	int nMax = 0;
	
	m_imagedata=m_processimage->GetData();
	// 扫描图像,计算出最大、最小灰度和直方图
	for(j = 0; j < nHeightBytes; j ++)
	{
		for(i=0; i<nWidthBytes; i++)
		{
			bt =m_imagedata[j*nWidthBytes+i];

			if(bt < nMin)
				nMin = bt;
			if(bt > nMax)
				nMax = bt;

			nHistogram[bt] ++;

		}
	}
	numMin=nMin;												//得到最小值
	numMax=nMax;												//得到最大值
	int nTotalGray = 0; //灰度值的和
	int nTotalPixel = 0; //像素数的和
	int nNewThreshold = (nMax + nMin)/2; //初始阈值

	nDiffRet = nMax - nMin;

	if (nMax == nMin)
		nThreshold = nNewThreshold;

	else
	{
		nThreshold = 0;

		// 迭代开始,直到迭代次数达到100或新阈值与上一轮得到的阈值相等，迭代结束
		for(int nIterationTimes = 0; nThreshold != nNewThreshold && nIterationTimes < nMaxIter; nIterationTimes ++)
		{
			nThreshold = nNewThreshold;
			nTotalGray = 0;
			nTotalPixel = 0;

			//计算图像中小于当前阈值部分的平均灰度
			for(i=nMin; i<nThreshold; i++)
			{
				nTotalGray += nHistogram[i]*i;
				nTotalPixel += nHistogram[i];
			}
			int nMean1GrayValue = nTotalGray/nTotalPixel;


			nTotalGray = 0;
			nTotalPixel = 0;

			//计算图像中大于当前阈值部分的平均灰度
			for(i=nThreshold + 1; i<=nMax; i++)
			{
				nTotalGray += nHistogram[i]*i;
				nTotalPixel += nHistogram[i];
			}
			int nMean2GrayValue = nTotalGray/nTotalPixel;

			nNewThreshold = (nMean1GrayValue + nMean2GrayValue)/2; //计算出新的阈值
			nDiffRet = abs(nMean1GrayValue - nMean2GrayValue);
		}
	}

	return nThreshold;
}
/***************************************************************************** 
* 
* \函数名称： 
*   OneDimentionOtsu() 
* 
* \输入参数: 
*   pGrayMat:      二值图像数据 
*   width:         图形尺寸宽度 
*   height:        图形尺寸高度 
*   nTlreshold:    经过算法处理得到的二值化分割阈值 
* \返回值: 
*   无 
* \函数说明:实现灰度图的二值化分割——最大类间方差法（Otsu算法，俗称大津算法） 
* 
****************************************************************************/  
int CImageDaoImpl::DetectOTSUMethod_OneDimension()
{
	
// 	double nHistogram[256];         //灰度直方图  
// 	double dVariance[256];          //类间方差  
// 	int N = nHeightBytes*nWidthBytes;           //总像素数  
// 	int nThreshold;
// 	int i;
// 	m_imagedata=m_processimage->GetData();
// 	for(int i=0; i<256; i++)  
// 	{  
// 		nHistogram[i] = 0.0;  
// 		dVariance[i] = 0.0;  
// 	}  
// 	for(i=0; i<nHeightBytes; i++)  
// 	{  
// 		for(int j=0; j<nWidthBytes; j++)  
// 		{  
// 			unsigned char nData = m_imagedata[i*nWidthBytes+j];//(unsigned char)cvmGet(pGrayMat, i, j);  
// 			nHistogram[nData]++;     //建立直方图  
// 		}  
// 	}  
// 	double Pa=0.0;      //背景出现概率  
// 	double Pb=0.0;      //目标出现概率  
// 	double Wa=0.0;      //背景平均灰度值  
// 	double Wb=0.0;      //目标平均灰度值  
// 	double W0=0.0;      //全局平均灰度值  
// 	double dData1=0.0,  dData2=0.0;  
// 	for(i=0; i<256; i++)     //计算全局平均灰度  
// 	{  
// 		nHistogram[i] /= N;  
// 		W0 += i*nHistogram[i];  
// 	}  
// 	for(i=0; i<256; i++)     //对每个灰度值计算类间方差  
// 	{  
// 		Pa += nHistogram[i];  
// 		Pb = 1-Pa;  
// 		dData1 += i*nHistogram[i];  
// 		dData2 = W0-dData1;  
// 		Wa = dData1/Pa;  
// 		Wb = dData2/Pb;  
// 		dVariance[i] = (Pa*Pb* pow((Wb-Wa), 2));  
// 	}  
// 	//遍历每个方差，求取类间最大方差所对应的灰度值  
// 	double temp=0.0;  
// 	for(i=0; i<256; i++)  
// 	{  
// 		if(dVariance[i]>temp)  
// 		{  
// 			temp = dVariance[i];  
// 			nThreshold = i;  
// 		}  
// 	}  
// 	return nThreshold;
// 	

	int width = nWidthBytes;  
	int height = nHeightBytes;  
	int pixelCount[256];
	float pixelPro[256];  
	int i,j,pixelSum,threshold;  
	pixelSum= width * height;
	threshold=0;
	m_imagedata=m_processimage->GetData();
	  
		for(i = 0; i < 256; i++)  
		{  
			    pixelCount[i] = 0;  
			    pixelPro[i] = 0;  
		}  
		//统计灰度级中每个像素在整幅图像中的个数  
		for(i = 0; i < height; i++)  
		{  
			for(j = 0;j < width;j++)  
			{  
				pixelCount[(int)m_imagedata[i * width + j]]++;  
			}  
		}  

		//计算每个像素在整幅图像中的比例  
		for(i = 0; i < 256; i++)  
		{  
			pixelPro[i] = (float)pixelCount[i] / pixelSum;  
		}  
	  
		//遍历灰度级[0,255]  
		float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;  
		for(i = 0; i < 256; i++)  
		{  
			w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;  
			for(j = 0; j < 256; j++)  
				{  
					if(j <= i)   //背景部分  
					{  
						w0 += pixelPro[j];  
						u0tmp += j * pixelPro[j];  
					}  
					else   //前景部分  
					{  
						w1 += pixelPro[j];  
						u1tmp += j * pixelPro[j];  
					}  
				}  
			u0 = u0tmp / w0;  
			u1 = u1tmp / w1;  
			u = u0tmp + u1tmp;  
			deltaTmp =  w0 * (u0 - u)*(u0-u) + w1 *(u1 - u)*(u1-u);  
			if(deltaTmp > deltaMax)  
			{  
				deltaMax = deltaTmp;  
				threshold = i;  
			}  
		}
	return threshold;

}
/***************************************************************************** 
* 
* \函数名称： 
*   TwoDimentionOtsu() 
* 
* \输入参数: 
*   pGrayMat:      二值图像数据 
*   width:         图形尺寸宽度 
*   height:        图形尺寸高度 
*   nTlreshold:    经过算法处理得到的二值化分割阈值 
* \返回值: 
*   无 
* \函数说明:实现灰度图的二值化分割——最大类间方差法（二维Otsu算法） 
* \备注：在构建二维直方图的时候，采用灰度点的3*3邻域均值 
******************************************************************************/  
int CImageDaoImpl::DetectThreshOTSUMethod_TwoDimension()
{
	double dHistogram[256][256];        //建立二维灰度直方图  
    double dTrMatrix = 0.0;             //离散矩阵的迹  
    int N = nHeightBytes*nWidthBytes;               //总像素数  
	int nThreshold;
	m_imagedata=m_processimage->GetData();
	int i,j,n,m;
    for( i=0; i<256; i++)  
    {  
        for(int j=0; j<256; j++)  
            dHistogram[i][j] = 0.0;      //初始化变量  
    }  
    for(i=0; i<nHeightBytes; i++)  
    {  
        for( j=0; j<nWidthBytes; j++)  
        {  
            unsigned char nData1 = m_imagedata[i*nWidthBytes+j];//(unsigned char)cvmGet(pGrayMat, i, j);  //当前的灰度值  
            unsigned char nData2 = 0;  
            int nData3 = 0;         //注意9个值相加可能超过一个字节  
            for(int m=i-1; m<=i+1; m++)  
            {  
                for( n=j-1; n<=j+1; n++)  
                {  
                    if((m>=0)&&(m<nHeightBytes)&&(n>=0)&&(n<nWidthBytes))  
                        nData3 += m_imagedata[m*nWidthBytes+n];//(unsigned char)cvmGet(pGrayMat, m, n); //当前的灰度值  
                }  
            }  
            nData2 = (unsigned char)(nData3/9);    //对于越界的索引值进行补零,邻域均值  
            dHistogram[nData1][nData2]++;  
        }  
    }  
    for(i=0; i<256; i++)  
        for(int j=0; j<256; j++)  
            dHistogram[i][j] /= N;  //得到归一化的概率分布  
   
    double Pai = 0.0;      //目标区均值矢量i分量  
    double Paj = 0.0;      //目标区均值矢量j分量  
    double Pbi = 0.0;      //背景区均值矢量i分量  
    double Pbj = 0.0;      //背景区均值矢量j分量  
    double Pti = 0.0;      //全局均值矢量i分量  
    double Ptj = 0.0;      //全局均值矢量j分量  
    double W0 = 0.0;       //目标区的联合概率密度  
    double W1 = 0.0;       //背景区的联合概率密度  
    double dData1 = 0.0;  
    double dData2 = 0.0;  
    double dData3 = 0.0;  
    double dData4 = 0.0;   //中间变量  
    int nThreshold_s = 0;  
    int nThreshold_t = 0;  
    double temp = 0.0;     //寻求最大值  
    for(i=0; i<256; i++)  
    {  
        for(int j=0; j<256; j++)  
        {  
            Pti += i*dHistogram[i][j];  
            Ptj += j*dHistogram[i][j];  
        }  
    }  
    for(i=0; i<256; i++)  
    {  
        for(int j=0; j<256; j++)  
        {  
            W0 += dHistogram[i][j];  
            dData1 += i*dHistogram[i][j];  
            dData2 += j*dHistogram[i][j];  
  
            W1 = 1-W0;  
            dData3 = Pti-dData1;  
            dData4 = Ptj-dData2;  
/*          W1=dData3=dData4=0.0;   //对内循环的数据进行初始化 
            for(int s=i+1; s<256; s++) 
            { 
                for(int t=j+1; t<256; t++) 
                { 
                    W1 += dHistogram[s][t]; 
                    dData3 += s*dHistogram[s][t];  //方法2 
                    dData4 += t*dHistogram[s][t];  //也可以增加循环进行计算 
                } 
            }*/  
  
            Pai = dData1/W0;  
            Paj = dData2/W0;  
            Pbi = dData3/W1;  
            Pbj = dData4/W1;   // 得到两个均值向量，用4个分量表示  
            dTrMatrix = ((W0*Pti-dData1)*(W0*Pti-dData1)+(W0*Ptj-dData1)*(W0*Ptj-dData2))/(W0*W1);  
            if(dTrMatrix > temp)  
            {  
                temp = dTrMatrix;  
                nThreshold_s = i;  
                nThreshold_t = j;  
            }  
        }  
    }  
    nThreshold = nThreshold_t;   //返回结果中的灰度值  
    //nThreshold = 100;  
	return nThreshold;
}
/**************************************************
int DetectThresholdMethod(int nMaxIter, int &nDiffRet)

功能：
	利用像素周围领域均值确定阈值，所以每个像素的阈值都不一样

参数：
	unsigned char *srcImage：原值
	int x,int y         : 位置点
	int tempx,int tempy ：	模版大小
返回值：
	int类型，算法所确定的阈值
***************************************************/
int CImageDaoImpl::DetectPixelRoundThreshold(unsigned char *srcImage,int x,int y,int tempx,int tempy)
{
	if (y==0||y==nHeightBytes-1||x==0||x==nWidthBytes-1)
	{
		return 0;
	}
	int i,j;
	double result=0;
	for( i=-tempy/2;i<=tempy/2;i++)
		for(j=-tempx/2;j<=tempx/2;j++)
		{
			result+=*(srcImage+(y+i)*nWidthBytes+(x+j));
		}
	result/=tempx*tempy;
	result-=10;
	return result;
}
bool CImageDaoImpl::ConvertGray2BW()
{
	//unsigned char * srcImage=new unsigned char[nWidthBytes*nHeightBytes];
	m_imagedata=m_processimage->GetData();
	//memcpy(srcImage,m_imagedata,nWidthBytes*nHeightBytes);
	int diff;
	int threshnew,nummax, nummin;
	int otsuThresh;
// 	ofstream of("time.txt",ios::app);
// 	of.seekp(0,ios::end);
//	DWORD start1=GetTickCount();
//	int threshold=DetectThreshold(100,diff,nummax,nummin);
//	DWORD end1=GetTickCount();

	//of<<threshold<<"   "<<diff<<"   "<<nummax<<"   "<<nummin<<endl;
//	of.close();
	   
	//			otsuThresh=DetectThreshOTSUMethod_TwoDimensional();
//	DWORD start2=GetTickCount();
	otsuThresh=DetectOTSUMethod_OneDimension();     
	otsuThresh=otsuThresh<m_threshold?m_threshold:otsuThresh;
//	DWORD end2=GetTickCount();
//	of<<end1-start1<<"   "<<end2-start2<<"\n";
//	of.close();
	int bitdata;
	m_frontColorCount=0;
	m_backgroundColorCount=0;
	//计算灰度数据, 0表示纯黑，255表示纯白
	for(int i=0;i<nHeightBytes;i++)
	{
		for(int j=0;j<nWidthBytes;j++)
		{
			bitdata=*(m_imagedata+i*nWidthBytes+j);//getPixels(i,j);
		//	threshnew = DetectThresholdMethod(m_imagedata,j,i,3,3);
			//if(bitdata>=threshnew&&bitdata<=threshold)//threshold)
			//if(bitdata>threshnew)
// 			if (threshold<m_threshold)
// 			{
// 				threshold=m_threshold;
// 			}
			//of<<bitdata<<" ";
			//(threshold=threshold<m_threshold?m_threshold:threshold)
		//	if(bitdata<=threshold)
			
			//otsuThresh=30;
			if (bitdata<otsuThresh)
			{
				*(m_imagedata+i*nWidthBytes+j)=backgroundcolor;//setPixels(i,j,255);	
				//如果想把前景物变成背景物
				//这里需要把当前二值化图像与之前的二值化图像的对应像素点想比较，如果相等则sTime数组加1，不相等sTime就置为0 累计到一定值后此点像素更新、
				//if(*(m_imagedata+i*nWidthBytes+j) == *(m_last2Bimagedata+i*nWidthBytes+j))
				// sTime[x][y]++
				//else 
				//sTime[x][y]=0
			//	m_backgroundUpdateDao->CountImageUpdatePixels(i,j,1);
				m_backgroundColorCount++;
			}
			else
			{
				*(m_imagedata+i*nWidthBytes+j)=frontcolor;//setPixels(i,j,0);
				//如果想把前景物变成背景物
				//这里需要把当前二值化图像与之前的二值化图像的对应像素点想比较，如果相等则sTime数组加1，不相等sTime就置为0 累计到一定值后此点像素更新、
				//if(*(m_imagedata+i*nWidthBytes+j) == *(m_last2Bimagedata+i*nWidthBytes+j))
				// sTime[x][y]++
				//else 
				//sTime[x][y]=0
			//	m_backgroundUpdateDao->CountImageUpdatePixels(i,j,0);
				m_frontColorCount++;
			
			}
		}
		//of<<"\n\n";
	}
	//of.close();
	//delete []srcImage;

	m_front_background_Pixelstatics.Format("b %d  f %d",m_backgroundColorCount,m_frontColorCount);
	//AfxMessageBox(str);
	return true;
}
int CImageDaoImpl::BytesWide(unsigned int num)
{
	return((num*8) + 31) / 32 * 4;
}
bool CImageDaoImpl::DrawRectangle(unsigned char *lpdata,int x, int y,int step,COLORREF color )
{
	int x1=x-step;
	int y1=y-step;
	int x2=x+step;
	int y2=y+step;
	unsigned char b,g,r;
	x1=x1<0?0:x1;
	x2=x2>=nWidthBytes?nWidthBytes-1:x2;
	y1=y1<0?0:y1;
	y2=y2>=nHeightBytes?nHeightBytes-1:y2;
 	if(m_imagedata==lpdata)
 	{
		for(int i=y1;i<=y2;i++)
		{
			*(m_imagedata+i*nWidthBytes+x1)=frontcolor;
			*(m_imagedata+i*nWidthBytes+x2)=frontcolor;
		}
		for (int j=x1;j<=x2;j++)
		{
			*(m_imagedata+y1*nWidthBytes+j)=frontcolor;
			*(m_imagedata+y2*nWidthBytes+j)=frontcolor;
		}
	}
	else 
	{
		b=color>>16;					
		g=color>>8;
		r=color>>0;
		for(int i=y1;i<=y2;i++)
		{
			*(lpdata+i*nWidthBytes*3+x1*3)=b;
			*(lpdata+i*nWidthBytes*3+x1*3+1)=g;
			*(lpdata+i*nWidthBytes*3+x1*3+2)=r;
			*(lpdata+i*nWidthBytes*3+x2*3)=b;
			*(lpdata+i*nWidthBytes*3+x2*3+1)=g;
			*(lpdata+i*nWidthBytes*3+x2*3+2)=r;
		}
		for (int j=x1;j<=x2;j++)
		{
			*(lpdata+y1*nWidthBytes*3+j*3)=b;
			*(lpdata+y1*nWidthBytes*3+j*3+1)=g;
			*(lpdata+y1*nWidthBytes*3+j*3+2)=r;
			*(lpdata+y2*nWidthBytes*3+j*3)=b;
			*(lpdata+y2*nWidthBytes*3+j*3+1)=g;
			*(lpdata+y2*nWidthBytes*3+j*3+2)=r;
		}
	}
// 		for(int i=y1;i<=y2;i++)
// 			for (int j=x1;j<=x2;j++)
// 			{
// 				*(m_imagedata+i*nWidthBytes+j)=frontcolor;//255;//setPixels(i,j,255);;				//把框弄成白色
// 			}
		
//	}
// 	else
// 	{
// 		for(int i=y1;i<=y2;i++)
// 			for (int j=x1;j<=x2;j++)
// 			{
// 				*(lpdata+i*nWidthBytes*3+j*3)=frontcolor;//setPixels(i,j,255);;				//把框弄成白色
// 				*(lpdata+i*nWidthBytes*3+j*3+1)=frontcolor;
// 				*(lpdata+i*nWidthBytes*3+j*3+2)=frontcolor;
// 			}
// 	}
		return true;
}
CString CImageDaoImpl::GetHasClickedKey()
{
	if (0==keyclickedArray->size())
	{
		return  CString();
	}
	char *data=new char[keyclickedArray->size()];
	memset(data,0,keyclickedArray->size());
	vector<char>::iterator ite=keyclickedArray->begin();
	for (int i=0;ite<keyclickedArray->end();ite++)
	{
		data[i]=*ite;
		i++;
	}
	CString str(data,keyclickedArray->size());
	delete []data;
	return str;
}

 bool CImageDaoImpl::ClearHasClickedKey()
{
	keyclickedArray->clear();
	return true;
}
bool CImageDaoImpl::IsFingerSkin(unsigned char *lpdata,int i,int j)
{
	unsigned char r,g,b;
	unsigned char y,cb,cr;
	b=lpdata[i*nWidthBytes*3+j*3];
	g=lpdata[i*nWidthBytes*3+j*3+1];
	r=lpdata[i*nWidthBytes*3+j*3+2];

	//另一个标准
	// 		y=(unsigned char)(r * 0.256789 + g * 0.504129 + b * 0.097906)+ 16;
	// 		cb= (unsigned char)(r *(-0.148223) + g *(-0.290992) + b * 0. 439215)+ 128;
	// 		cr=(unsigned char)(r * 0.439215 + g *(-0.367789) + b *(-0.071426))+ 128;    133<=cr<=173  77<=cb<=127


	y=(unsigned char)(0.2990*r+0.5870*g+0.1140*b);
	cb=(unsigned char)(-0.1687*r-0.3313*g+0.5000*b+128);
	cr=(unsigned char)(0.5000*r-0.4187*g-0.0813*b+128);
	//if (133<=cr&&cr<=173&&77<=cb&&cb<=127 )				//cb   90 120    cr 135  168      77<Cb<127,133<Cr<137
	//if (135<=cr&&cr<=168&&90<=cb&&cb<=120)
	if (133<=cr&&cr<=137&&77<=cb&&cb<=127)
	{
		return true;
	}
	return false;
}
 bool CImageDaoImpl::SearchFingerPoint(unsigned char * lpdata,int nTempW, int nTempH,int nTempMX, int nTempMY,vector<CKeyLocation> *keyVector)
{
	int i,j;
	int searchStartLocationY=10;
	//扫描图像进行模板操作
	if (keyVector==NULL||keyVector->size()==0)
	{
		//AfxMessageBox("keyVector 不应该为0");
		//return false;
	}
	else
	{
		searchStartLocationY=keyVector->at(0).y1+10;
	}
	//从拨号键盘的  分割线 y以下开始搜索  10为 向下分割
	for(i=nTempMY+searchStartLocationY; i<nHeightBytes - (nTempH - nTempMY)+1; i++)		
	{
		for(j=nTempMX; j<nWidthBytes - (nTempW - nTempMX) + 1; j++)											//先按行扫描
		{
			// (j,i)为中心点
			int fResult = 0;
			for(int l=0; l<nTempW; l++)
			{
				for(int k=0; k<nTempH; k++)
				{
					//计算加权和
					fResult += *(m_imagedata+(i+k-nTempMY)*nWidthBytes+j + l - nTempMX);//getPixels(i+k-nTempMY,j + l - nTempMX);
				}
			}
			//找到手指指尖点
			if(fResult>=thredFingerCount*frontcolor)//&&true==IsFingerSkin(lpdata,i,j))										//判断区域白点个数   框架搭好只欠肤色检验
			{
				showImageDrawRECflag=true;     
				m_imagedata=m_processimage->GetData();
				DrawRectangle(m_imagedata,j,i,Rectanglstep,frontcolor);
				
				//直接对当前点进行判断

				pointArray->push_back(CPoint(j,i));


				//处理按键信息
				char ch=O_IskeyClicked(j,i,keyVector);
				//string showText="the clicked number is";
				// showText+=ch;
				if(ch!=NULL)
				{
					if (o_startCount==0)
					{
						o_startCount=GetTickCount();
						o_lastkey=ch;
					}
					else
					{
						if (ch!=o_lastkey)
						{
							o_startCount=GetTickCount();
							o_lastkey=ch;
							
						}    
						else
						{
							o_endCount=GetTickCount();
							//确定按了按个哪个键，按键延迟2秒					/////////////////////////////////////////////
							if (o_endCount-o_startCount>=500)			
							{
								//
								if (ch==m_IsKeyRepeatFlag)
								{
									o_startCount=0;
									o_lastkey=0;
									o_endCount=0;
									return true;
								}
								m_IsKeyRepeatFlag=ch;
								O_resolution(ch);
								Beep(1000,500);			//叫500毫秒
								//这里可以对要投影的数据图像文件 
								//先图像置为开始状态      然后那个按键位置进行 圈圈
								//也可以加入 已经按键的数字 在图片上
								//辅助数据置为0
								o_startCount=0;
								o_lastkey=0;
								o_endCount=0;
							}
						}
					}
					// AfxMessageBox(showText.c_str());  
				}


				return true;
			}
		}//for j
	}//for i
	return true;
}


char CImageDaoImpl::O_IskeyClicked(int x,int y,vector<CKeyLocation> *keyArray)
{
	if (keyArray==NULL||keyArray->size()==0)
	{
		return NULL;
	}
	int x0,x1,y0,y1;
	for (int i=1;i<keyArray->size();i++)
	{
		x0=keyArray->at(i).x0;
		x1=keyArray->at(i).x1;
		y0=keyArray->at(i).y0;
		y1=keyArray->at(i).y1;

		if (x<x1&&x>x0&&y<y1&&y>y0)
		{
			//....................
			if (m_IsKeyRepeatFlag!=keyArray->at(i).key)
			{
				m_IsKeyRepeatFlag=0;  //置位
			}
			//............................
			return	keyArray->at(i).key;
		}
	}
	//....................................
	m_IsKeyRepeatFlag=0;		//置位
	//..............................
	return NULL;
}
bool CImageDaoImpl::O_resolution(char key)
{
	switch(key)
	{
	case 'B':
		if (0==keyclickedArray->size())
		{
			AfxMessageBox("不能退格，因为还没有数据");
		}
		else
			keyclickedArray->pop_back();
		//keyclickedtemp=0;
		break;
	case 'C':
		AfxMessageBox("执行打电话操作");
		//keyclickedtemp=0;
		break;
	case 'Y':
// 		if (keyclickedtemp!=0)
// 		{
// 			keyclickedArray->push_back(keyclickedtemp);
// 		}
// 		keyclickedtemp=0;
		break;
	default:
		//keyclickedtemp=key;
		keyclickedArray->push_back(key);
		break;
	}
	return true;
}
 bool CImageDaoImpl::SetImagePointTrace(unsigned char *lpdata)   
 {    
	 vector<CPoint>::iterator ite,lastite;
	 unsigned char b,g,r;
	 if (pointArray->size()==0)
	 {
		 return true;
	 }
	 ite=pointArray->begin();
	 if (pointArray->size()==1)
	 {
		 m_imagedata=m_processimage->GetData();
		 b=pointColor>>16;					
		 g=pointColor>>8;
		 r=pointColor>>0;
		 lpdata[ite->y*nWidthBytes*3+ite->x*3]=b;
		 lpdata[ite->y*nWidthBytes*3+ite->x*3+1]=g;
		 lpdata[ite->y*nWidthBytes*3+ite->x*3+2]=r;
		 return true;
	 }
	 lastite=ite;
	 for(ite++;ite<pointArray->end();ite++)
	 {
		 //data[ite->y*nWidth+ite->x]=value;
		// DrawRectangle(lpdata,ite->x,ite->y,10);
		 BresenhamDrawline(lpdata,lastite->x,lastite->y,ite->x,ite->y,3,frontcolor);
		 lastite=ite;
	 }
	 //////////////////////////////////////////////////////////////////////////
	 // draw the last bitmap finger position
	 if (true==showImageDrawRECflag)
	 {
		 DrawRectangle(lpdata,lastite->x,lastite->y,Rectanglstep,RGB(0,255,0));
		 showImageDrawRECflag=false;
	 }
	
	     
	 return true;
 }
 /*******************
 int CImgProcess::GetMedianValue(int * pAryGray, int nFilterLen)
 
 功能：采用冒泡法对数组进行排序，并返回数组元素的中值。

 参数:
	int * pAryGray：要排序提取中值的数组
    int nFilterLen：数组长度
 
 返回值:
    int 中值
 
*******************/

int CImageDaoImpl::GetMedianValue(int * pAryGray, int nFilterLen)
{
	int i, j;	
	int nMedianValue;
	int nTemp; //中间变量
	
	//排序
	for (j=0; j < nFilterLen - 1; j++)
	{
		for (i=0; i < nFilterLen - j - 1; i++)
		{
			if (pAryGray[i] > pAryGray[i + 1])
			{
				// 交换位置
				nTemp = pAryGray[i];
				pAryGray[i] = pAryGray[i + 1];
				pAryGray[i + 1] = nTemp;
			}//if
		}//for i
	}//for j
	
	// 计算中值
	if ((nFilterLen & 1) > 0)
	{
		// 数组有奇数个元素，返回中间一个元素
		nMedianValue = pAryGray[(nFilterLen + 1) / 2];
	}
	else
	{
		// 数组有偶数个元素，返回中间两个元素平均值
		nMedianValue = (pAryGray[nFilterLen / 2] + pAryGray[nFilterLen / 2 + 1]) / 2;
	}
	
	// 返回中值
	return nMedianValue;
}

 bool CImageDaoImpl::AdaptiveMedianFilter(int nFilterH, int nFilterW, int nFilterMY, int nFilterMX)
 {
	 
	 int i, j, k, l;
	 unsigned char *data=new unsigned char[nWidthBytes*nHeightBytes]; 
	 memset(data,0,nWidthBytes*nHeightBytes);
	 byte nGray;

	 int* pAryGray; //邻域像素数组
	 pAryGray = new int[nFilterH * nFilterW];

	 // 逐行扫描图像，进行中值滤波	
	 for(i = nFilterMY; i < nHeightBytes - nFilterH + nFilterMY + 1; i++)// 行(除去边缘几行)
	 {
		 for(j = nFilterMX; j < nWidthBytes - nFilterW + nFilterMX + 1; j++)// 列(除去边缘几列)
		 {
			 // 读取滤波器数组
			 for (k = 0; k < nFilterH; k++)
			 {
				 for (l = 0; l < nFilterW; l++)
				 {
					 //原图像第i + k - nFilterMY行，第j + l - nFilterMX列的像素值
					 nGray = m_imagedata[ (i + k -nFilterMY)*nWidthBytes+j + l - nFilterMX];					

					 // 保存象素值
					 pAryGray[k * nFilterW + l] = nGray;
				 }//l
			 }//k

			 nGray = GetMedianValue(pAryGray, nFilterH * nFilterW); //通过排序获取中值

			 //判断当前像素是否是邻域的极大或极小值
			 if( (m_imagedata[i*nWidthBytes+j] == pAryGray[0]) || (m_imagedata[i*nWidthBytes+j] == pAryGray[nFilterH * nFilterW - 1]) )
				 data[ i*nWidthBytes+j]=nGray; //以中值作为响应
			 else
				 data[ i*nWidthBytes+j]=m_imagedata[i*nWidthBytes+j]; //不是极值则不改变原图像的值
		 }//j
	 }//i
	 unsigned char *vv=m_processimage->GetData();
	 m_processimage->ReleaseBuffer();

	 if (vv!=NULL&&true==isfirstFlag)
	 {
		
		 delete []vv; 
	 }
	 Error error=m_processimage->SetData(data,nWidthBytes*nHeightBytes);
	 m_imagedata=m_processimage->GetData();
	 delete [] pAryGray;

	  isfirstFlag=true;
	 return true;
 }



 

/******************* 
void CImgProcessProcessing::Erode(CImgProcess* pTo, int se[3][3])
 
 功能：
	3*3结构元素的二值图像腐蚀运算
 注：
	只能处理2值图象

 参数：
	ImgProcess* pTo: 目标输出图像的 CImgProcess 指针
	se[3][3]: 3*3的结构元素，其数组元素的合法取值为: 
		1 --- 前景
		0 --- 背景
	   -1 --- 不关心	 
 返回值：
	无	
*******************/
void CImageDaoImpl::Erode(int se[3][3])
{


	int i, j; //图像循环变量
	int k, l; //结构元素循环变量

	BOOL bMatch; //结构元素是否与局部图像匹配

	unsigned char *imagedata=new unsigned char[nHeightBytes*nWidthBytes];
	//memset(imagedata,255,nHeightBytes*nWidthBytes); //清空目标输出图像
	memset(imagedata,backgroundcolor,nHeightBytes*nWidthBytes);
	//逐行扫描图像，为防止访问越界，四周留出一个像素宽的空边
	for(i=1; i<nHeightBytes-1; i++)
	{
		for(j=1; j<nWidthBytes-1; j++)
		{
			//由于使用的是3*3的结构元素，为防止越界，不处理最上和最下的两行像素以及最左和最右的两列像素
			bMatch = true;
			for(k=0; k<3; k++)
			{
				for(l=0; l<3; l++)
				{
					if( se[k][l] == -1 ) //不关心
						continue;

					if( se[k][l] == 1 ) //前景
					{
						//if( m_imagedata[(i-1+k)*nWidthBytes+j-1+l] != backgroundcolor )
						if( m_imagedata[(i-1+k)*nWidthBytes+j-1+l] != frontcolor )
						{
							bMatch = false;
							break;
						}
					}
					else if( se[k][l] == 0 ) //背景
					{
						if(  m_imagedata[(i-1+k)*nWidthBytes+j-1+l] != backgroundcolor )
						{
							bMatch = false;
							break;
						}
					}
					else
					{
						AfxMessageBox("结构元素含有非法值！请检查后重新设定。");
						return;
					}
					
				}//for l
			}//for k
			
			if( bMatch )
				//imagedata[i*nWidthBytes+j]=backgroundcolor;
				imagedata[i*nWidthBytes+j]=frontcolor;
				

		}// for j
	}// for i
	unsigned char *vv=m_processimage->GetData();
	m_processimage->ReleaseBuffer();

	if (vv!=NULL&&true==isfirstFlag)
	{
		delete []vv; 
	}
	Error error=m_processimage->SetData(imagedata,nWidthBytes*nHeightBytes);
	m_imagedata=m_processimage->GetData();

	 isfirstFlag=true;
}


/******************* 
void CImgProcess::Dilate(CImgProcess* pTo, int se[3][3])
 
 功能：
	3*3结构元素的二值图像膨胀运算
 注：
	只能处理2值图象

 参数：
	Image* pTo: 目标输出图像的 CImgProcess 指针
	se[3][3]: 3*3的结构元素，其数组元素的合法取值为: 
		1 --- 前景
	   -1 --- 不关心
	 
 返回值：
	无
	
*******************/
void CImageDaoImpl::Dilate( int se[3][3])
{


	int i, j; //图像循环变量
	int k, l; //结构元素循环变量


	//计算se关于中心的对称集
	int nTmp;
	for(i=0; i<2; i++)
	{
		for(j=0; j<3-i; j++)
		{
			nTmp = se[i][j];
			se[i][j] = se[2-i][2-j];
			se[2-i][2-j] = nTmp;
		}
	}
	unsigned char *imagedata=new unsigned char[nWidthBytes*nHeightBytes];
	memset(imagedata,backgroundcolor,nWidthBytes*nHeightBytes);

	//逐行扫描图像，为防止访问越界，四周留出一个像素宽的空边
	for(i=1; i<nHeightBytes-1; i++)
	{
		for(j=1; j<nWidthBytes-1; j++)
		{
			//由于使用的是3*3的结构元素，为防止越界，不处理最上和最下的两行像素以及最左和最右的两列像素

			for(k=0; k<3; k++)
			{
				for(l=0; l<3; l++)
				{
					if( se[k][l] == -1 ) // 不关心
						continue;

					if( se[k][l] == 1 )
					{
						//if( m_imagedata[(i-1+k)*nWidthBytes+j-1+l] ==  0)
						if( m_imagedata[(i-1+k)*nWidthBytes+j-1+l] ==  frontcolor)
						{
							//原图中对应结构元素的局部区域有一点为1，就将目标图像对应于结构元素中心的像素置0
							//imagedata[i*nWidthBytes+j]=0;
							imagedata[i*nWidthBytes+j]=frontcolor;
							break;
						}
					}
					else
					{
						AfxMessageBox("结构元素含有非法值！请检查后重新设定。");
						return;
					}

				}//for l
			}//for k
			
		}// for j
	}// for i

	unsigned char *vv=m_processimage->GetData();
	m_processimage->ReleaseBuffer();

	if (vv!=NULL&&true==isfirstFlag)
	{
		delete []vv; 
	}
	isfirstFlag=true;
	Error error=m_processimage->SetData(imagedata,nWidthBytes*nHeightBytes);
	m_imagedata=m_processimage->GetData();
}



#define ABS(a) (a >= 0 ? a : -a)
#define SWAP(a, b) {int t = a; a = b; b = t;}
void CImageDaoImpl::BresenhamDrawline(unsigned char *lpdata,int x1,int y1,int x2,int y2,int bitcount,int color)
{

	int dx=x2-x1;
	int dy=y2-y1;
	int x,y,e;
	unsigned char b,g,r;
	if (0==dx)								//斜率不存在的情况
	{
		int min=y1>y2?y2:y1;				// 从y递增画线
		for(int i=(int)min;i<y1+y2-min;i++)
		{
			b=color>>16;					
			g=color>>8;
			r=color>>0;
			lpdata[i*nWidthBytes*bitcount+x1*bitcount]=b;//pDC->SetPixel(x1,i,color);
			lpdata[i*nWidthBytes*bitcount+x1*bitcount+1]=g;
			lpdata[i*nWidthBytes*bitcount+x1*bitcount+2]=r;
		}
		return ;
	}
	if (ABS(dx) >= ABS(dy))
	{
		if (x1 > x2)
		{
			SWAP(x1, x2);
			SWAP(y1, y2);
			dx = -dx;
			dy = -dy;
		}
		if (dy >= 0)
		{
			// 直线的倾斜角位于 [0, pi / 4]
			for (int x = x1, y = y1, e = -dx; x <= x2; x ++)
			{
				b=color>>16;					
				g=color>>8;
				r=color>>0;
				lpdata[y*nWidthBytes*bitcount+x*bitcount]=b;//pDC->SetPixel(x,y,color);
				lpdata[y*nWidthBytes*bitcount+x*bitcount+1]=g;
				lpdata[y*nWidthBytes*bitcount+x*bitcount+2]=r;
				e =e+ dy +dy;
				if (e >= 0)
				{
					y ++;
					e =e-dx-dx;
				}
			}
		}
		else  
		{
			// 直线的倾斜角位于 [-pi / 4, 0), 
			for (int x = x1, y = y1, e = dx; x <= x2; x ++)
			{				
				b=color>>16;					
				g=color>>8;
				r=color>>0;
				lpdata[y*nWidthBytes*bitcount+x*bitcount]=b;//pDC->SetPixel(x,y,color);
				lpdata[y*nWidthBytes*bitcount+x*bitcount+1]=g;
				lpdata[y*nWidthBytes*bitcount+x*bitcount+2]=r;
				e =e+ dy +dy;
				if (e <= 0)
				{
					y --;
					e =e+ dx +dx;
				}
			}
		}
	}
	else   
	{
		if (y1 > y2)
		{
			SWAP(x1, x2);
			SWAP(y1, y2);
			dx = -dx;
			dy = -dy;
		}
		if (dx >= 0)
		{
			// 直线的倾斜角位于 (pi / 4, pi / 2]	与(0,pi/4)情况 x,y换位思考
			for (int x = x1, y = y1, e = -dy; y <= y2; y ++)
			{
				b=color>>16;					
				g=color>>8;
				r=color>>0;
				lpdata[y*nWidthBytes*bitcount+x*bitcount]=b;//pDC->SetPixel(x,y,color);
				lpdata[y*nWidthBytes*bitcount+x*bitcount+1]=g;
				lpdata[y*nWidthBytes*bitcount+x*bitcount+2]=r;	
				e =e+ dx +dx;
				if (e >= 0)
				{
					x ++;
					e =e- dy-dy;
				}
			}
		}
		else  
		{
			// 直线的倾斜角位于 [-pi / 2, -pi / 4)
			for (int x = x1, y = y1, e = dy; y <= y2; y ++)
			{
				b=color>>16;					
				g=color>>8;
				r=color>>0;
				lpdata[y*nWidthBytes*bitcount+x*bitcount]=b;//pDC->SetPixel(x,y,color);
				lpdata[y*nWidthBytes*bitcount+x*bitcount+1]=g;
				lpdata[y*nWidthBytes*bitcount+x*bitcount+2]=r;	
				e =e+ dx +dx;
				if (e <= 0)
				{
					x --;
					e =e+ dy +dy;
				}
			}
		}
	}
}

void CImageDaoImpl::Open( int se[3][3])//开运算
{

	Erode(se);

	Dilate(se);
}
void CImageDaoImpl::Close(int se[3][3])//闭运算
{

	Dilate( se);


	Erode(se);
}


void CImageDaoImpl::SkinDetect()
{
	unsigned char y,cb,cr;

	unsigned char r,g,b;
	double max,min;
	double R,G,B,h,s,v;			//RGB FOR DIVIDE/255.0
	ofstream of;

	//unsigned char *imagedata=new unsigned char[nHeightBytes*nWidthBytes*3];
	//memset(imagedata,255,nHeightBytes*nWidthBytes); //清空目标输出图像
	//memset(imagedata,backgroundcolor,nHeightBytes*nWidthBytes*3);
	for (int i=0;i<nWidthBytes*nHeightBytes*3;i+=3)
	{
		b=m_imagedata[i];
		g=m_imagedata[i+1];
		r=m_imagedata[i+2];
		
		//另一个标准
// 		y=(unsigned char)(r * 0.256789 + g * 0.504129 + b * 0.097906)+ 16;
// 		cb= (unsigned char)(r *(-0.148223) + g *(-0.290992) + b * 0.439215)+ 128;
// 		cr=(unsigned char)(r * 0.439215 + g *(-0.367789) + b *(-0.071426))+ 128;
		

		y=(unsigned char)(0.2990*r+0.5870*g+0.1140*b);
		cb=(unsigned char)(-0.1687*r-0.3313*g+0.5000*b+128);
		cr=(unsigned char)(0.5000*r-0.4187*g-0.0813*b+128);
		/*
		/////////////////////hsv 中 H计算///////////////////////////////////////
		R=r/255.0;
		G=g/255.0;
		B=b/255.0;
		max=(R>G?R:G);
		max=max>B?max:B;
		min=(R>G?G:R);
		min=min>B?B:min;
		if (R==max)
		{
			h=(G-B)/(max-min);
		}
		else if(G==max)
		{
			h=2+(B-R)/(max-min);
		}
		else
		{
			h=4+(R-G)/(max-min);
		}
		h*=60;
		if (h<0)
		{
			h+=360;
		}
		*/
		//////////////////////////////////////////////////////////////////////////

		//of<<y<<cb<<cr;
		
		//cb=(unsigned char)((-0.1687)*r+(-0.3313)*g+0.5*b);
		//cr=(unsigned char)(0.5*r+(-0.4187)*g+(-0.0813)*b);
// 		if (cb>=106&&cb<=131&&cr>=120&&cr<=159)
// 		{
// 			imagedata[i]=b;
// 			imagedata[i+1]=g;
// 			imagedata[i+2]=r;
// 		}
		if (cb<77||cb>127||cr<133||cr>173)//||h>45||h<0)			//一种基于H-CRCB颜色空间的肤色检测算法研究  133<=cr<=173  77<=cb<=127    0<=h<=45
		{
			m_imagedata[i]=backgroundcolor;
			m_imagedata[i+1]=backgroundcolor;
			m_imagedata[i+2]=backgroundcolor;
 		}
	}

	
//	int num[3][3]={{-1,1,-1},{1,1,1,},{-1,1,-1}};
// 	unsigned char *vv=m_processimage->GetData();
// 	m_processimage->ReleaseBuffer();
// 
// 	if (vv!=NULL&&true==isfirstFlag)
// 	{
// 		delete []vv; 
// 	}
// 	isfirstFlag=true;
// 	Error error=m_processimage->SetData(imagedata,nWidthBytes*nHeightBytes*3);
// 	m_imagedata=m_processimage->GetData();
}

bool CImageDaoImpl::BackgroundUpdateOperate(unsigned char *curImageData,unsigned char *backgroundImageData)
{
	if (m_frontColorCount==0)
	{
		m_backgroundUpdateDao->ImageCountAddOne();
	}
	else
	{
		m_backgroundUpdateDao->ImageCountClearZero();
	}
	//m_backgroundUpdateDao->ImageCountAddOne();
	m_backgroundUpdateDao->UpdateBackgroundPixels(curImageData,backgroundImageData);
	return true;
}
// bool CImageDaoImpl::ResetCountImageUpdatePixels()
// {
// 	m_backgroundUpdateDao->ResetImageCountUpdatePixels();
// 	return true;
// }

bool CImageDaoImpl::GetGrayAndBackgroundPTR(unsigned char *curImage,unsigned char *backgroundImage)
{
	m_backgroundUpdateDao->GetGrayAndBackgroundPTR(curImage,backgroundImage);
	return true;
}

CString CImageDaoImpl::Getfront_background_Pixelstatics(void)
{

	return m_front_background_Pixelstatics;
}
