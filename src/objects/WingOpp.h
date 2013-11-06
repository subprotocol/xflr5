/****************************************************************************

	WOpp Class
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



/*! \file
 *
 * This class defines the operating point object for the 3D analysis of wings.
 *
 */


#ifndef WINGOPP_H
#define WINGOPP_H


/**
*@brief
*	This class defines the operating point object which stores the data of plane analysis
*
	In the case of an analysis of an independant wing, the instance of this class is uniquely associated to an instance of a WPolar, 
	which is itself uniquely associated to the Wing object.
	Alternatively, the WingOpp may be a member variable of a PlaneOpp object.
	The data is stored in International Standard Units, i.e. meters, seconds, kg, and Newtons.
	Angular data is stored in degrees.
*/


#include "CVector.h"
#include "../params.h"
#include <QString>
#include <QColor>
#include <QTextStream>
#include <complex>

using namespace std;

class WingOpp
{
	friend class QMiarex;
	friend class MainFrame;
	friend class WPolar;
	friend class PlaneOpp;
	friend class StabViewDlg;
	friend class PanelAnalysisDlg;
	friend class ObjectPropsDlg;

public:
	WingOpp();

private:
//________________METHODS____________________________________
	bool SerializeWOpp(QDataStream &ar, bool bIsStoring);
	bool Export(QTextStream &out, enumTextFileType FileType);
	void GetBWStyle(QColor &color, int &style, int &width);
	void GetWOppProperties(QString &WOppProperties, bool bData=false);
	double GetMaxLift();

private:
	static void *s_pMiarex, *s_pMainFrame;
	QString m_WingName;	// the wing name to which the WOpp belongs
	QString m_PlrName; 	// the polar name to which the WOpp belongs

	bool m_bIsVisible;	// true if the curves should be displayed in the WOpp graphs
	bool m_bShowPoints;	// true if the curve' points should be displayed in the WOpp graphs
	bool m_bVLM1;		// true if the WOpp is the result to a classic VLM calculation
	bool m_bThinSurface;// true if the WOpp is the results of a calculation on the middle surface
	bool m_bTiltedGeom;	// true if the WOpp is the results of a calculation on the tilted geometry

	int m_Style;		//WOpp display style
	int m_Width;
	QColor m_Color;

	int m_WingType;		//0 for wing, 1 for elevator, 2 for fin
	int m_nWakeNodes;	// number of wake nodes 
	int m_NXWakePanels;	// 
	int m_nControls;	// number of controls associated to the WOpp

	double m_FirstWakePanel;
	double m_WakeFactor;
	double m_Weight;
	float m_Twist[MAXSPANSTATIONS+1];		// twist at span stations

	//RESULTS
	float m_Re[MAXSPANSTATIONS+1];		// Reynolds number at stations
	float m_Cm[MAXSPANSTATIONS+1];			//Total pitching moment coefficient at stations
	float m_CmAirf[MAXSPANSTATIONS+1];		//airfoil Pitching moment coefficient at stations about 1/4 chord point
	float m_BendingMoment[MAXSPANSTATIONS+1];

	double m_CY, m_CX;			//Side force, drag coefficients
	float m_MaxBending;		// max bending moment along the span


	//non dimensional stability derivatives
	double CXu, CZu, Cmu, CXa, CLa, Cma, CXq, CLq, Cmq, CYb, CYp, CYr, Clb, Clp, Clr, Cnb, Cnp, Cnr;
	//non-dimensional control derivatives
	double CXe,CYe,CZe;
	double CLe,CMe,CNe;
	
	double m_ALong[4][4];	//Longitudinal State Matrix
	double m_ALat[4][4];	//Lateral State Matrix
	double m_BLong[4];
	double m_BLat[4];
	

public:
	bool m_bOut;		// true if the WOpp is the result of an unconverged calculation

	int m_NVLMPanels;	// number of VLMpanels
	enumAnalysisMethod m_AnalysisMethod;
	enumPolarType m_WPolarType;		// Polar Analysis Type
	int m_NStation;		// number of stations along wing span
	int m_nFlaps;		// number of flaps

	double m_Span;
	double m_MAChord;
	double m_QInf;
	float m_Cp[VLMMAXMATSIZE];		// lift coeffs for each panel
	float m_G[VLMMAXMATSIZE];			// vortice or doublet strengths
	float m_Sigma[VLMMAXMATSIZE];		// source strengths
	double m_Alpha, m_Beta, m_Phi;
	float m_SpanPos[MAXSPANSTATIONS+1];	// station spanwise positions
	float m_Ai[MAXSPANSTATIONS+1];		//Induced angles, in degrees
	float m_Chord[MAXSPANSTATIONS+1];		// chord at stations
	float m_ICd[MAXSPANSTATIONS+1];		//Drag coefficient at stations
	float m_PCd[MAXSPANSTATIONS+1];		//Drag coefficient at stations
	float m_Cl[MAXSPANSTATIONS+1];		//Lift coefficient at stations
	float m_XCPSpanRel[MAXSPANSTATIONS+1];	//Centre of pressure position at stations
	float m_XCPSpanAbs[MAXSPANSTATIONS+1];	//Centre of pressure position at stations
	float m_StripArea[MAXSPANSTATIONS+1];
	float m_XTrTop[MAXSPANSTATIONS+1];		// Transition location - top
	float m_XTrBot[MAXSPANSTATIONS+1];		// Transition location - bottom
	CVector m_Vd[MAXSPANSTATIONS];		// speed deflection at trailing edge
	CVector m_F[MAXSPANSTATIONS];		// Stripforce
	double m_FlapMoment[MAXSPANSECTIONS]; 		// flap hinge moments

	double m_CL;				// Wing lift coefficient
	double m_VCD;			// wing viscous drag coefficient
	double m_ICD;			// wing induced drag coefficient
    double m_XCP, m_YCP,m_ZCP;		// centre of pressure position relative to the wing's XCref
	double m_GCm;		// wing pitching moment
	double m_GRm;			// wing rolling moment
	double m_VCm;		// pitching moment induced by viscous drag forces
	double m_ICm;		// pitching moment induced by pressure forces
	double m_GYm;			// geometric yawing moment
	double m_VYm;		// wing viscous yawing moment
	double m_IYm;		// wing induced yawing moment
	double m_Ctrl;		//control variable - converged value
	double m_XNP;       //Neutral point position resulting from stability calculations

	complex<double> m_EigenValue[8]; //four longitudinal and four lateral modes
	complex<double> m_EigenVector[8][4]; // (4 longitudinal + 4 lateral) x 4 components
};
#endif
