
#include "ShareLib.h"

void UtilReboot()
{
#ifdef _WIN32
	ShellExecute(NULL,"open", "reboot.exe", NULL, NULL, SW_SHOW); 
#elif _SOLARIS
	reboot(RB_AUTOBOOT, "");
#else
	reboot(0);
#endif
}

