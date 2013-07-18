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
	// 找种子
	while(RecFindSeed4RegionGrow(src2BImgPto ,pto,nwidth,nheight, Seed_x_y , frontcolor ))
	{
		// 找出第一个连通区域的方框坐标
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
	// 临时图像数据指针
	unsigned char *pImgdatatmp,*pImgdatatmppto;

// 	// 每行图像数据的字节数(8位灰度图)
// 	int nLineBytes = (nwidth*8+31)/32*4;
	int i=0,j=0;
	int n,m;
	//the location for the start location

	//第一个是分割条，所以 根据顺序的应该是下面的 B 键开始的

	if (keystrCount!=0)				//找到的为第1调分隔开线
	{
		
		
		//根据 real_NumkeyCount 来确定 搜索点 开始位置
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
	// 每行Y
	for( i = keyfindlocationy; i < nheight; i++)
	{
		// 每列X
		for(j=keyfindlocationx ; j < nwidth; j++)
		{
			pImgdatatmp = srcImage + nwidth *  i + j;	
			pImgdatatmppto=pto+nwidth *  i + j;
			if(*pImgdatatmp == nType&&*pImgdatatmppto==backgroundcolor)
			{

				// 						pxy[0]=j; // X
				// 						pxy[1]=i; // Y
				// 						return TRUE;
				//当找到的此点可能为  y比较高的的那一块所以 找另外的种子点
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

	// 生长起始点灰度
	//BYTE bSeed = GetGray(nSeedX, nSeedY);
	// 生长区域灰度值之和
	//long int lSum = bSeed;
	// 生长区域的点总数和每次八邻域中符合条件点个数
	/*int  nCount = 1;
	// 开始区域生长循环操作
	*(pto+pxy[1]*nwidth+pxy[0])=frontcolor;
	while (nCount > 0)
	{
		nCount = 0;

		for (int i=pxy[1]; i<nheight-1; i++)
		{	// 纵向
			for (int j=pxy[0]; j<nwidth-1; j++)
			{	// 横向
				if (*(pto+i*nwidth+j)==frontcolor)
				{	// 是种子点
					// 开始8邻域扫描
					for (int m=i-1; m<=i+1; m++)
					{
						for (int n=j-1; n<=j+1; n++)
						{
							// 判断是否符合阈值条件且未标记
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

		// 计算新种子值（这里使用改进的种子值算法为已标记区域的平均灰度）
		//bSeed = lSum / nSuit;
	}
	*/
	int k;
	// 定义堆栈，存储坐标
	int * pnGrowQueX ;
	int * pnGrowQueY ;

	// 分配空间
	pnGrowQueX = new int[nwidth*nheight];
	pnGrowQueY = new int[nwidth*nheight];
	int ncurx,ncury;
	int nstart=0,nend=0;
	int xx,yy;
	static int nDx[]={-1,-1,-1,  0,  0,   1,1,1};
	static int nDy[]={-1, 0, 1,  -1, 1,   1,0,-1};  //8领域

	int minx=nwidth,miny=nheight,maxx=-1,maxy=-1;
	// 把种子点的坐标压入栈
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

				// 像素(xx,yy) 压入栈
				pnGrowQueX[nend] = xx;
				pnGrowQueY[nend] = yy;

				*(pto+nwidth*yy+xx)=frontcolor;

				*(srcImage + nwidth * yy + xx) = backgroundcolor;				//原图变黑
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
	BYTE gray;			// 临时变量,存储当前光标像素的灰度值
	int target;			// 临时变量,存储当前光标像素的目标值

	double pdHist[256];	//临时变量,存储灰度直方图

	double dTemp;		// 临时变量,存储累加的直方图数据



	int nHistogram[256] = { 0 };
	int i, j;

	BYTE bt;
	// 扫描图像,计算出最大、最小灰度和直方图
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

			// 写入目标图像
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


int CFindKeyPositionDaoImpl::O_DetectThreshold(unsigned char *srcImage,int nMaxIter, int &nDiffRet,int &numMax,int &numMin)			//牛顿迭代法 最大迭代次数和 亮暗差值
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
	//统计灰度级中每个像素在整幅图像中的个数  
	for(i = 0; i < nheight; i++)  
	{  
		for(j = 0;j < nwidth;j++)  
		{  
			pixelCount[(int)imagedata[i * nwidth + j]]++;  
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
void CFindKeyPositionDaoImpl::O_Erode(unsigned char *srcImg,int se[3][3],int nwidth,int nheight) //腐蚀算法
{
	int i, j; //图像循环变量
	int k, l; //结构元素循环变量

	BOOL bMatch; //结构元素是否与局部图像匹配

	unsigned char *imagedata=new unsigned char[nheight*nwidth];
	//memset(imagedata,255,nHeightBytes*nWidthBytes); //清空目标输出图像
	memset(imagedata,backgroundcolor,nheight*nwidth);
	//逐行扫描图像，为防止访问越界，四周留出一个像素宽的空边
	for(i=KeyOutsideBorderY; i<nheight-KeyOutsideBorderY; i++)
	{
		for(j=KeyOutsideBorderX; j<nwidth-KeyOutsideBorderX; j++)
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
						if( srcImg[(i-1+k)*nwidth+j-1+l] != frontcolor )
						{
							bMatch = false;
							break;
						}
					}
					else if( se[k][l] == 0 ) //背景
					{
						if(  imagedata[(i-1+k)*nwidth+j-1+l] != backgroundcolor )
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
				imagedata[i*nwidth+j]=frontcolor;


		}// for j
	}// for i
	//delete[] srcImg;
	memcpy(srcImg,imagedata,nwidth*nheight);
	//srcImg=imagedata;
	delete []imagedata;
}
void CFindKeyPositionDaoImpl::O_Dilate(unsigned char *srcImg,int se[3][3],int nwidth,int nheight) //膨胀算法
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
	unsigned char *imagedata=new unsigned char[nwidth*nheight];
	memset(imagedata,backgroundcolor,nwidth*nheight);

	//逐行扫描图像，为防止访问越界，四周留出一个像素宽的空边
	for(i=KeyOutsideBorderY; i<nheight-KeyOutsideBorderY; i++)
	{
		for(j=KeyOutsideBorderX; j<nwidth-KeyOutsideBorderX; j++)
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
						if( srcImg[(i-1+k)*nwidth+j-1+l] ==  frontcolor)
						{
							//原图中对应结构元素的局部区域有一点为1，就将目标图像对应于结构元素中心的像素置0
							//imagedata[i*nWidthBytes+j]=0;
							imagedata[i*nwidth+j]=frontcolor;
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
	//delete[]srcImg;
	memcpy(srcImg,imagedata,nwidth*nheight);
	//srcImg=imagedata;
	delete[]imagedata;
}
void CFindKeyPositionDaoImpl::O_Open( unsigned char *srcImg,int se[3][3],int nwidth,int nheight)//开运算
{
	O_Erode(srcImg,se,nwidth,nheight);
	O_Dilate(srcImg,se,nwidth,nheight);
}
void CFindKeyPositionDaoImpl::O_Close(unsigned char *srcImg,int se[3][3],int nwidth,int nheight)//闭运算
{
	O_Dilate(srcImg,se,nwidth,nheight);
	O_Erode(srcImg,se,nwidth,nheight);
}
