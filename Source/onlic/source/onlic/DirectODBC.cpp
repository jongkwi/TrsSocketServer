/*
**	DirectODBC.cpp : unixODBC 직접 호출 래퍼 (onmw CDirectODBC 핵심 이식)
**	Windows 원본(DirectODBC.cpp, ODBC 2.x) 과의 차이
**	  - SQLAllocHandle/SQLFreeHandle (3.x), SQL_ATTR_ODBC_VERSION 설정
**	  - 접속 직후 set client_encoding / SET statement_timeout (PostgreSQL)
**	  - 매 문장 후 "COMMIT" 실행 제거 (오토커밋)
**	  - 실패 시 재접속 + 재실행 (원본은 재접속만 하고 실패 반환)
**	  - 오류는 SQLGetDiagRec 로 sqlerr 로그에 SQLSTATE/메시지/쿼리 기록
**	2026-10-14 최종 검증 반영: 재접속은 연결 끊김(08xxx)일 때만 / SQLDisconnect 무조건 호출(핸들 누수) /
**	  SqlEscape 는 CR·LF·TAB 통과(알림톡 줄바꿈 보존) / client_encoding 값 검증 / MSSQL 핸들에는 PG 세션 설정 생략 / 로그인 타임아웃 핸들별
*/
#include "onlicDefine.h"
#include <ctype.h>

CDirectODBC::CDirectODBC()
{
	m_hEnv = SQL_NULL_HENV;
	m_hDbc = SQL_NULL_HDBC;
	m_hStmt = SQL_NULL_HSTMT;
	m_nConn = FALSE;
	m_nExecRetry = TRUE;
	memset_st(m_szDSN, 0, sizeof(m_szDSN));
	memset_st(m_szUID, 0, sizeof(m_szUID));
	memset_st(m_szPWD, 0, sizeof(m_szPWD));
	memset_st(m_szName, 0, sizeof(m_szName));
	memset_st(m_szClientEncoding, 0, sizeof(m_szClientEncoding));
	m_lStatementTimeout = 0;
	m_lLoginTimeout = DB_LOGIN_TIMEOUT_SEC;
	m_nDbType = DB_TYPE_PG;
}

CDirectODBC::~CDirectODBC()
{
	DisConnectODBC();
}

int CDirectODBC::DbConnectODBC(const DATABASEINFO &dbInfo, const char* szClientEncoding)
{
	strncpy_st(m_szDSN, dbInfo.szDSN, sizeof(m_szDSN)-1);
	strncpy_st(m_szUID, dbInfo.szUID, sizeof(m_szUID)-1);
	strncpy_st(m_szPWD, dbInfo.szPWD, sizeof(m_szPWD)-1);
	strncpy_st(m_szName, dbInfo.szName, sizeof(m_szName)-1);
	strncpy_st(m_szClientEncoding, szClientEncoding, sizeof(m_szClientEncoding)-1);
	m_lStatementTimeout = dbInfo.lStatementTimeout;
	m_lLoginTimeout = (dbInfo.lLoginTimeout > 0) ? dbInfo.lLoginTimeout : DB_LOGIN_TIMEOUT_SEC;
	m_nDbType = dbInfo.nDbType;
	return DbConnectODBC();
}

/*
**	접속. 실패해도 핸들을 정리하고 FALSE (호출측이 기동 계속 여부 결정 - Windows 와 동일하게 실패해도 기동)
*/
int CDirectODBC::DbConnectODBC()
{
	SQLRETURN ret;

	DisConnectODBC();

	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
	if(!SQL_SUCCEEDED(ret)){
		m_hEnv = SQL_NULL_HENV;
		OnlicLogf(LOG_SQLERR, "[%s] SQLAllocHandle(ENV) fail ret:%d", m_szName, ret);
		return FALSE;
	}
	SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

	ret = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc);
	if(!SQL_SUCCEEDED(ret)){
		LogSqlError("SQLAllocHandle(DBC)", SQL_HANDLE_ENV, m_hEnv, "");
		FreeHandles();
		return FALSE;
	}
	SQLSetConnectAttr(m_hDbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER)(size_t)m_lLoginTimeout, 0);

	ret = SQLConnect(m_hDbc, (SQLCHAR*)m_szDSN, SQL_NTS, (SQLCHAR*)m_szUID, SQL_NTS, (SQLCHAR*)m_szPWD, SQL_NTS);
	if(!SQL_SUCCEEDED(ret)){
		LogSqlError("SQLConnect", SQL_HANDLE_DBC, m_hDbc, m_szDSN);
		FreeHandles();
		return FALSE;
	}

	m_nConn = TRUE;
	SetSessionProc();
	OnlicLogf(LOG_SOFT, "[%s] db connect ok dsn:%s uid:%s", m_szName, m_szDSN, m_szUID);
	return TRUE;
}

