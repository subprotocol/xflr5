/****************************************************************************

    CUserVarDlg Class
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


#pragma once


// UserVarDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserVarDlg dialog

class CUserVarDlg : public CDialog
{
	friend class CMiarex;
	friend class CXDirect;

// Construction
public:
	CUserVarDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUserVarDlg)
	enum { IDD = IDD_USERVARDLG };
	CListBox	m_ctrlYSel;
	CListBox	m_ctrlXSel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserVarDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
private:
	int m_XSel, m_YSel;
	bool m_bWing;//else XFLR5 app

	// Generated message map functions
	//{{AFX_MSG(CUserVarDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeXSel();
	afx_msg void OnSelChangeYSel();
	afx_msg void OnDblclkYsel();
	afx_msg void OnDblclkXsel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

