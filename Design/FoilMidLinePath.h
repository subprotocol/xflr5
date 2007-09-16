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
#ifndef FOILMIDLINEPATH_H
#define FOILMIDLINEPATH_H

#include <QPainterPath>
#include <QFont>
#include <QFontMetrics>
#include <math.h>
#include "Foil.h"
#include "helper.h"


class FoilMidLinePath : public QPainterPath
{
private:
	CFoil &foil;
	double alpha;
	double scalex;
	double scaley;
	QPoint Offset;
	QRectF DrawRect;

public:
    FoilMidLinePath(CFoil &Foil,double Alpha, double Scalex, double Scaley,
 					 QPoint offset, QRect drawRect): foil(Foil){
		CreatePath(Alpha,Scalex,Scaley,offset,drawRect);
	};
	virtual ~FoilMidLinePath(){;};

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

		// create path 
		xa = (foil.m_rpMid[0].x-0.5)*cosa - foil.m_rpMid[0].y*sina + 0.5;
		ya = (foil.m_rpMid[0].x-0.5)*sina + foil.m_rpMid[0].y*cosa;
		From.rx() = xa*scalex+Offset.x();
		From.ry() = -ya*scaley+Offset.y();

 		moveTo(From);
 
 		for (int k=1; k<1000; k++){
			xa = (foil.m_rpMid[k].x-0.5)*cosa - foil.m_rpMid[0].y*sina + 0.5;
			ya = (foil.m_rpMid[k].x-0.5)*sina + foil.m_rpMid[0].y*cosa;
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
 				xp = From.x();
 				yp = From.y();
 				if(Intersect(xp,yp, DrawRect, From, To)){
 					moveTo(xp,yp);
 					lineTo(To);
 				}
 			}else moveTo(To);
 			From = To;
 		}
	};
};



#endif




