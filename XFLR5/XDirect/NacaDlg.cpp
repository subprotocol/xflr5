/****************************************************************************

	CNacaDlg class
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


// NacaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "NacaDlg.h"



/////////////////////////////////////////////////////////////////////////////
// CNacaDlg dialog


CNacaDlg::CNacaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNacaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNacaDlg)
	//}}AFX_DATA_INIT
	m_bApplied   = false;
	m_bGenerated = false;
}


void CNacaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNacaDlg)
	DDX_Control(pDX, IDC_PANELS, m_ctrlPanels);
	DDX_Control(pDX, IDC_MESSAGE, m_ctrlMessage);
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_DIGITS, m_ctrlDigits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNacaDlg, CDialog)
	//{{AFX_MSG_MAP(CNacaDlg)
	ON_EN_CHANGE(IDC_DIGITS, OnChange)
	ON_EN_CHANGE(IDC_PANELS, OnChange)
	ON_EN_KILLFOCUS(IDC_DIGITS, OnKillFocus)
	ON_EN_KILLFOCUS(IDC_PANELS, OnKillFocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNacaDlg message handlers

void CNacaDlg::GenerateFoil() 
{
	if(m_bApplied) return;
	int itype;
	m_bApplied   = true;
	m_pXFoil->lflap = false;
	m_pXFoil->lbflap = false;

	m_Digits = m_ctrlDigits.GetValue();
	int panels = (int)(m_ctrlPanels.GetValue()/2);
	if(m_Digits<=25099) itype = 5;
	if(m_Digits<=9999 ) itype = 4;

	if(itype==4) m_pXFoil->naca4(m_Digits, panels);
	else if(itype==5) {
		int three  = m_Digits/100;
		if(three!=210 && three !=220 && three !=230 && three !=240 && three !=250){
			m_ctrlDigits.SetSel(0,-1);
			m_ctrlMessage.SetWindowText("Illegal NACA Number");
			m_bGenerated = false;
			return;
		}
		if(!m_pXFoil->naca5(m_Digits, panels)) {
			m_bGenerated = false;
			m_ctrlMessage.SetWindowText("Illegal NACA Number");
			return;
		}
	}
	else {
		m_ctrlDigits.SetSel(0,-1);
		m_ctrlMessage.SetWindowText("Illegal NACA Number");
		m_bGenerated = false;
		return;
	}
	m_ctrlMessage.SetWindowText(" ");

	for (int j=0; j< m_pXFoil->nb; j++){
		m_pBufferFoil->xb[j] = m_pXFoil->xb[j+1];
		m_pBufferFoil->yb[j] = m_pXFoil->yb[j+1];
		m_pBufferFoil->x[j]  = m_pXFoil->xb[j+1];
		m_pBufferFoil->y[j]  = m_pXFoil->yb[j+1];
	}
	m_pBufferFoil->nb = m_pXFoil->nb;
	m_pBufferFoil->n = m_pXFoil->nb;
	m_pBufferFoil->InitFoil();

	m_pChildView->Invalidate();	
	m_bGenerated = true;
}

BOOL CNacaDlg::PreTranslateMessage(MSG* pMsg) 
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)){
		CWnd* ppp = GetFocus();

		if(GetDlgItem(IDOK) == ppp) 
			OnOK();
		else if(GetDlgItem(IDCANCEL) == ppp) 
			OnCancel();
		else if(&m_ctrlDigits == ppp || &m_ctrlPanels == ppp ) {
			GenerateFoil();
			if(m_bGenerated) m_ctrlOK.SetFocus();
			else {
				m_ctrlDigits.SetSel(0,-1);
			}
		}
		return true;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CNacaDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ctrlDigits.SetWindowText("0000");
	m_ctrlPanels.SetValue(100);
	m_ctrlDigits.SetFocus();
	return FALSE;
}

void CNacaDlg::OnChange()
{
	m_bApplied = false;	
}

void CNacaDlg::OnOK() 
{
	if(!m_bApplied){
		GenerateFoil();
		if(!m_bGenerated) return;
	}
	if(m_bApplied && !m_bGenerated){//try again
		GenerateFoil();
		if(!m_bGenerated) return;
	}
	if(!m_bGenerated) {
		EndDialog(0);//couldn't generate the foil...
		return;
	}
	CDialog::OnOK();
}

void CNacaDlg::OnKillFocus() 
{
	GenerateFoil();
}

