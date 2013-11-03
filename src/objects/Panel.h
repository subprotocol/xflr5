/****************************************************************************

    CPanel Class
	Copyright (C) 2006 Andre Deperrois adeperrois@xflr5.com

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



#ifndef PANEL_H
#define PANEL_H


#include "Quaternion.h"
#include "CVector.h"

typedef enum {BOTSURFACE, MIDSURFACE, TOPSURFACE, SIDESURFACE, BODYSURFACE} enumPanelPosition;

class Panel
{
	friend class Surface;
	friend class Wing;
	friend class QMiarex;
	friend class Body;
	friend class PanelAnalysisDlg;
	friend class GL3dBodyDlg;
	friend class GL3dWingDlg;

public:
	Panel();

	void RotatePanel(CVector const &O, Quaternion & Qt);
	void RotateBC(CVector const &HA, Quaternion & Qt);
	void Reset();
	void SetFrame();
	void SetFrame(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB);
	bool Intersect(CVector const &A, CVector const &U, CVector &I, double &dist);
	bool Invert33(double *l);
	CVector GlobalToLocal(CVector const &V);
	CVector GlobalToLocal(double const &Vx, double const &Vy, double const &Vz);
	CVector LocalToGlobal(CVector const &V);

	double Width();
	double GetArea();

protected:	
	bool m_bIsInSymPlane;
	bool m_bIsLeftPanel;
	bool m_bIsWakePanel;

	int m_iElement; // panel identification number ; used when panel array is re-arranged in non sequential order
	int m_iSym; //reference of the symetric panel, or -1 if none
	int m_iPL, m_iPR, m_iPU, m_iPD;//Panels left, rigth; upstream, downstream
	int m_iWake;//-1 if not followed by a wake panel, else equal to wake panel number
	int m_iWakeColumn;

	//Local frame of refernce
	CVector VortexPos; // the vortex mid position
	CVector Vortex; // the vortex vector
	CVector P1, P2, P3, P4;//point's local coordinates
	CVector m, l;

	double dl;
	double Area;
	double Size, SMP, SMQ;
	double lij[9];

	static CVector *s_pNode;
	static CVector smp, smq, MidA, MidB;
	static CVector ILA, ILB, ITA, ITB, T, V, W, P, LATB, TALB;

	static double s_VortexPos;//between 0 and 1; usually the vortex is positioned at the panel's quarter chord : s_VortexPos=0.25
	static double s_CtrlPos;//between 0 and 1; usually the control point is positioned at the panel's 3/4 chord : s_VortexPos=0.75
	static double det;
	static double mat[9];

public:
	enumPanelPosition m_Pos; // defines if the panel is positioned on a top, middle, bottom, side or body surface
	bool m_bIsLeading, m_bIsTrailing;
	int m_iLA, m_iLB, m_iTA, m_iTB;//Corner Node numbers
	CVector Normal; // the unit vector normal to the panel
	CVector CtrlPt; // the control point for VLM analysis or 3D/Thin panels
	CVector CollPt; // the collocation point for 3d panel analysis
	CVector VA, VB; //the left and right end points of the vortex on this panel
};

#endif
