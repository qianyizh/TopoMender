// This is the main project file for VC++ application project 
// generated using an Application Wizard.

#include "stdafx.h"
#include "Constant.h"
#include "OcTree.h"

//#using <mscorlib.dll>

//using namespace System;

void PrintHelp()
{
	printf("TopoMender program for robustly handle removal.\n");
	printf("Author : Qian-Yi Zhou (zqy@mails.tsinghua.edu.cn)\n");
	printf("Version : 1.0, last updated on Jul, 21, 2006\n");
	printf("\n");
	printf("Usage :   TopoMender [-skt] SOGFile ThinThresh GrowThresh ModelFile\n");
	printf("Example : TopoMender eight.sog 1.0 1.0 eight.ply\n");
	printf("\n");
	printf("SOGFile :    Input file, must be of SOG file format\n");
	printf("ModelFile :  Output file, could be of either OBJ or PLY file format\n");
	printf("ThinThresh : A threshold used in thinning process\n");
	printf("GrowThresh : A threshold used in growing process\n");
	printf("-skt :       Indicates a corresponding SKT file would be generated\n");
	printf("\n");
}

bool ParseSOGFile(FILE * pFile, double & dbX, double & dbY, double & dbZ, double & dbSize)
{
	char pHeadBuf[128];
	if (pFile == NULL || fread(pHeadBuf, 1, 128, pFile) != 128) 
		return false;
	const char * strHead = "SOG.Format 1.0";
	if (strncmp(pHeadBuf, strHead, strlen(strHead)) == 0) {			// 1.0 SOG File Format
		float fCoord[4];
		memcpy(fCoord, pHeadBuf + 15, sizeof(float) * 4);
		dbX = fCoord[0];
		dbY = fCoord[1];
		dbZ = fCoord[2];
		dbSize = fCoord[3];
		return true;
	} else {
		return false;
	}
}

bool FindString(int nMin, int nMax, char * argv[], const char * strFind)
{
	for (int i = nMin; i <= nMax; i++)
		if (stricmp(argv[i], strFind) == 0)
			return true;
	return false;
}

int _tmain(int argc , char *argv[ ])
{
	CConstant cConstant;
	TCHAR pFileName[MAX_PATH];
	::GetModuleFileName(NULL, pFileName, MAX_PATH);
	CString strPath(pFileName);
	strPath.Truncate(strPath.ReverseFind('\\'));
	CString strTemp = strPath + "\\Regular.txt";
	cConstant.LoadRegularTable(strTemp);
	strTemp = strPath + "\\RegCons.txt";
	cConstant.LoadRegularTableEx(strTemp);
	strTemp = strPath + "\\HashSize.txt";
	cConstant.LoadHashSize(strTemp);
	CString strSOG, strOBJ, strCFG, strSKT;
	double dbX, dbY, dbZ, dbSize;
	double dbBreak, dbFill;
	FILE * pSOGFile;

	// -h and -help
	if (argc < 5 || FindString(1, argc - 5, argv, "-HELP") || FindString(1, argc - 5, argv, "-H")) {
		PrintHelp();
		return 0;
	}
	// sourse and destination
	strSOG = CString(argv[argc - 4]);
	dbBreak = atof(argv[argc - 3]);
	dbFill = atof(argv[argc - 2]);
	strOBJ = CString(argv[argc - 1]);
	// -skt
	if (FindString(1, argc - 5, argv, "-SKT")) {
		strSKT = strOBJ.Left(strOBJ.GetLength() - 3) + "skt";
	} else {
		strSKT = "";
	}

	printf("TopoMender program for robustly handle removal.\n");
	printf("Author : Qian-Yi Zhou (zqy@mails.tsinghua.edu.cn)\n");
	printf("Version : 1.0, last updated on Jul, 21, 2006\n");
	printf("\n");
	printf("========================= Initializaion & Input ========================\n");
	printf("Input from SOG file :\n\t%s\n", strSOG.GetBuffer(0));
	COcTree * pInitOcTree = new COcTree();
	pInitOcTree->RegisterConstant(&cConstant);
	pSOGFile = fopen(strSOG, "rb");
	if (ParseSOGFile(pSOGFile, dbX, dbY, dbZ, dbSize) == false) {
		fclose(pSOGFile);
		printf("ERROR : SOG file format error!\nProgram terminate.\n");
		return 0;
	} else {
		pInitOcTree->LoadFromSOG(pSOGFile);
		fclose(pSOGFile);
		printf("Origin : (%6.4f, %6.4f, %6.4f), Dimension : %6.4f\n", dbX, dbY, dbZ, dbSize);
	}
	pInitOcTree->PrintInfo();
	pInitOcTree->InitFaceTags();

	COcTree * pOcTree = new COcTree();
	pOcTree->RegisterConstant(&cConstant);
	pOcTree->SetPositionSize(dbX, dbY, dbZ, dbSize);
	pOcTree->LoadFromOcTree(pInitOcTree);
	pOcTree->m_pMemoryManager->PrintInfo();
	delete pInitOcTree;
	printf("Initialization successfully.\n");

	printf("\n========================= Main Break Process ===========================\n");

	if (dbBreak <= 0.0f) {
		printf("Break threshold <= 0.0, break process omitted.\n");
	} else {
		printf("Phase 0 - Initialization...\n\n");
		pOcTree->RecFixTags(pOcTree->m_pRoot, 0);

		printf("Phase 1 - Thinning process :");
		pOcTree->Thin();

		printf("Phase 2 - Create Minimum Spanning Tree :\n");
		pOcTree->CreateMST((float)dbBreak, strSKT);

		printf("Phase 3 - Grow cutting plane...\n");
		pOcTree->Grow();

		printf("Phase 4 - Finalization...\n\n");
		pOcTree->m_nType = 7;
		pOcTree->RecTraversalCell(pOcTree->m_pRoot, 0, 0);
		pOcTree->RecFixTags(pOcTree->m_pRoot, 2);

		pOcTree->PrintInfo();
		printf("\n");
	}

	printf("\n========================= Main Fill Process ===========================\n");

	if (dbFill <= 0.0f) {
		printf("Fill threshold <= 0.0, fill process omitted.\n");
	} else {
		printf("Phase 0 - Initialization...\n\n");
		pOcTree->RecFixTags(pOcTree->m_pRoot, 0);

		printf("Phase 1 - Thinning process :");
		pOcTree->DualThin();

		printf("Phase 2 - Create Minimum Spanning Tree :\n");
		pOcTree->DualCreateMST((float)dbFill, strSKT);

		printf("Phase 3 - Grow cutting plane...\n");
		pOcTree->DualGrow();

		printf("Phase 4 - Finalization...\n\n");
		pOcTree->m_nType = 5;
		pOcTree->Traversal();
		pOcTree->RecFixTags(pOcTree->m_pRoot, 3);

		pOcTree->PrintInfo();
		printf("\n");
	}

	printf("\n========================= Contouring & Output ==========================\n");
	pOcTree->ContouringQD(strOBJ, strSOG);
	printf("Output successfully to file :\n\t%s\n", strOBJ.GetBuffer(0));

	delete pOcTree;

	getchar();
	return 0;
}
