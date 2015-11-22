/********************************************************************
	Copyright 	(C) 2014 All Right Reserved
	Created:	2014/11/17 19:26
	FileName: 	FileEnum.h
	Version:	1.0.0.0
	Author:		Jasen.Xu	
	Purpose:	���������ļ�Ŀ¼
*********************************************************************/
#include "StdAfx.h"
#include "FileEnum.h"



/************************************************************************/
/* �ļ���������ʵ����                                                      */
/************************************************************************/
CBaseFileFinder::CBaseFileFinder(LPCTSTR lpszDirectory, FindDataInfo& fdi)
	: m_fileInfo(fdi)
	, m_strPath(_T(""))
	, m_nFolderLen(0)
	, m_hFind(NULL)
{
	Init(lpszDirectory);
}


CBaseFileFinder::~CBaseFileFinder(void)
{
}


//************************************************************************
// Method:    Init
// Purpose:   ȥ��Ŀ¼�����Դ���"\\"
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR lpszDirectory	Ҫ������Ŀ¼
//************************************************************************
void CBaseFileFinder::Init(LPCTSTR lpszDirectory)
{
	m_strPath = lpszDirectory;
	m_nFolderLen = m_strPath.GetLength();

	if (m_strPath[m_nFolderLen-1] == _T('\\'))
	{
		m_strPath.TrimRight(_T('\\'));
		m_nFolderLen = m_strPath.GetLength();
	}
}


//************************************************************************
// Method:    EnumCurDirFirst
// Purpose:   ���ҵ�ǰĿ¼�ĵ�һ���ļ�
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: void
//************************************************************************
BOOL CBaseFileFinder::FindCurDirFirst(void)
{
	m_strPath += _T("\\*");
	++m_nFolderLen;

	m_hFind = ::FindFirstFile(m_strPath, &m_fileInfo);
	if (m_hFind != INVALID_HANDLE_VALUE)
	{
		m_strPath = m_strPath.Left(m_nFolderLen) + m_fileInfo.cFileName;
		return TRUE;
	} 

	return FALSE;
}


//************************************************************************
// Method:    FindCurDirNext
// Purpose:   ���ҵ�ǰĿ¼����һ���ļ�
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: void
//************************************************************************
BOOL CBaseFileFinder::FindCurDirNext(void)
{
	BOOL bRet = ::FindNextFile(m_hFind, &m_fileInfo);
	if (bRet)
	{
		m_strPath = m_strPath.Left(m_nFolderLen) + m_fileInfo.cFileName;
	} 
	else
	{
		::FindClose(m_hFind);
		m_hFind = INVALID_HANDLE_VALUE;
	}

	return bRet;
}


//************************************************************************
// Method:    IsFinished
// Purpose:   �Ƿ���ɱ���
// Access:    public 
// Returns:   BOOL		���ΪTRUE������ΪFALSE
// Qualifier: const
// Parameter: void
//************************************************************************
BOOL CBaseFileFinder::IsFinished(void) const
{
	return (m_hFind == INVALID_HANDLE_VALUE);
}


//************************************************************************
// Method:    GetPath
// Purpose:   ��ȡ��ǰ���ҵ�Ŀ¼
// Access:    public 
// Returns:   LPCTSTR		��ǰ������Ŀ¼
// Qualifier: const
// Parameter: void
//************************************************************************
LPCTSTR CBaseFileFinder::GetPath(void) const
{
	return LPCTSTR(m_strPath);
}


//************************************************************************
// Method:    GetFileInfo
// Purpose:   ��ȡ�ļ���Ϣ
// Access:    public 
// Returns:   const FindDataInfo&
// Qualifier: const
//************************************************************************
const FindDataInfo& CBaseFileFinder::GetFileInfo() const
{
	return m_fileInfo;
}


