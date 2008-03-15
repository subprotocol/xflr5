
/****************************************************************************

	Inverse dialog class

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
/////////////////////////////////////////////////////////////////////////////
// XInverse.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "../main/ChildView.h"
#include "../Graph/GraphDlg.h"
#include "../misc/PrintOptionsDlg.h"
#include "../misc/NameDlg.h"
#include "Math.h"
#include "PertDlg.h"
#include "SelectFoilDlg.h"
#include ".\xinverse.h"



CXInverse::CXInverse()
{
	m_bTransGraph = false;

	m_hcArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_hcCross = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
	m_hcMove  = AfxGetApp()->LoadCursor(IDC_HMOVE);

	m_bLoaded        = false;
	m_bSaved         = true;
	m_bZoomPlus      = false;
	m_bShowPoints    = false;
	m_bRefCurves     = false;
	m_bTangentSpline = true;
	m_bReflected     = false;

	m_pRefFoil = new CFoil();
	m_pModFoil = new CFoil();
	m_pRefFoil->m_FoilColor = RGB(255,0,0);
	m_pRefFoil->m_nFoilStyle = PS_SOLID;
	m_pRefFoil->m_nFoilWidth = 0;
	m_pModFoil->m_FoilColor = RGB(0,0,255);
	m_pModFoil->m_nFoilStyle = PS_SOLID;
	m_pModFoil->m_nFoilWidth = 1;

	m_Spline.InsertPoint(0.0,0.0);
	m_Spline.InsertPoint(0.25,0.0);
	m_Spline.InsertPoint(0.5f,0.0);
	m_Spline.InsertPoint(0.75f,0.0);
	m_Spline.InsertPoint(1.0,0.0);
	m_Spline.SplineKnots();
	m_Spline.SplineCurve();

	m_SplineStyle = 0;
	m_SplineWidth = 1;
	m_SplineClr = RGB(170,120, 0);

	m_ReflectedStyle = PS_DASH;
	m_ReflectedWidth = 1;
	m_ReflectedClr = RGB(170,120, 0);
	
	m_nPos    = 0;
	m_tmpPos  = -1;
	m_Pos1    = -1;
	m_Pos2    = -1;
	m_SplineLeftPos   = -1;
	m_SplineRightPos  = -1;

	m_QGraph.SetType(2);
	m_QGraph.SetDefaults();
	m_QGraph.SetXTitle("x/c");
	m_QGraph.SetYTitle("Q/Vinf");
	m_QGraph.SetXMin(0.0);
	m_QGraph.SetXMax(1.0);
	m_QGraph.SetYMin(-0.1);
	m_QGraph.SetYMax(0.1);
	m_pQCurve  = m_QGraph.AddCurve();
	m_pMCurve  = m_QGraph.AddCurve();
	m_pQVCurve = m_QGraph.AddCurve();
	m_pReflectedCurve = m_QGraph.AddCurve();
	m_pReflectedCurve->SetVisible(m_bReflected);
}

CXInverse::~CXInverse()
{
	delete m_pRefFoil;
	delete m_pModFoil;
}


BEGIN_MESSAGE_MAP(CXInverse, CWnd)
	//{{AFX_MSG_MAP(CXInverse)
	ON_COMMAND(IDT_ZOOMIN, OnZoomIn)
	ON_COMMAND(IDT_ZOOMLESS, OnZoomLess)
	ON_COMMAND(IDM_GRAPHOPTIONS, OnGraphOptions)
	ON_COMMAND(IDM_RESETGRAPH, OnResetGraph)
	ON_COMMAND(IDM_PRINT, OnPrint)
	ON_COMMAND(IDM_QINIT, OnQInit)
	ON_COMMAND(IDM_QMOD, OnQMod)
	ON_COMMAND(IDM_EXPORTFOIL, OnExportFoil)
	ON_COMMAND(IDM_QVISC, OnQVisc)
	ON_COMMAND(IDM_COLORSTYLES, OnColorStyles)
	ON_COMMAND(IDM_ADDPOINT, OnAddPoint)
	ON_COMMAND(IDM_DELETEPOINT, OnDeletePoint)
	ON_COMMAND(IDM_SHOWREF, OnShowRef)
	ON_COMMAND(IDM_SHOWMOD, OnShowMod)
	ON_COMMAND(IDM_INVERSERESTORE, OnRestore)
	ON_COMMAND(IDM_STOREFOIL, OnStoreFoil)
	ON_COMMAND(ID_APP_ABOUT, OnAbout)
	ON_COMMAND(IDM_SELECTFOIL, OnSelectFoil)
	ON_COMMAND(IDM_RESETFOIL, OnResetFoil)
	ON_COMMAND(IDM_LOAD, OnLoad)
	ON_COMMAND(IDM_RESETQGRAPH, OnResetGraph)
	ON_COMMAND(IDM_SHOWQPOINTS, OnShowPoints)
	ON_COMMAND(IDM_DELETESPOINT, OnDeletePoint)
	ON_COMMAND(IDM_INSERTSPOINT, OnAddPoint)
	ON_COMMAND(IDM_EXPORTREFFOIL, OnExportFoil)
	ON_COMMAND(IDC_STOREFOIL, OnStoreFoil)
	ON_COMMAND(IDT_LOAD, OnLoadFoil)
//	ON_COMMAND(IDM_RESETREFCURVES, OnResetRefCurves)
//	ON_COMMAND(IDM_SHOWREFCURVES, OnShowRefCurves)
//	ON_COMMAND(IDM_STOREREFCURVE, OnStoreRefCurve)
	//}}AFX_MSG_MAP
	ON_COMMAND(IDM_SHOWREFLECTED, OnShowReflected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXInverse message handlers
void CXInverse::OnAbout() 
{
	((CXFLR5App*)AfxGetApp())->OnAppAbout();
}

bool CXInverse::SetParams() 
{
	CMenu *pMenu = m_pFrame->GetMenu();

	pMenu->GetSubMenu(1)->CheckMenuItem(IDM_BSPLINES,      MF_BYCOMMAND | MF_UNCHECKED);
	pMenu->GetSubMenu(1)->CheckMenuItem(IDM_SPLINEDPOINTS, MF_BYCOMMAND | MF_UNCHECKED);
	if(m_bFullInverse){
		pMenu->GetSubMenu(1)->CheckMenuItem(IDM_MIXEDINVERSE,  MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->GetSubMenu(1)->CheckMenuItem(IDM_FULLINVERSE,   MF_BYCOMMAND | MF_CHECKED);
	}
	else{
		pMenu->GetSubMenu(1)->CheckMenuItem(IDM_MIXEDINVERSE,  MF_BYCOMMAND | MF_CHECKED);
		pMenu->GetSubMenu(1)->CheckMenuItem(IDM_FULLINVERSE,   MF_BYCOMMAND | MF_UNCHECKED);
	}
	pMenu->GetSubMenu(1)->CheckMenuItem(IDM_XFLR5,         MF_BYCOMMAND | MF_UNCHECKED);
	pMenu->GetSubMenu(1)->CheckMenuItem(IDM_MIAREX,        MF_BYCOMMAND | MF_UNCHECKED);

	m_pQCurve->SetColor(m_pRefFoil->m_FoilColor);
	m_pQCurve->SetStyle(m_pRefFoil->m_nFoilStyle);
	m_pQCurve->SetWidth(m_pRefFoil->m_nFoilWidth);
	m_pMCurve->SetColor(m_pModFoil->m_FoilColor);
	m_pMCurve->SetStyle(m_pModFoil->m_nFoilStyle);
	m_pMCurve->SetWidth(m_pModFoil->m_nFoilWidth);
	m_pQCurve->SetTitle("Q - Reference");
	m_pMCurve->SetTitle("Q - Specification");
	m_pQVCurve->SetTitle("Q - Viscous");
	m_pQVCurve->SetColor(RGB(50,170,0));
	m_pQVCurve->SetStyle(PS_SOLID);

	m_pReflectedCurve->SetColor(m_ReflectedClr);
	m_pReflectedCurve->SetStyle(m_ReflectedStyle);
	m_pReflectedCurve->SetWidth(m_ReflectedWidth);
	m_pReflectedCurve->SetTitle("Reflected");

	m_bTrans   = false;
	m_bSpline  = false;
	m_bSplined = true;
	m_bRefFoil = true;
	m_bModFoil = false;
	m_bGetPos  = false;
	m_bMark    = false;
	m_bMarked  = false;
	m_bSmooth  = false; 

	m_QGraph.SetDrawRect(m_rGraphRect);
	m_QGraph.Init();
	m_pQCurve->SetVisible(true);

	m_pFInvCtrlBar->CheckRadioButton(IDC_SPECAL, IDC_SPECCL, IDC_SPECAL);
	m_pFInvCtrlBar->OnSpecal();

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CFoil*pFoil;
	//is a foil set as current in the mainframe ?
	if (pFrame->m_pCurFoil
		&& m_pXFoil->m_FoilName==pFrame->m_pCurFoil->m_FoilName && m_pXFoil->lqspec){
		m_pRefFoil->CopyFoil(pFrame->m_pCurFoil);
		m_pRefFoil->m_FoilColor = m_pQCurve->GetColor();
//		m_pXFoil->m_FoilName    = m_pRefFoil->m_FoilName ;
//		InitXFoil(m_pRefFoil);

	}
	else if(!m_pXFoil->m_FoilName.GetLength()){
		// XFoil is not initialized
		//is there anything in the database ?
		if(pFrame->m_oaFoil.GetSize()){
			pFoil = (CFoil*)pFrame->m_oaFoil.GetAt(0);
			m_pRefFoil->CopyFoil(pFoil);
			m_pRefFoil->m_FoilColor = m_pQCurve->GetColor();
			m_pXFoil->m_FoilName    = m_pRefFoil->m_FoilName ;
			InitXFoil(m_pRefFoil);
		}
		else{//nothing to initialize
			if(m_bFullInverse){
				m_pFInvCtrlBar->m_ctrlSpec.SetValue(0.0);
				m_pFInvCtrlBar->m_ctrlTAngle.SetValue(0.0);
				m_pFInvCtrlBar->m_ctrlTGapx.SetValue(0.0);
				m_pFInvCtrlBar->m_ctrlTGapy.SetValue(0.0);
			}
			else{
				m_pMInvCtrlBar->m_ctrlIter.SetValue(m_pXFoil->niterq);
			}
			CheckMenu();
			Clear();
			return false;// 
		}
	}


	//XFOIL has already been initialized so retrieve the foil
	for (int i=1; i<=m_pXFoil->n; i++){
		m_pRefFoil->x[i-1]  = m_pXFoil->x[i];
		m_pRefFoil->y[i-1]  = m_pXFoil->y[i];
		m_pRefFoil->xb[i-1] = m_pXFoil->x[i];
		m_pRefFoil->yb[i-1] = m_pXFoil->y[i];
	}
	m_pRefFoil->n          = m_pXFoil->n;
	m_pRefFoil->nb         = m_pXFoil->n;
	m_pRefFoil->m_FoilName = m_pXFoil->m_FoilName;
	m_pRefFoil->InitFoil();
	m_pModFoil->m_FoilName = m_pXFoil->m_FoilName + " Modified";

	SetFoil();
//	if(m_bFullInverse)  ResetQ();
//	else				ResetMixedQ();

	CheckMenu();

	return true;
}


void CXInverse::Clear() 
{
	m_pRefFoil->n = 0;
	m_pRefFoil->m_FoilName = "";
	m_pModFoil->m_FoilName = "";
	m_bLoaded = false;
	m_pReflectedCurve->ResetCurve();
	m_pMCurve->ResetCurve();
	m_pQCurve->ResetCurve();
	m_pQVCurve->ResetCurve();
}

void CXInverse::OnShowWindow(BOOL bShow, UINT nStatus) 
{	
	if(bShow) {

		CToolBarCtrl *pTB = &(m_pXInverseBar->GetToolBarCtrl());
		pTB->EnableButton(IDT_UNDO, false);
		pTB->EnableButton(IDT_REDO, false);

		UpdateView();
	}
	else {
		m_bMark     = false;
		m_bMarked   = false;
		m_bGetPos   = false;
		m_bSpline   = false;
		m_bSplined  = false;
		m_bSmooth   = false;
		m_bZoomPlus = false;
		m_bTrans    = false;
	}
}


BOOL CXInverse::PreTranslateMessage(MSG* pMsg)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if (pMsg->message == WM_KEYDOWN){
		if (pMsg->wParam == VK_ESCAPE){
			if(m_bZoomPlus)	{
				ReleaseZoom();
				return true; // no need to process further
			}
			else if(m_bGetPos){
				m_bGetPos = false; 
				m_bSpline = false;
				m_bSmooth = false;
				if(m_bFullInverse)	{
					m_pFInvCtrlBar->CancelSpline();
					m_pFInvCtrlBar->CancelSmooth();
				}
				else {
					m_pMInvCtrlBar->CancelSpline();
					m_pMInvCtrlBar->CancelSmooth();
				}
				UpdateView();
				return true;
			}
		}
		else if (pMsg->wParam == VK_RETURN){
			CWnd* pWnd = GetFocus();
			if (m_bSmooth) {
				Smooth(-1);
				m_bGetPos = false;
			}
			else {
				if(m_bFullInverse){
                    if(pFrame->m_FInvCtrlBar.IsWindowVisible())
						pFrame->m_FInvCtrlBar.m_ctrlExec.SetFocus();
				}
				else {
                    if(pFrame->m_MInvCtrlBar.IsWindowVisible())
						pFrame->m_MInvCtrlBar.m_ctrlExec.SetFocus();
				}
				return true;
			}	
		}
		else if (pMsg->wParam == 'O' && 
			(GetAsyncKeyState(VK_LCONTROL) || GetAsyncKeyState(VK_RCONTROL))) { 
			OnLoad();
			return true;
		} 
		else if (pMsg->wParam == 'P' && 
			(GetAsyncKeyState(VK_LCONTROL) || GetAsyncKeyState(VK_RCONTROL))) { 
			OnPrint();
			return true;
		}
		else if (pMsg->wParam == 'Z') { 
			return true;//User is zooming with 'Z' key instead of mouse midle button
		} 
		else if (pMsg->wParam == 'G') {
			OnGraphOptions();
			return true;
		} 
	}
	return CWnd::PreTranslateMessage(pMsg);
}


void CXInverse::OnZoomIn() 
{
	CToolBarCtrl *pTB = &(m_pXInverseBar->GetToolBarCtrl());

	if(!m_bZoomPlus){
		m_bZoomPlus = true;
		pTB->PressButton(IDT_ZOOMIN, true); 
	}
	else {
		ReleaseZoom();
	}
}

void CXInverse::OnLButtonDblClk(UINT nFlags, CPoint point)
{
//	point is in client coordinates

	if (m_QGraph.IsInDrawRect(point)){
		CGraphDlg dlg(m_pChildWnd);

		dlg.m_pGraph = &m_QGraph;

		if(IDOK == dlg.DoModal()){
			UpdateView();
		}
	}	
}

void CXInverse::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int CtrlPt;
	CPoint pttmp;

	if(!m_bGetPos)
	{
		m_PointDown.x = point.x;
		m_PointDown.y = point.y;
		pttmp.SetPoint(point.x, point.y);

		if(m_QGraph.IsInDrawRect(pttmp))
		{
			m_bTransGraph = true;
			SetCursor(m_hcMove);
			xd = m_QGraph.ClientTox(point.x);
			yd = m_QGraph.ClientToy(point.y);
			if(m_bSpline){
				CtrlPt = m_Spline.IsControlPoint(xd, yd,
												m_QGraph.GetXScale(),
												m_QGraph.GetYScale());
				if(CtrlPt<0) m_Spline.m_iSelect = -1;
				else {
					m_Spline.m_iSelect = CtrlPt;
//					return;
				}
				SHORT sh1 = GetKeyState(VK_LCONTROL);
				SHORT sh2 = GetKeyState(VK_RCONTROL);
				SHORT sh3 = GetKeyState(VK_SHIFT);
				if ((sh1 & 0x8000) || (sh2 & 0x8000))
				{
					if(CtrlPt>=0) {
						if (m_Spline.m_iSelect>=0) {
							m_Spline.RemovePoint(m_Spline.m_iSelect);
							m_Spline.SplineKnots();
							m_Spline.SplineCurve();
						}
					}
				}
				else if (sh3 & 0x8000) 
				{
					m_Spline.InsertPoint(xd,yd);
					m_Spline.SplineKnots();
					m_Spline.SplineCurve();
				}
				if(CtrlPt>=0) return;
			}
		}
		else m_bTransGraph = false;

		if(m_bZoomPlus && m_QGraph.IsInDrawRect(point)){
			m_ZoomRect.left   = point.x;
			m_ZoomRect.top    = point.y;
			m_ZoomRect.right  = point.x;
			m_ZoomRect.bottom = point.y;
			return;
		}
		else if(m_bZoomPlus && !m_QGraph.IsInDrawRect(point)){
			ReleaseZoom();
		}
		else {
			SetCursor(m_hcMove);
			m_bTrans = true;
			m_bZoomPlus = false;
		}
	}	
}


void CXInverse::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bTrans = false;

	int tmp, width, height;
	double x1,x2,w,h,xw,yh,xm,ym;
	double xmin, ymin, xmax, ymax;
	double ratio,x, y, ux, uy, xpt, ypt, norm;

	if(m_bZoomPlus && m_rCltRect.PtInRect(point))
	{
		CRect ZRect(&m_ZoomRect);
		ZRect.NormalizeRect();
		if (!ZRect.IsRectEmpty() ){

			xu = m_QGraph.ClientTox(point.x);
			yu = m_QGraph.ClientToy(point.y);

			width  = abs(m_PointDown.x-point.x);
			height = abs(m_PointDown.y-point.y);
			//preserve ratio
			w = abs(xu-xd);
			h = abs(yu-yd);
			xw = 	m_QGraph.GetXMax() - m_QGraph.GetXMin();
			yh = 	m_QGraph.GetYMax() - m_QGraph.GetYMin();
			xm = (xu+xd)/2.0;
			ym = (yu+yd)/2.0;

			if(width>=height){
				xmin  = xm - w/2.0;
				xmax  = xm + w/2.0;
				ratio = w/xw;

				ymin  = ym - ratio*yh/2.0;
				ymax  = ym + ratio*yh/2.0;
			}
			else {
				ymin  = ym - h/2.0;
				ymax  = ym + h/2.0;
				ratio = h/yh;
				xmin  = xm - ratio * xw/2.0;
				xmax  = xm + ratio * xw/2.0;
			}
			if (m_QGraph.IsInDrawRect(ZRect.left, ZRect.top) &&
				m_QGraph.IsInDrawRect(ZRect.right, ZRect.bottom)){
				m_QGraph.SetWindow(xmin, xmax, ymin, ymax);
			}
			m_ZoomRect.SetRectEmpty();
		}
		else {
			ReleaseZoom();
		}
	}
	else if(m_bZoomPlus && !m_rCltRect.PtInRect(point))
	{
			ReleaseZoom();
	}
	else if(m_bGetPos && m_rCltRect.PtInRect(point))
	{
		if(m_nPos == 0)
		{
			m_Pos1 = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x));
		}
		if(m_nPos == 1)
		{
			m_Pos2 = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x));
		}
		m_nPos++;
		if(m_nPos == 2) 
		{
			if(m_bSmooth) 
			{
				if (m_bFullInverse) m_pFInvCtrlBar->m_ctrlOutput.SetWindowText(" ");
				else m_pMInvCtrlBar->m_ctrlOutput.SetWindowText(" ");
				Smooth(m_Pos1, m_Pos2);
			}
			else if(m_bSpline) 
			{
				x1 = m_pMCurve->x[m_Pos1];
				x2 = m_pMCurve->x[m_Pos2];
				if(abs(x2-x1)<0.00001) return;
				if(x2<x1)
				{
					tmp    = m_Pos2;
					m_Pos2 = m_Pos1;
					m_Pos1 = tmp;
				}

				m_SplineLeftPos  = m_Pos1;
				m_SplineRightPos = m_Pos2;

				m_Spline.m_iCtrlPoints = 0;
				m_Spline.InsertPoint(m_pMCurve->x[m_Pos1], m_pMCurve->y[m_Pos1]);
				m_Spline.InsertPoint(m_pMCurve->x[m_Pos2], m_pMCurve->y[m_Pos2]);

				x = (3.0*m_pMCurve->x[m_Pos1] + m_pMCurve->x[m_Pos2])/4.0;
				y = (3.0*m_pMCurve->y[m_Pos1] + m_pMCurve->y[m_Pos2])/4.0;
				m_Spline.InsertPoint(x,y);

				x = (m_pMCurve->x[m_Pos1] + m_pMCurve->x[m_Pos2])/2.0;
				y = (m_pMCurve->y[m_Pos1] + m_pMCurve->y[m_Pos2])/2.0;
				m_Spline.InsertPoint(x,y);

				x = (m_pMCurve->x[m_Pos1] + 3.0*m_pMCurve->x[m_Pos2])/4.0;
				y = (m_pMCurve->y[m_Pos1] + 3.0*m_pMCurve->y[m_Pos2])/4.0;
				m_Spline.InsertPoint(x,y);


				if (m_bTangentSpline)
				{
					//Second point must remain on tangent to curve
					ux = m_pMCurve->x[m_Pos1+1] - m_pMCurve->x[m_Pos1];
					uy = m_pMCurve->y[m_Pos1+1] - m_pMCurve->y[m_Pos1];
					norm = sqrt(ux*ux+uy*uy);
					ux /= norm;
					uy /= norm;
					xpt = m_Spline.m_Input[1].x - m_Spline.m_Input[0].x;
					ypt = m_Spline.m_Input[1].y - m_Spline.m_Input[0].y;
				
					m_Spline.m_Input[1].x = m_Spline.m_Input[0].x + (ux*xpt + uy*ypt) * ux;
					m_Spline.m_Input[1].y = m_Spline.m_Input[0].y + (ux*xpt + uy*ypt) * uy;
				

					//penultimate point must remain on tangent to curve
					ux = m_pMCurve->x[m_Pos2] - m_pMCurve->x[m_Pos2-1];
					uy = m_pMCurve->y[m_Pos2] - m_pMCurve->y[m_Pos2-1];
					norm = sqrt(ux*ux+uy*uy);
					ux /= norm;
					uy /= norm;

					xpt = m_Spline.m_Input[m_Spline.m_iCtrlPoints-2].x - m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x;
					ypt = m_Spline.m_Input[m_Spline.m_iCtrlPoints-2].y - m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].y;
					
					m_Spline.m_Input[m_Spline.m_iCtrlPoints-2].x = m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x + (ux*xpt + uy*ypt) * ux;
					m_Spline.m_Input[m_Spline.m_iCtrlPoints-2].y = m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].y + (ux*xpt + uy*ypt) * uy;
				}	

				m_Spline.SplineKnots();
				m_Spline.SplineCurve();
				if(m_bFullInverse)
				{
					m_pFInvCtrlBar->m_ctrlNewSpline.SetCheck(0);
					m_pFInvCtrlBar->m_ctrlNewSpline.Invalidate();
					m_pFInvCtrlBar->m_ctrlOutput.SetWindowText(
						"Drag points to modify splines, Apply, and Execute to generate new geometry");
				}
				else
				{
					m_pMInvCtrlBar->m_ctrlNewSpline.SetCheck(0);
					m_pMInvCtrlBar->m_ctrlNewSpline.Invalidate();
					m_pMInvCtrlBar->m_ctrlOutput.SetWindowText(	
						"Drag points to modify splines, Apply, and Execute to generate new geometry");
				}
			}
			else if(m_bMark)
			{
				m_pMInvCtrlBar->m_ctrlOutput.SetWindowText(" ");
				if (m_Pos1 == m_Pos2) return;
				m_Mk1 = m_Pos1;
				m_Mk2 = m_Pos2;
				m_pXFoil->iq1 = min(m_Pos1, m_Pos2)+1;
				m_pXFoil->iq2 = max(m_Pos1, m_Pos2)+1;
				if(m_pXFoil->iq1<=1) m_pXFoil->iq1 = 2;
				if(m_pXFoil->iq2>=m_pXFoil->n) m_pXFoil->iq2 = m_pXFoil->n-1;
				m_pXFoil->liqset = true;
				m_bMarked = true;
				m_bMark = false;
				m_pMInvCtrlBar->m_ctrlMark.SetCheck(0);
			}
			m_bGetPos = false;
//			ReleaseCapture();
		}
	}
	else {
//		ReleaseCapture();// in case we have a grip cursor
	}
	if(m_QGraph.IsInDrawRect(point)){
		SetCursor(m_hcCross);
	}
	else {
		SetCursor(m_hcArrow);
	}
	UpdateView();
}


void CXInverse::OnMouseMove(UINT nFlags, CPoint point) 
{
	SHORT shZ  = GetKeyState(90);
	double x1,y1, xmin, xmax, ymin,  ymax, xpt, ypt, scale, ux, uy, unorm, vx, vy, vnorm, scal;
	double xx0,xx1,xx2,yy0,yy1,yy2;
	int a, n, ipt;

	if(m_bGetPos)
	{
		m_tmpPos = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x));
	}
	else if(m_bZoomPlus && (nFlags & MK_LBUTTON))
	{
		m_ZoomRect.right  = point.x;
		m_ZoomRect.bottom = point.y;
	}
	else if(m_rCltRect.PtInRect(point) && (nFlags & MK_LBUTTON) && m_bTrans)
	{
		CPoint pttmp(point.x, point.y);
		if(m_bTransGraph)
		{
			// we're dragging the graph
			x1 =  m_QGraph.ClientTox(m_PointDown.x) ;
			y1 =  m_QGraph.ClientToy(m_PointDown.y) ;
			
			xu = m_QGraph.ClientTox(point.x);
			yu = m_QGraph.ClientToy(point.y);

			xmin = m_QGraph.GetXMin() - xu+x1;
			xmax = m_QGraph.GetXMax() - xu+x1;
			ymin = m_QGraph.GetYMin() - yu+y1;
			ymax = m_QGraph.GetYMax() - yu+y1;

			m_QGraph.SetWindow(xmin, xmax, ymin, ymax);
		}
		else 
		{
			//we're dragging the foil
			m_ptOffset.x += point.x - m_PointDown.x;
			m_ptOffset.y += point.y - m_PointDown.y;
		}
		m_PointDown = point;
	}
	else if ((nFlags & MK_LBUTTON) && !m_bZoomPlus && m_bSpline && m_Spline.m_iSelect>=0) 
	{
		// user is dragging the point
		x1 =  m_QGraph.ClientTox(point.x) ;
		y1 =  m_QGraph.ClientToy(point.y) ;
		if(m_rGraphRect.PtInRect(point))
		{
			n = m_Spline.m_iSelect;
			if(n==0)
			{
				// user is dragging end point
				// find closest graph point
				ipt = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x));
				m_SplineLeftPos = ipt;
				xpt = m_pMCurve->x[ipt];
				ypt = m_pMCurve->y[ipt];
				// check for inversion
				if(xpt> m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x)
				{
					m_Spline.m_Input[n].x = m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x;
					m_Spline.m_Input[n].y = m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].y;
					m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x = xpt;
					m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].y = ypt;
				}
				else
				{
					m_Spline.m_Input[n].x = xpt;
					m_Spline.m_Input[n].y = ypt;
				}
				m_Spline.SplineCurve();
			}
			else if(n == m_Spline.m_iCtrlPoints-1)
			{
				// user is dragging end point
				// find closest graph point
				ipt = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x));
				m_SplineRightPos = ipt;
				xpt = m_pMCurve->x[ipt];
				ypt = m_pMCurve->y[ipt];
				// check for inversion
				if(xpt< m_Spline.m_Input[0].x)
				{
					m_Spline.m_Input[n].x = m_Spline.m_Input[0].x;
					m_Spline.m_Input[n].y = m_Spline.m_Input[0].y;
					m_Spline.m_Input[0].x = xpt;
					m_Spline.m_Input[0].y = ypt;
				}
				else
				{
					m_Spline.m_Input[n].x = xpt;
					m_Spline.m_Input[n].y = ypt;
				}
				m_Spline.SplineCurve();
			}
			else if (n==1 && m_bTangentSpline)
			{
				// Second point must remain on tangent to curve
				// difficulty is that we are working in non-normal coordinates

				tanpt.SetPoint(point.x, point.y);
				P0.SetPoint(m_QGraph.xToClient(m_pMCurve->x[m_SplineLeftPos-1]), m_QGraph.yToClient(m_pMCurve->y[m_SplineLeftPos-1]));
				P1.SetPoint(m_QGraph.xToClient(m_pMCurve->x[m_SplineLeftPos]),   m_QGraph.yToClient(m_pMCurve->y[m_SplineLeftPos]));
				P2.SetPoint(m_QGraph.xToClient(m_pMCurve->x[m_SplineLeftPos+1]), m_QGraph.yToClient(m_pMCurve->y[m_SplineLeftPos+1]));
				//v is the tangent to the curve in screen coordinates
				vx = (double)((P0.x-P1.x)*(P0.x-P2.x)*(P1.x-P2.x)*(P2.x-P0.x));
				vy = (double)( P0.y *(P1.x-P2.x)          * (P1.x-P2.x) * (P2.x-P0.x)         
							 - P1.y *(2.0*P1.x-P0.x-P2.x) * (P0.x-P2.x) * (P2.x-P0.x)
							 - P2.y *(P1.x-P0.x)          * (P0.x-P1.x) * (P0.x-P2.x));
				vnorm = sqrt(vx*vx+vy*vy);
				vx/=vnorm;
				vy/=vnorm;
				scal = (double)(point.x-P1.x)*vx + (double)(point.y-P1.y)*vy;
				tanpt.x = P1.x + (int)(vx * scal);
				tanpt.y = P1.y + (int)(vy * scal);

				x1 =  m_QGraph.ClientTox(tanpt.x) ;
				y1 =  m_QGraph.ClientToy(tanpt.y) ;

				xx0 = m_pMCurve->x[m_SplineLeftPos-1];
				xx1 = m_pMCurve->x[m_SplineLeftPos];
				xx2 = m_pMCurve->x[m_SplineLeftPos+1];
				yy0 = m_pMCurve->y[m_SplineLeftPos-1];
				yy1 = m_pMCurve->y[m_SplineLeftPos];
				yy2 = m_pMCurve->y[m_SplineLeftPos+1];

				ux = (xx0-xx1)*(xx0-xx2)*(xx1-xx2)*(xx2-xx0);
				uy =	  yy0 *(xx1-xx2)         * (xx1-xx2) * (xx2-xx0)         
						- yy1 *(2.0*xx1-xx0-xx2) * (xx0-xx2) * (xx2-xx0)
						- yy2 *(xx1-xx0)         * (xx0-xx1) * (xx0-xx2);
			
//				unorm = sqrt(ux*ux*scx*scx+uy*uy*scy*scy)/scx/scy;
				unorm = sqrt(ux*ux+uy*uy);
				ux /= unorm;
				uy /= unorm;

				vx = x1-m_Spline.m_Input[n-1].x;
				vy = y1-m_Spline.m_Input[n-1].y;

				scal =  (ux*vx + uy*vy);
				m_Spline.m_Input[n].x = m_Spline.m_Input[0].x + scal * ux ;
				m_Spline.m_Input[n].y = m_Spline.m_Input[0].y + scal * uy ;
				m_Spline.SplineCurve();
			}
			else if (n==m_Spline.m_iCtrlPoints-2 && m_bTangentSpline)
			{
				//penultimate point must remain on tangent to curve
				// difficulty is that we are working in non-normal coordinates
				tanpt.SetPoint(point.x, point.y);
				P0.SetPoint(m_QGraph.xToClient(m_pMCurve->x[m_SplineRightPos-1]), m_QGraph.yToClient(m_pMCurve->y[m_SplineRightPos-1]));
				P1.SetPoint(m_QGraph.xToClient(m_pMCurve->x[m_SplineRightPos]),   m_QGraph.yToClient(m_pMCurve->y[m_SplineRightPos]));
				P2.SetPoint(m_QGraph.xToClient(m_pMCurve->x[m_SplineRightPos+1]), m_QGraph.yToClient(m_pMCurve->y[m_SplineRightPos+1]));
				//v is the tangent to the curve in screen coordinates
				vx = (double)((P0.x-P1.x)*(P0.x-P2.x)*(P1.x-P2.x)*(P2.x-P0.x));
				vy = (double)( P0.y *(P1.x-P2.x)          * (P1.x-P2.x) * (P2.x-P0.x)         
							 - P1.y *(2.0*P1.x-P0.x-P2.x) * (P0.x-P2.x) * (P2.x-P0.x)
							 - P2.y *(P1.x-P0.x)          * (P0.x-P1.x) * (P0.x-P2.x));
				vnorm = sqrt(vx*vx+vy*vy);
				vx/=vnorm;
				vy/=vnorm;
				scal = (double)(point.x-P1.x)*vx + (double)(point.y-P1.y)*vy;
				tanpt.x = P1.x + (int)(vx * scal);
				tanpt.y = P1.y + (int)(vy * scal);

				x1 =  m_QGraph.ClientTox(tanpt.x) ;
				y1 =  m_QGraph.ClientToy(tanpt.y) ;

				xx0 = m_pMCurve->x[m_SplineRightPos-1];
				xx1 = m_pMCurve->x[m_SplineRightPos];
				xx2 = m_pMCurve->x[m_SplineRightPos+1];
				yy0 = m_pMCurve->y[m_SplineRightPos-1];
				yy1 = m_pMCurve->y[m_SplineRightPos];
				yy2 = m_pMCurve->y[m_SplineRightPos+1];

				ux = (xx0-xx1)*(xx0-xx2)*(xx1-xx2)*(xx2-xx0);
				uy =	  yy0 *(xx1-xx2)         * (xx1-xx2) * (xx2-xx0)         
						- yy1 *(2.0*xx1-xx0-xx2) * (xx0-xx2) * (xx2-xx0)
						- yy2 *(xx1-xx0)         * (xx0-xx1) * (xx0-xx2);

				unorm = sqrt(ux*ux+uy*uy);
				ux /= unorm;
				uy /= unorm;

				vx = x1-m_Spline.m_Input[n+1].x;
				vy = y1-m_Spline.m_Input[n+1].y;

				scal =  (ux*vx + uy*vy);
				m_Spline.m_Input[n].x = m_Spline.m_Input[n+1].x + scal * ux;
				m_Spline.m_Input[n].y = m_Spline.m_Input[n+1].y + scal * uy;
				m_Spline.SplineCurve();
			}	
			else if (n>0 && n<m_Spline.m_iCtrlPoints-1)
			{
//				if(x1<m_Spline.m_Input[0].x) 				        x1 = m_Spline.m_Input[0].x;
//				if(x1>m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x) x1 = m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x;

				m_Spline.m_Input[n].x = x1;
				m_Spline.m_Input[n].y = y1;
				m_Spline.SplineCurve();
			}
		}
	}
	else if((nFlags & MK_MBUTTON) ||  (shZ & 0x8000))
	{
		ReleaseZoom();
		CPoint pttmp(point.x, point.y);
		if(m_QGraph.IsInDrawRect(pttmp)){
			//zoom graph

			SHORT shX = GetKeyState('X');
			SHORT shY = GetKeyState('Y');

			if (shX & 0x8000){
				//zoom x scale
				m_QGraph.SetAutoX(false);
				if(point.x-m_PointDown.x<0) m_QGraph.Scalex(1.04);
				else						m_QGraph.Scalex(1.0/1.04);
			}
			else if(shY & 0x8000){
				//zoom y scale
				m_QGraph.SetAutoY(false);
				if(point.y-m_PointDown.y<0) m_QGraph.Scaley(1.04);
				else						m_QGraph.Scaley(1.0/1.04);
			}
			else{
				//zoom both
				m_QGraph.SetAuto(false);
				if(point.y-m_PointDown.y<0) m_QGraph.Scale(1.06);
				else						m_QGraph.Scale(1.0/1.06);
			}
		}
		else{
			scale = m_fScale;

			if(point.y-m_PointDown.y>0) m_fScale *= 1.06;
			else						m_fScale /= 1.06;

			a = (int)((m_rCltRect.right+m_rCltRect.left)/2);
			m_ptOffset.x = a + (int)((m_ptOffset.x-a)*m_fScale/scale);
		}
		m_PointDown = point;
	}
	else{// highlight if mouse passe over a point
		if(m_bSpline)
		{
			x1 =  m_QGraph.ClientTox(point.x) ;
			y1 =  m_QGraph.ClientToy(point.y) ;
			n = m_Spline.IsControlPoint(x1,y1,
				m_QGraph.GetXScale(),
				m_QGraph.GetYScale());
			if (n>=0 && n<=m_Spline.m_iCtrlPoints) {
				m_Spline.m_iHighlight = n;
			}
			else m_Spline.m_iHighlight = -1;
		}
	}
	UpdateView();
}


void CXInverse::OnZoomLess() 
{
	// can't do two things at the same time can we ?
	ReleaseZoom();

	double ZoomFactor = 1.2f;
	m_QGraph.Scale(ZoomFactor);

	UpdateView();
}



BOOL CXInverse::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
//	ScreenToClient(&pt);

	ReleaseZoom();
	CPoint pttmp(pt.x, pt.y);
	if(m_QGraph.IsInDrawRect(pttmp)){
		SHORT shX = GetKeyState('X');
		SHORT shY = GetKeyState('Y');

		if (shX & 0x8000){
			//zoom x scale
			m_QGraph.SetAutoX(false);
			if(zDelta>0) m_QGraph.Scalex(1.06);
			else m_QGraph.Scalex(1.0/1.06);
		}
		else if(shY & 0x8000){
			//zoom y scale
			m_QGraph.SetAutoY(false);
			if(zDelta>0) m_QGraph.Scaley(1.06);
			else m_QGraph.Scaley(1.0/1.06);
		}
		else{
			//zoom both
			m_QGraph.SetAuto(false);
			if(zDelta>0) m_QGraph.Scale(1.06);
			else m_QGraph.Scale(1.0/1.06);
		}
	}
	else{
		double scale = m_fScale;

		if(zDelta<0) m_fScale *= 1.06f;
		else m_fScale /= 1.06f;

		int a = (int)((m_rCltRect.right+m_rCltRect.left)/2);
		m_ptOffset.x = a + (int)((m_ptOffset.x-a)*m_fScale/scale);
	}
	UpdateView();
	return TRUE;
}



void CXInverse::OnContextMenu(CPoint ScreenPoint, CPoint ClientPoint) 
{
	m_ptPopUp.x = ClientPoint.x;
	m_ptPopUp.y = ClientPoint.y;

	if(m_QGraph.IsInDrawRect(m_ptPopUp)){
		CMenu menu;
		if (menu.LoadMenu(IDR_CTXINVMENU)){
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);
			if(!m_bSpline){
				pPopup->EnableMenuItem(IDM_DELETEPOINT,MF_BYCOMMAND | MF_GRAYED);
				pPopup->EnableMenuItem(IDM_ADDPOINT,MF_BYCOMMAND | MF_GRAYED);
			}
			if (m_pQCurve->IsVisible())		pPopup->CheckMenuItem(IDM_QINIT,MF_BYCOMMAND | MF_CHECKED);
			else							pPopup->CheckMenuItem(IDM_QINIT,MF_BYCOMMAND | MF_UNCHECKED);
			if (m_pMCurve->IsVisible())		pPopup->CheckMenuItem(IDM_QMOD,MF_BYCOMMAND | MF_CHECKED);
			else							pPopup->CheckMenuItem(IDM_QMOD,MF_BYCOMMAND | MF_UNCHECKED);
			if (m_pQVCurve->IsVisible())	pPopup->CheckMenuItem(IDM_QVISC,MF_BYCOMMAND | MF_CHECKED);
			else							pPopup->CheckMenuItem(IDM_QVISC,MF_BYCOMMAND | MF_UNCHECKED);
			if (m_bShowPoints)				pPopup->CheckMenuItem(IDM_SHOWQPOINTS,MF_BYCOMMAND | MF_CHECKED);
			else							pPopup->CheckMenuItem(IDM_SHOWQPOINTS,MF_BYCOMMAND | MF_UNCHECKED);
			if (m_bReflected)				pPopup->CheckMenuItem(IDM_SHOWREFLECTED,MF_BYCOMMAND | MF_CHECKED);
			else							pPopup->CheckMenuItem(IDM_SHOWREFLECTED,MF_BYCOMMAND | MF_UNCHECKED);

			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				ScreenPoint.x, ScreenPoint.y, m_pFrame); // use main window for cmds
		}
	}
	else{
		CMenu menu;
		if (menu.LoadMenu(IDR_CTXINVFOIL)){
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);

			if (m_bRefFoil) pPopup->CheckMenuItem(IDM_SHOWREF,MF_BYCOMMAND | MF_CHECKED);
			if (m_bModFoil) pPopup->CheckMenuItem(IDM_SHOWMOD,MF_BYCOMMAND | MF_CHECKED);
			
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				ScreenPoint.x, ScreenPoint.y, m_pFrame); // use main window for cmds
		}
	}
}

void CXInverse::OnGraphOptions() 
{
	CGraphDlg dlg;
	dlg.m_pParent = m_pChildWnd;
	dlg.m_pGraph = &m_QGraph;

	if(IDOK == dlg.DoModal()){
		SetBlackAndWhite(false);
		UpdateView();
	}
}



void CXInverse::OnResetGraph() 
{
	ReleaseZoom();
	m_QGraph.SetAuto(true);
	m_QGraph.Init();
	UpdateView();
}


void CXInverse::OnDeletePoint() 
{
/*	double xd = m_QGraph.ClientTox(m_ptPopUp.x);
	double yd = m_QGraph.ClientToy(m_ptPopUp.y);

	int n =  m_Spline.IsControlPoint(xd,yd ,
									 m_QGraph.GetXScale(),
									 m_QGraph.GetYScale());

	if (n>=0) m_Spline.RemovePoint(n);*/
	if (m_Spline.m_iSelect>=0) m_Spline.RemovePoint(m_Spline.m_iSelect);
	m_Spline.SplineCurve();
	UpdateView();
}


