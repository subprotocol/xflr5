/****************************************************************************

    CSurface Class
    Copyright (C) 2005-2008 André Deperrois XFLR5@yahoo.com

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

//////////////////////////////////////////////////////////////////////
//
// Surface.h: interface for the CSurface class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "../XDirect/Foil.h"
#include "Body.h"

class CSurface : public CObject  
{
	friend class CWing;
	friend class CMiarex;
	friend class CMainFrame;
	friend class CVLMDlg;
	friend class CVLMThread;
	friend class C3DPanelDlg;
	friend class CWingDlg;
 
public:
	void AddFlapPanel(CPanel *pPanel);
	void Copy(CSurface const &Surface);
	void GetC4(int k, CVector &Pt, double &tau);
	void GetLeadingPt(int k, CVector &C);
	void GetNormal(double yrel, CVector &N);
	void GetTrailingPt(int k, CVector &C);
	void GetPanel(int const &k, int const &l, int const &pos);
	void GetPoint(double const &xArel, double const &xBrel, double const &yrel, CVector &Point, int const &pos=0);
	void GetyDist(int const &k, double &y1, double &y2);
	void Init();
	void ResetFlap();
	void RotateX(CVector const &O, double XTilt);
	void RotateY(CVector const &O, double YTilt);
	void RotateZ(CVector const &O, double ZTilt);
	void SetNormal();
	void SetFlap();
	void SetSidePoints(CBody *pBody=NULL);
	void SetTwist();
	void Translate(CVector const &T);

	bool IsFlapPanel(int const &p);
	bool RotateFlap(double const &Angle);
//	bool RotateFlap(double const &Angle, CPanel *pPanel, CVector *pNode);
	double GetTwist(int const &k);
	double GetChord(int const &k);
	double GetChord(double const &tau);
	double GetOffset(double const &tau);
	double GetStripSpanPos(int const &k);

	CSurface();
	virtual ~CSurface();

private :
	CVector SideA[MAXCHORDPANELS];	 
	CVector SideB[MAXCHORDPANELS];	 
	CVector SideA_T[MAXCHORDPANELS];	 
	CVector SideB_T[MAXCHORDPANELS];	 
	CVector SideA_B[MAXCHORDPANELS];	 
	CVector SideB_B[MAXCHORDPANELS];	 
	static CVector LA, LB, TA, TB;//leading and trailing corners of strip k
	static CVector VTemp;
	static CPanel *s_pPanel;
	static CVector *s_pNode;

	bool m_bIsInSymPlane;
	bool m_bTEFlap;
	bool m_bIsTipLeft, m_bIsTipRight;
	bool m_bIsLeftSurf, m_bIsRightSurf;
	bool m_bIsCenterSurf;//true if is either left or right center surface... need to connect to body
	bool m_bJoinRight; //true if the surface's right side should be connected to the next right surface's right left side - for panel analysis only

	CVector m_LA, m_LB, m_TA, m_TB;
	CVector Normal, NormalA, NormalB;
	CVector V1, V2, DL, DC, u, v;
	CVector NA, NB;

	double m_TwistA, m_TwistB;
	double m_Dihedral;
	double m_Length;// along span-y direction...
	double y1, y2, xLA, xTA, xLB, xTB;
	double chordA, chordB, zA, zB, Chord, Length;
	double m_posATE, m_posBTE;//flap break positions at sides A and B
	double m_xPointA[MAXCHORDPANELS], m_xPointB[MAXCHORDPANELS];	//chorwise position of VLM panels
	double pi;

	int m_XDistType;
	int m_YDistType;
	int m_NXPanels;
	int m_NXLead, m_NXFlap;//number of panels before and after the break
	int m_NYPanels;
	int m_NElements;

	int m_nFlapNodes, m_nFlapPanels;
	int m_FlapNode[VLMHALF];
	int m_FlapPanel[VLMHALF];

	CVector m_HingePoint;
	CVector m_HingeVector;

	CFoil *m_pFoilA, *m_pFoilB; //Left and right foils

public:
};
