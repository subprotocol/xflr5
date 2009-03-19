/****************************************************************************

    CWPolarAnalysis Class
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
// WPolarAnalysis.h : header file
//
#include "WakeParamsDlg.h"
#include "../misc/FloatEdit.h"
#include "../misc/NumEdit.h"
#include "Plane.h"
#include "afxwin.h"
#include "afxcmn.h"

/////////////////////////////////////////////////////////////////////////////
// CWPolarAnalysis dialog

class CWPolarAnalysis : public CDialog
{
	friend class CLLTThread;
	friend class CMiarex;
	friend class CMainFrame;
// Construction
public:
	CWPolarAnalysis(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWPolarAnalysis)
	enum { IDD = IDD_WPOLARANALYSIS };
	CButton	m_ctrlWakeRollUp;
	CButton	m_ctrlVLM2;
	CButton	m_ctrlVLM1;
	CStatic	m_ctrlLengthUnit;
	CStatic	m_ctrlLengthUnit2;
	CFloatEdit	m_ctrlXCmRef;
	CStatic	m_ctrlRho;
	CStatic	m_ctrlNu;
	CButton	m_ctrlMethod1;
	CButton	m_ctrlMethod3;
	CStatic	m_ctrlQInfStat;
	CStatic	m_ctrlQInfCl;
	CStatic	m_ctrlReTip;
	CStatic	m_ctrlReRoot;
	CButton	m_ctrlAutoName;
	CFloatEdit	m_ctrlDensity;
	CFloatEdit	m_ctrlViscosity;
	CStatic	m_ctrlSViscosity;
	CStatic	m_ctrlSDensity;
	CStatic	m_ctrlWLUnit;
	CStatic	m_ctrlWingLoad;
	CStatic	m_ctrlSpeedUnit;
	CStatic	m_ctrlWeightUnit;
	CFloatEdit	m_ctrlAlpha;
	CFloatEdit	m_ctrlBeta;
	CFloatEdit	m_ctrlWeight;
	CButton m_ctrlGroundEffect;
	CFloatEdit m_ctrlHeight;
	CButton	m_ctrlType1;
	CButton	m_ctrlOK;
	CButton m_ctrlViscous;
	CButton m_ctrlTiltGeom;
	CEdit	m_ctrlWPolarName;
	CButton m_ctrlWakeParams;
	CStatic	m_ctrlWingName;
	CFloatEdit	m_ctrlQInf;
	CStatic	m_ctrlSRe;
	CStatic	m_ctrlRRe;
	BOOL	m_BAutoName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWPolarAnalysis)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

private:

	CWakeParamsDlg m_WakeParamsdlg;
	CFont m_SymbolFont;
	void SetWingLoad();
	void EnableControls();
	void CheckType();
	void SetWPolarName();
	void ReadParams();
	void SetReynolds();
	void SetDensity();

	CWnd* m_pParent;//MainFrame !
	CObArray *m_poaXPolar;

	int m_Type;
	int m_UnitType;//1= International, 2= English
	int m_AnalysisType; //0=LLT;1=VLM;2=Panel

	bool m_bVLM1; //true if Classic, false if Quendez
	bool m_bThinSurfaces;//true if Plane Panel calculation on middle surface, false if on top & bottom
	bool m_bWakeRollUp;//true if wake roll up is to be taken into account in calculation
	bool m_bTiltedGeom;//true if calculation is performed on the tilted geometry, at alpha=0.0
	bool m_bViscous;
	bool m_bGround;

	double m_QInf, m_Weight, m_Alpha, m_XCmRef;
	double m_Beta;
	double m_Density, m_Viscosity;
	double m_WingLoad;
	double m_Height;

	int m_NXWakePanels;
	double m_TotalWakeLength;//% mac
	double m_WakePanelFactor;

	CWing* m_pWing;
	CPlane * m_pPlane;

	CString m_WPolarName;
	CString m_UFOName;

	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillFocusQInf();
	afx_msg void OnType();
	afx_msg void OnGroundEffect();
	afx_msg void OnKillFocusHeight();
	afx_msg void OnKillFocusWeight();
	afx_msg void OnKillFocusAlpha();
	afx_msg void OnKillFocusBeta();
	afx_msg void OnKillFocusDensity();
	afx_msg void OnKillFocusViscosity();
	afx_msg void OnUnit();
	afx_msg void OnAutoName();
	afx_msg void OnSetFocusWPolarName();
	afx_msg void OnMethod();
	afx_msg void OnKillFocusXCmRef();
	afx_msg void OnVLMMethod();
	afx_msg void OnWakeRollUp();
	afx_msg void OnViscous();
	afx_msg void OnTiltedGeom();
	DECLARE_MESSAGE_MAP()


public:
	afx_msg void OnWakeParams();
	CButton m_ctrlThinSurfaces;
	afx_msg void OnThinSurfaces();
};

//{{AFX_INSERT_LOCATION}}
