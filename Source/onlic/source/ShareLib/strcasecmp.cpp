/******************************************************************************
Copyright (C) 2007-12-11

This program is free software;

Make Kongjin
******************************************************************************/

#include "ShareLib.h"

int util_strncasecmp(const char *a, const char *b, int len)
{
    int i; 

    for (i=0; i< len; i++) {
        if ( toupper(a[i]) != toupper(b[i]) )
            return 1; 
    }
    return 0; 
}

#ifdef WIN32
int strcasecmp(const char *s1, const char *s2)
{
    return util_strncasecmp( s1, s2, strlen_st(s1)); 
}
#endif

void trim(char *str)
{
    char *sp, *head, *tail;
    if(str == NULL) return;

    sp = str;
    head = tail = NULL;

    while(*sp){
        if(*sp != ' '){
            tail = sp;
            if(head == NULL)
                head = sp;
		}
        ++sp;
	}
    if(tail){
        *(tail + 1) = '\0';
        while(*head) *str++ = *head++;
	}
    *str = '\0';
}


