/****************************************************************************

    LineCbBox Class
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

// LineCbBox.h : header file
//
#include "../X-FLR5.h" //may be suppressed at release time

/////////////////////////////////////////////////////////////////////////////
// CLineCbBox window

struct LineData{
	public:
		int nPenStyle;
		int nWidth;
		COLORREF crColor; 
};


class CLineCbBox : public CComboBox{
// Construction
public:
	CLineCbBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLineCbBox)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	bool m_bWidth;
	LineData m_Lines[10];
	void DrawLine(LPDRAWITEMSTRUCT lpDIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual int DeleteString(int nIndex);
	virtual int InsertString(int nIindex, LPCTSTR lpszString);
	virtual int AddString(LPCTSTR lpszString);
	int AddLine();
	virtual ~CLineCbBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLineCbBox)

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

