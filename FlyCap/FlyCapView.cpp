
// FlyCapView.cpp : implementation of the CFlyCapView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "FlyCap.h"
#endif
#include "MainFrm.h"
#include "FlyCapDoc.h"
#include "FlyCapView.h"
#include "FontDisplay.h"
#include "AeraWordPic.h"
#include <iostream>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFlyCapView

IMPLEMENT_DYNCREATE(CFlyCapView, CView)

BEGIN_MESSAGE_MAP(CFlyCapView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFlyCapView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
//	ON_WM_PAINT()
ON_WM_MOUSEMOVE()
ON_COMMAND(ID_OPERATE_GETBACKGROUNDIMAGE, &CFlyCapView::OnOperateGetbackgroundimage)
ON_WM_KEYDOWN()
ON_COMMAND(ID_OPERATE_FULLSCREEN, &CFlyCapView::OnOperateFullscreen)
ON_UPDATE_COMMAND_UI(ID_OPERATE_FULLSCREEN, &CFlyCapView::OnUpdateOperateFullscreen)

ON_MESSAGE(USER_HANDLE_CAPTURE_BACKGROUND,& CFlyCapView::DoHandleCaptureBackgroundMessage)

ON_UPDATE_COMMAND_UI(ID_OPERATE_SHOW_DEBUG_FRAMEMODE, &CFlyCapView::OnUpdateOperateShowDebugFrameMode)
ON_COMMAND(ID_OPERATE_SHOW_DEBUG_FRAMEMODE, &CFlyCapView::OnOperateShowDebugFrameMode)
END_MESSAGE_MAP()




int GetMinimumPowerOfTwo(int in)
{
	int i = 1;
	while ( i < in)
	{
		i *= 2;
	}

	return i;
}

// CFlyCapView construction/destruction

CFlyCapView::CFlyCapView()
	: m_keyBoardFrameFlag(false)
{
	// TODO: add construction code here
	m_enableOpenGL=true;
	m_openGLInitialized=false;
	m_keyBoardFrameFlag=false;
}

CFlyCapView::~CFlyCapView()
{
	 glDeleteTextures( sk_maxNumTextures, m_imageTextures );
}

BOOL CFlyCapView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CFlyCapView drawing

void CFlyCapView::OnDraw(CDC* pDC)
{
	CFlyCapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	//CView::OnDraw(pDC);
	// TODO: add draw code for native data here
/*
	CRect rect;
	GetWindowRect(&rect);
	float Clientwidth=rect.right-rect.left;
	float Clientheight=rect.bottom-rect.top;
	SetPositionImage(pDC,pDoc,0,Clientheight/2,Clientwidth/2,Clientheight/2,width,height);

	SetPositionImage(pDC,pDoc,Clientwidth/2,0,Clientwidth/2,Clientheight/2,width,height);
	m_displayedFrameRate.NewFrame();

	dataLock.Unlock();
	*/
	int width = pDoc->m_bitmapInfo.bmiHeader.biWidth;
	int height = ::abs( pDoc->m_bitmapInfo.bmiHeader.biHeight );



	((CMainFrame*)GetParentFrame())->ResizeToFit();
	((CMainFrame*)GetParentFrame())->UpdateStatusBar();

// Transfer the RGB buffer to graphics card.
	

	CRect rect;
	CSingleLock dataLock( &pDoc->m_csData );
	dataLock.Lock();

// 		// Just one texture
// 		BindGL( );
// 		
// 		GetWindowRect(&rect);
// 		float Clientwidth=rect.right-rect.left;
// 		float Clientheight=rect.bottom-rect.top;
// 	
// 		glMatrixMode(GL_MODELVIEW);
// 		glLoadIdentity();
// 		glClearColor(0,0,0,0);
// 		glClear(GL_COLOR_BUFFER_BIT);

//#ifdef KEYBOARDFRAME
	if(m_keyBoardFrameFlag==true)
	{
		if (pDoc->m_projectImage!=NULL)
		{
			width=pDoc->m_projectImage->GetCols();
			height=pDoc->m_projectImage->GetRows();
		}

// 		CImage image;
// 		image.LoadFromResource(theApp.m_hInstance,IDB_BITMAP_KEYBOARD);
		CBitmap bitmap;     //定义位图变量

		bitmap.LoadBitmap(IDB_BITMAP_KEYBOARD);  //获得位图ID
		BITMAP bmp;
		bitmap.GetBitmap(&bmp);
		CDC dc;
		dc.CreateCompatibleDC(pDC);         //创建DC
		dc.SelectObject(&bitmap);         //选择DC设备描述表
		CRect my_rect;
		GetClientRect(&my_rect);		
		
		//显示文字用的
	
		CString str;
		str=pDoc->GetHasClickedKey();
		//str=pDoc->m_imageDao->GetHasClickedKey();
		int strlength=str.GetLength();
	
		if (strlength!=0&&false!=pDoc->m_isBackgroundImageOperateFlag)
		{
			
			CFont font;
			//font.CreatePointFont(400,"Times New Roman",&dc);
			font.CreateFont(
				100,		//height
				20,			//width
				0,
				0,
				FW_NORMAL,
				FALSE,
				FALSE,
				0,
				ANSI_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH|FF_SWISS,
				_T("Times New Roman")
				);
			CFont *oldfont=dc.SelectObject(&font);
			dc.SetTextCharacterExtra(10);

			
	
			//pDC->SetTextAlign(TA_CENTER );
			dc.SetBkColor(RGB(0,0,0));
			dc.SetTextColor(RGB(24,203,33));
			CSize sz=dc.GetTextExtent(str);	
			//对齐x到B 之上
			int draw_start_x=pDoc->m_keypositionVector->at(1).x0;//10;
			draw_start_x=100;
			int draw_start_y=pDoc->m_projectImageSeparateLocationy-sz.cy-5;
			dc.ExtTextOutA(draw_start_x,draw_start_y,ETO_OPAQUE,CRect(CPoint(draw_start_x,draw_start_y),sz),str,strlength,NULL);

			dc.SelectObject(oldfont);		
			font.DeleteObject();
			//pDC->DrawText()
		}	
		//贴图
		//dc.SetBkMode(TRANSPARENT);
		pDC->StretchBlt(0,0,my_rect.Width(),my_rect.Height(),&dc,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
		
		//保存当前 dc到 bmp
		CFile f;
		char buf[512];
		if( !f.Open( "Currentcdc.bmp", CFile::modeCreate | CFile::modeWrite ) )
		{

		}
		CArchive ar( &f, CArchive::store, 512, buf );
		this->SaveDCtoBmp(&dc,&ar);
		ar.Close();
		f.Close();


		//clean the memory
		ReleaseDC(&dc);

	
		//CRect(CPoint(10,20),CSize(100,20))

		//str=pDoc->GetHasClickedKey();
		//CRect(CPoint(10,20),CSize(100,20))

		//pDoc->m_projectImageSeparateLocationy-20
		//pDC->TextOutA(10,30,"WFWF");

//		SetPositionImage(pDC,pDoc,pDoc->GetProjectedPixels(),0,0,Clientwidth,Clientheight,width,height,1);
// 		CAeraWordPic caerawordpic;
// 		caerawordpic.display("asdf");
// 		static char x='a';
// 		static std::string aa="";
// 		aa+='w';
	}
	
//#else	
	else
	{
		BindGL( );
		GetWindowRect(&rect);
		float Clientwidth=rect.right-rect.left;
		float Clientheight=rect.bottom-rect.top;
	
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT);

		glColor3f(1,0,0);
		glViewport(0,0,Clientwidth,Clientheight);			//draw lines
		glLineWidth(3.0);
		glBegin(GL_LINES);
// 		glVertex2f(0,0.5);
// 		glVertex2f(1,0.5);
// 		glVertex2f(0.5,0);
// 		glVertex2f(0.5,1);
		glVertex2f(0,0.5);
		glVertex2f(1,0.5);
		glVertex2f(1.0/3,0);
		glVertex2f(1.0/3,1);
		
		glVertex2f(2.0/3,0);
		glVertex2f(2.0/3,1);
		glEnd();

			/************************************************************************/
			/* left up                                                                     */
			/************************************************************************/
		if(pDoc->m_getfirstPicFlag==true)
		{
			//			SetPositionImage(pDC,pDoc,pDoc->GetProcessedPixels(),0,Clientheight/2,Clientwidth/2,Clientheight/2,width,height,1);
			SetPositionImage(pDC,pDoc,pDoc->GetProcessedPixels(),0,Clientheight/2,Clientwidth/3,Clientheight/2,width,height,1);
			//SetPositionImage(pDC,pDoc,pDoc->GetGRAYPixels(),0,Clientheight/2,Clientwidth/2,Clientheight/2,width,height,1);
	//......//
			//glViewport(Clientwidth/2,Clientheight/2,Clientwidth/2,Clientheight/2);
	// 		CFontDisplay fontdis;
	// 		GLfloat fontcolor[3]={1,0,0};
	// 		GLfloat fontlocation[2]={0.2,0.3};
	// 		fontdis.display(48,"我是啊",fontlocation,fontcolor);
	//.......//	
			/************************************************************************/
			/* right up                                                                     */
			/************************************************************************/
			SetPositionImage(pDC,pDoc,pDoc->GetGRAYPixels(),Clientwidth/3,Clientheight/2,Clientwidth/3,Clientheight/2,width,height,2);
			if (true==pDoc->m_isBackgroundImageOperateFlag)
			{
				SetPositionImage(pDC,pDoc,pDoc->GetBackgroundPixels(),Clientwidth*2/3,Clientheight/2,Clientwidth/3,Clientheight/2,width,height,2);
				
				//SetPositionImage(pDC,pDoc,pDoc->GetAfterBackgroundMinusImage(),Clientwidth/2,Clientheight/2,Clientwidth/2,Clientheight/2,width,height,2);
			
				SetPositionImage(pDC,pDoc,pDoc->GetAfterBackgroundMinusImage(),0,0,Clientwidth/3,Clientheight/2,width,height,2);


			/************************************************************************/
			/* left down                                                                     */
			/************************************************************************/
				//SetPositionImage(pDC,pDoc,pDoc->Get2BImagePixels(),0,0,Clientwidth/2,Clientheight/2,width,height,3); 		
				
				SetPositionImage(pDC,pDoc,pDoc->Get2BImagePixels(),Clientwidth/3,0,Clientwidth/3,Clientheight/2,width,height,2);
				//SetPositionImage(pDC,pDoc,pDoc->GetBackgroundPixels(),0,0,Clientwidth/2,Clientheight/2,width,height,3); 	
			/************************************************************************/
			/* right down                                                                     */
			/************************************************************************/
		/*	glViewport(Clientwidth/2,0,Clientwidth/2,Clientheight/2);
			m_caerawordpic.display("wfkjhkkhugjgjgjhgjgjgjgdtfryfhfaslkjfjlaksjdfijasoifqkhwnfjknbsdfbgjhwhfilknasjbfwjltgbhjasbdjnbdskhbgjhsdkjfgbkjabhsfjhabshfgasjdgf");
	*/
				//SetPositionImage(pDC,pDoc,pDoc->getShowPixels(),Clientwidth/2,0,Clientwidth/2,Clientheight/2,width,height,4);
				SetPositionImage(pDC,pDoc,pDoc->getShowPixels(),Clientwidth*2/3,0,Clientwidth/3,Clientheight/2,width,height,1);
				//SetPositionImage(pDC,pDoc,pDoc->GetGRAYPixels(),Clientwidth/2,0,Clientwidth/2,Clientheight/2,width,height,4);
			
				//画线
				glColor3f(1,0,0);
				glViewport(0,0,Clientwidth,Clientheight);			//draw lines
				glLineWidth(3.0);
				glBegin(GL_LINES);
				glVertex2f(0,0.5);
				glVertex2f(1,0.5);
				glVertex2f(1.0/3,0);
				glVertex2f(1.0/3,1);

				glVertex2f(2.0/3,0);
				glVertex2f(2.0/3,1);
				glEnd();
			}
	// 		
	// 		//CAeraWordPic caerawordpic;
 			//static char x='a';
	// 		static std::string aa="";
	// 		aa+='w';
	 	
		}
	// // 	CAeraWordPic caerawordpic;
	// // 	caerawordpic.display("asdf");
 		SwapBuffers( pDC->m_hDC );
 		UnbindGL( );

		CString str;
		str=pDoc->GetHasClickedKey();
		int strlen=str.GetLength();
		//str="fsfwefwf";
		if (strlen!=0)
		{
			CFont font;
			font.CreatePointFont(400,"Times New Roman",pDC);
			CFont *oldfont=pDC->SelectObject(&font);
			pDC->SetTextCharacterExtra(2);
			CSize sz=pDC->GetTextExtent(str);


			pDoc->m_projectImageSeparateLocationy;
	
			//pDC->SetTextAlign(TA_CENTER );
			pDC->ExtTextOutA(10,20,ETO_OPAQUE,CRect(CPoint(10,20),sz),str,str.GetLength(),NULL);

			pDC->SelectObject(oldfont);
			//
		}
	}
//#endif

	m_displayedFrameRate.NewFrame();
	
	

	// Clean up memory

	dataLock.Unlock();	


}


// CFlyCapView printing


void CFlyCapView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CFlyCapView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CFlyCapView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CFlyCapView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CFlyCapView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CFlyCapView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CFlyCapView diagnostics

#ifdef _DEBUG
void CFlyCapView::AssertValid() const
{
	CView::AssertValid();
}

void CFlyCapView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFlyCapDoc* CFlyCapView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFlyCapDoc)));
	return (CFlyCapDoc*)m_pDocument;
}
#endif //_DEBUG


