/*
**	md5.cpp
*/
#include "ShareLib.h"


/* /////////////////////////////////////////////////////////////////////// */
/* constants for MD5Transform routine. */
static unsigned char PADDING[ 64 ] =
{	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
} ;

/* macro functions. */
#define F(x, y, z)	(((x) & (y)) | ((~x) & (z)))
#define G(x, y, z)	(((x) & (z)) | ((y) & (~z)))
#define H(x, y, z)	((x) ^ (y) ^ (z))
#define I(x, y, z)	((y) ^ ((x) | (~z)))

/*	rotates x left n bits. */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/*	FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4. */
#define FF(a, b, c, d, x, s, ac) \
{	(a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) \
{	(a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) \
{	(a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}
#define II(a, b, c, d, x, s, ac) \
{	(a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}

/* /////////////////////////////////////////////////////////////////////// */
/*	MD5 context. */
void md5Init( MD5_CTX *context )
{
	context->count[0] = context->count[1] = 0;

	/* Load magic initialization constants. */
	context->state[0] = 0x67452301 ;
	context->state[1] = 0xefcdab89 ;
	context->state[2] = 0x98badcfe ;
	context->state[3] = 0x10325476 ;
}

/* MD5 basic transformation. Transforms state based on block. */
static void md5Transform( UINT4 state[4], unsigned char block[64] )
{	UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];
	unsigned int i = 0, j = 0;

	/* init x */
	while( j < 64 )
	{    x[i++] = ((UINT4)block[j])|(((UINT4)block[j+1]) << 8)|(((UINT4)block[j+2]) << 16)|(((UINT4)block[j+3]) << 24) ;
	     j += 4 ;
	}
	/* Round 1 */
	FF (a, b, c, d, x[ 0], S11, 0xd76aa478) ;	/*  1 */
	FF (d, a, b, c, x[ 1], S12, 0xe8c7b756) ;	/*  2 */
	FF (c, d, a, b, x[ 2], S13, 0x242070db) ;	/*  3 */
	FF (b, c, d, a, x[ 3], S14, 0xc1bdceee) ;	/*  4 */
	FF (a, b, c, d, x[ 4], S11, 0xf57c0faf) ;	/*  5 */
	FF (d, a, b, c, x[ 5], S12, 0x4787c62a) ;	/*  6 */
	FF (c, d, a, b, x[ 6], S13, 0xa8304613) ;	/*  7 */
	FF (b, c, d, a, x[ 7], S14, 0xfd469501) ;	/*  8 */
	FF (a, b, c, d, x[ 8], S11, 0x698098d8) ;	/*  9 */
	FF (d, a, b, c, x[ 9], S12, 0x8b44f7af) ;	/* 10 */
	FF (c, d, a, b, x[10], S13, 0xffff5bb1) ;	/* 11 */
	FF (b, c, d, a, x[11], S14, 0x895cd7be) ;	/* 12 */
	FF (a, b, c, d, x[12], S11, 0x6b901122) ;	/* 13 */
	FF (d, a, b, c, x[13], S12, 0xfd987193) ;	/* 14 */
	FF (c, d, a, b, x[14], S13, 0xa679438e) ;	/* 15 */
	FF (b, c, d, a, x[15], S14, 0x49b40821) ;	/* 16 */
	/* Round 2 */
	GG (a, b, c, d, x[ 1], S21, 0xf61e2562) ;	/* 17 */
	GG (d, a, b, c, x[ 6], S22, 0xc040b340) ;	/* 18 */
	GG (c, d, a, b, x[11], S23, 0x265e5a51) ;	/* 19 */
	GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa) ;	/* 20 */
	GG (a, b, c, d, x[ 5], S21, 0xd62f105d) ;	/* 21 */
	GG (d, a, b, c, x[10], S22, 0x2441453 ) ;	/* 22 */
	GG (c, d, a, b, x[15], S23, 0xd8a1e681) ;	/* 23 */
	GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8) ;	/* 24 */
	GG (a, b, c, d, x[ 9], S21, 0x21e1cde6) ;	/* 25 */
	GG (d, a, b, c, x[14], S22, 0xc33707d6) ;	/* 26 */
	GG (c, d, a, b, x[ 3], S23, 0xf4d50d87) ;	/* 27 */
	GG (b, c, d, a, x[ 8], S24, 0x455a14ed) ;	/* 28 */
	GG (a, b, c, d, x[13], S21, 0xa9e3e905) ;	/* 29 */
	GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8) ;	/* 30 */
	GG (c, d, a, b, x[ 7], S23, 0x676f02d9) ;	/* 31 */
	GG (b, c, d, a, x[12], S24, 0x8d2a4c8a) ;	/* 32 */
	/* Round 3 */
	HH (a, b, c, d, x[ 5], S31, 0xfffa3942) ;	/* 33 */
	HH (d, a, b, c, x[ 8], S32, 0x8771f681) ;	/* 34 */
	HH (c, d, a, b, x[11], S33, 0x6d9d6122) ;	/* 35 */
	HH (b, c, d, a, x[14], S34, 0xfde5380c) ;	/* 36 */
	HH (a, b, c, d, x[ 1], S31, 0xa4beea44) ;	/* 37 */
	HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9) ;	/* 38 */
	HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60) ;	/* 39 */
	HH (b, c, d, a, x[10], S34, 0xbebfbc70) ;	/* 40 */
	HH (a, b, c, d, x[13], S31, 0x289b7ec6) ;	/* 41 */
	HH (d, a, b, c, x[ 0], S32, 0xeaa127fa) ;	/* 42 */
	HH (c, d, a, b, x[ 3], S33, 0xd4ef3085) ;	/* 43 */
	HH (b, c, d, a, x[ 6], S34, 0x4881d05 ) ;	/* 44 */
	HH (a, b, c, d, x[ 9], S31, 0xd9d4d039) ;	/* 45 */
	HH (d, a, b, c, x[12], S32, 0xe6db99e5) ;	/* 46 */
	HH (c, d, a, b, x[15], S33, 0x1fa27cf8) ;	/* 47 */
	HH (b, c, d, a, x[ 2], S34, 0xc4ac5665) ;	/* 48 */
	/* Round 4 */
	II (a, b, c, d, x[ 0], S41, 0xf4292244) ;	/* 49 */
	II (d, a, b, c, x[ 7], S42, 0x432aff97) ;	/* 50 */
	II (c, d, a, b, x[14], S43, 0xab9423a7) ;	/* 51 */
	II (b, c, d, a, x[ 5], S44, 0xfc93a039) ;	/* 52 */
	II (a, b, c, d, x[12], S41, 0x655b59c3) ;	/* 53 */
	II (d, a, b, c, x[ 3], S42, 0x8f0ccc92) ;	/* 54 */
	II (c, d, a, b, x[10], S43, 0xffeff47d) ;	/* 55 */
	II (b, c, d, a, x[ 1], S44, 0x85845dd1) ;	/* 56 */
	II (a, b, c, d, x[ 8], S41, 0x6fa87e4f) ;	/* 57 */
	II (d, a, b, c, x[15], S42, 0xfe2ce6e0) ;	/* 58 */
	II (c, d, a, b, x[ 6], S43, 0xa3014314) ;	/* 59 */
	II (b, c, d, a, x[13], S44, 0x4e0811a1) ;	/* 60 */
	II (a, b, c, d, x[ 4], S41, 0xf7537e82) ;	/* 61 */
	II (d, a, b, c, x[11], S42, 0xbd3af235) ;	/* 62 */
	II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb) ;	/* 63 */
	II (b, c, d, a, x[ 9], S44, 0xeb86d391) ;	/* 64 */

	state[0] += a ;	state[1] += b ;	state[2] += c ;	state[3] += d ;
}

