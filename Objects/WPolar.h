/****************************************************************************

    WPolar Class
	Copyright (C) 2005 Andre Deperrois XFLR5@yahoo.com

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


// WPolar.h: interface for the CWPolar class.
//
//////////////////////////////////////////////////////////////////////


#ifndef WPOLAR_H
#define WPOLAR_H

#include "WOpp.h"
#include "POpp.h"



class CWPolar
{
	friend class CWing;
	friend class QMiarex;
	friend class MainFrame;
	friend class WPolarDlg;
	friend class GL3dViewDlg;
	friend class LLTAnalysisDlg;
        friend class VLMAnalysisDlg;
        friend class PanelAnalysisDlg;

public:
	CWPolar();
	virtual ~CWPolar();

	void* m_pParent;

private:
	int m_AnalysisType;
	int m_Type;
	double m_QInf;
	double m_Weight;
	double m_ASpec;
	double m_XCmRef;
	double m_Beta;
	double m_AMem;

	int m_Style;
	int m_Width;
	QColor m_Color;

	QString m_UFOName;
	QString m_PlrName;

	// general aerodynamic data - specific to a polar
	double m_Density  ;
	double m_Viscosity;
	double pi;
	double m_WArea;//for lift and drag calculations
	double m_WMAChord;// for moment calculations
	double m_WSpan;//for moment calculations
	double m_Height;//for ground effect

	bool m_bIsVisible;
	bool m_bShowPoints;
	bool m_bVLM1;
	bool m_bThinSurfaces;
	bool m_bGround;
	bool m_bWakeRollUp;
	bool m_bTiltedGeom;
	bool m_bViscous;
	bool m_bPolar;//true if classic polar, false, if control polar

	int m_NXWakePanels;
	double m_TotalWakeLength;
	double m_WakePanelFactor;

	bool SerializeWPlr(QDataStream &ar, bool bIsStoring);

	void AddPoint(CWOpp* pWOpp);
	void AddPoint(CPOpp* pPOpp);
	void CalculatePoint(int i);
	void Copy(CWPolar *pWPolar);
	void Export(QString FileName, int FileType);
	void Remove(int i);
	void ResetWPlr();

	// the rest is litterature
	QList <double> m_Alpha; //angle of attack
	QList <double> m_Cl;    //lift coef.
	QList <double> m_CY;    //Side Force
	QList <double> m_ICd;   //induced drag coef.
	QList <double> m_PCd;   //profile drag coef.
	QList <double> m_TCd;   //total drag coef.
	QList <double> m_GCm;   //Total Pitching Moment coefficient
	QList <double> m_GRm;   //Total rolling moment
	QList <double> m_GYm;   //Total yawing moment coefficient
	QList <double> m_VYm;   //Profile yawing Moment coefficient
	QList <double> m_IYm;   //induced yawing moment coefficient
	QList <double> m_ClCd;  //glide ratio
	QList <double> m_Cl32Cd;  //powerfactor
	QList <double> m_QInfinite;  //free stream speed - type2 Wpolars
	QList <double> m_L;//Wing lift
	QList <double> m_D;//Wing Drag
	QList <double> m_1Cl;//1/Cl, special for Matthieu
	QList <double> m_Vx;//horizontal speed
	QList <double> m_Vz;//Sink speed = sqrt(2mg/rho/S)/powerfactor
	QList <double> m_Gamma;//glide angle = Atan(Cx/Cz), in degrees
	QList <double> m_Rm;
	QList <double> m_Ym;
	QList <double> m_Pm;
	QList <double> m_XCP;// centre of pressure X-position relative to the wing's root LE
	QList <double> m_YCP;// centre of pressure Y-position relative to the wing's root LE
	QList <double> m_MaxBending;
	QList <double> m_VertPower;
	QList <double> m_Oswald;
	QList <double> m_SM;//pseudo Static Margin = (XCP-XCmRef)/m.a.c;
	QList <double> m_Ctrl;//Ctrl variable

	//control data - Type 5 polars
	int m_nControls;
	double m_MinControl[100], m_MaxControl[100];
	bool   m_bActiveControl[100];


};

#endif
