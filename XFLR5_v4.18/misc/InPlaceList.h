/****************************************************************************

    CInPlaceList Class
	Copyright (C) 2006 André Deperrois xflr5@yahoo.com

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
#include "afxwin.h"

class CInPlaceList :
	public CComboBox
{

// Construction
public:
	CInPlaceList();
	CInPlaceList(CListCtrl *pCtrl, int iItem, int iSubItem);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceList)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceList();
	void Set(int iItem, int iSubItem);

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnCloseup();
	afx_msg void OnCbnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int 	m_nSel;
	int 	m_iItem;
	int 	m_iSubItem;
	bool m_bWingCtrl;
	BOOL	m_bESC;				// To indicate whether ESC key was pressed
	CStringArray m_strArray ;
public:
	CListCtrl *m_pListCtrl;

};
