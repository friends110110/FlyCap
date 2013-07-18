
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#define WINDOWS_WIDTH 1280
#define WINDOWS_HEIGHT 860
class CMainFrame : public CFrameWndEx
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

public:
	UINT_PTR m_timer;
	RECT m_rectStatus;
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void ResizeToFit(void);
	void UpdateStatusBar(void);
	afx_msg void OnDestroy();
	CMFCStatusBar * GetStatusBar(void);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

	//used for fullscreen
	bool m_bFullScreenMode;
	CMenu m_menuMainWnd;


	//used for fullscreen
private:
	bool m_bStatusBarWasVisible;
	bool m_bToolBarWasVisible;
	CRect m_mainRect;
public:
	void FullScreenModeOn(void);
	void FullScreenModeOff(void);
	void FullScreenModeSwitch(void);
};


