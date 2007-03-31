/****************************************************************************

    Foil Name Dlg Class
    Copyright (C) 2003 André Deperrois XFLR5@yahoo.com

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
#include "NameDlg.h"
//#include "Polar.h"
//#include "WPolar.h"



/////////////////////////////////////////////////////////////////////////////
// CNameDlg dialog


CNameDlg::CNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNameDlg)
	m_strName = _T("");
	//}}AFX_DATA_INIT
	m_strQuestion = "Enter the new name :";
	m_Note = "Note : Overwrite will delete all associated OpPoints and will reset polars";
	m_pStrArray = NULL;
	m_bEnableOverwrite = true;
//	m_pWing = NULL;
//	m_pFoil = NULL;
}


void CNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNameDlg)
	DDX_Control(pDX, IDC_LIST, m_ctrlList);
	DDX_Control(pDX, IDC_QUESTION, m_ctrlQuestion);
	DDX_Control(pDX, IDC_NEWNAME, m_ctrlName);
	DDX_Text(pDX, IDC_NEWNAME, m_strName);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_NOTE, m_ctrlNote);
	DDX_Control(pDX, IDC_OVERWRITE, m_ctrlOverwrite);
}


BEGIN_MESSAGE_MAP(CNameDlg, CDialog)
	//{{AFX_MSG_MAP(CNameDlg)
	ON_BN_CLICKED(IDC_OVERWRITE, OnOverwrite)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelChangeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNameDlg message handlers

BOOL CNameDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctrlNote.SetWindowText(m_Note);

	if(!m_bEnableOverwrite) m_ctrlOverwrite.EnableWindow(false);
	m_StartName = m_strName;
	m_ctrlQuestion.SetWindowText(m_strQuestion);
	m_ctrlName.SetFocus();
	m_ctrlName.SetSel(0,-1);
	
	for (int i=0; i<m_pStrArray->GetSize(); i++){
		m_ctrlList.AddString(m_pStrArray->GetAt(i));
	}

	return false;
}

void CNameDlg::OnOverwrite() 
{
	m_ctrlName.GetWindowText(m_strName);
	EndDialog(10);
}


void CNameDlg::OnOK() 
{
	m_ctrlName.GetWindowText(m_strName);
	if (!m_strName.GetLength()){
		AfxMessageBox("Must enter a name");
		m_ctrlName.SetFocus();
		return;
	}

	CString strong;
	
	//exists ?
	for (int l=0; l<m_pStrArray->GetSize(); l++){
		strong = m_pStrArray->GetAt(l);
		if(strong == m_strName) {
			CString str = "Do you wish to overwrite "+m_strName + " ?";
			if(IDYES==AfxMessageBox(str, MB_YESNOCANCEL)){
				EndDialog(10);
				return;
			}
			else return;
		}
	}


	/*	if(m_strName!=m_StartName){
		//exists ?
		for (int l=0; l<m_pStrArray->GetSize(); l++){
			strong = m_pStrArray->GetAt(l);
			if(strong == m_strName) {
				CString str = "Do you wish to overwrite "+m_strName + " ?";
				if(IDYES==AfxMessageBox(str, MB_YESNOCANCEL)){
					EndDialog(10);
					return;
				}
				else return;
			}
		}
	}
	else{
		//just cancel
//		EndDialog(IDCANCEL);
		return;
	}*/

	CDialog::OnOK();
}


void CNameDlg::OnSelChangeList() 
{
	int pos = m_ctrlList.GetCurSel();
	if(pos == LB_ERR) return;
	CString str;
	m_ctrlList.GetText(pos, str);
	m_ctrlName.SetWindowText(str);
	m_ctrlName.SetSel(0,-1);
	m_ctrlName.SetFocus();
}


void CNameDlg::OnDblclkList() 
{
	int pos = m_ctrlList.GetCurSel();
	if(pos == LB_ERR) return;
	m_ctrlList.GetText(pos, m_strName);
	CString str = "Do you wish to overwrite "+m_strName + " ?";
	if(IDYES==AfxMessageBox(str, MB_YESNOCANCEL))
		EndDialog(10);
}


