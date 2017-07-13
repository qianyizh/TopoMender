#include "stdafx.h"
#include ".\hashmap.h"

CHashMap::CHashMap(CConstant * pConstant, int nSize)
{
	m_pConstant = pConstant;
	for (int i = 0; i < 40; i++) {
		if (nSize * 2 < pConstant->m_pHashSize[i])
			break;
	}
	m_nNum = pConstant->m_pHashSize[i];
	m_pData = new (CHashNode *[m_nNum]);
	for (int i = 0; i < m_nNum; i++)
		m_pData[i] = NULL;
	m_nHashKey = (((UINT64)1) << 60) % m_nNum;
}

CHashMap::~CHashMap(void)
{
	for (int i = 0; i < m_nNum; i++)
		if (m_pData[i] != NULL)
			delete m_pData[i];

	delete []m_pData;
}

void CHashMap::Set(UINT64 nKey0, UINT64 nKey1, int nIndex)
{
	int i = Hash(nKey0, nKey1);
	CHashNode ** ppNode = &m_pData[i];
	while (*ppNode != NULL && ((*ppNode)->m_nKey[0] != nKey0 || (*ppNode)->m_nKey[1] != nKey1))
		ppNode = &((*ppNode)->m_pNext);
	if (*ppNode == NULL)
		*ppNode = new CHashNode(nKey0, nKey1, nIndex);
	else
		(*ppNode)->m_nIndex = nIndex;
}

int CHashMap::Get(UINT64 nKey0, UINT64 nKey1)
{
	int i = Hash(nKey0, nKey1);
	CHashNode * pNode = m_pData[i];
	while (pNode != NULL && (pNode->m_nKey[0] != nKey0 || pNode->m_nKey[1] != nKey1))
		pNode = pNode->m_pNext;
	return (pNode == NULL ? -1 : pNode->m_nIndex);
}

int CHashMap::Hash(UINT64 nKey0, UINT64 nKey1)
{
	UINT64 k0 = nKey0 % m_nNum;
	UINT64 k1 = nKey1 % m_nNum;
	UINT64 k = k0 * m_nHashKey + k1;
	return (int)(k % m_nNum);
}
