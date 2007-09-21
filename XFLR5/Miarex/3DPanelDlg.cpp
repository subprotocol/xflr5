/****************************************************************************

    C3DPanelDlg Class
	Copyright (C) 2007 André Deperrois xflr5@yahoo.com

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
	RFF = 10;
	eps = 1.e-7;

	m_bSequence      = false;
	m_bWarning       = false;
	m_bType4         = false;
	m_bXFile         = false;
	m_bPlaneAnalysis = false;
	m_b3DSymetric    = true;
	m_bConverged     = false;
	m_bNeumann       = false;

	m_QInf       = 0.0;//Speed vector in m/s
	m_Alpha      = 0.0;//Angle of Attack in °
	m_AlphaMax   = 0.0;
	m_DeltaAlpha = 0.0;
	
	m_MatSize   = 0;
	m_nNodes    = 0;
	m_NSurfaces = 0;
	m_NWakeColumn  = 0;
	m_NXWakePanels = 0;

	m_strOut = "";

	m_pWing   = NULL;//pointer to the geometry class of the wing 
	m_pWing2  = NULL;
	m_pStab   = NULL;
	m_pFin    = NULL;
	m_pPlane  = NULL;
	m_pMiarex = NULL;
	m_pFrame  = NULL;
	m_pPanelThread = NULL;

	memset(m_aij,   0, sizeof(m_aij));
	memset(m_RHS,   0, sizeof(m_RHS));
	memset(m_Sigma, 0, sizeof(m_Sigma));
	memset(m_Mu,    0, sizeof(m_Mu));
	memset(m_Cp,    0, sizeof(m_Cp));
	memset(m_Speed, 0, sizeof(m_Speed));
}

C3DPanelDlg::~C3DPanelDlg()
{
}

void C3DPanelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTPUT, m_ctrlOutput);
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
}


BEGIN_MESSAGE_MAP(C3DPanelDlg, CDialog)
//	ON_MESSAGE(V_ENDTHREAD, OnEndViscDialog)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// Gestionnaires de messages C3DPanelDlg

BOOL C3DPanelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CString str;
	CString strAppDirectory;
	char    szAppPath[MAX_PATH] = "";
	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
	// Extract directory
	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.GetLength()-9);
	str =strAppDirectory + "XFLR5.log";
	BOOL bOpen = m_XFile.Open(str, CFile::modeCreate | CFile::modeWrite);
	if(bOpen) m_bXFile = true;
	else      m_bXFile = false;

	m_bPointOut = false;
	m_bCancel   = false;
	m_bWarning  = false;


	SetFileHeader();
	m_ctrlOutput.SetLimitText(100000);

	m_ctrlProgress.SetRange(0,100);
	m_Progress = 0;

	if(m_pWPolar->m_AnalysisType==3) 
		StartPanelThread();

	m_b3DSymetric = false;

	SetTimer(ID_THREADTIMER, 100, NULL);
	return FALSE;
}

void C3DPanelDlg::OnTimer(UINT nIDEvent)
{
	if(m_pPanelThread && m_pPanelThread->m_bFinished){
		HANDLE hThread = m_pPanelThread->m_hThread;
		WaitForSingleObject(hThread, INFINITE);
		delete m_pPanelThread;
		m_XFile.Close();
		EndDialog(0);
	}
	CDialog::OnTimer(nIDEvent);
}


bool C3DPanelDlg::StartPanelThread()
{
	m_pPanelThread = new C3DPanelThread();
	m_pPanelThread->m_pParent = this;
//	m_pPanelThread->m_bAutoDelete = true;
	m_pPanelThread->m_bAutoDelete = false;

	m_pPanelThread->m_pWPolar  = m_pWPolar;

	m_pPanelThread->m_bSequence  = m_bSequence;

	m_pPanelThread->m_Alpha      = m_Alpha;
	m_pPanelThread->m_AlphaMax   = m_AlphaMax;
	m_pPanelThread->m_DeltaAlpha = m_DeltaAlpha;
			
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
		m_bCancel               = true;
		m_pWing->m_bCancel      = true;
		m_pPanelThread->m_bCancel = true;
	}
}
void C3DPanelDlg::SetProgress(int progress,double fraction)
{
	m_ctrlProgress.SetPos(m_Progress+ (int)(progress * fraction));
}




void C3DPanelDlg::EndSequence()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
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
     
/*LRESULT C3DPanelDlg::OnEndViscDialog(WPARAM wParam, LPARAM lParam)
{
	EndSequence();
	return 0;
}*/
BOOL C3DPanelDlg::PreTranslateMessage(MSG* pMsg) 
{
/*	if(pMsg->message == V_ENDTHREAD){
		EndSequence();
		return true; 
	} 
	else */
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN){ 
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
	if(!m_bPlaneAnalysis)	m_XFile.WriteString(m_pWing->m_WingName);
//	else 					m_XFile.WriteString(m_pPlane->m_PlaneName);
	m_XFile.WriteString("\n\n");

	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString str, strong;
	switch (tm.wMonth){
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
	str.Format(" %02d, %d  at  %02d:%02d:%02d \n\n",
		 tm.wDay, tm.wYear,
		 tm.wHour, tm.wMinute, tm.wSecond);

	m_XFile.WriteString(strong + str);
	if(m_pWPolar->m_AnalysisType==3){
		m_XFile.WriteString("\n\nnPanel Analysis\n\n\n");
	}
	m_XFile.WriteString("\n___________________________________\n\n");
}


void C3DPanelDlg::AddString(CString strong)
{
	if(m_bXFile) {
		m_XFile.WriteString(strong);
		int length = m_ctrlOutput.GetWindowTextLength();
		m_ctrlOutput.SetSel(length,length,true);
		m_ctrlOutput.ReplaceSel(strong);
	}
}


bool  C3DPanelDlg::CreateRHS(double V0, double VDelta, int nval)
{
	int p, pp, q, m;
	double alpha, phi;
	CVector V;
	CVector C;
	CVector QInf;
	AddString("      Creating RHS vector...\r\n");

//_____________________________________________________________________________________
	int N=2;//panel number
	CVector D, TestPt;
	D = m_pPanel[N].Normal;
//	D = m_pPanel[N].l;
	double h = -0.2;	//m
	double Delta = -2.0*h/100.0;	//m
	
/*	for (int l=0; l<=100;l++){	
		TestPt = m_pPanel[N].CollPt + D * h ;//+ m_pPanel[N].l * m_pPanel[N].SMP*2.0;
		DoubletNASA4023(TestPt, m_pPanel+N, V, phi);
//		SourceNASA4023(TestPt, m_pPanel+N, V, phi);
		TRACE("%10.5f      %10.5e      %10.5e      %10.5e\n", h, V.x, V.y, V.z);
		h += Delta;
	}
	return false;	
//_____________________________________________________________________________________s
*/

	m=0;
	for (q=0; q<=nval;q++){
		alpha = V0+q*VDelta;
		QInf.Set(m_QInf*cos(alpha*pi/180.0), 0.0, m_QInf * sin(alpha*pi/180.0));
		for (p=0; p< m_MatSize; p++){
			if(m_bCancel) return false;
			m_Sigma[m]= -1.0/4.0/pi* QInf.dot(m_pPanel[p].Normal);// 
			m++;
		}
		SetProgress((int)(100*q/(nval+1)),0.05);
	}
	m_Progress = 5;

	m = 0;
	for (q=0; q<=nval;q++){
		for (p=0; p<m_MatSize; p++){
			if(m_bCancel) return false;
			m_RHS[m] = 0.0;
			if(m_bNeumann) m_RHS[m] = -QInf.dot(m_pPanel[p].Normal);
			C = m_pPanel[p].CollPt; 
			for (pp=0; pp<m_MatSize; pp++){
				SourceNASA4023(C, m_pPanel+pp, V, phi);
				if(!m_bNeumann)	m_RHS[m] -=  phi* m_Sigma[pp+q*m_MatSize];
				else			m_RHS[m] -=  V.dot(m_pPanel[p].Normal) * m_Sigma[pp+q*m_MatSize];
			}
			m++;
			SetProgress((int)(100*m/((nval+1)*m_MatSize)),0.10);
		}
	}
	
	m_Progress =15;
	return true;
}


bool C3DPanelDlg::CreateMatrix()
{
	CVector C,V;
	int p, pp;
	int pos = 0;
	double phi = 0.0;
			
	AddString("      Creating the influence matrix...\r\n");

	for(p=0; p<m_MatSize; p++){
		if(m_bCancel) return false;
		C = m_pPanel[p].CollPt;// - m_pPanel[p].Normal*m_pPanel[p].Size/100.0;

		for(pp=0; pp<m_MatSize; pp++)
		{
			if(m_bCancel) return false;

			DoubletNASA4023(C, m_pPanel+pp, V, phi);
			if(!m_bNeumann)	m_aij[p*m_MatSize+pp] = phi;
			else			m_aij[p*m_MatSize+pp] = V.dot(m_pPanel[p].Normal);


		}
		SetProgress((int)(100*p/m_MatSize),0.20);
	}
	m_Progress =35;
	return true;
}

bool C3DPanelDlg::AddWakeContribution()
{
	int kw, lw, pw, p, pp;
	CVector V, C;
	double phi;
	double Delta_phi_inf = 0.0;
	double phiw[MAXSTATIONS];	

	for(p=0; p<m_MatSize; p++)//for each matrix row 
	{
		if(m_bCancel) return false;
		C = m_pPanel[p].CollPt;

		//build contributions of each wake column at point C
		//we have m_NWakeColum to consider
		pw=0;
		for (kw=0; kw<m_NWakeColumn; kw++)
		{
			phiw[kw] = 0.0;
			//each wake column has m_pWing->m_NXWakePanels
			for(lw=0; lw<m_NXWakePanels; lw++)
			{
				DoubletNASA4023(C, m_pWakePanel+pw, V, phi, true);
				phiw[kw] += phi;
				pw++;
			}
		}
		for(pp=0; pp<m_MatSize; pp++) //for each matrix column
		{
			if(m_bCancel) return false;

			// Is the panel pp shedding a wake ?
			if(m_pPanel[pp].m_bIsTrailing)
			{
				//If so, we need to add, to the matrix, the contribution of the wake column shedded by this panel
				if(m_pPanel[pp].m_iPos == -1)
				{
					//bottom side, substract the wake contribution
					m_aij[p*m_MatSize+pp] -= phiw[m_pPanel[pp].m_iWakeColumn];
				}
				else
				{
					//top side, add the wake contribution
					m_aij[p*m_MatSize+pp] += phiw[m_pPanel[pp].m_iWakeColumn];
				}
			} 
		}
		// for each wake column, add, to the RHS, the contribution of Phi_top - Phi_bot
		// i.a.w.  NASA4023 eq. 44

		for(pp=0; pp<m_MatSize; pp++)
		{
			Delta_phi_inf = 0.0;

			//for each trailing point from the bottom side
			if(m_pPanel[pp].m_bIsTrailing && m_pPanel[pp].m_iPos==-1)
			{
				// The corresponding trailing upper panel is the next coming up
				// TODO : upper panel should be indexed in panel definition
				for(int i=pp; i<m_MatSize; i++){
					if(m_pPanel[i].m_bIsTrailing && m_pPanel[i].m_iPos==1)
					{
						//calculate the freestream potential difference at the two collocation points
						Delta_phi_inf += m_QInf * ( cos(m_Alpha*pi/180) * (m_pPanel[i].CollPt.x - m_pPanel[pp].CollPt.x)
												   -sin(m_Alpha*pi/180) * (m_pPanel[i].CollPt.z - m_pPanel[pp].CollPt.z));
	//					TRACE("pp=%d   i=%d   Delta_phi_inf=%e\n", pp, i, Delta_phi_inf);
						break;
					}
					
				}
			}
			//finally add the contribution to RHS
			m_RHS[p] -= Delta_phi_inf * phiw[m_pPanel[pp].m_iWakeColumn];
		}
	}
	return true;
}

bool C3DPanelDlg::SolveMultiple(double V0, double VDelta, int nval)
{
	// solves the linear system for all DeltaAlphas (or Delta QInfs) simultaneously
	// solves the system for doublet strength stored in the m_Gamma array
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString strong, strange;
	int p, q, pp,Size, j, k, l, nrhs;
	double Lift, aoa;
	double row[VLMMATSIZE];
	CVector N, Force;
	CVector VInf(1.0,0.0,0.0);

	if(m_b3DSymetric) Size = m_MatSize/2;
	else              Size = m_MatSize;
	
//memcpy(row, m_row, sizeof(row));
	AddString("      Solving the linear system...\r\n");

	if(m_pWPolar->m_Type!=4) nrhs = nval;
	else                     nrhs = 0;

	if(!Gauss(m_aij,Size, m_RHS, nrhs)){
		AddString("      Singular Matrix.... Aborting calculation...\r\n");
		m_bConverged = false;
		return false;
	}
	else m_bConverged = true;

	//so far we have a unit Doublet Strength
	if(m_pWPolar->m_Type==2){
		//type 2; find the speeds which will create a lift equal to the weight
		AddString("      Calculating speeds to balance the weight\r\n");

		for (q=0; q<=nval;q++){
			aoa    =  (V0+q*VDelta)*pi/180.0;
			memcpy(row, m_RHS+q*Size, sizeof(row));
			Lift = 0.0;
			p=0;
			for (p=0; p<Size; p++){
				// for each panel, add the lift coef
				Lift += Force.z * row[p];
			}
			if(m_b3DSymetric) Lift *=2.0;

			if(Lift<=0.0){
				strong.Format("      Found a negative lift for Alpha=%.2f.... skipping the angle...\r\n", V0+q*VDelta);
				AddString(strong);
				m_bPointOut = true;
				m_bWarning = true;
				m_3DQInf[q] = -100.0;
			}
			else {
				Lift  *= 2.0/m_pWing->m_Area;//*cos(aoa)
				m_3DQInf[q] =  sqrt(2.0* 9.81 * m_pWPolar->m_Weight/m_pWPolar->m_Density/m_pWing->m_Area/Lift);
				strong.Format("      Alpha=%5.2f   QInf = %5.2f", V0+q*VDelta, m_3DQInf[q]*pFrame->m_mtoUnit);
				GetSpeedUnit(strange, pFrame->m_SpeedUnit);
				strong+= strange + "\r\n";
				AddString(strong);
			}
		}
	}
	else if (m_pWPolar->m_Type==1)
		for (q=0; q<=nval;q++) m_3DQInf[q] = m_pWPolar->m_QInf;
	else if (m_pWPolar->m_Type==4)
		for (q=0; q<=nval;q++) m_3DQInf[q] = V0 + q*VDelta;

	
	if(m_pWPolar->m_Type!=4){
		p=0;
		for (q=0; q<=nval;q++){
			p = q*m_MatSize;
			for(pp=0; pp<Size; pp++){
//				m_Mu[p] = m_RHS[q*Size+pp]*m_3DQInf[q];
				m_Mu[p] = m_RHS[q*Size+pp];
				p++;
			}
			if(m_b3DSymetric){
				p  =  q   * m_MatSize;
				pp = (q+1)* m_MatSize;
				for (j=0; j<m_NSurfaces/2; j++){//All surfaces
					for (k=0; k<m_pSurface[j].m_NYPanels; k++){
						pp -= 2*m_pSurface[j].m_NXPanels;
						for (l=0; l<2*m_pSurface[j].m_NXPanels; l++){
							m_Mu[pp+l] = m_Mu[p];
							p++;
						}
					}
				}
			}
		}
	}
	else{
		p=0;
		for (q=0; q<=nval;q++){
			p = q*m_MatSize;
			for(pp=0; pp<Size; pp++){
				m_Mu[p] = m_RHS[pp]*m_3DQInf[q];
				p++;
			}
			if(m_b3DSymetric){
				p  =  q   * m_MatSize;
				pp = (q+1)* m_MatSize;
				for (j=0; j<m_NSurfaces/2; j++){//All surfaces
					for (k=0; k<m_pSurface[j].m_NYPanels; k++){
						pp -= 2*m_pSurface[j].m_NXPanels;
						for (l=0; l<2*m_pSurface[j].m_NXPanels; l++){
							m_Mu[pp+l] = m_Mu[p];
							p++;
						}
					}
				}
			}
		}
	}	
	AddString("\r\n");
	return true;
}


bool C3DPanelDlg::Gauss(double *A, int n, double *B, int m)
{
 	int row, i, j, pivot_row, k;
	double max, dum, *pa, *pA, *A_pivot_row;
	// for each variable find pivot row and perform forward substitution
	pa = A;
	for (row = 0; row < (n - 1); row++, pa += n) {
		//  find the pivot row
		A_pivot_row = pa;
		max = abs(*(pa + row));
		pA = pa + n;
		pivot_row = row;
		for (i=row+1; i < n; pA+=n, i++)
			if ((dum = abs(*(pA+row))) > max) { 
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
		SetProgress((int)(100*row/(n-1)/2),.15);
	}
	m_Progress +=15;

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
		}
		SetProgress((int)(100*(n-1-row)/(n-1)/2),.15);
	}
	m_Progress +=15;
	return true;
}


