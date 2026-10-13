#ifndef __WRAPUNIX_H__
#define __WRAPUNIX_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h> 
#include <sys/ioctl.h>

#define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
					/* default file access permissions for new files */
#define	DIR_MODE	(FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)
					/* default permissions for new directories */

void *Calloc(size_t n, size_t size);
void Close(int fd);
void Dup2(int fd1, int fd2);
int Fcntl(int fd, int cmd, int arg);
void Gettimeofday(struct timeval *tv, void *foo);
int Ioctl(int fd, int request, void *arg);
pid_t Fork(void);
int Open(const char *pathname, int oflag, mode_t mode);
ssize_t Read(int fd, void *ptr, size_t nbytes);
void Sigaddset(sigset_t *set, int signo);
void Sigdelset(sigset_t *set, int signo);
void Sigemptyset(sigset_t *set);
void Sigfillset(sigset_t *set);
int Sigismember(const sigset_t *set, int signo);
void Sigpending(sigset_t *set);
void Sigprocmask(int how, const sigset_t *set, sigset_t *oset);
char *Strdup(const char *str);
pid_t Wait(int *iptr);
pid_t Waitpid(pid_t pid, int *iptr, int options);
int Mkstemp(char *szTemplate);
void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
void Pipe(int *fds);
long Sysconf(int name);

#ifdef	HAVE_SYS_SYSCTL_H
void Sysctl(int *name, u_int namelen, void *oldp, size_t *oldlenp, void *newp, size_t newlen);
#endif

void Unlink(const char *pathname);
void Write(int fd, void *ptr, size_t nbytes);


#endif//wrapunix.h


