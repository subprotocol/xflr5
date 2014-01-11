/****************************************************************************

    Iteration Thread Class
    Copyright (C) 2003 André Deperrois xflr5@yahoo.com

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


// IterThread.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "ViscThread.h"
#include "ViscDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CViscThread

IMPLEMENT_DYNCREATE(CViscThread, CWinThread)


CViscThread::CViscThread(CWnd *pParent)
{
	m_Iterations  = 0;
	m_bFinished   = false;
	m_bSkip       = false;
	m_bExit       = false;
	m_bSuspend    = false;
	m_bCalc       = false;
	m_pParent     = pParent;
	m_bType4      = false;
	m_bAutoInitBL = true;
	m_AlphaMin = 0.0;
	m_AlphaMax = 1.0;
	m_DeltaAlpha = 0.5;
	m_ClMin = 0.0;
	m_ClMax = 1.0;
	m_DeltaCl = 0.1;
	m_ReMin = 10000.0;
	m_ReMax = 100000.0;
	m_DeltaRe = 10000.0;
}


CViscThread::~CViscThread()
{
}


BOOL CViscThread::InitInstance()
{
	if (!m_bType4) {
		if(!AlphaLoop()) return FALSE;
	}
	else{
		if(!ReLoop()) return FALSE;
	}

	return FALSE;
}

int CViscThread::ExitInstance()
{
	CViscDlg* pVDlg = (CViscDlg*) m_pParent;
	m_bFinished = true;
//	pVDlg->EndSequence();
//	m_pParent->PostMessage(V_ENDTHREAD);
//	pVDlg->EndDialog(0);
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CViscThread, CWinThread)
	//{{AFX_MSG_MAP(CViscThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViscThread message handlers



void CViscThread::SetAlpha(double AlphaMin, double AlphaMax, double DeltaAlpha)
{
	m_AlphaMin   = AlphaMin;
	m_AlphaMax   = AlphaMax;
	m_DeltaAlpha = DeltaAlpha;
}

void CViscThread::SetCl(double ClMin, double ClMax, double DeltaCl)
{
	m_ClMin   = ClMin;
	m_ClMax   = ClMax;
	m_DeltaCl = DeltaCl;
}

void CViscThread::SetRe(double ReMin, double ReMax, double DeltaRe)
{
	m_ReMin   = ReMin;
	m_ReMax   = ReMax;
	m_DeltaRe = DeltaRe;
}



bool CViscThread::AlphaLoop()
{
	double alfa;
	double pi = 3.141592654;

	CViscDlg* pVDlg = (CViscDlg*) m_pParent;
	CString str;
	int ia, total;

	if(pVDlg->m_bAlpha) total=int((m_AlphaMax-m_AlphaMin)*1.0001/m_DeltaAlpha);//*1.0001 to make sure upper limit is included
	else                total=int((m_ClMax-m_ClMin)*1.0001/m_DeltaCl);//*1.0001 to make sure upper limit is included

	for (ia=0; ia<=total; ia++)
	{
		if(!m_bExit)
		{
			if(pVDlg->m_bAlpha)
			{
				alfa = m_AlphaMin+ia*m_DeltaAlpha;
				m_pXFoil->alfa = alfa*pi/180.0;
				m_pXFoil->lalfa = true;
				m_pXFoil->qinf = 1.0;
				str.Format("\r\n\r\nAlpha = %.3f\r\n\r\n", alfa);
				m_pXFoil->pXFile->WriteString(str);

				// here we go !
				if (!m_pXFoil->specal()) 
				{
					CString str;
					str.Format("Invalid Analysis Settings\nCpCalc: local speed too large \n Compressibility corrections invalid ");
					AfxMessageBox(str, MB_OK);
					m_bExit = true;
					return FALSE;
				}
			}
			else
			{
				m_pXFoil->lalfa = false;
				m_pXFoil->alfa = 0.0;
				m_pXFoil->qinf = 1.0;
				m_pXFoil->clspec = m_ClMin+ia*m_DeltaCl;
				str.Format("\r\n\r\nCl = %.3f\r\n\r\n", m_pXFoil->clspec);
				m_pXFoil->pXFile->WriteString(str);
				if(!m_pXFoil->speccl())
				{
					CString str;
					str.Format("Invalid Analysis Settings\nCpCalc: local speed too large \n Compressibility corrections invalid ");
					AfxMessageBox(str, MB_OK);
					m_bExit = true;
					return FALSE;
				}
			}
			if ((double)abs(m_pXFoil->alfa-m_pXFoil->awake) > 0.00001) 
				m_pXFoil->lwake  = false;
			if ((double)abs(m_pXFoil->alfa-m_pXFoil->avisc) > 0.00001) 
				m_pXFoil->lvconv = false;
			if ((double)abs(m_pXFoil->minf-m_pXFoil->mvisc) > 0.00001) 
				m_pXFoil->lvconv = false;

			m_pXFoil->lwake = false;
			m_pXFoil->lvconv = false;

			m_bSkip = false;
			while(!Iterate()){}

			
			pVDlg->ResetCurves();
			m_Iterations = 0;

			pVDlg->AddOpPoint();// only if converged ???
		}
		else
		{
			break;
		}
	}
	return TRUE;
}



bool CViscThread::ReLoop()
{
//	double alphadeg;
	CViscDlg* pVDlg = (CViscDlg*) m_pParent;
	CString str;
	int ia;
	double Re;

	int total=int((m_ReMax*1.0001-m_ReMin)/m_DeltaRe);//*1.0001 to make sure upper limit is included

	for (ia=0; ia<=total; ia++)
	{
		if(!m_bExit)
		{
			Re = m_ReMin+ia*m_DeltaRe;
			m_pXFoil->reinf1 = Re;
			m_pXFoil->lalfa = true;
			m_pXFoil->qinf = 1.0;
			str.Format("\r\n\r\nRe = %.f\r\n\r\n", Re);
			m_pXFoil->pXFile->WriteString(str);

			// here we go !
			if (!m_pXFoil->specal()) 
			{
				CString str;
				str.Format("Invalid Analysis Settings\nCpCalc: local speed too large \n Compressibility corrections invalid ");
				AfxMessageBox(str, MB_OK);
				m_bExit = true;
				return FALSE;
			}

			if (abs(m_pXFoil->alfa-m_pXFoil->awake) > 0.00001) 
				m_pXFoil->lwake  = false;
			if (abs(m_pXFoil->alfa-m_pXFoil->avisc) > 0.00001) 
				m_pXFoil->lvconv = false;
			if (abs(m_pXFoil->minf-m_pXFoil->mvisc) > 0.00001) 
				m_pXFoil->lvconv = false;

			m_pXFoil->lwake = false;
			m_pXFoil->lvconv = false;

			m_bSkip = false;

			while(!Iterate()){}

			pVDlg->ResetCurves();
			m_Iterations = 0;

			pVDlg->AddOpPoint();// only if converged ???
		}
		else 
		{
			break;
		}
	}
	return TRUE;
}

bool CViscThread::Iterate()
{
	CViscDlg* pViscDlg = (CViscDlg*)m_pParent;
	CCurve *pCurve;
	if(!m_pXFoil->viscal()){
		m_pXFoil->lvconv = false;//point is unconverged
		CString str;
		str.Format("CpCalc: local speed too large \r\n Compressibility corrections invalid \r\n");
		AfxMessageBox(str, MB_OK); // to advise user analysis has failed
		m_bExit = true;
		return true;// to exit loop
	}

	CString str;
    str="\r\n Solving BL system ...\r\n\r\n";
	m_pXFoil->pXFile->WriteString(str);

	while(m_Iterations<m_IterLim  && !m_pXFoil->lvconv && !m_bSkip){
		str.Format(" Iteration %d ... \r\n", m_Iterations);
		m_pXFoil->pXFile->WriteString(str);
		if(m_bSuspend) Sleep (100);
		else{
			if(m_pXFoil->ViscousIter()){
				pCurve = pViscDlg->m_RmsGraph.GetCurve(0);
				if(pCurve) pCurve->AddPoint((double)m_Iterations, m_pXFoil->rmsbl);
				pCurve = pViscDlg->m_RmsGraph.GetCurve(1);
				if(pCurve)pCurve->AddPoint((double)m_Iterations, m_pXFoil->rmxbl);
				m_Iterations++;
			}
			else {
				m_Iterations = m_IterLim;
			}
			pViscDlg->UpdateView();
		}
	}

	if(m_bSkip) {
		m_pXFoil->lblini = false;
		m_pXFoil->lipan = false;
		return true;
	}
	if(!m_pXFoil->ViscalEnd()){
		m_pXFoil->lvconv = false;//point is unconverged
		CString str;
		str.Format("CpCalc: local speed too large \r\n Compressibility corrections invalid \r\n");
		AfxMessageBox(str, MB_OK); // to advise the user that the analysis has failed
		m_bExit = true;
		m_pXFoil->lblini = false;
		m_pXFoil->lipan = false;
		return true;// to exit loop
	}
	m_bCalc = true;
	
	if(m_Iterations>=m_IterLim && !m_pXFoil->lvconv){
		str.Format("---------Point Unconverged ! ...Giving up ----------\r\n");
		m_pXFoil->pXFile->WriteString(str);
		pViscDlg->UpdateView();
		m_pXFoil->fcpmin();// Is it of any use ?
		if(m_bAutoInitBL) {
			m_pXFoil->lblini = false;
			m_pXFoil->lipan = false;
		}
		return true;
	}
	if(!m_pXFoil->lvconv) {
		m_pXFoil->fcpmin();// Is it of any use ?
		return false;
	}
	else {//converged at last
		m_pXFoil->fcpmin();
		return true;
	}
}

