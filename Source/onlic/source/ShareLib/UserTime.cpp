
#include "ShareLib.h"

int64_t __cpu_freq = 0;
int		__freq_flag = FALSE;

void WaitSleep(unsigned long ltime)
{
#ifdef _WIN32
	Sleep(ltime*1000);
#else
	struct timespec inteval;
	inteval.tv_sec = ltime;
	inteval.tv_nsec = 0;
	nanosleep(&inteval, NULL);
#endif
}

void WaitMSleep(DWORD dwTime)
{
#ifdef _WIN32
	if(dwTime <= 0)
		dwTime = 0;
	Sleep(dwTime);
#else
	struct timeval inteval;
	inteval.tv_sec = dwTime/1000;
	inteval.tv_usec = (dwTime%1000)*1000;
	select(1, NULL, NULL, NULL, &inteval);
#endif
}

int64_t GetMilliSencondProc()
{
	int64_t ct;
	time_t  unixT;

	// 1970년 이후 유닉스 시간을 구한다.
	time(&unixT);
#ifdef _WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);
	DWORD msecond = st.wMilliseconds;
	ct = ((int64_t)unixT*1000)+(int64_t)msecond;
#else


#ifdef _64BIT

	timeval tv;
	//	struct tm* today_tm;
	gettimeofday(&tv, NULL);
	//	DWORD msecond = (DWORD)(tv.tv_usec/1000);
	//	ct = ((int64_t)unixT*1000)+(int64_t)msecond;

	// 기존 초와 밀리초 각각 구해 처리로 발생되는 오류에 대한 수정 - 240102
	ct = ((int64_t)(tv.tv_sec * 1000)) + ((int64_t)(tv.tv_usec / 1000));
#else

	timeval tv;	
	gettimeofday(&tv, NULL);
	DWORD msecond = (DWORD)(tv.tv_usec/1000);
	ct = ((int64_t)unixT*1000)+(int64_t)msecond;	
#endif



#endif

	// 15:48:54.993 1672037334993
	// 15:48:55.000 1672037334000 <---------------- 오류 발생 

	return ct;
}

int GetIntMilliSencond()
{
#ifdef _WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);
	int msecond = st.wMilliseconds;
#else
	timeval tv;

	gettimeofday(&tv, NULL);
	int msecond = (int)(tv.tv_usec/1000);
#endif

	return msecond;
}


int64_t GetSencondProc()
{
	int64_t ct;
	time_t  unixT;

	// 1970년 이후 유닉스 시간을 구한다.
	time(&unixT);
#ifdef _WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);
//	int msecond = st.wMilliseconds;
#else
	timeval tv;
	gettimeofday(&tv, NULL);
//	int msecond = (int)(tv.tv_usec/1000);
#endif

	// 미리세컨단위로 변환한다.
	ct = (int64_t)((int64_t)unixT);	
	return ct;

}



void GetLocalDayProc(char *szLocalDay)
{
	struct tm today_tm;
	LocalTime_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 3);

	nprintf((char*)szLocalDay, 20, "%04d-%02d-%02d",
		today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday);	
}



void GetLocalDay2Proc(char *szLocalDay)
{
	struct tm today_tm;
	LocalTime_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 4);

	nprintf((char*)szLocalDay, 20, "%04d%02d%02d",
		today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday);
}



void GetLastDayProc(char *szLastDay)	//한달을 뺀 날짜 //JM
{	
	struct tm today_tm;
	LocalTime_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 5);

	nprintf((char*)szLastDay, 20, "%04d-%02d-%02d",
		today_tm.tm_year+1900, today_tm.tm_mon, today_tm.tm_mday);
}



void GetMakeDayProc(char *szLocalDay, int nDay)
{
	struct tm today_tm;
	LocalTime_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 6);

	today_tm.tm_mday += nDay;
	mktime(&today_tm);
	nprintf((char*)szLocalDay, 20, "%04d-%02d-%02d",
		today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday);
}



