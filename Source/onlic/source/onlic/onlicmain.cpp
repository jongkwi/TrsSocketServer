/*
**	onlicmain.cpp : onlic 진입점 (onmw ncmwmain.cpp 패턴)
**	Windows: CTrsSocketServerApp::InitInstance + CTrsSocketServerDlg (트레이 상주) 의 데몬 대체
*/
#include "onlicDefine.h"
#include "onlicmain.h"
#include <syslog.h>
#include <getopt.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/resource.h>
#ifdef _CURL
#include <curl/curl.h>
#endif

static void	quit_func(int);
static void pipe_func(int);

/* 시그널 핸들러는 플래그만 설정 (async-signal-safe). 로그·syslog 는 메인 루프에서 (L4, 2026-10-14) */
static volatile sig_atomic_t g_sig_quit = 0;
static volatile sig_atomic_t g_sig_pipe = 0;

/* fd 상한을 hard limit 까지 올린다 (세션 누적 대비). 결과는 StartSystem 뒤 로그 */
static void RaiseFdLimit(long *pSoft, long *pHard)
{
	struct rlimit rl;
	*pSoft = *pHard = -1;
	if(getrlimit(RLIMIT_NOFILE, &rl) != 0) return;
	if(rl.rlim_cur < rl.rlim_max){ rl.rlim_cur = rl.rlim_max; setrlimit(RLIMIT_NOFILE, &rl); }
	if(getrlimit(RLIMIT_NOFILE, &rl) == 0){ *pSoft = (long)rl.rlim_cur; *pHard = (long)rl.rlim_max; }
}

/* 데몬: 기동 메시지까지 터미널에 보여 준 뒤 0/1/2 를 /dev/null 로 (파이프로 출력을 잡는 래퍼가 멈추지 않게, L11) */
static void DetachStdio()
{
	int fd = open("/dev/null", O_RDWR);
	if(fd < 0) return;
	dup2(fd, 0); dup2(fd, 1); dup2(fd, 2);
	if(fd > 2) close(fd);
}

/////////////////////////////////////////////////////////////////////////////
//	main
/////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	char cpath[256] = "", szDefine[501] = "";
	int  rcode, isExit = FALSE, nRet = FALSE;
	long lFdSoft = -1, lFdHard = -1;

	GetCompileDefineString(szDefine);
	strcpy_st(cpath, DEFAULT_CONFIG_FILE);
	onlic_pid = getpid();

	while((rcode = getopt(argc, argv, ":c:C:dDvVhHp:P:l")) != EOF){
		switch(rcode){
		  case 'd': case 'D':
			if(daemon_init(argv[0], 0) >= 0) daemon_proc = 1;
			onlic_pid = getpid();			/* onmw 는 daemon_init 반환값(0) 을 pid 로 씀 (결함) */
			break;
		  case 'c': case 'C':
			if(optarg != NULL) strcpy_st(cpath, optarg);
			else { printf("Argument config error!\n"); isExit = TRUE; }
			break;
		  case 'p': case 'P':
			if(optarg != NULL) strcpy_st(m_gPidFile, optarg);
			else { printf("PID File config error!\n"); isExit = TRUE; }
			break;
		  case 'l':
			sys_load.m_nLogView = TRUE;		/* 포그라운드 화면 출력 */
			break;
		  case 'v': case 'V':
			printf("Version %s / %s (base %s) %s\n", ONLIC_VERSION, ONLIC_LAST_UPDATE, ONLIC_WIN_BASE_VERSION, szDefine);
			isExit = TRUE;
			break;
		  case 'h': case 'H':
		  default:
			printf("Usage: %s [-d] [-c config] [-p pidfile] [-l] [-v] [-h]\n", argv[0]);
			printf("	-d	daemon mode\n");
			printf("	-c	config file (default %s)\n", DEFAULT_CONFIG_FILE);
			printf("	-p	pid file (default %s or [PATH_CONF] PID_FILE)\n", PID_FILE);
			printf("	-l	print log to stdout (foreground)\n");
			printf("	-v	print version\n");
			printf("	-h	help\n");
			return 0;
		}
	}
	if(isExit == TRUE) exit(1);

	/* 시그널: onmw 는 데몬일 때만 등록 (포그라운드 SIGPIPE 결함) → 항상 등록 */
	signal(SIGTERM, quit_func);
	signal(SIGINT,  quit_func);
	signal(SIGABRT, quit_func);
	signal(SIGPIPE, pipe_func);
	signal(SIGHUP,  SIG_IGN);

	/* 시스템 시간을 9시간 적게 구하는 오류 대비 (onmw 240711) */
	setenv("TZ", "Asia/Seoul", 1);
	tzset();

	OnlicIntro();

	/* PID 파일: -p 옵션 > ini [PATH_CONF] PID_FILE > 기본값. 중복 실행 검사는 StartSystem 보다 먼저
	** (2번째 인스턴스가 DbLic_UpdateTimeAll 로 전 사이트 cc_update_time 을 초기화하는 것을 막는다, L6) */
	if(strlen_st(m_gPidFile) <= 0){
		GetPrivateProfileStringUsr((char*)"PATH_CONF", (char*)"PID_FILE", PID_FILE, m_gPidFile, sizeof(m_gPidFile)-1, cpath);
		if(strlen_st(m_gPidFile) <= 0) strcpy_st(m_gPidFile, PID_FILE);
	}
	if(is_check_process(onlic_pid, argv[0]) == TRUE){
		printf("onlic already running (pid file %s)\n", m_gPidFile);
		return 1;
	}

	RaiseFdLimit(&lFdSoft, &lFdHard);
