#pragma once
#include "ControlCAN.h"
#pragma comment(lib, "ControlCAN.lib")

// CAN��ȡʧ�ܷ�����
#define CAN_ERROR		0xffffffff

class CCANCtrl
{
public:
	CCANCtrl(void);
	virtual ~CCANCtrl(void);

protected:
	DWORD	m_dwDeviceType;			// �豸���ͣ�Ĭ��ΪUSBCAN2��m_dwDeviceType = 4��
	DWORD	m_dwDeviceIndex;		// �豸������
	BOOL	m_bWork;				// �豸����״̬��ʶ��TRUE����������FALSE��δ����

	typedef enum tagBaudRate
	{
		BR_5 = 5, BR_10 = 10, BR_20 = 20, BR_40 = 40, BR_50 = 50, BR_80 = 80, 
		BR_100 = 100, BR_125 = 125, BR_200 = 200, BR_250 = 250, BR_400 = 400, 
		BR_500 = 500, BR_666 = 666, BR_800 = 800, BR_1000 = 1000
	};

public:
	//	���豸
	BOOL Open(DWORD dwDevType, DWORD dwDevIndex = 0);

	//	��ʼ��CAN
	BOOL Init(DWORD dwCANIndex, UINT nBaudRate);

	//	����CAN
	BOOL Start(DWORD dwCANIndex);

	//	��λCAN
	BOOL Reset(DWORD dwCANIndex);

	//	�ر��豸
	void Close(void);

	//	��ȡ�豸����״̬
	BOOL GetStatus(void);

	//	��ȡӲ����Ϣ
	BOOL GetDeviceInfo(PVCI_BOARD_INFO pInfo);

	// ��ȡ����
	UINT GetLastError(DWORD dwCANIndex);

	// ��������
	ULONG Send(DWORD dwCANIndex, PVCI_CAN_OBJ pSend, ULONG ulLen);

	// ��������
	ULONG Receive(DWORD dwCANIndex, PVCI_CAN_OBJ pReceive, ULONG ulLen, int nWaitTime = 400);

protected:
	void GetTimerValue(__in UINT nBaudrate, __out UCHAR * pTimer0, __out UCHAR * pTimer1);
};

