#include "OcTree.h"

//////////////////////////////////////////////////////////////////////////
// containing point functions
//////////////////////////////////////////////////////////////////////////
// COcTree::m_nType, which determines the behaviors done during each traversal.
//
// 32 -- set m_bSketch of all containing elements of a cell
// 33 -- check if a dual element is seperating
// 34 -- generate a dual generating set
//
// 256 + 32 -- print all the belongings
// 256 + 33 -- count all the containing elements of a cell
//////////////////////////////////////////////////////////////////////////

void OcTree::CheckEdge(UINT64 n64CheckEdge, int nCheckEdge, int dirCheckEdge)
{
	UCHAR * pNodeFace[2];
	UCHAR * pNodeEdge[2][2];
	UCHAR * pNodePoint[2][2][2];
	UINT64 n64Face[2];
	UINT64 n64Edge[2][2];
	UINT64 n64Point[2][2][2];
	int nFace[2];
	int nEdge[2][2];
	int nPoint[2][2][2];
	RecCheckEdgeCell(m_pInfiniteNode, m_n64Infinite, -1, n64CheckEdge, nCheckEdge, dirCheckEdge);
	RecCheckEdgeCell(m_pRoot, 0, 0, n64CheckEdge, nCheckEdge, dirCheckEdge);
	for (int i = 0; i < 2; i++)
	{
		for (int ii = 0; ii < 2; ii++) {
			pNodeFace[ii] = m_pInfiniteNode;
			n64Face[ii] = 0;
			nFace[ii] = -1;
		}
		pNodeFace[i] = m_pRoot;
		n64Face[i] = 0;
		nFace[i] = 0;
		RecCheckEdgeFace(pNodeFace, n64Face, nFace, 0, n64CheckEdge, nCheckEdge, dirCheckEdge);
		RecCheckEdgeFace(pNodeFace, n64Face, nFace, 1, n64CheckEdge, nCheckEdge, dirCheckEdge);
		RecCheckEdgeFace(pNodeFace, n64Face, nFace, 2, n64CheckEdge, nCheckEdge, dirCheckEdge);
	}
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
		{
			for (int ii = 0; ii < 2; ii++)
				for (int jj = 0; jj < 2; jj++) {
					pNodeEdge[ii][jj] = m_pInfiniteNode;
					n64Edge[ii][jj] = 0;
					nEdge[ii][jj] = -1;
				}
				pNodeEdge[i][j] = m_pRoot;
				n64Edge[i][j] = 0;
				nEdge[i][j] = 0;
				RecCheckEdgeEdge(pNodeEdge, n64Edge, nEdge, 0, n64CheckEdge, nCheckEdge, dirCheckEdge);
				RecCheckEdgeEdge(pNodeEdge, n64Edge, nEdge, 1, n64CheckEdge, nCheckEdge, dirCheckEdge);
				RecCheckEdgeEdge(pNodeEdge, n64Edge, nEdge, 2, n64CheckEdge, nCheckEdge, dirCheckEdge);
		}
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					for (int ii = 0; ii < 2; ii++)
						for (int jj = 0; jj < 2; jj++)
							for (int kk = 0; kk < 2; kk++) {
								pNodePoint[ii][jj][kk] = m_pInfiniteNode;
								n64Point[ii][jj][kk] = 0;
								nPoint[ii][jj][kk] = -1;
							}
							pNodePoint[i][j][k] = m_pRoot;
							n64Point[i][j][k] = 0;
							nPoint[i][j][k] = 0;
							RecCheckEdgePoint(pNodePoint, n64Point, nPoint, n64CheckEdge, nCheckEdge, dirCheckEdge);
				}
}

