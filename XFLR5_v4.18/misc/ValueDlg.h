/****************************************************************************

	CValueDlg dialog 

    Copyright (C) 2006 André Deperrois XFLR5@yahoo.com

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

// ValueDlg.h : header file
//
#include "FloatEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CValueDlg dialog

class CValueDlg : public CDialog
{
	friend class CXDirect;
	friend class CMiarex;
// Construction
public:
	CValueDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CValueDlg)
	enum { IDD = IDD_VALUEDLG };
	CFloatEdit	m_ctrlValue;
	CButton m_ctrlText1;
	CButton m_ctrlText2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CValueDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	double m_Value;
	CString m_WindowText;
	CString m_Text1;
	CString m_Text2;
	// Generated message map functions
	//{{AFX_MSG(CValueDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};