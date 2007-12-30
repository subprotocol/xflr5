/****************************************************************************

	CBodyCtrlBar class
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

// AFoilCtrl.cpp: implementation of the CBodyCtrlBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../misc/NameDlg.h"
#include "../misc/LinePickerDlg.h"
#include "../Design/SplinesCtrlDlg.h"
#include "../main/MainFrm.h"
#include ".\BodyCtrlBar.h"

 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CBodyCtrlBar, CInitDialogBar)

BEGIN_MESSAGE_MAP(CBodyCtrlBar, CInitDialogBar)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BODYCOLOR, OnBodyColor)
	ON_WM_DRAWITEM()
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEWHEEL()
	ON_NOTIFY(NM_CLICK, IDC_FRAMELIST, OnNMClickFrameList)
	ON_NOTIFY(NM_CLICK, IDC_FRAMEPOINTS, OnNMClickFramePoint)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_FRAMELIST, OnLvnEndLabelEditFrameList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_FRAMEPOINTS, OnLvnEndLabelEditFramePoints)
	ON_CBN_SELCHANGE(IDC_BODYLIST, OnSelChangeBodyList)
	ON_COMMAND(IDM_INSERTPOINT, OnInsertPoint)
	ON_COMMAND(IDM_DELETEPOINT, OnDeletePoint)
	ON_COMMAND(IDM_INSERTFRAME, OnInsertFrame)
	ON_COMMAND(IDM_DELETEFRAME, OnDeleteFrame)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio)
	ON_BN_CLICKED(IDC_CLOSEDSURFACE, OnClosedSurface)
	ON_CBN_SELCHANGE(IDC_SPLINEDEGREE, OnSelChangeSplineDegree)
	ON_EN_KILLFOCUS(IDC_NXPANELS, OnKillFocusPanels)
	ON_EN_KILLFOCUS(IDC_NHPANELS, OnKillFocusPanels)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBodyCtrlBar dialog



CBodyCtrlBar::CBodyCtrlBar(CWnd* pParent)
{
	//{{AFX_DATA_INIT(CBodyCtrlBar)

	//}}AFX_DATA_INIT
	m_poaBody  = NULL;
	m_ppCurBody = NULL;
	m_StackSize = 0; //the current stacksize
	m_StackPos    = 0; //the current position of the stack
}

CBodyCtrlBar::~CBodyCtrlBar()
{

}


void CBodyCtrlBar::DoDataExchange(CDataExchange* pDX)
{
	ASSERT(pDX);

	CInitDialogBar::DoDataExchange(pDX);

	// DDX_??? functions to associate control with
	// data or control object

	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_FRAMELIST, m_ctrlFrameList);
	DDX_Control(pDX, IDC_FRAMEPOINTS, m_ctrlFramePoints);
	DDX_Control(pDX, IDC_BODYCOLOR, m_ctrlBodyColor); 
	DDX_Control(pDX, IDC_BODYLIST, m_ctrlBodyList); 
	DDX_Control(pDX, IDC_CLOSEDSURFACE, m_ctrlClosedSurface);  
	DDX_Control(pDX, IDC_SPLINEDEGREE, m_ctrlSplineDegree);  
	DDX_Control(pDX, IDC_NXPANELS, m_ctrlnxPanels);  
	DDX_Control(pDX, IDC_NHPANELS, m_ctrlnhPanels);  
	DDX_Control(pDX, IDC_RADIO1, m_ctrlRadio1);  
	DDX_Control(pDX, IDC_RADIO2, m_ctrlRadio2);  
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CBodyCtrlBar message handlers


BOOL CBodyCtrlBar::OnInitDialogBar() 
{
	CInitDialogBar::OnInitDialogBar();
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pFrame = (CMainFrame*)(m_pMainFrame);
	CString str;
	int l,i;
	CBody *pBody;

	GetLengthUnit(str, pFrame->m_LengthUnit);
	str = " ("+str+")";

	m_ctrlFrameList.InsertColumn(0,"Frame",LVCFMT_LEFT, 50);
	m_ctrlFrameList.InsertColumn(1,"x"+str,LVCFMT_RIGHT,60);
	m_ctrlFrameList.InsertColumn(2,"z"+str,LVCFMT_RIGHT,60);
	m_ctrlFrameList.InsertColumn(3,"N",LVCFMT_RIGHT,50);
	m_ctrlFrameList.m_iPrecision[0] = 0;
	m_ctrlFrameList.m_iPrecision[1] = 2;
	m_ctrlFrameList.m_iPrecision[2] = 2;
	m_ctrlFrameList.m_iPrecision[3] = 0;
	m_ctrlFrameList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_ctrlFramePoints.InsertColumn(0,"Point",LVCFMT_LEFT, 50);
	m_ctrlFramePoints.InsertColumn(1,"y"+str,LVCFMT_RIGHT,60);
	m_ctrlFramePoints.InsertColumn(2,"z"+str,LVCFMT_RIGHT,60);
	m_ctrlFramePoints.InsertColumn(3,"N",LVCFMT_RIGHT,50);
	m_ctrlFramePoints.m_iPrecision[0] = 0;
	m_ctrlFramePoints.m_iPrecision[1] = 2;
	m_ctrlFramePoints.m_iPrecision[2] = 2;
	m_ctrlFramePoints.m_iPrecision[3] = 0;

	m_ctrlFramePoints.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_ctrlFramePoints.SetFocus();

	m_ctrlBodyList.ResetContent();
	if(m_poaBody->GetSize())
	{
		for (i=0; i<m_poaBody->GetSize(); i++)
		{
			pBody = (CBody*)m_poaBody->GetAt(i);
			m_ctrlBodyList.AddString(pBody->m_BodyName);
		}
		m_ctrlBodyList.SetCurSel(0);
	}

	for (i=1; i<6; i++){
		str.Format("%d", i);
		m_ctrlSplineDegree.AddString(str);
	}

	if(*m_ppCurBody)
	{
		m_ctrlFrameList.SetItemState((*m_ppCurBody)->m_iActiveFrame, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		l = (*m_ppCurBody)->m_iActiveFrame;
		m_ctrlFramePoints.SetItemState((*m_ppCurBody)->m_Frame[l].m_iSelect,
									LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_ctrlFramePoints.SetFocus();
		m_ctrlSplineDegree.SetCurSel((*m_ppCurBody)->m_nhDegree-1);
		if((*m_ppCurBody)->m_LineType == 1) m_ctrlSplineDegree.EnableWindow(false);
	}
	else
	{
		m_ctrlSplineDegree.SetCurSel(1);
		m_ctrlSplineDegree.EnableWindow(false);
	}

	FillFrameList();
	FillFrameCoord();

	return TRUE;
}



BOOL CBodyCtrlBar::PreTranslateMessage(MSG* pMsg) 
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pMainFrame = (CMainFrame*)(m_pMainFrame);
	CWnd* pWnd = GetFocus();

	SHORT sh1 = GetKeyState(VK_LCONTROL);
	SHORT sh2 = GetKeyState(VK_RCONTROL);

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		if (pWnd == this)	ShowWindow(SW_HIDE);
		else SetFocus();
	}

	else if(pMsg->message == WM_MOUSEWHEEL && pWnd==m_pChildView)
	{

		CPoint pt((short) LOWORD(pMsg->lParam),(short) HIWORD(pMsg->lParam));

		pMainFrame->OnMouseWheel(LOWORD(pMsg->wParam),(short) HIWORD(pMsg->wParam), pt);
		return true;
	}
	else if (pMsg->wParam == 'O' && 
			( (sh1 & 0x8000)||(sh2 & 0x8000) )) 
	{ 
		pMainFrame->OnLoadProject();
		return true;
	} 
	else if (pMsg->wParam == VK_RETURN)
	{
		TakePicture();
		StorePicture();
		(*m_ppCurBody)->m_nxPanels = m_ctrlnxPanels.GetValue();
		(*m_ppCurBody)->m_nhPanels = m_ctrlnhPanels.GetValue();
		if(pWnd==&m_ctrlnxPanels) m_ctrlnxPanels.SetSel(0,-1);
		if(pWnd==&m_ctrlnhPanels) m_ctrlnhPanels.SetSel(0,-1);
		pMainFrame->SetSaveState(false);
		pMiarex->m_bResetglMesh     = true;
		pMiarex->m_bResetglBodyMesh = true;
		pMiarex->UpdateView();

	}
	return CInitDialogBar::PreTranslateMessage(pMsg);
}


void CBodyCtrlBar::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	if (!pDC) return;
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;

	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	COLORREF color;
	int style, width;

	if(nIDCtl==IDC_BODYCOLOR )
	{
		if(*m_ppCurBody)
		{
			color = (*m_ppCurBody)->m_BodyColor;
			style = PS_SOLID;
			width = 1;
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
		GetDlgItem(IDC_BODYCOLOR)->GetClientRect(&SRect);
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

void CBodyCtrlBar::UpdateBodies()
{
	int i;
	CBody *pBody;

	m_ctrlBodyList.ResetContent();
	if(m_poaBody->GetSize())
	{
		for (i=0; i<m_poaBody->GetSize(); i++)
		{
			pBody = (CBody*)m_poaBody->GetAt(i);
			m_ctrlBodyList.AddString(pBody->m_BodyName);
		}
		m_ctrlBodyList.EnableWindow(true);
	}
	else m_ctrlBodyList.EnableWindow(false);

}

BOOL CBodyCtrlBar::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	ScreenToClient(&pt);
	return CWnd ::OnMouseWheel(nFlags, zDelta, pt);
}


void CBodyCtrlBar::OnBodyColor()
{
	if(!*m_ppCurBody) return;
	CMainFrame *pFrame = (CMainFrame*)m_pMainFrame;
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	CLinePickerDlg LPdlg;

	COLORREF color = (*m_ppCurBody)->m_BodyColor;
	int style      = (*m_ppCurBody)->m_BodyStyle;
	int width      = (*m_ppCurBody)->m_BodyWidth;
	if(IsBlackAndWhite()) GetBWColor(color, style, width);

	LPdlg.SetColor(color);
	LPdlg.SetStyle(style);
	LPdlg.SetWidth(width);

	if (IDOK==LPdlg.DoModal())
	{
		(*m_ppCurBody)->m_BodyColor = LPdlg.GetColor();
		(*m_ppCurBody)->m_BodyStyle = LPdlg.GetStyle();
		(*m_ppCurBody)->m_BodyWidth = LPdlg.GetWidth();
		pMiarex->m_bResetglBody   = true;
		pMiarex->m_bResetglBody2D = true;
		pFrame->SetSaveState(false);
	}

	m_ctrlBodyColor.Invalidate();

	pMiarex->UpdateView();
}


void CBodyCtrlBar::FillFrameList()
{
	CMainFrame *pFrame = (CMainFrame*)(m_pMainFrame);
	CString strong;
	m_ctrlFrameList.DeleteAllItems();

	if(!*m_ppCurBody) return;

	for (int i=0; i<(*m_ppCurBody)->m_NStation; i++)
	{
		strong.Format("%d", i+1);
		m_ctrlFrameList.InsertItem(i, strong);
		strong.Format(" %8.2f", (*m_ppCurBody)->m_FramePosition[i].x*pFrame->m_mtoUnit);
		m_ctrlFrameList.SetItemText(i,1,strong);
		strong.Format(" %8.2f", (*m_ppCurBody)->m_FramePosition[i].y*pFrame->m_mtoUnit);
		m_ctrlFrameList.SetItemText(i,2,strong);
		strong.Format(" %2d", 1);
		m_ctrlFrameList.SetItemText(i,3,strong);
	}
}

void CBodyCtrlBar::FillFrameCoord()
{
	CMainFrame *pFrame = (CMainFrame*)(m_pMainFrame);
	CString strong;
	m_ctrlFramePoints.DeleteAllItems();

	if(!*m_ppCurBody) return;
	int l = (*m_ppCurBody)->m_iActiveFrame;

	for (int i=0; i<(*m_ppCurBody)->m_NSideLines; i++)
	{
		strong.Format("%d", i+1);
		m_ctrlFramePoints.InsertItem(i, strong);
		strong.Format(" %8.2f", (*m_ppCurBody)->m_Frame[l].m_Point[i].y*pFrame->m_mtoUnit);
		m_ctrlFramePoints.SetItemText(i,1,strong);
		strong.Format(" %8.2f", (*m_ppCurBody)->m_Frame[l].m_Point[i].z*pFrame->m_mtoUnit);
		m_ctrlFramePoints.SetItemText(i,2,strong);
		strong.Format(" %2d", 1);
		m_ctrlFramePoints.SetItemText(i,3,strong);
	}
}


void CBodyCtrlBar::OnSize(UINT nType, int cx, int cy) 
{
//
	CInitDialogBar::OnSize(nType, cx, cy);
	CRect ViewRect;
	CRect CltRect;

	GetClientRect(&CltRect);
	int width = CltRect.Width();
	if(width){
		ReSizeCtrls();
	}
}


void CBodyCtrlBar::ReSizeCtrls() 
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pMainFrame = (CMainFrame*)m_pMainFrame;
	CRect CltRect, wndRect;
	GetClientRect(CltRect);
	GetWindowRect(wndRect);

	int w = CltRect.Width();

	if(w){
		HWND hwnd = m_ctrlFrameList.GetSafeHwnd();
		if(hwnd)
		{
//			m_ctrlFrameList.MoveWindow(CltRect.left+10, CltRect.top+40,260,100, TRUE);
//			m_ctrlFramePoints.SetWindowPos(NULL,250,30,500,0,SWP_NOSIZE);
		}
	}
}

void CBodyCtrlBar::OnNMClickFrameList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem == -1 || pNMListView->iSubItem == -1)
	{
		*pResult =0;
		return ;
	}
	SetFrame(pNMListView->iItem);
	pMiarex->UpdateView();

	*pResult = 0;
}


void CBodyCtrlBar::OnNMClickFramePoint(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem == -1 || pNMListView->iSubItem == -1)
	{
		*pResult =0;
		return ;
	}
	SetPoint(pNMListView->iItem);
	pMiarex->UpdateView();
	*pResult = 0;
}


void CBodyCtrlBar::ReadFrameData(int sel, double &x, double &z)
{
	double d;
	int res;
	CString strong;

	strong = m_ctrlFrameList.GetItemText(sel,1);
	strong.Replace(" ","");
	res = sscanf(strong, "%lf",&d);
	if(res==1) x=d;

	strong = m_ctrlFrameList.GetItemText(sel,2);
	res = sscanf(strong, "%lf",&d);
	if(res==1) z=d;
}


void CBodyCtrlBar::OnLvnEndLabelEditFramePoints(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;
	if(pDispInfo->item.iItem == -1 || pDispInfo->item.iSubItem == -1 )
	{
		*pResult =0;
		return ;
	}

	POSITION pos = m_ctrlFramePoints.GetFirstSelectedItemPosition();
	int iSelection = m_ctrlFramePoints.GetNextSelectedItem(pos);

	if(iSelection>=0)
	{
		double y,z;
		ReadPointData(iSelection, y, z);
		(*m_ppCurBody)->m_Frame[(*m_ppCurBody)->m_iActiveFrame].m_Point[iSelection].y = y/pFrame->m_mtoUnit;
		(*m_ppCurBody)->m_Frame[(*m_ppCurBody)->m_iActiveFrame].m_Point[iSelection].z = z/pFrame->m_mtoUnit;
		pMiarex->m_bResetglBody   = true;
		pMiarex->m_bResetglBody2D = true;
		pFrame->SetSaveState(false);
		pMiarex->UpdateView();
	}
}

void CBodyCtrlBar::OnLvnEndLabelEditFrameList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;
	if(pDispInfo->item.iItem == -1 || pDispInfo->item.iSubItem == -1 )
	{
		*pResult =0;
		return ;
	}

	POSITION pos = m_ctrlFrameList.GetFirstSelectedItemPosition();
	int iSelection = m_ctrlFrameList.GetNextSelectedItem(pos);

	if(iSelection>=0)
	{
		double x,z;
		ReadFrameData(iSelection, x, z);
		(*m_ppCurBody)->m_FramePosition[iSelection].x = x/pFrame->m_mtoUnit;
		(*m_ppCurBody)->m_FramePosition[iSelection].z = z/pFrame->m_mtoUnit;
		pMiarex->m_bResetglBody   = true;
		pMiarex->m_bResetglBody2D = true;
		pMiarex->UpdateView();
		pFrame->SetSaveState(false);
	}
}

void CBodyCtrlBar::ReadPointData(int sel, double &y, double &z)
{
	double d;
	int res;
	CString strong;

	strong = m_ctrlFramePoints.GetItemText(sel,1);
	strong.Replace(" ","");
	res = sscanf(strong, "%lf",&d);
	if(res==1) y=d;

	strong = m_ctrlFramePoints.GetItemText(sel,2);
	res = sscanf(strong, "%lf",&d);
	if(res==1) z=d;
}


void CBodyCtrlBar::OnSelChangeBodyList()
{
	// Gets the new selected body name and notifies Miarex
	// then updates Body combobox
	// and selects either the current Body
	// or the first one in the list, if any
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	pMiarex->m_bAnimate = false;

	CString strong;
	int sel = m_ctrlBodyList.GetCurSel();
	if (sel !=CB_ERR) m_ctrlBodyList.GetLBText(sel, strong);

	pMiarex->m_bResetglBody2D = true;
	pMiarex->SetBody(strong);
	pMiarex->UpdateView();
}

void CBodyCtrlBar::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pFrame = (CMainFrame*)(m_pMainFrame);
	CRect FrameRect, PointRect;
	m_ctrlFrameList.GetWindowRect(&FrameRect);
	m_ctrlFramePoints.GetWindowRect(&PointRect);
	CMenu menu;
	int FrameSel, PointSel;

	if(FrameRect.PtInRect(point))
	{
		if (menu.LoadMenu(IDR_CTXBODYFRAMECTRLMENU))
		{
			POSITION pos = m_ctrlFrameList.GetFirstSelectedItemPosition();
			FrameSel = m_ctrlFrameList.GetNextSelectedItem(pos);
			if(FrameSel>=0 && FrameSel<(*m_ppCurBody)->m_NStation)
			{
				SetFrame(FrameSel);

				CMenu* pPopup = menu.GetSubMenu(0);
				ASSERT(pPopup != NULL);
				pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	point.x, point.y, this); 
			}
		}
	}
	else if(PointRect.PtInRect(point))
	{
		if (menu.LoadMenu(IDR_CTXBODYPOINTCTRLMENU))
		{
			POSITION pos = m_ctrlFramePoints.GetFirstSelectedItemPosition();
			PointSel = m_ctrlFramePoints.GetNextSelectedItem(pos);
			if(PointSel>=0 && PointSel<(*m_ppCurBody)->m_NSideLines)
			{
				SetPoint(PointSel);

				CMenu* pPopup = menu.GetSubMenu(0);
				ASSERT(pPopup != NULL);
				pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	point.x, point.y, this); 
			}
		}
	}
}

void CBodyCtrlBar::SetBody(CBody *pBody)
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pFrame = (CMainFrame*)(m_pMainFrame);
	CString strong;

	m_StackSize = 0;
	m_StackPos = 0;

	*m_ppCurBody = pBody;
	if(!pBody) 
	{
		if(m_ctrlBodyList.GetCount())
		{
			m_ctrlBodyList.SetCurSel(0);
			m_ctrlBodyList.GetLBText(0, strong);
			pBody = pMiarex->GetBody(strong);
			*m_ppCurBody = pBody;
			pMiarex->m_pCurBody = pBody;
			pMiarex->m_bResetglBody   = true;
			pMiarex->m_bResetglBody2D = true;
		}
		if(!pBody)
		{
			*m_ppCurBody = NULL;
			pMiarex->m_pCurBody = NULL;
			pMiarex->m_pCurFrame = NULL;

			UpdateBodyCtrls();
			return;
		}
	}
	int pos = m_ctrlBodyList.FindStringExact(0, pBody->m_BodyName);
	if (pos!=CB_ERR)
	{
		m_ctrlBodyList.SetCurSel(pos);
	}
	pBody->SetKnots();
	UpdateBodyCtrls();
}


void CBodyCtrlBar::UpdateBodyCtrls()
{
	if(!*m_ppCurBody) 
	{
		FillFrameList();
		FillFrameCoord();
		m_ctrlRadio1.EnableWindow(false);
		m_ctrlRadio2.EnableWindow(false);
		m_ctrlBodyList.EnableWindow(false);
		m_ctrlBodyColor.EnableWindow(false);
		m_ctrlClosedSurface.EnableWindow(false);
		m_ctrlFrameList.EnableWindow(false);
		m_ctrlFramePoints.EnableWindow(false);
		m_ctrlnhPanels.EnableWindow(false);
		m_ctrlnxPanels.EnableWindow(false);
		m_ctrlSplineDegree.EnableWindow(false);
		return;
	}
	else
	{
		FillFrameList();
		SetFrameSel((*m_ppCurBody)->m_iActiveFrame);
		SetFrame((*m_ppCurBody)->m_iActiveFrame);
		SetPointSel((*m_ppCurBody)->m_Frame[(*m_ppCurBody)->m_iActiveFrame].m_iSelect);

		if((*m_ppCurBody)->m_LineType==1) CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
		if((*m_ppCurBody)->m_LineType==2) CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);

		if((*m_ppCurBody)->m_bClosedSurface) m_ctrlClosedSurface.SetCheck(TRUE);
		else                             m_ctrlClosedSurface.SetCheck(FALSE);

		m_ctrlSplineDegree.SetCurSel((*m_ppCurBody)->m_nhDegree-1);
		if((*m_ppCurBody)->m_LineType==1) m_ctrlSplineDegree.EnableWindow(false);
		else                          m_ctrlSplineDegree.EnableWindow(true);
	
		m_ctrlnxPanels.SetValue((*m_ppCurBody)->m_nxPanels);
		m_ctrlnhPanels.SetValue((*m_ppCurBody)->m_nhPanels);

		m_ctrlRadio1.EnableWindow(true);
		m_ctrlRadio2.EnableWindow(true);
		m_ctrlBodyList.EnableWindow(true);
		m_ctrlBodyColor.EnableWindow(true);
		m_ctrlClosedSurface.EnableWindow(true);
		m_ctrlFrameList.EnableWindow(true);
		m_ctrlFramePoints.EnableWindow(true);
		m_ctrlnhPanels.EnableWindow(true);
		m_ctrlnxPanels.EnableWindow(true);
		m_ctrlSplineDegree.EnableWindow(true);
	}
}

void CBodyCtrlBar::SetFrame(int iFrame)
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	pMiarex->m_pCurFrame = &(*m_ppCurBody)->m_Frame[iFrame];
	(*m_ppCurBody)->m_iActiveFrame = iFrame;

	FillFrameCoord();
	SetPointSel(pMiarex->m_pCurFrame->m_iSelect);

	pMiarex->m_bResetglBody2D = true;
}

void CBodyCtrlBar::SetPoint(int iPoint)
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	int iFrame = (*m_ppCurBody)->m_iActiveFrame;
	(*m_ppCurBody)->m_Frame[iFrame].m_iSelect= iPoint;
	pMiarex->m_bResetglBody2D = true;
}

void CBodyCtrlBar::SetFrameSel(int sel)
{
	if(sel>=0){
		m_ctrlFrameList.EnsureVisible(sel,FALSE);
		m_ctrlFrameList.SetItemState(sel, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_ctrlFrameList.m_iItem = sel;
	}
}


void CBodyCtrlBar::SetPointSel(int sel)
{
	if(sel>=0){
		m_ctrlFramePoints.EnsureVisible(sel,FALSE);
		m_ctrlFramePoints.SetItemState(sel, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_ctrlFramePoints.m_iItem = sel;
	}
}


void CBodyCtrlBar::OnInsertFrame()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pFrame = (CMainFrame*)(m_pMainFrame);
	pFrame->SetSaveState(false);
	CVector Real;

	TakePicture();
	StorePicture();

	POSITION pos = m_ctrlFrameList.GetFirstSelectedItemPosition();
	int FrameSel = m_ctrlFrameList.GetNextSelectedItem(pos);
	if(FrameSel>=0 && FrameSel<(*m_ppCurBody)->m_NStation)
	{
		if(FrameSel==0) FrameSel++;
		Real.x = ((*m_ppCurBody)->m_FramePosition[FrameSel].x + (*m_ppCurBody)->m_FramePosition[FrameSel-1].x)/2.0;
		Real.z = ((*m_ppCurBody)->m_FramePosition[FrameSel].z + (*m_ppCurBody)->m_FramePosition[FrameSel-1].z)/2.0;
		(*m_ppCurBody)->InsertFrame(Real);
	}
	FillFrameList();
	SetFrame(FrameSel);
	pMiarex->UpdateView();
}

void CBodyCtrlBar::OnDeleteFrame()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pFrame = (CMainFrame*)(m_pMainFrame);
	pFrame->SetSaveState(false);
	CVector Real;

	TakePicture();
	StorePicture();

	POSITION pos = m_ctrlFrameList.GetFirstSelectedItemPosition();
	int FrameSel = m_ctrlFrameList.GetNextSelectedItem(pos);
	if(FrameSel>=0 && FrameSel<(*m_ppCurBody)->m_NStation)
	{
		(*m_ppCurBody)->RemoveFrame(FrameSel);
	}
	FillFrameList();
	SetFrame(FrameSel);
	pMiarex->UpdateView();}



void CBodyCtrlBar::OnInsertPoint()
{
	CVector Real;
	
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pFrame = (CMainFrame*)(m_pMainFrame);
	pFrame->SetSaveState(false);

	TakePicture();
	StorePicture();

	POSITION pos = m_ctrlFramePoints.GetFirstSelectedItemPosition();
	int FramePoint = m_ctrlFramePoints.GetNextSelectedItem(pos);
	if(FramePoint>=0 && FramePoint<(*m_ppCurBody)->m_NSideLines)
	{
		if(FramePoint==0)	FramePoint++;
		(*m_ppCurBody)->InsertSideLine(FramePoint);
	}
	FillFrameCoord();
	SetPoint(FramePoint);
	pMiarex->UpdateView();
}

void CBodyCtrlBar::OnDeletePoint()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pFrame = (CMainFrame*)(m_pMainFrame);
	CVector Real;
	pFrame->SetSaveState(false);

	TakePicture();
	StorePicture();

	POSITION pos = m_ctrlFramePoints.GetFirstSelectedItemPosition();
	int FramePoint = m_ctrlFramePoints.GetNextSelectedItem(pos);
	if(FramePoint>=0 && FramePoint<(*m_ppCurBody)->m_NSideLines)
	{
		if(FramePoint==0)	FramePoint++;
		(*m_ppCurBody)->RemoveSideLine(FramePoint);
	}
	FillFrameCoord();
	SetPoint(FramePoint);
	pMiarex->UpdateView();
}


void CBodyCtrlBar::OnRadio()
{
	CMainFrame *pFrame = (CMainFrame*)(m_pMainFrame);
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(*m_ppCurBody)
	{

		TakePicture();
		StorePicture();

		if(GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO4)==IDC_RADIO1)
		{
			(*m_ppCurBody)->m_LineType = 1;
		}
		if(GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO2)==IDC_RADIO2)
		{
			(*m_ppCurBody)->m_LineType = 2;
		}
		m_ctrlSplineDegree.SetCurSel((*m_ppCurBody)->m_nhDegree-1);
		if((*m_ppCurBody)->m_LineType==1) m_ctrlSplineDegree.EnableWindow(false);
		else                          m_ctrlSplineDegree.EnableWindow(true);
	}

	pFrame->SetSaveState(false);

	pMiarex->m_bResetglBody   = true;
	pMiarex->m_bResetglBody2D = true;
	pMiarex->UpdateView();
}

void CBodyCtrlBar::OnClosedSurface()
{
	CMainFrame *pFrame = (CMainFrame*)(m_pMainFrame);
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(*m_ppCurBody)
	{

		TakePicture();
		StorePicture();

		if(m_ctrlClosedSurface.GetCheck()) (*m_ppCurBody)->m_bClosedSurface = true;
		else                               (*m_ppCurBody)->m_bClosedSurface = false;
	}
	pFrame->SetSaveState(false);
	pMiarex->m_bResetglBody   = true;
	pMiarex->m_bResetglBody2D = true;
	pMiarex->UpdateView();
}


void CBodyCtrlBar::OnSelChangeSplineDegree()
{
	CWaitCursor wait;
	CMainFrame *pMainFrame = (CMainFrame*)m_pMainFrame;
	pMainFrame->SetSaveState(false);
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	if(*m_ppCurBody) 
	{

		TakePicture();
		StorePicture();

		(*m_ppCurBody)->m_nhDegree = m_ctrlSplineDegree.GetCurSel()+1;
		(*m_ppCurBody)->m_nxDegree = m_ctrlSplineDegree.GetCurSel()+1;
		(*m_ppCurBody)->SetKnots();
		pMiarex->m_bResetglBody   = true;
		pMiarex->m_bResetglBody2D = true;
		pMiarex->UpdateView();
	}
}

void CBodyCtrlBar::OnKillFocusPanels()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pMainFrame = (CMainFrame*)m_pMainFrame;
	if(*m_ppCurBody)
	{
		TakePicture();
		StorePicture();
		(*m_ppCurBody)->m_nxPanels = m_ctrlnxPanels.GetValue();
		(*m_ppCurBody)->m_nhPanels = m_ctrlnhPanels.GetValue();
		pMainFrame->SetSaveState(false);
		pMiarex->m_bResetglBodyMesh = true;
		pMiarex->m_bResetglMesh = true;
		pMiarex->UpdateView();
	}
}

void CBodyCtrlBar::TakePicture()
{
	m_bStored = false;
	m_TmpPic.Duplicate(*m_ppCurBody);
}

void CBodyCtrlBar::StorePicture()
{
	int i;
	if(m_StackPos>=20){
		for (i=1; i<20; i++)
		{
			m_UndoPic[i-1].Duplicate(m_UndoPic+i);
		}
		m_StackPos = 19;
		m_StackSize = 19;
	}
	m_UndoPic[m_StackPos].Duplicate(&m_TmpPic);

	m_bStored = true;
	m_StackPos++;
	m_StackSize = m_StackPos;

//	CToolBarCtrl *pTB = &(m_pAFoilBar->GetToolBarCtrl());
//	pTB->EnableButton(IDT_UNDO, true);
}

void CBodyCtrlBar::SetPicture()
{
	CMiarex* pMiarex = (CMiarex*)m_pMiarex;
	CMainFrame *pFrame = (CMainFrame*)(m_pMainFrame);

	(*m_ppCurBody)->Duplicate(m_UndoPic+m_StackPos);

	UpdateBodyCtrls();

	pMiarex->m_bResetglBody   = true;
	pMiarex->m_bResetglBody2D = true;
	
	pMiarex->UpdateView();
}


void CBodyCtrlBar::OnUndo() 
{
	CWaitCursor wait;
	if(m_StackPos>0) {
		if(m_StackPos == m_StackSize)
		{
			//if we're at the first undo command, save current state
			TakePicture();
			StorePicture();//in case we redo
			m_StackPos--;
		}
		m_StackPos--;
		SetPicture();
//		CToolBarCtrl *pTB = &(m_pAFoilBar->GetToolBarCtrl());
//		if(m_StackPos==0)	pTB->EnableButton(IDT_UNDO, false);
//		pTB->EnableButton(IDT_REDO, true);
	}
	else m_StackPos = 0;
}

void CBodyCtrlBar::OnRedo() 
{
	CWaitCursor wait;
	if(m_StackPos<m_StackSize-1)
	{
		m_StackPos++;
		SetPicture();
//		CToolBarCtrl *pTB = &(m_pAFoilBar->GetToolBarCtrl());
//		pTB->EnableButton(IDT_UNDO, true);
//		if(m_StackPos==m_StackSize-1) 	pTB->EnableButton(IDT_REDO, false);
		
	}
}
