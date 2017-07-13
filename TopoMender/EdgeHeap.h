#pragma once

//////////////////////////////////////////////////////////////////////////
// Minimum Heap
//////////////////////////////////////////////////////////////////////////

class CEdgeHeapElement
{
public:
	UCHAR * m_pNode[2][2];
	int m_nLevel[2][2];
	UINT64 m_n64Position[2][2];
	int m_dirEdge;
	float m_fValue;

	CEdgeHeapElement() {
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
					m_pNode[i][j] = NULL;
		m_dirEdge = -1;
	}

	CEdgeHeapElement(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, float fValue) {
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
			{
				m_pNode[i][j] = pNode[i][j];
				m_nLevel[i][j] = nLevel[i][j];
				m_n64Position[i][j] = n64Position[i][j];
			}
		m_dirEdge = dirEdge;
		m_fValue = fValue;
	}

	CEdgeHeapElement(const CEdgeHeapElement & cDup) {
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
			{
				m_pNode[i][j] = cDup.m_pNode[i][j];
				m_nLevel[i][j] = cDup.m_nLevel[i][j];
				m_n64Position[i][j] = cDup.m_n64Position[i][j];
			}
		m_dirEdge = cDup.m_dirEdge;
		m_fValue = cDup.m_fValue;
	}
};

class CEdgeHeap
{
public:
	CEdgeHeap(int nSize);
	~CEdgeHeap(void);

public:
	int m_nSize;
	int m_nNum;
	CEdgeHeapElement * m_pElement;

	bool Push(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, float fValue);
	CEdgeHeapElement Pop();
	bool Empty();

	void DoubleSize();
};
