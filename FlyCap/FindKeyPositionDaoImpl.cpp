#include "StdAfx.h"
#include "FindKeyPositionDaoImpl.h"


char CFindKeyPositionDaoImpl::keystrArray[Blank_keyNum]={'L','B','C','Y','1','2','3','4','5','6', '7', '8' ,'9', '*', '0', '#'};
CFindKeyPositionDaoImpl::CFindKeyPositionDaoImpl(void)
{

	keystrCount=0;
	m_keypositionArray=new vector<CKeyLocation>;
	keyfindlocationx=keyfindlocationy=0;
}


CFindKeyPositionDaoImpl::~CFindKeyPositionDaoImpl(void)
{
	//if(0!=m_keypositionArray->size())
	{
		m_keypositionArray->empty();
		m_keypositionArray->~vector();
	}
}

vector<CKeyLocation> *CFindKeyPositionDaoImpl:: getLocationPosition()
{
	return m_keypositionArray;
}
bool CFindKeyPositionDaoImpl::RegionGrow(unsigned char *srcImg,int nwidth,int nheight)
{	
	nwidth=BytesWide(nwidth);
	Image wfSaveImage(nheight,nwidth,PIXEL_FORMAT_MONO8);


	//save the background;
	wfSaveImage.SetData(srcImg,nwidth*nheight);
	//unsigned char *pp=wfSaveImage.GetData();

	wfSaveImage.Save("beforefindkey2B.bmp");


	//the background imagedata could not be changed
	//save the 2B image 
	unsigned char *src2BImgPto=new unsigned char[nwidth*nheight];
	O_Grayto2B(srcImg,src2BImgPto,nwidth,nheight);
	wfSaveImage.SetData(src2BImgPto,nwidth*nheight);
	wfSaveImage.Save("findkey2B.bmp");
	

	//after close algorithm, save the image
	int se[3][3];		//	template
	for (int l=0;l<3;l++)
	{
		for(int k=0;k<3;k++)
			se[l][k]=1;
	}

	O_Close(src2BImgPto,se,nwidth,nheight);
	wfSaveImage.SetData(src2BImgPto,nwidth*nheight);
	wfSaveImage.Save("findkey2Boclose.bmp");
	unsigned char * pto=new unsigned char[nwidth*nheight];
//	mapArray->clear();
	m_keypositionArray->clear();

	keyfindlocationx=KeyOutsideBorderX;
	keyfindlocationy=KeyOutsideBorderY;

	keystrCount=0;
	memset(pto,backgroundcolor,nwidth*nheight);
	int Seed_x_y[2]={-1,-1};
	// ������
	while(RecFindSeed4RegionGrow(src2BImgPto ,pto,nwidth,nheight, Seed_x_y , frontcolor ))
	{
		// �ҳ���һ����ͨ����ķ�������
		RecFindRegionWithSeed(src2BImgPto ,pto,nwidth,nheight,Seed_x_y,frontcolor ); 

		//save every step which find a seed and handle
		wfSaveImage.SetData(pto,nheight*nwidth);
		wfSaveImage.Save("wfbaocuneverystep.bmp");
	}
	
	//save the final outcome
	wfSaveImage.SetData(pto,nheight*nwidth);
	wfSaveImage.Save("wfptofindkeyoutcome.bmp");
	delete []pto;
	delete[] src2BImgPto;

	if (keystrCount!=Blank_keyNum)
	{
		// means the following is wrong
 		AfxMessageBox("the position number is not fifteen, get the background failed,TRY AGAIN");
		return false;
	}
	return true;
}
bool CFindKeyPositionDaoImpl::RecFindSeed4RegionGrow(unsigned char *srcImage,unsigned char * pto,int nwidth,int nheight,int pxy[2],unsigned char nType)
{
	// ��ʱͼ������ָ��
	unsigned char *pImgdatatmp,*pImgdatatmppto;

// 	// ÿ��ͼ�����ݵ��ֽ���(8λ�Ҷ�ͼ)
// 	int nLineBytes = (nwidth*8+31)/32*4;
	int i=0,j=0;
	int n,m;
	//the location for the start location

	//��һ���Ƿָ��������� ����˳���Ӧ��������� B ����ʼ��

	if (keystrCount!=0)				//�ҵ���Ϊ��1���ָ�����
	{
		
		
		//���� real_NumkeyCount ��ȷ�� ������ ��ʼλ��
		int real_NumkeyCount=keystrCount-1;
		if (keystrCount==1)
		{
			keyfindlocationy=m_keypositionArray->at(0).y1+10;
		}
		if (real_NumkeyCount/3!=0&&real_NumkeyCount%3==0)
		{
			//get the max number
			int temp1=m_keypositionArray->at(keystrCount-1).y1,temp2=m_keypositionArray->at(keystrCount-2).y1,temp3=m_keypositionArray->at(keystrCount-3).y1;
			temp1=temp1>temp2?temp1:temp2;
			temp1=temp1>temp3?temp1:temp3;
			keyfindlocationy=temp1;
		}

		if (real_NumkeyCount%3==0)
		{
			keyfindlocationx=0;
		}
		else
		{
			keyfindlocationx=m_keypositionArray->at(keystrCount-1).x1;
		}
	}
	// ÿ��Y
	for( i = keyfindlocationy; i < nheight; i++)
	{
		// ÿ��X
		for(j=keyfindlocationx ; j < nwidth; j++)
		{
			pImgdatatmp = srcImage + nwidth *  i + j;	
			pImgdatatmppto=pto+nwidth *  i + j;
			if(*pImgdatatmp == nType&&*pImgdatatmppto==backgroundcolor)
			{

				// 						pxy[0]=j; // X
				// 						pxy[1]=i; // Y
				// 						return TRUE;
				//���ҵ��Ĵ˵����Ϊ  y�Ƚϸߵĵ���һ������ ����������ӵ�
				if (keystrCount==0)
				{
					m=i;
				}
				else
					m=i+keyySearchStep;
				for (;m<nheight;m++)
				{
					for(n=keyfindlocationx;n<nwidth;n++)
					{
						pImgdatatmp = srcImage + nwidth *  m + n;	
						pImgdatatmppto=pto+nwidth *  m + n;
						if(*pImgdatatmp == nType&&*pImgdatatmppto==backgroundcolor)
						{
							pxy[0]=n; // X
							pxy[1]=m; // Y
							return TRUE;
						}
					}
				}
				
			}
		}
	}
	pxy[0]=-1; // X
	pxy[1]=-1; // Y
	return FALSE;
}
bool CFindKeyPositionDaoImpl::RecFindRegionWithSeed(unsigned char *srcImage,unsigned char *pto,int nwidth,int nheight,int pxy[2],unsigned char nType)
{

	// ������ʼ��Ҷ�
	//BYTE bSeed = GetGray(nSeedX, nSeedY);
	// ��������Ҷ�ֵ֮��
	//long int lSum = bSeed;
	// ��������ĵ�������ÿ�ΰ������з������������
	/*int  nCount = 1;
	// ��ʼ��������ѭ������
	*(pto+pxy[1]*nwidth+pxy[0])=frontcolor;
	while (nCount > 0)
	{
		nCount = 0;

		for (int i=pxy[1]; i<nheight-1; i++)
		{	// ����
			for (int j=pxy[0]; j<nwidth-1; j++)
			{	// ����
				if (*(pto+i*nwidth+j)==frontcolor)
				{	// �����ӵ�
					// ��ʼ8����ɨ��
					for (int m=i-1; m<=i+1; m++)
					{
						for (int n=j-1; n<=j+1; n++)
						{
							// �ж��Ƿ������ֵ������δ���
							if ((*(pto+m*nwidth+n)==backgroundcolor)&&(*(srcImage+m*nwidth+n)==frontcolor))
							{
								//pTo->SetPixel(n, m, RGB(255, 255, 255));
								*(pto+m*nwidth+n)=frontcolor;
								nCount++;
								//lSum += GetGray(n, m);
								
							}
						}
					}
				}
			}
		}

		//nSuit += nCount;

		// ����������ֵ������ʹ�øĽ�������ֵ�㷨Ϊ�ѱ�������ƽ���Ҷȣ�
		//bSeed = lSum / nSuit;
	}
	*/
	int k;
	// �����ջ���洢����
	int * pnGrowQueX ;
	int * pnGrowQueY ;

	// ����ռ�
	pnGrowQueX = new int[nwidth*nheight];
	pnGrowQueY = new int[nwidth*nheight];
	int ncurx,ncury;
	int nstart=0,nend=0;
	int xx,yy;
	static int nDx[]={-1,-1,-1,  0,  0,   1,1,1};
	static int nDy[]={-1, 0, 1,  -1, 1,   1,0,-1};  //8����

	int minx=nwidth,miny=nheight,maxx=-1,maxy=-1;
	// �����ӵ������ѹ��ջ
	pnGrowQueX[nend] = pxy[0];
	pnGrowQueY[nend] = pxy[1];
	
	*(pto+nwidth*pxy[1]+pxy[0])=frontcolor;

	while(nstart<=nend)
	{
		ncurx=pnGrowQueX[nstart];
		ncury=pnGrowQueY[nstart];
		for (k=0;k<8;k++)
		{
			xx=ncurx+nDx[k];
			yy=ncury+nDy[k];	
			if ((xx<nwidth) && (xx>=0) && (yy>=0) &&(yy<nheight)&&(*(pto+nwidth*yy+xx)==backgroundcolor)&&(*(srcImage + nwidth * yy + xx) == frontcolor))
			{
				nend++;

				// ����(xx,yy) ѹ��ջ
				pnGrowQueX[nend] = xx;
				pnGrowQueY[nend] = yy;

				*(pto+nwidth*yy+xx)=frontcolor;

				*(srcImage + nwidth * yy + xx) = backgroundcolor;				//ԭͼ���
				if (minx>xx)
				{
					minx=xx;
				}
				if (miny>yy)
				{
					miny=yy;
				}
				if (maxx<xx)
				{
					maxx=xx;
				}
				if (maxy<yy)
				{
					maxy=yy;
				}
			}
		}
		nstart++;
	}

// 	mapArray[keystrArray[keystrCount++]]=new CRect(minx,miny,maxx,maxy);
//	*(pto+miny*nwidth+minx)=frontcolor;
//	*(pto+maxy*nwidth+maxx)=frontcolor;
 	//mapArray->insert(pair<char,CRect>(keystrArray[keystrCount++],CRect(minx,miny,maxx,maxy)));
	//Ckey_location *key=new Ckey_location(minx,miny,maxx,maxy,keystrArray[keystrCount++]);
	m_keypositionArray->push_back(CKeyLocation(minx,miny,maxx,maxy,keystrArray[keystrCount++]));

	delete[] pnGrowQueX;
	delete[] pnGrowQueY;
	return true;
}




