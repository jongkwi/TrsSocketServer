/*****************************************************************************
Module :     URLEncode.cpp
Notices:     Written 2002 by ChandraSekar Vuppalapati
Description: CPP URL Encoder
*****************************************************************************/
#define _CRTDBG_MAP_ALLOC

#include "stdafx.h"
#include "URLEncode.h"
#include <math.h>
#include <malloc.h>
#include <memory.h>
#include <new.h>
#include <stdlib.h>
#include <string.h>
#include <WININET.H>
#include "charconv.h"



#define MAX_BUFFER_SIZE 4096
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#include <stdlib.h>
#include <crtdbg.h>
// HEX Values array
char hexVals[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
// UNSAFE String
CString CURLEncode::csUnsafeString= "\"<>%\\^[]`+$,@:;/!#?=&";

// PURPOSE OF THIS FUNCTION IS TO CONVERT A GIVEN CHAR TO URL HEX FORM
CString CURLEncode::convert(char val) 
{
	CString csRet;
	csRet += "%";
	csRet += decToHex(val, 16);	
	return  csRet;
}

// THIS IS A HELPER FUNCTION.
// PURPOSE OF THIS FUNCTION IS TO GENERATE A HEX REPRESENTATION OF GIVEN CHARACTER
CString CURLEncode::decToHex(char num, int radix)
{	
	int temp=0;	
	CString csTmp;
	int num_char;
	num_char = (int) num;
	
	// ISO-8859-1 
	// IF THE IF LOOP IS COMMENTED, THE CODE WILL FAIL TO GENERATE A 
	// PROPER URL ENCODE FOR THE CHARACTERS WHOSE RANGE IN 127-255(DECIMAL)
	if (num_char < 0)
		num_char = 256 + num_char;

	while (num_char >= radix)
    {
		temp = num_char % radix;
		num_char = (int)floor(float(num_char / radix));
		csTmp = hexVals[temp];
    }
	
	csTmp += hexVals[num_char];

	if(csTmp.GetLength() < 2)
	{
		csTmp += '0';
	}

	CString strdecToHex(csTmp);
	// Reverse the String
	strdecToHex.MakeReverse(); 
	
	return strdecToHex;
}

// PURPOSE OF THIS FUNCTION IS TO CHECK TO SEE IF A CHAR IS URL UNSAFE.
// TRUE = UNSAFE, FALSE = SAFE
bool CURLEncode::isUnsafe(char compareChar)
{
	bool bcharfound = false;
	char tmpsafeChar;
	int m_strLen = 0;
	
	m_strLen = csUnsafeString.GetLength();
	for(int ichar_pos = 0; ichar_pos < m_strLen ;ichar_pos++)
	{
		tmpsafeChar = csUnsafeString.GetAt(ichar_pos); 
		if(tmpsafeChar == compareChar)
		{ 
			bcharfound = true;
			break;
		} 
	}
	int char_ascii_value = 0;
	//char_ascii_value = __toascii(compareChar);
	char_ascii_value = (int) compareChar;

	if(bcharfound == false &&  char_ascii_value > 32 && char_ascii_value < 123)
	{
		return false;
	}
	// found no unsafe chars, return false		
	else
	{
		return true;
	}
	
	return true;
}
// PURPOSE OF THIS FUNCTION IS TO CONVERT A STRING 
// TO URL ENCODE FORM.
CString CURLEncode::URLEncode(CString pcsEncode)
{	
	int ichar_pos;
	CString csEncode;
	CString csEncoded;	
	int m_length;
//	int ascii_value;

	csEncode = pcsEncode;
	m_length = csEncode.GetLength();
	
	for(ichar_pos = 0; ichar_pos < m_length; ichar_pos++)
	{
		char ch = csEncode.GetAt(ichar_pos);
		if (ch < ' ') 
		{
			ch = ch;
		}		
		if(!isUnsafe(ch))
		{
			// Safe Character				
			csEncoded += CString(ch);
		}
		else
		{
			// get Hex Value of the Character
			csEncoded += convert(ch);
		}
	}
	return csEncoded;
}

CString CURLEncode::URLDecode(CString str) {
	int len = str.GetLength();
	char* buff = new char[len + 1];
	strcpy(buff,str);
	CString ret = "";
	for(int i=0;i<len;i++) {
		if(buff[i] == '+') {
			ret = ret + " ";
		}else if(buff[i] == '%') {
			char tmp[4];
			char hex[4];			
			hex[0] = buff[++i];
			hex[1] = buff[++i];
			hex[2] = '\0';		
			//int hex_i = atoi(hex);
			sprintf(tmp,"%c",convertToDec(hex));
			ret = ret + tmp;
		}else {
			ret = ret + buff[i];
		}
	}
	delete[] buff;
	return ret;
}

int CURLEncode::convertToDec(const char* hex) {
	char buff[12];
	sprintf(buff,"%s",hex);
	int ret = 0;
	int len = strlen(buff);
	for(int i=0;i<len;i++) {
		char tmp[4];
		tmp[0] = buff[i];
		tmp[1] = '\0';
		getAsDec(tmp);
		int tmp_i = atoi(tmp);
		int rs = 1;
		for(int j=i;j<(len-1);j++) {
			rs *= 16;
		}
		ret += (rs * tmp_i);
	}
	return ret;
}

void CURLEncode::getAsDec(char* hex) {
	char tmp = tolower(hex[0]);
	if(tmp == 'a') {
		strcpy(hex,"10");
	}else if(tmp == 'b') {
		strcpy(hex,"11");
	}else if(tmp == 'c') {
		strcpy(hex,"12");
	}else if(tmp == 'd') {
		strcpy(hex,"13");
	}else if(tmp == 'e') {
		strcpy(hex,"14");
	}else if(tmp == 'f') {
		strcpy(hex,"15");
	}else if(tmp == 'g') {
		strcpy(hex,"16");
	}
}


CString CURLEncode::Utf8_UrlEncode(CString strData) 
{
	CString strOut, strConv;
	if(strData.GetLength() > 0){
		char* pConv = mbcsToUtf8((LPCTSTR)strData);
		if(pConv != NULL){
			strConv.Format("%s", pConv);
			free(pConv);
			strOut = URLEncode(strConv);
		}
	}
	return strOut;
}


CString CURLEncode::Utf8_UrlDecode(CString strData) 
{
	CString strOut;
	if(strData.GetLength() > 0){
		CString strConv = URLDecode(strData);
		if(strConv.GetLength() >0){
			char* pConv = utf8ToMbcs((LPCTSTR)strConv);
			if(pConv != NULL){
				strOut.Format("%s", pConv);
				free(pConv);
			}
		}
	}
	return strOut;
}


CString CURLEncode::Utf8_Encode(CString strData) 
{
	CString strOut, strConv;
	if(strData.GetLength() > 0){
		char* pConv = mbcsToUtf8((LPCTSTR)strData);
		if(pConv != NULL){
			strConv.Format("%s", pConv);
			free(pConv);
			//strOut = URLEncode(strConv);
		}
	}
	return strConv;
}
