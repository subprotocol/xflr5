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

// LECircleDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "AFoil.h"
#include ".\lecircledlg.h"


// Boîte de dialogue CLECircleDlg

IMPLEMENT_DYNAMIC(CLECircleDlg, CDialog)
CLECircleDlg::CLECircleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLECircleDlg::IDD, pParent)
{
	m_Radius      = 1.0;
	m_bShowRadius = false;
	m_pAFoil      = pParent;
}

CLECircleDlg::~CLECircleDlg()
{
}

void CLECircleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LERAD, m_ctrlRadius);
	DDX_Control(pDX, IDC_LERADCHECK, m_ctrlLEShow);
}


BEGIN_MESSAGE_MAP(CLECircleDlg, CDialog)
	ON_BN_CLICKED(IDC_LERADCHECK, OnLERad)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_EN_KILLFOCUS(IDC_LERAD, OnKillFocusRadius)
END_MESSAGE_MAP()


// Gestionnaires de messages CLECircleDlg

BOOL CLECircleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlRadius.SetValue(m_Radius);
	if(m_bShowRadius) m_ctrlLEShow.SetCheck(TRUE);
	else              m_ctrlLEShow.SetCheck(FALSE);
	if(!m_bShowRadius) m_ctrlRadius.EnableWindow(false);
	m_ctrlLEShow.SetFocus();

	return FALSE;
}

void CLECircleDlg::OnLERad()
{
	CAFoil *pAFoil = (CAFoil*)m_pAFoil;
	if (m_ctrlLEShow.GetCheck()) m_bShowRadius=true;
	else                         m_bShowRadius=false;
	if(!m_bShowRadius) m_ctrlRadius.EnableWindow(false);
	else               m_ctrlRadius.EnableWindow(true);
	pAFoil->m_bCircle = m_bShowRadius;
	pAFoil->UpdateView();
}


void CLECircleDlg::OnKillFocusRadius()
{
	CAFoil *pAFoil = (CAFoil*)m_pAFoil;
	pAFoil->m_LERad = m_Radius;
	pAFoil->UpdateView();
}

void CLECircleDlg::OnOK()
{
	m_Radius = m_ctrlRadius.GetValue();
	if (m_ctrlLEShow.GetCheck()) m_bShowRadius=true;
	else                         m_bShowRadius=false;
	CDialog::OnOK();
}

