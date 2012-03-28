/*
Parts of this class are derived from:
http://gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation
*/
#include "Quaternion.h"
#include "vector3D.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
using namespace std;

//default constructor
Quaternion::Quaternion(void)
{
	x = y = z = w = 0.0;
}
//constructor
Quaternion::Quaternion(double x1, double y1, double z1, double w1)
{
	x = x1;
	y = y1;
	z = z1;
	w = w1;
}
//returns the transformation matrix form of the quaternion
//this matrix can be direclty pushed on the opengl matrix stack
float * Quaternion::getMatrix()
{
	float temp1[16]   =   { 
		w*w + x*x - y*y - z*z,  2*x*y + 2*w*z, 	        2*x*z - 2*w*y,        	0,
		2*x*y - 2*w*z, 	      w*w - x*x + y*y - z*z, 	2*y*z + 2*w*x, 	        0,
		2*x*z + 2*w*y, 	      2*y*z - 2*w*x, 	        w*w - x*x - y*y + z*z, 	0,
		0, 	                  0,                        0,                      w*w + x*x + y*y + z*z 
	};	
	return temp1;
}
//gets axis of rotation from quatertion
Vector3D Quaternion::getAxis()
{
	float scale = sqrt(x * x + y * y + z * z);
	float axisx = x / scale;
	float axisy = y / scale;
	float axisz = z / scale;
	Vector3D toreturn(axisx, axisy, axisz);
	return toreturn;
}
//gets the angle of the quaternion trasformation
double Quaternion::getAngle()
{
	double toreturn = acos(w) * 2.0;
	return toreturn;
}
//sets the quaternion to rotation about Vector &v1 with angle
void Quaternion::fromAxis(const Vector3D &v1, double angle) 
{
	double sinAngle;
	Vector3D vn = Vector3D(v1.x,v1.y,v1.z);//gets a vector
	sinAngle = sin(angle * .5);	//takes sing of have the angle of rotation
	x = (vn.x * sinAngle);  //sets up the quaternion compnonests
	y = (vn.y * sinAngle);
	z = (vn.z * sinAngle);
	w = cos(angle);	
	
	//everthing beyond this is to make suer that the magnitude 
	//of the quaternion is exactly 1, it can be off from using doubles
	///These lines of code ensure that it is exactly 1
	double q = x *x + y*y + z*z + w*w;  
	double xtra = sqrt((double)1.000000000/q);
	x *= xtra;
	y *= xtra;
	z *= xtra;
	w  *= xtra;
	q = x *x + y*y + z*z + w*w;
	xtra = sqrt((double)1.000000000/q);
	x *= xtra;
	y *= xtra;
	z *= xtra;
	w  *= xtra;
}
//Gets the conjugate of the quaternion
Quaternion Quaternion::getConjugate() const
{
	return Quaternion(-x, -y, -z, w);
}

///normalizes the quaternion so that the mag is 1
void Quaternion::normalise()
{	
	double mag2 = w * w + x * x + y * y + z * z;
	if (  mag2!=0.f && (fabs(mag2 - 1.0f) > .000001f)) {
		double mag = sqrt(mag2);
		w /= mag;
		x /= mag;
		y /= mag;
		z /= mag;
	}		
}
//copy constructor for the quaternion
Quaternion::Quaternion(const Quaternion& p)
{
	x = p.x;
	y = p.y;
	z = p.z;
	w = p.w;
}
//overloaded * operator to multiply 2 quaternions together
//effectivly combining the transformations of both quaterions'
//into a single quaternion
Quaternion Quaternion::operator* (const Quaternion &rq) const
{
	// the constructor takes its arguments as (x, y, z, w)
	return Quaternion(w * rq.x + x * rq.w + y * rq.z - z * rq.y,
	                  w * rq.y + y * rq.w + z * rq.x - x * rq.z,
	                  w * rq.z + z * rq.w + x * rq.y - y * rq.x,
	                  w * rq.w - x * rq.x - y * rq.y - z * rq.z);
}
//overloads teh * operator so that a quaternion
//can be applied to a vector.  returns the transformed vector
Vector3D Quaternion::operator* (const Vector3D &vec) const
{
	//To apply to a vector,
	//The vector needs to be multiplied by the quaternion
	//and that quaternions conjugate
	Vector3D vn = Vector3D(vec);
	Quaternion vecQuat(vn.x, vn.y, vn.z, 0);
	Quaternion resQuat;	 
	resQuat = vecQuat * getConjugate();
	resQuat = *this * resQuat;
	Vector3D toreturn(resQuat.x, resQuat.y, resQuat.z);
	return toreturn;
}
