/*
**	onlicmain.cpp : onlic 진입점 (onmw ncmwmain.cpp 패턴)
**	Windows: CTrsSocketServerApp::InitInstance + CTrsSocketServerDlg (트레이 상주) 의 데몬 대체
*/
#include "onlicDefine.h"
#include "onlicmain.h"
#include <syslog.h>
#include <getopt.h>
#include <fcntl.h>
#ifdef _CURL
#include <curl/curl.h>
#endif

static void	quit_func(int);
static void pipe_func(int);

/////////////////////////////////////////////////////////////////////////////
//	main
/////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	char cpath[256] = "", szDefine[501] = "";
	int  rcode, isExit = FALSE, nRet = FALSE;

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

	onlic_start = TRUE;
	nRet = sys_load.StartSystem(cpath);
	if(nRet != SYSTEMLOAD_SUCCESS){
		onlic_start = FALSE;
		WaitSleep(1);
		return FALSE;
	}

	/* PID 파일은 설정 로드 후 (ini 의 PID_FILE 반영). 이미 실행 중이면 종료 */
	if(strlen_st(m_gPidFile) <= 0) strcpy_st(m_gPidFile, PID_FILE);
	if(is_check_process(onlic_pid, argv[0]) == TRUE){
		printf("onlic already running (pid file %s)\n", m_gPidFile);
		onlic_start = FALSE;
		return FALSE;
	}

#ifdef _CURL
	curl_global_init(CURL_GLOBAL_DEFAULT);
#endif
	RunOnlicSystem();
	printf("onlic Server Start!! (%s)\n", (daemon_proc == 1) ? "daemon" : "foreground");
	fflush(stdout);

	while(TRUE){
		WaitSleep(1);
		if(onlic_start == FALSE) break;
		if(daemon_start == FALSE) break;
	}

	StopOnlicSystem();
#ifdef _CURL
	curl_global_cleanup();
#endif
	printf("onlic Server Killed!!\n");
	return 0;
}

void OnlicIntro()
{
	char szDefine[501] = "";
	GetCompileDefineString(szDefine);
	printf("ONPIA License/Alarm Relay Server (onlic) %s / %s [%s]\n", ONLIC_VERSION, ONLIC_LAST_UPDATE, szDefine);
	printf("Copyright 2001, ONPIA Corporation. Linux port of %s\n", ONLIC_WIN_BASE_VERSION);
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
int StopOnlicSystem()
{
	int i = 0;

	if(onlic_start == FALSE && lic_log_object == NULL) return FALSE;
	onlic_start = FALSE;

	if(licTm != NULL){ licTm->EndUserTimer(); delete licTm; licTm = NULL; }

	for(i = 0; i < 10; i++){ if(lic_pid <= 0) break; WaitMSleep(i*100); }
	for(i = 0; i < 10; i++){ if(alarm_pid <= 0) break; WaitMSleep(i*100); }
	for(i = 0; i < 10; i++){ if(udp_pid <= 0) break; WaitMSleep(i*100); }
	if(lic_pid > 0){ printf("Lic Socket Thread Force Kill!!\n"); Pthread_kill(&lic_pid, SIGINT); }
	if(alarm_pid > 0){ printf("Alarm Socket Thread Force Kill!!\n"); Pthread_kill(&alarm_pid, SIGINT); }
	if(udp_pid > 0){ printf("Udp Socket Thread Force Kill!!\n"); Pthread_kill(&udp_pid, SIGINT); }

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

static void quit_func(int signo)
{
	char buf[64] = "";
	nprintf(buf, 63, "onlic : kill signo:%d", signo);
	syslog(LOG_NOTICE, "%s", buf);
	OnlicLog(LOG_SOFT, buf);
	onlic_start = FALSE;
}

static void pipe_func(int signo)
{
	/* 상대가 끊은 소켓에 send 한 경우. 프로세스 종료 방지만 하고 로그 */
	OnlicLogf(LOG_SOFT, "SIGPIPE ignored signo:%d", signo);
}
