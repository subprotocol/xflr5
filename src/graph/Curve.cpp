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


int CCurve::AddPoint(double xn, double yn)
{
	x[n] = xn;
	y[n] = yn;
	n++;
	return n;
}


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


QColor CCurve::GetColor()
{
	return CurveColor;
}

void CCurve::GetBWStyle(QColor &color, int &style, int &width)
{
	color = CurveColor;
	style = CurveStyle;
	width = CurveWidth;
//	GetBWColor(color, style, width);
}


CVector CCurve::GetPoint(int ref)
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



int CCurve::GetClosestPoint(double xs, double ys, double &dist )
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

void CCurve::GetClosestPoint(double xs, double ys, double &dist ,int &n)
{
	Graph *pGraph = (Graph*)m_pParentGraph;
	static int ref;
	static double d2;
	ref = -1;
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


void CCurve::GetClosestPoint(double const &xs, double const &ys, double &xSel, double &ySel, double &dist, int &nSel)
{
//	Graph *pGraph = (Graph*)m_pParentGraph;
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



int CCurve::GetCount()
{
	return n;
}


int CCurve::GetStyle()
{
	return CurveStyle;
}



void CCurve::GetTitle(QString &string)
{
	string =  m_CurveName;
}


QString CCurve::GetTitle()
{
	return m_CurveName;
}



int CCurve::GetWidth()
{
	return CurveWidth;
}


double CCurve::GetxMin()
{
	double xMin = 99999999.0;
//	if(n==0) xmin = .0; 
//	else
		for(int i=0; i<n;i++)
			xMin = qMin(xMin, x[i]);
	return xMin;
}


double CCurve::GetxMax()
{
	double xMax = -99999999.0;
//	if(n==0) xmax = 1.0; 
//	else
		for(int i=0; i<n;i++)
			xMax = qMax(xMax, x[i]);
	return xMax;
}


double CCurve::GetyMin()
{
	double yMin = 99999999.0;
//	if(n==0) ymin = .0; 
//	else
		for(int i=0; i<n;i++)
			yMin = qMin(yMin, y[i]);
	return yMin;
}


double CCurve::GetyMax()
{
	double yMax = -99999999.0;
//	if(n==0) ymax = 1.0; 
//	else
		for(int i=0; i<n;i++)
			yMax = qMax(yMax, y[i]);
	return yMax;
}


bool CCurve::IsVisible()
{
	return m_bIsVisible;
}


bool CCurve::PointsVisible()
{
	return m_bShowPoints;
}


void CCurve::ResetCurve()
{
	n = 0;
}


void CCurve::SetTitle(QString Title)
{
	m_CurveName = Title;
}


void CCurve::SetColor(QColor clr)
{
	CurveColor = clr;
}


void CCurve::SetSelected(int n)
{
	m_iSelected = n;
}

void CCurve::SetStyle(int nStyle)
{
	CurveStyle = nStyle;
}


void CCurve::SetWidth(int nWidth)
{
	CurveWidth = nWidth;
}


void CCurve::SetVisible(bool bVisible)
{
	m_bIsVisible = bVisible;
}

void CCurve::ShowPoints(bool bShow)
{
	m_bShowPoints = bShow;
}



int CCurve::GetSelected()
{
	return m_iSelected;
}
