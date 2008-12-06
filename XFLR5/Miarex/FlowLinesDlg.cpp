/****************************************************************************
	
	FlowLinesDlg	class
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

// FlowLinesDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "Miarex.h"
#include ".\flowlinesdlg.h"


// Boîte de dialogue CFlowLinesDlg

IMPLEMENT_DYNAMIC(CFlowLinesDlg, CDialog)
CFlowLinesDlg::CFlowLinesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFlowLinesDlg::IDD, pParent)
{
	m_pMiarex   = pParent;
	m_pChildWnd = NULL;
	m_pFrame    = NULL;
	m_pos       = 1;
	m_NX        =   30;
	m_XFactor   = 1.10;
	m_DeltaL    =  0.01;
	m_XOffset   =  0.0;
	m_ZOffset   =  0.0;
}

CFlowLinesDlg::~CFlowLinesDlg()
{
}

void CFlowLinesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NX, m_ctrlNX);
	DDX_Control(pDX, IDC_XOFFSET, m_ctrlXOffset);
	DDX_Control(pDX, IDC_ZOFFSET, m_ctrlZOffset);
	DDX_Control(pDX, IDC_XFACTOR, m_ctrlXFactor);
	DDX_Control(pDX, IDC_DELTAL, m_ctrlDeltaL);
	DDX_Control(pDX, IDC_APPLY, m_ctrlApply);
	DDX_Control(pDX, IDC_LENGTH1, m_ctrlLength1);
	DDX_Control(pDX, IDC_LENGTH2, m_ctrlLength2);
	DDX_Control(pDX, IDC_LENGTH3, m_ctrlLength3);
}


BEGIN_MESSAGE_MAP(CFlowLinesDlg, CDialog)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_WM_CLOSE()
	ON_EN_KILLFOCUS(IDC_NX, OnChanged)
	ON_EN_KILLFOCUS(IDC_XOFFSET, OnChanged)
	ON_EN_KILLFOCUS(IDC_ZOFFSET, OnChanged)
	ON_EN_KILLFOCUS(IDC_XFACTOR, OnChanged)
	ON_EN_KILLFOCUS(IDC_DELTAL, OnChanged)
END_MESSAGE_MAP()


// Gestionnaires de messages CFlowLinesDlg

BOOL CFlowLinesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	SetUnits();
	if(m_pFrame) SetParams();
	return TRUE;  
}

void CFlowLinesDlg::SetUnits() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString len;
	GetLengthUnit(len, pFrame->m_LengthUnit);
	m_ctrlLength1.SetWindowText(len);
	m_ctrlLength2.SetWindowText(len);
	m_ctrlLength3.SetWindowText(len);
}


void CFlowLinesDlg::SetParams() 
{
	if(!m_pFrame) return;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	if(m_pos==0)	    CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO1);
	else if(m_pos==1)	CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO2);
	else if(m_pos==2)	CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO3);

	m_ctrlDeltaL.SetValue(m_DeltaL* pFrame->m_mtoUnit);
	m_ctrlXOffset.SetValue(m_XOffset* pFrame->m_mtoUnit);
	m_ctrlZOffset.SetValue(m_ZOffset* pFrame->m_mtoUnit);
	m_ctrlXFactor.SetValue(m_XFactor);
	m_ctrlNX.SetValue(m_NX);
}


void CFlowLinesDlg::ReadParams() 
{
	if(!m_pFrame) return;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	m_NX = m_ctrlNX.GetValue();
	m_XOffset = m_ctrlXOffset.GetValue() / pFrame->m_mtoUnit;
	m_ZOffset = m_ctrlZOffset.GetValue() / pFrame->m_mtoUnit;
	m_DeltaL  = m_ctrlDeltaL.GetValue()  / pFrame->m_mtoUnit;
	m_XFactor = m_ctrlXFactor.GetValue();
	if(GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO3)==IDC_RADIO1)	    m_pos=0;
	else if(GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO3)==IDC_RADIO2)	m_pos=1;
	else if(GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO3)==IDC_RADIO3)	m_pos=2;
}


BOOL CFlowLinesDlg::PreTranslateMessage(MSG* pMsg) 
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)){
		CWnd* ppp = GetFocus();

		if(GetDlgItem(IDC_APPLY) == ppp) 
			OnApply();
		else 
		{
			ReadParams();
			m_ctrlApply.SetFocus();
		}
		return true;
	}	
	return CDialog::PreTranslateMessage(pMsg);
}

void CFlowLinesDlg::OnApply() 
{
	CWaitCursor Wait;
	ReadParams();
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	pMiarex->m_bResetglStream = true;
	pMiarex->UpdateView();
}

void CFlowLinesDlg::OnClose()
{
	if(m_pChildWnd) m_pChildWnd->SetFocus();
	CDialog::OnClose();
}

void CFlowLinesDlg::OnChanged()
{
	if(IsWindowVisible()) ReadParams();
}

void CFlowLinesDlg::PostNcDestroy()
{
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	pMiarex->UpdateView();

	CDialog::PostNcDestroy();
}

