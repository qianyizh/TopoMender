#include "OcTree.h"
using namespace std;

//////////////////////////////////////////////////////////////////////////
// containing point functions
//////////////////////////////////////////////////////////////////////////
// COcTree::m_nType, which determines the behaviors done during each traversal.
//
// 48 -- check if an element is seperating
// 49 -- generate a generating set
// 50 -- generate vast generating set
// 51 -- modify dual skeleton of generating vast generating set
// 52 -- reverse path
// 53 -- assign the FAREST footpoint, for object thinning
// 54 -- set invalid for complement skeleton elements
// 55 -- check if a element is simple
// 56 -- push elements for dual pure thinning
// 57 -- dual set invalid set
//////////////////////////////////////////////////////////////////////////
void OcTree::CheckPoint2(UINT64 n64CheckPoint)
{
	UCHAR * pThisPoint[2][2][2];
	UINT64 n64ThisPoint[2][2][2];
	int nThisPoint[2][2][2];
	GetPointFromPosition(n64CheckPoint, m_nMaxLevel, pThisPoint, n64ThisPoint, nThisPoint);
	Element * pTempPoint = GetElementPoint(pThisPoint, nThisPoint);
	CheckPoint2(pThisPoint, n64ThisPoint, nThisPoint, pTempPoint);
}

