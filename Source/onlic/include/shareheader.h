#ifndef ___SHAREHEADER_H___
#define ___SHAREHEADER_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

#ifdef _WIN32
#ifndef WINVER
//#define WINVER			0x0600	//Windows Vista		
#define WINVER			0x0502	//Windows Server 2003
//#define WINVER			0x0501	//Windows XP  
//#define WINVER			0x0500	//Windows 2000  
//#define WINVER			0x0400	//Windows NT 4.0  
//#define WINVER			0x0500	//Windows Me  
//#define WINVER			0x0410	//Windows 98  
//#define WINVER			0x0400	//Windows 95  
#endif
#endif


#ifdef _UNIX
#include	<pthread.h>
#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */

	#ifdef _FREEBSD
	#include	<sys/sysctl.h>
	#include	<net/bpf.h>
	#include	<sys/stdint.h>
	#include	<net/if_media.h>
	#include	<sched.h>
	#elif _LINUX
	#include	<stdint.h>
	#include	<iconv.h>
	#include	<sys/reboot.h>
	#include	<netinet/ether.h>
	#include	<net/ethernet.h>
	#elif _SOLARIS
	#include	<sys/stdint.h>
	#include	<sys/asm_linkage.h>
	#include	<sys/syscall.h>
	#include	<sys/reboot.h>
	#include	<stropts.h>
	#include	<sys/stropts.h>
	#include	<sys/dlpi.h>
	#include	<time.h>		/* timespec{} for pselect() */
	#else
	#include	<stdint.h>
	#endif

#include	<sys/time.h>	/* timeval{} for select() */

#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include	<errno.h>
#include	<netdb.h>
#include	<sys/uio.h>		/* for iovec{} and readv/writev */

#include	<unistd.h>

#include	<syslog.h>		/* for syslog */
#include	<sys/syslog.h>		/* for syslog */
#include	<sys/wait.h>
#include	<sys/un.h>		/* for Unix domain sockets */
#include	<sys/select.h>	/* for convenience */
#include	<sys/param.h>
#include	<poll.h>		/* for convenience */
#include	<strings.h>		/* for convenience */
#include	<getopt.h>		/* for syslog */
#include	<net/if.h>
#include	<arpa/inet.h> 
#include	<dirent.h>
#include	<iconv.h>

/* Three headers are normally needed for socket/file ioctl's:
 * <sys/ioctl.h>, <sys/filio.h>, and <sys/sockio.h>.
 */
#include	<sys/ioctl.h>
#include	<pthread.h>
#include	<sys/resource.h>
#include	<dlfcn.h>

#define	SOCKET_ERROR			-1
#define	SD_RECEIVE				0
#define	SD_SEND					1
#define	SD_BOTH					2

#endif


#define	MAX_CODEC			31
#define	L_FRAME				80

#ifdef _WIN32
#include <afx.h>
#include <winsock2.h>
#include <afxwin.h>		// MFC socket extensions
#include <io.h>
#include <iostream>
#ifdef _VC60
#include <native.h>
#endif
#include <ws2tcpip.h>
#include <process.h>
#include <iphlpapi.h>
#include <malloc.h>
#include <mmsystem.h>
#endif

typedef void* LISTPOS;

#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>		/* for variable-argument  */
#include	<string.h>
#include	<time.h>		/* timespec{} for pselect() */
#include	<fcntl.h>		/* for nonblocking */
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sql.h>
#include	<sqlext.h>
#include	<ctype.h>
#include	<math.h>
#include	<iostream>
#include	<fstream>
#include	<signal.h>
#include	<vector>


#ifdef _WIN32
	class TEventDummy
	{
	public:
		TEventDummy();
		void InitEvent();
		BOOL SetEvent();
		HANDLE  m_hObject;
	};

	#define pthread_mutex_t		CRITICAL_SECTION
	#define pthread_mutexattr_t	void
	#define pthread_t			long
	#define pthread_attr_t		void
	#define timespec			void
	#define pthread_cond_t		TEventDummy
	#define pthread_condattr_t	void

	#ifndef socklen_t
	#define socklen_t int
	#endif
	#ifndef ssize_t
	#define ssize_t	int   
	#endif
	#ifndef pid_t
	#define pid_t	int
	#endif
	#ifndef _VC60
	typedef __int64		int64_t;
	#endif
#endif

#ifdef _WIN32
#define DllExport __declspec( dllexport )
#else
#define DllExport extern "C"
#define HMODULE void*
#endif
	

#ifndef BYTE
#define  BYTE       unsigned char
#endif
#ifndef UINT
#define  UINT		unsigned int
#endif
#ifndef DWORD
#define  DWORD		unsigned long
#endif
#ifndef WORD
#define WORD	UINT
#endif
#ifndef FALSE
#define  FALSE		false
#endif
#ifndef TRUE
#define  TRUE		true
#endif
#ifndef SOCK_ADDR
#define  SOCK_ADDR		struct sockaddr
#endif
#ifndef PI
#define  PI		(3.14159265358979323846)
#endif
#ifndef HG729
#define  HG729		void*
#endif
#ifndef LPVOID
#define  LPVOID       void*
#endif


#ifndef __getbit
#define __getbit(x, y)   ((x)>>(y)) & (0x1)
#endif
#ifndef __setbit
#define __setbit(x, y)   (x) |= ((0x1)<<(y))
#endif
#ifndef __clrbit
#define __clrbit(x, y)   (x) &= ~((0x1)<<(y))
#endif


#define MAX_RTP_READDATA		1024  /*  Norris Communications, Inc.  */
#define	MAX_BUFF				16384	/* max Sip char length */

// SOCKET state
#define	F_CREATE		1	/* create() in progress */
#define	F_CONNECTING	2	/* connect() in progress */
#define	F_READING		4	/* connect() complete; now reading */
#define	F_SENDING		8	/* all done */
#define	F_DONE			16	/* all done */


enum agtcode_update { update_false = 0, update_ture = 1 }; // 상담원 상태 실시간 업데이트 - 150422

/*------------------------------------------------ NCMW Compile Log ---------------------------------------------------------//



/************************************************ 3.0.1E5 - Oct 01, 2010

//------ 2010/10/03 - 15:55 --- d:\_PJ\ncmw\onmw_101001_3.0.1E5\include
// SendAgentStateGroupDataProc - 호분배 그룹이 없어도 자기 상태이면 상태 정보 전송 


/************************************************ 3.0.1E6 - Oct 01, 2010
//------ 2010/10/05 - 22:59 --- d:\_PJ\ncmw\onmw_101001_3.0.1E5\include

* 그룹 200개로 수정 MAX_SOCK_BUFF_SIZE 

/************************************************ 3.0.1E7 - Oct 06, 2010
//------ 2010/10/06 - 11:06 --- d:\_PJ\ncmw\onmw_101006_3.0.1E7\include

* 그룹의 해당 호분배그룹의 속한 로그인 상담원수, 대기중인 상담원 수
* strncpy 끝에 '\0'

/************************************************ 3.0.1E8 - Oct 06, 2010
//------ 2010/10/07 - 21:12 --- d:\_PJ\ncmw\onmw_101006_3.0.1E7\include
* 사용자 VCID 추가

/************************************************ 3.0.1G1 - Oct 08, 2010
//------ 2010/10/08 - 13:36 --- d:\_PJ\ncmw\onmw_101008_3.0.1G1\include
// * 대표번호통계 , 그룹통계 웹전송 스레드 추가 
//---------------------------------------------------------------------------------------------------------------------------*/



//------ 2010/10/08 - 13:36 --- d:\_PJ\ncmw\onmw_101008_3.0.1G1\include

//------ 2010/10/13 - 15:25 --- d:\_PJ\_Onpia\ncmw\onmw_101008_3.0.1G1\include

// 호분배 그룹의 로그인 상담원 수, 대기중인 상담원수 구하는 부분에서 
// 상담원그룹의 인덱스를 호분배 그룹의 인덱스를 기억하는 방식 nAgtGroupIDX 으로 수정 중 
//SetAgentDivGroupProc(pAgent, CTI_CALLCOUNT_DIV_LOGIN_ADD); // 상담원 호분배 그룹에 로그인 수 올림 
//SSWDataRecvMainProcbIDLEFalg
//* 링중에 링 

//------ 2010/10/14 - 17:31 --- d:\_PJ\_Onpia\ncmw\onmw_101008_3.0.1G1\include
//* nprintf overfloweed 나는곳 수정 모니터트랜스퍼하기할때 등 수정 

//------ 2010/10/14 - 21:17 --- d:\_PJ\_Onpia\ncmw\onmw_101014_3.0.1H1\include	//"3.0.1H1""Oct 14, 2010"
//* 그룹 통계, 대표번호 통계 전송 부분 , 대기중상담원수, 로그인한 상담원수 실시간으로 구해놓기 




//------ 2010/10/15 - 18:34 --- d:\_PJ\_Onpia\ncmw\onmw_101014_3.0.1H1\include	"3.0.1H2"	"Oct 14, 2010"
//* 모임스톤 단말 보류중 보류시 또 보류 시 상태 부분 
//* 폴리콤 단말 보류중 보류시 

//* nasa_code 상태 추가 
//AGT_HOLD_OUT_RE_CONNECT		"0401"	// 보류 OUT 재연결
//AGT_HOLD_IN_RE_CONNECT		"0402"	// 보류 IN 재연결
//AGT_HOLD_DIV_RE_CONNECT		"0403"	// 보류 호분배 재연결



//------ 2010/10/18 - 10:50 --- d:\_PJ\_Onpia\ncmw\onmw_101014_3.0.1H1\include	"3.0.1H3"	"Oct 14, 2010"
//웹소켓 접속시 그룹데이터 특정 클라이언트만 전송 하도록  

//------ 2010/10/18 - 22:54 --- d:\_PJ\_Onpia\ncmw\onmw_101014_3.0.1H1\include	"3.0.1H5"	"Oct 14, 2010"
//소켓 로그아웃 처리되는 부분 스레드로 처리 
//------ 2010/10/19 - 14:04 --- d:\_PJ\_Onpia\ncmw\onmw_101014_3.0.1H1\include
//발신자번호시 특수문제 제외, 
//로그인시오류 로그 

//------ 2010/10/23 - 17:53 --- d:\_PJ\_Onpia\ncmw\onmw_101023_3.0.1H5\include	"3.0.1H6"	"Oct 23, 2010"
// L101, L102 에서 호분배 상태 처리 

//------ 2010/10/25 - 13:43 --- d:\_PJ\_Onpia\ncmw\onmw_101023_3.0.1H5\include	"3.0.1H7"	"Oct 23, 2010"
//AgentSend() nprintf ,로그쪽 상태 부분 버그 수정 

//------ 2010/10/25 - 20:08 --- d:\_PJ\_Onpia\ncmw\onmw_101023_3.0.1H5\include	"3.0.1H7"	"Oct 23, 2010"
// L200 프로토콜 추가 - 부재중 호전환 - AgentRpt 에 로그 쌓이도로고 수정 

//------ 2010/10/26 - 15:11 --- d:\_PJ\_Onpia\ncmw\onmw_101023_3.0.1H5\include	"3.0.1H8""Oct 23, 2010"
// 상담원 프로토콜 26, 27, 28 추가

//------ 2010/10/27 - 11:03 --- d:\_PJ\_Onpia\ncmw\onmw_101023_3.0.1H5_수정중\include
// 아웃바운드할 번호가 없는지 있는지 체크 


//------ 2010/10/28 - 18:52 --- d:\_PJ\_Onpia\ncmw\onmw_101027_3.0.1H9\include	"3.0.1H9"	"Oct 28, 2010"
// 쪽지 스레드로 변경
// syslog() 로그파일 수정 

//------ 2010/10/29 - 16:12 --- d:\_PJ\_Onpia\ncmw\onmw_101029_3.0.1H9\include	"3.0.1H9"	"Oct 28, 2010"
// 콜백스레드 추가 

//------ 2010/11/01 - 9:23 --- d:\_PJ\_Onpia\ncmw\onmw_101029_3.0.1H9\include
//로그에 프로토콜명 

//------ 2010/11/02 - 21:18 --- d:\_PJ\_Onpia\ncmw\onmw_101029_3.0.1J0\include	"3.0.1J1"	"Nov 02, 2010"
// 고객대기자 삭제시 상담원 전송하는 부분 수정 
// 로그인시 상담원 단말상태 가져올때 통화중이면 시도 로그 쌓기
// makecall할때 리로트 쌓지 않고 ctdring 일때  쌓기 

//------ 2010/11/03 - 12:00 --- d:\_PJ\_Onpia\ncmw\onmw_101029_3.0.1J0\include
// AgentModyfyProc 웹에서 정보 수정시 디비 패치가 안되었경우 오류 처리

//------ 2010/11/09 - 11:55 --- d:\_PJ\_Onpia\ncmw\onmw_101029_3.0.1J0\include
// 상담원 모드 변경 (인/아웃바운드) 85번 프로토콜로 전송하도록 

//------ 2010/11/10 - 13:50 --- d:\_PJ\_Onpia\ncmw\onmw_101109_3.0.1J0\include	"3.0.1J2"	"Nov 10, 2010"
//발신자 번호 조회로 호분배 시도 하기 ng_lastcon_day
//콜백 - 1:호분배 연결시 발신자 번호로 콜백이 남겨져 있으면 콜백확인으로 처리 
//       2:콜백 조회일자 Db(nasa_group3) ng_cb_type, ng_cb_day

//------ 2010/11/16 - 20:37 --- d:\_PJ\_Onpia\ncmw\onmw_101112_3.0.1J2\include
// 호분배시도시 인입시간(ivr인입시간)을 리프트(agtrpt)에 적용
// 상담원 로그아웃시 클로즈된 소켓에게는 전송 안하도록 

//------ 2010/11/17 - 12:00 --- d:\_PJ\_Onpia\ncmw\onmw_101112_3.0.1J2\include	"3.0.1J3"	"Nov 17, 2010"
//상담원 인아웃 바운드 모드 변경시 IDLE Timer안쓰고 직접 처리 

//------ 2010/11/17 - 13:53 --- d:\_PJ\_Onpia\ncmw\onmw_101117_3.0.2\include	"3.0.2"		"Nov 17, 2010"
// ODBC 정보를 ini 파일에서 읽도록 수정 

//------ 2010/11/24 - 21:20 --- d:\_PJ\_Onpia\ncmw\onmw_101117_3.0.2\include	"3.0.3"		"Nov 24, 2010"
// 픽업시 해당 그룹이 아니면 인바운드 픽업으로 처리 
// 상태변경시 그룹 변경이 호분배그룹에 속했을경우에만 변경 

//------ 2010/12/01 - 9:57 --- d:\_PJ\_Onpia\ncmw\onmw_101126_3.0.6\include		"3.0.7"		"Dec 01, 2010"
// 로그인했을때 전송 91
// 로그인 했을때 해당 (마지막 사용 그룹체크후 전송 ) 호분배그룹 첫번째 그룹 전송 
// 로그아웃, 로그인 했을때 subscriber에 기본 그룹, 내선  에 업데이트 
// 그룹 변경 햇을때 해당그룹이 호분배 그룹일경우 에만 변경

//------ 2010/12/01 - 18:05 --- d:\_PJ\_Onpia\ncmw\onmw_101201_3.0.8\include	"3.0.8"		"Dec 01, 2010"
// 팝업 callcenter_info 테이블에서 팝업타입으로 - 3:응답시나리오명 추가 

//------ 2010/12/13 - 13:20 --- d:\_Svn\Project\ncmw-onpbx\include				"3.1.0"		"Dec 13, 2010"
// CallCenter_info 테이블에서 정보 패치 하도록 수정 

//------ 2010/12/27 - 16:23 --- d:\_Svn\Project\ncmw-onpbx\include				"3.1.1"		"Dec 27, 2010"
// AGT 소켓 클로즈시 fd 리로드
// 스레드별 받은 데이터 로그 
// 로그인시 링크 체크 타이머 설정 

//------ 2010/12/28 - 9:56 --- d:\_Svn\Project\ncmw-onpbx\include				"3.1.2"		"Dec 28, 2010"
// AGT 소켓 클로즈시 fd 리로드 (셀렉트시 락설정)

//------ 2010/12/30 - 20:59 --- d:\_Svn\Project\ncmw-onpbx\include				"3.1.3"		"Dec 30, 2010"
//예상 대기시간 추가 

//------ 2011/01/04 - 9:33 --- d:\_Svn\Project\ncmw-onpbx\include				"3.1.4"		"Jan 04, 2011"
//로그정리
//MakeCall 시 로그 정리 
//AgentGroupLoad() 실행시 그룹별 로그인수, idle 상담원수 다시 구함 (SetGroupAllReCountPush())

//------ 2011/01/05 - 16:58 --- d:\_Svn\Project\ncmw-onpbx\include				"3.1.5"		"Jan 05, 2011"
// 상담원 디폴트 상태 변경값 로그아웃시 초기화 

//------ 2011/01/06 - 14:59 --- d:\_Svn\Project\ncmw-onpbx\include				"3.1.6"		"Jan 06, 2011"
// #define FD_SETSIZE 2048

//------ 2011/01/14 - 11:50 --- d:\_Svn\Project\ncmw-onpbx\include				"3.1.7"		"Jan 14, 2011"
// 대표번호별 콜백 추가 (프로토콜 29)

//------ 2011/01/24 - 18:31 --- d:\_Svn\Project\ncmw-onpbx\include				"3.1.8"		"Jan 24, 2011"
//아이들상태 포기호 건수 수정 

//------ 2011/02/08 - 14:13 --- d:\_Svn\Project\ncmw-onpbx\include				"3.1.9"		"Feb 08, 2011"
//호전환 시도중에는 복귀코드 무시

//------ 2011/02/09 - 16:25 --- d:\_Svn\Project\ncmw-onpbx\include				3.2.0"		"Feb 09, 2011
//웹소켓 접속시 WEB_SERVER_TIME  서버시간 전송 

//------ 2011/02/11 - 18:14 --- d:\_Svn\Project\ncmw-onpbx\include				"3.2.1"		"Feb 11, 2011"
// 호분배시도중에 아웃바운드를 동시에 했을때 ...호분배 시도콜 처리 수정  //110211