void CXInverse::OnAddPoint() 
{
	double xd = m_QGraph.ClientTox(m_ptPopUp.x);
	double yd = m_QGraph.ClientToy(m_ptPopUp.y);

	if(xd < m_Spline.m_Input[0].x) return;
	if(xd > m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x) return;


	m_Spline.InsertPoint(xd,yd);
	m_Spline.SplineKnots();
	m_Spline.SplineCurve();
	UpdateView();
}


void CXInverse::ResetQ()
{
	m_pXFoil->cncalc(m_pXFoil->qgamm,false);
	m_pXFoil->qspcir();
	CreateMCurve();
}


/*
double CXInverse::qcomp(double g)
{
//---- statement function for compressible karman-tsien velocity
    double qq =  g*(1.0-m_pXFoil->tklam) 
		        / (1.0 - m_pXFoil->tklam*(g/m_pXFoil->qinf)*(g/m_pXFoil->qinf));
	return qq;
}*/


double CXInverse::qincom(double qc, double qinf, double tklam)
{
//-------------------------------------
//     sets incompressible speed from
//     karman-tsien compressible speed
//-------------------------------------

	if(tklam<1.0e-4 || abs(qc)<1.0e-4) {
//----- for nearly incompressible case or very small speed, use asymptotic
//      expansion of singular quadratic formula to avoid numerical problems
		return( qc/(1.0 - tklam));
	}
	else{
//----- use quadratic formula for typical case
		double tmp = 0.5f*(1.0 - tklam)*qinf/(qc*tklam);
		return (qinf*tmp*((double)sqrt(1.0 + 1.0/(tklam*tmp*tmp)) - 1.0));
	}
}


