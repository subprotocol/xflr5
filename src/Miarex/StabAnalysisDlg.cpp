/****************************************************************************

	StabAnalysisDlg Class
        Copyright (C) 2010 Andre Deperrois XFLR5@yahoo.com

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

#include <QApplication>
#include <QDir>
#include <QDateTime>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QMessageBox>
#include <math.h>
#include "StabAnalysisDlg.h"
#include "../MainFrame.h"
#include "../Globals.h"
#include "../Objects/CVector.h"
#include "Miarex.h"
#include <QtDebug>

void *StabAnalysisDlg::s_pMiarex;
void *StabAnalysisDlg::s_pMainFrame;


StabAnalysisDlg::StabAnalysisDlg()
{
	setWindowTitle(tr("Stability Analysis"));
	SetupLayout();

	m_bSequence      = false;
	m_bWarning       = false;
	m_bXFile         = false;
	m_bConverged     = false;
	m_bPointOut      = false;
	m_bCancel        = false;
	m_bTrefftz       = false;
	m_bIsFinished    = false;
	m_bTrace         = true;

	m_MatSize = m_nNodes = m_NSurfaces = 0;

	m_Ctrl = 0.0;
	Theta0 = 0.0;
	u0     = 0.0;
	m_Mass = 0.0;
	Xu = Xw = Zu = Zw = Zq = Mu = Mw = Mq = Zwp = Mwp = 0.0;
	Yv = Yp = Yr = Lv = Lp = Lr = Nv = Np = Nr = 0.0;
	m_ControlMin  = m_ControlMax = m_ControlDelta  = 0.0;
	m_XCP = m_YCP = m_VCm = m_GCm = m_GCm = m_IYm = m_GYm = m_CL  = 0.0;
	m_ViscousDrag = m_InducedDrag = 0.0;
	memset(m_ALong, 0, 16*sizeof(double));
	memset(m_ALat,  0, 16*sizeof(double));
	memset(m_R,     0,  9*sizeof(double));
	memset(m_Is,    0,  9*sizeof(double));
	memset(m_Ib,    0,  9*sizeof(double));

	m_strOut = "";

	m_pWing   = m_pWing2 = m_pStab = m_pFin = NULL;
	m_pWPolar = NULL;
	m_pPlane  = NULL;
	m_ppSurface = NULL;

	m_pPanel        = NULL;
	m_pMemPanel     = NULL;
	m_pNode         = NULL;
	m_pMemNode      = NULL;

	memset(m_Cp,0,sizeof(m_Cp));
	memset(m_uRHS, 0, sizeof(m_uRHS));
	memset(m_vRHS, 0, sizeof(m_vRHS));
	memset(m_wRHS, 0, sizeof(m_wRHS));
	memset(m_pRHS, 0, sizeof(m_pRHS));
	memset(m_qRHS, 0, sizeof(m_qRHS));
	memset(m_rRHS, 0, sizeof(m_rRHS));
}




void StabAnalysisDlg::AddString(QString strong)
{
	m_pctrlTextOutput->insertPlainText(strong);
	m_pctrlTextOutput->ensureCursorVisible();
	WriteString(strong);
}



void StabAnalysisDlg::BuildRHS()
{
	//Builds the RHS for unit translation vectors in geometric axis
	//This is to get the initial trimmed conditions

	int m;
	
	//______________________________________________________________________________
	// RHS for unit speed vectors
	for (m=0; m<m_MatSize; m++)
	{
		m_uRHS[m] = - m_pPanel[m].Normal.x;
		m_wRHS[m] = - m_pPanel[m].Normal.z;
	}
}


void StabAnalysisDlg::BuildControlRHS(double const & DeltaAngle)
{
	//______________________________________________________________________________
	// Constructs RHS for unit control rotations
	//
	// At this stage, the control surfaces have already been set at the specified positions;
	// We have an equilibrium position defined by AlphaEq
	// We just rotate the normals, we do not modify the geometry
	//
	static Quaternion Quat;
	static CVector is, js, ks;
	static CVector H, RN, VInf;
	static double sina, cosa;
	static int p, j, nCtrl, pos;

	// Define the stability axes
	cosa = cos(m_AlphaEq*PI/180);
	sina = sin(m_AlphaEq*PI/180);
	is.Set(-cosa, 0.0, -sina);
	js.Set(  0.0, 1.0,   0.0);
	ks.Set( sina, 0.0, -cosa);
	
	VInf.Set(u0*cos(m_AlphaEq*PI/180.0), 0.0, u0*sin(m_AlphaEq*PI/180));
	nCtrl = 0;

	for(p=0; p<m_MatSize;p++)
	{
		m_uRHS[p] = -VInf.dot(m_pPanel[p].Normal);
	}

	pos = 0;
	
	if(m_pPlane)
	{
		H.Set(0.0, 1.0, 0.0);
		//wing tilt
		memcpy(m_cRHS+nCtrl*m_MatSize, m_uRHS, m_MatSize*sizeof(double));

		Quat.Set(DeltaAngle, H);
		for(j=0; j<m_pWing->m_MatSize; j++)
		{
			Quat.Conjugate(m_pWing->m_pPanel[j].Normal, RN);
			m_cRHS[nCtrl*m_MatSize+j] = -VInf.dot(RN);
		}
		pos = m_pWing->m_MatSize;
		
		nCtrl=1;

		if(m_pStab)
		{
			//Elevator tilt
			memcpy(m_cRHS+nCtrl*m_MatSize, m_uRHS, m_MatSize*sizeof(double));
			Quat.Set(DeltaAngle, H);
	
			for(p=0; p<m_pStab->m_MatSize; p++)
			{
				Quat.Conjugate(m_pStab->m_pPanel[p].Normal, RN);
				m_cRHS[nCtrl*m_MatSize+pos+p] = -VInf.dot(RN);
			}
			nCtrl++;
		}
	}

	// flap controls
	for (j=0; j<m_NSurfaces; j++)
	{
		if(m_ppSurface[j]->m_bTEFlap)
		{
			memcpy(m_cRHS+nCtrl*m_MatSize, m_uRHS, m_MatSize*sizeof(double));
			H.Set(m_ppSurface[j]->m_HingeVector);

			Quat.Set(DeltaAngle, m_ppSurface[j]->m_HingeVector);

			for(p=0;p<m_MatSize; p++)
			{
				if(m_ppSurface[j]->IsFlapPanel(p))
				{
					Quat.Conjugate(m_pPanel[p].Normal, RN);
					m_cRHS[nCtrl*m_MatSize+p] = -VInf.dot(RN);
				}
//				else m_cRHS[nCtrl*m_MatSize+p] = VInf.dot(m_pPanel[p].Normal);
			}
			nCtrl++;
		}
	}
}



bool StabAnalysisDlg::BuildInfluenceMatrix()
{
	// Builds the influence matrix a[p,pp]
	// which give the influence of vortex pp on the panel p
	//
	static int p, pp;
	static CVector V;

	AddString("      Creating the influence matrix...\n");

	for(p=0; p<m_MatSize; p++)
	{
		qApp->processEvents();

		if(m_bCancel) break;

		for(pp=0; pp<m_MatSize; pp++)
		{
			if(m_bCancel) break;
			VLMGetVortexInfluence(m_pPanel+pp, m_pPanel[p].CtrlPt, V, true);
			m_aij[p*m_MatSize+pp] = V.dot(m_pPanel[p].Normal);
		}
	}

	return true;
}



void StabAnalysisDlg::BuildStateMatrices()
{
	// Creates the longitudinal and lateral state matrices
	// from the derivatives and inertias calculated previously
	//
	// Creates the control state matrix from the control derivatives
	//
	static int i;
	static double Ipxx, Ipzz, Ipzx;
	static double Ixx,Iyy,Izz, Izx;
	QString strange;

	//use inertia measured in stability axis, CoG origin
	Ixx = m_Is[0][0];
	Iyy = m_Is[1][1];
	Izz = m_Is[2][2];
	Izx = m_Is[0][2];

	//____________________Longitudinal stability_____________
	m_ALong[0][0] = Xu/m_Mass;
	m_ALong[0][1] = Xw/m_Mass;
	m_ALong[0][2] = 0.0;
	m_ALong[0][3] = -9.81*cos(Theta0*PI/180.0);

	m_ALong[1][0] =  Zu                              /(m_Mass-Zwp);
	m_ALong[1][1] =  Zw                              /(m_Mass-Zwp);
	m_ALong[1][2] = (Zq+m_Mass*u0)                   /(m_Mass-Zwp);
	m_ALong[1][3] = -9.81*m_Mass*sin(Theta0*PI/180.0)/(m_Mass-Zwp);

	m_ALong[2][0] = (Mu + Mwp*Zu/(m_Mass-Zwp))                   /Iyy;
	m_ALong[2][1] = (Mw + Mwp*Zw/(m_Mass-Zwp))                   /Iyy;
	m_ALong[2][2] = (Mq + Mwp*(Zq+m_Mass*u0)/(m_Mass-Zwp))       /Iyy;
	m_ALong[2][3] = (Mwp*(-m_Mass*9.81*sin(Theta0))/(m_Mass-Zwp))/Iyy;

	m_ALong[3][0] = 0.0;
	m_ALong[3][1] = 0.0;
	m_ALong[3][2] = 1.0;
	m_ALong[3][3] = 0.0;


	strange = tr("   _____State matrices__________\n");
	AddString(strange);
	strange = tr("       Longitudinal state matrix\n");
	AddString(strange);
	for (i=0; i<4; i++)
	{
		strange = QString("        %1      %2      %3      %4\n")
					.arg(m_ALong[i][0], 14, 'g', 6)
					.arg(m_ALong[i][1], 14, 'g', 6)
					.arg(m_ALong[i][2], 14, 'g', 6)
					.arg(m_ALong[i][3], 14, 'g', 6);
		AddString(strange);
	}


	//____________________Lateral stability_____________
	Ipxx = (Ixx * Izz - Izx*Izx)/Izz;
	Ipzz = (Ixx * Izz - Izx*Izx)/Ixx;
	Ipzx =  Izx/(Ixx * Izz - Izx*Izx);

	m_ALat[0][0] = Yv/m_Mass;
	m_ALat[0][1] = Yp/m_Mass;
	m_ALat[0][2] = Yr/m_Mass - u0;
	m_ALat[0][3] = 9.81 * cos(Theta0*PI/180.0);

	m_ALat[1][0] = Lv/Ipxx+Ipzx*Nv;
	m_ALat[1][1] = Lp/Ipxx+Ipzx*Np;
	m_ALat[1][2] = Lr/Ipxx+Ipzx*Nr;
	m_ALat[1][3] = 0.0;

	m_ALat[2][0] = Lv*Ipzx+ Nv/Ipzz;
	m_ALat[2][1] = Lp*Ipzx+ Np/Ipzz;
	m_ALat[2][2] = Lr*Ipzx+ Nr/Ipzz;
	m_ALat[2][3] = 0.0;

	m_ALat[3][0] = 0.0;
	m_ALat[3][1] = 1.0;
	m_ALat[3][2] = tan(Theta0*PI/180.0);
	m_ALat[3][3] = 0.0;

	strange = tr("       Lateral state matrix\n");
	AddString(strange);
	for (i=0; i<4; i++)
	{
		strange = QString("        %1      %2      %3      %4\n")
					.arg(m_ALat[i][0], 14, 'g', 6)
					.arg(m_ALat[i][1], 14, 'g', 6)
					.arg(m_ALat[i][2], 14, 'g', 6)
					.arg(m_ALat[i][3], 14, 'g', 6);
		AddString(strange);
	}

	strange ="\n";
	AddString(strange);
	
	//build the control matrix
	for(i=0; i<m_pWPolar->m_nControls; i++)
	{
		m_BLong[i][0] = Xde[i]/m_Mass;
		m_BLong[i][1] = Zde[i]/m_Mass;
		m_BLong[i][2] = Mde[i]/Iyy;
		m_BLong[i][3] = 0.0;
//
		m_BLat[i][0] = Yde[i]/m_Mass;
		m_BLat[i][1] = Lde[i]/Ipxx+Nde[i]*Ipzx;
		m_BLat[i][2] = Lde[i]*Ipzx+Nde[i]/Ipzz;
		m_BLat[i][3] = 0.0;
	}
}


void StabAnalysisDlg::BuildRotationMatrix()
{
	// This is the rotation matrix from geometrical axis to stability axis,
	// i.e. if V is a vector defined in the body axis, its coordinates
	// in stability axis are      v = R.V
	// In XFLR5, the x-body-axis is pointing backward, and the convention
	// for stability axes is with the x-axis pointing forward.
	// The rotation matrix is set in accordance

	m_R[0][0] = -cos(m_AlphaEq*PI/180.0);
	m_R[1][0] =  0.0;
	m_R[2][0] =  sin(m_AlphaEq*PI/180.0);
	m_R[0][1] =  0.0;
	m_R[1][1] =  1.0;
	m_R[2][1] =  0.0;
	m_R[0][2] = -sin(m_AlphaEq*PI/180.0);
	m_R[1][2] =  0.0;
	m_R[2][2] = -cos(m_AlphaEq*PI/180.0);
}


void StabAnalysisDlg::ComputeBodyAxisInertia()
{
	// Initializes the inertia tensor in geometrical (body) axis :
	//  - add the volume inertia AND the point masses of all components
	//  - the body axis is the frame in which all the geometry has been defined
	//  - the origin=BodyFrame;
	//  - the center of gravity is calculated from component masses and is NOT the CoG defined in the polar

	int i, NWing;
	CVector LA, CoG;
	CWing *pWing[4];
	pWing[0] = pWing[1] = pWing[2] = pWing[3] = NULL;
	double Ixx, Iyy, Izz, Ixz, Mass;
	Ixx = Iyy = Izz = Ixz = Mass = 0.0;

	NWing = 1;
	pWing[0] = m_pWing;
	if(m_pPlane)
	{
		pWing[0] = &m_pPlane->m_Wing;
		if(m_pPlane->m_bBiplane)
		{
			pWing[NWing] = &m_pPlane->m_Wing2;
			NWing++;
		}
		if(m_pPlane->m_bStab)
		{
			pWing[NWing] = &m_pPlane->m_Stab;
			NWing++;
		}
		if(m_pPlane->m_bFin)
		{
			pWing[NWing] = &m_pPlane->m_Fin;
			NWing++;
		}
	}

	if(m_pPlane)
	{
		m_pPlane->ComputeVolumeInertia(Mass, m_CoG, Ixx, Iyy, Izz, Ixz);
		m_Mass = Mass;

	}
	else if(m_pWing)
	{
		m_pWing->ComputeVolumeInertia(m_pWing->m_Mass, m_CoG, Ixx, Iyy, Izz, Ixz);
		m_Mass = m_pWing->m_Mass;
	}
	CoG = m_CoG *m_Mass;
	// add point masses
	if(m_pPlane)
	{
		for(i=0; i<m_pPlane->m_NMass; i++)
		{
			m_Mass += m_pPlane->m_MassValue[i];
			CoG += m_pPlane->m_MassPosition[i] * m_pPlane->m_MassValue[i];
		}
	}

	for(i=0; i<m_pWing->m_NMass; i++)
	{
		m_Mass += m_pWing->m_MassValue[i];
		CoG += m_pWing->m_MassPosition[i] * m_pWing->m_MassValue[i];
	}

	if(m_pWing2)
	{
		for(i=0; i<m_pWing2->m_NMass; i++)
		{
			m_Mass += m_pWing2->m_MassValue[i];
			CoG += m_pWing2->m_MassPosition[i] * m_pWing2->m_MassValue[i];
		}
	}
	if(m_pStab)
	{
		for(i=0; i<m_pStab->m_NMass; i++)
		{
			m_Mass += m_pStab->m_MassValue[i];
			CoG += m_pStab->m_MassPosition[i] * m_pStab->m_MassValue[i];
		}
	}

	if(m_pFin)
	{
		for(i=0; i<m_pFin->m_NMass; i++)
		{
			m_Mass += m_pFin->m_MassValue[i];
			CoG += m_pFin->m_MassPosition[i] * m_pFin->m_MassValue[i];
		}
	}

	if(m_pPlane && m_pPlane->m_bBody)
	{
		CBody *pBody = m_pPlane->m_pBody;
		for(i=0; i<pBody->m_NMass; i++)
		{
			m_Mass += pBody->m_MassValue[i];
			CoG += pBody->m_MassPosition[i] * pBody->m_MassValue[i];
		}
	}
	m_CoG = CoG/m_Mass;

	// The CoG position is now available, so calculate the inertia w.r.t the CoG

	if(m_pPlane)
	{
		for(i=0; i<m_pPlane->m_NMass; i++)
		{
			LA = m_pPlane->m_MassPosition[i] - m_CoG;
			Ixx += m_pPlane->m_MassValue[i] * (LA.y*LA.y + LA.z*LA.z);
			Iyy += m_pPlane->m_MassValue[i] * (LA.x*LA.x + LA.z*LA.z);
			Izz += m_pPlane->m_MassValue[i] * (LA.x*LA.x + LA.y*LA.y);
			Ixz += m_pPlane->m_MassValue[i] * (LA.x*LA.z);
		}
	}

	for(i=0; i<m_pWing->m_NMass; i++)
	{
		if(m_pPlane) LA = m_pWing->m_MassPosition[i] + m_pPlane->m_LEWing - m_CoG;
		else         LA = m_pWing->m_MassPosition[i] - m_CoG;
		Ixx += m_pWing->m_MassValue[i] * (LA.y*LA.y + LA.z*LA.z);
		Iyy += m_pWing->m_MassValue[i] * (LA.x*LA.x + LA.z*LA.z);
		Izz += m_pWing->m_MassValue[i] * (LA.x*LA.x + LA.y*LA.y);
		Ixz += m_pWing->m_MassValue[i] * (LA.x*LA.z);
	}

	if(m_pWing2)
	{
		for(i=0; i<m_pWing2->m_NMass; i++)
		{
			LA = m_pWing2->m_MassPosition[i] + m_pPlane->m_LEWing2 - m_CoG;
			Ixx += m_pWing2->m_MassValue[i] * (LA.y*LA.y + LA.z*LA.z);
			Iyy += m_pWing2->m_MassValue[i] * (LA.x*LA.x + LA.z*LA.z);
			Izz += m_pWing2->m_MassValue[i] * (LA.x*LA.x + LA.y*LA.y);
			Ixz += m_pWing2->m_MassValue[i] * (LA.x*LA.z);
		}
	}
	if(m_pStab)
	{
		for(i=0; i<m_pStab->m_NMass; i++)
		{
			LA = m_pStab->m_MassPosition[i] + m_pPlane->m_LEStab - m_CoG;
			Ixx += m_pStab->m_MassValue[i] * (LA.y*LA.y + LA.z*LA.z);
			Iyy += m_pStab->m_MassValue[i] * (LA.x*LA.x + LA.z*LA.z);
			Izz += m_pStab->m_MassValue[i] * (LA.x*LA.x + LA.y*LA.y);
			Ixz += m_pStab->m_MassValue[i] * (LA.x*LA.z);
		}
	}

	if(m_pFin)
	{
		for(i=0; i<m_pFin->m_NMass; i++)
		{
			LA = m_pFin->m_MassPosition[i] + m_pPlane->m_LEFin - m_CoG;
			Ixx += m_pFin->m_MassValue[i] * (LA.y*LA.y + LA.z*LA.z);
			Iyy += m_pFin->m_MassValue[i] * (LA.x*LA.x + LA.z*LA.z);
			Izz += m_pFin->m_MassValue[i] * (LA.x*LA.x + LA.y*LA.y);
			Ixz += m_pFin->m_MassValue[i] * (LA.x*LA.z);
		}
	}

	if(m_pPlane && m_pPlane->m_bBody)
	{
		CBody *pBody = m_pPlane->m_pBody;
		for(i=0; i<pBody->m_NMass; i++)
		{
			m_Mass += pBody->m_MassValue[i];
			LA = pBody->m_MassPosition[i] - m_CoG;
			Ixx += pBody->m_MassValue[i] * (LA.y*LA.y + LA.z*LA.z);
			Iyy += pBody->m_MassValue[i] * (LA.x*LA.x + LA.z*LA.z);
			Izz += pBody->m_MassValue[i] * (LA.x*LA.x + LA.y*LA.y);
			Ixz += pBody->m_MassValue[i] * (LA.x*LA.z);
		}
	}

	m_Ib[0][0] = Ixx;
	m_Ib[1][1] = Iyy;
	m_Ib[2][2] = Izz;
	m_Ib[0][2] = m_Ib[2][0] = -Ixz;
	m_Ib[1][0] = m_Ib[1][2] = m_Ib[0][1] = m_Ib[2][1] = 0.0;
}




void StabAnalysisDlg::ComputeResults()
{
	// Calculates the various wing coefficients at each span station

	static int pos, Station;
	static double Lift, IDrag, VDrag, WingIDrag,XCP, YCP, qdyn;
	static double cosa, sina;
	static CVector Force, WindNormal, WindDirection, WindSide;
	QString strange;

	CWing::s_bTrace   = false;
	CWing::s_bVLM1    = m_pWPolar->m_bVLM1;

	Force.Set(0.0, 0.0, 0.0);

	m_pWing->m_bWingOut = false;
	if(m_pWing2) m_pWing2->m_bWingOut = false;
	if(m_pStab)  m_pStab->m_bWingOut  = false;
	if(m_pFin)   m_pFin->m_bWingOut   = false;

	//   Define wind axis
	cosa = cos(m_AlphaEq*PI/180.0);
	sina = sin(m_AlphaEq*PI/180.0);
	WindNormal.Set(  -sina, 0.0, cosa);
	WindDirection.Set(cosa, 0.0, sina);
	WindSide = WindNormal * WindDirection;

	qdyn = 0.5 * m_pWPolar->m_Density * u0 * u0;
	if(u0 <0.0)
	{
		m_bPointOut = true;
		return;
	}
	strange = QString("\n\n      ...Alpha=%1\n").arg(m_AlphaEq,0,'f',2);
	AddString(strange);
	AddString("        Calculating aerodynamic coefficients...\n");
	m_bPointOut        = false;

	CWing::s_Alpha     = m_AlphaEq;
	CWing::s_QInf      = u0;
	CWing::s_Viscosity = m_pWPolar->m_Viscosity;
	CWing::s_Density   = m_pWPolar->m_Density;

	Lift   = 0.0;
	IDrag  = 0.0;
	VDrag  = 0.0;
	XCP    = 0.0;
	YCP    = 0.0;

	m_GRm                 = 0.0;
	m_GCm = m_VCm = m_ICm = 0.0;
	m_GYm = m_VYm = m_IYm = 0.0;
	AddString("         Calculating main wing...\n");

	m_pWing->VLMTrefftz(m_Gamma, 0, Force, IDrag, m_pWPolar->m_bTiltedGeom);
	m_pWing->VLMComputeWing(m_Gamma, m_Cp,VDrag, XCP, YCP, m_GCm, m_VCm, m_ICm, m_GRm, m_GYm, m_VYm, m_IYm,
							m_CoG, m_pWPolar->m_bViscous, m_pWPolar->m_bTiltedGeom);

	m_pWing->VLMSetBending();
	if(m_pWing->m_bWingOut)  m_bPointOut = true;
	pos = m_pWing->m_MatSize;

	Station = m_pWing->m_NStation;

	if(m_pWing2)
	{
		AddString("       Calculating 2nd wing...\n");
		WingIDrag = 0.0;
		m_pWing2->VLMTrefftz(m_Gamma, pos, Force, WingIDrag, m_pWPolar->m_bTiltedGeom);
		m_pWing2->VLMComputeWing(m_Gamma+m_pWing->m_MatSize,
								 m_Cp+m_pWing->m_MatSize,
								 VDrag, XCP, YCP, m_GCm, m_VCm, m_ICm, m_GRm, m_GYm, m_VYm, m_IYm,
								 m_CoG, m_pWPolar->m_bViscous, m_pWPolar->m_bTiltedGeom);
		IDrag += WingIDrag;

		m_pWing2->VLMSetBending();
		if(m_pWing2->m_bWingOut) m_bPointOut = true;

		Station += m_pWing2->m_NStation;
		pos += m_pWing2->m_MatSize;
	}

	if(m_pStab)
	{
		AddString("         Calculating elevator...\n");
		WingIDrag = 0.0;

		m_pStab->VLMTrefftz(m_Gamma, pos, Force, WingIDrag, m_pWPolar->m_bTiltedGeom);
		m_pStab->VLMComputeWing(m_Gamma+pos,
								m_Cp+pos,
								VDrag, XCP, YCP, m_GCm, m_VCm, m_ICm, m_GRm, m_GYm, m_VYm, m_IYm,
								m_CoG, m_pWPolar->m_bViscous, m_pWPolar->m_bTiltedGeom);
		IDrag += WingIDrag;

		m_pStab->VLMSetBending();
		if(m_pStab->m_bWingOut) m_bPointOut = true;

		Station += m_pStab->m_NStation;
		pos += m_pStab->m_MatSize;
	}

	if(m_pFin)
	{
		AddString("         Calculating fin...\n");
		WingIDrag = 0.0;
		m_pFin->VLMTrefftz(m_Gamma, pos, Force, WingIDrag, m_pWPolar->m_bTiltedGeom);
		m_pFin->VLMComputeWing( m_Gamma+pos,
								m_Cp+pos,
								VDrag, XCP, YCP, m_GCm, m_VCm, m_ICm, m_GRm, m_GYm, m_VYm, m_IYm,
								m_CoG, m_pWPolar->m_bViscous, m_pWPolar->m_bTiltedGeom);
		if(m_pFin->m_bWingOut)  m_bPointOut = true;

		IDrag += WingIDrag;
		m_pFin->VLMSetBending();
	}

//	m_CL          =  2.0*Lift /u0/u0/m_pWing->m_Area;
	m_CL          =  2.0*Force.dot(WindNormal)    /u0/u0/m_pWPolar->m_WArea;
	m_CX          =  2.0*Force.dot(WindDirection) /u0/u0/m_pWPolar->m_WArea;
	m_CY          =  2.0*Force.dot(WindSide)      /u0/u0/m_pWPolar->m_WArea;
	m_InducedDrag =  1.0*IDrag                    /u0/u0/m_pWPolar->m_WArea;

	m_ViscousDrag = VDrag / m_pWPolar->m_WArea;

	m_XCP         = XCP/Force.dot(WindNormal)/m_pWPolar->m_Density;
	m_YCP         = YCP/Force.dot(WindNormal)/m_pWPolar->m_Density;

	m_GCm *= 1.0 / m_pWPolar->m_WArea /m_pWing->m_MAChord    /qdyn;
	m_VCm *= 1.0 / m_pWPolar->m_WArea /m_pWing->m_MAChord    /qdyn;
	m_ICm *= 1.0 / m_pWPolar->m_WArea /m_pWing->m_MAChord    /qdyn;
	m_GRm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan    /qdyn;

	m_GYm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan    /qdyn;
	m_VYm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan    /qdyn;
	m_IYm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan    /qdyn;

	VLMSetDownwash(m_Gamma);

	if(m_bPointOut) m_bWarning = true;
}



void StabAnalysisDlg::SolveCtrlDer(int ic, double const & DeltaAngle,
								   double *Xd, double *Yd, double *Zd, double *Ld, double *Md, double *Nd)
{
	CVector Force, Moment, is, js, ks;
	double cosa, sina;
	
	cosa = cos(m_AlphaEq*PI/180);
	sina = sin(m_AlphaEq*PI/180);
	
	is.Set(-cosa, 0.0, -sina);
	js.Set(  0.0, 1.0,   0.0);
	ks.Set( sina, 0.0, -cosa);
	
	QString strong = QString("     Solving the linear system for control %1\n").arg(ic);
	AddString(strong); 
	
	Gauss(m_aij, m_MatSize, m_cRHS+ic*m_MatSize, 1, &m_bCancel);
	
	strong = "     Calculating the control derivatives\n\n";
	AddString(strong);
	
	Forces(m_cRHS+ic*m_MatSize, m_RHS+50*m_MatSize, Force, Moment, m_pWPolar->m_bTiltedGeom);
/*qDebug("%d,   %14.7g,   %14.7g,   %14.7g,   %14.7g,   %14.7g,   %14.7g",
	   ic, Force.x, Force.y, Force.z, Moment.x, Moment.y, Moment.z);
qDebug("%d,   %14.7g,   %14.7g,   %14.7g,   %14.7g,   %14.7g,   %14.7g",
	   ic, Force0.x, Force0.y, Force0.z, Moment0.x, Moment0.y, Moment0.z);*/
	// make the difference with nominal results
	// which gives the stability derivative for a rotation of control ic
		
	*Xd = (Force-Force0).dot(is)/DeltaAngle;
	*Yd = (Force-Force0).dot(js)/DeltaAngle;
	*Zd = (Force-Force0).dot(ks)/DeltaAngle;
	*Ld = (Moment - Moment0).dot(is) /DeltaAngle;  // N.m/rad
	*Md = (Moment - Moment0).dot(js) /DeltaAngle;
	*Nd = (Moment - Moment0).dot(ks) /DeltaAngle;
}


