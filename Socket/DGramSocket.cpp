// CDGramSocket.cpp : ʵ���ļ�
//


#include "StdAfx.h"
#include "DGramSocket.h"


CDGramSocket::CDGramSocket(void)
	: m_strTargetIP(_T(""))
	, m_nTargetPort(0)
	, m_nLocalPort(0)
	, m_bIsReceiving(FALSE)
	, m_hRecviveThread(NULL)
{
}


CDGramSocket::~CDGramSocket(void)
{
	// ��������ʹ�ù���Socket
	CloseSender();
	CloseReceiver();
	StopReceiveThread();
}

//------------------------------------------------------------------------
// ����Զ�̶�IP
void CDGramSocket::SetTargetIP(CString strTargetIP)
{
	m_strTargetIP = strTargetIP;
}

//------------------------------------------------------------------------
// ��ȡԶ�̶�IP
CString CDGramSocket::GetTargetIP(void)
{
	return CString(m_strTargetIP);
}

//------------------------------------------------------------------------
// ����Զ�̶˿ں�
void CDGramSocket::SetTargetPort(UINT nTargetPort)
{
	m_nTargetPort = nTargetPort;
}

//------------------------------------------------------------------------
// ��ȡԶ�̶˿ں�
UINT CDGramSocket::GetTargetPort(void)
{
	return m_nTargetPort;
}

//------------------------------------------------------------------------
// ���ñ��ض˿ں�
void CDGramSocket::SetLocalPort(UINT nLocalPort)
{
	m_nLocalPort = nLocalPort;
}

//------------------------------------------------------------------------
// ��ȡ���ض˿ں�
UINT CDGramSocket::GetLocalPort(void)
{
	return m_nLocalPort;
}