void CXInverse::Smooth(int Pos1, int Pos2)
{
	if(Pos1 ==-1){//smooth it all
		Pos1 = 1;
		Pos2 = m_pXFoil->nsp;
	}
	m_bGetPos = false;
	if (abs(Pos2-Pos1)<=2) return;
	if (Pos1>Pos2) {
		int tmp = Pos2;
		Pos2  = Pos1;
		Pos1  = tmp;
	}
	if(m_bFullInverse){
		m_pXFoil->smooq(Pos1,Pos2,1);
		m_pXFoil->cncalc(m_pXFoil->qspec[1], false);
		m_pXFoil->qspcir();
		m_pXFoil->lqspec = true;
	}
	else{// added v1.17, i.e. different sequence as for Full Inverse
		m_pXFoil->smooq(Pos1,Pos2,1);
		m_pXFoil->splqsp(1);
		m_pXFoil->clcalc(m_pXFoil->xcmref,m_pXFoil->ycmref);
//		m_pXFoil->lqspec = true; ?? should we
	}
	CreateMCurve();
	m_bSmooth = false;
	m_pFInvCtrlBar->CancelSmooth();
	m_pMInvCtrlBar->CancelSmooth();
	UpdateView();
}


void CXInverse::SetTAngle(double a)
{
	m_pXFoil->agte = a/180.0;
}


