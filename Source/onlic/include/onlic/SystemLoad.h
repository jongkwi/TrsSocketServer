/*
**	SystemLoad.h : 기동 순서 (onmw CSystemLoad 패턴)
**	Windows: CTrsSocketServerDlg::OnTimer(START_TIMER) → DB 연결 3개 → SmsExcludeLoadProc → CreateSocketProc → LINK_TIMER
*/
#ifndef ___ONLIC_SYSTEMLOAD_H___
#define ___ONLIC_SYSTEMLOAD_H___

class CSystemLoad
{
public:
	CSystemLoad();
	virtual ~CSystemLoad();

	int		StartSystem(const char *szConfig);
	int		GetConfigFileFatchProc();
	int		CheckRoleProc();
	int		SystemDBConnectProc();
	int		LoadSocketProc();
	void	LogObj_Create();
	void	LogObj_SetDay();
	void	PrintSystemError(int error, int nLogWrite = LOG_YN_FILE, const char* szAddMsg = "");
	void	KillSystemMemory();
	void	ConfigLogWrite();

	char	m_szConfig[256];
	char	m_szLogPath[256];
	int		m_nLogView;				/* 포그라운드 실행 시 화면 출력 */

	/* 메인 스레드용 DB 핸들 (워커/타이머는 각자 핸들 보유 - 리눅스참조구조 §3) */
	CDirectODBC	*m_pLicDB;			/* ipcs_log  */
	CDirectODBC	*m_pAtalkDB;		/* onpia_check */
	CDirectODBC	*m_pSmsDB;			/* onpia_sms */

	/* SITE_SmsExcludeTime 캐시 (기동 시 1회 로드, Windows 동일) */
	SMS_EXCLUDE	*m_pSmsExclude;
	int			m_nSmsExcludeCnt;
};

#endif /* SystemLoad.h */
