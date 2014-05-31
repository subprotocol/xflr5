/****************************************************************************

    Reference Foil Class
	Copyright (C) 2003-2014 Andre Deperrois adeperrois@xflr5.com

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



#include "Foil.h"
#include "Spline.h"
#include <math.h>
#include "../globals.h"
#include <QTextStream>
#include <QMessageBox>
#include <QtDebug>

QList <void *> Foil::s_oaFoil;
Foil *Foil::s_pCurFoil = NULL;

/**
 * The public constructor.
 */
Foil::Foil()
{
	m_FoilStyle = 0;
	m_FoilWidth = 1;
	m_FoilColor  = randomColor();

	m_iHighLight = -1;

	m_bCenterLine = false;
	m_bPoints     = false;
	m_bVisible    = true;
	m_bSaved      = true;

	m_FoilName = "";
	m_FoilDescription = "";

	m_fCamber     = 0.0;
	m_fXCamber    = 0.0;
	m_fThickness  = 0.0;
	m_fXThickness = 0.0;

	n = 0;
	memset(x, 0, sizeof(x));
	memset(y, 0, sizeof(y));
	memset(nx, 0, sizeof(nx));
	memset(ny, 0, sizeof(ny));

	nb = 0;
	memset(xb, 0, sizeof(xb));
	memset(yb, 0, sizeof(yb));


	m_iInt = 0;
	m_iExt = 0;
	m_iBaseExt = 0;
	m_iBaseInt = 0;

	m_Gap  = 0.0;

	memset(m_rpExtrados, 0, sizeof(m_rpExtrados));
	memset(m_rpIntrados, 0, sizeof(m_rpIntrados));
	memset(m_rpMid, 0, sizeof(m_rpMid));

	memset(m_BaseExtrados, 0, sizeof(m_BaseExtrados));
	memset(m_BaseIntrados, 0, sizeof(m_BaseIntrados));

	m_bTEFlap     = false;
	m_TEFlapAngle = 0.0;
	m_TEXHinge    = 80.0;
	m_TEYHinge    = 50.0;

	m_bLEFlap     = false;
	m_LEFlapAngle = 0.0;
	m_LEXHinge    = 20.0;
	m_LEYHinge    = 50.0;
}


/**
* Constructs the postion of the foil's mid camber line.
* Calculates the foil's thickness and camber, if requested.
*@param bParams true if the max thickness and camber properties shold be calculated
*/
void Foil::CompMidLine(bool bParams)
{
	static int l;
	static double xt, yex, yin, step;

	if(bParams)
	{
		m_fThickness  = 0.0;
		m_fCamber     = 0.0;
		m_fXCamber    = 0.0;
		m_fXThickness = 0.0;
	}

	//	double length = GetLength();
	step = (m_rpExtrados[m_iExt].x-m_rpExtrados[0].x)/(double)MIDPOINTCOUNT;

	for (l=0; l<=MIDPOINTCOUNT; l++)
	{
		xt = m_rpExtrados[0].x + l*step;
		yex = GetUpperY((double)l*step);
		yin = GetLowerY((double)l*step);

		m_rpMid[l].x = xt;
		m_rpMid[l].y = (yex+yin)/2.0;

		if(bParams)
		{
			if(qAbs(yex-yin)>m_fThickness)
			{
				m_fThickness  = qAbs(yex-yin);
				m_fXThickness = xt;
			}
			if(qAbs(m_rpMid[l].y)>qAbs(m_fCamber))
			{
				m_fCamber  = m_rpMid[l].y;
				m_fXCamber = xt;
			}
		}
	}

	//use xfoil geopar, getcam routines: based on a splined foil and more precise therefore
/*	XFoil *pXFoil = new XFoil;
	pXFoil->InitXFoilGeometry(this);

	m_iMid = pXFoil->ncam;
	for(int i=1; i<=pXFoil->ncam; i++)
	{
		m_rpMid[i-1].x = pXFoil->xcam[i];
		m_rpMid[i-1].y = pXFoil->ycam[i];
	}

	if(bParams)
	{
		m_fXThickness = pXFoil->xthick;
		m_fThickness  = pXFoil->thick;
		m_fXCamber    = pXFoil->xcambr;
		m_fCamber     = pXFoil->cambr;
	}

	delete pXFoil;*/
}


/**
* Copies the data from an existing foil and maps it to this foil's variables.
* @param pSrcFoil a pointer to the reference foil from which the data wil be copied
*/
void Foil::CopyFoil(Foil *pSrcFoil)
{
	memcpy(x, pSrcFoil->x,  sizeof(pSrcFoil->x));
	memcpy(y, pSrcFoil->y,  sizeof(pSrcFoil->y));
	memcpy(xb,pSrcFoil->xb, sizeof(pSrcFoil->xb));
	memcpy(yb,pSrcFoil->yb, sizeof(pSrcFoil->yb));
	memcpy(nx,pSrcFoil->nx, sizeof(pSrcFoil->nx));
	memcpy(ny,pSrcFoil->ny, sizeof(pSrcFoil->ny));
	memcpy(m_rpMid,        pSrcFoil->m_rpMid,        sizeof(m_rpMid));
	memcpy(m_rpBaseMid,    pSrcFoil->m_rpBaseMid,    sizeof(m_rpBaseMid));
	memcpy(m_rpExtrados,   pSrcFoil->m_rpExtrados,   sizeof(m_rpExtrados));
	memcpy(m_rpIntrados,   pSrcFoil->m_rpIntrados,   sizeof(m_rpIntrados));
	memcpy(m_BaseExtrados, pSrcFoil->m_BaseExtrados, sizeof(m_BaseExtrados));
	memcpy(m_BaseIntrados, pSrcFoil->m_BaseIntrados, sizeof(m_BaseIntrados));
	m_iExt = pSrcFoil->m_iExt;
	m_iInt = pSrcFoil->m_iInt;
	m_iBaseExt = pSrcFoil->m_iBaseExt;
	m_iBaseInt = pSrcFoil->m_iBaseInt;
	m_Gap  = pSrcFoil->m_Gap;
	m_TE.x = pSrcFoil->m_TE.x;
	m_TE.y = pSrcFoil->m_TE.y;
	m_LE.x = pSrcFoil->m_LE.x;
	m_LE.y = pSrcFoil->m_LE.y;

	m_fThickness  = pSrcFoil->m_fThickness;
	m_fXThickness = pSrcFoil->m_fXThickness;
	m_fCamber     = pSrcFoil->m_fCamber;
	m_fXCamber    = pSrcFoil->m_fXCamber;

	n  = pSrcFoil->n;
	nb = pSrcFoil->nb;
	m_FoilName  = pSrcFoil->m_FoilName;

	m_bLEFlap     = pSrcFoil->m_bLEFlap;
	m_LEFlapAngle = pSrcFoil->m_LEFlapAngle;
	m_LEXHinge    = pSrcFoil->m_LEXHinge;
	m_LEYHinge    = pSrcFoil->m_LEYHinge;

	m_bTEFlap     = pSrcFoil->m_bTEFlap;
	m_TEFlapAngle = pSrcFoil->m_TEFlapAngle;
	m_TEXHinge    = pSrcFoil->m_TEXHinge;
	m_TEYHinge    = pSrcFoil->m_TEYHinge;

	m_FoilColor   = pSrcFoil->m_FoilColor;
	m_FoilStyle   = pSrcFoil->m_FoilStyle;
	m_FoilWidth   = pSrcFoil->m_FoilWidth;
	m_bCenterLine = pSrcFoil->m_bCenterLine;
	m_bPoints     = pSrcFoil->m_bPoints;
}




/**
* Derotates the fol's geometry i.e. aligns the mid-line with the x-axis.
* @return the angle, in degrees, by which the foil has been de-rotated
*/
double Foil::DeRotate()
{
	//De-rotates the foil,

//	double xle, xte, yle, yte;
	double angle, cosa, sina;
	int i;
	// first find offset
	//and translate the leading edge to the origin point
	for (i=0; i<nb; i++)
	{
		xb[i] -= m_LE.x;
		yb[i] -= m_LE.y;
	}

	for (i=0; i<n; i++)
	{
		x[i] -= m_LE.x;
		y[i] -= m_LE.y;
	}
//	InitFoil();//to get the new LE and TE

//	xle = (m_rpIntrados[0].x+m_rpExtrados[0].x)/2.0;
//	yle = (m_rpIntrados[0].y+m_rpExtrados[0].y)/2.0;

//	xte = (m_rpIntrados[m_iInt].x+m_rpExtrados[m_iExt].x)/2.0;
//	yte = (m_rpIntrados[m_iInt].y+m_rpExtrados[m_iExt].y)/2.0;

	// then find current angle
//	angle = atan2(yte-yle, xte-xle);// xle=tle=0;
	angle = atan2(m_TE.y-m_LE.y, m_TE.x-m_LE.x);// xle=tle=0;

	//rotate about the L.E.
	cosa = cos(angle);
	sina = sin(angle);

/*	for (i=0; i<nb; i++)
	{
		xb[i] = ( (xb[i]-m_LE.x)*cosa + (yb[i]-m_LE.y)*sina);
		yb[i] = (-(xb[i]-m_LE.x)*sina + (yb[i]-m_LE.y)*cosa);
	}

	for (i=0; i<n; i++)
	{
		x[i] = ( (x[i]-m_LE.x)*cosa + (y[i]-m_LE.y)*sina);
		y[i] = (-(x[i]-m_LE.x)*sina + (y[i]-m_LE.y)*cosa);
	}*/

	for (i=0; i<nb; i++)
	{
		xb[i] = ( (xb[i])*cosa + (yb[i])*sina);
		yb[i] = (-(xb[i])*sina + (yb[i])*cosa);
	}

	for (i=0; i<n; i++)
	{
		x[i] = ( (x[i])*cosa + (y[i])*sina);
		y[i] = (-(x[i])*sina + (y[i])*cosa);
	}
	
	InitFoil();

	return angle*180.0/PI;
}



