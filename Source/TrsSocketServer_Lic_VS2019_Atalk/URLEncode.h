/*****************************************************************************
Module :     URLEncode.H
Notices:     Written 2002 by ChandraSekar Vuppalapati
Description: H URL Encoder
*****************************************************************************/
#ifndef __CURLENCODE_H_
#define __CURLENCODE_H_

#include "stdafx.h"

class CURLEncode
{
private:
	static CString csUnsafeString;
	CString decToHex(char num, int radix);
	bool isUnsafe(char compareChar);
	CString convert(char val);
	static int convertToDec(const char* hex);
	static void getAsDec(char* hex);

public:
	CURLEncode() { };
	virtual ~CURLEncode() { };
	CString URLEncode(CString vData);
	CString URLDecode(CString str);

	CString Utf8_UrlEncode(CString strData);
	CString Utf8_UrlDecode(CString strData);
	CString Utf8_Encode(CString strData);
};


#endif //__CURLENCODE_H_