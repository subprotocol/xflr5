/****************************************************************************

    AFoilDlg Class
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


// AFoil.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "../misc/LinePickerDlg.h"
#include "../misc/NameDlg.h"
#include "../misc/PrintOptionsDlg.h"
#include "../XDirect/LEDlg.h"
#include "../XDirect/TEGapDlg.h"
#include "../XDirect/GeomDlg.h"
#include "../XDirect/2DPanelDlg.h"
#include "../XDirect/CAddDlg.h"
#include "../XDirect/InterpolateDlg.h"
#include "../XDirect/EditFoilDlg.h"
#include "../XDirect/FlapDlg.h"
#include "../XDirect/NacaDlg.h"
#include ".\afoil.h"
#include "AGridDlg.h"
#include "LECircleDlg.h"
#include "SplinesCtrlDlg.h"
#include <math.h>

//#define FOILLIST	1500
//#define SPLINELIST	1501
//#define GRIDLIST	1502
//#define POINTLIST	1503
//#define HIGHLIST	1504

/////////////////////////////////////////////////////////////////////////////
// CAFoil

CAFoil::CAFoil()
{
	m_hcArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_hcCross = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
	m_hcMove  = AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_HMOVE));

	m_MousePos.x = 0.0;
	m_MousePos.y = 0.0;

	m_pSF = new CSF();
	m_pSF->m_bModified = false;
	SFNew();
	
	m_pPF = new CPF();
	m_pPF->m_bModified = false;
	PFNew();

	m_bZoomPlus    = false;
	m_bZoomYOnly   = false;
	m_bTrans       = false;
	m_bNeutralLine = false;
	m_bScale       = true;
	m_bCircle      = false;
	m_bShowLegend  = true;
	m_bStored      = false;

	memset(&m_TmpPic,0, sizeof(Picture));

	m_LERad   = 1.0;

	m_bXGrid     = false;
	m_XGridUnit  = 0.05;
	m_XGridStyle = 1;
	m_XGridWidth = 1;
	m_XGridColor = RGB(150,150,150);
	m_bYGrid     = false;
	m_YGridUnit  = 0.05;
	m_YGridStyle = 1;
	m_YGridWidth = 1;
	m_YGridColor = RGB(150,150,150);

	m_bXMinGrid  = false;
	m_XMinorUnit = 0.01;
	m_XMinStyle  = PS_DASH;
	m_XMinWidth  = 1;
	m_XMinColor  = RGB(70,70,70);
	m_bYMinGrid  = false;
	m_YMinorUnit = 0.01;
	m_YMinStyle  = PS_DASH;
	m_YMinWidth  = 1;
	m_YMinColor  = RGB(70,70,70);

	m_NeutralStyle = 3;
	m_NeutralWidth = 1;
	m_NeutralColor = RGB(70,70,70);

	m_fScale    = 1.0;
	m_fRefScale = 1.0;

	m_fScaleY    = 1.0;
	m_ptOffset.x = 0;
	m_ptOffset.y = 0;

	m_pBufferFoil = new CFoil();
	m_pChildWnd = NULL;
	m_rCltRect.SetRectEmpty();
}


 
CAFoil::~CAFoil()
{
	delete m_pSF;
	delete m_pPF;
	delete m_pBufferFoil;
	m_pBufferFoil = NULL;
}

BEGIN_MESSAGE_MAP(CAFoil, CWnd)
	//{{AFX_MSG_MAP(CAFoil)
	ON_COMMAND(IDM_AFOIL_SFSAVE, OnSFSave)
	ON_COMMAND(IDM_AFOIL_EXPORTSPLINES, OnExportSplines)
	ON_COMMAND(IDM_AFOIL_PRINT, OnPrint)
	ON_COMMAND(IDT_ZOOMIN, OnZoomIn)
	ON_COMMAND(IDT_ZOOMLESS, OnZoomLess)
	ON_COMMAND(IDT_ZOOMOUT, OnZoomOut)
	ON_COMMAND(IDM_DELETEPOINT, OnDeletePoint)
	ON_COMMAND(IDM_ADDPOINT, OnAddPoint)
	ON_COMMAND(IDT_UNDO, OnUndo)
	ON_COMMAND(IDT_REDO, OnRedo)
	ON_COMMAND(IDM_AFOIL_GRID, OnGrid)
	ON_COMMAND(IDC_STOREFOIL, OnStoreFoil)
	ON_COMMAND(IDM_RESETVIEW, OnResetView)
	ON_COMMAND(ID_APP_ABOUT, OnAbout)
	ON_COMMAND(IDM_HIDEALLFOILS, OnHideAllFoils)
	ON_COMMAND(IDM_SHOWALLFOILS, OnShowAllFoils)
	ON_COMMAND(IDM_HIDEFOIL, OnHideFoil)
	ON_COMMAND(IDM_DELETEFOIL, OnDeleteFoil)
	ON_COMMAND(IDM_SHOWFOIL, OnShowFoil)
	ON_COMMAND(IDM_RENAMEFOIL, OnRenameFoil)
	ON_COMMAND(IDT_ZOOMYONLY, OnZoomYOnly)
	ON_COMMAND(IDM_RESETYSCALE, OnResetYScale)
	ON_COMMAND(IDM_NORMALIZE, OnNormalize)
	ON_COMMAND(IDM_ROTATEFOIL, OnDeRotate)
	ON_COMMAND(IDM_LERAD, OnLERad)
	ON_COMMAND(IDM_TEGAP, OnTEGap)
	ON_COMMAND(IDM_GEOM, OnGeom)
	ON_COMMAND(IDM_PANEL, OnPanel)
	ON_COMMAND(IDM_CADD, OnCadd)
	ON_COMMAND(IDM_INTERPOLATE, OnInterpolate)
	ON_COMMAND(IDM_DUPLICATEFOIL, OnDuplicate)
	ON_COMMAND(IDM_EXPORTFOIL, OnExportFoil)
	ON_COMMAND(IDM_EDITCOORD, OnEditCoord)
	ON_COMMAND(IDM_SETFLAP, OnSetFlap)
	ON_COMMAND(IDM_NACAFOILS, OnNacaFoils)
	ON_COMMAND(IDM_AFOIL_NEWSPLINES, OnNewSplines)
	ON_COMMAND(IDM_AFOIL_PRINT, OnPrint)
	ON_COMMAND(IDT_PRINT, OnPrint)
	ON_COMMAND(IDM_EXPORTTOAPP, OnStoreFoil)
	ON_COMMAND(IDM_LECIRCLE, OnLECircle)
	ON_COMMAND(IDM_SPLINECONTROLS, OnSplineControls)
	ON_COMMAND(IDM_SHOWLEGEND, OnShowLegend)
	ON_COMMAND(IDM_BSPLINES, OnBSplines)
	ON_COMMAND(IDM_SPLINEDPOINTS, OnSplinedpoints)
	ON_COMMAND(IDM_EDITCTRLPOINTS, OnEditCtrlPoints)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAFoil message handlers


/////////////////////////////////////////////////////////////////////////////
// CAFoil message handlers

void CAFoil::SetParams(CFoil *pFoil) 
{
	m_pACtrl->m_pSF = m_pSF;
	m_pACtrl->m_pPF = m_pPF;

	//	SetScale();	

	if(m_bSF) m_pSF->CompMidLine();
	else      m_pPF->CompMidLine(true);

	// no undo yet
	m_StackPos = 0;
	m_StackSize = 0;

	SetFoils(pFoil);

	CMenu *pMenu = m_pFrame->GetMenu();
	CMenu *pSubMenu = pMenu->GetSubMenu(5);

	if(m_bSF) {
		pMenu->GetSubMenu(5)->CheckMenuItem(IDM_BSPLINES,      MF_BYCOMMAND | MF_CHECKED);
		pMenu->GetSubMenu(5)->CheckMenuItem(IDM_SPLINEDPOINTS, MF_BYCOMMAND | MF_UNCHECKED);
	}
	else{
		pMenu->GetSubMenu(5)->CheckMenuItem(IDM_BSPLINES,      MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->GetSubMenu(5)->CheckMenuItem(IDM_SPLINEDPOINTS, MF_BYCOMMAND | MF_CHECKED);
	}
	pMenu->GetSubMenu(1)->CheckMenuItem(IDM_MIXEDINVERSE,  MF_BYCOMMAND | MF_UNCHECKED);
	pMenu->GetSubMenu(1)->CheckMenuItem(IDM_FULLINVERSE,   MF_BYCOMMAND | MF_UNCHECKED);
	pMenu->GetSubMenu(1)->CheckMenuItem(IDM_XFLR5,         MF_BYCOMMAND | MF_UNCHECKED);
	pMenu->GetSubMenu(1)->CheckMenuItem(IDM_MIAREX,        MF_BYCOMMAND | MF_UNCHECKED);

	if(m_bShowLegend)   pMenu->GetSubMenu(2)->CheckMenuItem(IDM_SHOWLEGEND, MF_BYCOMMAND | MF_CHECKED);
	else				pMenu->GetSubMenu(2)->CheckMenuItem(IDM_SHOWLEGEND, MF_BYCOMMAND | MF_UNCHECKED);

	CToolBarCtrl *pTB = &(m_pAFoilBar->GetToolBarCtrl());
	m_bZoomYOnly = false;
	pTB->PressButton(IDT_ZOOMYONLY, false);

//	m_pACtrl->SelectFoil();
}


void CAFoil::UpdateView(CDC* pDC) 
{
	CChildView * pChildView = (CChildView*)m_pChildWnd;

	COLORREF color;
	int style, width;
	CString str;
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	if(!pDC) pDC = pChildView->GetDC();
	if(!pDC) return;

	CDC memdc; 
	memdc.CreateCompatibleDC (pDC);

	CBitmap  bmp;
	bmp.CreateCompatibleBitmap(pDC, m_rDrawRect.Width(), m_rDrawRect.Height());
	CBitmap * pOldBmp = memdc.SelectObject (&bmp);

	memdc.SetBkColor(pChildView->m_crBackColor);
	memdc.SetBkMode(TRANSPARENT);
	memdc.SetTextColor(pChildView->m_WndTextColor);
	memdc.FillSolidRect(&m_rDrawRect,pChildView->m_crBackColor);

	CFoil *pFoil;
	CPen     *pOldPen;
	CBrush   *pOldBrush;
	CPen BlackPen(PS_SOLID,  1, RGB(0,0,0));
	pOldPen   = memdc.SelectObject(&BlackPen);
	pOldBrush =  (CBrush*)memdc.SelectStockObject(NULL_BRUSH);

	if(m_bZoomPlus){
		CRect ZRect(m_ZoomRect.left   - m_rCltRect.left,
					m_ZoomRect.top    - m_rCltRect.top ,
					m_ZoomRect.right  - m_rCltRect.left,
					m_ZoomRect.bottom - m_rCltRect.top );
		ZRect.NormalizeRect();
		CPen ZoomPen(PS_DASH,1, RGB(100,100,100));
		memdc.SelectObject(&ZoomPen);
		memdc.Rectangle(ZRect);
	}

	if(m_bCircle) {
		int rx = (int)(m_LERad/100.0 * m_fScale);
		int ry = (int)(m_LERad/100.0 * m_fScale * m_fScaleY);
		CRect rc(m_ptOffset.x, m_ptOffset.y - ry, m_ptOffset.x+2*rx,m_ptOffset.y + ry);
		CPen CirclePen(PS_DASH,1,RGB(128,128,128));
		memdc.SelectObject(&CirclePen);
		memdc.Ellipse(&rc);
	}
	if (m_bNeutralLine) {
		color = m_NeutralColor;
		style = m_NeutralStyle;
		width = m_NeutralWidth;
		if(IsBlackAndWhite()) GetBWColor(color, style, width);
		lb.lbColor = color;
		CPen NeutralPen(PS_GEOMETRIC | style,GetPenWidth(width,false), &lb);
		memdc.SelectObject(&NeutralPen);
		memdc.MoveTo(m_rDrawRect.right,m_ptOffset.y);
		memdc.LineTo(m_rDrawRect.left,m_ptOffset.y);
	}

//draw grids
	if(m_bXGrid)	DrawXGrid(&memdc, m_fScale, m_fScale*m_fScaleY, m_ptOffset, m_rDrawRect);
	if(m_bYGrid)	DrawYGrid(&memdc, m_fScale, m_fScale*m_fScaleY, m_ptOffset, m_rDrawRect);
	if(m_bXMinGrid) DrawXMinGrid(&memdc, m_fScale, m_fScale*m_fScaleY, m_ptOffset, m_rDrawRect);
	if(m_bYMinGrid) DrawYMinGrid(&memdc, m_fScale, m_fScale*m_fScaleY, m_ptOffset, m_rDrawRect);

	memdc.SetTextColor(pChildView->m_WndTextColor);
	CFont RFont;
	RFont.CreateFontIndirect(&pChildView->m_WndLogFont);
	CFont *pOldFont = memdc.SelectObject(&RFont);

	if(m_bScale) DrawScale(&memdc, &m_rDrawRect, m_fScale, m_ptOffset,false);

	for (int k=0; k< GetFoilCount(); k++){
		pFoil = (CFoil*)m_poaFoil->GetAt(k);
		if (pFoil->m_bVisible){
			color = pFoil->m_FoilColor;
			style = pFoil->m_nFoilStyle;
			width = pFoil->m_nFoilWidth;
			GetBWColor(color, style, width);
			
			lb.lbColor = color;
			CPen FoilPen(PS_GEOMETRIC | style, GetPenWidth(width, false), &lb);

			memdc.SelectObject(&FoilPen);		
			pFoil->DrawFoil(&memdc, 0.0, m_fScale, m_fScale*m_fScaleY,m_ptOffset, m_rCltRect, false);

			if (pFoil->m_bCenterLine){
				CPen CenterPen(PS_DASH,1,color);
				memdc.SelectObject(&CenterPen);
				pFoil->DrawMidLine(&memdc, m_fScale, m_fScale*m_fScaleY, m_ptOffset, m_rCltRect, false);
			}
			if (pFoil->m_bPoints){
				CPen CtrlPen(PS_SOLID,  1, color);
				memdc.SelectObject(&CtrlPen);
				pFoil->DrawPoints(&memdc, m_fScale,m_fScale*m_fScaleY, m_ptOffset, m_rCltRect, false);
			}
		}
	}

	if (m_pBufferFoil->m_bVisible){
		color = m_pBufferFoil->m_FoilColor;
		style = m_pBufferFoil->m_nFoilStyle;
		width = m_pBufferFoil->m_nFoilWidth;
		GetBWColor(color, style, width);
		
		lb.lbColor = color;
		CPen FoilPen(PS_GEOMETRIC | style, GetPenWidth(width, false), &lb);	

		CPen *pOld0 = memdc.SelectObject(&FoilPen);		
		m_pBufferFoil->DrawFoil(&memdc, 0.0, m_fScale, m_fScale*m_fScaleY,m_ptOffset, m_rCltRect, false);
		memdc.SelectObject(pOld0);
		if (m_pBufferFoil->m_bCenterLine){
			CPen CenterPen(PS_DASH,1,color);
			memdc.SelectObject(&CenterPen);
			m_pBufferFoil->DrawMidLine(&memdc, m_fScale, m_fScale*m_fScaleY, m_ptOffset, m_rCltRect, false);
		}
		if (m_pBufferFoil->m_bPoints){
			CPen CtrlPen(PS_SOLID,  1, color);
			memdc.SelectObject(&CtrlPen);
			m_pBufferFoil->DrawPoints(&memdc, m_fScale,m_fScale*m_fScaleY, m_ptOffset, m_rCltRect, false);
		}
	}
	if (m_bSF){
		if(m_pSF->m_bVisible){
			color = m_pSF->m_FoilColor;
			style = m_pSF->m_FoilStyle;
			width = m_pSF->m_FoilWidth;
			GetBWColor(color, style, width);
			lb.lbColor = color;
			CPen SplinePen(PS_GEOMETRIC | style, GetPenWidth(width,false), &lb);

			memdc.SelectObject(&SplinePen);
			m_pSF->DrawFoil(&memdc, m_fScale,m_fScale*m_fScaleY, m_ptOffset,false);

			
			CPen CtrlPen(PS_SOLID, 1, color);
			memdc.SelectObject(&CtrlPen);
			m_pSF->DrawCtrlPoints(&memdc, m_fScale,m_fScale*m_fScaleY, m_ptOffset, false);
			
			if (m_pSF->m_bCenterLine){
				CPen CenterPen(PS_DASH, 1, color);
				memdc.SelectObject(&CenterPen);
				m_pSF->DrawMidLine(&memdc, m_fScale,m_fScale*m_fScaleY, m_ptOffset, false);
			}
			if (m_pSF->m_bOutPoints){
				CPen OutPen(PS_SOLID,  1, color);
				memdc.SelectObject(&OutPen);
				m_pSF->DrawOutPoints(&memdc, m_fScale,m_fScale*m_fScaleY, m_ptOffset, false);
			}
		}
	}
	else if(m_pPF->m_bVisible)
	{
		color = m_pPF->m_FoilColor;
		style = m_pPF->m_FoilStyle;
		width = m_pPF->m_FoilWidth;
		if(IsBlackAndWhite()) GetBWColor(color, style, width);
		lb.lbColor = color;
		CPen SplinePen(PS_GEOMETRIC | style, GetPenWidth(width,false), &lb);

		memdc.SelectObject(&SplinePen);
		m_pPF->DrawFoil(&memdc, m_fScale, m_fScale*m_fScaleY, m_ptOffset,false);

		CPen CtrlPen(PS_SOLID, 1, color);
		memdc.SelectObject(&CtrlPen);

		m_pPF->DrawCtrlPoints(&memdc, m_fScale,m_fScale*m_fScaleY, m_ptOffset, false);

		if (m_pPF->m_bCenterLine){
			CPen CenterPen(PS_DASH, 1, color);
			memdc.SelectObject(&CenterPen);
			m_pPF->DrawMidLine(&memdc, m_fScale,m_fScale*m_fScaleY, m_ptOffset, false);
		}
	}

	if(m_bShowLegend){
		CPoint Place(m_rCltRect.right-350, 20);
		DrawLegend(&memdc, Place, false);
	}

	str.Format("X-Scale = %4.1f", m_fScale/m_fRefScale);
	memdc.TextOut(5,10, str);
	str.Format("Y-Scale = %4.1f", m_fScaleY*m_fScale/m_fRefScale);
	memdc.TextOut(5,22, str);
	str.Format("x  = %7.4f",m_MousePos.x);
	memdc.TextOut(5,34, str);
	str.Format("y  = %7.4f",m_MousePos.y);
	memdc.TextOut(5,46, str);

	pDC->BitBlt(m_rCltRect.left,m_rCltRect.top,m_rDrawRect.Width(),m_rDrawRect.Height(),
		&memdc,0,0,SRCCOPY);

	pChildView->ReleaseDC(pDC);

	memdc.SelectObject(pOldFont);
	memdc.SelectObject(pOldBmp);

	memdc.SelectObject(pOldPen);
	memdc.SelectObject(pOldBrush);

	memdc.DeleteDC();

}


int CAFoil::GetFoilCount()
{
	return (int)m_poaFoil->GetSize();
}


CFoil* CAFoil::GetFoil(CString FoilName)
{
	CFoil* pRefFoil;
	for (int i=0; i<GetFoilCount(); i++){
		pRefFoil = (CFoil*)m_poaFoil->GetAt(i);
		if (pRefFoil->m_FoilName == FoilName)	return pRefFoil;
	}
	return NULL;
}



int CAFoil::ReadData(CStdioFile *pXFile, CFoil *pFoil)
{
	CString Strong;
	int PointCount = 0;
	double x,y;
	BOOL bRead = TRUE;
 
	int pos =0;
	while(pos>=0 && bRead){
		bRead = pXFile->ReadString(Strong);
		pos = Strong.Find("#",0);
	}
	pFoil->m_FoilName = Strong;

	// remove fore and aft spaces
	pFoil->m_FoilName.TrimLeft();
	pFoil->m_FoilName.TrimRight();
	
	int res;

	while (bRead) {
		bRead = pXFile->ReadString(Strong);
		pos = Strong.Find("#",0);
		if (bRead && pos<0) {
			res = sscanf(Strong, "%lf%lf", &x,&y);
			if(res ==2){
				pFoil->x[PointCount] = x;
				pFoil->y[PointCount] = y;
				PointCount++;
			}
			else bRead = false;
		}
	}
	pFoil->n = PointCount;
	return PointCount;
}



void CAFoil::OnZoomLess() 
{
	// can't do two things at the same time can we ?
	ReleaseZoom();

	double ZoomFactor = 0.8;
	double newScale = __max(ZoomFactor*m_fScale, m_fRefScale);

	ZoomFactor = __max(ZoomFactor, newScale/m_fScale);

	m_fScale = ZoomFactor*m_fScale;
	int a = (int)((m_rDrawRect.right+m_rDrawRect.left)/2);
	int b = (int)((m_rDrawRect.top+m_rDrawRect.bottom)/2);

	//scale
	m_ptOffset.x = (int)(ZoomFactor*(m_ptOffset.x-a)+a);
	m_ptOffset.y = (int)(ZoomFactor*(m_ptOffset.y-b)+b);
	
	UpdateView();
}


void CAFoil::OnZoomOut()
{
	SetScale();
	ReleaseZoom();
	UpdateView();
}


void CAFoil::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CVector Real = MousetoReal(&point);

	// get a reference for mouse movements 
	m_PointDown.x = point.x;
	m_PointDown.y = point.y;//

	if(m_rDrawRect.PtInRect(point)) m_pChildWnd->SetFocus();

	if(m_bZoomPlus && m_rDrawRect.PtInRect(point)){
		m_ZoomRect.left   = point.x;
		m_ZoomRect.top    = point.y;
		m_ZoomRect.right  = point.x;
		m_ZoomRect.bottom = point.y;
	}
	else if(!m_bZoomPlus){
		if(m_bSF){
			if (nFlags & MK_SHIFT) { //shift --> removes the point
				TakePicture();
				StorePicture();
				int n =  m_pSF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale);
				if (n>=0) {
					m_pSF->m_Extrados.RemovePoint(n);
					m_pSF->Update(true);
				}
				else {
					int n=m_pSF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale);
					if (n>=0) {
						m_pSF->m_Intrados.RemovePoint(n);
						m_pSF->Update(false);
					}
				}

			}
			else if (nFlags & MK_CONTROL) {//Ctrl --> inserts a point
				TakePicture();
				StorePicture();

				if(Real.y>=0) {
					m_pSF->m_Extrados.InsertPoint(Real.x,Real.y);
					m_pSF->Update(true);
				}
				else {
					m_pSF->m_Intrados.InsertPoint(Real.x,Real.y);
					m_pSF->Update(false);
				}
			}
			else { //Selects the point
			
				m_pSF->m_Extrados.m_iSelect = m_pSF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale) ;
				m_pSF->m_Intrados.m_iSelect = m_pSF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale) ;
				if (m_pSF->m_Extrados.m_iSelect>=0 || m_pSF->m_Intrados.m_iSelect>=0){
					TakePicture();
				}			
			}
			if(m_pSF->m_Extrados.m_iSelect ==-10 && m_pSF->m_Intrados.m_iSelect ==-10){
				SetCursor(m_hcMove);
				m_bTrans = true;
			}
		}
		else{
			if (nFlags & MK_SHIFT) { //shift --> removes the point
				TakePicture();
				StorePicture();
				int n =  m_pPF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale);
				if (n>=0) {
					m_pPF->m_Extrados.RemovePoint(n);
					m_pPF->Update(true);
				}
				else {
					int n=m_pPF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale);
					if (n>=0) m_pPF->m_Intrados.RemovePoint(n);
					m_pPF->Update(false);
				}
			}
			else if (nFlags & MK_CONTROL) {//Ctrl --> inserts a point
				TakePicture();
				StorePicture();
				if(Real.y>=0) {
					m_pPF->m_Extrados.InsertPoint(Real.x,Real.y);
					m_pPF->Update(true);
				}
				else {
					m_pPF->m_Intrados.InsertPoint(Real.x,Real.y);
					m_pPF->Update(false);
				}
				
			}
			else { //Selects the point
				m_pPF->m_Extrados.m_iSelect = m_pPF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale) ;
				int n =  m_pPF->m_Extrados.IsRearPoint(Real, m_fScale/m_fRefScale) ;
				if(n==-1) m_pPF->m_Extrados.m_iSelect = n;

				m_pPF->m_Intrados.m_iSelect = m_pPF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale) ;
				n =  m_pPF->m_Intrados.IsRearPoint(Real, m_fScale/m_fRefScale) ;
				if(n==-1) m_pPF->m_Intrados.m_iSelect = n;

				TakePicture();
			}
			if(m_pPF->m_Extrados.m_iSelect ==-10 && m_pPF->m_Intrados.m_iSelect ==-10){
				SetCursor(m_hcMove);
				m_bTrans = true;
			}
		}
	}
	UpdateView();	

	CWnd::OnLButtonDown(nFlags, point);
}


void CAFoil::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bTrans = false;
	m_pACtrl->FillFoilList();

	if(m_bZoomPlus && m_rDrawRect.PtInRect(point)){
		CRect ZRect(&m_ZoomRect);
		ZRect.NormalizeRect();
		if(!ZRect.IsRectEmpty()){
			m_ZoomRect.right     = point.x;
			m_ZoomRect.bottom    = point.y;
			m_ZoomRect.NormalizeRect();

			double ZoomFactor = __min((double)m_rDrawRect.Width()  / (double)m_ZoomRect.Width() , 
									 (double)m_rDrawRect.Height() / (double)m_ZoomRect.Height());

			double newScale = __min(ZoomFactor*m_fScale, 32.f*m_fRefScale);
			ZoomFactor = min(ZoomFactor, newScale/m_fScale);
			m_fScale = ZoomFactor*m_fScale;
			int a = (int)((m_rDrawRect.right+m_rDrawRect.left)/2);
			int b = (int)((m_rDrawRect.top+m_rDrawRect.bottom)/2);

			int aZoom = (int)((m_ZoomRect.right+m_ZoomRect.left)/2);
			int bZoom = (int)((m_ZoomRect.top+m_ZoomRect.bottom)/2);

			//translate view
			m_ptOffset.x += (a - aZoom);
			m_ptOffset.y += (b - bZoom);
			//scale view
			m_ptOffset.x = (int)(ZoomFactor*(m_ptOffset.x-a)+a);
			m_ptOffset.y = (int)(ZoomFactor*(m_ptOffset.y-b)+b);
			m_ZoomRect.SetRectEmpty();
		}
		else {
			m_ZoomRect.SetRectEmpty();
			ReleaseZoom();
		}
	}
	else if(m_bZoomPlus && !m_rDrawRect.PtInRect(point)){
		ReleaseZoom();
	}

	else {
		if(m_bSF){
//			ReleaseCapture();// in case we have a grip cursor
			m_pSF->CompMidLine();

/*			int n = m_pSF->m_Extrados.m_iSelect;
			if (n>0 && n<m_pSF->m_Extrados.m_iCtrlPoints) {
				double x = m_pSF->m_Extrados.m_Input[n].x;
				double y = m_pSF->m_Extrados.m_Input[n].y;
			}
			else {
				int n = m_pSF->m_Intrados.m_iSelect;
				if (n>0 && n<m_pSF->m_Intrados.m_iCtrlPoints) {
					double x = m_pSF->m_Intrados.m_Input[n].x;
					double y = m_pSF->m_Intrados.m_Input[n].y;
				}
			}*/
		}
		else{
//			ReleaseCapture();// in case we have a grip cursor
			m_pPF->CompMidLine();

/*			int n = m_pPF->m_Extrados.m_iSelect;
			if (n>0 && n<m_pPF->m_Extrados.m_iPoints) {
				double x = m_pPF->m_Extrados.m_ctrlPoint[n].x;
				double y = m_pPF->m_Extrados.m_ctrlPoint[n].y;
			}
			else {
				int n = m_pPF->m_Intrados.m_iSelect;
				if (n>0 && n<m_pPF->m_Intrados.m_iPoints) {
					double x = m_pPF->m_Intrados.m_ctrlPoint[n].x;
					double y = m_pPF->m_Intrados.m_ctrlPoint[n].y;
				}
			}*/
		}
		SetCursor(m_hcCross);
	}

	UpdateView();
}


