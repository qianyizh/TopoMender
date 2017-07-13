#include "SingleHashMap.h"

//SingleHashNode

SingleHashNode::SingleHashNode(UINT64 nKey, const CVector3D & vData)
{
	m_nKey = nKey;
	m_vData = vData;
	m_pNext = NULL;
}

SingleHashNode::~SingleHashNode()
{
	if (m_pNext != NULL)
		delete m_pNext;
}

//SingleHashMap

SingleHashMap::SingleHashMap(Constant * pConstant, int nSize)
{
	m_pConstant = pConstant;
	int i;
	for (i = 0; i < 40; i++) {
		if (nSize * 2 < pConstant->m_pHashSize[i])
			break;
	}
	m_nNum = pConstant->m_pHashSize[i];
	m_pData = new (SingleHashNode *[m_nNum]);
	for (i = 0; i < m_nNum; i++)
		m_pData[i] = NULL;
}

SingleHashMap::~SingleHashMap()
{
	for (int i = 0; i < m_nNum; i++)
		if (m_pData[i] != NULL)
			delete m_pData[i];
	delete []m_pData;
}

void SingleHashMap::Set(UINT64 nKey, const CVector3D & vData)
{
	int i = Hash(nKey);
	SingleHashNode ** ppNode = &m_pData[i];
	while (*ppNode != NULL && (*ppNode)->m_nKey != nKey)
		ppNode = &((*ppNode)->m_pNext);
	if (*ppNode == NULL)
		*ppNode = new SingleHashNode(nKey, vData);
	else
		(*ppNode)->m_vData = vData;
}

CVector3D SingleHashMap::Get(UINT64 nKey)
{
	int i = Hash(nKey);
	SingleHashNode * pNode = m_pData[i];
	while (pNode != NULL && pNode->m_nKey != nKey)
		pNode = pNode->m_pNext;
	return (pNode == NULL ? CVector3D(1e300, 1e300, 1e300) : pNode->m_vData);
}

int SingleHashMap::Hash(UINT64 nKey)
{
	return (int)(nKey % m_nNum);
}