//------ 2011/03/07 - 14:26 --- d:\_Svn\Project\ncmw-onpbx\include				"3.2.2"		"Mar 07, 2011"
// 소켓 강제 코드 70번 추가 AgentForceLogoutPorc

//------ 2011/03/09 - 20:16 --- d:\_Svn\Project\ncmw-onpbx\include				"3.2.3"		"Mar 09, 2011"
// 웹전송 스레드 추가 

//------ 2011/03/10 - 15:00 --- d:\_Svn\Project\ncmw-onpbx\include				"3.2.4"		"Mar 10, 2011"
// 상담원 상태 사용시간 - GetMilliSencondProc -> GetSencondProc 초단위로 계산

//------ 2011/03/11 - 21:27 --- d:\_Svn\Project\ncmw-onpbx\include				"3.2.5"		"Mar 10, 2011"
// 웹소켓 정리 

//------ 2011/03/12 - 10:41 --- d:\_Svn\Project\ncmw-onpbx\include				"3.2.6"		"Mar 12, 2011"
// SO_SNDTIMEO SO_RCVTIMEO 소켓 타임지정 

//------ 2011/03/23 - 14:56 --- d:\_Svn\Project\ncmw-onpbx\include				"3.2.7"		"Mar 23, 2011"
// 녹취파일명 _RES_VRS_CALLER 저장만 하고 요청후 
// _RES_VRS_CREATE 이벤트 왔을때 팝업 , 타이머 1초

//------ 2011/03/24 - 12:24 --- d:\_Svn\Project\ncmw-onpbx\include				"3.2.8"		"Mar 24, 2011"
// 팝업을 타이머로 강제 설정 (호분배, 픽업, 아웃바운드)
// ini 파일로 설정 

//------ 2011/03/25 - 15:44 --- d:\_Svn\Project\ncmw-onpbx\include				"3.2.9"		"Mar 25, 2011"
// 고객예상대시간 프로토콜에  총 고객대기수 포함 

//------ 2011/03/29 - 11:45 --- d:\_Svn\Project\ncmw-onpbx\include				"3.3.0"		Mar 29, 2011"
//아이들상태 포기호 건수 수정 

//------ 2011/03/31 - 9:51 --- d:\_Svn\Project\ncmw-onpbx\include				"3.3.1"		"Mar 31, 2011"
// SetDivGroupCount 카운트 오류시 재동기화 
//(그룹별 로그인수, idle 상담원수 다시 구함- SetGroupAllReCountPus)
// WebBaseCodeChange 수정시 상담원 통계 다시 읽기GetAllAgentCallCntProc()

//------ 2011/04/01 - 11:59 --- d:\_Svn\Project\ncmw-onpbx\include
// 픽업관련 통계 및 상태리포트 정리 
// 고객예상대기산 프로토콜에 대그룹, 중그롭 소그룹 고객대기인원수 포함 

//------ 2011/04/12 - 18:22 --- d:\_Svn\Project\ncmw-onpbx\include				"3.3.2"		"Apr 12, 2011"
// CWaitList 리스트 뮤텍스 설정, DivFailRealCallCountProc 포기호부분 변경

//------ 2011/04/15 - 17:09 --- d:\_Svn\Project\ncmw-onpbx\include				"3.3.3"		"Apr 15, 2011"
// LogBuffer 200개 에서 400개로

//------ 2011/04/20 - 18:11 --- d:\_Svn\Project\ncmw-onpbx\include				"3.3.4"		"Apr 20, 2011"
// 고객등급별호전환, 고객정보 팝업타입 추가 
// 리눅스 버젼 포팅 

//------ 2011/04/27 - 21:04 --- d:\_Svn\Project\ncmw-onpbx\include				"3.3.5"		"Apr 27, 2011"
// CAgentList, CThWaittimeWait - ODBC Connect 삭제
// CDbManager - MAX_DB_PTR 3개로 변경, GetIndex/ReleaseDB 뮤텍스 설정 

//------ 2011/04/28 - 14:59 --- d:\_Svn\Project\ncmw-onpbx\include				"3.3.7"		"Apr 28, 2011"
// CFG 파일 쪽지스레드,전화예약스레드 설정하도록, DB메니저 DB 커넥터 설정 하도록 
// GetPrivateProfileStringUsr 디폴트 문자 수정 

//------ 2011/05/12 - 2:35 --- d:\_Svn\Project\ncmw-onpbx\include				"3.3.8"		"May 12, 2011"
// nasa_callcenter_info 필드 추가 

//------ 2011/05/16 - 11:06 --- d:\_Svn\Project\ncmw-onpbx\include				"3.3.9"		"May 16, 2011"
// vip 연동-41번프로토콜 utf8 처리 (member_ascii_name, member_ascii_id, member_ascii_info1, member_ascii_info2)
// 팝업정보 : 보류 정보 menu_code -> sc_name 변경 

//------ 2011/05/17 - 14:13 --- d:\_Svn\Project\ncmw-onpbx\include				"3.4.0"		"May 17, 2011"
// L142 호전환받기 성공시 상담원 상태 변경(IN연결 -> IN재연결)

//------ 2011/05/31 - 15:12 --- d:\_Svn\Project\ncmw-onpbx\include				"3.4.1"		"May 31, 2011"
// ivr 고객대기자 순위 옵션 추가

//------ 2011/06/08 - 14:27 --- d:\_Svn\Project\ncmw-onpbx\include				"3.4.2"		"Jun 08, 2011"	
// 전화걸기 발신번호 디지트외 문자 제거 GetCallNumberDigit

//------ 2011/06/17 - 17:30 --- d:\_Svn\Project\ncmw-onpbx\include				"3.4.3"		"Jun 17, 2011"
// 리스트 continue 버그  수정 

//------ 2011/06/21 - 16:38 --- d:\_Svn\Project\ncmw-onpbx\include				"3.4.4"		"Jun 21, 2011"
// ACS 프로토콜 추가 중

//------ 2011/07/04 - 14:33 --- d:\_Svn\Project\ncmw-onpbx\include				"3.4.5"		"July 04 2011"
// agt_state_send_mode 추가 94번 모든 상담원 전송하도록 

//------ 2011/07/05 - 10:30 --- d:\_Svn\Project\ncmw-onpbx\include				"3.4.6"		"Jul 05, 2011"
// 로그인시 상담원상태'출근' 전송 

//------ 2011/07/12 - 12:43 --- d:\_Svn\Project\ncmw-onpbx\include				"3.4.7"		"Jul 12, 2011"
// 호전환 상담원 링크 내선으로 찾기 

//------ 2011/07/13 - 10:13 --- d:\_Svn\Project\ncmw-onpbx\include				"3.4.8"		"Jul 13, 2011"
// 모니터 호전환 보류콜아이디와 아웃바운드 콜아디 변경  //AgentPhone_Attend

//------ 2011/07/13 - 17:44 --- d:\_Svn\Project\ncmw-onpbx\include
// 모니터 호전환 보류콜아이디와 아웃바운드 콜아디 변경  //AgentPhone_Attend - 취소 원본했음 

//------ 2011/07/14 - 14:33 --- d:\_Svn\Project\ncmw-onpbx\include				"3.4.9"		"Jul 14, 2011"
//아웃운드 실패시 (PbxMakecallFailProc) - 실패이벤트 2번왔을경우무시(Kill-IDLETimer 제거)

//------ 2011/07/15 - 11:01 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.0"		"Jul 15, 2011"
// 호전환 성공이후 콜아디 변경 오류 수정 (PbxTransAnswerSucc)

//------ 2011/07/18 - 16:03 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.1"		"Jul 18, 2011"
// AGT_SendClient 위치로그 (마지막사용pos 722)

//------ 2011/07/20 - 10:19 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.1"		"Jul 20, 2011"

//------ 2011/08/02 - 12:44 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.2"		"Aug 02, 2011"
// 링크 이벤트의 링크내선을 풀번호호로 변환 옵션 LINKTEL_CONVERT	
// LinkFullNumberConvert

//------ 2011/08/03 - 13:04 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.3"		"Aug 03, 2011"
//** _SITE_MULTI 멀티테넌트 작업
// DB Insert 에 site_idx 추가(nasa_agentrpt,nasa_call_log,nasa_call_id)
// 상담원 그룹 코드 / 상태코드 를 사이트별로 관리 

//------ 2011/08/08 - 17:45 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.3"		"Aug 08, 2011"
// 설정부분 일부 CC_OPTION으로 분리 
// -CC_OPTION 옵션부분 로딩/패치부분, 기능 추가(agt_idle_time, 
// agt_oneday_logout, pup_type, popup_wait)  옵션정보를 상담원 구조체에서 로딩 

//------ 2011/08/09 - 13:38 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.4"		"Aug 09, 2011"
// SITE_MULTI 디파인에서 config g_nMultiTenant 으로 변경 


//------ 2011/08/10 - 20:25 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.5"		"Aug 10, 2011"
// 시스템 로딩시 로그출력  [LOAD_LOG] LOG_VIEW =1


//------ 2011/08/10 - 20:25 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.6"		"Aug 10, 2011"
// CTimer 메모리 할당 위치 변경 

//------ 2011/08/11 - 12:01 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.6"		"Aug 11, 2011"
//StartSystem 정리 


//------ 2011/08/24 - 19:54 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.7"		"Aug 24, 2011"
// 컴파일(멀티테넌트 최종버전)

//------ 2011/08/30 - 18:05 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.8"		"Aug 30, 2011"
// ACS 로딩부분 수정
// 상담원 로그인시 디비패치 로그인한경우 에도 로그인 처리 
// m_AgtList 뮤텍스 처리 
// SendAgtWaitStack 상담원 전송 스레드 추가
// list_agt_socket 뮤텍스 처리 

//------ 2011/08/31 - 16:20 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.8"		"Aug 31, 2011"
// 상담원 평균대기 시간 수정
// 상담원 소켓 로그 수정


//------ 2011/08/31 - 20:15 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.8.1"	"Aug 31, 2011"
// 고객대기자수 웹전송시 멀티테넌트 프로토콜 사이트 인덱스 추가


//------ 2011/09/02 - 12:41 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.8.2"	"Sep 02, 2011"
// 로기인후 자기자신이 강제로그인 한경우 처리 
// 상담원 소켓 agt_socket_mutex, agt_socklist_mutex 두개로 뮤텍스 처리 


//------ 2011/09/08 - 11:55 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.8.3"	"Sep 08, 2011"
// 85, 85번 프로토콜 해당 사이트에 속한 상담원 만 전송 

//------ 2011/09/08 - 18:41 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.8.4"	"Sep 08, 2011"
// Out시도, CTD시도 상태에서 클릭투다이얼 금지 
// 동일번호로 1초 이내 재발신금지

//------ 2011/09/28 - 19:24 --- d:\_Svn\Project\ncmw-onpbx\include				"3.5.9"		"Sep 28, 2011"
// 소스정리 


//------ 2011/10/17 - 16:35 --- d:\_Svn\Project\ncmw-onpbx\include				"3.6.0"		"Oct 17, 2011"
// 로그아웃시 "퇴근"상태 전송시 스레드에서 전송안하고 직접 전송 


//------ 2011/10/18 - 12:24 --- d:\_Svn\Project\ncmw-onpbx\include				"3.6.01"	"Oct 18, 2011"
// WaitForDbConnect 추가 


//------ 2011/10/18 - 18:40 --- d:\_Svn\Project\ncmw-onpbx\include				"3.6.02"	"Oct 18, 2011"
// 대표번호 설정 없이도 실행 하도록 수정 
// 51, 52번 웹 통계 요청 프로토콜 추가 


//------ 2011/10/21 - 18:33 --- d:\_Svn\Project\ncmw-onpbx\include				"3.6.03"	"Oct 21, 2011"
//  52번 웹 통계 요청 - 대그룹 ,중그룹 추가 



//------ 2011/11/24 - 21:00 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.03T1"

//------ 2011/11/25 - 9:44 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.03T2"

//------ 2011/11/25 - 22:54 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.03T3"


//------ 2011/11/26 - 17:59 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.03T5"

//------ 2011/11/27 - 4:04 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.03T6"		"Nov 27, 2011"
// 캠페인 수정 

//------ 2011/11/27 - 8:10 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.03T7"


//------ 2011/11/27 - 19:00 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.05"
// 캠페인 상담원 고객대기자 전송 부분 수정 

//------ 2011/11/29 - 13:02 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.06"		"Nov 29, 2011"

//------ 2011/11/29 - 21:28 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.07"

//------ 2011/11/30 - 20:36 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.08"
// 상담원 정보 수정시 사이트옵션정보 패치 버그 수정 


//------ 2011/12/04 - 14:30 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.11T1"


//------ 2011/12/05 - 13:45 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.11T2"
// 웹소켓 Any 바인딩


//------ 2011/12/05 - 20:44 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.11T3"	"


//------ 2011/12/06 - 0:21 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.11T5"


//------ 2011/12/07 - 13:22 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.11T6"		"Dec 7, 2012"
// 웹 상담원 정보 수정시 패치 버그 수정 
// 녹취 소켓전송 타임아웃 설정 


//------ 2011/12/07 - 21:35 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.12"		"Dec 7, 2012"
// 상담원 IDLE 상태 변경을 타이머에서 상담원 스레드에서 실행 하도록 푸쉬 


//------ 2011/12/08 - 21:26 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.13"		"Dec 8, 2011"
// PbxAnswerFailProc 호분배 실패에 대한처리 추가 div_fail_call_set;
// 호분배시 상담원 단말이 수화기를 들어 Busy가 날경우 호분배 실패에 통계가 없어서 추가


//------ 2011/12/10 - 15:17 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.13T1"		"Dec 10, 2011"
// 소켓 통신시 recvWait 스레드의 개수 로그 
// 디비 지연 로그 , 스레드 지연로그 추가 


//------ 2011/12/10 - 18:36 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.13T2"
// 캠페인 데이터 로딩시 유저데이터 상태 DB 업데이트부분 뮤텍스밖에서 작업하도록 수정 


//------ 2011/12/10 - 20:26 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.13T3"
// 리포트 디비 큐스레드에서 처리하도록 옵션 추가 SQL_ALL_QUEUE(sys_conf.sql_all_queue)



//------ 2011/12/10 - 21:04 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.13T5"
// 캠페인 데이터 로딩 시간및 개수 지정 INI 추가 
//PDS_LOAD_SLEEP_SEC=15
//PDS_LOAD_USER_COUNT=150


//------ 2011/12/12 - 10:24 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.14"		"Dec 12, 2011"
// ListPgrsv_FindPDS 코어덤프 오류부분 수정 


//------ 2011/12/12 - 20:27 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.15"
// pds_set 쿼리 오타 수정 
// 녹취소켓 딜레이 타임 로그 


//------ 2011/12/13 - 2:17 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.15T2"
// DB pds_user update 


//------ 2011/12/13 - 23:34 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.15T3"
// maxUser 오류 수정 
// pds_user 디비 패치시 불피요 필드 제거 
// SqlWaitStack Push카운트및 로그 추가 



//------ 2011/12/14 - 2:37 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.16"
// SqlWaitStack 버퍼수 2000


//------ 2011/12/15 - 13:18 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.17"		"Dec 15, 2011"
// PDS Send 스레드 추가 , 스케줄에서 Send쓰레드 대기캠페인수 체크 딜레이
// 상담원 호분배 그룹코드요청시(91) 해당 상담원 정보만 보내도록 수정 - AGENT_CMD_GROUP_CODE
// pds_user > pds_user2 완료된것 데이터 옮기기 PDSMoveDbMainProc()추가 


//------ 2011/12/15 - 20:58 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.18"
// WaitList_GetData로 고객대기리스트 포인터 오류 수정 


//------ 2011/12/16 - 22:11 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.19"		"Dec 16, 2011"
// IVR의 캠페인처리 대기 개수를 파악하여 스케줄관리를한다. m_nIvrWaitPds


//------ 2011/12/19 - 20:23 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.20"		"Dec 19, 2011"
// 유저데이터 로딩시 아웃바운드 카운트 비교후 처리 되는 오류 수정 
// 캠페인명 을 구하지 못하는 버그 


//------ 2011/12/21 - 18:03 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.22"			"Dec 22, 2011"
// 인재열후 아웃바운드 인재열결시 "보류중인재연결"로 상태가 변경되는 버그 수정


//------ 2011/12/23 - 17:46 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.23"			"Dec 23, 2011"
// SQLObjectPush의 g_nSqlThIDX 뮤텍스 오류 수정 


	
//------ 2011/12/28 - 19:31 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.24"			"Dec 28, 2011"
// Sql 스레드 딜레이 로그 추가 - 디버깅 옵션시 


//------ 2011/12/29 - 13:51 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.25"			"Dec 29, 2011"
// 호분배시도,캠페인시도 해당 콜정보 녹취 전송 프로토콜 64번 추가 
// VRS_DIVINFO_SEND=1 / sys_conf.vrs_divinfo_send
// 상담원 이름, ID 디비로딩시 아스키변환 데이터 추가 
// agt_ascii_name[31];
// agt_ascii_id[31];


//------ 2011/12/30 - 19:02 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.26"			"Dec 30, 2011"
// 녹취 연동 (위드인적용버전)


//------ 2012/01/05 - 13:45 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.27"			"Jan 05, 2012"
// 상당원 소켓이 로그인도중 소켓이 종료된경우 상담원구조체에 소켓 정보 패치시
// 오버플로우가 발생해서 상담원 구조체에 영향을 미쳐서 
// 로그인시 소켓 포인터를 사용하지 않고 복사해서 사용하는방식으로 수정했고
// 최종 로그인처리하기 직전 소켓이 존재하는지 체크해서 로그인 처리를 하도록수정했다.


//------ 2012/01/10 - 20:11 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.28"			"Jan 10, 2012"
// 녹취 연동 정보에 "캠페인제휴사코드" 추가 
// PDS_PARTNER_CODE pds_ptn_code ptn_code szPartnerCod


//------ 2012/01/16 - 10:14 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.29"			"Jan 16, 2012"
// 캠페인 완료되었을때 캠페인 상태처리 버그 수정 


//------ 2012/01/26 - 19:58 --- d:\_Svn\Project\ncmw-onpbx\include		3.6.29E				"Jan 26, 2012"
// 이크레더블 버전 


