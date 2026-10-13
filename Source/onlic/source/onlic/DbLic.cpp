/*
**	DbLic.cpp : 쿼리 함수 구현. 각 함수 주석의 :NNN 은 TrsSocketServer.cpp 원본 라인
*/
#include "onlicDefine.h"
#include <time.h>

#define ESC_LEN		512
#define ESC(src, dst)	CDirectODBC::SqlEscape((src), (dst), sizeof(dst))

/* ---------------- 시각 ---------------- */
static void DbLic_LocalTm(struct tm &t, time_t tAdd)
{
	time_t now = time(NULL) + tAdd;
	localtime_r(&now, &t);
}

void DbLic_Now14(char *szOut)
{
	struct tm t; DbLic_LocalTm(t, 0);
	sprintf(szOut, "%04d%02d%02d%02d%02d%02d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
}

void DbLic_Today8(char *szOut)
{
	struct tm t; DbLic_LocalTm(t, 0);
	sprintf(szOut, "%04d%02d%02d", t.tm_year+1900, t.tm_mon+1, t.tm_mday);
}

void DbLic_TodayHyphen(char *szOut)
{
	struct tm t; DbLic_LocalTm(t, 0);
	sprintf(szOut, "%04d-%02d-%02d", t.tm_year+1900, t.tm_mon+1, t.tm_mday);
}

/* Windows GetAgoTimeProc(:1545): 현재 - nAgoMinute 분 */
void DbLic_AgoMinute14(int nAgoMinute, char *szOut)
{
	struct tm t; DbLic_LocalTm(t, -(time_t)nAgoMinute * 60);
	sprintf(szOut, "%04d%02d%02d%02d%02d%02d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
}

/* ---------------- 6321 라이선스 ---------------- */
/* :714 INSERT INTO IPCS_LOG (LOG_DATA, LOG_IP, LOG_TYPE, LOG_REAL_IP) */
int DbLic_IpcsLogInsert(CDirectODBC *pDB, const char *szMac, const char *szIP, const char *szType, const char *szRealIP)
{
	char q[MAX_QUERY+1] = "", eMac[ESC_LEN] = "", eIP[ESC_LEN] = "", eType[ESC_LEN] = "", eReal[ESC_LEN] = "";
	if(pDB == NULL) return FALSE;
	ESC(szMac, eMac); ESC(szIP, eIP); ESC(szType, eType); ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY, "INSERT INTO IPCS_LOG (LOG_DATA, LOG_IP, LOG_TYPE, LOG_REAL_IP) VALUES ('%s', '%s', '%s', '%s')", eMac, eIP, eType, eReal);
	return pDB->ExecSql(q);
}

/* :719 select count(*) from lic_access_list where nasa_con_ip='%s' and nasa_use='Y' */
long DbLic_AccessCount(CDirectODBC *pDB, const char *szRealIP)
{
	char q[MAX_QUERY+1] = "", eReal[ESC_LEN] = "";
	if(pDB == NULL) return -1;
	ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY, "select count(*) from lic_access_list where nasa_con_ip='%s' and nasa_use='Y'", eReal);
	return pDB->RowCount(q);
}

/* ---------------- CallCenter_Link ---------------- */
long DbLic_SiteCount(CDirectODBC *pDB, const char *szIP, const char *szRealIP)
{
	char q[MAX_QUERY+1] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "";
	if(pDB == NULL) return -1;
	ESC(szIP, eIP); ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY, "SELECT COUNT(cc_ip) FROM CallCenter_Link WHERE cc_ip='%s' AND cc_real_ip='%s'", eIP, eReal);
	return pDB->RowCount(q);
}

long DbLic_SiteCountUse(CDirectODBC *pDB, const char *szIP, const char *szRealIP)
{
	char q[MAX_QUERY+1] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "";
	if(pDB == NULL) return -1;
	ESC(szIP, eIP); ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY, "SELECT COUNT(cc_ip) FROM CallCenter_Link WHERE cc_ip='%s' AND cc_real_ip='%s' AND cc_use='Y'", eIP, eReal);
	return pDB->RowCount(q);
}

long DbLic_SiteCountLinkError(CDirectODBC *pDB, const char *szIP, const char *szRealIP)
{
	char q[MAX_QUERY+1] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "";
	if(pDB == NULL) return -1;
	ESC(szIP, eIP); ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY, "SELECT Count(cc_ip) FROM CallCenter_Link where cc_ip='%s' AND cc_real_ip='%s' AND cc_use='Y' AND cc_link_error='Y'", eIP, eReal);
	return pDB->RowCount(q);
}

