#include "StdAfx.h"
#include ".\octree.h"
#include "HashMap.h"
#include "SingleHashMap.h"
#using <mscorlib.dll>

#define CLEAN_SKELETON
#undef CLEAN_SKELETON

COcTree::COcTree(void)
{
	m_nMaxLevel = 0;
	m_pRoot = NULL;
	m_pConstant = NULL;
	m_pMemoryManager = NULL;
	m_pHashMap = NULL;
	m_pSingleHashMap = NULL;
	m_cInfiniteNode.m_n64Type = 3;
	m_cInfiniteNode.m_n64SimpleTag = 0;
	m_cInfiniteNode.m_nVertex = -1;
}

COcTree::~COcTree(void)
{
	Clear();
	if (m_pHashMap != NULL)
		delete m_pHashMap;
	if (m_pSingleHashMap != NULL)
		delete m_pSingleHashMap;
}

void COcTree::Clear()
{
	if (m_pMemoryManager != NULL)
		delete m_pMemoryManager;
}

//////////////////////////////////////////////////////////////////////////
// Load from SOG
//////////////////////////////////////////////////////////////////////////
void COcTree::LoadFromSOG(FILE * pFile)
{
	Clear();
	m_pMemoryManager = new CMemoryManager();
	fread(&m_nMaxLevel, sizeof(int), 1, pFile);
	int i = 0;
	while (m_nMaxLevel != 1) {
		i++;
		m_nMaxLevel >>= 1;
	}
	m_nMaxLevel = i;

	RecLoadFromSOG(&m_pRoot, pFile);
}

void COcTree::RecLoadFromSOG(UCHAR ** ppNode, FILE * pFile, UINT64 n64Position, int nLevel)
{
	UCHAR nType;
	UCHAR nNext;
	float fNext[3];
	fread(&nType, 1, 1, pFile);
	switch(nType) {
	case 0:
		*ppNode = m_pMemoryManager->New( sizeof(INTER_NODE) );
		((INTER_NODE *)(*ppNode))->m_cType = 0;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					RecLoadFromSOG(&(((INTER_NODE *)(*ppNode))->m_pNode[i*4+j*2+k]), pFile, INT64TRANS(i*NLENGTH(nLevel)+INT64GETI(n64Position), j*NLENGTH(nLevel)+INT64GETJ(n64Position), k*NLENGTH(nLevel)+INT64GETK(n64Position)), nLevel + 1);
		break;
	case 1:
		fread(&nNext, 1, 1, pFile);
		*ppNode = m_pMemoryManager->New( sizeof(EMPTY_NODE) );
		((EMPTY_NODE *)(*ppNode))->m_n64Type = nNext == 0 ? 0x7ffffff1 : 1;
		break;
	case 2:
		fread(&nNext, 1, 1, pFile);
		fread(fNext, 4, 3, pFile);
		*ppNode = m_pMemoryManager->New( sizeof(LEAF_NODE) );
		((LEAF_NODE *)(*ppNode))->m_n64Type = 2 | ((UCHAR)(~nNext) << 4);
		UINT64 n64Temp = ((LEAF_NODE *)(*ppNode))->m_n64Type;
		UINT64 mask = 0x1000;
		for (int i = 0; i < 12; i++)
		{
			if (GET_POINT_INSIDE(n64Temp, CConstant::I_SHIFT_EP[i][0]) &&
				GET_POINT_INSIDE(n64Temp, CConstant::I_SHIFT_EP[i][1]))
				((LEAF_NODE *)(*ppNode))->m_n64Type |= mask;
			mask <<= 1;
		}
		for (int i = 0; i < 6; i++)
		{
			if (GET_POINT_INSIDE(n64Temp, CConstant::I_SHIFT_FP[i][0]) &&
				GET_POINT_INSIDE(n64Temp, CConstant::I_SHIFT_FP[i][1]) && 
				GET_POINT_INSIDE(n64Temp, CConstant::I_SHIFT_FP[i][2]) && 
				GET_POINT_INSIDE(n64Temp, CConstant::I_SHIFT_FP[i][3])
				)
				((LEAF_NODE *)(*ppNode))->m_n64Type |= mask;
			mask <<= 1;
		}
		if (GET_POINT_INSIDE(n64Temp, 0) && GET_POINT_INSIDE(n64Temp, 1) && GET_POINT_INSIDE(n64Temp, 2) && GET_POINT_INSIDE(n64Temp, 3)
			&& GET_POINT_INSIDE(n64Temp, 4) && GET_POINT_INSIDE(n64Temp, 5) && GET_POINT_INSIDE(n64Temp, 6) && GET_POINT_INSIDE(n64Temp, 7))
			((LEAF_NODE *)(*ppNode))->m_n64Type |= mask;
		break;
	}
}

void COcTree::RecLoadFromSOG(UCHAR ** ppNode, FILE * pFile)
{
	UCHAR nType;
	UCHAR nNext;
	float fNext[3];
	fread(&nType, 1, 1, pFile);
	switch(nType) {
	case 0:
		*ppNode = m_pMemoryManager->New( sizeof(INTER_NODE) );
		((INTER_NODE *)(*ppNode))->m_cType = 0;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					RecLoadFromSOG(&(((INTER_NODE *)(*ppNode))->m_pNode[i*4+j*2+k]), pFile);
		break;
	case 1:
		fread(&nNext, 1, 1, pFile);
		*ppNode = m_pMemoryManager->New( sizeof(EMPTY_NODE) );
		((EMPTY_NODE *)(*ppNode))->m_n64Type = nNext == 0 ? 0x7ffffff1 : 1;
		break;
	case 2:
		fread(&nNext, 1, 1, pFile);
		fread(fNext, 4, 3, pFile);
		*ppNode = m_pMemoryManager->New( sizeof(LEAF_NODE) );
		((LEAF_NODE *)(*ppNode))->m_n64Type = 2 | ((UCHAR)(~nNext) << 4);
		UINT64 n64Temp = ((LEAF_NODE *)(*ppNode))->m_n64Type;
		UINT64 mask = 0x1000;
		for (int i = 0; i < 12; i++)
		{
			if (GET_POINT_INSIDE(n64Temp, CConstant::I_SHIFT_EP[i][0]) &&
				GET_POINT_INSIDE(n64Temp, CConstant::I_SHIFT_EP[i][1]))
				((LEAF_NODE *)(*ppNode))->m_n64Type |= mask;
			mask <<= 1;
		}
		for (int i = 0; i < 6; i++)
		{
			if (GET_POINT_INSIDE(n64Temp, CConstant::I_SHIFT_FP[i][0]) &&
				GET_POINT_INSIDE(n64Temp, CConstant::I_SHIFT_FP[i][1]) && 
				GET_POINT_INSIDE(n64Temp, CConstant::I_SHIFT_FP[i][2]) && 
				GET_POINT_INSIDE(n64Temp, CConstant::I_SHIFT_FP[i][3])
				)
				((LEAF_NODE *)(*ppNode))->m_n64Type |= mask;
			mask <<= 1;
		}
		if (GET_POINT_INSIDE(n64Temp, 0) && GET_POINT_INSIDE(n64Temp, 1) && GET_POINT_INSIDE(n64Temp, 2) && GET_POINT_INSIDE(n64Temp, 3)
			&& GET_POINT_INSIDE(n64Temp, 4) && GET_POINT_INSIDE(n64Temp, 5) && GET_POINT_INSIDE(n64Temp, 6) && GET_POINT_INSIDE(n64Temp, 7))
			((LEAF_NODE *)(*ppNode))->m_n64Type |= mask;
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// Load From OcTree, allocate memory for retrace_edge
//////////////////////////////////////////////////////////////////////////
void COcTree::LoadFromOcTree(COcTree * pOcTree)
{
	Clear();
	m_pMemoryManager = new CMemoryManager();
	m_nMaxLevel = pOcTree->m_nMaxLevel;
	RecLoadFromOcTree(&m_pRoot, pOcTree->m_pRoot, pOcTree);
}

void COcTree::RecLoadFromOcTree(UCHAR ** ppNode, UCHAR * pOcTreeNode, COcTree * pOctree)
{
	UCHAR nType = (*pOcTreeNode) & 0x3;
	INTER_NODE * pTempInter;
	LEAF_NODE * pTempLeaf;
	INTER_NODE interNodeTemp;
	LEAF_NODE leafNodeTemp;
	switch(nType) {
	case 0:
		pTempInter = (INTER_NODE *)pOcTreeNode;
		interNodeTemp = *pTempInter;
		pOctree->m_pMemoryManager->Delete( pOcTreeNode );
		*ppNode = m_pMemoryManager->New( sizeof(INTER_NODE) );
		((INTER_NODE *)(*ppNode))->m_cType = 0;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					RecLoadFromOcTree(&(((INTER_NODE *)(*ppNode))->m_pNode[i*4+j*2+k]), interNodeTemp.m_pNode[i*4+j*2+k], pOctree);
		break;
	case 2:
	case 1:
		pTempLeaf = (LEAF_NODE *)pOcTreeNode;
		leafNodeTemp = *pTempLeaf;
		pOctree->m_pMemoryManager->Delete( pOcTreeNode );
		int nCount = 0;
		for (int i = 0; i < 6; i++)
			if ((leafNodeTemp.m_n64Type >> (56 + i)) & 1)
				nCount++;
		*ppNode = m_pMemoryManager->New( sizeof(LEAF_NODE) + nCount * (sizeof(RETRACE_EDGE)) );
		m_nCountCell += nCount;
		((LEAF_NODE *)(*ppNode))->m_n64Type = leafNodeTemp.m_n64Type;
		((LEAF_NODE *)(*ppNode))->m_n64SimpleTag = 0;
		((LEAF_NODE *)(*ppNode))->m_nVertex = 0xffffffff;
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// Trying a traversal
//////////////////////////////////////////////////////////////////////////
void COcTree::RecTraversalCell(UCHAR * pNode, UINT64 n64Position, int nLevel)
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
					//Leaf(pNode, n64Position, nLevel, i, j, k, pNodeCell, n64Cell, nCell);
					pNodeCell = pNodePoint[i][j][k];
					n64Cell = n64Point[i][j][k];
					nCell = nPoint[i][j][k];
					RecTraversalCell(pNodeCell, n64Cell, nCell);
				}
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
					//Leaf(pNode, n64Position, nLevel, k, i, j, pNodeFace[k], n64Face[k], nFace[k]);
				{
					pNodeFace[k] = pNodePoint[k][i][j];
					n64Face[k] = n64Point[k][i][j];
					nFace[k] = nPoint[k][i][j];
				}
				RecTraversalFace(pNodeFace, n64Face, nFace, 0);
				for (int k = 0; k < 2; k++)
					//Leaf(pNode, n64Position, nLevel, j, k, i, pNodeFace[k], n64Face[k], nFace[k]);
				{
					pNodeFace[k] = pNodePoint[j][k][i];
					n64Face[k] = n64Point[j][k][i];
					nFace[k] = nPoint[j][k][i];
				}
				RecTraversalFace(pNodeFace, n64Face, nFace, 1);
				for (int k = 0; k < 2; k++)
					//Leaf(pNode, n64Position, nLevel, i, j, k, pNodeFace[k], n64Face[k], nFace[k]);
				{
					pNodeFace[k] = pNodePoint[i][j][k];
					n64Face[k] = n64Point[i][j][k];
					nFace[k] = nPoint[i][j][k];
				}
				RecTraversalFace(pNodeFace, n64Face, nFace, 2);
			}
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					//Leaf(pNode, n64Position, nLevel, i, j, k, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				{
					pNodeEdge[j][k] = pNodePoint[i][j][k];
					n64Edge[j][k] = n64Point[i][j][k];
					nEdge[j][k] = nPoint[i][j][k];
				}
			RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 0);
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					//Leaf(pNode, n64Position, nLevel, k, i, j, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				{
					pNodeEdge[j][k] = pNodePoint[k][i][j];
					n64Edge[j][k] = n64Point[k][i][j];
					nEdge[j][k] = nPoint[k][i][j];
				}
			RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 1);
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					//Leaf(pNode, n64Position, nLevel, j, k, i, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				{
					pNodeEdge[j][k] = pNodePoint[j][k][i];
					n64Edge[j][k] = n64Point[j][k][i];
					nEdge[j][k] = nPoint[j][k][i];
				}
			RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 2);
		}
		RecTraversalPoint(pNodePoint, n64Point, nPoint);
	} else {
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode;
		switch(m_nType) {
		case 0:
			if (GET_CELL_INSIDE(pTempNode->m_n64Type, 0) != 0)
				m_nCountCell++;
			break;
		case 1:
			m_nCountCell++;
			break;
		case 8:
			if (GET_CELL_SIMPLE(pTempNode->m_n64SimpleTag, 0) == 1) {
				SetObjectSimple(pTempNode->m_n64SimpleTag, 0, 3, 2);
			} else {
				SetObjectSimple(pTempNode->m_n64SimpleTag, 0, 3, 0);
			}
			break;
		}
	}
}

