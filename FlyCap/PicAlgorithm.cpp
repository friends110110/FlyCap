#include "StdAfx.h"
#include "PicAlgorithm.h"
#include "FlyCapture2.h"
#include "FlyCapture2GUI.h" 
/*#include <ctype.h>*/
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
using namespace FlyCapture2;
CPicAlgorithm::CPicAlgorithm(void)
{
}


CPicAlgorithm::~CPicAlgorithm(void)
{
}


//����������ͼ���Լ���������λ�24�������ػҶ�����ָ��
unsigned char* CPicAlgorithm::RGB2Gray(unsigned char* imgData, int width, int height, int bitCount)
{
	if (imgData==NULL)
	{
		return NULL;
	}
	int n=0;
	int i,j;
	unsigned char r,g,b,gray;
	int widthStep=(int)((width*bitCount+31)/32)*4;
	int newwidthStep=(int)((width*8+31)/32)*4;
	unsigned char * dst =new unsigned char [newwidthStep*height];//(unsigned char*)malloc(newwidthStep*height*sizeof(unsigned char));
	memset(dst,0,newwidthStep*height);
	//����Ҷ�����
	for(i=0;i<height;i++)
	{
		n=0;
		for(j=0;j<width*3;j++,n++)
		{
			b=*(imgData+widthStep*(height-1-i)+j);  //bmp��˳����BGR
			j++;
			g=*(imgData+widthStep*(height-1-i)+j);
			j++;
			r=*(imgData+widthStep*(height-1-i)+j);
			gray=(r*19595 + g*38469 + b*7472) >> 16;
			*(dst+newwidthStep*(height-1-i)+n)=gray;
		}
	}
	return dst;
}


/**//*
	* ͼ���ֵ��
	*/
unsigned char* CPicAlgorithm::Gray2BW(unsigned char *imgData, int width, int height)
{

	if (imgData==NULL)
	{
		return NULL;
	}
	int rows=height;
	int cols=width;
	int x0=0,y0=0;
	int dx=width,dy=height;
	unsigned char *np;     // ͼ��ָ��
	int threshold=1; // ��ֵ
	int ihist[256];         // ͼ��ֱ��ͼ��256����

	int i, j, k;       // various counters
	int  n1, n2, gmin, gmax;
	double m1, m2, sum, csum, fmax, sb;
	int n;
	// ��ֱ��ͼ����...
	memset(ihist, 0, sizeof(ihist));

	gmin=255; gmax=0;
	// ����ֱ��ͼ
	for (i = y0 + 1; i < y0 + dy - 1; i++) {
		np = &imgData[i*cols+x0+1];
		for (j = x0 + 1; j < x0 + dx - 1; j++) {
			ihist[*np]++;			//�����ͳ������1
			if(*np > gmax) gmax=*np;	//gmax��gminͳ��������С������ֵ
			if(*np < gmin) gmin=*np;
			np++; /* next pixel */
		}
	}

	// set up everything
	sum = csum = 0.0;
	n = 0;

	for (k = 0; k <= 255; k++) {
		sum += (double) k * (double) ihist[k];   /* x*f(x) ������*/
		n   += ihist[k];                           /* f(x)   ����   */
	}

	if (!n) {
		// if n has no value, there is problems...
		fprintf (stderr, "NOT NORMAL thresholdValue = 160\n");

	}

	// do the otsu global thresholding method
	fmax = -1.0;	//�������Ϊ���������趨���ĳ�ʼֵ
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


	//Ϊdstͼ��������ݿռ�
	unsigned char* dst =new unsigned char[widthStep*height];
		//(unsigned char*)malloc(widthStep*height*sizeof(unsigned char));
	//memset(dst,NULL,widthStep*height);

	//����Ҷ�����, 0��ʾ���ڣ�255��ʾ����
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++,n++)
		{
			r=*(imgData+widthStep*(height-1-i)+j);
			if(r>=threshold)
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


//��ͼ���ϻ���һ�����ĵĺ�ɫ������,���þ�������ϵ�����½�Ϊԭ�㡣
//x,yΪ���������½ǵ����꣬dx��dyΪ�����εĳ���tΪ�߿�
unsigned char* CPicAlgorithm::drawEmptyRect(unsigned char *imgData, int width, int height, int x, int y, int dx, int dy,int t)
{
	if (imgData==NULL)
	{
		return NULL;
	}
	if(x<0||y<0||x>width||y>height||(x+dx)>width||(y+dy)>height)
	{
		printf("wrong parameter!!!");
		return 0;
	}
	int i,j;
	int widthStep=((width*8+31)/32)*4;  //��ռ���ֽ���
	unsigned char *dst=new unsigned char[widthStep*height];	//��������һ���ڴ棬����ԭ��������������
	memcpy(dst, imgData, widthStep*height);	

	for(i=y;i<y+t;i++)
	{
		for(j=x;j<x+dx;j++)
		{	
			*(dst+widthStep*(y+i-1)+x+j)=0;	//��ӡ��ɫ
		}
	} 
	for(i=y+t;i<y+dy;i++)
	{
		for(j=x;j<x+t;j++)
		{	
			*(dst+widthStep*(y+i-1)+x+j)=0;	//��ӡ��ɫ
		}
	} 
	for(i=y+t;i<y+dy;i++)
	{
		for(j=x+dx-t;j<x+dx;j++)
		{	
			*(dst+widthStep*(y+i-1)+x+j)=0;	//��ӡ��ɫ
		}
	} 
	for(i=y+dy-t;i<y+dy;i++)
	{
		for(j=x+t;j<x+dx-t;j++)
		{	
			*(dst+widthStep*(y+i-1)+x+j)=0;	//��ӡ��ɫ
		}
	} 

	return dst;

}



//��ͼ���ϼ���ĳһС�����εĻҶ�ֵ�ĺ�,���������ڻҶ�ͼ��.
//����Ҷ�ֵ70%Ϊ0����ɫ�����򷵻�1�����򷵻�0.
int CPicAlgorithm::getRect(unsigned char *imgData, int width, int height, int x, int y, int dx, int dy)
{
	if (imgData==NULL)
	{
		return NULL;
	}
	int i,j;
	int widthStep=((width*8+31)/32)*4;  //��ռ���ֽ���
	double sum=0;						//sumΪ���������ֵ��
	double num=0;						//numΪ�������ص�Ϊ��ɫ��ֵ��
	const int percent=0.7;
	for(i=y;i<y+dy;i++)
	{
		for(j=x;j<x+dx;j++)
		{
			sum+=*(imgData+widthStep*(y+i-1)+x+j);
			num+=255;
		}
	} 
	sum/=255;	//��һ������ɫ��Ϊ1����ɫ��Ϊ0
	num/=255;	//��һ������ɫ��Ϊ1����ɫ��Ϊ0

	if(sum<num*(1-percent))
		return 1;
	return 0;

}
