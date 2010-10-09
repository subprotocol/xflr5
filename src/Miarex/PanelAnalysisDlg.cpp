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
	m_bDirichlet     = false;//true if Dirichlet boundary conditions, false if Neumann
	m_bCancel        = false;
	m_bTrefftz       = false;

	m_CL = m_CX = m_CY = 0.0;
	m_GCm = m_GRm = m_GYm = m_VCm = m_VYm = m_IYm = 0.0;

	m_MatSize        = 0;
	m_nNodes         = 0;
	m_NSurfaces      = 0;
	m_NWakeColumn    = 0;
	m_WakeInterNodes = 1;
	m_MaxWakeIter    = 0;

	m_nWakeNodes = 0;
	m_WakeSize   = 0;

	m_strOut = "";

	m_pWingList[0] = m_pWingList[1] = m_pWingList[2] = m_pWingList[3] = NULL;
	m_ppBody  = NULL;
	m_pWing   = NULL; 
	m_pWing2  = NULL;
	m_pStab   = NULL;
	m_pFin    = NULL;
	m_pPlane  = NULL;
	s_pMiarex        = NULL;
	s_pMainFrame     = NULL;
	m_pWPolar        = NULL;

	m_ppPanel        = NULL;
	m_pPanel         = NULL;
	m_pWakePanel     = NULL;
	m_pRefWakePanel  = NULL;
	m_pMemPanel      = NULL;
	m_pNode          = NULL;
	m_pMemNode       = NULL;
	m_pWakeNode      = NULL;
	m_pRefWakeNode   = NULL;
	m_pTempWakeNode  = NULL;

	m_aij = m_aijRef = NULL;
	m_RHS = m_RHSRef = NULL;

	memset(m_Sigma,  0, sizeof(m_Sigma));
	memset(m_Mu,     0, sizeof(m_Mu));
	memset(m_Cp,     0, sizeof(m_Cp));
	memset(m_Speed,  0, sizeof(m_Speed));
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
	int nrhs, TotalTime;

	if(m_AlphaMax<m_Alpha) m_AlphaDelta = -fabs(m_AlphaDelta);
	nrhs  = (int)fabs((m_AlphaMax-m_Alpha)*1.0001/m_AlphaDelta) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100)
	{
		QMessageBox::warning(this, tr("Warning"),tr("The number of points to be calculated will be limited to 100"));
		nrhs = 100;
	}

	int MaxWakeIter = 1;
	int Size;
	if(m_b3DSymetric) Size = m_MatSize/2;
	else              Size = m_MatSize;


//ESTIMATED UNIT TIMES FOR OPERATIONS
//CreateMatrix :		   10 *Size/400
//CreateRHS :			   10
//CreateWakeContribution :  1
//SolveMultiple :		   40 *Size/400
//CreateDoubletStrength : 	1 x nrhs
//RelaxWake :			 20 x nrhs x MaxWakeIter *
//ComputeAeroCoefs :		5 x nrhs
	
	TotalTime = 10*Size/400 + 10 + 1 + 40*Size/400 + 1*nrhs + 5*nrhs;

	if(m_pWPolar->m_bWakeRollUp) TotalTime += 20*nrhs*MaxWakeIter;
//	if(!m_b3DSymetric) TotalTime+=30;//Solve multiple is 4x longer

	m_pctrlProgress->setMinimum(0);
	m_pctrlProgress->setMaximum(TotalTime);
	m_Progress = 0.0;
	qApp->processEvents();

	str = QString(tr("   Solving the problem... ")+"\n");
	AddString(str);

	CreateMatrix();
	if (m_bCancel) return true;

	CreateRHS(m_Alpha, m_AlphaDelta, nrhs);
	if (m_bCancel) return true;

	if(!m_pWPolar->m_bThinSurfaces) CreateWakeContribution();
	else                            memcpy(m_aij, m_aijRef, m_MatSize*m_MatSize*sizeof(double));

	if (m_bCancel) return true;

	if (!SolveMultiple(m_Alpha, m_AlphaDelta, nrhs))
	{
		AddString("\n\n"+tr("Singular matrix - aborting....")+"\n");
		m_bWarning = true;
		return true;
	}

	if (m_bCancel) return true;

	CreateDoubletStrength(m_Alpha, m_AlphaDelta, nrhs);
	if (m_bCancel) return true;


	ComputeAeroCoefs(m_Alpha, m_AlphaDelta, nrhs);

	return true;
}


void PanelAnalysisDlg::CreateMatrix()
{
	CVector C, CC, V, VS;
	int p, pp, Size;
	double phi, phiSym;

	AddString(tr("    Creating the influence matrix...")+"\n");
	if(m_b3DSymetric) Size = m_MatSize/2;
	else              Size = m_MatSize;

	for(p=0; p<Size; p++)
	{
		if(m_bCancel) return;

		//for each Boundary Condition point
		if(m_ppPanel[p]->m_iPos!=0)
		{
			//Thick surfaces, 3D-panel type BC, use collocation point
			C    = m_ppPanel[p]->CollPt;
		}
		else
		{
			//Thin surface, VLM type BC, use control point
			C    = m_ppPanel[p]->CtrlPt;
		}
		CC.x =  C.x;//symmetric point, just in case
		CC.y = -C.y;
		CC.z =  C.z;

		for(pp=0; pp<Size; pp++)
		{
			if(m_bCancel) return;
			//for each panel, get the unit doublet or vortex influence at the boundary condition pt

			GetDoubletInfluence(C, m_ppPanel[pp], V, phi);

			if(m_b3DSymetric && !m_ppPanel[pp]->m_bIsInSymPlane) // add symmetric contribution
			{
				GetDoubletInfluence(CC, m_ppPanel[pp], VS, phiSym);

				V.x += VS.x;
				V.y -= VS.y;
				V.z += VS.z;

				phi += phiSym;
			}
			if(!m_bDirichlet || m_ppPanel[p]->m_iPos==0)   m_aijRef[p*Size+pp] = V.dot(m_ppPanel[p]->Normal);
			else if(m_bDirichlet)	                       m_aijRef[p*Size+pp] = phi;

		}
		m_Progress += 10.0*(double)Size/400./(double)Size;
		qApp->processEvents();
	}
}


