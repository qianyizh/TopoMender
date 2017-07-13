#include "OcTree.h"
using namespace std;

//////////////////////////////////////////////////////////////////////////
// thinning functions
//////////////////////////////////////////////////////////////////////////
void OcTree::Thin()
{
	// Initialization
	m_nType = 16;
	RecNode(m_pRoot);

	// Thinning
#ifdef CONSOLE_OUTPUT
	printf("Thinning for object :");
	printf("\n\tMain thinning process start :");
	printf("\n\tStep count - %3d", 0);
#endif
	TimeMeter cTimer;
	cTimer.Start();
	m_nStep = 0;
	bool bContinue = true;
#ifdef BETTER_GENERATING_SET
	UINT64 n64Threshold = 0;
	UINT64 n64Step = NLENGTH(m_nMaxLevel + 1);
	m_n64Threshold = 0;
#endif

	while (bContinue) {
		if (m_nStep % 2 == 0) {
			m_nTestOut = 2;
			m_nTestLastOut = 3;
		} else {
			m_nTestOut = 3;
			m_nTestLastOut = 2;
		}
#ifdef CONSOLE_OUTPUT
		printf("\b\b\b%3d", m_nStep);
#endif

#ifdef BETTER_GENERATING_SET
		StepThreshold(n64Threshold);
		m_bCountOnly = true;
		RecThinCell(m_pRoot, 0, 0);
		m_bCountOnly = false;
		bContinue = RecThinCell(m_pRoot, 0, 0);
#else
		bContinue = RecThinCell(m_pRoot, 0);
#endif
		m_nStep++;

		//m_nType = 17;
		//RecNode(m_pRoot);
	}

	cTimer.End();
#ifdef CONSOLE_OUTPUT
	printf("\b\b\b%3d", m_nStep);
	printf("\n\tMain thinning process finished in %d step(s).", m_nStep);
	printf("\n\tTime consuming ");
	cTimer.Print();
#endif

	// Finallization
	// m_nType = 17;
	// RecNode(m_pRoot);
	printf("\n");
}

bool OcTree::RecThinCell(UCHAR * pNode, int nLevel)
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
					if (RecThinCell(pNodeCell, nCell))
						bRtn = true;
				}
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
					{
						for (int k = 0; k < 2; k++)
						{
							pNodeFace[k] = pNodePoint[k][i][j];
							nFace[k] = nPoint[k][i][j];
						}
						if (RecThinFace(pNodeFace, nFace, 0))
							bRtn = true;
						for (int k = 0; k < 2; k++)
						{
							pNodeFace[k] = pNodePoint[j][k][i];
							nFace[k] = nPoint[j][k][i];
						}
						if (RecThinFace(pNodeFace, nFace, 1))
							bRtn = true;
						for (int k = 0; k < 2; k++)
						{
							pNodeFace[k] = pNodePoint[i][j][k];
							nFace[k] = nPoint[i][j][k];
						}
						if (RecThinFace(pNodeFace, nFace, 2))
							bRtn = true;
					}
					for (int i = 0; i < 2; i++)
					{
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++)
							{
								pNodeEdge[j][k] = pNodePoint[i][j][k];
								nEdge[j][k] = nPoint[i][j][k];
							}
							if (RecThinEdge(pNodeEdge, nEdge, 0))
								bRtn = true;
							for (int j = 0; j < 2; j++)
								for (int k = 0; k < 2; k++)
								{
									pNodeEdge[j][k] = pNodePoint[k][i][j];
									nEdge[j][k] = nPoint[k][i][j];
								}
								if (RecThinEdge(pNodeEdge, nEdge, 1))
									bRtn = true;
								for (int j = 0; j < 2; j++)
									for (int k = 0; k < 2; k++)
									{
										pNodeEdge[j][k] = pNodePoint[j][k][i];
										nEdge[j][k] = nPoint[j][k][i];
									}
									if (RecThinEdge(pNodeEdge, nEdge, 2))
										bRtn = true;
					}
					if (RecThinPoint(pNodePoint, nPoint))
						bRtn = true;
	} else {
		Element * pThisCell = GetElementCell(pNode);
		if (pThisCell->m_bObject && pThisCell->m_bSkeleton && !pThisCell->m_bSketch) {
			if (pThisCell->m_cSimpleTag == m_nTestLastOut) {
				pThisCell->m_bSkeleton = 0;
				pThisCell->m_cSimpleTag = 0;
			}
		}
	}

	((NodeHeader *)pNode)->m_bLastTime = bRtn;

	return bRtn;
}

