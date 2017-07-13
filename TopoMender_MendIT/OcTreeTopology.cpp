#include "OcTree.h"

//////////////////////////////////////////////////////////////////////////
// topology repair functions
//////////////////////////////////////////////////////////////////////////
void OcTree::TopologyRepair()
{
	//m_bCleanSkeleton = true;
	//Thin();
	//DualThin();
	//PrintSkeletonInfo();
	m_listSkeleton.clear();
	m_listDualSkeleton.clear();
	m_nType = 6;
	Traversal();
	BuildSkeletonHash();
	bool bContinue = false;
	for (list<SkeletonElement>::iterator it = m_listSkeleton.begin(); it != m_listSkeleton.end(); it++) {
		if (it->m_pElement->m_bSkeleton) {
			bContinue = true;
			break;
		}
	}
	PrintPureSkeletonInfo();
	int nMethod;
	TimeMeter timerMain;

	timerMain.Start();

	while ( bContinue ) {
		m_bComputeOnly = true;

		// compute background generating set
#ifdef CONSOLE_OUTPUT
		TimeMeter timer[3];
		int nCount = 0;
		printf("Compute generating set for background skeleton elements :%7d", nCount);
		timer[0].Start();
#endif
		for ( list<SkeletonElement>::iterator it = m_listDualSkeleton.begin(); it != m_listDualSkeleton.end(); it++ )
		{
			if ( it->m_pElement->m_bSkeleton && !it->m_bValid ) {
				it->m_n64Size = 0;
				it->m_pDualElement = NULL;
				m_pCurrentSkeletonElement = &(*it);
				if (DualCheckSkeletonElemement(*it)) {
					DualComputeGeneratingSet(*it);
					it->m_bValid = true;
					//printf("%16I64x\n", m_pCurrentSkeletonElement->m_n64Size);
#ifdef CONSOLE_OUTPUT
					nCount ++;
					printf("\b\b\b\b\b\b\b%7d", nCount);
#endif
				}
			}
		}

#ifdef CONSOLE_OUTPUT
		timer[0].End();
		nCount = 0;
		printf("\nCompute generating set for object skeleton elements :%7d", nCount);
		timer[1].Start();
#endif
		// compute object generating set
		for ( list<SkeletonElement>::iterator it = m_listSkeleton.begin(); it != m_listSkeleton.end(); it++ )
		{
			if ( it->m_pElement->m_bSkeleton && !it->m_bValid ) {
				it->m_n64Size = 0;
				it->m_pDualElement = NULL;
				m_pCurrentSkeletonElement = &(*it);
				if (CheckSkeletonElemement(*it)) {
					ComputeGeneratingSet(*it);
					//printf("%16I64x\n", m_pCurrentSkeletonElement->m_n64Size);
#ifdef CONSOLE_OUTPUT
					nCount ++;
					printf("\b\b\b\b\b\b\b%7d", nCount);
#endif
				}
			}
		}

#ifdef CONSOLE_OUTPUT
		timer[1].End();
		nCount = 0;
		printf("\nCompute compatible elements for object skeleton elements :%7d", nCount);
		timer[2].Start();
#endif
		// compute complement elements
		m_pMinSkeletonElement = NULL;
		for ( list<SkeletonElement>::iterator it = m_listSkeleton.begin(); it != m_listSkeleton.end(); it++ )
		{
			if ( it->m_pElement->m_bSkeleton ) {
				if (m_pMinSkeletonElement == NULL) {
					m_pMinSkeletonElement = &(*it);
				}
				m_pCurrentSkeletonElement = &(*it);
				m_pCurrentSkeletonElement->m_pDualElement = NULL;
				if (CheckSkeletonElemement(*it)) {
					UINT64 n64MinSize = 0x00ffffffffffffff;
					if ( !it->m_bValid ) {
						ComputeVastGeneratingSet(*it);
						it->m_bValid = true;
						//for (list<SkeletonElement *>::iterator itt = it->m_listCompatible.begin(); itt != it->m_listCompatible.end(); itt++) {
						//	SkeletonElement * pTemp = *itt;
						//	hash_map<unsigned int, unsigned int>::iterator thisit;
						//	thisit = m_hashElementExtend.find( (unsigned int)pTemp->m_pElement );
						//	if ( thisit->second != 0 ) {
						//		list<SkeletonElement *>::iterator ittTemp = itt;
						//		itt--;
						//		it->m_listCompatible.erase(ittTemp);
						//	} else if ( thisit->second == 0 && pTemp->m_n64Size < n64MinSize ) {
						//		n64MinSize = pTemp->m_n64Size;
						//		m_pCurrentSkeletonElement->m_pDualElement = pTemp;
						//	}
						//}
						list<SkeletonElement *>::iterator itt = it->m_listCompatible.begin();
						while ( itt != it->m_listCompatible.end() ) {
							SkeletonElement * pTemp = *itt;
							list<SkeletonElement *>::iterator ittTemp = itt;
							itt++;
							hash_map<unsigned int, unsigned int>::iterator thisit;
							thisit = m_hashElementExtend.find( (unsigned int)pTemp->m_pElement );
							if ( thisit->second != 0 ) {
								it->m_listCompatible.erase(ittTemp);
							} else if ( thisit->second == 0 && pTemp->m_n64Size < n64MinSize ) {
								n64MinSize = pTemp->m_n64Size;
								m_pCurrentSkeletonElement->m_pDualElement = pTemp;
							}
						}
					} else {
						for (list<SkeletonElement *>::iterator itt = it->m_listCompatible.begin(); itt != it->m_listCompatible.end(); itt++) {
							SkeletonElement * pTemp = *itt;
							if ( pTemp->m_pElement->m_bSkeleton && !pTemp->m_pElement->m_bObject && pTemp->m_n64Size < n64MinSize ) {
								n64MinSize = pTemp->m_n64Size;
								m_pCurrentSkeletonElement->m_pDualElement = pTemp;
							}

						}
					}
					// printf("%16I64x\n", n64MinSize);
					if ( GetObjectSkeletonElementSize(m_pCurrentSkeletonElement) < GetObjectSkeletonElementSize(m_pMinSkeletonElement) ) {
						m_pMinSkeletonElement = m_pCurrentSkeletonElement;
					}
#ifdef CONSOLE_OUTPUT
					nCount ++;
					printf("\b\b\b\b\b\b\b%7d", nCount);
#endif
				}
			}
		}

		// fix it
#ifdef CONSOLE_OUTPUT
		timer[2].End();
		printf("\nSmallest generating set with size : %16I64x, at %16I64x\n", GetObjectSkeletonElementSize(m_pMinSkeletonElement), m_pMinSkeletonElement->m_n64Position);
		printf("Dimension is %d\n", m_pMinSkeletonElement->m_nDimension);
#endif
		m_bComputeOnly = false;
		m_listChanging.clear();
		if (m_pMinSkeletonElement->m_pDualElement == NULL) {
			nMethod = 0;
#ifdef CONSOLE_OUTPUT
			printf("Use method %d, use time ", nMethod);
			timer[0].Print(); printf(", ");
			timer[1].Print(); printf(", ");
			timer[2].Print(); printf("\n");
#endif
			m_listChanging.push_back(*m_pMinSkeletonElement);
			ModifySkeletonComputeGeneratingSet(*m_pMinSkeletonElement);			
			for (list<SkeletonElement>::iterator it = m_listChanging.begin(); it != m_listChanging.end(); it++) {
				(*it).m_pElement->m_bObject = 0;
				(*it).m_pElement->m_bSkeleton = 1;
			}
			m_listSkeleton.clear();
			m_listDualSkeleton.clear();
			m_nType = 6;
			Traversal();
			BuildSkeletonHash();
			PrintPureSkeletonInfo();
			ThinPureSkeleton();
			DualThinPureSkeleton();
		} else if ( m_pMinSkeletonElement->m_n64Size < m_pMinSkeletonElement->m_pDualElement->m_n64Size ) {
			nMethod = 1;
#ifdef CONSOLE_OUTPUT
			printf("Use method %d, use time ", nMethod);
			timer[0].Print(); printf(", ");
			timer[1].Print(); printf(", ");
			timer[2].Print(); printf("\n");
#endif
			m_listChanging.push_back(*m_pMinSkeletonElement);
			ComputeGeneratingSet(*m_pMinSkeletonElement);
			// set invalid
			SetVastInvalidGeneratingSet(*m_pMinSkeletonElement);
			SetInvalidGeneratingSet(*m_pMinSkeletonElement);
			// reverse the path
			m_pFindElement = m_pMinSkeletonElement->m_pDualElement->m_pElement;
			m_pCurrentSkeletonElement = m_pMinSkeletonElement;
			ReversePathComputeGeneratingSet(*m_pMinSkeletonElement);
			for (list<SkeletonElement>::iterator it = m_listChanging.begin(); it != m_listChanging.end(); it++) {
				(*it).m_pElement->m_bObject = 0;
			}
			m_pMinSkeletonElement->m_pElement->m_bSkeleton = 0;
			m_pMinSkeletonElement->m_pDualElement->m_pElement->m_bSkeleton = 0;
			ThinPureSkeletonElement();
			DualThinPureSkeletonElement();
		} else {
			nMethod = 2;
#ifdef CONSOLE_OUTPUT
			printf("Use method %d, use time ", nMethod);
			timer[0].Print(); printf(", ");
			timer[1].Print(); printf(", ");
			timer[2].Print(); printf("\n");
#endif
			m_listChanging.push_back(*(m_pMinSkeletonElement->m_pDualElement));
			DualComputeGeneratingSet(*(m_pMinSkeletonElement->m_pDualElement));
			// set invalid
			DualSetVastInvalidGeneratingSet(*(m_pMinSkeletonElement->m_pDualElement));
			DualSetInvalidGeneratingSet(*(m_pMinSkeletonElement->m_pDualElement));
			// reverse the path
			m_pFindElement = m_pMinSkeletonElement->m_pDualElement->m_pElement;
			m_pCurrentSkeletonElement = m_pMinSkeletonElement;
			ReversePathComputeGeneratingSet(*m_pMinSkeletonElement);
			for (list<SkeletonElement>::iterator it = m_listChanging.begin(); it != m_listChanging.end(); it++) {
				(*it).m_pElement->m_bObject = 1;
			}
			m_pMinSkeletonElement->m_pElement->m_bSkeleton = 0;
			m_pMinSkeletonElement->m_pDualElement->m_pElement->m_bSkeleton = 0;
			ThinPureSkeletonElement();
			DualThinPureSkeletonElement();
		}

#ifdef CONSOLE_OUTPUT
		printf("------------------------------------------------------\n");
#endif
		// Thin();
		// DualThin();
		//PrintSkeletonInfo();
		// m_listSkeleton.remove_if( is_removed_skeleton() );
		// m_listDualSkeleton.remove_if( is_removed_dual_skeleton() );
		bContinue = false;
		for (list<SkeletonElement>::iterator it = m_listSkeleton.begin(); it != m_listSkeleton.end(); it++) {
			if (it->m_pElement->m_bSkeleton) {
				bContinue = true;
				break;
			}
		}
		PrintPureSkeletonInfo();
		// return;
	}
	timerMain.End();
#ifdef CONSOLE_OUTPUT
	printf("Topology repair finished!\n");
	printf("Total time : ");
	timerMain.Print();
	printf("\n");
#endif
}