// CFlyCapView message handlers


double CFlyCapView::GetDisplayedFrameRate(void)
{
	return m_displayedFrameRate.GetFrameRate();
}


int CFlyCapView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	if ( m_enableOpenGL)
	{
		if( !InitializeOpenGL() )
		{
			return -1;
		}
		else
		{
			m_openGLInitialized = true;
		}
	}

	return 0;
}


void CFlyCapView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	if (m_openGLInitialized)
	{
		// Make the RC non-current
		UnbindGL( );

		// Delete the rendering context
		if ( ::wglDeleteContext( m_hRC ) == FALSE )
		{
			MessageBox("Could not Make RC non-Current.");
		}

		// Delete DC
		if ( m_pDC )
		{
			delete m_pDC;
			m_pDC = NULL;
		}        
	}
}


BOOL CFlyCapView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return 0;		//text directly
	//return true;
	//return CView::OnEraseBkgnd(pDC);
}


void CFlyCapView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if( ( cx <= 0 ) || ( cy <= 0 ) )
	{
		return;
	}

	if ( m_enableOpenGL)
	{
		BindGL( );
		::glViewport( 0, 0, cx, cy );
		UnbindGL( );
	}
}


void CFlyCapView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CFlyCapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	m_openGLInitialized = InitializeOpenGL();

	RECT clientRect;
	GetClientRect( &clientRect );

	// Resize the window to properly display the image
	GetParentFrame()->SetWindowPos( NULL,
		0, 0, clientRect.right, clientRect.right/2,
		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER );
}

