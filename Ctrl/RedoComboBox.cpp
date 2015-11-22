// RedoComboBox.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RedoComboBox.h"


// CRedoComboBox

IMPLEMENT_DYNAMIC(CRedoComboBox, CComboBox)

CRedoComboBox::CRedoComboBox()
	: m_clrFore(RGB(0, 0, 0))
	, m_clrBack(RGB(255, 255, 255))
	, m_clrSelFore(RGB(255, 255, 255))
	, m_clrSelBack(RGB(51, 153, 255))
{

}

CRedoComboBox::~CRedoComboBox()
{
}


BEGIN_MESSAGE_MAP(CRedoComboBox, CComboBox)

END_MESSAGE_MAP()



// CRedoComboBox ��Ϣ�������




void CRedoComboBox::PreSubclassWindow()
{

	CComboBox::PreSubclassWindow();
}

//  ...........................................................................
//  �ػ������б�
//  ...........................................................................
void CRedoComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	RECT rcItem(lpDrawItemStruct->rcItem);
	UINT nState(lpDrawItemStruct->itemState);

	// �ػ汳��
	dc.SetBkMode(TRANSPARENT);
	dc.FillSolidRect(&rcItem, m_clrBack);

	// ��������
//	CFont font;
//	font.CreatePointFont(90, _T("΢���ź�"), &dc);
	dc.SelectObject(m_pFont);

	// ��ʾ����
	CString strLb(_T(""));
	GetLBText(lpDrawItemStruct->itemID, strLb);
	dc.SetTextColor(m_clrFore);
	dc.DrawText(strLb, &rcItem, DT_SINGLELINE | DT_LEFT);

	// ѡ��״̬
	if (nState & ODS_SELECTED)
	{
		dc.FillSolidRect(&rcItem, m_clrSelBack);
		GetLBText(GetCurSel(), strLb);
		dc.SetTextColor(m_clrSelFore);
		dc.DrawText(strLb, &rcItem, DT_SINGLELINE | DT_LEFT);
	}

	dc.Detach();
}

//  ...........................................................................
//  �ı��ȸ߶ȣ������໯��Ч��
//  ...........................................................................
void CRedoComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if ((LONG)lpMeasureItemStruct->itemID == -1)
	{
		// �ı���
		lpMeasureItemStruct->itemHeight = 15;
	}
	else
	{
		// �б�
		lpMeasureItemStruct->itemHeight = 40;
	}
}

//  ...........................................................................
//  ����ǰ��ɫ
//  ...........................................................................
void CRedoComboBox::SetForeColor(COLORREF clrFore)
{
	m_clrFore = clrFore;
}

//  ...........................................................................
//  ���ñ���ɫ
//  ...........................................................................
void CRedoComboBox::SetBkColor(COLORREF clrBack)
{
	m_clrBack = clrBack;
}

//  ...........................................................................
//  ��ȡǰ��ɫ
//  ...........................................................................
COLORREF CRedoComboBox::GetForeColor(void)
{
	return COLORREF(m_clrFore);
}

//  ...........................................................................
//  ��ȡ����ɫ
//  ...........................................................................
COLORREF CRedoComboBox::GetBkColor(void)
{
	return COLORREF(m_clrBack);
}

//  ...........................................................................
//  ����ѡ��ǰ��ɫ
//  ...........................................................................
void CRedoComboBox::SetSelForeColor(COLORREF clrFore)
{
	m_clrSelFore = clrFore;
}

//  ...........................................................................
//  ����ѡ�񱳾�ɫ
//  ...........................................................................
void CRedoComboBox::SetSelBkColor(COLORREF clrBack)
{
	m_clrSelBack = clrBack;
}

void CRedoComboBox::SetFont(CFont * pFont)
{
	m_pFont = pFont;
}
