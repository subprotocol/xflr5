/****************************************************************************

	Curve Class
	Copyright (C) 2003-2009 Andre Deperrois adeperrois@xflr5.com

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

/*! @file
 *
 * This file defines the Curve object for the graphs
 *
 *
 */



#ifndef CURVE_H
#define CURVE_H

#include "../params.h"
#include "../objects/CVector.h"
#include <QVarLengthArray>
#include <QColor>

class Curve
{
	friend class Graph;

public:
	Curve();

	int     AppendPoint(double xn, double yn);
	void    clear();
	int     closestPoint(double xs, double ys, double &dist);
	void    closestPoint(double xs, double ys, double &dist, int &n);
	void    closestPoint(double const &xs, double const &ys, double &xSel, double &ySel, double &dist, int &nSel);
	QColor  color();
	void    CopyData(Curve *pCurve);
	int     count();
	void    Duplicate(Curve *pCurve);
	int     GetSelected();
	bool    IsVisible();
	CVector point(int ref);
	bool    PointsVisible();
	void    ShowPoints(bool bShow);
	void    SetVisible(bool bVisible);
	void    SetColor(QColor clr);
	void    SetStyle(int nStyle);
	void    SetSelected(int n);
	void    SetWidth(int nWidth);
	void    SetTitle(QString Title);
	/** Returns the curve's number of points. */
	int     size() {return x.count();}
	int     style();
	void    title(QString &string);
	int     width();
	QString title();
	double  xMin();
	double  xMax();
	double  yMin();
	double  yMax();


	//	Curve Data
	QVarLengthArray<double,  1024> x;          /**< the array of the points x-coordinates */
	QVarLengthArray<double,  1024> y;          /**< the array of the points y-coordinates */


private:	
	bool m_bIsVisible;        /**< true if the curve is visible */
	bool m_bShowPoints;       /**< true if the curve's points are visible */
	QColor CurveColor;        /**< the curve's display color */
	QString m_CurveName;      /**< the curves's name */
	int CurveStyle;           /**< the index of the curve's display style */
	int CurveWidth;           /**< the width of the curve's display */
	int m_iSelected;          /**< the index of the curve's currently selected point, or -1 if none is selected */
	void *m_pParentGraph;     /**< a pointer to the parent graph to which this curve belongs */
};


#endif
