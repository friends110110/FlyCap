#include "StdAfx.h"
#include "AeraWordPic.h"
#include "math.h"
#include <windows.h>
CAeraWordPic::CAeraWordPic(void)
	: isFirstCall(false)
{
	g_texID = 0;
	g_items = 0;
	g_count = 0;
	g_total = 0;
	g_fontsize = 0;
	g_texturesize = 0;
	p_textarea = NULL;
	isFirstCall=true;
	m_bkgroundcolor=new GLfloat[3];
	m_bkgroundcolor[0]=0;
	m_bkgroundcolor[1]=0;
	m_bkgroundcolor[2]=1;
	m_fontcolor=new GLfloat[3];
	m_fontcolor[0]=1;
	m_fontcolor[1]=0;
	m_fontcolor[2]=0;

}


CAeraWordPic::~CAeraWordPic(void)
{
	if (g_items!=NULL)
	{
		ctbuf_destroy();
		//delete g_items;
	}
	if (p_textarea!=NULL)
	{
		//delete p_textarea;
		ta_destroy(p_textarea);
	}
	if (m_bkgroundcolor!=NULL)
	{
		delete m_bkgroundcolor;
	}
	if (m_fontcolor!=NULL)
	{
		delete m_fontcolor;
	}

}



void CAeraWordPic::selectFont(int size, int charset, const char* face)
{
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}


void CAeraWordPic::drawChar_To_SubTexture(wchar_t w, int texcoord_x, int texcoord_y)
{
	HDC hDC = wglGetCurrentDC();

	glPushAttrib(GL_COLOR_BUFFER_BIT |
		GL_ENABLE_BIT | GL_VIEWPORT_BIT);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);

	// 把将要绘制字符的地方涂色
	glViewport(0, 0, g_fontsize, g_fontsize);
	//glColor3f(1.0f, 0.0f, 0.0f);
	glColor3fv(m_bkgroundcolor);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRectf(-1.0f, -1.0f, 1.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// 设置颜色为白色
	//glColor3f(0.0f, 1.0f, 0.0f);
	glColor3fv(m_fontcolor);
	// 计算绘制的位置
	{
		int width, x, y;
		GetCharWidth32W(hDC, w, w, &width);    // 取得字符的宽度
		x = (g_fontsize - width) / 2;
		y = g_fontsize / 8;
		glWindowPos2iARB(x, y);
	}

	// 绘制字符
	// 绘制前应该将各种可能影响字符颜色的效果关闭
	// 以保证能够绘制出白色的字符
	{
		GLuint list = glGenLists(1);

		wglUseFontBitmaps(hDC, w, 1, list);
		glCallList(list);
		glDeleteLists(list, 1);
	}

	// 复制字符像素到纹理
	glBindTexture(GL_TEXTURE_2D, g_texID);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
		texcoord_x, texcoord_y,
		0, 0, g_fontsize, g_fontsize);


	glPopAttrib();
}


void CAeraWordPic::ctbuf_destroy(void)
{
	glDeleteTextures(1, &g_texID);
	//free(g_items);
	delete []g_items;
	g_items = NULL;
	g_count = 0;
	g_total = 0;
	g_fontsize = 0;
	g_texturesize = 0;
}