//------------------------------------------------------------------------
// �������ڽ��յ�Socket
BOOL CDGramSocket::CreateReceiver(UINT nLocalPort, LPCTSTR lpszSocketAddress)
{
	CloseReceiver();

	// ����һ��UDP���͵�Socket
	m_sockReceiver = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_sockReceiver != INVALID_SOCKET)
	{
		// ��Socket�����ò����������ַ����
		BOOL bOptionValue(TRUE);
		int nRet = setsockopt(m_sockReceiver, SOL_SOCKET, SO_REUSEADDR, (char FAR*)&bOptionValue, sizeof(bOptionValue));
		if (nRet == SOCKET_ERROR)
		{
			CloseReceiver();
			return FALSE;
		}

		m_nLocalPort = nLocalPort;

		// ��Socket�󶨵����ض˿ں���
		SOCKADDR_IN addr = {0};
		addr.sin_family			= AF_INET;
		addr.sin_addr.s_addr	= htonl(INADDR_ANY);
		addr.sin_port			= htons(m_nLocalPort);

		nRet = bind(m_sockReceiver, (SOCKADDR*)&addr, sizeof(addr));
		if (nRet == SOCKET_ERROR)
		{
			CloseReceiver();
			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------
// �ر����ڽ��յ�Socket
void CDGramSocket::CloseReceiver(void)
{
	if (m_sockReceiver != INVALID_SOCKET)
	{
		closesocket(m_sockReceiver);
		m_sockReceiver = INVALID_SOCKET;
	}
}

//------------------------------------------------------------------------
// �������ڷ��͵�Socket
BOOL CDGramSocket::CreateSender(UINT nTargetPort, LPCTSTR lpszTargetAddress)
{
	CloseSender();

	m_nTargetPort = nTargetPort;
	m_strTargetIP = lpszTargetAddress;

	// ����һ��UDP���͵�Socket
	m_sockSender = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_sockSender != INVALID_SOCKET)
	{
		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------
// �ر����ڷ��͵�Socket
void CDGramSocket::CloseSender(void)
{
	if (m_sockSender != INVALID_SOCKET)
	{
		closesocket(m_sockSender);
		m_sockSender = INVALID_SOCKET;
	}
}

//------------------------------------------------------------------------
// ��������
int CDGramSocket::ReceiveFrom(LPVOID lpBuf, int nBufLen, CString& rSocketAddress, UINT& rSocketPort, int nFlags)
{
	SOCKADDR_IN addr = {0};
	int nAddrLen(sizeof(addr));

	int nRet = recvfrom(m_sockReceiver, (char*)lpBuf, nBufLen, 0, (SOCKADDR*)&addr, &nAddrLen);

	try
	{
		char* pszAddr(inet_ntoa(addr.sin_addr));
		rSocketAddress = pszAddr;
		rSocketPort = ntohs(addr.sin_port);
	}
	catch (...)
	{
		rSocketAddress = _T("");
		rSocketPort = 0;
	}

	return nRet;
}


int CDGramSocket::ReceiveFrom(LPVOID lpBuf, int nBufLen, SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags)
{
	int nRet = recvfrom(m_sockReceiver, (char*)lpBuf, nBufLen, 0, (SOCKADDR*)lpSockAddr, lpSockAddrLen);
	return nRet;
}


int CDGramSocket::ReceiveFrom(LPVOID lpBuf, int nBufLen)
{
	int nRet = recvfrom(m_sockReceiver, (char*)lpBuf, nBufLen, 0, NULL, NULL);
	return nRet;
}


//------------------------------------------------------------------------
// ��������
int CDGramSocket::SendTo(const LPVOID lpBuf, int nBufLen, UINT nTargetPort, LPCTSTR lpszTargetAddress, int nFlags)
{
	char szIP[MAX_PATH] = {0};
	CString str(lpszTargetAddress);
	CStringToPSZ(str, szIP);

	SOCKADDR_IN addr = {0};
	addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= inet_addr(szIP);
	addr.sin_port			= htons((USHORT)nTargetPort);

	int nRet = sendto(m_sockSender, (char*)lpBuf, nBufLen, 0, (SOCKADDR*)&addr, sizeof(addr));
	return nRet;
}


int CDGramSocket::SendTo(const LPVOID lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags)
{
	int nRet = sendto(m_sockSender, (char*)lpBuf, nBufLen, 0, (SOCKADDR*)lpSockAddr, nSockAddrLen);
	return nRet;
}


int CDGramSocket::SendTo(const LPVOID lpBuf, int nBufLen)
{
	char szIP[MAX_PATH] = {0};
	CString str(m_strTargetIP);
	CStringToPSZ(str, szIP);

	SOCKADDR_IN addr = {0};
	addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= inet_addr(szIP);
	addr.sin_port			= htons((USHORT)m_nTargetPort);

	int nRet = sendto(m_sockSender, (char*)lpBuf, nBufLen, 0, (SOCKADDR*)&addr, sizeof(addr));
	return nRet;
}


//------------------------------------------------------------------------
// �������ݽ����߳�
BOOL CDGramSocket::StartReceiveThread(void)
{
	if (m_sockReceiver == INVALID_SOCKET)
	{
		CreateReceiver(m_nLocalPort);
	}

	if (m_sockReceiver != INVALID_SOCKET)
	{
		if (m_bIsReceiving)
		{
			return TRUE;
		}

		m_hRecviveThread = AfxBeginThread((AFX_THREADPROC)ReceiveProc, (LPVOID)this);
		return (m_hRecviveThread != NULL);
	}

	return FALSE;
}

//------------------------------------------------------------------------
// ֹͣ���ݽ����߳�
void CDGramSocket::StopReceiveThread(void)
{
	if (m_bIsReceiving)
	{
		// ����Socket��ʹ���պ���ʧ�ܻ���������
		CloseReceiver();

		// �ȴ������߳���ȫ�˳�
		if (m_hRecviveThread)
		{
			if (WaitForSingleObject(m_hRecviveThread->m_hThread, 5000) == WAIT_TIMEOUT)
			{
				::TerminateThread(m_hRecviveThread->m_hThread, 1);
			}
			m_hRecviveThread = NULL;
		}
	}
}

//------------------------------------------------------------------------
// ���ݽ����߳�
UINT CDGramSocket::ReceiveProc(LPVOID lpParam)
{
	ASSERT(lpParam);
	CDGramSocket* ptr = (CDGramSocket*)lpParam;
	ptr->ReceiveLoop();
	return 0;
}


void CDGramSocket::ReceiveLoop(void)
{
	char szBuffer[1024] = {0};
	SOCKADDR_IN addrClient = {0};
	int nLenClient(sizeof(addrClient));
	int nRetLen(0);

	m_bIsReceiving = TRUE;
	while (m_bIsReceiving)
	{
		// �ȴ���������
		nRetLen = recvfrom(m_sockReceiver, szBuffer, 1024, 0, (SOCKADDR*)&addrClient, (int*)&nLenClient);
		if (nRetLen == SOCKET_ERROR || nRetLen == 0)
		{
			// ���Socket���ʹ������Socket�Ͽ���������ѭ��
			m_bIsReceiving = FALSE;
		}
		else
		{
			// TODO: �ڴ��������Ԥ�������
			// ���ӿ���
			// ��Զ�̶˷��ͷ�����Ϣ
		}
	}
}

//------------------------------------------------------------------------
// CStringת��Ϊchar*
int CDGramSocket::CStringToPSZ(CString str, char* ptr)
{
#ifdef _UNICODE
	int nLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, str, -1, ptr, nLen, NULL, NULL);
#endif
	return 0;
}

//------------------------------------------------------------------------
// CString���͵�IPת��ΪDWORD���͵�IP
DWORD CDGramSocket::CStringToDWordIP(CString strIP)
{
	char szIP[MAX_PATH] = {0};
	CStringToPSZ(strIP, szIP);
	return DWORD(inet_addr(szIP));
}

//------------------------------------------------------------------------
// DWORD���͵�IPת��ΪCString���͵�IP
CString CDGramSocket::DWordToCStringIP(DWORD dwIP)
{
	BYTE *p = (BYTE *)&dwIP;
	CString str(_T(""));
	str.Format(_T("%d.%d.%d.%d"), *p, *(p+1), *(p+2), *(p+3));
	return str;
}

//------------------------------------------------------------------------
//	��ȡ������ַ
void CDGramSocket::GetHostAddr(CString& strIP)
{
	// ��ȡ����������
	char chName[256] = {0x00};
	if (::gethostname(chName, 256))
	{
		strIP = _T("127.0.0.1");
		return;
	}

	// ��ȡ����������ַ��Ϣ
	HOSTENT* pHost(::gethostbyname(chName));
	if (!pHost)
	{
		strIP = _T("127.0.0.1");
		return;
	}

	try
	{
		if (pHost->h_addrtype == AF_INET)
		{
			// IPv4
			in_addr addr = {0};
			if ((u_long*)pHost->h_addr_list == NULL)
			{
				throw addr;
			}
			addr.S_un.S_addr = *(u_long*)pHost->h_addr_list[0];
			strIP = ::inet_ntoa(addr);
		}
		else
		{
			// IPv6
			strIP = _T("127.0.0.1");
			return;
		}
	}
	catch (...)
	{
		strIP = _T("127.0.0.1");
		return;
	}
}