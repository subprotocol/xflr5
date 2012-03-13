/****************************************************************************

    WPolar Class
	Copyright (C) 2005-2010 Andre Deperrois adeperrois@xflr5.com

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
	friend class StabPolarDlg;
	friend class LLTAnalysisDlg;
	friend class LLTAnalysis;
	friend class PanelAnalysisDlg;
	friend class EditPlrDlg;
	friend class ObjectPropsDlg;
	friend class StabViewDlg;
	friend class ManageBodiesDlg;

public:
	CWPolar();
	void AddPoint(CWOpp* pWOpp);
	void AddPoint(CPOpp* pPOpp);
	void AddPoint(double alpha, double CL, double ICd, double PCd, double CY, double GCm, double VCm, double ICm, double GRm, double GYm, double IYm, double QInf, double XCP);
	void CalculatePoint(int i);
	void Export(QTextStream &out, int FileType, bool bDataOnly=false);
	void GetPolarProperties(QString &Properties, bool bData=false);
	void Remove(int i);
	void SetInertia(void *ptr, bool bPlane);
	void *GetUFOPlrVariable(int iVar);
	void ResetWPlr();
	void Copy(CWPolar *pWPolar);
	bool SerializeWPlr(QDataStream &ar, bool bIsStoring, int ProjectFormat);

	static void* s_pMainFrame;
	static void* s_pMiarex;

private:
	int m_PolarFormat;
	int m_RefAreaType;
	double m_QInf;
	double m_ASpec;
	double m_AMem;

	int m_Style;
	int m_Width;
	QColor m_Color;

	QString m_UFOName;
	QString m_PlrName;

	// general aerodynamic data - specific to a polar
	double m_Viscosity;
	double m_Height;//for ground effect
	double m_BankAngle;

	bool m_bIsVisible;
	bool m_bShowPoints;
	bool m_bGround;
	bool m_bWakeRollUp;
	bool m_bDirichlet;
	bool m_bTiltedGeom;
	bool m_bViscous;

	int m_NXWakePanels;
	double m_TotalWakeLength;
	double m_WakePanelFactor;

	
	// the rest is litterature
	QList <double> m_Alpha; //angle of attack
	QList <double> m_CL;    //lift coef.
	QList <double> m_CY;    //Side Force
	QList <double> m_ICd;   //induced drag coef.
	QList <double> m_PCd;   //profile drag coef.
	QList <double> m_TCd;   //total drag coef.
	QList <double> m_GCm;   //Total Pitching Moment coefficient
	QList <double> m_VCm;   //Viscous Pitching Moment coefficient
	QList <double> m_ICm;   //Induced Pitching Moment coefficient
	QList <double> m_GRm;   //Total rolling moment
	QList <double> m_GYm;   //Total yawing moment coefficient
	QList <double> m_VYm;   //Profile yawing Moment coefficient
	QList <double> m_IYm;   //induced yawing moment coefficient
	QList <double> m_ClCd;  //glide ratio
	QList <double> m_Cl32Cd;  //powerfactor
	QList <double> m_QInfinite;  //free stream speed - type2 Wpolars
	QList <double> m_FX;//Wing Drag
	QList <double> m_FY;//Wing Side force
	QList <double> m_FZ;//Wing Lift
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
	QList <double> m_XNP;
	QList <double> m_PhugoidFrequency;
	QList <double> m_PhugoidDamping;
	QList <double> m_ShortPeriodFrequency;
	QList <double> m_ShortPeriodDamping;
	QList <double> m_DutchRollFrequency;
	QList <double> m_DutchRollDamping;
	QList <double> m_RollDamping;
	QList <double> m_SpiralDamping;

	complex<double> m_EigenValue[8][MAXPOLARPOINTS]; //until we have a QList<complex<double>> ?

	//control data - Type 5-6-7 polars
	int m_nControls;
	double m_MinControl[4*MAXCONTROLS], m_MaxControl[4*MAXCONTROLS];
	bool   m_bActiveControl[4*MAXCONTROLS];

public:
	enumAnalysisMethod m_AnalysisMethod;
	enumPolarType m_WPolarType;

	double m_Beta;
	double m_Density  ;
	double m_WArea;//for lift and drag calculations
	double m_WMAChord;// for moment calculations
	double m_WSpan;//for moment calculations

	//Inertia properties
	double m_Mass;
	bool m_bAutoInertia;
	bool m_bThinSurfaces;
	double m_CoGIxx, m_CoGIyy, m_CoGIzz, m_CoGIxz;
	CVector m_CoG;

	bool m_bVLM1;
	bool m_bAVLControls; // true if the control is defined only by its "gain" AVL-like
						 // false if the control is defined by Active/Min/Max params
};

#endif
