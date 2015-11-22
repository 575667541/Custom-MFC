/********************************************************************
	Copyright 	(C) 2014 All Right Reserved
	Created:	2014/11/14 9:34
	FileName: 	FindFile.cpp
	Version:	1.0.0.0
	Author:		Jasen.Xu	
	Purpose:	�����ļ��в����ļ�
*********************************************************************/
#include "StdAfx.h"
#include "FindFile.h"


CFindFile::CFindFile(void)
	: m_pFindThread(NULL)
	, m_bFind(FALSE)
	, m_strMainPath(_T(""))
	, m_hComplete(CreateEvent(NULL, TRUE, FALSE, NULL))
	, m_bNoExtension(FALSE)
{
}


CFindFile::~CFindFile(void)
{
	// ���������߳�
	m_bFind = FALSE;
	if (m_pFindThread)
	{
		if (::WaitForSingleObject(m_pFindThread->m_hThread, 5000) == WAIT_TIMEOUT)
		{
			::TerminateThread(m_pFindThread->m_hThread, 1);
		}
		m_pFindThread = NULL;
	}

	// ��ս���б�
	ClearFileList();
}


//************************************************************************
// Method:    IsRoot
// Purpose:   �ж��Ƿ�Ϊ��Ŀ¼
// Access:    protected 
// Returns:   BOOL		��ΪTRUE����ΪFALSE
// Qualifier:
// Parameter: LPCTSTR lpszPath	Ҫ�жϵ�Ŀ¼
//************************************************************************
BOOL CFindFile::IsRoot(LPCTSTR lpszPath)
{
	TCHAR tszRoot[4] = {0};
	wsprintf(tszRoot, _T("%c:\\"), lpszPath[0]);
	return (lstrcmp(tszRoot, lpszPath) == 0);
}


//************************************************************************
// Method:    FindProc
// Purpose:   �����߳�
// Access:    protected static 
// Returns:   UINT
// Qualifier:
// Parameter: LPVOID lpParam
//************************************************************************
UINT CFindFile::FindProc(LPVOID lpParam)
{
	if (lpParam)
	{
		CFindFile *ptr = (CFindFile *)lpParam;
		ptr->FindLoop();

		SetEvent(ptr->m_hComplete);
	}

	return 0;
}


void CFindFile::FindLoop(void)
{
	FindAllFile(m_strMainPath);
}


//************************************************************************
// Method:    StartFind
// Purpose:   ��ʼ����
// Access:    public 
// Returns:   BOOL		�ɹ�ΪTRUE��ʧ��ΪFALSE
// Qualifier:
// Parameter: LPCTSTR lpszPath		������Ŀ¼
//************************************************************************
BOOL CFindFile::StartFind(LPCTSTR lpszPath)
{
	// ��ս���б�
	ClearFileList();

	m_strMainPath = lpszPath;

	m_bFind = TRUE;
	m_pFindThread = AfxBeginThread(AFX_THREADPROC(FindProc), (LPVOID)this);

	return (m_pFindThread != NULL);
}


//************************************************************************
// Method:    StopFind
// Purpose:   ֹͣ����
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************************************************
void CFindFile::StopFind(void)
{
	m_bFind = FALSE;
	if (m_pFindThread)
	{
		if (::WaitForSingleObject(m_pFindThread->m_hThread, 5000) == WAIT_TIMEOUT)
		{
			::TerminateThread(m_pFindThread->m_hThread, 1);
		}
		m_pFindThread = NULL;
	}
}


