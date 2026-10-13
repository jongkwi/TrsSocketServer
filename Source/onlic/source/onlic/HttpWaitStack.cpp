/*
**	HttpWaitStack.cpp : 웹푸시/메일 워커. AlramWeb.cpp 의 50ms 폴링 루프 + 큐를 그대로, 전송만 libcurl
**	Windows 와 다른 점: 타임아웃 5초(원본 없음), 응답 코드·본문 로그, 세션 핸들 누수 없음
*/
#include "onlicDefine.h"
#include "HttpWaitStack.h"
#ifdef _CURL
#include <curl/curl.h>
#endif

CHttpWait	*__http_wait = NULL;

int HttpWaitStackInit()
{
	if(__http_wait != NULL) return FALSE;
	__http_wait = new CHttpWait();
	return __http_wait->Start();
}

void HttpWaitStackKill()
{
	if(__http_wait != NULL){ __http_wait->Kill(); delete __http_wait; __http_wait = NULL; }
}

/* ---------------- 인코딩 ---------------- */
/*
**	Windows: mbcsToUtf8 → URLEncode(unsafe 문자 집합만 %XX). Linux: iconv CP949→UTF-8 → UrlEnDe urlencode(영숫자 -._ 외 전부 %XX)
**	수신측(PHP/Java)은 둘 다 같은 값으로 디코드한다
*/
int Http_Utf8UrlEncode(const char *szSrc, char *szDst, int nDstSize)
{
	iconv_t cd;
	char szUtf8[8192];
	unsigned char szEnc[8192 * 3];
	size_t nRet = 0;
	int nLen = 0;

	if(szDst == NULL || nDstSize <= 0) return 0;
	szDst[0] = 0;
	if(szSrc == NULL || strlen_st(szSrc) <= 0) return 0;

	memset_st(szUtf8, 0, sizeof(szUtf8));
	cd = IConv_open((char*)"UTF-8", (char*)"CP949");
	if(cd == (iconv_t)-1){
		strncpy_st(szUtf8, szSrc, sizeof(szUtf8)-1);		/* 변환 불가 시 원문 */
	}else{
		nRet = IConv(cd, szSrc, strlen_st(szSrc), szUtf8, sizeof(szUtf8)-1);
		IConv_close(cd);
		if(nRet == (size_t)-1 || strlen_st(szUtf8) <= 0) strncpy_st(szUtf8, szSrc, sizeof(szUtf8)-1);
	}
	memset_st(szEnc, 0, sizeof(szEnc));
	urlencode((unsigned char*)szUtf8, szEnc);
	nLen = strlen_st((char*)szEnc);
	if(nLen >= nDstSize) nLen = nDstSize - 1;
	memcpy(szDst, szEnc, nLen); szDst[nLen] = 0;
	return nLen;
}

/* ---------------- 워커 ---------------- */
void* RunHttpWaitThread(void *arg)
{
	CHttpWait *p = (CHttpWait*)arg;
	ALARM_OBJ obj;
	int i = 0;

	OnlicLog(LOG_SOFT, "RunHttpWaitThread start");
	for( ; ; ){
		if(p->m_bStop == TRUE || onlic_start == FALSE) break;
		memset_st(&obj, 0, sizeof(obj));
		if(p->AlarmList_Get(obj) == TRUE){
			if(obj.ao_type == AR_EMAIL) p->AlarmList_SendEmail(obj);
			else p->AlarmList_Show(obj);
			for(i = 0; i < 5; i++){ WaitMSleep(50); if(p->m_bStop == TRUE) break; }
		}else{
			for(i = 0; i < 10; i++){ WaitMSleep(50); if(p->m_bStop == TRUE) break; }
		}
	}
	OnlicLog(LOG_SOFT, "RunHttpWaitThread exit");
	p->m_pid = 0;
	return NULL;
}

CHttpWait::CHttpWait()
{
	m_pid = 0;
	m_bStop = FALSE;
	m_run = FALSE;
}

CHttpWait::~CHttpWait()
{
	Kill();
}

int CHttpWait::Start()
{
	if(m_run == TRUE) return FALSE;
	Pthread_mutex_init(&m_mutex, NULL);
	m_bStop = FALSE;
	m_run = TRUE;
	Pthread_create(&m_pid, RunHttpWaitThread, this);
	return TRUE;
}

