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
	if(true== isFirstCall ) { // 如果是第一次调用，执行初始化
		// 为每一个ASCII字符产生一个显示列表
		isFirstCall = false;

		// 申请MAX_CHAR个连续的显示列表编号

		lists = glGenLists(MAX_CHAR);

		// 把每个字符的绘制命令都装到对应的显示列表中
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	}
	// 调用每个字符对应的显示列表，绘制每个字符
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
	selectFont(fontsize, DEFAULT_CHARSET, "Times New Roman");		//默认设定
	glColor3fv(fontcolor);
	glRasterPos2fv(fontlocation);
	// DrawString("Hello, World!");
	//	glRasterPos2f(0.5,0.8);
	drawCNString(str);
	//	drawChar_To_Texture(("我"));
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

	// 计算字符的个数
	// 如果是双字节字符的（比如中文字符），两个字节才算一个字符
	// 否则一个字节算一个字符
	len = 0;
	for(i=0; str[i]!='\0'; ++i)
	{
		if( IsDBCSLeadByte(str[i]) )
			++i;
		++len;
	}

	// 将混合字符转化为宽字符
	wstring = (wchar_t*)malloc((len+1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, wstring, len);
	wstring[len] = L'\0';

	// 逐个输出字符
	for(i=0; i<len; ++i)
	{
		wglUseFontBitmapsW(hDC, wstring[i], 1, list);
		glCallList(list);
	}

	// 回收所有临时资源
	free(wstring);
	glDeleteLists(list, 1);
}
