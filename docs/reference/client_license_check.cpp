/*
 * [참조 자료] IVR 클라이언트 ShareLib 의 라이선스 체크 부분 (사용자 제공, 2026-10-13)
 * 이 파일은 TrsSocketServer 빌드 대상이 아니다. TCP 6321 프로토콜 호환 검증용 근거 자료.
 * 핵심 함수: WriteServerLog (서버 요청/응답 처리), WriteLicense / CheckLicense (로컬 .lic 생성/검증)
 *
 * pthreads wrapper functions.
 */

#include "ShareLib.h"

#ifdef _WIN32
TEventDummy::TEventDummy()
{
    m_hObject = ::CreateEvent(NULL, 0, 0, NULL);
    if(m_hObject)
        ::ResetEvent(m_hObject);
    m_add_cnt = 0;
}

void TEventDummy::InitEvent()
{
    m_hObject = ::CreateEvent(NULL, 0, 0, NULL);
    if(m_hObject)
        ::ResetEvent(m_hObject);
    m_add_cnt = 0;
}
BOOL TEventDummy::SetEvent()
{
    if(m_hObject){
        m_add_cnt++;
        return ::SetEvent(m_hObject);
    }
    return FALSE;
}

void TEventDummy::ResetEvent()
{
    if(m_hObject){
        m_add_cnt = 0;
        ::ResetEvent(m_hObject);
    }
}
#endif

void Pthread_create(pthread_t *tid, void * (*func)(void *), void *arg)
{

#ifdef _WIN32
    long    th_id;
    th_id = _beginthread((void (__cdecl *)(void *))*func,  0, (void*)arg);
    if(tid)
        *tid = th_id;
#else
    CError err;
    int        n;
    if((n = pthread_create((pthread_t *)tid, 0, func, arg)) == 0)
        return;
    errno = n;
    err.err_msg("pthread_create error no:%d, %s\n", n, strerror(errno));
#endif
}

void Pthread_join(pthread_t  __th, void** __thread_return)
{
#ifdef _WIN32
    WaitForSingleObject((HANDLE)__th, INFINITE);
#else
    CError err;
    int        n;
    if((n = pthread_join(__th, __thread_return)) == 0)
        return;
    errno = n;
    err.err_msg("Pthread_join error");
#endif
}

void Pthread_kill(pthread_t *tid, int signo)
{

#ifdef _WIN32
    TerminateThread(tid, signo);
#else
    CError err;
    int        n;

    if ( (n = pthread_kill(*tid, signo)) == 0)
        return;

    errno = n;
    err.err_msg("pthread_kill error");
#endif
}

void Pthread_mutex_init(pthread_mutex_t *__mutex, pthread_mutexattr_t *__mutex_attr)
{
#ifdef _WIN32
    InitializeCriticalSection(__mutex);
#else
#ifdef _FREEBSD
    *__mutex = PTHREAD_MUTEX_INITIALIZER;
    __mutex_attr = NULL;
#else
    int        n;
    __mutex_attr = NULL;
    n = pthread_mutex_init(__mutex, __mutex_attr);
    errno = n;
    if (n < 0)
        printf("Pthread_mutex_init error:%s\n", strerror(errno));
#endif
#endif
}

void Pthread_mutex_lock(pthread_mutex_t *__mutex)
{
    if(__mutex == NULL){
        printf("pthread_mutex_lock is NULL");
        return;
    }
#ifdef _WIN32
    EnterCriticalSection(__mutex);
#else
    CError err;
    int        n;
    if((n = pthread_mutex_lock(__mutex)) == 0)
        return;
    errno = n;
    err.err_msg("pthread_mutex_lock error:%d", n);
#endif
}

void Pthread_mutex_unlock(pthread_mutex_t *__mutex)
{
    if(__mutex == NULL){
        printf("pthread_mutex_unlock is NULL");
        return;
    }
#ifdef _WIN32
    LeaveCriticalSection(__mutex);
#else
    CError err;
    int        n;
    if((n = pthread_mutex_unlock(__mutex)) == 0)
        return;
    errno = n;
    err.err_msg("pthread_mutex_unlock error:%d", n);
#endif
}

void Pthread_cond_init(pthread_cond_t *__cond, pthread_condattr_t *__cond_attr)
{
#ifdef _WIN32
    __cond->InitEvent();
    return;
#else
#ifdef _FREEBSD
    *__cond = PTHREAD_COND_INITIALIZER;
    __cond_attr = NULL;
#else
    CError err;
    int        n;

    __cond_attr = NULL;
    n = pthread_cond_init(__cond, __cond_attr);
    if(n == 0){
        return;
    }
    errno = n;
    err.err_msg("Pthread_cond_init error");
#endif
#endif
}

