/****************************************************************************

	CReListDlg Class
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

// ReListDlg.h : header file
//

#pragma once

#include "../misc/EditListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CReListDlg dialog

class CReListDlg : public CDialog
{
	friend class CBatchDlg;
// Construction
public:
	CReListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CReListDlg)
	enum { IDD = IDD_RELISTDLG };
	CButton	m_ctrlOK;
	CEditListCtrl	m_ctrlReList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReListDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd* m_pParent;
	double m_ReList[30];
	double m_MachList[30];
	double m_NCritList[30];
	int m_NRe;
	int m_iSelection;
	void ReadSectionData(int sel, double &Re, double &Mach, double &NCrit);
	void SetSelection(int sel);
	void FillReList();
	void FillCell(int iItem, int iSubItem);

	// Generated message map functions
	//{{AFX_MSG(CReListDlg)
	afx_msg void OnNMClickReList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickReList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndLabelEditReList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnInsertBefore();
	afx_msg void OnDelete();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
public:
};