void CAeraWordPic::ctbuf_init(int fontsize, int texturesize, const char* font)
{
	int i;

	ctbuf_destroy();

	g_count = texturesize / fontsize;
	g_total = g_count * g_count;
	g_fontsize = fontsize;
	g_texturesize = texturesize;

	glGenTextures(1, &g_texID);
	glBindTexture(GL_TEXTURE_2D, g_texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,//GL_LUMINANCE4,
		texturesize, texturesize, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	g_items = new ctitem_t[g_total];//(ctitem_t*)malloc(g_total * sizeof(ctitem_t));
	for(i=0; i<g_total; ++i) {
		g_items[i].valid = 0;
		g_items[i].texcoord_x = (i%g_count * fontsize) / (GLfloat)texturesize;
		g_items[i].texcoord_y = (i/g_count * fontsize) / (GLfloat)texturesize;
	}

	selectFont(fontsize, DEFAULT_CHARSET, font);
}


void CAeraWordPic::ctbuf_bind(void)
{
	glBindTexture(GL_TEXTURE_2D, g_texID);
}


ctitem_t *CAeraWordPic::ctbuf_find(wchar_t c)
{
	int i;
	// 寻找是否已经在缓冲中存在
	for(i=0; i<g_total; ++i)
		if( g_items[i].valid && g_items[i].c == c )
			return &g_items[i];

	// 若不存在，寻找空位以便产生新的数据
	for(i=0; i<g_total; ++i)
		if( !g_items[i].valid )
			break;

	// 没有空位，删除一个数据以得到空位
	if( i == g_total ) {
		// 这里随机的删除一个
		i = rand() % g_total;
	}

	// 在空位上产生新的数据
	g_items[i].valid = 1;
	g_items[i].c = c;
	drawChar_To_SubTexture(c,
		(int)(g_items[i].texcoord_x * g_texturesize),
		(int)(g_items[i].texcoord_y * g_texturesize));

	// 返回
	return &g_items[i];
}


void CAeraWordPic::ctbuf_drawString(const char* str, GLfloat cwidth, GLfloat cheight)
{
	int i, len;
	wchar_t* wstring;
	GLfloat texinc;

	len = 0;
	for(i=0; str[i]!='\0'; ++i)
	{
		if( IsDBCSLeadByte(str[i]) )
			++i;
		++len;
	}

	wstring =new wchar_t[len+1];// (wchar_t*)malloc((len+1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, wstring, len);
	wstring[len] = L'\0';

	ctbuf_bind();

	texinc = (GLfloat)g_fontsize / g_texturesize;
	for(i=0; i<len; ++i) {
		const ctitem_t* pItem = ctbuf_find(wstring[i]);
		glBegin(GL_QUADS);
		glTexCoord2f(pItem->texcoord_x, pItem->texcoord_y);
		glVertex2f(cwidth * i, 0.0f);
		glTexCoord2f(pItem->texcoord_x + texinc, pItem->texcoord_y);
		glVertex2f(cwidth * (i+1), 0.0f);
		glTexCoord2f(pItem->texcoord_x + texinc, pItem->texcoord_y + texinc);
		glVertex2f(cwidth * (i+1), cheight);
		glTexCoord2f(pItem->texcoord_x, pItem->texcoord_y + texinc);
		glVertex2f(cwidth * i, cheight);
		glEnd();
	}
	delete [] wstring;
	//free(wstring);
}


GLfloat CAeraWordPic::ctbuf_texinc(void)
{
	return (GLfloat)g_fontsize / g_texturesize;
}


textarea_t * CAeraWordPic::ta_create(GLfloat min_x, GLfloat min_y, GLfloat max_x, GLfloat max_y,
	int char_in_row, int char_in_col, const char* data)
{
	textarea_t* ret = NULL;
	wchar_t* wdata = NULL;
	wchar_t* wdata_start = NULL;

	// 分配内存，填充基本的数据
	ret = new textarea_t;//(textarea_t*)malloc(sizeof(textarea_t));
	ret->min_x = min_x;
	ret->min_y = min_y;
	ret->max_x = max_x;
	ret->max_y = max_y;
	ret->char_in_row = char_in_row;
	ret->char_in_col = char_in_col;
	ret->scroll_pos = 0.0f;

	// 把参数中的字符串转化为宽字符
	{
		int len = 0;
		int i;
		for(i=0; data[i]!='\0'; ++i)
		{
			if( IsDBCSLeadByte(data[i]) )
				++i;
			++len;
		}

		wdata =new wchar_t[len+1];// (wchar_t*)malloc((len+1) * sizeof(wchar_t));
		wdata_start = wdata;
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, data, -1, wdata, len);
		wdata[len] = L'\0';
	}

	// 把参数中的字符串拆分成多行，每行正好可以够显示
	ret->line_count = 0;
	ret->lines = NULL;
	while( *wdata != L'\0' ) {
		wchar_t* tmpLine = new wchar_t[char_in_row + 1];//(wchar_t*)malloc((char_in_row + 1) * sizeof(wchar_t));
		wchar_t* s;
		int count = 0;
		for(s=tmpLine; *wdata != L'\0';) {
			wchar_t c;
			*s++ = (c=*wdata++);
			if( ++count == char_in_row || c == L'\n' )
				break;
		}
		*s = L'\0';

		++ret->line_count;
		ret->lines = (wchar_t**)realloc(
			ret->lines, (ret->line_count) * sizeof(wchar_t*));
		ret->lines[ret->line_count-1] = tmpLine;
	}

	delete[] wdata_start;
	//free(wdata_start);

	return ret;
}


void CAeraWordPic::ta_destroy(textarea_t * pta)
{
	int i;
	for(i=0; i<pta->line_count; ++i)
		free(pta->lines[i]);
	delete pta;
	
	//free(pta);
}


void CAeraWordPic::ta_scroll(textarea_t* pta, GLfloat range)
{
	GLfloat char_height = (pta->max_y - pta->min_y) / pta->char_in_col;
	GLfloat max_scroll = char_height * (pta->line_count - 1);
	pta->scroll_pos += range;
	if( pta->scroll_pos < 0.0f )
		pta->scroll_pos = 0.0f;
	if( pta->scroll_pos > max_scroll )
		pta->scroll_pos = max_scroll;
}


