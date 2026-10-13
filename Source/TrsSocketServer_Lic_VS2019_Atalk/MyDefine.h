#ifndef __MYDEFINE_H__
#define __MYDEFINE_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif

#define SOCKET_LOG			1
#define DB_LOG				2
#define CONTROL_LOG			3


//소켓 타입
#define	SOCKTYPE_NONE		0				// 설정 되지 안음
#define	SOCKTYPE_WEB		1				// 웹 소켓
#define	SOCKTYPE_SYS		2				// 시스템 소켓
#define	SOCKTYPE_AGT		3				// 상담원 소켓
#define	SOCKTYPE_PBX		4				// 교환기 소켓


//호분배 모드
#define	CALL_DIV_CIRCLE		0				// 순차방식
#define	CALL_DIV_TIME		1				// 대기시간 방식
#define	CALL_DIV_GROUP_TIME	2				// 그룹단위 대기시간 방식


#define APP_LOCAL_PATH		".\\NcMiddle.ini"

#define PAGE_COUNT			9		//한 화면당 출력할 갯수

//AGENT client Command
#define AGENT_CMD_LOGON				0
#define AGENT_CMD_LOGOUT			1
#define AGENT_CMD_PASS				2
#define AGENT_CMD_REQUEST_LINK		3
#define AGENT_CMD_RESPONSE_LINK		4
#define AGENT_CMD_FORCE_LOGON		5
#define AGENT_CMD_PHONE_OFFHOOK		11
#define AGENT_CMD_PHONE_ONHOOK		12
#define AGENT_CMD_PHONE_MAKECALL	13
#define AGENT_CMD_PHONE_PICKUP		14
#define SYS_CMD_PHONE_OFFHOOK		31
#define SYS_CMD_PHONE_ONHOOK		32
#define SYS_CMD_PHONE_MAKECALL		33
#define LINK_MESSAGE_REQUEST		41
#define LINK_CALLID_SET				42
#define LINK_MESSAGE_CALLCENTER		43			//	CTI 콜수
#define LINK_REMOVE_CON_COUNT		44
#define LINK_TRUNK_ANSWER			45
#define LINK_AGT_MAKECALL			46
#define LINK_RESPONSE_IDLEAGT		47
#define LINK_REMOVE_DIS_COUNT		48			// 호분배 실패
#define CTI_CALLCNT					51
#define GROUP_CALLCNT				52
#define CUSTOM_WAIT_CNT				53
#define CUSTOM_WAIT_NOTCNT			54
#define WEB_CMD_PROG_RESET			73
#define SYS_CMD_LINE_STATE			75
#define AGENT_CMD_MNG_MESSAGE		76
#define AGENT_CMD_MNG_REPORT		77
#define WEB_CMD_BASECODE			78
#define WEB_CMD_MODIFY				79
#define AGENT_MODIFY_BROADCAST		80
#define AGENT_CMD_OUT_MSG			84
#define AGENT_CMD_MODE				85
#define AGENT_CMD_POPUP				86
#define WEB_CMD_ALL_STATE			88
#define AGENT_CMD_ALL_STATE_RES		89
#define AGENT_CMD_STATE				92


//CALLCENTER CALLCOUNT	콜수 DEFINE
#define		CTI_CALLCOUNT_NONE			0			// IVR 연결콜
#define		CTI_CALLCOUNT_IVR			1			// IVR 연결콜
#define		CTI_CALLCOUNT_MEM_CTI		2			// 회원 콜센터 연결콜
#define		CTI_CALLCOUNT_NO_CTI		3			// 비회원 콜센터 연결콜
#define		CTI_CALLCOUNT_AGT_TRY		4			// 상담원 연결콜
#define		CTI_CALLCOUNT_AGT_CON		5			// 단순 조회 콜
#define		CTI_CALLCOUNT_CALLBACK		6			// Callback 콜


#define AGENT_LOGON_SUCC				1			//로그인 성공
#define AGENT_LOGON_USER				2			//아이디가 없을때
#define AGENT_LOGON_PASS				3			//비밀번호가 틀릴때
#define AGENT_LOGON_FAIL				4			//아이디가 중복 로그인 일때
#define AGENT_LOGON_EXT					5			//내선 번호가 중복 로그인 일때
#define AGENT_LOGON_REG					6			//등록이 안된 내선 일때
#define AGENT_LOGON_GROUP				7			//상담원 기본 그룹이 없을때 오류

#define AGENT_PASSCHANGE_SUCC			1			//비밀번호 변경 성공
#define AGENT_PASSCHANGE_USER			2			//사용자 아이디 오류
#define AGENT_PASSCHANGE_PASS			3			//비밀번호 오류