void COcTree::RecTraversalFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace)
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
					for (int k = 0; k < 2; k++)
						//Leaf(pNode[k], n64Position[k], nLevel[k], 1-k, i, j, pNodeFace[k], n64Face[k], nFace[k]);
					{
						pNodeFace[k] = pNodePoint[k][i][j];
						n64Face[k] = n64Point[k][i][j];
						nFace[k] = nPoint[k][i][j];
					}
					RecTraversalFace(pNodeFace, n64Face, nFace, 0);
				}
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						//Leaf(pNode[k], n64Position[k], nLevel[k], 1-k, i, j, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
					{
						pNodeEdge[j][k] = pNodePoint[k][i][j];
						n64Edge[j][k] = n64Point[k][i][j];
						nEdge[j][k] = nPoint[k][i][j];
					}
				RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 1);
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						//Leaf(pNode[j], n64Position[j], nLevel[j], 1-j, k, i, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
					{
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
					for (int k = 0; k < 2; k++)
						//Leaf(pNode[k], n64Position[k], nLevel[k], j, 1-k, i, pNodeFace[k], n64Face[k], nFace[k]);
					{
						pNodeFace[k] = pNodePoint[j][k][i];
						n64Face[k] = n64Point[j][k][i];
						nFace[k] = nPoint[j][k][i];
					}
					RecTraversalFace(pNodeFace, n64Face, nFace, 1);
				}
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						//Leaf(pNode[k], n64Position[k], nLevel[k], j, 1-k, i, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
					{
						pNodeEdge[j][k] = pNodePoint[j][k][i];
						n64Edge[j][k] = n64Point[j][k][i];
						nEdge[j][k] = nPoint[j][k][i];
					}
				RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 2);
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						//Leaf(pNode[j], n64Position[j], nLevel[j], i, 1-j, k, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
					{
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
					for (int k = 0; k < 2; k++)
						//Leaf(pNode[k], n64Position[k], nLevel[k], i, j, 1-k, pNodeFace[k], n64Face[k], nFace[k]);
					{
						pNodeFace[k] = pNodePoint[i][j][k];
						n64Face[k] = n64Point[i][j][k];
						nFace[k] = nPoint[i][j][k];
					}
					RecTraversalFace(pNodeFace, n64Face, nFace, 2);
				}
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						//Leaf(pNode[k], n64Position[k], nLevel[k], i, j, 1-k, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
					{
						pNodeEdge[j][k] = pNodePoint[i][j][k];
						n64Edge[j][k] = n64Point[i][j][k];
						nEdge[j][k] = nPoint[i][j][k];
					}
				RecTraversalEdge(pNodeEdge, n64Edge, nEdge, 0);
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						//Leaf(pNode[j], n64Position[j], nLevel[j], k, i, 1-j, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
					{
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
		GetStorageFace(nLevel, dirFace, nI, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI];
		switch(m_nType) {
		case -1:
			if (GET_FACE_INSIDE(pTempNode->m_n64Type, nIndex) == 0) {
				for (int i = 0; i < 2; i++){
					if (GET_CELL_INSIDE(((LEAF_NODE *)pNode[i])->m_n64Type, 0) != 0) {	// error
						printf("An error face-cell detected!\n");
					}
				}
			}
			break;
		case 0:
			if (GET_FACE_INSIDE(pTempNode->m_n64Type, nIndex) != 0)
				m_nCountFace++;
			break;
		case 1:
			m_nCountFace++;
			break;
		case 4:
			if (GET_FACE_INSIDE(pTempNode->m_n64Type, nIndex)) {
				for (int i = 0; i < 2; i++)
					if (GET_CELL_INSIDE(((LEAF_NODE *)pNode[i])->m_n64Type, 0) == 0) {
						int dirEdge;
						list<int> listIndex;
						UINT64 n64Face;
						UCHAR * pNodeEdge[2][2];
						UINT64 n64Edge[2][2];
						int nEdge[2][2];

						GetFacePos(n64Position, nLevel, dirFace, nI, 0, n64Face);
						dirEdge = (dirFace + 2 - i) % 3;
						GetEdgeFromPos(n64Face, nLevel[nI], dirEdge, pNodeEdge, n64Edge, nEdge);
						RecCreateFaceFaceEdgeTraversal(pNode[i], n64Position[i], nLevel[i], pNodeEdge, n64Edge, nEdge, dirEdge, 0, listIndex);

						GetFacePos(n64Position, nLevel, dirFace, nI, 1+i, n64Face);
						dirEdge = (dirFace + 1 + i) % 3;
						GetEdgeFromPos(n64Face, nLevel[nI], dirEdge, pNodeEdge, n64Edge, nEdge);
						RecCreateFaceFaceEdgeTraversal(pNode[i], n64Position[i], nLevel[i], pNodeEdge, n64Edge, nEdge, dirEdge, 0, listIndex);

						GetFacePos(n64Position, nLevel, dirFace, nI, 2-i, n64Face);
						dirEdge = (dirFace + 2 - i) % 3;
						GetEdgeFromPos(n64Face, nLevel[nI], dirEdge, pNodeEdge, n64Edge, nEdge);
						RecCreateFaceFaceEdgeTraversal(pNode[i], n64Position[i], nLevel[i], pNodeEdge, n64Edge, nEdge, dirEdge, 1, listIndex);

						GetFacePos(n64Position, nLevel, dirFace, nI, 0, n64Face);
						dirEdge = (dirFace + 1 + i) % 3;
						GetEdgeFromPos(n64Face, nLevel[nI], dirEdge, pNodeEdge, n64Edge, nEdge);
						RecCreateFaceFaceEdgeTraversal(pNode[i], n64Position[i], nLevel[i], pNodeEdge, n64Edge, nEdge, dirEdge, 1, listIndex);

						if (listIndex.size() >= 3) {
							WriteFace(listIndex, m_nFaceNum);
							//m_nFaceNum++;
						}
					}
			}
			break;
		case 5:
			if (GET_FACE_SIMPLE(pTempNode->m_n64SimpleTag, nIndex) == 3) {
				for (int i = 0; i < 2; i++)
				{
					if (nLevel[i] == nLevel[nI]) {
						SetObjectSimple(((LEAF_NODE *)pNode[i])->m_n64SimpleTag, dirFace * 2 + (1 - i), 2, 3);
					}
				}
			}
			//for (int i = 0; i < 2; i++) 
			//{
			//	if (nLevel[i] < nLevel[nI]) {
			//		SetObjectInside(((LEAF_NODE *)pNode[i])->m_n64Type, dirFace * 2 + (1 - i), 2, false);
			//	} else {
			//		SetObjectInside(((LEAF_NODE *)pNode[i])->m_n64Type, dirFace * 2 + (1 - i), 2, GET_FACE_INSIDE(pTempNode->m_n64Type, nIndex));
			//	}
			//}
			break;
		case 7:
			if (GET_FACE_SIMPLE(pTempNode->m_n64SimpleTag, nIndex) == 3) {
				for (int i = 0; i < 2; i++)
				{
					if (nLevel[i] == nLevel[nI]) {
						SetObjectSimple(((LEAF_NODE *)pNode[i])->m_n64SimpleTag, dirFace * 2 + (1 - i), 2, 3);
					}
				}
			}
			break;
		case 8:
			if (GET_FACE_SIMPLE(pTempNode->m_n64SimpleTag, nIndex) == 1) {
				SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 2, 2);
			} else {
				SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 2, 0);
			}
			break;
		}
	}
}

void COcTree::RecTraversalEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge)
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
					for (int k = 0; k < 2; k++)
						//Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], i, 1-j, 1-k, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
					{
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
					for (int k = 0; k < 2; k++)
						//Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], 1-k, i, 1-j, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
					{
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
					for (int k = 0; k < 2; k++)
						//Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], 1-j, 1-k, i, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
					{
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
		GetStorageEdge(nLevel, dirEdge, nI, nJ, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI][nJ];
		switch(m_nType) {
		case -1:
			if (GET_EDGE_INSIDE(pTempNode->m_n64Type, nIndex) == 0) {
				UCHAR * pNodeFace[2];
				int nFace[2];
				int dirFace, nFaceI, nFaceIndex;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++) {
						pNodeFace[j] = pNode[i][j];
						nFace[j] = nLevel[i][j];
					}
					dirFace = (dirEdge + 2) % 3;
					if (CheckStorageFace(pNodeFace)) {
						GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
						if (GET_FACE_INSIDE(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64Type, nFaceIndex) != 0) {	// error
							printf("An error edge-face detected!\n");
						}
					}
					for (int j = 0; j < 2; j++) {
						pNodeFace[j] = pNode[j][i];
						nFace[j] = nLevel[j][i];
					}
					dirFace = (dirEdge + 1) % 3;
					if (CheckStorageFace(pNodeFace)) {
						GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
						if (GET_FACE_INSIDE(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64Type, nFaceIndex) != 0) {	// error
							printf("An error edge-face detected!\n");
						}
					}
				}
			}
			break;
		case 0:
			if (GET_EDGE_INSIDE(pTempNode->m_n64Type, nIndex))
				m_nCountEdge++;
			break;
		case 1:
			m_nCountEdge++;
			break;
		case 4:
			if (GET_EDGE_INSIDE(pTempNode->m_n64Type, nIndex)) {
				UCHAR * pNodeFace[2];
				UINT64 n64Face[2];
				int nFace[2];
				int dirFace;
				list<int> listIndex;
				UINT64 n64Edge[2];
				GetEdgePos(n64Position, nLevel, dirEdge, nI, nJ, 0, n64Edge[0]);
				GetEdgePos(n64Position, nLevel, dirEdge, nI, nJ, 1, n64Edge[1]);
				for (int i = 0; i < 2; i++)
				{
					listIndex.clear();
					for (int j = 0; j < 2; j++)
					{
						pNodeFace[j] = pNode[i][j];
						n64Face[j] = n64Position[i][j];
						nFace[j] = nLevel[i][j];
					}
					dirFace = (dirEdge + 2) % 3;
					if (!GetFaceInside(pNodeFace, nFace, dirFace)) {
						InsertIndex(listIndex, GetIndex(pNodeFace[i], n64Face[i], n64Edge[0]));
						InsertIndex(listIndex, GetIndex(pNodeFace[i], n64Face[i], n64Edge[1]));
						InsertIndex(listIndex, GetIndex(pNodeFace[1-i], n64Face[1-i], n64Edge[1]));
						InsertIndex(listIndex, GetIndex(pNodeFace[1-i], n64Face[1-i], n64Edge[0]));
						if (listIndex.size() >= 3) {
							WriteFace(listIndex, m_nFaceNum);
							//m_nFaceNum ++;
						}
					}

					listIndex.clear();
					for (int j = 0; j < 2; j++)
					{
						pNodeFace[j] = pNode[j][i];
						n64Face[j] = n64Position[j][i];
						nFace[j] = nLevel[j][i];
					}
					dirFace = (dirEdge + 1) % 3;
					if (!GetFaceInside(pNodeFace, nFace, dirFace)) {
						InsertIndex(listIndex, GetIndex(pNodeFace[1-i], n64Face[1-i], n64Edge[0]));
						InsertIndex(listIndex, GetIndex(pNodeFace[1-i], n64Face[1-i], n64Edge[1]));
						InsertIndex(listIndex, GetIndex(pNodeFace[i], n64Face[i], n64Edge[1]));
						InsertIndex(listIndex, GetIndex(pNodeFace[i], n64Face[i], n64Edge[0]));
						if (listIndex.size() >= 3) {
							WriteFace(listIndex, m_nFaceNum);
							//m_nFaceNum ++;
						}
					}
				}
			}
			break;
		case 5:
			if (GET_EDGE_SIMPLE(pTempNode->m_n64SimpleTag, nIndex) == 3) {
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
					{
						if (nLevel[i][j] == nLevel[nI][nJ]) {
							SetObjectSimple(((LEAF_NODE *)pNode[i][j])->m_n64SimpleTag, dirEdge * 4 + (1 - i) * 2 + (1 - j), 1, 3);
						}
					}
			}
			//for (int i = 0; i < 2; i++)
			//	for (int j = 0; j < 2; j++)
			//	{
			//		if (nLevel[i][j] < nLevel[nI][nJ]) {
			//			SetObjectInside(((LEAF_NODE *)pNode[i][j])->m_n64Type, dirEdge * 4 + (1 - i) * 2 + (1 - j), 1, false);
			//		} else {
			//			SetObjectInside(((LEAF_NODE *)pNode[i][j])->m_n64Type, dirEdge * 4 + (1 - i) * 2 + (1 - j), 1, GET_EDGE_INSIDE(pTempNode->m_n64Type, nIndex));
			//		}
			//	}
			break;
		case 6:
			if (GET_EDGE_SIMPLE(pTempNode->m_n64SimpleTag, nIndex) == 0) {
				SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 1, 2);
			} else {
				SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 1, 1);
			}
			break;
		case 7:
			if (GET_EDGE_SIMPLE(pTempNode->m_n64SimpleTag, nIndex) == 3) {
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
					{
						if (nLevel[i][j] == nLevel[nI][nJ]) {
							SetObjectSimple(((LEAF_NODE *)pNode[i][j])->m_n64SimpleTag, dirEdge * 4 + (1 - i) * 2 + (1 - j), 1, 3);
						}
					}
			}
			break;
		}
	}
}

