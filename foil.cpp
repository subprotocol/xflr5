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

#include <math.h>
#include <limits>
#include <list>
#include "foil.h"

Foil::Foil()
{
	xMax=xMin=yMax=yMin=0.;
	m=p=t=0.;
}


Foil::Foil(const Foil &foil)
{
	*this=foil;
}


Foil::~Foil()
{
}


/////////////////
//
// Operators
//
/////////////////

// return coordinate
double Foil::operator()(double s,int coo)
{
// 	std::cerr<<"Foil::operator()\n";
	return spline(s*(x.size()-1),coo);
}

Foil &Foil::operator=(const Foil &foil)
{
	name=foil.Name();
	x=foil.X();
	y=foil.Y();
	
	xMax=foil.xMax;
	xMin=foil.xMin;
	yMax=foil.yMax;
	yMin=foil.yMin;

	m=foil.m;
	p=foil.p;
	t=foil.t;
	return *this;
}

// interpolator
Foil &Foil::operator+=(const Foil &foil)
{
	return *this;
}


// erase points that are double
int Foil::EraseConcurrentPoints()
{
	tFoilCoo::iterator iterX=x.begin();
	tFoilCoo::iterator iterY=y.begin();
	std::list<tFoilCoo::iterator> toDeleteX;
	std::list<tFoilCoo::iterator> toDeleteY;

	while(iterX!=x.end()-1||iterY!=y.end()-1){
		if(*iterX==*(iterX+1)&&*iterY==*(iterY+1)){
			toDeleteX.push_front(iterX);
			toDeleteY.push_front(iterY);
		}
		++iterX; ++iterY;		
	}
	
	std::list<tFoilCoo::iterator>::iterator iterDelX=toDeleteX.begin();
	std::list<tFoilCoo::iterator>::iterator iterDelY=toDeleteY.begin();
	int erasedNum=0;

	while(iterDelX!=toDeleteX.end()&&iterDelY!=toDeleteY.end()){
		x.erase(*iterDelX);
		y.erase(*iterDelY);
		++iterDelX; ++iterDelY; ++erasedNum;
	}
	return erasedNum;
}


void Foil::Characteristics()
{

}


// stream communication
std::ostream &operator<<(std::ostream &stream, const Foil &foil)
{
	stream<<foil.Name()<<"\n";

	tFoilCoo::const_iterator iterX=foil.X().begin();
	tFoilCoo::const_iterator iterY=foil.Y().begin();

	while(iterX!=foil.X().end()&&iterY!=foil.Y().end()){
		stream<<*iterX<<" "<<*iterY<<"\n";
		++iterX; ++iterY;
	}

	return stream;
}


#ifndef cMaxFoilName
#define cMaxFoilName 1024
#endif

std::istream &operator>>(std::istream &stream, Foil &foil)
{
	char foilName[cMaxFoilName];
	tFoilCoo xCoo,yCoo;
	double xMom, yMom, xFirst, yFirst;
	int fileType=0;
	bool firstTime=true;	


	foil.X().clear(); foil.Y().clear();
	// first Line is Foil Name
	stream.getline(foilName,cMaxFoilName);
	if(!stream.good()){
		stream.setstate(std::istream::failbit);
		return stream;
	}

	// get the first two separately to find out file format
	stream>>xFirst; stream>>yFirst;

	while(stream.good()){
		stream>>xMom; stream>>yMom;
		// seems to be the amount of points
 		if(xMom==0.&&yMom==0.&&firstTime){
 			fileType=1;
 		}else if(firstTime){
			fileType=0;
			xCoo.push_back(xFirst);
			yCoo.push_back(yFirst);
		}
		if(!stream.good())continue;

		xCoo.push_back(xMom);
		yCoo.push_back(yMom);
		foil.xMax=firstTime ? xMom : std::max(xMom,foil.xMax);
		foil.xMin=firstTime ? xMom : std::min(xMom,foil.xMin);
		foil.yMax=firstTime ? yMom : std::max(yMom,foil.yMax);
		foil.yMin=firstTime ? yMom : std::min(yMom,foil.yMin);
		firstTime=false;
	}

	if(xCoo.size()<3||xCoo.size()!=yCoo.size()){
		stream.setstate(std::istream::failbit);
		return stream;
	}

	if(foilName[strlen(foilName)-1]=='\r')foilName[strlen(foilName)-1]=0;
	
	foil.Name()=foilName;



	// now order points if needed
	if(fileType==0){
		foil.X()=xCoo; foil.Y()=yCoo;
		// clear double points and create spline
		foil.EraseConcurrentPoints();
		foil.spline.Init(foil.X(),foil.Y());
		return stream;
	}

	// search for greatest "jump" in x;
	tFoilCoo::const_iterator iterX=xCoo.begin();
	tFoilCoo::const_iterator iterY=yCoo.begin();

	double dX=0.;
	int i=0,iJump=0;
	
	++iterX;++i;
	while(iterX!=xCoo.end()){
		if(fabs((*iterX)-(*(iterX-1)))>dX){
			dX=fabs((*iterX)-(*(iterX-1)));
			iJump=i-1;
		}
		++iterX; ++i;
	}
	// reorganize the first block before "jump"
	i=0;
	iterX=xCoo.begin();iterY=yCoo.begin();

	while(iterX!=xCoo.end()||iterY!=yCoo.end()){
		if(i<=iJump){
			foil.X().insert(foil.X().begin(),*iterX);
			foil.Y().insert(foil.Y().begin(),*iterY);
		}else{
			foil.X().push_back(*iterX);
			foil.Y().push_back(*iterY);
		}
		++iterX; ++iterY; ++i;
	}

	// clear double points and create spline
	foil.EraseConcurrentPoints();
	foil.spline.Init(foil.X(),foil.Y());
	return stream;
}

#undef cMaxFoilName