void OcTree::BuildSkeletonHash()
{
	m_hashSkeleton.clear();
	m_hashDualSkeleton.clear();

	for ( list<SkeletonElement>::iterator it = m_listSkeleton.begin(); it != m_listSkeleton.end(); it++ )
	{
		m_hashSkeleton.insert(pair<unsigned int, SkeletonElement *>((unsigned int)it->m_pElement, &(*it)));
	}
	for ( list<SkeletonElement>::iterator it = m_listDualSkeleton.begin(); it != m_listDualSkeleton.end(); it++ )
	{
		m_hashDualSkeleton.insert(pair<unsigned int, SkeletonElement *>((unsigned int)it->m_pElement, &(*it)));
	}
}

bool OcTree::CheckSkeletonElemement(SkeletonElement & cElement)
{
	m_bIsSeperating = true;
	m_nType = 48;
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
	return m_bIsSeperating;
}

bool OcTree::DualCheckSkeletonElemement(SkeletonElement & cDualElement)
{
	m_bIsSeperating = true;
	m_nType = 33;
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
	return m_bIsSeperating;
}

void OcTree::DualComputeGeneratingSet(SkeletonElement & cDualElement)
{
	if (cDualElement.m_nLevel == -1) {
		cDualElement.m_n64Size = INFINITE_ELEMENT_VOLUME;
		return;
	}

	m_hashElement.clear();
	m_listGeneratingSet.clear();
	m_listGeneratingSet.push_back(cDualElement);
	m_hashElement.insert(((unsigned int)cDualElement.m_pElement));
	if ( !m_bComputeOnly ) {
		m_listChanging.push_back(cDualElement);
	} else {
		switch (cDualElement.m_nDimension) {
		case 0:
			m_pCurrentSkeletonElement->m_n64Size += GetPointVolume();
			break;
		case 1:
			m_pCurrentSkeletonElement->m_n64Size += GetEdgeVolume(cDualElement.m_nLevel);
			break;
		case 2:
			m_pCurrentSkeletonElement->m_n64Size += GetFaceVolume(cDualElement.m_nLevel);
			break;
		case 3:
			m_pCurrentSkeletonElement->m_n64Size += GetCellVolume(cDualElement.m_nLevel);
			break;
		}
	}
	m_nType = 34;

	while (m_listGeneratingSet.size() != 0) {
		//if ( m_bComputeOnly ) {
		//	if (m_pCurrentSkeletonElement->m_n64Size >= UPPER_LIMIT) {
		//		m_listGeneratingSet.clear();
		//		return;
		//	}
		//}
		SkeletonElement cTemp = *m_listGeneratingSet.begin();
		m_listGeneratingSet.pop_front();
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
	}
}

