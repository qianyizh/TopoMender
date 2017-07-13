#include "OcTree.h"

//////////////////////////////////////////////////////////////////////////
// thin pure skeleton functions
//////////////////////////////////////////////////////////////////////////
void OcTree::ThinPureSkeleton()
{
	bool bContinue = true;
	m_listGeneratingSet.clear();

	while (bContinue) {
		bContinue = false;
		for (list<SkeletonElement>::iterator it = m_listSkeleton.begin(); it != m_listSkeleton.end(); it++)
		{
			if (it->m_pElement->m_bSkeleton) {
				m_listGeneratingSet.push_back(*it);
				while (m_listGeneratingSet.size() != 0) {
					SkeletonElement cTemp = *m_listGeneratingSet.begin();
					m_listGeneratingSet.pop_front();
					if (CheckSimpleElement(cTemp)) {
						bContinue = true;
						// fix this simple removal
						m_pCurrentSkeletonElement->m_pElement->m_bSkeleton = 0;
						m_pBelong->m_pElement->m_bSkeleton = 0;
						m_pCurrentSkeletonElement->m_pElement->m_cPointer = m_pBelong->m_cPointer;
						m_pBelong->m_pElement->m_cPointer = m_pBelong->m_cBackPointer;
						// push next element to m_listGeneratingSet
						m_nType = 38;
						switch (cTemp.m_nDimension) {
						case 0:
							CheckPoint(cTemp.m_n64Position);
							break;
						case 1:
							CheckEdge(cTemp.m_n64Position, cTemp.m_nLevel, cTemp.m_nDir);
							break;
						case 2:
							CheckFace(cTemp.m_n64Position, cTemp.m_nLevel, cTemp.m_nDir);
							break;
						case 3:
							CheckCell(cTemp.m_n64Position, cTemp.m_nLevel);
							break;
						}
						switch (m_pBelong->m_nDimension) {
						case 0:
							CheckPoint(m_pBelong->m_n64Position);
							break;
						case 1:
							CheckEdge(m_pBelong->m_n64Position, m_pBelong->m_nLevel, m_pBelong->m_nDir);
							break;
						case 2:
							CheckFace(m_pBelong->m_n64Position, m_pBelong->m_nLevel, m_pBelong->m_nDir);
							break;
						case 3:
							CheckCell(m_pBelong->m_n64Position, m_pBelong->m_nLevel);
							break;
						}
					}
				}
			}
		}
	}
}

void OcTree::ThinPureSkeletonElement()
{
	m_listGeneratingSet.clear();

	m_nType = 38;
	switch (m_pMinSkeletonElement->m_nDimension) {
	case 0:
		CheckPoint(m_pMinSkeletonElement->m_n64Position);
		break;
	case 1:
		CheckEdge(m_pMinSkeletonElement->m_n64Position, m_pMinSkeletonElement->m_nLevel, m_pMinSkeletonElement->m_nDir);
		break;
	case 2:
		CheckFace(m_pMinSkeletonElement->m_n64Position, m_pMinSkeletonElement->m_nLevel, m_pMinSkeletonElement->m_nDir);
		break;
	case 3:
		CheckCell(m_pMinSkeletonElement->m_n64Position, m_pMinSkeletonElement->m_nLevel);
		break;
	}

	//m_listGeneratingSet.push_back(*m_pMinSkeletonElement);
	while (m_listGeneratingSet.size() != 0) {
		SkeletonElement cTemp = *m_listGeneratingSet.begin();
		m_listGeneratingSet.pop_front();

		if (CheckSimpleElement(cTemp)) {
			// fix this simple removal
			m_pCurrentSkeletonElement->m_pElement->m_bSkeleton = 0;
			m_pBelong->m_pElement->m_bSkeleton = 0;
			m_pCurrentSkeletonElement->m_pElement->m_cPointer = m_pBelong->m_cPointer;
			m_pBelong->m_pElement->m_cPointer = m_pBelong->m_cBackPointer;
			// push next element to m_listGeneratingSet
			m_nType = 38;
			switch (m_pCurrentSkeletonElement->m_nDimension) {
			case 0:
				CheckPoint(m_pCurrentSkeletonElement->m_n64Position);
				break;
			case 1:
				CheckEdge(m_pCurrentSkeletonElement->m_n64Position, m_pCurrentSkeletonElement->m_nLevel, m_pCurrentSkeletonElement->m_nDir);
				break;
			case 2:
				CheckFace(m_pCurrentSkeletonElement->m_n64Position, m_pCurrentSkeletonElement->m_nLevel, m_pCurrentSkeletonElement->m_nDir);
				break;
			case 3:
				CheckCell(m_pCurrentSkeletonElement->m_n64Position, m_pCurrentSkeletonElement->m_nLevel);
				break;
			}
			switch (m_pBelong->m_nDimension) {
			case 0:
				CheckPoint(m_pBelong->m_n64Position);
				break;
			case 1:
				CheckEdge(m_pBelong->m_n64Position, m_pBelong->m_nLevel, m_pBelong->m_nDir);
				break;
			case 2:
				CheckFace(m_pBelong->m_n64Position, m_pBelong->m_nLevel, m_pBelong->m_nDir);
				break;
			case 3:
				CheckCell(m_pBelong->m_n64Position, m_pBelong->m_nLevel);
				break;
			}
		}
	}
}

