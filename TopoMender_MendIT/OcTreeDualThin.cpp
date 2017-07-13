#include "OcTree.h"

//////////////////////////////////////////////////////////////////////////
// dual thinning function
//////////////////////////////////////////////////////////////////////////
void OcTree::DualThin()
{
	// Initialization
	((NodeHeader *)(m_pInfiniteNode))->m_bLastTime = 1;
	((NodeHeader *)(m_pInfiniteNode))->m_bLastFix = 1;
	GetElementCell(m_pInfiniteNode)->m_cSimpleTag = 2;
	m_nType = 16;
	RecNode(m_pRoot);

	// Thinning
#ifdef CONSOLE_OUTPUT
	printf("Dual thinning for background :");
	printf("\n\tMain dual thinning process start :");
	printf("\n\tStep count - %3d", 0);
#endif
	TimeMeter cTimer;
	cTimer.Start();
	m_nStep = 0;
	bool bContinue = true;

	while (bContinue) {
#ifdef CONSOLE_OUTPUT
		printf("\b\b\b%3d", m_nStep);
#endif
		m_bCountOnly = true;
		DualThinStep();

		m_bCountOnly = false;
		bContinue = DualThinStep();
		m_nStep++;
	}

	cTimer.End();
#ifdef CONSOLE_OUTPUT
	printf("\b\b\b%3d", m_nStep);
	printf("\n\tMain dual thinning process finished in %d step(s).", m_nStep);
	printf("\n\tTime consuming ");
	cTimer.Print();
#endif

	// Finallization
	printf("\n\n");
}

bool OcTree::DualThinStep()
{
	bool bRtn = false;
	UCHAR * pNodeFace[2];
	UCHAR * pNodeEdge[2][2];
	UCHAR * pNodePoint[2][2][2];
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
							pNodePoint[ii][jj][kk] = m_pInfiniteNode;
							nPoint[ii][jj][kk] = -1;
						}
						pNodePoint[i][j][k] = m_pRoot;
						nPoint[i][j][k] = 0;
						if (RecDualThinPoint(pNodePoint, nPoint))
							bRtn = true;
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
						if (RecDualThinEdge(pNodeEdge, nEdge, 0))
							bRtn = true;
						if (RecDualThinEdge(pNodeEdge, nEdge, 1))
							bRtn = true;
						if (RecDualThinEdge(pNodeEdge, nEdge, 2))
							bRtn = true;
				}
				for (int i = 0; i < 2; i++)
				{
					for (int ii = 0; ii < 2; ii++) {
						pNodeFace[ii] = m_pInfiniteNode;
						nFace[ii] = -1;
					}
					pNodeFace[i] = m_pRoot;
					nFace[i] = 0;
					if (RecDualThinFace(pNodeFace, nFace, 0))
						bRtn = true;
					if (RecDualThinFace(pNodeFace, nFace, 1))
						bRtn = true;
					if (RecDualThinFace(pNodeFace, nFace, 2))
						bRtn = true;
				}
				if (RecDualThinCell(m_pRoot, 0))
					bRtn = true;
				return bRtn;
}

