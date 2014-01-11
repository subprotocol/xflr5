/****************************************************************************

    CSpanPosBar Class
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


//////////////////////////////////////////////////////////////////////
//
// Interface for the CSpanPosBar class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../misc/InitDialogBar.h"
#include "../misc/FloatEdit.h"

// CSpanPosBar window

class CSpanPosBar : public CInitDialogBar
{
 	friend class CMainFrame;
 	friend class CMiarex;

	DECLARE_DYNAMIC(CSpanPosBar)

	// Construction
public:
	CSpanPosBar(CWnd* pParent=NULL);
	virtual ~CSpanPosBar();

	// Attributes
public:
protected:
	CSliderCtrl	m_ctrlSpanSlide;
	CFloatEdit	m_ctrlSpanPos;
	CButton m_ctrlKeepCurve;
	CButton m_ctrlResetCurves;


	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpanPosBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKillFocusSpanPos();
	afx_msg void OnKeepCurve();
	afx_msg void OnResetCurves();

	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	CWnd *m_pFrame;
	CWnd *m_pMiarex;
	CWnd *m_pChildView;

	double m_SpanPos;
	bool m_bApplied;
	void Apply();
	virtual BOOL OnInitDialogBar();

	// Implementation
	public:


	// Generated message map functions
	protected:
	//{{AFX_MSG(CSpanPosBar)

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

