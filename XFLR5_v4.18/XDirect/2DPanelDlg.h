/****************************************************************************

	Panel dialog class

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


// 2DPanelDlg.h : header file
//
#include "../misc/FloatEdit.h"
#include "../misc/NumEdit.h"
#include "XFoil.h"


/////////////////////////////////////////////////////////////////////////////
// C2DPanelDlg dialog

class C2DPanelDlg : public CDialog
{
	friend class CXDirect;
	friend class CAFoil;
// Construction
public:
	C2DPanelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(C2DPanelDlg)
	enum { IDD = IDD_2DPANELDLG };
	CButton	m_ctrlOK;
	CButton	m_ctrlApply;
	CNumEdit m_ctrlNPanels;
	CFloatEdit	m_ctrlCVpar;
	CFloatEdit	m_ctrlCTRrat;
	CFloatEdit	m_ctrlCTErat;
	CFloatEdit	m_ctrlXsRef1;
	CFloatEdit	m_ctrlXsRef2;
	CFloatEdit	m_ctrlXpRef1;
	CFloatEdit	m_ctrlXpRef2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C2DPanelDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
private:
	void ReadParams();
	CWnd* m_pChildView;
	XFoil* m_pXFoil;
	CString Title;

	bool m_bApplied;
	bool m_bModified;

	int npan;
	double cvpar;
	double cterat;
	double ctrrat;
	double xsref1;
	double xsref2;
	double xpref1;
	double xpref2;
	CFoil* m_pBufferFoil;
	CFoil* m_pMemFoil;


protected:

	// Generated message map functions
	//{{AFX_MSG(C2DPanelDlg)
	afx_msg void OnApply();
	afx_msg void OnChanged();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
