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

// BodyScaleDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "BodyTransDlg.h"
#include ".\bodytransdlg.h"
#include "Miarex.h"
#include "../main/MainFrm.h"
// Boîte de dialogue CBodyTransDlg

IMPLEMENT_DYNAMIC(CBodyTransDlg, CDialog)

CBodyTransDlg::CBodyTransDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBodyTransDlg::IDD, pParent)
{
	m_pMiarex = pParent;
	m_XTrans = 0.0;
	m_YTrans = 0.0;
	m_ZTrans = 0.0;
	m_bFrameOnly = false;
	m_FrameID = 0;
	m_pMainFrame = NULL;
}

CBodyTransDlg::~CBodyTransDlg()
{
}

void CBodyTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LENGTH1, m_ctrlLength1);
	DDX_Control(pDX, IDC_LENGTH2, m_ctrlLength2);
	DDX_Control(pDX, IDC_LENGTH3, m_ctrlLength3);
	DDX_Control(pDX, IDC_FRAMEID, m_ctrlFrameID);
	DDX_Control(pDX, IDC_XTRANS, m_ctrlXTransFactor);
	DDX_Control(pDX, IDC_YTRANS, m_ctrlYTransFactor);
	DDX_Control(pDX, IDC_ZTRANS, m_ctrlZTransFactor);
	DDX_Control(pDX, IDOK, m_ctrlOK);
}


BEGIN_MESSAGE_MAP(CBodyTransDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio1)
	ON_EN_KILLFOCUS(IDC_FRAMEID, OnKillFocusFrameId)
END_MESSAGE_MAP()


// Gestionnaires de messages CBodyTransDlg

BOOL CBodyTransDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CMainFrame * pMainFrame = (CMainFrame*)m_pMainFrame;

	CString str;
	GetLengthUnit(str, pMainFrame->m_LengthUnit);
	m_ctrlLength1.SetWindowText(str);
	m_ctrlLength2.SetWindowText(str);
	m_ctrlLength3.SetWindowText(str);

	m_ctrlXTransFactor.SetValue(m_XTrans * pMainFrame->m_mtoUnit);
	m_ctrlYTransFactor.SetValue(m_YTrans * pMainFrame->m_mtoUnit);
	m_ctrlZTransFactor.SetValue(m_ZTrans * pMainFrame->m_mtoUnit);

	m_ctrlXTransFactor.SetFocus();
	m_ctrlYTransFactor.EnableWindow(FALSE);


	CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
	m_ctrlFrameID.EnableWindow(false);
	m_ctrlFrameID.SetValue(m_FrameID+1);

	if(m_bFrameOnly)
	{
		CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);
		GetDlgItem(IDC_RADIO1)->EnableWindow(false);
		GetDlgItem(IDC_RADIO2)->EnableWindow(false);
	}

	return FALSE;
}

BOOL CBodyTransDlg::PreTranslateMessage(MSG* pMsg)
{
	CWnd* pWnd = GetFocus();

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if(pWnd != &m_ctrlOK) 
		{
			m_ctrlOK.SetFocus();
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}



void CBodyTransDlg::OnOK()
{
	CMainFrame *pMainFrame = (CMainFrame*)m_pMainFrame;

	m_FrameID = m_ctrlFrameID.GetValue()-1;
	
	m_XTrans = m_ctrlXTransFactor.GetValue()/ pMainFrame->m_mtoUnit;
	m_YTrans = m_ctrlYTransFactor.GetValue()/ pMainFrame->m_mtoUnit;
	m_ZTrans = m_ctrlZTransFactor.GetValue()/ pMainFrame->m_mtoUnit;
	
	CDialog::OnOK();
}

void CBodyTransDlg::OnRadio1()
{
	if(GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO2)==IDC_RADIO1)
	{
		m_ctrlFrameID.EnableWindow(false);
		m_bFrameOnly = false;
	}
	else
	{
		m_ctrlFrameID.EnableWindow(true);
		m_bFrameOnly = true;
	}
}

void CBodyTransDlg::OnKillFocusFrameId()
{
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	m_FrameID = m_ctrlFrameID.GetValue()-1;
	pMiarex->m_pCurBody->m_iActiveFrame = m_FrameID;
	pMiarex->m_bResetglBody2D = true;
	pMiarex->UpdateView();
}
