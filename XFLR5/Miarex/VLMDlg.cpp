/****************************************************************************

    CVLMDlg Class
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

// VLMDlg.cpp : implementation file
// Sets up the dialog box which will run the VLM thread
// We need a thread to authorize user interruption of the calculation
//
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "VLMDlg.h"
#include "Miarex.h"
#include ".\vlmdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CVLMDlg dialog


CVLMDlg::CVLMDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVLMDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVLMDlg)
	//}}AFX_DATA_INIT
	pi = 3.141592654;

	m_bSequence      = false;
	m_bWarning       = false;
	m_bXFile         = false;
	m_bVLMSymetric   = true;
	m_bWakeRollUp    = false;
	m_bConverged     = false;
	m_bPointOut      = false;
	m_bCancel        = false;

	m_VLMMatSize     = 0;
	m_nNodes         = 0;
	m_NSurfaces      = 0;
	m_NXWakePanels   = 0;
	m_MaxWakeIter    = 5;
	m_WakeInterNodes = 6;

	m_QInf       = 0.0;
	m_QInfMax    = 0.0;
	m_DeltaQInf  = 0.0;

	m_Alpha      = 0.0;
	m_AlphaMax   = 0.0;
	m_DeltaAlpha = 0.0;

	m_AlphaCalc  = 0.0;//calculation on a tilted geometry

	m_CoreSize   = 0.0;

	m_XCP = 0.0;
	m_YCP = 0.0;
	m_VCm = 0.0;
	m_TCm = 0.0;
	m_GCm = 0.0;
	m_CL  = 0.0;
	m_ViscousDrag = 0.0;
	m_InducedDrag = 0.0;
	m_IYm = 0.0;
	m_GYm = 0.0;

	m_strOut = "";

	m_pWing   = NULL;
	m_pWing2  = NULL;
	m_pStab   = NULL;
	m_pFin    = NULL;
	m_pWPolar = NULL;
	m_pPlane  = NULL;
	m_pVLMThread  = NULL;
	m_pMiarex = NULL;
	m_pFrame  = NULL;
	m_pWakePanel  = NULL;
	m_pWakeNode   = NULL;

	memset(m_Cp,0,sizeof(m_Cp));
	memset(m_row,0,sizeof(m_row));
	memset(m_aij,0,sizeof(m_aij));
	memset(m_VLMQInf,0,sizeof(m_VLMQInf));
	memset(m_Gamma,0,sizeof(m_Gamma));
	memset(m_pPanel, 0, sizeof(m_pPanel));
	memset(m_pNode, 0, sizeof(m_pNode));
}


void CVLMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTPUT, m_ctrlOutput);
}


BEGIN_MESSAGE_MAP(CVLMDlg, CDialog)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVLMDlg message handlers

BOOL CVLMDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CRect WndRect;
	GetWindowRect(WndRect);
	SetWindowPos(NULL,GetSystemMetrics(SM_CXSCREEN)-WndRect.Width()-10,60,0,0,SWP_NOSIZE);


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
	else m_bXFile = false;

	m_bPointOut = false;
	m_bCancel   = false;
	m_bWarning  = false;

	m_ctrlOutput.SetLimitText(100000);

	memcpy(m_pPanel, m_pRefPanels, sizeof(m_pPanel));
	memcpy(m_pNode, m_pRefNodes, sizeof(m_pNode));
//	memcpy(m_pSurface, m_pRefSurfaces, sizeof(m_pSurface));
	for (int l=0; l<m_NSurfaces; l++)
		m_pSurface[l].Copy(m_pRefSurfaces[l]);

	m_pRefPanels->m_CtrlPos = m_pPanel->m_CtrlPos;
	m_pRefPanels->m_VortexPos = m_pPanel->m_VortexPos;

	SetFileHeader();

	m_bVLMSymetric = m_pWing->m_bVLMSymetric;
	if(m_pFin || m_pStab || m_pWing2)		m_bVLMSymetric = false;
	if(!m_pWPolar->m_bMiddle)	m_bVLMSymetric = false;

	m_pWing->m_bVLMSymetric = m_bVLMSymetric;
	
	if(m_pWing2)	m_pWing2->m_bVLMSymetric = m_bVLMSymetric;
	if(m_pStab)		m_pStab->m_bVLMSymetric = m_bVLMSymetric;
	if(m_pFin)		m_pFin->m_bVLMSymetric  = m_bVLMSymetric;

	if(m_pWPolar->m_AnalysisType==2){ //no way it could be anything else
		m_pVLMThread = new CVLMThread();
		m_pVLMThread->m_pParent = this;
//		m_pVLMThread->m_bAutoDelete = true;
		m_pVLMThread->m_bAutoDelete = false;
		m_pVLMThread->m_bType4      = (m_pWPolar->m_Type==4);
		m_pVLMThread->m_MaxWakeIter = m_MaxWakeIter;
		m_pVLMThread->m_bSequence   = m_bSequence;
		m_pVLMThread->m_Alpha       = m_Alpha;
		m_pVLMThread->m_AlphaMax    = m_AlphaMax;
		m_pVLMThread->m_DeltaAlpha  = m_DeltaAlpha;
		m_pVLMThread->m_QInf        = m_QInf;
		m_pVLMThread->m_QInfMax     = m_QInfMax;
		m_pVLMThread->m_DeltaQInf   = m_DeltaQInf;
		m_pVLMThread->m_pMiarex     = m_pMiarex;
				
		m_pVLMThread->CreateThread(CREATE_SUSPENDED);
		VERIFY(m_pVLMThread->SetThreadPriority(THREAD_PRIORITY_LOWEST));

		m_pVLMThread->ResumeThread();
	}
	SetTimer(ID_THREADTIMER, 100, NULL);
	return FALSE;
}


void CVLMDlg::OnTimer(UINT nIDEvent)
{
	if(m_pVLMThread && m_pVLMThread->m_bFinished){
		HANDLE hThread = m_pVLMThread->m_hThread;
		WaitForSingleObject(hThread, INFINITE);
		EndSequence();
	}
	CDialog::OnTimer(nIDEvent);
}

bool CVLMDlg::Gauss(double *A, int n, double *B, int m)
{
////////////////////////////////////////////////////////////////////////////////
//  int Gaussian_Elimination(double *A, int n, double *B)                     //
//                                                                            //
//     Solve the linear system of equations AX=B where A is an n x n matrix   //
//     B is an n-dimensional column vector (n x 1 matrix) for the             //
//     n-dimensional column vector (n x 1 matrix) X.                          //
//                                                                            //
//     This routine performs partial pivoting and the elements of A are       //
//     modified during computation.  The result X is returned in B.           //
//     If the matrix A is singular, the return value of the function call is  //
//     false. If the solution was found, the function return value is true    //
//                                                                            //
//  Arguments:                                                                //
//     double *A      On input, the pointer to the first element of the       //
//                    matrix A[n][n].  On output, the matrix A is destroyed.  //
//     int     n      The number of rows and columns of the matrix A and the  //
//                    dimension of B.                                         //
//     double *B      On input, the pointer to the first element of the       //
//                    vector B[n].  On output, the vector B is replaced by the//
//                    vector X, the solution of AX = B.                       //
//     int m          The number of right hand side vectors                   //
//                    The default is m=0                                      //
//                                                                            //
//  Return Values:                                                            //
//     true :  Success                                                        //
//     false :  Failure - The matrix A is singular.                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
	int row, i, j, pivot_row, k;
	double max, dum, *pa, *pA, *A_pivot_row;
	// for each variable find pivot row and perform forward substitution
	pa = A;
	for (row = 0; row < (n - 1); row++, pa += n) {
		if(m_bCancel) return false;
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
			if(m_bCancel) return false;
			pA = A + i * n;
			dum = - *(pA + row) / *(pa + row);
			*(pA + row) = 0.0;
			for (j=row+1; j<n; j++) *(pA+j) += dum * *(pa + j);
			for (k=0; k<=m; k++) 
				B[i+k*n] += dum * B[row+k*n];
		}
	}
	
	// Perform backward substitution
	
	pa = A + (n - 1) * n;
	for (row = n - 1; row >= 0; pa -= n, row--) {
		if(m_bCancel) return false;
		if ( *(pa + row) == 0.0 ) 
			return false;           // matrix is singular
		dum = 1.0 / *(pa + row);
		for ( i = row + 1; i < n; i++) *(pa + i) *= dum; 
		for(k=0; k<=m; k++) B[row+k*n] *= dum;
		for ( i = 0, pA = A; i < row; pA += n, i++) {
			if(m_bCancel) return false;
			dum = *(pA + row);
			for ( j = row + 1; j < n; j++) *(pA + j) -= dum * *(pa + j);
			for(k=0; k<=m; k++) 
				B[i+k*n] -= dum * B[row+k*n];
		}
	}
	return true;
}

void CVLMDlg::SetFileHeader()
{
	m_XFile.WriteString("\n");
	m_XFile.WriteString(m_VersionName);
	m_XFile.WriteString("\n\n");
	if(!m_pPlane)	m_XFile.WriteString(m_pWing->m_WingName);
	else m_XFile.WriteString(m_pPlane->m_PlaneName);
	
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
		 tm.wDay, tm.wYear,	tm.wHour, tm.wMinute, tm.wSecond);

	m_XFile.WriteString(strong + str);
	m_XFile.WriteString("\n___________________________________\n\n");
}

void CVLMDlg::AddString(CString strong)
{
	if(!m_bTrace) return;
	if(m_bXFile) {
		m_XFile.WriteString(strong);
		int length = m_ctrlOutput.GetWindowTextLength();
		m_ctrlOutput.SetSel(length,length,true);
		m_ctrlOutput.ReplaceSel(strong);
	}
}
      

BOOL CVLMDlg::PreTranslateMessage(MSG* pMsg) 
{
/*	if(pMsg->message == V_ENDTHREAD){
		EndSequence();
		return true; 
	} 
	else*/
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN){ 
		OnCancel();
		return true;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}


