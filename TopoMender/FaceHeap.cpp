#include "stdafx.h"
#include ".\faceheap.h"

CFaceHeap::CFaceHeap(int nSize)
{
	m_nSize = nSize;
	m_pElement = new CFaceHeapElement[m_nSize];
	m_nNum = 0;
}

CFaceHeap::~CFaceHeap(void)
{
	delete []m_pElement;
}

void CFaceHeap::DoubleSize()
{
	CFaceHeapElement * pElement = m_pElement;
	m_pElement = new CFaceHeapElement[m_nSize * 2];
	for (int i = 0; i < m_nSize; i++)
	{
		m_pElement[i] = pElement[i];
	}

	m_nSize *= 2;
	delete []pElement;
}

bool CFaceHeap::Push(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, float fValue)
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
	m_pElement[iIndex] = CFaceHeapElement(pNode, n64Position, nLevel, dirFace, fValue);

	return true;
}

CFaceHeapElement CFaceHeap::Pop()
{
	CFaceHeapElement cRtn;
	if (m_nNum == 0) {
		cRtn.m_dirFace = -1;
		return cRtn;
	}
	cRtn = m_pElement[0];

	int iIndex = 0;
	double fValue = m_pElement[m_nNum - 1].m_fValue;
	CFaceHeapElement cElement = m_pElement[m_nNum - 1];
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

bool CFaceHeap::Empty()
{
	return (m_nNum == 0);
}