void GetMakeTimeProc(char *szLocalTime, int nMin)
{
	struct tm today_tm;
	LocalTime_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 7);

	today_tm.tm_min += nMin;
	mktime(&today_tm);
	nprintf((char*)szLocalTime, 20, "%02d:%02d:%02d", today_tm.tm_hour, today_tm.tm_min, today_tm.tm_sec);
}



void GetCarDsipMakeTime(char* szCompDay, char* szSearchDay, int nMin)
{
	struct tm today_tm;
	LocalTime_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 8);

	today_tm.tm_min += nMin;
	mktime(&today_tm);	
	nprintf((char*)szCompDay, 20, "%04d%02d%02d%02d%02d%02d",
		today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday, today_tm.tm_hour, today_tm.tm_min, today_tm.tm_sec);

	nprintf((char*)szSearchDay, 20, "%04d-%02d-%02d",
		today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday);
}



void GetCarDsipMakeTime2(char* szCompDay, char* szSearchDay, char* szSearchTime, int nMin)
{
	struct tm today_tm;
	LocalTime_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 9);

	today_tm.tm_min += nMin;
	mktime(&today_tm);	
	nprintf((char*)szCompDay, 20, "%04d-%02d-%02d%02d:%02d:%02d",
		today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday, today_tm.tm_hour, today_tm.tm_min, today_tm.tm_sec);

	nprintf((char*)szSearchDay, 20, "%04d-%02d-%02d",
		today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday);

	nprintf((char*)szSearchTime, 20, "%02d:%02d:%02d",
		today_tm.tm_hour, today_tm.tm_min, today_tm.tm_sec);
}



void Make14StrTime(char* szTime, int nSec)
{
	struct tm today_tm;
	LocalTime_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 10);

	today_tm.tm_sec += nSec;
	mktime(&today_tm);

	sprintf((char*)szTime, "%04d%02d%02d%02d%02d%02d",
		today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday, today_tm.tm_hour, today_tm.tm_min, today_tm.tm_sec);	
}



void GetMakeDay2Proc(char *szLocalDay, int nDay)
{
	struct tm today_tm;
	LocalTime_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 11);

	today_tm.tm_mday += nDay;
	mktime(&today_tm);
	nprintf((char*)szLocalDay, 20, "%04d%02d%02d",
		today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday);
}



void GetMakeAgoDayProc(char *szMakeDay, char* szAgoDay, int nDay)
{
	struct tm today_tm;
	LocalTime_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 12);

	char szValue[5] = "";
	if(strlen_st(szAgoDay) == 8){  // YYYYMMDD
		strncpy(szValue, szAgoDay, 4); szValue[4] = '\0';		
		today_tm.tm_year = -1900 + atoi(szValue);
		strncpy(szValue, szAgoDay + 4, 2); szValue[2] = '\0';	
		today_tm.tm_mon = atoi(szValue) - 1;
		strncpy(szValue, szAgoDay +6, 2); szValue[2] = '\0';
		today_tm.tm_mday = atoi(szValue);
		today_tm.tm_mday += nDay;
		mktime(&today_tm);
		nprintf((char*)szMakeDay, 20, "%04d%02d%02d",
			today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday);
	}
}



void GetMakeDayMonthProc(char *szLocalDay, int nMonth)
{
	struct tm today_tm;
	LocalTime_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 13);

	today_tm.tm_mon += nMonth;
	mktime(&today_tm);
	nprintf((char*)szLocalDay, 20, "%04d-%02d-%02d",
		today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday);
}



void GetMakeDayMonth2Proc(char *szLocalDay, int nMonth)
{
	struct tm today_tm;
	LocalTime_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 14);
	
	today_tm.tm_mon += nMonth;
	mktime(&today_tm);
	nprintf((char*)szLocalDay, 20, "%04d%02d%02d",
		today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday);
}



void GetLocalDateProc(int &nYear, int &nMonth, int &nDay, int &nWeek)
{
	struct tm today_tm;
	LocalTime_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 15);
	
	nYear = today_tm.tm_year+1900;
	nMonth = today_tm.tm_mon+1;
	nDay = today_tm.tm_mday;
	nWeek = today_tm.tm_wday;
}



