/****************************************************************************

	CModDlg class
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


// ModDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "ModDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CModDlg dialog


CModDlg::CModDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModDlg)
	m_Question = _T("");
	//}}AFX_DATA_INIT
}


void CModDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModDlg)
	DDX_Text(pDX, IDC_QUESTION, m_Question);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModDlg, CDialog)
	//{{AFX_MSG_MAP(CModDlg)
	ON_BN_CLICKED(IDC_SAVEASNEW, OnSaveAsNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModDlg message handlers

void CModDlg::OnSaveAsNew() 
{
	EndDialog(20);
}
