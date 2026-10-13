
#include <shareheader.h>
#include "Objplex.h"

/////////////////////////////////////////////////////////////////////////////
// CObjplex

CObjplex* CObjplex::Create(CObjplex*& pHead, UINT nMax, UINT cbElement)
{
	CObjplex* p = (CObjplex*) new BYTE[sizeof(CObjplex) + nMax * cbElement];	
	// may throw exception
	p->pNext = pHead;
	pHead = p;  // change head (adds in reverse order for simplicity)
	return p;
}

void CObjplex::FreeDataChain()     // free this one and links
{
	CObjplex* p = this;
	while (p != NULL)
	{
		BYTE* bytes = (BYTE*) p;
		CObjplex* pNext = p->pNext;
		free(bytes);
		bytes = NULL;
		p = pNext;
	}
}