void OcTree::DualThinPureSkeleton()
{
	bool bContinue = true;
	m_listGeneratingSet.clear();

	while (bContinue) {
		bContinue = false;
		for (list<SkeletonElement>::iterator it = m_listDualSkeleton.begin(); it != m_listDualSkeleton.end(); it++)
		{
			if (it->m_pElement->m_bSkeleton) {
				m_listGeneratingSet.push_back(*it);
				while (m_listGeneratingSet.size() != 0) {
					SkeletonElement cTemp = *m_listGeneratingSet.begin();
					m_listGeneratingSet.pop_front();
					if (DualCheckSimpleElement(cTemp)) {
						bContinue = true;
						// fix this simple removal
						m_pCurrentSkeletonElement->m_pElement->m_bSkeleton = 0;
						m_pBelong->m_pElement->m_bSkeleton = 0;
						m_pCurrentSkeletonElement->m_pElement->m_cPointer = m_pBelong->m_cBackPointer;
						m_pBelong->m_pElement->m_cPointer = m_pBelong->m_cPointer;
						// push next element to m_listGeneratingSet
						m_nType = 56;
						switch (cTemp.m_nDimension) {
						case 0:
							CheckPoint2(cTemp.m_n64Position);
							break;
						case 1:
							CheckEdge2(cTemp.m_n64Position, cTemp.m_nLevel, cTemp.m_nDir);
							break;
						case 2:
							CheckFace2(cTemp.m_n64Position, cTemp.m_nLevel, cTemp.m_nDir);
							break;
						case 3:
							CheckCell2(cTemp.m_n64Position, cTemp.m_nLevel);
							break;
						}
						switch (m_pBelong->m_nDimension) {
						case 0:
							CheckPoint2(m_pBelong->m_n64Position);
							break;
						case 1:
							CheckEdge2(m_pBelong->m_n64Position, m_pBelong->m_nLevel, m_pBelong->m_nDir);
							break;
						case 2:
							CheckFace2(m_pBelong->m_n64Position, m_pBelong->m_nLevel, m_pBelong->m_nDir);
							break;
						case 3:
							CheckCell2(m_pBelong->m_n64Position, m_pBelong->m_nLevel);
							break;
						}
					}
				}
			}
		}
	}
}