/* :857/:1242 INSERT INTO CallCenter_Link (cc_name, cc_site_name, cc_ip, cc_real_ip) values ('new_site', '신규1', ...) */
int DbLic_SiteInsertNew(CDirectODBC *pDB, const char *szIP, const char *szRealIP)
{
	char q[MAX_QUERY+1] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "";
	if(pDB == NULL) return FALSE;
	ESC(szIP, eIP); ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY, "INSERT INTO CallCenter_Link (cc_name, cc_site_name, cc_ip, cc_real_ip) values ('new_site', '신규1', '%s', '%s')", eIP, eReal);
	return pDB->ExecSql(q);
}

/*
**	:903 "신규1" 등록 후 nHour 시간 경과 → 알람 억제
**	MSSQL: DATEDIFF(hour, cc_insert_time, getdate()) > 3  →  PG: EXTRACT(EPOCH FROM now() - cc_insert_time)/3600 > 3
**	반환 TRUE = 경과(억제), FALSE = 아님/오류
*/
int DbLic_IsNewSiteExpired(CDirectODBC *pDB, const char *szIP, const char *szRealIP, int nHour)
{
	char q[MAX_QUERY+1] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "";
	if(pDB == NULL) return FALSE;
	ESC(szIP, eIP); ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY,
		"SELECT COUNT(cc_ip) FROM CallCenter_Link where cc_use='Y' AND cc_site_name='신규1' AND EXTRACT(EPOCH FROM now() - cc_insert_time)/3600 > %d AND cc_insert_time is NOT NULL AND cc_ip='%s' AND cc_real_ip='%s'",
		nHour, eIP, eReal);
	return (pDB->RowCount(q) > 0) ? TRUE : FALSE;
}

/* :255 UPDATE CallCenter_Link SET cc_update_time='%s' WHERE cc_use='Y' */
int DbLic_UpdateTimeAll(CDirectODBC *pDB, const char *szDate14)
{
	char q[MAX_QUERY+1] = "";
	if(pDB == NULL) return FALSE;
	nprintf(q, MAX_QUERY, "UPDATE CallCenter_Link SET cc_update_time='%s' WHERE cc_use='Y'", szDate14);
	return pDB->ExecSql(q);
}

/* :1441 UPDATE CallCenter_Link SET cc_update_time='%s' where cc_link_error='N' AND cc_use='Y' (LINK_TIMER 최초 1회) */
int DbLic_UpdateTimeNotError(CDirectODBC *pDB, const char *szDate14)
{
	char q[MAX_QUERY+1] = "";
	if(pDB == NULL) return FALSE;
	nprintf(q, MAX_QUERY, "UPDATE CallCenter_Link SET cc_update_time='%s' where cc_link_error='N' AND cc_use='Y'", szDate14);
	return pDB->ExecSql(q);
}

/* :1026 UPDATE CallCenter_Link SET cc_update_time='%s',cc_link_error='N' where cc_ip='%s' AND cc_real_ip='%s' AND cc_use='Y' */
int DbLic_UpdateTimeSite(CDirectODBC *pDB, const char *szDate14, const char *szIP, const char *szRealIP)
{
	char q[MAX_QUERY+1] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "";
	if(pDB == NULL) return FALSE;
	ESC(szIP, eIP); ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY, "UPDATE CallCenter_Link SET cc_update_time='%s',cc_link_error='N' where cc_ip='%s' AND cc_real_ip='%s' AND cc_use='Y'", szDate14, eIP, eReal);
	return pDB->ExecSql(q);
}

/* :1523 UPDATE CallCenter_Link SET cc_link_error='Y' where cc_ip='%s' AND cc_real_ip='%s' AND cc_use='Y' */
int DbLic_SetLinkError(CDirectODBC *pDB, const char *szIP, const char *szRealIP)
{
	char q[MAX_QUERY+1] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "";
	if(pDB == NULL) return FALSE;
	ESC(szIP, eIP); ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY, "UPDATE CallCenter_Link SET cc_link_error='Y' where cc_ip='%s' AND cc_real_ip='%s' AND cc_use='Y'", eIP, eReal);
	return pDB->ExecSql(q);
}

