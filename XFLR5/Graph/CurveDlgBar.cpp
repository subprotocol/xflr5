/****************************************************************************

    PolarDlgBar Class
    Copyright (C) 2003 André Deperrois XFLR5@yahoo.com

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

//////////////////////////////////////////////////////////////////////
// PolarDlgBar.cpp: implementation of the CCurveDlgBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../XDirect/XDirect.h"
#include "../Miarex/Miarex.h"
#include "../main/MainFrm.h"
#include "CurveDlgBar.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CCurveDlgBar, CInitDialogBar)

BEGIN_MESSAGE_MAP(CCurveDlgBar, CInitDialogBar)
//{{AFX_MSG_MAP(CCurveDlgBar)
	ON_BN_CLICKED(IDC_SHOW, OnShow)
	ON_BN_CLICKED(IDC_SHOWPOINTS, OnShowPoints)
	ON_BN_CLICKED(IDC_COLORBTN, OnColorBtn)
	ON_CBN_SELCHANGE(IDC_STYLE, OnSelChangeStyle)
	ON_CBN_SELCHANGE(IDC_WIDTH, OnSelChangeWidth)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CCurveDlgBar::CCurveDlgBar(CWnd *pParent)
{
	m_pParent = pParent;
	m_pFrame = NULL;
}

CCurveDlgBar::~CCurveDlgBar()
{

}

BOOL CCurveDlgBar::OnInitDialogBar()
{
	// Support for DDX mechanism
	// If you do not want DDX then
	// do not call base class
	CInitDialogBar::OnInitDialogBar();

	// Update any controls NOT supported by DDX

//	m_ctrlShow.EnableWindow(false);
//	m_ctrlShowPoints.EnableWindow(false);

	m_ctrlStyle.ResetContent();
	m_ctrlStyle.AddLine();
	m_ctrlStyle.AddLine();
	m_ctrlStyle.AddLine();
	m_ctrlStyle.AddLine();
	m_ctrlStyle.AddLine();

	m_ctrlWidth.ResetContent();
	m_ctrlWidth.AddLine();
	m_ctrlWidth.AddLine();
	m_ctrlWidth.AddLine();
	m_ctrlWidth.AddLine();
	m_ctrlWidth.AddLine();
	m_ctrlWidth.m_bWidth = true;

	return TRUE;
}

void CCurveDlgBar::DoDataExchange(CDataExchange* pDX)
{
	ASSERT(pDX);

	CInitDialogBar::DoDataExchange(pDX);

	// DDX_??? functions to associate control with
	// data or control object
	// Call UpdateData(TRUE) to get data at any time
	// Call UpdateData(FALSE) to set data at any time


	//{{AFX_DATA_MAP(CCurveDlgBar)
	DDX_Control(pDX, IDC_SHOW, m_ctrlShow);
	DDX_Control(pDX, IDC_WIDTH, m_ctrlWidth);
	DDX_Control(pDX, IDC_STYLE, m_ctrlStyle);
	DDX_Control(pDX, IDC_COLORBTN, m_ctrlColor);
	DDX_Control(pDX, IDC_SHOWPOINTS, m_ctrlShowPoints);
	//}}AFX_DATA_MAP
}

void CCurveDlgBar::OnShow() 
{
	//user has toggled visible switch
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(m_idApp == XFOILANALYSIS){
		CXDirect* pXDirect = (CXDirect*)m_pParent;
		if(pXDirect->m_bPolar){
			if (pXDirect->m_pCurPolar){ 
				if (m_ctrlShow.GetCheck())       pXDirect->m_pCurPolar->m_bIsVisible = true;
				else                             pXDirect->m_pCurPolar->m_bIsVisible = false;
				if (m_ctrlShowPoints.GetCheck()) pXDirect->m_pCurPolar->m_bShowPoints = true;
				else                             pXDirect->m_pCurPolar->m_bShowPoints = false;
			}
			pXDirect->CreatePolarCurves(); 
		}
		else if (pXDirect->m_pCurOpp){
			if (m_ctrlShow.GetCheck())       pXDirect->m_pCurOpp->m_bIsVisible = true;
			else                             pXDirect->m_pCurOpp->m_bIsVisible = false;
			if (m_ctrlShowPoints.GetCheck()) pXDirect->m_pCurOpp->m_bShowPoints = true;
			else                             pXDirect->m_pCurOpp->m_bShowPoints = false;

			pXDirect->CreateOppCurves(); 
		}
		pFrame->SetSaveState(false);
		pXDirect->UpdateView();
	}
	else if(m_idApp == MIAREX){
		CMiarex* pMiarex = (CMiarex*)m_pParent;
		if(pMiarex->m_iView==2){
			if (pMiarex->m_pCurWPolar){ 
				if (m_ctrlShow.GetCheck())       pMiarex->m_pCurWPolar->m_bIsVisible = true;
				else                             pMiarex->m_pCurWPolar->m_bIsVisible = false;
				if (m_ctrlShowPoints.GetCheck()) pMiarex->m_pCurWPolar->m_bShowPoints = true;
				else                             pMiarex->m_pCurWPolar->m_bShowPoints = false;
			}
			pMiarex->CreateWPolarCurves(); 
			pMiarex->UpdateView();
			pFrame->SetSaveState(false);
		}
		else if(pMiarex->m_iView==4)
		{
			if (m_ctrlShow.GetCheck())		pMiarex->m_bShowCp = true;
			else                            pMiarex->m_bShowCp = false;
			pMiarex->CreateCpCurves(); 
			pMiarex->UpdateView();
		}
		else if (pMiarex->m_pCurPOpp){
			if (m_ctrlShow.GetCheck())       pMiarex->m_pCurPOpp->m_bIsVisible = true;
			else                             pMiarex->m_pCurPOpp->m_bIsVisible = false;
			if (m_ctrlShowPoints.GetCheck()) pMiarex->m_pCurPOpp->m_bShowPoints = true;
			else                             pMiarex->m_pCurPOpp->m_bShowPoints = false;

			if(pMiarex->m_iView==1)	pMiarex->CreateWOppCurves(); 
			pMiarex->UpdateView();
			pFrame->SetSaveState(false);
		}
		else if (pMiarex->m_pCurWOpp){
			if (m_ctrlShow.GetCheck())       pMiarex->m_pCurWOpp->m_bIsVisible = true;
			else                             pMiarex->m_pCurWOpp->m_bIsVisible = false;
			if (m_ctrlShowPoints.GetCheck()) pMiarex->m_pCurWOpp->m_bShowPoints = true;
			else                             pMiarex->m_pCurWOpp->m_bShowPoints = false;

			if(pMiarex->m_iView==1)	pMiarex->CreateWOppCurves(); 
			pMiarex->UpdateView();
			pFrame->SetSaveState(false);
		}
	}
}

void CCurveDlgBar::OnShowPoints() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(m_idApp == XFOILANALYSIS)
	{
		CXDirect* pXDirect = (CXDirect*)m_pParent;
		if(pXDirect->m_bPolar){
			if (pXDirect->m_pCurPolar){ 
				if (m_ctrlShowPoints.GetCheck()) pXDirect->m_pCurPolar->m_bShowPoints = true;
				else                             pXDirect->m_pCurPolar->m_bShowPoints = false;
			}
			pXDirect->CreatePolarCurves(); 
		}
		else if (pXDirect->m_pCurOpp){
			if (m_ctrlShowPoints.GetCheck()) pXDirect->m_pCurOpp->m_bShowPoints = true;
			else                             pXDirect->m_pCurOpp->m_bShowPoints = false;
			pXDirect->CreateOppCurves(); 
		}
		
		pFrame->SetSaveState(false);
		pXDirect->UpdateView();	
	}
	else if(m_idApp == MIAREX)
	{
		CMiarex* pMiarex = (CMiarex*)m_pParent;
		if(pMiarex->m_iView==2)
		{
			if (pMiarex->m_pCurWPolar){
				if (m_ctrlShowPoints.GetCheck()) pMiarex->m_pCurWPolar->m_bShowPoints = true;
				else                             pMiarex->m_pCurWPolar->m_bShowPoints = false;
			}
			pMiarex->CreateWPolarCurves(); 
			pMiarex->UpdateView();
			pFrame->SetSaveState(false);
		}
		if(pMiarex->m_iView==4)
		{
			if (m_ctrlShowPoints.GetCheck()) pMiarex->m_bShowCpPoints = true;
			else                             pMiarex->m_bShowCpPoints = false;
			pMiarex->CreateCpCurves(); 
			pMiarex->UpdateView();
		}
		else if (pMiarex->m_pCurPOpp)
		{
			if (m_ctrlShowPoints.GetCheck()) 
			{
				pMiarex->m_pCurPOpp->m_bShowPoints = true;
				if(pMiarex->m_pCurWOpp) pMiarex->m_pCurWOpp->m_bShowPoints = true;
			}
			else
			{
				pMiarex->m_pCurPOpp->m_bShowPoints = false;
				if(pMiarex->m_pCurWOpp) pMiarex->m_pCurWOpp->m_bShowPoints = false;
			}

			if(pMiarex->m_iView==1) pMiarex->CreateWOppCurves(); 
			pMiarex->UpdateView();
			pFrame->SetSaveState(false);
		}
		else if (pMiarex->m_pCurWOpp)
		{
			if (m_ctrlShowPoints.GetCheck()) pMiarex->m_pCurWOpp->m_bShowPoints = true;
			else                             pMiarex->m_pCurWOpp->m_bShowPoints = false;

			if(pMiarex->m_iView==1) pMiarex->CreateWOppCurves(); 
			pMiarex->UpdateView();
			pFrame->SetSaveState(false);
		}
	}
}

void CCurveDlgBar::OnColorBtn() 
{
	m_crColor = m_ctrlColor.GetColor();

	for (int i=0; i<5;i++){
		m_ctrlWidth.m_Lines[i].crColor = m_crColor;
	}

	for (i=0; i<5;i++){
		m_ctrlStyle.m_Lines[i].crColor = m_crColor;
	}
	Invalidate(true);
	Update();

//	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
//	pFrame->SetSaveState(false);
}

void CCurveDlgBar::OnSelChangeStyle() 
{
	int j = m_ctrlStyle.GetCurSel();
	if(j != CB_ERR){
		LineData* pLineData = (LineData*)m_ctrlStyle.GetItemDataPtr(j);
		m_nStyle = pLineData->nPenStyle;
		for (int i=0; i<5;i++){
			m_ctrlWidth.m_Lines[i].nPenStyle = m_nStyle;
		}
		Invalidate(true);
		Update();
	}
}

void CCurveDlgBar::OnSelChangeWidth() 
{
	int j = m_ctrlWidth.GetCurSel();
	if(j != CB_ERR){
		LineData* pLineData = (LineData*)m_ctrlWidth.GetItemDataPtr(j);
		m_nWidth = pLineData->nWidth;

		for (int i=0; i<5;i++){
			m_ctrlStyle.m_Lines[i].nWidth = m_nWidth;
		}
		Invalidate(true);
		Update();
	}
}

void CCurveDlgBar::SetParams()
{
	int i, j;

	if(m_idApp == XFOILANALYSIS){
		CXDirect* pXDirect = (CXDirect*)m_pParent;
		if(pXDirect->m_bPolar){
			if(pXDirect->m_pCurPolar){
				if(pXDirect->m_pCurPolar->m_bIsVisible)  m_ctrlShow.SetCheck(1);
				else                                   m_ctrlShow.SetCheck(0);
				if(pXDirect->m_pCurPolar->m_bShowPoints) m_ctrlShowPoints.SetCheck(1);
				else                                   m_ctrlShowPoints.SetCheck(0);
				
				m_crColor = pXDirect->m_pCurPolar->m_Color;
				m_nStyle  = pXDirect->m_pCurPolar->m_Style;
				m_nWidth  = pXDirect->m_pCurPolar->m_Width;
			}
			else {
				m_crColor = RGB(150,150,150);
				m_nStyle = 0;
				m_nWidth = 1;

			}
		}
		else if(!pXDirect->m_bPolar && pXDirect->m_pCurOpp){
			if(pXDirect->m_pCurOpp->m_bIsVisible)  m_ctrlShow.SetCheck(1);
			else                                 m_ctrlShow.SetCheck(0);
			if(pXDirect->m_pCurOpp->m_bShowPoints) m_ctrlShowPoints.SetCheck(1);
			else                                 m_ctrlShowPoints.SetCheck(0);
			
			m_crColor = pXDirect->m_pCurOpp->m_Color;
			m_nStyle  = pXDirect->m_pCurOpp->m_Style;
			m_nWidth  = pXDirect->m_pCurOpp->m_Width;
		}
		else {
			m_crColor = RGB(150,150,150);
			m_nStyle = 0;
			m_nWidth = 1;

		}
	}
	else if (m_idApp == MIAREX){
		CMiarex* pMiarex = (CMiarex*)m_pParent;
		if(pMiarex->m_iView==2){
			if(!pMiarex->m_pCurWPolar){
				m_crColor = RGB(150,150,150);
				m_nStyle = 0;
				m_nWidth = 1;

			}
			else{
				if(pMiarex->m_pCurWPolar->m_bIsVisible) m_ctrlShow.SetCheck(1);
				else  m_ctrlShow.SetCheck(0);
				if(pMiarex->m_pCurWPolar->m_bShowPoints) m_ctrlShowPoints.SetCheck(1);
				else  m_ctrlShowPoints.SetCheck(0);

				m_crColor = pMiarex->m_pCurWPolar->m_Color;
				m_nStyle  = pMiarex->m_pCurWPolar->m_Style;
				m_nWidth  = pMiarex->m_pCurWPolar->m_Width;
			}
		}
		else if(pMiarex->m_iView==4)
		{
			m_ctrlShow.SetCheck(1);
			m_ctrlShowPoints.SetCheck(pMiarex->m_bShowCpPoints);

			m_crColor = pMiarex->m_CpColor;
			m_nStyle  = pMiarex->m_CpStyle;
			m_nWidth  = pMiarex->m_CpWidth;
			
		}
		else if(!pMiarex->m_pCurWOpp && !pMiarex->m_pCurPOpp){
			m_crColor = RGB(150,150,150);
			m_nStyle = 0;
			m_nWidth = 1;
		}
		else{
			if(pMiarex->m_pCurPOpp){
				if(pMiarex->m_pCurPOpp->m_bIsVisible)  m_ctrlShow.SetCheck(1);
				else                                   m_ctrlShow.SetCheck(0);
				if(pMiarex->m_pCurPOpp->m_bShowPoints) m_ctrlShowPoints.SetCheck(1);
				else                                   m_ctrlShowPoints.SetCheck(0);

				m_crColor = pMiarex->m_pCurPOpp->m_Color;
				m_nStyle  = pMiarex->m_pCurPOpp->m_Style;
				m_nWidth  = pMiarex->m_pCurPOpp->m_Width;
			}
			else 
			if(pMiarex->m_pCurWOpp){
				if(pMiarex->m_pCurWOpp->m_bIsVisible)  m_ctrlShow.SetCheck(1);
				else                                   m_ctrlShow.SetCheck(0);
				if(pMiarex->m_pCurWOpp->m_bShowPoints) m_ctrlShowPoints.SetCheck(1);
				else                                   m_ctrlShowPoints.SetCheck(0);

				m_crColor = pMiarex->m_pCurWOpp->m_Color;
				m_nStyle  = pMiarex->m_pCurWOpp->m_Style;
				m_nWidth  = pMiarex->m_pCurWOpp->m_Width;
			}
		}
	}
	if (IsBlackAndWhite())	GetBWColor(m_crColor, m_nStyle, m_nWidth);
	m_ctrlColor.SetColor(m_crColor);

	for (i=0; i<5;i++){
		m_ctrlWidth.m_Lines[i].nWidth = i;
		m_ctrlWidth.m_Lines[i].crColor = m_crColor;
		m_ctrlWidth.m_Lines[i].nPenStyle = m_nStyle;
	}

	for (i=0; i<5;i++){
		m_ctrlStyle.m_Lines[i].nWidth = m_nWidth;
		m_ctrlStyle.m_Lines[i].crColor = m_crColor;
	}

	m_ctrlStyle.m_Lines[0].nPenStyle = PS_SOLID;
	m_ctrlStyle.m_Lines[1].nPenStyle = PS_DASH;
	m_ctrlStyle.m_Lines[2].nPenStyle = PS_DOT;
	m_ctrlStyle.m_Lines[3].nPenStyle = PS_DASHDOT;
	m_ctrlStyle.m_Lines[4].nPenStyle = PS_DASHDOTDOT;

	m_ctrlStyle.SetCurSel(0);
	m_ctrlWidth.SetCurSel(0);

	LineData* pLineData;
	for (j=0; j<m_ctrlStyle.GetCount(); j++){
		pLineData = (LineData*)m_ctrlStyle.GetItemDataPtr(j);
		if(pLineData->nPenStyle == m_nStyle){
			m_ctrlStyle.SetCurSel(j);
		}
	}
	for (j=0; j<m_ctrlWidth.GetCount(); j++){
		pLineData = (LineData*)m_ctrlWidth.GetItemDataPtr(j);
		if(pLineData->nWidth == m_nWidth){
			m_ctrlWidth.SetCurSel(j);
		}
	}
	Invalidate();
}


void CCurveDlgBar::Update()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(m_idApp == XFOILANALYSIS)
	{
		CXDirect* pXDirect = (CXDirect*)m_pParent;
		if(pXDirect->m_bPolar && pXDirect->m_pCurPolar)
		{
			pXDirect->m_pCurPolar->m_Color = m_crColor;
			pXDirect->m_pCurPolar->m_Style = m_nStyle;
			pXDirect->m_pCurPolar->m_Width = m_nWidth;
			pXDirect->CreatePolarCurves();
		}
		else if (!pXDirect->m_bPolar && pXDirect->m_pCurOpp)
		{
			pXDirect->m_pCurOpp->m_Color = m_crColor;
			pXDirect->m_pCurOpp->m_Style = m_nStyle;
			pXDirect->m_pCurOpp->m_Width = m_nWidth;
			pXDirect->CreateOppCurves();
		}
		pXDirect->UpdateView();
		pFrame->SetSaveState(false);
	}
	else if(m_idApp == MIAREX)
	{
		CMiarex* pMiarex = (CMiarex*)m_pParent;
		if(pMiarex->m_iView==2)
		{
			if(!pMiarex->m_pCurWPolar) return;
			pMiarex->m_pCurWPolar->m_Color = m_crColor;
			pMiarex->m_pCurWPolar->m_Style = m_nStyle;
			pMiarex->m_pCurWPolar->m_Width = m_nWidth;

			pMiarex->CreateWPolarCurves();
			pMiarex->UpdateView();
			pFrame->SetSaveState(false);
		}
		else if (pMiarex->m_iView==1)
		{
			if(pMiarex->m_pCurPOpp)
			{
				pMiarex->m_pCurPOpp->m_Color = m_crColor;
				pMiarex->m_pCurPOpp->m_Style = m_nStyle;
				pMiarex->m_pCurPOpp->m_Width = m_nWidth;
				pMiarex->m_pCurWOpp->m_Color = m_crColor;
				pMiarex->m_pCurWOpp->m_Style = m_nStyle;
				pMiarex->m_pCurWOpp->m_Width = m_nWidth;
			}
			else if(pMiarex->m_pCurWOpp)
			{
				pMiarex->m_pCurWOpp->m_Color = m_crColor;
				pMiarex->m_pCurWOpp->m_Style = m_nStyle;
				pMiarex->m_pCurWOpp->m_Width = m_nWidth;
			}
			if (pMiarex->m_iView==1) pMiarex->CreateWOppCurves();
			pFrame->SetSaveState(false);
			pMiarex->UpdateView();
		}
		else if (pMiarex->m_iView==4)
		{
			pMiarex->m_CpColor = m_crColor;
			pMiarex->m_CpStyle = m_nStyle;
			pMiarex->m_CpWidth = m_nWidth;
			pMiarex->CreateCpCurves();
			pMiarex->UpdateView();
		}
	}
}