void PanelAnalysisDlg::CreateRHS(double V0, double VDelta, int nval)
{
	// Creates the RHS of linear problem, using boundary condition 
	// BC may be of the Neumann or Dirichlet type depending on the analysis type and on the geometry
	//
	// if 3d-panel, use NASA 4023 equation (20) & (22)
	// 

	// Compute with a unit speed
	// We'll scale the results to speed later depending on the polar type
	//
	
	int p, pp, q, m, Size;
	double alpha, phi, phiSym;
	CVector V, VS, C, CC;
	CVector QInf[VLMMAXRHS];

	if(m_b3DSymetric) Size = m_MatSize/2;
	else              Size = m_MatSize;

	AddString(tr("      Creating RHS vector...")+"\n");
	p=0;

	for (q=0; q<nval;q++)
	{
		alpha = V0+q*VDelta;
		QInf[q].Set(cos(alpha*PI/180.0), 0.0, sin(alpha*PI/180.0));

		for (pp=0; pp< m_MatSize; pp++)
		{
			if(m_bCancel) return;
			if(m_ppPanel[pp]->m_iPos!=0) m_Sigma[p] = -1.0/4.0/PI* QInf[q].dot(m_ppPanel[pp]->Normal);
			else                         m_Sigma[p] =  0.0;
			p++;
		}
	}

	// Add influence of sources to RHS
	m = 0;
	for (p=0; p<Size; p++)
	{
		if(m_bCancel) return;

		if(!m_bDirichlet || m_ppPanel[p]->m_iPos==0) 
		{
			m_cosRHS[m] = - m_ppPanel[p]->Normal.x;
			m_sinRHS[m] = - m_ppPanel[p]->Normal.z;
		}
		else if(m_bDirichlet) 
		{
			m_cosRHS[m] = 0.0;
			m_sinRHS[m] = 0.0;
		}

		if(m_ppPanel[p]->m_iPos!=0)
		{
			//Thick surfaces, 3D-panel type BC, use collocation point
			C    = m_ppPanel[p]->CollPt;
		}
		else
		{
			//Thin surface, VLM type BC, use control point
			C    = m_ppPanel[p]->CtrlPt;
		}
		CC.x =  C.x;//symmetric point, just in case
		CC.y = -C.y;
		CC.z =  C.z;

		for (pp=0; pp<Size; pp++)
		{
			if(m_ppPanel[pp]->m_iPos!=0) GetSourceInfluence(C, *(m_ppPanel+pp), V, phi);
			else
			{
				//sigma is zero on a thin surface, no need to compute the influence of this panel
				V.Set(0.0, 0.0, 0.0);
				phi = 0.0;
			}

			if(!m_bDirichlet || m_ppPanel[p]->m_iPos==0) 
			{
				// V is zero anyway ???
				m_cosRHS[m] -= V.dot(m_ppPanel[p]->Normal) * m_ppPanel[pp]->Normal.x * -1.0/4.0/PI;
				m_sinRHS[m] -= V.dot(m_ppPanel[p]->Normal) * m_ppPanel[pp]->Normal.z * -1.0/4.0/PI;
			}
			else if(m_bDirichlet)
			{
				m_cosRHS[m] -= phi * m_ppPanel[pp]->Normal.x * -1.0/4.0/PI;
				m_sinRHS[m] -= phi * m_ppPanel[pp]->Normal.z * -1.0/4.0/PI;
			}

			if(m_b3DSymetric && !m_ppPanel[pp]->m_bIsInSymPlane) // add right wing contribution
			{
				if(m_ppPanel[pp]->m_iPos!=0)	GetSourceInfluence(CC, *(m_ppPanel+pp), VS, phiSym);
				else
				{
					//sigma is zero on a thin surface
					VS.Set(0.0, 0.0, 0.0);
					phiSym = 0.0;
				}

				VS.y = -VS.y;
							
				if(!m_bDirichlet || m_ppPanel[p]->m_iPos==0) 
				{
					m_cosRHS[m] -= VS.dot(m_ppPanel[p]->Normal) * m_ppPanel[pp]->Normal.x * -1.0/4.0/PI;
					m_sinRHS[m] -= VS.dot(m_ppPanel[p]->Normal) * m_ppPanel[pp]->Normal.z * -1.0/4.0/PI;
				}
				else if(m_bDirichlet)
				{
					m_cosRHS[m] -= phiSym * m_ppPanel[pp]->Normal.x * -1.0/4.0/PI;
					m_sinRHS[m] -= phiSym * m_ppPanel[pp]->Normal.z * -1.0/4.0/PI;
				}
			}
		}
		m++;
		m_Progress += 10.0/(double)Size;
		qApp->processEvents();
	}
}


void PanelAnalysisDlg::CreateWakeContribution()
{
	//______________________________________________________________________________________
	// Method :
	// 	- follow the method described in NASA 4023 eq. (44)
	//	- add the wake's doublet contribution to the matrix
	//	- add the potential difference at the trailing edge panels to the RHS
	//______________________________________________________________________________________

	int kw, lw, pw, p, pp;
	int Size;
	CVector V, VS, C, CC;
	double phi, phiSym;
//	double Delta_phi_inf = 0.0;
	double PHC[MAXSTATIONS];
	CVector VHC[MAXSTATIONS];
//	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	AddString(tr("      Adding the wake's contribution...")+"\n");

	if(m_b3DSymetric) Size = m_MatSize/2;
	else              Size = m_MatSize;

	memcpy(m_aij, m_aijRef, m_MatSize * m_MatSize * sizeof(double));

	for(p=0; p<Size; p++)//for each matrix row
	{
		if(m_bCancel) return;

		C    = m_ppPanel[p]->CollPt;
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
				GetDoubletInfluence(C,  m_pWakePanel+pw, V, phi, true, true);
				PHC[kw] += phi;
				VHC[kw] += V;

				if(m_b3DSymetric && !m_pWakePanel[pw].m_bIsInSymPlane) // add right wing contribution
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
		//Add the contributions to the matrix coefficients and to the RHS

		for(pp=0; pp<Size; pp++) //for each matrix column
		{
			if(m_bCancel) return;

			// Is the panel pp shedding a wake ?
			if(m_ppPanel[pp]->m_bIsTrailing)
			{
				//If so, we need to add the contributions of the wake column shedded by this panel to the RHS and to the Matrix

				if(m_ppPanel[pp]->m_iPos == 0)
				{
					//The panel shedding a wake is on a thin surface
					if(!m_bDirichlet || m_ppPanel[p]->m_iPos==0)
					{
						//then add the velocity contribution of the wake column to the matrix coefficient
						m_aij[p*Size+pp] += VHC[m_ppPanel[pp]->m_iWakeColumn].dot(m_ppPanel[p]->Normal);
						//we do not add the term Phi_inf_KWPUM - Phi_inf_KWPLM (eq. 44) since it is 0, thin edge
					}
					else if(m_bDirichlet)
					{
						//then add the potential contribution of the wake column to the matrix coefficient
						m_aij[p*Size+pp] += PHC[m_ppPanel[pp]->m_iWakeColumn];
						//we do not add the term Phi_inf_KWPUM - Phi_inf_KWPLM (eq. 44) since it is 0, thin edge
					}
				}
				else if(m_ppPanel[pp]->m_iPos == -1)//bottom side, substract
				{
					if(!m_bDirichlet || m_ppPanel[p]->m_iPos==0)
					{
						//use Neumann B.C.
						m_aij[p*Size+pp] -= VHC[m_ppPanel[pp]->m_iWakeColumn].dot(m_ppPanel[p]->Normal);
						m_cosRHS[p] -= m_ppPanel[pp]->CollPt.x  * VHC[m_ppPanel[pp]->m_iWakeColumn].x * m_ppPanel[p]->Normal.x;
						m_sinRHS[p] -= m_ppPanel[pp]->CollPt.z  * VHC[m_ppPanel[pp]->m_iWakeColumn].z * m_ppPanel[p]->Normal.z;
					}
					else if(m_bDirichlet)
					{
						m_aij[p*Size+pp] -= PHC[m_ppPanel[pp]->m_iWakeColumn];
						m_cosRHS[p] +=  m_ppPanel[pp]->CollPt.x * PHC[m_ppPanel[pp]->m_iWakeColumn];
						m_sinRHS[p] +=  m_ppPanel[pp]->CollPt.z * PHC[m_ppPanel[pp]->m_iWakeColumn];
					}
				}
				else if(m_ppPanel[pp]->m_iPos == 1)  //top side, add
				{
					if(!m_bDirichlet || m_ppPanel[p]->m_iPos==0)
					{
						//use Neumann B.C.
						m_aij[p*Size+pp] += VHC[m_ppPanel[pp]->m_iWakeColumn].dot(m_ppPanel[p]->Normal);
						m_cosRHS[p] += m_ppPanel[pp]->CollPt.x * VHC[m_ppPanel[pp]->m_iWakeColumn].x * m_ppPanel[p]->Normal.x;
						m_sinRHS[p] += m_ppPanel[pp]->CollPt.z * VHC[m_ppPanel[pp]->m_iWakeColumn].z * m_ppPanel[p]->Normal.z;
					}
					else if(m_bDirichlet)
					{
						m_aij[p*Size+pp] += PHC[m_ppPanel[pp]->m_iWakeColumn];
						m_cosRHS[p] -= m_ppPanel[pp]->CollPt.x * PHC[m_ppPanel[pp]->m_iWakeColumn];
						m_sinRHS[p] -= m_ppPanel[pp]->CollPt.z * PHC[m_ppPanel[pp]->m_iWakeColumn];
					}
				}
			}
		}

		m_Progress += 1.0/(double)Size;
		qApp->processEvents();
	}
}



