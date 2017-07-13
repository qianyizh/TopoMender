#pragma once

#include "Constant.h"
#include "MemoryManager.h"
#include "HashMap.h"
#include "SingleHashMap.h"
#include "Vector3D.h"
#include "EdgeHeap.h"
#include "FaceHeap.h"
#include "TimeMeter.h"
#include <list>
using namespace std;

#define INT64_INVALID 0xffffffffffffffff
#define MAX_LEVEL 19
#define NLENGTH(i) (1 << (MAX_LEVEL - (i)))
#define INT64TRANS(i, j, k) (((UINT64)(i) << 40) | ((UINT64)(j) << 20) | ((UINT64)(k)))
#define INT64GETI(n) (((n) >> 40) & (0xfffff))
#define INT64GETJ(n) (((n) >> 20) & (0xfffff))
#define INT64GETK(n) ((n) & (0xfffff))
#define INT64TRANSD(i, j, k, d) (((UINT64)(d) << 60) | ((UINT64)(i) << 40) | ((UINT64)(j) << 20) | ((UINT64)(k)))
#define INT64COMBD(n, d) (((UINT64)(d) << 60) | n)
#define INT64GETD(n) ((int)(((n) >> 60) & (0xf)))
#define INT64GETN(n) ((n) & (0xfffffffffffffff))
#define GET_CHANGED(n) ((n) & 0xc000000000000000)
#define GET_CHANGED_A(n) ((n) & 0x4000000000000000)
#define GET_CHANGED_B(n) ((n) & 0x8000000000000000)
#define GET_POINT_INSIDE(n, i) (((n) >> ((i) + 4)) & 1)
#define GET_EDGE_INSIDE(n, i) (((n) >> ((i) + 12)) & 1)
#define GET_FACE_INSIDE(n, i) (((n) >> ((i) + 24)) & 1)
#define GET_CELL_INSIDE(n, i) (((n) >> ((i) + 30)) & 1)
#define GET_POINT_SIMPLE(n, i) (((n) >> ((i) * 2 + 0)) & 3)
#define GET_EDGE_SIMPLE(n, i) (((n) >> ((i) * 2 + 16)) & 3)
#define GET_FACE_SIMPLE(n, i) (((n) >> ((i) * 2 + 40)) & 3)
#define GET_CELL_SIMPLE(n, i) (((n) >> ((i) * 2 + 52)) & 3)
#define GET_EDGE_RI(n, i) (((n) >> ((i) * 2 + 32)) & 3)
#define GET_FACE_RIT(n, i) (((n) >> ((i) + 56)) & 1)
#define GET_LAST_TIME(n) ((*(n)) & 0x4)
#define GET_LAST_FIX(n) ((*(n)) & 0x8)
#define NOT_LEAF(n) ((*(n) & 0x3) == 0)
#define NOT_INFINITE(n) ((*(n) & 0x3) != 3)
#define POSITION_TRANS_X(n) (0.5 * m_dbNodeLength * (INT64GETI(n)) + m_vCorner.pVec[0])
#define POSITION_TRANS_Y(n) (0.5 * m_dbNodeLength * (INT64GETJ(n)) + m_vCorner.pVec[1])
#define POSITION_TRANS_Z(n) (0.5 * m_dbNodeLength * (INT64GETK(n)) + m_vCorner.pVec[2])
#define POSITION_TRANS(n) (CVector3D(POSITION_TRANS_X(n), POSITION_TRANS_Y(n), POSITION_TRANS_Z(n)))

//////////////////////////////////////////////////////////////////////////
// COcTree::m_nType, which determines the behaviors done during each traversal.
//
// -1-- Check the correctness of an ESO
// 0 -- Count genus
// 1 -- Count number
// 2 -- Create Vertex, count only
// 3 -- Create Vertex, real create
// 4 -- Create Face
// 5 -- Correctly set I/O tags for points/edges/faces in EVERY cell
// 6 -- Find a start position to create MST
// 7 -- Same as 5, but set Simple tags which are 3 instead of I/O tags
// 8 -- Initilize dual create MST
//////////////////////////////////////////////////////////////////////////

typedef struct _Inter_Node {
	UCHAR m_cType;
	UCHAR * m_pNode[8];
} INTER_NODE;

typedef struct _Empty_Node {
	UINT64 m_n64Type;
	UINT64 m_n64SimpleTag;
	UINT m_nVertex;
} EMPTY_NODE;

typedef struct _Leaf_Node {
	UINT64 m_n64Type;
	UINT64 m_n64SimpleTag;
	UINT m_nVertex;
} LEAF_NODE;

typedef struct _Retrace_Edge {
	float m_fWeight;
	UINT64 m_n64PositionEdge;
} RETRACE_EDGE;

typedef struct _Retrace_Edge_Outside {
	float m_fWeight;
} RETRACE_EDGE_OUTSIDE;

class COcTree
{
public:
	COcTree(void);
	~COcTree(void);

	CConstant * m_pConstant;
	CMemoryManager * m_pMemoryManager;

public:
	// variant indicates traversal type
	int m_nType;

	// OcTree properties
	int m_nMaxLevel;
	CVector3D m_vCorner;
	double m_dbScalar;
	double m_dbLength;
	double m_dbNodeLength;

