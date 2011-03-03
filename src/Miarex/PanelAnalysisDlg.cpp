/****************************************************************************

	PanelAnalysisDlg Class
	Copyright (C) 2009-2010 Andre Deperrois XFLR5@yahoo.com

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

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QTimer>
#include <QDir>
#include <math.h>
#include "PanelAnalysisDlg.h"
#include "../MainFrame.h"
#include "../Globals.h"
#include "../Objects/CVector.h"
#include "Miarex.h"
#include <QtDebug>

void *PanelAnalysisDlg::s_pMiarex;
void *PanelAnalysisDlg::s_pMainFrame;

bool g_bTrace = false;


PanelAnalysisDlg::PanelAnalysisDlg()
{
	setWindowTitle(tr("3D Panel Analysis"));

	SetupLayout();

	m_Alpha      = 0.0;
	m_AlphaMax   = 0.0;
	m_AlphaDelta = 0.0;
	m_QInf       = 0.0;
	m_QInfMax    = 0.0;
	m_QInfDelta  = 0.0;

	m_pWPolar = NULL;
	RFF = 10.0;
	eps = 1.e-7;
	
	m_bSequence      = false;
	m_bIsFinished    = false;
	m_b3DSymetric    = false;
	m_bSequence      = false;
	m_bWarning       = false;
	m_bType4         = false;
	m_bXFile         = false;
	m_bConverged     = false;
	m_bCancel        = false;
	m_bTrefftz       = false;

	m_MatSize = m_nNodes = m_NSurfaces = 0;
	m_CL  = m_CX  = m_CY  = 0.0;
	m_GCm = m_GRm = m_GYm = m_VCm = m_VYm = m_IYm = 0.0;
	m_ControlMin  = m_ControlMax = m_ControlDelta  = 0.0;
	m_XCP = m_YCP = 0.0;
	m_ViscousDrag = m_InducedDrag = 0.0;

	m_MatSize        = 0;
	m_SymSize        = 0;
	m_nNodes         = 0;
	m_NSurfaces      = 0;
	m_NWakeColumn    = 0;
	m_WakeInterNodes = 1;
	m_MaxWakeIter    = 0;

	m_nWakeNodes = 0;
	m_WakeSize   = 0;

	m_strOut = "";

	m_pWingList[0] = m_pWingList[1] = m_pWingList[2] = m_pWingList[3] = NULL;
	m_pBody  = NULL;

	m_pWing   = NULL;
	m_pWPolar = NULL;
	m_pPlane  = NULL;
	m_ppSurface = NULL;

	s_pMiarex        = NULL;
	s_pMainFrame     = NULL;
	m_pWPolar        = NULL;

//	m_ppPanel        = NULL;
	m_pPanel         = NULL;
	m_pWakePanel     = NULL;
	m_pRefWakePanel  = NULL;
	m_pMemPanel      = NULL;
	m_pNode          = NULL;
	m_pMemNode       = NULL;
	m_pWakeNode      = NULL;
	m_pRefWakeNode   = NULL;
	m_pTempWakeNode  = NULL;

	m_aij = m_aijWake = NULL;
	m_RHS = m_RHSRef = NULL;

	memset(m_Sigma,  0, sizeof(m_Sigma));
	memset(m_Mu,     0, sizeof(m_Mu));
	memset(m_Cp,     0, sizeof(m_Cp));
	memset(m_Speed,  0, sizeof(m_Speed));

	m_Ctrl = 0.0;
	Theta0 = 0.0;
	u0     = 0.0;
//	m_Mass = 0.0;

	//Dimensional stability derivatives
	Xu = Xw = Zu = Zw = Zq = Mu = Mw = Mq = Zwp = Mwp = 0.0;
	Yv = Yp = Yr = Lv = Lp = Lr = Nv = Np = Nr = 0.0;

	// Non dimensional stability derivatives
	CXu = CZu = Cmu = CXq = CZq = Cmq = CXa = CZa = Cma = 0.0;
	CYb = CYp = CYr = Clb = Clp = Clr = Cnb = Cnp = Cnr = 0.0;
	CXe = CYe = CZe = Cle = Cme = Cne = 0.0;

	memset(m_ALong, 0, 16*sizeof(double));
	memset(m_ALat,  0, 16*sizeof(double));
	memset(m_R,     0,  9*sizeof(double));
	memset(m_Is,    0,  9*sizeof(double));
	memset(m_Ib,    0,  9*sizeof(double));

	memset(m_Cp,0,sizeof(m_Cp));
	memset(m_uRHS, 0, sizeof(m_uRHS));
	memset(m_vRHS, 0, sizeof(m_vRHS));
	memset(m_wRHS, 0, sizeof(m_wRHS));
	memset(m_pRHS, 0, sizeof(m_pRHS));
	memset(m_qRHS, 0, sizeof(m_qRHS));
	memset(m_rRHS, 0, sizeof(m_rRHS));

	memset(m_uVl, 0, sizeof(m_uVl));
	memset(m_wVl, 0, sizeof(m_wVl));
}



void PanelAnalysisDlg::AddString(QString strong)
{
	m_pctrlTextOutput->insertPlainText(strong);
	m_pctrlTextOutput->ensureCursorVisible();
	WriteString(strong);
}


bool PanelAnalysisDlg::AlphaLoop()
{
	QString str;
	int nrhs;

	if(m_AlphaMax<m_Alpha) m_AlphaDelta = -fabs(m_AlphaDelta);
	nrhs  = (int)fabs((m_AlphaMax-m_Alpha)*1.0001/m_AlphaDelta) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100)
	{
		QMessageBox::warning(this, tr("Warning"),tr("The number of points to be calculated will be limited to 100"));
		nrhs = 100;
	}

	int MaxWakeIter = 1;
	int Size= m_MatSize;
	if(m_b3DSymetric) Size = m_SymSize;


	//ESTIMATED UNIT TIMES FOR OPERATIONS
	//BuildInfluenceMatrix :     10 x MatSize/400
	//CreateRHS :                10
	//CreateWakeContribution :    1
	//SolveUnitRHS :             30 x MatSize/400
	//ComputeFarField :          10 x MatSize/400x nrhs
	//ComputeOnBodyCp :           1 x nrhs
	//RelaxWake :                20 x nrhs x MaxWakeIter *
	//ComputeAeroCoefs :          5 x nrhs
	
	double TotalTime = 10.0*(double)m_MatSize/400.
					 + 10.
					 + 30.*(double)m_MatSize/400.
					 + 10*(double)m_MatSize/400*(double)nrhs
					 + 1*(double)nrhs
					 + 5*(double)nrhs ;

	if(!m_pWPolar->m_bThinSurfaces) TotalTime +=1.0; //for wake contribution

	if(m_pWPolar->m_bWakeRollUp) TotalTime += 20*nrhs*MaxWakeIter;

	m_pctrlProgress->setMinimum(0);
	m_pctrlProgress->setMaximum((int)TotalTime);
	m_Progress = 0.0;
	qApp->processEvents();

	str = QString(tr("   Solving the problem... ")+"\n");
	AddString(str);

	BuildInfluenceMatrix();
	if (m_bCancel) return true;

	CreateUnitRHS();
	if (m_bCancel) return true;

	if(!m_pWPolar->m_bThinSurfaces)
	{
		//compute wake contribution
		CreateWakeContribution();

		//add wake contribution to matrix and RHS
		for(int p=0; p<m_MatSize; p++)
		{
			m_uRHS[p]+= m_uWake[p];
			m_wRHS[p]+= m_wWake[p];
			for(int pp=0; pp<m_MatSize; pp++)
			{
				m_aij[p*m_MatSize+pp] += m_aijWake[p*m_MatSize+pp];
			}
		}
	}
	if (m_bCancel) return true;

	if (!SolveUnitRHS())
	{
		m_bWarning = true;
		return true;
	}
	if (m_bCancel) return true;

	CreateSourceStrength(m_Alpha, m_AlphaDelta, nrhs);
	if (m_bCancel) return true;

	CreateDoubletStrength(m_Alpha, m_AlphaDelta, nrhs);
	if (m_bCancel) return true;

	ComputeFarField(1.0, m_Alpha, m_AlphaDelta, nrhs);
	if (m_bCancel) return true;


	for(int q=0; q<nrhs; q++)
		ComputeBalanceSpeeds(m_Alpha+q*m_AlphaDelta, q);


	ScaleResultstoSpeed(nrhs);
	if (m_bCancel) return true;


	ComputeOnBodyCp(m_Alpha, m_AlphaDelta, nrhs);
	if (m_bCancel) return true;

	ComputeAeroCoefs(m_Alpha, m_AlphaDelta, nrhs);

	return true;
}


void PanelAnalysisDlg::BuildInfluenceMatrix()
{
	CVector C, CC, V, VS;
	int m, mm, p, pp;
	double phi, phiSym;

	int Size = m_MatSize;
	if(m_b3DSymetric) Size = m_SymSize;

	AddString("      Creating the influence matrix...\n");

	m=0;
	for(p=0; p<m_MatSize; p++)
	{
		if(m_bCancel) return;
		if(!m_b3DSymetric || m_pPanel[p].m_bIsLeftPanel)
		{
			//for each Boundary Condition point
			if(m_pPanel[p].m_iPos!=0)
			{
				//Thick surfaces, 3D-panel type BC, use collocation point
				C = m_pPanel[p].CollPt;
			}
			else
			{
				//Thin surface, VLM type BC, use control point
				C = m_pPanel[p].CtrlPt;
			}
			CC.x =  C.x;//symmetric point, just in case
			CC.y = -C.y;
			CC.z =  C.z;

			mm = 0;
			for(pp=0; pp<m_MatSize; pp++)
			{
				if(m_bCancel) return;
				if(!m_b3DSymetric || m_pPanel[pp].m_bIsLeftPanel) 
				{
					//for each panel, get the unit doublet or vortex influence at the boundary condition pt
					GetDoubletInfluence(C, m_pPanel+pp, V, phi);

					if(m_b3DSymetric && m_pPanel[pp].m_iSym>=0) // add symmetric contribution
					{
						GetDoubletInfluence(CC, m_pPanel+pp, VS, phiSym);
						V.x += VS.x;
						V.y -= VS.y;
						V.z += VS.z;
						phi += phiSym;
					}

					if(!m_pWPolar->m_bDirichlet || m_pPanel[p].m_iPos==0) m_aij[m*Size+mm] = V.dot(m_pPanel[p].Normal);
					else if(m_pWPolar->m_bDirichlet)                      m_aij[m*Size+mm] = phi;

					mm++;
				}
			}
			m++;
		}
		m_Progress += 10.0*(double)Size/400./(double)Size;
		qApp->processEvents();
	}
}


void PanelAnalysisDlg::CreateSourceStrength(double Alpha0, double AlphaDelta, int nval)
{
	// Creates the RHS of the linear problem, using boundary conditions
	// BC may be of the Neumann or Dirichlet type depending on the analysis type and on the geometry
	//
	// if 3d-panel, use NASA 4023 equation (20) & (22)
	// 
	// Compute with a unit speed
	// The results are scaled to speed later depending on the polar type
	//
	
	int p, pp, q;
	double alpha;
	CVector QInf[VLMMAXRHS];

	AddString(tr("      Creating source strengths...")+"\n");

	p=0;
	for (q=0; q<nval;q++)
	{
		alpha = Alpha0+q*AlphaDelta;
		QInf[q].Set(cos(alpha*PI/180.0), 0.0, sin(alpha*PI/180.0));

		for (pp=0; pp< m_MatSize; pp++)
		{
			if(m_bCancel) return;
			if(m_pPanel[pp].m_iPos!=0) m_Sigma[p] = -1.0/4.0/PI* QInf[q].dot(m_pPanel[pp].Normal);
			else                       m_Sigma[p] =  0.0;
			p++;
		}
	}
}



void PanelAnalysisDlg::CreateRHS(double *RHS, CVector VInf, double *VField, bool bTrace)
{
	//Creates the RHS vector for
	//  free stream velocity VInf,
	//	rotation Angle around vector Omega
	int m, p, pp;
	double  phi, sigmapp;
	CVector V, C, VPanel;

	m = 0;

	for (p=0; p<m_MatSize; p++)
	{
		if(m_bCancel) return;
		if(VField)
		{
			VPanel.x = *(VField             +p);
			VPanel.y = *(VField+  m_MatSize +p);
			VPanel.z = *(VField+2*m_MatSize +p);
		}
		else VPanel = VInf;


		if(!m_b3DSymetric || m_pPanel[p].m_bIsLeftPanel)
		{
			if(!m_pWPolar->m_bDirichlet || m_pPanel[p].m_iPos==0)
			{
				// first term of RHS is -V.n  
				RHS[m] = - m_pPanel[p].Normal.dot(VPanel);
			}
			else if(m_pWPolar->m_bDirichlet) RHS[m] = 0.0;
			if(m_pPanel[p].m_iPos!=0) C = m_pPanel[p].CollPt;
			else                      C = m_pPanel[p].CtrlPt;

			for (pp=0; pp<m_MatSize; pp++)
			{
				// Consider only the panels positioned on thick surfaces, 
				// since the source strength is zero on thin surfaces
				if(m_pPanel[pp].m_iPos!=0)
				{
					// Define the source strength on panel pp
					sigmapp = -1.0/4.0/PI * m_pPanel[pp].Normal.dot(VPanel);

					// Add to RHS the source influence of panel pp on panel p
					GetSourceInfluence(C, m_pPanel+pp, V, phi);

					if(!m_pWPolar->m_bDirichlet || m_pPanel[p].m_iPos==0)
					{
						// Apply Neumann B.C.
						// NASA4023 eq. (22) and (23)
						// The RHS term is sigma[pp]*DJK = nj.Vjk
						RHS[m] -= V.dot(m_pPanel[p].Normal) * sigmapp;
					}
					else if(m_pWPolar->m_bDirichlet)
					{
						//NASA4023 eq. (20)
						RHS[m] -= (phi * sigmapp);
					}
				}
			}
			m++;
		}
		m_Progress += 5.0/(double)m_MatSize;
		qApp->processEvents();
	}
}



void PanelAnalysisDlg::CreateUnitRHS()
{
	//Creates the two RHS for unit speeds along x and z
	CVector VInf;

	VInf.Set(1.0, 0.0, 0.0);
	CreateRHS(m_uRHS,  VInf);
	VInf.Set(0.0, 0.0, 1.0);
	CreateRHS(m_wRHS, VInf);
}



void PanelAnalysisDlg::CreateWakeContribution()
{
	//______________________________________________________________________________________
	// Method :
	// 	- follow the method described in NASA 4023 eq. (44)
	//	- add the wake's doublet contribution to the matrix
	//	- add the difference in potential at the trailing edge panels to the RHS
	//______________________________________________________________________________________
	//
	int kw, lw, pw, p, pp, Size;

	CVector V, VS, C, CC, TrPt;
	double phi, phiSym;
	double PHC[MAXSTATIONS];
	CVector VHC[MAXSTATIONS];

	AddString(tr("      Adding the wake's contribution...")+"\n");

	if(m_b3DSymetric) Size = m_SymSize;
	else              Size = m_MatSize;

	int m, mm;
	m = mm = 0;

	for(p=0; p<m_MatSize; p++)
	{
		if(m_bCancel) return;
		if(!m_b3DSymetric || m_pPanel[p].m_bIsLeftPanel)
		{
			m_uWake[m] = m_wWake[m] = 0.0;
			C    = m_pPanel[p].CollPt;
			CC.x =  C.x;//symmetric point, just in case
			CC.y = -C.y;
			CC.z =  C.z;

			//____________________________________________________________________________
			//build the contributions of each wake column at point C
			//we have m_NWakeColum to consider
			pw=0;
			for (kw=0; kw<m_NWakeColumn; kw++)
			{
				PHC[kw] = 0.0;
				VHC[kw].Set(0.0,0.0,0.0);
				//each wake column has m_NXWakePanels
				for(lw=0; lw<m_pWPolar->m_NXWakePanels; lw++)
				{
					GetDoubletInfluence(C, m_pWakePanel+pw, V, phi, true, true);

					PHC[kw] += phi;
					VHC[kw] += V;

					if(m_b3DSymetric && m_pPanel[p].m_bIsLeftPanel)
					{
						GetDoubletInfluence(CC,  m_pWakePanel+pw, VS, phiSym, true,true);
						PHC[kw]    +=  phiSym;
						VHC[kw].x  +=  VS.x;
						VHC[kw].y  -=  VS.y;
						VHC[kw].z  +=  VS.z;
					}
					pw++;
				}
			}
			//____________________________________________________________________________
			//Add the contributions of trailing panels to the matrix coefficients and to the RHS
			mm = 0;
			for(pp=0; pp<m_MatSize; pp++) //for each matrix column
			{
				if(!m_b3DSymetric || m_pPanel[pp].m_bIsLeftPanel)
				{
					if(m_bCancel) return;
					m_aijWake[m*Size+mm] = 0.0;
					// Is the panel pp shedding a wake ?
					if(m_pPanel[pp].m_bIsTrailing)
					{
						// If so, we need to add the contributions of the wake column
						// shedded by this panel to the RHS and to the Matrix
						// Get trailing point where the jup in potential is evaluated v6.02
						TrPt = (m_pNode[m_pPanel[pp].m_iTA] + m_pNode[m_pPanel[pp].m_iTB])/2.0;

						if(m_pPanel[pp].m_iPos == 0)
						{
							//The panel shedding a wake is on a thin surface
							if(!m_pWPolar->m_bDirichlet || m_pPanel[p].m_iPos==0)
							{
								//then add the velocity contribution of the wake column to the matrix coefficient
								m_aijWake[m*Size+mm] += VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
								//we do not add the term Phi_inf_KWPUM - Phi_inf_KWPLM (eq. 44) since it is 0, thin edge
							}
							else if(m_pWPolar->m_bDirichlet)
							{
								//then add the potential contribution of the wake column to the matrix coefficient
								m_aijWake[m*Size+mm] += PHC[m_pPanel[pp].m_iWakeColumn];
								//we do not add the term Phi_inf_KWPUM - Phi_inf_KWPLM (eq. 44) since it is 0, thin edge
							}
						}
						else if(m_pPanel[pp].m_iPos == -1)
						{
							//the panel sedding a wake is on the bottom side, substract
							if(!m_pWPolar->m_bDirichlet || m_pPanel[p].m_iPos==0)
							{
								//use Neumann B.C.
								m_aijWake[m*Size+mm] -= VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
								//corrected in v6.02;
								m_uWake[m] -= TrPt.x  * VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
								m_wWake[m] -= TrPt.z  * VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
							}
							else if(m_pWPolar->m_bDirichlet)
							{
								m_aijWake[m*Size+mm] -= PHC[m_pPanel[pp].m_iWakeColumn];
								m_uWake[m] +=  TrPt.x * PHC[m_pPanel[pp].m_iWakeColumn];
								m_wWake[m] +=  TrPt.z * PHC[m_pPanel[pp].m_iWakeColumn];
							}
						}
						else if(m_pPanel[pp].m_iPos == 1)
						{
							//the panel sedding a wake is on the top side, add
							if(!m_pWPolar->m_bDirichlet || m_pPanel[p].m_iPos==0)
							{
								//use Neumann B.C.
								m_aijWake[m*Size+mm] += VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
								//corrected in v6.02;
								m_uWake[m] += TrPt.x * VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
								m_wWake[m] += TrPt.z * VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
							}
							else if(m_pWPolar->m_bDirichlet)
							{
								m_aijWake[m*Size+mm] += PHC[m_pPanel[pp].m_iWakeColumn];
								m_uWake[m] -= TrPt.x * PHC[m_pPanel[pp].m_iWakeColumn];
								m_wWake[m] -= TrPt.z * PHC[m_pPanel[pp].m_iWakeColumn];
							}
						}
					}
					mm++;
				}
			}
			m++;
		}
		m_Progress += 1.0/(double)m_MatSize;
		qApp->processEvents();
	}
}


void PanelAnalysisDlg::CreateWakeContribution(double *pWakeContrib, CVector WindDirection)
{
	//______________________________________________________________________________________
	// Method :
	// 	- follow the method described in NASA 4023 eq. (44)
	//	- add the wake's doublet contribution to the matrix
	//	- add the potential difference at the trailing edge panels to the RHS ; the potential's origin
	//    is set arbitrarily to the geometrical orgin so that phi = V.dot(WindDirectio) x point_position
	//______________________________________________________________________________________
	//
	int kw, lw, pw, p, pp, Size;

	static CVector V, VS, C, CC, TrPt;
	double phi, phiSym;
	double PHC[MAXSTATIONS];
	CVector VHC[MAXSTATIONS];

	AddString(tr("      Adding the wake's contribution...")+"\n");

	if(m_b3DSymetric) Size = m_SymSize;
	else              Size = m_MatSize;

	int m, mm;
	m = mm = 0;

	for(p=0; p<m_MatSize; p++)
	{
		if(m_bCancel) return;
		if(!m_b3DSymetric || m_pPanel[p].m_bIsLeftPanel)
		{
			pWakeContrib[m] = 0.0;
			C    = m_pPanel[p].CollPt;
			CC.x =  C.x;//symmetric point, just in case
			CC.y = -C.y;
			CC.z =  C.z;

			//____________________________________________________________________________
			//build the contributions of each wake column at point C
			//we have m_NWakeColum to consider
			pw=0;
			for (kw=0; kw<m_NWakeColumn; kw++)
			{
				PHC[kw] = 0.0;
				VHC[kw].Set(0.0,0.0,0.0);
				//each wake column has m_NXWakePanels
				for(lw=0; lw<m_pWPolar->m_NXWakePanels; lw++)
				{
					GetDoubletInfluence(C, m_pWakePanel+pw, V, phi, true, true);

					PHC[kw] += phi;
					VHC[kw] += V;

					if(m_b3DSymetric && m_pPanel[p].m_bIsLeftPanel)
					{
						GetDoubletInfluence(CC, m_pWakePanel+pw, VS, phiSym, true, true);
						PHC[kw]    +=  phiSym;
						VHC[kw].x  +=  VS.x;
						VHC[kw].y  -=  VS.y;
						VHC[kw].z  +=  VS.z;
					}
					pw++;
				}
			}
			//____________________________________________________________________________
			//Add the contributions to the matrix coefficients and to the RHS
			mm = 0;
			for(pp=0; pp<m_MatSize; pp++) //for each matrix column
			{
				if(!m_b3DSymetric || m_pPanel[pp].m_bIsLeftPanel)
				{
					if(m_bCancel) return;

					// Is the panel pp shedding a wake ?
					if(m_pPanel[pp].m_bIsTrailing)
					{
						// Get trailing point where the jup in potential is evaluated
						TrPt = (m_pNode[m_pPanel[pp].m_iTA] + m_pNode[m_pPanel[pp].m_iTB])/2.0;
						// If so, we need to add the contributions of the wake column
						// shedded by this panel to the RHS and to the Matrix
						if(m_pPanel[pp].m_iPos == 0)
						{
							//The panel shedding a wake is on a thin surface
							if(!m_pWPolar->m_bDirichlet || m_pPanel[p].m_iPos==0)
							{
								//then add the velocity contribution of the wake column to the matrix coefficient
								//we do not add the term Phi_inf_KWPUM - Phi_inf_KWPLM (eq. 44) since it is 0, thin edge
							}
							else if(m_pWPolar->m_bDirichlet)
							{
								//then add the potential contribution of the wake column to the matrix coefficient
								//we do not add the term Phi_inf_KWPUM - Phi_inf_KWPLM (eq. 44) since it is 0, thin edge
							}
						}
						else if(m_pPanel[pp].m_iPos == -1)//bottom side, substract
						{
							//evaluate the potential on the bottom side panel pp which is shedding a wake
							if(!m_pWPolar->m_bDirichlet || m_pPanel[p].m_iPos==0)
							{
								//use Neumann B.C.
								pWakeContrib[m] -= TrPt.dot(WindDirection) * VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
							}
							else if(m_pWPolar->m_bDirichlet)
							{
								pWakeContrib[m] += TrPt.dot(WindDirection) * PHC[m_pPanel[pp].m_iWakeColumn];
							}
						}
						else if(m_pPanel[pp].m_iPos == 1)  //top side, add
						{
							//evaluate the potential on the top side panel pp which is shedding a wake
							if(!m_pWPolar->m_bDirichlet || m_pPanel[p].m_iPos==0)
							{
								//use Neumann B.C.
								pWakeContrib[m] += TrPt.dot(WindDirection) * VHC[m_pPanel[pp].m_iWakeColumn].dot(m_pPanel[p].Normal);
							}
							else if(m_pWPolar->m_bDirichlet)
							{
								pWakeContrib[m] -= TrPt.dot(WindDirection) * PHC[m_pPanel[pp].m_iWakeColumn];
							}
						}
					}
					mm++;
				}
			}
			m++;
		}
		m_Progress += 1.0/(double)m_MatSize;
		qApp->processEvents();
	}
}


void PanelAnalysisDlg::ComputeFarField(double QInf, double Alpha0, double AlphaDelta, int nval)
{
	//______________________________________________________________________________________
	// Method : 
	// 	First calculate the Cp coefficients
	// 	Deduce the lift for unit speed
	// 	Calculate the speeds i.a.w. the polar's type
	// 	Scale the doublet and source strength i.a.w. the speeds
	//______________________________________________________________________________________

	QString strong;
	int i, q, pos;
	double alpha, Lift, IDrag, *Mu, *Sigma;
	double ThinSize = 0.0;
	CVector WindNormal, Force, WingForce;

	//______________________________________________________________________________________
	// Scale the speeds i.a.w. the polar's type

	AddString(tr("      Calculating aerodynamic coefficients in the far field plane")+"\n");

	for(i=0; i<4; i++)
	{
		if(m_pWingList[i]) ThinSize += (double)m_pWingList[i]->m_MatSize;
	}

//	CWing::s_Viscosity = m_pWPolar->m_Viscosity;
//	CWing::s_Density   = m_pWPolar->m_Density;

	for (q=0; q<nval;q++)
	{
		Force.Set(0.0,0.0,0.0);
		if(m_pWPolar->m_Type!=FIXEDAOAPOLAR)
		{
			alpha = Alpha0 + q*AlphaDelta;
		}
		else alpha = m_OpAlpha;
		WindNormal.Set(-sin(alpha*PI/180.0),   0.0, cos(alpha*PI/180.0));
		Lift = 0.0;
		pos = 0;
		Mu     = m_Mu    + q*m_MatSize;
		Sigma  = m_Sigma + q*m_MatSize;

		strong = tr("        Calculating point ") + QString("%1").arg(alpha,7,'f',2)+QString::fromUtf8("°....\n");
		AddString(strong);

		for(i=0; i<4; i++)
		{
			if(m_pWingList[i])
			{
				WingForce.Set(0.0, 0.0, 0.0);
				m_pWingList[i]->PanelTrefftz(QInf, alpha, Mu, Sigma, pos, WingForce, IDrag, m_pWPolar, m_pWakePanel, m_pWakeNode);
				m_WingForce[q*4+i] = WingForce;

				Force             += WingForce;
				//save the results... will save another FF calculation when computing operating point
				m_WingIDrag[q*4+i] = IDrag;

				memcpy(m_Cl  + q*4*MAXSTATIONS + i*MAXSTATIONS, m_pWingList[i]->m_Cl,  m_pWingList[i]->m_NStation*sizeof(double));
				memcpy(m_ICd + q*4*MAXSTATIONS + i*MAXSTATIONS, m_pWingList[i]->m_ICd, m_pWingList[i]->m_NStation*sizeof(double));
				memcpy(m_Ai  + q*4*MAXSTATIONS + i*MAXSTATIONS, m_pWingList[i]->m_Ai,  m_pWingList[i]->m_NStation*sizeof(double));
				memcpy(m_F   + q*4*MAXSTATIONS + i*MAXSTATIONS, m_pWingList[i]->m_F,   m_pWingList[i]->m_NStation*sizeof(CVector));
				memcpy(m_Vd  + q*4*MAXSTATIONS + i*MAXSTATIONS, m_pWingList[i]->m_Vd,  m_pWingList[i]->m_NStation*sizeof(CVector));

				pos += m_pWingList[i]->m_MatSize;

				m_Progress += 10.0 * (double)m_pWingList[i]->m_MatSize/ThinSize *(double)m_MatSize/400.;
				qApp->processEvents();
				if(m_bCancel)return;
			}
		}
	}

	AddString("\n");
}



void PanelAnalysisDlg::ComputeBalanceSpeeds(double Alpha, int q)
{
	QString strong, strange;
	CVector Force, WindNormal;
	double TempCl,Lift;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	WindNormal.Set(-sin(Alpha*PI/180.0),   0.0, cos(Alpha*PI/180.0));

	Force.Set(0.0,0.0,0.0);

	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pWingList[iw]) Force += m_WingForce[q*4+iw];
	}

	if (m_pWPolar->m_Type==FIXEDSPEEDPOLAR) m_3DQInf[q] = m_pWPolar->m_QInf;

	else if(m_pWPolar->m_Type==FIXEDLIFTPOLAR)
	{
		Lift =  Force.dot(WindNormal) ;//N/q, for 1/ms
		TempCl = Lift/m_pWPolar->m_WArea;
		if(Lift<=0.0)
		{
			strong = "           "+QString(tr("Found a negative lift for Alpha=%1.... skipping the angle...")+"\n").arg(Alpha, 5,'f',2);
			AddString(strong);
			m_bPointOut = true;
			m_bWarning  = true;
			m_3DQInf[q] = -100.0;
		}
		else
		{
			m_3DQInf[q] =  sqrt(2.0* 9.81 * m_pWPolar->m_Mass/m_pWPolar->m_Density/TempCl/m_pWPolar->m_WArea);
			strong = QString("           Alpha=%1   QInf=%2").arg(Alpha, 5,'f',2).arg(m_3DQInf[q]*pMainFrame->m_mstoUnit,5,'f',2);
			GetSpeedUnit(strange, pMainFrame->m_SpeedUnit);
			strong+= strange + "\n";
			AddString(strong);
		}
	}
}


void PanelAnalysisDlg::ScaleResultstoSpeed(int nval)
{
	int p, q, pp, i;
	//______________________________________________________________________________________
	// Scale RHS and Sigma i.a.w. speeds (so far we have unit doublet and source strengths)

	QString strong="\n";
	AddString(strong);

	static double SigmaRef[VLMMAXMATSIZE * VLMMAXRHS];
	memcpy(SigmaRef, m_Sigma, nval*m_MatSize*sizeof(double));
	memcpy(m_RHSRef, m_Mu,    nval*m_MatSize*sizeof(double));

	if(m_pWPolar->m_Type!=FIXEDAOAPOLAR)
	{
		p=0;
		for (q=0; q<nval;q++)
		{
			for(pp=0; pp<m_MatSize; pp++)
			{
				m_Mu[p]     *= m_3DQInf[q];
				m_Sigma[p]  *= m_3DQInf[q];
				p++;
			}
		}
	}
	else
	{
		//type 4, we scale the first single rhs for all specified speed values
		p=0;
		for (q=0; q<nval;q++)
		{
			for(pp=0; pp<m_MatSize; pp++)
			{
				m_Mu[p]    = m_RHSRef[pp] * m_3DQInf[q];
				m_Sigma[p] = SigmaRef[pp] * m_3DQInf[q];
				p++;
			}
		}
	}

	//scale the strip force and downwash fields
	for (q=0; q<nval;q++)
	{
		for(i=0; i<4; i++)
		{
			if(m_pWingList[i])
			{
				for(int m=0; m<m_pWingList[i]->m_NStation; m++)
				{
					m_F[ m+ q*4*MAXSTATIONS + i*MAXSTATIONS] *= m_3DQInf[q] * m_3DQInf[q];
					m_Vd[m+ q*4*MAXSTATIONS + i*MAXSTATIONS] *= m_3DQInf[q] * m_3DQInf[q];
				}
			}
		}
	}
}


void PanelAnalysisDlg::ComputeAeroCoefs(double V0, double VDelta, int nrhs)
{
	// Calculates the viscous and inviscid wing coefficients 
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int q;
	QString str, strong;

	if(m_pWPolar->m_Type !=FIXEDAOAPOLAR)
	{
		for (q=0; q<nrhs; q++)
		{
			if(m_bCancel) return;
			if(m_3DQInf[q]>0.0)
			{
				if(!m_pWPolar->m_bTiltedGeom) str = QString(tr("      Computing Plane for alpha=%1")).arg(V0+q*VDelta,7,'f',2);
				else                          str = QString(tr("      Computing Plane for alpha=%1")).arg(m_OpAlpha,7,'f',2);
				str += QString::fromUtf8("°\n");
				AddString(str);
				ComputePlane(V0+q*VDelta, m_3DQInf[q], q);
			}
			m_Progress += 5.0*(double)nrhs/(double)nrhs;
			qApp->processEvents();
		}
	}
	else
	{
		for (q=0; q<nrhs; q++)
		{
			if(m_bCancel) return;
			GetSpeedUnit(strong, pMainFrame->m_SpeedUnit);
			str = QString(tr("      Computing Plane for QInf=%1")).arg((V0+q*VDelta)*pMainFrame->m_mstoUnit,7,'f',2);
			str += strong+"\n";
			AddString(str);
			ComputePlane(m_Alpha, V0+q*VDelta, q);
			m_Progress += 5.0*(double)nrhs /(double)nrhs;
			qApp->processEvents();
		}
	}
}


void PanelAnalysisDlg::ComputePlane(double Alpha, double QInf, int qrhs)
{
	// calculates the viscous and inviscid wing coefficients 
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	int pos;
	double *Mu, *Sigma;
	double cosa, sina;
	double Lift, IDrag, VDrag ,XCP, YCP, WingVDrag;
	CVector WindNormal, WindDirection, WindSide;
	CVector Force;
	QString OutString;
	//   Define wind (stability) axis
	cosa = cos(Alpha*PI/180.0);
	sina = sin(Alpha*PI/180.0);

	WindNormal.Set(  -sina, 0.0, cosa);
	WindDirection.Set(cosa, 0.0, sina);
	WindSide = WindNormal * WindDirection;
	
	Mu     = m_Mu    + qrhs*m_MatSize;
	Sigma  = m_Sigma + qrhs*m_MatSize;

	m_QInf      = QInf;

	if(m_pWPolar->m_bTiltedGeom) Alpha     = m_OpAlpha;
	else                         m_OpAlpha = Alpha;

//	CWing::s_bTrace	   = true;
//	CWing::s_Viscosity = m_pWPolar->m_Viscosity;
//	CWing::s_Density   = m_pWPolar->m_Density;

	for(int iw=0; iw<MAXWINGS; iw++)
		if(m_pWingList[iw])m_pWingList[iw]->m_bWingOut = false;
	m_pWing->m_bWingOut                = false;

	if(QInf >0.0) 
	{
		AddString(tr("       Calculating aerodynamic coefficients...")+"\n");
		m_bPointOut = false;
		Force.Set(0.0, 0.0, 0.0);

		Lift = IDrag = VDrag = XCP = YCP = 0.0;
		m_GCm = m_VCm = m_ICm = m_GRm = m_GYm = m_VYm = m_IYm = 0.0;

		pos = 0;

		for(int iw=0; iw<4; iw++)
		{
			if(m_pWingList[iw])
			{
				AddString(tr("         Calculating wing...") + m_pWingList[iw]->m_WingName+"\n");
				//restore the saved FF results
				Force += m_WingForce[qrhs*4+iw];
				IDrag += m_WingIDrag[qrhs*4+iw];

				memcpy(m_pWingList[iw]->m_Cl,  m_Cl  + qrhs*4*MAXSTATIONS + iw*MAXSTATIONS, m_pWingList[iw]->m_NStation*sizeof(double));
				memcpy(m_pWingList[iw]->m_ICd, m_ICd + qrhs*4*MAXSTATIONS + iw*MAXSTATIONS, m_pWingList[iw]->m_NStation*sizeof(double));
				memcpy(m_pWingList[iw]->m_Ai,  m_Ai  + qrhs*4*MAXSTATIONS + iw*MAXSTATIONS, m_pWingList[iw]->m_NStation*sizeof(double));
				memcpy(m_pWingList[iw]->m_F,   m_F   + qrhs*4*MAXSTATIONS + iw*MAXSTATIONS, m_pWingList[iw]->m_NStation*sizeof(CVector));
				memcpy(m_pWingList[iw]->m_Vd,  m_Vd  + qrhs*4*MAXSTATIONS + iw*MAXSTATIONS, m_pWingList[iw]->m_NStation*sizeof(CVector));

				//Get viscous interpolations
				m_pWingList[iw]->PanelComputeViscous(QInf, Alpha, m_pWPolar, WingVDrag, m_pWPolar->m_bViscous, OutString);
				VDrag += WingVDrag;

				AddString(OutString);
				if(m_pWingList[iw]->m_bWingOut)  m_bPointOut = true;

				//Compute moment coefficients
				m_pWingList[iw]->PanelComputeOnBody(QInf, Alpha, m_Cp+qrhs*m_MatSize+pos, m_Mu+qrhs*m_MatSize+pos,
				                                   XCP, YCP, m_GCm, m_VCm, m_ICm, m_GRm, m_GYm, m_VYm, m_IYm, 
											m_pWPolar, m_pWPolar->m_CoG);


				m_pWingList[iw]->PanelSetBending(m_pWPolar->m_bThinSurfaces);

				pos += m_pWingList[iw]->m_MatSize;
			}
		}

		if(m_pBody && m_pWPolar->m_AnalysisMethod==PANELMETHOD)
		{
			AddString(tr("       Calculating body...")+"\n");
			double ICm = 0.0;
			m_pBody->ComputeAero(m_Cp+qrhs*m_MatSize+pos, XCP, YCP, ICm, m_GRm, m_GYm, Alpha, m_pWPolar->m_CoG);
			m_ICm += ICm;
			m_GCm += ICm;

			//the body does not shed any wake --> no induced lift or drag
		}

		if(!m_bTrefftz) SumPanelForces(m_Cp+qrhs*m_MatSize, Alpha, Lift, IDrag);

		m_CL          =       Force.dot(WindNormal)    /m_pWPolar->m_WArea;
		m_CX          =       Force.dot(WindDirection) /m_pWPolar->m_WArea;
		m_CY          =       Force.dot(WindSide)      /m_pWPolar->m_WArea;
		m_InducedDrag =  1.0*IDrag/m_pWPolar->m_WArea;
		m_ViscousDrag =  1.0*VDrag/m_pWPolar->m_WArea;

		if(fabs(Force.dot(WindNormal))>0.0)
		{
			m_XCP         = XCP/Force.dot(WindNormal);
			m_YCP         = YCP/Force.dot(WindNormal);
		}
		else
		{
			m_XCP         = 0.0;
			m_YCP         = 0.0;
		}

		m_GCm *= 1.0 / m_pWPolar->m_WArea /m_pWing->m_MAChord;
		m_VCm *= 1.0 / m_pWPolar->m_WArea /m_pWing->m_MAChord;
		m_ICm *= 1.0 / m_pWPolar->m_WArea /m_pWing->m_MAChord;

		m_GRm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan;

		m_GYm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan;
		m_VYm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan;
		m_IYm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan;

		if(m_bPointOut) m_bWarning = true;

		if(m_pWPolar->m_Type==STABILITYPOLAR) m_Alpha = m_AlphaEq; // so it is set by default at the end of the analyis
		if(m_pPlane) pMiarex->AddPOpp(m_bPointOut, m_Cp+qrhs*m_MatSize, Mu, Sigma);
		else         pMiarex->AddWOpp(QInf, Alpha, m_bPointOut, Mu, Sigma, m_Cp+qrhs*m_MatSize);

		if(pMiarex->m_iView==WPOLARVIEW)
		{
			pMiarex->CreateWPolarCurves();
			pMiarex->UpdateView();
		}
		else if(pMiarex->m_iView==WSTABVIEW && pMiarex->m_iStabilityView==STABPOLARVIEW)
		{
			pMiarex->CreateStabRLCurves();
			pMiarex->UpdateView();
		}

		AddString("\n");
	}
	else m_bPointOut = true;

	qApp->processEvents();
}


void PanelAnalysisDlg::GetVortexCp(const int &p, double *Gamma, double *Cp, CVector &VInf)
{
	static CVector PanelForce, Force;
	// for each panel along the chord, add the lift coef
	PanelForce  = VInf * m_pPanel[p].Vortex;
	PanelForce *= Gamma[p] * m_pWPolar->m_Density;                 //Newtons

	if(!m_pWPolar->m_bVLM1 && !m_pPanel[p].m_bIsLeading)
	{
		Force       = VInf* m_pPanel[p].Vortex;
		Force      *= Gamma[p+1] * m_pWPolar->m_Density;       //Newtons
		PanelForce -= Force;
	}

	Cp[p]  = -2.0 * PanelForce.dot(m_pPanel[p].Normal) /m_pPanel[p].Area/m_pWPolar->m_Density;
}


void PanelAnalysisDlg::GetDoubletDerivative(const int &p, double *Mu, double &Cp, CVector &VLocal, double const &QInf, double Vx, double Vy, double Vz)
{
	static int PL,PR, PU, PD;
	static double DELQ, DELP, mu0,mu1,mu2, x0,x1,x2, Speed2;
	static CVector VTot;//total local panel speed
	static CVector S2, Sl2;

	PL = m_pPanel[p].m_iPL;
	PR = m_pPanel[p].m_iPR;
	PU = m_pPanel[p].m_iPU;
	PD = m_pPanel[p].m_iPD;

	if(PL>=0 && PR>=0)
	{
		//we have two side neighbours
		x1  = 0.0;
		x0  = x1 - m_pPanel[p].SMQ - m_pPanel[PL].SMQ;
		x2  = x1 + m_pPanel[p].SMQ + m_pPanel[PR].SMQ;
		mu0 = Mu[PL];
		mu1 = Mu[p];
		mu2 = Mu[PR];
		DELQ =	  mu0 *(x1-x2)       /(x0-x1)/(x0-x2)
				+ mu1 *(2.0*x1-x0-x2)/(x1-x0)/(x1-x2)
				+ mu2 *(x1-x0)       /(x2-x0)/(x2-x1);
	}
	else if(PL>=0 && PR<0)
	{
		// no right neighbour
		// do we have two left neighbours ?
		if(m_pPanel[PL].m_iPL>=0){
			x2  = 0.0;
			x1  = x2 - m_pPanel[p].SMQ  - m_pPanel[PL].SMQ;
			x0  = x1 - m_pPanel[PL].SMQ - m_pPanel[m_pPanel[PL].m_iPL].SMQ;

			mu0 = Mu[m_pPanel[PL].m_iPL];
			mu1 = Mu[PL];
			mu2 = Mu[p];
			DELQ =	  mu0 *(x2-x1)       /(x0-x1)/(x0-x2)
					+ mu1 *(x2-x0)       /(x1-x0)/(x1-x2)
					+ mu2 *(2.0*x2-x0-x1)/(x2-x0)/(x2-x1);
		}
		else
		{
			//calculate the derivative on two panels only
			DELQ = -(Mu[PL]-Mu[p])/(m_pPanel[p].SMQ  + m_pPanel[PL].SMQ);
		}
	}
	else if(PL<0 && PR>=0)
	{
		// no left neighbour
		// do we have two right neighbours ?
		if(m_pPanel[PR].m_iPR>=0){
			x0  = 0.0;
			x1  = x0 + m_pPanel[p].SMQ  + m_pPanel[PR].SMQ;
			x2  = x1 + m_pPanel[PR].SMQ + m_pPanel[m_pPanel[PR].m_iPR].SMQ;
			mu0 = Mu[p];
			mu1 = Mu[PR];
			mu2 = Mu[m_pPanel[PR].m_iPR];
			DELQ =	  mu0 *(2.0*x0-x1-x2)/(x0-x1)/(x0-x2)
					+ mu1 *(x0-x2)       /(x1-x0)/(x1-x2)
					+ mu2 *(x0-x1)       /(x2-x0)/(x2-x1);
		}
		else
		{
			//calculate the derivative on two panels only
			DELQ = (Mu[PR]-Mu[p])/(m_pPanel[p].SMQ  + m_pPanel[PR].SMQ);
		}
	}
	else
	{
		DELQ = 0.0;
		//Cannot calculate a derivative on one panel only
	}

	if(PU>=0 && PD>=0)
	{
		//we have one upstream and one downstream neighbour
		x1  = 0.0;
		x0  = x1 - m_pPanel[p].SMP - m_pPanel[PU].SMP;
		x2  = x1 + m_pPanel[p].SMP + m_pPanel[PD].SMP;
		mu0 = Mu[PU];
		mu1 = Mu[p];
		mu2 = Mu[PD];
		DELP =	  mu0 *(x1-x2)       /(x0-x1)/(x0-x2)
				+ mu1 *(2.0*x1-x0-x2)/(x1-x0)/(x1-x2)
				+ mu2 *(x1-x0)       /(x2-x0)/(x2-x1);
	}
	else if(PU>=0 && PD<0)
	{
		// no downstream neighbour
		// do we have two upstream neighbours ?
		if(m_pPanel[PU].m_iPU>=0)
		{
			x2  = 0.0;
			x1  = x2 - m_pPanel[p ].SMP  - m_pPanel[PU].SMP;
			x0  = x1 - m_pPanel[PU].SMP  - m_pPanel[m_pPanel[PU].m_iPU].SMP;
			mu0 = Mu[m_pPanel[PU].m_iPU];
			mu1 = Mu[PU];
			mu2 = Mu[p];
			DELP =	  mu0 *(x2-x1)       /(x0-x1)/(x0-x2)
					+ mu1 *(x2-x0)       /(x1-x0)/(x1-x2)
					+ mu2 *(2.0*x2-x0-x1)/(x2-x0)/(x2-x1);
		}
		else
		{
			//calculate the derivative on two panels only
			DELP = -(Mu[PU]-Mu[p])/(m_pPanel[p].SMP  + m_pPanel[PU].SMP);
		}
	}
	else if(PU<0 && PD>=0)
	{
		// no upstream neighbour
		// do we have two downstream neighbours ?
		if(m_pPanel[PD].m_iPD>=0)
		{
			x0  = 0.0;
			x1  = x0 + m_pPanel[p].SMP  + m_pPanel[PD].SMP;
			x2  = x1 + m_pPanel[PD].SMP + m_pPanel[m_pPanel[PD].m_iPD].SMP;
			mu0 = Mu[p];
			mu1 = Mu[PD];
			mu2 = Mu[m_pPanel[PD].m_iPD];
			DELP =	  mu0 *(2.0*x0-x1-x2)/(x0-x1)/(x0-x2)
					+ mu1 *(x0-x2)       /(x1-x0)/(x1-x2)
					+ mu2 *(x0-x1)       /(x2-x0)/(x2-x1);
		}
		else
		{
			//calculate the derivative on two panels only
			DELP = (Mu[PD]-Mu[p])/(m_pPanel[p].SMP  + m_pPanel[PD].SMP);
		}
	}
	else
	{
		DELP = 0.0;
	}

	//find middle of side 2
	S2 = (m_pNode[m_pPanel[p].m_iTA] + m_pNode[m_pPanel[p].m_iTB])/2.0 - m_pPanel[p].CollPt;
	//convert to local coordinates
	Sl2   = m_pPanel[p].GlobalToLocal(S2);
	VTot  = m_pPanel[p].GlobalToLocal(Vx, Vy, Vz);

	//in panel referential
	VLocal.x = -4.0*PI*(m_pPanel[p].SMP*DELP - Sl2.y*DELQ)/Sl2.x;
	VLocal.y = -4.0*PI*DELQ;
//	Vl.z =  4.0*PI*Sigma[p];

	VTot +=VLocal;
	VTot.z = 0.0;

	Speed2 = VTot.x*VTot.x + VTot.y*VTot.y;

	Cp  = 1.0-Speed2/QInf/QInf;
}



void PanelAnalysisDlg::ComputeOnBodyCp(double V0, double VDelta, int nval)
{
	//following VSAERO theory manual
	//the on-body tangential perturbation speed is the derivative of the doublet strength
	int p, q;
	static double Alpha, *Mu, * Sigma, *Cp;
	static CVector Qp, VInf, VLocal;
	double Speed2, cosa, sina;

	//______________________________________________________________________________________
	AddString(tr("      Computing On-Body Speeds...")+"\n");

	if(m_pWPolar->m_Type != FIXEDAOAPOLAR)
	{
		for (q=0; q<nval; q++)
		{
			//   Define wind axis
			Alpha = V0 + (double)q * VDelta;
			cosa = cos(Alpha*PI/180.0);
			sina = sin(Alpha*PI/180.0);
			Qp.Set(cosa, 0.0, sina);
			VInf = Qp * m_3DQInf[q];

			Mu     = m_Mu    + q * m_MatSize;
			Sigma  = m_Sigma + q * m_MatSize;
			Cp     = m_Cp    + q * m_MatSize;

			for (p=0; p<m_MatSize; p++)
			{
				if(m_pPanel[p].m_iPos!=0)
				{
					VLocal  = m_pPanel[p].GlobalToLocal(VInf);
					VLocal += m_uVl[p]*cosa*m_3DQInf[q] + m_wVl[p]*sina*m_3DQInf[q];
					Speed2 = VLocal.x*VLocal.x + VLocal.y*VLocal.y;
					Cp[p]  = 1.0-Speed2/m_3DQInf[q]/m_3DQInf[q];
				}
				else GetVortexCp(p, Mu, Cp, Qp);

				if(m_bCancel) return;
			}
			if(m_bCancel) return;
			m_Progress += 1.0 *(double)nval/(double)nval;
			qApp->processEvents();
		}
	}
	else
	{
		//   Define wind axis
		Qp.Set(cos(m_Alpha*PI/180.0), 0.0, sin(m_Alpha*PI/180.0));
		for (q=0; q<nval; q++)
		{
			VInf = Qp * m_3DQInf[q];

			Mu     = m_Mu    + q * m_MatSize;
			Sigma  = m_Sigma + q * m_MatSize;
			Cp     = m_Cp    + q * m_MatSize;

			for (p=0; p<m_MatSize; p++)
			{
				if(m_bCancel) break;

				if(m_pPanel[p].m_iPos!=0) GetDoubletDerivative(p, Mu, Cp[p], VLocal, m_3DQInf[q], VInf.x, VInf.y, VInf.z);
				else                      GetVortexCp(p, Mu, Cp, Qp);
			}

			m_Progress += 1.0  *(double)nval/(double)nval;
			qApp->processEvents();
		}
		for (q=1; q<nval; q++)
		{
			if(m_bCancel) return;
			for (p=0; p<m_MatSize; p++)
			{
				m_Cp[p+q*m_MatSize] = m_Cp[p];
			}
		}
	}
	AddString("\n");
}



void PanelAnalysisDlg::ComputeSurfSpeeds(double *Mu, double *Sigma)
{
	int p;
	CVector C;
	CVector Q(m_3DQInf[0]*cos(m_Alpha*PI/180.0),0.0,m_3DQInf[0]*sin(m_Alpha*PI/180.0));

	for (p=0; p<m_MatSize; p++)
	{
		if(m_bCancel) return;
		C = m_pPanel[p].CollPt;//+ m_pPanel[p].Normal*m_pPanel[p].Size/100.0;
		C += m_pPanel[p].Normal*0.001;

		GetSpeedVector(C, Mu, Sigma, m_Speed[p]);
		m_Speed[p] += Q;

	}
}





void PanelAnalysisDlg::DoubletNASA4023(CVector const &C, CPanel *pPanel, CVector &V, double &phi, bool bWake)
{
	// VSAERO theory Manual
	// Influence of panel pp at coll pt of panel p
	// vectorial operations are written inline to save computing times
	// -->longer code, but 4x more efficient....
	int i;
	double CoreSize = 0.00000;
	if(fabs(*m_pCoreSize)>1.e-10) CoreSize = *m_pCoreSize;
	CVector *pNode;
	if(bWake)	pNode = m_pWakeNode;
	else		pNode = m_pNode;

	phi = 0.0;

	V.x=0.0; V.y=0.0; V.z=0.0;

	PJK.x = C.x - pPanel->CollPt.x;
	PJK.y = C.y - pPanel->CollPt.y;
	PJK.z = C.z - pPanel->CollPt.z;

	PN  = PJK.x*pPanel->Normal.x + PJK.y*pPanel->Normal.y + PJK.z*pPanel->Normal.z;
	pjk = sqrt(PJK.x*PJK.x + PJK.y*PJK.y + PJK.z*PJK.z);

	if(pjk> RFF*pPanel->Size)
	{
		// use far-field formula
		phi = PN * pPanel->Area /pjk/pjk/pjk;
		T1.x =PJK.x*3.0*PN - pPanel->Normal.x*pjk*pjk;
		T1.y =PJK.y*3.0*PN - pPanel->Normal.y*pjk*pjk;
		T1.z =PJK.z*3.0*PN - pPanel->Normal.z*pjk*pjk;
		V.x   = T1.x * pPanel->Area /pjk/pjk/pjk/pjk/pjk;
		V.y   = T1.y * pPanel->Area /pjk/pjk/pjk/pjk/pjk;
		V.z   = T1.z * pPanel->Area /pjk/pjk/pjk/pjk/pjk;
		return;
	}

	if(pPanel->m_iPos>=0)
	{
		m_pR[0] = pNode + pPanel->m_iLA;
		m_pR[1] = pNode + pPanel->m_iTA;
		m_pR[2] = pNode + pPanel->m_iTB;
		m_pR[3] = pNode + pPanel->m_iLB;
		m_pR[4] = pNode + pPanel->m_iLA;
	}
	else
	{
		m_pR[0] = pNode + pPanel->m_iLB;
		m_pR[1] = pNode + pPanel->m_iTB;
		m_pR[2] = pNode + pPanel->m_iTA;
		m_pR[3] = pNode + pPanel->m_iLA;
		m_pR[4] = pNode + pPanel->m_iLB;
	}

	for (i=0; i<4; i++)
	{
		a.x  = C.x - m_pR[i]->x;
		a.y  = C.y - m_pR[i]->y;
		a.z  = C.z - m_pR[i]->z;
		b.x  = C.x - m_pR[i+1]->x;
		b.y  = C.y - m_pR[i+1]->y;
		b.z  = C.z - m_pR[i+1]->z;
		s.x  = m_pR[i+1]->x - m_pR[i]->x;
		s.y  = m_pR[i+1]->y - m_pR[i]->y;
		s.z  = m_pR[i+1]->z - m_pR[i]->z;
		A    = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
		B    = sqrt(b.x*b.x + b.y*b.y + b.z*b.z);
		SM   = s.x*pPanel->m.x + s.y*pPanel->m.y + s.z*pPanel->m.z;
		SL   = s.x*pPanel->l.x + s.y*pPanel->l.y + s.z*pPanel->l.z;
		AM   = a.x*pPanel->m.x + a.y*pPanel->m.y + a.z*pPanel->m.z;
		AL   = a.x*pPanel->l.x + a.y*pPanel->l.y + a.z*pPanel->l.z;
		Al   = AM*SL - AL*SM;
		PA   = PN*PN*SL + Al*AM;
		PB   = PA - Al*SM;

		//get the distance of the TestPoint to the panel's side
		h.x =  a.y*s.z - a.z*s.y;
		h.y = -a.x*s.z + a.z*s.x;
		h.z =  a.x*s.y - a.y*s.x;

		//first the potential
		if(m_pR[i]->IsSame(*m_pR[i+1]))
		{
			CJKi = 0.0;
			//no contribution to speed either
		}
		else if ((((h.x*h.x+h.y*h.y+h.z*h.z)/(s.x*s.x+s.y*s.y+s.z*s.z) <= CoreSize*CoreSize) && a.x*s.x+a.y*s.y+a.z*s.z>=0.0  && b.x*s.x+b.y*s.y+b.z*s.z<=0.0)
				 ||  A < CoreSize || B < CoreSize)
		{
			CJKi = 0.0;//speed is singular at panel edge, the value of the potential is unknown
		}
		else
		{
			RNUM = SM*PN * (B*PA-A*PB);
			DNOM = PA*PB + PN*PN*A*B*SM*SM;
			if(fabs(PN)<eps)
			{
				// side is >0 if on the panel's right side
				side = pPanel->Normal.x*h.x +pPanel->Normal.y*h.y +pPanel->Normal.z*h.z;

				if(side >=0.0) sign = 1.0; else sign = -1.0;
				if(DNOM<0.0)
				{
					if(PN>0.0)	CJKi =  PI * sign;
					else		CJKi = -PI * sign;
				}
				else if(DNOM == 0.0)
				{
					if(PN>0.0)	CJKi =  PI/2.0 * sign;
					else		CJKi = -PI/2.0 * sign;
				}
				else
					CJKi = 0.0;
			}
			else
			{
				CJKi = atan2(RNUM,DNOM);
			}
			// next the induced velocity
			h.x =  a.y*b.z - a.z*b.y;
			h.y = -a.x*b.z + a.z*b.x;
			h.z =  a.x*b.y - a.y*b.x;
			GL = ((A+B) /A/B/ (A*B + a.x*b.x+a.y*b.y+a.z*b.z));
			V.x += h.x * GL;
			V.y += h.y * GL;
			V.z += h.z * GL;
		}
		phi += CJKi;

	}
	if (( (C.x-pPanel->CollPt.x)*(C.x-pPanel->CollPt.x)
		 +(C.y-pPanel->CollPt.y)*(C.y-pPanel->CollPt.y)
		 +(C.z-pPanel->CollPt.z)*(C.z-pPanel->CollPt.z))<1.e-10)
	{
//		if(m_R[0]->IsSame(*m_R[1]) || m_R[1]->IsSame(*m_R[2]) || m_R[2]->IsSame(*m_R[3]) || m_R[3]->IsSame(*m_R[0]))
//			phi = -3.0*pi/2.0;
//		else
			phi  = -2.0*PI;
	}
}


void PanelAnalysisDlg::GetDoubletInfluence(CVector const &C, CPanel *pPanel, CVector &V, double &phi, bool bWake, bool bAll)
{
	// returns the influence of the panel pPanel at point C
	// if the panel pPanel is located on a thin surface, then its the influence of a vortex
	// if it is on a thick surface, then its a doublet

	if(pPanel->m_iPos!=0 || pPanel->m_bIsWakePanel)	DoubletNASA4023(C, pPanel, V, phi, bWake);
	else
	{
		VLMGetVortexInfluence(pPanel, C, V, bAll);
		phi = 0.0;
	}

	if(m_pWPolar->m_bGround) 
	{
		CG.Set(C.x, C.y, -C.z-2.0*m_pWPolar->m_Height);

		if(pPanel->m_iPos!=0 || pPanel->m_bIsWakePanel)	DoubletNASA4023(C, pPanel, VG, phiG, bWake);
		else
		{
			VLMGetVortexInfluence(pPanel, C, VG, bAll);
			phiG = 0.0;
		}
		V.x += VG.x;
		V.y += VG.y;
		V.z -= VG.z;
		phi += phiG;
	}
}


void PanelAnalysisDlg::GetSourceInfluence(CVector const &C, CPanel *pPanel, CVector &V, double &phi)
{
	// returns the influence of a uniform source distribution on the panel pPanel at point C
	// The panel is necessarily located on a thick surface, else the source strength is zero

	SourceNASA4023(C, pPanel, V, phi);

	if(m_pWPolar->m_bGround) 
	{
		CG.Set(C.x, C.y, -C.z-2.0*m_pWPolar->m_Height);
		SourceNASA4023(CG, pPanel, VG, phiG);
		V.x += VG.x;
		V.y += VG.y;
		V.z -= VG.z;
		phi += phiG;
	}
}


void PanelAnalysisDlg::GetSpeedVector(CVector const &C, double *Mu, double *Sigma, CVector &VT, bool bAll)
{
	CVector V;
	int pp, pw, lw;
	double phi, sign;

	VT.Set(0.0,0.0,0.0);

	for (pp=0; pp<m_MatSize;pp++)
	{
		if(m_bCancel) return;

		if(m_pPanel[pp].m_iPos!=0) //otherwise Sigma[pp] =0.0, so contribution is zero also
		{
			GetSourceInfluence(C, m_pPanel+pp, V, phi);
			VT += V * Sigma[pp] ;
		}
		GetDoubletInfluence(C, m_pPanel+pp, V, phi, false, bAll);

		VT += V * Mu[pp];

		// Is the panel pp shedding a wake ?
		if(m_pPanel[pp].m_bIsTrailing && m_pPanel[pp].m_iPos!=0)
		{
			//If so, we need to add the contribution of the wake column shedded by this panel
			if(m_pPanel[pp].m_iPos==-1) sign=-1.0; else sign=1.0;
			pw = m_pPanel[pp].m_iWake;
			for(lw=0; lw<m_pWPolar->m_NXWakePanels; lw++)
			{
				GetDoubletInfluence(C, m_pWakePanel+pw+lw, V, phi, true, bAll);
				VT += V * Mu[pp]*sign;
			}
		}
	}
}


void PanelAnalysisDlg::InitDialog()
{
	m_Progress = 0.0;
	m_pctrlProgress->setValue(m_Progress);
	QString FileName = QDir::tempPath() + "/XFLR5.log";
	m_pXFile = new QFile(FileName);
	if (!m_pXFile->open(QIODevice::WriteOnly | QIODevice::Text)) m_pXFile = NULL;

/*	if(m_pWPolar && (m_pWPolar->m_bTiltedGeom || m_pWPolar->m_bWakeRollUp))
	{
		//back-up the current geometry if the analysis is to be performed on the tilted geometry
		memcpy(m_pMemPanel, m_pPanel, m_MatSize * sizeof(CPanel));
		memcpy(m_pMemNode,  m_pNode,  m_nNodes * sizeof(CVector));
		memcpy(m_pRefWakePanel, m_pWakePanel, m_WakeSize * sizeof(CPanel));
		memcpy(m_pRefWakeNode,  m_pWakeNode,  m_nWakeNodes * sizeof(CVector));
	}*/

	m_pctrlTextOutput->clear();

	m_bPointOut = false;
	m_bCancel   = false;
	m_bWarning  = false;
	m_bWakeRollUp    = false;

	SetFileHeader();

	QString str = "";

	m_b3DSymetric = m_pWing->m_bSymetric;

	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pWingList[iw])
		{
			if(!m_pWingList[iw]->m_bSymetric)
			{
				m_b3DSymetric = false;
			}
		}
	}

	if(m_pWPolar->m_Type==STABILITYPOLAR ) m_b3DSymetric=false;
	if(fabs(m_pWPolar->m_Beta)>0)          m_b3DSymetric=false;

	if (m_b3DSymetric) AddString(tr("Perfoming symmetric calculation")+"\n");
	else
	{
		str = tr("Performing asymmetric calculation : ")+ str +"\n";
		AddString(str);
	}

	str = QString(tr("Counted %1 panel elements")+"\n").arg(m_MatSize,4);
	AddString(str);

	AddString("\n");

	CWing::s_bVLMSymetric = m_b3DSymetric;

	m_pctrlProgress->setMinimum(0);
	m_pctrlProgress->setMaximum(100);
}