void OcTree::ComputeGeneratingSet(SkeletonElement & cElement)
{
	m_hashElement.clear();
	m_listGeneratingSet.clear();
	m_listGeneratingSet.push_back(cElement);
	m_hashElement.insert(((unsigned int)cElement.m_pElement));
	if ( !m_bComputeOnly ) {
		m_listChanging.push_back(cElement);
	} else {
		switch (cElement.m_nDimension) {
		case 0:
			m_pCurrentSkeletonElement->m_n64Size += GetPointVolume();
			break;
		case 1:
			m_pCurrentSkeletonElement->m_n64Size += GetEdgeVolume(cElement.m_nLevel);
			break;
		case 2:
			m_pCurrentSkeletonElement->m_n64Size += GetFaceVolume(cElement.m_nLevel);
			break;
		case 3:
			m_pCurrentSkeletonElement->m_n64Size += GetCellVolume(cElement.m_nLevel);
			break;
		}
	}
	m_nType = 49;

	while (m_listGeneratingSet.size() != 0) {
		SkeletonElement cTemp = *m_listGeneratingSet.begin();
		m_listGeneratingSet.pop_front();
		switch (cTemp.m_nDimension) {
		case 0:
			// printf("----- pop point (%16I64x)  -----\n", cTemp.m_n64Position);
			CheckPoint2(cTemp.m_n64Position);
			break;
		case 1:
			// printf("----- pop edge (%16I64x, %d, %d)  -----\n", cTemp.m_n64Position, cTemp.m_nLevel, cTemp.m_nDir);
			CheckEdge2(cTemp.m_n64Position, cTemp.m_nLevel, cTemp.m_nDir);
			break;
		case 2:
			// printf("----- pop face (%16I64x, %d, %d)  -----\n", cTemp.m_n64Position, cTemp.m_nLevel, cTemp.m_nDir);
			CheckFace2(cTemp.m_n64Position, cTemp.m_nLevel, cTemp.m_nDir);
			break;
		case 3:
			// printf("----- pop cell (%16I64x, %d)  -----\n", cTemp.m_n64Position, cTemp.m_nLevel);
			CheckCell2(cTemp.m_n64Position, cTemp.m_nLevel);
			break;
		}
	}
}

void OcTree::ComputeVastGeneratingSet(SkeletonElement & cElement)
{
	m_pCurrentSkeletonElement->m_listCompatible.clear();
	m_hashElementExtend.clear();
	m_listGeneratingSet.clear();
	m_listGeneratingSet.push_back(cElement);
	m_hashElementExtend.insert(pair<unsigned, unsigned>((unsigned int)cElement.m_pElement, 0));
	m_nType = 50;

	while (m_listGeneratingSet.size() != 0) {
		SkeletonElement cTemp = *m_listGeneratingSet.begin();
		m_listGeneratingSet.pop_front();
		hash_map<unsigned int, unsigned int>::iterator it = m_hashElementExtend.find((unsigned int)cTemp.m_pElement);
		m_nFatherStatus = it->second;
		if (it->second == 0)
			it->second = 2;

		switch (cTemp.m_nDimension) {
		case 0:
			// printf("----- pop point (%16I64x)  -----\n", cTemp.m_n64Position);
			CheckPoint2(cTemp.m_n64Position);
			break;
		case 1:
			// printf("----- pop edge (%16I64x, %d, %d)  -----\n", cTemp.m_n64Position, cTemp.m_nLevel, cTemp.m_nDir);
			CheckEdge2(cTemp.m_n64Position, cTemp.m_nLevel, cTemp.m_nDir);
			break;
		case 2:
			// printf("----- pop face (%16I64x, %d, %d)  -----\n", cTemp.m_n64Position, cTemp.m_nLevel, cTemp.m_nDir);
			CheckFace2(cTemp.m_n64Position, cTemp.m_nLevel, cTemp.m_nDir);
			break;
		case 3:
			// printf("----- pop cell (%16I64x, %d)  -----\n", cTemp.m_n64Position, cTemp.m_nLevel);
			CheckCell2(cTemp.m_n64Position, cTemp.m_nLevel);
			break;
		}
	}
}

void OcTree::SetInvalidGeneratingSet(SkeletonElement & cElement)
{
	m_hashElement.clear();

	for (list<SkeletonElement>::iterator it = m_listChanging.begin(); it != m_listChanging.end(); it++) {
		pair< hash_set<unsigned int>::iterator, bool > pr;
		pr = m_hashElement.insert( (unsigned int)it->m_pElement );
		if ( pr.second ) {
			m_listGeneratingSet.clear();
			m_listGeneratingSet.push_back(*it);
			m_nType = 37;

			while (m_listGeneratingSet.size() != 0) {
				SkeletonElement cTemp = *m_listGeneratingSet.begin();
				m_listGeneratingSet.pop_front();

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
			}
		}
	}
}

void OcTree::DualSetInvalidGeneratingSet(SkeletonElement & cDualElement)
{
	m_hashElement.clear();

	for (list<SkeletonElement>::iterator it = m_listChanging.begin(); it != m_listChanging.end(); it++) {
		pair< hash_set<unsigned int>::iterator, bool > pr;
		pr = m_hashElement.insert( (unsigned int)it->m_pElement );
		if ( pr.second ) {
			m_listGeneratingSet.clear();
			m_listGeneratingSet.push_back(*it);
			m_nType = 37;

			while (m_listGeneratingSet.size() != 0) {
				SkeletonElement cTemp = *m_listGeneratingSet.begin();
				m_listGeneratingSet.pop_front();

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
			}
		}
	}
}

void OcTree::ModifySkeletonComputeGeneratingSet(SkeletonElement & cElement)
{
	m_hashElement.clear();
	m_listGeneratingSet.clear();
	m_listGeneratingSet.push_back(cElement);
	m_hashElement.insert(((unsigned int)cElement.m_pElement));
	m_listChanging.push_back(cElement);
	m_nType = 51;
	//printf("Type 51 from Face : (%16I64x, %d, %d)\n", cElement.m_n64Position, cElement.m_nLevel, cElement.m_nDir);

	while (m_listGeneratingSet.size() != 0) {
		SkeletonElement cTemp = *m_listGeneratingSet.begin();
		m_listGeneratingSet.pop_front();
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
	}
}

void OcTree::SetVastInvalidGeneratingSet(SkeletonElement & cElement)
{
	m_hashElement.clear();
	m_hashElement.insert(((unsigned int)cElement.m_pElement));
	m_listGeneratingSet.clear();
	m_listGeneratingSet.push_back(cElement);
	m_nType = 54;
	//printf("Type 51 from Face : (%16I64x, %d, %d)\n", cElement.m_n64Position, cElement.m_nLevel, cElement.m_nDir);

	while (m_listGeneratingSet.size() != 0) {
		SkeletonElement cTemp = *m_listGeneratingSet.begin();
		m_listGeneratingSet.pop_front();
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
	}
}