void COcTree::RecTraversalPoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2])
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
		GetStoragePoint(nLevel, nI, nJ, nK, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI][nJ][nK];
		switch(m_nType) {
		case -1:
			if (GET_POINT_INSIDE(pTempNode->m_n64Type, nIndex) == 0) {
				UCHAR * pNodeEdge[2][2];
				int nEdge[2][2];
				int dirEdge, nEdgeI, nEdgeJ, nEdgeIndex;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNode[i][j][k];
							nEdge[j][k] = nLevel[i][j][k];
						}
					if (CheckStorageEdge(pNodeEdge)) {
						dirEdge = 0;
						GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
						if (GET_EDGE_INSIDE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64Type, nEdgeIndex) != 0) {	// error
							printf("An error point-edge detected!\n");
						}
					}
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNode[k][i][j];
							nEdge[j][k] = nLevel[k][i][j];
						}
					if (CheckStorageEdge(pNodeEdge)) {
						dirEdge = 1;
						GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
						if (GET_EDGE_INSIDE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64Type, nEdgeIndex) != 0) {	// error
							printf("An error point-edge detected!\n");
						}
					}
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNode[j][k][i];
							nEdge[j][k] = nLevel[j][k][i];
						}
					if (CheckStorageEdge(pNodeEdge)) {
						dirEdge = 2;
						GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
						if (GET_EDGE_INSIDE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64Type, nEdgeIndex) != 0) {	// error
							printf("An error point-edge detected!\n");
						}
					}
				}
			}
			break;
		case 0:
			if (GET_POINT_INSIDE(pTempNode->m_n64Type, nIndex))
				m_nCountPoint++;
			break;
		case 1:
			m_nCountPoint++;
			break;
		case 2:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						if (GET_POINT_INSIDE(pTempNode->m_n64Type, nIndex) && GET_CELL_INSIDE(((LEAF_NODE *)pNode[i][j][k])->m_n64Type, 0) == 0 && ((LEAF_NODE *)pNode[i][j][k])->m_nVertex == 0xffffffff) {
							if (nLevel[i][j][k] == m_nMaxLevel) {
								m_nIrregularCount++;
								m_nVertexNum ++;
							} else {
								int iIndex = i * 4 + j * 2 + k;
								int l = 0;
								for (; l < iIndex; l++)
									if (n64Position[i][j][k] == n64Position[(l >> 2) & 1][(l >> 1) & 1][l & 1])
										break;
								if (l == iIndex) {
									m_nIrregularCount++;
									m_nVertexNum ++;
								}
							}
						}
			break;
		case 3:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						if (GET_POINT_INSIDE(pTempNode->m_n64Type, nIndex) && GET_CELL_INSIDE(((LEAF_NODE *)pNode[i][j][k])->m_n64Type, 0) == 0 && ((LEAF_NODE *)pNode[i][j][k])->m_nVertex == 0xffffffff) {
							UINT64 n64TempPoint = n64Position[nI][nJ][nK] + INT64TRANS((1-nI)*NLENGTH(nLevel[nI][nJ][nK]), (1-nJ)*NLENGTH(nLevel[nI][nJ][nK]), (1-nK)*NLENGTH(nLevel[nI][nJ][nK]));
							LEAF_NODE * pLeafNode = (LEAF_NODE *)pNode[i][j][k];
							if (m_bCountOnly) {
								if (m_pHashMap->Get(n64Position[i][j][k], n64TempPoint) == -1) {
									m_pHashMap->Set(n64Position[i][j][k], n64TempPoint, m_nVertexNum);
									m_nIrregularCount ++;
									m_nVertexNum ++;
								}
							} else {
								bool bWrite = false;
								if (nLevel[i][j][k] == m_nMaxLevel) {
									m_nIrregularCount++;
									m_nVertexNum ++;
									bWrite = true;
								} else {
									int iIndex = i * 4 + j * 2 + k;
									int l = 0;
									for (; l < iIndex; l++)
										if (n64Position[i][j][k] == n64Position[(l >> 2) & 1][(l >> 1) & 1][l & 1])
											break;
									if (l == iIndex) {
										m_nIrregularCount++;
										m_nVertexNum ++;
										bWrite = true;
									}
								}
								if (bWrite) {
									float fPos[3];

									//if (!GET_CHANGED(pLeafNode->m_n64Type) && (*pNode[i][j][k] & 0x3) == 2) {
									if ((*pNode[i][j][k] & 0x3) == 2) {
										CVector3D vCell = m_pSingleHashMap->Get((int)pLeafNode);
										if (vCell.pVec[0] != 1e300 && !GET_CHANGED(pLeafNode->m_n64Type)) {
											fPos[0] = (float)(0.05 * (m_dbNodeLength * (INT64GETI(n64TempPoint)) + m_vCorner.pVec[0]) + 0.95 * vCell.pVec[0]);
											fPos[1] = (float)(0.05 * (m_dbNodeLength * (INT64GETJ(n64TempPoint)) + m_vCorner.pVec[1]) + 0.95 * vCell.pVec[1]);
											fPos[2] = (float)(0.05 * (m_dbNodeLength * (INT64GETK(n64TempPoint)) + m_vCorner.pVec[2]) + 0.95 * vCell.pVec[2]);
										} else {
											fPos[0] = (float)(0.5 * m_dbNodeLength * (INT64GETI(n64TempPoint) + INT64GETI(n64Position[i][j][k]) + NLENGTH(nLevel[i][j][k] + 1)) + m_vCorner.pVec[0]);
											fPos[1] = (float)(0.5 * m_dbNodeLength * (INT64GETJ(n64TempPoint) + INT64GETJ(n64Position[i][j][k]) + NLENGTH(nLevel[i][j][k] + 1)) + m_vCorner.pVec[1]);
											fPos[2] = (float)(0.5 * m_dbNodeLength * (INT64GETK(n64TempPoint) + INT64GETK(n64Position[i][j][k]) + NLENGTH(nLevel[i][j][k] + 1)) + m_vCorner.pVec[2]);
										}
									} else {
										fPos[0] = (float)(0.5 * m_dbNodeLength * (INT64GETI(n64TempPoint) + INT64GETI(n64Position[i][j][k]) + NLENGTH(nLevel[i][j][k] + 1)) + m_vCorner.pVec[0]);
										fPos[1] = (float)(0.5 * m_dbNodeLength * (INT64GETJ(n64TempPoint) + INT64GETJ(n64Position[i][j][k]) + NLENGTH(nLevel[i][j][k] + 1)) + m_vCorner.pVec[1]);
										fPos[2] = (float)(0.5 * m_dbNodeLength * (INT64GETK(n64TempPoint) + INT64GETK(n64Position[i][j][k]) + NLENGTH(nLevel[i][j][k] + 1)) + m_vCorner.pVec[2]);
									}
									WriteVertex(fPos);
								}
							}
						}
			break;
		case 4:
			if (GET_POINT_INSIDE(pTempNode->m_n64Type, nIndex)) {
				UCHAR * pNodeEdge[2][2];
				UINT64 n64Edge[2][2];
				int nEdge[2][2];
				int dirEdge;
				list<int> listIndex;
				UINT64 n64Point;
				GetPointPos(n64Position, nLevel, nI, nJ, nK, n64Point);

				for (int i = 0; i < 2; i++)
				{
					listIndex.clear();
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNode[i][j][k];
							n64Edge[j][k] = n64Position[i][j][k];
							nEdge[j][k] = nLevel[i][j][k];
						}
					dirEdge = 0;
					if (!GetEdgeInside(pNodeEdge, nEdge, dirEdge)) {
						InsertIndex(listIndex, GetIndex(pNodeEdge[0][0], n64Edge[0][0], n64Point));
						InsertIndex(listIndex, GetIndex(pNodeEdge[i][1-i], n64Edge[i][1-i], n64Point));
						InsertIndex(listIndex, GetIndex(pNodeEdge[1][1], n64Edge[1][1], n64Point));
						InsertIndex(listIndex, GetIndex(pNodeEdge[1-i][i], n64Edge[1-i][i], n64Point));
						if (listIndex.size() >= 3) {
							WriteFace(listIndex, m_nFaceNum);
							//m_nFaceNum ++;
						}
					}

					listIndex.clear();
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNode[k][i][j];
							n64Edge[j][k] = n64Position[k][i][j];
							nEdge[j][k] = nLevel[k][i][j];
						}
					dirEdge = 1;
					if (!GetEdgeInside(pNodeEdge, nEdge, dirEdge)) {
						InsertIndex(listIndex, GetIndex(pNodeEdge[0][0], n64Edge[0][0], n64Point));
						InsertIndex(listIndex, GetIndex(pNodeEdge[i][1-i], n64Edge[i][1-i], n64Point));
						InsertIndex(listIndex, GetIndex(pNodeEdge[1][1], n64Edge[1][1], n64Point));
						InsertIndex(listIndex, GetIndex(pNodeEdge[1-i][i], n64Edge[1-i][i], n64Point));
						if (listIndex.size() >= 3) {
							WriteFace(listIndex, m_nFaceNum);
							//m_nFaceNum ++;
						}
					}

					listIndex.clear();
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNode[j][k][i];
							n64Edge[j][k] = n64Position[j][k][i];
							nEdge[j][k] = nLevel[j][k][i];
						}
					dirEdge = 2;
					if (!GetEdgeInside(pNodeEdge, nEdge, dirEdge)) {
						InsertIndex(listIndex, GetIndex(pNodeEdge[0][0], n64Edge[0][0], n64Point));
						InsertIndex(listIndex, GetIndex(pNodeEdge[i][1-i], n64Edge[i][1-i], n64Point));
						InsertIndex(listIndex, GetIndex(pNodeEdge[1][1], n64Edge[1][1], n64Point));
						InsertIndex(listIndex, GetIndex(pNodeEdge[1-i][i], n64Edge[1-i][i], n64Point));
						if (listIndex.size() >= 3) {
							WriteFace(listIndex, m_nFaceNum);
							//m_nFaceNum ++;
						}
					}
				}
			}
			break;
		case 5:
			if (GET_POINT_SIMPLE(pTempNode->m_n64SimpleTag, nIndex) == 3) {
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
							if (pNode[i][j][k] != pNode[1-i][j][k] && pNode[i][j][k] != pNode[i][1-j][k] && pNode[i][j][k] != pNode[i][j][1-k])
								SetObjectSimple(((LEAF_NODE *)pNode[i][j][k])->m_n64SimpleTag, (1 - i) * 4 + (1 - j) * 2 + (1 - k), 0, 3);
			}
			//for (int i = 0; i < 2; i++)
			//	for (int j = 0; j < 2; j++)
			//		for (int k = 0; k < 2; k++)
			//			if (pNode[i][j][k] != pNode[1-i][j][k] && pNode[i][j][k] != pNode[i][1-j][k] && pNode[i][j][k] != pNode[i][j][1-k])
			//				SetObjectInside(((LEAF_NODE *)pNode[i][j][k])->m_n64Type, (1 - i) * 4 + (1 - j) * 2 + (1 - k), 0, GET_POINT_INSIDE(pTempNode->m_n64Type, nIndex));
			break;
		case 6:
			if (GET_POINT_SIMPLE(pTempNode->m_n64SimpleTag, nIndex) == 0) {
				SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 0, 2);
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							m_pPointStart[i][j][k] = pNode[i][j][k];
							m_nPointStart[i][j][k] = nLevel[i][j][k];
							m_n64PointStart[i][j][k] = n64Position[i][j][k];
						}
			} else {
				SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 0, 1);				
			}
			break;
		}
	}
}

void COcTree::Traversal()
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
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
			{
				for (int ii = 0; ii < 2; ii++)
					for (int jj = 0; jj < 2; jj++)
						for (int kk = 0; kk < 2; kk++) {
							pNodePoint[ii][jj][kk] = (UCHAR *)(&m_cInfiniteNode);
							n64Point[ii][jj][kk] = 0;
							nPoint[ii][jj][kk] = -1;
						}
				pNodePoint[i][j][k] = m_pRoot;
				n64Point[i][j][k] = 0;
				nPoint[i][j][k] = 0;
				RecTraversalPoint(pNodePoint, n64Point, nPoint);
			}
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
		{
			for (int ii = 0; ii < 2; ii++)
				for (int jj = 0; jj < 2; jj++) {
					pNodeEdge[ii][jj] = (UCHAR *)(&m_cInfiniteNode);
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
	{
		for (int ii = 0; ii < 2; ii++) {
			pNodeFace[ii] = (UCHAR *)(&m_cInfiniteNode);
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
	RecTraversalCell(m_pRoot, 0, 0);
}

//////////////////////////////////////////////////////////////////////////
// init face tags, for create new OcTree
//////////////////////////////////////////////////////////////////////////
void COcTree::InitFaceTags()
{
	UCHAR * pNodeFace[2];
	UINT64 n64Face[2];
	int nFace[2];
	for (int i = 0; i < 2; i++)
	{
		for (int ii = 0; ii < 2; ii++) {
			pNodeFace[ii] = (UCHAR *)(&m_cInfiniteNode);
			n64Face[ii] = 0;
			nFace[ii] = -1;
		}
		pNodeFace[i] = m_pRoot;
		n64Face[i] = 0;
		nFace[i] = 0;
		RecInitFaceTagsFace(pNodeFace, n64Face, nFace, 0);
		RecInitFaceTagsFace(pNodeFace, n64Face, nFace, 1);
		RecInitFaceTagsFace(pNodeFace, n64Face, nFace, 2);
	}

	RecInitFaceTagsCell(m_pRoot, 0, 0);
}

void COcTree::RecInitFaceTagsCell(UCHAR * pNode, UINT64 n64Position, int nLevel)
{
	UCHAR * pNodeCell;
	UCHAR * pNodeFace[2];
	UCHAR * pNodePoint[2][2][2];
	UINT64 n64Cell;
	UINT64 n64Face[2];
	UINT64 n64Point[2][2][2];
	int nCell;
	int nFace[2];
	int nPoint[2][2][2];

	if (NOT_LEAF(pNode)) {
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
					RecInitFaceTagsCell(pNodeCell, n64Cell, nCell);
				}
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					pNodeFace[k] = pNodePoint[k][i][j];
					n64Face[k] = n64Point[k][i][j];
					nFace[k] = nPoint[k][i][j];
				}
				RecInitFaceTagsFace(pNodeFace, n64Face, nFace, 0);
				for (int k = 0; k < 2; k++)
				{
					pNodeFace[k] = pNodePoint[j][k][i];
					n64Face[k] = n64Point[j][k][i];
					nFace[k] = nPoint[j][k][i];
				}
				RecInitFaceTagsFace(pNodeFace, n64Face, nFace, 1);
				for (int k = 0; k < 2; k++)
				{
					pNodeFace[k] = pNodePoint[i][j][k];
					n64Face[k] = n64Point[i][j][k];
					nFace[k] = nPoint[i][j][k];
				}
				RecInitFaceTagsFace(pNodeFace, n64Face, nFace, 2);
			}
	} else {
	}
}

void COcTree::RecInitFaceTagsFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace)
{
	UCHAR * pNodeFace[2];
	UCHAR * pNodePoint[2][2][2];
	UINT64 n64Face[2];
	UINT64 n64Point[2][2][2];
	int nFace[2];
	int nPoint[2][2][2];

	if (*(pNode[0]) == 0 || *(pNode[1]) == 0) {
		switch(dirFace) {
		case 0:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[i], n64Position[i], nLevel[i], 1-i, j, k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
					{
						pNodeFace[k] = pNodePoint[k][i][j];
						n64Face[k] = n64Point[k][i][j];
						nFace[k] = nPoint[k][i][j];
					}
					RecInitFaceTagsFace(pNodeFace, n64Face, nFace, 0);
				}
			break;
		case 1:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j], n64Position[j], nLevel[j], i, 1-j, k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
					{
						pNodeFace[k] = pNodePoint[j][k][i];
						n64Face[k] = n64Point[j][k][i];
						nFace[k] = nPoint[j][k][i];
					}
					RecInitFaceTagsFace(pNodeFace, n64Face, nFace, 1);
				}
			break;
		case 2:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k], n64Position[k], nLevel[k], i, j, 1-k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
					{
						pNodeFace[k] = pNodePoint[i][j][k];
						n64Face[k] = n64Point[i][j][k];
						nFace[k] = nPoint[i][j][k];
					}
					RecInitFaceTagsFace(pNodeFace, n64Face, nFace, 2);
				}
			break;
		}
	} else {
		int nI, nIndex;
		GetStorageFace(nLevel, dirFace, nI, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI];
		pTempNode->m_n64Type |= ((UINT64)1 << (56 + nIndex));
	}
}

//////////////////////////////////////////////////////////////////////////
// Contouring code
//////////////////////////////////////////////////////////////////////////
void COcTree::ContouringQD(CString strFile, CString strSOGFile)
{
	CString strExtName = strFile.Right(4);
	strExtName.MakeUpper();
	if (strExtName == CString(".OBJ")) {
		m_nFileType = 0;
	} else {
		m_nFileType = 1;
	}

	// initialization
	m_nType = 5;
	RecTraversalCell(m_pRoot, 0, 0);

	// create write file and read file
	m_pWriteFile = fopen(strFile.GetBuffer(0), "wb");
	m_pSOGFile = fopen(strSOGFile.GetBuffer(0), "rb");
	fseek(m_pSOGFile, 128, SEEK_SET);

	int nTemp;
	fread(&nTemp, sizeof(int), 1, m_pSOGFile);

	// count vertices and faces
	m_bCountOnly = true;
	m_nVertexNum = 0;
	m_nFaceNum = 0;
	int nRegularCount = 0;
	m_nIrregularCount = 0;
	m_nIrregularUnchangedCell = 0;
	printf("Count vertices for contouring...\n");

	RecRegularTest(m_pRoot, nRegularCount);
	m_nType = 2;
	RecTraversalCell(m_pRoot, 0, 0);

	printf("Allocing memory for vertex hash map, number is %d\n", m_nIrregularCount);
	m_pHashMap = new CHashMap(m_pConstant, m_nIrregularCount);
	printf("Allocing memory for special hash map, number is %d\n", m_nIrregularUnchangedCell);
	m_pSingleHashMap = new CSingleHashMap(m_pConstant, m_nIrregularUnchangedCell);
	
	m_nVertexNum = 0;
	nRegularCount = 0;
	m_nIrregularCount = 0;
	RecRegularTest(m_pRoot, nRegularCount);
	m_nType = 3;
	RecTraversalCell(m_pRoot, 0, 0);

	printf("Count faces for contouring...\n");
	m_nType = 4;
	RecTraversalCell(m_pRoot, 0, 0);

	printf("Totally %d vertices and %d faces are going to be writen.\n", m_nVertexNum, m_nFaceNum);

	// write vertices and faces
	WriteFileHeader();

	m_bCountOnly = false;
	m_nVertexNum = 0;
	m_nFaceNum = 0;
	nRegularCount = 0;
	m_nIrregularCount = 0;
	printf("Writing vertexes...\n");
	RecRegularTest(m_pRoot, nRegularCount);
	m_nType = 3;
	RecTraversalCell(m_pRoot, 0, 0);

	printf("Writing faces...\n");
	m_nType = 4;
	RecTraversalCell(m_pRoot, 0, 0);

	fclose(m_pWriteFile);
	fclose(m_pSOGFile);

	printf("Total write %d vertices and %d triangles.\n", m_nVertexNum, m_nFaceNum);
}

