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

// C3DPanelThread

IMPLEMENT_DYNCREATE(C3DPanelThread, CWinThread)

C3DPanelThread::C3DPanelThread()
{
	m_bCancel   = false;
	m_bSequence = false;
	m_bFinished = false;
	m_pWPolar = NULL;
}

C3DPanelThread::~C3DPanelThread()
{
}

BOOL C3DPanelThread::InitInstance()
{
	CString strong;
	C3DPanelDlg * pDlg = (C3DPanelDlg*)m_pParent;
	pDlg->AddString("Launching Panel Analysis....\r\n");

	strong.Format("Type %d Analysis\r\n", m_pWPolar->m_Type);
	pDlg->AddString(strong);
	pDlg->m_bCancel = false;

	if(m_pWPolar->m_Type != 4) 
		AlphaLoop();
	else 
		ReLoop();

	if (!m_bCancel && !pDlg->m_bWarning) pDlg->AddString("\r\nPanel Analysis completed successfully\r\n");
	else if (pDlg->m_bWarning)           pDlg->AddString("\r\nPanel Analysis completed ... Errors encountered\r\n");

	Sleep(100);
	return FALSE;
}

int C3DPanelThread::ExitInstance()
{
	m_bFinished = true;
//	m_pParent->PostMessage(V_ENDTHREAD);
//	C3DPanelDlg * pDlg = (C3DPanelDlg*)m_pParent;
//	pDlg->EndDialog(0);
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(C3DPanelThread, CWinThread)
END_MESSAGE_MAP()


// Gestionnaires de messages C3DPanelThread

bool C3DPanelThread::AlphaLoop(void)
{
	CString str;
	C3DPanelDlg * pDlg = (C3DPanelDlg*)m_pParent;
	int nrhs;

	if(m_AlphaMax<m_Alpha) m_DeltaAlpha = -abs(m_DeltaAlpha);
	nrhs  = (int)abs((m_AlphaMax-m_Alpha)*1.0001/m_DeltaAlpha);

	if(!m_bSequence) nrhs = 0;
	else if(nrhs>=100){
		AfxMessageBox("The number of points to be calculated will be limited to 100", MB_OK);
		nrhs = 100;
	}
	CWaitCursor Wait;
	str.Format("   Solving the problem... \r\n");
	pDlg->AddString(str);

	if (!pDlg->CreateRHS(m_Alpha, m_DeltaAlpha, nrhs)) {
		pDlg->AddString("\r\nFailed to create RHS Vector....\r\n");
		pDlg->m_bWarning = true;
		return true;
	}

	if (!pDlg->CreateMatrix()) {
		pDlg->AddString("\r\nFailed to create the matrix....\r\n");
		pDlg->m_bWarning = true;
		return true;
	}

	if (m_bCancel) {
		pDlg->AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");
		return true;
	}
	if (!pDlg->AddWakeContribution()) {
		pDlg->AddString("\r\nFailed to create the matrix....\r\n");
		pDlg->m_bWarning = true;
		return true;
	}
	if (m_bCancel) {
		pDlg->AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");
		return true;
	}

	if (!pDlg->SolveMultiple(m_Alpha, m_DeltaAlpha, nrhs))	{
		pDlg->AddString("\r\n\r\nSingular matrix - aborting....\r\n");
		pDlg->m_bWarning = true;
		return true;
	}
	if (m_bCancel) {
		pDlg->AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");
		return true;
	}
		
	pDlg->ComputePlane(m_Alpha, m_DeltaAlpha, nrhs);
	
	if (m_bCancel) {
		 pDlg->AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");
		return true;
	}
	return true;
}

bool C3DPanelThread::ReLoop(void)
{
	return false;
}