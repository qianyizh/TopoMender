#include "Geometry.h"

//Vector2D

Vector2D::Vector2D() {
	x = 0;
	y = 0;
}

Vector2D::Vector2D(double xx, double yy) {
	x = xx;
	y = yy;
}

Vector2D::Vector2D(const Vector2D &v) {
	x = v.x;
	y = v.y;
}

double Vector2D::length() {
	return sqrt(x*x + y*y);
}

double Vector2D::length2() {
	return x*x + y*y;
}

double Vector2D::normalize() {
	double len = length();
	if (!EQUAL_ZERO(len)) {
		x /= len;
		y/=len;
	}
	return len;
}

Vector2D& Vector2D::operator = (const Vector2D& v) {
	x = v.x;
	y = v.y;
	return (*this);
}

Vector2D& Vector2D::operator += (const Vector2D& v) {
	x += v.x;
	y += v.y;
	return (*this);
}
Vector2D& Vector2D::operator -= (const Vector2D& v) {
	x -= v.x;
	y -= v.y;
	return (*this);
}

Vector2D& Vector2D::operator *= (double u) {
	x *= u;
	y *= u;
	return (*this);
}

Vector2D& Vector2D::operator /= (double u) {
	if (!EQUAL_ZERO(u)) {
		x /= u;
		y /= u;
	}
	return(*this);
}

Vector2D operator + (const Vector2D& lv, const Vector2D& rv) {
	Vector2D rel = lv;
	rel += rv;
	return rel;
}


Vector2D operator - (const Vector2D& lv, const Vector2D& rv) {
	Vector2D rel = lv;
	rel -= rv;
	return rel;
}

Vector2D operator * (const double u, const Vector2D& rv) {
	Vector2D rel = rv;
	rel *= u;
	return rel;
}

Vector2D operator * (const Vector2D& lv, const double u) {
	Vector2D rel = lv;
	rel *= u;
	return rel;
}

Vector2D operator / (const Vector2D& lv, const double u) {
	Vector2D rel = lv;
	rel /= u;
	return rel;
}

double operator*(const Vector2D& lv, const Vector2D& rv) {
	return lv.x*rv.x + lv.y*rv.y;
}

short Vector2D::AtWhere(Vector2D v0,Vector2D v1) {
	Vector2D vTemp1(v1.y-v0.y,v0.x-v1.x);
	Vector2D vTemp2(x-v0.x,y-v0.y);
	double d=vTemp1*vTemp2;
	if(EQUAL_ZERO(d))
		return 0;
	if(d > 0)
		return 1;//right
	return -1;//left
}

bool Vector2D::AtRight(Vector2D v0,Vector2D v1) {
	if(AtWhere(v0,v1)==1)
		return true;
	return false;
}

bool Vector2D::AtLeft(Vector2D v0,Vector2D v1) {
	if(AtWhere(v0,v1)==-1)
		return true;
	return false;
}

bool Vector2D::OnLine(Vector2D v0,Vector2D v1) {
	if(AtWhere(v0,v1)==0)
		return true;
	return false;
}

bool Vector2D::Intersect(Vector2D v1,Vector2D v2)//intersect with uint circle
{
	Vector2D vOrigin;
	short s=vOrigin.AtWhere(v1,v2);
	Vector2D vTemp1=v2-v1;
	vTemp1.normalize();
	Vector2D vTemp2;
	//if(s==0)//pass point (0,0)
	if(s==1)//right
	{
		vTemp2.x=-vTemp1.y;
		vTemp2.y=vTemp1.x;
		double d=vTemp2*v1;
		vTemp2*=d;
		d=sqrt(1.0-d*d);
		vTemp1*=d;
	}
	else if(s==-1)//left
	{
		vTemp2.x=vTemp1.y;
		vTemp2.y=-vTemp1.x;
		double d=vTemp2*v1;
		vTemp2*=d;
		d=sqrt(1.0-d*d);
		vTemp1*=d;
	}
	x=vTemp1.x+vTemp2.x;
	y=vTemp1.y+vTemp2.y;
	return true;
}

bool Vector2D::Intersect(Vector2D v1,Vector2D v2,Vector2D v3,Vector2D v4)//tow line intersect
{
	double d=(v4.y-v3.y)*(v1.x-v2.x)-(v2.y-v1.y)*(v3.x-v4.x);
	if(EQUAL_ZERO(d))
		return false;

	double d1=v1.x*v2.y-v2.x*v1.y;
	double d2=v3.x*v4.y-v4.x*v3.y;
	x=((v4.x-v3.x)*d1-(v2.x-v1.x)*d2)/d;
	y=((v4.y-v3.y)*d1-(v2.y-v1.y)*d2)/d;
	return true;
}

double Vector2D::GetArea(Vector2D v)
{
	return x*v.y-v.x*y;
}

//Vector3D

Vector3D::Vector3D() {
	x = 0;
	y = 0;
	z = 0;
}

Vector3D::Vector3D(double xx,double yy,double zz) {
	x = xx;
	y = yy;
	z = zz;
}

Vector3D::Vector3D(const Vector3D& v) {
	x = v.x;
	y = v.y;
	z = v.z;
}

double Vector3D::length() {
	return sqrt(x*x + y*y + z*z);
}

double Vector3D::length2() {
	return x*x + y*y + z*z;
}

double Vector3D::normalize() {
	double len = length();
	if (!EQUAL_ZERO(len)) {
		x /= len;
		y /= len;
		z /= len;
	}
	return len;
}

Vector3D& Vector3D::operator = (const Vector3D& v) {	
	x = v.x;
	y = v.y;
	z = v.z;
	return (*this);
}

Vector3D& Vector3D::operator += (const Vector3D& v) {	
	x += v.x;
	y += v.y;
	z += v.z;	
	return (*this);	
}

Vector3D& Vector3D::operator -= (const Vector3D& v) {	
	x -= v.x;
	y -= v.y;
	z -= v.z;	
	return (*this);	
}
Vector3D& Vector3D::operator *= (double u) {	
	x *= u;	
	y *= u;	
	z *= u;		
	return (*this);	
}

Vector3D& Vector3D::operator /= (double u) {	
	if (!EQUAL_ZERO(u))	{
		x /= u;
		y /= u;
		z /= u;
	}
	return(*this);
}

Vector3D& Vector3D::operator ^= (const Vector3D& v) {	
	double xx = y*v.z - z*v.y;	
	double yy = z*v.x - x*v.z;	
	double zz = x*v.y - y*v.x;	
	x = xx;
	y = yy;
	z = zz; 
	return (*this);	
}


Vector3D operator + (const Vector3D& lv, const Vector3D& rv) {
	Vector3D rel = lv;
	rel += rv;
	return rel;
}

Vector3D operator - (const Vector3D& lv, const Vector3D& rv) {
	Vector3D rel = lv;
	rel -= rv;
	return rel;
}

Vector3D operator * (const double u, const Vector3D& rv) {
	Vector3D rel = rv;
	rel *= u;
	return rel;
}

Vector3D operator * (const Vector3D& lv, const double u) {
	Vector3D rel = lv;
	rel *= u;
	return rel;
}

Vector3D operator / (const Vector3D& lv, const double u) {
	Vector3D rel = lv;
	rel /= u;
	return rel;
}

double operator * (const Vector3D& lv, const Vector3D& rv) {
	return lv.x*rv.x + lv.y*rv.y + lv.z*rv.z;
}

Vector3D operator ^ (const Vector3D& lv, const Vector3D& rv) {
	Vector3D rel = lv;
	rel ^= rv;
	return rel;
}