void COcTree::RecRegularTest(UCHAR * pNode, int & nRegularCount)
{
	UCHAR nType;
	UCHAR nNext;
	float fNext[3];
	if (!m_bCountOnly)
		fread(&nType, 1, 1, m_pSOGFile);

	if (NOT_LEAF(pNode)) {
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					RecRegularTest(LeafNode(pNode, i, j, k), nRegularCount);
	} else {
		if (!m_bCountOnly)
			fread(&nNext, 1, 1, m_pSOGFile);
		if (!m_bCountOnly && (*pNode & 0x3) == 2)
			fread(fNext, 4, 3, m_pSOGFile);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode;
		if ((*pNode & 0x3) == 2 && GET_CELL_INSIDE(pTempNode->m_n64Type, 0) == 0) {
			int nPointTag = (int)(pTempNode->m_n64Type >> 4) & 0xff;

			//if (!GET_CHANGED(pTempNode->m_n64Type)) {
			bool bRegularTest = (m_pConstant->m_pRegularTableEx[nPointTag][2] == 1);
			if (bRegularTest) {
				for (int i = 0; i < 12; i++)
					if (!GET_EDGE_INSIDE(pTempNode->m_n64Type, i)
						&& GET_POINT_INSIDE(pTempNode->m_n64Type, CConstant::I_SHIFT_EP[i][0])
						&& GET_POINT_INSIDE(pTempNode->m_n64Type, CConstant::I_SHIFT_EP[i][1])
						)
					bRegularTest = false;
			}
			if (bRegularTest) {
				for (int i = 0; i < 6; i++)
					if (!GET_FACE_INSIDE(pTempNode->m_n64Type, i)
						&& GET_POINT_INSIDE(pTempNode->m_n64Type, CConstant::I_SHIFT_FP[i][0])
						&& GET_POINT_INSIDE(pTempNode->m_n64Type, CConstant::I_SHIFT_FP[i][1])
						&& GET_POINT_INSIDE(pTempNode->m_n64Type, CConstant::I_SHIFT_FP[i][2])
						&& GET_POINT_INSIDE(pTempNode->m_n64Type, CConstant::I_SHIFT_FP[i][3])
						)
					bRegularTest = false;
			}

			if (bRegularTest) {
				if (m_bCountOnly) {
					if (pTempNode->m_nVertex == 0xfffffffe)
						pTempNode->m_nVertex = m_nVertexNum;
					else
						pTempNode->m_nVertex = 0xfffffffe;
					nRegularCount++;
					m_nVertexNum ++;
				} else {
					pTempNode->m_nVertex = m_nVertexNum;
					WriteVertex(fNext);
					nRegularCount++;
					m_nVertexNum ++;
				}
			} else {
				if (m_bCountOnly) {
					m_nIrregularUnchangedCell ++;
				} else {
					m_pSingleHashMap->Set((int)pNode, CVector3D(fNext[0], fNext[1], fNext[2]));
				}
			}
			//}
		}
	}
}

void COcTree::RecCreateFaceFaceEdgeTraversal(UCHAR * pCoNode, UINT64 n64CoPosition, int nCoLevel, UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, int direction, list<int> & listIndex)
{
	// direction : 0 is for normal, 1 is for opposite
	UCHAR * pNodeEdge[2][2];
	UINT64 n64Edge[2][2];
	int nEdge[2][2];

	if (NOT_LEAF(pNode[0][0]) || NOT_LEAF(pNode[0][1]) || NOT_LEAF(pNode[1][0]) || NOT_LEAF(pNode[1][1])) {
		switch(dirEdge) {
		case 0:
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], direction == 0 ? i : 1-i, 1-j, 1-k, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				RecCreateFaceFaceEdgeTraversal(pCoNode, n64CoPosition, nCoLevel, pNodeEdge, n64Edge, nEdge, dirEdge, direction, listIndex);
			}
			break;
		case 1:
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], 1-k, direction == 0 ? i : 1-i, 1-j, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				RecCreateFaceFaceEdgeTraversal(pCoNode, n64CoPosition, nCoLevel, pNodeEdge, n64Edge, nEdge, dirEdge, direction, listIndex);
			}
			break;
		case 2:
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], 1-j, 1-k, direction == 0 ? i : 1-i, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				RecCreateFaceFaceEdgeTraversal(pCoNode, n64CoPosition, nCoLevel, pNodeEdge, n64Edge, nEdge, dirEdge, direction, listIndex);
			}
			break;
		}
	} else {
		int nI, nJ, nIndex;
		UINT64 n64Point;
		GetStorageEdge(nLevel, dirEdge, nI, nJ, nIndex);
		GetEdgePos(n64Position, nLevel, dirEdge, nI, nJ, direction, n64Point);
		InsertIndex(listIndex, GetIndex(pCoNode, n64CoPosition, n64Point));
	}
}

//////////////////////////////////////////////////////////////////////////
// Thinning code
//////////////////////////////////////////////////////////////////////////
void COcTree::Thin()
{
	// Initialization

	// Thinning
	printf("\n\tMain thinning process start :");
	printf("\n\tStep count - %3d", 0);
	DWORD dwStart = GetTickCount();
	m_nStep = 0;
	bool bContinue = true;

	while (bContinue) {
		if (m_nStep % 2 == 0) {
			m_nTestOut = 2;
			m_nTestLastOut = 3;
		} else {
			m_nTestOut = 3;
			m_nTestLastOut = 2;
		}
		printf("\b\b\b%3d", m_nStep);
		bContinue = RecThinCell(m_pRoot, 0, 0);
		m_nStep++;
	}

	printf("\b\b\b%3d", m_nStep);
	DWORD dwEnd = GetTickCount();
	printf("\n\tMain thinning process finished.\n\tTime consuming %d.%d second(s) in %d step(s).", (dwEnd - dwStart) / 1000, (dwEnd - dwStart) % 1000, m_nStep);

	// Finallization
	printf("\n\n");
}

void COcTree::RecFixTags(UCHAR * pNode, int nType)
//////////////////////////////////////////////////////////////////////////
// Meaning of nType
// 0 -- I/O tags --> Simple tags
// 1 -- Simple tags --> I/O tags
// 2 -- I/O tags outside | Simple Tags 11 == outside
// 3 -- I/O tags inside | Simple Tags 11 == inside
//////////////////////////////////////////////////////////////////////////
{
	if (NOT_LEAF(pNode)) {
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					RecFixTags(LeafNode(pNode, i, j, k), nType);
	} else {
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode;
		UCHAR * pTemp = pNode + sizeof(LEAF_NODE);
		UINT64 n64Temp;

		switch(nType) {
		case 0:
			for (int i = 0; i < 8; i++)
				SetObjectSimple(pTempNode->m_n64SimpleTag, i, 0, GET_POINT_INSIDE(pTempNode->m_n64Type, i) ? 0 : 1);
			for (int i = 0; i < 12; i++)
				SetObjectSimple(pTempNode->m_n64SimpleTag, i, 1, GET_EDGE_INSIDE(pTempNode->m_n64Type, i) ? 0 : 1);
			for (int i = 0; i < 6; i++)
				SetObjectSimple(pTempNode->m_n64SimpleTag, i, 2, GET_FACE_INSIDE(pTempNode->m_n64Type, i) ? 0 : 1);
			for (int i = 0; i < 1; i++)
				SetObjectSimple(pTempNode->m_n64SimpleTag, i, 3, GET_CELL_INSIDE(pTempNode->m_n64Type, i) ? 0 : 1);
			for (int i = 0; i < 6; i++)
				if (GET_FACE_RIT(pTempNode->m_n64Type, i)) {
					((RETRACE_EDGE *)pTemp)->m_fWeight = 0.0;
					((RETRACE_EDGE *)pTemp)->m_n64PositionEdge = INT64_INVALID;
					pTemp += sizeof(RETRACE_EDGE);
				}
			for (int i = 0; i < 12; i++)
				SetEdgeRI(pTempNode->m_n64Type, i, 0);
			break;
		case 1:
			for (int i = 0; i < 8; i++)
				SetObjectInside(pTempNode->m_n64Type, i, 0, GET_POINT_SIMPLE(pTempNode->m_n64SimpleTag, i) == 0);
			for (int i = 0; i < 12; i++)
				SetObjectInside(pTempNode->m_n64Type, i, 1, GET_EDGE_SIMPLE(pTempNode->m_n64SimpleTag, i) == 0);
			for (int i = 0; i < 6; i++)
				SetObjectInside(pTempNode->m_n64Type, i, 2, GET_FACE_SIMPLE(pTempNode->m_n64SimpleTag, i) == 0);
			for (int i = 0; i < 1; i++)
				SetObjectInside(pTempNode->m_n64Type, i, 3, GET_CELL_SIMPLE(pTempNode->m_n64SimpleTag, i) == 0);
			break;
		case 2:
			n64Temp = pTempNode->m_n64Type;
			for (int i = 0; i < 8; i++)
				SetObjectInside(pTempNode->m_n64Type, i, 0, GET_POINT_SIMPLE(pTempNode->m_n64SimpleTag, i) != 3 && GET_POINT_INSIDE(pTempNode->m_n64Type, i));
			for (int i = 0; i < 12; i++)
				SetObjectInside(pTempNode->m_n64Type, i, 1, GET_EDGE_SIMPLE(pTempNode->m_n64SimpleTag, i) != 3 && GET_EDGE_INSIDE(pTempNode->m_n64Type, i));
			for (int i = 0; i < 6; i++)
				SetObjectInside(pTempNode->m_n64Type, i, 2, GET_FACE_SIMPLE(pTempNode->m_n64SimpleTag, i) != 3 && GET_FACE_INSIDE(pTempNode->m_n64Type, i));
			for (int i = 0; i < 1; i++)
				SetObjectInside(pTempNode->m_n64Type, i, 3, GET_CELL_SIMPLE(pTempNode->m_n64SimpleTag, i) != 3 && GET_CELL_INSIDE(pTempNode->m_n64Type, i));
			if (n64Temp != pTempNode->m_n64Type) {
				SetChangedA(pTempNode->m_n64Type, true);
			}
			break;
		case 3:
			n64Temp = pTempNode->m_n64Type;
			for (int i = 0; i < 8; i++)
				SetObjectInside(pTempNode->m_n64Type, i, 0, GET_POINT_SIMPLE(pTempNode->m_n64SimpleTag, i) == 3 || GET_POINT_INSIDE(pTempNode->m_n64Type, i));
			for (int i = 0; i < 12; i++)
				SetObjectInside(pTempNode->m_n64Type, i, 1, GET_EDGE_SIMPLE(pTempNode->m_n64SimpleTag, i) == 3 || GET_EDGE_INSIDE(pTempNode->m_n64Type, i));
			for (int i = 0; i < 6; i++)
				SetObjectInside(pTempNode->m_n64Type, i, 2, GET_FACE_SIMPLE(pTempNode->m_n64SimpleTag, i) == 3 || GET_FACE_INSIDE(pTempNode->m_n64Type, i));
			for (int i = 0; i < 1; i++)
				SetObjectInside(pTempNode->m_n64Type, i, 3, GET_CELL_SIMPLE(pTempNode->m_n64SimpleTag, i) == 3 || GET_CELL_INSIDE(pTempNode->m_n64Type, i));
			if (n64Temp != pTempNode->m_n64Type) {
				SetChangedB(pTempNode->m_n64Type, true);
			}
			break;
		}
	}
	SetLastTime(pNode, true);
	SetLastFix(pNode, true);
}

bool COcTree::RecThinCell(UCHAR * pNode, UINT64 n64Position, int nLevel)
{
	SetLastFix(pNode, GET_LAST_TIME(pNode));

	if (!GET_LAST_TIME(pNode))
		return false;

	bool bRtn = false;

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
					if (RecThinCell(pNodeCell, n64Cell, nCell))
						bRtn = true;
				}
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					pNodeFace[k] = pNodePoint[k][i][j];
					n64Face[k] = n64Point[k][i][j];
					nFace[k] = nPoint[k][i][j];
				}
				if (RecThinFace(pNodeFace, n64Face, nFace, 0))
					bRtn = true;
				for (int k = 0; k < 2; k++)
				{
					pNodeFace[k] = pNodePoint[j][k][i];
					n64Face[k] = n64Point[j][k][i];
					nFace[k] = nPoint[j][k][i];
				}
				if (RecThinFace(pNodeFace, n64Face, nFace, 1))
					bRtn = true;
				for (int k = 0; k < 2; k++)
				{
					pNodeFace[k] = pNodePoint[i][j][k];
					n64Face[k] = n64Point[i][j][k];
					nFace[k] = nPoint[i][j][k];
				}
				if (RecThinFace(pNodeFace, n64Face, nFace, 2))
					bRtn = true;
			}
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					pNodeEdge[j][k] = pNodePoint[i][j][k];
					n64Edge[j][k] = n64Point[i][j][k];
					nEdge[j][k] = nPoint[i][j][k];
				}
			if (RecThinEdge(pNodeEdge, n64Edge, nEdge, 0))
				bRtn = true;
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					pNodeEdge[j][k] = pNodePoint[k][i][j];
					n64Edge[j][k] = n64Point[k][i][j];
					nEdge[j][k] = nPoint[k][i][j];
				}
			if (RecThinEdge(pNodeEdge, n64Edge, nEdge, 1))
				bRtn = true;
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					pNodeEdge[j][k] = pNodePoint[j][k][i];
					n64Edge[j][k] = n64Point[j][k][i];
					nEdge[j][k] = nPoint[j][k][i];
				}
			if (RecThinEdge(pNodeEdge, n64Edge, nEdge, 2))
				bRtn = true;
		}
		if (RecThinPoint(pNodePoint, n64Point, nPoint))
			bRtn = true;
	} else {
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode;
		UINT64 nSimpleTag = GET_CELL_SIMPLE(pTempNode->m_n64SimpleTag, 0);
		if (nSimpleTag == m_nTestLastOut) {
			SetObjectSimple(pTempNode->m_n64SimpleTag, 0, 3, 1);
		}
	}

	SetLastTime(pNode, bRtn);

	return bRtn;
}

bool COcTree::RecThinFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace)
{
	if (!GET_LAST_FIX(pNode[0]) && !GET_LAST_FIX(pNode[1]))
		return false;

	bool bRtn = false;

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
					for (int k = 0; k < 2; k++)
					{
						pNodeFace[k] = pNodePoint[k][i][j];
						n64Face[k] = n64Point[k][i][j];
						nFace[k] = nPoint[k][i][j];
					}
					if (RecThinFace(pNodeFace, n64Face, nFace, 0))
						bRtn = true;
				}
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[k][i][j];
						n64Edge[j][k] = n64Point[k][i][j];
						nEdge[j][k] = nPoint[k][i][j];
					}
				if (RecThinEdge(pNodeEdge, n64Edge, nEdge, 1))
					bRtn = true;
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[j][k][i];
						n64Edge[j][k] = n64Point[j][k][i];
						nEdge[j][k] = nPoint[j][k][i];
					}
				if (RecThinEdge(pNodeEdge, n64Edge, nEdge, 2))
					bRtn = true;
			}
			if (RecThinPoint(pNodePoint, n64Point, nPoint))
				bRtn = true;
			break;
		case 1:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j], n64Position[j], nLevel[j], i, 1-j, k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
					{
						pNodeFace[k] = pNodePoint[j][k][i];
						n64Face[k] = n64Point[j][k][i];
						nFace[k] = nPoint[j][k][i];
					}
					if (RecThinFace(pNodeFace, n64Face, nFace, 1))
						bRtn = true;
				}
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[j][k][i];
						n64Edge[j][k] = n64Point[j][k][i];
						nEdge[j][k] = nPoint[j][k][i];
					}
				if (RecThinEdge(pNodeEdge, n64Edge, nEdge, 2))
					bRtn = true;
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[i][j][k];
						n64Edge[j][k] = n64Point[i][j][k];
						nEdge[j][k] = nPoint[i][j][k];
					}
				if (RecThinEdge(pNodeEdge, n64Edge, nEdge, 0))
					bRtn = true;
			}
			if (RecThinPoint(pNodePoint, n64Point, nPoint))
				bRtn = true;
			break;
		case 2:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k], n64Position[k], nLevel[k], i, j, 1-k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
					{
						pNodeFace[k] = pNodePoint[i][j][k];
						n64Face[k] = n64Point[i][j][k];
						nFace[k] = nPoint[i][j][k];
					}
					if (RecThinFace(pNodeFace, n64Face, nFace, 2))
						bRtn = true;
				}
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[i][j][k];
						n64Edge[j][k] = n64Point[i][j][k];
						nEdge[j][k] = nPoint[i][j][k];
					}
				if (RecThinEdge(pNodeEdge, n64Edge, nEdge, 0))
					bRtn = true;
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[k][i][j];
						n64Edge[j][k] = n64Point[k][i][j];
						nEdge[j][k] = nPoint[k][i][j];
					}
				if (RecThinEdge(pNodeEdge, n64Edge, nEdge, 1))
					bRtn = true;
			}
			if (RecThinPoint(pNodePoint, n64Point, nPoint))
				bRtn = true;
			break;
		}
	} else {
		int nI, nIndex;
		GetStorageFace(nLevel, dirFace, nI, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI];
		UINT64 nSimpleTag = GET_FACE_SIMPLE(pTempNode->m_n64SimpleTag, nIndex);
		if (nSimpleTag == m_nTestLastOut) {
			SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 2, 1);
		} else if (nSimpleTag == 0) {
			int nSimpleTest = 0;
			UCHAR * pNodeUniqueCell;
			for (int i = 0; i < 2; i++)
			{
				nSimpleTag = GET_CELL_SIMPLE(((LEAF_NODE *)pNode[i])->m_n64SimpleTag, 0);
				if (nSimpleTag == m_nTestOut) {
					nSimpleTest = 100;
					break;
				} else if (nSimpleTag == 0) {
					nSimpleTest++;
					if (nSimpleTest == 1) {
						pNodeUniqueCell = pNode[i];
					} else {
						break;
					}
				}
			}
			if (nSimpleTest == 1) {
				SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 2, m_nTestOut);
				SetObjectSimple(((LEAF_NODE *)pNodeUniqueCell)->m_n64SimpleTag, 0, 3, m_nTestOut);
				bRtn = true;
			}
		}
	}

	if (bRtn)
		for (int i = 0; i < 2; i++)
			SetLastTime(pNode[i], bRtn);

	return bRtn;
}

