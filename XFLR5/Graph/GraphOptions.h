/****************************************************************************

    Graph Options class
	Copyright (C) 2003 André Deperrois xflr5@yahoo.com

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


// GraphOptions.h : header file
//
#include "../misc/ClrClasses.h"
#include "../misc/LineCbBox.h"
 
/////////////////////////////////////////////////////////////////////////////
// CGraphOptions dialog

class CGraphOptions : public CDialog
{
	friend class CXDirect;
// Construction
public:
	CGraphOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGraphOptions)
	enum { IDD = IDD_GRAPHOPTIONS };
	CButton	m_ctrlYMin;
	CButton	m_ctrlXMin;
	CButton	m_ctrlYMain;
	CButton	m_ctrlXMain;
	CButton	m_ctrlAxis;
	CFloatEdit	m_ctrlMOrigin;
	CFloatEdit	m_ctrlMMin;
	CFloatEdit	m_ctrlMMax;
	CFloatEdit	m_ctrlMUnit;
	CFloatEdit	m_ctrlLOrigin;
	CFloatEdit	m_ctrlLMin;
	CFloatEdit	m_ctrlLMax;
	CFloatEdit	m_ctrlLUnit;
	CFloatEdit	m_ctrlDOrigin;
	CFloatEdit	m_ctrlDMin;
	CFloatEdit	m_ctrlDMax;
	CFloatEdit	m_ctrlDUnit;
	CFloatEdit	m_ctrlAOrigin;
	CFloatEdit	m_ctrlAMin;
	CFloatEdit	m_ctrlAMax;
	CFloatEdit	m_ctrlAUnit;
	CButton	m_ctrlAuto;
	BOOL	m_bXMaj;
	BOOL	m_bYMaj;
	BOOL	m_bXMin;
	BOOL	m_bYMin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	LOGFONT m_TitleLogFont;
	COLORREF m_TitleFontColor;

	LOGFONT m_LabelLogFont;
	COLORREF m_LabelFontColor;

	LOGFONT m_LegendLogFont;
	COLORREF m_LegendFontColor;

	// axis data
	int m_nStyle;
	int m_nWidth;
	COLORREF m_crColor;
	// Main grid data
	int m_XMStyle, m_YMStyle;
	int m_XMWidth, m_YMWidth;
	COLORREF m_XMClr, m_YMClr;
	// Min grid data
	int m_XMinStyle, m_YMinStyle;
	int m_XMinWidth, m_YMinWidth;
	COLORREF m_XMinClr, m_YMinClr;
	double m_XMinorUnit, m_YMinorUnit;//unused... too different from graph to graph...

protected:
	void SetXMajData(bool b, int s, int w, COLORREF clr);
	void SetYMajData(bool b, int s, int w, COLORREF clr);
	void SetXMinData(bool b, int s, int w, COLORREF clr, double f);
	void SetYMinData(bool b, int s, int w, COLORREF clr, double f);
	void GetAxisData(int *pStyle, int *pWidth, COLORREF *pClr);
	void SetAxisData(int Style, int Width, COLORREF Clr);
	COLORREF GetTitleFontColor();
	COLORREF GetLabelFontColor();
	COLORREF GetLegendFontColor();
	void GetTitleLogFont(LOGFONT *plgft);
	void GetLabelLogFont(LOGFONT *plgft);
	void GetLegendLogFont(LOGFONT *plgft);

	void SetTitleFontColor(COLORREF crColor);
	void SetLabelFontColor(COLORREF crColor);
	void SetLegendFontColor(COLORREF crColor);
	void SetTitleLogFont(LOGFONT *plgft);
	void SetLabelLogFont(LOGFONT *plgft);
	void SetLegendLogFont(LOGFONT *plgft);

	double LMin, LMax, LOrigin, LUnit;
	double DMin, DMax, DOrigin, DUnit;
	double MMin, MMax, MOrigin, MUnit;
	double AMin, AMax, AOrigin, AUnit;
	
	bool m_bAuto;

	CWnd* m_pParent;

	// Generated message map functions
	//{{AFX_MSG(CGraphOptions)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnAutoGraph();
	afx_msg void OnTitleFont();
	afx_msg void OnApply();
	afx_msg void OnLabelFont();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnXMainStyle();
	afx_msg void OnYMainStyle();
	afx_msg void OnAxisStyle();
	afx_msg void OnXMinStyle();
	afx_msg void OnYMinStyle();
	afx_msg void OnLegendFont();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