void CXInverse::SetTGap(double tr, double ti)
{
	m_pXFoil->dzte = complex<double>(tr,ti);
}

void CXInverse::Filter(double filt)
{
	m_pXFoil->Filter(filt);
	CreateMCurve();
	UpdateView();
}

void CXInverse::Pertubate()
{
	m_pXFoil->pert_init(1);
	int m;

	CPertDlg dlg;
	for (m=0; m<=__min(32, m_pXFoil->nc); m++){
		dlg.m_cnr[m] = (double)real(m_pXFoil->cn[m]);
		dlg.m_cni[m] = (double)imag(m_pXFoil->cn[m]);
	}
	dlg.m_nc = __min(32, m_pXFoil->nc);

	if(dlg.DoModal() == IDOK){
		for (m=0; m<=__min(32, m_pXFoil->nc); m++){
			m_pXFoil->cn[m] = complex<double>(dlg.m_cnr[m],dlg.m_cni[m]);
		}

		m_pXFoil->pert_process(1);
		CreateMCurve();
		m_pMCurve->SetVisible(true);
		UpdateView();
	}
}


void CXInverse::OnShowControls() 
{
	if(m_bFullInverse) 	m_pFInvCtrlBar->ShowWindow(SW_SHOW);
	else m_pMInvCtrlBar->ShowWindow(SW_SHOW);	
}


