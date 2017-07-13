#pragma once

#include "Constant.h"

class CHashNode {
public:
	UINT64 m_nKey[2];
	int m_nIndex;
	CHashNode * m_pNext;

	CHashNode(UINT64 nKey0, UINT64 nKey1, int nIndex)
	{
		m_nKey[0] = nKey0;
		m_nKey[1] = nKey1;
		m_nIndex = nIndex;
		m_pNext = NULL;
	}

	~CHashNode()
	{
		if (m_pNext != NULL)
			delete m_pNext;
	}
};

class CHashMap
{
public:
	CHashMap(CConstant * pConstant, int nSize);
	~CHashMap(void);

public:
	CConstant * m_pConstant;
	CHashNode ** m_pData;
	int m_nNum;

public:
	void Set(UINT64 nKey0, UINT64 nKey1, int nIndex);
	int Get(UINT64 nKey0, UINT64 nKey1);

private:
	int Hash(UINT64 nKey0, UINT64 nKey1);
	UINT64 m_nHashKey;
};
