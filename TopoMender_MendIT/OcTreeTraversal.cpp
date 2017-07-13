#include "OcTree.h"
using namespace std;

//////////////////////////////////////////////////////////////////////////
// recursive functions
//////////////////////////////////////////////////////////////////////////
// COcTree::m_nType, which determines the behaviors done during each traversal.
//
// 0 -- Set correct storage tag
// 1 -- Count number
// 2 -- Count inside number
// 3 -- Assign inside tags to m_nVertex
// 4 -- Create Vertex
// 5 -- Create Face
// 6 -- Push skeleton elements
//
// 256 -- Count inside skeleton number
// 256 + 1 -- simple-witness check
//////////////////////////////////////////////////////////////////////////

void OcTree::Traversal()
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
	RecTraversalCell(m_pInfiniteNode, m_n64Infinite, -1);
	RecTraversalCell(m_pRoot, 0, 0);
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
		RecTraversalFace(pNodeFace, n64Face, nFace, 0);
		RecTraversalFace(pNodeFace, n64Face, nFace, 1);
		RecTraversalFace(pNodeFace, n64Face, nFace, 2);
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
				RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 0);
				RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 1);
				RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 2);
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
							RecTraversalPoint(pNodePoint, n64Point, nPoint);
				}
}

void OcTree::RecTraversalCell(UCHAR * pNode, UINT64 n64Position, int nLevel)
{
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
					RecTraversalCell(pNodeCell, n64Cell, nCell);
				}
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
					{
						for (int k = 0; k < 2; k++) {
							pNodeFace[k] = pNodePoint[k][i][j];
							n64Face[k] = n64Point[k][i][j];
							nFace[k] = nPoint[k][i][j];
						}
						RecTraversalFace(pNodeFace, n64Face, nFace, 0);
						for (int k = 0; k < 2; k++) {
							pNodeFace[k] = pNodePoint[j][k][i];
							n64Face[k] = n64Point[j][k][i];
							nFace[k] = nPoint[j][k][i];
						}
						RecTraversalFace(pNodeFace, n64Face, nFace, 1);
						for (int k = 0; k < 2; k++) {
							pNodeFace[k] = pNodePoint[i][j][k];
							n64Face[k] = n64Point[i][j][k];
							nFace[k] = nPoint[i][j][k];
						}
						RecTraversalFace(pNodeFace, n64Face, nFace, 2);
					}
					for (int i = 0; i < 2; i++)
					{
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[i][j][k];
								n64Edge[j][k] = n64Point[i][j][k];
								nEdge[j][k] = nPoint[i][j][k];
							}
							RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 0);
							for (int j = 0; j < 2; j++)
								for (int k = 0; k < 2; k++) {
									pNodeEdge[j][k] = pNodePoint[k][i][j];
									n64Edge[j][k] = n64Point[k][i][j];
									nEdge[j][k] = nPoint[k][i][j];
								}
								RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 1);
								for (int j = 0; j < 2; j++)
									for (int k = 0; k < 2; k++) {
										pNodeEdge[j][k] = pNodePoint[j][k][i];
										n64Edge[j][k] = n64Point[j][k][i];
										nEdge[j][k] = nPoint[j][k][i];
									}
									RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 2);
					}
					RecTraversalPoint(pNodePoint, n64Point, nPoint);
	} else {
		// LeafNode * pThisNode;
		Element * pThisCell;
		switch(m_nType) {
		case 6:
			pThisCell = GetElementCell( pNode );
			if ( !pThisCell->m_bSketch && pThisCell->m_bSkeleton ) {
				SkeletonElement element;
				element.m_n64Size = 0;
				element.m_n64Position = n64Position;
				element.m_nLevel = nLevel;
				element.m_nDimension = 3;
				element.m_nDir = 0;
				element.m_pElement = pThisCell;
				element.m_listCompatible.clear();
				element.m_bValid = false;
				if ( pThisCell->m_bObject ) {
					m_listSkeleton.push_back(element);
				} else {
					m_listDualSkeleton.push_back(element);
				}
			}
			break;
		}
	}
}