/* :1447 SELECT COUNT(cc_ip) FROM CallCenter_Link where cc_update_time<='%s' AND cc_link_error='N' AND cc_use='Y' */
long DbLic_LinkErrorCount(CDirectODBC *pDB, const char *szLocalDay14)
{
	char q[MAX_QUERY+1] = "";
	if(pDB == NULL) return -1;
	nprintf(q, MAX_QUERY, "SELECT COUNT(cc_ip) FROM CallCenter_Link where cc_update_time<='%s' AND cc_link_error='N' AND cc_use='Y'", szLocalDay14);
	return pDB->RowCount(q);
}

/* :1457 SELECT cc_index,cc_name,cc_ip, cc_real_ip, cc_site_name FROM CallCenter_Link where cc_update_time<='%s' AND cc_link_error='N' AND cc_use='Y' */
int DbLic_GetLinkErrorSites(CDirectODBC *pDB, const char *szLocalDay14, CC_LINKINFO **ppList)
{
	char q[MAX_QUERY+1] = "";
	long lCnt = 0;
	int i = 0, nRet = 0;
	CC_LINKINFO *pList = NULL;

	if(ppList == NULL) return 0;
	*ppList = NULL;
	if(pDB == NULL) return 0;

	lCnt = DbLic_LinkErrorCount(pDB, szLocalDay14);
	if(lCnt <= 0) return 0;

	pList = new CC_LINKINFO[lCnt + 1];
	memset_st(pList, 0, sizeof(CC_LINKINFO) * (lCnt + 1));

	nprintf(q, MAX_QUERY, "SELECT cc_index,cc_name,cc_ip, cc_real_ip, cc_site_name FROM CallCenter_Link where cc_update_time<='%s' AND cc_link_error='N' AND cc_use='Y'", szLocalDay14);
	if(pDB->OpenTable(q) == TRUE){
		while(i < lCnt && (nRet = pDB->Fetch()) == TRUE){
			pDB->GetInt(1, pList[i].cc_index);
			pDB->GetStr(2, pList[i].cc_name, sizeof(pList[i].cc_name));
			pDB->GetStr(3, pList[i].cc_ip, sizeof(pList[i].cc_ip));
			pDB->GetStr(4, pList[i].cc_real_ip, sizeof(pList[i].cc_real_ip));
			pDB->GetStr(5, pList[i].cc_site_name, sizeof(pList[i].cc_site_name));
			i++;
		}
		pDB->CloseTable();
	}
	if(i <= 0){ delete [] pList; return 0; }
	*ppList = pList;
	return i;
}

/* :1036 SELECT cc_nas_failcnt FROM CallCenter_Link WHERE cc_ip='%s' AND cc_real_ip='%s' */
int DbLic_GetNasFailCnt(CDirectODBC *pDB, const char *szIP, const char *szRealIP)
{
	char q[MAX_QUERY+1] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "";
	if(pDB == NULL) return -1;
	ESC(szIP, eIP); ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY, "SELECT cc_nas_failcnt FROM CallCenter_Link WHERE cc_ip='%s' AND cc_real_ip='%s'", eIP, eReal);
	return pDB->GetIntValue(q);
}

/* :1045/:1080 UPDATE CallCenter_Link SET cc_nas_failcnt = %d WHERE cc_ip='%s' AND cc_real_ip='%s' */
int DbLic_SetNasFailCnt(CDirectODBC *pDB, const char *szIP, const char *szRealIP, int nCnt)
{
	char q[MAX_QUERY+1] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "";
	if(pDB == NULL) return FALSE;
	ESC(szIP, eIP); ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY, "UPDATE CallCenter_Link SET cc_nas_failcnt = %d WHERE cc_ip='%s' AND cc_real_ip='%s'", nCnt, eIP, eReal);
	return pDB->ExecSql(q);
}

/* :1090 SELECT cc_nasupload_fail FROM CallCenter_Link WHERE cc_ip='%s' AND cc_real_ip='%s' */
int DbLic_GetNasUploadFail(CDirectODBC *pDB, const char *szIP, const char *szRealIP, char *szDay, int nSize)
{
	char q[MAX_QUERY+1] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "";
	if(pDB == NULL) return FALSE;
	ESC(szIP, eIP); ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY, "SELECT cc_nasupload_fail FROM CallCenter_Link WHERE cc_ip='%s' AND cc_real_ip='%s'", eIP, eReal);
	return pDB->GetValue(q, szDay, nSize);
}

