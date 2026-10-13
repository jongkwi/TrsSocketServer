/* include CListSocket */

#include <shareheader.h>
#include "ListObject.h"

/////////////////////////////////////////////////////////////////////////////

CListObject::CListObject(int nBlockSize)
{
	ListObjectInit(nBlockSize);
}

void CListObject::RemoveAll()
{
	// destroy elements
	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	if(m_pBlocks != NULL){
		m_pBlocks->FreeDataChain();
		m_pBlocks = NULL;
	}
}

CListObject::~CListObject()
{
	RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
// Node helpers
/*
 * Implementation note: CNode's are stored in CObjplex blocks and
 *  chained together. Free blocks are maintained in a singly linked list
 *  using the 'pNext' member of CNode with 'm_pNodeFree' as the head.
 *  Used blocks are maintained in a doubly linked list using both 'pNext'
 *  and 'pPrev' as links and 'm_pNodeHead' and 'm_pNodeTail'
 *   as the head/tail.
 *
 * We never free a CObjplex block unless the List is destroyed or RemoveAll()
 *  is used - so the total number of CObjplex blocks may grow large depending
 *  on the maximum past size of the list.
 */

void CListObject::ListObjectInit(int nBlockSize)
{
	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

CListObject::CNode*
CListObject::NewNode(CListObject::CNode* pPrev, CListObject::CNode* pNext)
{
	if (m_pNodeFree == NULL)
	{
		// add another block
		CObjplex* pNewBlock = CObjplex::Create(m_pBlocks, m_nBlockSize,
				 sizeof(CNode));

		// chain them into free list
		CNode* pNode = (CNode*) pNewBlock->data();
		// free in reverse order to make it easier to debug
		pNode += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pNode--)
		{
			pNode->pNext = m_pNodeFree;
			m_pNodeFree = pNode;
		}
	}

	CListObject::CNode* pNode = m_pNodeFree;
	m_pNodeFree = m_pNodeFree->pNext;
	pNode->pPrev = pPrev;
	pNode->pNext = pNext;
	++m_nCount;

	pNode->data = 0; // start with zero
	return pNode;
}

void CListObject::FreeNode(CListObject::CNode* pNode)
{

	pNode->pNext = m_pNodeFree;
	m_pNodeFree = pNode;
	m_nCount--;

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////

LISTPOS CListObject::AddHead(void* newElement)
{
	CNode* pNewNode = NewNode(NULL, m_pNodeHead);
	pNewNode->data = newElement;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = pNewNode;
	else
		m_pNodeTail = pNewNode;
	m_pNodeHead = pNewNode;
	return (LISTPOS) pNewNode;

}



LISTPOS CListObject::AddTail(void* newElement)
{
	CNode* pNewNode = NewNode(m_pNodeTail, NULL);
	pNewNode->data = newElement;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = pNewNode;
	else
		m_pNodeHead = pNewNode;
	m_pNodeTail = pNewNode;
	return (LISTPOS) pNewNode;

}

void CListObject::AddHead(CListObject* pNewList)
{
	// add a list of same elements to head (maintain order)
	LISTPOS pos = pNewList->GetTailPosition();
	while (pos != NULL)
		AddHead(pNewList->GetPrev(pos));
}

void CListObject::AddTail(CListObject* pNewList)
{
	// add a list of same elements
	LISTPOS pos = pNewList->GetHeadPosition();
	while (pos != NULL)
		AddTail(pNewList->GetNext(pos));
}

void* CListObject::RemoveHead()
{
	CNode* pOldNode = m_pNodeHead;
	void* returnValue = pOldNode->data;

	m_pNodeHead = pOldNode->pNext;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = NULL;
	else
		m_pNodeTail = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

void* CListObject::RemoveTail()
{
	CNode* pOldNode = m_pNodeTail;
	void* returnValue = pOldNode->data;

	m_pNodeTail = pOldNode->pPrev;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = NULL;
	else
		m_pNodeHead = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

LISTPOS CListObject::InsertBefore(LISTPOS position, void* newElement)
{
	if (position == NULL)
		return AddHead(newElement); // insert before nothing -> head of the list

	// Insert it before position
	CNode* pOldNode = (CNode*) position;
	CNode* pNewNode = NewNode(pOldNode->pPrev, pOldNode);
	pNewNode->data = newElement;

	if (pOldNode->pPrev != NULL)
	{
		pOldNode->pPrev->pNext = pNewNode;
	}
	else
	{
		m_pNodeHead = pNewNode;
	}
	pOldNode->pPrev = pNewNode;
	return (LISTPOS) pNewNode;

}



LISTPOS CListObject::InsertAfter(LISTPOS position, void* newElement)
{
	if (position == NULL)
		return AddTail(newElement); // insert after nothing -> tail of the list

	// Insert it before position
	CNode* pOldNode = (CNode*) position;
	CNode* pNewNode = NewNode(pOldNode, pOldNode->pNext);
	pNewNode->data = newElement;

	if (pOldNode->pNext != NULL)
	{
		pOldNode->pNext->pPrev = pNewNode;
	}
	else
	{
		m_pNodeTail = pNewNode;
	}
	pOldNode->pNext = pNewNode;
	return (LISTPOS) pNewNode;

}



void CListObject::RemoveAt(LISTPOS position)
{
	CNode* pOldNode = (CNode*) position;
	// remove pOldNode from list
	if (pOldNode == m_pNodeHead)
	{
		m_pNodeHead = pOldNode->pNext;
	}
	else
	{
		pOldNode->pPrev->pNext = pOldNode->pNext;
	}
	if (pOldNode == m_pNodeTail)
	{
		m_pNodeTail = pOldNode->pPrev;
	}
	else
	{
		pOldNode->pNext->pPrev = pOldNode->pPrev;
	}
	FreeNode(pOldNode);
}

int CListObject::GetCount()
{
	return m_nCount;
}

void * CListObject::GetAt(LISTPOS rPosition)
{ 
	CNode* pNode = (CNode*) rPosition;
	return (void*)pNode->data;
}


void CListObject::SetAt(LISTPOS pos, void* newElement)//140422 Ãß°¡ 
{ 
	CNode* pNode = (CNode*) pos;
	pNode->data = newElement;
}


LISTPOS CListObject::GetHeadPosition()
{ 
	return (LISTPOS) m_pNodeHead;
}

LISTPOS CListObject::GetTailPosition()
{ 
	return (LISTPOS) m_pNodeTail;
}

LISTPOS CListObject::GetNext(LISTPOS& rPosition)
{ 
	CNode* pNode = (CNode*) rPosition;
	rPosition = (LISTPOS) pNode->pNext;
	return pNode->data;
}

LISTPOS CListObject::GetPrev(LISTPOS& rPosition)
{ 
	CNode* pNode = (CNode*) rPosition;
	rPosition = (LISTPOS) pNode->pPrev;
	return pNode->data;
}


/////////////////////////////////////////////////////////////////////////////
// slow operations

LISTPOS CListObject::FindIndex(int nIndex) const
{
	if (nIndex >= m_nCount || nIndex < 0)
		return NULL;  // went too far

	CNode* pNode = m_pNodeHead;
	while (nIndex--)
	{
		pNode = pNode->pNext;
	}
	return (LISTPOS) pNode;
}

LISTPOS CListObject::Find(void* searchValue, LISTPOS startAfter) const
{
	CNode* pNode = (CNode*) startAfter;
	if (pNode == NULL)
	{
		pNode = m_pNodeHead;  // start at head
	}
	else
	{
		pNode = pNode->pNext;  // start after the one specified
	}

	for (; pNode != NULL; pNode = pNode->pNext)
		if (pNode->data == searchValue)
			return (LISTPOS) pNode;
	return NULL;
}
