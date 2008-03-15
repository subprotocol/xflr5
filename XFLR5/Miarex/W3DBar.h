/****************************************************************************

    CW3DBar Class
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
// Interface for the CW3DBar class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../misc/InitDialogBar.h"

// CW3DBar window

class CW3DBar : public CInitDialogBar
{
 	friend class CMainFrame;
 	friend class CMiarex;
	friend class CWOperDlgBar;
	friend class CPOperDlgBar;
	DECLARE_DYNAMIC(CW3DBar)

	// Construction
public:
	CW3DBar(CWnd* pParent=NULL);
	virtual ~CW3DBar();

	// Attributes
public:
protected:

	CButton	m_ctrlAxes;
	CButton	m_ctrlSurfaces;
	CButton	m_ctrlOutline;
	CButton	m_ctrlVLMPanels;
	CButton m_ctrlLight;
	CButton m_ctrlVortices;
	CButton m_ctrlPickCenter;

	CSliderCtrl	m_ctrlClipPlanePos;

	GLfloat m_glXRotatef;
	GLfloat m_glYRotatef;
	GLfloat m_glZRotatef;
//	GLfloat m_glScalef;//zoom factor for wing
//	GLfloat m_glXTransf;//translation vector for wing
//	GLfloat m_glYTransf;

	CWnd* m_pFrame;
	CWnd* m_pMiarex;

	// Operations
	public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CW3DBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	protected:
	virtual BOOL OnInitDialogBar();

	// Implementation
	public:
		void SetChecks();

	// Generated message map functions
	protected:
	//{{AFX_MSG(CW3DBar)
	afx_msg void On3DAxes();
	afx_msg void On3DLight();
	afx_msg void On3DSurfaces();
	afx_msg void On3DOutline();
	afx_msg void On3DVortices();
	afx_msg void On3DPanels();
	afx_msg void On3DReset();
	afx_msg void On3DIso();
	afx_msg void On3DLeft();
	afx_msg void On3DTop();
	afx_msg void On3DFront();
	afx_msg void OnPickCenter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};

////////////////////////////////////////////////////////////////////////////
