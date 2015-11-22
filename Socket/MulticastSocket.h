#pragma once
#include "afxwin.h"


#define SOCK_BUF_SIZE	(16*1024)
// CMulticastSocket

class CMulticastSocket
{
//	DECLARE_DYNAMIC(CMulticastSocket)

public:
	CMulticastSocket();
	virtual ~CMulticastSocket();

protected:
	SOCKET		m_sockRecv;
	SOCKET		m_sockSend;

	CString		m_strGroupIP;
	UINT		m_uGroupPort;

	CString		m_strLocalIP;
	UINT		m_uLocalPort;

	IP_MREQ		m_mreq;
	
	BOOL		m_bRecv;
	CWinThread*	m_hRecvThread;

	char*		m_pchBuf;
	UINT		m_uBufLen;

public:
	// ===========================================================================================
	// �����鲥IP
	// ===========================================================================================
	void SetGroupIP(CString strGroupIP);
	CString GetGroupIP(void);

	// ===========================================================================================
	// �����鲥�˿�
	// ===========================================================================================
	void SetGroupPort(UINT uGroupPort);
	UINT GetGroupPort(void);

	// ===========================================================================================
	// ������뿪�鲥��
	// ===========================================================================================
	BOOL JoinGroup(CString strGroupIP, UINT uGroupPort, UINT uTTL, BOOL bLookback);
	BOOL JoinGroup(CString strGroupIP, UINT uGroupPort);
	BOOL LeaveGroup(void);

	// ===========================================================================================
	// �������ͻ����socket
	// ===========================================================================================
	BOOL CreateSendSocket(UINT uTTL = 15, BOOL bLookback = 0);
	BOOL CreateRecvSocket(CString strGroupIP, UINT uGroupIP);

	// ===========================================================================================
	// ���û���������ʱ�����
	// ===========================================================================================
	void SetLookBack(BOOL bLoop = 0);
	BOOL SetTTL(UINT uTTL = 15);

	// ===========================================================================================
	// ���ͺ���
	// ===========================================================================================
	int SendTo(const void* pBuf, int nBufLen, UINT uPort);
	int SendTo(const void* pBuf, int nBufLen, UINT uHostPort, LPCTSTR lpszHostAddress);

	// ===========================================================================================
	// ���պ���
	// ===========================================================================================
	int RecvFrom(LPVOID lpBuf, int nBufLen);
	int RecvFrom(LPVOID lpBuf, int nBufLen, CString& rSocketAddress, UINT& rSocketPort);

	// ===========================================================================================
	// �ڲ����տ��ƺ���
	// ===========================================================================================
	BOOL StartRecv(LPVOID pBuf, UINT uBufLen);
	void StopRecv(void);

	// ===========================================================================================
	// ���ݽӿں���
	// ===========================================================================================
	BOOL Create(CString strGroupIP, UINT uGroupPort, UINT uTTL = 15, BOOL bLoopback = 0);
	void Close(void);

protected:
	// ===========================================================================================
	// �ڲ������߳�
	// ===========================================================================================
	static UINT RecvProc(LPVOID lpParam);
	void RecvLoop(void);

	// ===========================================================================================
	// CStringת��ΪPSZ
	// ===========================================================================================
	int CStringToPSZ(CString str, char* ptr);	
};