void OcTree::CheckPoint2(UCHAR * pThisPoint[2][2][2], UINT64 n64ThisPoint[2][2][2], int nThisPoint[2][2][2], Element * pTempPoint)
{
	UCHAR * pEdge[2][2];
	UCHAR * pFace[2];
	UCHAR * pCell;
	UINT64 n64Edge[2][2];
	UINT64 n64Face[2];
	UINT64 n64Cell;
	int nEdge[2][2];
	int nFace[2];
	int nCell;
	UINT64 n64Temp;
	int nTemp;
	int nI, nJ, nIndex;
	int dirEdge, dirFace;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
			{
				pEdge[j][k] = pThisPoint[i][j][k];
				n64Edge[j][k] = n64ThisPoint[i][j][k];
				nEdge[j][k] = nThisPoint[i][j][k];
			}
			dirEdge = 0;
			if (CheckStorageEdge(pEdge)) {
				Element * pTempEdge = GetElementEdge(pEdge, nEdge, dirEdge, nI, nJ, nIndex);
				n64Temp = LeafPosition(n64Edge[nI][nJ], nEdge[nI][nJ], Constant::I_SHIFT_EP[Constant::II_SHIFT_CE[dirEdge][nI][nJ]][0]);
				nTemp = nEdge[nI][nJ];
				if (m_nType == 52 || m_nType == 55) {
					m_cFindPointer = dirEdge * 2 + i;
					m_cFindBackPointer = 1 - i + 4;
				}
				if (pTempPoint->m_bSkeleton || pTempPoint->m_cPointer != dirEdge * 2 + i)
					CheckEdgeDetail( n64Temp, nTemp, dirEdge, pTempEdge );
			}
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					pEdge[j][k] = pThisPoint[k][i][j];
					n64Edge[j][k] = n64ThisPoint[k][i][j];
					nEdge[j][k] = nThisPoint[k][i][j];
				}
				dirEdge = 1;
				if (CheckStorageEdge(pEdge)) {
					Element * pTempEdge = GetElementEdge(pEdge, nEdge, dirEdge, nI, nJ, nIndex);
					n64Temp = LeafPosition(n64Edge[nI][nJ], nEdge[nI][nJ], Constant::I_SHIFT_EP[Constant::II_SHIFT_CE[dirEdge][nI][nJ]][0]);
					nTemp = nEdge[nI][nJ];
					if (m_nType == 52 || m_nType == 55) {
						m_cFindPointer = dirEdge * 2 + i;
						m_cFindBackPointer = 1 - i + 4;
					}
					if (pTempPoint->m_bSkeleton || pTempPoint->m_cPointer != dirEdge * 2 + i)
						CheckEdgeDetail( n64Temp, nTemp, dirEdge, pTempEdge );
				}
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
					{
						pEdge[j][k] = pThisPoint[j][k][i];
						n64Edge[j][k] = n64ThisPoint[j][k][i];
						nEdge[j][k] = nThisPoint[j][k][i];
					}
					dirEdge = 2;
					if (CheckStorageEdge(pEdge)) {
						Element * pTempEdge = GetElementEdge(pEdge, nEdge, dirEdge, nI, nJ, nIndex);
						n64Temp = LeafPosition(n64Edge[nI][nJ], nEdge[nI][nJ], Constant::I_SHIFT_EP[Constant::II_SHIFT_CE[dirEdge][nI][nJ]][0]);
						nTemp = nEdge[nI][nJ];
						if (m_nType == 52 || m_nType == 55) {
							m_cFindPointer = dirEdge * 2 + i;
							m_cFindBackPointer = 1 - i + 4;
						}
						if (pTempPoint->m_bSkeleton || pTempPoint->m_cPointer != dirEdge * 2 + i)
							CheckEdgeDetail( n64Temp, nTemp, dirEdge, pTempEdge );
					}
	}

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				pFace[k] = pThisPoint[k][i][j];
				n64Face[k] = n64ThisPoint[k][i][j];
				nFace[k] = nThisPoint[k][i][j];
			}
			dirFace = 0;
			if (CheckStorageFace(pFace)) {
				Element * pTempFace = GetElementFace(pFace, nFace, dirFace, nI, nIndex);
				n64Temp = LeafPosition(n64Face[nI], nFace[nI], Constant::I_SHIFT_FP[Constant::II_SHIFT_CF[dirFace][nI]][0]);
				nTemp = nFace[nI];
				CheckFaceDetail( n64Temp, nTemp, dirFace, pTempFace );
			}
			for (int k = 0; k < 2; k++)
			{
				pFace[k] = pThisPoint[j][k][i];
				n64Face[k] = n64ThisPoint[j][k][i];
				nFace[k] = nThisPoint[j][k][i];
			}
			dirFace = 1;
			if (CheckStorageFace(pFace)) {
				Element * pTempFace = GetElementFace(pFace, nFace, dirFace, nI, nIndex);
				n64Temp = LeafPosition(n64Face[nI], nFace[nI], Constant::I_SHIFT_FP[Constant::II_SHIFT_CF[dirFace][nI]][0]);
				nTemp = nFace[nI];
				CheckFaceDetail( n64Temp, nTemp, dirFace, pTempFace );
			}
			for (int k = 0; k < 2; k++)
			{
				pFace[k] = pThisPoint[i][j][k];
				n64Face[k] = n64ThisPoint[i][j][k];
				nFace[k] = nThisPoint[i][j][k];
			}
			dirFace = 2;
			if (CheckStorageFace(pFace)) {
				Element * pTempFace = GetElementFace(pFace, nFace, dirFace, nI, nIndex);
				n64Temp = LeafPosition(n64Face[nI], nFace[nI], Constant::I_SHIFT_FP[Constant::II_SHIFT_CF[dirFace][nI]][0]);
				nTemp = nFace[nI];
				CheckFaceDetail( n64Temp, nTemp, dirFace, pTempFace );
			}
		}

		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					pCell = pThisPoint[i][j][k];
					n64Cell = n64ThisPoint[i][j][k];
					nCell = nThisPoint[i][j][k];
					Element * pTempCell = GetElementCell(pCell);
					CheckCellDetail( n64Cell, nCell, pTempCell );
				}
}

void OcTree::CheckEdge2(UINT64 n64CheckEdge, int nCheckEdge, int dirCheckEdge)
{
	UCHAR * pThisEdge[2][2];
	UINT64 n64ThisEdge[2][2];
	int nThisEdge[2][2];
	GetEdgeFromPosition(n64CheckEdge, m_nMaxLevel, dirCheckEdge, pThisEdge, n64ThisEdge, nThisEdge);
	Element * pTempEdge = GetElementEdge(pThisEdge, nThisEdge, dirCheckEdge);
	CheckEdge2(pThisEdge, n64ThisEdge, nThisEdge, dirCheckEdge, pTempEdge);
}