void GetLocalTimeProc(char *szLocalTime)
{
#ifdef _WIN32
	
	SYSTEMTIME st;
	GetLocalTime(&st);
	nprintf(szLocalTime, 20, "%02d:%02d:%02d", 
        st.wHour, st.wMinute, st.wSecond);
#else	
	struct tm today_tm;
	GetTimeOfDay_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 16);

	nprintf(szLocalTime, 20, "%02d:%02d:%02d", 
        today_tm.tm_hour, today_tm.tm_min, today_tm.tm_sec);
#endif
}



void GetLocalTime2Proc(char *szLocalTime)
{
#ifdef _WIN32	
	SYSTEMTIME st;
	GetLocalTime(&st);
	nprintf(szLocalTime, 20, "%02d%02d%02d%03d", 
        st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else
 	timeval tv;
 	struct tm today_tm;
 	gettimeofday(&tv, NULL);									  
	localtime_r(&tv.tv_sec, &today_tm);	//today_tm = *localtime((time_t*)&tv.tv_sec);
	VerifyUtcCurrentTime(today_tm, 17);

	nprintf(szLocalTime, 20, "%02d%02d%02d%03d", 
        today_tm.tm_hour, today_tm.tm_min, today_tm.tm_sec, (int)(tv.tv_usec/1000));
#endif
}



void GetLocalTime3Proc(char *szLocalTime)
{
#ifdef _WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);
	nprintf(szLocalTime, 20, "%02d:%02d:%02d:%03d", 
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else
	timeval tv;
	struct tm today_tm;
	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &today_tm);	//today_tm = *localtime((time_t*)&tv.tv_sec);	
	VerifyUtcCurrentTime(today_tm, 18);

	nprintf(szLocalTime, 20, "%02d:%02d:%02d:%03d", 
		today_tm.tm_hour, today_tm.tm_min, today_tm.tm_sec, (int)(tv.tv_usec/1000));
#endif
}



void GetLocalTime4Proc(char *szLocalTime)
{
	struct tm today_tm;
	LocalTime_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 19);	
	nprintf((char*)szLocalTime, 20, "%04d%02d%02d%02d%02d%02d",
		today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday, today_tm.tm_hour, today_tm.tm_min, today_tm.tm_sec);
}



void GetLocalTime5Proc(char *szLocalTime)
{
#ifdef _WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);	
	nprintf(szLocalTime, 20, "%02d:%02d", st.wHour, st.wMinute);
#else
	struct tm today_tm;
	GetTimeOfDay_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 20);
	nprintf(szLocalTime, 20, "%02d:%02d", today_tm.tm_hour, today_tm.tm_min);
#endif
}



void GetLocalTime6Proc(char *szLocalTime)
{
	struct tm today_tm;
	LocalTime_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 21);
	nprintf((char*)szLocalTime, 20, "%04d-%02d-%02d %02d:%02d",
		today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday, today_tm.tm_hour, today_tm.tm_min);
}



void GetLocalTimeProc(int &nHour, int &nMinute, int &nSec)
{
#ifdef _WIN32	
	SYSTEMTIME st;
	GetLocalTime(&st);
	nHour = st.wHour;
	nMinute = st.wMinute;
	nSec = st.wSecond;
#else
	struct tm today_tm;
	GetTimeOfDay_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 22);

    nHour = today_tm.tm_hour;
	nMinute = today_tm.tm_min;
	nSec = today_tm.tm_sec;
#endif
}



