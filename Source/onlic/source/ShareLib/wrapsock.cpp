/*
 * Socket wrapper functions.
 * These could all go into separate files, so only the ones needed cause
 * the corresponding function to be added to the executable.  If sockets
 * are a library (SVR4) this might make a difference (?), but if sockets
 * are in the kernel (BSD) it doesn't matter.
 *
 * These wrapper functions also use the same prototypes as POSIX.1g,
 * which might differ from many implementations (i.e., POSIX.1g specifies
 * the fourth argument to getsockopt() as "void *", not "char *").
 *
 * If your system's headers are not correct [i.e., the Solaris 2.5
 * <sys/socket.h> omits the "const" from the second argument to both
 * bind() and connect()], you'll get warnings of the form:
 *warning: passing arg 2 of `bind' discards `const' from pointer target type
 *warning: passing arg 2 of `connect' discards `const' from pointer target type
 */

#include "ShareLib.h"

int Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	CError	err;
	struct sockaddr_in *tmp_sa = NULL;
	char szErrLog[201] = "";

	if (bind(fd, sa, salen) < 0){
		tmp_sa = (sockaddr_in *)sa;
		perror("bind() error");
		sprintf_st(szErrLog, "bind error %s:%d",
			Sock_ntop(sa, salen), ntohs(tmp_sa->sin_port));
		err.err_sys(szErrLog);
		return -1;
	}
	return TRUE;
}

void Closefd(int fd)
{

#ifdef _WIN32
	closesocket(fd);
#else
	close(fd);
#endif
}

int ConnectFd(int fd, const struct sockaddr *sa, socklen_t salen)
{
	int n;
	CError	err;
	if ((n = connect(fd, sa, salen)) < 0)
		err.err_msg("connect error");
	return(n);
}

int Getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	CError	err;
	if (getpeername(fd, sa, salenptr) < 0){
		err.err_sys("getpeername error");
		return -1;
	}
	return TRUE;
}

int Getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	CError	err;
	if (getsockname(fd, sa, salenptr) < 0){
		err.err_sys("getsockname error");
		return -1;
	}
	return TRUE;
}

void Getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlenptr)
{
	CError	err;
	if (getsockopt(fd, level, optname, (char*)optval, optlenptr) < 0)
		err.err_sys("getsockopt error");
}

#ifdef	HAVE_INET6_RTH_INIT
int Inet6_rth_space(int type, int segments)
{
	CError	err;
	int ret;
	
	ret = inet6_rth_space(type, segments);
	if (ret < 0)
		err.err_quit("inet6_rth_space error");

	return ret;
}

void * Inet6_rth_init(void *rthbuf, socklen_t rthlen, int type, int segments)
{
	CError	err;
	void *ret;

	ret = inet6_rth_init(rthbuf, rthlen, type, segments);
	if (ret == NULL)
		err.err_quit("inet6_rth_init error");

	return ret;
}

void Inet6_rth_add(void *rthbuf, const struct in6_addr *addr)
{
	CError	err;
	if (inet6_rth_add(rthbuf, addr) < 0)
		err.err_quit("inet6_rth_add error");
}

void Inet6_rth_reverse(const void *in, void *out)
{
	CError	err;
	if (inet6_rth_reverse(in, out) < 0)
		err.err_quit("inet6_rth_reverse error");
}

int Inet6_rth_segments(const void *rthbuf)
{
	CError	err;
	int ret;

	ret = inet6_rth_segments(rthbuf);
	if (ret < 0)
		err.err_quit("inet6_rth_segments error");

	return ret;
}

struct in6_addr * Inet6_rth_getaddr(const void *rthbuf, int idx)
{
	CError	err;
	struct in6_addr *ret;

	ret = inet6_rth_getaddr(rthbuf, idx);
	if (ret == NULL)
		err.err_quit("inet6_rth_getaddr error");

	return ret;
}
#endif

#ifdef HAVE_KQUEUE
int Kqueue(void)
{
	CError	err;
	int ret;

	if ((ret = kqueue()) < 0)
		err.err_sys("kqueue error");
	return ret;
}

