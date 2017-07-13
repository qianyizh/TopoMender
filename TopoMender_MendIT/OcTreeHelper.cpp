#include "OcTree.h"

//////////////////////////////////////////////////////////////////////////
// helper functions
//////////////////////////////////////////////////////////////////////////

// common helpers
bool OcTree::ParseSOGFile(FILE * pFile, CVector3D & vCorner, double & dbSize)
{
	char pHeadBuf[128];
	if (pFile == NULL || fread(pHeadBuf, 1, 128, pFile) != 128) 
		return false;
	const char * strHead = "SOG.Format 1.0";
	if (strncmp(pHeadBuf, strHead, strlen(strHead)) == 0) {			// 1.0 SOG File Format
		float fCoord[4];
		memcpy(fCoord, pHeadBuf + 15, sizeof(float) * 4);
		vCorner.pVec[0] = fCoord[0];
		vCorner.pVec[1] = fCoord[1];
		vCorner.pVec[2] = fCoord[2];
		dbSize = fCoord[3];
		return true;
	} else {
		return false;
	}
}

// storage helpers
void OcTree::SetStorages()
{
	m_nType = 0;
	TraversalWithoutPosition();
}

void OcTree::GetStorageFace(int nLevel[2], int dirFace, int & nI, int & nIndex) 
{
	int nMaxLevel = -1;
	for (int i = 0; i < 2; i++)
		if (nLevel[i] > nMaxLevel) {
			nI = i;
			nMaxLevel = nLevel[i];
		}
		nIndex = dirFace * 2 + (1 - nI);
}

void OcTree::GetStorageEdge(int nLevel[2][2], int dirEdge, int & nI, int & nJ, int & nIndex)
{
	int nMaxLevel = -1;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			if (nLevel[i][j] > nMaxLevel) {
				nI = i;
				nJ = j;
				nMaxLevel = nLevel[i][j];
			}
			nIndex = dirEdge * 4 + (1 - nI) * 2 + (1 - nJ);
}

void OcTree::GetStoragePoint(int nlevel[2][2][2], int & nI, int & nJ, int & nK, int & nIndex)
{
	int nMaxLevel = -1;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
				if (nlevel[i][j][k] > nMaxLevel) {
					nI = i;
					nJ = j;
					nK = k;
					nMaxLevel = nlevel[i][j][k];
				}
				nIndex = (1 - nI) * 4 + (1 - nJ) * 2 + (1 - nK);
}

int OcTree::CountElement( UCHAR * pNode, int nEnd )
{
	LeafNode * pTempNode = (LeafNode *)pNode;
	int nCount = 0;
	for ( int i = 0; i < nEnd; i++ )
		if ( GET_STORAGE_TAG( pTempNode->m_nType, i ) )
			nCount ++;
	return nCount;
}

Element * OcTree::GetElementCell(UCHAR * pNode) 
{
	return (Element *)(pNode + sizeof(LeafNode));
}

Element * OcTree::GetElementFace(UCHAR * pNode[2], int nLevel[2], int dirFace, int & nI, int & nIndex)
{
	GetStorageFace( nLevel, dirFace, nI, nIndex );
	int nCount = CountElement( pNode[nI], nIndex + 1 );
#ifdef BETTER_GENERATING_SET
	return (Element *)(pNode[nI] + sizeof(LeafNode) + nCount * (sizeof(Element) + sizeof(UINT64)));
#else
	return (Element *)(pNode[nI] + sizeof(LeafNode) + nCount * sizeof(Element));
#endif
}

Element * OcTree::GetElementEdge(UCHAR * pNode[2][2], int nLevel[2][2], int dirEdge, int & nI, int & nJ, int & nIndex)
{
	GetStorageEdge( nLevel, dirEdge, nI, nJ, nIndex );
	int nCount = CountElement( pNode[nI][nJ], nIndex + 1 + 6 );
#ifdef BETTER_GENERATING_SET
	return (Element *)(pNode[nI][nJ] + sizeof(LeafNode) + nCount * (sizeof(Element) + sizeof(UINT64)));
#else
	return (Element *)(pNode[nI][nJ] + sizeof(LeafNode) + nCount * sizeof(Element));
#endif
}

