/****************************************************************************

	CAFoilCtrlBar class
	Copyright (C) 2004 André Deperrois xflr5@yahoo.com

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

// AFoilCtrl.cpp: implementation of the CAFoilCtrlBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AFoil.h"
#include "../X-FLR5.h"
#include "../misc/NameDlg.h"
#include "../misc/LinePickerDlg.h"
#include "../main/MainFrm.h"
#include ".\afoilctrlbar.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CAFoilCtrlBar, CInitDialogBar)

BEGIN_MESSAGE_MAP(CAFoilCtrlBar, CInitDialogBar)
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_FOILCOLOR, OnFoilColor)
	ON_BN_CLICKED(IDC_CENTERLINE, OnCenterLine)
	ON_BN_CLICKED(IDC_MODELPOINTS, OnModelPoints)
	ON_BN_CLICKED(IDC_VISIBLE, OnVisible)
	ON_BN_CLICKED(IDC_RENAME, OnRename)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_DUPLICATE, OnDuplicate)
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FOILLIST, OnLvnItemchangedFoillist)


END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAFoilCtrlBar dialog



CAFoilCtrlBar::CAFoilCtrlBar(CWnd* pParent)
{
	//{{AFX_DATA_INIT(CAFoilCtrlBar)

	//}}AFX_DATA_INIT
	m_pADlg = pParent;
	m_pRefFoil = NULL;
	m_FixedFont.CreatePointFont(85, "Courier New");
	m_pSF = NULL;
	m_pPF = NULL;
	m_FoilSelection = 0;
	m_ctrlFoilList.m_pParent = this;
}

CAFoilCtrlBar::~CAFoilCtrlBar()
{

}


void CAFoilCtrlBar::DoDataExchange(CDataExchange* pDX)
{
	ASSERT(pDX);

	CInitDialogBar::DoDataExchange(pDX);

	// DDX_??? functions to associate control with
	// data or control object

	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_FOILLIST, m_ctrlFoilList);
	DDX_Control(pDX, IDC_VISIBLE, m_ctrlVisible);
	DDX_Control(pDX, IDC_CENTERLINE, m_ctrlCenterLine);
	DDX_Control(pDX, IDC_MODELPOINTS, m_ctrlModelPoints);
	DDX_Control(pDX, IDC_FOILCOLOR, m_ctrlFoilColor); 
	DDX_Control(pDX, IDC_RENAME, m_ctrlRename); 
	DDX_Control(pDX, IDC_DELETE, m_ctrlDelete); 
	DDX_Control(pDX, IDC_DUPLICATE, m_ctrlDuplicate); 


	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CAFoilCtrlBar message handlers

void CAFoilCtrlBar::OnCenterLine()
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	if(m_FoilSelection==0)
	{
		if(pADlg->m_bSF)
		{
			if(m_ctrlCenterLine.GetCheck()) m_pSF->m_bCenterLine = true;
			else							m_pSF->m_bCenterLine = false;
		}
		else 
		{
			if(m_ctrlCenterLine.GetCheck()) m_pPF->m_bCenterLine = true;
			else							m_pPF->m_bCenterLine = false;
		}
	}
	else if(m_pRefFoil)
	{
		if(m_ctrlCenterLine.GetCheck()) m_pRefFoil->m_bCenterLine = true;
		else							m_pRefFoil->m_bCenterLine = false;
	}
	SetSaveState(false);
	UpdateBlt();
}



void CAFoilCtrlBar::OnModelPoints() 
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	if(m_FoilSelection==0)
	{
		if(pADlg->m_bSF)
		{
			if(m_ctrlModelPoints.GetCheck()) m_pSF->m_bOutPoints = true;
			else							 m_pSF->m_bOutPoints = false;
		}
		else 
		{
			if(m_ctrlModelPoints.GetCheck()) m_pPF->m_bOutPoints = true;
			else							 m_pPF->m_bOutPoints = false;
			m_pPF->SetOutPoints(m_pPF->m_bOutPoints);
		}
	}
	else if(m_pRefFoil)
	{
		if (m_ctrlModelPoints.GetCheck()) m_pRefFoil->m_bPoints = true;
		else m_pRefFoil->m_bPoints = false;
	}
	SetSaveState(false);
	UpdateBlt();
}


void CAFoilCtrlBar::OnVisible()
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	if(m_FoilSelection==0)
	{
		if(pADlg->m_bSF){
			if(m_ctrlVisible.GetCheck()) m_pSF->m_bVisible = true;
			else						 m_pSF->m_bVisible = false;
		}
		else {
			if(m_ctrlVisible.GetCheck()) m_pPF->m_bVisible = true;
			else						 m_pPF->m_bVisible = false;
		}
	}
	else if(m_pRefFoil){
		if(m_ctrlVisible.GetCheck()) pADlg->ShowFoil(m_pRefFoil, true);
		else						 pADlg->ShowFoil(m_pRefFoil, false);
	}
	SetSaveState(false);
	UpdateBlt();
}



BOOL CAFoilCtrlBar::OnInitDialogBar() 
{
	CInitDialogBar::OnInitDialogBar();

	m_pRefFoil = NULL;
	SetParams();

	m_ctrlFoilList.InsertColumn(0,"Foil Name",LVCFMT_LEFT, 250);
	m_ctrlFoilList.InsertColumn(1,"Thickness",LVCFMT_RIGHT,60);
	m_ctrlFoilList.InsertColumn(2,"At",LVCFMT_RIGHT,60);
	m_ctrlFoilList.InsertColumn(3,"Camber",LVCFMT_RIGHT,60);
	m_ctrlFoilList.InsertColumn(4,"At",LVCFMT_RIGHT,60);
	m_ctrlFoilList.InsertColumn(5,"Points",LVCFMT_RIGHT,60);
	m_ctrlFoilList.InsertColumn(6,"TE Flap Angle",LVCFMT_RIGHT,60);
	m_ctrlFoilList.InsertColumn(7,"TE XHinge",LVCFMT_RIGHT,60);
	m_ctrlFoilList.InsertColumn(8,"TE YHinge",LVCFMT_RIGHT,60);
	m_ctrlFoilList.InsertColumn(9,"LE Flap Angle",LVCFMT_RIGHT,60);
	m_ctrlFoilList.InsertColumn(10,"LE XHinge",LVCFMT_RIGHT,60);
	m_ctrlFoilList.InsertColumn(11,"LE YHinge",LVCFMT_RIGHT,60);

	m_ctrlFoilList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_ctrlFoilList.ModifyStyle(NULL, (LVS_REPORT | LVS_SHOWSELALWAYS), 0);
	m_ctrlFoilList.SetFocus();

	return TRUE; 
}


void CAFoilCtrlBar::UpdateBlt() 
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	pADlg->UpdateView();
}



BOOL CAFoilCtrlBar::PreTranslateMessage(MSG* pMsg) 
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	CMainFrame *pFrame = (CMainFrame*)(pADlg->m_pFrame);
	CWnd* pWnd = GetFocus();


	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		CWnd* pWnd = GetFocus();
		if (pWnd == this)	ShowWindow(SW_HIDE);
		else SetFocus();
	}
	else if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F4)
	{
		pADlg->OnStoreFoil();
		return true;
	}
	else if(pMsg->message == WM_MOUSEWHEEL && pWnd==pADlg->m_pChildWnd)
	{

		CPoint pt((short) LOWORD(pMsg->lParam),(short) HIWORD(pMsg->lParam));

		pADlg->OnMouseWheel(LOWORD(pMsg->wParam),(short) HIWORD(pMsg->wParam), pt);
//		pADlg->m_pChildWnd->SetFocus();
		return true;
	}

	SHORT sh1 = GetKeyState(VK_LCONTROL);
	SHORT sh2 = GetKeyState(VK_RCONTROL);

	if (pMsg->wParam == 'Z' && 
			( (sh1 & 0x8000)||(sh2 & 0x8000) )) 
	{ 
		pADlg->OnUndo();
		return true;
	} 
	if (pMsg->wParam == 'Y' && 
			( (sh1 & 0x8000)||(sh2 & 0x8000) )) { 
		pADlg->OnRedo();
		return true;
	} 
	if (pMsg->wParam == 'E' && 
			( (sh1 & 0x8000)||(sh2 & 0x8000) )) { 
		pADlg->OnSFSave();
		return true;
	} 
	if (pMsg->wParam == 'S' && 
			( (sh1 & 0x8000)||(sh2 & 0x8000) )) { 
		pADlg->SaveProject();
		return true;
	} 
	if (pMsg->wParam == 'O' && 
			( (sh1 & 0x8000)||(sh2 & 0x8000) )) { 
//		pADlg->OnLoad();
		pFrame->OnLoadProject();
		return true;
	} 


	return CInitDialogBar::PreTranslateMessage(pMsg);
}

void CAFoilCtrlBar::SetSplineData()
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	if(pADlg->m_bSF)
	{
		if(m_pSF)
		{
			m_ctrlVisible.SetCheck(m_pSF->m_bVisible);
			m_ctrlCenterLine.SetCheck(m_pSF->m_bCenterLine);
			m_ctrlModelPoints.SetCheck(m_pSF->m_bOutPoints);
		}
	}
	else if(m_pPF) 
	{
		m_ctrlVisible.SetCheck(m_pPF->m_bVisible);
		m_ctrlCenterLine.SetCheck(m_pPF->m_bCenterLine);
		m_ctrlModelPoints.SetCheck(m_pPF->m_bOutPoints);
	}
	m_ctrlFoilColor.Invalidate();
}

void CAFoilCtrlBar::SelectFoil(CFoil* pFoil)
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	int i;
	if(!pFoil) pFoil = m_pRefFoil;

	if(pFoil)
	{
		m_pRefFoil = pFoil;
		SetFoil();
		for(i=0; i<pADlg->m_poaFoil->GetSize();i++)
		{
			if (pFoil == pADlg->m_poaFoil->GetAt(i))
			{
				m_ctrlFoilList.EnsureVisible(i+1,FALSE);
				m_ctrlFoilList.SetItemState(i+1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
				break;
			}
		}
	}
	else
	{
		if(pADlg->m_poaFoil->GetSize())
		{
			m_pRefFoil = (CFoil*)pADlg->m_poaFoil->GetAt(0);
			SetFoil(m_pRefFoil);
			m_ctrlFoilList.SetItemState(1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		}
		else
		{
			SetFoil();
			m_ctrlFoilList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

		}
	}
}

void CAFoilCtrlBar::SetFoil(CFoil* pFoil)
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	CFoil * pOldFoil = NULL;
	CMainFrame *pFrame = (CMainFrame*)(pADlg->m_pFrame);
	int i;

	if(!pFoil) pFoil = m_pRefFoil;
	if(!pFoil)
	{
		//nothing more to try
		m_pRefFoil = NULL;
		m_FoilSelection = 0;
		if(pADlg->m_bSF)
		{
			m_ctrlVisible.SetCheck(m_pSF->m_bVisible);
			m_ctrlCenterLine.SetCheck(m_pSF->m_bCenterLine);
			m_ctrlModelPoints.SetCheck(m_pSF->m_bOutPoints);	
		}
		else
		{
			m_ctrlVisible.SetCheck(m_pPF->m_bVisible);
			m_ctrlCenterLine.SetCheck(m_pPF->m_bCenterLine);
			m_ctrlModelPoints.SetCheck(m_pPF->m_bOutPoints);	
		}
	}
	else
	{
		pFrame->m_pCurFoil = pFoil;
		m_pRefFoil = pFoil;
		m_ctrlVisible.SetCheck(pFoil->m_bVisible);
		m_ctrlCenterLine.SetCheck(pFoil->m_bCenterLine);
		m_ctrlModelPoints.SetCheck(pFoil->m_bPoints);

		for(i=0; i<pADlg->m_poaFoil->GetSize();i++)
		{
			pOldFoil = (CFoil*)pADlg->m_poaFoil->GetAt(i);
			if (pFoil == pOldFoil)
			{
				m_FoilSelection = i+1;
				break;
			}
		}
	}
	
	m_ctrlFoilColor.Invalidate();
}


void CAFoilCtrlBar::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	if (!pDC) return;
	CAFoil* pADlg = (CAFoil*)m_pADlg;

	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	COLORREF color;
	int style, width;

	if(nIDCtl==IDC_FOILCOLOR ){
		if(m_FoilSelection==0) {
			if(pADlg->m_bSF){
				color = m_pSF->m_FoilColor;
				style = m_pSF->m_FoilStyle;
				width = m_pSF->m_FoilWidth;
			}
			else{
				color = m_pPF->m_FoilColor;
				style = m_pPF->m_FoilStyle;
				width = m_pPF->m_FoilWidth;
			}
		}
		else if(m_pRefFoil){
			color = m_pRefFoil->m_FoilColor;
			style = m_pRefFoil->m_nFoilStyle;
			width = m_pRefFoil->m_nFoilWidth;
		}
		else {
			color = RGB(100,100,100);
			style = 0;
			width = 1;
		}
	
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_FOILCOLOR)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		if (IsBlackAndWhite())	GetBWColor(color, style, width);

		lb.lbColor = color;
		CPen FoilPen(PS_GEOMETRIC | style, GetPenWidth(width,false), &lb);

		pDC->SelectObject(&FoilPen);

		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);

		pDC->SelectObject(pOldPen);
		FoilPen.DeleteObject();
		DotPen.DeleteObject();

		pDC->SelectObject(pOldBrush);
		FillBrush.DeleteObject();
		
	}
}

void CAFoilCtrlBar::SetParams(CFoil *pFoil)
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;

//	ReSize();
	if(pFoil) 
	{
		m_pRefFoil=pFoil;
		SetFoil(m_pRefFoil);
	}
	else 
	{
//		m_ctrlFoilList.SetCurSel(0);
		SetSplineData();
	}
}

BOOL CAFoilCtrlBar::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	ScreenToClient(&pt);
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	pADlg->OnMouseWheel(nFlags, zDelta, pt);
	return CWnd ::OnMouseWheel(nFlags, zDelta, pt);
}

CFoil* CAFoilCtrlBar::GetFoil()
{
	if(m_FoilSelection!=0)
	{
		return m_pRefFoil;
	}
	return NULL;
}

void CAFoilCtrlBar::OnFoilColor()
{
	CLinePickerDlg LPdlg;

	CAFoil* pADlg = (CAFoil*)m_pADlg;

	if(m_FoilSelection==0)
	{
		if(pADlg->m_bSF)
		{
			COLORREF color = m_pSF->m_FoilColor;
			int style      = m_pSF->m_FoilStyle;
			int width      = m_pSF->m_FoilWidth;
			if(IsBlackAndWhite()) GetBWColor(color, style, width);

			LPdlg.SetColor(color);
			LPdlg.SetStyle(style);
			LPdlg.SetWidth(width);

			if (IDOK==LPdlg.DoModal()){
				m_pSF->m_FoilColor = LPdlg.GetColor();
				m_pSF->m_FoilStyle = LPdlg.GetStyle();
				m_pSF->m_FoilWidth = LPdlg.GetWidth();
			}
		}
		else{
			COLORREF color = m_pPF->m_FoilColor;
			int style      = m_pPF->m_FoilStyle;
			int width      = m_pPF->m_FoilWidth;
			if(IsBlackAndWhite()) GetBWColor(color, style, width);

			LPdlg.SetColor(color);
			LPdlg.SetStyle(style);
			LPdlg.SetWidth(width);

			if (IDOK==LPdlg.DoModal()){
				m_pPF->m_FoilColor = LPdlg.GetColor();
				m_pPF->m_FoilStyle = LPdlg.GetStyle();
				m_pPF->m_FoilWidth = LPdlg.GetWidth();
				SetSaveState(false);
			}
		}
	}
	else if (m_pRefFoil)	{
		COLORREF color = m_pRefFoil->m_FoilColor;
		int style      = m_pRefFoil->m_nFoilStyle;
		int width      = m_pRefFoil->m_nFoilWidth;
		if(IsBlackAndWhite()) GetBWColor(color, style, width);

		LPdlg.SetColor(color);
		LPdlg.SetStyle(style);
		LPdlg.SetWidth(width);

		if (IDOK==LPdlg.DoModal()){
			m_pRefFoil->m_FoilColor = LPdlg.GetColor();
			m_pRefFoil->m_nFoilStyle = LPdlg.GetStyle();
			m_pRefFoil->m_nFoilWidth = LPdlg.GetWidth();
			SetSaveState(false);
		}
	}
	
	m_ctrlFoilColor.Invalidate();
	UpdateBlt();
}



void CAFoilCtrlBar::SetSaveState(bool bState) 
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	pADlg->SetSaveState(bState);
}

void CAFoilCtrlBar::OnSize(UINT nType, int cx, int cy) 
{
//
	CInitDialogBar::OnSize(nType, cx, cy);
	CRect ViewRect;
	CRect CltRect;
/*	CAFoil* pADlg = (CAFoil*)m_pADlg;
	CWnd* pChildView = pADlg->m_pChildWnd;

	pChildView->GetWindowRect(ViewRect);
	int w =ViewRect.Width()-30;*/

	GetClientRect(&CltRect);
