/****************************************************************************

    CToolBarDlg Class
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
// ToolBarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include ".\toolbardlg.h"

/////////////////////////////////////////////////////////////////////////////
// CToolBarDlg dialog


CToolBarDlg::CToolBarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CToolBarDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolBarDlg)
	m_BMiarexWAnaBar = FALSE;
	m_BMiarexWingBar = FALSE;
	m_BXDirectAnaBar   = FALSE;
	m_BXDirectFoilBar  = FALSE;
	m_BPolarBar      = FALSE;
	m_BAFoilBar      = FALSE;
	m_BACtrlBar      = FALSE;
	m_BFInvCtrlBar   = FALSE;
	m_BMInvCtrlBar   = FALSE;
	m_BXInverseBar   = FALSE;
	m_BMiarexW3DBar  = FALSE;
	m_BScaleOppBar   = FALSE;
	//}}AFX_DATA_INIT
	m_iApp = 0;
}


void CToolBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_MIAREXWANABAR, m_BMiarexWAnaBar);
	DDX_Check(pDX, IDC_MIAREXWINGBAR, m_BMiarexWingBar);
	DDX_Check(pDX, IDC_XDIRECTANABAR, m_BXDirectAnaBar);
	DDX_Check(pDX, IDC_XDIRECTFOILBAR, m_BXDirectFoilBar);
	DDX_Check(pDX, IDC_POLARBAR, m_BPolarBar);
	DDX_Check(pDX, IDC_AFOILBAR, m_BAFoilBar);
	DDX_Check(pDX, IDC_ACTRLBAR, m_BACtrlBar);
	DDX_Check(pDX, IDC_FINVCTRLBAR, m_BFInvCtrlBar);
	DDX_Check(pDX, IDC_MINVCTRLBAR, m_BMInvCtrlBar);
	DDX_Check(pDX, IDC_XINVERSEBAR, m_BXInverseBar);
	DDX_Check(pDX, IDC_MIAREXW3DBAR, m_BMiarexW3DBar);
	DDX_Check(pDX, IDC_SCALEOPPBAR, m_BScaleOppBar);

	DDX_Control(pDX, IDC_XDIRECTFOILBAR, m_ctrlXDirectFoilBar);
	DDX_Control(pDX, IDC_XDIRECTANABAR, m_ctrlXDirectAnaBar);
	DDX_Control(pDX, IDC_MIAREXWINGBAR, m_ctrlMiarexWingBar);
	DDX_Control(pDX, IDC_MIAREXWANABAR, m_ctrlMiarexWAnaBar);
	DDX_Control(pDX, IDC_MIAREXW3DBAR, m_ctrlMiarexW3DBar);
	DDX_Control(pDX, IDC_SCALEOPPBAR, m_ctrlScaleOppBar);
	DDX_Control(pDX, IDC_POLARBAR, m_ctrlPolarBar);
	DDX_Control(pDX, IDC_AFOILBAR, m_ctrlAFoilBar);
	DDX_Control(pDX, IDC_ACTRLBAR, m_ctrlACtrlBar);
	DDX_Control(pDX, IDC_XINVERSEBAR, m_ctrlXInverseBar);
	DDX_Control(pDX, IDC_FINVCTRLBAR, m_ctrlFInvCtrlBar);
	DDX_Control(pDX, IDC_MINVCTRLBAR, m_ctrlMInvCtrlBar);
}


BEGIN_MESSAGE_MAP(CToolBarDlg, CDialog)
	//{{AFX_MSG_MAP(CToolBarDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolBarDlg message handlers


BOOL CToolBarDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_ctrlXDirectFoilBar.EnableWindow(false);
	m_ctrlXDirectAnaBar.EnableWindow(false);
	m_ctrlMiarexWingBar.EnableWindow(false);
	m_ctrlMiarexWAnaBar.EnableWindow(false);
	m_ctrlMiarexW3DBar.EnableWindow(false);
	m_ctrlScaleOppBar.EnableWindow(false);
	m_ctrlPolarBar.EnableWindow(false);
	m_ctrlAFoilBar.EnableWindow(false);
	m_ctrlACtrlBar.EnableWindow(false);
	m_ctrlXInverseBar.EnableWindow(false);
	m_ctrlFInvCtrlBar.EnableWindow(false);
	m_ctrlMInvCtrlBar.EnableWindow(false);

	if(m_iApp==DIRECTDESIGN){
		m_ctrlAFoilBar.EnableWindow(true);
		m_ctrlACtrlBar.EnableWindow(true);
		m_ctrlAFoilBar.SetFocus();
	}
	else if(m_iApp==INVERSEDESIGN){
		m_ctrlXInverseBar.EnableWindow(true);
		m_ctrlFInvCtrlBar.EnableWindow(true);
		m_ctrlMInvCtrlBar.EnableWindow(true);
		m_ctrlXInverseBar.SetFocus();
	}
	else if(m_iApp==MIAREX){
		m_ctrlMiarexWingBar.EnableWindow(true);
		m_ctrlMiarexWAnaBar.EnableWindow(true);
		m_ctrlMiarexW3DBar.EnableWindow(true);
		m_ctrlScaleOppBar.EnableWindow(true);
		m_ctrlPolarBar.EnableWindow(true);
		m_ctrlMiarexWingBar.SetFocus();
	}
	else if(m_iApp==XFOILANALYSIS){
		m_ctrlXDirectFoilBar.EnableWindow(true);
		m_ctrlXDirectAnaBar.EnableWindow(true);
		m_ctrlPolarBar.EnableWindow(true);
		m_ctrlXDirectFoilBar.SetFocus();
	}

	return FALSE;  
}