bool COcTree::RecThinEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge)
{
	if (!GET_LAST_FIX(pNode[0][0]) && !GET_LAST_FIX(pNode[0][1]) && !GET_LAST_FIX(pNode[1][0]) && !GET_LAST_FIX(pNode[1][1]))
		return false;

	bool bRtn = false;

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
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[i][j][k];
						n64Edge[j][k] = n64Point[i][j][k];
						nEdge[j][k] = nPoint[i][j][k];
					}
				if (RecThinEdge(pNodeEdge, n64Edge, nEdge, 0))
					bRtn = true;
			}
			if (RecThinPoint(pNodePoint, n64Point, nPoint))
				bRtn = true;
			break;
		case 1:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k][i], n64Position[k][i], nLevel[k][i], 1-i, j, 1-k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[k][i][j];
						n64Edge[j][k] = n64Point[k][i][j];
						nEdge[j][k] = nPoint[k][i][j];
					}
				if (RecThinEdge(pNodeEdge, n64Edge, nEdge, 1))
					bRtn = true;
			}
			if (RecThinPoint(pNodePoint, n64Point, nPoint))
				bRtn = true;
			break;
		case 2:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[i][j], n64Position[i][j], nLevel[i][j], 1-i, 1-j, k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[j][k][i];
						n64Edge[j][k] = n64Point[j][k][i];
						nEdge[j][k] = nPoint[j][k][i];
					}
				if (RecThinEdge(pNodeEdge, n64Edge, nEdge, 2))
					bRtn = true;
			}
			if (RecThinPoint(pNodePoint, n64Point, nPoint))
				bRtn = true;
			break;
		}
	} else {
		int nI, nJ, nIndex;
		GetStorageEdge(nLevel, dirEdge, nI, nJ, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI][nJ];
		UINT64 nSimpleTag = GET_EDGE_SIMPLE(pTempNode->m_n64SimpleTag, nIndex);
		UCHAR * pNodeFace[2];
		int nFace[2];
		int dirFace;
		int nFaceI, nFaceIndex;
		UCHAR * pNodeUniqueFace;
		int nUniqueFaceIndex;

		if (nSimpleTag == m_nTestLastOut) {
			SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 1, 1);
		} else if (nSimpleTag == 0) {
			int nSimpleTest = 0;
			float fWeight = 0.0;
			RETRACE_EDGE * pRetraceEdge;
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++) {
					pNodeFace[j] = pNode[i][j];
					nFace[j] = nLevel[i][j];
				}
				dirFace = (dirEdge + 2) % 3;
				if (CheckStorageFace(pNodeFace)) {
					GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
					pRetraceEdge = GetRetraceEdge(pNodeFace[nFaceI], nFaceIndex);
					fWeight += pRetraceEdge->m_fWeight;
					nSimpleTag = GET_FACE_SIMPLE(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64SimpleTag, nFaceIndex);
					if (nSimpleTag == m_nTestOut) {
						nSimpleTest = 100;
						break;
					} else if (nSimpleTag == 0) {
						nSimpleTest++;
						if (nSimpleTest == 1) {
							pNodeUniqueFace = pNodeFace[nFaceI];
							nUniqueFaceIndex = nFaceIndex;
						} else {
							break;
						}
					}
				}
				for (int j = 0; j < 2; j++) {
					pNodeFace[j] = pNode[j][i];
					nFace[j] = nLevel[j][i];
				}
				dirFace = (dirEdge + 1) % 3;
				if (CheckStorageFace(pNodeFace)) {
					GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
					pRetraceEdge = GetRetraceEdge(pNodeFace[nFaceI], nFaceIndex);
					fWeight += pRetraceEdge->m_fWeight;
					nSimpleTag = GET_FACE_SIMPLE(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64SimpleTag, nFaceIndex);
					if (nSimpleTag == m_nTestOut) {
						nSimpleTest = 100;
						break;
					} else if (nSimpleTag == 0) {
						nSimpleTest++;
						if (nSimpleTest == 1) {
							pNodeUniqueFace = pNodeFace[nFaceI];
							nUniqueFaceIndex = nFaceIndex;
						} else {
							break;
						}
					}
				}
			}
			if (nSimpleTest == 1) {
				fWeight += GetEdgeArea(n64Position, nLevel);
				SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 1, m_nTestOut);
				UINT64 n64Edge;
				GetEdgePos(n64Position, nLevel, dirEdge, nI, nJ, 0, n64Edge);
				SetObjectSimple(((LEAF_NODE *)pNodeUniqueFace)->m_n64SimpleTag, nUniqueFaceIndex, 2, m_nTestOut);
				pRetraceEdge = GetRetraceEdge(pNodeUniqueFace, nUniqueFaceIndex);
				pRetraceEdge->m_fWeight = fWeight;
				pRetraceEdge->m_n64PositionEdge = INT64COMBD(n64Edge, dirEdge);
				bRtn = true;
			}
		}
	}

	if (bRtn)
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				SetLastTime(pNode[i][j], bRtn);

	return bRtn;
}

bool COcTree::RecThinPoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2])
{
	if (!GET_LAST_FIX(pNode[0][0][0]) && !GET_LAST_FIX(pNode[0][0][1]) && !GET_LAST_FIX(pNode[0][1][0]) && !GET_LAST_FIX(pNode[0][1][1])
		&& !GET_LAST_FIX(pNode[1][0][0]) && !GET_LAST_FIX(pNode[1][0][1]) && !GET_LAST_FIX(pNode[1][1][0]) && !GET_LAST_FIX(pNode[1][1][1]))
		return false;

	bool bRtn = false;

	if ((*(pNode[0][0][0]) & 0x3) == 0 || (*(pNode[0][0][1]) & 0x3) == 0 || (*(pNode[0][1][0]) & 0x3) == 0 || (*(pNode[0][1][1]) & 0x3) == 0 || 
		(*(pNode[1][0][0]) & 0x3) == 0 || (*(pNode[1][0][1]) & 0x3) == 0 || (*(pNode[1][1][0]) & 0x3) == 0 || (*(pNode[1][1][1]) & 0x3) == 0) {
		UCHAR * pNodePoint[2][2][2];
		UINT64 n64Point[2][2][2];
		int nPoint[2][2][2];
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					Leaf(pNode[i][j][k], n64Position[i][j][k], nLevel[i][j][k], 1-i, 1-j, 1-k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
		if (RecThinPoint(pNodePoint, n64Point, nPoint))
			bRtn = true;
	} else {
		int nI, nJ, nK, nIndex;
		GetStoragePoint(nLevel, nI, nJ, nK, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI][nJ][nK];
		UINT64 nSimpleTag = GET_POINT_SIMPLE(pTempNode->m_n64SimpleTag, nIndex);
		UCHAR * pNodeEdge[2][2];
		int nEdge[2][2];
		int dirEdge;
		int nEdgeI, nEdgeJ, nEdgeIndex;
		UCHAR * pNodeUniqueEdge;
		int nUniqueEdgeIndex;

		if (nSimpleTag == m_nTestLastOut) {
			SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 0, 1);
		} else if (nSimpleTag == 0) {
			int nSimpleTest = 0;
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNode[i][j][k];
						nEdge[j][k] = nLevel[i][j][k];
					}
				if (CheckStorageEdge(pNodeEdge)) {
					dirEdge = 0;
					GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
					nSimpleTag = GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex);
					if (nSimpleTag == m_nTestOut) {
						nSimpleTest = 100;
						break;
					} else if (nSimpleTag == 0) {
						nSimpleTest++;
						if (nSimpleTest == 1) {
							pNodeUniqueEdge = pNodeEdge[nEdgeI][nEdgeJ];
							nUniqueEdgeIndex = nEdgeIndex;
						} else {
							break;
						}
					}
				}
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNode[k][i][j];
						nEdge[j][k] = nLevel[k][i][j];
					}
				if (CheckStorageEdge(pNodeEdge)) {
					dirEdge = 1;
					GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
					nSimpleTag = GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex);
					if (nSimpleTag == m_nTestOut) {
						nSimpleTest = 100;
						break;
					} else if (nSimpleTag == 0) {
						nSimpleTest++;
						if (nSimpleTest == 1) {
							pNodeUniqueEdge = pNodeEdge[nEdgeI][nEdgeJ];
							nUniqueEdgeIndex = nEdgeIndex;
						} else {
							break;
						}
					}
				}
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNode[j][k][i];
						nEdge[j][k] = nLevel[j][k][i];
					}
				if (CheckStorageEdge(pNodeEdge)) {
					dirEdge = 2;
					GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
					nSimpleTag = GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex);
					if (nSimpleTag == m_nTestOut) {
						nSimpleTest = 100;
						break;
					} else if (nSimpleTag == 0) {
						nSimpleTest++;
						if (nSimpleTest == 1) {
							pNodeUniqueEdge = pNodeEdge[nEdgeI][nEdgeJ];
							nUniqueEdgeIndex = nEdgeIndex;
						} else {
							break;
						}
					}
				}
			}
			if (nSimpleTest == 1) {
				// SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 0, m_nTestOut);
				// SetObjectSimple(((LEAF_NODE *)pNodeUniqueEdge)->m_n64SimpleTag, nUniqueEdgeIndex, 1, m_nTestOut);
				SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 0, m_nTestOut);
				SetObjectSimple(((LEAF_NODE *)pNodeUniqueEdge)->m_n64SimpleTag, nUniqueEdgeIndex, 1, m_nTestOut);
				bRtn = true;
			}
		}
	}

	if (bRtn)
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					SetLastTime(pNode[i][j][k], bRtn);

#ifdef CLEAN_SKELETON
	return bRtn;
#else
	return false;
#endif
}

//////////////////////////////////////////////////////////////////////////
// Create Minimun Spanning Tree Code
//////////////////////////////////////////////////////////////////////////
void COcTree::CreateMST(float fThreshold, CString strSKTFile)
{
	// Initialization
	printf("\tInitialization...");
	m_listGrowSeedEdge.clear();
	const int MAX_HEAP_SIZE = 50000;
	CEdgeHeap heapEdge(MAX_HEAP_SIZE);
	m_nType = 6;
	RecTraversalCell(m_pRoot, 0, 0);
	int dirEdge;
	int nI, nJ, nK, nIndex;
	int nEdgeI, nEdgeJ, nEdgeIndex;
	UCHAR * pNodeEdge[2][2];
	UINT64 n64Edge[2][2];
	int nEdge[2][2];
	UCHAR * pNodePoint[2][2][2];
	UINT64 n64Point[2][2][2];
	int nPoint[2][2][2];
	float fEdge;

	GetStoragePoint(m_nPointStart, nI, nJ, nK, nIndex);
	SetObjectSimple(((LEAF_NODE *)m_pPointStart[nI][nJ][nK])->m_n64SimpleTag, nIndex, 0, 0);

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
			{
				pNodeEdge[j][k] = m_pPointStart[i][j][k];
				nEdge[j][k] = m_nPointStart[i][j][k];
				n64Edge[j][k] = m_n64PointStart[i][j][k];
			}
		dirEdge = 0;
		if (CheckStorageEdge(pNodeEdge)) {
			GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
			if (GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex) == 2) {
				SetObjectSimple(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex, 1, 3);
				fEdge = GetEdgeWeight(pNodeEdge, n64Edge, nEdge, dirEdge);
				heapEdge.Push(pNodeEdge, n64Edge, nEdge, dirEdge, fEdge);
			}
		}
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
			{
				pNodeEdge[j][k] = m_pPointStart[k][i][j];
				nEdge[j][k] = m_nPointStart[k][i][j];
				n64Edge[j][k] = m_n64PointStart[k][i][j];
			}
		dirEdge = 1;
		if (CheckStorageEdge(pNodeEdge)) {
			GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
			if (GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex) == 2) {
				SetObjectSimple(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex, 1, 3);
				fEdge = GetEdgeWeight(pNodeEdge, n64Edge, nEdge, dirEdge);
				heapEdge.Push(pNodeEdge, n64Edge, nEdge, dirEdge, fEdge);
			}
		}
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
			{
				pNodeEdge[j][k] = m_pPointStart[j][k][i];
				nEdge[j][k] = m_nPointStart[j][k][i];
				n64Edge[j][k] = m_n64PointStart[j][k][i];
			}
		dirEdge = 2;
		if (CheckStorageEdge(pNodeEdge)) {
			GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
			if (GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex) == 2) {
				SetObjectSimple(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex, 1, 3);
				fEdge = GetEdgeWeight(pNodeEdge, n64Edge, nEdge, dirEdge);
				heapEdge.Push(pNodeEdge, n64Edge, nEdge, dirEdge, fEdge);
			}
		}
	}

	bool bOutput = (strSKTFile.GetLength() != 0);
	UCHAR nType;
	FILE * pFile;
	if (bOutput) {
		pFile = fopen(strSKTFile.GetBuffer(0), "wb");
	}

	// Create MST
	printf("\n\tMain process of creating MST...");
	DWORD dwStart = GetTickCount();

	while (!heapEdge.Empty()) {
		CEdgeHeapElement cElement = heapEdge.Pop();
		GetStorageEdge(cElement.m_nLevel, cElement.m_dirEdge, nI, nJ, nIndex);
		if (GetEdgeNeighborPoint(cElement.m_pNode, cElement.m_n64Position, cElement.m_nLevel, cElement.m_dirEdge, pNodePoint, n64Point, nPoint) || cElement.m_fValue > fThreshold) {
			SetObjectSimple(((LEAF_NODE *)cElement.m_pNode[nI][nJ])->m_n64SimpleTag, nIndex, 1, 0);
			GetStoragePoint(nPoint, nI, nJ, nK, nIndex);
			SetObjectSimple(((LEAF_NODE *)pNodePoint[nI][nJ][nK])->m_n64SimpleTag, nIndex, 0, 0);
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[i][j][k];
						nEdge[j][k] = nPoint[i][j][k];
						n64Edge[j][k] = n64Point[i][j][k];
					}
				dirEdge = 0;
				if (CheckStorageEdge(pNodeEdge)) {
					GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
					if (GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex) == 2) {
						SetObjectSimple(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex, 1, 3);
						fEdge = GetEdgeWeight(pNodeEdge, n64Edge, nEdge, dirEdge);
						heapEdge.Push(pNodeEdge, n64Edge, nEdge, dirEdge, fEdge);
					}
				}
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[k][i][j];
						nEdge[j][k] = nPoint[k][i][j];
						n64Edge[j][k] = n64Point[k][i][j];
					}
				dirEdge = 1;
				if (CheckStorageEdge(pNodeEdge)) {
					GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
					if (GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex) == 2) {
						SetObjectSimple(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex, 1, 3);
						fEdge = GetEdgeWeight(pNodeEdge, n64Edge, nEdge, dirEdge);
						heapEdge.Push(pNodeEdge, n64Edge, nEdge, dirEdge, fEdge);
					}
				}
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[j][k][i];
						nEdge[j][k] = nPoint[j][k][i];
						n64Edge[j][k] = n64Point[j][k][i];
					}
				dirEdge = 2;
				if (CheckStorageEdge(pNodeEdge)) {
					GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
					if (GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex) == 2) {
						SetObjectSimple(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex, 1, 3);
						fEdge = GetEdgeWeight(pNodeEdge, n64Edge, nEdge, dirEdge);
						heapEdge.Push(pNodeEdge, n64Edge, nEdge, dirEdge, fEdge);
					}
				}
			}
			if (bOutput) {
				nType = 0;				// Non-broken skeleton
				UINT64 n64Pos;
				fwrite(&nType, 1, 1, pFile);
				fwrite(&cElement.m_fValue, sizeof(float), 1, pFile);
				GetStorageEdge(cElement.m_nLevel, cElement.m_dirEdge, nI, nJ, nIndex);
				GetEdgePos(cElement.m_n64Position, cElement.m_nLevel, cElement.m_dirEdge, nI, nJ, 0, n64Pos);
				fwrite(&n64Pos, sizeof(UINT64), 1, pFile);
				GetEdgePos(cElement.m_n64Position, cElement.m_nLevel, cElement.m_dirEdge, nI, nJ, 1, n64Pos);
				fwrite(&n64Pos, sizeof(UINT64), 1, pFile);
			}
		} else {
			printf("\n\t\tA cut with weight : %8.6f", cElement.m_fValue);
			//SetObjectSimple(((LEAF_NODE *)cElement.m_pNode[nI][nJ])->m_n64SimpleTag, nIndex, 1, 1);
			m_listGrowSeedEdge.push_back(cElement);
			if (bOutput) {
				nType = 1;				// Non-broken skeleton
				UINT64 n64Pos;
				fwrite(&nType, 1, 1, pFile);
				fwrite(&cElement.m_fValue, sizeof(float), 1, pFile);
				GetStorageEdge(cElement.m_nLevel, cElement.m_dirEdge, nI, nJ, nIndex);
				GetEdgePos(cElement.m_n64Position, cElement.m_nLevel, cElement.m_dirEdge, nI, nJ, 0, n64Pos);
				fwrite(&n64Pos, sizeof(UINT64), 1, pFile);
				GetEdgePos(cElement.m_n64Position, cElement.m_nLevel, cElement.m_dirEdge, nI, nJ, 1, n64Pos);
				fwrite(&n64Pos, sizeof(UINT64), 1, pFile);
			}
		}
	}

	DWORD dwEnd = GetTickCount();
	printf("\n\tMain process finished.\n\tTime consuming %d.%d second(s).", (dwEnd - dwStart) / 1000, (dwEnd - dwStart) % 1000);
	printf("\n\tFinal heap size is %d.", heapEdge.m_nSize);

	// Finallization
	printf("\n\tFinalization...\n\n");
	if (bOutput) {
		nType = 0xff;
		fwrite(&nType, 1, 1, pFile);
		fclose(pFile);
	}
}