/**
 * Draws the foil in the client area.
 * @todo separate foil object and GUI.
 * @param painter a reference to the QPainter object on which the foil will be drawn
 * @param alpha the rotation angle in degrees of the foil
 * @param scalex the scaling factor in the x-direction
 * @param scaley the scaling factor in the y-direction
 * @param Offset the foil offset in the client area
 */
void Foil::DrawFoil(QPainter &painter, double const &alpha, double const &scalex, double const &scaley, QPoint const &Offset)
{
	static double xa, ya, sina, cosa;
	static QPoint From, To;
	static QRect R;
	static int k;
	static QPen FoilPen, HighPen;

	FoilPen.setColor(m_FoilColor);
	FoilPen.setWidth(m_FoilWidth);
	FoilPen.setStyle(getStyle(m_FoilStyle));
	painter.setPen(FoilPen);

	HighPen.setColor(QColor(255,0,0));

	cosa = cos(alpha*PI/180.0);
	sina = sin(alpha*PI/180.0);

	xa = (x[0]-0.5)*cosa - y[0]*sina + 0.5;
	ya = (x[0]-0.5)*sina + y[0]*cosa;
	From.rx() = (int)( xa*scalex + Offset.x());
	From.ry() = (int)(-ya*scaley + Offset.y());

	if(m_bPoints)
	{
		R.setLeft((int)( xa*scalex) + Offset.x() -2);
		R.setTop( (int)(-ya*scaley) + Offset.y() -2);
		R.setWidth(4);
		R.setHeight(4);
		painter.drawRect(R);
	}
	if(m_bPoints && m_iHighLight==0)
	{
		HighPen.setWidth(2);
		painter.setPen(HighPen);
		painter.drawRect(R);
		painter.setPen(FoilPen);
	}

	for (k=1; k<n; k++)
	{
		xa = (x[k]-0.5)*cosa - y[k]*sina+ 0.5;
		ya = (x[k]-0.5)*sina + y[k]*cosa;
		To.rx() = (int)( xa*scalex+Offset.x());
		To.ry() = (int)(-ya*scaley+Offset.y());

		painter.drawLine(From,To);

		if(m_bPoints)
		{
			R.setLeft((int)( xa*scalex) + Offset.x() -2);
			R.setTop( (int)(-ya*scaley) + Offset.y() -2);
			R.setWidth(3);
			R.setHeight(3);
			painter.drawRect(R);
		}
		if(m_bPoints && m_iHighLight==k)
		{
 			HighPen.setWidth(2);
			painter.setPen(HighPen);
			painter.drawRect(R);
			painter.setPen(FoilPen);
		}

		From = To;
	}
}


/**
 * Draws the foil's mid line in the client area.
 * @todo separate foil object and GUI
 * @param painter a refernce to the QPainter object on which the foil will be drawn
 * @param alpha the rotation angle in degrees of the foil
 * @param scalex the scaling factor in the x-direction
 * @param scaley the scaling factor in the y-direction
 * @param Offset the foil offset in the client area
 */
void Foil::DrawMidLine(QPainter &painter, double const &scalex, double const &scaley, QPoint const &Offset)
{
	static QPoint From, To;
	static int k;
	static QPen FoilPen;
	FoilPen.setColor(m_FoilColor);
	FoilPen.setWidth(m_FoilWidth);
	FoilPen.setStyle(Qt::DashLine);
	painter.setPen(FoilPen);

	From.rx() = (int)( m_rpMid[0].x*scalex)  +Offset.x();
	From.ry() = (int)(-m_rpMid[0].y*scaley)  +Offset.y();


	for (k=0; k<=MIDPOINTCOUNT+1; k++)
	{
		To.rx() = (int)( m_rpMid[k].x*scalex)+Offset.x();
		To.ry() = (int)(-m_rpMid[k].y*scaley)+Offset.y();

		painter.drawLine(From, To);
		From = To;
	}
}





/**
 * Draws the foil's points in the client area.
 * @todo separate foil object and GUI
 * @param painter a refernce to the QPainter object on which the foil will be drawn
 * @param alpha the rotation angle in degrees of the foil
 * @param scalex the scaling factor in the x-direction
 * @param scaley the scaling factor in the y-direction
 * @param Offset the foil offset in the client area
 */
void Foil::DrawPoints(QPainter &painter, double const &scalex, double const &scaley, QPoint const &Offset)
{
	static int width;
	static QPoint pt1;

	width = 2;

	static QPen FoilPen, HighPen;
	FoilPen.setColor(m_FoilColor);
	FoilPen.setWidth(m_FoilWidth);
	FoilPen.setStyle(Qt::SolidLine);
	painter.setPen(FoilPen);

	HighPen.setColor(QColor(255,0,0));

	for (int i=0; i<n;i++)
	{
		pt1.rx() = (int)( x[i]*scalex + Offset.x() - width);
		pt1.ry() = (int)(-y[i]*scaley + Offset.y() - width);

		painter.drawRect(pt1.x(), pt1.y(), 4, 4) ;
	}
	if(m_iHighLight>=0)
	{
		HighPen.setWidth(2);
		painter.setPen(HighPen);

		pt1.rx() = (int)( x[m_iHighLight]*scalex + Offset.x() - width);
		pt1.ry() = (int)(-y[m_iHighLight]*scaley + Offset.y() - width);

		painter.drawRect(pt1.x(), pt1.y(), 4, 4);
	}
}


/**
 * Exports the foil geometry to a text .dat file.
 * @param out the QtextStream to which the output will be directed
 * @return true if the operation has been successful, false otherwise
 */
bool Foil::ExportFoil(QTextStream &out)
{
    int i;

    QString strOut;

	out << m_FoilName +"\n";

    for (i=0; i< n; i++)
    {
		strOut = QString("%1    %2\n").arg(x[i],8,'f',5).arg(y[i],8,'f',5);
        out << strOut;
    }

    return true;
}


/**
 * Returns the area defined by the foil's contour, in normalized units.
 * @return the foil's internal area
 */
double Foil::GetArea()
{
	int i;
	double area = 0.0;
	for (i=0; i<nb-1; i++)
	{
		area +=  qAbs((yb[i+1]+yb[i])/2.0 * (xb[i+1]-xb[i]));
	}
	return area;
}



/**
 * Returns the y-position on the lower surface, at a specified chord position.
 * @param &x the chordwise position
 * @return the y-position
 */
double Foil::GetBaseLowerY(double x)
{
	static int i;
	static double y;

	x = m_BaseIntrados[0].x + x*(m_BaseIntrados[m_iInt].x-m_BaseIntrados[0].x);//in case there is a flap which reduces the length

	for (i=0; i<m_iBaseInt; i++)
	{
		if (m_BaseIntrados[i].x <m_BaseIntrados[i+1].x  &&  m_BaseIntrados[i].x <= x && x<=m_BaseIntrados[i+1].x )
		{
			y = (m_BaseIntrados[i].y   + (m_BaseIntrados[i+1].y-m_BaseIntrados[i].y)
								   /(m_BaseIntrados[i+1].x-m_BaseIntrados[i].x)*(x-m_BaseIntrados[i].x));
			return y;
		}
	}
	return 0.0;
}




/**
 * Returns the y-position on the upper surface, at a specified chord position
 * @param &x the chordwise position
 * @return the y-position
 */
double Foil::GetBaseUpperY(double x)
{
	static double y;
	static int i;

	x = m_BaseExtrados[0].x + x*(m_BaseExtrados[m_iExt].x-m_BaseExtrados[0].x);//in case there is a flap which reduces the length

	for (i=0; i<m_iBaseExt; i++)
	{
		if (m_BaseExtrados[i].x <m_BaseExtrados[i+1].x  &&  m_BaseExtrados[i].x <= x && x<=m_BaseExtrados[i+1].x )
		{
			y = (m_BaseExtrados[i].y  + (m_BaseExtrados[i+1].y-m_BaseExtrados[i].y)
									 /(m_BaseExtrados[i+1].x-m_BaseExtrados[i].x)*(x-m_BaseExtrados[i].x));
			return y;
		}
	}
	return 0.0;
}


/**
 * Returns the slope in radians on the lower surface, at a specified chord position.
 * @param &x the chordwise position
 * @return the slope in radians
 */
double Foil::GetBotSlope(double const &x)
{
	//returns the bottom slope at position x
	static int i;
	static double dx, dy;
	for (i=0; i<m_iInt; i++)
	{
		if ((m_rpIntrados[i].x <= x) && (x < m_rpIntrados[i+1].x))
		{
			dx = m_rpIntrados[i+1].x-m_rpIntrados[i].x;
			dy = m_rpIntrados[i+1].y-m_rpIntrados[i].y;
			return -atan2(dy,dx);
		}
	}
	return 0.0;
}


