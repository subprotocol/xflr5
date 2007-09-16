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
#ifndef FOILPATH_H
#define FOILPATH_H

#include <QPainterPath>
#include <QFont>
#include <QFontMetrics>
#include <math.h>
#include "Foil.h"
#include "helper.h"


class FoilPath : public QPainterPath
{
private:
	CFoil &foil;
	double alpha;
	double scalex;
	double scaley;
	QPoint Offset;
	QRectF DrawRect;

public:
    FoilPath(CFoil &Foil,double Alpha, double Scalex, double Scaley,
 					 QPoint offset, QRect drawRect): foil(Foil){
		CreatePath(Alpha,Scalex,Scaley,offset,drawRect);
	};
	virtual ~FoilPath(){;};

	void CreatePath(double Alpha, double Scalex, double Scaley,QPoint offset, QRectF drawRect){
		if(foil.empty())return;
		DrawRect=drawRect;
		Offset=offset;
		scalex=Scalex;
		scaley=Scaley;
		alpha=Alpha;

		QPointF From, To;
		int xp, yp;
		double xa,ya;
		const double cosa = cos(ToRad(alpha));
		const double sina = sin(ToRad(alpha));

		std::vector<double>::const_iterator iterX=foil.Getx().begin();
		std::vector<double>::const_iterator iterY=foil.Gety().begin();

		xa = ((*iterX)-0.5)*cosa - (*iterY)*sina + 0.5;
		ya = ((*iterX)-0.5)*sina + (*iterY)*cosa;
		From.rx() = xa*scalex+Offset.x();
		From.ry() = -ya*scaley+Offset.y();

		//create path
		if(DrawRect.contains(From)) moveTo(From);

		while(iterX!=foil.Getx().end()&&iterY!=foil.Gety().end()){
 			xa = ((*iterX)-0.5)*cosa - (*iterY)*sina+ 0.5;
 			ya = ((*iterX)-0.5)*sina + (*iterY)*cosa;
 			To.rx() = xa*scalex+Offset.x();
 			To.ry() = -ya*scaley+Offset.y();
 
 			if(DrawRect.contains(From) && DrawRect.contains(To)){
 				lineTo(To);
 			}else if(DrawRect.contains(From) && !DrawRect.contains(To)){
 				xp = From.x();
 				yp = From.y();
 				if(Intersect(xp,yp, DrawRect, From, To)){
 					lineTo(xp,yp);				
 				}
 			}else if(!DrawRect.contains(From) && DrawRect.contains(To)){
 				xp = static_cast<int>(From.x());
 				yp = static_cast<int>(From.y());
 				if(Intersect(xp,yp, DrawRect, From, To)){
 					moveTo(xp,yp);
 					lineTo(To);
 				}
 			}else moveTo(To);
 			From = To;
			++iterX; ++iterY;
 		}

		//draw Foil Name
		QFont sansFont("Helvetica", 20);
		QFontMetrics fm(sansFont);
 		//QPointF baseline(DrawRect.x()-fm.width(foil.GetFoilName())*1.1, DrawRect.y()-fm.height()*1.1);
		QPointF baseline(DrawRect.x()+fm.height()*1.1,DrawRect.y()+fm.height()*1.1);
		addText(baseline, sansFont, foil.GetFoilName());
	};
};



#endif




