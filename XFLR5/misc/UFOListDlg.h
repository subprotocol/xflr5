/****************************************************************************

    CUFOListDlg Class
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
#include "afxcmn.h"
#include "afxwin.h"
#include "NoBeepListCtrl.h"

// Boîte de dialogue CUFOListDlg

class CUFOListDlg : public CDialog
{
	DECLARE_DYNAMIC(CUFOListDlg)

public:
	CUFOListDlg(CWnd* pParent = NULL);   
	virtual ~CUFOListDlg();
	CButton m_ctrlDelete;
	CButton m_ctrlSelect;
	CButton m_ctrlRename;
	CNoBeepListCtrl m_ctrlUFOList;

	enum { IDD = IDD_UFOLISTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   

	DECLARE_MESSAGE_MAP()
public:
	CObArray *m_poaWing;
	CObArray *m_poaPlane;
	CObArray *m_poaWPolar;
	CObArray *m_poaFoil;

	bool m_bWing;
	int m_iSelection;
	int m_ColSize[20];
	CWnd * m_pMiarex;
	CWnd * m_pXDirect;
	CWnd * m_pFrame;

	void FillUFOList();
	void FillFoilList();
	void EnableCommands(bool bEnable);

	virtual BOOL OnInitDialog();
	afx_msg void OnSelect();
	afx_msg void OnDelete();
	afx_msg void OnRename();
	afx_msg void OnNMClickUFOList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblClickUFOList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickUFOList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemchangedUfolist(NMHDR *pNMHDR, LRESULT *pResult);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CButton m_ctrlOK;
};
