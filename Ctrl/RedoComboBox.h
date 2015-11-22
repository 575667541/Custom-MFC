#pragma once
#include "afxwin.h"


//  =======================================================================
//	����		CRedoComboBox
//	����		���ڻ����Զ����ComboBox�ؼ�
//  =======================================================================

class CRedoComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CRedoComboBox)

public:
	CRedoComboBox();
	virtual ~CRedoComboBox();

protected:
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	DECLARE_MESSAGE_MAP()

public:
	//  =======================================================================
	//  ����ǰ��ɫ
	//  =======================================================================
	void SetForeColor(COLORREF clrFore);

	//  =======================================================================
	//  ���ñ���ɫ
	//  =======================================================================
	void SetBkColor(COLORREF clrBack);

	//  =======================================================================
	//  ��ȡǰ��ɫ
	//  =======================================================================
	COLORREF GetForeColor(void);

	//  =======================================================================
	//  ��ȡ����ɫ
	//  =======================================================================
	COLORREF GetBkColor(void);

	//  =======================================================================
	//  ����ѡ��ǰ��ɫ
	//  =======================================================================
	void SetSelForeColor(COLORREF clrFore);

	//  =======================================================================
	//  ����ѡ�񱳾�ɫ
	//  =======================================================================
	void SetSelBkColor(COLORREF clrBack);

private:
	COLORREF m_clrFore;		// ǰ��ɫ
	COLORREF m_clrBack;		// ����ɫ

	COLORREF m_clrSelFore;	// ѡ��ǰ��ɫ
	COLORREF m_clrSelBack;	// ѡ�񱳾�ɫ

	CFont	*m_pFont;

public:
	void SetFont(CFont * pFont);

};


