/****************************************************************************

    CUnitsDlg Class
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

// UnitsDlg.h : header file
//
#include "FloatEdit.h"
/////////////////////////////////////////////////////////////////////////////
// CUnitsDlg dialog

class CUnitsDlg : public CDialog
{
	friend class CMainFrame;
	friend class CWing;
	friend class CMiarex;
	friend class CBody;

// Construction
public:
	CUnitsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUnitsDlg)
	enum { IDD = IDD_UNITSDLG };
	CComboBox	m_ctrlMoment;
	CComboBox	m_ctrlSurface;
	CComboBox	m_ctrlWeight;
	CComboBox	m_ctrlSpeed;
	CComboBox	m_ctrlLength;
	CComboBox	m_ctrlForce;
	CString	m_SForce;
	CString	m_SLength;
	CString	m_SSpeed;
	CString	m_SSurface;
	CString	m_SWeight;
	CString	m_strQuestion;
	CString	m_SMoment;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnitsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bLengthOnly;
	UINT m_Length, m_Area, m_Weight, m_Speed, m_Force, m_Moment;
	double m_mtoUnit;
	double m_kgtoUnit;
	double m_mstoUnit;
	double m_NtoUnit;
	double m_NmtoUnit;
	double m_m2toUnit;

//	double m_Density, m_Viscosity;

	// Generated message map functions
	//{{AFX_MSG(CUnitsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