/* :1110 UPDATE CallCenter_Link SET cc_nasupload_fail='%s' WHERE cc_ip='%s' AND cc_real_ip='%s' */
int DbLic_SetNasUploadFail(CDirectODBC *pDB, const char *szIP, const char *szRealIP, const char *szDay)
{
	char q[MAX_QUERY+1] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "";
	if(pDB == NULL) return FALSE;
	ESC(szIP, eIP); ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY, "UPDATE CallCenter_Link SET cc_nasupload_fail='%s' WHERE cc_ip='%s' AND cc_real_ip='%s'", szDay, eIP, eReal);
	return pDB->ExecSql(q);
}

/* :1749 SELECT cc_pause FROM CallCenter_Link where cc_ip='%s' AND cc_real_ip='%s' AND cc_use='Y' */
int DbLic_GetPause(CDirectODBC *pDB, const char *szIP, const char *szRealIP, char *szPause, int nSize)
{
	char q[MAX_QUERY+1] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "";
	if(pDB == NULL) return FALSE;
	ESC(szIP, eIP); ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY, "SELECT cc_pause FROM CallCenter_Link where cc_ip='%s' AND cc_real_ip='%s' AND cc_use='Y'", eIP, eReal);
	return pDB->GetValue(q, szPause, nSize);
}

/* :1763 UPDATE CallCenter_Link SET cc_pause='' where cc_ip='%s' AND cc_real_ip='%s' AND cc_use='Y' */
int DbLic_ClearPause(CDirectODBC *pDB, const char *szIP, const char *szRealIP)
{
	char q[MAX_QUERY+1] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "";
	if(pDB == NULL) return FALSE;
	ESC(szIP, eIP); ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY, "UPDATE CallCenter_Link SET cc_pause='' where cc_ip='%s' AND cc_real_ip='%s' AND cc_use='Y'", eIP, eReal);
	return pDB->ExecSql(q);
}

/* ---------------- SITE_SMS_USER ---------------- */
#define SMS_USER_WHERE	"where (sms_use='Y' OR sms_use='T') AND (sms_site_ip='%s' OR sms_site_ip='ALL') AND (sms_site_real_ip='%s' OR sms_site_real_ip='ALL')"

/* :841 SELECT Count(sms_user_phone) FROM SITE_SMS_USER where ... */
long DbLic_SmsUserCount(CDirectODBC *pDB, const char *szIP, const char *szRealIP)
{
	char q[MAX_QUERY+1] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "";
	if(pDB == NULL) return -1;
	ESC(szIP, eIP); ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY, "SELECT Count(sms_user_phone) FROM SITE_SMS_USER " SMS_USER_WHERE, eIP, eReal);
	return pDB->RowCount(q);
}

/* :850 SELECT sms_site_ip,sms_user_name,sms_user_phone, sms_site_name, sms_site_real_ip, sms_use FROM SITE_SMS_USER where ... */
int DbLic_GetSmsSiteUsers(CDirectODBC *pDB, const char *szIP, const char *szRealIP, SMS_SITE_USERS **ppUsers)
{
	char q[MAX_QUERY+1] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "";
	long lCnt = 0;
	int i = 0;
	SMS_SITE_USERS *pUsers = NULL;

	if(ppUsers == NULL) return 0;
	*ppUsers = NULL;
	if(pDB == NULL) return 0;

	lCnt = DbLic_SmsUserCount(pDB, szIP, szRealIP);
	if(lCnt <= 0) return 0;

	pUsers = new SMS_SITE_USERS[lCnt + 1];
	memset_st(pUsers, 0, sizeof(SMS_SITE_USERS) * (lCnt + 1));

	ESC(szIP, eIP); ESC(szRealIP, eReal);
	nprintf(q, MAX_QUERY, "SELECT sms_site_ip,sms_user_name,sms_user_phone, sms_site_name, sms_site_real_ip, sms_use FROM SITE_SMS_USER " SMS_USER_WHERE, eIP, eReal);
	if(pDB->OpenTable(q) == TRUE){
		while(i < lCnt && pDB->Fetch() == TRUE){
			pDB->GetStr(1, pUsers[i].sms_site_ip, sizeof(pUsers[i].sms_site_ip));
			pDB->GetStr(2, pUsers[i].sms_user_name, sizeof(pUsers[i].sms_user_name));
			pDB->GetStr(3, pUsers[i].sms_user_phone, sizeof(pUsers[i].sms_user_phone));
			pDB->GetStr(4, pUsers[i].sms_site_name, sizeof(pUsers[i].sms_site_name));
			pDB->GetStr(5, pUsers[i].sms_site_real_ip, sizeof(pUsers[i].sms_site_real_ip));
			pDB->GetStr(6, pUsers[i].sms_use, sizeof(pUsers[i].sms_use));
			i++;
		}
		pDB->CloseTable();
	}
	if(i <= 0){ delete [] pUsers; return 0; }
	*ppUsers = pUsers;
	return i;
}

