/****************************************************************************

	Batch Thread class
    Copyright (C) 2004 André Deperrois XFLR5@yahoo.com

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
// BatchThread.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "BatchThread.h"
#include "BatchDlg.h"
#include "../main/MainFrm.h"

/////////////////////////////////////////////////////////////////////////////
// CBatchThread

IMPLEMENT_DYNCREATE(CBatchThread, CWinThread)

CBatchThread::CBatchThread(CWnd* pParent)
{
	m_pParent    = pParent;
	m_IterLim    = 100;
	m_ReList     = NULL;
	m_MachList   = NULL;
	m_NCritList  = NULL;
	m_NRe        = 0;
	m_bCancel         = false;
	m_bType4          = false;
	m_bInitBL         = false;
	m_bShowTextOutput = true;
	m_bFromList       = false;
	m_bFromZero       = false;
	m_bSkipPoint      = false;
	m_bSkipPolar      = false;
}

CBatchThread::~CBatchThread(){
}


int CBatchThread::ExitInstance()
{
	CBatchDlg* pBDlg = (CBatchDlg*) m_pParent;
	pBDlg->ResetCurves();
	pBDlg->UpdateGraph(-100.0, 0.0);
//	m_pParent->PostMessage(WM_CLOSE);
//	pBDlg->EndDialog(0);
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CBatchThread, CWinThread)
	//{{AFX_MSG_MAP(CBatchThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchThread message handlers

bool CBatchThread::Iterate()
{
	CBatchDlg* pBDlg = (CBatchDlg*) m_pParent;
	CString StrTmp;

	if(!m_pXFoil->viscal())
	{
		m_pXFoil->lvconv = false;//point is unconverged
		CString str;
		str.Format("CpCalc: local speed too large \r\n Compressibility corrections invalid \r\n");
		AfxMessageBox(str, MB_OK); // to advise user that the analysis has failed
		m_bCancel = true;
		return true;
	}

	while(m_Iterations<m_IterLim && !m_pXFoil->lvconv && !m_bCancel && !m_bSkipPoint && !m_bSkipPolar)
	{
		if(m_pXFoil->ViscousIter())
		{
			m_Iterations++;	
			pBDlg->OutputIter(m_Iterations, m_pXFoil->reinf1, m_pXFoil->alfa);
		}
		else 
		{
			m_Iterations = m_IterLim;
		}
	}
	pBDlg->OutputIter(0, 0.0, 0.0);
	if(m_bCancel) return true;

	if(m_bSkipPoint || m_bSkipPolar)
	{
		m_pXFoil->lblini = false;
		m_pXFoil->lipan = false;
		return true;
	}
	
	if(!m_pXFoil->ViscalEnd())
	{
		m_pXFoil->lvconv = false;//point is unconverged
		CString str;
		str.Format("CpCalc: local speed too large \r\n Compressibility corrections invalid \r\n");
		AfxMessageBox(str, MB_OK); // to advise user analysis has failed
		m_bCancel = true;
		m_pXFoil->lblini = false;
		m_pXFoil->lipan  = false;
		return true;// to exit loop
	}
	m_bCalc = true;
	
	if(m_Iterations>=m_IterLim && !m_pXFoil->lvconv)
	{
		if(m_pXDirect->m_bAutoInitBL) {
			m_pXFoil->lblini = false;
			m_pXFoil->lipan = false;
		}
		m_pXFoil->fcpmin();// Is it of any use ?
		return true;
	}
	if(!m_pXFoil->lvconv) {
		m_pXFoil->fcpmin();// Is it of any use ?
		return false;
	}
	else {//converged at last
		m_pXFoil->fcpmin();// Is it of any use ?
		return true;
	}
}


void CBatchThread::SetPlrName()
{
	if(!m_bType4){
		double R = m_pCurPolar->m_Reynolds/1000000.;
		m_pCurPolar->m_PlrName.Format("T%d_Re%.2f_M%.2f", m_pCurPolar->m_Type, R, m_pCurPolar->m_Mach);
	}
	else{
		m_pCurPolar->m_PlrName.Format("T%d_Al%5.2f_M%.2f", m_pCurPolar->m_Type, m_pCurPolar->m_ASpec, m_pCurPolar->m_Mach);
	}
	CString str;
	str.Format("_N%.1f", m_pCurPolar->m_ACrit);
	m_pCurPolar->m_PlrName += str;
}


void CBatchThread::Cancel()
{
	m_bCancel = true;
}

BOOL CBatchThread::InitInstance()
{
	CBatchDlg* pBDlg = (CBatchDlg*) m_pParent;

	if(!m_bType4) 
		ReLoop();
	
	else AlphaLoop();

	CString strong = "Analysis completed";
	if(m_bShowTextOutput) pBDlg->UpdateOutput(strong);

	pBDlg->CleanUp();
	return FALSE;
}

bool CBatchThread::ReLoop()
{
	CBatchDlg* pBDlg = (CBatchDlg*) m_pParent;
	CString str;
	CString strong = "";
	CString StrTmp;
	double alphadeg, alfa;
	int ia, iRe, nRe, series, total, MaxSeries;
	double SpMin, SpMax, SpInc;

	if(!m_bFromList) nRe = (int)abs((m_ReMax-m_ReMin)/m_ReInc);
	else             nRe = m_NRe-1;

	for (iRe=0; iRe<=nRe; iRe++)
	{
		if(!m_bFromList) m_pXFoil->reinf1 = m_ReMin + iRe *m_ReInc;
		else
		{
			m_pXFoil->reinf1 = m_ReList[iRe];
			m_pXFoil->minf1  = m_MachList[iRe];
			m_pXFoil->acrit  = m_NCritList[iRe];
		}
		str.Format("Re = %8.0f\r\n", m_pXFoil->reinf1);
		strong+= str;
		if(m_bShowTextOutput) pBDlg->UpdateOutput(str);

		if (m_bFromZero && m_SpMin*m_SpMax<0) 
		{
			MaxSeries = 2;
			SpMin = 0.0;
			SpMax = m_SpMax;
			SpInc = m_SpInc;
		}
		else  
		{
			MaxSeries = 1;
			SpMin = m_SpMin;
			SpMax = m_SpMax;
			SpInc = m_SpInc;
		}

		m_bSkipPolar = false;

		for (series=0; series<MaxSeries;series++)
		{
			total = int((SpMax*1.0001-SpMin)/SpInc);//*1.0001 to make sure upper limit is included
			CreatePolar(m_pXFoil->reinf1, m_pXFoil->minf1, m_pXFoil->acrit);
			if (m_bInitBL)
			{
				m_pXFoil->lblini = false;
				m_pXFoil->lipan = false;
			}

			for (ia=0; ia<=total; ia++)
			{
				pBDlg->ResetCurves();
				if(!m_bCancel && !m_bSkipPolar)
				{
					if(m_bAlpha)
					{
						alfa = SpMin+ia*SpInc;
						m_pXFoil->alfa = alfa;
						m_pXFoil->lalfa = true;
						m_pXFoil->qinf = 1.0;
						alphadeg = m_pXFoil->alfa*180./3.141592654;
						str.Format("Alpha = %9.3f", alphadeg);
						strong+=str;
						if(m_bShowTextOutput) pBDlg->UpdateOutput(str);

						// here we go !
						if (!m_pXFoil->specal())
						{
							CString str;
							str.Format("Invalid Analysis Settings\nCpCalc: local speed too large \n Compressibility corrections invalid ");
							AfxMessageBox(str, MB_OK);
							m_bCancel = true;
							pBDlg->CleanUp();
							return FALSE;
						}
					}
					else
					{
						m_pXFoil->lalfa = false;
						m_pXFoil->alfa = 0.0;
						m_pXFoil->qinf = 1.0;
						m_pXFoil->clspec = SpMin+ia*SpInc;
						str.Format("Cl = %9.3f", m_pXFoil->clspec);
						strong+=str;
						if(m_bShowTextOutput) pBDlg->UpdateOutput(str);
						if(!m_pXFoil->speccl())
						{
							CString str;
							str.Format("Invalid Analysis Settings\nCpCalc: local speed too large \n Compressibility corrections invalid ");
							AfxMessageBox(str, MB_OK);
							m_bCancel = true;
							pBDlg->CleanUp();
							return FALSE;
						}
					}

					m_pXFoil->lwake = false;
					m_pXFoil->lvconv = false;

					m_Iterations = 0;

					m_bSkipPoint = false;

					while(!Iterate()){}

					if(m_pXFoil->lvconv)
					{
						str.Format("   ...converged after %3d iterations\r\n", m_Iterations);
						strong+= str;
						if(m_bShowTextOutput) pBDlg->UpdateOutput(str);
						m_pCurPolar->AddData(m_pXFoil);
					}
					else if(m_bSkipPoint || m_bSkipPolar)
					{
						str.Format("   ...skipped after %3d iterations\r\n", m_Iterations);
						strong+= str;
						if(m_bShowTextOutput) pBDlg->UpdateOutput(str);

					}
					else
					{
						str.Format("   ...unconverged after %3d iterations\r\n", m_Iterations);
						strong+= str;
						if(m_bShowTextOutput) pBDlg->UpdateOutput(str);

					}
					CXDirect* pXDirect = (CXDirect*)pBDlg->m_pXDirect;
					if(pXDirect->m_bPolar){
						pXDirect->CreatePolarCurves();
						pXDirect->UpdateView();
					}
				}
				else
				{
					break;
				}
			}// end Alpha or Cl loop
			SpMin = 0;
			SpMax = m_SpMin;
			SpInc = -m_SpInc;
		}
		strong+="\r\n";
		if(m_bCancel)
		{
			strong+="Analysis interrupted\r\n";
			break;
		}
	}//end Re loop
	m_pXFile->WriteString(strong);
	return true;
}

bool CBatchThread::AlphaLoop()
{
	CBatchDlg* pBDlg = (CBatchDlg*) m_pParent;
	CString str;
	CString strong = "";
	CString StrTmp;
	int iRe, iAlpha, nAlpha;
	double alphadeg;
	nAlpha = (int)(abs((m_SpMax-m_SpMin)*1.000f/m_SpInc));//*1.0001 to make sure upper limit is included
	
	for (iAlpha=0; iAlpha<=nAlpha; iAlpha++)
	{
		alphadeg = m_SpMin + iAlpha*m_SpInc;
		m_pXFoil->alfa = alphadeg*3.141592654/180.0;
		str.Format("Alpha = %.2f\r\n", alphadeg);
		strong+= str;
		if(m_bShowTextOutput) pBDlg->UpdateOutput(str);

		int total = (int)(abs((m_ReMax-m_ReMin)*1.0001/m_ReInc));
		CreatePolar(alphadeg, m_pXFoil->minf1, m_pXFoil->acrit);// Do something

		if (m_bInitBL)
		{
			m_pXFoil->lblini = false;
			m_pXFoil->lipan = false;
		}

		m_bSkipPolar = false;

		for (iRe=0; iRe<=total; iRe++)
		{
			pBDlg->ResetCurves();
			if(!m_bCancel && !m_bSkipPolar)
			{
				m_pXFoil->reinf1 = m_ReMin + iRe *m_ReInc;
				m_pXFoil->lalfa  = true;
				m_pXFoil->qinf   = 1.0;
				str.Format("Re = %8.0f", m_pXFoil->reinf1);
				strong+=str;
				if(m_bShowTextOutput) pBDlg->UpdateOutput(str);

				// here we go !
				if (!m_pXFoil->specal()) 
				{
					CString str;
					str.Format("Invalid Analysis Settings\nCpCalc: local speed too large \n Compressibility corrections invalid ");
					AfxMessageBox(str, MB_OK);
					m_bCancel = true;
					pBDlg->CleanUp();
					return FALSE;
				}

				if (abs(m_pXFoil->alfa-m_pXFoil->awake) > 0.00001) m_pXFoil->lwake  = false;
				if (abs(m_pXFoil->alfa-m_pXFoil->avisc) > 0.00001) m_pXFoil->lvconv = false;
				if (abs(m_pXFoil->minf-m_pXFoil->mvisc) > 0.00001) m_pXFoil->lvconv = false;

				m_pXFoil->lwake  = false;
				m_pXFoil->lvconv = false;

				m_Iterations = 0;

				m_bSkipPoint = false;

				while(!Iterate()){}

				if(m_pXFoil->lvconv){
					str.Format("   ...converged after %3d iterations\r\n", m_Iterations);
					strong+= str;
					if(m_bShowTextOutput) pBDlg->UpdateOutput(str);
					m_pCurPolar->AddData(m_pXFoil);
				}
				else if(m_bSkipPoint || m_bSkipPolar){
					str.Format("   ...skipped after %3d iterations\r\n", m_Iterations);
					strong+= str;
					if(m_bShowTextOutput) pBDlg->UpdateOutput(str);

				}
				else {
					str.Format("   ...unconverged after %3d iterations\r\n", m_Iterations);
					strong+= str;
					if(m_bShowTextOutput) pBDlg->UpdateOutput(str);
				}
			}
			else {
				break;
			}
		}// end Alpha or Cl loop
		strong+="\r\n";
		if(m_bCancel){
			strong+="Analysis interrupted\r\n";
			break;
		}
	}//end Re loop
	m_pXFile->WriteString(strong);
	return true;
}



void CBatchThread::CreatePolar(double Spec, double Mach, double NCrit)
{
	CBatchDlg* pBDlg = (CBatchDlg*) m_pParent;
	
	m_pCurPolar = new CPolar(m_pXDirect->m_pFrame);
	m_pCurPolar->m_FoilName   = pBDlg->m_FoilName;
	m_pCurPolar->m_bIsVisible = true;
	m_pCurPolar->m_Type = pBDlg->m_Type;
	switch (m_pCurPolar->m_Type){
		case 1:
			m_pCurPolar->m_MaType = 1;
			m_pCurPolar->m_ReType = 1;
			break;
		case 2:
			m_pCurPolar->m_MaType = 2;
			m_pCurPolar->m_ReType = 2;
			break;
		case 3:
			m_pCurPolar->m_MaType = 1;
			m_pCurPolar->m_ReType = 3;
			break;
		case 4:
			m_pCurPolar->m_MaType = 1;
			m_pCurPolar->m_ReType = 1;
			break;
		default:
			m_pCurPolar->m_ReType = 1;
			m_pCurPolar->m_MaType = 1;
			break;
	}
	if(!m_bType4){
		m_pCurPolar->m_Reynolds = Spec;
	}
	else{
		m_pCurPolar->m_ASpec = Spec;
	}
	m_pCurPolar->m_Mach  = Mach;
	m_pCurPolar->m_ACrit = NCrit;

	m_pCurPolar->m_XTop  = pBDlg->m_XTopTr;
	m_pCurPolar->m_XBot  = pBDlg->m_XBotTr;

	CMainFrame *pFrame = (CMainFrame*)m_pXDirect->m_pFrame;
	m_pCurPolar->m_Color = pFrame->GetColor(1);

	SetPlrName();
	CPolar *pPolar = m_pXDirect->GetPolar(m_pCurPolar->m_PlrName);
	if(pPolar){
		delete m_pCurPolar;
		m_pCurPolar = pPolar;
	}
	else m_pCurPolar = m_pXDirect->AddPolar(m_pCurPolar);
}