void OcTree::RecTraversalFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace)
{
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
					RecTraversalFace(pNodeFace, n64Face, nFace, 0);
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++) {
							pNodeEdge[j][k] = pNodePoint[k][i][j];
							n64Edge[j][k] = n64Point[k][i][j];
							nEdge[j][k] = nPoint[k][i][j];
						}
						RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 1);
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[j][k][i];
								n64Edge[j][k] = n64Point[j][k][i];
								nEdge[j][k] = nPoint[j][k][i];
							}
							RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 2);
				}
				RecTraversalPoint(pNodePoint, n64Point, nPoint);
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
					RecTraversalFace(pNodeFace, n64Face, nFace, 1);
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++) {
							pNodeEdge[j][k] = pNodePoint[j][k][i];
							n64Edge[j][k] = n64Point[j][k][i];
							nEdge[j][k] = nPoint[j][k][i];
						}
						RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 2);
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[i][j][k];
								n64Edge[j][k] = n64Point[i][j][k];
								nEdge[j][k] = nPoint[i][j][k];
							}
							RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 0);
				}
				RecTraversalPoint(pNodePoint, n64Point, nPoint);
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
					RecTraversalFace(pNodeFace, n64Face, nFace, 2);
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++) {
							pNodeEdge[j][k] = pNodePoint[i][j][k];
							n64Edge[j][k] = n64Point[i][j][k];
							nEdge[j][k] = nPoint[i][j][k];
						}
						RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 0);
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[k][i][j];
								n64Edge[j][k] = n64Point[k][i][j];
								nEdge[j][k] = nPoint[k][i][j];
							}
							RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 1);
				}
				RecTraversalPoint(pNodePoint, n64Point, nPoint);
				break;
		}
	} else {
		int nI, nIndex;
		// LeafNode * pThisNode;
		Element * pThisFace;
		switch(m_nType) {
		case 5:
			pThisFace = GetElementFace(pNode, nLevel, dirFace, nI, nIndex);
			if (pThisFace->m_bObject) {
				for (int i = 0; i < 2; i++)
					if (!GetElementCell(pNode[i])->m_bObject) {
						int dirEdge;
						list<int> listIndex;
						UINT64 n64Temp;
						UCHAR * pEdge[2][2];
						UINT64 n64Edge[2][2];
						int nEdge[2][2];

						GetFacePosition(n64Position, nLevel, dirFace, nI, 0, n64Temp);
						dirEdge = (dirFace + 2 - i) % 3;
						GetEdgeFromPosition(n64Temp, nLevel[nI], dirEdge, pEdge, n64Edge, nEdge);
						RecContouringEdge(pNode[i], n64Position[i], nLevel[i], pEdge, n64Edge, nEdge, dirEdge, 0, listIndex);

						GetFacePosition(n64Position, nLevel, dirFace, nI, 1+i, n64Temp);
						dirEdge = (dirFace + 1 + i) % 3;
						GetEdgeFromPosition(n64Temp, nLevel[nI], dirEdge, pEdge, n64Edge, nEdge);
						RecContouringEdge(pNode[i], n64Position[i], nLevel[i], pEdge, n64Edge, nEdge, dirEdge, 0, listIndex);

						GetFacePosition(n64Position, nLevel, dirFace, nI, 2-i, n64Temp);
						dirEdge = (dirFace + 2 - i) % 3;
						GetEdgeFromPosition(n64Temp, nLevel[nI], dirEdge, pEdge, n64Edge, nEdge);
						RecContouringEdge(pNode[i], n64Position[i], nLevel[i], pEdge, n64Edge, nEdge, dirEdge, 1, listIndex);

						GetFacePosition(n64Position, nLevel, dirFace, nI, 0, n64Temp);
						dirEdge = (dirFace + 1 + i) % 3;
						GetEdgeFromPosition(n64Temp, nLevel[nI], dirEdge, pEdge, n64Edge, nEdge);
						RecContouringEdge(pNode[i], n64Position[i], nLevel[i], pEdge, n64Edge, nEdge, dirEdge, 1, listIndex);

						if (listIndex.size() >= 3) {
							if ( m_nContourPass == 2 )
								m_cWriter.CountFace(listIndex, m_nFaceNum);
							else
								m_cWriter.WriteFace(listIndex, m_nFaceNum);
						}
					}
			}
			break;
		case 6:
			pThisFace = GetElementFace( pNode, nLevel, dirFace, nI, nIndex );
			if ( !pThisFace->m_bSketch && pThisFace->m_bSkeleton ) {
				SkeletonElement element;
				element.m_n64Size = 0;
				element.m_n64Position = LeafPosition(n64Position[nI], nLevel[nI], Constant::I_SHIFT_FP[Constant::II_SHIFT_CF[dirFace][nI]][0]);
				element.m_nLevel = nLevel[nI];
				element.m_nDimension = 2;
				element.m_nDir = dirFace;
				element.m_pElement = pThisFace;
				element.m_listCompatible.clear();
				element.m_bValid = false;
				if ( pThisFace->m_bObject ) {
					m_listSkeleton.push_back(element);
				} else {
					m_listDualSkeleton.push_back(element);
				}
			}
			break;
		}
	}
}

