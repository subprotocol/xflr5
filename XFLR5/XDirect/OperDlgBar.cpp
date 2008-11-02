/****************************************************************************

    COperDlgBar Class
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

// OperDlgBar.cpp: implementation of the COperDlgBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../X-FLR5.h"
#include "OperDlgBar.h"
#include "XDirect.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(COperDlgBar, CInitDialogBar)

COperDlgBar::COperDlgBar(CWnd* pParent /*=NULL*/)
{
	m_IterLim   = 50;

	m_Alpha      = 0.0;
	m_AlphaMax   = 1.0;
	m_DeltaAlpha = 0.5;

	m_Cl         = 0.0;
	m_ClMax      = 0.5;
	m_DeltaCl    = 0.05;

	m_Re         = 100000.0;
	m_ReMax      = 300000.0;
	m_DeltaRe    = 50000.0;

	m_bSequence = false;
	m_pXDirect = NULL;
}

COperDlgBar::~COperDlgBar()
{

}

BEGIN_MESSAGE_MAP(COperDlgBar, CInitDialogBar)
	//{{AFX_MSG_MAP(COperDlgBar)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_ANIMATE, OnAnimate)
	ON_BN_CLICKED(IDC_SEQUENCE, OnSequence)
	ON_BN_CLICKED(IDC_ANALYZE, OnAnalyze)
	ON_BN_CLICKED(IDC_VISCOUS, OnViscous)
	ON_BN_CLICKED(IDC_INITBL, OnInitBL)
	ON_BN_CLICKED(IDC_STOREOPP, OnStoreOpp)
	ON_BN_CLICKED(IDC_SHOWBL, OnShowBL)
	ON_BN_CLICKED(IDC_SHOWPRESSURE, OnShowPressure)
	ON_BN_CLICKED(IDC_PARAM1, OnParam1)
	ON_BN_CLICKED(IDC_PARAM2, OnParam1)
	ON_EN_KILLFOCUS(IDC_ALPHA, OnKillFocus)
	ON_EN_KILLFOCUS(IDC_ALPHAMAX, OnKillFocus)
	ON_EN_KILLFOCUS(IDC_DELTAALPHA, OnKillFocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
 
BOOL COperDlgBar::OnInitDialogBar()
{
	// Support for DDX mechanism
	// If you do not want DDX then
	// do not call base class
	CInitDialogBar::OnInitDialogBar();

	// Update any controls NOT supported by DDX
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;
	SetParams();

	m_SymbolFont.CreatePointFont(100, "Symbol");
	m_ctrlDAlphaStat.SetFont(&m_SymbolFont);
	m_ctrlParam1.SetFont(&m_SymbolFont);

	m_ctrlAlpha.SetPrecision(2);
	m_ctrlAlphaMax.SetPrecision(2);
	m_ctrlDeltaAlpha.SetPrecision(2);

	m_ctrlAlpha.SetMax(90.0);
	m_ctrlAlpha.SetMin(-90.0);

	m_ctrlDeltaAlpha.SetMax(50.0);
	m_ctrlDeltaAlpha.SetMin(-50.0);

	m_ctrlSpeed.SetRange(0,500);
	m_ctrlSpeed.SetPos(250);
	m_ctrlSpeed.SetTicFreq(50);

	return TRUE;
}

void COperDlgBar::DoDataExchange(CDataExchange* pDX)
{
	ASSERT(pDX);

	CInitDialogBar::DoDataExchange(pDX);

	// DDX_??? functions to associate control with
	// data or control object
	// Call UpdateData(TRUE) to get data at any time
	// Call UpdateData(FALSE) to set data at any time

	//{{AFX_DATA_MAP(CInitDialogBar)
	DDX_Control(pDX, IDC_ANALYZE, m_ctrlAnalyze);
	DDX_Control(pDX, IDC_ANIMATE, m_ctrlAnimate);
	DDX_Control(pDX, IDC_SHOWBL, m_ctrlBL);
	DDX_Control(pDX, IDC_SHOWPRESSURE, m_ctrlPressure);
	DDX_Control(pDX, IDC_SPEED, m_ctrlSpeed);
	DDX_Control(pDX, IDC_INITBL, m_ctrlInitBL);
	DDX_Control(pDX, IDC_ALPHASTAT, m_ctrlAlphaStat);
	DDX_Control(pDX, IDC_AMAXSTAT, m_ctrlAMaxStat);
	DDX_Control(pDX, IDC_DALPHASTAT, m_ctrlDAlphaStat);
	DDX_Control(pDX, IDC_VISCOUS, m_ctrlViscous);
	DDX_Control(pDX, IDC_SEQUENCE, m_ctrlSequence);
	DDX_Control(pDX, IDC_DELTAALPHA, m_ctrlDeltaAlpha);
	DDX_Control(pDX, IDC_ALPHAMAX, m_ctrlAlphaMax);
	DDX_Control(pDX, IDC_ALPHA, m_ctrlAlpha);
	DDX_Control(pDX, IDC_STOREOPP, m_ctrlStoreOpp);
	DDX_Control(pDX, IDC_PARAM1, m_ctrlParam1);
	DDX_Control(pDX, IDC_PARAM2, m_ctrlParam2);
	DDX_Control(pDX, IDC_PARAM3, m_ctrlParam3);
	//}}AFX_DATA_MAP
} 


void COperDlgBar::OnSequence() 
{
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;
	if(m_ctrlSequence.GetCheck() && m_ctrlSequence.IsWindowEnabled()){
		m_bSequence = true;
	}
	else{
		m_bSequence = false;
	}
	SetOpPointSequence();
}


void COperDlgBar::SetOpPointSequence()
{
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;
	if(m_bSequence) m_ctrlSequence.SetCheck(1); else m_ctrlSequence.SetCheck(0);

	if(pXDirect->m_pCurPolar->m_Type != 4)
	{
		if(GetCheckedRadioButton(IDC_PARAM1, IDC_PARAM3) == IDC_PARAM3)
		{
			CheckRadioButton(IDC_PARAM1, IDC_PARAM3, IDC_PARAM1);
			pXDirect->m_bAlpha = true;
		}
		if(GetCheckedRadioButton(IDC_PARAM1, IDC_PARAM3) == IDC_PARAM1)
		{
			m_ctrlAlpha.SetPrecision(2);
			m_ctrlAlphaMax.SetPrecision(2);
			m_ctrlDeltaAlpha.SetPrecision(2);
			m_ctrlAlpha.SetValue(m_Alpha);
			m_ctrlAlphaMax.SetValue(m_AlphaMax);
			m_ctrlDeltaAlpha.SetValue(m_DeltaAlpha);
			m_ctrlAlpha.SetMax(90.0);
			m_ctrlAlpha.SetMin(-90.0);
			m_ctrlDeltaAlpha.SetMax(50.0);
			m_ctrlDeltaAlpha.SetMin(-50.0);
		}
		else if(GetCheckedRadioButton(IDC_PARAM1, IDC_PARAM3) == IDC_PARAM2)
		{
			m_ctrlAlpha.SetPrecision(2);
			m_ctrlAlphaMax.SetPrecision(2);
			m_ctrlDeltaAlpha.SetPrecision(2);
			m_ctrlAlpha.SetValue(m_Cl);
			m_ctrlAlphaMax.SetValue(m_ClMax);
			m_ctrlDeltaAlpha.SetValue(m_DeltaCl);
			m_ctrlAlpha.SetMax(5.0);
			m_ctrlAlpha.SetMin(-5.0);
			m_ctrlDeltaAlpha.SetMax(2.0);
			m_ctrlDeltaAlpha.SetMin(-2.0);
		}
	}
	else
	{
		CheckRadioButton(IDC_PARAM1, IDC_PARAM3, IDC_PARAM3);
		pXDirect->m_bAlpha = true;// no choice with type 4 polars
		m_ctrlAlpha.SetPrecision(0);
		m_ctrlAlphaMax.SetPrecision(0);
		m_ctrlDeltaAlpha.SetPrecision(0);
		m_ctrlAlpha.SetValue(m_Re);
		m_ctrlAlphaMax.SetValue(m_ReMax);
		m_ctrlDeltaAlpha.SetValue(m_DeltaRe);
		m_ctrlAlpha.SetMax(1e12);
		m_ctrlAlpha.SetMin(0.0);
		m_ctrlDeltaAlpha.SetMax(1e11);
		m_ctrlDeltaAlpha.SetMin(0.0);
	}
}


void COperDlgBar::OnAnalyze() 
{
	ReadParams();
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;
	pXDirect->OnAnalyze();
}


void COperDlgBar::OnInitBL() 
{
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;
	if (m_ctrlInitBL.GetCheck()) {
		m_pXFoil->lblini = false;
		m_pXFoil->lipan  = false;
	}
	else {
		m_pXFoil->lblini = true;
	}
}


void COperDlgBar::OnViscous() 
{
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;
	if (m_ctrlViscous.GetCheck()) {
		pXDirect->m_bViscous = true;
	}
	else {
		pXDirect->m_bViscous = false;
	}
}


void COperDlgBar::SetPolar()
{
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;
	CPolar *pPolar = pXDirect->m_pCurPolar;
	if(!pPolar) {
		EnableDispCtrls(false);
		EnableXFoilCtrls(false);
		return;
	}
	
	SetOpPointSequence();
	 
	EnableXFoilCtrls(true);
	Invalidate();
}


void COperDlgBar::OnAnimate() 
{
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;
	if(m_ctrlAnimate.GetCheck()) {
		if(pXDirect->m_bPolar) pXDirect->OnOper();
		pXDirect->Animate(true);
	}
	else pXDirect->Animate(false);
}


void COperDlgBar::EnableXFoilCtrls(bool show)
{
	m_ctrlAnalyze.EnableWindow(show);
	m_ctrlInitBL.EnableWindow(show);
}


void COperDlgBar::EnableDispCtrls(bool show)
{
	m_ctrlAnimate.EnableWindow(show);
	m_ctrlBL.EnableWindow(show);
	m_ctrlPressure.EnableWindow(show);
	m_ctrlSpeed.EnableWindow(show);
}


void COperDlgBar::OnShowBL()
{
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;

	if(m_ctrlBL.GetCheck()){
		if(pXDirect->m_bPolar) pXDirect->OnOper();
		m_ctrlPressure.SetCheck(0);
		pXDirect->m_bBL = true;
		pXDirect->m_bPressure = false;
	}
	else{
		pXDirect->m_bBL = false;
	}
	if(!m_ctrlAnimate.GetCheck()) 
		pXDirect->UpdateView();
}


void COperDlgBar::OnShowPressure() 
{
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;
	if(m_ctrlPressure.GetCheck()){
		if(pXDirect->m_bPolar) pXDirect->OnOper();
		m_ctrlBL.SetCheck(0);
		pXDirect->m_bBL = false;
		pXDirect->m_bPressure = true;
	}
	else{
		pXDirect->m_bPressure = false;
	}
	if(!m_ctrlAnimate.GetCheck()) 
		pXDirect->UpdateView();
}


void COperDlgBar::OnStoreOpp()
{
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;
	if(m_ctrlStoreOpp.GetCheck()) 
		pXDirect->m_bStoreOpp = true;
	else 
		pXDirect->m_bStoreOpp = false;
}


void COperDlgBar::OnParam1() 
{
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;
	if(GetCheckedRadioButton(IDC_PARAM1, IDC_PARAM2) == IDC_PARAM1){
		pXDirect->m_bAlpha = true;
	}
	else{
		pXDirect->m_bAlpha = false;
	}
	SetOpPointSequence();
}


void COperDlgBar::SetParams()
{
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;

	if(pXDirect->m_bViscous)	m_ctrlViscous.SetCheck(1);  else m_ctrlViscous.SetCheck(0); 
	if(pXDirect->m_bInitBL)	m_ctrlInitBL.SetCheck(1);   else m_ctrlInitBL.SetCheck(0); 
	if(pXDirect->m_bStoreOpp)	m_ctrlStoreOpp.SetCheck(1); else m_ctrlStoreOpp.SetCheck(0); 
	if(pXDirect->m_bPressure)	m_ctrlPressure.SetCheck(1); else m_ctrlPressure.SetCheck(0); 
	if(pXDirect->m_bBL)	    m_ctrlBL.SetCheck(1);       else m_ctrlBL.SetCheck(0); 
	if(pXDirect->m_bStoreOpp)	m_ctrlStoreOpp.SetCheck(1); else m_ctrlStoreOpp.SetCheck(0); 
	if(m_bSequence)         m_ctrlSequence.SetCheck(1); else m_ctrlSequence.SetCheck(0);

	m_ctrlAlpha.SetValue(m_Alpha);
	m_ctrlAlphaMax.SetValue(m_AlphaMax);
	m_ctrlDeltaAlpha.SetValue(m_DeltaAlpha);

	if(pXDirect->m_pCurPolar){
		if(pXDirect->m_pCurPolar->m_Type != 4){
			if(pXDirect->m_bAlpha) CheckRadioButton(IDC_PARAM1,IDC_PARAM3,IDC_PARAM1);
			else                   CheckRadioButton(IDC_PARAM1,IDC_PARAM3,IDC_PARAM2);
		}
		else CheckRadioButton(IDC_PARAM1,IDC_PARAM3,IDC_PARAM3);
	}
	else {
		if(pXDirect->m_bAlpha) CheckRadioButton(IDC_PARAM1,IDC_PARAM3,IDC_PARAM1);
		else                   CheckRadioButton(IDC_PARAM1,IDC_PARAM3,IDC_PARAM2);
	}
}


void COperDlgBar::ReadParams()
{
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;
	if(!pXDirect->m_pCurPolar) return;
	if(pXDirect->m_pCurPolar->m_Type !=4)
	{
		if(pXDirect->m_bAlpha)
		{
			m_Alpha      = m_ctrlAlpha.GetValue();
			m_AlphaMax   = m_ctrlAlphaMax.GetValue();
			m_DeltaAlpha = m_ctrlDeltaAlpha.GetValue();
		}
		else
		{
			m_Cl      = m_ctrlAlpha.GetValue();
			m_ClMax   = m_ctrlAlphaMax.GetValue();
			m_DeltaCl = m_ctrlDeltaAlpha.GetValue();
		}
	}
	else
	{
		m_Re      = m_ctrlAlpha.GetValue();
		m_ReMax   = m_ctrlAlphaMax.GetValue();
		m_DeltaRe = m_ctrlDeltaAlpha.GetValue();
	}
}


void COperDlgBar::OnKillFocus()
{
	ReadParams();
}

void COperDlgBar::SetAnimate(bool bAnimate)
{
	CXDirect* pXDirect = (CXDirect*)m_pXDirect;
	pXDirect->m_bAnimate = bAnimate;

	if(bAnimate) m_ctrlAnimate.SetCheck(TRUE);
	else  m_ctrlAnimate.SetCheck(FALSE);
}



BOOL COperDlgBar::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN){ 
		if(GetDlgItem(IDC_ANALYZE) != GetFocus())	m_ctrlAnalyze.SetFocus();
		else OnAnalyze();
		return true;
	}	
	else if(pMsg->message == WM_MOUSEWHEEL){
		CXDirect* pXDirect = (CXDirect*)m_pXDirect;
		CPoint pt((short) LOWORD(pMsg->lParam),(short) HIWORD(pMsg->lParam));

		pXDirect->OnMouseWheel(LOWORD(pMsg->wParam),(short) HIWORD(pMsg->wParam), pt);
		pXDirect->m_pChildWnd->SetFocus();
		return true;
	}
	return CInitDialogBar::PreTranslateMessage(pMsg);
}


void COperDlgBar::OnShowWindow(BOOL bShow, UINT nStatus){
	if(!bShow) return;
	if(m_pXFoil->lblini)	m_ctrlInitBL.SetCheck(false);
	else					m_ctrlInitBL.SetCheck(true);
}