/**
 * Returns the slope in radians on the upperer surface, at a specified chord position.
 * @param &x the chordwise position
 * @return the slope in radians
 */
double Foil::GetTopSlope(double const &x)
{
	//returns the upper slope at position x
	static int i;
	for (i=0; i<m_iExt; i++)
	{
		if ((m_rpExtrados[i].x <= x) && (x < m_rpExtrados[i+1].x))
		{
			double dx = m_rpExtrados[i+1].x-m_rpExtrados[i].x;
			double dy = m_rpExtrados[i+1].y-m_rpExtrados[i].y;
			return -atan2(dy,dx);
		}
	}
	return 0.0;
}



/**
 * Returns the camber value at a specified chord position, in normalized units.
 * @param &x the chordwise position
 * @return the camber value
 */
double Foil::GetCamber(double const &x)
{
	//returns the camber value at position x
	for (int i=0; i<MIDPOINTCOUNT; i++)
	{
		if ((m_rpMid[i].x <= x) && (x < m_rpMid[i+1].x))
		{
			return (m_rpMid[i+1].y+m_rpMid[i].y)/2.0;
		}
	}
	return 0.0;
}


/**
 * Returns the camber angle in degrees at a specified chord position.
 * @param &x the chordwise position
 * @return the camber angle, in degrees
 */
double Foil::GetCamberSlope(double const &x)
{
	//returns the camber slope at position x
	static int i;
	for (i=0; i<MIDPOINTCOUNT-1; i++)
	{
		if ((m_rpMid[i].x <= x) && (x < m_rpMid[i+1].x))
		{
			double dx = m_rpMid[i+1].x-m_rpMid[i].x;
			double dy = m_rpMid[i+1].y-m_rpMid[i].y;
			return atan2(dy,dx);
		}
	}
	if(x>=1.0)
	{
			double dx = m_rpMid[MIDPOINTCOUNT-1].x-m_rpMid[MIDPOINTCOUNT-2].x;
			double dy = m_rpMid[MIDPOINTCOUNT-1].y-m_rpMid[MIDPOINTCOUNT-2].y;
			return atan2(dy,dx);
	}
	return 0.0;
}


/**
 *Returns the foil's length.
 *@return the foil's length, in relative units
*/
double Foil::length()
{
	return qMax(m_rpExtrados[m_iExt].x, m_rpExtrados[m_iInt].x);
}


/**
* Returns the y-coordinate on the current foil's lower surface at the x position.
*@param x the chordwise position
*@return the position on the lower surface
*/
double Foil::GetLowerY(double x)
{
	x = m_rpIntrados[0].x + x*(m_rpIntrados[m_iInt].x-m_rpIntrados[0].x)*.999999999;//in case there is a flap which reduces the length
	static double y;
	for (int i=0; i<m_iInt; i++)
	{
		if (m_rpIntrados[i].x <m_rpIntrados[i+1].x  &&
			m_rpIntrados[i].x <= x && x<=m_rpIntrados[i+1].x )
		{
			y = (m_rpIntrados[i].y 	+ (m_rpIntrados[i+1].y-m_rpIntrados[i].y)
								 /(m_rpIntrados[i+1].x-m_rpIntrados[i].x) * (x-m_rpIntrados[i].x));
			return y;
		}
	}
	return 0.0;
}



/**
* Returns the y-coordinate and the normal to the surface on the foil's lower surface at the x position.
*@param x the chordwise position
*@param &y a reference to variable holding the position on the lower surface
*@param &normx a reference to variable holding the x-component of the normal to the surface
*@param &normy a reference to variable holding the y-component of the normal to the surface
*/
void Foil::GetLowerY(double x, double &y, double &normx, double &normy)
{
	static double nabs;
	static int i;

	x = m_rpIntrados[0].x + x*(m_rpIntrados[m_iInt].x-m_rpIntrados[0].x)*.999999999;//in case there is a flap which reduces the length
//	x = m_rpExtrados[0].x + x*.99999999;
	for (i=0; i<m_iInt; i++)
	{
		if (m_rpIntrados[i].x <m_rpIntrados[i+1].x  &&  m_rpIntrados[i].x <= x && x<=m_rpIntrados[i+1].x )
		{
			y = (m_rpIntrados[i].y 	+ (m_rpIntrados[i+1].y-m_rpIntrados[i].y) /(m_rpIntrados[i+1].x-m_rpIntrados[i].x)*(x-m_rpIntrados[i].x));
			nabs = sqrt(  (m_rpIntrados[i+1].x-m_rpIntrados[i].x) * (m_rpIntrados[i+1].x-m_rpIntrados[i].x)
					  + (m_rpIntrados[i+1].y-m_rpIntrados[i].y) * (m_rpIntrados[i+1].y-m_rpIntrados[i].y));
			normx = ( m_rpIntrados[i+1].y - m_rpIntrados[i].y)/nabs;
			normy = (-m_rpIntrados[i+1].x + m_rpIntrados[i].x)/nabs;
			return;
		}
	}
}


/**
* Returns the y-coordinate on the current foil's mid line at the x position.
* @param x the chordwise position
* @return the position on the mid line
*/
double Foil::GetMidY(double xl)
{
	xl = m_rpMid[0].x + xl*(m_rpMid[MIDPOINTCOUNT].x-m_rpMid[0].x)*.999999999;//in case there is a flap which reduces the length

	if(xl<m_rpMid[0].x || xl>m_rpMid[MIDPOINTCOUNT].x) return 0.0;

	for(int im=0; im<MIDPOINTCOUNT; im++)
	{
		if(m_rpMid[im].x<=xl && xl<=m_rpMid[im+1].x)
		{
			return  m_rpMid[im].y + (xl-m_rpMid[im].x) *   (m_rpMid[im+1].y - m_rpMid[im].y)
												/ (m_rpMid[im+1].x - m_rpMid[im].x);
		}
	}
	return 0.0;
}




/**
* Returns the y-coordinate on the current foil's upper surface at the x position.
* @param x the chordwise position
* @return the position on the upper surface
*/
double Foil::GetUpperY(double x)
{
	// Returns the y-coordinate on the current foil's upper surface at the x position
	x = m_rpExtrados[0].x + x*(m_rpExtrados[m_iExt].x-m_rpExtrados[0].x)*.999999999;//in case there is a flap which reduces the length

	for (int i=0; i<m_iExt; i++)
	{
		if (m_rpExtrados[i].x <m_rpExtrados[i+1].x  &&
		    m_rpExtrados[i].x <= x && x<=m_rpExtrados[i+1].x )
		{
			return (m_rpExtrados[i].y 	+ (m_rpExtrados[i+1].y-m_rpExtrados[i].y)
									 /(m_rpExtrados[i+1].x-m_rpExtrados[i].x) * (x-m_rpExtrados[i].x));
		}
	}
	return 0.0;
}



/**
* Returns the y-coordinate and the normal to the surface on the foil's upper surface at the x position.
*@param x the chordwise position
*@param &y a reference to variable holding the position on the surface
*@param &normx a reference to variable holding the x-component of the normal to the surface
*@param &normy a reference to variable holding the y-component of the normal to the surface
*/
void Foil::GetUpperY(double x, double &y, double &normx, double &normy)
{
	static double nabs;
	static int i;

	// Returns the y-coordinate on the current foil's upper surface at the x position
	x = m_rpExtrados[0].x + x*(m_rpExtrados[m_iExt].x-m_rpExtrados[0].x)*.999999999;//in case there is a flap which reduces the length

	for (i=0; i<m_iExt; i++)
	{
		if (m_rpExtrados[i].x <m_rpExtrados[i+1].x  &&  m_rpExtrados[i].x <= x && x<=m_rpExtrados[i+1].x )
		{
			y = (m_rpExtrados[i].y 	+ (m_rpExtrados[i+1].y-m_rpExtrados[i].y) / (m_rpExtrados[i+1].x-m_rpExtrados[i].x)*(x-m_rpExtrados[i].x));
			nabs = sqrt(  (m_rpExtrados[i+1].x-m_rpExtrados[i].x) * (m_rpExtrados[i+1].x-m_rpExtrados[i].x) 
					  + (m_rpExtrados[i+1].y-m_rpExtrados[i].y) * (m_rpExtrados[i+1].y-m_rpExtrados[i].y));
			normx = (-m_rpExtrados[i+1].y + m_rpExtrados[i].y)/nabs;
			normy = ( m_rpExtrados[i+1].x - m_rpExtrados[i].x)/nabs;
			return;
		}
	}
}


