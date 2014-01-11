/****************************************************************************

    CFoilClrDlg Class
	Copyright (C) 2003 André Deperrois xflr5@yahoo.com

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
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "FoilClrDlg.h"
#include "../misc/LinePickerDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CFoilClrDlg dialog


CFoilClrDlg::CFoilClrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFoilClrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFoilClrDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFoilClrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFoilClrDlg)
	DDX_Control(pDX, IDC_NEUTRALSTYLE, m_ctrlNeutralStyle);
	DDX_Control(pDX, IDC_NEUTRAL, m_ctrlNeutralCheck);
	DDX_Control(pDX, IDC_BL, m_ctrlBLStyle);
	DDX_Control(pDX, IDC_PRESSURE, m_ctrlPressureStyle);
	DDX_Control(pDX, IDC_FOIL, m_ctrlFoilStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFoilClrDlg, CDialog)
	//{{AFX_MSG_MAP(CFoilClrDlg)
	ON_BN_CLICKED(IDC_BL, OnBl)
	ON_BN_CLICKED(IDC_FOIL, OnFoil)
	ON_BN_CLICKED(IDC_PRESSURE, OnPressure)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_NEUTRALSTYLE, OnNeutralStyle)
	ON_BN_CLICKED(IDC_NEUTRAL, OnNeutral)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFoilClrDlg message handlers

void CFoilClrDlg::OnBl() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_BLColor);
	LPdlg.SetStyle(m_BLStyle);
	LPdlg.SetWidth(m_BLWidth);

	if (IDOK==LPdlg.DoModal()){
		m_BLColor = LPdlg.GetColor();
		m_BLStyle = LPdlg.GetStyle();
		m_BLWidth = LPdlg.GetWidth();
		m_ctrlBLStyle.Invalidate(true);
	}

}

void CFoilClrDlg::OnFoil() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_FoilColor);
	LPdlg.SetStyle(m_FoilStyle);
	LPdlg.SetWidth(m_FoilWidth);

	if (IDOK==LPdlg.DoModal()){
		m_FoilColor = LPdlg.GetColor();
		m_FoilStyle = LPdlg.GetStyle();
		m_FoilWidth = LPdlg.GetWidth();
		m_ctrlFoilStyle.Invalidate(true);
	}
	
}

void CFoilClrDlg::OnPressure() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_PressureColor);
	LPdlg.SetStyle(m_PressureStyle);
	LPdlg.SetWidth(m_PressureWidth);

	if (IDOK==LPdlg.DoModal()){
		m_PressureColor = LPdlg.GetColor();
		m_PressureStyle = LPdlg.GetStyle();
		m_PressureWidth = LPdlg.GetWidth();
		m_ctrlPressureStyle.Invalidate(true);
	}
}

void CFoilClrDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if(nIDCtl==IDC_FOIL){

		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		if (!pDC) return;
		int nSavedDC = pDC->SaveDC();
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_FOIL)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor = m_FoilColor;
		CPen Pen(PS_GEOMETRIC | m_FoilStyle, m_FoilWidth, &lb);
		pDC->SelectObject(&Pen);
		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		pDC->RestoreDC(nSavedDC);


	}
	else if(nIDCtl==IDC_BL){
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		if (!pDC) return;
		int nSavedDC = pDC->SaveDC();
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_BL)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor = m_BLColor;
		CPen Pen(PS_GEOMETRIC | m_BLStyle, m_BLWidth, &lb);
		pDC->SelectObject(&Pen);
		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		pDC->RestoreDC(nSavedDC);
	}
	else if(nIDCtl==IDC_PRESSURE){
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		if (!pDC) return;
		int nSavedDC = pDC->SaveDC();
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_PRESSURE)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor = m_PressureColor;
		CPen Pen(PS_GEOMETRIC | m_PressureStyle, m_PressureWidth, &lb);
		pDC->SelectObject(&Pen);
		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		pDC->RestoreDC(nSavedDC);
	}
	else if(nIDCtl==IDC_NEUTRALSTYLE){
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		if (!pDC) return;
		int nSavedDC = pDC->SaveDC();
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_NEUTRALSTYLE)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor = m_NeutralColor;
		CPen Pen(PS_GEOMETRIC | m_NeutralStyle, m_NeutralWidth, &lb);
		pDC->SelectObject(&Pen);
		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		pDC->RestoreDC(nSavedDC);
	}

	else CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CFoilClrDlg::OnNeutralStyle() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_NeutralColor);
	LPdlg.SetStyle(m_NeutralStyle);
	LPdlg.SetWidth(m_NeutralWidth);

	if (IDOK==LPdlg.DoModal()){
		m_NeutralColor = LPdlg.GetColor();
		m_NeutralStyle = LPdlg.GetStyle();
		m_NeutralWidth = LPdlg.GetWidth();
		m_ctrlNeutralStyle.Invalidate(true);
	}	
}

void CFoilClrDlg::OnNeutral() 
{
	if(m_ctrlNeutralCheck.GetCheck()){
		m_bNeutral = true;
		m_ctrlNeutralStyle.EnableWindow(true);
	}
	else{
		m_bNeutral = false;
		m_ctrlNeutralStyle.EnableWindow(false);
	}
}

BOOL CFoilClrDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_bNeutral) {
		m_ctrlNeutralCheck.SetCheck(TRUE);
		m_ctrlNeutralStyle.EnableWindow(true);
	}
	else {
        m_ctrlNeutralCheck.SetCheck(FALSE);
		m_ctrlNeutralStyle.EnableWindow(false);
	}
	return TRUE;
}