void PanelAnalysisDlg::CreateDoubletStrength(double V0, double VDelta, int nval)
{
	//______________________________________________________________________________________
	// Method : 
	// 	First calculate the Cp coefficients
	// 	Deduce the lift for unit speed
	// 	Calculate the speeds i.a.w. the polar's type
	// 	Scale the doublet and source strength i.a.w. the speeds
	//______________________________________________________________________________________

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
//	QMiarex *pMiarex   = (QMiarex*)s_pMiarex;

	QString strong, strange;
	int p, q, pp;
	double alpha;
	CVector WindNormal;


	//______________________________________________________________________________________
	// Scale the speeds i.a.w. the polar's type

	if(m_pWPolar->m_Type==2)
	{
		AddString(tr("      Calculating speeds to balance the weight")+"\n");
		int pos;
		double *Mu, *Sigma;
		double Lift, IDrag ,TempCl;

		CVector Force;

		CWing::s_Viscosity = m_pWPolar->m_Viscosity;
		CWing::s_Density   = m_pWPolar->m_Density;
		for (q=0; q<nval;q++)
		{
			alpha = V0+q*VDelta;
			WindNormal.Set(-sin(alpha*PI/180.0),   0.0, cos(alpha*PI/180.0));
			Lift = 0.0;
			p=0;

			Mu     = m_Mu    + q*m_MatSize;
			Sigma  = m_Sigma + q*m_MatSize;
			Force.Set(0.0, 0.0, 0.0);
		
//			CWing::s_QInf      = 1.0;
//			CWing::s_Alpha     = alpha;

			pos = 0;
			for(int i=0; i<4; i++)
			{
				if(m_pWingList[i])
				{
					m_pWingList[i]->PanelTrefftz(1.0, alpha, Mu, Sigma, pos, Force, IDrag, m_pWPolar, m_pWakePanel, m_pWakeNode);
					pos += m_pWingList[i]->m_MatSize;
				}
			}

			Lift =  Force.dot(WindNormal) ;//N/q, for 1/ms
			TempCl = Lift/m_pWPolar->m_WArea;
			if(Lift<=0.0)
			{
				strong = QString(tr("      Found a negative lift for Alpha=%1.... skipping the angle...")+"\n").arg(V0+q*VDelta,7,'f',2);
				AddString(strong);
				m_bPointOut = true;
				m_bWarning  = true;
				m_3DQInf[q] = -100.0;
			}
			else
			{
//				m_3DQInf[q] =  sqrt(2.0* 9.81 * m_pWPolar->m_Weight/m_pWPolar->m_Density/Lift/m_pWPolar->m_WArea);
				m_3DQInf[q] =  sqrt(2.0* 9.81 * m_pWPolar->m_Weight/m_pWPolar->m_Density/TempCl/m_pWPolar->m_WArea);
				strong = QString(tr("      Alpha=%1   QInf = %2")).arg(V0+q*VDelta,5,'f',2).arg(m_3DQInf[q]*pMainFrame->m_mstoUnit,5,'f',2);
				GetSpeedUnit(strange, pMainFrame->m_SpeedUnit);
				strong+= strange + "\n";
				AddString(strong);
			}
		}
	}

	else if (m_pWPolar->m_Type==1)
		for (q=0; q<nval;q++) m_3DQInf[q] = m_pWPolar->m_QInf;

	else if (m_pWPolar->m_Type==4)
		for (q=0; q<nval;q++) m_3DQInf[q] = V0 + q*VDelta;

	//______________________________________________________________________________________
	// Scale RHS and Sigma i.a.w. speeds (so far we have unit doublet and source strengths)

	double *SigmaRef = m_aij;//use existing reserved memory, do not re-allocate

	memcpy(SigmaRef, m_Sigma, nval*m_MatSize*sizeof(double));
	memcpy(m_RHSRef, m_Mu,   nval*m_MatSize*sizeof(double));

	if(m_pWPolar->m_Type!=4)
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

	//______________________________________________________________________________________
	AddString(tr("      Computing On-Body Speeds...")+"\n");

	if(m_pWPolar->m_Type !=4 )
	{
		for (q=0; q<nval; q++)
		{
			if(m_bCancel) return;
			ComputeOnBody(q, V0+q*VDelta);
			m_Progress += 1.0 *(double)nval/(double)nval;
			qApp->processEvents();
		}
	}
	else
	{
		ComputeOnBody(0, m_Alpha);
		for (q=1; q<nval; q++)
		{
			if(m_bCancel) return;
			for (p=0; p<m_MatSize; p++)
			{
				m_Cp[p+q*m_MatSize] = m_Cp[p];
			}
			m_Progress += 1.0  *(double)nval/(double)nval;
			qApp->processEvents();
		}
	}
}


