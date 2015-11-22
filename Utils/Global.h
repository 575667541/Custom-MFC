// �߳��˳��ȴ�ʱ��
#define WAIT_THREAD_EXIT	10000


// ��ȫɾ������
#define SAFE_DELETE_BUFFER(x) {if (x) { delete [] (x); (x)=NULL;}}
#define SAFE_DELETE_ARRAY(x)  { if(x) { delete [] (x); (x)=NULL;}}

//////////////////////////////////////////////////////////////////////////
// ��ȫɾ������
#define SAFE_DELETE_OBJECT(x) {if (x) {delete (x);(x)=NULL;}}
#define SAFE_DELETE(x) { if (x)	{delete (x); (x) = NULL;}}


//��ȫ�ͷ�COM�ӿ�
#define SAFE_RELEASE(x) { if (x) {(x)->Release(); (x) = NULL; }}


//��ȫ�رն���
#define SAFE_CLOSE(x) { if (x) {x->Close(); delete x; x = NULL;}}


//��ȫ�˳��߳�
#define SAFE_EXIT_THREAD(x) { \
	if (x) \
	{ \
		PostThreadMessage(x->m_nThreadID, PWM_EXIT, 0, 0); \
		if (::WaitForSingleObject(x->m_hThread, WAIT_THREAD_EXIT) == WAIT_TIMEOUT) \
		{ \
			::TerminateThread(x->m_hThread, 1); \
		} \
	} \
	x = NULL; \
}


//��ȫ�˳��߳�
#define SAFE_QUIT_THREAD(x, time) { \
	if (x) \
	{ \
		if (::WaitForSingleObject(x->m_hThread, time) == WAIT_TIMEOUT) \
		{ \
			::TerminateThread(x->m_hThread, 1); \
		} \
	} \
	x = NULL; \
}