/*
**	세션 설정: CP949 유지 결정에 따라 client_encoding=UHC (ini [DATABASE_INFO] CLIENT_ENCODING. PG 의 EUC_KR 은 완성형 2,350자만이라 CP949 확장 한글 불가)
**	statement_timeout 은 초 → ms. 6321 응답 5초 제한 때문에 라이선스 DB 는 짧게
*/
void CDirectODBC::SetSessionProc()
{
	char szQuery[256] = "";
	int i = 0, nValid = TRUE;
#ifdef _DB_TYPE_POSTGRES
	if(m_nDbType != DB_TYPE_PG) return;		/* MSSQL(SMS) 핸들: PG 전용 세션 설정을 보내지 않음 (B9) */
	if(strlen_st(m_szClientEncoding) > 0){
		for(i = 0; m_szClientEncoding[i] != 0; i++){	/* ini 값이 SQL 에 그대로 들어가므로 [A-Za-z0-9_] 만 허용 */
			if(!isalnum((unsigned char)m_szClientEncoding[i]) && m_szClientEncoding[i] != '_'){ nValid = FALSE; break; }
		}
		if(nValid == TRUE){
			nprintf(szQuery, 255, "set client_encoding to '%s'", m_szClientEncoding);
			ExecLastSql(szQuery);
		}else{
			OnlicLogf(LOG_SQLERR, "[%s] CLIENT_ENCODING '%s' invalid (A-Z 0-9 _ only) - skipped", m_szName, m_szClientEncoding);
		}
	}
	if(m_lStatementTimeout > 0){
		nprintf(szQuery, 255, "SET statement_timeout TO %ld", m_lStatementTimeout * 1000L);
		ExecLastSql(szQuery);
	}
#endif
}

void CDirectODBC::FreeHandles()
{
	if(m_hStmt != SQL_NULL_HSTMT){ SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt); m_hStmt = SQL_NULL_HSTMT; }
	if(m_hDbc != SQL_NULL_HDBC){ SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc); m_hDbc = SQL_NULL_HDBC; }
	if(m_hEnv != SQL_NULL_HENV){ SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv); m_hEnv = SQL_NULL_HENV; }
	m_nConn = FALSE;
}

void CDirectODBC::DisConnectODBC()
{
	CloseTable();
	if(m_hDbc != SQL_NULL_HDBC) SQLDisconnect(m_hDbc);	/* m_nConn 과 무관하게 호출: 08xxx 뒤 생략하면 DBC/ENV 해제가 HY010 으로 실패해 핸들·소켓 누수 (B3) */
	FreeHandles();
}

/*
**	SELECT 열기. 연결이 끊긴 경우(m_nConn=FALSE)에만 재접속 후 1회 더 시도 (onmw OpenTable 패턴 + B5)
*/
int CDirectODBC::OpenTable(const char *sqlCmd)
{
	int ret = OpenLastTable(sqlCmd);
	if(ret == TRUE) return TRUE;
	if(m_nConn == TRUE) return FALSE;			/* 데이터·문법 오류(22xxx, 42xxx, 23xxx): 재접속해도 같은 결과 → 즉시 실패 (B5) */
	if(DbConnectODBC() == FALSE) return FALSE;	/* 연결 끊김(08xxx)·핸들 오류만 재접속 후 1회 재시도 */
	return OpenLastTable(sqlCmd);
}

int CDirectODBC::OpenLastTable(const char *sqlCmd)
{
	SQLRETURN ret;

	CloseTable();
	if(m_hDbc == SQL_NULL_HDBC || m_nConn == FALSE) return FALSE;

	ret = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);
	if(!SQL_SUCCEEDED(ret)){
		LogSqlError("SQLAllocHandle(STMT)", SQL_HANDLE_DBC, m_hDbc, sqlCmd);
		m_hStmt = SQL_NULL_HSTMT;
		m_nConn = FALSE;
		return FALSE;
	}
	ret = SQLExecDirect(m_hStmt, (SQLCHAR*)sqlCmd, SQL_NTS);
	if(!SQL_SUCCEEDED(ret) && ret != SQL_NO_DATA){
		LogSqlError("OpenTable", SQL_HANDLE_STMT, m_hStmt, sqlCmd);
		CloseTable();
		return FALSE;
	}
	return TRUE;
}

int CDirectODBC::Fetch()
{
	SQLRETURN ret;
	if(m_hStmt == SQL_NULL_HSTMT) return -1;
	ret = SQLFetch(m_hStmt);
	if(SQL_SUCCEEDED(ret)) return TRUE;
	if(ret == SQL_NO_DATA) return FALSE;
	LogSqlError("SQLFetch", SQL_HANDLE_STMT, m_hStmt, "");
	return -1;
}

