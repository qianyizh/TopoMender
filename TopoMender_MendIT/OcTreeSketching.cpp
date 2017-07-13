#include "OcTree.h"

void OcTree::Sketching(Skeleton * pSkeleton)
{
	m_pSkeleton = pSkeleton;
	for (int i = 0; i < (int)pSkeleton->m_vecLine.size(); i++)
	{
		RecSketchingSplit(&m_pRoot, 0, 0, i);
	}
#ifdef SKETCHING_MERGE
	RecSketchingMerge(m_pRoot, 0, 0);
#endif
#ifdef CONSOLE_OUTPUT
	printf("Scan convert skeletons finished.\n");
#endif
}

void OcTree::RecSketchingSplit(UCHAR ** ppNode, UINT64 n64Position, int nLevel, int nLine)
{
	if (Intersection(n64Position, nLevel, nLine)) {
		UCHAR * pCell;
		UINT64 n64Cell;
		int nCell;
		switch ((**ppNode) & 3) {
		case 0:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						Leaf(*ppNode, n64Position, nLevel, i, j, k, pCell, n64Cell, nCell);
						RecSketchingSplit(&((InterNode *)(*ppNode))->m_pNode[i*4+j*2+k], n64Cell, nCell, nLine);
					}
					break;
		case 1:
			if (nLevel != m_nMaxLevel) {
				UCHAR * pOldNode = *ppNode;
				(*ppNode) = m_cMemoryManager.New( sizeof(InterNode) );
				((InterNode *)(*ppNode))->m_cType = 0;
				((NodeHeader *)(*ppNode))->m_bChanged = 1;
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							((InterNode *)(*ppNode))->m_pNode[i*4+j*2+k] = m_cMemoryManager.New( sizeof(LeafNode) );
							((LeafNode *)((InterNode *)(*ppNode))->m_pNode[i*4+j*2+k])->m_nType = ((LeafNode *)(pOldNode))->m_nType;
							((LeafNode *)((InterNode *)(*ppNode))->m_pNode[i*4+j*2+k])->m_nVertex = ((LeafNode *)(pOldNode))->m_nVertex;
							Leaf(*ppNode, n64Position, nLevel, i, j, k, pCell, n64Cell, nCell);
							RecSketchingSplit(&((InterNode *)(*ppNode))->m_pNode[i*4+j*2+k], n64Cell, nCell, nLine);
						}
			} else {
				((NodeHeader *)(*ppNode))->m_bLastFix = 1;				// sketch = 1
			}
			break;
		case 2:
			((NodeHeader *)(*ppNode))->m_bLastFix = 1;
			UCHAR nNext = (UCHAR)((LeafNode *)(*ppNode))->m_nVertex;
			UCHAR bObjectPoint[8];
			UINT64 n64ObjectPoint[8];
			for (int i = 0; i < 8; i++) {
				n64ObjectPoint[i] = LeafPosition(n64Position, nLevel, i);
				bObjectPoint[i] = GET_POINT_INSIDE(nNext, i);
			}
			bool bGoodCell = (m_cConstant.m_pRegularTableEx[nNext][2] == 1);
			for (int i = 0; i < 6; i++) {
				if (!bObjectPoint[Constant::I_SHIFT_FP[i][0]] &&
					!bObjectPoint[Constant::I_SHIFT_FP[i][1]] &&
					!bObjectPoint[Constant::I_SHIFT_FP[i][2]] &&
					!bObjectPoint[Constant::I_SHIFT_FP[i][3]])
				{
					UINT64 n64Face = n64ObjectPoint[Constant::I_SHIFT_FP[i][0]];
					if (Intersection(n64Face, nLevel, i / 2, nLine)) {
						bGoodCell = false;
					}
				}
			}
			if (bGoodCell) {
				((NodeHeader *)(*ppNode))->m_bLastTime = 1;
			}
			break;
		}
	}
}

