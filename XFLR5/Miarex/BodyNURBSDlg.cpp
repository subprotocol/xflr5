/****************************************************************************

	CBodyNURBSDlg class
	Copyright (C) 2008 André Deperrois xflr5@yahoo.com

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
// BodyNURBSDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "BodyNURBSDlg.h"
#include ".\bodynurbsdlg.h"


// Boîte de dialogue CBodyNURBSDlg

IMPLEMENT_DYNAMIC(CBodyNURBSDlg, CDialog)
CBodyNURBSDlg::CBodyNURBSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBodyNURBSDlg::IDD, pParent)
{
	m_pMainFrame = NULL;
	m_NHoopPoints = 20;
	m_NXPoints    = 30;
}

CBodyNURBSDlg::~CBodyNURBSDlg()
{
}

void CBodyNURBSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_XINTERVAL, m_ctrlXPoints);
	DDX_Control(pDX, IDC_HOOPRESOLUTION, m_ctrlHoopPoints);
}


BEGIN_MESSAGE_MAP(CBodyNURBSDlg, CDialog)
END_MESSAGE_MAP()


// Gestionnaires de messages CBodyNURBSDlg

BOOL CBodyNURBSDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str;
	CMainFrame *pMainFrame = (CMainFrame*)m_pMainFrame;

	m_ctrlXPoints.SetValue(m_NXPoints);
	m_ctrlHoopPoints.SetValue(m_NHoopPoints);

	m_ctrlXPoints.SetFocus();
	return FALSE;  
}

void CBodyNURBSDlg::OnOK()
{
	m_NXPoints = m_ctrlXPoints.GetValue();
	m_NHoopPoints = m_ctrlHoopPoints.GetValue();
	CDialog::OnOK();
}

