/****************************************************************************

    CGLLight Class
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

#pragma once
#include "afxwin.h"
// GLLight.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGLLight dialog

class CGLLight : public CDialog
{ 
	friend class CMiarex;
// Construction
public:
	CGLLight(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGLLight)
	enum { IDD = IDD_GLLIGHT };
	CSliderCtrl m_ctrlRed;
	CSliderCtrl m_ctrlGreen;
	CSliderCtrl m_ctrlBlue;
	CSliderCtrl	m_ctrlAmbient;
	CSliderCtrl	m_ctrlDiffuse;
	CSliderCtrl	m_ctrlSpecular;
	CSliderCtrl	m_ctrlMatAmbient;
	CSliderCtrl	m_ctrlMatDiffuse;
	CSliderCtrl	m_ctrlMatSpecular;
	CSliderCtrl	m_ctrlMatShininess;
	CSliderCtrl	m_ctrlMatEmission;
	CSliderCtrl m_ctrlXLight;
	CSliderCtrl m_ctrlYLight;
	CSliderCtrl m_ctrlZLight;
	CButton m_ctrlColorMaterial;
	CButton m_ctrlDepthTest;
	CButton m_ctrlCullFaces;
	CButton m_ctrlShade;
	CButton m_ctrlSmooth;
	CButton m_ctrlLocalView;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLLight)
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd* m_pMiarex;

	float m_Ambient, m_Diffuse, m_Specular;
	float m_MatAmbient, m_MatDiffuse, m_MatSpecular, m_MatEmission;
	float m_Red, m_Green, m_Blue;
	float m_XLight, m_YLight, m_ZLight;
	int m_iMatShininess;
	bool m_bCullFaces, m_bShade, m_bSmooth, m_bLocalView, m_bDepthTest;
	bool m_bColorMaterial;

	// Generated message map functions
	//{{AFX_MSG(CGLLight)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChanged();	
	afx_msg void OnColorMaterial();
	afx_msg void OnDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void ReadParams(void);
	void SetParams(void);
	bool LoadSettings(CArchive &ar);
	bool SaveSettings(CArchive &ar);
	void Apply();
};

