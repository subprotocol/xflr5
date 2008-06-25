/****************************************************************************

    CPanel Class
	Copyright (C) 2006-2008 André Deperrois xflr5@yahoo.com

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

// Panel.cpp: implementation of the CPanel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../X-FLR5.h"
#include ".\panel.h"
#include <math.h>

double CPanel::m_VortexPos = 0.25;
double CPanel::m_CtrlPos   = 0.75;
double CPanel::mat[9];
double CPanel::det;
CVector CPanel::smp, CPanel::smq, CPanel::MidA, CPanel::MidB;
CVector *CPanel::s_pNode;
CVector CPanel::ILA, CPanel::ILB, CPanel::ITA, CPanel::ITB, CPanel::T, CPanel::V, CPanel::W;
CVector CPanel::P;
CVector CPanel::LATB, CPanel::TALB;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPanel::CPanel()
{
	Reset();
}

void CPanel::Reset()
{
	dl     = 0.0;
	Size   = 0.0;
	SMP    = 0.0;
	SMQ    = 0.0;
	Area   = 0.0;

	m_bIsLeading     = false;
	m_bIsTrailing    = false;
	m_bIsInSymPlane  = false;
	m_bIsWakePanel   = false;

	m_iElement    = -1;
	m_iPos        =  0;
	m_iLA         =  0;
	m_iLB         =  0;
	m_iTA         =  0;
	m_iTB         =  0;
	m_iPL         = -1;
	m_iPR         = -1;
	m_iPU         = -1;
	m_iPD         = -1;
	m_iWake       = -1;
	m_iWakeColumn = -1;

	memset(lij, 0, sizeof(lij));

}

CPanel::~CPanel()
{

}


void CPanel::SetFrame(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB)
{
	LATB.x = TB.x - LA.x;
	LATB.y = TB.y - LA.y;
	LATB.z = TB.z - LA.z;
	TALB.x = LB.x - TA.x;
	TALB.y = LB.y - TA.y;
	TALB.z = LB.z - TA.z;

	Normal = LATB * TALB;
	Area = Normal.VAbs()/2.0;
	Normal.Normalize();

	A.x = LA.x*(1.0-m_VortexPos)+TA.x*m_VortexPos;
	A.y = LA.y*(1.0-m_VortexPos)+TA.y*m_VortexPos;
	A.z = LA.z*(1.0-m_VortexPos)+TA.z*m_VortexPos;

	B.x = LB.x*(1.0-m_VortexPos)+TB.x*m_VortexPos;
	B.y = LB.y*(1.0-m_VortexPos)+TB.y*m_VortexPos;
	B.z = LB.z*(1.0-m_VortexPos)+TB.z*m_VortexPos;

	Vortex.x = B.x - A.x;
	Vortex.y = B.y - A.y;
	Vortex.z = B.z - A.z;

	dl = Vortex.VAbs();

	VortexPos.x = (A.x+B.x)/2.0;
	VortexPos.y = (A.y+B.y)/2.0;
	VortexPos.z = (A.z+B.z)/2.0;

	if(abs(LA.y)<1.e-5 && abs(TA.y)<1.e-5 && abs(LB.y)<1.e-5 && abs(TB.y)<1.e-5) m_bIsInSymPlane = true;
	else m_bIsInSymPlane = false;

	MidA.x = LA.x*(1.0-m_CtrlPos)+TA.x*m_CtrlPos;
	MidA.y = LA.y*(1.0-m_CtrlPos)+TA.y*m_CtrlPos;
	MidA.z = LA.z*(1.0-m_CtrlPos)+TA.z*m_CtrlPos;

	MidB.x = LB.x*(1.0-m_CtrlPos)+TB.x*m_CtrlPos;
	MidB.y = LB.y*(1.0-m_CtrlPos)+TB.y*m_CtrlPos;
	MidB.z = LB.z*(1.0-m_CtrlPos)+TB.z*m_CtrlPos;

	CtrlPt.x = (MidA.x+MidB.x)/2.0;
	CtrlPt.y = (MidA.y+MidB.y)/2.0;
	CtrlPt.z = (MidA.z+MidB.z)/2.0;

	CollPt.x = (LA.x + LB.x + TA.x + TB.x)/4.0;
	CollPt.y = (LA.y + LB.y + TA.y + TB.y)/4.0;
	CollPt.z = (LA.z + LB.z + TA.z + TB.z)/4.0;

	//Use VSAERO figure 8. p23
//	if(m_iPos==0 || m_iPos==1 || m_iPos==100)
//	{
		m.x = (LB.x + TB.x) *0.5 - CollPt.x;
		m.y = (LB.y + TB.y) *0.5 - CollPt.y;
		m.z = (LB.z + TB.z) *0.5 - CollPt.z;
/*	}
	else						
	{
		m.x = (LA.x + TA.x) *0.5 - CollPt.x;
		m.y = (LA.y + TA.y) *0.5 - CollPt.y;
		m.z = (LA.z + TA.z) *0.5 - CollPt.z;
	}*/
	m.Normalize();

	l.x =  m.y * Normal.z - m.z * Normal.y;
	l.y = -m.x * Normal.z + m.z * Normal.x;
	l.z =  m.x * Normal.y - m.y * Normal.x;

	smq.x  = (LB.x + TB.x) * 0.5 - CollPt.x;
	smq.y  = (LB.y + TB.y) * 0.5 - CollPt.y;
	smq.z  = (LB.z + TB.z) * 0.5 - CollPt.z;
	smp.x  = (TB.x + TA.x) * 0.5 - CollPt.x;
	smp.y  = (TB.y + TA.y) * 0.5 - CollPt.y;
	smp.z  = (TB.z + TA.z) * 0.5 - CollPt.z;

	SMP = smp.VAbs();
	SMQ = smq.VAbs();

	Size = SMP + SMQ;

	//create the transformation matrix
	lij[0]=l.x;
	lij[1]=m.x;
	lij[2]=Normal.x;
	lij[3]=l.y;
	lij[4]=m.y;
	lij[5]=Normal.y;
	lij[6]=l.z;
	lij[7]=m.z;
	lij[8]=Normal.z;
	Invert33(lij);
	if(m_iPos>0)
	{
		P1.x = lij[0]*(LA.x-CollPt.x) + lij[1]*(LA.y-CollPt.y) + lij[2]*(LA.z-CollPt.z);
		P1.y = lij[3]*(LA.x-CollPt.x) + lij[4]*(LA.y-CollPt.y) + lij[5]*(LA.z-CollPt.z);
		P1.z = lij[6]*(LA.x-CollPt.x) + lij[7]*(LA.y-CollPt.y) + lij[8]*(LA.z-CollPt.z);
		P2.x = lij[0]*(LB.x-CollPt.x) + lij[1]*(LB.y-CollPt.y) + lij[2]*(LB.z-CollPt.z);
		P2.y = lij[3]*(LB.x-CollPt.x) + lij[4]*(LB.y-CollPt.y) + lij[5]*(LB.z-CollPt.z);
		P2.z = lij[6]*(LB.x-CollPt.x) + lij[7]*(LB.y-CollPt.y) + lij[8]*(LB.z-CollPt.z);
		P3.x = lij[0]*(TB.x-CollPt.x) + lij[1]*(TB.y-CollPt.y) + lij[2]*(TB.z-CollPt.z);
		P3.y = lij[3]*(TB.x-CollPt.x) + lij[4]*(TB.y-CollPt.y) + lij[5]*(TB.z-CollPt.z);
		P3.z = lij[6]*(TB.x-CollPt.x) + lij[7]*(TB.y-CollPt.y) + lij[8]*(TB.z-CollPt.z);
		P4.x = lij[0]*(TA.x-CollPt.x) + lij[1]*(TA.y-CollPt.y) + lij[2]*(TA.z-CollPt.z);
		P4.y = lij[3]*(TA.x-CollPt.x) + lij[4]*(TA.y-CollPt.y) + lij[5]*(TA.z-CollPt.z);
		P4.z = lij[6]*(TA.x-CollPt.x) + lij[7]*(TA.y-CollPt.y) + lij[8]*(TA.z-CollPt.z);
	}
	else
	{
		P1.x = lij[0]*(LB.x-CollPt.x) + lij[1]*(LB.y-CollPt.y) + lij[2]*(LB.z-CollPt.z);
		P1.y = lij[3]*(LB.x-CollPt.x) + lij[4]*(LB.y-CollPt.y) + lij[5]*(LB.z-CollPt.z);
		P1.z = lij[6]*(LB.x-CollPt.x) + lij[7]*(LB.y-CollPt.y) + lij[8]*(LB.z-CollPt.z);
		P2.x = lij[0]*(LA.x-CollPt.x) + lij[1]*(LA.y-CollPt.y) + lij[2]*(LA.z-CollPt.z);
		P2.y = lij[3]*(LA.x-CollPt.x) + lij[4]*(LA.y-CollPt.y) + lij[5]*(LA.z-CollPt.z);
		P2.z = lij[6]*(LA.x-CollPt.x) + lij[7]*(LA.y-CollPt.y) + lij[8]*(LA.z-CollPt.z);
		P3.x = lij[0]*(TA.x-CollPt.x) + lij[1]*(TA.y-CollPt.y) + lij[2]*(TA.z-CollPt.z);
		P3.y = lij[3]*(TA.x-CollPt.x) + lij[4]*(TA.y-CollPt.y) + lij[5]*(TA.z-CollPt.z);
		P3.z = lij[6]*(TA.x-CollPt.x) + lij[7]*(TA.y-CollPt.y) + lij[8]*(TA.z-CollPt.z);
		P4.x = lij[0]*(TB.x-CollPt.x) + lij[1]*(TB.y-CollPt.y) + lij[2]*(TB.z-CollPt.z);
		P4.y = lij[3]*(TB.x-CollPt.x) + lij[4]*(TB.y-CollPt.y) + lij[5]*(TB.z-CollPt.z);
		P4.z = lij[6]*(TB.x-CollPt.x) + lij[7]*(TB.y-CollPt.y) + lij[8]*(TB.z-CollPt.z);
	}
}



