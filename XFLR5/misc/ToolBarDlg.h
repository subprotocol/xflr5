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

// ToolBarDlg.h : header file
//

#pragma once
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CToolBarDlg dialog

class CToolBarDlg : public CDialog
{
	friend class CMainFrame;
// Construction
public:
	CToolBarDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CToolBarDlg)
	enum { IDD = IDD_TOOLBARDLG };
	BOOL	m_BMiarexWAnaBar;
	BOOL	m_BMiarexWingBar;
	BOOL	m_BXDirectAnaBar;
	BOOL	m_BXDirectFoilBar;
	BOOL	m_BPolarBar;
	BOOL	m_BAFoilBar;
	BOOL	m_BACtrlBar;
	BOOL	m_BFInvCtrlBar;
	BOOL	m_BMInvCtrlBar;
	BOOL	m_BXInverseBar;
	BOOL	m_BMiarexW3DBar;
	BOOL	m_BScaleOppBar;
	//}}AFX_DATA

	int m_iApp;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolBarDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CToolBarDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CButton m_ctrlXDirectFoilBar;
	CButton m_ctrlXDirectAnaBar;
	CButton m_ctrlMiarexWingBar;
	CButton m_ctrlMiarexWAnaBar;
	CButton m_ctrlMiarexW3DBar;
	CButton m_ctrlScaleOppBar;
	CButton m_ctrlPolarBar;
	CButton m_ctrlAFoilBar;
	CButton m_ctrlACtrlBar;
	CButton m_ctrlXInverseBar;
	CButton m_ctrlFInvCtrlBar;
	CButton m_ctrlMInvCtrlBar;
	virtual BOOL OnInitDialog();
};