void OcTree::RecCheckEdgeCell(UCHAR * pNode, UINT64 n64Position, int nLevel, UINT64 n64CheckEdge, int nCheckEdge, int dirCheckEdge)
{
	if (!CheckEdgeForCell(n64Position, nLevel, n64CheckEdge, nCheckEdge, dirCheckEdge))
		return;

	if (NOT_LEAF(pNode)) {
		UCHAR * pNodeCell;
		UCHAR * pNodeFace[2];
		UCHAR * pNodeEdge[2][2];
		UCHAR * pNodePoint[2][2][2];
		UINT64 n64Cell;
		UINT64 n64Face[2];
		UINT64 n64Edge[2][2];
		UINT64 n64Point[2][2][2];
		int nCell;
		int nFace[2];
		int nEdge[2][2];
		int nPoint[2][2][2];
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					Leaf(pNode, n64Position, nLevel, i, j, k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++) {
					pNodeCell = pNodePoint[i][j][k];
					n64Cell = n64Point[i][j][k];
					nCell = nPoint[i][j][k];
					RecCheckEdgeCell(pNodeCell, n64Cell, nCell, n64CheckEdge, nCheckEdge, dirCheckEdge);
				}
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
					{
						for (int k = 0; k < 2; k++) {
							pNodeFace[k] = pNodePoint[k][i][j];
							n64Face[k] = n64Point[k][i][j];
							nFace[k] = nPoint[k][i][j];
						}
						RecCheckEdgeFace(pNodeFace, n64Face, nFace, 0, n64CheckEdge, nCheckEdge, dirCheckEdge);
						for (int k = 0; k < 2; k++) {
							pNodeFace[k] = pNodePoint[j][k][i];
							n64Face[k] = n64Point[j][k][i];
							nFace[k] = nPoint[j][k][i];
						}
						RecCheckEdgeFace(pNodeFace, n64Face, nFace, 1, n64CheckEdge, nCheckEdge, dirCheckEdge);
						for (int k = 0; k < 2; k++) {
							pNodeFace[k] = pNodePoint[i][j][k];
							n64Face[k] = n64Point[i][j][k];
							nFace[k] = nPoint[i][j][k];
						}
						RecCheckEdgeFace(pNodeFace, n64Face, nFace, 2, n64CheckEdge, nCheckEdge, dirCheckEdge);
					}
					for (int i = 0; i < 2; i++)
					{
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[i][j][k];
								n64Edge[j][k] = n64Point[i][j][k];
								nEdge[j][k] = nPoint[i][j][k];
							}
							RecCheckEdgeEdge(pNodeEdge, n64Edge, nEdge, 0, n64CheckEdge, nCheckEdge, dirCheckEdge);
							for (int j = 0; j < 2; j++)
								for (int k = 0; k < 2; k++) {
									pNodeEdge[j][k] = pNodePoint[k][i][j];
									n64Edge[j][k] = n64Point[k][i][j];
									nEdge[j][k] = nPoint[k][i][j];
								}
								RecCheckEdgeEdge(pNodeEdge, n64Edge, nEdge, 1, n64CheckEdge, nCheckEdge, dirCheckEdge);
								for (int j = 0; j < 2; j++)
									for (int k = 0; k < 2; k++) {
										pNodeEdge[j][k] = pNodePoint[j][k][i];
										n64Edge[j][k] = n64Point[j][k][i];
										nEdge[j][k] = nPoint[j][k][i];
									}
									RecCheckEdgeEdge(pNodeEdge, n64Edge, nEdge, 2, n64CheckEdge, nCheckEdge, dirCheckEdge);
					}
					RecCheckEdgePoint(pNodePoint, n64Point, nPoint, n64CheckEdge, nCheckEdge, dirCheckEdge);
	} else {
		switch (m_nType) {
		case 256 + 32:
#ifdef CONSOLE_OUTPUT
			printf("Cell : (%16I64x, %d)\n", n64Position, nLevel);
#endif
			break;
		}
	}
}

