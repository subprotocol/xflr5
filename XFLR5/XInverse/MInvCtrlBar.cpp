/****************************************************************************

	Mixed inverse control dialog

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

#include "stdafx.h"
#include "../X-FLR5.h"
#include "MInvCtrlBar.h"
#include "XInverse.h"

IMPLEMENT_DYNAMIC(CMInvCtrlBar, CInitDialogBar)

BEGIN_MESSAGE_MAP(CMInvCtrlBar, CInitDialogBar)
//{{AFX_MSG_MAP(CMInvCtrlBar)
	ON_BN_CLICKED(IDC_CPXX, OnCpxx)
	ON_BN_CLICKED(IDC_EXEC, OnExec)
	ON_BN_CLICKED(IDC_NEWSPLINE, OnNewSpline)
	ON_BN_CLICKED(IDC_QRESET, OnQReset)
	ON_BN_CLICKED(IDC_TANGENTSPLINE, OnTangentSpline)
	ON_BN_CLICKED(IDC_SHOWSPLINE, OnShowSpline)
	ON_BN_CLICKED(IDC_SMOOTH, OnSmooth)
	ON_BN_CLICKED(IDC_APPLYSPLINE, OnApplySpline)
	ON_BN_CLICKED(IDC_MARK, OnMarkSegment)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CMInvCtrlBar::CMInvCtrlBar()
{
	//Set Initial conditions for controls
	m_pXInverse = NULL;
}


CMInvCtrlBar::~CMInvCtrlBar()
{

}



void CMInvCtrlBar::DoDataExchange(CDataExchange* pDX)
{
	ASSERT(pDX);

	CInitDialogBar::DoDataExchange(pDX);

	// DDX_??? functions to associate control with
	// data or control object
	// Call UpdateData(TRUE) to get data at any time
	// Call UpdateData(FALSE) to set data at any time

	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_SPECINV, m_ctrlSpec);
	DDX_Control(pDX, IDC_OUTPUT, m_ctrlOutput);
	DDX_Control(pDX, IDC_INVITER, m_ctrlIter);
	DDX_Control(pDX, IDC_MARK, m_ctrlMark);
	DDX_Control(pDX, IDC_NEWSPLINE, m_ctrlNewSpline);
	DDX_Control(pDX, IDC_TANGENTSPLINE, m_ctrlTangentSpline);
	DDX_Control(pDX, IDC_SMOOTH, m_ctrlSmooth);
	DDX_Control(pDX, IDC_SHOWSPLINE, m_ctrlShowSpline);
	DDX_Control(pDX, IDC_EXEC, m_ctrlExec);
	DDX_Control(pDX, IDC_CPXX, m_ctrlCpxx);
	//}}AFX_DATA_MAP
} 


BOOL CMInvCtrlBar::OnInitDialogBar()
{
	// Support for DDX mechanism
	// If you do not want DDX then
	// do not call base class
	CInitDialogBar::OnInitDialogBar();

	// Update any controls NOT supported by DDX
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if(pXInv->m_bTangentSpline) m_ctrlTangentSpline.SetCheck(TRUE);
//	m_ctrlIter.SetValue(pXInv->m_pXFoil->niterq);

	return TRUE;
}
void CMInvCtrlBar::OnMarkSegment() 
{
	CancelSpline();
	CancelSmooth();
	CXInverse * pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
	
	if(m_ctrlMark.GetCheck()) m_ctrlOutput.SetWindowText("Mark target segment for modification");

	pXInv->m_tmpPos  = -1;
	pXInv->m_bMark   = true;
	pXInv->m_bMarked = false;
	pXInv->m_bGetPos = true;
	pXInv->m_bSpline = false;
	pXInv->m_nPos    = 0;

	m_ctrlNewSpline.SetCheck(0);
	m_ctrlShowSpline.SetCheck(0);

	pXInv->UpdateView();	
}


void CMInvCtrlBar::OnCpxx() 
{
	CancelSpline();
	CancelSmooth();
	CancelMark();
	CXInverse * pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
	if(m_ctrlCpxx.GetCheck()) pXInv->m_pXFoil->lcpxx = true;
	else  pXInv->m_pXFoil->lcpxx = false;
}


void CMInvCtrlBar::OnExec() 
{
	CancelSpline();
	CancelSmooth();
	CancelMark();
	CXInverse * pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
	pXInv->m_pXFoil->niterq = m_ctrlIter.GetValue();
	m_ctrlOutput.SetWindowText(" ");
	pXInv->ExecQDES();
	pXInv->UpdateView();	
}


void CMInvCtrlBar::OnQReset() 
{
	CancelSpline();
	CancelSmooth();
	CancelMark();
	CXInverse * pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
	pXInv->ResetMixedQ();
	pXInv->UpdateView();
}


void CMInvCtrlBar::OnNewSpline() 
{
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
	if(m_ctrlNewSpline.GetCheck()){
		CancelMark();
		CancelSmooth();
		m_ctrlOutput.SetWindowText("Mark spline endpoints");
		pXInv->m_bSpline  = true;
		pXInv->m_bSplined = false;
		m_ctrlShowSpline.SetCheck(1);
		pXInv->m_bGetPos = true;
		pXInv->m_tmpPos  = -1;
		pXInv->m_nPos    = 0;
	}
	else {
		CancelSpline();
	}
	pXInv->UpdateView();	
}

void CMInvCtrlBar::OnTangentSpline() 
{
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if(m_ctrlTangentSpline.GetCheck())	pXInv->m_bTangentSpline = true;
	else								pXInv->m_bTangentSpline = false;
}


void CMInvCtrlBar::OnShowSpline() 
{
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
	if(m_ctrlShowSpline.GetCheck())	pXInv->m_bSpline = true;
	else							pXInv->m_bSpline = false;
	pXInv->m_bSplined =   !pXInv->m_bSpline;

	pXInv->UpdateView();
}


void CMInvCtrlBar::OnSmooth() 
{
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
	m_ctrlOutput.SetWindowText("Mark target segment for smoothing or 'Return' to smooth entire distribution");
	CancelSpline();
	CancelMark();
	if(m_ctrlSmooth.GetCheck()){
		pXInv->m_bSmooth = true;
		pXInv->m_bGetPos = true;
		pXInv->m_nPos    = 0;
	}
	else CancelSmooth();
	pXInv->UpdateView();
}


void CMInvCtrlBar::OnApplySpline()
{
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
	if(!pXInv->m_bSplined) pXInv->ApplySpline();
//	CancelSpline();
	
	pXInv->UpdateView();
}


BOOL CMInvCtrlBar::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN){
		CWnd *pWnd = GetFocus();
		CXInverse* pXInv = (CXInverse*)m_pXInverse;
		if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
		if (pMsg->wParam == VK_ESCAPE){
			CancelSmooth();
			CancelSpline();
			CancelMark();
			pXInv->UpdateView();
			return true;
		}
		if (pMsg->wParam == VK_RETURN){
			if( pXInv->m_bSmooth) {
				pXInv->Smooth(-1);
				CancelSmooth();
				return true;
			}
			else if (pWnd != &m_ctrlExec){
				m_ctrlExec.SetFocus();
				return true;
			}
			else if(pWnd == &m_ctrlExec){
				OnExec();
				return true;
			}
		}
	}	
	else if(pMsg->message == WM_MOUSEWHEEL){
		CXInverse* pXInv = (CXInverse*)m_pXInverse;

		CPoint pt((short) LOWORD(pMsg->lParam),(short) HIWORD(pMsg->lParam));

		pXInv->OnMouseWheel(LOWORD(pMsg->wParam),(short) HIWORD(pMsg->wParam), pt);
		pXInv->m_pChildWnd->SetFocus();
		return true;
	}
	return CInitDialogBar::PreTranslateMessage(pMsg);
}

void CMInvCtrlBar::CancelSmooth()
{
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
//	pXInv->m_bSpline  = false;
	pXInv->m_bSplined = false;
	pXInv->m_bGetPos  = false;
	pXInv->m_bMark    = false;
	pXInv->m_nPos    =  0;
	pXInv->m_tmpPos  = -1;
	pXInv->m_Pos1    = -1;
	pXInv->m_Pos2    = -1;
//	m_ctrlShowSpline.SetCheck(false);
	m_ctrlSmooth.SetCheck(false);
}

void CMInvCtrlBar::CancelSpline()
{
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
//	pXInv->m_bSpline  = false;
	pXInv->m_bSplined = false;
	pXInv->m_bGetPos  = false;
	pXInv->m_bMark    = false;
	pXInv->m_nPos    =  0;
	pXInv->m_tmpPos  = -1;
	pXInv->m_Pos1    = -1;
	pXInv->m_Pos2    = -1;
	m_ctrlNewSpline.SetCheck(false);
//	m_ctrlShowSpline.SetCheck(false);
}

void CMInvCtrlBar::CancelMark()
{
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	m_ctrlMark.SetCheck(false);
	pXInv->m_bGetPos = false;
	pXInv->m_bMark   = false;
}