void md5Update( MD5_CTX *context, char *input, unsigned int length )
{	unsigned int i, index, partLen;

	/* Compute number of bytes mod 64 */
	index = (unsigned int)((context->count[0] >> 3) & 0x3F);

	/* Update number of bits */
	if   ( (context->count[0] += ((UINT4)length << 3)) < ((UINT4)length << 3) )
	     ++context->count[1];
	context->count[1] += ((UINT4)length >> 29);
	/* Transform as many times as possible. */
	if   ( (partLen = 64 - index) <= length )
	{    memcpy( &context->buffer[index], input, partLen ) ;
	     md5Transform( (UINT4 *)context->state, context->buffer ) ;
	     for( i = partLen; i + 63 < length; i += 64 )
	          md5Transform( (UINT4 *)context->state, (unsigned char *)&input[i] ) ;
	     index = 0;
	}
	else i = 0;

	/* Buffer remaining input */
	memcpy( &context->buffer[index], &input[i], length - i ) ;
}

void md5Final( unsigned char digest[16], MD5_CTX *context )
{	unsigned char bits[8] ;
	unsigned int i, indx ;

	/* save number of bits */
	for( i = 0, indx = 0 ; i < 8 ; indx++ )
	{    bits[i++] = (unsigned char)( context->count[indx] & 0xFF ) ;
	     bits[i++] = (unsigned char)((context->count[indx] >>  8) & 0xFF) ;
	     bits[i++] = (unsigned char)((context->count[indx] >> 16) & 0xFF) ;
	     bits[i++] = (unsigned char)((context->count[indx] >> 24) & 0xFF) ;
	}

	/* pad out to 56 mod 64. */
	indx = (unsigned int)((context->count[0] >> 3) & 0x3f);
	md5Update( context, (char *)PADDING, (indx < 56) ? (56 - indx) : (120 - indx) ) ;
	/* append length (before padding) */
	md5Update( context, (char *)bits, 8 ) ;

	/* store state in digest */
	for( i = 0, indx = 0 ; i < 16 ; indx++ )
	{    digest[i++] = (unsigned char)( context->state[indx] & 0xFF ) ;
	     digest[i++] = (unsigned char)((context->state[indx] >>  8) & 0xFF) ;
	     digest[i++] = (unsigned char)((context->state[indx] >> 16) & 0xFF) ;
	     digest[i++] = (unsigned char)((context->state[indx] >> 24) & 0xFF) ;
	}
	memset_st( context, '\0', sizeof(MD5_CTX) ) ;
}

