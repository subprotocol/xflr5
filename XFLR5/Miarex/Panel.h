/****************************************************************************

    CPanel Class
	Copyright (C) 2006 André Deperrois xflr5@yahoo.com

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

// Panel.h: interface for the CPanel class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include "../misc/Vector.h"

class CPanel : public CObject  
{
	friend class CWing;
	friend class CMiarex;
	friend class CXFLR6;
	friend class C3DPanelDlg;
	friend class CVLMDlg;
	friend class CBody;

public:
	CPanel();
	virtual ~CPanel();

protected:
	void Reset();
	void SetFrame(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB);
	bool Intersect(CVector const &A, CVector const &U, CVector &I, double &dist);
	bool Invert33(double *l);
	CVector GlobalToLocal(CVector const &V);
	CVector LocalToGlobal(CVector const &V);

	bool m_bIsLeading;
	bool m_bIsTrailing;
	bool m_bIsInSymPlane;
	bool m_bIsWakePanel;
//	bool m_bIsThinSurface;

	int m_iElement; // panel identification number ; used when panel array is re-arranged in non sequential order
	int m_iLA, m_iLB, m_iTA, m_iTB;//Corner Node numbers
	int m_iPL, m_iPR, m_iPU, m_iPD;//Panels left, rigth; upstream, downstream
	int m_iPos;//-1 is bottom surface, 0 is middle, 1 is top;
	int m_iWake;//-1 if not followed by a wake panel, else equal to wake panel number
	int m_iWakeColumn;

	//Local frame of refernce
	CVector Normal;
	CVector VortexPos;
	CVector Vortex;
	CVector CtrlPt;
	CVector CollPt;
	CVector P1, P2, P3, P4;//point's local coordinates
	CVector A, B;
	CVector m, l;

	double dl;
	double Area;
	double Size, SMP, SMQ;
	double lij[9];

	static CVector *s_pNode;
	static CVector smp, smq, MidA, MidB;
	static CVector LA, LB, TA, TB, T, V, W, P;

	static double m_VortexPos;//between 0 and 1
	static double m_CtrlPos;//between 0 and 1
	static double det;
	static double mat[9];

public:
};

