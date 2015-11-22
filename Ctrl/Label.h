/********************************************************************
 * Copyright (C) 2014 All Right Reserved
 * \date 	 2014/11/21 16:18
 * \file 	 Label.h
 * \version  1.0.0.0
 * \author 	 JasenXu
 * \brief 	 �Զ����ǩ��ʵ��
*********************************************************************/
#pragma once
#include "afxwin.h"

class CLabel : public CWnd
{
public:
	CLabel();
	virtual ~CLabel();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
	CString			m_strLabel;		// ��ǩ����
	CFont			*m_pFont;		// �Զ�������
	COLORREF		m_clrText;		// �ı�ɫ
	COLORREF		m_clrBkgr;		// ����ɫ
	CFont			m_font;			// Ĭ������
	BOOL			m_bNoBorder;	// �ޱ߿��־
	COLORREF		m_clrBorder;	// �߿�ɫ
	CString			m_strPrefix;	// ǰ׺�ı�
	CToolTipCtrl	m_toolTip;		// ��ʾ����
	UINT			m_nFormat;		// ���뷽ʽ

public:
	void SetLabelText(CString strLabel);
	void SetLabelInt(int nLabel);

	void SetTextColor(COLORREF clrText);
	void SetBkColor(COLORREF clrBkgr);

	BOOL Create(const RECT& rect, CWnd * pParentWnd);

	void SetNoBorder(BOOL bNoBorder);
	void SetBorderColor(COLORREF clrBorder);
	void SetFont(CFont * pFont);
	void SetPrefixString(CString strPrefix);	
	void SetTextFormat(UINT nFormat);
};