//------ 2012/02/02 - 17:43 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.31"			"Feb 02, 2012"
// 캠페인 120202 국제전화번호 00 또는 060 로드 실패 처리 


//------ 2012/02/06 - 11:40 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.32"			"Feb 06, 2012"
// LINK 연동없이 사용할경우 환경 변수 추가 LINK_DISABLE=1 (sys_conf.link_disable)
// 이크레더블 소스 추가 - 공통 사용가능한 부분만 
// 다솔적용 버전 


//------ 2012/02/15 - 20:58 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.33"			"Feb 15, 2012"
// 위드인 녹취 팝업전송 오류 수정 : _RES_VRS_CREATE 이벤트시 sip-callid 비교 
// DisConnectODBC() - 주석처리 

//------ 2012/03/15 - 18:17 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.35"			"Mar 15, 2012"
// 자동로그인후 이전상태 변경 옵션 추가 AUTO_LOGON_CODE_CHANGE



//------ 2012/05/13 - 23:19 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.36"			"May 13, 2012"
// DB 로딩시 DUP_IP_USE 로 로딩 여부 추가 
// SOCK_IVR_CON_ANY 추가 


//------ 2012/05/23 - 12:00 --- d:\_Svn\Project\ncmw-onpbx\include     "3.6.37"				"May 23, 2012"
// 팝업타입 1 - 응답모드명 버그 수정 



//------ 2012/05/24 - 19:23 --- d:\_Svn\Project\ncmw-onpbx\include  "3.6.38"					"May 24, 2012"
// 그룹대기 호분배 방식중 오류 찾기 




//------ 2012/06/07 - 16:28 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.39"				"June 07, 2012"
// 녹취중인 파일에 분류 이벤트 추가 

//------ 2012/06/13 - 15:06 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.40"				"June 13, 2012"
//  녹취중인 파일에 분류 이벤트 -수정 


//------ 2012/06/25 - 19:29 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.41"				"June 25, 2012"
// 단말 레지 상태 정보 상담원에게 전송 
// PbxStateRegisterProc


//------ 2012/07/04 - 14:04 --- d:\_Svn\Project\ncmw-onpbx\include		3.6.42					"July 04, 2012"
// 로그 상태 표시 추가 PBX_STATE_REGISTER


//------ 2012/07/11 - 11:13 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.43"				"July 11, 2012"
// AET 소켓(상담원소켓2) 추가 - 클릭투다이얼 
// 설정 
/*
[MW_CONF]
AET_SOCK_USE=1
AET_RECV_TH=3
AET_SEND_TH=2
*/

//------ 2012/07/12 - 11:11 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.44"				"July 12, 2012"
// CALL_INFO member_name / member_ascii_name  31Byte-> 91Byte변경 
// UTF8 DB에서 한글은 3Byte 사용 


//------ 2012/07/16 - 18:35 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.45"				"July 16, 2012"
// 콜백 자동 업데이트 - 픽업성공/아웃바운드연결/인바운드 연결 에서도 처리 
// CallbackListenUpdatePush


//------ 2012/07/19 - 12:50 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.46"				"July 19, 2012"
// *상담원 픽업 요청시 링상태일때 요청 무시 추가 
// *클릭투 다이얼시 CTD 이벤트 전에 대표번호와 발신자 번호 변경해서 AgentRtp 기록 오류
//  대표번호와 발신번호 는 BX_CTD_RING 와 PBX_CTD_ANSWER 에서 리포트 찍기전 처리 
// *AgentRtp Call로그 - 보류와 복귀 했을때 변경 되서 DB 저장 되로록 수정 
// *클릭투다이얼수 단말에서 Busy 올경우 대기 상태로 처리 오류부분 수정 
//  MakeCall할때 CTD-Cid보관해서 PBX_CTD_ANSWER 이벤트 없이 
//  PBX_DISCONNECT될때 무시해서 대기상태가 되지 않도록 수정 
// *call_info[CALL_STATE_CTD] 상태 콜인포 추가 
//  120718 120719


//------ 2012/07/19 - 19:10 --- d:\_Svn\Project\ncmw-onpbx\include	"3.6.47"				"July 19, 2012"
// * CTD 시도 없이 바로 MakeFail 과 DisConnect 이벤트가올때 
//   Makecall 요청할때 상태코드와 아웃바운드 번호를 저장하여 
//   MakecallFail 이벤트올때 비교하여 무시 하도록 처리 


//------ 2012/08/14 - 13:08 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.48"			"Aug 14, 2012"
// GetMilliSencondProc 와 타이머오브젝트 RunTimerProc 에서 시간계산체크 


//------ 2012/09/10 - 17:37 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.49"			"Sep 10, 2012"

// AGTCALL_FROM_CALLER_USE - 추가 - 1차 상담원이 보류후 2차 상담원에게 아웃바운드시 2차 상담원 링정보의 발신자 번호를 FROM 번호로 사용여부 
// AGTCALL_POPUP_EXT_USE  - 추가 - 상담원간 내선통화일때 팝업정보에 상담원내선(링크내선)번호로 변환하여 정보로 팝업전달 
// auto_logincode_change - 기본값 "설정"으로 변경 

// PbxAttendTransSucc -  call_info[CALL_STATE_TRANS].call_status를 AGT_TRNS_SUCC 변경하고 PbxAnswerProc 비교 
// 모니터 트랜스퍼 하고 링 중에 끊었을경우 :
// "인바운드돌려주기시도" -> "인바운드돌려주기연결" 처리를 "인바운드돌려주기시도" -> "돌려주기 연결" 변경처리 

// 블라인드 팝업정보 처리시 고객번호(transfer_caller) 추가 
// PbxUnAttendTryProc -시도시 고객번호, 호전환내선, 호전환 콜아이디를 호전환 받는 상담원에게 저장 
// PbxTransAnswerRing - 저장된 호전환내선,과 호전환 콜아이디, 그리고 추가한 고객번호도 비교하여 호전환 정보 찾기
// OnPBX에서 PbxTransAnswerRing경우 (단말내선, 고객번호, 고객번호)으로 이벤트 발생하여서 추가 하였음 



//------ 2012/10/16 - 11:47 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.50"		"Oct 16, 2012"
// 연결팝업 전송시 이전 팝업이 있으면 전송후 다음 팝업 진행 121015
// PbxTransAnswerSucc 호전환받기 성공후 발신자번호를 변경 121016



//------ 2012/10/24 - 19:10 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.51"		"Oct 24, 201
// 호분배 링중에 링 처리 - 통화중대기호로 처리 
// 검색: 121024, div_real_ring



//------ 2012/10/24 - 20:43 --- d:\_Svn\Project\ncmw-onpbx\include			"3.6.52"		"Oct 24, 2012"
// 블라인드 트랜스퍼한 상담원을 찾지 못한경우 다시 한번 찾는다
// pbx.szData1 데이터가 고객번호로 잘못 오는경우 처리를 하기위해서 이다
// OnPbxLink 에서 오류 수정되면 삭제 해도 됨 
// 검색: 121024-1, div_real_ring, call_info[CALL_STATE_NEXT].transfer_caller, SearchAsUnAttendAgentProc



//------ 2012/10/29 - 18:06 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.52"			"Oct 24, 2012"
// UDP 클릭투 다이얼 추가 


//------ 2012/11/22 - 17:12 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.53"			"Nov 22, 2012"
// 로그인시도시 이미 로그인 되었을경우(자동, 강제)로그인시 로그아웃처리부터 



//------ 2012/11/26 - 15:17 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.54"			"Nov 26, 2012"
// DisConnectODBC() SQLDisconnect(m_hDbc) 주석 해제 
// SQLDisconnect 없으면 Odbc끊지 않고 ReConnect 함 

//------ 2013/3/28 - 10:20 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.55"			"Mar 28, 2013"
// * 통화종료후 IDLE 타임이 진행중에 클릭투다이얼 한경우 (AGT_IDLE_TIMER중지)처리 
// * 한경희에서 호분배 연결이 DB에 2건 처리되었음(AgentIdleTime, PBX_CTD_RING) 


//------ 2013/4/25 - 0:01 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.56"			"Apr 25, 2013"	
// * AgentIdleTimeResetProc() - 자정 IDLE 상담원 재 IDLE 상담원 상태로 변경시 
// * 복귀코드 사용해서 통화후 상태코드로 변경 되지 않도록 수정 


//------ 2013/5/27 - 15:49 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.57"			"May 26, 2013"  
// * 3.6.53 버전의 로그아웃 처리가 오류 발생(네트워크가 재연결되었을때 자동로그인처리 실패)
// * AgentChangeLogOut로 로그아웃 처리시 소켓 정보가 초기화되어 자동로그인시 기존 소켓정보가없어 
//    로그인이 실패 되는 현상이 발생하여 "로그아웃"상태만 DB에 기록하도록 처리 하였음(정선군청에서 최초 장애 발생함)


//------ 2013/6/14 - 19:07 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.58"			"JUNE 14, 2013"
// * 녹취 소켓 생성 Any 포트로 [SOCK_CONF]SOCK_REC_CON_ANY=1

//------ 2013/6/27 - 21:09 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.59"			"JUN 27, 2013"
// * 웹관리자 페이지와 링크체크 프로토콜 66 번 추가 


//------ 2013/6/28 - 19:33 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.60"			"JUN 28, 2013"
// *  단말의 3자통화버튼을 눌러서 3자통화를 시도한경우 보류이벤트 없이 바로 "Out재연결" 이벤트 발생함.
// *  3자통화 버튼 누르면 재연결 이벤트 발생 (PbxMakecallReConnectProc / PbxReAnswerProc) 추가

//------ 2013/8/22 - 9:38 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.61"				"AUG 22, 2013"
// * 클릭투다이얼 중복 발신 금지하는 로직 수정 //130822
// * customer에서 pAgent->caller_temp 변수로 비교하는 방식으로 수정 


//------ 2013/9/5 - 16:39 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.62"				"Sep 05, 2013"
// * "블라인드성공" 상태로 묶이는 장애처리 (130905)
// * PBX_UNATTEND_TRY -> PBX_DISCONNEC -> PBX_UNATTEND_SUCC 이벤트가 잘못와서 IDLE 타이머를 블라인드성공처리하는 부분에서 주석처리 
// * 정상인경우 PBX_UNATTEND_TRY -> PBX_UNATTEND_SUCC -> PBX_DISCONNECT


//------ 2013/9/26 - 15:07 --- d:\_Svn\Project\ncmw-onpbx\include     "3.6.63"				"Sep 26, 2013"
// * 담당상담원연결 로직 추가(동원홈푸드) 검색- "130924", 요청코드(38), 응답코드(39)
// * IVR에서 고객의 1,2,3순위 상담원ID로 요청하면, 대기중인 상담원 리턴 
// * IVR 버전 1.7.8 이상 에서만 연동 


//------ 2013/11/18 - 14:37 --- d:\_Svn\Project\ncmw-onpbx\include     "3.6.64"				"Nov 18, 2013"
// * AgentWriteReportProc 동일상태 2번리포트 찍히는 오류 수정 (131118)
// * IDLE Time와 클릭투다이얼의 두개의 루트에서 DB리프트(AgentWriteReportProc) 시도시 처리 
// * [MW_CONF] RTP_COUSEL=0  nasa_counsel 리포트 사용여부 설정(기본설정:미설정) 


//------ 2013/11/26 - 22:25 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.65"			"Nov 26, 2013"		   
// * SockList_Remove 소켓 삭제 및 리스트 출력 프로토콜 추가 
// * 
	
//------ 2013/11/27 - 9:36 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.66"			"Nov 27, 2013"
// * Agent소켓 리스트 뒤에서부터 검색하여 전문 송신
// * AGT_SendClient.GetTailPosition


//------ 2013/12/6 - 17:42 --- d:\_Svn\Project\ncmw-onpbx\include      "3.6.67"				"Dec 06, 2013"
// * 호분배 시도중 네트워크가 끊겼을 경우(로그아웃)에도 포기호를 구해서 웹쪽 전송 
// * DivFailRealCallCountProc(pDB, pAgent);// 131206

//------ 2014/1/2 - 19:34 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.68"			"Jan 02, 2014"
// * [MW_CONF] USER_VCID_DISABLE=1 
// * 사용자 VCID를 사용 못하도록 옵션 처리 (링크 풀번호 가입자는 예외처리)

//------ 2014/2/5 - 14:21 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.68"			"Feb 05, 2014"


//------ 2014/2/19 - 19:13 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.69"			"Feb 19, 2014"
// * DivFailRealCallCountProc 포기호 처리부분 버그 처리(현황판 포기호 오류) 
//   - 대표번호 통계전송부분에 그룹통계전송부분이 잘못 들어가 있었음 140219

	
//------ 2014/3/6 - 18:09 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.70"			"Mar 06, 2014"
// * 아웃바운드 시도중에 인바운드가 들어올경우 상태 오류 처리 
// *   - PbxRingRecvProc에서 대기콜로 처리 (안강병원)


//------ 2014/3/24 - 11:30 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.71"			"Mar 24, 2014"
// * 보류중 상태에서 "보류하기" 시도시 보류복귀로 처리
// * 보류중이아닌 상태에서 "보류복귀" 시도시 보류하기로 처리


//------ 2014/4/10 - 15:44 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.72"			"Apr 10, 2014"
// * 소켓 read 버퍼 사이즈 MAX_BUFF 로 수정 
// * GetAgentWaitTimeProc 예상 대기시간 구하는 부분에서 
//   c_info.ivr_reqtime 체크로 필요없는 부분에서 구하지 않도록 추가 


//------ 2014/4/22 - 11:25 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.73"			"Apr 22, 2014"
// * 상담원 리스트 - 이름순 정렬(CRM 타상담원상태리스트정렬) 옵션 추가 
// * SortByName
// * [MW_CONF]LISTSORT_NAME=1


//------ 2014/5/9 - 11:45 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.74"				"May 09, 2014"
// * 프로그램 시작시 이전통계 지우는 버그 수정 


//------ 2014/5/26 - 15:07 --- d:\_Svn\Project\ncmw-onpbx\include      "3.6.75"				"May 26, 2014"
// * 고객예상대기시간 구하는 부분 오류 수정 GetAgentWaitTimeProc
// * MAX_AGENT_WAIT를 100으로 변경 

//------ 2014/7/8 - 20:44 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.76"			"July 07, 2014"
// * 툴바 환경 설정 프로토콜 추가(31:AGENT_CMD_TB_CONF) 
// * 툴바 지원 설정 [MW_CONF] TOOLBAR_USE=1

//------ 2014/7/29 - 10:36 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.77"			"July 29, 2014"
// * 동일리포트 출력 버그 수정 (통계관련상태만 체크)140729 


//------ 2014/8/11 - 13:05 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.78"			"Aug 11, 2014"
// * Altibase 시퀜서 수정 - 140811 


//------ 2014/8/11 - 17:18 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.79"			"Aug 11, 2014"
// * 호분배 그룹변경시 자기 호분배 그룹 체크후변경 처리 

//------ 2014/8/25 - 1:31 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.80"				"Aug 25, 2014"
// * 
// *  64비트 처리

//------ 2014/8/28 - 16:11 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.81"			"Aug 28, 2014"
// * 상담원업무중에에 웹관리자에서호분배그룹을 모두 해제했을경우 
// * 전체 호분배 그룹이 오는 버그 수정 AgentSend_GroupCode


//------ 2014/9/2 - 17:02 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.82"			"Sep 02, 2014"
// * sys_info.agtGroup 배열 인덱스 -- 로그 처리 140902

//------ 2014/9/2 - 21:03 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.83"				"Sep 02, 2014"
// * 3.6.81 로그인후 정보패치시 호분배 그룹이 없으면 로그아웃 처리 


//------ 2014/9/15 - 19:34 --- d:\_Svn\Project\ncmw-onpbx\include      "3.6.84"				"Sep 15, 2014"
// * 툴바 멀티팝업 추가 
// * 팝업타입 4번 추가 (그룹코드)


//------ 2014/9/17 - 17:34 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.85"			"Sep 17, 2014"
// * 실요청 카운트 3초후 재전송 (DivFailRealCallCount_DelaySetTimer)
//   DB 통계를 구하는 쿼리보다 nasa_call_log 인서트 쿼리가 늦게 실행된된 경우
//   (MS에서 sql스레드방식으로 인서트) 정상적인 통계를 구하지 못하는 오류 수정

//------ 2014/9/18 - 13:32 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.86"			"Sep 18, 2014"
// * 툴바 band_md_def 필드 추가(bmd_autosel)


//------ 2014/10/2 - 17:21 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.90"			"Oct 02, 2014"
// * * Altibase Utf8 사용여부 SQLDriverConnect

//------ 2014/10/5 - 19:32 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.91"			"Oct 05, 2014"
// * 아스테리스크 클릭투 다이얼 사용시 AST_CTD=1 
// * PBX_CTD_CONNECTING 걸기시도
// * PBX_MAKECALL_CONNECT 연결


//------ 2014/10/7 - 22:11 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.92"			"Oct 07, 2014"
// * 녹취소켓 리스트 뮤텍스 적용 
// * 녹취 소켓 select 방식으로 변경 

//------ 2014/10/15 - 19:57 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.93"			"Oct 15, 2014"
// * 툴바 설정 프로토콜 추가 및 DB 필드 추가 
// * bs_3way,bs_agtinfo_popup,bs_callback,


//------ 2014/10/23 - 17:50 --- d:\_Svn\Project\ncmw-onpbx\include       "3.6.94"			"Oct 23, 2014"	
// * 상담원 콜백 확인처리(29) 번 프로토콜 추가 


//------ 2014/10/24 - 13:14 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.95"			"Oct 24, 2014"
// * Sql 디버거 로그 0초이상 모두 사용

//------ 2014/10/31 - 13:31 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.96"			"Oct 31, 2014"
// * 신규콜백 지연 전송 처리 NEW_CALLBACK_TIMER


//------ 2014/11/13 - 16:11 --- d:\_Svn\Project\ncmw-onpbx\include		"3.6.97"			"Nov 13, 2014"
// * 일반로그인시도시 중복로그인처리시 로그아웃시키는 버그 수정



//------ 2015/01/23 - 13:24 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.01"			"Jan 23, 2015"
// * 툴바 팝업전문에 새창팝업,팝업필터 정보 추가 



