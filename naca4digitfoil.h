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
#ifndef NACA4DIGITFOIL_H
#define NACA4DIGITFOIL_H

#include <math.h>
#include <foil.h>

/**
NACA 4-Digit foil generator. 
m max camber in fraction of camber c=1.
p postion of max camber in fractions of c.
t max thickness in fractions of c.

	@author Andres <andres@hal9000>
*/
class Naca4DigitFoil : public Foil
{
public:
	Naca4DigitFoil(double M, double P,double T, const unsigned int N);
  virtual ~Naca4DigitFoil();
	virtual void Init(const double M, const double P,const double T, const unsigned int N);

	// operators
	// overload operator() to use analytic expression of foil
	virtual double operator()(double s,int coo);
	//get members
	double Chord() const {return 1.;}
	double Camber() const {retrn m;}
protected:
	// mean camber line
	double yc(const double c){
		return c<p ? m/pow(p,2)*(2.*p*c-pow(c,2)) : m/pow(1.-p,2)*((1.-2*p)+2.*p*c-pow(c,2));
	}
	
	// derivative of yc
	double dyc(const double c){
		return 2.*m*(p-c)*(c<p ? 1./pow(p,2) : 1./pow(1.-p,2));
	}

	// angle of mean camber line
	double theta(const double c){
		return atan(dyc(c));
	}

	// thickness distribution
	double yt(const double c){
		return	(t/0.2)*(0.2969*sqrt(c)-0.1260*c-0.3516*pow(c,2)+0.2843*pow(c,3)-0.1015*pow(c,4));
	}

	// upper x coordinate 
	double Xu(const double c){
		return c - yt(c) * sin(theta(c));
	}
	// upper y coordinate 
	double Yu(const double c){
		return yc(c) + yt(c) * cos(theta(c));
	}

	// lower x coordinate 
	double Xl(const double c){
		return c + yt(c) * sin(theta(c));
	}

	// lower y coordinate 
	double Yl(const double c){
		return yc(c) - yt(c) * cos(theta(c));
	}
};

#endif
