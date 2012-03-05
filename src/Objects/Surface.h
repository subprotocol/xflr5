/****************************************************************************

    CSurface Class
	Copyright (C) 2005-2009 Andre Deperrois adeperrois@xflr5.com

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



#ifndef CSURFACE_H
#define  CSURFACE_H
#include "Foil.h"
#include "Body.h"
#include "CVector.h"

class CSurface
{
	friend class CWing;
	friend class QMiarex;
	friend class MainFrame;
	friend class CVLMDlg;
	friend class GL3dWingDlg;
	friend class PanelAnalysisDlg;
	friend class InertiaDlg;
	friend class CWPolar;

public:

	CSurface();

	void AddFlapPanel(CPanel *pPanel);
	void Copy(CSurface const &Surface);
	void GetC4(int k, CVector &Pt, double &tau);
	void GetLeadingPt(int k, CVector &C);
	void GetNormal(double yrel, CVector &N);
	void GetTrailingPt(int k, CVector &C);
	void GetPanel(int const &k, int const &l, int const &pos);
	void GetPoint(double const &xArel, double const &xBrel, double const &yrel, CVector &Point, int const &pos=0);
	void GetPoint(double const &xArel, double const &xBrel, double const &yrel, CVector &Point, CVector &PtNormal, int const &pos=0);
	void GetSection(double const &tau, double &Chord, double &Area, CVector &PtC4);
	void GetyDist(int const &k, double &y1, double &y2);
	void Init();
	void ResetFlap();
	void RotateX(CVector const &O, double XTilt);
	void RotateY(CVector const &O, double YTilt);
	void RotateZ(CVector const &O, double ZTilt);
	void SetNormal();
	void SetFlap();
	void SetSidePoints(CBody *pBody, double dx, double dz);
	void SetTwist1();
	void SetTwist2();
	void Translate(CVector const &T);
	void CreateXZSymetric(CSurface const &Surface);
	void CreateXPoints();

	bool IsCenterSurf() {return m_bIsCenterSurf;}
	bool IsLeftSurf()   {return m_bIsLeftSurf;}
	bool IsRightSurf()  {return m_bIsRightSurf;}
	bool IsTipLeft()    {return m_bIsTipLeft;}
	bool IsTipRight()   {return m_bIsTipRight;}
	bool IsInSymPlane() {return m_bIsInSymPlane;}

	bool IsFlapPanel(int const &p);
	bool IsFlapNode(int const &nNode);
	bool RotateFlap(double const &Angle, bool bBCOnly=false);
//	bool RotateFlap(double const &Angle, CPanel *pPanel, CVector *pNode);
	double GetTwist(int const &k);
	double GetChord(int const &k);
	double GetChord(double const &tau);
	double GetOffset(double const &tau);
	double GetStripSpanPos(int const &k);
	double GetFoilArea(double const &tau);
	double GetPanelWidth(int const &k);


private :
	CVector SideA[MAXCHORDPANELS];	 
	CVector SideB[MAXCHORDPANELS];	 
	CVector SideA_T[MAXCHORDPANELS];	 
	CVector SideB_T[MAXCHORDPANELS];	 
	CVector SideA_B[MAXCHORDPANELS];	 
	CVector SideB_B[MAXCHORDPANELS];	 
	static CVector VTemp;
	static CPanel *s_pPanel;
	static CVector *s_pNode;

	bool m_bIsInSymPlane;
	bool m_bTEFlap;
	bool m_bIsLeftSurf, m_bIsRightSurf;
	bool m_bIsTipLeft, m_bIsTipRight;
	bool m_bIsCenterSurf;//true if is either left or right center surface... need to connect to body

	CVector m_LA, m_LB, m_TA, m_TB;
	CVector Normal, NormalA, NormalB;
	CVector V1, V2, DL, DC, u, v;
	CVector NA, NB;

	double m_TwistA, m_TwistB;
	double m_Length;// along span-y direction...
	double y1, y2, xLA, xTA, xLB, xTB;
	double chordA, chordB, zA, zB, Chord, Length;
	double m_posATE, m_posBTE;//flap break positions at sides A and B
	double m_xPointA[MAXCHORDPANELS], m_xPointB[MAXCHORDPANELS];	//chorwise position of VLM panels


	int m_XDistType;
	int m_YDistType;
	int m_NXLead, m_NXFlap;//number of panels before and after the break
	int m_NElements;

	int m_nFlapNodes, m_nFlapPanels;
	int m_FlapNode[VLMHALF];
	int m_FlapPanel[VLMHALF];

	CVector m_HingePoint;
	CVector m_HingeVector;

	CSurface *m_pLeftSurface, *m_pRightSurface;

public:
	bool m_bJoinRight; //true if the surface's right side should be connected to the next right surface's right left side - for panel analysis only
	static CVector LA, LB, TA, TB;//leading and trailing corners of strip k
	int m_NYPanels;
	int m_NXPanels;	
	CFoil *m_pFoilA, *m_pFoilB; //Left and right foils
	
};
#endif
