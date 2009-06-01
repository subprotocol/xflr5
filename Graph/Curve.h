/****************************************************************************

	Curve Class
	Copyright (C) 2003-2009 Andre Deperrois xflr5@yahoo.com

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
// Curve.h: interface for the CCurve class.
//
//////////////////////////////////////////////////////////////////////
#ifndef CURVE_H
#define CURVE_H

#include"../Objects/CVector.h"
#include <QColor>

class CCurve : public QObject
{
	friend class Graph;

public:
	CVector GetClosestRealPoint(double xs);
	CVector GetPoint(int ref);

	bool PointsVisible();
	bool IsVisible();

	void Copy(CCurve *pCurve);
	void GetTitle(QString &string);
 	void ShowPoints(bool bShow);
	void SetVisible(bool bVisible);
	void SetColor(QColor clr);
	void SetStyle(int nStyle);
	void SetWidth(int nWidth);
	void SetTitle(QString Title);
	void GetBWStyle(QColor &color, int &style, int &width);
	void GetBWStyle();
	void ResetCurve();

	double GetxMin();
	double GetxMax();
	double GetyMin();
	double GetyMax();

	int AddPoint(double xn, double yn);
	int GetClosestPoint(double xs);
	int GetClosestPoint(double xs, double ys);
	int GetStyle();
	int GetWidth();

	QColor  GetColor();

	//	Curve Data
	int n;
	double x[600];
	double y[600];
	bool m_bShowPoints;

	CCurve();
	virtual ~CCurve();

private:	
	bool m_bIsVisible;
	QColor CurveColor;
	QString m_strName;
	int CurveStyle;
	int CurveWidth;

	void *m_pParentGraph;
};



#endif