void C3DPanelDlg::CheckSolution()
{
	CVector C,V;
	int p, pp;
	double phi, phiS, phiD, row;
	if(m_bNeumann) return;//...
	for (p=0; p<m_MatSize; p++){
		row = 0.0;
		C = m_pPanel[p].CollPt;// - m_pPanel[p].Normal*m_pPanel[p].Size/100.0;
		phiS = 0.0; phiD = 0.0;
		for (pp=0; pp<m_MatSize; pp++){
			SourceNASA4023(C, m_pPanel+pp, V, phi);
			phiS+= phi * m_Sigma[pp];
			DoubletNASA4023(C, m_pPanel+pp, V, phi);
			phiD+= phi * m_Mu[pp];
			row = phiS+phiD;
		}
TRACE("Potential[%d] =   %12.6e   %12.6e   %12.6e\n", p, phiS, phiD, row);
	}
}


void C3DPanelDlg::ComputePlane(double V0, double VDelta, int nrhs)
{
	// calculates the various wing coefficients by interpolating
	// the adequate variable, from Cl, on the XFoil polar mesh
	// at each span station
	int q,pos, Station;
	double deltProgress = .35/double(nrhs+1);
	double frac = deltProgress/2.0;
	CString str;

	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	
	double Lift, IDrag, VDrag ,XCP, YCP, Rm, IYm, GYm, LinPm;
	double WingLift, WingIDrag;

	m_pWing->m_pVLMDlg  = this;
	m_pWing->m_bTrace   = true;

	AddString("Computing Plane\r\n\r\n");

	if(m_pWing2) {
		m_pWing2->m_pVLMDlg  = this;
		m_pWing2->m_bTrace   = true;
	}
	if(m_pStab) {
		m_pStab->m_pVLMDlg  = this;
		m_pStab->m_bTrace   = true;
	}
	if(m_pFin){
		m_pFin->m_pVLMDlg  = this;
		m_pFin->m_bTrace   = true;
	}
	for(q=0; q<=nrhs; q++){

		if(m_bCancel) break;

		m_pWing->m_bWingOut = false;
		if(m_pWing2)	m_pStab->m_bWingOut = false;
		if(m_pStab)		m_pStab->m_bWingOut = false;
		if(m_pFin)		m_pFin->m_bWingOut  = false;

		if(m_pWPolar->m_Type!=4) m_Alpha = V0+q*VDelta;		
		if(!m_pWPolar->m_bTiltedGeom) m_AlphaCalc = m_Alpha;

		m_QInf = m_3DQInf[q];

		if(m_QInf >0.0) {
			if(m_pWPolar->m_Type!=4) str.Format("   ...Alpha=%.2f\r\n", m_Alpha);
			else                     str.Format("   ...QInf = %6.2f\r\n", m_QInf);
			AddString(str);
//			VLMSetAi(m_Gamma+i*m_MatSize);
		AddString("     Computing Off-Body and On-Body Speeds...");
		if(!ComputeOnBody(m_Mu+q*m_MatSize, m_Sigma+q*m_MatSize, frac)) break;
		m_Progress+=(int)(100*frac);
		if(!ComputeSurfSpeeds(m_Mu+q*m_MatSize, m_Sigma+q*m_MatSize, frac)) break;
		m_Progress+=(int)(100*frac);
		AddString("   Done\r\n");

			AddString("     Calculating aerodynamic coefficients...\r\n");
			m_bPointOut          = false;
			m_pWing->m_Alpha     = m_Alpha;
			m_pWing->m_QInf      = m_QInf;
			m_pWing->m_Viscosity = m_pWPolar->m_Viscosity;
			m_pWing->m_Density   = m_pWPolar->m_Density;

			Lift   = 0.0;
			IDrag  = 0.0;
			VDrag  = 0.0;
			XCP    = 0.0;
			YCP    = 0.0;
			Rm     = 0.0;
			IYm    = 0.0;
			GYm    = 0.0;
			LinPm  = 0.0;

			AddString("       Calculating wing...\r\n");
			m_pWing->PanelComputeWing(m_Ai, m_Cp, 
									Lift, IDrag, VDrag, XCP, YCP, LinPm, Rm, IYm, GYm, m_pWPolar->m_bViscous);

//			for (m=0; m< m_pWing->m_NStation; m++) m_ICd[m] = m_pWing->m_ICd[m];
//			m_pWing->VLMSetBending();
			if(m_pWing->m_bWingOut)  m_bPointOut = true;

			Station = m_pWing->m_NStation;
			
			if(m_pStab) {
				AddString("       Calculating elevator...\r\n");
				WingLift  = 0.0;
				WingIDrag = 0.0;
				m_pStab->m_Alpha     = m_Alpha;
				m_pStab->m_QInf      = m_QInf;
				m_pStab->m_Viscosity = m_pWPolar->m_Viscosity;
				m_pStab->m_Density   = m_pWPolar->m_Density;
/*				m_pStab->VLMComputeWing(m_Gamma+i*m_MatSize+m_pWing->m_MatSize,
										m_Ai+m_pWing->m_NStation,
										m_Cp+m_pWing->m_MatSize,
										WingLift, WingIDrag, VDrag, XCP, YCP, LinPm, Rm, IYm, GYm,
										m_pWPolar->m_bViscous);*/
				Lift  += WingLift;
				IDrag += WingIDrag;

				m_pStab->VLMSetBending();
				if(m_pStab->m_bWingOut) m_bPointOut = true;

				Station += m_pStab->m_NStation;
			}

			if(m_pFin){
				AddString("       Calculating fin...\r\n");
				WingLift  = 0.0;
				WingIDrag = 0.0;
				m_pFin->m_Alpha      = m_Alpha;
				m_pFin->m_QInf       = m_QInf;
				m_pFin->m_Viscosity  = m_pWPolar->m_Viscosity;
				m_pFin->m_Density    = m_pWPolar->m_Density;

				pos = m_pWing->m_3DMatSize;
				if(m_pStab) pos += m_pStab->m_3DMatSize;
				
/*				m_pFin->VLMComputeWing( m_Gamma+i*m_MatSize+pos,
										m_Ai+Station,
										m_Cp+pos,
										WingLift, WingIDrag, VDrag, XCP, YCP, LinPm, Rm, IYm, GYm,
										m_pWPolar->m_bViscous);*/
				if(m_pFin->m_bWingOut)  m_bPointOut = true;

				Lift  += WingLift;
				IDrag += WingIDrag;

				m_pFin->VLMSetBending();
			}
		
			m_CL          =  2.0*Lift /m_QInf/m_pWing->m_Area;
			m_InducedDrag = -1.0*IDrag/m_QInf/m_pWing->m_Area;
			m_ViscousDrag =  1.0*VDrag       /m_pWing->m_Area;

			m_XCP         = XCP/Lift;
			m_YCP         = YCP/Lift;

//			SetDownwash(m_Gamma+i*m_MatSize);

			m_IYm        =         IYm   / m_pWing->m_Area/m_pWing->m_Span;
			m_GYm         =  2.0 * GYm   / m_pWing->m_Area/m_pWing->m_Span  /m_QInf/m_QInf;
			m_Rm          = -2.0 * Rm    / m_pWing->m_Area/m_pWing->m_Span  /m_QInf/m_QInf;
			m_GCm         =  2.0 * LinPm / m_pWing->m_Area/m_pWing->m_MAChord/m_QInf/m_QInf;
			m_VCm         =        m_pWing->m_VCm;
			m_TCm         =  m_GCm + m_pWing->m_VCm;
			if(m_pStab) m_TCm += m_pStab->m_VCm;
			if(m_pFin)  m_TCm += m_pFin->m_VCm;

			if(m_bPointOut) m_bWarning = true;
//			if(m_bConverged){
				if(m_pPlane)
					pMiarex->AddPOpp(m_Mu+q*m_MatSize);
				else
					pMiarex->AddWOpp(m_bPointOut, m_Mu+q*m_MatSize);		
//			}
			AddString("\r\n");
		}
		else m_bPointOut = true;

		SetDownwash(m_Mu+q*m_MatSize, m_Sigma+q*m_MatSize);

		SetProgress((int)(100*q/(nrhs+1)),.35);
	}
	SetProgress(100,.35);
}


