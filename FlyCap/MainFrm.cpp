
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "FlyCap.h"
#include "FlyCapDoc.h"
#include "MainFrm.h"
#include "FlyCapView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
//	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	//ID_INDICATOR_CAPS,
	//ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	//ID_SEPARATOR,
	//ID_INDICATOR_NUM,
	//ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
	: m_timer(0)
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
	
	//初始化全屏状态为false
	this->m_bFullScreenMode=false;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	// Remove the maximize button
	ModifyStyle(WS_MAXIMIZEBOX, 0);

	/*
	// Remove the maximize button
	ModifyStyle(WS_MAXIMIZEBOX, 0);

	// Remove the ability to resize the window
	ModifyStyle(WS_THICKFRAME, 0);
	*/
	BOOL bNameValid;
	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// Allow user-defined toolbars operations:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Enable toolbar and docking window menu replacement
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// enable quick (Alt+drag) toolbar customization
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// load user-defined toolbar images
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// enable menu personalization (most-recently used commands)
	// TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);


	CMFCToolBar::SetBasicCommands(lstBasicCommands);
	if( m_timer == NULL )						// set timer
	{
		m_timer = ::SetTimer(m_hWnd, 123456, 100, (TIMERPROC)NULL);   
	}

 	//this->m_menuMainWnd.LoadMenu(IDR_MAINFRAME);
 	//this->SetMenu(&m_menuMainWnd);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style|=WS_VISIBLE|WS_MAXIMIZE;			// 后加最大化

	cs.style=cs.style&(~WS_THICKFRAME);			//后加进制修改大小

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// base class does the real work

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// enable customization button for all user toolbars
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}



//void CMainFrame::OnTimer(UINT_PTR nIDEvent)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	UpdateStatusBar();
//	CFrameWndEx::OnTimer(nIDEvent);
//}


void CMainFrame::ResizeToFit(void)
{
	/*
	CFlyCapDoc *pDoc=(CFlyCapDoc*)GetActiveDocument();
	if (pDoc==NULL)
	{
		return;
	}
	unsigned int width = 0;
	unsigned int height = 0;

	pDoc->GetImageSize( &width, &height );

	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.bottom = height;
	rect.right = width;

	GetActiveView()->CalcWindowRect( &rect, adjustOutside );   

	static bool first = true;
	if ( first)
	{
		first = false;
		//m_wndToolBar.GetWindowRect( &m_rectTool );
		m_wndStatusBar.GetWindowRect( &m_rectStatus );
	}

	if( m_wndStatusBar.IsWindowVisible() )
	{
		rect.bottom += m_rectStatus.bottom - m_rectStatus.top;
	}

	AdjustWindowRectEx( &rect, GetStyle(), TRUE, GetExStyle() );
	*/ 
	if(false==m_bFullScreenMode)
	{
		SetWindowPos( 
			NULL, 
			0, 
			0, 
			WINDOWS_WIDTH,//rect.right - rect.left, 
			WINDOWS_HEIGHT,//rect.bottom - rect.top, 
			SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER );
	}
}


void CMainFrame::UpdateStatusBar(void)
{
	if (true==m_bFullScreenMode)			//	全屏反正看不见就不更新了
	{
		return;
	}
	CString status;
	CFlyCapDoc* pDoc = (CFlyCapDoc*)GetActiveDocument();
	CFlyCapView* pView = (CFlyCapView*)GetActiveView();

	if ( pDoc == NULL || pView == NULL )
	{
		status = "N/A";
		m_wndStatusBar.SetPaneText( 0, status );
		return;
	}

	unsigned int width;
	unsigned int height;
	pDoc->GetImageSize( &width, &height );

	double processedFPS = pDoc->GetProcessedFrameRate();
	double displayedFPS = pView->GetDisplayedFrameRate();
	
	CString str=pDoc->m_front_background_Pixelstatics;
	
	status.Format( 
		"Processed FPS: %3.2fHz Displayed FPS: %3.2fHz Image dimensions: (%u x %u)", 
		processedFPS, 
		displayedFPS,
		width,
		height );
	m_wndStatusBar.SetPaneText( 0, status );

	m_wndStatusBar.SetPaneText(2,str);
	//m_wndStatusBar.SetPaneText(1,"wfwfwf");
}


