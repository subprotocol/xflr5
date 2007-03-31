/****************************************************************************

	CFlapDlg class
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

// FlapDlg.h : header file
//
#include "../misc/FloatEdit.h"
#include "XFoil.h"
#include "Foil.h"

/////////////////////////////////////////////////////////////////////////////
// CFlapDlg dialog

class CFlapDlg : public CDialog
{
	friend class CXDirect;
	friend class CAFoil;
// Construction
public:
	void EnableTEFlap(bool bEnable);
	void EnableLEFlap(bool bEnable);
	void ReadParams();
	CFlapDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFlapDlg)
	enum { IDD = IDD_FLAPDLG };
	CFloatEdit	m_ctrlLEYHinge;
	CFloatEdit	m_ctrlLEXHinge;
	CFloatEdit	m_ctrlLEFlapAngle;
	CButton	m_ctrlLEFlapCheck;
	CFloatEdit	m_ctrlTEYHinge;
	CFloatEdit	m_ctrlTEXHinge;
	CFloatEdit	m_ctrlTEFlapAngle;
	CButton	m_ctrlTEFlapCheck;
	CButton	m_ctrlOK;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlapDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFlapDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnApply();
	afx_msg void OnLEFlapCheck();
	afx_msg void OnTEFlapCheck();
	afx_msg void OnChanged();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool m_bTEFlap;
	bool m_bLEFlap;
	bool m_bApplied;
	bool m_bModified;

	CWnd *m_pChildView;
	double m_LEXHinge, m_LEYHinge, m_LEFlapAngle;
	double m_TEXHinge, m_TEYHinge, m_TEFlapAngle;
	CFoil *m_pMemFoil, *m_pBufferFoil;

	XFoil *m_pXFoil;
};