/* :1217 SELECT count(site_sms_index) FROM SITE_SMS_BASIC_USER WHERE site_sms_use='Y' OR site_sms_use='T' */
long DbLic_SmsBasicUserCount(CDirectODBC *pDB)
{
	if(pDB == NULL) return -1;
	return pDB->RowCount("SELECT count(site_sms_index) FROM SITE_SMS_BASIC_USER WHERE site_sms_use='Y' OR site_sms_use='T'");
}

/* :1231 SELECT site_sms_index, site_sms_name, site_sms_telnum, site_sms_use FROM SITE_SMS_BASIC_USER WHERE site_sms_use='Y' OR site_sms_use='T' */
int DbLic_GetSmsBasicUsers(CDirectODBC *pDB, SMS_SITE_BASIC_USER **ppUsers)
{
	long lCnt = 0;
	int i = 0;
	SMS_SITE_BASIC_USER *pUsers = NULL;

	if(ppUsers == NULL) return 0;
	*ppUsers = NULL;
	if(pDB == NULL) return 0;

	lCnt = DbLic_SmsBasicUserCount(pDB);
	if(lCnt <= 0) return 0;

	pUsers = new SMS_SITE_BASIC_USER[lCnt + 1];
	memset_st(pUsers, 0, sizeof(SMS_SITE_BASIC_USER) * (lCnt + 1));

	if(pDB->OpenTable("SELECT site_sms_index, site_sms_name, site_sms_telnum, site_sms_use FROM SITE_SMS_BASIC_USER WHERE site_sms_use='Y' OR site_sms_use='T'") == TRUE){
		while(i < lCnt && pDB->Fetch() == TRUE){
			pDB->GetInt(1, pUsers[i].site_sms_index);
			pDB->GetStr(2, pUsers[i].site_sms_name, sizeof(pUsers[i].site_sms_name));
			pDB->GetStr(3, pUsers[i].site_sms_telnum, sizeof(pUsers[i].site_sms_telnum));
			pDB->GetStr(4, pUsers[i].site_sms_use, sizeof(pUsers[i].site_sms_use));
			i++;
		}
		pDB->CloseTable();
	}
	if(i <= 0){ delete [] pUsers; return 0; }
	*ppUsers = pUsers;
	return i;
}

/* :1234 INSERT INTO SITE_SMS_USER (sms_site_ip, sms_site_real_ip, sms_user_name, sms_user_phone, sms_use, sms_site_name) values (..., '%c', '신규1') */
int DbLic_SmsUserInsert(CDirectODBC *pDB, const char *szIP, const char *szRealIP, const SMS_SITE_BASIC_USER &bu)
{
	char q[MAX_QUERY+1] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "", eName[ESC_LEN] = "", eTel[ESC_LEN] = "";
	char cUse = bu.site_sms_use[0];
	if(pDB == NULL) return FALSE;
	ESC(szIP, eIP); ESC(szRealIP, eReal); ESC(bu.site_sms_name, eName); ESC(bu.site_sms_telnum, eTel);
	if(cUse == '\'' || cUse == 0) cUse = ' ';
	nprintf(q, MAX_QUERY, "INSERT INTO SITE_SMS_USER (sms_site_ip, sms_site_real_ip, sms_user_name, sms_user_phone, sms_use, sms_site_name) values ('%s', '%s', '%s', '%s', '%c', '신규1')", eIP, eReal, eName, eTel, cUse);
	return pDB->ExecSql(q);
}

