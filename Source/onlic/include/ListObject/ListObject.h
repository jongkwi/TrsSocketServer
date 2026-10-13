#ifndef ___LISTOBJECT_H___
#define ___LISTOBJECT_H___

#ifdef __STANDALONE__            /* List of required header files */
#endif

#include <ShareLib/ShareLib.h>
#include "Objplex.h"

class CListObject
{

protected:
	struct CNode
	{
		CNode* pNext;
		CNode* pPrev;
		void* data;
	};
public:

// Construction

	CListObject(int nBlockSize = 10);
	void ListObjectInit(int nBlockSize = 10);

// Attributes (head and tail)
	// count of elements
	int GetCount();
	BOOL IsEmpty() const;

	// peek at head or tail
	void*& GetHead();
	void* GetHead() const;
	void*& GetTail();
	void* GetTail() const;

// Operations
	// get head or tail (and remove it) - don't call on empty list!
	void* RemoveHead();
	void* RemoveTail();

	// add before head or after tail
	LISTPOS AddHead(void* newElement);
	LISTPOS AddTail(void* newElement);


	// add another list of elements before head or after tail
	void AddHead(CListObject* pNewList);
	void AddTail(CListObject* pNewList);

	// remove all elements
	void RemoveAll();

	LISTPOS GetHeadPosition();
	LISTPOS GetTailPosition();
	LISTPOS GetNext(LISTPOS& rPosition);
	LISTPOS GetPrev(LISTPOS& rPosition);

	// getting/modifying an element at a given position
	void * GetAt(LISTPOS position);
	void SetAt(LISTPOS pos, void* newElement);

	void RemoveAt(LISTPOS position);

	// inserting before or after a given position
	LISTPOS InsertBefore(LISTPOS position, void* newElement);
	LISTPOS InsertAfter(LISTPOS position, void* newElement);

	void FreeNodeChain();     // free this one and links

	// helper functions (note: O(n) speed)
	LISTPOS Find(void* searchValue, LISTPOS startAfter = NULL) const;
						// defaults to starting at the HEAD
						// return NULL if not found
	LISTPOS FindIndex(int nIndex) const;
						// get the 'nIndex'th element (may return NULL)

// Implementation
protected:
	CNode* m_pNodeHead;
	CNode* m_pNodeTail;
	int m_nCount;
	CNode* m_pNodeFree;
	CObjplex* m_pBlocks;
	int m_nBlockSize;

	CNode* NewNode(CNode*, CNode*);
	void FreeNode(CNode*);

public:
	~CListObject();

	// local typedefs for class templates
	typedef void* BASE_TYPE;
	typedef void* BASE_ARG_TYPE;
};


#endif//ListObject.h


