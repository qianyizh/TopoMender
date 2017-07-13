#include "OcTree.h"
using namespace std;

//////////////////////////////////////////////////////////////////////////
// containing point functions
//////////////////////////////////////////////////////////////////////////
// COcTree::m_nType, which determines the behaviors done during each traversal.
//
// 32 -- set m_bSketch of all containing elements of a cell
// 33 -- check if a dual element is seperating
// 34 -- generate a dual generating set
// 35 -- set m_bSketch of all object elements of a cell
// 36 -- count all none-sketch element of a cell
// 37 -- set invalid flag for object skeleton elements
// 38 -- push next element for pure thinning
// 39 -- check if a dual element is simple
// 40 -- set vast generating set element invalid
//
// 256 + 32 -- print all the belongings
// 256 + 33 -- count all the containing elements of a cell
//////////////////////////////////////////////////////////////////////////

void OcTree::CheckCell(UINT64 n64CheckCell, int nCheckCell)
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
	RecCheckCellCell(m_pInfiniteNode, m_n64Infinite, -1, n64CheckCell, nCheckCell);
	RecCheckCellCell(m_pRoot, 0, 0, n64CheckCell, nCheckCell);
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
		RecCheckCellFace(pNodeFace, n64Face, nFace, 0, n64CheckCell, nCheckCell);
		RecCheckCellFace(pNodeFace, n64Face, nFace, 1, n64CheckCell, nCheckCell);
		RecCheckCellFace(pNodeFace, n64Face, nFace, 2, n64CheckCell, nCheckCell);
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
				RecCheckCellEdge(pNodeEdge, n64Edge, nEdge, 0, n64CheckCell, nCheckCell);
				RecCheckCellEdge(pNodeEdge, n64Edge, nEdge, 1, n64CheckCell, nCheckCell);
				RecCheckCellEdge(pNodeEdge, n64Edge, nEdge, 2, n64CheckCell, nCheckCell);
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
							RecCheckCellPoint(pNodePoint, n64Point, nPoint, n64CheckCell, nCheckCell);
				}
}

void OcTree::RecCheckCellCell(UCHAR * pNode, UINT64 n64Position, int nLevel, UINT64 n64CheckCell, int nCheckCell)
{
	if (!CheckCellForCell(n64Position, nLevel, n64CheckCell, nCheckCell))
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
					RecCheckCellCell(pNodeCell, n64Cell, nCell, n64CheckCell, nCheckCell);
				}
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
					{
						for (int k = 0; k < 2; k++) {
							pNodeFace[k] = pNodePoint[k][i][j];
							n64Face[k] = n64Point[k][i][j];
							nFace[k] = nPoint[k][i][j];
						}
						RecCheckCellFace(pNodeFace, n64Face, nFace, 0, n64CheckCell, nCheckCell);
						for (int k = 0; k < 2; k++) {
							pNodeFace[k] = pNodePoint[j][k][i];
							n64Face[k] = n64Point[j][k][i];
							nFace[k] = nPoint[j][k][i];
						}
						RecCheckCellFace(pNodeFace, n64Face, nFace, 1, n64CheckCell, nCheckCell);
						for (int k = 0; k < 2; k++) {
							pNodeFace[k] = pNodePoint[i][j][k];
							n64Face[k] = n64Point[i][j][k];
							nFace[k] = nPoint[i][j][k];
						}
						RecCheckCellFace(pNodeFace, n64Face, nFace, 2, n64CheckCell, nCheckCell);
					}
					for (int i = 0; i < 2; i++)
					{
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[i][j][k];
								n64Edge[j][k] = n64Point[i][j][k];
								nEdge[j][k] = nPoint[i][j][k];
							}
							RecCheckCellEdge(pNodeEdge, n64Edge, nEdge, 0, n64CheckCell, nCheckCell);
							for (int j = 0; j < 2; j++)
								for (int k = 0; k < 2; k++) {
									pNodeEdge[j][k] = pNodePoint[k][i][j];
									n64Edge[j][k] = n64Point[k][i][j];
									nEdge[j][k] = nPoint[k][i][j];
								}
								RecCheckCellEdge(pNodeEdge, n64Edge, nEdge, 1, n64CheckCell, nCheckCell);
								for (int j = 0; j < 2; j++)
									for (int k = 0; k < 2; k++) {
										pNodeEdge[j][k] = pNodePoint[j][k][i];
										n64Edge[j][k] = n64Point[j][k][i];
										nEdge[j][k] = nPoint[j][k][i];
									}
									RecCheckCellEdge(pNodeEdge, n64Edge, nEdge, 2, n64CheckCell, nCheckCell);
					}
					RecCheckCellPoint(pNodePoint, n64Point, nPoint, n64CheckCell, nCheckCell);
	} else {
		Element * pThisCell;
		switch (m_nType) {
		case 32:
			pThisCell = GetElementCell(pNode);
			pThisCell->m_bSketch = 1;
			pThisCell->m_bObject = 1;
			break;
		case 35:
			pThisCell = GetElementCell(pNode);
			if (pThisCell->m_bObject)
				pThisCell->m_bSketch = 1;
			break;
		case 36:
			if (((NodeHeader *)pNode)->m_bLastFix == 0)
				m_nCountCell ++;
			break;
		case 256 + 32:
#ifdef CONSOLE_OUTPUT
			printf("Cell : (%16I64x, %d)\n", n64Position, nLevel);
#endif
			break;
		case 256 + 33:
			m_nCountCell++;
			break;
		}
	}
}