/*
**	문자열 컬럼. NULL 이면 "" (Windows SQLGetData 후 trim 과 동일 효과)
**	버퍼보다 긴 값은 절단(SQL_SUCCESS_WITH_INFO) - 원본과 동일하게 오류로 보지 않음
*/
int CDirectODBC::GetStr(int nCol, char *szBuf, int nBufSize)
{
	SQLLEN cbLen = 0;
	SQLRETURN ret;
	if(szBuf == NULL || nBufSize <= 0) return FALSE;
	szBuf[0] = 0;
	if(m_hStmt == SQL_NULL_HSTMT) return FALSE;
	ret = SQLGetData(m_hStmt, (SQLUSMALLINT)nCol, SQL_C_CHAR, szBuf, (SQLLEN)nBufSize, &cbLen);
	if(!SQL_SUCCEEDED(ret)){ szBuf[0] = 0; return FALSE; }
	if(cbLen == SQL_NULL_DATA){ szBuf[0] = 0; return TRUE; }
	szBuf[nBufSize-1] = 0;
	trim(szBuf);
	return TRUE;
}

int CDirectODBC::GetInt(int nCol, int &nValue)
{
	SQLLEN cbLen = 0;
	SQLINTEGER v = 0;
	SQLRETURN ret;
	nValue = 0;
	if(m_hStmt == SQL_NULL_HSTMT) return FALSE;
	ret = SQLGetData(m_hStmt, (SQLUSMALLINT)nCol, SQL_C_SLONG, &v, 0, &cbLen);
	if(!SQL_SUCCEEDED(ret)) return FALSE;
	if(cbLen == SQL_NULL_DATA) return TRUE;
	nValue = (int)v;
	return TRUE;
}

int CDirectODBC::GetLong(int nCol, long &lValue)
{
	SQLLEN cbLen = 0;
	SQLBIGINT v = 0;
	SQLRETURN ret;
	lValue = 0;
	if(m_hStmt == SQL_NULL_HSTMT) return FALSE;
	ret = SQLGetData(m_hStmt, (SQLUSMALLINT)nCol, SQL_C_SBIGINT, &v, 0, &cbLen);
	if(!SQL_SUCCEEDED(ret)) return FALSE;
	if(cbLen == SQL_NULL_DATA) return TRUE;
	lValue = (long)v;
	return TRUE;
}

void CDirectODBC::CloseTable()
{
	if(m_hStmt != SQL_NULL_HSTMT){
		SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
		m_hStmt = SQL_NULL_HSTMT;
	}
}

/*
**	INSERT/UPDATE. 연결 끊김일 때만 재접속 후 1회 재실행 (m_nExecRetry, B5). 재실행도 실패하면 FALSE
**	주의: 네트워크 단절로 응답만 잃은 경우 중복 INSERT 가능 (onmw db_exec_last_query 와 같은 선택)
*/
int CDirectODBC::ExecSql(const char *sqlCmd)
{
	return (ExecSqlRows(sqlCmd) >= 0) ? TRUE : FALSE;
}

int CDirectODBC::ExecLastSql(const char *sqlCmd)
{
	return (ExecLastSqlRows(sqlCmd) >= 0) ? TRUE : FALSE;
}

/*
**	ExecSql + 영향 행 수. 연결 끊김(m_nConn=FALSE)일 때만 재접속 후 1회 재실행 (B5). 반환 행 수(SQL_NO_DATA 는 0), 실패 -1
*/
long CDirectODBC::ExecSqlRows(const char *sqlCmd)
{
	long lRows = ExecLastSqlRows(sqlCmd);
	if(lRows >= 0) return lRows;
	if(m_nExecRetry != TRUE) return -1;
	if(m_nConn == TRUE) return -1;				/* 데이터·문법 오류: 재접속하지 않음 */
	if(DbConnectODBC() == FALSE) return -1;
	return ExecLastSqlRows(sqlCmd);
}

long CDirectODBC::ExecLastSqlRows(const char *sqlCmd)
{
	SQLRETURN ret;
	SQLHSTMT hStmt = SQL_NULL_HSTMT;
	SQLLEN nRows = 0;

	if(m_hDbc == SQL_NULL_HDBC || m_nConn == FALSE) return -1;

	ret = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &hStmt);
	if(!SQL_SUCCEEDED(ret)){
		LogSqlError("SQLAllocHandle(STMT)", SQL_HANDLE_DBC, m_hDbc, sqlCmd);
		m_nConn = FALSE;
		return -1;
	}
	ret = SQLExecDirect(hStmt, (SQLCHAR*)sqlCmd, SQL_NTS);
	if(ret == SQL_NO_DATA){
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return 0;
	}
	if(!SQL_SUCCEEDED(ret)){
		LogSqlError("ExecSql", SQL_HANDLE_STMT, hStmt, sqlCmd);
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return -1;
	}
	if(!SQL_SUCCEEDED(SQLRowCount(hStmt, &nRows)) || nRows < 0) nRows = 0;
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return (long)nRows;
}