void OcTree::RecSketchingCheck(UCHAR * pNode, UINT64 n64Position, int nLevel)
{
	if (NOT_LEAF(pNode)) {
		UCHAR * pCell;
		UINT64 n64Cell;
		int nCell;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					Leaf(pNode, n64Position, nLevel, i, j, k, pCell, n64Cell, nCell);
					RecSketchingCheck(pCell, n64Cell, nCell);
				}
	} else {
		NodeHeader * pTemp = (NodeHeader *)pNode;
		if (pTemp->m_bLastTime) {
#ifdef SIMPLE_SKETCHING
			m_nType = 32;
#else
			m_nType = 35;
#endif
			CheckCell(n64Position, nLevel);
		} else if (pTemp->m_bLastFix) {
			m_nType = 32;
			CheckCell(n64Position, nLevel);
		}
		pTemp->m_bLastFix = 0;
		pTemp->m_bLastTime = 0;
	}
}

bool OcTree::RecSketchingMerge(UCHAR * pNode, UINT64 n64Position, int nLevel)
{
	UCHAR * pCell;
	//UCHAR * pFace[2];
	//UCHAR * pEdge[2][2];
	//UCHAR * pPoint[2][2][2];
	UINT64 n64Cell;
	//UINT64 n64Face[2];
	//UINT64 n64Edge[2][2];
	//UINT64 n64Point[2][2][2];
	int nCell;
	//int nFace[2];
	//int nEdge[2][2];
	//int nPoint[2][2][2];
	//int dirFace, dirEdge;
	if (NOT_LEAF(pNode)) {
		bool bTest = true;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					Leaf(pNode, n64Position, nLevel, i, j, k, pCell, n64Cell, nCell);
					if ( !RecSketchingMerge( pCell, n64Cell, nCell ) )
						bTest = false;
				}
				if (bTest == false)
					return false;
				if (((NodeHeader *)pNode)->m_bChanged) {
					m_nType = 36;
					m_nCountCell = 0;
					m_nCountFace = 0;
					m_nCountEdge = 0;
					m_nCountPoint = 0;
					CheckCell(n64Position, nLevel);
					if ( m_nCountCell - m_nCountFace + m_nCountEdge - m_nCountPoint == 0 ) {
						// congratulations! you have passed all the tests!
						((LeafNode *)pNode)->m_nType = 1;
						((LeafNode *)pNode)->m_nVertex = 0;
						((NodeHeader *)pNode)->m_bLastFix = 1;
						return true;
					}
				}
	} else {
		if ( ((*pNode) & 0x3) == 2 || ((LeafNode *)pNode)->m_nVertex != 0 )
			return false;
		else
			return true;
	}
	return false;
}

bool OcTree::RecSketchingMergeCheckEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge)
{
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
						Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], i, 1-j, 1-k, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				if ( !RecSketchingMergeCheckEdge(pNodeEdge, n64Edge, nEdge, dirEdge) )
					return false;
			}
			break;
		case 1:
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], 1-k, i, 1-j, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				if ( !RecSketchingMergeCheckEdge(pNodeEdge, n64Edge, nEdge, dirEdge) )
					return false;
			}
			break;
		case 2:
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], 1-j, 1-k, i, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				if ( !RecSketchingMergeCheckEdge(pNodeEdge, n64Edge, nEdge, dirEdge) )
					return false;
			}
			break;
		}
		return true;
	} else {
		return (((NodeHeader *)pNode[0][0])->m_bLastFix == 0 && ((NodeHeader *)pNode[0][1])->m_bLastFix == 0 &&
			((NodeHeader *)pNode[1][0])->m_bLastFix == 0 && ((NodeHeader *)pNode[1][1])->m_bLastFix == 0);
	}
}

bool OcTree::RecSketchingMergeCheckFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace)
{
	UCHAR * pNodeFace[2];
	UINT64 n64Face[2];
	int nFace[2];

	if (NOT_LEAF(pNode[0]) || NOT_LEAF(pNode[1])) {
		switch (dirFace) {
		case 0:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k], n64Position[k], nLevel[k], 1-k, i, j, pNodeFace[k], n64Face[k], nFace[k]);
					if ( !RecSketchingMergeCheckFace(pNodeFace, n64Face, nFace, dirFace) )
						return false;
				}
				break;
		case 1:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k], n64Position[k], nLevel[k], j, 1-k, i, pNodeFace[k], n64Face[k], nFace[k]);
					if ( !RecSketchingMergeCheckFace(pNodeFace, n64Face, nFace, dirFace) )
						return false;
				}
				break;
		case 2:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k], n64Position[k], nLevel[k], i, j, 1-k, pNodeFace[k], n64Face[k], nFace[k]);
					if ( !RecSketchingMergeCheckFace(pNodeFace, n64Face, nFace, dirFace) )
						return false;
				}
				break;
		}
		return true;
	} else {
		return (((NodeHeader *)pNode[0])->m_bLastFix == 0 && ((NodeHeader *)pNode[1])->m_bLastFix == 0);
	}
}

void OcTree::RecSketching(UCHAR * pNode, UINT64 n64Position, int nLevel, int nLine)
{
	if (Intersection(n64Position, nLevel, nLine)) {
		if (NOT_LEAF(pNode)) {
			UCHAR * pCell;
			UINT64 n64Cell;
			int nCell;
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						Leaf(pNode, n64Position, nLevel, i, j, k, pCell, n64Cell, nCell);
						RecSketching(pCell, n64Cell, nCell, nLine);
					}
		} else {
#ifdef SIMPLE_SKETCHING
			m_nType = 32;
			CheckCell(n64Position, nLevel);
#else
			if (!m_bBorderCheck && ((*pNode) & 0x3) == 1) {
				m_nType = 32;
				CheckCell(n64Position, nLevel);
			} else if (m_bBorderCheck && ((*pNode) & 0x3) == 2) {
				UCHAR bObjectPoint[8];
				UINT64 n64ObjectPoint[8];
				UCHAR * pPoint[2][2][2];
				UINT64 n64Point[2][2][2];
				int nPoint[2][2][2];
				for (int i = 0; i < 8; i++) {
					n64ObjectPoint[i] = LeafPosition(n64Position, nLevel, i);
					GetPointFromPosition(n64ObjectPoint[i], m_nMaxLevel, pPoint, n64Point, nPoint);
					Element * pTemp = GetElementPoint(pPoint, nPoint);
					bObjectPoint[i] = pTemp->m_bObject;
				}
				bool bGoodCell = true;
				for (int i = 0; i < 6; i++) {
					if (!bObjectPoint[Constant::I_SHIFT_FP[i][0]] &&
						!bObjectPoint[Constant::I_SHIFT_FP[i][1]] &&
						!bObjectPoint[Constant::I_SHIFT_FP[i][2]] &&
						!bObjectPoint[Constant::I_SHIFT_FP[i][3]])
					{
						UINT64 n64Face = n64ObjectPoint[Constant::I_SHIFT_FP[i][0]];
						if (Intersection(n64Face, nLevel, i / 2, nLine)) {
							bGoodCell = false;
						}
					}
				}
				if (bGoodCell) {
					m_nType = 35;
				} else {
					m_nType = 32;
				}
				CheckCell(n64Position, nLevel);
			}
#endif
		}
	}
}

