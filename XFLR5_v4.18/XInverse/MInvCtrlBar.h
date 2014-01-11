/****************************************************************************

	Mixed inverse controls class

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

// MInvCtrlBar.h : header file
//
#include "../misc/InitDialogBar.h"
#include "../misc/NumEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CMInvCtrlBar dialog

class CMInvCtrlBar : public CInitDialogBar
{ 
	friend class CXInverse;
	friend class CMainFrame;
	DECLARE_DYNAMIC(CMInvCtrlBar)

	// Construction
	public:
	CMInvCtrlBar();
	virtual ~CMInvCtrlBar();

	// Attributes
	public:
	protected:

	// Control Objects
	CEdit	m_ctrlSpec;
	CEdit	m_ctrlOutput;
	CNumEdit	m_ctrlIter;
	CButton	m_ctrlMark;
	CButton	m_ctrlNewSpline;
	CButton	m_ctrlShowSpline;
	CButton	m_ctrlCpxx;
	CButton	m_ctrlTangentSpline;
	CButton m_ctrlExec;
	CButton m_ctrlSmooth;

	// Control Variables

	// Operations
	public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMInvCtrlBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	protected:
	virtual BOOL OnInitDialogBar();

	// Implementation
private:
	void CancelMark();
	void CancelSpline();
	void CancelSmooth();
	CWnd* m_pXInverse;

	// Generated message map functions
	protected:
	//{{AFX_MSG(CMInvCtrlBar)
	afx_msg void OnCpxx();
	afx_msg void OnExec();
	afx_msg void OnNewSpline();
	afx_msg void OnQReset();
	afx_msg void OnTangentSpline();
	afx_msg void OnShowSpline();
	afx_msg void OnSmooth();
	afx_msg void OnApplySpline();
	afx_msg void OnMarkSegment();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

