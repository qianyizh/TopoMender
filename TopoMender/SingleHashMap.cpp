#include "StdAfx.h"
#include ".\singlehashmap.h"
#using <mscorlib.dll>

CSingleHashMap::CSingleHashMap(CConstant * pConstant, int nSize)
{
	m_pConstant = pConstant;
	for (int i = 0; i < 40; i++) {
		if (nSize * 2 < pConstant->m_pHashSize[i])
			break;
	}
	m_nNum = pConstant->m_pHashSize[i];
	m_pData = new (CSingleHashNode *[m_nNum]);
	for (int i = 0; i < m_nNum; i++)
		m_pData[i] = NULL;
}

CSingleHashMap::~CSingleHashMap(void)
{
	for (int i = 0; i < m_nNum; i++)
		if (m_pData[i] != NULL)
			delete m_pData[i];

	delete []m_pData;
}

void CSingleHashMap::Set(int nKey, const CVector3D & vData)
{
	int i = Hash(nKey);
	CSingleHashNode ** ppNode = &m_pData[i];
	while (*ppNode != NULL && (*ppNode)->m_nKey != nKey)
		ppNode = &((*ppNode)->m_pNext);
	if (*ppNode == NULL)
		*ppNode = new CSingleHashNode(nKey, vData);
	else
		(*ppNode)->m_vData = vData;
}

CVector3D CSingleHashMap::Get(int nKey)
{
	int i = Hash(nKey);
	CSingleHashNode * pNode = m_pData[i];
	while (pNode != NULL && pNode->m_nKey != nKey)
		pNode = pNode->m_pNext;
	return (pNode == NULL ? CVector3D(1e300, 1e300, 1e300) : pNode->m_vData);
}

int CSingleHashMap::Hash(int nKey)
{
	return (int)(nKey % m_nNum);
}