void CVLMDlg::OnCancel() 
{
	if(!m_pVLMThread) {
		CDialog::OnCancel();
		return;
	}
	if(m_pVLMThread && !m_pVLMThread->m_bFinished) {
		m_bCancel               = true;
		m_pWing->m_bCancel      = true;
		m_pVLMThread->m_bCancel = true;
	}
}

void CVLMDlg::EndSequence()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(pMiarex->m_bVLMFinished && !m_bXFile){
		return;
	}
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
	delete m_pVLMThread;
	m_pVLMThread = NULL;
	EndDialog(0);
}


bool CVLMDlg::VLMCreateMatrix()
{
//	CVLMDlg *pVLMDlg = (CVLMDlg*)m_pVLMDlg;
	int p,pp, Size;
	int pos = 0;
	CVector  CC;
	AddString("      Creating the influence matrix...\r\n");

	if(m_bVLMSymetric) Size = m_VLMMatSize/2;
	else               Size = m_VLMMatSize;

	for(p=0; p<Size; p++){
		if(m_bCancel) break;
		for(pp=0; pp<Size; pp++){
			VLMGetVortexInfluence(pp, m_pPanel[p].CtrlPt, V, true);

			if(m_bVLMSymetric){// add right wing contribution
				CC = m_pPanel[p].CtrlPt;
				CC.y = -CC.y;
				VLMGetVortexInfluence(pp, CC, VS, true);
				V.x += VS.x;
				V.y -= VS.y;
				V.z += VS.z;
			}

			m_aij[p*Size+pp] = DotProduct(V, m_pPanel[p].Normal);
		}
	}
//double row[VLMMATSIZE];
//memcpy(row, m_aij, sizeof(row));

	return true;
}