void CHttpWait::Kill()
{
	int i = 0;
	ALARM_OBJ *p = NULL;
	LISTPOS pos = NULL;
	if(m_run != TRUE) return;
	m_bStop = TRUE;
	for(i = 0; i < 30; i++){ if(m_pid == 0) break; WaitMSleep(100); }
	if(m_pid != 0){ Pthread_kill(&m_pid, SIGINT); m_pid = 0; }
	Pthread_mutex_lock(&m_mutex);
	pos = m_list.GetHeadPosition();
	while(pos != NULL){
		p = (ALARM_OBJ*)m_list.GetAt(pos);
		if(p != NULL) delete p;
		m_list.GetNext(pos);
	}
	m_list.RemoveAll();
	Pthread_mutex_unlock(&m_mutex);
	m_run = FALSE;
}

/* AlramWeb.cpp:105 - 500 바이트 미만만 메시지 복사 (원본 동일. 초과면 빈 메시지로 큐잉됨 → 여기서는 큐잉하지 않음) */
int CHttpWait::AlertList_Push(const char *szMsg, int nAlertType)
{
	ALARM_OBJ *p = NULL;
	if(m_run != TRUE) return TRUE;
	if(szMsg == NULL || strlen_st(szMsg) >= 500){
		OnlicLogf(LOG_ALARM, "AlertList_Push skip (len:%d >= 500)", (szMsg != NULL) ? strlen_st(szMsg) : 0);
		return TRUE;
	}
	p = new ALARM_OBJ;
	memset_st(p, 0, sizeof(ALARM_OBJ));
	p->ao_type = AR_ALERT;
	p->alert_type = nAlertType;
	strcpy_st(p->ao_msg, szMsg);
	Pthread_mutex_lock(&m_mutex);
	m_list.AddTail(p);
	Pthread_mutex_unlock(&m_mutex);
	return FALSE;
}

int CHttpWait::AlertList_EmailPush(const EMAIL_SITE_USERS &e)
{
	ALARM_OBJ *p = NULL;
	if(m_run != TRUE) return TRUE;
	p = new ALARM_OBJ;
	memset_st(p, 0, sizeof(ALARM_OBJ));
	p->ao_type = AR_EMAIL;
	strcpy_st(p->szEMail_to, e.email_to);
	strcpy_st(p->szEMail_from, e.email_from);
	strcpy_st(p->szEMail_title, e.email_title);
	strcpy_st(p->szEMail_Msg, e.email_msg);
	strcpy_st(p->szEMail_Url, e.email_url);
	Pthread_mutex_lock(&m_mutex);
	m_list.AddTail(p);
	Pthread_mutex_unlock(&m_mutex);
	return FALSE;
}

int CHttpWait::AlarmList_Get(ALARM_OBJ &obj)
{
	int nResult = FALSE;
	ALARM_OBJ *p = NULL;
	LISTPOS pos = NULL;
	Pthread_mutex_lock(&m_mutex);
	if(m_list.GetCount() > 0){
		pos = m_list.GetHeadPosition();
		if(pos != NULL){
			p = (ALARM_OBJ*)m_list.GetAt(pos);
			if(p != NULL){
				memcpy(&obj, p, sizeof(ALARM_OBJ));
				delete p;
				nResult = TRUE;
			}
			m_list.RemoveAt(pos);
		}
	}
	Pthread_mutex_unlock(&m_mutex);
	return nResult;
}

/* AlramWeb.cpp:208 - POST msg=<utf8 urlencoded> */
void CHttpWait::AlarmList_Show(ALARM_OBJ &obj)
{
	char szEnc[4096] = "", szBody[4200] = "", szOut[2048] = "";
	int nCode = 0;
	Http_Utf8UrlEncode(obj.ao_msg, szEnc, sizeof(szEnc));
	nprintf(szBody, sizeof(szBody)-1, "msg=%s", szEnc);
	nCode = RequestPost(sys_conf.webpush_url, szBody, szOut, sizeof(szOut));
	OnlicLogf(LOG_ALARM, "AlarmList_Show-S url:%s, data:%s", sys_conf.webpush_url, szBody);
	OnlicLogf(LOG_ALARM, "AlarmList_Show-R code:%d, data:%s", nCode, szOut);
}

