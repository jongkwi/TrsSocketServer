/*
**	DbLic.h : NASA_License / onpia_check / onpia_sms 쿼리 함수 (Windows CDirectODBC·TrsSocketServer.cpp 의 SQL 이식)
**	- SQL 문자열은 Windows 원본과 동일하게 유지 (docs/DB스키마.md). PG 방언 치환 1건: DbLic_IsNewSiteExpired
**	- 외부 입력(IP, MAC, 메시지)은 CDirectODBC::SqlEscape 로 이스케이프
**	- 배열 반환 함수는 new[] 로 할당, 호출측이 delete[]
*/
#ifndef ___ONLIC_DBLIC_H___
#define ___ONLIC_DBLIC_H___

/* 시각 문자열 (Windows sprintf 포맷 그대로) */
void	DbLic_Now14(char *szOut);						/* YYYYMMDDHHMMSS (cc_update_time, NOW_DATE) */
void	DbLic_Today8(char *szOut);						/* YYYYMMDD (cc_pause 비교) */
void	DbLic_TodayHyphen(char *szOut);					/* YYYY-MM-DD (cc_nasupload_fail, 결정 B12 소스 형식 유지) */
void	DbLic_AgoMinute14(int nAgoMinute, char *szOut);	/* now - n분 → YYYYMMDDHHMMSS (GetAgoTimeProc) */

/* ---- 6321 라이선스 (TrsSocketServer.cpp:714-719) ---- */
int		DbLic_IpcsLogInsert(CDirectODBC *pDB, const char *szMac, const char *szIP, const char *szType, const char *szRealIP);
long	DbLic_AccessCount(CDirectODBC *pDB, const char *szRealIP);		/* lic_access_list count. 실패 -1 */

/* ---- CallCenter_Link ---- */
long	DbLic_SiteCount(CDirectODBC *pDB, const char *szIP, const char *szRealIP);					/* :854 cc_ip/cc_real_ip */
long	DbLic_SiteCountUse(CDirectODBC *pDB, const char *szIP, const char *szRealIP);				/* :890 + cc_use='Y' */
long	DbLic_SiteCountLinkError(CDirectODBC *pDB, const char *szIP, const char *szRealIP);			/* :1017 + cc_link_error='Y' */
int		DbLic_SiteInsertNew(CDirectODBC *pDB, const char *szIP, const char *szRealIP);				/* :857 'new_site','신규1' */
int		DbLic_IsNewSiteExpired(CDirectODBC *pDB, const char *szIP, const char *szRealIP, int nHour);	/* :903 DATEDIFF → PG */
int		DbLic_UpdateTimeAll(CDirectODBC *pDB, const char *szDate14);								/* :255 cc_use='Y' */
int		DbLic_UpdateTimeNotError(CDirectODBC *pDB, const char *szDate14);							/* :1441 link_error='N' */
int		DbLic_UpdateTimeSite(CDirectODBC *pDB, const char *szDate14, const char *szIP, const char *szRealIP);	/* :1026 + link_error='N' */
int		DbLic_SetLinkError(CDirectODBC *pDB, const char *szIP, const char *szRealIP);				/* :1523 cc_link_error='Y' */
long	DbLic_LinkErrorCount(CDirectODBC *pDB, const char *szLocalDay14);							/* :1447 */
int		DbLic_GetLinkErrorSites(CDirectODBC *pDB, const char *szLocalDay14, CC_LINKINFO **ppList);	/* :1457, 반환 건수 */
int		DbLic_GetNasFailCnt(CDirectODBC *pDB, const char *szIP, const char *szRealIP);				/* :1036 실패 -1 */
int		DbLic_SetNasFailCnt(CDirectODBC *pDB, const char *szIP, const char *szRealIP, int nCnt);	/* :1045/:1080 */
int		DbLic_GetNasUploadFail(CDirectODBC *pDB, const char *szIP, const char *szRealIP, char *szDay, int nSize);	/* :1090 */
int		DbLic_SetNasUploadFail(CDirectODBC *pDB, const char *szIP, const char *szRealIP, const char *szDay);		/* :1110 */
int		DbLic_GetPause(CDirectODBC *pDB, const char *szIP, const char *szRealIP, char *szPause, int nSize);		/* :1749 */
int		DbLic_ClearPause(CDirectODBC *pDB, const char *szIP, const char *szRealIP);					/* :1763 */

/* ---- SITE_SMS_USER / SITE_SMS_BASIC_USER ---- */
long	DbLic_SmsUserCount(CDirectODBC *pDB, const char *szIP, const char *szRealIP);				/* :841 */
int		DbLic_GetSmsSiteUsers(CDirectODBC *pDB, const char *szIP, const char *szRealIP, SMS_SITE_USERS **ppUsers);	/* :850 반환 건수 */
long	DbLic_SmsBasicUserCount(CDirectODBC *pDB);													/* :1217 */
int		DbLic_GetSmsBasicUsers(CDirectODBC *pDB, SMS_SITE_BASIC_USER **ppUsers);					/* :1231 반환 건수 */
int		DbLic_SmsUserInsert(CDirectODBC *pDB, const char *szIP, const char *szRealIP, const SMS_SITE_BASIC_USER &bu);	/* :1234 */

/* ---- SITE_EMAIL_USER / SITE_SmsExcludeTime ---- */
int		DbLic_GetEmailUsers(CDirectODBC *pDB, const char *szIP, const char *szRealIP, int nEventType, EMAIL_SITE_USERS **ppUsers);	/* DirectODBC.cpp:649-657, 16컬럼 */
int		DbLic_GetSmsExclude(CDirectODBC *pDB, SMS_EXCLUDE **ppList);								/* DirectODBC.cpp:754-762 */

/* ---- 알림톡 / SMS INSERT ---- */
int		DbLic_AtalkInsert(CDirectODBC *pDB, const char *szTmplCode, const char *szPhone, const char *szMsg);		/* :874 MTS_ATALK_MSG */
int		DbLic_SmsInsert(CDirectODBC *pDB, const char *szSubject, const char *szMsg, const char *szDate14, const char *szCallback, int nDestCount, const char *szDestInfo);	/* :882/:1339 SDK_SMS_SEND */

#endif /* DbLic.h */
