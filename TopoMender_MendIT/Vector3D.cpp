#include "Vector3D.h"

//CVector3D

CVector3D::CVector3D() 
{ 
	pVec[0] = pVec[1] = pVec[2] = 0.0; 
}

CVector3D::CVector3D(double x, double y, double z)
{
	pVec[0] = x; 
	pVec[1] = y;
	pVec[2] = z;
}

CVector3D::CVector3D(const CVector3D & v) 
{ 
	pVec[0] = v.pVec[0]; 
	pVec[1] = v.pVec[1];
	pVec[2] = v.pVec[2];
}

CVector3D::CVector3D(const Vector3D & v)
{ 
	pVec[0] = v.x; 
	pVec[1] = v.y; 
	pVec[2] = v.z; 
}

double CVector3D::length()
{	
	return sqrt(pVec[0] * pVec[0] + pVec[1] * pVec[1] + pVec[2] * pVec[2]);
}

double CVector3D::length2()	
{	
	return pVec[0] * pVec[0] + pVec[1] * pVec[1] + pVec[2] * pVec[2];
}

double CVector3D::normalize()
{	
	double len = length();
	if (abs(len) > 1e-10)
	{
		pVec[0] /= len;
		pVec[1] /= len;
		pVec[2]/=len;
	}	
	return len;
}

CVector3D& CVector3D::operator = (const CVector3D& v)
{	
	pVec[0] = v.pVec[0];	
	pVec[1] = v.pVec[1];
	pVec[2] = v.pVec[2];
	return (*this);
}

CVector3D& CVector3D::operator += (const CVector3D& v)
{	
	pVec[0] += v.pVec[0];	
	pVec[1] += v.pVec[1];	
	pVec[2] += v.pVec[2];	
	return (*this);	
}

CVector3D& CVector3D::operator -= (const CVector3D& v)
{	
	pVec[0] -= v.pVec[0];	
	pVec[1] -= v.pVec[1];
	pVec[2] -= v.pVec[2];	
	return (*this);	
}

CVector3D& CVector3D::operator *= (double u)
{	
	pVec[0] *= u;		
	pVec[1] *= u;
	pVec[2] *= u;		
	return (*this);	
}

CVector3D& CVector3D::operator /= (double u)
{	
	if (abs(u) > 1e-10)
	{
		pVec[0] /= u;
		pVec[1] /= u;
		pVec[2] /= u;
	}
	return(*this);
}

CVector3D& CVector3D::operator ^= (const CVector3D& v)
{	
	double xx = pVec[1]*v.pVec[2] - pVec[2]*v.pVec[1];	
	double yy = pVec[2]*v.pVec[0] - pVec[0]*v.pVec[2];	
	double zz = pVec[0]*v.pVec[1] - pVec[1]*v.pVec[0];	
	pVec[0] = xx; pVec[1] = yy; pVec[2] = zz; 
	return (*this);	
}


CVector3D operator + (const CVector3D& lv, const CVector3D& rv)
{
	CVector3D rel = lv;
	rel += rv;
	return rel;
}


CVector3D operator - (const CVector3D& lv, const CVector3D& rv)
{
	CVector3D rel = lv;
	rel -= rv;
	return rel;
}

CVector3D operator * (const double u, const CVector3D& rv)
{
	CVector3D rel = rv;
	rel *= u;
	return rel;
}

CVector3D operator * (const CVector3D& lv, const double u)
{
	CVector3D rel = lv;
	rel *= u;
	return rel;
}

CVector3D operator / (const CVector3D& lv, const double u)
{
	CVector3D rel = lv;
	rel /= u;
	return rel;
}

double operator * (const CVector3D& lv, const CVector3D& rv)
{
	return lv.pVec[0]*rv.pVec[0] + lv.pVec[1]*rv.pVec[1] + lv.pVec[2]*rv.pVec[2];
}

CVector3D operator ^ (const CVector3D& lv, const CVector3D& rv)
{
	CVector3D rel = lv;
	rel ^= rv;
	return rel;
}
