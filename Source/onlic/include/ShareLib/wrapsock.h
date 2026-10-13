#ifndef ___WRAPSOCK_H___
#define ___WRAPSOCK_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

int Bind(int fd, const struct sockaddr *sa, socklen_t salen);
void Closefd(int fd);
int ConnectFd(int fd, const struct sockaddr *sa, socklen_t salen);
int Getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr);
int Getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr);
void Getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlenptr);

#ifdef	HAVE_INET6_RTH_INIT
int Inet6_rth_space(int type, int segments);
void * Inet6_rth_init(void *rthbuf, socklen_t rthlen, int type, int segments);
void Inet6_rth_add(void *rthbuf, const struct in6_addr *addr);
void Inet6_rth_reverse(const void *in, void *out);
int Inet6_rth_segments(const void *rthbuf);
struct in6_addr * Inet6_rth_getaddr(const void *rthbuf, int idx);
#endif

#ifdef HAVE_KQUEUE
int Kqueue(void);
int Kevent(int kq, const struct kevent *changelist, int nchanges,
       struct kevent *eventlist, int nevents, const struct timespec *timeout);
#endif

void Listen(int fd, int backlog);

#ifdef	HAVE_POLL
int Poll(struct pollfd *fdarray, unsigned long nfds, int timeout)
#endif


ssize_t Recv(int fd, void *ptr, size_t nbytes, int flags);
ssize_t Recvfrom(int fd, void *ptr, size_t nbytes, int flags, struct sockaddr *sa, socklen_t *salenptr);
int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
int Send(int fd, const void *ptr, size_t nbytes, int flags);
int Sendto(int fd, const void *ptr, size_t nbytes, int flags,
	   const struct sockaddr *sa, socklen_t salen);
void Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);
char * Sock_ntop(const struct sockaddr *sa, socklen_t salen);
int Sock_pton(int family, const char *strptr, void *addrptr);
int Shutdown(int fd, int how);
int Sockatmark(int fd);
int Socket(int family, int type, int protocol);
void Socketpair(int family, int type, int protocol, int *fd);
int ip_check(char *pa);

#endif//wrapsock.h