void OcTree::CheckEdge2(UCHAR * pThisEdge[2][2], UINT64 n64ThisEdge[2][2], int nThisEdge[2][2], int dirCheckEdge, Element * pTempEdge)
{
	UCHAR * pFace[2];
	UCHAR * pCell;
	UINT64 n64Face[2];
	UINT64 n64Cell;
	int nFace[2];
	int nCell;
	UINT64 n64Temp;
	int nTemp;
	int nI, nIndex;
	int dirFace;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++) {
			pFace[j] = pThisEdge[i][j];
			n64Face[j] = n64ThisEdge[i][j];
			nFace[j] = nThisEdge[i][j];
		}
		dirFace = (dirCheckEdge + 2) % 3;
		if (CheckStorageFace(pFace)) {
			Element * pTempFace = GetElementFace(pFace, nFace, dirFace, nI, nIndex);
			n64Temp = LeafPosition(n64Face[nI], nFace[nI], Constant::I_SHIFT_FP[Constant::II_SHIFT_CF[dirFace][nI]][0]);
			nTemp = nFace[nI];
			if (m_nType == 52 || m_nType == 55) {
				m_cFindPointer = (0 * 2) + i;
				m_cFindBackPointer = (0 * 2) + (1 - i) + 2;
			}
			if (pTempEdge->m_bSkeleton || pTempEdge->m_cPointer != (0 * 2) + i)
				CheckFaceDetail( n64Temp, nTemp, dirFace, pTempFace );
		}
		for (int j = 0; j < 2; j++) {
			pFace[j] = pThisEdge[j][i];
			n64Face[j] = n64ThisEdge[j][i];
			nFace[j] = nThisEdge[j][i];
		}
		dirFace = (dirCheckEdge + 1) % 3;
		if (CheckStorageFace(pFace)) {
			Element * pTempFace = GetElementFace(pFace, nFace, dirFace, nI, nIndex);
			n64Temp = LeafPosition(n64Face[nI], nFace[nI], Constant::I_SHIFT_FP[Constant::II_SHIFT_CF[dirFace][nI]][0]);
			nTemp = nFace[nI];
			if (m_nType == 52 || m_nType == 55) {
				m_cFindPointer = (1 * 2) + i;
				m_cFindBackPointer = (1 * 2) + (1 - i) + 2;
			}
			if (pTempEdge->m_bSkeleton || pTempEdge->m_cPointer != (1 * 2) + i)
				CheckFaceDetail( n64Temp, nTemp, dirFace, pTempFace );
		}
	}

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
		{
			pCell = pThisEdge[i][j];
			n64Cell = n64ThisEdge[i][j];
			nCell = nThisEdge[i][j];
			Element * pTempCell = GetElementCell(pCell);
			CheckCellDetail( n64Cell, nCell, pTempCell );
		}
}

void OcTree::CheckFace2(UINT64 n64CheckFace, int nCheckFace, int dirCheckFace)
{
	UCHAR * pThisFace[2];
	UINT64 n64ThisFace[2];
	int nThisFace[2];
	GetFaceFromPosition(n64CheckFace, m_nMaxLevel, dirCheckFace, pThisFace, n64ThisFace, nThisFace);
	Element * pTempFace = GetElementFace(pThisFace, nThisFace, dirCheckFace);
	CheckFace2(pThisFace, n64ThisFace, nThisFace, dirCheckFace, pTempFace);
}

void OcTree::CheckFace2(UCHAR * pThisFace[2], UINT64 n64ThisFace[2], int nThisFace[2], int dirCheckFace, Element * pTempFace)
{
	for (int i = 0; i < 2; i++)
	{
		Element * pTempCell = GetElementCell(pThisFace[i]);
		if (m_nType == 52 || m_nType == 55) {
			m_cFindPointer = i;
			m_cFindBackPointer = dirCheckFace * 2 + (1 - i);
		}
		if (pTempFace->m_bSkeleton || pTempFace->m_cPointer != i)
			CheckCellDetail( n64ThisFace[i], nThisFace[i], pTempCell );
	}
}

void OcTree::CheckCell2(UINT64 n64CheckCell, int nCheckCell)
{
	return;

	UCHAR * pThisCell;
	UINT64 n64ThisCell;
	int nThisCell;
	// GetCellFromPosition(n64CheckCell, m_nMaxLevel, pThisCell, n64ThisCell, nThisCell);
	// CheckCell2(pThisCell, n64ThisCell, nThisCell, pTempCell);
}

void OcTree::CheckCell2(UCHAR * pThisCell, UINT64 n64ThisCell, int nThisCell, Element * pTempCell)
{
	return;
}