	// OcTree root node  *** Important ***
	UCHAR * m_pRoot;

	// variants for counting
	int m_nCountCell;
	int m_nCountFace;
	int m_nCountEdge;
	int m_nCountPoint;

	// constant nodes indicate outside part
	EMPTY_NODE m_cInfiniteNode;

	// variants for input and output, including contouring process
	FILE * m_pWriteFile;
	FILE * m_pSOGFile;
	CHashMap * m_pHashMap;
	CSingleHashMap * m_pSingleHashMap;
	int m_nIrregularCount;
	int m_nIrregularUnchangedCell;
	int m_nVertexNum;
	int m_nFaceNum;
	bool m_bCountOnly;
	int m_nFileType;			// 0 - obj, 1 - ply

	// variants for thinning process
	int m_nStep;
	int m_nTestOut;
	int m_nTestLastOut;

	// variants for MST creation
	UCHAR * m_pPointStart[2][2][2];
	int m_nPointStart[2][2][2];
	UINT64 m_n64PointStart[2][2][2];

	// variants for growing process
	list<CEdgeHeapElement> m_listGrowSeedEdge;
	list<CFaceHeapElement> m_listGrowSeedFace;
	
public:
	void PrintNodeInfo(UCHAR * pNode) {
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode;
		printf("\n(%16I64x, %16I64x, %d)", pTempNode->m_n64Type, pTempNode->m_n64SimpleTag, pTempNode->m_nVertex);
	}
	void PrintNodeInfo(UCHAR * pNode, UINT64 n64Position, int nLevel) {
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode;
		printf("\n(%16I64x, %16I64x, %d) @ %I64x ~ %d", pTempNode->m_n64Type, pTempNode->m_n64SimpleTag, pTempNode->m_nVertex, n64Position, nLevel);
	}
	void PrintCellInfo(UCHAR * pNode, UINT64 n64Position, int nLevel) {
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode;
		printf("\nCell's info : I/O Tag = %I64d, Simple Tag = %I64d, and details below :", GET_CELL_INSIDE(pTempNode->m_n64Type, 0), GET_CELL_SIMPLE(pTempNode->m_n64SimpleTag, 0));
		PrintNodeInfo(pNode, n64Position, nLevel);
	}
	void PrintFaceInfo(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace) {
		int nI, nIndex;
		GetStorageFace(nLevel, dirFace, nI, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI];
		printf("\nFace's info : I/O Tag = %I64d, Simple Tag = %I64d, dirFace = %d, and details below :", GET_FACE_INSIDE(pTempNode->m_n64Type, nIndex), GET_FACE_SIMPLE(pTempNode->m_n64SimpleTag, nIndex), dirFace);
		for (int i = 0; i < 2; i++)
			PrintNodeInfo(pNode[i], n64Position[i], nLevel[i]);
	}
	void PrintEdgeInfo(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge) {
		int nI, nJ, nIndex;
		GetStorageEdge(nLevel, dirEdge, nI, nJ, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI][nJ];
		printf("\nEdge's info : I/O Tag = %I64d, Simple Tag = %I64d, dirEdge = %d, and details below :", GET_EDGE_INSIDE(pTempNode->m_n64Type, nIndex), GET_EDGE_SIMPLE(pTempNode->m_n64SimpleTag, nIndex), dirEdge);
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				PrintNodeInfo(pNode[i][j], n64Position[i][j], nLevel[i][j]);
	}
	void PrintPointInfo(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2]) {
		int nI, nJ, nK, nIndex;
		GetStoragePoint(nLevel, nI, nJ, nK, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI][nJ][nK];
		printf("\nPoint's info : I/O Tag = %I64d, Simple Tag = %I64d, and details below :", GET_POINT_INSIDE(pTempNode->m_n64Type, nIndex), GET_POINT_SIMPLE(pTempNode->m_n64SimpleTag, nIndex));
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					PrintNodeInfo(pNode[i][j][k], n64Position[i][j][k], nLevel[i][j][k]);
	}
	bool InNode(UCHAR * pNode, UINT64 n64Position, int nLevel, UINT64 n64Search) {
		return (
			(INT64GETI(n64Search) >> (MAX_LEVEL - nLevel)) == (INT64GETI(n64Position) >> (MAX_LEVEL - nLevel)) &&
			(INT64GETJ(n64Search) >> (MAX_LEVEL - nLevel)) == (INT64GETJ(n64Position) >> (MAX_LEVEL - nLevel)) &&
			(INT64GETK(n64Search) >> (MAX_LEVEL - nLevel)) == (INT64GETK(n64Position) >> (MAX_LEVEL - nLevel))
			);
	}
	void Search(UCHAR * pNode, UINT64 n64Position, int nLevel, UINT64 n64Search, int nSearch, UCHAR * & pRtnNode, UINT64 & n64RtnPosition, int & nRtnLevel) {
		if (!InNode(pNode, n64Position, nLevel, n64Search)) {
			pRtnNode = (UCHAR *)(&m_cInfiniteNode);
			n64RtnPosition = n64Search;
			nRtnLevel = -1;
		} else if (nSearch == nLevel || !NOT_LEAF(pNode)) {
			pRtnNode = pNode;
			n64RtnPosition = n64Position;
			nRtnLevel = nLevel;
		} else {
			UCHAR * pLeafNode;
			UINT64 n64Leaf;
			int nLeaf;
			Leaf(pNode, n64Position, nLevel, (int)(INT64GETI(n64Search)>>(MAX_LEVEL-nLevel-1))&1, (int)(INT64GETJ(n64Search)>>(MAX_LEVEL-nLevel-1))&1, (int)(INT64GETK(n64Search)>>(MAX_LEVEL-nLevel-1))&1, pLeafNode, n64Leaf, nLeaf);
			Search(pLeafNode, n64Leaf, nLeaf, n64Search, nSearch, pRtnNode, n64RtnPosition, nRtnLevel);
		}
	}
	void GetFaceFromPos(UINT64 n64Search, int nSearch, int dirFace, UCHAR * pRtnNode[2], UINT64 n64RtnPosition[2], int nRtnLevel[2]) {
		for (int i = 0; i < 2; i++)
		{
			switch (dirFace) {
			case 0:
				Search(m_pRoot, 0, 0, INT64TRANS(INT64GETI(n64Search)+i*2-1, INT64GETJ(n64Search)+1, INT64GETK(n64Search)+1), nSearch, pRtnNode[i], n64RtnPosition[i], nRtnLevel[i]);
				break;
			case 1:
				Search(m_pRoot, 0, 0, INT64TRANS(INT64GETI(n64Search)+1, INT64GETJ(n64Search)+i*2-1, INT64GETK(n64Search)+1), nSearch, pRtnNode[i], n64RtnPosition[i], nRtnLevel[i]);
				break;
			case 2:
				Search(m_pRoot, 0, 0, INT64TRANS(INT64GETI(n64Search)+1, INT64GETJ(n64Search)+1, INT64GETK(n64Search)+i*2-1), nSearch, pRtnNode[i], n64RtnPosition[i], nRtnLevel[i]);
				break;
			}
		}

	}
	void GetEdgeFromPos(UINT64 n64Search, int nSearch, int dirEdge, UCHAR * pRtnNode[2][2], UINT64 n64RtnPosition[2][2], int nRtnLevel[2][2]) {
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
			{
				switch(dirEdge) {
				case 0:
					Search(m_pRoot, 0, 0, INT64TRANS(INT64GETI(n64Search)+1, INT64GETJ(n64Search)+i*2-1, INT64GETK(n64Search)+j*2-1), nSearch, pRtnNode[i][j], n64RtnPosition[i][j], nRtnLevel[i][j]);
					break;
				case 1:
					Search(m_pRoot, 0, 0, INT64TRANS(INT64GETI(n64Search)+j*2-1, INT64GETJ(n64Search)+1, INT64GETK(n64Search)+i*2-1), nSearch, pRtnNode[i][j], n64RtnPosition[i][j], nRtnLevel[i][j]);
					break;
				case 2:
					Search(m_pRoot, 0, 0, INT64TRANS(INT64GETI(n64Search)+i*2-1, INT64GETJ(n64Search)+j*2-1, INT64GETK(n64Search)+1), nSearch, pRtnNode[i][j], n64RtnPosition[i][j], nRtnLevel[i][j]);
					break;
				}
			}
	}
	void GetPointFromPos(UINT64 n64Search, int nSearch, UCHAR * pRtnNode[2][2][2], UINT64 n64RtnPosition[2][2][2], int nRtnLevel[2][2][2]) {
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					Search(m_pRoot, 0, 0, INT64TRANS(INT64GETI(n64Search)+i*2-1, INT64GETJ(n64Search)+j*2-1, INT64GETK(n64Search)+k*2-1), nSearch, pRtnNode[i][j][k], n64RtnPosition[i][j][k], nRtnLevel[i][j][k]);
				}
	}
	void GetFacePos(UINT64 n64Position[2], int nLevel[2], int dirFace, int nI, int iIndex, UINT64 & n64RtnPosition) {
		n64RtnPosition = LeafPosition(n64Position[nI], nLevel[nI], CConstant::I_SHIFT_FP[CConstant::II_SHIFT_CF[dirFace][nI]][iIndex]);
	}
	void GetEdgePos(UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, int nI, int nJ, int iIndex, UINT64 & n64RtnPosition) {
		n64RtnPosition = LeafPosition(n64Position[nI][nJ], nLevel[nI][nJ], CConstant::I_SHIFT_EP[CConstant::II_SHIFT_CE[dirEdge][nI][nJ]][iIndex]);
	}
	void GetPointPos(UINT64 n64Position[2][2][2], int nLevel[2][2][2], int nI, int nJ, int nK, UINT64 & n64RtnPosition) {
		n64RtnPosition = LeafPosition(n64Position[nI][nJ][nK], nLevel[nI][nJ][nK], CConstant::II_SHIFT_CP[nI][nJ][nK]);
	}
	UINT64 NodePosition(UINT64 n64Position, int nLevel) {
		return n64Position + INT64TRANS(NLENGTH(nLevel + 1), NLENGTH(nLevel + 1), NLENGTH(nLevel + 1));
	}
	UINT64 FacePosition(UINT64 n64Position[2], int nLevel[2], int dirFace) {
		int nI, nIndex;
		UINT64 n64Face;
		GetStorageFace(nLevel, dirFace, nI, nIndex);
		GetFacePos(n64Position, nLevel, dirFace, nI, 0, n64Face);
		switch (dirFace) {
		case 0:
			return n64Face + INT64TRANS(0, NLENGTH(nLevel[nI] + 1), NLENGTH(nLevel[nI] + 1));
			break;
		case 1:
			return n64Face + INT64TRANS(NLENGTH(nLevel[nI] + 1), 0, NLENGTH(nLevel[nI] + 1));
			break;
		case 2:
			return n64Face + INT64TRANS(NLENGTH(nLevel[nI] + 1), NLENGTH(nLevel[nI] + 1), 0);
			break;
		}
		return n64Face;
	}
	UCHAR * LeafNode(UCHAR * pNode, int i, int j, int k) {
		return ((INTER_NODE *)(pNode))->m_pNode[i*4+j*2+k];
	}
	UINT64 LeafPosition(UINT64 n64Position, int nLevel, int i, int j, int k) {
		return INT64TRANS(i*NLENGTH(nLevel)+INT64GETI(n64Position), j*NLENGTH(nLevel)+INT64GETJ(n64Position), k*NLENGTH(nLevel)+INT64GETK(n64Position));
	}
	UINT64 LeafPosition(UINT64 n64Position, int nLevel, int iIndex) {
		int i = (iIndex >> 2) & 1;
		int j = (iIndex >> 1) & 1;
		int k = (iIndex) & 1;
		return INT64TRANS(i*NLENGTH(nLevel)+INT64GETI(n64Position), j*NLENGTH(nLevel)+INT64GETJ(n64Position), k*NLENGTH(nLevel)+INT64GETK(n64Position));
	}
	int LeafLevel(UCHAR * pNode, int nLevel) {
		return nLevel + 1;
	}
	void Leaf(UCHAR * pNode, UINT64 n64Position, int nLevel, int i, int j, int k, UCHAR * & pNext, UINT64 & nNextPos, int & nNextLevel) {
		if ((*pNode & 3) == 0) {
			pNext = ((INTER_NODE *)(pNode))->m_pNode[i*4+j*2+k];
			nNextPos = n64Position + INT64TRANS(i * NLENGTH(nLevel + 1), j * NLENGTH(nLevel + 1), k * NLENGTH(nLevel + 1));
			nNextLevel = nLevel + 1;
		} else {
			pNext = pNode;
			nNextPos = n64Position;
			nNextLevel = nLevel;
		}
	}
	void GetStorageFace(int nLevel[2], int dirFace, int & nI, int & nIndex) {
		int nMaxLevel = -1;
		for (int i = 0; i < 2; i++)
			if (nLevel[i] > nMaxLevel) {
				nI = i;
				nMaxLevel = nLevel[i];
			}
		nIndex = dirFace * 2 + (1 - nI);
	}
	void GetStorageEdge(int nLevel[2][2], int dirEdge, int & nI, int & nJ, int & nIndex) {
		int nMaxLevel = -1;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				if (nLevel[i][j] > nMaxLevel) {
					nI = i;
					nJ = j;
					nMaxLevel = nLevel[i][j];
				}
		nIndex = dirEdge * 4 + (1 - nI) * 2 + (1 - nJ);
	}
	void GetStoragePoint(int nlevel[2][2][2], int & nI, int & nJ, int & nK, int & nIndex) {
		int nMaxLevel = -1;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					if (nlevel[i][j][k] > nMaxLevel) {
						nI = i;
						nJ = j;
						nK = k;
						nMaxLevel = nlevel[i][j][k];
					}
		nIndex = (1 - nI) * 4 + (1 - nJ) * 2 + (1 - nK);
	}
	bool CheckStorageFace(UCHAR * pNode[2]) {
		if (pNode[0] == pNode[1])
			return false;
		else 
			return true;
	}
	bool CheckStorageEdge(UCHAR * pNode[2][2]) {
		if ((pNode[0][0] == pNode[0][1] && pNode[1][0] == pNode[1][1]) ||
			(pNode[0][0] == pNode[1][0] && pNode[0][1] == pNode[1][1]))
			return false;
		else
			return true;
	}
	UINT64 GetCellInside(UCHAR * pNode, int nLevel) {
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode;
		return GET_CELL_INSIDE(pTempNode->m_n64Type, 0);
	}
	UINT64 GetFaceInside(UCHAR * pNode[2], int nLevel[2], int dirFace) {
		int nI, nIndex;
		GetStorageFace(nLevel, dirFace, nI, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI];
		return GET_FACE_INSIDE(pTempNode->m_n64Type, nIndex);
	}
	UINT64 GetEdgeInside(UCHAR * pNode[2][2], int nLevel[2][2], int dirEdge) {
		int nI, nJ, nIndex;
		GetStorageEdge(nLevel, dirEdge, nI, nJ, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI][nJ];
		return GET_EDGE_INSIDE(pTempNode->m_n64Type, nIndex);
	}
	UINT64 GetPointInside(UCHAR * pNode[2][2][2], int nLevel[2][2][2]) {
		int nI, nJ, nK, nIndex;
		GetStoragePoint(nLevel, nI, nJ, nK, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI][nJ][nK];
		return GET_POINT_INSIDE(pTempNode->m_n64Type, nIndex);
	}
	void InsertIndex(list<int> & listIndex, int iInsert)
	{
		for (list<int>::iterator it = listIndex.begin(); it != listIndex.end(); it++)
			if (iInsert == (*it))
				return;
		listIndex.push_back(iInsert);
	}
	UINT GetIndex(UCHAR * pNode, UINT64 n64Position, UINT64 n64Point)
	{
		if (((LEAF_NODE *)pNode)->m_nVertex != 0xffffffff)
			return ((LEAF_NODE *)pNode)->m_nVertex;
		else
			return m_pHashMap->Get(n64Position, n64Point);
	}
	void WriteFileHeader() {
		switch(m_nFileType) {
		case 1:				//ply
			fprintf(m_pWriteFile, "ply\n");
			fprintf(m_pWriteFile, "format binary_big_endian 1.0\n");
			fprintf(m_pWriteFile, "element vertex %d\n", m_nVertexNum);
			fprintf(m_pWriteFile, "property float x\n");
			fprintf(m_pWriteFile, "property float y\n");
			fprintf(m_pWriteFile, "property float z\n");
			fprintf(m_pWriteFile, "element face %d\n", m_nFaceNum);
			fprintf(m_pWriteFile, "property list uchar int vertex_indices\n");
			fprintf(m_pWriteFile, "end_header\n");
			break;
		default:			// obj
			fprintf(m_pWriteFile, "# %d %d\n", m_nVertexNum, m_nFaceNum);
			break;
		}
	}
	void WriteFace(list<int> & listIndex)
	{
		int * pIndex = new int[listIndex.size()];
		list<int>::iterator itTemp;
		int l;
		for (itTemp = listIndex.begin(), l = 0; itTemp != listIndex.end(); itTemp++, l++)
			pIndex[l] = *itTemp;
		for (int j = 0; j < (int)listIndex.size() - 2; j++) {
			int nTemp = 3;
			switch(m_nFileType) {
			case 1:			// ply
				fwrite(&nTemp, 1, 1, m_pWriteFile);
				fwrite(&pIndex[0], 4, 1, m_pWriteFile);
				fwrite(&pIndex[j+1], 4, 1, m_pWriteFile);
				fwrite(&pIndex[j+2], 4, 1, m_pWriteFile);
				break;
			default:		// obj
				fprintf(m_pWriteFile, "f %d %d %d\n", pIndex[0]+1, pIndex[j+1]+1, pIndex[j+2]+1);
				break;
			}
		}
		delete []pIndex;
	}
	void WriteFace(list<int> & listIndex, int & nFaceNum)
	{
		int * pIndex = new int[listIndex.size()];
		list<int>::iterator itTemp;
		int l;
		for (itTemp = listIndex.begin(), l = 0; itTemp != listIndex.end(); itTemp++, l++)
			pIndex[l] = *itTemp;
		for (int j = 0; j < (int)listIndex.size() - 2; j++) {
			if (!m_bCountOnly) {
				int nTemp = 3;
				switch(m_nFileType) {
				case 1:			// ply
					reverse_fwrite((UCHAR *)&nTemp, 1, 1, m_pWriteFile);
					reverse_fwrite((UCHAR *)&pIndex[0], 4, 1, m_pWriteFile);
					reverse_fwrite((UCHAR *)&pIndex[j+1], 4, 1, m_pWriteFile);
					reverse_fwrite((UCHAR *)&pIndex[j+2], 4, 1, m_pWriteFile);
					break;
				default:		// obj
					fprintf(m_pWriteFile, "f %d %d %d\n", pIndex[0]+1, pIndex[j+1]+1, pIndex[j+2]+1);
					break;
				}
			}
			nFaceNum++;
		}
		delete []pIndex;
	}
	void WriteVertex(float fPos[3])
	{
		switch(m_nFileType) {
		case 1:			// ply
			reverse_fwrite((UCHAR *)fPos, 4, 3, m_pWriteFile);
			break;
		default:		// obj
			fprintf(m_pWriteFile, "v %12.10f %12.10f %12.10f\n", fPos[0], fPos[1], fPos[2]);
			break;
		}
	}
	void reverse_fwrite(UCHAR * pBuffer, int nSize, int nNum, FILE * pFile) {
		for (int i = 0; i < nNum; i++)
			for (UCHAR * pWrite = pBuffer + (i * nSize) + nSize - 1; pWrite >= pBuffer + (i * nSize); pWrite--)
				fwrite(pWrite, 1, 1, pFile);
	}

	void PrintInfo() {
		m_nType = 1;
		m_nCountCell = 0;
		m_nCountFace = 0;
		m_nCountEdge = 0;
		m_nCountPoint = 0;
		Traversal();
		printf("OcTree with %d cells, %d faces, %d edges, %d points.\n", m_nCountCell, m_nCountFace, m_nCountEdge, m_nCountPoint);
		m_nType = 0;
		m_nCountCell = 0;
		m_nCountFace = 0;
		m_nCountEdge = 0;
		m_nCountPoint = 0;
		Traversal();
		printf("Inside part contains %d cells, %d faces, %d edges, %d points.\n", m_nCountCell, m_nCountFace, m_nCountEdge, m_nCountPoint);
		printf("Total %d handles.\n", m_nCountEdge + m_nCountCell - m_nCountFace - m_nCountPoint + 1);
	}

	void SetScalar(double dbScalar) {		// a reversed function, just set a octree centered at (0, 0, 0), with scalar dbScalar
		m_dbScalar = dbScalar;
		m_dbLength = 2.0 / m_dbScalar;
		m_dbNodeLength = 2.0 / m_dbScalar / (double)NLENGTH(0);
		m_vCorner = CVector3D(- 1.0 / m_dbScalar, - 1.0 / m_dbScalar, - 1.0 / m_dbScalar);
	}

	void SetPositionSize(double dbX, double dbY, double dbZ, double dbSize) {
		m_dbScalar = 2.0 / dbSize;
		m_dbLength = dbSize;
		m_dbNodeLength = m_dbLength / (double)NLENGTH(0);
		m_vCorner = CVector3D(dbX, dbY, dbZ);
	}

	void SetObjectInside(UINT64 & n64Type, int iIndex, int nObjType, bool bInside) {
		if (bInside) {
			n64Type |= (1 << (iIndex + CConstant::OBJECT_START_POSITION[nObjType] + 4));
		} else {
			n64Type &= (~(1 << (iIndex + CConstant::OBJECT_START_POSITION[nObjType] + 4)));
		}
	}

	void SetObjectSimple(UINT64 & n64SimpleTag, int iIndex, int nObjType, UINT64 n64Type) {
		int nShift = iIndex * 2 + CConstant::OBJECT_START_POSITION[nObjType] * 2;
		n64SimpleTag &= (~((~n64Type & 3) << nShift));
		n64SimpleTag |= (n64Type << nShift);
	}

	void SetLastTime(UCHAR * pNode, bool bRtn) {
		if (bRtn) {
			(*pNode) |= (0x4);
		} else {
			(*pNode) &= (0xfb);
		}
	}

	void SetChangedA(UINT64 & n64Type, bool bChanged) {
		if (bChanged) {
			n64Type |= 0x4000000000000000;
		} else {
			n64Type &= 0xbfffffffffffffff;
		}
	}

	void SetChangedB(UINT64 & n64Type, bool bChanged) {
		if (bChanged) {
			n64Type |= 0x8000000000000000;
		} else {
			n64Type &= 0x7fffffffffffffff;
		}
	}

	void SetLastFix(UCHAR * pNode, bool bRtn) {
		if (bRtn) {
			(*pNode) |= (0x8);
		} else {
			(*pNode) &= (0xf7);
		}
	}
	void SetLastFix(UCHAR * pNode, int nRtn) {
		if (nRtn) {
			(*pNode) |= (0x8);
		} else {
			(*pNode) &= (0xf7);
		}
	}
	void SetEdgeRI(UINT64 & n64Type, int iIndex, UINT64 n64EdgeRI)	{
 		int nShift = iIndex * 2 + 32;
		n64Type &= (~((~n64EdgeRI & 3) << nShift));
		n64Type |= (n64EdgeRI << nShift);
	}

	RETRACE_EDGE * GetRetraceEdge(UCHAR * pNode, int iIndex) {
		LEAF_NODE * pTempLeaf = (LEAF_NODE *)pNode;
		int nCount = 0;
		for (int i = 0; i < iIndex; i++)
			if (GET_FACE_RIT(pTempLeaf->m_n64Type, i))
				nCount++;
		return ((RETRACE_EDGE *)(pNode + sizeof(LEAF_NODE) + nCount * sizeof(RETRACE_EDGE)));
	}

	float GetEdgeArea(UINT64 n64Position[2][2], int nLevel[2][2]) {
		UINT64 n64Node[2][2];
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				n64Node[i][j] = NodePosition(n64Position[i][j], nLevel[i][j]);
		CVector3D pVec[3];
		pVec[0] = POSITION_TRANS(n64Node[0][0]);
		pVec[1] = POSITION_TRANS(n64Node[0][1]);
		pVec[2] = POSITION_TRANS(n64Node[1][0]);
		double dbArea[2];
		dbArea[0] = GetArea(pVec);
		pVec[0] = POSITION_TRANS(n64Node[1][1]);
		dbArea[1] = GetArea(pVec);
		return ((float)(dbArea[0] + dbArea[1]));
	}

	double GetArea(CVector3D pVec[3]) {
		CVector3D vTemp = (pVec[1] - pVec[0]) ^ (pVec[2] - pVec[0]);
		return 0.5 * vTemp.length();
	}

	float GetFaceArea(int nLevel) {
		double dbLength = m_dbNodeLength * NLENGTH(nLevel);
		return ((float)(dbLength * dbLength));
	}

	int CheckEdgeNeighborFace(UCHAR * pNode[2][2], int nLevel[2][2], int dirEdge) {
		int nNum = 0;
		UCHAR * pNodeFace[2];
		int nFace[2];
		int dirFace, nFaceI, nFaceIndex;
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++) {
				pNodeFace[j] = pNode[i][j];
				nFace[j] = nLevel[i][j];
			}
			if (CheckStorageFace(pNodeFace)) {
				dirFace = (dirEdge + 2) % 3;
				GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
				if (GET_FACE_SIMPLE(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64SimpleTag, nFaceIndex) == 0)
					nNum++;
			}
			for (int j = 0; j < 2; j++) {
				pNodeFace[j] = pNode[j][i];
				nFace[j] = nLevel[j][i];
			}
			if (CheckStorageFace(pNodeFace)) {
				dirFace = (dirEdge + 1) % 3;
				GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
				if (GET_FACE_SIMPLE(((LEAF_NODE *)pNodeFace[nFaceI])->m_n64SimpleTag, nFaceIndex) == 0)
					nNum++;
			}
		}
		return nNum;
	}

	bool GetEdgeNeighborPoint(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, UCHAR * pRtnNode[2][2][2], UINT64 n64RtnPosition[2][2][2], int nRtnLevel[2][2][2]) {
		int nI, nJ, nIndex;
		GetStorageEdge(nLevel, dirEdge, nI, nJ, nIndex);
		UCHAR * pNodePoint[2][2][2];
		UINT64 n64Point[2][2][2];
		int nPoint[2][2][2];
		int nPointI, nPointJ, nPointK, nPointIndex;
		LEAF_NODE * pTempNode;
		int nRtn = 0;
		for (int l = 0; l < 2; l++)
		{
			UINT64 n64PointPosition = LeafPosition(n64Position[nI][nJ], nLevel[nI][nJ], CConstant::III_SHIFT_CEP[dirEdge][nI][nJ][l]);
			GetPointFromPos(n64PointPosition, MAX_LEVEL, pNodePoint, n64Point, nPoint);
			GetStoragePoint(nPoint, nPointI, nPointJ, nPointK, nPointIndex);
			pTempNode = (LEAF_NODE *)pNodePoint[nPointI][nPointJ][nPointK];
			if (GET_POINT_SIMPLE(pTempNode->m_n64SimpleTag, nPointIndex) == 2) {
				nRtn++;
				for (int i = 0; i < 2; i++)
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
						{
							pRtnNode[i][j][k] = pNodePoint[i][j][k];
							n64RtnPosition[i][j][k] = n64Point[i][j][k];
							nRtnLevel[i][j][k] = nPoint[i][j][k];
						}
			}
		}
		return (nRtn == 1);
	}

	float GetEdgeWeight(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge) {
		if (CheckEdgeNeighborFace(pNode, nLevel, dirEdge) > 0)
			return CConstant::FLOAT_INFINITE;
		UCHAR * pNodeFace[2];
		int nFace[2];
		float fWeight = 0.0;
		int nFaceI, nFaceIndex, dirFace;
		RETRACE_EDGE * pRetraceEdge;
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++) {
				pNodeFace[j] = pNode[i][j];
				nFace[j] = nLevel[i][j];
			}
			dirFace = (dirEdge + 2) % 3;
			if (CheckStorageFace(pNodeFace)) {
				GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
				pRetraceEdge = GetRetraceEdge(pNodeFace[nFaceI], nFaceIndex);
				fWeight += pRetraceEdge->m_fWeight;
			}
			for (int j = 0; j < 2; j++) {
				pNodeFace[j] = pNode[j][i];
				nFace[j] = nLevel[j][i];
			}
			dirFace = (dirEdge + 1) % 3;
			if (CheckStorageFace(pNodeFace)) {
				GetStorageFace(nFace, dirFace, nFaceI, nFaceIndex);
				pRetraceEdge = GetRetraceEdge(pNodeFace[nFaceI], nFaceIndex);
				fWeight += pRetraceEdge->m_fWeight;
			}
		}
		return GetEdgeArea(n64Position, nLevel) + fWeight;
	}

	int CheckFaceNeighborEdge(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace) {
		UINT64 n64Face;
		UCHAR * pNodeEdge[2][2];
		UINT64 n64Edge[2][2];
		int nEdge[2][2];
		int dirEdge;
		int nI, nIndex;

		int nNum = 0;

		GetStorageFace(nLevel, dirFace, nI, nIndex);
		LEAF_NODE * pTempNode = (LEAF_NODE *)pNode[nI];

		GetFacePos(n64Position, nLevel, dirFace, nI, 0, n64Face);
		dirEdge = (dirFace + 2) % 3;
		GetEdgeFromPos(n64Face, nLevel[nI], dirEdge, pNodeEdge, n64Edge, nEdge);
		nNum += RecCheckFaceNeighborEdge(pNodeEdge, n64Edge, nEdge, dirEdge);

		GetFacePos(n64Position, nLevel, dirFace, nI, 1, n64Face);
		dirEdge = (dirFace + 1) % 3;
		GetEdgeFromPos(n64Face, nLevel[nI], dirEdge, pNodeEdge, n64Edge, nEdge);
		nNum += RecCheckFaceNeighborEdge(pNodeEdge, n64Edge, nEdge, dirEdge);

		GetFacePos(n64Position, nLevel, dirFace, nI, 2, n64Face);
		dirEdge = (dirFace + 2) % 3;
		GetEdgeFromPos(n64Face, nLevel[nI], dirEdge, pNodeEdge, n64Edge, nEdge);
		nNum += RecCheckFaceNeighborEdge(pNodeEdge, n64Edge, nEdge, dirEdge);

		GetFacePos(n64Position, nLevel, dirFace, nI, 0, n64Face);
		dirEdge = (dirFace + 1) % 3;
		GetEdgeFromPos(n64Face, nLevel[nI], dirEdge, pNodeEdge, n64Edge, nEdge);
		nNum += RecCheckFaceNeighborEdge(pNodeEdge, n64Edge, nEdge, dirEdge);

		return nNum;
	}

	int RecCheckFaceNeighborEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge) {
		UCHAR * pNodeEdge[2][2];
		UINT64 n64Edge[2][2];
		int nEdge[2][2];
		int nI, nJ, nIndex;

		int nNum = 0;

		if (NOT_LEAF(pNode[0][0]) || NOT_LEAF(pNode[0][1]) || NOT_LEAF(pNode[1][0]) || NOT_LEAF(pNode[1][1])) {
			switch (dirEdge) {
			case 0:
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
							Leaf(pNode[j][k], n64Position[j][k], nLevel[i][j], i, 1-j, 1-k, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
					nNum += RecCheckFaceNeighborEdge(pNodeEdge, n64Edge, nEdge, dirEdge);
				}
				break;
			case 1:
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
							Leaf(pNode[j][k], n64Position[j][k], nLevel[i][j], 1-k, i, 1-j, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
					nNum += RecCheckFaceNeighborEdge(pNodeEdge, n64Edge, nEdge, dirEdge);
				}
				break;
			case 2:
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
						for (int k = 0; k < 2; k++)
							Leaf(pNode[j][k], n64Position[j][k], nLevel[i][j], 1-j, 1-k, i, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
					nNum += RecCheckFaceNeighborEdge(pNodeEdge, n64Edge, nEdge, dirEdge);
				}
				break;
			}
		} else {
			if (CheckStorageEdge(pNode)) {
				GetStorageEdge(nLevel, dirEdge, nI, nJ, nIndex);
				if (GET_EDGE_SIMPLE(((LEAF_NODE *)pNode[nI][nJ])->m_n64SimpleTag, nIndex) == 1)
					nNum = 1;
			}
		}

		return nNum;
	}

	float GetFaceWeight(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace) {
		if (CheckFaceNeighborEdge(pNode, n64Position, nLevel, dirFace) > 0)
			return CConstant::FLOAT_INFINITE;
		int nI, nIndex;
		float fFace = CConstant::FLOAT_INFINITE;

		if (CheckStorageFace(pNode)) {
			GetStorageFace(nLevel, dirFace, nI, nIndex);
			RETRACE_EDGE * pRetraceEdge = GetRetraceEdge(pNode[nI], nIndex);
			fFace = pRetraceEdge->m_fWeight;
		}

		return fFace;
	}

	bool GetFaceNeighborCell(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, int & nRtnCellIndex) {
		int nRtn = 0;

		for (int i = 0; i < 2; i++)
		{
			if (GET_CELL_SIMPLE(((LEAF_NODE *)pNode[i])->m_n64SimpleTag, 0) == 2) {
				nRtn ++;
				nRtnCellIndex = i;
			}
		}

		return (nRtn == 1);
	}