/* ---------------- SITE_EMAIL_USER ---------------- */
/*
**	DirectODBC.cpp:649-723 + TrsSocketServer.cpp:1867 WHERE email_site_ip='%s' AND email_site_real_ip='%s' AND alarm_type=%d
**	결함 B 수정: 16컬럼을 전부 읽는다 (원본은 13번 email_url 을 attr1 로 읽어 한 칸씩 밀림)
*/
int DbLic_GetEmailUsers(CDirectODBC *pDB, const char *szIP, const char *szRealIP, int nEventType, EMAIL_SITE_USERS **ppUsers)
{
	char q[MAX_QUERY+1] = "", w[1024] = "", eIP[ESC_LEN] = "", eReal[ESC_LEN] = "";
	long lCnt = 0;
	int i = 0;
	EMAIL_SITE_USERS *pUsers = NULL;

	if(ppUsers == NULL) return 0;
	*ppUsers = NULL;
	if(pDB == NULL) return 0;

	ESC(szIP, eIP); ESC(szRealIP, eReal);
	nprintf(w, 1023, "email_site_ip='%s' AND email_site_real_ip='%s' AND  alarm_type=%d", eIP, eReal, nEventType);

	nprintf(q, MAX_QUERY, "SELECT COUNT(idx) FROM SITE_EMAIL_USER WHERE %s", w);
	lCnt = pDB->RowCount(q);
	if(lCnt <= 0) return 0;

	pUsers = new EMAIL_SITE_USERS[lCnt + 1];
	memset_st(pUsers, 0, sizeof(EMAIL_SITE_USERS) * (lCnt + 1));

	nprintf(q, MAX_QUERY, "SELECT idx,email_site_ip,email_site_real_ip,email_site_name,alarm_type,alarm_proc_name,email_to,email_from,email_title,email_msg1,email_msg2,email_msg3,email_url,email_attr1,email_attr2,email_attr3 FROM SITE_EMAIL_USER WHERE %s", w);
	if(pDB->OpenTable(q) == TRUE){
		while(i < lCnt && pDB->Fetch() == TRUE){
			long lIdx = 0;
			EMAIL_SITE_USERS &o = pUsers[i];
			pDB->GetLong(1, lIdx); o.idx = (DWORD)lIdx;
			pDB->GetStr(2,  o.email_site_ip, sizeof(o.email_site_ip));
			pDB->GetStr(3,  o.email_site_real_ip, sizeof(o.email_site_real_ip));
			pDB->GetStr(4,  o.email_site_name, sizeof(o.email_site_name));
			pDB->GetInt(5,  o.alarm_type);
			pDB->GetStr(6,  o.alarm_proc_name, sizeof(o.alarm_proc_name));
			pDB->GetStr(7,  o.email_to, sizeof(o.email_to));
			pDB->GetStr(8,  o.email_from, sizeof(o.email_from));
			pDB->GetStr(9,  o.email_title, sizeof(o.email_title));
			pDB->GetStr(10, o.email_msg1, sizeof(o.email_msg1));
			pDB->GetStr(11, o.email_msg2, sizeof(o.email_msg2));
			pDB->GetStr(12, o.email_msg3, sizeof(o.email_msg3));
			pDB->GetStr(13, o.email_url, sizeof(o.email_url));
			pDB->GetStr(14, o.email_attr1, sizeof(o.email_attr1));
			pDB->GetStr(15, o.email_attr2, sizeof(o.email_attr2));
			pDB->GetStr(16, o.email_attr3, sizeof(o.email_attr3));
			i++;
		}
		pDB->CloseTable();
	}
	if(i <= 0){ delete [] pUsers; return 0; }
	*ppUsers = pUsers;
	return i;
}

/* ---------------- SITE_SmsExcludeTime (DirectODBC.cpp:754-818) ---------------- */
int DbLic_GetSmsExclude(CDirectODBC *pDB, SMS_EXCLUDE **ppList)
{
	long lCnt = 0;
	int i = 0;
	SMS_EXCLUDE *pList = NULL;

	if(ppList == NULL) return 0;
	*ppList = NULL;
	if(pDB == NULL) return 0;

	lCnt = pDB->RowCount("SELECT COUNT(sm_index) FROM SITE_SmsExcludeTime WHERE sm_use='Y'");
	if(lCnt <= 0) return 0;

	pList = new SMS_EXCLUDE[lCnt + 1];
	memset_st(pList, 0, sizeof(SMS_EXCLUDE) * (lCnt + 1));

	if(pDB->OpenTable("SELECT sm_index, sm_type, sm_msg, sm_stime, sm_etime FROM SITE_SmsExcludeTime WHERE sm_use='Y'") == TRUE){
		while(i < lCnt && pDB->Fetch() == TRUE){
			pDB->GetInt(1, pList[i].sm_index);
			pDB->GetInt(2, pList[i].sm_type);
			pDB->GetStr(3, pList[i].sm_msg, sizeof(pList[i].sm_msg));
			pDB->GetStr(4, pList[i].sm_stime, sizeof(pList[i].sm_stime));
			pDB->GetStr(5, pList[i].sm_etime, sizeof(pList[i].sm_etime));
			strcpy_st(pList[i].sm_use, "Y");
			i++;
		}
		pDB->CloseTable();
	}
	if(i <= 0){ delete [] pList; return 0; }
	*ppList = pList;
	return i;
}