void PanelAnalysisDlg::keyPressEvent(QKeyEvent *event)
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


void PanelAnalysisDlg::OnCancelAnalysis()
{
	if(m_pXFile->isOpen()) m_pXFile->close();
//	CWing::s_bCancel        = true;
	m_bCancel = true;
	m_bSkip = true;
	m_bExit = true;
	if(m_bIsFinished) done(1);
}



void PanelAnalysisDlg::RelaxWake()
{
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	CVector VL;
	int mw, kw, lw, llw;
	int nInter;
	double t, dx, dx0;
	double *Mu    = m_Mu   ;
	double *Sigma = m_Sigma;
	mw=0;

	//Since the wake roll-up is performed on the tilted geometry,
	// we define a speed vector parallel to the x-axis
	CVector QInf(m_QInf, 0.0, 0.0);

	// Andre's method : fit the wake panels on the streamlines
	// we have the computing power to do it

	CVector LATB, TALB;
	CVector WLA, WLB,WTA,WTB, WTemp;//wake panel's leading corner points

	dx0 = 0.05;

	AddString(tr("      Relaxing the wake...")+"\n");

	memcpy(m_pTempWakeNode, m_pWakeNode, m_nWakeNodes * sizeof(CVector));

	for (lw=0; lw<m_pWPolar->m_NXWakePanels; lw++)
	{
		if(m_bCancel) break;
		for (kw=0; kw<m_NWakeColumn; kw++)
		{
			if(m_bCancel) break;

			mw = kw * m_pWPolar->m_NXWakePanels + lw;
			//left point
			WLA.Copy(m_pTempWakeNode[m_pWakePanel[mw].m_iLA]);
			WTA.Copy(m_pTempWakeNode[m_pWakePanel[mw].m_iTA]);
			WTemp.Copy(WLA);

			nInter = (int)((WTA.x - WLA.x)/dx0) ;
			dx = (WTA.x - WLA.x)/nInter;

			for (llw=0; llw<nInter; llw++)
			{
				GetSpeedVector(WTemp, Mu, Sigma, VL);
				VL += QInf;
				VL.Normalize();
				t = dx/VL.x;
				WTemp.x += dx;
				WTemp.y += VL.y * t;
				WTemp.z += VL.z * t;
			}
			m_pTempWakeNode[m_pWakePanel[mw].m_iTA] = WTemp;
		}
		//finally do the same for the right side of the last right column

		WLB.Copy(m_pTempWakeNode[m_pWakePanel[mw].m_iLB]);
		WTB.Copy(m_pTempWakeNode[m_pWakePanel[mw].m_iTB]);
		WTemp.Copy(WLB);

		nInter = (int)((WTB.x - WLB.x)/dx0);
		dx = (WTB.x - WLB.x)/nInter;

		for (llw=0; llw<nInter; llw++)
		{
			GetSpeedVector(WTemp, Mu, Sigma, VL);
			VL += QInf;
			VL.Normalize();
			t = dx/VL.x;
			WTemp.x += dx;
			WTemp.y += VL.y * t;
			WTemp.z += VL.z * t;
		}
		m_pTempWakeNode[m_pWakePanel[mw].m_iTB] = WTemp;
		m_Progress += 20.0/(double)m_pWPolar->m_NXWakePanels;
		qApp->processEvents();
	}

	// Paste the new wake nodes back into the wake node array
	memcpy(m_pWakeNode, m_pTempWakeNode, m_nWakeNodes * sizeof(CVector));

	// Re-create the wake panels
	mw=0;
	for (mw=0; mw<pMiarex->m_WakeSize; mw++)
	{
		if(m_bCancel) break;

		WLA.Copy(m_pWakeNode[m_pWakePanel[mw].m_iLA]);
		WLB.Copy(m_pWakeNode[m_pWakePanel[mw].m_iLB]);
		WTA.Copy(m_pWakeNode[m_pWakePanel[mw].m_iTA]);
		WTB.Copy(m_pWakeNode[m_pWakePanel[mw].m_iTB]);
		LATB.x = WTB.x - WLA.x;
		LATB.y = WTB.y - WLA.y;
		LATB.z = WTB.z - WLA.z;
		TALB.x = WLB.x - WTA.x;
		TALB.y = WLB.y - WTA.y;
		TALB.z = WLB.z - WTA.z;

		m_pWakePanel[mw].Normal = LATB * TALB;
		m_pWakePanel[mw].Area =  m_pWakePanel[mw].Normal.VAbs()/2.0;
		m_pWakePanel[mw].Normal.Normalize();
		m_pWakePanel[mw].SetFrame(WLA, WLB, WTA, WTB);
	}

	//Udpdate the view
//	pMiarex->m_bResetglWake = true;
	pMiarex->UpdateView();
}