//------ 2015/02/09 - 14:23 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.02"		"Feb 09, 2015"
// * 교환기 소켓 select 방식으로 변경 



//------ 2015/02/13 - 16:31 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.03"		"Feb 13, 2015"
// * 대표번호통계(51:CTI_CALLCNT) 프로토콜에 고객대기자수  추가 


//------ 2015/02/27 - 17:18 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.04"		"Feb 27, 2015"
// * 링일경우 CID 전송 스레드 추가 
// * 


//------ 2015/03/04 - 14:13 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.05"		"Mar 04, 2015"
// *	CID_SOCK_USE=0			CID 이벤트 사용여부 
//		CID_SOCK_LINK=0			CID 링크 체크 여부 
//		CID_RECV_TH=2			
//		CID_SEND_TH=2
// * CID 소켓 링크체크 추가 



//------ 2015/03/05 - 17:20 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.06"		"Mar 05, 2015"
// * 보류후 아웃연결후 HELD 후 재연결인 경우 오류 처리 - 150305


//------ 2015/03/09 - 14:28 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.07"		"Mar 09, 2015"
// * CID 타입별 설정 추가 
// * CID_MIN_LEN=4
// * CID_USE_IN=1
// * CID_USE_OUT=1
// * CID_USE_PICKUP=1


//------ 2015/03/17 - 17:33 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.08"		"Mar 17, 2015"
// * 콜백신규건수(29) 오류 수정 150317


//------ 2015/03/18 - 16:39 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.09"		"Mar 18, 2015"
// * 모든상담원상태 로그 제외 옵션추가 (SOCK_93LOG_EXCEPT)



//------ 2015/04/01 - 11:17 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.10"		"Apr 1, 2015"
// * 자동캠페인 추가 
// * 



//------ 2015/04/07 - 13:27 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.11"		"Apr 07, 2015"
// * 호분배 시도중에 아웃바운드 시도할경우 "호분배실패" 로그가 없는 부분 처리 - 150407


//------ 2015/04/09 - 18:26 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.12"		"Apr 09, 2015"
// * 상담원 정보 패치시 누락부분 추가 (link_extension) - 150409



//------ 2015/04/13 - 11:05 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.13"		"Apr 13, 2015"
// * Asterisk 3자통화 상태 추가 - 150413


//------ 2015/04/22 - 9:12 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.14"		"Apr 22, 2015"
// * 상담원상태 실시간 nasa_subscriber 업데이트 
// * AGT_STATE_DB_UPDATE=1


//------ 2015/04/22 - 18:45 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.15"		"Apr 22, 2015"
// * 자동캠페인 오류 수정 


//------ 2015/04/24 - 11:47 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.16a"		"Apr 24, 2015"
// * 캠페인 수정


//------ 2015/04/24 - 15:29 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.16b"		"Apr 24, 2015"
// * 캠페인통화중 상태 변경후 캠페인시작되는 버그 수정 



//------ 2015/04/24 - 18:43 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.16c"		"Apr 24, 2015"
// * 캠페인이 진행중 종료되거나 일시 중시된경우 ,IDLE 무한 반복 되는 버그 수정


//------ 2015/05/11 - 19:23 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.17"		"May 11, 2015"
// * 프리뷰 추가 


//------ 2015/05/12 - 18:29 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.18"		"May 12, 2015"
// * 프리뷰 수정


//------ 2015/06/03 - 10:29 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.19"		"June 03, 2015"
// * 단말기에서 돌려주기 후 받은 다음 끊은 경우 "07" 팝업 오류 수정 



//------ 2015/06/08 - 11:44 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.20"		"June 08, 2015"
// * 웹에서 콜백확인 처리 프로토콜(29) 추가 - (상담원에게 콜백건수 전송) 150608



//------ 2015/06/20 - 0:31 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.21"		"June 20, 2015"
// * 상담원 개인별 아웃바운드 통계건수에 보류아웃바운드 통계건수 포함 + 현재진행중인건수 포함



//------ 2015/06/22 - 19:23 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.22"		"June 22, 2015""
// * 아웃바운드 통계 버그 수정
// * 



//------ 2015/08/10 - 20:58 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.23"		"Aug 10, 2015"
// * 종료 팝업 추가 
// - [MW_CONF] POPUP_END_USE=1


//------ 2015/08/12 - 15:04 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.24"		"Aug 12, 2015"
// * 종료팝업 수정 


//------ 2015/09/16 - 0:27 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.25"		"Sep 16, 2015"
// * 부분 녹취 프로토콜 67번 추가 


//------ 2015/09/17 - 16:10 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.26"		"Sep 17, 2015"
// * 부분녹취 프로토콜 상담원 리턴값 발신자번호 오류 수정 



//------ 2015/09/22 - 10:24 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.27"		"Sep 22, 2015"
// * 상당원 코드 ORDER BY nc_code



//------ 2015/10/13 - 9:46 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.28"		"Oct 13, 2015"
// * 종료팝업 타이머 사용
// * 연결팝업 이벤트 발생시 팝업리스트에 저장후 타이머 이벤트 발생시 사용하도록 수정(타이머시간동안 값이 변경되는 오류 수정 ) 
// * AgentStateUpdateProc 상담원 상태 실시간 DB 업데이트 수정
// * 타이머 대역대 1000으로 축소



//------ 2015/10/15 - 14:26 --- D:\_Svn\Project\ncmw-onpbx\include       ""3.7.29"		"Oct 15, 2015"
// * 종료 팝업 수정 


//------ 2015/10/26 - 17:02 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.30"		"Oct 26, 2015"
// * CRM 조회 정보 녹취 전달 프로토콜 추가 (59)



//------ 2015/10/27 - 18:02 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.31"		"Oct 27, 2015"
// * 녹취 파일 생성완료 프로토콜 추가 (58)


//------ 2015/11/09 - 13:59 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.32"		"Nov 09, 2015"
// * 부분녹취시작/종료(67) - DTMF 또는 통화종료에 의한 결과 전문 전송 



//------ 2015/12/14 - 15:38 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.33"		"Dec 14, 2015"
// * TSA 녹취 정보 입력 60번 프로토콜 추가 
// * 종료팝업 - 사용시간 추가 


//------ 2016/01/12 - 15:54 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.34"		""Dec 30, 2015"
// * 서비스 레벨 추가 
// * [MW_CONF]SERVICE_LEVEL_USE=1 SERVICE_LEVEL_TIME=30




//------ 2016/01/13 - 19:41 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.35"		"Jan 13, 2016"
// * 통화중인고객에게세 다시 Makecall호출시 무시 하도록 
// * 콜센터 통계 패치시 현재 진행중인 통계를 더하여 구함 
// * 서비스 레벨 - 실시간 통계 처리 



//------ 2016/01/14 - 19:00 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.36"		"Jan 14, 2016"
// * 블라인호전환 팝업정보 찾기 조건 추가 
// * PBX_TRANS_ANSWER_RING 최초 가입자번호를 비교조건 추가
// * "//블라인드호전환" 으로 검색 


//------ 2016/01/15 - 13:50 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.37"		"Jan 15, 2016"
// * 당겨받기시 같은그룹아 아닐경우 에도 대표번호는 출력
// * 


//------ 2016/01/19 - 13:31 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.38"		"Jan 19, 2016"
// * 롯데칠성 사용자데이터 LOTTE_USERDATA=1


//------ 2016/01/20 - 16:40 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.39"		"Jan 20, 2016"
// * 영월군청 "호분배재연결"부분 수정 "160120"



//------ 2016/01/25 - 20:18 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.40"		"Jan 25, 2016"
// * 서비스레벨 통계 구하는 부분 설정 체크 오류 수정
// * 통화중인 고객에게 다시 전화를 시도 할경우 무시  - 160125



//------ 2016/01/26 - 11:05 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.41"		"Jan 26, 2016"
// * CID 연동 프로토콜 - 시도/연결추가 
// * 
/*    추가된 부분이 우선 

//기존 
CID_USE_IN=1
CID_USE_OUT=1
CID_USE_PICKUP=1

//추가 
CID_USE_TRY_IN=1
CID_USE_TRY_OUT=1
CID_USE_TRY_PICKUP=1
CID_USE_CON_IN=1
CID_USE_CON_OUT=1
CID_USE_CON_PICKUP=1
*/


//------ 2016/03/03 - 17:29 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.42"		""
// * 링중에 상담원 체크하여 대기인원이 없으면 통화중 처리 (안산도시공사)


//------ 2016/03/16 - 16:55 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.43"		""
// * CID 팝업 에서 종료 팝업 추가 
// * CID_USE_DISCONNECT=1
// * CID_EXTENSION=1


//------ 2016/03/22 - 17:03 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.44"		"Mar 22, 2016"
// * nasa_call_log_oneday - altibase 인서트 안되는 버구수정 
// * 


//------ 2016/03/23 - 15:07 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.45"		"Mar 23, 2016"
// * 블라인드 호전환 호 찾을때 링중에 트랜스퍼한 고객 번호로 찾기 추가 


//------ 2016/03/31 - 23:17 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.46"		"Mar 31, 2016"
// * 상담원 녹취파일 삭제 프로토콜 AGENT_CMD_DEL_RECFILE(98) 추가 - tsa
// * 


//------ 2016/04/20 - 16:44 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.47"		"Apr 20, 2016"
// * 호전환 팝업 찾기 진행시 pbx.szData3 공백일경우 예외처리 KGB


//------ 2016/04/25 - 20:26 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.48"		"Apr 25, 2016"

//------ 2016/05/10 - 16:43 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.49"		"May 10, 2016"
// * 


//------ 2016/05/16 - 10:42 --- D:\_Svn\Project\ncmw-onpbx\include       "3.7.50"		"May 16, 2016"
// * 프리뷰 이벤트 수정
// * 예전방식 CTD-Dial 현재 방식 CTD-Dial -> MakeCallTry / MakeCallTry



//------ 2016/05/19 - 12:19 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.51"		"May 19, 2016"
// * 파주교통약자 연동 추가


//------ 2016/05/24 - 18:36 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.52"		"May 24, 2016"
// * 서비스레벨 방식 추가 
// * [MW_CONF]SERVICE_LEVEL_USE=2 SERVICE_LEVEL_TIME=30



//------ 2016/06/02 - 13:17 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.53"		"June 02, 2016"
// * 콜백방식-관리자분배 추가 



//------ 2016/06/15 - 15:32 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.55"		"June 15, 2016"
// * 패스워드 변경 로직 추가
// * alert메세지 프로토콜 추가 



//------ 2016/06/23 - 15:28 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.56"		"June 23, 2016"
// * 한메디 녹취 파일 변경 추가 
// * RECFILE_RENAME_TYPE=5



//------ 2016/07/11 - 13:20 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.57"		"July 11, 2016"
// * 상담원 대기 인원 구하기 수정(안산도시공사)
// * 콜백자동확인시 날짜타입 변경 


//------ 2016/07/20 - 14:48 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.58"		"July 20, 2016"
// * NewPasswordComplexityCheck 패스워드 복잡도 2 추가 
// * 


//------ 2016/07/20 - 20:12 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.59"		"July 20, 2016""
// * // * 트렁크 회선 500개 define - 컴파일시 _TRUNK500 
// * 


//------ 2016/08/02 - 20:50 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.60"		"Aug 02, 2016"
// * 중복실행 PID 체크에서 ONMW 추가 
// * 툴바 DB 없이 사용했을경우 발행하는 오류 처리 


//------ 2016/08/24 - 10:09 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.61"		"Aug 24, 2016"
// * 이전상태 변경 옵션처리 PREV_STATE_CHANGE=0
// * 전화걸기 번호 체크에 * # 추가 


//------ 2016/08/24 - 13:10 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.62"		"Aug 24, 2016"
// * 이천행복콜 픽업시 배차소켓데이터 전송추가 
// * 소켓 종료시 재접속 시간 10초에서 1초로 변경 


//------ 2016/08/29 - 19:13 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.63"		"Aug 29, 2016"
// * 아웃바운드시도, 보류중, 보류후아웃바운드 연결시 단말기에 Busy처리되는 경우에 
// * 인바운드 처리 하지 않고 대기콜로 처리 하지 않도록 수정 



//------ 2016/08/29 - 21:57 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.64"		"Aug 29, 2016"
// * / 통화중 / 후처리 상태일때 링크연동하여 전화받지않기설정 NOTREADY_NOANSWER_USE=1
// * 


//------ 2016/08/31 - 16:34 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.66"		"Aug 31, 2016"
// * 종료팝업 대기 시간  설정 추가 POPUP_WAIT_HOLD_END=1



//------ 2016/09/01 - 22:03 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.67"		"Sep 01, 2016"
// * 대기상담원 구할경우 특정 코드 로 찾기 
// * IDLE_COUNT_GROUP=S301


//------ 2016/09/08 - 17:50 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.68"		"Sep 08, 2016"
// * 링크소켓 재연결시 오류 수정 
// * sqlpush 로그


//------ 2016/09/26 - 18:59 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.70"		"Sep 26, 2016"
// * 자동배차 링크 타이머 추가 
// * 아웃바운드 모드일경우 수신거부 추가 


//------ 2016/10/05 - 20:53 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.71"		"Oct 05, 2016"
// * 상담원ID,이름을 로그인및 로그아웃시 녹취에 전달 (전남교통약자)
// * 설정 REC_AGTID_SEND=1



//------ 2016/10/06 - 18:42 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.72"		"Oct 06, 2016"
// * 팝업정보에 그룹3정보 추가 


//------ 2016/10/17 - 22:45 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.73"		"Oct 17, 2016"
// * 
// * 


//------ 2016/10/21 - 14:53 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.75"		"Oct 21, 2016"
// * 제니엘 패스워드 DB 암호화 저장  Kisa-Seed / base64
// * 설정 [MW_CONF] DB_PWD_ENCODE=1



//------ 2016/10/27 - 20:20 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.77"		"Oct 27, 2016""
// * 그룹별 대기자수 상담원클라이언트 전송 
// * [MW_CONF] AGT_GROUP_WAITCNT=1



//------ 2016/10/31 - 17:31 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.78"		"Oct 31, 2016"
// * 패스워드 변경 - 로그인 체크 주석 처리 


//------ 2016/11/02 - 18:14 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.79"		"Nov 02, 2016"
// * 결제ARS 연결 프로토콜 추가 
// * /------ 2016/10/26 - 16:17 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.76"		"Oct 26, 2016"
// * 블라인드트랜스퍼가 IPX에서 되지 않아 클릭투다이얼후 모니터호전환방식으로 처리 
// * [MW_CONF] UNATTEND2ATTEND=1


//------ 2016/11/04 - 11:50 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.80"		"Nov 04, 2016"
// * 오토피온 전문 전송 BYTE 로 전송
// * 오포피온 대표번호 포트 구하는부분 회사명 앞 4자리로 구하도록 수정 



//------ 2016/11/15 - 19:16 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.81"		"Nov 15, 2016"
// * 호분배 시도시 중복 처리 경우 처리 pAgent->Div_Find (kmi)


//------ 2016/11/22 - 9:51 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.82"		"Nov 22, 2016"
// * LINK 연동 프로토콜 추가(L211) - 통화중 차단호 처리 
// * PbxBusyBlockProc


//------ 2016/12/21 - 11:53 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.83"		"Dec 21, 2016"
// * 센터링크 연동하여 콜백처리 결과를 DB 업데이트 (농정원) - 프로토콜 29번 
// * nasa_call_id  사용안하도록 주성 처리 



//------ 2016/12/29 - 14:52 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.84"		"Dec 29, 2016"
// * 센터링크 연동 콜백처리 결과값 처리 부분 수정 


//------ 2017/01/24 - 12:32 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.85"		"Jan 24, 2017"
// * 상담원인덱스 디버깅 sys_conf.DbgSt.d_agt_idx 



//------ 2017/02/01 - 15:03 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.86"		"Feb 01, 2017"
// * agentrpt 테이블 변경시간 기준 날짜 사용 (스트라이크존)


//------ 2017/02/07 - 17:38 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.87"		"Feb 07, 2017"
// * 상담원 대기상태변경과 IDLE 타이머 중복 실행시 오류 처리 (경동택배) - 170207 



//------ 2017/02/10 - 14:37 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.88"		"Feb 10, 2017"
// * 클릭투다이얼 요청후 상태변하기전에 호분배가 되는 경우 처리 
// * makecall 요청후 한후 pAgent->Div_Find = TRUE; 



//------ 2017/02/13 - 20:02 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.89"		"Feb 13, 2017"
// * 고객예상시간 설정시에만 평균상담시간 구하기

 

//------ 2017/02/16 - 9:59 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.90"		"Feb 16, 2017"
// * 이전상태 변경 옵션처리 PREV_STATE_CHANGE=0 / 수정(로그인후)




//------ 2017/03/02 - 18:40 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.91"		"Mar 02, 2017"
// * 오토피온 핑 체크 오류 수정 
// * 



//------ 2017/03/21 - 18:26 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.92"		"Mar 21, 2017"
// * 호분배 시도중에 보류를 눌렀을경우 ANSWER_FAIL 에서 호분배실패 상태 변경후 
// 포기호 상태로 변경 처리 후 IdleTIme 에서 포기호 상태로 변경  (비츠적용)


//------ 2017/04/07 - 17:27 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.93"		"Apr 07, 2017"
// * "자동대기" 상태 추가  코드 W999
//* [MW_CONF] 
//* WAITIDLE_TIME=5000 
//* PREV_STATE_CHANGE=0


//------ 2017/04/12 - 19:41 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.94"		"Apr 12, 2017"
// * 모니터 트랜스퍼 팝업 사용 여부
// * ATTEND_TRANS_POPUP=1


//------ 2017/04/13 - 18:03 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.95"		"Apr 13, 2017"
// * dst_num 추가 (인바운드/아웃바운드 공통사용)



//------ 2017/04/18 - 13:22 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.96"		"Apr 18, 2017"
// * 자동캠페인 pds_set gbn 값 'A' 상담원 내선번호로 아웃바운드 


//------ 2017/04/20 - 10:20 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.97"		"Apr 20, 2017"
// * 자동캠페인 최대다이얼시간 후 전화 끊기 추가 (이지수능)


//------ 2017/04/27 - 21:10 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.98"		"Apr 27, 2017"
// * 캠페인 실패 코드 추가 MakecallFailPdsCode



