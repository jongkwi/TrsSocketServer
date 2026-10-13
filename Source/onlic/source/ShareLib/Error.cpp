
#include "ShareLib.h"


/* Nonfatal error related to system call
 * Print message and return */

CError::CError()
{

}


CError::~CError()
{

}


void CError::err_ret(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

/* Fatal error related to system call
 * Print message and terminate */

void CError::err_sys(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
//	exit(1);
}

/* Fatal error related to system call
 * Print message, dump core, and terminate */

void CError::err_dump(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	abort();		/* dump core and terminate */
//	exit(1);		/* shouldn't get here */
}

/* Nonfatal error unrelated to system call
 * Print message and return */

void CError::err_msg(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(0, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}

/* Fatal error unrelated to system call
 * Print message and terminate */

void CError::err_quit(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
}

/* Print message and return to caller
 * Caller specifies "errnoflag" and "level" */

void CError::err_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
	int		errno_save, n;
	char	buf[2048] = "", tmpLog[2080] = "";

	errno_save = errno;		/* value caller might want printed */

	
	vsprintf_st(buf, fmt, ap);					/* not safe */

	n = strlen_st(buf);

	sprintf(tmpLog, "acromw: %s errmsg:%s\n", buf, strerror(errno));

	if (errnoflag){
#ifdef _UNIX
		syslog(LOG_ERR, tmpLog);
#else
		printf(tmpLog);
#endif
	}

	return;
}
