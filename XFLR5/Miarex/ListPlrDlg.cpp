/****************************************************************************

	CListPlrDlg class
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

// ListPlrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "./ListPlrDlg.h"



/////////////////////////////////////////////////////////////////////////////
// CListPlrDlg dialog


CListPlrDlg::CListPlrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CListPlrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CListPlrDlg)
	//}}AFX_DATA_INIT
	m_pParent = pParent;
}


void CListPlrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CListPlrDlg)
	DDX_Control(pDX, IDC_PLRLIST, m_ctrlPlrList);
	DDX_Control(pDX, IDC_FOILSEL, m_ctrlFoilSel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CListPlrDlg, CDialog)
	//{{AFX_MSG_MAP(CListPlrDlg)
	ON_CBN_SELCHANGE(IDC_FOILSEL, OnSelChangeFoilSel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListPlrDlg message handlers

BOOL CListPlrDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CMainFrame* pFrame =(CMainFrame*)m_pParent;
	CFoil* pFoil;
	for (int i=0; i<pFrame->m_oaFoil.GetSize(); i++){
		pFoil = (CFoil*)pFrame->m_oaFoil.GetAt(i);
		m_ctrlFoilSel.AddString(pFoil->m_FoilName);
	}
	if(!m_ctrlFoilSel.GetCount()) m_ctrlFoilSel.EnableWindow(false);
	else{
		m_ctrlFoilSel.SetCurSel(0);
		OnSelChangeFoilSel();
		m_ctrlFoilSel.SetFocus();
	}
	return FALSE;
}


void CListPlrDlg::OnSelChangeFoilSel() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pParent;

	CString string= "";
	CString strong;
	CPolar* pPolar;
	int pos = m_ctrlFoilSel.GetCurSel();
	if(pos!=CB_ERR){
		m_ctrlFoilSel.GetLBText(pos, strong);
		for (int i=0; i<pFrame->m_oaPolar.GetSize(); i++){
			pPolar = (CPolar*)pFrame->m_oaPolar.GetAt(i);
			if(pPolar->m_FoilName == strong) {
				string+= pPolar->m_PlrName;
				string+= "\r\n";
			}
		}
	}
	//	m_strPlrList = string;
	m_ctrlPlrList.SetWindowText(string);
	Invalidate();
}