void CAFoil::SFNew()
{
	bool bLoadNew = true;
	if(m_pSF->m_bModified){// if something already loaded & modified
		if(IDNO==AfxMessageBox("Discard changes to Splines ?", MB_YESNO)){
			bLoadNew = false;
		}
	}
	if(bLoadNew){// no objection to load new splines ...
		m_pSF->InitSplineFoil();
	}	
}

void CAFoil::PFNew()
{
	bool bLoadNew = true;
	if(m_pPF->m_bModified){// if something already loaded & modified
		if(IDNO==AfxMessageBox("Discard changes to Spline Foil ?", MB_YESNO)){
			bLoadNew = false;
		}
	}
	if(bLoadNew){// no objection to load new splines ...
		m_pPF->InitSplinedFoil();
	}	
}

void CAFoil::OnMButtonDown(UINT nFlags, CPoint point) 
{

	ReleaseZoom();
	m_ZoomRect.SetRectEmpty();
	// reference point for zooming
	m_PointDown.x = point.x;
	m_PointDown.y = point.y;
}


void CAFoil::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(! m_rCltRect.PtInRect(pt)) return;
	m_ZoomRect.SetRectEmpty();
	ReleaseZoom();
	double scale = m_fScale;

	if(!m_bZoomYOnly){
		SHORT shX = GetKeyState('X');
		SHORT shY = GetKeyState('Y');
		if (shX & 0x8000){
			if(zDelta<0){
				m_fScale  *= 1.06;
				m_fScaleY /= 1.06;
			}
			else {
				m_fScale  /= 1.06;
				m_fScaleY *= 1.06;
			}
		}
		else if (shY & 0x8000){
			if(zDelta<0) m_fScaleY *= 1.06;
			else		 m_fScaleY /= 1.06;
		}
		else{
			if(zDelta<0) m_fScale *= 1.06;
			else		 m_fScale /= 1.06;
		}
	}
	else {

		if(zDelta<0) m_fScaleY *= 1.06;
		else		 m_fScaleY /= 1.06;
	}

	int a = (int)((m_rDrawRect.right+m_rDrawRect.left)/2);
	m_ptOffset.x = a + (int)((m_ptOffset.x-a)*m_fScale/scale);

	UpdateView();
}