#ifdef _CURL
	curl_global_init(CURL_GLOBAL_DEFAULT);		/* HTTP 워커 스레드(StartSystem 7단계) 보다 먼저 (L10) */
#endif

	onlic_start = TRUE;
	nRet = sys_load.StartSystem(cpath);
	if(nRet != SYSTEMLOAD_SUCCESS){
		printf("onlic start failed (code:%d) - see run log\n", nRet);
		StopOnlicSystem();						/* 이미 기동된 타이머/워커 정리 후 종료 (L7) */
#ifdef _CURL
		curl_global_cleanup();
#endif
		return 1;
	}
	OnlicLogf(LOG_SOFT, "RLIMIT_NOFILE soft:%ld hard:%ld", lFdSoft, lFdHard);

	RunOnlicSystem();
	printf("License/Alarm Relay Server Start!! (%s)\n", (daemon_proc == 1) ? "daemon" : "foreground");
	fflush(stdout);
	if(daemon_proc == 1) DetachStdio();

	while(TRUE){
		WaitSleep(1);
		if(g_sig_pipe > 0){ OnlicLogf(LOG_SOFT, "SIGPIPE ignored (count:%d)", (int)g_sig_pipe); g_sig_pipe = 0; }
		if(onlic_start == FALSE) break;
		if(daemon_start == FALSE) break;
	}
	if(g_sig_quit != 0){
		syslog(LOG_NOTICE, "onlic : kill signo:%d", (int)g_sig_quit);
		OnlicLogf(LOG_SOFT, "onlic : kill signo:%d", (int)g_sig_quit);
	}

	StopOnlicSystem();
#ifdef _CURL
	curl_global_cleanup();
#endif
	printf("onlic Server Killed!!\n");
	return 0;
}

/*
**	기동 배너 (onmw NcmwIntro 와 같은 형식)
*/
void OnlicIntro()
{
	char szDefine[501] = "", szVersion[101] = "", szBase[101] = "";
	GetCompileDefineString(szDefine);
	nprintf(szVersion, 100, "%s / %s", ONLIC_VERSION, ONLIC_LAST_UPDATE);
	nprintf(szBase, 100, "Linux port of %s", ONLIC_WIN_BASE_VERSION);
	printf("\n\n*************************************************************\n");
	printf("*                                                           *\n");
	printf("*           %-48s*\n", "ONPIA License/Alarm Relay Server (onlic)");
	printf("*                                                           *\n");
	printf("*           Release Version %-32s*\n", szVersion);
	printf("*           %-48s*\n", szDefine);
	printf("*           %-48s*\n", szBase);
	printf("*                                                           *\n");
	printf("*           %-48s*\n", "Copyright 2001, ONPIA Corporation");
	printf("*                                                           *\n");
	printf("*************************************************************\n");
	fflush(stdout);
}

/*
**	소켓 스레드 기동 (5단계 SocketThread). 역할에 따라 선택
*/
int RunOnlicSystem()
{
	if(sys_conf.role_license == TRUE){
		Pthread_create(&lic_pid, RunLicSocketThread, &m_SockLic);
		WaitMSleep(10);
	}
	if(sys_conf.role_alarm == TRUE){
		Pthread_create(&alarm_pid, RunAlarmSocketThread, &m_SockAlarm);
		WaitMSleep(10);
		Pthread_create(&udp_pid, RunUdpSocketThread, &m_SockUdp);
		WaitMSleep(10);
	}
	return TRUE;
}

/*
**	종료: 타이머 → 소켓 스레드 종료 대기 → 워커 → 로그
*/
/* 스레드가 자기 pid 변수를 0 으로 만들 때까지 기다린 뒤 join (L5). 반환 TRUE = 정상 종료 */
static int JoinThreadVar(pthread_t *pVar, int nMaxMs, const char *szName)
{
	pthread_t tid = *pVar;
	int i = 0;
	if(tid == 0) return TRUE;
	for(i = 0; i < nMaxMs / 100; i++){ if(*pVar == 0) break; WaitMSleep(100); }
	if(*pVar != 0){ printf("%s thread did not exit in %dms\n", szName, nMaxMs); return FALSE; }
	pthread_join(tid, NULL);
	return TRUE;
}

