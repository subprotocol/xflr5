/****************************************************************************

	CBodyNURBSDlg class
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
#include "afxwin.h"
#include "../misc/NumEdit.h"


// Boîte de dialogue CBodyNURBSDlg

class CBodyNURBSDlg : public CDialog
{
	DECLARE_DYNAMIC(CBodyNURBSDlg)

public:
	CBodyNURBSDlg(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CBodyNURBSDlg();

// Données de boîte de dialogue
	enum { IDD = IDD_BODYNURBSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	CNumEdit m_ctrlXPoints;
	CNumEdit m_ctrlHoopPoints;
	virtual BOOL OnInitDialog();

	CWnd *m_pMainFrame;
	int m_NHoopPoints, m_NXPoints;

protected:
	virtual void OnOK();
};
