#ifndef VECTOR3D_H
#define VECTOR3D_H
#include <math.h>

class Vector3D
{
	 public:
	 Vector3D();
	 Vector3D(double x1, double y1, double z1);
	 Vector3D(const Vector3D& p);	
	 void normalize();	
	 double x;
	 double y;
	 double z;
	 double length;
	 private:
};
#endif
