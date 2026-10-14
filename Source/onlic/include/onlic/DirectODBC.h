/*
**	DirectODBC.h : unixODBC 직접 호출 래퍼 (onmw CDirectODBC 핵심만 이식, ODBC 3.x)
**	- 스레드당 핸들 1개 전제 (내부 뮤텍스 없음) - docs/리눅스참조구조.md §3
**	- 반환 규약: TRUE 성공 / FALSE 실패 (onmw 와 동일. Windows CDirectODBC 는 반대였음)
**	- 오토커밋 유지, 문장별 COMMIT 없음 (결정: 포팅영향분석 §8-4)
*/
#ifndef ___ONLIC_DIRECTODBC_H___
#define ___ONLIC_DIRECTODBC_H___

#include <sql.h>
#include <sqlext.h>

#define MAX_QUERY				4096
#define DB_LOGIN_TIMEOUT_SEC	30		/* Windows DirectODBC.cpp:110 SQL_LOGIN_TIMEOUT 30 */

class CDirectODBC
{
public:
	CDirectODBC();
	virtual ~CDirectODBC();

	/* 연결 */
	int		DbConnectODBC(const DATABASEINFO &dbInfo, const char* szClientEncoding = "");
	int		DbConnectODBC();
	void	DisConnectODBC();
	int		IsConnected() { return m_nConn; }
	void	SetExecRetry(int nRetry) { m_nExecRetry = nRetry; }

	/* 조회 (OpenTable → Fetch/Get* 반복 → CloseTable) */
	int		OpenTable(const char *sqlCmd);			/* 실패 시 재접속 후 1회 재시도 */
	int		OpenLastTable(const char *sqlCmd);		/* 1회 시도 */
	int		Fetch();								/* TRUE 행 있음 / FALSE 없음 / -1 오류 */
	int		GetStr(int nCol, char *szBuf, int nBufSize);	/* 앞뒤 공백 제거, 빈 값이면 "" */
	int		GetInt(int nCol, int &nValue);
	int		GetLong(int nCol, long &lValue);
	void	CloseTable();

	/* 실행 */
	int		ExecSql(const char *sqlCmd);			/* 연결 끊김(m_nConn=FALSE)일 때만 재접속 후 1회 재실행 (m_nExecRetry) */
	int		ExecLastSql(const char *sqlCmd);
	long	ExecSqlRows(const char *sqlCmd);		/* ExecSql + SQLRowCount. 반환 영향 행 수, 실패 -1 */
	long	ExecLastSqlRows(const char *sqlCmd);

	/* 단일 값 */
	long	RowCount(const char *sqlCmd);			/* 첫 컬럼 정수. 실패 -1 (Windows RowCount 와 동일) */
	int		GetValue(const char *sqlCmd, char *szValue, int nSize);	/* 첫 행 첫 컬럼 문자열. TRUE/FALSE */
	int		GetIntValue(const char *sqlCmd);		/* 첫 행 첫 컬럼 정수. 실패 -1 */

	/* 유틸 */
	static int	SqlEscape(const char *szSrc, char *szDst, int nDstSize);	/* ' → '' , 제어문자 제거 */
	const char*	GetName() { return m_szName; }

protected:
	void	LogSqlError(const char* szWhere, SQLSMALLINT nHandleType, SQLHANDLE hHandle, const char *sqlCmd);
	void	SetSessionProc();						/* client_encoding, statement_timeout */
	void	FreeHandles();

	SQLHENV		m_hEnv;
	SQLHDBC		m_hDbc;
	SQLHSTMT	m_hStmt;
	int			m_nConn;
	int			m_nExecRetry;
	char		m_szDSN[51], m_szUID[51], m_szPWD[51], m_szName[31];
	char		m_szClientEncoding[31];
	long		m_lStatementTimeout;				/* 초 */
	long		m_lLoginTimeout;					/* 초 (DATABASEINFO.lLoginTimeout, 0 이면 DB_LOGIN_TIMEOUT_SEC) */
	int			m_nDbType;							/* DB_TYPE_PG / DB_TYPE_MSSQL */
};

#endif /* DirectODBC.h */
