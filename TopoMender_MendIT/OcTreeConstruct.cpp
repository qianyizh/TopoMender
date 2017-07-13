#include "OcTree.h"
using namespace std;

//////////////////////////////////////////////////////////////////////////
// construction functions
//////////////////////////////////////////////////////////////////////////
bool OcTree::LoadFromSOG( string strSOGFile )
{
#ifdef CONSOLE_OUTPUT
	printf("====Initializaion&Input====\n");
	printf("InputfromSOGfile:\n\t%s\n", strSOGFile.c_str());
#endif

	m_strSOGFile = strSOGFile;
	m_pSOGFile = fopen( m_strSOGFile.c_str(), "rb" );
	if ( ParseSOGFile( m_pSOGFile, m_vCorner, m_dbLength ) == false ) {
		cerr << "Unrecognized SOG file!" << endl; ;
		return false;
	}

	m_dbNodeLength = m_dbLength / (double)NLENGTH(0);
	fread(&m_nMaxLevel, sizeof(int), 1, m_pSOGFile);
	int i = 0;
	while (m_nMaxLevel != 1) {
		i++;
		m_nMaxLevel >>= 1;
	}
	m_nMaxLevel = i;

#ifdef CONSOLE_OUTPUT
	printf("Origin : (%6.4f, %6.4f, %6.4f), Dimension : %6.4f, Level : %d\n", m_vCorner.pVec[0], m_vCorner.pVec[1], m_vCorner.pVec[2], m_dbLength, m_nMaxLevel);
#endif

	RecLoadFromSOG(&m_pRoot, m_pSOGFile);

	fclose(m_pSOGFile);
	return true;
}

void OcTree::RecLoadFromSOG(UCHAR ** ppNode, FILE * pFile)
{
	UCHAR nType;
	UCHAR nNext;
	float fNext[3];
	fread(&nType, 1, 1, pFile);
	switch(nType) {
	case 0:
		*ppNode = m_cMemoryManager.New( sizeof(InterNode) );
		((InterNode *)(*ppNode))->m_cType = 0;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					RecLoadFromSOG(&(((InterNode *)(*ppNode))->m_pNode[i*4+j*2+k]), pFile);
		break;
	case 1:
		fread(&nNext, 1, 1, pFile);
		*ppNode = m_cMemoryManager.New( sizeof(LeafNode) );
		((LeafNode *)(*ppNode))->m_nType = 1;
		if (nNext != 0) {
			nNext = 0xff;
		}
		((LeafNode *)(*ppNode))->m_nVertex = nNext;
		break;
	case 2:
		fread(&nNext, 1, 1, pFile);
		fread(fNext, 4, 3, pFile);
		*ppNode = m_cMemoryManager.New( sizeof(LeafNode) );
		((LeafNode *)(*ppNode))->m_nType = 2;
		((LeafNode *)(*ppNode))->m_nVertex = nNext;
		break;
	}
}

void OcTree::DuplicateOcTree(OcTree * pOcTree)
{
	m_nMaxLevel = pOcTree->m_nMaxLevel;
	m_vCorner = pOcTree->m_vCorner;
	m_dbLength = pOcTree->m_dbLength;
	m_dbNodeLength = pOcTree->m_dbNodeLength;
	RecDuplicateOcTree(&m_pRoot, pOcTree->m_pRoot, pOcTree);
}

void OcTree::RecDuplicateOcTree(UCHAR ** ppNode, UCHAR * pOcTreeNode, OcTree * pOcTree)
{
	UCHAR nType = (*pOcTreeNode) & 0x3;
	InterNode * pTempInter;
	LeafNode * pTempLeaf;
	switch(nType) {
	case 0:
		pTempInter = (InterNode *)pOcTreeNode;
		*ppNode = m_cMemoryManager.New( sizeof(InterNode) );
		((InterNode *)(*ppNode))->m_cType = 0;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					RecDuplicateOcTree(&(((InterNode *)(*ppNode))->m_pNode[i*4+j*2+k]), pTempInter->m_pNode[i*4+j*2+k], pOcTree);
		break;
	case 2:
	case 1:
		pTempLeaf = (LeafNode *)pOcTreeNode;
		*ppNode = m_cMemoryManager.New( sizeof(LeafNode) );
		((LeafNode *)(*ppNode))->m_nType = pTempLeaf->m_nType;
		((LeafNode *)(*ppNode))->m_nVertex = pTempLeaf->m_nVertex;
		break;
	}
}

void OcTree::LoadFromOcTree(OcTree * pOcTree)
{
	m_nMaxLevel = pOcTree->m_nMaxLevel;
	m_vCorner = pOcTree->m_vCorner;
	m_dbLength = pOcTree->m_dbLength;
	m_dbNodeLength = pOcTree->m_dbNodeLength;
	RecLoadFromOcTree(&m_pRoot, pOcTree->m_pRoot, pOcTree);
}