/**
* Initializes the foil geometry, constructs the upper and lower points, and applies the flap deflection if requested.
*/
bool Foil::InitFoil()
{
	// at this point, coordinates have been loaded
	// so has been the number of points defining the foil
	bool bNotFound = true;
	int k = 0;

	//first time is to calculate the base foil's thickness and camber

	if(nb<=0)
	{
		return false;
	}

	while (k<nb)
	{
		if (xb[k+1] < xb[k]) 
		{
			k++;
		}
		else 
		{
			if(bNotFound)
			{
				m_iBaseExt = k;
				m_BaseExtrados[k].x = xb[k];
				m_BaseExtrados[k].y = yb[k];
				bNotFound = false;
			}
			m_BaseIntrados[k-m_iBaseExt].x = xb[k];
			m_BaseIntrados[k-m_iBaseExt].y = yb[k];
			k++;
		}
	}
	m_iBaseInt = nb-m_iBaseExt-1;
	m_BaseIntrados[nb-m_iBaseExt-1].x = xb[nb-1];
	m_BaseIntrados[nb-m_iBaseExt-1].y = yb[nb-1];
	for (k=0; k<=m_iBaseExt;k++)
	{
		m_BaseExtrados[k].x = xb[m_iBaseExt-k];
		m_BaseExtrados[k].y = yb[m_iBaseExt-k];
	}

	memcpy(m_rpExtrados, m_BaseExtrados, sizeof(m_rpExtrados));
	memcpy(m_rpIntrados, m_BaseIntrados, sizeof(m_rpIntrados));
	m_iExt = m_iBaseExt;
	m_iInt = m_iBaseInt;

	CompMidLine(true);
	memcpy(m_rpBaseMid, m_rpMid, sizeof(m_rpBaseMid));


	m_Gap = m_BaseExtrados[m_iBaseExt].y-m_BaseIntrados[m_iBaseInt].y;

	m_LE.x = (m_BaseIntrados[0].x+m_BaseExtrados[0].x)/2.0;
	m_LE.y = (m_BaseIntrados[0].y+m_BaseExtrados[0].y)/2.0;
	
	m_TE.x = (m_BaseIntrados[m_iBaseInt].x+m_BaseExtrados[m_iBaseExt].x)/2.0;
	m_TE.y = (m_BaseIntrados[m_iBaseInt].y+m_BaseExtrados[m_iBaseExt].y)/2.0;


	//the second time is to get the mid line of the current foil
	//i.e. with flaps eventually
	//used for the VLM analysis
	k=0;
	bNotFound = true;
	while (k<n)
	{
		if (x[k+1] < x[k]) 
		{
			k++;
		}
		else {
			if(bNotFound)
			{
				m_iExt = k;
				m_rpExtrados[k].x = x[k];
				m_rpExtrados[k].y = y[k];
				bNotFound = false;
			}
			m_rpIntrados[k-m_iExt].x = x[k];
			m_rpIntrados[k-m_iExt].y = y[k];
			k++;
		}
	}
	m_iInt = n-m_iExt-1;
	m_rpIntrados[n-m_iExt-1].x = x[n-1];
	m_rpIntrados[n-m_iExt-1].y = y[n-1];
	for (k=0; k<=m_iExt;k++)
	{
		m_rpExtrados[k].x = x[m_iExt-k];
		m_rpExtrados[k].y = y[m_iExt-k];
	}

	CompMidLine(false);
	return true;
}


/**
*ABCD are assumed to lie in the xy plane
*@return true and intersection point M if AB and CD intersect inside, false and intersection point M if AB and CD intersect outside
*/
bool Foil::Intersect(CVector const &A, CVector const &B, CVector const &C, CVector const &D, CVector *M)
{
	static double Det, Det1, Det2, t, u;
	static CVector AB, CD;

	M->x = 0.0;
	M->y = 0.0;
	M->z = 0.0;
	AB.Set(B.x-A.x, B.y-A.y, B.z-A.z);
	CD.Set(D.x-C.x, D.y-C.y, D.z-C.z);

	//Cramer's rule

	Det  = -AB.x * CD.y + CD.x * AB.y;
	if(Det==0.0)
	{
		//vectors are parallel, no intersection
		return false;
	}
	Det1 = -(C.x-A.x)*CD.y + (C.y-A.y)*CD.x;
	Det2 = -(C.x-A.x)*AB.y + (C.y-A.y)*AB.x;

	t = Det1/Det;
	u = Det2/Det;

	M->x = A.x + t*AB.x;
	M->y = A.y + t*AB.y;

	if (0.0<=t && t<=1.0 && 0.0<=u && u<=1.0) return true;//M is between A and B
	else                                      return false;//M is outside
}


/**
*Returns the index of foil's point which coincides with the input point, if any, otherwise returns -10.
*@param &Real the Cvector which defines the input point
*/
int Foil::IsPoint(CVector const &Real)
{
	static int k;
	for (k=0; k<n; k++)
	{
		if(qAbs(Real.x-x[k])<0.005 && qAbs(Real.y-y[k])<0.005) return k;
	}
	return -10;
}

/**
* Normalizes the base foil's lengh to unity.
* The current foil's length is modified by the same ratio.
* @return the foil's former length
*/
double Foil::NormalizeGeometry()
{
	int i;
	double xmin = 1.0;
	double xmax = 0.0;

	for (i=0; i<nb; i++)
	{
		xmin = qMin(xmin, xb[i]);
		xmax = qMax(xmax, xb[i]);
	}
	double length = xmax - xmin;

	//reset origin
	for (i=0; i<nb; i++) xb[i] -= xmin;

	//set length to 1. and cancel y offset
	for(i=0; i<nb; i++)
	{
		xb[i] = xb[i]/length;
		yb[i] = yb[i]/length;
	}
	double yTrans = yb[0];
	for(i=0; i<nb; i++)	yb[i] -= yTrans;

	//reset origin
	for (i=0; i<n; i++)
	{
		x[i] -= xmin;
	}

	//set length to 1. and cancel y offset
	for(i=0; i<n; i++)
	{
		x[i] = x[i]/length;
		y[i] = y[i]/length;
	}
	yTrans = y[0];
	for(i=0; i<n; i++)	y[i] -= yTrans;

	return length;
}


/**
 * Loads or Saves the data of this foil to a binary file.
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool Foil::Serialize(QDataStream &ar, bool bIsStoring)
{
	// saves or loads the foil to the archive ar

	int ArchiveFormat = 1006;
	// 1006 : QFLR5 v0.02 : added Foil description
	// 1005 : added LE Flap data
	// 1004 : added Points and Centerline property
	// 1003 : added Visible property
	// 1002 : added color and style save
	// 1001 : initial format
	int p, j;
	float f,ff;

	if(bIsStoring)
	{
		ar << ArchiveFormat;
		WriteCString(ar, m_FoilName);
		WriteCString(ar, m_FoilDescription);
		ar << m_FoilStyle << m_FoilWidth;
		WriteCOLORREF(ar, m_FoilColor);
		if (m_bVisible)		ar << 1; else ar << 0;
		if (m_bPoints)		ar << 1; else ar << 0;//1004
		if (m_bCenterLine)	ar << 1; else ar << 0;//1004
		if (m_bLEFlap)		ar << 1; else ar << 0;
		ar << (float)m_LEFlapAngle << (float)m_LEXHinge << (float)m_LEYHinge;
		if (m_bTEFlap)		ar << 1; else ar << 0;
		ar << (float)m_TEFlapAngle << (float)m_TEXHinge << (float)m_TEYHinge;
		ar << 1.f << 1.f << 9.f;//formerly transition parameters
		ar << nb;
		for (j=0; j<nb; j++)
		{
			ar << (float)xb[j] << (float)yb[j];
		}
		ar << n;
		for (j=0; j<n; j++)
		{
			ar << (float)x[j] << (float)y[j];
		}
		return true;
	}
	else 
	{
		ar >> ArchiveFormat;
		if(ArchiveFormat<1000||ArchiveFormat>1010)
			return false;

		ReadCString(ar, m_FoilName);
		if(ArchiveFormat>=1006)
		{
			ReadCString(ar, m_FoilDescription);
		}
		if(ArchiveFormat>=1002)
		{
			ar >> m_FoilStyle >> m_FoilWidth;
			ReadCOLORREF(ar, m_FoilColor);

		}
		if(ArchiveFormat>=1003)
		{
			ar >> p;
			if(p) m_bVisible = true; else m_bVisible = false;
		}
		if(ArchiveFormat>=1004)
		{
			ar >> p;
			if(p) m_bPoints = true; else m_bPoints = false;
			ar >> p;
			if(p) m_bCenterLine = true; else m_bCenterLine = false;
		}

		if(ArchiveFormat>=1005)
		{
			ar >> p;
			if (p) m_bLEFlap = true; else m_bLEFlap = false;
			ar >> f; m_LEFlapAngle =f;
			ar >> f; m_LEXHinge = f;
			ar >> f; m_LEYHinge = f;
		}
		ar >> p;
		if (p) m_bTEFlap = true; else m_bTEFlap = false;
		ar >> f; m_TEFlapAngle =f;
		ar >> f; m_TEXHinge = f;
		ar >> f; m_TEYHinge = f;

		ar >> f >> f >> f; //formerly transition parameters
		ar >> nb;
		if(nb>IBX) return false;

		for (j=0; j<nb; j++)
		{
			ar >> f >> ff;
			xb[j]  = f;  yb[j]=ff;
		}
		if(ArchiveFormat>=1001)
		{
			ar >> n;
			if(n>IBX) return false;

			for (j=0; j<n; j++)
			{
				ar >> f >> ff;
				x[j]=f; y[j]=ff;
			}
			if(nb==0 && n!=0)
			{
				nb = n;
				memcpy(xb,x, sizeof(xb));
				memcpy(yb,y, sizeof(yb));
			}
		}
		else
		{
			memcpy(x,xb, sizeof(xb));
			memcpy(y,yb, sizeof(yb));
			n=nb;
		}


		InitFoil();
		SetFlap();

		return true;
	}
}




/**
 * Loads or Saves the data of this foil to a binary file.
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool Foil::SerializeFoilXFL(QDataStream &ar, bool bIsStoring)
{
	int j;

	int ArchiveFormat = 100006;
	// 100006 : first version of new xfl format

	if(bIsStoring)
	{
		ar << ArchiveFormat;
		ar << m_FoilName;
		ar << m_FoilDescription;
		ar << m_FoilStyle << m_FoilWidth;
		ar <<  m_FoilColor;
		ar << m_bVisible << m_bPoints << m_bCenterLine << m_bLEFlap << m_bTEFlap;
		ar << m_LEFlapAngle << m_LEXHinge << m_LEYHinge;
		ar << m_TEFlapAngle << m_TEXHinge << m_TEYHinge;
		ar << nb;
		for (j=0; j<nb; j++)
		{
			ar << xb[j] << yb[j];
		}
		return true;
	}
	else
	{
		ar >> ArchiveFormat;
		ar >> m_FoilName;
		ar >> m_FoilDescription;
		ar >> m_FoilStyle >> m_FoilWidth;
		ar >>  m_FoilColor;
		ar >> m_bVisible >> m_bPoints >> m_bCenterLine >> m_bLEFlap >> m_bTEFlap;
		ar >> m_LEFlapAngle >> m_LEXHinge >> m_LEYHinge;
		ar >> m_TEFlapAngle >> m_TEXHinge >> m_TEYHinge;
		ar >> nb;
		for (j=0; j<nb; j++)
		{
			ar >> xb[j] >> yb[j];
		}

		memcpy(x,xb, sizeof(xb));
		memcpy(y,yb, sizeof(yb));
		n=nb;

		InitFoil();
		SetFlap();
		return true;
	}
}


/**
 * 	Reset the foil to a default Naca 009 geometry.
 */