int Pthread_cond_wait(pthread_cond_t* __cond, pthread_mutex_t* __mutex)
{
    int ret = 0;
#ifdef _WIN32
    if(!__cond)
        return -1;

    ret = WaitForSingleObject(__cond->m_hObject, INFINITE);

    switch(ret){
      case WAIT_OBJECT_0:
        ret = TRUE;
        break;
      case WAIT_TIMEOUT:
      case WAIT_ABANDONED:
      default:
        ret = -1;
        __cond->InitEvent();
        WaitSleep(10);
        break;
    }
#else
    CError err;
    int        n;

    n = pthread_cond_wait(__cond, __mutex);
    switch(n){
      case EINTR:
        ret = TRUE;
        break;
      case ETIMEDOUT:
      case EBUSY:
      default:
        ret = -1;
        break;
    }
#endif
    return ret;
}

int Pthread_cond_waittime(pthread_cond_t* __cond, pthread_mutex_t* __mutex, timespec *tsptr)
{
    int ret = 0;
#ifdef _WIN32
    if(!__cond)
        return -1;

    ret = WaitForSingleObject(__cond->m_hObject, (unsigned long)tsptr);

    switch(ret){
      case WAIT_ABANDONED:
        ret = -1;
        __cond->InitEvent();
        WaitSleep(10);
        break;
      case WAIT_OBJECT_0:
        ret = TRUE;
        break;
      case WAIT_TIMEOUT:
      default:
        ret = FALSE;
        break;
    }
#else
    CError err;
    int        n;

    n = pthread_cond_timedwait(__cond, __mutex, tsptr);
    switch(n){
      case ETIMEDOUT:
        ret = 0;
        break;
      case EINTR:
      default:
        ret = TRUE;
        break;
    }
#endif
    return ret;
}

void Pthread_cond_signal(pthread_cond_t* __cond)
{
#ifdef _WIN32
    if(!__cond)
        return;
    __cond->SetEvent();
#else
    int n = pthread_cond_signal(__cond);
    if (n < 0)
        printf("Pthread_cond_signal error:%s\n", strerror(n));
#endif
}



int CheckLicense(int ns_line)
{
    int i = 0, nFind = FALSE, nLine;
    char szMac[51] = "", szMac_addr[51] = "", szLine[6] = "", szExpiresDay[51] = "", szLic_type[11] = "";
    char szLic_key[101] = "";
    char cnonce[33] = "";
    char nonce[33] = "";
    char realm[51] = "";
    char nc[11] = "";
    char qop[11] = "";
    char szMethod[21] = "";
    char szUri[101] = "";
    char szCurPath[1024] = "";
    char szCurFile[1024] = "";
    char szLocalDay[21] = "";
    char szSKey[51] = "", szNKey[51];
    HASHHEX sessionKey;
    HASHHEX szResponse;
    MD5Algorithm alMd5 = md5;

    GetCurrentExe(szCurFile, sizeof(szCurFile));

    strcpy(szUri, "sip:www.nasacti.com");
    strcpy(szMethod, "LICENSE");
    strcpy(nonce, szMac_addr);
    strcpy(realm, "nasacti");
    strcpy(szLic_type, "IVR");
    GetCurrentExe(szCurPath, 1024);

    for(i = 0; i < 30; i++){
        memset(&szMac, 0, 51);
        ReadNicMac(i, szMac, 0);
        if(strcmp(szMac, "") == FALSE) break;
        else{
#ifdef _WIN32
            sprintf_st(szCurFile, "%s\\%s%s.lic", szCurPath, szLic_type, szMac);
#else
            sprintf_st(szCurFile, "%s/%s%s.lic", szCurPath, szLic_type, szMac);
#endif

            strcpy(szSKey, "LICENSE");
            strcpy(szNKey, "MAC_ADDR");
            GetPrivateProfileStringUsr(szSKey, szNKey, (char*)"", szMac_addr, 50, szCurFile);
            if(strlen(szMac_addr) <= 0) continue;
            strcpy(szNKey, "LINE");
            GetPrivateProfileStringUsr(szSKey, szNKey, (char*)"", szLine, 6, szCurFile);
            nLine = atoi(szLine);
            if(nLine <= 0) break;
            strcpy(szSKey, "LICENSE");
            strcpy(szNKey, "LIC_KEY");
            GetPrivateProfileStringUsr(szSKey, szNKey, (char*)"", szLic_key, 100, szCurFile);
            strcpy(szSKey, "LICENSE");
            strcpy(szNKey, "EXPIRES_DAY");
            GetPrivateProfileStringUsr(szSKey, szNKey, (char*)"", szExpiresDay, 20, szCurFile);
            if(strlen(szExpiresDay) <= 0) break;

            sprintf(nonce, "%s%s%d", szMac_addr, szExpiresDay, nLine);
            digestCalcHA1(alMd5, szLic_type, szLine, realm, nonce, cnonce, sessionKey);
            digestCalcResponse(sessionKey, nonce, nc, cnonce, qop, szMethod, szUri, NULL, szResponse);

            if(strcmp(szResponse, szLic_key) == FALSE){
                GetLocalDayProc(szLocalDay);

                if(ns_line > nLine || ns_line <= 0) nFind = FALSE;
                else if(strcmp(szLocalDay, szExpiresDay) > 0) nFind = FALSE;
                else nFind = TRUE;
                break;
            }
        }
    }

    return nFind;
}