void OcTree::DualSetVastInvalidGeneratingSet(SkeletonElement & cDualElement)
{
	m_hashElement.clear();
	m_hashElement.insert(((unsigned int)cDualElement.m_pElement));
	m_listGeneratingSet.clear();
	m_listGeneratingSet.push_back(cDualElement);
	//m_nType = 54;
	//printf("Type 51 from Face : (%16I64x, %d, %d)\n", cElement.m_n64Position, cElement.m_nLevel, cElement.m_nDir);

	while (m_listGeneratingSet.size() != 0) {
		SkeletonElement cTemp = *m_listGeneratingSet.begin();
		m_listGeneratingSet.pop_front();
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
	}
}

void OcTree::ReversePathComputeGeneratingSet(SkeletonElement & cElement)
{
	m_hashElement.clear();
	m_hashElement.insert(((unsigned int)cElement.m_pElement));
	m_listGeneratingSet.clear();
	m_listGeneratingSet.push_back(cElement);
	m_nType = 52;

	list<SkeletonElement>::iterator it = m_listGeneratingSet.begin();
	it->m_pDualElement = NULL;

	while (it != m_listGeneratingSet.end() && m_pFindElement != NULL) {
		m_pCurrentSkeletonElement = &(*it);
		switch (it->m_nDimension) {
		case 0:
			CheckPoint2(it->m_n64Position);
			break;
		case 1:
			CheckEdge2(it->m_n64Position, it->m_nLevel, it->m_nDir);
			break;
		case 2:
			CheckFace2(it->m_n64Position, it->m_nLevel, it->m_nDir);
			break;
		case 3:
			CheckCell2(it->m_n64Position, it->m_nLevel);
			break;
		}
		it++;
	}
}

// c: 0-5 f;  f: 0-1 c, 2-5 e; e: 0-3 f, 4-5 p; p: 0-5 e
void OcTree::DualPushWitnessPoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2], UCHAR cPointer)
{
	UCHAR * pEdge[2][2];
	UINT64 n64Edge[2][2];
	int nEdge[2][2];
	int dirEdge = cPointer / 2;
	int i = cPointer % 2;
	switch (dirEdge) {
	case 0:
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
			{
				pEdge[j][k] = pNode[i][j][k];
				n64Edge[j][k] = n64Position[i][j][k];
				nEdge[j][k] = nLevel[i][j][k];
			}
			break;
	case 1:
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
			{
				pEdge[j][k] = pNode[k][i][j];
				n64Edge[j][k] = n64Position[k][i][j];
				nEdge[j][k] = nLevel[k][i][j];
			}
			break;
	case 2:
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
			{
				pEdge[j][k] = pNode[j][k][i];
				n64Edge[j][k] = n64Position[j][k][i];
				nEdge[j][k] = nLevel[j][k][i];
			}
			break;
	}

	Element * pThisEdge = GetElementEdge( pEdge, nEdge, dirEdge );

	pair< hash_set<unsigned int>::iterator, bool > pr;

	switch ( m_nType ) {
	case 34:
		pr = m_hashElement.insert( (unsigned int)pThisEdge );
		if ( pr.second ) {
			UINT64 n64ThisEdge; int nThisEdge;
			GetEdgePosition(n64Edge, nEdge, dirEdge, n64ThisEdge, nThisEdge);
			// printf("Edge (witness) : (%16I64x, %d, %d)\n", n64ThisEdge, nThisEdge, dirEdge);
			if ( !m_bComputeOnly ) {
				m_listChanging.push_back(SkeletonElement(n64ThisEdge, nThisEdge, dirEdge, 1, pThisEdge));
			}
			else
				m_pCurrentSkeletonElement->m_n64Size += GetEdgeVolume(nThisEdge);

			SkeletonElement cTemp;
			cTemp.m_nDimension = 1;
			cTemp.m_n64Position = n64ThisEdge;
			cTemp.m_nLevel = nThisEdge;
			cTemp.m_nDir = dirEdge;
			cTemp.m_pElement = pThisEdge;
			m_listGeneratingSet.push_back(cTemp);
		}
		break;
	case 37:
		pr = m_hashElement.insert( (unsigned int)pThisEdge );
		if ( pr.second ) {
			UINT64 n64ThisEdge; int nThisEdge;
			GetEdgePosition(n64Edge, nEdge, dirEdge, n64ThisEdge, nThisEdge);
			// printf("Edge (witness) : (%16I64x, %d, %d)\n", n64ThisEdge, nThisEdge, dirEdge);

			SkeletonElement cTemp;
			cTemp.m_nDimension = 1;
			cTemp.m_n64Position = n64ThisEdge;
			cTemp.m_nLevel = nThisEdge;
			cTemp.m_nDir = dirEdge;
			cTemp.m_pElement = pThisEdge;
			m_listGeneratingSet.push_back(cTemp);
		}
		break;
	case 40:
		pr = m_hashElement.insert( (unsigned int)pThisEdge );
		if ( pr.second ) {
			UINT64 n64ThisEdge; int nThisEdge;
			GetEdgePosition(n64Edge, nEdge, dirEdge, n64ThisEdge, nThisEdge);
			//printf("Point (witness) : (%16I64x)\n", n64Temp);
			m_listGeneratingSet.push_back(SkeletonElement(n64ThisEdge, nThisEdge, dirEdge, 1, pThisEdge));
		}
		break;
	}
}

