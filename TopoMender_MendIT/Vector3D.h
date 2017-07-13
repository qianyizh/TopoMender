#ifndef _VECTOR3D
#define _VECTOR3D

#include "Geometry.h"

class CVector3D {
public :
	double pVec[3];
public:
	CVector3D();
	CVector3D(double x, double y, double z);
	CVector3D(const CVector3D & v);
	CVector3D(const Vector3D & v);
	~CVector3D() {}
	double length();
	double length2();
	double normalize();
	CVector3D& operator  = (const CVector3D& v);
	CVector3D& operator += (const CVector3D& v);
	CVector3D& operator -= (const CVector3D& v);
	CVector3D& operator *= (double u);
	CVector3D& operator /= (double u);
	CVector3D& operator ^= (const CVector3D& v);
	friend CVector3D operator + (const CVector3D& lv, const CVector3D& rv);
	friend CVector3D operator - (const CVector3D& lv, const CVector3D& rv);
	friend CVector3D operator * (const double u, const CVector3D& rv);
	friend CVector3D operator * (const CVector3D& lv, const double u);
	friend CVector3D operator / (const CVector3D& lv, const double u);
	friend double operator * (const CVector3D& lv, const CVector3D& rv);
	friend CVector3D operator ^ (const CVector3D& lv, const CVector3D& rv);
};

#endif
