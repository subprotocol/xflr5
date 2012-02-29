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

// Spline.h: interface for the CSpline class.
//
//////////////////////////////////////////////////////////////////////


#ifndef CSpline_H
#define CSpline_H
#include <QPainter>
#include <QRect>
#include <QFile>
#include <QTextStream>
#include "../Objects/CVector.h"
#include "../Params.h"

class CSpline
{
	friend class CSF;
	friend class QXInverse;
	friend class InverseOptionsDlg;
public:

	CSpline();
	virtual ~CSpline();

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

	void Copy(CSpline *pSpline);
	void CopySymetric(CSpline *pSpline);
	void Export(QTextStream &out, bool bExtrados);
	void SplineCurve();
	void SplineKnots();
	
	void SetStyle(int style);
	void SetWidth(int width);
	void SetColor(QColor color);
	void SetSplineParams(int style, int width, QColor color);

	int m_iHighlight, m_iSelect;
	int m_iKnots;
	int m_iRes;
	int m_iCtrlPoints;
	int m_iDegree;
	double m_knots[SPLINECONTROLSIZE];
	CVector m_Input[SPLINECONTROLSIZE];
	CVector m_Output[IQX2];

	QRect m_rViewRect;

private:
	QColor m_Color;
	int m_Style, m_Width;
};


#endif
