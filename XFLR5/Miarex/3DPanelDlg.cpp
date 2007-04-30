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
	RFF = 5;
	eps = 1.e-7;

	m_bSequence      = false;
	m_bWarning       = false;
	m_bType4         = false;
	m_bXFile         = false;
	m_bPlaneAnalysis = false;
	m_b3DSymetric    = true;
	m_bConverged     = false;

	m_QInf       = 0.0;//Speed vector in m/s
	m_Alpha      = 0.0;//Angle of Attack in °
	m_AlphaMax   = 0.0;
	m_DeltaAlpha = 0.0;
	
	m_MatSize   = 0;
	m_nNodes    = 0;
	m_NSurfaces = 0;

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
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
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
	else m_bXFile = false;

	SetFileHeader();
	m_ctrlOutput.SetLimitText(100000);

	m_ctrlProgress.SetRange(0,100);
	m_ctrlProgress.SetPos(0);

	if(m_pWPolar->m_AnalysisType==3) 
		StartPanelThread();

	m_b3DSymetric = false;

	return FALSE;
}

bool C3DPanelDlg::StartPanelThread()
{
	m_pPanelThread = new C3DPanelThread();
	m_pPanelThread->m_pParent = this;
	m_pPanelThread->m_bAutoDelete = true;

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
	if(m_pPanelThread->m_bFinished) 
		EndSequence();
	else {
		m_bCancel               = true;
		m_pWing->m_bCancel      = true;
		m_pPanelThread->m_bCancel = true;
	}
}
void C3DPanelDlg::SetProgress(int p)
{
	m_ctrlProgress.SetPos(p);
}



bool C3DPanelDlg::Gauss(double *A, int n, double *B, int m)
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
	SetProgress(0);
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
		SetProgress((int)(100*row/(n-1)/2));
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
		SetProgress(50 + (int)(100*(n-1-row)/(n-1)/2));
	}
	return true;
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