//////////////////////////////////////////////////////////////////////////
// Growing code
//////////////////////////////////////////////////////////////////////////
void COcTree::Grow()
{
	DWORD dwStart = GetTickCount();

	list<CEdgeHeapElement>::iterator it;
	for (it = m_listGrowSeedEdge.begin(); it != m_listGrowSeedEdge.end(); it++)
	{
		GrowByEdge(*it);
	}

	DWORD dwEnd = GetTickCount();
	printf("\tGrow cutting plane finished.\n\tTime consuming %d.%d second(s).\n\n", (dwEnd - dwStart) / 1000, (dwEnd - dwStart) % 1000);
}

void COcTree::GrowByEdge(CEdgeHeapElement & cElement)
{
	UCHAR * pNodeFace[2];
	UINT64 n64Face[2];
	int nFace[2];
	int dirFace, nFaceI, nFaceIndex;
	UCHAR * pNodeEdge[2][2];
	UINT64 n64Edge[2][2];
	int nEdge[2][2];
	int dirEdge, nEdgeI, nEdgeJ, nEdgeIndex;
	RETRACE_EDGE * pRetraceEdge;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
		{
			if (GET_CELL_INSIDE(((LEAF_NODE *)cElement.m_pNode[i][j])->m_n64Type, 0)) {
				SetObjectSimple(((LEAF_NODE *)cElement.m_pNode[i][j])->m_n64SimpleTag, 0, 3, 3);
			}
		}
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++) {
			pNodeFace[j] = cElement.m_pNode[i][j];
			n64Face[j] = cElement.m_n64Position[i][j];
			nFace[j] = cElement.m_nLevel[i][j];
		}
		if (CheckStorageFace(pNodeFace)) {
			dirFace = (cElement.m_dirEdge + 2) % 3;
			GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
			if (GET_FACE_SIMPLE(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64SimpleTag, nFaceIndex) != 3 && GET_FACE_INSIDE(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64Type, nFaceIndex)) {
				SetObjectSimple(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64SimpleTag, nFaceIndex, 2, 3);
				pRetraceEdge = GetRetraceEdge(pNodeFace[nFaceI], nFaceIndex);
				if (pRetraceEdge->m_n64PositionEdge != INT64_INVALID) {
					dirEdge = INT64GETD(pRetraceEdge->m_n64PositionEdge);
					GetEdgeFromPos(
						INT64GETN(pRetraceEdge->m_n64PositionEdge), 
						MAX_LEVEL, 
						INT64GETD(pRetraceEdge->m_n64PositionEdge),
						pNodeEdge,
						n64Edge,
						nEdge
						);
					GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
					if (GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex) != 3) {
						SetObjectSimple(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex, 1, 3);
						GrowByEdge(CEdgeHeapElement(pNodeEdge, n64Edge, nEdge, dirEdge, 0.0f));
					}
				}
			}
		}
		for (int j = 0; j < 2; j++) {
			pNodeFace[j] = cElement.m_pNode[j][i];
			n64Face[j] = cElement.m_n64Position[j][i];
			nFace[j] = cElement.m_nLevel[j][i];
		}
		if (CheckStorageFace(pNodeFace)) {
			dirFace = (cElement.m_dirEdge + 1) % 3;
			GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
			if (GET_FACE_SIMPLE(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64SimpleTag, nFaceIndex) != 3 && GET_FACE_INSIDE(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64Type, nFaceIndex)) {
				SetObjectSimple(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64SimpleTag, nFaceIndex, 2, 3);
				pRetraceEdge = GetRetraceEdge(pNodeFace[nFaceI], nFaceIndex);
				if (pRetraceEdge->m_n64PositionEdge != INT64_INVALID) {
					dirEdge = INT64GETD(pRetraceEdge->m_n64PositionEdge);
					GetEdgeFromPos(
						INT64GETN(pRetraceEdge->m_n64PositionEdge), 
						MAX_LEVEL, 
						INT64GETD(pRetraceEdge->m_n64PositionEdge),
						pNodeEdge,
						n64Edge,
						nEdge
						);
					GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
					if (GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex) != 3) {
						SetObjectSimple(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex, 1, 3);
						GrowByEdge(CEdgeHeapElement(pNodeEdge, n64Edge, nEdge, dirEdge, 0.0f));
					}		
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Dual thinning code
//////////////////////////////////////////////////////////////////////////
void COcTree::DualThin()
{
	// Initialization
	SetLastTime((UCHAR *)&m_cInfiniteNode, true);
	SetLastFix((UCHAR *)&m_cInfiniteNode, true);

	// Thinning
	printf("\n\tMain dual thinning process start :");
	printf("\n\tStep count - %3d", 0);
	DWORD dwStart = GetTickCount();
	m_nStep = 0;
	bool bContinue = true;

	while (bContinue) {
		printf("\b\b\b%3d", m_nStep);

		m_bCountOnly = true;
		DualThinStep();

		m_bCountOnly = false;
		bContinue = DualThinStep();
		m_nStep++;
	}

	printf("\b\b\b%3d", m_nStep);
	DWORD dwEnd = GetTickCount();
	printf("\n\tMain thinning process finished.\n\tTime consuming %d.%d second(s) in %d step(s).", (dwEnd - dwStart) / 1000, (dwEnd - dwStart) % 1000, m_nStep);

	// Finallization
	printf("\n\n");
}

bool COcTree::DualThinStep()
{
	bool bRtn = false;
	UCHAR * pNodeFace[2];
	UCHAR * pNodeEdge[2][2];
	UCHAR * pNodePoint[2][2][2];
	UINT64 n64Face[2];
	UINT64 n64Edge[2][2];
	UINT64 n64Point[2][2][2];
	int nFace[2];
	int nEdge[2][2];
	int nPoint[2][2][2];
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
			{
				for (int ii = 0; ii < 2; ii++)
					for (int jj = 0; jj < 2; jj++)
						for (int kk = 0; kk < 2; kk++) {
							pNodePoint[ii][jj][kk] = (UCHAR *)(&m_cInfiniteNode);
							n64Point[ii][jj][kk] = 0;
							nPoint[ii][jj][kk] = -1;
						}
				pNodePoint[i][j][k] = m_pRoot;
				n64Point[i][j][k] = 0;
				nPoint[i][j][k] = 0;
				if (RecDualThinPoint(pNodePoint, n64Point, nPoint))
					bRtn = true;
			}
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
		{
			for (int ii = 0; ii < 2; ii++)
				for (int jj = 0; jj < 2; jj++) {
					pNodeEdge[ii][jj] = (UCHAR *)(&m_cInfiniteNode);
					n64Edge[ii][jj] = 0;
					nEdge[ii][jj] = -1;
				}
			pNodeEdge[i][j] = m_pRoot;
			n64Edge[i][j] = 0;
			nEdge[i][j] = 0;
			if (RecDualThinEdge(pNodeEdge, n64Edge, nEdge, 0))
				bRtn = true;
			if (RecDualThinEdge(pNodeEdge, n64Edge, nEdge, 1))
				bRtn = true;
			if (RecDualThinEdge(pNodeEdge, n64Edge, nEdge, 2))
				bRtn = true;
		}
	for (int i = 0; i < 2; i++)
	{
		for (int ii = 0; ii < 2; ii++) {
			pNodeFace[ii] = (UCHAR *)(&m_cInfiniteNode);
			n64Face[ii] = 0;
			nFace[ii] = -1;
		}
		pNodeFace[i] = m_pRoot;
		n64Face[i] = 0;
		nFace[i] = 0;
		if (RecDualThinFace(pNodeFace, n64Face, nFace, 0))
			bRtn = true;
		if (RecDualThinFace(pNodeFace, n64Face, nFace, 1))
			bRtn = true;
		if (RecDualThinFace(pNodeFace, n64Face, nFace, 2))
			bRtn = true;
	}
	if (RecDualThinCell(m_pRoot, 0, 0))
		bRtn = true;
	return bRtn;
}

bool COcTree::RecDualThinCell(UCHAR * pNode, UINT64 n64Position, int nLevel)
{
	bool bRtn = false;

	if (NOT_LEAF(pNode)) {
		if (GET_LAST_FIX(pNode)) {
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
			if (RecDualThinPoint(pNodePoint, n64Point, nPoint))
				bRtn = true;
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[i][j][k];
						n64Edge[j][k] = n64Point[i][j][k];
						nEdge[j][k] = nPoint[i][j][k];
					}
				if (RecDualThinEdge(pNodeEdge, n64Edge, nEdge, 0))
					bRtn = true;
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[k][i][j];
						n64Edge[j][k] = n64Point[k][i][j];
						nEdge[j][k] = nPoint[k][i][j];
					}
				if (RecDualThinEdge(pNodeEdge, n64Edge, nEdge, 1))
					bRtn = true;
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[j][k][i];
						n64Edge[j][k] = n64Point[j][k][i];
						nEdge[j][k] = nPoint[j][k][i];
					}
				if (RecDualThinEdge(pNodeEdge, n64Edge, nEdge, 2))
					bRtn = true;
			}
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
					{
						pNodeFace[k] = pNodePoint[k][i][j];
						n64Face[k] = n64Point[k][i][j];
						nFace[k] = nPoint[k][i][j];
					}
					if (RecDualThinFace(pNodeFace, n64Face, nFace, 0))
						bRtn = true;
					for (int k = 0; k < 2; k++)
					{
						pNodeFace[k] = pNodePoint[j][k][i];
						n64Face[k] = n64Point[j][k][i];
						nFace[k] = nPoint[j][k][i];
					}
					if (RecDualThinFace(pNodeFace, n64Face, nFace, 1))
						bRtn = true;
					for (int k = 0; k < 2; k++)
					{
						pNodeFace[k] = pNodePoint[i][j][k];
						n64Face[k] = n64Point[i][j][k];
						nFace[k] = nPoint[i][j][k];
					}
					if (RecDualThinFace(pNodeFace, n64Face, nFace, 2))
						bRtn = true;
				}
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++) {
						pNodeCell = pNodePoint[i][j][k];
						n64Cell = n64Point[i][j][k];
						nCell = nPoint[i][j][k];
						if (RecDualThinCell(pNodeCell, n64Cell, nCell))
							bRtn = true;
					}
		}
	} else {
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode;
		UINT64 nSimpleTag = GET_CELL_SIMPLE(pTempNode->m_n64SimpleTag, 0);

		if (nSimpleTag != 0) {
			if (m_bCountOnly) {
				// do nothing
			} else {
				// reset
				SetObjectSimple(pTempNode->m_n64SimpleTag, 0, 3, 1);
			}
		}
	}

	if (!m_bCountOnly) {
		if (GET_LAST_TIME(pNode))
			bRtn = true;
		SetLastFix(pNode, bRtn);
		SetLastTime(pNode, false);
	}

	return bRtn;
}

