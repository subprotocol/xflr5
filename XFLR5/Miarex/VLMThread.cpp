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
	pVLMDlg->AddString("Launching VLM Analysis....\r\n");

	strong.Format("Type %d Analysis\r\n", pVLMDlg->m_pWPolar->m_Type);
	pVLMDlg->AddString(strong);
	pVLMDlg->m_bCancel = false;

	if(!m_bType4) {
		if(pVLMDlg->m_bWakeRollUp)	AlphaWakeLoop();
		else {
			if(pVLMDlg->m_pWPolar->m_bTiltedGeom) AlphaTiltedLoop();
			else                                  AlphaLoop();
		}
	}
	else 		  ReLoop();

	if (!m_bCancel && !pVLMDlg->m_bWarning) pVLMDlg->AddString("\r\nVLM Analysis completed successfully\r\n");
	else if (pVLMDlg->m_bWarning)           pVLMDlg->AddString("\r\nVLM Analysis completed ... Errors encountered\r\n");

	Sleep(100);
	return FALSE;
}

int CVLMThread::ExitInstance()
{
	m_pParent->PostMessage(W_ENDTHREAD);
	m_bFinished = true;
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

	if(m_AlphaMax<m_Alpha) m_DeltaAlpha = -fabs(m_DeltaAlpha);
	nrhs  = (int)fabs((m_AlphaMax-m_Alpha)*1.0001/m_DeltaAlpha);

	if(!m_bSequence) nrhs = 0;
	else if(nrhs>=100){
		AfxMessageBox("The number of points to be calculated will be limited to 100", MB_OK);
		nrhs = 100;
	}
	pVLMDlg->m_bTrace = true;
	CWaitCursor Wait;
	str.Format("   Solving the problem... \r\n");
	pVLMDlg->AddString(str);

	if (!pVLMDlg->VLMCreateMatrix()) {
		pVLMDlg->AddString("\r\nFailed to create the matrix....\r\n");
		pVLMDlg->m_bWarning = true;
		return true;
	}

	if (m_bCancel) {
		pVLMDlg->AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");
		return true;
	}

	if (!pVLMDlg->VLMSolveMultiple(m_Alpha, m_DeltaAlpha, nrhs))	{
		if (m_bCancel) {
			pVLMDlg->AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");
			return true;
		}
		pVLMDlg->AddString("\r\n\r\nSingular matrix - aborting....\r\n");
		pVLMDlg->m_bWarning = true;
		return true;
	}
	if (m_bCancel) {
		pVLMDlg->AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");
		return true;
	}
		
	pVLMDlg->VLMComputePlane(m_Alpha, m_DeltaAlpha, nrhs);
	
	if (m_bCancel) {
		 pVLMDlg->AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");
		return true;
	}
	return true;
}

bool CVLMThread::AlphaWakeLoop()
{
	CString str;
	CVLMDlg * pVLMDlg = (CVLMDlg*)m_pParent;
	int nrhs;
	double Alpha = m_Alpha;

	if(m_AlphaMax<m_Alpha) m_DeltaAlpha = -fabs(m_DeltaAlpha);
	nrhs  = (int)fabs((m_AlphaMax-m_Alpha)*1.0001/m_DeltaAlpha);

	if(!m_bSequence) nrhs = 0;
	else if(nrhs>=100){
		AfxMessageBox("The number of points to be calculated will be limited to 100", MB_OK);
		nrhs = 100;
	}
	pVLMDlg->m_bTrace = true;
	CWaitCursor Wait;

	str.Format("      Solving the problem... \r\n");
	pVLMDlg->AddString(str);

	for (int n=0; n<=nrhs; n++){
		if (m_bCancel) {
			pVLMDlg->AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");
			return true;
		} 
		for (int nw=0; nw<m_MaxWakeIter; nw++){
			if (!pVLMDlg->VLMCreateMatrix()) {
				pVLMDlg->AddString("\r\nFailed to create the matrix....\r\n");
				pVLMDlg->m_bWarning = true;
				return true;
			}
			if (m_bCancel) {
				pVLMDlg->AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");
				return true;
			}

			if (!pVLMDlg->VLMSolveMultiple(m_Alpha, m_DeltaAlpha, 0))	{
				pVLMDlg->AddString("\r\n\r\nSingular matrix - aborting....\r\n");
				pVLMDlg->m_bWarning = true;
				return true;
			}
			str.Format("  Iter %d  -  Moving wake nodes\r\n",nw);
			pVLMDlg->AddString(str);
			pVLMDlg->VLMMoveWakeNodes();			
			if (m_bCancel) {
				pVLMDlg->AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");
				return true;
			}
		}
		pVLMDlg->VLMComputePlane(m_Alpha, m_DeltaAlpha, 0);
		m_Alpha += m_DeltaAlpha;
	}
	
	if (m_bCancel) {
		 pVLMDlg->AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");
		return true;
	}
	return true;
}


