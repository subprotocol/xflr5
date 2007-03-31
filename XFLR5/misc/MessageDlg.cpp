/****************************************************************************

    CMessageDlg Class
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

// MessageDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include ".\messagedlg.h"


// Boîte de dialogue CMessageDlg

IMPLEMENT_DYNAMIC(CMessageDlg, CDialog)
CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageDlg::IDD, pParent)
{
	m_bCancel = false;
}

CMessageDlg::~CMessageDlg()
{
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MESSAGE, m_ctrlMessage);
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
}

BEGIN_MESSAGE_MAP(CMessageDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
END_MESSAGE_MAP()


// Gestionnaires de messages CMessageDlg

BOOL CMessageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlProgress.SetRange(0,100);
	m_ctrlProgress.SetPos(0);

	m_ctrlMessage.SetWindowText(m_Message);
	return TRUE;
}
void CMessageDlg::SetProgress(int p)
{
	m_ctrlProgress.SetPos(p);
}
void CMessageDlg::SetMessage(CString Message)
{
	m_Message = Message;
	m_ctrlMessage.SetWindowText(Message);
}

void CMessageDlg::OnCancel()
{
	m_bCancel = true;
	OnCancel();
}
