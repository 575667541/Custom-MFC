// CDGramSocket.h : ͷ�ļ�
//


#pragma once
#include "afxwin.h"



class CDGramSocket
{
public:
	CDGramSocket(void);
	virtual ~CDGramSocket(void);

private:
	SOCKET		m_sockReceiver;		// ���ڽ��յ�Socket
	SOCKET		m_sockSender;		// ���ڷ��͵�Socket
	CString		m_strTargetIP;		// Զ�̶�IP
	UINT		m_nTargetPort;		// Զ�̶˿ں�
	UINT		m_nLocalPort;		// ���ض˿ں�
	BOOL		m_bIsReceiving;		// ���ڽ������ݱ��
	CWinThread* m_hRecviveThread;	// �����߳̾��

public:
	// ����/��ȡԶ�̶�IP
	void SetTargetIP(CString strTargetIP);
	CString GetTargetIP(void);

	// ����/��ȡԶ�̶˿ں�
	void SetTargetPort(UINT nTargetPort);
	UINT GetTargetPort(void);
	
	// ����/��ȡ���ض˿ں�
	void SetLocalPort(UINT nLocalPort);
	UINT GetLocalPort(void);
	
	// ����/�ر����ڽ��յ�Socket
	BOOL CreateReceiver(UINT nLocalPort, LPCTSTR lpszSocketAddress = NULL);
	void CloseReceiver(void);
	
	// ����/�ر����ڷ��͵�Socket
	BOOL CreateSender(UINT nTargetPort = 0, LPCTSTR lpszTargetAddress = NULL);
	void CloseSender(void);

	// ��������
	int ReceiveFrom(LPVOID lpBuf, int nBufLen, CString& rSocketAddress, UINT& rSocketPort, int nFlags = 0);
	int ReceiveFrom(LPVOID lpBuf, int nBufLen, SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags = 0);
	int	ReceiveFrom(LPVOID lpBuf, int nBufLen);

	// ��������
	int SendTo(const LPVOID lpBuf, int nBufLen, UINT nTargetPort, LPCTSTR lpszTargetAddress, int nFlags = 0);
	int SendTo(const LPVOID lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags = 0);
	int SendTo(const LPVOID lpBuf, int nBufLen);

	// ����/ֹͣ���ݽ����߳�
	BOOL StartReceiveThread(void);
	void StopReceiveThread(void);

protected:
	// ���ݽ����߳�
	static UINT ReceiveProc(LPVOID lpParam);
	void ReceiveLoop(void);	

	// CStringת��Ϊchar*
	int CStringToPSZ(CString str, char* ptr);

public:
	// CString���͵�IPת��ΪDWORD���͵�IP
	DWORD CStringToDWordIP(CString strIP);

	// DWORD���͵�IPת��ΪCString���͵�IP
	CString DWordToCStringIP(DWORD dwIP);

	// ��ȡ������ַ
	void GetHostAddr(CString& strIP);
};