public:
	void RegisterConstant(CConstant * pConstant) { m_pConstant = pConstant; }
	void Clear();

	void LoadFromSOG(FILE * pFile);
	void RecLoadFromSOG(UCHAR ** ppNode, FILE * pFile, UINT64 n64Position, int nLevel);
	void RecLoadFromSOG(UCHAR ** ppNode, FILE * pFile);
	
	void LoadFromOcTree(COcTree * pOcTree);
	void RecLoadFromOcTree(UCHAR ** pNode, UCHAR * pOcTreeNode, COcTree * pOcTree);

	void ContouringQD(CString strFile, CString strSOGFile);

	void Traversal();
	void RecTraversalCell(UCHAR * pNode, UINT64 n64Position, int nLevel);
	void RecTraversalFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace);
	void RecTraversalEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge);
	void RecTraversalPoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2]);

	void InitFaceTags();
	void RecInitFaceTagsCell(UCHAR * pNode, UINT64 n64Position, int nLevel);
	void RecInitFaceTagsFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace);

	void RecRegularTest(UCHAR * pNode, int & nRegularCount);
	void RecCreateFaceFaceEdgeTraversal(UCHAR * pCoNode, UINT64 n64CoPosition, int nCoLevel, UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, int direction, list<int> & listIndex);

	void RecFixTags(UCHAR * pNode, int nType);

	void Thin();
	bool RecThinCell(UCHAR * pNode, UINT64 n64Position, int nLevel);
	bool RecThinFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace);
	bool RecThinEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge);
	bool RecThinPoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2]);
	void CreateMST(float fThreshold, CString strSKTFile);

	void Grow();
	void GrowByEdge(CEdgeHeapElement & cElement);

	void DualThin();
	bool DualThinStep();
	bool RecDualThinCell(UCHAR * pNode, UINT64 n64Position, int nLevel);
	bool RecDualThinFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace);
	bool RecDualThinEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge);
	bool RecDualThinPoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2]);
	void DualCreateMST(float fThreshold, CString strSKTFile);
	void RecPushFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, CFaceHeap & heapFace);

	void DualGrow();
	void DualGrowByFace(CFaceHeapElement & cElement);
	void RecDualGrowByEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge);
};
