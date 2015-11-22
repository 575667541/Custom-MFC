#include "StdAfx.h"
#include "PaintTabCtrl.h"

/************************************************************************/
/* CPaintTabItem ��Ϣ��Ӧ����                                             */
/************************************************************************/
BEGIN_MESSAGE_MAP(CPaintTabItem, CButton)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



void CPaintTabItem::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// ��ȡ��ǩ�ߴ�
	CRect rc;
	GetWindowRect(&rc);
	ScreenToClient(&rc);

	// ���ݲ�ͬ״̬�����Ʋ�ͬ��ɫ�ı�ǩ
	switch (m_nStatusFlag)
	{
#ifdef DARK_BACKGR
	case statusMove:
		m_clrTxt = RGB(255, 255, 255);
		m_clrBk = RGB(28, 151, 234);
		break;
	case statusSelect:
		m_clrTxt = RGB(255, 255, 255);
		m_clrBk = RGB(0, 122, 204);
		break;
	case statusNormal:
	default:
		m_clrTxt = RGB(255, 255, 255);
		m_clrBk = RGB(45, 45, 48);
		break;
#else
	case statusMove:
		m_clrTxt = RGB(240, 240, 240);
		m_clrBk = RGB(91, 113, 153);
		break;
	case statusSelect:
		m_clrTxt = RGB(0, 0, 0);
		m_clrBk = RGB(255, 242, 157);
		break;
	case statusNormal:
	default:
		m_clrTxt = RGB(255, 255, 255);
		m_clrBk = RGB(54, 78, 111);
		break;
#endif
	}

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	if (m_pFont)
	{
		memDC.SelectObject(m_pFont);
	}

	// ��Ϳ��ǩ��ɫ
	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap *pOldBmp = memDC.SelectObject(&memBmp);
	memDC.FillSolidRect(&rc, m_clrBk);

	// ��ʾ��ǩ��
	memDC.SetTextColor(m_clrTxt);
	if (m_hIcon)
	{
		ICONINFO info = {0};
		GetIconInfo(m_hIcon, &info);

		memDC.DrawIcon(6, m_nItemHeight/2-info.yHotspot, m_hIcon);
		rc.left = rc.left + 2*info.xHotspot + 6 + 7;

		memDC.DrawText(m_lpszTabName, &rc, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	}
	else
	{
		memDC.DrawText(m_lpszTabName, &rc, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	}

	dc.BitBlt(0, 0, m_nItemWidth, m_nItemHeight, &memDC, 0, 0, SRCCOPY);

	// �ͷ���Դ
	memBmp.DeleteObject();
	memDC.DeleteDC();
}


BOOL CPaintTabItem::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;

//	return CButton::OnEraseBkgnd(pDC);
}


//************************************************************************
// Method:    OnMouseMove
// Purpose:   ���ı�ǩ״̬Ϊ��꾭��
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: UINT nFlags
// Parameter: CPoint point
//************************************************************************
void CPaintTabItem::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_nStatusFlag != statusSelect)
	{
		m_nStatusFlag = statusMove;
	}
	
	CButton::OnMouseMove(nFlags, point);
}


//************************************************************************
// Method:    OnMouseLeave
// Purpose:   �ָ���ǩ״̬Ϊ��̬
// Access:    protected 
// Returns:   void
// Qualifier:
//************************************************************************
void CPaintTabItem::OnMouseLeave()
{
	if (m_nStatusFlag != statusSelect)
	{
		m_nStatusFlag = statusNormal;
	}
	
	CButton::OnMouseLeave();
}


//************************************************************************
// Method:    OnLButtonDown
// Purpose:   ���ı�ǩ״̬Ϊѡ��
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: UINT nFlags
// Parameter: CPoint point
//************************************************************************
void CPaintTabItem::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_nStatusFlag = statusSelect;

	// ֪ͨCPaintTabCtrl�������л���ǩҳ
	::PostMessage(GetParent()->GetSafeHwnd(), WM_SELCHANGE, (WPARAM)m_nIndex, NULL);

	CButton::OnLButtonDown(nFlags, point);
}


BOOL CPaintTabItem::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE)
	{
		m_toolTip.RelayEvent(pMsg);
	}

	return CButton::PreTranslateMessage(pMsg);
}


/************************************************************************/
/* CPaintTabCtrl ��Ϣ��Ӧ����                                             */
/************************************************************************/
CPaintTabCtrl::CPaintTabCtrl(void)
	: m_nCurSel(0)
	, m_pFont(NULL)
	, m_nHighLineHeight(2)
	, m_nTabItemWidth(78)
	, m_nTabItemHeight(30)
	, m_nTabStyle(0x00000001)
{
	m_font.CreateFont(
		20,							// nHeight
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
		_T("Arial"));					 // lpszFacename
}