bool OcTree::Intersection(UINT64 n64Position, int nLevel, int nLine)
{
	CVector3D vV[2] = { 
		m_pSkeleton->m_vecPoint[m_pSkeleton->m_vecLine[nLine].m_iPoint[0]].m_vPosition,
		m_pSkeleton->m_vecPoint[m_pSkeleton->m_vecLine[nLine].m_iPoint[1]].m_vPosition
	};
	CVector3D vCell[8];
	CVector3D vecTest[7];
	double dbMin1, dbMax1, dbMin2, dbMax2;
	double dbX[8];
	vecTest[0] = vV[1] - vV[0];
	vecTest[1] = CVector3D(0, 0, 1);
	vecTest[2] = CVector3D(0, 1, 0);
	vecTest[3] = CVector3D(1, 0, 0);

	for (int i = 0; i < 3; i++) {
		vecTest[i + 4] = vecTest[0]	^ vecTest[i + 1];
	}
	for (int i = 0; i < 8; i++) {
		UINT64 n64Temp = LeafPosition(n64Position, nLevel, i);
		vCell[i] = m_vCorner + CVector3D(
			m_dbNodeLength * (INT64GETI(n64Temp)),
			m_dbNodeLength * (INT64GETJ(n64Temp)),
			m_dbNodeLength * (INT64GETK(n64Temp))
			);
	}

	for (int i = 0; i < 7; i++)
	{
		dbMin1 = 1e300;
		dbMax1 = -1e300;
		for (int j = 0; j < 8; j++)
		{
			dbX[j] = vecTest[i] * vCell[j];
			if (dbX[j] > dbMax1) {
				dbMax1 = dbX[j];
			}
			if (dbX[j] < dbMin1) {
				dbMin1 = dbX[j];
			}
		}
		dbMin2 = 1e300;
		dbMax2 = -1e300;
		for (int j = 0; j < 2; j++)
		{
			dbX[j] = vecTest[i] * vV[j];
			if (dbX[j] > dbMax2) {
				dbMax2 = dbX[j];
			}
			if (dbX[j] < dbMin2) {
				dbMin2 = dbX[j];
			}
		}
		if (dbMin1 > dbMax2 || dbMin2 > dbMax1) {
			return false;
		}
	}
	return true;
}

bool OcTree::Intersection(UINT64 n64Position, int nLevel, int dirFace, int nLine)
{
	CVector3D vV[2] = { 
		m_pSkeleton->m_vecPoint[m_pSkeleton->m_vecLine[nLine].m_iPoint[0]].m_vPosition,
		m_pSkeleton->m_vecPoint[m_pSkeleton->m_vecLine[nLine].m_iPoint[1]].m_vPosition
	};
	CVector3D vFace[4];
	CVector3D vecTest[5];
	double dbMin1, dbMax1, dbMin2, dbMax2;
	double dbX[4];
	vecTest[0] = vV[1] - vV[0];
	switch (dirFace) {
	case 0:
		vecTest[1] = CVector3D(0, 0, 1);
		vecTest[2] = CVector3D(0, 1, 0);
		break;
	case 1:
		vecTest[1] = CVector3D(0, 0, 1);
		vecTest[2] = CVector3D(1, 0, 0);
		break;
	case 2:
		vecTest[1] = CVector3D(1, 0, 0);
		vecTest[2] = CVector3D(0, 1, 0);
		break;
	}

	for (int i = 0; i < 2; i++) {
		vecTest[i + 3] = vecTest[0]	^ vecTest[i + 1];
	}
	for (int i = 0; i < 4; i++) {
		UINT64 n64Temp = LeafPosition(n64Position, nLevel, Constant::I_SHIFT_FP[dirFace * 2][i]);
		vFace[i] = m_vCorner + CVector3D(
			m_dbNodeLength * (INT64GETI(n64Temp)),
			m_dbNodeLength * (INT64GETJ(n64Temp)),
			m_dbNodeLength * (INT64GETK(n64Temp))
			);
	}

	for (int i = 0; i < 5; i++)
	{
		dbMin1 = 1e300;
		dbMax1 = -1e300;
		for (int j = 0; j < 4; j++)
		{
			dbX[j] = vecTest[i] * vFace[j];
			if (dbX[j] > dbMax1) {
				dbMax1 = dbX[j];
			}
			if (dbX[j] < dbMin1) {
				dbMin1 = dbX[j];
			}
		}
		dbMin2 = 1e300;
		dbMax2 = -1e300;
		for (int j = 0; j < 2; j++)
		{
			dbX[j] = vecTest[i] * vV[j];
			if (dbX[j] > dbMax2) {
				dbMax2 = dbX[j];
			}
			if (dbX[j] < dbMin2) {
				dbMin2 = dbX[j];
			}
		}
		if (dbMin1 > dbMax2 || dbMin2 > dbMax1) {
			return false;
		}
	}
	return true;
}