void OcTree::CheckPointDetail(UINT64 n64CheckPoint, Element * pCheckPoint)
{
}

void OcTree::CheckEdgeDetail(UINT64 n64CheckEdge, int nCheckEdge, int dirCheckEdge, Element * pCheckEdge)
{
	switch (m_nType) {
	case 48:
		if ( pCheckEdge->m_bObject && pCheckEdge->m_bSkeleton)
			m_bIsSeperating = false;
		break;
	case 49:
		if ( pCheckEdge->m_bObject && !pCheckEdge->m_bSkeleton) {
			pair< hash_set<unsigned int>::iterator, bool > pr;
			pr = m_hashElement.insert( (unsigned int)pCheckEdge );
			if ( pr.second ) {
				// printf("Edge : (%16I64x, %d, %d)\n", n64CheckEdge, nCheckEdge, dirCheckEdge);
				if ( !m_bComputeOnly ) {
					m_listChanging.push_back(SkeletonElement(n64CheckEdge, nCheckEdge, dirCheckEdge, 1, pCheckEdge));
				}
				else 
					m_pCurrentSkeletonElement->m_n64Size += GetEdgeVolume(nCheckEdge);
				PushWitnessEdge(n64CheckEdge, nCheckEdge, dirCheckEdge, pCheckEdge->m_cPointer);
			}
		}
		break;
	case 50:
		if ( m_pCurrentSkeletonElement->m_nDimension == 0 ) {
			if ( !pCheckEdge->m_bSkeleton ) {
				pair< hash_map<unsigned int, unsigned int>::iterator, bool > pr;
				pr = m_hashElementExtend.insert( pair<unsigned int, unsigned int>((unsigned int)pCheckEdge, 0) );
				if ( pr.second && m_nFatherStatus == 0 ) {
				} else {
					(pr.first)->second = 1;
				}
				PushWitnessEdge(n64CheckEdge, nCheckEdge, dirCheckEdge, pCheckEdge->m_cPointer);
			} else if ( pCheckEdge->m_bSkeleton && !pCheckEdge->m_bObject ) {
				pair< hash_map<unsigned int, unsigned int>::iterator, bool > pr;
				pr = m_hashElementExtend.insert( pair<unsigned int, unsigned int>((unsigned int)pCheckEdge, 0) );
				if ( pr.second && m_nFatherStatus == 0 ) {
					hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashDualSkeleton.find((unsigned int)pCheckEdge);
					if ( itTemp != m_hashDualSkeleton.end() && itTemp->second->m_bValid ) {
						m_pCurrentSkeletonElement->m_listCompatible.push_back(itTemp->second);
					}
				} else {
					(pr.first)->second = 1;
				}
			}
		}
		break;
	case 51:
		if ( !pCheckEdge->m_bSkeleton) {
			pair< hash_set<unsigned int>::iterator, bool > pr;
			pr = m_hashElement.insert( (unsigned int)pCheckEdge );
			if ( pr.second ) {
				//printf("Edge : (%16I64x, %d, %d)\n", n64CheckEdge, nCheckEdge, dirCheckEdge);
				m_listChanging.push_back(SkeletonElement(n64CheckEdge, nCheckEdge, dirCheckEdge, 1, pCheckEdge));
				PushWitnessEdge(n64CheckEdge, nCheckEdge, dirCheckEdge, pCheckEdge->m_cPointer);
			}
		}
		break;
	case 52:
		if ( m_pCurrentSkeletonElement->m_nDimension == 0 ) {
			if ( !pCheckEdge->m_bSkeleton) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pCheckEdge );
				if ( pr.second ) {
					//printf("Edge : (%16I64x, %d, %d)\n", n64CheckEdge, nCheckEdge, dirCheckEdge);
					m_pFindCurrentElement = pCheckEdge;
					PushWitnessEdge(n64CheckEdge, nCheckEdge, dirCheckEdge, pCheckEdge->m_cPointer);
				}
			} else if ( pCheckEdge == m_pFindElement ) {
				m_pFindCurrentElement = pCheckEdge;
				ReversePathTrackPath();
				m_pFindElement = NULL;
			}
		}
		break;
	case 53:
		if ( pCheckEdge->m_bObject && !pCheckEdge->m_bSkeleton ) {
			UINT64 * pThis = GetFootPoint(pCheckEdge);
			UINT64 * pThat = GetFootPoint(m_pSimpleElement);
			if (*pThat == INT64_INVALID) {
				*pThat = *pThis;
			} else {
				if (Distance(pThis, &m_n64SimpleElement) > Distance(pThat, &m_n64SimpleElement)) {
					*pThat = *pThis;
				}
			}
		}
		break;
	case 54:
		if ( !pCheckEdge->m_bSkeleton ) {
			pair< hash_set<unsigned int>::iterator, bool > pr;
			pr = m_hashElement.insert( (unsigned int)pCheckEdge );
			if ( pr.second ) {
				//printf("Edge : (%16I64x, %d, %d)\n", n64CheckEdge, nCheckEdge, dirCheckEdge);
				PushWitnessEdge(n64CheckEdge, nCheckEdge, dirCheckEdge, pCheckEdge->m_cPointer);
			}
		} else if ( pCheckEdge->m_bSkeleton && !pCheckEdge->m_bObject ) {
			pair< hash_set<unsigned int>::iterator, bool > pr;
			pr = m_hashElement.insert( (unsigned int)pCheckEdge );
			if ( pr.second ) {
				hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashDualSkeleton.find((unsigned int)pCheckEdge);
				if ( itTemp != m_hashDualSkeleton.end() ) {
					itTemp->second->m_bValid = false;
				}
			}
		}
		break;
	case 55:
		if ( m_pCurrentSkeletonElement->m_nDimension == 0 && pCheckEdge->m_bObject && pCheckEdge->m_bSkeleton && !pCheckEdge->m_bSketch ) {
			m_nBelong ++;
			if (m_nBelong == 1) {
				hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashSkeleton.find((unsigned int)pCheckEdge);
				if ( itTemp != m_hashSkeleton.end() ) {
					m_pBelong = itTemp->second;
					m_pBelong->m_cPointer = m_cFindPointer;
					m_pBelong->m_cBackPointer = m_cFindBackPointer;
				}
			}
		}
		break;
	case 56:
		if ( !pCheckEdge->m_bObject && pCheckEdge->m_bSkeleton ) {
			m_listGeneratingSet.push_back(SkeletonElement(n64CheckEdge, nCheckEdge, dirCheckEdge, 1, pCheckEdge));
		}
		break;
	case 57:
		if ( !pCheckEdge->m_bObject ) {
			pair< hash_set<unsigned int>::iterator, bool > pr;
			pr = m_hashElement.insert( (unsigned int)pCheckEdge );
			if ( pr.second ) {
				if ( !pCheckEdge->m_bSkeleton) {
					//printf("Edge : (%16I64x, %d, %d)\n", n64CheckEdge, nCheckEdge, dirCheckEdge);
					PushWitnessEdge(n64CheckEdge, nCheckEdge, dirCheckEdge, pCheckEdge->m_cPointer);
				} else {
					hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashSkeleton.find((unsigned int)pCheckEdge);
					if ( itTemp != m_hashSkeleton.end() ) {
						itTemp->second->m_bValid = false;
					}
				}
			}
		}
		break;
	}
}

