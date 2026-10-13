
#include <shareheader.h>
#include "daemon_init.h"

#include "wrapunix.h"

#define	MAXFD	64

long daemon_init(const char *pname, int facility)
{
	int		i;
	pid_t	pid, pid2;

	if ( (pid = Fork()) < 0)
		return (-1);
	else if (pid){
		_exit(0);			/* parent terminates */
	}

	/* child 1 continues... */

	if (setsid() < 0)			/* become session leader */
		return (-1);

	signal(SIGHUP, SIG_IGN);
	pid2 = Fork();
	if (pid2 < 0)
		return (-1);
	else if (pid2){
		_exit(0);			/* child 1 terminates */
	}

	return (long)pid;				/* success */
}