//------ 2017/05/04 - 14:08 --- d:\_Svn\Project\ncmw-onpbx\include       "3.7.99"		"May 04, 2017"
// * 원주콜 오토피온연동 - 30분이전고객 판단 (인입기준->배차요청기준) 변경
// * [MW_CONF]CARDISP_USAGER_DB_LOG=1
//* 테이블 필드 추가 
//* ALTER TABLE nasa_usager ADD COLUMN car_type character varying(3);
//* ALTER TABLE nasa_usager ADD COLUMN car_time character varying(21);
//* ALTER TABLE nasa_usager_oneday ADD COLUMN car_type character varying(3);
//* ALTER TABLE nasa_usager_oneday ADD COLUMN car_time character varying(21);



//------ 2017/05/19 - 17:30 --- d:\_Svn\Project\ncmw-onpbx\include       "3.8.01"		"May 19, 2017"
// * 51번 52번 웹통계 전송 부분에 모니터호전환/블라인드 호전환 시도건수 추가 


//------ 2017/05/30 - 13:28 --- d:\_Svn\Project\ncmw-onpbx\include       "3.8.02"		"May 30, 2017"
// * [MW_CONF] VRS_DIVINFO_SEND=2 녹취에 대표번호 정보 전송 



//------ 2017/06/07 - 17:41 --- d:\_Svn\Project\ncmw-onpbx\include       "3.8.03"		"June 07, 2017"
// * 아웃바운드 후 돌려주기 팝업의 고객번호 미표시 수정 (dst_num 변경)



//------ 2017/06/09 - 14:52 --- d:\_Svn\Project\ncmw-onpbx\include       "3.8.04"		"June 09, 2017"
// * 호분배 재요청 기능 추가에 따른 호분배 관련 통계및 상태 변경 처리(강남성심병원) - 170602
// * 상담원 상태 추가(AGT_NODIV_TRY / AGT_NODIV_FAIL)



//------ 2017/07/03 - 18:31 --- d:\_Svn\Project\ncmw-onpbx\include       "3.8.05"		"July 03, 2017"
// * 상담원 인덱스 오류 수정(DWORD 초기값 0 처리 / 64Bit, 웹에서 상담원수정시 발생)



//------ 2017/07/17 - 13:14 --- d:\_Svn\Project\ncmw-onpbx\include       "3.8.06"		"July 17, 2017"
// * 단말 ANSWER 체크(RING 이벤트만 존재할경우 상태가 묶이는 경우)
// * AGT_ANSWER_TIMER 타이머 추가 (MAX_ANSWER_TIME : 5분) 이내 Answer 이벤트 발생 안할경우 대기상태로 변경 


//------ 2017/11/07 - 14:39 --- d:\_Svn\Project\ncmw-onpbx\include       "3.8.07"		"Nov 07, 2017"
// * 악성고객응대 설정 
// * DB 테이블 nasa_black_cunsumer 추가 
// * 설정 [MW_CONF] BLACK_CONSUMER_USE=1



//------ 2017/11/08 - 21:03 --- d:\_Svn\Project\ncmw-onpbx\include       "3.8.08"		"Nov 08, 2017"
// * 녹취청취 URL 요청 및 응답 (36번) 프로토콜 추가 
// * [MW_CONF]
// * RECFIlE_STREAM_TH_USE=1
// * RECFIlE_STREAM_TIME=500
// * RECFIlE_STREAM_URL=http://203.239.159.133:8090/wavurl/



//------ 2017/11/13 - 17:43 --- d:\_Svn\Project\ncmw-onpbx\include       "3.8.09"		"Nov 13, 2017"
// * DB 쿼리 타임아웃 설정 옵션 추가 
// * [DATABASE_INFO] STATEMENT_TIMEOUT=30000



//------ 2017/12/13 - 15:09 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.10"		"Dec 13, 2017"
// * 고객대기자 검색할때 순위우선 분배 가능하도록 DB 로딩시 정렬(nasa_div_group /ng_group_rank)



//------ 2017/12/15 - 17:44 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.11"		"Dec 15, 2017"
// * 고객대기자 검색할때 순위우선 분배 가능하도록 재 수정 


//------ 2017/12/15 - 21:36 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.12"		"Dec 15, 2017"
// * 아웃바운드 프리픽스코드제거


//------ 2017/12/18 - 10:24 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.13"		"Dec 18, 2017""
// * 아웃바운드 프리릭스 코드 제거 
// * [MW_CONF] 
//*  MAKECALL_PREFIX_CODE_01=9
//*  MAKECALL_PREFIX_MIN_NUM=4


//------ 2017/12/20 - 20:41 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.14"		"Dec 20, 2017"
// * 소그룹번호가 없이 아웃바운드 진행시 해당 번호로 VICID 사용
// * nasa_subscriber / agt_cellnum 사용 
// * KMI 광화문



//------ 2017/12/21 - 15:47 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.15"		"Dec 21, 2017"
// * 링크 재연결시 통화중인 상담원 상태를 대기 상태로 변경 처리 
// * [MW_CONF] LINK_CONNECT_SET_IDLE=1



//------ 2017/12/29 - 14:56 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.16"		"Dec 29, 2017"
// * 아웃바운드 프리릭스 코드 제거 (한자리이상)
// * [MW_CONF] 
//*  MAKECALL_PREFIX_CODE_01=010
//*  MAKECALL_PREFIX_CODE_02=9
//*  MAKECALL_PREFIX_CODE_03=
//*  MAKECALL_PREFIX_MIN_NUM=4



//------ 2018/01/03 - 18:28 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.17"		"Jan 03, 2018"
// * 직통대기 상태 추가 AGT_DID_IDLE / W9998 - 호분배받지않고 DID 인입사용


//------ 2018/01/08 - 21:24 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.18"		"Jan 08, 2018"
// * 93 상태 응답 전문의 마지막 항목에 순번 추가 


//------ 2018/01/15 - 20:15 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.19"		"Jan 15, 2018"
// * 로그인시간 업데이트 여부
// * [MW_CONF] LOGON_TIME_UPDATE=1



//------ 2018/01/17 - 15:59 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.20"		"Jan 17, 2018"
// * 상담원상태94 로그파일 별도 처리  [MW_CONF] SOCK_94LOG_EXCEPT=1
// * 상담원모드85 로그파일 별도 처리  [MW_CONF] SOCK_85LOG_EXCEPT=1


//------ 2018/02/08 - 14:54 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.21"		"Feb 08, 2018"
// * Db Connect timeout 옵션 추가 
// * DB_CONNECT_TIMEOUT = 1


//------ 2018/02/20 - 18:01 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.22"		"Feb 20, 2018"
// * 대량콜백건 존재시 메모리누수에 대한 처리 
// * 이벤트 발생한 콜백 그룹만 구하도록 수정(CallbackNewCountSendProc)



//------ 2018/02/21 - 17:31 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.23"		"Feb 21, 2018"
// * 로그인 없는 상담원 소켓 정리(5분마다)  - 180221


//------ 2018/03/07 - 16:16 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.24"		"Mar  7, 2018"
// * 보류후 아웃시도/연결 도중 보류된호가 종료된경우 아웃바운드 통계상태 처리 -180307 (철도시설관리)



//------ 2018/03/07 - 18:28 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.25"		""Mar 7, 2018"
// * 미분배 오류(상담원재요청시대기로빠지는 경우 요청 카운트 증가) 수정 - 180307


//------ 2018/03/22 - 10:30 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.26"		"Mar 22, 2018"
// * 호분배 재연결 시도후 실패경우 상담원 포기호로 처리 (KRT)
// * 53번 프로토콜 추가 


//------ 2018/03/23 - 12:05 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.27"		"Mar 23, 2018"
// * 링크에서 상태 받아서 처리이전 로그인상태로 변경하는 부분 주석 
// * 재호분배 로직에서 미로그인 상담원이 당겨받기한경우 호분배 실패 처리 


//------ 2018/03/30 - 18:51 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.28"		"Mar 30, 2018"
// * 재연결로직 수정 - 로그아웃시 



//------ 2018/04/04 - 18:15 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.29"		"Apr 04, 2018"
// * 고객대기자 정보 웹전송 프로토콜 55번 추가 



//------ 2018/04/05 - 12:14 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.30"		"Apr 05, 2018"
// * 캠페인시도시 자동받기 옵션 추가 - [PDS] PDS_AUTOANSWER_TIME=100
// * 상담원 별 캠페인 시도, 연결 처리 180405



//------ 2018/04/09 - 21:28 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.31"		""
// * 픽업시 최근 링울린 단말 찾기 
// * 호분배 시도중에 발신시 미분배 실패처리, 호분배성공시 호분배 연결 처리  - 180409



//------ 2018/04/13 - 13:03 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.32"		"Apr 13, 2018"
// * 통화중 클릭투다이얼후 보류 이벤트가 나중에 발생하는 경우 
// * CTD Answer 에서 보류 콜 정보 보관 / 홀드에서 보류콜정보 비교하여 업데이트 



//------ 2018/04/25 - 11:19 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.33"		"Apr 25, 2018"
// * 아웃바운드호롤 돌려주는 경우 돌려주기 팝업 발신자번호 오류 수정 
// * 


//------ 2018/05/17 - 17:26 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.34"		"May 17, 2018"
// * 전화받기 중복 예외 처리 - 180517



//------ 2018/05/25 - 18:15 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.35"		"May 25, 2018"
// * 픽업시에 픽업성공과 이벤트와 대상 연결이 동시에 이루어졌을때 처리 - 180525



//------ 2018/06/07 - 20:03 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.36"		"June 07, 2018"
// * 92,93,94 평균통화시간, 평균대기시간, 평균후처리시간, 누적통화시간 추가 


//------ 2018/07/04 - 10:44 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.37"		"July 04, 2018"
// * 3자통화 이벤트 처리 (교환기 컨퍼런스 방식)
// * PBX_CONFER_DISCONNECT ( L221 ) 이벤트 추가 : 3자통화 종료시 발생 ( 애경산업 적용 )
// * 일반동보방식 일부 수정 



//------ 2018/07/06 - 9:28 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.38"		"July 06, 2018"
// * 일반동보 추가 (중구청)
// * pds_group3 "0000" 설정 



//------ 2018/07/20 - 13:43 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.39"		"uly 20, 2018"
// * sha256 암호화 처리 
// * [MW_CONF]PWD_SHA256_CONV = 1



//------ 2018/07/24 - 17:16 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.41"		"July 24, 2018"
// * 일반동보시 - 재전송 추가 
// * 3자통화 2차상원 상태 변경 처리 



//------ 2018/08/20 - 11:29 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.42"		"Aug 20, 2018"		
// * Seed 오류 부분 수정 



//------ 2018/08/20 - 15:36 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.43"		"Aug 20, 2018"
// * 
// * IVR Data 존재시 캠페인 종료 중지 하도록 수정 - 180820


//------ 2018/08/28 - 16:59 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.44"		"Aug 28, 2018"
// * 통화중 차단호 발생시 PBX_BUSY_BLOCK / PBX_DISCONNECT 2개 발생시 무시하도록(비츠) - 180828
// * 통화중 대기콜 인입시 기존 DB 상태 입력후에 WAIT 정보 USE로 옮기기 (리사이클파크) - - 180828
// * CID 전송 스레드에 Makecall 프로토콜 추가 - 서울대건강검진


//------ 2018/09/14 - 14:17 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.45"		"Sep 14, 201"
// * 웹소그룹수정시 상담원 사용시간 초기화 되는 버그 수정 


//------ 2018/10/08 - 19:29 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.46"		"Oct 08, 2018"
// * CID 소켓 끊기 추가 


//------ 2018/11/01 - 10:48 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.47"		"Nov 01, 2018"
// * 서울대학교건강검진 - 캠페인추가 
// * 픽업시 - 오토피온콜리스트 ring 삭제 
// * 1차아웃바운드후 2차아웃바운드후에 호전환 돌려주기 팝업의 cid 미표시 부분 수정  (PbxTransAnswerSucc)



//------ 2018/11/02 - 15:39 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.48"		"Nov 02, 2018"
// * 아웃바운드 팝업시 보류후정보 추가 



//------ 2018/11/06 - 14:15 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.49"		"Nov 06, 2018"
// * // 181106 - PBX_CTD_ANSWER 의 고객 번호 그래로 유지 하도록 - 181106 나은병원
// *  // 181106  PBX_CTD_DIALING 에서 AGT_HOLD_OUT_TRY  처리 한 경우 makecall_try에서 처리 하지 않도록 (나은병원) - 181106


//------ 2018/11/19 - 15:41 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.50"		"Nov 19, 2018"
// * UnAttend / Attend Try  CallerTel 과 customtel 을 보류 번호화 호전환 번호로 변경 
// * 통계 처리  - 한마음 창원변원



//------ 2018/12/04 - 17:29 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.51"		"Dec 04, 2018"
// * 인바운드실패시 이전상태로 변경 (KRT)
// * [MW_CONF] INFAIL_PREV_STATE=1


//------ 2018/12/11 - 12:06 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.52"		""Dec 12, 2018"
// * PbxCtdDialingProc 이벤트의 CID로 사용 (KRT)-181211 옵션 추가 
// * CTD_DIAL_CID_UPDATE=1


//------ 2018/12/31 - 11:43 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.53"		"Dec 31, 2018"
// * 클릭투다이얼시 VCID 번호에 '*'표시 허용 



//------ 2018/12/31 - 14:18 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.54"		"Dec 31, 2018"
// * 인바운드실패시(픽업당함) 이전 부재중 상태로 변경 처리(KRT) - 181231
// * [MW_CONF] INFAIL_PREV_STATE=1



//------ 2019/01/02 - 20:57 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.55"		"Jan 02, 2019"
// * 상담원 자동응대 추가 
// * [MW_CONF] AUTOMENT_TRUNK_NUM=99102


//------ 2019/01/09 - 12:57 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.56"		"Jan 09, 2019"
// * 캠페인 수정시 m_nMaxRank = 1 초기화 되는 부분 수정 (서울대건강검진)
// * 



//------ 2019/01/09 - 18:12 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.57"		"Jan 09, 2019"
// * 상담원 설정한 날짜에 호분배및 콜백 제외 처리 - 190109
// * [MW_CONF] DIV_DATE_SET=1 
// * 호분배 상담원 찾을때 IsDivDateCheck 설정체크 
// * 상태변경 후 고객대기가 있는 경우 IsDivDateCheck 설정체크
// * ALTER TABLE NASA_SUBSCRIBER ADD COLUMN CALLBACK_ST_DT character varying(20);
// * ALTER TABLE NASA_SUBSCRIBER ADD COLUMN CALLBACK_ED_DT character varying(20);
// * ALTER TABLE NASA_SUBSCRIBER ADD COLUMN CALLBACK_CHECK character(1);



//------ 2019/01/18 - 10:31 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.58"		"Jan 18, 2019"
// * 픽업시 VCID로 한번더 찾기 


//------ 2019/01/21 - 16:11 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.59"		"Jan 21, 2019"
// * 픽업실패시 전문에 콜아이디 없는 부분 처리 (기업데이터)



//------ 2019/01/31 - 18:53 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.61"		"Jan 31, 2019"
// 자동응대 블라인드 추가 


//------ 2019/02/20 - 16:35 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.62"		"Feb 20, 2019"
// * 호분배시도시, 아웃바운드한경우 (disconnect이벤트 없이 
//   PBX_BUSY_BLOCK 만 발생한경우 ) 예외 처리 190220



//------ 2019/02/25 - 11:49 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.63"		"Feb 25, 2019"
// * Direct호분배 휴가자 우선 처리 
// * 디지트트랜스퍼 정보 녹취에 전달(55) - 분당핫라인


//------ 2019/02/28 - 16:23 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.64"		"Feb 28, 2019"
// * 자동대기(타이머) 상태시간 DB 미입력 부분 수정  190228



//------ 2019/03/11 - 19:57 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.65"		"Mar 11, 2019"
// * 고객대기찾는 로직 오류 수정 -  이전 찾은 동일한 순위의 다른 그룹 부터 찾는다 190311 
// * WaitList_GetData



//------ 2019/03/12 - 16:32 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.66"		"Mar 12, 2019"
// * 예상시간안내 배수 설정 - 190312
// * [MW_CONF] WAITTIME_MULTIPLY=2   (아이퀘스트 2배수 설정 요청)



//------ 2019/03/21 - 17:13 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.67"		"Mar 21, 2019"
// * 예측캠페인 방식 추가(주기적발송)  - 서울대강남센터
//		발송건수    =   ps_line_start    
//		지연시간(초)  = ps_line_end
// * Maibox_FilenameFatch0XFF    -> Parsing_Fatch0XFF  변경 
//   버퍼오버플러우 발생(롯데삼강) 190321


//------ 2019/03/28 - 18:44 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.68"		"Mar 28, 2019"
// * 블라인드 동시 요청 예외 처리  pAgent->BlindLastTime



//------ 2019/04/01 - 17:37 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.69"		"Apr 01, 2019"
// * 자신의 호분배시도콜을 당겨받는 경우 예외처리 (농정원)



//------ 2019/04/05 - 16:04 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.70"		"Apr 05, 2019"
// * 블라인드 07번 팝업의 CID 오류 처리 (아웃바운드호를 블라인드)



//------ 2019/04/09 - 17:57 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.71"		"Apr 09, 2019"
// * 고객대기시간 
// * WAITTIME_MIN=60
// * WAITTIME_ADD=60



//------ 2019/04/15 - 17:00 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.72"		"Apr 15, 2019"
// * IVR 소켓 정리 


//------ 2019/04/18 - 11:59 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.73"		"Apr 18, 2019"
// * 보류, 보류복귀 중복 예외처리 
// * HoldLastTime / HoldRejectLastTime



//------ 2019/04/23 - 17:33 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.74"		"Apr 24, 2019"
// * 지정단말 상태 정보 전송 프로토콜 추가 AGENT_CMD_EVENT_PHONE 72번 (이대목동 EMR)
/*   [EP_CONF]
      EP_COUNT=4  // 사용할 개수 설정
	  EP_NUM_001=07042622854
	  EP_NUM_002=07042622854
	  EP_NUM_003=07042622854
	  EP_NUM_004=07042622854
*/
// *  선택당겨받기 변경 - 기존 프로토콜에 추가 



