#pragma once

#include "Constant.h"
#include "Vector3D.h"

class CSingleHashNode {
public:
	int m_nKey;
	CVector3D m_vData;
	CSingleHashNode * m_pNext;

	CSingleHashNode(int nKey, const CVector3D & vData)
	{
		m_nKey = nKey;
		m_vData = vData;
		m_pNext = NULL;
	}

	~CSingleHashNode()
	{
		if (m_pNext != NULL)
			delete m_pNext;
	}
};

class CSingleHashMap
{
public:
	CSingleHashMap(CConstant * pConstant, int nSize);
	~CSingleHashMap(void);

public:
	CConstant * m_pConstant;
	CSingleHashNode ** m_pData;
	int m_nNum;

public:
	void Set(int nKey, const CVector3D & vData);
	CVector3D Get(int nKey);

private:
	int Hash(int nKey);
};
