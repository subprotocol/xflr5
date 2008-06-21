/****************************************************************************

    CVector Class
    Copyright (C) 2005 André Deperrois XFLR5@yahoo.com

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


// Vector.h: interface for the CVector class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CVector  
{
public: 
	bool IsSame(CVector const &V);
	void Set(CVector const &V);
	void Set(double const &x0, double const &y0, double const &z0);
	void Copy(CVector const &V);
	void RotateX(CVector const &O, double XTilt);
	void RotateY(CVector const &O, double YTilt);
	void RotateZ(CVector const &O, double ZTilt);
	void RotateY(double YTilt);
	void Translate(CVector const &T);
	bool operator ==(CVector const &V);
	void operator =(CVector T);
	void operator+=(CVector const &T);
	void operator-=(CVector const &T);
	void operator*=(double d);
	CVector operator *(double d);
	CVector operator *(CVector const &T);
	CVector operator /(double d);
	CVector operator +(CVector const &V);
	CVector operator -(CVector const &V);
	void Normalize();
	double VAbs();
	double dot(CVector const &V);

	CVector();
	CVector(double xi, double yi, double zi);
	virtual ~CVector();

	double x;
	double y;
	double z;
 
};

