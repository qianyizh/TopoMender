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

void OcTree::CheckFace(UINT64 n64CheckFace, int nCheckFace, int dirCheckFace)
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
	RecCheckFaceCell(m_pInfiniteNode, m_n64Infinite, -1, n64CheckFace, nCheckFace, dirCheckFace);
	RecCheckFaceCell(m_pRoot, 0, 0, n64CheckFace, nCheckFace, dirCheckFace);
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
		RecCheckFaceFace(pNodeFace, n64Face, nFace, 0, n64CheckFace, nCheckFace, dirCheckFace);
		RecCheckFaceFace(pNodeFace, n64Face, nFace, 1, n64CheckFace, nCheckFace, dirCheckFace);
		RecCheckFaceFace(pNodeFace, n64Face, nFace, 2, n64CheckFace, nCheckFace, dirCheckFace);
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
				RecCheckFaceEdge(pNodeEdge, n64Edge, nEdge, 0, n64CheckFace, nCheckFace, dirCheckFace);
				RecCheckFaceEdge(pNodeEdge, n64Edge, nEdge, 1, n64CheckFace, nCheckFace, dirCheckFace);
				RecCheckFaceEdge(pNodeEdge, n64Edge, nEdge, 2, n64CheckFace, nCheckFace, dirCheckFace);
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
							RecCheckFacePoint(pNodePoint, n64Point, nPoint, n64CheckFace, nCheckFace, dirCheckFace);
				}
}

void OcTree::RecCheckFaceCell(UCHAR * pNode, UINT64 n64Position, int nLevel, UINT64 n64CheckFace, int nCheckFace, int dirCheckFace)
{
	if (!CheckFaceForCell(n64Position, nLevel, n64CheckFace, nCheckFace, dirCheckFace))
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
					RecCheckFaceCell(pNodeCell, n64Cell, nCell, n64CheckFace, nCheckFace, dirCheckFace);
				}
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
					{
						for (int k = 0; k < 2; k++) {
							pNodeFace[k] = pNodePoint[k][i][j];
							n64Face[k] = n64Point[k][i][j];
							nFace[k] = nPoint[k][i][j];
						}
						RecCheckFaceFace(pNodeFace, n64Face, nFace, 0, n64CheckFace, nCheckFace, dirCheckFace);
						for (int k = 0; k < 2; k++) {
							pNodeFace[k] = pNodePoint[j][k][i];
							n64Face[k] = n64Point[j][k][i];
							nFace[k] = nPoint[j][k][i];
						}
						RecCheckFaceFace(pNodeFace, n64Face, nFace, 1, n64CheckFace, nCheckFace, dirCheckFace);
						for (int k = 0; k < 2; k++) {
							pNodeFace[k] = pNodePoint[i][j][k];
							n64Face[k] = n64Point[i][j][k];
							nFace[k] = nPoint[i][j][k];
						}
						RecCheckFaceFace(pNodeFace, n64Face, nFace, 2, n64CheckFace, nCheckFace, dirCheckFace);
					}
					for (int i = 0; i < 2; i++)
					{
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[i][j][k];
								n64Edge[j][k] = n64Point[i][j][k];
								nEdge[j][k] = nPoint[i][j][k];
							}
							RecCheckFaceEdge(pNodeEdge, n64Edge, nEdge, 0, n64CheckFace, nCheckFace, dirCheckFace);
							for (int j = 0; j < 2; j++)
								for (int k = 0; k < 2; k++) {
									pNodeEdge[j][k] = pNodePoint[k][i][j];
									n64Edge[j][k] = n64Point[k][i][j];
									nEdge[j][k] = nPoint[k][i][j];
								}
								RecCheckFaceEdge(pNodeEdge, n64Edge, nEdge, 1, n64CheckFace, nCheckFace, dirCheckFace);
								for (int j = 0; j < 2; j++)
									for (int k = 0; k < 2; k++) {
										pNodeEdge[j][k] = pNodePoint[j][k][i];
										n64Edge[j][k] = n64Point[j][k][i];
										nEdge[j][k] = nPoint[j][k][i];
									}
									RecCheckFaceEdge(pNodeEdge, n64Edge, nEdge, 2, n64CheckFace, nCheckFace, dirCheckFace);
					}
					RecCheckFacePoint(pNodePoint, n64Point, nPoint, n64CheckFace, nCheckFace, dirCheckFace);
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