void OcTree::DualPushWitnessEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, UCHAR cPointer)
{
	if (cPointer > 3)
		return;

	UINT64 n64Temp; int nTemp;
	GetEdgePosition(n64Position, nLevel, dirEdge, n64Temp, nTemp);

	UCHAR * pFace[2];
	UINT64 n64Face[2];
	int nFace[2];
	int dirFace;
	int i = cPointer % 2;
	int ii = cPointer / 2;
	if (ii == 0) {
		for (int j = 0; j < 2; j++) {
			pFace[j] = pNode[i][j];
			n64Face[j] = n64Position[i][j];
			nFace[j] = nLevel[i][j];
		}
		dirFace = (dirEdge + 2) % 3;
	} else {
		for (int j = 0; j < 2; j++) {
			pFace[j] = pNode[j][i];
			n64Face[j] = n64Position[j][i];
			nFace[j] = nLevel[j][i];
		}
		dirFace = (dirEdge + 1) % 3;
	}

	Element * pThisFace = GetElementFace( pFace, nFace, dirFace );
	pair< hash_set<unsigned int>::iterator, bool > pr;

	switch (m_nType) {
	case 34:
		pr = m_hashElement.insert( (unsigned int)pThisFace );
		if ( pr.second ) {
			UINT64 n64ThisFace; int nThisFace;
			GetFacePosition(n64Face, nFace, dirFace, n64ThisFace, nThisFace);
			// printf("Face (witness) : (%16I64x, %d, %d)\n", n64ThisFace, nThisFace, dirFace);
			if ( !m_bComputeOnly ) {
				m_listChanging.push_back(SkeletonElement(n64ThisFace, nThisFace, dirFace, 2, pThisFace));
			} else
				m_pCurrentSkeletonElement->m_n64Size += GetFaceVolume(nThisFace);

			SkeletonElement cTemp;
			cTemp.m_nDimension = 2;
			cTemp.m_n64Position = n64ThisFace;
			cTemp.m_nLevel = nThisFace;
			cTemp.m_nDir = dirFace;
			cTemp.m_pElement = pThisFace;
			m_listGeneratingSet.push_back(cTemp);
		}
		break;
	case 37:
		pr = m_hashElement.insert( (unsigned int)pThisFace );
		if ( pr.second ) {
			UINT64 n64ThisFace; int nThisFace;
			GetFacePosition(n64Face, nFace, dirFace, n64ThisFace, nThisFace);
			// printf("Face (witness) : (%16I64x, %d, %d)\n", n64ThisFace, nThisFace, dirFace);

			SkeletonElement cTemp;
			cTemp.m_nDimension = 2;
			cTemp.m_n64Position = n64ThisFace;
			cTemp.m_nLevel = nThisFace;
			cTemp.m_nDir = dirFace;
			cTemp.m_pElement = pThisFace;
			m_listGeneratingSet.push_back(cTemp);
		}
		break;
	case 40:
		pr = m_hashElement.insert( (unsigned int)pThisFace );
		if ( pr.second ) {
			UINT64 n64ThisFace; int nThisFace;
			GetFacePosition(n64Face, nFace, dirFace, n64ThisFace, nThisFace);
			// printf("Face (witness) : (%16I64x, %d, %d)\n", n64ThisFace, nThisFace, dirFace);
			m_listGeneratingSet.push_back(SkeletonElement(n64ThisFace, nThisFace, dirFace, 2, pThisFace));
		}
		break;
	}
}

void OcTree::DualPushWitnessFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, UCHAR cPointer)
{
	if (cPointer > 1)
		return;
	Element * pThisCell = GetElementCell( pNode[cPointer] );

	pair< hash_set<unsigned int>::iterator, bool > pr;

	switch ( m_nType ) {
	case 34:
		pr = m_hashElement.insert( (unsigned int)pThisCell );
		if ( pr.second ) {
			//printf("Cell (witness) : (%16I64x, %d)\n", n64Position[cPointer], nLevel[cPointer]);
			if ( !m_bComputeOnly ) {
				m_listChanging.push_back(SkeletonElement(n64Position[cPointer], nLevel[cPointer], 0, 3, pThisCell));
			} else
				m_pCurrentSkeletonElement->m_n64Size += GetCellVolume(nLevel[cPointer]);

			SkeletonElement cTemp;
			cTemp.m_nDimension = 3;
			cTemp.m_n64Position = n64Position[cPointer];
			cTemp.m_nLevel = nLevel[cPointer];
			cTemp.m_pElement = pThisCell;
			m_listGeneratingSet.push_back(cTemp);
		}
		break;
	case 37:
		pr = m_hashElement.insert( (unsigned int)pThisCell );
		if ( pr.second ) {
			//printf("Cell (witness) : (%16I64x, %d)\n", n64Position[cPointer], nLevel[cPointer]);

			SkeletonElement cTemp;
			cTemp.m_nDimension = 3;
			cTemp.m_n64Position = n64Position[cPointer];
			cTemp.m_nLevel = nLevel[cPointer];
			cTemp.m_pElement = pThisCell;
			m_listGeneratingSet.push_back(cTemp);
		}
		break;
	case 40:
		pr = m_hashElement.insert( (unsigned int)pThisCell );
		if ( pr.second ) {
			//printf("Cell (witness) : (%16I64x, %d)\n", n64Position[cPointer], nLevel[cPointer]);
			m_listGeneratingSet.push_back(SkeletonElement(n64Position[cPointer], nLevel[cPointer], 0, 3, pThisCell));
		}
		break;
	}
}

void OcTree::PushWitnessEdge(UINT64 n64ThisEdge, int nThisEdge, int dirThisEdge, UCHAR cPointer)
{
	if (cPointer <= 3)
		return;
	UINT64 n64Temp;
	UCHAR * pPoint[2][2][2];
	UINT64 n64Point[2][2][2];
	int nPoint[2][2][2];
	if (cPointer == 4) {
		n64Temp = n64ThisEdge;
	} else {
		switch (dirThisEdge) {
		case 0:
			n64Temp = n64ThisEdge + INT64TRANS(NLENGTH(nThisEdge), 0, 0);
			break;
		case 1:
			n64Temp = n64ThisEdge + INT64TRANS(0, NLENGTH(nThisEdge), 0);
			break;
		case 2:
			n64Temp = n64ThisEdge + INT64TRANS(0, 0, NLENGTH(nThisEdge));
			break;
		}
	}

	GetPointFromPosition(n64Temp, m_nMaxLevel, pPoint, n64Point, nPoint);
	Element * pThisPoint = GetElementPoint(pPoint, nPoint);
	pair< hash_set<unsigned int>::iterator, bool > pr;

	switch (m_nType) {
	case 49:
		pr = m_hashElement.insert( (unsigned int)pThisPoint );
		if ( pr.second ) {
			// printf("Point (witness) : (%16I64x)\n", n64Temp);
			if ( !m_bComputeOnly ) {
				m_listChanging.push_back(SkeletonElement(n64Temp, 0, 0, 0, pThisPoint));
			} else
				m_pCurrentSkeletonElement->m_n64Size += GetPointVolume();
			SkeletonElement cTemp;
			cTemp.m_nDimension = 0;
			cTemp.m_n64Position = n64Temp;
			cTemp.m_pElement = pThisPoint;
			m_listGeneratingSet.push_back(cTemp);
		}
		break;
	case 50:
		if (!pThisPoint->m_bSkeleton) {
			pair< hash_map<unsigned int, unsigned int>::iterator, bool > pr;
			pr = m_hashElementExtend.insert( pair<unsigned int, unsigned int>((unsigned int)pThisPoint, m_nFatherStatus) );
			if ( pr.second ) {
				// printf("Point (witness) : (%16I64x)\n", n64Temp);
				SkeletonElement cTemp;
				cTemp.m_nDimension = 0;
				cTemp.m_n64Position = n64Temp;
				cTemp.m_pElement = pThisPoint;
				m_listGeneratingSet.push_back(cTemp);
			} else {
				if ((pr.first)->second == 2) {
					// printf("Point (witness, again) : (%16I64x)\n", n64Temp);
					SkeletonElement cTemp;
					cTemp.m_nDimension = 0;
					cTemp.m_n64Position = n64Temp;
					cTemp.m_pElement = pThisPoint;
					m_listGeneratingSet.push_back(cTemp);
				}
				(pr.first)->second = 1;
			}
		}
		break;
	case 51:
		pr = m_hashElement.insert( (unsigned int)pThisPoint );
		if ( pr.second ) {
			//printf("Point (witness) : (%16I64x)\n", n64Temp);
			SkeletonElement cTemp(n64Temp, 0, 0, 0, pThisPoint);
			m_listChanging.push_back(cTemp);
			m_listGeneratingSet.push_back(cTemp);
		}
		break;
	case 52:
		pr = m_hashElement.insert( (unsigned int)pThisPoint );
		if ( pr.second ) {
			//printf("Point (witness) : (%16I64x)\n", n64Temp);
			SkeletonElement cTemp;
			cTemp.m_nDimension = 0;
			cTemp.m_n64Position = n64Temp;
			cTemp.m_pElement = pThisPoint;
			cTemp.m_pSimpleElement = m_pFindCurrentElement;
			cTemp.m_pDualElement = m_pCurrentSkeletonElement;
			cTemp.m_cPointer = m_cFindPointer;
			cTemp.m_cBackPointer = m_cFindBackPointer;
			m_listGeneratingSet.push_back(cTemp);
		}
		break;
	case 54:
		pr = m_hashElement.insert( (unsigned int)pThisPoint );
		if ( pr.second ) {
			//printf("Point (witness) : (%16I64x)\n", n64Temp);
			SkeletonElement cTemp;
			cTemp.m_nDimension = 0;
			cTemp.m_n64Position = n64Temp;
			cTemp.m_pElement = pThisPoint;
			m_listGeneratingSet.push_back(cTemp);
		}
		break;
	case 57:
		pr = m_hashElement.insert( (unsigned int)pThisPoint );
		if ( pr.second ) {
			//printf("Point (witness) : (%16I64x)\n", n64Temp);
			m_listGeneratingSet.push_back(SkeletonElement(n64Temp, 0, 0, 0, pThisPoint));
		}
		break;
	}
}

