#ifndef ___USERTIME_H___
#define ___USERTIME_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

void WaitSleep(unsigned long ltime);
void WaitMSleep(DWORD dwTime);
int64_t GetMilliSencondProc();
int GetIntMilliSencond();

int64_t GetSencondProc();

void GetLocalDayProc(char *szLocalDay);
void GetLocalDay2Proc(char *szLocalDay);
void GetLastDayProc(char *szLastDay);			//JM 한달을 뺀 날짜


void GetMakeAgoDayProc(char *szMakeDay, char* szAgoDay, int nDay);
void GetMakeDayProc(char *szLocalDay, int nDay);
void GetMakeTimeProc(char *szLocalTime, int nMin);
void GetCarDsipMakeTime(char* szCompDay, char* szSearchDay, int nMin);
void Make14StrTime(char* szTime, int nSec = 0);
void GetMakeDay2Proc(char *szLocalDay, int nDay);

void GetCarDsipMakeTime2(char* szCompDay, char* szSearchDay, char* szSearchTime, int nMin);

void GetMakeDayMonthProc(char *szLocalDay, int nMonth);
void GetMakeDayMonth2Proc(char *szLocalDay, int nMonth);


void GetLocalDateProc(int &nYear, int &nMonth, int &nDay, int &nWeek);
void GetLocalTimeProc(char *szLocalTime);
void GetLocalTimeProc(int &nHour, int &nMinute, int &nSec);
void GetLocalTimeProc(int &nHour, int &nMinute, int &nSec, int &nMilli);
void GetLocalTime2Proc(char *szLocalTime);
void GetLocalTime3Proc(char *szLocalTime);
void GetLocalTime4Proc(char *szLocalTime);
void GetLocalTime5Proc(char *szLocalTime);
void GetLocalTime6Proc(char *szLocalTime);



int64_t GetCPUClockCount();
int64_t GetCPUClockDebugCount();
void GetRandomChar(char *szRand);
void GetRandomNumber(UINT &dwRand);
int64_t GetCpuTick();		//JK 090720

void GetLocalDayTimeProc(char *szLocalTime);
void GetLocalDayTime2Proc(char *szLocalTime);

void GetInt64ToString(int64_t _delaytime, char* szTimeString);

int GetDateStringHyphenFormat(char* szDate, char* szHyphenDate);// YYYYMMDD -> YYYY-MM-DD
int GetTimeStringColonFormat(char* szTime, char* szColonTime); // hhmmss -> hh:mm:ss



int VerifyUtcCurrentTime(struct tm &tm, int pos = 0);
void GetTimeOfDay_Tm(struct tm &tm);
void LocalTime_Tm(struct tm &tm);
void LocalTime_Tm(struct tm &tm, time_t &localt);


extern int64_t __cpu_freq;
extern int __freq_flag;
#endif//UserTime.h
