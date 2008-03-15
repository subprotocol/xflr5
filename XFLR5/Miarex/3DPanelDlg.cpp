/****************************************************************************

    C3DPanelDlg Class
	Copyright (C) 2007-2008 André Deperrois xflr5@yahoo.com

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


#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "Miarex.h"
#include ".\3dpaneldlg.h"
#include <math.h>



C3DPanelDlg::C3DPanelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(C3DPanelDlg::IDD, pParent)
{
	pi  = 3.141592654;
	RFF = 10.0;
	eps = 1.e-7;

	m_b3DSymetric    = false;
	m_bSequence      = false;
	m_bWarning       = false;
	m_bType4         = false;
	m_bXFile         = false;
	m_bConverged     = false;
	m_bDirichlet     = true;//true if Dirichlet boundary conditions, false if Neumann
	m_bCancel        = false;
	m_bTrefftz       = false;

	m_QInf       = 0.0;//Speed vector in m/s
	m_Alpha      = 0.0;//Angle of Attack in °
	m_AlphaMax   = 0.0;
	m_DeltaAlpha = 0.0;

	m_MatSize        = 0;
	m_nNodes         = 0;
	m_NSurfaces      = 0;
	m_NWakeColumn    = 0;
	m_WakeInterNodes = 1;
	m_MaxWakeIter    = 0;

	m_nWakeNodes = 0;
	m_WakeSize   = 0;

	m_strOut = "";

	m_GCm = m_GRm = m_GYm = m_VCm = m_VYm = m_IYm = 0.0;

	m_ppBody  = NULL;
	m_pWing   = NULL;//pointer to the geometry class of the wing 
	m_pWing2  = NULL;
	m_pStab   = NULL;
	m_pFin    = NULL;
	m_pPlane  = NULL;
	m_pMiarex = NULL;
	m_pFrame  = NULL;
	m_pPanelThread = NULL;
	m_pWPolar      = NULL;;

	m_ppPanel       = NULL;
	m_pPanel        = NULL;
	m_pWakePanel    = NULL;
	m_pRefWakePanel = NULL;
	m_pMemPanel     = NULL;
	m_pNode         = NULL;
	m_pMemNode      = NULL;
	m_pWakeNode     = NULL;
	m_pRefWakeNode  = NULL;
	m_pTempWakeNode = NULL;

	memset(m_Sigma,  0, sizeof(m_Sigma));
	memset(m_Mu,     0, sizeof(m_Mu));
	memset(m_Cp,     0, sizeof(m_Cp));
	memset(m_Speed,  0, sizeof(m_Speed));
}

C3DPanelDlg::~C3DPanelDlg()
{
}

void C3DPanelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTPUT,   m_ctrlOutput);
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
}


BEGIN_MESSAGE_MAP(C3DPanelDlg, CDialog)
//	ON_MESSAGE(V_ENDTHREAD, OnEndViscDialog)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL C3DPanelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString str;
	CString strAppDirectory;
	char    szAppPath[MAX_PATH] = "";
	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
	// Extract directory
	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.GetLength()-9);
	str = strAppDirectory + "XFLR5.log";
	BOOL bOpen = m_XFile.Open(str, CFile::modeCreate | CFile::modeWrite);
	if(bOpen) m_bXFile = true;
	else      m_bXFile = false;

	m_bPointOut = false;
	m_bCancel   = false;
	m_bWarning  = false;

	SetFileHeader();
	if(m_pWPolar && (m_pWPolar->m_bTiltedGeom || m_pWPolar->m_bWakeRollUp))
	{
		//back-up the current geometry if the analysis is to be performed on the tilted geometry
		memcpy(m_pMemPanel, m_pPanel, m_MatSize * sizeof(CPanel));
		memcpy(m_pMemNode,  m_pNode,  m_nNodes * sizeof(CVector));
		memcpy(m_pRefWakePanel, m_pWakePanel, m_WakeSize * sizeof(CPanel));
		memcpy(m_pRefWakeNode,  m_pWakeNode,  m_nWakeNodes * sizeof(CVector));
	}

	str = "";
	m_b3DSymetric = m_pWing->m_bSymetric;
	if(!m_pWing->m_bSymetric) str += "     Main wing is asymmetric\r\n";

	if(m_pWing2)
	{
		if(!m_pWing2->m_bSymetric)
		{
			m_b3DSymetric = false;
			str += "     2nd wing is asymmetric\r\n";
		}
	}
	if(m_pStab)  
	{
		if(!m_pStab->m_bSymetric)
		{
			m_b3DSymetric = false;
			str += "     Elevator is asymmetric\r\n";
		}
	}

	if(m_pFin)   
	{
		m_b3DSymetric = false;
		str += "     A fin is considered asymmetric\r\n";
	}

	if (m_b3DSymetric) AddString("Perfoming symmetric calculation\r\n\r\n");
	else 
	{
		str = "Performing asymmetric calculation : "+ str +"\r\n";
		AddString(str);
	}


	str.Format("Counted %4d panel elements\r\n", m_MatSize);
	AddString(str);

	AddString("\r\n");

	m_pWing->m_bVLMSymetric = m_b3DSymetric;
	if(m_pWing2)  m_pWing2->m_bVLMSymetric = m_b3DSymetric;
	if(m_pStab)   m_pStab->m_bVLMSymetric  = m_b3DSymetric; 
	if(m_pFin)    m_pFin->m_bVLMSymetric   = m_b3DSymetric; 

	m_ctrlOutput.SetLimitText(100000);

	m_ctrlProgress.SetRange(0,100);
	m_Progress = 0;

	if(m_pWPolar->m_AnalysisType==3) 
		StartPanelThread();

	SetTimer(ID_THREADTIMER, 100, NULL);
	return FALSE;
}


void C3DPanelDlg::OnTimer(UINT nIDEvent)
{
	if(m_pPanelThread && m_pPanelThread->m_bFinished){
		HANDLE hThread = m_pPanelThread->m_hThread;
		WaitForSingleObject(hThread, INFINITE);
		delete m_pPanelThread;
		m_pPanelThread = NULL;
		EndSequence();
//		m_XFile.Close();
//		EndDialog(0);
	}
	CDialog::OnTimer(nIDEvent);
}


bool C3DPanelDlg::StartPanelThread()
{
	m_pPanelThread = new C3DPanelThread();

	m_pPanelThread->m_pParent     = this;
	m_pPanelThread->m_pMiarex     = m_pMiarex;
	m_pPanelThread->m_bAutoDelete = false;
	m_pPanelThread->m_pWPolar     = m_pWPolar;
	m_pPanelThread->m_bSequence   = m_bSequence;
	m_pPanelThread->m_Alpha       = m_Alpha;
	m_pPanelThread->m_AlphaMax    = m_AlphaMax;
	m_pPanelThread->m_DeltaAlpha  = m_DeltaAlpha;
	m_pPanelThread->m_QInf        = m_QInf;
	m_pPanelThread->m_QInfMax     = m_QInfMax;
	m_pPanelThread->m_DeltaQInf   = m_DeltaQInf;
			
	m_pPanelThread->CreateThread(CREATE_SUSPENDED);
	VERIFY(m_pPanelThread->SetThreadPriority(THREAD_PRIORITY_LOWEST));

	m_pPanelThread->ResumeThread();
	return true;
}

void C3DPanelDlg::OnCancel()
{
	if(!m_pPanelThread) {
		CDialog::OnCancel();
		return;
	}
	if(m_pPanelThread->m_bFinished) {
		HANDLE hThread = m_pPanelThread->m_hThread;
		WaitForSingleObject(hThread, INFINITE);
		EndSequence();
	}
	else {
		m_bCancel                 = true;
		m_pWing->m_bCancel        = true;
		if(m_pWing2) m_pWing2->m_bCancel      = true;
		if(m_pStab)  m_pStab->m_bCancel       = true;
		if(m_pFin)   m_pFin->m_bCancel        = true;
		m_pPanelThread->m_bCancel = true;
	}
}

void C3DPanelDlg::SetProgress(int TaskSize, double TaskProgress)
{
	m_ctrlProgress.SetPos(m_Progress+ (int)((double)TaskSize * TaskProgress));
}

void C3DPanelDlg::EndSequence()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;

	if(m_pWPolar && m_pWPolar->m_bTiltedGeom)
	{
		//restore the initial geometry
		memcpy(m_pPanel, m_pMemPanel, m_MatSize * sizeof(CPanel));
		memcpy(m_pNode,  m_pMemNode,  m_nNodes * sizeof(CVector));

		//and reset the wake for new calculations
		memcpy(m_pWakePanel, m_pRefWakePanel, m_WakeSize * sizeof(CPanel));
		memcpy(m_pWakeNode,  m_pRefWakeNode,  m_nWakeNodes * sizeof(CVector));
	}

	if (m_bCancel) 
		AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");

	pMiarex->m_bVLMFinished = true;
	m_bXFile = false;
	m_XFile.Close();
	if(m_bWarning && pMiarex->m_bLogFile){
		if(IDYES == AfxMessageBox("Some points were found outside the available flight envelope\r\nView the Log file for details ?", MB_YESNOCANCEL)){
			CFile lf;
			CString str;
			CString strAppDirectory;
			char    szAppPath[MAX_PATH] = "";
			::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
			// Extract directory
			strAppDirectory = szAppPath;
			strAppDirectory = strAppDirectory.Left(strAppDirectory.GetLength()-9);
			str =strAppDirectory + "XFLR5.log";

			if(lf.Open(str, CFile::modeRead)){// file exists (there should be a better way to do this)
				lf.Close();
				
				ShellExecute(GetSafeHwnd(),
					_T("open"),
					str,
					NULL,
					NULL,
					SW_SHOWNORMAL);
			}
		}
	}

	EndDialog(0);
}
     
BOOL C3DPanelDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{ 
		OnCancel();
		return true;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void C3DPanelDlg::SetFileHeader()
{
	m_XFile.WriteString("\n");
	m_XFile.WriteString(m_VersionName);
	m_XFile.WriteString("\n\n");
	if(!m_pPlane)	m_XFile.WriteString(m_pWing->m_WingName);
	else 		    m_XFile.WriteString(m_pPlane->m_PlaneName);
	m_XFile.WriteString("\n\n");

	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString str, strong;
	switch (tm.wMonth)
	{
		case 1:{
			strong = "January";
			break;
		}
		case 2:{
			strong = "February";
			break;
		}
		case 3:{
			strong = "March";
			break;
		}
		case 4:{
			strong = "April";
			break;
		}
		case 5:{
			strong = "May";
			break;
		}
		case 6:{
			strong = "June";
			break;
		}
		case 7:{
			strong = "July";
			break;
		}
		case 8:{
			strong = "August";
			break;
		}
		case 9:{
			strong = "September";
			break;
		}
		case 10:{
			strong = "October";
			break;
		}
		case 11:{
			strong = "November";
			break;
		}
		case 12:{
			strong = "December";
			break;
		}
	}
	str.Format(" %02d, %d  at  %02d:%02d:%02d \n\n", tm.wDay, tm.wYear, tm.wHour, tm.wMinute, tm.wSecond);

	m_XFile.WriteString(strong + str);
	if(m_pWPolar->m_AnalysisType==3){
		m_XFile.WriteString("\n\nPanel Analysis\n\n\n");
	}
	m_XFile.WriteString("\n___________________________________\n\n");
}


void C3DPanelDlg::AddString(CString strong)
{
	if(m_bXFile)
	{
		m_XFile.WriteString(strong);
		int length = m_ctrlOutput.GetWindowTextLength();
		m_ctrlOutput.SetSel(length,length,true);
		m_ctrlOutput.ReplaceSel(strong);
	}
}


bool C3DPanelDlg::Gauss(double *A, int n, double *B, int m, int TaskSize)
{
 	int row, i, j, pivot_row, k;
	double max, dum, *pa, *pA, *A_pivot_row;
	// for each variable find pivot row and perform forward substitution
	pa = A;
	for (row = 0; row < (n - 1); row++, pa += n) 
	{
		//  find the pivot row
		A_pivot_row = pa;
		max = abs(*(pa + row));
		pA = pa + n;
		pivot_row = row;
		for (i=row+1; i < n; pA+=n, i++)
			if ((dum = abs(*(pA+row))) > max) 
			{ 
				max = dum; 
				A_pivot_row = pA; 
				pivot_row = i; 
			}
		if (max <= 0.0) 
			return false;                // the matrix A is singular
		
			// and if it differs from the current row, interchange the two rows.
			
		if (pivot_row != row) {
			for (i = row; i < n; i++) {
				dum = *(pa + i);
				*(pa + i) = *(A_pivot_row + i);
				*(A_pivot_row + i) = dum;
			}
			for(k=0; k<=m; k++){
				dum = B[row+k*n];
				B[row+k*n] = B[pivot_row+k*n];
				B[pivot_row+k*n] = dum;
			}
		}
		
		// Perform forward substitution
		for (i = row+1; i<n; i++) {
			pA = A + i * n;
			dum = - *(pA + row) / *(pa + row);
			*(pA + row) = 0.0;
			for (j=row+1; j<n; j++) *(pA+j) += dum * *(pa + j);
			for (k=0; k<=m; k++) 
				B[i+k*n] += dum * B[row+k*n];
		}
		if(m_bCancel) break;
		SetProgress((int)(TaskSize/2), (double)row/(double)n);
	}

	m_Progress +=  (int)(TaskSize/2);

	// Perform backward substitution
	
	pa = A + (n - 1) * n;
	for (row = n - 1; row >= 0; pa -= n, row--) {
		if ( *(pa + row) == 0.0 ) 
			return false;           // matrix is singular
		dum = 1.0 / *(pa + row);
		for ( i = row + 1; i < n; i++) *(pa + i) *= dum; 
		for(k=0; k<=m; k++) B[row+k*n] *= dum;
		for ( i = 0, pA = A; i < row; pA += n, i++) {
			dum = *(pA + row);
			for ( j = row + 1; j < n; j++) *(pA + j) -= dum * *(pa + j);
			for(k=0; k<=m; k++) 
				B[i+k*n] -= dum * B[row+k*n];
			if(m_bCancel) break;
		}
		if(m_bCancel) break;
		SetProgress((int)(TaskSize/2), (double)(n-1-row)/(double)n);
	}
	m_Progress +=  (int)(TaskSize/2);
	return true;
}


void C3DPanelDlg::CheckSolution()
{
	//need to add wake contribution...
	CVector C,V,VS,VV;
	int p, pp, pw, lw;
	double phi, phiTot, phiS, phiD, sign;
	CVector QInf(m_QInf * cos(m_Alpha*pi/180.0), 0.0, m_QInf * sin(m_Alpha*pi/180.0));

	for (p=0; p<m_MatSize; p++)
	{
		if(m_pPanel[p].m_bIsTrailing)
		{
			C = m_pPanel[p].CollPt;
			
			phiS = 0.0; phiD = 0.0; phiTot = 0.0;
			VS.Set(0.0, 0.0, 0.0);

			for (pp=0; pp<m_MatSize; pp++)
			{
				SourceNASA4023(C, m_pPanel+pp, V, phi);
				phiS   += phi * m_Sigma[pp];
				VS     += V* m_Sigma[pp];

				DoubletNASA4023(C, m_pPanel+pp, V, phi);
				
				phiD   += phi * m_Mu[pp];
				phiTot += phiS+phiD;
				VS     +=V *m_Mu[pp];

				if(m_pPanel[pp].m_bIsTrailing)
				{
					if(m_pPanel[pp].m_iPos == -1) sign = -1.0; else sign  = 1.0;
					pw = m_pPanel[pp].m_iWake;
					for(lw=0; lw<m_pWPolar->m_NXWakePanels; lw++)
					{
						DoubletNASA4023(C, m_pWakePanel+pw+lw, V, phi, true);
						VS += V * m_Mu[pp] * sign;
					}	
				}
			}
//			TRACE("%3d    %12.6f    %12.6f\n", p, VS.dot(m_pPanel[p].Normal),-QInf.dot(m_pPanel[p].Normal));
		}
	}
}


void C3DPanelDlg::GetSpeedVector(CVector const &C, double *Mu, double *Sigma, CVector &VT)
{	
	CVector V;
	int pp, pw, lw;
	double phi, sign;
	CVector Vw[MAXSTATIONS];
	VT.Set(0.0,0.0,0.0);

	for (pp=0; pp<m_MatSize;pp++)
	{
		if(m_bCancel) return;

		if(m_pPanel[pp].m_iPos!=0) //otherwise Sigma[pp] =0.0, so contribution is zero also
		{
			GetSourceInfluence(C, m_pPanel+pp, V, phi);
			VT += V * Sigma[pp] ;
		}
		GetDoubletInfluence(C, m_pPanel+pp, V, phi);

		VT += V * Mu[pp];

		// Is the panel pp shedding a wake ?
		if(m_pPanel[pp].m_bIsTrailing)
		{
			//If so, we need to add the contribution of the wake column shedded by this panel
			if(m_pPanel[pp].m_iPos == -1) sign = -1.0; else sign  = 1.0;
			pw = m_pPanel[pp].m_iWake;
			for(lw=0; lw<m_pWPolar->m_NXWakePanels; lw++)
			{
				GetDoubletInfluence(C, m_pWakePanel+pw+lw, V, phi, true);
				VT += V * Mu[pp] * sign;
			}	
		}
	}
}


bool C3DPanelDlg::CreateMatrix()
{
	CVector C, CC, V, VS;
	int p, pp, Size;
	double phi, phiSym;

	AddString("    Creating the influence matrix...\r\n");
	if(m_b3DSymetric) Size = m_MatSize/2;
	else              Size = m_MatSize;

	for(p=0; p<Size; p++)
	{
		if(m_bCancel) return false;
		//for each collocation point
		C    = m_ppPanel[p]->CollPt;
		CC   = m_ppPanel[p]->CollPt;//symmetric point, just in case
		CC.y = -CC.y;

		for(pp=0; pp<Size; pp++)
		{
			if(m_bCancel) return false;
			//for each panel, get the unit doublet influence at the coll pt

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
		SetProgress(15, (double)p/(double)Size);
	}

	m_Progress += 15;
	return true;
}


bool C3DPanelDlg::CreateRHS(double V0, double VDelta, int nval)
{
	//NASA 4023 equation (20) & (22)
	int p, pp, q, m, Size;
	double alpha, phi, phiSym;
	CVector V, VS, C, CC;
	CVector QInf[100];

	if(m_b3DSymetric) Size = m_MatSize/2;
	else              Size = m_MatSize;

	//compute with a unit speed
	AddString("      Creating RHS vector...\r\n");

	p=0;
	for (q=0; q<nval;q++)
	{
		alpha = V0+q*VDelta;
		QInf[q].Set(cos(alpha*pi/180.0), 0.0, sin(alpha*pi/180.0));

		for (pp=0; pp< m_MatSize; pp++)
		{
			if(m_bCancel) return false;
			if(m_ppPanel[pp]->m_iPos==0) m_Sigma[p] =  0.0;
			else                         m_Sigma[p] = -1.0/4.0/pi* QInf[q].dot(m_ppPanel[pp]->Normal);
			p++;
		}
		SetProgress(1*nval, (double)q/(double)nval);
	}
	m_Progress += 1 * nval;

	m = 0;
	for (p=0; p<Size; p++)
	{
		if(m_bCancel) return false;

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

		C.x    =  m_ppPanel[p]->CollPt.x; 
		C.y    =  m_ppPanel[p]->CollPt.y; 
		C.z    =  m_ppPanel[p]->CollPt.z; 
		CC.x   =  m_ppPanel[p]->CollPt.x; //symetric point, just in case
		CC.y   = -m_ppPanel[p]->CollPt.y; 
		CC.z   =  m_ppPanel[p]->CollPt.z; 

		for (pp=0; pp<Size; pp++)
		{
			if(m_ppPanel[pp]->m_iPos!=0) GetSourceInfluence(C, *(m_ppPanel+pp), V, phi);
			else
			{
				//sigma is zero on a thin surface
				V.Set(0.0, 0.0, 0.0);
				phi = 0.0;
			}

			if(!m_bDirichlet || m_ppPanel[p]->m_iPos==0) 
			{
				m_cosRHS[m] -= V.dot(m_ppPanel[p]->Normal) * m_ppPanel[pp]->Normal.x * -1.0/4.0/pi;
				m_sinRHS[m] -= V.dot(m_ppPanel[p]->Normal) * m_ppPanel[pp]->Normal.z * -1.0/4.0/pi;
			}
			else if(m_bDirichlet)
			{
				m_cosRHS[m] -= phi * m_ppPanel[pp]->Normal.x * -1.0/4.0/pi;
				m_sinRHS[m] -= phi * m_ppPanel[pp]->Normal.z * -1.0/4.0/pi;
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
					m_cosRHS[m] -= VS.dot(m_ppPanel[p]->Normal) * m_ppPanel[pp]->Normal.x;
					m_sinRHS[m] -= VS.dot(m_ppPanel[p]->Normal) * m_ppPanel[pp]->Normal.z;
				}
				else if(m_bDirichlet)
				{
					m_cosRHS[m] -= phiSym * m_ppPanel[pp]->Normal.x * -1.0/4.0/pi;
					m_sinRHS[m] -= phiSym * m_ppPanel[pp]->Normal.z * -1.0/4.0/pi;
				}
			}
		}
		m++;
		SetProgress(9, (double)m/(double)(nval*Size));
	}

	m_Progress += 9 ;
	return true;
}

bool C3DPanelDlg::CreateWakeContribution()
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
	double Delta_phi_inf = 0.0;
	double PHC[MAXSTATIONS];
	CVector VHC[MAXSTATIONS];
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	AddString("      Adding the wake's contribution...\r\n");

	if(m_b3DSymetric)	Size = m_MatSize/2;
	else				Size = m_MatSize;

//	memcpy(m_RHS, m_RHSRef, nval      * m_MatSize * sizeof(double));
	memcpy(m_aij, m_aijRef, m_MatSize * m_MatSize * sizeof(double));

	for(p=0; p<Size; p++)//for each matrix row 
	{
		if(m_bCancel) return false;

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
				GetDoubletInfluence(C,  m_pWakePanel+pw, V, phi, true);
				PHC[kw] += phi;
				VHC[kw] += V;

				if(m_b3DSymetric && !m_pWakePanel[pw].m_bIsInSymPlane) // add right wing contribution
				{
					GetDoubletInfluence(CC,  m_pWakePanel+pw, VS, phiSym, true);
	
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
			if(m_bCancel) return false;

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

		SetProgress(2, (double)p/(double)Size);
	}
	m_Progress += 2;

//double row[VLMMATSIZE]; memcpy(row, m_aij, sizeof(row));
	return true;
}


bool C3DPanelDlg::SolveMultiple(double V0, double VDelta, int nval)
{
	//______________________________________________________________________________________
	// Method : 
	// 	- If the polar is of type 1 or 2, solve the linear system 
	//	- for cosine and sine parts, for a unit speed
	//	- If the polar is of type 4, solve only for unit speed and for the specified Alpha
	//	- Reconstruct right side results if calculation was symetric
	//	- Sort results i.a.w. panel numbering
	//______________________________________________________________________________________

	int Size, nrhs, q, o, p, nel;

	if(m_b3DSymetric) 
	{
		Size = m_MatSize/2;
	}
	else
	{
		Size = m_MatSize;
	}

	AddString("      Solving the linear system...\r\n");

	if(m_pWPolar->m_Type!=4) nrhs = nval;
	else                     nrhs = 0;

	memcpy(m_RHS,      m_cosRHS, Size * sizeof(double));
	memcpy(m_RHS+Size, m_sinRHS, Size * sizeof(double));


	if(!Gauss(m_aij, Size, m_RHS, 2, 30))
	{
		AddString("      Singular Matrix.... Aborting calculation...\r\n");
		m_bConverged = false;
		return false;
	}
	else m_bConverged = true;

	memcpy(m_cosRHS, m_RHS,      Size * sizeof(double));
	memcpy(m_sinRHS, m_RHS+Size, Size * sizeof(double));

	//______________________________________________________________________________________
	//Reconstruct right side results if calculation was symetric
	double *SigmaRef = m_aij;//use existing reserved memory, do not re-allocate

	if(m_b3DSymetric)
	{
		memcpy(m_RHSRef, m_RHS,   nval*Size*sizeof(double));
		memcpy(SigmaRef, m_Sigma, nval*Size*sizeof(double));

//		n  =  q    * Size;
//		o  =  q    * m_MatSize;
//		o1 = (q+1) * m_MatSize;
		//cosine
		for (p=0; p<m_MatSize/2; p++)
		{
			m_cosRHS[p]             = m_RHSRef[p];				
			m_cosRHS[m_MatSize-1-p] = m_RHSRef[p];
//				m_Sigma[o+p]    = SigmaRef[n+p];
//				m_Sigma[o1-1-p] = SigmaRef[n+p];
		}
		//sine
		for (p=0; p<m_MatSize/2; p++)
		{
			m_sinRHS[p]             = m_RHSRef[Size+p];
			m_sinRHS[m_MatSize-1-p] = m_RHSRef[Size+p];

//				m_Sigma[o+p]    = SigmaRef[n+p];
//				m_Sigma[o1-1-p] = SigmaRef[n+p];
		}
	}

//	reconstruct all results from cosine and sine unit vectors
	int m;
	double alpha, cosa, sina;
	m=0;
	for (q=0; q<nval;q++)
	{
		alpha = V0 + q * VDelta;
		cosa = cos(alpha*pi/180.0);
		sina = sin(alpha*pi/180.0);
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

//	CheckSolution();
//	return false;
	return true;
}


bool C3DPanelDlg::CreateDoubletStrength(double V0, double VDelta, int nval)
{
	//______________________________________________________________________________________
	// Method : 
	// 	First calculate the Cp coefficients
	// 	Deduce the lift for unit speed
	// 	Calculate the speeds i.a.w. the polar's type
	// 	Scale the doublet and source strength i.a.w. the speeds
	//______________________________________________________________________________________

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CMiarex *pMiarex   = (CMiarex*)m_pMiarex;

	CString strong, strange;
	int p, q, pp;
	double Lift, alpha;
	CVector PanelForce, WindNormal;

	//______________________________________________________________________________________
	
	AddString("      Computing On-Body Speeds...\r\n");

	if(m_pWPolar->m_Type !=4 )
	{
		for (q=0; q<nval; q++)
		{
			if(!ComputeOnBody(q, V0+q*VDelta)) return false;
			SetProgress(1*nval, (double)q/(double)nval);
		}
	}
	else
	{
		if(!ComputeOnBody(0, m_Alpha)) return false;
		for (q=1; q<nval; q++)
		{
			for (p=0; p<m_MatSize; p++)
			{
				m_Cp[p+q*m_MatSize] = m_Cp[p];
			}
			SetProgress(1*nval, (double)q/(double)nval);
		}
	}
	m_Progress += 1*nval ;

	//______________________________________________________________________________________
	// Scale the speeds i.a.w. the polar's type

	if(m_pWPolar->m_Type==2)
	{
		AddString("      Calculating speeds to balance the weight\r\n");

		for (q=0; q<nval;q++)
		{

			alpha = V0+q*VDelta;
			WindNormal.Set(-sin(alpha*pi/180.0), 0.0, cos(alpha*pi/180.0));
			Lift = 0.0;
			p=0;
			for (p=0; p<m_MatSize; p++)
			{
				// for each panel, add the lift coef
				PanelForce = m_pPanel[p].Normal * (-m_Cp[p+q*m_MatSize]) * m_pPanel[p].Area;
				Lift += PanelForce.dot(WindNormal);
			}

			if(Lift<=0.0)
			{
				strong.Format("      Found a negative lift for Alpha=%.2f.... skipping the angle...\r\n", V0+q*VDelta);
				AddString(strong);
				m_bPointOut = true;
				m_bWarning  = true;
				m_3DQInf[q] = -100.0;
			}
			else
			{
				m_3DQInf[q] =  sqrt(2.0* 9.81 * m_pWPolar->m_Weight/m_pWPolar->m_Density/Lift);
				strong.Format("      Alpha=%5.2f   QInf = %5.2f", V0+q*VDelta, m_3DQInf[q]*pFrame->m_mstoUnit);
				GetSpeedUnit(strange, pFrame->m_SpeedUnit);
				strong+= strange + "\r\n";
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
//	memcpy(m_RHSRef, m_RHS,   nval*m_MatSize*sizeof(double));
//	memcpy(SigmaRef, m_Sigma, nval*m_MatSize*sizeof(double));
	
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

	return true;
}


bool C3DPanelDlg::ComputeAeroCoefs(double V0, double VDelta, int nrhs)
{
	// calculates the various wing coefficients by interpolating
	// the adequate variable, from Cl, on the XFoil polar mesh
	// at each span station
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	int q;
	CString str, strong;

	if(m_pWPolar->m_Type !=4)
	{
		for (q=0; q<nrhs; q++)
		{
			if(m_bCancel) break;
			if(!m_pWPolar->m_bTiltedGeom)	str.Format("      Computing Plane for alpha=%.2f\r\n",V0+q*VDelta);
			else							str.Format("      Computing Plane for alpha=%.2f\r\n",m_OpAlpha);
			AddString(str);
			ComputePlane(V0+q*VDelta, q);
			SetProgress(5*nrhs,(double)(q)/(double)nrhs);
		}
	}
	else
	{
		for (q=0; q<nrhs; q++)
		{
			if(m_bCancel) break;
			GetSpeedUnit(strong, pFrame->m_SpeedUnit);
			str.Format("      Computing Plane for QInf=%.2f",(V0+q*VDelta)*pFrame->m_mstoUnit);
			str += strong+"\r\n";
			AddString(str);
			ComputePlane(m_Alpha, q);
			SetProgress(5*nrhs,(double)(q)/(double)nrhs);
		}
	}

	m_Progress += 5*nrhs;
	SetProgress(5*nrhs,1.0);

	return true;
}


void C3DPanelDlg::SumPanelForces(double *Cp, double Alpha, double Qinf, double &Lift, double &Drag)
{
	int p;
	CVector PanelForce;
	
	for(p=0; p<m_MatSize; p++)
	{
		PanelForce += m_pPanel[p].Normal * (-Cp[p]) * m_pPanel[p].Area;
	}
	Lift = PanelForce.z * cos(Alpha*pi/180.0) - PanelForce.x * sin(Alpha*pi/180.0);
	Drag = PanelForce.x * cos(Alpha*pi/180.0) + PanelForce.z * sin(Alpha*pi/180.0);
}


bool C3DPanelDlg::ComputePlane(double Alpha, int qrhs)
{
	// calculates the various wing coefficients by interpolating
	// the adequate variable, from Cl, on the XFoil polar mesh
	// at each span station
	int pos, Station;
	double *Mu, *Sigma;
	double Lift, IDrag, VDrag ,XCP, YCP, QInf, WingLift, WingIDrag;;
//	double GRm, GYm, GCm, VCm, VYm, IYm; 
	CString str;

	bool bThinSurf = m_pWPolar->m_bThinSurfaces;

	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	
	Mu     = m_Mu    + qrhs*m_MatSize;
	Sigma  = m_Sigma + qrhs*m_MatSize;
	QInf        = m_3DQInf[qrhs];
	m_QInf      = m_3DQInf[qrhs];
	if(m_pWPolar->m_bTiltedGeom)	Alpha = m_OpAlpha;
	else							m_OpAlpha = Alpha;

	m_pWing->m_bTrace		= true;
	m_pWing->m_bWingOut		= false;

	if(m_pWing2) 
	{
		m_pWing2->m_bTrace		= true;
		m_pWing2->m_bWingOut	= false;
	}
	if(m_pStab) 
	{
		m_pStab->m_bTrace		= true;
		m_pStab->m_bWingOut		= false;
	}
	if(m_pFin)
	{
		m_pFin->m_bTrace		= true;
		m_pFin->m_bWingOut		= false;
	}

	if(QInf >0.0) 
	{
		SetAi(qrhs);

		AddString("       Calculating aerodynamic coefficients...\r\n");
		m_bPointOut          = false;
		m_pWing->m_Alpha     = Alpha;
		m_pWing->m_QInf      = QInf;
		m_pWing->m_Viscosity = m_pWPolar->m_Viscosity;
		m_pWing->m_Density   = m_pWPolar->m_Density;

		Lift   = 0.0;
		IDrag  = 0.0;
		VDrag  = 0.0;
		XCP = YCP = 0.0;

		m_GCm = m_GRm = m_GYm  = 0.0;
		m_VCm = m_VYm = m_IYm  = 0.0;

		AddString("       Calculating wing...\r\n");
		m_pWing->PanelTrefftz(m_Cp+qrhs*m_MatSize, Mu, Sigma, 0, Lift, IDrag, m_pWPolar->m_bTiltedGeom, bThinSurf, m_pWakePanel, m_pWakeNode);
		m_pWing->PanelComputeWing(m_Cp+qrhs*m_MatSize,
								  VDrag, XCP, YCP, 
								  m_GCm, m_GRm, m_GYm, m_VCm, m_VYm, m_IYm, m_pWPolar->m_bViscous, bThinSurf, m_pWPolar->m_bTiltedGeom);
		m_pWing->PanelSetBending();

		pos = m_pWing->m_MatSize;

		if(m_pWing->m_bWingOut)  m_bPointOut = true;

		Station = m_pWing->m_NStation;
		
		if(m_pWing2) 
		{
			AddString("       Calculating elevator...\r\n");
			WingLift  = 0.0;
			WingIDrag = 0.0;
			m_pWing2->m_Alpha     = Alpha;
			m_pWing2->m_QInf      = QInf;
			m_pWing2->m_Viscosity = m_pWPolar->m_Viscosity;
			m_pWing2->m_Density   = m_pWPolar->m_Density;
			m_pWing2->PanelTrefftz(m_Cp+qrhs*m_MatSize+pos, Mu, Sigma, pos, Lift, IDrag, m_pWPolar->m_bTiltedGeom,bThinSurf,m_pWakePanel, m_pWakeNode);
			m_pWing2->PanelComputeWing(m_Cp+qrhs*m_MatSize+pos,
									  VDrag, XCP, YCP, 
									  m_GCm, m_GRm, m_GYm, m_VCm, m_VYm, m_IYm, m_pWPolar->m_bViscous, bThinSurf, m_pWPolar->m_bTiltedGeom);
			m_pWing2->PanelSetBending();
			pos += m_pWing2->m_MatSize;
			Lift  += WingLift;
			IDrag += WingIDrag;

			m_pStab->VLMSetBending();
			if(m_pStab->m_bWingOut) m_bPointOut = true;

			Station += m_pStab->m_NStation;
		}

		if(m_pStab) 
		{
			AddString("       Calculating elevator...\r\n");
			WingLift  = 0.0;
			WingIDrag = 0.0;
			m_pStab->m_Alpha     = Alpha;
			m_pStab->m_QInf      = QInf;
			m_pStab->m_Viscosity = m_pWPolar->m_Viscosity;
			m_pStab->m_Density   = m_pWPolar->m_Density;
			m_pStab->PanelTrefftz(m_Cp+qrhs*m_MatSize+pos, Mu, Sigma, pos, Lift, IDrag, m_pWPolar->m_bTiltedGeom, bThinSurf, m_pWakePanel, m_pWakeNode);
			m_pStab->PanelComputeWing(m_Cp+qrhs*m_MatSize+pos,
									  VDrag, XCP, YCP, 
									  m_GCm, m_GRm, m_GYm, m_VCm, m_VYm, m_IYm, m_pWPolar->m_bViscous, bThinSurf, m_pWPolar->m_bTiltedGeom);
			m_pStab->PanelSetBending();

			pos += m_pStab->m_MatSize;
			Lift  += WingLift;
			IDrag += WingIDrag;

			m_pStab->VLMSetBending();
			if(m_pStab->m_bWingOut) m_bPointOut = true;

			Station += m_pStab->m_NStation;
		}

		if(m_pFin)
		{
			AddString("       Calculating fin...\r\n");
			WingLift  = 0.0;
			WingIDrag = 0.0;
			m_pFin->m_Alpha      = Alpha;
			m_pFin->m_QInf       = QInf;
			m_pFin->m_Viscosity  = m_pWPolar->m_Viscosity;
			m_pFin->m_Density    = m_pWPolar->m_Density;
			
			m_pFin->PanelTrefftz(m_Cp+qrhs*m_MatSize+pos, Mu, Sigma, pos, Lift, IDrag, m_pWPolar->m_bTiltedGeom, bThinSurf, m_pWakePanel, m_pWakeNode);
			m_pFin->PanelComputeWing(m_Cp+qrhs*m_MatSize+pos,
									  VDrag, XCP, YCP, 
									  m_GCm, m_GRm, m_GYm, m_VCm, m_VYm, m_IYm, m_pWPolar->m_bViscous, bThinSurf, m_pWPolar->m_bTiltedGeom);
			m_pFin->PanelSetBending();
			pos += m_pFin->m_MatSize;

			if(m_pFin->m_bWingOut)  m_bPointOut = true;

			Lift  += WingLift;
			IDrag += WingIDrag;

			m_pFin->VLMSetBending();
		}

		if(*m_ppBody)
		{
			AddString("       Calculating body...\r\n");
			WingLift  = 0.0;
			WingIDrag = 0.0;

			(*m_ppBody)->ComputeAero(m_Cp+qrhs*m_MatSize+pos, WingLift, XCP, YCP,
				m_GCm, m_GRm, m_GYm, Alpha, m_pWPolar->m_XCmRef, m_pWPolar->m_bTiltedGeom);

			//the body does not shed any wake--> no induced lift or drag
//			Lift  += WingLift;
//			IDrag += WingIDrag;
		}
	
		if(!m_bTrefftz)
		{
			SumPanelForces(m_Cp+qrhs*m_MatSize, Alpha, QInf, Lift, IDrag);
		}

		m_CL          =       Lift/m_pWing->m_Area;
		m_InducedDrag =  1.0*IDrag/m_pWing->m_Area;
		m_ViscousDrag =  1.0*VDrag/m_pWing->m_Area;

		m_XCP         = XCP/Lift;
		m_YCP         = YCP/Lift;

		m_GCm *= 1.0 / m_pWing->m_Area /m_pWing->m_MAChord;
		m_GRm *= 1.0 / m_pWing->m_Area /m_pWing->m_Span;
		m_GYm *= 1.0 / m_pWing->m_Area /m_pWing->m_Span;

		m_VCm *= 1.0 / m_pWing->m_Area /m_pWing->m_MAChord;
		m_VYm *= 1.0 / m_pWing->m_Area /m_pWing->m_Span;

		m_IYm *= 1.0 / m_pWing->m_Area /m_pWing->m_Span;

		if(m_bPointOut) m_bWarning = true;

		if(m_pPlane)
		{
			if(!m_bCancel) pMiarex->AddPOpp(m_bPointOut, m_Cp+qrhs*m_MatSize, Mu, Sigma);
		}
		else
		{
			if(!m_bCancel) pMiarex->AddWOpp(m_bPointOut, Mu, Sigma, m_Cp+qrhs*m_MatSize);		
		}

		AddString("\r\n");
	}
	else m_bPointOut = true;

	return true;
}

void C3DPanelDlg::RelaxWake()
{
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	CVector V, VL, VT;
	int mw, kw, lw, llw;
	int nInter;
	double t, dx, dx0;
	double damping = 1.0;
	double *Mu    = m_Mu   ;
	double *Sigma = m_Sigma;

	//Since the wake roll-up is performed on the tilted geometry,
	// we define a speed vector parallel to the x-axis
	CVector QInf(m_QInf, 0.0, 0.0);

	// Andre's method : fit the wake panels on the streamlines
	// we have the computing power to do it

	CVector LATB, TALB, Trans, PP;
	CVector WLA, WLB,WTA,WTB, WTemp;//wake panel's leading corner points

	dx0 = 0.05;

	AddString("      Relaxing the wake...\r\n");

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
		SetProgress(20, (double)lw/(double)m_pWPolar->m_NXWakePanels);
	}
	m_Progress +=20;

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
}

void C3DPanelDlg::GetDoubletInfluence(CVector const &TestPt, CPanel *pPanel, CVector &V, double &phi, bool bWake)
{
	DoubletNASA4023(TestPt, pPanel, V, phi, bWake);

	if(m_pWPolar->m_bGround) 
	{
		CG.Set(TestPt.x, TestPt.y, -TestPt.z-2.0*m_pWPolar->m_Height);
		DoubletNASA4023(CG, pPanel, VG, phiG, bWake);
		V.x += VG.x;
		V.y += VG.y;
		V.z -= VG.z;
		phi += phiG;
	}
}

void C3DPanelDlg::GetSourceInfluence(CVector const &TestPt, CPanel *pPanel, CVector &V, double &phi)
{
	SourceNASA4023(TestPt, pPanel, V, phi);

	if(m_pWPolar->m_bGround) 
	{
		CG.Set(TestPt.x, TestPt.y, -TestPt.z-2.0*m_pWPolar->m_Height);
		SourceNASA4023(CG, pPanel, VG, phiG);
		V.x += VG.x;
		V.y += VG.y;
		V.z -= VG.z;
		phi += phiG;
	}
}

void C3DPanelDlg::DoubletNASA4023(CVector const &C, CPanel *pPanel, CVector &V, double &phi, bool bWake)
{
	// VSAERO theory Manual
	// Influence of panel pp at coll pt of panel p
	// vectorial operations are written inline to save computing times
	// -->longer code, but 4x more efficient....
	int i;
	double CoreSize = 0.000001;
	if(abs(*m_pCoreSize)>1.e-10) CoreSize = *m_pCoreSize;
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
		R[0].x = pNode[pPanel->m_iLA].x;
		R[0].y = pNode[pPanel->m_iLA].y;
		R[0].z = pNode[pPanel->m_iLA].z;
		R[1].x = pNode[pPanel->m_iTA].x;
		R[1].y = pNode[pPanel->m_iTA].y;
		R[1].z = pNode[pPanel->m_iTA].z;
		R[2].x = pNode[pPanel->m_iTB].x;
		R[2].y = pNode[pPanel->m_iTB].y;
		R[2].z = pNode[pPanel->m_iTB].z;
		R[3].x = pNode[pPanel->m_iLB].x;
		R[3].y = pNode[pPanel->m_iLB].y;
		R[3].z = pNode[pPanel->m_iLB].z;
		R[4].x = pNode[pPanel->m_iLA].x;	
		R[4].y = pNode[pPanel->m_iLA].y;	
		R[4].z = pNode[pPanel->m_iLA].z;	
	}
	else
	{
		R[0].x = pNode[pPanel->m_iLB].x;
		R[0].y = pNode[pPanel->m_iLB].y;
		R[0].z = pNode[pPanel->m_iLB].z;
		R[1].x = pNode[pPanel->m_iTB].x;
		R[1].y = pNode[pPanel->m_iTB].y;
		R[1].z = pNode[pPanel->m_iTB].z;
		R[2].x = pNode[pPanel->m_iTA].x;
		R[2].y = pNode[pPanel->m_iTA].y;
		R[2].z = pNode[pPanel->m_iTA].z;
		R[3].x = pNode[pPanel->m_iLA].x;
		R[3].y = pNode[pPanel->m_iLA].y;
		R[3].z = pNode[pPanel->m_iLA].z;
		R[4].x = pNode[pPanel->m_iLB].x;	
		R[4].y = pNode[pPanel->m_iLB].y;	
		R[4].z = pNode[pPanel->m_iLB].z;	
	}

	for (i=0; i<4; i++)
	{
		a.x  = C.x - R[i].x;
		a.y  = C.y - R[i].y;
		a.z  = C.z - R[i].z;
		b.x  = C.x - R[i+1].x;
		b.y  = C.y - R[i+1].y;
		b.z  = C.z - R[i+1].z;
		s.x  = R[i+1].x - R[i].x;
		s.y  = R[i+1].y - R[i].y;
		s.z  = R[i+1].z - R[i].z;
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
		if(R[i].IsSame(R[i+1])) 
		{
			CJKi = 0.0;
			//no contribution to speed either
		}
		else if ((h.x*h.x+h.y*h.y+h.z*h.z)/(s.x*s.x+s.y*s.y+s.z*s.z) <= CoreSize*CoreSize
				  && a.x*s.x+a.y*s.y+a.z*s.z>=0.0
				  && b.x*s.x+b.y*s.y+b.z*s.z<=0.0) 
		{
			CJKi = 0.0;//speed is singular on panel edge, value of potential is unknown
		}
		else
		{
			RNUM = SM*PN * (B*PA-A*PB);
			DNOM = PA*PB + PN*PN*A*B*SM*SM;
			if(abs(PN)<eps)
			{
				// side is >0 if on the panel's right side
				side = pPanel->Normal.x*h.x +pPanel->Normal.y*h.y +pPanel->Normal.z*h.z;
					   
				if(side >=0.0) sign = 1.0; else sign = -1.0;
				if(DNOM<0.0){
					if(PN>0.0)	CJKi =  pi * sign;
					else		CJKi = -pi * sign;
				}
				else if(DNOM == 0.0){
					if(PN>0.0)	CJKi =  pi/2.0 * sign;
					else		CJKi = -pi/2.0 * sign;
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
//		if(R[0].IsSame(R[1]) || R[1].IsSame(R[2]) || R[2].IsSame(R[3]) || R[3].IsSame(R[0]))
//			phi = -3.0*pi/2.0;
//		else 
			phi  = -2.0*pi;	
	}
}

void C3DPanelDlg::SourceNASA4023(CVector const &C, CPanel *pPanel, CVector &V, double &phi)
{
	//VSAERO theory Manual
	//Influence of panel pp at coll pt of panel p
	//vectorial operations are written inline to save computing times
	//-->longer code, but 4x more efficient....
	int i;
	
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
		R[0].x = m_pNode[pPanel->m_iLA].x;
		R[0].y = m_pNode[pPanel->m_iLA].y;
		R[0].z = m_pNode[pPanel->m_iLA].z;
		R[1].x = m_pNode[pPanel->m_iTA].x;
		R[1].y = m_pNode[pPanel->m_iTA].y;
		R[1].z = m_pNode[pPanel->m_iTA].z;
		R[2].x = m_pNode[pPanel->m_iTB].x;
		R[2].y = m_pNode[pPanel->m_iTB].y;
		R[2].z = m_pNode[pPanel->m_iTB].z;
		R[3].x = m_pNode[pPanel->m_iLB].x;
		R[3].y = m_pNode[pPanel->m_iLB].y;
		R[3].z = m_pNode[pPanel->m_iLB].z;
		R[4].x = m_pNode[pPanel->m_iLA].x;	
		R[4].y = m_pNode[pPanel->m_iLA].y;	
		R[4].z = m_pNode[pPanel->m_iLA].z;	
	}
	else{
		R[0].x = m_pNode[pPanel->m_iLB].x;
		R[0].y = m_pNode[pPanel->m_iLB].y;
		R[0].z = m_pNode[pPanel->m_iLB].z;
		R[1].x = m_pNode[pPanel->m_iTB].x;
		R[1].y = m_pNode[pPanel->m_iTB].y;
		R[1].z = m_pNode[pPanel->m_iTB].z;
		R[2].x = m_pNode[pPanel->m_iTA].x;
		R[2].y = m_pNode[pPanel->m_iTA].y;
		R[2].z = m_pNode[pPanel->m_iTA].z;
		R[3].x = m_pNode[pPanel->m_iLA].x;
		R[3].y = m_pNode[pPanel->m_iLA].y;
		R[3].z = m_pNode[pPanel->m_iLA].z;
		R[4].x = m_pNode[pPanel->m_iLB].x;	
		R[4].y = m_pNode[pPanel->m_iLB].y;	
		R[4].z = m_pNode[pPanel->m_iLB].z;	
	}

	for (i=0; i<4; i++)
	{
		a.x  = C.x - R[i].x;
		a.y  = C.y - R[i].y;
		a.z  = C.z - R[i].z;

		b.x  = C.x - R[i+1].x;
		b.y  = C.y - R[i+1].y;
		b.z  = C.z - R[i+1].z;

		s.x  = R[i+1].x - R[i].x;
		s.y  = R[i+1].y - R[i].y;
		s.z  = R[i+1].z - R[i].z;

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

			if(abs(PN)<eps){
				// side is >0 if on the panel's right side
				side = pPanel->Normal.x*h.x + pPanel->Normal.y*h.y + pPanel->Normal.z*h.z;
				if(side >=0.0) sign = 1.0; else sign = -1.0;
				if(DNOM<0.0){
					if(PN>0.0)	CJKi =  pi * sign;
					else		CJKi = -pi * sign;
				}
				else if(DNOM == 0.0){
					if(PN>0.0)	CJKi =  pi/2.0 * sign;
					else		CJKi = -pi/2.0 * sign;
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
void C3DPanelDlg::SourceNASA4023(CVector TestPt, CPanel *pPanel, CVector &V, double &phi)
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

			if(abs(PN)<eps){
				side = pPanel->Normal.dot(a * s); // Positive if on the panel's right side
				if(side >=0.0) sign = 1.0; else sign = -1.0;
				if(DNOM<0.0){
					if(PN>0.0)	CJKi =  pi * sign;
					else		CJKi = -pi * sign;
				}
				else if(DNOM == 0.0){
					if(PN>0.0)	CJKi =  pi/2.0 * sign;
					else		CJKi = -pi/2.0 * sign;
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
void C3DPanelDlg::DoubletNASA4023(CVector TestPt, CPanel *pPanel, CVector &V, double &phi, bool bWake)
{
	//VSAERO theory Manual
	//Influence of panel pp at coll pt of panel p
	int i;
	double CoreSize = 0.000001;
	if(abs(*m_pCoreSize)>1.e-10) CoreSize = *m_pCoreSize;
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

	if(pPanel->m_iPos>=0){
		R[0] = pNode[pPanel->m_iLA];
		R[1] = pNode[pPanel->m_iTA];
		R[2] = pNode[pPanel->m_iTB];
		R[3] = pNode[pPanel->m_iLB];
		R[4] = pNode[pPanel->m_iLA];	
	}
	else{
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
			if(abs(PN)<eps)
			{
				side = pPanel->Normal.dot(a * s); // Positive if on the panel's right side
				if(side >=0.0) sign = 1.0; else sign = -1.0;
				if(DNOM<0.0){
					if(PN>0.0)	CJKi =  pi * sign;
					else		CJKi = -pi * sign;
				}
				else if(DNOM == 0.0){
					if(PN>0.0)	CJKi =  pi/2.0 * sign;
					else		CJKi = -pi/2.0 * sign;
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
	if(TestPt == pPanel->CollPt){
//		if(R[0].IsSame(R[1]) || R[1].IsSame(R[2]) || R[2].IsSame(R[3]) || R[3].IsSame(R[0]))
//			phi = -3.0*pi/2.0;
//		else 
			phi  = -2.0*pi;	
	}
}
*/


