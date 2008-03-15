/****************************************************************************

    WOpp Class
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


// WOpp.h: interface for the CWOpp class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CWOpp : public CObject  
{
	friend class CMainFrame;
	friend class CWOper;
	friend class CLLTThread;
	friend class CMiarex;
	friend class CWPolar;
	friend class CListWing;
	friend class CCurveDlgBar;
	friend class CPOpp;

public:
	CWOpp();
	virtual ~CWOpp();

private:
	CString m_WingName;	// the wing name to which the WOpp belongs 
	CString m_PlrName; 	// the polar name to which the WOpp belongs

	bool m_bIsVisible;	// true if the curves should be displayed in the WOpp graphs
	bool m_bShowPoints;	// true if the curve' points should be displayed in the WOpp graphs
	bool m_bOut;		// true if the WOpp is the result of an unconverged calculation
	bool m_bVLM1;		// true if the WOpp is the result to a classic VLM calculation
	bool m_bThinSurface;// true if the WOpp is the results of a calculation on the middle surface
	bool m_bTiltedGeom;	// true if the WOpp is the results of a calculation on the tilted geometry

	int m_Style;		//WOpp display style
	int m_Width;
	COLORREF m_Color;


	int m_AnalysisType;	//1=LLT, 2=VLM, 3=3D Panels
	int m_WingType;		//0 for wing, 1 for elevator, 2 for fin
	int m_Type;		// Polar Analysis Type
	int m_NStation;		// number of stations along wing span
	int m_NVLMPanels;	// number of VLMpanels
	int m_nWakeNodes;	// number of wake nodes 
	int m_NXWakePanels;	// 
	int m_nFlaps;		// number fo flaps

	double m_FirstWakePanel;
	double m_WakeFactor;
	double m_Alpha;
	double m_QInf;
	double m_Weight;
	double m_Span;
	double m_MAChord;
	double m_Chord[MAXSTATIONS+1];		// chord at stations
	double m_SpanPos[MAXSTATIONS+1];	// station spanwise positions
	double m_StripArea[MAXSTATIONS+1];	
	double m_Twist[MAXSTATIONS+1];		// twist at span stations

	//RESULTS
	double m_Re[MAXSTATIONS+1];		// Reynolds number at stations
	double m_Ai[MAXSTATIONS+1];		//Induced angles, in degrees
	double m_Cl[MAXSTATIONS+1];		//Lift coefficient at stations
	double m_PCd[MAXSTATIONS+1];		//Drag coefficient at stations
	double m_ICd[MAXSTATIONS+1];		//Drag coefficient at stations
	double m_Cm[MAXSTATIONS+1];		//Pitching moment coefficient at stations
	double m_CmAirf[MAXSTATIONS+1];		//Pitching moment coefficient at stations
	double m_CmXRef[MAXSTATIONS+1];		//Pitching moment coefficient at stations
	double m_XCPSpanRel[MAXSTATIONS+1];	//Centre of pressure position at stations
	double m_XCPSpanAbs[MAXSTATIONS+1];	//Centre of pressure position at stations
	double m_XTrTop[MAXSTATIONS+1];		// Transition location - top
	double m_XTrBot[MAXSTATIONS+1];		// Transition location - bottom
	double m_BendingMoment[MAXSTATIONS+1];
	double m_Cp[VLMMATSIZE];		// lift coeffs for each panel
	double m_G[VLMMATSIZE];			// vortice or doublet strengths
	double m_Sigma[VLMMATSIZE];		// source strengths
	double m_FlapMoment[20]; 		// flap hinge moments

	double m_CL;				// Wing lift coefficient
	double m_ViscousDrag;			// wing viscous drag
	double m_InducedDrag;			// wing induced drag
	double m_GCm;		// wing pitching moment
	double m_GRm;			// wing rolling moment
	double m_GYm;			// geometric yawing moment
	double m_VYm;		// wing viscous yawing moment
	double m_IYm;		// wing induced yawing moment
	double m_MaxBending;		// max bending moment along the span
	double m_XCP, m_YCP;		// centre of pressure position relative to the wing's XCref


	CVector m_F[MAXSTATIONS];		// Stripforce
	CVector m_Vd[MAXSTATIONS];		// speed deflection at trailing edge

//________________METHODS____________________________________
	bool SerializeWOpp(CArchive &ar);
	bool Export(CStdioFile *pXFile);
	void GetBWStyle(COLORREF &color, int &style, int &width);
	double GetMaxLift();
};
