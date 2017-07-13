#include "OcTree.h"

void OcTree::PrintInfo() {
#ifdef CONSOLE_OUTPUT
	m_nType = 1;
	m_nCountCell = 0;
	m_nCountFace = 0;
	m_nCountEdge = 0;
	m_nCountPoint = 0;
	TraversalWithoutPosition();
	printf("OcTree with %d cells, %d faces, %d edges, %d points.\n", m_nCountCell, m_nCountFace, m_nCountEdge, m_nCountPoint);

	m_nType = 2;
	m_nCountCell = 0;
	m_nCountFace = 0;
	m_nCountEdge = 0;
	m_nCountPoint = 0;
	TraversalWithoutPosition();

	printf("Inside part contains %d cells, %d faces, %d edges, %d points.\n", m_nCountCell, m_nCountFace, m_nCountEdge, m_nCountPoint);
	printf("Total %d handles.\n", m_nCountEdge + m_nCountCell - m_nCountFace - m_nCountPoint + 1);
#endif
}

void OcTree::PrintSkeletonInfo() {
#ifdef CONSOLE_OUTPUT
#ifdef CONSOLE_DEBUG
	m_nType = 256 + 16;
	m_nCountCell = 0;
	m_nCountFace = 0;
	m_nCountEdge = 0;
	m_nCountPoint = 0;
	RecNode(m_pInfiniteNode);
	RecNode(m_pRoot);

	printf("Inside skeleton contains %d cells, %d faces, %d edges, %d points.\n", m_nCountCell, m_nCountFace, m_nCountEdge, m_nCountPoint);
	printf("Total %d handles.\n", m_nCountEdge + m_nCountCell - m_nCountFace - m_nCountPoint + 1);

	m_nType = 256 + 17;
	m_nCountCell = 0;
	m_nCountFace = 0;
	m_nCountEdge = 0;
	m_nCountPoint = 0;
	RecNode(m_pInfiniteNode);
	RecNode(m_pRoot);

	printf("Outside skeleton contains %d cells, %d faces, %d edges, %d points.\n", m_nCountCell, m_nCountFace, m_nCountEdge, m_nCountPoint);
	printf("Total %d handles.\n", - m_nCountEdge - m_nCountCell + m_nCountFace + m_nCountPoint + 1);
#endif
#endif
}

void OcTree::PrintSketchingInfo() {
#ifdef CONSOLE_OUTPUT
#ifdef CONSOLE_DEBUG
	m_nType = 256 + 18;
	m_nSketchCell = 0;
	m_nSketchFace = 0;
	m_nSketchEdge = 0;
	m_nSketchPoint = 0;
	RecNode(m_pInfiniteNode);
	RecNode(m_pRoot);

	printf("Sketching contains %d cells, %d faces, %d edges, %d points.\n", m_nSketchCell, m_nSketchFace, m_nSketchEdge, m_nSketchPoint);
	printf("Total %d handles.\n", m_nSketchEdge + m_nSketchCell - m_nSketchFace - m_nSketchPoint + 1);
#endif
#endif
}

void OcTree::PrintPureSkeletonInfo()
{
#ifdef CONSOLE_OUTPUT
#ifdef CONSOLE_DEBUG
	int nCount[4];
	nCount[0] = nCount[1] = nCount[2] = nCount[3] = 0;
	for (list<SkeletonElement>::iterator it = m_listSkeleton.begin(); it != m_listSkeleton.end(); it++) {
		if (it->m_pElement->m_bSkeleton)
			nCount[it->m_nDimension]++;
	}

	printf("Inside pure skeleton contains %d cells, %d faces, %d edges, %d points.\n", nCount[3], nCount[2], nCount[1], nCount[0]);

	int nOutCount[4];
	nOutCount[0] = nOutCount[1] = nOutCount[2] = nOutCount[3] = 0;
	for (list<SkeletonElement>::iterator it = m_listDualSkeleton.begin(); it != m_listDualSkeleton.end(); it++) {
		if (it->m_pElement->m_bSkeleton)
			nOutCount[it->m_nDimension]++;
	}

	printf("Outside pure skeleton contains %d cells, %d faces, %d edges, %d points.\n", nOutCount[3], nOutCount[2], nOutCount[1], nOutCount[0]);

	printf("Handle numbers are : inside = %d, outside = %d\n", 
		m_nSketchEdge + m_nSketchCell - m_nSketchFace - m_nSketchPoint + 1 + nCount[1] + nCount[3] - nCount[2] - nCount[0],
		1 - nOutCount[1] - nOutCount[3] + nOutCount[2] + nOutCount[0]
	);
#endif
#endif
}