void CAFoil::OnMouseMove(UINT nFlags, CPoint point) 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	m_MousePos = MousetoReal(&point);

	bool bCtrl = false;
	SHORT sh1  = GetKeyState(VK_LCONTROL);
	SHORT sh2  = GetKeyState(VK_RCONTROL);
	SHORT shZ  = GetKeyState('Z');
	SHORT shX = GetKeyState('X');
	SHORT shY = GetKeyState('Y');

	if(GetFocus()!=m_pChildWnd) m_pChildWnd->SetFocus();

	if ((sh1 & 0x8000)||(sh2 & 0x8000)) bCtrl =true;

	if(m_bZoomPlus && (nFlags & MK_LBUTTON )){
		// we're zooming in using the rectangle method
		m_ZoomRect.right  = point.x;
		m_ZoomRect.bottom = point.y;
		UpdateView();
		return;
	}
	else if(m_rDrawRect.PtInRect(point) && (nFlags & MK_LBUTTON) && m_bTrans){

		//translate
		m_ptOffset.x += point.x - m_PointDown.x;
		m_ptOffset.y += point.y - m_PointDown.y;

		m_PointDown.x = point.x;
		m_PointDown.y = point.y;
		m_MousePos = MousetoReal(&point);
		UpdateView();
		return;
	}
	if(!m_bZoomPlus){	//not zooming, check if mouse passes over control point 
						//and highlight
		CVector Real = MousetoReal(&point);
		if(m_bSF && m_pSF->m_bVisible){
			
			int n = m_pSF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale);
			if (n>0 && n<m_pSF->m_Extrados.m_iCtrlPoints) {
				m_pSF->m_Extrados.m_iHighlight = n;
			}
			else {
				if(m_pSF->m_Extrados.m_iHighlight>=0){
					m_pSF->m_Extrados.m_iHighlight = -10;
				}
			}
			n = m_pSF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale);
			if (n>0 && n<m_pSF->m_Intrados.m_iCtrlPoints) {
				m_pSF->m_Intrados.m_iHighlight = n;
			}
			else {
				if(m_pSF->m_Intrados.m_iHighlight>=0){
					m_pSF->m_Intrados.m_iHighlight = -10;
				}
			}
			UpdateView();
		}
		else if (m_pPF->m_bVisible){
			bool bFound = false;
			int n = m_pPF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale);
			if (n>0 && n<=m_pPF->m_Extrados.m_iPoints) {
				m_pPF->m_Extrados.m_iHighlight = n;
				bFound = true;
			}
			else if(m_pPF->m_Extrados.IsRearPoint(Real, m_fScale/m_fRefScale) == -1){
				m_pPF->m_Extrados.m_iHighlight = -1;
				bFound = true;
			}

			n = m_pPF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale);
			if (n>0 && n<=m_pPF->m_Intrados.m_iPoints) {
				m_pPF->m_Intrados.m_iHighlight = n;
				bFound = true;
			}
			else if(m_pPF->m_Intrados.IsRearPoint(Real, m_fScale/m_fRefScale) == -1){
				m_pPF->m_Intrados.m_iHighlight = -1;
				bFound = true;
			}
			if(!bFound){
				m_pPF->m_Extrados.m_iHighlight = -10;
				m_pPF->m_Intrados.m_iHighlight = -10;
			}	
			UpdateView();
		}
	}
	if (nFlags & MK_LBUTTON   && !m_bZoomPlus) {
		// user is dragging the point
		if(m_rDrawRect.PtInRect(point)){
			if(m_bSF){
				int n = m_pSF->m_Extrados.m_iSelect;
				if (n>=0 && n<=m_pSF->m_Extrados.m_iCtrlPoints) {
					if(!m_bStored) StorePicture();//save for undo only the first time
//					if(n==1) m_MousePos.x = 0.0;// we can't move point 1 for vertical slope
					m_pSF->m_Extrados.m_Input[n].x = m_MousePos.x;
					m_pSF->m_Extrados.m_Input[n].y = m_MousePos.y;
//					m_pSF->m_Extrados.SplineCurve();
					m_pSF->m_bModified = true;
					SetSaveState(false);
					m_pSF->Update(true);
				}
				else {
					int n = m_pSF->m_Intrados.m_iSelect;
					if (n>=0 && n<=m_pSF->m_Intrados.m_iCtrlPoints) {
						if(!m_bStored) StorePicture();//save for undo only the first time
//						if(n==1) m_MousePos.x = 0.0;// we can't move point 1 for vertical slope
						m_pSF->m_Intrados.m_Input[n].x = m_MousePos.x;
						m_pSF->m_Intrados.m_Input[n].y = m_MousePos.y;
//						m_pSF->m_Intrados.SplineCurve();
						m_pSF->m_bModified = true;
						SetSaveState(false);
						m_pSF->Update(false);
					}
				}
			}
			else{
				int n = m_pPF->m_Extrados.m_iSelect;
				if (n>=0 && n<m_pPF->m_Extrados.m_iPoints) {
					if(!m_bStored) StorePicture();//save for undo only the first time
					m_pPF->m_Extrados.m_ctrlPoint[n].x = m_MousePos.x;
					m_pPF->m_Extrados.m_ctrlPoint[n].y = m_MousePos.y;
					m_pPF->m_bModified = true;
					SetSaveState(false);
				}
				else {
					n = m_pPF->m_Intrados.m_iSelect;
					if (n>=0 && n<m_pPF->m_Intrados.m_iPoints) {
						if(!m_bStored) StorePicture();//save for undo only the first time
						m_pPF->m_Intrados.m_ctrlPoint[n].x = m_MousePos.x;
						m_pPF->m_Intrados.m_ctrlPoint[n].y = m_MousePos.y;
						m_pPF->m_bModified = true;
						SetSaveState(false);
					}
				}
				if(n<0) {//check for rear point
					if(m_pPF->m_Extrados.m_iSelect == -1){
						if(!m_bStored) StorePicture();//save for undo only the first time
						m_pPF->m_Extrados.m_RearPoint.x = m_MousePos.x;
						m_pPF->m_Extrados.m_RearPoint.y = m_MousePos.y;
						m_pPF->m_bModified = true;
						SetSaveState(false);
						m_pPF->Update(true);
					}
					else if(m_pPF->m_Intrados.m_iSelect == -1){
						if(!m_bStored) StorePicture();//save for undo only the first time
						m_pPF->m_Intrados.m_RearPoint.x = m_MousePos.x;
						m_pPF->m_Intrados.m_RearPoint.y = m_MousePos.y;
						m_pPF->m_bModified = true;
						SetSaveState(false);
						m_pPF->Update(false);
					}
				}
				m_pPF->CompMidLine();
			}
//			m_pACtrl->FillFoilList();
		}

		UpdateView();
		return;
	}
	else if (((nFlags & MK_MBUTTON) || (shZ & 0x8000))  && !bCtrl) {
		// user is zooming with mouse button down rather than with wheel
		if(m_rDrawRect.PtInRect(point)){		
			double scale = m_fScale;
			
			if(!m_bZoomYOnly){
				SHORT shX = GetKeyState('X');
				SHORT shY = GetKeyState('Y');
				if (shX & 0x8000){
					if(point.y-m_PointDown.y>0){
						m_fScale  *= 1.06;
						m_fScaleY /= 1.06;
					}
					else {
						m_fScale  /= 1.06;
						m_fScaleY *= 1.06;
					}
				}
				else if (shY & 0x8000){
					if(point.y-m_PointDown.y>0) m_fScaleY *= 1.06;
					else		 m_fScaleY /= 1.06;
				}
				else{
					if(point.y-m_PointDown.y>0) m_fScale *= 1.06;
					else		 m_fScale /= 1.06;
				}

			}
			else{
				if(point.y-m_PointDown.y>0) m_fScaleY *= 1.06;
				else						m_fScaleY /= 1.06;
			}

			m_PointDown = point;

			int a = (int)((m_rDrawRect.right+m_rDrawRect.left)/2);
			m_ptOffset.x = a + (int)((m_ptOffset.x-a)*m_fScale/scale);
		}

		UpdateView();
		return;
	}
	UpdateView();
}



void CAFoil::OnExportSplines() 
{
	CFoil *pFoil;
	CStringArray NameList;
	for(int k=0; k<m_poaFoil->GetSize(); k++){
		pFoil = (CFoil*)m_poaFoil->GetAt(k);
		NameList.Add(pFoil->m_FoilName);
	}
	CNameDlg dlg(this);
	dlg.m_pStrArray = & NameList;
	dlg.m_strQuestion = "Enter a name for the splines";
	dlg.m_strName = "Spline Foil";
	if(dlg.DoModal() != IDOK) return;

	CString FoilName = dlg.m_strName;
	CString FileName;

	CStdioFile XFile;
	CFileException fe;
	
	bool bNotFound= true;
	bool bWrite = false;

	// deselect points so as not to interfere with other mouse commands
	m_pSF->m_Intrados.m_iSelect = -10;
	m_pSF->m_Extrados.m_iSelect = -10;
	m_pPF->m_Intrados.m_iSelect = -10;
	m_pPF->m_Extrados.m_iSelect = -10;

	//check that number of output points is compatible with array sizing
	if(m_bSF){
		if(m_pSF->m_Extrados.m_iRes>IQX2) {
			CString strong;
			strong.Format("Too many output points on upper surface\n Max =%d", IQX2);
			AfxMessageBox( strong, MB_OK);
			return;
		}
		if(m_pSF->m_Intrados.m_iRes>IQX2) {
			CString strong;
			strong.Format("Too many output points on lower surface\n Max =%d", IQX2);
			AfxMessageBox( strong, MB_OK);
			return;
		}
	}
	else {
		int size = m_pPF->m_Extrados.m_iPoints * (m_pPF->m_Extrados.m_Freq-1) ;//+ 1;
		if(size>IQX2) {
			CString strong;
			strong.Format("Too many output points on upper surface\n Max =%d", IQX2);
			AfxMessageBox( strong, MB_OK);
			return;
		}
		size = m_pPF->m_Intrados.m_iPoints * (m_pPF->m_Intrados.m_Freq-1) ;//+ 1;
		if(size>IQX2) {
			CString strong;
			strong.Format("Too many output points on lower surface\n Max =%d", IQX2);
			AfxMessageBox( strong, MB_OK);
			return;
		}
	}

	while(bNotFound){// until we find a file to export to
		FoilName.Replace("/", " ");
		CFileDialog XFileDlg(false, "dat", FoilName, NULL,
			_T("Labeled Format (.dat)|*.dat|"));
		if(IDOK==XFileDlg.DoModal()) FileName = XFileDlg.GetFileName();

		CFileFind FileFinder; 
		
		if(!FileFinder.FindFile(FileName)) {// file does not exist : OK to create and export
			bWrite = true;
			bNotFound = false;
		}
		else{
			if(IDYES==AfxMessageBox("Overwrite existing file ?", MB_YESNO)){
				bWrite = true;// file exists, but OK to overwrite and export
				bNotFound = false;
			}
			else bWrite = false;// file exists, but user does not wan't to overwrite
		}
	}
	if(bWrite){
		BOOL bOpen = XFile.Open(FileName, CFile::modeCreate | CFile::modeWrite, &fe);
		if (bOpen){
			CString strOut;
			strOut = FoilName + "\n";
			XFile.WriteString(strOut);
			if(m_bSF) {
//				XFile.WriteString("1\n");		
				m_pSF->Export(&XFile);
			}
			else {
//				XFile.WriteString("2\n");		
				m_pPF->Export(&XFile);
			}
			XFile.Close();
		}
	}
}


void CAFoil::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(nFlags | MK_LBUTTON){}// removes level 4 warning at compile time

	if (m_rDrawRect.PtInRect(point)){
		int a = (int)((m_rDrawRect.right+m_rDrawRect.left)/2);
		int b = (int)((m_rDrawRect.top+m_rDrawRect.bottom)/2);
		//translate
		m_ptOffset.x = m_ptOffset.x	+ (a - point.x);
		m_ptOffset.y = m_ptOffset.y	+ (b - point.y);
		UpdateView();
	}
}


void CAFoil::OnNewSplines() 
{
	if(m_bSF) SFNew();
	else PFNew();

	m_StackPos  = 0;
	m_StackSize = 0;

	SetSaveState(false);
	UpdateView();
}


