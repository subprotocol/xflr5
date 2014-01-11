/****************************************************************************

	Panel dialog options

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
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "XDirect.h"
#include "2DPanelDlg.h"
#include ".\2dpaneldlg.h"



/////////////////////////////////////////////////////////////////////////////
// C2DPanelDlg dialog


C2DPanelDlg::C2DPanelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(C2DPanelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(C2DPanelDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pChildView = NULL;
	m_bModified = false;
	m_bApplied  = false;

	cterat = 0.f;
	ctrrat = 0.f;
	cvpar  = 0.f;
	npan = 0;
	Title = "";
	xpref1 = 0.f;
	xpref2 = 0.f;
	xsref1 = 0.f;
	xsref2 = 0.f;
}


void C2DPanelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(C2DPanelDlg)
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_NPANELS, m_ctrlNPanels);
	DDX_Control(pDX, IDC_CVPAR, m_ctrlCVpar);
	DDX_Control(pDX, IDC_CTRRAT, m_ctrlCTRrat);
	DDX_Control(pDX, IDC_CTERAT, m_ctrlCTErat);
	DDX_Control(pDX, IDC_XSREF1, m_ctrlXsRef1);
	DDX_Control(pDX, IDC_XSREF2, m_ctrlXsRef2);
	DDX_Control(pDX, IDC_XPREF1, m_ctrlXpRef1);
	DDX_Control(pDX, IDC_XPREF2, m_ctrlXpRef2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(C2DPanelDlg, CDialog)
	//{{AFX_MSG_MAP(C2DPanelDlg)
	ON_EN_CHANGE(IDC_NPANELS, OnChanged)
	ON_EN_CHANGE(IDC_CVPAR,  OnChanged)
	ON_EN_CHANGE(IDC_CTERAT, OnChanged)
	ON_EN_CHANGE(IDC_CTRRAT, OnChanged)
	ON_EN_CHANGE(IDC_XSREF1, OnChanged)
	ON_EN_CHANGE(IDC_XPREF1, OnChanged)
	ON_EN_CHANGE(IDC_XSREF2, OnChanged)
	ON_EN_CHANGE(IDC_XPREF2, OnChanged)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C2DPanelDlg message handlers

void C2DPanelDlg::OnChanged() 
{
	m_bApplied = false;
}


void C2DPanelDlg::OnApply() 
{
	if(m_bApplied) return;
	//reset everything and retry

	for (int i=0; i< m_pMemFoil->nb; i++){
		m_pXFoil->xb[i+1] = m_pMemFoil->xb[i] ;
		m_pXFoil->yb[i+1] = m_pMemFoil->yb[i];
	}
	m_pXFoil->nb = m_pMemFoil->nb;

	m_pXFoil->lflap = false;
	m_pXFoil->lbflap = false;

	if(m_pXFoil->Preprocess()){
		m_pXFoil->CheckAngles();

	}
	else {
		AfxMessageBox("Unrecognized foil format", MB_OK);
		return;
	}

	ReadParams();

	m_pXFoil->pangen();
	if(m_pXFoil->n>350) {
		AfxMessageBox("Panel number cannot exceed 350", MB_OK);
		//reset everything and retry
		for (int i=0; i< m_pMemFoil->nb; i++){
			m_pXFoil->x[i+1] = m_pMemFoil->xb[i] ;
			m_pXFoil->y[i+1] = m_pMemFoil->yb[i];
		}
		m_pXFoil->n = m_pMemFoil->nb;
	}
	else{
		for (int j=0; j< m_pXFoil->n; j++){
			m_pBufferFoil->xb[j] = m_pXFoil->x[j+1];
			m_pBufferFoil->yb[j] = m_pXFoil->y[j+1];
		}
		m_pBufferFoil->nb = m_pXFoil->n;
//		m_pXFoil->SetFoilFlap(m_pBufferFoil);
		m_pBufferFoil->InitFoil();
		m_pBufferFoil->SetFlap();
	}
	m_bApplied = true;
	m_bModified = true;
	m_pChildView->Invalidate();
	
}

BOOL C2DPanelDlg::PreTranslateMessage(MSG* pMsg) 
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)){
		CWnd* ppp = GetFocus();

		if(GetDlgItem(IDOK) == ppp) 
			OnOK();
		else if(GetDlgItem(IDCANCEL) == ppp) 
			OnCancel();
		else {
			OnApply();
			m_ctrlOK.SetFocus();
		}
		return true;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL C2DPanelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//memorize initial values
//	npan   = m_pXFoil->n;
	npan   = m_pMemFoil->nb;
	cvpar  = m_pXFoil->cvpar;
	cterat = m_pXFoil->cterat;
	ctrrat = m_pXFoil->ctrrat;
	xsref1 = m_pXFoil->xsref1;
	xsref2 = m_pXFoil->xsref2; 
	xpref1 = m_pXFoil->xpref1;
	xpref2 = m_pXFoil->xpref2;

	m_ctrlNPanels.SetValue(npan);
	m_ctrlNPanels.SetMin(0); m_ctrlNPanels.SetMax(300);
	m_ctrlCVpar.SetValue(cvpar);
	m_ctrlCTErat.SetValue(cterat);
	m_ctrlCTRrat.SetValue(ctrrat);
	m_ctrlXsRef1.SetValue(xsref1);
	m_ctrlXsRef2.SetValue(xsref2);
	m_ctrlXpRef1.SetValue(xpref1);
	m_ctrlXpRef2.SetValue(xpref2);
	SetWindowText(Title);
	m_ctrlNPanels.SetFocus();

	return FALSE; 
}


void C2DPanelDlg::OnOK()
{
	if(!m_bModified) EndDialog(0);
	else {
		OnApply();
		EndDialog(1);
	}
//	CDialog::OnOK();
}


void C2DPanelDlg::ReadParams()
{
	m_pXFoil->npan   = m_ctrlNPanels.GetValue();
	m_pXFoil->cvpar  = m_ctrlCVpar.GetValue();
	m_pXFoil->cterat = m_ctrlCTErat.GetValue();
	m_pXFoil->ctrrat = m_ctrlCTRrat.GetValue();
	m_pXFoil->xsref1 = m_ctrlXsRef1.GetValue();
	m_pXFoil->xsref2 = m_ctrlXsRef2.GetValue();
	m_pXFoil->xpref1 = m_ctrlXpRef1.GetValue();
	m_pXFoil->xpref2 = m_ctrlXpRef2.GetValue();

}

