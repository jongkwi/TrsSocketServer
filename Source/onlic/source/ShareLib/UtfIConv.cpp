
#include "ShareLib.h"

iconv_t IConv_open(char *toCode, char *fromCode)
{
	iconv_t convH;
	CError err;

#ifdef _WIN32
	convH = NULL;
#else
	if((convH = iconv_open(toCode, fromCode)) == (iconv_t)-1){
		err.err_sys("iconv_open error");
	}
#endif
	return convH;
}

size_t IConv(iconv_t cd, const char *inbuf, size_t inbytesleft, char *outbuf, size_t outbytesleft)
{
	size_t nRet;
	CError err;

#ifdef _WIN32
	nRet = UTF8_Encode(inbuf, outbuf, outbytesleft);

#elif _LINUX
	if((nRet = iconv(cd, (char**)&inbuf, &inbytesleft, &outbuf, &outbytesleft)) < 0){
		strcpy(outbuf, "");
		err.err_sys("iconv error");
	}
#else
//printf("cd:%d, inbuf:%s, inlen:%d, outbuf:%s, outlen:%d", cd, inbuf, inbytesleft, outbuf, outbytesleft);
	if((nRet = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft)) < 0){
//		printf(" ------- fail\n");
		strcpy(outbuf, "");
		err.err_sys("iconv error");
	}
//		printf(" ------- succ outbuf:%s, outlen:%d\n", outbuf, outbytesleft);
#endif

	return nRet;
}
/*
int IConvctl(iconv_t cd , int request, void *argument)
{
	int nRet;
	CError err;

#ifdef _WIN32
	nRet = 0;
#else
	if((nRet = iconvctl(cd, request, argument)) < 0){
		err.err_sys("iconvctl error");
	}
#endif

	return nRet;
}
*/

int IConv_close(iconv_t cd)
{
	int nRet;
	CError err;

#ifdef _WIN32
	nRet = 0;
#else
	if((nRet = iconv_close(cd)) < 0){
		err.err_sys("iconv_close error");
	}
#endif

	return nRet;
}

