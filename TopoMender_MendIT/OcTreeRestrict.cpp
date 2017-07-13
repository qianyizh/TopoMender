#include "OcTree.h"

//////////////////////////////////////////////////////////////////////////
// restrict functions
//////////////////////////////////////////////////////////////////////////
void OcTree::LevelRestrict()
{
	while (RecLevelRestrictCheckCell(m_pRoot, 0)) {
		RecLevelRestrictNode(&m_pRoot);
	}
#ifdef CONSOLE_OUTPUT
	printf("Restrict nodes (neighboring nodes differ by no more than one level), finished.\n");
#endif
}

bool OcTree::RecLevelRestrictCheckCell(UCHAR * pNode, int nLevel)
{
	bool bRtn = false;

	if (NOT_LEAF(pNode)) {
		UCHAR * pNodeCell;
		UCHAR * pNodeFace[2];
		UCHAR * pNodePoint[2][2][2];
		int nCell;
		int nFace[2];
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
					if (RecLevelRestrictCheckCell(pNodeCell, nCell))
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
						if (RecLevelRestrictCheckFace(pNodeFace, nFace, 0))
							bRtn = true;
						for (int k = 0; k < 2; k++)
						{
							pNodeFace[k] = pNodePoint[j][k][i];
							nFace[k] = nPoint[j][k][i];
						}
						if (RecLevelRestrictCheckFace(pNodeFace, nFace, 1))
							bRtn = true;
						for (int k = 0; k < 2; k++)
						{
							pNodeFace[k] = pNodePoint[i][j][k];
							nFace[k] = nPoint[i][j][k];
						}
						if (RecLevelRestrictCheckFace(pNodeFace, nFace, 2))
							bRtn = true;
					}
	}

	return bRtn;
}

bool OcTree::RecLevelRestrictCheckFace(UCHAR * pNode[2], int nLevel[2], int dirFace)
{
	bool bRtn = false;

	if (NOT_LEAF(pNode[0]) || NOT_LEAF(pNode[1])) {
		UCHAR * pNodeFace[2];
		UCHAR * pNodePoint[2][2][2];
		int nFace[2];
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
					if (RecLevelRestrictCheckFace(pNodeFace, nFace, 0))
						bRtn = true;
				}
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
					if (RecLevelRestrictCheckFace(pNodeFace, nFace, 1))
						bRtn = true;
				}
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
					if (RecLevelRestrictCheckFace(pNodeFace, nFace, 2))
						bRtn = true;
				}
				break;
		}
	} else {
		if ( nLevel[0] - nLevel[1] > 1 ) {
			((NodeHeader *)pNode[1])->m_bLastTime = 1;
			bRtn = true;
		} else if ( nLevel[1] - nLevel[0] > 1 ) {
			((NodeHeader *)pNode[0])->m_bLastTime = 1;
			bRtn = true;
		}
	}

	return bRtn;
}

void OcTree::RecLevelRestrictNode(UCHAR ** ppNode)
{
	if (NOT_LEAF(*ppNode)) {
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					RecLevelRestrictNode(& ((InterNode *)(*ppNode))->m_pNode[i*4+j*2+k]);
				}
	} else {
		if (((NodeHeader *)(*ppNode))->m_cType == 1 && ((NodeHeader *)(*ppNode))->m_bLastTime) { // well, we should split it
			((NodeHeader *)(*ppNode))->m_bLastTime = 0;
			UCHAR * pOldNode = *ppNode;
			(*ppNode) = m_cMemoryManager.New( sizeof(InterNode) );
			((InterNode *)(*ppNode))->m_cType = 0;
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						((InterNode *)(*ppNode))->m_pNode[i*4+j*2+k] = m_cMemoryManager.New( sizeof(LeafNode) );
						((LeafNode *)((InterNode *)(*ppNode))->m_pNode[i*4+j*2+k])->m_nType = ((LeafNode *)(pOldNode))->m_nType;
						((LeafNode *)((InterNode *)(*ppNode))->m_pNode[i*4+j*2+k])->m_nVertex = ((LeafNode *)(pOldNode))->m_nVertex;
					}
		}
	}
}
