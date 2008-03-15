/****************************************************************************

    Quaternion Class
	Copyright (C) 2008-2008 André Deperrois xflr5@yahoo.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

#include "StdAfx.h"
#include "../X-FLR5.h"
#include ".\quaternion.h"
# include <math.h>

double Quaternion::pi = 3.141592654;
double Quaternion::t1;
double Quaternion::t2;
double Quaternion::t3;
double Quaternion::t4;
double Quaternion::t5;
double Quaternion::t6;
double Quaternion::t7;
double Quaternion::t8;
double Quaternion::t9;
double Quaternion::t10;
double Quaternion::t11;
double Quaternion::t12;
double Quaternion::t15;
double Quaternion::t19;
double Quaternion::t20;
double Quaternion::t24;
	

Quaternion::Quaternion(void)
{
	a=0.0; qx= 0.0; qy=0.0; qz = 0.0;
	theta = 0.0;
}

Quaternion::Quaternion(double t, double x, double y, double z)
{
	a=t; qx= x; qy=y; qz = z;
	theta = 2.0*acos(t);
}

Quaternion::~Quaternion(void)
{
}


void Quaternion::Set(double real, double x, double y, double z)
{	
	a = real;
	qx = x;
	qy = y;
	qz = z;
}

void Quaternion::Conjugate(CVector Vin, CVector &Vout)
{
	//Pseudo-code for rotating using a quaternion: 
	//given a quaternion z = a + bi + cj + dk (with |z| = 1) 
	//and a vector v with elements v1, v2, and v3, the following code performs a rotation. 
	//Note the use of temporary variables txx.
	//Also note one optimization of the diagonal entries of the R matrix:
	//since a2 + b2 + c2 + d2 = 1, rewrite the top-left entry as a2 + b2 + c2 + d2 - 2c2 - 2d2 = 1 - 2c2 - 2d2;
	//the other two diagonal entries can be similarly rewritten.

	t2 =   a*qx;
	t3 =   a*qy;
	t4 =   a*qz;
	t5 =  -qx*qx;
	t6 =   qx*qy;
	t7 =   qx*qz;
	t8 =  -qy*qy;
	t9 =   qy*qz;
	t10 = -qz*qz;
	Vout.x = 2*( (t8 + t10)*Vin.x + (t6 -  t4)*Vin.y + (t3 + t7)*Vin.z ) + Vin.x;
	Vout.y = 2*( (t4 +  t6)*Vin.x + (t5 + t10)*Vin.y + (t9 - t2)*Vin.z ) + Vin.y;
	Vout.z = 2*( (t7 -  t3)*Vin.x + (t2 +  t9)*Vin.y + (t5 + t8)*Vin.z ) + Vin.z;

}

void Quaternion::QuattoMat(double m[][4])
{
	//Pseudo-code for creating an angle/axis matrix where the unit axis is (v1, v2, v3) and the angle is theta:

	if(abs(a)<=1.0)  theta = 2.0 * acos(a);
	else             theta = 0.0;

	t1 =  cos(theta);
	t2 =  1.0 - t1;
	t3 =  qx*qx;
	t6 =  t2*qx;
	t7 =  t6*qy;
	t8 =  sin(theta);
	t9 =  t8*qz;
	t11 = t6*qz;
	t12 = t8*qy;
	t15 = qy*qy;
	t19 = t2*qy*qz;
	t20 = t8*qx;
	t24 = qz*qz;
	m[0][0] = t1 + t2*t3;
	m[0][1] = t7 - t9;
	m[0][2] = t11 + t12;
	m[1][0] = t7 + t9;
	m[1][1] = t1 + t2*t15;
	m[1][2] = t19 - t20;
	m[2][0] = t11 - t12;
	m[2][1] = t19 + t20;
	m[2][2] = t1 + t2*t24;

}	

void Quaternion::operator *=(Quaternion Q)
{
	double t1,t2,t3,t4;

	t1 = a*Q.a  - qx*Q.qx - qy*Q.qy - qz*Q.qz;
	t2 = a*Q.qx + qx*Q.a  + qy*Q.qz - qz*Q.qy ;
	t3 = a*Q.qy + qy*Q.a  + qz*Q.qx - qx*Q.qz ;
	t4 = a*Q.qz + qz*Q.a  + qx*Q.qy - qy*Q.qx ;

	a  = t1;
	qx = t2;
	qy = t3;
	qz = t4;
}

Quaternion Quaternion::operator *(Quaternion Q)
{
	Quaternion prod;

	prod.a = a*Q.a  - qx*Q.qx - qy*Q.qy - qz*Q.qz;

	prod.qx = a*Q.qx + qx*Q.a  + qy*Q.qz - qz*Q.qy ;
	prod.qy = a*Q.qy + qy*Q.a  + qz*Q.qx - qx*Q.qz ;
	prod.qz = a*Q.qz + qz*Q.a  + qx*Q.qy - qy*Q.qx ;

	return prod;
}

void Quaternion::operator =(Quaternion Q)
{

	a  = Q.a;
	qx = Q.qx;
	qy = Q.qy;
	qz = Q.qz;
}


void Quaternion::operator ~()
{
	qx = -qx;
	qy = -qy;
	qz = -qz;
}


void Quaternion::Normalize()
{
	double norm = sqrt(a*a + qx*qx + qy*qy + qz*qz);
	if (norm < 1.0e-10)
	{
		a = 1.0;
		qx = 0.0;
		qy = 0.0;
		qz = 0.0;
	}
	else
	{
		a *= 1/norm;

		qx *= 1/norm;
		qy *= 1/norm;
		qz *= 1/norm;
	}
}