bool OcTree::RecThinFace(UCHAR * pNode[2], int nLevel[2], int dirFace)
{
	if (!GET_LAST_FIX(pNode[0]) && !GET_LAST_FIX(pNode[1]))
		return false;

	bool bRtn = false;

	if (NOT_LEAF(pNode[0]) || NOT_LEAF(pNode[1])) {
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
					for (int k = 0; k < 2; k++)
					{
						pNodeFace[k] = pNodePoint[k][i][j];
						nFace[k] = nPoint[k][i][j];
					}
					if (RecThinFace(pNodeFace, nFace, 0))
						bRtn = true;
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[k][i][j];
							nEdge[j][k] = nPoint[k][i][j];
						}
						if (RecThinEdge(pNodeEdge, nEdge, 1))
							bRtn = true;
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++)
							{
								pNodeEdge[j][k] = pNodePoint[j][k][i];
								nEdge[j][k] = nPoint[j][k][i];
							}
							if (RecThinEdge(pNodeEdge, nEdge, 2))
								bRtn = true;
				}
				if (RecThinPoint(pNodePoint, nPoint))
					bRtn = true;
				break;
		case 1:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j], nLevel[j], i, 1-j, k, pNodePoint[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
					{
						pNodeFace[k] = pNodePoint[j][k][i];
						nFace[k] = nPoint[j][k][i];
					}
					if (RecThinFace(pNodeFace, nFace, 1))
						bRtn = true;
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[j][k][i];
							nEdge[j][k] = nPoint[j][k][i];
						}
						if (RecThinEdge(pNodeEdge, nEdge, 2))
							bRtn = true;
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++)
							{
								pNodeEdge[j][k] = pNodePoint[i][j][k];
								nEdge[j][k] = nPoint[i][j][k];
							}
							if (RecThinEdge(pNodeEdge, nEdge, 0))
								bRtn = true;
				}
				if (RecThinPoint(pNodePoint, nPoint))
					bRtn = true;
				break;
		case 2:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k], nLevel[k], i, j, 1-k, pNodePoint[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
					{
						pNodeFace[k] = pNodePoint[i][j][k];
						nFace[k] = nPoint[i][j][k];
					}
					if (RecThinFace(pNodeFace, nFace, 2))
						bRtn = true;
				}
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[i][j][k];
							nEdge[j][k] = nPoint[i][j][k];
						}
						if (RecThinEdge(pNodeEdge, nEdge, 0))
							bRtn = true;
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++)
							{
								pNodeEdge[j][k] = pNodePoint[k][i][j];
								nEdge[j][k] = nPoint[k][i][j];
							}
							if (RecThinEdge(pNodeEdge, nEdge, 1))
								bRtn = true;
				}
				if (RecThinPoint(pNodePoint, nPoint))
					bRtn = true;
				break;
		}
	} else {
		Element * pThisFace = GetElementFace(pNode, nLevel, dirFace);
		if (pThisFace->m_bObject && pThisFace->m_bSkeleton && !pThisFace->m_bSketch) {
			if (pThisFace->m_cSimpleTag == m_nTestLastOut) {
				pThisFace->m_bSkeleton = 0;
				pThisFace->m_cSimpleTag = 0;
			} else {
				int nSimpleTest = 0;
				Element * pUniqueCell;
				int nCell2Face;
				int nFace2Cell;
				for (int i = 0; i < 2; i++) {
					Element * pTempCell = GetElementCell(pNode[i]);
					if (pTempCell->m_cSimpleTag == m_nTestOut) {
						nSimpleTest = 100;
						break;
					} else if (pTempCell->m_bObject && pTempCell->m_bSkeleton) {
						nSimpleTest ++;
						if (nSimpleTest == 1) {
							pUniqueCell = pTempCell;
							nCell2Face = dirFace * 2 + (1 - i);
							nFace2Cell = i;
						} else {
							break;
						}
					}
				}
				if (nSimpleTest == 1) {
					pThisFace->m_cSimpleTag = m_nTestOut;
					pThisFace->m_cPointer = nFace2Cell;
					pUniqueCell->m_cSimpleTag = m_nTestOut;
					pUniqueCell->m_cPointer = nCell2Face;
					bRtn = true;
				}
			}
		}
	}

	if (bRtn)
		for (int i = 0; i < 2; i++)
			((NodeHeader *)pNode[i])->m_bLastTime = bRtn;

	return bRtn;
}