//------ 2019/04/30 - 8:57 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.75"		"Apr 30, 2019"
// * 92상태변경시 소속 그룹 업데이트 여부 옵션처리(기본미처리) - 190430
// * [MW_CONF] STATUS92_GROUP_UPDATE=0



//------ 2019/05/14 - 18:00 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.76"		"May 14, 2019"
// * 상담원 호분배그룹 수정시 사용중인 그룹이 삭제되면, 설정된 호분배그룹으로 처리(KRT)
// * 상담원상태전송(92~94)프로토콜에 설정된 코드의 누적시간 전송 
// * [MW_CONF] 
// *  STATE_ITEM_TYPE=1
// *  STATE_SUM_CODE_01=W004
// *  STATE_SUM_CODE_02=W004
// *  STATE_SUM_CODE_03=W004
// *  STATE_SUM_CODE_04=W004
// *  STATE_SUM_CODE_05=W004
// *  STATE_SUM_CODE_06=W005


//------ 2019/05/15 - 12:18 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.77"		"May 15, 2019"
// * 관리자웹에서 상태 변경 추가 
 

//------ 2019/05/28 - 17:27 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.78"		"May 28, 2019"
// * PBX_CTD_DIALING  pAgent->customtel 업데이트 처리 
// * CTD_DIAL_CID_UPDATE=1 디폴트 설정으로 변경 



//------ 2019/06/14 - 15:57 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.79"		"June 14, 2019"
// * 부재중콜 IVR 연동 처리 (웰스라이프) - 190614 
//   56번 프로토콜 추가
//   [MW_CONF] LOGOUT_INFAIL=IVR REJECT 인덱스 
// * 로그인 프로토콜에 결과 메세지 전송 



//------ 2019/06/14 - 16:39 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.80"		"June 14, 2019"
// * 
// * 상담원상태전송(92~94)프로토콜에 설정된 코드의 누적시간 전송 개수 추가 - KRT
// *  STATE_SUM_CODE_07=W004
// *  STATE_SUM_CODE_08=W005
// *  아웃바운드 실패로 묶이는 경우 처리 - PbxDiconnectProc



//------ 2019/06/20 - 22:00 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.81"		"June 20, 2019"
// * 부재중상태 호인입 차단(교환기방식설정)
// * [MW_CONF] NOTREADY_NOANSWER_USE=2


//------ 2019/06/24 - 16:06 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.82"		"June 24, 2019"
// * [MW_CONF]STATE_ITEM_TYPE=2  93번 프로토콜에 타입추가 


//------ 2019/07/01 - 19:45 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.83"		"July 01, 2019"
// * 아웃바운드 후 상태 지정 
// * nasa_subscriber 추가필드 로딩 - 190701 
// * [MW_CONF] LOAD_SUBSCRIBER_EXT2=1


// 고객사연동 스레드 추가 (시각장애인)
// CUSTLINK_TH=5
// CUSTLINK_DIV_END=5(reject_idx)  호분배 종료 팝업시 nasa_reject 테이블사용 



//------ 2019/07/23 - 12:06 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.84"		"July 23, 2019"
// * 기업데이터 아웃바운드 통화 후 상태 지정 요청 
//  아웃바운드 통화 연결 후 상태 지정 
//  아웃바운드 통화 실패 후 상태 지정 
//  인바운드 통화 연결 후 상태 지정 
//  인바운드 통화 실패 후 상태 지정 


//------ 2019/07/25 - 18:33 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.85"		"July 25, 2019"
// * 상담원상태건수 캠페인 연결건수 오류 수정 



//------ 2019/08/06 - 17:52 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.86"		"Aug 06, 2019"
// * 로그아웃시 부재중착신전환 설정 - 기업데이터 
// * [MW_CONF] LOGOUT_NOANSWER_USE=1


//------ 2019/08/07 - 13:42 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.87""		"Aug 07, 2019"
// * 픽업 CallID로 PBX_BUSY_BLOCK 이벤트 발생후 콜연결된 경우 예외처리 - 직방  
//   PBX_BUSY_BLOCK 이벤트 때문에 전화대기가 되지 않은 경우 



//------ 2019/08/21 - 15:36 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.88"		"Aug 21, 2019"
// * CID 링크 오류 횟수 설정 
// * [MW_CONF] SET_CID_LINK_COUNT=60   // 10분 (1=10초) 보라매병원
// * [MW_CONF] VIP_CODE 고객대기자리스트 맨앞에 추가 (서울대강남센터)



//------ 2019/09/09 - 10:41 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.89"		"Sep 09, 2019"
// * 상담원 (웹수정)패치시 rejectStatus 복사 



//------ 2019/09/24 - 10:30 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.90"		"Sep 24, 2019"
// * 호분배픽업당할때 호분배 이전상태로 변경 KRT 
// * 링크 오류 처리 카운트 옵션 처리 [MW_CONF] AGT_LINK_COUNT=30   (N*5초 ) + 30초



//------ 2019/10/07 - 9:27 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.91"		"Oct 07, 2019"
// * 보류후 통화상태시 통화종료되면 자동재연결 처리  - 상계백병원
// * [MW_CONF] AUTO_HOLD_REJECT=타이머지연시간


//------ 2019/10/08 - 17:35 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.92"		"Oct 08, 2019"
// * 상담원 고객대기자 정보 전송 추가 
// * [MW_CONF] AGT_WAITINFO_USE=1


//------ 2019/10/15 - 12:42 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.93"		"Oct 15, 2019"
// * 통화중간에 웹에서 호분배 그룹을 변겨하였을경우 
//   통화후에 통계그룹 변경 처리(비츠) - 191015


//------ 2019/10/23 - 18:58 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.94"		"Oct 23, 2019"
// * // 상담원 고객대기자 전송타입// 0: 그룹최대대기자수, 1: 대기자수 합 옵션추가 
// * [MW_CONF]AGT_WAITCNT_TYPE=1



//------ 2019/10/29 - 16:23 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.95"		"Oct 29, 2019"
// * 아웃바운드 시도시 콜백 업데이트 (상계백병원) - 191029
// * [MW_CONF] CB_OUTTRY_UPDATE=1



//------ 2019/11/06 - 17:41 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.96"		"Nov 06, 2019"
// * 호분배재연결 요청시 그룹대기일때 요청 연결수 로 우선 순위처리(웰스라이프) -191104
//   [MW_CONF] REDIV_REQCOUNT_DIV=1 
// * 상담원 그룹변경시 통화중인경우 통화후에 처리 (비츠)



//------ 2019/12/02 - 21:05 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.97"		"Dec 02, 2019"
// * 오토피온 정보동의 추가 
//  [MW_CONF] 
//  CAR_RECV_TH=1
//  AGREE_TRUNK_NUM1=99103
//	AGREE_TRUNK_NUM2=99104
//  테이블 atp_agree 추가 


//* [MW_CONF] DIV_WAITCALL_CLEAR=1 통화중 호분배 대기콜 존재시 삭제 옵션 처리 (농정원) - 191203



//------ 2019/12/05 - 13:46 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.98"		"Dec 05, 2019"
// * 아웃바운드 시도시 콜백 업데이트 (상계백병원) - 버그 수정
// * [MW_CONF] CB_OUTTRY_UPDATE=1



//------ 2019/12/12 - 16:01 --- e:\_Svn\Project\ncmw-onpbx\include       "3.8.99"		"Dec 12, 2019"
// * 아웃바운드 중복요청시 차단 수정 1000>=0 




//------ 2019/12/17 - 12:32 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.01"		"Dec 17, 2019"
// * 실시간통계구하는부분에서 대기중인 요청건은 DB에 값이 없어 대기자자 리스트로 건수 추출 - 이대목동 
//*  HOLD_MAKECALL_BLOCK=1   보류상태에서 발신 금지 처리 (보류자동복귀사용중일때만) 상계백병원  - 191217 
//* CURTEL_MAKECALL=1    현재통화중인번호로 발신 처리 상계백병원  - 191217 



//------ 2019/12/23 - 15:44 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.02"		"Dec 23, 2019""
// IDLE 타이머도중 클릭투 다이얼시도시 종료된 콜정보가 보류로 처리 되는 경우(상계백병원) - 191223
// * 


//------ 2019/12/23 - 19:50 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.03"		""
// * 팝업타입 14추가 -마지막 상태코드(오리온)
// * 



//------ 2020/01/07 - 13:29 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.04"		"Jan 07, 2020"
// * 자정기준으로 인입된 콜 통계 정보를 다음날통계로 변경 처리 (이대목동)


//------ 2020/01/08 - 16:23 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.05"		"Jan 08, 2020"
// * 포기발생시 고객대기자 삭제 프로토콜(48)에 요청호의 실시간 건수 삭제처리 - 서울백병원



//------ 2020/01/16 - 12:25 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.06"		"Jan 16, 2020"
// * 모니터호전환/ 블라인드호전환 상태일때
//   nasa_agentrpt의 caller_tel은 통화자번호, customer_tel는 호전환 번호로 처리 - 이대목동 적용 



//------ 2020/01/21 - 10:34 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.07"		"Jan 21, 2020"
// * 오토피온 AP 다중 사용 처리 
// * 테이블 ap_cardisp_conf (trunk_set의 TRK_INFO0 코드로 ap_code  매칭)
// * ApSock.cpp 파일 추가
// * 설정
//   AP_CARDISP_USE=1
//	 TRKSET_LOAD2=1


//------ 2020/01/29 - 12:21 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.08"		"Jan 29, 2020"
// * config 파일 로그 처리 및 정리 



//------ 2020/02/01 - 2:47 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.09"		"Feb 01, 2020"
// * AP 다중서버 연동 버그 (CPU 200%) 수정 
//   AP 소켓 스레드에 tval.tv_sec = 1; 


//------ 2020/02/10 - 9:31 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.10"		"Feb 10, 2020"
// * AP 서버 재연결 오류 수정 (인천콜)



//------ 2020/02/18 - 12:55 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.11"		"Feb 18, 2020"
// * 픽업대상 상태 묶이는 경우 체크 처리 (엘비전)  Ring이벤트없이 픽업당했을때
//  [MW_CONF] PICKUPED_IDLE_TIME=5



//------ 2020/02/18 - 13:48 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.12"		"Feb 18, 2020"
// * 호분배 시도중에 아웃바운드를 한경우 (호분배이벤트X) 통계처리 - 해피콜 




//------ 2020/02/18 - 19:25 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.13"		"Feb 18, 2020"
// * 실제 호분배 링이벤트 발생했을때만 받기 처리(애터미아자) - 200218
//  [MW_CONF] REALRING_OFFHOOK=1


//------ 2020/02/27 - 11:09 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.14"		"Feb 27, 2020"
// * 호전환시도중에 실패처리시 HOut실패로 처리  - 200227



//------ 2020/03/16 - 17:59 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.15"		"Mar 16, 2020"
// * 오토피온 MSSQL 쿼리 타임아웃설정 
//  CARDISP_DB_TYPE=MSSQL
//  CARDISP_DB_TIMEOUT=10000



//------ 2020/03/30 - 17:03 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.16"		"Mar 30, 2020"
// * 상담원별 강제 VCID 적용   subcriber / AGT_CELLNUM 사용 
//   FORCE_AGT_VCID=1   
//   롯데칠성 적용 


//------ 2020/04/08 - 14:07 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.17"		"Apr 08, 2020"
// * 정보동의 프로토콜(73) 추가 - 스마트카라 
// [MW_CONF] 
//  AGREE_TRUNK_NUM1=99103
//  테이블 atp_agree 사용 




//------ 2020/04/09 - 19:48 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.18"		"Apr 09, 2020"
// * 팝업타입 4(14)에 마지막상태코드 처리를 기본으로 사용하도록 



//------ 2020/04/13 - 17:52 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.19"		"Apr 13, 2020"
// * 보류해제 없이 Re-Answer가 오는 경우 처리(평촌성심)
// * 



//------ 2020/04/14 - 17:16 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.20"		"Apr 14, 2020"
// * 비츠 통화시도중에 그룹변경할경우 통계오류 수정 
//    AgentState_IsOneCallTrying 추가 



//------ 2020/05/13 - 15:57 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.21"		"May 13, 2020"
// * 호분배 시도중에 보류되지 않은상태에서 단말기(폴리콤vvx)로 발신하고 
//   아웃바운드콜이 종료된 경우에 상태 처리 - 상계백병원 - 200513



//------ 2020/05/28 - 14:36 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.28"		"May 28, 2020"
// * 종료팝업 타입6번에 마지막상태코드 추가(직방) 


//------ 2020/05/28 - 16:08 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.29"		"May 28, 2020"
// * AgentConPopupResProc 로그작업 


//------ 2020/06/15 - 18:40 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.30"		"June 15, 2020"
// * 종료팝업 6번타입에 마지막상태코드 추가 
// * Disconnect 처리시 종료팝업 구분 처리 
// * 보료중인 콜이 종료되었을때 상태 오류 수정 
// * 통화중 클릭투다이얼 방식변경작업 중 
// * 통화연결후 콜백 인아웃구분 추가 
//   nm_frommailbox='admin'  에서 I / O 으로 변경 - 200615(이대목동)



//------ 2020/06/18 - 13:50 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.31"		"June 18, 2020"
// * 통화중일 경우 보류 후 클릭투다이얼 
//  [MW_CONF] CTD_AUTO_HOLD=1



//------ 2020/06/25 - 16:31 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.32"		"June 25, 2020"
// * 상담원 상태코드 관련함수 정리 


//------ 2020/06/30 - 17:25 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.33"		"June 30, 2020"
// * PBX_DISCONNECT 이후에 TRANS_ATTEND_SUCC 이벤트가  발생한 경우 IDLE 타이머를Kill 하지 않도록 - 200630 (정선군청)


//------ 2020/07/01 - 10:15 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.34"		"July 01, 2020"
// * 통화중일 경우 보류 후 클릭투다이얼 중복 걸기 예외처리 


//------ 2020/07/09 - 13:54 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.34"		"July 09, 2020"
// * 행업코드 정리(직방) 



//------ 2020/07/10 - 13:28 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.35"		"July 10, 2020"
// * (상담)포기호데이터 고객사 연동(브레인헬스) 
//   CUSTLINK_TH=10
//   CUSTLINK_ABANDON=1



//------ 2020/07/13 - 12:07 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.36"		"July 13, 2020"
// * (상담)포기호데이터 고객사 연동(브레인헬스) - 대표번호별로 설정
//  rjt_type=7, rjt_display_tel=대표번호, *(기본) 



//------ 2020/07/14 - 15:17 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.37"		"July 14, 2020"
// * nasa_agentrpt의 customerid필드를 아웃바운드 시도 시간으로 사용(직방) - 200714
//  [MW_CONF] MEMBERID_USE_TRYTIME=1



//------ 2020/08/03 - 17:29 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.38"		"Aug 03, 2020"
// * 상담원 수정시 DB에서 읽어오는 값 패치하도록 


//------ 2020/08/04 - 18:07 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.39"		"Aug 04, 2020"
// * 중복실행 수정 



//------ 2020/09/02 - 17:18 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.40"		"Sep 02, 2020"
// * 상담원명 30Byte만 로딩 처리(UTF8/한글)처리 



//------ 2020/09/09 - 15:06 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.41"		"Sep 09, 2020"
// * Div_Find 변수 타이머로 체크 (이대목동)


//------ 2020/09/18 - 18:02 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.42"		"Sep 18, 2020"
// * Div_Find 변수 타이머로 체크 옵션처리 //  DBG_MASK_DIV_FIND



//------ 2020/10/12 - 17:45 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.43"		"Oct 12, 2020"
// * 예상대기시간디버깅 작업 (DBG_MASK_WIATTIME/ccdbg_info.d_waittime)


//------ 2020/10/29 - 16:28 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.44"		"Oct 29, 2020"
// * KTIS 통화연결시 콜백건 업데이트 처리 



//------ 2020/11/05 - 12:15 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.45"		"Nov 05, 2020"
// * 블라인드 시도 보류번호 처리 수정 
//  (당겨받기후 호전환 경우 보류된 번호가 당겨받기 당한 내선으로 표시되어 호전환 전 저장하여 사용)



//------ 2020/11/25 - 18:18 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.46"		"Nov 25, 2020"
// *  KTIS 통화시 콜백 플래그 처리 
// * 고객대기자 로그 처리 (DBG_MASK_WIATTIME)
// * IVR 세션ID 추가 (포기호삭제관련)
// [MW_CONF]
// DB_SSID=1  



//------ 2020/12/15 - 21:09 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.47"		"Dec 15, 2020"
// * 응답시간관리명 앞4자리(FA00)로 호분배 요청시 호분배 방식 강제 지정 - 한성자동차
// * 요청호(고객포기호) 회수 처리 58번 프로토콜 추가 



//------ 2021/01/05 - 20:13 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.48"		"Jan 05, 2021"
// * 호분배시도중에 로그아웃시 재연결 로직이면 전화를 먼저끊고 미분배처리하도록 수정 
// * 미분배 처리한다음 재로그인해서 전화를 받으면 통계오류 발생해서 - 바로스



//------ 2021/01/07 - 17:06 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.49"		"Jan 07, 2021"
// * 로그인후 이전 통화콜정보 통계 미처리 
// * 로그인후 이전 통화콜정보 상담원에게 확인처리후 삭제 (프로토콜 37번 추가 )


//------ 2021/01/08 - 13:38 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.50"		"Jan 08, 2021"
// * 로그인전 상태를 DB에서 로딩 후 로그인 통계처리를 처리하도록 수정 



//------ 2021/01/12 - 14:19 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.51"		"Jan 12, 2021"
// * 서비스레벨 전체건수 오류 수정 (바로스)
//   - 요청호 삭제, 미분배처리시, 서비스레벨 전체건수도 감소 처리 



//------ 2021/01/22 - 11:12 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.52"		"Jan 22, 2021"
// * DB_EXEC_LAST_QUERY DB 쿼리 실패후 재 실행여부(상계백병원 실행후 실패처리 되었으나 입력되어 중복처리됨) - 210122


//------ 2021/02/02 - 16:14 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.53"		"Feb 02, 2021"
// * 오토피온 30분 이전콜 조회 오류 부분 수정 



//------ 2021/02/17 - 11:15 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.54"		"Feb 17, 2021"
// * 모니터호전환 중복 실행 체크 



