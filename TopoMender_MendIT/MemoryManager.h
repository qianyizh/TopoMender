#ifndef _MEMORYMANAGER
#define _MEMORYMANAGER

#include "resources.h"

class MemoryManager {
public:
	UCHAR ** m_ppData;
	int m_nSize;
	int m_nNum;
	int m_nCurrent;
	int m_nDeleted;
public:
	MemoryManager();
	~MemoryManager();
	void NewBlock();
	UCHAR * New(int nLength);
	bool InBlock(UCHAR * pPointer, int nBlockIndex);
	void Delete(UCHAR * pPointer);
	void PrintInfo();
	void Clear();
};

#endif