void StabAnalysisDlg::ComputeControlDerivatives()
{
	//
	// Calculates the control derivatives for small control deflections, using forward derivatives
	// 
	// The geometry has been previously modified to set the control in the position 
	// defined for this iteration of the analysis
	//
	// Two options :
	//    1. Keep it simple, just change the normal vector of the rotated panels
	//       Advantage : only the RHS and a few columns of the influence matrix need to be changed
	//    2. A bit more complicated, regenerate the geometry.
	//       Advantage : More representative
	// Since we consider only small perturbations, use option 1
	//
	static CVector  V0, H, RN, V, C, H0;
	static int j, ic, p, pp;
	static double sina, cosa, DeltaAngle;
	QString str;
	Quaternion Quat;

	cosa = cos(m_AlphaEq*PI/180);
	sina = sin(m_AlphaEq*PI/180);
	
	V0.Set(u0*cosa, 0.0, u0*sina);

	double q = 1./2. * m_pWPolar->m_Density * u0 * u0;
	double b   = m_pWPolar->m_WSpan;
	double S   = m_pWPolar->m_WArea;
	double mac = m_pWing->m_MAChord;

	// Define the stability axes
	cosa = cos(m_AlphaEq*PI/180);
	sina = sin(m_AlphaEq*PI/180);

//	DeltaAngle = 0.001; //rad
	DeltaAngle = -7.*PI/180.0;
	BuildControlRHS(DeltaAngle*180.0/PI);

	for(ic=0; ic<m_NCtrls; ic++)
	{
		Xde[ic] = Yde[ic] =  Zde[ic] = Lde[ic] = Mde[ic] = Nde[ic] = 0.0;
	}
	
	str = "\n   ___Control derivatives____\n";
	AddString(str);
	
	
	// set the freestream veolocity field
	for (int p=0; p<m_MatSize; p++)
	{
		m_RHS[50*m_MatSize+p] = V0.x;
		m_RHS[51*m_MatSize+p] = V0.y;
		m_RHS[52*m_MatSize+p] = V0.z;	
	}
	
	//For each control, compute the stability derivative
	//  Longitudinal control derivatives : Xde/Zde/Mde
	//  Lateral stability derivatives :    Yde/Lde/Zde
	
	
	ic = 0;

	if(m_pPlane && ic==0)
	{
		// reload the base influence matrix
		memcpy(m_aij, m_aijRef, m_MatSize*m_MatSize*sizeof(double));
		//Wing tilt
		H0 = m_pPlane->m_LEWing;//hinge origin
		H.Set(0.0, 1.0, 0.0);
		Quat.Set(DeltaAngle*180.0/PI, H);
		// if panel p is on the modified control surface, then change 
		// the influence coefficient of all panels pp at ctrl point p
		str = "    Changing the influence matrix and RHS for wing tilt control\n";
		AddString(str);
		for(p=0; p<m_pWing->m_MatSize; p++)
		{
			Quat.Conjugate(m_pWing->m_pPanel[p].Normal, RN);
			for(pp=0; pp<m_MatSize; pp++)
			{
				C = m_pPanel[p].CtrlPt;
				C.RotateY(H0, DeltaAngle*180.0/PI);
				VLMGetVortexInfluence(m_pPanel+pp, C, V, true);
				m_aij[p*m_MatSize+pp] = V.dot(RN);
			}
		}
		SolveCtrlDer(ic, DeltaAngle, Xde+ic, Yde+ic, Zde+ic, Lde+ic, Mde+ic, Nde+ic);
		ic=1;
	}
	
	if(m_pPlane && m_pStab && ic==1)
	{
		str = "    Changing the influence matrix and RHS for elevator tilt control\n";
		AddString(str);
		memcpy(m_aij, m_aijRef, m_MatSize*m_MatSize*sizeof(double));
		//Elevator tilt
		H0 = m_pPlane->m_LEStab;//hinge origin
		H.Set(0.0, 1.0, 0.0);
		Quat.Set(DeltaAngle*180.0/PI, H);
		int pos = m_pWing->m_MatSize;

		//change the influence of each panel of the plane at each of the elevator's ctrl points
		for(p=0; p<m_pStab->m_MatSize; p++)
		{
			Quat.Conjugate(m_pPanel[p+pos].Normal, RN);
			for(pp=0; pp<m_MatSize; pp++)
			{
				C= m_pPanel[p+pos].CtrlPt;
				C.RotateY(H0, DeltaAngle*180.0/PI);
				VLMGetVortexInfluence(m_pPanel+pp, C, V, true);
				m_aij[(p+pos)*m_MatSize+pp] = V.dot(RN);
			}
		}
		SolveCtrlDer(ic, DeltaAngle, Xde+ic, Yde+ic, Zde+ic, Lde+ic, Mde+ic, Nde+ic);
		ic++;
	}

	//flap tilt
	for (j=0; j<m_NSurfaces; j++)
	{				
		if(m_ppSurface[j]->m_bTEFlap)
		{					
			str = QString("    Changing the influence matrix and RHS for flap control %1\n").arg(ic);
			AddString(str);
			memcpy(m_aij, m_aijRef, m_MatSize*m_MatSize*sizeof(double));
			Quat.Set(DeltaAngle*180.0/PI, m_ppSurface[j]->m_HingeVector);
		
			for(p=0; p<m_MatSize; p++)
			{
				if(m_ppSurface[j]->IsFlapPanel(p))
				{
					Quat.Conjugate(m_pPanel[p].Normal, RN);

					for(pp=0; pp<m_MatSize; pp++)
					{
						C.Copy(m_pPanel[p].CtrlPt);
						C.RotateY(m_ppSurface[j]->m_HingePoint, DeltaAngle*180.0/PI);
						VLMGetVortexInfluence(m_pPanel+pp, C, V, true);
						m_aij[p*m_MatSize+pp] = V.dot(RN);
					}
				}
			}
			SolveCtrlDer(ic, DeltaAngle, Xde+ic, Yde+ic, Zde+ic, Lde+ic, Mde+ic, Nde+ic);
			ic++;
		}
	}
	
	
	int nCtrl=0;
	if(m_pPlane)
	{
		//wing incidence

		str = tr("    Derivatives w.r.t. wing incidence")+"\n";
		AddString(str);
		str = QString("    Xd%1=%2        CXd%3=%4\n").arg(nCtrl).arg(Xde[nCtrl],      12,'g',5)
		                                              .arg(nCtrl).arg(Xde[nCtrl]/(q*S),12,'g',5);
		AddString(str);
		str = QString("    Yd%1=%2        CYd%3=%4\n").arg(nCtrl).arg(Yde[nCtrl],      12,'g',5)
		                                              .arg(nCtrl).arg(Yde[nCtrl]/(q*S),12,'g',5);
		AddString(str);
		str = QString("    Zd%1=%2        CZd%3=%4\n").arg(nCtrl).arg(Zde[nCtrl],      12,'g',5)
		                                              .arg(nCtrl).arg(Zde[nCtrl]/(q*S),12,'g',5);
		AddString(str);
		str = QString("    Ld%1=%2        CLd%3=%4\n").arg(nCtrl).arg(Lde[nCtrl],        12,'g',5)
		                                              .arg(nCtrl).arg(Lde[nCtrl]/(q*S*b),12,'g',5);
		AddString(str);
		str = QString("    Md%1=%2        CMd%3=%4\n").arg(nCtrl).arg(Mde[nCtrl],          12,'g',5)
		                                              .arg(nCtrl).arg(Mde[nCtrl]/(q*S*mac),12,'g',5);
		AddString(str);
		str = QString("    Nd%1=%2        CNd%3=%4\n").arg(nCtrl).arg(Nde[nCtrl],        12,'g',5)
		                                              .arg(nCtrl).arg(Nde[nCtrl]/(q*S*b),12,'g',5);
		AddString(str+"\n");
		
		nCtrl=1;

		if(m_pStab)
		{
			//elevator incidence

			str = tr("    Derivatives w.r.t. elevator incidence")+"\n";
			AddString(str);
			str = QString("    Xd%1=%2        CXd%3=%4\n").arg(nCtrl).arg(Xde[nCtrl],12,'g',5)
														  .arg(nCtrl).arg(Xde[nCtrl]/(q*S),12,'g',5);
			AddString(str);
			str = QString("    Yd%1=%2        CYd%3=%4\n").arg(nCtrl).arg(Yde[nCtrl],12,'g',5)
														  .arg(nCtrl).arg(Yde[nCtrl]/(q*S),12,'g',5);
			AddString(str);
			str = QString("    Zd%1=%2        CZd%3=%4\n").arg(nCtrl).arg(Zde[nCtrl],12,'g',5)
														  .arg(nCtrl).arg(Zde[nCtrl]/(q*S),12,'g',5);
			AddString(str);
			str = QString("    Ld%1=%2        CLd%3=%4\n").arg(nCtrl).arg(Lde[nCtrl],12,'g',5)
														  .arg(nCtrl).arg(Lde[nCtrl]/(q*S*b),12,'g',5);
			AddString(str);
			str = QString("    Md%1=%2        CMd%3=%4\n").arg(nCtrl).arg(Mde[nCtrl],12,'g',5)
														  .arg(nCtrl).arg(Mde[nCtrl]/(q*S*mac),12,'g',5);
			AddString(str);
			str = QString("    Nd%1=%2        CNd%3=%4\n").arg(nCtrl).arg(Nde[nCtrl],12,'g',5)
														  .arg(nCtrl).arg(Nde[nCtrl]/(q*S*b),12,'g',5);
			AddString(str+"\n");
			
			nCtrl = 2;
		}
	}

	// flap controls
	//wing first
	int nFlap = 0;
	for (j=0; j<m_pWing->m_NSurfaces; j++)
	{
		if(m_pWing->m_Surface[j].m_bTEFlap)
		{
			str = QString(tr("    Derivatives w.r.t. wing flap %1")+"\n").arg(nFlap);
			AddString(str);
			str = QString("    Xd%1=%2        CXd%3=%4\n").arg(nCtrl).arg(Xde[nCtrl],12,'g',5)
														  .arg(nCtrl).arg(Xde[nCtrl]/(q*S),12,'g',5);
			AddString(str);
			str = QString("    Yd%1=%2        CYd%3=%4\n").arg(nCtrl).arg(Yde[nCtrl],12,'g',5)
														  .arg(nCtrl).arg(Yde[nCtrl]/(q*S),12,'g',5);
			AddString(str);
			str = QString("    Zd%1=%2        CZd%3=%4\n").arg(nCtrl).arg(Zde[nCtrl],12,'g',5)
														  .arg(nCtrl).arg(Zde[nCtrl]/(q*S),12,'g',5);
			AddString(str);
			str = QString("    Ld%1=%2        CLd%3=%4\n").arg(nCtrl).arg(Lde[nCtrl],12,'g',5)
														  .arg(nCtrl).arg(Lde[nCtrl]/(q*S*b),12,'g',5);
			AddString(str);
			str = QString("    Md%1=%2        CMd%3=%4\n").arg(nCtrl).arg(Mde[nCtrl],12,'g',5)
														  .arg(nCtrl).arg(Mde[nCtrl]/(q*S*mac),12,'g',5);
			AddString(str);
			str = QString("    Nd%1=%2        CNd%3=%4\n").arg(nCtrl).arg(Nde[nCtrl],12,'g',5)
														  .arg(nCtrl).arg(Nde[nCtrl]/(q*S*b),12,'g',5);
			AddString(str+"\n");

			nCtrl++;
			nFlap++;
		}
	}
	//elevator next 
	nFlap=0;
	if(m_pStab)
	{
		for (j=0; j<m_pStab->m_NSurfaces; j++)
		{
			if(m_pStab->m_Surface[j].m_bTEFlap)
			{
				str = QString(tr("    Derivatives w.r.t. elevator flap %1")+"\n").arg(nFlap);
				str = QString("    Xd%1=%2        CXd%3=%4\n").arg(nCtrl).arg(Xde[nCtrl],12,'g',5)
															  .arg(nCtrl).arg(Xde[nCtrl]/(q*S),12,'g',5);
				AddString(str);
				str = QString("    Yd%1=%2        CYd%3=%4\n").arg(nCtrl).arg(Yde[nCtrl],12,'g',5)
															  .arg(nCtrl).arg(Yde[nCtrl]/(q*S),12,'g',5);
				AddString(str);
				str = QString("    Zd%1=%2        CZd%3=%4\n").arg(nCtrl).arg(Zde[nCtrl],12,'g',5)
															  .arg(nCtrl).arg(Zde[nCtrl]/(q*S),12,'g',5);
				AddString(str);
				str = QString("    Ld%1=%2        CLd%3=%4\n").arg(nCtrl).arg(Lde[nCtrl],12,'g',5)
															  .arg(nCtrl).arg(Lde[nCtrl]/(q*S*b),12,'g',5);
				AddString(str);
				str = QString("    Md%1=%2        CMd%3=%4\n").arg(nCtrl).arg(Mde[nCtrl],12,'g',5)
															  .arg(nCtrl).arg(Mde[nCtrl]/(q*S*mac),12,'g',5);
				AddString(str);
				str = QString("    Nd%1=%2        CNd%3=%4\n").arg(nCtrl).arg(Nde[nCtrl],12,'g',5)
															  .arg(nCtrl).arg(Nde[nCtrl]/(q*S*b),12,'g',5);
				AddString(str+"\n");
				
				nCtrl++;
				nFlap++;
			}
		}
	}
	nFlap=0;
	if(m_pFin)
	{
		for (j=0; j<m_pFin->m_NSurfaces; j++)
		{
			if(m_pFin->m_Surface[j].m_bTEFlap)
			{
				str = QString(tr("    Derivatives w.r.t. fin flap %1")+"\n").arg(nFlap);
				AddString(str);
				str = QString("    Xd%1=%2        CXd%3=%4\n").arg(nCtrl).arg(Xde[nCtrl],12,'g',5)
															  .arg(nCtrl).arg(Xde[nCtrl]/(q*S),12,'g',5);
				AddString(str);
				str = QString("    Yd%1=%2        CYd%3=%4\n").arg(nCtrl).arg(Yde[nCtrl],12,'g',5)
															  .arg(nCtrl).arg(Yde[nCtrl]/(q*S),12,'g',5);
				AddString(str);
				str = QString("    Zd%1=%2        CZd%3=%4\n").arg(nCtrl).arg(Zde[nCtrl],12,'g',5)
															  .arg(nCtrl).arg(Zde[nCtrl]/(q*S),12,'g',5);
				AddString(str);
				str = QString("    Ld%1=%2        CLd%3=%4\n").arg(nCtrl).arg(Lde[nCtrl],12,'g',5)
															  .arg(nCtrl).arg(Lde[nCtrl]/(q*S*b),12,'g',5);
				AddString(str);
				str = QString("    Md%1=%2        CMd%3=%4\n").arg(nCtrl).arg(Mde[nCtrl],12,'g',5)
															  .arg(nCtrl).arg(Mde[nCtrl]/(q*S*mac),12,'g',5);
				AddString(str);
				str = QString("    Nd%1=%2        CNd%3=%4\n").arg(nCtrl).arg(Nde[nCtrl],12,'g',5)
															  .arg(nCtrl).arg(Nde[nCtrl]/(q*S*b),12,'g',5);
				AddString(str+"\n");
			
				nCtrl++;
				nFlap++;
			}
		}
	}
	
	str ="\n";
	AddString(str);
}



