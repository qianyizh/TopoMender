#ifndef _OCTREE
#define _OCTREE

#include "Constant.h"
#include "FileWriter.h"
#include "HashMap.h"
#include "Helper.h"
#include "MemoryManager.h"
#include "OcTreeSkeleton.h"
#include "SingleHashMap.h"
#include "Skeleton.h"
#include "TimeMeter.h"
#include "Vector3D.h"
using namespace std;
using namespace stdext;

// extern ofstream fout;

class OcTree {
public:
	//////////////////////////////////////////////////////////////////////////
	// variables
	//////////////////////////////////////////////////////////////////////////
	// OcTree root node  *** Important ***
	UCHAR * m_pRoot;
	// help variables
	Constant m_cConstant;
	MemoryManager m_cMemoryManager;
	int m_nType;
	UCHAR * m_pInfiniteNode;
	UINT64 m_n64Infinite;
	// OcTree properties
	int m_nMaxLevel;
	CVector3D m_vCorner;
	double m_dbLength;
	double m_dbNodeLength;
	// variants for counting
	int m_nCountCell;
	int m_nCountFace;
	int m_nCountEdge;
	int m_nCountPoint;
	int m_nSketchCell;
	int m_nSketchFace;
	int m_nSketchEdge;
	int m_nSketchPoint;
	// variants for thinning process
	int m_nStep;
	int m_nTestOut;
	int m_nTestLastOut;
	bool m_bCountOnly;
	bool m_bCleanSkeleton;
	// variants for I/O and contouring
	FileWriter m_cWriter;
	FILE * m_pSOGFile;
	string m_strSOGFile;
	HashMap * m_pHashMap;
	SingleHashMap * m_pSingleHashMap;
	int m_nRegularCount;
	int m_nIrregularCount;
	int m_nIrregularUnchangedCell;
	int m_nVertexNum;
	int m_nFaceNum;
	int m_nContourPass;
	bool m_bEnableRead;
	// variants for sketching
	Skeleton * m_pSkeleton;
	bool m_bBorderCheck;
	// variants for topology repair
	list<SkeletonElement> m_listSkeleton;
	list<SkeletonElement> m_listDualSkeleton;
	list<SkeletonElement> m_listGeneratingSet;
	SkeletonElement * m_pCurrentSkeletonElement;
	SkeletonElement * m_pMinSkeletonElement;
	hash_set<unsigned int> m_hashElement;
	bool m_bIsSeperating;
	bool m_bComputeOnly;
	bool m_bFirstTime;
	list<SkeletonElement> m_listChanging;
	//list<SkeletonElement *> m_listCompatible;
	hash_map<unsigned int, SkeletonElement *> m_hashDualSkeleton;
	hash_map<unsigned int, SkeletonElement *> m_hashSkeleton;
	hash_map<unsigned int, unsigned int> m_hashElementExtend;			// 0 -- only visited once;  1 -- circled
	unsigned int m_nFatherStatus;
	Element * m_pFindElement;
	CHAR m_cFindPointer;
	CHAR m_cFindBackPointer;
	Element * m_pFindCurrentElement;
	int m_nBelong;
	SkeletonElement * m_pBelong;
	// variants for thinning extension
	Element * m_pSimpleElement;
	UINT64 m_n64SimpleElement;
	UINT64 m_n64Threshold;
	// variants for debug
	bool m_bCorrect;
public:
	// OCTREE construction
	OcTree();
	~OcTree();

	void RecNode(UCHAR * pNode);

	//////////////////////////////////////////////////////////////////////////
	// construction functions
	//////////////////////////////////////////////////////////////////////////
	bool LoadFromSOG(string strSOGFile);
	void RecLoadFromSOG(UCHAR ** ppNode, FILE * pFile);

	void DuplicateOcTree(OcTree * pOcTree);
	void RecDuplicateOcTree(UCHAR ** ppNode, UCHAR * pOcTreeNode, OcTree * pOcTree);

	void LoadFromOcTree(OcTree * pOcTree);
	void RecLoadFromOcTree(UCHAR ** ppNode, UCHAR * pOcTreeNode, OcTree * pOcTree);