void CAFoil::OnPrint() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	bool bStatus = IsBlackAndWhite();

	CDC dc;
	CPrintDialog dlgPrint(false);

	if(IDOK==dlgPrint.DoModal()){
		DOCINFO di;
		ZeroMemory(&di, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);

		CString str = "XFLR5 - " + m_pSF->m_strFoilName;
		di.lpszDocName = str;
		dc.Attach(dlgPrint.GetPrinterDC());
		
		// set landscape mode
		DEVMODE *pDevMode = dlgPrint.GetDevMode();
		int h, w;
		GetPaperSize(pDevMode, w, h);

		CPrintOptionsDlg dlg;
		if (bStatus) dlg.m_bColor = false;
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

		CRect CltRect(  (int)(dlg.m_Left*100.0),
						(int)(dlg.m_Top*100.0),
						(int)(h*10-dlg.m_Right*100.0),
						(int)(-w*10+dlg.m_Bottom*100.0));// x 0.01 mm


//		CRect CltRect0(0,0,h*10,-w*10);// x 0.01 mm

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

bool CAFoil::PrintAll(CDC *pDC, CRect pRect)
{
	CChildView *pChildView = (CChildView*)m_pChildWnd;
	pDC->SetMapMode(MM_HIMETRIC);
	if(GetWindowBk() && !IsBlackAndWhite()){
		pDC->SetBkMode(OPAQUE);

		CPen WhitePen;
		WhitePen.CreateStockObject(WHITE_PEN);
		CPen *pOldPen = pDC->SelectObject(&WhitePen);

		CBrush BkBrush(pChildView->m_crBackColor);
		CBrush *pOldBrush = pDC->SelectObject(&BkBrush);
		pDC->Rectangle(pRect);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);

	}

	COLORREF color;
	int style, width;
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	double scalex, scaley;
	CPoint Offset;
	pRect.NormalizeRect();

	CPen AnyPen(PS_SOLID,50,RGB(0,0,0));
	// we need to get a pointer to the old pen
	// because we don't know which type of foil is selected
	CPen* pOldPen = pDC->SelectObject(&AnyPen);

	double conv = (double)(pRect.Width()/m_rCltRect.Width());
	scalex   =  m_fScale * conv;
	scaley   =  m_fScale * conv * m_fScaleY;
	Offset.x =  int(m_ptOffset.x*conv);
	Offset.y = -int(m_ptOffset.y*conv)-3000;


	if(m_bCircle) {
		lb.lbColor = RGB(128,128,128);
		int rx = (int)(m_LERad/100.0 * scalex);
		int ry = (int)(m_LERad/100.0 * scalex * m_fScaleY);
		CRect rc(Offset.x, Offset.y - ry, Offset.x+2*rx,Offset.y + ry);
		CPen CirclePen(PS_GEOMETRIC | PS_DASHDOT,
						GetPenWidth(1,true),&lb);
		CPen *pOld = (CPen*)pDC->SelectObject(&CirclePen);
		pDC->Ellipse(&rc);
		pDC->SelectObject(pOld);
	}

	if(m_bNeutralLine){
		color = m_NeutralColor;
		style = m_NeutralStyle;
		width = m_NeutralWidth;
		if(IsBlackAndWhite()) GetBWColor(color, style, width);
		lb.lbColor = color;
		CPen NeutralPen(PS_GEOMETRIC | style, GetPenWidth(width,true), &lb);
		CPen *pOld1 = (CPen*)pDC->SelectObject(&NeutralPen);
		pDC->MoveTo(pRect.right,Offset.y);
		pDC->LineTo(pRect.left,Offset.y);
		pDC->SelectObject(pOld1);
	}


//draw grids
	if(m_bXGrid) DrawXGrid(pDC, scalex,scaley, Offset, pRect, true);
	if(m_bYGrid) DrawYGrid(pDC, scalex,scaley, Offset, pRect, true);
	if(m_bXMinGrid) DrawXMinGrid(pDC, scalex,scaley, Offset, pRect, true);
	if(m_bYMinGrid) DrawYMinGrid(pDC, scalex,scaley, Offset, pRect, true);

	// start with the reference foils
	for (int i=0; i< m_poaFoil->GetSize();i++){
		CFoil* pFoil = (CFoil*)m_poaFoil->GetAt(i);		
		if(pFoil->m_bVisible){
			color = pFoil->m_FoilColor;
			style = pFoil->m_nFoilStyle;
			width = pFoil->m_nFoilWidth;
			if(IsBlackAndWhite()) 
				GetBWColor(color, style, width);
			lb.lbColor = color;
			CPen ContourPen(PS_GEOMETRIC | style, GetPenWidth(width,true), &lb);

			pDC->SelectObject(&ContourPen);
			pFoil->DrawFoil(pDC, 0.0, scalex, scaley, Offset, *pRect, true);
			if (pFoil->m_bCenterLine){
				lb.lbColor = color;
				CPen CenterPen(PS_GEOMETRIC | PS_DASH, 20, &lb);
				pDC->SelectObject(&CenterPen);
				pFoil->DrawMidLine(pDC, scalex, scaley, Offset, *pRect, true);
			}
			if (pFoil->m_bPoints){
				CPen PointsPen(PS_SOLID, 20,  color);
				pDC->SelectObject(&PointsPen);
				pFoil->DrawPoints(pDC, scalex, scaley, Offset, *pRect, true);
			}
		}
	}

// on with the spline foil 
	if (m_bSF){
		if(m_pSF->m_bVisible){
			m_pSF->SetViewRect(pRect);
			color = m_pSF->m_FoilColor;
			style = m_pSF->m_FoilStyle;
			width = m_pSF->m_FoilWidth;
			if(IsBlackAndWhite()) GetBWColor(color, style, width);
			lb.lbColor = color;

			CPen ContourPen(PS_GEOMETRIC | style, GetPenWidth(width,true), &lb);

			pDC->SelectObject(&ContourPen);
			m_pSF->DrawFoil(pDC, scalex, scaley, Offset, true);

			CPen PointsPen(PS_SOLID, 20,  color);
			pDC->SelectObject(&PointsPen);
			m_pSF->DrawCtrlPoints(pDC, scalex, scaley, Offset, true);

			if (m_pSF->m_bCenterLine){
				lb.lbColor = color;
				CPen CenterPen(PS_GEOMETRIC | PS_DASH, 20, &lb);
				pDC->SelectObject(&CenterPen);
				m_pSF->DrawMidLine(pDC, scalex, scaley, Offset, true);
			}
			if (m_pSF->m_bOutPoints){
				CPen PointsPen(PS_SOLID, 20,  RGB(150,150,150));
				pDC->SelectObject(&PointsPen);
				m_pSF->DrawOutPoints(pDC, scalex, scaley, Offset, true);
			}
		}
	}
	else if(m_pPF->m_bVisible){
		m_pPF->SetViewRect(pRect);
		color = m_pPF->m_FoilColor;
		style = m_pPF->m_FoilStyle;
		width = m_pPF->m_FoilWidth;
		if(IsBlackAndWhite()) GetBWColor(color, style, width);
		lb.lbColor = color;

		CPen ContourPen(PS_GEOMETRIC | style, GetPenWidth(width,true), &lb);

		pDC->SelectObject(&ContourPen);
		m_pPF->DrawFoil(pDC, scalex, scaley, Offset, true);

		if (m_pPF->m_bCenterLine){
			lb.lbColor = color;
			CPen CenterPen(PS_GEOMETRIC | PS_DASH, 20, &lb);
			pDC->SelectObject(&CenterPen);
			m_pPF->DrawMidLine(pDC, scalex, scaley, Offset, true);
		}
		
		CPen PointsPen(PS_SOLID, 20, color);
		pDC->SelectObject(&PointsPen);
		m_pPF->DrawCtrlPoints(pDC, scalex, scaley, Offset, true);
		
	}

	pDC->SelectObject(pOldPen);

	//last print the foils' name
	CPoint Place (pRect.left+2000, -1500);
	DrawLegend(pDC, Place, true);

	CFont VFont;
	pDC->SetTextColor(0);// black version name
	VFont.CreateFont(280,0,0,0,FW_NORMAL,FALSE,FALSE,0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, FF_SWISS+VARIABLE_PITCH, "Arial");

	CFont * pOldFont = pDC->SelectObject(&VFont);
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pDC->SetTextAlign(TA_RIGHT);
	pDC->TextOut(pRect.right,pRect.top, pFrame->m_VersionName);

	pDC->SelectObject(pOldFont);
//	VFont.DeleteObject();

	return true;
}



BOOL CAFoil::PreTranslateMessage(MSG* pMsg) 
{
	double fZoom;
	double fDelta = 0.005;
	fZoom = m_fScale/630.0;
	CMainFrame *pFrame = (CMainFrame*)(m_pFrame);

	if (pMsg->message == WM_KEYDOWN){
		if (pMsg->wParam == VK_ESCAPE){
			if(m_bZoomPlus){
				ReleaseZoom();
				return true; // no need to process further
			}
			else if(m_bZoomYOnly){
				CToolBarCtrl *pTB = &(m_pAFoilBar->GetToolBarCtrl());
				pTB->PressButton(IDT_ZOOMYONLY, false);
				m_bZoomYOnly = false;
			}
		}
		if(pMsg->wParam == VK_UP){ 
			int i = m_pSF->m_Extrados.m_iSelect;
			if (i>0){
				m_pSF->m_Extrados.m_Input[i].y += fDelta/fZoom;
				m_pSF->Update(true);

				UpdateView();
				Sleep(50);
				return true; // no need to process further
			}
			i = m_pSF->m_Intrados.m_iSelect;
			if (i>0){
				m_pSF->m_Intrados.m_Input[i].y += fDelta/fZoom;
				m_pSF->Update(false);

				UpdateView();
				Sleep(50);
				return true; // no need to process further
			}
		} 
		if (pMsg->wParam == VK_DOWN){
			int i = m_pSF->m_Extrados.m_iSelect;
			if (i>0){
				m_pSF->m_Extrados.m_Input[i].y -= fDelta/fZoom;
				m_pSF->Update(true);

				UpdateView();
				Sleep(50);
				return true; // no need to process further
			}
			i = m_pSF->m_Intrados.m_iSelect;
			if (i>0){
				m_pSF->m_Intrados.m_Input[i].y -= fDelta/fZoom;
				m_pSF->Update(false);

				UpdateView();
				Sleep(50);
				return true; // no need to process further
			}
		} 
		if (pMsg->wParam == VK_RIGHT){ 
			int i = m_pSF->m_Extrados.m_iSelect;
			if (i>0){
				m_pSF->m_Extrados.m_Input[i].x += fDelta/fZoom;
				m_pSF->Update(true);

				UpdateView();
				Sleep(50);
				return true; // no need to process further
			}
			i = m_pSF->m_Intrados.m_iSelect;
			if (i>0){
				m_pSF->m_Intrados.m_Input[i].x += fDelta/fZoom;
				m_pSF->Update(false);

				UpdateView();
				Sleep(50);
				return true; // no need to process further
			}
		} 
		if (pMsg->wParam == VK_LEFT){ 
			int i = m_pSF->m_Extrados.m_iSelect;
			if (i>0){
				m_pSF->m_Extrados.m_Input[i].x -= fDelta/fZoom;
				m_pSF->Update(true);

				UpdateView();
				Sleep(50);
				return true; // no need to process further
			}
			i = m_pSF->m_Intrados.m_iSelect;
			if (i>0){
				m_pSF->m_Intrados.m_Input[i].x -= fDelta/fZoom;
				m_pSF->Update(false);

				UpdateView();
				Sleep(50);
				return true; // no need to process further
			}
		} 

		if (pMsg->wParam == VK_F2){ 
			OnRenameFoil();
			return true;
		} 
		if (pMsg->wParam == VK_F4){ 
			OnStoreFoil();
			return true;
		} 
		if (pMsg->wParam == VK_F6){ 
			OnSplineControls();
			return true;
		} 
		if (pMsg->wParam == VK_F7){ 
			m_pACtrl->m_ctrlFoilList.SetFocus();
			return true;
		} 

		SHORT sh1 = GetKeyState(VK_LCONTROL);
		SHORT sh2 = GetKeyState(VK_RCONTROL);

		if (pMsg->wParam == 'Z' && 
			( (sh1 & 0x8000)||(sh2 & 0x8000) )) { 
			OnUndo();
			return true;
		} 
		if (pMsg->wParam == 'Z') { 
			return true;//User is zooming with 'Z' key instead of mouse midle button
		} 
		if (pMsg->wParam == 'Y' && 
			( (sh1 & 0x8000)||(sh2 & 0x8000) )) { 
			OnRedo();
			return true;
		} 
		if (pMsg->wParam == 'E' && 
				( (sh1 & 0x8000)||(sh2 & 0x8000) )) { 
			OnSFSave();
			return true;
		} 

		if (pMsg->wParam == 'P' && 
				( (sh1 & 0x8000)||(sh2 & 0x8000) )) { 
			OnPrint();
			return true;
		}
		if (pMsg->wParam == 'S' && 
				( (sh1 & 0x8000)||(sh2 & 0x8000) )) { 
			SaveProject();
			return true;
		}
	}
	return CWnd::PreTranslateMessage(pMsg);
}


void CAFoil::SetScale()
{//scale is set by user zooming
	m_fRefScale = (double)m_rCltRect.Width()-150.0;

	m_fScale = m_fRefScale;
	
	double width  = (double)m_rCltRect.Width();
	double height = (double)m_rCltRect.Height();
//	double clippedh = height/width * 20.0;

	m_ptOffset.x = 75;
	m_ptOffset.y = (int)(m_rDrawRect.Height()/2);

	m_pSF->SetViewRect(m_rDrawRect);
	m_pPF->SetViewRect(m_rDrawRect);

}


void CAFoil::SetScale(CRect CltRect)
{//scale is set by ChildView
	m_rCltRect = CltRect;
	m_rDrawRect.CopyRect(m_rCltRect);

	int width = m_rCltRect.Width();
	if(width){
		m_pACtrl->SetWindowPos(NULL,0,0,m_rCltRect.Width()-10,200,SWP_SHOWWINDOW);
	}
	SetScale();
}


void CAFoil::OnContextMenu(CPoint ScreenPoint, CPoint ClientPoint) 
{
	if(m_bZoomPlus) ReleaseZoom();
	m_ptPopUp.x = ClientPoint.x;
	m_ptPopUp.y = ClientPoint.y;

	if(m_rDrawRect.PtInRect(m_ptPopUp)){
		CMenu menu;
		if (menu.LoadMenu(IDR_CTXDESIGNMENU)){
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);
			if(m_bShowLegend)   pPopup->CheckMenuItem(IDM_SHOWLEGEND, MF_BYCOMMAND | MF_CHECKED);
			else				pPopup->CheckMenuItem(IDM_SHOWLEGEND, MF_BYCOMMAND | MF_UNCHECKED);
			
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				ClientPoint.x, ClientPoint.y, m_pFrame); // use main window for cmds
		}
	}
}


