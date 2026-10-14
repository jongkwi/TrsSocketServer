
#include "ShareLib.h"

/* onlic (2026-10-14): differs from onmw - vsnprintf bounded to the caller buffer instead of unbounded vsprintf.
   Callers pass (sizeof(buf)-1) as size, so the real buffer is size+1 bytes (= size+2 after --size below).
   onmw checked the length only after writing (silent stack/heap overflow; err_quit prints but does not exit). */

int nprintf(char *buf, size_t size, const char *fmt, ...)
{
	CError		err;
	int			n;
	va_list		ap;

	--size;
	va_start(ap, fmt);
	vsnprintf(buf, size + 2, fmt, ap);	/* bounded: truncates instead of overflowing */
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

