/****************************************************************************

    Miarex Class
	Copyright (C) 2007-2008 André Deperrois xflr5@yahoo.com

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
// BodyGridDlg.cpp : fichier d'implémentation
//

#include "stdafx.h"
#include "../main/MainFrm.h"
#include "../X-FLR5.h"
#include ".\BodyGridDlg.h"
#include "../misc/LinePickerDlg.h"

// Boîte de dialogue CBodyGridDlg

IMPLEMENT_DYNAMIC(CBodyGridDlg, CDialog)
CBodyGridDlg::CBodyGridDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBodyGridDlg::IDD, pParent)
{
	m_bScale = false;

	m_bGrid = false; 
	m_Style = 1; m_Width =1;
	m_Color = RGB(150,150,150); 
	m_Unit = 0.2; 

	m_bMinGrid = false; 
	m_MinStyle=2; m_MinWidth =1; 
	m_MinColor=RGB(75,75,75); 
	m_MinorUnit=0.05;


	m_bGrid2 = false; 
	m_Style2 = 1; m_Width2 =1;
	m_Color2 = RGB(150,150,150); 
	m_Unit2 = 0.01; 

	m_bMinGrid2 = false; 
	m_MinStyle2=2; m_MinWidth2 =1;
	m_MinColor2=RGB(75,75,75);
	m_MinorUnit2=0.002; 

}

CBodyGridDlg::~CBodyGridDlg()
{
}

void CBodyGridDlg::DoDataExchange(CDataExchange* pD)
{
	CDialog::DoDataExchange(pD);
	DDX_Control(pD, IDC_SCALE, m_ctrlScale);
	DDX_Control(pD, IDC_MINORUNIT, m_ctrlMinorUnit);
	DDX_Control(pD, IDC_UNIT, m_ctrlUnit);
	DDX_Control(pD, IDC_MINSTYLE, m_ctrlMinStyle);
	DDX_Control(pD, IDC_MINORGRID, m_ctrlMinCheck);
	DDX_Control(pD, IDC_MAINGRID, m_ctrlMainCheck);
	DDX_Control(pD, IDC_GRIDSTYLE, m_ctrlMainStyle);

	DDX_Control(pD, IDC_MINORUNIT2, m_ctrlMinorUnit2);
	DDX_Control(pD, IDC_UNIT2, m_ctrlUnit2);
	DDX_Control(pD, IDC_MINSTYLE2, m_ctrlMinStyle2);
	DDX_Control(pD, IDC_MINORGRID2, m_ctrlMinCheck2);
	DDX_Control(pD, IDC_MAINGRID2, m_ctrlMainCheck2);
	DDX_Control(pD, IDC_GRIDSTYLE2, m_ctrlMainStyle2);

	DDX_Control(pD, IDC_LENGTH1, m_ctrlLength1);
	DDX_Control(pD, IDC_LENGTH2, m_ctrlLength2);
	DDX_Control(pD, IDC_LENGTH3, m_ctrlLength3);
	DDX_Control(pD, IDC_LENGTH4, m_ctrlLength4);

}


// Gestionnaires de messages CBodyGridDlg



BEGIN_MESSAGE_MAP(CBodyGridDlg, CDialog)
	//{{AF_MSG_MAP(CBodyGridDlg)
	ON_BN_CLICKED(IDC_GRIDSTYLE, OnGridStyle)
	ON_BN_CLICKED(IDC_MAINGRID, OnGridCheck)
	ON_BN_CLICKED(IDC_MINSTYLE, OnMinStyle)
	ON_BN_CLICKED(IDC_MINORGRID, OnMinCheck)
	ON_BN_CLICKED(IDC_SCALE, OnScale)
	ON_BN_CLICKED(IDC_GRIDSTYLE2, OnGridStyle2)
	ON_BN_CLICKED(IDC_MAINGRID2, OnGridCheck2)
	ON_BN_CLICKED(IDC_MINSTYLE2, OnMinStyle2)
	ON_BN_CLICKED(IDC_MINORGRID2, OnMinCheck2)

	ON_WM_DRAWITEM()
	ON_WM_DESTROY()
	//}}AF_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBodyGridDlg message handlers


BOOL CBodyGridDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	CString str;

	if(m_bScale) m_ctrlScale.SetCheck(1);

	m_ctrlUnit.SetPrecision(3);
	m_ctrlMinorUnit.SetPrecision(3);

	if(m_bGrid) m_ctrlMainCheck.SetCheck(1);	
	OnGridCheck();

	m_ctrlUnit.SetValue(m_Unit*pFrame->m_mtoUnit);

	if(m_bMinGrid) m_ctrlMinCheck.SetCheck(1);	
	m_ctrlMinorUnit.SetValue(m_MinorUnit*pFrame->m_mtoUnit);
	OnMinCheck();

	m_ctrlUnit2.SetPrecision(3);
	m_ctrlMinorUnit2.SetPrecision(3);

	if(m_bGrid2) m_ctrlMainCheck2.SetCheck(1);	
	OnGridCheck2();

	m_ctrlUnit2.SetValue(m_Unit2*pFrame->m_mtoUnit);

	if(m_bMinGrid2) m_ctrlMinCheck2.SetCheck(1);	
	m_ctrlMinorUnit2.SetValue(m_MinorUnit2*pFrame->m_mtoUnit);
	OnMinCheck2();

	GetLengthUnit(str, pFrame->m_LengthUnit);
	m_ctrlLength1.SetWindowText(str);
	m_ctrlLength2.SetWindowText(str);
	m_ctrlLength3.SetWindowText(str);
	m_ctrlLength4.SetWindowText(str);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // ECEPTION: OC Property Pages should return FALSE
}

void CBodyGridDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	if (!pDC) return;
//	int nSavedDC = dc.SaveDC();
	switch (nIDCtl)
	{

		case IDC_GRIDSTYLE:
		{
			LOGBRUSH lb;
			lb.lbStyle = BS_SOLID;
			lb.lbColor = RGB(100,100,100);
			CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
			CPen *pOldPen =  pDC->SelectObject(&DotPen);
			CBrush FillBrush(GetSysColor(COLOR_3DFACE));
			CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
			CRect SRect ;
			GetDlgItem(IDC_GRIDSTYLE)->GetClientRect(&SRect);
			SRect.DeflateRect(2,2,2,2);
			pDC->Rectangle(&SRect);

			lb.lbStyle = BS_SOLID;
			lb.lbColor = m_Color;
			CPen CurvePen(PS_GEOMETRIC | m_Style, GetPenWidth(m_Width,false), &lb);

			pDC->SelectObject(&CurvePen);
			pDC->MoveTo(5,8);
			pDC->LineTo(SRect.right-5,8);
			pDC->SelectObject(pOldPen);
			pDC->SelectObject(pOldBrush);
			break;
		}

		case IDC_MINSTYLE:{
			LOGBRUSH lb;
			lb.lbStyle = BS_SOLID;
			lb.lbColor = RGB(100,100,100);
			CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
			CPen *pOldPen =  pDC->SelectObject(&DotPen);
			CBrush FillBrush(GetSysColor(COLOR_3DFACE));
			CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
			CRect SRect ;
			GetDlgItem(IDC_MINSTYLE)->GetClientRect(&SRect);
			SRect.DeflateRect(2,2,2,2);
			pDC->Rectangle(&SRect);

			lb.lbStyle = BS_SOLID;
			lb.lbColor = m_MinColor;
			CPen CurvePen(PS_GEOMETRIC | m_MinStyle, GetPenWidth(m_MinWidth,false), &lb);

			pDC->SelectObject(&CurvePen);
			pDC->MoveTo(5,8);
			pDC->LineTo(SRect.right-5,8);
			pDC->SelectObject(pOldPen);
			pDC->SelectObject(pOldBrush);
			break;
		}


		case IDC_GRIDSTYLE2:{
			LOGBRUSH lb;
			lb.lbStyle = BS_SOLID;
			lb.lbColor = RGB(100,100,100);
			CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
			CPen *pOldPen =  pDC->SelectObject(&DotPen);
			CBrush FillBrush(GetSysColor(COLOR_3DFACE));
			CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
			CRect SRect ;
			GetDlgItem(IDC_GRIDSTYLE2)->GetClientRect(&SRect);
			SRect.DeflateRect(2,2,2,2);
			pDC->Rectangle(&SRect);

			lb.lbStyle = BS_SOLID;
			lb.lbColor = m_Color2;
			CPen CurvePen(PS_GEOMETRIC | m_Style2, GetPenWidth(m_Width2,false), &lb);

			pDC->SelectObject(&CurvePen);
			pDC->MoveTo(5,8);
			pDC->LineTo(SRect.right-5,8);
			pDC->SelectObject(pOldPen);
			pDC->SelectObject(pOldBrush);
			break;
		}

		case IDC_MINSTYLE2:{
			LOGBRUSH lb;
			lb.lbStyle = BS_SOLID;
			lb.lbColor = RGB(100,100,100);
			CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
			CPen *pOldPen =  pDC->SelectObject(&DotPen);
			CBrush FillBrush(GetSysColor(COLOR_3DFACE));
			CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
			CRect SRect ;
			GetDlgItem(IDC_MINSTYLE2)->GetClientRect(&SRect);
			SRect.DeflateRect(2,2,2,2);
			pDC->Rectangle(&SRect);

			lb.lbStyle = BS_SOLID;
			lb.lbColor = m_MinColor2;
			CPen CurvePen(PS_GEOMETRIC | m_MinStyle2, GetPenWidth(m_MinWidth2,false), &lb);

			pDC->SelectObject(&CurvePen);
			pDC->MoveTo(5,8);
			pDC->LineTo(SRect.right-5,8);
			pDC->SelectObject(pOldPen);
			pDC->SelectObject(pOldBrush);
			break;
		}

	}
}



void CBodyGridDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	
	m_Unit      = m_ctrlUnit.GetValue()/pFrame->m_mtoUnit;
	m_MinorUnit = m_ctrlMinorUnit.GetValue()/pFrame->m_mtoUnit;

	m_Unit2      = m_ctrlUnit2.GetValue()/pFrame->m_mtoUnit;
	m_MinorUnit2 = m_ctrlMinorUnit2.GetValue()/pFrame->m_mtoUnit;

}


void CBodyGridDlg::OnGridCheck() 
{
	if(m_ctrlMainCheck.GetCheck())	{
		m_ctrlUnit.EnableWindow(true);
		m_ctrlMainStyle.EnableWindow(true);
		m_bGrid = true;
	}
	else {
		m_ctrlUnit.EnableWindow(false);
		m_ctrlMainStyle.EnableWindow(false);
		m_bGrid = false;
	}
}




void CBodyGridDlg::OnGridStyle() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_Color);
	LPdlg.SetStyle(m_Style);
	LPdlg.SetWidth(m_Width);

	if (IDOK==LPdlg.DoModal()){
		m_Color = LPdlg.GetColor();
		m_Style = LPdlg.GetStyle();
		m_Width = LPdlg.GetWidth();
	}

	Invalidate();	
}




void CBodyGridDlg::OnMinStyle() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_MinColor);
	LPdlg.SetStyle(m_MinStyle);
	LPdlg.SetWidth(m_MinWidth);

	if (IDOK==LPdlg.DoModal()){
		m_MinColor = LPdlg.GetColor();
		m_MinStyle = LPdlg.GetStyle();
		m_MinWidth = LPdlg.GetWidth();
	}
	Invalidate();	
}


void CBodyGridDlg::OnMinCheck() 
{
	if(m_ctrlMinCheck.GetCheck())	{
		m_ctrlMinorUnit.EnableWindow(true);
		m_ctrlMinStyle.EnableWindow(true);
		m_bMinGrid = true;
	}
	else {
		m_ctrlMinorUnit.EnableWindow(false);
		m_ctrlMinStyle.EnableWindow(false);
		m_bMinGrid = false;
	}
	
}




void CBodyGridDlg::OnOK() 
{
/*	if (m_MinorUnit > m_Unit){
		m_ctrlMinorUnit.SetSel(0,-1);
		m_ctrlMinorUnit.SetFocus();
		return;
	}*/

	CDialog::OnOK();
}

