#ifndef _OCTREESKELETON
#define _OCTREESKELETON

#include "resources.h"
#include "Helper.h"
using namespace std;

typedef struct _SKELETON_ELEMENT {
	UINT64 m_n64Position;
	int m_nLevel;
	int m_nDir;
	int m_nDimension;
	bool m_bValid;
	list<_SKELETON_ELEMENT *> m_listCompatible;
	UINT64 m_n64Size;
	Element * m_pElement;
	_SKELETON_ELEMENT * m_pDualElement;
	Element * m_pSimpleElement;
	BYTE m_cPointer;
	BYTE m_cBackPointer;

	_SKELETON_ELEMENT() : m_bValid(false) { }
	_SKELETON_ELEMENT(UINT64 n64Position, int nLevel, int nDir, int nDimension, Element * pElement)
		: m_n64Position(n64Position)
		, m_nLevel(nLevel)
		, m_nDir(nDir)
		, m_nDimension(nDimension)
		, m_bValid(false)
		, m_pElement(pElement)
	{}
	_SKELETON_ELEMENT(const _SKELETON_ELEMENT & cRef)
		: m_n64Position(cRef.m_n64Position)
		, m_nLevel(cRef.m_nLevel)
		, m_nDir(cRef.m_nDir)
		, m_nDimension(cRef.m_nDimension)
		, m_bValid(cRef.m_bValid)
		, m_n64Size(cRef.m_n64Size)
		, m_pElement(cRef.m_pElement)
		, m_pDualElement(cRef.m_pDualElement)
		, m_pSimpleElement(cRef.m_pSimpleElement)
		, m_cPointer(cRef.m_cPointer)
		, m_cBackPointer(cRef.m_cBackPointer) {}
} SkeletonElement;

class is_removed_skeleton : public std::unary_function<SkeletonElement, bool> 
{
public:
	bool operator( ) ( SkeletonElement & val ) 
	{
		return (val.m_pElement->m_bSkeleton == 0 || val.m_pElement->m_bObject == 0);
	}
};

class is_removed_dual_skeleton : public std::unary_function<SkeletonElement, bool> 
{
public:
	bool operator( ) ( SkeletonElement & val ) 
	{
		return (val.m_pElement->m_bSkeleton == 0 || val.m_pElement->m_bObject == 1);
	}
};

#endif