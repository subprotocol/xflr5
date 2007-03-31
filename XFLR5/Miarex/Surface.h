/****************************************************************************

    CSurface Class
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

//////////////////////////////////////////////////////////////////////
//
// Surface.h: interface for the CSurface class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "../XDirect/Foil.h"

class CSurface : public CObject  
{
	friend class CWing;
	friend class CMiarex;
	friend class CVLMDlg;
	friend class C3DPanelDlg;

public:
	int GetNPanels();
	void Copy(CSurface &surface);
	void GetLeadingPt(CVector &C, int k);
	void GetTrailingPt(CVector &C, int k);
	void GetPanel(int k, int l, CVector &dF, int pos=0);
	void GetPoint(double xArel, double xBrel, double yrel, CVector &Point, int pos=0);
	void GetxDist(int l, double &xA1, double &xA2, double &xB1, double &xB2);
	void GetyDist(int k, double &y1, double &y2);
	void GetNormal(CVector &N);
	void Translate(CVector T);
	void RotateX(CVector O, double XTilt);
	void RotateY(CVector O, double YTilt);
	void RotateZ(CVector O, double ZTilt);
	void SetTwist();
	int Getk(double y);
	void Getyz(int k, double &y, double &z);
//	double Gety(int k);
	double Getyrel(double y);
	double GetTwist(int k);
	double GetStripArea(int k);
	double Getdl(int k);
	double GetChord(int k);
	double GetChord(double tau);
	double GetOffset(double tau);
	void GetC4(int k, CVector &Pt);
	void SetFlap();

	CSurface();
	virtual ~CSurface();

private :
	
	static CVector LA, LB, TA, TB;//leading and trailing corners of strip k
	static CVector VTemp;

	bool m_bTEFlap;

	CVector m_LA, m_LB, m_TA, m_TB;
	double m_TwistA, m_TwistB;
	double m_Dihedral;
	double m_Length;// along span-y direction...

	int m_XDistType;
	int m_YDistType;
	int m_NXPanels;
	int m_NX1, m_NX2;//number of panels before and after the break
	int m_NYPanels;
	int m_iPos; //=0 if is positioned on middle surface, -1 for lower surf, 1 for upper surf
	double m_posATE, m_posBTE;//flap break positions at sides A and B
	double pi;
//	COLORREF m_Color;

	CFoil *m_pFoilA, *m_pFoilB;

};

