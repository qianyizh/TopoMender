#include "Topomender.h"

Topomender::Topomender(string meshStr,string sogStr,string sktStr,string outputStr)
{
	m_strMeshFile = meshStr;
	m_strSOGFile = sogStr;
	m_strSktFile = sktStr;
	m_strOutputFile = outputStr;
}

bool Topomender::testValid()
{
	if(!(m_strMeshFile.substr(m_strMeshFile.length()-4) == ".obj")) return false;
	if(!(m_strSOGFile.substr(m_strSOGFile.length()-4) == ".sog")) return false;
	if(!(m_strSktFile.substr(m_strSktFile.length()-4) == ".axs")) return false;
	if(!(m_strOutputFile.substr(m_strOutputFile.length()-4) == ".obj")) return false;
	return true;
}

bool Topomender::Initialize()
{
	m_pRenderMesh = new RenderMesh();
	if(!m_pRenderMesh->LoadFrom(m_strMeshFile))
	{
		return false;
	}
	//if(!m_cOcTree.LoadFromSOG(m_strSOGFile)) return false;
	if(!m_cSkeleton.LoadFrom(m_strSktFile)) {
		return false;
	}
	m_cOcTree.m_cConstant.Initialize();
	return true;
}

void Topomender::Go()
{
	// load OcTree
	OcTree * pInitOcTree = new OcTree();
	if ( pInitOcTree->LoadFromSOG( m_strSOGFile ) == false ) {
		delete pInitOcTree;
		return;
	}
	pInitOcTree->m_cConstant.Initialize();
	pInitOcTree->Sketching(& m_cSkeleton);
#ifdef OCTREE_LEVEL_RESTRICT
	pInitOcTree->LevelRestrict();
#endif
	pInitOcTree->SetStorages();
	OcTree * pTempOcTree = new OcTree();
	pTempOcTree->DuplicateOcTree(pInitOcTree);
	delete pInitOcTree;
	m_cOcTree.LoadFromOcTree( pTempOcTree );
	delete pTempOcTree;
	m_cOcTree.RecSketchingCheck(m_cOcTree.m_pRoot, 0, 0);
#ifdef CONSOLE_OUTPUT
	m_cOcTree.PrintInfo();
	m_cOcTree.m_cMemoryManager.PrintInfo();
	printf("Initialization successfully.\n");
	m_cOcTree.PrintSketchingInfo();
	printf("\n========================== Thinning Process ============================\n");
#endif
	m_cOcTree.m_bCleanSkeleton = true;
	m_cOcTree.Thin();
	m_cOcTree.DualThin();
	m_cOcTree.PrintSkeletonInfo();
#ifdef CONSOLE_OUTPUT
	printf("\n========================== Topology Repair =============================\n");
#endif
	m_cOcTree.TopologyRepair();
	// m_cOcTree.PrintSketchingInfo();
}

void Topomender::Contour()
{
	m_cOcTree.Contouring( m_strOutputFile, m_strSOGFile );
}
