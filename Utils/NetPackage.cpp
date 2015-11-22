// NetPackage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NetPackage.h"


// CNetPackage

CNetPackage::CNetPackage()
	: m_usDataType(0)
	, m_usDataChannel(0)
	, m_pBuf(nullptr)
	, m_nBufSize(32*1024*1024)
	, m_nLen(0)
	, m_oleLastRecvTime(COleDateTime::GetCurrentTime())
	, m_nWritePos(0)
	, m_nReadPos(0)
	, m_bFindHeader(FALSE)
	, m_nFrameLen(0)
	, m_hCLose(CreateEvent(NULL, TRUE, FALSE, NULL))
	, m_hPackageThread(nullptr)
	, m_bWorkFlag(FALSE)
	, m_nCallBack(0)
{
	memset(m_chFrameFlag, FRAME_FLAG, FRAME_FLAG_LEN);
}

CNetPackage::~CNetPackage()
{
	SetEvent(m_hCLose);
	if (m_hPackageThread)
	{
		if (::WaitForSingleObject(m_hPackageThread->m_hThread, WAIT_THREAD_EXIT) == WAIT_TIMEOUT)
			::TerminateThread(m_hPackageThread->m_hThread, 1);
		m_hPackageThread = nullptr;
	}

	if (m_pBuf)
	{
		delete[] m_pBuf;
		m_pBuf = nullptr;
	}

	if (m_pNetBuf)
	{
		delete m_pNetBuf;
		m_pNetBuf = nullptr;
	}
}


// CNetPackage ��Ϣ�������



// =========================================================================================
// ��ʼ������
// =========================================================================================
void CNetPackage::Init(void)
{
	m_bWorkFlag = TRUE;

	m_pBuf = new BYTE[m_nBufSize]();
	m_pNetBuf = new CNetBuffer;
	m_pNetBuf->Init(16*1024*1024, 5);

	m_hPackageThread = AfxBeginThread((AFX_THREADPROC)PackageProc, (LPVOID)this);
}

// =========================================================================================
// �������ݵ����������
// =========================================================================================
int CNetPackage::CopyData(char* pBuf, UINT nLen)
{
	LockWrite();
	if (m_nLen >= m_nBufSize)
	{
		UnLockWrite();
		return;
	}

	int i(0);
	for (; i < nLen; ++i)
	{
		m_pBuf[m_nWritePos] = *(pBuf+i);
		m_nWritePos = ++m_nWritePos % m_nBufSize;
		if (++m_nLen >= m_nBufSize)
		{
			break;
		}
	}
	UnLockWrite();

	return i;
}

// =========================================================================================
// �����������
// =========================================================================================
void CNetPackage::SetDataType(USHORT usType)
{
	LockWrite();
	m_usDataType = usType;
	UnLockWrite();
}


USHORT CNetPackage::GetDataType(void)
{
	return USHORT(m_usDataType);
}

// =========================================================================================
// ��������ͨ��
// =========================================================================================
void CNetPackage::SetDataChannel(USHORT usChannel)
{
	LockWrite();
	m_usDataChannel = usChannel;
	UnLockWrite();
}


USHORT CNetPackage::GetDataChannel(void)
{
	return USHORT(m_usDataChannel);
}

// =========================================================================================
// ����֡ͷ
// =========================================================================================
void CNetPackage::FindHeader(void)
{
	// ѭ����ͷ
	while (m_nLen >= sizeof(MULTICAST_HEADER))
	{
		// ����ͷ��־
		for (UINT i(0); m_pBuf[m_nReadPos] != FRAME_FLAG && m_nLen > 0; ++i)
		{
			m_nReadPos = (m_nReadPos+1) % m_nBufSize;
			m_nLen--;

			// ÿ��500���ֽ�δ�ҵ�ͷ����ͣ1����
			if (i > 500)
			{
				i = 0;
				::Sleep(1);
			}
		}

		if (IsHeader())
		{
			// ��ȡ֡����
			UINT nFrameLenPos = (m_nReadPos+sizeof(MULTICAST_HEADER)-sizeof(UINT)) % m_nBufSize;

			BYTE* p = (BYTE*)(&m_nFrameLen);
			for (UINT n(0); n < sizeof(m_nFrameLen); ++n)
			{
				*(p+n) = m_pBuf[nFrameLenPos];
				nFrameLenPos = (++nFrameLenPos) % m_nBufSize;
			}

			// ��֡���ȴ��ڻ��������ȣ�������ͷ
			if(m_nFrameLen > m_nBufSize)
			{
				m_nReadPos = nFrameLenPos;
				m_nLen -= sizeof(MULTICAST_HEADER)-sizeof(UINT);
				continue;
			}

			m_bFindHeader = TRUE;
			return;
		}
	}
}

