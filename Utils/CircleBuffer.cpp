#include "StdAfx.h"
#include "CircleBuffer.h"


CCircleBuffer::CCircleBuffer(int nBufferSize)
	: m_nBufferSize(nBufferSize)
	, m_nReadPos(0)
	, m_nWritePos(m_nReadPos)
	, m_pBuffer(NULL)
{
	m_pBuffer = new BYTE[m_nBufferSize+1]();
#ifdef WRITE_TEST
	m_file.Open(_T("~circle_buffer_write_test.dat"), CFile::modeCreate | CFile::modeWrite);
#endif
}


CCircleBuffer::~CCircleBuffer(void)
{
	if (m_pBuffer != NULL)
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	}
#ifdef WRITE_TEST
	m_file.Close();
#endif
}

//////////////////////////////////////////////////////////////////////////
// д������
int CCircleBuffer::Write(LPVOID lpData, int nLen)
{
	CAutoLock lock(m_cs);

	// ���������ָ�벻��ȷ
	if (lpData == NULL)
	{
		return 0;
	}

	// ��������ݳ��Ȳ���ȷ������0
	if (nLen <= 0)
	{
		return 0;
	}

	// û�п��пռ�ʱ������0
	int nFreeLen = GetFreeLength() - 1;
	if (nFreeLen <= 0)
	{
		return 0;
	}

	// ȷ��ʵ�ʿ�д������ݳ���
	int nValidDataLen = min(nFreeLen, nLen);

	if (m_nReadPos > m_nWritePos)
	{
		// ��дλ���ڶ�λ�ú���ʱ�����ûػ�
		memcpy_s(m_pBuffer+m_nWritePos, nFreeLen, (BYTE *)lpData, nValidDataLen);
#ifdef WRITE_TEST
		m_file.Write(m_pBuffer+m_nWritePos, nValidDataLen);
#endif
		// дλ����β���ƶ�
		m_nWritePos += nValidDataLen;	
	}
	else
	{
		// ��дλ���ڶ�λ��ǰ��ʱ��������Ҫ�ػ�
		// ȷ������β��ʵ�ʿ�д������ݳ���
		int nTailLen = m_nBufferSize - m_nWritePos;
		int nTailWriteLen = min(nValidDataLen, nTailLen);

		memcpy_s(m_pBuffer+m_nWritePos, nTailLen, (BYTE *)lpData, nTailWriteLen);
#ifdef WRITE_TEST
		m_file.Write(m_pBuffer+m_nWritePos, nTailWriteLen);
#endif
		m_nWritePos = (m_nWritePos+nValidDataLen) % m_nBufferSize;
		memcpy_s(m_pBuffer, nFreeLen-nTailWriteLen, (BYTE *)lpData+nTailWriteLen, nValidDataLen-nTailWriteLen);
#ifdef WRITE_TEST
		m_file.Write(m_pBuffer, nValidDataLen-nTailWriteLen);
#endif
	}

	return nValidDataLen;
}

//////////////////////////////////////////////////////////////////////////
// ��ȡ����
int CCircleBuffer::Read(LPVOID lpData, int nLen)
{
	CAutoLock lock(m_cs);

	// ���������ָ�벻��ȷ
	if (lpData == NULL)
	{
		return 0;
	}

	// ��������ݳ��Ȳ���ȷ������0
	if (nLen <= 0)
	{
		return 0;
	}

	// �����ѿ�ʱ������0
	int nDataLen = GetDataLength();
	if (nDataLen <= 0)
	{
		return 0;
	}

	// ȷ��ʵ�ʿɶ�ȡ�����ݳ���
	int nValidDataLen = min(nDataLen, nLen);

	if (m_nWritePos > m_nReadPos)
	{
		// ��дλ���ڶ�λ��ǰ��ʱ�����ûػ�
		memcpy_s((BYTE *)lpData, nLen, m_pBuffer+m_nReadPos, nValidDataLen);

		// ��λ����β���ƶ�
		m_nReadPos += nValidDataLen;
	} 
	else
	{
		// ��дλ���ڶ�λ�ú���ʱ����Ҫ�ػ�
		// ȷ��������β��ʵ�ʿɶ�ȡ�����ݳ���
		int nTailLen = m_nBufferSize - m_nReadPos;
		int nTailReadLen = min(nTailLen, nValidDataLen);

		memcpy_s((BYTE *)lpData, nLen, m_pBuffer+m_nReadPos, nTailReadLen);
		m_nReadPos = (m_nReadPos+nValidDataLen) % m_nBufferSize;
		memcpy_s((BYTE *)lpData+nTailLen, nLen-nTailReadLen, m_pBuffer, nValidDataLen-nTailReadLen);
	}

	return nValidDataLen;
}

//////////////////////////////////////////////////////////////////////////
// �Ƿ񻺳�������
BOOL CCircleBuffer::IsFull(void)
{
	CAutoLock lock(m_cs);

	return BOOL(GetFreeLength() == 1);
}

//////////////////////////////////////////////////////////////////////////
// �Ƿ񻺳���Ϊ��
BOOL CCircleBuffer::IsEmpty(void)
{
	CAutoLock lock(m_cs);

	return BOOL(m_nWritePos == m_nReadPos);
}

//////////////////////////////////////////////////////////////////////////
// ��ȡ���ݳ���
int CCircleBuffer::GetDataLength(void) const
{
	int nLen = m_nWritePos - m_nReadPos;
	if (nLen < 0)
	{
		// ���д�����Ѿ��ػ�����������δ�ػ�
		nLen = m_nBufferSize + nLen;
	}

	return nLen;
}

//////////////////////////////////////////////////////////////////////////
// ��ȡ���пռ䳤��
int CCircleBuffer::GetFreeLength(void) const
{
	int nLen  = m_nReadPos - m_nWritePos;
	if (nLen <= 0)
	{
		// �����д����û�лػ�
		nLen = m_nBufferSize + nLen;
	}

	return nLen;
}