Element * OcTree::GetElementPoint(UCHAR * pNode[2][2][2], int nLevel[2][2][2], int & nI, int & nJ, int & nK, int & nIndex) 
{
	GetStoragePoint( nLevel, nI, nJ, nK, nIndex );
	int nCount = CountElement( pNode[nI][nJ][nK], nIndex + 1 + 6 + 12 );
#ifdef BETTER_GENERATING_SET
	return (Element *)(pNode[nI][nJ][nK] + sizeof(LeafNode) + nCount * (sizeof(Element) + sizeof(UINT64)));
#else
	return (Element *)(pNode[nI][nJ][nK] + sizeof(LeafNode) + nCount * sizeof(Element));
#endif
}

Element * OcTree::GetElementFace(UCHAR * pNode[2], int nLevel[2], int dirFace)
{
	int nI, nIndex;
	GetStorageFace( nLevel, dirFace, nI, nIndex );
	int nCount = CountElement( pNode[nI], nIndex + 1 );
#ifdef BETTER_GENERATING_SET
	return (Element *)(pNode[nI] + sizeof(LeafNode) + nCount * (sizeof(Element) + sizeof(UINT64)));
#else
	return (Element *)(pNode[nI] + sizeof(LeafNode) + nCount * sizeof(Element));
#endif
}

Element * OcTree::GetElementEdge(UCHAR * pNode[2][2], int nLevel[2][2], int dirEdge)
{
	int nI, nJ, nIndex;
	GetStorageEdge( nLevel, dirEdge, nI, nJ, nIndex );
	int nCount = CountElement( pNode[nI][nJ], nIndex + 1 + 6 );
#ifdef BETTER_GENERATING_SET
	return (Element *)(pNode[nI][nJ] + sizeof(LeafNode) + nCount * (sizeof(Element) + sizeof(UINT64)));
#else
	return (Element *)(pNode[nI][nJ] + sizeof(LeafNode) + nCount * sizeof(Element));
#endif
}

Element * OcTree::GetElementPoint(UCHAR * pNode[2][2][2], int nLevel[2][2][2])
{
	int nI, nJ, nK, nIndex;
	GetStoragePoint( nLevel, nI, nJ, nK, nIndex );
	int nCount = CountElement( pNode[nI][nJ][nK], nIndex + 1 + 6 + 12 );
#ifdef BETTER_GENERATING_SET
	return (Element *)(pNode[nI][nJ][nK] + sizeof(LeafNode) + nCount * (sizeof(Element) + sizeof(UINT64)));
#else
	return (Element *)(pNode[nI][nJ][nK] + sizeof(LeafNode) + nCount * sizeof(Element));
#endif
}

bool OcTree::CheckStorageFace(UCHAR * pNode[2])
{
	if (pNode[0] == pNode[1])
		return false;
	else 
		return true;
}

bool OcTree::CheckStorageEdge(UCHAR * pNode[2][2])
{
	if ((pNode[0][0] == pNode[0][1] && pNode[1][0] == pNode[1][1]) ||
		(pNode[0][0] == pNode[1][0] && pNode[0][1] == pNode[1][1]))
		return false;
	else
		return true;
}

// seek & search helpers
bool OcTree::InNode(UCHAR * pNode, UINT64 n64Position, int nLevel, UINT64 n64Search)
{
	return (
		(INT64GETI(n64Search) >> (MAX_LEVEL - nLevel)) == (INT64GETI(n64Position) >> (MAX_LEVEL - nLevel)) &&
		(INT64GETJ(n64Search) >> (MAX_LEVEL - nLevel)) == (INT64GETJ(n64Position) >> (MAX_LEVEL - nLevel)) &&
		(INT64GETK(n64Search) >> (MAX_LEVEL - nLevel)) == (INT64GETK(n64Position) >> (MAX_LEVEL - nLevel))
		);
}

