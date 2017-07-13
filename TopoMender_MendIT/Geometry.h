#ifndef _GEOMETRY
#define _GEOMETRY

#include "resources.h"

#define DOUBLE_EPS 1e-16
#define EQUAL_ZERO(x) (fabs((x))< DOUBLE_EPS)

class Vector2D {
public:
	double x,y;
public:
	Vector2D();
	Vector2D(double xx,double yy);
	Vector2D(const Vector2D& v);
	double length();
	double length2();
	double normalize();
	Vector2D& operator  = (const Vector2D& v);
	Vector2D& operator += (const Vector2D& v);
	Vector2D& operator -= (const Vector2D& v);
	Vector2D& operator *= (double u);
	Vector2D& operator /= (double u);
	bool Intersect(Vector2D v1,Vector2D v2,Vector2D v3,Vector2D v4);
	bool Intersect(Vector2D v1,Vector2D v2);
	friend Vector2D operator + (const Vector2D& lv, const Vector2D& rv);
	friend Vector2D operator - (const Vector2D& lv, const Vector2D& rv);
	friend Vector2D operator * (const double u, const Vector2D& rv);
	friend Vector2D operator * (const Vector2D& lv, const double u);
	friend Vector2D operator / (const Vector2D& lv, const double u);
	friend double   operator * (const Vector2D& lv, const Vector2D& rv);
	short AtWhere(Vector2D v0,Vector2D v1);
	bool AtLeft(Vector2D v0,Vector2D v1);
	bool AtRight(Vector2D v0,Vector2D v1);
	bool OnLine(Vector2D v0,Vector2D v1);
	double GetArea(Vector2D v);
};

class Vector3D {
public:
	double x,y,z;
public:
	Vector3D();
	Vector3D(double xx,double yy,double zz);
	Vector3D(const Vector3D& v);
	double length();
	double length2();
	double normalize();
	Vector3D& operator  = (const Vector3D& v);
	Vector3D& operator += (const Vector3D& v);
	Vector3D& operator -= (const Vector3D& v);
	Vector3D& operator *= (double u);
	Vector3D& operator /= (double u);
	Vector3D& operator ^= (const Vector3D& v);
	friend Vector3D operator + (const Vector3D& lv, const Vector3D& rv);
	friend Vector3D operator - (const Vector3D& lv, const Vector3D& rv);
	friend Vector3D operator * (const double u, const Vector3D& rv);
	friend Vector3D operator * (const Vector3D& lv, const double u);
	friend Vector3D operator / (const Vector3D& lv, const double u);
	friend double   operator * (const Vector3D& lv, const Vector3D& rv);
	friend Vector3D operator ^ (const Vector3D& lv, const Vector3D& rv);
};

#endif
