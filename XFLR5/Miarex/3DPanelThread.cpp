/****************************************************************************

    C3DPanelThread Class
	Copyright (C) 2005 André Deperrois xflr5@yahoo.com

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
// PanelThread.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "3DPanelThread.h"
#include "3DPanelDlg.h"
#include <math.h>
#include "Miarex.h"

// C3DPanelThread

IMPLEMENT_DYNCREATE(C3DPanelThread, CWinThread)

C3DPanelThread::C3DPanelThread()
{
	m_bCancel   = false;
	m_bSequence = false;
	m_bFinished = false;
	m_Alpha      = 0.0;
	m_AlphaMax   = 0.0;
	m_DeltaAlpha = 0.0;
	m_QInf       = 0.0;
	m_QInfMax    = 0.0;
	m_DeltaQInf  = 0.0;
	m_pWPolar = NULL;

//ESTIMATED UNIT TIMES FOR OPERATION
//CreateMatrix :			15
//CreateRHS :				5 x nrhs
//CreateWakeContribution :	1 x nrhs
//SolveMultiple :			10
//CreateDoubletStrength :	1 x nrhs
//ComputeAeroCoefs :		5 x nrhs
//RelaxWake :				20 x nrhs

}

C3DPanelThread::~C3DPanelThread()
{
}

BOOL C3DPanelThread::InitInstance()
{
	CString strong;
	C3DPanelDlg * p3DDlg = (C3DPanelDlg*)m_pParent;
	p3DDlg->AddString("Launching the 3D-Panel Analysis....\r\n");

	strong.Format("Type %d Analysis\r\n", m_pWPolar->m_Type);
	p3DDlg->AddString(strong);

	if(p3DDlg->m_bDirichlet)	strong.Format("Dirichlet boundary conditions\r\n");
	else						strong.Format("Neumann boundary conditions\r\n");
	p3DDlg->AddString(strong);


	p3DDlg->m_bCancel = false;

	if(m_pWPolar->m_Type != 4 && (m_pWPolar->m_bWakeRollUp || m_pWPolar->m_bTiltedGeom))	UnitLoop();
	else if(m_pWPolar->m_Type != 4)	AlphaLoop();
	else							ReLoop();

	if (!m_bCancel && !p3DDlg->m_bWarning) p3DDlg->AddString("\r\nPanel Analysis completed successfully\r\n");
	else if (p3DDlg->m_bWarning)           p3DDlg->AddString("\r\nPanel Analysis completed ... Errors encountered\r\n");

	Sleep(100);
	return FALSE;
}

int C3DPanelThread::ExitInstance()
{
	m_bFinished = true;
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(C3DPanelThread, CWinThread)
END_MESSAGE_MAP()


// Gestionnaires de messages C3DPanelThread



bool C3DPanelThread::UnitLoop()
{
	CString str;
	CVector O(0.0,0.0,0.0);
	C3DPanelDlg * p3DDlg = (C3DPanelDlg*)m_pParent;
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	int n, nrhs, nWakeIter, MaxWakeIter, TotalTime;
	double Alpha;
	if(m_AlphaMax<m_Alpha) m_DeltaAlpha = -abs(m_DeltaAlpha);
	nrhs  = (int)abs((m_AlphaMax-m_Alpha)*1.0001/m_DeltaAlpha) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100){
		int res = AfxMessageBox("The number of points to be calculated will be limited to 100", MB_OKCANCEL);
		if(res ==IDCANCEL) return false;
		nrhs = 100;
	}

	if(!m_pWPolar->m_bWakeRollUp)	MaxWakeIter = 1;
	else							MaxWakeIter = max(p3DDlg->m_MaxWakeIter, 1);

//ESTIMATED UNIT TIMES FOR OPERATIONS
//CreateMatrix :			15 x nrhs
//CreateRHS :				10 x nrhs
//CreateWakeContribution :	 1 x nrhs x MaxWakeIter
//SolveMultiple :			40 x nrhs x MaxWakeIter
//CreateDoubletStrength : 	 1 x nrhs x MaxWakeIter
//RelaxWake :				20 x nrhs x MaxWakeIter
//ComputeAeroCoefs :		 5 x nrhs
	
	TotalTime = 15 + (10+5) * nrhs + (1+40+1) * nrhs * MaxWakeIter;
	if(m_pWPolar->m_bWakeRollUp) TotalTime += 20 * nrhs * MaxWakeIter;
//	if(!p3DDlg->m_b3DSymetric) TotalTime+=30*nrhs;//Solve multiple is 4x longer

	p3DDlg->m_ctrlProgress.SetRange(0,TotalTime);
	p3DDlg->m_Progress = 0;

	CWaitCursor Wait;
	str.Format("   Solving the problem... \r\n");
	p3DDlg->AddString(str);

	if (!p3DDlg->CreateMatrix())
	{
		p3DDlg->AddString("\r\nFailed to create the matrix....\r\n");
		p3DDlg->m_bWarning = true;
		return true;
	}

	for (n=0; n<nrhs; n++)
	{
//TRACE("%d\n", p3DDlg->m_Progress);
		Alpha = m_Alpha + n * m_DeltaAlpha;
		str.Format("      \r\n    Processing Alpha= %7.2f\r\n",Alpha);
		p3DDlg->AddString(str);

		//reset the initial geometry before a new angle is processed
		memcpy(p3DDlg->m_pPanel, p3DDlg->m_pMemPanel, p3DDlg->m_MatSize * sizeof(CPanel));
		memcpy(p3DDlg->m_pNode,  p3DDlg->m_pMemNode,  p3DDlg->m_nNodes  * sizeof(CVector));
		memcpy(p3DDlg->m_pWakePanel, p3DDlg->m_pRefWakePanel, p3DDlg->m_WakeSize * sizeof(CPanel));
		memcpy(p3DDlg->m_pWakeNode,  p3DDlg->m_pRefWakeNode,  p3DDlg->m_nWakeNodes * sizeof(CVector));

		// Rotate the wing panels and translate the wake to the new T.E. position
		pMiarex->RotateGeomY(m_Alpha+n*m_DeltaAlpha, O);

		// The calculation will be performed at AOA=0.0 because the geometry is tilted...
		Alpha = 0.0;
		// ... but the operatinging angle is different :
		p3DDlg->m_OpAlpha = m_Alpha+n*m_DeltaAlpha;

		// The rest is silence (Hamlet)
		if (!p3DDlg->CreateRHS(Alpha, m_DeltaAlpha, 1)) 
		{
			p3DDlg->AddString("\r\nFailed to create RHS Vector....\r\n");
			p3DDlg->m_bWarning = true;
			return true;
		}

		for (nWakeIter = 0; nWakeIter<MaxWakeIter; nWakeIter++)
		{
			if(m_pWPolar->m_bWakeRollUp) 
			{
				str.Format("      Wake iteration %3d\r\n",nWakeIter+1);
				p3DDlg->AddString(str);
			}

			if (m_bCancel) return true;

			if (!p3DDlg->CreateWakeContribution()) 
			{
				p3DDlg->AddString("\r\nFailed to add the wake contribution....\r\n");
				p3DDlg->m_bWarning = true;
				return true;
			}

			if (m_bCancel) return true;

			if (!p3DDlg->SolveMultiple(Alpha, m_DeltaAlpha, 1))	
			{
				p3DDlg->AddString("\r\n\r\nSingular matrix - aborting....\r\n");
				p3DDlg->m_bWarning = true;
				return true;
			}

			if (m_bCancel) return true;

			if(!p3DDlg->CreateDoubletStrength(Alpha, m_DeltaAlpha, 1))
			{
					p3DDlg->AddString("\r\n\r\nFailed to create doublet strengths....\r\n");
					p3DDlg->m_bWarning = true;
					return true;
			}
			if (m_bCancel) return true;

			if(MaxWakeIter>0 && m_pWPolar->m_bWakeRollUp) p3DDlg->RelaxWake();	

		}

		p3DDlg->AddString("\r\n");


		if(!p3DDlg->ComputeAeroCoefs(Alpha, m_DeltaAlpha, 1)) 
		{
				p3DDlg->AddString("\r\n\r\nFailed to compute aerodynamic coefficients....\r\n");
				p3DDlg->m_bWarning = true;
				return true;
		}
	}

	return true;
}



bool C3DPanelThread::AlphaLoop(void)
{
	CString str;
	C3DPanelDlg * p3DDlg = (C3DPanelDlg*)m_pParent;
	int nrhs, TotalTime;

	if(m_AlphaMax<m_Alpha) m_DeltaAlpha = -abs(m_DeltaAlpha);
	nrhs  = (int)abs((m_AlphaMax-m_Alpha)*1.0001/m_DeltaAlpha) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100)
	{
		int res = AfxMessageBox("The number of points to be calculated will be limited to 100", MB_OKCANCEL);
		if(res ==IDCANCEL) return false;
		nrhs = 100;
	}

	int MaxWakeIter = 1;
	
//ESTIMATED UNIT TIMES FOR OPERATIONS
//CreateMatrix :			15
//CreateRHS :				10 x 2
//CreateWakeContribution :	 1 x 2 x MaxWakeIter
//SolveMultiple :			30        x MaxWakeIter
//CreateDoubletStrength : 	 1 x nrhs x MaxWakeIter
//RelaxWake :				20 x nrhs x MaxWakeIter
//ComputeAeroCoefs :		 5 x nrhs
	
	TotalTime = 15 + 2*10 + 5 * nrhs + 30 * MaxWakeIter + 2*MaxWakeIter + 1 * nrhs * MaxWakeIter;

	if(m_pWPolar->m_bWakeRollUp) TotalTime += 20*nrhs*MaxWakeIter;
//	if(!p3DDlg->m_b3DSymetric) TotalTime+=30;//Solve multiple is 4x longer

	p3DDlg->m_ctrlProgress.SetRange(0,TotalTime);
	p3DDlg->m_Progress = 0;


	CWaitCursor Wait;
	str.Format("   Solving the problem... \r\n");
	p3DDlg->AddString(str);

	if (!p3DDlg->CreateMatrix()) 
	{
		p3DDlg->AddString("\r\nFailed to create the matrix....\r\n");
		p3DDlg->m_bWarning = true;
		return true;
	}

	if (m_bCancel) return true;

	if (!p3DDlg->CreateRHS(m_Alpha, m_DeltaAlpha, nrhs)) 
	{
		p3DDlg->AddString("\r\nFailed to create RHS Vector....\r\n");
		p3DDlg->m_bWarning = true;
		return true;
	}

	if (m_bCancel) return true;

	if (!p3DDlg->CreateWakeContribution()) 
	{
		p3DDlg->AddString("\r\nFailed to add the wake contribution....\r\n");
		p3DDlg->m_bWarning = true;
		return true;
	}
	if (m_bCancel) return true;

	if (!p3DDlg->SolveMultiple(m_Alpha, m_DeltaAlpha, nrhs))	
	{
		p3DDlg->AddString("\r\n\r\nSingular matrix - aborting....\r\n");
		p3DDlg->m_bWarning = true;
		return true;
	}
	if (m_bCancel) return true;

	if(!p3DDlg->CreateDoubletStrength(m_Alpha, m_DeltaAlpha, nrhs)) 
	{
		p3DDlg->AddString("\r\n\r\nFailed to create doublet strengths....\r\n");
		p3DDlg->m_bWarning = true;
		return true;
	}

	if (m_bCancel) return true;

	if(!p3DDlg->ComputeAeroCoefs(m_Alpha, m_DeltaAlpha, nrhs))
	{
		p3DDlg->AddString("\r\n\r\nFailed to compute aerodynamics....\r\n");
		p3DDlg->m_bWarning = true;
		return true;
	}

	return true;
}

bool C3DPanelThread::ReLoop(void)
{
	CWaitCursor Wait;
	CString str;
	int nrhs, TotalTime;
	double QInf=0.0;
	double Alpha = 0.0;

	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	C3DPanelDlg * p3DDlg = (C3DPanelDlg*)m_pParent;

	if(m_QInfMax<m_QInf) m_DeltaQInf = -abs(m_DeltaQInf);
	nrhs  = (int)abs((m_QInfMax-m_QInf)*1.0001/m_DeltaQInf) +1 ;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100){
		int res = AfxMessageBox("The number of points to be calculated will be limited to 100", MB_OKCANCEL);
		if(res ==IDCANCEL) return false;
		nrhs = 100;
	}

	int MaxWakeIter = 1;
//ESTIMATED UNIT TIMES FOR OPERATIONS
//CreateMatrix :			15
//CreateRHS :				10 x nrhs
//CreateWakeContribution :	 1 x nrhs x MaxWakeIter
//SolveMultiple :			30        x MaxWakeIter
//CreateDoubletStrength : 	 1 x nrhs x MaxWakeIter
//RelaxWake :				20 x nrhs x MaxWakeIter
//ComputeAeroCoefs :		 3 x nrhs
	
	TotalTime = 15+ (10+3) * nrhs + 30 * MaxWakeIter + (1+1) * nrhs * MaxWakeIter;

	if(!p3DDlg->m_b3DSymetric) TotalTime+=30;//Solve multiple is 4x longer

	p3DDlg->m_ctrlProgress.SetRange(0,TotalTime);
	p3DDlg->m_Progress = 0;

	if(m_pWPolar->m_bTiltedGeom)
	{
		//reset the initial geometry before a new angle is processed
		memcpy(p3DDlg->m_pPanel,     p3DDlg->m_pMemPanel,     p3DDlg->m_MatSize * sizeof(CPanel));
		memcpy(p3DDlg->m_pNode,      p3DDlg->m_pMemNode,      p3DDlg->m_nNodes  * sizeof(CVector));
		memcpy(p3DDlg->m_pWakePanel, p3DDlg->m_pRefWakePanel, p3DDlg->m_WakeSize * sizeof(CPanel));
		memcpy(p3DDlg->m_pWakeNode,  p3DDlg->m_pRefWakeNode,  p3DDlg->m_nWakeNodes * sizeof(CVector));

		// Rotate the wing panels and translate the wake to the new T.E. position
		CVector O;
		pMiarex->RotateGeomY(m_pWPolar->m_ASpec, O);
		p3DDlg->m_OpAlpha = m_pWPolar->m_ASpec;
		Alpha = 0.0;
	}
	else Alpha = m_Alpha;

	str.Format("   Solving the problem... \r\n");
	p3DDlg->AddString(str);

	if(!p3DDlg->CreateMatrix()) {
		p3DDlg->AddString("\r\n\r\nFailed to create matrix....\r\n");
		p3DDlg->m_bWarning = true;
		return true;
	}

//first solve for unit speed... calculation is linear

	if (!p3DDlg->CreateRHS(Alpha, m_DeltaAlpha, 1)) {
		p3DDlg->AddString("\r\nFailed to create RHS Vector....\r\n");
		p3DDlg->m_bWarning = true;
		return true;
	}

	if (m_bCancel) return true;

	if (!p3DDlg->CreateWakeContribution()) {
		p3DDlg->AddString("\r\nFailed to add the wake contribution....\r\n");
		p3DDlg->m_bWarning = true;
		return true;
	}

	if (m_bCancel) return true;

	if (!p3DDlg->SolveMultiple(Alpha, m_DeltaAlpha, 1))	{
		p3DDlg->AddString("\r\n\r\nSingular matrix - aborting....\r\n");
		p3DDlg->m_bWarning = true;
		return true;
	}

	if (m_bCancel) return true;

	if(!p3DDlg->CreateDoubletStrength(m_QInf, m_DeltaQInf, nrhs)) {
		p3DDlg->AddString("\r\n\r\nFailed to create doublet strengths....\r\n");
		p3DDlg->m_bWarning = true;
		return true;
	}

	if (m_bCancel) return true;

	if(!p3DDlg->ComputeAeroCoefs(m_QInf, m_DeltaQInf, nrhs)){
		p3DDlg->AddString("\r\n\r\nFailed to compute aerodynamics....\r\n");
		p3DDlg->m_bWarning = true;
		return true;
	}

	if (m_bCancel) return true;

	return true;
}
