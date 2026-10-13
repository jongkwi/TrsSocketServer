/*
**	MwTimer.h : CTimerObject 래퍼 (onmw MwTimer.h 동일 패턴)
*/
#ifndef	___ONLIC_MW_TIMER_H___
#define	___ONLIC_MW_TIMER_H___

#include <TimerObject/TimerObject.h>

class CMwTimer
{
public:
	CMwTimer();
	virtual ~CMwTimer();

	void MwTimerInit(const char* szTimerName = "");
	void SetUserTimer(UINT nTimerId, long lPeriod);
	void SetUserTimer(UINT nTimerId, long lPeriod, char *szNumber);
	void KillUserTimer(UINT nTimerId);
	void KillUserNumberTimer(UINT nTimerId, char *szNumber);
	void KillUserAllTimer();
	void EndUserTimer();

	CTimerObject *pTimer;
	char m_szTimerName[51];

protected:
	int __timer_run;
};

void OnMwTimerEvent(UINT nTimerId, UINT nPeriod, char *szNumber, void* pTimer, int64_t delay_time);

#endif /* MwTimer.h */
