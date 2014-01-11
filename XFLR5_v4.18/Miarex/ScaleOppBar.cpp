/****************************************************************************

    CScaleOppBar Class
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
// MainDialogBar.cpp: implementation of the CScaleOppBar class.
// The class associated to the dialog box, which will process
// all messages related to 3D commands
//
//////////////////////////////////////////////////////////////////////

//#include <math.h>
#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "Miarex.h"
#include "ScaleOppBar.h"
#include ".\scaleoppbar.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CScaleOppBar, CInitDialogBar)
		
BEGIN_MESSAGE_MAP(CScaleOppBar, CInitDialogBar)
//{{AFX_MSG_MAP(CScaleOppBar)
	ON_WM_HSCROLL()
//}}AFX_MSG_MAP
ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_DOWNWASHSCALE, OnNMReleasedcaptureDownwashScale)
END_MESSAGE_MAP()


CScaleOppBar::CScaleOppBar(CWnd* pParent)
{
	//Set Initial conditions for controls
	m_pFrame    = pParent;
	m_bLiftChanged     = false;
	m_bDragChanged     = false;
	m_bDownwashChanged = false;
	m_LiftScale     = 0.7;
	m_DragScale     = 0.7;
	m_DownwashScale = 0.7;
}


CScaleOppBar::~CScaleOppBar()
{

}


void CScaleOppBar::DoDataExchange(CDataExchange* pDX)
{
	ASSERT(pDX);
	CInitDialogBar::DoDataExchange(pDX);
	// DDX_??? functions to associate control with
	// data or control object
	// Call UpdateData(TRUE) to get data at any time
	// Call UpdateData(FALSE) to set data at any time

	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_DOWNWASHSCALE, m_ctrlDownWashScale);
	DDX_Control(pDX, IDC_LIFTSCALE, m_ctrlLiftScale);
	DDX_Control(pDX, IDC_DRAGSCALE, m_ctrlDragScale);

	//}}AFX_DATA_MAP
}

BOOL CScaleOppBar::OnInitDialogBar()
{
	// Support for DDX mechanism
	// If you do not want DDX then
	// do not call base class
	CInitDialogBar::OnInitDialogBar();

	m_ctrlLiftScale.SetRange(0,100,false);
	m_ctrlLiftScale.SetTicFreq(5);

	m_ctrlDragScale.SetRange(0,100,false);
	m_ctrlDragScale.SetTicFreq(5);

	m_ctrlDownWashScale.SetRange(0,100,false);
	m_ctrlDownWashScale.SetTicFreq(5);

	SetPos();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CScaleOppBar message handlers

BOOL CScaleOppBar::PreTranslateMessage(MSG* pMsg) 
{
	return CInitDialogBar::PreTranslateMessage(pMsg);
}

void CScaleOppBar::Apply()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pFrame->m_LiftScale     = m_LiftScale;
	pFrame->m_DragScale     = m_DragScale;
	pFrame->m_DownwashScale = m_DownwashScale;
	if(pFrame->m_iApp == MIAREX){
		pFrame->Miarex.m_bResetglDownwash = m_bDownwashChanged;
//		pFrame->Miarex.m_bResetglFlow     = m_bDownwashChanged;
		pFrame->Miarex.m_bResetglLift     = m_bLiftChanged;
		pFrame->Miarex.m_bResetglDrag     = m_bDragChanged;
	}
	m_pChildView->Invalidate();
	m_bLiftChanged     = false;
	m_bDragChanged     = false;
	m_bDownwashChanged = false;
}


void CScaleOppBar::OnNMReleasedcaptureDownwashScale(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pFrame->Miarex.m_bResetglFlow     = true;
	m_pChildView->Invalidate();
	
	*pResult = 0;
}



void CScaleOppBar::SetPos()
{
	double pos;
	pos = -m_LiftScale*m_LiftScale + m_LiftScale*sqrt(m_LiftScale*m_LiftScale+4.0*1.01);
	pos = pos/2.0/1.01;
	m_ctrlLiftScale.SetPos((int)(pos*100.0));

	pos = -m_DragScale*m_DragScale + m_DragScale*sqrt(m_DragScale*m_DragScale+4.0*1.01);
	pos = pos/2.0/1.01;
	m_ctrlDragScale.SetPos((int)(pos*100.0));

	pos = -m_DownwashScale*m_DownwashScale + m_DownwashScale*sqrt(m_DownwashScale*m_DownwashScale+4.0*1.01);
	pos = pos/2.0/1.01;
	m_ctrlDownWashScale.SetPos((int)(pos*100.0));
}
void CScaleOppBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar->GetSafeHwnd() == GetDlgItem(IDC_LIFTSCALE)->GetSafeHwnd())   
	{
		double pos =  (double)m_ctrlLiftScale.GetPos();
		m_LiftScale    = pos/100.0/sqrt(1.01-pos/100.0);
		m_bLiftChanged = true;
		Apply();
	}
	else if (pScrollBar->GetSafeHwnd() == GetDlgItem(IDC_DRAGSCALE)->GetSafeHwnd())   
	{
		double pos =  (double)m_ctrlDragScale.GetPos();
		m_DragScale    = pos/100.0/sqrt(1.01-pos/100.0);
		m_bDragChanged = true;
		Apply();
	}
	else if (pScrollBar->GetSafeHwnd() == GetDlgItem(IDC_DOWNWASHSCALE)->GetSafeHwnd())  
	{
		double pos =  (double)m_ctrlDownWashScale.GetPos();
		m_DownwashScale    = pos/100.0/sqrt(1.01-pos/100.0);
		m_bDownwashChanged = true;
		Apply();
	}
//	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