#define AGENT_MODIFY_ADD				1			//비밀번호 변경 성공
#define AGENT_MODIFY_MODIFY				2			//사용자 아이디 오류
#define AGENT_MODIFY_DELETE				3			//비밀번호 오류

#define AGENT_MODIFY_RETURN_SUCC		1			//비밀번호 변경 성공
#define AGENT_MODIFY_RETURN_USER		2			//사용자 아이디 오류
#define AGENT_MODIFY_RETURN_LOGIN		3			//이미 로그인한 아이디



//		상담원 상태
#define	AGT_GO_IN		 		"0200"// 출근
#define	AGT_GO_OUT		 		"0201"// 퇴근
#define	AGT_UNKNOWN		 		"0202"// 알수 없는 상태
#define	AGT_IDLE		 		"0300"// 대기
#define	AGT_IN_TRY		 		"0310"// IN BOUND 시도중
#define	AGT_IN_CONNECT	 		"0311"// IN BOUND 통화중
#define	AGT_IN_FAIL		 		"0312"// IN BOUND 실패
#define	AGT_DIV_TRY		 		"0320"// 호분배 시도중
#define	AGT_DIV_CONNECT	 		"0321"// 호분배 통화중
#define	AGT_DIV_FAIL	 		"0322"// 호분배 실패
#define	AGT_OUT_TRY		 		"0330"// OUT BOUND 시도중
#define	AGT_OUT_CONNECT	 		"0331"// OUT BOUND 통화중
#define	AGT_OUT_FAIL	 		"0332"// OUT BOUND 실패
#define	AGT_AUTOCALL_TRY 		"0333"// AUTOCALL 시도중
#define	AGT_HOLD_TRY	 		"0340"// 보류 하는 중
#define	AGT_HOLDING		 		"0341"// 보류된 중
#define	AGT_UNATTEND_TRY 		"0342"// 호전환 Callout
#define	AGT_UNATTEND_FAIL 		"0343"// 호전환 Callout 실패
#define	AGT_HOLD_REJECT			"0344"// 보류 복귀
#define	AGT_TRS_SUCCESS			"0345"// 호전환 성공
#define	AGT_CONFER_TRY	 		"0350"// 삼자통화 시도중
#define	AGT_CONFER_CONNECT		"0351"// 삼자통화 통화중
#define	AGT_CONFER_FAIL	 		"0352"// 삼자통화 실패
#define	AGT_TRS_TRY		 		"0361"// 호전환 시도
#define	AGT_TRS_SUCC	 		"0362"// 호전환 성공
#define	AGT_TRS_FAIL	 		"0363"// 호전환 실패
#define	AGT_TRS_CPT_TRY	 		"0364"// 호전환 완료 시도
#define	AGT_TRS_CPT_SUCC 		"0365"// 호전환 완료 성공
#define	AGT_TRS_CPT_FAIL 		"0366"// 호전환 완료 시패
#define	AGT_TRS_RJT_TRY 		"0367"// 호전환 복귀 시도
#define	AGT_TRS_RJT_SUCC 		"0368"// 호전환 복귀 성공
#define	AGT_TRS_RJT_FAIL 		"0369"// 호전환 복귀 실패


//		PBX 상태
#define		PBX_LOG_ON				1			//L001	상담원 로그온
#define		PBX_LOG_OFF				2			//L002	상담원 로그아웃
#define		PBX_REQ_STATE			3			//L003	상담원 상태요구
#define		PBX_RING				4			//L004	링(Offered)
#define		PBX_ANSWER				5			//L005	전화받기(Answered)
#define		PBX_MAKECALL_TRY		6			//L006	전화걸기 시도(Makecall try)
#define		PBX_MAKECALL_FAIL		7			//L007	전화걸기 실패(Makecall Fail)
#define		PBX_MAKECALL_CONNECT	8			//L008	연결성공(Connected)
#define		PBX_DISCONNECT			9			//L009	전화끊기(Disconnect)
#define		PBX_UNATTEND_TRY		10			//L010	호전환 시도 하기(UnAttend Transfer Try)
#define		PBX_UNATTEND_FAIL		11			//L011	호전환 실패 (UnAttend Transfer Fail)
#define		PBX_HOLD_REJECT			12			//L012	보류 복귀(HOLD Reject)
#define		PBX_TRS_CONNECT			13			//L013	호전환 Callout 성공
#define		PBX_TRS_SUCCESS			14			//L014	호전환 성공
#define		PBX_CONFER_SUCCESS		14			//L014	3자 통화
#define		PBX_PICKUP				15			//L015	당겨받기(PickUp)
#define		PBX_FORWARD				16			//L016	착신전환 링(Forwarding Offered)
#define		PBX_HOLD_TRY			17			//L017	보류 시도(HOLD Try)
#define		PBX_LINK_CHECK			31			//S001	Link Check
#define		PBX_PHONE_OFFHOOK		41			//P001	PHONE OFFHOOK
#define		PBX_PHONE_ONHOOK		42			//P002	PHONE ONHOOK
#define		PBX_PHONE_MAKECALL		43			//P003	PHONE MAKECALL
#define		PBX_PHONE_PICKUP		44			//P004	PHONE HOLD
#define		PBX_PHONE_CID_CHANGE	45			//P005	PHONE CID CHANGE
#define		PBX_PHONE_HOLD			46			//P006	PHONE HOLD
#define		PBX_PHONE_REJECT		47			//P007	PHONE REJECT
#define		PBX_PHONE_TRANSFER		48			//P008	PHONE TRANSFER
#define		PBX_PHONE_CONFERENCE	49			//P009	PHONE CONFERENCE