bool PanelAnalysisDlg::ReLoop()
{
	QString str;
	int nrhs, Size;
	double Alpha = 0.0;

	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	if(m_QInfMax<m_QInf) m_QInfDelta = -fabs(m_QInfDelta);
	nrhs  = (int)fabs((m_QInfMax-m_QInf)*1.0001/m_QInfDelta) +1 ;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100)
	{
		QMessageBox::warning(this, tr("Warning"),tr("The number of points to be calculated will be limited to 100"));
		nrhs = 100;
	}

//	int MaxWakeIter = 1;

	if(m_b3DSymetric) Size = m_SymSize;
	else              Size = m_MatSize;


	//ESTIMATED UNIT TIMES FOR OPERATIONS
	//BuildInfluenceMatrix :     10 x m_MatSize/400
	//CreateRHS :                10
	//CreateWakeContribution :    1
	//SolveUnitRHS :             30 x MatSize/400
	//ComputeFarField :          10 x MatSize/400x 1
	//ComputeOnBodyCp :           1 x nrhs
	//RelaxWake :                20 x nrhs x MaxWakeIter *
	//ComputeAeroCoefs :          5 x nrhs

	double TotalTime = 10.0*(double)m_MatSize/400.
					 + 10.
					 + 30.*(double)m_MatSize/400.
					 + 10*(double)m_MatSize/400*1.
					 + 1*(double)nrhs
					 + 5*(double)nrhs ;

	if(!m_pWPolar->m_bThinSurfaces) TotalTime +=1.0; //for wake contribution

	m_pctrlProgress->setMinimum(0);
	m_pctrlProgress->setMaximum((int)TotalTime);

	m_Progress = 0.0;
	qApp->processEvents();

	if(m_pWPolar->m_bTiltedGeom)
	{
		//reset the initial geometry before a new angle is processed
		memcpy(m_pPanel,     m_pMemPanel,     m_MatSize * sizeof(CPanel));
		memcpy(m_pNode,      m_pMemNode,      m_nNodes  * sizeof(CVector));
		memcpy(m_pWakePanel, m_pRefWakePanel, m_WakeSize * sizeof(CPanel));
		memcpy(m_pWakeNode,  m_pRefWakeNode,  m_nWakeNodes * sizeof(CVector));

		// Rotate the wing panels and translate the wake to the new T.E. position
		CVector O;
		pMiarex->RotateGeomY(m_pWPolar->m_ASpec, O);
		m_OpAlpha = m_pWPolar->m_ASpec;
		Alpha = 0.0;
	}
	else Alpha = m_Alpha;

	str = QString(tr("   Solving the problem... ")+"\n");
	AddString(str);

	BuildInfluenceMatrix();
	if (m_bCancel) return true;

	CreateUnitRHS();
	if (m_bCancel) return true;

	CreateSourceStrength(m_Alpha, m_AlphaDelta, nrhs);
	if (m_bCancel) return true;

	if(!m_pWPolar->m_bThinSurfaces)
	{
		//compute wake contribution
		CreateWakeContribution();
		//add wake contribution to matrix and RHS
		for(int p=0; p<m_MatSize; p++)
		{
			m_uRHS[p]+= m_uWake[p];
			m_wRHS[p]+= m_wWake[p];
			for(int pp=0; pp<m_MatSize; pp++)
			{
				m_aij[p*m_MatSize+pp] += m_aijWake[p*m_MatSize+pp];
			}
		}
	}


	if (m_bCancel) return true;

	if (!SolveUnitRHS())
	{
		m_bWarning = true;
		return true;
	}
	if (m_bCancel) return true;

	CreateDoubletStrength(Alpha, m_AlphaDelta, 1);
	if (m_bCancel) return true;

	ComputeFarField(m_QInf, m_OpAlpha, 0.0, 1);
	if (m_bCancel) return true;

	for(int q=0; q<nrhs; q++)
		m_3DQInf[q] = m_QInf+q*m_QInfDelta;
		//ComputeBalanceSpeeds(m_Alpha, q);

	ScaleResultstoSpeed(nrhs);
	if (m_bCancel) return true;

	ComputeOnBodyCp(m_QInf, m_QInfDelta, nrhs);
	if (m_bCancel) return true;

	ComputeAeroCoefs(m_QInf, m_QInfDelta, nrhs);
	if (m_bCancel) return true;

	return true;
}