void Foil::SetNaca009()
{	
	x[0]  = 1.00000    ; y[0]  = 0.00000;
	x[1]  = 0.99572    ; y[1]  = 0.00057;
	x[2]  = 0.98296    ; y[2]  = 0.00218;
	x[3]  = 0.96194    ; y[3]  = 0.00463;
	x[4]  = 0.93301    ; y[4]  = 0.00770;
	x[5]  = 0.89668    ; y[5]  = 0.01127;
	x[6]  = 0.85355    ; y[6]  = 0.01522;
	x[7]  = 0.80438    ; y[7]  = 0.01945;
	x[8]  = 0.75000    ; y[8]  = 0.02384;
	x[9]  = 0.69134    ; y[9]  = 0.02823;
	x[10] = 0.62941    ; y[10] = 0.03247;
	x[11] = 0.56526    ; y[11] = 0.03638;
	x[12] = 0.50000    ; y[12] = 0.03978;
	x[13] = 0.43474    ; y[13] = 0.04248;
	x[14] = 0.37059    ; y[14] = 0.04431;
	x[15] = 0.33928    ; y[15] = 0.04484;
	x[16] = 0.30866    ; y[16] = 0.04509;
	x[17] = 0.27886    ; y[17] = 0.04504;
	x[18] = 0.25000    ; y[18] = 0.04466;
	x[19] = 0.22221    ; y[19] = 0.04397;
	x[20] = 0.19562    ; y[20] = 0.04295;
	x[21] = 0.17033    ; y[21] = 0.04161;
	x[22] = 0.14645    ; y[22] = 0.03994;
	x[23] = 0.12408    ; y[23] = 0.03795;
	x[24] = 0.10332    ; y[24] = 0.03564;
	x[25] = 0.08427    ; y[25] = 0.03305;
	x[26] = 0.06699    ; y[26] = 0.03023;
	x[27] = 0.05156    ; y[27] = 0.02720;
	x[28] = 0.03806    ; y[28] = 0.02395;
	x[29] = 0.02653    ; y[29] = 0.02039;
	x[30] = 0.01704    ; y[30] = 0.01646;
	x[31] = 0.00961    ; y[31] = 0.01214;
	x[32] = 0.00428    ; y[32] = 0.00767;
	x[33] = 0.00107    ; y[33] = 0.00349;
	x[34] = 0.00000    ; y[34] = 0.00000;
	for (int i=0; i<34; i++){
		x[i+35] =  x[33-i];
		y[i+35] = -y[33-i];
	}
	n = 69;
	nb = 69;
	memcpy(xb,x, sizeof(x));
	InitFoil();
}


/**
* Sets the properties for the trailing edge flap.
* @param bFlap true if a flap is applied to the trailing edge
* @param xhinge the relative x-position of the flap's hinge
* @param yhinge the relative y-position of the flap's hinge
* @param angle the flap angle in degrees
*/
void  Foil::SetTEFlapData(bool bFlap, double xhinge, double yhinge, double angle)
{
	m_bTEFlap     = bFlap;
	m_TEXHinge    = xhinge;
	m_TEYHinge    = yhinge;
	m_TEFlapAngle = angle;
}


/**
* Sets the properties for the leading edge flap.
* @param bFlap true if a flap is applied to the leading edge
* @param xhinge the relative x-position of the flap's hinge
* @param yhinge the relative y-position of the flap's hinge
* @param angle the flap angle in degrees
*/
void  Foil::SetLEFlapData(bool bFlap, double xhinge, double yhinge, double angle)
{
	m_bLEFlap     = bFlap;
	m_LEXHinge    = xhinge;
	m_LEYHinge    = yhinge;
	m_LEFlapAngle = angle;
}

/**
 * Modifies the geometry of the current foil by setting the leading edge flap.
 * The specification for the flap is assumed to have been set previously
 */