void CAeraWordPic::ta_display(textarea_t* pta)
{
	int i, start;
	GLfloat texinc = ctbuf_texinc();

	GLfloat char_height = (pta->max_y - pta->min_y) / pta->char_in_col;
	GLfloat char_width = (pta->max_x - pta->min_x) / pta->char_in_row;
	GLfloat char_scroll = fmod(pta->scroll_pos, char_height);
	glBindTexture(GL_TEXTURE_2D, g_texID);
	//glBindTexture(GL_TEXTURE_2D,g_texID);
	// 计算开始显示的行数
	start = (int)(pta->scroll_pos/char_height);


	// 显示每一行的字符
	for(i=0; i<=pta->char_in_col && i+start<pta->line_count; ++i) {
		const wchar_t* p = pta->lines[i+start];
		int j = 0;
		for(; *p!=L'\0'; ++p, ++j) {
			const ctitem_t* pItem = ctbuf_find(*p);
			glBegin(GL_QUADS);
			glTexCoord2f(pItem->texcoord_x, pItem->texcoord_y);
			glVertex2f(
				char_width * j + pta->min_x,
				pta->max_y - char_height * (i+1) + char_scroll);
			glTexCoord2f(pItem->texcoord_x + texinc, pItem->texcoord_y);
			glVertex2f(
				char_width * (j+1) + pta->min_x,
				pta->max_y - char_height * (i+1) + char_scroll);
			glTexCoord2f(pItem->texcoord_x + texinc, pItem->texcoord_y + texinc);
			glVertex2f(
				char_width * (j+1) + pta->min_x,
				pta->max_y - char_height * i + char_scroll);
			glTexCoord2f(pItem->texcoord_x, pItem->texcoord_y + texinc);
			glVertex2f(
				char_width * j + pta->min_x,
				pta->max_y - char_height * i + char_scroll);

			glEnd();
		}
	}
}

void CAeraWordPic::CreateFontLines(const char *data)
{
	//textarea_t* ret = NULL;
	//ta_destroy(p_textarea);
	for(int i=0;i<p_textarea->line_count;i++)
		delete [] p_textarea->lines[i];
	p_textarea->line_count=0;
	wchar_t* wdata = NULL;
	wchar_t* wdata_start = NULL;
	// 把参数中的字符串转化为宽字符
	{
		int len = 0;
		int i;
		for(i=0; data[i]!='\0'; ++i)
		{
			if( IsDBCSLeadByte(data[i]) )
				++i;
			++len;
		}

		wdata =new wchar_t[len+1];// (wchar_t*)malloc((len+1) * sizeof(wchar_t));
		wdata_start = wdata;
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, data, -1, wdata, len);
		wdata[len] = L'\0';
	}

	// 把参数中的字符串拆分成多行，每行正好可以够显示
	p_textarea->line_count = 0;
	p_textarea->lines = NULL;
	while( *wdata != L'\0' ) {
		wchar_t* tmpLine = new wchar_t[p_textarea->char_in_row + 1];//(wchar_t*)malloc((char_in_row + 1) * sizeof(wchar_t));
		wchar_t* s;
		int count = 0;
		for(s=tmpLine; *wdata != L'\0';) {
			wchar_t c;
			*s++ = (c=*wdata++);
			if( ++count == p_textarea->char_in_row || c == L'\n' )
				break;
		}
		*s = L'\0';

		++p_textarea->line_count;
		p_textarea->lines = (wchar_t**)realloc(
			p_textarea->lines, (p_textarea->line_count) * sizeof(wchar_t*));
		p_textarea->lines[p_textarea->line_count-1] = tmpLine;
	}

	delete[] wdata_start;
	//free(wdata_start);
}

