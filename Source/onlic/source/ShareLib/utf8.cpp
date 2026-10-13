// utf8.cpp
//

#include <shareheader.h>
#include "utf8.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//

int utf8_encode_calcsize(unsigned short * i)
{
    int n = 0;
#ifdef _WIN32
    while( *i )
    {
        n += utf8_encode_calc_long(*i);
		i += 1;
    }
#endif
    return n;
}

int utf8_encode_calc_long(unsigned long c)
{
#ifdef _WIN32
    if( c < 0x80 )
        return 1;

    if( c < 0x800 )
        return 2;

    if( c < 0x10000L )
        return 3;

    if( c < 0x200000L )
        return 4;

    if( c < 0x4000000L )
        return 5;
#endif
    return 6;
}


int utf8_encode(unsigned short * in, unsigned char * out)
{
    int n = 0, m;

#ifdef _WIN32
    while( *in )
    {
        m = utf8_encode(*in, out);
        out += m;
        n   += m;
        in  += 1;
    }

    *out = NULL;
#endif
    return n;
}

#pragma warning(disable:4244)

int utf8_encode(unsigned long c, unsigned char * out)
{
#ifdef _WIN32
    if( c < 0x80 )
    {
        out[0] = c;

        return 1;
    }

    if( c < 0x800 )
    {
        out[0] = (0xC0 | (c>>6 ) & 0x1F);
        out[1] = (0x80 | (c    ) & 0x3F);

        return 2;
    }

    if( c < 0x10000L )
    {
        out[0] = (0xE0 | (c>>12) & 0xF );
        out[1] = (0x80 | (c>>6 ) & 0x3F);
        out[2] = (0x80 | (c    ) & 0x3F);

        return 3;
    }

    if( c < 0x200000L )
    {
        out[0] = (0xF0 | (c>>18) & 0x7 );
        out[1] = (0x80 | (c>>12) & 0x3F);
        out[2] = (0x80 | (c>>6 ) & 0x3F);
        out[3] = (0x80 | (c    ) & 0x3F);

        return 4;
    }

    if( c < 0x4000000L )
    {
        out[0] = (0xF8 | (c>>24) & 0x3 );
        out[1] = (0x80 | (c>>18) & 0x3F);
        out[2] = (0x80 | (c>>12) & 0x3F);
        out[3] = (0x80 | (c>>6 ) & 0x3F);
        out[4] = (0x80 | (c    ) & 0x3F);

        return 5;
    }

    // else
    {
        out[0] = (0xFC | (c>>30) & 0x1 );
        out[1] = (0x80 | (c>>24) & 0x3F);
        out[2] = (0x80 | (c>>18) & 0x3F);
        out[3] = (0x80 | (c>>12) & 0x3F);
        out[4] = (0x80 | (c>>6 ) & 0x3F);
        out[5] = (0x80 | (c    ) & 0x3F);

        return 6;
    }
#endif

	return 0;
}

#pragma warning(default:4244)

int utf8_decode_calcsize(unsigned char * in)
{
    int i = 0;
    int n = 0;

#ifdef _WIN32
    for( i = 0; in[i] != NULL; i++ )
    {
        if( (in[i] & 0x80) == 0x00 )
        {
            if( in[i] == '%' )
                n += 3;
            else
                n += 1;
        }
        else if( (in[i] & 0xE0) == 0xC0 )
            n += 2;
        else if( (in[i] & 0xF0) == 0xE0 )
            n += 3;
        else if( (in[i] & 0xF8) == 0xF0 )
            n += 4;
        else if( (in[i] & 0xFC) == 0xF8 )
            n += 5;
        else if( (in[i] & 0xFE) == 0xFC )
            n += 6;
//      else
//          return 0;
        else
            n += 1;
    }
#endif
    return n;
}