void OcTree::RecCheckFaceFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, UINT64 n64CheckFace, int nCheckFace, int dirCheckFace)
{
	UINT64 n64ThisFace;
	int nThisFace;
	GetFacePosition(n64Position, nLevel, dirFace, n64ThisFace, nThisFace);
	if (!CheckFaceForFace(n64ThisFace, nThisFace, dirFace, n64CheckFace, nCheckFace, dirCheckFace))
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
					RecCheckFaceFace(pNodeFace, n64Face, nFace, 0, n64CheckFace, nCheckFace, dirCheckFace);
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++) {
							pNodeEdge[j][k] = pNodePoint[k][i][j];
							n64Edge[j][k] = n64Point[k][i][j];
							nEdge[j][k] = nPoint[k][i][j];
						}
						RecCheckFaceEdge(pNodeEdge, n64Edge, nEdge, 1, n64CheckFace, nCheckFace, dirCheckFace);
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[j][k][i];
								n64Edge[j][k] = n64Point[j][k][i];
								nEdge[j][k] = nPoint[j][k][i];
							}
							RecCheckFaceEdge(pNodeEdge, n64Edge, nEdge, 2, n64CheckFace, nCheckFace, dirCheckFace);
				}
				RecCheckFacePoint(pNodePoint, n64Point, nPoint, n64CheckFace, nCheckFace, dirCheckFace);
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
					RecCheckFaceFace(pNodeFace, n64Face, nFace, 1, n64CheckFace, nCheckFace, dirCheckFace);
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++) {
							pNodeEdge[j][k] = pNodePoint[j][k][i];
							n64Edge[j][k] = n64Point[j][k][i];
							nEdge[j][k] = nPoint[j][k][i];
						}
						RecCheckFaceEdge(pNodeEdge, n64Edge, nEdge, 2, n64CheckFace, nCheckFace, dirCheckFace);
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[i][j][k];
								n64Edge[j][k] = n64Point[i][j][k];
								nEdge[j][k] = nPoint[i][j][k];
							}
							RecCheckFaceEdge(pNodeEdge, n64Edge, nEdge, 0, n64CheckFace, nCheckFace, dirCheckFace);
				}
				RecCheckFacePoint(pNodePoint, n64Point, nPoint, n64CheckFace, nCheckFace, dirCheckFace);
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
					RecCheckFaceFace(pNodeFace, n64Face, nFace, 2, n64CheckFace, nCheckFace, dirCheckFace);
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++) {
							pNodeEdge[j][k] = pNodePoint[i][j][k];
							n64Edge[j][k] = n64Point[i][j][k];
							nEdge[j][k] = nPoint[i][j][k];
						}
						RecCheckFaceEdge(pNodeEdge, n64Edge, nEdge, 0, n64CheckFace, nCheckFace, dirCheckFace);
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[k][i][j];
								n64Edge[j][k] = n64Point[k][i][j];
								nEdge[j][k] = nPoint[k][i][j];
							}
							RecCheckFaceEdge(pNodeEdge, n64Edge, nEdge, 1, n64CheckFace, nCheckFace, dirCheckFace);
				}
				RecCheckFacePoint(pNodePoint, n64Point, nPoint, n64CheckFace, nCheckFace, dirCheckFace);
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

