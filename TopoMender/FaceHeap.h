#pragma once

//////////////////////////////////////////////////////////////////////////
// Minimum Heap
//////////////////////////////////////////////////////////////////////////

class CFaceHeapElement
{
public:
	UCHAR * m_pNode[2];
	int m_nLevel[2];
	UINT64 m_n64Position[2];
	int m_dirFace;
	float m_fValue;

	CFaceHeapElement() {
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
					m_pNode[i] = NULL;
		m_dirFace = -1;
	}

	CFaceHeapElement(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, float fValue) {
		for (int i = 0; i < 2; i++)
		{
			m_pNode[i] = pNode[i];
			m_nLevel[i] = nLevel[i];
			m_n64Position[i] = n64Position[i];
		}
		m_dirFace = dirFace;
		m_fValue = fValue;
	}

	CFaceHeapElement(const CFaceHeapElement & cDup) {
		for (int i = 0; i < 2; i++)
		{
			m_pNode[i] = cDup.m_pNode[i];
			m_nLevel[i] = cDup.m_nLevel[i];
			m_n64Position[i] = cDup.m_n64Position[i];
		}
		m_dirFace = cDup.m_dirFace;
		m_fValue = cDup.m_fValue;
	}
};

class CFaceHeap
{
public:
	CFaceHeap(int nSize);
	~CFaceHeap(void);

public:
	int m_nSize;
	int m_nNum;
	CFaceHeapElement * m_pElement;

	bool Push(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, float fValue);
	CFaceHeapElement Pop();
	bool Empty();

	void DoubleSize();
};