void CBodyGridDlg::OnScale() 
{
	if(m_ctrlScale.GetCheck())	{
		m_bScale = true;
	}
	else {
		m_bScale = false;
	}
}


void CBodyGridDlg::OnGridStyle2() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_Color2);
	LPdlg.SetStyle(m_Style2);
	LPdlg.SetWidth(m_Width2);

	if (IDOK==LPdlg.DoModal()){
		m_Color2 = LPdlg.GetColor();
		m_Style2 = LPdlg.GetStyle();
		m_Width2 = LPdlg.GetWidth();
	}

	Invalidate();	
}




void CBodyGridDlg::OnMinStyle2() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_MinColor2);
	LPdlg.SetStyle(m_MinStyle2);
	LPdlg.SetWidth(m_MinWidth2);

	if (IDOK==LPdlg.DoModal()){
		m_MinColor2 = LPdlg.GetColor();
		m_MinStyle2 = LPdlg.GetStyle();
		m_MinWidth2 = LPdlg.GetWidth();
	}
	Invalidate();	
}


void CBodyGridDlg::OnMinCheck2() 
{
	if(m_ctrlMinCheck2.GetCheck())	{
		m_ctrlMinorUnit2.EnableWindow(true);
		m_ctrlMinStyle2.EnableWindow(true);
		m_bMinGrid2 = true;
	}
	else {
		m_ctrlMinorUnit2.EnableWindow(false);
		m_ctrlMinStyle2.EnableWindow(false);
		m_bMinGrid2 = false;
	}
	
}



void CBodyGridDlg::OnGridCheck2() 
{
	if(m_ctrlMainCheck2.GetCheck())	{
		m_ctrlUnit2.EnableWindow(true);
		m_ctrlMainStyle2.EnableWindow(true);
		m_bGrid2 = true;
	}
	else {
		m_ctrlUnit2.EnableWindow(false);
		m_ctrlMainStyle2.EnableWindow(false);
		m_bGrid2 = false;
	}
}

