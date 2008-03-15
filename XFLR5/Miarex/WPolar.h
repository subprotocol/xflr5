/****************************************************************************

    WPolar Class
    Copyright (C) 2005 André Deperrois XFLR5@yahoo.com

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


#pragma once

#include <afxtempl.h>
#include "WOpp.h"
#include "POpp.h"



class CWPolar : public CObject  
{
	friend class CMiarex;
	friend class CWing;
	friend class CPlane;
	friend class CSurface;
	friend class CMainFrame;
	friend class CEditPlrDlg;
	friend class CLLTDlg;
	friend class CCurveDlgBar;
	friend class CWOperDlgBar;
	friend class CPOperDlgBar;
	friend class CWngAnalysis;
	friend class CCtrlAnalysis;
	friend class CNameDlg;
	friend class CVLMDlg;
	friend class CVLMThread;
	friend class C3DPanelDlg;
	friend class C3DPanelThread;
	friend class CUFOListDlg;

public:
//	DECLARE_SERIAL (CWPolar);

	CWPolar(CWnd* pParent=NULL);
	virtual ~CWPolar();

private:
	int m_AnalysisType;
	int m_Type;
	double m_QInf;
	double m_Weight;
	double m_ASpec;
	double m_XCmRef;

	double m_AMem;

	int m_Style;
	int m_Width;
	COLORREF m_Color;

	CString m_UFOName;
	CString m_PlrName;

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

	bool SerializeWPlr(CArchive &ar);

	void AddPoint(CWOpp* pWOpp);
	void AddPoint(CPOpp* pPOpp);
	void CalculatePoint(int i);
	void Copy(CWPolar *pWPolar);
	void Export(CString FileName);
	void GetBWStyle(COLORREF &color, int &style, int &width);
	void Remove(int i);
	void ResetWPlr();

	// the rest is litterature
	CArray <double, double> m_Alpha; //angle of attack
	CArray <double, double> m_Cl;    //lift coef.
	CArray <double, double> m_ICd;   //induced drag coef.
	CArray <double, double> m_PCd;   //profile drag coef.
	CArray <double, double> m_TCd;   //total drag coef.
	CArray <double, double> m_GCm;   //Total Pitching Moment coefficient
	CArray <double, double> m_GRm;   //Total rolling moment
	CArray <double, double> m_GYm;   //Total yawing moment coefficient
	CArray <double, double> m_VYm;   //Profile yawing Moment coefficient
	CArray <double, double> m_IYm;   //induced yawing moment coefficient
	CArray <double, double> m_ClCd;  //glide ratio
	CArray <double, double> m_Cl32Cd;  //powerfactor
	CArray <double, double> m_QInfinite;  //free stream speed - type2 Wpolars
	CArray <double, double> m_L;//Wing lift
	CArray <double, double> m_D;//Wing Drag
	CArray <double, double> m_1Cl;//1/Cl, special for Matthieu
	CArray <double, double> m_Vx;//horizontal speed
	CArray <double, double> m_Vz;//Sink speed = sqrt(2mg/rho/S)/powerfactor
	CArray <double, double> m_Gamma;//glide angle = Atan(Cx/Cz), in degrees
	CArray <double, double> m_Rm;
	CArray <double, double> m_Ym;
	CArray <double, double> m_Pm;
	CArray <double, double> m_XCP;// centre of pressure X-position relative to the wing's root LE
	CArray <double, double> m_YCP;// centre of pressure Y-position relative to the wing's root LE
	CArray <double, double> m_MaxBending;
	CArray <double, double> m_VertPower;
	CArray <double, double> m_Oswald;
	CArray <double, double> m_SM;//Static Margin = (XCP-XCmRef)/m.a.c;


	CWnd* m_pParent;
		

private:
};

