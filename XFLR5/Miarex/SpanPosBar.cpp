/****************************************************************************

    CSpanPosBar Class
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


////////////////////////////////////////////////////////////////////////////
//
// MainDialogBar.cpp: implementation of the CSpanPosBar class.
// The class associated to the dialog box, which will process
// all messages related to 3D commands
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../main/MainFrm.h"
#include "Miarex.h"
#include "SpanPosBar.h"
#include ".\spanposbar.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CSpanPosBar, CInitDialogBar)
		
BEGIN_MESSAGE_MAP(CSpanPosBar, CInitDialogBar)
//{{AFX_MSG_MAP(CSpanPosBar)
	ON_WM_HSCROLL()
	ON_EN_KILLFOCUS(IDC_SPANPOS, OnKillFocusSpanPos)
	ON_BN_CLICKED(IDC_KEEPCURVE, OnKeepCurve)
	ON_BN_CLICKED(IDC_RESETCURVES, OnResetCurves)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CSpanPosBar::CSpanPosBar(CWnd* pParent)
{
	//Set Initial conditions for controls
//	m_pMiarex  = pParent;
	m_SpanPos  = 0.0;
	m_bApplied = false;
}


CSpanPosBar::~CSpanPosBar()
{

}


void CSpanPosBar::DoDataExchange(CDataExchange* pDX)
{
	ASSERT(pDX);
	CInitDialogBar::DoDataExchange(pDX);
	// DDX_??? functions to associate control with
	// data or control object
	// Call UpdateData(TRUE) to get data at any time
	// Call UpdateData(FALSE) to set data at any time

	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_SPANSLIDE, m_ctrlSpanSlide);
	DDX_Control(pDX, IDC_SPANPOS, m_ctrlSpanPos);
	DDX_Control(pDX, IDC_KEEPCURVE, m_ctrlKeepCurve);
	DDX_Control(pDX, IDC_RESETCURVES, m_ctrlResetCurves);

	//}}AFX_DATA_MAP
}

BOOL CSpanPosBar::OnInitDialogBar()
{
	// Support for DDX mechanism
	// If you do not want DDX then
	// do not call base class
	CInitDialogBar::OnInitDialogBar();

	m_ctrlSpanPos.SetPrecision(2);
	m_ctrlSpanPos.SetValue(m_SpanPos);
	m_ctrlSpanPos.SetSel(0,-1);
	m_ctrlSpanPos.SetFocus();

	m_ctrlSpanSlide.SetRange(-100,100,false);
	m_ctrlSpanSlide.SetPos((int)(m_SpanPos*100.0));
	m_ctrlSpanSlide.SetTicFreq(10);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSpanPosBar message handlers

BOOL CSpanPosBar::PreTranslateMessage(MSG* pMsg) 
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		CWnd* ppp = GetFocus();

		if(GetDlgItem(IDC_SPANPOS) == ppp) 
		{
			m_SpanPos = m_ctrlSpanPos.GetValue();
			m_SpanPos = max(-1.0, m_SpanPos);
			m_SpanPos = min( 1.0, m_SpanPos);

			m_ctrlSpanPos.SetValue(m_SpanPos);
			m_ctrlSpanSlide.SetPos((int)(m_SpanPos*100));
			m_bApplied = false;
			m_ctrlSpanSlide.SetFocus();
			
			return true;
		}
	}	

	return CInitDialogBar::PreTranslateMessage(pMsg);
}

void CSpanPosBar::Apply()
{
	if(m_bApplied) return;
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	if(pFrame->m_iApp == MIAREX && pMiarex->m_iView==4)
	{
		pMiarex->m_CurSpanPos = m_SpanPos;
		pMiarex->CreateCpCurves();
		pMiarex->UpdateView();
		m_bApplied = true;
	}
}

void CSpanPosBar::OnKillFocusSpanPos() 
{
	m_bApplied = false;
	m_SpanPos = m_ctrlSpanPos.GetValue();
	m_SpanPos = max(-1.0, m_SpanPos);
	m_SpanPos = min( 1.0, m_SpanPos);

	m_ctrlSpanSlide.SetPos((int)(m_SpanPos*100));
	Apply();
}

void CSpanPosBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar->GetSafeHwnd() == GetDlgItem(IDC_SPANSLIDE)->GetSafeHwnd())  
	{
		m_bApplied = false;
		m_SpanPos = (double)m_ctrlSpanSlide.GetPos()/100.0;
		m_ctrlSpanPos.SetValue(m_SpanPos);
		Apply();
	}

//	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSpanPosBar::OnKeepCurve()
{
	CCurve *pCurve, *pNewCurve;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;

	pCurve = pMiarex->m_CpGraph.GetCurve(0);
	pNewCurve = pMiarex->m_CpGraph.AddCurve();
	pNewCurve->Copy(pCurve);

	pMiarex->m_CpColor = pFrame->m_crColors[(pMiarex->m_CpGraph.GetCurveCount()-1)%24];
	pCurve->SetColor(pMiarex->m_CpColor);

	pMiarex->m_CpStyle = PS_SOLID;
	pMiarex->m_CpWidth = 1;
	pMiarex->m_bShowCpPoints = false;
	pFrame->m_PolarDlgBar.SetParams();
	
	m_bApplied = false;
	Apply();
}

void CSpanPosBar::OnResetCurves()
{
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	for(int i=pMiarex->m_CpGraph.GetCurveCount()-1; i>3 ;i--)	pMiarex->m_CpGraph.DeleteCurve(i);
	m_bApplied = false;
	Apply();
}













