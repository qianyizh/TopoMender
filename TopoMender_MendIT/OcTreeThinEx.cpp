#include "OcTree.h"

//////////////////////////////////////////////////////////////////////////
// thinning extension functions
//////////////////////////////////////////////////////////////////////////
bool OcTree::RecThinCell(UCHAR * pNode, UINT64 n64Position, int nLevel)
{
	((NodeHeader *)pNode)->m_bLastFix = ((NodeHeader *)pNode)->m_bLastTime;

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
		//Element * pThisCell = GetElementCell(pNode);
		//if (pThisCell->m_bObject && pThisCell->m_bSkeleton && !pThisCell->m_bSketch) {
		//	if (pThisCell->m_cSimpleTag == m_nTestLastOut) {
		//		pThisCell->m_bSkeleton = 0;
		//		pThisCell->m_cSimpleTag = 0;
		//	}
		//}
	}

	if (!m_bCountOnly) {
		((NodeHeader *)pNode)->m_bLastTime = bRtn;
	}

	return bRtn;
}

bool OcTree::RecThinFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace)
{
	if (!GET_LAST_FIX(pNode[0]) && !GET_LAST_FIX(pNode[1]))
		return false;

	bool bRtn = false;

	if (NOT_LEAF(pNode[0]) || NOT_LEAF(pNode[1])) {
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
		Element * pThisFace = GetElementFace(pNode, nLevel, dirFace);
		if (pThisFace->m_bObject && pThisFace->m_bSkeleton && !pThisFace->m_bSketch) {
			int nSimpleTest = 0;
			Element * pUniqueCell;
			UINT64 n64WitnessElement;
			int nCell2Face;
			int nFace2Cell;
			for (int i = 0; i < 2; i++) {
				Element * pTempCell = GetElementCell(pNode[i]);
				if (pTempCell->m_bObject && pTempCell->m_bSkeleton) {
					nSimpleTest ++;
					if (nSimpleTest == 1) {
						pUniqueCell = pTempCell;
						nCell2Face = dirFace * 2 + (1 - i);
						nFace2Cell = i;
						n64WitnessElement = GetCellCenterPosition(n64Position[i], nLevel[i]);
					} else {
						break;
					}
				}
			}
			if (nSimpleTest == 1) {
				if (m_bCountOnly) {
					pThisFace->m_cSimpleTag = 1;
					// set face
					m_n64SimpleElement = GetFaceCenterPosition(n64Position, nLevel, dirFace);
					m_pSimpleElement = pThisFace;
					m_nType = 53;
					CheckFace2(pNode, n64Position, nLevel, dirFace, pThisFace);
					UINT64 * pThis = GetFootPoint(pThisFace);
					if (*pThis == INT64_INVALID) {
						*pThis = m_n64SimpleElement;
					}
					// now set the cell
					UINT64 * pThat = GetFootPoint(pUniqueCell);
					if (*pThat == INT64_INVALID) {
						*pThat = *pThis;
					} else {
						if (Distance(pThis, &n64WitnessElement) < Distance(pThat, &n64WitnessElement)) {
							*pThat = *pThis;
						}
					}
				} else {
					if (*GetFootPoint(pThisFace) != INT64_INVALID && 
						*GetFootPoint(pUniqueCell) == *GetFootPoint(pThisFace) && 
						Distance(GetFootPoint(pUniqueCell), &n64WitnessElement) < m_n64Threshold) 
					{
						//printf("Face : (%16I64x, %d)\n", GetFaceCenterPosition(n64Position, nLevel, dirFace), dirFace);
						//printf("Cell (witness) : (%16I64x)\n", n64WitnessElement);
						pThisFace->m_cSimpleTag = 0;
						pThisFace->m_bSkeleton = 0;
						pThisFace->m_cPointer = nFace2Cell;
						pUniqueCell->m_cSimpleTag = 0;
						pUniqueCell->m_bSkeleton = 0;
						pUniqueCell->m_cPointer = nCell2Face;
					}
				}
				bRtn = true;
			}
			if (!m_bCountOnly && pThisFace->m_cSimpleTag == 1) {
				*GetFootPoint(pThisFace) = INT64_INVALID;
				pThisFace->m_cSimpleTag = 0;
			}
		}
	}

	if (!m_bCountOnly) {
		if (bRtn)
			for (int i = 0; i < 2; i++)
				((NodeHeader *)pNode[i])->m_bLastTime = bRtn;
	}

	return bRtn;
}

