/****************************************************************************

    Foil Name Dlg Class
	Copyright (C) 2003 André Deperrois xflr5@yahoo.com

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

#include "afxwin.h"


#pragma once


// NameDlg.h : header file
//
//#include "Plane.h"
//#include "Wing.h"
//#include "Foil.h"

/////////////////////////////////////////////////////////////////////////////
// CNameDlg dialog

class CNameDlg : public CDialog
{
	friend class CAFoil;
	friend class CXDirect;
	friend class CMiarex;
	friend class CXBody;
	friend class CXInverse;
	friend class CMainFrame;

// Construction
public:
	CNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNameDlg)
	enum { IDD = IDD_NAMEDLG };
	CListBox	m_ctrlList;
	CEdit	m_ctrlQuestion;
	CEdit	m_ctrlName;
	CStatic m_ctrlNote;
	CButton m_ctrlOverwrite;
	CString	m_strName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CString m_strQuestion;
	CString m_Note;
	bool m_bEnableOverwrite;

protected:
	CStringArray *m_pStrArray;
	CString m_StartName;
	// Generated message map functions
	//{{AFX_MSG(CNameDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnOverwrite();
	virtual void OnOK();
	afx_msg void OnDblclkList();
	afx_msg void OnSelChangeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