void StabAnalysisDlg::ComputeStabilityDerivatives()
{
	// The stability derivatives are estimated by forward difference at U=(U0,0,0).
	// The reference condition has been saved during the calculation of the trimmed condition
	
	static CVector V0, Force, Moment, CGM, is, js, ks, Vi, Vj, Vk, Ris, Rjs, Rks;
	static int p;
	static double sina, cosa, deltaspeed, deltarotation, q, S, mac, b;
	QString str;

	deltaspeed    = 0.01;        //  m/s   for forward difference estimation
	deltarotation = 0.001;       //  rad/s for forward difference estimation

	// Define the stability axes
	cosa = cos(m_AlphaEq*PI/180);
	sina = sin(m_AlphaEq*PI/180);
	is.Set(-cosa, 0.0, -sina);
	js.Set(  0.0, 1.0,   0.0);
	ks.Set( sina, 0.0, -cosa);

	V0 = is * (-u0); //is the steady state velocity vector, if no sideslip

	q = 1./2. * m_pWPolar->m_Density * u0 * u0;

	//______________________________________________________________________________
	// RHS for unit speed vectors
	Vi = V0 + is * deltaspeed;
	Vj = V0 + js * deltaspeed;
	Vk = V0 + ks * deltaspeed;

	for (p=0; p<m_MatSize; p++)
	{
		m_uRHS[p] = - m_pPanel[p].Normal.dot(Vi);
		m_vRHS[p] = - m_pPanel[p].Normal.dot(Vj);
		m_wRHS[p] = - m_pPanel[p].Normal.dot(Vk);
		m_RHS[50*m_MatSize+p] = Vi.x;
		m_RHS[51*m_MatSize+p] = Vi.y;
		m_RHS[52*m_MatSize+p] = Vi.z;
		m_RHS[53*m_MatSize+p] = Vj.x;
		m_RHS[54*m_MatSize+p] = Vj.y;
		m_RHS[55*m_MatSize+p] = Vj.z;
		m_RHS[56*m_MatSize+p] = Vk.x;
		m_RHS[57*m_MatSize+p] = Vk.y;
		m_RHS[58*m_MatSize+p] = Vk.z;
	}
	//______________________________________________________________________________
	// RHS for unit rotation vectors around Stability axis
	// stability axis origin is CoG
	for (p=0; p<m_MatSize; p++)
	{
		CGM = m_pPanel[p].VortexPos - m_CoG;
		Ris = is*CGM *deltarotation+V0;
		Rjs = js*CGM *deltarotation+V0;
		Rks = ks*CGM *deltarotation+V0;
		m_pRHS[p] = - m_pPanel[p].Normal.dot(Ris);
		m_qRHS[p] = - m_pPanel[p].Normal.dot(Rjs);
		m_rRHS[p] = - m_pPanel[p].Normal.dot(Rks);
		m_RHS[59*m_MatSize+p] = Ris.x;
		m_RHS[60*m_MatSize+p] = Ris.y;
		m_RHS[61*m_MatSize+p] = Ris.z;
		m_RHS[62*m_MatSize+p] = Rjs.x;
		m_RHS[63*m_MatSize+p] = Rjs.y;
		m_RHS[64*m_MatSize+p] = Rjs.z;
		m_RHS[65*m_MatSize+p] = Rks.x;
		m_RHS[66*m_MatSize+p] = Rks.y;
		m_RHS[67*m_MatSize+p] = Rks.z;
	}

	// The influence matrix is unchanged, so baksubstitute for unit vortex circulations
/*	memcpy(m_RHS,             m_uRHS, m_MatSize * sizeof(double));
	memcpy(m_RHS+m_MatSize,   m_vRHS, m_MatSize * sizeof(double));
	memcpy(m_RHS+2*m_MatSize, m_wRHS, m_MatSize * sizeof(double));
	memcpy(m_RHS+3*m_MatSize, m_pRHS, m_MatSize * sizeof(double));
	memcpy(m_RHS+4*m_MatSize, m_qRHS, m_MatSize * sizeof(double));
	memcpy(m_RHS+5*m_MatSize, m_rRHS, m_MatSize * sizeof(double));*/

//	memcpy(m_aij, m_aijRef, m_MatSize*m_MatSize*sizeof(double));
//	Gauss(m_aij, m_MatSize, m_RHS, 6, &m_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_uRHS, m_Index, m_RHS,             m_MatSize, &m_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_vRHS, m_Index, m_RHS+m_MatSize,   m_MatSize, &m_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_wRHS, m_Index, m_RHS+2*m_MatSize, m_MatSize, &m_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_pRHS, m_Index, m_RHS+3*m_MatSize, m_MatSize, &m_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_qRHS, m_Index, m_RHS+4*m_MatSize, m_MatSize, &m_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_rRHS, m_Index, m_RHS+5*m_MatSize, m_MatSize, &m_bCancel);

/*	memcpy(m_uRHS, m_RHS,             m_MatSize * sizeof(double));
	memcpy(m_vRHS, m_RHS+m_MatSize,   m_MatSize * sizeof(double));
	memcpy(m_wRHS, m_RHS+2*m_MatSize, m_MatSize * sizeof(double));
	memcpy(m_pRHS, m_RHS+3*m_MatSize, m_MatSize * sizeof(double));
	memcpy(m_qRHS, m_RHS+4*m_MatSize, m_MatSize * sizeof(double));
	memcpy(m_rRHS, m_RHS+5*m_MatSize, m_MatSize * sizeof(double));*/


	// Compute stabiliy and control derivatives
	Xu = Xw = Zu = Zw = Mu = Mw = Mq = Zwp = Mwp = 0.0;
	Yv = Yp = Yr = Lv = Lp = Lr = Nv = Np  = Nr  = 0.0;

	//________________________________________________
	// 1st ORDER STABILITY DERIVATIVES

	// x-derivatives________________________
	Forces(m_RHS, m_RHS+50*m_MatSize, Force, Moment, m_pWPolar->m_bTiltedGeom);
	Xu = (Force-Force0).dot(is)/deltaspeed;
	Zu = (Force-Force0).dot(ks)/deltaspeed;
	Mu = 0.0;
//	Mu = (Moment-Moment0).dot(js)/deltaspeed;

	// y-derivatives________________________
	Forces(m_RHS+m_MatSize, m_RHS+53*m_MatSize, Force, Moment, m_pWPolar->m_bTiltedGeom);
	Yv = (Force-Force0).dot(js)/deltaspeed;
	Lv = (Moment-Moment0).dot(is)/deltaspeed;
	Nv = (Moment-Moment0).dot(ks)/deltaspeed;

	// z-derivatives________________________
	Forces(m_RHS+2*m_MatSize, m_RHS+56*m_MatSize, Force, Moment, m_pWPolar->m_bTiltedGeom);
	Xw = (Force-Force0).dot(is)/deltaspeed;
	Zw = (Force-Force0).dot(ks)/deltaspeed;  
	Mw = (Moment-Moment0).dot(js)/deltaspeed;

	// p-derivatives
	Forces(m_RHS+3*m_MatSize, m_RHS+59*m_MatSize, Force, Moment, m_pWPolar->m_bTiltedGeom);
	Yp = (Force-Force0).dot(js)/deltarotation;
	Lp = (Moment-Moment0).dot(is)/deltarotation;
	Np = (Moment-Moment0).dot(ks)/deltarotation;

	// q-derivatives
	Forces(m_RHS+4*m_MatSize, m_RHS+62*m_MatSize, Force, Moment, m_pWPolar->m_bTiltedGeom);
	Zq = (Force-Force0).dot(ks)  /deltarotation;
	Mq = (Moment-Moment0).dot(js)/deltarotation;

	// r-derivatives
	Forces(m_RHS+5*m_MatSize, m_RHS+65*m_MatSize, Force, Moment, m_pWPolar->m_bTiltedGeom);
	Yr = (Force-Force0).dot(js)/deltarotation;
	Lr = (Moment-Moment0).dot(is)/deltarotation;
	Nr = (Moment-Moment0).dot(ks)/deltarotation;

	//________________________________________________
	// 2nd ORDER STABILITY DERIVATIVES
	// Zwp & Mwp ... ?

	b   = m_pWPolar->m_WSpan;
	S   = m_pWPolar->m_WArea;
	mac = m_pWing->m_MAChord;

	str = "   ___Longitudinal derivatives____\n";
	AddString(str);
	str = QString("    Xu=%1 \n").arg(Xu,12,'g',5);
	AddString(str);
	str = QString("    Xw=%1 \n").arg(Xw,12,'g',5);
	AddString(str);
	str = QString("    Zu=%1 \n").arg(Zu,12,'g',5);
	AddString(str);
	str = QString("    Zw=%1      CLa=%2\n").arg(Zw,12,'g',5).arg(Zw*   u0      /(q*S),12,'g',5);
	AddString(str);
	str = QString("    Zq=%1      CLq=%2\n").arg(Zq,12,'g',5).arg(Zq* 2.*u0     /(q*S*mac),12,'g',5);
	AddString(str);
	str = QString("    Mu=%1 \n").arg(Mu,12,'g',5);
	AddString(str);
	str = QString("    Mw=%1      Cma=%2\n").arg(Mw,12,'g',5).arg(Mw*    u0     /(q*S*mac),12,'g',5);
	AddString(str);
	str = QString("    Mq=%1      Cmq=%2\n").arg(Mq,12,'g',5).arg(Mq*(2.*u0/mac)/(q*S*mac),12,'g',5);
	AddString(str);

	str = "\n   ___Lateral derivatives____\n";
	AddString(str);
	str = QString("    Yv=%1      CYb=%2\n").arg(Yv,12,'g',5).arg(  Yv*    u0   /(q*S),12,'g',5);
	AddString(str);
	str = QString("    Yp=%1      CYp=%2\n").arg(Yp,12,'g',5).arg(  Yp* 2.*u0   /(q*S*b),12,'g',5);
	AddString(str);
	str = QString("    Yr=%1      CYr=%2\n").arg(Yr,12,'g',5).arg(  Yr* 2.*u0   /(q*S*b),12,'g',5);
	AddString(str);
	str = QString("    Lv=%1      Clb=%2\n").arg(Lv,12,'g',5).arg(  Lv*    u0   /(q*S*b),12,'g',5);
	AddString(str);
	str = QString("    Lp=%1      Clp=%2\n").arg(Lp,12,'g',5).arg(  Lp*(2.*u0/b)/(q*S*b),12,'g',5);
	AddString(str);
	str = QString("    Lr=%1      Clr=%2\n").arg(Lr,12,'g',5).arg(  Lr*(2.*u0/b)/(q*S*b),12,'g',5);
	AddString(str);
	str = QString("    Nv=%1      Cnb=%2\n").arg(Nv,12,'g',5).arg(  Nv*    u0   /(q*S*b),12,'g',5);
	AddString(str);
	str = QString("    Np=%1      Cnp=%2\n").arg(Np,12,'g',5).arg(  Np*(2.*u0/b)/(q*S*b),12,'g',5);
	AddString(str);
	str = QString("    Nr=%1      Cnr=%2\n\n").arg(Nr,12,'g',5).arg(Nr*(2.*u0/b)/(q*S*b),12,'g',5);
	AddString(str);

	/*
	Xu  = -Xu;
	Xw  = -Xw;
	Mw  = -Mw;
	Mq  = -Mq;
	Yv  = -Yv;
	Yp  = -Yp;
	Yr  = -Yr;
	Lp  = -Lp;
	Lr  = -Lr;
	Lv  = -Lv;
	Np  = -Np;
	Nr  = -Nr;
	Nv  = -Nv;*/
	Zq = -Zq;
	Zw = -Zw;
	Zu = -Zu;
}



