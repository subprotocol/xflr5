/****************************************************************************

    Advanced Dlg options Class
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

// AdvDlg.h : header file
//
#include "../misc/FloatEdit.h"
#include "../misc/NumEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CXFoilAdvDlg dialog

class CXFoilAdvDlg : public CDialog
{
	friend class CXDirect;
// Construction
public:
	CXFoilAdvDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CXFoilAdvDlg)
	enum { IDD = IDD_ADVDLG };
	CNumEdit	m_ctrlIterLim;

	CButton	m_ctrlInitBL;
	CFloatEdit	m_ctrlVAccel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXFoilAdvDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_iIterLim;
	double m_fVAccel;
	bool m_bAutoInitBL;


	// Generated message map functions
	//{{AFX_MSG(CXFoilAdvDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAutoInitBLs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
