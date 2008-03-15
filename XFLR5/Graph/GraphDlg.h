/****************************************************************************

	Graph dialog options

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
// GraphDlg.h : header file
//
#include "Pages.h"

/////////////////////////////////////////////////////////////////////////////
// CGraphDlg dialog

class CGraphDlg : public CDialog
{
// Construction
public:

	CGraphDlg(Graph *pGraph,CWnd* pParent);
	CGraphDlg(CWnd* pParent = NULL);   // standard constructor
	Graph* m_pGraph;
	CWnd * m_pParent;

// Dialog Data
	//{{AFX_DATA(CGraphDlg)
	enum { IDD = IDD_GRAPHDLG };
	CButton	m_ctrlApply;
	CButton	m_ctrlOK;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGraphDlg)
	virtual void OnOK();
	afx_msg void OnApply();
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int GetWidth();
	int GetStyle();
	bool GetAutoX();
	bool GetAutoY();
	COLORREF GetColor();
	void GetGridPage();
	void GetFontPage();
	void GetBackPage();	
	void GetScalePage();

	void SetGridPage();
	void SetBackPage();
	void SetFontPage();
	void SetScalePage();

	CPropertySheet m_GraphSheet;
	CGridPage m_GridPage;
	CFontPage m_FontPage;
	CScalePage m_ScalePage;
	CBackPage m_BackPage;

	LOGFONT m_TitleLogFont;
	LOGFONT m_LabelLogFont;
	LOGFONT m_LegendLogFont;
	// axis data
	int m_nStyle;
	int m_nWidth;
	COLORREF m_crColor;

	//background data
	COLORREF m_crBack;
	COLORREF m_crBorder;
	bool m_bBorder;
	int m_BorderStyle, m_BorderWidth;

	// Major grid data
	int m_XMStyle, m_YMStyle;
	int m_XMWidth, m_YMWidth;
	COLORREF m_XMClr, m_YMClr;
	bool m_bXMGrid, m_bYMGrid;

	// Minor grid data
	int m_XMinStyle, m_YMinStyle;
	int m_XMinWidth, m_YMinWidth;
	COLORREF m_XMinClr, m_YMinClr;
	double m_XMinorUnit, m_YMinorUnit;
	bool m_bXMinGrid, m_bYMinGrid;

	//Scale data
	double m_XMin, m_XMax, m_X0, m_XUnit;
	double m_YMin, m_YMax, m_Y0, m_YUnit;
	bool m_bAutoX, m_bAutoY;
	bool m_bYInverted;
	bool m_bXAutoMin, m_bYAutoMin;
	//Font data
	COLORREF m_TitleFontColor;
	COLORREF m_LabelFontColor;
	COLORREF m_LegendFontColor;
};

