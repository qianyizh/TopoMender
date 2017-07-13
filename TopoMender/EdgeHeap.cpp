#include "stdafx.h"
#include ".\edgeheap.h"

CEdgeHeap::CEdgeHeap(int nSize)
{
	m_nSize = nSize;
	m_pElement = new CEdgeHeapElement[m_nSize];
	m_nNum = 0;
}

CEdgeHeap::~CEdgeHeap(void)
{
	delete []m_pElement;
}

void CEdgeHeap::DoubleSize()
{
	CEdgeHeapElement * pElement = m_pElement;
	m_pElement = new CEdgeHeapElement[m_nSize * 2];
	for (int i = 0; i < m_nSize; i++)
	{
		m_pElement[i] = pElement[i];
	}

	m_nSize *= 2;
	delete []pElement;
}

bool CEdgeHeap::Push(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, float fValue)
{
	if (m_nNum == m_nSize) {
		DoubleSize();
	}

	int iIndex = m_nNum;
	m_nNum++;

	while (iIndex != 0) {
		int iFather = (iIndex + 1) / 2 - 1;
		if (fValue > m_pElement[iFather].m_fValue) {
			m_pElement[iIndex] = m_pElement[iFather];
			iIndex = iFather;
		} else {
			break;
		}
	}
	m_pElement[iIndex] = CEdgeHeapElement(pNode, n64Position, nLevel, dirEdge, fValue);

	return true;
}

CEdgeHeapElement CEdgeHeap::Pop()
{
	CEdgeHeapElement cRtn;
	if (m_nNum == 0) {
		cRtn.m_dirEdge = -1;
		return cRtn;
	}
	cRtn = m_pElement[0];

	int iIndex = 0;
	double fValue = m_pElement[m_nNum - 1].m_fValue;
	CEdgeHeapElement cElement = m_pElement[m_nNum - 1];
	m_nNum--;

	while (true) {
		int iChildLeft = (iIndex + 1) * 2 - 1;
		int iChildRight = iChildLeft + 1;
		if (iChildLeft >= m_nNum) {
			break;
		} else if (iChildRight >= m_nNum) {
			if (fValue < m_pElement[iChildLeft].m_fValue) {
				m_pElement[iIndex] = m_pElement[iChildLeft];
				iIndex = iChildLeft;
			} else {
				break;
			}
		} else {
			if (m_pElement[iChildLeft].m_fValue >= m_pElement[iChildRight].m_fValue) {
				if (fValue < m_pElement[iChildLeft].m_fValue) {
					m_pElement[iIndex] = m_pElement[iChildLeft];
					iIndex = iChildLeft;
				} else {
					break;
				}
			} else {
				if (fValue < m_pElement[iChildRight].m_fValue) {
					m_pElement[iIndex] = m_pElement[iChildRight];
					iIndex = iChildRight;
				} else {
					break;
				}
			}
		}
	}

	m_pElement[iIndex] = cElement;

	return cRtn;
}

bool CEdgeHeap::Empty()
{
	return (m_nNum == 0);
}
