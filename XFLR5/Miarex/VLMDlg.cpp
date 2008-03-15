/****************************************************************************

    CVLMDlg Class
    Copyright (C) 2005-2007 André Deperrois xflr5@yahoo.com

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
	m_bTrefftz       = false;

	m_MatSize        = 0;
	m_nNodes         = 0;
	m_NSurfaces      = 0;
	m_MaxWakeIter    = 5;
	m_WakeInterNodes = 6;
	m_NWakeColumn    = 0;

	m_nWakeNodes = 0;
	m_WakeSize   = 0;

	m_QInf       = 0.0;
	m_QInfMax    = 0.0;
	m_DeltaQInf  = 0.0;

	m_Alpha      = 0.0;
	m_AlphaMax   = 0.0;
	m_DeltaAlpha = 0.0;

	m_OpAlpha  = 0.0;//calculation on a tilted geometry
//	*m_pCoreSize   = 0.0;

	m_XCP = 0.0;
	m_YCP = 0.0;
	m_VCm = 0.0;
	m_GCm = 0.0;
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

	memset(m_Cp,0,sizeof(m_Cp));
	memset(m_VLMQInf,0,sizeof(m_VLMQInf));
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

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
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

	if(m_pWPolar && m_pWPolar->m_bTiltedGeom)
	{
		//back-up the current geometry if the analysis is to be performed on the tilted geometry
		memcpy(m_pMemPanel, m_pPanel, m_MatSize * sizeof(CPanel));
		memcpy(m_pMemNode,  m_pNode,  m_nNodes * sizeof(CVector));
	}
	memcpy(m_pRefWakeNode, m_pWakeNode, sizeof(m_pRefWakeNode));

	SetFileHeader();

	str = "";
	m_bVLMSymetric = m_pWing->m_bSymetric;
	if(!m_pWing->m_bSymetric) str += "     Main wing is asymmetric\r\n";

	if(m_pWing2)
	{
		if(!m_pWing2->m_bSymetric)
		{
			m_bVLMSymetric = false;
			str += "     2nd wing is asymmetric\r\n";
		}
	}
	if(m_pStab)  
	{
		if(!m_pStab->m_bSymetric)
		{
			m_bVLMSymetric = false;
			str += "     Elevator is asymmetric\r\n";
		}
	}

	if(m_pFin)   
	{
		m_bVLMSymetric = false;
		str += "     A fin is considered asymmetric\r\n";
	}

	if (m_bVLMSymetric) AddString("Perfoming symmetric calculation\r\n");
	else 
	{
		str = "Performing asymmetric calculation : \r\n" + str;
		AddString(str);
	}
	AddString("\r\n");

	m_pWing->m_bVLMSymetric = m_bVLMSymetric;
	if(m_pWing2)  m_pWing2->m_bVLMSymetric = m_bVLMSymetric;
	if(m_pStab)   m_pStab->m_bVLMSymetric  = m_bVLMSymetric;
	if(m_pFin)    m_pFin->m_bVLMSymetric   = m_bVLMSymetric;

	if(m_pWPolar->m_AnalysisType==2){ //no way it could be anything else
		m_pVLMThread = new CVLMThread();
		m_pVLMThread->m_pParent = this;
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
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{ 
		OnCancel();
		return true;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}


bool CVLMDlg::Gauss(double *A, int n, double *B, int m)
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
	}

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
	}
	return true;
}

void CVLMDlg::OnCancel() 
{
	if(!m_pVLMThread) {
		EndSequence();
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
	if(m_pWPolar && m_pWPolar->m_bTiltedGeom)
	{
		//restore initial gemoetry
		memcpy(m_pPanel, m_pMemPanel, m_MatSize * sizeof(CPanel));
		memcpy(m_pNode,  m_pMemNode,  m_nNodes * sizeof(CVector));
	}

	if (m_bCancel) 
		AddString("\r\n\r\nAnalysis cancelled per user request....\r\n");

	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(pMiarex->m_bVLMFinished && !m_bXFile) return;
	
	pMiarex->m_bVLMFinished = true;
	m_bXFile = false;
	m_XFile.Close();
	if(m_bWarning && pMiarex->m_bLogFile && !m_bCancel){
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

bool CVLMDlg::VLMCreateRHS(double V0, double VDelta, int nval)
{
	int p, q, m, Size;
	double aoa, cosaoa, sinaoa;

	if(m_bVLMSymetric) Size = m_MatSize/2;
	else               Size = m_MatSize;

	//______________________________________________________________________________________
	// Create RHS
	if(m_pWPolar->m_Type!=4)
	{
		p=0;

		for (q=0; q<nval;q++)
		{
			aoa    =  (V0+q*VDelta)*pi/180.0;
			cosaoa = cos(aoa);
			sinaoa = sin(aoa);
			for (m=0; m<Size; m++)
			{
				m_RHS[p] = -cosaoa*m_ppPanel[m]->Normal.x  - sinaoa*m_ppPanel[m]->Normal.z;
				p++;
			}
		}
	}
	else 
	{
		//type 4... solve only once with unit speed
		aoa    = V0*pi/180.0;
		cosaoa = cos(aoa);
		sinaoa = sin(aoa);
		for (p=0;p<Size; p++)
		{
			m_RHS[p] = -cosaoa*m_ppPanel[p]->Normal.x  - sinaoa*m_ppPanel[p]->Normal.z;
		}
	}
	return true;
}

bool CVLMDlg::VLMCreateMatrix()
{
	int p,pp, Size;
	int pos = 0;
	CVector  CC;
	AddString("      Creating the influence matrix...\r\n");

	if(m_bVLMSymetric) Size = m_MatSize/2;
	else               Size = m_MatSize;

	for(p=0; p<Size; p++)
	{
		if(m_bCancel) break;
		CC = m_ppPanel[p]->CtrlPt;
		CC.y = -CC.y;

		for(pp=0; pp<Size; pp++)
		{
			if(m_bCancel) break;
			VLMGetVortexInfluence(m_ppPanel[pp], m_ppPanel[p]->CtrlPt, V, true);

			if(m_bVLMSymetric)
			{
				if(!m_ppPanel[pp]->m_bIsInSymPlane)
				{
					// add right wing contribution
					VLMGetVortexInfluence(m_ppPanel[pp], CC, VS, true);
					V.x += VS.x;
					V.y -= VS.y;
					V.z += VS.z;
				}
			}

			m_aij[p*Size+pp] = V.dot(m_ppPanel[p]->Normal);
		}
	}

	return true;
}


bool CVLMDlg::VLMSolveMultiple(double V0, double VDelta, int nval)
{
	//______________________________________________________________________________________
	// Method : 
	// 	- If the polar is of type 1 or 2, solve the linear system 
	//	  for all DeltaAlphas simultaneously, for a unit speed
	//	- If the polar is of type 4, solve only for unit speed and for the specified Alpha
	//	- Reconstruct right side results if calculation was symetric
	//	- Sort results i.a.w. panel numbering
	//______________________________________________________________________________________

	
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString strong, strange;
	int p, q, pp, Size,  nrhs;
	int n, o, o1, nel;
	double Lift, alpha;
	CVector N, Force, WindNormal;
	CVector VInf(1.0,0.0,0.0);
	double row[VLMMATSIZE];
	
	if(m_bVLMSymetric) Size = m_MatSize/2;
	else               Size = m_MatSize;

	memset(row,   0, sizeof(row));

	//______________________________________________________________________________________
	// Solve the system for unit vortex circulation

	AddString("      Solving the linear system...\r\n");

	if(m_pWPolar->m_Type!=4) nrhs = nval;
	else                     nrhs = 0;

	if(!Gauss(m_aij,Size, m_RHS, nrhs))
//	if(!GaussSeidel(m_aij,Size, m_RHS, row, 0.0001, 500))
	{
		AddString("      Singular Matrix.... Aborting calculation...\r\n");
		m_bConverged = false;
		return false;
	}
	else m_bConverged = true;

	//______________________________________________________________________________________
	// Calculate speeds i.a.w. polar types
	
	AddString("      Calculating the vortices circulations...\r\n");

	//so far we have a unit Vortex Strength
	if(m_pWPolar->m_Type==2)
	{
		//type 2; find the speeds which will create a lift equal to the weight
		AddString("      Calculating speeds to balance the weight\r\n\r\n");

		for (q=0; q<nval;q++)
		{
			alpha = V0+q*VDelta;
			WindNormal.Set(-sin(alpha*pi/180.0), 0.0, cos(alpha*pi/180.0));

			memcpy(row, m_RHS+q*Size, sizeof(row));
			Lift = 0.0;
			p=0;
			for (p=0; p<Size; p++)
			{
				// for each panel along the chord, add the lift coef
				if(m_pWPolar->m_bVLM1 || m_ppPanel[p]->m_bIsTrailing)
				{
					//changed v4.00 Leading-->Trailing
					Force = VInf * m_ppPanel[p]->Vortex;
					Lift += Force.dot(WindNormal) * row[p];
				}
				else 
				{
					Force = VInf * m_ppPanel[p]->Vortex;
					Lift += Force.dot(WindNormal) * row[p];
					Force = VInf * m_ppPanel[p-1]->Vortex;
					Lift -= Force.dot(WindNormal) * row[p];
				}
			}
			if(m_bVLMSymetric) Lift *=2.0;

			if(Lift<=0.0)
			{
				strong.Format("      Found a negative lift for Alpha=%.2f.... skipping the angle...\r\n", V0+q*VDelta);
				if(m_bTrace) AddString(strong);
				m_bPointOut = true;
				m_bWarning = true;
				m_VLMQInf[q] = -100.0;
			}
			else 
			{
				m_VLMQInf[q] =  sqrt( 9.81 * m_pWPolar->m_Weight /m_pWPolar->m_Density / Lift);
				strong.Format("      Alpha=%5.2f   QInf = %5.2f", V0+q*VDelta, m_VLMQInf[q]*pFrame->m_mstoUnit);
				GetSpeedUnit(strange, pFrame->m_SpeedUnit);
				strong+= strange + "\r\n";
				if(m_bTrace) AddString(strong);
			}
		}
	}

	else if (m_pWPolar->m_Type==1)
		for (q=0; q<nval;q++) m_VLMQInf[q] = m_pWPolar->m_QInf;

	else if (m_pWPolar->m_Type==4)
		for (q=0; q<nval;q++) m_VLMQInf[q] = V0 + q*VDelta;

	//______________________________________________________________________________________
	// Scale circulations to speeds

	p=0;
	for (q=0; q<nval;q++)
	{
		for(pp=0; pp<Size; pp++)
		{
			if(m_pWPolar->m_Type!=4)	m_Gamma[q*Size+pp] = m_RHS[q*Size+pp]*m_VLMQInf[q];
			else 						m_Gamma[q*Size+pp] = m_RHS[pp]*m_VLMQInf[q];
		}
	}
	//______________________________________________________________________________________
	//Reconstruct right side results

	double *GammaRef = m_aij;//use existing reserved memory, do not re-allocate
	if(m_bVLMSymetric)	
	{
		memcpy(GammaRef, m_Gamma, nval*Size*sizeof(double));

		for (q=0; q<nval;q++)
		{
			n  =  q    * Size;
			o  =  q    * m_MatSize;
			o1 = (q+1) * m_MatSize;

			for (p=0; p<m_MatSize/2; p++)
			{
				m_Gamma[o+p]    = GammaRef[n+p];
				m_Gamma[o1-1-p] = GammaRef[n+p];
			}
		}
	}

	//______________________________________________________________________________________
	//at this stage, m_Gamma is ordered as m_ppPanel[]... need to sort as m_pPanel

	memcpy(GammaRef, m_Gamma, nval*m_MatSize*sizeof(double));

	for (q=0; q<nval;q++)
	{
		o  =  q * m_MatSize;

		for (p=0; p<m_MatSize; p++)
		{
			nel = m_ppPanel[p]->m_iElement;
			m_Gamma[o+nel]    = GammaRef[o+p];
		}
	}
//memcpy(row, m_Gamma, m_MatSize*sizeof(double));
	return  true;
}

void CVLMDlg::VLMComputePlane(double V0, double VDelta, int nrhs)
{
	// calculates the various wing coefficients by interpolating
	// the adequate variable, from Cl, on the XFoil polar mesh
	// at each span station
	int q, pos, Station;
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	double Lift, IDrag, VDrag ,XCP, YCP, qdyn;
	double WingLift, WingIDrag, Alpha;
	CString str, strong;

	m_pWing->m_bTrace   = false;
	m_pWing->m_bVLM1    = m_pWPolar->m_bVLM1;
	m_pWing->m_bTrace   = true;
	m_pWing->m_pWakeNode  = m_pWakeNode;
	m_pWing->m_pWakePanel = m_pWakePanel;

	if(m_pWing2) 
	{
		m_pWing2->m_bTrace   = false;
		m_pWing2->m_bVLM1    = m_pWPolar->m_bVLM1;
		m_pWing2->m_bTrace   = true;
	}

	if(m_pStab) 
	{
		m_pStab->m_bTrace   = false;
		m_pStab->m_bVLM1    = m_pWPolar->m_bVLM1;
		m_pStab->m_bTrace   = true;
	}
	if(m_pFin)
	{
		m_pFin->m_bTrace   = false;
		m_pFin->m_bVLM1    = m_pWPolar->m_bVLM1;
		m_pFin->m_bTrace   = true;
	}
	for(q=0; q<nrhs; q++)
	{
		if(m_bCancel) break;
		m_pWing->m_bWingOut = false;
		if(m_pWing2)	m_pWing2->m_bWingOut = false;
		if(m_pStab)		m_pStab->m_bWingOut = false;
		if(m_pFin)		m_pFin->m_bWingOut  = false;

		if(m_pWPolar->m_bTiltedGeom)	Alpha = m_OpAlpha;
		else
		{
			if(m_pWPolar->m_Type!=4)
			{
				Alpha = V0+q*VDelta;
			}
			else          
			{
				Alpha = m_pWPolar->m_ASpec;
			}
			m_OpAlpha = Alpha;
		}

		m_QInf = m_VLMQInf[q];
		qdyn = 0.5 * m_pWPolar->m_Density * m_QInf * m_QInf;

		if(m_QInf >0.0) 
		{
			if(m_pWPolar->m_Type!=4 && !m_pWPolar->m_bTiltedGeom)
			{
				str.Format("      ...Alpha=%.2f\r\n", m_OpAlpha);
				AddString(str);
			}
			else if (m_pWPolar->m_Type==4) 
			{
				str.Format("   ...QInf = %6.2f ", m_QInf*pFrame->m_mstoUnit);
				GetSpeedUnit(strong, pFrame->m_SpeedUnit);
				str += strong + "\r\n";
				AddString(str);
			}

			VLMSetAi(m_Gamma+q*m_MatSize);

			AddString("        Calculating aerodynamic coefficients...\r\n");
			m_bPointOut          = false;
			m_pWing->m_Alpha     = Alpha;
			m_pWing->m_QInf      = m_QInf;
			m_pWing->m_Viscosity = m_pWPolar->m_Viscosity;
			m_pWing->m_Density   = m_pWPolar->m_Density;

			Lift   = 0.0;
			IDrag  = 0.0;
			VDrag  = 0.0;
			XCP    = 0.0;
			YCP    = 0.0;

			m_GRm                 = 0.0;
			m_GCm = m_VCm         = 0.0;
			m_GYm = m_VYm = m_IYm = 0.0;

			AddString("         Calculating wing...\r\n");
			m_pWing->VLMTrefftz(m_Gamma+q*m_MatSize, 0, Lift, IDrag, m_pWPolar->m_bTiltedGeom);
			m_pWing->VLMComputeWing(m_Gamma+q*m_MatSize, m_Cp, 
						VDrag, XCP, YCP, m_GCm, m_VCm, m_GRm, m_GYm, m_IYm, m_VYm, m_pWPolar->m_bViscous, m_pWPolar->m_bTiltedGeom);

			m_pWing->VLMSetBending();
			if(m_pWing->m_bWingOut)  m_bPointOut = true;
			pos = m_pWing->m_MatSize;

			Station = m_pWing->m_NStation;
			
			if(m_pWing2) 
			{
				AddString("       Calculating 2nd wing...\r\n");
				WingLift  = 0.0;
				WingIDrag = 0.0;
				m_pWing2->m_Alpha     = Alpha;
				m_pWing2->m_QInf      = m_QInf;
				m_pWing2->m_Viscosity = m_pWPolar->m_Viscosity;
				m_pWing2->m_Density   = m_pWPolar->m_Density;
				m_pWing2->VLMTrefftz(m_Gamma+q*m_MatSize, pos, WingLift, WingIDrag, m_pWPolar->m_bTiltedGeom);
				m_pWing2->VLMComputeWing(m_Gamma+q*m_MatSize+m_pWing->m_MatSize,
										m_Cp+m_pWing->m_MatSize,
										VDrag, XCP, YCP, m_GCm, m_VCm, m_GRm, m_GYm, m_IYm, m_VYm, 
										m_pWPolar->m_bViscous, m_pWPolar->m_bTiltedGeom);
				Lift  += WingLift;
				IDrag += WingIDrag;

				m_pWing2->VLMSetBending();
				if(m_pWing2->m_bWingOut) m_bPointOut = true;

				Station += m_pWing2->m_NStation;
				pos += m_pWing2->m_MatSize;
			}

			if(m_pStab) 
			{
				AddString("         Calculating elevator...\r\n");
				WingLift  = 0.0;
				WingIDrag = 0.0;
				m_pStab->m_Alpha     = Alpha;
				m_pStab->m_QInf      = m_QInf;
				m_pStab->m_Viscosity = m_pWPolar->m_Viscosity;
				m_pStab->m_Density   = m_pWPolar->m_Density;

				m_pStab->VLMTrefftz(m_Gamma+q*m_MatSize, pos, WingLift, WingIDrag, m_pWPolar->m_bTiltedGeom);
				m_pStab->VLMComputeWing(m_Gamma+q*m_MatSize+pos,
										m_Cp+pos,
										VDrag, XCP, YCP, m_GCm, m_VCm, m_GRm, m_GYm, m_IYm, m_VYm, 
										m_pWPolar->m_bViscous, m_pWPolar->m_bTiltedGeom);
				Lift  += WingLift;
				IDrag += WingIDrag;

				m_pStab->VLMSetBending();
				if(m_pStab->m_bWingOut) m_bPointOut = true;

				Station += m_pStab->m_NStation;
				pos += m_pStab->m_MatSize;
			}

			if(m_pFin)
			{
				AddString("         Calculating fin...\r\n");
				WingLift  = 0.0;
				WingIDrag = 0.0;
				m_pFin->m_Alpha      = Alpha;
				m_pFin->m_QInf       = m_QInf;
				m_pFin->m_Viscosity  = m_pWPolar->m_Viscosity;
				m_pFin->m_Density    = m_pWPolar->m_Density;
				
				m_pFin->VLMTrefftz(m_Gamma+q*m_MatSize, pos, WingLift, WingIDrag, m_pWPolar->m_bTiltedGeom);
				m_pFin->VLMComputeWing( m_Gamma+q*m_MatSize+pos,
										m_Cp+pos,
										VDrag, XCP, YCP, m_GCm, m_VCm, m_GRm, m_GYm, m_IYm, m_VYm, 
										m_pWPolar->m_bViscous, m_pWPolar->m_bTiltedGeom);
				if(m_pFin->m_bWingOut)  m_bPointOut = true;

				Lift  += WingLift;
				IDrag += WingIDrag;

				m_pFin->VLMSetBending();
			}

			m_CL          =  2.0*Lift /m_QInf/m_QInf/m_pWing->m_Area;
			m_InducedDrag =  1.0*IDrag/m_QInf/m_QInf/m_pWing->m_Area;
			m_ViscousDrag =  1.0*VDrag              /m_pWing->m_Area;

			m_XCP         = XCP/Lift;
			m_YCP         = YCP/Lift;

			m_GCm *= 1.0 / m_pWing->m_Area /m_pWing->m_MAChord /qdyn;
			m_GRm *= 1.0 / m_pWing->m_Area /m_pWing->m_Span    /qdyn;
			m_GYm *= 1.0 / m_pWing->m_Area /m_pWing->m_Span    /qdyn;

			m_VCm *= 1.0 / m_pWing->m_Area /m_pWing->m_MAChord /qdyn;
			m_VYm *= 1.0 / m_pWing->m_Area /m_pWing->m_Span    /qdyn;

			m_IYm *= 1.0 / m_pWing->m_Area /m_pWing->m_Span    /qdyn;

			VLMSetDownwash(m_Gamma+q*m_MatSize);

			if(m_bPointOut) m_bWarning = true;

			if(m_bConverged)
			{
				if(m_pPlane)
					pMiarex->AddPOpp(m_bPointOut, m_Cp, m_Gamma+q*m_MatSize);
				else
					pMiarex->AddWOpp(m_bPointOut, m_Gamma+q*m_MatSize);
			}
			AddString("     \r\n");
		}
		else m_bPointOut = true;
	}
}


void CVLMDlg::VLMSumForces(double *Gamma, double Alpha, double QInf, double &Lift, double &Drag)
{
	//We are working in Body Axes
	int p;
	CVector PanelForce, TotalForce;
	CVector UInf(cos(m_Alpha*pi/180.0),0.0,sin(m_Alpha*pi/180.0));
	
	for(p=0; p<m_MatSize; p++)
	{
		if(m_pWPolar->m_bVLM1 || m_pPanel[p].m_bIsTrailing)
			PanelForce  += m_pPanel[p].Vortex * Gamma[p];
	}
	
	TotalForce = UInf * PanelForce;

	Lift = TotalForce.z * cos(Alpha*pi/180.0) - TotalForce.x * sin(Alpha*pi/180.0);
	Drag = TotalForce.x * cos(Alpha*pi/180.0) + TotalForce.z * sin(Alpha*pi/180.0);
}


CVector CVLMDlg::GetSpeedVector(CVector C, double *Gamma)
{
	int pp;
	CVector VTot;
	VTot.Set(0.0,0.0,0.0);
	
	for (pp=0; pp<m_MatSize;pp++)
	{
		VLMGetVortexInfluence(m_pPanel+pp, C, V, true);
		VTot += V * Gamma[pp];
	}

	return VTot;
}

void CVLMDlg::VLMGetVortexInfluence(CPanel *pPanel, CVector const &C, CVector &V, bool bAll)
{
	// calculates the the panel p's vortex influence at point C
	// V is the resulting velocity
	int lw, pw;
	int p = pPanel->m_iElement;
	V.Set(0.0,0.0,0.0);

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
				V += VT;
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
//				//for the very last wake panel downstream, just add a horseshoe vortex influence
//				//TODO : check influence on results
//				VLMCmn(m_pWakePanel[pw].A, m_pWakePanel[pw].B,C,VT,bAll);
//				V += VT;
//				if(m_pWPolar->m_bGround) {
//					VLMCmn(AAG, BBG, C, VG);
//					V.x += VG.x;
//					V.y += VG.y;
//					V.z -= VG.z;
//				}
				//simple really !
			}
			//so says Katz and Plotkin !
		}
	}
}

/*
void CVLMDlg::VLMGetVortexInfluence(CPanel *pPanel, CVector const &C, CVector &V, bool bAll)
{
	// calculates the the panel p's vortex influence at point C
	// V is the resulting velocity
	int lw, pw;
	int p = pPanel->m_iElement;
	V.Set(0.0,0.0,0.0);
	if(m_pWPolar->m_bGround) 	CG.Set(C.x, C.y, -C.z - 2.0 * m_pWPolar->m_Height);
	if(m_pWPolar->m_bVLM1)
	{
		//just get the horseshoe vortex's influence
		VLMCmn(pPanel->A, pPanel->B, C, V, bAll);
		if(m_pWPolar->m_bGround) 
		{
			VLMCmn(pPanel->A, pPanel->B, CG, VG, bAll);
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
					VLMQmn(pPanel->A, pPanel->B, m_pPanel[p-1].A, m_pPanel[p-1].B, CG, VG);
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
				// first we get the quad vortex's influence
				if (bAll) 
				{
					VLMQmn(pPanel->A, pPanel->B, AA1, BB1, C, V);
					if(m_pWPolar->m_bGround)
					{
						VLMQmn(pPanel->A, pPanel->B, AA1, BB1, CG, VG);
						V.x += VG.x;
						V.y += VG.y;
						V.z -= VG.z;
					}
				}

				//we just add a trailing horseshoe vortex's influence to simulate the wake
				VLMCmn(AA1,BB1,C,VT,bAll);

				if(m_pWPolar->m_bGround) 
				{
					VLMCmn(AA1, BB1, CG, VG);
					VT.x += VG.x;
					VT.y += VG.y;
					VT.z -= VG.z;
				}
				V += VT;
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
						VLMQmn(pPanel->A, pPanel->B, m_pWakePanel[pw].A, m_pWakePanel[pw].B, CG, VG);
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
							VLMQmn(m_pWakePanel[pw  ].A, m_pWakePanel[pw  ].B,
								   m_pWakePanel[pw+1].A, m_pWakePanel[pw+1].B, CG, VG);
							V.x += VG.x;
							V.y += VG.y;
							V.z -= VG.z;
						}
						pw++;
					}
				}
//				//for the very last wake panel downstream, just add a horseshoe vortex influence
//				//TODO : check influence on results
//				VLMCmn(m_pWakePanel[pw].A, m_pWakePanel[pw].B,C,VT,bAll);
//				V += VT;
//				if(m_pWPolar->m_bGround) {
//					VLMCmn(AAG, BBG, C, VG);
//					V.x += VG.x;
//					V.y += VG.y;
//					V.z -= VG.z;
//				}
				//simple really !
			}
			//so says Katz and Plotkin !
		}
	}
}
*/

