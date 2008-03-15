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
#include "BodyScaleDlg.h"
#include ".\bodyscaledlg.h"
#include "Miarex.h"

// Boîte de dialogue CBodyScaleDlg

IMPLEMENT_DYNAMIC(CBodyScaleDlg, CDialog)
CBodyScaleDlg::CBodyScaleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBodyScaleDlg::IDD, pParent)
{
	m_pMiarex = pParent;
	m_XFactor = 1.0;
	m_YFactor = 1.0;
	m_ZFactor = 1.0;
	m_bFrameOnly = false;
	m_FrameID = 0;
}

CBodyScaleDlg::~CBodyScaleDlg()
{
}

void CBodyScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_FRAMEID, m_ctrlFrameID);
	DDX_Control(pDX, IDC_XFACTOR, m_ctrlXScaleFactor);
	DDX_Control(pDX, IDC_YFACTOR, m_ctrlYScaleFactor);
	DDX_Control(pDX, IDC_ZFACTOR, m_ctrlZScaleFactor);
	DDX_Control(pDX, IDOK, m_ctrlOK);
}


BEGIN_MESSAGE_MAP(CBodyScaleDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio1)
	ON_EN_KILLFOCUS(IDC_FRAMEID, OnKillFocusFrameId)
END_MESSAGE_MAP()


// Gestionnaires de messages CBodyScaleDlg

BOOL CBodyScaleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlXScaleFactor.SetValue(m_XFactor);
	m_ctrlYScaleFactor.SetValue(m_XFactor);
	m_ctrlZScaleFactor.SetValue(m_XFactor);
	m_ctrlXScaleFactor.SetFocus();

	CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
	m_ctrlFrameID.EnableWindow(false);
	m_ctrlFrameID.SetValue(m_FrameID+1);

	if(m_bFrameOnly)
	{
		CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);
		GetDlgItem(IDC_RADIO1)->EnableWindow(false);
		GetDlgItem(IDC_RADIO2)->EnableWindow(false);
		m_ctrlXScaleFactor.EnableWindow(false);
	}

	return FALSE;
}

BOOL CBodyScaleDlg::PreTranslateMessage(MSG* pMsg)
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



void CBodyScaleDlg::OnOK()
{
	m_FrameID = m_ctrlFrameID.GetValue()-1;
	m_XFactor = m_ctrlXScaleFactor.GetValue();
	m_YFactor = m_ctrlYScaleFactor.GetValue();
	m_ZFactor = m_ctrlZScaleFactor.GetValue();
	CDialog::OnOK();
}

void CBodyScaleDlg::OnRadio1()
{
	if(GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO2)==IDC_RADIO1)
	{
		m_ctrlFrameID.EnableWindow(false);
		m_ctrlXScaleFactor.EnableWindow(true);
		m_bFrameOnly = false;
	}
	else
	{
		m_ctrlFrameID.EnableWindow(true);
		m_ctrlXScaleFactor.EnableWindow(false);
		m_bFrameOnly = true;
	}
}

void CBodyScaleDlg::OnKillFocusFrameId()
{
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	m_FrameID = m_ctrlFrameID.GetValue()-1;
	pMiarex->m_pCurBody->m_iActiveFrame = m_FrameID;
	pMiarex->m_bResetglBody2D = true;
	pMiarex->UpdateView();
}
