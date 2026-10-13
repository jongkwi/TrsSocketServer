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
	__timer_run = FALSE;
	if(pTimer != NULL){
		pTimer->TimerObjectKill();
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
