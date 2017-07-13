#ifndef _SINGLEHASHMAP
#define _SINGLEHASHMAP

#include "Constant.h"
#include "Vector3D.h"

class SingleHashNode {
public:
	UINT64 m_nKey;
	CVector3D m_vData;
	SingleHashNode * m_pNext;
public:
	SingleHashNode(UINT64 nKey, const CVector3D & vData);
	~SingleHashNode();
};

class SingleHashMap {
public:
	Constant * m_pConstant;
	SingleHashNode ** m_pData;
	int m_nNum;
public:
	SingleHashMap(Constant * pConstant, int nSize);
	~SingleHashMap();
	void Set(UINT64 nKey, const CVector3D & vData);
	CVector3D Get(UINT64 nKey);
	int Hash(UINT64 nKey);
};

#endif