void PanelAnalysisDlg::SourceNASA4023(CVector const &C, CPanel *pPanel, CVector &V, double &phi)
{
	//VSAERO theory Manual
	//Influence of panel pp at coll pt of panel p
	//vectorial operations are written inline to save computing times
	//-->longer code, but 4x more efficient....
	int i;
	double CoreSize = 0.00000;
	if(fabs(*m_pCoreSize)>1.e-10) CoreSize = *m_pCoreSize;

	phi = 0.0;
	V.x=0.0; V.y=0.0; V.z=0.0;

	PJK.x = C.x - pPanel->CollPt.x;
	PJK.y = C.y - pPanel->CollPt.y;
	PJK.z = C.z - pPanel->CollPt.z;

	PN  = PJK.x*pPanel->Normal.x + PJK.y*pPanel->Normal.y + PJK.z*pPanel->Normal.z;
	pjk = sqrt(PJK.x*PJK.x + PJK.y*PJK.y + PJK.z*PJK.z);

	if(pjk> RFF*pPanel->Size)
	{
		// use far-field formula
		phi = pPanel->Area /pjk;
		V.x = PJK.x * pPanel->Area/pjk/pjk/pjk;
		V.y = PJK.y * pPanel->Area/pjk/pjk/pjk;
		V.z = PJK.z * pPanel->Area/pjk/pjk/pjk;
		return;
	}


	if(pPanel->m_iPos>=0)
	{
		m_pR[0] = m_pNode + pPanel->m_iLA;
		m_pR[1] = m_pNode + pPanel->m_iTA;
		m_pR[2] = m_pNode + pPanel->m_iTB;
		m_pR[3] = m_pNode + pPanel->m_iLB;
		m_pR[4] = m_pNode + pPanel->m_iLA;
	}
	else
	{
		m_pR[0] = m_pNode + pPanel->m_iLB;
		m_pR[1] = m_pNode + pPanel->m_iTB;
		m_pR[2] = m_pNode + pPanel->m_iTA;
		m_pR[3] = m_pNode + pPanel->m_iLA;
		m_pR[4] = m_pNode + pPanel->m_iLB;
	}

	for (i=0; i<4; i++)
	{
		a.x  = C.x - m_pR[i]->x;
		a.y  = C.y - m_pR[i]->y;
		a.z  = C.z - m_pR[i]->z;

		b.x  = C.x - m_pR[i+1]->x;
		b.y  = C.y - m_pR[i+1]->y;
		b.z  = C.z - m_pR[i+1]->z;

		s.x  = m_pR[i+1]->x - m_pR[i]->x;
		s.y  = m_pR[i+1]->y - m_pR[i]->y;
		s.z  = m_pR[i+1]->z - m_pR[i]->z;

		A    = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
		B    = sqrt(b.x*b.x + b.y*b.y + b.z*b.z);
		S    = sqrt(s.x*s.x + s.y*s.y + s.z*s.z);
		SM   = s.x*pPanel->m.x + s.y*pPanel->m.y + s.z*pPanel->m.z;
		SL   = s.x*pPanel->l.x + s.y*pPanel->l.y + s.z*pPanel->l.z;
		AM   = a.x*pPanel->m.x + a.y*pPanel->m.y + a.z*pPanel->m.z;
		AL   = a.x*pPanel->l.x + a.y*pPanel->l.y + a.z*pPanel->l.z;
		Al   = AM*SL - AL*SM;
		PA   = PN*PN*SL + Al*AM;
		PB   = PA - Al*SM; 

	//get the distance of the TestPoint to the panel's side
		h.x =  a.y*s.z - a.z*s.y;
		h.y = -a.x*s.z + a.z*s.x;
		h.z =  a.x*s.y - a.y*s.x;
	
		if(m_pR[i]->IsSame(*m_pR[i+1]))
		{
			//no contribution from this side
			CJKi = 0.0;
		}
		else if ((((h.x*h.x+h.y*h.y+h.z*h.z)/(s.x*s.x+s.y*s.y+s.z*s.z) <= CoreSize*CoreSize) && a.x*s.x+a.y*s.y+a.z*s.z>=0.0 && b.x*s.x+b.y*s.y+b.z*s.z<=0.0) ||
			         A < CoreSize || B < CoreSize)
		{
			//if lying on the panel's side... no contribution
			CJKi = 0.0;
		}
		else
		{
			//first the potential
			if(A+B-S>0.0)	GL = 1.0/S * log((A+B+S)/(A+B-S));

			RNUM = SM*PN * (B*PA-A*PB);
			DNOM = PA*PB + PN*PN*A*B*SM*SM;

			if(fabs(PN)<eps)
			{
				// side is >0 if the point is on the panel's right side
				side = pPanel->Normal.x*h.x + pPanel->Normal.y*h.y + pPanel->Normal.z*h.z;
				if(side >=0.0) sign = 1.0; else sign = -1.0;
				if(DNOM<0.0)
				{
					if(PN>0.0)	CJKi =  PI * sign;
					else		CJKi = -PI * sign;
				}
				else if(DNOM == 0.0)
				{
					if(PN>0.0)	CJKi =  PI/2.0 * sign;
					else		CJKi = -PI/2.0 * sign;
				}
				else
					CJKi = 0.0;
			}
			else 
			{
				CJKi = atan2(RNUM, DNOM);
			}

			phi += Al*GL - PN*CJKi;

			// next the induced velocity
			T1.x   = pPanel->l.x      * SM*GL;
			T1.y   = pPanel->l.y      * SM*GL;
			T1.z   = pPanel->l.z      * SM*GL;
			T2.x   = pPanel->m.x      * SL*GL;
			T2.y   = pPanel->m.y      * SL*GL;
			T2.z   = pPanel->m.z      * SL*GL;
			T.x    = pPanel->Normal.x * CJKi;
			T.y    = pPanel->Normal.y * CJKi;
			T.z    = pPanel->Normal.z * CJKi;
			V.x   += T.x + T1.x - T2.x;
			V.y   += T.y + T1.y - T2.y;
			V.z   += T.z + T1.z - T2.z;
		}
	}
}


void PanelAnalysisDlg::SetFileHeader()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QTextStream out(m_pXFile);

	out << "\n";
	out << pMainFrame->m_VersionName;
	out << "\n";
	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");
	out << str<<"\n";

	if(m_pPlane)     out << m_pPlane->PlaneName();
	else if(m_pWing) out << m_pWing->WingName();
	out << "\n";

	out << m_pWPolar->m_PlrName;
	out << "\n___________________________________\n";
}


void PanelAnalysisDlg::OnProgress()
{
	m_pctrlProgress->setValue((int)m_Progress);
}


void PanelAnalysisDlg::SetupLayout()
{
	QDesktopWidget desktop;
	QRect r = desktop.geometry();
	setMinimumHeight(r.height()/2);
	setMinimumWidth(r.width()/2);

	m_pctrlTextOutput = new QTextEdit;
	m_pctrlTextOutput->setReadOnly(true);
	m_pctrlTextOutput->setLineWrapMode(QTextEdit::NoWrap);
	m_pctrlTextOutput->setWordWrapMode(QTextOption::NoWrap);
	m_pctrlTextOutput->setFontFamily("Courier");

	m_pctrlProgress = new QProgressBar;
	m_pctrlProgress->setOrientation(Qt::Horizontal);
	m_pctrlProgress->setMinimum(0);
	m_pctrlProgress->setMaximum(100);
	m_pctrlProgress->setValue(0);

	m_pctrlCancel = new QPushButton(tr("Cancel"));
	connect(m_pctrlCancel, SIGNAL(clicked()), this, SLOT(OnCancelAnalysis()));

	QHBoxLayout *ButtonLayout = new QHBoxLayout;
	ButtonLayout->addStretch(1);
	ButtonLayout->addWidget(m_pctrlCancel);
	ButtonLayout->addStretch(1);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_pctrlTextOutput);
	mainLayout->addWidget(m_pctrlProgress);
	mainLayout->addLayout(ButtonLayout);
	setLayout(mainLayout);
}