void OcTree::RecTraversalEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge)
{
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
					RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 0);
			}
			RecTraversalPoint(pNodePoint, n64Point, nPoint);
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
					RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 1);
			}
			RecTraversalPoint(pNodePoint, n64Point, nPoint);
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
					RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 2);
			}
			RecTraversalPoint(pNodePoint, n64Point, nPoint);
			break;
		}
	} else {
		int nI, nJ, nIndex;
		// LeafNode * pThisNode;
		Element * pThisEdge;
		switch(m_nType) {
		case 5:
			pThisEdge = GetElementEdge(pNode, nLevel, dirEdge, nI, nJ, nIndex);
			if (pThisEdge->m_bObject) {
				UCHAR * pFace[2];
				UINT64 n64Face[2];
				int nFace[2];
				int dirFace;
				list<int> listIndex;
				UINT64 n64ThisEdge[2];
				GetEdgePosition(n64Position, nLevel, dirEdge, nI, nJ, 0, n64ThisEdge[0]);
				GetEdgePosition(n64Position, nLevel, dirEdge, nI, nJ, 1, n64ThisEdge[1]);
				for (int i = 0; i < 2; i++)
				{
					listIndex.clear();
					for (int j = 0; j < 2; j++)
					{
						pFace[j] = pNode[i][j];
						n64Face[j] = n64Position[i][j];
						nFace[j] = nLevel[i][j];
					}
					dirFace = (dirEdge + 2) % 3;
					if (CheckStorageFace(pFace) && !GetElementFace(pFace, nFace, dirFace)->m_bObject) {
						InsertIndex(listIndex, GetIndex(pFace[i], n64Face[i], n64ThisEdge[0]));
						InsertIndex(listIndex, GetIndex(pFace[i], n64Face[i], n64ThisEdge[1]));
						InsertIndex(listIndex, GetIndex(pFace[1-i], n64Face[1-i], n64ThisEdge[1]));
						InsertIndex(listIndex, GetIndex(pFace[1-i], n64Face[1-i], n64ThisEdge[0]));
						if (listIndex.size() >= 3) {
							if ( m_nContourPass == 2 )
								m_cWriter.CountFace(listIndex, m_nFaceNum);
							else
								m_cWriter.WriteFace(listIndex, m_nFaceNum);
						}
					}

					listIndex.clear();
					for (int j = 0; j < 2; j++)
					{
						pFace[j] = pNode[j][i];
						n64Face[j] = n64Position[j][i];
						nFace[j] = nLevel[j][i];
					}
					dirFace = (dirEdge + 1) % 3;
					if (CheckStorageFace(pFace) && !GetElementFace(pFace, nFace, dirFace)->m_bObject) {
						InsertIndex(listIndex, GetIndex(pFace[1-i], n64Face[1-i], n64ThisEdge[0]));
						InsertIndex(listIndex, GetIndex(pFace[1-i], n64Face[1-i], n64ThisEdge[1]));
						InsertIndex(listIndex, GetIndex(pFace[i], n64Face[i], n64ThisEdge[1]));
						InsertIndex(listIndex, GetIndex(pFace[i], n64Face[i], n64ThisEdge[0]));
						if (listIndex.size() >= 3) {
							if ( m_nContourPass == 2 )
								m_cWriter.CountFace(listIndex, m_nFaceNum);
							else
								m_cWriter.WriteFace(listIndex, m_nFaceNum);
						}
					}
				}
			}
			break;
		case 6:
			pThisEdge = GetElementEdge( pNode, nLevel, dirEdge, nI, nJ, nIndex );
			if ( !pThisEdge->m_bSketch && pThisEdge->m_bSkeleton ) {
				SkeletonElement element;
				element.m_n64Size = 0;
				element.m_n64Position = LeafPosition(n64Position[nI][nJ], nLevel[nI][nJ], Constant::I_SHIFT_EP[Constant::II_SHIFT_CE[dirEdge][nI][nJ]][0]);
				element.m_nLevel = nLevel[nI][nJ];
				element.m_nDimension = 1;
				element.m_nDir = dirEdge;
				element.m_pElement = pThisEdge;
				element.m_listCompatible.clear();
				element.m_bValid = false;
				if ( pThisEdge->m_bObject ) {
					m_listSkeleton.push_back(element);
				} else {
					m_listDualSkeleton.push_back(element);
				}
			}
			break;
		}
	}
}

