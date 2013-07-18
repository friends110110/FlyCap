
// FlyCapView.h : interface of the CFlyCapView class
//

#pragma once
#include "FrameRateCounter.h"
//#include "FlyCapDoc.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "aerawordpic.h"

// 操作捕获背景消息
#define USER_HANDLE_CAPTURE_BACKGROUND (WM_USER +100)

class CFlyCapView : public CView
{
protected: // create from serialization only
	CFlyCapView();
	DECLARE_DYNCREATE(CFlyCapView)

// Attributes
public:
	CFlyCapDoc * GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CFlyCapView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	double GetDisplayedFrameRate(void);
	bool InitializeOpenGL();
	bool SetupPixelFormat();

	void BindGL( );
	void UnbindGL( );
private:
	FrameRateCounter m_displayedFrameRate;
	bool m_openGLInitialized;
	bool m_enableOpenGL;

	/** OpenGL rendering context. */
	HGLRC m_hRC;

	/** Device context for drawing. */
	CDC* m_pDC;

	static const unsigned int sk_maxNumTextures = 16;
	GLuint m_imageTextures[sk_maxNumTextures];
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
//	afx_msg void OnPaint();
	//x,y 坐标点 区域宽度高度  图片宽度 高度
	bool SetPositionImage(CDC *pDc,CFlyCapDoc *pDoc,PUCHAR pImageData,float x, float y,float Clientwidth,float Clientheight, int width, int height,int locationflags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	CAeraWordPic m_caerawordpic;
	afx_msg void OnOperateGetbackgroundimage();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnOperateFullscreen();
	afx_msg void OnUpdateOperateFullscreen(CCmdUI *pCmdUI);


	afx_msg LRESULT DoHandleCaptureBackgroundMessage(WPARAM wParam,LPARAM lParam);
public:
	// 调试模式和展示模式的切换
	bool m_keyBoardFrameFlag;
public:
	afx_msg void OnUpdateOperateShowDebugFrameMode(CCmdUI *pCmdUI);
	afx_msg void OnOperateShowDebugFrameMode();
	void SaveDCtoBmp(CDC * pMemDC,CArchive *ar);
};

#ifndef _DEBUG  // debug version in FlyCapView.cpp
inline CFlyCapDoc* CFlyCapView::GetDocument() const
   { return reinterpret_cast<CFlyCapDoc*>(m_pDocument); }
#endif

