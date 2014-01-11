/****************************************************************************

	Advanced Dialaog class
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
#include "XFoilAdvDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CXFoilAdvDlg dialog


CXFoilAdvDlg::CXFoilAdvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXFoilAdvDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXFoilAdvDlg)
	//}}AFX_DATA_INIT
	m_bAutoInitBL = true;
	m_iIterLim    = 100;
	m_fVAccel     = 0.01;
}


void CXFoilAdvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXFoilAdvDlg)
	DDX_Control(pDX, IDC_ITERLIM, m_ctrlIterLim);
	DDX_Control(pDX, IDC_INITBL, m_ctrlInitBL);
	DDX_Control(pDX, IDC_VACCEL, m_ctrlVAccel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXFoilAdvDlg, CDialog)
	//{{AFX_MSG_MAP(CXFoilAdvDlg)
	ON_BN_CLICKED(IDC_INITBL, OnAutoInitBLs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXFoilAdvDlg message handlers

BOOL CXFoilAdvDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ctrlVAccel.SetValue(m_fVAccel);
	m_ctrlIterLim.SetValue(m_iIterLim);

	if(m_bAutoInitBL) m_ctrlInitBL.SetCheck(1);

	m_ctrlVAccel.SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CXFoilAdvDlg::OnOK() {
	// TODO: Add extra validation here
	m_fVAccel  = m_ctrlVAccel.GetValue();
	m_iIterLim = m_ctrlIterLim.GetValue();
	

	CDialog::OnOK();
}



void CXFoilAdvDlg::OnAutoInitBLs() 
{
	if(m_ctrlInitBL.GetCheck()) m_bAutoInitBL = true;
	else						m_bAutoInitBL = false;
}

