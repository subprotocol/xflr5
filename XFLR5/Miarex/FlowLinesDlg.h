/****************************************************************************
	
	FlowLinesDlg	class
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

#pragma once
#include "../misc/NumEdit.h"
#include "../misc/FloatEdit.h"
#include "afxwin.h"


class CFlowLinesDlg : public CDialog
{
	friend class CMiarex;
	friend class CMainFrame;
	friend class CWOperDlgBar;
	DECLARE_DYNAMIC(CFlowLinesDlg)

public:
	CFlowLinesDlg(CWnd* pParent = NULL);  
	virtual ~CFlowLinesDlg();

	enum { IDD = IDD_FLOWLINESDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);  
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()
public:
	void SetUnits();
	void SetParams();
	void ReadParams();
	CNumEdit m_ctrlNX;
	CFloatEdit m_ctrlXOffset;
	CFloatEdit m_ctrlZOffset;
	CFloatEdit m_ctrlXFactor;
	CFloatEdit m_ctrlDeltaL;
	CButton m_ctrlApply;
	CStatic m_ctrlLength2;
	CStatic m_ctrlLength3;
	CStatic m_ctrlLength1;
protected:
	bool m_bLE;
	int m_NX;
	double m_DeltaL;
	double m_XFactor;
	double m_XOffset, m_ZOffset;
	CWnd *m_pMiarex, *m_pChildWnd;
	CWnd *m_pFrame;

public:
	afx_msg void OnApply();
	afx_msg void OnClose();
	afx_msg void OnChanged();
protected:
};
