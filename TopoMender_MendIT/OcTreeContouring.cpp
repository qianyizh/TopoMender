#include "OcTree.h"
using namespace std;

//////////////////////////////////////////////////////////////////////////
// contouring functions
//////////////////////////////////////////////////////////////////////////
void OcTree::Contouring(string strFile, string strSOGFile)
{
	/*
	Two operations should go : write vertexes and faces.
	Operation 1: write vertexes
	A cell can be :
	(1) regular cell, if and only if the cell is leaf type (& 0x3 == 2 & has a vertex in SOG file), and pass the regular test
	(2) irregular cell, otherwise ( can also be divided into two cases : (2.1) unchanged irregular cell with vertex, (2.2) other irregular cell )
	We generate vertex :
	For each regular cell, generate a vertex
	For each inside point and outside irregular cell, generate a vertex ( be cautious with the repeated cells in the pNode[][][] array )
	For each inside point and outside irregular cell, if the cell has a vertex and has not been changed, we use a 0.05 generate
	Operation 2: write faces
	An inside i-D element and an outside (i+1)-D element generate a face.
	Check how many triangles are in the face.

	Three passes should go :
	Init  : put all the inside tags into m_nVertex
	Pass 1: determine regular/irregular cells, count hash map size
	Pass 2: write the vertex number, count vertex number; count face number
	Pass 3: output to file
	**** Pass 2 could be use for performance measurement ****
	*/

#ifdef CONSOLE_OUTPUT
	printf("\n====Contouring&Output====\n");
#endif

	// Initialize
	m_nType = 3;
	TraversalWithoutPosition();
	m_pSOGFile = fopen(strSOGFile.c_str(), "rb");
	fseek(m_pSOGFile, 128, SEEK_SET);
	int nTemp;
	fread(&nTemp, sizeof(int), 1, m_pSOGFile);
	m_bEnableRead = true;
	m_nContourPass = 0;
	m_nVertexNum = 0;
	m_nRegularCount = 0;
	m_nIrregularCount = 0;
	m_nIrregularUnchangedCell = 0;
	RecRegularTest( m_pRoot );
	fclose(m_pSOGFile);

	// Pass 1
	m_nContourPass = 1;
	m_nVertexNum = 0;
	m_nRegularCount = 0;
	m_nIrregularCount = 0;
	m_nIrregularUnchangedCell = 0;
	RecRegularTest( m_pRoot );
	m_nType = 4;
	TraversalWithoutPosition();
	m_pHashMap = new HashMap(&m_cConstant, m_nIrregularCount);
	m_pSingleHashMap = new SingleHashMap(&m_cConstant, m_nIrregularUnchangedCell);
#ifdef CONSOLE_OUTPUT
	printf("Allocing memory for vertex hash map, number is %d\n", m_nIrregularCount);
	printf("Allocing memory for special hash map, number is %d\n", m_nIrregularUnchangedCell);
#endif

	// Pass 2
	TimeMeter cTimer;
	cTimer.Start();
	m_nContourPass = 2;
	m_nVertexNum = 0;
	m_nFaceNum = 0;
	m_nRegularCount = 0;
	m_nIrregularCount = 0;
	m_nIrregularUnchangedCell = 0;
#ifdef CONSOLE_OUTPUT
	printf("Count vertices for contouring...\n");
#endif
	RecRegularTest( m_pRoot );
	m_nType = 4;
	Traversal();
#ifdef CONSOLE_OUTPUT
	printf("Count faces for contouring...\n");
#endif
	m_nType = 5;
	Traversal();
#ifdef CONSOLE_OUTPUT
	printf("Totally %d vertices and %d faces are going to be writen.\n", m_nVertexNum, m_nFaceNum);
#endif
	cTimer.End();

	// Pass 3
	m_nContourPass = 3;
#ifdef LOGFILE_OUTPUT
	string strTemp = strFile.substr(0,strFile.rfind('.')) + ".log";
	m_cWriter.Open(strFile.c_str(), strTemp);
#else
	m_cWriter.Open(strFile);
#endif
	m_cWriter.WriteFileHeader(m_nVertexNum, m_nFaceNum);
	m_pSOGFile = fopen(strSOGFile.c_str(), "rb");
	fseek(m_pSOGFile, 128, SEEK_SET);
	fread(&nTemp, sizeof(int), 1, m_pSOGFile);
	m_bEnableRead = true;

	m_nVertexNum = 0;
	m_nFaceNum = 0;
	m_nRegularCount = 0;
	m_nIrregularCount = 0;
	m_nIrregularUnchangedCell = 0;
#ifdef CONSOLE_OUTPUT
	printf("Writing vertices...\n");
#endif
	RecRegularTest( m_pRoot );
	m_nType = 4;
	Traversal();
#ifdef CONSOLE_OUTPUT
	printf("Writing faces...\n");
#endif
	m_nType = 5;
	Traversal();
#ifdef CONSOLE_OUTPUT
	printf("Totally write %d vertices and %d triangles.\n", m_nVertexNum, m_nFaceNum);
#endif

	// finalization
#ifdef LOGFILE_OUTPUT
	m_cWriter.WriteLogInt(-1);
#endif
	m_cWriter.Close();
	fclose(m_pSOGFile);
#ifdef CONSOLE_OUTPUT
	printf("Time consuming (excluding I/O) : ");
	cTimer.Print();
	printf("\n");
#endif
}