void C3DPanelDlg::DoubletNASA4023(CVector TestPt, CPanel *pPanel, CVector &V, double &phi, bool bWake)
{
	//VSAERO theory Manual
	//Influence of panel pp at coll pt of panel p
	int i;
	double side, sign;
	double eps = 1.e-7;
	double RNUM, DNOM, PN, A, B, PA, PB, SM, SL, AM, AL, Al, pjk, CJKi;
	CVector PJK, a, b, s, T;
	CVector R[5];
	CVector *pNode;
	if(bWake)	pNode = m_pWakeNode;
	else		pNode = m_pNode;

	phi = 0.0;
	V.Set(0.0,0.0,0.0);

	PJK = TestPt - pPanel->CollPt;
	PN  = PJK.dot(pPanel->Normal);
	pjk = PJK.VAbs();

	if(pjk> RFF*pPanel->Size){ // use far-field formula
		phi = PN * pPanel->Area /pjk/pjk/pjk;
		V   = (PJK*3.0*PN - pPanel->Normal*pjk*pjk)*pPanel->Area /pjk/pjk/pjk/pjk/pjk;
		return;
	}  
	else
	{
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
		for (i=0; i<4; i++){
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


	//first the potential
			if(R[i].IsSame(R[i+1])) 
			{
				CJKi = 0.0;
				//no contribution to speed either
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
				V += (a * b) * (A+B) /A/B/ (A*B + a.dot(b));
			}
			phi += CJKi;

		}
		if(TestPt.IsSame(pPanel->CollPt)){
			phi  = -2.0*pi;	
		}
	}
}


