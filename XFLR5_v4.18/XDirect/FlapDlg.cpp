/****************************************************************************

	CFlapDlg class
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

// FlapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "FlapDlg.h"
#include "../main/ChildView.h"


/////////////////////////////////////////////////////////////////////////////
// CFlapDlg dialog


CFlapDlg::CFlapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFlapDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFlapDlg)
	//}}AFX_DATA_INIT
	m_pMemFoil    = NULL;
	m_pBufferFoil = NULL;
	m_bTEFlap     = false;
	m_TEFlapAngle = 0.0;
	m_TEXHinge    = 80.0;//%
	m_TEYHinge    = 50.0;//%
	m_bLEFlap     = false;
	m_LEFlapAngle = 0.0;
	m_LEXHinge    = 20.0;//%
	m_LEYHinge    = 50.0;//%
	m_pXFoil    = NULL;

	m_bModified   = false;
	m_bApplied    = true;	
}


void CFlapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFlapDlg)
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_LEYHINGE, m_ctrlLEYHinge);
	DDX_Control(pDX, IDC_LEXHINGE, m_ctrlLEXHinge);
	DDX_Control(pDX, IDC_LEFLAPANGLE, m_ctrlLEFlapAngle);
	DDX_Control(pDX, IDC_LEFLAPCHECK, m_ctrlLEFlapCheck);
	DDX_Control(pDX, IDC_TEYHINGE, m_ctrlTEYHinge);
	DDX_Control(pDX, IDC_TEXHINGE, m_ctrlTEXHinge);
	DDX_Control(pDX, IDC_TEFLAPANGLE, m_ctrlTEFlapAngle);
	DDX_Control(pDX, IDC_TEFLAPCHECK, m_ctrlTEFlapCheck);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFlapDlg, CDialog)
	//{{AFX_MSG_MAP(CFlapDlg)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_LEFLAPCHECK, OnLEFlapCheck)
	ON_EN_CHANGE(IDC_LEFLAPANGLE, OnChanged)
	ON_EN_CHANGE(IDC_LEXHINGE, OnChanged)
	ON_EN_CHANGE(IDC_LEYHINGE, OnChanged)
	ON_BN_CLICKED(IDC_TEFLAPCHECK, OnTEFlapCheck)
	ON_EN_CHANGE(IDC_TEFLAPANGLE, OnChanged)
	ON_EN_CHANGE(IDC_TEXHINGE, OnChanged)
	ON_EN_CHANGE(IDC_TEYHINGE, OnChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlapDlg message handlers

BOOL CFlapDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_pMemFoil->m_bTEFlap) m_ctrlTEFlapCheck.SetCheck(TRUE);
	EnableTEFlap(m_pMemFoil->m_bTEFlap);
	m_ctrlTEFlapAngle.SetValue(m_pMemFoil->m_TEFlapAngle);
	m_ctrlTEXHinge.SetValue(m_pMemFoil->m_TEXHinge);
	m_ctrlTEYHinge.SetValue(m_pMemFoil->m_TEYHinge);

	if(m_pMemFoil->m_bLEFlap) m_ctrlLEFlapCheck.SetCheck(TRUE);
	EnableLEFlap(m_pMemFoil->m_bLEFlap);
	m_ctrlLEFlapAngle.SetValue(m_pMemFoil->m_LEFlapAngle);
	m_ctrlLEXHinge.SetValue(m_pMemFoil->m_LEXHinge);
	m_ctrlLEYHinge.SetValue(m_pMemFoil->m_LEYHinge);

	m_ctrlLEFlapCheck.SetFocus();
	return FALSE;
}


void CFlapDlg::ReadParams()
{
	if(m_ctrlLEFlapCheck.GetCheck()) m_bLEFlap = true;
	else							 m_bLEFlap = false;
	m_LEFlapAngle = m_ctrlLEFlapAngle.GetValue();
	m_LEXHinge    = m_ctrlLEXHinge.GetValue();
	m_LEYHinge    = m_ctrlLEYHinge.GetValue();

	if(m_ctrlTEFlapCheck.GetCheck()) m_bTEFlap = true;
	else							 m_bTEFlap = false;
	m_TEFlapAngle = m_ctrlTEFlapAngle.GetValue();
	m_TEXHinge    = m_ctrlTEXHinge.GetValue();
	m_TEYHinge    = m_ctrlTEYHinge.GetValue();
	if(m_LEXHinge>=m_TEXHinge && m_bLEFlap && m_bTEFlap){
		AfxMessageBox("The trailing edge hinge must be downstream of the leading edge hinge", MB_OK);
		m_ctrlLEXHinge.SetFocus();
		m_ctrlLEXHinge.SetSel(0,-1);
	}
}


void CFlapDlg::OnApply()
{
	CChildView* pView = (CChildView*)m_pChildView;

	if(m_bApplied) return;
	//reset everything and retry

	ReadParams();

	m_pBufferFoil->SetTEFlapData(m_bTEFlap, m_TEXHinge, m_TEYHinge, m_TEFlapAngle);
	m_pBufferFoil->SetLEFlapData(m_bLEFlap, m_LEXHinge, m_LEYHinge, m_LEFlapAngle);
	m_pBufferFoil->SetFlap();
	
	m_bApplied = true;
	m_bModified = true;
	pView->Invalidate();
}

BOOL CFlapDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN)
		{
			CWnd* pWnd = GetFocus();
			if(pWnd==GetDlgItem(IDOK))
				OnOK();
			else if(pWnd==GetDlgItem(IDCANCEL))
				OnCancel();
			else
			{
				OnApply();
				m_ctrlOK.SetFocus();
				return TRUE ;
			}
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CFlapDlg::EnableLEFlap(bool bEnable)
{
	m_ctrlLEFlapAngle.EnableWindow(bEnable);
	m_ctrlLEXHinge.EnableWindow(bEnable);
	m_ctrlLEYHinge.EnableWindow(bEnable);
}

void CFlapDlg::EnableTEFlap(bool bEnable)
{
	m_ctrlTEFlapAngle.EnableWindow(bEnable);
	m_ctrlTEXHinge.EnableWindow(bEnable);
	m_ctrlTEYHinge.EnableWindow(bEnable);
}

void CFlapDlg::OnTEFlapCheck() 
{
	if(m_ctrlTEFlapCheck.GetCheck()) 
	{
		EnableTEFlap(true);
		m_ctrlTEFlapAngle.SetFocus();
	}
	else							
		EnableTEFlap(false);
	m_bApplied = false;
	OnApply();
}

void CFlapDlg::OnLEFlapCheck() 
{
	if(m_ctrlLEFlapCheck.GetCheck()) 
	{
		EnableLEFlap(true);
		m_ctrlLEFlapAngle.SetFocus();
	}
	else					
		EnableLEFlap(false);
	m_bApplied = false;
	OnApply();
}

void CFlapDlg::OnChanged() 
{
	m_bApplied = false;
}

void CFlapDlg::OnOK() 
{
	if(!m_bApplied) OnApply();
	CDialog::OnOK();
}
