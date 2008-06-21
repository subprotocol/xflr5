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

#pragma once

class Quaternion
{
public:
	Quaternion(void);
	Quaternion(double const &t, double const &x, double const &y, double const &z);
	~Quaternion(void);

	void Set(double const &real, double const &x, double const &y, double const &z);
	void Set(double const &Angle, CVector const &R);
	void Conjugate(CVector const &Vin, CVector &Vout);
	void Conjugate(CVector &V);
	void Conjugate(double &x, double &y, double &z);
	void QuattoMat(double m[][4]);
	void Normalize();

	void operator*=(Quaternion Q);
	void operator ~();
	void operator =(Quaternion Q);
	Quaternion operator *(Quaternion Q);


	double theta;
	double a, qx, qy,qz;
	static double pi, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t15, t19, t20, t24;
	static CVector R
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		;
};
