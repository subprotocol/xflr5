/****************************************************************************

    CVLMThread Class
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

// VLMThread.cpp : implementation file
// This class will run the VLM thread and call the appropriate
// methods implemented in the CWing class
//
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "VLMThread.h"
#include "VLMDlg.h"
#include "Miarex.h"
#include <math.h>



/////////////////////////////////////////////////////////////////////////////
// CVLMThread

IMPLEMENT_DYNCREATE(CVLMThread, CWinThread)

CVLMThread::CVLMThread()
{
	m_bCancel    = false;
	m_bSequence  = false;
	m_bType4     = false;
	m_bFinished  = false;
	m_Alpha      = 0.0;
	m_AlphaMax   = 0.0;
	m_DeltaAlpha = 0.0;
	m_QInf       = 0.0;
	m_QInfMax    = 0.0;
	m_DeltaQInf  = 0.0;
	m_MaxWakeIter = 5;
}

CVLMThread::~CVLMThread()
{
}

BOOL CVLMThread::InitInstance()
{
	CString strong;
	CVLMDlg * pVLMDlg = (CVLMDlg*)m_pParent;
	pVLMDlg->AddString("Launching VLM Analysis....\r\n\r\n");

	strong.Format("Type %d Analysis\r\n\r\n", pVLMDlg->m_pWPolar->m_Type);
	pVLMDlg->AddString(strong);
	pVLMDlg->m_bCancel = false;

	if(!m_bType4) 
	{
		if(pVLMDlg->m_pWPolar->m_bTiltedGeom) UnitLoop();
		else                                  AlphaLoop();
	}
	else 		  ReLoop();

	if (!m_bCancel && !pVLMDlg->m_bWarning) pVLMDlg->AddString("\r\nVLM Analysis completed successfully\r\n");
	else if (pVLMDlg->m_bWarning)           pVLMDlg->AddString("\r\nVLM Analysis completed ... Errors encountered\r\n");

	Sleep(100);
	return FALSE;
}

int CVLMThread::ExitInstance()
{
	m_bFinished = true;
//	m_pParent->PostMessage(WM_CLOSE);
//	m_pParent->PostMessage(V_ENDTHREAD);
//	CVLMDlg * pVLMDlg = (CVLMDlg*)m_pParent;
//	pVLMDlg->EndDialog(0);
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CVLMThread, CWinThread)
	//{{AFX_MSG_MAP(CVLMThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVLMThread message handlers

bool CVLMThread::AlphaLoop()
{
	CString str;
	CVLMDlg * pVLMDlg = (CVLMDlg*)m_pParent;
	int nrhs;

	if(m_AlphaMax<m_Alpha) m_DeltaAlpha = -abs(m_DeltaAlpha);
	nrhs  = (int)abs((m_AlphaMax-m_Alpha)*1.0001/m_DeltaAlpha) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100){
		int res = AfxMessageBox("The number of points to be calculated will be limited to 100", MB_OKCANCEL);
		if(res ==IDCANCEL) return false;
		nrhs = 100;
	}
	pVLMDlg->m_bTrace = true;
	CWaitCursor Wait;
	str.Format("   Solving the problem... \r\n\r\n");
	pVLMDlg->AddString(str);

	if(!pVLMDlg->VLMCreateRHS(m_Alpha, m_DeltaAlpha, nrhs)) 
	{
		pVLMDlg->AddString("\r\n\r\nFailed to create RHS....\r\n");
		pVLMDlg->m_bWarning = true;
		return true;
	}
	if (m_bCancel) return true;

	if (!pVLMDlg->VLMCreateMatrix()) 
	{
		pVLMDlg->AddString("\r\nFailed to create the matrix....\r\n");
		pVLMDlg->m_bWarning = true;
		return true;
	}

	if (m_bCancel) return true;

	if (!pVLMDlg->VLMSolveMultiple(m_Alpha, m_DeltaAlpha, nrhs))	{
		if (m_bCancel) {
			pVLMDlg->AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");
			return true;
		}
	}

	if (m_bCancel) return true;
		
	pVLMDlg->VLMComputePlane(m_Alpha, m_DeltaAlpha, nrhs);
	
	if (m_bCancel) return true;

	return true;
}



bool CVLMThread::UnitLoop()
{
	int i, nrhs;
	CString str;

	CVLMDlg * pVLMDlg = (CVLMDlg*)m_pParent;
	CMiarex * pMiarex = (CMiarex*)m_pMiarex;

	CVector O(0.0,0.0,0.0);

	if(m_AlphaMax<m_Alpha) m_DeltaAlpha = -abs(m_DeltaAlpha);

	nrhs  = (int)abs((m_AlphaMax-m_Alpha)*1.0001/m_DeltaAlpha) + 1;


	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100){
		AfxMessageBox("The number of points to be calculated will be limited to 100", MB_OK);
		nrhs = 100;
	}

	pVLMDlg->m_bTrace = true;
	pVLMDlg->m_Alpha = 0.0;

	CWaitCursor Wait;
	str.Format("   Solving the problem... \r\n");
	pVLMDlg->AddString(str);

	for (i=0; i<nrhs; i++)
	{
		memcpy(pVLMDlg->m_pPanel, pVLMDlg->m_pMemPanel, pVLMDlg->m_MatSize * sizeof(CPanel));
		memcpy(pVLMDlg->m_pNode,  pVLMDlg->m_pMemNode,  pVLMDlg->m_nNodes  * sizeof(CVector));

		pMiarex->RotateGeomY(m_Alpha+i*m_DeltaAlpha, O);

		str.Format("      ...Alpha = %8.2f\r\n",m_Alpha+i*m_DeltaAlpha);
		pVLMDlg->AddString(str);

		if (m_bCancel) break;

		pVLMDlg->m_OpAlpha = m_Alpha+i*m_DeltaAlpha;

		if(!pVLMDlg->VLMCreateRHS(0.0, m_DeltaAlpha, 1)) 
		{
			pVLMDlg->AddString("\r\n\r\nFailed to create RHS....\r\n");
			pVLMDlg->m_bWarning = true;
			return true;
		}

		if (m_bCancel) break;

		if (!pVLMDlg->VLMCreateMatrix()) {
			pVLMDlg->AddString("\r\nFailed to create the matrix....\r\n");
			pVLMDlg->m_bWarning = true;
			return true;
		}
		if (m_bCancel) break;

		if (!pVLMDlg->VLMSolveMultiple(0.0, m_DeltaAlpha, 1))	{
			pVLMDlg->AddString("\r\n\r\nSingular matrix - aborting....\r\n");
			pVLMDlg->m_bWarning = true;
			return true;
		}
	
		if (m_bCancel) break;
			
		pVLMDlg->VLMComputePlane(m_Alpha+i*m_DeltaAlpha, m_DeltaAlpha, 1);
		
	}
	
	return true;
}




bool CVLMThread::ReLoop()
{
	CWaitCursor Wait;
	CString str;
	int nrhs;
	double QInf  = 0.0;
	double Alpha = 0.0;

	CVLMDlg * pVLMDlg = (CVLMDlg*)m_pParent;
	CMiarex * pMiarex = (CMiarex*)m_pMiarex;

	if(m_QInfMax<m_QInf) m_DeltaQInf = -abs(m_DeltaQInf);
	nrhs  = (int)abs((m_QInfMax-m_QInf)*1.0001/m_DeltaQInf) + 1;

	if(!m_bSequence) nrhs = 1;
	else if(nrhs>=100){
		int res = AfxMessageBox("The number of points to be calculated will be limited to 100", MB_OKCANCEL);
		if(res ==IDCANCEL) return false;
		nrhs = 100;
	}
	pVLMDlg->m_bTrace = true;

	if(pVLMDlg->m_pWPolar->m_bTiltedGeom)
	{
		//reset the initial geometry before a new angle is processed
		memcpy(pVLMDlg->m_pPanel,     pVLMDlg->m_pMemPanel,     pVLMDlg->m_MatSize * sizeof(CPanel));
		memcpy(pVLMDlg->m_pNode,      pVLMDlg->m_pMemNode,      pVLMDlg->m_nNodes  * sizeof(CVector));

		// Rotate the wing panels and translate the wake to the new T.E. position
		CVector O;
		pMiarex->RotateGeomY(pVLMDlg->m_pWPolar->m_ASpec, O);
		pVLMDlg->m_OpAlpha = pVLMDlg->m_pWPolar->m_ASpec;
		Alpha = 0.0;
	}
	else Alpha = m_Alpha;


	if(!pVLMDlg->VLMCreateRHS(Alpha, m_DeltaAlpha, 1)) {
		pVLMDlg->AddString("\r\n\r\nFailed to create RHS....\r\n");
		pVLMDlg->m_bWarning = true;
		return true;
	}
	if(!pVLMDlg->VLMCreateMatrix()) {
		pVLMDlg->AddString("\r\n\r\nFailed to create matrix....\r\n");
		pVLMDlg->m_bWarning = true;
		return true;
	}

//first solve for unit speed... calculation is linear

	if(!pVLMDlg->VLMSolveMultiple(m_QInf, m_DeltaQInf, nrhs)){//solve only once
		pVLMDlg->m_bWarning = true;
		return true;
	}

//then compute all points
	pVLMDlg->VLMComputePlane(m_QInf, m_DeltaQInf, nrhs);

	return true;
}
