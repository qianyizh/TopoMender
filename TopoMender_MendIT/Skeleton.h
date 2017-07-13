#ifndef _SKELETON
#define _SKELETON

#include "resources.h"
#include "Geometry.h"
using namespace std;

#define MAX_LAYERS 10
#define MAX_DEPTH 6

class PointInfo {
public:
	Vector3D m_vecLayer[MAX_LAYERS];
	Vector3D m_vPosition;
	int m_nLayer;
	int m_nMaxLayer;
	int m_nDepth;
public:
	PointInfo(); 
	~PointInfo(){}
	Vector3D MidPosition();
	Vector3D DepthPosition();
};

class LineSegment {
public:
	Vector3D m_vPos[2];
	int m_nLayer;
public:
	LineSegment() {}
	LineSegment(Vector3D v1, Vector3D v2, int l);
	~LineSegment() {}
};

class LineInfo {
public:
	int m_iPoint[2];
	vector<LineSegment> m_vecSegment;
public:
	LineInfo() {}
	LineInfo(int iStart, int iEnd);
	~LineInfo() {}
};

class Skeleton {
public:
	enum NextOperation {
		NextOpr_None,
		NextOpr_NewPT,
		NextOpr_NewPTOutside,
		NextOpr_NewPTDepth,
		NextOpr_SelectPT,
		NextOpr_NewLinePT
	};
public:
	vector<LineInfo> m_vecLine;
	vector<PointInfo> m_vecPoint;
	int m_iPickPoint;
	NextOperation m_enumNext;
	PointInfo m_cFuturePoint;
	LineInfo m_cFutureLine;
	int m_iFutureLine;
	int m_iFuturePoint;
	bool m_bGoodFutureLine;
public:
	Skeleton();
	~Skeleton(){}
	bool LoadFrom(string strFile);
	void Show();
};

#endif