//	CltRect.right = CltRect.left + w;
//	SetWindowPos(NULL,CltRect.left,CltRect.top,w,CltRect.Height(),SWP_NOSIZE);
	int width = CltRect.Width();
	if(width){
		ReSizeCtrls();
	}
}
void CAFoilCtrlBar::ReSizeCtrls() 
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	CRect CltRect, wndRect;
	GetClientRect(&CltRect);
	GetWindowRect(&wndRect);
	int w = CltRect.Width();
	if(w)
	{
		HWND hwnd = m_ctrlFoilList.GetSafeHwnd();
		if(hwnd)
		{
			m_ctrlDuplicate.SetWindowPos(NULL,CltRect.right-130,145,0,0,SWP_NOSIZE);
			m_ctrlDelete.SetWindowPos(NULL,CltRect.right-130,120,0,0,SWP_NOSIZE);
			m_ctrlRename.SetWindowPos(NULL,CltRect.right-130,95,0,0,SWP_NOSIZE);
			m_ctrlFoilColor.SetWindowPos(NULL,CltRect.right-130,65,0,0,SWP_NOSIZE);
			m_ctrlCenterLine.SetWindowPos(NULL,CltRect.right-130,45,0,0,SWP_NOSIZE);
			m_ctrlModelPoints.SetWindowPos(NULL,CltRect.right-130,30,0,0,SWP_NOSIZE);
			m_ctrlVisible.SetWindowPos(NULL,CltRect.right-130,15,0,0,SWP_NOSIZE);

/*			m_ctrlModelPoints.SetWindowPos(NULL,CltRect.right-130,30,0,0,SWP_NOSIZE);
			m_ctrlCenterLine.SetWindowPos(NULL,CltRect.right-130,45,0,0,SWP_NOSIZE);
			m_ctrlFoilColor.SetWindowPos(NULL,CltRect.right-130,65,0,0,SWP_NOSIZE);
			m_ctrlRename.SetWindowPos(NULL,CltRect.right-130,95,0,0,SWP_NOSIZE);
			m_ctrlDelete.SetWindowPos(NULL,CltRect.right-130,120,0,0,SWP_NOSIZE);
			m_ctrlDuplicate.SetWindowPos(NULL,CltRect.right-130,145,0,0,SWP_NOSIZE);*/
			CltRect.DeflateRect(10,10,150,0);
			w = CltRect.Width();
			int w2 = (int)(w*2/24);
			m_ctrlFoilList.MoveWindow(CltRect);
			m_ctrlFoilList.SetColumnWidth(0,w-8*w2-21);
			m_ctrlFoilList.SetColumnWidth(1,w2);
			m_ctrlFoilList.SetColumnWidth(2,w2);
			m_ctrlFoilList.SetColumnWidth(3,w2);
			m_ctrlFoilList.SetColumnWidth(4,w2);
			m_ctrlFoilList.SetColumnWidth(5,w2);
			m_ctrlFoilList.SetColumnWidth(6,w2);
			m_ctrlFoilList.SetColumnWidth(7,w2);
			m_ctrlFoilList.SetColumnWidth(8,w2);
			m_ctrlFoilList.SetColumnWidth(9,w2);
			m_ctrlFoilList.SetColumnWidth(10,w2);
			m_ctrlFoilList.SetColumnWidth(11,w2);
		}
	}
}



