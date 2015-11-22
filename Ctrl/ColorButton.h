#pragma once


// CColorButton

class CColorButton : public CButton
{
	DECLARE_DYNAMIC(CColorButton)

public:
	CColorButton();
	virtual ~CColorButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();

private:
	COLORREF	m_clrBack;		// ����ɫ
	COLORREF	m_clrFore;		// ǰ��ɫ
	COLORREF	m_clrDisabled;
	BOOL		m_bMouseOver;
	BOOL		m_bCheck;
	int			m_nGradient;
	CFont		*m_pFont;

public:
	//  =======================================================================
	//	���ñ���ɫ
	//  =======================================================================
	void SetBkColor(COLORREF clrBack);
	
	//  =======================================================================
	//	��ȡ����ɫ
	//  =======================================================================
	COLORREF GetBkColor(void);
	
	//  =======================================================================
	//	����ǰ��ɫ
	//  =======================================================================
	void SetFeColor(COLORREF clrFore);
	
	//  =======================================================================
	//	��ȡǰ��ɫ
	//  =======================================================================
	COLORREF GetFeColor(void);

	//  =======================================================================
	//	���ð���״̬
	//  =======================================================================
	void SetCheck(BOOL bCheck);
	
private:
	//  =======================================================================
	// ���ư�ť����״̬
	//  =======================================================================
	void DrawButtonDown(CDC* pDC, CRect rc, CString strCaption);

	//  =======================================================================
	//	������꾭��״̬
	//  =======================================================================
	void DrawMouseOverStatus(CDC* pDC, CRect rc, CString strCaption);

	//  =======================================================================
	//	���ư�ťʧЧ״̬
	//  =======================================================================
	void DrawDisabledStatus(CDC* pDC, CRect rc, CString strCaption);

	//  =======================================================================
	//	���ư�ť���屳��
	//  =======================================================================
	void DrawSolidBackgroud(CDC* pDC, CRect rc, int r, int g, int b);
public:
	void SetFont(CFont * pFont);
};