bool CVLMThread::ReLoop()
{
	CString str;
	int nrhs;
	double QInf=0.0;

	CVLMDlg * pVLMDlg = (CVLMDlg*)m_pParent;
	CMiarex * pMiarex = (CMiarex*)m_pMiarex;

	if(m_QInfMax<m_QInf) m_DeltaQInf = -fabs(m_DeltaQInf);
	nrhs  = (int)fabs((m_QInfMax-m_QInf)*1.0001/m_DeltaQInf);

	if(!m_bSequence) nrhs = 0;
	else if(nrhs>=100){
		AfxMessageBox("The number of points to be calculated will be limited to 100", MB_OK);
		nrhs = 100;
	}
	pVLMDlg->m_bTrace = true;
	CWaitCursor Wait;

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


bool CVLMThread::AlphaTiltedLoop()
{
	CString str;
	CVLMDlg * pVLMDlg = (CVLMDlg*)m_pParent;
	int nrhs;
	CVector O(0.0,0.0,0.0);

	if(m_AlphaMax<m_Alpha) m_DeltaAlpha = -fabs(m_DeltaAlpha);
	nrhs  = (int)fabs((m_AlphaMax-m_Alpha)*1.0001/m_DeltaAlpha);

	if(!m_bSequence) nrhs = 0;
	else if(nrhs>=100){
		AfxMessageBox("The number of points to be calculated will be limited to 100", MB_OK);
		nrhs = 100;
	}

	pVLMDlg->m_bTrace = true;
	pVLMDlg->m_Alpha = 0.0;
	CWaitCursor Wait;
	str.Format("   Solving the problem... \r\n");
	pVLMDlg->AddString(str);

	for (int i=0; i<=nrhs; i++){

		memcpy(pVLMDlg->m_pPanel, pVLMDlg->m_pRefPanels, sizeof(pVLMDlg->m_pPanel));
		memcpy(pVLMDlg->m_pNode, pVLMDlg->m_pRefNodes, sizeof(pVLMDlg->m_pNode));
		for (int l=0; l<pVLMDlg->m_NSurfaces; l++)
			pVLMDlg->m_pSurface[l].Copy(pVLMDlg->m_pRefSurfaces[l]);
//		memcpy(pVLMDlg->m_pSurface, pVLMDlg->m_pRefSurfaces, sizeof(pVLMDlg->m_pSurface));

		pVLMDlg->VLMRotateGeomY(m_Alpha+i*m_DeltaAlpha, O);

		pVLMDlg->m_AlphaCalc = m_Alpha+i*m_DeltaAlpha;

		if (!pVLMDlg->VLMCreateMatrix()) {
			pVLMDlg->AddString("\r\nFailed to create the matrix....\r\n");
			pVLMDlg->m_bWarning = true;
			return true;
		}

		if (m_bCancel) {
			pVLMDlg->AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");
			return true;
		}

		if (!pVLMDlg->VLMSolveMultiple(0.0, m_DeltaAlpha, 0))	{
			pVLMDlg->AddString("\r\n\r\nSingular matrix - aborting....\r\n");
			pVLMDlg->m_bWarning = true;
			return true;
		}
		if (m_bCancel) {
			pVLMDlg->AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");
			return true;
		}
			
		pVLMDlg->VLMComputePlane(m_Alpha+i*m_DeltaAlpha, m_DeltaAlpha, 0);
		
		if (m_bCancel) {
			pVLMDlg->AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");
			return true;
		}
	}
	return true;
}