void OcTree::RecTraversalPoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2])
{
	if ((*(pNode[0][0][0]) & 0x3) == 0 || (*(pNode[0][0][1]) & 0x3) == 0 || (*(pNode[0][1][0]) & 0x3) == 0 || (*(pNode[0][1][1]) & 0x3) == 0 || 
		(*(pNode[1][0][0]) & 0x3) == 0 || (*(pNode[1][0][1]) & 0x3) == 0 || (*(pNode[1][1][0]) & 0x3) == 0 || (*(pNode[1][1][1]) & 0x3) == 0) {
			UCHAR * pNodePoint[2][2][2];
			UINT64 n64Point[2][2][2];
			int nPoint[2][2][2];
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[i][j][k], n64Position[i][j][k], nLevel[i][j][k], 1-i, 1-j, 1-k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
			RecTraversalPoint(pNodePoint, n64Point, nPoint);
	} else {
		int nI, nJ, nK, nIndex;
		// LeafNode * pThisNode;
		Element * pThisPoint;
		switch(m_nType) {
		case 4:
			pThisPoint = GetElementPoint( pNode, nLevel, nI, nJ, nK, nIndex );
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++) {
						LeafNode * pItNode = (LeafNode *)pNode[i][j][k];
						Element * pItCell = (Element *)(pNode[i][j][k] + sizeof(LeafNode));
						if (pThisPoint->m_bObject && pItCell->m_bObject == 0 && IS_CELL_IRREGULAR(pItNode->m_nVertex)) {
							UINT64 n64ThisPoint = n64Position[nI][nJ][nK] + INT64TRANS((1-nI)*NLENGTH(nLevel[nI][nJ][nK]), (1-nJ)*NLENGTH(nLevel[nI][nJ][nK]), (1-nK)*NLENGTH(nLevel[nI][nJ][nK]));
							bool bVertex = false;
							if (nLevel[i][j][k] == m_nMaxLevel) {
								bVertex = true;
							} else {
								int l;
								int iIndex = i * 4 + j * 2 + k;
								for (l = 0; l < iIndex; l++)
									if (pNode[i][j][k] == pNode[(l >> 2) & 1][(l >> 1) & 1][l & 1])
										break;
								if (l == iIndex) {
									bVertex = true;
								}
							}
							if (bVertex) {
								if ( m_nContourPass == 2 ) {
									m_pHashMap->Set(n64Position[i][j][k], n64ThisPoint, m_nVertexNum);
								} else {
									float fPos[3];
									if (pThisPoint->m_bSketch == 1 && pThisPoint->m_cSimpleTag == 1) {
										fPos[0] = (float)(m_dbNodeLength * (INT64GETI(n64ThisPoint) + SKELETON_WIDTH_DOUBLE * (i*2-1) * NLENGTH(m_nMaxLevel + 1)) + m_vCorner.pVec[0]);
										fPos[1] = (float)(m_dbNodeLength * (INT64GETJ(n64ThisPoint) + SKELETON_WIDTH_DOUBLE * (j*2-1) * NLENGTH(m_nMaxLevel + 1)) + m_vCorner.pVec[1]);
										fPos[2] = (float)(m_dbNodeLength * (INT64GETK(n64ThisPoint) + SKELETON_WIDTH_DOUBLE * (k*2-1) * NLENGTH(m_nMaxLevel + 1)) + m_vCorner.pVec[2]);
#ifdef LOGFILE_OUTPUT
										m_cWriter.WriteLogInt(m_nVertexNum);
#endif
									} else if ((*pNode[i][j][k] & 0x3) == 2) {
										CVector3D vCell = m_pSingleHashMap->Get((UINT64)pNode[i][j][k]);
										if (vCell.pVec[0] != 1e300 && !GET_CHANGED(pNode[i][j][k])) {
#ifdef BETTER_APPEARANCE
											fPos[0] = (float)(0.05 * (m_dbNodeLength * (INT64GETI(n64ThisPoint)) + m_vCorner.pVec[0]) + 0.95 * vCell.pVec[0]);
											fPos[1] = (float)(0.05 * (m_dbNodeLength * (INT64GETJ(n64ThisPoint)) + m_vCorner.pVec[1]) + 0.95 * vCell.pVec[1]);
											fPos[2] = (float)(0.05 * (m_dbNodeLength * (INT64GETK(n64ThisPoint)) + m_vCorner.pVec[2]) + 0.95 * vCell.pVec[2]);
#else
											fPos[0] = (float)(0.5 * (m_dbNodeLength * (INT64GETI(n64ThisPoint)) + m_vCorner.pVec[0]) + 0.5 * vCell.pVec[0]);
											fPos[1] = (float)(0.5 * (m_dbNodeLength * (INT64GETJ(n64ThisPoint)) + m_vCorner.pVec[1]) + 0.5 * vCell.pVec[1]);
											fPos[2] = (float)(0.5 * (m_dbNodeLength * (INT64GETK(n64ThisPoint)) + m_vCorner.pVec[2]) + 0.5 * vCell.pVec[2]);
#endif
#ifdef LOGFILE_OUTPUT
											m_cWriter.WriteLogInt(m_nVertexNum);
#endif
										} else {
											fPos[0] = (float)(0.5 * m_dbNodeLength * (INT64GETI(n64ThisPoint) + INT64GETI(n64Position[i][j][k]) + NLENGTH(nLevel[i][j][k] + 1)) + m_vCorner.pVec[0]);
											fPos[1] = (float)(0.5 * m_dbNodeLength * (INT64GETJ(n64ThisPoint) + INT64GETJ(n64Position[i][j][k]) + NLENGTH(nLevel[i][j][k] + 1)) + m_vCorner.pVec[1]);
											fPos[2] = (float)(0.5 * m_dbNodeLength * (INT64GETK(n64ThisPoint) + INT64GETK(n64Position[i][j][k]) + NLENGTH(nLevel[i][j][k] + 1)) + m_vCorner.pVec[2]);
#ifdef LOGFILE_OUTPUT
											m_cWriter.WriteLogInt(m_nVertexNum);
#endif
										}
									} else {
										fPos[0] = (float)(0.5 * m_dbNodeLength * (INT64GETI(n64ThisPoint) + INT64GETI(n64Position[i][j][k]) + NLENGTH(nLevel[i][j][k] + 1)) + m_vCorner.pVec[0]);
										fPos[1] = (float)(0.5 * m_dbNodeLength * (INT64GETJ(n64ThisPoint) + INT64GETJ(n64Position[i][j][k]) + NLENGTH(nLevel[i][j][k] + 1)) + m_vCorner.pVec[1]);
										fPos[2] = (float)(0.5 * m_dbNodeLength * (INT64GETK(n64ThisPoint) + INT64GETK(n64Position[i][j][k]) + NLENGTH(nLevel[i][j][k] + 1)) + m_vCorner.pVec[2]);
#ifdef LOGFILE_OUTPUT
										m_cWriter.WriteLogInt(m_nVertexNum);
#endif
									}
									m_cWriter.WriteVertex(fPos);
								}
								m_nIrregularCount++;
								m_nVertexNum ++;
							}
						}
					}
					break;
		case 5:
			pThisPoint = GetElementPoint( pNode, nLevel, nI, nJ, nK, nIndex );
			if (pThisPoint->m_bObject) {
				UCHAR * pEdge[2][2];
				UINT64 n64Edge[2][2];
				int nEdge[2][2];
				int dirEdge;
				list<int> listIndex;
				UINT64 n64ThisPoint;
				GetPointPosition(n64Position, nLevel, nI, nJ, nK, n64ThisPoint);

				for (int i = 0; i < 2; i++)
				{
					listIndex.clear();
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pEdge[j][k] = pNode[i][j][k];
							n64Edge[j][k] = n64Position[i][j][k];
							nEdge[j][k] = nLevel[i][j][k];
						}
						dirEdge = 0;
						if (CheckStorageEdge(pEdge) && !GetElementEdge(pEdge, nEdge, dirEdge)->m_bObject) {
							InsertIndex(listIndex, GetIndex(pEdge[0][0], n64Edge[0][0], n64ThisPoint));
							InsertIndex(listIndex, GetIndex(pEdge[i][1-i], n64Edge[i][1-i], n64ThisPoint));
							InsertIndex(listIndex, GetIndex(pEdge[1][1], n64Edge[1][1], n64ThisPoint));
							InsertIndex(listIndex, GetIndex(pEdge[1-i][i], n64Edge[1-i][i], n64ThisPoint));
							if (listIndex.size() >= 3) {
								if ( m_nContourPass == 2 )
									m_cWriter.CountFace(listIndex, m_nFaceNum);
								else
									m_cWriter.WriteFace(listIndex, m_nFaceNum);
							}
						}

						listIndex.clear();
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++)
							{
								pEdge[j][k] = pNode[k][i][j];
								n64Edge[j][k] = n64Position[k][i][j];
								nEdge[j][k] = nLevel[k][i][j];
							}
							dirEdge = 1;
							if (CheckStorageEdge(pEdge) && !GetElementEdge(pEdge, nEdge, dirEdge)->m_bObject) {
								InsertIndex(listIndex, GetIndex(pEdge[0][0], n64Edge[0][0], n64ThisPoint));
								InsertIndex(listIndex, GetIndex(pEdge[i][1-i], n64Edge[i][1-i], n64ThisPoint));
								InsertIndex(listIndex, GetIndex(pEdge[1][1], n64Edge[1][1], n64ThisPoint));
								InsertIndex(listIndex, GetIndex(pEdge[1-i][i], n64Edge[1-i][i], n64ThisPoint));
								if (listIndex.size() >= 3) {
									if ( m_nContourPass == 2 )
										m_cWriter.CountFace(listIndex, m_nFaceNum);
									else
										m_cWriter.WriteFace(listIndex, m_nFaceNum);
								}
							}

							listIndex.clear();
							for (int j = 0; j < 2; j++)
								for (int k = 0; k < 2; k++)
								{
									pEdge[j][k] = pNode[j][k][i];
									n64Edge[j][k] = n64Position[j][k][i];
									nEdge[j][k] = nLevel[j][k][i];
								}
								dirEdge = 2;
								if (CheckStorageEdge(pEdge) && !GetElementEdge(pEdge, nEdge, dirEdge)->m_bObject) {
									InsertIndex(listIndex, GetIndex(pEdge[0][0], n64Edge[0][0], n64ThisPoint));
									InsertIndex(listIndex, GetIndex(pEdge[i][1-i], n64Edge[i][1-i], n64ThisPoint));
									InsertIndex(listIndex, GetIndex(pEdge[1][1], n64Edge[1][1], n64ThisPoint));
									InsertIndex(listIndex, GetIndex(pEdge[1-i][i], n64Edge[1-i][i], n64ThisPoint));
									if (listIndex.size() >= 3) {
										if ( m_nContourPass == 2 )
											m_cWriter.CountFace(listIndex, m_nFaceNum);
										else
											m_cWriter.WriteFace(listIndex, m_nFaceNum);
									}
								}
				}
			}
			break;
		case 6:
			pThisPoint = GetElementPoint( pNode, nLevel, nI, nJ, nK, nIndex );
			if ( !pThisPoint->m_bSketch && pThisPoint->m_bSkeleton ) {
				SkeletonElement element;
				element.m_n64Size = 0;
				element.m_n64Position = LeafPosition(n64Position[nI][nJ][nK], nLevel[nI][nJ][nK], Constant::II_SHIFT_CP[nI][nJ][nK]);
				element.m_nLevel = nLevel[nI][nJ][nK];
				element.m_nDimension = 0;
				element.m_nDir = 0;
				element.m_pElement = pThisPoint;
				element.m_listCompatible.clear();
				element.m_bValid = false;
				if ( pThisPoint->m_bObject ) {
					m_listSkeleton.push_back(element);
				} else {
					m_listDualSkeleton.push_back(element);
				}
			}
			break;
		}
	}
}