bool OcTree::RecDualThinCell(UCHAR * pNode, int nLevel)
{
	bool bRtn = false;

	if (NOT_LEAF(pNode)) {
		if (GET_LAST_FIX(pNode)) {
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
			if (RecDualThinPoint(pNodePoint, nPoint))
				bRtn = true;
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pNodeEdge[j][k] = pNodePoint[i][j][k];
						nEdge[j][k] = nPoint[i][j][k];
					}
					if (RecDualThinEdge(pNodeEdge, nEdge, 0))
						bRtn = true;
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[k][i][j];
							nEdge[j][k] = nPoint[k][i][j];
						}
						if (RecDualThinEdge(pNodeEdge, nEdge, 1))
							bRtn = true;
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++)
							{
								pNodeEdge[j][k] = pNodePoint[j][k][i];
								nEdge[j][k] = nPoint[j][k][i];
							}
							if (RecDualThinEdge(pNodeEdge, nEdge, 2))
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
					if (RecDualThinFace(pNodeFace, nFace, 0))
						bRtn = true;
					for (int k = 0; k < 2; k++)
					{
						pNodeFace[k] = pNodePoint[j][k][i];
						nFace[k] = nPoint[j][k][i];
					}
					if (RecDualThinFace(pNodeFace, nFace, 1))
						bRtn = true;
					for (int k = 0; k < 2; k++)
					{
						pNodeFace[k] = pNodePoint[i][j][k];
						nFace[k] = nPoint[i][j][k];
					}
					if (RecDualThinFace(pNodeFace, nFace, 2))
						bRtn = true;
				}
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++) {
							pNodeCell = pNodePoint[i][j][k];
							nCell = nPoint[i][j][k];
							if (RecDualThinCell(pNodeCell, nCell))
								bRtn = true;
						}
		}
	} else {
		Element * pThisCell = GetElementCell(pNode);
		if (!pThisCell->m_bObject && pThisCell->m_bSkeleton) {
			if (m_bCountOnly) {
				// do nothing
			} else {
				// reset
				pThisCell->m_cSimpleTag = 0;
			}
		}
	}

	if (!m_bCountOnly) {
		if (GET_LAST_TIME(pNode))
			bRtn = true;
		((NodeHeader *)pNode)->m_bLastFix = bRtn;
		((NodeHeader *)pNode)->m_bLastTime = false;
	}

	return bRtn;
}

bool OcTree::RecDualThinFace(UCHAR * pNode[2], int nLevel[2], int dirFace)
{
	bool bRtn = false;

	if (NOT_LEAF(pNode[0]) || NOT_LEAF(pNode[1])) {
		if (GET_LAST_FIX(pNode[0]) || GET_LAST_FIX(pNode[1])) {
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
				if (RecDualThinPoint(pNodePoint, nPoint))
					bRtn = true;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[k][i][j];
							nEdge[j][k] = nPoint[k][i][j];
						}
						if (RecDualThinEdge(pNodeEdge, nEdge, 1))
							bRtn = true;
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++)
							{
								pNodeEdge[j][k] = pNodePoint[j][k][i];
								nEdge[j][k] = nPoint[j][k][i];
							}
							if (RecDualThinEdge(pNodeEdge, nEdge, 2))
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
						if (RecDualThinFace(pNodeFace, nFace, 0))
							bRtn = true;
					}
					break;
			case 1:
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
							Leaf(pNode[j], nLevel[j], i, 1-j, k, pNodePoint[i][j][k], nPoint[i][j][k]);
				if (RecDualThinPoint(pNodePoint, nPoint))
					bRtn = true;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[j][k][i];
							nEdge[j][k] = nPoint[j][k][i];
						}
						if (RecDualThinEdge(pNodeEdge, nEdge, 2))
							bRtn = true;
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++)
							{
								pNodeEdge[j][k] = pNodePoint[i][j][k];
								nEdge[j][k] = nPoint[i][j][k];
							}
							if (RecDualThinEdge(pNodeEdge, nEdge, 0))
								bRtn = true;
				}
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
					{
						for (int k = 0; k < 2; k++)
						{
							pNodeFace[k] = pNodePoint[j][k][i];
							nFace[k] = nPoint[j][k][i];
						}
						if (RecDualThinFace(pNodeFace, nFace, 1))
							bRtn = true;
					}
					break;
			case 2:
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
							Leaf(pNode[k], nLevel[k], i, j, 1-k, pNodePoint[i][j][k], nPoint[i][j][k]);
				if (RecDualThinPoint(pNodePoint, nPoint))
					bRtn = true;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[i][j][k];
							nEdge[j][k] = nPoint[i][j][k];
						}
						if (RecDualThinEdge(pNodeEdge, nEdge, 0))
							bRtn = true;
						for (int j = 0; j < 2; j++)
							for (int k = 0; k < 2; k++)
							{
								pNodeEdge[j][k] = pNodePoint[k][i][j];
								nEdge[j][k] = nPoint[k][i][j];
							}
							if (RecDualThinEdge(pNodeEdge, nEdge, 1))
								bRtn = true;
				}
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
					{
						for (int k = 0; k < 2; k++)
						{
							pNodeFace[k] = pNodePoint[i][j][k];
							nFace[k] = nPoint[i][j][k];
						}
						if (RecDualThinFace(pNodeFace, nFace, 2))
							bRtn = true;
					}
					break;
			}
		}
	} else {
		Element * pThisFace = GetElementFace(pNode, nLevel, dirFace);
		if (!pThisFace->m_bObject && pThisFace->m_bSkeleton) {
			if (m_bCountOnly) {
				for (int i = 0; i < 2; i++)
				{
					if (GET_LAST_FIX(pNode[i])) {
						Element * pTempCell = GetElementCell(pNode[i]);
						if (!pTempCell->m_bObject && pTempCell->m_bSkeleton && pTempCell->m_cSimpleTag != 2) {
							pTempCell->m_cSimpleTag ++;
						}
					}
				}
			} else {
				Element * pUniqueCell;
				for (int i = 0; i < 2; i++) {
					pUniqueCell = GetElementCell(pNode[i]);
					if (pUniqueCell->m_cSimpleTag == 1) {
						pThisFace->m_bSkeleton = 0;
						pThisFace->m_cSimpleTag = 0;
						pThisFace->m_cPointer = i;
						pUniqueCell->m_bSkeleton = 0;
						pUniqueCell->m_cSimpleTag = 0;
						pUniqueCell->m_cPointer = dirFace * 2 + (1 - i);
#ifdef CLEAN_SKELETON
						bRtn = true;
#else
						bRtn = false;
#endif
						if ( m_bCleanSkeleton )
							bRtn = true;
						else
							bRtn = false;
						break;
					}
				}
				pThisFace->m_cSimpleTag = 0;
			}
		}
	}

	if (bRtn)
		for (int i = 0; i < 2; i++)
			((NodeHeader *)pNode[i])->m_bLastTime = bRtn;

	return bRtn;
}

