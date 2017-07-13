#include "StdAfx.h"
#include ".\memorymanager.h"
#using <mscorlib.dll>

CMemoryManager::CMemoryManager(void)
{
	m_nSize = HEAP_UNIT;
	m_nNum = 1;
	m_nCurrent = 0;
	m_ppData = ( UCHAR ** )malloc( sizeof( UCHAR * ) ) ;
	m_ppData[ 0 ] = ( UCHAR * )malloc( HEAP_UNIT ) ;
	m_nDeleted = 0;
}

CMemoryManager::~CMemoryManager(void)
{
	for ( int i = m_nDeleted ; i < m_nNum ; i ++ )
	{
		delete[] m_ppData[ i ] ;
	}
	delete[] m_ppData ;
}
