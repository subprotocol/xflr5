/****************************************************************************

    CW3DBar Class
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
// MainDialogBar.cpp: implementation of the CW3DBar class.
// The class associated to the dialog box, which will process
// all messages related to 3D commands
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "stdafx.h"

#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "Miarex.h"
#include "W3DBar.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CW3DBar, CInitDialogBar)

BEGIN_MESSAGE_MAP(CW3DBar, CInitDialogBar)
//{{AFX_MSG_MAP(CW3DBar)
	ON_BN_CLICKED(IDC_3DAXES, On3DAxes)
	ON_BN_CLICKED(IDC_3DLIGHT, On3DLight)
	ON_BN_CLICKED(IDC_3DSURFACES, On3DSurfaces)
	ON_BN_CLICKED(IDC_3DOUTLINE, On3DOutline)
	ON_BN_CLICKED(IDC_3DVORTICES, On3DVortices)
	ON_BN_CLICKED(IDC_3DPANELS, On3DPanels)
	ON_BN_CLICKED(IDC_3DRESET, On3DReset)
	ON_BN_CLICKED(IDC_3DISO, On3DIso)
	ON_BN_CLICKED(IDC_3DLEFT, On3DLeft)
	ON_BN_CLICKED(IDC_3DTOP, On3DTop)
	ON_BN_CLICKED(IDC_3DFRONT, On3DFront)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CW3DBar::CW3DBar(CWnd* pParent)
{
	//Set Initial conditions for controls
	m_pFrame     = pParent;
	m_pMiarex    = NULL;
	m_glXRotatef = 125.0;
	m_glYRotatef = 180.0;
	m_glZRotatef = -35.0;
//	m_glXRotatef =    90.0;
//	m_glYRotatef =   180.0;
//	m_glZRotatef =   180.0;
}


CW3DBar::~CW3DBar()
{

}



void CW3DBar::DoDataExchange(CDataExchange* pDX)
{
	ASSERT(pDX);
	CInitDialogBar::DoDataExchange(pDX);
	// DDX_??? functions to associate control with
	// data or control object
	// Call UpdateData(TRUE) to get data at any time
	// Call UpdateData(FALSE) to set data at any time

	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_3DAXES, m_ctrlAxes);
	DDX_Control(pDX, IDC_3DLIGHT, m_ctrlLight);
	DDX_Control(pDX, IDC_3DSURFACES, m_ctrlSurfaces);
	DDX_Control(pDX, IDC_3DVORTICES, m_ctrlVortices);
	DDX_Control(pDX, IDC_3DOUTLINE, m_ctrlOutline);
	DDX_Control(pDX, IDC_3DPANELS, m_ctrlVLMPanels);
	//}}AFX_DATA_MAP
}

BOOL CW3DBar::OnInitDialogBar()
{
	// Support for DDX mechanism
	// If you do not want DDX then
	// do not call base class
	CInitDialogBar::OnInitDialogBar();

	SetChecks();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CW3DBar message handlers

BOOL CW3DBar::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_MOUSEWHEEL){
		CMiarex* pMiarex = (CMiarex*)m_pMiarex;
		CPoint pt((short) LOWORD(pMsg->lParam),(short) HIWORD(pMsg->lParam));

		pMiarex->OnMouseWheel(LOWORD(pMsg->wParam),(short) HIWORD(pMsg->wParam), pt);
		pMiarex->m_pChildWnd->SetFocus();
		return true;
	}
	return CInitDialogBar::PreTranslateMessage(pMsg);
}


void CW3DBar::On3DAxes()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(m_ctrlAxes.GetCheck()) pMiarex->m_bAxes = true;
	else					  pMiarex->m_bAxes = false;
	pMiarex->m_bResetglBody2D = true;
	pMiarex->UpdateView();
}

void CW3DBar::On3DLight()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(m_ctrlLight.GetCheck()) pMiarex->m_bglLight = true;
	else					   pMiarex->m_bglLight = false;
	
	pMiarex->UpdateView();
}
void CW3DBar::On3DSurfaces()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	
	if(m_ctrlSurfaces.GetCheck()) {
		pMiarex->m_bSurfaces   = true;
		pMiarex->m_b3DVLMCl    = false;
	}
	else pMiarex->m_bSurfaces = false;
	
 	pMiarex->UpdateView();
}

void CW3DBar::On3DOutline()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(m_ctrlOutline.GetCheck()) pMiarex->m_bOutline = true;
	else						 pMiarex->m_bOutline = false;
	
	pMiarex->UpdateView();
}

void CW3DBar::On3DVortices()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(m_ctrlVortices.GetCheck()) pMiarex->m_bVortices = true;
	else						  pMiarex->m_bVortices = false;
	
	pMiarex->UpdateView();
}



void CW3DBar::On3DPanels()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(m_ctrlVLMPanels.GetCheck())
		pMiarex->m_bVLMPanels = true;
	else		
		pMiarex->m_bVLMPanels = false;
	
	pMiarex->UpdateView();
}

void CW3DBar::On3DReset()
{
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	pMiarex->m_glXTransf  =  0.0;
	pMiarex->m_glYTransf  =  0.0;
	pMiarex->m_bIs3DScaleSet  = false;
	pMiarex->SetScale();
	pMiarex->UpdateView();
}

void CW3DBar::On3DIso()
{
	m_glXRotatef =  125.0;
	m_glYRotatef =  180.0;
	m_glZRotatef =  -35.0;

	CMiarex *pMiarex = (CMiarex*)m_pMiarex;

	pMiarex->m_ArcBall.ab_quat[0]	= -0.65987748f;
	pMiarex->m_ArcBall.ab_quat[1]	=  0.38526487f;
	pMiarex->m_ArcBall.ab_quat[2]	= -0.64508355f;
	pMiarex->m_ArcBall.ab_quat[3]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[4]	= -0.75137258f;
	pMiarex->m_ArcBall.ab_quat[5]	= -0.33720365f;
	pMiarex->m_ArcBall.ab_quat[6]	=  0.56721509f;
	pMiarex->m_ArcBall.ab_quat[7]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[8]	=  0.000f;
	pMiarex->m_ArcBall.ab_quat[9]	=  0.85899049f;
	pMiarex->m_ArcBall.ab_quat[10]	=  0.51199043f;
	pMiarex->m_ArcBall.ab_quat[11]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[12]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[13]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[14]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[15]	=  1.0f;


	pMiarex->UpdateView();
}

void CW3DBar::On3DTop()
{
	m_glXRotatef =   0.0;
	m_glYRotatef =   0.0;
	m_glZRotatef = -90.0;

	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	pMiarex->m_ArcBall.ab_quat[0]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[1]	= -1.0f;
	pMiarex->m_ArcBall.ab_quat[2]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[3]	=  0.0f;

	pMiarex->m_ArcBall.ab_quat[4]	=  1.0f;
	pMiarex->m_ArcBall.ab_quat[5]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[6]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[7]	=  0.0f;

	pMiarex->m_ArcBall.ab_quat[8]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[9]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[10]	=  1.0f;
	pMiarex->m_ArcBall.ab_quat[11]	=  0.0f;

	pMiarex->m_ArcBall.ab_quat[12]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[13]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[14]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[15]	=  1.0f;

	pMiarex->UpdateView();
}
void CW3DBar::On3DLeft()
{
	m_glXRotatef =    90.0;
	m_glYRotatef =   180.0;
	m_glZRotatef =   180.0;
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	pMiarex->m_ArcBall.ab_quat[0]	= -1.0f;
	pMiarex->m_ArcBall.ab_quat[1]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[2]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[3]	=  0.0f;

	pMiarex->m_ArcBall.ab_quat[4]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[5]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[6]	=  1.0f;
	pMiarex->m_ArcBall.ab_quat[7]	=  0.0f;

	pMiarex->m_ArcBall.ab_quat[8]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[9]	=  1.0f;
	pMiarex->m_ArcBall.ab_quat[10]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[11]	=  0.0f;

	pMiarex->m_ArcBall.ab_quat[12]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[13]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[14]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[15]	=  1.0f;

	pMiarex->UpdateView();

}


void CW3DBar::On3DFront()
{
	m_glXRotatef =  90.0;
	m_glYRotatef = 180.0;
	m_glZRotatef =  90.0;

	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	pMiarex->m_ArcBall.ab_quat[0]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[1]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[2]	=  1.0f;
	pMiarex->m_ArcBall.ab_quat[3]	=  0.0f;

	pMiarex->m_ArcBall.ab_quat[4]	=  1.0f;
	pMiarex->m_ArcBall.ab_quat[5]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[6]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[7]	=  0.0f;

	pMiarex->m_ArcBall.ab_quat[8]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[9]	=  1.0f;
	pMiarex->m_ArcBall.ab_quat[10]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[11]	=  0.0f;

	pMiarex->m_ArcBall.ab_quat[12]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[13]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[14]	=  0.0f;
	pMiarex->m_ArcBall.ab_quat[15]	=  1.0f;

	pMiarex->UpdateView();
}



void CW3DBar::SetChecks()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	if(pMiarex->m_bAxes)		m_ctrlAxes.SetCheck(TRUE);
	else						m_ctrlAxes.SetCheck(FALSE);
	if(pMiarex->m_bglLight)		m_ctrlLight.SetCheck(TRUE);
	else						m_ctrlLight.SetCheck(FALSE);
	if(pMiarex->m_bOutline)		m_ctrlOutline.SetCheck(TRUE);
	else						m_ctrlOutline.SetCheck(FALSE);
	if(pMiarex->m_bVLMPanels)	m_ctrlVLMPanels.SetCheck(TRUE);
	else						m_ctrlVLMPanels.SetCheck(FALSE);
	if(pMiarex->m_bSurfaces)	m_ctrlSurfaces.SetCheck(TRUE);
	else						m_ctrlSurfaces.SetCheck(FALSE);
	if(pMiarex->m_bVortices)	m_ctrlVortices.SetCheck(true);
	else						m_ctrlVortices.SetCheck(false);

}