//AGENT LOAD RETURN
#define AGT_RELOAD_SUCC				0
#define AGT_RELOAD_DB				1
#define AGT_RELOAD_LOGON			2



//AGENT COUNT
#define AGT_COUNT_NONE				0
#define AGT_COUNT_IN_CON			1
#define AGT_COUNT_DIV_CON			2
#define AGT_COUNT_OUT_CON			3
#define AGT_COUNT_IN_TRY			4
#define AGT_COUNT_DIV_TRY			5
#define AGT_COUNT_OUT_TRY			6


//SYSTEM Command
#define SOCK_SEND	1
#define SOCK_RECV	2

//SYSTEM Command
#define MAX_SQL_QUE				10000
#define MAX_LOG_QUE				10000


#define MAX_GROUP_CNT			50				//AGENT GROUP MAX COUNT
#define MAX_AGT_CNT				1000			//AGENT MAX COUNT


typedef struct{
	char	szAllLength[4];
	int		nAllLength;
	char	szDataLength[4];
	int		nDataLength;
	char	szSeq[5];
	char	szCallID[33];
	char	szReturn[2];
	char	szCmd[5];
	int		nCmd;
	char	szTime[15];
	char	szJangbi[3];
	char	szData1[101];
	char	szData2[101];
	char	szData3[101];
	char	szData4[101];
	char	szData5[101];
	char	szData6[101];
} PBX;


typedef struct{
	char	groupCode1[6];
	char	groupName1[51];
	char	groupCode2[6];
	char	groupName2[51];
	char	groupCode3[6];
	char	groupName3[51];
	char	szTelnum[21];
	int		NG_GROUP_RANK;
} GROUP_CODE;


typedef struct{
	char		CallID[101];
	GROUP_CODE	g_info;
} CALL_INFO;


typedef struct{
	char	AGT_CallID[101];
	char	PEOPLE_NO[15];
	char	CUST_ID[11];
	char	Member_YN[2];
	char	Member_level[2];
	char	CALLER_ID[21];
	char	NG_CODE1[21];
	char	NG_CODE2[21];
	char	NG_CODE3[21];
	char	TRUNK_NUM[21];
	int		Claim_Cnt;
} POPUP_DATA;


typedef struct{
	int	in_year;
	int	in_month;
	int	in_day;
	int	in_hour;
	int	in_minute;
	int	in_second;
	int	start_hour;
	int	start_minute;
	int	start_second;
	int	in_week;
	DWORD cnt_in;
	DWORD cnt_out;
	DWORD cnt_div;
	DWORD cnt_try_in;
	DWORD cnt_try_out;
	DWORD cnt_try_div;
	DWORD useSt;
	DWORD LogonSt;
} AGENT_USAGE;


typedef struct{
	int	in_mem_sumcall;			// 회원 콜센터 입입콜
	int	in_no_sumcall;			// 비회원 콜센터 입입콜
	int	in_div_cnt;				// 호분배콜
	int	in_con_cnt;				// 상담원 연결콜
	int	in_msg_cnt;				// 메세지콜
	int	in_cti_real_cnt;		// 실인입콜
	int	in_cct_real_cnt;		// 실응답콜
} GROUP_USAGE;


typedef struct{
	DWORD64		dwAgtIndex;
	char		szAgtID[21];
	char		nc_code[5];
	char		nc_name[101];
	char		default_status[5];
	char		tmpStatus[5];
	char		rejectStatus[5];
	int			agentOrder;
	char		pswd[11];
	char		ext[21];
	char		changeTime[11];
	char		boxname[31];
	char		jumin[15];
	char		trunktel[31];
	char		callertel[31];
	char		customtel[31];
	char		customID[31];
	char		nextbox[11];
	char		language[3];
	char		mwi[4];
	char		msave[6];
	char		cellnum[21];
	int			rectime;
	int			msgRetention;
	char		userAnncfile[31];
	char		email[29];
	char		AgtSendData[201];
	char		popup_callid[101];
	char		regIP[16];
	short		hangup_code;
	BOOL		bAuto_hangup;
	SHORT		logon;
	BOOL		bCalloutMode;
	BOOL		bHold;
	GROUP_CODE	rpt_group;
	GROUP_CODE	reg_group;
	GROUP_CODE	*div_group;
	AGENT_USAGE	agt_use;
	CALL_INFO	useCallInfo;
	CALL_INFO	holdCallInfo;
} AGENT_INFO;