CPaintTabCtrl::~CPaintTabCtrl(void)
{
	while (m_arTabMgr.GetSize())
	{
		TabMgr *p = m_arTabMgr.GetAt(0);
		if (p)
		{
			delete p;
			p = NULL;
		}
		m_arTabMgr.RemoveAt(0);
	}
}


BEGIN_MESSAGE_MAP(CPaintTabCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_MESSAGE(WM_SELCHANGE, &CPaintTabCtrl::OnSelchange)
	ON_WM_ERASEBKGND()
//	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


void CPaintTabCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc;
	CWnd::GetClientRect(&rc);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap *pOldBmp = memDC.SelectObject(&memBmp);

	// ���ƿؼ���ǩ����ɫ
#ifdef DARK_BACKGR
	memDC.FillSolidRect(0, 0, rc.Width(), m_nTabItemHeight, RGB(45, 45, 48));
#else
	memDC.FillSolidRect(0, 0, rc.Width(), m_nTabItemHeight, RGB(41, 57, 86));
#endif

	// ���Ƹ�����
#ifdef DARK_BACKGR
	memDC.FillSolidRect(0, m_nTabItemHeight, rc.Width(), m_nHighLineHeight, RGB(0, 122, 204));
//	memDC.FillSolidRect(0, m_nTabItemHeight + m_nHighLineHeight, rc.Width(),
//		1, RGB(0, 122, 204));
#else
	memDC.FillSolidRect(0, m_nTabItemHeight, rc.Width(), m_nHighLineHeight, RGB(255, 242, 157));
	memDC.FillSolidRect(0, m_nTabItemHeight + m_nHighLineHeight, rc.Width(),
		1, RGB(133, 145, 162));
#endif

	// ���ƿؼ���ɫ
#ifdef DARK_BACKGR
	memDC.FillSolidRect(0, m_nTabItemHeight+m_nHighLineHeight+1, rc.Width(), 
		rc.Height()-(m_nTabItemHeight+m_nHighLineHeight+1), RGB(45, 45, 48));
#else
	memDC.FillSolidRect(0, m_nTabItemHeight + m_nHighLineHeight+1, rc.Width(),
		rc.Height() - (m_nTabItemHeight + m_nHighLineHeight+1), RGB(255, 255, 255));
#endif

	dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBmp);
	memBmp.DeleteObject();
	memDC.DeleteDC();
}


BOOL CPaintTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;

//	return CWnd::OnEraseBkgnd(pDC);
}


void CPaintTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (m_arTabMgr.GetSize() <= 0)
	{
		return;
	}

	// �������б�ǩ��λ��
	for (int i(0); i < m_arTabMgr.GetSize(); ++i)
	{
		TabMgr *p = m_arTabMgr.GetAt(i);
		if (i == m_arTabMgr.GetSize()-1 && p->pDlg == NULL)
		{
			p->tab.MoveWindow(max(i*(m_nTabItemWidth), cx-m_nTabItemWidth), 0, m_nTabItemWidth, m_nTabItemHeight);
		}
		else
		{
			if (m_nTabStyle&styleLeft)
			{
				p->tab.MoveWindow(i*(m_nTabItemWidth), 0, m_nTabItemWidth, m_nTabItemHeight);
			}
			else if (m_nTabStyle&styleCenter && m_nTabStyle&styleSplit)
			{
				int nSplit = (cx-2*m_nTabItemWidth-m_arTabMgr.GetSize()*m_nTabItemWidth) / m_arTabMgr.GetSize();
				p->tab.MoveWindow(m_nTabItemWidth+i*(m_nTabItemWidth+nSplit), 0, m_nTabItemWidth, m_nTabItemHeight);
			}
			else if (m_nTabStyle&styleCenter)
			{
				int nIndent = (cx - m_arTabMgr.GetSize()*m_nTabItemWidth) / 2;
				p->tab.MoveWindow(nIndent + i*m_nTabItemWidth, 0, m_nTabItemWidth, m_nTabItemHeight);
			}
			p->pDlg->MoveWindow(0, m_nTabItemHeight + m_nHighLineHeight + 1, cx, cy - (m_nTabItemHeight + m_nHighLineHeight + 1));
		}	
	}

	// ������ǰ��ǩҳ��λ��
/*	TabMgr *p = m_arTabMgr.GetAt(m_nCurSel);
	if (p->pDlg)
	{
		p->pDlg->MoveWindow(0, m_nTabItemHeight+m_nHighLineHeight+1, cx, cy-(m_nTabItemHeight+m_nHighLineHeight+1));
	}	
*/
}


