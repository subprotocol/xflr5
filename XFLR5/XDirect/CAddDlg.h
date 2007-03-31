/****************************************************************************

	Corner Add class
	Copyright (C) 2004 André Deperrois xflr5@yahoo.com

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

// CAddDlg.h : header file
//
#include "../misc/FloatEdit.h"
#include "XFoil.h"

/////////////////////////////////////////////////////////////////////////////
// CCAddDlg dialog

class CCAddDlg : public CDialog
{
	friend class CXDirect;
	friend class CAFoil;
// Construction
public:
	CCAddDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCAddDlg)
	enum { IDD = IDD_CADDDLG };
	CButton	m_ctrlApply;
	CStatic	m_ctrlAtPanel;
	CStatic	m_ctrlTotal;
	CStatic	m_ctrlAdded;
	CStatic	m_ctrlMaxAngle;
	CButton	m_ctrlUniform;
	CFloatEdit	m_ctrlTo;
	CFloatEdit	m_ctrlFrom;
	CFloatEdit	m_ctrlAngTol;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCAddDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
private:
	CFoil* m_pMemFoil;
	CFoil* m_pBufferFoil;
	CWnd* m_pChildView;
	XFoil* m_pXFoil;
	CString Title;
	double atol;

protected:
	int m_iSplineType;

	// Generated message map functions
	//{{AFX_MSG(CCAddDlg)
	afx_msg void OnApply();
	afx_msg void OnUniform();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

