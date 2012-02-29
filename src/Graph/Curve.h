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


#ifndef CURVE_H
#define CURVE_H

#include "../Params.h"
#include "../Objects/CVector.h"
#include <QColor>

class CCurve : public QObject
{
	friend class Graph;

public:
	CVector GetPoint(int ref);

	bool PointsVisible();
	bool IsVisible();

	void Copy(CCurve *pCurve);
	void GetTitle(QString &string);
 	void ShowPoints(bool bShow);
	void SetVisible(bool bVisible);
	void SetColor(QColor clr);
	void SetStyle(int nStyle);
	void SetSelected(int n);
	void SetWidth(int nWidth);
	void SetTitle(QString Title);
	void GetBWStyle(QColor &color, int &style, int &width);
	void GetBWStyle();
	void ResetCurve();

	QString GetTitle();
	
	double GetxMin();
	double GetxMax();
	double GetyMin();
	double GetyMax();

	int AddPoint(double xn, double yn);
	int GetStyle();
	int GetWidth();
	int GetSelected();
	int GetCount();

	int GetClosestPoint(double xs, double ys, double &dist);
	void GetClosestPoint(double xs, double ys, double &dist, int &n);
	void GetClosestPoint(double const &xs, double const &ys, double &xSel, double &ySel, double &dist, int &nSel);

	QColor  GetColor();

	//	Curve Data
	int n;
	double x[1000];
	double y[1000];
	bool m_bShowPoints;

	CCurve();

private:	
	bool m_bIsVisible;
	QColor CurveColor;
	QString m_CurveName;
	int CurveStyle;
	int CurveWidth;
	int m_iSelected;
	void *m_pParentGraph;
};



#endif
