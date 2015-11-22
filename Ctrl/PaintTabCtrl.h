/********************************************************************
	Copyright 	(C) 2014 All Right Reserved
	Created:	2014/11/12 18:26
	FileName: 	PaintTabCtrl.h
	Version:	1.0.0.0
	Author:		Jasen.Xu	
	Purpose:	�Զ���TAB�ؼ���ʵ��
*********************************************************************/
#pragma once
#include "afxwin.h"
#include "afxtempl.h"


// ��ǩҳ�л���Ϣ
#ifndef WM_SELCHANGE
#define WM_SELCHANGE	(WM_USER+1)
#endif


/************************************************************************/
/* ��ǩ������                                                             */
/************************************************************************/
class CPaintTabItem : public CButton
{
public:
	// ��ǩ״̬
	enum StatusFlags
	{
		statusNormal,		// ��̬
		statusMove,			// ��꾭��
		statusSelect,		// ѡ��
	};

	CPaintTabItem(void)
		: m_nItemWidth(78)
		, m_nItemHeight(30)
		, m_nStatusFlag(0)
		, m_pFont(NULL)
		, m_hIcon(NULL)
		, m_nIndex(0)
	{
	}

	~CPaintTabItem(void){}

protected:
	COLORREF		m_clrBk;			// ��ǩ��ɫ
	COLORREF		m_clrTxt;			// ��ǩ�ı�ɫ
	LPCTSTR			m_lpszTabName;		// ��ǩ��
	int				m_nItemWidth;		// ��ǩ���
	int				m_nItemHeight;		// ��ǩ�߶�
	UINT			m_nStatusFlag;		// ��ǩ״̬
	CFont			*m_pFont;			// ��ǩ����
	HICON			m_hIcon;			// ͼ����
	int				m_nIndex;			// ��ǩ��TAB�ؼ��е����
	CToolTipCtrl	m_toolTip;			// ��ʾ��Ϣ		

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	//************************************************************************
	// Method:    SetFont
	// Purpose:   ���ñ�ǩ����
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: CFont * pFont
	//************************************************************************
	void SetFont(CFont * pFont)
	{
		m_pFont = pFont;
	}

	//************************************************************************
	// Method:    Create
	// Purpose:   ������ǩ
	// Access:    public 
	// Returns:   BOOL	�ɹ�ΪTRUE��ʧ��ΪFALSE
	// Qualifier:
	// Parameter: const RECT & rect		��ǩ�ߴ�
	// Parameter: LPCTSTR lpszTabName	��ǩ��
	// Parameter: CWnd * pParentWnd		������
	// Parameter: HICON hIcon			ͼ����
	// Parameter: CFont * pFont			��ǩ����
	//************************************************************************
	BOOL Create(int nIndex, const SIZE& size, LPCTSTR lpszTabName, CWnd * pParentWnd, HICON hIcon = NULL, CFont * pFont = NULL)
	{
		m_lpszTabName = lpszTabName;
		m_hIcon = hIcon;
		m_pFont = pFont;
		m_nItemWidth = size.cx;
		m_nItemHeight = size.cy;
		m_nIndex = nIndex;

		return CButton::Create(NULL, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, CRect(0, 0, size.cx, size.cy), pParentWnd, NULL);
	}

	//************************************************************************
	// Method:    SetSize
	// Purpose:   ���ñ�ǩ�ߴ�
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: int nWidth	��ǩ���		-1��ʾ����ԭ��ֵ
	// Parameter: int nHeight	��ǩ�߶�		-1��ʾ����ԭ��ֵ
	//************************************************************************
	void SetSize(int nWidth, int nHeight)
	{
		if (nWidth > 0)
		{
			m_nItemWidth = nWidth;
		}
		
		if (nHeight > 0)
		{
			m_nItemHeight = nHeight;
		}	
	}

	//************************************************************************
	// Method:    SetTabStatus
	// Purpose:   ���ñ�ǩ״̬
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: UINT nStatusFlag
	//************************************************************************
	void SetTabStatus(UINT nStatusFlag)
	{
		m_nStatusFlag = nStatusFlag;

		// ˢ����ʾ��ǩ
		Invalidate(FALSE);
	}
	

	//************************************************************************
	// Method:    SetToolTipText
	// Brief:	  ���¹�����ʾ��Ϣ
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: CString strTip	�µ���ʾ��Ϣ
	//************************************************************************
	void SetToolTipText(CString strTip)
	{
		if (m_toolTip.GetSafeHwnd() == NULL)
		{
			// ���ù�����ʾ�ؼ�
			EnableToolTips(TRUE);

			m_toolTip.Create(this);

			// ע�Ṥ����ʾ�ؼ�
			m_toolTip.AddTool(this);

			// �������ʾ�ؼ�
			m_toolTip.Activate(TRUE);
		}
		
		m_toolTip.UpdateTipText(strTip, this);
	}
};


/************************************************************************/
/* �Զ���ؼ�ʵ����									                    */
/************************************************************************/
class CPaintTabCtrl : public CWnd
{
private:
	// ��ǩ����
	struct TabMgr
	{
		CPaintTabItem	tab;		// ��ǩ
		CDialogEx		*pDlg;		// ��ǩҳ�Ի���
	};

public:
	// StyleFlags
	enum TabStyleFlags
	{
		styleLeft		= 0x00000001,		// ��ǩ��������
		styleCenter		= 0x00000002,		// ��ǩ��������
		styleHorizontal	= 0x00000004,		// ��ǩ��������
		styleVertical	= 0x00000008,		// ��ǩ��������
		styleSplit		= 0x00000010,		// ��ǩ��������
	};

	CPaintTabCtrl(void);
	virtual ~CPaintTabCtrl(void);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnSelchange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNcHitTest(CPoint point);

protected:
	int					m_nCurSel;				// ��ǰ��ʾ�ı�ǩҳ���
	CArray<TabMgr *>	m_arTabMgr;				// ��ǩҳ������
	CFont				m_font;					// �ؼ�Ĭ������
	CFont				*m_pFont;				// �ؼ��Զ�������
	int					m_nHighLineHeight;		// ��ǩ���ײ������ߵĸ߶�
	int					m_nTabItemWidth;		// ��ǩ���
	int					m_nTabItemHeight;		// ��ǩ�߶�
	UINT				m_nTabStyle;			// ��ǩ����ʽ

public:
	BOOL Create(const RECT& rect, CWnd * pParent, UINT nID = NULL);	

	LONG AddItem(CDialogEx * pDlg, UINT nIDDialog, const SIZE& size, LPCTSTR lpszTabName, UINT nIDIcon = NULL, CString strToolTip = NULL);

	void SetItemSize(int nWidth, int nHeight);
	void SetFont(CFont * pFont);

	void GetClientRect(LPRECT lpRect) const;
	void SetTabStyle(UINT nStyleFlags);
	UINT GetTabStyle(void) const;
};