void OcTree::RecCheckCellFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, UINT64 n64CheckCell, int nCheckCell)
{
	UINT64 n64ThisFace;
	int nThisFace;
	GetFacePosition(n64Position, nLevel, dirFace, n64ThisFace, nThisFace);
	if (!CheckCellForFace(n64ThisFace, nThisFace, dirFace, n64CheckCell, nCheckCell))
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
					RecCheckCellFace(pNodeFace, n64Face, nFace, 0, n64CheckCell, nCheckCell);
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++) {
							pNodeEdge[j][k] = pNodePoint[k][i][j];
							n64Edge[j][k] = n64Point[k][i][j];
							nEdge[j][k] = nPoint[k][i][j];
						}
						RecCheckCellEdge(pNodeEdge, n64Edge, nEdge, 1, n64CheckCell, nCheckCell);
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[j][k][i];
								n64Edge[j][k] = n64Point[j][k][i];
								nEdge[j][k] = nPoint[j][k][i];
							}
							RecCheckCellEdge(pNodeEdge, n64Edge, nEdge, 2, n64CheckCell, nCheckCell);
				}
				RecCheckCellPoint(pNodePoint, n64Point, nPoint, n64CheckCell, nCheckCell);
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
					RecCheckCellFace(pNodeFace, n64Face, nFace, 1, n64CheckCell, nCheckCell);
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++) {
							pNodeEdge[j][k] = pNodePoint[j][k][i];
							n64Edge[j][k] = n64Point[j][k][i];
							nEdge[j][k] = nPoint[j][k][i];
						}
						RecCheckCellEdge(pNodeEdge, n64Edge, nEdge, 2, n64CheckCell, nCheckCell);
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[i][j][k];
								n64Edge[j][k] = n64Point[i][j][k];
								nEdge[j][k] = nPoint[i][j][k];
							}
							RecCheckCellEdge(pNodeEdge, n64Edge, nEdge, 0, n64CheckCell, nCheckCell);
				}
				RecCheckCellPoint(pNodePoint, n64Point, nPoint, n64CheckCell, nCheckCell);
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
					RecCheckCellFace(pNodeFace, n64Face, nFace, 2, n64CheckCell, nCheckCell);
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++) {
							pNodeEdge[j][k] = pNodePoint[i][j][k];
							n64Edge[j][k] = n64Point[i][j][k];
							nEdge[j][k] = nPoint[i][j][k];
						}
						RecCheckCellEdge(pNodeEdge, n64Edge, nEdge, 0, n64CheckCell, nCheckCell);
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++) {
								pNodeEdge[j][k] = pNodePoint[k][i][j];
								n64Edge[j][k] = n64Point[k][i][j];
								nEdge[j][k] = nPoint[k][i][j];
							}
							RecCheckCellEdge(pNodeEdge, n64Edge, nEdge, 1, n64CheckCell, nCheckCell);
				}
				RecCheckCellPoint(pNodePoint, n64Point, nPoint, n64CheckCell, nCheckCell);
				break;
		}
	} else {
		Element * pThisFace;
		switch (m_nType) {
		case 32:
			pThisFace = GetElementFace(pNode, nLevel, dirFace);
			pThisFace->m_bSketch = 1;
			pThisFace->m_bObject = 1;
			break;
		case 35:
			pThisFace = GetElementFace(pNode, nLevel, dirFace);
			if (pThisFace->m_bObject)
				pThisFace->m_bSketch = 1;
			break;
		case 33:
			pThisFace = GetElementFace(pNode, nLevel, dirFace);
			if ( !pThisFace->m_bObject && pThisFace->m_bSkeleton )
				m_bIsSeperating = false;
			break;
		case 34:
			pThisFace = GetElementFace(pNode, nLevel, dirFace);
			if ( !pThisFace->m_bObject && !pThisFace->m_bSkeleton) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisFace );
				if ( pr.second ) {
					// printf("Face : (%16I64x, %d, %d)\n", n64ThisFace, nThisFace, dirFace);
					if ( !m_bComputeOnly ) {
						m_listChanging.push_back(SkeletonElement(n64ThisFace, nThisFace, dirFace, 2, pThisFace));
					}
					else 
						m_pCurrentSkeletonElement->m_n64Size += GetFaceVolume(nThisFace);
					DualPushWitnessFace(pNode, n64Position, nLevel, dirFace, pThisFace->m_cPointer);
				}
			}
			break;
		case 36:
			if (((NodeHeader *)pNode[0])->m_bLastFix == 0 && ((NodeHeader *)pNode[1])->m_bLastFix == 0)
				m_nCountFace ++;
			break;
		case 37:
			pThisFace = GetElementFace(pNode, nLevel, dirFace);
			if ( pThisFace->m_bObject ) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisFace );
				if ( pr.second ) {
					if ( !pThisFace->m_bSkeleton) {
						//printf("Face : (%16I64x, %d, %d) --> %d\n", n64ThisFace, nThisFace, dirFace, pThisFace->m_cPointer);
						DualPushWitnessFace(pNode, n64Position, nLevel, dirFace, pThisFace->m_cPointer);
					} else {
						hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashSkeleton.find((unsigned int)pThisFace);
						if ( itTemp != m_hashSkeleton.end() ) {
							itTemp->second->m_bValid = false;
						}
					}
				}
			}
			break;
		case 38:
			pThisFace = GetElementFace(pNode, nLevel, dirFace);
			if ( pThisFace->m_bObject && pThisFace->m_bSkeleton && !pThisFace->m_bSketch ) {
				m_listGeneratingSet.push_back(SkeletonElement(n64ThisFace, nThisFace, dirFace, 2, pThisFace));
			}
			break;
		case 39:
			pThisFace = GetElementFace(pNode, nLevel, dirFace);
			if ( !pThisFace->m_bObject && pThisFace->m_bSkeleton ) {
				m_nBelong ++;
				if (m_nBelong == 1) {
					hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashDualSkeleton.find((unsigned int)pThisFace);
					if ( itTemp != m_hashDualSkeleton.end() ) {
						m_pBelong = itTemp->second;
						int iTest;
						if (n64Position[0] == n64CheckCell) {
							iTest = 0;
						} else {
							iTest = 1;
						}
						m_pBelong->m_cPointer = iTest;
						m_pBelong->m_cBackPointer = dirFace * 2 + (1 - iTest);
					}
				}
			}
			break;
		case 40:
			pThisFace = GetElementFace(pNode, nLevel, dirFace);
			if ( !pThisFace->m_bSkeleton ) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisFace );
				if ( pr.second ) {
					//printf("Face : (%16I64x, %d, %d) --> %d\n", n64ThisFace, nThisFace, dirFace, pThisFace->m_cPointer);
					DualPushWitnessFace(pNode, n64Position, nLevel, dirFace, pThisFace->m_cPointer);
				}
			} else if ( pThisFace->m_bSkeleton && pThisFace->m_bObject ) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisFace );
				if ( pr.second ) {
					hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashSkeleton.find((unsigned int)pThisFace);
					if ( itTemp != m_hashSkeleton.end() ) {
						itTemp->second->m_bValid = false;
					}
				}
			}
			break;
		case 256 + 32:
#ifdef CONSOLE_OUTPUT
			printf("Face : (%16I64x, %d, %d)\n", n64ThisFace, nThisFace, dirFace);
#endif
			break;
		case 256 + 33:
			m_nCountFace++;
			break;
		}
	}
}

void OcTree::RecCheckCellEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, UINT64 n64CheckCell, int nCheckCell)
{
	UINT64 n64ThisEdge;
	int nThisEdge;
	GetEdgePosition(n64Position, nLevel, dirEdge, n64ThisEdge, nThisEdge);
	if (!CheckCellForEdge(n64ThisEdge, nThisEdge, dirEdge, n64CheckCell, nCheckCell))
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
					RecCheckCellEdge(pNodeEdge, n64Edge, nEdge, 0, n64CheckCell, nCheckCell);
			}
			RecCheckCellPoint(pNodePoint, n64Point, nPoint, n64CheckCell, nCheckCell);
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
					RecCheckCellEdge(pNodeEdge, n64Edge, nEdge, 1, n64CheckCell, nCheckCell);
			}
			RecCheckCellPoint(pNodePoint, n64Point, nPoint, n64CheckCell, nCheckCell);
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
					RecCheckCellEdge(pNodeEdge, n64Edge, nEdge, 2, n64CheckCell, nCheckCell);
			}
			RecCheckCellPoint(pNodePoint, n64Point, nPoint, n64CheckCell, nCheckCell);
			break;
		}
	} else {
		Element * pThisEdge;
		switch (m_nType) {
		case 32:
			pThisEdge = GetElementEdge(pNode, nLevel, dirEdge);
			pThisEdge->m_bSketch = 1;
			pThisEdge->m_bObject = 1;
			break;
		case 35:
			pThisEdge = GetElementEdge(pNode, nLevel, dirEdge);
			if (pThisEdge->m_bObject)
				pThisEdge->m_bSketch = 1;
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
		case 36:
			if (((NodeHeader *)pNode[0][0])->m_bLastFix == 0 && ((NodeHeader *)pNode[0][1])->m_bLastFix == 0 &&
				((NodeHeader *)pNode[1][0])->m_bLastFix == 0 && ((NodeHeader *)pNode[1][1])->m_bLastFix == 0)
				m_nCountEdge ++;
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
		case 256 + 33:
			m_nCountEdge++;
			break;
		}
	}
}