void OcTree::RecRegularTest(UCHAR * pNode)
{
	UCHAR nType = 0;
	UCHAR nNext;
	float fNext[3];

	if ( (m_nContourPass == 0 || m_nContourPass == 3) && m_bEnableRead ) {
		fread(&nType, 1, 1, m_pSOGFile);
		if (nType != 0) {
			fread(&nNext, 1, 1, m_pSOGFile);
			if (nType == 2) {
				fread(fNext, 4, 3, m_pSOGFile);
			}
		}
	}

	if (NOT_LEAF(pNode)) {
		if (nType != 0)
			m_bEnableRead = false;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					RecRegularTest(GetLeafNode(pNode, i, j, k));
		if (nType != 0)
			m_bEnableRead = true;
	} else {
		LeafNode * pThisNode = (LeafNode *)pNode;
		Element * pThisCell = (Element *)(pNode + sizeof(LeafNode));
		if ((*pNode & 0x3) == 2 && pThisCell->m_bObject == 0) {
			bool bRegularTest;

			// if pass 3, then directly get the irregular information stored in pass 2
			// otherwise, compute the irregular information
			if ( m_nContourPass == 0 ) {
				bRegularTest = false;
				bool bChange = false;
				UCHAR nPointTag = (UCHAR)(~((pThisNode->m_nVertex >> (1 + 6 + 12)) & 0xff));
				if (nPointTag != nNext)
					bChange = true;
				if (!bChange) {
					for (int i = 0; i < 12; i++)
						if (!GET_INSIDE_TAG(pThisNode->m_nVertex, i + 1 + 6)
							&& GET_INSIDE_TAG(pThisNode->m_nVertex, Constant::I_SHIFT_EP[i][0] + 1 + 6 + 12)
							&& GET_INSIDE_TAG(pThisNode->m_nVertex, Constant::I_SHIFT_EP[i][1] + 1 + 6 + 12)
							)
							bChange = true;
				}
				if (!bChange) {
					for (int i = 0; i < 6; i++)
						if (!GET_INSIDE_TAG(pThisNode->m_nVertex, i + 1)
							&& GET_INSIDE_TAG(pThisNode->m_nVertex, Constant::I_SHIFT_FP[i][0] + 1 + 6 + 12)
							&& GET_INSIDE_TAG(pThisNode->m_nVertex, Constant::I_SHIFT_FP[i][1] + 1 + 6 + 12)
							&& GET_INSIDE_TAG(pThisNode->m_nVertex, Constant::I_SHIFT_FP[i][2] + 1 + 6 + 12)
							&& GET_INSIDE_TAG(pThisNode->m_nVertex, Constant::I_SHIFT_FP[i][3] + 1 + 6 + 12)
							)
							bChange = true;
				}
				if (bChange) {
					((NodeHeader *)pNode)->m_bChanged = 1;
				} else {
					((NodeHeader *)pNode)->m_bChanged = 0;
				}
			} else if ( m_nContourPass == 3 ) {
				bRegularTest = !IS_CELL_IRREGULAR(pThisNode->m_nVertex);
			} else {
				int nPointTag = (int)(pThisNode->m_nVertex >> (1 + 6 + 12)) & 0xff;
				bRegularTest = (m_cConstant.m_pRegularTableEx[nPointTag][2] == 1);
				if (bRegularTest) {
					for (int i = 0; i < 12; i++)
						if (!GET_INSIDE_TAG(pThisNode->m_nVertex, i + 1 + 6)
							&& GET_INSIDE_TAG(pThisNode->m_nVertex, Constant::I_SHIFT_EP[i][0] + 1 + 6 + 12)
							&& GET_INSIDE_TAG(pThisNode->m_nVertex, Constant::I_SHIFT_EP[i][1] + 1 + 6 + 12)
							)
							bRegularTest = false;
				}
				if (bRegularTest) {
					for (int i = 0; i < 6; i++)
						if (!GET_INSIDE_TAG(pThisNode->m_nVertex, i + 1)
							&& GET_INSIDE_TAG(pThisNode->m_nVertex, Constant::I_SHIFT_FP[i][0] + 1 + 6 + 12)
							&& GET_INSIDE_TAG(pThisNode->m_nVertex, Constant::I_SHIFT_FP[i][1] + 1 + 6 + 12)
							&& GET_INSIDE_TAG(pThisNode->m_nVertex, Constant::I_SHIFT_FP[i][2] + 1 + 6 + 12)
							&& GET_INSIDE_TAG(pThisNode->m_nVertex, Constant::I_SHIFT_FP[i][3] + 1 + 6 + 12)
							)
							bRegularTest = false;
				}
			}

			switch (m_nContourPass) {
			case 1:
				if (!bRegularTest)
					pThisNode->m_nVertex |= 0x80000000;
				break;
			case 2:
				if (bRegularTest)
					pThisNode->m_nVertex = m_nVertexNum;
				break;
			case 3:
				if (bRegularTest)
					m_cWriter.WriteVertex(fNext);
				else
					m_pSingleHashMap->Set((UINT64)pNode, CVector3D(fNext[0], fNext[1], fNext[2]));
				break;
			}

			if (bRegularTest) {
				m_nRegularCount++;
				m_nVertexNum++;
			} else {
				m_nIrregularUnchangedCell++;
			}
		} else {
			if (m_nContourPass == 1)
				pThisNode->m_nVertex |= 0x80000000;
		}
	}
}