bool CFlyCapView::InitializeOpenGL()
{
	if ( m_openGLInitialized )
	{
		// Nothing to be done here
		return true;
	}

	// Get a DC for the Client Area
	m_pDC = new CClientDC( this );
	if ( m_pDC == NULL )
	{
		MessageBox("Error Obtaining DC");
		return false;
	}

	// Set Pixel Format
	if ( !SetupPixelFormat() )
	{
		return false;
	}

	// Create Rendering Context
	m_hRC = ::wglCreateContext( m_pDC->GetSafeHdc() );
	if( m_hRC == NULL )
	{
		MessageBox("Error Creating RC.");;
		return false;
	}

	// initialize OGL texture
	BindGL();

#define GL_CLAMP_TO_EDGE 0x812F

	glGenTextures( sk_maxNumTextures, m_imageTextures );

	bool useClampToEdge = true;
	if ( atof( (const char*)glGetString(GL_VERSION)) < 1.15)
	{
		useClampToEdge = false;
	}

	for ( int i = 0; i < sk_maxNumTextures; i++)
	{
		glBindTexture( GL_TEXTURE_2D, m_imageTextures[ i] );

		if ( useClampToEdge)
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		}
		else
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
		}

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	}

	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	glShadeModel( GL_SMOOTH );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	//
	// initialize matrices
	//
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0, 1, 0, 1 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	UnbindGL();

	return true; 
}
	
