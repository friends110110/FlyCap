#include "StdAfx.h"
#include "BackgroundUpdateDaoImpl.h"


CBackgroundUpdateDaoImpl::CBackgroundUpdateDaoImpl(void)
{
	m_nwidth=0;
	m_nheight=0;
	m_PixelsTimes=NULL;
	m_count=0;
	m_curGrayImagePtr=NULL;
	m_backgroundImagePtr=NULL;
	a=0.1;
}
// CBackgroundUpdateDaoImpl::CBackgroundUpdateDaoImpl(int nwidth,int nheight)
// {
// 	m_PixelsTimes=new int *[nheight];
// 	for (int i=0;i<nheight;i++)
// 	{
// 		m_PixelsTimes[i]=new int[nwidth];
// 		memset(m_PixelsTimes[i],0,nwidth);
// 	}
// 	m_nheight=nheight;
// 	m_nwidth=nwidth;
// 
// }

CBackgroundUpdateDaoImpl::~CBackgroundUpdateDaoImpl(void)
{
	for(int i=0;i<m_nheight;i++)
		delete[](m_PixelsTimes[i]);
	delete []m_PixelsTimes;
}
bool CBackgroundUpdateDaoImpl::SetWidth_Height(int width,int height)
{
	if (width==m_nwidth&&height==m_nheight)
	{
		return true;
	}
// 	重新分配
// 	for(int i=0;i<m_nheight;i++)
// 		delete[](m_PixelsTimes[i]);
// 	delete []m_PixelsTimes;

	m_nheight=height;
	m_nwidth=width;
// 	m_PixelsTimes=new int *[m_nheight];
// 	for (int i=0;i<m_nheight;i++)
// 	{
// 		m_PixelsTimes[i]=new int[m_nwidth];
// 		memset(m_PixelsTimes[i],0,m_nwidth*sizeof(int));
// 	}

}

bool CBackgroundUpdateDaoImpl::CountImageUpdatePixels(int i,int j,int x)
{
	if (x==0)
	{
		m_PixelsTimes[i][j]=0;
	}
	else
	{
		unsigned char backgroundpixel,curimagepixel;
		m_PixelsTimes[i][j]++;

		if (m_PixelsTimes[i][j]==m_UpdateImageCount)
		{
			*(m_backgroundImagePtr+i*m_nwidth+j)=*(m_curGrayImagePtr+i*m_nwidth+j);
		}
		else if (m_PixelsTimes[i][j]>m_UpdateImageCount)
		{
			backgroundpixel=*(m_backgroundImagePtr+i*m_nwidth+j);
			curimagepixel=*(m_curGrayImagePtr+i*m_nwidth+j);
			*(m_backgroundImagePtr+i*m_nwidth+j)=a*curimagepixel+(1-a)*backgroundpixel;

			//防止 m_PixelsTimes[i][j]    int类型的数据元素加到最大值以后
			m_PixelsTimes[i][j]=m_UpdateImageCount+1;
		}

		if (m_PixelsTimes[i][j]>m_UpdateImageCount)
		{
			//防止 m_PixelsTimes[i][j]    int类型的数据元素加到最大值以后
			m_PixelsTimes[i][j]=m_UpdateImageCount+1;
		}
	}
	return true;
}
bool CBackgroundUpdateDaoImpl::UpdateBackgroundPixels(unsigned char *curImageData,unsigned char *backgroundImageData)
{
	unsigned char backgroundpixel,curimagepixel;
	if (m_count==m_UpdateImageCount)
	{
		for(int i=0;i<m_nheight;i++)
			for(int j=0;j<m_nwidth;j++)
			{
				//if (m_PixelsTimes[i][j]==m_UpdateImageCount)
				{
					*(backgroundImageData+i*m_nwidth+j)=*(curImageData+i*m_nwidth+j);
				}
// 				else if (m_PixelsTimes[i][j]>m_UpdateImageCount)
// 				{
// 					backgroundpixel=*(backgroundImageData+i*m_nwidth+j);
// 					curimagepixel=*(curImageData+i*m_nwidth+j);
// 					*(backgroundImageData+i*m_nwidth+j)=a*curimagepixel+(1-a)*backgroundpixel;
// 				}
// 				else
// 				{
//					*(backgroundImageData+i*m_nwidth+j)=*(backgroundImageData+i*m_nwidth+j);
// 				}
			}
			this->ImageCountClearZero();
	}
	return true;
}

bool CBackgroundUpdateDaoImpl::ImageCountAddOne()
{
	m_count++;
	return true;
}
bool CBackgroundUpdateDaoImpl::ImageCountClearZero()
{
	m_count=0;
	return true;
}
// bool CBackgroundUpdateDaoImpl::ResetImageCountUpdatePixels()
// {
// 	if (m_PixelsTimes==NULL)
// 	{
// 		return false;
// 	}
// 	for(int i=0;i<m_nheight;i++)
// 			memset(m_PixelsTimes[i],0,m_nwidth*sizeof(int));
// 	return true;
// }

bool CBackgroundUpdateDaoImpl::GetGrayAndBackgroundPTR(unsigned char *curImage,unsigned char *backgroundImage)
{
	m_curGrayImagePtr=curImage;
	m_backgroundImagePtr=backgroundImage;
	return true;
}