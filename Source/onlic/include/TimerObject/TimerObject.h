#ifndef ___TIMER_OBJECT_H___
#define ___TIMER_OBJECT_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

#include <shareheader.h>
#include <ListObject/ListObject.h>
#include <ShareLib/ShareLib.h>

//	timer_type
#define		T_TYPE_SET			0
#define		T_TYPE_KILL			1
#define		T_NUM_KILL			2
#define		T_TYPE_END			3


typedef struct{
	UINT	nTimerId;
	char    szNumber[256];
	int		timer_type;
	long	lPeriod;
	int64_t	dwCurTime;
} TIMER_STRUCT;

typedef std::vector<TIMER_STRUCT*> TIMER_VECTOR;
typedef std::vector<TIMER_STRUCT*>::iterator TIMER_ITER;

class CTimerObject
{
// attr
public:
	CTimerObject();
	~CTimerObject();


	void* m_pEventTimer;

	int timer_thread_start;
	pthread_t timer_pid;
// Operations
public:

	void TimerObjectStart(char *szTimerName, void* pTimer);
	int TimerWaitStruct(DWORD nWaitTime, TIMER_STRUCT *pTData);
	int TimerWaitGetMsg(TIMER_STRUCT *pTData);
	int TimerSetListProc(TIMER_STRUCT *pTData);
	void TimerListAddTail(TIMER_STRUCT *pTData);
	void TimerListRemove(TIMER_STRUCT *pTData);
	void TimerListNumRemove(TIMER_STRUCT *pTData);
	void TimerListRemoveAll();
	int RunTimerProc();
	void TimerObjectPush(TIMER_STRUCT *pTData);
	void TimerObjectKill();

	void TimerSleep(DWORD dwTime);
	void Timer_Pthread_create(pthread_t *tid, void * (*func)(void *), void *arg);
	void Timer_Pthread_cond_init(pthread_cond_t *__cond, pthread_condattr_t *__cond_attr);
	void Timer_Pthread_mutex_init(pthread_mutex_t *__mutex, pthread_mutexattr_t *__mutex_attr);
	void Timer_Pthread_mutex_lock(pthread_mutex_t *__mutex);
	void Timer_Pthread_mutex_unlock(pthread_mutex_t *__mutex);
	void Timer_Pthread_cond_signal(pthread_cond_t* __cond);
	int Timer_Pthread_cond_wait(pthread_cond_t* __cond, pthread_mutex_t* __mutex);
	int Timer_Pthread_cond_waittime(pthread_cond_t* __cond, pthread_mutex_t* __mutex, timespec *tsptr);
	int Timer_printf(char *buf, size_t size, const char *fmt, ...);
	int Timer_check_printf(char *buf, size_t size, const char *fmt, ...);
	void err_Timer_end(const char *fmt, ...);
	void err_Timer_doit(int errnoflag, int level, const char *fmt, va_list ap);

	TIMER_VECTOR *m_timer_event;
	CListObject timer_list_object;
	void (*OnCallBackFunc)(UINT, UINT, char*, void*, int64_t delay);

	char __timer_name[51];

protected:
	void TimerObjectInit(char *szTimerName);
	int64_t TimerLocalTimeProc();

	pthread_cond_t timer_cond;
	pthread_mutex_t timer_mutex;
	pthread_condattr_t *timer_cond_attr;
	pthread_mutexattr_t *timer_mutex_attr;
	int timer_action;
	int timer_count;
	int __timer_id;
	int timer_thread_init;
};


#endif//TimerObject.h