void StabAnalysisDlg::ComputeStabilityInertia()
{
	//
	// Returns the inertia tensor in stability axis
	//
	// R  is the rotation matrix from Body Frame to stability frame
	// Ib is the inertia tensor in the body frame, with origin at the CoG
	//
	// The body frame is the one in which the geometry has been defined,
	// i.e. the x axis points backwards, y is starboard and z is upwards
	//
	// Is is the inertia tensor in stability axes with origin at the CoG
	//                               Is = tR.Ib.R
	//

	static int i,j;
	static double tR[3][3], tmp[3][3];

	tR[0][0] = m_R[0][0];
	tR[0][1] = m_R[1][0];
	tR[0][2] = m_R[2][0];
	tR[1][0] = m_R[0][1];
	tR[2][0] = m_R[0][2];

	tR[1][1] = m_R[1][1];
	tR[1][2] = m_R[2][1];
	tR[2][1] = m_R[1][2];

	tR[2][2] = m_R[2][2];

	// tmp = Ib.R
	for(i=0; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			tmp[i][j] = m_Ib[i][0]*m_R[0][j] + m_Ib[i][1]*m_R[1][j] + m_Ib[i][2]*m_R[2][j];
		}
	}

	// Is = tR.tmp
	for(i=0; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			m_Is[i][j] = tR[i][0]*tmp[0][j] + tR[i][1]*tmp[1][j] + tR[i][2]*tmp[2][j];
		}
	}
}