int StopOnlicSystem()
{
	if(onlic_start == FALSE && lic_log_object == NULL) return FALSE;
	onlic_start = FALSE;

	if(licTm != NULL){ licTm->EndUserTimer(); delete licTm; licTm = NULL; }

	/* 소켓 스레드: poll 타임아웃 1초 → 5초 안에 종료. SIGINT 로 죽이는 방식(Pthread_kill) 은 스레드를 종료시키지 못하므로 제거 */
	JoinThreadVar(&lic_pid, 5000, "Lic Socket");
	JoinThreadVar(&alarm_pid, 5000, "Alarm Socket");
	JoinThreadVar(&udp_pid, 5000, "Udp Socket");

	if(sys_conf.role_license == TRUE) LicRecvWaitStackKill();
	if(sys_conf.role_alarm == TRUE){ EventRecvWaitStackKill(); HttpWaitStackKill(); }

	sys_load.KillSystemMemory();

	if(lic_log_object != NULL){
		lic_log_object->LogObjectPush((char*)"onlic stop", LOG_SOFT, LOG_YN_FILE);
		lic_log_object->LogObjectExit();
		delete lic_log_object;
		lic_log_object = NULL;
	}
	unlink(m_gPidFile);
	return TRUE;
}

/*
**	PID 파일 lockf 로 중복 실행 검사 (onmw is_check_process Linux 분기)
**	반환 TRUE = 이미 실행 중
*/
int is_check_process(pid_t my_pid, const char *szPName)
{
	int fd, nRet = FALSE;
	char szReadPid[11] = "", szWritePid[11] = "";
	pid_t pid;

	fd = open(m_gPidFile, O_CREAT | O_RDWR, 0644);
	if(fd < 0){
		printf("Make PID FILE error path:%s\n", m_gPidFile);
		return TRUE;
	}
	lockf(fd, F_LOCK, 0L);
	lseek(fd, 0L, SEEK_SET);
	memset_st(szReadPid, 0, sizeof(szReadPid));
	if(read(fd, szReadPid, 10) < 0) szReadPid[0] = 0;
	lseek(fd, 0L, SEEK_SET);

	if(strlen_st(szReadPid) > 0){
		char procdir[64] = "", buf[1024] = "", szProcName[101] = "", szRealProcName[101] = "";
		pid = atoi(szReadPid);
		if(pid > 0 && pid != my_pid){
			nprintf(procdir, 63, "/proc/%d/exe", pid);
			if(readlink(procdir, buf, 1023) >= 0){
				GetProcessName(buf, szProcName);
				GetProcessName(szPName, szRealProcName);
				if(strcmp(szProcName, szRealProcName) == 0) nRet = TRUE;
			}
		}
	}
	if(nRet == FALSE){
		if(ftruncate(fd, 0) == 0){
			nprintf(szWritePid, 10, "%d", (int)my_pid);
			if(write(fd, szWritePid, strlen_st(szWritePid)) < 0) nRet = FALSE;
		}
	}
	lockf(fd, F_ULOCK, 0L);
	close(fd);
	return nRet;
}

void GetProcessName(const char *szFullPath, char *szProcName)
{
	const char *p = strrchr(szFullPath, '/');
	if(p != NULL) strcpy_st(szProcName, p + 1);
	else strcpy_st(szProcName, szFullPath);
}

void GetCompileDefineString(char* szDef)
{
	strcpy_st(szDef, "");
#ifdef _LINUX
	strcat_st(szDef, "LINUX");
#endif
#ifdef _64BIT
	strcat_st(szDef, ",64BIT");
#endif
#ifdef _DB_TYPE_POSTGRES
	strcat_st(szDef, ",POSTGRES");
#endif
#ifdef _UTF8
	strcat_st(szDef, ",UTF8");
#else
	strcat_st(szDef, ",CP949");
#endif
#ifdef _SOCK_SELECT
	strcat_st(szDef, ",SEL_SOCK");
#endif
#ifdef _CURL
	strcat_st(szDef, ",CURL");
#endif
}

/* 핸들러 안에서 로그 뮤텍스를 잡으면 로그 스레드에 시그널이 왔을 때 자기 교착 → 플래그만 설정 (L4) */
static void quit_func(int signo)
{
	g_sig_quit = signo;
	onlic_start = FALSE;
}

static void pipe_func(int signo)
{
	/* 상대가 끊은 소켓에 send 한 경우 (send 는 모두 MSG_NOSIGNAL 이라 실제로는 거의 없음). 메인 루프가 횟수를 로그 */
	(void)signo;
	g_sig_pipe++;
}