void OcTree::PushWitnessFace(UINT64 n64ThisFace, int nThisFace, int dirThisFace, UCHAR cPointer)
{
	if (cPointer <= 1)
		return;
	UINT64 n64Temp;
	UCHAR * pEdge[2][2];
	UINT64 n64Edge[2][2];
	int nEdge[2][2];
	int dirEdge;
	UCHAR cCheck;
	switch (cPointer) {
	case 2:
		cCheck = 1;
		dirEdge = (dirThisFace + 1) % 3;
		n64Temp = n64ThisFace;
		break;
	case 3:
		cCheck = 0;
		dirEdge = (dirThisFace + 1) % 3;
		switch (dirThisFace) {
	case 0:
		n64Temp = n64ThisFace + INT64TRANS(0, 0, NLENGTH(nThisFace));
		break;
	case 1:
		n64Temp = n64ThisFace + INT64TRANS(NLENGTH(nThisFace), 0, 0);
		break;
	case 2:
		n64Temp = n64ThisFace + INT64TRANS(0, NLENGTH(nThisFace), 0);
		break;
		}
		break;
	case 4:
		cCheck = 3;
		dirEdge = (dirThisFace + 2) % 3;
		n64Temp = n64ThisFace;
		break;
	case 5:
		cCheck = 2;
		dirEdge = (dirThisFace + 2) % 3;
		switch (dirThisFace) {
	case 0:
		n64Temp = n64ThisFace + INT64TRANS(0, NLENGTH(nThisFace), 0);
		break;
	case 1:
		n64Temp = n64ThisFace + INT64TRANS(0, 0, NLENGTH(nThisFace));
		break;
	case 2:
		n64Temp = n64ThisFace + INT64TRANS(NLENGTH(nThisFace), 0, 0);
		break;
		}
		break;
	}

	GetEdgeFromPosition(n64Temp, nThisFace, dirEdge, pEdge, n64Edge, nEdge);
	RecPushWitnessFaceEdge(pEdge, n64Edge, nEdge, dirEdge, cCheck);
}

void OcTree::PushWitnessCell(UINT64 n64ThisCell, int nThisCell, UCHAR cPointer)
{
	UINT64 n64Temp;
	UCHAR * pFace[2];
	UINT64 n64Face[2];
	int nFace[2];
	int dirFace;
	UCHAR cCheck;

	dirFace = cPointer / 2;
	cCheck = 1 - (cPointer % 2);
	if (cCheck == 1) {
		n64Temp = n64ThisCell;
	} else {
		switch (dirFace) {
		case 0:
			n64Temp = n64ThisCell + INT64TRANS(NLENGTH(nThisCell), 0, 0);
			break;
		case 1:
			n64Temp = n64ThisCell + INT64TRANS(0, NLENGTH(nThisCell), 0);
			break;
		case 2:
			n64Temp = n64ThisCell + INT64TRANS(0, 0, NLENGTH(nThisCell));
			break;
		}
	}

	GetFaceFromPosition(n64Temp, nThisCell, dirFace, pFace, n64Face, nFace);
	RecPushWitnessCellFace(pFace, n64Face, nFace, dirFace, cCheck);
}