/* /////////////////////////////////////////////////////////////////////// */
static void cvtHex( HASH bin, HASHHEX hex )
{	unsigned short i ;
	unsigned char c ;

	for( i = 0 ; i < HASHLEN ; i++ )
	{    c = (bin[ i ] >> 4) & 0x0F ;
	     hex[ i*2 ]   = c + (c <= 9 ? '0' : 'a' - 10) ;
	     c = bin[ i ] & 0x0F ;
	     hex[ i*2+1 ] = c + (c <= 9 ? '0' : 'a' - 10) ;
	}
	hex[ HASHHEXLEN ] = '\0';
}

void digestCalcPasswd( char *pszPassword, HASHHEX hashhex )
{	
	MD5_CTX md5Ctx;
	HASH ha1;
	char szB[51] = "";
	
	md5Init( &md5Ctx ) ;
	if   ( pszPassword )
	     md5Update( &md5Ctx, pszPassword, strlen_st(pszPassword) ) ;
	else md5Update( &md5Ctx, szB, 0 ) ;
	md5Final( (unsigned char *)ha1, &md5Ctx ) ;
	cvtHex( ha1, hashhex ) ;
}

void digestCalcPer( char *pszPer, int len, HASH hash )
{
	MD5_CTX md5Ctx;
	char szB[51] = "";
	
	md5Init( &md5Ctx ) ;
	if   ( len )
	     md5Update( &md5Ctx, pszPer, len ) ;
	else md5Update( &md5Ctx, szB, 0 ) ;
	md5Final( (unsigned char *)hash, &md5Ctx ) ;
}

