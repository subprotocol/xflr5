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

#include <sstream>
#include "naca4digitfoil.h"

Naca4DigitFoil::Naca4DigitFoil(const double M, const double P,const double T,const unsigned int N) : Foil()
{
	Init(M,P,T,N);
}


Naca4DigitFoil::~Naca4DigitFoil()
{
}


void Naca4DigitFoil::Init(double M, double P,double T, const unsigned int N)
{
	m=M; p=P;	t=T;

	for(unsigned int i=0;i<N/2;i++){
		const double c=1.-static_cast<double>(i)/(N/2-1);
		x.push_back(Xu(c));
		y.push_back(Yu(c));
	}

	for(unsigned int i=1;i<N/2;i++){
		const double c=static_cast<double>(i)/(N/2-1);
		x.push_back(Xl(c));
		y.push_back(Yl(c));
	}

	std::ostringstream s;
	s<<"NACA "<<static_cast<int>(M*100)<<static_cast<int>(P*10)<<static_cast<int>(T*100);
	name=s.str();
}


/////////////////
//
// Operators
//
/////////////////

// return coordinate
double Naca4DigitFoil::operator()(double s,int coo)
{
// 	std::cerr<<"Naca4DigitFoil::operator()\n";
	double val=0.;
	switch(coo){
		case	0	:	val=s<0.5 ? Xu(1.-s*2.) : Xl(s*2.-1.);break;
		case	1	:	val=s<0.5 ? Yu(1.-s*2.) : Yl(s*2.-1.);break;
		default	:	val=std::numeric_limits<double>::quiet_NaN();
	}
	return val;
}

