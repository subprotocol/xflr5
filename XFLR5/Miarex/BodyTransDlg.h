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


// Boîte de dialogue CBodyTransDlg

class CBodyTransDlg : public CDialog
{
	friend class CMiarex;
	friend class CXBody;
	DECLARE_DYNAMIC(CBodyTransDlg)

public:

	CFloatEdit	m_ctrlXTransFactor;
	CFloatEdit	m_ctrlYTransFactor;
	CFloatEdit	m_ctrlZTransFactor;

	CStatic	m_ctrlLength1;
	CStatic	m_ctrlLength2;
	CStatic	m_ctrlLength3;

	CNumEdit    m_ctrlFrameID;
	CButton m_ctrlOK;

	CBodyTransDlg(CWnd* pParent = NULL);  
	virtual ~CBodyTransDlg();

	enum { IDD = IDD_BODYTRANSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CWnd * m_pMiarex;
	CWnd * m_pMainFrame;

	double m_XTrans, m_YTrans, m_ZTrans;
	bool m_bFrameOnly;
	int m_FrameID;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRadio1();
	afx_msg void OnKillFocusFrameId();
	afx_msg void OnOK();
};