void OcTree::Search(UCHAR * pNode, UINT64 n64Position, int nLevel, UINT64 n64Search, int nSearch, UCHAR * & pRtnNode, UINT64 & n64RtnPosition, int & nRtnLevel)
{
	if (!InNode(pNode, n64Position, nLevel, n64Search)) {
		pRtnNode = m_pInfiniteNode;
		n64RtnPosition = 0;
		nRtnLevel = -1;
	} else if (nSearch == nLevel || !NOT_LEAF(pNode)) {
		pRtnNode = pNode;
		n64RtnPosition = n64Position;
		nRtnLevel = nLevel;
	} else {
		UCHAR * pLeafNode;
		UINT64 n64Leaf;
		int nLeaf;
		Leaf(pNode, n64Position, nLevel, (int)(INT64GETI(n64Search)>>(MAX_LEVEL-nLevel-1))&1, (int)(INT64GETJ(n64Search)>>(MAX_LEVEL-nLevel-1))&1, (int)(INT64GETK(n64Search)>>(MAX_LEVEL-nLevel-1))&1, pLeafNode, n64Leaf, nLeaf);
		Search(pLeafNode, n64Leaf, nLeaf, n64Search, nSearch, pRtnNode, n64RtnPosition, nRtnLevel);
	}
}

void OcTree::GetCellFromPosition(UINT64 n64Search, int nSearch, UCHAR * & pRtnNode, UINT64 & n64RtnPosition, int & nRtnLevel) 
{
	Search(m_pRoot, 0, 0, INT64TRANS(INT64GETI(n64Search)+1, INT64GETJ(n64Search)+1, INT64GETK(n64Search)+1), nSearch, pRtnNode, n64RtnPosition, nRtnLevel);
}

void OcTree::GetFaceFromPosition(UINT64 n64Search, int nSearch, int dirFace, UCHAR * pRtnNode[2], UINT64 n64RtnPosition[2], int nRtnLevel[2])
{
	for (int i = 0; i < 2; i++)
	{
		switch (dirFace) {
			case 0:
				Search(m_pRoot, 0, 0, INT64TRANS(INT64GETI(n64Search)+i*2-1, INT64GETJ(n64Search)+1, INT64GETK(n64Search)+1), nSearch, pRtnNode[i], n64RtnPosition[i], nRtnLevel[i]);
				break;
			case 1:
				Search(m_pRoot, 0, 0, INT64TRANS(INT64GETI(n64Search)+1, INT64GETJ(n64Search)+i*2-1, INT64GETK(n64Search)+1), nSearch, pRtnNode[i], n64RtnPosition[i], nRtnLevel[i]);
				break;
			case 2:
				Search(m_pRoot, 0, 0, INT64TRANS(INT64GETI(n64Search)+1, INT64GETJ(n64Search)+1, INT64GETK(n64Search)+i*2-1), nSearch, pRtnNode[i], n64RtnPosition[i], nRtnLevel[i]);
				break;
		}
	}
}

void OcTree::GetEdgeFromPosition(UINT64 n64Search, int nSearch, int dirEdge, UCHAR * pRtnNode[2][2], UINT64 n64RtnPosition[2][2], int nRtnLevel[2][2]) 
{
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
		{
			switch(dirEdge) {
				case 0:
					Search(m_pRoot, 0, 0, INT64TRANS(INT64GETI(n64Search)+1, INT64GETJ(n64Search)+i*2-1, INT64GETK(n64Search)+j*2-1), nSearch, pRtnNode[i][j], n64RtnPosition[i][j], nRtnLevel[i][j]);
					break;
				case 1:
					Search(m_pRoot, 0, 0, INT64TRANS(INT64GETI(n64Search)+j*2-1, INT64GETJ(n64Search)+1, INT64GETK(n64Search)+i*2-1), nSearch, pRtnNode[i][j], n64RtnPosition[i][j], nRtnLevel[i][j]);
					break;
				case 2:
					Search(m_pRoot, 0, 0, INT64TRANS(INT64GETI(n64Search)+i*2-1, INT64GETJ(n64Search)+j*2-1, INT64GETK(n64Search)+1), nSearch, pRtnNode[i][j], n64RtnPosition[i][j], nRtnLevel[i][j]);
					break;
			}
		}
}

