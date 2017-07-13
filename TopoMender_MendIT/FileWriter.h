#ifndef _FILEWRITER
#define _FILEWRITER

#include "resources.h"
using namespace std;

class FileWriter {
public:
	FILE * m_pWriteFile;
	FILE * m_pLogFile;
	int m_nFileType;				// default obj; 1 ply
public:
	FileWriter();
	~FileWriter();
	void Open(string strFileName, string strLogName = "");
	void Close();
	void WriteFileHeader(int nVertexNum, int nFaceNum);
	void WriteFace(list<int> & listIndex);
	void WriteFace(list<int> & listIndex, int & nFaceNum);
	void CountFace(list<int> & listIndex, int & nFaceNum);
	void WriteVertex(float fPos[3]);
	void reverse_fwrite(UCHAR * pBuffer, int nSize,
		int nNum, FILE * pFile);
	void WriteLogInt(int nNum);
};

#endif