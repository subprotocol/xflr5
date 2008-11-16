/****************************************************************************

    AFoil Control Class
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

// AFoilCtrl.h: interface for the CAFoilCtrlBar class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../misc/NoBeepListCtrl.h"
#include "../misc/InitDialogBar.h"
#include "../misc/NumEdit.h"
#include "../misc/FloatEdit.h"
#include "SF.h"
#include "PF.h"


class CAFoilCtrlBar : public CInitDialogBar
{ 
	DECLARE_DYNAMIC(CAFoilCtrlBar)
	friend class CMainFrame;
	friend class CChildView;
	friend class CAFoil;
	friend class CNoBeepListCtrl;

	// Construction
	public:
	CAFoilCtrlBar(CWnd* pParent = NULL);
	virtual ~CAFoilCtrlBar();

	// Attributes
public:
protected:
	CButton	m_ctrlFoilColor;
	CButton	m_ctrlVisible;
	CButton	m_ctrlCenterLine;
	CButton	m_ctrlModelPoints;
	CButton m_ctrlRename;
	CButton m_ctrlDelete;
	CButton m_ctrlDuplicate;

	CNoBeepListCtrl m_ctrlFoilList;

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAFoilCtrlBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	protected:
	virtual BOOL OnInitDialogBar();
 
	// Implementation
	public:
private:
	void FillFoilList();
	void SetParams(CFoil *pFoil=NULL);
	void SetSplineData();
	void ReSizeCtrls();
	void SetSaveState(bool bState);
	void SetFoil(CFoil* pFoil = NULL);
	void SelectFoil(CFoil* pFoil = NULL);
	void UpdateFoil(int iFoil = -1);
	void UpdateBlt();

	CFoil* GetFoil();
	CFont m_FixedFont;
	CWnd* m_pADlg;
	CFoil *m_pRefFoil;
	CSF *m_pSF;
	CPF *m_pPF;

	int m_FoilSelection;
	// Generated message map functions
protected:
	afx_msg void OnCenterLine();
	afx_msg void OnModelPoints();
	afx_msg void OnVisible();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnFoilColor();
	afx_msg void OnLvnItemchangedFoillist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnRename();
	afx_msg void OnDelete();
	afx_msg void OnDuplicate();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	DECLARE_MESSAGE_MAP()
public:
};
