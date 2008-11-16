/****************************************************************************

    CChildview Class
	Copyright (C) 2005 André Deperrois xflr5@yahoo.com

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

// ChildView.h : interface of the CChildView class
//
/////////////////////////////////////////////////////////////////////////////


#pragma once


#include "../XDirect/XDirect.h"
#include "../Miarex/Miarex.h"
#include "../Design/AFoil.h"
#include "../XInverse/XInverse.h"


/////////////////////////////////////////////////////////////////////////////
// CChildView window

class CChildView : public CWnd
{
	friend class CXDirect; 
	friend class CXBody; 
	friend class CMiarex;
	friend class CMainFrame;
	friend class CAFoil;
	friend class CXInverse;
	friend class CPrefsDlg;
	friend class CEditFoilDlg;

// Construction
public: 
	CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClrSettings();
	afx_msg void OnSaveImage();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool LoadSettings(CArchive &ar);
	bool GLLoadFont();
	int GetStringWidth(CString strong);
	void GLSetupPalette();
	void SaveSettings(CArchive &ar);
	void SetScale();
	void ApplyClrSettings(bool bBW, COLORREF WingPanels, COLORREF XCPColor,
						  COLORREF XTopColor, COLORREF XBotColor, COLORREF DownwashColor,
						  COLORREF crFoilColor, COLORREF crBLColor, COLORREF crPressureColor) ;

    HGLRC m_hRC;
	HPALETTE m_hPalette;

	// color settings...
	CFrameWnd* m_pFrameWnd;
	COLORREF m_crBackColor;
	COLORREF m_crGraphBkClr;
	COLORREF m_crBorderClr;
	int m_iBorderStyle;
	int m_iBorderWidth;
	int m_dz;
	int m_LetterWidth;
	bool m_bBorder;

	LOGFONT  m_WndLogFont;
	COLORREF m_WndTextColor;
	// end color settings

	int	m_nFilterLoad;
	int m_ImageFileType;
	
	CRect m_rCltRect;
	CXDirect *m_pXDirect;
	CMiarex* m_pMiarex;
	CAFoil* m_pAFoil;
	CXInverse* m_pXInverse;

	Graph m_DefaultGraph;

public:
};