bool COcTree::RecDualThinFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace)
{
	bool bRtn = false;

	if ((*(pNode[0]) & 0x3) == 0 || (*(pNode[1]) & 0x3) == 0) {
		if (GET_LAST_FIX(pNode[0]) || GET_LAST_FIX(pNode[1])) {
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
				if (RecDualThinPoint(pNodePoint, n64Point, nPoint))
					bRtn = true;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[k][i][j];
							n64Edge[j][k] = n64Point[k][i][j];
							nEdge[j][k] = nPoint[k][i][j];
						}
					if (RecDualThinEdge(pNodeEdge, n64Edge, nEdge, 1))
						bRtn = true;
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[j][k][i];
							n64Edge[j][k] = n64Point[j][k][i];
							nEdge[j][k] = nPoint[j][k][i];
						}
					if (RecDualThinEdge(pNodeEdge, n64Edge, nEdge, 2))
						bRtn = true;
				}
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
					{
						for (int k = 0; k < 2; k++)
						{
							pNodeFace[k] = pNodePoint[k][i][j];
							n64Face[k] = n64Point[k][i][j];
							nFace[k] = nPoint[k][i][j];
						}
						if (RecDualThinFace(pNodeFace, n64Face, nFace, 0))
							bRtn = true;
					}
				break;
			case 1:
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
							Leaf(pNode[j], n64Position[j], nLevel[j], i, 1-j, k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
				if (RecDualThinPoint(pNodePoint, n64Point, nPoint))
					bRtn = true;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[j][k][i];
							n64Edge[j][k] = n64Point[j][k][i];
							nEdge[j][k] = nPoint[j][k][i];
						}
					if (RecDualThinEdge(pNodeEdge, n64Edge, nEdge, 2))
						bRtn = true;
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[i][j][k];
							n64Edge[j][k] = n64Point[i][j][k];
							nEdge[j][k] = nPoint[i][j][k];
						}
					if (RecDualThinEdge(pNodeEdge, n64Edge, nEdge, 0))
						bRtn = true;
				}
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
					{
						for (int k = 0; k < 2; k++)
						{
							pNodeFace[k] = pNodePoint[j][k][i];
							n64Face[k] = n64Point[j][k][i];
							nFace[k] = nPoint[j][k][i];
						}
						if (RecDualThinFace(pNodeFace, n64Face, nFace, 1))
							bRtn = true;
					}
				break;
			case 2:
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
							Leaf(pNode[k], n64Position[k], nLevel[k], i, j, 1-k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
				if (RecDualThinPoint(pNodePoint, n64Point, nPoint))
					bRtn = true;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[i][j][k];
							n64Edge[j][k] = n64Point[i][j][k];
							nEdge[j][k] = nPoint[i][j][k];
						}
					if (RecDualThinEdge(pNodeEdge, n64Edge, nEdge, 0))
						bRtn = true;
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[k][i][j];
							n64Edge[j][k] = n64Point[k][i][j];
							nEdge[j][k] = nPoint[k][i][j];
						}
					if (RecDualThinEdge(pNodeEdge, n64Edge, nEdge, 1))
						bRtn = true;
				}
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
					{
						for (int k = 0; k < 2; k++)
						{
							pNodeFace[k] = pNodePoint[i][j][k];
							n64Face[k] = n64Point[i][j][k];
							nFace[k] = nPoint[i][j][k];
						}
						if (RecDualThinFace(pNodeFace, n64Face, nFace, 2))
							bRtn = true;
					}
				break;
			}
		}
	} else {
		int nI, nIndex;
		GetStorageFace(nLevel, dirFace, nI, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI];
		UINT64 nSimpleTag = GET_FACE_SIMPLE(pTempNode->m_n64SimpleTag, nIndex);
		if (nSimpleTag != 0) {
			if (m_bCountOnly) {
				for (int i = 0; i < 2; i++)
				{
					if (GET_LAST_FIX(pNode[i])) {
						nSimpleTag = GET_CELL_SIMPLE(((LEAF_NODE *)pNode[i])->m_n64SimpleTag, 0);	// 1 or 2 or 3
						if (nSimpleTag != 3 && NOT_INFINITE(pNode[i])) {
							SetObjectSimple(((LEAF_NODE *)pNode[i])->m_n64SimpleTag, 0, 3, nSimpleTag + 1);
						}
					}
				}
			} else {
				int i = 0;
				for (; i < 2; i++)
					if (GET_CELL_SIMPLE(((LEAF_NODE *)pNode[i])->m_n64SimpleTag, 0) == 2)
						break;
				if (i < 2) {
					SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 2, 0);
					SetObjectSimple(((LEAF_NODE *)pNode[i])->m_n64SimpleTag, 0, 3, 0);
#ifdef CLEAN_SKELETON
					bRtn = true;
#else
					bRtn = false;
#endif
				} else {
					SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 2, 1);
				}
			}
		}
	}

	if (bRtn)
		for (int i = 0; i < 2; i++)
			SetLastTime(pNode[i], bRtn);

	return bRtn;
}

bool COcTree::RecDualThinEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge)
{
	bool bRtn = false;

	if ((*(pNode[0][0]) & 0x3) == 0 || (*(pNode[0][1]) & 0x3) == 0 || (*(pNode[1][0]) & 0x3) == 0 || (*(pNode[1][1]) & 0x3) == 0) {
		if (GET_LAST_FIX(pNode[0][0]) || GET_LAST_FIX(pNode[0][1]) || GET_LAST_FIX(pNode[1][0]) || GET_LAST_FIX(pNode[1][1])) {
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
				if (RecDualThinPoint(pNodePoint, n64Point, nPoint))
					bRtn = true;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[i][j][k];
							n64Edge[j][k] = n64Point[i][j][k];
							nEdge[j][k] = nPoint[i][j][k];
						}
					if (RecDualThinEdge(pNodeEdge, n64Edge, nEdge, 0))
						bRtn = true;
				}
				break;
			case 1:
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
							Leaf(pNode[k][i], n64Position[k][i], nLevel[k][i], 1-i, j, 1-k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
				if (RecDualThinPoint(pNodePoint, n64Point, nPoint))
					bRtn = true;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[k][i][j];
							n64Edge[j][k] = n64Point[k][i][j];
							nEdge[j][k] = nPoint[k][i][j];
						}
					if (RecDualThinEdge(pNodeEdge, n64Edge, nEdge, 1))
						bRtn = true;
				}
				break;
			case 2:
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
							Leaf(pNode[i][j], n64Position[i][j], nLevel[i][j], 1-i, 1-j, k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
				if (RecDualThinPoint(pNodePoint, n64Point, nPoint))
					bRtn = true;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[j][k][i];
							n64Edge[j][k] = n64Point[j][k][i];
							nEdge[j][k] = nPoint[j][k][i];
						}
					if (RecDualThinEdge(pNodeEdge, n64Edge, nEdge, 2))
						bRtn = true;
				}
				break;
			}
		}
	} else {
		int nI, nJ, nIndex;
		GetStorageEdge(nLevel, dirEdge, nI, nJ, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI][nJ];
		UINT64 nSimpleTag = GET_EDGE_SIMPLE(pTempNode->m_n64SimpleTag, nIndex);

		if (nSimpleTag != 0) {
			UCHAR * pNodeFace[2];
			int nFace[2];
			int dirFace;
			int nFaceI, nFaceIndex;
			UCHAR * pNodeUniqueFace;
			int nUniqueFaceIndex;

			if (m_bCountOnly) {
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++) {
						pNodeFace[j] = pNode[i][j];
						nFace[j] = nLevel[i][j];
					}
					dirFace = (dirEdge + 2) % 3;
					if (GET_LAST_FIX(pNodeFace[0]) && GET_LAST_FIX(pNodeFace[1])) {
						if (CheckStorageFace(pNodeFace)) {
							GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
							nSimpleTag = GET_FACE_SIMPLE(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64SimpleTag, nFaceIndex);
							if (nSimpleTag != 3) {
								SetObjectSimple(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64SimpleTag, nFaceIndex, 2, nSimpleTag + 1);
							}
						}
					}
					for (int j = 0; j < 2; j++) {
						pNodeFace[j] = pNode[j][i];
						nFace[j] = nLevel[j][i];
					}
					dirFace = (dirEdge + 1) % 3;
					if (GET_LAST_FIX(pNodeFace[0]) && GET_LAST_FIX(pNodeFace[1])) {
						if (CheckStorageFace(pNodeFace)) {
							GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
							nSimpleTag = GET_FACE_SIMPLE(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64SimpleTag, nFaceIndex);
							if (nSimpleTag != 3) {
								SetObjectSimple(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64SimpleTag, nFaceIndex, 2, nSimpleTag + 1);
							}
						}
					}
				}
			} else {
				float fWeight = CConstant::FLOAT_INFINITE;
				int nEdgeRI = -1;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++) {
						pNodeFace[j] = pNode[i][j];
						nFace[j] = nLevel[i][j];
					}
					dirFace = (dirEdge + 2) % 3;
					if (CheckStorageFace(pNodeFace)) {
						GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
						if (GET_FACE_SIMPLE(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64SimpleTag, nFaceIndex) == 2) {
							RETRACE_EDGE * pRetraceEdge = GetRetraceEdge(pNodeFace[nFaceI], nFaceIndex);
							float fTempWeight = pRetraceEdge->m_fWeight + GetFaceArea(nFace[nFaceI]);
							if (fTempWeight < fWeight) {
								pNodeUniqueFace = pNodeFace[nFaceI];
								nUniqueFaceIndex = nFaceIndex;
								nEdgeRI = i * 2 + 0;
								fWeight = fTempWeight;
							}
							//break;
						}
					}
					for (int j = 0; j < 2; j++) {
						pNodeFace[j] = pNode[j][i];
						nFace[j] = nLevel[j][i];
					}
					dirFace = (dirEdge + 1) % 3;
					if (CheckStorageFace(pNodeFace)) {
						GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
						if (GET_FACE_SIMPLE(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64SimpleTag, nFaceIndex) == 2) {
							RETRACE_EDGE * pRetraceEdge = GetRetraceEdge(pNodeFace[nFaceI], nFaceIndex);
							float fTempWeight = pRetraceEdge->m_fWeight + GetFaceArea(nFace[nFaceI]);
							if (fTempWeight < fWeight) {
								pNodeUniqueFace = pNodeFace[nFaceI];
								nUniqueFaceIndex = nFaceIndex;
								nEdgeRI = i * 2 + 1;
								fWeight = fTempWeight;
							}
							//break;
						}
					}
				}
				if (nEdgeRI != -1) {
					SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 1, 0);
					SetObjectSimple(((LEAF_NODE *)pNodeUniqueFace)->m_n64SimpleTag, nUniqueFaceIndex, 2, 0);
					RETRACE_EDGE * pRetraceEdge = GetRetraceEdge(pNodeUniqueFace, nUniqueFaceIndex);
					GetEdgePos(n64Position, nLevel, dirEdge, nI, nJ, 0, pRetraceEdge->m_n64PositionEdge);
					SetEdgeRI(pTempNode->m_n64Type, nIndex, nEdgeRI);
					for (int i = 0; i < 2; i++)
					{
						for (int j = 0; j < 2; j++) {
							pNodeFace[j] = pNode[i][j];
							nFace[j] = nLevel[i][j];
						}
						dirFace = (dirEdge + 2) % 3;
						if (CheckStorageFace(pNodeFace)) {
							GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
							if (nEdgeRI != i * 2 + 0) {
								RETRACE_EDGE * pRetraceEdge = GetRetraceEdge(pNodeFace[nFaceI], nFaceIndex);
								pRetraceEdge->m_fWeight += fWeight;
							}
						}
						for (int j = 0; j < 2; j++) {
							pNodeFace[j] = pNode[j][i];
							nFace[j] = nLevel[j][i];
						}
						dirFace = (dirEdge + 1) % 3;
						if (CheckStorageFace(pNodeFace)) {
							GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
							if (nEdgeRI != i * 2 + 1) {
								RETRACE_EDGE * pRetraceEdge = GetRetraceEdge(pNodeFace[nFaceI], nFaceIndex);
								pRetraceEdge->m_fWeight += fWeight;
							}
						}
					}
					bRtn = true;
				} else {
					SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 1, 1);
				}
			}
		}
	}

	if (bRtn)
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				SetLastTime(pNode[i][j], bRtn);

	return bRtn;
}

bool COcTree::RecDualThinPoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2])
{
	bool bRtn = false;

	if ((*(pNode[0][0][0]) & 0x3) == 0 || (*(pNode[0][0][1]) & 0x3) == 0 || (*(pNode[0][1][0]) & 0x3) == 0 || (*(pNode[0][1][1]) & 0x3) == 0 || 
		(*(pNode[1][0][0]) & 0x3) == 0 || (*(pNode[1][0][1]) & 0x3) == 0 || (*(pNode[1][1][0]) & 0x3) == 0 || (*(pNode[1][1][1]) & 0x3) == 0) {
		if (GET_LAST_FIX(pNode[0][0][0]) || GET_LAST_FIX(pNode[0][0][1]) || GET_LAST_FIX(pNode[0][1][0]) || GET_LAST_FIX(pNode[0][1][1]) || 
			GET_LAST_FIX(pNode[1][0][0]) || GET_LAST_FIX(pNode[1][0][1]) || GET_LAST_FIX(pNode[1][1][0]) || GET_LAST_FIX(pNode[1][1][1])) {
			UCHAR * pNodePoint[2][2][2];
			UINT64 n64Point[2][2][2];
			int nPoint[2][2][2];
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[i][j][k], n64Position[i][j][k], nLevel[i][j][k], 1-i, 1-j, 1-k, pNodePoint[i][j][k], n64Point[i][j][k], nPoint[i][j][k]);
			if (RecDualThinPoint(pNodePoint, n64Point, nPoint))
				bRtn = true;
		}
	} else {
		int nI, nJ, nK, nIndex;
		GetStoragePoint(nLevel, nI, nJ, nK, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI][nJ][nK];
		UINT64 nSimpleTag = GET_POINT_SIMPLE(pTempNode->m_n64SimpleTag, nIndex);
		UCHAR * pNodeEdge[2][2];
		int nEdge[2][2];
		int dirEdge;
		int nEdgeI, nEdgeJ, nEdgeIndex;
		UCHAR * pNodeUniqueEdge;
		int nUniqueEdgeIndex;

		if (nSimpleTag != 0) {
			if (m_bCountOnly) {
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNode[i][j][k];
							nEdge[j][k] = nLevel[i][j][k];
						}
					if (GET_LAST_FIX(pNodeEdge[0][0]) && GET_LAST_FIX(pNodeEdge[0][1]) && GET_LAST_FIX(pNodeEdge[1][0]) && GET_LAST_FIX(pNodeEdge[1][1])) {
						if (CheckStorageEdge(pNodeEdge)) {
							dirEdge = 0;
							GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
							nSimpleTag = GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex);
							if (nSimpleTag != 3) {
								SetObjectSimple(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex, 1, nSimpleTag + 1);
							}
						}
					}
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNode[k][i][j];
							nEdge[j][k] = nLevel[k][i][j];
						}
					if (GET_LAST_FIX(pNodeEdge[0][0]) && GET_LAST_FIX(pNodeEdge[0][1]) && GET_LAST_FIX(pNodeEdge[1][0]) && GET_LAST_FIX(pNodeEdge[1][1])) {
						if (CheckStorageEdge(pNodeEdge)) {
							dirEdge = 1;
							GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
							nSimpleTag = GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex);
							if (nSimpleTag != 3) {
								SetObjectSimple(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex, 1, nSimpleTag + 1);
							}
						}
					}
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNode[j][k][i];
							nEdge[j][k] = nLevel[j][k][i];
						}
					if (GET_LAST_FIX(pNodeEdge[0][0]) && GET_LAST_FIX(pNodeEdge[0][1]) && GET_LAST_FIX(pNodeEdge[1][0]) && GET_LAST_FIX(pNodeEdge[1][1])) {
						if (CheckStorageEdge(pNodeEdge)) {
							dirEdge = 2;
							GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
							nSimpleTag = GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex);
							if (nSimpleTag != 3) {
								SetObjectSimple(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex, 1, nSimpleTag + 1);
							}
						}
					}
				}
			} else {
				int i = 0;
				for (; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNode[i][j][k];
							nEdge[j][k] = nLevel[i][j][k];
						}
					if (CheckStorageEdge(pNodeEdge)) {
						dirEdge = 0;
						GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
						if (GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex) == 2) {
							pNodeUniqueEdge = pNodeEdge[nEdgeI][nEdgeJ];
							nUniqueEdgeIndex = nEdgeIndex;
							break;
						}
					}
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNode[k][i][j];
							nEdge[j][k] = nLevel[k][i][j];
						}
					if (CheckStorageEdge(pNodeEdge)) {
						dirEdge = 1;
						GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
						if (GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex) == 2) {
							pNodeUniqueEdge = pNodeEdge[nEdgeI][nEdgeJ];
							nUniqueEdgeIndex = nEdgeIndex;
							break;
						}
					}
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNode[j][k][i];
							nEdge[j][k] = nLevel[j][k][i];
						}
					if (CheckStorageEdge(pNodeEdge)) {
						dirEdge = 2;
						GetStorageEdge(nEdge, dirEdge, nEdgeI, nEdgeJ, nEdgeIndex);
						if (GET_EDGE_SIMPLE(((LEAF_NODE *)pNodeEdge[nEdgeI][nEdgeJ])->m_n64SimpleTag, nEdgeIndex) == 2) {
							pNodeUniqueEdge = pNodeEdge[nEdgeI][nEdgeJ];
							nUniqueEdgeIndex = nEdgeIndex;
							break;
						}
					}
				}
				if (i < 2) {
					SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 0, 0);
					SetObjectSimple(((LEAF_NODE *)pNodeUniqueEdge)->m_n64SimpleTag, nUniqueEdgeIndex, 1, 0);
					bRtn = true;
				} else {
					SetObjectSimple(pTempNode->m_n64SimpleTag, nIndex, 0, 1);
				}
			}
		}
	}

	if (bRtn)
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					SetLastTime(pNode[i][j][k], bRtn);

	return bRtn;
}