typedef struct{
	GROUP_CODE	codeName;			//그룹 코드와 이름
	GROUP_USAGE grp_use;			//그룹 콜 건수
	int			agentCnt;			//총 상담원수
} AGENT_GROUP;


typedef struct{
	GROUP_CODE	codeName;			//그룹 코드와 이름
	int			chdev;
	char		szInDate[11];
	char		szIntime[11];
	char		szAni[51];
	char		szDnis[51];
	char		szTrunkTel[51];
	char		szPersonNum[15];
} WAIT_CUSTOM;


typedef struct{
	char	szDnisNum[21];			// IVR 대표번호
	char	szTrunkNum[21];			// IVR 대표번호
	char	szCode[6];				// IVR 코드
	char	szLineData[201];		// 회선 상태값
	int		in_sum_cnt;				// IVR 인입콜
	int		in_mem_call_cnt;		// 회원 콜센터 인입콜
	int		in_no_call_cnt;			// 비회원 콜센터 연결
	int		in_agt_try_cnt;			// 상담원 연결
	int		in_agt_con_cnt;			// 상담원 연결
	int		callback;				// callback
	int		in_cti_real_cnt;		// 실인입콜
	int		in_cct_real_cnt;		// 실응답콜
} IVR_GROUP;


typedef struct{
	UCHAR	ServerIp[31];
	UCHAR	port[4];
	UCHAR	extention[21];
	UCHAR	linetype[11];
	UCHAR	inband[2];
	UCHAR	outband[2];
	UCHAR	callout[2];
	UCHAR	desktop[2];
	char	szStatus[5];
	char	szPrint[201];
} LINE;


typedef struct{
	int		PstnLine;
	int		IpLine; 
	UCHAR	ServerIp[31];
	UCHAR	sitename[41]; 
	SHORT	Defaultmsg[2]; 
	SHORT	Internetadds[21];
	UCHAR	mailid[21];
	SHORT	boxlen;
	UCHAR	pbx[21];
	UCHAR	vms[2];
	UCHAR	longext[6]; 
	SHORT	simsi;
	SHORT	overday;
	SHORT	lang;
	SHORT	logotime; 
	UCHAR	pbxip[26];
	UCHAR	pbxip2[26];
	UCHAR	pbxid[11];
	LINE	*pLine;
	PBX		*m_pbx;
} CNS_SET;


typedef struct {
	CNS_SET			*pSystem;
	int				nStatus;
} CHINFO; 



typedef struct {
	DWORD	idx;
	char	email_site_ip[51];			//	
	char	email_site_real_ip[51];
	char	email_site_name[101];
	int		alarm_type;
	char	alarm_proc_name[51];
	char	email_to[501];
	char	email_from[201];
	char	email_title[201];
	char	email_msg1[501];
	char	email_msg2[501];
	char	email_msg3[501];
	char	email_url[2048];
	char	email_attr1[101];
	char	email_attr2[101];
	char	email_attr3[101];
	char	email_msg[2048];
} EMAIL_SITE_USERS;





typedef struct {
	char sms_site_ip[51];
	char sms_user_name[51];
	char sms_user_phone[21];
	char sms_site_name[51];
	char sms_site_real_ip[51];
	char sms_use[2];
} SMS_SITE_USERS; 

typedef struct {
	int site_sms_index;
	char site_sms_name[51];
	char site_sms_telnum[21];
	char site_sms_use[2];
} SMS_SITE_BASIC_USER; 

typedef struct {
	int cc_index;
	char cc_name[51];
	char cc_ip[51];
	char cc_real_ip[51];
	char cc_site_name[256];
} CC_LINKINFO; 



typedef struct {
	int  sm_index;
	int  sm_type;
	char sm_msg[201];
	char sm_stime[11];
	char sm_etime[11];
	char sm_use[2];
} SMS_EXCLUDE;  // SMS 발송 중지 시간 설정 - 190117








// Locate a driver that supports a given format and return its ID
typedef struct {
	BOOL	bData;
	char	sqlData[2000];
} QUE_SQL;

#define USE_NORAML		0		// 기본 문자발송 유저 
#define USE_ALL			1		// 기본 문자발송 유저와 긴급 문자 유저 

#endif//MyDefine.h


