/****************************************************************************

	Preferences Dialog 
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

// BckgndclrDlg.h : header file
//
#include "ClrClasses.h"
#include "../Graph/Graph.h"

/////////////////////////////////////////////////////////////////////////////
// CPrefsDlg dialog

class CPrefsDlg : public CDialog
{
	friend class CChildView;
// Construction
public:
	CPrefsDlg(CWnd* pParent = NULL);   // standard constructor
 
// Dialog Data
	//{{AFX_DATA(CPrefsDlg)
	enum { IDD = IDD_PREFERENCES };
	CButton	m_ctrlTextFont;
	CComboBox m_ctrlStdSettings;
	CClrBtn	m_ctrlWndClr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrefsDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	Graph *m_pGraph;

	LOGFONT  m_WndLogFont;
	COLORREF m_crWnd;
	COLORREF m_WndTextColor;
	CWnd * m_pParent;
	bool m_bUserDefined;
	bool m_bBlackAndWhite;

//	Miarex wing settings
	int m_WingStyle, m_WingWidth;
	COLORREF m_WingPanels;
//	COLORREF m_WingSurfaces;

	int m_XCPStyle, m_XCPWidth;
	COLORREF m_XCPColor;

	int m_XTopStyle, m_XTopWidth;
	COLORREF m_XTopColor;

	int m_XBotStyle, m_XBotWidth;
	COLORREF m_XBotColor;

	int m_DownwashStyle, m_DownwashWidth;
	COLORREF m_DownwashColor;

// XFLR5 foil settings
	COLORREF m_crFoilColor, m_crBLColor, m_crPressureColor, m_crMomentColor;
	int m_iFoilStyle, m_iFoilWidth;
	int m_iBLStyle, m_iBLWidth;
	int m_iPressureStyle, m_iPressureWidth;
	int m_iMomentStyle, m_iMomentWidth;



private:
	CFont m_WndFont;
	// Generated message map functions
	//{{AFX_MSG(CPrefsDlg)
	afx_msg void OnApply();
	afx_msg void OnWndCr();
	afx_msg void OnSelChangeStdSettings();
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnWndTxtFont();
	afx_msg void OnGraphsPrefs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
