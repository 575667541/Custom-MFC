/********************************************************************
	Copyright 	(C) 2014 All Right Reserved
	Created:	2014/11/14 9:34
	FileName: 	FindFile.h
	Version:	1.0.0.0
	Author:		Jasen.Xu	
	Purpose:	�����ļ��в����ļ�
*********************************************************************/
#pragma once

// �����б�����������
typedef LPSTR TYPE;

class CFindFile
{
public:
	CFindFile(void);
	virtual ~CFindFile(void);
	

protected:
	CWinThread	*m_pFindThread;

	static UINT FindProc(LPVOID lpParam);
	void FindLoop(void);
public:
	BOOL IsRoot(LPCTSTR lpszPath);
	void FindAllFile(LPCTSTR lpszPath);

	void ClearFileList(void);

public:
	BOOL StartFind(LPCTSTR lpszPath);
	void StopFind(void);

	void SetFileExtension(LPCTSTR lpszExtension);
	void IncludeNoExtension(BOOL bNoExtension);
	
protected:
	CString			m_strMainPath;			// ��·��
	BOOL			m_bFind;				// �������
	CList<TYPE>		m_listFile;			// ����б�
	HANDLE			m_hComplete;			// ������ɱ��
	CList<LPCTSTR>	m_listExtension;		// Ҫ�������ļ���չ���б�
	BOOL			m_bNoExtension;			// ��������չ�����ļ�

public:	
	BOOL IsComplete(void);

	POSITION GetFirstPosition(void);
	POSITION GetLastPosition(void);
	TYPE& GetNextFile(POSITION& rPosition);
	TYPE& GetPrevFile(POSITION& rPosition);
};

