#ifndef _Vector3D_cpp_
#define _Vector3D_cpp_
#include "Vector3D.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
using namespace std;

Vector3D::Vector3D()
{
 x = 0; 
 y = 0;
 z = 0;
 length = 0.0;
}
Vector3D::Vector3D(double x1, double y1, double z1)
{
 x = x1;
 y = y1;
 z = z1;
 length = sqrt(  (x * x) + (y * y) + (z * z));  //magnitude of the vector
}

Vector3D::Vector3D(const Vector3D& p)
{
  x = p.x;
  y = p.y;
  z = p.z;
  length = p.length;
}

void Vector3D::normalize()
{
	   if(x != 0)
       x = x / length;

	   if(y != 0)
       y = y / length;

	   if(z != 0)
       z = z / length;
	   
}
#endif