void PanelAnalysisDlg::ComputeAeroCoefs(double V0, double VDelta, int nrhs)
{
	// calculates the various wing coefficients by interpolating
	// the adequate variable, from Cl, on the XFoil polar mesh
	// at each span station
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	int q;
	QString str, strong;

	if(m_pWPolar->m_Type !=4)
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
				ComputePlane(V0+q*VDelta, q);
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
			ComputePlane(m_Alpha, q);
			m_Progress += 5.0 /(double)nrhs;
			qApp->processEvents();
		}
	}
}


void PanelAnalysisDlg::ComputePlane(double Alpha, int qrhs)
{
	// calculates the various wing coefficients by interpolating
	// the adequate variable, from Cl, on the XFoil polar mesh
	// at each span station
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;
	int pos, Station;
	double *Mu, *Sigma;
	double cosa, sina;
	double Lift, IDrag, VDrag ,XCP, YCP, QInf, WingIDrag, WingVDrag;
	CVector WindNormal, WindDirection, WindSide;
	CVector Force;
	QString OutString;

	bool bThinSurf = m_pWPolar->m_bThinSurfaces;

	//   Define wind (stability) axis
	cosa = cos(Alpha*PI/180.0);
	sina = sin(Alpha*PI/180.0);

	WindNormal.Set(-sina, 0.0, cosa);
	WindDirection.Set(cosa, 0.0, sina);
	WindSide = WindNormal * WindDirection;
	
	Mu     = m_Mu    + qrhs*m_MatSize;
	Sigma  = m_Sigma + qrhs*m_MatSize;
	QInf        = m_3DQInf[qrhs];
	m_QInf      = m_3DQInf[qrhs];
	if(m_pWPolar->m_bTiltedGeom) Alpha = m_OpAlpha;
	else                         m_OpAlpha = Alpha;

	CWing::s_bTrace		= true;
	m_pWing->m_bWingOut                = false;
	if(m_pWing2) m_pWing2->m_bWingOut  = false;
	if(m_pStab) m_pStab->m_bWingOut    = false;
	if(m_pFin)  m_pFin->m_bWingOut     = false;


	if(QInf >0.0) 
	{
		SetAi(qrhs);

		AddString(tr("       Calculating aerodynamic coefficients...")+"\n");
		m_bPointOut = false;
//		CWing::s_Alpha     = Alpha;
//		CWing::s_QInf      = QInf;
		CWing::s_Viscosity = m_pWPolar->m_Viscosity;
		CWing::s_Density   = m_pWPolar->m_Density;
		Lift   = 0.0;
		IDrag  = 0.0;
		VDrag  = 0.0;
		XCP = YCP = 0.0;
		Force.Set(0.0, 0.0, 0.0);

		m_GCm = m_VCm = m_ICm = 0.0;
		m_GRm = 0.0;
		m_GYm = m_VYm = m_IYm = 0.0;

		pos = 0;
		Station = 0;
		for(int i=0; i<4; i++)
		{
			if(m_pWingList[i])
			{
				AddString(tr("         Calculating wing...")+m_pWingList[i]->m_WingName+"\n");

				m_pWingList[i]->PanelTrefftz(QInf, Alpha, Mu, Sigma, pos, Force, WingIDrag, m_pWPolar, m_pWakePanel, m_pWakeNode);
				IDrag += WingIDrag;

				m_pWingList[i]->PanelComputeViscous(QInf, Alpha, WingVDrag, m_pWPolar->m_bViscous, OutString);
				VDrag += WingVDrag;
				AddString(OutString);
				if(m_pWingList[i]->m_bWingOut)  m_bPointOut = true;

				m_pWingList[i]->PanelComputeOnBody(QInf, Alpha, m_Cp+qrhs*m_MatSize+pos, m_Mu+qrhs*m_MatSize+pos,
				                                   XCP, YCP, m_GCm, m_VCm, m_ICm, m_GRm, m_GYm, m_VYm, m_IYm,
												   m_pWPolar);

				m_pWingList[i]->PanelSetBending(bThinSurf);

				pos += m_pWingList[i]->m_MatSize;
				Station += m_pWingList[i]->m_NStation;
			}
		}


		if(*m_ppBody)
		{
			AddString(tr("       Calculating body...")+"\n");
			(*m_ppBody)->ComputeAero(m_Cp+qrhs*m_MatSize+pos, XCP, YCP, m_GCm, m_GRm, m_GYm, Alpha, m_pWPolar->m_CoG.x);

			//the body does not shed any wake --> no induced lift or drag
		}

		if(!m_bTrefftz)
		{
			SumPanelForces(m_Cp+qrhs*m_MatSize, Alpha, Lift, IDrag);
		}

		m_CL          =       Force.dot(WindNormal)    /m_pWPolar->m_WArea;
		m_CX          =       Force.dot(WindDirection) /m_pWPolar->m_WArea;
		m_CY          =       Force.dot(WindSide)      /m_pWPolar->m_WArea;

		m_InducedDrag =  1.0*IDrag/m_pWPolar->m_WArea;
		m_ViscousDrag =  1.0*VDrag/m_pWPolar->m_WArea;

		m_XCP         = XCP/Force.dot(WindNormal);
		m_YCP         = YCP/Force.dot(WindNormal);

		m_GCm *= 1.0 / m_pWPolar->m_WArea /m_pWing->m_MAChord;
		m_VCm *= 1.0 / m_pWPolar->m_WArea /m_pWing->m_MAChord;
		m_ICm *= 1.0 / m_pWPolar->m_WArea /m_pWing->m_MAChord;

		m_GRm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan;

		m_GYm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan;
		m_VYm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan;
		m_IYm *= 1.0 / m_pWPolar->m_WArea /m_pWPolar->m_WSpan;

		if(m_bPointOut) m_bWarning = true;

		if(m_pPlane) pMiarex->AddPOpp(m_bPointOut, m_Cp+qrhs*m_MatSize, Mu, Sigma);
		else         pMiarex->AddWOpp(m_bPointOut, Mu, Sigma, m_Cp+qrhs*m_MatSize);		

		AddString("\n");
	}
	else m_bPointOut = true;

	qApp->processEvents();
}


void PanelAnalysisDlg::GetVortexCp(const int &p, double *Gamma, double *Cp, double const Alpha, CVector &VInf)
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