bool PanelAnalysisDlg::SolveUnitRHS()
{
	//______________________________________________________________________________________
	// Method :
	// 	- If the polar is of type 1 or 2, solve the linear system
	//	- for cosine and sine parts, for a unit speed
	//	- If the polar is of type 4, solve only for unit speed and for the specified Alpha
	//	- Reconstruct right side results if calculation was symetric
	//	- Sort results i.a.w. panel numbering
	//______________________________________________________________________________________

	int Size = m_MatSize;
	if(m_b3DSymetric) Size = m_SymSize;

	memcpy(m_RHS,      m_uRHS, Size * sizeof(double));
	memcpy(m_RHS+Size, m_wRHS, Size * sizeof(double));

	AddString("      Performing LU Matrix decomposition...\n");

	if(!Crout_LU_Decomposition_with_Pivoting(m_aij, m_Index, Size, &m_bCancel, 30.0*(double)m_MatSize/400.0, m_Progress))
	{
		AddString(tr("      Singular Matrix.... Aborting calculation...\n"));
		m_bConverged = false;
		return false;
	}
	else m_bConverged = true;


	AddString("      Solving LU system...\n");
	Crout_LU_with_Pivoting_Solve(m_aij, m_uRHS, m_Index, m_RHS,      Size, &m_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_wRHS, m_Index, m_RHS+Size, Size, &m_bCancel);
	//______________________________________________________________________________________
	//Reconstruct right side results if calculation was symetric
	int m,p;
	if(m_b3DSymetric)
	{
		m=0;
		for (p=0; p<m_MatSize; p++)
		{
			if(m_pPanel[p].m_bIsLeftPanel)
			{
				m_uRHS[p] = m_RHS[m];
				m_wRHS[p] = m_RHS[m+Size];
				if(m_pPanel[p].m_iSym>=0)
				{
					m_uRHS[m_pPanel[p].m_iSym] = m_RHS[m];
					m_wRHS[m_pPanel[p].m_iSym] = m_RHS[m+Size];
				}
				m++;
			}
		}
	}
	else
	{
		memcpy(m_uRHS, m_RHS,           m_MatSize*sizeof(double));
		memcpy(m_wRHS, m_RHS+m_MatSize, m_MatSize*sizeof(double));
	}


	//   Define unit local velocity vector, necessary for moment calculations in stability analysis of 3D panels
	CVector u(1.0, 0.0, 0.0);
	CVector w(0.0, 0.0, 1.0);
	double Cp;
	for (p=0; p<m_MatSize; p++)
	{
		if(m_pPanel[p].m_iPos!=0)
		{
			GetDoubletDerivative(p, m_uRHS, Cp, m_uVl[p], 1.0, u.x, u.y, u.z);
			GetDoubletDerivative(p, m_wRHS, Cp, m_wVl[p], 1.0, w.x, w.y, w.z);
		}
		if(m_bCancel) return false;
	}

	return true;
}


void PanelAnalysisDlg::CreateDoubletStrength(double Alpha0, double AlphaDelta, int nval)
{
	//
	// Creates the doublet strength for all the operating points from the unit sine and cosine results
	// The doublet and source strengths are for a unit speed
	// The scaling to speed is performed at the next step, depending on the polar type
	//
	int  nrhs, q, p;
	double alpha, cosa, sina;

	int Size = m_MatSize;
	if(m_b3DSymetric) Size = m_SymSize;

	AddString("      Calculating doublet strength...\n");
	if(m_pWPolar->m_Type!=FIXEDAOAPOLAR) nrhs = nval;
	else                                 nrhs = 0;

	//______________________________________________________________________________________
	//	reconstruct all results from cosine and sine unit vectors
	for (q=0; q<nval;q++)
	{
		alpha = Alpha0 + q * AlphaDelta;
		cosa = cos(alpha*PI/180.0);
		sina = sin(alpha*PI/180.0);
		for(p=0; p<m_MatSize; p++)
		{
			m_Mu[p+q*m_MatSize] = cosa * m_uRHS[p] + sina * m_wRHS[p];
		}
	}

}


void PanelAnalysisDlg::StartAnalysis()
{
	//
	// Method applied from v6.00 onwards
	// 
	// First case :
	// If the analysis is for a wing and not a plane, the full 3D panel method is applied
	// and the wing is modelled as a thick surface
	// The method is strictly the one described in NASA TN 4023
	// The boundary condition is of the Dirichlet type, which has proved more convincing 
	// than the Neumann BC for full 3D panel methods
	//
	// Second case :
	// If the analysis is for a plane, the full 3D method is not applicable since the 
	// junctions between wing and body, or between fin and elevator, cannot be adequately 
	// represented as closed surfaces. This would require a 3D CAD programe. 
	// Therefore, in this case, the wings are modelled as thin surfaces.
	// Trial tests using the method of NASA TN 4023 have not been conclusive. With a uniform doublet
	// distribution and a boundary condition applied at the panel's centroid, the results 
	// are less convincing than with VLM.
	// Therefore in this case, the VLM1 method is applied to the thin surfaces, and the 3D-panel method 
	// is applied to the body.
	// Last consideration : since the potential of a straight vortex line requires a lot of computations, 
	// the Neumann type BC is applied to the body panels, rather than the Dirichlet type BC
	//
	qApp->processEvents();

	if(!m_pWPolar) return;

	QString strong;
	m_pctrlCancel->setText(tr("Cancel"));
	m_bIsFinished = false;

	if(m_pWPolar->m_AnalysisMethod==PANELMETHOD)	strong = tr("Launching 3D Panel Analysis....")+"\n";
	else if(m_pWPolar->m_AnalysisMethod==VLMMETHOD)
	{
		if(m_pWPolar->m_bVLM1) strong = tr("Launching VLM1 Analysis....")+"\n";
		else                   strong = tr("Launching VLM2 Analysis....")+"\n";
	}
	AddString(strong);

	if(m_pWPolar->m_AnalysisMethod==PANELMETHOD)
	{
		strong = tr("Using Dirichlet boundary conditions")+"\n";

		if(m_pWPolar->m_bDirichlet) strong = tr("Using Dirichlet boundary conditions")+"\n";
		else                        strong = tr("Using Neumann boundary conditions")+"\n";
		AddString(strong);
	}

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(m_pPlane)
	{
		QString len, str;
		GetLengthUnit(len, pMainFrame->m_LengthUnit);
		if(fabs(m_pPlane->WingLE(0).z-m_pPlane->WingLE(2).z)<.0001)
		{	
			str = QString("%1 ").arg(m_pPlane->WingLE(0).z*pMainFrame->m_mtoUnit, 7, 'g', 3);
			strong = tr("Warning: The wing and elevator lie in the same plane z=")+str+len+"\n";
			AddString(strong);
			strong = tr("It is recommended to slightly offset the wing or the elevator to avoid numerical instabilities")+"\n\n";
			AddString(strong);
		}
	}

	if(m_pWPolar->m_Type==FIXEDSPEEDPOLAR)     strong = tr("Type 1 - Fixed speed polar");
	else if(m_pWPolar->m_Type==FIXEDLIFTPOLAR) strong = tr("Type 2 - Fixed lift polar");
	else if(m_pWPolar->m_Type==FIXEDAOAPOLAR)  strong = tr("Type 4 - Fixed angle of attack polar");
	else if(m_pWPolar->m_Type==STABILITYPOLAR) strong = tr("Type 7 - Stability polar");
	AddString(strong);
	m_bCancel = false;

	m_SymSize = 0;
	int PlusSize=0;
	for(int p=0; p<m_MatSize; p++)
	{
		if(m_pPanel[p].m_bIsLeftPanel) m_SymSize++;
	}
	for(int p=0; p<m_MatSize; p++)
	{
		if(m_pPanel[p].m_iSym>=0) PlusSize++;
	}

	QTimer *pTimer = new QTimer;
	connect(pTimer, SIGNAL(timeout()), this, SLOT(OnProgress()));
	pTimer->setInterval(100);
	pTimer->start();

	qApp->processEvents();


	if(m_pWPolar->m_Type<3)
	{
		if(m_pWPolar->m_bTiltedGeom) UnitLoop();
		else                         AlphaLoop();
	}
	else if(m_pWPolar->m_Type==FIXEDAOAPOLAR)
	{
		ReLoop();
	}
	else if(m_pWPolar->m_Type==STABILITYPOLAR)
	{
		m_Ib[0][0] = m_pWPolar->m_CoGIxx;
		m_Ib[1][1] = m_pWPolar->m_CoGIyy;
		m_Ib[2][2] = m_pWPolar->m_CoGIzz;
		m_Ib[0][2] = m_Ib[2][0] = m_pWPolar->m_CoGIxz;
		m_Ib[1][0] = m_Ib[1][2] = m_Ib[0][1] = m_Ib[2][1] = 0.0;

		ControlLoop();
	}

	if (!m_bCancel && !m_bWarning) strong = "\n"+tr("Panel Analysis completed successfully")+"\n";
	else if (m_bWarning)           strong = "\n"+tr("Panel Analysis completed ... Errors encountered")+"\n";
	AddString(strong);
	pTimer->stop();

	if(m_pWPolar && (m_pWPolar->m_Type==STABILITYPOLAR || m_pWPolar->m_bTiltedGeom || m_pWPolar->m_bWakeRollUp))
	{
		//restore the panels and nodes;
		memcpy(m_pPanel, m_pMemPanel, m_MatSize * sizeof(CPanel));
		memcpy(m_pNode,  m_pMemNode,  m_nNodes  * sizeof(CVector));
		memcpy(m_pWakePanel, m_pRefWakePanel, m_WakeSize * sizeof(CPanel));
		memcpy(m_pWakeNode,  m_pRefWakeNode,  m_nWakeNodes * sizeof(CVector));
	}

	m_bIsFinished = true;
	m_pctrlCancel->setText(tr("Close"));
}


void PanelAnalysisDlg::SumPanelForces(double *Cp, double Alpha, double &Lift, double &Drag)
{
	int p;
	CVector PanelForce;

	for(p=0; p<m_MatSize; p++)
	{
		PanelForce += m_pPanel[p].Normal * (-Cp[p]) * m_pPanel[p].Area;
	}

	Lift = PanelForce.z * cos(Alpha*PI/180.0) - PanelForce.x * sin(Alpha*PI/180.0);
	Drag = PanelForce.x * cos(Alpha*PI/180.0) + PanelForce.z * sin(Alpha*PI/180.0);
}



bool PanelAnalysisDlg::UnitLoop()
{
	QString str;
	CVector O(0.0,0.0,0.0);

	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	int n, nrhs, nWakeIter, MaxWakeIter;

	if(m_AlphaMax<m_Alpha) m_AlphaDelta = -fabs(m_AlphaDelta);
	nrhs  = (int)fabs((m_AlphaMax-m_Alpha)*1.0001/m_AlphaDelta) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100)
	{
		QMessageBox::warning(this, tr("Warning"),tr("The number of points to be calculated will be limited to 100"));
		nrhs = 100;
	}

	if(!m_pWPolar->m_bWakeRollUp) MaxWakeIter = 1;
	else                          MaxWakeIter = qMax(m_MaxWakeIter, 1);

	int Size = m_MatSize;
	if(m_b3DSymetric) Size = m_SymSize;


	//ESTIMATED UNIT TIMES FOR OPERATIONS
	//BuildInfluenceMatrix :     10 x MatSize/400
	//CreateRHS :                10
	//CreateWakeContribution :    1
	//SolveUnitRHS :             30 x MatSize/400
	//ComputeFarField :          10 x MatSize/400x nrhs
	//ComputeOnBodyCp :           1 x nrhs
	//RelaxWake :                20 x nrhs x MaxWakeIter *
	//ComputeAeroCoefs :          5 x nrhs

	double TotalTime = 10.0*(double)m_MatSize/400.*(double)nrhs
					 + 10.*(double)nrhs
					 + 30.*(double)m_MatSize/400.*(double)nrhs
					 + 10*(double)m_MatSize/400*(double)nrhs
					 + 1*(double)nrhs
					 + 5*(double)nrhs ;


//	if(m_pWPolar->m_bWakeRollUp) TotalTime += 20.0 * (double)nrhs * (double)MaxWakeIter;

	m_pctrlProgress->setMinimum(0);
	m_pctrlProgress->setMaximum((int)TotalTime);
	m_Progress = 0.0;
	qApp->processEvents();

	str = QString(tr("   Solving the problem... ")+"\n");
	AddString(str);

	for (n=0; n<nrhs; n++)
	{
		m_OpAlpha = m_Alpha+n*m_AlphaDelta;
		str = QString("      \n    "+tr("Processing Alpha= %1")+"\n").arg(m_OpAlpha,7,'f',2);
		AddString(str);

		//reset the initial geometry before a new angle is processed
		memcpy(m_pPanel, m_pMemPanel, m_MatSize * sizeof(CPanel));
		memcpy(m_pNode,  m_pMemNode,  m_nNodes  * sizeof(CVector));
		memcpy(m_pWakePanel, m_pRefWakePanel, m_WakeSize * sizeof(CPanel));
		memcpy(m_pWakeNode,  m_pRefWakeNode,  m_nWakeNodes * sizeof(CVector));

		// Rotate the wing panels and translate the wake to the new T.E. position
		pMiarex->RotateGeomY(m_OpAlpha, O);

		BuildInfluenceMatrix();
		if (m_bCancel) return true;

		// The calculation will be performed at AOA=0.0 because the geometry is tilted...
		// ... but the operating angle is different :

		CreateUnitRHS();
		if (m_bCancel) return true;

		CreateSourceStrength(0.0, m_AlphaDelta, 1);
		if (m_bCancel) return true;

		for (nWakeIter = 0; nWakeIter<MaxWakeIter; nWakeIter++)
		{
			if(m_pWPolar->m_bWakeRollUp)
			{
				str = QString(tr("      Wake iteration %1")+"\n").arg(nWakeIter+1,3);
				AddString(str);
			}

			if (m_bCancel) return true;

			//todo : check... may not be quite correct
			if(!m_pWPolar->m_bThinSurfaces)
			{
				//compute wake contribution
				CreateWakeContribution();
				//add wake contribution to matrix and RHS
				for(int p=0; p<m_MatSize; p++)
				{
					m_uRHS[p]+= m_uWake[p];
					m_wRHS[p]+= m_wWake[p];
					for(int pp=0; pp<m_MatSize; pp++)
					{
						m_aij[p*m_MatSize+pp] += m_aijWake[p*m_MatSize+pp];
					}
				}
			}

			if (m_bCancel) return true;

			if (!SolveUnitRHS())
			{
				m_bWarning = true;
				return true;
			}
			if (m_bCancel) return true;

			CreateDoubletStrength(0.0, m_AlphaDelta, 1);
			if (m_bCancel) return true;

			ComputeFarField(1.0, 0.0, m_AlphaDelta, 1);
			if (m_bCancel) return true;

			ComputeBalanceSpeeds(0.0, 0);
			if (m_bCancel) return true;

			ScaleResultstoSpeed(1);
			if (m_bCancel) return true;

			ComputeOnBodyCp(0.0, m_AlphaDelta, 1);
			if (m_bCancel) return true;

			if(MaxWakeIter>0 && m_pWPolar->m_bWakeRollUp) RelaxWake();
		}

		ComputeAeroCoefs(0.0, m_AlphaDelta, 1);
	}
	return true;
}




void PanelAnalysisDlg::WriteString(QString strong)
{
	if(!m_pXFile) return;
	if(!m_pXFile->isOpen()) return;
	QTextStream ds(m_pXFile);
	ds << strong;
}





