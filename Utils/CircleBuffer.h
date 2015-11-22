#pragma once
#include "afxmt.h"

// ��д��������
//#define WRITE_TEST		1


//////////////////////////////////////////////////////////////////////////
//�Զ���
class CAutoLock
{
public:
	CAutoLock(CCriticalSection& lock)
		: m_cs(lock)
	{
		m_cs.Lock();
	}

	virtual ~CAutoLock()
	{
		m_cs.Unlock();
	}

private:
	CCriticalSection&	m_cs;
};


//////////////////////////////////////////////////////////////////////////
// ���λ�����
class CCircleBuffer
{
public:
	CCircleBuffer(int nBufferSize);
	virtual ~CCircleBuffer(void);

private:
	CCriticalSection	m_cs;			// ������
	int					m_nBufferSize;	// ����������
	int					m_nReadPos;		// ��λ��
	int					m_nWritePos;	// дλ��
	BYTE				*m_pBuffer;		// �������׵�ַָ��

public:
	int Write(LPVOID lpData, int nLen);
	int Read(LPVOID lpData, int nLen);

	BOOL IsFull(void);
	BOOL IsEmpty(void);

private:
	int GetDataLength(void) const;
	int GetFreeLength(void) const;

#ifdef WRITE_TEST
	CFile				m_file;			// д�����ʱ�����ڱ���д�������
#endif
};

