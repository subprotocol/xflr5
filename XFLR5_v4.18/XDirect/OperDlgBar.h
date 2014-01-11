/****************************************************************************

	COperDlgBar Class
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
// OperDlgBar.h: interface for the COperDlgBar class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
 
#include "../misc/NumEdit.h"
#include "../misc/FloatEdit.h"
#include "../misc/InitDialogBar.h"
#include "XFoil.h"

class COperDlgBar  : public CInitDialogBar
{
	friend class CXDirect;
	friend class CMainFrame;
	// Construction
public:
	COperDlgBar(CWnd* pParent =NULL);
	virtual ~COperDlgBar();
	DECLARE_DYNAMIC(COperDlgBar)


	// Attributes
	public:
	void EnableDispCtrls(bool show);
	void EnableXFoilCtrls(bool show);
	void SetPolar();
	void SetOpPointSequence();

	bool m_bSequence;
//	double m_AlphaDisp;
	double m_Alpha, m_AlphaMax, m_DeltaAlpha;
	double m_Cl, m_ClMax, m_DeltaCl;
	double m_Re, m_ReMax, m_DeltaRe;
	int m_IterLim;

	CWnd* m_pXDirect;

	XFoil* m_pXFoil;
	protected:
	CButton m_ctrlType;
	CButton	m_ctrlViscous;
	CButton	m_ctrlSequence;
	CButton	m_ctrlStoreOpp;
	CButton	m_ctrlAnalyze;
	CButton	m_ctrlAnimate;
	CButton	m_ctrlInitBL;
	CButton m_ctrlParam1;
	CButton m_ctrlParam2;
	CButton m_ctrlParam3;
	
	CButton	m_ctrlPressure;
	CSliderCtrl	m_ctrlSpeed;
	CButton	m_ctrlBL;

	CStatic	m_ctrlAlphaStat;
	CStatic	m_ctrlAMaxStat;
	CStatic	m_ctrlDAlphaStat;

	CFloatEdit	m_ctrlDeltaAlpha;
	CFloatEdit	m_ctrlAlphaMax;
	CFloatEdit	m_ctrlAlpha;
	// Operations
	public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COperDlgBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	protected:
	virtual BOOL OnInitDialogBar();

	// Implementation
private:
	void SetParams();
	void ReadParams();
	CFont m_SymbolFont;

	// Generated message map functions
	protected:
		void SetAnimate(bool bAnimate);
	//{{AFX_MSG(COperDlgBar)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnAnimate();
	afx_msg void OnSequence();
	afx_msg void OnAnalyze();
	afx_msg void OnViscous();
	afx_msg void OnInitBL();
	afx_msg void OnShowBL();
	afx_msg void OnShowPressure();
	afx_msg void OnParam1();
	afx_msg void OnStoreOpp();
	afx_msg void OnKillFocus();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
