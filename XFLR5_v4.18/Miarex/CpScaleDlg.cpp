/****************************************************************************

	CpScaleDlg class
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

// CpScaleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "CpScaleDlg.h"
#include ".\cpscaledlg.h"


/////////////////////////////////////////////////////////////////////////////
// CCpScaleDlg dialog


CCpScaleDlg::CCpScaleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCpScaleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCpScaleDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bAutoCpScale = true;
	m_bShowCpScale = true;
	m_CpMin = -1.0;
	m_CpMax =  1.0;
}


void CCpScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCpScaleDlg)
	DDX_Control(pDX, IDC_SHOWCPSCALE, m_ctrlShowCpScale);
	DDX_Control(pDX, IDC_MIN, m_ctrlMin);
	DDX_Control(pDX, IDC_MAX, m_ctrlMax);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCpScaleDlg, CDialog)
	//{{AFX_MSG_MAP(CCpScaleDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnScaleType)
	ON_BN_CLICKED(IDC_RADIO2, OnScaleType)
	ON_BN_CLICKED(IDC_SHOWCPSCALE, OnShowCpScale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCpScaleDlg message handlers

BOOL CCpScaleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_bShowCpScale) m_ctrlShowCpScale.SetCheck(TRUE);

	if(m_bAutoCpScale) {
		m_ctrlMin.EnableWindow(false);
		m_ctrlMax.EnableWindow(false);
		CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);	
	}
	else CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);

	m_ctrlMin.SetValue(m_CpMin);
	m_ctrlMax.SetValue(m_CpMax);
	GetDlgItem(IDC_RADIO1)->SetFocus();

	return FALSE;
}




void CCpScaleDlg::OnScaleType() 
{
	if(GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO2)==IDC_RADIO1){
		m_bAutoCpScale = true;
		m_ctrlMin.EnableWindow(false);
		m_ctrlMax.EnableWindow(false);
	}
	else{
		m_bAutoCpScale = false;
		m_ctrlMin.EnableWindow(true);
		m_ctrlMax.EnableWindow(true);
	}
}


void CCpScaleDlg::OnOK() 
{
	if(m_bAutoCpScale){
		m_CpMax = -10.0;
		m_CpMin =  10.0;
	}
	else{
		m_CpMin = m_ctrlMin.GetValue();
		m_CpMax = m_ctrlMax.GetValue();
		if(m_CpMax<m_CpMin){
			double tmp = m_CpMin;
			m_CpMin = m_CpMax;
			m_CpMax = tmp;
		}
	}
	CDialog::OnOK();
}

void CCpScaleDlg::OnShowCpScale()
{
	if(m_ctrlShowCpScale.GetCheck())	m_bShowCpScale = true;
	else								m_bShowCpScale = false;
}