bool OcTree::RecDualThinEdge(UCHAR * pNode[2][2], int nLevel[2][2], int dirEdge)
{
	bool bRtn = false;

	if (NOT_LEAF(pNode[0][0]) || NOT_LEAF(pNode[0][1]) || NOT_LEAF(pNode[1][0]) || NOT_LEAF(pNode[1][1])) {
		if (GET_LAST_FIX(pNode[0][0]) || GET_LAST_FIX(pNode[0][1]) || GET_LAST_FIX(pNode[1][0]) || GET_LAST_FIX(pNode[1][1])) {
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
				if (RecDualThinPoint(pNodePoint, nPoint))
					bRtn = true;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[i][j][k];
							nEdge[j][k] = nPoint[i][j][k];
						}
						if (RecDualThinEdge(pNodeEdge, nEdge, 0))
							bRtn = true;
				}
				break;
			case 1:
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
							Leaf(pNode[k][i], nLevel[k][i], 1-i, j, 1-k, pNodePoint[i][j][k], nPoint[i][j][k]);
				if (RecDualThinPoint(pNodePoint, nPoint))
					bRtn = true;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[k][i][j];
							nEdge[j][k] = nPoint[k][i][j];
						}
						if (RecDualThinEdge(pNodeEdge, nEdge, 1))
							bRtn = true;
				}
				break;
			case 2:
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
							Leaf(pNode[i][j], nLevel[i][j], 1-i, 1-j, k, pNodePoint[i][j][k], nPoint[i][j][k]);
				if (RecDualThinPoint(pNodePoint, nPoint))
					bRtn = true;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pNodeEdge[j][k] = pNodePoint[j][k][i];
							nEdge[j][k] = nPoint[j][k][i];
						}
						if (RecDualThinEdge(pNodeEdge, nEdge, 2))
							bRtn = true;
				}
				break;
			}
		}
	} else {
		Element * pThisEdge = GetElementEdge(pNode, nLevel, dirEdge);
		if (!pThisEdge->m_bObject && pThisEdge->m_bSkeleton) {
			UCHAR * pFace[2];
			int nFace[2];
			int dirFace;

			if (m_bCountOnly) {
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++) {
						pFace[j] = pNode[i][j];
						nFace[j] = nLevel[i][j];
					}
					dirFace = (dirEdge + 2) % 3;
					if (GET_LAST_FIX(pFace[0]) && GET_LAST_FIX(pFace[1])) {
						if (CheckStorageFace(pFace)) {
							Element * pTempFace = GetElementFace(pFace, nFace, dirFace);
							if (!pTempFace->m_bObject && pTempFace->m_bSkeleton && pTempFace->m_cSimpleTag != 2) {
								pTempFace->m_cSimpleTag ++;
							}
						}
					}
					for (int j = 0; j < 2; j++) {
						pFace[j] = pNode[j][i];
						nFace[j] = nLevel[j][i];
					}
					dirFace = (dirEdge + 1) % 3;
					if (GET_LAST_FIX(pFace[0]) && GET_LAST_FIX(pFace[1])) {
						if (CheckStorageFace(pFace)) {
							Element * pTempFace = GetElementFace(pFace, nFace, dirFace);
							if (!pTempFace->m_bObject && pTempFace->m_bSkeleton && pTempFace->m_cSimpleTag != 2) {
								pTempFace->m_cSimpleTag ++;
							}
						}
					}
				}
			} else {
				Element * pUniqueFace;
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++) {
						pFace[j] = pNode[i][j];
						nFace[j] = nLevel[i][j];
					}
					dirFace = (dirEdge + 2) % 3;
					if (CheckStorageFace(pFace)) {
						pUniqueFace = GetElementFace(pFace, nFace, dirFace);
						if (pUniqueFace->m_cSimpleTag == 1) {
							pThisEdge->m_bSkeleton = 0;
							pThisEdge->m_cSimpleTag = 0;
							pThisEdge->m_cPointer = (0 * 2) + i;
							pUniqueFace->m_bSkeleton = 0;
							pUniqueFace->m_cSimpleTag = 0;
							pUniqueFace->m_cPointer = (0 * 2) + (1 - i) + 2;
							bRtn = true;
							break;
						}
					}
					for (int j = 0; j < 2; j++) {
						pFace[j] = pNode[j][i];
						nFace[j] = nLevel[j][i];
					}
					dirFace = (dirEdge + 1) % 3;
					if (CheckStorageFace(pFace)) {
						pUniqueFace = GetElementFace(pFace, nFace, dirFace);
						if (pUniqueFace->m_cSimpleTag == 1) {
							pThisEdge->m_bSkeleton = 0;
							pThisEdge->m_cSimpleTag = 0;
							pThisEdge->m_cPointer = (1 * 2) + i;
							pUniqueFace->m_bSkeleton = 0;
							pUniqueFace->m_cSimpleTag = 0;
							pUniqueFace->m_cPointer = (1 * 2) + (1 - i) + 2;
							bRtn = true;
							break;
						}
					}
				}
				pThisEdge->m_cSimpleTag = 0;
			}
		}
	}

	if (bRtn)
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				((NodeHeader *)pNode[i][j])->m_bLastTime = bRtn;

	return bRtn;
}

