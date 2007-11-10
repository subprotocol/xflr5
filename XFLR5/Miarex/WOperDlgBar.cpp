/****************************************************************************

    CWOperDlgBar Class
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


////////////////////////////////////////////////////////////////////////////
//
// MainDialogBar.cpp: implementation of the CWOperDlgBar class.
// The class which will collect input of the analysis parameters for LLT and VLM
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"

#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "./WOperDlgBar.h"
#include "Miarex.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CWOperDlgBar, CInitDialogBar)

BEGIN_MESSAGE_MAP(CWOperDlgBar, CInitDialogBar)
//{{AFX_MSG_MAP(CWOperDlgBar)
	ON_BN_CLICKED(IDC_WANALYSIS, OnAnalyze)
	ON_BN_CLICKED(IDC_STOREWOPP, OnStoreWOpp)
	ON_BN_CLICKED(IDC_INITCALC, OnInitCalc)
	ON_BN_CLICKED(IDC_WSEQUENCE, OnWSequence)
	ON_BN_CLICKED(IDC_ANIMATE, OnAnimate)
	ON_BN_CLICKED(IDC_WSEQUENCE, OnWSequence)
	ON_BN_CLICKED(IDC_HALFWING, OnHalfWing)
	ON_BN_CLICKED(IDC_SHOWLIFT, OnShowLift)
	ON_BN_CLICKED(IDC_SHOWMOMENTS, OnShowMoments)
	ON_BN_CLICKED(IDC_SHOWICD, OnShowICd)
	ON_BN_CLICKED(IDC_SHOWVCD, OnShowVCd)
	ON_BN_CLICKED(IDC_TOPTRANS, OnShowTrans)
	ON_BN_CLICKED(IDC_BOTTRANS, OnShowTrans)
	ON_BN_CLICKED(IDC_DOWNWASH, OnShowDownwash)
	ON_BN_CLICKED(IDC_CL, OnShowCl)
	ON_BN_CLICKED(IDC_STREAM, OnStream)
	ON_EN_KILLFOCUS(IDC_AMIN, ReadParams)
	ON_EN_KILLFOCUS(IDC_AMAX, ReadParams)
	ON_EN_KILLFOCUS(IDC_DALPHA, ReadParams)

//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CWOperDlgBar::CWOperDlgBar(CWnd* pParent)
{
	//Set Initial conditions for controls
	m_pMiarex = NULL;
	m_pFrame = pParent;

	m_Alpha0       =  0.0;
	m_AlphaMax     =  1.0;
	m_DeltaAlpha   =  0.5;
	m_QInf0        = 10.0;
	m_QInfMax      = 15.0;
	m_DeltaQInf    =  5.0;

	m_bSequence  = false;
	m_bInitCalc = true;

	m_Type = 1;
}


CWOperDlgBar::~CWOperDlgBar()
{

}


BOOL CWOperDlgBar::OnInitDialogBar()
{
	// Support for DDX mechanism
	// If you do not want DDX then
	// do not call base class
	CInitDialogBar::OnInitDialogBar();

	// Update any controls NOT supported by DDX
	// CBitmapButton is one
	
	m_ctrlAlpha.SetPrecision(2);
	m_ctrlAlphaMax.SetPrecision(2);
	m_ctrlDeltaAlpha.SetPrecision(2);

	if(m_Type!=4){
		m_ctrlAlpha.SetValue(m_Alpha0);
		m_ctrlAlphaMax.SetValue(m_AlphaMax);
		m_ctrlDeltaAlpha.SetValue(m_DeltaAlpha);
	}
	else {
		m_ctrlAlpha.SetValue(m_QInf0);
		m_ctrlAlphaMax.SetValue(m_QInfMax);
		m_ctrlDeltaAlpha.SetValue(m_DeltaQInf);
	}
 
	m_ctrlSequence.SetCheck(m_bSequence);
	
	m_ctrlStoreWOpp.SetCheck(true);
//	m_ctrlKeepOutOpps.SetCheck(false);

	m_SymbolFont.CreatePointFont(100, "Symbol");
	m_ctrlDeltaStat.SetFont(&m_SymbolFont);
	
	CFont *pFont = GetFont();
	LOGFONT lgft;
	pFont->GetLogFont(&lgft);
	m_StdFont.CreateFontIndirect(&lgft);

	m_ctrlSpeed.SetRange(0,500);
	m_ctrlSpeed.SetPos(250);
	m_ctrlSpeed.SetTicFreq(50);

	EnableAnalysis(false);

	m_ctrlInitCalc.SetCheck(1);
	m_bInitCalc = true;

	return TRUE;
}

void CWOperDlgBar::DoDataExchange(CDataExchange* pDX)
{
	ASSERT(pDX);
	CInitDialogBar::DoDataExchange(pDX);
	// DDX_??? functions to associate control with
	// data or control object
	// Call UpdateData(TRUE) to get data at any time
	// Call UpdateData(FALSE) to set data at any time

	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_INITCALC, m_ctrlInitCalc);
	DDX_Control(pDX, IDC_STOREWOPP, m_ctrlStoreWOpp);
	DDX_Control(pDX, IDC_INITCALC, m_ctrlInitCalc);
	DDX_Control(pDX, IDC_WSEQUENCE, m_ctrlSequence);
	DDX_Control(pDX, IDC_WANALYSIS, m_ctrlAnalyze);
	DDX_Control(pDX, IDC_SPEC, m_ctrlSpec);
	DDX_Control(pDX, IDC_DASTAT, m_ctrlDeltaStat);
	DDX_Control(pDX, IDC_DALPHA, m_ctrlDeltaAlpha);
	DDX_Control(pDX, IDC_AMAX, m_ctrlAlphaMax);
	DDX_Control(pDX, IDC_AMIN, m_ctrlAlpha);
	DDX_Control(pDX, IDC_ANIMATE, m_ctrlAnimate);
	DDX_Control(pDX, IDC_SPEED, m_ctrlSpeed);
	DDX_Control(pDX, IDC_SHOWLIFT, m_ctrlLift);
	DDX_Control(pDX, IDC_SHOWMOMENTS, m_ctrlMoments);
	DDX_Control(pDX, IDC_SHOWICD, m_ctrlICd);
	DDX_Control(pDX, IDC_SHOWVCD, m_ctrlVCd);
	DDX_Control(pDX, IDC_TOPTRANS, m_ctrlTopTrans);
	DDX_Control(pDX, IDC_BOTTRANS, m_ctrlBotTrans);
	DDX_Control(pDX, IDC_DOWNWASH, m_ctrlDownwash);
	DDX_Control(pDX, IDC_HALFWING, m_ctrlHalfWing);
	DDX_Control(pDX, IDC_CL, m_ctrlCl);
	DDX_Control(pDX, IDC_STREAM, m_ctrlStream);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CWOperDlgBar message handlers

void CWOperDlgBar::OnWSequence() 
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
//	pMiarex->Animate(false);
	if(m_ctrlSequence.GetCheck()) m_bSequence = true;
	else						  m_bSequence = false;
}

void CWOperDlgBar::OnHalfWing() 
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	pMiarex->m_bIs2DScaleSet = false;
	if(m_ctrlHalfWing.GetCheck()) {
		pMiarex->m_bHalfWing = true;

	}
	else {
		pMiarex->m_bHalfWing = false;
	}

	pMiarex->SetWGraphScale();
	pMiarex->Set2DScale();
	pMiarex->OnAdjustToWing();
}

void CWOperDlgBar::OnAnalyze() 
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	ReadParams();
	if(m_Type !=4)
		pMiarex->Analyze(m_Alpha0, m_AlphaMax, m_DeltaAlpha, m_bSequence, m_bInitCalc);
	else
		pMiarex->Analyze(m_QInf0, m_QInfMax, m_DeltaQInf, m_bSequence, m_bInitCalc);
}


void CWOperDlgBar::ReadParams()
{
//	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	if(m_ctrlSequence.GetCheck()) m_bSequence = true;
	else m_bSequence = false;
	if(m_ctrlInitCalc.GetCheck()) m_bInitCalc = true;
	else m_bInitCalc = false;

	if(m_Type !=4){
		m_Alpha0     = m_ctrlAlpha.GetValue();
		m_AlphaMax   = m_ctrlAlphaMax.GetValue();
		m_DeltaAlpha = fabs(m_ctrlDeltaAlpha.GetValue());
		if(fabs(m_DeltaAlpha)<0.1) {
			m_DeltaAlpha = 0.1;
			m_ctrlDeltaAlpha.SetValue(0.1);
		}
	}
	else{
		m_QInf0     = m_ctrlAlpha.GetValue()/pFrame->m_mstoUnit;
		m_QInfMax   = m_ctrlAlphaMax.GetValue()/pFrame->m_mstoUnit;
		m_DeltaQInf = abs(m_ctrlDeltaAlpha.GetValue())/pFrame->m_mstoUnit;
		if(fabs(m_DeltaQInf)<0.1) {
			m_DeltaQInf = 1.0;
			m_ctrlDeltaAlpha.SetValue(1.0);
		}
	}
}

BOOL CWOperDlgBar::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN){ 
		ReadParams();
		if(GetDlgItem(IDC_WANALYSIS) != GetFocus())	m_ctrlAnalyze.SetFocus();
		else OnAnalyze();
		return true;
	}	
	else if(pMsg->message == WM_MOUSEWHEEL){
		CMiarex* pMiarex = (CMiarex*)m_pMiarex;
		CPoint pt((short) LOWORD(pMsg->lParam),(short) HIWORD(pMsg->lParam));

		pMiarex->OnMouseWheel(LOWORD(pMsg->wParam),(short) HIWORD(pMsg->wParam), pt);
		pMiarex->m_pChildWnd->SetFocus();
		return true;
	}
	return CInitDialogBar::PreTranslateMessage(pMsg);
}

void CWOperDlgBar::EnableAnalysis(bool bEnable)
{
	m_ctrlSequence.EnableWindow(bEnable);
	m_ctrlAnalyze.EnableWindow(bEnable);
	m_ctrlAlpha.EnableWindow(bEnable);
	m_ctrlAlphaMax.EnableWindow(bEnable);
	m_ctrlDeltaAlpha.EnableWindow(bEnable);
}


void CWOperDlgBar::OnStoreWOpp() 
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(m_ctrlStoreWOpp.GetCheck()){
		pMiarex->m_bStoreWOpp = true;
	}
	else pMiarex->m_bStoreWOpp = false;
}

void CWOperDlgBar::OnInitCalc() 
{
	if(m_ctrlInitCalc.GetCheck()){
		m_bInitCalc = true;
	}
	else m_bInitCalc = false;
}

void CWOperDlgBar::OnAnimate() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(m_ctrlAnimate.GetCheck()) {
		if(pMiarex->m_iView==2) pMiarex->OnWOpp();
		pFrame->m_bStream = false;
		m_ctrlStream.SetCheck(false);
		pMiarex->Animate(true);
	}
	else pMiarex->Animate(false);
}


void CWOperDlgBar::OnShowLift() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(m_ctrlLift.GetCheck()){
		pFrame->m_bXCP = true;
	}
	else pFrame->m_bXCP = false;
//	pMiarex->m_bResetglLift = true;
	if(!pMiarex->m_bAnimate) pMiarex->UpdateView();
}
void CWOperDlgBar::OnShowMoments() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(m_ctrlMoments.GetCheck()){
		pFrame->m_bMoments = true;
	}
	else pFrame->m_bMoments = false;
//	pMiarex->m_bResetglLift = true;
	if(!pMiarex->m_bAnimate) pMiarex->UpdateView();
}
void CWOperDlgBar::OnShowICd() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(m_ctrlICd.GetCheck()){
		pFrame->m_bICd = true;
	}
	else pFrame->m_bICd = false;
	pMiarex->m_bResetglDrag = true;
	if(!pMiarex->m_bAnimate) pMiarex->UpdateView();
}


void CWOperDlgBar::OnShowVCd() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(m_ctrlVCd.GetCheck()){
		pFrame->m_bVCd = true;
	}
	else pFrame->m_bVCd = false;
	pMiarex->m_bResetglDrag = true;
	if(!pMiarex->m_bAnimate) pMiarex->UpdateView();
}

void CWOperDlgBar::OnShowTrans() 
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(m_ctrlTopTrans.GetCheck())	pFrame->m_bXTop = true;
	else pFrame->m_bXTop = false;
	if(m_ctrlBotTrans.GetCheck())	pFrame->m_bXBot = true;
	else pFrame->m_bXBot = false;
//	pMiarex->m_bResetglOpp = true;
	if(!pMiarex->m_bAnimate) pMiarex->UpdateView();
}

void CWOperDlgBar::OnShowDownwash() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(m_ctrlDownwash.GetCheck())	pFrame->m_b3DDownwash = true;
	else							pFrame->m_b3DDownwash = false;
//	pMiarex->m_bResetglDownwash = true;
	if(!pMiarex->m_bAnimate) pMiarex->UpdateView();
}

void CWOperDlgBar::OnShowCl() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(m_ctrlCl.GetCheck())	{
		if(!pFrame->m_b3DVLMCl){
			pFrame->m_b3DVLMCl = true;
		}
		pFrame->m_W3DBar.m_bSurfaces = false;
	}
	else pFrame->m_b3DVLMCl = false;
//	pMiarex->m_bResetglOpp = true;
	if(!pMiarex->m_bAnimate) pMiarex->UpdateView();
}


void CWOperDlgBar::SetParams(CWPolar *pWPolar)
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;

	if(pFrame->m_bXCP)			m_ctrlLift.SetCheck(true);
	else						m_ctrlLift.SetCheck(false);
	if(pFrame->m_bMoments)		m_ctrlMoments.SetCheck(true);
	else						m_ctrlMoments.SetCheck(false);
	if(pFrame->m_bStream)		m_ctrlStream.SetCheck(true);
	else						m_ctrlStream.SetCheck(false);
	if(pFrame->m_bICd)			m_ctrlICd.SetCheck(true);
	else						m_ctrlICd.SetCheck(false);
	if(pFrame->m_bVCd)			m_ctrlVCd.SetCheck(true);
	else						m_ctrlVCd.SetCheck(false);
	if(pFrame->m_bXTop)			m_ctrlTopTrans.SetCheck(true);
	else						m_ctrlTopTrans.SetCheck(false);
	if(pFrame->m_bXBot)			m_ctrlBotTrans.SetCheck(true);
	else						m_ctrlBotTrans.SetCheck(false);
	if(pFrame->m_b3DDownwash)	m_ctrlDownwash.SetCheck(true);
	else						m_ctrlDownwash.SetCheck(false);
	if(pMiarex->m_bHalfWing)	m_ctrlHalfWing.SetCheck(true);
	else						m_ctrlHalfWing.SetCheck(false);

//	if (!pWPolar) return;
//	if(pWPolar->m_Type == m_Type) return;
	if(pWPolar) m_Type = pWPolar->m_Type;

	if(m_bSequence)             m_ctrlSequence.SetCheck(1);    else m_ctrlSequence.SetCheck(0);
	if(m_bInitCalc)             m_ctrlInitCalc.SetCheck(1);    else m_ctrlInitCalc.SetCheck(0);
	if(pMiarex->m_bStoreWOpp)   m_ctrlStoreWOpp.SetCheck(1);   else m_ctrlStoreWOpp.SetCheck(0);

	if(pWPolar  && !pWPolar->m_bPolar){//we have a control polar
		m_ctrlSpec.SetWindowText("Ctrl");
		m_ctrlSpec.SetFont(&m_StdFont);
		m_ctrlAlpha.SetValue(0.0);
		m_ctrlAlphaMax.SetValue(1.0);
		m_ctrlDeltaAlpha.SetValue(0.1);
	}
	else if (!pWPolar || (pWPolar && pWPolar->m_Type !=4)){
		m_ctrlSpec.SetWindowText("a");
		m_ctrlSpec.SetFont(&m_SymbolFont);
		m_ctrlAlpha.SetValue(m_Alpha0);
		m_ctrlAlphaMax.SetValue(m_AlphaMax);
		m_ctrlDeltaAlpha.SetValue(m_DeltaAlpha);
	}
	else if(pWPolar  && pWPolar->m_Type ==4){
		m_ctrlSpec.SetWindowText("VInf");
		m_ctrlSpec.SetFont(&m_StdFont);
		m_ctrlAlpha.SetValue(m_QInf0*pFrame->m_mstoUnit);
		m_ctrlAlphaMax.SetValue(m_QInfMax*pFrame->m_mstoUnit);
		m_ctrlDeltaAlpha.SetValue(m_DeltaQInf*pFrame->m_mstoUnit);
	}
}


void CWOperDlgBar::OnStream() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	if(m_ctrlStream.GetCheck()){
		if(!pMiarex->m_bAnimate){
			pFrame->m_bStream = true;
//			pFrame->m_bFlow = true;
			pMiarex->UpdateView();
		}
		else m_ctrlStream.SetCheck(FALSE);
	}
	else {
		pFrame->m_bStream = false;
//		pFrame->m_bFlow = false;
		pMiarex->m_FlowLinesDlg.ShowWindow(SW_HIDE);
		pMiarex->UpdateView();
	}
}