// =========================================================================================
// �ж�֡ͷ��־�������������ͨ��
// =========================================================================================
BOOL CNetPackage::IsHeader(void)
{
	//�ж������Ƿ�Ϊ֡ͷ
	int nPos(m_nReadPos);
	for(UINT i(0); i < FRAME_FLAG_LEN; ++i)
	{
		if(m_pBuf[nPos] != FRAME_FLAG)
		{
			//��λ����ǰƫ��i�����ȼ�����Ӧ�ֽ�
			m_nReadPos = (m_nReadPos+i) % m_nBufSize;
			m_nLen -= i;
			return FALSE;
		}
		//��λ����ǰƫ��1
		nPos = ++nPos % m_nBufSize;
	}

#if 1
	USHORT n(0);
	BYTE* p = (BYTE*)&n;

	// �ж��������
	*p = m_pBuf[nPos];
	*(p+1) = m_pBuf[++nPos % m_nBufSize];
	if (n != m_usDataType)
	{
		//��λ����ǰƫ��2�����ȼ�����Ӧ�ֽ�
		m_nReadPos = (m_nReadPos+2) % m_nBufSize;
		m_nLen -= 2;
		return FALSE;
	}

	// �ж�����ͨ��
	*p = m_pBuf[++nPos % m_nBufSize];
	*(p+1) = m_pBuf[++nPos % m_nBufSize];
	if (n != m_usDataChannel)
	{
		//��λ����ǰƫ��2�����ȼ�����Ӧ�ֽ�
		m_nReadPos = (m_nReadPos+2) % m_nBufSize;
		m_nLen -= 2;
		return FALSE;
	}
#endif
	TRACE(_T("\nfind out! "));
	return TRUE;
}

// =========================================================================================
// �ж�֡ͷ��־
// =========================================================================================
BOOL CNetPackage::IsHeader(UINT nStartPos)
{
	//�ж������Ƿ�Ϊ֡ͷ
	for(UINT i(0); i < FRAME_FLAG_LEN; ++i)
	{
		if(m_pBuf[nStartPos] != FRAME_FLAG)
		{
		//	TRACE(_T("is not header %d\n"), i);
			return FALSE;
		}
		//��λ��ƫ��1
		nStartPos = ++nStartPos % m_nBufSize;
	}
	return TRUE;
}

// =========================================================================================
// ����߳�
// =========================================================================================
UINT CNetPackage::PackageProc(LPVOID lpParam)
{
	ASSERT(lpParam);
	CNetPackage* ptr = (CNetPackage*)lpParam;
	while (::WaitForSingleObject(ptr->m_hCLose, 1) == WAIT_TIMEOUT)
	{
//  		if (ptr->m_nLen <= sizeof(MULTICAST_HEADER))
// 			continue;

	//	ptr->LockRead();
		ptr->PackageLoop();
	//	ptr->UnLockRead();
	}

	return 0;
}