void digestCalcHA1( MD5Algorithm algorithm, char *pszUserName, char *pszPassword,
				char *pszRealm, char *pszNonce, char *pszCNonce, HASHHEX sessionKey )
{	
	MD5_CTX md5Ctx;
	HASH ha1;
	char szB[51] = "";
	char szC[2];
	strcpy_st(szC, ":");
	
	md5Init( &md5Ctx ) ;
	if   ( pszUserName )
	     md5Update( &md5Ctx, pszUserName, strlen_st(pszUserName) ) ;
	else md5Update( &md5Ctx, szB, 0 ) ;
	md5Update( &md5Ctx, szC, 1 ) ;
	if   ( pszRealm )
	     md5Update( &md5Ctx, pszRealm, strlen_st(pszRealm) ) ;
	else md5Update( &md5Ctx, szB, 0 ) ;
	md5Update( &md5Ctx, szC, 1 ) ;
	if   ( pszPassword )
	     md5Update( &md5Ctx, pszPassword, strlen_st(pszPassword) ) ;
	else md5Update( &md5Ctx, szB, 0 ) ;
	md5Final( (unsigned char *)ha1, &md5Ctx ) ;

	if   ( algorithm == md5_sess )
	{    md5Init( &md5Ctx ) ;
	     md5Update( &md5Ctx, ha1, HASHLEN ) ;
	     md5Update( &md5Ctx, szC, 1 ) ;
	     if   ( pszNonce )
	          md5Update( &md5Ctx, pszNonce, strlen_st(pszNonce) ) ;
	     else md5Update( &md5Ctx, szB, 0 ) ;
	     md5Update( &md5Ctx, szC, 1 ) ;
	     if   ( pszCNonce )
	          md5Update( &md5Ctx, pszCNonce, strlen_st(pszCNonce) ) ;
	     else md5Update( &md5Ctx, szB, 0 ) ;
	     md5Final( (unsigned char *)ha1, &md5Ctx ) ;
	}
	cvtHex( ha1, sessionKey ) ;
}

/* calculate request-digest/response-digest as per HTTP Digest spec */
void digestCalcResponse(
	HASHHEX	ha1,			/* H(A1) */
	char *	pszNonce,		/* nonce from server */
	char *	pszNonceCount,	/* 8 hex digits */
	char *	pszCNonce,		/* client nonce */
	char *	pszQop,			/* qop-value: "", "auth", "auth-int" */
	char *	pszMethod,		/* method from the request */
	char *	pszDigestUri,	/* requested URL */
	HASHHEX	hEntity,		/* H(entity body) if qop="auth-int" */
	HASHHEX	response		/* request-digest or response-digest */)
{
	MD5_CTX	md5Ctx;
	HASH ha2, respHash;
	HASHHEX	ha2Hex;
	char szB[51] = "";
	char szC[2];
	strcpy_st(szC, ":");
	
	/* calculate H(A2) */
	md5Init( &md5Ctx ) ;
	if   ( pszMethod )
	     md5Update( &md5Ctx, pszMethod, strlen_st(pszMethod) ) ;
	else md5Update( &md5Ctx, szB, 0 ) ;
	md5Update( &md5Ctx, szC, 1 ) ;
	if   ( pszDigestUri )
	     md5Update( &md5Ctx, pszDigestUri, strlen_st(pszDigestUri) ) ;
	else md5Update( &md5Ctx, szB, 0 ) ;
	if   ( pszQop && ! strcmp( pszQop, "auth-int" ) )
	{    md5Update( &md5Ctx, szC, 1 ) ;
	     md5Update( &md5Ctx, hEntity, HASHHEXLEN ) ;
	}
	md5Final( (unsigned char *)ha2, &md5Ctx ) ;
	cvtHex( ha2, ha2Hex ) ;
	
	/* calculate response */
	md5Init( &md5Ctx ) ;
	md5Update( &md5Ctx, ha1, HASHHEXLEN ) ;
	md5Update( &md5Ctx, szC, 1 ) ;
	if   ( pszNonce )
	     md5Update( &md5Ctx, pszNonce, strlen_st(pszNonce) ) ;
	else md5Update( &md5Ctx, szB, 0 ) ;
	md5Update( &md5Ctx, szC, 1 ) ;
	if   ( pszQop && *pszQop )
	{    if   ( pszNonceCount )
	          md5Update( &md5Ctx, pszNonceCount, strlen_st(pszNonceCount) ) ;
	     else md5Update( &md5Ctx, szB, 0 ) ;
	     md5Update( &md5Ctx, szC, 1 ) ;
	     if   ( pszCNonce )
	          md5Update( &md5Ctx, pszCNonce, strlen_st(pszCNonce) ) ;
	     else md5Update( &md5Ctx, szB, 0 ) ;
	     md5Update( &md5Ctx, szC, 1 ) ;
	     if   ( pszQop )
	          md5Update( &md5Ctx, pszQop, strlen_st(pszQop) ) ;
	     else md5Update( &md5Ctx, szB, 0 ) ;
	     md5Update( &md5Ctx, szC, 1 ) ;
	}
	md5Update( &md5Ctx, ha2Hex, HASHHEXLEN ) ;
	md5Final( (unsigned char *)respHash, &md5Ctx ) ;
	cvtHex( respHash, response ) ;
}