bool C3DPanelDlg::ComputeOnBody(int qrhs, double Alpha)
{	
	//following VSAERO theory manual
	//the on-body tangential perturbation speed is the derivative of the doublet strength
	int p;
	int PL,PR, PU, PD;
	double DELQ, DELP;
	CVector Vl, QInfl;//local panel speed
	CVector S2, Sl2;
	double mu0,mu1,mu2;
	double x0,x1,x2;
	double Speed2;
	double *Mu, * Sigma, *Cp;

	Mu     = m_Mu    + qrhs * m_MatSize;
	Sigma  = m_Sigma + qrhs * m_MatSize;
	Cp     = m_Cp    + qrhs * m_MatSize;

	CVector Qp(cos(Alpha*pi/180.0), 0.0, sin(Alpha*pi/180.0));

	for (p=0; p<m_MatSize; p++)
	{
		if(m_bCancel) return false;

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
			else ASSERT(FALSE);
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
			else ASSERT(FALSE);
		}
		else {
			DELQ = 0.0;
//			ASSERT(FALSE);
		}

		if(PU>=0 && PD>=0){
			//we have two upstream and downstream neighbours
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
		else if(PU>=0 && PD<0){
			// no downstream neighbour
			// do we have two upstream neighbours ?
			if(m_pPanel[PU].m_iPU>=0){
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
			else ASSERT(FALSE);
		}
		else if(PU<0 && PD>=0){
			// no upstream neighbour
			// do we have two downstream neighbours ?
			if(m_pPanel[PD].m_iPD>=0){
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
			else ASSERT(FALSE);
		}
		else {
			DELP = 0.0;
//			ASSERT(FALSE);
		}

		//find middle of side 2
		S2 = (m_pNode[m_pPanel[p].m_iTA] + m_pNode[m_pPanel[p].m_iTB])/2.0 - m_pPanel[p].CollPt;
		//convert to local coordinates
		Sl2   = m_pPanel[p].GlobalToLocal(S2);
		QInfl = m_pPanel[p].GlobalToLocal(Qp);
		//in panel referential
		Vl.x = -4.0*pi*(m_pPanel[p].SMP*DELP - Sl2.y*DELQ)/Sl2.x;
		Vl.y = -4.0*pi*DELQ;
		Vl.z =  4.0*pi*Sigma[p];

		QInfl +=Vl;

		Speed2 = QInfl.x*QInfl.x + QInfl.y*QInfl.y + QInfl.z*QInfl.z;

		Cp[p]  = 1.0-Speed2;
//		Cp[p]  = m_Mu[p];
		m_Speed[p] = m_pPanel[p].LocalToGlobal(QInfl) * m_pWPolar->m_QInf;
	}
	return true;
}


bool C3DPanelDlg::ComputeSurfSpeeds(double *Mu, double *Sigma)
{
	int p;
	CVector C;
	CVector Q(m_3DQInf[0]*cos(m_Alpha*pi/180.0),0.0,m_3DQInf[0]*sin(m_Alpha*pi/180.0));

	for (p=0; p<m_MatSize; p++)
	{
		if(m_bCancel) return false;
		C = m_pPanel[p].CollPt;//+ m_pPanel[p].Normal*m_pPanel[p].Size/100.0;
		C += m_pPanel[p].Normal*0.001;

		GetSpeedVector(C, Mu, Sigma, m_Speed[p]);
		m_Speed[p] += Q;

	}
	return true;
}


void C3DPanelDlg::SetDownwash(double *Mu, double *Sigma)
{
	// calculates the downwash from the doublet and source strengths
	int m, p;
	CVector C;
	CVector X(0.001, 0.0, 0.0);

	//wing first
	memset(m_pWing->m_Vd, 0, sizeof(m_pWing->m_Vd));

	m=0;
	for (p=0; p< m_pWing->m_MatSize; p++)
	{
		if(m_pWing->m_pPanel[p].m_bIsTrailing)
		{
			C = (m_pNode[m_pWing->m_pPanel[p].m_iTA] + m_pNode[m_pWing->m_pPanel[p].m_iTB])/2.0;
			GetSpeedVector(C, Mu, Sigma, m_pWing->m_Vd[m]);
			m++;
		}
	}

	m=0;
	for (p=0; p< m_pWing2->m_MatSize; p++)
	{
		if(m_pWing2->m_pPanel[p].m_bIsTrailing)
		{
			C = (m_pNode[m_pWing2->m_pPanel[p].m_iTA] + m_pNode[m_pWing2->m_pPanel[p].m_iTB])/2.0;
			GetSpeedVector(C, Mu, Sigma, m_pWing2->m_Vd[m]);
			m++;
		}
	}

	m=0;
	for (p=0; p< m_pStab->m_MatSize; p++)
	{
		if(m_pStab->m_pPanel[p].m_bIsTrailing)
		{
			C = (m_pNode[m_pStab->m_pPanel[p].m_iTA] + m_pNode[m_pStab->m_pPanel[p].m_iTB])/2.0;
			GetSpeedVector(C, Mu, Sigma, m_pStab->m_Vd[m]);
			m++;
		}
	}

	m=0;
	for (p=0; p< m_pFin->m_MatSize; p++)
	{
		if(m_pFin->m_pPanel[p].m_bIsTrailing)
		{
			C = (m_pNode[m_pFin->m_pPanel[p].m_iTA] + m_pNode[m_pFin->m_pPanel[p].m_iTB])/2.0;
			GetSpeedVector(C, Mu, Sigma, m_pFin->m_Vd[m]);
			m++;
		}
	}
}


void C3DPanelDlg::SetAi(int qrhs)
{
	// calculates the induced angles from the doublet and source strengths
	CVector C, V, TA, TB;
	int kw, m, mw;
	double *Mu, *Sigma;
	double Ai[MAXSTATIONS];

	Mu    = m_Mu    + qrhs * m_MatSize;
	Sigma = m_Sigma + qrhs * m_MatSize;
	m=0;

	AddString("       Calculating induced angles...\r\n");
	mw = 0;

	for (kw=0; kw<m_NWakeColumn; kw++)
	{
		TA = m_pWakeNode[m_pWakePanel[mw + m_pWPolar->m_NXWakePanels-1].m_iTA];
		TB = m_pWakeNode[m_pWakePanel[mw + m_pWPolar->m_NXWakePanels-1].m_iTB];
		C = (TA+TB)/2.0;

		GetSpeedVector(C, Mu, Sigma, V);
		Ai[m] = atan2(V.z, m_3DQInf[qrhs]) * 180.0/pi;
		m_pWing->m_Vd[m] = V;

		mw += m_pWPolar->m_NXWakePanels;
		m++;
	}
	int pos = 0;
	for (m=0; m<m_pWing->m_NStation; m++)	    m_pWing->m_Ai[m] = Ai[m];
	pos += m_pWing->m_NStation;
	if(m_pWing2) {
		for (m=0; m<m_pWing2->m_NStation; m++)	m_pWing2->m_Ai[m] = Ai[m+pos];
		pos += m_pWing2->m_NStation;
	}
	if(m_pStab) {
		for (m=0; m<m_pStab->m_NStation; m++)	m_pStab->m_Ai[m] = Ai[m+pos];
		pos += m_pStab->m_NStation;
	}
	if(m_pFin) {
		for (m=0; m<m_pFin->m_NStation; m++)	m_pFin->m_Ai[m] = Ai[m+pos];
	}
}


/*
void C3DPanelDlg::RelaxWake()
{
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
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

	AddString("      Relaxing the wake...\r\n");

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


void C3DPanelDlg::VLMQmn(CVector LA, CVector LB, CVector TA, CVector TB, CVector C, CVector &V)
{
	// Quadrilateral ring VLM FORMULATION
	// Calculates the influence at point C of the vortex ring defined by the points LA, LB, TA, TB
	//
	//    LA__________LB               |
	//    |           |                |
	//    |           |                | freestream speed
	//    |           |                |
	//    |           |                \/
	//    |           |
	//    TA__________TB
	//
	// V is the resulting speed
	//

	V.Set(0.0, 0.0, 0.0);

	//Leading finite vortex contribution
	r0 = LB - LA;
	r1 = C  - LA;
	r2 = C  - LB;
	Psi = r1*r2;
	ftmp = Psi.VAbs();

	t = r1 * r0;
	if(t.VAbs()/r0.VAbs() > *m_pCoreSize)
	{
		Psi = Psi/ftmp/ftmp;
		Omega = r0.dot(r1)/r1.VAbs() - r0.dot(r2)/r2.VAbs();
		V += Psi * Omega/4.0/pi;
	}
		
	//Trailing finite vortex contribution
	r0 = TA - TB;
	r1 = C  - TB;
	r2 = C  - TA;
	Psi = r1*r2;
	ftmp = Psi.VAbs();

	t = r1 * r0;
	if(t.VAbs()/r0.VAbs() > *m_pCoreSize)
	{
		Psi = Psi/ftmp/ftmp;
		Omega = r0.dot(r1)/r1.VAbs() - r0.dot(r2)/r2.VAbs();
		V += Psi * Omega/4.0/pi;
	}

	//Next add 'right' finite contribution
	r0 = TB - LB;
	r1 = C  - LB;
	r2 = C  - TB;

	Psi = r1*r2;
	ftmp = Psi.VAbs();

	t = r1 * r0;
	if(t.VAbs()/r0.VAbs() > *m_pCoreSize)
	{
		Psi = Psi/ftmp/ftmp;
		Omega = r0.dot(r1)/r1.VAbs() - r0.dot(r2)/r2.VAbs();
		V += Psi * Omega/4.0/pi;
	}

	//Last add 'left' finite contribution
	r0 = LA - TA;
	r1 = C  - TA;
	r2 = C  - LA;

	Psi = r1*r2;
	ftmp = Psi.VAbs();

	t = r1 * r0;
	if(t.VAbs()/r0.VAbs() > *m_pCoreSize)
	{
		Psi = Psi/ftmp/ftmp;
		Omega = r0.dot(r1)/r1.VAbs() - r0.dot(r2)/r2.VAbs();
		V += Psi * Omega/4.0/pi;
	}
}


























