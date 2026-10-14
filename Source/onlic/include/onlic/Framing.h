/*
**	Framing.h : STX/ETX 프레임 재조립 (Windows TrsSocketServer.cpp ReadMsgProc:500-537, SearchSocketSTX_ETX:617, CutSocketSTX_ETX:644 이식)
**	- 클라이언트 호환 계약 (docs/프로토콜명세.md §2). 바이트 규칙을 바꾸면 안 됨
**	- 이 헤더는 onlicDefine.h 없이 단독 컴파일 가능 (test/framing_test.cpp)
**	- 세션별 잔여 버퍼를 호출측이 보관 (ONLIC_SOCKET::rdata[RESIDUAL_MAX+1]). onmw 의 공유 버퍼 결함 회피
*/
#ifndef ___ONLIC_FRAMING_H___
#define ___ONLIC_FRAMING_H___

#ifndef MAXLENGTH
#define	MAXLENGTH				500		/* Receive(500) 단위 */
#endif
#ifndef RESIDUAL_MAX
#define RESIDUAL_MAX			(MAXLENGTH*2)	/* 잔여 버퍼 1000 */
#endif
#ifndef STR_SPLIT_MAX
#define STR_SPLIT_MAX			50
#endif
#ifndef STR_SPLIT_LEN
#define STR_SPLIT_LEN			1024
#endif
#ifndef PKT_STX
#define PKT_STX					0x02
#define PKT_ETX					0x03
#define PKT_SEP					0xff
#endif

/* 프레임 1개 콜백: nCmd = STX 뒤 2글자 atoi (원본 ret), szPayload = STX/ETX 제외, nLen = payload 길이 */
typedef void (*FRAME_CALLBACK)(void *pCtx, int nCmd, const char *szPayload, int nLen);

/*
**	TCP 수신 1회분 처리 (원본 ReadMsgProc 와 동일 규칙)
**	  szResidual : 세션 잔여 버퍼 [RESIDUAL_MAX+1], NUL 종료. 호출 후 갱신됨
**	  szRdata    : recv() 로 받은 바이트. 원본과 같이 **strlen 기준** (NUL 이후는 무시). 호출측은 memset 0 후 recv(…, MAXLENGTH)
**	  반환: 콜백 호출 횟수(완성 프레임 수)
*/
int		Framing_OnReceive(char *szResidual, const char *szRdata, FRAME_CALLBACK cb, void *pCtx);

/* 원본 SearchSocketSTX_ETX : 첫 STX 를 찾고, ETX 전에 STX 가 또 나오면 startPos 갱신. 못 찾으면 -1 */
void	Framing_SearchSTX_ETX(const char *data, int &startPos, int &endPos);

/* 원본 CutSocketSTX_ETX : 프레임을 잘라 콜백 호출 후 data 를 나머지로 갱신. 반환 = 자른 길이(STX~ETX), 없으면 0 */
int		Framing_CutSTX_ETX(char *data, int startPos, int endPos, FRAME_CALLBACK cb, void *pCtx);

/* UDP 1 datagram (원본 ReadUdpMsgProc): 첫 프레임 payload 만 추출. 반환 payload 길이(빈 프레임 0), 프레임 없으면 -1 */
int		Framing_UdpExtract(const char *szRdata, char *szPayload, int nPayloadSize);

/* 원본 StrSplit : cmp 로 분할. 경계 검사 추가 (원본은 50/1024 초과 시 오버플로) */
int		Framing_StrSplit(const char *src, char dst[STR_SPLIT_MAX][STR_SPLIT_LEN], char cmp);

/* 원본 Hangul_StrCut : 2바이트 문자 경계를 지키며 nMaxByteCount 이내로 자름 */
void	Framing_HangulStrCut(const char *szSrc, char *szDst, int nMaxByteCount);

/* 송신 프레임 조립 (6770 푸시: STX cmd 0xFF msg 0xFF HH:MM:SS ETX / 하트비트 ACK). 반환 길이 */
int		Framing_BuildAlarmPush(char *szOut, int nOutSize, char cCmd, const char *szMsg, const char *szTime);

#endif /* Framing.h */
