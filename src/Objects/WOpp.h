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


#ifndef CWOPP_H
#define CWOPP_H
#include "CVector.h"
#include "../Params.h"
#include <QString>
#include <QColor>
#include <QTextStream>
#include <complex>

using namespace std;

class CWOpp
{
	friend class QMiarex;
	friend class MainFrame;
	friend class CWPolar;
	friend class CPOpp;
	friend class StabViewDlg;
	friend class PanelAnalysisDlg;
	friend class ObjectPropsDlg;

public:
	CWOpp();

private:
//________________METHODS____________________________________
	bool SerializeWOpp(QDataStream &ar, bool bIsStoring, int ProjectFormat);
	bool Export(QTextStream &out, int FileType, bool bDataOnly=false);
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
	double m_Twist[MAXSTATIONS+1];		// twist at span stations

	//RESULTS
	double m_Re[MAXSTATIONS+1];		// Reynolds number at stations
	double m_Cm[MAXSTATIONS+1];			//Total pitching moment coefficient at stations
	double m_CmAirf[MAXSTATIONS+1];		//airfoil Pitching moment coefficient at stations about 1/4 chord point
	double m_BendingMoment[MAXSTATIONS+1];

	double m_CY, m_CX;			//Side force, drag coefficients
	double m_MaxBending;		// max bending moment along the span


	//non dimensional stability derivatives
	double CXu, CZu, Cmu, CXa, CLa, Cma, CXq, CLq, Cmq, CYb, CYp, CYr, Clb, Clp, Clr, Cnb, Cnp, Cnr;
	//non-dimensional control derivatives
	double CXe[MAXCONTROLS],CYe[MAXCONTROLS],CZe[MAXCONTROLS];
	double CLe[MAXCONTROLS],CMe[MAXCONTROLS],CNe[MAXCONTROLS];
	
	double m_ALong[4][4];	//Longitudinal State Matrix
	double m_ALat[4][4];	//Lateral State Matrix
	double m_BLong[MAXCONTROLS][4];
	double m_BLat[MAXCONTROLS][4];
	

public:
	bool m_bOut;		// true if the WOpp is the result of an unconverged calculation

	int m_NVLMPanels;	// number of VLMpanels
	int m_AnalysisMethod;	//1=LLT, 2=VLM, 3=3D Panels
	int m_Type;		// Polar Analysis Type
	int m_NStation;		// number of stations along wing span
	int m_nFlaps;		// number of flaps

	double m_Span;
	double m_MAChord;
	double m_QInf;
	double m_Cp[VLMMAXMATSIZE];		// lift coeffs for each panel
	double m_G[VLMMAXMATSIZE];			// vortice or doublet strengths
	double m_Sigma[VLMMAXMATSIZE];		// source strengths
	double m_Alpha, m_Beta, m_Phi;
	double m_SpanPos[MAXSTATIONS+1];	// station spanwise positions
	double m_Ai[MAXSTATIONS+1];		//Induced angles, in degrees
	double m_Chord[MAXSTATIONS+1];		// chord at stations
	double m_ICd[MAXSTATIONS+1];		//Drag coefficient at stations
	double m_PCd[MAXSTATIONS+1];		//Drag coefficient at stations
	double m_Cl[MAXSTATIONS+1];		//Lift coefficient at stations
	double m_XCPSpanRel[MAXSTATIONS+1];	//Centre of pressure position at stations
	double m_XCPSpanAbs[MAXSTATIONS+1];	//Centre of pressure position at stations
	double m_StripArea[MAXSTATIONS+1];	
	double m_XTrTop[MAXSTATIONS+1];		// Transition location - top
	double m_XTrBot[MAXSTATIONS+1];		// Transition location - bottom
	CVector m_Vd[MAXSTATIONS];		// speed deflection at trailing edge
	CVector m_F[MAXSTATIONS];		// Stripforce
	double m_FlapMoment[MAXPANELS]; 		// flap hinge moments

	double m_CL;				// Wing lift coefficient
	double m_VCD;			// wing viscous drag coefficient
	double m_ICD;			// wing induced drag coefficient
	double m_XCP, m_YCP;		// centre of pressure position relative to the wing's XCref
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