#define CM_ITER_MAX 20

bool StabAnalysisDlg::ComputeTrimmedConditions()
{
	//______________________________________________________________________________________
	// Method :
	//	- For level flight, find a.o.a. such that Cm=0, by iterations
	//	- Reconstruct right side circulations if calculation was symmetric
	//	- Sort results i.a.w. panel numbering
	//	- Set trimmed parameters for level flight or other
	//______________________________________________________________________________________

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong, strange;

	static int p, m, iter;
	static double a, a0, a1, Cm, Cm0, Cm1, tmp;
	static double eps = 1.e-7;
	static double Lift, cosa, sina, phi;
	static double VerticalCl;
	static CVector VInf, Force, Moment, WindNormal;

	// find aoa such that Cm=0;

	//Build the unit RHS vectors along x and z in Body Axis
	BuildRHS();

	if (m_bCancel) return false;

	// build the influence matrix in Body Axis
	BuildInfluenceMatrix();

	if (m_bCancel) return false;

	if (!SolveUnitSpeeds())	//solve for the u,w unit vectors
	{
		AddString(tr("\n\nSingular matrix - aborting....\n"));
		m_bWarning = true;
		return false;
	}

	//start loop to find zero-pitching-moment alpha
	iter = 0;
	a0 = -PI/4.0;
	a1 =  PI/4.0;
	a = 0.0;
	Cm0 = VLMComputeCm(a0*180.0/PI);
	Cm1 = VLMComputeCm(a1*180.0/PI);
	Cm = 1.0;

	//are there two initial values of opposite signs ?
	while(Cm0*Cm1>0.0 && iter <=20)
	{
		a0 *=0.9;
		a1 *=0.9;
		Cm0 = VLMComputeCm(a0*180.0/PI);
		Cm1 = VLMComputeCm(a1*180.0/PI);
		iter++;
		qApp->processEvents();
		if(m_bCancel) break;
	}
	if(iter>=100 || m_bCancel) return false;
	
	iter = 0;
	
	//Cm0 and Cm1 are of opposite sign
	if(Cm0>Cm1)
	{
		tmp = Cm1;
		Cm1 = Cm0;
		Cm0 = tmp;
		tmp = a0;
		a0  =  a1;
		a1  = tmp;
	}

	while (fabs(Cm)>eps && iter<=CM_ITER_MAX)
	{
		a = a0 - (a1-a0) * Cm0/(Cm1-Cm0);
		Cm = VLMComputeCm(a*180.0/PI);
		if(Cm>0.0)
		{
			a1  = a;
			Cm1 = Cm;
		}
		else
		{
			a0  = a;
			Cm0 = Cm;
		}
		iter++;
		qApp->processEvents();
		if(m_bCancel) break;
	}
	if(iter>=CM_ITER_MAX || m_bCancel) return false;

	m_AlphaEq = a*180.0/PI;

	Cm = VLMComputeCm(m_AlphaEq);// for information only, should be zero

	//reconstruct all results from cosine and sine unit vectors
	m=0;

	cosa = cos(m_AlphaEq*PI/180.0);
	sina = sin(m_AlphaEq*PI/180.0);
	for(p=0; p<m_MatSize; p++)
	{
		m_Gamma[m] =   cosa * m_uRHS[p] + sina * m_wRHS[p];
		m++;
	}

	qApp->processEvents();
	if(m_bCancel) return false;

	//______________________________________________________________________________________
	// Calculate the trimmed conditions for this control setting and calculated Alpha_eq
/*		phi = bank angle
		V   = sqrt(2 m g / rho S CL cos(phi))    (airspeed)
		R   = V^2 / g tan(phi)        (turn radius, positive for right turn)
		W   = V / R                   (turn rate, positive for right turn)
		p   = 0                       (roll rate, zero for steady turn)
		q   = W sin(phi)              (pitch rate, positive nose upward)
		r   = W cos(phi)              (yaw rate, positive for right turn) */

	AddString("      Calculating the vortices circulations...\n");

	//so far we have a unit Vortex Strength
	if(m_pWPolar->m_Type==7)
	{
		// type 7 : fixed lift
		// find the speeds which will create a lift equal to the weight

		AddString("      Calculating speed to balance the weight\n");
		CWing::s_QInf      = 1.0;
		CWing::s_Alpha     = m_AlphaEq;
		CWing::s_Viscosity = m_pWPolar->m_Viscosity;
		CWing::s_Density   = m_pWPolar->m_Density;

		WindNormal.Set(-sin(m_AlphaEq*PI/180.0), 0.0, cos(m_AlphaEq*PI/180.0));
		VInf.Set(       cos(m_AlphaEq*PI/180.0), 0.0, sin(m_AlphaEq*PI/180.0));
		for(p=0; p<m_MatSize; p++)
		{
			m_RHS[50*m_MatSize+p] = VInf.x;
			m_RHS[51*m_MatSize+p] = VInf.y;
			m_RHS[52*m_MatSize+p] = VInf.z;
		}

		Lift = 0.0;
		p=0;

		u0 = 1.0;
		Forces(m_Gamma, m_RHS+50*m_MatSize, Force, Moment, m_pWPolar->m_bTiltedGeom);

		phi = m_pWPolar->m_BankAngle *PI/180.0;

		Lift   = Force.dot(WindNormal);		//N/rho ; bank effect not included
		VerticalCl = Lift*2.0/m_pWPolar->m_WArea * cos(phi)/m_pWPolar->m_Density;

		if(Lift<=0.0)
		{
			u0 = -100.0;
			strong = QString("      Found a negative lift for Alpha=%1.... skipping the angle...\n").arg(m_AlphaEq,0,'f',2);
			if(m_bTrace) AddString("\n");
			if(m_bTrace) AddString(strong);
			m_bPointOut = true;
			m_bWarning = true;
		}
		else
		{
			u0 =  sqrt( 2.0* 9.81 * m_Mass /m_pWPolar->m_Density/m_pWPolar->m_WArea / VerticalCl );
			strong = QString("      Alpha=%1   QInf = %2").arg(m_AlphaEq,5,'f',2).arg(u0*pMainFrame->m_mstoUnit,5,'f',2);
			GetSpeedUnit(strange, pMainFrame->m_SpeedUnit);
			strong+= strange + "\n";
			if(m_bTrace) AddString(strong);

			if(m_pWPolar->m_BankAngle>PRECISION)
			{
				m_radius = u0*u0/9.81/tan(phi);
				m_W      = u0/m_radius;
				m_p      = 0.0;
				m_q      = m_W * sin(phi);
				m_r      = m_W * cos(phi);

				strong = QString("      Phi         =%1").arg(m_pWPolar->m_BankAngle,5,'f',2);
				strong += QString::fromUtf8("°");
				if(m_bTrace) AddString(strong);

				strong = QString("      Turn radius =%1").arg(m_radius,5,'f',2);
				if(m_bTrace) AddString(strong);

				strong = QString("      Turn rate   =%1").arg(m_W,5,'f',2);
				if(m_bTrace) AddString(strong);

				strong = QString("      Roll rate   =%1").arg(m_p,5,'f',2);
				if(m_bTrace) AddString(strong);

				strong = QString("      Pitch rate  =%1").arg(m_q,5,'f',2);
				if(m_bTrace) AddString(strong);

				strong = QString("      Yaw rate    =%1").arg(m_r,5,'f',2);
				if(m_bTrace) AddString(strong);
			}
		}
	}

	//______________________________________________________________________________________
	// Scale circulations to speeds
	for(p=0; p<m_MatSize; p++)	m_Gamma[p] *= u0;

	// Store the force and moment acting on the surfaces
	// Will be of use later for stability control derivatives
	Force0  = Force*u0*u0;
	Moment0 = Moment*u0*u0;

	VInf *= u0;
	for(p=0; p<m_MatSize; p++)
	{
		m_RHS[50*m_MatSize+p] = VInf.x;
		m_RHS[51*m_MatSize+p] = VInf.y;
		m_RHS[52*m_MatSize+p] = VInf.z;
	}

	Forces(m_Gamma, m_RHS+50*m_MatSize, Force0, Moment0, m_pWPolar->m_bTiltedGeom);
	return  true;
}