void OcTree::RecCheckEdgeFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, UINT64 n64CheckEdge, int nCheckEdge, int dirCheckEdge)
{
	UINT64 n64ThisFace;
	int nThisFace;
	GetFacePosition(n64Position, nLevel, dirFace, n64ThisFace, nThisFace);
	if (!CheckEdgeForFace(n64ThisFace, nThisFace, dirFace, n64CheckEdge, nCheckEdge, dirCheckEdge))
		return;

	if ((*(pNode[0]) & 0x3) == 0 || (*(pNode[1]) & 0x3) == 0) {
		UCHAR * pNodeFace[2];
		UCHAR * pNodeEdge[2][2];
		UCHAR * pNodePoint[2][2][2];
		UINT64 n64Face[2];
		UINT64 n64Edge[2][2];
		UINT64 n64Point[2][2][2];
		int nFace[2];
		int nEdge[2][2];
		int nPoint[2][2][2];
		switch(dirFace) {
		case 0:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[i], n64Position[i], nLevel[i], 1-i, j, k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++) {
						pNodeFace[k] = pNodePoint[k][i][j];
						n64Face[k] = n64Point[k][i][j];
						nFace[k] = nPoint[k][i][j];
					}
					RecCheckEdgeFace(pNodeFace, n64Face, nFace, 0, n64CheckEdge, nCheckEdge, dirCheckEdge);
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++) {
							pNodeEdge[j][k] = pNodePoint[k][i][j];
							n64Edge[j][k] = n64Point[k][i][j];
							nEdge[j][k] = nPoint[k][i][j];
						}
						RecCheckEdgeEdge(pNodeEdge, n64Edge, nEdge, 1, n64CheckEdge, nCheckEdge, dirCheckEdge);
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[j][k][i];
								n64Edge[j][k] = n64Point[j][k][i];
								nEdge[j][k] = nPoint[j][k][i];
							}
							RecCheckEdgeEdge(pNodeEdge, n64Edge, nEdge, 2, n64CheckEdge, nCheckEdge, dirCheckEdge);
				}
				RecCheckEdgePoint(pNodePoint, n64Point, nPoint, n64CheckEdge, nCheckEdge, dirCheckEdge);
				break;
		case 1:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j], n64Position[j], nLevel[j], i, 1-j, k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++) {
						pNodeFace[k] = pNodePoint[j][k][i];
						n64Face[k] = n64Point[j][k][i];
						nFace[k] = nPoint[j][k][i];
					}
					RecCheckEdgeFace(pNodeFace, n64Face, nFace, 1, n64CheckEdge, nCheckEdge, dirCheckEdge);
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++) {
							pNodeEdge[j][k] = pNodePoint[j][k][i];
							n64Edge[j][k] = n64Point[j][k][i];
							nEdge[j][k] = nPoint[j][k][i];
						}
						RecCheckEdgeEdge(pNodeEdge, n64Edge, nEdge, 2, n64CheckEdge, nCheckEdge, dirCheckEdge);
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[i][j][k];
								n64Edge[j][k] = n64Point[i][j][k];
								nEdge[j][k] = nPoint[i][j][k];
							}
							RecCheckEdgeEdge(pNodeEdge, n64Edge, nEdge, 0, n64CheckEdge, nCheckEdge, dirCheckEdge);
				}
				RecCheckEdgePoint(pNodePoint, n64Point, nPoint, n64CheckEdge, nCheckEdge, dirCheckEdge);
				break;
		case 2:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k], n64Position[k], nLevel[k], i, j, 1-k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++) {
						pNodeFace[k] = pNodePoint[i][j][k];
						n64Face[k] = n64Point[i][j][k];
						nFace[k] = nPoint[i][j][k];
					}
					RecCheckEdgeFace(pNodeFace, n64Face, nFace, 2, n64CheckEdge, nCheckEdge, dirCheckEdge);
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++) {
							pNodeEdge[j][k] = pNodePoint[i][j][k];
							n64Edge[j][k] = n64Point[i][j][k];
							nEdge[j][k] = nPoint[i][j][k];
						}
						RecCheckEdgeEdge(pNodeEdge, n64Edge, nEdge, 0, n64CheckEdge, nCheckEdge, dirCheckEdge);
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[k][i][j];
								n64Edge[j][k] = n64Point[k][i][j];
								nEdge[j][k] = nPoint[k][i][j];
							}
							RecCheckEdgeEdge(pNodeEdge, n64Edge, nEdge, 1, n64CheckEdge, nCheckEdge, dirCheckEdge);
				}
				RecCheckEdgePoint(pNodePoint, n64Point, nPoint, n64CheckEdge, nCheckEdge, dirCheckEdge);
				break;
		}
	} else {
		switch (m_nType) {
		case 256 + 32:
#ifdef CONSOLE_OUTPUT
			printf("Face : (%16I64x, %d, %d)\n", n64ThisFace, nThisFace, dirFace);
#endif
			break;
		}
	}
}

