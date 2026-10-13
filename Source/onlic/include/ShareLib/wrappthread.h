#ifndef ___WRAPPTHREAD_H___
#define ___WRAPPTHREAD_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

#ifndef _Max            /* List of required header files */
#define _Max(a,b)    (((a) > (b)) ? (a) : (b))
#endif

#define sip_timeval			void

typedef struct{
	pthread_t	pid;		
	int			nStCh;
	int			nChCnt;
	int			nThIndex;
} THRD_INFO;


void Pthread_create(pthread_t *tid, void * (*func)(void *), void *arg);
void Pthread_kill(pthread_t *tid, int signo);

void Pthread_mutex_init(pthread_mutex_t *__mutex, pthread_mutexattr_t *__mutex_attr);
void Pthread_mutex_lock(pthread_mutex_t *__mutex);
void Pthread_mutex_unlock(pthread_mutex_t *__mutex);

void Pthread_cond_init(pthread_cond_t *__cond, pthread_condattr_t *__cond_attr);
void Pthread_cond_signal(pthread_cond_t *__cond);
int Pthread_cond_wait(pthread_cond_t* __cond, pthread_mutex_t* __mutex);
int Pthread_cond_waittime(pthread_cond_t *__cond, pthread_mutex_t* __mutex, timespec *tsptr);
void Pthread_cond_broadcast (pthread_cond_t *cv);


#endif	/* wrappthread.h */