bool StabAnalysisDlg::ControlLoop()
{
	//
	//	Loop for each control value
	//          Update the geometry, design variables
	//          Build the influence matrix
	//          Perfom LU matrix decomposition
	//          Solve a first time the VLM problem to find the trimmed conditions:
	//              - solve for unit RHS
	//              - iterate to find equilibrium aoa such that Cm=0 in steady level flight or banked turn
	//          Build the rotation matrix from body axes to stability axes
	//          Build the RHS for unit velocity fields in stability axes
	//          Build the RHS for unit control rotations (Normals only) in stability axes
	//          Solve the VLM problem for RHS in stability axes
	//          Compute stability derivatives
	//          Compute control derivatives
	//          Compute inertia in stability axis
	//          Longitudinal stability :
	//             - Build longitudinal state matrix
	//             - Solve for eigenvalues and eigenvectors
	//          Lateral stability
	//             - Build lateral state matrix
	//             - Solve for eigenvalues and eigenvectors
	//          Compute aero coeffs for alpha_eq
	//          Store OpPoint and polar data
	//	end loop
	//
	int i, nrhs;
	double t;
	QString str;

	QMiarex *pMiarex= (QMiarex*)s_pMiarex;

	if(m_ControlMax<m_ControlMin) m_ControlDelta = -fabs(m_ControlDelta);

	nrhs  = (int)fabs((m_ControlMax-m_ControlMin)*1.0001/m_ControlDelta) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs==0) nrhs = 1;//compute at least nominal control positions, even if none is active nor defined
	else if(nrhs>=100)
	{
		QMessageBox::warning(this, tr("Warning"), "The number of points to be calculated will be limited to 100");
		nrhs = 100;
	}

	m_bTrace = true;

	str = QString("   Solving the problem... \n");
	AddString(str);
	for (i=0; i<nrhs; i++)
	{
		// create the geometry for the control parameter
		// so first restore the initial geometry
		memcpy(m_pPanel, m_pMemPanel, m_MatSize * sizeof(CPanel));
		memcpy(m_pNode,  m_pMemNode,  m_nNodes  * sizeof(CVector));

		//define the control position for this iteration
		t = m_ControlMin +(double)i *m_ControlDelta;
		SetControlPositions(t);
		if(m_bCancel) break;

		// next find the balanced and trimmed conditions
		if(!ComputeTrimmedConditions())
		{
			//no zero moment alpha
			str = QString("      Unsuccessfull attempt to trim the model for Control=%1 - skipping.\n\n\n").arg(t,10,'f',3);
			AddString(str);
			m_bWarning = true;
		}
		else
		{
			//Build the rotation matrix from body axes to stability axes
			BuildRotationMatrix();

			// Compute inertia in stability axes
			ComputeStabilityInertia();

			str = QString("      Mass=%1\n").arg(m_Mass, 12,'g',5);
			AddString(str);

			str = "\n   ___Center of Gravity Position - Body axis____\n";
			AddString(str);
			str = QString("    CoG_x=%1\n").arg(m_CoG.x, 12,'g',5);
			AddString(str);
			str = QString("    CoG_y=%1\n").arg(m_CoG.y, 12,'g',5);
			AddString(str);
			str = QString("    CoG_z=%1\n").arg(m_CoG.z, 12,'g',5);
			AddString(str);

			str = "\n   ___Inertia - Body Axis - CoG Origin____\n";
			AddString(str);
			str = QString("    Ibxx=%1\n").arg(m_Ib[0][0], 12,'g',5);
			AddString(str);
			str = QString("    Ibyy=%1\n").arg(m_Ib[1][1], 12,'g',5);
			AddString(str);
			str = QString("    Ibzz=%1\n").arg(m_Ib[2][2], 12,'g',5);
			AddString(str);
			str = QString("    Ibxz=%1\n").arg(m_Ib[0][2], 12,'g',5);
			AddString(str);
			str = "\n   ___Inertia - Stability Axis - CoG Origin____\n";
			AddString(str);
			str = QString("    Isxx=%1\n").arg(m_Is[0][0], 12,'g',5);
			AddString(str);
			str = QString("    Isyy=%1\n").arg(m_Is[1][1], 12,'g',5);
			AddString(str);
			str = QString("    Iszz=%1\n").arg(m_Is[2][2], 12,'g',5);
			AddString(str);
			str = QString("    Isxz=%1\n").arg(m_Is[0][2], 12,'g',5);
			AddString(str);
			AddString("\n");

			// Compute stability and control derivatives in stability axes
			// viscous or not viscous ?
			ComputeStabilityDerivatives();
			ComputeControlDerivatives();

			// Construct the state matrices - longitudinal and lateral
			BuildStateMatrices();
			// Solve for eigenvalues
			if(!SolveEigenvalues())
			{
				str = QString("      Unsuccessfull attempt to compute eigenvalues for Control=%1 - skipping.\n\n\n").arg(t,10,'f',3);
				AddString(str);
				m_bWarning = true;
			}
			else
			{
				// Compute aero coefficients for trimmed conditions CL, CD, ...
				ComputeResults();

				if(m_bConverged)
				{
					//store results
					if(m_pPlane) pMiarex->AddPOpp(m_bPointOut, m_Cp, m_Gamma);
					else         pMiarex->AddWOpp(m_bPointOut, m_Gamma);
				}
			}
			AddString("     \n");
		}
		if(m_bCancel) break;
	}
	return true;
}



void StabAnalysisDlg::Forces(double *Gamma, double *VInf, CVector &Force, CVector &Moment, bool bTilted, bool bTrace)
{
	// Calculates the forces using a farfield method
	// Calculates the moments by a near field method, i.e. direct summation on the panels
	// Downwash is evaluated at a distance 100 times the span downstream (i.e. infinite)
	//
	if(!m_pPanel||!m_pWPolar) return;

	static bool bOutRe, bError, bOut, bOutCl;
	static int j, k, l,  p,  m;
	static double alpha, cosa, sina, Re, PCd, Cl, tau, StripArea, ViscousDrag;
	static double QInf, qdyn;
	static CVector  C, PtC4, LeverArm, dF, WindDirection, WindNormal, PanelLeverArm, Wg, Fd, dFM;
	static CVector Velocity;
	QMiarex *pMiarex= (QMiarex*)s_pMiarex;

	bOut = bOutCl = bError = false;

	if(!bTilted) alpha = m_AlphaEq;
	else         alpha = 0.0;

	cosa = cos(alpha*PI/180.0);
	sina = sin(alpha*PI/180.0);

	//   Define the wind axis
	WindNormal.Set(   -sina, 0.0, cosa);
	WindDirection.Set( cosa, 0.0, sina);

	p=m=0;

	Fd.Set(0.0, 0.0, 0.0);
	Force.Set( 0.0, 0.0, 0.0);
	Moment.Set(0.0, 0.0, 0.0);
	ViscousDrag = 0.0;

	for(j=0; j<m_NSurfaces; j++)
	{
		for(k=0; k<m_ppSurface[j]->m_NYPanels; k++)
		{
			StripArea = 0.0;
			
			for(l=0; l<m_ppSurface[j]->m_NXPanels; l++)
			{
				Velocity.x = *(VInf               +p);
				Velocity.y = *(VInf +   m_MatSize +p);
				Velocity.z = *(VInf + 2*m_MatSize +p);
				
				StripArea += m_pPanel[p].Area;
				// get the far-field force
				if(m_pWPolar->m_bVLM1 || m_pPanel[p].m_bIsTrailing)
				{
					C = m_pPanel[p].CtrlPt;
					C.x = m_pWing->m_PlanformSpan * 100.0;

					Wg  = GetSpeedVector(C, Gamma);
					Wg += Velocity; //total speed vector

					//induced force
					dF  = Wg * m_pPanel[p].Vortex;
					dF.x *= 1.    * Gamma[p];  // N/rho
					dF.y *=         Gamma[p];  // N/rho
					dF.z *=         Gamma[p];  // N/rho

					Force += dF;        // N/rho
					Fd += dF;
				}

				// for the moments, get the direct summation on the panels
				PanelLeverArm = m_pPanel[p].VortexPos - m_CoG;
				dFM  = Velocity * m_pPanel[p].Vortex * Gamma[p];      // Newtons/rho
				Moment += dFM * PanelLeverArm;                    // N.m/rho
				p++;
			}

			if(m_pWPolar->m_bViscous)
			{
				//add the viscous drag component to force and moment
				QInf = Velocity.VAbs();
				
				qdyn = 0.5 * m_pWPolar->m_Density * QInf * QInf;
				m_ppSurface[j]->GetC4(k, PtC4, tau);
				Re = m_ppSurface[j]->GetChord(tau) * QInf /m_pWPolar->m_Viscosity;
				Cl = Fd.dot(WindNormal)*m_pWPolar->m_Density/qdyn/StripArea;
				PCd    = GetVar(pMiarex->m_poaPolar, 2, m_ppSurface[j]->m_pFoilA, m_ppSurface[j]->m_pFoilB, Re, Cl, tau, bOutRe, bError);
				PCd   *= StripArea * 1/2*QInf*QInf;              // Newtons/rho
				bOut = bOut || bOutRe || bError;
				ViscousDrag += PCd ;                             // Newtons/rho

				LeverArm   = PtC4 - m_CoG;
//				Moment += (WindDirection*LeverArm)*PCd;                                        // N.m/rho
				Moment.x += PCd * (WindDirection.y*LeverArm.z - WindDirection.z*LeverArm.y);   // N.m/rho
				Moment.y += PCd * (WindDirection.z*LeverArm.x - WindDirection.x*LeverArm.z);
				Moment.z += PCd * (WindDirection.x*LeverArm.y - WindDirection.y*LeverArm.x);
			}
			Fd.Set(0.0,0.0,0.0);
			m++;
		}
	}
	Force -= WindDirection*Force.dot(WindDirection)/2.0;

	if(m_pWPolar->m_bViscous) Force += WindDirection * ViscousDrag;

	Force  *= m_pWPolar->m_Density;                          // N
	Moment *= m_pWPolar->m_Density;                          // N.m
}