/* AlramWeb.cpp:289 - POST to=&cc=&subject=&content= (cc 는 원본에서 빈 값) */
void CHttpWait::AlarmList_SendEmail(ALARM_OBJ &obj)
{
	char szTo[2048] = "", szTitle[1024] = "", szMsg[8192] = "", szBody[12000] = "", szOut[2048] = "";
	int nCode = 0;
	Http_Utf8UrlEncode(obj.szEMail_to, szTo, sizeof(szTo));
	Http_Utf8UrlEncode(obj.szEMail_title, szTitle, sizeof(szTitle));
	Http_Utf8UrlEncode(obj.szEMail_Msg, szMsg, sizeof(szMsg));
	nprintf(szBody, sizeof(szBody)-1, "to=%s&cc=%s&subject=%s&content=%s", szTo, "", szTitle, szMsg);
	nCode = RequestPost(sys_conf.mail_url, szBody, szOut, sizeof(szOut));
	OnlicLogf(LOG_ALARM, "AlarmList_SendEmail-S url:%s, data:%s", sys_conf.mail_url, szBody);
	OnlicLogf(LOG_ALARM, "AlarmList_SendEmail-R code:%d, data:%s", nCode, szOut);
}

#ifdef _CURL
static size_t Http_WriteCb(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	char *out = (char*)userdata;
	size_t n = size * nmemb, cur = strlen(out);
	size_t room = 2047 - cur;					/* szOut 2048 가정 (RequestPost 에서 nOutSize 로 제한) */
	if(room == 0) return n;
	if(n > room) n = room;
	memcpy(out + cur, ptr, n); out[cur + n] = 0;
	return size * nmemb;
}
#endif

/*
**	AlramWeb.cpp:323 RequestPost - application/x-www-form-urlencoded POST. 반환 HTTP 코드(>0) 또는 음수(오류)
*/
int CHttpWait::RequestPost(const char *szUrl, const char *szBody, char *szOut, int nOutSize)
{
	if(szOut != NULL && nOutSize > 0) szOut[0] = 0;
#ifdef _CURL
	CURL *curl = NULL;
	CURLcode res;
	long lCode = 0;
	struct curl_slist *hdr = NULL;

	curl = curl_easy_init();
	if(curl == NULL) return -1;
	hdr = curl_slist_append(hdr, "Content-Type: application/x-www-form-urlencoded");
	curl_easy_setopt(curl, CURLOPT_URL, szUrl);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdr);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, szBody);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen_st(szBody));
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, (long)sys_conf.http_timeout_sec);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)(sys_conf.http_timeout_sec * 2));
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Http_WriteCb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, szOut);
	res = curl_easy_perform(curl);
	if(res != CURLE_OK){
		OnlicLogf(LOG_ALARM, "RequestPost fail url:%s err:%d(%s)", szUrl, (int)res, curl_easy_strerror(res));
		curl_slist_free_all(hdr);
		curl_easy_cleanup(curl);
		return -2;
	}
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &lCode);
	curl_slist_free_all(hdr);
	curl_easy_cleanup(curl);
	return (int)lCode;
#else
	OnlicLogf(LOG_ALARM, "RequestPost skipped (built without _CURL) url:%s", szUrl);
	return -9;
#endif
}

/* ---------------- Windows CTrsSocketServerApp 래퍼 ---------------- */
int AlarmWebTh_Push(const char *szMsg, int nType)
{
	if(__http_wait == NULL) return TRUE;
	return __http_wait->AlertList_Push(szMsg, nType);
}