//------ 2021/03/03 - 13:36 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.55"		"Mar 03, 2021"

// * AST 16버전 아웃바운드 후 아웃바운드 연결시보류가 나중에 오는경우 처리 
// * PDS속성값(PDS_ATTR_MASK) 처리 추가 - MSI 대부 210302
// * 상담원 수정 요청시 락 걸고 호분배 그룹을 삭제처리 - 한성자동차 - 210303



//------ 2021/03/18 - 18:58 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.56"		"Mar 18, 2021"
// *  MSI ACS 녹취 
//	- ACS 콜 녹취 정보 IVR 전송 프로토콜 추가 (34번)
// * Asterisk 녹취 추가 
//	- ncr_user 관리 
//	- 녹취 페치 이벤트 프로토콜 추가 
//	- MixMonitor LINK와 연동 
//	[ASTREC]
//	ASTREC_USE=1
//	ASTREC_PATH=/usrdata/recmsg
//	ASTREC_POST=/var/lib/asterisk/agi-bin/astrec_end.php
//	ASTREC_SERVER_IP=127.0.0.1
//	ASTREC_SERVER_PORT=5365
//	* 팝업이벤트에 호전환 ID 추가 03, 07번 (그린카)
//	* 소그룹 최대 값 수정 300-> 400개 (비츠)
//  * 팝업 이벤트 녹취 누락 로그 작업 (씨젠)
//  * 서울대강남센터 D7 타입변수 10->50 변경 



//------ 2021/03/26 - 10:43 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.57"		"Mar 26, 2021"
// * ASTREC_STOPMIX=1 옵션추가
// * 



//------ 2021/03/30 - 11:37 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.58"		"Mar 30, 2021"
// * ASTREC_OUTTRY=1 // AST녹취 아웃바운드 시도 부터 녹취 - 210330



//------ 2021/04/06 - 10:41 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.59"		"Apr 06, 2021"
// * DB try catch 처리 



//------ 2021/04/06 - 13:32 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.60"		"Apr 06, 2021"
// * 평촌성심 TimerWaitGetMsg free 할때 죽는 경우 발생 - 210406


//------ 2021/04/15 - 20:20 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.61"		"Apr 15, 2021"
// * changeTime 시간을 제대로 못구해 호분배되지 않은 오류 수정 (씨젠)


//------ 2021/04/16 - 15:37 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.62"		"Apr 15, 2021"
// * WebBaseCodeChange의  스레드 DB로 변경 
// * SetTimer Mil시간 비교 처리 


//------ 2021/04/22 - 19:11 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.63"		"Apr 22, 2021"-------------- 심각한 버그 (고객대기가 많은경우)
// * 고객대기자 연결 구하는 부분 오류 수정 (WaitList_GetData) -양산부산대



//------ 2021/04/23 - 17:31 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.64"		"Apr 23, 2021"
// * AST 아웃시도 녹취시 지연 처리 (직방) 
// [ASTREC] ASTREC_OUTDELAY=2



//------ 2021/05/14 - 10:26 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.65"		"May 14, 2021"
// * // 대기자수 전송 그룹 지정( 0:호분배그룹, 1:전체그룹) 옵션추가 - 성빈센트
//   AGT_GROUP_WAITTYPE=1


//------ 2021/05/21 - 17:20 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.66"		"May 21, 2021"
// * call_log, agentrpt 인입날짜를 통계날짜료 변경처리(자정지났을때통계처리) - 그린카 


//------ 2021/05/27 - 14:40 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.67"		"May 27, 2021"
// * Ast16 버전에서 호분배 돌려주기 정보 처리 



//------ 2021/05/28 - 15:30 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.68"		"May 28, 2021"
// * 호분배재연결로직의 호분배 시도중에 
//   호분배 실패시 IVR에서 IvrRecv_ReDivDisconnect 처리가 지연발생되었을때 처리  - 210528



//------ 2021/05/31 - 16:58 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.69"		"May 31, 2021"
// * 폴리콤 호분배 링중에 직통링이 울리고 수화기로 받은 경우- 210531
//   수화기로 받으면 마지막링을 연결시킴.



//------ 2021/06/08 - 19:19 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.70"		"June 08, 2021"
// * IVR 3자통화 프로토콜 추가 (KMI)
// * IVR_CONFER_NUM
// * IVR_CONFER_DB_USE



//------ 2021/06/15 - 14:35 --- e:\_Svn\Project\ncmw-onpbx\include       ""3.9.70"		"June 08, 2021"
// * 캠페인 시스템 스케줄 사용 여부(농산물품질관리원/이음) - 210615
//   휴일, 업무시간, 점심시간 시스템에 대한 스케줄 설정하여 캠페인 처리 (restde_manage / schdul_manage 테이블 생성)
// 	[PDS_CONF]  PDS_SYS_SCHEDULE=1


// * 차수별 결과 DB 처리(시도횟수 3회까지 DB 로그 처리)
//	PDS_USER (pu_status1, pu_error1, pu_date1, pu_time2, pu_status1, pu_error2, pu_date2, pu_time2, pu_status3, pu_error3, pu_date3, pu_time3) 사용필드 생성필요 
//	[PDS_CONF] PDS_COUNT_RET_DB=1


// * ACS 완료후 통계 저장 DB 저장 (이음) 
//	PDS_SET (cc_total, cc_out, cc_out_all,cc_out1, cc_out2,cc_out3,  cc_answer, cc_answer1, cc_answer2, cc_answer3, cc_callback) 사용 필드 생성 필요 
//	[PDS_CONF] ACS_END_RET_DB=1



//------ 2021/07/08 - 20:22 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.71"		"July 08, 2021"
// * 블라인드시 보류 하지 않고 처리 옵션 추가 (유니메오-단말기에서 호종료되지않아수정)
//  [MW_CONF] UNATTEND2NOHOLD



//------ 2021/07/08 - 20:22 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.72"		"July 13, 2021"
// 통화중 상태에서 후처리 상태변경시 기본값으로 적용 (비트컴퓨터)
// (비트컴퓨터, 통화중 상태에서변경시 통화종료후 상태는 변경되나 기본 상태를 변경하지 않아 다음호에는 기본호 상태로 변경됨 ) 
// [MW_CONF] CALLING_STATE_CHANGE=1



//------ 2021/07/16 - 9:59 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.73"		"July 16, 2021"
// * 오토피온 자정 30분 이전 체크 쿼리 변경 concat 함수 제거



//------ 2021/07/19 - 13:35 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.74"		"July 19, 2021"
// * 통화 종료(disconnect)와 다음호 IN 시도가 동시에 이루어지면 종료된 콜처리 하지 못하고 Wait콜로 처리로 처리 되는 오류 수정(next_status, AGT_IDLE 비교처리) 210719(비트컴퓨터)



//------ 2021/07/20 - 10:06 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.74"		"July 19, 2021"
// * AST16에서 1차 상담원연결 후 고객 연결된 녹취 파일이 1개로 생성되어 
//   1차 상담원 연결 녹취파일명을 유지하도록 변경 - 210719 (그린카WebRTC)
// * 팝업 처리 로그 추가 - 210719



//------ 2021/08/04 - 19:00 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.75"		"Aug 04, 2021"
// * 로그인후 상태 변경시 이미 상태가 변경된 경우 무시하도록 처리 - 210804 그린카 


//------ 2021/08/09 - 14:25 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.76"		""Aug 10, 2021"
// * 
// * 자정통계 추가(인바운드/아웃바운드, 부재중 상태)



//------ 2021/08/18 - 15:46 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.77"		"Aug 18, 2021"
// * [MW_CONF] WAITCALL_ONHOOK=1 전화끊기시 대기콜 먼저 끊도록 설정 - 210818 더블유 병원


//------ 2021/08/23 - 16:38 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.78"		"Aug 23, 2021""
// * AGT_DIV_TIMER 타이머에서 호분배 실패상태일때 처리하지 않도록 - 210823 상계백병원



//------ 2021/08/31 - 14:47 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.79"		"Aug 31, 2021"
// * 통화중 대기콜을 받을때 이전호가 종료되고 IDLE타이머(InTry처리) 처리되기 전에 
//   전화를 받았을때 InTry 상태 없이 InCon만 존재하는 경우 -210831 (바로스)
//   전화를 받았을때 InTry 먼저 처리하고 InCon 처리 하도록 수정 



//------ 2021/09/02 - 14:49 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.80"		"Sep 02, 2021"
// * 호분배 처리 로직 수정 
//  - 통화중료후 IDLE타이머에 "전화대기" 상태로 변경 후 고객대기자 조회할 시점에 
//    요청호가 존재하면 동시에 중복으로 호분배 처리하는 버그 존재 
//    "전화대기" 상태 변경전에 pAgent->Div_Find 를 플래그값 변경하여 요청호에 대해서 처리 하지 못하도록 수정 
//    SetDivFindCheckLock 함수추가 



//------ 2021/09/08 - 17:47 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.81"		"Sep 08, 2021"
// * IVR (DTM수집 /결제) 3자통화 요청시 타입 추가 
//   - 타입 (S:시작, E:중지) - 210908 추가(서울대강남센터) 
// * 결제3자통화 가입자2 Disconnect 프로토콜 추가 L222
//    PbxPayConferenceDisconnect


//------ 2021/09/14 - 11:48 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.82"		"Sep 14, 2021"
// * SQLObjectPush / AgentWriteReportProc 사용 위치로그 처리 



//------ 2021/09/29 - 10:39 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.83"		"Sep 29, 2021"
// * "캠페인대기" 상태 코드 추가 
// - [PDS_CONF] PDS_STATE_IDLE_USE=1
// - 캠페인대기 코드 사용시 고객대기자에서 캠페인과 호분배 구분하여 처리 
// - 캠페인 대기 인원 산정시 캠페인대기와 전화대기 구분 처리 



//------ 2021/10/18 - 17:45 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.84"		"Oct 18, 2021"
// *  // PDS_USER 의 VCID 사용 여부 (PU_IVR_NUM)  - 211018(위버스마인드)
// [PDS_CONF]  PDS_USER_VCID=1

// * 캠페인 시도시 PBX_BUSY_BLOCK 이면, 캠페인 실패처리 - 211020
// * 예측 pds_user 상태 업데이트 추가 처리 ListPgrsv_SetAgentCallCount(PDS_USER_ST &acsUserData, int nState, int nErrorCode = 0); //211020 추가
// * 예측 캠페인 pds_user  상태 업데이트 이후 IVR 이벤트 발생시 처리 하지 못하도록 수정 SetDB_UserStat_IVR_Push



//------ 2021/10/25 - 19:57 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.85"		"Oct 25, 2021"
// * 호분배 시도시 아웃바운드하여 PBX_BUSY_BLOCK 이벤트만 발생되는 경우 
//   호분배 실패 처리 - 211025



//------ 2021/11/04 - 16:23 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.86"		"Nov 04, 2021"
// * 표기호 문자연동(CUSTLINK_ABANDON) 관련하여 IVR과 동일하게 CharacterSet() 처리 - 더블유병원 오류 - 211104


//------ 2021/11/30 - 9:49 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.86"		"Nov 30, 2021"
// * 위버스마인드 팝업 캠페인타입 추가 - 211028


//------ 2021/12/21 - 14:31 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.87"		"Dec 21, 2021"
// * AST 녹취 - 보류시 녹취 중지 처리 - 211220(바로스)
//  [ASTREC] 
//  ASTREC_HOLDSTOP=1
//  ASTREC_LOG=1   


//------ 2021/12/28 - 13:31 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.88"		"Dec 28, 2021"
// * stc_calllist에  IVR (stc_port)포트 추가  - 동해콜- 211228


//------ 2021/12/29 - 14:01 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.89"		"Dec 29, 2021"
// * 통화시도중 상태에서 상태변경시 상태 변경 처리 - 211229 (화성도시공사 - 후처리 유지하게 위해)
//  CALLTRY_STATE_CHANGE=1


//------ 2022/01/11 - 14:45 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.90"		"Jan 11, 2022"
// *  CTI Admin 소켓이 무한 Accept과 Close 될때 소켓 핸들이 있을때만 처리 - 220111 (위버스마인드)


//------ 2022/01/12 - 14:07 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.91"		"Jan 13, 2022"
// * 웹 소켓 처리 타이머 분리webTm, 오토피온 배차 타이머 분리 apTm 
// * 포기호 연동시 포기호타입 추가(cld_abandon) - 연세사랑


//------ 2022/01/24 - 15:01 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.92"		"Jan 24, 2022"
// * CID로 상담원별 호분배 거부 처리 - 220121 화성교통약자
//   설정 [MW_CONF] BLOCK_CUSTOMER=1
//   테이블 생성 nasa_block_customer 



//------ 2022/02/07 - 15:29 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.93"		"Feb 07, 2022"
// * member_ascii_id 30에서 100으로 변경 - 위버스 





//------ 2022/02/15 - 14:38 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.94"		"Feb 15, 2022"
// * 콜백자동업데이트시 nm_divide 업데이트 처리 - 해운대백병원




//------ 2022/03/18 - 10:38 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.95"		"Mar 18, 2022"
// * 로그인 되어 있는 상태에서 강제로그인 시도시에 
//   현재 상태가 호분배 시도 중이고, 재호분배연결로직 사용중이면, 
//	 미분배 처리하고,  시도중인 전화를 강제로 끊고, 다른 상담원으로 연결 하도록 처리 



//------ 2022/04/26 - 17:49 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.96"		"Apr 26, 2022"
// * AST녹취 인바운드 연결 후 녹취 지연처리 - 220426
//  [ASTREC] ASTREC_INDELAY=500


//------ 2022/05/02 - 18:12 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.97"		"May 02, 2022"
// * 호분배 ANSWER_FAIL AST16 옵션 처리 
//  [MW_CONF] DIV_ANSWER_FAIL_DISABLE=1     


//------ 2022/05/11 - 19:50 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.98"		"May 11, 2022"
// * 캠페인후 상태 처리 - 220511 한성자동차


//------ 2022/05/16 - 14:10 --- e:\_Svn\Project\ncmw-onpbx\include       "3.9.99"		"May 16, 2022"
// *  상담원 DB Index로 (타이머인덱스)사용시 DB 인덱스 값이 1000 이상인 경우 장애 발생되어, 
//    리스트 입력시 인덱스 값을 구하여 사용하도록 변경 - 220516


//------ 2022/05/18 - 17:49 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.01"		"May 18, 2022"
// * AgentAct_IdlePush  로그인 상태에만 처리 - 220518 (동원홈푸드)


//------ 2022/05/19 - 11:44 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.02"		"May 19, 2022"
// * 상담원 Timer 인덱스(ver:3.9.99) 오류 부분 수정 - 220519


//------ 2022/05/24 - 15:57 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.03"		"May 24, 2022"
// * 픽업이 처리된 후에 픽업당한단말에서 전화를 받았을때 "연결" 후 "끊기" 이벤트 처리 되는 경우 처리 (온라인팜) - 220524
// - 픽업 이벤트에서 픽업 당한 콜 이력 삭제 처리 
// - 설정 [MW_CONF] CONNECTEDCALL_PICKUP=1


//------ 2022/05/31 - 16:56 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.04"		"May 31, 2022"
// * 팝업 전송 로그 인덱스 정리 
// * 녹취 url_conv 변경 요청 필드 변경 nuc_attr2
// * IVR 3자통화 방식 녹취 청취 기능 추가 (식품안전정보원) - 220530
// - 타입 R 추가, 33번 프로토콜 수정 
// * 녹취 청취 전용 대표번호 설정 추가 
// - [MW_CONF] IVR_REC_LISTEN_NUM=99106
// * 호분배시도 중 아웃바운드 시도시 체크타이머 중지처리 - 220531
// -호분배 콜은 Busy 이벤트 없고, 중복 호분배 실패 통계 처리 됨 
// * IVR 3자통화 방식 녹취 청취 기능 중지 추가 (타입:T)



//------ 2022/06/02 - 14:43 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.05"		"June 02, 2022"
// * 위버스 미납추심 예측캠페인후 호분배 처리를 캠페인 처리로 변경처리  
//  IVR User 삭제 부분 옵션 처리 [MW_CONF] WB_PDS_IVR_OPT=0 



//------ 2022/06/13 - 17:24 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.06"		"June 13, 2022"
//------ 2022/06/13 - 17:47 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.07"		"June 13, 2022"
// * 설정로딩 부분 정리 



//------ 2022/06/22 - 15:32 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.08"		"June 22, 2022"
// * 호전환 성공 이벤트에 아웃바운드번호가 없는 경우 호정보로 처리 - 나은병원 - 220622



//------ 2022/07/19 - 11:39 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.09"		"July 19, 2022"
// * 통화연결시 콜백 업데이트 처리시 user_id='admin' 조건 제거
//   강남성심 웹에서 콜백 분배시 user_id 업데이트 처리해서 


//------ 2022/07/21 - 14:38 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.10"		"July 26, 2022"
// * 실수형 캠페인 배수 설정(건강관리협회) - 220721 
//  [PDS] PDS_F_CALLOUT_USE=1
//  pds_set 필드 ps_f_callcout_count 추가 



//------ 2022/07/26 - 20:35 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.11"		"July 26, 2022"
// * Agentrpt 통계 초 필드 추가  - tik
// * Agentrpt 상태 시도 시작 시간 추가 - tik
// * Agentrpt 상태 시도 종료 시간 추가 - tik
// AGTRPT_TIME_SEC=1
// AGTRPT_STIME_TRY=1
// AGTRPT_ETIME_TRY=1


//------ 2022/08/25 - 9:21 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.12"		"Aug 25, 2022"
// * REC_DIVINFO_SET 픽업시 연결 시점에 전달 



//------ 2022/09/21 - 15:38 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.13"		"Sep 21, 2022"
// * KMI 포기호 콜백 연동 처리 - 220920
//  KMI_ABANDON_CB=1



//------ 2022/10/25 - 16:24 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.14"		"Oct 25, 2022"
// * 개인별 부재중 상태 착신전환(교환기버전) 설정(교원투어) 
//   설정 NOTREADY_NOANSWER_AGT_USE=1
//   NASA_SUBSCRIBER / NA_FORWARD_USE, NA_FORWARD_NUM 사용 