	//////////////////////////////////////////////////////////////////////////
	// recursive functions
	//////////////////////////////////////////////////////////////////////////
	void Traversal();
	void RecTraversalCell(UCHAR * pNode, UINT64 n64Position, int nLevel);
	void RecTraversalFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace);
	void RecTraversalEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge);
	void RecTraversalPoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2]);
	void TraversalWithoutPosition();
	void RecTraversalCell(UCHAR * pNode, int nLevel);
	void RecTraversalFace(UCHAR * pNode[2], int nLevel[2], int dirFace);
	void RecTraversalEdge(UCHAR * pNode[2][2], int nLevel[2][2], int dirEdge);
	void RecTraversalPoint(UCHAR * pNode[2][2][2], int nLevel[2][2][2]);

	//////////////////////////////////////////////////////////////////////////
	// contouring functions
	//////////////////////////////////////////////////////////////////////////
	void Contouring(string strFile, string strSOGFile);
	void RecRegularTest(UCHAR * pNode);
	void RecContouringEdge(UCHAR * pCoNode, UINT64 n64CoPosition, int nCoLevel, UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, int direction, list<int> & listIndex);

	//////////////////////////////////////////////////////////////////////////
	// thinning functions
	//////////////////////////////////////////////////////////////////////////
	void Thin();
	bool RecThinCell(UCHAR * pNode, int nLevel);
	bool RecThinFace(UCHAR * pNode[2], int nLevel[2], int dirFace);
	bool RecThinEdge(UCHAR * pNode[2][2], int nLevel[2][2], int dirEdge);
	bool RecThinPoint(UCHAR * pNode[2][2][2], int nLevel[2][2][2]);
	void DualThin();
	bool DualThinStep();
	bool RecDualThinCell(UCHAR * pNode, int nLevel);
	bool RecDualThinFace(UCHAR * pNode[2], int nLevel[2], int dirFace);
	bool RecDualThinEdge(UCHAR * pNode[2][2], int nLevel[2][2], int dirEdge);
	bool RecDualThinPoint(UCHAR * pNode[2][2][2], int nLevel[2][2][2]);

	//////////////////////////////////////////////////////////////////////////
	// thinning extension functions
	//////////////////////////////////////////////////////////////////////////
	bool RecThinCell(UCHAR * pNode, UINT64 n64Position, int nLevel);
	bool RecThinFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace);
	bool RecThinEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge);
	bool RecThinPoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2]);

	//////////////////////////////////////////////////////////////////////////
	// containing check functions
	//////////////////////////////////////////////////////////////////////////
	bool CheckPointForCell(UINT64 n64CoCell, int nCoCell, UINT64 n64Point);
	bool CheckEdgeForCell(UINT64 n64CoCell, int nCoCell, UINT64 n64Edge, int nEdge, int dirEdge);
	bool CheckFaceForCell(UINT64 n64CoCell, int nCoCell, UINT64 n64Face, int nFace, int dirFace);
	bool CheckCellForCell(UINT64 n64CoCell, int nCoCell, UINT64 n64Cell, int nCell);
	bool CheckPointForFace(UINT64 n64CoFace, int nCoFace, int dirCoFace, UINT64 n64Point);
	bool CheckEdgeForFace(UINT64 n64CoFace, int nCoFace, int dirCoFace, UINT64 n64Edge, int nEdge, int dirEdge);
	bool CheckFaceForFace(UINT64 n64CoFace, int nCoFace, int dirCoFace, UINT64 n64Face, int nFace, int dirFace);
	bool CheckCellForFace(UINT64 n64CoFace, int nCoFace, int dirCoFace, UINT64 n64Cell, int nCell);
	bool CheckPointForEdge(UINT64 n64CoEdge, int nCoEdge, int dirCoEdge, UINT64 n64Point);
	bool CheckEdgeForEdge(UINT64 n64CoEdge, int nCoEdge, int dirCoEdge, UINT64 n64Edge, int nEdge, int dirEdge);
	bool CheckFaceForEdge(UINT64 n64CoEdge, int nCoEdge, int dirCoEdge, UINT64 n64Face, int nFace, int dirFace);
	bool CheckCellForEdge(UINT64 n64CoEdge, int nCoEdge, int dirCoEdge, UINT64 n64Cell, int nCell);
	bool CheckPointForPoint(UINT64 n64CoPoint, UINT64 n64Point);
	bool CheckEdgeForPoint(UINT64 n64CoPoint, UINT64 n64Edge, int nEdge, int dirEdge);
	bool CheckFaceForPoint(UINT64 n64CoPoint, UINT64 n64Face, int nFace, int dirFace);
	bool CheckCellForPoint(UINT64 n64CoPoint, UINT64 n64Cell, int nCell);

	//////////////////////////////////////////////////////////////////////////
	// containing functions
	//////////////////////////////////////////////////////////////////////////
	void CheckPoint(UINT64 n64CheckPoint);
	void RecCheckPointCell(UCHAR * pNode, UINT64 n64Position, int nLevel, UINT64 n64CheckPoint);
	void RecCheckPointFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, UINT64 n64CheckPoint);
	void RecCheckPointEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, UINT64 n64CheckPoint);
	void RecCheckPointPoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2], UINT64 n64CheckPoint);
	void CheckEdge(UINT64 n64CheckEdge, int nCheckEdge, int dirCheckEdge);
	void RecCheckEdgeCell(UCHAR * pNode, UINT64 n64Position, int nLevel, UINT64 n64CheckEdge, int nCheckEdge, int dirCheckEdge);
	void RecCheckEdgeFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, UINT64 n64CheckEdge, int nCheckEdge, int dirCheckEdge);
	void RecCheckEdgeEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, UINT64 n64CheckEdge, int nCheckEdge, int dirCheckEdge);
	void RecCheckEdgePoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2], UINT64 n64CheckEdge, int nCheckEdge, int dirCheckEdge);
	void CheckFace(UINT64 n64CheckFace, int nCheckFace, int dirCheckFace);
	void RecCheckFaceCell(UCHAR * pNode, UINT64 n64Position, int nLevel, UINT64 n64CheckFace, int nCheckFace, int dirCheckFace);
	void RecCheckFaceFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, UINT64 n64CheckFace, int nCheckFace, int dirCheckFace);
	void RecCheckFaceEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, UINT64 n64CheckFace, int nCheckFace, int dirCheckFace);
	void RecCheckFacePoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2], UINT64 n64CheckFace, int nCheckFace, int dirCheckFace);
	void CheckCell(UINT64 n64CheckCell, int nCheckCell);
	void RecCheckCellCell(UCHAR * pNode, UINT64 n64Position, int nLevel, UINT64 n64CheckCell, int nCheckCell);
	void RecCheckCellFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, UINT64 n64CheckCell, int nCheckCell);
	void RecCheckCellEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, UINT64 n64CheckCell, int nCheckCell);
	void RecCheckCellPoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2], UINT64 n64CheckCell, int nCheckCell);

	//////////////////////////////////////////////////////////////////////////
	// belonging functions
	//////////////////////////////////////////////////////////////////////////
	void CheckPoint2(UINT64 n64CheckPoint);
	void CheckEdge2(UINT64 n64CheckEdge, int nCheckEdge, int dirCheckEdge);
	void CheckFace2(UINT64 n64CheckFace, int nCheckFace, int dirCheckFace);
	void CheckCell2(UINT64 n64CheckCell, int nCheckCell);
	void CheckPoint2(UCHAR * pThisPoint[2][2][2], UINT64 n64ThisPoint[2][2][2], int nThisPoint[2][2][2], Element * pTempPoint);
	void CheckEdge2(UCHAR * pThisEdge[2][2], UINT64 n64ThisEdge[2][2], int nThisEdge[2][2], int dirCheckEdge, Element * pTempEdge);
	void CheckFace2(UCHAR * pThisFace[2], UINT64 n64ThisFace[2], int nThisFace[2], int dirCheckFace, Element * pTempFace);
	void CheckCell2(UCHAR * pThisCell, UINT64 n64ThisCell, int nThisCell, Element * pTempCell);
	void CheckPointDetail(UINT64 n64CheckPoint, Element * pCheckPoint);
	void CheckEdgeDetail(UINT64 n64CheckEdge, int nCheckEdge, int dirCheckEdge, Element * pCheckEdge);
	void CheckFaceDetail(UINT64 n64CheckFace, int nCheckFace, int dirCheckFace, Element * pCheckFace);
	void CheckCellDetail(UINT64 n64CheckCell, int nCheckCell, Element * pCheckCell);

	//////////////////////////////////////////////////////////////////////////
	// sketching functions
	//////////////////////////////////////////////////////////////////////////
	void Sketching(Skeleton * pSkeleton);
	void RecSketching(UCHAR * pNode, UINT64 n64Position, int nLevel, int nLine);
	void RecSketchingSplit(UCHAR ** ppNode, UINT64 n64Position, int nLevel, int nLine);
	bool RecSketchingMerge(UCHAR * pNode, UINT64 n64Position, int nLevel);
	bool RecSketchingMergeCheckEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge);
	bool RecSketchingMergeCheckFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace);
	void RecSketchingCheck(UCHAR * pNode, UINT64 n64Position, int nLevel);
	bool Intersection(UINT64 n64Position, int nLevel, int nLine);
	bool Intersection(UINT64 n64Position, int nLevel, int dirFace, int nLine);

	//////////////////////////////////////////////////////////////////////////
	// topology repair functions
	//////////////////////////////////////////////////////////////////////////
	void TopologyRepair();
	void BuildSkeletonHash();
	void DualComputeGeneratingSet(SkeletonElement & cDualElement);
	void ComputeGeneratingSet(SkeletonElement & cElement);
	void ComputeVastGeneratingSet(SkeletonElement & cElement);
	void SetInvalidGeneratingSet(SkeletonElement & cElement);
	void SetVastInvalidGeneratingSet(SkeletonElement & cElement);
	void DualSetInvalidGeneratingSet(SkeletonElement & cDualElement);
	void DualSetVastInvalidGeneratingSet(SkeletonElement & cDualElement);
	void ModifySkeletonComputeGeneratingSet(SkeletonElement & cElement);
	void ReversePathComputeGeneratingSet(SkeletonElement & cElement);
	void ReversePathTrackPath();
	bool CheckSkeletonElemement(SkeletonElement & cElement);
	bool DualCheckSkeletonElemement(SkeletonElement & cDualElement);
	void PushWitnessEdge(UINT64 n64ThisEdge, int nThisEdge, int dirThisEdge, UCHAR cPointer);
	void PushWitnessFace(UINT64 n64ThisFace, int nThisFace, int dirThisFace, UCHAR cPointer);
	void PushWitnessCell(UINT64 n64ThisCell, int nThisCell, UCHAR cPointer);
	void RecPushWitnessFaceEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, UCHAR cCheck);
	void RecPushWitnessCellFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, UCHAR cCheck);
	void DualPushWitnessPoint(UCHAR * pNode[2][2][2], UINT64 n64Position[2][2][2], int nLevel[2][2][2], UCHAR cPointer);
	void DualPushWitnessEdge(UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, UCHAR cPointer);
	void DualPushWitnessFace(UCHAR * pNode[2], UINT64 n64Position[2], int nLevel[2], int dirFace, UCHAR cPointer);

	//////////////////////////////////////////////////////////////////////////
	// thinning for pure skeleton
	//////////////////////////////////////////////////////////////////////////
	void ThinPureSkeletonElement();
	void DualThinPureSkeletonElement();
	void ThinPureSkeleton();
	void DualThinPureSkeleton();
	bool CheckSimpleElement(SkeletonElement & cElement);
	bool DualCheckSimpleElement(SkeletonElement & cDualElement);

	//////////////////////////////////////////////////////////////////////////
	// print functions
	//////////////////////////////////////////////////////////////////////////
	void PrintInfo();
	void PrintSkeletonInfo();
	void PrintSketchingInfo();
	void PrintPureSkeletonInfo();

	//////////////////////////////////////////////////////////////////////////
	// restrict functions
	//////////////////////////////////////////////////////////////////////////
	void LevelRestrict();
	bool RecLevelRestrictCheckCell(UCHAR * pNode, int nLevel);
	bool RecLevelRestrictCheckFace(UCHAR * pNode[2], int nLevel[2], int dirFace);
	void RecLevelRestrictNode(UCHAR ** ppNode);

	//////////////////////////////////////////////////////////////////////////
	// helper functions
	//////////////////////////////////////////////////////////////////////////
	// common helpers
	bool ParseSOGFile(FILE * pFile, CVector3D & vCorner, double & dbSize);
	// storage helpers
	void SetStorages();
	void GetStorageFace(int nLevel[2], int dirFace, int & nI, int & nIndex);
	void GetStorageEdge(int nLevel[2][2], int dirEdge, int & nI, int & nJ, int & nIndex);
	void GetStoragePoint(int nlevel[2][2][2], int & nI, int & nJ, int & nK, int & nIndex);
	int CountElement( UCHAR * pNode, int nEnd );
	Element * GetElementCell(UCHAR * pNode);
	Element * GetElementFace(UCHAR * pNode[2], int nLevel[2],
		int dirFace, int & nI, int & nIndex);
	Element * GetElementEdge(UCHAR * pNode[2][2], int nLevel[2][2],
		int dirEdge, int & nI, int & nJ, int & nIndex);
	Element * GetElementPoint(UCHAR * pNode[2][2][2], int nLevel[2][2][2],
		int & nI, int & nJ, int & nK, int & nIndex);
	Element * GetElementFace(UCHAR * pNode[2], int nLevel[2], int dirFace);
	Element * GetElementEdge(UCHAR * pNode[2][2], int nLevel[2][2], int dirEdge);
	Element * GetElementPoint(UCHAR * pNode[2][2][2], int nLevel[2][2][2]);
	bool CheckStorageFace(UCHAR * pNode[2]);
	bool CheckStorageEdge(UCHAR * pNode[2][2]);
	// seek & search helpers
	bool InNode(UCHAR * pNode, UINT64 n64Position, int nLevel, UINT64 n64Search);
	void Search(UCHAR * pNode, UINT64 n64Position, int nLevel, UINT64 n64Search,
		int nSearch, UCHAR * & pRtnNode, UINT64 & n64RtnPosition, int & nRtnLevel);
	void GetCellFromPosition(UINT64 n64Search, int nSearch, UCHAR * & pRtnNode,
		UINT64 & n64RtnPosition, int & nRtnLevel);
	void GetFaceFromPosition(UINT64 n64Search, int nSearch, int dirFace,
		UCHAR * pRtnNode[2], UINT64 n64RtnPosition[2], int nRtnLevel[2]);
	void GetEdgeFromPosition(UINT64 n64Search, int nSearch, int dirEdge, 
		UCHAR * pRtnNode[2][2], UINT64 n64RtnPosition[2][2], int nRtnLevel[2][2]);
	void GetPointFromPosition(UINT64 n64Search, int nSearch, UCHAR * pRtnNode[2][2][2],
		UINT64 n64RtnPosition[2][2][2], int nRtnLevel[2][2][2]);
	UINT64 LeafPosition(UINT64 n64Position, int nLevel, int i, int j, int k);
	UINT64 LeafPosition(UINT64 n64Position, int nLevel, int iIndex);
	void GetFacePosition(UINT64 n64Position[2], int nLevel[2], int dirFace,
		int nI, int iIndex, UINT64 & n64RtnPosition);
	void GetEdgePosition(UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge,
		int nI, int nJ, int iIndex, UINT64 & n64RtnPosition);
	void GetPointPosition(UINT64 n64Position[2][2][2], int nLevel[2][2][2],
		int nI, int nJ, int nK, UINT64 & n64RtnPosition);
	void GetFacePosition(UINT64 n64Position[2], int nLevel[2], int dirFace,
		UINT64 & n64Face, int & nFace);
	void GetEdgePosition(UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge,
		UINT64 & n64Edge, int & nEdge);
	void GetPointPosition(UINT64 n64Position[2][2][2], int nLevel[2][2][2], UINT64 & n64Point);
	UINT64 GetCellCenterPosition(UINT64 n64Position, int nLevel);
	UINT64 GetFaceCenterPosition(UINT64 n64Position[2], int nLevel[2], int dirFace);
	UINT64 GetEdgeCenterPosition(UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge);
	UINT64 GetPointCenterPosition(UINT64 n64Position[2][2][2], int nLevel[2][2][2]);
	UINT64 NodePosition(UINT64 n64Position, int nLevel); 
	void Leaf(UCHAR * pNode, UINT64 n64Position, int nLevel, int i, int j, int k,
		UCHAR * & pNext, UINT64 & nNextPos, int & nNextLevel);
	void Leaf(UCHAR * pNode, int nLevel, int i, int j, int k, UCHAR * & pNext, int & nNextLevel);
	UCHAR * GetLeafNode(UCHAR * pNode, int i, int j, int k);
	void InsertIndex(list<int> & listIndex, int iInsert);
	UINT GetIndex(UCHAR * pNode, UINT64 n64Position, UINT64 n64Point);
	UINT64 GetPointVolume();
	UINT64 GetEdgeVolume(int nLevel);
	UINT64 GetFaceVolume(int nLevel);
	UINT64 GetCellVolume(int nLevel);
	UINT64 GetObjectSkeletonElementSize(SkeletonElement * pSkeletonElement);
	void UpdateObjectSkeletonElementSize(SkeletonElement * pSkeletonElement, 
		SkeletonElement * pAddElement);
	// debug helpers
	void PrintNodeInfo(UCHAR * pNode);
	UINT64 * GetFootPoint(Element * pElement);
	UINT64 Distance(UINT64 * pA, UINT64 * pB);

#ifdef BETTER_GENERATING_SET
	void StepThreshold(UINT64 & n64Threshold)
	{
		n64Threshold += BETTER_GENERATING_SET;
		m_n64Threshold = n64Threshold * n64Threshold;
	}
#endif

};

#endif