#ifndef _RENDERMESH
#define _RENDERMESH

#include "resources.h"
#include "BoundingBox.h"
#include "FileWriter.h"
using namespace std;

class RenderVertex {
public:
	float m_fPosition[3];
};

class RenderFace {
public:
	int m_iVertex[3];
	float m_fNormal[3];
};

class RenderMesh {
public:
	BoundingBox m_cBoundingBox;
	vector<RenderVertex> m_vVertex;
	vector<RenderFace> m_vFace;
public:
	RenderMesh(){}
	~RenderMesh(){}
	bool LoadFrom(string strFileName);
	void SaveTo(string strFileName);
	void ReCalcNormal();
};


#endif