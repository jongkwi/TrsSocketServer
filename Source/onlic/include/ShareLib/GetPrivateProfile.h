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

// this functions emulates the windows ini file reading
// retriving values from a text (initialization) file:
//
// [group1]
// key1=value1
// key2=value2
// .
// .
// [group2]
// .
// .
//
// characters after ';' are ignored (comments)
// key and app strings are case insensitive
// spaces between [] and before = should be avoided
#ifndef ___GETPRIVATEPROFILE_H___
#define ___GETPRIVATEPROFILE_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

// return: the lenght of found string 
long GetPrivateProfileStringUsr(char* lpAppName,		// the group name
                             char* lpKeyName,           // the key name
                             const char* lpDefault,           // default value
                             char* lpReturnedString,    // the string where return value
                             long nSize,                // max size of returned string
                             char* lpFileName           // ini file path
                             );
	             
// return: the found value  (unsigned integer)                 
unsigned int GetPrivateProfileIntUsr(char* lpAppName,      // the group name
                                  char* lpKeyName,      // the key name
                                  int nDefault,         // default value
                                  char* lpFileName      // ini file path
                                  );


#endif // !defined(GETPRIVATEPROFILE_H_INCLUDED)

