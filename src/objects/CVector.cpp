/****************************************************************************

    Vector Class
	Copyright (C) 2008 Andre Deperrois adeperrois@xflr5.com

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



#include <math.h>
#include "CVector.h"

#define PI             3.14159265358979


void CVector::Rotate(CVector const &R, double Angle)
{
	//rotate the vector around R with an angle Angle
	static double norm, ux, uy,uz, ca, sa, x0, y0, z0;

	ca = cos(Angle *PI/180.0);
	sa = sin(Angle *PI/180.0);

	x0 = x;
	y0 = y;
	z0 = z;

	norm = sqrt(R.x*R.x+ R.y*R.y + R.z*R.z);
	ux = R.x/norm;
	uy = R.y/norm;
	uz = R.z/norm;

	x =     (ca+ux*ux*(1-ca))  *x0  +  (ux*uy*(1-ca)-uz*sa) *y0 +  (ux*uz*(1-ca)+uy*sa) *z0;
	y =   (uy*ux*(1-ca)+uz*sa) *x0  +    (ca+uy*uy*(1-ca))  *y0 +  (uy*uz*(1-ca)-ux*sa) *z0;
	z =   (uz*ux*(1-ca)-uy*sa) *x0  +  (uz*uy*(1-ca)+ux*sa) *y0 +    (ca+uz*uz*(1-ca))  *z0;

}


void CVector::Rotate(CVector &O, CVector const &R, double Angle)
{
	//rotate the point defined by the vector around origin O, rotation vector R and angle Angle
	static CVector OP;
	OP.x = x-O.x;
	OP.y = y-O.y;
	OP.z = z-O.z;

	OP.Rotate(R, Angle);

	x = O.x + OP.x;
	y = O.y + OP.y;
	z = O.z + OP.z;
}


void CVector::RotateX(CVector const &O, double XTilt)
{
	//Rotate the vector around the X-axis, by an angle XTilt
	CVector OP;
	OP.x = x-O.x;
	OP.y = y-O.y;
	OP.z = z-O.z;
		
	XTilt *=PI/180.0;
	y = O.y + OP.y * cos(XTilt) - OP.z * sin(XTilt);
	z = O.z + OP.y * sin(XTilt) + OP.z * cos(XTilt);
}

void CVector::RotateY(CVector const &O, double YTilt)
{
	//Rotate the vector around the Y-axis, by an angle YTilt
	CVector OP;
	OP.x = x-O.x;
	OP.y = y-O.y;
	OP.z = z-O.z;
		
	YTilt *=PI/180.0;

	x = O.x + OP.x * cos(YTilt) + OP.z * sin(YTilt);
	z = O.z - OP.x * sin(YTilt) + OP.z * cos(YTilt);
}


void CVector::RotateZ(CVector const &O, double ZTilt)
{
	//Rotate the vector around the Z-axis, by an angle ZTilt
	CVector OP;
	OP.x = x-O.x;
	OP.y = y-O.y;
	OP.z = z-O.z;
	
	ZTilt *=PI/180.0;

	x = O.x + OP.x * cos(ZTilt) - OP.y * sin(ZTilt);
	y = O.y + OP.x * sin(ZTilt) + OP.y * cos(ZTilt);
}


void  CVector::RotateY(double YTilt)
{
	YTilt *=PI/180.0;

	double xo = x;
	double zo = z;
	x =  xo * cos(YTilt) + zo * sin(YTilt);
	z = -xo * sin(YTilt) + zo * cos(YTilt);
}


