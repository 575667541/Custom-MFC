/********************************************************************
 * Copyright (C) 2014 All Right Reserved
 * \date 	 2014/11/21 16:20
 * \file 	 Label.cpp
 * \version  1.0.0.0
 * \author 	 JasenXu
 * \brief 	 �Զ����ǩ��ʵ��
*********************************************************************/
#include "StdAfx.h"
#include "Label.h"


CLabel::CLabel()
	: m_strLabel(_T("ʾ���ı�"))
	, m_pFont(NULL)
	, m_bNoBorder(FALSE)
	, m_strPrefix(_T(""))
	, m_nFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE)
{
	m_clrText = GetSysColor(COLOR_WINDOWTEXT);
	m_clrBkgr = GetSysColor(COLOR_WINDOW);
	m_clrBorder = RGB(133, 145, 162);

	m_font.CreateFont(
		13,							// nHeight
		0,							// nWidth
		0,							// nEscapement
		0,							// nOrientation
		FW_NORMAL,					// nWeight
		FALSE,						 // bItalic
		FALSE,						 // bUnderline
		0,							 // cStrikeOut
		DEFAULT_CHARSET,			 // nCharSet
		OUT_DEFAULT_PRECIS,			 // nOutPrecision
		CLIP_DEFAULT_PRECIS,			// nClipPrecision
		DEFAULT_QUALITY,				// nQuality
		DEFAULT_PITCH | FF_SWISS,		// nPitchAndFamily
		_T("����"));					 // lpszFacename
}


CLabel::~CLabel()
{
}


BEGIN_MESSAGE_MAP(CLabel, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_CREATE()
END_MESSAGE_MAP()


void CLabel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CRect rc;
	GetClientRect(&rc);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	// ��������
	if (m_pFont)
	{
		memDC.SelectObject(m_pFont);
	}
	else
	{
		memDC.SelectObject(&m_font);
	}
	
	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	memDC.SelectObject(&memBmp);

	// ���Ƶ�ɫ
	memDC.FillSolidRect(&rc, m_clrBkgr);

	// �����ı�ɫ
	memDC.SetTextColor(m_clrText);

	CRect rcBorder;
	CRect rcText;
	if (m_strPrefix.IsEmpty() == FALSE)
	{
		CSize size = memDC.GetTextExtent(m_strPrefix);
		memDC.DrawText(m_strPrefix, &CRect(rc.left + 2, rc.top, rc.left + 2 + size.cx, rc.bottom),
			DT_LEFT | DT_VCENTER | DT_SINGLELINE);

		rcBorder = rc;
		rcBorder.left += 4;
		rcBorder.left = rcBorder.left + size.cx + 2;

		rcText = rcBorder;
	}
	else
	{
		rcBorder = rc;
		rcText = rc;
	}

	// ���Ʊ߿�
	if (m_bNoBorder == FALSE)
	{
		memDC.FrameRect(&rcBorder, &CBrush(m_clrBorder));
	}
	
	// �����ı�
	rcText.left += 4;
	rcText.right -= 2;
	if ((m_nFormat & DT_SINGLELINE )== 0)
	{
		rcText.top += 10;
	}
	memDC.DrawText(m_strLabel, &rcText, m_nFormat);

	dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);

	memBmp.DeleteObject();
	memDC.DeleteDC();
}


BOOL CLabel::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;

//	return CWnd::OnEraseBkgnd(pDC);
}


//************************************************************************
// Method:    SetLabelText
// Brief:	  ���ñ�ǩ�ı�����
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CString strLabel		�ı�����
//************************************************************************
void CLabel::SetLabelText(CString strLabel)
{
	m_strLabel = strLabel;

	// ������ʾ��Ϣ
	m_toolTip.UpdateTipText(m_strLabel, this);

	Invalidate(FALSE);
}


//************************************************************************
// Method:    SetLabelInt
// Brief:	  ���ñ�ǩ��ֵ����
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int nLabel			��ֵ����
//************************************************************************
void CLabel::SetLabelInt(int nLabel)
{
	m_strLabel.Format(_T("%d"), nLabel);

	// ������ʾ��Ϣ
	m_toolTip.UpdateTipText(m_strLabel, this);

	Invalidate(FALSE);
}


//************************************************************************
// Method:    SetTextColor
// Brief:	  �����ı�ɫ
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: COLORREF clrText
//************************************************************************
void CLabel::SetTextColor(COLORREF clrText)
{
	m_clrText = clrText;
	Invalidate(FALSE);
}


//************************************************************************
// Method:    SetBkColor
// Brief:	  ���ñ���ɫ
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: COLORREF clrBkgr
//************************************************************************
void CLabel::SetBkColor(COLORREF clrBkgr)
{
	m_clrBkgr = clrBkgr;
	Invalidate(FALSE);
}


//************************************************************************
// Method:    Create
// Brief:	  ������ǩ
// Access:    public 
// Returns:   BOOL			�ɹ�ΪTRUE������ΪFALSE
// Qualifier:
// Parameter: const RECT & rect		��ǩ�ߴ�
// Parameter: CWnd * pParentWnd		������
//************************************************************************
BOOL CLabel::Create(const RECT& rect, CWnd * pParentWnd)
{
	if (rect.right-rect.left <= 0 || rect.bottom-rect.top <= 0)
	{
		return CWnd::Create(NULL, NULL, WS_CHILD | WS_VISIBLE,
			CRect(0, 0, 200, 22), pParentWnd, NULL);
	}

	return CWnd::Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, pParentWnd, NULL);
}


//************************************************************************
// Method:    SetNoBorder
// Brief:	  �������ޱ߿�
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: BOOL bNoBorder		TRUE��ʾ�ޱ߿�FALSE��ʾ�б߿�
//************************************************************************
void CLabel::SetNoBorder(BOOL bNoBorder)
{
	m_bNoBorder = bNoBorder;
	Invalidate(FALSE);
}


//************************************************************************
// Method:    SetBorderColor
// Brief:	  ���ñ߿���ɫ
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: COLORREF clrBorder
//************************************************************************
void CLabel::SetBorderColor(COLORREF clrBorder)
{
	m_clrBorder = clrBorder;
	Invalidate(FALSE);
}


//************************************************************************
// Method:    SetFont
// Brief:	  �����Զ�������
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CFont * pFont
//************************************************************************
void CLabel::SetFont(CFont * pFont)
{
	m_pFont = pFont;
}


//************************************************************************
// Method:    SetPrefixString
// Brief:	  ����ǰ׺��Ϣ
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CString strPrefix
//************************************************************************
void CLabel::SetPrefixString(CString strPrefix)
{
	m_strPrefix = strPrefix;
}


BOOL CLabel::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE)
	{
		m_toolTip.RelayEvent(pMsg);
	}

	return CWnd::PreTranslateMessage(pMsg);
}


//************************************************************************
// Method:    OnCreate
// Brief:	  �ڴ��ڴ��������ToolTip
// Access:    protected 
// Returns:   int
// Qualifier:
// Parameter: LPCREATESTRUCT lpCreateStruct
//************************************************************************
int CLabel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// ���ù�����ʾ�ؼ�
	EnableToolTips(TRUE);

	m_toolTip.Create(this);
	
	// ע�Ṥ����ʾ�ؼ�
	m_toolTip.AddTool(this);

	// �������ʾ�ؼ�
	m_toolTip.Activate(TRUE);

	return 0;
}


//************************************************************************
// Method:    SetTextFormat
// Brief:	  �����ı����뷽ʽ
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: UINT nFormat
//************************************************************************
void CLabel::SetTextFormat(UINT nFormat)
{
	m_nFormat = nFormat;
}