void CXInverse::OnPrint() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	bool bStatus = IsBlackAndWhite();

	CDC dc;
	CPrintDialog dlgPrint(false);

	if(IDOK==dlgPrint.DoModal()){
		DOCINFO di;
		ZeroMemory(&di, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);

		CString str = "XFLR5 - " + m_pModFoil->m_FoilName;
		di.lpszDocName = str;
		dc.Attach(dlgPrint.GetPrinterDC());
		
		// set landscape mode
		DEVMODE *pDevMode = dlgPrint.GetDevMode();
		int h, w;
		GetPaperSize(pDevMode, w, h);

		CPrintOptionsDlg dlg;
		dlg.m_bColor    = GetPrintColor();
		dlg.m_bWindowBk = GetWindowBk();
		dlg.m_bGraphBk  = GetGraphBk();
		dlg.m_Left   = pFrame->m_LeftMargin;
		dlg.m_Right  = pFrame->m_RightMargin;
		dlg.m_Top    = pFrame->m_TopMargin;
		dlg.m_Bottom = pFrame->m_BottomMargin;
		dlg.m_PenWidth = ((CXFLR5App*)AfxGetApp())->m_PenWidth;
		dlg.m_Width  = w;
		dlg.m_Height = h;
		dlg.m_PrinterName = dlgPrint.GetDeviceName();

		if (bStatus) dlg.m_bColor = false;

		if(dlg.DoModal()!=IDOK) {
			dc.Detach();
			return;
		}


		pFrame->m_LeftMargin   = dlg.m_Left;
		pFrame->m_RightMargin  = dlg.m_Right;
		pFrame->m_TopMargin    = dlg.m_Top;
		pFrame->m_BottomMargin = dlg.m_Bottom;
		((CXFLR5App*)AfxGetApp())->m_PenWidth = dlg.m_PenWidth;

		SetPrintOptions(dlg.m_bWindowBk, dlg.m_bGraphBk, dlg.m_bColor);

		if(!dlg.m_bColor) SetBlackAndWhite(true);

		CRect CltRect(  (int)(dlg.m_Left*100),
						(int)(dlg.m_Top*100),
						(int)(h*10-dlg.m_Right*100),
						(int)(w*10-dlg.m_Bottom*100));// x 0.01 mm

//		CRect CltRect(0,0,h*10,w*10);// x 0.01 mm

		if (pDevMode != NULL) { 
			pDevMode->dmFields = DM_ORIENTATION;
			pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
			dc.ResetDC(pDevMode);
			GlobalFree(pDevMode);
		}
		ASSERT(dc.m_hDC != NULL);
		dc.StartDoc(&di);
		dc.StartPage();
		PrintAll(&dc, &CltRect);
		dc.EndPage();
		dc.EndDoc();
		dc.Detach();
	}
	SetBlackAndWhite(bStatus);
	UpdateView();
}

bool CXInverse::PrintAll(CDC *pDC, CRect DrawRect)
{
	CChildView *pChildView = (CChildView*)m_pChildWnd;
	pDC->SetMapMode(MM_HIMETRIC);
	if(GetWindowBk() && !IsBlackAndWhite()){
		pDC->SetBkMode(OPAQUE);

		CPen WhitePen;
		WhitePen.CreateStockObject(WHITE_PEN);
		CPen *pOldPen = pDC->SelectObject(&WhitePen);

		CRect BkRect(DrawRect);
		BkRect.top = -BkRect.top;
		BkRect.bottom = -BkRect.bottom;
		CBrush BkBrush(pChildView->m_crBackColor);
		CBrush *pOldBrush = pDC->SelectObject(&BkBrush);
		pDC->Rectangle(&BkRect);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
		WhitePen.DeleteObject();
		BkBrush.DeleteObject();
	}

	double scale;
	CPoint Offset;
	COLORREF color;
	int style, width;

	CPen AnyPen(PS_SOLID,50,RGB(0,0,0));
	CPen* pOldPen = pDC->SelectObject(&AnyPen);


	CRect QRect;
	QRect.SetRect(DrawRect.left,	//left
				  DrawRect.top,		//top
				  DrawRect.right,		//right
				  DrawRect.bottom-6000);	//bottom
	m_QGraph.DrawGraph(pDC, &QRect, true);

	scale    =   m_fScale * (double)(QRect.Width()-3000)/m_fRefScale;

	if(IsBlackAndWhite()) pDC->SetTextColor(0);
	else				  pDC->SetTextColor(pChildView->m_WndTextColor);

	CPoint Place(QRect.right-6000, -QRect.top-300);
	m_QGraph.DrawLegend(pDC, true, Place);

	double alpha = -m_pXFoil->alqsp[1];
	double conv = (double)DrawRect.Height()/(double)m_rCltRect.Height();

	Offset.x =  (int)(m_ptOffset.x*conv);
	Offset.y = -DrawRect.top - (int)(m_ptOffset.y*conv);
	
	DrawGrid(pDC, &DrawRect, scale, Offset, true);

	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	color = m_pRefFoil->m_FoilColor;
	style = m_pRefFoil->m_nFoilStyle;
	width = m_pRefFoil->m_nFoilWidth;
	GetBWColor(color, style, width);
	lb.lbColor = color;
	CPen ContourPen(PS_GEOMETRIC | style, GetPenWidth(width,true), &lb);

	pDC->SelectObject(&ContourPen);

	DrawRect.bottom = -DrawRect.bottom;
	DrawRect.NormalizeRect();
	m_pRefFoil->DrawFoil(pDC, alpha, scale, scale, Offset, DrawRect, true);

	CFont RFont;
	LOGFONT TempFont;
	memcpy(&TempFont, &pChildView->m_WndLogFont,sizeof(LOGFONT));
	TempFont.lfHeight = TempFont.lfHeight*30;
	RFont.CreateFontIndirect(&TempFont);
	CFont *pOldFont = pDC->SelectObject(&RFont);

	if(IsBlackAndWhite()) pDC->SetTextColor(0);
	else				  pDC->SetTextColor(pChildView->m_WndTextColor);

	pDC->MoveTo(DrawRect.left+1500,  DrawRect.top+5300);
	pDC->LineTo(DrawRect.left+2500,  DrawRect.top+5300);
	pDC->TextOut(DrawRect.left+3000, DrawRect.top+5500,m_pRefFoil->m_FoilName);

	if(m_bModFoil) {
		color = m_pModFoil->m_FoilColor;
		style = m_pModFoil->m_nFoilStyle;
		width = m_pModFoil->m_nFoilWidth;
		if(IsBlackAndWhite()) GetBWColor(color, style, width);
		lb.lbColor = color;
		CPen ModPen(PS_GEOMETRIC | style, GetPenWidth(width,true), &lb);
		pDC->SelectObject(&ModPen);
		m_pModFoil->DrawFoil(pDC, alpha, scale, scale, Offset, DrawRect, true);

		pDC->MoveTo(DrawRect.left+1500,  DrawRect.top+4900);
		pDC->LineTo(DrawRect.left+2500,  DrawRect.top+4900);
		pDC->TextOut(DrawRect.left+3000, DrawRect.top+5100,m_pModFoil->m_FoilName);
	}

	pDC->SelectObject(pOldPen);


// and print the program version
	CFont VFont;
	pDC->SetTextColor(0);// black version name
	VFont.CreateFont(280,0,0,0,FW_NORMAL,FALSE,FALSE,0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, FF_SWISS+VARIABLE_PITCH, "Arial");

	pDC->SelectObject(&VFont);
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pDC->SetTextAlign(TA_RIGHT);
	pDC->TextOut(DrawRect.right,DrawRect.top, pFrame->m_VersionName);

	pDC->SelectObject(pOldFont);

	return true;
}




void CXInverse::ResetMixedQ()
{
	for (int i=0; i<=m_pQCurve->n; i++){
		m_pMCurve->x[i] = m_pQCurve->x[i] ;
		m_pMCurve->y[i] = m_pQCurve->y[i] ;
	}
	m_pMCurve->n = m_pQCurve->n;

//	m_pXFoil->gamqsp(1);
//	CreateMCurve();
}


void CXInverse::ExecMDES() 
{
//----- put modified info back into global arrays
	CWaitCursor wait;
	int i, isp;
	double qscom;
	for (i=1; i<= m_pXFoil->nsp; i++)
	{
		isp = m_pXFoil->nsp - i + 1;
		qscom =  m_pXFoil->qinf*m_pMCurve->y[i-1];
		m_pXFoil->qspec[1][i] = qincom(qscom,m_pXFoil->qinf,m_pXFoil->tklam);
	}
	m_pXFoil->ExecMDES();

	for(i=1; i<=m_pXFoil->nsp; i++)
	{
		m_pModFoil->x[i-1] = m_pXFoil->xb[i];
		m_pModFoil->y[i-1] = m_pXFoil->yb[i];
	}
	for(i=1; i<=m_pXFoil->nsp; i++)
	{
		m_pModFoil->xb[i-1] = m_pXFoil->xb[i];
		m_pModFoil->yb[i-1] = m_pXFoil->yb[i];
	}
	m_pModFoil->n  = m_pXFoil->nsp;
	m_pModFoil->nb = m_pXFoil->nsp;
	m_pModFoil->InitFoil();
	m_pModFoil->m_bSaved = false;

	m_bModFoil = true;
	CheckMenu();

}


bool CXInverse::ExecQDES()
{
	CWaitCursor wait;

	if(!m_bMarked) { // || !m_pXFoil->liqset 
        m_pMInvCtrlBar->m_ctrlOutput.SetWindowText("Must mark off target segment first");
		return false;  
	}

//----- put modified info back into global arrays
	int isp;
	double qscom;
	for (int i=1; i<= m_pXFoil->nsp; i++){
		isp = m_pXFoil->nsp - i + 1;
		qscom =  m_pXFoil->qinf*m_pMCurve->y[i-1];
		m_pXFoil->qspec[1][i] = qincom(qscom,m_pXFoil->qinf,m_pXFoil->tklam);
	}
	bool bRes = m_pXFoil->ExecQDES();

	CString str;
	CString strong = "";
	strong = "   dNMax       dGMax\r\n";
	for(int l=1; l<=m_pXFoil->QMax; l++){
		str.Format("%4.2e  %4.2e\r\n", m_pXFoil->dnTrace[l], m_pXFoil->dgTrace[l]);
		strong += str;
	}

	if(bRes) {
		strong += "Converged";
		m_pMInvCtrlBar->m_ctrlOutput.SetWindowText(strong);
	}
	else {
		strong += "Unconverged";
		m_pMInvCtrlBar->m_ctrlOutput.SetWindowText(strong);
	}

	for (i=1; i<=m_pXFoil->n; i++){
		m_pModFoil->x[i-1] = m_pXFoil->x[i];
		m_pModFoil->y[i-1] = m_pXFoil->y[i];
	}
	for (i=1; i<=m_pXFoil->nb; i++){
		m_pModFoil->xb[i-1] = m_pXFoil->x[i];
		m_pModFoil->yb[i-1] = m_pXFoil->y[i];
	}
	m_pModFoil->n = m_pXFoil->n;
	m_pModFoil->nb = m_pXFoil->nb;
	m_pModFoil->InitFoil();
	m_bModFoil = true;
	CheckMenu();
	return true;
}


void CXInverse::CreateQCurve()
{
	double x,y;
	m_pQCurve->n = 0;

	int points;
	if(m_bFullInverse) points = 257;
	else points  = m_pXFoil->n;

	for (int i=1; i<=points; i++)
	{
		x = 1.0 - m_pXFoil->sspec[i];
		y = m_pXFoil->qcomp(m_pXFoil->qspec[1][i])/m_pXFoil->qinf;
		m_pQCurve->AddPoint(x,y);
	}
}