void OcTree::TraversalWithoutPosition()
{
	UCHAR * pNodeFace[2];
	UCHAR * pNodeEdge[2][2];
	UCHAR * pNodePoint[2][2][2];
	int nFace[2];
	int nEdge[2][2];
	int nPoint[2][2][2];
	RecTraversalCell(m_pInfiniteNode, -1);
	RecTraversalCell(m_pRoot, 0);
	for (int i = 0; i < 2; i++)
	{
		for (int ii = 0; ii < 2; ii++) {
			pNodeFace[ii] = m_pInfiniteNode;
			nFace[ii] = -1;
		}
		pNodeFace[i] = m_pRoot;
		nFace[i] = 0;
		RecTraversalFace(pNodeFace, nFace, 0);
		RecTraversalFace(pNodeFace, nFace, 1);
		RecTraversalFace(pNodeFace, nFace, 2);
	}
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
		{
			for (int ii = 0; ii < 2; ii++)
				for (int jj = 0; jj < 2; jj++) {
					pNodeEdge[ii][jj] = m_pInfiniteNode;
					nEdge[ii][jj] = -1;
				}
				pNodeEdge[i][j] = m_pRoot;
				nEdge[i][j] = 0;
				RecTraversalEdge(pNodeEdge, nEdge, 0);
				RecTraversalEdge(pNodeEdge, nEdge, 1);
				RecTraversalEdge(pNodeEdge, nEdge, 2);
		}
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					for (int ii = 0; ii < 2; ii++)
						for (int jj = 0; jj < 2; jj++)
							for (int kk = 0; kk < 2; kk++) {
								pNodePoint[ii][jj][kk] = m_pInfiniteNode;
								nPoint[ii][jj][kk] = -1;
							}
							pNodePoint[i][j][k] = m_pRoot;
							nPoint[i][j][k] = 0;
							RecTraversalPoint(pNodePoint, nPoint);
				}
}

