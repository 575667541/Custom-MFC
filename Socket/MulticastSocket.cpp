// MulticastSocket.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MulticastSocket.h"


// CMulticastSocket

//IMPLEMENT_DYNAMIC(CMulticastSocket, CWnd)

CMulticastSocket::CMulticastSocket()
	: m_sockRecv(INVALID_SOCKET)
	, m_sockSend(INVALID_SOCKET)
	, m_strGroupIP(_T(""))
	, m_uGroupPort(0)
	, m_strLocalIP(_T(""))
	, m_uLocalPort(0)
	, m_bRecv(FALSE)
	, m_hRecvThread(nullptr)
	, m_pchBuf(nullptr)
	, m_uBufLen(0)
{

}

CMulticastSocket::~CMulticastSocket()
{
	setsockopt(m_sockRecv, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char FAR*)&m_mreq, sizeof(IP_MREQ));

	closesocket(m_sockRecv);
	m_sockRecv = INVALID_SOCKET;

	closesocket(m_sockSend);
	m_sockSend = INVALID_SOCKET;

	if (m_hRecvThread)
	{
		m_bRecv = FALSE;
		if (::WaitForSingleObject(m_hRecvThread->m_hThread, 3000) == WAIT_TIMEOUT)
			::TerminateThread(m_hRecvThread->m_hThread, 1);
		m_hRecvThread = nullptr;
	}
}


// CMulticastSocket ��Ϣ�������


//	-----------------------------------------------------------------------------------
//	�����鲥IP
//	-----------------------------------------------------------------------------------
void CMulticastSocket::SetGroupIP(CString strGroupIP)
{
	m_strGroupIP = strGroupIP;
}

//	-----------------------------------------------------------------------------------
//	��ȡ�鲥IP
//	-----------------------------------------------------------------------------------
CString CMulticastSocket::GetGroupIP(void)
{
	return CString(m_strGroupIP);
}

//	-----------------------------------------------------------------------------------
//	�����鲥�˿�
//	-----------------------------------------------------------------------------------
void CMulticastSocket::SetGroupPort(UINT uGroupPort)
{
	m_uGroupPort = uGroupPort;
}

//	-----------------------------------------------------------------------------------
//	��ȡ�鲥�˿�
//	-----------------------------------------------------------------------------------
UINT CMulticastSocket::GetGroupPort(void)
{
	return UINT(m_uGroupPort);
}

//	-----------------------------------------------------------------------------------
//	�����鲥�飨��Ϊ�����ߣ�
//	-----------------------------------------------------------------------------------
BOOL CMulticastSocket::JoinGroup(CString strGroupIP, UINT uGroupPort, UINT uTTL, BOOL bLookback)
{
	m_strGroupIP = strGroupIP;
	m_uGroupPort = uGroupPort;

	BOOL bRet = CreateRecvSocket(m_strGroupIP, m_uGroupPort);
	if (!bRet)
	{
		return FALSE;
	}

	bRet = CreateSendSocket(uTTL, bLookback);
	if (!bRet)
	{
		return FALSE;
	}

	return TRUE;
}

//	-----------------------------------------------------------------------------------
//	�����鲥�飨��Ϊ�����ߣ�
//	-----------------------------------------------------------------------------------
BOOL CMulticastSocket::JoinGroup(CString strGroupIP, UINT uGroupPort)
{
	m_strGroupIP = strGroupIP;
	m_uGroupPort = uGroupPort;

	BOOL bRet = CreateRecvSocket(m_strGroupIP, m_uGroupPort);
	if (!bRet)
	{
		return FALSE;
	}

	bRet = CreateSendSocket(15, FALSE);
	if (!bRet)
	{
		return FALSE;
	}

	return TRUE;
}

//	-----------------------------------------------------------------------------------
//	�뿪�鲥��
//	-----------------------------------------------------------------------------------
BOOL CMulticastSocket::LeaveGroup(void)
{
	Close();

	if (m_hRecvThread)
	{
		m_bRecv = FALSE;
		if (::WaitForSingleObject(m_hRecvThread->m_hThread, 3000) == WAIT_TIMEOUT)
			::TerminateThread(m_hRecvThread->m_hThread, 1);
		m_hRecvThread = nullptr;
	}

	return TRUE;
}

//	-----------------------------------------------------------------------------------
//	���������׽���
//	-----------------------------------------------------------------------------------
BOOL CMulticastSocket::CreateSendSocket(UINT uTTL, BOOL bLookback)
{
	m_sockSend = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_sockSend == INVALID_SOCKET)
	{
		return FALSE;
	}

#ifdef SOCK_BUF_SIZE
	int nVal(SOCK_BUF_SIZE);
	int nRet = setsockopt(m_sockSend, SOL_SOCKET, SO_SNDBUF, (char FAR*)&nVal, sizeof(int));
	if (nRet)
	{
		return FALSE;
	}
#endif

	if (SetTTL(uTTL) == FALSE)
	{
		AfxMessageBox(_T("Set TTL Failure!"));
	}

	SetLookBack(bLookback);

	return TRUE;
}

