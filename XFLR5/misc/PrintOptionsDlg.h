/****************************************************************************

    CPrintOptions Class
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

// PrintOptionsDlg.h : header file
//
#include "FloatEdit.h"
/////////////////////////////////////////////////////////////////////////////
// CPrintOptionsDlg dialog

class CPrintOptionsDlg : public CDialog
{
	friend class CMiarex;
	friend class CXDirect;
	friend class CAFoil;
	friend class CXInverse;
// Construction
public:
	CPrintOptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPrintOptionsDlg)
	enum { IDD = IDD_PRINTOPTIONSDLG };
	CFloatEdit	m_ctrlPenWidth;
	CButton	m_ctrlRadio1;
	CFloatEdit	m_ctrlTop;
	CFloatEdit	m_ctrlRight;
	CFloatEdit	m_ctrlLeft;
	CFloatEdit	m_ctrlBottom;
	CButton	m_ctrlWindowBk;
	CButton	m_ctrlGraphBk;
	CString	m_PrinterName;
	CString	m_PaperSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bColor;
	bool m_bWindowBk;
	bool m_bGraphBk;

	double m_Width, m_Height;
	double m_PenWidth;

	double m_Left, m_Right, m_Top, m_Bottom;

	// Generated message map functions
	//{{AFX_MSG(CPrintOptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio1();
	afx_msg void OnWindowBk();
	afx_msg void OnGraphBk();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