//************************************************************************
// Method:    Create
// Purpose:   ����TAB�ؼ�
// Access:    public 
// Returns:   BOOL		�ɹ�����TRUE��ʧ�ܷ���FLASE
// Qualifier:
// Parameter: const RECT & rect		�ؼ��ߴ�
// Parameter: CWnd * pParent		������
// Parameter: UINT nID				�ؼ���ԴID
//************************************************************************
BOOL CPaintTabCtrl::Create(const RECT& rect, CWnd * pParent, UINT nID)
{
	return CWnd::Create(NULL, NULL, WS_CHILD|WS_VISIBLE, rect, pParent, nID);
}


//************************************************************************
// Method:    AddItem
// Purpose:   ����µı�ǩ
// Access:    public 
// Returns:   LONG		�±�ǩ����ţ�ʧ�ܷ���-1
// Qualifier:
// Parameter: CDialogEx * pDlg		��ǩҳ�Ի���
// Parameter: UINT nIDDialog		�Ի�����ԴID
// Parameter: const RECT & rect		��ǩ�ߴ�
// Parameter: LPCTSTR lpszTabName	��ǩ��
// Parameter: HICON hIcon			ͼ����
//************************************************************************
LONG CPaintTabCtrl::AddItem(CDialogEx * pDlg, UINT nIDDialog, const SIZE& size, LPCTSTR lpszTabName, UINT nIDIcon, CString strToolTip)
{
	// �������ĳߴ�
	if (m_nTabItemWidth < size.cx)
	{
		m_nTabItemWidth = size.cx;
		SetItemSize(m_nTabItemWidth, -1);
	}
	
	if (m_nTabItemHeight < size.cy)
	{
		m_nTabItemHeight = size.cy;
		SetItemSize(-1, m_nTabItemHeight);
	}

	// ��ȡͼ����
	HICON hIcon(NULL);
	if (nIDIcon != NULL)
	{
		hIcon = AfxGetApp()->LoadIconW(nIDIcon);
		if (hIcon == NULL)
		{
			// �����ȡʧ�ܣ��򷵻�-1
			return -1;
		}

		// ����ͼ���С������ǩ���߶�
		ICONINFO info = {0};
		GetIconInfo(hIcon, &info);

		if (2*(info.yHotspot+4) > (DWORD)m_nTabItemHeight)
		{
			m_nTabItemHeight = 2*(info.yHotspot+4);
		}
	}

	// ������ǩ
	BOOL bPass(FALSE);
	TabMgr *p = new TabMgr;
	if (m_pFont)
	{
		bPass = p->tab.Create(m_arTabMgr.GetSize(), CSize(m_nTabItemWidth, m_nTabItemHeight), lpszTabName, this, hIcon, m_pFont);
	} 
	else
	{
		bPass = p->tab.Create(m_arTabMgr.GetSize(), CSize(m_nTabItemWidth, m_nTabItemHeight), lpszTabName, this, hIcon, &m_font);
	}
	
	if (bPass == FALSE)
	{
		return -1;
	}

	p->tab.SetToolTipText(strToolTip);

	// ������ǩҳ
	p->pDlg = pDlg;
	if (pDlg)
	{
		bPass = pDlg->Create(nIDDialog, this);
	}
	

	if (bPass == FALSE)
	{
		return -1;
	}

	// ��ӵ���ǩҳ������
	m_arTabMgr.Add(p);
	if (m_arTabMgr.GetSize() == 1)
	{
		// ����ǵ�һ����ǩ������ʾ��
		p->tab.SetTabStatus(CPaintTabItem::statusSelect);
		if (pDlg)
		{
			pDlg->ShowWindow(SW_SHOW);
		}		
		m_nCurSel = 0;	
	}
	else
	{
		if (pDlg)
		{
			pDlg->ShowWindow(SW_HIDE);
		}	
	}
	
	return LONG(m_arTabMgr.GetSize()-1);
}


//************************************************************************
// Method:    SetItemSize
// Purpose:   ���ñ�ǩ�ߴ�
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int nWidth		��ǩ���		-1��ʾ����ԭ��ֵ
// Parameter: int nHeight		��ǩ�߶�		-1��ʾ����ԭ��ֵ
//************************************************************************
void CPaintTabCtrl::SetItemSize(int nWidth, int nHeight)
{
	for (int i(0); i < m_arTabMgr.GetSize(); ++i)
	{
		TabMgr *p = m_arTabMgr.GetAt(i);
		p->tab.SetSize(nWidth, nHeight);
	}
}