void StabAnalysisDlg::GetNDStabDerivatives(CWOpp *pNewPoint)
{
	double b, S, mac, q;

	q = 1./2. * m_pWPolar->m_Density * u0 * u0;	
	b   = m_pWPolar->m_WSpan;
	S   = m_pWPolar->m_WArea;
	mac = m_pWing->m_MAChord;

	pNewPoint->CLa = Zw*    u0     /(q*S);
	pNewPoint->CLq = Zq* 2.*u0     /(q*S*mac);
	pNewPoint->Cma = Mw*    u0     /(q*S*mac);
	pNewPoint->Cmq = Mq*(2.*u0/mac)/(q*S*mac);
	pNewPoint->CYb = Yv*    u0     /(q*S);
	pNewPoint->CYp = Yp* 2.*u0     /(q*S*b);
	pNewPoint->CYr = Yr* 2.*u0     /(q*S*b);
	pNewPoint->Clb = Lv*    u0     /(q*S*b);
	pNewPoint->Clp = Lp*(2.*u0/b)  /(q*S*b);
	pNewPoint->Clr = Lr*(2.*u0/b)  /(q*S*b);
	pNewPoint->Cnb = Nv*    u0     /(q*S*b);
	pNewPoint->Cnp = Np*(2.*u0/b)  /(q*S*b);
	pNewPoint->Cnr = Nr*(2.*u0/b)  /(q*S*b);
	
	pNewPoint->m_nControls = m_pWPolar->m_nControls;
	for(int i=0; i<m_pWPolar->m_nControls; i++)
	{
		pNewPoint->CXe[i] = Xde[i]/(q*S);
		pNewPoint->CYe[i] = Yde[i]/(q*S);
		pNewPoint->CZe[i] = Zde[i]/(q*S);
		pNewPoint->CLe[i] = Lde[i]/(q*S*b);
		pNewPoint->CMe[i] = Mde[i]/(q*S*b);
		pNewPoint->CNe[i] = Nde[i]/(q*S*b);
   }
}


CVector StabAnalysisDlg::GetSpeedVector(CVector C, double *Gamma)
{
	// Returns the velocity vector at point C based on the panel distribution
	// and on the array Gamma of vortex circulation

	int pp;
	static CVector V, VTot;
	VTot.Set(0.0,0.0,0.0);

	for (pp=0; pp<m_MatSize;pp++)
	{
		VLMGetVortexInfluence(m_pPanel+pp, C, V, true);
		VTot += V * Gamma[pp];
	}
	return VTot;
}



void StabAnalysisDlg::InitDialog()
{
	QString FileName = QDir::tempPath() + "/XFLR5.log";
	m_pXFile = new QFile(FileName);
	if (!m_pXFile->open(QIODevice::WriteOnly | QIODevice::Text)) m_pXFile = NULL;

	m_pctrlTextOutput->clear();
	m_bPointOut = false;
	m_bCancel   = false;
	m_bWarning  = false;

	SetFileHeader();
}




void StabAnalysisDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Escape:
		{
			OnCancelAnalysis();
			event->accept();
			return;
		}
		default:
			event->ignore();
	}
}



void StabAnalysisDlg::OnCancelAnalysis()
{
	if(m_pXFile->isOpen()) m_pXFile->close();
	m_bCancel = true;
	if(m_bIsFinished) done(1);
	WriteString("Analysis Cancelled\n");
}


void StabAnalysisDlg::SetControlPositions(double t)
{
	// Modifies the geometry by setting the control positions to the specified position t
	// The panels are rotated as a whole, i.e. both the panel's boundary point position and the panel's normal

	Quaternion Quat;
	int j;
	double angle;
	CWing *pWing, *pStab, *pFin;
	CVector HingeVector(0.0, 1.0, 0.0);

	pWing = m_pWing;
	pStab = m_pStab;
	pFin  = m_pFin;

	m_Ctrl = t;

	// update the variables & geometry
	// if plane : WingTilt, elevator Tilt
	// if flaps : wing flaps, elevator flaps

	//the CG position is fixed for this analysis

	m_NCtrls = 0;

	if(m_pPlane)
	{
		//wing incidence
		if(m_pWPolar->m_bActiveControl[0])
		{
			//wing tilt
			angle = m_pWPolar->m_MinControl[0] + t * (m_pWPolar->m_MaxControl[0] - m_pWPolar->m_MinControl[0]);
			angle -= m_pPlane->m_WingTilt;
			Quat.Set(angle, HingeVector);

			for(j=0; j<m_pWing->m_MatSize; j++)
			{
				(m_pWing->m_pPanel+j)->Rotate(m_pPlane->m_LEWing, Quat, angle);
			}
		}
		m_NCtrls=1;

		if(m_pStab)
		{
			//elevator incidence
			if(m_pWPolar->m_bActiveControl[1] )
			{
				//Elevator tilt
				angle = m_pWPolar->m_MinControl[1] + t * (m_pWPolar->m_MaxControl[1] - m_pWPolar->m_MinControl[1]);
				angle -= m_pPlane->m_StabTilt;

				Quat.Set(angle, HingeVector);

				for(j=0; j<m_pStab->m_MatSize; j++)
				{
					(m_pStab->m_pPanel+j)->Rotate(m_pPlane->m_LEStab, Quat, angle);
				}
			}
			m_NCtrls = 2;
		}
	}

	// flap controls
	//wing first
	for (j=0; j<pWing->m_NSurfaces; j++)
	{
		if(pWing->m_Surface[j].m_bTEFlap)
		{
			if(m_pWPolar->m_bActiveControl[m_NCtrls])
			{
				angle = m_pWPolar->m_MinControl[m_NCtrls] + t * (m_pWPolar->m_MaxControl[m_NCtrls] - m_pWPolar->m_MinControl[m_NCtrls]);
				if(!pWing->m_Surface[j].RotateFlap(angle))  return;
			}
			m_NCtrls++;
		}
	}
	//elevator next and last
	if(pStab)
	{
		for (j=0; j<pStab->m_NSurfaces; j++)
		{
			if(pStab->m_Surface[j].m_bTEFlap)
			{
				if(m_pWPolar->m_bActiveControl[m_NCtrls])
				{
					angle = m_pWPolar->m_MinControl[m_NCtrls] + t * (m_pWPolar->m_MaxControl[m_NCtrls] - m_pWPolar->m_MinControl[m_NCtrls]);
					if(!pStab->m_Surface[j].RotateFlap(angle)) return;
				}
				m_NCtrls++;
			}
		}
	}
	if(pFin)
	{
		for (j=0; j<pFin->m_NSurfaces; j++)
		{
			if(pFin->m_Surface[j].m_bTEFlap)
			{
				if(m_pWPolar->m_bActiveControl[m_NCtrls])
				{
					angle = m_pWPolar->m_MinControl[m_NCtrls] + t * (m_pWPolar->m_MaxControl[m_NCtrls] - m_pWPolar->m_MinControl[m_NCtrls]);
					if(!pFin->m_Surface[j].RotateFlap(angle)) return;
				}
				m_NCtrls++;
			}
		}
	}
//	str = QString("      ...Control = %1\n").arg(m_ControlMin+i*m_ControlDelta,8,'f',2);
//	AddString(str);
}



void StabAnalysisDlg::SetFileHeader()
{
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QTextStream out(m_pXFile);

	out << "\n";
	out << pMainFrame->m_VersionName;
	out << "\n";
	out << m_pWing->m_WingName;
	out << "\n";
	if(pMiarex && pMiarex->m_pCurWPolar)
	{
//		out << pMiarex->m_pCurWPolar->m_WPlrName;
//		out << "\n";
	}

	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");

	out << str;
	out << "\n___________________________________\n\n";

}


void StabAnalysisDlg::SetupLayout()
{
	QDesktopWidget desktop;
	QRect r = desktop.geometry();
	setMinimumHeight(r.height()/2);
	setMinimumWidth(r.width()/2);

	m_pctrlTextOutput = new QTextEdit;
	QFont CourierFont = QFont("Courier",10);
	CourierFont.setStyleHint(QFont::Courier);
	m_pctrlTextOutput->setFont(CourierFont);
	m_pctrlTextOutput->setReadOnly(true);
	m_pctrlTextOutput->setLineWrapMode(QTextEdit::NoWrap);
	m_pctrlTextOutput->setWordWrapMode(QTextOption::NoWrap);

	m_pctrlCancel = new QPushButton(tr("Cancel"));
	connect(m_pctrlCancel, SIGNAL(clicked()), this, SLOT(OnCancelAnalysis()));

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(m_pctrlCancel);
	buttonsLayout->addStretch(1);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_pctrlTextOutput);
	mainLayout->addLayout(buttonsLayout);
	setLayout(mainLayout);
}




