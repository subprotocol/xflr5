/****************************************************************************

    ControlAnalysis Class
	Copyright (C) 2008 André Deperrois xflr5@yahoo.com

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
#include "../misc/EditListCtrl.h"
#include "Plane.h"
#include "../misc/FloatEdit.h"
#include "afxcmn.h"
#include "afxwin.h"


class CControlAnalysis : public CDialog
{
	friend class CMiarex;
	friend class CMainFrame;
	DECLARE_DYNAMIC(CControlAnalysis)

public:
	CControlAnalysis(CWnd* pParent = NULL);   
	virtual ~CControlAnalysis();


	//{{AFX_DATA(CReListDlg)
	enum { IDD = IDD_CONTROLANALYSIS };
	CEdit m_ctrlWPolarName;
	CFloatEdit	m_ctrlDensity;
	CFloatEdit	m_ctrlViscosity;
	CFloatEdit m_ctrlQInf;
	CStatic	m_ctrlSViscosity;
	CStatic	m_ctrlSDensity;
	CStatic	m_ctrlRho;
	CStatic	m_ctrlNu;
	CStatic m_ctrlSpeedUnit;
	CEditListCtrl m_ctrlControlList;
	CStatic m_ctrlUFOName;
	CFloatEdit m_ctrlWeight;
	CStatic m_ctrlWeightUnit;
	CButton m_ctrlViscous;
	CButton m_ctrlWType5;
	CButton m_ctrlWType6;
	//}}AFX_DATA

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    

	DECLARE_MESSAGE_MAP()
public:

private :
	CWnd * m_pMainFrame;

	CFont m_SymbolFont;
	CObArray *m_poaXPolar;
	double m_Density, m_Viscosity;
	CString m_WPolarName;
	CString m_UFOName;

	double m_XCmRef;
	double m_WingTilt;
	double m_StabTilt;

	CPlane *m_pPlane;
	CWing *m_pWing;
	CWing *m_pWing2;
	CWing *m_pStab;
	CWing *m_pFin;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnViscous();
	afx_msg void OnKillFocus();
	afx_msg void OnNMClickCtrlList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickCtrlList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndLabelEditControlList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnType();
	afx_msg void OnOK();
	afx_msg void OnUnit();

	virtual BOOL OnInitDialog();
	bool ReadData();
	void ReadSectionData(int sel);
	void FillControlList();
	void ReadParams();
	void SetDensity();
	void SetWPolarName();

	int m_iSelection;
	int m_UnitType;//1= International, 2= English
	int m_nControls;
	int m_Type;

	double m_MinControl[100], m_MaxControl[100];
	bool m_bActiveControl[100];
	bool m_bViscous;
	double m_QInf;
	double m_Weight;
};