void PanelAnalysisDlg::VLMGetVortexInfluence(CPanel *pPanel, CVector const &C, CVector &V, bool bAll)
{
	// calculates the the panel p's vortex influence at point C
	// V is the resulting velocity
	static int lw, pw, p;
	static CVector AA, BB, AA1, BB1, AAG, BBG, VT;
	p = pPanel->m_iElement;

	V.x = V.y = V.z = 0.0;

	if(m_pWPolar->m_bVLM1)
	{
		//just get the horseshoe vortex's influence
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
	else
	{
		// we have quad vortices
		// so we follow Katz and Plotkin's lead
		if(!pPanel->m_bIsTrailing)
		{
			if(bAll)
			{
				VLMQmn(pPanel->A, pPanel->B, m_pPanel[p-1].A, m_pPanel[p-1].B, C, V);
				if(m_pWPolar->m_bGround)
				{
					AA  = pPanel->A;
					BB  = pPanel->B;
					AA1 = m_pPanel[p-1].A;
					BB1 = m_pPanel[p-1].B;
					AA.z  = -AA.z - 2.0*m_pWPolar->m_Height;
					BB.z  = -BB.z - 2.0*m_pWPolar->m_Height;
					AA1.z = -AA1.z - 2.0*m_pWPolar->m_Height;
					BB1.z = -BB1.z - 2.0*m_pWPolar->m_Height;
					VLMQmn(AA, BB, AA1, BB1, C, VG);
					V.x += VG.x;
					V.y += VG.y;
					V.z -= VG.z;
				}
			}
		}
		else
		{
			// then panel p is trailing and shedding a wake
			if(!m_bWakeRollUp)
			{
				// since Panel p+1 does not exist...
				// we define the points AA=A+1 and BB=B+1
				AA1.x = m_pNode[pPanel->m_iTA].x + (m_pNode[pPanel->m_iTA].x-pPanel->A.x)/3.0;
				AA1.y = m_pNode[pPanel->m_iTA].y;
				AA1.z = m_pNode[pPanel->m_iTA].z;
				BB1.x = m_pNode[pPanel->m_iTB].x + (m_pNode[pPanel->m_iTB].x-pPanel->B.x)/3.0;
				BB1.y = m_pNode[pPanel->m_iTB].y;
				BB1.z = m_pNode[pPanel->m_iTB].z;
				if(m_pWPolar->m_bGround)
				{
					AA  = pPanel->A;
					BB  = pPanel->B;
					AAG = AA1;
					BBG = BB1;
					AA.z  = -AA.z - 2.0*m_pWPolar->m_Height;
					BB.z  = -BB.z - 2.0*m_pWPolar->m_Height;
					AAG.z = -AAG.z - 2.0*m_pWPolar->m_Height;
					BBG.z = -BBG.z - 2.0*m_pWPolar->m_Height;
				}
				// first we get the quad vortex's influence
				if (bAll)
				{
					VLMQmn(pPanel->A, pPanel->B, AA1, BB1, C, V);
					if(m_pWPolar->m_bGround)
					{
						VLMQmn(AA, BB, AAG, BBG, C, VG);
						V.x += VG.x;
						V.y += VG.y;
						V.z -= VG.z;
					}
				}

				//we just add a trailing horseshoe vortex's influence to simulate the wake
				VLMCmn(AA1,BB1,C,VT,bAll);

				if(m_pWPolar->m_bGround)
				{
					VLMCmn(AAG, BBG, C, VG);
					V.x += VG.x;
					V.y += VG.y;
					V.z -= VG.z;
				}
				V.x += VT.x;
				V.y += VT.y;
				V.z += VT.z;
			}
			else
			{
				// if there is a wake roll-up required
				pw = pPanel->m_iWake;
				// first close the wing's last vortex ring at T.E.
				if (bAll)
				{
					VLMQmn(pPanel->A, pPanel->B, m_pWakePanel[pw].A, m_pWakePanel[pw].B, C, V);
					if(m_pWPolar->m_bGround)
					{
						AA  = pPanel->A;
						BB  = pPanel->B;
						AAG = m_pWakePanel[pw].A;
						BBG = m_pWakePanel[pw].B;
						AA.z  = -AA.z - 2.0*m_pWPolar->m_Height;
						BB.z  = -BB.z - 2.0*m_pWPolar->m_Height;
						AAG.z = -AAG.z - 2.0*m_pWPolar->m_Height;
						BBG.z = -BBG.z - 2.0*m_pWPolar->m_Height;

						VLMQmn(AA, BB, AAG, BBG, C, VG);
						V.x += VG.x;
						V.y += VG.y;
						V.z -= VG.z;
					}
				}

				//each wake panel has the same vortex strength than the T.E. panel
				//so we just cumulate their unit influences
				if(bAll)
				{
					for (lw=0; lw<m_pWPolar->m_NXWakePanels-1; lw++)
					{
						VLMQmn(m_pWakePanel[pw  ].A, m_pWakePanel[pw  ].B,
							   m_pWakePanel[pw+1].A, m_pWakePanel[pw+1].B, C, VT);
						V += VT;
						if(m_pWPolar->m_bGround)
						{
							AA  = m_pWakePanel[pw].A;
							BB  = m_pWakePanel[pw].B;
							AAG = m_pWakePanel[pw+1].A;
							BBG = m_pWakePanel[pw+1].B;
							AA.z  = -AA.z - 2.0*m_pWPolar->m_Height;
							BB.z  = -BB.z - 2.0*m_pWPolar->m_Height;
							AAG.z = -AAG.z - 2.0*m_pWPolar->m_Height;
							BBG.z = -BBG.z - 2.0*m_pWPolar->m_Height;

							VLMQmn(AA, BB, AAG, BBG, C, VG);
							V.x += VG.x;
							V.y += VG.y;
							V.z -= VG.z;
						}
						pw++;
					}
				}
//
// For the very last wake panel downstream, just add a horseshoe vortex influence
//
// TODO : check influence on results
//
//				VLMCmn(m_pWakePanel[pw].A, m_pWakePanel[pw].B,C,VT,bAll);
//				V += VT;
//				if(m_pWPolar->m_bGround)
//				{
//					VLMCmn(AAG, BBG, C, VG);
//					V.x += VG.x;
//					V.y += VG.y;
//					V.z -= VG.z;
//				}
//
// simple really !
//
			}
		}
	}
}




bool PanelAnalysisDlg::ControlLoop()
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
	QString str, strlen, strmass, strInertia, outString;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex *pMiarex  = (QMiarex*)s_pMiarex;

	GetLengthUnit(strlen, pMainFrame->m_LengthUnit);
	GetWeightUnit(strmass, pMainFrame->m_WeightUnit);
	strInertia = strmass+"."+strlen+QString::fromUtf8("²");


	str = QString("   Mass=%1 ").arg(m_pWPolar->m_Mass*pMainFrame->m_kgtoUnit, 12,'f',3)+strmass+"\n";
	AddString(str);

	str = "\n   ___Center of Gravity Position - Body axis____\n";
	AddString(str);
	str = QString("    CoG_x=%1 ").arg(m_pWPolar->m_CoG.x * pMainFrame->m_mtoUnit, 12,'f',4)+strlen+"\n";
	AddString(str);
	str = QString("    CoG_y=%1 ").arg(m_pWPolar->m_CoG.y * pMainFrame->m_mtoUnit, 12,'f',4)+strlen+"\n";
	AddString(str);
	str = QString("    CoG_z=%1 ").arg(m_pWPolar->m_CoG.z * pMainFrame->m_mtoUnit, 12,'f',4)+strlen+"\n";
	AddString(str);

	str = "\n   ___Inertia - Body Axis - CoG Origin____\n";
	AddString(str);
	str = QString("    Ibxx=%1 ").arg(m_Ib[0][0]*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_kgtoUnit, 12,'g',4);
	AddString(str+strInertia+"\n");
	str = QString("    Ibyy=%1 ").arg(m_Ib[1][1]*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_kgtoUnit, 12,'g',4);
	AddString(str+strInertia+"\n");
	str = QString("    Ibzz=%1 ").arg(m_Ib[2][2]*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_kgtoUnit, 12,'g',4);
	AddString(str+strInertia+"\n");
	str = QString("    Ibxz=%1 ").arg(m_Ib[0][2]*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_kgtoUnit, 12,'g',4);
	AddString(str+strInertia+"\n\n");

	if(m_ControlMax<m_ControlMin) m_ControlDelta = -fabs(m_ControlDelta);

	nrhs  = (int)fabs((m_ControlMax-m_ControlMin)*1.0001/m_ControlDelta) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs==0) nrhs = 1;//compute at least nominal control positions, even if none is active nor defined
	else if(nrhs>=100)
	{
		QMessageBox::warning(this, tr("Warning"), "The number of points to be calculated will be limited to 100");
		nrhs = 100;
	}

	double TotalTime = 10.0*(double)m_MatSize/400.      //BuildInfluenceMatrix
					 + 10.                               //CreateRHS
					 + 30.*(double)m_MatSize/400.        //SolveUnitRHS
					 + 10*(double)m_MatSize/400          //ComputeFarField
					 + 2+5*6                                 //ComputeStabDer
					 + 1                                 //ComputeOnBodyCp
					 + 5;                                //ComputeAeroCoefs
	TotalTime *= (double)nrhs;

	m_pctrlProgress->setMinimum(0);
	m_pctrlProgress->setMaximum((int)TotalTime);
	m_Progress = 0.0;
	qApp->processEvents();

	m_bTrace = true;

	str = QString("   Solving the problem... \n\n");
	AddString(str);
	for (i=0; i<nrhs; i++)
	{
		// create the geometry for the control parameter
		// so first restore the initial geometry
		memcpy(m_pPanel, m_pMemPanel, m_MatSize * sizeof(CPanel));
		memcpy(m_pNode,  m_pMemNode,  m_nNodes  * sizeof(CVector));

		//define the control position for this iteration
		m_Ctrl = m_ControlMin +(double)i *m_ControlDelta;
		str = QString("      Calculation for control position %1\n").arg(m_Ctrl ,5,'f',2);
		AddString(str);
		outString.clear();

		pMiarex->SetControlPositions(m_pPanel, m_pNode, m_Ctrl, m_NCtrls, outString, true);

		AddString(outString);
		if(m_bCancel) break;

		// next find the balanced and trimmed conditions
		if(!ComputeTrimmedConditions())
		{
			if(m_bCancel) break;
			//no zero moment alpha
			str = QString("      Unsuccessfull attempt to trim the model for control position=%1 - skipping.\n\n\n").arg(m_Ctrl,5,'f',2);
			AddString(str);
			m_bWarning = true;
		}
		else
		{
			m_3DQInf[i] = u0;
			m_QInf      = u0;

			if (m_bCancel) return true;

			//Build the rotation matrix from body axes to stability axes
			BuildRotationMatrix();
			if(m_bCancel) break;

			// Compute inertia in stability axes
			ComputeStabilityInertia();
			if(m_bCancel) break;

			str = "\n      ___Inertia - Stability Axis - CoG Origin____\n";
			AddString(str);
			str = QString("      Isxx=%1 ").arg(m_Is[0][0]*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_kgtoUnit, 12,'g',4);
			AddString(str+strInertia+"\n");
			str = QString("      Isyy=%1 ").arg(m_Is[1][1]*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_kgtoUnit, 12,'g',4);
			AddString(str+strInertia+"\n");
			str = QString("      Iszz=%1 ").arg(m_Is[2][2]*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_kgtoUnit, 12,'g',4);
			AddString(str+strInertia+"\n");
			str = QString("      Isxz=%1 ").arg(m_Is[0][2]*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit*pMainFrame->m_kgtoUnit, 12,'g',4);
			AddString(str+strInertia+"\n\n");

			// Compute stability and control derivatives in stability axes
			// viscous or not viscous ?
			ComputeStabilityDerivatives();
			if(m_bCancel) break;

			ComputeControlDerivatives(); //single derivative, wrt the polar's control variable
			if(m_bCancel) break;

			ComputeNDStabDerivatives();

			// Construct the state matrices - longitudinal and lateral
			BuildStateMatrices();

			// Solve for eigenvalues
			if(!SolveEigenvalues())
			{
				str = QString("      Unsuccessfull attempt to compute eigenvalues for Control=%1 - skipping.\n\n\n").arg(m_Ctrl,10,'f',3);
				AddString(str);
				m_bWarning = true;
			}
			else
			{
				// Compute aero coefficients for trimmed conditions
				ComputeFarField(m_QInf, m_AlphaEq, 0.0, 1);
				if (m_bCancel) return true;

				ComputeOnBodyCp(m_AlphaEq, 0.0, 1);
				if (m_bCancel) return true;


				str = QString(tr("      Computing Plane for alpha=%1")).arg(m_AlphaEq,7,'f',2);
				str += QString::fromUtf8("°\n");
				AddString(str);
				ComputePlane(m_AlphaEq, u0, 0);

				if (m_bCancel) return true;
			}
			str = QString("\n     ______Finished operating point calculation for control position %1________\n\n\n\n\n").arg(m_Ctrl, 5,'f',2);
			AddString(str);
		}
		if(m_bCancel) break;
	}
	return true;
}




bool PanelAnalysisDlg::SolveEigenvalues()
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

	//sort them
	ComplexSort(m_rLong, 4);

	for(i=0; i<4; i++)
	{
		if(!Eigenvector(m_ALong, m_rLong[i], m_vLong+i*4))
		{
			str = QString("Error extracting longitudinal eigenvector for mode %1\n").arg(i);
			return false;
		}
	}

	str = "\n\n      ___Longitudinal modes____\n\n";
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

	//sort them
	ComplexSort(m_rLat, 4);

	for(i=0; i<4; i++)
	{
		if(!Eigenvector(m_ALat, m_rLat[i], m_vLat+i*4))
		{
			str = QString("Error extracting lateral eigenvector for mode %1\n").arg(i);
			return false;
		}
	}


	str = "\n\n      ___Lateral modes____\n\n";
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


void PanelAnalysisDlg::ComputeNDStabDerivatives()
{
	QString str;
	double b, S, mac, q, theta0, Cw0, Cx0, Cz0;
	double mass = m_pWPolar->m_Mass;
	double rho = m_pWPolar->m_Density;

	q = 1./2. * m_pWPolar->m_Density * u0 * u0;
	b   = m_pWPolar->m_WSpan;
	S   = m_pWPolar->m_WArea;
	mac = m_pWing->m_MAChord;
	theta0 = 0.0;//steady level flight only ?

	Cw0 = mass * 9.81/q/S; //E&R p.127
	Cx0 =  Cw0 * sin(theta0); //E&R p.119
	Cz0 = -Cw0 * cos(theta0); //E&R p.118

	//E&R p. 118, table 4.4
	CXu = (Xu - rho * u0*S*Cw0*sin(theta0))/(0.5*rho*u0*S);
	CZu = (Zu + rho * u0*S*Cw0*cos(theta0))/(0.5*rho*u0*S);
	Cmu = Mu /(0.5*rho*u0*mac*S);
	CXa = Xw /(0.5*rho*u0*S);
	CZa = Zw /(0.5*rho*u0*S);
	Cma = Mw /(0.5*rho*u0*mac*S);
	CXq = Xq /(.25*rho*u0*mac*S);
	CZq = Zq /(.25*rho*u0*mac*S);
	Cmq = Mq /(.25*rho*u0*mac*mac*S);

	XNP = m_pWPolar->m_CoG.x + Cma/CZa * mac; //E&R (eq. 2.3.5 p.29)

	CYb = Yv*    u0     /(q*S);
	CYp = Yp* 2.*u0     /(q*S*b);
	CYr = Yr* 2.*u0     /(q*S*b);
	Clb = Lv*    u0     /(q*S*b);
	Clp = Lp*(2.*u0/b)  /(q*S*b);
	Clr = Lr*(2.*u0/b)  /(q*S*b);
	Cnb = Nv*    u0     /(q*S*b);
	Cnp = Np*(2.*u0/b)  /(q*S*b);
	Cnr = Nr*(2.*u0/b)  /(q*S*b);

	CXe = Xde/(q*S);
	CYe = Yde/(q*S);
	CZe = Zde/(q*S);
	Cle = Lde/(q*S*b);
	Cme = Mde/(q*S*mac);
	Cne = Nde/(q*S*b);

	// no OpPoint, we output the data to the log file
	str = "      Longitudinal derivatives\n";
	AddString(str);
	str = QString("      Xu=%1         Cxu=%2\n").arg(Xu,12,'g',5).arg(CXu, 12, 'g', 5);
	AddString(str);
	str = QString("      Xw=%1         Cxa=%2\n").arg(Xw,12,'g',5).arg(CXa, 12, 'g', 5);
	AddString(str);
	str = QString("      Zu=%1         Czu=%2\n").arg(Zu,12,'g',5).arg(CZu, 12, 'g', 5);
	AddString(str);
	str = QString("      Zw=%1         CLa=%2\n").arg(Zw,12,'g',5).arg(-CZa,12, 'g', 5);
	AddString(str);
	str = QString("      Zq=%1         CLq=%2\n").arg(Zq,12,'g',5).arg(-CZq,12, 'g', 5);
	AddString(str);
	str = QString("      Mu=%1         Cmu=%2\n").arg(Mu,12,'g',5).arg(Cmu, 12, 'g', 5);
	AddString(str);
	str = QString("      Mw=%1         Cma=%2\n").arg(Mw,12,'g',5).arg(Cma, 12, 'g', 5);
	AddString(str);
	str = QString("      Mq=%1         Cmq=%2\n").arg(Mq,12,'g',5).arg(Cmq, 12, 'g', 5);
	AddString(str);

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strLength;
	GetLengthUnit(strLength, pMainFrame->m_LengthUnit);
	str = QString("      Neutral Point position=%1").arg(XNP*pMainFrame->m_mtoUnit, 10,'f',5);
	str += strLength;
	str +="\n\n";
	AddString(str);

	str = "\n      Lateral derivatives\n";
	AddString(str);
	str = QString("      Yv=%1         CYb=%2\n").arg(Yv,12,'g',5).arg(CYb,12,'g',5);
	AddString(str);
	str = QString("      Yp=%1         CYp=%2\n").arg(Yp,12,'g',5).arg(CYp,12,'g',5);
	AddString(str);
	str = QString("      Yr=%1         CYr=%2\n").arg(Yr,12,'g',5).arg(CYr,12,'g',5);
	AddString(str);
	str = QString("      Lv=%1         Clb=%2\n").arg(Lv,12,'g',5).arg(Clb,12,'g',5);
	AddString(str);
	str = QString("      Lp=%1         Clp=%2\n").arg(Lp,12,'g',5).arg(Clp,12,'g',5);
	AddString(str);
	str = QString("      Lr=%1         Clr=%2\n").arg(Lr,12,'g',5).arg(Clr,12,'g',5);
	AddString(str);
	str = QString("      Nv=%1         Cnb=%2\n").arg(Nv,12,'g',5).arg(Cnb,12,'g',5);
	AddString(str);
	str = QString("      Np=%1         Cnp=%2\n").arg(Np,12,'g',5).arg(Cnp,12,'g',5);
	AddString(str);
	str = QString("      Nr=%1         Cnr=%2\n\n").arg(Nr,12,'g',5).arg(Cnr,12,'g',5);
	AddString(str);


	//output control derivatives

	str = QString("      Control derivatives \n");
	AddString(str);

	str = QString("      Xde=%1        CXde=%2\n").arg(Xde,12,'g',5).arg(Xde/(q*S),12,'g',5);
	AddString(str);

	str = QString("      Yde=%1        CYde=%2\n").arg(Yde,12,'g',5).arg(Yde/(q*S),12,'g',5);
	AddString(str);

	str = QString("      Zde=%1        CZde=%2\n").arg(Zde,12,'g',5).arg(Zde/(q*S),12,'g',5);
	AddString(str);

	str = QString("      Lde=%1        CLde=%2\n").arg(Lde,12,'g',5).arg(Lde/(q*S*b),12,'g',5);
	AddString(str);

	str = QString("      Mde=%1        CMde=%2\n").arg(Mde,12,'g',5).arg(Mde/(q*S*mac),12,'g',5);
	AddString(str);

	str = QString("      Nde=%1        CNde=%2\n").arg(Nde,12,'g',5).arg(Nde/(q*S*b),12,'g',5);
	AddString(str+"\n");

	str ="\n";
	AddString(str);

}



void PanelAnalysisDlg::Forces(double *Mu, double *Sigma, double alpha, double *VInf, CVector &Force, CVector &Moment, bool bTilted, bool bTrace)
{
	// Calculates the forces using a farfield method
	// Calculates the moments by a near field method, i.e. direct summation on the panels
	// Downwash is evaluated at a distance 100 times the span downstream (i.e. infinite)
	//
	if(!m_pPanel||!m_pWPolar) return;

	static bool bOutRe, bError, bOut, bOutCl;
	static int j, k, l, p, pp, m, nw, iTA, iTB;
	static double cosa, sina, Re, PCd, Cl, Cp, tau, StripArea, ViscousDrag;
	static double QInf, QInfStrip, qdyn, GammaStrip;
	static CVector  C, PtC4, LeverArm, WindDirection, WindNormal, PanelLeverArm, Wg;
	static CVector Velocity, StripForce, ViscousMoment, dF, PanelForce, PanelForcep1;
	QMiarex *pMiarex= (QMiarex*)s_pMiarex;

	bOut = bOutCl = bError = false;

	int coef = 2;
	if (m_pWPolar->m_bThinSurfaces) coef = 1;

	cosa = cos(alpha*PI/180.0);
	sina = sin(alpha*PI/180.0);

	//   Define the wind axis
	WindNormal.Set(   -sina, 0.0, cosa);
	WindDirection.Set( cosa, 0.0, sina);

	p=m=0;

	Force.Set( 0.0, 0.0, 0.0);
	Moment.Set(0.0, 0.0, 0.0);
	ViscousDrag = 0.0;
	ViscousMoment.Set(0.0,0.0,0.0);

	for(j=0; j<m_NSurfaces; j++)
	{
		if(m_ppSurface[j]->m_bIsTipLeft && !m_pWPolar->m_bThinSurfaces) p+=m_ppSurface[j]->m_NXPanels;//tip patch panels

		for(k=0; k<m_ppSurface[j]->m_NYPanels; k++)
		{
			//Get the strip area
			pp=p;
			StripArea = 0.0;
			for (l=0; l<coef*m_ppSurface[j]->m_NXPanels; l++)
			{
				StripArea  += m_pPanel[pp].Area;
				pp++;
			}

			//Get the strip's lifting force
			if(m_pPanel[p].m_iPos!=0)
			{
				StripArea /=2.0;
				//FF force
				nw  = m_pPanel[p].m_iWake;
				iTA = m_pWakePanel[nw].m_iTA;
				iTB = m_pWakePanel[nw].m_iTB;
				C = (m_pWakeNode[iTA] + m_pWakeNode[iTB])/2.0;
				GetSpeedVector(C, Mu, Sigma, Wg, false);
				Wg.x += VInf[p            ];
				Wg.y += VInf[p+m_MatSize  ];
				Wg.z += VInf[p+2*m_MatSize];

				GammaStrip = (-Mu[p+coef*m_ppSurface[j]->m_NXPanels-1] + Mu[p]) *4.0*PI;

				StripForce  = m_pPanel[p].Vortex * Wg;
				StripForce *= GammaStrip;                            //Newtons/rho
				Force += StripForce;

				Velocity.x = *(VInf               +p);
				Velocity.y = *(VInf +   m_MatSize +p);
				Velocity.z = *(VInf + 2*m_MatSize +p);
				QInfStrip = Velocity.VAbs(); //used for viscous drag at the next step

				p+=m_ppSurface[j]->m_NXPanels*coef;
			}
			else
			{
				//iPos=0, VLM type panel
				StripForce.Set(0.0,0.0,0.0);
				for(l=0; l<m_ppSurface[j]->m_NXPanels; l++)
				{
					Velocity.x = *(VInf               +p);
					Velocity.y = *(VInf +   m_MatSize +p);
					Velocity.z = *(VInf + 2*m_MatSize +p);
					QInfStrip = Velocity.VAbs();

					//FF force
					if(m_pWPolar->m_bVLM1 || m_pPanel[p].m_bIsTrailing)
					{
						C = m_pPanel[p].CtrlPt;
						C.x = m_pWing->m_PlanformSpan * 100.0;

						GetSpeedVector(C, Mu, Sigma, Wg, false);
						Wg += Velocity; //total speed vector

						//induced force
						dF  = Wg * m_pPanel[p].Vortex;
						dF *=  Mu[p];  // N/rho

						Force += dF;        // N/rho
						StripForce += dF;
					}
					//On-Body moment
					PanelForce  = Velocity * m_pPanel[p].Vortex;
					PanelForce *= Mu[p];                                 //Newtons/rho

					if(!m_pWPolar->m_bVLM1 && !m_pPanel[p].m_bIsLeading)
					{
						PanelForcep1       = Velocity * m_pPanel[p].Vortex;
						PanelForcep1      *= Mu[p+1];                          //Newtons/rho

						PanelForce -= PanelForcep1;
					}

					PanelLeverArm = m_pPanel[p].VortexPos - m_pWPolar->m_CoG;
					Moment += PanelLeverArm * PanelForce;                     // N.m/rho
					p++;
				}

			}
			if(m_pWPolar->m_bViscous)
			{
				//add the viscous drag component to force and moment
				qdyn = 0.5 * m_pWPolar->m_Density * QInfStrip * QInfStrip;
				m_ppSurface[j]->GetC4(k, PtC4, tau);
				Re = m_ppSurface[j]->GetChord(tau) * QInfStrip /m_pWPolar->m_Viscosity;
				Cl = StripForce.dot(WindNormal)*m_pWPolar->m_Density/qdyn/StripArea;
				PCd    = GetVar(pMiarex->m_poaPolar, 2, m_ppSurface[j]->m_pFoilA, m_ppSurface[j]->m_pFoilB, Re, Cl, tau, bOutRe, bError);
				PCd   *= StripArea * 1/2*QInfStrip*QInfStrip;                // Newtons/rho
				bOut = bOut || bOutRe || bError;
				ViscousDrag += PCd ;                                         // Newtons/rho

				LeverArm   = PtC4 - m_pWPolar->m_CoG;

				ViscousMoment.x += PCd * (WindDirection.y*LeverArm.z - WindDirection.z*LeverArm.y);   // N.m/rho
				ViscousMoment.y += PCd * (WindDirection.z*LeverArm.x - WindDirection.x*LeverArm.z);
				ViscousMoment.z += PCd * (WindDirection.x*LeverArm.y - WindDirection.y*LeverArm.x);
			}
			m++;
		}
	}

	if(!m_pWPolar->m_bThinSurfaces)
	{
		//On-Body moment
		// same as before, except that we take into account tip patches
		CVector VLocal;
		Moment.Set(0.0,0.0,0.0);
		for(p=0; p<m_MatSize; p++)
		{
			Velocity.x = *(VInf               +p);
			Velocity.y = *(VInf +   m_MatSize +p);
			Velocity.z = *(VInf + 2*m_MatSize +p);
			QInf = Velocity.VAbs();

			GetDoubletDerivative(p, Mu, Cp, VLocal, QInf, Velocity.x, Velocity.y, Velocity.z);
			PanelForce = m_pPanel[p].Normal * (-Cp) * m_pPanel[p].Area *1/2.*QInf*QInf;      // Newtons/rho

			PanelLeverArm = m_pPanel[p].CollPt - m_pWPolar->m_CoG;
			Moment += PanelLeverArm * PanelForce;                     // N.m/rho
		}
	}

	if(m_pWPolar->m_bThinSurfaces) Force -= WindDirection*Force.dot(WindDirection)/2.0;
	if(m_pWPolar->m_bViscous)
	{
		Force += WindDirection * ViscousDrag;
		Moment += ViscousMoment;
	}

	Force  *= m_pWPolar->m_Density;                          // N
	Moment *= m_pWPolar->m_Density;                          // N.m
}



