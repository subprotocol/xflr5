/****************************************************************************

    Reference Foil Class
	Copyright (C) 2003 Andr�Deperrois xflr5@yahoo.com

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

// Foil.h: interface for the CFoil class.
//
//////////////////////////////////////////////////////////////////////


#pragma once

#include <QPainter>
#include <QColor>
#include <QString>
#include <vector>
#include <iostream>
#include "../misc/ComDef.h"
#include "../misc/Vector.h"
#include "helper.h"

#define cMaxFoilName			1024




class CFoil  //: public CObject
{
	friend class DesignChild;
	friend class FoilMidLinePath;
// 	friend class CAFoil;
// 	friend class CBatchDlg;
// 	friend class CAFoilCtrlBar;
// 	friend class CCAddDlg;
// 	friend class CEditFoilDlg;
// 	friend class CFoilSettingsDlg;
// 	friend class CFoilAnalysisDlg;
// 	friend class CFlapDlg;
// 	friend class CGeomDlg;
// 	friend class CMainFrame;
// 	friend class CXDirect;
// 	friend class CMiarex;
// 	friend class CPolar;
// 	friend class CWing;
// 	friend class CListPlrDlg;
// 	friend class CNameDlg;
	friend class CSF;
// 	friend class CPF;
// 	friend class CSplinedPoints;
// 	friend class CInterpolateDlg;
// 	friend class CXInverse;
// 	friend class C2DPanelDlg;
// 	friend class CSelectFoilDlg;
// 	friend class CSurface;
// 	friend class CTEGapDlg;
// 	friend class CLEDlg;
// 	friend class XFoil;
// 	friend class CNacaDlg;
// 	friend class CUFOListDlg;
// 	friend class OpPoint;

public:
	int IsPoint(CVector Real);
	CFoil();
	virtual ~CFoil();

	const tFoilCoo &Getx(){return xV;};
	const tFoilCoo &Gety(){return yV;};
	const QString &GetFoilName(){return m_FoilName;};
	bool empty(){return xV.empty()||yV.empty();}
	double Length(){return xMax-xMin;}
	void Scale(double scaleX, double scaleY);

	// read a foil from a std istream
	bool Read(std::istream& stream);

private:
	QString m_FoilName;		// the foil's name...
	QColor m_FoilColor;	//... and its color

	bool m_bVisible;	//true if the foil is to be displayed
	bool m_bCenterLine;	//true if the foil mid camber line is to be displayed
	bool m_bPoints;		//true if the foil's panels are to be displayed
	bool m_bSaved;		//true if the design modifications have been saved
	int m_iExt, m_iInt;	//the number of points on the upper and lower surfaces of the current foil
	int m_iBaseExt, m_iBaseInt;	//the number of points on the upper and lower surfaces of the base foil

	Qt::PenStyle m_nFoilStyle;
	int m_nFoilWidth;
	int m_iHighLight;

	int n;				// the number of points of the current foil
	tFoilCoo xV;
	tFoilCoo yV;
	double xMin,xMax;
	double yMin,yMax;
	double  x[IBX],  y[IBX];	// the point coordinates of the current foil
	double nx[IBX], ny[IBX];	// the normal vector coordinates of the current foil's points
	//Base geometry;
	int nb;				// the number of points of the base foil
	double  xb[IBX],  yb[IBX];	// the point coordinates of the base foil

	double m_fCamber, m_fXCamber;	//foil camber and max camber position
	double m_fThickness, m_fXThickness;// foil thickness and thickness position

	double m_Gap;			// trailing edge gap
	CVector m_LE, m_TE;		// leading edge and trailing edge points


//	CRect m_rViewRect;

	CVector m_rpBaseMid[1001];	//base mid camber line points
	CVector m_BaseExtrados[IQX];	//base upper surface points
	CVector m_BaseIntrados[IQX];	//base lower surface points

	CVector m_rpMid[1001];		//mid camber line points
	CVector m_rpExtrados[IQX];	//upper surface points
	CVector m_rpIntrados[IQX];	//lower surface points

// Trailing edge flap  data
	bool m_bTEFlap;
	double m_TEFlapAngle;
	double m_TEXHinge, m_TEYHinge;
// Leading edge flap  data
	bool m_bLEFlap;
	double m_LEFlapAngle;
	double m_LEXHinge, m_LEYHinge;

	bool CompMidLine(bool bParams);
// 	bool DrawPoints(CDC *pDC, double scalex, double scaley, CPoint Offset, CRect DrawRect, bool IsPrinting);
	bool ExportFoil(const QString &FileName);
	bool InitFoil();
	bool SetCamber(double f, double u);

	void CopyFoil(CFoil *pSrcFoil);
 	void DrawFoil(QPainter &painter, double alpha, double scalex, double scaley, QPoint Offset, QRect DrawRect,  bool IsPrinting);
 	void DrawMidLine(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect DrawRect, bool IsPrinting);
// 	void Serialize(CArchive &ar);
	void SetFlap();
	void SetNaca009();
	void SetLEFlap(bool bFlap, double xhinge, double yhinge, double angle);
	void SetTEFlap(bool bFlap, double xhinge, double yhinge, double angle);
	
	double DeRotate();
	double GetBaseUpperY(double x);
	double GetBaseLowerY(double x);
	double GetUpperY(double x);
	double GetLowerY(double x);
	double GetMidY(double x);
	double GetCamber(double x);
	double GetCamberAngle(double x);
	double GetCamberSlope(double x);
	double GetLength();
	double GetArea();
	double GetTopSlope(double x);
	double GetBotSlope(double x);
	double NormalizeGeometry();
};

