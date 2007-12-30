/****************************************************************************

	Full inverse controls class

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

// FInvCtrlBar.h : header file
//
#include "../misc/InitDialogBar.h"
#include "../misc/FloatEdit.h"
#include "../misc/NumEdit.h"
/////////////////////////////////////////////////////////////////////////////
// CFInvCtrlBar dialog

class CFInvCtrlBar : public CInitDialogBar
{
	friend class CXInverse;
	friend class CMainFrame;
	DECLARE_DYNAMIC(CFInvCtrlBar)

	// Construction
	public:
	CFInvCtrlBar();
	virtual ~CFInvCtrlBar();

	// Attributes
	public:
	protected:

	// Control Objects
	CStatic	m_ctrlOutput;
	CStatic	m_ctrlSpecif;
	CButton m_ctrlExec;
	CButton	m_ctrlSymm;
	CButton	m_ctrlFilter;
	CButton	m_ctrlPert;
	CButton	m_ctrlSpecal;
	CButton	m_ctrlNewSpline;
	CButton	m_ctrlSmooth;
	CButton	m_ctrlShowSpline;
	CButton	m_ctrlTangentSpline;
	CFloatEdit	m_ctrlSpec;
	CFloatEdit  m_ctrlFilterParam;
	CFloatEdit	m_ctrlTGapy;
	CFloatEdit	m_ctrlTGapx;
	CFloatEdit	m_ctrlTAngle;

	// Control Variables

	// Operations
	public:
		void CancelSmooth();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFInvCtrlBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	protected:
	virtual BOOL OnInitDialogBar();

	// Implementation
private:
	void CancelSpline();
	CWnd* m_pXInverse;

	// Generated message map functions
	protected:
	//{{AFX_MSG(CFInvCtrlBar)
	afx_msg void OnQReset();
	afx_msg void OnApplySpline();
	afx_msg void OnExec();
	afx_msg void OnPerturb();
	afx_msg void OnSmooth();
	afx_msg void OnKillFocusTAngle();
	afx_msg void OnFilter();
	afx_msg void OnNewSpline();
	afx_msg void OnShowSpline();
	afx_msg void OnTangentSpline();
	afx_msg void OnSpecal();
	afx_msg void OnKillFocusSpec();
	afx_msg void OnKillFocusTGap();
	afx_msg void OnSymm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

