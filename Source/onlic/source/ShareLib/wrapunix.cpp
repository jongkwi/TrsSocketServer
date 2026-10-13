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

void *Calloc(size_t n, size_t size)
{
	CError err;
	void	*ptr;

	if ( (ptr = calloc(n, size)) == NULL)
		err.err_sys("calloc error");
	return(ptr);
}

void Close(int fd)
{
	CError err;
	if (close(fd) == -1)
		err.err_sys("close error");
}

void Dup2(int fd1, int fd2)
{
	CError err;
	if (dup2(fd1, fd2) == -1)
		err.err_sys("dup2 error");
}

int Fcntl(int fd, int cmd, int arg)
{
	CError err;
	int	n;

	if ( (n = fcntl(fd, cmd, arg)) == -1)
		err.err_sys("fcntl error");
	return(n);
}

void Gettimeofday(struct timeval *tv, void *foo)
{
	CError err;
	if (gettimeofday(tv, NULL) == -1)
		err.err_sys("gettimeofday error");
	return;
}

int Ioctl(int fd, int request, void *arg)
{
	CError err;
	int		n;

	if ( (n = ioctl(fd, request, arg)) == -1)
		err.err_sys("ioctl error");
	return(n);	/* streamio of I_LIST returns value */
}

pid_t Fork(void)
{
	CError err;
	pid_t	pid;

	if ( (pid = fork()) == -1)
		err.err_sys("fork error");
	return(pid);
}

int Mkstemp(char *szTemplate)
{
	CError err;
	int i, j;

	j = mkstemp(szTemplate);
	if (j == 0 || szTemplate[0] == 0)
		err.err_quit("mkstemp error");
	i = Open(szTemplate, O_CREAT | O_WRONLY, FILE_MODE);

	return i;
}

#include	<sys/mman.h>

void * Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	CError err;
	void	*ptr;

	if ( (ptr = mmap(addr, len, prot, flags, fd, offset)) == ((void *) -1))
		err.err_sys("mmap error");
	return(ptr);
}

int Open(const char *pathname, int oflag, mode_t mode)
{
	CError err;
	int		fd;

	if ( (fd = open(pathname, oflag, mode)) == -1)
		err.err_sys("open error for %s", pathname);
	return(fd);
}

void Pipe(int *fds)
{
	CError err;
	if (pipe(fds) < 0)
		err.err_sys("pipe error");
}

ssize_t Read(int fd, void *ptr, size_t nbytes)
{
	CError err;
	ssize_t		n;

	if ( (n = read(fd, ptr, nbytes)) == -1)
		err.err_sys("read error");
	return(n);
}

void Sigaddset(sigset_t *set, int signo)
{
	CError err;
	if (sigaddset(set, signo) == -1)
		err.err_sys("sigaddset error");
}

void Sigdelset(sigset_t *set, int signo)
{
	CError err;
	if (sigdelset(set, signo) == -1)
		err.err_sys("sigdelset error");
}

void Sigemptyset(sigset_t *set)
{
	CError err;
	if (sigemptyset(set) == -1)
		err.err_sys("sigemptyset error");
}

void Sigfillset(sigset_t *set)
{
	CError err;
	if (sigfillset(set) == -1)
		err.err_sys("sigfillset error");
}

int Sigismember(const sigset_t *set, int signo)
{
	CError err;
	int		n;

	if ( (n = sigismember(set, signo)) == -1)
		err.err_sys("sigismember error");
	return(n);
}

void Sigpending(sigset_t *set)
{
	CError err;
	if (sigpending(set) == -1)
		err.err_sys("sigpending error");
}

void Sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
	CError err;
	if (sigprocmask(how, set, oset) == -1)
		err.err_sys("sigprocmask error");
}

char * Strdup(const char *str)
{
	CError err;
	char	*ptr;

	if ( (ptr = strdup(str)) == NULL)
		err.err_sys("strdup error");
	return(ptr);
}

long Sysconf(int name)
{
	CError err;
	long	val;

	errno = 0;		/* in case sysconf() does not change this */
	if ( (val = sysconf(name)) == -1)
		err.err_sys("sysconf error");
	return(val);
}

#ifdef	HAVE_SYS_SYSCTL_H
void Sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp,
	   void *newp, size_t newlen)
{
	CError err;
	if (sysctl(name, namelen, oldp, oldlenp, newp, newlen) == -1)
		err.err_sys("sysctl error");
}
#endif

void Unlink(const char *pathname)
{
	CError err;
	if (unlink(pathname) == -1)
		err.err_sys("unlink error for %s", pathname);
}

pid_t Wait(int *iptr)
{
	CError err;
	pid_t	pid;

	if ( (pid = wait(iptr)) == -1)
		err.err_sys("wait error");
	return(pid);
}

pid_t Waitpid(pid_t pid, int *iptr, int options)
{
	CError err;
	pid_t	retpid;

	if ( (retpid = waitpid(pid, iptr, options)) == -1)
		err.err_sys("waitpid error");
	return(retpid);
}

void Write(int fd, void *ptr, size_t nbytes)
{
	CError err;
	if (write(fd, ptr, nbytes) != nbytes)
		err.err_sys("write error");
}