bool CVLMDlg::VLMSolveMultiple(double V0, double VDelta, int nval)
{
	// solves the linear system for all DeltaAlphas (or Delta QInfs) simultaneously
	// solves the system for vortices Strength stored in the m_Gamma array
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString strong, strange;
	int p, q, pp, m ,Size, j, k, l, nrhs;
	double Lift, aoa, cosaoa, sinaoa;
	double row[VLMMATSIZE];
	CVector N, Force;
	CVector VInf(1.0,0.0,0.0);

	if(m_bVLMSymetric) Size = m_VLMMatSize/2;
	else               Size = m_VLMMatSize;
	
	memset(m_row, 0, sizeof(m_row));
	memset(row,   0, sizeof(row));


	//Creating RHS
	if(m_pWPolar->m_Type!=4){
		p=0;

		for (q=0; q<=nval;q++){
			aoa    =  (V0+q*VDelta)*pi/180.0;
			cosaoa = cos(aoa);
			sinaoa = sin(aoa);
			for (m=0; m<Size; m++){
				m_row[p] = -cosaoa*m_pPanel[m].Normal.x  - sinaoa*m_pPanel[m].Normal.z;
				p++;
			}
		}
	}
	else {//type 4... solve only once with unit speed
		aoa    = m_Alpha*pi/180.0;
		cosaoa = cos(aoa);
		sinaoa = sin(aoa);
		for (p=0;p<Size; p++){
			m_row[p] = -cosaoa*m_pPanel[p].Normal.x  - sinaoa*m_pPanel[p].Normal.z;
		}
	}

	AddString("      Solving the linear system...\r\n");

	if(m_pWPolar->m_Type!=4) nrhs = nval;
	else                     nrhs = 0;

	if(!Gauss(m_aij,Size, m_row, nrhs)){
		AddString("      Singular Matrix.... Aborting calculation...\r\n");
		m_bConverged = false;
		return false;
	}
	else m_bConverged = true;

	AddString("      Calculating the vortices strengths...\r\n");

	//so far we have a unit Vortex Strength
	if(m_pWPolar->m_Type==2){
		//type 2; find the speeds which will create a lift equal to the weight
		AddString("      Calculating speeds to balance the weight\r\n");

		for (q=0; q<=nval;q++){
			aoa    =  (V0+q*VDelta)*pi/180.0;
			memcpy(row, m_row+q*Size, sizeof(row));
			Lift = 0.0;
			p=0;
			for (p=0; p<Size; p++){
				// for each panel along the chord, add the lift coef
				if(m_pWPolar->m_bVLM1 || m_pPanel[p].m_bIsLeading){
					Force = VInf * m_pPanel[p].Vortex;
					Lift += Force.z * row[p];
				}
				else {
					Force = VInf * m_pPanel[p].Vortex;
					Lift += Force.z * row[p];
					Force = VInf * m_pPanel[p+1].Vortex;
					Lift -= Force.z * row[p+1];
				}
//				TRACE("p=%d   Force.z=%f   rowp=%f   rowp1=%f\n",p, Force.z, row[p], row[p+1]);
			}
			if(m_bVLMSymetric) Lift *=2.0;

			if(Lift<=0.0){
				strong.Format("      Found a negative lift for Alpha=%.2f.... skipping the angle...\r\n", V0+q*VDelta);
				if(m_bTrace) AddString(strong);
				m_bPointOut = true;
				m_bWarning = true;
				m_VLMQInf[q] = -100.0;
			}
			else {
				m_VLMQInf[q] =  sqrt( 9.81 * m_pWPolar->m_Weight
									/(m_pWPolar->m_Density*Lift));
				strong.Format("      Alpha=%5.2f   QInf = %5.2f", V0+q*VDelta, m_VLMQInf[q]*pFrame->m_mstoUnit);
				GetSpeedUnit(strange, pFrame->m_SpeedUnit);
				strong+= strange + "\r\n";
				if(m_bTrace) AddString(strong);
			}
		}
	}
	else if (m_pWPolar->m_Type==1)
		for (q=0; q<=nval;q++) m_VLMQInf[q] = m_pWPolar->m_QInf;
	else if (m_pWPolar->m_Type==4)
		for (q=0; q<=nval;q++) m_VLMQInf[q] = V0 + q*VDelta;

	int coef = 1;
	if(!m_pWPolar->m_bMiddle) coef = 2;
	if(m_pWPolar->m_Type!=4){
		p=0;
		for (q=0; q<=nval;q++){
			p = q*m_VLMMatSize;
			for(pp=0; pp<Size; pp++){
				m_Gamma[p] = m_row[q*Size+pp]*m_VLMQInf[q];
				p++;
			}
			if(m_bVLMSymetric){
				p  =  q   * m_VLMMatSize;
				pp = (q+1)* m_VLMMatSize;
				for (j=0; j<m_NSurfaces/2; j++){//All surfaces
					for (k=0; k<m_pSurface[j].m_NYPanels; k++){
						pp -= coef*m_pSurface[j].m_NXPanels;
						for (l=0; l<coef*m_pSurface[j].m_NXPanels; l++){
							m_Gamma[pp+l] = m_Gamma[p];
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
			p = q*m_VLMMatSize;
			for(pp=0; pp<Size; pp++){
				m_Gamma[p] = m_row[pp]*m_VLMQInf[q];
				p++;
			}
			if(m_bVLMSymetric){
				p  =  q   * m_VLMMatSize;
				pp = (q+1)* m_VLMMatSize;
				for (j=0; j<m_NSurfaces/2; j++){//All surfaces
					for (k=0; k<m_pSurface[j].m_NYPanels; k++){
						pp -= coef*m_pSurface[j].m_NXPanels;
						for (l=0; l<coef*m_pSurface[j].m_NXPanels; l++){
							m_Gamma[pp+l] = m_Gamma[p];
							p++;
						}
					}
				}
			}
		}
	}

	memcpy(row, m_Gamma, sizeof(row));
	AddString("\r\n");

	return  true;
}

void CVLMDlg::VLMComputePlane(double V0, double VDelta, int nrhs)
{
	// calculates the various wing coefficients by interpolating
	// the adequate variable, from Cl, on the XFoil polar mesh
	// at each span station
	int i, m, pos, Station;
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	double Lift, IDrag, VDrag ,XCP, YCP, Rm, IYm, GYm, LinPm;
	double WingLift, WingIDrag;
	CString str, strong;

	m_pWing->m_pVLMDlg  = this;
	m_pWing->m_bTrace   = false;
	m_pWing->m_bVLM1    = m_pWPolar->m_bVLM1;
	m_pWing->m_bTrace   = true;
	m_pWing->m_pWakeNode  = m_pWakeNode;
	m_pWing->m_pWakePanel = m_pWakePanel;

	if(m_pWing2) {
		m_pWing2->m_pVLMDlg  = this;
		m_pWing2->m_bTrace   = false;
		m_pWing2->m_bVLM1    = m_pWPolar->m_bVLM1;
		m_pWing2->m_bTrace   = true;
	}

	if(m_pStab) {
		m_pStab->m_pVLMDlg  = this;
		m_pStab->m_bTrace   = false;
		m_pStab->m_bVLM1    = m_pWPolar->m_bVLM1;
		m_pStab->m_bTrace   = true;
	}
	if(m_pFin){
		m_pFin->m_pVLMDlg  = this;
		m_pFin->m_bTrace   = false;
		m_pFin->m_bVLM1    = m_pWPolar->m_bVLM1;
		m_pFin->m_bTrace   = true;
	}
	for(i=0; i<=nrhs; i++){
		if(m_bCancel) break;
		m_pWing->m_bWingOut = false;
		if(m_pWing2)	m_pWing2->m_bWingOut = false;
		if(m_pStab)		m_pStab->m_bWingOut = false;
		if(m_pFin)		m_pFin->m_bWingOut  = false;

		if(m_pWPolar->m_Type!=4) m_Alpha = V0+i*VDelta;		
		if(!m_pWPolar->m_bTiltedGeom) m_AlphaCalc = m_Alpha;

		m_QInf = m_VLMQInf[i];

		if(m_QInf >0.0) {
			if(m_pWPolar->m_Type!=4) str.Format("   ...Alpha=%.2f\r\n", m_Alpha);
			else {
				str.Format("   ...QInf = %6.2f ", m_QInf*pFrame->m_mstoUnit);
				GetSpeedUnit(strong, pFrame->m_SpeedUnit);
				str += strong + "\r\n";
			}
			AddString(str);
			VLMSetAi(m_Gamma+i*m_VLMMatSize);

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
			m_pWing->VLMComputeWing(m_Gamma+i*m_VLMMatSize,m_Ai, m_Cp, 
									Lift, IDrag, VDrag, XCP, YCP, LinPm, Rm, IYm, GYm, m_pWPolar->m_bViscous);

			for (m=0; m< m_pWing->m_NStation; m++) m_ICd[m] = m_pWing->m_ICd[m];
			m_pWing->VLMSetBending();
			if(m_pWing->m_bWingOut)  m_bPointOut = true;

			Station = m_pWing->m_NStation;
			
			if(m_pWing2) {
				AddString("       Calculating elevator...\r\n");
				WingLift  = 0.0;
				WingIDrag = 0.0;
				m_pWing2->m_Alpha     = m_Alpha;
				m_pWing2->m_QInf      = m_QInf;
				m_pWing2->m_Viscosity = m_pWPolar->m_Viscosity;
				m_pWing2->m_Density   = m_pWPolar->m_Density;
				m_pWing2->VLMComputeWing(m_Gamma+i*m_VLMMatSize+m_pWing->m_VLMMatSize,
										m_Ai+m_pWing->m_NStation,
										m_Cp+m_pWing->m_VLMMatSize,
										WingLift, WingIDrag, VDrag, XCP, YCP, LinPm, Rm, IYm, GYm,
										m_pWPolar->m_bViscous);
				Lift  += WingLift;
				IDrag += WingIDrag;

				m_pWing2->VLMSetBending();
				if(m_pWing2->m_bWingOut) m_bPointOut = true;

				Station += m_pWing2->m_NStation;
			}

			if(m_pStab) {
				AddString("       Calculating elevator...\r\n");
				WingLift  = 0.0;
				WingIDrag = 0.0;
				m_pStab->m_Alpha     = m_Alpha;
				m_pStab->m_QInf      = m_QInf;
				m_pStab->m_Viscosity = m_pWPolar->m_Viscosity;
				m_pStab->m_Density   = m_pWPolar->m_Density;

				pos = m_pWing->m_VLMMatSize;
				if(m_pWing2)	pos += m_pWing2->m_VLMMatSize;

				m_pStab->VLMComputeWing(m_Gamma+i*m_VLMMatSize+pos,
										m_Ai+Station,
										m_Cp+pos,
										WingLift, WingIDrag, VDrag, XCP, YCP, LinPm, Rm, IYm, GYm,
										m_pWPolar->m_bViscous);
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

				pos = m_pWing->m_VLMMatSize;
				if(m_pWing2)	pos += m_pWing2->m_VLMMatSize;
				if(m_pStab)		pos += m_pStab->m_VLMMatSize;
				
				m_pFin->VLMComputeWing( m_Gamma+i*m_VLMMatSize+pos,
										m_Ai+Station,
										m_Cp+pos,
										WingLift, WingIDrag, VDrag, XCP, YCP, LinPm, Rm, IYm, GYm,
										m_pWPolar->m_bViscous);
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

			VLMSetDownwash(m_Gamma+i*m_VLMMatSize);

			m_IYm         =        IYm   / m_pWing->m_Area/m_pWing->m_Span;
			m_GYm         =  2.0 * GYm   / m_pWing->m_Area/m_pWing->m_Span  /m_QInf/m_QInf;
			m_Rm          = -2.0 * Rm    / m_pWing->m_Area/m_pWing->m_Span  /m_QInf/m_QInf;
			m_GCm         =  2.0 * LinPm / m_pWing->m_Area/m_pWing->m_MAChord/m_QInf/m_QInf;
			m_VCm         =        m_pWing->m_VCm;
			m_TCm         =  m_GCm + m_pWing->m_VCm;

			if(m_pWing2)	m_TCm += m_pWing2->m_VCm;
			if(m_pStab)		m_TCm += m_pStab->m_VCm;
			if(m_pFin)		m_TCm += m_pFin->m_VCm;

			if(!m_pWPolar->m_bMiddle) 
				VLMComputeTBCp(m_Gamma+i*m_VLMMatSize);

			if(m_bPointOut) m_bWarning = true;
			if(m_bConverged){
				if(m_pPlane)
					pMiarex->AddPOpp(m_Gamma+i*m_VLMMatSize);
				else
					pMiarex->AddWOpp(m_bPointOut, m_Gamma+i*m_VLMMatSize);		
			}
			AddString("     \r\n");
		}
		else m_bPointOut = true;
	}
}

void CVLMDlg::VLMComputeTBCp(double *Gamma)
{
	CVector Qp;// local speed at panel control point
	double Speed2;
	for (int p=0; p<m_VLMMatSize; p++){
//TRACE("%d    C.x=%.4f    C.y=%.4f    C.z=%.4f\n", p, m_pPanel[p].CtrlPt.x, m_pPanel[p].CtrlPt.y, m_pPanel[p].CtrlPt.z);
//TRACE("Gamma[%d] = %10.3f\n",p, Gamma[p]);

		Qp.Set(m_QInf,0.0,0.0);
		for (int pp=0; pp<m_VLMMatSize;pp++){
			VLMGetVortexInfluence(pp,m_pPanel[p].CtrlPt, V, true);
//TRACE("%d    Qp.x=%f    Qp.y=%f    Qp.z=%f\n", p,V.x, V.y, V.z);
			V  *= Gamma[pp];
			Qp += V;
		}
//TRACE("%d    Qp.x=%.4f    Qp.y=%.4f    Qp.z=%.4f\n", p, Qp.x, Qp.y, Qp.z);

//		TRACE("%d length = %.5f\n", p, Qp.VAbs());

		Speed2 = Qp.x*Qp.x + Qp.y*Qp.y + Qp.z*Qp.z;
		m_Cp[p] = 1.0-Speed2/m_QInf/m_QInf;
//		TRACE("p=%d   Speed2 = %.5f,  QInf=%.5f  Cp=%.4f\n", p, Speed2, m_QInf, m_Cp[p]);
	}
}

void CVLMDlg::VLMRotateGeomY(double Angle, CVector P)
{
//angle is in degree
	double cosa = cos(Angle*3.141592654/180.0);
	double sina = sin(Angle*3.141592654/180.0);
	int iLA, iLB, iTA, iTB;
	CVector LATB, TALB;

	for (int s=0; s< m_NSurfaces; s++){
		m_pSurface[s].m_LA.RotateY(P, Angle);
		m_pSurface[s].m_TA.RotateY(P, Angle);
		m_pSurface[s].m_LB.RotateY(P, Angle);
		m_pSurface[s].m_TB.RotateY(P, Angle);

	}
	for (int n=0; n< m_nNodes; n++){
		m_pNode[n].RotateY(P, Angle);
/*		x = P.x + (m_pNode[n].x-P.x)*cosa + (m_pNode[n].z - P.z)*sina;
		z = P.z - (m_pNode[n].x-P.x)*sina + (m_pNode[n].z - P.z)*cosa;
		m_pNode[n].x = x;
		m_pNode[n].z = z;*/
	}
	for (int p=0; p< m_VLMMatSize; p++){
		m_pPanel[p].Normal.RotateY(P, Angle);

		iLA = m_pPanel[p].m_iLA; iLB = m_pPanel[p].m_iLB;
		iTA = m_pPanel[p].m_iTA; iTB = m_pPanel[p].m_iTB;

		LATB.x = m_pNode[iLA].x - m_pNode[iTB].x;
		LATB.y = m_pNode[iLA].y - m_pNode[iTB].y;
		LATB.z = m_pNode[iLA].z - m_pNode[iTB].z;
		TALB.x = m_pNode[iTA].x - m_pNode[iLB].x;
		TALB.y = m_pNode[iTA].y - m_pNode[iLB].y;
		TALB.z = m_pNode[iTA].z - m_pNode[iLB].z;

		CrossProduct(LATB, TALB, m_pPanel[p].Normal);

//		m_pPanel[p].Area =  m_pPanel[p].Normal.VAbs()/2.0;
		m_pPanel[p].Normal.Normalize();
		m_pPanel[p].SetVortex(m_pNode[iLA], m_pNode[iLB], m_pNode[iTA], m_pNode[iTB]);
		m_pPanel[p].SetFrame(m_pNode[iLA], m_pNode[iLB], m_pNode[iTA], m_pNode[iTB]);
	}
}


CVector CVLMDlg::GetSpeedVector(CVector C, double *Gamma)
{
	int pp;
	CVector VTot;
	VTot.Set(0.0,0.0,0.0);
	for (pp=0; pp<m_VLMMatSize;pp++){
		VLMGetVortexInfluence(pp, C, V, true);
		VTot += V * Gamma[pp];
	}
	return VTot;
}


void CVLMDlg::VLMGetVortexInfluence(int p, CVector C, CVector &V, bool bAll, bool bGround, double Height)
{
	// calculates the the panel p's vortex influence at point C
	// V is the resulting velocity
	int j, k;
	V.x = 0.0;
	V.y = 0.0;
	V.z = 0.0;

	if(m_pWPolar->m_bVLM1) {//just get the horseshoe vortex's influence
		VLMCmn(m_pPanel[p].A, m_pPanel[p].B, C, V, bAll);
		if(m_pWPolar->m_bGround) {
			AA = m_pPanel[p].A;
			BB = m_pPanel[p].B;
			AA.z -= 2.0*m_pWPolar->m_Height;
			BB.z -= 2.0*m_pWPolar->m_Height;
			VLMCmn(AA, BB, C, VG, bAll);
			V.x += VG.x;
			V.y += VG.y;
			V.z -= VG.z;
		}
	}
	else{
		// we have quad vortices
		// so we follow Katz and Plotkin's lead
		if(!m_pPanel[p].m_bIsTrailing){
			if(bAll) {
				if(m_pPanel[p].m_iPos <=0) {
					VLMQmn(m_pPanel[p].A, m_pPanel[p].B, m_pPanel[p-1].A, m_pPanel[p-1].B, C, V);
					if(m_pWPolar->m_bGround) {
						AA  = m_pPanel[p].A;
						BB  = m_pPanel[p].B;
						AA1 = m_pPanel[p-1].A;
						BB1 = m_pPanel[p-1].B;
						AA.z  -= 2.0*m_pWPolar->m_Height;
						BB.z  -= 2.0*m_pWPolar->m_Height;
						AA1.z -= 2.0*m_pWPolar->m_Height;
						BB1.z -= 2.0*m_pWPolar->m_Height;
						VLMQmn(AA, BB, AA1, BB1, C, VG);
						V.x += VG.x;
						V.y += VG.y;
						V.z -= VG.z;
					}
				}
				else
					VLMQmn(m_pPanel[p].A, m_pPanel[p].B, m_pPanel[p+1].A, m_pPanel[p+1].B, C, V);
			}
		}
		else {// then panel p is trailing and shedding a wake
			//is there a wake ?
			if(!m_bWakeRollUp){//if not
				// since Panel p+1 does not exist... 
				// we define the points AA=A+1 and BB=B+1
				AA1.x = m_pNode[m_pPanel[p].m_iTA].x + (m_pNode[m_pPanel[p].m_iTA].x-m_pPanel[p].A.x)/3.0;
				AA1.y = m_pNode[m_pPanel[p].m_iTA].y;
				AA1.z = m_pNode[m_pPanel[p].m_iTA].z;
				BB1.x = m_pNode[m_pPanel[p].m_iTB].x + (m_pNode[m_pPanel[p].m_iTB].x-m_pPanel[p].B.x)/3.0;
				BB1.y = m_pNode[m_pPanel[p].m_iTB].y;
				BB1.z = m_pNode[m_pPanel[p].m_iTB].z;
				// first we get the quad vortex's influence
				if (bAll) {
					VLMQmn(m_pPanel[p].A, m_pPanel[p].B, AA1, BB1, C, V);
					if(m_pWPolar->m_bGround) {
						AA  = m_pPanel[p].A;
						BB  = m_pPanel[p].B;
						AAG = AA1;
						BBG = BB1;
						AA.z  -= 2.0*m_pWPolar->m_Height;
						BB.z  -= 2.0*m_pWPolar->m_Height;
						AAG.z -= 2.0*m_pWPolar->m_Height;
						BBG.z -= 2.0*m_pWPolar->m_Height;

						VLMQmn(AA, BB, AAG, BBG, C, VG);
						V.x += VG.x;
						V.y += VG.y;
						V.z -= VG.z;
					}
				}
				//we just add a trailing horseshoe vortex's influence to simulate the wake
				VLMCmn(AA1,BB1,C,VT,bAll);
				if(m_pWPolar->m_bGround) {
					VLMCmn(AAG, BBG, C, VG);
					V.x += VG.x;
					V.y += VG.y;
					V.z -= VG.z;
				}
				V += VT;
			}
			else{// if there is
				//first we find which trailing strip and trailing point we are talking about
				int pc = -1;
				int pw =  0;
				for (j=0; j<m_NSurfaces; j++){
					for (k=0; k<m_pSurface[j].m_NYPanels; k++){
						pc += m_pSurface[j].m_NXPanels;
						if(pc==p) break;
						pw += m_NXWakePanels;
					}
					if(pc==p) break;
				}
				// now we can calculate the wing's trailing vortex influence
				if (bAll) {
					VLMQmn(m_pPanel[p].A, m_pPanel[p].B, m_pWakePanel[pw].A, m_pWakePanel[pw].B, C, V);
					if(m_pWPolar->m_bGround) {
						AA  = m_pPanel[p].A;
						BB  = m_pPanel[p].B;
						AAG = m_pWakePanel[pw].A;
						BBG = m_pWakePanel[pw].B;
						AA.z  -= 2.0*m_pWPolar->m_Height;
						BB.z  -= 2.0*m_pWPolar->m_Height;
						AAG.z -= 2.0*m_pWPolar->m_Height;
						BBG.z -= 2.0*m_pWPolar->m_Height;

						VLMQmn(AA, BB, AAG, BBG, C, VG);
						V.x += VG.x;
						V.y += VG.y;
						V.z -= VG.z;
					}
				}

				//each wake panel has the same vortex strength than the T.E. panel
				//so we just cumulate their unit influences
				if(bAll){
					for (int lw=0; lw<m_NXWakePanels-1; lw++){
						VLMQmn(m_pWakePanel[pw  ].A, m_pWakePanel[pw  ].B,
							   m_pWakePanel[pw+1].A, m_pWakePanel[pw+1].B, C, VT);
						V += VT;
						if(m_pWPolar->m_bGround) {
							AA  = m_pWakePanel[pw].A;
							BB  = m_pWakePanel[pw].B;
							AAG = m_pWakePanel[pw+1].A;
							BBG = m_pWakePanel[pw+1].B;
							AA.z  -= 2.0*m_pWPolar->m_Height;
							BB.z  -= 2.0*m_pWPolar->m_Height;
							AAG.z -= 2.0*m_pWPolar->m_Height;
							BBG.z -= 2.0*m_pWPolar->m_Height;

							VLMQmn(AA, BB, AAG, BBG, C, VG);
							V.x += VG.x;
							V.y += VG.y;
							V.z -= VG.z;
						}
						pw++;
					}
				}
				//for the very last wake panel downstream, just add a horseshoe vortex influence
				VLMCmn(m_pWakePanel[pw].A, m_pWakePanel[pw].B,C,VT,bAll);
				V += VT;
				if(m_pWPolar->m_bGround) {
					VLMCmn(AAG, BBG, C, VG);
					V.x += VG.x;
					V.y += VG.y;
					V.z -= VG.z;
				}
				//simple really !
			}
			//so says Katz and Plotkin !
		}
	}
}


void CVLMDlg::VLMSetAi(double *Gamma)
{
	// calculates the induced angles from the vortices strengths
	CVector C, CG;
	double wg;
	int j, k, pp, m;
	m=0;

	if(m_bTrace) AddString("     Calculating induced angles...\r\n");

	for (j=0; j<m_NSurfaces; j++){
		for (k=0; k<m_pSurface[j].m_NYPanels; k++){

			// André's very own method
			// Calculate the downwash far downstream... sort of a Trefftz plane
			// The other option is to calculate the downwash at the trailing edge ?

			m_pSurface[j].GetTrailingPt(C, k);
			C.x = 10.0*m_pWing->m_Span;
			CG.x = C.x;
			CG.y = C.y;
			CG.z = C.z-2.0*m_pWPolar->m_Height;
			wg  = 0.0;

			for (pp=0; pp<m_VLMMatSize; pp++){
				VLMGetVortexInfluence(pp, C, V, true);
				wg += Gamma[pp]*V.z;
			}
			m_Ai[m]       = atan(wg/m_QInf) * 180.0/pi;
			m++;
		}
	}
}


void CVLMDlg::VLMSetDownwash(double *Gamma)
{
	// calculates the induced angles from the vortices strengths
	int j, k, pp, m, nSurf;
	CVector C;
	m=0;
	nSurf = 0;
	//wing first
	memset(m_pWing->m_Vd, 0, sizeof(m_pWing->m_Vd));
	for (j=0; j<m_pWing->m_NSurfaces; j++){
		for (k=0; k<m_pWing->m_Surface[j].m_NYPanels; k++){
			m_pSurface[j].GetTrailingPt(C, k);

			for (pp=0; pp<m_VLMMatSize; pp++){			
				VLMGetVortexInfluence(pp,C,V,false);
				m_pWing->m_Vd[m].x += Gamma[pp]*V.x;// is zero anyways
				m_pWing->m_Vd[m].y += Gamma[pp]*V.y;
				m_pWing->m_Vd[m].z += Gamma[pp]*V.z;
			}
			m++;
		}
		nSurf++;
	}

	m=0;
	if(m_pWing2){
		memset(m_pWing2->m_Vd, 0, sizeof(m_pWing2->m_Vd));
		for (j=0; j<m_pWing2->m_NSurfaces; j++){
			for (k=0; k<m_pWing2->m_Surface[j].m_NYPanels; k++){
				m_pSurface[j+nSurf].GetTrailingPt(C, k);

				for (pp=0; pp<m_VLMMatSize; pp++){			
					VLMGetVortexInfluence(pp,C,V,false);
					m_pWing2->m_Vd[m].x += Gamma[pp]*V.x;// is zero anyways
					m_pWing2->m_Vd[m].y += Gamma[pp]*V.y;
					m_pWing2->m_Vd[m].z += Gamma[pp]*V.z;
				}
				m++;
			}
		}
		nSurf += m_pWing2->m_NSurfaces;
	}

	m=0;
	if(m_pStab){
		memset(m_pStab->m_Vd, 0, sizeof(m_pStab->m_Vd));
		for (j=0; j<m_pStab->m_NSurfaces; j++){
			for (k=0; k<m_pStab->m_Surface[j].m_NYPanels; k++){
				m_pSurface[j+nSurf].GetTrailingPt(C, k);

				for (pp=0; pp<m_VLMMatSize; pp++){			
					VLMGetVortexInfluence(pp,C,V,false);
					m_pStab->m_Vd[m].x += Gamma[pp]*V.x;// is zero anyways
					m_pStab->m_Vd[m].y += Gamma[pp]*V.y;
					m_pStab->m_Vd[m].z += Gamma[pp]*V.z;
				}
				m++;
			}
		}
		nSurf += m_pStab->m_NSurfaces;
	}

	m=0;
	if(m_pFin){
		memset(m_pFin->m_Vd, 0, sizeof(m_pFin->m_Vd));
		for (j=0; j<m_pFin->m_NSurfaces; j++){
			for (k=0; k<m_pFin->m_Surface[j].m_NYPanels; k++){
				m_pSurface[j+nSurf].GetTrailingPt(C, k);

				for (pp=0; pp<m_VLMMatSize; pp++){			
					VLMGetVortexInfluence(pp,C,V,false);
					m_pFin->m_Vd[m].x += Gamma[pp]*V.x;// is zero anyways
					m_pFin->m_Vd[m].y += Gamma[pp]*V.y;
					m_pFin->m_Vd[m].z += Gamma[pp]*V.z;
				}
				m++;
			}
		}
	}
}

void CVLMDlg::VLMResetWakeNodes()
{
	memcpy(m_pWakeNode, m_WakeNode, sizeof(m_WakeNode));
}

void CVLMDlg::VLMMoveWakeNodes()
{
	int nPiter = m_WakeInterNodes;
	
	int size = m_VLMMatSize;
	double *Gamma = m_Gamma;

	// calculates the induced velocity at the wake panel points 
	// and realigns the panel's sides with the local flow vector
	// we move all the wake nodes except for the leading nodes which remain at the wing's T.E.
	CVector LATB, TALB, Trans, PP;
	CVector WLA, WLB,WTA,WTB, WTemp;//wake panel's leading corner points
	int j, k, lw, llw, pp;
	double t, dx;
	//METHOD A.1 / B.2 + André's method
	//find the velocity at the panel's LE points
	// and align the panel's side

	int mw=0;
	for (j=0; j<m_NSurfaces; j++){//All surfaces
		if(m_bCancel) break;
		for (k=0; k<m_pSurface[j].m_NYPanels; k++){
			if(m_bCancel) break;
			for (lw=0; lw <m_NXWakePanels; lw++){
				if(m_bCancel) break;
				//left point
				WLA.Copy(m_pWakeNode[m_pWakePanel[mw].m_iLA]);
				WTA.Copy(m_pWakeNode[m_pWakePanel[mw].m_iTA]);
				WTemp.Copy(WLA);
				dx  = (WTA.x-WLA.x)/(double)nPiter;

				for (llw=0; llw<nPiter-1; llw++){
					V.x = m_QInf;
					V.y = 0;
					V.z = 0;
					for(pp=0; pp<m_VLMMatSize; pp++) {
						VLMGetVortexInfluence(pp, WTemp,VT, true);
						VT *= Gamma[pp];
						V += VT;
//						VT.Multiply(Gamma[pp]);
//						V.Add(VT);
					}
					V.Normalize();
					t = dx/V.x;
					WTemp.x += dx;
					WTemp.y += V.y * t;
					WTemp.z += V.z * t;

				}
				PP.Copy(WTemp);
				//define the translation vector
				Trans.x = PP.x-WLA.x;
				Trans.y = PP.y-WLA.y;
				Trans.z = PP.z-WLA.z;
				
				//and move all the nodes downstream, i.e. all panels left nodes
				for (llw=0; llw <m_NXWakePanels-lw; llw++){
//					m_pWakeNode[m_pWakePanel[mw+llw].m_iTA].x += Trans.x;
					m_pWakeNode[m_pWakePanel[mw+llw].m_iTA].y += Trans.y;
					m_pWakeNode[m_pWakePanel[mw+llw].m_iTA].z += Trans.z;
				}

				mw++;
			}
		}
	}
	//finally do the same for the right side of the last panel
	mw -= m_NXWakePanels;
	for (lw=0; lw <m_NXWakePanels; lw++){
		WLB.Copy(m_pWakeNode[m_pWakePanel[mw].m_iLB]);
		WTB.Copy(m_pWakeNode[m_pWakePanel[mw].m_iTB]);
		WTemp.Copy(WLB);
		dx  = (WTB.x-WLB.x)/(double)nPiter;

		for (llw=0; llw<nPiter-1; llw++){
			V.x = m_QInf;
			V.y = 0;
			V.z = 0;
			for(pp=0; pp<m_VLMMatSize; pp++) {
				VLMGetVortexInfluence(pp, WTemp,VT, true);
				VT *= Gamma[pp];
				V  +=VT;
			}
			V.Normalize();
			t = dx/V.x;
			WTemp.x += dx;
			WTemp.y += V.y * t;
			WTemp.z += V.z * t;
		}
		PP.Copy(WTemp);
		//define the translation vector
		Trans.x = PP.x-WLB.x;
		Trans.y = PP.y-WLB.y;
		Trans.z = PP.z-WLB.z;
		
		//and move all the nodes downstream, i.e. all panels left nodes
		for (llw=0; llw <m_NXWakePanels-lw; llw++){
//			m_pWakeNode[m_pWakePanel[mw+llw].m_iTB].x += Trans.x;
			m_pWakeNode[m_pWakePanel[mw+llw].m_iTB].y += Trans.y;
			m_pWakeNode[m_pWakePanel[mw+llw].m_iTB].z += Trans.z;
		}
		mw++;
	}

	mw=0;
	for (j=0; j<m_NSurfaces; j++){//All surfaces
		for (k=0; k<m_pSurface[j].m_NYPanels; k++){
			for (int lw=0; lw <m_NXWakePanels; lw++){
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

				CrossProduct(LATB, TALB, m_pWakePanel[mw].Normal);

				m_pWakePanel[mw].Area =  m_pWakePanel[mw].Normal.VAbs()/2.0;
				m_pWakePanel[mw].Normal.Normalize();
				m_pWakePanel[mw].SetFrame(WLA, WLB, WTA, WTB);
				m_pWakePanel[mw].dl = m_pSurface[j].Getdl(k);

				mw++;
			}
		}
	}
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	pMiarex->m_bResetglWake = true;
	pMiarex->UpdateView();
}

/*
void CVLMDlg::VLMMoveWakeNodes(double *Gamma, int size)
{
	//calculates the induced velocity at the wake panel points and moves them
	//we move all the wake nodes except for the leading nodes which remain at the wing's T.E.
	CVLMDlg *pVLMDlg = (CVLMDlg*)m_pVLMDlg;

	int j, k, pp;
	int nw;
	double rearx = -1.0e99;
	for (j=0; j<m_NSurfaces; j++){//All surfaces
		rearx = max(m_Surface[j].m_TA.x, rearx);
		rearx = max(m_Surface[j].m_TB.x, rearx);
	}

	if(m_bTrace)
		pVLMDlg->AddString("     Modifying the wake panels...\r\n");

	//METHOD A.2 / B.2
	//find the velocity at the panel's corner points
	double K = 1.; //K&P eq. 15.2
	double dt, dxl; //K&P eq. 15.2
	for (nw=0; nw<m_nWakeNodes; nw++){
		if(VLMIsNode(m_pWakeNode[nw])<0){
			V.x = 0;
			V.y = 0;
			V.z = 0;

			for(pp=0; pp<m_VLMMatSize; pp++) {
				VLMGetVortexInfluence(pp, m_pWakeNode[nw],VT, true);
				VT.Multiply(Gamma[pp]);
				V.Add(VT);
//				if(nw==5)TRACE("%f  %f   %f \n", VT.x, VT.y, VT.z);
			}
			dxl = m_pWakeNode[nw].x - rearx;
			dt  = K * dxl/m_QInf;
			V.Multiply(dt);
//			V.x = 0;// keep the panels in the wake grid planes (optional)
//TRACE("%d  %f   %f   %f   \n",nw,V.x,V.y,V.z);
			m_pWakeNode[nw].Add(V);
		}
	}

//	TRACE("________\n");
	CVector LATB, TALB;
	CVector WLA, WLB, WTA,WTB;//wake panel's corner points

	int mw=0;
	for (j=0; j<m_NSurfaces; j++){//All surfaces
		for (k=0; k<m_Surface[j].m_NYPanels; k++){
			for (int lw=0; lw <m_NXWakePanels; lw++){
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

				CrossProduct(LATB, TALB, m_pWakePanel[mw].Normal);

				m_pWakePanel[mw].Area =  m_pWakePanel[mw].Normal.VAbs()/2.0;
				m_pWakePanel[mw].Normal.Normalize();
				m_pWakePanel[mw].SetVortex(WLA, WLB, WTA, WTB);
				m_pWakePanel[mw].SetControlPoint(WLA, WLB, WTA, WTB);
				m_pWakePanel[mw].dl = m_Surface[j].Getdl(k);

				mw++;
			}
		}
	}
}*/

/*
void CVLMDlg::VLMMoveWakeNodes(double *Gamma, int size)
{
	//calculates the induced velocity at the wake panel points and moves them
	//we move all the wake nodes except for the leading nodes which remain at the wing's T.E.
	CVLMDlg *pVLMDlg = (CVLMDlg*)m_pVLMDlg;
	CVector C[100];
	int j, k, pp;
	int pw, lw;
	double rearx = -1.0e99;
	for (j=0; j<m_NSurfaces; j++){//All surfaces
		rearx = max(m_Surface[j].m_TA.x, rearx);
		rearx = max(m_Surface[j].m_TB.x, rearx);
	}

	if(m_bTrace)
		pVLMDlg->AddString("     Modifying the wake panels...\r\n");

	for(pw=0; pw<m_VLMWakeSize; pw++){
		C[pw].x = (m_pWakeNode[m_pWakePanel[pw].m_iTA].x + m_pWakeNode[m_pWakePanel[pw].m_iTB].x)/2.;
		C[pw].y = (m_pWakeNode[m_pWakePanel[pw].m_iTA].y + m_pWakeNode[m_pWakePanel[pw].m_iTB].y)/2.;
		C[pw].z = (m_pWakeNode[m_pWakePanel[pw].m_iTA].z + m_pWakeNode[m_pWakePanel[pw].m_iTB].z)/2.;
	}

	//METHOD 1
	//fin the velocity at panel's TE mid points, and intepolate the rest
	double K = 1.0; //K&P eq. 15.2
	double dt, dxl; //K&P eq. 15.2
	pw = 0;
	for (j=0; j <m_NSurfaces; j++){
		for (k=0; k<m_Surface[j].m_NYPanels; k++){
			for(lw=0; lw<m_NXWakePanels; lw++){
				V.x = 0;
				V.y = 0;
				V.z = 0;

				for(pp=0; pp<m_VLMMatSize; pp++) {
					VLMGetVortexInfluence(pp, C[pw],VT, true);
					VT.Multiply(Gamma[pp]);
					V.Add(VT);
				}
				dxl = C[pw].x - rearx;
				dt  = K * dxl/m_QInf;
				V.Multiply(dt);
				V.x = 0;// keep the panels in the wake grid planes (optional)
				
				if(j==0 && k==0) {
					m_pWakeNode[m_pWakePanel[pw].m_iTA].y += V.y;
					m_pWakeNode[m_pWakePanel[pw].m_iTA].z += V.z;
				}
				else {
					m_pWakeNode[m_pWakePanel[pw].m_iTA].y += V.y/2.0;
					m_pWakeNode[m_pWakePanel[pw].m_iTA].z += V.z/2.0;
				}
				if(j==m_NSurfaces-1 && k==m_Surface[j].m_NYPanels-1) {
					m_pWakeNode[m_pWakePanel[pw].m_iTB].y += V.y;
					m_pWakeNode[m_pWakePanel[pw].m_iTB].z += V.z;
				}
				else {
					m_pWakeNode[m_pWakePanel[pw].m_iTB].y += V.y/2.0;
					m_pWakeNode[m_pWakePanel[pw].m_iTB].z += V.z/2.0;
				}
				pw++;
			}
		}
	}
}*/

void CVLMDlg::VLMQmn(CVector LA, CVector LB, CVector TA, CVector TB, CVector C, CVector &V)
{//Quadrilateral VLM FORMULATION
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
	// if !bAll only the effect of the side vortices LA-TA and LB-TB is taken into account

	double pi  = 3.141592654;
	double Omega;
	double ftmp;

	CVector Psi;
	CVector r0, r1, r2;

	V.x = 0.0;
	V.y = 0.0;
	V.z = 0.0;

	//Leading finite vortex contribution
	r0.x = LB.x - LA.x;
	r0.y = LB.y - LA.y;
	r0.z = LB.z - LA.z;
	r1.x = C.x - LA.x;
	r1.y = C.y - LA.y;
	r1.z = C.z - LA.z;
	r2.x = C.x - LB.x;
	r2.y = C.y - LB.y;
	r2.z = C.z - LB.z;
	CrossProduct(r1,r2,Psi);
	ftmp = Psi.VAbs();
	if(r1.VAbs()>m_CoreSize && r2.VAbs()>m_CoreSize && ftmp>m_CoreSize){
		ftmp *= ftmp;
		Psi.x /= ftmp;
		Psi.y /= ftmp;
		Psi.z /= ftmp;
		Omega = DotProduct(r0,r1)/r1.VAbs() - DotProduct(r0,r2)/r2.VAbs();
		V.x += Psi.x * Omega/4.0/pi;
		V.y += Psi.y * Omega/4.0/pi;
		V.z += Psi.z * Omega/4.0/pi;
	}
		
	//Trailing finite vortex contribution
	r0.x = TA.x - TB.x;
	r0.y = TA.y - TB.y;
	r0.z = TA.z - TB.z;
	r1.x = C.x - TB.x;
	r1.y = C.y - TB.y;
	r1.z = C.z - TB.z;
	r2.x = C.x - TA.x;
	r2.y = C.y - TA.y;
	r2.z = C.z - TA.z;
	CrossProduct(r1,r2,Psi);
	ftmp = Psi.VAbs();
	if(r1.VAbs()>m_CoreSize && r2.VAbs()>m_CoreSize && ftmp>m_CoreSize){
		ftmp *= ftmp;
		Psi.x /= ftmp;
		Psi.y /= ftmp;
		Psi.z /= ftmp;
		Omega = DotProduct(r0,r1)/r1.VAbs() - DotProduct(r0,r2)/r2.VAbs();
		V.x += Psi.x * Omega/4.0/pi;
		V.y += Psi.y * Omega/4.0/pi;
		V.z += Psi.z * Omega/4.0/pi;
	}

	//Next add 'right' finite contribution
	r0.x = TB.x - LB.x;
	r0.y = TB.y - LB.y;
	r0.z = TB.z - LB.z;
	r1.x = C.x - LB.x;
	r1.y = C.y - LB.y;
	r1.z = C.z - LB.z;
	r2.x = C.x - TB.x;
	r2.y = C.y - TB.y;
	r2.z = C.z - TB.z;
	CrossProduct(r1,r2,Psi);
	ftmp = Psi.VAbs();
	if(r1.VAbs()>m_CoreSize && r2.VAbs()>m_CoreSize && ftmp>m_CoreSize){
		ftmp *= ftmp;
		Psi.x /= ftmp;
		Psi.y /= ftmp;
		Psi.z /= ftmp;
		Omega = DotProduct(r0,r1)/r1.VAbs() - DotProduct(r0,r2)/r2.VAbs();
		V.x += Psi.x * Omega/4.0/pi;
		V.y += Psi.y * Omega/4.0/pi;
		V.z += Psi.z * Omega/4.0/pi;
	}

	//Last add 'left' finite contribution
	r0.x = LA.x - TA.x;
	r0.y = LA.y - TA.y;
	r0.z = LA.z - TA.z;
	r1.x = C.x - TA.x;
	r1.y = C.y - TA.y;
	r1.z = C.z - TA.z;
	r2.x = C.x - LA.x;
	r2.y = C.y - LA.y;
	r2.z = C.z - LA.z;
	CrossProduct(r1,r2,Psi);
	ftmp = Psi.VAbs();
	if(r1.VAbs()>m_CoreSize && r2.VAbs()>m_CoreSize && ftmp>m_CoreSize){
		ftmp *= ftmp;
		Psi.x /= ftmp;
		Psi.y /= ftmp;
		Psi.z /= ftmp;
		Omega = DotProduct(r0,r1)/r1.VAbs() - DotProduct(r0,r2)/r2.VAbs();
		V.x += Psi.x * Omega/4.0/pi;
		V.y += Psi.y * Omega/4.0/pi;
		V.z += Psi.z * Omega/4.0/pi;
	}
}




void CVLMDlg::VLMCmn(CVector A, CVector B, CVector C, CVector &V, bool bAll)
{
	//CLASSIC VLM FORMULATION
	double pi  = 3.141592654;
	// creates the VLM matrix coefficient
	// corresponding to the influence of the vortex located at segment AB 
	// at the tangential point C
	double Omega;
	double ftmp;
	CVector Psi;
	CVector r0, r1, r2;

	V.x = 0.0;
	V.y = 0.0;
	V.z = 0.0;

	if(bAll){
		r0.x = B.x - A.x;
		r0.y = B.y - A.y;
		r0.z = B.z - A.z;

		//First calculate finite vortex contribution
		r1.x = C.x - A.x;
		r1.y = C.y - A.y;
		r1.z = C.z - A.z;

		r2.x = C.x - B.x;
		r2.y = C.y - B.y;
		r2.z = C.z - B.z;
		
		CrossProduct(r1,r2,Psi);
		ftmp = Psi.VAbs();
		if(r1.VAbs()>m_CoreSize && r2.VAbs()>m_CoreSize && ftmp>m_CoreSize){
			ftmp *= ftmp;
			Psi.x /= ftmp;
			Psi.y /= ftmp;
			Psi.z /= ftmp;

			Omega = DotProduct(r0,r1)/r1.VAbs() - DotProduct(r0,r2)/r2.VAbs();

			V.x = Psi.x * Omega/4.0/pi;
			V.y = Psi.y * Omega/4.0/pi;
			V.z = Psi.z * Omega/4.0/pi;
		}
	}
	
	//Next add 'left'  semi-infinite contribution
	//eq.6-56
	ftmp = (C.z-A.z)*(C.z-A.z) + (A.y-C.y) * (A.y-C.y);
	if(ftmp>m_CoreSize){
		Psi.x = 0.0;//? unused...
		Psi.y = (C.z-A.z)/ftmp;
		Psi.z = (A.y-C.y)/ftmp;

		ftmp = sqrt((C.x-A.x)*(C.x-A.x) + (A.y-C.y)*(A.y-C.y) + (C.z-A.z)*(C.z-A.z));
		if(ftmp>m_CoreSize){
			Omega = 1.0 + (C.x-A.x)/ftmp;
			V.y += Psi.y * Omega/4.0/pi;
			V.z += Psi.z * Omega/4.0/pi;
		}
	}

	//Last add 'right' semi-infinite contribution
	//eq.6-57
	ftmp = (C.z-B.z)*(C.z-B.z) + (B.y-C.y) * (B.y-C.y);
	if(ftmp>m_CoreSize){
		Psi.x = 0.0;//? unused...
		Psi.y = (C.z-B.z)/ftmp;
		Psi.z = (B.y-C.y)/ftmp;
		ftmp = sqrt((C.x-B.x)*(C.x-B.x) + (B.y-C.y)*(B.y-C.y) + (C.z-B.z)*(C.z-B.z));
		if(ftmp>m_CoreSize){
			Omega = 1.0 + (C.x-B.x)/ftmp;
			V.y -= Psi.y * Omega/4.0/pi;
			V.z -= Psi.z * Omega/4.0/pi;
		}
	}

/*	else {//we need to re-align the trailing vortices with the flow direction
		t.Set(cos(m_pWPolar->m_Beta*pi/180.0), sin(m_pWPolar->m_Beta*pi/180.0), 0.0);

		//left trailing vortex
		Far = A + t * 1000000.0 * (B-A).VAbs();
		r0 = Far - A;
		r1 = C  - A;
		r2 = C  - Far ;
		u  = r1 * r2;
		uv = u.VAbs();
		VL = u /uv/uv/4.0/pi;
		r1.Normalize();
		r2.Normalize();
		VL *= r0.dot(r1-r2);
		
		//right trailing vortex
		Far = B + t * 1000000.0 * (B-A).VAbs();
		r0 = Far - B;
		r1 = C  - B;
		r2 = C  - Far ;
		u  = r1 * r2;
		uv = u.VAbs();
		VR = u /uv/uv/4.0/pi;
		r1.Normalize();
		r2.Normalize();
		VR *= r0.dot(r1-r2);
	
		V +=  VR-VL;
	}*/
}