void OcTree::RecTraversalCell(UCHAR * pNode, int nLevel)
{
	if (NOT_LEAF(pNode)) {
		UCHAR * pNodeCell;
		UCHAR * pNodeFace[2];
		UCHAR * pNodeEdge[2][2];
		UCHAR * pNodePoint[2][2][2];
		int nCell;
		int nFace[2];
		int nEdge[2][2];
		int nPoint[2][2][2];
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					Leaf(pNode, nLevel, i, j, k, pNodePoint[i][j][k], nPoint[i][j][k]);
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++) {
					pNodeCell = pNodePoint[i][j][k];
					nCell = nPoint[i][j][k];
					RecTraversalCell(pNodeCell, nCell);
				}
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
					{
						for (int k = 0; k < 2; k++) {
							pNodeFace[k] = pNodePoint[k][i][j];
							nFace[k] = nPoint[k][i][j];
						}
						RecTraversalFace(pNodeFace, nFace, 0);
						for (int k = 0; k < 2; k++) {
							pNodeFace[k] = pNodePoint[j][k][i];
							nFace[k] = nPoint[j][k][i];
						}
						RecTraversalFace(pNodeFace, nFace, 1);
						for (int k = 0; k < 2; k++) {
							pNodeFace[k] = pNodePoint[i][j][k];
							nFace[k] = nPoint[i][j][k];
						}
						RecTraversalFace(pNodeFace, nFace, 2);
					}
					for (int i = 0; i < 2; i++)
					{
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[i][j][k];
								nEdge[j][k] = nPoint[i][j][k];
							}
							RecTraversalEdge(pNodeEdge, nEdge, 0);
							for (int j = 0; j < 2; j++)
								for (int k = 0; k < 2; k++) {
									pNodeEdge[j][k] = pNodePoint[k][i][j];
									nEdge[j][k] = nPoint[k][i][j];
								}
								RecTraversalEdge(pNodeEdge, nEdge, 1);
								for (int j = 0; j < 2; j++)
									for (int k = 0; k < 2; k++) {
										pNodeEdge[j][k] = pNodePoint[j][k][i];
										nEdge[j][k] = nPoint[j][k][i];
									}
									RecTraversalEdge(pNodeEdge, nEdge, 2);
					}
					RecTraversalPoint(pNodePoint, nPoint);
	} else {
		LeafNode * pThisNode;
		Element * pThisCell;
		switch(m_nType) {
		case 0:
			pThisNode = (LeafNode *)pNode;
			SET_STORAGE_TAG(pThisNode->m_nType, 0);
			break;
		case 1:
			m_nCountCell++;
			break;
		case 2:
			pThisCell = GetElementCell( pNode );
			if (pThisCell->m_bObject)
				m_nCountCell++;
			break;
		case 3:
			pThisCell = GetElementCell( pNode );
			pThisNode = (LeafNode *)pNode;
			if (pThisCell->m_bObject)
				SET_INSIDE_TAG( pThisNode->m_nVertex, 0 );
			break;
		case 256:
			pThisCell = GetElementCell( pNode );
			if (pThisCell->m_bObject && pThisCell->m_bSkeleton)
				m_nCountCell++;
			break;
		}
	}
}

void OcTree::RecTraversalFace(UCHAR * pNode[2], int nLevel[2], int dirFace)
{
	if ((*(pNode[0]) & 0x3) == 0 || (*(pNode[1]) & 0x3) == 0) {
		UCHAR * pNodeFace[2];
		UCHAR * pNodeEdge[2][2];
		UCHAR * pNodePoint[2][2][2];
		int nFace[2];
		int nEdge[2][2];
		int nPoint[2][2][2];
		switch(dirFace) {
		case 0:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[i], nLevel[i], 1-i, j, k, pNodePoint[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++) {
						pNodeFace[k] = pNodePoint[k][i][j];
						nFace[k] = nPoint[k][i][j];
					}
					RecTraversalFace(pNodeFace, nFace, 0);
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++) {
							pNodeEdge[j][k] = pNodePoint[k][i][j];
							nEdge[j][k] = nPoint[k][i][j];
						}
						RecTraversalEdge(pNodeEdge, nEdge, 1);
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[j][k][i];
								nEdge[j][k] = nPoint[j][k][i];
							}
							RecTraversalEdge(pNodeEdge, nEdge, 2);
				}
				RecTraversalPoint(pNodePoint, nPoint);
				break;
		case 1:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j], nLevel[j], i, 1-j, k, pNodePoint[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++) {
						pNodeFace[k] = pNodePoint[j][k][i];
						nFace[k] = nPoint[j][k][i];
					}
					RecTraversalFace(pNodeFace, nFace, 1);
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++) {
							pNodeEdge[j][k] = pNodePoint[j][k][i];
							nEdge[j][k] = nPoint[j][k][i];
						}
						RecTraversalEdge(pNodeEdge, nEdge, 2);
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[i][j][k];
								nEdge[j][k] = nPoint[i][j][k];
							}
							RecTraversalEdge(pNodeEdge, nEdge, 0);
				}
				RecTraversalPoint(pNodePoint, nPoint);
				break;
		case 2:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k], nLevel[k], i, j, 1-k, pNodePoint[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++) {
						pNodeFace[k] = pNodePoint[i][j][k];
						nFace[k] = nPoint[i][j][k];
					}
					RecTraversalFace(pNodeFace, nFace, 2);
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++) {
							pNodeEdge[j][k] = pNodePoint[i][j][k];
							nEdge[j][k] = nPoint[i][j][k];
						}
						RecTraversalEdge(pNodeEdge, nEdge, 0);
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[k][i][j];
								nEdge[j][k] = nPoint[k][i][j];
							}
							RecTraversalEdge(pNodeEdge, nEdge, 1);
				}
				RecTraversalPoint(pNodePoint, nPoint);
				break;
		}
	} else {
		int nI, nIndex;
		LeafNode * pThisNode;
		Element * pThisFace;
		switch(m_nType) {
		case 0:
			GetStorageFace(nLevel, dirFace, nI, nIndex);
			pThisNode = (LeafNode *)pNode[nI];
			SET_STORAGE_TAG(pThisNode->m_nType, 1 + nIndex);
			break;
		case 1:
			m_nCountFace++;
			break;
		case 2:
			pThisFace = GetElementFace( pNode, nLevel, dirFace, nI, nIndex );
			if (pThisFace->m_bObject)
				m_nCountFace++;
			break;
		case 3:
			pThisFace = GetElementFace( pNode, nLevel, dirFace, nI, nIndex );
			if (pThisFace->m_bObject) {
				for (int i = 0; i < 2; i++)
					if (nLevel[i] == nLevel[nI])
						SET_INSIDE_TAG( ((LeafNode *)pNode[i])->m_nVertex, dirFace * 2 + (1 - i) + 1 );
			}
			break;
		case 256:
			pThisFace = GetElementFace( pNode, nLevel, dirFace, nI, nIndex );
			if (pThisFace->m_bObject && pThisFace->m_bSkeleton)
				m_nCountFace++;
			break;
		case 256 + 1:
			pThisFace = GetElementFace( pNode, nLevel, dirFace, nI, nIndex );
			if ( !pThisFace->m_bSkeleton && pThisFace->m_cPointer < 2 ) {
				int i = pThisFace->m_cPointer;
				Element * pTempCell = GetElementCell(pNode[i]);
				if (pTempCell->m_bSkeleton || pTempCell->m_cPointer != dirFace * 2 + (1 - i))
					m_bCorrect = false;
			}	
			break;
		}
	}
}

