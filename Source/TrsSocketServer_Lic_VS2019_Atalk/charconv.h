#ifndef ___CHAR_CONV_H___
#define ___CHAR_CONV_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif


WCHAR *utf8ToUnicode(const char *zFilename);
char *unicodeToUtf8(const WCHAR *zWideFilename);
WCHAR *mbcsToUnicode(const char *zFilename);
char *unicodeToMbcs(const WCHAR *zWideFilename);
char *mbcsToUtf8(const char *zFilename);
char *utf8ToMbcs(const char *zFilename);

#endif