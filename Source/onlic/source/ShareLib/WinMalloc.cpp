
#include "ShareLib.h"

void * Malloc(size_t size)
{
	CError err;
	void	*ptr;

	if ( (ptr = malloc(size)) == NULL){
		err.err_sys("malloc error size:%d", size);
	}
	else{		
		//if(size == 5796 || size == 84){
		//	TRACE("Malloc size:%d\n", size);
		//}
	}
	return(ptr);
}

void Free(void *ptr)
{
	free(ptr);
}
