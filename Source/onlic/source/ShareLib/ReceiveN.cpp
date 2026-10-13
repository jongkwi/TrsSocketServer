
#include "ShareLib.h"

size_t
ReceiveN(int fd, char *ptr, size_t nbytes)
{
	CError err;
	size_t		n;

	if ( (n = recv(fd, ptr, nbytes, 0)) < 0)
		err.err_sys("recv error");
	return(n);
}