bool CPanel::Invert33(double *l)
{
	memcpy(mat,l,sizeof(mat));
/*		a0 b1 c2
        d3 e4 f5
		g6 h7 i8

	          1                     (ei-fh)   (bi-ch)   (bf-ce)
-----------------------------   x   (fg-di)   (ai-cg)   (cd-af)
a(ei-fh) - b(di-fg) + c(dh-eg)      (dh-eg)   (bg-ah)   (ae-bd)*/

	det  = mat[0] *(mat[4] * mat[8] - mat[5]* mat[7]);
	det -= mat[1] *(mat[3] * mat[8] - mat[5]* mat[6]);
	det += mat[2] *(mat[3] * mat[7] - mat[4]* mat[6]);
	if(det==0.0) return false;
	
	* l     = (mat[4] * mat[8] - mat[5] * mat[7])/det;
	*(l+1)  = (mat[2] * mat[7] - mat[1] * mat[8])/det;
	*(l+2)  = (mat[1] * mat[5] - mat[2] * mat[4])/det;

	*(l+3)  = (mat[5] * mat[6] - mat[3] * mat[8])/det;
	*(l+4)  = (mat[0] * mat[8] - mat[2] * mat[6])/det;
	*(l+5)  = (mat[2] * mat[3] - mat[0] * mat[5])/det;

	*(l+6)  = (mat[3] * mat[7] - mat[4] * mat[6])/det;
	*(l+7)  = (mat[1] * mat[6] - mat[0] * mat[7])/det;
	*(l+8)  = (mat[0] * mat[4] - mat[1] * mat[3])/det;

	return true;
}

