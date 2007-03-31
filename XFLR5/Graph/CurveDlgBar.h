/****************************************************************************

    PolarDlgBar Class
    Copyright (C) 2003 André Deperrois XFLR5@yahoo.com

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
//////////////////////////////////////////////////////////////////////

// PolarDlgBar.h: interface for the CCurveDlgBar class.
//
//////////////////////////////////////////////////////////////////////


#pragma once

#include "../misc/InitDialogBar.h"
#include "../misc/ClrClasses.h"
#include "../misc/LineCbBox.h"

class CCurveDlgBar : public CInitDialogBar   
{

	// Construction
public:
	CCurveDlgBar(CWnd* pParent = NULL);
	virtual ~CCurveDlgBar();
	DECLARE_DYNAMIC(CCurveDlgBar)


	// Attributes
	public:
	protected:

	// Control Objects
	CButton m_ctrlShow;
	CButton m_ctrlShowPoints;
	CLineCbBox m_ctrlStyle;
	CLineCbBox m_ctrlWidth;
	CClrBtn m_ctrlColor;

	// Polarations
	public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCurveDlgBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL


	// Implementation
public:
	UINT m_idApp;
	void Update();
	void SetParams();
	CWnd* m_pParent;
	CWnd* m_pFrame;

private:
	int m_nStyle;
	int m_nWidth;
	COLORREF m_crColor;


	// Generated message map functions
	protected:
	//{{AFX_MSG(CMainDialogBar)
	afx_msg void OnShow();
	virtual BOOL OnInitDialogBar();
	afx_msg void OnShowPoints();
	afx_msg void OnColorBtn();
	afx_msg void OnSelChangeStyle();
	afx_msg void OnSelChangeWidth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

