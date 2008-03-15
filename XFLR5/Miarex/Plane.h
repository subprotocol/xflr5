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
#include "Body.h"
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
	friend class CBody;
	friend class C3DPanelDlg;
	friend class CBodyCtrlBar;


// Construction
public:
	CPlane(CWnd* pParent, CWnd* pMiarex);   // standard constructor
 
// Dialog Data
	//{{AFX_DATA(CPlane)
	enum { IDD = IDD_PLANEDLG };
	CStatic	m_ctrlSurf1;
	CStatic	m_ctrlSurf2;
	CStatic	m_ctrlSurf3;
	CStatic m_ctrlVolume;
	CStatic m_ctrlVolumeUnit;
	CStatic	m_ctrlLen1;
	CStatic	m_ctrlLen2;
	CStatic	m_ctrlLen3;
	CStatic	m_ctrlLen4;
	CStatic	m_ctrlLen5;
	CStatic	m_ctrlLen6;
	CStatic	m_ctrlLen7;
	CStatic m_ctrlLen8;
	CStatic m_ctrlLen9;
	CStatic m_ctrlLen10;
	CStatic m_ctrlLen11;
	CStatic	m_ctrlWingSpan;
	CStatic	m_ctrlWingSurface;
	CStatic	m_ctrlStabVolume;
	CStatic	m_ctrlFinSurface;
	CStatic	m_ctrlStabLeverArm;
	CStatic	m_ctrlStabSurface;
	CFloatEdit m_ctrlXLEFin;
	CFloatEdit m_ctrlYLEFin;
	CFloatEdit m_ctrlZLEFin;
	CFloatEdit m_ctrlZLEStab;
	CFloatEdit m_ctrlXLEStab;
	CFloatEdit m_ctrlXLEWing;
	CFloatEdit m_ctrlZLEWing;
	CFloatEdit m_ctrlXLEWing2;
	CFloatEdit m_ctrlZLEWing2;
	CFloatEdit m_ctrlStabTilt;
	CFloatEdit m_ctrlFinTilt;
	CFloatEdit m_ctrlWingTilt;
	CFloatEdit m_ctrlWingTilt2;
	CNumEdit m_ctrlVLMTotalPanels;
	CEdit m_ctrlPlaneName;
	CComboBox m_ctrlBodyList;
	CButton m_ctrlBody;
	CButton m_ctrlDoubleFin;
	CButton	m_ctrlStabCheck;
	CButton	m_ctrlFinCheck;
	CButton m_ctrlBiplane;
	CButton m_ctrlDefineWing2;
	CButton m_ctrlImportWing2;
	CButton m_ctrlExportWing2;
	CButton	m_ctrlDefineFin;
	CButton	m_ctrlVTail;
	CButton	m_ctrlDefineStab;
	CButton	m_ctrlCheckPanels;
	CButton	m_ctrlSymFin;
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
	static CObArray *s_poaWing;
	static CObArray *s_poaBody;

	CWnd * m_pMiarex;
	CWnd * m_pFrame;
	CBody *m_pBody;
	CWing m_Wing, m_Wing2;
	CWing m_Stab;
	CWing m_Fin;
	bool m_bBody;
	bool m_bActive;
	bool m_bChanged;
	bool m_bFin, m_bStab, m_bCheckName;
	bool m_bDoubleFin, m_bSymFin, m_bDoubleSymFin;
	bool m_bBiplane;
	double m_StabTilt;
	double m_FinTilt;
	double m_WingTilt, m_WingTilt2;
	double m_Volume;
	double m_XCmRef;
	double m_TailVolume;

	CVector m_LEStab, m_LEFin, m_LEWing, m_LEWing2;
	CString m_PlaneName;

	// Generated message map functions
	//{{AFX_MSG(CPlane)
	virtual BOOL OnInitDialog();
	afx_msg void OnFin();
	afx_msg void OnStab();
	virtual void OnOK();
	afx_msg void OnBodyCheck();
	virtual void OnCancel();
	afx_msg void OnDefineFin();
	afx_msg void OnDefineStab();
	afx_msg void OnChanged();
	afx_msg void OnDefineWing();
	afx_msg void OnImportWing();
	afx_msg void OnPlaneName();
	afx_msg void OnSymFin();
	afx_msg void OnDoubleFin();
	afx_msg void OnExportWing();
	afx_msg void OnBiplane();
	afx_msg void OnDefineWing2();
	afx_msg void OnImportWing2();
	afx_msg void OnExportWing2();
	afx_msg void OnSelChangeBodyList();
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
	bool HasResults();


public:

};