void WriteLicense(int ns_line, char *szUser, char *szMac, char *szLocalIP, char *szExpireDay, char *szRecv)
{
    // TODO: Add your control notification handler code here
    char szLic_line[11] = "";
    char szLic_key[101] = "";
    char cnonce[33] = "";
    char nonce[33] = "";
    char realm[51] = "";
    char nc[11] = "";
    char qop[11] = "";
    char szMethod[21] = "";
    char szUri[101] = "";
    char szLog[4096] = "";
    char szFullPath[1024] = "";
    HASHHEX sessionKey;
    HASHHEX szResponse;
    MD5Algorithm alMd5 = md5;
    int fd;
    char szData[1024];
    TCHAR infoBuf[256];
//    CDaoDatabase m_db;

    sprintf(szLic_line, "%d", ns_line);

    strcpy(szUri, "sip:www.nasacti.com");
    strcpy(szMethod, "LICENSE");
    sprintf(nonce, "%s%s%s", szMac, szExpireDay, szLic_line);
    strcpy(realm, "nasacti");

    digestCalcHA1(alMd5, (char*)(LPCSTR)szUser, (char*)(LPCSTR)szLic_line, realm, nonce, cnonce, sessionKey);
    digestCalcResponse(sessionKey, nonce, nc, cnonce, qop, szMethod, szUri, NULL, szResponse);

#ifdef _WIN32
    GetCurrentExe(infoBuf, 1024);
    sprintf(szFullPath, "%s/IVR%s.lic", infoBuf, szMac);
#else
    GetCurrentExe(infoBuf, 1024);
    sprintf(szFullPath, "%s/IVR%s.lic", infoBuf, szMac);
#endif

#ifdef _WIN32
    DeleteFile(szFullPath);
#else
    unlink(szFullPath);
#endif

#ifdef _WIN32
    fd = _open(szFullPath, O_CREAT | O_WRONLY | O_BINARY, 666);
#else
    fd = open(szFullPath, O_CREAT | O_RDWR, 500);
#endif
    if(fd > 0){
        strcpy(szData, "[LICENSE]\r\n");
#ifdef _WIN32
        _write(fd, szData, strlen(szData));
#else
        write(fd, szData, strlen(szData));
#endif
        sprintf(szData, "MAC_ADDR=%s\r\n", szMac);
#ifdef _WIN32
        _write(fd, szData, strlen(szData));
#else
        write(fd, szData, strlen(szData));
#endif
        sprintf(szData, "LINE=%s\r\n", szLic_line);
#ifdef _WIN32
        _write(fd, szData, strlen(szData));
#else
        write(fd, szData, strlen(szData));
#endif
        sprintf(szData, "EXPIRES_DAY=%s\r\n", szExpireDay);
#ifdef _WIN32
        _write(fd, szData, strlen(szData));
#else
        write(fd, szData, strlen(szData));
#endif
        sprintf(szData, "LIC_TYPE=%s\r\n", szUser);
#ifdef _WIN32
        _write(fd, szData, strlen(szData));
#else
        write(fd, szData, strlen(szData));
#endif
        sprintf(szData, "LIC_KEY=%s\r\n", szResponse);
#ifdef _WIN32
        _write(fd, szData, strlen(szData));
#else
        write(fd, szData, strlen(szData));
#endif

#ifdef _WIN32
        _close(fd);
#else
        close(fd);
#endif
        /*
#ifdef _UNIX
        char szSysCmd[51] = "";
        sprintf(szSysCmd, "touch -t 1001131300 %s", szFullPath);
        system(szSysCmd);
#endif

        */
    }
}