int Kevent(int kq, const struct kevent *changelist, int nchanges,
       struct kevent *eventlist, int nevents, const struct timespec *timeout)
{
	CError	err;
	int ret;

	if ((ret = kevent(kq, changelist, nchanges, eventlist, nevents, timeout)) < 0)
		err.err_sys("kevent error");
	return ret;
}
#endif


/* include Listen */
void Listen(int fd, int backlog)
{
	CError	err;
	char	*ptr;

		/*4can override 2nd argument with environment variable */
	if ( (ptr = getenv_st("LISTENQ")) != NULL)
		backlog = atoi(ptr);

	if (listen(fd, backlog) < 0)
		err.err_sys("listen error");
}
/* end Listen */

#ifdef	HAVE_POLL
int Poll(struct pollfd *fdarray, unsigned long nfds, int timeout)
{
	CError	err;
	int		n;

	if ( (n = poll(fdarray, nfds, timeout)) < 0)
		err.err_sys("poll error");

	return(n);
}
#endif

ssize_t Recv(int fd, void *ptr, size_t nbytes, int flags)
{
	CError	err;
	ssize_t		n;

	if ( (n = recv(fd, (char*)ptr, nbytes, flags)) < 0)
		err.err_msg("recv error");
	return(n);
}

ssize_t Recvfrom(int fd, void *ptr, size_t nbytes, int flags, struct sockaddr *sa, socklen_t *salenptr)
{
	CError	err;
	ssize_t		n;

	if ( (n = recvfrom(fd, (char*)ptr, nbytes, flags, sa, salenptr)) < 0)
		err.err_msg("recvfrom error");
	return(n);
}


int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	CError	err;
	int		n = 0;

	if ( (n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
		err.err_sys("select error");

	return(n);		/* can return 0 on timeout */
}

int Send(int fd, const void *ptr, size_t nbytes, int flags)
{
	ssize_t nSend = 0;
	CError	err;

	try{
	
	if ((nSend = send(fd, (const char *)ptr, nbytes, flags)) != (ssize_t)nbytes){		
		err.err_msg("send error");
	}
	}catch(...){
		printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Send Error \r\n");
	}

	return nSend;	
}

int Sendto(int fd, const void *ptr, size_t nbytes, int flags,
	   const struct sockaddr *sa, socklen_t salen)
{
	ssize_t nSend;
	CError	err;
	nSend = sendto(fd, (const char *)ptr, nbytes, flags, sa, salen);
	if (nSend != (ssize_t)nbytes)
		err.err_msg("sendto error");
	return nSend;
}

void Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	CError	err;
	if (setsockopt(fd, level, optname, (const char *)optval, optlen) < 0)
		err.err_sys("setsockopt error");
}

/* include _inet_pton */
int _inet_pton(int family, const char *strptr, void *addrptr)
{
	if (family == AF_INET) {
		struct in_addr  in_val;
		in_val.s_addr = inet_addr(strptr);
		if (in_val.s_addr > 0) {
			memcpy(addrptr, &in_val, sizeof(struct in_addr));
			return (1);
		}
		return(0);
	}
#ifdef _WIN32
	errno = WSAEAFNOSUPPORT;
#else
	errno = EAFNOSUPPORT;
#endif
	return (-1);
}
/* end _inet_pton */