void Foil::SetLEFlap()
{
	int i, j, k, l, p, i1, i2;
	i=j=k=l=p=i1=i2=0;
	double xh, yh, dx, dy;
	CVector M;
	bool bIntersect;
	double cosa, sina;

	cosa = cos(m_LEFlapAngle*PI/180.0);
	sina = sin(m_LEFlapAngle*PI/180.0);
	//first convert xhinge and yhinge in absolute coordinates
	xh = m_LEXHinge/100.0;
	double ymin = GetBaseLowerY(xh);
	double ymax = GetBaseUpperY(xh);
	yh = ymin + m_LEYHinge/100.0 * (ymax-ymin);

	// insert a breakpoint at xhinge location, if there isn't one already
	int iUpperh = 0;
	int iLowerh = 0;
	for (i=0; i<m_iExt; i++)
	{
		if(qAbs(m_rpExtrados[i].x-xh)<0.001)
		{
			//then no need to add an extra point, just break
			iUpperh = i;
			break;
		}
		else if(m_rpExtrados[i].x>xh)
		{
			//insert one
			for(j=m_iExt+1; j>i; j--)
			{
				m_rpExtrados[j].x = m_rpExtrados[j-1].x;
				m_rpExtrados[j].y = m_rpExtrados[j-1].y;
			}

			m_rpExtrados[i].x = xh;
			m_rpExtrados[i].y = ymax;
			iUpperh = i;
			m_iExt+=1;
			break;
		}
	}

	for (i=0; i<m_iInt; i++)
	{
		if(qAbs(m_rpIntrados[i].x-xh)<0.001)
		{
			//then no need to add an Intra point, just break
			iLowerh = i;
			break;
		}
		else if(m_rpIntrados[i].x>xh)
		{//insert one
			for(j=m_iInt+1; j>i; j--)
			{
				m_rpIntrados[j].x = m_rpIntrados[j-1].x;
				m_rpIntrados[j].y = m_rpIntrados[j-1].y;
			}

			m_rpIntrados[i].x = xh;
			m_rpIntrados[i].y = ymin;
			iLowerh = i;
			m_iInt+=1;
			break;
		}
	}

	// rotate all points upstream of xh
	if(m_LEFlapAngle>0.0)
	{
		//insert an extra point on intrados
		for (i=m_iInt+1; i>iLowerh; i--)
		{
			m_rpIntrados[i] = m_rpIntrados[i-1];
		}
		m_rpIntrados[iLowerh] = m_rpIntrados[iLowerh+1];
		iLowerh++;
		m_iInt++;

		// extend to infinity last segments around hinge on flap internal side to make sure
		// they intersect the spline on the other side
		m_rpIntrados[iLowerh-1].x += 30.0*(m_rpIntrados[iLowerh-1].x-m_rpIntrados[iLowerh-2].x);
		m_rpIntrados[iLowerh-1].y += 30.0*(m_rpIntrados[iLowerh-1].y-m_rpIntrados[iLowerh-2].y);
		m_rpIntrados[iLowerh].x   += 30.0*(m_rpIntrados[iLowerh].x   - m_rpIntrados[iLowerh+1].x);
		m_rpIntrados[iLowerh].y   += 30.0*(m_rpIntrados[iLowerh].y   - m_rpIntrados[iLowerh+1].y);
	}
	if(m_LEFlapAngle<0.0)
	{
		//insert an extra point on extrados
		for (i=m_iExt+1; i>iUpperh; i--)
		{
			m_rpExtrados[i] = m_rpExtrados[i-1];
		}
		m_rpExtrados[iUpperh] = m_rpExtrados[iUpperh+1];
		iUpperh++;
		m_iExt++;

		// extend to infinity last segments around hinge on flap internal side to make sure
		// they intersect the spline on the other side
		m_rpExtrados[iUpperh-1].x += 30.0 * (m_rpExtrados[iUpperh-1].x - m_rpExtrados[iUpperh-2].x);
		m_rpExtrados[iUpperh-1].y += 30.0 * (m_rpExtrados[iUpperh-1].y - m_rpExtrados[iUpperh-2].y);
		m_rpExtrados[iUpperh].x   += 30.0 * (m_rpExtrados[iUpperh].x   - m_rpExtrados[iUpperh+1].x);
		m_rpExtrados[iUpperh].y   += 30.0 * (m_rpExtrados[iUpperh].y   - m_rpExtrados[iUpperh+1].y);
	}
	for (i=0; i<iUpperh; i++)
	{
		dx = m_rpExtrados[i].x-xh;
		dy = m_rpExtrados[i].y-yh;
		m_rpExtrados[i].x = xh + cosa * dx - sina * dy;
		m_rpExtrados[i].y = yh + sina * dx + cosa * dy;
	}
	for (i=0; i<iLowerh; i++)
	{
		dx = m_rpIntrados[i].x-xh;
		dy = m_rpIntrados[i].y-yh;
		m_rpIntrados[i].x = xh + cosa * dx - sina * dy;
		m_rpIntrados[i].y = yh + sina * dx + cosa * dy;
	}

	Spline LinkSpline;
	LinkSpline.m_iRes = 4;
	LinkSpline.m_iDegree = 2;
	LinkSpline.m_CtrlPoint.clear();

	if(m_LEFlapAngle<0.0)
	{

		//define a 3 ctrl-pt spline to smooth the connection between foil and flap on bottom side
		Intersect(m_rpIntrados[iLowerh-2], m_rpIntrados[iLowerh-1],
				  m_rpIntrados[iLowerh],   m_rpIntrados[iLowerh+1], &M);
		//sanity check
		if(M.x <= m_rpIntrados[iLowerh-1].x || M.x >= m_rpIntrados[iLowerh].x)
			M = (m_rpIntrados[iLowerh-1] + m_rpIntrados[iLowerh])/2.0;

		LinkSpline.InsertPoint(m_rpIntrados[iLowerh-1].x,m_rpIntrados[iLowerh-1].y);
		LinkSpline.InsertPoint(M.x, M.y);
		LinkSpline.InsertPoint(m_rpIntrados[iLowerh].x,m_rpIntrados[iLowerh].y);
		LinkSpline.SplineKnots();
		LinkSpline.SplineCurve();
		//retrieve point 1 and 2 and insert them
		for (i=m_iInt; i>=iLowerh; i--)
		{
			m_rpIntrados[i+2].x = m_rpIntrados[i].x;
			m_rpIntrados[i+2].y = m_rpIntrados[i].y;
		}

		m_rpIntrados[iLowerh+1].x = LinkSpline.m_Output[2].x;
		m_rpIntrados[iLowerh+1].y = LinkSpline.m_Output[2].y;
		m_rpIntrados[iLowerh].x   = LinkSpline.m_Output[1].x;
		m_rpIntrados[iLowerh].y   = LinkSpline.m_Output[1].y;

		m_iInt+=2;
	}
	if(m_LEFlapAngle>0.0)
	{

		//define a 3 ctrl-pt spline to smooth the connection between foil and flap on bottom side
		Intersect(m_rpExtrados[iUpperh-2], m_rpExtrados[iUpperh-1],
				  m_rpExtrados[iUpperh],   m_rpExtrados[iUpperh+1], &M);

		//sanity check
		if(M.x <= m_rpExtrados[iUpperh-1].x || M.x >= m_rpExtrados[iUpperh].x)
			M = (m_rpExtrados[iUpperh-1] + m_rpExtrados[iUpperh])/2.0;

		LinkSpline.InsertPoint(m_rpExtrados[iUpperh-1].x,m_rpExtrados[iUpperh-1].y);
		LinkSpline.InsertPoint(M.x, M.y);
		LinkSpline.InsertPoint(m_rpExtrados[iUpperh].x,m_rpExtrados[iUpperh].y);
		LinkSpline.SplineKnots();
		LinkSpline.SplineCurve();
		//retrieve point 1 and 2 and insert them
		for (i=m_iExt; i>=iUpperh; i--)
		{
			m_rpExtrados[i+2].x = m_rpExtrados[i].x;
			m_rpExtrados[i+2].y = m_rpExtrados[i].y;
		}

		m_rpExtrados[iUpperh+1].x = LinkSpline.m_Output[2].x;
		m_rpExtrados[iUpperh+1].y = LinkSpline.m_Output[2].y;
		m_rpExtrados[iUpperh].x   = LinkSpline.m_Output[1].x;
		m_rpExtrados[iUpperh].y   = LinkSpline.m_Output[1].y;

		m_iExt+=2;
	}
	// trim upper surface first
	i1 = iUpperh;
	i2 = iUpperh-1;
	p=0;
	bIntersect = false;
	for (j=i2-1; j>0; j--)
	{
		for (k=i1;k<m_iExt; k++)
		{
			if(Intersect(m_rpExtrados[j], m_rpExtrados[j+1],
						 m_rpExtrados[k], m_rpExtrados[k+1], &M))
			{
				bIntersect = true;
				break;
			}
		}
		if(bIntersect) break;
	}

	if(bIntersect)
	{
		m_rpExtrados[j+1].x = M.x;
		m_rpExtrados[j+1].y = M.y;
		p=1;
		for (l=k+1;l<=m_iExt; l++){
			m_rpExtrados[j+1+p]  = m_rpExtrados[l];
			p++;
		}
		m_iExt = j+p;
	}

	// trim lower surface next
	i1 = iLowerh;
	i2 = iLowerh-1;
	p=0;
	bIntersect = false;
	for (j=i2-1; j>0; j--)
	{
		for (k=i1;k<m_iInt; k++)
		{
			if(Intersect(m_rpIntrados[j], m_rpIntrados[j+1],
						 m_rpIntrados[k], m_rpIntrados[k+1], &M))
			{
				bIntersect = true;
				break;
			}
		}
		if(bIntersect) break;
	}

	if(bIntersect)
	{
		m_rpIntrados[j+1].x = M.x;
		m_rpIntrados[j+1].y = M.y;
		p=1;
		for (l=k+1;l<=m_iInt; l++)
		{
			m_rpIntrados[j+1+p]  = m_rpIntrados[l];
			p++;
		}
		m_iInt = j+p;
	}
}


/**
 * Modifies the geometry of the current foil by setting the trailing edge flap.
 * The specification for the flap is assumed to have been set previously
 */
