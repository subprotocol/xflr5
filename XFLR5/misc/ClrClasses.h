/****************************************************************************

All Color and style selection classes 
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


class CClrBtn : public CButton{

// Construction
public:
	CClrBtn(CWnd* pParent = NULL);


// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClrBtn)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetColor(COLORREF clr);
	COLORREF GetColor();
	virtual ~CClrBtn();

	// Generated message map functions
protected:
	//{{AFX_MSG(CClrBtn)
	afx_msg BOOL OnClicked();
   	//}}AFX_MSG
 void DrawItem(LPDRAWITEMSTRUCT);

	DECLARE_MESSAGE_MAP()
private:
	static COLORREF m_clrUser[6];
	static int m_iDefined;

    COLORREF m_clrCurrent;
};


// ClrDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CClrDlg dialog

class CClrDlg : public CDialog
{ 
// Construction
public:
	CClrDlg(CWnd* pParent = NULL);   // standard constructor
	COLORREF GetCurrentColor();
	COLORREF GetUserColor(int i);
	void SetCurrentColor(COLORREF clr);
	void SetUserColor(int i, COLORREF clr);
	void SetUserDefined(int i);
	int GetUserDefined();
	void EndDialog(int nResult);
	void OnColor(UINT id);
	void InitColors();

// Dialog Data
	//{{AFX_DATA(CClrDlg)
	enum { IDD = IDD_CLRDLG };
	CButton	m_ctrlOtherClr;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClrDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

private:
	COLORREF m_clrCurrent;
    CButton *m_pParent;
	int m_iSelected;
	int m_iDefined;
	COLORREF m_crColors[36];
	// Generated message map functions
	//{{AFX_MSG(CClrDlg)
	afx_msg void OnOtherClr();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
