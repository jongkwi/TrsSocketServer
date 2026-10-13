#ifndef ___UTFICONV_H___
#define ___UTFICONV_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

#ifdef _WIN32
	#define iconv_t		long
#endif

iconv_t IConv_open(char *toCode, char *fromCode);
size_t IConv(iconv_t cd, const char *inbuf, size_t inbytesleft, char *outbuf, size_t outbytesleft);
//int IConvctl(iconv_t cd , int request, void *argument);
int IConv_close(iconv_t cd);

#endif//UtfIConv.h


