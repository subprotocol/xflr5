/****************************************************************************

	CLECircleDlg class
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
#include "afxwin.h"

#include "../misc/FloatEdit.h"

// Boîte de dialogue CLECircleDlg

class CLECircleDlg : public CDialog
{
	DECLARE_DYNAMIC(CLECircleDlg)

public:
	CLECircleDlg(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CLECircleDlg();
	CButton m_ctrlLEShow;
	CFloatEdit m_ctrlRadius;
	double m_Radius;
	bool m_bShowRadius;
	CWnd* m_pAFoil;
// Données de boîte de dialogue
	enum { IDD = IDD_LECIRCLEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLERad();
	afx_msg void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillFocusRadius();
};
