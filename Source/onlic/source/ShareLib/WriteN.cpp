
#include "ShareLib.h"

ssize_t						/* Read "n" bytes from a descriptor. */
writen(int fd, void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = (char *)vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = send(fd, ptr, nleft, 0)) < 0) {
			if (errno == EINTR)
				nread = 0;		/* and call read() again */
			else
				return(-1);
		} else if (nread == 0)
			break;				/* EOF */

		nleft -= nread;
		ptr   += nread;
	}
	return(n - nleft);		/* return >= 0 */
}
/* end receiven */

ssize_t
WriteN(int fd, char *ptr, size_t nbytes)
{
	CError err;
	ssize_t		n;

	if ( (n = writen(fd, ptr, nbytes)) < 0)
		err.err_msg("WriteN error");
	return(n);
}
