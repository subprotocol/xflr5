/****************************************************************************

	Foil Analysis Dialog class
	Copyright (C) 2004 André Deperrois xflr5@yahoo.com

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

// AnalysisDlg.h : header file
//
#include "../misc/FloatEdit.h"
#include "../misc/NumEdit.h"
/////////////////////////////////////////////////////////////////////////////
// CFoilAnalysisDlg dialog

class CFoilAnalysisDlg : public CDialog
{
	friend class CXDirect;
// Construction
public:
	CFoilAnalysisDlg(CWnd* pParent = NULL);   // standard constructor


// Dialog Data
	//{{AFX_DATA(CFoilAnalysisDlg)
	enum { IDD = IDD_FOILANALYSISDLG };
	CFloatEdit	m_ctrlNCrit;
	CFloatEdit	m_ctrlXTrip1;
	CFloatEdit	m_ctrlXTrip2;
	CButton	m_ctrlOK;
	CFloatEdit	m_ctrlReNbr;
	CStatic	m_ctrlMatyp;
	CEdit	m_ctrlNewPlrName;
	CStatic	m_ctrlRetyp;
	CButton	m_ctrlType;
	CFloatEdit	m_ctrlMachNbr;
	double	m_fMach;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFoilAnalysisDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private :
	void EnableControls(bool bEnable);
	void Initialize();
	void ReadParams();

	bool  m_bAutoName;
	int   m_Type;
	int   m_MaTypDef, m_ReTypDef;
	double m_Reynolds;
	double m_ReDef;
	double m_MachDef;
	double m_ASpec;
	double m_XTopTr, m_XBotTr;
	double m_NCrit;
	CString m_FoilName;
	CString m_PlrName;
	CWnd* m_pParent;

protected:
	// Generated message map functions
	//{{AFX_MSG(CFoilAnalysisDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnType1();
	afx_msg void OnSetfocusNewplrname();
	afx_msg void OnName();
	afx_msg void SetPlrName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