void CAFoilCtrlBar::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	CPoint pt;
	GetCursorPos(&pt);
	CMenu menu;
	if (menu.LoadMenu(IDR_CTXAFOILCTRLMENU))
	{
//		POSITION pos = m_ctrlFoilList.GetFirstSelectedItemPosition();
//		m_FoilSelection = m_ctrlFoilList.GetNextSelectedItem(pos);
		if(m_FoilSelection>0 && m_FoilSelection<=pADlg->m_poaFoil->GetSize())
		{
			m_pRefFoil = (CFoil*)pADlg->m_poaFoil->GetAt(m_FoilSelection-1);
			SetFoil();

			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	pt.x, pt.y, this); 
		}
	}
}



void CAFoilCtrlBar::OnRename()
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	pADlg->OnRenameFoil();
}

void CAFoilCtrlBar::OnDelete()
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	pADlg->OnDeleteFoil();
}

void CAFoilCtrlBar::OnDuplicate()
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	pADlg->OnDuplicate();
}

void CAFoilCtrlBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//override
//	CInitDialogBar::OnLButtonDblClk(nFlags, point);
}


void CAFoilCtrlBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	//override and disable window handler to prevent wild resize
//	CInitDialogBar::OnLButtonDown(nFlags, point);
}


void CAFoilCtrlBar::OnLvnItemchangedFoillist(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_ctrlFoilList.SetFocus();

	CAFoil* pADlg = (CAFoil*)m_pADlg;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem == -1 || pNMListView->iSubItem == -1)
	{
		*pResult =0;
		return ;
	}
	m_FoilSelection = pNMListView->iItem;
	if(m_FoilSelection==0)
	{
		SetSplineData();
	}
	else 
	{
		CFoil *pFoil;
		CString FoilName;
		FoilName = m_ctrlFoilList.GetItemText(pNMListView->iItem, 0);
		pFoil = pADlg->GetFoil(FoilName);
		SetFoil(pFoil);
	}

	*pResult = 0;
}


