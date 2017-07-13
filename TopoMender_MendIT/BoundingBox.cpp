#include "BoundingBox.h"

//BoundingBox

BoundingBox::BoundingBox()
{
	Reset();
}

float BoundingBox::Max(float a, float b) 
{
	if (a > b)
		return a;
	else
		return b;
}

float BoundingBox::Min(float a, float b)
{
	if (a > b)
		return b;
	else
		return a;
}

void BoundingBox::Reset()
{
	m_fMin[0] = m_fMin[1] = m_fMin[2] = 1e10;
	m_fMax[0] = m_fMax[1] = m_fMax[2] = -1e10;
}

void BoundingBox::Push(float m_fVertex[3])
{
	for (int i = 0; i < 3; i++) {
		if (m_fVertex[i] < m_fMin[i])
			m_fMin[i] = m_fVertex[i];
		if (m_fVertex[i] > m_fMax[i])
			m_fMax[i] = m_fVertex[i];
	}
}

Vector3D BoundingBox::GetPosition()
{
	return Vector3D(
		(m_fMin[0] + m_fMax[0]) / 2.0,
		(m_fMin[1] + m_fMax[1]) / 2.0,
		(m_fMin[2] + m_fMax[2]) / 2.0
		);
}

float BoundingBox::GetLength()
{
	return Max(m_fMax[2] - m_fMin[2], Max(m_fMax[1] - m_fMin[1], m_fMax[0] - m_fMin[0]));
}

float BoundingBox::GetMaxLength()
{
	return (float)(2.0 * GetLength());
}

Vector3D BoundingBox::GetCorner(int i, int j, int k)
{
	Vector3D vPos = GetPosition();
	float fLength = GetLength();
	return vPos + Vector3D(
		(i - 0.5) * fLength,
		(j - 0.5) * fLength,
		(k - 0.5) * fLength
		);
}