void OcTree::RecTraversalEdge(UCHAR * pNode[2][2], int nLevel[2][2], int dirEdge)
{
	if ((*(pNode[0][0]) & 0x3) == 0 || (*(pNode[0][1]) & 0x3) == 0 || (*(pNode[1][0]) & 0x3) == 0 || (*(pNode[1][1]) & 0x3) == 0) {
		UCHAR * pNodeEdge[2][2];
		UCHAR * pNodePoint[2][2][2];
		int nEdge[2][2];
		int nPoint[2][2][2];
		switch(dirEdge) {
		case 0:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j][k], nLevel[j][k], i, 1-j, 1-k, pNodePoint[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++) {
						pNodeEdge[j][k] = pNodePoint[i][j][k];
						nEdge[j][k] = nPoint[i][j][k];
					}
					RecTraversalEdge(pNodeEdge, nEdge, 0);
			}
			RecTraversalPoint(pNodePoint, nPoint);
			break;
		case 1:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k][i], nLevel[k][i], 1-i, j, 1-k, pNodePoint[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++) {
						pNodeEdge[j][k] = pNodePoint[k][i][j];
						nEdge[j][k] = nPoint[k][i][j];
					}
					RecTraversalEdge(pNodeEdge, nEdge, 1);
			}
			RecTraversalPoint(pNodePoint, nPoint);
			break;
		case 2:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[i][j], nLevel[i][j], 1-i, 1-j, k, pNodePoint[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++) {
						pNodeEdge[j][k] = pNodePoint[j][k][i];
						nEdge[j][k] = nPoint[j][k][i];
					}
					RecTraversalEdge(pNodeEdge, nEdge, 2);
			}
			RecTraversalPoint(pNodePoint, nPoint);
			break;
		}
	} else {
		int nI, nJ, nIndex;
		LeafNode * pThisNode;
		Element * pThisEdge;
		switch(m_nType) {
		case 0:
			GetStorageEdge(nLevel, dirEdge, nI, nJ, nIndex);
			pThisNode = (LeafNode *)pNode[nI][nJ];
			SET_STORAGE_TAG(pThisNode->m_nType, 1 + 6 + nIndex);
			break;
		case 1:
			m_nCountEdge++;
			break;
		case 2:
			pThisEdge = GetElementEdge( pNode, nLevel, dirEdge, nI, nJ, nIndex );
			if (pThisEdge->m_bObject)
				m_nCountEdge++;
			break;
		case 3:
			pThisEdge = GetElementEdge( pNode, nLevel, dirEdge, nI, nJ, nIndex );
			if (pThisEdge->m_bObject) {
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
						if (nLevel[i][j] == nLevel[nI][nJ])
							SET_INSIDE_TAG( ((LeafNode *)pNode[i][j])->m_nVertex, dirEdge * 4 + (1 - i) * 2 + (1 - j) + 1 + 6 );
			}
			break;
		case 256:
			pThisEdge = GetElementEdge( pNode, nLevel, dirEdge, nI, nJ, nIndex );
			if (pThisEdge->m_bObject && pThisEdge->m_bSkeleton)
				m_nCountEdge++;
			break;
		case 256 + 1:
			pThisEdge = GetElementEdge( pNode, nLevel, dirEdge, nI, nJ, nIndex );
			if ( !pThisEdge->m_bSkeleton && pThisEdge->m_cPointer < 4 ) {
				UCHAR * pFace[2];
				int nFace[2];
				int dirFace;
				int c = pThisEdge->m_cPointer / 2;
				int i = pThisEdge->m_cPointer % 2;
				if (c == 0) {
					for (int j = 0; j < 2; j++) {
						pFace[j] = pNode[i][j];
						nFace[j] = nLevel[i][j];
					}
					dirFace = (dirEdge + 2) % 3;
				} else {
					for (int j = 0; j < 2; j++) {
						pFace[j] = pNode[j][i];
						nFace[j] = nLevel[j][i];
					}
					dirFace = (dirEdge + 1) % 3;
				}
				if (CheckStorageFace(pFace)) {
					Element * pTempFace = GetElementFace(pFace, nFace, dirFace);
					if (pTempFace->m_bSkeleton || pTempFace->m_cPointer != (c * 2) + (1 - i) + 2)
						m_bCorrect = false;
				}
			}	
			break;
		}
	}
}