int WriteServerLog(int nLine, char *szMyIP)
{
    int ch_line = nLine;
    int handle, nCon, nRecvLen, nSAlen, nSendLen, nRet;
    char szTmp[51], buf[1024], szIP[21], szRecv[51], szHostName[101], szExpireDay[21], szMac[51] = "";
    struct  hostent *hp;
    struct sockaddr_in src_sa;
    struct sockaddr_in dst_sa;
    nSAlen = sizeof(src_sa);

#if (defined(_FREEBSD) && defined(_NO_CHECK_LIC))   // 아크로 장비 / 부산 - 180730
    if(strncmp(szMyIP, "220.73.223", 10) == FALSE){
        return 1;
    }
#endif


    int i, nFind = FALSE;
    for(i = 0; i < 10; i++){
        memset(&szTmp, 0, 50);
        nRet = ReadNicMac(i, szTmp, FALSE);
        if(strcmp(szTmp, "") == FALSE || nRet <= 0){
            continue;
        }
        else{
            nFind = TRUE;
            strcpy(szMac, szTmp);
        }
        break;
    }
    if(nFind == FALSE)
        KillLib(ch_line, szMac, 1);

    memset(&src_sa, 0, sizeof(sockaddr_in));
    memset(&dst_sa, 0, sizeof(sockaddr_in));
    handle = Socket(AF_INET, SOCK_STREAM, 0);


    struct timeval tv_timeo;
    tv_timeo.tv_sec = 5;
    tv_timeo.tv_usec = 0;
    Setsockopt(handle, SOL_SOCKET, SO_SNDTIMEO,  &tv_timeo, sizeof( timeval ));
    Setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO,  &tv_timeo, sizeof( timeval ));

    strcpy(szHostName, "lic.onpia.co.kr");
    if((hp=(struct hostent *)gethostbyname(szHostName))==NULL){
        KillLib(ch_line, szMac, 2);
    }
    memcpy((char *)&dst_sa.sin_addr,(char *)hp->h_addr,hp->h_length);
    dst_sa.sin_family = AF_INET;
    src_sa.sin_family = AF_INET;
    dst_sa.sin_port = htons(6321);
    strcpy_st(szIP, Sock_ntop((SOCK_ADDR *)&dst_sa, nSAlen));



    nCon = ConnectFd(handle, (SOCK_ADDR*)&dst_sa, nSAlen);
    if(nCon < 0){
        strcpy(szHostName, "lic2.onpia.co.kr");
        if((hp=(struct hostent *)gethostbyname(szHostName))==NULL){
            KillLib(ch_line, szMac, 3);
        }
        Closefd(handle);
        handle = Socket(AF_INET, SOCK_STREAM, 0);
        memcpy((char *)&dst_sa.sin_addr,(char *)hp->h_addr,hp->h_length);
        strcpy_st(szIP, Sock_ntop((SOCK_ADDR *)&dst_sa, nSAlen));
        dst_sa.sin_port = htons(6321);
        nCon = ConnectFd(handle, (SOCK_ADDR*)&dst_sa, nSAlen);
        if(nCon < 0){
            KillLib(ch_line, szMac, 4);
        }
    }

    WaitMSleep(1000);
    Getsockname(handle, (SOCK_ADDR*)&src_sa, (socklen_t*)&nSAlen);
    Getpeername(handle, (SOCK_ADDR*)&dst_sa, (socklen_t*)&nSAlen);

    sprintf(buf, "%c%s%c%sIP:%s%c", 0x02, "IVR", 0xff, szMac, szMyIP, 0x03);
    nSendLen = Send(handle, buf, strlen(buf), 0);
    int nfds;
    fd_set read_fds;
    struct timeval    tval;
    tval.tv_sec = 5;
    tval.tv_usec = 0;

    memset(&szRecv, 0, 50);
    nfds = handle + 1;
    FD_ZERO(&read_fds);

    FD_SET(handle, &read_fds);
    if(select(nfds, &read_fds, NULL, NULL, &tval) < 0) {
        KillLib(ch_line, szMac, 5);
    }

    nRecvLen = Recv(handle, szRecv, 50, FALSE);
    if(nRecvLen > 0){
        if(strcmp(szRecv, "Run") == FALSE){
            strcpy(szExpireDay, "9999-99-99");
            WriteLicense(ch_line, (char*)"IVR", szMac, (char*)(LPCSTR)szIP, szExpireDay, szRecv);
            nRet = TRUE;
        }
        else if(strcmp(szRecv, "Close") == FALSE){
            KillLib(ch_line, szMac, 6);
        }
        else{
            KillLib(ch_line, szMac, 7);
        }
    }
    else{
        KillLib(ch_line, szMac, 8);
        return -1;
    }

    Closefd(handle);
    return 1;
}

void KillLib(int ch_line, char *szMac, int nFunc)
{
    SysErrorMsg("Process error KillLib ch:%d, szMac:%s, nFunc:%d", ch_line, szMac, nFunc);
    exit(0);
}
