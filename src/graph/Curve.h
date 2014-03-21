/****************************************************************************

	Curve Class
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

/** @file
 *
 * This file implements the Curve object for the graphs.
 *
 */



#ifndef CURVE_H
#define CURVE_H

#include "../params.h"
#include <QVarLengthArray>
#include <QColor>

/**
* @class Curve
* This class defines the curve object used by the Graph class.
*/
class Curve
{
	friend class Graph;
	friend class QGraph;

public:
	Curve();

	int     AppendPoint(double xn, double yn);


	/**
	 * Resets the content of the curve.
	 */
	void    clear()
	{
		x.clear();
		y.clear();
	}

	int     closestPoint(double xs, double ys, double &dist);
	void    closestPoint(double xs, double ys, double &dist, int &n);
	void    closestPoint(double const &xs, double const &ys, double &xSel, double &ySel, double &dist, int &nSel);
	void    CopyData(Curve *pCurve);
	void    Duplicate(Curve *pCurve);


	/**
	 * Return the index of the currently selected point
	 *@param the index of the currently selected point
	*/
	int selected(){return m_iSelected;}


	/**
	 * Sets the visibility of the points in the graphs
	 *@param bShow true if the points are to be displayed, false otherwise
	 */
	void ShowPoints(bool bShow){m_bShowPoints = bShow;}

	/**
	 * Sets the visibility of the curve in the graphs
	 *@param bVisible true if the curve is to be displayed, false otherwise
	 */
	void SetVisible(bool bVisible){m_bIsVisible = bVisible;}

	/**
	 * Sets the curve's color
	 * @param clr the new QColor value for the curve
	 */
	void SetColor(QColor clr){CurveColor = clr;}

	/**
	 * Sets the curve's style
	 * @param nStyle the index of the new curve's style
	 */
	void SetStyle(int nStyle){ CurveStyle = nStyle;}


	/**
	 * Sets the index of the currently selected point of this curve
	 * @param n the point to select
	 */
	void SetSelected(int n){	m_iSelected = n;}

	/**
	 *Sets the curve's width
	 *@param nWidth the new curve's width in pixels
	 **/
	void SetWidth(int nWidth){CurveWidth = nWidth;}

	/**
	 * Sets the curve title
	 *@param Title the new curve's title
	 */
	void SetTitle(QString Title){ m_CurveName = Title;}


	/** Return the visibility of the curve as a boolean. */
	bool IsVisible() {return m_bIsVisible;}

	/** Return the visibility of the points as a boolean. */
	bool PointsVisible() {return m_bShowPoints; }

	/** Returns the Curve's number of points. */
	int size() {return x.count();}

	/** Returns the Curve's number of points. */
	int count() {return x.size();}

	/** Returns the Curve style*/
	int style() {return CurveStyle;}

	/** Returns the Curve width*/
	int width() {return CurveWidth;}

	/** Returns the Curve color*/
	QColor  color() {return CurveColor;}

	/** Returns the Curve's title */
	void title(QString &string) {string =  m_CurveName;}

	/** Returns the Curve's title */
	QString title(){ return m_CurveName;}

	double  xMin();
	double  xMax();
	double  yMin();
	double  yMax();

public:
	//	Curve Data
	QVarLengthArray<double,  1024> x;          /**< the array of the points x-coordinates */
	QVarLengthArray<double,  1024> y;          /**< the array of the points y-coordinates */


private:	
	bool m_bIsVisible;                         /**< true if the curve is visible */
	bool m_bShowPoints;                        /**< true if the curve's points are visible */
	QColor CurveColor;                         /**< the curve's display color */
	QString m_CurveName;                       /**< the curves's name */
	int CurveStyle;                            /**< the index of the curve's display style */
	int CurveWidth;                            /**< the width of the curve's display */
	int m_iSelected;                           /**< the index of the curve's currently selected point, or -1 if none is selected */
	void *m_pParentGraph;                      /**< a pointer to the parent graph to which this curve belongs */
};


#endif