void OcTree::RecCheckFaceEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, UINT64 n64CheckFace, int nCheckFace, int dirCheckFace)
{
	UINT64 n64ThisEdge;
	int nThisEdge;
	GetEdgePosition(n64Position, nLevel, dirEdge, n64ThisEdge, nThisEdge);
	if (!CheckFaceForEdge(n64ThisEdge, nThisEdge, dirEdge, n64CheckFace, nCheckFace, dirCheckFace))
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
					RecCheckFaceEdge(pNodeEdge, n64Edge, nEdge, 0, n64CheckFace, nCheckFace, dirCheckFace);
			}
			RecCheckFacePoint(pNodePoint, n64Point, nPoint, n64CheckFace, nCheckFace, dirCheckFace);
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
					RecCheckFaceEdge(pNodeEdge, n64Edge, nEdge, 1, n64CheckFace, nCheckFace, dirCheckFace);
			}
			RecCheckFacePoint(pNodePoint, n64Point, nPoint, n64CheckFace, nCheckFace, dirCheckFace);
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
					RecCheckFaceEdge(pNodeEdge, n64Edge, nEdge, 2, n64CheckFace, nCheckFace, dirCheckFace);
			}
			RecCheckFacePoint(pNodePoint, n64Point, nPoint, n64CheckFace, nCheckFace, dirCheckFace);
			break;
		}
	} else {
		Element * pThisEdge;
		switch (m_nType) {
		case 33:
			pThisEdge = GetElementEdge(pNode, nLevel, dirEdge);
			if ( !pThisEdge->m_bObject && pThisEdge->m_bSkeleton )
				m_bIsSeperating = false;
			break;
		case 34:
			pThisEdge = GetElementEdge(pNode, nLevel, dirEdge);
			if ( !pThisEdge->m_bObject && !pThisEdge->m_bSkeleton) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisEdge );
				if ( pr.second ) {
					// printf("Edge : (%16I64x, %d, %d)\n", n64ThisEdge, nThisEdge, dirEdge);
					if ( !m_bComputeOnly ) {
						m_listChanging.push_back(SkeletonElement(n64ThisEdge, nThisEdge, dirEdge, 1, pThisEdge));
					}
					else
						m_pCurrentSkeletonElement->m_n64Size += GetEdgeVolume(nThisEdge);
					DualPushWitnessEdge(pNode, n64Position, nLevel, dirEdge, pThisEdge->m_cPointer);
				}
			}
			break;
		case 37:
			pThisEdge = GetElementEdge(pNode, nLevel, dirEdge);
			if ( pThisEdge->m_bObject ) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisEdge );
				if ( pr.second ) {
					if ( !pThisEdge->m_bSkeleton) {
						// printf("Edge : (%16I64x, %d, %d)\n", n64ThisEdge, nThisEdge, dirEdge);
						DualPushWitnessEdge(pNode, n64Position, nLevel, dirEdge, pThisEdge->m_cPointer);
					} else {
						hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashSkeleton.find((unsigned int)pThisEdge);
						if ( itTemp != m_hashSkeleton.end() ) {
							itTemp->second->m_bValid = false;
						}
					}
				}
			}
			break;
		case 38:
			pThisEdge = GetElementEdge(pNode, nLevel, dirEdge);
			if ( pThisEdge->m_bObject && pThisEdge->m_bSkeleton && !pThisEdge->m_bSketch ) {
				m_listGeneratingSet.push_back(SkeletonElement(n64ThisEdge, nThisEdge, dirEdge, 1, pThisEdge));
			}
			break;
		case 39:
			pThisEdge = GetElementEdge(pNode, nLevel, dirEdge);
			if ( !pThisEdge->m_bObject && pThisEdge->m_bSkeleton ) {
				m_nBelong ++;
				if (m_nBelong == 1) {
					hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashDualSkeleton.find((unsigned int)pThisEdge);
					if ( itTemp != m_hashDualSkeleton.end() ) {
						m_pBelong = itTemp->second;
						UCHAR * pTest[2];
						UINT64 n64Test[2];
						int nTest[2];
						GetFaceFromPosition(n64CheckFace, nCheckFace, dirCheckFace, pTest, n64Test, nTest);
						int iTest;
						if (dirCheckFace == (dirEdge + 2) % 3) {
							if ( n64Test[0] == n64Position[0][0] && n64Test[1] == n64Position[0][1] ) {
								iTest = 0;
							} else {
								iTest = 1;
							}
							m_pBelong->m_cPointer = (0 * 2) + iTest;
							m_pBelong->m_cBackPointer = (0 * 2) + (1 - iTest) + 2;
						} else {
							if ( n64Test[0] == n64Position[0][0] && n64Test[1] == n64Position[1][0] ) {
								iTest = 0;
							} else {
								iTest = 1;
							}
							m_pBelong->m_cPointer = (1 * 2) + iTest;
							m_pBelong->m_cBackPointer = (1 * 2) + (1 - iTest) + 2;
						}
					}
				}
			}
			break;
		case 40:
			pThisEdge = GetElementEdge(pNode, nLevel, dirEdge);
			if ( !pThisEdge->m_bSkeleton ) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisEdge );
				if ( pr.second ) {
					// printf("Edge : (%16I64x, %d, %d)\n", n64ThisEdge, nThisEdge, dirEdge);
					DualPushWitnessEdge(pNode, n64Position, nLevel, dirEdge, pThisEdge->m_cPointer);
				}
			} else if ( pThisEdge->m_bSkeleton && pThisEdge->m_bObject ) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisEdge );
				if ( pr.second ) {
					hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashSkeleton.find((unsigned int)pThisEdge);
					if ( itTemp != m_hashSkeleton.end() ) {
						itTemp->second->m_bValid = false;
					}
				}
			}
			break;
		case 256 + 32:
#ifdef CONSOLE_OUTPUT
			printf("Edge : (%16I64x, %d, %d)\n", n64ThisEdge, nThisEdge, dirEdge);
#endif
			break;
		}
	}
}

void OcTree::RecCheckFacePoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2], UINT64 n64CheckFace, int nCheckFace, int dirCheckFace)
{
	UINT64 n64ThisPoint;
	GetPointPosition(n64Position, nLevel, n64ThisPoint);
	if (!CheckFaceForPoint(n64ThisPoint, n64CheckFace, nCheckFace, dirCheckFace))
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
			RecCheckFacePoint(pNodePoint, n64Point, nPoint, n64CheckFace, nCheckFace, dirCheckFace);
	} else {
		Element * pThisPoint;
		switch (m_nType) {
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
