/****************************************************************************

	Geometry Dialog class
    Copyright (C) 2004 André Deperrois XFLR5@yahoo.com

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

// GeomDlg.h : header file
//
#include "../misc/FloatEdit.h"
#include "XFoil.h"

/////////////////////////////////////////////////////////////////////////////
// CGeomDlg dialog
 
class CGeomDlg : public CDialog
{
	friend class CXDirect;
	friend class CAFoil;
// Construction
public:
	CGeomDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGeomDlg)
	enum { IDD = IDD_GEOMDLG };
	CButton	m_ctrlOK;
	CButton	m_ctrlCamb;
	CSliderCtrl	m_ctrlCamberSlide;
	CSliderCtrl	m_ctrlThickSlide;
	CSliderCtrl	m_ctrlXThickSlide;
	CSliderCtrl	m_ctrlXCamberSlide;
	CFloatEdit	m_ctrlXCamber;
	CFloatEdit	m_ctrlXThickness;
	CFloatEdit	m_ctrlThickness;
	CFloatEdit	m_ctrlCamber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeomDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	double m_fCamber;
	double m_fThickness;
	double m_fXCamber;
	double m_fXThickness;
	XFoil* m_pXFoil;
	CFoil* m_pBufferFoil;
	CFoil* m_pMemFoil;
	CWnd *m_pChildView;
	bool  m_bApplied,m_bAppliedX, m_bModified;
	void Apply();

public:

protected:

	// Generated message map functions
	//{{AFX_MSG(CGeomDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRestore();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChanged();
	afx_msg void OnChangedX();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

