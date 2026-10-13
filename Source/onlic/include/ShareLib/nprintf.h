#ifndef ___STRNPRINTF_H___
#define ___STRNPRINTF_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

int nprintf(char *buf, size_t size, const char *fmt, ...);
void flushprintf(const char *fmt, ...);

#endif//strnprintf.h


