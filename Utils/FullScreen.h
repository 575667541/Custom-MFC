/********************************************************************
	Copyright 	(C) 2014 All Right Reserved
	Created:	2014/11/11 16:55
	FileName: 	FullScreen.h
	Version:	1.0.0.0
	Author:		Jasen.Xu	
	Purpose:	�Ի���Ӧ�ó���ȫ����ʾ
*********************************************************************/
#pragma once
#include "atltypes.h"


class CFullScreen
{
public:
	CFullScreen(CWnd * pParentWnd);
	virtual ~CFullScreen(void);

private:
	CWnd			*m_pParentWnd;

protected:
	WINDOWPLACEMENT m_OldWndPlacement;		// ���ڱ���ԭ����λ��
	BOOL			m_bFullScreen;			// ȫ����ʾ��־
	CRect			m_rcFullScreen;			// ��ʾȫ����ʾʱ�Ĵ���λ��

public:
	void EnterFullScreen(void); 
	void EndFullScreen(void);

	BOOL IsFullScreen(void);
};