void CVLMDlg::VLMSetDownwash(double *Gamma)
{
	// calculates the induced angles from the vortices strengths
	int p, pp, m, nSurf;
	CVector C;

	m=0;
	nSurf = 0;
	//wing first
	memset(m_pWing->m_Vd, 0, sizeof(m_pWing->m_Vd));

	m=0;
	for (p=0; p< m_pWing->m_MatSize; p++)
	{
		if(m_pWing->m_pPanel[p].m_bIsTrailing)
		{
			C = (m_pNode[m_pWing->m_pPanel[p].m_iTA] + m_pNode[m_pWing->m_pPanel[p].m_iTB])/2.0;
			for (pp=0; pp<m_MatSize; pp++)
			{			
				VLMGetVortexInfluence(m_pPanel+pp,C,V,false);
				m_pWing->m_Vd[m] += V * Gamma[pp];
			}
			m++;
		}
	}
	

	if(m_pWing2)
	{
		m=0;
		for (p=0; p< m_pWing2->m_MatSize; p++)
		{
			if(m_pWing2->m_pPanel[p].m_bIsTrailing)
			{
				C = (m_pNode[m_pWing2->m_pPanel[p].m_iTA] + m_pNode[m_pWing2->m_pPanel[p].m_iTB])/2.0;
				for (pp=0; pp<m_MatSize; pp++)
				{			
					VLMGetVortexInfluence(m_pPanel+pp,C,V,false);
					m_pWing2->m_Vd[m] += V * Gamma[pp];
				}
				m++;
			}
		}
	}
	
	if(m_pStab)
	{
		m=0;
		for (p=0; p< m_pStab->m_MatSize; p++)
		{
			if(m_pStab->m_pPanel[p].m_bIsTrailing)
			{
				C = (m_pNode[m_pStab->m_pPanel[p].m_iTA] + m_pNode[m_pStab->m_pPanel[p].m_iTB])/2.0;
				for (pp=0; pp<m_MatSize; pp++)
				{			
					VLMGetVortexInfluence(m_pPanel+pp,C,V,false);
					m_pStab->m_Vd[m] += V * Gamma[pp];
				}
				m++;
			}
		}
	}

	if(m_pFin)
	{
		m=0;
		for (p=0; p< m_pFin->m_MatSize; p++)
		{
			if(m_pFin->m_pPanel[p].m_bIsTrailing)
			{
				C = (m_pNode[m_pFin->m_pPanel[p].m_iTA] + m_pNode[m_pFin->m_pPanel[p].m_iTB])/2.0;
				for (pp=0; pp<m_MatSize; pp++)
				{			
					VLMGetVortexInfluence(m_pPanel+pp,C,V,false);
					m_pFin->m_Vd[m] += V * Gamma[pp];
				}
				m++;
			}
		}
	}
}