#define CM_ITER_MAX 50
bool PanelAnalysisDlg::GetZeroMomentAngle()
{
	//start loop to find zero-pitching-moment alpha
	static int iter;
	static double a, a0, a1, Cm, Cm0, Cm1, tmp;
	static double eps = 1.e-7;

	iter = 0;
	a0 = -PI/4.0;
	a1 =  PI/4.0;

	a = 0.0;
	Cm0 = ComputeCm(a0*180.0/PI);
	Cm1 = ComputeCm(a1*180.0/PI);
	Cm = 1.0;

	//are there two initial values of opposite signs ?
	while(Cm0*Cm1>0.0 && iter <=20)
	{
		a0 *=0.9;
		a1 *=0.9;
		Cm0 = ComputeCm(a0*180.0/PI);
		Cm1 = ComputeCm(a1*180.0/PI);
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
		Cm = ComputeCm(a*180.0/PI);
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
	Cm = ComputeCm(m_AlphaEq, true);// for information only, should be zero

	return true;
}



void PanelAnalysisDlg::BuildStateMatrices()
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
	double Mass = m_pWPolar->m_Mass;

	//use inertia measured in stability axis, CoG origin
	Ixx = m_Is[0][0];
	Iyy = m_Is[1][1];
	Izz = m_Is[2][2];
	Izx = m_Is[0][2];

	//____________________Longitudinal stability_____________
	m_ALong[0][0] = Xu/Mass;
	m_ALong[0][1] = Xw/Mass;
	m_ALong[0][2] = 0.0;
	m_ALong[0][3] = -9.81*cos(Theta0*PI/180.0);

	m_ALong[1][0] =  Zu                            /(Mass-Zwp);
	m_ALong[1][1] =  Zw                            /(Mass-Zwp);
	m_ALong[1][2] = (Zq+Mass*u0)                   /(Mass-Zwp);
	m_ALong[1][3] = -9.81*Mass*sin(Theta0*PI/180.0)/(Mass-Zwp);

	m_ALong[2][0] = (Mu + Mwp*Zu/(Mass-Zwp))                 /Iyy;
	m_ALong[2][1] = (Mw + Mwp*Zw/(Mass-Zwp))                 /Iyy;
	m_ALong[2][2] = (Mq + Mwp*(Zq+Mass*u0)/(Mass-Zwp))       /Iyy;
	m_ALong[2][3] = (Mwp*(-Mass*9.81*sin(Theta0))/(Mass-Zwp))/Iyy;

	m_ALong[3][0] = 0.0;
	m_ALong[3][1] = 0.0;
	m_ALong[3][2] = 1.0;
	m_ALong[3][3] = 0.0;

	strange = "      _____State matrices__________\n";
	AddString(strange);
	strange = "       Longitudinal state matrix\n";
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

	m_ALat[0][0] = Yv/Mass;
	m_ALat[0][1] = Yp/Mass;
	m_ALat[0][2] = Yr/Mass - u0;
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

	strange = "       Lateral state matrix\n";
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
//	for(i=0; i<m_pWPolar->m_nControls; i++)
	{
		// per radian
		m_BLong[0] = Xde/Mass;
		m_BLong[1] = Zde/Mass;
		m_BLong[2] = Mde/Iyy;
		m_BLong[3] = 0.0;

		m_BLat[0] = Yde/Mass;
		m_BLat[1] = Lde/Ipxx+Nde*Ipzx;
		m_BLat[2] = Lde*Ipzx+Nde/Ipzz;
		m_BLat[3] = 0.0;
	}

	strange = "      _____Control Matrices__________\n";
	AddString(strange);
	strange = "       Longitudinal control matrix\n";
	AddString(strange);

	strange = QString("        %1\n      %2\n      %3\n      %4\n\n")
				.arg(m_BLong[0], 13, 'g', 7)
				.arg(m_BLong[1], 13, 'g', 7)
				.arg(m_BLong[2], 13, 'g', 7)
				.arg(m_BLong[3], 13, 'g', 7);
	AddString(strange);
	strange = "       Lateral control matrix\n";
	AddString(strange);

	strange = QString("        %1\n      %2\n      %3\n      %4\n\n")
				.arg(m_BLat[0], 13, 'g', 7)
				.arg(m_BLat[1], 13, 'g', 7)
				.arg(m_BLat[2], 13, 'g', 7)
				.arg(m_BLat[3], 13, 'g', 7);
	AddString(strange);
}


void PanelAnalysisDlg::BuildRotationMatrix()
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


bool PanelAnalysisDlg::ComputeTrimmedConditions()
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
	int p;
	static double Lift, phi, VerticalCl;
	static CVector VInf, Force, Moment, WindNormal;

	// find aoa such that Cm=0;

	//Build the unit RHS vectors along x and z in Body Axis
	CreateUnitRHS();
	if (m_bCancel) return false;


	// build the influence matrix in Body Axis
	BuildInfluenceMatrix();
	if (m_bCancel) return false;

	if(!m_pWPolar->m_bThinSurfaces)
	{
		//compute wake contribution
		CreateWakeContribution();
		//add wake contribution to matrix and RHS
		for(int p=0; p<m_MatSize; p++)
		{
			m_uRHS[p]+= m_uWake[p];
			m_wRHS[p]+= m_wWake[p];
			for(int pp=0; pp<m_MatSize; pp++)
			{
				m_aij[p*m_MatSize+pp] += m_aijWake[p*m_MatSize+pp];
			}
		}
	}

	if (!SolveUnitRHS())	//solve for the u,w unit vectors
	{
		m_bWarning = true;
		return false;
	}

	strong ="      Searching for zero-moment angle... ";
	if(!GetZeroMomentAngle())
	{
		strong += "none found\n";
		AddString(strong);
		return false;
	}
	strong += QString("Alpha=%1").arg(m_AlphaEq,0,'f',5) + QString::fromUtf8("°") +"\n";
	AddString(strong);

	CreateSourceStrength(m_AlphaEq, 0.0, 1);
	if (m_bCancel) return true;

	//reconstruct doublet strengths from unit cosine and sine vectors
	CreateDoubletStrength(m_AlphaEq, 0.0, 1.0);
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

	//so far we have a unit Vortex Strength
	// find the speeds which will create a lift equal to the weight

	AddString("      Calculating speed to balance the weight...");

//	CWing::s_Viscosity = m_pWPolar->m_Viscosity;
//	CWing::s_Density   = m_pWPolar->m_Density;

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

	Forces(m_Mu, m_Sigma, m_AlphaEq, m_RHS+50*m_MatSize, Force, Moment, m_pWPolar->m_bTiltedGeom);

	phi = m_pWPolar->m_BankAngle *PI/180.0;
	Lift   = Force.dot(WindNormal);		//N/rho ; bank effect not included
	VerticalCl = Lift*2.0/m_pWPolar->m_WArea * cos(phi)/m_pWPolar->m_Density;
	if(Lift<=0.0)
	{
		u0 = -100.0;
		strong = QString("  Found a negative lift for Alpha=%1.... skipping the angle...\n").arg(m_AlphaEq,0,'f',5);
		if(m_bTrace) AddString(strong);
		m_bPointOut = true;
		m_bWarning = true;
		return false;
	}
	else
	{
		u0 =  sqrt( 2.0* 9.81 * m_pWPolar->m_Mass /m_pWPolar->m_Density/m_pWPolar->m_WArea / VerticalCl );
		strong = QString("VInf = %2").arg(u0*pMainFrame->m_mstoUnit,0,'f',5);
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

	//______________________________________________________________________________________
	// Scale circulations to speeds

	for(p=0; p<m_MatSize; p++)
	{
		m_Mu[p]    *= u0;
		m_Sigma[p] *= u0;
	}
	VInf *= u0;

	// Store the force and moment acting on the surfaces
	// Will be of use later for stability control derivatives
	// need to re-calculate because of viscous drag which is not linear

	for(p=0; p<m_MatSize; p++)
	{
		m_RHS[50*m_MatSize+p] = VInf.x;
		m_RHS[51*m_MatSize+p] = VInf.y;
		m_RHS[52*m_MatSize+p] = VInf.z;
	}

	Forces(m_Mu, m_Sigma, m_AlphaEq, m_RHS+50*m_MatSize, Force0, Moment0, m_pWPolar->m_bTiltedGeom);

	return true;
}


void PanelAnalysisDlg::ComputeStabilityDerivatives()
{
	// The stability derivatives are estimated by forward difference at U=(U0,0,0).
	// The reference condition has been saved during the calculation of the trimmed condition

	static CVector V0, Force, Moment, CGM, is, js, ks, Vi, Vj, Vk, Ris, Rjs, Rks, WindDirection, WindNormal;
	static int p;
	static double alpha, sina, cosa, deltaspeed, deltarotation;
	QString strong;
	int Size= m_MatSize;
	if(m_b3DSymetric) Size = m_SymSize;

	strong = "      Calculating the stability derivatives\n";
	AddString(strong);

	deltaspeed    = 0.01;         //  m/s   for forward difference estimation
	deltarotation = 0.001;       //  rad/s for forward difference estimation

	// Define the stability axes
	cosa = cos(m_AlphaEq*PI/180);
	sina = sin(m_AlphaEq*PI/180);
	WindDirection.Set(cosa, 0.0, sina);
	WindNormal.Set(-sina, 0.0, cosa);

	is.Set(-cosa, 0.0, -sina);
	js.Set(  0.0, 1.0,   0.0);
	ks.Set( sina, 0.0, -cosa);

	V0 = is * (-u0); //is the steady state velocity vector, if no sideslip

	//______________________________________________________________________________
	// RHS for unit speed vectors
	// The change in wind velocity is opposite to the change in plane velocity
	Vi = V0 - is * deltaspeed; //a positive increase in axial speed is a positive increase in wind speed
	Vj = V0 - js * deltaspeed; //a plane movement to the right is a wind flow to the left, i.e. negative y
	Vk = V0 - ks * deltaspeed; //a plane movement downwards (Z_stability>0) is a positive increase of V in geometry axes

	for (p=0; p<m_MatSize; p++)
	{
		//re-use existing memory to define the velocity field
		m_RHS[50*m_MatSize+p] = Vi.x;
		m_RHS[51*m_MatSize+p] = Vi.y;
		m_RHS[52*m_MatSize+p] = Vi.z;
		m_RHS[53*m_MatSize+p] = Vj.x;
		m_RHS[54*m_MatSize+p] = Vj.y;
		m_RHS[55*m_MatSize+p] = Vj.z;
		m_RHS[56*m_MatSize+p] = Vk.x;
		m_RHS[57*m_MatSize+p] = Vk.y;
		m_RHS[58*m_MatSize+p] = Vk.z;
		m_Sigma[p]             = -1.0/4.0/PI* (m_RHS[50*m_MatSize+p] *m_pPanel[p].Normal.x + m_RHS[51*m_MatSize+p] *m_pPanel[p].Normal.y + m_RHS[52*m_MatSize+p] *m_pPanel[p].Normal.z);
		m_Sigma[p+ m_MatSize]  = -1.0/4.0/PI* (m_RHS[53*m_MatSize+p] *m_pPanel[p].Normal.x + m_RHS[54*m_MatSize+p] *m_pPanel[p].Normal.y + m_RHS[55*m_MatSize+p] *m_pPanel[p].Normal.z);
		m_Sigma[p+2*m_MatSize] = -1.0/4.0/PI* (m_RHS[56*m_MatSize+p] *m_pPanel[p].Normal.x + m_RHS[57*m_MatSize+p] *m_pPanel[p].Normal.y + m_RHS[58*m_MatSize+p] *m_pPanel[p].Normal.z);
	}

	CreateRHS(m_uRHS, Vi);
	CreateRHS(m_vRHS, Vj);
	CreateRHS(m_wRHS, Vk);

	//______________________________________________________________________________
	// RHS for unit rotation vectors around Stability axis
	// stability axis origin is CoG

	for (p=0; p<m_MatSize; p++)
	{
		//re-use existing memory to define the velocity field
		if(m_pPanel[p].m_iPos==0) CGM = m_pPanel[p].VortexPos - m_pWPolar->m_CoG;
		else                      CGM = m_pPanel[p].CollPt    - m_pWPolar->m_CoG;

		// a rotation of the plane about a vector is the opposite of a rotation of the freestream about this vector
		Ris = is*CGM * (-deltarotation) + V0;
		Rjs = js*CGM * (-deltarotation) + V0;
		Rks = ks*CGM * (-deltarotation) + V0;

		m_RHS[59*m_MatSize+p] = Ris.x;
		m_RHS[60*m_MatSize+p] = Ris.y;
		m_RHS[61*m_MatSize+p] = Ris.z;
		m_RHS[62*m_MatSize+p] = Rjs.x;
		m_RHS[63*m_MatSize+p] = Rjs.y;
		m_RHS[64*m_MatSize+p] = Rjs.z;
		m_RHS[65*m_MatSize+p] = Rks.x;
		m_RHS[66*m_MatSize+p] = Rks.y;
		m_RHS[67*m_MatSize+p] = Rks.z;
		m_Sigma[p+3*m_MatSize] = -1.0/4.0/PI* (m_RHS[59*m_MatSize+p]*m_pPanel[p].Normal.x + m_RHS[60*m_MatSize+p]*m_pPanel[p].Normal.y + m_RHS[61*m_MatSize+p]*m_pPanel[p].Normal.z);
		m_Sigma[p+4*m_MatSize] = -1.0/4.0/PI* (m_RHS[62*m_MatSize+p]*m_pPanel[p].Normal.x + m_RHS[63*m_MatSize+p]*m_pPanel[p].Normal.y + m_RHS[64*m_MatSize+p]*m_pPanel[p].Normal.z);
		m_Sigma[p+5*m_MatSize] = -1.0/4.0/PI* (m_RHS[65*m_MatSize+p]*m_pPanel[p].Normal.x + m_RHS[66*m_MatSize+p]*m_pPanel[p].Normal.y + m_RHS[67*m_MatSize+p]*m_pPanel[p].Normal.z);
	}

	CreateRHS(m_pRHS, WindDirection, m_RHS+59*m_MatSize);
	CreateRHS(m_qRHS, WindDirection, m_RHS+62*m_MatSize);
	CreateRHS(m_rRHS, WindDirection, m_RHS+65*m_MatSize);

	if(!m_pWPolar->m_bThinSurfaces)
	{
		// Compute the wake's contribution
		// We ignore the perturbations and consider only the potential of the steady state flow
		// Clearly an approximation which is also implicit in the VLM formulation
		CreateWakeContribution(m_uWake,  WindDirection);// re-use m_uWake memory, which is re-calculated anyway at the next control iteration

		//add wake contribution to all 6 RHS
		for(p=0; p<m_MatSize; p++)
		{
			m_uRHS[p]+= m_uWake[p]*u0;
			m_vRHS[p]+= m_uWake[p]*u0;
			m_wRHS[p]+= m_uWake[p]*u0;
			m_pRHS[p]+= m_uWake[p]*u0;
			m_qRHS[p]+= m_uWake[p]*u0;
			m_rRHS[p]+= m_uWake[p]*u0;
		}
	}


	// The LU matrix is unchanged, so baksubstitute for unit vortex circulations
	Crout_LU_with_Pivoting_Solve(m_aij, m_uRHS, m_Index, m_RHS,             Size, &m_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_vRHS, m_Index, m_RHS+  m_MatSize, Size, &m_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_wRHS, m_Index, m_RHS+2*m_MatSize, Size, &m_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_pRHS, m_Index, m_RHS+3*m_MatSize, Size, &m_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_qRHS, m_Index, m_RHS+4*m_MatSize, Size, &m_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_rRHS, m_Index, m_RHS+5*m_MatSize, Size, &m_bCancel);

	memcpy(m_uRHS, m_RHS,             m_MatSize*sizeof(double));
	memcpy(m_vRHS, m_RHS+  m_MatSize, m_MatSize*sizeof(double));
	memcpy(m_wRHS, m_RHS+2*m_MatSize, m_MatSize*sizeof(double));
	memcpy(m_pRHS, m_RHS+3*m_MatSize, m_MatSize*sizeof(double));
	memcpy(m_qRHS, m_RHS+4*m_MatSize, m_MatSize*sizeof(double));
	memcpy(m_rRHS, m_RHS+5*m_MatSize, m_MatSize*sizeof(double));


	// Compute stabiliy and control derivatives
	Xu = Xw = Zu = Zw = Mu = Mw = Mq = Zwp = Mwp = 0.0;
	Yv = Yp = Yr = Lv = Lp = Lr = Nv = Np  = Nr  = 0.0;

	//________________________________________________
	// 1st ORDER STABILITY DERIVATIVES
	// x-derivatives________________________
	alpha = atan2(Vi.z, Vi.x) * 180.0/PI;// =m_AlphaEq....
	Forces(m_uRHS, m_Sigma, alpha, m_RHS+50*m_MatSize, Force, Moment, m_pWPolar->m_bTiltedGeom);
	Xu = (Force - Force0).dot(is)   /deltaspeed;
	Zu = (Force - Force0).dot(ks)   /deltaspeed;
	Mu = (Moment- Moment0).dot(js)  /deltaspeed;

	// y-derivatives________________________
	alpha = atan2(Vj.z, Vj.x)*180.0/PI;// =m_AlphaEq....
	Forces(m_vRHS, m_Sigma+m_MatSize, alpha, m_RHS+53*m_MatSize, Force, Moment, m_pWPolar->m_bTiltedGeom);
	Yv = (Force - Force0).dot(js)   /deltaspeed;
//	Lv = (Moment.dot(WindDirection) - Moment0.dot(is)) /deltaspeed;
	Nv = (Moment.dot(WindNormal)    - Moment0.dot(ks)) /deltaspeed;
	Lv = (Moment - Moment0).dot(is) /deltaspeed;
	Nv = (Moment - Moment0).dot(ks) /deltaspeed;

	// z-derivatives________________________
	alpha = atan2(Vk.z, Vk.x)* 180.0/PI;
	Forces(m_wRHS, m_Sigma+2*m_MatSize, alpha, m_RHS+56*m_MatSize, Force, Moment, m_pWPolar->m_bTiltedGeom);
	Xw = (Force - Force0).dot(is)   /deltaspeed;
	Zw = (Force - Force0).dot(ks)   /deltaspeed;
	Mw = (Moment - Moment0).dot(js) /deltaspeed;

	m_Progress +=1;
	qApp->processEvents();

	// p-derivatives
	Forces(m_pRHS, m_Sigma+3*m_MatSize, m_AlphaEq, m_RHS+59*m_MatSize, Force, Moment, m_pWPolar->m_bTiltedGeom);
	Yp = (Force-Force0).dot(js)   /deltarotation;
	Lp = (Moment-Moment0).dot(is) /deltarotation;
	Np = (Moment-Moment0).dot(ks) /deltarotation;

	// q-derivatives
	Forces(m_qRHS, m_Sigma+4*m_MatSize, m_AlphaEq, m_RHS+62*m_MatSize, Force, Moment, m_pWPolar->m_bTiltedGeom);
	Xq = (Force-Force0).dot(is)   /deltarotation;
	Zq = (Force-Force0).dot(ks)   /deltarotation;
	Mq = (Moment-Moment0).dot(js) /deltarotation;

	// r-derivatives
	Forces(m_rRHS, m_Sigma+5*m_MatSize, m_AlphaEq, m_RHS+65*m_MatSize, Force, Moment, m_pWPolar->m_bTiltedGeom);
	Yr = (Force-Force0).dot(js)   /deltarotation;
	Lr = (Moment-Moment0).dot(is) /deltarotation;
	Nr = (Moment-Moment0).dot(ks) /deltarotation;

	m_Progress +=1;
	qApp->processEvents();

	//________________________________________________
	// 2nd ORDER STABILITY DERIVATIVES
	// Zwp & Mwp ... ?
	// M. Drela's answer to the question posted on Yahoo Groups:

	/*	May I take this opportunity to ask you about the stability derivatives w.r.t. alpha dot ?
	Are they ignored in AVL and if so, is it a safe assumption ?

	Yes, the _alphadot derivatives are ignored, for the simple reason that there's no way to do it
	in an algorithmic way for a general configuration. The usual estimates in Etkin or Nelson assume
	 a "typical" tailed airplane configuration, with a small tail and a well-defined tail arm.
	These estimates won't work for other configurations like canard, tandem, or flying wing.

	Normally, only the Cm_alphadot derivative is significant, and slightly augments the pitch-damping
	derivative Cm_q. Leaving it out therefore underpredicts pitch damping slightly, so this is a
	conservative approximation. And pitch damping is not a major concern in any case.
	Simple static pitch stability is more important, and that's not affected by alphadot.

	All this is for a conventional configuration. Not sure what the impact is on the pitch damping of flying wings. */
}


