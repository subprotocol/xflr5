/****************************************************************************

	CAGridDlg class
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

// AGridDlg.h : header file
//
#include "../misc/FloatEdit.h"
#include "../misc/NumEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CAGridDlg dialog

class CAGridDlg : public CDialog
{
	friend class CAFoil;
	friend class CMiarex;
// Construction
public:
	CAGridDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAGridDlg)
	enum { IDD = IDD_AGRIDDLG };
	CButton	m_ctrlScale;
	CButton	m_ctrlYMinStyle;
	CButton	m_ctrlYMinCheck;
	CButton	m_ctrlYMainCheck;
	CButton	m_ctrlYMainStyle;
	CFloatEdit	m_ctrlYMinorUnit;
	CFloatEdit	m_ctrlXMinorUnit;
	CFloatEdit	m_ctrlYUnit;
	CFloatEdit	m_ctrlXUnit;
	CButton	m_ctrlXMinStyle;
	CButton	m_ctrlXMinCheck;
	CButton	m_ctrlXMainCheck;
	CButton	m_ctrlXMainStyle;
	CButton	m_ctrlNeutralLine;
	CButton	m_ctrlNeutralCheck;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAGridDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd* m_pParent;
	bool m_bNeutral;
	bool m_bScale;
	int m_NeutralStyle, m_NeutralWidth;
	COLORREF m_NeutralColor;
	bool m_bXGrid, m_bYGrid;
	int m_XStyle, m_XWidth;
	int m_YStyle, m_YWidth;
	COLORREF m_XColor, m_YColor;
	double m_XUnit, m_YUnit;

	bool m_bXMinGrid, m_bYMinGrid;
	int m_XMinStyle, m_XMinWidth;
	int m_YMinStyle, m_YMinWidth;
	COLORREF m_XMinColor, m_YMinColor;
	double m_XMinorUnit, m_YMinorUnit;

	// Generated message map functions
	//{{AFX_MSG(CAGridDlg)
	afx_msg void OnXGridStyle();
	afx_msg void OnXGridCheck();
	afx_msg void OnYGridCheck();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnYGridStyle();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnNeutralLine();
	afx_msg void OnNeutralCheck();
	afx_msg void OnXMinStyle();
	afx_msg void OnYMinStyle();
	afx_msg void OnXMinCheck();
	afx_msg void OnYMinCheck();
	virtual void OnOK();
	afx_msg void OnScale();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

