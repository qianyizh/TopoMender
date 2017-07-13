#pragma once

#define HEAP_UNIT	0x100000

class CMemoryManager
{
public:
	CMemoryManager(void);
	~CMemoryManager(void);

	UCHAR ** m_ppData;
	int m_nSize;
	int m_nNum;
	int m_nCurrent;
	int m_nDeleted;

public:
	void NewBlock()
	{
		m_nSize += HEAP_UNIT;
		m_nNum ++;
		m_ppData = ( UCHAR ** )realloc( m_ppData, sizeof( UCHAR * ) * m_nNum );
		m_ppData[ m_nNum - 1 ] = ( UCHAR * )malloc(HEAP_UNIT);
		m_nCurrent = 0;
	}

	UCHAR * New ( int nLength )
	{
		if (m_nCurrent + nLength > HEAP_UNIT) {
			NewBlock();
		}

		UCHAR * pRtn = m_ppData[m_nNum - 1] + m_nCurrent;
		m_nCurrent += nLength;

		return pRtn;
	}

	bool InBlock( UCHAR * pPointer, int nBlockIndex )
	{
		return (pPointer >= m_ppData[ nBlockIndex ] && (int)(pPointer - m_ppData[ nBlockIndex ]) < HEAP_UNIT);
	}

	void Delete ( UCHAR * pPointer )
	{
		if (!InBlock(pPointer, m_nDeleted)) {
			delete[] m_ppData[ m_nDeleted ] ;
			m_nDeleted++;
		}
	}

	void PrintInfo()
	{
		printf("Allocate memory (block size * block number) : %d * %d = %d\n", HEAP_UNIT, m_nNum, HEAP_UNIT * m_nNum);
	}
};