void Foil::SetTEFlap()
{
	int i, j, k, l, p, i1, i2;
	double xh, yh, dx, dy;
	CVector M;
	bool bIntersect;
	double cosa, sina;

	cosa = cos(m_TEFlapAngle*PI/180.0);
	sina = sin(m_TEFlapAngle*PI/180.0);
	//first convert xhinge and yhinge in absolute coordinates
	xh = m_TEXHinge/100.0;
	double ymin = GetBaseLowerY(xh);
	double ymax = GetBaseUpperY(xh);
	yh = ymin + m_TEYHinge/100.0 * (ymax-ymin);
	// insert a breakpoint at xhinge location, if there isn't one already
	int iUpperh = 0;
	int iLowerh = 0;
	for (i=0; i<m_iExt; i++)
	{
					if(qAbs(m_rpExtrados[i].x-xh)<0.001)
		{
			//then no need to add an extra point, just break
			iUpperh = i;
			break;
		}
		else if(m_rpExtrados[i].x>xh)
		{
			for(j=m_iExt+1; j>i; j--)
			{
				m_rpExtrados[j].x = m_rpExtrados[j-1].x;
				m_rpExtrados[j].y = m_rpExtrados[j-1].y;
			}
			m_rpExtrados[i].x = xh;
			m_rpExtrados[i].y = ymax;
			iUpperh = i;
			m_iExt++;
			break;
		}
	}

	for (i=0; i<m_iInt; i++)
	{
					if(qAbs(m_rpIntrados[i].x-xh)<0.001)
		{
			//then no need to add an Intra point, just break
			iLowerh = i;
			break;
		}
		else if(m_rpIntrados[i].x>xh)
		{
			for(j=m_iInt+1; j>i; j--)
			{
				m_rpIntrados[j].x = m_rpIntrados[j-1].x;
				m_rpIntrados[j].y = m_rpIntrados[j-1].y;
			}
			m_rpIntrados[i].x = xh;
			m_rpIntrados[i].y = ymin;
			iLowerh = i;
			m_iInt++;
			break;
		}
	}

	// rotate all points downstream of xh
	if(m_TEFlapAngle>0.0)
	{
		//insert an extra point on intrados
		for (i=m_iInt+1; i>iLowerh; i--)
		{
			m_rpIntrados[i] = m_rpIntrados[i-1];
		}
		m_iInt++;
		// extend to infinity last segments around hinge on flap internal side to make sure
		// they intersect the spline on the other side
		m_rpIntrados[iLowerh+1].x += 30.0*(m_rpIntrados[iLowerh+1].x - m_rpIntrados[iLowerh+2].x);
		m_rpIntrados[iLowerh+1].y += 30.0*(m_rpIntrados[iLowerh+1].y - m_rpIntrados[iLowerh+2].y);
		m_rpIntrados[iLowerh].x   += 30.0*(m_rpIntrados[iLowerh].x   - m_rpIntrados[iLowerh-1].x);
		m_rpIntrados[iLowerh].y   += 30.0*(m_rpIntrados[iLowerh].y   - m_rpIntrados[iLowerh-1].y);
	}
	if(m_TEFlapAngle<0.0)
	{
		//insert an extra point on extrados
		for (i=m_iExt+1; i>iUpperh; i--)
		{
			m_rpExtrados[i] = m_rpExtrados[i-1];
		}
		m_iExt++;

		// extend to infinity last segments around hinge on flap internal side to make sure
		// they intersect the spline on the other side
		m_rpExtrados[iUpperh+1].x += 30.0*(m_rpExtrados[iUpperh+1].x-m_rpExtrados[iUpperh+2].x);
		m_rpExtrados[iUpperh+1].y += 30.0*(m_rpExtrados[iUpperh+1].y-m_rpExtrados[iUpperh+2].y);
		m_rpExtrados[iUpperh].x   += 30.0 * (m_rpExtrados[iUpperh].x-m_rpExtrados[iUpperh-1].x);
		m_rpExtrados[iUpperh].y   += 30.0 * (m_rpExtrados[iUpperh].y-m_rpExtrados[iUpperh-1].y);
	}
	for (i=iUpperh+1; i<=m_iExt; i++)
	{
		dx = m_rpExtrados[i].x-xh;
		dy = m_rpExtrados[i].y-yh;
		m_rpExtrados[i].x = xh + cosa * dx + sina * dy;
		m_rpExtrados[i].y = yh - sina * dx + cosa * dy;
	}
	for (i=iLowerh+1; i<=m_iInt; i++)
	{
		dx = m_rpIntrados[i].x-xh;
		dy = m_rpIntrados[i].y-yh;
		m_rpIntrados[i].x = xh + cosa * dx + sina * dy;
		m_rpIntrados[i].y = yh - sina * dx + cosa * dy;
	}

	Spline LinkSpline;
	LinkSpline.m_iRes = 4;
	LinkSpline.m_iDegree = 2;
	LinkSpline.m_CtrlPoint.clear();

	if(m_TEFlapAngle<0.0)
	{
		//define a 3 ctrl-pt spline to smooth the connection between foil and flap on bottom side
		Intersect(m_rpIntrados[iLowerh-1], m_rpIntrados[iLowerh],
				  m_rpIntrados[iLowerh+1], m_rpIntrados[iLowerh+2], &M);

		//sanity check
		if(M.x <= m_rpIntrados[iLowerh].x || M.x >= m_rpIntrados[iLowerh+1].x)
			M = (m_rpIntrados[iLowerh] + m_rpIntrados[iLowerh+1])/2.0;

		LinkSpline.InsertPoint(m_rpIntrados[iLowerh].x,m_rpIntrados[iLowerh].y);
		LinkSpline.InsertPoint(M.x, M.y);
		LinkSpline.InsertPoint(m_rpIntrados[iLowerh+1].x,m_rpIntrados[iLowerh+1].y);
		LinkSpline.SplineKnots();
		LinkSpline.SplineCurve();
		//retrieve point 1 and 2 and insert them
		for (i=m_iInt; i>=iLowerh+1; i--)
		{
			m_rpIntrados[i+2].x = m_rpIntrados[i].x;
			m_rpIntrados[i+2].y = m_rpIntrados[i].y;
		}

		m_rpIntrados[iLowerh+2].x = LinkSpline.m_Output[2].x;
		m_rpIntrados[iLowerh+2].y = LinkSpline.m_Output[2].y;
		m_rpIntrados[iLowerh+1].x = LinkSpline.m_Output[1].x;
		m_rpIntrados[iLowerh+1].y = LinkSpline.m_Output[1].y;

		m_iInt+=2;
	}
	else if(m_TEFlapAngle>0.0)
	{
		//define a 3 ctrl-pt spline to smooth the connection between foil and flap on top side
		Intersect(m_rpExtrados[iUpperh-1], m_rpExtrados[iUpperh],
				  m_rpExtrados[iUpperh+1], m_rpExtrados[iUpperh+2], &M);

		//sanity check
		if(M.x <= m_rpExtrados[iUpperh].x || M.x >= m_rpExtrados[iUpperh+1].x)
			M = (m_rpExtrados[iUpperh] + m_rpExtrados[iUpperh+1])/2.0;

		LinkSpline.InsertPoint(m_rpExtrados[iUpperh].x,m_rpExtrados[iUpperh].y);
		LinkSpline.InsertPoint(M.x, M.y);
		LinkSpline.InsertPoint(m_rpExtrados[iUpperh+1].x,m_rpExtrados[iUpperh+1].y);
		LinkSpline.SplineKnots();
		LinkSpline.SplineCurve();

		//retrieve point 1 and 2 and insert them
		for (i=m_iExt; i>=iUpperh+1; i--)
		{
			m_rpExtrados[i+2].x = m_rpExtrados[i].x;
			m_rpExtrados[i+2].y = m_rpExtrados[i].y;
		}

		m_rpExtrados[iUpperh+2].x = LinkSpline.m_Output[2].x;
		m_rpExtrados[iUpperh+2].y = LinkSpline.m_Output[2].y;
		m_rpExtrados[iUpperh+1].x = LinkSpline.m_Output[1].x;
		m_rpExtrados[iUpperh+1].y = LinkSpline.m_Output[1].y;

		m_iExt+=2;
	}

	// trim upper surface first

	i1 = iUpperh;
	i2 = iUpperh+1;
	p=0;
	bIntersect = false;

	k=0;
	for (j=i2; j<m_iExt; j++)
	{
		for (k=i1;k>0; k--)
		{
			if(Intersect(m_rpExtrados[j], m_rpExtrados[j+1], m_rpExtrados[k], m_rpExtrados[k-1], &M))
			{
					bIntersect = true;
					break;
			}
		}
		if(bIntersect) break;
	}

	if(bIntersect)
	{
		m_rpExtrados[k] = M;
		p=1;
		for (l=j+1;l<=m_iExt; l++)
		{
			m_rpExtrados[k+p]  = m_rpExtrados[l];
			p++;
		}
		m_iExt = k+p-1;
	}
	// trim lower surface next

	i1 = iLowerh;
	i2 = iLowerh+1;
	p=0;
	bIntersect = false;
	for (j=i2; j<m_iInt; j++)
	{
		for (k=i1;k>0; k--)
		{
			if(Intersect(m_rpIntrados[j], m_rpIntrados[j+1], m_rpIntrados[k], m_rpIntrados[k-1], &M))
			{
					bIntersect = true;
					break;
			}
		}
		if(bIntersect) break;
	}

	if(bIntersect)
	{
		m_rpIntrados[k] = M;
		p=1;
		for (l=j+1;l<=m_iInt; l++)
		{
			m_rpIntrados[k+p]  = m_rpIntrados[l];
			p++;
		}
		m_iInt = k+p-1;
	}
}

/**
 * Creates the leading and trailing edge flaps on the current geometry.
 */
void Foil::SetFlap()
{
	int i;
	// modifies the current airfoil's geometry 
	// by setting a flap i.a.w. the member variables

	//copy the base foil to the current foil
	memcpy(m_rpExtrados, m_BaseExtrados, sizeof(m_rpExtrados));
	memcpy(m_rpIntrados, m_BaseIntrados, sizeof(m_rpIntrados));

	m_iExt = m_iBaseExt;
	m_iInt = m_iBaseInt;


	if(m_bLEFlap) SetLEFlap();
	if(m_bTEFlap) SetTEFlap();

	//And finally rebuild the current foil
	for (i=m_iExt; i>=0; i--)
	{
		x[m_iExt-i] = m_rpExtrados[i].x;
		y[m_iExt-i] = m_rpExtrados[i].y;
	}
	for (i=1; i<=m_iInt; i++)
	{
		x[m_iExt+i] = m_rpIntrados[i].x;
		y[m_iExt+i] = m_rpIntrados[i].y;
	}
	
	n = m_iExt + m_iInt + 1;

	if(m_bTEFlap)
	{
		//rotate the mid line
		int im = 0;
		//restore the id line from the base flap
		memcpy(m_rpMid, m_rpBaseMid, sizeof(m_rpMid));

		//convert xhinge and yhinge in absolute coordinates
		double xh = m_TEXHinge/100.0;
		double ymin = GetBaseLowerY(xh);
		double ymax = GetBaseUpperY(xh);
		double yh = ymin + m_TEYHinge/100.0 * (ymax-ymin);

		CVector hinge(xh, yh, 0.0);

		while(im<=MIDPOINTCOUNT)
		{
			if(m_rpMid[im].x>=hinge.x)
			{
				// rotate around XHinge
				m_rpMid[im].RotateZ(hinge, -m_TEFlapAngle);
			}
			im++;
		}
	}

//	InitFoil();//normals are set in InitXFoil() at calculation time
}




