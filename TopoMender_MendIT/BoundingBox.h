#ifndef _BOUNDINGBOX
#define _BOUNDINGBOX

#include "resources.h"
#include "Geometry.h"

class BoundingBox {
public:
	float m_fMin[3];
	float m_fMax[3];
public:
	BoundingBox();
	~BoundingBox(){}
	float Max(float a, float b);
	float Min(float a, float b);
	void Reset();
	void Push(float m_fVertex[3]);
	Vector3D GetPosition();
	float GetLength();
	float GetMaxLength();
	Vector3D GetCorner(int i, int j, int k);
};

#endif