void OcTree::CheckFaceDetail(UINT64 n64CheckFace, int nCheckFace, int dirCheckFace, Element * pCheckFace)
{
	switch (m_nType) {
	case 48:
		if ( pCheckFace->m_bObject && pCheckFace->m_bSkeleton)
			m_bIsSeperating = false;
		break;
	case 49:
		if ( pCheckFace->m_bObject && !pCheckFace->m_bSkeleton) {
			pair< hash_set<unsigned int>::iterator, bool > pr;
			pr = m_hashElement.insert( (unsigned int)pCheckFace );
			if ( pr.second ) {
				// printf("Face : (%16I64x, %d, %d)\n", n64CheckFace, nCheckFace, dirCheckFace);
				if ( !m_bComputeOnly ) {
					m_listChanging.push_back(SkeletonElement(n64CheckFace, nCheckFace, dirCheckFace, 2, pCheckFace));
				}
				else
					m_pCurrentSkeletonElement->m_n64Size += GetFaceVolume(nCheckFace);
				PushWitnessFace(n64CheckFace, nCheckFace, dirCheckFace, pCheckFace->m_cPointer);
			}
		}
		break;
	case 50:
		if ( m_pCurrentSkeletonElement->m_nDimension == 1 ) {
			if ( !pCheckFace->m_bSkeleton ) {
				pair< hash_map<unsigned int, unsigned int>::iterator, bool > pr;
				pr = m_hashElementExtend.insert( pair<unsigned int, unsigned int>((unsigned int)pCheckFace, 0) );
				if ( pr.second && m_nFatherStatus == 0 ) {
				} else {
					(pr.first)->second = 1;
				}
				PushWitnessFace(n64CheckFace, nCheckFace, dirCheckFace, pCheckFace->m_cPointer);
			} else if ( pCheckFace->m_bSkeleton && !pCheckFace->m_bObject ) {
				pair< hash_map<unsigned int, unsigned int>::iterator, bool > pr;
				pr = m_hashElementExtend.insert( pair<unsigned int, unsigned int>((unsigned int)pCheckFace, 0) );
				if ( pr.second && m_nFatherStatus == 0 ) {
					hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashDualSkeleton.find((unsigned int)pCheckFace);
					if ( itTemp != m_hashDualSkeleton.end() && itTemp->second->m_bValid ) {
						m_pCurrentSkeletonElement->m_listCompatible.push_back(itTemp->second);
					}
				} else {
					(pr.first)->second = 1;
				}
			}
		}
		break;
	case 51:
		if ( !pCheckFace->m_bSkeleton) {
			pair< hash_set<unsigned int>::iterator, bool > pr;
			pr = m_hashElement.insert( (unsigned int)pCheckFace );
			if ( pr.second ) {
				//printf("Face : (%16I64x, %d, %d)\n", n64CheckFace, nCheckFace, dirCheckFace);
				m_listChanging.push_back(SkeletonElement(n64CheckFace, nCheckFace, dirCheckFace, 2, pCheckFace));
				PushWitnessFace(n64CheckFace, nCheckFace, dirCheckFace, pCheckFace->m_cPointer);
			}
		}
		break;
	case 52:
		if ( m_pCurrentSkeletonElement->m_nDimension == 1 ) {
			if ( !pCheckFace->m_bSkeleton) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pCheckFace );
				if ( pr.second ) {
					//printf("Face : (%16I64x, %d, %d)\n", n64CheckFace, nCheckFace, dirCheckFace);
					m_pFindCurrentElement = pCheckFace;
					PushWitnessFace(n64CheckFace, nCheckFace, dirCheckFace, pCheckFace->m_cPointer);
				}
			} else if ( pCheckFace == m_pFindElement ) {
				m_pFindCurrentElement = pCheckFace;
				ReversePathTrackPath();
				m_pFindElement = NULL;
			}
		}
		break;
	case 53:
		if ( pCheckFace->m_bObject && !pCheckFace->m_bSkeleton ) {
			UINT64 * pThis = GetFootPoint(pCheckFace);
			UINT64 * pThat = GetFootPoint(m_pSimpleElement);
			if (*pThat == INT64_INVALID) {
				*pThat = *pThis;
			} else {
				if (Distance(pThis, &m_n64SimpleElement) > Distance(pThat, &m_n64SimpleElement)) {
					*pThat = *pThis;
				}
			}
		}
		break;
	case 54:
		if ( !pCheckFace->m_bSkeleton ) {
			pair< hash_set<unsigned int>::iterator, bool > pr;
			pr = m_hashElement.insert( (unsigned int)pCheckFace );
			if ( pr.second ) {
				//printf("Face : (%16I64x, %d, %d)\n", n64CheckFace, nCheckFace, dirCheckFace);
				PushWitnessFace(n64CheckFace, nCheckFace, dirCheckFace, pCheckFace->m_cPointer);
			}
		} else if ( pCheckFace->m_bSkeleton && !pCheckFace->m_bObject ) {
			pair< hash_set<unsigned int>::iterator, bool > pr;
			pr = m_hashElement.insert( (unsigned int)pCheckFace );
			if ( pr.second ) {
				hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashDualSkeleton.find((unsigned int)pCheckFace);
				if ( itTemp != m_hashDualSkeleton.end() ) {
					itTemp->second->m_bValid = false;
				}
			}
		}
		break;
	case 55:
		if ( m_pCurrentSkeletonElement->m_nDimension == 1 && pCheckFace->m_bObject && pCheckFace->m_bSkeleton && !pCheckFace->m_bSketch ) {
			m_nBelong ++;
			if (m_nBelong == 1) {
				hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashSkeleton.find((unsigned int)pCheckFace);
				if ( itTemp != m_hashSkeleton.end() ) {
					m_pBelong = itTemp->second;
					m_pBelong->m_cPointer = m_cFindPointer;
					m_pBelong->m_cBackPointer = m_cFindBackPointer;
				}
			}
		}
		break;
	case 56:
		if ( !pCheckFace->m_bObject && pCheckFace->m_bSkeleton ) {
			m_listGeneratingSet.push_back(SkeletonElement(n64CheckFace, nCheckFace, dirCheckFace, 2, pCheckFace));
		}
		break;
	case 57:
		if ( !pCheckFace->m_bObject ) {
			pair< hash_set<unsigned int>::iterator, bool > pr;
			pr = m_hashElement.insert( (unsigned int)pCheckFace );
			if ( pr.second ) {
				if ( !pCheckFace->m_bSkeleton) {
					//printf("Face : (%16I64x, %d, %d)\n", n64CheckFace, nCheckFace, dirCheckFace);
					PushWitnessFace(n64CheckFace, nCheckFace, dirCheckFace, pCheckFace->m_cPointer);
				} else {
					hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashSkeleton.find((unsigned int)pCheckFace);
					if ( itTemp != m_hashSkeleton.end() ) {
						itTemp->second->m_bValid = false;
					}
				}
			}
		}
		break;
	}
}

