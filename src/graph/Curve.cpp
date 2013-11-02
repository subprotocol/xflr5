/****************************************************************************

    Curve Class
	Copyright (C) 2003-2008 Andre Deperrois adeperrois@xflr5.com

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


#include "Curve.h"
#include "math.h"
#include "Graph.h"


/**
 * The public constructor
 */
CCurve::CCurve()
{
    CurveColor = QColor(255,0,0,127);
	memset(x,0,sizeof(x));
	memset(y,0,sizeof(y));
	n=0;
	m_CurveName = "";
	m_bIsVisible = true;
	m_bShowPoints = false;
	CurveWidth = 1;
	CurveStyle = Qt::SolidLine;
	m_iSelected = -1;
}

/**
 * Appends a point to the end of the data
 * @param xn: x-coordinate
 * @param yn: y-coordinate
 * @return the new number of points in the curve
 */
int CCurve::AppendPoint(double xn, double yn)
{
	x[n] = xn;
	y[n] = yn;
	n++;
	return n;
}

/**
 * Copies the data and settings from an existing curve
 * @param pCurve: a pointer to the input curve
 */
void CCurve::Copy(CCurve *pCurve)
{
	if(!pCurve) return;
	int i;
	n  = pCurve->n;
	for (i=0; i<n ;i++)
	{
		x[i] = pCurve->x[i];
		y[i] = pCurve->y[i];
	}
	CurveColor = pCurve->CurveColor;
	CurveStyle = pCurve->CurveStyle;
	CurveWidth = pCurve->CurveWidth;
	m_bIsVisible = pCurve->m_bIsVisible;
	m_bShowPoints = pCurve->m_bShowPoints;
	m_CurveName = pCurve->m_CurveName;
}


/**
 * returns the color of the curve
 * @return the curve's color
 */
QColor CCurve::color()
{
	return CurveColor;
}


/**
 * @deprecated Returns the equivalent black and white grey tone for the current curve.
 * Was formerly used for b&w printing
 * @param &color
 * @param &style
 * @param &width
 */
void CCurve::BWStyle(QColor &color, int &style, int &width)
{
	color = CurveColor;
	style = CurveStyle;
	width = CurveWidth;
//	GetBWColor(color, style, width);
}

/**
 * Returns the point referenced by the input parameter as a CVector object
 * @param ref the reference of the requested pointed
 * @return a CVector instance of the point, or (0, 0, 0) if the reference is out of bounds
 */
CVector CCurve::point(int ref)
{
	CVector r;
	if(ref<0 || ref>=n){
		r.x = 0.0;
		r.y = 0.0;
	}
	else{
		r.x = x[ref];
		r.y = y[ref];
	}
	return r;
}


/**
 * Returns the index of the curve's point closest to the input coordinates
 * @param xs x coordinate
 * @param ys y coordinate
 * @param &dist distance to the return point
 * @return the index of the closest point
 */
int CCurve::closestPoint(double xs, double ys, double &dist )
{
	Graph *pGraph = (Graph*)m_pParentGraph;
	static int ref;
	static double d2;
	ref = -1;
	dist = 1.e10;
	if (n<1) return -1;
	for(int i=0; i<n; i++)
	{
		d2 =   (xs-x[i])*(xs-x[i])/pGraph->GetXScale()/pGraph->GetXScale() 
			 + (ys-y[i])*(ys-y[i])/pGraph->GetYScale()/pGraph->GetYScale();
		if (d2<dist)
		{
			dist = d2;
			ref = i;
		}
	}
	return ref;
}



/**
 * Returns the index of the curve's point closest to the input coordinates
 * @overload overloaded function
 * @param xs x coordinate
 * @param ys y coordinate
 * @param &dist distance to the return point
 * @param &n the index of the closest point
 */
void CCurve::closestPoint(double xs, double ys, double &dist, int &n)
{
	Graph *pGraph = (Graph*)m_pParentGraph;
	static double d2;
	dist = 1.e10;
	if (n<1) return;
	for(int i=0; i<n; i++)
	{
		d2 =   (xs-x[i])*(xs-x[i])/pGraph->GetXScale()/pGraph->GetXScale()
			 + (ys-y[i])*(ys-y[i])/pGraph->GetYScale()/pGraph->GetYScale();
		if (d2<dist)
		{
			dist = d2;
			n = i;
		}
	}
}


/**
 * Returns the index and the coordinates of the curve's point closest to the input coordinates
 * @param xs x input coordinate
 * @param ys y input coordinate
 * @param xSel x output coordinate
 * @param ySel y output coordinate
 * @param &dist distance to the return point
 * @param &nSel the index of the closest point
 */
