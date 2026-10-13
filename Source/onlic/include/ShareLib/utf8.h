#ifndef ___UTF8_H___
#define ___UTF8_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

int utf8_encode_calcsize(unsigned short * in);
int utf8_encode_calc_long(unsigned long c);
int utf8_encode(unsigned short * in, unsigned char * out);
int utf8_encode(unsigned long c, unsigned char * out);
int utf8_decode_calcsize(unsigned char * in);
int utf8_decode(unsigned char * in, unsigned short * out);

int UTF8_Encode(LPCSTR lpszIn, LPSTR lpszOut, int nBufLen);
int UTF8_Decode(LPCSTR lpszIn, LPSTR lpszOut, int nBufLen);

#endif//utf8.h