void OcTree::RecLoadFromOcTree(UCHAR ** ppNode, UCHAR * pOcTreeNode, OcTree * pOcTree)
{
	UCHAR nType = (*pOcTreeNode) & 0x3;
	InterNode * pTempInter;
	LeafNode * pTempLeaf;
	InterNode interNodeTemp;
	LeafNode leafNodeTemp;
	UCHAR * pPointer;
	int nCount;
	UINT nTemp;
	switch(nType) {
	case 0:
		pTempInter = (InterNode *)pOcTreeNode;
		interNodeTemp = *pTempInter;
		pOcTree->m_cMemoryManager.Delete( pOcTreeNode );
		*ppNode = m_cMemoryManager.New( sizeof(InterNode) );
		((InterNode *)(*ppNode))->m_cType = 0;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					RecLoadFromOcTree(&(((InterNode *)(*ppNode))->m_pNode[i*4+j*2+k]), interNodeTemp.m_pNode[i*4+j*2+k], pOcTree);
		break;
	case 2:
	case 1:
		pTempLeaf = (LeafNode *)pOcTreeNode;
		leafNodeTemp = *pTempLeaf;
		pOcTree->m_cMemoryManager.Delete( pOcTreeNode );
		nCount = CountElement( (UCHAR *)&leafNodeTemp, 27 );
#ifdef BETTER_GENERATING_SET
		*ppNode = m_cMemoryManager.New( sizeof(LeafNode) + nCount * (sizeof(Element) + sizeof(UINT64)) );
#else
		*ppNode = m_cMemoryManager.New( sizeof(LeafNode) + nCount * (sizeof(Element)) );
#endif
		((LeafNode *)(*ppNode))->m_nType = leafNodeTemp.m_nType;
		((LeafNode *)(*ppNode))->m_nVertex = 0;
		nTemp = leafNodeTemp.m_nType;
		UCHAR nNext = (UCHAR)leafNodeTemp.m_nVertex;
		// cell
		pPointer = (*ppNode) + sizeof(LeafNode);
		nTemp >>= 5;
		if ( nTemp & 1 ) {
			*pPointer = 0;
			((Element *)pPointer)->m_bSkeleton = 1;
			if (GET_POINT_INSIDE(nNext, 0) && GET_POINT_INSIDE(nNext, 1) && GET_POINT_INSIDE(nNext, 2) && GET_POINT_INSIDE(nNext, 3)
				&& GET_POINT_INSIDE(nNext, 4) && GET_POINT_INSIDE(nNext, 5) && GET_POINT_INSIDE(nNext, 6) && GET_POINT_INSIDE(nNext, 7))
			{
				((Element *)pPointer)->m_bObject = 1;
			}
#ifdef BETTER_GENERATING_SET
			*((UINT64 *)(pPointer + sizeof(Element))) = INT64_INVALID;
			pPointer += (sizeof(Element) + sizeof(UINT64));
#else
			pPointer += sizeof(Element);
#endif
		}
		// face
		for (int i = 0; i < 6; i++)
		{
			nTemp >>= 1;
			if ( nTemp & 1 ) {
				*pPointer = 0;
				((Element *)pPointer)->m_bSkeleton = 1;
				if (GET_POINT_INSIDE(nNext, Constant::I_SHIFT_FP[i][0]) &&
					GET_POINT_INSIDE(nNext, Constant::I_SHIFT_FP[i][1]) && 
					GET_POINT_INSIDE(nNext, Constant::I_SHIFT_FP[i][2]) && 
					GET_POINT_INSIDE(nNext, Constant::I_SHIFT_FP[i][3])
					)
				{
					((Element *)pPointer)->m_bObject = 1;
				}
#ifdef BETTER_GENERATING_SET
				*((UINT64 *)(pPointer + sizeof(Element))) = INT64_INVALID;
				pPointer += (sizeof(Element) + sizeof(UINT64));
#else
				pPointer += sizeof(Element);
#endif
			}
		}
		// edge
		for (int i = 0; i < 12; i++)
		{
			nTemp >>= 1;
			if ( nTemp & 1 ) {
				*pPointer = 0;
				((Element *)pPointer)->m_bSkeleton = 1;
				if (GET_POINT_INSIDE(nNext, Constant::I_SHIFT_EP[i][0]) &&
					GET_POINT_INSIDE(nNext, Constant::I_SHIFT_EP[i][1]))
				{
					((Element *)pPointer)->m_bObject = 1;
				}
#ifdef BETTER_GENERATING_SET
				*((UINT64 *)(pPointer + sizeof(Element))) = INT64_INVALID;
				pPointer += (sizeof(Element) + sizeof(UINT64));
#else
				pPointer += sizeof(Element);
#endif
			}
		}
		// point
		for (int i = 0; i < 8; i++)
		{
			nTemp >>= 1;
			if ( nTemp & 1 ) {
				*pPointer = 0;
				((Element *)pPointer)->m_bSkeleton = 1;
				if (GET_POINT_INSIDE(nNext, i))
				{
					((Element *)pPointer)->m_bObject = 1;
				}
#ifdef BETTER_GENERATING_SET
				*((UINT64 *)(pPointer + sizeof(Element))) = INT64_INVALID;
				pPointer += (sizeof(Element) + sizeof(UINT64));
#else
				pPointer += sizeof(Element);
#endif
			}
		}
		break;
	}
}
