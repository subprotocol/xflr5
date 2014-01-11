/****************************************************************************

	Preferences dialog 

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


// BckgndclrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/ChildView.h"
#include "PrefsDlg.h"
#include "LinePickerDlg.h"
#include "../Graph/GraphDlg.h"



/////////////////////////////////////////////////////////////////////////////
// CPrefsDlg dialog


CPrefsDlg::CPrefsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrefsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrefsDlg)
	//}}AFX_DATA_INIT
	m_pParent = pParent;
	m_bUserDefined = true;
	m_bBlackAndWhite = false;
}


void CPrefsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrefsDlg)
	DDX_Control(pDX, IDC_STDSETTINGS, m_ctrlStdSettings);
	DDX_Control(pDX, IDC_WNDTXTFONT, m_ctrlTextFont);
	DDX_Control(pDX, IDC_WNDCLR, m_ctrlWndClr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrefsDlg, CDialog)
	//{{AFX_MSG_MAP(CPrefsDlg)
	ON_BN_CLICKED(IDC_WNDCLR, OnWndCr)
	ON_CBN_SELCHANGE(IDC_STDSETTINGS, OnSelChangeStdSettings)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_WNDTXTFONT, OnWndTxtFont)
	ON_BN_CLICKED(IDC_GRAPHPREFS, OnGraphsPrefs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefsDlg message handlers
 


void CPrefsDlg::OnWndCr() 
{
	m_crWnd = m_ctrlWndClr.GetColor();
	m_ctrlStdSettings.SetCurSel(3);
	m_bBlackAndWhite = false;
}


BOOL CPrefsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_WndFont.CreateFontIndirect(&m_WndLogFont);
	m_ctrlTextFont.SetFont(&m_WndFont);

	m_ctrlWndClr.SetColor(m_crWnd);

	m_ctrlStdSettings.AddString("Black and White");
	m_ctrlStdSettings.AddString("Dark");
	m_ctrlStdSettings.AddString("Light");
	m_ctrlStdSettings.AddString("User Defined");
	if(m_bBlackAndWhite) m_ctrlStdSettings.SetCurSel(0);
	else{
		if (m_bUserDefined) m_ctrlStdSettings.SetCurSel(3);
		else m_ctrlStdSettings.SetCurSel(0);
	}
	m_ctrlStdSettings.SetFocus();
	

	return FALSE; 
}


void CPrefsDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if(nIDCtl==IDC_WNDTXTFONT){
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
		CRect SRect;
		GetDlgItem(IDC_WNDTXTFONT)->GetClientRect(&SRect);
		pDC->Rectangle(&SRect);

		pDC->SetTextColor(m_WndTextColor);
		CFont m_BtnFont;
		m_BtnFont.CreateFontIndirect(&m_WndLogFont);
		CFont *pOldFont = pDC->SelectObject(&m_BtnFont);
		
		pDC->TextOut(5, 5, "Text");

		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldFont);
		pDC->RestoreDC(nSavedDC);
	}
	else{
		// draw color buttons
		CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}


void CPrefsDlg::OnWndTxtFont() 
{
	CFontDialog FtDlg(&m_WndLogFont);
	FtDlg.m_cf.rgbColors = m_WndTextColor;
	if(IDOK==FtDlg.DoModal()){
		FtDlg.GetCurrentFont(&m_WndLogFont);
		m_WndTextColor = FtDlg.GetColor();
		m_bBlackAndWhite = false;
		m_ctrlStdSettings.SetCurSel(3);
	}	
	m_ctrlTextFont.Invalidate();
}

void CPrefsDlg::OnGraphsPrefs() 
{
	Graph TempGraph;
//	CCurve *pCurve = TempGraph.AddCurve();
	TempGraph.CopySettings(m_pGraph);
	TempGraph.SetXMinorUnit(0.01f);
	TempGraph.SetYMinorUnit(0.01f);

	CGraphDlg dlg;
	dlg.m_pGraph = &TempGraph;
	dlg.m_pParent = this;

	if(IDOK == dlg.DoModal()){
		m_pGraph->CopySettings(&TempGraph);
		m_ctrlStdSettings.SetCurSel(3);
	}
}

void CPrefsDlg::OnApply()
{
	CChildView* pChildView = (CChildView*)m_pParent;
	pChildView->m_crBackColor = m_crWnd;
	pChildView->m_WndTextColor = m_WndTextColor;

	memcpy(&(pChildView->m_WndLogFont), &m_WndLogFont, sizeof(m_WndLogFont));
	pChildView->ApplyClrSettings(m_bBlackAndWhite, m_WingPanels, m_XCPColor,
								 m_XTopColor, m_XBotColor, m_DownwashColor,
								 m_crFoilColor, m_crBLColor, m_crPressureColor);
}

void CPrefsDlg::OnSelChangeStdSettings()
{
	int pos = m_ctrlStdSettings.GetCurSel();
	switch (pos)
	{
		case 0: //black and white
			{
				m_bBlackAndWhite = true;
				m_crWnd = RGB(255,255,255);
				m_WndTextColor = RGB(0,0,0);
				m_pGraph->SetBkColor(RGB(255,255,255));
				m_pGraph->SetBorderColor(RGB(20,20,20));
				m_pGraph->SetAxisColor(RGB(0,0,0));
				m_pGraph->SetXMajGrid(true, RGB(100,100,100), PS_DASH, 0);
				m_pGraph->SetYMajGrid(true, RGB(100,100,100), PS_DASH, 0);
				m_pGraph->SetXMinGrid(false, true, RGB(170,170,170),PS_DASH,0,0.01);
				m_pGraph->SetYMinGrid(false, true, RGB(170,170,170),PS_DASH,0,0.01);
				m_pGraph->SetLabelColor(RGB(0,0,0));
				m_pGraph->SetLegendColor(RGB(0,0,0));
				m_pGraph->SetTitleColor(RGB(0,0,0));

				//	Miarex wing settings
				m_WingPanels    = RGB(0,0,0);
//				m_WingSurfaces  = RGB(70,70,70);
				m_XCPColor      = RGB(120,120,120);
				m_XTopColor     = RGB(60,60,60);
				m_XBotColor     = RGB(60,60,60);
				m_DownwashColor = RGB(60,60,60);

				// XFLR5 foil settings
				m_crFoilColor     = RGB(0,0,0);
				m_crBLColor       = RGB(60,60,60);
				m_crPressureColor = RGB(60,60,60);

				break;
			}
		case 1: //Dark
			{
				m_bBlackAndWhite = false;
				m_crWnd        = RGB(0, 20, 40);
				m_WndTextColor = 12632256;
				m_pGraph->SetBkColor(RGB(0, 30, 50));
				m_pGraph->SetBorderColor(RGB(200,200,200));
				m_pGraph->SetAxisColor(15780518);
				m_pGraph->SetTitleColor(RGB(255,255,255));
				m_pGraph->SetLabelColor(RGB(255,255,255));
				m_pGraph->SetLegendColor(RGB(255,255,255));

				m_pGraph->SetXMajGrid(true, RGB(120,120,120), PS_DASH, 0);
				m_pGraph->SetYMajGrid(true, RGB(120,120,120), PS_DASH, 0);
				m_pGraph->SetXMinGrid(false, true, RGB(70,70,70),PS_DASH,0,0.01);
				m_pGraph->SetYMinGrid(false, true, RGB(70,70,70),PS_DASH,0,0.01);

				//	Miarex wing settings
					m_WingPanels    = RGB(140,140,140);
//					m_WingSurfaces  = RGB(70,70,70);
					m_XCPColor      = RGB(100, 255, 100);
					m_XTopColor     = RGB(200, 0, 255);;
					m_XBotColor     = RGB(200, 0, 255);;
					m_DownwashColor = RGB(255, 100, 100);


				// XFLR5 foil settings
					m_crFoilColor     = RGB(220,220,220);
					m_crBLColor       = RGB(200,50,50);
					m_crPressureColor = RGB(50,200,50);

				break;
			}
		case 2: //Light
			{
				m_bBlackAndWhite = false;
				m_crWnd = RGB(255,255,255);
				m_WndTextColor = RGB(0,20,40);

				m_pGraph->SetBkColor(RGB(242, 242, 255));
				m_pGraph->SetBorderColor(RGB(50,50,50));
				m_pGraph->SetAxisColor(RGB(20,20,60));
				m_pGraph->SetTitleColor(RGB(20,20,60));
				m_pGraph->SetLabelColor(RGB(20,20,60));
				m_pGraph->SetLegendColor(RGB(20,20,60));

				m_pGraph->SetXMajGrid(true, RGB(120,120,120), PS_DASH, 0);
				m_pGraph->SetYMajGrid(true, RGB(120,120,120), PS_DASH, 0);
				m_pGraph->SetXMinGrid(false, true, RGB(150,150,150),PS_DASH,0,0.01);
				m_pGraph->SetYMinGrid(false, true, RGB(150,150,150),PS_DASH,0,0.01);

				//	Miarex wing settings
					m_WingPanels    = RGB(60,60,60);
//					m_WingSurfaces  = RGB(100,100,100);
					m_XCPColor      = RGB(100, 255, 100);
					m_XTopColor     = RGB(200, 0, 255);;
					m_XBotColor     = RGB(200, 0, 255);;
					m_DownwashColor = RGB(255, 100, 100);


				// XFLR5 foil settings
					m_crFoilColor     = RGB(60,60,60);
					m_crBLColor       = RGB(200,50,50);
					m_crPressureColor = RGB(50,150,50);

				break;
			}
	}
	m_ctrlWndClr.SetColor(m_crWnd);
	Invalidate();
}