void OcTree::RecPushWitnessFaceEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, UCHAR cCheck)
{
	UCHAR * pNodeEdge[2][2];
	UINT64 n64Edge[2][2];
	int nEdge[2][2];

	if (NOT_LEAF(pNode[0][0]) || NOT_LEAF(pNode[0][1]) || NOT_LEAF(pNode[1][0]) || NOT_LEAF(pNode[1][1])) {
		switch(dirEdge) {
		case 0:
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], i, 1-j, 1-k, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				RecPushWitnessFaceEdge(pNodeEdge, n64Edge, nEdge, dirEdge, cCheck);
			}
			break;
		case 1:
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], 1-k, i, 1-j, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				RecPushWitnessFaceEdge(pNodeEdge, n64Edge, nEdge, dirEdge, cCheck);
			}
			break;
		case 2:
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], 1-j, 1-k, i, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				RecPushWitnessFaceEdge(pNodeEdge, n64Edge, nEdge, dirEdge, cCheck);
			}
			break;
		}
	} else {
		Element * pThisEdge = GetElementEdge(pNode, nLevel, dirEdge);

		switch (m_nType) {
		case 49:
			if (pThisEdge->m_bObject && !pThisEdge->m_bSkeleton && pThisEdge->m_cPointer == cCheck) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisEdge );
				if ( pr.second ) {
					UINT64 n64ThisEdge; int nThisEdge;
					GetEdgePosition(n64Position, nLevel, dirEdge, n64ThisEdge, nThisEdge);
					// printf("Edge (witness) : (%16I64x, %d, %d)\n", n64ThisEdge, nThisEdge, dirEdge);
					if ( !m_bComputeOnly ) {
						m_listChanging.push_back(SkeletonElement(n64ThisEdge, nThisEdge, dirEdge, 1, pThisEdge));
					} else
						m_pCurrentSkeletonElement->m_n64Size += GetEdgeVolume(nThisEdge);

					SkeletonElement cTemp;
					cTemp.m_nDimension = 1;
					cTemp.m_n64Position = n64ThisEdge;
					cTemp.m_nLevel = nThisEdge;
					cTemp.m_nDir = dirEdge;
					cTemp.m_pElement = pThisEdge;
					m_listGeneratingSet.push_back(cTemp);
				}
			}
			break;
		case 50:
			if (!pThisEdge->m_bSkeleton && pThisEdge->m_cPointer == cCheck) {
				pair< hash_map<unsigned int, unsigned int>::iterator, bool > pr;
				pr = m_hashElementExtend.insert( pair<unsigned int, unsigned int>((unsigned int)pThisEdge, m_nFatherStatus) );
				if ( pr.second ) {
					UINT64 n64ThisEdge; int nThisEdge;
					GetEdgePosition(n64Position, nLevel, dirEdge, n64ThisEdge, nThisEdge);
					// printf("Edge (witness) : (%16I64x, %d, %d)\n", n64ThisEdge, nThisEdge, dirEdge);
					SkeletonElement cTemp;
					cTemp.m_nDimension = 1;
					cTemp.m_n64Position = n64ThisEdge;
					cTemp.m_nLevel = nThisEdge;
					cTemp.m_nDir = dirEdge;
					cTemp.m_pElement = pThisEdge;
					m_listGeneratingSet.push_back(cTemp);
				} else {
					if ((pr.first)->second == 2) {
						UINT64 n64ThisEdge; int nThisEdge;
						GetEdgePosition(n64Position, nLevel, dirEdge, n64ThisEdge, nThisEdge);
						// printf("Edge (witness, again) : (%16I64x, %d, %d)\n", n64ThisEdge, nThisEdge, dirEdge);
						SkeletonElement cTemp;
						cTemp.m_nDimension = 1;
						cTemp.m_n64Position = n64ThisEdge;
						cTemp.m_nLevel = nThisEdge;
						cTemp.m_nDir = dirEdge;
						cTemp.m_pElement = pThisEdge;
						m_listGeneratingSet.push_back(cTemp);
					}
					(pr.first)->second = 1;
				}
			}
			break;
		case 51:
			if (!pThisEdge->m_bSkeleton && pThisEdge->m_cPointer == cCheck) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisEdge );
				if ( pr.second ) {
					UINT64 n64ThisEdge; int nThisEdge;
					GetEdgePosition(n64Position, nLevel, dirEdge, n64ThisEdge, nThisEdge);
					//printf("Edge (witness) : (%16I64x, %d, %d)\n", n64ThisEdge, nThisEdge, dirEdge);
					SkeletonElement cTemp(n64ThisEdge, nThisEdge, dirEdge, 1, pThisEdge);
					m_listChanging.push_back(cTemp);
					m_listGeneratingSet.push_back(cTemp);
				}
			}
			break;
		case 52:
			if (!pThisEdge->m_bSkeleton && pThisEdge->m_cPointer == cCheck) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisEdge );
				if ( pr.second ) {
					UINT64 n64ThisEdge; int nThisEdge;
					GetEdgePosition(n64Position, nLevel, dirEdge, n64ThisEdge, nThisEdge);
					//printf("Edge (witness) : (%16I64x, %d, %d)\n", n64ThisEdge, nThisEdge, dirEdge);
					SkeletonElement cTemp;
					cTemp.m_nDimension = 1;
					cTemp.m_n64Position = n64ThisEdge;
					cTemp.m_nLevel = nThisEdge;
					cTemp.m_nDir = dirEdge;
					cTemp.m_pElement = pThisEdge;
					cTemp.m_pSimpleElement = m_pFindCurrentElement;
					cTemp.m_pDualElement = m_pCurrentSkeletonElement;
					cTemp.m_cPointer = m_cFindPointer;
					cTemp.m_cBackPointer = m_cFindBackPointer;
					m_listGeneratingSet.push_back(cTemp);
				}
			}
			break;
		case 54:
			if (!pThisEdge->m_bSkeleton && pThisEdge->m_cPointer == cCheck) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisEdge );
				if ( pr.second ) {
					UINT64 n64ThisEdge; int nThisEdge;
					GetEdgePosition(n64Position, nLevel, dirEdge, n64ThisEdge, nThisEdge);
					//printf("Edge (witness) : (%16I64x, %d, %d)\n", n64ThisEdge, nThisEdge, dirEdge);
					SkeletonElement cTemp;
					cTemp.m_nDimension = 1;
					cTemp.m_n64Position = n64ThisEdge;
					cTemp.m_nLevel = nThisEdge;
					cTemp.m_nDir = dirEdge;
					cTemp.m_pElement = pThisEdge;
					m_listGeneratingSet.push_back(cTemp);
				}
			}
			break;
		case 57:
			pair< hash_set<unsigned int>::iterator, bool > pr = m_hashElement.insert( (unsigned int)pThisEdge );
			if ( pr.second ) {
				UINT64 n64ThisEdge; int nThisEdge;
				GetEdgePosition(n64Position, nLevel, dirEdge, n64ThisEdge, nThisEdge);
				//printf("Edge (witness) : (%16I64x, %d, %d)\n", n64ThisEdge, nThisEdge, dirEdge);
				m_listGeneratingSet.push_back(SkeletonElement(n64ThisEdge, nThisEdge, dirEdge, 1, pThisEdge));
			}
			break;
		}
	}
}

