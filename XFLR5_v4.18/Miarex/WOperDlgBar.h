/****************************************************************************

    CWOperDlgBar Class
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

#include "../misc/InitDialogBar.h"

// CWOperDlgBar window
 
class CWOperDlgBar : public CInitDialogBar
{
 	friend class CMainFrame;
 	friend class CMiarex;
	friend class CLLTThread;
	DECLARE_DYNAMIC(CWOperDlgBar)

	// Construction
public:
	CWOperDlgBar(CWnd* pParent=NULL);
	virtual ~CWOperDlgBar();

	// Attributes
public:
protected:

	CButton	m_ctrlInitCalc;
	CButton	m_ctrlStoreWOpp;
	CButton	m_ctrlSequence;
	CButton m_ctrlAnalyze;
	CStatic m_ctrlSpec;
	CButton m_ctrlAnimate;
	CSliderCtrl	m_ctrlSpeed;
	CButton m_ctrlLift;
	CButton m_ctrlMoments;
	CButton m_ctrlICd, m_ctrlVCd;
	CButton m_ctrlTransitions;
	CButton m_ctrlSurfVelocities;
	CButton m_ctrlDownwash;
	CButton m_ctrlHalfWing;
	CButton m_ctrlCl;
	CButton m_ctrlStream;
	CStatic	m_ctrlDeltaStat; 
	CFloatEdit	m_ctrlDeltaAlpha;
	CFloatEdit	m_ctrlAlphaMax;
	CFloatEdit	m_ctrlAlpha;

	private:
	void EnableAnalysis(bool bEnable);
	void ReadParams();

	bool m_bSequence;
	bool m_bInitCalc;

	CWnd* m_pFrame;
	CWnd* m_pMiarex;

	int m_Type;

	double m_Alpha0;//Angle of Attack in °
	double m_AlphaMax;//Angle of Attack in °
	double m_DeltaAlpha;//Angle of Attack in °
	double m_QInf0;
	double m_QInfMax;
	double m_DeltaQInf;
	double m_Control0;
	double m_ControlMax;
	double m_DeltaControl;

	CFont m_SymbolFont;
	CFont m_StdFont;

	// Operations
	public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWOperDlgBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	protected:
	virtual BOOL OnInitDialogBar();

	// Implementation
	public:

	// Generated message map functions
	protected:
		void SetParams(CWPolar* pWPolar);
	//{{AFX_MSG(CWOperDlgBar)
	afx_msg void OnInitCalc();
	afx_msg void OnAnalyze();
	afx_msg void OnAnimate();
	afx_msg void OnShowLift();
	afx_msg void OnShowMoments();
	afx_msg void OnShowICd();
	afx_msg void OnShowVCd();
	afx_msg void OnShowTrans();
	afx_msg void OnShowDownwash();
	afx_msg void OnShowCl();
	afx_msg void OnStoreWOpp();
	afx_msg void OnWSequence();
	afx_msg void OnStream();
	afx_msg void OnSurfVelocities();
	afx_msg void OnHalfWing();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////