bool CFindKeyPositionDaoImpl::O_Histeq(unsigned char *srcImage,int nwidth,int nheight)
{
	BYTE gray;			// ��ʱ����,�洢��ǰ������صĻҶ�ֵ
	int target;			// ��ʱ����,�洢��ǰ������ص�Ŀ��ֵ

	double pdHist[256];	//��ʱ����,�洢�Ҷ�ֱ��ͼ

	double dTemp;		// ��ʱ����,�洢�ۼӵ�ֱ��ͼ����



	int nHistogram[256] = { 0 };
	int i, j;

	BYTE bt;
	// ɨ��ͼ��,����������С�ҶȺ�ֱ��ͼ
	for(j = 0; j < nheight; j ++)
	{
		for(i=0; i<nwidth; i++)
		{
			bt =srcImage[j*nwidth+i];
			nHistogram[bt] ++;

		}
	}
	//this->GenHist(pdHist);
	for (int i=0; i<nheight; i++)
	{
		for (int j=0; j<nwidth; j++)
		{
			dTemp = 0;

			gray = srcImage[i*nwidth+j];//GetGray(j, i);

			for (BYTE k=0; k<gray; k++)
			{
				dTemp+=*(nHistogram + k);
			};

			target = 255 * dTemp;

			if (target < 0) target = 0;
			if (target > 255) target = 255;

			// д��Ŀ��ͼ��
			//pTo->SetPixel(j, i, RGB(target, target, target));
			srcImage[i*nwidth+j]=(unsigned char )target;
		}
	};
	return true;
}
int CFindKeyPositionDaoImpl::O_DetectPixelRoundThreshold(unsigned char *srcImage,int i,int j,int tempx,int tempy)
{
	return 0;
}