void CXInverse::CreateMCurve()
{
	int i, points;
	double x,y;
	m_pMCurve->n = 0;
	m_pReflectedCurve->n = 0;

	if(m_bFullInverse) points = 257;
	else               points = m_pXFoil->n;

	for (i=1; i<=points; i++)
	{
		x = 1.0 - m_pXFoil->sspec[i];
		y = m_pXFoil->qcomp(m_pXFoil->qspec[1][i])/m_pXFoil->qinf;
		m_pMCurve->AddPoint(x,y);
		m_pReflectedCurve->AddPoint(m_pXFoil->sspec[i],-y);
	}
}


void CXInverse::ApplySpline()
{
	CWaitCursor wait;

	int i, isp;
	double qscom, xx;
	for (i=1; i<m_pMCurve->n-1; i++)
	{
		xx = m_pMCurve->x[i];
		if (xx > m_Spline.m_Input[0].x &&
			xx < m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x )
		{
			//interpolate spline at xx
			m_pMCurve->y[i] = m_Spline.GetY(xx);
		}
	}

	for (i=1; i<m_pMCurve->n-1; i++)
	{
		m_pReflectedCurve->y[i] = -m_pMCurve->y[i];
	}

	m_bSplined = true;
	for (i=1; i<= m_pXFoil->nsp; i++)
	{
		isp = m_pXFoil->nsp - i + 1;
		qscom =  m_pXFoil->qinf*m_pMCurve->y[i-1];
		m_pXFoil->qspec[1][i] = qincom(qscom,m_pXFoil->qinf,m_pXFoil->tklam);
	}
	
	m_pXFoil->lqspec = false;

	UpdateView();
}

void CXInverse::OnQInit() 
{
	m_pQCurve->SetVisible(!m_pQCurve->IsVisible());
	CheckMenu();
	UpdateView();
}


void CXInverse::OnQMod() 
{
	m_pMCurve->SetVisible(!m_pMCurve->IsVisible());
	CheckMenu();
	UpdateView();
}


void CXInverse::OnQVisc() 
{
	if(m_pXFoil->lvisc) {
		m_pQVCurve->SetVisible(!m_pQVCurve->IsVisible());
		CheckMenu();
		UpdateView();
	}
}


void CXInverse::OnShowPoints()
{
	m_bShowPoints = !m_bShowPoints;
	m_pQCurve->ShowPoints(m_bShowPoints);
	m_pMCurve->ShowPoints(m_bShowPoints);
	CheckMenu();
	UpdateView();
}


void CXInverse::OnShowReflected()
{
	m_bReflected = !m_bReflected;
	m_pReflectedCurve->SetVisible(m_bReflected);
	CheckMenu();
	UpdateView();
}

void CXInverse::OnExportFoil() 
{
	CString FileName, FoilName;
	CStdioFile XFile;
	CFileException fe;

//	bool bNotFound= true;
//	bool bWrite = false;
	FileName = m_pModFoil->m_FoilName;
	FileName.Replace("/", " ");
	CFileDialog XFileDlg(false, "dat", FileName,  OFN_OVERWRITEPROMPT,
		_T("Labeled Format (.dat)|*.dat|"));
	if(IDOK==XFileDlg.DoModal()) {
		FileName = XFileDlg.GetFileName();
		m_pModFoil->ExportFoil(FileName);	
	}
}


void CXInverse::UpdateView(CDC *pDC)
{
	COLORREF color;
	int style, width;

	CChildView * pChildView = (CChildView*)m_pChildWnd;

	if(!pDC) pDC = pChildView->GetDC();
	if(!pDC) return;
	
	CPen *pOldPen;

	CDC memdc; 
	memdc.CreateCompatibleDC (pDC);

	CBitmap  bmp;
	bmp.CreateCompatibleBitmap(pDC, m_rCltRect.Width(), m_rCltRect.Height());
	CBitmap * pOldBmp = memdc.SelectObject (&bmp);

	memdc.SetBkColor(pChildView->m_crBackColor);
	memdc.FillSolidRect(&m_rCltRect,pChildView->m_crBackColor);

	memdc.SetTextColor(pChildView->m_WndTextColor);

	CPen AnyPen(PS_SOLID, 1, RGB(0,0,0));
	pOldPen   = memdc.SelectObject(&AnyPen);
	memdc.SelectStockObject(NULL_BRUSH);

	CFont RFont;
	RFont.CreateFontIndirect(&pChildView->m_WndLogFont);

	CFont *pOldFont = memdc.SelectObject(&RFont);

//	draw the scale/grid
	if(m_bModFoil || m_bRefFoil){
		DrawGrid(&memdc, &m_rCltRect, m_fScale, m_ptOffset, false);
	}

//draw the reference and modified foils  
	double alpha = m_pXFoil->alqsp[1];
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	if(m_bRefFoil && m_bLoaded)
	{
		color = m_pRefFoil->m_FoilColor;
		style = m_pRefFoil->m_nFoilStyle;
		width = m_pRefFoil->m_nFoilWidth;
		if(IsBlackAndWhite()) GetBWColor(color, style, width);
		
		lb.lbColor = color;
		CPen FoilPen(PS_GEOMETRIC | style,	GetPenWidth(width, false), &lb);
		memdc.SelectObject(&FoilPen);		
		m_pRefFoil->DrawFoil(&memdc, -alpha, m_fScale, m_fScale, m_ptOffset, m_rCltRect, false);
		memdc.MoveTo(50, m_rCltRect.bottom-180);
		memdc.LineTo(80, m_rCltRect.bottom-180);
		memdc.TextOut(90, m_rCltRect.bottom-186,m_pRefFoil->m_FoilName);
	}

	if(m_bModFoil && m_bLoaded) 
	{
		color = m_pModFoil->m_FoilColor;
		style = m_pModFoil->m_nFoilStyle;
		width = m_pModFoil->m_nFoilWidth;
		if(IsBlackAndWhite()) GetBWColor(color, style, width);
		
		lb.lbColor = color;
		CPen ModPen(PS_GEOMETRIC | style,	GetPenWidth(width, false), &lb);
		CPen *pOld = memdc.SelectObject(&ModPen);		
		m_pModFoil->DrawFoil(&memdc, -alpha, m_fScale, m_fScale, m_ptOffset, m_rCltRect, false);
		memdc.MoveTo(50, m_rCltRect.bottom-165);
		memdc.LineTo(80, m_rCltRect.bottom-165);
		memdc.TextOut(90, m_rCltRect.bottom-171,m_pModFoil->m_FoilName);
		memdc.SelectObject(pOld);
	}

	if (m_pRefFoil->m_bPoints)
	{
		color = m_pRefFoil->m_FoilColor;
		style = m_pRefFoil->m_nFoilStyle;
		width = m_pRefFoil->m_nFoilWidth;
		if(IsBlackAndWhite()) GetBWColor(color, style, width);
		CPen CtrlPen(PS_SOLID,  1, color);
		CPen *pOld = memdc.SelectObject(&CtrlPen);
		m_pRefFoil->DrawPoints(&memdc, 1.0,  1.0, m_ptOffset, m_rCltRect, false);
		memdc.SelectObject(pOld);
	}


//  draw  the graph	
	if(m_rGraphRect.Width()>200 && m_rGraphRect.Height()>150)
	{
		m_QGraph.DrawGraph(&memdc, &m_rGraphRect, false);
		CPoint Place((int)(m_rGraphRect.right-300), m_rGraphRect.top+12);
		m_QGraph.DrawLegend(&memdc, false, Place);
	}

// draw the zoom rectangle, if relevant
	CRect ZRect(&m_ZoomRect);
	ZRect.NormalizeRect();
	if(m_bZoomPlus && !ZRect.IsRectEmpty()) {
		CRect ZRect(m_ZoomRect.left   - m_rCltRect.left,
					m_ZoomRect.top    - m_rCltRect.top,
					m_ZoomRect.right  - m_rCltRect.left,
					m_ZoomRect.bottom - m_rCltRect.top);
		ZRect.NormalizeRect();
		CPen ZoomPen(PS_DASH,1, RGB(100,100,100));
		CPen *pOld = memdc.SelectObject(&ZoomPen);
		memdc.SetBkMode(TRANSPARENT);
		memdc.Rectangle(ZRect);
		memdc.SelectObject(pOld);
	}


//Draw spline, if any
	if(m_bSpline && !m_bGetPos){
		lb.lbStyle = BS_SOLID;
		lb.lbColor = m_SplineClr;
		CPen SplinePen(PS_GEOMETRIC | m_SplineStyle,m_SplineWidth, &lb);
		
		CPen *pOld = memdc.SelectObject(&SplinePen);
		m_Spline.DrawSpline(&memdc, 
							1.0/m_QGraph.GetXScale(),	-1.0/m_QGraph.GetYScale(),
							m_QGraph.GetOffset(), false);
		m_Spline.DrawCtrlPoints(&memdc, 
							1.0/m_QGraph.GetXScale(),	-1.0/m_QGraph.GetYScale(),
							m_QGraph.GetOffset(), false);
		memdc.SelectObject(pOld);
	}
// Highlight selected points, if any
	if(m_bGetPos){
		CPoint pt;
		CRect r;
		m_QGraph.Highlight(&memdc, m_pMCurve,m_tmpPos);
		if(m_nPos>=1) m_QGraph.Highlight(&memdc, m_pMCurve,m_Pos1);
		if(m_nPos>=2) m_QGraph.Highlight(&memdc, m_pMCurve,m_Pos2);
		
	}
// Show marked segment if mixed-inverse design
	if(m_bMarked){
		CPoint ptl, ptr;
		ptl.x = m_QGraph.xToClient(m_pMCurve->x[m_Mk1]);
		ptr.x = m_QGraph.xToClient(m_pMCurve->x[m_Mk2]);		
		ptl.y = m_QGraph.yToClient(m_pMCurve->y[m_Mk1]);
		ptr.y = m_QGraph.yToClient(m_pMCurve->y[m_Mk2]);

		CPen MarkPen(PS_SOLID, 2, RGB(175,30,30));
		CPen *pOld = memdc.SelectObject(&MarkPen);

		if(m_rGraphRect.PtInRect(ptl)){
			memdc.MoveTo(ptl.x, ptl.y-20);
			memdc.LineTo(ptl.x, ptl.y+20);
		}
		if(m_rGraphRect.PtInRect(ptr)){
			memdc.MoveTo(ptr.x, ptr.y-20);
			memdc.LineTo(ptr.x, ptr.y+20);
		}
		memdc.SelectObject(pOld);
	}
// Blit back
	pDC->BitBlt(m_rCltRect.left,m_rCltRect.top,m_rCltRect.Width(),m_rCltRect.Height(),
			 &memdc,0,0,SRCCOPY);

	pChildView->ReleaseDC(pDC);

	memdc.SelectObject(pOldBmp);
	memdc.SelectObject(pOldPen);
	memdc.SelectObject(pOldFont);
	RFont.DeleteObject();
	bmp.DeleteObject();
	memdc.DeleteDC();
}