bool StabAnalysisDlg::SolveEigenvalues()
{
	// Finds the eigenvalues and eigenvectors of the state matrices ALong and ALat

	static double pLong[5], pLat[5];//the coefficients of the characteristic polynomial
	static int i;
	QString str;

	CharacteristicPol(m_ALong, pLong);

	if(!LinBairstow(pLong, m_rLong, 4))
	{
		AddString("\n       Error extracting longitudinal eigenvalues\n");
		return false;
	}

	for(i=0; i<4; i++)
	{
		if(!Eigenvector(m_ALong, m_rLong[i], m_vLong+i*4))
		{
			str = QString("Error extracting longitudinal eigenvector for mode %1\n").arg(i);
			return false;
		}
	}

	str = "\n\n   ___Longitudinal modes____\n\n";
	AddString(str);

	str = QString("      Eigenvalue:  %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
		  .arg(m_rLong[0].real(),9, 'g', 4).arg(m_rLong[0].imag(),9, 'g', 4)
		  .arg(m_rLong[1].real(),9, 'g', 4).arg(m_rLong[1].imag(),9, 'g', 4)
		  .arg(m_rLong[2].real(),9, 'g', 4).arg(m_rLong[2].imag(),9, 'g', 4)
		  .arg(m_rLong[3].real(),9, 'g', 4).arg(m_rLong[3].imag(),9, 'g', 4);
	AddString(str);
	str=("                    _____________________________________________________________________________________________________\n");
	AddString(str);  

	str = QString("      Eigenvector: %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
			  .arg(m_vLong[i].real(),    9, 'g', 4).arg(m_vLong[i].imag(),    9, 'g', 4)
			  .arg(m_vLong[i+4].real(),  9, 'g', 4).arg(m_vLong[i+4].imag(),  9, 'g', 4)
			  .arg(m_vLong[i+8].real(),  9, 'g', 4).arg(m_vLong[i+8].imag(),  9, 'g', 4)
			  .arg(m_vLong[i+12].real(), 9, 'g', 4).arg(m_vLong[i+12].imag(), 9, 'g', 4);
	AddString(str);
	for (i=1; i<4; i++)
	{
			str = QString("                   %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(m_vLong[i].real(),    9, 'g', 4).arg(m_vLong[i].imag(),    9, 'g', 4)
					  .arg(m_vLong[i+4].real(),  9, 'g', 4).arg(m_vLong[i+4].imag(),  9, 'g', 4)
					  .arg(m_vLong[i+8].real(),  9, 'g', 4).arg(m_vLong[i+8].imag(),  9, 'g', 4)
					  .arg(m_vLong[i+12].real(), 9, 'g', 4).arg(m_vLong[i+12].imag(), 9, 'g', 4);
			AddString(str);
	}
	str = "\n";
	AddString(str);

	CharacteristicPol(m_ALat, pLat);

	if(!LinBairstow(pLat, m_rLat, 4))
	{
		AddString("\n       Error extracting lateral eigenvalues\n");
		return false;
	}

	for(i=0; i<4; i++)
	{
		if(!Eigenvector(m_ALat, m_rLat[i], m_vLat+i*4))
		{
			str = QString("Error extracting lateral eigenvector for mode %1\n").arg(i);
			return false;
		}
	}

	
	str = "\n\n   ___Lateral modes____\n\n";
	AddString(str);

	str = QString("      Eigenvalue:  %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
		  .arg(m_rLat[0].real(),9, 'g', 4).arg(m_rLat[0].imag(),9, 'g', 4)
		  .arg(m_rLat[1].real(),9, 'g', 4).arg(m_rLat[1].imag(),9, 'g', 4)
		  .arg(m_rLat[2].real(),9, 'g', 4).arg(m_rLat[2].imag(),9, 'g', 4)
		  .arg(m_rLat[3].real(),9, 'g', 4).arg(m_rLat[3].imag(),9, 'g', 4);
	AddString(str);
	str=("                    _____________________________________________________________________________________________________\n");
	AddString(str);  

	str = QString("      Eigenvector: %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
			  .arg(m_vLat[i].real(),    9, 'g', 4).arg(m_vLat[i].imag(),    9, 'g', 4)
			  .arg(m_vLat[i+4].real(),  9, 'g', 4).arg(m_vLat[i+4].imag(),  9, 'g', 4)
			  .arg(m_vLat[i+8].real(),  9, 'g', 4).arg(m_vLat[i+8].imag(),  9, 'g', 4)
			  .arg(m_vLat[i+12].real(), 9, 'g', 4).arg(m_vLat[i+12].imag(), 9, 'g', 4);
	AddString(str);
	for (i=1; i<4; i++)
	{
			str = QString("                   %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(m_vLat[i].real(),    9, 'g', 4).arg(m_vLat[i].imag(),    9, 'g', 4)
					  .arg(m_vLat[i+4].real(),  9, 'g', 4).arg(m_vLat[i+4].imag(),  9, 'g', 4)
					  .arg(m_vLat[i+8].real(),  9, 'g', 4).arg(m_vLat[i+8].imag(),  9, 'g', 4)
					  .arg(m_vLat[i+12].real(), 9, 'g', 4).arg(m_vLat[i+12].imag(), 9, 'g', 4);
			AddString(str);
	}
	str = "\n";
	AddString(str);
	
	return true;
}


bool StabAnalysisDlg::SolveUnitSpeeds()
{
	//______________________________________________
	// Solves the system for unit vortex circulations
	// The input is the RHS previoulsy built for unit speeds in the x and z directions
	//

	AddString("      Solving the linear system...\n");

	memcpy(m_RHS,           m_uRHS, m_MatSize * sizeof(double));
	memcpy(m_RHS+m_MatSize, m_wRHS, m_MatSize * sizeof(double));
	memcpy(m_aijRef, m_aij, m_MatSize*m_MatSize*sizeof(double));// we'll need it later for stability derivatives

//	if(!Gauss(m_aij,m_MatSize, m_RHS, 2, &m_bCancel))
	if(!Crout_LU_Decomposition_with_Pivoting(m_aij, m_Index, m_MatSize, &m_bCancel))
	{
		AddString("      Singular Matrix.... Aborting calculation...\n");
		m_bConverged = false;
		return false;
	}
	else m_bConverged = true;

	Crout_LU_with_Pivoting_Solve(m_aij, m_uRHS, m_Index, m_RHS,           m_MatSize, &m_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_wRHS, m_Index, m_RHS+m_MatSize, m_MatSize, &m_bCancel);
	
	memcpy(m_uRHS, m_RHS,           m_MatSize * sizeof(double));
	memcpy(m_wRHS, m_RHS+m_MatSize, m_MatSize * sizeof(double));

	return true;
}



void StabAnalysisDlg::StartAnalysis()
{
	// Initializes, then launches the analysis

	if(!m_pPlane && !m_pWing) return;
	if(!m_pWPolar) return;
	if(m_pWPolar->m_Type !=7) return;
	
	m_pctrlCancel->setText("Cancel");

	QString strong;
	m_bIsFinished = false;
	m_bCancel = false;
	strong = "Launching VLM Analysis....\n\n";
	AddString(strong);

	
	strong = QString("Total VLM panels = %1\n\n").arg(m_MatSize);
	AddString(strong);

	//back-up the current geometry
	memcpy(m_pMemPanel, m_pPanel, m_MatSize* sizeof(CPanel));
	memcpy(m_pMemNode,  m_pNode,  m_nNodes * sizeof(CVector));

	strong = QString("Type %1 Analysis\n\n").arg(m_pWPolar->m_Type);
	AddString(strong);
	m_bCancel = false;

	if(m_pWPolar->m_bAutoInertia)	ComputeBodyAxisInertia();
	else
	{
		m_Mass = m_pWPolar->m_Weight;
		m_CoG.Set(m_pWPolar->m_CoG);
		m_Ib[0][0] = m_pWPolar->m_CoGIxx;
		m_Ib[1][1] = m_pWPolar->m_CoGIyy;
		m_Ib[2][2] = m_pWPolar->m_CoGIzz;
		m_Ib[0][2] = m_Ib[2][0] = -m_pWPolar->m_CoGIxz;
		m_Ib[1][0] = m_Ib[1][2] = m_Ib[0][1] = m_Ib[2][1] = 0.0;	
	}

	if(m_pWPolar->m_Type==7) ControlLoop();

	if (!m_bCancel && !m_bWarning) strong = "\nVLM Analysis completed successfully\n";
	else if (m_bWarning)           strong = "\nVLM Analysis completed ... Errors encountered\n";
	AddString(strong);
	m_bIsFinished = true;
	m_pctrlCancel->setText("Close");

	//restore the geometry
	memcpy(m_pPanel, m_pMemPanel, m_MatSize * sizeof(CPanel));
	memcpy(m_pNode,  m_pMemNode,  m_nNodes * sizeof(CVector));
}


double StabAnalysisDlg::VLMComputeCm(double alpha)
{
	// Returns the geometric pithing moment coefficient for the specified angle of attack
	// The effect of the viscous drag is not included

	static int p;
	static double Cm, cosa, sina;
	static CVector V, Force, PanelLeverArm;

	Cm = 0.0;
	cosa = cos(alpha*PI/180.0);
	sina = sin(alpha*PI/180.0);

	V.Set(cosa, 0.0, sina);

	for(p=0; p<m_MatSize; p++)
	{
		//write vector operations in-line, more efficient
//		PanelLeverArm = m_pPanel[p].VortexPos - m_CoG;
		PanelLeverArm.x = m_pPanel[p].VortexPos.x - m_CoG.x;
		PanelLeverArm.y = m_pPanel[p].VortexPos.y - m_CoG.y;
		PanelLeverArm.z = m_pPanel[p].VortexPos.z - m_CoG.z;

//		Force  = V * m_pPanel[p].Vortex * (m_uRHS[p]*cosa + m_wRHS[p]*sina);         //Newtons/rho
		Force.x  = (V.y*m_pPanel[p].Vortex.z - V.z*m_pPanel[p].Vortex.y) * (m_uRHS[p]*cosa + m_wRHS[p]*sina);  //Newtons/rho
		Force.y  = (V.z*m_pPanel[p].Vortex.x - V.x*m_pPanel[p].Vortex.z) * (m_uRHS[p]*cosa + m_wRHS[p]*sina);  //Newtons/rho
		Force.z  = (V.x*m_pPanel[p].Vortex.y - V.y*m_pPanel[p].Vortex.x) * (m_uRHS[p]*cosa + m_wRHS[p]*sina);  //Newtons/rho

		Cm += -PanelLeverArm.x * Force.z + PanelLeverArm.z*Force.x; //N.m/rho
	}
	Cm *= m_pWPolar->m_Density;
	return Cm;
}



void StabAnalysisDlg::VLMGetVortexInfluence(CPanel *pPanel, CVector const &C, CVector &V, bool bAll)
{
	// Calculates the influence of the vortex on pPanel at point C
	// V is the resulting velocity
	// if bAll, then both the bound and the trailing vortices influence is taken into account
	// otherwise, only the trailing vortices influence calculated
	//

	static CVector AA,BB, VG;

	V.Set(0.0,0.0,0.0);

	if(m_pWPolar->m_bVLM1)
	{
		// get the horseshoe vortex's influence
		VLMCmn(pPanel->A, pPanel->B, C, V, bAll);
		if(m_pWPolar->m_bGround)
		{
			AA = pPanel->A;
			BB = pPanel->B;
			AA.z = -AA.z - 2.0*m_pWPolar->m_Height;//correction in V4.0
			BB.z = -BB.z - 2.0*m_pWPolar->m_Height;
			VLMCmn(AA, BB, C, VG, bAll);
			V.x += VG.x;
			V.y += VG.y;
			V.z -= VG.z;
		}
	}
}



void StabAnalysisDlg::VLMSetDownwash(double *Gamma)
{
	// calculates the induced angles from the vortices strengths
	int p, pp, m, nSurf;
	static CVector C, V;

	m=0;
	nSurf = 0;
	//wing first
	memset(m_pWing->m_Vd, 0, sizeof(m_pWing->m_Vd));

	m=0;
	for (p=0; p< m_pWing->m_MatSize; p++)
	{
		if(m_pWing->m_pPanel[p].m_bIsTrailing)
		{
			C = (m_pNode[m_pWing->m_pPanel[p].m_iTA] + m_pNode[m_pWing->m_pPanel[p].m_iTB])/2.0;
			for (pp=0; pp<m_MatSize; pp++)
			{
				VLMGetVortexInfluence(m_pPanel+pp,C,V,false);
				m_pWing->m_Vd[m] += V * Gamma[pp];
			}
			m_pWing->m_Ai[m] = atan2(m_pWing->m_Vd[m].z, u0) * 180.0/PI;
			m++;
		}
	}

	if(m_pWing2)
	{
		m=0;
		for (p=0; p< m_pWing2->m_MatSize; p++)
		{
			if(m_pWing2->m_pPanel[p].m_bIsTrailing)
			{
				C = (m_pNode[m_pWing2->m_pPanel[p].m_iTA] + m_pNode[m_pWing2->m_pPanel[p].m_iTB])/2.0;
				for (pp=0; pp<m_MatSize; pp++)
				{
					VLMGetVortexInfluence(m_pPanel+pp,C,V,false);
					m_pWing2->m_Vd[m] += V * Gamma[pp];
				}
				m_pWing2->m_Ai[m] = atan2(m_pWing2->m_Vd[m].z, u0) * 180.0/PI;
				m++;
			}
		}
	}

	if(m_pStab)
	{
		m=0;
		for (p=0; p< m_pStab->m_MatSize; p++)
		{
			if(m_pStab->m_pPanel[p].m_bIsTrailing)
			{
				C = (m_pNode[m_pStab->m_pPanel[p].m_iTA] + m_pNode[m_pStab->m_pPanel[p].m_iTB])/2.0;
				for (pp=0; pp<m_MatSize; pp++)
				{
					VLMGetVortexInfluence(m_pPanel+pp,C,V,false);
					m_pStab->m_Vd[m] += V * Gamma[pp];
				}
				m_pStab->m_Ai[m] = atan2(m_pStab->m_Vd[m].z, u0) * 180.0/PI;
				m++;
			}
		}
	}

	if(m_pFin)
	{
		m=0;
		for (p=0; p< m_pFin->m_MatSize; p++)
		{
			if(m_pFin->m_pPanel[p].m_bIsTrailing)
			{
				C = (m_pNode[m_pFin->m_pPanel[p].m_iTA] + m_pNode[m_pFin->m_pPanel[p].m_iTB])/2.0;
				for (pp=0; pp<m_MatSize; pp++)
				{
					VLMGetVortexInfluence(m_pPanel+pp,C,V,false);
					m_pFin->m_Vd[m] += V * Gamma[pp];
				}
				m_pFin->m_Ai[m] = atan2(m_pFin->m_Vd[m].z, u0) * 180.0/PI;
				m++;
			}
		}
	}
}



void StabAnalysisDlg::VLMSetAi(double *Gamma)
{
	// calculates the induced angles from the vortices strengths
	static CVector C, CG, V, Vt;
	int p, pp, m;
	double Ai[4*MAXSTATIONS];
	m=0;

//	CVector K(   -sin(m_Alpha*PI/180.0), 0.0, cos(m_Alpha*PI/180.0));
//	CVector QInf( cos(m_Alpha*PI/180.0), 0.0, sin(m_Alpha*PI/180.0));
//	QInf.Set(1.0,0.0,0.0);

	if(m_bTrace) AddString("        Calculating induced angles...\n");

	for (p=0; p<m_MatSize; p++)
	{
		if(m_pPanel[p].m_bIsTrailing)
		{
			C = (m_pNode[m_pPanel[p].m_iTA] + m_pNode[m_pPanel[p].m_iTB])/2.0;
//			C += QInf * 20.0*m_pWing->m_PlanformSpan;
			C.x  = 100.0*m_pWing->m_PlanformSpan;

			CG.x = C.x;
			CG.y = C.y;
			CG.z = C.z-2.0*m_pWPolar->m_Height;

			Vt.Set(0.0,0.0,0.0);

			for (pp=0; pp<m_MatSize; pp++)
			{
				VLMGetVortexInfluence(m_pPanel+pp, C, V, true);
				Vt += V * Gamma[pp];
			}
			Ai[m] = atan2(Vt.z, u0) * 180.0/PI; // TODO : factor 2 in excess when calculating in the Trefftz plane
			m++;
		}
	}
	int pos = 0;
	for (m=0; m<m_pWing->m_NStation; m++)	    m_pWing->m_Ai[m] = Ai[m];
	pos += m_pWing->m_NStation;
	if(m_pWing2)
	{
		for (m=0; m<m_pWing2->m_NStation; m++)	m_pWing2->m_Ai[m] = Ai[m+pos];
		pos += m_pWing2->m_NStation;
	}
	if(m_pStab)
	{
		for (m=0; m<m_pStab->m_NStation; m++)	m_pStab->m_Ai[m] = Ai[m+pos];
		pos += m_pStab->m_NStation;
	}
	if(m_pFin)
	{
		for (m=0; m<m_pFin->m_NStation; m++)	m_pFin->m_Ai[m] = Ai[m+pos];
	}
}


void StabAnalysisDlg::WriteString(QString strong)
{
	if(!m_pXFile) return;
	if(!m_pXFile->isOpen()) return;
	QTextStream ds(m_pXFile);
	ds << strong;
}