void OcTree::RecTraversalPoint(UCHAR * pNode[2][2][2], int nLevel[2][2][2])
{
	if ((*(pNode[0][0][0]) & 0x3) == 0 || (*(pNode[0][0][1]) & 0x3) == 0 || (*(pNode[0][1][0]) & 0x3) == 0 || (*(pNode[0][1][1]) & 0x3) == 0 || 
		(*(pNode[1][0][0]) & 0x3) == 0 || (*(pNode[1][0][1]) & 0x3) == 0 || (*(pNode[1][1][0]) & 0x3) == 0 || (*(pNode[1][1][1]) & 0x3) == 0) {
			UCHAR * pNodePoint[2][2][2];
			int nPoint[2][2][2];
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[i][j][k], nLevel[i][j][k], 1-i, 1-j, 1-k, pNodePoint[i][j][k], nPoint[i][j][k]);
			RecTraversalPoint(pNodePoint, nPoint);
	} else {
		int nI, nJ, nK, nIndex;
		LeafNode * pThisNode;
		Element * pThisPoint;
		switch(m_nType) {
		case 0:
			GetStoragePoint(nLevel, nI, nJ, nK, nIndex);
			pThisNode = (LeafNode *)pNode[nI][nJ][nK];
			SET_STORAGE_TAG(pThisNode->m_nType, 1 + 6 + 12 + nIndex);
			break;
		case 1:
			m_nCountPoint++;
			break;
		case 2:
			pThisPoint = GetElementPoint( pNode, nLevel, nI, nJ, nK, nIndex );
			if (pThisPoint->m_bObject)
				m_nCountPoint++;
			break;
		case 3:
			pThisPoint = GetElementPoint( pNode, nLevel, nI, nJ, nK, nIndex );
			if (pThisPoint->m_bObject) {
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
							if (pNode[i][j][k] != pNode[1-i][j][k] && pNode[i][j][k] != pNode[i][1-j][k] && pNode[i][j][k] != pNode[i][j][1-k])
								SET_INSIDE_TAG( ((LeafNode *)pNode[i][j][k])->m_nVertex, (1 - i) * 4 + (1 - j) * 2 + (1 - k) + 1 + 6 + 12 );
			}
			break;
		case 4:
			pThisPoint = GetElementPoint( pNode, nLevel, nI, nJ, nK, nIndex );
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++) {
						LeafNode * pItNode = (LeafNode *)pNode[i][j][k];
						Element * pItCell = (Element *)(pNode[i][j][k] + sizeof(LeafNode));
						if (pThisPoint->m_bObject && pItCell->m_bObject == 0 && IS_CELL_IRREGULAR(pItNode->m_nVertex)) {
							if (nLevel[i][j][k] == m_nMaxLevel) {
								m_nIrregularCount++;
								m_nVertexNum ++;
							} else {
								int iIndex = i * 4 + j * 2 + k;
								int l;
								for (l = 0; l < iIndex; l++)
									if (pNode[i][j][k] == pNode[(l >> 2) & 1][(l >> 1) & 1][l & 1])
										break;
								if (l == iIndex) {
									m_nIrregularCount++;
									m_nVertexNum ++;
								}
							}
						}
					}
					break;
		case 256:
			pThisPoint = GetElementPoint( pNode, nLevel, nI, nJ, nK, nIndex );
			if (pThisPoint->m_bObject && pThisPoint->m_bSkeleton)
				m_nCountPoint++;
			break;
		case 256 + 1:
			pThisPoint = GetElementPoint( pNode, nLevel, nI, nJ, nK, nIndex );
			if ( !pThisPoint->m_bSkeleton ) {
				UCHAR * pEdge[2][2];
				int nEdge[2][2];
				int dirEdge = pThisPoint->m_cPointer / 2;
				int i = pThisPoint->m_cPointer % 2;
				switch (dirEdge) {
		case 0:
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					pEdge[j][k] = pNode[i][j][k];
					nEdge[j][k] = nLevel[i][j][k];
				}
				break;
		case 1:
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					pEdge[j][k] = pNode[k][i][j];
					nEdge[j][k] = nLevel[k][i][j];
				}
				break;
		case 2:
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					pEdge[j][k] = pNode[j][k][i];
					nEdge[j][k] = nLevel[j][k][i];
				}
				break;
				}
				if (CheckStorageEdge(pEdge)) {
					Element * pTempEdge = GetElementEdge(pEdge, nEdge, dirEdge);
					if (pTempEdge->m_bSkeleton || pTempEdge->m_cPointer != 1 - i + 4)
						m_bCorrect = false;
				}
			}	
			break;
		}
	}
}
