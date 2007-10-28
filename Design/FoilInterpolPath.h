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
#ifndef FOILINTERPOLPATH_H
#define FOILINTERPOLPATH_H

#include "FoilPath.h"
#include "cubicspline.h"


class FoilInterpolPath : public FoilPath
{
protected:
	double nFine;
public:
		FoilInterpolPath(){;}
    FoilInterpolPath(int nFine,Foil &foil,double alpha, double scalex, double scaley,
 					 QPoint Offset, QRect DrawRect){
			CreatePath(nFine,foil,alpha,scalex,scaley,Offset,DrawRect);
			pen.setColor(Qt::green);
		}
	virtual ~FoilInterpolPath(){;};

	virtual void CreatePath(Foil &foil,double alpha, double scalex, double scaley,QPoint Offset, QRectF DrawRect){
		CreatePath(20, foil,alpha, scalex, scaley,Offset, DrawRect);
	}
	virtual void CreatePath(int nFine, Foil &foil,double alpha, double scalex, double scaley,QPoint Offset, QRectF DrawRect){
		if(foil.Empty())return;
		drawRect=DrawRect;
		offset=Offset;
		Scalex=scalex;
		Scaley=scaley;
		Alpha=alpha;

		int width=1;
		double xa,ya;
		const double cosa = cos(ToRad(alpha));
		const double sina = sin(ToRad(alpha));
		for(unsigned int i=0;i<foil.N()-1;i++){
			for(double j=0.;j<nFine;j++){
				const double s=(i+j/(nFine-1.))/(foil.N()-1);
				xa = ((foil(s,0)-0.5)*cosa - foil(s,1)*sina + 0.5)*scalex+Offset.x();
				ya = -((foil(s,0)-0.5)*sina + foil(s,1)*cosa)*scaley+Offset.y();
 				if(i==0&&j==0.){
 					moveTo(xa,ya);
 					continue;
 				}
 				QRectF pointRect(xa-width, ya-width, 2*width, 2*width);
//  				addEllipse(pointRect);
 				lineTo(xa,ya);
			}
		}
	};
};



#endif




