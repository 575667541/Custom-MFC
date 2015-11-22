#pragma once

//δ��������DhcpNotifyConfigChangeλ�� dhcpcsvc.dll��.  
//ԭ������ 
typedef BOOL ( CALLBACK* DHCPNOTIFYPROC) ( 
	LPWSTR lpwszServerName,			// ���ػ���ΪNULL 
	LPWSTR lpwszAdapterName,		// ���������� 
	BOOL bNewIpAddress,				// TRUE��ʾ����IP 
	DWORD dwIpIndex,				// ָ���ڼ���IP��ַ�����ֻ�иýӿ�ֻ��һ��IP��ַ��Ϊ0 
	DWORD dwIpAddress,				// IP��ַ 
	DWORD dwSubNetMask,				// �������� 
	int nDhcpAction					// ��DHCP�Ĳ��� 0:���޸�, 1:���� DHCP��2:���� DHCP 
	);  

class CIPConfig
{
public:
	CIPConfig(void);
	virtual ~CIPConfig(void);

	// ��ȡע���ȡ������������
	void GetLanAdapterName(char * pszAdapterName);

	// ע��IP��ַ
	BOOL RegisterIPAddress(LPCSTR pszAdapterName, LPCSTR pszIPAddress, LPCSTR pszNetMask, LPCSTR pszGateway = NULL, LPCSTR pszDNSServer1 = NULL, LPCSTR pszDNSServer2 = NULL);
	
	BOOL NotifyIPChange(LPCSTR pszAdapterName, int nIndex, LPCSTR pszIPAddress, LPCSTR pszNetMask);
};

