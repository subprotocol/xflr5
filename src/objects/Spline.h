/****************************************************************************

    Spline  Class
	Copyright (C) 1996 Paul Bourke	http://astronomy.swin.edu.au/~pbourke/curves/spline/
	Copyright (C) 2003 Andre Deperrois adeperrois@xflr5.com

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



/**
 *@file This file implements the spline class used in foil direct and inverse design.
 */


#ifndef SPLINE_H
#define SPLINE_H


#include <QPainter>
#include <QFile>
#include <QList>
#include <QTextStream>
#include "../objects/CVector.h"
#include "../params.h"


/**
*@class Spline
*@brief  The class which defines the 2D spline object.

The spline is used in direct foil design to represent upper and lower surfaces, and in XInverse foil design to define the specification curve.

Based on the code provided by Paul Bourke.
*/
class Spline
{
//	friend class SplineFoil;
//	friend class QXInverse;
//	friend class InverseOptionsDlg;

public:
	Spline();

	void DrawSpline(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset);
	void DrawCtrlPoints(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset);
	void DrawOutputPoints(QPainter & painter, double const &scalex, double const &scaley, QPoint const &Offset);

	bool InsertPoint(double const &x, double const &y);
	bool RemovePoint(int const &k);
	int IsControlPoint(CVector const &Real);
	int IsControlPoint(double const &x, double const &y, double const &zx, double const &zy);
	int IsControlPoint(CVector const &Real, double const &ZoomFactor);
	double SplineBlend(int const &i, int const &p, double const &t);
	double GetY(double const &x);

	void Copy(Spline *pSpline);
	void CopySymetric(Spline *pSpline);
	void Export(QTextStream &out, bool bExtrados);
	void SplineCurve();
	void SplineKnots();
	
	void SetStyle(int style);
	void SetWidth(int width);
	void SetColor(QColor color);
	void SetSplineParams(int style, int width, QColor color);
	QColor color() {return m_Color;}
	int style() {return m_Style;}
	int width() {return m_Width;}

	int m_iHighlight;                /**< the index of the currently highlighted control point, i.e. the point over which the mouse hovers, or -1 of none. */
	int m_iSelect;                   /**< the index of the currently selected control point, i.e. the point on which the user has last click, or -1 if none. */
	int m_iRes;                      /**< the number of output points to draw the spline */
	int m_iDegree;                   /**< the spline's degree */

	double m_PtWeight;               /**< the common weight of all control points. Default is 1. The higher the value, the more the curve will be pulled towards the control points. */

	QList<double> m_knot;            /**< the array of the values of the spline's knot */
	QList<CVector> m_CtrlPoint;      /**< the array of the positions of the spline's control points */
	CVector m_Output[IQX2];          /**< the array of output points, size of which is m_iRes @todo use a QVarLengthArray or a QList*/

private:
	QColor m_Color;                  /**< the spline's display color */
	int m_Style;                     /**< the index of the spline's style */
	int m_Width;                     /**< the width of the spline */
};


#endif