void GetLocalTimeProc(int &nHour, int &nMinute, int &nSec, int &nMilli)
{
#ifdef _WIN32	
	SYSTEMTIME st;
	GetLocalTime(&st);
	nHour = st.wHour;
	nMinute = st.wMinute;
	nSec = st.wSecond;
	nMilli = st.wMilliseconds;
#else
	timeval tv;
	struct tm today_tm;
	gettimeofday(&tv, NULL);	
	localtime_r(&tv.tv_sec, &today_tm);	//today_tm = *localtime((time_t*)&tv.tv_sec);
	VerifyUtcCurrentTime(today_tm, 23);

    nHour = today_tm.tm_hour;
	nMinute = today_tm.tm_min;
	nSec = today_tm.tm_sec;
	nMilli = (int)(tv.tv_usec/1000);
#endif
}



int64_t GetCPUClockCount()
{
#ifdef _WIN32
    int64_t tick, tmp_tick;
    int *lo = ((int*) &tmp_tick);
    int *hi = ((int*) &tmp_tick) + 1;

	__asm {
      rdtsc
      mov ebx, lo
      mov [ebx], eax
      mov ebx, hi
      mov [ebx], edx
   }
	tick = tmp_tick/1000;
#elif _FREEBSD
    int64_t tick, tmp_tick;
	__asm__ volatile (".byte 0x0f, 0x31" : "=A" (tmp_tick));
	tick = tmp_tick/1000;
#else 
	struct timespec ts;
    int64_t tick;
	clock_gettime(CLOCK_REALTIME, &ts);
	tick =  (int64_t)((ts.tv_sec-((ts.tv_sec/1000)*1000)) * 1000000) + (int64_t)(ts.tv_nsec/1000);
#endif
    return tick;
}



int64_t GetCPUClockDebugCount()
{
#ifdef _WIN32
    int64_t tick, tmp_tick;
    int *lo = ((int*) &tmp_tick);
    int *hi = ((int*) &tmp_tick) + 1;

	__asm {
      rdtsc
      mov ebx, lo
      mov [ebx], eax
      mov ebx, hi
      mov [ebx], edx
   }
	tick = tmp_tick/1000;
#elif _FREEBSD
    int64_t tick, tmp_tick;
	__asm__ volatile (".byte 0x0f, 0x31" : "=A" (tmp_tick));
	tick = tmp_tick/1000;
#else 
	struct timespec ts;
    int64_t tick;
	clock_gettime(CLOCK_REALTIME, &ts);
	tick =  (int64_t)((ts.tv_sec-((ts.tv_sec/1000000)*1000000)) * 1000) + (int64_t)(ts.tv_nsec/1000000);
#endif
    return tick;
}



void GetRandomChar(char *szRand)
{
	int n1;
	n1 = rand();
#ifdef _WIN32
    int64_t tick;
    int *lo = ((int*) &tick);
    int *hi = ((int*) &tick) + 1;

	__asm {
      rdtsc
      mov ebx, lo
      mov [ebx], eax
      mov ebx, hi
      mov [ebx], edx
   }
#elif _FREEBSD
    int64_t tick;
    int *lo = ((int*) &tick);
    int *hi = ((int*) &tick) + 1;
	__asm__ volatile (".byte 0x0f, 0x31" : "=A" (tick));
#else 
	struct timespec ts;
    int64_t tick;
    int *lo = ((int*) &tick);
    int *hi = ((int*) &tick) + 1;
	
	clock_gettime(CLOCK_REALTIME, &ts);
	tick = (ts.tv_sec * (int64_t)1000000000) + (int64_t)(ts.tv_nsec);
#endif
	sprintf_st(szRand, "%x%x%x", *hi, *lo, n1);
}



void GetRandomNumber(UINT &dwRand)
{
	int n1;
	n1 = rand();
#ifdef _WIN32
    int64_t tick;
    int *lo = ((int*) &tick);
    int *hi = ((int*) &tick) + 1;

	__asm {
      rdtsc
      mov ebx, lo
      mov [ebx], eax
      mov ebx, hi
      mov [ebx], edx
   }
#elif _FREEBSD
    int64_t tick;
    int *lo = ((int*) &tick);
    int *hi = ((int*) &tick) + 1;
	__asm__ volatile (".byte 0x0f, 0x31" : "=A" (tick));
#else 
	struct timespec ts;
    int64_t tick;
    int *lo = ((int*) &tick);
    int *hi = ((int*) &tick) + 1;
	
	clock_gettime(CLOCK_REALTIME, &ts);
	tick = (ts.tv_sec * (int64_t)1000000000) + (int64_t)(ts.tv_nsec);
#endif

	dwRand = *lo + n1;
}