void OcTree::CheckCellDetail(UINT64 n64CheckCell, int nCheckCell, Element * pCheckCell)
{
	switch (m_nType) {
	case 48:
		if ( pCheckCell->m_bObject && pCheckCell->m_bSkeleton)
			m_bIsSeperating = false;
		break;
	case 49:
		if ( pCheckCell->m_bObject && !pCheckCell->m_bSkeleton) {
			pair< hash_set<unsigned int>::iterator, bool > pr;
			pr = m_hashElement.insert( (unsigned int)pCheckCell );
			if ( pr.second ) {
				// printf("Cell : (%16I64x, %d)\n", n64CheckCell, nCheckCell);
				if ( !m_bComputeOnly ) {
					m_listChanging.push_back(SkeletonElement(n64CheckCell, nCheckCell, 0, 3, pCheckCell));
				}
				else 
					m_pCurrentSkeletonElement->m_n64Size += GetFaceVolume(nCheckCell);
				PushWitnessCell(n64CheckCell, nCheckCell, pCheckCell->m_cPointer);
			}
		}
		break;
	case 50:
		if ( m_pCurrentSkeletonElement->m_nDimension == 2 ) {
			if ( !pCheckCell->m_bSkeleton ) {
				pair< hash_map<unsigned int, unsigned int>::iterator, bool > pr;
				pr = m_hashElementExtend.insert( pair<unsigned int, unsigned int>((unsigned int)pCheckCell, 0) );
				if ( pr.second && m_nFatherStatus == 0 ) {
				} else {
					(pr.first)->second = 1;
				}
				PushWitnessCell(n64CheckCell, nCheckCell, pCheckCell->m_cPointer);
			} else if ( pCheckCell->m_bSkeleton && !pCheckCell->m_bObject ) {
				pair< hash_map<unsigned int, unsigned int>::iterator, bool > pr;
				pr = m_hashElementExtend.insert( pair<unsigned int, unsigned int>((unsigned int)pCheckCell, 0) );
				if ( pr.second && m_nFatherStatus == 0 ) {
					hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashDualSkeleton.find((unsigned int)pCheckCell);
					if ( itTemp != m_hashDualSkeleton.end() && itTemp->second->m_bValid ) {
						m_pCurrentSkeletonElement->m_listCompatible.push_back(itTemp->second);
					}
				} else {
					(pr.first)->second = 1;
				}
			}
		}
		break;
	case 51:
		if (!pCheckCell->m_bSkeleton) {
			pair< hash_set<unsigned int>::iterator, bool > pr;
			pr = m_hashElement.insert( (unsigned int)pCheckCell );
			if ( pr.second ) {
				//printf("Cell : (%16I64x, %d)\n", n64CheckCell, nCheckCell);
				m_listChanging.push_back(SkeletonElement(n64CheckCell, nCheckCell, 0, 3, pCheckCell));
				PushWitnessCell(n64CheckCell, nCheckCell, pCheckCell->m_cPointer);
			}
		}
		break;
	case 52:
		if ( m_pCurrentSkeletonElement->m_nDimension == 2 ) {
			if (!pCheckCell->m_bSkeleton) {
				pair< hash_set<unsigned int>::iterator, bool > pr;
				pr = m_hashElement.insert( (unsigned int)pCheckCell );
				if ( pr.second ) {
					//printf("Cell : (%16I64x, %d)\n", n64CheckCell, nCheckCell);
					m_pFindCurrentElement = pCheckCell;
					PushWitnessCell(n64CheckCell, nCheckCell, pCheckCell->m_cPointer);
				}
			} else if ( pCheckCell == m_pFindElement ) {
				m_pFindCurrentElement = pCheckCell;
				ReversePathTrackPath();
				m_pFindElement = NULL;
			}
		}
		break;
	case 53:
		if ( pCheckCell->m_bObject && !pCheckCell->m_bSkeleton ) {
			UINT64 * pThis = GetFootPoint(pCheckCell);
			UINT64 * pThat = GetFootPoint(m_pSimpleElement);
			if (*pThat == INT64_INVALID) {
				*pThat = *pThis;
			} else {
				if (Distance(pThis, &m_n64SimpleElement) > Distance(pThat, &m_n64SimpleElement)) {
					*pThat = *pThis;
				}
			}
		}
		break;
	case 54:
		if ( !pCheckCell->m_bSkeleton ) {
			pair< hash_set<unsigned int>::iterator, bool > pr;
			pr = m_hashElement.insert( (unsigned int)pCheckCell );
			if ( pr.second ) {
				//printf("Cell : (%16I64x, %d)\n", n64CheckCell, nCheckCell);
				PushWitnessCell(n64CheckCell, nCheckCell, pCheckCell->m_cPointer);
			}
		} else if ( pCheckCell->m_bSkeleton && !pCheckCell->m_bObject ) {
			pair< hash_set<unsigned int>::iterator, bool > pr;
			pr = m_hashElement.insert( (unsigned int)pCheckCell );
			if ( pr.second ) {
				hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashDualSkeleton.find((unsigned int)pCheckCell);
				if ( itTemp != m_hashDualSkeleton.end() ) {
					itTemp->second->m_bValid = false;
				}
			}
		}
		break;
	case 55:
		if ( m_pCurrentSkeletonElement->m_nDimension == 2 && pCheckCell->m_bObject && pCheckCell->m_bSkeleton && !pCheckCell->m_bSketch ) {
			m_nBelong ++;
			if (m_nBelong == 1) {
				hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashSkeleton.find((unsigned int)pCheckCell);
				if ( itTemp != m_hashSkeleton.end() ) {
					m_pBelong = itTemp->second;
					m_pBelong->m_cPointer = m_cFindPointer;
					m_pBelong->m_cBackPointer = m_cFindBackPointer;
				}
			}
		}
		break;
	case 56:
		if ( !pCheckCell->m_bObject && pCheckCell->m_bSkeleton ) {
			m_listGeneratingSet.push_back(SkeletonElement(n64CheckCell, nCheckCell, 0, 3, pCheckCell));
		}
		break;
	case 57:
		if ( !pCheckCell->m_bObject ) {
			pair< hash_set<unsigned int>::iterator, bool > pr;
			pr = m_hashElement.insert( (unsigned int)pCheckCell );
			if ( pr.second ) {
				if ( !pCheckCell->m_bSkeleton) {
					//printf("Cell : (%16I64x, %d)\n", n64CheckCell, nCheckCell);
					PushWitnessCell(n64CheckCell, nCheckCell, pCheckCell->m_cPointer);
				} else {
					hash_map<unsigned int, SkeletonElement *>::const_iterator itTemp = m_hashSkeleton.find((unsigned int)pCheckCell);
					if ( itTemp != m_hashSkeleton.end() ) {
						itTemp->second->m_bValid = false;
					}
				}
			}
		}
		break;
	}
}