bool OcTree::RecDualThinPoint(UCHAR * pNode[2][2][2], int nLevel[2][2][2])
{
	bool bRtn = false;

	if (NOT_LEAF(pNode[0][0][0]) || NOT_LEAF(pNode[0][0][1]) || NOT_LEAF(pNode[0][1][0]) || NOT_LEAF(pNode[0][1][1]) ||
		NOT_LEAF(pNode[1][0][0]) || NOT_LEAF(pNode[1][0][1]) || NOT_LEAF(pNode[1][1][0]) || NOT_LEAF(pNode[1][1][1]) )
	{
		if (GET_LAST_FIX(pNode[0][0][0]) || GET_LAST_FIX(pNode[0][0][1]) || GET_LAST_FIX(pNode[0][1][0]) || GET_LAST_FIX(pNode[0][1][1]) || 
			GET_LAST_FIX(pNode[1][0][0]) || GET_LAST_FIX(pNode[1][0][1]) || GET_LAST_FIX(pNode[1][1][0]) || GET_LAST_FIX(pNode[1][1][1])) {
				UCHAR * pNodePoint[2][2][2];
				int nPoint[2][2][2];
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
							Leaf(pNode[i][j][k], nLevel[i][j][k], 1-i, 1-j, 1-k, pNodePoint[i][j][k], nPoint[i][j][k]);
				if (RecDualThinPoint(pNodePoint, nPoint))
					bRtn = true;
		}
	} else {
		Element * pThisPoint = GetElementPoint(pNode, nLevel);
		if (!pThisPoint->m_bObject && pThisPoint->m_bSkeleton) {
			UCHAR * pEdge[2][2];
			int nEdge[2][2];
			int dirEdge;

			if (m_bCountOnly) {
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pEdge[j][k] = pNode[i][j][k];
							nEdge[j][k] = nLevel[i][j][k];
						}
						dirEdge = 0;
						if (GET_LAST_FIX(pEdge[0][0]) && GET_LAST_FIX(pEdge[0][1]) && GET_LAST_FIX(pEdge[1][0]) && GET_LAST_FIX(pEdge[1][1])) {
							if (CheckStorageEdge(pEdge)) {
								Element * pTempEdge = GetElementEdge(pEdge, nEdge, dirEdge);
								if (!pTempEdge->m_bObject && pTempEdge->m_bSkeleton && pTempEdge->m_cSimpleTag != 2) {
									pTempEdge->m_cSimpleTag ++;
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
							if (GET_LAST_FIX(pEdge[0][0]) && GET_LAST_FIX(pEdge[0][1]) && GET_LAST_FIX(pEdge[1][0]) && GET_LAST_FIX(pEdge[1][1])) {
								if (CheckStorageEdge(pEdge)) {
									Element * pTempEdge = GetElementEdge(pEdge, nEdge, dirEdge);
									if (!pTempEdge->m_bObject && pTempEdge->m_bSkeleton && pTempEdge->m_cSimpleTag != 2) {
										pTempEdge->m_cSimpleTag ++;
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
								if (GET_LAST_FIX(pEdge[0][0]) && GET_LAST_FIX(pEdge[0][1]) && GET_LAST_FIX(pEdge[1][0]) && GET_LAST_FIX(pEdge[1][1])) {
									if (CheckStorageEdge(pEdge)) {
										Element * pTempEdge = GetElementEdge(pEdge, nEdge, dirEdge);
										if (!pTempEdge->m_bObject && pTempEdge->m_bSkeleton && pTempEdge->m_cSimpleTag != 2) {
											pTempEdge->m_cSimpleTag ++;
										}
									}
								}
				}
			} else {
				Element * pUniqueEdge;
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
							pUniqueEdge = GetElementEdge(pEdge, nEdge, dirEdge);
							if (pUniqueEdge->m_cSimpleTag == 1) {
								pThisPoint->m_bSkeleton = 0;
								pThisPoint->m_cSimpleTag = 0;
								pThisPoint->m_cPointer = dirEdge * 2 + i;
								pUniqueEdge->m_bSkeleton = 0;
								pUniqueEdge->m_cSimpleTag = 0;
								pUniqueEdge->m_cPointer = 1 - i + 4;
								bRtn = true;
								break;
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
								pUniqueEdge = GetElementEdge(pEdge, nEdge, dirEdge);
								if (pUniqueEdge->m_cSimpleTag == 1) {
									pThisPoint->m_bSkeleton = 0;
									pThisPoint->m_cSimpleTag = 0;
									pThisPoint->m_cPointer = dirEdge * 2 + i;
									pUniqueEdge->m_bSkeleton = 0;
									pUniqueEdge->m_cSimpleTag = 0;
									pUniqueEdge->m_cPointer = 1 - i + 4;
									bRtn = true;
									break;
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
									pUniqueEdge = GetElementEdge(pEdge, nEdge, dirEdge);
									if (pUniqueEdge->m_cSimpleTag == 1) {
										pThisPoint->m_bSkeleton = 0;
										pThisPoint->m_cSimpleTag = 0;
										pThisPoint->m_cPointer = dirEdge * 2 + i;
										pUniqueEdge->m_bSkeleton = 0;
										pUniqueEdge->m_cSimpleTag = 0;
										pUniqueEdge->m_cPointer = 1 - i + 4;
										bRtn = true;
										break;
									}
								}
				}
				pThisPoint->m_cSimpleTag = 0;
			}
		}
	}

	if (bRtn)
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					((NodeHeader *)pNode[i][j][k])->m_bLastTime = bRtn;

	return bRtn;
}
