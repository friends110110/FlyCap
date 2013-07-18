#pragma once
typedef struct __ctitem__ 
{
	char valid;
	wchar_t c;
	GLfloat texcoord_x;
	GLfloat texcoord_y;
} ctitem_t;


typedef struct __textarea__ {
	// 显示区域
	GLfloat min_x;
	GLfloat min_y;
	GLfloat max_x;
	GLfloat max_y;

	// 可以显示的行列数
	int char_in_row;
	int char_in_col;

	// 滚动位置
	GLfloat scroll_pos;

	// 总行数
	int line_count;

	// 每行的字符，lines[i]表示第i行字符
	wchar_t** lines;
} textarea_t;


class CAeraWordPic
{
public:
	CAeraWordPic(void);
	~CAeraWordPic(void);
private:
	int g_fontsize;
	GLuint g_texID ; 
	int g_count ;
	int g_total ;
	int g_texturesize ;
	ctitem_t* g_items ;
	textarea_t* p_textarea ;
	GLfloat *m_bkgroundcolor;
	GLfloat *m_fontcolor;
public:
	void selectFont(int size, int charset, const char* face);
	void drawChar_To_SubTexture(wchar_t w, int texcoord_x, int texcoord_y);
	void ctbuf_destroy(void);
	void ctbuf_init(int fontsize, int texturesize, const char* font);
	void ctbuf_bind(void);
	ctitem_t *ctbuf_find(wchar_t c);
	void ctbuf_drawString(const char* str, GLfloat cwidth, GLfloat cheight);
	GLfloat ctbuf_texinc(void);
	textarea_t * ta_create(GLfloat min_x, GLfloat min_y, GLfloat max_x, GLfloat max_y,
		int char_in_row, int char_in_col, const char* data);
	void ta_destroy(textarea_t * pta);
	void ta_scroll(textarea_t* pta, GLfloat range);
	void ta_display(textarea_t* pta);
	void display(const char *g_string,GLint fontsize=24,GLfloat *fontcolor=NULL,GLfloat *bkgroundcolor=NULL,GLfloat min_x=0.1, GLfloat min_y=0.2, GLfloat max_x=0.5, GLfloat max_y=0.6);
	void CreateFontLines(const char *data);
	void AppendToFontLines(const char *g_string);

	bool isFirstCall;
};