void OcTree::RecPushWitnessCellFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, UCHAR cCheck)
{
	UCHAR * pNodeFace[2];
	UINT64 n64Face[2];
	int nFace[2];

	if (NOT_LEAF(pNode[0]) || NOT_LEAF(pNode[1])) {
		switch (dirFace) {
		case 0:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k], n64Position[k], nLevel[k], 1-k, i, j, pNodeFace[k], n64Face[k], nFace[k]);
					RecPushWitnessCellFace(pNodeFace, n64Face, nFace, dirFace, cCheck);
				}
				break;
		case 1:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k], n64Position[k], nLevel[k], j, 1-k, i, pNodeFace[k], n64Face[k], nFace[k]);
					RecPushWitnessCellFace(pNodeFace, n64Face, nFace, dirFace, cCheck);
				}
				break;
		case 2:
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					for (int k = 0; k < 2; k++)
						Leaf(pNode[k], n64Position[k], nLevel[k], i, j, 1-k, pNodeFace[k], n64Face[k], nFace[k]);
					RecPushWitnessCellFace(pNodeFace, n64Face, nFace, dirFace, cCheck);
				}
				break;
		}
	} else {
		Element * pThisFace = GetElementFace( pNode, nLevel, dirFace );

		switch (m_nType) {
		case 49:
			if (pThisFace->m_bObject && !pThisFace->m_bSkeleton && pThisFace->m_cPointer == cCheck) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisFace );
				if ( pr.second ) {
					UINT64 n64ThisFace; int nThisFace;
					GetFacePosition(n64Position, nLevel, dirFace, n64ThisFace, nThisFace);
					// printf("Face (witness) : (%16I64x, %d, %d)\n", n64ThisFace, nThisFace, dirFace);
					if ( !m_bComputeOnly ) {
						m_listChanging.push_back(SkeletonElement(n64ThisFace, nThisFace, dirFace, 2, pThisFace));
					} else
						m_pCurrentSkeletonElement->m_n64Size += GetFaceVolume(nThisFace);

					SkeletonElement cTemp;
					cTemp.m_nDimension = 2;
					cTemp.m_n64Position = n64ThisFace;
					cTemp.m_nLevel = nThisFace;
					cTemp.m_nDir = dirFace;
					cTemp.m_pElement = pThisFace;
					m_listGeneratingSet.push_back(cTemp);
				}
			}
			break;
		case 50:
			if (!pThisFace->m_bSkeleton && pThisFace->m_cPointer == cCheck) {
				pair< hash_map<unsigned int, unsigned int>::iterator, bool > pr;
				pr = m_hashElementExtend.insert( pair<unsigned int, unsigned int>((unsigned int)pThisFace, m_nFatherStatus) );
				if ( pr.second ) {
					UINT64 n64ThisFace; int nThisFace;
					GetFacePosition(n64Position, nLevel, dirFace, n64ThisFace, nThisFace);
					// printf("Face (witness) : (%16I64x, %d, %d)\n", n64ThisFace, nThisFace, dirFace);
					SkeletonElement cTemp;
					cTemp.m_nDimension = 2;
					cTemp.m_n64Position = n64ThisFace;
					cTemp.m_nLevel = nThisFace;
					cTemp.m_nDir = dirFace;
					cTemp.m_pElement = pThisFace;
					m_listGeneratingSet.push_back(cTemp);
				} else {
					if ((pr.first)->second == 2) {
						UINT64 n64ThisFace; int nThisFace;
						GetFacePosition(n64Position, nLevel, dirFace, n64ThisFace, nThisFace);
						// printf("Face (witness) : (%16I64x, %d, %d)\n", n64ThisFace, nThisFace, dirFace);
						SkeletonElement cTemp;
						cTemp.m_nDimension = 2;
						cTemp.m_n64Position = n64ThisFace;
						cTemp.m_nLevel = nThisFace;
						cTemp.m_nDir = dirFace;
						cTemp.m_pElement = pThisFace;
						m_listGeneratingSet.push_back(cTemp);
					}
					(pr.first)->second = 1;
				}
			}
			break;
		case 51:
			if (!pThisFace->m_bSkeleton && pThisFace->m_cPointer == cCheck) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisFace );
				if ( pr.second ) {
					UINT64 n64ThisFace; int nThisFace;
					GetFacePosition(n64Position, nLevel, dirFace, n64ThisFace, nThisFace);
					//printf("Face (witness) : (%16I64x, %d, %d)\n", n64ThisFace, nThisFace, dirFace);
					SkeletonElement cTemp(n64ThisFace, nThisFace, dirFace, 2, pThisFace);
					m_listChanging.push_back(cTemp);
					m_listGeneratingSet.push_back(cTemp);
				}
			}
			break;
		case 52:
			if (!pThisFace->m_bSkeleton && pThisFace->m_cPointer == cCheck) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisFace );
				if ( pr.second ) {
					UINT64 n64ThisFace; int nThisFace;
					GetFacePosition(n64Position, nLevel, dirFace, n64ThisFace, nThisFace);
					//printf("Face (witness) : (%16I64x, %d, %d)\n", n64ThisFace, nThisFace, dirFace);
					SkeletonElement cTemp;
					cTemp.m_nDimension = 2;
					cTemp.m_n64Position = n64ThisFace;
					cTemp.m_nLevel = nThisFace;
					cTemp.m_nDir = dirFace;
					cTemp.m_pElement = pThisFace;
					cTemp.m_pSimpleElement = m_pFindCurrentElement;
					cTemp.m_pDualElement = m_pCurrentSkeletonElement;
					cTemp.m_cPointer = m_cFindPointer;
					cTemp.m_cBackPointer = m_cFindBackPointer;
					m_listGeneratingSet.push_back(cTemp);
				}
			}
			break;
		case 54:
			if (!pThisFace->m_bSkeleton && pThisFace->m_cPointer == cCheck) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pThisFace );
				if ( pr.second ) {
					UINT64 n64ThisFace; int nThisFace;
					GetFacePosition(n64Position, nLevel, dirFace, n64ThisFace, nThisFace);
					//printf("Face (witness) : (%16I64x, %d, %d)\n", n64ThisFace, nThisFace, dirFace);
					SkeletonElement cTemp;
					cTemp.m_nDimension = 2;
					cTemp.m_n64Position = n64ThisFace;
					cTemp.m_nLevel = nThisFace;
					cTemp.m_nDir = dirFace;
					cTemp.m_pElement = pThisFace;
					m_listGeneratingSet.push_back(cTemp);
				}
			}
			break;
		case 57:
			pair< hash_set<unsigned int>::iterator, bool > pr = m_hashElement.insert( (unsigned int)pThisFace );
			if ( pr.second ) {
				UINT64 n64ThisFace; int nThisFace;
				GetFacePosition(n64Position, nLevel, dirFace, n64ThisFace, nThisFace);
				//printf("Face (witness) : (%16I64x, %d, %d)\n", n64ThisFace, nThisFace, dirFace);
				m_listGeneratingSet.push_back(SkeletonElement(n64ThisFace, nThisFace, dirFace, 2, pThisFace));
			}
			break;
		}
	}
}

void OcTree::ReversePathTrackPath()
{
	m_pFindCurrentElement->m_cPointer = m_cFindBackPointer;
	m_pCurrentSkeletonElement->m_pElement->m_cPointer = m_cFindPointer;

	while (m_pCurrentSkeletonElement->m_pDualElement != NULL) {
		// printf("Edge (path) : (%16I64x, %d, %d)\n", m_pCurrentSkeletonElement->m_n64Position, m_pCurrentSkeletonElement->m_nLevel, m_pCurrentSkeletonElement->m_nDir);
		m_pCurrentSkeletonElement->m_pSimpleElement->m_cPointer = m_pCurrentSkeletonElement->m_cBackPointer;
		m_pCurrentSkeletonElement->m_pDualElement->m_pElement->m_cPointer = m_pCurrentSkeletonElement->m_cPointer;
		m_pCurrentSkeletonElement = m_pCurrentSkeletonElement->m_pDualElement;
	}
	// printf("Edge (end point of path) : (%16I64x, %d, %d)\n", m_pCurrentSkeletonElement->m_n64Position, m_pCurrentSkeletonElement->m_nLevel, m_pCurrentSkeletonElement->m_nDir);

	m_pFindElement = NULL;
}
