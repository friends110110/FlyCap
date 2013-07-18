#include "StdAfx.h"
#include "FontDisplay.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFontDisplay::CFontDisplay()
{
	isFirstCall=true;
	m_fontcolor=new  GLfloat[3];
	m_fontcolor[0]=1;
	m_fontcolor[1]=1;
	m_fontcolor[2]=1;
	m_fontlocation=new GLfloat[2];
	m_fontlocation[0]=0.1;
	m_fontlocation[1]=0.1;

}
CFontDisplay::CFontDisplay(GLfloat *fontcolor,GLfloat *fontlocation)
	: isFirstCall(false)
{
	m_fontcolor=fontcolor;
	m_fontlocation=fontlocation;
}
CFontDisplay::~CFontDisplay()
{
	if(NULL!=m_fontcolor)
		delete m_fontcolor;
	if(NULL!=m_fontlocation)
		delete m_fontlocation;
}

void CFontDisplay::DrawString(const char* str)
{

	static GLuint lists;
	if(true== isFirstCall ) { // ����ǵ�һ�ε��ã�ִ�г�ʼ��
		// Ϊÿһ��ASCII�ַ�����һ����ʾ�б�
		isFirstCall = false;

		// ����MAX_CHAR����������ʾ�б���

		lists = glGenLists(MAX_CHAR);

		// ��ÿ���ַ��Ļ������װ����Ӧ����ʾ�б���
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	}
	// ����ÿ���ַ���Ӧ����ʾ�б�����ÿ���ַ�
	for(; *str!='\0'; ++str){
		glCallList(lists + *str);

	}
}

void CFontDisplay::display(int fontsize,char *str,GLfloat *fontlocation,GLfloat *fontcolor)
{
	if (fontcolor==NULL)
	{
		fontcolor=m_fontcolor;
	}
	if (fontlocation==NULL)
	{
		fontlocation=m_fontlocation;
	}
	//glClear(GL_COLOR_BUFFER_BIT);
	selectFont(fontsize, DEFAULT_CHARSET, "Times New Roman");		//Ĭ���趨
	glColor3fv(fontcolor);
	glRasterPos2fv(fontlocation);
	// DrawString("Hello, World!");
	//	glRasterPos2f(0.5,0.8);
	drawCNString(str);
	//	drawChar_To_Texture(("��"));
	//	drawChar_To_Texture("ff");
	// glutSwapBuffers();
}

void CFontDisplay::selectFont(int size, int charset, const char* face)
{
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}

void CFontDisplay::drawCNString(const char* str)
{
	int len, i;
	wchar_t* wstring;
	HDC hDC = wglGetCurrentDC();
	GLuint list = glGenLists(1);

	// �����ַ��ĸ���
	// �����˫�ֽ��ַ��ģ����������ַ����������ֽڲ���һ���ַ�
	// ����һ���ֽ���һ���ַ�
	len = 0;
	for(i=0; str[i]!='\0'; ++i)
	{
		if( IsDBCSLeadByte(str[i]) )
			++i;
		++len;
	}

	// ������ַ�ת��Ϊ���ַ�
	wstring = (wchar_t*)malloc((len+1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, wstring, len);
	wstring[len] = L'\0';

	// �������ַ�
	for(i=0; i<len; ++i)
	{
		wglUseFontBitmapsW(hDC, wstring[i], 1, list);
		glCallList(list);
	}

	// ����������ʱ��Դ
	free(wstring);
	glDeleteLists(list, 1);
}