void CXInverse::OnColorStyles() 
{
	CXInvClrDlg dlg;
	dlg.m_SplineClr   = m_SplineClr;
	dlg.m_SplineStyle = m_SplineStyle;
	dlg.m_SplineWidth = m_SplineWidth;

	dlg.m_ReflectedClr   = m_ReflectedClr;
	dlg.m_ReflectedStyle = m_ReflectedStyle;
	dlg.m_ReflectedWidth = m_ReflectedWidth;

	dlg.m_ModFoilClr   = m_pModFoil->m_FoilColor;
	dlg.m_ModFoilStyle = m_pModFoil->m_nFoilStyle;
	dlg.m_ModFoilWidth = m_pModFoil->m_nFoilWidth;

	dlg.m_RefFoilClr   = m_pRefFoil->m_FoilColor;
	dlg.m_RefFoilStyle = m_pRefFoil->m_nFoilStyle;
	dlg.m_RefFoilWidth = m_pRefFoil->m_nFoilWidth;

	if(IDOK == dlg.DoModal()) 
	{
		m_SplineStyle = dlg.m_SplineStyle;
		m_SplineWidth = dlg.m_SplineWidth;
		m_SplineClr   = dlg.m_SplineClr;

		m_ReflectedClr   = dlg.m_ReflectedClr;
		m_ReflectedStyle = dlg.m_ReflectedStyle;
		m_ReflectedWidth = dlg.m_ReflectedWidth;
		m_pReflectedCurve->SetColor(m_ReflectedClr);
		m_pReflectedCurve->SetStyle(m_ReflectedStyle);
		m_pReflectedCurve->SetWidth(m_ReflectedWidth);

		m_pModFoil->m_nFoilStyle = dlg.m_ModFoilStyle;
		m_pModFoil->m_nFoilWidth = dlg.m_ModFoilWidth;
		m_pModFoil->m_FoilColor  = dlg.m_ModFoilClr;
		m_pMCurve->SetColor(m_pModFoil->m_FoilColor);
		m_pMCurve->SetStyle(m_pModFoil->m_nFoilStyle);
		m_pMCurve->SetWidth(m_pModFoil->m_nFoilWidth);

		m_pRefFoil->m_nFoilStyle = dlg.m_RefFoilStyle;
		m_pRefFoil->m_nFoilWidth = dlg.m_RefFoilWidth;
		m_pRefFoil->m_FoilColor  = dlg.m_RefFoilClr;
		m_pQCurve->SetColor(m_pRefFoil->m_FoilColor);
		m_pQCurve->SetStyle(m_pRefFoil->m_nFoilStyle);
		m_pQCurve->SetWidth(m_pRefFoil->m_nFoilWidth);

		UpdateView();
	}	
}


void CXInverse::ReleaseZoom()
{
	m_bZoomPlus = false;
	m_ZoomRect.SetRectEmpty();
	CToolBarCtrl *pTB = &(m_pXInverseBar->GetToolBarCtrl());
	pTB->PressButton(IDT_ZOOMIN, false);
}

void CXInverse::ResetScale()
{
	m_ptOffset.x = m_rGraphRect.left +(int)(1.0*m_QGraph.GetMargin());
	m_fRefScale  = m_rGraphRect.Width()-2.0*m_QGraph.GetMargin();

	m_ptOffset.y = m_rCltRect.bottom-100;
	m_fScale = m_fRefScale;
}



void CXInverse::OnShowRef() 
{
	m_bRefFoil = !m_bRefFoil;
	CheckMenu();
	UpdateView();
}

void CXInverse::OnShowMod() 
{
	m_bModFoil = !m_bModFoil;
	CheckMenu();
	UpdateView();
}

void CXInverse::SetRect(CRect CltRect)
{
	m_rCltRect.CopyRect(CltRect);
	m_rGraphRect.CopyRect(m_rCltRect);
	m_rGraphRect.DeflateRect(20,20,20,200);

	m_Spline.m_rViewRect = m_rGraphRect;
	ResetScale();
}

void CXInverse::SetLogPixels(int LPY)
{
	m_QGraph.SetLogPixelsY(m_LogPixelsY);
}



