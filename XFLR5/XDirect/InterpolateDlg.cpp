/****************************************************************************

	Interpolation Dialog class
    Copyright (C) 2004 André Deperrois XFLR5@yahoo.com

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

// InterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "InterpolateDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CInterpolateDlg dialog


CInterpolateDlg::CInterpolateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInterpolateDlg::IDD, pParent){
	//{{AFX_DATA_INIT(CInterpolateDlg)
	//}}AFX_DATA_INIT
	m_pChildView = NULL;
	m_pXFoil = NULL;
	m_pBufferFoil = NULL;
	m_poaFoil = NULL;
}


void CInterpolateDlg::DoDataExchange(CDataExchange* pDX){
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInterpolateDlg)
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_THICKNESS2, m_ctrlThickness2);
	DDX_Control(pDX, IDC_THICKNESS1, m_ctrlThickness1);
	DDX_Control(pDX, IDC_CAMBER2, m_ctrlCamber2);
	DDX_Control(pDX, IDC_CAMBER1, m_ctrlCamber1);
	DDX_Control(pDX, IDC_THICKNESS0, m_ctrlThickness0);
	DDX_Control(pDX, IDC_CAMBER0, m_ctrlCamber0);
	DDX_Control(pDX, IDC_FRAC, m_ctrlFrac);
	DDX_Control(pDX, IDC_FOIL2, m_ctrlFoil2);
	DDX_Control(pDX, IDC_FOIL1, m_ctrlFoil1);
	DDX_Control(pDX, IDC_MIXT, m_ctrlMixt);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_NEWFOILNAME, m_ctrlNewFoilName);
}


BEGIN_MESSAGE_MAP(CInterpolateDlg, CDialog)
	//{{AFX_MSG_MAP(CInterpolateDlg)
	ON_CBN_SELCHANGE(IDC_FOIL1, OnSelChangeFoil1)
	ON_CBN_SELCHANGE(IDC_FOIL2, OnSelChangeFoil2)
	ON_WM_VSCROLL()
	ON_EN_KILLFOCUS(IDC_FRAC, OnKillFocusFrac)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInterpolateDlg message handlers

BOOL CInterpolateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int i;
	CFoil* pFoil;
	for (i=0; i<m_poaFoil->GetSize(); i++)
	{
		pFoil = (CFoil*)m_poaFoil->GetAt(i);
		m_ctrlFoil1.AddString(pFoil->m_FoilName);
		m_ctrlFoil2.AddString(pFoil->m_FoilName);
	}
	m_ctrlFoil1.SetCurSel(0);
	m_ctrlFoil2.SetCurSel(1);
	m_Frac = 0.0;

	m_ctrlMixt.SetRange(0,100,false);
	m_ctrlMixt.SetPos(0);
	m_ctrlMixt.SetTicFreq(10);
	m_ctrlFrac.SetValue(0);
	OnSelChangeFoil1();
	OnSelChangeFoil2();

	m_ctrlNewFoilName.SetWindowText("New Foil Name");
	m_ctrlNewFoilName.SetSel(0, -1);
	m_ctrlNewFoilName.SetFocus();

	return FALSE;
}

void CInterpolateDlg::OnSelChangeFoil1() 
{
	CString strong="";
	
	int sel = m_ctrlFoil1.GetCurSel();
	if (sel !=CB_ERR) m_ctrlFoil1.GetLBText(sel, strong);

	CFoil* pFoil = GetFoil(strong);
	if(pFoil){
		CString str;	
		str.Format("Camb.=%5.2f", pFoil->m_fCamber*100);
		str += "%";
		strong.Format(" at x=%5.1f", pFoil->m_fXCamber*100);
		strong += "%";
		str+=strong;
		m_ctrlCamber1.SetWindowText(str);

		str.Format("Thick.=%5.2f", pFoil->m_fThickness*100);
		str += "%";
		strong.Format(" at x=%5.1f", pFoil->m_fXThickness*100);
		strong += "%";
		str+=strong;
		m_ctrlThickness1.SetWindowText(str);
	}
	Update();
	Invalidate();
}


void CInterpolateDlg::OnSelChangeFoil2() 
{
	CString strong ="";

	int sel = m_ctrlFoil2.GetCurSel();
	if (sel !=CB_ERR) m_ctrlFoil2.GetLBText(sel, strong);

	CFoil* pFoil = GetFoil(strong);
	if(pFoil){
		CString str;	
		str.Format("Camb.=%5.2f", pFoil->m_fCamber*100);
		str += "%";
		strong.Format(" at x=%5.1f", pFoil->m_fXCamber*100);
		strong += "%";
		str+=strong;
		m_ctrlCamber2.SetWindowText(str);

		str.Format("Thick.=%5.2f", pFoil->m_fThickness*100);
		str += "%";
		strong.Format(" at x=%5.1f", pFoil->m_fXThickness*100);
		strong += "%";
		str+=strong;
		m_ctrlThickness2.SetWindowText(str);
	}
	Update();
	Invalidate();
}


void CInterpolateDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{

	if (pScrollBar->GetSafeHwnd() == GetDlgItem(IDC_MIXT)->GetSafeHwnd())    {
		m_Frac = (float)m_ctrlMixt.GetPos();
		m_ctrlFrac.SetValue(m_Frac);
		Update();
	}
	
//	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CInterpolateDlg::Update()
{
	CString strong ="";

	
	int sel = m_ctrlFoil1.GetCurSel();
	if (sel !=CB_ERR) m_ctrlFoil1.GetLBText(sel, strong);
	CFoil* pFoil1 = GetFoil(strong);
	
	sel = m_ctrlFoil2.GetCurSel();
	if (sel !=CB_ERR) m_ctrlFoil2.GetLBText(sel, strong);
	CFoil* pFoil2 = GetFoil(strong);
	
	if(!pFoil1 || !pFoil2) return;

	m_pXFoil->Interpolate(pFoil1->x, pFoil1->y, pFoil1->n,	
						  pFoil2->x, pFoil2->y, pFoil2->n,
 						  m_Frac/100.f);
	
	for (int j=0; j< pFoil1->n; j++){
		m_pBufferFoil->x[j] = m_pXFoil->xb[j+1];
		m_pBufferFoil->y[j] = m_pXFoil->yb[j+1];
		m_pBufferFoil->xb[j] = m_pXFoil->xb[j+1];
		m_pBufferFoil->yb[j] = m_pXFoil->yb[j+1];
	}
	m_pBufferFoil->n  = pFoil1->n;
	m_pBufferFoil->nb = pFoil1->n;

	m_pBufferFoil->InitFoil();

	CString str;	
	str.Format("Camb.=%5.2f", m_pBufferFoil->m_fCamber*100);
	str += "%";
	strong.Format(" at x=%5.1f", m_pBufferFoil->m_fXCamber*100);
	strong += "%";
	str+=strong;
	m_ctrlCamber0.SetWindowText(str);

	str.Format("Thick.=%5.2f", m_pBufferFoil->m_fThickness*100);
	str += "%";
	strong.Format(" at x=%5.1f", m_pBufferFoil->m_fXThickness*100);
	strong += "%";
	str+=strong;
	m_ctrlThickness0.SetWindowText(str);

	m_pChildView->Invalidate();

}


BOOL CInterpolateDlg::PreTranslateMessage(MSG* pMsg)
{

	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)){
		CWnd* ppp = GetFocus();
		if (ppp == GetDlgItem(IDC_FRAC) )	{
			OnKillFocusFrac();
			m_ctrlOK.SetFocus();
			return true;
		}
		if(ppp != GetDlgItem(IDOK) && ppp != GetDlgItem(IDCANCEL) ){
			m_ctrlOK.SetFocus();
			return TRUE ;
		}

	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CInterpolateDlg::OnOK() 
{
	m_ctrlNewFoilName.GetWindowText(m_NewFoilName);
	CDialog::OnOK();
}



void CInterpolateDlg::OnKillFocusFrac() 
{
	if(m_ctrlFrac.GetValue()>100.0) m_ctrlFrac.SetValue(100.0);
	if(m_ctrlFrac.GetValue()<0.0)   m_ctrlFrac.SetValue(0.0);
	m_Frac = m_ctrlFrac.GetValue();
	m_ctrlMixt.SetPos((int)m_Frac);
	Update();
	
}

CFoil* CInterpolateDlg::GetFoil(CString strong)
{
	//returns a pointer to the foil with the corresponding name
	// returns NULL if not found
	if(!strong.GetLength()) return NULL;

	CFoil* pFoil;
	for (int i=0; i<m_poaFoil->GetSize(); i++){
		pFoil = (CFoil*)m_poaFoil->GetAt(i);
		if (pFoil->m_FoilName == strong) return pFoil;
	}
	return NULL;

}
