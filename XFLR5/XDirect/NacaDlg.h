/****************************************************************************

	CNacaDlg class
	Copyright (C) 2007 André Deperrois xflr5@yahoo.com

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

// NacaDlg.h : header file
//
#include "../misc/NumEdit.h"
#include "XFoil.h"

/////////////////////////////////////////////////////////////////////////////
// CNacaDlg dialog

class CNacaDlg : public CDialog
{
	friend class CXDirect;
	friend class CAFoil;
// Construction
public:
	CNacaDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNacaDlg)
	enum { IDD = IDD_NACADLG };
	CNumEdit	m_ctrlPanels;
	CStatic	m_ctrlMessage;
	CButton	m_ctrlOK;
	CNumEdit	m_ctrlDigits;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNacaDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bApplied;
	bool m_bGenerated;
	int m_Digits;
	XFoil *m_pXFoil;
	CFoil* m_pBufferFoil;
	void GenerateFoil();

	CWnd *m_pChildView;
	// Generated message map functions
	//{{AFX_MSG(CNacaDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKillFocus();
	afx_msg void OnChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