CVector CPanel::GlobalToLocal(CVector const &V)
{
	CVector L;
	L.x = lij[0]*V.x +lij[1]*V.y +lij[2]*V.z;
	L.y = lij[3]*V.x +lij[4]*V.y +lij[5]*V.z;
	L.z = lij[6]*V.x +lij[7]*V.y +lij[8]*V.z;
	return L;
}

CVector CPanel::LocalToGlobal(CVector const &V)
{
	CVector L;
	L.x = V.x * l.x + V.y * m.x + V.z * Normal.x;
	L.y = V.x * l.y + V.y * m.y + V.z * Normal.y;
	L.z = V.x * l.z + V.y * m.z + V.z * Normal.z;
	return L;
}



bool CPanel::Intersect(CVector const &A, CVector const &U, CVector &I, double &dist)
{
	bool b1, b2, b3, b4;
	double r,s;

	ILA.Copy(s_pNode[m_iLA]);
	ITA.Copy(s_pNode[m_iTA]);
	ILB.Copy(s_pNode[m_iLB]);
	ITB.Copy(s_pNode[m_iTB]);
		
	r = (CollPt.x-A.x)*Normal.x + (CollPt.y-A.y)*Normal.y + (CollPt.z-A.z)*Normal.z ;
	s = U.x*Normal.x + U.y*Normal.y + U.z*Normal.z;

	dist = 10000.0;

	if(abs(s)>0.0)
	{
		dist = r/s;

		//inline operations to save time
		P.x = A.x + U.x * dist;
		P.y = A.y + U.y * dist;
		P.z = A.z + U.z * dist;

		// P is inside the panel if on left side of each panel side
		W.x = P.x  - ITA.x;
		W.y = P.y  - ITA.y;
		W.z = P.z  - ITA.z;
		V.x = ITB.x - ITA.x;
		V.y = ITB.y - ITA.y;
		V.z = ITB.z - ITA.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		if(T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0) b1 = true; else b1 = false;

		W.x = P.x  - ITB.x;
		W.y = P.y  - ITB.y;
		W.z = P.z  - ITB.z;
		V.x = ILB.x - ITB.x;
		V.y = ILB.y - ITB.y;
		V.z = ILB.z - ITB.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		if(T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0) b2 = true; else b2 = false;

		W.x = P.x  - ILB.x;
		W.y = P.y  - ILB.y;
		W.z = P.z  - ILB.z;
		V.x = ILA.x - ILB.x;
		V.y = ILA.y - ILB.y;
		V.z = ILA.z - ILB.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		if(T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0) b3 = true; else b3 = false;

		W.x = P.x  - ILA.x;
		W.y = P.y  - ILA.y;
		W.z = P.z  - ILA.z;
		V.x = ITA.x - ILA.x;
		V.y = ITA.y - ILA.y;
		V.z = ITA.z - ILA.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		if(T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0) b4 = true; else b4 = false;
		
		if(b1 && b2 && b3 && b4) 
		{
			I.Set(P.x, P.y, P.z);
			return true;
		}
	}
	return false;
}

