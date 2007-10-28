/***************************************************************************
 *   Copyright (C) 2007 by Andrés Chavarría Krauser  *
 *   el_andrecillo@users.sourceforge.net   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef FOIL_H
#define FOIL_H

#include <iostream>
#include <vector>
#include <string>

#include "cubicspline.h"

typedef std::vector<double> tFoilCoo;

/**
An abstract foil class

	@author Andres <andres@hal9000>
*/

class Foil{
public:
	Foil();
	Foil(const Foil &foil);
  virtual ~Foil();

	// operators
	virtual double operator()(double s,int coo);
	virtual Foil &operator=(const Foil &foil);
	virtual Foil &operator+=(const Foil &foil);
	friend std::ostream &operator<<(std::ostream &stream, const Foil &foil);
  friend std::istream &operator>>(std::istream &stream, Foil &foil);

	// get members
	// non-constant
	tFoilCoo &X(){return x;}
	tFoilCoo &Y(){return y;}
	std::string &Name(){return name;}
	// constant
	const tFoilCoo &X() const {return x;}
	const tFoilCoo &Y() const {return y;}
	const std::string &Name() const {return name;}
	virtual double Chord() const {return xMax-xMin;}
	virtual double Camber() const {return m;}
	int N() const {return x.size();}
	bool Empty(){return x.size()==0;}

protected:
	int EraseConcurrentPoints();
	void Characteristics();

protected:
	std::string name;
	tFoilCoo	x;
	tFoilCoo	y;

	nNumerics::CubicSpline2D	spline;
	
	double xMax,xMin,yMax,yMin;
	
	// foil characteristics
	double m; 
	double p;
	double t;
};


#endif
