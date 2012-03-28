/*
Parts of this class are derived from:
http://gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation
*/

#ifndef QUATERNION_H
#define QUATERNION_H
#include "Vector3D.h"
class Quaternion
{
public:
	 Quaternion();	
	 Quaternion(double x1, double y1, double z1, double w1);
	 Quaternion(const Quaternion& p);	
	 void normalise();	 
     float * getMatrix();
     Quaternion operator* (const Quaternion &rq) const;
     Quaternion getConjugate() const;
	 void fromAxis(const Vector3D &v1, double angle);
	 Vector3D getAxis();
	 double getAngle();
	 Vector3D operator* (const Vector3D &vec) const;
	 double x;
	 double y;
	 double z;
	 double w;

	private:
};
#endif