bool OcTree::RecThinEdge(UCHAR * pNode[2][2], int nLevel[2][2], int dirEdge)
{
	if (!GET_LAST_FIX(pNode[0][0]) && !GET_LAST_FIX(pNode[0][1]) && !GET_LAST_FIX(pNode[1][0]) && !GET_LAST_FIX(pNode[1][1]))
		return false;

	bool bRtn = false;

	if (NOT_LEAF(pNode[0][0]) || NOT_LEAF(pNode[0][1]) || NOT_LEAF(pNode[1][0]) || NOT_LEAF(pNode[1][1])) {
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
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[i][j][k];
						nEdge[j][k] = nPoint[i][j][k];
					}
					if (RecThinEdge(pNodeEdge, nEdge, 0))
						bRtn = true;
			}
			if (RecThinPoint(pNodePoint, nPoint))
				bRtn = true;
			break;
		case 1:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k][i], nLevel[k][i], 1-i, j, 1-k, pNodePoint[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[k][i][j];
						nEdge[j][k] = nPoint[k][i][j];
					}
					if (RecThinEdge(pNodeEdge, nEdge, 1))
						bRtn = true;
			}
			if (RecThinPoint(pNodePoint, nPoint))
				bRtn = true;
			break;
		case 2:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[i][j], nLevel[i][j], 1-i, 1-j, k, pNodePoint[i][j][k], nPoint[i][j][k]);
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[j][k][i];
						nEdge[j][k] = nPoint[j][k][i];
					}
					if (RecThinEdge(pNodeEdge, nEdge, 2))
						bRtn = true;
			}
			if (RecThinPoint(pNodePoint, nPoint))
				bRtn = true;
			break;
		}
	} else {
		Element * pThisEdge = GetElementEdge(pNode, nLevel, dirEdge);
		if (pThisEdge->m_bObject && pThisEdge->m_bSkeleton && !pThisEdge->m_bSketch) {
			if (pThisEdge->m_cSimpleTag == m_nTestLastOut) {
				pThisEdge->m_bSkeleton = 0;
				pThisEdge->m_cSimpleTag = 0;
			} else {
				int nSimpleTest = 0;
				UCHAR * pFace[2];
				int nFace[2];
				int dirFace;
				Element * pUniqueFace;
				int nFace2Edge;
				int nEdge2Face;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++) {
						pFace[j] = pNode[i][j];
						nFace[j] = nLevel[i][j];
					}
					dirFace = (dirEdge + 2) % 3;
					if (CheckStorageFace(pFace)) {
						Element * pTempFace = GetElementFace(pFace, nFace, dirFace);
						if (pTempFace->m_cSimpleTag == m_nTestOut) {
							nSimpleTest = 100;
							break;
						} else if (pTempFace->m_bObject && pTempFace->m_bSkeleton) {
							nSimpleTest ++;
							if (nSimpleTest == 1) {
								pUniqueFace = pTempFace;
								nFace2Edge = (0 * 2) + (1 - i) + 2;
								nEdge2Face = (0 * 2) + i;
							} else {
								break;
							}
						}
					}
					for (int j = 0; j < 2; j++) {
						pFace[j] = pNode[j][i];
						nFace[j] = nLevel[j][i];
					}
					dirFace = (dirEdge + 1) % 3;
					if (CheckStorageFace(pFace)) {
						Element * pTempFace = GetElementFace(pFace, nFace, dirFace);
						if (pTempFace->m_cSimpleTag == m_nTestOut) {
							nSimpleTest = 100;
							break;
						} else if (pTempFace->m_bObject && pTempFace->m_bSkeleton) {
							nSimpleTest ++;
							if (nSimpleTest == 1) {
								pUniqueFace = pTempFace;
								nFace2Edge = (1 * 2) + (1 - i) + 2;
								nEdge2Face = (1 * 2) + i;
							} else {
								break;
							}
						}
					}
				}
				if (nSimpleTest == 1) {
					pThisEdge->m_cSimpleTag = m_nTestOut;
					pThisEdge->m_cPointer = nEdge2Face;
					pUniqueFace->m_cSimpleTag = m_nTestOut;
					pUniqueFace->m_cPointer = nFace2Edge;
					bRtn = true;
				}
			}
		}
	}

	if (bRtn)
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				((NodeHeader *)pNode[i][j])->m_bLastTime = bRtn;

	return bRtn;
}