void OcTree::GetPointFromPosition(UINT64 n64Search, int nSearch, UCHAR * pRtnNode[2][2][2], UINT64 n64RtnPosition[2][2][2], int nRtnLevel[2][2][2])
{
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
			{
				Search(m_pRoot, 0, 0, INT64TRANS(INT64GETI(n64Search)+i*2-1, INT64GETJ(n64Search)+j*2-1, INT64GETK(n64Search)+k*2-1), nSearch, pRtnNode[i][j][k], n64RtnPosition[i][j][k], nRtnLevel[i][j][k]);
			}
}

UINT64 OcTree::LeafPosition(UINT64 n64Position, int nLevel, int i, int j, int k)
{
	return INT64TRANS(i*NLENGTH(nLevel)+INT64GETI(n64Position), j*NLENGTH(nLevel)+INT64GETJ(n64Position), k*NLENGTH(nLevel)+INT64GETK(n64Position));
}

UINT64 OcTree::LeafPosition(UINT64 n64Position, int nLevel, int iIndex)
{
	int i = (iIndex >> 2) & 1;
	int j = (iIndex >> 1) & 1;
	int k = (iIndex) & 1;
	return INT64TRANS(i*NLENGTH(nLevel)+INT64GETI(n64Position), j*NLENGTH(nLevel)+INT64GETJ(n64Position), k*NLENGTH(nLevel)+INT64GETK(n64Position));
}

void OcTree::GetFacePosition(UINT64 n64Position[2], int nLevel[2], int dirFace, int nI, int iIndex, UINT64 & n64RtnPosition)
{
	n64RtnPosition = LeafPosition(n64Position[nI], nLevel[nI], Constant::I_SHIFT_FP[Constant::II_SHIFT_CF[dirFace][nI]][iIndex]);
}

void OcTree::GetEdgePosition(UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, int nI, int nJ, int iIndex, UINT64 & n64RtnPosition)
{
	n64RtnPosition = LeafPosition(n64Position[nI][nJ], nLevel[nI][nJ], Constant::I_SHIFT_EP[Constant::II_SHIFT_CE[dirEdge][nI][nJ]][iIndex]);
}

void OcTree::GetPointPosition(UINT64 n64Position[2][2][2], int nLevel[2][2][2], int nI, int nJ, int nK, UINT64 & n64RtnPosition)
{
	n64RtnPosition = LeafPosition(n64Position[nI][nJ][nK], nLevel[nI][nJ][nK], Constant::II_SHIFT_CP[nI][nJ][nK]);
}

void OcTree::GetFacePosition(UINT64 n64Position[2], int nLevel[2], int dirFace, UINT64 & n64Face, int & nFace)
{
	int nI, nIndex;
	GetStorageFace(nLevel, dirFace, nI, nIndex);
	nFace = nLevel[nI];
	n64Face = LeafPosition(n64Position[nI], nLevel[nI], Constant::I_SHIFT_FP[Constant::II_SHIFT_CF[dirFace][nI]][0]);
}

void OcTree::GetEdgePosition(UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, UINT64 & n64Edge, int & nEdge)
{
	int nI, nJ, nIndex;
	GetStorageEdge(nLevel, dirEdge, nI, nJ, nIndex);
	nEdge = nLevel[nI][nJ];
	n64Edge = LeafPosition(n64Position[nI][nJ], nLevel[nI][nJ], Constant::I_SHIFT_EP[Constant::II_SHIFT_CE[dirEdge][nI][nJ]][0]);
}

