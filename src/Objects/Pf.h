/****************************************************************************

	Splined Curve Foil class
    Copyright (C) 2004 Andre Deperrois XFLR5@yahoo.com
 
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


#ifndef CSPLINEDPOINTS_H
#define CSPLINEDPOINTS_H

#include "Foil.h"

class CSplinedPoints 
{
	friend class SplineCtrlsDlg;
public:
	bool CompSlopes();
	
	void DrawControlPoints(QPainter &painter, double scalex, double scaley, QPoint Offset);
	void DrawSplines(QPainter &painter, double scalex, double scaley, QPoint Offset);
	void DrawRearPoint(QPainter &painter, double scalex, double scaley, QPoint Offset);
	bool RemovePoint(int k);

	void SetCurveParams(int style, int width, QColor color);

	int InsertPoint(double x, double y);
	int IsControlPoint(CVector Real, double ZoomFactor);
	int IsControlPoint(double x, double y, double Zoom);
	int IsRearPoint(CVector Real, double ZoomFactor);

	double GetY(double x);

	void ExportToFile(QTextStream &out, bool bExtrados);
	void ExportToBuffer(CFoil *pFoil, bool bExtrados);

	CSplinedPoints();

	bool m_bOutPts;
	int m_iHighlight;
	int m_iSelect;
	int m_iPoints;				// number of control points
	int m_Freq;
	double m_Outy[1001];
	CVector m_RearPoint;
	CVector m_ctrlPoint[SPLINECONTROLSIZE]; //control points of the foil
	CVector m_Slope[SPLINECONTROLSIZE];    //slope vector at each point

	QRect m_rViewRect;
	QColor m_Color;
	int m_Style, m_Width;

};


class CPF 
{
	friend class QAFoil;
	friend class MainFrame;
	friend class SplineCtrlsDlg;

public:
	CPF();

private:
	bool CompMidLine(bool first = false);
	bool InitSplinedFoil();
	bool Serialize(QDataStream &ar, bool bISStoring);

	void Copy(CPF* pPF);
	void DrawCtrlPoints(QPainter &painter, double scalex, double scaley, QPoint Offset);
	void DrawFoil(QPainter &painter, double scalex, double scaley, QPoint Offset);
	void DrawMidLine(QPainter &painter, double scalex, double scaley, QPoint Offset);
	void ExportToBuffer(CFoil *pFoil);
	void ExportToFile(QTextStream &out);
	void SetOutPoints(bool state);
	void SetViewRect(QRect rc);
	void Update();
	void SetCurveParams(int style, int width, QColor color);

	bool m_bModified;
	bool m_bVisible, m_bOutPoints, m_bCenterLine;

	int m_OutPoints;

	int m_FoilStyle;
	int m_FoilWidth;
	QColor m_FoilColor;

	double m_fCamber;
	double m_fThickness;
	double m_fxCambMax;
	double m_fxThickMax;

	QString m_strFoilName;

	CSplinedPoints m_Extrados;
	CSplinedPoints m_Intrados;
	CVector m_rpMid[1001];

};
 
#endif
