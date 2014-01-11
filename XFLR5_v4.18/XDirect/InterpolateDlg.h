/****************************************************************************

	Interpolation Dialog class
    Copyright (C) 2004 André Deperrois XFLR5@yahoo.com

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

// InterDlg.h : header file
//
#include "../misc/FloatEdit.h"
#include "Foil.h"
#include "XFoil.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// CInterpolateDlg dialog

class CInterpolateDlg : public CDialog
{
	friend class CXDirect;
	friend class CAFoil;

// Construction
public:
	CInterpolateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInterpolateDlg)
	enum { IDD = IDD_INTERDLG };
	CButton	m_ctrlOK;
	CStatic	m_ctrlThickness2;
	CStatic	m_ctrlThickness1;
	CStatic	m_ctrlCamber2;
	CStatic	m_ctrlCamber1;
	CStatic	m_ctrlThickness0;
	CStatic	m_ctrlCamber0;
	CFloatEdit	m_ctrlFrac;
	CEdit m_ctrlNewFoilName;
	CComboBox	m_ctrlFoil2;
	CComboBox	m_ctrlFoil1;
	CSliderCtrl	m_ctrlMixt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInterpolateDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	void Update();
	CFoil* m_pBufferFoil;
	CObArray *m_poaFoil;
	XFoil * m_pXFoil;

	CWnd* m_pChildView;
	double m_Frac;
	CString m_NewFoilName;

protected:

	// Generated message map functions
	//{{AFX_MSG(CInterpolateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeFoil1();
	afx_msg void OnSelChangeFoil2();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnOK();
	afx_msg void OnKillFocusFrac();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CFoil* GetFoil(CString strong);
public:
};