void OcTree::GetPointPosition(UINT64 n64Position[2][2][2], int nLevel[2][2][2], UINT64 & n64Point)
{
	int nI, nJ, nK, nIndex;
	GetStoragePoint(nLevel, nI, nJ, nK, nIndex);
	n64Point = LeafPosition(n64Position[nI][nJ][nK], nLevel[nI][nJ][nK], Constant::II_SHIFT_CP[nI][nJ][nK]);
}

UINT64 OcTree::GetCellCenterPosition(UINT64 n64Position, int nLevel) 
{
	UINT64 nHalfLength = NLENGTH(nLevel + 1);
	return (n64Position + INT64TRANS(nHalfLength, nHalfLength, nHalfLength));
}

UINT64 OcTree::GetFaceCenterPosition(UINT64 n64Position[2], int nLevel[2], int dirFace)
{
	int nI, nIndex;
	GetStorageFace(nLevel, dirFace, nI, nIndex);
	int nFace = nLevel[nI];
	UINT64 n64Face = LeafPosition(n64Position[nI], nLevel[nI], Constant::I_SHIFT_FP[Constant::II_SHIFT_CF[dirFace][nI]][0]);
	UINT64 nHalfLength = NLENGTH(nLevel[nI] + 1);
	switch (dirFace) {
		case 0:
			return (n64Face + INT64TRANS(0, nHalfLength, nHalfLength));
			break;
		case 1:
			return (n64Face + INT64TRANS(nHalfLength, 0, nHalfLength));
			break;
		case 2:
			return (n64Face + INT64TRANS(nHalfLength, nHalfLength, 0));
			break;
	}
	return 0;
}

UINT64 OcTree::GetEdgeCenterPosition(UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge)
{
	int nI, nJ, nIndex;
	GetStorageEdge(nLevel, dirEdge, nI, nJ, nIndex);
	int nEdge = nLevel[nI][nJ];
	UINT64 n64Edge = LeafPosition(n64Position[nI][nJ], nLevel[nI][nJ], Constant::I_SHIFT_EP[Constant::II_SHIFT_CE[dirEdge][nI][nJ]][0]);
	UINT64 nHalfLength = NLENGTH(nLevel[nI][nJ] + 1);
	switch (dirEdge) {
		case 0:
			return (n64Edge + INT64TRANS(nHalfLength, 0, 0));
			break;
		case 1:
			return (n64Edge + INT64TRANS(0, nHalfLength, 0));
			break;
		case 2:
			return (n64Edge + INT64TRANS(0, 0, nHalfLength));
			break;
	}
	return 0;
}

UINT64 OcTree::GetPointCenterPosition(UINT64 n64Position[2][2][2], int nLevel[2][2][2]) 
{
	int nI, nJ, nK, nIndex;
	GetStoragePoint(nLevel, nI, nJ, nK, nIndex);
	return LeafPosition(n64Position[nI][nJ][nK], nLevel[nI][nJ][nK], Constant::II_SHIFT_CP[nI][nJ][nK]);
}

UINT64 OcTree::NodePosition(UINT64 n64Position, int nLevel) 
{
	return n64Position + INT64TRANS(NLENGTH(nLevel + 1), NLENGTH(nLevel + 1), NLENGTH(nLevel + 1));
}

void OcTree::Leaf(UCHAR * pNode, UINT64 n64Position, int nLevel, int i, int j, int k, UCHAR * & pNext, UINT64 & nNextPos, int & nNextLevel)
{
	if ( NOT_LEAF(pNode) ) {
		pNext = ((InterNode *)(pNode))->m_pNode[i*4+j*2+k];
		nNextPos = n64Position + INT64TRANS(i * NLENGTH(nLevel + 1), j * NLENGTH(nLevel + 1), k * NLENGTH(nLevel + 1));
		nNextLevel = nLevel + 1;
	} else {
		pNext = pNode;
		nNextPos = n64Position;
		nNextLevel = nLevel;
	}
}

