#ifndef ___OBJPLEX_H___
#define ___OBJPLEX_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

struct CObjplex     // warning variable length structure
{
	CObjplex* pNext;

	// BYTE data[maxNum*elementSize];

	void* data() { return this+1; }

	static CObjplex* Create(CObjplex*& head, UINT nMax, UINT cbElement);
			// like 'calloc' but no zero fill
			// may throw memory exceptions

	void FreeDataChain();       // free this one and links
};


#endif//Objplex.h