int64_t GetCpuTick()		//JK 090720
{
	int64_t tick = 0;
	int nResult = 0;
#ifdef _WIN32
	if(__cpu_freq == 0 && __freq_flag == FALSE){		
		int64_t  start, stop;//, nCtr, nFreq, nCtrStop;
		/*
		int nFreqChk = FALSE;
		if((nFreqChk = QueryPerformanceFrequency((LARGE_INTEGER *)&nFreq)) == TRUE){
			int *lo_start = ((int*) &start);
			int *hi_start = ((int*) &start) + 1;
			__asm {
				rdtsc
					mov ebx, lo_start
					mov [ebx], eax
					mov ebx, hi_start
					mov [ebx], edx
			}
			QueryPerformanceCounter((LARGE_INTEGER *)&nCtrStop);
			nCtrStop += nFreq;
			do{
				QueryPerformanceCounter((LARGE_INTEGER *)&nCtr);
			}while (nCtr < nCtrStop);
			int *lo_stop = ((int*) &stop);
			int *hi_stop = ((int*) &stop) + 1;
			__asm {
				rdtsc
					mov ebx, lo_stop
					mov [ebx], eax
					mov ebx, hi_stop
					mov [ebx], edx
			}
			__cpu_freq =  (stop-start) / 1000;
		}else
			__freq_flag = TRUE;
		//printf("GetCPUTick __cpu_freq = %I64d , nFreqChk = %d\r\n", __cpu_freq, nFreqChk);
		*/
		start = GetCPUClockCount();
		WaitMSleep(1000);
		stop = GetCPUClockCount();
		__cpu_freq = (unsigned long)(stop-start);
		printf("GetCPUTick __cpu_freq = %I64d \n", __cpu_freq);
		__freq_flag = TRUE;
	}
	if(__cpu_freq > 0){		
		int64_t tmp_tick;
		int *lo = ((int*) &tmp_tick);
		int *hi = ((int*) &tmp_tick) + 1;
		__asm {
			rdtsc
				mov ebx, lo
				mov [ebx], eax
				mov ebx, hi
				mov [ebx], edx
		}
		tick = tmp_tick / __cpu_freq;
		//printf("GetCPUTick %I64d, tick = %I64d \r\n", __cpu_freq, tick);
	}else{
		tick = timeGetTime();
		//printf("GetCPUTick timeGetTime = %I64d \r\n", tick);
	}
#endif
	return tick;
}



void GetLocalDayTimeProc(char *szLocalTime)
{
#ifdef _WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);	
	nprintf(szLocalTime, 20, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);		
#else
	struct tm today_tm;
	GetTimeOfDay_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 24);
	nprintf(szLocalTime, 20, "%04d%02d%02d%02d%02d%02d", today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday,
		today_tm.tm_hour, today_tm.tm_min, today_tm.tm_sec);	
#endif
}



void GetLocalDayTime2Proc(char *szLocalTime)
{
#ifdef _WIN32
	SYSTEMTIME st;
	GetLocalTime(&st);	
	nprintf(szLocalTime, 20, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);		
#else
	struct tm today_tm;
	GetTimeOfDay_Tm(today_tm);
	VerifyUtcCurrentTime(today_tm, 25);
	nprintf(szLocalTime, 20, "%04d-%02d-%02d %02d:%02d:%02d", today_tm.tm_year+1900, today_tm.tm_mon+1, today_tm.tm_mday,
		today_tm.tm_hour, today_tm.tm_min, today_tm.tm_sec);	
#endif
}



void GetInt64ToString(int64_t _delaytime, char* szTimeString)
{
#ifdef _WIN32
	sprintf_st(szTimeString, "%I64d", _delaytime);
#else
	sprintf_st(szTimeString, "%lld", _delaytime);
#endif

}



