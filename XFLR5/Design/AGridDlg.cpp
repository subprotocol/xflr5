/****************************************************************************

	CAGridDlg class
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
// AGridDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "AGridDlg.h"
#include "../misc/LinePickerDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CAGridDlg dialog


CAGridDlg::CAGridDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAGridDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAGridDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bScale   = true;
	m_bNeutral = true;
	m_pParent  = pParent;
}


void CAGridDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAGridDlg)
	DDX_Control(pDX, IDC_SCALE, m_ctrlScale);
	DDX_Control(pDX, IDC_YMINSTYLE, m_ctrlYMinStyle);
	DDX_Control(pDX, IDC_YMINORGRID, m_ctrlYMinCheck);
	DDX_Control(pDX, IDC_YMAINGRID, m_ctrlYMainCheck);
	DDX_Control(pDX, IDC_YGRIDSTYLE, m_ctrlYMainStyle);
	DDX_Control(pDX, IDC_YMINORUNIT, m_ctrlYMinorUnit);
	DDX_Control(pDX, IDC_XMINORUNIT, m_ctrlXMinorUnit);
	DDX_Control(pDX, IDC_YUNIT, m_ctrlYUnit);
	DDX_Control(pDX, IDC_XUNIT, m_ctrlXUnit);
	DDX_Control(pDX, IDC_XMINSTYLE, m_ctrlXMinStyle);
	DDX_Control(pDX, IDC_XMINORGRID, m_ctrlXMinCheck);
	DDX_Control(pDX, IDC_XMAINGRID, m_ctrlXMainCheck);
	DDX_Control(pDX, IDC_XGRIDSTYLE, m_ctrlXMainStyle);
	DDX_Control(pDX, IDC_NEUTRALLINE, m_ctrlNeutralLine);
	DDX_Control(pDX, IDC_NEUTRALCHECK, m_ctrlNeutralCheck);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAGridDlg, CDialog)
	//{{AFX_MSG_MAP(CAGridDlg)
	ON_BN_CLICKED(IDC_XGRIDSTYLE, OnXGridStyle)
	ON_BN_CLICKED(IDC_XMAINGRID, OnXGridCheck)
	ON_BN_CLICKED(IDC_YMAINGRID, OnYGridCheck)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_YGRIDSTYLE, OnYGridStyle)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_NEUTRALLINE, OnNeutralLine)
	ON_BN_CLICKED(IDC_NEUTRALCHECK, OnNeutralCheck)
	ON_BN_CLICKED(IDC_XMINSTYLE, OnXMinStyle)
	ON_BN_CLICKED(IDC_YMINSTYLE, OnYMinStyle)
	ON_BN_CLICKED(IDC_XMINORGRID, OnXMinCheck)
	ON_BN_CLICKED(IDC_YMINORGRID, OnYMinCheck)
	ON_BN_CLICKED(IDC_SCALE, OnScale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAGridDlg message handlers

void CAGridDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	if (!pDC) return;
//	int nSavedDC = dc.SaveDC();
	switch (nIDCtl){
		case IDC_XGRIDSTYLE:{
			LOGBRUSH lb;
			lb.lbStyle = BS_SOLID;
			lb.lbColor = RGB(100,100,100);
			CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
			CPen *pOldPen =  pDC->SelectObject(&DotPen);
			CBrush FillBrush(GetSysColor(COLOR_3DFACE));
			CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
			CRect SRect ;
			GetDlgItem(IDC_XGRIDSTYLE)->GetClientRect(&SRect);
			SRect.DeflateRect(2,2,2,2);
			pDC->Rectangle(&SRect);

			lb.lbStyle = BS_SOLID;
			lb.lbColor = m_XColor;
			CPen CurvePen(PS_GEOMETRIC | m_XStyle, GetPenWidth(m_XWidth,false), &lb);

			pDC->SelectObject(&CurvePen);
			pDC->MoveTo(5,8);
			pDC->LineTo(SRect.right-5,8);
			pDC->SelectObject(pOldPen);
			pDC->SelectObject(pOldBrush);
			break;
		}
		case IDC_YGRIDSTYLE:{
			LOGBRUSH lb;
			lb.lbStyle = BS_SOLID;
			lb.lbColor = RGB(100,100,100);
			CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
			CPen *pOldPen =  pDC->SelectObject(&DotPen);
			CBrush FillBrush(GetSysColor(COLOR_3DFACE));
			CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
			CRect SRect ;
			GetDlgItem(IDC_YGRIDSTYLE)->GetClientRect(&SRect);
			SRect.DeflateRect(2,2,2,2);
			pDC->Rectangle(&SRect);

			lb.lbStyle = BS_SOLID;
			lb.lbColor = m_YColor;
			CPen CurvePen(PS_GEOMETRIC | m_YStyle, GetPenWidth(m_YWidth,false), &lb);

			pDC->SelectObject(&CurvePen);
			pDC->MoveTo(5,8);
			pDC->LineTo(SRect.right-5,8);
			pDC->SelectObject(pOldPen);
			pDC->SelectObject(pOldBrush);
			break;
		}
		case IDC_XMINSTYLE:{
			LOGBRUSH lb;
			lb.lbStyle = BS_SOLID;
			lb.lbColor = RGB(100,100,100);
			CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
			CPen *pOldPen =  pDC->SelectObject(&DotPen);
			CBrush FillBrush(GetSysColor(COLOR_3DFACE));
			CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
			CRect SRect ;
			GetDlgItem(IDC_XMINSTYLE)->GetClientRect(&SRect);
			SRect.DeflateRect(2,2,2,2);
			pDC->Rectangle(&SRect);

			lb.lbStyle = BS_SOLID;
			lb.lbColor = m_XMinColor;
			CPen CurvePen(PS_GEOMETRIC | m_XMinStyle, GetPenWidth(m_XMinWidth,false), &lb);

			pDC->SelectObject(&CurvePen);
			pDC->MoveTo(5,8);
			pDC->LineTo(SRect.right-5,8);
			pDC->SelectObject(pOldPen);
			pDC->SelectObject(pOldBrush);
			break;
		}
		case IDC_YMINSTYLE:{
			LOGBRUSH lb;
			lb.lbStyle = BS_SOLID;
			lb.lbColor = RGB(100,100,100);
			CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
			CPen *pOldPen =  pDC->SelectObject(&DotPen);
			CBrush FillBrush(GetSysColor(COLOR_3DFACE));
			CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
			CRect SRect ;
			GetDlgItem(IDC_YMINSTYLE)->GetClientRect(&SRect);
			SRect.DeflateRect(2,2,2,2);
			pDC->Rectangle(&SRect);

			lb.lbStyle = BS_SOLID;
			lb.lbColor = m_YMinColor;
			CPen CurvePen(PS_GEOMETRIC | m_YMinStyle, GetPenWidth(m_YMinWidth,false), &lb);

			pDC->SelectObject(&CurvePen);
			pDC->MoveTo(5,8);
			pDC->LineTo(SRect.right-5,8);
			pDC->SelectObject(pOldPen);
			pDC->SelectObject(pOldBrush);
			break;
		}
		case IDC_NEUTRALLINE:{
			LOGBRUSH lb;
			lb.lbStyle = BS_SOLID;
			lb.lbColor = RGB(100,100,100);
			CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
			CPen *pOldPen =  pDC->SelectObject(&DotPen);
			CBrush FillBrush(GetSysColor(COLOR_3DFACE));
			CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
			CRect SRect ;
			GetDlgItem(IDC_NEUTRALLINE)->GetClientRect(&SRect);
			SRect.DeflateRect(2,2,2,2);
			pDC->Rectangle(&SRect);

			lb.lbStyle = BS_SOLID;
			lb.lbColor = m_NeutralColor;
			CPen CurvePen(PS_GEOMETRIC | m_NeutralStyle, GetPenWidth(m_NeutralWidth,false), &lb);

			pDC->SelectObject(&CurvePen);
			pDC->MoveTo(5,8);
			pDC->LineTo(SRect.right-5,8);
			pDC->SelectObject(pOldPen);
			pDC->SelectObject(pOldBrush);
			break;
		}
	}
}


void CAGridDlg::OnXGridCheck() 
{
	if(m_ctrlXMainCheck.GetCheck())	{
		m_ctrlXUnit.EnableWindow(true);
		m_ctrlXMainStyle.EnableWindow(true);
		m_bXGrid = true;
	}
	else {
		m_ctrlXUnit.EnableWindow(false);
		m_ctrlXMainStyle.EnableWindow(false);
		m_bXGrid = false;
	}
}


void CAGridDlg::OnYGridCheck() 
{
	if(m_ctrlYMainCheck.GetCheck())	{
		m_ctrlYUnit.EnableWindow(true);
		m_ctrlYMainStyle.EnableWindow(true);
		m_bYGrid = true;
	}
	else {
		m_ctrlYUnit.EnableWindow(false);
		m_ctrlYMainStyle.EnableWindow(false);
		m_bYGrid = false;
	}
}


BOOL CAGridDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_ctrlXUnit.SetPrecision(3);
	m_ctrlYUnit.SetPrecision(3);
	m_ctrlXMinorUnit.SetPrecision(3);
	m_ctrlYMinorUnit.SetPrecision(3);
	if(m_bNeutral) m_ctrlNeutralCheck.SetCheck(1);
	if(m_bScale) m_ctrlScale.SetCheck(1);

	if(m_bXGrid) m_ctrlXMainCheck.SetCheck(1);	
	if(m_bYGrid) m_ctrlYMainCheck.SetCheck(1);
	OnXGridCheck();
	OnYGridCheck();

	m_ctrlXUnit.SetValue(m_XUnit);
	m_ctrlYUnit.SetValue(m_YUnit);

	if(m_bXMinGrid) m_ctrlXMinCheck.SetCheck(1);	
	if(m_bYMinGrid) m_ctrlYMinCheck.SetCheck(1);
	m_ctrlXMinorUnit.SetValue(m_XMinorUnit);
	m_ctrlYMinorUnit.SetValue(m_YMinorUnit);
	OnXMinCheck();
	OnYMinCheck();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CAGridDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	m_XUnit = m_ctrlXUnit.GetValue();
	m_YUnit = m_ctrlYUnit.GetValue();
	m_XMinorUnit = m_ctrlXMinorUnit.GetValue();
	m_YMinorUnit = m_ctrlYMinorUnit.GetValue();

}



void CAGridDlg::OnNeutralLine()
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_NeutralColor);
	LPdlg.SetStyle(m_NeutralStyle);
	LPdlg.SetWidth(m_NeutralWidth);

	if (IDOK==LPdlg.DoModal()){
		m_NeutralColor = LPdlg.GetColor();
		m_NeutralStyle = LPdlg.GetStyle();
		m_NeutralWidth = LPdlg.GetWidth();
	}

	m_ctrlNeutralLine.Invalidate(TRUE);
	
}

void CAGridDlg::OnNeutralCheck() 
{
	if(m_ctrlNeutralCheck.GetCheck())	{
		m_ctrlNeutralLine.EnableWindow(true);
		m_bNeutral = true;
	}
	else {
		m_ctrlNeutralLine.EnableWindow(false);
		m_bNeutral = false;
	}
}


void CAGridDlg::OnXGridStyle() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_XColor);
	LPdlg.SetStyle(m_XStyle);
	LPdlg.SetWidth(m_XWidth);

	if (IDOK==LPdlg.DoModal()){
		m_XColor = LPdlg.GetColor();
		m_XStyle = LPdlg.GetStyle();
		m_XWidth = LPdlg.GetWidth();
	}

	Invalidate();	
}


void CAGridDlg::OnYGridStyle() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_YColor);
	LPdlg.SetStyle(m_YStyle);
	LPdlg.SetWidth(m_YWidth);

	if (IDOK==LPdlg.DoModal()){
		m_YColor = LPdlg.GetColor();
		m_YStyle = LPdlg.GetStyle();
		m_YWidth = LPdlg.GetWidth();
	}

	Invalidate();	
}


void CAGridDlg::OnXMinStyle() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_XMinColor);
	LPdlg.SetStyle(m_XMinStyle);
	LPdlg.SetWidth(m_XMinWidth);

	if (IDOK==LPdlg.DoModal()){
		m_XMinColor = LPdlg.GetColor();
		m_XMinStyle = LPdlg.GetStyle();
		m_XMinWidth = LPdlg.GetWidth();
	}
	Invalidate();	
}

void CAGridDlg::OnYMinStyle() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_YMinColor);
	LPdlg.SetStyle(m_YMinStyle);
	LPdlg.SetWidth(m_YMinWidth);

	if (IDOK==LPdlg.DoModal()){
		m_YMinColor = LPdlg.GetColor();
		m_YMinStyle = LPdlg.GetStyle();
		m_YMinWidth = LPdlg.GetWidth();
	}

	Invalidate();	
}

void CAGridDlg::OnXMinCheck() 
{
	if(m_ctrlXMinCheck.GetCheck())	{
		m_ctrlXMinorUnit.EnableWindow(true);
		m_ctrlXMinStyle.EnableWindow(true);
		m_bXMinGrid = true;
	}
	else {
		m_ctrlXMinorUnit.EnableWindow(false);
		m_ctrlXMinStyle.EnableWindow(false);
		m_bXMinGrid = false;
	}
	
}

void CAGridDlg::OnYMinCheck() 
{
	if(m_ctrlYMinCheck.GetCheck())	{
		m_ctrlYMinorUnit.EnableWindow(true);
		m_ctrlYMinStyle.EnableWindow(true);
		m_bYMinGrid = true;
	}
	else {
		m_ctrlYMinorUnit.EnableWindow(false);
		m_ctrlYMinStyle.EnableWindow(false);
		m_bYMinGrid = false;
	}	
}



void CAGridDlg::OnOK() 
{
	if (m_XMinorUnit > m_XUnit){
		m_ctrlXMinorUnit.SetSel(0,-1);
		m_ctrlXMinorUnit.SetFocus();
		return;
	}
	if (m_YMinorUnit > m_YUnit){
		m_ctrlYMinorUnit.SetSel(0,-1);
		m_ctrlYMinorUnit.SetFocus();
		return;
	}
	CDialog::OnOK();
}

void CAGridDlg::OnScale() 
{
	if(m_ctrlScale.GetCheck())	{
		m_bScale = true;
	}
	else {
		m_bScale = false;
	}
	
}
