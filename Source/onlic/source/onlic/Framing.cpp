/*
**	Framing.cpp : STX/ETX 프레임 재조립. 원본 알고리즘을 한 줄씩 대응시켜 이식했다
**	단독 컴파일 가능 (의존: string.h, stdlib.h, stdio.h)
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Framing.h"

/*
**	원본 ReadMsgProc(:500-537)
**	  len = strlen(m_Rdata) + strlen(Rdata)
**	  if(len < MAXLENGTH*2) temp = m_Rdata + Rdata   else temp = Rdata        ← 잔여 1000 규칙 (초과 시 잔여 폐기)
**	  loop { Search; if(Cut <= 0){ if(strlen(temp) < MAXLENGTH) m_Rdata = temp else m_Rdata = ""; break; } }  ← 잔여 500 규칙
*/
int Framing_OnReceive(char *szResidual, const char *szRdata, FRAME_CALLBACK cb, void *pCtx)
{
	int		startPos = -1, endPos = -1, len = 0, nFrames = 0;
	char	temp[RESIDUAL_MAX+1];
	char	rdata[MAXLENGTH+1];

	if(szResidual == NULL || szRdata == NULL) return 0;

	/* 원본: UCHAR Rdata[MAXLENGTH+1] 에 Receive(MAXLENGTH) → strlen 기준 */
	memset(rdata, 0, sizeof(rdata));
	strncpy(rdata, szRdata, MAXLENGTH);
	rdata[MAXLENGTH] = 0;
	szResidual[RESIDUAL_MAX] = 0;

	memset(temp, 0, sizeof(temp));
	len = (int)strlen(szResidual) + (int)strlen(rdata);
	if(len < MAXLENGTH*2){
		strcpy(temp, szResidual);
		strcat(temp, rdata);
	}else{
		strcpy(temp, rdata);
	}

	while(1){
		startPos = endPos = -1;
		Framing_SearchSTX_ETX(temp, startPos, endPos);
		if((len = Framing_CutSTX_ETX(temp, startPos, endPos, cb, pCtx)) <= 0){
			if(strlen(temp) < MAXLENGTH)
				strcpy(szResidual, temp);
			else
				strcpy(szResidual, "");
			break;
		}
		nFrames++;
	}
	return nFrames;
}

/*
**	원본 SearchSocketSTX_ETX(:617-636)
**	첫 STX 에서 시작해 ETX 를 찾되, 그 사이에 STX 가 다시 나오면 startPos 를 뒤 STX 로 옮긴다 (마지막 STX ~ 첫 ETX)
*/
void Framing_SearchSTX_ETX(const char *data, int &startPos, int &endPos)
{
	int i = 0, j = 0;
	for(i = 0; data[i] != 0; i++){
		if(data[i] == (char)PKT_STX){
			startPos = i;
			for(j = i+1; data[j] != 0; j++){
				if(data[j] == (char)PKT_STX)
					startPos = j;
				else if(data[j] == (char)PKT_ETX){
					endPos = j;
					break;
				}
			}
			break;
		}
	}
}

/*
**	원본 CutSocketSTX_ETX(:644-672)
**	  양쪽 있음 : command = atoi(STX 뒤 2글자), payload = STX+1 ~ ETX-1, ServerProc 호출, data = ETX 뒤 나머지. 반환 len(STX~ETX 길이)
**	  STX 만    : data = STX 부터 (ETX 를 기다림). 반환 0
**	  없음      : data = "". 반환 0
*/
int Framing_CutSTX_ETX(char *data, int startPos, int endPos, FRAME_CALLBACK cb, void *pCtx)
{
	int		ret = 0, len = 0;
	char	command[5] = "";
	char	temp[RESIDUAL_MAX+1];
	char	temp1[RESIDUAL_MAX+1];

	memset(temp, 0, sizeof(temp));
	memset(temp1, 0, sizeof(temp1));

	if(startPos != -1 && endPos != -1){
		len = endPos - startPos + 1;
		strncpy(command, data + startPos + 1, 2); command[2] = 0;
		ret = atoi(command);
		strncpy(temp1, data + startPos + 1, len - 2); temp1[len - 2] = 0;
		if(cb != NULL) cb(pCtx, ret, temp1, len - 2);
		strcpy(temp, data + endPos + 1);
		strcpy(data, temp);
	}
	else if(startPos != -1 && endPos == -1){
		strcpy(temp, data + startPos);
		strcpy(data, temp);
	}
	else{
		strcpy(data, "");
	}
	return len;
}