bool OcTree::RecThinEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge)
{
	if (!GET_LAST_FIX(pNode[0][0]) && !GET_LAST_FIX(pNode[0][1]) && !GET_LAST_FIX(pNode[1][0]) && !GET_LAST_FIX(pNode[1][1]))
		return false;

	bool bRtn = false;

	if (NOT_LEAF(pNode[0][0]) || NOT_LEAF(pNode[0][1]) || NOT_LEAF(pNode[1][0]) || NOT_LEAF(pNode[1][1])) {
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
		Element * pThisEdge = GetElementEdge(pNode, nLevel, dirEdge);
		if (pThisEdge->m_bObject && pThisEdge->m_bSkeleton && !pThisEdge->m_bSketch) {
			int nSimpleTest = 0;
			UCHAR * pFace[2];
			UINT64 n64Face[2];
			int nFace[2];
			int dirFace;
			Element * pUniqueFace;
			UINT64 n64WitnessElement;
			int nFace2Edge;
			int nEdge2Face;
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++) {
					pFace[j] = pNode[i][j];
					n64Face[j] = n64Position[i][j];
					nFace[j] = nLevel[i][j];
				}
				dirFace = (dirEdge + 2) % 3;
				if (CheckStorageFace(pFace)) {
					Element * pTempFace = GetElementFace(pFace, nFace, dirFace);
					if (pTempFace->m_bObject && pTempFace->m_bSkeleton) {
						nSimpleTest ++;
						if (nSimpleTest == 1) {
							pUniqueFace = pTempFace;
							nFace2Edge = (0 * 2) + (1 - i) + 2;
							nEdge2Face = (0 * 2) + i;
							n64WitnessElement = GetFaceCenterPosition(n64Face, nFace, dirFace);
						} else {
							break;
						}
					}
				}
				for (int j = 0; j < 2; j++) {
					pFace[j] = pNode[j][i];
					n64Face[j] = n64Position[j][i];
					nFace[j] = nLevel[j][i];
				}
				dirFace = (dirEdge + 1) % 3;
				if (CheckStorageFace(pFace)) {
					Element * pTempFace = GetElementFace(pFace, nFace, dirFace);
					if (pTempFace->m_bObject && pTempFace->m_bSkeleton) {
						nSimpleTest ++;
						if (nSimpleTest == 1) {
							pUniqueFace = pTempFace;
							nFace2Edge = (1 * 2) + (1 - i) + 2;
							nEdge2Face = (1 * 2) + i;
							n64WitnessElement = GetFaceCenterPosition(n64Face, nFace, dirFace);
						} else {
							break;
						}
					}
				}
			}
			if (nSimpleTest == 1) {
				if (m_bCountOnly) {
					pThisEdge->m_cSimpleTag = 1;
					// set edge
					m_n64SimpleElement = GetEdgeCenterPosition(n64Position, nLevel, dirEdge);
					m_pSimpleElement = pThisEdge;
					m_nType = 53;
					CheckEdge2(pNode, n64Position, nLevel, dirEdge, pThisEdge);
					UINT64 * pThis = GetFootPoint(pThisEdge);
					if (*pThis == INT64_INVALID) {
						*pThis = m_n64SimpleElement;
					}
					// now set the cell
					UINT64 * pThat = GetFootPoint(pUniqueFace);
					if (*pThat == INT64_INVALID) {
						*pThat = *pThis;
					} else {
						if (Distance(pThis, &n64WitnessElement) < Distance(pThat, &n64WitnessElement)) {
							*pThat = *pThis;
						}
					}
				} else {
					if (*GetFootPoint(pThisEdge) != INT64_INVALID && 
						*GetFootPoint(pUniqueFace) == *GetFootPoint(pThisEdge) &&
						Distance(GetFootPoint(pUniqueFace), &n64WitnessElement) < m_n64Threshold) 
					{
						//printf("Edge : (%16I64x, %d)\n", GetEdgeCenterPosition(n64Position, nLevel, dirEdge), dirEdge);
						//printf("Face (witness) : (%16I64x, %d)\n", n64WitnessElement, dirFace);
						pThisEdge->m_cSimpleTag = 0;
						pThisEdge->m_bSkeleton = 0;
						pThisEdge->m_cPointer = nEdge2Face;
						pUniqueFace->m_cSimpleTag = 0;
						pUniqueFace->m_bSkeleton = 0;
						pUniqueFace->m_cPointer = nFace2Edge;
					}
				}
				bRtn = true;
			}
			if (!m_bCountOnly && pThisEdge->m_cSimpleTag == 1) {
				*GetFootPoint(pThisEdge) = INT64_INVALID;
				pThisEdge->m_cSimpleTag = 0;
			}
		}
	}

	if (!m_bCountOnly) {
		if (bRtn)
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					((NodeHeader *)pNode[i][j])->m_bLastTime = bRtn;
	}

	return bRtn;
}

