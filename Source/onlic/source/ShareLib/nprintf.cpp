
#include "ShareLib.h"

int nprintf(char *buf, size_t size, const char *fmt, ...)
{
	CError		err;
	int			n;
	va_list		ap;

	--size;
	va_start(ap, fmt);
	vsprintf_st(buf, fmt, ap);	/* Sigh, some vsprintf's return ptr, not length */
	n = strlen_st(buf);
	va_end(ap);
	if (n > (signed)size)
		err.err_quit("nprintf:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x, overflowed array",
				fmt[0], fmt[1], fmt[2], fmt[3], fmt[4], fmt[5], fmt[6], fmt[7], fmt[8], fmt[9]);
	return(n);
}

void flushprintf(const char *fmt, ...)
{
	printf(fmt);
	fflush(stdout);
}

