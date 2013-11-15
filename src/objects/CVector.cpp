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
#include "Quaternion.h"
#include "../globals.h"


void CVector::Rotate(CVector const &R, double Angle)
{
	//rotate the vector around R with an angle Angle
	Quaternion Qt;
	Qt.Set(Angle, R);
	Qt.Conjugate(x,y,z);
}



void CVector::Rotate(CVector &O, CVector const &R, double Angle)
{
	//rotate the point defined by the vector around origin O, rotation vector R and angle Angle
	Quaternion Qt;
	Qt.Set(Angle, R);
	CVector OP;
	OP.x = x-O.x;
	OP.y = y-O.y;
	OP.z = z-O.z;
	Qt.Conjugate(OP);
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

	x = O.x + OP.x * cos(ZTilt) + OP.y * sin(ZTilt);
	y = O.y - OP.x * sin(ZTilt) + OP.y * cos(ZTilt);
}

void  CVector::RotateY(double YTilt)
{
	YTilt *=PI/180.0;

	double xo = x;
	double zo = z;
	x =  xo * cos(YTilt) + zo * sin(YTilt);
	z = -xo * sin(YTilt) + zo * cos(YTilt);
}