/*
**	SELECT COUNT(...) 전용. 실패 -1 (Windows: nCnt <= 0 → Close 이므로 -1 이면 Close. 결정 B2 현행 유지)
*/
long CDirectODBC::RowCount(const char *sqlCmd)
{
	long lCount = -1;
	char szTemp[32] = "";

	if(OpenTable(sqlCmd) != TRUE) return -1;
	if(Fetch() == TRUE){
		if(GetStr(1, szTemp, sizeof(szTemp)) == TRUE) lCount = atol(szTemp);
		else lCount = -1;
	}else{
		lCount = 0;
	}
	CloseTable();
	return lCount;
}

int CDirectODBC::GetValue(const char *sqlCmd, char *szValue, int nSize)
{
	int nRet = FALSE;
	if(szValue == NULL || nSize <= 0) return FALSE;
	szValue[0] = 0;
	if(OpenTable(sqlCmd) != TRUE) return FALSE;
	if(Fetch() == TRUE){
		nRet = GetStr(1, szValue, nSize);
	}
	CloseTable();
	return nRet;
}

int CDirectODBC::GetIntValue(const char *sqlCmd)
{
	int nValue = -1;
	if(OpenTable(sqlCmd) != TRUE) return -1;
	if(Fetch() == TRUE){
		if(GetInt(1, nValue) != TRUE) nValue = -1;
	}
	CloseTable();
	return nValue;
}

/*
**	SQL 리터럴 이스케이프 (결정: 결함 B - SQL 인젝션 수정). ' → '' , CR/LF/TAB 은 통과(알림톡 문구 줄바꿈 보존, 원본 동일), 그 외 0x00~0x1F 제거 (B4)
**	CP949 2바이트 문자의 뒷바이트에 0x27 은 없으므로 바이트 단위 치환 안전
*/
int CDirectODBC::SqlEscape(const char *szSrc, char *szDst, int nDstSize)
{
	int i = 0, j = 0;
	unsigned char c;
	if(szDst == NULL || nDstSize <= 0) return FALSE;
	szDst[0] = 0;
	if(szSrc == NULL) return TRUE;
	for(i = 0; szSrc[i] != 0 && j < nDstSize - 1; i++){
		c = (unsigned char)szSrc[i];
		if(c == '\''){
			if(j + 2 >= nDstSize) break;
			szDst[j++] = '\''; szDst[j++] = '\'';
		}else if(c == '\r' || c == '\n' || c == '\t'){
			szDst[j++] = (char)c;			/* 따옴표 리터럴 안에서 합법. TXT_TABLE_CHECK/TXT_NEW_SITE 의 줄바꿈 유지 */
		}else if(c < 0x20){
			continue;
		}else{
			szDst[j++] = (char)c;
		}
	}
	szDst[j] = 0;
	return TRUE;
}

/*
**	Windows SqlErrLogging 대응: SQLSTATE, native, 메시지, 쿼리를 sqlerr 로그에 기록
*/
void CDirectODBC::LogSqlError(const char* szWhere, SQLSMALLINT nHandleType, SQLHANDLE hHandle, const char *sqlCmd)
{
	SQLCHAR szState[8] = "", szMsg[512] = "";
	SQLINTEGER nNative = 0;
	SQLSMALLINT nLen = 0;
	SQLRETURN ret;
	int i = 1;

	if(hHandle == SQL_NULL_HANDLE){
		OnlicLogf(LOG_SQLERR, "[%s] %s : null handle / %s", m_szName, szWhere, sqlCmd);
		return;
	}
	while(TRUE){
		ret = SQLGetDiagRec(nHandleType, hHandle, (SQLSMALLINT)i, szState, &nNative, szMsg, sizeof(szMsg)-1, &nLen);
		if(!SQL_SUCCEEDED(ret)) break;
		OnlicLogf(LOG_SQLERR, "[%s] %s : [%s](%d) %s / %s", m_szName, szWhere, szState, (int)nNative, szMsg, sqlCmd);
		/* 연결 끊김 계열이면 재접속 대상으로 표시 (08xxx, HYT00 timeout, HY000) */
		if(szState[0] == '0' && szState[1] == '8') m_nConn = FALSE;
		if(++i > 5) break;
	}
	if(i == 1) OnlicLogf(LOG_SQLERR, "[%s] %s : (no diag) / %s", m_szName, szWhere, sqlCmd);
}