// =========================================================================================
// �������
// =========================================================================================
void CNetPackage::PackageLoop(void)
{
	if (m_bFindHeader)
	{
		// ��¼��ʼλ��
		UINT nPreReadPos(m_nReadPos);
		UINT nPreLen(m_nLen);

		if (m_nLen >= 2*sizeof(MULTICAST_HEADER)+m_nFrameLen)
		{
			// ----------- ƫ��֡���ȵ��ֽڵ����ݣ������������֡ͷ������Ϊ������֡ ------------ //
			m_nReadPos = (m_nReadPos+sizeof(MULTICAST_HEADER)+m_nFrameLen) % m_nBufSize;
			m_nLen = m_nLen - (sizeof(MULTICAST_HEADER)+m_nFrameLen);

			if (IsHeader(m_nReadPos))
			{
				//д��֡����
				UINT nDataPos = (nPreReadPos+sizeof(MULTICAST_HEADER)) % m_nBufSize;
				int nRet = WriteBuffer(nDataPos, m_nFrameLen);
				if (nRet >= 0)
				{
					::PostMessage((HWND)m_nCallBack, WM_NET_RESOLVE, (WPARAM)m_pNetBuf, NULL);
				}
				else
					TRACE("I---write buffer fail\n");
				TRACE("I--Post message\n");
			} 
			else
			{
				m_nReadPos = (nPreReadPos+sizeof(MULTICAST_HEADER)-sizeof(UINT)) % m_nBufSize;
				m_nLen = nPreLen - (sizeof(MULTICAST_HEADER)-sizeof(UINT));
			}
			TRACE(_T("....I%d, len:%d\n"), m_nFrameLen, m_nLen);
			m_bFindHeader = FALSE;
			return;
		}
		else if (m_nLen >= sizeof(MULTICAST_HEADER)+m_nFrameLen)
		{
			// --------------- ������������м�û������ͷ��Ҳ���Գ�֡ --------------------- //
			m_nReadPos = (m_nReadPos+sizeof(MULTICAST_HEADER)) % m_nBufSize;
			m_nLen = m_nLen - sizeof(MULTICAST_HEADER);
			UINT nFrameLen(0);
			BOOL bFindHeader(FALSE);

			while (nFrameLen <= m_nFrameLen)
			{
				if (IsHeader(m_nReadPos))
				{
					bFindHeader = TRUE;
					break;
				}
				m_nReadPos = ++m_nReadPos % m_nBufSize;
				++nFrameLen;
			}
			m_nReadPos = nPreReadPos;
			m_nLen = nPreLen;

			if (!bFindHeader)
			{
				//д��֡����
				UINT nDataPos = (nPreReadPos+sizeof(MULTICAST_HEADER)) % m_nBufSize;
				int nRet = WriteBuffer(nDataPos, m_nFrameLen);
				if (nRet >= 0)
				{
					::PostMessage((HWND)m_nCallBack, WM_NET_RESOLVE, (WPARAM)m_pNetBuf, NULL);
				}
				else
					TRACE("II---write buffer fail\n");
				TRACE("II---Post message\n");
				m_nReadPos = (m_nReadPos+sizeof(MULTICAST_HEADER)+m_nFrameLen) % m_nBufSize;
				m_nLen = m_nLen - (sizeof(MULTICAST_HEADER)+m_nFrameLen);
			}
			TRACE(_T("....II%d, len:%d\n"), m_nFrameLen, m_nLen);
			m_bFindHeader = FALSE;
			return;
		} 
		else if (m_nLen < m_nFrameLen+sizeof(MULTICAST_HEADER) && m_nLen > sizeof(MULTICAST_HEADER) && !m_bWorkFlag)
		{
			// ------------------------ ���ڲ�֡����Ȼ��ʾ -----------------------//
			if (m_usDataType == TYPE_RADAR)
			{
				m_bFindHeader = FALSE;
				m_nFrameLen = 0;
				return;
			}

			m_nReadPos = (m_nReadPos+sizeof(MULTICAST_HEADER)) % m_nBufSize;
			m_nLen -= sizeof(MULTICAST_HEADER);

			UINT nFrameLen(0);
			BOOL bFindHeader(FALSE);
			while(nFrameLen <= m_nLen)
			{
				if(IsHeader(m_nReadPos))
				{
					bFindHeader = TRUE;
					break;
				}
				m_nReadPos = ++m_nReadPos % m_nBufSize;
				++nFrameLen;
			}

			//д��֡����
			UINT nDataPos = (nPreReadPos+sizeof(MULTICAST_HEADER)) %m_nBufSize;
			int nRet = WriteBuffer(nDataPos, nFrameLen-1);
		//	TRACE("III---Post message\n");
			if (nRet >= 0)
			{
				::PostMessage((HWND)m_nCallBack, WM_NET_RESOLVE, (WPARAM)m_pNetBuf, NULL);
			}

			bFindHeader = FALSE;
			m_nLen = 0;
			return;
		}
	}
	else
	{
		FindHeader();
	}
}

// =========================================================================================
// д����
// =========================================================================================
void CNetPackage::LockWrite(void)
{
	m_ccs.Lock();
}

// =========================================================================================
// д����
// =========================================================================================
void CNetPackage::UnLockWrite(void)
{
	m_ccs.Unlock();
}

// =========================================================================================
// ������
// =========================================================================================
void CNetPackage::LockRead(void)
{
	m_ccs.Lock();
}

// =========================================================================================
// ������
// =========================================================================================
void CNetPackage::UnLockRead(void)
{
	m_ccs.Unlock();
}

// =========================================================================================
// д�����ݵ�������������
// =========================================================================================
BOOL CNetPackage::WriteBuffer(UINT nStartPos, UINT nLen)
{
	UINT nIndex(0);
	if (nStartPos+nLen > m_nBufSize-1)
	{
		// ���д���
		UINT nLeft(m_nBufSize - nStartPos);
		UINT nRight(nLen - nLeft);
		nIndex = m_pNetBuf->AddData(m_pBuf+nStartPos, nLeft);
		if (nIndex >= 0)
		{
			nIndex = m_pNetBuf->AddDataAt(m_pBuf, nRight, nIndex);
		}
	}
	else
	{
		nIndex = m_pNetBuf->AddData(m_pBuf+nStartPos, nLen);
	}

	return nIndex;
}

// =========================================================================================
// �����������״̬���
// =========================================================================================
void CNetPackage::SetWorkFlag(BOOL bWork)
{
	LockWrite();
	m_bWorkFlag = bWork;
	UnLockWrite();
}

// =========================================================================================
// ��ȡ����������������
// =========================================================================================
void CNetPackage::ReadBuffer(PUCHAR& pBuf, UINT& nLen)
{
	m_pNetBuf->GetData(pBuf, nLen);
}

// =========================================================================================
// ���ûص����ھ��
// =========================================================================================
void CNetPackage::SetCallBack(int nCallBack)
{
	LockWrite();
	m_nCallBack = nCallBack;
	UnLockWrite();
}

// =========================================================================================
// �ͷ��Ѷ�ȡ�Ļ���������
// =========================================================================================
void CNetPackage::ReleaseBuffer(void)
{
	m_pNetBuf->DeleteData();
}
