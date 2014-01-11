/****************************************************************************

    Inverse Color Dialog Class
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

// XInvClrDlg.cpp : implementation file
//


#include "stdafx.h"
#include "../X-FLR5.h"
#include "../misc/LinePickerDlg.h"
#include "XInvClrdlg.h"


/////////////////////////////////////////////////////////////////////////////
// CXInvClrDlg dialog


CXInvClrDlg::CXInvClrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXInvClrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXInvClrDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CXInvClrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXInvClrDlg)
	DDX_Control(pDX, IDC_MODFOIL, m_ctrlModFoil);
	DDX_Control(pDX, IDC_SPLINESTYLE, m_ctrlSpline);
	DDX_Control(pDX, IDC_REFFOIL, m_ctrlRefFoil);
	DDX_Control(pDX, IDC_REFLECTEDSTYLE, m_ctrlReflected);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXInvClrDlg, CDialog)
	//{{AFX_MSG_MAP(CXInvClrDlg)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_SPLINESTYLE, OnSplineStyle)
	ON_BN_CLICKED(IDC_MODFOIL, OnModFoilStyle)
	ON_BN_CLICKED(IDC_REFFOIL, OnRefFoilStyle)
	ON_BN_CLICKED(IDC_REFLECTEDSTYLE, OnReflectedStyle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXInvClrDlg message handlers


void CXInvClrDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	if (!pDC) return;
	int nSavedDC = pDC->SaveDC();
	if(nIDCtl==IDC_SPLINESTYLE)
	{
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_SPLINESTYLE)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor = m_SplineClr;
		CPen CurvePen(PS_GEOMETRIC | m_SplineStyle, GetPenWidth(m_SplineWidth,false), &lb);

		pDC->SelectObject(&CurvePen);
		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		pDC->RestoreDC(nSavedDC);	
	}
	if(nIDCtl==IDC_REFLECTEDSTYLE)
	{
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_REFLECTEDSTYLE)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor = m_ReflectedClr;
		CPen CurvePen(PS_GEOMETRIC | m_ReflectedStyle, GetPenWidth(m_ReflectedWidth,false), &lb);

		pDC->SelectObject(&CurvePen);
		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		pDC->RestoreDC(nSavedDC);	
	}
	else if(nIDCtl==IDC_MODFOIL)
	{
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_MODFOIL)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor = m_ModFoilClr;
		CPen CurvePen(PS_GEOMETRIC | m_ModFoilStyle, GetPenWidth(m_ModFoilWidth,false), &lb);

		pDC->SelectObject(&CurvePen);
		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		pDC->RestoreDC(nSavedDC);	
	}
	else if(nIDCtl==IDC_REFFOIL)
	{
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_REFFOIL)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor = m_RefFoilClr;
		CPen CurvePen(PS_GEOMETRIC | m_RefFoilStyle, GetPenWidth(m_RefFoilWidth,false), &lb);

		pDC->SelectObject(&CurvePen);
		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		pDC->RestoreDC(nSavedDC);	
	}
}


void CXInvClrDlg::OnModFoilStyle() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_ModFoilClr);
	LPdlg.SetStyle(m_ModFoilStyle);
	LPdlg.SetWidth(m_ModFoilWidth);

	if (IDOK==LPdlg.DoModal()){
		m_ModFoilClr = LPdlg.GetColor();
		m_ModFoilStyle  = LPdlg.GetStyle();
		m_ModFoilWidth  = LPdlg.GetWidth();
	}

	m_ctrlModFoil.Invalidate(TRUE);
}

void CXInvClrDlg::OnRefFoilStyle() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_RefFoilClr);
	LPdlg.SetStyle(m_RefFoilStyle);
	LPdlg.SetWidth(m_RefFoilWidth);

	if (IDOK==LPdlg.DoModal()){
		m_RefFoilClr = LPdlg.GetColor();
		m_RefFoilStyle  = LPdlg.GetStyle();
		m_RefFoilWidth  = LPdlg.GetWidth();
	}

	m_ctrlRefFoil.Invalidate(TRUE);
}


void CXInvClrDlg::OnReflectedStyle() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_ReflectedClr);
	LPdlg.SetStyle(m_ReflectedStyle);
	LPdlg.SetWidth(m_ReflectedWidth);

	if (IDOK==LPdlg.DoModal()){
		m_ReflectedClr = LPdlg.GetColor();
		m_ReflectedStyle  = LPdlg.GetStyle();
		m_ReflectedWidth  = LPdlg.GetWidth();
	}

	m_ctrlReflected.Invalidate(TRUE);
}
void CXInvClrDlg::OnSplineStyle(){
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_SplineClr);
	LPdlg.SetStyle(m_SplineStyle);
	LPdlg.SetWidth(m_SplineWidth);

	if (IDOK==LPdlg.DoModal()){
		m_SplineClr = LPdlg.GetColor();
		m_SplineStyle  = LPdlg.GetStyle();
		m_SplineWidth  = LPdlg.GetWidth();
	}

	m_ctrlSpline.Invalidate(TRUE);	
}
