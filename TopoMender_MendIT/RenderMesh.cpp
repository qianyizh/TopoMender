#include "RenderMesh.h"

//.obj
bool RenderMesh::LoadFrom(string strFileName)
{
	FILE *f = NULL;
	if ((f = fopen(strFileName.c_str(),"rb")) == NULL) return false;
	RenderVertex tvertex;
	RenderFace tface;
	UINT l[3];
	char ch = fgetc(f);
	while (ch > 0)
	{
		Vector3D vNorm;
		switch(ch)
		{
		case 'v':	//vertex
			ch=fgetc(f);
			if((ch!=' ')&&(ch!='\t'))
				break;
			fscanf(f, "%f%f%f", &tvertex.m_fPosition[0],&tvertex.m_fPosition[1],&tvertex.m_fPosition[2]);
			m_vVertex.push_back(tvertex);
			m_cBoundingBox.Push(tvertex.m_fPosition);
			break;
		case 'f':	//face
			ch=fgetc(f);
			if((ch!=' ')&&(ch!='\t'))
				break;
			fscanf(f, "%ld%ld%ld\n",&l[0],&l[1],&l[2]);
			for(int j=0;j<3;j++)
				tface.m_iVertex[j] = l[j] - 1;
			// cal normal
			float vvv[2][3];
			for (int i = 0; i < 3; i++) {
				vvv[0][i] = m_vVertex[tface.m_iVertex[1]].m_fPosition[i] - m_vVertex[tface.m_iVertex[0]].m_fPosition[i];
				vvv[1][i] = m_vVertex[tface.m_iVertex[2]].m_fPosition[i] - m_vVertex[tface.m_iVertex[0]].m_fPosition[i];
			}
			vNorm = Vector3D(vvv[0][0], vvv[0][1], vvv[0][2]) ^ Vector3D(vvv[1][0], vvv[1][1], vvv[1][2]);
			vNorm.normalize();
			tface.m_fNormal[0] = (float)vNorm.x;
			tface.m_fNormal[1] = (float)vNorm.y;
			tface.m_fNormal[2] = (float)vNorm.z;
			// end
			m_vFace.push_back(tface);
			break;
		}
		ch=fgetc(f);
	}
	fclose(f);
	return true;
}

void RenderMesh::SaveTo(string strFileName)
{
	FileWriter cWriter;
	cWriter.Open(strFileName);
	cWriter.WriteFileHeader(m_vVertex.size(), m_vFace.size());
	for (int i = 0; i < (int)m_vVertex.size(); i++)
		cWriter.WriteVertex(m_vVertex[i].m_fPosition);
	for (int i = 0; i < (int)m_vFace.size(); i++) {
		list<int> listFace;
		listFace.push_back(m_vFace[i].m_iVertex[0]);
		listFace.push_back(m_vFace[i].m_iVertex[1]);
		listFace.push_back(m_vFace[i].m_iVertex[2]);
		cWriter.WriteFace(listFace);
	}
	cWriter.Close();
}

void RenderMesh::ReCalcNormal()
{
	for ( int j = 0; j < m_vFace.size(); j++ ) {
		Vector3D vNorm;
		float vvv[2][3];
		for (int i = 0; i < 3; i++) {
			vvv[0][i] = m_vVertex[m_vFace[j].m_iVertex[1]].m_fPosition[i] - m_vVertex[m_vFace[j].m_iVertex[0]].m_fPosition[i];
			vvv[1][i] = m_vVertex[m_vFace[j].m_iVertex[2]].m_fPosition[i] - m_vVertex[m_vFace[j].m_iVertex[0]].m_fPosition[i];
		}
		vNorm = Vector3D(vvv[0][0], vvv[0][1], vvv[0][2]) ^ Vector3D(vvv[1][0], vvv[1][1], vvv[1][2]);
		vNorm.normalize();
		m_vFace[j].m_fNormal[0] = (float)vNorm.x;
		m_vFace[j].m_fNormal[1] = (float)vNorm.y;
		m_vFace[j].m_fNormal[2] = (float)vNorm.z;
	}
}
