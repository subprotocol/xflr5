/****************************************************************************

    SplinesCtrlDlg Class
    Copyright (C) 2006 André Deperrois XFLR5@yahoo.com

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
#include "../misc/NumEdit.h"
#include "../misc/FloatEdit.h"

#include "SF.h"
#include "PF.h"

// Boîte de dialogue CSplinesCtrlDlg

class CSplinesCtrlDlg : public CDialog
{
	friend class CAFoil;
	DECLARE_DYNAMIC(CSplinesCtrlDlg)

public:
	CSplinesCtrlDlg(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CSplinesCtrlDlg();

	CNumEdit	m_ctrlOutExtrados;
	CNumEdit	m_ctrlOutIntrados;
	CComboBox	m_ctrlDegExtrados;
	CComboBox	m_ctrlDegIntrados;
	CSpinButtonCtrl	m_ctrlSpinOut;
	CSpinButtonCtrl	m_ctrlSpinIn;
	CButton m_ctrlOutText;

// Données de boîte de dialogue
	enum { IDD = IDD_SPLINECONTROLSDLG };

protected:

	bool m_bSF;
	CSF *m_pSF;
	CPF *m_pPF;

	CWnd* m_pADlg;
//	void SetParams();

	virtual void DoDataExchange(CDataExchange* pDX);
protected:
	//{{AFX_MSG(CAFoilCtrlBar)
	afx_msg void OnSelChangeDegExtrados();
	afx_msg void OnSelChangeDegIntrados();
	afx_msg void OnEditChangeDegExtrados();
	afx_msg void OnEditChangeDegIntrados();
	afx_msg void OnKillFocusOutExtrados();
	afx_msg void OnKillFocusOutIntrados();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
