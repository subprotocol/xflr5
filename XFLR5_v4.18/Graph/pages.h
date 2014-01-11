/****************************************************************************

	Background Page option 
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

#include "../misc/ClrClasses.h"
#include "../misc/FloatEdit.h"
#include "../misc/NumEdit.h"
#include "Graph.h"

// BackPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBackPage dialog

class CBackPage : public CPropertyPage{
	friend class CGraphDlg;

	DECLARE_DYNCREATE(CBackPage)

// Construction
public:
	CBackPage();
	CBackPage(CWnd* pParent);
	~CBackPage();

// Dialog Data
	//{{AFX_DATA(CBackPage)
	enum { IDD = IDD_BACKDLG };
	CButton	m_ctrlBorderStyle;
	CClrBtn	m_ctrlGraphClr;
	CButton	m_ctrlGraphBorder;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBackPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBackPage)
	afx_msg void OnGraphBorder();
	afx_msg void OnGraphClr();
	virtual BOOL OnInitDialog();
	afx_msg void OnBorderStyle();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool m_bBorder;
	COLORREF m_GraphClr;
	COLORREF m_BorderClr;
	int m_BorderStyle, m_BorderWidth;
	CWnd *m_pParent;

};

/////////////////////////////////////////////////////////////////////////////
// CFontPage dialog

class CFontPage : public CPropertyPage
{
	friend class CGraphDlg;
	DECLARE_DYNCREATE(CFontPage)

// Construction
public:
	CFontPage();
	CFontPage(CWnd* pParent);
	~CFontPage();

// Dialog Data
	//{{AFX_DATA(CFontPage)
	enum { IDD = IDD_FONTDLG };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFontPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFontPage)
	afx_msg void OnLabelFont();
	afx_msg void OnTitleFont();
	afx_msg void OnPaint();
	afx_msg void OnLegendFont();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	LOGFONT m_TitleLogFont;
	LOGFONT m_LabelLogFont;
	LOGFONT m_LegendLogFont;
	COLORREF m_TitleFontColor;
	COLORREF m_LabelFontColor;
	COLORREF m_LegendFontColor;
	CWnd *m_pParent;

};


/////////////////////////////////////////////////////////////////////////////
// CGridPage dialog

class CGridPage : public CPropertyPage{
	friend class CGraphDlg;
	DECLARE_DYNCREATE(CGridPage)

// Construction
public:
	void SetXMajGrid(bool bstate, COLORREF clr, int style, int width);
	void SetXMinGrid(bool bstate, bool bAuto, COLORREF clr, int style, int width, double MinorUnit);
	void SetYMajGrid(bool bstate, COLORREF clr, int style, int width);
	void SetYMinGrid(bool bstate, bool bAuto, COLORREF clr, int style, int width, double MinorUnit);
	CGridPage();
	CGridPage(CWnd* pParent);
	~CGridPage();

// Dialog Data
	//{{AFX_DATA(CGridPage)
	enum { IDD = IDD_GRIDDLG };
	CButton	m_ctrlAxis;
	CButton	m_ctrlYMinStyle;
	CButton	m_ctrlYMinCheck;
	CButton	m_ctrlYMajStyle;
	CButton	m_ctrlYMajCheck;
	CButton	m_ctrlXMinStyle;
	CButton	m_ctrlXMinCheck;
	CButton	m_ctrlXMajStyle;
	CButton	m_ctrlXMajCheck;
	CFloatEdit	m_ctrlXMinorUnit;
	CFloatEdit	m_ctrlYMinorUnit;
	BOOL	m_bXM;
	BOOL	m_bXMin;
	BOOL	m_bYM;
	BOOL	m_bYMin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGridPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGridPage)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnXMajStyle();
	afx_msg void OnXMinStyle();
	afx_msg void OnYMajStyle();
	afx_msg void OnYMinStyle();
	virtual BOOL OnInitDialog();
	afx_msg void OnAxisStyle();
	afx_msg void OnXMajCheck();
	afx_msg void OnXMinCheck();
	afx_msg void OnYMajCheck();
	afx_msg void OnYMinCheck();
	afx_msg void OnChangeXMinor();
	afx_msg void OnChangeYMinor();
	afx_msg void OnKillFocusXMinorUnit();
	afx_msg void OnKillFocusYMinorUnit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void SetAxisStyle(int cr, int s, int w);
	// axis data
	int m_nStyle;
	int m_nWidth;
	COLORREF m_crColor;

	// Major grid data
	int m_XMStyle, m_YMStyle;
	int m_XMWidth, m_YMWidth;
	COLORREF m_XMClr, m_YMClr;


	// Minor grid data
	int m_XMinStyle, m_YMinStyle;
	int m_XMinWidth, m_YMinWidth;
	COLORREF m_XMinClr, m_YMinClr;
	double m_XMinorUnit, m_YMinorUnit;
	bool m_bAutoXMinUnit, m_bAutoYMinUnit;

	Graph *m_pGraph;
	CWnd *m_pParent;
};

/////////////////////////////////////////////////////////////////////////////
// CScalePage dialog

class CScalePage : public CPropertyPage
{
	friend class CGraphDlg;

	DECLARE_DYNCREATE(CScalePage)

// Construction
public:
	CScalePage();
	CScalePage(CWnd* pParent);
	~CScalePage();

// Dialog Data
	//{{AFX_DATA(CScalePage)
	enum { IDD = IDD_SCALEDLG };
	CFloatEdit	m_ctrlYUnit;
	CFloatEdit	m_ctrlY0;
	CFloatEdit	m_ctrlYMin;
	CFloatEdit	m_ctrlYMax;
	CFloatEdit	m_ctrlXUnit;
	CFloatEdit	m_ctrlX0;
	CFloatEdit	m_ctrlXMin;
	CFloatEdit	m_ctrlXMax;
	CButton	m_ctrlAutoX;
	CButton	m_ctrlAutoY;
	CButton m_ctrlYInverted;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CScalePage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SetValues();
	// Generated message map functions
	//{{AFX_MSG(CScalePage)
	afx_msg void OnAutoX();
	afx_msg void OnAutoY();
	afx_msg void OnYInverted();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void EnableXAxisCtrls(bool bEnable);
	void EnableYAxisCtrls(bool bEnable);
	Graph *m_pGraph;

	bool m_bAutoX, m_bAutoY;
	bool m_bYInverted;

	CWnd* m_pParent;
};