/*
**	원본 ReadUdpMsgProc(:541-568) : datagram 1개에서 첫 프레임만. 잔여 없음
**	반환: payload 길이(빈 프레임 STX ETX 는 0), 프레임 없음(STX/ETX 없거나 ETX 가 STX 앞) 은 -1
*/
int Framing_UdpExtract(const char *szRdata, char *szPayload, int nPayloadSize)
{
	int startPos = -1, endPos = -1, len = 0;
	char rdata[MAXLENGTH+1];

	if(szRdata == NULL || szPayload == NULL || nPayloadSize <= 0) return -1;
	szPayload[0] = 0;
	memset(rdata, 0, sizeof(rdata));
	strncpy(rdata, szRdata, MAXLENGTH);

	Framing_SearchSTX_ETX(rdata, startPos, endPos);
	if(startPos != -1 && endPos != -1){
		len = endPos - startPos + 1;
		if(len - 2 >= nPayloadSize) len = nPayloadSize + 1;	/* 방어: 호출측 버퍼 초과 방지 (원본은 1001) */
		strncpy(szPayload, rdata + startPos + 1, len - 2); szPayload[len - 2] = 0;
		return len - 2;
	}
	return -1;
}

/*
**	원본 StrSplit(:1708-1722). 반환 = 필드 수. 50 필드 / 1023 바이트 초과분은 버린다 (원본은 오버플로)
*/
int Framing_StrSplit(const char *src, char dst[STR_SPLIT_MAX][STR_SPLIT_LEN], char cmp)
{
	int j = 0, m = 0, n = 0;
	memset(&dst[0][0], 0, STR_SPLIT_MAX * STR_SPLIT_LEN);
	if(src == NULL) return 0;
	for(j = 0; src[j] != 0; j++){
		if(src[j] == cmp){
			dst[m][n] = 0;
			n = 0;
			m++;
			if(m >= STR_SPLIT_MAX){ m = STR_SPLIT_MAX - 1; break; }
		}else{
			if(n < STR_SPLIT_LEN - 1) dst[m][n++] = src[j];
		}
	}
	dst[m][n] = 0;
	return m + 1;
}

/*
**	원본 Hangul_StrCut(:764-779) : 0x80 비트가 있으면 2바이트 문자로 보고 함께 복사. 한계 넘으면 그 앞에서 자름
**	주의: 원본과 같이 2바이트 문자의 두 번째 바이트가 NUL 이면 그 뒤는 보지 않는다
*/
void Framing_HangulStrCut(const char *szSrc, char *szDst, int nMaxByteCount)
{
	int i = 0, j = 0;
	if(szDst == NULL) return;
	szDst[0] = 0;
	if(szSrc == NULL) return;
	while(szSrc[i] != 0){
		if(szSrc[i] & 0x80){
			if(j + 2 > nMaxByteCount) break;
			szDst[j++] = szSrc[i++];
			if(szSrc[i] == 0) break;			/* 짝이 없는 선행 바이트: 원본은 NUL 을 복사하고 i++ 로 넘어감(버퍼 초과 읽기). 여기서 종료 */
			szDst[j++] = szSrc[i];
		}else{
			if(j + 1 > nMaxByteCount) break;
			szDst[j++] = szSrc[i];
		}
		i++;
	}
	szDst[j] = 0;
}

/*
**	6770 송신 프레임 (원본 AlarmUser_Send:1774 / ServerProc 하트비트 ACK:731-742)
**	  STX cCmd 0xFF szMsg 0xFF szTime ETX   (szMsg 는 1000 바이트 이내로 호출측이 보장)
*/
int Framing_BuildAlarmPush(char *szOut, int nOutSize, char cCmd, const char *szMsg, const char *szTime)
{
	int n = 0, l = 0;
	if(szOut == NULL || nOutSize < 8) return 0;
	if(szMsg == NULL) szMsg = "";
	if(szTime == NULL) szTime = "";
	szOut[n++] = (char)PKT_STX;
	szOut[n++] = cCmd;
	szOut[n++] = (char)PKT_SEP;
	l = (int)strlen(szMsg);
	if(n + l + 2 + (int)strlen(szTime) + 1 >= nOutSize) l = nOutSize - n - 2 - (int)strlen(szTime) - 2;
	if(l < 0) l = 0;
	memcpy(szOut + n, szMsg, l); n += l;
	szOut[n++] = (char)PKT_SEP;
	l = (int)strlen(szTime);
	memcpy(szOut + n, szTime, l); n += l;
	szOut[n++] = (char)PKT_ETX;
	szOut[n] = 0;
	return n;
}