//************************************************************************
// Method:    FindAllFile
// Purpose:   �ݹ���������������ļ�
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR lpszPath		Ҫ������Ŀ¼
//************************************************************************
void CFindFile::FindAllFile(LPCTSTR lpszPath)
{
	m_bFind = TRUE;

	// ��ʼ����
	TCHAR tszFind[MAX_PATH] = {0};
	lstrcpy(tszFind, lpszPath);

	if (!IsRoot(tszFind))
	{
		lstrcat(tszFind, _T("\\"));           
	}

	// ���������ļ�
	lstrcat(tszFind, _T("*.*"));

	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(tszFind, &wfd);

	// ���û���ҵ������ʧ��
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	} 
	
	do 
	{
		// ����������Ŀ¼
		if (wfd.cFileName[0] == '.')
			continue;

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			TCHAR tszFile[MAX_PATH] = {0};
			if (IsRoot(lpszPath))
			{
				wsprintf(tszFile, _T("%s%s"), lpszPath, wfd.cFileName);
			}
			else
			{
				wsprintf(tszFile, _T("%s\\%s"), lpszPath, wfd.cFileName);
			}

			// ����ҵ�����Ŀ¼��������Ŀ¼���еݹ�
			FindAllFile(tszFile);
		} 
		else
		{
			// ���ļ����в���
			// ɸѡ�ļ�
			CString strName(wfd.cFileName);
			int nIndex = strName.ReverseFind('.');
			if (nIndex != -1)
			{
				BOOL bFindOut(FALSE);

				strName = strName.Right(strName.GetLength()-nIndex-1);
				strName.MakeLower();

				POSITION pos = m_listExtension.GetHeadPosition();
				while (pos && m_bFind && bFindOut == FALSE)
				{
					LPCTSTR lpsz = m_listExtension.GetNext(pos);
					CString strExtension(lpsz);
					strExtension.MakeLower();

					if (strName.Compare(strExtension) == 0)
						bFindOut = TRUE;
				}

				if (bFindOut)
				{
					// ��ӵ�����б���
					CString strResult(lpszPath);
					strResult += _T("\\");
					strResult += wfd.cFileName;

					char *psz = new char[MAX_PATH]();
					int nLen = ::WideCharToMultiByte(CP_ACP, 0, strResult, -1, NULL, 0, NULL, NULL);
					::WideCharToMultiByte(CP_ACP, 0, strResult, -1, psz, nLen, NULL, NULL);

					m_listFile.AddTail(psz);
				}
			}
			else if (nIndex == -1 && m_bNoExtension == TRUE)
			{
				// ����չ�����ļ�
				// ��ӵ�����б���
				CString strResult(lpszPath);
				strResult += _T("\\");
				strResult += wfd.cFileName;

				char *psz = new char[MAX_PATH]();
				int nLen = ::WideCharToMultiByte(CP_ACP, 0, strResult, -1, NULL, 0, NULL, NULL);
				::WideCharToMultiByte(CP_ACP, 0, strResult, -1, psz, nLen, NULL, NULL);

				m_listFile.AddTail(psz);
			}
		}

	} while (FindNextFile(hFind, &wfd) && m_bFind == TRUE);

	// �رղ��Ҿ��
	FindClose(hFind);
}


//************************************************************************
// Method:    ClearResultList
// Purpose:   ����ļ��б�
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************************************************
void CFindFile::ClearFileList(void)
{
	POSITION pos = m_listFile.GetHeadPosition();
	while (pos)
	{
		LPSTR ptr = m_listFile.GetNext(pos);
		delete[] ptr;
		ptr = NULL;
	}

	m_listFile.RemoveAll();
}


//************************************************************************
// Method:    GetFirstPosition
// Purpose:   ��ȡ��λ��
// Access:    public 
// Returns:   POSITION
// Qualifier:
// Parameter: void
//************************************************************************
POSITION CFindFile::GetFirstPosition(void)
{
	return m_listFile.GetHeadPosition();
}


//************************************************************************
// Method:    GetLastPosition
// Purpose:   ��ȡβλ��
// Access:    public 
// Returns:   POSITION
// Qualifier:
// Parameter: void
//************************************************************************
POSITION CFindFile::GetLastPosition(void)
{
	return m_listFile.GetTailPosition();
}


//************************************************************************
// Method:    GetNextFile
// Purpose:   ��ȡ��һ�ļ�	
// Access:    public 
// Returns:   TYPE&						�ļ���
// Qualifier:
// Parameter: POSITION & rPosition		����Ҫ���ҵ�λ�ã�������һ��λ��
//************************************************************************
TYPE& CFindFile::GetNextFile(POSITION& rPosition)
{
	return m_listFile.GetNext(rPosition);
}


//************************************************************************
// Method:    GetPrevFile
// Purpose:   ��ȡ��һ���ļ�
// Access:    public 
// Returns:   TYPE&						�ļ���
// Qualifier:
// Parameter: POSITION & rPosition		����Ҫ���ҵ�λ�ã�������һ��λ��
//************************************************************************
TYPE& CFindFile::GetPrevFile(POSITION& rPosition)
{
	return m_listFile.GetPrev(rPosition);
}


//************************************************************************
// Method:    IsComplete
// Purpose:   �Ƿ���ɱ���
// Access:    public 
// Returns:   BOOL		���ΪTRUE������ΪFALSE
// Qualifier:
// Parameter: void
//************************************************************************
BOOL CFindFile::IsComplete(void)
{
	if (::WaitForSingleObject(m_hComplete, 0) != WAIT_TIMEOUT)
		return TRUE;

	return FALSE;
}


//************************************************************************
// Method:    SetFileExtension
// Purpose:   ����Ҫ�������ļ���չ��
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR lpszExtension		�ļ���չ��
//************************************************************************
void CFindFile::SetFileExtension(LPCTSTR lpszExtension)
{
	m_listExtension.AddTail(lpszExtension);
}


//************************************************************************
// Method:    IncludeNoExtension
// Purpose:   ����û����չ�����ļ�
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: BOOL bNoExtension		����ΪTRUE������ΪFALSE
//************************************************************************
void CFindFile::IncludeNoExtension(BOOL bNoExtension)
{
	m_bNoExtension = bNoExtension;
}