void CCurve::closestPoint(double const &xs, double const &ys, double &xSel, double &ySel, double &dist, int &nSel)
{
	static double d2;
	dist = 1.e40;

	for(int i=0; i<n; i++)
	{
		d2 =   (xs-x[i])*(xs-x[i]) + (ys-y[i])*(ys-y[i]);
		if (d2<dist)
		{
			dist = d2;
			xSel = x[i];
			ySel = y[i];
			nSel = i;
		}
	}
}


/**
 * Returns the number of point in this curve
 * @return the number of point in this curve
 */
int CCurve::count()
{
	return n;
}

/**
 * Returns the curve style
 * @return the index of the curve's style
 */
int CCurve::style()
{
	return CurveStyle;
}


/**
 * Returns the curve's title
 * @param &string a reference to the string to be filled with the curve's title
 */
void CCurve::title(QString &string)
{
	string =  m_CurveName;
}


/**
 * Returns the curve's title
 * @return a QString instance of curve's title
 *@overload
 */
QString CCurve::title()
{
	return m_CurveName;
}


/**
 * Returns the curve's width in pixels
 * @return the curve's width as an int value
 */
int CCurve::width()
{
	return CurveWidth;
}


/**
 * Returns the minimum x value of this curve
 *@return the x min value, or +99999999.0 if the curve has no points
 */
double CCurve::xMin()
{
	double xMin = 99999999.0;
//	if(n==0) xmin = .0; 
//	else
		for(int i=0; i<n;i++)
			xMin = qMin(xMin, x[i]);
	return xMin;
}


/**
 * Returns the maximum x value of this curve
 *@return the x max value, or -99999999.0 if the curve has no points
 */
double CCurve::xMax()
{
	double xMax = -99999999.0;
//	if(n==0) xmax = 1.0; 
//	else
		for(int i=0; i<n;i++)
			xMax = qMax(xMax, x[i]);
	return xMax;
}


/**
 * Returns the minimum y value of this curve
 *@return the y min value, or +99999999.0 if the curve has no points
 */
double CCurve::yMin()
{
	double yMin = 99999999.0;
//	if(n==0) ymin = .0; 
//	else
		for(int i=0; i<n;i++)
			yMin = qMin(yMin, y[i]);
	return yMin;
}


/**
 * Returns the maximum y value of this curve
 *@return the y max value, or -99999999.0 if the curve has no points
 */
double CCurve::yMax()
{
	double yMax = -99999999.0;
//	if(n==0) ymax = 1.0; 
//	else
		for(int i=0; i<n;i++)
			yMax = qMax(yMax, y[i]);
	return yMax;
}


/**
 * Return the visibility of the curve as a boolean.
 * @return true if the curve should be displayed, false otherwise
 */
bool CCurve::IsVisible()
{
	return m_bIsVisible;
}


/**
 * Return the visibility of the points as a boolean.
 * @return true if the points should be displayed, false otherwise
 */
bool CCurve::PointsVisible()
{
	return m_bShowPoints;
}


/**
 * Resets the content of the curve by setting the count to 0
 */
void CCurve::ResetCurve()
{
	n = 0;
}


/**
 * Sets the curve title
 *@param Title the new curve's title
 */
void CCurve::SetTitle(QString Title)
{
	m_CurveName = Title;
}


/**
 * Sets the curve's color
 * @param clr the new QColor value for the curve
 */
void CCurve::SetColor(QColor clr)
{
	CurveColor = clr;
}


/**
 * Sets the index of the currently selected point of this curve
 * @param n the point to select
 */
void CCurve::SetSelected(int n)
{
	m_iSelected = n;
}


/**
 * Sets the curve's style
 * @param nStyle the index of the new curve's style
 */
void CCurve::SetStyle(int nStyle)
{
	CurveStyle = nStyle;
}

/**
 *Sets the curve's width
 *@param nWidth the new curve's width in pixels
 **/
void CCurve::SetWidth(int nWidth)
{
	CurveWidth = nWidth;
}

/**
 * Sets the visibility of the curve in the graphs
 *@param bVisible true if the curve is to be displayed, false otherwise
 */
void CCurve::SetVisible(bool bVisible)
{
	m_bIsVisible = bVisible;
}


/**
 * Sets the visibility of the points in the graphs
 *@param bShow true if the points are to be displayed, false otherwise
 */
void CCurve::ShowPoints(bool bShow)
{
	m_bShowPoints = bShow;
}

/**
 * Return the index of the currently selected point
 *@param the index of the currently selected point
*/
int CCurve::GetSelected()
{
	return m_iSelected;
}