bool CXInverse::LoadSettings(CArchive &ar)
{
	COLORREF cr;
	CString str;
	int k;
	try{
		m_QGraph.Serialize(ar);
			ar >> cr;
			if(cr <0 || cr> RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			m_SplineClr = cr;

			ar >> k;
			if(k <0 || k> 20){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			m_SplineStyle = k;
			ar >> k;
			if(k <0 || k> 5){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			m_SplineWidth= k;


			ar >> cr;
			if(cr <0 || cr> RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			m_pRefFoil->m_FoilColor = cr;
			ar >> k;
			if(k <0 || k> 20){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			m_pRefFoil->m_nFoilStyle = k;
			ar >> k;
			if(k <0 || k> 5){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			m_pRefFoil->m_nFoilWidth = k;

			ar >> cr;
			if(cr <0 || cr> RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			m_pModFoil->m_FoilColor = cr;
			ar >> k;
			if(k <0 || k> 20){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			m_pModFoil->m_nFoilStyle = k;
			ar >> k;
			if(k <0 || k> 5){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			m_pModFoil->m_nFoilWidth = k;

			ar >> cr;
			if(cr <0 || cr> RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			m_ReflectedClr = cr;

			ar >> k;
			if(k <0 || k> 20){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			m_ReflectedStyle = k;
			ar >> k;
			if(k <0 || k> 5){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			m_ReflectedWidth= k;

		return true;

	}
	catch (CArchiveException *ex){
		ex->Delete();
		return false;
	}
	catch (CException *ex){
		ex->Delete();
		return false;
	}
//	return true;
}


void CXInverse::SaveSettings(CArchive &ar)
{
	m_QGraph.Serialize(ar);
	ar << m_SplineClr;
	ar << m_SplineStyle;
	ar << m_SplineWidth;

	ar << m_pRefFoil->m_FoilColor;
	ar << m_pRefFoil->m_nFoilStyle;
	ar << m_pRefFoil->m_nFoilWidth;

	ar << m_pModFoil->m_FoilColor;
	ar << m_pModFoil->m_nFoilStyle;
	ar << m_pModFoil->m_nFoilWidth;

	ar << m_ReflectedClr;
	ar << m_ReflectedStyle;
	ar << m_ReflectedWidth;
}


void CXInverse::OnLoadFoil() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;	
	CString FileName;
	CFoil* pFoil;

	CFileDialog XFileDlg(TRUE, "dat", NULL, OFN_HIDEREADONLY, _T("Labeled Format (.dat)|*.dat|"));
	
	if(IDOK==XFileDlg.DoModal()) {
		m_bMark = false;
		m_bMarked = false;
		m_bSpline = false;
		m_bSplined  = true;
		FileName = XFileDlg.GetFileName();
		pFoil = pFrame->ReadFoilFile(FileName);
		pFrame->SetCurrentFoil(pFoil);
		if(!pFoil) return;//
		pFrame->UpdateFoils();
	}
	else return;

	CWaitCursor Wait;

	m_pRefFoil->CopyFoil(pFoil);
	InitXFoil(m_pRefFoil);
	SetFoil();
}


bool CXInverse::InitXFoil(CFoil * pFoil)
{
	//loads pFoil in XFoil, calculates normal vectors, and sets results in current foil
	
	if(!pFoil) return  false;
	m_pModFoil->m_FoilName = pFoil->m_FoilName + " Modified";
	m_pXFoil->m_FoilName = m_pRefFoil->m_FoilName ;

	m_pXFoil->Initialize();
	for(int i =0; i<pFoil->n; i++){
		m_pXFoil->xb[i+1] = pFoil->x[i];
		m_pXFoil->yb[i+1] = pFoil->y[i];
	}
	m_pXFoil->nb     = pFoil->n;
	m_pXFoil->lflap  = false;
	m_pXFoil->lbflap = false;
	m_pXFoil->ddef   = 0.0;
	m_pXFoil->xbf    = 1.0;
	m_pXFoil->ybf    = 0.0;

	m_pXFoil->lqspec = false;
	m_pXFoil->lscini = false;

	if(m_pXFoil->Preprocess()){

		m_pXFoil->CheckAngles();

		for (int k=0; k<m_pXFoil->n;k++){
			pFoil->nx[k] = m_pXFoil->nx[k+1];
			pFoil->ny[k] = m_pXFoil->ny[k+1];
		}
		pFoil->n = m_pXFoil->n;
		return true;
	}
	else {
		AfxMessageBox("Unrecognized foil format", MB_OK);
		return false;
	}
}


void CXInverse::OnRestore() 
{
	if (m_bZoomPlus) ReleaseZoom();

	for (int i=0; i<m_pRefFoil->n; i++){
		m_pXFoil->x[i+1] = m_pRefFoil->x[i];
		m_pXFoil->y[i+1] = m_pRefFoil->y[i];
	}
	InitXFoil(m_pRefFoil);

	if(m_bFullInverse){
		m_pXFoil->InitMDES();
		CreateQCurve();
		CreateMCurve();
//		ResetQ();
	}
	else {
		// Mixed Inverse
		m_pXFoil->InitQDES();
		CreateQCurve();
		CreateMCurve();
//		ResetMixedQ();
	}
	for(i=1; i<=m_pXFoil->n; i++){
		m_pModFoil->x[i-1] = m_pXFoil->x[i];
		m_pModFoil->y[i-1] = m_pXFoil->y[i];
	}
	m_pModFoil->n = m_pXFoil->n;
	m_pModFoil->InitFoil();

	UpdateView();
}


void CXInverse::OnStoreFoil() 
{
	if(!m_bLoaded) return;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	CFoil* pFoil = new CFoil();
	pFoil->CopyFoil(m_pModFoil);
	pFoil->m_nFoilStyle = PS_SOLID;
	pFoil->m_nFoilWidth = 1;
	memcpy(pFoil->xb, m_pModFoil->x, sizeof(m_pModFoil->x));
	memcpy(pFoil->yb, m_pModFoil->y, sizeof(m_pModFoil->y));
	pFoil->nb = m_pModFoil->n;
	pFoil->m_FoilName = m_pRefFoil->m_FoilName;
	pFrame->SetModFoil(pFoil);
}


bool CXInverse::SetXInverseCursor(CWnd* pWnd, CPoint ptMouse, UINT message) 
{
	if (pWnd != m_pChildWnd) return false;
	if(m_bTrans){
		SetCursor(m_hcMove);
		return TRUE;
	}
	if(m_QGraph.IsInDrawRect(ptMouse)){
		SetCursor(m_hcCross);
		return TRUE;
	}
	else {
		SetCursor(m_hcArrow);
		return TRUE;
	}
//	return true;
}



void CXInverse::SetFoil()
{
	int i;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString strong;
	for(i=1; i<=m_pXFoil->n; i++){
		m_pModFoil->x[i-1] = m_pXFoil->x[i];
		m_pModFoil->y[i-1] = m_pXFoil->y[i];
	}
	m_pModFoil->n = m_pXFoil->n;

	if(m_bFullInverse){
		m_pXFoil->InitMDES();
		CreateQCurve();
		CreateMCurve();
//		ResetQ();

		m_pFInvCtrlBar->m_ctrlSpec.SetValue(m_pXFoil->alqsp[1]*180.0/3.141592654);
		m_pFInvCtrlBar->m_ctrlTAngle.SetValue(m_pXFoil->agte*180.0);//agte expressed in pi units:!?!?
		m_pFInvCtrlBar->m_ctrlTGapx.SetValue(real(m_pXFoil->dzte));
		m_pFInvCtrlBar->m_ctrlTGapy.SetValue(imag(m_pXFoil->dzte));
	}
	else {
		// Mixed Inverse
		m_pXFoil->InitQDES();
		CreateQCurve();
		CreateMCurve();
//		ResetMixedQ();
		strong.Format("Alpha = %.3f \r\n      Cl = %.3f", m_pXFoil->algam/m_pXFoil->dtor, m_pXFoil->clgam);
		m_pMInvCtrlBar->m_ctrlSpec.SetWindowText(strong);
		m_pMInvCtrlBar->m_ctrlIter.SetValue(m_pXFoil->niterq);
	}

	if(m_pXFoil->lvisc){
		//a previous xfoil calculation is still active, so add the associated viscous curve
		double x,y;
		double dsp, dqv, sp1, sp2, qv1, qv2;
		m_pQVCurve->n = 0;
		for(i=2; i<= m_pXFoil->n; i++){
			dsp = m_pXFoil->s[i] - m_pXFoil->s[i-1];
			dqv = m_pXFoil->qcomp(m_pXFoil->qvis[i]) - m_pXFoil->qcomp(m_pXFoil->qvis[i-1]);
			sp1 = (m_pXFoil->s[i-1] + 0.25*dsp)/m_pXFoil->s[m_pXFoil->n];
			sp2 = (m_pXFoil->s[i]   - 0.25*dsp)/m_pXFoil->s[m_pXFoil->n];
			qv1 = m_pXFoil->qcomp(m_pXFoil->qvis[i-1]) + 0.25*dqv;
			qv2 = m_pXFoil->qcomp(m_pXFoil->qvis[i]  ) - 0.25*dqv;
			x = 1.0 - sp1;
			y = qv1/m_pXFoil->qinf;
			m_pQVCurve->AddPoint(x,y);
			x = 1.0 - sp2;
			y = qv2/m_pXFoil->qinf;
			m_pQVCurve->AddPoint(x,y);
		}
		m_pQVCurve->SetVisible(true);
	}
	else {
		m_pQVCurve->SetVisible(false);
	}

	m_bLoaded = true;
}

void CXInverse::OnSelectFoil() 
{
	CSelectFoilDlg dlg;
	dlg.m_pMainFrame = m_pFrame;

	if(m_bLoaded)
	{
		dlg.m_FoilName = m_pRefFoil->m_FoilName;
	}

	if(IDOK == dlg.DoModal())
	{
		CWaitCursor Wait;
		m_bMark = false;
		m_bMarked = false;
		m_bSpline = false;
		m_bSplined  = true;
		CFoil *pFoil;
		CMainFrame* pFrame = (CMainFrame*)m_pFrame;
		pFoil = pFrame->GetFoil(dlg.m_FoilName);
		pFrame->SetCurrentFoil(pFoil);
		m_pRefFoil->CopyFoil(pFoil);

		m_pModFoil->m_FoilName = m_pRefFoil->m_FoilName + " Modified";
		InitXFoil(m_pRefFoil);
		SetFoil();
	}
}


void CXInverse::DrawGrid(CDC *pDC, CRect* pCltRect, double scale, CPoint ptOffset, bool bIsPrinting)
{
	CChildView * pChildView = (CChildView*)m_pChildWnd;

	double scalex,scaley;
	int TickSize, xTextOff, offy;
	LOGFONT TempFont;
	memcpy(&TempFont, &pChildView->m_WndLogFont, sizeof(LOGFONT));
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	if (bIsPrinting){
		TempFont.lfHeight = TempFont.lfHeight*30;
		TickSize = -150;
		scalex= scale;
		scaley=-scale;
		xTextOff = 700;
		offy = ptOffset.y;
	}
	else{
		TickSize = 5;
		scalex= scale;
		scaley= scale;
		xTextOff = 14;
		offy = ptOffset.y;
//		offy = pCltRect->bottom-100;
	}
	if(IsBlackAndWhite()){
		lb.lbColor = RGB(0,0,0);
		pDC->SetTextColor(RGB(0,0,0));
	}
	else{
		lb.lbColor = pChildView->m_WndTextColor;
		pDC->SetTextColor(pChildView->m_WndTextColor);
	}

	//neutral line first
	CPen LinePen(PS_GEOMETRIC | PS_SOLID, GetPenWidth(0, bIsPrinting), &lb);
	CPen *pOld = pDC->SelectObject(&LinePen);

	pDC->MoveTo(0, offy);
	pDC->LineTo(pCltRect->right, offy);


	double xo            =  0.0;
	double xmin          =  0.0;
	double xmax          =  1.0;
//	double ymin          = -0.2;
//	double ymax          =  0.2;
	double XGridUnit     =  0.1;
	double XHalfGridUnit =  0.05;
	double XMinGridUnit  =  0.01;

	double xt  = xo-int((xo-xmin)*1.0001/XGridUnit)*XGridUnit;//one tick at the origin
	double xht = xo-int((xo-xmin)*1.0001/XHalfGridUnit)*XHalfGridUnit;//one tick at the origin
	double xmt = xo-int((xo-xmin)*1.0001/XMinGridUnit)*XMinGridUnit;//one tick at the origin


	CString strLabel;
	while(xt<=xmax*1.001){
		//Draw  ticks
		pDC->MoveTo(int(xt*scalex) + ptOffset.x, offy);
		pDC->LineTo(int(xt*scalex) + ptOffset.x, offy+TickSize);
		strLabel.Format("%.1f",xt);
		pDC->TextOut(int(xt*scalex)+ptOffset.x-5, offy+(int)(TickSize*2), strLabel);
		xt += XGridUnit ;
	}
	while(xht<=xmax*1.001){
		//Draw  ticks
		pDC->MoveTo(int(xht*scalex) + ptOffset.x, offy);
		pDC->LineTo(int(xht*scalex) + ptOffset.x, offy+TickSize*2);
		xht += XHalfGridUnit ;
	}
	while(xmt<=xmax*1.001){
		//Draw  ticks
		pDC->MoveTo(int(xmt*scalex) + ptOffset.x, offy);
		pDC->LineTo(int(xmt*scalex) + ptOffset.x, offy+TickSize);
		xmt += XMinGridUnit ;
	}

	pDC->SelectObject(pOld);
	LinePen.DeleteObject();

}

void CXInverse::OnResetFoil() 
{
	ReleaseZoom();
	ResetScale();
	UpdateView();
	
}

void CXInverse::SetColors(Graph* pDefaultGraph)
{
	m_QGraph.CopySettings(pDefaultGraph);
}

void CXInverse::CheckMenu()
{
	CMenu *pMenu = m_pFrame->GetMenu()->GetSubMenu(4);
	if (m_pQCurve->IsVisible())  pMenu->CheckMenuItem(IDM_QINIT,MF_BYCOMMAND | MF_CHECKED);
	else                         pMenu->CheckMenuItem(IDM_QINIT,MF_BYCOMMAND | MF_UNCHECKED);
	if (m_pMCurve->IsVisible())  pMenu->CheckMenuItem(IDM_QMOD,MF_BYCOMMAND | MF_CHECKED);
	else                         pMenu->CheckMenuItem(IDM_QMOD,MF_BYCOMMAND | MF_UNCHECKED);
	if (m_pQVCurve->IsVisible()) pMenu->CheckMenuItem(IDM_QVISC,MF_BYCOMMAND | MF_CHECKED);
	else                         pMenu->CheckMenuItem(IDM_QVISC,MF_BYCOMMAND | MF_UNCHECKED);
	if (m_bShowPoints)           pMenu->CheckMenuItem(IDM_SHOWQPOINTS,MF_BYCOMMAND | MF_CHECKED);
	else                         pMenu->CheckMenuItem(IDM_SHOWQPOINTS,MF_BYCOMMAND | MF_UNCHECKED);
	if (m_bReflected)            pMenu->CheckMenuItem(IDM_SHOWREFLECTED,MF_BYCOMMAND | MF_CHECKED);
	else                         pMenu->CheckMenuItem(IDM_SHOWREFLECTED,MF_BYCOMMAND | MF_UNCHECKED);
	
	pMenu = m_pFrame->GetMenu()->GetSubMenu(3);

	if (m_bRefFoil) pMenu->CheckMenuItem(IDM_SHOWREF,MF_BYCOMMAND | MF_CHECKED);
	else            pMenu->CheckMenuItem(IDM_SHOWREF,MF_BYCOMMAND | MF_UNCHECKED);
	if (m_bModFoil) pMenu->CheckMenuItem(IDM_SHOWMOD,MF_BYCOMMAND | MF_CHECKED);
	else            pMenu->CheckMenuItem(IDM_SHOWMOD,MF_BYCOMMAND | MF_UNCHECKED);
			
}

void CXInverse::OnLoad() 
{
	CString FileName, PathName;
	CFoil *pFoil;
	CFileDialog XFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, 
		_T("XFLR5 file (.dat; .plr; .wpa)|*.dat; *.plr; *.wpa|"));

	if(IDOK==XFileDlg.DoModal()) {
		CWaitCursor Wait;
		FileName = XFileDlg.GetFileName();
		CMainFrame*pFrame = (CMainFrame*)m_pFrame;
		int pos1 = FileName.Find(".plr");
		int pos2 = FileName.Find(".PLR");
		if (pos1>0 || pos2>0) {
			pFrame->LoadFile(XFileDlg.GetFileName(), XFileDlg.GetPathName());
			if(!m_bLoaded){
				if (pFrame->m_oaFoil.GetSize()){
					pFoil = (CFoil*)(pFrame->m_oaFoil.GetAt(0));
					if(!pFoil) return;//

					m_pRefFoil->CopyFoil(pFoil);
					InitXFoil(m_pRefFoil);
					SetFoil();	
				}
			}
		}
		else{
			pos1 = FileName.Find(".wpa");
			pos2 = FileName.Find(".WPA");
			if(pos1>0 || pos2>0) {
				pFrame->LoadFile(XFileDlg.GetFileName(), XFileDlg.GetPathName());
//				if(pFrame->m_oaWing.GetSize()) pFrame->OnMiarex();
				// XFoil is not initialized
				//is there anything in the database ?
				if(pFrame->m_oaFoil.GetSize()){
					pFoil = (CFoil*)pFrame->m_oaFoil.GetAt(0);
					if(!pFoil) return;//

					m_pRefFoil->CopyFoil(pFoil);
					InitXFoil(m_pRefFoil);
					SetFoil();
				}
			}
			else {
				pos1 = FileName.Find(".dat");
				pos2 = FileName.Find(".DAT");
				m_bMark = false;
				m_bMarked = false;
				m_bSpline = false;
				m_bSplined  = true;
				pFoil = pFrame->ReadFoilFile(FileName);
				if(!pFoil) return;//

				m_pRefFoil->CopyFoil(pFoil);
				InitXFoil(m_pRefFoil);
				SetFoil();	
			}		
		}
		UpdateView();
	}	
}



void CXInverse::OnResetRefCurves()
{
	for (int i=3; i<m_QGraph.GetCurveCount(); i++){
		m_QGraph.DeleteCurve(i);
	}
}



void CXInverse::OnShowRefCurves()
{
	m_bRefCurves = ! m_bRefCurves;

	for (int i=3; i<m_QGraph.GetCurveCount(); i++){
		m_QGraph.GetCurve(i)->SetVisible(m_bRefCurves);
	}
	CheckMenu();
	UpdateView();
}


void CXInverse::OnStoreRefCurve()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString strong;
	int i;

	m_bRefCurves = true;
	CheckMenu();

	//duplicate the QCurve for reference

	if(m_bFullInverse){
		if(m_pXFoil->iacqsp == 1){
			strong.Format("_aoa = %.2f",m_pXFoil->alqsp[1]*180.0/3.141592654);
			strong = m_pRefFoil->m_FoilName + strong;
			
		}
		else if(m_pXFoil->iacqsp == 2){
			strong.Format("_Cl = %.2f",m_pXFoil->clqsp[1]);
			strong = m_pRefFoil->m_FoilName + strong;
		}
		else return;
	}
	else {
		strong.Format("_aoa = %.2f",m_pXFoil->algam*180.0/3.141592654);
		strong = m_pRefFoil->m_FoilName + strong;
	}


	bool bFound = false;
	for(i=3; i<m_QGraph.GetCurveCount(); i++){
		if(m_QGraph.GetCurve(i)->GetTitle() == strong){
			bFound = true;
			break;
		}
	}

	if(!bFound){
		CCurve *pRefCurve = m_QGraph.AddCurve();
		pRefCurve->Copy(m_pQCurve);
		pRefCurve->SetColor(pFrame->m_crColors[m_QGraph.GetCurveCount()%24]);

		pRefCurve->SetTitle(strong);
		pRefCurve->SetVisible(m_bRefCurves);
	}
}

