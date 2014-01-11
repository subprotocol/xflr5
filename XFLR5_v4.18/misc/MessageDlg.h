/****************************************************************************

    CMessageDlg Class
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
#include "afxwin.h"
#include "afxcmn.h"


// Boîte de dialogue CMessageDlg

class CMessageDlg : public CDialog
{
	friend class CMiarex;
	friend class CMainFrame;
	DECLARE_DYNAMIC(CMessageDlg)

public:
	CMessageDlg(CWnd* pParent = NULL);   
	virtual ~CMessageDlg();

// Données de boîte de dialogue
	enum { IDD = IDD_MESSAGEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	CString m_Message;
	bool m_bCancel;

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_ctrlMessage;
	virtual BOOL OnInitDialog();
	void SetMessage(CString Message);
	void SetProgress(int p);
	afx_msg void OnCancel();
	CProgressCtrl m_ctrlProgress;
};