void CAFoilCtrlBar::FillFoilList()
{
	CAFoil* pADlg = (CAFoil*)m_pADlg;
	CFoil *pFoil;
	int i;
	m_ctrlFoilList.DeleteAllItems();

	if(pADlg->m_bSF)
	{
		m_ctrlFoilList.InsertItem(0, "Spline Foil");
	}
	else 
	{
		m_ctrlFoilList.InsertItem(0, "Spline Points Foil");
	}

	UpdateFoil(-1);

	for(i=0;i<pADlg->m_poaFoil->GetSize();i++)
	{
		pFoil = (CFoil*)pADlg->m_poaFoil->GetAt(i);
		m_ctrlFoilList.InsertItem(i+1,pFoil->m_FoilName);
		UpdateFoil(i);
	}

	if(pADlg->m_poaFoil->GetSize())
	{
		m_ctrlDelete.EnableWindow(true);
		m_ctrlRename.EnableWindow(true);
		m_ctrlDuplicate.EnableWindow(true);
	}
	else
	{
		m_ctrlDelete.EnableWindow(false);
		m_ctrlRename.EnableWindow(false);
		m_ctrlDuplicate.EnableWindow(false);
	}
}


void CAFoilCtrlBar::UpdateFoil(int iFoil)
{
	if(iFoil==-10) iFoil = m_FoilSelection;

	CAFoil* pADlg = (CAFoil*)m_pADlg;
	CFoil *pFoil = NULL;
	CString strong;
	CString percent = "%";
	double Thickness, xThickness, Camber, xCamber;
	Thickness = xThickness = Camber = xCamber = 0;
	int points = 0;

	if(pADlg->m_bSF && iFoil==-1)
	{
		if(!m_pSF) return;
		Thickness  = m_pSF->m_fThickness*100.0;
		xThickness = m_pSF->m_fxThickMax*100.0;
		Camber     = m_pSF->m_fCamber*100.0;
		xCamber    = m_pSF->m_fxCambMax*100.0;
		points     = m_pSF->m_OutPoints;
		iFoil = 0;
	}
	else if(!pADlg->m_bSF && iFoil==-1)
	{
		if(!m_pPF) return;
		Thickness  = m_pPF->m_fThickness*100.0;
		xThickness = m_pPF->m_fxThickMax*100.0;
		Camber     = m_pPF->m_fCamber*100.0;
		xCamber    = m_pPF->m_fxCambMax*100.0;
		points     =  (m_pPF->m_Extrados.m_iPoints)*(m_pPF->m_Extrados.m_Freq-1)
			         +(m_pPF->m_Intrados.m_iPoints)*(m_pPF->m_Intrados.m_Freq-1);//+1;
		iFoil = 0;
	}
	else if(iFoil>=0)
	{
		if(iFoil>=pADlg->m_poaFoil->GetSize()) return;

		pFoil = (CFoil*)pADlg->m_poaFoil->GetAt(iFoil);

		Thickness  = pFoil->m_fThickness*100.0;
		xThickness = pFoil->m_fXThickness*100.0;
		Camber     = pFoil->m_fCamber*100.0;
		xCamber    = pFoil->m_fXCamber*100.0;
		points     = pFoil->n;
		iFoil++;
	}

	strong.Format("%.2f", Thickness);
	strong+= percent;
	m_ctrlFoilList.SetItemText(iFoil,1,strong);

	strong.Format("%.1f", xThickness);
	strong+= percent;
	m_ctrlFoilList.SetItemText(iFoil,2,strong);

	strong.Format("%.2f", Camber);
	strong+= percent;
	m_ctrlFoilList.SetItemText(iFoil,3,strong);

	strong.Format("%.1f", xCamber);
	strong+= percent;
	m_ctrlFoilList.SetItemText(iFoil,4,strong);

	strong.Format("%d", points);
	m_ctrlFoilList.SetItemText(iFoil,5,strong);

	if(pFoil && pFoil->m_bTEFlap)
	{
		strong.Format("%.1f°", pFoil->m_TEFlapAngle);
		m_ctrlFoilList.SetItemText(iFoil,6,strong);

		strong.Format("%.1f", pFoil->m_TEXHinge);
		strong+= percent;
		m_ctrlFoilList.SetItemText(iFoil,7,strong);

		strong.Format("%.1f", pFoil->m_TEYHinge);
		strong+= percent;
		m_ctrlFoilList.SetItemText(iFoil,8,strong);
	}
	if(pFoil && pFoil->m_bLEFlap)
	{
		strong.Format("%.1f°", pFoil->m_LEFlapAngle);
		m_ctrlFoilList.SetItemText(iFoil,9,strong);

		strong.Format("%.1f", pFoil->m_LEXHinge);
		strong+= percent;
		m_ctrlFoilList.SetItemText(iFoil,10,strong);

		strong.Format("%.1f", pFoil->m_LEYHinge);
		strong+= percent;
		m_ctrlFoilList.SetItemText(iFoil,11,strong);
	}
}