void CAFoil::OnDeletePoint() 
{
	CVector Real = MousetoReal(&m_ptPopUp);

	if(m_bSF){
		TakePicture();
		StorePicture();
		int n =  m_pSF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale);
		if (n>=0){
			m_pSF->m_Extrados.RemovePoint(n);
			m_pSF->Update(true);
		}
		else {
			int n=m_pSF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale);
			if (n>=0) {
				m_pSF->m_Intrados.RemovePoint(n);
				m_pSF->Update(true);
			}
		}
	}
	else{
		TakePicture();
		StorePicture();
		int n =  m_pPF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale);
		if (n>=0) {
			m_pPF->m_Extrados.RemovePoint(n);
			m_pPF->Update(true);
		}
		else {
			int n=m_pPF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale);
			if (n>=0) {
				m_pPF->m_Intrados.RemovePoint(n);
				m_pPF->Update(false);
			}
		}
	}
	m_pACtrl->FillFoilList();
	UpdateView();
}


void CAFoil::OnAddPoint()
{

	CVector Real = MousetoReal(&m_ptPopUp);

	if(m_bSF){
		TakePicture();
		StorePicture();
		if(Real.y>=0) {
			if(m_pSF->m_Extrados.m_iCtrlPoints >=50) {
				AfxMessageBox("Cannot add more points", MB_OK);
				return;
			}
			else {
				m_pSF->m_Extrados.InsertPoint(Real.x,Real.y);
				m_pSF->Update(true);
			}
		}
		else 
			if(m_pSF->m_Intrados.m_iCtrlPoints >=50) {
				AfxMessageBox("Cannot add more points", MB_OK);
				return;
			}
			else {
				m_pSF->m_Intrados.InsertPoint(Real.x,Real.y);
				m_pSF->Update(false);
			}
	
	}
	else{
		TakePicture();
		StorePicture();
		if(Real.y>=0) {
			m_pPF->m_Extrados.InsertPoint(Real.x,Real.y);
			m_pSF->Update(true);
		}
		else {
			m_pPF->m_Intrados.InsertPoint(Real.x,Real.y);
			m_pSF->Update(false);
		}
	}
	m_pACtrl->FillFoilList();
	UpdateView();
	
}


void CAFoil::TakePicture()
{
	m_bStored = false;
	if(m_bSF){
		m_TmpPic.m_iExt = m_pSF->m_Extrados.m_iCtrlPoints;
		for (int i=0; i<=m_TmpPic.m_iExt; i++){
			m_TmpPic.xExt[i] = m_pSF->m_Extrados.m_Input[i].x;
			m_TmpPic.yExt[i] = m_pSF->m_Extrados.m_Input[i].y;
		}
		m_TmpPic.m_iInt = m_pSF->m_Intrados.m_iCtrlPoints;
		for (i=0; i<=m_TmpPic.m_iInt; i++){
			m_TmpPic.xInt[i] = m_pSF->m_Intrados.m_Input[i].x;
			m_TmpPic.yInt[i] = m_pSF->m_Intrados.m_Input[i].y;
		}
	}
	else{
		m_TmpPic.m_iExt = m_pPF->m_Extrados.m_iPoints;
		for (int i=0; i<=m_TmpPic.m_iExt; i++){
			m_TmpPic.xExt[i] = m_pPF->m_Extrados.m_ctrlPoint[i].x;
			m_TmpPic.yExt[i] = m_pPF->m_Extrados.m_ctrlPoint[i].y;
		}
		m_TmpPic.ExtRearPt.x = m_pPF->m_Extrados.m_RearPoint.x;
		m_TmpPic.ExtRearPt.y = m_pPF->m_Extrados.m_RearPoint.y;

		m_TmpPic.m_iInt = m_pPF->m_Intrados.m_iPoints;
		for (i=0; i<=m_TmpPic.m_iInt; i++){
			m_TmpPic.xInt[i] = m_pPF->m_Intrados.m_ctrlPoint[i].x;
			m_TmpPic.yInt[i] = m_pPF->m_Intrados.m_ctrlPoint[i].y;
		}
		m_TmpPic.IntRearPt.x = m_pPF->m_Intrados.m_RearPoint.x;
		m_TmpPic.IntRearPt.y = m_pPF->m_Intrados.m_RearPoint.y;
	}
}

void CAFoil::StorePicture()
{
	if(m_StackPos>=50){
		for (int i=1; i<50; i++){
			memcpy(&m_UndoPic[i-1],&m_UndoPic[i], sizeof(Picture));
		}
		m_StackPos = 49;
		m_StackSize = 49;
	}
	memcpy(&m_UndoPic[m_StackPos], &m_TmpPic, sizeof(Picture));
	m_bStored = true;
	m_StackPos++;
	m_StackSize = m_StackPos;

	CToolBarCtrl *pTB = &(m_pAFoilBar->GetToolBarCtrl());
	pTB->EnableButton(IDT_UNDO, true);
}

void CAFoil::SetPicture()
{
	double gap;

	if(m_bSF)
	{
		 m_pSF->m_Extrados.m_iCtrlPoints = m_UndoPic[m_StackPos].m_iExt;
		 m_pSF->m_Intrados.m_iCtrlPoints = m_UndoPic[m_StackPos].m_iInt;

		for (int i=0; i<=m_UndoPic[m_StackPos].m_iExt; i++)
		{
			m_pSF->m_Extrados.m_Input[i].x = m_UndoPic[m_StackPos].xExt[i];
			m_pSF->m_Extrados.m_Input[i].y = m_UndoPic[m_StackPos].yExt[i];
		}
		for (i=0; i<=m_UndoPic[m_StackPos].m_iInt; i++)
		{
			m_pSF->m_Intrados.m_Input[i].x = m_UndoPic[m_StackPos].xInt[i];
			m_pSF->m_Intrados.m_Input[i].y = m_UndoPic[m_StackPos].yInt[i];
		}

		gap =   m_pSF->m_Extrados.m_Input[m_pSF->m_Extrados.m_iCtrlPoints].y
			  - m_pSF->m_Intrados.m_Input[m_pSF->m_Intrados.m_iCtrlPoints].y;

		m_pSF->UpdateKnots();
		m_pSF->Update(true);
		m_pSF->Update(false);
	}
	else {
		 m_pPF->m_Extrados.m_iPoints = m_UndoPic[m_StackPos].m_iExt;
		 m_pPF->m_Intrados.m_iPoints = m_UndoPic[m_StackPos].m_iInt;

		 m_pPF->m_Extrados.m_RearPoint.x = m_UndoPic[m_StackPos].ExtRearPt.x;
		 m_pPF->m_Extrados.m_RearPoint.y = m_UndoPic[m_StackPos].ExtRearPt.y;
		 m_pPF->m_Intrados.m_RearPoint.x = m_UndoPic[m_StackPos].IntRearPt.x;
		 m_pPF->m_Intrados.m_RearPoint.y = m_UndoPic[m_StackPos].IntRearPt.y;

		for (int i=0; i<=m_UndoPic[m_StackPos].m_iExt; i++){
			m_pPF->m_Extrados.m_ctrlPoint[i].x = m_UndoPic[m_StackPos].xExt[i];
			m_pPF->m_Extrados.m_ctrlPoint[i].y = m_UndoPic[m_StackPos].yExt[i];
		}
		for (i=0; i<=m_UndoPic[m_StackPos].m_iInt; i++){
			m_pPF->m_Intrados.m_ctrlPoint[i].x = m_UndoPic[m_StackPos].xInt[i];
			m_pPF->m_Intrados.m_ctrlPoint[i].y = m_UndoPic[m_StackPos].yInt[i];
		}
		gap =   m_pPF->m_Extrados.m_ctrlPoint[m_pPF->m_Extrados.m_iPoints].y
			  - m_pPF->m_Intrados.m_ctrlPoint[m_pPF->m_Intrados.m_iPoints].y;

		m_pPF->Update(true);
		m_pPF->Update(false);
	}
//	m_pACtrl->SetGap(gap);
	UpdateView();
}


void CAFoil::OnUndo() 
{
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
		CToolBarCtrl *pTB = &(m_pAFoilBar->GetToolBarCtrl());
		if(m_StackPos==0){
			pTB->EnableButton(IDT_UNDO, false);
		}
		pTB->EnableButton(IDT_REDO, true);
	}
	else {
		m_StackPos = 0;
	}
}

void CAFoil::OnRedo() 
{
	if(m_StackPos<m_StackSize-1) {
		m_StackPos++;
		SetPicture();
		CToolBarCtrl *pTB = &(m_pAFoilBar->GetToolBarCtrl());
		pTB->EnableButton(IDT_UNDO, true);
		if(m_StackPos==m_StackSize-1) {
			pTB->EnableButton(IDT_REDO, false);
		}
	}
}


void CAFoil::OnGrid() 
{
	CAGridDlg  dlg(this);
	dlg.m_bXGrid = m_bXGrid;	
	dlg.m_bYGrid = m_bYGrid;
	dlg.m_XColor = m_XGridColor;
	dlg.m_YColor = m_YGridColor;
	dlg.m_XStyle = m_XGridStyle;
	dlg.m_YStyle = m_YGridStyle;
	dlg.m_XWidth = m_XGridWidth;
	dlg.m_YWidth = m_YGridWidth;
	dlg.m_XUnit  = m_XGridUnit;
	dlg.m_YUnit  = m_YGridUnit;

	dlg.m_bXMinGrid = m_bXMinGrid;
	dlg.m_XMinColor = m_XMinColor;
	dlg.m_XMinStyle = m_XMinStyle;
	dlg.m_XMinWidth = m_XMinWidth;
	dlg.m_XMinorUnit = m_XMinorUnit;
	dlg.m_bYMinGrid = m_bYMinGrid;
	dlg.m_YMinColor = m_YMinColor;
	dlg.m_YMinStyle = m_YMinStyle;
	dlg.m_YMinWidth = m_YMinWidth;
	dlg.m_YMinorUnit = m_YMinorUnit;

	dlg.m_bNeutral  = m_bNeutralLine;
	dlg.m_NeutralStyle = m_NeutralStyle;
	dlg.m_NeutralWidth = m_NeutralWidth;
	dlg.m_NeutralColor = m_NeutralColor;

	dlg.m_bScale       = m_bScale;


	if(dlg.DoModal() == IDOK){
		m_bXGrid = dlg.m_bXGrid;	
		m_bYGrid = dlg.m_bYGrid;
		m_XGridColor = dlg.m_XColor;
		m_YGridColor = dlg.m_YColor;
		m_XGridStyle = dlg.m_XStyle;
		m_YGridStyle = dlg.m_YStyle;
		m_XGridWidth = dlg.m_XWidth;
		m_YGridWidth = dlg.m_YWidth;
		m_XGridUnit  = dlg.m_XUnit;
		m_YGridUnit  = dlg.m_YUnit;

		m_bXMinGrid   = dlg.m_bXMinGrid;
		m_XMinColor   = dlg.m_XMinColor;
		m_XMinStyle   = dlg.m_XMinStyle;
		m_XMinWidth   = dlg.m_XMinWidth;
		m_XMinorUnit  = dlg.m_XMinorUnit;
		m_bYMinGrid   = dlg.m_bYMinGrid;
		m_YMinColor   = dlg.m_YMinColor;
		m_YMinStyle   = dlg.m_YMinStyle;
		m_YMinWidth   = dlg.m_YMinWidth;
		m_YMinorUnit  = dlg.m_YMinorUnit;

		m_bNeutralLine = dlg.m_bNeutral;
		m_NeutralStyle = dlg.m_NeutralStyle;
		m_NeutralWidth = dlg.m_NeutralWidth;
		m_NeutralColor = dlg.m_NeutralColor;

		m_bScale       = dlg.m_bScale;


	}
	UpdateView();
}



void CAFoil::ReleaseZoom()
{
	m_bZoomPlus = false;
	m_ZoomRect.SetRectEmpty();
	CToolBarCtrl *pTB = &(m_pAFoilBar->GetToolBarCtrl());
	pTB->PressButton(IDT_ZOOMIN, false);
	SetCursor(m_hcCross);
}


void CAFoil::OnDuplicate() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CFoil *pCurFoil = m_pACtrl->GetFoil();
	if(!pCurFoil) return;
	CFoil *pNewFoil = new CFoil();
	pNewFoil->CopyFoil(pCurFoil);
	pNewFoil->InitFoil();

	if(pFrame->SetModFoil(pNewFoil)){
		m_pACtrl->FillFoilList();
		m_pACtrl->SelectFoil(pNewFoil);
	}
	else {
		pNewFoil = NULL;
		m_pACtrl->FillFoilList();
		m_pACtrl->SelectFoil();
	}		
}

void CAFoil::OnExportFoil() 
{
	CFoil *pCurFoil = m_pACtrl->GetFoil();
	if(!pCurFoil) return;

	CString FileName, DestFileName, OutString;
	CStdioFile DestFile;
	CFileException fe;

	FileName = pCurFoil->m_FoilName;
	FileName.Replace("/", " ");
	CFileDialog XFileDlg(false, "dat", FileName, OFN_OVERWRITEPROMPT,
		_T("Labeled Format (.dat)|*.dat|VisuAero Format (.dat)|*.dat|"));
	if(IDOK==XFileDlg.DoModal()) {
		DestFileName = XFileDlg.GetFileName();
		int FilterIndex = XFileDlg.m_ofn.nFilterIndex;
		CFoil *pFoil = GetFoil(pCurFoil->m_FoilName);
		if(pFoil) {
			if(FilterIndex==1)	
				pFoil->ExportFoil(DestFileName);
			else {//writing ViSuAero Format, special for... me
				BOOL bOpen2 = DestFile.Open(DestFileName, CFile::modeCreate | CFile::modeWrite, &fe);

				OutString  = '"' ;
				OutString += pCurFoil->m_FoilName;
				OutString += '"';
				OutString += "\n";
				DestFile.WriteString(OutString);

				OutString.Format("%.2f, %.2f, %.2f, %.2f, 0.0, 0.0\n",
								 pCurFoil->m_fThickness*100.0,
								 pCurFoil->m_fXThickness*100.0,
								 pCurFoil->m_fCamber*100.0,
								 pCurFoil->m_fXCamber*100.0);

				DestFile.WriteString(OutString);

				for (int i=0; i<pCurFoil->n; i++){
					OutString.Format("%8.4f, %8.4f\n", pCurFoil->x[i]*100.0, pCurFoil->y[i]*100.0); 
					DestFile.WriteString(OutString);
				}

				
				DestFile.Close();
			}
		}
	}
}

void CAFoil::OnRenameFoil() 
{
	if(!m_pACtrl->m_pRefFoil) return;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
/*	if(pFrame->SetModFoil(m_pACtrl->m_pRefFoil)){
		m_pACtrl->m_pRefFoil = NULL;
		m_pACtrl->FillFoilList();
		m_pACtrl->SetFoil(m_pACtrl->m_pRefFoil);
		UpdateView();
	}*/
	if(pFrame->RenameFoil(m_pACtrl->m_pRefFoil)){
		m_pACtrl->FillFoilList();
		m_pACtrl->SelectFoil();
		UpdateView();
	}
}

