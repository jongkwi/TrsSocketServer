# TrsSocketServer_Lic_VS2019_Atalk - C/C++ 소스 심층 분석 리포트

> **분석일:** 2026-10-13 10:31:56
> **대상:** F:\AI_Pro\TrsSocketserver\Source\TrsSocketServer_Lic_VS2019_Atalk
> **파일:** 25개 | **함수:** 12개 | **라인:** 6,147

---

## 1. 분석 요약

| 항목 | 수치 |
|------|------|
| 전체 파일 | 25개 |
| 전체 라인 | 6,147 |
| 코드 라인 | 4,139 |
| 주석 라인 | 657 |
| 함수 수 | 12개 |
| 구조체/클래스 | 39개 |
| 스레드 생성 | 0개 |
| 뮤텍스 | 0개 |
| 조건변수 | 0개 |
| 보안 이슈 (전체) | **236개** |
| - Critical | **0개** |
| - High | **222개** |
| - Medium | 14개 |
| - Low | 0개 |
| 품질 이슈 (전체) | **62개** |
| - High | **2개** |
| - Medium | 39개 |
| - Low | 21개 |

---

## 2. 보안 취약점 (236건)

### 2.2 [High] (222건)

| 유형 | 파일:라인 | 설명 | 코드 |
|------|----------|------|------|
| BUFFER_OVERFLOW | `AlramWeb.cpp:116` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(pAlObj->ao_msg, szMsg);` |
| BUFFER_OVERFLOW | `AlramWeb.cpp:139` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(pAlObj->szEMail_to, emailObj.email_to);` |
| BUFFER_OVERFLOW | `AlramWeb.cpp:140` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(pAlObj->szEMail_from, emailObj.email_from);` |
| BUFFER_OVERFLOW | `AlramWeb.cpp:141` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(pAlObj->szEMail_title, emailObj.email_title);` |
| BUFFER_OVERFLOW | `AlramWeb.cpp:142` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(pAlObj->szEMail_Msg, emailObj.email_msg);` |
| BUFFER_OVERFLOW | `AlramWeb.cpp:143` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(pAlObj->szEMail_Url, emailObj.email_url);` |
| BUFFER_OVERFLOW | `AlramWeb.cpp:314` | sprintf 사용 (snprintf 권장) | `sprintf(szLog, "AlarmList_SendEmail-S rul:%s, data:%s", strUrl, strMsg);` |
| BUFFER_OVERFLOW | `AlramWeb.cpp:317` | sprintf 사용 (snprintf 권장) | `sprintf(szLog, "AlarmList_SendEmail-R code:%d,  data:%s ", nCode, strOutMsg);` |
| BUFFER_OVERFLOW | `ClientSocket.cpp:59` | sprintf 사용 (snprintf 권장) | `sprintf((char*)szLog, "LINK Server Connect Success");` |
| BUFFER_OVERFLOW | `ClientSocket.cpp:69` | sprintf 사용 (snprintf 권장) | `sprintf((char*)szLog, "LINK Server Connect Fail");` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:401` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(szValue, szTemp);` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:505` | strcpy 사용 (strncpy/strcpy_st 권장) | `trim(sms_site_ip); strcpy((char*)pApp->m_pSmsSiteUsers[nIndex].sms_site_ip, sms_` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:506` | strcpy 사용 (strncpy/strcpy_st 권장) | `trim(sms_user_name); strcpy((char*)pApp->m_pSmsSiteUsers[nIndex].sms_user_name, ` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:507` | strcpy 사용 (strncpy/strcpy_st 권장) | `trim(sms_user_phone); strcpy((char*)pApp->m_pSmsSiteUsers[nIndex].sms_user_phone` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:508` | strcpy 사용 (strncpy/strcpy_st 권장) | `trim(sms_site_name); strcpy((char*)pApp->m_pSmsSiteUsers[nIndex].sms_site_name, ` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:509` | strcpy 사용 (strncpy/strcpy_st 권장) | `trim(sms_site_real_ip); strcpy((char*)pApp->m_pSmsSiteUsers[nIndex].sms_site_rea` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:510` | strcpy 사용 (strncpy/strcpy_st 권장) | `trim(sms_use); strcpy((char*)pApp->m_pSmsSiteUsers[nIndex].sms_use, sms_use);` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:569` | strcpy 사용 (strncpy/strcpy_st 권장) | `trim(site_sms_name); strcpy((char*)pBasicUser->site_sms_name, site_sms_name);` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:570` | strcpy 사용 (strncpy/strcpy_st 권장) | `trim(site_sms_telnum); strcpy((char*)pBasicUser->site_sms_telnum, site_sms_telnu` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:571` | strcpy 사용 (strncpy/strcpy_st 권장) | `trim(site_sms_use); strcpy((char*)pBasicUser->site_sms_use, site_sms_use);` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:629` | strcpy 사용 (strncpy/strcpy_st 권장) | `trim(cc_name); strcpy((char*)pCCLink->cc_name, cc_name);` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:630` | strcpy 사용 (strncpy/strcpy_st 권장) | `trim(cc_ip); strcpy((char*)pCCLink->cc_ip, cc_ip);` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:631` | strcpy 사용 (strncpy/strcpy_st 권장) | `trim(cc_real_ip); strcpy((char*)pCCLink->cc_real_ip, cc_real_ip);` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:632` | strcpy 사용 (strncpy/strcpy_st 권장) | `trim(cc_site_name); strcpy((char*)pCCLink->cc_site_name, cc_site_name);` |
| SQL_INJECTION | `DirectODBC.cpp:649` | SQL 동적 조립 - Prepared Statement 미사용 | `sprintf(szQuery, "SELECT COUNT(idx) FROM SITE_EMAIL_USER WHERE %s", strWhereQuer` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:649` | sprintf 사용 (snprintf 권장) | `sprintf(szQuery, "SELECT COUNT(idx) FROM SITE_EMAIL_USER WHERE %s", strWhereQuer` |
| SQL_INJECTION | `DirectODBC.cpp:657` | SQL 동적 조립 - Prepared Statement 미사용 | `sprintf(szQuery, "SELECT idx,email_site_ip,email_site_real_ip,email_site_name,al` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:657` | sprintf 사용 (snprintf 권장) | `sprintf(szQuery, "SELECT idx,email_site_ip,email_site_real_ip,email_site_name,al` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:754` | sprintf 사용 (snprintf 권장) | `sprintf(szQuery, "SELECT COUNT(sm_index) FROM SITE_SmsExcludeTime WHERE sm_use='` |
| BUFFER_OVERFLOW | `DirectODBC.cpp:762` | sprintf 사용 (snprintf 권장) | `sprintf(szQuery, "SELECT sm_index, sm_type, sm_msg, sm_stime, sm_etime FROM SITE` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:118` | sprintf 사용 (snprintf 권장) | `sprintf(szLog, "SmsExcludeLoad idx:%d, type:%d, msg:%s, time:%s-%s",` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:149` | sprintf 사용 (snprintf 권장) | `sprintf(szCurrentTime, "%02d%02d", st.wHour, st.wMinute);` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:179` | sprintf 사용 (snprintf 권장) | `sprintf(szLog, "IsSmsExclude:%d, type:%d, msg:%s, findidx:%d",  Exclude, nType, ` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:202` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(buf, "������Դϴ�.");` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:207` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(buf, "��⿡ �����Ͽ����ϴ�.");` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:214` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(buf, "��⿡ �����Ͽ����ϴ�.");` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:222` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(buf, "������Դϴ�.");` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:227` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(buf, "��⿡ �����Ͽ����ϴ�.");` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:234` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(buf, "��⿡ �����Ͽ����ϴ�.");` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:241` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(buf, "������Դϴ�.");` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:247` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(buf, "��⿡ �����Ͽ����ϴ�.");` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:254` | sprintf 사용 (snprintf 권장) | `sprintf(szDate, "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHo` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:287` | sprintf 사용 (snprintf 권장) | `sprintf((char*)szLog, "Socket Connect IP:%s,Port:%d",` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:345` | sprintf 사용 (snprintf 권장) | `sprintf((char*)szLog, "link Server Close Socket");` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:512` | sprintf 사용 (snprintf 권장) | `sprintf(temp, "%s%s", pSocket->m_Rdata, Rdata);` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:514` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(temp, (LPCSTR)Rdata);` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:524` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(pSocket->m_Rdata, temp);` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:526` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy(pSocket->m_Rdata, "");` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:585` | strcpy 사용 (strncpy/strcpy_st 권장) | `strcpy((char*)Rdata, (LPCSTR)szRdata);` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:589` | sprintf 사용 (snprintf 권장) | `sprintf(temp, "%s%s", m_Rdata, Rdata);` |
| ... | ... | *172건 추가* | ... |

### 2.3 [Medium] (14건)

| 유형 | 파일:라인 | 설명 | 코드 |
|------|----------|------|------|
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:1369` | strcat 사용 (strncat 권장) | `if(strlen(szTmpUser) > 3) strcat(szTmpUser, "\|");` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:1370` | strcat 사용 (strncat 권장) | `strcat(szTmpUser, pApp->m_pSmsSiteUsers[j].sms_user_name);` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:1371` | strcat 사용 (strncat 권장) | `strcat(szTmpUser, "^");` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:1372` | strcat 사용 (strncat 권장) | `strcat(szTmpUser, pApp->m_pSmsSiteUsers[j].sms_user_phone);` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:1379` | strcat 사용 (strncat 권장) | `if(strlen(szTmpUser) > 3) strcat(szTmpUser, "\|");` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:1380` | strcat 사용 (strncat 권장) | `strcat(szTmpUser, pApp->m_pSmsSiteUsers[j].sms_user_name);` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:1381` | strcat 사용 (strncat 권장) | `strcat(szTmpUser, "^");` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:1382` | strcat 사용 (strncat 권장) | `strcat(szTmpUser, pApp->m_pSmsSiteUsers[j].sms_user_phone);` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:1559` | strcat 사용 (strncat 권장) | `strcat(szLog, format);` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:1584` | strcat 사용 (strncat 권장) | `strcat(szLog, format);` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:1611` | strcat 사용 (strncat 권장) | `strcat(szLog, format);` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:1640` | strcat 사용 (strncat 권장) | `strcat(szLog, format);` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:1666` | strcat 사용 (strncat 권장) | `strcat(szLog, format);` |
| BUFFER_OVERFLOW | `TrsSocketServer.cpp:1693` | strcat 사용 (strncat 권장) | `strcat(szLog, format);` |

---

## 3. 코드 품질 이슈 (62건)

### 3.1 [High] (2건)

| 유형 | 파일:라인 | 설명 | 코드 |
|------|----------|------|------|
| A1_NULL_CHECK | `DirectODBC.cpp:652` | [A1] 메모리 할당 후 NULL 체크 누락 | `pEMailuser = (EMAIL_SITE_USERS*)malloc( sizeof(EMAIL_SITE_USERS) * (nCount + 2))` |
| A1_NULL_CHECK | `DirectODBC.cpp:757` | [A1] 메모리 할당 후 NULL 체크 누락 | `pSmsExclude = (SMS_EXCLUDE*)malloc( sizeof(SMS_EXCLUDE) * (nCount + 2));` |

### 3.2 [Medium] (39건)

| 유형 | 파일:라인 | 설명 | 코드 |
|------|----------|------|------|
| A1_NEW_NULL | `AlramWeb.cpp:111` | [A1] new 할당 후 NULL 체크 누락 | `ALRAM_OBJ* pAlObj = new ALRAM_OBJ;` |
| A1_NEW_NULL | `AlramWeb.cpp:136` | [A1] new 할당 후 NULL 체크 누락 | `ALRAM_OBJ* pAlObj = new ALRAM_OBJ;` |
| H2_DB_RETURN | `DirectODBC.cpp:162` | [H2] DB 함수 반환값 미체크 | `int CDirectODBC::OpenTable(CString SqlCmd)` |
| H2_DB_RETURN | `DirectODBC.cpp:212` | [H2] DB 함수 반환값 미체크 | `int CDirectODBC::OpenTable(CString SqlCmd, HENV &m_localEnv, HDBC &m_localDbc, 	` |
| H2_DB_RETURN | `DirectODBC.cpp:231` | [H2] DB 함수 반환값 미체크 | `int CDirectODBC::ExecSql(CString SqlCmd)` |
| A3_USE_AFTER_FREE | `DirectODBC.cpp:454` | [A3] free/delete 후 포인터 NULL 미설정 | `delete pApp->m_pSmsSiteUsers;` |
| A1_NEW_NULL | `DirectODBC.cpp:457` | [A1] new 할당 후 NULL 체크 누락 | `pApp->m_pSmsSiteUsers = new SMS_SITE_USERS[nUserCnt+1];` |
| B4_NO_INIT | `MyDefine.h:325` | [B4] 구조체 CALL_INFO 초기화 누락 (memset 없음) | `CALL_INFO	useCallInfo;` |
| B4_NO_INIT | `MyDefine.h:326` | [B4] 구조체 CALL_INFO 초기화 누락 (memset 없음) | `CALL_INFO	holdCallInfo;` |
| B5_ARRAY_BOUND | `SystemTray.cpp:244` | [B5] 배열 경계 <= 비교 (확인 필요) | `for (UINT i = 0; i <= nNumIcons; i++)` |
| H2_DB_RETURN | `TrsSocketServer.cpp:256` | [H2] DB 함수 반환값 미체크 | `m_dlgSql->ExecSql(strSql);` |
| H2_DB_RETURN | `TrsSocketServer.cpp:259` | [H2] DB 함수 반환값 미체크 | `m_dlgSql->ExecSql(strSql1);` |
| A1_NEW_NULL | `TrsSocketServer.cpp:278` | [A1] new 할당 후 NULL 체크 누락 | `CClientSocket* pSocket = new CClientSocket();` |
| H2_DB_RETURN | `TrsSocketServer.cpp:716` | [H2] DB 함수 반환값 미체크 | `m_dlgSql->ExecSql(strSql);` |
| F2_PARTIAL_SEND | `TrsSocketServer.cpp:723` | [F2] Send/Recv 반환값 미체크 (부분 송수신) | `pSocket->Send("Close", 5);` |
| F2_PARTIAL_SEND | `TrsSocketServer.cpp:726` | [F2] Send/Recv 반환값 미체크 (부분 송수신) | `pSocket->Send("Run", 3);` |
| F2_PARTIAL_SEND | `TrsSocketServer.cpp:739` | [F2] Send/Recv 반환값 미체크 (부분 송수신) | `pSocket->Send(Sdata, strlen(Sdata));` |
| F2_PARTIAL_SEND | `TrsSocketServer.cpp:751` | [F2] Send/Recv 반환값 미체크 (부분 송수신) | `pSocket->Send(Sdata, strlen((LPCSTR)Sdata));` |
| H2_DB_RETURN | `TrsSocketServer.cpp:858` | [H2] DB 함수 반환값 미체크 | `m_dlgSql->ExecSql(strSql);` |
| H2_DB_RETURN | `TrsSocketServer.cpp:1027` | [H2] DB 함수 반환값 미체크 | `m_dlgSql->ExecSql(strSql);` |
| H2_DB_RETURN | `TrsSocketServer.cpp:1046` | [H2] DB 함수 반환값 미체크 | `m_dlgSql->ExecSql(strSql4);` |
| H2_DB_RETURN | `TrsSocketServer.cpp:1081` | [H2] DB 함수 반환값 미체크 | `m_dlgSql->ExecSql(strSql4);` |
| H2_DB_RETURN | `TrsSocketServer.cpp:1111` | [H2] DB 함수 반환값 미체크 | `m_dlgSql->ExecSql(strSql4);` |
| A1_NEW_NULL | `TrsSocketServer.cpp:1230` | [A1] new 할당 후 NULL 체크 누락 | `SMS_SITE_BASIC_USER *pBasicUser = new SMS_SITE_BASIC_USER[nUserBasicCnt+1];` |
| H2_DB_RETURN | `TrsSocketServer.cpp:1236` | [H2] DB 함수 반환값 미체크 | `m_dlgSql->ExecSql(strSql);` |
| H2_DB_RETURN | `TrsSocketServer.cpp:1243` | [H2] DB 함수 반환값 미체크 | `m_dlgSql->ExecSql(strSql);` |
| H2_DB_RETURN | `TrsSocketServer.cpp:1442` | [H2] DB 함수 반환값 미체크 | `m_dlgSql->ExecSql(strInitSql);` |
| A1_NEW_NULL | `TrsSocketServer.cpp:1455` | [A1] new 할당 후 NULL 체크 누락 | `pCCLink = new CC_LINKINFO[nCnt];` |
| H2_DB_RETURN | `TrsSocketServer.cpp:1520` | [H2] DB 함수 반환값 미체크 | `m_smsSql->ExecSql(strSql5);` |
| H2_DB_RETURN | `TrsSocketServer.cpp:1524` | [H2] DB 함수 반환값 미체크 | `m_dlgSql->ExecSql(strSql6);` |
| H2_DB_RETURN | `TrsSocketServer.cpp:1764` | [H2] DB 함수 반환값 미체크 | `m_dlgSql->ExecSql(strSql);` |
| F2_PARTIAL_SEND | `TrsSocketServer.cpp:1800` | [F2] Send/Recv 반환값 미체크 (부분 송수신) | `ret = pSocket->Send(Sdata, strlen((LPCSTR)Sdata));` |
| F2_PARTIAL_SEND | `TrsSocketServer.cpp:1804` | [F2] Send/Recv 반환값 미체크 (부분 송수신) | `sprintf((char*)szLog, "Send(%d) [%s:%d] %s", ret, strIP, uPort, Sdata);` |
| A1_NEW_NULL | `TrsSocketServer.cpp:1827` | [A1] new 할당 후 NULL 체크 누락 | `m_pAlramWeb = new CAlramWeb;` |
| A1_NEW_NULL | `TrsSocketServerDlg.cpp:216` | [A1] new 할당 후 NULL 체크 누락 | `pApp->m_dlgSql = new CDirectODBC((unsigned char*)"ipcs_log", (unsigned char*)"li` |
| A1_NEW_NULL | `TrsSocketServerDlg.cpp:220` | [A1] new 할당 후 NULL 체크 누락 | `pApp->m_pAtalkSql = new CDirectODBC((unsigned char*)"onpia_check", (unsigned cha` |
| A1_NEW_NULL | `TrsSocketServerDlg.cpp:223` | [A1] new 할당 후 NULL 체크 누락 | `pApp->m_smsSql = new CDirectODBC((unsigned char*)"onpia_sms", (unsigned char*)"l` |
| F2_PARTIAL_SEND | `TrsSocketServerDlg.cpp:332` | [F2] Send/Recv 반환값 미체크 (부분 송수신) | `m_sock.Send(send_buf, strlen(send_buf));` |
| A1_NEW_NULL | `URLEncode.cpp:151` | [A1] new 할당 후 NULL 체크 누락 | `char* buff = new char[len + 1];` |

### 3.3 [Low] (21건)

| 유형 | 파일:라인 | 설명 | 코드 |
|------|----------|------|------|
| E1_INFINITE_LOOP | `AlramWeb.cpp:39` | [E1] 무한루프 - break/return 조건 확인 필요 | `for(;;){` |
| D3_CHECK_CLOSETABLE | `DirectODBC.cpp:162` | [D3] OpenTable 호출 - CloseTable 쌍 확인 필요 | `int CDirectODBC::OpenTable(CString SqlCmd)` |
| D3_CHECK_CLOSETABLE | `DirectODBC.cpp:212` | [D3] OpenTable 호출 - CloseTable 쌍 확인 필요 | `int CDirectODBC::OpenTable(CString SqlCmd, HENV &m_localEnv, HDBC &m_localDbc, 	` |
| D3_CHECK_CLOSETABLE | `DirectODBC.cpp:355` | [D3] OpenTable 호출 - CloseTable 쌍 확인 필요 | `switch(OpenTable(sqlCmd)){` |
| D3_CHECK_CLOSETABLE | `DirectODBC.cpp:390` | [D3] OpenTable 호출 - CloseTable 쌍 확인 필요 | `switch(OpenTable(sqlCmd)){` |
| D3_CHECK_CLOSETABLE | `DirectODBC.cpp:422` | [D3] OpenTable 호출 - CloseTable 쌍 확인 필요 | `switch (OpenTable(sqlCmd)) {` |
| D3_CHECK_CLOSETABLE | `DirectODBC.cpp:460` | [D3] OpenTable 호출 - CloseTable 쌍 확인 필요 | `switch(OpenTable(sqlCmd)){` |
| D3_CHECK_CLOSETABLE | `DirectODBC.cpp:523` | [D3] OpenTable 호출 - CloseTable 쌍 확인 필요 | `switch(OpenTable(sqlCmd)){` |
| D3_CHECK_CLOSETABLE | `DirectODBC.cpp:582` | [D3] OpenTable 호출 - CloseTable 쌍 확인 필요 | `switch(OpenTable(sqlCmd)){` |
| D3_CHECK_CLOSETABLE | `DirectODBC.cpp:659` | [D3] OpenTable 호출 - CloseTable 쌍 확인 필요 | `switch(OpenTable(szQuery)){` |
| D3_CHECK_CLOSETABLE | `DirectODBC.cpp:764` | [D3] OpenTable 호출 - CloseTable 쌍 확인 필요 | `switch(OpenTable(szQuery)){` |
| I2_STRING_TIME | `TrsSocketServer.cpp:152` | [I2] 문자열 기반 시간 비교 - 형식 불일치 시 오류 가능 | `if(strcmp(m_pSmsExclude[i].sm_stime, szCurrentTime) <= 0 && strcmp(szCurrentTime` |
| UNSAFE_CONVERT | `TrsSocketServer.cpp:653` | atoi 사용 (strtol 권장 - 에러 감지) | `ret = atoi((LPCSTR)command);` |
| UNSAFE_CONVERT | `TrsSocketServer.cpp:733` | atoi 사용 (strtol 권장 - 에러 감지) | `if(atoi(command) == 3){` |
| UNSAFE_CONVERT | `TrsSocketServer.cpp:815` | atoi 사용 (strtol 권장 - 에러 감지) | `nIndex = atoi(szResult[0]);` |
| UNSAFE_CONVERT | `TrsSocketServer.cpp:816` | atoi 사용 (strtol 권장 - 에러 감지) | `nType = atoi(szResult[1]);` |
| I2_STRING_TIME | `TrsSocketServer.cpp:1094` | [I2] 문자열 기반 시간 비교 - 형식 불일치 시 오류 가능 | `if ((strcmp(szDay, szNasUpFailDate) != FALSE)) {` |
| I2_STRING_TIME | `TrsSocketServer.cpp:1108` | [I2] 문자열 기반 시간 비교 - 형식 불일치 시 오류 가능 | `if ((strcmp(szDay, szNasUpFailDate) != FALSE)) {` |
| UNSAFE_CONVERT | `TrsSocketServer.cpp:1315` | atoi 사용 (strtol 권장 - 에러 감지) | `nType =		atoi(szResult[1]);` |
| I2_STRING_TIME | `TrsSocketServer.cpp:1755` | [I2] 문자열 기반 시간 비교 - 형식 불일치 시 오류 가능 | `if(nLen == 8 && (strcmp(szToday, szValue) <= 0)){` |
| UNSAFE_CONVERT | `URLEncode.cpp:184` | atoi 사용 (strtol 권장 - 에러 감지) | `int tmp_i = atoi(tmp);` |

---

## 4. 스레드 분석

### 4.1 스레드 생성 (0개)

| 생성 위치 | 대상 함수 | 라인 |
|----------|----------|------|

### 4.2 뮤텍스 목록 (0개)


### 4.3 조건변수 목록 (0개)


---

## 5. 상태 전이 분석

---

## 6. DB 쿼리 패턴 (76건)

### 6.1 테이블 사용 빈도

| 테이블명 | 쿼리 수 |
|----------|---------|
| `CallCenter_Link` | 24 |
| `SDK_SMS_SEND` | 5 |
| `SITE_SMS_USER` | 4 |
| `SITE_SmsExcludeTime` | 2 |
| `SITE_EMAIL_USER` | 1 |
| `IPCS_LOG` | 1 |
| `lic_access_list` | 1 |
| `SITE_SMS_BASIC_USER` | 1 |
| `SITE_SMS_BASIC` | 1 |
| `onpia_reschk_log` | 1 |

### 6.2 쿼리 유형별 분포

| 유형 | 건수 |
|------|------|
| SELECT | 26 |
| INSERT | 22 |
| DELETE | 19 |
| UPDATE | 9 |

---

## 7. 설정값(bkCfg) 영향도 Top 30

| 설정값 | 참조 횟수 |
|--------|----------|

---

## 8. 대형 함수 Top 30

| 함수명 | 파일 | 라인 | 본문 크기 |
|--------|------|------|----------|
| **AlarmWorkerThread** | `AlramWeb.cpp` | 34 | 33줄 |
| **DbgOutLastError** | `DbgOut_S.h` | 85 | 30줄 |
| **trim** | `DirectODBC.cpp` | 16 | 20줄 |
| **DbgOut** | `DbgOut_S.h` | 59 | 14줄 |
| **DbgOutA** | `DbgOut_S.h` | 11 | 10줄 |
| **DbgOutW** | `DbgOut_S.h` | 35 | 10줄 |
| **CATCH** | `TrsSocketServer.cpp` | 532 | 2줄 |
| **CATCH** | `TrsSocketServer.cpp` | 569 | 2줄 |
| **CATCH** | `TrsSocketServer.cpp` | 609 | 2줄 |
| **Enabled** | `SystemTray.h` | 46 | 0줄 |
| **Visible** | `SystemTray.h` | 47 | 0줄 |
| **CURLEncode** | `URLEncode.h` | 22 | 0줄 |

### 파일별 순환 복잡도 Top 20

| 파일 | 복잡도 | 라인 | 함수 수 |
|------|--------|------|---------|
| `TrsSocketServer.cpp` | **242** | 1998 | 3 |
| `DirectODBC.cpp` | **91** | 828 | 1 |
| `AlramWeb.cpp` | **38** | 388 | 1 |
| `SystemTray.cpp` | **37** | 464 | 0 |
| `URLEncode.cpp` | **29** | 259 | 0 |
| `TrsSocketServerDlg.cpp` | **22** | 443 | 0 |
| `charconv.cpp` | **11** | 119 | 0 |
| `StdAfx.h` | **6** | 57 | 0 |
| `TrsSocketServer.h` | **6** | 126 | 0 |
| `ClientSocket.cpp` | **5** | 94 | 0 |
| `DbgOut_S.h` | **5** | 128 | 4 |
| `SystemTray.h` | **4** | 125 | 2 |
| `AlramWeb.h` | **3** | 99 | 0 |
| `ClientSocket.h` | **3** | 54 | 0 |
| `DirectODBC.h` | **3** | 100 | 0 |
| `ListenSocket.h` | **3** | 48 | 0 |
| `TrsSocketServerDlg.h` | **3** | 67 | 0 |
| `UdpSocket.h` | **3** | 82 | 0 |
| `ListenSocket.cpp` | **2** | 44 | 0 |
| `UdpSocket.cpp` | **2** | 44 | 0 |

---

## 9. 전역 변수 (extern) 목록 (0개)

| 타입 | 변수명 | 파일:라인 |
|------|--------|----------|

---

## 10. 파일별 통계 (라인 수 기준 상위)

| 파일 | 전체 | 코드 | 주석 | 함수 | 보안 | 품질 |
|------|------|------|------|------|------|------|
| `TrsSocketServer.cpp` | 1998 | 1454 | 83 | 3 | 194 | 33 |
| `DirectODBC.cpp` | 828 | 623 | 21 | 1 | 20 | 17 |
| `MyDefine.h` | 488 | 393 | 11 | 0 | 0 | 2 |
| `SystemTray.cpp` | 464 | 286 | 84 | 0 | 0 | 1 |
| `TrsSocketServerDlg.cpp` | 443 | 211 | 132 | 0 | 2 | 4 |
| `AlramWeb.cpp` | 388 | 216 | 69 | 1 | 8 | 3 |
| `URLEncode.cpp` | 259 | 204 | 25 | 0 | 10 | 2 |
| `DbgOut_S.h` | 128 | 87 | 6 | 4 | 0 | 0 |
| `TrsSocketServer.h` | 126 | 73 | 18 | 0 | 0 | 0 |
| `SystemTray.h` | 125 | 58 | 44 | 2 | 0 | 0 |
| `charconv.cpp` | 119 | 88 | 15 | 0 | 0 | 0 |
| `DirectODBC.h` | 100 | 55 | 18 | 0 | 0 | 0 |
| `AlramWeb.h` | 99 | 57 | 8 | 0 | 0 | 0 |
| `ClientSocket.cpp` | 94 | 65 | 12 | 0 | 2 | 0 |
| `UdpSocket.h` | 82 | 50 | 18 | 0 | 0 | 0 |
| `TrsSocketServerDlg.h` | 67 | 38 | 17 | 0 | 0 | 0 |
| `StdAfx.h` | 57 | 33 | 6 | 0 | 0 | 0 |
| `ClientSocket.h` | 54 | 22 | 18 | 0 | 0 | 0 |
| `ListenSocket.h` | 48 | 17 | 18 | 0 | 0 | 0 |
| `ListenSocket.cpp` | 44 | 24 | 10 | 0 | 0 | 0 |
| `UdpSocket.cpp` | 44 | 24 | 10 | 0 | 0 | 0 |
| `resource.h` | 35 | 27 | 6 | 0 | 0 | 0 |
| `URLEncode.h` | 33 | 22 | 5 | 1 | 0 | 0 |
| `charconv.h` | 15 | 11 | 0 | 0 | 0 | 0 |
| `StdAfx.cpp` | 9 | 1 | 3 | 0 | 0 | 0 |

---

## 11. 모듈 분류

| 모듈 유형 | 파일 수 | 파일 목록 |
|----------|---------|----------|
| **ShareLib** | 10 | AlramWeb.cpp, AlramWeb.h, DbgOut_S.h, StdAfx.cpp, StdAfx.h 외 5개 |
| **Socket_Util** | 8 | ClientSocket.cpp, ClientSocket.h, TrsSocketServer.cpp, TrsSocketServer.h, TrsSocketServerDlg.cpp 외 3개 |
| **Database** | 2 | DirectODBC.cpp, DirectODBC.h |
| **Queue_List** | 2 | ListenSocket.cpp, ListenSocket.h |
| **Config_System** | 2 | SystemTray.cpp, SystemTray.h |
| **Definition** | 1 | MyDefine.h |

---

## 12. 스레드-뮤텍스 매핑


---

## 13. 스레드-DB 매핑

| 파일 | DB 포인터 |
|------|----------|

---

## 14. 소켓 구조

| 파일 | 유형 | 상세 | 라인 |
|------|------|------|------|
| `TrsSocketServer.cpp` | accept |  | 282 |
| `TrsSocketServerDlg.cpp` | connect |  | 329 |

---

## 15. 리소스 쌍 불균형

리소스 쌍 불균형 없음

---

## 16. 상태 머신


---

## 17. 코드 중복 탐지

총 129건 (5줄 이상 동일 블록)

| 파일 | 라인1 | 라인2 |
|------|-------|-------|
| `AlramWeb.cpp` | 107 | 132 |
| `AlramWeb.cpp` | 117 | 144 |
| `AlramWeb.cpp` | 117 | 145 |
| `AlramWeb.cpp` | 119 | 146 |
| `AlramWeb.cpp` | 120 | 147 |
| `AlramWeb.cpp` | 121 | 148 |
| `AlramWeb.cpp` | 122 | 149 |
| `AlramWeb.cpp` | 122 | 150 |
| `AlramWeb.cpp` | 252 | 275 |
| `AlramWeb.cpp` | 253 | 276 |
| `AlramWeb.cpp` | 254 | 277 |
| `AlramWeb.cpp` | 255 | 278 |
| `DbgOut_S.h` | 12 | 36 |
| `DbgOut_S.h` | 19 | 43 |
| `DbgOut_S.h` | 61 | 90 |
| `DbgOut_S.h` | 62 | 91 |
| `DirectODBC.cpp` | 46 | 58 |
| `DirectODBC.cpp` | 120 | 128 |
| `DirectODBC.cpp` | 121 | 129 |
| `DirectODBC.cpp` | 169 | 196 |
| `DirectODBC.cpp` | 170 | 197 |
| `DirectODBC.cpp` | 171 | 198 |
| `DirectODBC.cpp` | 207 | 226 |
| `DirectODBC.cpp` | 195 | 234 |
| `DirectODBC.cpp` | 169 | 235 |
| `DirectODBC.cpp` | 241 | 250 |
| `DirectODBC.cpp` | 242 | 251 |
| `DirectODBC.cpp` | 195 | 266 |
| `DirectODBC.cpp` | 169 | 267 |
| `DirectODBC.cpp` | 236 | 268 |

---

## 18. 데드락 위험

데드락 위험 없음

---

## 19. 플랫폼 이식성

| 플랫폼 | #ifdef 수 |
|--------|----------|
| unknown | 56 |

---

## 20. 품질 메트릭


### 중첩 깊이 Top 10
| 파일 | 최대 깊이 |
|------|----------|
| `TrsSocketServer.cpp` | 8 |
| `DirectODBC.cpp` | 5 |
| `AlramWeb.cpp` | 4 |
| `URLEncode.cpp` | 4 |
| `ClientSocket.cpp` | 3 |
| `ClientSocket.h` | 3 |
| `DirectODBC.h` | 3 |
| `ListenSocket.h` | 3 |
| `SystemTray.cpp` | 3 |
| `SystemTray.h` | 3 |

### 주석 비율 하위 10 (10% 미만)
| 파일 | 주석 비율 | 코드 | 주석 |
|------|----------|------|------|
| `MyDefine.h` | 2.7% | 393 | 11 |
| `DirectODBC.cpp` | 3.3% | 623 | 21 |
| `TrsSocketServer.cpp` | 5.4% | 1454 | 83 |

---

> 분석 도구: NCBK C/C++ 심층 분석기 v2.0 (`create_deep_c_analysis.py`) - A~I 탐지 + 2단계 검증 + S/D/R/M 분석