//************************************************************************
// Method:    SetFont
// Purpose:   ���ñ�ǩ����
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CFont * pFont
//************************************************************************
void CPaintTabCtrl::SetFont(CFont * pFont)
{
	m_pFont = pFont;

	if (pFont)
	{
		for (int i(0); i < m_arTabMgr.GetSize(); ++i)
		{
			TabMgr *p = m_arTabMgr.GetAt(i);
			p->tab.SetFont(pFont);
		}
	}
}


//************************************************************************
// Method:    OnSelchange
// Purpose:   �л���ǩҳ
// Access:    protected 
// Returns:   LRESULT
// Qualifier:
// Parameter: WPARAM wParam		Ҫ��ʾ�ı�ǩҳ���
// Parameter: LPARAM lParam
//************************************************************************
afx_msg LRESULT CPaintTabCtrl::OnSelchange(WPARAM wParam, LPARAM lParam)
{
	// ������ֱ�ǩ��Ŵ��ڹ������Ա������������˳�����
	if ((int)wParam >= m_arTabMgr.GetSize())
	{
		::PostQuitMessage(0);
		return 0;
	}

	// ����ظ������ǰ��ʾ�ı�ǩҳ����ֱ�ӷ���
	if (m_nCurSel == (int)wParam)
	{
		return 0;
	}

	// ������ǰ��ʾ�ı�ǩҳ
	TabMgr *p = m_arTabMgr.GetAt(m_nCurSel);
	if (p->pDlg)
	{
		p->pDlg->ShowWindow(SW_HIDE);
	}	

	// ����֮ǰ�ı�ǩ״̬Ϊ��̬
	p->tab.SetTabStatus(CPaintTabItem::statusNormal);

	// ��ȡ��ǰ��ʾ�ı�ǩҳ���
	m_nCurSel = (int)wParam;

	// ��ȡ��ǰ��ǩҳ�ĳߴ�
	p = m_arTabMgr.GetAt(m_nCurSel);
	CRect rcShow;
	if (p->pDlg == NULL)
	{
		::PostQuitMessage(0);
		return 0;
		
	}

	p->pDlg->GetWindowRect(&rcShow);
	ScreenToClient(&rcShow);

	// ��ȡTAB�ؼ��ͻ����ߴ�
	CRect rcTabCtrl;
	GetClientRect(&rcTabCtrl);

	// ���Ҫ��ʾ�ı�ǩҳδ�����ؼ��ͻ������������ǩҳ�ߴ�
	if (rcShow != rcTabCtrl)
	{
		p->pDlg->MoveWindow(&rcTabCtrl);
	}

	// ��ʾ��ǰ��ǩҳ
	p->pDlg->ShowWindow(SW_SHOW);
	return 0;
}


//************************************************************************
// Method:    GetClientRect
// Purpose:   ��ȡTAB�ؼ��ͻ����ĳߴ�
// Access:    public 
// Returns:   void
// Qualifier: const
// Parameter: LPRECT lpRect
//************************************************************************
void CPaintTabCtrl::GetClientRect(LPRECT lpRect) const
{
	CRect rc;
	CWnd::GetClientRect(&rc);

	lpRect->left = rc.left;
	lpRect->top = rc.top + m_nTabItemHeight + m_nHighLineHeight + 1;
	lpRect->right = lpRect->left + rc.Width();
	lpRect->bottom = rc.bottom;	
}


//************************************************************************
// Method:    SetTabStyle
// Purpose:   ���ñ�ǩ����ʽ
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: UINT nStyleFlags
//************************************************************************
void CPaintTabCtrl::SetTabStyle(UINT nStyleFlags)
{
	m_nTabStyle = nStyleFlags;
}


//************************************************************************
// Method:    GetTabStyle
// Purpose:   ��ȡ��ǩ����ʽ
// Access:    public 
// Returns:   UINT
// Qualifier: const
// Parameter: void
//************************************************************************
UINT CPaintTabCtrl::GetTabStyle(void) const
{
	return m_nTabStyle;
}


//************************************************************************
// Method:    OnNcHitTest
// Purpose:   �϶��ؼ�
// Access:    protected 
// Returns:   LRESULT
// Qualifier:
// Parameter: CPoint point
//************************************************************************
LRESULT CPaintTabCtrl::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(&rc);
	CRect rcBar(CPoint(0, 0), CSize(rc.Width(), m_nTabItemHeight+m_nHighLineHeight));

	if (rcBar.PtInRect(point))
	{
		//return HTCAPTION;
		//::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_NCHITTEST, NULL, NULL);
	}

	return CWnd::OnNcHitTest(point);
}