void OcTree::RecCheckEdgeEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, UINT64 n64CheckEdge, int nCheckEdge, int dirCheckEdge)
{
	UINT64 n64ThisEdge;
	int nThisEdge;
	GetEdgePosition(n64Position, nLevel, dirEdge, n64ThisEdge, nThisEdge);
	if (!CheckEdgeForEdge(n64ThisEdge, nThisEdge, dirEdge, n64CheckEdge, nCheckEdge, dirCheckEdge))
		return;
	if ((*(pNode[0][0]) & 0x3) == 0 || (*(pNode[0][1]) & 0x3) == 0 || (*(pNode[1][0]) & 0x3) == 0 || (*(pNode[1][1]) & 0x3) == 0) {
		UCHAR * pNodeEdge[2][2];
		UCHAR * pNodePoint[2][2][2];
		UINT64 n64Edge[2][2];
		UINT64 n64Point[2][2][2];
		int nEdge[2][2];
		int nPoint[2][2][2];
		switch(dirEdge) {
		case 0:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], i, 1-j, 1-k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++) {
						pNodeEdge[j][k] = pNodePoint[i][j][k];
						n64Edge[j][k] = n64Point[i][j][k];
						nEdge[j][k] = nPoint[i][j][k];
					}
					RecCheckEdgeEdge(pNodeEdge, n64Edge, nEdge, 0, n64CheckEdge, nCheckEdge, dirCheckEdge);
			}
			RecCheckEdgePoint(pNodePoint, n64Point, nPoint, n64CheckEdge, nCheckEdge, dirCheckEdge);
			break;
		case 1:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k][i], n64Position[k][i], nLevel[k][i], 1-i, j, 1-k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++) {
						pNodeEdge[j][k] = pNodePoint[k][i][j];
						n64Edge[j][k] = n64Point[k][i][j];
						nEdge[j][k] = nPoint[k][i][j];
					}
					RecCheckEdgeEdge(pNodeEdge, n64Edge, nEdge, 1, n64CheckEdge, nCheckEdge, dirCheckEdge);
			}
			RecCheckEdgePoint(pNodePoint, n64Point, nPoint, n64CheckEdge, nCheckEdge, dirCheckEdge);
			break;
		case 2:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[i][j], n64Position[i][j], nLevel[i][j], 1-i, 1-j, k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++) {
						pNodeEdge[j][k] = pNodePoint[j][k][i];
						n64Edge[j][k] = n64Point[j][k][i];
						nEdge[j][k] = nPoint[j][k][i];
					}
					RecCheckEdgeEdge(pNodeEdge, n64Edge, nEdge, 2, n64CheckEdge, nCheckEdge, dirCheckEdge);
			}
			RecCheckEdgePoint(pNodePoint, n64Point, nPoint, n64CheckEdge, nCheckEdge, dirCheckEdge);
			break;
		}
	} else {
		switch (m_nType) {
		case 256 + 32:
#ifdef CONSOLE_OUTPUT
			printf("Edge : (%16I64x, %d, %d)\n", n64ThisEdge, nThisEdge, dirEdge);
#endif
			break;
		}
	}
}

