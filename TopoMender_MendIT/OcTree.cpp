#include "OcTree.h"

OcTree::OcTree()
{
#ifdef BETTER_GENERATING_SET
	m_pInfiniteNode = new UCHAR[sizeof(LeafNode) + sizeof(Element)];
	memset(m_pInfiniteNode, 0, sizeof(LeafNode) + sizeof(Element));
	((LeafNode *)m_pInfiniteNode)->m_nType = 3;
	((LeafNode *)m_pInfiniteNode)->m_nVertex = 0xFFFFFFFF;
	SET_STORAGE_TAG(((LeafNode *)m_pInfiniteNode)->m_nType, 0);
	GetElementCell(m_pInfiniteNode)->m_bSkeleton = 1;
#else
	m_pInfiniteNode = new UCHAR[sizeof(LeafNode) + sizeof(Element) + sizeof(UINT64)];
	memset(m_pInfiniteNode, 0, sizeof(LeafNode) + sizeof(Element) + sizeof(UINT64));
	((LeafNode *)m_pInfiniteNode)->m_nType = 3;
	((LeafNode *)m_pInfiniteNode)->m_nVertex = 0xFFFFFFFF;
	SET_STORAGE_TAG(((LeafNode *)m_pInfiniteNode)->m_nType, 0);
	Element * pInfCell = GetElementCell(m_pInfiniteNode);
	pInfCell->m_bSkeleton = 1;
	UINT64 * pInfFootPoint = GetFootPoint(pInfCell);
	*pInfFootPoint = INT64_INVALID;
#endif
	m_n64Infinite = INT64TRANS(NLENGTH(0), NLENGTH(0), NLENGTH(0));

	m_pHashMap = NULL;
	m_pSingleHashMap = NULL;
	m_pRoot = NULL;
}

OcTree::~OcTree(void)
{
	if (m_pHashMap)
		delete m_pHashMap;
	if (m_pSingleHashMap)
		delete m_pSingleHashMap;
	delete []m_pInfiniteNode;
}


//////////////////////////////////////////////////////////////////////////
// COcTree::m_nType, which determines the behaviors done during each traversal.
//
// 16 -- set last_time and last_fix to true
// 17 -- after thinning, set all the m_bSkeleton for objects
// 256 + 16 -- count inside skeleton numbers
// 256 + 17 -- count outside skeleton numbers
// 258 + 18 -- count sketching numbers
//////////////////////////////////////////////////////////////////////////
void OcTree::RecNode(UCHAR * pNode)
{
	if (NOT_LEAF(pNode)) {
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					RecNode(GetLeafNode(pNode, i, j, k));
	} else {
		int nCount;
		UCHAR * pPointer;
		int k;
		switch (m_nType) {
		case 16:
			break;
		case 17:
			nCount = CountElement(pNode, 27);
			pPointer = pNode + sizeof(LeafNode);
			for (int i = 0; i < nCount; i++) {
				Element * pElement = (Element *)pPointer;
				if (pElement->m_bObject) {
					if (pElement->m_cSimpleTag != 0) {
						pElement->m_bSkeleton = 0;
						pElement->m_cSimpleTag = 0;
					}
				}
#ifdef BETTER_GENERATING_SET
				pPointer += (sizeof(Element) + sizeof(UINT64));
#else
				pPointer += sizeof(Element);
#endif
			}
			break;
		case 256 + 16:
			nCount = CountElement(pNode, 27);
			pPointer = pNode + sizeof(LeafNode);
			k = 0;
			for ( int i = 0; i < 27; i++ ) {
				if ( GET_STORAGE_TAG( ((LeafNode *)pNode)->m_nType, i ) ) {
					Element * pElement = (Element *)pPointer;
					if (pElement->m_bObject && pElement->m_bSkeleton) {
						if (i < 1) {
							m_nCountCell ++;
						} else if (i < 1 + 6) {
							m_nCountFace ++;
						} else if (i < 1 + 6 + 12) {
							m_nCountEdge ++;
						} else {
							m_nCountPoint ++;
						}
					}
#ifdef BETTER_GENERATING_SET
					pPointer += (sizeof(Element) + sizeof(UINT64));
#else
					pPointer += sizeof(Element);
#endif
				}
			}
			break;
		case 256 + 17:
			nCount = CountElement(pNode, 27);
			pPointer = pNode + sizeof(LeafNode);
			k = 0;
			for ( int i = 0; i < 27; i++ ) {
				if ( GET_STORAGE_TAG( ((LeafNode *)pNode)->m_nType, i ) ) {
					Element * pElement = (Element *)pPointer;
					if (!pElement->m_bObject && pElement->m_bSkeleton) {
						if (i < 1) {
							m_nCountCell ++;
						} else if (i < 1 + 6) {
							m_nCountFace ++;
						} else if (i < 1 + 6 + 12) {
							m_nCountEdge ++;
						} else {
							m_nCountPoint ++;
						}
					}
#ifdef BETTER_GENERATING_SET
					pPointer += (sizeof(Element) + sizeof(UINT64));
#else
					pPointer += sizeof(Element);
#endif
				}
			}
			break;
		case 256 + 18:
			nCount = CountElement(pNode, 27);
			pPointer = pNode + sizeof(LeafNode);
			k = 0;
			for ( int i = 0; i < 27; i++ ) {
				if ( GET_STORAGE_TAG( ((LeafNode *)pNode)->m_nType, i ) ) {
					Element * pElement = (Element *)pPointer;
					if (pElement->m_bSketch) {
						if (i < 1) {
							m_nSketchCell ++;
						} else if (i < 1 + 6) {
							m_nSketchFace ++;
						} else if (i < 1 + 6 + 12) {
							m_nSketchEdge ++;
						} else {
							m_nSketchPoint ++;
						}
					}
#ifdef BETTER_GENERATING_SET
					pPointer += (sizeof(Element) + sizeof(UINT64));
#else
					pPointer += sizeof(Element);
#endif
				}
			}
			break;
		}
	}
	((NodeHeader *)pNode)->m_bLastTime = 1;
	((NodeHeader *)pNode)->m_bLastFix = 1;
}