void CAFoil::OnSFSave() 
{
	if(m_bSF) SFSave();
	else PFSave();
}

void CAFoil::PFSave()
{
	CStdioFile XFile;
	CFileException fe;
	CString SFLName;
	
	bool bNotFound= true;
	bool bWrite = false;
	// deselect points so as not to interfere with other mouse commands
	m_pPF->m_Intrados.m_iSelect = -10;
	m_pPF->m_Extrados.m_iSelect = -10;

	while(bNotFound){// until we find a file to export to
		CFileDialog XFileDlg(false, "cfl", NULL, NULL,
			_T("Curve file (.cfl)|*.cfl|"));
		if(IDOK==XFileDlg.DoModal()){
			SFLName = XFileDlg.GetFileName();

			CFileFind FileFinder; 
			if(!FileFinder.FindFile(SFLName)) {// file does not exist : OK to create and export
				bWrite = true;
				bNotFound = false;
			}
			else{
				if(IDYES==AfxMessageBox("Overwrite existing file ?", MB_YESNO)){
					bWrite = true;// file exists, but OK to overwrite and export
					bNotFound = false;
				}
				else bWrite = false;// file exists, but user does not wan't to overwrite
			}
		}
		else{//user changed his/her mind
			bWrite = false; //do not write...
			bNotFound = false; // ...and exit
		}
	}
	if(bWrite){// we're set to save spline control points
		BOOL bOpen = XFile.Open(SFLName, CFile::modeCreate | CFile::modeWrite, &fe);
		if (bOpen){
			m_pPF->SaveFile(&XFile);
			XFile.Close();
		}
	}
}

void CAFoil::SFSave()
{
	CStdioFile XFile;
	CFileException fe;
	CString SFLName;
	
	bool bNotFound= true;
	bool bWrite = false;
	// deselect points so as not to interfere with other mouse commands
	m_pSF->m_Intrados.m_iSelect = -10;
	m_pSF->m_Extrados.m_iSelect = -10;

	while(bNotFound){// until we find a file to export to
		CFileDialog XFileDlg(false, "sfl", NULL, NULL,
			_T("Spline file (.sfl)|*.sfl|"));
		if(IDOK==XFileDlg.DoModal()){
			SFLName = XFileDlg.GetFileName();

			CFileFind FileFinder; 
			if(!FileFinder.FindFile(SFLName)) {// file does not exist : OK to create and export
				bWrite = true;
				bNotFound = false;
			}
			else{
				if(IDYES==AfxMessageBox("Overwrite existing file ?", MB_YESNO)){
					bWrite = true;// file exists, but OK to overwrite and export
					bNotFound = false;
				}
				else bWrite = false;// file exists, but user does not wan't to overwrite
			}
		}
		else{//user changed his/her mind
			bWrite = false; //do not write...
			bNotFound = false; // ...and exit
		}
	}
	if(bWrite){// we're set to save spline control points
		BOOL bOpen = XFile.Open(SFLName, CFile::modeCreate | CFile::modeWrite, &fe);
		if (bOpen){		
			m_pSF->SaveFile(&XFile);
			XFile.Close();
		}
	}
}


void CAFoil::DrawScale(CDC *pDC, CRect* pCltRect, double scale, CPoint ptOffset, bool bIsPrinting)
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


	double xo = 0.0;
	double xmin = 0.0;
	double xmax = 1.0;
//	double ymin = -0.2;
//	double ymax =  0.2;
	double XGridUnit = 0.1;
	double XHalfGridUnit = 0.05;
	double XMinGridUnit = 0.01;

	double xt  = xo-int((xo-xmin)*1.0001/XGridUnit)*XGridUnit;//one tick at the origin
	double xht = xo-int((xo-xmin)*1.0001/XHalfGridUnit)*XHalfGridUnit;//one tick at the origin
	double xmt = xo-int((xo-xmin)*1.0001/XMinGridUnit)*XMinGridUnit;//one tick at the origin

	pDC->MoveTo(int(xt*scalex) + ptOffset.x, offy);
	pDC->LineTo(int(xmax*scalex) + ptOffset.x, offy);

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
//	LinePen.DeleteObject();
}

void CAFoil::DrawXGrid(CDC *pDC, double scalex, double scaley, CPoint Offset, CRect dRect, bool bIsPrinting)
{
	if (bIsPrinting){
		scaley=-scaley;
	}

	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	COLORREF color= m_XGridColor;
	int style = m_XGridStyle;
	int width = m_XGridWidth;

	GetBWColor(color, style, width);

	if(m_XGridWidth<=1) width = m_XGridWidth;

	lb.lbColor = color;

	CPen GridPen;
	GridPen.CreatePen(PS_GEOMETRIC | style, GetPenWidth(width,bIsPrinting), &lb);
	CPen *pOldPen = pDC->SelectObject(&GridPen);

	double xo = 0.0;
	double xmin = 0.0;
	double xmax = 1.0;
	double ymin = -0.2f;
	double ymax =  0.2f;

	int YMin = max(int(ymin*scaley)+ Offset.y, dRect.top);
	int YMax = min(int(ymax*scaley)+ Offset.y, dRect.bottom);

	double xt = xo-int((xo-xmin)*1.0001/m_XGridUnit)*m_XGridUnit;//one tick at the origin

	while(xt<=xmax*1.001){
		//Draw  grid
		pDC->MoveTo(int(xt*scalex) + Offset.x, YMin);
		pDC->LineTo(int(xt*scalex) + Offset.x, YMax);
		xt += m_XGridUnit ;
	}

	pDC->SelectObject(pOldPen);
//	GridPen.DeleteObject();
}




void CAFoil::DrawYGrid(CDC *pDC, double scalex, double scaley, CPoint Offset, CRect dRect, bool bIsPrinting)
{
	if (bIsPrinting){
		scaley=-scaley;
	}
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	COLORREF color= m_YGridColor;
	int style = m_YGridStyle;
	int width = m_YGridWidth;

	GetBWColor(color, style, width);

	if(m_YGridWidth<=1) width = m_YGridWidth;

	lb.lbColor = color;

	CPen GridPen;
	GridPen.CreatePen(PS_GEOMETRIC | style, GetPenWidth(width,bIsPrinting), &lb);
	CPen *pOldPen = pDC->SelectObject(&GridPen);

	double yo = 0.0;
	double ymin = -0.2f;
	double ymax = 0.2f;
	double xmin = 0.0;
	double xmax = 1.0;

	int XMin = max(int(xmin*scalex)+ Offset.x, dRect.left);
	int XMax = min(int(xmax*scalex)+ Offset.x, dRect.right);

	double yt = yo-int((yo-ymin)*1.0001/m_YGridUnit)*m_YGridUnit;//one tick at the origin

	while(yt<=ymax*1.0001){

		pDC->MoveTo(XMin, (int)(yt*scaley) + Offset.y);
		pDC->LineTo(XMax, (int)(yt*scaley) + Offset.y);
		
		yt += m_YGridUnit ;
	}

	pDC->SelectObject(pOldPen);
//	GridPen.DeleteObject();
}


void CAFoil::DrawXMinGrid(CDC *pDC, double scalex, double scaley,
						  CPoint Offset, CRect dRect, bool bIsPrinting)
{
	if (bIsPrinting){
		scaley=-scaley;
	}

	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	COLORREF color= m_XMinColor;
	int style = m_XMinStyle;
	int width = m_XMinWidth;

	GetBWColor(color, style, width);

	if(m_XMinWidth<=1) width = m_XMinWidth;

	lb.lbColor = color;

	CPen GridPen;
	GridPen.CreatePen(PS_GEOMETRIC | style, GetPenWidth(width,bIsPrinting), &lb);
	CPen *pOldPen = pDC->SelectObject(&GridPen);


	double xo = 0.0;
	double xmin = 0.0;
	double xmax = 1.0;
	double ymin = -0.2;
	double ymax =  0.2;

	int YMin = max(int(ymin*scaley)+ Offset.y, dRect.top);
	int YMax = min(int(ymax*scaley)+ Offset.y, dRect.bottom);

	double xDelta = m_XMinorUnit;
	int MinFreq = (int)(m_XGridUnit/m_XMinorUnit);
	int k=0;
	double xt = xo-int((xo-xmin)*1.0001/m_XGridUnit)*m_XGridUnit;//one tick at the origin

	while(xt<=xmax*1.001){
		if(k%(MinFreq)!=0){// do not overwrite main grid
			if (xt>=xmin){
				pDC->MoveTo(int(xt*scalex) + Offset.x, YMin);
				pDC->LineTo(int(xt*scalex) + Offset.x, YMax);
			}
		}
		xt += xDelta;
		k++;
	}

	pDC->SelectObject(pOldPen);
//	GridPen.DeleteObject();
}


void CAFoil::DrawYMinGrid(CDC *pDC, double scalex, double scaley,
						  CPoint Offset, CRect dRect, bool bIsPrinting)
{
	if (bIsPrinting){
		scaley=-scaley;
	}

	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	COLORREF color= m_YMinColor;
	int style = m_YMinStyle;
	int width = m_YMinWidth;

	GetBWColor(color, style, width);

	if(m_YMinWidth<=1) width = m_YMinWidth;

	lb.lbColor = color;

	CPen GridPen;
	GridPen.CreatePen(PS_GEOMETRIC | style, GetPenWidth(width,bIsPrinting), &lb);
	CPen *pOldPen = pDC->SelectObject(&GridPen);

	double yo = 0.0;
	double xmin = 0.0;
	double xmax = 1.0;
	double ymin = -0.2;
	double ymax =  0.2;

	int XMin = max(int(xmin*scalex)+ Offset.x, dRect.left);
	int XMax = min(int(xmax*scalex)+ Offset.x, dRect.right);


//	double yDelta = m_YGridUnit/(m_YMinFreq+1);
	double yDelta = m_YMinorUnit;
	int MinFreq = (int)(m_YGridUnit/m_YMinorUnit);
	int k=0;
	double yt = yo-int((yo-ymin)*1.0001/m_YGridUnit)*m_YGridUnit;//one tick at the origin

	while(yt<=ymax*1.001){
		if(k%(MinFreq)!=0){// do not overwrite main grid
			if (yt>=ymin){
				pDC->MoveTo(XMin, (int)(yt*scaley) + Offset.y);
				pDC->LineTo(XMax, (int)(yt*scaley) + Offset.y);
			}
		}
		yt += yDelta;
		k++;
	}

	pDC->SelectObject(pOldPen);
//	GridPen.DeleteObject();
}