void OcTree::RecCheckEdgePoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2], UINT64 n64CheckEdge, int nCheckEdge, int dirCheckEdge)
{
	UINT64 n64ThisPoint;
	GetPointPosition(n64Position, nLevel, n64ThisPoint);
	if (!CheckEdgeForPoint(n64ThisPoint, n64CheckEdge, nCheckEdge, dirCheckEdge))
		return;

	if ((*(pNode[0][0][0]) & 0x3) == 0 || (*(pNode[0][0][1]) & 0x3) == 0 || (*(pNode[0][1][0]) & 0x3) == 0 || (*(pNode[0][1][1]) & 0x3) == 0 || 
		(*(pNode[1][0][0]) & 0x3) == 0 || (*(pNode[1][0][1]) & 0x3) == 0 || (*(pNode[1][1][0]) & 0x3) == 0 || (*(pNode[1][1][1]) & 0x3) == 0) {
			UCHAR * pNodePoint[2][2][2];
			UINT64 n64Point[2][2][2];
			int nPoint[2][2][2];
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[i][j][k], n64Position[i][j][k], nLevel[i][j][k], 1-i, 1-j, 1-k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
			RecCheckEdgePoint(pNodePoint, n64Point, nPoint, n64CheckEdge, nCheckEdge, dirCheckEdge);
	} else {
		Element * pThisPoint;
		switch (m_nType) {
		case 33:
			pThisPoint = GetElementPoint(pNode, nLevel);
			if ( !pThisPoint->m_bObject && pThisPoint->m_bSkeleton )
				m_bIsSeperating = false;
			break;
		case 34:
			pThisPoint = GetElementPoint(pNode, nLevel);
			if ( !pThisPoint->m_bObject && !pThisPoint->m_bSkeleton) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisPoint );
				if ( pr.second ) {
					// printf("Point : (%16I64x)\n", n64ThisPoint);
					if ( !m_bComputeOnly ) {
						m_listChanging.push_back(SkeletonElement(n64ThisPoint, 0, 0, 0, pThisPoint));
					}
					else
						m_pCurrentSkeletonElement->m_n64Size += GetPointVolume();
					DualPushWitnessPoint(pNode, n64Position, nLevel, pThisPoint->m_cPointer);
				}
			}
			break;
		case 37:
			pThisPoint = GetElementPoint(pNode, nLevel);
			if ( pThisPoint->m_bObject ) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisPoint );
				if ( pr.second ) {
					if ( !pThisPoint->m_bSkeleton) {
						// printf("Point : (%16I64x)\n", n64ThisPoint);
						DualPushWitnessPoint(pNode, n64Position, nLevel, pThisPoint->m_cPointer);
					} else {
						hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashSkeleton.find((unsigned int)pThisPoint);
						if ( itTemp != m_hashSkeleton.end() ) {
							itTemp->second->m_bValid = false;
						}
					}
				}
			}
			break;
		case 38:
			pThisPoint = GetElementPoint(pNode, nLevel);
			if ( pThisPoint->m_bObject && pThisPoint->m_bSkeleton && !pThisPoint->m_bSketch ) {
				m_listGeneratingSet.push_back(SkeletonElement(n64ThisPoint, 0, 0, 0, pThisPoint));
			}
			break;
		case 39:
			pThisPoint = GetElementPoint(pNode, nLevel);
			if ( !pThisPoint->m_bObject && pThisPoint->m_bSkeleton ) {
				m_nBelong ++;
				if (m_nBelong == 1) {
					hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashDualSkeleton.find((unsigned int)pThisPoint);
					if ( itTemp != m_hashDualSkeleton.end() ) {
						m_pBelong = itTemp->second;
						UCHAR * pTest[2][2];
						UINT64 n64Test[2][2];
						int nTest[2][2];
						GetEdgeFromPosition(n64CheckEdge, nCheckEdge, dirCheckEdge, pTest, n64Test, nTest);
						int iTest;
						switch (dirCheckEdge) {
		case 0:
			if ( n64Test[0][0] == n64Position[0][0][0] && n64Test[0][1] == n64Position[0][0][1] &&
				n64Test[1][0] == n64Position[0][1][0] && n64Test[1][1] == n64Position[0][1][1] )
			{
				iTest = 0;
			} else {
				iTest = 1;
			}
			m_pBelong->m_cPointer = dirCheckEdge * 2 + iTest;
			m_pBelong->m_cBackPointer = 1 - iTest + 4;
			break;
		case 1:
			if ( n64Test[0][0] == n64Position[0][0][0] && n64Test[0][1] == n64Position[1][0][0] &&
				n64Test[1][0] == n64Position[0][0][1] && n64Test[1][1] == n64Position[1][0][1] )
			{
				iTest = 0;
			} else {
				iTest = 1;
			}
			m_pBelong->m_cPointer = dirCheckEdge * 2 + iTest;
			m_pBelong->m_cBackPointer = 1 - iTest + 4;
			break;
		case 2:
			if ( n64Test[0][0] == n64Position[0][0][0] && n64Test[0][1] == n64Position[0][1][0] &&
				n64Test[1][0] == n64Position[1][0][0] && n64Test[1][1] == n64Position[1][1][0] )
			{
				iTest = 0;
			} else {
				iTest = 1;
			}
			m_pBelong->m_cPointer = dirCheckEdge * 2 + iTest;
			m_pBelong->m_cBackPointer = 1 - iTest + 4;
			break;
						}
					}
				}
			}
			break;
		case 40:
			pThisPoint = GetElementPoint(pNode, nLevel);
			if ( !pThisPoint->m_bSkeleton ) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisPoint );
				if ( pr.second ) {
					// printf("Point : (%16I64x)\n", n64ThisPoint);
					DualPushWitnessPoint(pNode, n64Position, nLevel, pThisPoint->m_cPointer);
				}
			} else if ( pThisPoint->m_bSkeleton && pThisPoint->m_bObject ) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisPoint );
				if ( pr.second ) {
					hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashSkeleton.find((unsigned int)pThisPoint);
					if ( itTemp != m_hashSkeleton.end() ) {
						itTemp->second->m_bValid = false;
					}
				}
			}
			break;
		case 256 + 32:
#ifdef CONSOLE_OUTPUT
			printf("Point : (%16I64x)\n", n64ThisPoint);
#endif
			break;
		}
	}
}
