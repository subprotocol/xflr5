/****************************************************************************

    Reference Foil Class
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



#ifndef CFOIL_H
#define CFOIL_H

#include <QColor>
#include <QPainter>
#include <QPoint>
#include "../Params.h"
#include "CVector.h"
#include "CRectangle.h"
#include <QTextStream>


class CFoil
{
	friend class QAFoil;
	friend class CPF;
	friend class CSF;
	friend class CSplinedPoints;
	friend class CSurface;
	friend class GLWidget;
	friend class QXDirect;
	friend class QXInverse;
	friend class QMiarex;
	friend class XFoil;
	friend class FoilPolarDlg;
	friend class CWing;
	friend class MainFrame;
	friend class NacaFoilDlg;
	friend class FoilCoordDlg;
	friend class TwoDPanelDlg;
	friend class InverseOptionsDlg;
	friend class InterpolateFoilsDlg;
	friend class FoilGeomDlg;
	friend class TEGapDlg;
	friend class LEDlg;
	friend class FlapDlg;
	friend class CAddDlg;
	friend class BatchDlg;
	friend class GL3dWingDlg;
	friend class WingDlg;
	friend class AFoil;
	friend class FoilSelectionDlg;
	friend class ManageFoilsDlg;


public:
	static double pi;
	CFoil();
	~CFoil();
	void GetFoilName(QString &FoilName);
	int IsPoint(CVector Real);
	void DrawFoil(QPainter &painter, double alpha, double scalex, double scaley,QPoint Offset);
	void DrawPoints(QPainter &painter, double scalex, double scaley, QPoint Offset);
	void DrawMidLine(QPainter &painter, double scalex, double scaley, QPoint Offset);

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
	bool CompMidLine(bool bParams);

	bool ExportFoil(QTextStream &out);
	bool InitFoil();
	bool SetCamber(double f, double u);

	void CopyFoil(CFoil *pSrcFoil);
	void DrawFoil(double alpha, double scalex, double scaley, CVector const &Offset);
	void DrawPoints(double scalex, double scaley, CVector Offset);
	void DrawMidLine(double scalex, double scaley, CVector Offset);
	void Serialize(QDataStream &ar, bool bIsStoring);
	void SetFlap();
	void SetTEFlap();
	void SetLEFlap();
	void SetNaca009();
	void SetLEFlapData(bool bFlap, double xhinge, double yhinge, double angle);
	void SetTEFlapData(bool bFlap, double xhinge, double yhinge, double angle);


	bool Intersect(CVector A, CVector B, CVector C, CVector D, CVector *M);
	bool IsBetween(int f, int f1, int f2);
	bool IsBetween(int f, double f1, double f2);

private:

	QString m_FoilName;		// the foil's name...
	QColor m_FoilColor;	//... and its color

	bool m_bVisible;	//true if the foil is to be displayed
	bool m_bCenterLine;	//true if the foil mid camber line is to be displayed
	bool m_bPoints;		//true if the foil's panels are to be displayed
	bool m_bSaved;		//true if the design modifications have been saved
	int m_iExt, m_iInt;	//the number of points on the upper and lower surfaces of the current foil
	int m_iBaseExt, m_iBaseInt;	//the number of points on the upper and lower surfaces of the base foil

	int m_nFoilStyle;
	int m_nFoilWidth;
	int m_iHighLight;

	int n;				// the number of points of the current foil
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

};

#endif
