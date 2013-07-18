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
bool CImageDaoImpl::CaptureBackgroundImage()			//��׽����ͼƬ
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
	//���ڱ�������ȷ����ͼ�񳤿�
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
//����������ͼ���Լ���������λ�24�������ػҶ�����ָ��
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
	//����Ҷ�����
	for(i=0;i<nHeightBytes;i++)
	{
		n=0;
		for(j=0;j<nWidthBytes*3;j++,n++)
		{
			b=*(m_imagedata+nWidthBytes*(nHeightBytes-1-i)+j);  //bmp��˳����BGR
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

���ܣ�
	���õ������Զ�ȷ����ֵ

������
	int nMaxIter������������
	int &nDiffRet��	ʹ�ø�����ֵȷ���ĵ������밵��ƽ���ҶȵĲ���ֵ
����ֵ��
	int���ͣ��㷨��ȷ������ֵ
***************************************************/

int CImageDaoImpl::DetectThreshold(int nMaxIter, int &nDiffRet,int &numMax,int &numMin)
{
	int nThreshold;

	nDiffRet = 0;

	// ֱ��ͼ����
	int nHistogram[256] = { 0 };
	int i, j;

	BYTE bt;
	
	int nMin = 255;
	int nMax = 0;
	
	m_imagedata=m_processimage->GetData();
	// ɨ��ͼ��,����������С�ҶȺ�ֱ��ͼ
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
	numMin=nMin;												//�õ���Сֵ
	numMax=nMax;												//�õ����ֵ
	int nTotalGray = 0; //�Ҷ�ֵ�ĺ�
	int nTotalPixel = 0; //�������ĺ�
	int nNewThreshold = (nMax + nMin)/2; //��ʼ��ֵ

	nDiffRet = nMax - nMin;

	if (nMax == nMin)
		nThreshold = nNewThreshold;

	else
	{
		nThreshold = 0;

		// ������ʼ,ֱ�����������ﵽ100������ֵ����һ�ֵõ�����ֵ��ȣ���������
		for(int nIterationTimes = 0; nThreshold != nNewThreshold && nIterationTimes < nMaxIter; nIterationTimes ++)
		{
			nThreshold = nNewThreshold;
			nTotalGray = 0;
			nTotalPixel = 0;

			//����ͼ����С�ڵ�ǰ��ֵ���ֵ�ƽ���Ҷ�
			for(i=nMin; i<nThreshold; i++)
			{
				nTotalGray += nHistogram[i]*i;
				nTotalPixel += nHistogram[i];
			}
			int nMean1GrayValue = nTotalGray/nTotalPixel;


			nTotalGray = 0;
			nTotalPixel = 0;

			//����ͼ���д��ڵ�ǰ��ֵ���ֵ�ƽ���Ҷ�
			for(i=nThreshold + 1; i<=nMax; i++)
			{
				nTotalGray += nHistogram[i]*i;
				nTotalPixel += nHistogram[i];
			}
			int nMean2GrayValue = nTotalGray/nTotalPixel;

			nNewThreshold = (nMean1GrayValue + nMean2GrayValue)/2; //������µ���ֵ
			nDiffRet = abs(nMean1GrayValue - nMean2GrayValue);
		}
	}

	return nThreshold;
}
/***************************************************************************** 
* 
* \�������ƣ� 
*   OneDimentionOtsu() 
* 
* \�������: 
*   pGrayMat:      ��ֵͼ������ 
*   width:         ͼ�γߴ��� 
*   height:        ͼ�γߴ�߶� 
*   nTlreshold:    �����㷨����õ��Ķ�ֵ���ָ���ֵ 
* \����ֵ: 
*   �� 
* \����˵��:ʵ�ֻҶ�ͼ�Ķ�ֵ���ָ�������䷽���Otsu�㷨���׳ƴ���㷨�� 
* 
****************************************************************************/  
int CImageDaoImpl::DetectOTSUMethod_OneDimension()
{
	
// 	double nHistogram[256];         //�Ҷ�ֱ��ͼ  
// 	double dVariance[256];          //��䷽��  
// 	int N = nHeightBytes*nWidthBytes;           //��������  
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
// 			nHistogram[nData]++;     //����ֱ��ͼ  
// 		}  
// 	}  
// 	double Pa=0.0;      //�������ָ���  
// 	double Pb=0.0;      //Ŀ����ָ���  
// 	double Wa=0.0;      //����ƽ���Ҷ�ֵ  
// 	double Wb=0.0;      //Ŀ��ƽ���Ҷ�ֵ  
// 	double W0=0.0;      //ȫ��ƽ���Ҷ�ֵ  
// 	double dData1=0.0,  dData2=0.0;  
// 	for(i=0; i<256; i++)     //����ȫ��ƽ���Ҷ�  
// 	{  
// 		nHistogram[i] /= N;  
// 		W0 += i*nHistogram[i];  
// 	}  
// 	for(i=0; i<256; i++)     //��ÿ���Ҷ�ֵ������䷽��  
// 	{  
// 		Pa += nHistogram[i];  
// 		Pb = 1-Pa;  
// 		dData1 += i*nHistogram[i];  
// 		dData2 = W0-dData1;  
// 		Wa = dData1/Pa;  
// 		Wb = dData2/Pb;  
// 		dVariance[i] = (Pa*Pb* pow((Wb-Wa), 2));  
// 	}  
// 	//����ÿ�������ȡ�����󷽲�����Ӧ�ĻҶ�ֵ  
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
		//ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���  
		for(i = 0; i < height; i++)  
		{  
			for(j = 0;j < width;j++)  
			{  
				pixelCount[(int)m_imagedata[i * width + j]]++;  
			}  
		}  

		//����ÿ������������ͼ���еı���  
		for(i = 0; i < 256; i++)  
		{  
			pixelPro[i] = (float)pixelCount[i] / pixelSum;  
		}  
	  
		//�����Ҷȼ�[0,255]  
		float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;  
		for(i = 0; i < 256; i++)  
		{  
			w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;  
			for(j = 0; j < 256; j++)  
				{  
					if(j <= i)   //��������  
					{  
						w0 += pixelPro[j];  
						u0tmp += j * pixelPro[j];  
					}  
					else   //ǰ������  
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
* \�������ƣ� 
*   TwoDimentionOtsu() 
* 
* \�������: 
*   pGrayMat:      ��ֵͼ������ 
*   width:         ͼ�γߴ��� 
*   height:        ͼ�γߴ�߶� 
*   nTlreshold:    �����㷨����õ��Ķ�ֵ���ָ���ֵ 
* \����ֵ: 
*   �� 
* \����˵��:ʵ�ֻҶ�ͼ�Ķ�ֵ���ָ�������䷽�����άOtsu�㷨�� 
* \��ע���ڹ�����άֱ��ͼ��ʱ�򣬲��ûҶȵ��3*3�����ֵ 
******************************************************************************/  
int CImageDaoImpl::DetectThreshOTSUMethod_TwoDimension()
{
	double dHistogram[256][256];        //������ά�Ҷ�ֱ��ͼ  
    double dTrMatrix = 0.0;             //��ɢ����ļ�  
    int N = nHeightBytes*nWidthBytes;               //��������  
	int nThreshold;
	m_imagedata=m_processimage->GetData();
	int i,j,n,m;
    for( i=0; i<256; i++)  
    {  
        for(int j=0; j<256; j++)  
            dHistogram[i][j] = 0.0;      //��ʼ������  
    }  
    for(i=0; i<nHeightBytes; i++)  
    {  
        for( j=0; j<nWidthBytes; j++)  
        {  
            unsigned char nData1 = m_imagedata[i*nWidthBytes+j];//(unsigned char)cvmGet(pGrayMat, i, j);  //��ǰ�ĻҶ�ֵ  
            unsigned char nData2 = 0;  
            int nData3 = 0;         //ע��9��ֵ��ӿ��ܳ���һ���ֽ�  
            for(int m=i-1; m<=i+1; m++)  
            {  
                for( n=j-1; n<=j+1; n++)  
                {  
                    if((m>=0)&&(m<nHeightBytes)&&(n>=0)&&(n<nWidthBytes))  
                        nData3 += m_imagedata[m*nWidthBytes+n];//(unsigned char)cvmGet(pGrayMat, m, n); //��ǰ�ĻҶ�ֵ  
                }  
            }  
            nData2 = (unsigned char)(nData3/9);    //����Խ�������ֵ���в���,�����ֵ  
            dHistogram[nData1][nData2]++;  
        }  
    }  
    for(i=0; i<256; i++)  
        for(int j=0; j<256; j++)  
            dHistogram[i][j] /= N;  //�õ���һ���ĸ��ʷֲ�  
   
    double Pai = 0.0;      //Ŀ������ֵʸ��i����  
    double Paj = 0.0;      //Ŀ������ֵʸ��j����  
    double Pbi = 0.0;      //��������ֵʸ��i����  
    double Pbj = 0.0;      //��������ֵʸ��j����  
    double Pti = 0.0;      //ȫ�־�ֵʸ��i����  
    double Ptj = 0.0;      //ȫ�־�ֵʸ��j����  
    double W0 = 0.0;       //Ŀ���������ϸ����ܶ�  
    double W1 = 0.0;       //�����������ϸ����ܶ�  
    double dData1 = 0.0;  
    double dData2 = 0.0;  
    double dData3 = 0.0;  
    double dData4 = 0.0;   //�м����  
    int nThreshold_s = 0;  
    int nThreshold_t = 0;  
    double temp = 0.0;     //Ѱ�����ֵ  
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
/*          W1=dData3=dData4=0.0;   //����ѭ�������ݽ��г�ʼ�� 
            for(int s=i+1; s<256; s++) 
            { 
                for(int t=j+1; t<256; t++) 
                { 
                    W1 += dHistogram[s][t]; 
                    dData3 += s*dHistogram[s][t];  //����2 
                    dData4 += t*dHistogram[s][t];  //Ҳ��������ѭ�����м��� 
                } 
            }*/  
  
            Pai = dData1/W0;  
            Paj = dData2/W0;  
            Pbi = dData3/W1;  
            Pbj = dData4/W1;   // �õ�������ֵ��������4��������ʾ  
            dTrMatrix = ((W0*Pti-dData1)*(W0*Pti-dData1)+(W0*Ptj-dData1)*(W0*Ptj-dData2))/(W0*W1);  
            if(dTrMatrix > temp)  
            {  
                temp = dTrMatrix;  
                nThreshold_s = i;  
                nThreshold_t = j;  
            }  
        }  
    }  
    nThreshold = nThreshold_t;   //���ؽ���еĻҶ�ֵ  
    //nThreshold = 100;  
	return nThreshold;
}
/**************************************************
int DetectThresholdMethod(int nMaxIter, int &nDiffRet)

���ܣ�
	����������Χ�����ֵȷ����ֵ������ÿ�����ص���ֵ����һ��

������
	unsigned char *srcImage��ԭֵ
	int x,int y         : λ�õ�
	int tempx,int tempy ��	ģ���С
����ֵ��
	int���ͣ��㷨��ȷ������ֵ
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
	//����Ҷ�����, 0��ʾ���ڣ�255��ʾ����
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
				//������ǰ�����ɱ�����
				//������Ҫ�ѵ�ǰ��ֵ��ͼ����֮ǰ�Ķ�ֵ��ͼ��Ķ�Ӧ���ص���Ƚϣ���������sTime�����1�������sTime����Ϊ0 �ۼƵ�һ��ֵ��˵����ظ��¡�
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
				//������ǰ�����ɱ�����
				//������Ҫ�ѵ�ǰ��ֵ��ͼ����֮ǰ�Ķ�ֵ��ͼ��Ķ�Ӧ���ص���Ƚϣ���������sTime�����1�������sTime����Ϊ0 �ۼƵ�һ��ֵ��˵����ظ��¡�
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
// 				*(m_imagedata+i*nWidthBytes+j)=frontcolor;//255;//setPixels(i,j,255);;				//�ѿ�Ū�ɰ�ɫ
// 			}
		
//	}
// 	else
// 	{
// 		for(int i=y1;i<=y2;i++)
// 			for (int j=x1;j<=x2;j++)
// 			{
// 				*(lpdata+i*nWidthBytes*3+j*3)=frontcolor;//setPixels(i,j,255);;				//�ѿ�Ū�ɰ�ɫ
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

	//��һ����׼
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
	//ɨ��ͼ�����ģ�����
	if (keyVector==NULL||keyVector->size()==0)
	{
		//AfxMessageBox("keyVector ��Ӧ��Ϊ0");
		//return false;
	}
	else
	{
		searchStartLocationY=keyVector->at(0).y1+10;
	}
	//�Ӳ��ż��̵�  �ָ��� y���¿�ʼ����  10Ϊ ���·ָ�
	for(i=nTempMY+searchStartLocationY; i<nHeightBytes - (nTempH - nTempMY)+1; i++)		
	{
		for(j=nTempMX; j<nWidthBytes - (nTempW - nTempMX) + 1; j++)											//�Ȱ���ɨ��
		{
			// (j,i)Ϊ���ĵ�
			int fResult = 0;
			for(int l=0; l<nTempW; l++)
			{
				for(int k=0; k<nTempH; k++)
				{
					//�����Ȩ��
					fResult += *(m_imagedata+(i+k-nTempMY)*nWidthBytes+j + l - nTempMX);//getPixels(i+k-nTempMY,j + l - nTempMX);
				}
			}
			//�ҵ���ָָ���
			if(fResult>=thredFingerCount*frontcolor)//&&true==IsFingerSkin(lpdata,i,j))										//�ж�����׵����   ��ܴ��ֻǷ��ɫ����
			{
				showImageDrawRECflag=true;     
				m_imagedata=m_processimage->GetData();
				DrawRectangle(m_imagedata,j,i,Rectanglstep,frontcolor);
				
				//ֱ�ӶԵ�ǰ������ж�

				pointArray->push_back(CPoint(j,i));


				//��������Ϣ
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
							//ȷ�����˰����ĸ����������ӳ�2��					/////////////////////////////////////////////
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
								Beep(1000,500);			//��500����
								//������Զ�ҪͶӰ������ͼ���ļ� 
								//��ͼ����Ϊ��ʼ״̬      Ȼ���Ǹ�����λ�ý��� ȦȦ
								//Ҳ���Լ��� �Ѿ����������� ��ͼƬ��
								//����������Ϊ0
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
				m_IsKeyRepeatFlag=0;  //��λ
			}
			//............................
			return	keyArray->at(i).key;
		}
	}
	//....................................
	m_IsKeyRepeatFlag=0;		//��λ
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
			AfxMessageBox("�����˸���Ϊ��û������");
		}
		else
			keyclickedArray->pop_back();
		//keyclickedtemp=0;
		break;
	case 'C':
		AfxMessageBox("ִ�д�绰����");
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
 
 ���ܣ�����ð�ݷ�������������򣬲���������Ԫ�ص���ֵ��

 ����:
	int * pAryGray��Ҫ������ȡ��ֵ������
    int nFilterLen�����鳤��
 
 ����ֵ:
    int ��ֵ
 
*******************/

int CImageDaoImpl::GetMedianValue(int * pAryGray, int nFilterLen)
{
	int i, j;	
	int nMedianValue;
	int nTemp; //�м����
	
	//����
	for (j=0; j < nFilterLen - 1; j++)
	{
		for (i=0; i < nFilterLen - j - 1; i++)
		{
			if (pAryGray[i] > pAryGray[i + 1])
			{
				// ����λ��
				nTemp = pAryGray[i];
				pAryGray[i] = pAryGray[i + 1];
				pAryGray[i + 1] = nTemp;
			}//if
		}//for i
	}//for j
	
	// ������ֵ
	if ((nFilterLen & 1) > 0)
	{
		// ������������Ԫ�أ������м�һ��Ԫ��
		nMedianValue = pAryGray[(nFilterLen + 1) / 2];
	}
	else
	{
		// ������ż����Ԫ�أ������м�����Ԫ��ƽ��ֵ
		nMedianValue = (pAryGray[nFilterLen / 2] + pAryGray[nFilterLen / 2 + 1]) / 2;
	}
	
	// ������ֵ
	return nMedianValue;
}

 bool CImageDaoImpl::AdaptiveMedianFilter(int nFilterH, int nFilterW, int nFilterMY, int nFilterMX)
 {
	 
	 int i, j, k, l;
	 unsigned char *data=new unsigned char[nWidthBytes*nHeightBytes]; 
	 memset(data,0,nWidthBytes*nHeightBytes);
	 byte nGray;

	 int* pAryGray; //������������
	 pAryGray = new int[nFilterH * nFilterW];

	 // ����ɨ��ͼ�񣬽�����ֵ�˲�	
	 for(i = nFilterMY; i < nHeightBytes - nFilterH + nFilterMY + 1; i++)// ��(��ȥ��Ե����)
	 {
		 for(j = nFilterMX; j < nWidthBytes - nFilterW + nFilterMX + 1; j++)// ��(��ȥ��Ե����)
		 {
			 // ��ȡ�˲�������
			 for (k = 0; k < nFilterH; k++)
			 {
				 for (l = 0; l < nFilterW; l++)
				 {
					 //ԭͼ���i + k - nFilterMY�У���j + l - nFilterMX�е�����ֵ
					 nGray = m_imagedata[ (i + k -nFilterMY)*nWidthBytes+j + l - nFilterMX];					

					 // ��������ֵ
					 pAryGray[k * nFilterW + l] = nGray;
				 }//l
			 }//k

			 nGray = GetMedianValue(pAryGray, nFilterH * nFilterW); //ͨ�������ȡ��ֵ

			 //�жϵ�ǰ�����Ƿ�������ļ����Сֵ
			 if( (m_imagedata[i*nWidthBytes+j] == pAryGray[0]) || (m_imagedata[i*nWidthBytes+j] == pAryGray[nFilterH * nFilterW - 1]) )
				 data[ i*nWidthBytes+j]=nGray; //����ֵ��Ϊ��Ӧ
			 else
				 data[ i*nWidthBytes+j]=m_imagedata[i*nWidthBytes+j]; //���Ǽ�ֵ�򲻸ı�ԭͼ���ֵ
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
 
 ���ܣ�
	3*3�ṹԪ�صĶ�ֵͼ��ʴ����
 ע��
	ֻ�ܴ���2ֵͼ��

 ������
	ImgProcess* pTo: Ŀ�����ͼ��� CImgProcess ָ��
	se[3][3]: 3*3�ĽṹԪ�أ�������Ԫ�صĺϷ�ȡֵΪ: 
		1 --- ǰ��
		0 --- ����
	   -1 --- ������	 
 ����ֵ��
	��	
*******************/
void CImageDaoImpl::Erode(int se[3][3])
{


	int i, j; //ͼ��ѭ������
	int k, l; //�ṹԪ��ѭ������

	BOOL bMatch; //�ṹԪ���Ƿ���ֲ�ͼ��ƥ��

	unsigned char *imagedata=new unsigned char[nHeightBytes*nWidthBytes];
	//memset(imagedata,255,nHeightBytes*nWidthBytes); //���Ŀ�����ͼ��
	memset(imagedata,backgroundcolor,nHeightBytes*nWidthBytes);
	//����ɨ��ͼ��Ϊ��ֹ����Խ�磬��������һ�����ؿ�Ŀձ�
	for(i=1; i<nHeightBytes-1; i++)
	{
		for(j=1; j<nWidthBytes-1; j++)
		{
			//����ʹ�õ���3*3�ĽṹԪ�أ�Ϊ��ֹԽ�磬���������Ϻ����µ����������Լ���������ҵ���������
			bMatch = true;
			for(k=0; k<3; k++)
			{
				for(l=0; l<3; l++)
				{
					if( se[k][l] == -1 ) //������
						continue;

					if( se[k][l] == 1 ) //ǰ��
					{
						//if( m_imagedata[(i-1+k)*nWidthBytes+j-1+l] != backgroundcolor )
						if( m_imagedata[(i-1+k)*nWidthBytes+j-1+l] != frontcolor )
						{
							bMatch = false;
							break;
						}
					}
					else if( se[k][l] == 0 ) //����
					{
						if(  m_imagedata[(i-1+k)*nWidthBytes+j-1+l] != backgroundcolor )
						{
							bMatch = false;
							break;
						}
					}
					else
					{
						AfxMessageBox("�ṹԪ�غ��зǷ�ֵ������������趨��");
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
 
 ���ܣ�
	3*3�ṹԪ�صĶ�ֵͼ����������
 ע��
	ֻ�ܴ���2ֵͼ��

 ������
	Image* pTo: Ŀ�����ͼ��� CImgProcess ָ��
	se[3][3]: 3*3�ĽṹԪ�أ�������Ԫ�صĺϷ�ȡֵΪ: 
		1 --- ǰ��
	   -1 --- ������
	 
 ����ֵ��
	��
	
*******************/
void CImageDaoImpl::Dilate( int se[3][3])
{


	int i, j; //ͼ��ѭ������
	int k, l; //�ṹԪ��ѭ������


	//����se�������ĵĶԳƼ�
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

	//����ɨ��ͼ��Ϊ��ֹ����Խ�磬��������һ�����ؿ�Ŀձ�
	for(i=1; i<nHeightBytes-1; i++)
	{
		for(j=1; j<nWidthBytes-1; j++)
		{
			//����ʹ�õ���3*3�ĽṹԪ�أ�Ϊ��ֹԽ�磬���������Ϻ����µ����������Լ���������ҵ���������

			for(k=0; k<3; k++)
			{
				for(l=0; l<3; l++)
				{
					if( se[k][l] == -1 ) // ������
						continue;

					if( se[k][l] == 1 )
					{
						//if( m_imagedata[(i-1+k)*nWidthBytes+j-1+l] ==  0)
						if( m_imagedata[(i-1+k)*nWidthBytes+j-1+l] ==  frontcolor)
						{
							//ԭͼ�ж�Ӧ�ṹԪ�صľֲ�������һ��Ϊ1���ͽ�Ŀ��ͼ���Ӧ�ڽṹԪ�����ĵ�������0
							//imagedata[i*nWidthBytes+j]=0;
							imagedata[i*nWidthBytes+j]=frontcolor;
							break;
						}
					}
					else
					{
						AfxMessageBox("�ṹԪ�غ��зǷ�ֵ������������趨��");
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
	if (0==dx)								//б�ʲ����ڵ����
	{
		int min=y1>y2?y2:y1;				// ��y��������
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
			// ֱ�ߵ���б��λ�� [0, pi / 4]
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
			// ֱ�ߵ���б��λ�� [-pi / 4, 0), 
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
			// ֱ�ߵ���б��λ�� (pi / 4, pi / 2]	��(0,pi/4)��� x,y��λ˼��
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
			// ֱ�ߵ���б��λ�� [-pi / 2, -pi / 4)
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

void CImageDaoImpl::Open( int se[3][3])//������
{

	Erode(se);

	Dilate(se);
}
void CImageDaoImpl::Close(int se[3][3])//������
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
	//memset(imagedata,255,nHeightBytes*nWidthBytes); //���Ŀ�����ͼ��
	//memset(imagedata,backgroundcolor,nHeightBytes*nWidthBytes*3);
	for (int i=0;i<nWidthBytes*nHeightBytes*3;i+=3)
	{
		b=m_imagedata[i];
		g=m_imagedata[i+1];
		r=m_imagedata[i+2];
		
		//��һ����׼
// 		y=(unsigned char)(r * 0.256789 + g * 0.504129 + b * 0.097906)+ 16;
// 		cb= (unsigned char)(r *(-0.148223) + g *(-0.290992) + b * 0.439215)+ 128;
// 		cr=(unsigned char)(r * 0.439215 + g *(-0.367789) + b *(-0.071426))+ 128;
		

		y=(unsigned char)(0.2990*r+0.5870*g+0.1140*b);
		cb=(unsigned char)(-0.1687*r-0.3313*g+0.5000*b+128);
		cr=(unsigned char)(0.5000*r-0.4187*g-0.0813*b+128);
		/*
		/////////////////////hsv �� H����///////////////////////////////////////
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
		if (cb<77||cb>127||cr<133||cr>173)//||h>45||h<0)			//һ�ֻ���H-CRCB��ɫ�ռ�ķ�ɫ����㷨�о�  133<=cr<=173  77<=cb<=127    0<=h<=45
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