BOOL C3DPanelDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == W_ENDTHREAD){
		EndSequence();
		return true; 
	} 
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN){ 
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
			strong = "Jun";
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



void C3DPanelDlg::SourceVelocity(int p, CVector C, CVector &V)
{
		// Quadrilateral source
	// Katz & Plotkin 10.89 and following
	// all four corners are assumed to lie in the same plane
	// the transformation matrix from global to local coordinates has been set previously

	R = C-m_pPanel[p].CollPt;
	dist = R.VAbs();

	//Convert coordinates
	x  = m_pPanel[p].lij[0]*(C.x-m_pPanel[p].CollPt.x)  + m_pPanel[p].lij[1]*(C.y-m_pPanel[p].CollPt.y)  + m_pPanel[p].lij[2]*(C.z-m_pPanel[p].CollPt.z);
	y  = m_pPanel[p].lij[3]*(C.x-m_pPanel[p].CollPt.x)  + m_pPanel[p].lij[4]*(C.y-m_pPanel[p].CollPt.y)  + m_pPanel[p].lij[5]*(C.z-m_pPanel[p].CollPt.z);
	z  = m_pPanel[p].lij[6]*(C.x-m_pPanel[p].CollPt.x)  + m_pPanel[p].lij[7]*(C.y-m_pPanel[p].CollPt.y)  + m_pPanel[p].lij[8]*(C.z-m_pPanel[p].CollPt.z);

	if(dist>RFF*m_pPanel[p].Size){ // use far-field formula
		V.x =  1./4./pi * m_pPanel[p].Area * x /dist/dist/dist;
		V.y =  1./4./pi * m_pPanel[p].Area * y /dist/dist/dist;
		V.z =  1./4./pi * m_pPanel[p].Area * z /dist/dist/dist;
		return;
	}
	
	r1 = sqrt((x-m_pPanel[p].P1.x)*(x-m_pPanel[p].P1.x)+(y-m_pPanel[p].P1.y)*(y-m_pPanel[p].P1.y)+z*z);
	r2 = sqrt((x-m_pPanel[p].P2.x)*(x-m_pPanel[p].P2.x)+(y-m_pPanel[p].P2.y)*(y-m_pPanel[p].P2.y)+z*z);
	r3 = sqrt((x-m_pPanel[p].P3.x)*(x-m_pPanel[p].P3.x)+(y-m_pPanel[p].P3.y)*(y-m_pPanel[p].P3.y)+z*z);
	r4 = sqrt((x-m_pPanel[p].P4.x)*(x-m_pPanel[p].P4.x)+(y-m_pPanel[p].P4.y)*(y-m_pPanel[p].P4.y)+z*z);

	e1 = (x-m_pPanel[p].P1.x)*(x-m_pPanel[p].P1.x) + z*z;
	e2 = (x-m_pPanel[p].P2.x)*(x-m_pPanel[p].P2.x) + z*z;
	e3 = (x-m_pPanel[p].P3.x)*(x-m_pPanel[p].P3.x) + z*z;
	e4 = (x-m_pPanel[p].P4.x)*(x-m_pPanel[p].P4.x) + z*z;

	h1 = (x-m_pPanel[p].P1.x)*(y-m_pPanel[p].P1.y);
	h2 = (x-m_pPanel[p].P2.x)*(y-m_pPanel[p].P2.y);
	h3 = (x-m_pPanel[p].P3.x)*(y-m_pPanel[p].P3.y);
	h4 = (x-m_pPanel[p].P4.x)*(y-m_pPanel[p].P4.y);

	d12 = sqrt((m_pPanel[p].P2.x-m_pPanel[p].P1.x)*(m_pPanel[p].P2.x-m_pPanel[p].P1.x)+(m_pPanel[p].P2.y-m_pPanel[p].P1.y)*(m_pPanel[p].P2.y-m_pPanel[p].P1.y));
	d23 = sqrt((m_pPanel[p].P3.x-m_pPanel[p].P2.x)*(m_pPanel[p].P3.x-m_pPanel[p].P2.x)+(m_pPanel[p].P3.y-m_pPanel[p].P2.y)*(m_pPanel[p].P3.y-m_pPanel[p].P2.y));
	d34 = sqrt((m_pPanel[p].P4.x-m_pPanel[p].P3.x)*(m_pPanel[p].P4.x-m_pPanel[p].P3.x)+(m_pPanel[p].P4.y-m_pPanel[p].P3.y)*(m_pPanel[p].P4.y-m_pPanel[p].P3.y));
	d41 = sqrt((m_pPanel[p].P1.x-m_pPanel[p].P4.x)*(m_pPanel[p].P1.x-m_pPanel[p].P4.x)+(m_pPanel[p].P1.y-m_pPanel[p].P4.y)*(m_pPanel[p].P1.y-m_pPanel[p].P4.y));

	us = 0.0;
	vs = 0.0;
	ws = 0.0;

	if((r1+r2+d12)!=0.0 && (r1+r2-d12)/(r1+r2+d12)>0.0){
		us  = (m_pPanel[p].P2.y-m_pPanel[p].P1.y)/d12*log((r1+r2-d12)/(r1+r2+d12));
		vs  = (m_pPanel[p].P1.x-m_pPanel[p].P2.x)/d12*log((r1+r2-d12)/(r1+r2+d12));
	}
	if((r2+r3+d23)!=0.0 && (r2+r3-d23)/(r2+r3+d23)>0.0){
		us += (m_pPanel[p].P3.y-m_pPanel[p].P2.y)/d23*log((r2+r3-d23)/(r2+r3+d23));
		vs += (m_pPanel[p].P2.x-m_pPanel[p].P3.x)/d23*log((r2+r3-d23)/(r2+r3+d23));
	}
	if((r3+r4+d34)!=0.0 && (r3+r4-d34)/(r3+r4+d34)>0.0) {
		us += (m_pPanel[p].P4.y-m_pPanel[p].P3.y)/d34*log((r3+r4-d34)/(r3+r4+d34));
		vs += (m_pPanel[p].P3.x-m_pPanel[p].P4.x)/d34*log((r3+r4-d34)/(r3+r4+d34));
	}
	if((r4+r1+d41)!=0.0 && (r4+r1-d41)/(r4+r1+d41)>0.0){
		us += (m_pPanel[p].P1.y-m_pPanel[p].P4.y)/d41*log((r4+r1-d41)/(r4+r1+d41));
		vs += (m_pPanel[p].P4.x-m_pPanel[p].P1.x)/d41*log((r4+r1-d41)/(r4+r1+d41));
	}

	if(abs(z)<eps){
		if(z>=0.0){
			ws =  1.0/2.0;
		}
		else{
			ws = -1.0/2.0; 
		}
	}
	else{
		if(d12>eps){
			f = (m_pPanel[p].P2.y-m_pPanel[p].P1.y)*e1 - (m_pPanel[p].P2.x-m_pPanel[p].P1.x)*h1;
			g = (m_pPanel[p].P2.y-m_pPanel[p].P1.y)*e2 - (m_pPanel[p].P2.x-m_pPanel[p].P1.x)*h2;
			ws  = atan2(z*(m_pPanel[p].P2.x-m_pPanel[p].P1.x)*(f*r2-g*r1), z*z*(m_pPanel[p].P2.x-m_pPanel[p].P1.x)*(m_pPanel[p].P2.x-m_pPanel[p].P1.x)*r1*r2+f*g);
		}
		if(d23>eps){
			f = (m_pPanel[p].P3.y-m_pPanel[p].P2.y)*e2 - (m_pPanel[p].P3.x-m_pPanel[p].P2.x)*h2;
			g = (m_pPanel[p].P3.y-m_pPanel[p].P2.y)*e3 - (m_pPanel[p].P3.x-m_pPanel[p].P2.x)*h3;
			ws += atan2(z*(m_pPanel[p].P3.x-m_pPanel[p].P2.x)*(f*r3-g*r2), z*z*(m_pPanel[p].P3.x-m_pPanel[p].P2.x)*(m_pPanel[p].P3.x-m_pPanel[p].P2.x)*r2*r3+f*g);
		}

		if(d34>eps){
			f = (m_pPanel[p].P4.y-m_pPanel[p].P3.y)*e3 - (m_pPanel[p].P4.x-m_pPanel[p].P3.x)*h3;
			g = (m_pPanel[p].P4.y-m_pPanel[p].P3.y)*e4 - (m_pPanel[p].P4.x-m_pPanel[p].P3.x)*h4;
			ws += atan2(z*(m_pPanel[p].P4.x-m_pPanel[p].P3.x)*(f*r4-g*r3), z*z*(m_pPanel[p].P4.x-m_pPanel[p].P3.x)*(m_pPanel[p].P4.x-m_pPanel[p].P3.x)*r3*r4+f*g);
		}

		if(d41>eps){
			f = (m_pPanel[p].P1.y-m_pPanel[p].P4.y)*e4 - (m_pPanel[p].P1.x-m_pPanel[p].P4.x)*h4;
			g = (m_pPanel[p].P1.y-m_pPanel[p].P4.y)*e1 - (m_pPanel[p].P1.x-m_pPanel[p].P4.x)*h1;
			ws += atan2(z*(m_pPanel[p].P1.x-m_pPanel[p].P4.x)*(f*r1-g*r4), z*z*(m_pPanel[p].P1.x-m_pPanel[p].P4.x)*(m_pPanel[p].P1.x-m_pPanel[p].P4.x)*r4*r1+f*g);
		}
	}
	V.x = us/4.0/pi;
	V.y = vs/4.0/pi;
	V.z = ws/4.0/pi;
}


void C3DPanelDlg::DoubletPotential(int p, CVector C, double &phi)
{
	// Quadrilateral source
	// Katz & Plotkin 10.89 and following
	// all four corners are assumed to lie in the same plane
	// the transformation matrix from global to local coordinates has been set previously
	// returns V, panel p influence at point C
	phiw = 0.0;

	if(C.IsSame(m_pPanel[p].CollPt)) {
//		phi = -1.0/2.0;
		phi = 2.0*pi;
	}
	else{

		R = C-m_pPanel[p].CollPt;
		rz = R.dot(m_pPanel[p].Normal);
		dist = R.VAbs();
		if(abs(rz)<eps){
	//		if(rz>=0.0)	phi = -1.0/2.0;
	//		else		phi =  1.0/2.0;
			phi = 0.0;
		}
		else{
			if(dist>RFF*m_pPanel[p].Size){ // use far-field formula
				phi = -rz * m_pPanel[p].Area /dist/dist/dist;// * 1/4/pi
			}
			else{
				//Convert coordinates
				x  = m_pPanel[p].lij[0]*(C.x-m_pPanel[p].CollPt.x)  + m_pPanel[p].lij[1]*(C.y-m_pPanel[p].CollPt.y)  + m_pPanel[p].lij[2]*(C.z-m_pPanel[p].CollPt.z);
				y  = m_pPanel[p].lij[3]*(C.x-m_pPanel[p].CollPt.x)  + m_pPanel[p].lij[4]*(C.y-m_pPanel[p].CollPt.y)  + m_pPanel[p].lij[5]*(C.z-m_pPanel[p].CollPt.z);
				z  = m_pPanel[p].lij[6]*(C.x-m_pPanel[p].CollPt.x)  + m_pPanel[p].lij[7]*(C.y-m_pPanel[p].CollPt.y)  + m_pPanel[p].lij[8]*(C.z-m_pPanel[p].CollPt.z);

				r1 = sqrt((x-m_pPanel[p].P1.x)*(x-m_pPanel[p].P1.x)+(y-m_pPanel[p].P1.y)*(y-m_pPanel[p].P1.y)+z*z);
				r2 = sqrt((x-m_pPanel[p].P2.x)*(x-m_pPanel[p].P2.x)+(y-m_pPanel[p].P2.y)*(y-m_pPanel[p].P2.y)+z*z);
				r3 = sqrt((x-m_pPanel[p].P3.x)*(x-m_pPanel[p].P3.x)+(y-m_pPanel[p].P3.y)*(y-m_pPanel[p].P3.y)+z*z);
				r4 = sqrt((x-m_pPanel[p].P4.x)*(x-m_pPanel[p].P4.x)+(y-m_pPanel[p].P4.y)*(y-m_pPanel[p].P4.y)+z*z);

				e1 = (x-m_pPanel[p].P1.x)*(x-m_pPanel[p].P1.x) + z*z;
				e2 = (x-m_pPanel[p].P2.x)*(x-m_pPanel[p].P2.x) + z*z;
				e3 = (x-m_pPanel[p].P3.x)*(x-m_pPanel[p].P3.x) + z*z;
				e4 = (x-m_pPanel[p].P4.x)*(x-m_pPanel[p].P4.x) + z*z;

				h1 = (x-m_pPanel[p].P1.x)*(y-m_pPanel[p].P1.y);
				h2 = (x-m_pPanel[p].P2.x)*(y-m_pPanel[p].P2.y);
				h3 = (x-m_pPanel[p].P3.x)*(y-m_pPanel[p].P3.y);
				h4 = (x-m_pPanel[p].P4.x)*(y-m_pPanel[p].P4.y);

				d12 = sqrt((m_pPanel[p].P2.x-m_pPanel[p].P1.x)*(m_pPanel[p].P2.x-m_pPanel[p].P1.x)+(m_pPanel[p].P2.y-m_pPanel[p].P1.y)*(m_pPanel[p].P2.y-m_pPanel[p].P1.y));
				d23 = sqrt((m_pPanel[p].P3.x-m_pPanel[p].P2.x)*(m_pPanel[p].P3.x-m_pPanel[p].P2.x)+(m_pPanel[p].P3.y-m_pPanel[p].P2.y)*(m_pPanel[p].P3.y-m_pPanel[p].P2.y));
				d34 = sqrt((m_pPanel[p].P4.x-m_pPanel[p].P3.x)*(m_pPanel[p].P4.x-m_pPanel[p].P3.x)+(m_pPanel[p].P4.y-m_pPanel[p].P3.y)*(m_pPanel[p].P4.y-m_pPanel[p].P3.y));
				d41 = sqrt((m_pPanel[p].P1.x-m_pPanel[p].P4.x)*(m_pPanel[p].P1.x-m_pPanel[p].P4.x)+(m_pPanel[p].P1.y-m_pPanel[p].P4.y)*(m_pPanel[p].P1.y-m_pPanel[p].P4.y));

				phi = 0.0;

				if(d12>eps){
					f = (m_pPanel[p].P2.y-m_pPanel[p].P1.y)*e1 - (m_pPanel[p].P2.x-m_pPanel[p].P1.x)*h1;
					g = (m_pPanel[p].P2.y-m_pPanel[p].P1.y)*e2 - (m_pPanel[p].P2.x-m_pPanel[p].P1.x)*h2;
					phi  = -atan2(z*(m_pPanel[p].P2.x-m_pPanel[p].P1.x)*(f*r2-g*r1), z*z*(m_pPanel[p].P2.x-m_pPanel[p].P1.x)*(m_pPanel[p].P2.x-m_pPanel[p].P1.x)*r1*r2+f*g);
				}
				if(d23>eps){
					f = (m_pPanel[p].P3.y-m_pPanel[p].P2.y)*e2 - (m_pPanel[p].P3.x-m_pPanel[p].P2.x)*h2;
					g = (m_pPanel[p].P3.y-m_pPanel[p].P2.y)*e3 - (m_pPanel[p].P3.x-m_pPanel[p].P2.x)*h3;
					phi  -= atan2(z*(m_pPanel[p].P3.x-m_pPanel[p].P2.x)*(f*r3-g*r2), z*z*(m_pPanel[p].P3.x-m_pPanel[p].P2.x)*(m_pPanel[p].P3.x-m_pPanel[p].P2.x)*r2*r3+f*g);
				}

				if(d34>eps){
					f = (m_pPanel[p].P4.y-m_pPanel[p].P3.y)*e3 - (m_pPanel[p].P4.x-m_pPanel[p].P3.x)*h3;
					g = (m_pPanel[p].P4.y-m_pPanel[p].P3.y)*e4 - (m_pPanel[p].P4.x-m_pPanel[p].P3.x)*h4;
					phi  -= atan2(z*(m_pPanel[p].P4.x-m_pPanel[p].P3.x)*(f*r4-g*r3), z*z*(m_pPanel[p].P4.x-m_pPanel[p].P3.x)*(m_pPanel[p].P4.x-m_pPanel[p].P3.x)*r3*r4+f*g);
				}

				if(d41>eps){
					f = (m_pPanel[p].P1.y-m_pPanel[p].P4.y)*e4 - (m_pPanel[p].P1.x-m_pPanel[p].P4.x)*h4;
					g = (m_pPanel[p].P1.y-m_pPanel[p].P4.y)*e1 - (m_pPanel[p].P1.x-m_pPanel[p].P4.x)*h1;
					phi  -= atan2(z*(m_pPanel[p].P1.x-m_pPanel[p].P4.x)*(f*r1-g*r4), z*z*(m_pPanel[p].P1.x-m_pPanel[p].P4.x)*(m_pPanel[p].P1.x-m_pPanel[p].P4.x)*r4*r1+f*g);
				}
			}
		}
	}

	if(m_pPanel[p].m_bIsTrailing){
		//add the wake panel contribution to enforce Kutta's condition
		// we start at the first wake panel and go thru the wake

		int pw = m_pPanel[p].m_iWake;
		while (pw>=0)
		{
			R = C-m_pWakePanel[pw].CollPt;
			rz = R.dot(m_pWakePanel[pw].Normal);
			dist = R.VAbs();
			if(abs(rz)<eps){
//				if(rz>=0.0)	phiw = -1.0/2.0;
//				else		phiw =  1.0/2.0;
				phiw = 0.0;
			}
			else{
				if(dist>RFF*m_pWakePanel[pw].Size){ // use far-field formula
					phiw = -rz * m_pWakePanel[pw].Area /dist/dist/dist;// * -1/4/pi
					if(m_pWakePanel[pw].m_iPos==-1) phiw = -phiw;
				}
				else{

					//Convert coordinates
					x  = m_pWakePanel[pw].lij[0]*(C.x-m_pWakePanel[pw].CollPt.x)  + m_pWakePanel[pw].lij[1]*(C.y-m_pWakePanel[pw].CollPt.y)  + m_pWakePanel[pw].lij[2]*(C.z-m_pWakePanel[pw].CollPt.z);
					y  = m_pWakePanel[pw].lij[3]*(C.x-m_pWakePanel[pw].CollPt.x)  + m_pWakePanel[pw].lij[4]*(C.y-m_pWakePanel[pw].CollPt.y)  + m_pWakePanel[pw].lij[5]*(C.z-m_pWakePanel[pw].CollPt.z);
					z  = m_pWakePanel[pw].lij[6]*(C.x-m_pWakePanel[pw].CollPt.x)  + m_pWakePanel[pw].lij[7]*(C.y-m_pWakePanel[pw].CollPt.y)  + m_pWakePanel[pw].lij[8]*(C.z-m_pWakePanel[pw].CollPt.z);

					r1 = sqrt((x-m_pWakePanel[pw].P1.x)*(x-m_pWakePanel[pw].P1.x)+(y-m_pWakePanel[pw].P1.y)*(y-m_pWakePanel[pw].P1.y)+z*z);
					r2 = sqrt((x-m_pWakePanel[pw].P2.x)*(x-m_pWakePanel[pw].P2.x)+(y-m_pWakePanel[pw].P2.y)*(y-m_pWakePanel[pw].P2.y)+z*z);
					r3 = sqrt((x-m_pWakePanel[pw].P3.x)*(x-m_pWakePanel[pw].P3.x)+(y-m_pWakePanel[pw].P3.y)*(y-m_pWakePanel[pw].P3.y)+z*z);
					r4 = sqrt((x-m_pWakePanel[pw].P4.x)*(x-m_pWakePanel[pw].P4.x)+(y-m_pWakePanel[pw].P4.y)*(y-m_pWakePanel[pw].P4.y)+z*z);

					e1 = (x-m_pWakePanel[pw].P1.x)*(x-m_pWakePanel[pw].P1.x) + z*z;
					e2 = (x-m_pWakePanel[pw].P2.x)*(x-m_pWakePanel[pw].P2.x) + z*z;
					e3 = (x-m_pWakePanel[pw].P3.x)*(x-m_pWakePanel[pw].P3.x) + z*z;
					e4 = (x-m_pWakePanel[pw].P4.x)*(x-m_pWakePanel[pw].P4.x) + z*z;

					h1 = (x-m_pWakePanel[pw].P1.x)*(y-m_pWakePanel[pw].P1.y);
					h2 = (x-m_pWakePanel[pw].P2.x)*(y-m_pWakePanel[pw].P2.y);
					h3 = (x-m_pWakePanel[pw].P3.x)*(y-m_pWakePanel[pw].P3.y);
					h4 = (x-m_pWakePanel[pw].P4.x)*(y-m_pWakePanel[pw].P4.y);

					d12 = sqrt((m_pWakePanel[pw].P2.x-m_pWakePanel[pw].P1.x)*(m_pWakePanel[pw].P2.x-m_pWakePanel[pw].P1.x)+
						       (m_pWakePanel[pw].P2.y-m_pWakePanel[pw].P1.y)*(m_pWakePanel[pw].P2.y-m_pWakePanel[pw].P1.y));
					d23 = sqrt((m_pWakePanel[pw].P3.x-m_pWakePanel[pw].P2.x)*(m_pWakePanel[pw].P3.x-m_pWakePanel[pw].P2.x)+
						       (m_pWakePanel[pw].P3.y-m_pWakePanel[pw].P2.y)*(m_pWakePanel[pw].P3.y-m_pWakePanel[pw].P2.y));
					d34 = sqrt((m_pWakePanel[pw].P4.x-m_pWakePanel[pw].P3.x)*(m_pWakePanel[pw].P4.x-m_pWakePanel[pw].P3.x)+
						       (m_pWakePanel[pw].P4.y-m_pWakePanel[pw].P3.y)*(m_pWakePanel[pw].P4.y-m_pWakePanel[pw].P3.y));
					d41 = sqrt((m_pWakePanel[pw].P1.x-m_pWakePanel[pw].P4.x)*(m_pWakePanel[pw].P1.x-m_pWakePanel[pw].P4.x)+
						       (m_pWakePanel[pw].P1.y-m_pWakePanel[pw].P4.y)*(m_pWakePanel[pw].P1.y-m_pWakePanel[pw].P4.y));

					phiw = 0.0;

					if(d12>eps){				
						f = (m_pWakePanel[pw].P2.y-m_pWakePanel[pw].P1.y)*e1 - (m_pWakePanel[pw].P2.x-m_pWakePanel[pw].P1.x)*h1;
						g = (m_pWakePanel[pw].P2.y-m_pWakePanel[pw].P1.y)*e2 - (m_pWakePanel[pw].P2.x-m_pWakePanel[pw].P1.x)*h2;
						phiw  = atan2(z*(m_pWakePanel[pw].P2.x-m_pWakePanel[pw].P1.x)*(f*r2-g*r1), z*z*(m_pWakePanel[pw].P2.x-m_pWakePanel[pw].P1.x)*(m_pWakePanel[pw].P2.x-m_pWakePanel[pw].P1.x)*r1*r2+f*g);
					}
					if(d23>eps){
						f = (m_pWakePanel[pw].P3.y-m_pWakePanel[pw].P2.y)*e2 - (m_pWakePanel[pw].P3.x-m_pWakePanel[pw].P2.x)*h2;
						g = (m_pWakePanel[pw].P3.y-m_pWakePanel[pw].P2.y)*e3 - (m_pWakePanel[pw].P3.x-m_pWakePanel[pw].P2.x)*h3;
						phiw  += atan2(z*(m_pWakePanel[pw].P3.x-m_pWakePanel[pw].P2.x)*(f*r3-g*r2), z*z*(m_pWakePanel[pw].P3.x-m_pWakePanel[pw].P2.x)*(m_pWakePanel[pw].P3.x-m_pWakePanel[pw].P2.x)*r2*r3+f*g);
					}

					if(d34>eps){
						f = (m_pWakePanel[pw].P4.y-m_pWakePanel[pw].P3.y)*e3 - (m_pWakePanel[pw].P4.x-m_pWakePanel[pw].P3.x)*h3;
						g = (m_pWakePanel[pw].P4.y-m_pWakePanel[pw].P3.y)*e4 - (m_pWakePanel[pw].P4.x-m_pWakePanel[pw].P3.x)*h4;
						phiw  += atan2(z*(m_pWakePanel[pw].P4.x-m_pWakePanel[pw].P3.x)*(f*r4-g*r3), z*z*(m_pWakePanel[pw].P4.x-m_pWakePanel[pw].P3.x)*(m_pWakePanel[pw].P4.x-m_pWakePanel[pw].P3.x)*r3*r4+f*g);
					}

					if(d41>eps){
						f = (m_pWakePanel[pw].P1.y-m_pWakePanel[pw].P4.y)*e4 - (m_pWakePanel[pw].P1.x-m_pWakePanel[pw].P4.x)*h4;
						g = (m_pWakePanel[pw].P1.y-m_pWakePanel[pw].P4.y)*e1 - (m_pWakePanel[pw].P1.x-m_pWakePanel[pw].P4.x)*h1;
						phiw  += atan2(z*(m_pWakePanel[pw].P1.x-m_pWakePanel[pw].P4.x)*(f*r1-g*r4), z*z*(m_pWakePanel[pw].P1.x-m_pWakePanel[pw].P4.x)*(m_pWakePanel[pw].P1.x-m_pWakePanel[pw].P4.x)*r4*r1+f*g);
					}
				}
			}
			pw = m_pWakePanel[pw].m_iPD;
		}
	}

	if(m_pPanel[p].m_iPos>0)	phi = -(phi+phiw)/4.0/pi;
	else						phi = -(phi-phiw)/4.0/pi;
}


void C3DPanelDlg::SourcePotential(int p, CVector C, double &phi)
{
	// Quadrilateral source
	// Katz & Plotkin 10.89 and following
	// all four corners are assumed to lie in the same plane
	// the transformation matrix from global to local coordinates has been set previously

	double dist = (C-m_pPanel[p].CollPt).VAbs();
	
	if(dist>RFF*m_pPanel[p].Size){ // use far-field formula
		phi = -1./4./pi * m_pPanel[p].Area /dist;
		return;
	}

	//Convert coordinates
	x  = m_pPanel[p].lij[0]*(C.x-m_pPanel[p].CollPt.x)  + m_pPanel[p].lij[1]*(C.y-m_pPanel[p].CollPt.y)  + m_pPanel[p].lij[2]*(C.z-m_pPanel[p].CollPt.z);
	y  = m_pPanel[p].lij[3]*(C.x-m_pPanel[p].CollPt.x)  + m_pPanel[p].lij[4]*(C.y-m_pPanel[p].CollPt.y)  + m_pPanel[p].lij[5]*(C.z-m_pPanel[p].CollPt.z);
	z  = m_pPanel[p].lij[6]*(C.x-m_pPanel[p].CollPt.x)  + m_pPanel[p].lij[7]*(C.y-m_pPanel[p].CollPt.y)  + m_pPanel[p].lij[8]*(C.z-m_pPanel[p].CollPt.z);

	//calculate the influence
	r1 = sqrt((x-m_pPanel[p].P1.x)*(x-m_pPanel[p].P1.x)+(y-m_pPanel[p].P1.y)*(y-m_pPanel[p].P1.y)+z*z);
	r2 = sqrt((x-m_pPanel[p].P2.x)*(x-m_pPanel[p].P2.x)+(y-m_pPanel[p].P2.y)*(y-m_pPanel[p].P2.y)+z*z);
	r3 = sqrt((x-m_pPanel[p].P3.x)*(x-m_pPanel[p].P3.x)+(y-m_pPanel[p].P3.y)*(y-m_pPanel[p].P3.y)+z*z);
	r4 = sqrt((x-m_pPanel[p].P4.x)*(x-m_pPanel[p].P4.x)+(y-m_pPanel[p].P4.y)*(y-m_pPanel[p].P4.y)+z*z);

	e1 = (x-m_pPanel[p].P1.x)*(x-m_pPanel[p].P1.x) + z*z;
	e2 = (x-m_pPanel[p].P2.x)*(x-m_pPanel[p].P2.x) + z*z;
	e3 = (x-m_pPanel[p].P3.x)*(x-m_pPanel[p].P3.x) + z*z;
	e4 = (x-m_pPanel[p].P4.x)*(x-m_pPanel[p].P4.x) + z*z;

	h1 = (x-m_pPanel[p].P1.x)*(y-m_pPanel[p].P1.y);
	h2 = (x-m_pPanel[p].P2.x)*(y-m_pPanel[p].P2.y);
	h3 = (x-m_pPanel[p].P3.x)*(y-m_pPanel[p].P3.y);
	h4 = (x-m_pPanel[p].P4.x)*(y-m_pPanel[p].P4.y);

	d12 =sqrt((m_pPanel[p].P2.x-m_pPanel[p].P1.x)*(m_pPanel[p].P2.x-m_pPanel[p].P1.x)+(m_pPanel[p].P2.y-m_pPanel[p].P1.y)*(m_pPanel[p].P2.y-m_pPanel[p].P1.y));
	d23 =sqrt((m_pPanel[p].P3.x-m_pPanel[p].P2.x)*(m_pPanel[p].P3.x-m_pPanel[p].P2.x)+(m_pPanel[p].P3.y-m_pPanel[p].P2.y)*(m_pPanel[p].P3.y-m_pPanel[p].P2.y));
	d34 =sqrt((m_pPanel[p].P4.x-m_pPanel[p].P3.x)*(m_pPanel[p].P4.x-m_pPanel[p].P3.x)+(m_pPanel[p].P4.y-m_pPanel[p].P3.y)*(m_pPanel[p].P4.y-m_pPanel[p].P3.y));
	d41 =sqrt((m_pPanel[p].P1.x-m_pPanel[p].P4.x)*(m_pPanel[p].P1.x-m_pPanel[p].P4.x)+(m_pPanel[p].P1.y-m_pPanel[p].P4.y)*(m_pPanel[p].P1.y-m_pPanel[p].P4.y));
	
	phi1 = 0.0;
	phi2 = 0.0;

	if(d12>eps){
		f = (m_pPanel[p].P2.y-m_pPanel[p].P1.y)*e1 - (m_pPanel[p].P2.x-m_pPanel[p].P1.x)*h1;
		g = (m_pPanel[p].P2.y-m_pPanel[p].P1.y)*e2 - (m_pPanel[p].P2.x-m_pPanel[p].P1.x)*h2;
		phi1  = ((x-m_pPanel[p].P1.x)*(m_pPanel[p].P2.y-m_pPanel[p].P1.y) 
			   - (y-m_pPanel[p].P1.y)*(m_pPanel[p].P2.x-m_pPanel[p].P1.x))/d12 * log((r1+r2+d12)/(r1+r2-d12));
		phi2  = atan2(z*(m_pPanel[p].P2.x-m_pPanel[p].P1.x)*(f*r2-g*r1), z*z*(m_pPanel[p].P2.x-m_pPanel[p].P1.x)*(m_pPanel[p].P2.x-m_pPanel[p].P1.x)*r1*r2+f*g);
	}
	if(d23>eps){

		f = (m_pPanel[p].P3.y-m_pPanel[p].P2.y)*e2 - (m_pPanel[p].P3.x-m_pPanel[p].P2.x)*h2;
		g = (m_pPanel[p].P3.y-m_pPanel[p].P2.y)*e3 - (m_pPanel[p].P3.x-m_pPanel[p].P2.x)*h3;
		phi1 += ((x-m_pPanel[p].P2.x)*(m_pPanel[p].P3.y-m_pPanel[p].P2.y) 
			   - (y-m_pPanel[p].P2.y)*(m_pPanel[p].P3.x-m_pPanel[p].P2.x))/d23 * log((r2+r3+d23)/(r2+r3-d23));
		phi2 += atan2(z*(m_pPanel[p].P3.x-m_pPanel[p].P2.x)*(f*r3-g*r2), z*z*(m_pPanel[p].P3.x-m_pPanel[p].P2.x)*(m_pPanel[p].P3.x-m_pPanel[p].P2.x)*r2*r3+f*g);
	}

	if(d34>eps){

		f = (m_pPanel[p].P4.y-m_pPanel[p].P3.y)*e3 - (m_pPanel[p].P4.x-m_pPanel[p].P3.x)*h3;
		g = (m_pPanel[p].P4.y-m_pPanel[p].P3.y)*e4 - (m_pPanel[p].P4.x-m_pPanel[p].P3.x)*h4;
		phi1 += ((x-m_pPanel[p].P3.x)*(m_pPanel[p].P4.y-m_pPanel[p].P3.y) 
			   - (y-m_pPanel[p].P3.y)*(m_pPanel[p].P4.x-m_pPanel[p].P3.x))/d34 * log((r3+r4+d34)/(r3+r4-d34));
		phi2 += atan2(z*(m_pPanel[p].P4.x-m_pPanel[p].P3.x)*(f*r4-g*r3), z*z*(m_pPanel[p].P4.x-m_pPanel[p].P3.x)*(m_pPanel[p].P4.x-m_pPanel[p].P3.x)*r3*r4+f*g);
	}

	if(d41>eps){

		f = (m_pPanel[p].P1.y-m_pPanel[p].P4.y)*e4 - (m_pPanel[p].P1.x-m_pPanel[p].P4.x)*h4;
		g = (m_pPanel[p].P1.y-m_pPanel[p].P4.y)*e1 - (m_pPanel[p].P1.x-m_pPanel[p].P4.x)*h1;
		phi1 += ((x-m_pPanel[p].P4.x)*(m_pPanel[p].P1.y-m_pPanel[p].P4.y) 
			   - (y-m_pPanel[p].P4.y)*(m_pPanel[p].P1.x-m_pPanel[p].P4.x))/d41 * log((r4+r1+d41)/(r4+r1-d41));
		phi2 += atan2(z*(m_pPanel[p].P1.x-m_pPanel[p].P4.x)*(f*r1-g*r4), z*z*(m_pPanel[p].P1.x-m_pPanel[p].P4.x)*(m_pPanel[p].P1.x-m_pPanel[p].P4.x)*r4*r1+f*g);
	}
	
	phi2 *= abs(z);
	phi = -(phi1 - phi2)/4./pi;
}



void C3DPanelDlg::DoubletVelocity(int p, CVector C, CVector &V)
{
	// Quadrilateral source
	// Katz & Plotkin 10.89 and following
	// all four corners are assumed to lie in the same plane
	// the transformation matrix from global to local coordinates has been set previously

	//returns V, panel p influence at point C

	R = C-m_pPanel[p].CollPt;
	dist = R.VAbs();

	//Convert coordinates
	x  = m_pPanel[p].lij[0]*(R.x)  + m_pPanel[p].lij[1]*(R.y)  + m_pPanel[p].lij[2]*(R.z);
	y  = m_pPanel[p].lij[3]*(R.x)  + m_pPanel[p].lij[4]*(R.y)  + m_pPanel[p].lij[5]*(R.z);
	z  = m_pPanel[p].lij[6]*(R.x)  + m_pPanel[p].lij[7]*(R.y)  + m_pPanel[p].lij[8]*(R.z);

	if(dist>RFF*m_pPanel[p].Size){ // use far-field formula
		V.x =  3./4./pi * m_pPanel[p].Area * x * z /dist/dist/dist/dist/dist;
		V.y =  3./4./pi * m_pPanel[p].Area * y * z /dist/dist/dist/dist/dist;
		V.z = -1./4./pi * m_pPanel[p].Area * (x*x+y*y-2.0*z*z) /dist/dist/dist/dist/dist;
		int rien = 0;
//		return;
	}

	x1 = m_pPanel[p].P1.x; y1 = m_pPanel[p].P1.y; 
	x2 = m_pPanel[p].P2.x; y2 = m_pPanel[p].P2.y; 
	x3 = m_pPanel[p].P3.x; y3 = m_pPanel[p].P3.y; 
	x4 = m_pPanel[p].P4.x; y4 = m_pPanel[p].P4.y; 

	r1 = sqrt((x-x1)*(x-x1)+(y-y1)*(y-y1)+z*z);
	r2 = sqrt((x-x2)*(x-x2)+(y-y2)*(y-y2)+z*z);
	r3 = sqrt((x-x3)*(x-x3)+(y-y3)*(y-y3)+z*z);
	r4 = sqrt((x-x4)*(x-x4)+(y-y4)*(y-y4)+z*z);

	if(abs(z)>eps){
		us  = z*(y1-y2)*(r1+r2)/r1/r2/(r1*r2-((x-x1)*(x-x2)+(y-y1)*(y-y2)+z*z));
		us += z*(y2-y3)*(r2+r3)/r2/r3/(r2*r3-((x-x2)*(x-x3)+(y-y2)*(y-y3)+z*z));
		us += z*(y3-y4)*(r3+r4)/r3/r4/(r3*r4-((x-x3)*(x-x4)+(y-y3)*(y-y4)+z*z));
		us += z*(y4-y1)*(r4+r1)/r4/r1/(r4*r1-((x-x4)*(x-x1)+(y-y4)*(y-y1)+z*z));

		vs  = z*(x2-x1)*(r1+r2)/r1/r2/(r1*r2-((x-x1)*(x-x2)+(y-y1)*(y-y2)+z*z));
		vs += z*(x3-x2)*(r2+r3)/r2/r3/(r2*r3-((x-x2)*(x-x3)+(y-y2)*(y-y3)+z*z));
		vs += z*(x4-x3)*(r3+r4)/r3/r4/(r3*r4-((x-x3)*(x-x4)+(y-y3)*(y-y4)+z*z));
		vs += z*(x1-x4)*(r4+r1)/r4/r1/(r4*r1-((x-x4)*(x-x1)+(y-y4)*(y-y1)+z*z));
	}
	else {
		us = 0.0;
		vs = 0.0;
	}

	ws  = ((x-x2)*(y-y1)-(x-x1)*(y-y2))*(r1+r2)/r1/r2/(r1*r2-((x-x1)*(x-x2)+(y-y1)*(y-y2)+z*z));
	ws += ((x-x3)*(y-y2)-(x-x2)*(y-y3))*(r2+r3)/r2/r3/(r2*r3-((x-x2)*(x-x3)+(y-y2)*(y-y3)+z*z));
	ws += ((x-x4)*(y-y3)-(x-x3)*(y-y4))*(r3+r4)/r3/r4/(r3*r4-((x-x3)*(x-x4)+(y-y3)*(y-y4)+z*z));
	ws += ((x-x1)*(y-y4)-(x-x4)*(y-y1))*(r4+r1)/r4/r1/(r4*r1-((x-x4)*(x-x1)+(y-y4)*(y-y1)+z*z));

	V.x = us/4.0/pi;
	V.y = vs/4.0/pi;
	V.z = ws/4.0/pi;
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

bool  C3DPanelDlg::CreateRHS()
{
	int i, p, pp;
	double phi;
	CVector V;
	CVector QInf(m_QInf*cos(m_Alpha*pi/180.0), 0.0, m_QInf * sin(m_Alpha*pi/180.0));
//	CVector QInf(m_QInf, 0.0, 0.0);
	AddString("      Creating RHS vector...\r\n");
	SetProgress(0);

	// the source strength is the scalar product of the freestream vector speed and the panel normal
	for (i=0; i< m_MatSize; i++){
		if(m_bCancel) return false;
		m_Sigma[i]= QInf.dot(m_pPanel[i].Normal);
	}
	
	for (p=0; p<m_MatSize; p++){
		if(m_bCancel) return false;
		m_RHS[p] = 0.0;
		for (pp=0; pp<m_MatSize; pp++){
//			Source(p, pp, V, phi);
//			double phi1 = phi/4.0/pi;
			SourcePotential(pp, m_pPanel[p].CollPt, phi);
//if(p==0) TRACE("p=%d   pp=%d   phi1=%f     phi=%f\n",p ,pp, phi1, phi);
			m_RHS[p] +=  phi* m_Sigma[pp];
		}
		SetProgress((int)(100*p/m_MatSize));
	}
	return true;
}


bool C3DPanelDlg::CreateMatrix()
{
	SetProgress(0);

	int p,pp;
	int pos = 0;
	double phi1 = 0.0;
	double phi = 0.0; 
	CVector  V;
	AddString("      Creating the influence matrix...\r\n");

/*	CVector C = m_pPanel[0].CollPt;
	CVector P = C;
	for (int i=-100; i<100; i++){
		P = C + m_pPanel[0].Normal * (double)i ;
		DoubletPotential(0, P, phi);
		TRACE("%.5e\n", phi);
	}*/

	for(p=0; p<m_MatSize; p++){
		if(m_bCancel) return false;
		for(pp=0; pp<m_MatSize; pp++){
			if(m_bCancel) return false;
//			Doublet(p, pp, V, phi);
//			phi1 = phi/4.0/pi;
			DoubletPotential(pp, m_pPanel[p].CollPt, phi);
if(p==0) TRACE("p=%d   pp=%d   phi1=%e     phi=%e\n",p ,pp, phi1, phi);
if(p==30) TRACE("p=%d   pp=%d   phi1=%e     phi=%e\n",p ,pp, phi1, phi);
			m_aij[p*m_MatSize+pp] = phi;
		}	
		SetProgress((int)(100*p/m_MatSize));
	}
//double row[VLMMATSIZE];
//memcpy(row, m_aij, sizeof(row));
	return true;
}

bool C3DPanelDlg::SolveMultiple(double V0, double VDelta, int nval)
{
	// solves the linear system for all DeltaAlphas (or Delta QInfs) simultaneously
	// solves the system for vortices Strength stored in the m_Gamma array
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

	SetProgress(0);
	if(!Gauss(m_aij,Size, m_RHS, nrhs)){
		AddString("      Singular Matrix.... Aborting calculation...\r\n");
		m_bConverged = false;
		return false;
	}
	else m_bConverged = true;

//memcpy(row, m_row, sizeof(row));
	AddString("      Calculating the doublet strengths...\r\n");

	//so far we have a unit Vortex Strength
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
//				CrossProduct(VInf, m_pPanel[p].Vortex, Force);
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
				Lift  *= 2.0/m_pWing->m_Area/1000000.0;//*cos(aoa)
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
				m_Mu[p] = m_RHS[q*Size+pp]*m_3DQInf[q];
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

//	memcpy(row, m_Mu, sizeof(row));
	AddString("\r\n");
	return true;
}


void C3DPanelDlg::ComputePlane(double V0, double VDelta, int nrhs)
{
	// calculates the various wing coefficients by interpolating
	// the adequate variable, from Cl, on the XFoil polar mesh
	// at each span station
	int i,pos, Station;
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	
	double Lift, IDrag, VDrag ,XCP, YCP, Rm, IYm, GYm, LinPm;
	double WingLift, WingIDrag;

	m_pWing->m_pVLMDlg  = this;
	m_pWing->m_bTrace   = true;

	if(m_pStab) {
		m_pStab->m_pVLMDlg  = this;
		m_pStab->m_bTrace   = true;
	}
	if(m_pFin){
		m_pFin->m_pVLMDlg  = this;
		m_pFin->m_bTrace   = true;
	}
	for(i=0; i<=nrhs; i++){
		if(m_bCancel) break;

		ComputeSurfaceSpeed(m_Mu+i*m_MatSize, m_Sigma+i*m_MatSize);

		m_pWing->m_bWingOut = false;
		if(m_pStab) m_pStab->m_bWingOut = false;
		if(m_pFin)  m_pFin->m_bWingOut  = false;

		if(m_pWPolar->m_Type!=4) m_Alpha = V0+i*VDelta;		
		if(!m_pWPolar->m_bTiltedGeom) m_AlphaCalc = m_Alpha;

		m_QInf = m_3DQInf[i];

		if(m_QInf >0.0) {
			CString str;
			if(m_pWPolar->m_Type!=4) str.Format("   ...Alpha=%.2f\r\n", m_Alpha);
			else                     str.Format("   ...QInf = %6.2f\r\n", m_QInf);
			AddString(str);
//			VLMSetAi(m_Gamma+i*m_MatSize);

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
		
			m_CL          =  2.0*Lift /m_QInf/m_pWing->m_Area/1000000.0;
			m_InducedDrag = -1.0*IDrag/m_QInf/m_pWing->m_Area/1000000.0;
			m_ViscousDrag =  1.0*VDrag       /m_pWing->m_Area/1000000.0;

			m_XCP         = XCP/Lift;
			m_YCP         = YCP/Lift;

//			SetDownwash(m_Gamma+i*m_MatSize);

			m_IYm        =         IYm   / m_pWing->m_Area/1000000.0/m_pWing->m_Span;
			m_GYm         =  2.0 * GYm   / m_pWing->m_Area/1000000.0/m_pWing->m_Span  /m_QInf/m_QInf;
			m_Rm          = -2.0 * Rm    / m_pWing->m_Area/1000000.0/m_pWing->m_Span  /m_QInf/m_QInf;
			m_GCm         =  2.0 * LinPm / m_pWing->m_Area/1000000.0/m_pWing->m_MAChord/m_QInf/m_QInf;
			m_VCm         =        m_pWing->m_VCm;
			m_TCm         =  m_GCm + m_pWing->m_VCm;
			if(m_pStab) m_TCm += m_pStab->m_VCm;
			if(m_pFin)  m_TCm += m_pFin->m_VCm;

			if(m_bPointOut) m_bWarning = true;
//			if(m_bConverged){
				if(m_pPlane)
					pMiarex->AddPOpp(m_Mu+i*m_MatSize);
				else
					pMiarex->AddWOpp(m_bPointOut, m_Mu+i*m_MatSize);		
//			}
			AddString("     \r\n");
		}
		else m_bPointOut = true;
	}
}


void C3DPanelDlg::Doublet(int J, int K, CVector &V, double &phi)
{
	//VSAERO theory Manual
	//Influence of panel pp at coll pt of panel p
	double eps = 1.e-7;
	CVector PJK, a, b, s, T;
	CVector R[5];
	double RNUM, DNOM, PN, A, B, PA, PB, SM, SL, AM, AL, Al, pjk;

	phi = 0.0;
	V.Set(0.0,0.0,0.0);

/*	if(J==K){
		phi = -2.0*pi;
		return;
	}*/

	PJK = m_pPanel[J].CollPt - m_pPanel[K].CollPt;
	PN  = PJK.dot(m_pPanel[K].Normal);
	pjk = PJK.VAbs();

	if(pjk> RFF*m_pPanel[K].Size){ // use far-field formula
		phi = PN * m_pPanel[K].Area /pjk/pjk/pjk;
		V   = (PJK*3.0*PN - m_pPanel[K].Normal*pjk*pjk)*m_pPanel[K].Area /pjk/pjk/pjk/pjk/pjk;
		return;
	}
	else{
		phi = 0.0;
		V.Set(0.0,0.0,0.0);

		if(m_pPanel[K].m_iPos>=0){
			R[0] = m_pNode[m_pPanel[K].m_iLA];
			R[1] = m_pNode[m_pPanel[K].m_iTA];
			R[2] = m_pNode[m_pPanel[K].m_iTB];
			R[3] = m_pNode[m_pPanel[K].m_iLB];
			R[4] = m_pNode[m_pPanel[K].m_iLA];	
		}
		else{
			R[0] = m_pNode[m_pPanel[K].m_iLB];
			R[1] = m_pNode[m_pPanel[K].m_iTB];
			R[2] = m_pNode[m_pPanel[K].m_iTA];
			R[3] = m_pNode[m_pPanel[K].m_iLA];
			R[4] = m_pNode[m_pPanel[K].m_iLB];	
		}
		for (int i=0; i<4; i++){
			a  = m_pPanel[J].CollPt - R[i];
			b  = m_pPanel[J].CollPt - R[i+1];
			s    = R[i+1] - R[i];
			A    = a.VAbs();
			B    = b.VAbs();
			SM   = s.dot(m_pPanel[K].m);
			SL   = s.dot(m_pPanel[K].l);
			AM   = a.dot(m_pPanel[K].m);
			AL   = a.dot(m_pPanel[K].l);
			Al   = AM*SL - AL*SM;
			PA   = PN*PN*SL + Al*AM;
			PB   = PA - Al*SM;

//first the potential
			RNUM = SM*PN * (B*PA-A*PB);
			DNOM = PA*PB + PN*PN*A*B*SM*SM;

			if(abs(PN)<eps){
				if(DNOM<0.0){
					if(PN>0.0)	phi += pi;
					else		phi -= pi;
				}
				else if(DNOM == 0.0){
					if(PN>0.0)	phi += pi/2.0;
					else		phi -= pi/2.0;
				}
				else
					phi += 0.0;
			}
			else {
				phi += atan2(RNUM,DNOM);
			}

// next the induced velocity
			T = (a * b) * (A+B) /A/B/ (A*B + DotProduct(a,b));
			V += T;
		}
	}
/*	if(m_pPanel[K].m_bIsTrailing){
		//then it is shedding a wake
		//we need to create a specific wake panel
		//and add its influence

		//create a horizontal wake panel for the present time
		CVector m,l,C,N;
		T.Set(0.0,0.0,0.0);

		double phiw = 0.0;
		N.Set(0.0,0.0,1.0);
		double xw = m_pNode[m_pPanel[K].m_iTA].x - m_pNode[m_pPanel[K].m_iLA].x;

		R[0] = m_pNode[m_pPanel[K].m_iTA];
		R[1] = m_pNode[m_pPanel[K].m_iTA];
		R[1].x += xw;
		R[2] = m_pNode[m_pPanel[K].m_iTB];
		R[2].x += xw;
		R[3] = m_pNode[m_pPanel[K].m_iTB];
		R[4] = m_pNode[m_pPanel[K].m_iTA];	

		C = (R[0]+R[1]+R[2]+R[3])/4.0;

		if(m_pPanel[K].m_iPos>=0)	m = (R[2]+R[3]) *0.5 - C;
		else						m = (R[0]+R[1]) *0.5 - C;
		m.Normalize();

		l = m * N;

		PJK = m_pPanel[J].CollPt - C;
		PN  = PJK.dot(N);
		pjk = PJK.VAbs();

		for (int i=0; i<4; i++){
			a  = m_pPanel[J].CollPt - R[i];
			b  = m_pPanel[J].CollPt - R[i+1];
			s    = R[i+1] - R[i];
			A    = a.VAbs();
			B    = b.VAbs();
			SM   = DotProduct(s, m);
			SL   = DotProduct(s, l);
			AM   = DotProduct(a, m);
			AL   = DotProduct(a, l);
			Al   = AM*SL - AL*SM;
			PA   = PN*PN*SL + Al*AM;
			PB   = PA - Al*SM;

//first the potential
			RNUM = SM*PN * (B*PA-A*PB);
			DNOM = PA*PB + PN*PN*A*B*SM*SM;

			if(abs(PN)<eps){
				if(DNOM<0.0){
					if(PN>0.0)	phiw += pi;
					else		phiw -= pi;
				}
				else if(DNOM == 0.0){
					if(PN>0.0)	phiw += pi/2.0;
					else		phiw -= pi/2.0;
				}
				else
					phiw += 0.0;
			}
			else 
				phiw += atan2(RNUM,DNOM);

// next the induced velocity
	
			T += (a * b) * (A+B) / A/B/ (A*B + DotProduct(a,b));
		}
//        V   += T;
//		phi += phiw;
		if(m_pPanel[K].m_iPos==-1){
			V   += T;
			phi += phiw;
		}
		else {
			V   -= T;
			phi -= phiw;
		}
	}*/
}

void C3DPanelDlg::Source(int J, int K, CVector &V, double &phi)
{
	//VSAERO theory Manual
	//Influence of panel pp at coll pt of panel p
	double eps = 1.e-7;
	CVector PJK, a, b, s, T1, T2, T;
	CVector R[5];
	double PN, A, B, PA, PB, S, SM, SL, AM, AL, Al, GL, CJKi, pjk;
	double RNUM, DNOM;
	phi = 0.0;
	V.Set(0.0,0.0,0.0);

	PJK = m_pPanel[J].CollPt - m_pPanel[K].CollPt;
	PN  = PJK.dot(m_pPanel[K].Normal);
	pjk = PJK.VAbs();

	if(pjk> RFF*m_pPanel[K].Size){
		// use far-field formula
		phi = m_pPanel[K].Area /pjk;
		V = PJK * m_pPanel[K].Area/pjk/pjk/pjk;
		return;
	}
	else{
		phi = 0.0;
		V.Set(0.0,0.0,0.0);
		if(m_pPanel[K].m_iPos>=0){
			R[0] = m_pNode[m_pPanel[K].m_iLA];
			R[1] = m_pNode[m_pPanel[K].m_iTA];
			R[2] = m_pNode[m_pPanel[K].m_iTB];
			R[3] = m_pNode[m_pPanel[K].m_iLB];
			R[4] = m_pNode[m_pPanel[K].m_iLA];	
		}
		else{
			R[0] = m_pNode[m_pPanel[K].m_iLB];
			R[1] = m_pNode[m_pPanel[K].m_iTB];
			R[2] = m_pNode[m_pPanel[K].m_iTA];
			R[3] = m_pNode[m_pPanel[K].m_iLA];
			R[4] = m_pNode[m_pPanel[K].m_iLB];	
		}
		for (int i=0; i<4; i++){
			a  = m_pPanel[J].CollPt - R[i];
			b  = m_pPanel[J].CollPt - R[i+1];
			s    = R[i+1] - R[i];
			A    = a.VAbs();
			B    = b.VAbs();
			S    = s.VAbs();
			SM   = DotProduct(s, m_pPanel[K].m);
			SL   = DotProduct(s, m_pPanel[K].l);
			AM   = DotProduct(a, m_pPanel[K].m);
			AL   = DotProduct(a, m_pPanel[K].l);
			Al   = AM*SL - AL*SM;
			PA   = PN*PN * SL + Al*AM;
			PB   = PA - Al*SM;

//first the potential
			if(S>0.0 && A+B-S>0.0)		GL = 1.0/S * log((A+B+S)/(A+B-S));
			else						GL = 0.0;
			RNUM = SM*PN * (B*PA-A*PB);
			DNOM = PA*PB + PN*PN*A*B*SM*SM;
			if(abs(PN)<eps){
				if(DNOM<0.0){
					if(PN>0.0)	CJKi = pi;
					else		CJKi -= pi;
				}
				else if(DNOM == 0.0){
					if(PN>0.0)	CJKi = pi/2.0;
					else		CJKi = pi/2.0;
				}
				else
					CJKi = 0.0;
			}
			else {
				CJKi = atan2(RNUM, DNOM);
//				CJKi = atan(RNUM/DNOM);
			}
			phi += Al*GL - PN*CJKi;

// next the induced velocity
			T1   = m_pPanel[K].l      * SM*GL;
			T2   = m_pPanel[K].m      * SL*GL;
			T    = m_pPanel[K].Normal * CJKi;
			V   += T + T1 - T2;
		}
	}
}

void C3DPanelDlg::ComputeSurfaceSpeed(double *Mu, double *Sigma)
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
		else DELQ = 0.0;

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
		else DELP = 0.0;

		//find middle of side 2
		S2 = (m_pNode[m_pPanel[p].m_iTA] + m_pNode[m_pPanel[p].m_iTB])/2.0 - m_pPanel[p].CollPt;
		//convert to local coordinates
		Sl2   = m_pPanel[p].GlobalToLocal(S2);
		QInfl = m_pPanel[p].GlobalToLocal(Qp);
		//in panel referential
		Vl.x = (m_pPanel[p].SMP*DELP - Sl2.y*DELQ)/Sl2.x;
		Vl.y =  DELQ;
		Vl.z = -Sigma[p];

		QInfl +=Vl;

		Speed2 = QInfl.x*QInfl.x + QInfl.y*QInfl.y + QInfl.z*QInfl.z;
		m_Cp[p]  = 1.0-Speed2/m_QInf/m_QInf;
//		m_Cp[p]  = m_Mu[p];
//		m_Cp[p]  = QInfl.x;

		if(p<10) {
			CString str;
			str.Format("%d   %f      %f     %f\n", p,m_pPanel[p].CollPt.x,Mu[p],QInfl.x);
			TRACE(str);
		}

		m_Speed[p] = m_pPanel[p].LocalToGlobal(QInfl);
	}
}


CVector C3DPanelDlg::GetSpeedVector(CVector Pt, double *Mu, double *Sigma)
{	
	CVector V;
	int pp;
	CVector VT;
	VT.Set(0.0,0.0,0.0);
	for (pp=0; pp<m_MatSize;pp++){
		DoubletVelocity(pp, Pt, V);
		VT += V * Mu[pp];
		SourceVelocity(pp, Pt, V);
		VT += V * Sigma[pp];
	}
	return VT;
}