void OcTree::Leaf(UCHAR * pNode, int nLevel, int i, int j, int k, UCHAR * & pNext, int & nNextLevel) 
{
	if ( NOT_LEAF(pNode) ) {
		pNext = ((InterNode *)(pNode))->m_pNode[i*4+j*2+k];
		nNextLevel = nLevel + 1;
	} else {
		pNext = pNode;
		nNextLevel = nLevel;
	}
}

UCHAR * OcTree::GetLeafNode(UCHAR * pNode, int i, int j, int k)
{
	return ((InterNode *)(pNode))->m_pNode[i*4+j*2+k];
}

void OcTree::InsertIndex(list<int> & listIndex, int iInsert)
{
	for (list<int>::iterator it = listIndex.begin(); it != listIndex.end(); it++)
		if (iInsert == (*it))
			return;
	listIndex.push_back(iInsert);
}

UINT OcTree::GetIndex(UCHAR * pNode, UINT64 n64Position, UINT64 n64Point)
{
	if (IS_CELL_IRREGULAR(((LeafNode *)pNode)->m_nVertex))
		return m_pHashMap->Get(n64Position, n64Point);
	else
		return ((LeafNode *)pNode)->m_nVertex;
}

UINT64 OcTree::GetPointVolume() 
{
	return THIN_ELEMENT_WIDTH * THIN_ELEMENT_WIDTH * THIN_ELEMENT_WIDTH;
}

UINT64 OcTree::GetEdgeVolume(int nLevel) 
{
	return THIN_ELEMENT_WIDTH * THIN_ELEMENT_WIDTH * THICK_ELEMENT_WIDTH(nLevel);
}

UINT64 OcTree::GetFaceVolume(int nLevel)
{
	return THIN_ELEMENT_WIDTH * THICK_ELEMENT_WIDTH(nLevel) * THICK_ELEMENT_WIDTH(nLevel);
}

UINT64 OcTree::GetCellVolume(int nLevel)
{
	if (nLevel == -1)
		return INFINITE_ELEMENT_VOLUME;
	else
		return THICK_ELEMENT_WIDTH(nLevel) * THICK_ELEMENT_WIDTH(nLevel) * THICK_ELEMENT_WIDTH(nLevel);
}

UINT64 OcTree::GetObjectSkeletonElementSize(SkeletonElement * pSkeletonElement) 
{
	if (pSkeletonElement->m_pDualElement == NULL)
		return pSkeletonElement->m_n64Size + 0x00ffffffffffffff;
	else if (pSkeletonElement->m_n64Size < pSkeletonElement->m_pDualElement->m_n64Size)
		return pSkeletonElement->m_n64Size;
	else
		return pSkeletonElement->m_pDualElement->m_n64Size;
}

void OcTree::UpdateObjectSkeletonElementSize(SkeletonElement * pSkeletonElement, SkeletonElement * pAddElement) 
{
	if (GetObjectSkeletonElementSize(pSkeletonElement) > pAddElement->m_n64Size) {
		pSkeletonElement->m_pDualElement = pAddElement;
	}
}

// debug helpers
void OcTree::PrintNodeInfo(UCHAR * pNode)
{
	printf("\nLeafNode : (%8x, %8x)", ((LeafNode *)pNode)->m_nType, ((LeafNode *)pNode)->m_nVertex);
}

UINT64 * OcTree::GetFootPoint(Element * pElement)
{
	UCHAR * pTemp = (UCHAR *)pElement;
	pTemp += sizeof(Element);
	return ((UINT64 *)pTemp);
}

UINT64 OcTree::Distance(UINT64 * pA, UINT64 * pB) 
{
	UINT64 x, y, z, _x, _y, _z, i, j, k;
	INT64GETIJK(*pA, x, y, z);
	INT64GETIJK(*pB, _x, _y, _z);
	if (x > _x)
		i = x - _x;
	else
		i = _x - x;
	if (y > _y)
		j = y - _y;
	else
		j = _y - y;
	if (z > _z)
		k = z - _z;
	else
		k = _z - z;
	return (i * i + j * j + k * k);
}