int CFindKeyPositionDaoImpl::O_DetectThreshold(unsigned char *srcImage,int nMaxIter, int &nDiffRet,int &numMax,int &numMin)			//ţ�ٵ����� ������������ ������ֵ
{
	return 0;
}
int CFindKeyPositionDaoImpl::O_DetectOTSUMethod_OneDimension(unsigned char *srcImage,int nwidth,int nheight)
{

	int pixelCount[256];
	float pixelPro[256];  
	int i,j,pixelSum,threshold;  
	pixelSum= nwidth * nheight;
	threshold=0;
	unsigned char *imagedata=srcImage;//m_processimage->GetData();

	for(i = 0; i < 256; i++)  
	{  
		pixelCount[i] = 0;  
		pixelPro[i] = 0;  
	}  
	//ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���  
	for(i = 0; i < nheight; i++)  
	{  
		for(j = 0;j < nwidth;j++)  
		{  
			pixelCount[(int)imagedata[i * nwidth + j]]++;  
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

bool CFindKeyPositionDaoImpl::O_Grayto2B(unsigned char *srcImage,unsigned char * srcImage2Bpto,int nwidth,int nheight)
{
	int i,j;
	int threshold=O_DetectOTSUMethod_OneDimension(srcImage,nwidth,nheight);
	if (threshold<m_thread)
	{
		threshold=m_thread;
	}
	for(i=0;i<nheight;i++)
		for(j=0;j<nwidth;j++)
			if (*(srcImage+i*nwidth+j)>=threshold)
			{
				*(srcImage2Bpto+i*nwidth+j)=frontcolor;
			}
			else
			{
				*(srcImage2Bpto+i*nwidth+j)=backgroundcolor;
			}
			return true;
}
void CFindKeyPositionDaoImpl::O_Erode(unsigned char *srcImg,int se[3][3],int nwidth,int nheight) //��ʴ�㷨
{
	int i, j; //ͼ��ѭ������
	int k, l; //�ṹԪ��ѭ������

	BOOL bMatch; //�ṹԪ���Ƿ���ֲ�ͼ��ƥ��

	unsigned char *imagedata=new unsigned char[nheight*nwidth];
	//memset(imagedata,255,nHeightBytes*nWidthBytes); //���Ŀ�����ͼ��
	memset(imagedata,backgroundcolor,nheight*nwidth);
	//����ɨ��ͼ��Ϊ��ֹ����Խ�磬��������һ�����ؿ�Ŀձ�
	for(i=KeyOutsideBorderY; i<nheight-KeyOutsideBorderY; i++)
	{
		for(j=KeyOutsideBorderX; j<nwidth-KeyOutsideBorderX; j++)
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
						if( srcImg[(i-1+k)*nwidth+j-1+l] != frontcolor )
						{
							bMatch = false;
							break;
						}
					}
					else if( se[k][l] == 0 ) //����
					{
						if(  imagedata[(i-1+k)*nwidth+j-1+l] != backgroundcolor )
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
				imagedata[i*nwidth+j]=frontcolor;


		}// for j
	}// for i
	//delete[] srcImg;
	memcpy(srcImg,imagedata,nwidth*nheight);
	//srcImg=imagedata;
	delete []imagedata;
}
void CFindKeyPositionDaoImpl::O_Dilate(unsigned char *srcImg,int se[3][3],int nwidth,int nheight) //�����㷨
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
	unsigned char *imagedata=new unsigned char[nwidth*nheight];
	memset(imagedata,backgroundcolor,nwidth*nheight);

	//����ɨ��ͼ��Ϊ��ֹ����Խ�磬��������һ�����ؿ�Ŀձ�
	for(i=KeyOutsideBorderY; i<nheight-KeyOutsideBorderY; i++)
	{
		for(j=KeyOutsideBorderX; j<nwidth-KeyOutsideBorderX; j++)
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
						if( srcImg[(i-1+k)*nwidth+j-1+l] ==  frontcolor)
						{
							//ԭͼ�ж�Ӧ�ṹԪ�صľֲ�������һ��Ϊ1���ͽ�Ŀ��ͼ���Ӧ�ڽṹԪ�����ĵ�������0
							//imagedata[i*nWidthBytes+j]=0;
							imagedata[i*nwidth+j]=frontcolor;
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
	//delete[]srcImg;
	memcpy(srcImg,imagedata,nwidth*nheight);
	//srcImg=imagedata;
	delete[]imagedata;
}
void CFindKeyPositionDaoImpl::O_Open( unsigned char *srcImg,int se[3][3],int nwidth,int nheight)//������
{
	O_Erode(srcImg,se,nwidth,nheight);
	O_Dilate(srcImg,se,nwidth,nheight);
}
void CFindKeyPositionDaoImpl::O_Close(unsigned char *srcImg,int se[3][3],int nwidth,int nheight)//������
{
	O_Dilate(srcImg,se,nwidth,nheight);
	O_Erode(srcImg,se,nwidth,nheight);
}
