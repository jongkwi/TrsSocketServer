
#include "ShareLib.h"

void strcpy_st(char *dst_str, const char *src_str)
{
	strcpy(dst_str, src_str);
}

void strncpy_st(char *dst_str, const char *src_str, int nLen)
{
	strncpy(dst_str, src_str, nLen);
}

void strcat_st(char *dst_str, const char *src_str)
{
	strcat(dst_str, src_str);
}

void strncat_st(char *dst_str, const char *src_str, int nLen)
{
	strncat(dst_str, src_str, nLen);
}

int strlen_st(const char *src_str)
{
	return strlen(src_str);
}

char * strchr_st(char *_Str, int _Ch)
{
	return strchr(_Str, _Ch);
}

char * strpbrk_st(char * _Str,  const char * _Control)
{
	return strpbrk(_Str, _Control);
}

char * strrchr_st(char * _Str,  int _Ch)
{
	return strrchr(_Str, _Ch);
}

char * strstr_st(char *_Str, const char *_SubStr)
{
	return strstr(_Str, _SubStr);
}

int vsprintf_st(char *dst_str, const char *fmt, va_list ap)
{
	return vsprintf(dst_str, fmt, ap);
}

void sprintf_st(char *dst_str, const char *fmt, ...)
{
	int			n;
	va_list		ap;

	va_start(ap, fmt);
	vsprintf_st(dst_str, fmt, ap);	/* Sigh, some vsprintf's return ptr, not length */
	n = strlen_st(dst_str);
	va_end(ap);

}

int isdigit_st(char cDigit)
{
	return isdigit(cDigit);
}

void * memset_st(void *vpDst, int nVal, int nSize)
{
	return memset(vpDst, nVal, nSize);
}

char * getenv_st(const char *strEnv)
{
	return getenv(strEnv);
}