bool CAFoil::LoadSettings(CArchive& ar) 
{
	try{
		int k, cr;

		ar >> m_pSF->m_FoilColor >> m_pSF->m_FoilStyle >> m_pSF->m_FoilWidth;
		ar >> m_pPF->m_FoilColor >> m_pPF->m_FoilStyle >> m_pPF->m_FoilWidth;

		ar >> k;
		if(k !=0 && k !=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_pSF->m_bVisible = true; else m_pSF->m_bVisible = false;

		ar >> k;
		if(k !=0 && k !=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_pSF->m_bOutPoints = true; else m_pSF->m_bOutPoints = false;

		ar >> k;
		if(k !=0 && k !=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_pSF->m_bCenterLine = true; else m_pSF->m_bCenterLine = false;

		ar >> k;
		if(k !=0 && k !=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_pPF->m_bVisible = true; else m_pPF->m_bVisible = false;

		ar >> k;
		if(k !=0 && k !=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_pPF->m_bOutPoints = true; else m_pPF->m_bOutPoints = false;

		ar >> k;
		if(k !=0 && k !=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_pPF->m_bCenterLine = true; else m_pPF->m_bCenterLine = false;

		ar >> k;
		if(k !=0 && k !=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_bCircle = true; else m_bCircle = false;


		ar >> k;
		if(k !=0 && k !=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_bNeutralLine = true; else m_bNeutralLine = false;



		ar >> k;
		if(k !=0 && k !=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_bXGrid = true; else m_bXGrid = false;


		ar >> k;
		if(k !=0 && k !=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_bYGrid = true; else m_bYGrid = false;
		
		ar >> k;
		if(k !=0 && k !=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_bXMinGrid = true; else m_bXMinGrid = false;


		ar >> k;
		if(k !=0 && k !=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_bYMinGrid = true; else m_bYMinGrid = false;
		
		ar >> k;
		if(k !=0 && k !=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_bScale = true; else m_bScale = false;

		ar >> k;
		if(k !=0 && k !=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_bShowLegend = true; else m_bShowLegend = false;

		ar >> cr;
		if(cr <0 || cr> RGB(255,255,255)){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_XGridColor = cr;

		ar >> cr;
		if(cr <0 || cr> RGB(255,255,255)){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_YGridColor = cr;

		ar >> k;
		if(k<0 || k>5){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_XGridStyle = k;

		ar >> k;
		if(k<0 || k>5){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_YGridStyle = k;

		ar >> k;
		if(k<0 || k>5){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_XGridWidth = k;

		ar >> k;
		if(k<0 || k>5){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_YGridWidth = k;

		ar >> m_XGridUnit >> m_YGridUnit;


		ar >> cr;
		if(cr <0 || cr> RGB(255,255,255)){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_XMinColor = cr;

		ar >> cr;
		if(cr <0 || cr> RGB(255,255,255)){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_YMinColor = cr;

		ar >> k;
		if(k<0 || k>5){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_XMinStyle = k;

		ar >> k;
		if(k<0 || k>5){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_YMinStyle = k;

		ar >> k;
		if(k<0 || k>5){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_XMinWidth = k;

		ar >> k;
		if(k<0 || k>5){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_YMinWidth = k;

		ar >> m_XMinorUnit >> m_YMinorUnit;

		ar >> k;
		if(k<0 || k>5){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_NeutralStyle = k;
		ar >> k;
		if(k<0 || k>5){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_NeutralWidth = k;
		ar >> cr;
		if(cr <0 || cr> RGB(255,255,255)){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_NeutralColor = cr;


	}
	catch (CArchiveException *ex){
		TCHAR   szCause[255];
		CString str;
		ex->GetErrorMessage(szCause, 255);
		ex->Delete();
		return false;
	}
	return true;
}

void CAFoil::SaveSettings(CArchive& ar) 
{
	ar << m_pSF->m_FoilColor << m_pSF->m_FoilStyle << m_pSF->m_FoilWidth;
	ar << m_pPF->m_FoilColor << m_pPF->m_FoilStyle << m_pPF->m_FoilWidth;
	if(m_pSF->m_bVisible)      ar << 1; else ar << 0;
	if(m_pSF->m_bOutPoints)    ar << 1; else ar << 0;
	if(m_pSF->m_bCenterLine)   ar << 1; else ar << 0;
	if(m_pPF->m_bVisible)      ar << 1; else ar << 0;
	if(m_pPF->m_bOutPoints)    ar << 1; else ar << 0;
	if(m_pPF->m_bCenterLine)   ar << 1; else ar << 0;
	if(m_bCircle)      ar << 1; else ar << 0;
	if(m_bNeutralLine) ar << 1; else ar << 0;
	if(m_bXGrid)       ar << 1; else ar << 0;
	if(m_bYGrid)       ar << 1; else ar << 0;
	if(m_bXMinGrid)    ar << 1; else ar << 0;
	if(m_bYMinGrid)    ar << 1; else ar << 0;
	if(m_bScale)       ar << 1; else ar << 0;
	if(m_bShowLegend)  ar << 1; else ar << 0;

	ar << m_XGridColor << m_YGridColor;
	ar << m_XGridStyle << m_YGridStyle;
	ar << m_XGridWidth << m_YGridWidth;
	ar << m_XGridUnit  << m_YGridUnit;

	ar << m_XMinColor << m_YMinColor;
	ar << m_XMinStyle << m_YMinStyle;
	ar << m_XMinWidth << m_YMinWidth;
	ar << m_XMinorUnit  << m_YMinorUnit;

	ar << m_NeutralStyle << m_NeutralWidth << m_NeutralColor;
}

void CAFoil::OnStoreFoil() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(m_bSF){
		if(m_pSF->m_Extrados.m_iRes>IQX2) {
			CString strong;
			strong.Format("Too many output points on upper surface\n Max =%d", IQX2);
			AfxMessageBox( strong, MB_OK);
			return;
		}
		if(m_pSF->m_Intrados.m_iRes>IQX2) {
			CString strong;
			strong.Format("Too many output points on lower surface\n Max =%d", IQX2);
			AfxMessageBox( strong, MB_OK);
			return;
		}
		Trace("m_iRes_Ext=",m_pSF->m_Extrados.m_iRes);
		Trace("m_iRes_Int=",m_pSF->m_Intrados.m_iRes);
		CFoil *pNewFoil = new CFoil();
		m_pSF->ExportToBuffer(pNewFoil);

		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilName = "";
		if(pFrame->SetModFoil(pNewFoil)){
			m_pACtrl->m_pRefFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil(pNewFoil);
		}
		else {
			pNewFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil();
		}
	}
	else{
		int size = m_pPF->m_Extrados.m_iPoints * (m_pPF->m_Extrados.m_Freq-1) ;//+ 1;
		if(size>IQX2) {
			CString strong;
			strong.Format("Too many output points on upper surface\n Max =%d", IQX2);
			AfxMessageBox( strong, MB_OK);
			return;
		}
//		Trace("m_iPts_Ext=",size);
		size = m_pPF->m_Intrados.m_iPoints * (m_pPF->m_Intrados.m_Freq-1) ;//+ 1;
		if(size>IQX2) {
			CString strong;
			strong.Format("Too many output points on lower surface\n Max =%d", IQX2);
			AfxMessageBox( strong, MB_OK);
			return;
		}
//		Trace("m_iPts_Int=",size);

		CFoil *pNewFoil = new CFoil();
		m_pPF->ExportToBuffer(pNewFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilName = "";
		if(pFrame->SetModFoil(pNewFoil)){
			m_pACtrl->m_pRefFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil(pNewFoil);
		}
		else {
			pNewFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil();
		}
	}	
	UpdateView();
}

void CAFoil::OnResetView() 
{
	SetScale();
	UpdateView();
}

bool CAFoil::SetAFoilCursor(CWnd* pWnd, CPoint ptMouse, UINT message) 
{
	if (pWnd == m_pChildWnd){
		if(m_bTrans){
			SetCursor(m_hcMove);
			return TRUE;
		}
		else {
			SetCursor(m_hcCross);
			return TRUE;
		}
	}
	return true;
}
void CAFoil::OnAbout() 
{
	((CXFLR5App*)AfxGetApp())->OnAppAbout();
}


void CAFoil::SaveProject() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->OnSaveProject();
}
void CAFoil::SetSaveState(bool bState) 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(bState);
}

void CAFoil::OnHideAllFoils() 
{
	SetSaveState(false);
	CFoil*pFoil;
	for (int k=0; k<m_poaFoil->GetSize(); k++){
		pFoil = (CFoil*)m_poaFoil->GetAt(k);
		pFoil->m_bVisible = false;
	}
	m_pACtrl->m_ctrlVisible.SetCheck(false);
	UpdateView();
}

void CAFoil::OnShowAllFoils() 
{
	SetSaveState(false);
	CFoil*pFoil;
	for (int k=0; k<m_poaFoil->GetSize(); k++){
		pFoil = (CFoil*)m_poaFoil->GetAt(k);
		pFoil->m_bVisible = true;
	}
	m_pACtrl->m_ctrlVisible.SetCheck(true);	
	UpdateView();
}

void CAFoil::SetFoils(CFoil *pCurFoil)
{
// import all the buffer foils
	CFoil *pFoil;
	m_pACtrl->FillFoilList();

	if(pCurFoil){
		m_pACtrl->SelectFoil(pCurFoil);
	}
	else if(m_poaFoil->GetSize()) {
		pFoil = (CFoil*)m_poaFoil->GetAt(0);
		m_pACtrl->SelectFoil(pFoil);
	}
}




void CAFoil::OnHideFoil() 
{
	CFoil *pRefFoil = m_pACtrl->GetFoil();
	if(!pRefFoil) return;
	ShowFoil(pRefFoil, false);
	UpdateView();
}


void CAFoil::OnDeleteFoil() 
{	
	CFoil *pRefFoil = m_pACtrl->GetFoil();
	if(!pRefFoil) return;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	if(pFrame->DeleteFoil(pRefFoil)){
		m_pACtrl->m_pRefFoil = NULL;
		m_pACtrl->FillFoilList();
		m_pACtrl->SelectFoil();
	}
	UpdateView();
}


void CAFoil::OnShowFoil() 
{
	CFoil *pRefFoil = m_pACtrl->GetFoil();
	if(!pRefFoil) return;
	ShowFoil(pRefFoil, true);
	UpdateView();

}


void CAFoil::ShowFoil(CFoil* pFoil, bool bShow) 
{
	if(!pFoil) return;
	m_pACtrl->m_pRefFoil->m_bVisible = bShow;
	m_pACtrl->SetFoil(pFoil);
	SetSaveState(false);
}



void CAFoil::SetCurFoil(CFoil *pFoil)
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->m_pCurFoil = pFoil;
	m_pACtrl->SetFoil(pFoil);
}



void CAFoil::OnZoomIn() 
{
	// can't do two things at the same time can we ?
	if(!m_bZoomPlus){
		if(m_fScale/m_fRefScale <32.f){
			m_bZoomPlus = true;
			CToolBarCtrl *pTB = &(m_pAFoilBar->GetToolBarCtrl());
			pTB->PressButton(IDT_ZOOMIN, true);
		}
		else {
			ReleaseZoom();
		}
	}
	else {
		ReleaseZoom();
	}
}

void CAFoil::OnZoomYOnly() 
{
	CToolBarCtrl *pTB = &(m_pAFoilBar->GetToolBarCtrl());
	m_bZoomYOnly = !m_bZoomYOnly;
	if(m_bZoomYOnly)	pTB->PressButton(IDT_ZOOMYONLY, true);
	else				pTB->PressButton(IDT_ZOOMYONLY, false);
	m_pChildWnd->SetFocus();
	
}


void CAFoil::OnResetYScale() 
{
	m_fScaleY = 1.0;
	UpdateView();
}


void CAFoil::OnNormalize() 
{
	CFoil *pCurFoil = m_pACtrl->GetFoil();
	if(!pCurFoil) return;
	double length = pCurFoil->NormalizeGeometry();

	CString str;
	str.Format("Foil has been normalized from %7.3f  to 1.000", length);
	
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->m_wndStatusBar.SetWindowText(str);

	UpdateView();
	
}


void CAFoil::OnDeRotate() 
{
	CFoil *pCurFoil = m_pACtrl->GetFoil();
	if(!pCurFoil) return;

	double angle = pCurFoil->DeRotate();

	CString str;
	str.Format("Foil has been de-rotated by %6.3f degrees", angle);

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->m_wndStatusBar.SetWindowText(str);
	UpdateView();
}


void CAFoil::OnLERad() 
{
	CFoil *pCurFoil = m_pACtrl->GetFoil();
	if(!pCurFoil) return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	m_pBufferFoil->CopyFoil(pCurFoil);
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_bPoints  = false;
	m_pBufferFoil->m_FoilName = pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = RGB(160,160,160);
	m_pBufferFoil->m_nFoilStyle = PS_DASH;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();	

	CLEDlg Ldlg(this);
	Ldlg.m_pBufferFoil = m_pBufferFoil;
	Ldlg.m_pMemFoil    = pCurFoil;
	Ldlg.m_pXFoil      = m_pXFoil;
	Ldlg.m_pChildView  = m_pChildWnd;

	if(IDOK == Ldlg.DoModal()){
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		if(pFrame->SetModFoil(pNewFoil)){
			m_pACtrl->m_pRefFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil(pNewFoil);
		}
		else {
			pNewFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil(pNewFoil);
		}
	}

	else{
		m_pACtrl->FillFoilList();
		m_pACtrl->SelectFoil(pCurFoil);
		m_pXFoil->m_FoilName ="";

	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();		
}

void CAFoil::OnTEGap() 
{
	CFoil *pCurFoil = m_pACtrl->GetFoil();
	if(!pCurFoil) return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	m_pBufferFoil->CopyFoil(pCurFoil);
	m_pBufferFoil->m_bPoints  = false;
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = RGB(160,160,160);
	m_pBufferFoil->m_nFoilStyle = PS_DASH;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();	

	CTEGapDlg Gdlg(this);
	Gdlg.m_pBufferFoil = m_pBufferFoil;
	Gdlg.m_pMemFoil    = pCurFoil;
	Gdlg.m_pXFoil      = m_pXFoil;
	Gdlg.m_pChildView  = m_pChildWnd;
	
	if(IDOK == Gdlg.DoModal()){
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		if(pFrame->SetModFoil(pNewFoil)){
			m_pACtrl->m_pRefFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil(pNewFoil);
		}
		else {
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil();
		}
	}

	else{
		m_pACtrl->FillFoilList();
		m_pACtrl->SelectFoil(pCurFoil);
		m_pXFoil->m_FoilName ="";
		//to un-initialize XFoil in case user switches to XInverse
		//Thanks Jean-Marc !
	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();	

}



void CAFoil::OnPanel() 
{
	CFoil *pCurFoil = m_pACtrl->GetFoil();
	if(!pCurFoil) return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	m_pBufferFoil->CopyFoil(pCurFoil);
	m_pBufferFoil->m_FoilName  = pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = RGB(160,160,160);
	m_pBufferFoil->m_nFoilStyle = PS_DASH;
	m_pBufferFoil->m_nFoilWidth = 1;
	m_pBufferFoil->m_bPoints   = true;
	m_pBufferFoil->m_bVisible  = true;

	UpdateView();	

	C2DPanelDlg Pdlg;
	Pdlg.m_pBufferFoil = m_pBufferFoil;
	Pdlg.m_pMemFoil    = pCurFoil;
	Pdlg.m_pXFoil      = m_pXFoil;
	Pdlg.m_pChildView  = m_pChildWnd;
	
	if(IDOK == Pdlg.DoModal()){
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		if(pFrame->SetModFoil(pNewFoil)){
			m_pACtrl->m_pRefFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil(pNewFoil);
		}
		else {
			pNewFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil();
		}
	}

	else{
		m_pACtrl->FillFoilList();
		m_pACtrl->SelectFoil(pCurFoil);
		m_pXFoil->m_FoilName ="";

	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}

void CAFoil::OnCadd() 
{
	CFoil *pCurFoil = m_pACtrl->GetFoil();
	if(!pCurFoil) return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	m_pBufferFoil->CopyFoil(pCurFoil);
	m_pBufferFoil->m_FoilName  = pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = RGB(160,160,160);
	m_pBufferFoil->m_nFoilStyle = PS_DASH;
	m_pBufferFoil->m_nFoilWidth = 1;
	m_pBufferFoil->m_bPoints   = true;
	m_pBufferFoil->m_bVisible  = true;

	UpdateView();	


	CCAddDlg Adlg;
	Adlg.m_pBufferFoil = m_pBufferFoil;
	Adlg.m_pMemFoil    = pCurFoil;
	Adlg.m_pXFoil      = m_pXFoil;
	Adlg.m_pChildView  = m_pChildWnd;

	if(IDOK == Adlg.DoModal()){
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		if(pFrame->SetModFoil(pNewFoil)){
			m_pACtrl->m_pRefFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil(pNewFoil);
		}
		else {
			pNewFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil();
		}
	}

	else{
		m_pACtrl->FillFoilList();
		m_pACtrl->SelectFoil(pCurFoil);
		m_pXFoil->m_FoilName ="";

	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
	
}

void CAFoil::OnInterpolate() 
{
	if(m_poaFoil->GetSize()<2){
		AfxMessageBox("At least two foils are required", MB_OK);
		return;
	}
	CFoil *pCurFoil = m_pACtrl->GetFoil();
	if(!pCurFoil) return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	m_pBufferFoil->CopyFoil(pCurFoil);
	m_pBufferFoil->m_FoilName  = pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = RGB(160,160,160);
	m_pBufferFoil->m_nFoilStyle = PS_DASH;
	m_pBufferFoil->m_nFoilWidth = 1;
	m_pBufferFoil->m_bPoints   = false;
	m_pBufferFoil->m_bVisible  = true;

	UpdateView();	

	CInterpolateDlg dlg(this);
	dlg.m_pChildView = m_pChildWnd;
	dlg.m_poaFoil = m_poaFoil;
	dlg.m_pXFoil = m_pXFoil;
	dlg.m_pBufferFoil = m_pBufferFoil;


	if(IDOK == dlg.DoModal()){
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilName = dlg.m_FoilName;
		if(pFrame->SetModFoil(pNewFoil)){
			m_pACtrl->m_pRefFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil(pNewFoil);
		}
		else {
			pNewFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil();
		}
	}

	else{
		m_pACtrl->FillFoilList();
		m_pACtrl->SelectFoil(pCurFoil);
		m_pXFoil->m_FoilName ="";

	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
	
}

void CAFoil::OnEditCtrlPoints()
{
	CEditFoilDlg dlg(this);
	dlg.m_pChildView  = m_pChildWnd;
	if(m_bSF){
		CSF memSF;
		memSF.Copy(m_pSF);
		dlg.m_pSF = m_pSF;
		dlg.m_pmemSF = &memSF;
		TakePicture();
		if(IDOK == dlg.DoModal()){
			StorePicture();
		}
		else m_pSF->Copy(&memSF);
	}
	else{
		CPF memPF;
		memPF.Copy(m_pPF);
		dlg.m_pmemPF = &memPF;
		dlg.m_pPF = m_pPF;
		TakePicture();
		if(IDOK == dlg.DoModal())
		{
			StorePicture();
		}
		else m_pPF->Copy(&memPF);
	}
	UpdateView();
}

void CAFoil::OnEditCoord() 
{
	CFoil *pCurFoil = m_pACtrl->GetFoil();
	if(!pCurFoil) return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	m_pBufferFoil->CopyFoil(pCurFoil);
	m_pBufferFoil->m_bPoints  = true;
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = RGB(160,160,160);
	m_pBufferFoil->m_nFoilStyle = PS_DASH;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();	

	CEditFoilDlg dlg(this);
	dlg.m_pMemFoil    = pCurFoil;
	dlg.m_pBufferFoil = m_pBufferFoil;
	dlg.m_pChildView  = m_pChildWnd;
	
	if(IDOK == dlg.DoModal()){
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_iHighLight = -1;
		if(pFrame->SetModFoil(pNewFoil)){
			m_pACtrl->m_pRefFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil(pNewFoil);
		}
		else {
			pNewFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil();
		}
	}
	else{
		m_pACtrl->FillFoilList();
		m_pACtrl->SelectFoil(pCurFoil);
		m_pXFoil->m_FoilName ="";

	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}



void CAFoil::OnGeom() 
{
	CFoil *pCurFoil = m_pACtrl->GetFoil();
	if(!pCurFoil) return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	m_pBufferFoil->CopyFoil(pCurFoil);
	m_pBufferFoil->m_bPoints  = true;
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = RGB(160,160,160);
	m_pBufferFoil->m_nFoilStyle = PS_DASH;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();	

	CGeomDlg dlg(this);
	dlg.m_pXFoil      = m_pXFoil;
	dlg.m_pMemFoil    = pCurFoil;
	dlg.m_pBufferFoil = m_pBufferFoil;
	dlg.m_pChildView  = m_pChildWnd;
	
	if(IDOK == dlg.DoModal()){
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		if(pFrame->SetModFoil(pNewFoil)){
			m_pACtrl->m_pRefFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil(pNewFoil);
		}
		else {
			pNewFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil();
		}
	}

	else{
		m_pACtrl->FillFoilList();
		m_pACtrl->SelectFoil(pCurFoil);
		m_pXFoil->m_FoilName ="";

	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}

void CAFoil::OnNacaFoils() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	m_pBufferFoil->SetNaca009();
	m_pBufferFoil->m_bPoints  = true;
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = "Naca xxxx";
	m_pBufferFoil->m_FoilColor  = RGB(160,160,160);
	m_pBufferFoil->m_nFoilStyle = PS_DASH;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();	

	CNacaDlg dlg(this);
	dlg.m_pXFoil      = m_pXFoil;
	dlg.m_pBufferFoil = m_pBufferFoil;
	dlg.m_pChildView  = m_pChildWnd;
	
	if(IDOK == dlg.DoModal()){
		//then duplicate the buffer foil and add it
		CString str;

		if(dlg.m_Digits>0 && log10((double)dlg.m_Digits)<4)
			str.Format("%04d", dlg.m_Digits);
		else 
			str.Format("%d", dlg.m_Digits);
		str = "NACA "+ str;

		CFoil *pNewFoil    = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints    = false;
		pNewFoil->m_FoilName   = str;
		if(pFrame->SetModFoil(pNewFoil)){
			m_pACtrl->m_pRefFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil(pNewFoil);
		}
		else {
			pNewFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil();
		}
	}

	else{
		m_pACtrl->FillFoilList();
		CFoil *pCurFoil = m_pACtrl->GetFoil();
		if(pCurFoil) m_pACtrl->SelectFoil(pCurFoil);
		m_pXFoil->m_FoilName ="";

	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}

/*

void CAFoil::OnSetFlap() 
{
	CFoil *pCurFoil = m_pACtrl->GetFoil();
	if(!pCurFoil) return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	UpdateView();	

	CFoil *pNewFoil = new CFoil();
	pNewFoil->CopyFoil(pCurFoil);
	pNewFoil->SetTEFlap(true, 80.0, 50.0, 80.0);

	pNewFoil->m_FoilColor  = pFrame->GetColor(0);
	pNewFoil->m_nFoilStyle = PS_SOLID;
	pNewFoil->m_nFoilWidth = 1;
	if(pFrame->SetModFoil(pNewFoil)){
		m_pACtrl->m_pRefFoil = NULL;
		m_pACtrl->FillFoilList();
		m_pACtrl->SetFoil(pNewFoil);
	}
	else {
		pNewFoil = NULL;
		m_pACtrl->FillFoilList();
		m_pACtrl->SelectFoil();
	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
	
}*/
/*
void CAFoil::OnSetFlap() 
{
	CFoil *pCurFoil = m_pACtrl->GetFoil();
	if(!pCurFoil) return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	m_pBufferFoil->CopyFoil(pCurFoil);
	m_pBufferFoil->m_bPoints  = false;
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = RGB(160,160,160);
	m_pBufferFoil->m_nFoilStyle = PS_DASH;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();	

	CFlapDlg dlg(this);
	dlg.m_pXFoil      = m_pXFoil;
	dlg.m_pMemFoil    = pCurFoil;
	dlg.m_pBufferFoil = m_pBufferFoil;
	dlg.m_pChildView  = m_pChildWnd;
		
	if(IDOK == dlg.DoModal()){
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0)];
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		if(pFrame->SetModFoil(pNewFoil)){
			m_pACtrl->m_pRefFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SetFoil(pNewFoil);
		}
		else {
			pNewFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil();
		}
	}

	else{
		m_pACtrl->FillFoilList();
		m_pACtrl->SelectFoil(pCurFoil);
		m_pXFoil->m_FoilName ="";

	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
	
}*/


void CAFoil::DrawLegend(CDC *pDC, CPoint Place, bool bIsPrinting)
{
	CChildView * pChildView = (CChildView*)m_pChildWnd;

	int LegendSize, ypos;
	LOGFONT TempFont;
	int style, width;
	int x1;
	int n, k;
	COLORREF color;
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	memcpy(&TempFont, &pChildView->m_WndLogFont, sizeof(LOGFONT));

	if(bIsPrinting){
		TempFont.lfHeight = TempFont.lfHeight*30;
		LegendSize = 1500;
		ypos = -330;
	}
	else {
		LegendSize = 20;
		ypos = 15;
	}


	CFont ThisFont;
	ThisFont.CreateFontIndirect(&TempFont);
	CFont* pOldFont = pDC->SelectObject(&ThisFont);

	CString strLabel;
	pDC->SetTextAlign(TA_LEFT);
	pDC->SetBkMode(TRANSPARENT);
	if (IsBlackAndWhite()) pDC->SetTextColor(0);// black is black
	else pDC->SetTextColor(pChildView->m_WndTextColor);

	k=0;
	if(m_bSF){
		if(m_pSF->m_bVisible){
			color = m_pSF->m_FoilColor;
			style = m_pSF->m_FoilStyle;
			width = m_pSF->m_FoilWidth;
			if(IsBlackAndWhite()) GetBWColor(color, style, width);
			lb.lbColor = color;

			CPen LegendPen(PS_GEOMETRIC | style,  GetPenWidth(width, bIsPrinting), &lb);

			CPen *pOldPen = pDC->SelectObject(&LegendPen);

			pDC->MoveTo(Place.x, Place.y + ypos*k);
			pDC->LineTo(Place.x + (int)(LegendSize),
						Place.y + ypos*k);
			if(m_pSF->m_bOutPoints && !bIsPrinting){
				x1 = Place.x + (int)(0.5*LegendSize);
				pDC->Rectangle(x1-2, Place.y + ypos*k-2,
								x1+2, Place.y + ypos*k+2);
			}
			pDC->TextOut(Place.x + (int)(1.5*LegendSize),
							Place.y  + ypos*k-(int)(ypos/2),
							m_pSF->m_strFoilName);
			pDC->SelectObject(pOldPen);
		}
	}
	else{
		if(m_pPF->m_bVisible){
			color = m_pPF->m_FoilColor;
			style = m_pPF->m_FoilStyle;
			width = m_pPF->m_FoilWidth;
			if(IsBlackAndWhite()) GetBWColor(color, style, width);
			lb.lbColor = color;

			CPen LegendPen(PS_GEOMETRIC | style, GetPenWidth(width, bIsPrinting), &lb);

			CPen *pOldPen = pDC->SelectObject(&LegendPen);

			pDC->MoveTo(Place.x, Place.y + ypos*k);
			pDC->LineTo(Place.x + (int)(LegendSize),
						Place.y + ypos*k);
			if(m_pPF->m_bOutPoints && !bIsPrinting){
				x1 = Place.x + (int)(0.5*LegendSize);
				pDC->Rectangle(x1-2, Place.y + ypos*k-2,
								x1+2, Place.y + ypos*k+2);
			}
			pDC->TextOut(Place.x + (int)(1.5*LegendSize),
							Place.y  + ypos*k-(int)(ypos/2),
							m_pPF->m_strFoilName);
			pDC->SelectObject(pOldPen);
		}
	}
	CFoil* pRefFoil;

	k++;
	for (n=0; n < m_poaFoil->GetSize(); n++){
		pRefFoil = (CFoil*)m_poaFoil->GetAt(n);
		if(pRefFoil->m_bVisible){
			CString strong;
			strong = pRefFoil->m_FoilName;
			if(strong.GetLength()){//is there anything to draw ?

				color = pRefFoil->m_FoilColor;
				style = pRefFoil->m_nFoilStyle;
				width = pRefFoil->m_nFoilWidth;
				if(IsBlackAndWhite()) GetBWColor(color, style, width);
				lb.lbColor = color;

				CPen LegendPen(PS_GEOMETRIC | style, 
							   GetPenWidth(width,bIsPrinting), &lb);


				CPen *pOldPen = pDC->SelectObject(&LegendPen);

				pDC->MoveTo(Place.x, Place.y + ypos*k);
				pDC->LineTo(Place.x + (int)(LegendSize),
							Place.y + ypos*k);

				if(pRefFoil->m_bPoints && !bIsPrinting){
					x1 = Place.x + (int)(0.5*LegendSize);
					pDC->Rectangle(x1-2, Place.y + ypos*k-2,
								   x1+2, Place.y + ypos*k+2);
				}
				pDC->TextOut(Place.x + (int)(1.5*LegendSize),
							 Place.y  + ypos*k-(int)(ypos/2),
							 pRefFoil->m_FoilName);
				pDC->SelectObject(pOldPen);
				k++;
			}
		}
	}

	pDC->SelectObject(pOldFont);
}



CVector CAFoil::MousetoReal(CPoint *point)
{
	CVector Real;
	
	Real.x =  (point->x - m_ptOffset.x)/m_fScale;
	Real.y = -(point->y - m_ptOffset.y)/m_fScale/m_fScaleY;
	Real.z = 0.0;
	
	return Real;
}


void CAFoil::OnLECircle()
{
	CLECircleDlg dlg(this);
	dlg.m_bShowRadius = m_bCircle;
	dlg.m_Radius      = m_LERad;
	dlg.DoModal();

	UpdateView();
}

void CAFoil::OnSplineControls()
{
	CSplinesCtrlDlg dlg(this);
	dlg.m_pSF = m_pSF;
	dlg.m_pPF = m_pPF;
	dlg.m_bSF = m_bSF;

	if(dlg.DoModal() == IDOK){
	}
}


void CAFoil::OnShowLegend()
{
	m_bShowLegend = !m_bShowLegend;

	CMenu *pMenu = m_pFrame->GetMenu();

	if(m_bShowLegend)   pMenu->GetSubMenu(2)->CheckMenuItem(IDM_SHOWLEGEND, MF_BYCOMMAND | MF_CHECKED);
	else				pMenu->GetSubMenu(2)->CheckMenuItem(IDM_SHOWLEGEND, MF_BYCOMMAND | MF_UNCHECKED);
}


void CAFoil::OnSetFlap() 
{
	CFoil *pCurFoil = m_pACtrl->GetFoil();
	if(!pCurFoil) return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	m_pBufferFoil->CopyFoil(pCurFoil);
//	m_pBufferFoil->m_bPoints  = pCurFoil->m_bPoints;
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = RGB(160,160,160);
	m_pBufferFoil->m_nFoilStyle = PS_DASH;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();	

	CFlapDlg dlg(this);
	dlg.m_pXFoil      = m_pXFoil;
	dlg.m_pMemFoil    = pCurFoil;
	dlg.m_pBufferFoil = m_pBufferFoil;
	dlg.m_pChildView  = m_pChildWnd;
		
	if(IDOK == dlg.DoModal()){
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		if(pFrame->SetModFoil(pNewFoil)){
			m_pACtrl->m_pRefFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil(pNewFoil);
		}
		else {
			pNewFoil = NULL;
			m_pACtrl->FillFoilList();
			m_pACtrl->SelectFoil();
		}
	}

	else{
		m_pACtrl->FillFoilList();
		m_pACtrl->SelectFoil(pCurFoil);
		m_pXFoil->m_FoilName ="";

	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}

void CAFoil::OnBSplines()
{
	m_bSF = true;
	m_pACtrl->FillFoilList();
	m_pACtrl->SelectFoil();
	CMenu *pMenu = m_pFrame->GetMenu();
	pMenu->GetSubMenu(5)->CheckMenuItem(IDM_BSPLINES,      MF_BYCOMMAND | MF_CHECKED);
	pMenu->GetSubMenu(5)->CheckMenuItem(IDM_SPLINEDPOINTS, MF_BYCOMMAND | MF_UNCHECKED);
	UpdateView();
}

void CAFoil::OnSplinedpoints()
{
	m_bSF = false;
	m_pACtrl->FillFoilList();
	m_pACtrl->SelectFoil();
	CMenu *pMenu = m_pFrame->GetMenu();
	pMenu->GetSubMenu(5)->CheckMenuItem(IDM_BSPLINES,      MF_BYCOMMAND | MF_UNCHECKED);
	pMenu->GetSubMenu(5)->CheckMenuItem(IDM_SPLINEDPOINTS, MF_BYCOMMAND | MF_CHECKED);
	UpdateView();
}





