#include "MemoryManager.h"

//MemoryManager

MemoryManager::MemoryManager()
{
	m_nSize = HEAP_UNIT;
	m_nNum = 1;
	m_nCurrent = 0;
	m_ppData = ( UCHAR ** )malloc( sizeof( UCHAR * ) ) ;
	m_ppData[ 0 ] = ( UCHAR * )malloc( HEAP_UNIT ) ;
	m_nDeleted = 0;
}

MemoryManager::~MemoryManager()
{
	Clear();
}

void MemoryManager::NewBlock()
{
	m_nSize += HEAP_UNIT;
	m_nNum ++;
	m_ppData = ( UCHAR ** )realloc( m_ppData, sizeof( UCHAR * ) * m_nNum );
	m_ppData[ m_nNum - 1 ] = ( UCHAR * )malloc(HEAP_UNIT);
	m_nCurrent = 0;
}

UCHAR * MemoryManager::New( int nLength )
{
	if (m_nCurrent + nLength > HEAP_UNIT) {
		NewBlock();
	}
	UCHAR * pRtn = m_ppData[m_nNum - 1] + m_nCurrent;
	m_nCurrent += nLength;
	return pRtn;
}

bool MemoryManager::InBlock( UCHAR * pPointer, int nBlockIndex )
{
	return (pPointer >= m_ppData[ nBlockIndex ] && (int)(pPointer - m_ppData[ nBlockIndex ]) < HEAP_UNIT);
}

void MemoryManager::Delete( UCHAR * pPointer )
{
	if (!InBlock(pPointer, m_nDeleted)) {
		delete[] m_ppData[ m_nDeleted ] ;
		m_nDeleted++;
	}
}

void MemoryManager::PrintInfo()
{
	printf("Allocatememory(blocksize*blocknumber):%d*%d=%d\n", 
		HEAP_UNIT, m_nNum, HEAP_UNIT * m_nNum);
}

void MemoryManager::Clear()
{
	if ( m_ppData != NULL ) {
		for ( int i = m_nDeleted ; i < m_nNum ; i ++ )
		{
			delete[] m_ppData[ i ] ;
		}
		delete[] m_ppData ;
		m_ppData = NULL;
	}
}
