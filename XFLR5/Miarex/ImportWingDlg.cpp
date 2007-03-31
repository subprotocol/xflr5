/****************************************************************************

    CImportWingDlg Class
	Copyright (C) 2005 André Deperrois xflr5@yahoo.com

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
// ImportWingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "ImportWingDlg.h"
#include "Wing.h"

/////////////////////////////////////////////////////////////////////////////
// CImportWingDlg dialog


CImportWingDlg::CImportWingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImportWingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportWingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CImportWingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportWingDlg)
	DDX_Control(pDX, IDC_WINGSEL, m_ctrlWingSel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportWingDlg, CDialog)
	//{{AFX_MSG_MAP(CImportWingDlg)
	ON_CBN_SELCHANGE(IDC_WINGSEL, OnSelChangeWingSel)
	ON_LBN_DBLCLK(IDC_WINGSEL, OnDblclkWingSel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportWingDlg message handlers

BOOL CImportWingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CWing *pWing;
	if(m_poaWing->GetSize()){
		int sel = LB_ERR;
		for (int i=0; i<m_poaWing->GetSize(); i++){
			pWing = (CWing*)m_poaWing->GetAt(i);
			m_ctrlWingSel.AddString(pWing->m_WingName);
			if(pWing->m_WingName == m_WingName) sel = i;
		}
		
		if (sel==LB_ERR) m_ctrlWingSel.SetCurSel(0);
		else m_ctrlWingSel.SetCurSel(sel);
	}
	else m_ctrlWingSel.EnableWindow(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImportWingDlg::OnSelChangeWingSel() 
{
	int sel = m_ctrlWingSel.GetCurSel();
	if(sel !=LB_ERR)m_ctrlWingSel.GetText(sel,m_WingName);
	else m_WingName = "";
}


void CImportWingDlg::OnDblclkWingSel() 
{
	OnSelChangeWingSel();
	OnOK();
}