//	-----------------------------------------------------------------------------------
//	���������׽���
//	-----------------------------------------------------------------------------------
BOOL CMulticastSocket::CreateRecvSocket(CString strGroupIP, UINT uGroupIP)
{
	m_sockRecv = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_sockRecv == INVALID_SOCKET)
	{
		return FALSE;
	}

	BOOL bMultipleApps(TRUE);
	int nRet = setsockopt(m_sockRecv, SOL_SOCKET, SO_REUSEADDR, (char FAR*)&bMultipleApps, sizeof(BOOL));
	if (nRet)
	{
		return FALSE;
	}

#ifdef SOCK_BUF_SIZE
	int nVal(SOCK_BUF_SIZE);
	nRet = setsockopt(m_sockRecv, SOL_SOCKET, SO_RCVBUF, (char FAR*)&nVal, sizeof(int));
	if (nRet)
	{
		return FALSE;
	}
#endif

	char chIP[256] = {0};
	CStringToPSZ(strGroupIP, chIP);

	SOCKADDR_IN addr;
	SecureZeroMemory(&addr, sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons((USHORT)uGroupIP);

	nRet = bind(m_sockRecv, (LPSOCKADDR)&addr, sizeof(SOCKADDR_IN));
	if (nRet)
	{
		TRACE(_T("err %d"), WSAGetLastError());
		return FALSE;
	}

	m_mreq.imr_multiaddr.s_addr = inet_addr(chIP);
	m_mreq.imr_interface.s_addr = htons(INADDR_ANY);
	nRet = setsockopt(m_sockRecv, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char FAR*) &m_mreq, sizeof(IP_MREQ));

	return (nRet == 0);
}

//	-----------------------------------------------------------------------------------
//	���û���
//	-----------------------------------------------------------------------------------
void CMulticastSocket::SetLookBack(BOOL bLoop)
{
	if (setsockopt(m_sockSend, IPPROTO_IP, IP_MULTICAST_LOOP, (char FAR*)&bLoop, sizeof(BOOL)))
	{
		TRACE(_T("error: set loopback failure!\n"));
		return;
	}

	if (!bLoop)
	{
		char chLocalHost[256] = {0};
		gethostname(chLocalHost, 256);
		HOSTENT* pHost(gethostbyname(chLocalHost));
		if (!pHost)
		{
			m_strLocalIP = _T("127.0.0.1");
			return;
		}

		try
		{
			in_addr addr;
			addr.S_un.S_addr = *(ULONG*)pHost->h_addr_list[0];
			m_strLocalIP = inet_ntoa(addr);
//			CString strDummy(m_strLocalIP);
//			if (!m_socketSend.GetSockName(strDummy, m_uLocalPort))
//				m_uLocalPort = 64360;
		}
		catch (...)
		{
			m_strLocalIP = _T("127.0.0.1");
			m_uLocalPort = 64360;
			return;
		}	
	}
}

//	-----------------------------------------------------------------------------------
//	����TTL��ֵ
//	-----------------------------------------------------------------------------------
BOOL CMulticastSocket::SetTTL(UINT uTTL)
{
	int nRet = setsockopt(m_sockSend, IPPROTO_IP, IP_MULTICAST_TTL, (char FAR*)&uTTL, sizeof(UINT));
	
	return (nRet == 0);
}

