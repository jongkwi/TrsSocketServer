#include <stdio.h> 
#include <string.h> 
#include "url_en_de.h"
 
/* 
 * urldecode 
 * 입력된 source를 urldecode해서 dest에 복사한다. 
 * 
 */ 
int urldecode(unsigned char *source, unsigned char *dest) 
{ 
    int num=0, i, index=0; 
    int retval=0; 
    while(*source) 
    { 
        if (*source == '%') 
        { 
            num = 0; 
            retval = 0; 
            for (i = 0; i < 2; i++) 
            { 
                *source++; 
                if (*(source) < ':') 
                { 
                    num = *(source) - 48; 
                } 
                else if (*(source) > '@' && *(source) < '[') 
                { 
                    num = (*(source) - 'A')+10; 
                } 
                else 
                { 
                    num = (*(source) - 'a')+10; 
                } 
 
                if ((16*(1-i)))  
                    num = (num*16); 
                retval += num; 
            } 
            dest[index] = retval; 
            index++; 
        } 
        else 
        { 
            dest[index] = *source; 
            index++; 
        } 
        *source++; 
    } 
    return index; 
} 
 
int urlencode(unsigned char *source, unsigned char *dest) 
{ 
    unsigned char hex[4]; 
    unsigned char *sbuf; 
    int size = 0; 
    sbuf = dest; 
    while(*source) 
    { 
        if ((*source > 47 && *source < 58) ||  
            (*source > 64 && *source < 91) || 
            (*source > 96 && *source < 123) || 
            *source == '-' || *source == '.' || *source == '_') 
        { 
            *sbuf = *source;  
        } 
        else 
        { 
            sprintf_st((char*)hex, "%%%02X", *source); 
            strncat_st((char*)sbuf, (const char *)hex, 3); 
            *sbuf++; 
            *sbuf++; 
            size += 2; 
        } 
        *source++; 
        *sbuf++; 
        size++; 
    } 
    return size; 
} 
