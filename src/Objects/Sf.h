/****************************************************************************

    Spline Foil Class
	Copyright (C) 2003-2010 Andre Deperrois adeperrois@xflr5.com

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


#ifndef CSF_H
#define CSF_H


#include "Foil.h"
#include "Spline.h"


class CSF  
{
	friend class QAFoil;;
	friend class MainFrame;
	friend class SplineCtrlsDlg;
	friend class FoilTableDelegate;


public:
	CSF();
	virtual ~CSF();

private:
	bool CompMidLine();

	bool InitSplineFoil();

	bool Serialize(QDataStream &ar, bool bIsStoring);

	void Copy(CSF* pSF);
	void DrawCtrlPoints(QPainter &painter, double scalex, double scaley, QPoint Offset);
	void DrawMidLine(QPainter &painter, double scalex, double scaley, QPoint Offset);
	void DrawFoil(QPainter &painter, double scalex, double scaley, QPoint Offset);
	void DrawOutPoints(QPainter &painter, double scalex, double scaley, QPoint Offset);
	void ExportToBuffer(CFoil *pFoil);
	void ExportToFile(QTextStream &out);
	void SetViewRect(QRect &rc);
	void Update(bool bExtrados);
	void UpdateSelected(double x, double y);
	void UpdateKnots();
	void SetCurveParams(int style, int width, QColor color);

	bool m_bModified;
	bool m_bVisible, m_bOutPoints, m_bCenterLine;

	bool m_bSymetric;
	
	int m_OutPoints;
	int	m_FoilStyle;
	int m_FoilWidth;
	double m_fCamber;
	double m_fThickness;
	double m_fxCambMax, m_fxThickMax;
	QString m_strFoilName;
	QColor m_FoilColor;
	CSpline m_Extrados;
	CSpline m_Intrados;
	CVector m_rpMid[1001];
};
#endif