void C3DPanelDlg::SourceNASA4023(CVector TestPt, CPanel *pPanel, CVector &V, double &phi)
{
	//VSAERO theory Manual
	//Influence of panel pp at coll pt of panel p
	double eps = 1.e-7;
	int i;
	CVector PJK, a, b, s, T1, T2, T;
	CVector R[5];
	double PN, A, B, PA, PB, S, SM, SL, AM, AL, Al, GL, CJKi, pjk;
	double RNUM, DNOM;
	double side, sign;
	
	phi = 0.0;
	V.Set(0.0,0.0,0.0);

	PJK = TestPt - pPanel->CollPt;
	PN  = PJK.dot(pPanel->Normal);
	pjk = PJK.VAbs();

	if(pjk> RFF*pPanel->Size){
		// use far-field formula
		phi = pPanel->Area /pjk;
		V = PJK * pPanel->Area/pjk/pjk/pjk;
		return;
	}
	
	phi = 0.0;
	V.Set(0.0,0.0,0.0);
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


bool C3DPanelDlg::ComputeOnBody(double *Mu, double *Sigma, double frac)
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
	
	CVector Qp(m_QInf*cos(m_Alpha*pi/180.0), 0.0, m_QInf * sin(m_Alpha*pi/180.0));

	for (p=0; p<m_MatSize; p++){
		if(m_bCancel) return false;
		PL = m_pPanel[p].m_iPL;
		PR = m_pPanel[p].m_iPR;
		PU = m_pPanel[p].m_iPU;
		PD = m_pPanel[p].m_iPD;

		if(PL>=0 && PR>=0){
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
		else if(PL>=0 && PR<0){
			// no right neighbour
			// do we have two left neighbours ?
			if(m_pPanel[PL].m_iPL>=0){
				x2  = 0.0;
				x1  = x2 - m_pPanel[p].SMQ  - m_pPanel[PL].SMQ;
				x0  = x1 - m_pPanel[PL].SMQ 
						 - m_pPanel[m_pPanel[PL].m_iPL].SMQ;
				mu0 = Mu[m_pPanel[PL].m_iPL];
				mu1 = Mu[PL];
				mu2 = Mu[p];
				DELQ =	  mu0 *(x2-x1)       /(x0-x1)/(x0-x2) 
						+ mu1 *(x2-x0)       /(x1-x0)/(x1-x2) 
						+ mu2 *(2.0*x2-x0-x1)/(x2-x0)/(x2-x1);
			}
			else ASSERT(FALSE);
		}
		else if(PL<0 && PR>=0){
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
				x1  = x2 - m_pPanel[p].SMP  - m_pPanel[PU].SMP;
				x0  = x1 - m_pPanel[PU].SMP 
						 - m_pPanel[m_pPanel[PU].m_iPU].SMP;
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
		m_Cp[p]  = 1.0-Speed2/m_QInf/m_QInf;
		m_Speed[p] = m_pPanel[p].LocalToGlobal(QInfl);
		SetProgress((int)(100*p/m_MatSize), frac);
	}
	return true;
}



CVector C3DPanelDlg::GetSpeedVector(CVector C, double *Mu, double *Sigma)
{	
	CVector V;
	int pp;

	double phi;
	CVector VT(0.0,0.0,0.0);
	for (pp=0; pp<m_MatSize;pp++){
		if(m_bCancel) return VT;
		DoubletNASA4023(C,m_pPanel+pp, V, phi);
		VT += V * Mu[pp];
		SourceNASA4023(C, m_pPanel+pp, V, phi);
		VT += V * Sigma[pp];
	}
	return VT;
}



bool C3DPanelDlg::ComputeSurfSpeeds(double *Mu, double *Sigma, double frac)
{
	int p;
	CVector C;
	CVector Q(m_QInf,0.0,0.0);

	for (p=0; p<m_MatSize; p++){
		if(m_bCancel) return false;
		C = m_pPanel[p].CollPt + m_pPanel[p].Normal*m_pPanel[p].Size/20.0;

		m_Speed[p]  = GetSpeedVector(C, Mu, Sigma);
		m_Speed[p] += Q;

		SetProgress((int)(100*p/m_MatSize), frac);
	}
	return true;
}



void C3DPanelDlg::SetDownwash(double *Mu, double *Sigma)
{
	// calculates the induced angles from the vortices strengths
	int j, k, m, nSurf;
	CVector C;
	CVector X(0.001, 0.0, 0.0);
	m=0;
	nSurf = 0;
	//wing first
	memset(m_pWing->m_Vd, 0, sizeof(m_pWing->m_Vd));
	for (j=0; j<m_pWing->m_NSurfaces; j++){
		for (k=0; k<m_pWing->m_Surface[j].m_NYPanels; k++)
		{
			m_pSurface[j].GetTrailingPt(C, k);
			C += X;
			m_pWing->m_Vd[m] = GetSpeedVector(C, Mu, Sigma);
			m++;
		}
		nSurf++;
	}
}