void OcTree::RecCheckCellPoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2], UINT64 n64CheckCell, int nCheckCell)
{
	UINT64 n64ThisPoint;
	GetPointPosition(n64Position, nLevel, n64ThisPoint);
	if (!CheckCellForPoint(n64ThisPoint, n64CheckCell, nCheckCell))
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
			RecCheckCellPoint(pNodePoint, n64Point, nPoint, n64CheckCell, nCheckCell);
	} else {
		Element * pThisPoint;
		switch (m_nType) {
		case 32:
			pThisPoint = GetElementPoint(pNode, nLevel);
			if (pThisPoint->m_bObject == 0) {
				pThisPoint->m_cSimpleTag = 1;
			}
			pThisPoint->m_bSketch = 1;
			pThisPoint->m_bObject = 1;
			break;
		case 35:
			pThisPoint = GetElementPoint(pNode, nLevel);
			if (pThisPoint->m_bObject)
				pThisPoint->m_bSketch = 1;
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
		case 36:
			if (((NodeHeader *)pNode[0][0][0])->m_bLastFix == 0 && ((NodeHeader *)pNode[0][0][1])->m_bLastFix == 0 &&
				((NodeHeader *)pNode[0][1][0])->m_bLastFix == 0 && ((NodeHeader *)pNode[0][1][1])->m_bLastFix == 0 &&
				((NodeHeader *)pNode[1][0][0])->m_bLastFix == 0 && ((NodeHeader *)pNode[1][0][1])->m_bLastFix == 0 &&
				((NodeHeader *)pNode[1][1][0])->m_bLastFix == 0 && ((NodeHeader *)pNode[1][1][1])->m_bLastFix == 0)
				m_nCountPoint ++;
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
		case 256 + 33:
			m_nCountPoint++;
			break;
		}
	}
}