/*
**	TrsSocketServer.cpp:1859-1996 AlarmWebTh_EmailPush : SITE_EMAIL_USER 조회 → 제목/본문 기본값 채움 → 큐
*/
int AlarmWebTh_EmailPush(CDirectODBC *pDB, int nEventType, const char *szIP, const char *szRealIP, const char *szSiteName, const char *szProcName, const char *szContents)
{
	EMAIL_SITE_USERS *pEmailUser = NULL;
	int nCnt = 0, i = 0, nFind = TRUE;

	OnlicLogf(LOG_ALARM, "AlarmWebTh_EmailPush1 type:%d, ip:%s, rip:%s, name:%s, proc:%s", nEventType, szIP, szRealIP, szSiteName, szProcName);
	if(pDB == NULL || __http_wait == NULL) return FALSE;
	if(strlen_st(szIP) == 0 || strlen_st(szRealIP) == 0) return FALSE;

	nCnt = DbLic_GetEmailUsers(pDB, szIP, szRealIP, nEventType, &pEmailUser);
	if(nCnt <= 0 || pEmailUser == NULL){
		OnlicLogf(LOG_ALARM, "AlarmWebTh_EmailPush nodata type:%d, ip:%s, rip:%s, name:%s, proc:%s", nEventType, szIP, szRealIP, szSiteName, szProcName);
		return FALSE;
	}
	OnlicLogf(LOG_ALARM, "AlarmWebTh_EmailPush getdata type:%d, ip:%s, rip:%s, name:%s, proc:%s cnt:%d", nEventType, szIP, szRealIP, szSiteName, szProcName, nCnt);

	for(i = 0; i < nCnt; i++){
		EMAIL_SITE_USERS &e = pEmailUser[i];
		if(strlen_st(e.email_site_real_ip) <= 0) break;		/* 원본 루프 종료 조건 */
		if(strlen_st(e.email_to) <= 0) continue;
		nFind = TRUE;
		switch(nEventType){
		  case SEND_TYPE_NET_ERROR:
			if(strlen_st(e.email_msg1) <= 0) nprintf(e.email_msg1, 500, "%s-%s에서 링크 데이터가 수신 되고 있지 않습니다. 인터넷 회선 점검 및 서버 점검 필요!", szSiteName, szIP);
			if(strlen_st(e.email_title) <= 0) nprintf(e.email_title, 200, "%s-%s의 서버 및 인터넷 회선 장애", szSiteName, szIP);
			break;
		  case SEND_TYPE_NET_SUCC:
			if(strlen_st(e.email_msg1) <= 0) nprintf(e.email_msg1, 500, "%s-%s에서 링크 데이터가 수신 되고 있습니다", szSiteName, szIP);
			if(strlen_st(e.email_title) <= 0) nprintf(e.email_title, 200, "%s-%s의 네트워크 정상 연결", szSiteName, szIP);
			break;
		  case SEND_TYPE_PROC_ERROR:
			if(strlen_st(e.alarm_proc_name) > 0 && strstr(szProcName, e.alarm_proc_name) == NULL) nFind = FALSE;
			if(strlen_st(e.email_msg1) <= 0) strncpy_st(e.email_msg1, szContents, 500);
			if(strlen_st(e.email_title) <= 0) nprintf(e.email_title, 200, "%s-%s의 %s 프로세스 장애 발생", szSiteName, szIP, szProcName);
			break;
		  case SEND_TYPE_PROC_SUCC:
			if(strlen_st(e.alarm_proc_name) > 0 && strstr(szProcName, e.alarm_proc_name) == NULL) nFind = FALSE;
			if(strlen_st(e.email_msg1) <= 0) strncpy_st(e.email_msg1, szContents, 500);
			if(strlen_st(e.email_title) <= 0) nprintf(e.email_title, 200, "%s-%s의 %s 프로세스 장애 복구", szSiteName, szIP, szProcName);
			break;
		  case SEND_TYPE_NAS_CON_FAIL:
			if(strlen_st(e.email_msg1) <= 0) strncpy_st(e.email_msg1, szContents, 500);
			if(strlen_st(e.email_title) <= 0) nprintf(e.email_title, 200, "%s-%s의 %s NAS 네트워크 장애", szSiteName, szIP, szProcName);
			break;
		  case SEND_TYPE_NAS_CON_SUCC:
			if(strlen_st(e.email_msg1) <= 0) strncpy_st(e.email_msg1, szContents, 500);
			if(strlen_st(e.email_title) <= 0) nprintf(e.email_title, 200, "%s-%s의 %s NAS 네트워크 연결", szSiteName, szIP, szProcName);
			break;
		  case SEND_TYPE_NAS_UPLOAD_FAIL:
			if(strlen_st(e.email_msg1) <= 0) strncpy_st(e.email_msg1, szContents, 500);
			if(strlen_st(e.email_title) <= 0) nprintf(e.email_title, 200, "%s-%s의 %s  NAS 업로드 실패", szSiteName, szIP, szProcName);
			break;
		  case SEND_TYPE_NAS_UPLOAD_SUCC:
			if(strlen_st(e.email_msg1) <= 0) strncpy_st(e.email_msg1, szContents, 500);
			if(strlen_st(e.email_title) <= 0) nprintf(e.email_title, 200, "%s-%s의 %s NAS 업로드 성공", szSiteName, szIP, szProcName);
			break;
		  default:
			break;
		}
		nprintf(e.email_msg, 2047, "%s%s%s", e.email_msg1, e.email_msg2, e.email_msg3);
		if(nFind == TRUE){
			OnlicLogf(LOG_ALARM, "AlarmWebTh_EmailPush push type:%d, ip:%s, rip:%s, name:%s, proc:%s to:%s", nEventType, szIP, szRealIP, szSiteName, szProcName, e.email_to);
			__http_wait->AlertList_EmailPush(e);
		}
	}
	delete [] pEmailUser;
	return FALSE;
}
