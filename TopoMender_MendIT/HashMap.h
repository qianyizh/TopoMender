#ifndef _HASHMAP
#define _HASHMAP

#include "resources.h"
#include "Constant.h"

class HashNode {
public:
	UINT64 m_nKey[2];
	int m_nIndex;
	HashNode * m_pNext;
public:
	HashNode(UINT64 nKey0, UINT64 nKey1, int nIndex);
	~HashNode();
};

class HashMap {
public:
	Constant * m_pConstant;
	HashNode ** m_pData;
	int m_nNum;
	UINT64 m_nHashKey;
public:
	HashMap(Constant * pConstant, int nSize);
	~HashMap();
	void Set(UINT64 nKey0, UINT64 nKey1, int nIndex);
	int Get(UINT64 nKey0, UINT64 nKey1);
	int Hash(UINT64 nKey0, UINT64 nKey1);
};

#endif
