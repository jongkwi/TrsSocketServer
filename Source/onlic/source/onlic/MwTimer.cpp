/*
**	MwTimer.cpp : CTimerObject 래퍼 + 타이머 콜백 분기 (onmw MwTimer.cpp 패턴)
**	Windows: CTrsSocketServerDlg::OnTimer (LINK_TIMER 5초) → OnCheckLinkData
*/
#include "onlicDefine.h"

CMwTimer::CMwTimer()
{
	pTimer = NULL;
	__timer_run = FALSE;
	memset_st(m_szTimerName, 0, sizeof(m_szTimerName));
}

CMwTimer::~CMwTimer()
{
	EndUserTimer();
}

void CMwTimer::MwTimerInit(const char* szTimerName)
{
	__timer_run = FALSE;
	/* onmw 는 Malloc 으로 생성자 미실행 (결함) → new 사용 */
	pTimer = new CTimerObject();
	pTimer->OnCallBackFunc = OnMwTimerEvent;
	strcpy_st(m_szTimerName, szTimerName);
	pTimer->TimerObjectStart(m_szTimerName, this);
	__timer_run = TRUE;
}

void CMwTimer::SetUserTimer(UINT nTimerId, long lPeriod)
{
	TIMER_STRUCT tSt;
	if(__timer_run != TRUE) return;
	memset_st(&tSt, 0, sizeof(TIMER_STRUCT));
	tSt.nTimerId = nTimerId;
	tSt.timer_type = T_TYPE_SET;
	tSt.lPeriod = lPeriod;
	tSt.dwCurTime = GetMilliSencondProc();
	pTimer->TimerObjectPush(&tSt);
}

void CMwTimer::SetUserTimer(UINT nTimerId, long lPeriod, char *szNumber)
{
	TIMER_STRUCT tSt;
	if(__timer_run != TRUE) return;
	memset_st(&tSt, 0, sizeof(TIMER_STRUCT));
	tSt.nTimerId = nTimerId;
	tSt.timer_type = T_TYPE_SET;
	tSt.lPeriod = lPeriod;
	strcpy_st(tSt.szNumber, szNumber);
	tSt.dwCurTime = GetMilliSencondProc();
	pTimer->TimerObjectPush(&tSt);
}

void CMwTimer::KillUserTimer(UINT nTimerId)
{
	TIMER_STRUCT tSt;
	if(__timer_run != TRUE) return;
	memset_st(&tSt, 0, sizeof(TIMER_STRUCT));
	tSt.nTimerId = nTimerId;
	tSt.timer_type = T_TYPE_KILL;
	pTimer->TimerObjectPush(&tSt);
}

void CMwTimer::KillUserNumberTimer(UINT nTimerId, char *szNumber)
{
	TIMER_STRUCT tSt;
	if(__timer_run != TRUE) return;
	memset_st(&tSt, 0, sizeof(TIMER_STRUCT));
	tSt.nTimerId = nTimerId;
	tSt.timer_type = T_NUM_KILL;
	strcpy_st(tSt.szNumber, szNumber);
	pTimer->TimerObjectPush(&tSt);
}

void CMwTimer::KillUserAllTimer()
{
	if(__timer_run != TRUE) return;
	pTimer->TimerListRemoveAll();
}

void CMwTimer::EndUserTimer()
{
	pthread_t tid = 0;
	int i = 0;
	__timer_run = FALSE;
	if(pTimer != NULL){
		/* TimerObjectKill 은 1초만 기다린다. LinkCheck_OnTimer 가 DB 호출 중(최대 로그인 30초 + 문장 10초)이면
		** 스레드가 살아 있는 채 delete 되므로, timer_pid 가 0 이 될 때까지 기다려 join 한다 (L5). 안 끝나면 delete 하지 않는다 */
		tid = pTimer->timer_pid;
		pTimer->TimerObjectKill();
		for(i = 0; i < 450; i++){ if(pTimer->timer_pid == 0) break; WaitMSleep(100); }
		if(pTimer->timer_pid != 0){
			OnlicLogf(LOG_SOFT, "timer thread [%s] did not exit in 45s - object left for process exit", m_szTimerName);
			pTimer = NULL;
			return;
		}
		if(tid != 0) pthread_join(tid, NULL);
		delete pTimer;
		pTimer = NULL;
	}
}

/*
**	타이머 콜백 (TimerObject 스레드에서 호출)
**	- LINK_TIMER : 주기 타이머. CTimerObject 는 1회성 이므로 콜백 안에서 재등록
*/
void OnMwTimerEvent(UINT nTimerId, UINT nPeriod, char *szNumber, void* pTimer, int64_t timer_delay)
{
	CMwTimer* pMwTimer = (CMwTimer*)pTimer;
	if(pMwTimer == NULL) return;
	if(onlic_start != TRUE) return;

	switch(nTimerId){
	  case LINK_TIMER:
		if(sys_conf.role_link_check == TRUE){
			LinkCheck_OnTimer();		/* AlarmProc.cpp (5단계) : OnCheckLinkData 이식 */
		}
		pMwTimer->SetUserTimer(LINK_TIMER, (long)sys_conf.link_check_period_sec * 1000);
		break;
	  default:
		OnlicLogf(LOG_SOFT, "OnMwTimerEvent unknown timer id:%u", nTimerId);
		break;
	}
}
