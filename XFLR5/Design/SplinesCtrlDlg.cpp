/****************************************************************************

    SplinesCtrlDlg Class
    Copyright (C) 2006 André Deperrois XFLR5@yahoo.com

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

// SplinesCtrlDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "AFoil.h"
#include ".\splinesctrldlg.h"

// Boîte de dialogue CSplinesCtrlDlg

IMPLEMENT_DYNAMIC(CSplinesCtrlDlg, CDialog)
CSplinesCtrlDlg::CSplinesCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSplinesCtrlDlg::IDD, pParent)
{
	m_pADlg = pParent;
	m_pSF = NULL;
	m_pPF = NULL;
}

CSplinesCtrlDlg::~CSplinesCtrlDlg()
{
}

void CSplinesCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAGridDlg)
	DDX_Control(pDX, IDC_SPINOUT, m_ctrlSpinOut);
	DDX_Control(pDX, IDC_SPININ, m_ctrlSpinIn);
	DDX_Control(pDX, IDC_DEGEXTRADOS, m_ctrlDegExtrados);
	DDX_Control(pDX, IDC_DEGINTRADOS, m_ctrlDegIntrados);
	DDX_Control(pDX, IDC_OUTEXTRADOS, m_ctrlOutExtrados);
	DDX_Control(pDX, IDC_OUTINTRADOS, m_ctrlOutIntrados);
	DDX_Control(pDX, IDC_OUTTEXT, m_ctrlOutText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSplinesCtrlDlg, CDialog)
//{{AFX_MSG_MAP()
	ON_CBN_SELCHANGE(IDC_DEGEXTRADOS, OnSelChangeDegExtrados)
	ON_CBN_SELCHANGE(IDC_DEGINTRADOS, OnSelChangeDegIntrados)
	ON_CBN_EDITCHANGE(IDC_DEGEXTRADOS, OnEditChangeDegExtrados)
	ON_CBN_EDITCHANGE(IDC_DEGINTRADOS, OnEditChangeDegIntrados)
	ON_EN_KILLFOCUS(IDC_OUTEXTRADOS, OnKillFocusOutExtrados)
	ON_EN_KILLFOCUS(IDC_OUTINTRADOS, OnKillFocusOutIntrados)
	ON_WM_VSCROLL()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// Gestionnaires de messages CSplinesCtrlDlg

void CSplinesCtrlDlg::OnSelChangeDegExtrados()
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	m_pSF->m_Extrados.m_iDegree = m_ctrlDegExtrados.GetCurSel()+2;
	m_pSF->Update(true);
	pADlg->UpdateView();
}


void CSplinesCtrlDlg::OnSelChangeDegIntrados() 
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	m_pSF->m_Intrados.m_iDegree = m_ctrlDegIntrados.GetCurSel()+2;
	m_pSF->Update(false);
	pADlg->UpdateView();
}


void CSplinesCtrlDlg::OnKillFocusOutExtrados() 
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	if(pADlg->m_bSF){
		int n = m_ctrlOutExtrados.GetValue();
		if(n<1) m_ctrlOutExtrados.SetValue(1);
		if(n>IQX2-10) m_ctrlOutExtrados.SetValue(IQX2-10);
		m_pSF->m_Extrados.m_iRes = m_ctrlOutExtrados.GetValue();
		m_pSF->Update(true);
	}
	else{
		m_pPF->m_Extrados.m_Freq = m_ctrlOutExtrados.GetValue();
	}
	pADlg->UpdateView();
}


void CSplinesCtrlDlg::OnKillFocusOutIntrados()
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	if(pADlg->m_bSF){
		int n = m_ctrlOutIntrados.GetValue();
		if(n<1) m_ctrlOutIntrados.SetValue(1);
		if(n>IQX2) m_ctrlOutIntrados.SetValue(IQX2);
		m_pSF->m_Intrados.m_iRes = m_ctrlOutIntrados.GetValue();
		m_pSF->Update(false);
	}
	else{
		m_pPF->m_Intrados.m_Freq = m_ctrlOutIntrados.GetValue();
	}
	pADlg->UpdateView();
}

void CSplinesCtrlDlg::OnEditChangeDegExtrados() 
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;

	CString strong;
	int degree;

	m_ctrlDegExtrados.GetWindowText(strong);
	sscanf(strong, "%d", &degree);
	
	if(degree <2) {
		m_ctrlDegExtrados.SetCurSel(0);
	}
	if(degree >4) {
		m_ctrlDegExtrados.SetCurSel(2);
	}
	m_pSF->m_Extrados.m_iDegree = m_ctrlDegExtrados.GetCurSel()+2;
	m_pSF->Update(true);
	pADlg->UpdateView();
}

void CSplinesCtrlDlg::OnEditChangeDegIntrados() 
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	CString strong;
	int degree;

	m_ctrlDegIntrados.GetWindowText(strong);
	sscanf(strong, "%d", &degree);
	
	if(degree <2) {
		m_ctrlDegIntrados.SetCurSel(0);
	}
	if(degree >4) {
		m_ctrlDegIntrados.SetCurSel(2);
	}
	m_pSF->m_Intrados.m_iDegree = m_ctrlDegIntrados.GetCurSel()+2;
	m_pSF->Update(false);
	pADlg->UpdateView();
}



BOOL CSplinesCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	int i;

	CString str;
	m_ctrlDegExtrados.ResetContent();
	m_ctrlDegIntrados.ResetContent();
	for (i=2; i<6; i++){
		str.Format("%d", i);
		m_ctrlDegExtrados.AddString(str);
		m_ctrlDegIntrados.AddString(str);
	}
	m_ctrlDegExtrados.EnableWindow(true);
	m_ctrlDegIntrados.EnableWindow(true);

	if(m_bSF)
	{
		m_ctrlDegExtrados.SetCurSel(m_pSF->m_Extrados.m_iDegree-2);
		m_ctrlDegIntrados.SetCurSel(m_pSF->m_Intrados.m_iDegree-2);
		m_ctrlOutExtrados.SetValue(m_pSF->m_Extrados.m_iRes);
		m_ctrlOutIntrados.SetValue(m_pSF->m_Intrados.m_iRes);
		m_ctrlSpinIn.SetRange(10,140);
		m_ctrlSpinOut.SetRange(10,140);
	}
	else
	{
		m_ctrlDegExtrados.EnableWindow(false);
		m_ctrlDegIntrados.EnableWindow(false);
		m_ctrlOutExtrados.m_iMin = 3;
		m_ctrlOutExtrados.m_iMax = 30;
		m_ctrlOutIntrados.m_iMin = 3;
		m_ctrlOutIntrados.m_iMax = 30;

		m_ctrlOutExtrados.SetValue(m_pPF->m_Extrados.m_Freq);
		m_ctrlOutIntrados.SetValue(m_pPF->m_Intrados.m_Freq);
		m_ctrlOutText.SetWindowText("Output density:");
		m_ctrlSpinIn.SetRange(3,30);
		m_ctrlSpinOut.SetRange(3,30);
	}

	m_ctrlDegExtrados.SetFocus();
	return FALSE; 
}

void CSplinesCtrlDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar->GetSafeHwnd() == GetDlgItem(IDC_SPININ)->GetSafeHwnd())    {
		OnKillFocusOutIntrados();
	}
	if (pScrollBar->GetSafeHwnd() == GetDlgItem(IDC_SPINOUT)->GetSafeHwnd())    {
		OnKillFocusOutExtrados();
	}

//	CInitDialogBar::OnVScroll(nSBCode, nPos, pScrollBar);
}
/*
void CSplinesCtrlDlg::SetParams()
{
	CString str;

	CAFoil* pADlg = (CAFoil*)m_pADlg;
	if(pADlg->m_bSF){
		double gap =  m_pSF->m_Extrados.m_Input[m_pSF->m_Extrados.m_iCtrlPoints].y
					 -m_pSF->m_Intrados.m_Input[m_pSF->m_Intrados.m_iCtrlPoints].y;
		m_ctrlTEGap.SetValue(gap*100.0);
		m_ctrlOutExtrados.m_iMin = 1;
		m_ctrlOutExtrados.m_iMax = IQX2;
		m_ctrlOutIntrados.m_iMin = 1;
		m_ctrlOutIntrados.m_iMax = IQX2;
		
		m_ctrlDegExtrados.ResetContent();
		m_ctrlDegIntrados.ResetContent();
		for (int i=2; i<6; i++){
			str.Format("%d", i);
			m_ctrlDegExtrados.AddString(str);
			m_ctrlDegIntrados.AddString(str);
		}
		m_ctrlDegExtrados.EnableWindow(true);
		m_ctrlDegIntrados.EnableWindow(true);

		m_ctrlDegExtrados.SetCurSel(m_pSF->m_Extrados.m_iDegree);
		m_ctrlDegIntrados.SetCurSel(m_pSF->m_Intrados.m_iDegree);

		m_ctrlOutExtrados.SetValue(m_pSF->m_Extrados.m_iRes);
		m_ctrlOutIntrados.SetValue(m_pSF->m_Intrados.m_iRes);

		m_ctrlOutText.SetWindowText("Output pts:");
		m_ctrlSpinIn.SetRange(10,140);
		m_ctrlSpinOut.SetRange(10,140);
	}
	else{
		double gap =  m_pPF->m_Extrados.m_ctrlPoint[m_pPF->m_Extrados.m_iPoints].y
					-m_pPF->m_Intrados.m_ctrlPoint[m_pPF->m_Intrados.m_iPoints].y;
		m_ctrlTEGap.SetValue(gap*100.0);

		m_ctrlDegExtrados.EnableWindow(false);
		m_ctrlDegIntrados.EnableWindow(false);
		m_ctrlOutExtrados.m_iMin = 3;
		m_ctrlOutExtrados.m_iMax = 30;
		m_ctrlOutIntrados.m_iMin = 3;
		m_ctrlOutIntrados.m_iMax = 30;

		m_ctrlOutExtrados.SetValue(m_pPF->m_Extrados.m_Freq);
		m_ctrlOutIntrados.SetValue(m_pPF->m_Intrados.m_Freq);
		m_ctrlOutText.SetWindowText("Output density:");
		m_ctrlSpinIn.SetRange(3,30);
		m_ctrlSpinOut.SetRange(3,30);
	}

}*/