/****************************************************************************

	Pertubation Dialog

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

#include "stdafx.h"
#include "../X-FLR5.h"
#include "PertDlg.h"



/////////////////////////////////////////////////////////////////////////////
// CPertDlg dialog


CPertDlg::CPertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPertDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPertDlg)
	//}}AFX_DATA_INIT
	memset(m_cnr,   0, sizeof(m_cnr));
	memset(m_cni,   0, sizeof(m_cni));
	memset(m_backr, 0, sizeof(m_backr));
	memset(m_backi, 0, sizeof(m_backi));
}


void CPertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPertDlg)
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_CNR, m_ctrlCnr);
	DDX_Control(pDX, IDC_CNI, m_ctrlCni);
	DDX_Control(pDX, IDC_CNLIST, m_ctrlCnList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPertDlg, CDialog)
	//{{AFX_MSG_MAP(CPertDlg)
	ON_LBN_SELCHANGE(IDC_CNLIST, OnSelChangeCnList)
	ON_EN_KILLFOCUS(IDC_CNI, OnUpdate)
	ON_EN_KILLFOCUS(IDC_CNR, OnUpdate)
	ON_BN_CLICKED(IDC_RESTORE, OnRestore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPertDlg message handlers

BOOL CPertDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_RFont.CreatePointFont(85, "Courier New");
	m_ctrlCnList.SetFont(&m_RFont);
	m_ctrlCnr.SetFont(&m_RFont);
	m_ctrlCni.SetFont(&m_RFont);
	memcpy(m_backr, m_cnr, sizeof(m_cnr));
	memcpy(m_backi, m_cni, sizeof(m_cni));

	SetList();
	m_ctrlCnr.SetPrecision(5);
	m_ctrlCni.SetPrecision(5);
	m_ctrlCnList.SetCurSel(0);
	m_ctrlCnr.SetValue(m_cnr[0]);
	m_ctrlCni.SetValue(m_cni[0]);
	m_ctrlCnList.SetFocus();
	return FALSE;
}


void CPertDlg::SetList()
{
	int pos = m_ctrlCnList.GetCurSel();
	if(pos==LB_ERR) pos = 0;
	CString strong;
	m_ctrlCnList.ResetContent();
	for (int i=0; i<m_nc; i++){
		strong.Format("%d   %8.5f     %8.5f ", i, m_cnr[i], m_cni[i]);
		m_ctrlCnList.AddString(strong);
	}
	m_ctrlCnList.SetCurSel(pos);
}

void CPertDlg::OnSelChangeCnList() 
{
	int pos = m_ctrlCnList.GetCurSel();
	if(pos==LB_ERR) {
		return;
	}
	m_ctrlCnr.SetValue(m_cnr[pos]);
	m_ctrlCni.SetValue(m_cni[pos]);	
}

void CPertDlg::OnUpdate() 
{
	int pos = m_ctrlCnList.GetCurSel();
	if(pos==LB_ERR) {
		return;
	}
	m_cnr[pos] = m_ctrlCnr.GetValue();
	m_cni[pos] = m_ctrlCni.GetValue();
	CString strong;
	strong.Format("%d   %8.5f     %8.5f ", pos, m_cnr[pos], m_cni[pos]);
	m_ctrlCnList.DeleteString(pos);
	m_ctrlCnList.InsertString(pos, strong);
}

void CPertDlg::OnRestore() 
{
	for (int i=0; i<m_nc;i++){
		m_cnr[i] = m_backr[i];
		m_cni[i] = m_backi[i];
		m_backi[i] = m_cni[i];
	}

	SetList();
	m_ctrlCnList.SetCurSel(0);
	m_ctrlCnr.SetValue(m_cnr[0]);
	m_ctrlCni.SetValue(m_cni[0]);
	m_ctrlCnList.SetFocus();
}

BOOL CPertDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN){
		if(pMsg->wParam == VK_RETURN){
			CWnd* ppp = GetFocus();

			if(GetDlgItem(IDOK) == ppp) 
				OnOK();
			else if(GetDlgItem(IDCANCEL) == ppp) 
				OnCancel();
			else {
				OnUpdate();
				m_ctrlOK.SetFocus();
			}
			return true;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
