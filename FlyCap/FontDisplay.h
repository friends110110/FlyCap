#pragma once
//#include "GLee.h"
#include "GL/gl.h"
#include <windows.h>
#define MAX_CHAR       128
class CFontDisplay		// to the 0 , 0 , 1 ,1  glviewport(0,1,0,1)
{
public:
	//��
	void drawCNString(const char* str);
	//�����С���ַ���(ANSI_CHARSET��GB2312_CHARSET��DEFAULT_CHARSET)����������
	void selectFont(int size, int charset, const char* face);
	void display(int fontsize,char *str,GLfloat *fontlocation=NULL,GLfloat *fontcolor=NULL);
	CFontDisplay();
	CFontDisplay(GLfloat *fcolor,GLfloat *m_fontlocation);
	virtual ~CFontDisplay();

	void DrawString(const char* str);
private:
	GLfloat *m_fontcolor;
	GLfloat *m_fontlocation;
public:
	bool isFirstCall;
};