int GetDateStringHyphenFormat(char* szDate, char* szHyphenDate) // YYYYMMDD -> YYYY-MM-DD
{
	int nReplace = FALSE;
	if(strlen_st(szDate) == 8){ //szDate = YYYYMMDD		
		char szYear[5] = "", szMonth[3] = "", szDay[3] = "";
		strncpy(szYear, szDate, 4); szYear[4] = '\0';
		strncpy(szMonth, szDate+4, 2); szMonth[2] = '\0';
		strncpy(szDay, szDate+6, 2); szDay[2] = '\0';

		if( (atoi(szYear) >= 1900) && 
			(atoi(szMonth) >= 1 && atoi(szMonth) <= 12) &&
			(atoi(szDay) >= 1 && atoi(szDay) <= 31) ){

				sprintf_st(szHyphenDate, "%s-%s-%s", szYear, szMonth, szDay);
				nReplace = TRUE;
		}
	}	

	// 날짜형식이 아니면 원본데이터로 치환한다. 
	if(nReplace == FALSE){
		strcpy_st(szHyphenDate, szDate);
	}
	return nReplace;
}



int GetTimeStringColonFormat(char* szTime, char* szColonTime) // hhmmss -> hh:mm:ss
{
	int nReplace = FALSE;
	if(strlen_st(szTime) == 6){ //szTime = hhmmss

		char szHour[3] = "", szMin[3] = "", szSec[3] = "";

		strncpy(szHour, szTime, 2); szHour[2] = '\0';
		strncpy(szMin, szTime+2, 2); szMin[2] = '\0';
		strncpy(szSec, szTime+4, 2); szSec[2] = '\0';

		if( (atoi(szHour) >= 0  && atoi(szHour) < 24) && 
			(atoi(szMin) >= 0 && atoi(szMin) < 60) &&
			(atoi(szSec) >= 0 && atoi(szSec) < 60) ){

				sprintf_st(szColonTime, "%s:%s:%s", szHour, szMin, szSec);
				nReplace = TRUE;
		}
	}	

	// 시간형식이 아니면 원본데이터로 치환한다. 
	if(nReplace == FALSE){
		strcpy_st(szColonTime, szTime);
	}
	return nReplace;
}



int VerifyUtcCurrentTime(struct tm &tm, int pos)
{
#ifdef _UNIX
	time_t current_time;
	struct tm utc_time;
		
	for(int i = 0; i < 3; i++){
		struct timeval tv;
		gettimeofday(&tv, NULL);		
		gmtime_r(&tv.tv_sec, &utc_time);
		if(utc_time.tm_hour == tm.tm_hour){		

			gettimeofday(&tv, NULL);
			localtime_r(&tv.tv_sec, &tm);
			//time(&current_time);
			//tm = *localtime(&current_time);

			char szLog[1024] = "";
			openlog("onmw", LOG_PID|LOG_CONS, LOG_USER);
			sprintf_st(szLog, "VerifyUtcCurrentTime error count:%d, hour:%d, pos:%d", i, utc_time.tm_hour, pos);
			syslog(LOG_WARNING, szLog);
			closelog();
		}else
			break;
	}	
#endif
	return FALSE;
}



void GetTimeOfDay_Tm(struct tm &tm)
{
#ifdef _WIN32
	time_t	localt;
	time(&localt);
	tm = *localtime(&localt);	
#else
	timeval tv;	
	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &tm);	
#endif	
}



void LocalTime_Tm(struct tm &tm)
{
	time_t	localt;
	time(&localt);
#ifdef _WIN32	
	tm = *localtime(&localt);	
#else	
	localtime_r(&localt, &tm);	
#endif
}



void LocalTime_Tm(struct tm &tm, time_t &localt)
{
	time(&localt);
#ifdef _WIN32	
	tm = *localtime(&localt);	
#else	
	localtime_r(&localt, &tm);	
#endif
}
