/****************************************************************************

    LinePickerDlg Class
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

// LinePickerDlg.h : header file
//
#include "LineCbBox.h"
#include "ClrClasses.h"


/////////////////////////////////////////////////////////////////////////////
// CLinePickerDlg dialog

class CLinePickerDlg : public CDialog
{
// Construction
public:
	CLinePickerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLinePickerDlg)
	enum { IDD = IDD_LINEPICKERDLG };
	CButton	m_ctrlOK;
	CLineCbBox	m_ctrlWidth;
	CLineCbBox	m_ctrlStyle;
	CClrBtn	m_ctrlColorBtn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinePickerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetWidth(int nWidth);
	void SetStyle(int nStyle);
	void SetColor(COLORREF crColor);
	int GetWidth();
	int GetStyle();
	COLORREF GetColor();

protected:
	CWnd* m_pParent;

private:
	int m_nStyle;
	int m_nWidth;
	COLORREF m_crColor;

	// Generated message map functions
	//{{AFX_MSG(CLinePickerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeWidth();
	afx_msg void OnSelChangeStyle();
	afx_msg void OnColorBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