bool CFlyCapView::SetupPixelFormat()
{
	// Fill in the Pixel Format Descriptor
	PIXELFORMATDESCRIPTOR pfd;
	memset( &pfd, 0x0, sizeof( PIXELFORMATDESCRIPTOR ) );

	pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
	pfd.nVersion = 1;
	pfd.dwFlags =	
		PFD_DOUBLEBUFFER |
		PFD_SUPPORT_OPENGL |
		PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cAlphaBits = 0;
	pfd.cDepthBits = 0;

	int nPixelFormat = ::ChoosePixelFormat( m_pDC->m_hDC, &pfd );
	if( nPixelFormat == 0 )
	{
		ASSERT( FALSE );
		return false;
	}

	if( !::SetPixelFormat( m_pDC->m_hDC, nPixelFormat, &pfd ) )
	{
		ASSERT( FALSE );
		return false;
	}

	return true;
}


void CFlyCapView::BindGL( )
{
	if( !::wglMakeCurrent( m_pDC->m_hDC, m_hRC ) )
	{
		MessageBox("Error binding OpenGL.");
	}
}

void CFlyCapView::UnbindGL( )
{
	if( !::wglMakeCurrent( m_pDC->m_hDC, NULL ) )
	{
		MessageBox("Error unbinding OpenGL.");
	}
}

