/****************************************************************************

    CPlane Class
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

// 
#include "afxwin.h"
#include "Wing.h"
#include "../misc/NumEdit.h"
#include "../misc/ClrClasses.h"
#include "../misc/floatedit.h"

/////////////////////////////////////////////////////////////////////////////
// CPlane dialog

class CPlane : public CDialog
{
	friend class CMiarex;
	friend class CMainFrame;
	friend class CNameDlg;
	friend class CPOperDlgBar;
	friend class CVLMDlg;
	friend class CUFOListDlg;

// Construction
public:
	CPlane(CWnd* pParent = NULL);   // standard constructor
 
// Dialog Data
	//{{AFX_DATA(CPlane)
	enum { IDD = IDD_PLANEDLG };
	CButton	m_ctrlCheckPanels;
	CButton	m_ctrlSymFin;
	CStatic	m_ctrlSurf3;
	CStatic	m_ctrlSurf2;
	CStatic	m_ctrlSurf1;
	CStatic	m_ctrlLen1;
	CStatic	m_ctrlLen2;
	CStatic	m_ctrlLen3;
	CStatic	m_ctrlLen4;
	CStatic	m_ctrlLen5;
	CStatic	m_ctrlLen6;
	CStatic	m_ctrlLen7;
	CStatic	m_ctrlWingSpan;
	CStatic	m_ctrlWingSurface;
	CFloatEdit	m_ctrlWingTilt;
	CButton	m_ctrlDefineFin;
	CButton	m_ctrlDefineStab;
	CEdit	m_ctrlPlaneName;
	CStatic	m_ctrlStabVolume;
	CStatic	m_ctrlFinSurface;
	CStatic	m_ctrlStabLeverArm;
	CStatic	m_ctrlStabSurface;
	CButton	m_ctrlVTail;
	CFloatEdit	m_ctrlXLEFin;
	CFloatEdit	m_ctrlYLEFin;
	CFloatEdit	m_ctrlZLEFin;
	CFloatEdit	m_ctrlStabTilt;
	CFloatEdit	m_ctrlZLEStab;
	CFloatEdit	m_ctrlXLEStab;
	CFloatEdit m_ctrlFinTilt;
	CNumEdit m_ctrlVLMTotalPanels;
	CButton m_ctrlDoubleFin;
	CButton	m_ctrlStabCheck;
	CButton	m_ctrlFinCheck;
	CButton	m_ctrlRadio1;
	CButton	m_ctrlRadio2;
	CButton m_ctrlOK;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlane)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd * m_pParent;
	CWnd * m_pFrame;
//	CWing *m_pWing;
	CWing m_Wing;
	CWing m_Stab;
	CWing m_Fin;
	bool m_bActive;
	bool m_bChanged;
	bool m_bFin, m_bStab, m_bCheckName;
	bool m_bDoubleFin, m_bSymFin, m_bDoubleSymFin;
	bool m_bCheckPanels;
	double m_StabTilt;
	double m_FinTilt;
	double m_WingTilt;
	double m_XCmRef;
	double m_TailVolume;
	CObArray *m_poaWing;
	CObArray *m_poaPlane;

	CVector m_LEStab, m_LEFin;
	CString m_PlaneName;

	// Generated message map functions
	//{{AFX_MSG(CPlane)
	virtual BOOL OnInitDialog();
	afx_msg void OnFin();
	afx_msg void OnStab();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDefineFin();
	afx_msg void OnDefineStab();
	afx_msg void OnChanged();
	afx_msg void OnDefineWing();
	afx_msg void OnImportWing();
	afx_msg void OnPlaneName();
	afx_msg void OnDoubleSymFin();
	afx_msg void OnSymFin();
	afx_msg void OnCheckPanels();
	afx_msg void OnDoubleFin();
	afx_msg void OnExportWing();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void ComputePlane(void);
	void CheckPanelDisposition();
	void SetResults();
	void Duplicate(CPlane *pPlane);
	void ReadParams();
	void SetParams();
	bool SerializePlane(CArchive& ar) ;


};

