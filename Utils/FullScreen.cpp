/********************************************************************
	Copyright 	(C) 2014 All Right Reserved
	Created:	2014/11/11 17:09
	FileName: 	FullScreen.cpp
	Version:	1.0.0.0
	Author:		Jasen.Xu	
	Purpose:	
*********************************************************************/
#include "StdAfx.h"
#include "FullScreen.h"


CFullScreen::CFullScreen(CWnd * pParentWnd)
	: m_pParentWnd(pParentWnd)
	, m_bFullScreen(FALSE)
{
}


CFullScreen::~CFullScreen(void)
{
}


//************************************************************************
// Method:    EnterFullScreen
// Purpose:   ȫ����ʾ
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************************************************
void CFullScreen::EnterFullScreen(void)
{
	m_pParentWnd->GetWindowPlacement(&m_OldWndPlacement);

	CRect rcWindow;
	m_pParentWnd->GetWindowRect(&rcWindow);

	CRect rcClient;
	m_pParentWnd->RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, m_pParentWnd->reposQuery, &rcClient);
	m_pParentWnd->ClientToScreen(&rcClient);

	// ��ȡ��Ļ�ķֱ���
	int nFullWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int nFullHeight = ::GetSystemMetrics(SM_CYSCREEN);

	// ������������Ŀͻ���ȫ����ʾ�ӣ�0, 0)��(nFullWidth, nFullHeight)����
	// ��(0, 0)��(nFullWidth, nFullHeight)������������ԭ���ںͳ�������֮��Ŀͻ���λ�ü�Ĳ�ֵ, �͵õ�ȫ����ʾ�Ĵ���λ��
	m_rcFullScreen.left	= rcWindow.left - rcClient.left;
	m_rcFullScreen.top = rcWindow.top - rcClient.top;
	m_rcFullScreen.right = rcWindow.right - rcClient.right + nFullWidth;
	m_rcFullScreen.bottom = rcWindow.bottom - rcClient.bottom + nFullHeight;

	// ����ȫ����ʾ��־
	m_bFullScreen = TRUE;

	// ����ȫ����ʾ״̬
	WINDOWPLACEMENT wndpl = {0};
	wndpl.length = sizeof(WINDOWPLACEMENT);
	wndpl.flags = 0;
	wndpl.showCmd = SW_SHOWNORMAL;
	wndpl.rcNormalPosition = m_rcFullScreen;
	m_pParentWnd->SetWindowPlacement(&wndpl);
}


//************************************************************************
// Method:    EndFullScreen
// Purpose:	  �˳�ȫ����ʾ
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************************************************
void CFullScreen::EndFullScreen(void)
{
	if (m_bFullScreen)
	{
		m_pParentWnd->ShowWindow(SW_HIDE);
		m_pParentWnd->SetWindowPlacement(&m_OldWndPlacement);
		m_bFullScreen = FALSE;
	}
}


//************************************************************************
// Method:    IsFullScreen
// Purpose:	  �ж��Ƿ���ȫ��״̬, ȫ��ʱTRUE, ��ȫ��ʱFALSE
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: void
//************************************************************************
BOOL CFullScreen::IsFullScreen(void)
{
	return m_bFullScreen;
}