void PanelAnalysisDlg::GetDoubletDerivative(const int &p, double *Mu, double * Sigma, double *Cp, double const Alpha, double const &QInf, CVector &VInf)
{
	static int PL,PR, PU, PD;
	static double DELQ, DELP;
	static CVector Vl, VInfl;//local panel speed
	static CVector S2, Sl2;
	static double mu0,mu1,mu2;
	static double x0,x1,x2;
	static double Speed2;

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
	VInfl = m_pPanel[p].GlobalToLocal(VInf);

	//in panel referential
	Vl.x = -4.0*PI*(m_pPanel[p].SMP*DELP - Sl2.y*DELQ)/Sl2.x;
	Vl.y = -4.0*PI*DELQ;
	Vl.z =  4.0*PI*Sigma[p];

	VInfl +=Vl;

	Speed2 = VInfl.x*VInfl.x + VInfl.y*VInfl.y + VInfl.z*VInfl.z;

	Cp[p]  = 1.0-Speed2/QInf/QInf;
//	m_Speed[p] = m_pPanel[p].LocalToGlobal(QInfl) * m_3DQInf[q];
}



void PanelAnalysisDlg::ComputeOnBody(int qrhs, double Alpha)
{
	//following VSAERO theory manual
	//the on-body tangential perturbation speed is the derivative of the doublet strength
	int p;
	static double *Mu, * Sigma, *Cp;
	static CVector Qp, VInf;

	//   Define wind axis
	Qp.Set(cos(Alpha*PI/180.0), 0.0, sin(Alpha*PI/180.0));
	VInf = Qp * m_3DQInf[qrhs];

	Mu     = m_Mu    + qrhs * m_MatSize;
	Sigma  = m_Sigma + qrhs * m_MatSize;
	Cp     = m_Cp    + qrhs * m_MatSize;


	for (p=0; p<m_MatSize; p++)
	{
		if(m_bCancel) break;

		if(m_pPanel[p].m_iPos!=0) GetDoubletDerivative(p, Mu, Sigma, Cp, Alpha, m_3DQInf[qrhs], VInf);
		else                      GetVortexCp(p, Mu, Cp, Alpha, Qp);
	}
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

	if(m_pWPolar && (m_pWPolar->m_bTiltedGeom || m_pWPolar->m_bWakeRollUp))
	{
		//back-up the current geometry if the analysis is to be performed on the tilted geometry
		memcpy(m_pMemPanel, m_pPanel, m_MatSize * sizeof(CPanel));
		memcpy(m_pMemNode,  m_pNode,  m_nNodes * sizeof(CVector));
		memcpy(m_pRefWakePanel, m_pWakePanel, m_WakeSize * sizeof(CPanel));
		memcpy(m_pRefWakeNode,  m_pWakeNode,  m_nWakeNodes * sizeof(CVector));
	}

	m_pctrlTextOutput->clear();

	m_bPointOut = false;
	m_bCancel   = false;
	m_bWarning  = false;
	m_bWakeRollUp    = false;

	SetFileHeader();

	QString str = "";
	m_b3DSymetric = m_pWing->m_bSymetric;
	if(!m_pWing->m_bSymetric) str += tr("     Main wing is asymmetric")+"\n";

	if(fabs(m_pWPolar->m_Beta)>0.001)
	{
		str += tr("     Sideslip is asymmetric")+"\n";
		m_b3DSymetric = false;
	}

	if(m_pWing2)
	{
		if(!m_pWing2->m_bSymetric)
		{
			m_b3DSymetric = false;
			str += tr("     2nd wing is asymmetric")+"\n";
		}
	}

	if(m_pStab)
	{
		if(!m_pStab->m_bSymetric)
		{
			m_b3DSymetric = false;
			str += tr("     Elevator is asymmetric")+"\n";
		}
	}

	if(m_pFin)
	{
		m_b3DSymetric = false;
		str += tr("     A fin is considered asymmetric")+"\n";
	}

	if (m_b3DSymetric) AddString(tr("Perfoming symmetric calculation")+"\n\n");
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
	CWing::s_bCancel        = true;
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
	int nrhs, TotalTime;
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
	int Size;
	if(m_b3DSymetric) Size = m_MatSize/2;
	else              Size = m_MatSize;


	//ESTIMATED UNIT TIMES FOR OPERATIONS
	//CreateMatrix :		   10 *Size/400
	//CreateRHS :			   10
	//CreateWakeContribution :  1
	//SolveMultiple :		   30 *Size/400
	//CreateDoubletStrength : 	1 x nrhs
	//RelaxWake :			 20 x nrhs x MaxWakeIter *
	//ComputeAeroCoefs :		5 x nrhs

	TotalTime = 10*Size/400 + 10 + 1 + 30*Size/400 + 1*nrhs + 5*nrhs;


	if(!m_b3DSymetric) TotalTime+=30;//Solve multiple is 4x longer

	m_pctrlProgress->setMinimum(0);
	m_pctrlProgress->setMaximum(TotalTime);

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

	CreateMatrix();
	if (m_bCancel) return true;

	CreateRHS(m_Alpha, m_AlphaDelta, nrhs);
	if (m_bCancel) return true;

	if(!m_pWPolar->m_bThinSurfaces) CreateWakeContribution();
	else memcpy(m_aij, m_aijRef, m_MatSize*m_MatSize*sizeof(double));

	if (m_bCancel) return true;

	if (!SolveMultiple(Alpha, m_AlphaDelta, 1))
	{
		AddString("\n\n"+tr("Singular matrix - aborting....")+"\n");
		m_bWarning = true;
		return true;
	}
	if (m_bCancel) return true;

	CreateDoubletStrength(m_QInf, m_QInfDelta, nrhs);
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
/*
void PanelAnalysisDlg::SourceNASA4023(CVector TestPt, CPanel *pPanel, CVector &V, double &phi)
{
	//VSAERO theory Manual
	//Influence of panel pp at coll pt of panel p
	int i;
	
	phi = 0.0;
	V.Set(0.0,0.0,0.0);

	PJK = TestPt - pPanel->CollPt;
	PN  = PJK.dot(pPanel->Normal);
	pjk = PJK.VAbs();

	if(pjk> RFF*pPanel->Size)
	{
		// use far-field formula
		phi = pPanel->Area /pjk;
		V = PJK * pPanel->Area/pjk/pjk/pjk;
		return;
	}
	
	if(pPanel->m_iPos>=0){
		R[0] = m_pNode[pPanel->m_iLA];
		R[1] = m_pNode[pPanel->m_iTA];
		R[2] = m_pNode[pPanel->m_iTB];
		R[3] = m_pNode[pPanel->m_iLB];
		R[4] = m_pNode[pPanel->m_iLA];	
	}
	else{
		R[0] = m_pNode[pPanel->m_iLB];
		R[1] = m_pNode[pPanel->m_iTB];
		R[2] = m_pNode[pPanel->m_iTA];
		R[3] = m_pNode[pPanel->m_iLA];
		R[4] = m_pNode[pPanel->m_iLB];	
	}
	for (i=0; i<4; i++){
		a  = TestPt - R[i]; 
		b  = TestPt - R[i+1];
		s    = R[i+1] - R[i];
		A    = a.VAbs();
		B    = b.VAbs();
		S    = s.VAbs();
		SM   = s.dot(pPanel->m);
		SL   = s.dot(pPanel->l);
		AM   = a.dot(pPanel->m);
		AL   = a.dot(pPanel->l);
		Al   = AM*SL - AL*SM;
		PA   = PN*PN * SL + Al*AM;
		PB   = PA - Al*SM;

		if(R[i].IsSame(R[i+1])){
			//no contribution from this side
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
				side = pPanel->Normal.dot(a * s); // Positive if on the panel's right side
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
			T1   = pPanel->l      * SM*GL;
			T2   = pPanel->m      * SL*GL;
			T    = pPanel->Normal * CJKi;
			V   += T + T1 - T2;
		}
	}
}
*/
/*
void PanelAnalysisDlg::DoubletNASA4023(CVector TestPt, CPanel *pPanel, CVector &V, double &phi, bool bWake)
{
	//VSAERO theory Manual
	//Influence of panel pp at coll pt of panel p
	int i;
	double CoreSize = 0.000001;
	if(fabs(*m_pCoreSize)>1.e-10) CoreSize = *m_pCoreSize;
	CVector *pNode;
	if(bWake)	pNode = m_pWakeNode;
	else		pNode = m_pNode;

	phi = 0.0;
	V.Set(0.0,0.0,0.0);

	PJK = TestPt - pPanel->CollPt;
	PN  = PJK.dot(pPanel->Normal);
	pjk = PJK.VAbs();

	if(pjk> RFF*pPanel->Size)
	{ // use far-field formula
		phi = PN * pPanel->Area /pjk/pjk/pjk;
		V   = (PJK*3.0*PN - pPanel->Normal*pjk*pjk)*pPanel->Area /pjk/pjk/pjk/pjk/pjk;
		return;
	}
	
	phi = 0.0;
	V.Set(0.0,0.0,0.0);

	if(pPanel->m_iPos>=0)
	{
		R[0] = pNode[pPanel->m_iLA];
		R[1] = pNode[pPanel->m_iTA];
		R[2] = pNode[pPanel->m_iTB];
		R[3] = pNode[pPanel->m_iLB];
		R[4] = pNode[pPanel->m_iLA];	
	}
	else
	{
		R[0] = pNode[pPanel->m_iLB];
		R[1] = pNode[pPanel->m_iTB];
		R[2] = pNode[pPanel->m_iTA];
		R[3] = pNode[pPanel->m_iLA];
		R[4] = pNode[pPanel->m_iLB];	
	}
	for (i=0; i<4; i++)
	{
		a  = TestPt - R[i];
		b  = TestPt - R[i+1];
		s    = R[i+1] - R[i];
		A    = a.VAbs();
		B    = b.VAbs();
		SM   = s.dot(pPanel->m);
		SL   = s.dot(pPanel->l);
		AM   = a.dot(pPanel->m);
		AL   = a.dot(pPanel->l);
		Al   = AM*SL - AL*SM;
		PA   = PN*PN*SL + Al*AM;
		PB   = PA - Al*SM; //try alternative PB   = PN*PN*SL + Al*BM;

		//get the distance of the TestPoint to the panel's side
		h = a * s;
		
		//first the potential
		if(R[i].IsSame(R[i+1])) 
		{
			CJKi = 0.0;
			//no contribution to speed either
		}
		else if (h.VAbs()/s.VAbs() <= CoreSize && a.dot(s)>=0.0 && b.dot(s)<=0.0) 
		{
			CJKi = 0.0;//speed is singular on panel edge, value of potential is unknown
		}
		else
		{
			RNUM = SM*PN * (B*PA-A*PB);
			DNOM = PA*PB + PN*PN*A*B*SM*SM;
			if(fabs(PN)<eps)
			{
				side = pPanel->Normal.dot(a * s); // Positive if on the panel's right side
				if(side >=0.0) sign = 1.0; else sign = -1.0;
				if(DNOM<0.0){
					if(PN>0.0)	CJKi =  PI * sign;
					else		CJKi = -PI * sign;
				}
				else if(DNOM == 0.0){
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
			V += (a * b) * ((A+B) /A/B/ (A*B + a.dot(b)));
		}
		phi += CJKi;

	}
	if(TestPt == pPanel->CollPt)
	{
//		if(R[0].IsSame(R[1]) || R[1].IsSame(R[2]) || R[2].IsSame(R[3]) || R[3].IsSame(R[0]))
//			phi = -3.0*PI/2.0;
//		else 
			phi  = -2.0*PI;
	}
}
*/


void PanelAnalysisDlg::SetAi(int qrhs)
{
	// calculates the induced angles from the doublet and source strengths
	CVector C, TA, TB;
	int m, mw, p;
	double *Mu, *Sigma;
	CWing *pWing[4];
	pWing[0] = m_pWing;
	pWing[1] = m_pWing2;
	pWing[2] = m_pStab;
	pWing[3] = m_pFin;

	Mu    = m_Mu    + qrhs * m_MatSize;
	Sigma = m_Sigma + qrhs * m_MatSize;

	AddString(tr("       Calculating induced angles...")+"\n");

	for(int i=0; i<4; i++)
	{
		if(pWing[i])
		{
			m=0;
			for (p=0; p<pWing[i]->m_MatSize; p++)
			{
				if(pWing[i]->m_pPanel[p].m_bIsTrailing)
				{
					if(m_pWPolar->m_bThinSurfaces)
					{
						//TODO : check if the mid-point at the trailing wake panel leads to the same results
						//       if so, don't make a distinction for thin surfaces
						C = (m_pNode[pWing[i]->m_pPanel[p].m_iTA] + m_pNode[pWing[i]->m_pPanel[p].m_iTB])/2.0;
						GetSpeedVector(C, Mu, Sigma, pWing[i]->m_Vd[m], false);
						pWing[i]->m_Ai[m] = atan2(pWing[i]->m_Vd[m].z, m_3DQInf[qrhs]) * 180.0/PI;
						m++;
					}
					else if(pWing[i]->m_pPanel[p].m_iPos<0)
					{
						mw = pWing[i]->m_pPanel[p].m_iWake;
						TA = m_pWakeNode[m_pWakePanel[mw + m_pWPolar->m_NXWakePanels-1].m_iTA];
						TB = m_pWakeNode[m_pWakePanel[mw + m_pWPolar->m_NXWakePanels-1].m_iTB];
						C = (TA+TB)/2.0;
						GetSpeedVector(C, Mu, Sigma, pWing[i]->m_Vd[m], false);
						pWing[i]->m_Ai[m] = atan2(pWing[i]->m_Vd[m].z, m_3DQInf[qrhs]) * 180.0/PI;
						m++;
					}

				}
			}
		}
	}
}



/*
void PanelAnalysisDlg::SetAi(int qrhs)
{
	// calculates the induced angles from the doublet and source strengths
	CVector C, V, TA, TB;
	int kw, m, mw, pos;
	double *Mu, *Sigma;
	double Ai[MAXSTATIONS];

	Mu    = m_Mu    + qrhs * m_MatSize;
	Sigma = m_Sigma + qrhs * m_MatSize;
	m=0;

	AddString("       Calculating induced angles...\n");
	mw = 0;

	for (kw=0; kw<m_NWakeColumn; kw++)
	{
		TA = m_pWakeNode[m_pWakePanel[mw + m_pWPolar->m_NXWakePanels-1].m_iTA];
		TB = m_pWakeNode[m_pWakePanel[mw + m_pWPolar->m_NXWakePanels-1].m_iTB];
		C = (TA+TB)/2.0;

		GetSpeedVector(C, Mu, Sigma, V);
		Ai[m] = atan2(V.z, m_3DQInf[qrhs]) * 180.0/PI;
		m_pWing->m_Vd[m] = V;

		mw += m_pWPolar->m_NXWakePanels;
		m++;
	}

	pos = 0;
	for(int i=0; i<4; i++)
	{
		if(m_pWingList[i])
		{
			for (m=0; m<m_pWingList[i]->m_NStation; m++) m_pWingList[i]->m_Ai[m] = Ai[m+pos];
			pos = m_pWingList[i]->m_NStation;
		}
	}
}

*/

/*
void PanelAnalysisDlg::RelaxWake()
{
	CMiarex *pMiarex = (CMiarex*)s_pMiarex;
	CVector V, VL, VT;
	int mw, kw, lw, llw;
	double t, dx;
	double damping = 1.0;
	double *Mu    = m_Mu   ;
	double *Sigma = m_Sigma;

	//Since the wake roll-up is performed on the tilted geometry,
	// we define a speed vector parallel to the x-axis
	CVector QInf(m_QInf, 0.0, 0.0);

	// VSAERO method
	// calculates the induced velocity at the wake panel points 
	// and realigns the panel's sides with the local flow vector
	// we move all the wake nodes except for the leading nodes which remain at the wing's T.E.
	CVector LATB, TALB, Trans, PP;
	CVector WLA, WLB,WTA,WTB, WTemp;//wake panel's leading corner points

	AddString("      Relaxing the wake...\n");

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

			VL = GetSpeedVector(WLA, Mu, Sigma);
			VT = GetSpeedVector(WTA, Mu, Sigma);
			VL += QInf;
			VT += QInf;
			V = (VL + VT)/2.0;
	
			dx  = (WTA.x-WLA.x);
			t = dx/V.x;
			WTemp.x += dx;
			WTemp.y += V.y * t;
			WTemp.z += V.z * t;

			//define the translation vector
			Trans = (WTemp - WLA) / damping;

			//and move all the nodes downstream, i.e. all panels left nodes
			for (llw=0; llw <m_pWPolar->m_NXWakePanels-lw; llw++)
			{
				m_pTempWakeNode[m_pWakePanel[mw+llw].m_iTA].y += Trans.y;
				m_pTempWakeNode[m_pWakePanel[mw+llw].m_iTA].z += Trans.z;
			}
		}
		//finally do the same for the right side of the last right column

		WLB.Copy(m_pTempWakeNode[m_pWakePanel[mw].m_iLB]);
		WTB.Copy(m_pTempWakeNode[m_pWakePanel[mw].m_iTB]);
		WTemp.Copy(WLB);

		VL = GetSpeedVector(WLB, Mu, Sigma);
		VT = GetSpeedVector(WTB, Mu, Sigma);
		VL += QInf;
		VT += QInf;
		V = (VL + VT)/2.0;

		dx  = (WTB.x-WLB.x);
		t = dx/V.x;
		WTemp.x += dx;
		WTemp.y += V.y * t;
		WTemp.z += V.z * t;

		//define the translation vector
		Trans = (WTemp - WLB) / damping;

		//and move all the nodes downstream, i.e. all panels left nodes
		for (llw=0; llw <m_pWPolar->m_NXWakePanels-lw; llw++)
		{
			m_pTempWakeNode[m_pWakePanel[mw+llw].m_iTB].y += Trans.y;
			m_pTempWakeNode[m_pWakePanel[mw+llw].m_iTB].z += Trans.z;
		}
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
	pMiarex->m_bResetglWake = true;
	pMiarex->UpdateView();
}*/


void PanelAnalysisDlg::SetFileHeader()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QTextStream out(m_pXFile);

	out << "\n";
	out << pMainFrame->m_VersionName;
	out << "\n";
	out << m_pWing->m_WingName;
	out << "\n";


	QDateTime dt = QDateTime::currentDateTime();
	QString str = dt.toString("dd.MM.yyyy  hh:mm:ss");

	out << str;
	out << "\n___________________________________\n\n";
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
	QFont CourierFont = QFont("Courier",10);
	CourierFont.setStyleHint(QFont::Courier);
	m_pctrlTextOutput->setFont(CourierFont);	
	m_pctrlTextOutput->setReadOnly(true);
	m_pctrlTextOutput->setLineWrapMode(QTextEdit::NoWrap);
	m_pctrlTextOutput->setWordWrapMode(QTextOption::NoWrap);

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




bool PanelAnalysisDlg::SolveMultiple(double V0, double VDelta, int nval)
{
	//______________________________________________________________________________________
	// Method :
	// 	- If the polar is of type 1 or 2, solve the linear system
	//	- for cosine and sine parts, for a unit speed
	//	- If the polar is of type 4, solve only for unit speed and for the specified Alpha
	//	- Reconstruct right side results if calculation was symetric
	//	- Sort results i.a.w. panel numbering
	//______________________________________________________________________________________

	static int index[VLMMAXMATSIZE];
	int Size, nrhs, q, o, p, nel;
	QString strong;

	if(m_b3DSymetric) Size = m_MatSize/2;
	else              Size = m_MatSize;


	if(m_pWPolar->m_Type!=4) nrhs = nval;
	else                     nrhs = 0;

	memcpy(m_RHS,      m_cosRHS, Size * sizeof(double));
	memcpy(m_RHS+Size, m_sinRHS, Size * sizeof(double));


/*	if(!Gauss(m_aij, Size, m_RHS, 2, &m_bCancel))
	{
		AddString(tr("      Singular Matrix.... Aborting calculation...")+"\n");
		m_bConverged = false;
		return false;
	}
	else m_bConverged = true;*/

	strong = "      Performing LU Matrix decomposition...\n";
	AddString(strong);

	if(!Crout_LU_Decomposition_with_Pivoting(m_aij, index, Size, &m_bCancel, 30.0*Size/400.0, m_Progress))
	{
		AddString(tr("      Singular Matrix.... Aborting calculation...\n"));
		m_bConverged = false;
		return false;
	}
	else m_bConverged = true;

	AddString("      Solving LU system...\n");
	Crout_LU_with_Pivoting_Solve(m_aij, m_cosRHS, index, m_RHS,      Size, &m_bCancel);
	Crout_LU_with_Pivoting_Solve(m_aij, m_sinRHS, index, m_RHS+Size, Size, &m_bCancel);

	memcpy(m_cosRHS, m_RHS,      Size * sizeof(double));
	memcpy(m_sinRHS, m_RHS+Size, Size * sizeof(double));

	AddString("      Constructing results...\n");

	//______________________________________________________________________________________
	//Reconstruct right side results if calculation was symetric
	double *SigmaRef = m_aij;//use existing reserved memory, do not re-allocate


	if(m_b3DSymetric)
	{
		memcpy(m_RHSRef, m_RHS,   2*Size*sizeof(double));
		memcpy(SigmaRef, m_Sigma, 2*Size*sizeof(double));

		//cosine
		for (p=0; p<m_MatSize/2; p++)
		{
			m_cosRHS[p]             = m_RHSRef[p];
			m_cosRHS[m_MatSize-1-p] = m_RHSRef[p];
		}
		//sine
		for (p=0; p<m_MatSize/2; p++)
		{
			m_sinRHS[p]             = m_RHSRef[Size+p];
			m_sinRHS[m_MatSize-1-p] = m_RHSRef[Size+p];
		}
	}

//	reconstruct all results from cosine and sine unit vectors
	int m;
	double alpha, cosa, sina;
	m=0;
	for (q=0; q<nval;q++)
	{
		alpha = V0 + q * VDelta;
		cosa = cos(alpha*PI/180.0);
		sina = sin(alpha*PI/180.0);
		for(p=0; p<m_MatSize; p++)
		{
			m_RHS[m] = cosa * m_cosRHS[p] + sina * m_sinRHS[p];
			m++;
		}
	}

	//______________________________________________________________________________________
	//at this stage, m_RHS and m_Sigma are ordered as m_ppPanel[]... need to sort as m_pPanel

	memcpy(m_RHSRef, m_RHS,   nval*m_MatSize*sizeof(double));
	memcpy(SigmaRef, m_Sigma, nval*m_MatSize*sizeof(double));

	for (q=0; q<nval;q++)
	{
		o  =  q * m_MatSize;

		for (p=0; p<m_MatSize; p++)
		{
			nel = m_ppPanel[p]->m_iElement;
			m_Mu[o+nel]       = m_RHSRef[o+p];
			m_Sigma[o+nel]    = SigmaRef[o+p];
		}
	}

	return true;
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
	// junctions between wing and body, or between fin and elevator cannot be adequately 
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
	strong = tr("Launching 3D Panel Analysis....")+"\n\n";
	AddString(strong);

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(m_pPlane)
	{
		QString len, str;
		GetLengthUnit(len, pMainFrame->m_LengthUnit);
		if(fabs(m_pPlane->m_LEWing.z-m_pPlane->m_LEStab.z)<.0001)
		{	
			str = QString("%1 ").arg(m_pPlane->m_LEWing.z*pMainFrame->m_mtoUnit, 7, 'g', 3);
			strong = tr("Warning: The wing and elevator lie in the same plane z=")+str+len+"\n";
			AddString(strong);
			strong = tr("It is recommended to slightly offset the wing or the elevator to avoid numerical instabilities")+"\n\n";
			AddString(strong);
			
		}
	}

	strong = QString(tr("Type %1 Analysis")+"\n\n").arg(m_pWPolar->m_Type);
	AddString(strong);
	m_bCancel = false;

	m_pWingList[0] = m_pWing;
	m_pWingList[1] = m_pWing2;
	m_pWingList[2] = m_pStab;
	m_pWingList[3] = m_pFin;

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
	else 	if(m_pWPolar->m_Type==4) ReLoop();

	if (!m_bCancel && !m_bWarning) strong = "\n"+tr("Panel Analysis completed successfully")+"\n";
	else if (m_bWarning)           strong = "\n"+tr("Panel Analysis completed ... Errors encountered")+"\n";
	AddString(strong);
	pTimer->stop();

	if(m_pWPolar && (m_pWPolar->m_bTiltedGeom || m_pWPolar->m_bWakeRollUp))
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
	int n, nrhs, nWakeIter, MaxWakeIter, TotalTime;

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

	int Size;
	if(m_b3DSymetric) Size = m_MatSize/2;
	else              Size = m_MatSize;


	//ESTIMATED UNIT TIMES FOR OPERATIONS
	//CreateMatrix :		   10 *Size/400 * nrhs
	//CreateRHS :			   10 *nrhs
	//CreateWakeContribution :  1*nrhs
	//SolveMultiple :		   30 *Size/400
	//CreateDoubletStrength : 	1 x nrhs
	//RelaxWake :			 20 x nrhs x MaxWakeIter *
	//ComputeAeroCoefs :		5 x nrhs

	TotalTime = 10*Size/400*nrhs + 10*nrhs + 1*nrhs + 40*Size/400*nrhs + 1*nrhs + 5*nrhs;

	if(m_pWPolar->m_bWakeRollUp) TotalTime += 20 * nrhs * MaxWakeIter;
//	if(!m_b3DSymetric) TotalTime+=30*nrhs;//Solve multiple is 4x longer

	m_pctrlProgress->setMinimum(0);
	m_pctrlProgress->setMaximum(TotalTime);
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

		CreateMatrix();
		if (m_bCancel) return true;

		// The calculation will be performed at AOA=0.0 because the geometry is tilted...
		// ... but the operating angle is different :

		CreateRHS(0.0, m_AlphaDelta, 1);
		if (m_bCancel) return true;

		for (nWakeIter = 0; nWakeIter<MaxWakeIter; nWakeIter++)
		{
			if(m_pWPolar->m_bWakeRollUp)
			{
				str = QString(tr("      Wake iteration %1")+"\n").arg(nWakeIter+1,3);
				AddString(str);
			}

			if (m_bCancel) return true;

			if(!m_pWPolar->m_bThinSurfaces) CreateWakeContribution();
			else                            memcpy(m_aij, m_aijRef, m_MatSize*m_MatSize*sizeof(double));

			if (m_bCancel) return true;

			if (!SolveMultiple(0.0, m_AlphaDelta, 1))
			{
				AddString("\n\n"+tr("Singular matrix - aborting....")+"\n");
				m_bWarning = true;
				return true;
			}
			if (m_bCancel) return true;

			CreateDoubletStrength(0.0, m_AlphaDelta, 1);
			if (m_bCancel) return true;

			if(MaxWakeIter>0 && m_pWPolar->m_bWakeRollUp) RelaxWake();
		}

		AddString("\n");

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