void CMainFrame::OnDestroy()
{
	CFrameWndEx::OnDestroy();
	if (m_timer!=NULL)
	{
		::KillTimer(m_hWnd,m_timer);
	}
	// TODO: 在此处添加消息处理程序代码
}


CMFCStatusBar * CMainFrame::GetStatusBar(void)
{
	return &m_wndStatusBar;
}


void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)					//设置标题
{
	// TODO: 在此添加专用代码和/或调用基类

	CFrameWndEx::OnUpdateFrameTitle(bAddToTitle);
	SetWindowText("基于计算机视觉感知的指尖检测软件V1.O");
}


void CMainFrame::FullScreenModeOn(void)
{
	m_bToolBarWasVisible=(m_wndToolBar.IsWindowVisible()!=0);//隐藏工具栏之前工具栏的显示状态    
	
	//隐藏工具栏 
	//m_wndToolBar.ShowWindow(SW_HIDE);  以前版本风格
	this->ShowPane(&m_wndToolBar,FALSE,FALSE,TRUE);
	
	m_bStatusBarWasVisible=(m_wndStatusBar.IsWindowVisible()!=0);//隐藏状态栏之前状态栏的显示状态 
	//m_wndStatusBar.ShowWindow(SW_HIDE);//隐藏状态栏 
	this->ShowPane(&m_wndStatusBar,FALSE,FALSE,TRUE);

	//隐藏菜单栏 
	//this->SetMenu(NULL); 
	//新版本
	//this->ShowPane(&m_wndMenuBar,FALSE,FALSE,TRUE);
	// 保存以前的位置信息 

	GetWindowRect(&m_mainRect); 

	// 去除主窗口的标题 
	LONG style=::GetWindowLong(m_hWnd,GWL_STYLE); 
	style&=~WS_CAPTION; 
	::SetWindowLong(m_hWnd,GWL_STYLE,style); 
	

	//得到当前系统的分辨率 
	int screenx=GetSystemMetrics(SM_CXSCREEN); 
	int screeny=GetSystemMetrics(SM_CYSCREEN); 

	// 全屏显示 
	SetWindowPos(NULL,-4,-4,screenx+8,screeny+8,SWP_NOZORDER); 
	style=::GetWindowLong(this->m_hWnd,GWL_STYLE); 

	//设置全屏显示标志 
	m_bFullScreenMode=true; 
}


void CMainFrame::FullScreenModeOff(void)
{
	//恢复窗口标题 
	LONG style=::GetWindowLong(m_hWnd,GWL_STYLE); 
	style|=WS_CAPTION; 
	::SetWindowLong(m_hWnd,GWL_STYLE,style); 
	//如果需要，显示工具栏 
	if(m_bToolBarWasVisible) 
	{
		//
		//m_wndToolBar.ShowWindow(SW_SHOW); 
		this->ShowPane(&m_wndToolBar,TRUE,FALSE,TRUE);
	}
	//如果需要，显示状态栏 
	if(m_bStatusBarWasVisible) 
	{
		m_wndStatusBar.ShowWindow(SW_SHOW); 
		this->ShowPane(&m_wndStatusBar,TRUE,FALSE,TRUE);
	}
	//恢复窗口以前的大小 
	MoveWindow(&m_mainRect); 

	//恢复菜单栏 
	//this->SetMenu(&m_menuMainWnd); 
	this->ShowPane(&m_wndMenuBar,TRUE,FALSE,TRUE);
// 	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
// 	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
// 	EnableDocking(CBRS_ALIGN_ANY);
// 	DockPane(&m_wndMenuBar);

	//设置全屏显示标志 
	m_bFullScreenMode=false; 
}


void CMainFrame::FullScreenModeSwitch(void)
{
	if(this->m_bFullScreenMode) 
	{ 
		this->FullScreenModeOff();
		//m_wndMenuBar.SetDlgItemTextA(ID_OPERATE_FULLSCREEN,"全屏");
	} 
	else 
	{ 
		this->FullScreenModeOn(); 
		//m_wndMenuBar.SetDlgItemTextA(ID_OPERATE_FULLSCREEN,"取消全屏");
	} 
}