/* ---------------- 알림톡 / SMS ---------------- */
/*
**	:874/:1191/:1265/:1510 INSERT INTO "MTS_ATALK_MSG" (...) VALUES (nextval('mts_atalk_msg_seq'), '<sender_key>', '%s', '15889310', '%s', 'OnpiaATalk', '%s', now(), 5, '1', 'S', '%s')
**	고정값은 ini [ATALK] 에서 (기본값 = 원본 하드코딩). TRAN_REPLACE_MSG = TRAN_MSG
*/
int DbLic_AtalkInsert(CDirectODBC *pDB, const char *szTmplCode, const char *szPhone, const char *szMsg)
{
	char q[MAX_QUERY+1] = "", eTmpl[ESC_LEN] = "", ePhone[ESC_LEN] = "", eMsg[2048] = "", eKey[ESC_LEN] = "", eCb[ESC_LEN] = "", eSub[ESC_LEN] = "";
	if(pDB == NULL) return FALSE;
	ESC(szTmplCode, eTmpl); ESC(szPhone, ePhone); ESC(szMsg, eMsg);
	ESC(sys_conf.atalk_sender_key, eKey); ESC(sys_conf.atalk_callback, eCb); ESC(sys_conf.atalk_subject, eSub);
	nprintf(q, MAX_QUERY,
		"INSERT INTO \"MTS_ATALK_MSG\" (TRAN_PR, TRAN_SENDER_KEY, TRAN_TMPL_CD, TRAN_CALLBACK, TRAN_PHONE, TRAN_SUBJECT, TRAN_MSG, TRAN_DATE, TRAN_TYPE, TRAN_STATUS, TRAN_REPLACE_TYPE, TRAN_REPLACE_MSG) "
		"VALUES (nextval('mts_atalk_msg_seq'), '%s', '%s', '%s', '%s', '%s', '%s', now(), %d, '1', 'S', '%s')",
		eKey, eTmpl, eCb, ePhone, eSub, eMsg, sys_conf.atalk_tran_type, eMsg);
	return pDB->ExecSql(q);
}

/*
**	:882/:1199/:1274/:1518/:1339 INSERT INTO SDK_SMS_SEND (USER_ID, SUBJECT, SMS_MSG, NOW_DATE, SEND_DATE, CALLBACK, DEST_COUNT, DEST_INFO)
**	VALUES ('onpia', '%s', '%s', '%s', '%s', '%s', %d, '%s')   - NOW_DATE = SEND_DATE = szDate14, DEST_INFO = 이름^번호|이름^번호
*/
int DbLic_SmsInsert(CDirectODBC *pDB, const char *szSubject, const char *szMsg, const char *szDate14, const char *szCallback, int nDestCount, const char *szDestInfo)
{
	char q[MAX_QUERY+1] = "", eSub[ESC_LEN] = "", eMsg[2048] = "", eCb[ESC_LEN] = "", eDest[2048] = "", eUser[ESC_LEN] = "";
	if(pDB == NULL) return FALSE;
	ESC(szSubject, eSub); ESC(szMsg, eMsg); ESC(szCallback, eCb); ESC(szDestInfo, eDest); ESC(sys_conf.sms_user_id, eUser);
	nprintf(q, MAX_QUERY,
		"INSERT INTO SDK_SMS_SEND (USER_ID, SUBJECT, SMS_MSG, NOW_DATE, SEND_DATE, CALLBACK, DEST_COUNT, DEST_INFO) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', %d, '%s')",
		eUser, eSub, eMsg, szDate14, szDate14, eCb, nDestCount, eDest);
	return pDB->ExecSql(q);
}
