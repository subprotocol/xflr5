/****************************************************************************

    CWAdvDlg Class
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
// WAdvDlg.h : header file
//
#include "../misc/NumEdit.h"
#include "../misc/FloatEdit.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CWAdvDlg dialog

class CWAdvDlg : public CDialog
{
	friend class CMiarex;
// Construction
public:
	CWAdvDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWAdvDlg)
	enum { IDD = IDD_WADVDLG };
	CButton	m_ctrlLogFile;
	CButton m_ctrlOK;
	CButton m_ctrlResetWake;
	CButton	m_ctrlKeepOutOpps;
	CNumEdit	m_ctrlInterNodes;
	CFloatEdit	m_ctrlRelax;
	CFloatEdit	m_ctrlAlphaPrec;
	CFloatEdit m_ctrlMinPanelSize;
	CNumEdit	m_ctrlNStation;
	CNumEdit	m_ctrlIterMax;
	CNumEdit    m_ctrlMaxWakeIter;
	CFloatEdit m_ctrlCoreSize;
	CFloatEdit m_ctrlVortexPos;
	CFloatEdit m_ctrlControlPos;
	CStatic	m_ctrlAStat;
	CStatic m_ctrlLength;
	BOOL	m_BLogFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWAdvDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	bool m_bDirichlet;
	bool m_bTrefftz;
	bool m_bKeepOutOpps;
	bool m_bResetWake;

	int m_Iter;
	int m_NStation;
	int m_WakeInterNodes;
	int m_MaxWakeIter;

	double m_ControlPos, m_VortexPos;
	double m_Relax, m_AlphaPrec;
	double m_CoreSize;
	double m_MinPanelSize;
	CFont m_SymbolFont;

	void ReadParams();
	void SetParams();

	CWnd * m_pMiarex;
	CWnd* m_pFrame;
	// Generated message map functions
	//{{AFX_MSG(CWAdvDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnKeepOutOpps();
	afx_msg void OnResetWake();
	afx_msg void OnRadio1();
	afx_msg void OnRadio3();
	afx_msg void OnResetDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

