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

// Panel.cpp: implementation of the CPanel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../X-FLR5.h"
#include ".\panel.h"
#include <math.h>

double CPanel::m_VortexPos = 0.25;
double CPanel::m_CtrlPos   = 0.75;


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

	m_bIsLeading   = false;
	m_bIsTrailing  = false;
	m_bIsInSymPlane = false;
	m_bIsWakePanel = false;

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


void CPanel::SetFrame(CVector LA, CVector LB, CVector TA, CVector TB)
{
	//assumes the normal vector has already been set

	A = LA*(1.0-m_VortexPos)+TA*m_VortexPos;
	B = LB*(1.0-m_VortexPos)+TB*m_VortexPos;

	Vortex = B - A;

	dl = Vortex.VAbs();

	VortexPos = (A+B)/2.0;

	if(abs(LA.y)<1.e-5 && abs(TA.y)<1.e-5 && abs(LB.y)<1.e-5 && abs(TB.y)<1.e-5) m_bIsInSymPlane = true;
	else m_bIsInSymPlane = false;

	CVector smp, smq, MidA, MidB;
	MidA = LA*(1.0-m_CtrlPos)+TA*m_CtrlPos;
	MidB = LB*(1.0-m_CtrlPos)+TB*m_CtrlPos;

	CtrlPt = (MidA+MidB)/2.0;

	CollPt = (LA + LB + TA + TB)/4.0;

	//Use VSAERO figure 8. p23
	if(m_iPos==0 || m_iPos==1)	m = (LB + TB) *0.5 - CollPt;
	else						m = (LA + TA) *0.5 - CollPt;
	m.Normalize();

	l = m * Normal;

	smq  = (LB + TB) * 0.5 - CollPt;
	smp  = (TB + TA) * 0.5 - CollPt;

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
	if(m_iPos>0){
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
	else{
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
	double det;
	double m[9];
	memcpy(m,l,sizeof(m));
/*		a0 b1 c2
        d3 e4 f5
		g6 h7 i8

	          1                     (ei-fh)   (bi-ch)   (bf-ce)
-----------------------------   x   (fg-di)   (ai-cg)   (cd-af)
a(ei-fh) - b(di-fg) + c(dh-eg)      (dh-eg)   (bg-ah)   (ae-bd)*/

	det  = m[0] *(m[4] * m[8] - m[5]* m[7]);
	det -= m[1] *(m[3] * m[8] - m[5]* m[6]);
	det += m[2] *(m[3] * m[7] - m[4]* m[6]);
	if(det==0.0) return false;
	
	* l     = (m[4] * m[8] - m[5] * m[7])/det;
	*(l+1)  = (m[2] * m[7] - m[1] * m[8])/det;
	*(l+2)  = (m[1] * m[5] - m[2] * m[4])/det;

	*(l+3)  = (m[5] * m[6] - m[3] * m[8])/det;
	*(l+4)  = (m[0] * m[8] - m[2] * m[6])/det;
	*(l+5)  = (m[2] * m[3] - m[0] * m[5])/det;

	*(l+6)  = (m[3] * m[7] - m[4] * m[6])/det;
	*(l+7)  = (m[1] * m[6] - m[0] * m[7])/det;
	*(l+8)  = (m[0] * m[4] - m[1] * m[3])/det;

	return true;
}

CVector CPanel::GlobalToLocal(CVector V)
{
	CVector L;
	L.x = lij[0]*V.x +lij[1]*V.y +lij[2]*V.z;
	L.y = lij[3]*V.x +lij[4]*V.y +lij[5]*V.z;
	L.z = lij[6]*V.x +lij[7]*V.y +lij[8]*V.z;
	return L;
}

CVector CPanel::LocalToGlobal(CVector V)
{
	CVector L;
	L.x = V.x * l.x + V.y * m.x + V.z * Normal.x;
	L.y = V.x * l.y + V.y * m.y + V.z * Normal.y;
	L.z = V.x * l.z + V.y * m.z + V.z * Normal.z;
	return L;
}