void OcTree::RecContouringEdge(UCHAR * pCoNode, UINT64 n64CoPosition, int nCoLevel, UCHAR * pNode[2][2], UINT64 n64Position[2][2], int nLevel[2][2], int dirEdge, int direction, list<int> & listIndex)
{
	// direction : 0 is for normal, 1 is for opposite
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
						Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], direction == 0 ? i : 1-i, 1-j, 1-k, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				RecContouringEdge(pCoNode, n64CoPosition, nCoLevel, pNodeEdge, n64Edge, nEdge, dirEdge, direction, listIndex);
			}
			break;
		case 1:
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], 1-k, direction == 0 ? i : 1-i, 1-j, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				RecContouringEdge(pCoNode, n64CoPosition, nCoLevel, pNodeEdge, n64Edge, nEdge, dirEdge, direction, listIndex);
			}
			break;
		case 2:
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++)
						Leaf(pNode[j][k], n64Position[j][k], nLevel[j][k], 1-j, 1-k, direction == 0 ? i : 1-i, pNodeEdge[j][k], n64Edge[j][k], nEdge[j][k]);
				RecContouringEdge(pCoNode, n64CoPosition, nCoLevel, pNodeEdge, n64Edge, nEdge, dirEdge, direction, listIndex);
			}
			break;
		}
	} else {
		int nI, nJ, nIndex;
		UINT64 n64Point;
		GetStorageEdge(nLevel, dirEdge, nI, nJ, nIndex);
		GetEdgePosition(n64Position, nLevel, dirEdge, nI, nJ, direction, n64Point);
		InsertIndex(listIndex, GetIndex(pCoNode, n64CoPosition, n64Point));
	}
}
