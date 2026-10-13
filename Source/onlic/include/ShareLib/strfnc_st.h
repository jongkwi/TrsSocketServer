#ifndef ___STRFNC_H___
#define ___STRFNC_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

void strcpy_st(char *dst_str, const char *src_str);
void strncpy_st(char *dst_str, const char *src_str, int nLen);
void strcat_st(char *dst_str, const char *src_str);
void strncat_st(char *dst_str, const char *src_str, int nLen);
int strlen_st(const char *src_str);
char * strchr_st(char *_Str, int _Ch);
char * strpbrk_st(char * _Str,  const char * _Control);
char * strrchr_st(char * _Str,  int _Ch);
char * strstr_st(char *_Str, const char *_SubStr);
int vsprintf_st(char *dst_str, const char *fmt, va_list ap);
void sprintf_st(char *dst_str, const char *fmt, ...);
int isdigit_st(char cDigit);
char * getenv_st(const char *strEnv);

void * memset_st(void *vpDst, int nVal, int nSize);


#endif//strfnc.h


