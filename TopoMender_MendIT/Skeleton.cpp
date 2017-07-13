#include "Skeleton.h"

//PointInfo

PointInfo::PointInfo() {
	m_nLayer = 0;
	m_nMaxLayer = -1;
	m_nDepth = MAX_DEPTH / 2;
	for(int i = 0; i < MAX_LAYERS; i++)
		m_vecLayer[i] = Vector3D(1e30, 1e30, 1e30);
}

Vector3D PointInfo::MidPosition() {
	if ( m_nLayer >= 1 && m_nLayer < m_nMaxLayer )
		return ( m_vecLayer[m_nLayer] + m_vecLayer[m_nLayer - 1] ) / 2.0;
	else
		return Vector3D(1e30, 1e30, 1e30);
}

Vector3D PointInfo::DepthPosition() {
	if ( m_nLayer >= 1 && m_nLayer < m_nMaxLayer )
		return ( m_vecLayer[m_nLayer] - m_vecLayer[m_nLayer - 1] )
			/ MAX_DEPTH * m_nDepth + m_vecLayer[m_nLayer - 1];
	else
		return Vector3D(1e30, 1e30, 1e30);
}

//LineSegment

LineSegment::LineSegment(Vector3D v1, Vector3D v2, int l) {
	m_vPos[0] = v1;
	m_vPos[1] = v2;
	m_nLayer = l;
}

//LineInfo

LineInfo::LineInfo(int iStart, int iEnd) {
	m_iPoint[0] = iStart;
	m_iPoint[1] = iEnd;
}

//Skeleton

Skeleton::Skeleton() {
	m_iPickPoint = -1;
}

void Skeleton::Show() {
	for(int i = 0;i < m_vecPoint.size();i++)
	{
		cout << i << ":\t";
		cout << m_vecPoint[i].m_vPosition.x << "\t"
			<< m_vecPoint[i].m_vPosition.y << "\t"
			<< m_vecPoint[i].m_vPosition.z << endl;
	}
	for(int i = 0;i < m_vecLine.size();i++)
	{
		cout << i << ":\t";
		cout << m_vecLine[i].m_iPoint[0] << "\t"
			<< m_vecLine[i].m_iPoint[1] << endl;
	}
}

//.axs

bool Skeleton::LoadFrom(string strFile) {
	ifstream fload(strFile.c_str());
	char tmpChar;
	string tmpStr;
	int tmpInt;
	while(fload >> tmpChar)
	{
		PointInfo ptInfo;
		LineInfo lnInfo;
		switch(tmpChar)
		{
		case '#':
			fload >> tmpStr;
			break;
		case 'v':
			fload >> ptInfo.m_vPosition.x;
			fload >> ptInfo.m_vPosition.y;
			fload >> ptInfo.m_vPosition.z;
			m_vecPoint.push_back(ptInfo);
			break;
		case 'l':
			fload >> lnInfo.m_iPoint[0];
			lnInfo.m_iPoint[0]--;
			fload >> lnInfo.m_iPoint[1];
			lnInfo.m_iPoint[1]--;
			m_vecLine.push_back(lnInfo);
			break;
		case 's':
			fload >> tmpInt;
			break;
		case 'o':
			return true;
		default:
			return false;
		}
	}
	fload.close();
	return true;
}
