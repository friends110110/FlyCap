
// FlyCap.h : main header file for the FlyCap application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CFlyCapApp:
// See FlyCap.cpp for the implementation of this class
//

class CFlyCapApp : public CWinAppEx
{
public:
	CFlyCapApp();


// Overrides
public:
	virtual BOOL InitInstance();
//	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

//	virtual void PreLoadState();
//	virtual void LoadCustomState();
//	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CFlyCapApp theApp;