bool OcTree::RecThinPoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2])
{
	if (!GET_LAST_FIX(pNode[0][0][0]) && !GET_LAST_FIX(pNode[0][0][1]) && !GET_LAST_FIX(pNode[0][1][0]) && !GET_LAST_FIX(pNode[0][1][1])
		&& !GET_LAST_FIX(pNode[1][0][0]) && !GET_LAST_FIX(pNode[1][0][1]) && !GET_LAST_FIX(pNode[1][1][0]) && !GET_LAST_FIX(pNode[1][1][1]))
		return false;

	bool bRtn = false;

	if (NOT_LEAF(pNode[0][0][0]) || NOT_LEAF(pNode[0][0][1]) || NOT_LEAF(pNode[0][1][0]) || NOT_LEAF(pNode[0][1][1]) ||
		NOT_LEAF(pNode[1][0][0]) || NOT_LEAF(pNode[1][0][1]) || NOT_LEAF(pNode[1][1][0]) || NOT_LEAF(pNode[1][1][1]) )
	{
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
		Element * pThisPoint = GetElementPoint(pNode, nLevel);
		if (pThisPoint->m_bObject && pThisPoint->m_bSkeleton && !pThisPoint->m_bSketch) {
			int nSimpleTest = 0;
			UCHAR * pEdge[2][2];
			UINT64 n64Edge[2][2];
			int nEdge[2][2];
			int dirEdge;
			Element * pUniqueEdge;
			UINT64 n64WitnessElement;
			int nEdge2Point;
			int nPoint2Edge;
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pEdge[j][k] = pNode[i][j][k];
						n64Edge[j][k] = n64Position[i][j][k];
						nEdge[j][k] = nLevel[i][j][k];
					}
					dirEdge = 0;
					if (CheckStorageEdge(pEdge)) {
						Element * pTempEdge = GetElementEdge(pEdge, nEdge, dirEdge);
						if (pTempEdge->m_bObject && pTempEdge->m_bSkeleton) {
							nSimpleTest ++;
							if (nSimpleTest == 1) {
								pUniqueEdge = pTempEdge;
								nEdge2Point = 1 - i + 4;
								nPoint2Edge = dirEdge * 2 + i;
								n64WitnessElement = GetEdgeCenterPosition(n64Edge, nEdge, dirEdge);
							} else {
								break;
							}
						}
					}
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pEdge[j][k] = pNode[k][i][j];
							n64Edge[j][k] = n64Position[k][i][j];
							nEdge[j][k] = nLevel[k][i][j];
						}
						dirEdge = 1;
						if (CheckStorageEdge(pEdge)) {
							Element * pTempEdge = GetElementEdge(pEdge, nEdge, dirEdge);
							if (pTempEdge->m_bObject && pTempEdge->m_bSkeleton) {
								nSimpleTest ++;
								if (nSimpleTest == 1) {
									pUniqueEdge = pTempEdge;
									nEdge2Point = 1 - i + 4;
									nPoint2Edge = dirEdge * 2 + i;
									n64WitnessElement = GetEdgeCenterPosition(n64Edge, nEdge, dirEdge);
								} else {
									break;
								}
							}
						}
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++)
							{
								pEdge[j][k] = pNode[j][k][i];
								n64Edge[j][k] = n64Position[j][k][i];
								nEdge[j][k] = nLevel[j][k][i];
							}
							dirEdge = 2;
							if (CheckStorageEdge(pEdge)) {
								Element * pTempEdge = GetElementEdge(pEdge, nEdge, dirEdge);
								if (pTempEdge->m_bObject && pTempEdge->m_bSkeleton) {
									nSimpleTest ++;
									if (nSimpleTest == 1) {
										pUniqueEdge = pTempEdge;
										nEdge2Point = 1 - i + 4;
										nPoint2Edge = dirEdge * 2 + i;
										n64WitnessElement = GetEdgeCenterPosition(n64Edge, nEdge, dirEdge);
									} else {
										break;
									}
								}
							}
			}
			if (nSimpleTest == 1) {
				if (m_bCountOnly) {
					pThisPoint->m_cSimpleTag = 1;
					// set point
					m_n64SimpleElement = GetPointCenterPosition(n64Position, nLevel);
					m_pSimpleElement = pThisPoint;
					m_nType = 53;
					CheckPoint2(pNode, n64Position, nLevel, pThisPoint);
					UINT64 * pThis = GetFootPoint(pThisPoint);
					if (*pThis == INT64_INVALID) {
						*pThis = m_n64SimpleElement;
					}
					// now set the cell
					UINT64 * pThat = GetFootPoint(pUniqueEdge);
					if (*pThat == INT64_INVALID) {
						*pThat = *pThis;
					} else {
						if (Distance(pThis, &n64WitnessElement) < Distance(pThat, &n64WitnessElement)) {
							*pThat = *pThis;
						}
					}
				} else {
					if (*GetFootPoint(pThisPoint) != INT64_INVALID && 
						*GetFootPoint(pUniqueEdge) == *GetFootPoint(pThisPoint) &&
						Distance(GetFootPoint(pUniqueEdge), &n64WitnessElement) < m_n64Threshold) 
					{
						//printf("Edge : (%16I64x)\n", GetPointCenterPosition(n64Position, nLevel));
						//printf("Edge (witness) : (%16I64x, %d)\n", n64WitnessElement, nPoint2Edge / 2);

						pThisPoint->m_cSimpleTag = 0;
						pThisPoint->m_bSkeleton = 0;
						pThisPoint->m_cPointer = nPoint2Edge;
						pUniqueEdge->m_cSimpleTag = 0;
						pUniqueEdge->m_bSkeleton = 0;
						pUniqueEdge->m_cPointer = nEdge2Point;
					} 
				}
				bRtn = true;
			}
			if (!m_bCountOnly && pThisPoint->m_cSimpleTag == 1) {
				*GetFootPoint(pThisPoint) = INT64_INVALID;
				pThisPoint->m_cSimpleTag = 0;
			}
		}
	}

	if (!m_bCountOnly) {
		if (bRtn)
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						((NodeHeader *)pNode[i][j][k])->m_bLastTime = bRtn;
	}

	if ( m_bCleanSkeleton )
		return bRtn;
	else
		return false;

#ifdef CLEAN_SKELETON
	return bRtn;
#else
	return false;
#endif
}