void CAeraWordPic::AppendToFontLines(const char *data)
{
	
	wchar_t* wdata = NULL;
	wchar_t* wdata_start = NULL;
	// 把参数中的字符串转化为宽字符
	{
		int len = 0;
		int i,j;
		for(i=0; data[i]!='\0'; ++i)
		{
			if( IsDBCSLeadByte(data[i]) )
				++i;
			++len;
		}

		wdata =new wchar_t[len+1];// (wchar_t*)malloc((len+1) * sizeof(wchar_t));
		wdata_start = wdata;
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, data, -1, wdata, len);
		wdata[len] = L'\0';
	}
	unsigned int row_char_number=p_textarea->char_in_row;
	unsigned int char_num=strlen(data);
	unsigned int line_num=char_num/row_char_number;
	if (char_num%row_char_number!=0)
	{
		line_num++;
	}
	if (line_num>p_textarea->line_count)
	{
		wchar_t **rember=new wchar_t *[p_textarea->line_count];
		for(int k=0;k<p_textarea->line_count;k++)
			rember[k]=p_textarea->lines[k];
		wchar_t *ff=p_textarea->lines[0];
		delete [] p_textarea->lines;
		p_textarea->lines=new wchar_t*[line_num];
	//	(wchar_t**)realloc(
	//p_textarea->lines, (line_num) * sizeof(wchar_t*));
		for(int k=0;k<p_textarea->line_count;k++)
			p_textarea->lines[k]=rember[k];
		for(int i=p_textarea->line_count;i<line_num;i++)
		{
			p_textarea->lines[i]=new wchar_t[p_textarea->char_in_row+1];
			p_textarea->lines[i][p_textarea->char_in_row]=L'\0';

		}
		delete []rember;
	}
	else if(line_num<p_textarea->line_count)
	{
		for(int i=p_textarea->line_count-1;i>=line_num;i--)
			delete []p_textarea->lines[i];
		//delete [] (p_textarea->lines+line_num);
	}
	p_textarea->line_count=line_num;
	int i=0,j=0;
	for(i=0;i<p_textarea->line_count;i++)
	{
		for(j=0;j<p_textarea->char_in_row&&*wdata!=L'\0';j++)
		{
			if (p_textarea->lines[i][j]!=*wdata)
			{
				p_textarea->lines[i][j]=*wdata;
			}
			
			wdata++;
		}
		p_textarea->lines[i][j]=L'\0';
		if (*wdata==0x00)
		{
			break;
		}
	}
	p_textarea->lines[i][j]=L'\0';

	/*int i=0,j=0;
	while(*wdata!=L'\0')
	{
		if(i>p_textarea->line_count)
		{
			p_textarea->line_count++;
			p_textarea->lines=(wchar_t**)realloc(
				p_textarea->lines, (p_textarea->line_count) * sizeof(wchar_t*));
			p_textarea->lines[p_textarea->line_count]=new wchar_t[p_textarea->char_in_col+1];
		}
		wchar_t* tmpLine = p_textarea->lines[i];//(wchar_t*)malloc((char_in_row + 1) * sizeof(wchar_t));
		wchar_t* s;
		int count = 0;
		for(s=tmpLine; *wdata != L'\0';) 
		{
			wchar_t c=*wdata;
			if (*s!=c)
			{
				*s = (c);
			}
			wdata++;
			s++;
			if( ++count == p_textarea->char_in_row || c == L'\n' )
				break;
		}
		*s = L'\0';
		i++;
	}
	*/
	delete [] wdata_start;
}
void CAeraWordPic::display(const char *g_string,GLint fontsize,GLfloat *fontcolor,GLfloat *bkgroundcolor,GLfloat min_x, GLfloat min_y, GLfloat max_x, GLfloat max_y)
{
	
	if (fontcolor!=NULL)
	{
		m_fontcolor=fontcolor;
	}
	if (bkgroundcolor!=NULL)
	{
		m_bkgroundcolor=bkgroundcolor;
	}

	if(true== isFirstCall )
	{
		isFirstCall = false;
		ctbuf_init(fontsize, 256, "隶书");
		//  		glMatrixMode(GL_PROJECTION);
		//  		glLoadIdentity();
		//  		gluOrtho2D(0,1,0,1);
		p_textarea = ta_create(min_x,min_y,max_x,max_y,
			20, 10, g_string);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	}
	else
	{
	//	CreateFontLines(g_string);
		AppendToFontLines(g_string);
	}
 	glDisable(GL_TEXTURE_2D);
 	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);  ///could change
	glRectf(min_x, min_y, max_x, max_y);
	glEnable(GL_TEXTURE_2D);
	ta_display(p_textarea);
	
	// 	glPushAttrib(GL_COLOR_BUFFER_BIT |
	// 		GL_ENABLE_BIT | GL_VIEWPORT_BIT);
	// 	glPushMatrix();
	// 	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	// 	glMatrixMode(GL_MODELVIEW);
	// 	glLoadIdentity();
	// 	glViewport(0,0,24,24);
	// 	glColor3f(0,0,0);
	// 	glBegin(GL_QUADS);
	// 	glVertex2f(0,0);
	// 	glVertex2f(1,0);
	// 	glVertex2f(1,1);
	// 	glVertex2f(0,1);
	// 	glEnd();
	// 	glPopMatrix();
	// 	glPopAttrib();
	// 	glPushAttrib(GL_COLOR_BUFFER_BIT |
	// 		GL_ENABLE_BIT | GL_VIEWPORT_BIT);
	// 	glPushMatrix();
	// 	glViewport(0, 0, g_fontsize, g_fontsize);
	// 	glColor3f(0.0f, 0.0f, 0.0f);
	// 	//glRectf(0,0,1,1);//g_fontsize,g_fontsize);
	// 	//glRectf(-1.0f, -1.0f, 1.0f, 1.0f);
	// 	glPopMatrix();
	// 	glPopAttrib();

	//glutSwapBuffers();
}