//void CFlyCapView::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting
//	// TODO: 在此处添加消息处理程序代码
//	// 不为绘图消息调用 CView::OnPaint()
//	OnPrepareDC(&dc);
//	OnDraw(&dc);
//}


bool CFlyCapView::SetPositionImage(CDC *pDC,CFlyCapDoc *pDoc,PUCHAR pImageData,float x, float y,float Clientwidth,float Clientheight, int width, int height,int locationflags)
{
	
	
	if( pImageData == NULL ||pDC==NULL||pDoc==NULL)
	{
		return false;
	}

	if ( m_enableOpenGL)
	{
		double validTextureWidth = 1.0;
		double validTextureHeight = 1.0;
		bool useTiledTextures = false;
		GLenum errorno;

		GLint colorcomponents;
		GLenum colorformat;
		//测试  locationflags==1 之后去掉
		if (locationflags==2)//||locationflags==3)//||locationflags==1)
		{
			colorcomponents=GL_LUMINANCE;
			colorformat=GL_LUMINANCE ;
		}
		else
		{
			 colorcomponents=GL_RGB;//;//means the number of color kinds
			 colorformat=GL_BGR_EXT;	//means color pixel component
		}
		

		//BindGL( );
		glEnable( GL_TEXTURE_2D );
		//glMatrixMode( GL_MODELVIEW );
		//glLoadIdentity();

		glBindTexture( GL_TEXTURE_2D, m_imageTextures[0] );
		glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			colorcomponents, 
			width, 
			height, 
			0, 
			colorformat, 
			GL_UNSIGNED_BYTE,
			pImageData);//pDoc->GetProcessedPixels() );
		errorno = glGetError();
		if ( errorno != GL_NO_ERROR)
		{
			// Attempt to fall back and use a power-of-two sized texture.
			// This is for older cards that don't support more arbitrary
			// texture sizes.

			const int textureWidth = GetMinimumPowerOfTwo(width);
			const int textureHeight = GetMinimumPowerOfTwo(height);
			validTextureWidth = (double)width / textureWidth;
			validTextureHeight = (double)height / textureHeight;

			glTexImage2D(
				GL_TEXTURE_2D, 
				0, 
				colorcomponents, 
				textureWidth, 
				textureHeight, 
				0, 
				colorformat, 
				GL_UNSIGNED_BYTE,
				NULL );
			errorno = glGetError();
			if ( errorno != GL_NO_ERROR)
			{
				// The graphics doesn't seem to support this texture size.
				// Images must be split and then tiled.
				useTiledTextures = true;
			}
			else
			{
				glTexSubImage2D( 
					GL_TEXTURE_2D, 
					0, 
					0, 
					0,
					width, 
					height,
					colorformat, 
					GL_UNSIGNED_BYTE, 
					pImageData);//pDoc->GetProcessedPixels() );
				errorno = glGetError();
				if ( errorno != GL_NO_ERROR)
				{
					// Error
				}
			}
		}


		glViewport(x,y,Clientwidth,Clientheight);
		glBegin( GL_QUADS );

		glTexCoord2d( 0.0, validTextureHeight );
		//glTexCoord2d( 0.0, 0.0 );
		glVertex2d( 0.0, 0.0 );

		glTexCoord2d( validTextureWidth, validTextureHeight );
		//glTexCoord2d( validTextureWidth, 0.0 );
		glVertex2d( 1.0, 0.0 );

		glTexCoord2d( validTextureWidth, 0.0 );
		//glTexCoord2d( validTextureWidth, validTextureHeight );
		glVertex2d( 1.0, 1.0 );

		glTexCoord2d( 0.0, 0.0 );
		//glTexCoord2d( 0.0, validTextureHeight );
		glVertex2d( 0.0, 1.0 );

		glEnd();
	return true;
	}
	return false;
}