//	-----------------------------------------------------------------------------------
//	�����鲥��Ϣ
//	-----------------------------------------------------------------------------------
int CMulticastSocket::SendTo(const void* pBuf, int nBufLen, UINT uPort)
{
	char chIP[256] = {0};
	CStringToPSZ(m_strGroupIP, chIP);

	SOCKADDR_IN addr;
	SecureZeroMemory(&addr, sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(chIP);
	addr.sin_port = htons((USHORT)uPort);

	int nRet = sendto(m_sockSend, (char*)pBuf, nBufLen, 0, (LPSOCKADDR)&addr, sizeof(SOCKADDR_IN));
//	TRACE(_T("err %d\n"), WSAGetLastError());
	return nRet;
}

//	-----------------------------------------------------------------------------------
//	������Ϣ
//	-----------------------------------------------------------------------------------
int CMulticastSocket::SendTo(const void* pBuf, int nBufLen, UINT uHostPort, LPCTSTR lpszHostAddress)
{
	char chIP[256] = {0};
	CString strIP(lpszHostAddress);
	CStringToPSZ(strIP, chIP);

	SOCKADDR_IN addr;
	SecureZeroMemory(&addr, sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(chIP);
	addr.sin_port = htons((USHORT)uHostPort);

	int nRet = sendto(m_sockSend, (char*)pBuf, nBufLen, 0, (LPSOCKADDR)&addr, sizeof(SOCKADDR_IN));
//	TRACE(_T("err %d\n"), WSAGetLastError());
	return nRet;
}

//	-----------------------------------------------------------------------------------
//	�����鲥��Ϣ
//	-----------------------------------------------------------------------------------
int CMulticastSocket::RecvFrom(LPVOID lpBuf, int nBufLen)
{
	SOCKADDR_IN addrClient;
	int nClientLen(sizeof(SOCKADDR_IN));

	int nRet = recvfrom(m_sockRecv, (char*)lpBuf, nBufLen, 0, (LPSOCKADDR)&addrClient, (int*)&nClientLen);
// 	TRACE(_T("err %d\n"), WSAGetLastError());
// 	ASSERT(nRet > 0);
	return nRet;
}

//	-----------------------------------------------------------------------------------
//	������Ϣ
//	-----------------------------------------------------------------------------------
int CMulticastSocket::RecvFrom(LPVOID lpBuf, int nBufLen, CString& rSocketAddress, UINT& rSocketPort)
{
	SOCKADDR_IN addrClient;
	int nClientLen(sizeof(SOCKADDR_IN));

	int nRet = recvfrom(m_sockRecv, (char*)lpBuf, nBufLen, 0, (LPSOCKADDR)&addrClient, (int*)&nClientLen);

	try
	{
		char* pchAddr(inet_ntoa(addrClient.sin_addr));
		rSocketAddress = pchAddr;
		rSocketPort = ntohs(addrClient.sin_port);
	}
	catch (...)
	{
		rSocketAddress = _T("");
		rSocketPort = 0;
	}

	return nRet;
}

//	-----------------------------------------------------------------------------------
//	��ʼ����
//	-----------------------------------------------------------------------------------
BOOL CMulticastSocket::StartRecv(LPVOID pBuf, UINT uBufLen)
{
	if (m_sockRecv != INVALID_SOCKET)
	{
		if (m_bRecv)
		{
			return TRUE;
		}

		m_pchBuf = (char*)pBuf;
		m_uBufLen = uBufLen;
		m_hRecvThread = AfxBeginThread(RecvProc, (LPVOID)this);
		return (m_hRecvThread != nullptr);
	}

	return FALSE;
}

//	-----------------------------------------------------------------------------------
//	ֹͣ����
//	-----------------------------------------------------------------------------------
void CMulticastSocket::StopRecv(void)
{
	closesocket(m_sockRecv);
	m_sockRecv = INVALID_SOCKET;

	if (m_hRecvThread)
	{
		m_bRecv = FALSE;
		if (::WaitForSingleObject(m_hRecvThread->m_hThread, 3000) == WAIT_TIMEOUT)
			::TerminateThread(m_hRecvThread->m_hThread, 1);
		m_hRecvThread = nullptr;
	}
}

//	-----------------------------------------------------------------------------------
//	������Ϣ�߳�
//	-----------------------------------------------------------------------------------
UINT CMulticastSocket::RecvProc(LPVOID lpParam)
{
	ASSERT(lpParam);
	CMulticastSocket* p((CMulticastSocket*)lpParam);
	p->RecvLoop();

	return 0;
}

//	-----------------------------------------------------------------------------------
//	������Ϣ����
//	-----------------------------------------------------------------------------------
void CMulticastSocket::RecvLoop(void)
{
	SOCKADDR_IN addrClient;
	int nClientLen(sizeof(SOCKADDR_IN));
	
	m_bRecv = TRUE;
	for (int nRecvLen(0); m_bRecv; )
	{
		nRecvLen = recvfrom(m_sockRecv, m_pchBuf, m_uBufLen, 0, (LPSOCKADDR)&addrClient, (int*)&nClientLen);
		switch (nRecvLen){
		case SOCKET_ERROR:
		case 0:
			m_bRecv = FALSE;
			break;
		default:
			{
				// TODO: �ڴ��������Ԥ�������
//				m_file.Write(m_pchBuf, lRecvLen);
			}
		}
		::Sleep(5);
	}
}

//	-----------------------------------------------------------------------------------
//	�����׽���
//	-----------------------------------------------------------------------------------
BOOL CMulticastSocket::Create(CString strGroupIP, UINT uGroupPort, UINT uTTL, BOOL bLoopback)
{
	m_strGroupIP = strGroupIP;
	m_uGroupPort = uGroupPort;

	BOOL bRet = CreateRecvSocket(m_strGroupIP, m_uGroupPort);
	if (!bRet)
	{
		return FALSE;
	}

	bRet = CreateSendSocket(uTTL, bLoopback);
	if (!bRet)
	{
		return FALSE;
	}

	return TRUE;
}

//	-----------------------------------------------------------------------------------
//	�ر��׽���
//	-----------------------------------------------------------------------------------
void CMulticastSocket::Close(void)
{
	int nRet = setsockopt(m_sockRecv, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char FAR*)&m_mreq, sizeof(IP_MREQ));
	if (nRet)
	{
		return;
	}

	closesocket(m_sockRecv);
	m_sockRecv = INVALID_SOCKET;

	closesocket(m_sockSend);
	m_sockSend = INVALID_SOCKET;
}

//	-----------------------------------------------------------------------------------
// CStringת��ΪPSZ
//	-----------------------------------------------------------------------------------
int CMulticastSocket::CStringToPSZ(CString str, char* ptr)
{
#ifdef _UNICODE
	int nLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, str, -1, ptr, nLen, NULL, NULL);
#endif
	return 0;
}
