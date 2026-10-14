/******************************************************************************
Copyright (C) 2005 Matteo Lucarelli 

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
******************************************************************************/

#include "ShareLib.h"

/* onlic (2026-10-14): differs from onmw - values/defaults are passed to nprintf via "%s", never as the format string.
   onmw passed the raw ini value as the format: a value containing %s/%d ([ALARM_MSG] texts, passwords, URLs)
   made vsprintf read nonexistent varargs (garbage or SIGSEGV). */

long GetPrivateProfileStringUsr(char* lpAppName,char* lpKeyName, const char* lpDefault,char* lpReturnedString,long nSize,char* lpFileName)
{

	
#ifdef _WIN32
	return GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
#else
	FILE* fp;
	int rightApp=0;
	char line[1024];
	char *c1,*c2;
	*lpReturnedString=0;

	nprintf(lpReturnedString,nSize,"%s",lpDefault);
	
	fp=fopen(lpFileName,"r");
	if (!fp) return 0;
	
	while (fgets(line,1024,fp)){
		
		// truncate line after comment char (;)
		if (*line==';') continue;
		for (c1=line;c1<(line+strlen_st(line));c1++) if (*(c1)==';') *(c1)=0;
		
		// check for apps
		if ((*line=='[')&&(c2=strchr_st(line,']'))){
			c1=line+1;
			*c2=0;
			if (!strcasecmp(c1,lpAppName)) rightApp=1;
			else rightApp=0;
			continue;
		}
		
		// check for key
		if (rightApp==1){
			c1=line;
			if (c2=strchr_st(c1,'=')){
				*c2=0;
				if (!strcasecmp(c1,lpKeyName)){
					++c2;
					if (c2!=0){
						// exclude blanks and CR/LF at value end
						c1=c2+strlen_st(c2)-1;
						while ((*c1==' ')||(*c1=='\t')||(*c1=='\n')||(*c1=='\r')) c1--;
						*(c1+1)=0;
						// copy value and return
						nprintf(lpReturnedString,nSize,"%s",c2);
						break;
					}
				}
			}
		}
	}
	fclose(fp);
	return strlen_st(lpReturnedString);
#endif
}

unsigned int GetPrivateProfileIntUsr(char* lpAppName,char* lpKeyName,int nDefault,char* lpFileName)
{
	char val[1024];
	if (GetPrivateProfileStringUsr(lpAppName, lpKeyName, "", val, 1024, lpFileName)==0) return nDefault;
	else return atoi(val);
}