//------ 2022/11/03 - 13:30 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.15"		"Nov 03, 2022"
// * 부재중착신전환상태 "인연결시도" 추가 
//   -KRT 405 단말에서 착신전환시 암됨 

// * AST 녹취 아웃(재)연결시 지연처리 - 한성자동차 



//------ 2022/11/14 - 13:53 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.16"		"Nov 14, 2022"
// * bind 오류 발생시 종료시키도록 수정 


//------ 2022/12/28 - 8:49 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.17"		"Dec 28, 2022"
// * PayCall 블라인드 방식 추가(S오일) 


//------ 2023/01/11 - 17:28 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.18"		"Jan 11, 2023"
// * 호분배 취소 처리  (당진시청적용)
// * [MW_CONF] DIV_CANCEL_TIME=3
// * 호분배 취소 처리 상태 추가 '0690', '0691'



//------ 2023/01/17 - 14:57 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.19"		"Jan 17, 2023"
// * AST녹취 RXTX 미설정 여부 옵션처리 
//   [ASTREC] ASTREC_RXTX_DISABLE=1


//------ 2023/01/30 - 18:50 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.20"		"Jan 30, 2023"
// * 포기호 취소처리 후  요청호 삭제 처리 - 당진시청 
//   [MW_CONF] DIV_CANCEL_TYPE=2



//------ 2023/02/21 - 20:25 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.21"		"Feb 21, 2023"
// * agentrpt 테이블에 내선 아웃바운드콜 여부 처리 (EXT_OUT) - S오일
//  [MW_CONF] RPT_EXTOUT_LEN=4


//------ 2023/03/13 - 10:55 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.22"		"Mar 13, 2023"
// * 호분배 취소처리 예외사항(실제 링이 울리지 않은경우 포함) 처리 추가 - 당진시청 


//------ 2023/03/13 - 16:38 --- e:\_Svn\Project\ncmw-onpbx\include       ""3.10.22"		"Mar 13, 2023"
// * 미분배 실패시 호분배 실패처리 되는 오류(AGT_DIV_TIMER) 수정 



//------ 2023/04/12 - 14:58 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.24"		"Apr 12, 2023"
// * 보류후 아웃바운드호가 종료되었을때, 보류 복귀이벤트 없이 재연결 이벤트 발생시 처리 (명지성모병원)
//   "보류후아웃바운드재연결" 에서 "아웃바운드재연결"


//------ 2023/06/22 - 20:10 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.25"		"June 22, 2023"
// * 호전환시 AST 녹취 세션 다시 시작하도록 (호전환이후 녹취 없음) - 에스오일
// * ASTREC_TRANSSTOP=1

//------ 2023/06/30 - 15:25 --- e:\_Svn\synap\ncmw-onpbx\include       "3.10.26"		"June 30, 2023"   <-------------  중요패치 버전  
// * 타이머 (mwTm, webTm)에서 DB 커넥션 1개로 사용하여 충돌 발생 DB 케넥션(m_pTimerDB, m_pTimerWebDB) 분리 처리 



//------ 2023/07/10 - 20:51 --- e:\_Svn\Project\ncmw-onpbx\include       ""3.10.27"		"July 10, 2023"
// * 호분배 그룹이 설정된 상담원만 통계 산출 (위버스마인드)


//------ 2023/07/11 - 13:14 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.28"		"July 11, 2023"
// * CTD 상태 체크 
// * [MW_CONF] CTD_IDLE_TIME=1


//------ 2023/07/13 - 13:30 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.29"		"July 13, 2023"
// * 패치시 상담원 통계부분 스레드로 분리처리 - 위버스마인드 
// * [MW_CONF] DB_LOAD_TH=3   (기본스레드수:3, 최소스레드수:1)



//------ 2023/08/21 - 12:54 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.30"		"Aug 21, 2023"
// * GetACSUserPorc 오류 수정 
// * 

//------ 2023/09/06 - 15:48 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.31"		"Sep 06, 2023"  JY
// * IVR <-> MW 상담원 상태 체크 추가
//   IVR_CMD_AGTSTATUS 81번 프로토콜 



//------ 2023/09/19 - 16:33 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.32"		"Sep 19, 2023"
// * AgentPopupList_Add 인덱스 오류 수정 


//------ 2023/09/21 - 14:22 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.33"		"Sep 21, 2023"
// * 단말 통화중 인입콜 정보 상담원 전달 (온라인평생교육원)
//  - [MW_CONF] AGT_BUSYBLOCK_EVENT=1
//  - 테이블 NASA_BUSY_BLOCK


//------ 2023/10/17 - 14:13 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.34"		"Oct 21, 2023"
// * STT 변환 프로토콜 추가 



//------ 2023/10/31 - 15:11 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.35"		"Oct 31, 2023"
// * LINK 상담원 ID 요청및 응답 프로토콜 추가 (PbxReqAgentID) - L401




//------ 2023/11/14 - 18:58 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.36"		"Nov 31, 2023"
// * 보류 이벤트 없이 재연결 이벤트가 발생된 경우 처리 하기 위해서 콜정보 보류 처리  - 231114


//------ 2023/12/06 - 16:14 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.37"		"Dec 06, 2023""
// * 그룹수 400-> 500으로 변경 (비츠)



//------ 2024/01/03 - 10:25 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.38"		"Jan 03, 2024"
// * GetMilliSencondProc() 타이머 오류 수정 


//------ 2024/01/04 - 13:43 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.39"		"Jan 04, 2024"
// * 단말 통화중 인입콜 정보 상담원 전달 (온라인평생교육원)
//  - 전달 타입 추가 - 수신 실패 ( 호분배 및 인바운드 통화 실패 )
//  - 기존 통화중 수신콜 및 부재중 수신콜 구분하여 상담원 전달 및 DB 입력


//------ 2024/03/08 - 14:47 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.40"		"May 08, 2024"		- JY
// * 상담원 실시간 호 카운트 중 보류 후 아웃바운드 건 제외
// * CID2 소켓 추가
/*
CID_SOCK_USE2=
CID_SOCK_LINK2=
CID_MIN_LEN2=
CID_USE_IN2=
CID_USE_OUT2=
CID_USE_PICKUP2=
CID_EXTENSION2=
CID_USE_TRY_IN2=
CID_USE_TRY_OUT2=
CID_USE_TRY_PICKUP2=
CID_USE_CON_IN2=
CID_USE_CON_OUT2=
CID_USE_CON_PICKUP2=
CID_USE_DISCONNECT2=
*/


//------ 2024/03/12 - 16:30 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.41"		"May 12, 2024"		- JK 
// * 첫번째콜 구분하여 팝업 처리 (전기안전공사)


//------ 2024/03/19 - 09:34 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.42"		"May 19, 2024"		- JY
// * HOut 시도/연결 상태에서 보류 중인 콜 통화 종료 시, 상태 유지 (전기안전공사)

//------ 2024/03/20 - 17:51 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.43"		"May 20, 2024"		- JY
// * HOut 시도/연결 상태에서 보류 중인 콜 통화 종료 시 팝업 시점 수정
// * 92, 93, 94번 이벤트 전달 시, 상담원 포기호 데이터 같이 전달

//------ 2024/03/22 - 15:54 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.44"		"May 22, 2024"		- JY
// * CID 소켓 내선 실시간 상태 요청/응답 적용 LC13 - IDLE or BUSY


//------ 2024/04/01 - 15:30 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.45"		"Apr 01, 2024"		- JK 
// * 94번 상담원 포기호 처리 부분 수정 
// * CID 소켓에 이전 콜 구조체 제거 


//------ 2024/04/16 - 16:57 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.46"		"Apr 16, 2024"		- JK
// * 팝업리스트 검색시 nEmptyIDX 오류 수정 
// * 녹취시 STT 처리 옵션 추가 
//   linux uuid 라이브러리 사용 (컴파일옵션 _UUID 추가)


//------ 2024/05/02 - 14:47 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.47"		"May 02, 2024"
// * 캠페인 실시간 요청건수 오류 수정 


//------ 2024/05/27 - 14:25 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.48"		"May 25, 2024"
// * 상담원 대기 변경시 고객 대기자 찾을때, 상담원 그룹의 우선순위로 찾기 설정 여부 - 240527
//   WL_FIND_GROUPRANK_ENABLE = 0



//------ 2024/06/26 - 14:50 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.49"		"June 26, 2024"
// * 웹전체 패치시 Db 커넥션 포인터(m_pSysDB) 충돌 나지 않도록 수정



//------ 2024/06/28 - 11:15 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.50"		"June 28, 2024"
// * Rocky Linux 컴파일 오류 수정 

//------ 2024/07/08 - 18:14 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.51"		"July 08, 2024"		- JY
// * 자정에 영향을 받지않는 날짜 옵션 추가 ( 마약퇴치운동본부 )
// * 자정에 m_list_ivruser 관련 초기화 30분 이상 지난 콜에 대해서만 처리 ( 한림대 평촌 ) 


//------ 2024/07/11 - 18:05 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.51"		"July 11, 2024"
// * 타임존 설정(setenv("TZ", "Asia/Seoul", 1)) 현재시간을 9시간 적게 구하는 오류 (전기안전공사)  


//------ 2024/07/12 - 17:25 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.52"		"July 12, 2024"
// * PDS 수정 


//------ 2024/07/18 - 13:11 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.53"		"July 18, 2024"
// * localtime 반환되는 포인터가 static 이어서 tm 구조체 복사해서 쓰도록 변경(localtime_r) 
// * VerifyUtcCurrentTime 현재시간과 UTC 시간 검증 처리 

//------ 2024/09/23 - 17:34 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.54"		"Sep 23, 2024"		- JY
// * 마약 퇴치 운동본부 자정 통계초기화 사용 시간 추가 
// * nasa_agentrpt add column agt_orig_u_time integer



//------ 2024/10/18 - 16:18 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.54"		"Oct 18, 2024"
// *  IVR 호전환 중 단말이 착신되었을때 이벤트처리, LINK 이벤트가 없어서 추가 (안산도시공사) - 241018
//    이벤트 발생시 시도, 연결 팝업 이벤트 발생 
//    AMI 이벤트에 단말에 대한 이벤트가 발생되지 않음. 


//------ 2024/10/21 - 15:30 --- e:\_Svn\Project\ncmw-onpbx\include       "3.10.55"		"Oct 21, 2024"
// *  녹취 파일명 생성 시, 사용할 수 없는 특수문자 제외 처리


//------ 2024/12/19 - 9:10 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.56"		"Dec 19, 2024"
// * IVR 녹취 기능 추가 
// 
// [ASTREC]
// ASTIVRREC_USE=1
// ASTIVRREC_POST=/var/lib/asterisk/agi-bin/astivrrec_end.php
// 
//# 트렁크별 자동 녹취 설정 
//  DB 설정 nasa_trunk_set.trk_info1 ('A' or 'S')
// 전수 (A) - 인/아웃 바운드 자동 녹취 설정이며, 호전환콜은 제외, 미들웨어에서 처리 
// 선택 (S) - IVR에서 녹취 요청 방식으로 처리되며, 콜타입별, 또는 오픈 스크립트로 IVR에서 처리 
// 
// onpbxlink 설정 [SYSTEM_INFO] IVR_MIXMON_USE=1



//------ 2025/01/20 - 16:42 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.57"		"Jan 20, 2025"
// * IVR_CMD_RES_IDLEAGT(47) 응답에 대한 결과값 프로토콜 추가 
//   IVR(41) -> MW(47) -> IVR(47) -> MW
//   MW(47) 호분배 내선 전송과, IVR(48) 호분배 요청 중지 전송이 동시에 이루어져, 
//   고객 포기호, 상담원 포기호 2개가 발생되는 통계 오류 처리 


//------ 2025/01/20 - 16:42 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.58"		"Jan 23, 2025"		- JY
// * BusyBlock 중 상태 변경 시, 포기호 중복 발생 건 업데이트

//------ 2025/04/11 - 10:54 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.59"		"Apr 11, 2025"		- JY
// * 상담원 상태 변경 요청 시, 상담원이 변경할 수 없는 상태는 무시하도록 수정



//------ 2025/06/04 - 17:00 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.60"		"June 03, 2025"
// *   픽업시 구Onpbxlink로 전화번호를 링크내선으로 변경처리 - 250604 
// *  [MW_CONF] REQ_LINKNUMBER_CHANGE=1


//------ 2025/06/05 - 16:25 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.61"		"June 05, 2025"
// * 32Bit에서 타이머 오류 수정 



//------ 2025/06/17 - 15:02 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.62"		"June 17, 2025"
// * 호분배 취소(고객포기호전환) 설정시 외부 포기호 전송 로직 추가 


//------ 2025/07/08 - 16:18 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.63"		"July 08, 2025"  -- 사용안함(처리안됨)
// * 돌려주기 이후 녹취가 종료 되지 않았을때 녹취 종료 요청 
//   [ASTREC] ASTREC_VERIFY_STOP_TIME=500  / 0 이면 미사용  


//------ 2025/07/09 - 10:07 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.64"		"July 09, 2025"
// *  AST녹취 보류 후 아웃바운드 시도 부터 녹취 옵션처리 
//  [ASTREC] ASTREC_HOLDOUTTRY=1 


//------ 2025/07/17 - 10:53 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.65"		"July 17, 2025"		- JY
// * IVR PDS 결과 DB 업데이트 관련 채널에 종속하여 SQLThread 사용하도록 변경



//------ 2025/07/23 - 9:31 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.66"		"July 21, 2025"
// * 녹취 ncr_user의 STT_USE 값으로 STT 사용 여부 처리 
//   [ASTREC] ASTSTT_USER=1  

//------ 2025/08/19 - 15:49 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.67"		"Aug 19, 2025"
// * DB 접속 정보에 도메인도 입력할 수 있도록 변경

//------ 2025/08/29 - 17:01 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.68"		"Aug 29, 2025"
// * 포기호 연동 관련 고객 번호 누락 수정


//------ 2025/09/02 - 18:22 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.69"		"Sep 02, 2025"
// * 교환기 BUSY_BLOCK 이벤트가 발생되지 않을때 호분배 실패 처리오류 수정 (성심당) - 250902
//    [MW_CONF] BUSY_BLOCK_DISABLE=1

//------ 2025/11/12 - 14:14 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.70"		"Nov 12, 2025" - WJ
// * AgentLogonProc에서 로그온 성공 시 보내는 패킷 수정 (로그온 메시지 누락)


//------ 2025/11/17 - 9:57 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.70"		"Nov 17, 2025"
// * 자동캠페인에서 pu_info 팝업 전송 처리 

//------ 2025/12/01 - 20:26 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.71"		"Dec 01, 2025" - JY
// * 포기호 연동 처리 시 nasa_message 테이블 리포팅 처리 - 해운대 백병원

//------ 2025/12/01 - 16:54 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.72"		"Dec 03, 2025"
// * 자동PDS 대기상태일때 데이터가 없어도 상태 유지 처리 (주기적으로 요청하는 방식) 한성자동차- 251201
//  [PDS] PDS_APDS_IDLE_STATE_KEEP=1


//------ 2025/12/05 - 15:27 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.73"		"Dec 05, 2025"
// * 보이스봇캠페인 추가 


//------ 2026/01/20 - 17:05 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.74"		"Jan 20, 2026"
// * nasa_call_log DB입력 시, ivr_callid와 agt_callid 별도 구분  
// * [MW_CONF] USE_IVR_CALLID=1



//------ 2026/02/04 - 14:18 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.75"		"Feb 04, 2026"
// * 소그룹변동이 있을경우 상담원 그룹의 인덱스변경처리
// * GROUP_CALLCNT의 상담원 로그인수, 대기자수 사용하는 경우 52통계 전송처리 (그린카) 
//   [MW_CONF] AGT_STAT52_SEND=1


//------ 2026/02/11 - 9:19 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.75"		"Feb 11, 2026"
// * 이전버전 로그작업
 
//------ 2026/02/20	- 10:10 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.76"		"Feb 20, 2026"
// * MFMobility 연동 처리
//   [MW_CONF] MFMOBILITY_USE=1
//   [MW_CONF] MFMOBILITY_URL=

//------ 2026/02/20	- 14:30 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.77"		"Feb 20, 2026"
// * Rocky 9 컴파일 오류 처리


//------ 2026/02/26 - 14:11 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.78"		"Feb 26, 2026"
// * 재연결시 녹취 안되는 버그 수정 


//------ 2026/04/06 - 13:07 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.79"		"Apr 06, 2026"
// * 호분배 시도중 단말 180없거나 지연되는 현상시 아웃바운드로 처리되는 버그를 
//   링크의 수정된 버전으로 적용시 RING, ANSWER 동시 발생되는 경우에 통계 오류 수정 (S오일)


//------ 2026/04/14 - 17:09 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.80"		"Apr 14, 2026"
// * WaitForDbConnect 도메인 host IP 구하는 부분 오류 수정 


//------ 2026/04/20 - 17:54 --- E:\_Svn\Project\ncmw-onpbx\include       "3.10.81"		"Apr 20, 2026"
// * 보이스봇 PDS - posgres DB 변경(DSN 사용안하고 동적처리) 
//   PDS_EXTDB_DSNLESS=1
// * 분산서버 Host 연동 처리
//  IP변경 감지시 재 접속 처리 
//  PDS_HB_API_URL= 
// 	PDS_HB_API_KEY=
// 	PDS_HB_MODEL_TYPE=postgre
// 	PDS_HB_POLL_INTERVAL=30




#define NCMW_VERSION		"3.10.81"
#define NCMW_LAST_UPDATE	"Apr 20, 2026"





/*-------------위치 인덱스 
	- AgentConPopupReqProc	 51 부터 사용 가능 
	- AgentStateSendProc	853 부터 사용 가능 
	  AGT_SendPush 
	- SQLObjectPush			188 부터 
	- AgentWriteReportProc  650 부터 
*/










// 처리해야할것 
/*



* GetMilliSencondProc 구하는 방식 수정 필요 - 221228



#define MAX_GROUP_COUNT			300		
#define MAX_GROUP_CODE			300
그룹갯수 초과시 프로세서 종료됨 



* CTI Admin 콜백 처리 - 관리자쪽은 콜백 처리가 안되어 있음 - 210503


* 자정 부재중 상태 자정기점으로 분리 처리 - 그린카 - 210616

















*/




#endif//shareheader.h