void CPanel::Rotate(CVector const &HA, CVector const &H, double const &Angle)
{
	// HA is a point on the rotating axis
	// H is a unit vector in the direction of the axis
	// Alpha is the rotation angle in degrees

}



void CPanel::Rotate(CVector const &HA, Quaternion &Qt, double const &angle)
{
	// HA is a point on the rotation axis
	// for a VLM analysis, we need to rotate :
	//     - the control point
	//     - the vortice vector
	//     - the vortice's end points

	W.x = VortexPos.x - HA.x;
	W.y = VortexPos.y - HA.y;
	W.z = VortexPos.z - HA.z;
	Qt.Conjugate(W);
	VortexPos.x = W.x + HA.x;
	VortexPos.y = W.y + HA.y;
	VortexPos.z = W.z + HA.z;

	W.x = CtrlPt.x - HA.x;
	W.y = CtrlPt.y - HA.y;
	W.z = CtrlPt.z - HA.z;
	Qt.Conjugate(W);
	CtrlPt.x = W.x + HA.x;
	CtrlPt.y = W.y + HA.y;
	CtrlPt.z = W.z + HA.z;

	Qt.Conjugate(Vortex);

	Normal.RotateY(angle);

	//TODO ; remove ? 
	// What's the point of A and B anyway?
	// are they used anywhere

	W.x = A.x - HA.x;
	W.y = A.y - HA.y;
	W.z = A.z - HA.z;
	Qt.Conjugate(W);
	A.x = W.x + HA.x;
	A.y = W.y + HA.y;
	A.z = W.z + HA.z;


	W.x = B.x - HA.x;
	W.y = B.y - HA.y;
	W.z = B.z - HA.z;
	Qt.Conjugate(W);
	B.x = W.x + HA.x;
	B.y = W.y + HA.y;
	B.z = W.z + HA.z;


}

























