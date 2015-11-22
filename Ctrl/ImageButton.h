#pragma once


typedef enum tagButtonState
{
	BS_Normal = 0, BS_Hot, BS_Disabled

} BUTTON_STATUS;

// CImageButton

class CImageButton : public CButton
{
	DECLARE_DYNAMIC(CImageButton)

public:
	CImageButton();
	virtual ~CImageButton();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	CFont			*m_pFont;			// �ı�����

	BOOL			m_bMouseOver;		// ��꾭�����
	BOOL			m_bMouseDown;		// ��갴�±��

	COLORREF		m_clrGround;		// ��ť��ɫ

	COLORREF		m_clrBkByNormal;	// ��������ɫ
	COLORREF		m_clrEdgeByNormal;	// �����߽�ɫ

	COLORREF		m_clrBkByOver;		// ��꾭������ɫ
	COLORREF		m_clrEdgeByOver;	// ��꾭���߽�ɫ

	COLORREF		m_clrBkByDown;		// ��갴�±���ɫ
	COLORREF		m_clrEdgeByDown;	// ��갴�±߽�ɫ

	HIMAGELIST		m_hImageList;
	UINT			m_nBitmapID;
	BOOL			m_bDisabled;

	int				m_nWidth;			// ��ť���
	int				m_nHeight;			// ��ť�߶�
	CToolTipCtrl	m_toolTip;			// ��ʾ����

public:
	void SetBackColorByNormal(COLORREF clrBk);
	void SetEdgeColorByNormal(COLORREF clrEdge);

	void SetBackColorByOver(COLORREF clrBk);
	void SetEdgeColorByOver(COLORREF clrEdge);

	void SetBackColorByDown(COLORREF clrBk);
	void SetEdgeColorByDown(COLORREF clrEdge);

	void SetGroundColor(COLORREF clrGround);

public:
	BOOL Create(const RECT& rect, CWnd * pParentWnd, UINT nID);

	void SetFont(CFont * pFont);
	BOOL SetImageList(BUTTON_STATUS state, UINT uBitmapID, const SIZE& buttonSize, DWORD numButtons, COLORREF mask);
	void SetBitmap(UINT nBitmapID);

	BOOL EnableWindow(BOOL bEnable = TRUE);
	BOOL IsButtonEnable(void);

	void SetButtonSize(CSize size);
	void SetButtonSize(int cx, int cy);
	
	void SetToolTipText(CString strTip);
};


