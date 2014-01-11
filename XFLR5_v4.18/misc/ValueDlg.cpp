/****************************************************************************

	ValueDlg dialog 

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


// ValueDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "ValueDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CValueDlg dialog


CValueDlg::CValueDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CValueDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CValueDlg)
	//}}AFX_DATA_INIT
}


void CValueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CValueDlg)
	DDX_Control(pDX, IDC_VALUE, m_ctrlValue);
	DDX_Control(pDX, IDC_TEXT1, m_ctrlText1);
	DDX_Control(pDX, IDC_TEXT2, m_ctrlText2);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CValueDlg, CDialog)
	//{{AFX_MSG_MAP(CValueDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CValueDlg message handlers

void CValueDlg::OnOK() 
{
	m_Value = m_ctrlValue.GetValue();
	CDialog::OnOK();
}

BOOL CValueDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(m_WindowText);
	m_ctrlText1.SetWindowText(m_Text1);
	m_ctrlText2.SetWindowText(m_Text2);

	m_ctrlValue.SetPrecision(4);
	m_ctrlValue.SetValue(m_Value);
	m_ctrlValue.SetSel(0,-1);
	m_ctrlValue.SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
