#ifndef ___URL_EN_DE_H___
#define ___URL_EN_DE_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

#include <ShareLib/ShareLib.h>

int urldecode(unsigned char *source, unsigned char *dest);
int urlencode(unsigned char *source, unsigned char *dest);

#endif			//  url_en_de.h
