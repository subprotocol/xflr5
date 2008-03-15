/****************************************************************************

    CBodyScale Class
	Copyright (C) 2007 André Deperrois xflr5@yahoo.com

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

#include "../misc/FloatEdit.h"
#include "../misc/NumEdit.h"
#include "afxwin.h"

#pragma once


// Boîte de dialogue CBodyScaleDlg

class CBodyScaleDlg : public CDialog
{
	friend class CMiarex;
	DECLARE_DYNAMIC(CBodyScaleDlg)

public:
	afx_msg void OnOK();

	CFloatEdit	m_ctrlXScaleFactor;
	CFloatEdit	m_ctrlYScaleFactor;
	CFloatEdit	m_ctrlZScaleFactor;
	CNumEdit    m_ctrlFrameID;
	CButton m_ctrlOK;

	CBodyScaleDlg(CWnd* pParent = NULL);   // constructeur standard
	virtual ~CBodyScaleDlg();

// Données de boîte de dialogue
	enum { IDD = IDD_BODYSCALEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge DDX/DDV
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CWnd * m_pMiarex;

	double m_XFactor, m_YFactor, m_ZFactor;
	bool m_bFrameOnly;
	int m_FrameID;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRadio1();
	afx_msg void OnKillFocusFrameId();
};
