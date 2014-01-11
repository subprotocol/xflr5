/****************************************************************************

    CScaleOppBar Class
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


//////////////////////////////////////////////////////////////////////
//
// Interface for the CScaleOppBar class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../misc/InitDialogBar.h"


// CScaleOppBar window

class CScaleOppBar : public CInitDialogBar
{
 	friend class CMainFrame;
 	friend class CMiarex;

	DECLARE_DYNAMIC(CScaleOppBar)

	// Construction
public:
	CScaleOppBar(CWnd* pParent=NULL);
	virtual ~CScaleOppBar();

	// Attributes
public:
protected:
	CSliderCtrl	m_ctrlDownWashScale;
	CSliderCtrl	m_ctrlLiftScale;
	CSliderCtrl	m_ctrlDragScale;

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScaleOppBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	CWnd *m_pFrame;
	CWnd *m_pChildView;
	double m_LiftScale;
	double m_DragScale;
	double m_DownwashScale;
	bool m_bLiftChanged, m_bDownwashChanged, m_bDragChanged;
	void Apply();
	virtual BOOL OnInitDialogBar();

	// Implementation
	public:


	// Generated message map functions
	protected:
		void SetPos();
	//{{AFX_MSG(CScaleOppBar)

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMReleasedcaptureDownwashScale(NMHDR *pNMHDR, LRESULT *pResult);
};