void PanelAnalysisDlg::ComputeStabilityInertia()
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

void PanelAnalysisDlg::ComputeControlDerivatives()
{
	//
	// Calculates the control derivatives for small control deflections, using forward derivatives
	//
	// The geometry has been previously modified to set the control in the position
	// defined for this iteration of the analysis
	//
	// The problem is not linear, since we take into account viscous forces
	// Therefore to get the derivative, we need to make the difference between two states
	// We use forward differences between the reference state corresponding to the trimmed conditions
	// and the same state + application of a delta angle to all the controls
	//
	// AVL ignores the modification of the matrix terms and therefore requires only a
	// single LU decomposition throughout the problem. 
	//
	// The same method is applied here
	//   - Save the reference geometry
	//   - Change the control point positions and the normals of the flaps
	//   - re-generate the RHS vector
	//   - the geometry is reset at the next iteration loop
	//
	static CVector WindDirection, H, Force, Moment, V0, is, js, ks;
	static int j, p, pos, NCtrls;
	static double DeltaAngle, SignedDeltaAngle, q,S,b,mac,cosa, sina;
	QString str;
	Quaternion Quat;

	bool bActive = false;
	for(int c=0; c<m_NCtrls; c++)
	{
		if(!m_pWPolar->m_bAVLControls && m_pWPolar->m_bActiveControl[c])
		{
			bActive = true;
			break;
		}
		if(m_pWPolar->m_bAVLControls && fabs(m_pWPolar->m_MaxControl[c])>PRECISION)
		{
			bActive = true;
			break;
		}
	}
	if(!bActive)
	{
		str = "\n      No active control - skipping control derivatives\n\n\n";
		AddString(str);
		return;
	}

	// Define the stability axes and the freestream velocity field
	cosa = cos(m_AlphaEq*PI/180);
	sina = sin(m_AlphaEq*PI/180);
	V0.Set(u0*cosa, 0.0, u0*sina);
	WindDirection.Set(cosa, 0.0, sina);
	is.Set(-cosa, 0.0, -sina);
	js.Set(  0.0, 1.0,   0.0);
	ks.Set( sina, 0.0, -cosa);

	q = 1./2. * m_pWPolar->m_Density * u0 * u0;
	b   = m_pWPolar->m_WSpan;
	S   = m_pWPolar->m_WArea;
	mac = m_pWing->m_MAChord;

	DeltaAngle = 0.001;

	pos = 0;
	Xde = Yde =  Zde = Lde = Mde = Nde= 0.0;
	
	NCtrls = 0;

	if(m_pPlane)
	{
		//Wing tilt
		if ((!m_pWPolar->m_bAVLControls && m_pWPolar->m_bActiveControl[0]) ||
		    ( m_pWPolar->m_bAVLControls && fabs(m_pWPolar->m_MaxControl[0])>PRECISION))
		{
			//rotate the normals and control point positions
			H.Set(0.0, 1.0, 0.0);
			if(m_pWPolar->m_bAVLControls && fabs(m_pWPolar->m_MaxControl[0])>PRECISION)
				SignedDeltaAngle = DeltaAngle * m_pWPolar->m_MaxControl[0]/fabs(m_pWPolar->m_MaxControl[0]);
			else SignedDeltaAngle = DeltaAngle;
			Quat.Set(SignedDeltaAngle*180.0/PI, H);

			for(p=0; p<m_pWing->m_MatSize; p++)
			{
				(m_pWing->m_pPanel+p)->RotateBC(m_pPlane->WingLE(0), Quat);
			}
		}

		pos = m_pWing->m_MatSize;
		NCtrls = 1;
	}

	if(m_pPlane && m_pWingList[2])
	{
		//Elevator tilt
		if ((!m_pWPolar->m_bAVLControls && m_pWPolar->m_bActiveControl[1]) ||
		    ( m_pWPolar->m_bAVLControls && fabs(m_pWPolar->m_MaxControl[1])>PRECISION))
		{
			H.Set(0.0, 1.0, 0.0);

			if(m_pWPolar->m_bAVLControls && fabs(m_pWPolar->m_MaxControl[1])>PRECISION)
			   SignedDeltaAngle = DeltaAngle * m_pWPolar->m_MaxControl[1]/fabs(m_pWPolar->m_MaxControl[1]);
			else SignedDeltaAngle = DeltaAngle;
			Quat.Set(SignedDeltaAngle*180.0/PI, H);

			for(p=0; p<m_pWingList[2]->m_MatSize; p++)
			{
				(m_pWingList[2]->m_pPanel+p)->RotateBC(m_pPlane->WingLE(2), Quat);
			}
		}
		pos += m_pWingList[2]->m_MatSize;
		NCtrls = 2;
	}

	//flap tilt
	// TODO : exclude fin flaps, or add them to StabPolarDlg
	for (j=0; j<m_NSurfaces; j++)
	{
		if(m_ppSurface[j]->m_bTEFlap)
		{
			if ((!m_pWPolar->m_bAVLControls && m_pWPolar->m_bActiveControl[NCtrls]) ||
				( m_pWPolar->m_bAVLControls && fabs(m_pWPolar->m_MaxControl[NCtrls])>PRECISION))
			{
				//Add delta rotations to initial control setting and to wing or flap delta rotation
				//TODO : proportional to max setting ???
				if(m_pWPolar->m_bAVLControls && fabs(m_pWPolar->m_MaxControl[NCtrls])>PRECISION)
				   SignedDeltaAngle = DeltaAngle * m_pWPolar->m_MaxControl[NCtrls]/fabs(m_pWPolar->m_MaxControl[NCtrls]);
				else SignedDeltaAngle = DeltaAngle;

				Quat.Set(SignedDeltaAngle*180.0/PI, m_ppSurface[j]->m_HingeVector);
				for(p=0; p<m_MatSize;p++)
				{
					if(m_ppSurface[j]->IsFlapPanel(p))
					{
						m_pPanel[p].RotateBC(m_ppSurface[j]->m_HingePoint, Quat);
//						m_pPanel[p].RotatePanel(m_ppSurface[j]->m_HingePoint, Quat);
					}
				}
			}
			NCtrls++;
		}
	}

	//create the RHS
	CreateRHS(m_cRHS, V0);

	if(!m_pWPolar->m_bThinSurfaces)
	{
		CreateWakeContribution(m_uWake,  WindDirection);// re-use m_uWake memory, which is re-calculated anyway at the next control iteration
		for(p=0; p<m_MatSize; p++)	m_cRHS[p]+= m_uWake[p]*u0;
	}

	//Solve the system
	for (int p=0; p<m_MatSize; p++)
	{
		m_RHS[50*m_MatSize+p] = V0.x;
		m_RHS[51*m_MatSize+p] = V0.y;
		m_RHS[52*m_MatSize+p] = V0.z;
	}

	QString strong = "      Calculating the control derivatives\n\n";
	AddString(strong);

	Crout_LU_with_Pivoting_Solve(m_aij, m_cRHS, m_Index, m_RHS, m_MatSize, &m_bCancel);
	memcpy(m_cRHS, m_RHS, m_MatSize*sizeof(double));

	Forces(m_cRHS, m_Sigma, m_AlphaEq, m_RHS+50*m_MatSize, Force, Moment, m_pWPolar->m_bTiltedGeom);

	// make the forward difference with nominal results
	// which gives the stability derivative for a rotation of control ic
	Xde = (Force-Force0).dot(is)/DeltaAngle;
	Yde = (Force-Force0).dot(js)/DeltaAngle;
	Zde = (Force-Force0).dot(ks)/DeltaAngle;
	Lde = (Moment - Moment0).dot(is) /DeltaAngle;  // N.m/rad
	Mde = (Moment - Moment0).dot(js) /DeltaAngle;
	Nde = (Moment - Moment0).dot(ks) /DeltaAngle;
}


double PanelAnalysisDlg::ComputeCm(double Alpha, bool bTrace)
{
	// Returns the geometric pithing moment coefficient for the specified angle of attack
	// The effect of the viscous drag is not included

	static int p;
	static double Cm, cosa, sina, Gamma, Gammap1;
	static CVector VInf, Force, PanelLeverArm, ForcePt, PanelForce, WindDirection, VLocal;
	double Speed2, Cp;
	// Define the wind axis
	cosa = cos(Alpha*PI/180.0);
	sina = sin(Alpha*PI/180.0);
	WindDirection.Set( cosa, 0.0, sina);
	VInf.Set(cosa, 0.0, sina);

	Cm = 0.0;
	for(p=0; p<m_MatSize; p++)
	{
		//write vector operations in-line, more efficient
		if(m_pPanel[p].m_iPos!=0)
		{
			//first calculate Cp for this angle
			VLocal  = m_pPanel[p].GlobalToLocal(VInf);
			VLocal += m_uVl[p]*cosa + m_wVl[p]*sina;
			Speed2 = VLocal.x*VLocal.x + VLocal.y*VLocal.y;
			Cp  = 1.0-Speed2; // QInf=unit, /1.0/1.0;
			m_Cp[p] = Cp; //todo : remove, for information only

			//next calculate the force acting on the panel
			ForcePt = m_pPanel[p].CollPt;
			PanelForce = m_pPanel[p].Normal * (-Cp) * m_pPanel[p].Area;      // Newtons/q
		}
		else
		{
			// for each panel along the chord, add the lift coef
			Gamma   = m_uRHS[p]  *cosa + m_wRHS[p]  *sina;
			Gammap1 = m_uRHS[p+1]*cosa + m_wRHS[p+1]*sina;
			ForcePt = m_pPanel[p].VortexPos;

			PanelForce  = WindDirection * m_pPanel[p].Vortex;
			PanelForce *= 2.0 * Gamma;                                       //Newtons/q   (QInf = unit)
			if(!m_pWPolar->m_bVLM1 && !m_pPanel[p].m_bIsLeading)
			{
				Force       = WindDirection * m_pPanel[p].Vortex;
				Force      *= 2.0 * Gammap1;       //Newtons/q/QInf
				PanelForce -= Force;
			}
			m_Cp[p] = PanelForce.dot(m_pPanel[p].Normal)/m_pPanel[p].Area; //todo : remove, for information only
		}
		PanelLeverArm.x = ForcePt.x - m_pWPolar->m_CoG.x;
		PanelLeverArm.y = ForcePt.y - m_pWPolar->m_CoG.y;
		PanelLeverArm.z = ForcePt.z - m_pWPolar->m_CoG.z;
		Cm += -PanelLeverArm.x * PanelForce.z + PanelLeverArm.z*PanelForce.x; //N.m/rho
	}

	Cm *= m_pWPolar->m_Density;
	return Cm;
}



void PanelAnalysisDlg::VLMQmn(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB, CVector const &C, CVector &V)
{
	//Quadrilateral VLM FORMULATION
	// LA, LB, TA, TB are the vortex's four corners
	// LA and LB are at the 3/4 point of panel nx
	// TA and TB are at the 3/4 point of panel nx+1
	//
	//    LA__________LB               |
	//    |           |                |
	//    |           |                | freestream speed
	//    |           |                |
	//    |           |                \/
	//    |           |
	//    TA__________TB
	//
	//
	//
	// C is the point where the induced speed is calculated
	// V is the resulting speed
	//
	// Vectorial operations are written explicitly to save computing times (4x more efficient)
	//
	double CoreSize = 0.0001;
	if(fabs(*m_pCoreSize)>1.e-10) CoreSize = *m_pCoreSize;


	V.x = 0.0;
	V.y = 0.0;
	V.z = 0.0;

	R[0].x = LB.x;
	R[0].y = LB.y;
	R[0].z = LB.z;
	R[1].x = TB.x;
	R[1].y = TB.y;
	R[1].z = TB.z;
	R[2].x = TA.x;
	R[2].y = TA.y;
	R[2].z = TA.z;
	R[3].x = LA.x;
	R[3].y = LA.y;
	R[3].z = LA.z;
	R[4].x = LB.x;
	R[4].y = LB.y;
	R[4].z = LB.z;

	for (int i=0; i<4; i++)
	{
		r0.x = R[i+1].x - R[i].x;
		r0.y = R[i+1].y - R[i].y;
		r0.z = R[i+1].z - R[i].z;
		r1.x = C.x - R[i].x;
		r1.y = C.y - R[i].y;
		r1.z = C.z - R[i].z;
		r2.x = C.x - R[i+1].x;
		r2.y = C.y - R[i+1].y;
		r2.z = C.z - R[i+1].z;

		Psi.x = r1.y*r2.z - r1.z*r2.y;
		Psi.y =-r1.x*r2.z + r1.z*r2.x;
		Psi.z = r1.x*r2.y - r1.y*r2.x;

		ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

		r1v = sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z));
		r2v = sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

		//get the distance of the TestPoint to the panel's side
		t.x =  r1.y*r0.z - r1.z*r0.y;
		t.y = -r1.x*r0.z + r1.z*r0.x;
		t.z =  r1.x*r0.y - r1.y*r0.x;

		if ((t.x*t.x+t.y*t.y+t.z*t.z)/(r0.x*r0.x+r0.y*r0.y+r0.z*r0.z) > CoreSize * CoreSize)
		{
			Psi.x /= ftmp;
			Psi.y /= ftmp;
			Psi.z /= ftmp;

			Omega = (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/r1v - (r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/r2v;
			V.x += Psi.x * Omega/4.0/PI;
			V.y += Psi.y * Omega/4.0/PI;
			V.z += Psi.z * Omega/4.0/PI;
		}
	}
}



void PanelAnalysisDlg::VLMCmn(CVector const &A, CVector const &B, CVector const &C, CVector &V, bool const &bAll)
{
	// CLASSIC VLM FORMULATION/
	//
	//    LA__________LB               |
	//    |           |                |
	//    |           |                | freestream speed
	//    |           |                |
	//    |           |                \/
	//    |           |
	//    \/          \/
	//
	// Note : the geometry has been rotated by sideslip, hence, there is no need to align the trailing vortices with sideslip
	//
	// V is the resulting speed at point C
	//
	// Vectorial operations are written inline to save computing times
	// -->longer code, but 4x more efficient....

	double CoreSize = 0.0001;
	if(fabs(*m_pCoreSize)>1.e-10) CoreSize = *m_pCoreSize;

	V.x = 0.0;
	V.y = 0.0;
	V.z = 0.0;

	if(bAll)
	{
		r0.x = B.x - A.x;
		r0.y = B.y - A.y;
		r0.z = B.z - A.z;

		r1.x = C.x - A.x;
		r1.y = C.y - A.y;
		r1.z = C.z - A.z;

		r2.x = C.x - B.x;
		r2.y = C.y - B.y;
		r2.z = C.z - B.z;

		Psi.x = r1.y*r2.z - r1.z*r2.y;
		Psi.y =-r1.x*r2.z + r1.z*r2.x;
		Psi.z = r1.x*r2.y - r1.y*r2.x;

		ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

		//get the distance of the TestPoint to the panel's side
		t.x =  r1.y*r0.z - r1.z*r0.y;
		t.y = -r1.x*r0.z + r1.z*r0.x;
		t.z =  r1.x*r0.y - r1.y*r0.x;

		if ((t.x*t.x+t.y*t.y+t.z*t.z)/(r0.x*r0.x+r0.y*r0.y+r0.z*r0.z) >CoreSize * CoreSize)
		{
			Psi.x /= ftmp;
			Psi.y /= ftmp;
			Psi.z /= ftmp;

			Omega = (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z))
				   -(r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

			V.x = Psi.x * Omega/4.0/PI;
			V.y = Psi.y * Omega/4.0/PI;
			V.z = Psi.z * Omega/4.0/PI;
		}
	}

	// We create Far points to align the trailing vortices with the reference axis
	// The trailing vortex legs are not aligned with the free-stream, i.a.w. the small angle approximation
	// If this approximation is not valid, then the geometry should be tilted in the polar definition

	// calculate left contribution
	Far.x = A.x +  10000.0; //10 km... should be enough
	Far.y = A.y;
	Far.z = A.z;// + (Far.x-A.x) * tan(m_Alpha*PI/180.0);

	r0.x = A.x - Far.x;
	r0.y = A.y - Far.y;
	r0.z = A.z - Far.z;

	r1.x = C.x - A.x;
	r1.y = C.y - A.y;
	r1.z = C.z - A.z;

	r2.x = C.x - Far.x;
	r2.y = C.y - Far.y;
	r2.z = C.z - Far.z;

	Psi.x = r1.y*r2.z - r1.z*r2.y;
	Psi.y =-r1.x*r2.z + r1.z*r2.x;
	Psi.z = r1.x*r2.y - r1.y*r2.x;

	ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

	t.x=1.0; t.y=0.0; t.z=0.0;

	h.x =  r1.y*t.z - r1.z*t.y;
	h.y = -r1.x*t.z + r1.z*t.x;
	h.z =  r1.x*t.y - r1.y*t.x;

	//Next add 'left' semi-infinite contribution
	//eq.6-56

	if ((h.x*h.x+h.y*h.y+h.z*h.z) > CoreSize * CoreSize)
	{
		Psi.x /= ftmp;
		Psi.y /= ftmp;
		Psi.z /= ftmp;

		Omega =  (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z))
				-(r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

		V.x += Psi.x * Omega/4.0/PI;
		V.y += Psi.y * Omega/4.0/PI;
		V.z += Psi.z * Omega/4.0/PI;
	}

	// calculate right vortex contribution
	Far.x = B.x +  10000.0;
	Far.y = B.y ;
	Far.z = B.z;// + (Far.x-B.x) * tan(m_Alpha*PI/180.0);

	r0.x = Far.x - B.x;
	r0.y = Far.y - B.y;
	r0.z = Far.z - B.z;

	r1.x = C.x - Far.x;
	r1.y = C.y - Far.y;
	r1.z = C.z - Far.z;

	r2.x = C.x - B.x;
	r2.y = C.y - B.y;
	r2.z = C.z - B.z;

	Psi.x = r1.y*r2.z - r1.z*r2.y;
	Psi.y =-r1.x*r2.z + r1.z*r2.x;
	Psi.z = r1.x*r2.y - r1.y*r2.x;

	ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

	//Last add 'right' semi-infinite contribution
	//eq.6-57
	h.x =  r2.y*t.z - r2.z*t.y;
	h.y = -r2.x*t.z + r2.z*t.x;
	h.z =  r2.x*t.y - r2.y*t.x;

	if ((h.x*h.x+h.y*h.y+h.z*h.z) > CoreSize * CoreSize)
	{
		Psi.x /= ftmp;
		Psi.y /= ftmp;
		Psi.z /= ftmp;

		Omega =  (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z))
			-(r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

		V.x += Psi.x * Omega/4.0/PI;
		V.y += Psi.y * Omega/4.0/PI;
		V.z += Psi.z * Omega/4.0/PI;
	}
}




