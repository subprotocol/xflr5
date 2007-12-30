/****************************************************************************

	Full inverse controls class

	Copyright (C) 2004 André Deperrois xflr5@yahoo.com

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
// FInvCtrlBar.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../misc/InitDialogBar.h"
#include "FInvCtrlBar.h"
#include "XInverse.h"



IMPLEMENT_DYNAMIC(CFInvCtrlBar, CDialogBar)

BEGIN_MESSAGE_MAP(CFInvCtrlBar, CDialogBar)
//{{AFX_MSG_MAP(CFInvCtrlBar)
	ON_BN_CLICKED(IDC_QRESET, OnQReset)
	ON_BN_CLICKED(IDC_APPLYSPLINE, OnApplySpline)
	ON_BN_CLICKED(IDC_EXEC, OnExec)
	ON_BN_CLICKED(IDC_PERT, OnPerturb)
	ON_BN_CLICKED(IDC_SMOOTH, OnSmooth)
	ON_EN_KILLFOCUS(IDC_TANGLE, OnKillFocusTAngle)
	ON_BN_CLICKED(IDC_FILTER, OnFilter)
	ON_BN_CLICKED(IDC_NEWSPLINE, OnNewSpline)
	ON_BN_CLICKED(IDC_SHOWSPLINE, OnShowSpline)
	ON_BN_CLICKED(IDC_TANGENTSPLINE, OnTangentSpline)
	ON_BN_CLICKED(IDC_SPECAL, OnSpecal)
	ON_EN_KILLFOCUS(IDC_SPECINV, OnKillFocusSpec)
	ON_EN_KILLFOCUS(IDC_TGAPDY, OnKillFocusTGap)
	ON_BN_CLICKED(IDC_SYMM, OnSymm)
	ON_BN_CLICKED(IDC_SPECCL, OnSpecal)
	ON_EN_KILLFOCUS(IDC_TGAPDX, OnKillFocusTGap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CFInvCtrlBar::CFInvCtrlBar()
{
	//Set Initial conditions for controls
	m_ctrlTAngle.SetPrecision(3);
	m_ctrlTGapx.SetPrecision(3);
	m_ctrlTGapy.SetPrecision(3);
	m_pXInverse = NULL;
}


CFInvCtrlBar::~CFInvCtrlBar()
{

}



void CFInvCtrlBar::DoDataExchange(CDataExchange* pDX)
{
	ASSERT(pDX);

	CInitDialogBar::DoDataExchange(pDX);

	// DDX_??? functions to associate control with
	// data or control object
	// Call UpdateData(TRUE) to get data at any time
	// Call UpdateData(FALSE) to set data at any time

	//{{AFX_DATA_MAP(CFInvCtrlBar)
	DDX_Control(pDX, IDC_EXEC, m_ctrlExec);
	DDX_Control(pDX, IDC_OUTPUT, m_ctrlOutput);
	DDX_Control(pDX, IDC_SYMM, m_ctrlSymm);
	DDX_Control(pDX, IDC_FILTER, m_ctrlFilter);
	DDX_Control(pDX, IDC_PERT, m_ctrlPert);
	DDX_Control(pDX, IDC_SPECAL, m_ctrlSpecal);
	DDX_Control(pDX, IDC_SPECIF, m_ctrlSpecif);
	DDX_Control(pDX, IDC_SPECINV, m_ctrlSpec);
	DDX_Control(pDX, IDC_NEWSPLINE, m_ctrlNewSpline);
	DDX_Control(pDX, IDC_SMOOTH, m_ctrlSmooth);
	DDX_Control(pDX, IDC_SHOWSPLINE, m_ctrlShowSpline);
	DDX_Control(pDX, IDC_TANGENTSPLINE, m_ctrlTangentSpline);
	DDX_Control(pDX, IDC_TGAPDY, m_ctrlTGapy);
	DDX_Control(pDX, IDC_TGAPDX, m_ctrlTGapx);
	DDX_Control(pDX, IDC_TANGLE, m_ctrlTAngle);
	DDX_Control(pDX, IDC_FILTERPARAM, m_ctrlFilterParam);
	//}}AFX_DATA_MAP
}


void CFInvCtrlBar::OnQReset() 
{
	CancelSpline();
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
	pXInv->ResetQ();
	pXInv->UpdateView();	
}


void CFInvCtrlBar::OnApplySpline() 
{
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if(!pXInv->m_bSplined)  pXInv->ApplySpline();
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
//	pXInv->m_bSpline = false;
	pXInv->m_nPos    = 0;
	pXInv->m_tmpPos  = -1;
	pXInv->m_Pos1    = -1;
	pXInv->m_Pos2    = -1;
//	m_ctrlShowSpline.SetCheck(false);
	
	pXInv->UpdateView();
	m_ctrlShowSpline.Invalidate();
}


void CFInvCtrlBar::OnExec() 
{
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
	pXInv->SetTAngle(m_ctrlTAngle.GetValue());
	pXInv->SetTGap(m_ctrlTGapx.GetValue(),m_ctrlTGapy.GetValue());
	m_ctrlOutput.SetWindowText(" ");
	pXInv->ExecMDES();
	pXInv->UpdateView();
}

void CFInvCtrlBar::OnPerturb() 
{
	CancelSpline();
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
	pXInv->Pertubate();
}

void CFInvCtrlBar::OnFilter() 
{
	CancelSpline();
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();

	double filt = m_ctrlFilterParam.GetValue();
	pXInv->Filter(filt);	
}


void CFInvCtrlBar::OnNewSpline() 
{
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
	if(m_ctrlNewSpline.GetCheck())
	{
		CancelSmooth();
		m_ctrlOutput.SetWindowText("Mark spline endpoints");
		pXInv->m_bSpline = true;
		pXInv->m_bSplined = false;
		m_ctrlShowSpline.SetCheck(1);
		pXInv->m_bSmooth = false;
		pXInv->m_bGetPos = true;
		pXInv->m_nPos    = 0;
		pXInv->m_tmpPos  = -1;
		pXInv->m_Pos1    = -1;
		pXInv->m_Pos2    = -1;
	}
	else
	{
		CancelSpline();
	}
	pXInv->UpdateView();
}



void CFInvCtrlBar::CancelSpline()
{
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	m_ctrlOutput.SetWindowText(" ");
//	pXInv->m_bSpline  = false;
	pXInv->m_bSplined = false;
//	m_ctrlShowSpline.SetCheck(0);
	m_ctrlNewSpline.SetCheck(0);
	pXInv->m_bSmooth = false;
	pXInv->m_bGetPos = false;
	pXInv->m_nPos    = 0;
	pXInv->m_tmpPos  = -1;
	pXInv->m_Pos1    = -1;
	pXInv->m_Pos2    = -1;
}

void CFInvCtrlBar::OnTangentSpline() 
{
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if(m_ctrlTangentSpline.GetCheck())	pXInv->m_bTangentSpline = true;
	else								pXInv->m_bTangentSpline = false;
}

void CFInvCtrlBar::OnShowSpline()
{
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	
	if(m_ctrlShowSpline.GetCheck())	pXInv->m_bSpline = true;
	else							pXInv->m_bSpline = false;
	pXInv->m_bSplined =   !pXInv->m_bSpline;
	pXInv->UpdateView();
}


void CFInvCtrlBar::OnKillFocusSpec() 
{
	if(!IsWindowVisible()) return;
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
	int idc = GetCheckedRadioButton(IDC_SPECAL, IDC_SPECCL);
	if(idc == IDC_SPECAL){
		pXInv->m_pXFoil->alqsp[1] = m_ctrlSpec.GetValue()*3.141592654/180.0;
		pXInv->m_pXFoil->iacqsp = 1;
		pXInv->m_pXFoil->qspcir();
	}
	else{
		pXInv->m_pXFoil->clqsp[1] = m_ctrlSpec.GetValue();
		pXInv->m_pXFoil->iacqsp = 2;
		pXInv->m_pXFoil->qspcir();
	}
	pXInv->CreateQCurve();
	pXInv->CreateMCurve();
	pXInv->UpdateView();
}


BOOL CFInvCtrlBar::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN){
		CXInverse* pXInv = (CXInverse*)m_pXInverse;
		if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
		if (pMsg->wParam == VK_ESCAPE){
			CancelSmooth();
			CancelSpline();
			pXInv->UpdateView();
			return true;
		}
		else if(pMsg->wParam == VK_RETURN){ 
			CWnd* pWnd = GetFocus();
			if(GetDlgItem(IDC_SPECINV) == pWnd ){
				OnKillFocusSpec();
				m_ctrlExec.SetFocus();
				return true;
			}
			else if(GetDlgItem(IDC_TANGLE) == pWnd ){
				OnKillFocusTAngle();
				m_ctrlExec.SetFocus();
				return true;
			}
			else if(GetDlgItem(IDC_TGAPDX) == pWnd || GetDlgItem(IDC_TGAPDY) == pWnd){
				OnKillFocusTGap();
				m_ctrlExec.SetFocus();
				return true;
			}
			else if (m_ctrlSmooth.GetCheck()) {
				pXInv->Smooth(-1);
				pXInv->m_bGetPos = false;
				pXInv->m_bSmooth = false;
				return true;
			}
			else{
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
	return CWnd::PreTranslateMessage(pMsg);
}


void CFInvCtrlBar::OnKillFocusTAngle() 
{
	if(!IsWindowVisible()) return;
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
	pXInv->SetTAngle(m_ctrlTAngle.GetValue());
}


void CFInvCtrlBar::OnKillFocusTGap() 
{
	if(!IsWindowVisible()) return;
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
	pXInv->SetTGap(m_ctrlTGapx.GetValue(),m_ctrlTGapy.GetValue());
}

void CFInvCtrlBar::OnSymm() 
{
	CancelSpline();
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if (pXInv->m_bZoomPlus) pXInv->ReleaseZoom();
	if(m_ctrlSymm.GetCheck()) pXInv->m_pXFoil->lqsym = true;
	else pXInv->m_pXFoil->lqsym = false;
	pXInv->m_pXFoil->lqspec = false;

}


BOOL CFInvCtrlBar::OnInitDialogBar()
{
	// Support for DDX mechanism
	// If you do not want DDX then
	// do not call base class
	CInitDialogBar::OnInitDialogBar();

	// Update any controls NOT supported by DDX
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if(pXInv->m_bTangentSpline) m_ctrlTangentSpline.SetCheck(TRUE);
	m_ctrlFilterParam.SetValue(0.2);

	return TRUE;
}

void CFInvCtrlBar::OnSpecal() 
{
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	int idc = GetCheckedRadioButton(IDC_SPECAL, IDC_SPECCL);
	if(idc == IDC_SPECAL){
		m_ctrlSpecif.SetWindowText("Alpha = ");
		m_ctrlSpec.SetPrecision(2);
		m_ctrlSpec.SetValue(pXInv->m_pXFoil->alqsp[1]*180.0/3.141592654);
	}
	else{
		m_ctrlSpecif.SetWindowText("Cl = ");
		m_ctrlSpec.SetPrecision(3);
		m_ctrlSpec.SetValue(pXInv->m_pXFoil->clqsp[1]);
	}
}


void CFInvCtrlBar::OnSmooth() 
{
	CancelSpline();
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	if(m_ctrlSmooth.GetCheck()){
		m_ctrlOutput.SetWindowText("Mark target segment for smoothing, or type 'Return' to smooth the entire distribution");

		pXInv->m_bSpline = false;
		pXInv->m_bSmooth = true;
		pXInv->UpdateView();
		pXInv->m_bGetPos = true;
		pXInv->m_nPos    = 0;
	}
	else CancelSmooth();
}


void CFInvCtrlBar::CancelSmooth()
{
	CXInverse* pXInv = (CXInverse*)m_pXInverse;
	pXInv->m_bSmooth = false;
	pXInv->m_bGetPos = false;
	m_ctrlSmooth.SetCheck(0);
}