//************************************************************************
// Method:    IsDirectory
// Purpose:   �Ƿ�Ϊ�ļ�Ŀ¼
// Access:    public 
// Returns:   BOOL		��ΪTRUE������ΪFALSE
// Qualifier: const
// Parameter: void
//************************************************************************
BOOL CBaseFileFinder::IsDirectory(void) const
{
	return (m_fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}


//************************************************************************
// Method:    IsDot
// Purpose:   �Ƿ�Ϊ.Ŀ¼
// Access:    public 
// Returns:   BOOL		��ΪTRUE������ΪFALSE
// Qualifier: const
// Parameter: void
//************************************************************************
BOOL CBaseFileFinder::IsDot(void) const
{
	return (m_fileInfo.cFileName[0] == '.') && ((m_fileInfo.cFileName[1] == '.') || (m_fileInfo.cFileName[1] == '\0'));
}


//************************************************************************
// Method:    IsReparsePoint
// Purpose:   �Ƿ�Ϊ�ؽ�����
// Access:    public 
// Returns:   BOOL		��ΪTRUE������ΪFALSE
// Qualifier: const
// Parameter: void
//************************************************************************
BOOL CBaseFileFinder::IsReparsePoint(void) const
{
	return (m_fileInfo.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
}



/************************************************************************/
/* ���������������ʵ����	                                                */
/************************************************************************/
CDFSFinder::CDFSFinder(LPCTSTR lpszDirectory, FindDataInfo& fdi)
	: CBaseFileFinder(lpszDirectory, fdi)
	, m_nFolderLevel(0)
{
}

CDFSFinder::~CDFSFinder()
{
	for (int i(0); i < m_nFolderLevel; ++i)
	{
		EnumFolderCtx& efc = m_arEnumFolderCtx[i];
		::FindClose(efc.hFind);
	}
}

BOOL CDFSFinder::FindSubDirFirst(void)
{
	if (IsDirectory() && !IsDot())
	{
		int nCurFolderLen = m_nFolderLen;
		m_nFolderLen += _tcslen(m_fileInfo.cFileName);
		BOOL bRet = FindCurDirFirst();
		if (!bRet)
		{
			m_nFolderLen = nCurFolderLen;
		}
		return bRet;
	}

	return FALSE;
}


BOOL CDFSFinder::FindParentDirNext(void)
{
	--m_nFolderLevel;
	if (m_nFolderLevel > 0)
	{
		EnumFolderCtx& efcParent = m_arEnumFolderCtx[m_nFolderLevel-1];
		m_nFolderLen = efcParent.nFolderPathLen;
		m_hFind = efcParent.hFind;
		return FindCurDirNext();
	}
	return FALSE;
}


BOOL CDFSFinder::IsFinished(void)
{
	return (CBaseFileFinder::IsFinished() && m_nFolderLevel == 0);
}


BOOL CDFSFinder::FindCurDirFirst(void)
{
	BOOL bRet = CBaseFileFinder::FindCurDirFirst();
	if (bRet)
	{
		EnumFolderCtx efc(m_hFind, m_nFolderLen);
		if (m_nFolderLevel >= static_cast<int>(m_arEnumFolderCtx.size()))
		{
			m_arEnumFolderCtx.push_back(efc);
		} 
		else
		{
			m_arEnumFolderCtx[m_nFolderLevel] = efc;
		}
		++m_nFolderLevel;
	}

	return bRet;
}



/************************************************************************/
/* �ļ���������                                                           */
/************************************************************************/
CFileEnumBase::CFileEnumBase()
{
}

CFileEnumBase::~CFileEnumBase()
{
	while (!m_queueFinder.empty())
	{
		CBaseFileFinder *pFinder = m_queueFinder.front();
		m_queueFinder.pop();
		DELETE_OBJECT(pFinder);
	}
}

BOOL CFileEnumBase::EnumFiles(LPCTSTR lpszDirectory, BOOL bFindSubDir, HANDLE hStopEvent)
{
	if (!lpszDirectory || !*lpszDirectory)
	{
		return FALSE;
	}

	BOOL bRet = TRUE;
	FindDataInfo fdi = {0};

#ifdef __RECURSION
	// �ݹ�����
	bRet = EnumRecursively(lpszDirectory, fdi, bFindSubDir, hStopEvent);
#else
	if (bFindSubDir)
	{
#ifdef __BFS
		// �����������
		bRet = EnumBFS(lpszDirectory, fdi, hStopEvent);
#else
		// �����������
		bRet = EnumDFS(lpszDirectory, fdi, hStopEvent);
#endif // __BFS
	}
	else
	{
		// ��������Ŀ¼
		CBaseFileFinder fileFinder(lpszDirectory, fdi);
		if (!fileFinder.FindCurDirFirst())
		{
			return FALSE;
		} 

		for (; !fileFinder.IsFinished() && !IsStopEventSignaled(); fileFinder.FindCurDirNext())
		{
			const FindDataInfo& fileInfo = fileFinder.GetFileInfo();
			if (CheckUseFile(fileFinder.GetPath(), fileInfo))
			{
				HandleFile(fileFinder.GetPath(), fileInfo);
			}
		}
	}
#endif // __RECURSION

	return bRet;
}


//************************************************************************
// Method:    CheckUseFile
// Purpose:   ȷ���Ƿ�����Ҫ���ļ�
// Access:    virtual protected 
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpszPath
// Parameter: const FindDataInfo & fdi
//************************************************************************
BOOL CFileEnumBase::CheckUseFile(LPCTSTR lpszPath, const FindDataInfo& fdi)
{
	return TRUE;
}


//************************************************************************
// Method:    CheckUseDir
// Purpose:   ȷ���Ƿ�����Ҫ��Ŀ¼
// Access:    virtual protected 
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpszPath
// Parameter: const FindDataInfo & fdi
//************************************************************************
BOOL CFileEnumBase::CheckUseDir(LPCTSTR lpszPath, const FindDataInfo& fdi)
{
	return TRUE;
}


//************************************************************************
// Method:    HandleFile
// Purpose:   �������������ļ�
// Access:    virtual protected 
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR lpszPath
// Parameter: const FindDataInfo & fdi
//************************************************************************
void CFileEnumBase::HandleFile(LPCTSTR lpszPath, const FindDataInfo& fdi)
{
}


//************************************************************************
// Method:    HandleDir
// Purpose:   ������������Ŀ¼
// Access:    virtual protected 
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR lpszPath
// Parameter: const FindDataInfo & fdi
//************************************************************************
void CFileEnumBase::HandleDir(LPCTSTR lpszPath, const FindDataInfo& fdi)
{
}


//************************************************************************
// Method:    FinishedDir
// Purpose:   ��ɵ�ǰĿ¼��������Ĵ���
// Access:    virtual protected 
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR lpszPath
//************************************************************************
void CFileEnumBase::FinishedDir(LPCTSTR lpszPath)
{
}

#ifdef __RECURSION
//************************************************************************
// Method:    EnumRecursively
// Purpose:   ������ȵݹ�����
// Access:    protected 
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpszDirectory		// Ҫ������Ŀ¼
// Parameter: FindDataInfo & fdi		// ������Ϣ
// Parameter: BOOL bFindSubDir			// �Ƿ������Ŀ¼
// Parameter: HANDLE hStopEvent			// ֹͣ�ź���
//************************************************************************
BOOL CFileEnumBase::EnumRecursively(LPCTSTR lpszDirectory, FindDataInfo& fdi, BOOL bFindSubDir, HANDLE hStopEvent)
{
	CBaseFileFinder fileFinder(lpszDirectory, fdi);
	
	// Ѱ�ұ����ĸ��ڵ�
	BOOL bRet = fileFinder.FindCurDirFirst();
	if (bRet == FALSE)
	{
		return FALSE;
	} 
	
	while (!fileFinder.IsFinished() && !IsStopEventSignaled())
	{
		const FindDataInfo& fileInfo = fileFinder.GetFileInfo();
		// ��������ѽ������Ľڵ�
		if (!fileFinder.IsReparsePoint())
		{
			// ������ļ�Ŀ¼
			if (fileFinder.IsDirectory())
			{
				// �������.��..Ŀ¼
				if (!fileFinder.IsDot() && bFindSubDir)
				{
					if (CheckUseDir(fileFinder.GetPath(), fileInfo))
					{
						// �����ҵ���Ŀ¼
						HandleDir(fileFinder.GetPath(), fileInfo);
						bRet &= EnumRecursively(fileFinder.GetPath(), fdi, bFindSubDir);
					}
				}
			}
			else
			{
				// ������ļ�
				if (CheckUseFile(fileFinder.GetPath(), fileInfo))
				{
					// �����ҵ����ļ�
					HandleFile(fileFinder.GetPath(), fileInfo);
				//	TRACE(_T("%s\n"), fileFinder.GetPath());
				}
			}	
		}

		// ����Ҳ�����һ��Ŀ��ʱ���˳�����
		if (!fileFinder.FindCurDirNext())
		{
			FinishedDir(fileFinder.GetPath());
			break;
		}
	}

	return bRet;
}

#else
#ifdef __BFS
//************************************************************************
// Method:    EnumBFS
// Purpose:   �ǵݹ�����������
// Access:    protected 
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpszDirectory		// Ҫ������Ŀ¼
// Parameter: FindDataInfo & fdi		// ������Ϣ
// Parameter: HANDLE hStopEvent			// ֹͣ�ź���
//************************************************************************
BOOL CFileEnumBase::EnumBFS(LPCTSTR lpszDirectory, FindDataInfo& fdi, HANDLE hStopEvent)
{
	CBaseFileFinder *pFinder = NULL;
	try
	{
		pFinder = new CBaseFileFinder(lpszDirectory, fdi);
	}
	catch (...)
	{
		DELETE_OBJECT(pFinder);
		return FALSE;
	}

	BOOL bRet = pFinder->FindCurDirFirst();
	if (bRet == FALSE)
	{
		DELETE_OBJECT(pFinder);
		return FALSE;
	}

	while (!pFinder->IsFinished() && !IsStopEventSignaled())
	{
		const FindDataInfo& fileInfo = pFinder->GetFileInfo();
		// ��������ѽ������Ľڵ�
		if (!pFinder->IsReparsePoint())
		{
			// ������ļ�Ŀ¼
			if (pFinder->IsDirectory())
			{
				// �������.��..Ŀ¼
				if (!pFinder->IsDot())
				{
					if (CheckUseDir(pFinder->GetPath(), fileInfo))
					{
						// �����ҵ���Ŀ¼
						HandleDir(pFinder->GetPath(), fileInfo);
					//	TRACE(_T("%s\n"), pFinder->GetPath());

						CBaseFileFinder *pNewFinder = NULL;
						try
						{
							//  ��ӵ�������
							pNewFinder = new CBaseFileFinder(pFinder->GetPath(), fdi);
							m_queueFinder.push(pNewFinder);
						}
						catch (...)
						{
							DELETE_OBJECT(pNewFinder);
						}
					}
				}
			} 
			else
			{
				// ������ļ�
				if (CheckUseFile(pFinder->GetPath(), fileInfo))
				{
					// �����ҵ����ļ�
					HandleFile(pFinder->GetPath(), fileInfo);
					//TRACE(_T("%s\n"), pFinder->GetPath());
				}
			}
		}

		// ����Ҳ�����һ��Ŀ��ʱ���˳�����
		if (!pFinder->FindCurDirNext())
		{
			FinishedDir(pFinder->GetPath());
			if (m_queueFinder.empty())
			{
				break;
			}
			else
			{
				// ����������δ��
				while (!IsStopEventSignaled())
				{
					// ȡ������Ԫ��
					CBaseFileFinder *pNextFinder = m_queueFinder.front();
					m_queueFinder.pop();

					DELETE_OBJECT(pFinder);
					pFinder = pNextFinder;
					
					// ���ҵ�һ��Ŀ¼
					if (!pFinder->FindCurDirFirst())
					{
						// ����ʧ�ܣ���ն���
						DELETE_OBJECT(pFinder);
						while (!m_queueFinder.empty())
						{
							pFinder = m_queueFinder.front();
							m_queueFinder.pop();
							DELETE_OBJECT(pFinder);
						}
					}
					else
					{
						break;
					}
				} // ����������δ��
			}
		} // ����Ҳ�����һ��Ŀ��ʱ���˳�����
	}

	// ��ն���
	DELETE_OBJECT(pFinder);
	while (!m_queueFinder.empty())
	{
		pFinder = m_queueFinder.front();
		m_queueFinder.pop();
		DELETE_OBJECT(pFinder);
	}

	return bRet;
}

#else
//************************************************************************
// Method:    EnumDFS
// Purpose:   �ǵݹ������������
// Access:    protected 
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpszDirectory		// Ҫ������Ŀ¼
// Parameter: FindDataInfo & fdi		// ������Ϣ
// Parameter: HANDLE hStopEvent			// ֹͣ�ź���
//************************************************************************
BOOL CFileEnumBase::EnumDFS(LPCTSTR lpszDirectory, FindDataInfo& fdi, HANDLE hStopEvent)
{
	CDFSFinder fileFinder(lpszDirectory, fdi);
	if (!fileFinder.FindCurDirFirst())
	{
		return FALSE;
	}

	while (!fileFinder.IsFinished() && !IsStopEventSignaled())
	{
		const FindDataInfo& fileInfo = fileFinder.GetFileInfo();
		// ��������ѽ������Ľڵ�
		if (!fileFinder.IsReparsePoint())
		{
			// ������ļ�Ŀ¼
			if (fileFinder.IsDirectory())
			{
				// �������.��..Ŀ¼
				if (!fileFinder.IsDot())
				{
					if (CheckUseDir(fileFinder.GetPath(), fileInfo))
					{
						// �����ҵ���Ŀ¼
						HandleDir(fileFinder.GetPath(), fileInfo);
						if (!fileFinder.FindSubDirFirst())
						{
							return FALSE;
						}
					}
				}
			} 
			else
			{
				// ������ļ�
				if (CheckUseFile(fileFinder.GetPath(), fileInfo))
				{
					// �����ҵ����ļ�
					HandleFile(fileFinder.GetPath(), fileInfo);
				//	TRACE(_T("%s\n"), fileFinder.GetPath());
				}
			}
		}

		// ����Ҳ�����һ���ļ�ʱ
		if (!fileFinder.FindCurDirNext())
		{
			// ������һ�����ڵ�Ŀ¼
			do 
			{
				FinishedDir(fileFinder.GetPath());

			} while (!fileFinder.FindParentDirNext() && !fileFinder.IsFinished() && !IsStopEventSignaled());
		}
	}

	return TRUE;
}
#endif // __BFS
#endif // __RECURSION
