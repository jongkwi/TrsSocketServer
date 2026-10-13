#ifndef ___WRITEN_H___
#define ___WRITEN_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

ssize_t	writen(int fd, void *vptr, size_t n);
ssize_t WriteN(int fd, char *ptr, size_t nbytes);

#endif//writen.h