void CVLMDlg::VLMSetAi(double *Gamma)
{
	// calculates the induced angles from the vortices strengths
	CVector C, CG;
	CVector Vt;
	int p, pp, m;
	double Ai[MAXSTATIONS];
	m=0;

//	CVector K(   -sin(m_Alpha*pi/180.0), 0.0, cos(m_Alpha*pi/180.0));
//	CVector QInf( cos(m_Alpha*pi/180.0), 0.0, sin(m_Alpha*pi/180.0));

//	QInf.Set(1.0,0.0,0.0);

	if(m_bTrace) AddString("        Calculating induced angles...\r\n");

	for (p=0; p<m_MatSize; p++)
	{
		if(m_pPanel[p].m_bIsTrailing)
		{
			C = (m_pNode[m_pPanel[p].m_iTA] + m_pNode[m_pPanel[p].m_iTB])/2.0;
//			C += QInf * 20.0*m_pWing->m_Span;
			C.x  = 100.0*m_pWing->m_Span;

			CG.x = C.x;
			CG.y = C.y;
			CG.z = C.z-2.0*m_pWPolar->m_Height;

			Vt.Set(0.0,0.0,0.0);

			for (pp=0; pp<m_MatSize; pp++)
			{
				VLMGetVortexInfluence(m_pPanel+pp, C, V, true);
				Vt += V * Gamma[pp];
			}
//			m_Ai[m] = atan2(Vt.dot(K), m_QInf) * 180.0/pi;
			Ai[m] = atan2(Vt.z, m_QInf) * 180.0/pi;
			m++;
		}
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


void CVLMDlg::VLMRotateGeomY(double Angle, CVector P)
{
	int n, p;
	double cosa = cos(Angle*pi/180.0);
	double sina = sin(Angle*pi/180.0);
	int iLA, iLB, iTA, iTB;
	CVector LATB, TALB;

	for (n=0; n< m_nNodes; n++)
		m_pNode[n].RotateY(P, Angle);
	
	for (p=0; p< m_MatSize; p++)
	{
		iLA = m_pPanel[p].m_iLA; iLB = m_pPanel[p].m_iLB;
		iTA = m_pPanel[p].m_iTA; iTB = m_pPanel[p].m_iTB;

		LATB = m_pNode[iLA] - m_pNode[iTB];
		TALB = m_pNode[iTA] - m_pNode[iLB];

		m_pPanel[p].Normal = LATB * TALB;

//		m_pPanel[p].Area =  m_pPanel[p].Normal.VAbs()/2.0; //area is left unchanged by the rotation
		m_pPanel[p].Normal.Normalize();
		m_pPanel[p].SetFrame( m_pNode[iLA], m_pNode[iLB], m_pNode[iTA], m_pNode[iTB]);
	}
}


void CVLMDlg::ResetWakeNodes()
{
	memcpy(m_pWakeNode, m_pRefWakeNode, sizeof(m_pRefWakeNode));
}


void CVLMDlg::RelaxWake()
{
	int lw, kw, llw;
	double t, dx;
	int mw=0;
	double *Gamma = m_Gamma;
	CVector LATB, TALB, Trans, VL, VT;
	CVector QInf(m_QInf*cos(m_Alpha*pi/180.0), 0.0, m_QInf*sin(m_Alpha*pi/180.0)) ;
	CVector WLA, WLB,WTA,WTB, WTemp;//wake panel's leading corner points
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;

	// calculates the induced velocity at the wake panel points 
	// and realigns the panel's sides with the local flow vector
	// we move all the wake nodes except for the leading nodes which remain at the wing's T.E.
	//METHOD A.1 / B.2 + André's method
	//find the velocity at the panel's LE points
	// and align the panel's side

	AddString("      Relaxing the wake...\r\n\r\n");

	memcpy(m_pTempWakeNode, m_pWakeNode, sizeof(m_pTempWakeNode));

	for (kw=0; kw<m_NWakeColumn; kw++)
	{
		if(m_bCancel) break;
		for (lw=0; lw<m_pWPolar->m_NXWakePanels; lw++)
		{
			//left point
			mw = kw * m_pWPolar->m_NXWakePanels + lw;

			//left point
			WLA.Copy(m_pTempWakeNode[m_pWakePanel[mw].m_iLA]);
			WTA.Copy(m_pTempWakeNode[m_pWakePanel[mw].m_iTA]);
			WTemp.Copy(WLA);
			
			dx  = (WTA.x-WLA.x);

			VL  = GetSpeedVector(WLA, Gamma);
			VL += QInf;
			VT  = GetSpeedVector(WTA, Gamma);
			VT += QInf;
			V = (VL +VT)/2.0;
			V.Normalize();

			t = dx/V.x;
			WTemp.x += dx;
			WTemp.y += V.y * t;
			WTemp.z += V.z * t;
			

			//define the translation vector and move the panel's left T.E. node
			Trans = WTemp-WLA;

			//and move all the nodes downstream, i.e. all panels left nodes
			for (llw=0; llw <m_pWPolar->m_NXWakePanels-lw; llw++)
			{
				m_pTempWakeNode[m_pWakePanel[mw+llw].m_iTA].y += Trans.y;
				m_pTempWakeNode[m_pWakePanel[mw+llw].m_iTA].z += Trans.z;
			}
		}
	}

	//finally do the same for the right side of the last panel

	for (lw=0; lw <m_pWPolar->m_NXWakePanels; lw++)
	{
		mw = (m_NWakeColumn-1) * m_pWPolar->m_NXWakePanels + lw;
		WLB.Copy(m_pTempWakeNode[m_pWakePanel[mw].m_iLB]);
		WTB.Copy(m_pTempWakeNode[m_pWakePanel[mw].m_iTB]);
		WTemp.Copy(WLB);
		dx  = (WTB.x-WLB.x);

		VL  = GetSpeedVector(WLB, Gamma);
		VL += QInf;
		VT  = GetSpeedVector(WTB, Gamma);
		VT += QInf;
		V = (VL +VT)/2.0;
		V.Normalize();

		t = dx/V.x;
		WTemp.x += dx;
		WTemp.y += V.y * t;
		WTemp.z += V.z * t;
		

		//define the translation vector and move the panel's right T.E. node
		Trans = WTemp-WLB;
		//and move all the nodes downstream, i.e. all panels left nodes
		for (llw=0; llw <m_pWPolar->m_NXWakePanels-lw; llw++)
		{
			m_pTempWakeNode[m_pWakePanel[mw+llw].m_iTB].y += Trans.y;
			m_pTempWakeNode[m_pWakePanel[mw+llw].m_iTB].z += Trans.z;
		}
	}


	// Paste the new wake nodes back into the working wake node array
	memcpy(m_pWakeNode, m_pTempWakeNode, sizeof(m_pTempWakeNode));

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



/*
void CVLMDlg::RelaxWake()
{
	int lw, kw, llw;
	double t, dx;
	int nPiter = m_WakeInterNodes;
	int mw=0;
	double *Gamma = m_Gamma;
	CVector LATB, TALB, Trans;
	CVector WLA, WLB,WTA,WTB, WTemp;//wake panel's leading corner points
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;

	// calculates the induced velocity at the wake panel points 
	// and realigns the panel's sides with the local flow vector
	// we move all the wake nodes except for the leading nodes which remain at the wing's T.E.
	//METHOD A.1 / B.2 + André's method
	//find the velocity at the panel's LE points
	// and align the panel's side

	AddString("      Relaxing the wake...\r\n\r\n");

	memcpy(m_pTempWakeNode, m_pWakeNode, sizeof(m_pTempWakeNode));

	for (kw=0; kw<m_NWakeColumn; kw++)
	{
		if(m_bCancel) break;
		for (lw=0; lw<m_pWPolar->m_NXWakePanels; lw++)
		{
			//left point
			mw = kw * m_pWPolar->m_NXWakePanels + lw;
			//left point
			WLA.Copy(m_pTempWakeNode[m_pWakePanel[mw].m_iLA]);
			WTA.Copy(m_pTempWakeNode[m_pWakePanel[mw].m_iTA]);
			WTemp.Copy(WLA);
			
			dx  = (WTA.x-WLA.x)/(double)nPiter;

			for (llw=0; llw<nPiter; llw++)
			{
				V = GetSpeedVector(WTemp, Gamma);
				V.x += m_QInf;
				V.Normalize();
				t = dx/V.x;
				WTemp.x += dx;
				WTemp.y += V.y * t;
				WTemp.z += V.z * t;
			}

			//define the translation vector and move the panel's left T.E. node
			Trans = WTemp-WLA;

			//and move all the nodes downstream, i.e. all panels left nodes
			for (llw=0; llw <m_pWPolar->m_NXWakePanels-lw; llw++)
			{
				m_pTempWakeNode[m_pWakePanel[mw+llw].m_iTA].y += Trans.y;
				m_pTempWakeNode[m_pWakePanel[mw+llw].m_iTA].z += Trans.z;
			}
		}
	}

	//finally do the same for the right side of the last panel

	for (lw=0; lw <m_pWPolar->m_NXWakePanels; lw++)
	{
		mw = (m_NWakeColumn-1) * m_pWPolar->m_NXWakePanels + lw;
		WLB.Copy(m_pTempWakeNode[m_pWakePanel[mw].m_iLB]);
		WTB.Copy(m_pTempWakeNode[m_pWakePanel[mw].m_iTB]);
		WTemp.Copy(WLB);
		dx  = (WTB.x-WLB.x)/(double)nPiter;

		for (llw=0; llw<nPiter; llw++)
		{
				V = GetSpeedVector(WTemp, Gamma);
				V.x += m_QInf;
				V.Normalize();
				t = dx/V.x;
				WTemp.x += dx;
				WTemp.y += V.y * t;
				WTemp.z += V.z * t;
		}

		//define the translation vector and move the panel's right T.E. node
		Trans = WTemp-WLB;
		//and move all the nodes downstream, i.e. all panels left nodes
		for (llw=0; llw <m_pWPolar->m_NXWakePanels-lw; llw++)
		{
			m_pTempWakeNode[m_pWakePanel[mw+llw].m_iTB].y += Trans.y;
			m_pTempWakeNode[m_pWakePanel[mw+llw].m_iTB].z += Trans.z;
		}
	}


	// Paste the new wake nodes back into the working wake node array
	memcpy(m_pWakeNode, m_pTempWakeNode, sizeof(m_pTempWakeNode));

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

*/


/*

void CVLMDlg::VLMQmn(CVector LA, CVector LB, CVector TA, CVector TB, CVector C, CVector &V)
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



void CVLMDlg::VLMCmn(CVector A, CVector B, CVector C, CVector &V, bool bAll)
{
	// CLASSIC VLM FORMULATION
	// Calculates the influence at point C of the vortex positioned at segment AB 
	//
	//    A___________B                |
	//    |           |                |
	//    |           |                | freestream speed
	//    |           |                |
	//    |           |                \/
	//    |           |
	//    \/         \/
	//
	// V is the resulting speed
	//

	V.Set(0.0,0.0,0.0);

	if(bAll)
	{
		r0 = B - A;
		r1 = C - A;
		r2 = C - B;

		Psi = r1 * r2;
		ftmp = Psi.VAbs();

		t = r1 * r0;

		if(t.VAbs()/r0.VAbs() > *m_pCoreSize){
			Psi = Psi/ftmp/ftmp;
			Omega = r0.dot(r1)/r1.VAbs() - r0.dot(r2)/r2.VAbs();
			V = Psi * Omega/4.0/pi;
		}
	}

	// We create Far points to align the trailing vortices with the reference axis
	// The trailing vortex legs are not aligned with the free-stream, i.a.w. the small angle approximation
	// If this approximation is not valid, then the geometry should be tilted in the polar definition

	// calculate left contribution
	Far.x = m_pWing->m_Span * 100.0;
	Far.y = A.y;
	Far.z = A.z;// + (Far.x-A.x) * tan(m_Alpha*pi/180.0);

	r0 = A - Far;
	r1 = C - A;
	r2 = C - Far;
	Psi = r1 * r2;
	ftmp = Psi.VAbs();

	t = r1 * r0;
	if(t.VAbs()/r0.VAbs() > *m_pCoreSize){
		Psi = Psi/ftmp/ftmp;
		Omega = r0.dot(r1)/r1.VAbs() - r0.dot(r2)/r2.VAbs();
		V += Psi * Omega/4.0/pi;
	}

	// calculate right vortex contribution
	Far.x = m_pWing->m_Span * 100.0;
	Far.y = B.y;
	Far.z = B.z;// + (Far.x-B.x) * tan(m_Alpha*pi/180.0);

	r0 = Far - B;
	r1 = C - Far;
	r2 = C - B;
	Psi = r1 * r2;
	ftmp = Psi.VAbs();

	t = r1 * r0;
	if(t.VAbs()/r0.VAbs() > *m_pCoreSize){
		Psi = Psi/ftmp/ftmp;
		Omega = r0.dot(r1)/r1.VAbs() - r0.dot(r2)/r2.VAbs();
		V += Psi * Omega/4.0/pi;
	}
}


*/



void CVLMDlg::VLMQmn(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB, CVector const &C, CVector &V)
{
	//Quadrilateral VLM FORMULATION
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
	//
	// Vectorial operations are written explicitly to save computing times (4x more efficient)
	//

	V.x = 0.0;
	V.y = 0.0;
	V.z = 0.0;

	R[0].x = LB.x;
	R[0].y = LB.y;
	R[0].z = LB.z;
	R[1].x = TB.x;
	R[1].y = TB.y;
	R[1].z = TB.z;
	R[2].x = TA.x;
	R[2].y = TA.y;
	R[2].z = TA.z;
	R[3].x = LA.x;
	R[3].y = LA.y;
	R[3].z = LA.z;
	R[4].x = LB.x;	
	R[4].y = LB.y;	
	R[4].z = LB.z;	
	
	for (int i=0; i<4; i++)
	{
		r0.x = R[i+1].x - R[i].x;
		r0.y = R[i+1].y - R[i].y;
		r0.z = R[i+1].z - R[i].z;
		r1.x = C.x - R[i].x;
		r1.y = C.y - R[i].y;
		r1.z = C.z - R[i].z;
		r2.x = C.x - R[i+1].x;
		r2.y = C.y - R[i+1].y;
		r2.z = C.z - R[i+1].z;

		Psi.x = r1.y*r2.z - r1.z*r2.y;
		Psi.y =-r1.x*r2.z + r1.z*r2.x;
		Psi.z = r1.x*r2.y - r1.y*r2.x;

		ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

		r1v = sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z));
		r2v = sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

		//get the distance of the TestPoint to the panel's side
		t.x =  r1.y*r0.z - r1.z*r0.y;
		t.y = -r1.x*r0.z + r1.z*r0.x;
		t.z =  r1.x*r0.y - r1.y*r0.x;

		if ((t.x*t.x+t.y*t.y+t.z*t.z)/(r0.x*r0.x+r0.y*r0.y+r0.z*r0.z) > *m_pCoreSize * *m_pCoreSize)
		{
			Psi.x /= ftmp;
			Psi.y /= ftmp;
			Psi.z /= ftmp;
		
			Omega = (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/r1v - (r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/r2v;	
			V.x += Psi.x * Omega/4.0/pi;
			V.y += Psi.y * Omega/4.0/pi;
			V.z += Psi.z * Omega/4.0/pi;
		}
	}
}



void CVLMDlg::VLMCmn(CVector const &A, CVector const &B, CVector const &C, CVector &V, bool bAll)
{
	// CLASSIC VLM FORMULATION
	// Calculates the influence at point C of the vortex positioned at segment AB 
	//
	//    A___________B                |
	//    |           |                |
	//    |           |                | freestream speed
	//    |           |                |
	//    |           |                \/
	//    |           |
	//    \/         \/
	//
	// V is the resulting speed at point C
	//
	// Vectorial operations are written inline to save computing times
	// -->longer code, but 4x more efficient....

	V.x = 0.0;
	V.y = 0.0;
	V.z = 0.0;

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

	if(bAll)
	{		
		Psi.x = r1.y*r2.z - r1.z*r2.y;
		Psi.y =-r1.x*r2.z + r1.z*r2.x;
		Psi.z = r1.x*r2.y - r1.y*r2.x;

		ftmp = Psi.x*Psi.x + Psi.y*Psi.y + Psi.z*Psi.z;

		//get the distance of the TestPoint to the panel's side
		t.x =  r1.y*r0.z - r1.z*r0.y;
		t.y = -r1.x*r0.z + r1.z*r0.x;
		t.z =  r1.x*r0.y - r1.y*r0.x;

		if ((t.x*t.x+t.y*t.y+t.z*t.z)/(r0.x*r0.x+r0.y*r0.y+r0.z*r0.z) > *m_pCoreSize * *m_pCoreSize)
		{
			Psi.x /= ftmp;
			Psi.y /= ftmp;
			Psi.z /= ftmp;

			Omega = (r0.x*r1.x + r0.y*r1.y + r0.z*r1.z)/sqrt((r1.x*r1.x + r1.y*r1.y + r1.z*r1.z)) 
			       -(r0.x*r2.x + r0.y*r2.y + r0.z*r2.z)/sqrt((r2.x*r2.x + r2.y*r2.y + r2.z*r2.z));

			V.x = Psi.x * Omega/4.0/pi;
			V.y = Psi.y * Omega/4.0/pi;
			V.z = Psi.z * Omega/4.0/pi;
		}
	}
	t.x=1.0; t.y=0.0; t.z=0.0;

	h.x =  r1.y*t.z - r1.z*t.y;
	h.y = -r1.x*t.z + r1.z*t.x;
	h.z =  r1.x*t.y - r1.y*t.x;

	//Next add 'left'  semi-infinite contribution
	//eq.6-56

	if ((h.x*h.x+h.y*h.y+h.z*h.z) > *m_pCoreSize * *m_pCoreSize)
	{
		ftmp = r1.z*r1.z + r1.y*r1.y;
		Psi.y =  r1.z/ftmp;
		Psi.z = -r1.y/ftmp;

		ftmp = sqrt(r1.x*r1.x + r1.y*r1.y + r1.z*r1.z);
		Omega = 1.0 + (C.x-A.x)/ftmp;
		V.y += Psi.y * Omega/4.0/pi;
		V.z += Psi.z * Omega/4.0/pi;
	}

	//Last add 'right' semi-infinite contribution
	//eq.6-57
	h.x =  r2.y*t.z - r2.z*t.y;
	h.y = -r2.x*t.z + r2.z*t.x;
	h.z =  r2.x*t.y - r2.y*t.x;

	if ((h.x*h.x+h.y*h.y+h.z*h.z) > *m_pCoreSize * *m_pCoreSize)
	{
		ftmp = r2.z*r2.z + r2.y*r2.y;

		Psi.y =  r2.z/ftmp;
		Psi.z = -r2.y/ftmp;

		ftmp = sqrt(r2.x*r2.x + r2.y*r2.y + r2.z*r2.z);
		Omega = 1.0 + (C.x-B.x)/ftmp;
		V.y -= Psi.y * Omega/4.0/pi;
		V.z -= Psi.z * Omega/4.0/pi;
	}
}