/**
 * Returns a pointer to the foil with the corresponding nam or NULL if not found.
 * @param strFoilName the name of the Foil to search for in the array
 * @return a pointer to the foil with the corresponding nam or NULL if not found.
 */
Foil* Foil::foil(QString strFoilName)
{
	if(!strFoilName.length()) return NULL;
	Foil* pFoil;
	for (int i=0; i<s_oaFoil.size(); i++)
	{
		pFoil = (Foil*)s_oaFoil.at(i);
		if (pFoil->m_FoilName == strFoilName)
		{
			return pFoil;
		}
	}

	return NULL;
}



/**
 * Deletes the Foil object removes it from the array, and returns a pointer to the next Foil in the array
 * @param pFoil a pointer to the Foil to delete
 * @return a pointer to the next or previous Foil in the array, or NULL if none
 */
Foil * Foil::deleteFoil(Foil *pFoil)
{
	if(!pFoil || !pFoil->m_FoilName.length()) return NULL;
	Foil *pOldFoil;
	OpPoint * pOpPoint;
	Polar* pPolar;
	int j;


	for (j=OpPoint::s_oaOpp.size()-1; j>=0; j--)
	{
		pOpPoint = (OpPoint*)OpPoint::s_oaOpp[j];
		if(pOpPoint->foilName() == pFoil->foilName())
		{
			if(pOpPoint==OpPoint::curOpp()) OpPoint::setCurOpp(NULL);
			OpPoint::s_oaOpp.removeAt(j);
			delete pOpPoint;
		}
	}


	for (j=Polar::s_oaPolar.size()-1; j>=0; j--)
	{
		pPolar = (Polar*)Polar::s_oaPolar.at(j);
		if(pPolar->foilName() == pFoil->foilName())
		{
			if(pPolar==Polar::curPolar()) Polar::setCurPolar(NULL);
			Polar::s_oaPolar.removeAt(j);
			delete pPolar;
		}
	}


	Foil *pNewCurFoil= NULL;

	for (j=0; j<s_oaFoil.size(); j++)
	{
		pOldFoil = (Foil*)s_oaFoil.at(j);
		if (pOldFoil == pFoil)
		{
			if(j<s_oaFoil.count()-1) pNewCurFoil = (Foil*)s_oaFoil.at(j+1);
			else if(j>0)             pNewCurFoil = (Foil*)s_oaFoil.at(j-1);
			else                     pNewCurFoil = NULL;

			s_oaFoil.removeAt(j);
			delete pOldFoil;
			break;
		}
	}
	return pNewCurFoil;
}



/**
 * The Foil does not exist yet in the array.
 * Delete any former Foil with the same name, including its children objects.
 * Insert the Foil in sorted order in the array.
 * Questions have been answered previously : which-name, overwrite-or-not-overwrite, etc. Just do it.
*/
void Foil::insertThisFoil()
{
	Foil*pOldFoil;
	QString oldFoilName = m_FoilName;

	//check that this Foil does not exist in the array
	for(int iFoil=0; iFoil<s_oaFoil.count(); iFoil++)
	{
		if(this == s_oaFoil.at(iFoil))
		{
			Trace("This foil "+m_FoilName+" aready exists and has not been inserted");
			return;
		}
	}

	//check if it's an overwrite
	for(int iFoil=0; iFoil<s_oaFoil.size(); iFoil++)
	{
		pOldFoil = (Foil*)s_oaFoil.at(iFoil);
		if(pOldFoil->foilName()==oldFoilName && pOldFoil!=this)
		{
			//copy the old foil's style
			m_FoilColor = pOldFoil->m_FoilColor;
			m_FoilStyle = pOldFoil->m_FoilStyle;
			m_FoilWidth = pOldFoil->m_FoilWidth;

			//we overwrite the old foil and delete its children objects
			deleteFoil(pOldFoil);
			//continue loop, shouldn't find any other Foil with the same name, but cleans up former errors eventually
		}
	}

	// no existing former foil with the same name, straightforward insert
	for(int iFoil=0; iFoil<s_oaFoil.size(); iFoil++)
	{
		pOldFoil = (Foil*)s_oaFoil.at(iFoil);
		if(m_FoilName.compare(pOldFoil->m_FoilName, Qt::CaseInsensitive)<0)
		{
			//then insert before
			s_oaFoil.insert(iFoil, this);
			return;
		}
	}

	//not inserted, append
	s_oaFoil.append(this);
}



/**
 * The foil exists in the array.
 * Rename it with the specified new name, and rename its children Polar and OpPoint objects
 * Overwrites any former foil with the same name.
 * Questions have been answered previously : which-name, overwrite-or-not-overwrite, etc. Just do it.
 * @param newFoilName
 */
void Foil::renameThisFoil(QString newFoilName)
{
	Foil *pOldFoil=NULL;
	Polar* pOldPolar=NULL;
	OpPoint *pOpPoint=NULL;
	QString oldFoilName = m_FoilName;

	//check that this Foil exists in the array
	bool bFound = false;
	for(int iFoil=0; iFoil<s_oaFoil.count(); iFoil++)
	{
		if(this == s_oaFoil.at(iFoil))
		{
			bFound = true;
			break;
		}
	}
	if(!bFound)
	{
		Trace("The foil "+m_FoilName+" could not be found in the array and has not been renamed");
		return;
	}

	//rename it
	this->m_FoilName = newFoilName;

	//delete any former Foil with the new name
	for(int iFoil=0; iFoil<s_oaFoil.count(); iFoil++)
	{
		pOldFoil = (Foil*)s_oaFoil.at(iFoil);
		if(pOldFoil->foilName() == oldFoilName)
		{
			pOldFoil->deleteThisFoil();
			//continue loop to purge old duplicates, who knows.
		}
	}
	
	//ready to rename

	//rename its children objects
	for (int iPolar=0; iPolar<Polar::s_oaPolar.size(); iPolar++)
	{
		pOldPolar = (Polar*)Polar::s_oaPolar.at(iPolar);
		if(pOldPolar->foilName() == oldFoilName)
		{
			pOldPolar->setFoilName(newFoilName);
		}
	}

	for (int iOpp=0; iOpp<OpPoint::s_oaOpp.size(); iOpp++)
	{
		pOpPoint = (OpPoint*)OpPoint::s_oaOpp.at(iOpp);
		if(pOpPoint->foilName() == oldFoilName)
		{
			pOpPoint->setFoilName(newFoilName);
		}
	}


	//remove the Foil from its current position in the array
	for(int iFoil=0; iFoil<s_oaFoil.size(); iFoil++)
	{
		if(this == (Foil*)s_oaFoil.at(iFoil))
		{
			s_oaFoil.removeAt(iFoil);
			break;
		}
	}


	//re-insert and we're done
	for(int iFoil=0; iFoil<s_oaFoil.size(); iFoil++)
	{
		pOldFoil = (Foil*)s_oaFoil.at(iFoil);
		if(m_FoilName.compare(pOldFoil->m_FoilName, Qt::CaseInsensitive)<0)
		{
			//then insert before
			s_oaFoil.insert(iFoil, this);
			return;
		}
	}

	//Not inserted, append
	s_oaFoil.append(this);
}



/**
 * Deletes the Foil and its children Polar and OpPoint objects
 * @return a pointer to the next or previous Foil in the array
 */
Foil * Foil::deleteThisFoil()
{
	Foil *pOldFoil=NULL;
	Polar* pOldPolar=NULL;
	OpPoint *pOpPoint=NULL;

	//delete any OpPoints with this FoilName
	for (int jOpp=OpPoint::s_oaOpp.size()-1; jOpp>=0; jOpp--)
	{
		pOpPoint = (OpPoint*)OpPoint::s_oaOpp[jOpp];
		if(pOpPoint->foilName() == this->foilName())
		{
			if(pOpPoint==OpPoint::curOpp()) OpPoint::setCurOpp(NULL);
			OpPoint::s_oaOpp.removeAt(jOpp);
			delete pOpPoint;
		}
	}


	//delete any Polars  with this FoilName
	for (int jPlr=Polar::s_oaPolar.size()-1; jPlr>=0; jPlr--)
	{
		pOldPolar = (Polar*)Polar::s_oaPolar.at(jPlr);
		if(pOldPolar->foilName() == this->foilName())
		{
			if(pOldPolar==Polar::curPolar()) Polar::setCurPolar(NULL);
			Polar::s_oaPolar.removeAt(jPlr);
			delete pOldPolar;
		}
	}


	//delete the Foil
	Foil *pNewCurFoil= NULL;

	for (int jFoil=0; jFoil<s_oaFoil.size(); jFoil++)
	{
		pOldFoil = (Foil*)s_oaFoil.at(jFoil);
		if (pOldFoil == this)
		{
			if(jFoil<s_oaFoil.count()-1) pNewCurFoil = (Foil*)s_oaFoil.at(jFoil+1);
			else if(jFoil>0)             pNewCurFoil = (Foil*)s_oaFoil.at(jFoil-1);
			else                         pNewCurFoil = NULL;

			s_oaFoil.removeAt(jFoil);
			delete pOldFoil;
			break;
		}
	}
	return pNewCurFoil;
}






