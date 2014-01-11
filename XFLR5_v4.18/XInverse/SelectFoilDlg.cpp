/****************************************************************************

    SelectFoilDlg Class
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

// SelectFoilDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "SelectFoilDlg.h"
#include "../main/MainFrm.h"


/////////////////////////////////////////////////////////////////////////////
// CSelectFoilDlg dialog


CSelectFoilDlg::CSelectFoilDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectFoilDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectFoilDlg)
	//}}AFX_DATA_INIT
	m_FoilName ="";
}


void CSelectFoilDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectFoilDlg)
	DDX_Control(pDX, IDC_LBFOIL, m_ctrlLBFoil);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectFoilDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectFoilDlg)
	ON_CBN_SELCHANGE(IDC_LBFOIL, OnSelChangeCBFoil)
	ON_LBN_DBLCLK(IDC_LBFOIL, OnDblclkLBFoil)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectFoilDlg message handlers

BOOL CSelectFoilDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;
	
	m_ctrlLBFoil.SetFocus();
	CFoil *pFoil;

	for (int i=0; i<pFrame->m_oaFoil.GetSize(); i++){
		pFoil = (CFoil*)pFrame->m_oaFoil.GetAt(i);
		m_ctrlLBFoil.AddString(pFoil->m_FoilName);
	}

	if(!m_ctrlLBFoil.GetCount()){
		m_ctrlLBFoil.EnableWindow(false);
	}
	else{
		if(m_FoilName.GetLength()){
			int pos = m_ctrlLBFoil.FindStringExact(-1,m_FoilName);
			if(pos !=CB_ERR) m_ctrlLBFoil.SetCurSel(pos);
			else {
				m_ctrlLBFoil.SetCurSel(0);
				OnSelChangeCBFoil();
			}
		}
		else{
			m_ctrlLBFoil.SetCurSel(0);
			OnSelChangeCBFoil();
		}
	}

	return FALSE;
}

void CSelectFoilDlg::OnSelChangeCBFoil() 
{
	int pos = m_ctrlLBFoil.GetCurSel();
	if(pos !=  CB_ERR) m_ctrlLBFoil.GetText(pos, m_FoilName);
}



void CSelectFoilDlg::OnDblclkLBFoil() 
{
	int pos = m_ctrlLBFoil.GetCurSel();
	if(pos !=  CB_ERR) m_ctrlLBFoil.GetText(pos, m_FoilName);
	EndDialog(IDOK);
}