//////////////////////////////////////////////////////////////////////////
// Dual Create Minimun Spanning Tree Code
//////////////////////////////////////////////////////////////////////////
void COcTree::DualCreateMST(float fThreshold, CString strSKTFile)
{
	// Initialization
	printf("\tInitialization...");
	m_listGrowSeedFace.clear();
	const int MAX_HEAP_SIZE = 50000;
	CFaceHeap heapFace(MAX_HEAP_SIZE);
	m_nType = 8;
	Traversal();
	int dirFace;
	int nI, nIndex;
	UCHAR * pNodeFace[2];
	UINT64 n64Face[2];
	int nFace[2];
	int nCellIndex;

	for (int i = 0; i < 2; i++)
	{
		pNodeFace[i] = m_pRoot;
		n64Face[i] = 0;
		nFace[i] = 0;
		pNodeFace[1 - i] = (UCHAR *)(&m_cInfiniteNode);
		n64Face[1 - i] = 0;
		nFace[1 - i] = -1;
		for (int j = 0; j < 3; j++)
			RecPushFace(pNodeFace, n64Face, nFace, j, heapFace);
	}

	bool bOutput = (strSKTFile.GetLength() != 0);
	UCHAR nType;
	FILE * pFile;
	if (bOutput) {
		pFile = fopen(strSKTFile.GetBuffer(0), "ab");
	}

	printf("\n\tMain process of dual create MST...");
	DWORD dwStart = GetTickCount();

	while (!heapFace.Empty()) {
		CFaceHeapElement cElement = heapFace.Pop();
		//printf("\n\n# Pop element, value is : %10.8f", cElement.m_fValue);
		//PrintFaceInfo(cElement.m_pNode, cElement.m_n64Position, cElement.m_nLevel, cElement.m_dirFace);
		GetStorageFace(cElement.m_nLevel, cElement.m_dirFace, nI, nIndex);
		if (GetFaceNeighborCell(cElement.m_pNode, cElement.m_n64Position, cElement.m_nLevel, cElement.m_dirFace, nCellIndex) || cElement.m_fValue > fThreshold) {
			SetObjectSimple(((LEAF_NODE *)cElement.m_pNode[nI])->m_n64SimpleTag, nIndex, 2, 1);
			SetObjectSimple(((LEAF_NODE *)cElement.m_pNode[nCellIndex])->m_n64SimpleTag, 0, 3, 0);
			UINT64 n64BaseCell = cElement.m_n64Position[nCellIndex];
			int nBaseCell = cElement.m_nLevel[nCellIndex];
			for (int i = 0; i < 2; i++)
				for (dirFace = 0; dirFace < 3; dirFace++)
				{
					UINT64 n64Search = LeafPosition(n64BaseCell, nBaseCell, CConstant::I_SHIFT_FP[CConstant::II_SHIFT_CF[dirFace][1-i]][0]);
					GetFaceFromPos(n64Search, nBaseCell, dirFace, pNodeFace, n64Face, nFace);
					RecPushFace(pNodeFace, n64Face, nFace, dirFace, heapFace);
				}
			if (bOutput) {
				nType = 2;				// Non-filled skeleton
				UINT64 n64Pos;
				fwrite(&nType, 1, 1, pFile);
				fwrite(&cElement.m_fValue, sizeof(float), 1, pFile);
				for (int i = 0; i < 2; i++)
				{
					if (cElement.m_nLevel[i] == -1) {
						n64Pos = FacePosition(cElement.m_n64Position, cElement.m_nLevel, cElement.m_dirFace);
					} else {
						n64Pos = NodePosition(cElement.m_n64Position[nI], cElement.m_nLevel[nI]);
					}
					fwrite(&n64Pos, sizeof(UINT64), 1, pFile);
				}
			}
		} else {
			printf("\n\t\tA fill with weight : %8.6f", cElement.m_fValue);
			m_listGrowSeedFace.push_back(cElement);
			if (bOutput) {
				nType = 3;				// filled skeleton
				UINT64 n64Pos;
				fwrite(&nType, 1, 1, pFile);
				fwrite(&cElement.m_fValue, sizeof(float), 1, pFile);
				for (int i = 0; i < 2; i++)
				{
					if (cElement.m_nLevel[i] == -1) {
						n64Pos = FacePosition(cElement.m_n64Position, cElement.m_nLevel, cElement.m_dirFace);
					} else {
						n64Pos = NodePosition(cElement.m_n64Position[nI], cElement.m_nLevel[nI]);
					}
					fwrite(&n64Pos, sizeof(UINT64), 1, pFile);
				}
			}
		}
	}

	DWORD dwEnd = GetTickCount();
	printf("\n\tMain process finished.\n\tTime consuming %d.%d second(s).", (dwEnd - dwStart) / 1000, (dwEnd - dwStart) % 1000);
	printf("\n\tFinal heap size is %d.", heapFace.m_nSize);

	// Finallization
	printf("\n\tFinalization...\n\n");
	if (bOutput) {
		nType = 0xff;
		fwrite(&nType, 1, 1, pFile);
		fclose(pFile);
	}
}

void COcTree::RecPushFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, CFaceHeap & heapFace)
{
	UCHAR * pNodeFace[2];
	UINT64 n64Face[2];
	int nFace[2];
	int nI, nIndex;

	if (NOT_LEAF(pNode[0]) || NOT_LEAF(pNode[1])) {
		switch (dirFace) {
		case 0:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k], n64Position[k], nLevel[k], 1 - k, i, j, pNodeFace[k], n64Face[k], nFace[k]);
					RecPushFace(pNodeFace, n64Face, nFace, dirFace, heapFace);
				}
			break;
		case 1:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k], n64Position[k], nLevel[k], j, 1 - k, i, pNodeFace[k], n64Face[k], nFace[k]);
					RecPushFace(pNodeFace, n64Face, nFace, dirFace, heapFace);
				}
			break;
		case 2:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k], n64Position[k], nLevel[k], i, j, 1 - k, pNodeFace[k], n64Face[k], nFace[k]);
					RecPushFace(pNodeFace, n64Face, nFace, dirFace, heapFace);
				}
			break;
		}
	} else {
		if (CheckStorageFace(pNode)) {
			GetStorageFace(nLevel, dirFace, nI, nIndex);
			if (GET_FACE_SIMPLE(((LEAF_NODE *)pNode[nI])->m_n64SimpleTag, nIndex) == 2) {
				SetObjectSimple(((LEAF_NODE *)pNode[nI])->m_n64SimpleTag, nIndex, 2, 3);
				heapFace.Push(pNode, n64Position, nLevel, dirFace, GetFaceWeight(pNode, n64Position, nLevel, dirFace));
				//printf("\n\n# Push element, value is : %10.8f", GetFaceWeight(pNode, n64Position, nLevel, dirFace));
				//PrintFaceInfo(pNode, n64Position, nLevel, dirFace);
			}
		}
	}
}

void COcTree::DualGrow()
{
	DWORD dwStart = GetTickCount();

	list<CFaceHeapElement>::iterator it;
	for (it = m_listGrowSeedFace.begin(); it != m_listGrowSeedFace.end(); it++)
	{
		DualGrowByFace(*it);
	}

	DWORD dwEnd = GetTickCount();
	printf("\tGrow dual cutting plane finished.\n\tTime consuming %d.%d second(s).\n\n", (dwEnd - dwStart) / 1000, (dwEnd - dwStart) % 1000);
}

void COcTree::DualGrowByFace(CFaceHeapElement & cElement)
{
	UCHAR * pNodeEdge[2][2];
	UINT64 n64Edge[2][2];
	int nEdge[2][2];
	int dirEdge;
	int nI, nIndex;
	UINT64 n64Face;

	GetStorageFace(cElement.m_nLevel, cElement.m_dirFace, nI, nIndex);
	//printf("\n\n# Go for a face :");
	//PrintFaceInfo(cElement.m_pNode, cElement.m_n64Position, cElement.m_nLevel, cElement.m_dirFace);

	GetFacePos(cElement.m_n64Position, cElement.m_nLevel, cElement.m_dirFace, nI, 0, n64Face);
	dirEdge = (cElement.m_dirFace + 2) % 3;
	GetEdgeFromPos(n64Face, cElement.m_nLevel[nI], dirEdge, pNodeEdge, n64Edge, nEdge);
	RecDualGrowByEdge(pNodeEdge, n64Edge, nEdge, dirEdge);

	GetFacePos(cElement.m_n64Position, cElement.m_nLevel, cElement.m_dirFace, nI, 1, n64Face);
	dirEdge = (cElement.m_dirFace + 1) % 3;
	GetEdgeFromPos(n64Face, cElement.m_nLevel[nI], dirEdge, pNodeEdge, n64Edge, nEdge);
	RecDualGrowByEdge(pNodeEdge, n64Edge, nEdge, dirEdge);

	GetFacePos(cElement.m_n64Position, cElement.m_nLevel, cElement.m_dirFace, nI, 2, n64Face);
	dirEdge = (cElement.m_dirFace + 2) % 3;
	GetEdgeFromPos(n64Face, cElement.m_nLevel[nI], dirEdge, pNodeEdge, n64Edge, nEdge);
	RecDualGrowByEdge(pNodeEdge, n64Edge, nEdge, dirEdge);

	GetFacePos(cElement.m_n64Position, cElement.m_nLevel, cElement.m_dirFace, nI, 0, n64Face);
	dirEdge = (cElement.m_dirFace + 1) % 3;
	GetEdgeFromPos(n64Face, cElement.m_nLevel[nI], dirEdge, pNodeEdge, n64Edge, nEdge);
	RecDualGrowByEdge(pNodeEdge, n64Edge, nEdge, dirEdge);
}

void COcTree::RecDualGrowByEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge)
{
	UCHAR * pNodeEdge[2][2];
	UINT64 n64Edge[2][2];
	int nEdge[2][2];

	//printf("\n\n# Go for a edge :");
	//PrintEdgeInfo(pNode, n64Position, nLevel, dirEdge);

	if (NOT_LEAF(pNode[0][0]) || NOT_LEAF(pNode[0][1]) || NOT_LEAF(pNode[1][0]) || NOT_LEAF(pNode[1][1])) {
		//printf("\nOperation : split.");
		switch(dirEdge) {
		case 0:
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], i, 1-j, 1-k, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				RecDualGrowByEdge(pNodeEdge, n64Edge, nEdge, dirEdge);
			}
			break;
		case 1:
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], 1-k, i, 1-j, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				RecDualGrowByEdge(pNodeEdge, n64Edge, nEdge, dirEdge);
			}
			break;
		case 2:
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], 1-j, 1-k, i, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				RecDualGrowByEdge(pNodeEdge, n64Edge, nEdge, dirEdge);
			}
			break;
		}
	} else {
		//printf("\nOperation : check retrace faces.");
		UCHAR * pNodePoint[2][2][2];
		UINT64 n64Point[2][2][2];
		int nPoint[2][2][2];
		int nPointI, nPointJ, nPointK, nPointIndex;
		UCHAR * pNodeFace[2];
		UINT64 n64Face[2];
		int nFace[2];
		int dirFace;
		int nFaceI, nFaceIndex;
		UINT64 n64EdgePosition;

		int nI, nJ, nIndex;
		GetStorageEdge(nLevel, dirEdge, nI, nJ, nIndex);
		if (GET_EDGE_SIMPLE(((LEAF_NODE *)pNode[nI][nJ])->m_n64SimpleTag, nIndex) != 3 && !GET_EDGE_INSIDE(((LEAF_NODE *)pNode[nI][nJ])->m_n64Type, nIndex)) {
			SetObjectSimple(((LEAF_NODE *)pNode[nI][nJ])->m_n64SimpleTag, nIndex, 1, 3);
			UINT64 n64Search;
			for (int i = 0; i < 2; i++)
			{
				GetEdgePos(n64Position, nLevel, dirEdge, nI, nJ, i, n64Search);
				GetPointFromPos(n64Search, MAX_LEVEL, pNodePoint, n64Point, nPoint);
				GetStoragePoint(nPoint, nPointI, nPointJ, nPointK, nPointIndex);
				if (!GET_POINT_INSIDE(((LEAF_NODE *)pNodePoint[nPointI][nPointJ][nPointK])->m_n64Type, nPointIndex)) {
					SetObjectSimple(((LEAF_NODE *)pNodePoint[nPointI][nPointJ][nPointK])->m_n64SimpleTag, nPointIndex, 0, 3);
				}
			}
			int nEdgeRI = (int)GET_EDGE_RI(((LEAF_NODE *)pNode[nI][nJ])->m_n64Type, nIndex);
			int nTempI = nEdgeRI / 2;
			int nTempK = nEdgeRI % 2;
			if (nTempK == 0) {
				for (int j = 0; j < 2; j++) {
					pNodeFace[j] = pNode[nTempI][j];
					n64Face[j] = n64Position[nTempI][j];
					nFace[j] = nLevel[nTempI][j];
				}
				dirFace = (dirEdge + 2) % 3;
			} else {
				for (int j = 0; j < 2; j++) {
					pNodeFace[j] = pNode[j][nTempI];
					n64Face[j] = n64Position[j][nTempI];
					nFace[j] = nLevel[j][nTempI];
				}
				dirFace = (dirEdge + 1) % 3;
			}
			if (CheckStorageFace(pNodeFace)) {
				GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
				GetEdgePos(n64Position, nLevel, dirEdge, nI, nJ, 0, n64EdgePosition);
				RETRACE_EDGE * pRetraceEdge = GetRetraceEdge(pNodeFace[nFaceI], nFaceIndex);
				if (pRetraceEdge->m_n64PositionEdge == n64EdgePosition) {
					//printf("\nnEdgeRI : %d, pass!", nEdgeRI);
					if (GET_FACE_SIMPLE(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64SimpleTag, nFaceIndex) != 3) {
						SetObjectSimple(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64SimpleTag, nFaceIndex, 2, 3);
						DualGrowByFace(CFaceHeapElement(pNodeFace, n64Face, nFace, dirFace, 0.0f));
					}
				} else {
					//printf("\nnEdgeRI : %d, fail!", nEdgeRI);
				}
			}
		}
	}
}
