/*
**	md5.h : md5 encoding functions
*/

#ifndef  ___MD5_AUTHENTICATION_TOOLS_H___
#define  ___MD5_AUTHENTICATION_TOOLS_H___


/* /////////////////////////////////////////////////////////////////////// */
/*	global definition */
typedef unsigned short	UINT2 ;		/* 2 byte word */

#if defined(__OS64) || defined(_64BIT)
typedef uint32_t UINT4;
#else
typedef unsigned long	UINT4 ;		/* a four byte word */
#endif

/*	constant */
#define	HASHLEN			16
#define	HASHHEXLEN		32

typedef char			HASH[ HASHLEN ] ;
typedef char			HASHHEX[ HASHHEXLEN+1 ] ;

/*	transformation constant */
#define	S11				 7
#define	S12				12
#define	S13				17
#define	S14				22
#define	S21				 5
#define	S22				 9
#define	S23				14
#define	S24				20
#define	S31				 4
#define	S32				11
#define	S33				16
#define	S34				23
#define	S41				 6
#define	S42				10
#define	S43				15
#define	S44				21


/* /////////////////////////////////////////////////////////////////////// */
/*	MD5 context. */
typedef struct
{	UINT4				state[4] ;		/* state (ABCD) */
	UINT4				count[2] ;		/* number of bits, modulo 2^64 (lsb first) */
	unsigned char		buffer[64] ;	/* input buffer */
} MD5_CTX;

void md5Init( MD5_CTX *context ) ;
void md5Update( MD5_CTX *context, char *input, unsigned int length ) ;
void md5Final( unsigned char digest[16], MD5_CTX *context ) ;


/* /////////////////////////////////////////////////////////////////////// */
/* encoding api */
typedef enum { md5, md5_sess }	MD5Algorithm ;

void digestCalcPasswd( char *pszPassword, HASHHEX hashhex ) ;
void digestCalcPer( char *pszToken, int len, HASH hash ) ;

/* calculate H(A1) as per HTTP Digest spec */
void digestCalcHA1(
		MD5Algorithm	algorithm,
		char *			pszUserName,	/* user name */
		char *			pszPassword,	/* password */
		char *			pszRealm,		/* realm */
		char *			pszNonce,		/* nonce */
		char *			pszCNonce,		/* cnonce */
		HASHHEX			sessionKey ) ;

/* calculate request-digest/response-digest as per HTTP Digest spec */
void digestCalcResponse(
		HASHHEX			ha1,			/* H(A1) */
		char *			pszNonce,		/* nonce from server */
		char *			pszNonceCount,	/* 8 hex digits */
		char *			pszCNonce,		/* client nonce */
		char *			pszQop,			/* qop-value: "", "auth", "auth-int" */
		char *			pszMethod,		/* method from the request */
		char *			pszDigestUri,	/* requested URL */
		HASHHEX			hEntity,		/* H(entity body) if qop="auth-int" */
		HASHHEX			response		/* request-digest or response-digest */ ) ;


#endif	/* __MD5_AUTHENTICATION_TOOLS_H__ */
