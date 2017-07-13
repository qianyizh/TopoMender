#ifndef _TOPOMENDER
#define _TOPOMENDER

#include "resources.h"
#include "Constant.h"
#include "OcTree.h"
#include "RenderMesh.h"
#include "Skeleton.h"
using namespace std;
 
// extern ofstream fout;

class Topomender {
public:
	string m_strMeshFile;
	string m_strSOGFile;
	string m_strSktFile;
	string m_strOutputFile;
	RenderMesh * m_pRenderMesh;
	OcTree m_cOcTree;
	Skeleton m_cSkeleton;
public:
	Topomender(){}
	Topomender(string meshStr,string sogStr,string sktStr,string outputStr);
	bool testValid();
	bool Initialize();
	// Important Steps
	void Contour();
	void Go();
};

#endif