void CFlyCapView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString str;
	str.Format("[x=%d,y=%d]",point.x,point.y);
	CMainFrame *pFrame=(CMainFrame *)AfxGetApp()->m_pMainWnd; 
	pFrame->GetStatusBar()->SetPaneText(1,str);				//the second location
	CView::OnMouseMove(nFlags, point);
}


void CFlyCapView::OnOperateGetbackgroundimage()
{
	// TODO: 在此添加命令处理程序代码
	CFlyCapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	//把按键信息清空后 然后发送 WM_PAINT消息
	pDoc->CaptureBackgroundImagePreProcess();

	//在此之后，发送获取图片USER_HANDLE_CAPTURE_BACKGROUND 消息  ，自己定义的
	//AfxGetMainWnd()->PostMessageA(USER_HANDLE_CAPTURE_BACKGROUND,NULL,NULL);
	LPARAM lparam=(LPARAM)m_keyBoardFrameFlag;
	//测试注释，背景更新 
	this->PostMessageA(USER_HANDLE_CAPTURE_BACKGROUND,NULL,lparam);
	

}


void CFlyCapView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CMainFrame *pMain=(CMainFrame *)AfxGetApp()->m_pMainWnd;
	
	//按了F11	
	if (nChar== VK_F11)
	{
		pMain->FullScreenModeSwitch();
	}
	else if (nChar==VK_ESCAPE)
	{
		//AfxGetMainWnd()->SendMessage(WM_CLOSE);//关闭应用程序
		//DestroyWindow();
		exit(0);
	}
	else if(nChar==VK_F2)   
	{
		OnOperateGetbackgroundimage();    
	}
	else if (nChar==VK_F3)
	{
		OnOperateShowDebugFrameMode();
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CFlyCapView::OnOperateFullscreen()
{
	// TODO: 在此添加命令处理程序代码
	CMainFrame *pMain=(CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMain->FullScreenModeSwitch();
	//pMain->m_menuMainWnd.set   ;(ID_OPERATE_FULLSCREEN,"取消全屏");
// 	CMFCMenuBar *pmenu=(CMFCMenuBar *)GetMenu();
// 	pmenu->SetDlgItemTextA(ID_OPERATE_FULLSCREEN,"取消全屏");
// 	pmenu->Detach();
}



void CFlyCapView::OnUpdateOperateFullscreen(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	CMainFrame *pMain=(CMainFrame *)AfxGetApp()->m_pMainWnd;
	if (true==pMain->m_bFullScreenMode)
	{
		pCmdUI->SetText("取消全屏");
	}
	else
	{
		pCmdUI->SetText("全屏");
	}
	
}


LRESULT CFlyCapView::DoHandleCaptureBackgroundMessage(WPARAM wParam,LPARAM lParam)
{
	CFlyCapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return false;
	LPARAM lpwaram=(LPARAM)m_keyBoardFrameFlag;
	pDoc->DoHandleCaptureBackground(NULL,lParam);

	
	return true;
}


void CFlyCapView::OnUpdateOperateShowDebugFrameMode(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if(m_keyBoardFrameFlag==false)
	{
		pCmdUI->SetText("显示框架模式");
	}
	else
	{
		pCmdUI->SetText("调试模式");
	}
}


void CFlyCapView::OnOperateShowDebugFrameMode()
{
	// TODO: 在此添加命令处理程序代码
	m_keyBoardFrameFlag=!m_keyBoardFrameFlag;
}


void CFlyCapView::SaveDCtoBmp(CDC * pMemDC,CArchive *ar)
{
	BITMAPINFO bi;
	BITMAPINFOHEADER *pbi= &bi.bmiHeader;

	HBITMAP hbm = ::CreateCompatibleBitmap(pMemDC->m_hDC,
		1, 1);
	HBITMAP hold = (HBITMAP)::SelectObject(pMemDC->m_hDC,
		hbm);
	pbi->biSize = sizeof(BITMAPINFOHEADER);     
	pbi->biBitCount = 0;     

	int n =
		GetDIBits(pMemDC->m_hDC,hold,0,1,NULL,&bi,DIB_RGB_COLORS);
	pbi->biBitCount = 24;
	pbi->biCompression = 0;
	pbi->biSizeImage = pbi->biWidth * pbi->biHeight * 3;
	BYTE *pBits = new BYTE[pbi->biSizeImage];

	n = GetDIBits(pMemDC->m_hDC,hold,0,pbi->biHeight,
		pBits, &bi, DIB_RGB_COLORS);
	hbm = (HBITMAP)::SelectObject(pMemDC->m_hDC, hold);
	DeleteObject(hbm);

	BITMAPFILEHEADER hdr;
	hdr.bfType=0x4D42;

	hdr.bfSize=sizeof(BITMAPFILEHEADER)+pbi->biSize+pbi->biSizeImage;
	hdr.bfReserved1 = hdr.bfReserved2 = 0;

	hdr.bfOffBits=(DWORD)sizeof(BITMAPFILEHEADER)+pbi->biSize;

	ar->Write(&hdr,sizeof(BITMAPFILEHEADER));
	ar->Write(pbi,sizeof(BITMAPINFOHEADER));
	ar->Write(pBits,pbi->biSizeImage);
	delete [] pBits;
}