const char* _inet_ntop(int family, const void *addrptr, char *strptr, size_t len)
{
	const u_char *p = (const u_char *) addrptr;
	char temp[51];

	if (family == AF_INET) {
		nprintf(temp, 50, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
		if (strlen_st(temp) == len) {
			errno = ENOSPC;
			return (NULL);
		}
		strcpy_st(strptr, temp);
		return strptr;
	}

#ifdef	_WIN32
	errno = WSAEAFNOSUPPORT;
#else
	errno = EAFNOSUPPORT;
#endif
	return 0;
}

/* include sock_ntop */
char * sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
    static char str[128];		/* Unix domain is largest */

	switch (sa->sa_family) {
	case AF_INET:{
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

		if (_inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			return(NULL);

/*		if (ntohs(sin->sin_port) != 0) {
			inet_printf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
			strcat_st(str, portstr);
		}*/
		return(str);
	}
/* end sock_ntop */

#ifdef	IPV6
	case AF_INET6: {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

		str[0] = '[';
		if (_inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1, sizeof(str) - 1) == FALSE)
			return(NULL);
		if (ntohs(sin6->sin6_port) != 0) {
			inet_printf(portstr, sizeof(portstr), "]:%d", ntohs(sin6->sin6_port));
			strcat_st(str, portstr);
			return(str);
		}
		return (str + 1);
	} 
#endif

#ifdef	_UNIX
	case AF_UNIX: {
		struct sockaddr_un	*unp = (struct sockaddr_un *) sa;

			/* OK to have no pathname bound to the socket: happens on
			   every connect() unless client calls bind() first. */
		if (unp->sun_path[0] == 0)
			strcpy_st(str, "(no pathname bound)");
		else
			nprintf(str, sizeof(str), "%s", unp->sun_path);
		return(str);
	}
#endif

	default:
		nprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d",
				 sa->sa_family, salen);
		return(str);
	}
    return (NULL);
}

char * Sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
	CError	err;
	char	*ptr;

	if ( (ptr = sock_ntop(sa, salen)) == NULL)
		err.err_sys("sock_ntop error");	/* _inet_ntop() sets errno */
	return(ptr);
}


int Sock_pton(int family, const char *strptr, void *addrptr)
{
	int nRet;
	nRet = _inet_pton(family, strptr, addrptr);
	return nRet;
}

int Shutdown(int fd, int how)
{
	CError	err;
	int n;
	if ((n = shutdown(fd, how)) < 0)
		err.err_sys("shutdown error");
	return (n);
}

int Sockatmark(int fd)
{
	CError	err;
	int		n = 0;

#ifdef _UNIX
	if ( (n = sockatmark(fd)) < 0)
		err.err_sys("sockatmark error");
#endif
	return(n);
}

/* include Socket */
int Socket(int family, int type, int protocol)
{
	CError	err;
	int		n;

	if ( (n = socket(family, type, protocol)) < 0)
		err.err_sys("socket error");
	return(n);
}
/* end Socket */

void Socketpair(int family, int type, int protocol, int *fd)
{
	CError	err;
	int		n = 0;

#ifdef _UNIX
	if ( (n = socketpair(family, type, protocol, fd)) < 0)
		err.err_sys("socketpair error");
#endif
}

int ip_check(char *pa)
{
    char *ptrc;

    ptrc = pa;
    int j;
    char a1[5],a2[5],a3[5],a4[5];

    for (j = 0; j <= 3; j++,ptrc++){
        if (*ptrc != '.') a1[j] = *ptrc;
        else{
            a1[j] = '\0';
            ptrc++;
            break;
        }
    }

    for (j = 0; j <= 3; j++,ptrc++){
        if (*ptrc != '.') a2[j] = *ptrc;
        else{
            a2[j] = '\0';
            ptrc++;
            break;
        }
    }

    for (j = 0; j <= 3; j++,ptrc++){
        if (*ptrc != '.') a3[j] = *ptrc;
        else{
            a3[j] = '\0';
            ptrc++;
            break;
        }
    }

    for (j = 0; j <= 3; j++,ptrc++){
        if (*ptrc != '.') a4[j] = *ptrc;
        else{
            a4[j] = '\0';
            ptrc++;
            break;
        }
    }

    if (atoi(a1) > 255) return FALSE;
    if (atoi(a2) > 255) return FALSE;
    if (atoi(a3) > 255) return FALSE;
    if (atoi(a4) > 255) return FALSE;
    if ((atoi(a1) == 0) && (atoi(a2) == 0) && (atoi(a3) == 0) && (atoi(a4) == 0)) return FALSE;

    return TRUE;
}