int utf8_decode(unsigned char * in, unsigned short * out)
{
	int i, j;
#ifdef _WIN32
    for( i = 0, j = 0; in[i] != NULL; j++ )
    {
        if( (in[i] & 0x80) == 0x00 )
        {
            if( in[i] == '%' )
            {
                // ESCAPE
                out[j] = 0x20;
                i += 3;
            }
            else
            {
                // ASCII
                out[j] =    (in[i] & 0x7F);
                i++;
            }
        }
        else if( (in[i] & 0xE0) == 0xC0 )
        {
            out[j] =    ((in[i] & 0x1F) << 6 ) |
                        ((in[i+1] & 0x3F)      );
            i += 2;
        }
        else if( (in[i] & 0xF0) == 0xE0 )
        {
            out[j] =    ((in[i] & 0x1F) << 12) |
                        ((in[i+1] & 0x3F) << 6 ) |
                        ((in[i+2] & 0x3F)      );
            i += 3;
        }
        else if( (in[i] & 0xF8) == 0xF0 )
        {
            out[j] =    ((in[i] & 0x1F) << 18) |
                        ((in[i+1] & 0x3F) << 12) |
                        ((in[i+2] & 0x3F) << 6 ) |
                        ((in[i+3] & 0x3F)      );
            i += 4;
        }
        else if( (in[i] & 0xFC) == 0xF8 )
        {
            out[j] =    ((in[i] & 0x1F) << 24) |
                        ((in[i+1] & 0x3F) << 18) |
                        ((in[i+2] & 0x3F) << 12) |
                        ((in[i+3] & 0x3F) << 6 ) |
                        ((in[i+4] & 0x3F)     );
            i += 5;
        }
        else if( (in[i] & 0xFE) == 0xFC )
        {
            out[j] =    ((in[i] & 0x1F) << 30) |
                        ((in[i+1] & 0x3F) << 24) |
                        ((in[i+2] & 0x3F) << 18) |
                        ((in[i+3] & 0x3F) << 12) |
                        ((in[i+4] & 0x3F) << 6 ) |
                        ((in[i+5] & 0x3F)      );
            i += 6;
        }
        else
        {
            // ERROR!!!
            // just use bad character
            out[j] = in[i++];
        }
    }

    //
    out[j] = NULL;
#endif

    return j;
}

/////////////////////////////////////////////////////////////////////////////
//

int UTF8_Encode(LPCSTR lpszIn, LPSTR lpszOut, int nBufLen)
{
    int     nInBuf, nOutLen = 0;

#ifdef _WIN32
    LPWSTR  lpwzIn;
    //
    nInBuf = MultiByteToWideChar(CP_ACP, 0, lpszIn, -1, NULL, 0);
    nOutLen = 0;

    if( nInBuf > 0 )
    {
        if( (lpwzIn = new WCHAR[nInBuf + 1]) != NULL )
        {			
            MultiByteToWideChar(CP_ACP, 0, lpszIn, -1, lpwzIn, nInBuf + 1);

            if( (nOutLen = utf8_encode_calcsize((unsigned short*)lpwzIn)) <= nBufLen )
            {
                if( lpszOut )
                    utf8_encode((unsigned short*)lpwzIn, (unsigned char *)lpszOut);
            }

            delete[] lpwzIn;
			lpwzIn = NULL;
        }
    }
#endif

    return nOutLen;
}

int UTF8_Decode(LPCSTR lpszIn, LPSTR lpszOut, int nBufLen)
{
    int     nOutLen = 0;
#ifdef _WIN32
    LPWSTR  lpwzOut;

    //
    if( (nOutLen = utf8_decode_calcsize((unsigned char *)lpszIn)) > 0 )
    {
        if( (lpwzOut = new WCHAR[nOutLen + 1]) != NULL )
        {
			utf8_decode((unsigned char *)lpszIn, (unsigned short*)lpwzOut);
            if( (nOutLen = WideCharToMultiByte(CP_ACP, 0, lpwzOut, -1, NULL, 0, NULL, NULL)) <= nBufLen && nOutLen > 0 )
            {
                if( lpszOut )
                    WideCharToMultiByte(CP_ACP, 0, lpwzOut, -1, lpszOut, nBufLen, NULL, NULL);
            }

            delete[] lpwzOut;
			lpwzOut = NULL;
        }
    }
#endif

    return nOutLen;
}