bool OcTree::RecThinPoint(UCHAR * pNode[2][2][2], int nLevel[2][2][2])
{
	if (!GET_LAST_FIX(pNode[0][0][0]) && !GET_LAST_FIX(pNode[0][0][1]) && !GET_LAST_FIX(pNode[0][1][0]) && !GET_LAST_FIX(pNode[0][1][1])
		&& !GET_LAST_FIX(pNode[1][0][0]) && !GET_LAST_FIX(pNode[1][0][1]) && !GET_LAST_FIX(pNode[1][1][0]) && !GET_LAST_FIX(pNode[1][1][1]))
		return false;

	bool bRtn = false;

	if (NOT_LEAF(pNode[0][0][0]) || NOT_LEAF(pNode[0][0][1]) || NOT_LEAF(pNode[0][1][0]) || NOT_LEAF(pNode[0][1][1]) ||
		NOT_LEAF(pNode[1][0][0]) || NOT_LEAF(pNode[1][0][1]) || NOT_LEAF(pNode[1][1][0]) || NOT_LEAF(pNode[1][1][1]) )
	{
		UCHAR * pNodePoint[2][2][2];
		int nPoint[2][2][2];
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					Leaf(pNode[i][j][k], nLevel[i][j][k], 1-i, 1-j, 1-k, pNodePoint[i][j][k], nPoint[i][j][k]);
		if (RecThinPoint(pNodePoint, nPoint))
			bRtn = true;
	} else {
		Element * pThisPoint = GetElementPoint(pNode, nLevel);
		if (pThisPoint->m_bObject && pThisPoint->m_bSkeleton && !pThisPoint->m_bSketch) {
			if (pThisPoint->m_cSimpleTag == m_nTestLastOut) {
				pThisPoint->m_bSkeleton = 0;
				pThisPoint->m_cSimpleTag = 0;
			} else {
				int nSimpleTest = 0;
				UCHAR * pEdge[2][2];
				int nEdge[2][2];
				int dirEdge;
				Element * pUniqueEdge;
				int nEdge2Point;
				int nPoint2Edge;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pEdge[j][k] = pNode[i][j][k];
							nEdge[j][k] = nLevel[i][j][k];
						}
						dirEdge = 0;
						if (CheckStorageEdge(pEdge)) {
							Element * pTempEdge = GetElementEdge(pEdge, nEdge, dirEdge);
							if (pTempEdge->m_cSimpleTag == m_nTestOut) {
								nSimpleTest = 100;
								break;
							} else if (pTempEdge->m_bObject && pTempEdge->m_bSkeleton) {
								nSimpleTest ++;
								if (nSimpleTest == 1) {
									pUniqueEdge = pTempEdge;
									nEdge2Point = 1 - i + 4;
									nPoint2Edge = dirEdge * 2 + i;
								} else {
									break;
								}
							}
						}
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++)
							{
								pEdge[j][k] = pNode[k][i][j];
								nEdge[j][k] = nLevel[k][i][j];
							}
							dirEdge = 1;
							if (CheckStorageEdge(pEdge)) {
								Element * pTempEdge = GetElementEdge(pEdge, nEdge, dirEdge);
								if (pTempEdge->m_cSimpleTag == m_nTestOut) {
									nSimpleTest = 100;
									break;
								} else if (pTempEdge->m_bObject && pTempEdge->m_bSkeleton) {
									nSimpleTest ++;
									if (nSimpleTest == 1) {
										pUniqueEdge = pTempEdge;
										nEdge2Point = 1 - i + 4;
										nPoint2Edge = dirEdge * 2 + i;
									} else {
										break;
									}
								}
							}
							for (int j = 0; j < 2; j++)
								for (int k = 0; k < 2; k++)
								{
									pEdge[j][k] = pNode[j][k][i];
									nEdge[j][k] = nLevel[j][k][i];
								}
								dirEdge = 2;
								if (CheckStorageEdge(pEdge)) {
									Element * pTempEdge = GetElementEdge(pEdge, nEdge, dirEdge);
									if (pTempEdge->m_cSimpleTag == m_nTestOut) {
										nSimpleTest = 100;
										break;
									} else if (pTempEdge->m_bObject && pTempEdge->m_bSkeleton) {
										nSimpleTest ++;
										if (nSimpleTest == 1) {
											pUniqueEdge = pTempEdge;
											nEdge2Point = 1 - i + 4;
											nPoint2Edge = dirEdge * 2 + i;
										} else {
											break;
										}
									}
								}
				}
				if (nSimpleTest == 1) {
					//pThisPoint->m_cSimpleTag = m_nTestOut;
					pThisPoint->m_bSkeleton = 0;
					pThisPoint->m_cSimpleTag = 0;
					pThisPoint->m_cPointer = nPoint2Edge;
					//pUniqueEdge->m_cSimpleTag = m_nTestOut;
					pUniqueEdge->m_bSkeleton = 0;
					pUniqueEdge->m_cSimpleTag = 0;
					pUniqueEdge->m_cPointer = nEdge2Point;
					bRtn = true;
				}
			}
		}
	}

	if (bRtn)
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					((NodeHeader *)pNode[i][j][k])->m_bLastTime = bRtn;

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
