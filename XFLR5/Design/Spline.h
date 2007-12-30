/****************************************************************************

    Spline  Class
	Copyright (C) 1996 Paul Bourke	http://astronomy.swin.edu.au/~pbourke/curves/spline/
	Copyright (C) 2003 André Deperrois xflr5@yahoo.com

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


#pragma once

#include "../XDirect/Foil.h"

class CSpline
{
public:
	int m_iHighlight, m_iSelect;
	int m_iKnots;
	int m_iRes;
	int m_iCtrlPoints;
	int m_iDegree;
	double m_knots[SPLINECONTROLSIZE];
	CVector m_Input[SPLINECONTROLSIZE];
	CVector m_Output[IQX2];

	CRect m_rViewRect;

//	bool AddPoint(double x, double y);
	bool DrawControlPoint(CDC *pDC, int i, double scalex, double scaley, CPoint Offset, bool IsPrinting);
	bool DrawSpline(CDC *pDC, double scx, double scy, CPoint Offset, bool IsPrinting);
	bool InsertPoint(double x, double y);
	bool RemovePoint(int k);
	int IsControlPoint(CVector Real);
	int IsControlPoint(double x, double y, double zx, double zy);
	int IsControlPoint(CVector Real, double ZoomFactor);
	double SplineBlend(int i, int p, double t);
	double GetY(double x);

	void Copy(CSpline *pSpline);
	void DrawCtrlPoints(CDC *pDC,double scx, double scy, CPoint Offset, bool IsPrinting);
	void DrawOutputPoint(CDC *pDC, int i,double scalex, double scaley, CPoint Offset, bool IsPrinting);
	void Export(CStdioFile *pFile, bool bExtrados);
	void SplineCurve();
	void SplineKnots();

	CSpline();
	virtual ~CSpline();

};

class CBezier
{
public:
	int m_iHighlight, m_iSelect;
	int m_iRes;
	int m_iCtrlPoints;
	int m_iDegree;

	CVector m_Input[SPLINECONTROLSIZE];
	CVector m_Output[IQX2];

	CRect m_rViewRect;

	bool AddPoint(double x, double y);
	bool InsertPoint(double x, double y);
	bool RemovePoint(int k);
	int IsControlPoint(CVector Real);
	int IsControlPoint(double x, double y, double zx, double zy);
	int IsControlPoint(CVector Real, double ZoomFactor);
	double GetY(double x);
	void BezierCurve();
	void Copy(CBezier *pBezier);
	void DrawCtrlPoints(CDC *pDC,double scx, double scy, CPoint Offset, bool IsPrinting);
	void DrawOutputPoint(CDC *pDC, int i,double scalex, double scaley, CPoint Offset, bool IsPrinting);
	void Export(CStdioFile *pFile, bool bExtrados);


	CBezier();
	virtual ~CBezier();

};
