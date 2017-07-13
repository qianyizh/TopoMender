#include "FileWriter.h"

FileWriter::FileWriter()
{
	m_pWriteFile = NULL;
	m_pLogFile = NULL;
	m_nFileType = 0;
}

FileWriter::~FileWriter()
{
	Close();
}

void FileWriter::Open(string strFileName, string strLogName)
{
	string strExtName = strFileName.substr(strFileName.length()-4);
	if (strExtName == ".obj") { //only understand lowercase
		m_nFileType = 0;
	} else {
		m_nFileType = 1;
	}
	m_pWriteFile = fopen(strFileName.c_str(), "wb");
	if (strLogName != "") {
		m_pLogFile = fopen(strLogName.c_str(), "wb");
	}
}

void FileWriter::Close()
{
	if (m_pWriteFile) {
		fclose(m_pWriteFile);
		m_pWriteFile = NULL;
		m_nFileType = 0;
	}

	if (m_pLogFile) {
		fclose(m_pLogFile);
		m_pLogFile = NULL;
	}
}

void FileWriter::WriteFileHeader(int nVertexNum, int nFaceNum)
{
	switch(m_nFileType)
	{
	case 1:				//ply
		fprintf(m_pWriteFile, "ply\n");
		fprintf(m_pWriteFile, "format binary_big_endian 1.0\n");
		fprintf(m_pWriteFile, "element vertex %d\n", nVertexNum);
		fprintf(m_pWriteFile, "property float x\n");
		fprintf(m_pWriteFile, "property float y\n");
		fprintf(m_pWriteFile, "property float z\n");
		fprintf(m_pWriteFile, "element face %d\n", nFaceNum);
		fprintf(m_pWriteFile, "property list uchar int vertex_indices\n");
		fprintf(m_pWriteFile, "end_header\n");
		break;
	default:			// obj
		fprintf(m_pWriteFile, "# %d %d\n", nVertexNum, nFaceNum);
		break;
	}
}

void FileWriter::WriteFace(list<int> & listIndex)
{
	int * pIndex = new int[listIndex.size()];
	list<int>::iterator itTemp;
	int l;
	for (itTemp = listIndex.begin(), l = 0; itTemp != listIndex.end(); itTemp++, l++)
		pIndex[l] = *itTemp;
	for (int j = 0; j < (int)listIndex.size() - 2; j++) {
		int nTemp = 3;
		switch(m_nFileType) 
		{
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
	delete []pIndex;
}

void FileWriter::WriteFace(list<int> & listIndex, int & nFaceNum)
{
	int * pIndex = new int[listIndex.size()];
	list<int>::iterator itTemp;
	int l;
	for (itTemp = listIndex.begin(), l = 0; itTemp != listIndex.end(); itTemp++, l++)
		pIndex[l] = *itTemp;
	for (int j = 0; j < (int)listIndex.size() - 2; j++) {
		int nTemp = 3;
		switch(m_nFileType)
		{
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
		nFaceNum++;
	}
	delete []pIndex;
}

void FileWriter::CountFace(list<int> & listIndex, int & nFaceNum)
{
	int * pIndex = new int[listIndex.size()];
	list<int>::iterator itTemp;
	int l;
	for (itTemp = listIndex.begin(), l = 0; itTemp != listIndex.end(); itTemp++, l++)
		pIndex[l] = *itTemp;
	for (int j = 0; j < (int)listIndex.size() - 2; j++) {
		nFaceNum++;
	}
	delete []pIndex;
}
void FileWriter::WriteVertex(float fPos[3])
{
	switch(m_nFileType)
	{
	case 1:			// ply
		reverse_fwrite((UCHAR *)fPos, 4, 3, m_pWriteFile);
		break;
	default:		// obj
		fprintf(m_pWriteFile, "v %12.10f %12.10f %12.10f\n", fPos[0], fPos[1], fPos[2]);
		break;
	}
}

void FileWriter::reverse_fwrite(UCHAR * pBuffer, int nSize, int nNum, FILE * pFile)
{
	for (int i = 0; i < nNum; i++)
		for (UCHAR * pWrite = pBuffer + (i * nSize) + nSize - 1;
			pWrite >= pBuffer + (i * nSize); pWrite--)
			fwrite(pWrite, 1, 1, pFile);
}

void FileWriter::WriteLogInt(int nNum) 
{
	if (m_pLogFile)
	{
		fprintf(m_pLogFile, "%d\n", nNum);
	}
}