void OcTree::DualThinPureSkeletonElement()
{
	m_listGeneratingSet.clear();

	m_nType = 56;
	switch (m_pMinSkeletonElement->m_pDualElement->m_nDimension) {
	case 0:
		CheckPoint2(m_pMinSkeletonElement->m_pDualElement->m_n64Position);
		break;
	case 1:
		CheckEdge2(m_pMinSkeletonElement->m_pDualElement->m_n64Position, m_pMinSkeletonElement->m_pDualElement->m_nLevel, m_pMinSkeletonElement->m_pDualElement->m_nDir);
		break;
	case 2:
		CheckFace2(m_pMinSkeletonElement->m_pDualElement->m_n64Position, m_pMinSkeletonElement->m_pDualElement->m_nLevel, m_pMinSkeletonElement->m_pDualElement->m_nDir);
		break;
	case 3:
		CheckCell2(m_pMinSkeletonElement->m_pDualElement->m_n64Position, m_pMinSkeletonElement->m_pDualElement->m_nLevel);
		break;
	}

	//m_listGeneratingSet.push_back(*(m_pMinSkeletonElement->m_pDualElement));
	while (m_listGeneratingSet.size() != 0) {
		SkeletonElement cTemp = *m_listGeneratingSet.begin();
		m_listGeneratingSet.pop_front();
		if (DualCheckSimpleElement(cTemp)) {
			// fix this simple removal
			m_pCurrentSkeletonElement->m_pElement->m_bSkeleton = 0;
			m_pBelong->m_pElement->m_bSkeleton = 0;
			m_pCurrentSkeletonElement->m_pElement->m_cPointer = m_pBelong->m_cBackPointer;
			m_pBelong->m_pElement->m_cPointer = m_pBelong->m_cPointer;
			// push next element to m_listGeneratingSet
			m_nType = 56;
			switch (m_pCurrentSkeletonElement->m_nDimension) {
			case 0:
				CheckPoint2(m_pCurrentSkeletonElement->m_n64Position);
				break;
			case 1:
				CheckEdge2(m_pCurrentSkeletonElement->m_n64Position, m_pCurrentSkeletonElement->m_nLevel, m_pCurrentSkeletonElement->m_nDir);
				break;
			case 2:
				CheckFace2(m_pCurrentSkeletonElement->m_n64Position, m_pCurrentSkeletonElement->m_nLevel, m_pCurrentSkeletonElement->m_nDir);
				break;
			case 3:
				CheckCell2(m_pCurrentSkeletonElement->m_n64Position, m_pCurrentSkeletonElement->m_nLevel);
				break;
			}
			switch (m_pBelong->m_nDimension) {
			case 0:
				CheckPoint2(m_pBelong->m_n64Position);
				break;
			case 1:
				CheckEdge2(m_pBelong->m_n64Position, m_pBelong->m_nLevel, m_pBelong->m_nDir);
				break;
			case 2:
				CheckFace2(m_pBelong->m_n64Position, m_pBelong->m_nLevel, m_pBelong->m_nDir);
				break;
			case 3:
				CheckCell2(m_pBelong->m_n64Position, m_pBelong->m_nLevel);
				break;
			}
		}
	}
}

bool OcTree::CheckSimpleElement(SkeletonElement & cElement)
{
	m_pCurrentSkeletonElement = & cElement;
	m_nBelong = 0;
	m_nType = 55;

	switch (cElement.m_nDimension) {
	case 0:
		CheckPoint2(cElement.m_n64Position);
		break;
	case 1:
		CheckEdge2(cElement.m_n64Position, cElement.m_nLevel, cElement.m_nDir);
		break;
	case 2:
		CheckFace2(cElement.m_n64Position, cElement.m_nLevel, cElement.m_nDir);
		break;
	case 3:
		CheckCell2(cElement.m_n64Position, cElement.m_nLevel);
		break;
	}

	return (m_nBelong == 1);
}

bool OcTree::DualCheckSimpleElement(SkeletonElement & cDualElement)
{
	if (cDualElement.m_nLevel == -1)
		return false;

	m_pCurrentSkeletonElement = & cDualElement;
	m_nBelong = 0;
	m_nType = 39;

	switch (cDualElement.m_nDimension) {
	case 0:
		CheckPoint(cDualElement.m_n64Position);
		break;
	case 1:
		CheckEdge(cDualElement.m_n64Position, cDualElement.m_nLevel, cDualElement.m_nDir);
		break;
	case 2:
		CheckFace(cDualElement.m_n64Position, cDualElement.m_nLevel, cDualElement.m_nDir);
		break;
	case 3:
		CheckCell(cDualElement.m_n64Position, cDualElement.m_nLevel);
		break;
	}

	return (m_nBelong == 1);
}
