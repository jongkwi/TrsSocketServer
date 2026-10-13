/******************************************************************************
Copyright (C) 2007-12-11

This program is free software;

Make Kongjin
******************************************************************************/

#ifndef ___STRCASECMP_H___
#define ___STRCASECMP_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

int util_strncasecmp(const char *a, const char *b, int len);
#ifdef WIN32
int strcasecmp(const char *s1, const char *s2);
#endif

void trim(char *str);

#endif // !defined(__STRCASECMP_H__)

