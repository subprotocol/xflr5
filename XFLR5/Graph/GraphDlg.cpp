/****************************************************************************

	Graph options dialog

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

// GraphDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../misc/LinePickerDlg.h"
#include "GraphDlg.h"
#include "Graph.h"
#include <math.h>


/////////////////////////////////////////////////////////////////////////////
// CGraphDlg dialog


CGraphDlg::CGraphDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGraphDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGraphDlg)
	//}}AFX_DATA_INIT
	m_pParent = pParent;
}

CGraphDlg::CGraphDlg(Graph *pGraph, CWnd *pParent/*=NULL*/)
{
	m_pParent = pParent;
	m_pGraph = pGraph;
}


void CGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraphDlg)
	DDX_Control(pDX, IDC_APPLY, m_ctrlApply);
	DDX_Control(pDX, IDOK, m_ctrlOK);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGraphDlg, CDialog)
	//{{AFX_MSG_MAP(CGraphDlg)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphDlg message handlers


bool CGraphDlg::GetAutoX(){
	return m_bAutoX;
}
bool CGraphDlg::GetAutoY(){
	return m_bAutoY;
}

COLORREF CGraphDlg::GetColor(){
	return m_crColor;
}

int CGraphDlg::GetStyle(){
	return m_nStyle;
}

int CGraphDlg::GetWidth(){
	return m_nWidth;
}


void CGraphDlg::OnOK() 
{
	OnApply();
	CDialog::OnOK();
}

void CGraphDlg::OnApply() 
{
	//	No way back...
	GetScalePage();
	GetGridPage();
	GetBackPage();
	GetFontPage();

	m_pGraph->SetAxisColor(m_crColor);
	m_pGraph->SetAxisWidth(m_nWidth);
	m_pGraph->SetAxisStyle(m_nStyle);

	m_pGraph->SetBorder(m_bBorder);
	m_pGraph->SetBorderStyle(m_BorderStyle);
	m_pGraph->SetBorderWidth(m_BorderWidth);
	m_pGraph->SetBorderColor(m_crBorder);
	m_pGraph->SetBkColor(m_crBack);

	m_pGraph->ResetXLimits();
	m_pGraph->ResetYLimits();
	if(m_bAutoX){
		m_pGraph->SetAutoX(true);
		m_pGraph->SetXScale();
	}
	else {
		m_pGraph->SetAutoX(false);
		m_pGraph->SetXMin(m_XMin);
		m_pGraph->SetXMax(m_XMax);
		if(m_XUnit<=0.0001f) m_XUnit = abs((m_XMax-m_XMin)/2.0);
		m_pGraph->SetXUnit(m_XUnit);
		m_pGraph->SetX0(m_X0);
	}
	if(m_bAutoY){
		m_pGraph->SetAutoY(true);
		m_pGraph->SetYScale();
	}
	else {
		m_pGraph->SetAutoY(false);
		m_pGraph->SetYMin(m_YMin);
		m_pGraph->SetYMax(m_YMax);
		if(m_YUnit<=0.0001f) m_YUnit = abs((m_YMax-m_YMin)/2.0);
		m_pGraph->SetYUnit(m_YUnit);
		m_pGraph->SetY0(m_Y0);
	}

	m_pGraph->SetXMajGrid(m_bXMGrid, m_XMClr, m_XMStyle, m_XMWidth);
	m_pGraph->SetYMajGrid(m_bYMGrid, m_YMClr, m_YMStyle, m_YMWidth);
	m_pGraph->SetXMinGrid(m_bXMinGrid, m_bXAutoMin, m_XMinClr, m_XMinStyle, m_XMinWidth, m_XMinorUnit);
	m_pGraph->SetYMinGrid(m_bYMinGrid, m_bYAutoMin, m_YMinClr, m_YMinStyle, m_YMinWidth, m_YMinorUnit);

	m_pGraph->SetInverted(m_bYInverted);
	m_pGraph->SetTitleLogFont(&m_TitleLogFont);
	m_pGraph->SetLabelLogFont(&m_LabelLogFont);
	m_pGraph->SetLegendLogFont(&m_LegendLogFont);

	m_pGraph->SetTitleColor(m_TitleFontColor);
	m_pGraph->SetLabelColor(m_LabelFontColor);
	m_pGraph->SetLegendColor(m_LegendFontColor);

	m_pParent->Invalidate();
	m_ctrlOK.SetFocus();
}


BOOL CGraphDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetScalePage();
	SetGridPage();
	SetBackPage();
	SetFontPage();

	int page = GetGraphDlgPage();
	if(page<m_GraphSheet.GetPageCount())	m_GraphSheet.SetActivePage(page);

	Invalidate(true);
	GetDlgItem(IDOK)->SetFocus();
	return FALSE;
	 // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


int CGraphDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_pGraph) return -1;
	m_XMin   = m_pGraph->GetXMin();
	m_XMax   = m_pGraph->GetXMax();
	m_XUnit  = m_pGraph->GetXUnit();
	m_X0     = m_pGraph->GetX0();
	m_YMin   = m_pGraph->GetYMin();
	m_YMax   = m_pGraph->GetYMax();
	m_YUnit  = m_pGraph->GetYUnit();
	m_Y0     = m_pGraph->GetY0();
	m_bAutoX = m_pGraph->GetAutoX();
	m_bAutoY = m_pGraph->GetAutoY();
	
	m_pGraph->GetTitleLogFont(&m_TitleLogFont);
	m_pGraph->GetLabelLogFont(&m_LabelLogFont);
	m_pGraph->GetLegendLogFont(&m_LegendLogFont);

	m_TitleFontColor = m_pGraph->GetTitleColor();
	m_LabelFontColor = m_pGraph->GetLabelColor();
	m_LegendFontColor = m_pGraph->GetLegendColor();

	
	m_GraphSheet.AddPage(&m_ScalePage);
	m_GraphSheet.AddPage(&m_GridPage);
	m_GraphSheet.AddPage(&m_FontPage);
	m_GraphSheet.AddPage(&m_BackPage);

	m_ScalePage.m_pParent = this;
	m_GridPage.m_pParent = this;
	m_FontPage.m_pParent = this;
	m_BackPage.m_pParent = this;

	m_GraphSheet.Create(this, WS_CHILD | WS_VISIBLE, 0);
	m_GraphSheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_GraphSheet.ModifyStyle(0, WS_TABSTOP );

	CTabCtrl *pTabCtrl = m_GraphSheet.GetTabControl();
	TCITEM tc;
	tc.mask = TCIF_TEXT;

	tc.pszText = "Scales";
	pTabCtrl->SetItem(0, &tc);
	tc.pszText = "Axis and Grids";
	pTabCtrl->SetItem(1, &tc);
	tc.pszText = "Fonts";
	pTabCtrl->SetItem(2, &tc);
	tc.pszText = "Background";
	pTabCtrl->SetItem(3, &tc);

	CRect rectClient;
	m_GraphSheet.GetWindowRect(rectClient);

	m_GraphSheet.SetWindowPos(NULL, 10, 10,
                              rectClient.Width(), rectClient.Height(),
							  SWP_NOZORDER | SWP_NOACTIVATE);

	return 0;
}


void CGraphDlg::SetBackPage()
{
	m_crBack   = m_pGraph->GetBackColor();
	m_crBorder = m_pGraph->GetBorderColor();
	m_bBorder  = m_pGraph->GetBorder();
	
	m_BackPage.m_bBorder     = m_bBorder;
	m_BackPage.m_BorderClr   = m_crBorder;
	m_BackPage.m_GraphClr    = m_crBack;
	m_BackPage.m_BorderStyle = m_pGraph->GetBorderStyle();
	m_BackPage.m_BorderWidth = m_pGraph->GetBorderWidth();
}


void CGraphDlg::SetGridPage()
{

	m_GridPage.m_pGraph = m_pGraph;

	m_crColor = m_pGraph->GetAxisColor();
	m_nStyle  = m_pGraph->GetAxisStyle();
	m_nWidth  = m_pGraph->GetAxisWidth();
	m_GridPage.SetAxisStyle(m_crColor, m_nStyle, m_nWidth);

	bool bstate;
	m_pGraph->GetXMajGrid (&bstate, &m_XMClr, &m_XMStyle, &m_XMWidth);
	m_GridPage.SetXMajGrid(bstate,m_XMClr,m_XMStyle,m_XMWidth);
	m_pGraph->GetYMajGrid (&bstate, &m_YMClr, &m_YMStyle, &m_YMWidth);
	m_GridPage.SetYMajGrid(bstate,m_YMClr,m_YMStyle,m_YMWidth);


	m_pGraph->GetXMinGrid (&bstate, &m_bXAutoMin, &m_XMinClr, &m_XMinStyle, &m_XMinWidth, &m_XMinorUnit);
	m_GridPage.SetXMinGrid(bstate,m_bXAutoMin, m_XMinClr,m_XMinStyle,m_XMinWidth,m_XMinorUnit);
	m_pGraph->GetYMinGrid (&bstate, &m_bYAutoMin, &m_YMinClr, &m_YMinStyle, &m_YMinWidth, &m_YMinorUnit);
	m_GridPage.SetYMinGrid(bstate,m_bYAutoMin, m_YMinClr,m_YMinStyle,m_YMinWidth,m_YMinorUnit);

}

void CGraphDlg::SetScalePage()
{
	m_ScalePage.m_pGraph = m_pGraph;
	m_bAutoX = m_pGraph->GetAutoX();
	m_ScalePage.m_bAutoX = m_bAutoX;
	m_bAutoY = m_pGraph->GetAutoY();
	m_ScalePage.m_bAutoY = m_bAutoY;
	m_bYInverted = m_pGraph->GetInverted();
	m_ScalePage.m_bYInverted = m_bYInverted;
	m_ScalePage.SetValues();
}


void CGraphDlg::SetFontPage()
{
	memcpy(&m_FontPage.m_TitleLogFont, &m_TitleLogFont, sizeof(LOGFONT));
	memcpy(&m_FontPage.m_LabelLogFont, &m_LabelLogFont, sizeof(LOGFONT));
	memcpy(&m_FontPage.m_LegendLogFont, &m_LegendLogFont, sizeof(LOGFONT));
	m_FontPage.m_TitleFontColor = m_TitleFontColor;
	m_FontPage.m_LabelFontColor = m_LabelFontColor;
	m_FontPage.m_LegendFontColor = m_LegendFontColor;
}



void CGraphDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	SetGraphDlgPage(m_GraphSheet.GetActiveIndex());
}


void CGraphDlg::GetScalePage()
{
	m_XMin   = m_ScalePage.m_ctrlXMin.GetValue();
	m_XMax   = m_ScalePage.m_ctrlXMax.GetValue();
	m_XUnit  = m_ScalePage.m_ctrlXUnit.GetValue();
	m_X0     = m_ScalePage.m_ctrlX0.GetValue();
	m_YMin   = m_ScalePage.m_ctrlYMin.GetValue();
	m_YMax   = m_ScalePage.m_ctrlYMax.GetValue();
	m_YUnit  = m_ScalePage.m_ctrlYUnit.GetValue();
	m_Y0     = m_ScalePage.m_ctrlY0.GetValue();
	m_bAutoX = m_ScalePage.m_bAutoX;
	m_bAutoY = m_ScalePage.m_bAutoY;
	m_bYInverted = m_ScalePage.m_bYInverted;

	double XRange = abs(m_XMax-m_XMin);
	m_XUnit = abs(m_XUnit);
	if(m_XUnit < XRange/50.0) m_XUnit = XRange/50.0;

	double YRange = abs(m_YMax-m_YMin);
	m_YUnit = abs(m_YUnit);
	if(m_YUnit < YRange/50.0) m_YUnit = YRange/50.0;
}

void CGraphDlg::GetBackPage()
{

	m_crBack = m_BackPage.m_GraphClr;
	m_crBorder = m_BackPage.m_BorderClr;
	m_bBorder = m_BackPage.m_bBorder;
	m_BorderWidth = m_BackPage.m_BorderWidth;
	m_BorderStyle = m_BackPage.m_BorderStyle;
}


void CGraphDlg::GetGridPage()
{
	m_XMinorUnit = m_GridPage.m_XMinorUnit;
	m_YMinorUnit = m_GridPage.m_YMinorUnit;

	if(m_XMinorUnit>m_XUnit) m_XMinorUnit = m_XUnit/2.f;// why not ?
	if(m_YMinorUnit>m_YUnit) m_YMinorUnit = m_YUnit/2.f;

	m_crColor = m_GridPage.m_crColor;
	m_nStyle  = m_GridPage.m_nStyle;
	m_nWidth  = m_GridPage.m_nWidth;
	
	m_XMClr   = m_GridPage.m_XMClr;
	m_XMStyle = m_GridPage.m_XMStyle;
	m_XMWidth = m_GridPage.m_XMWidth;
	if(m_GridPage.m_bXM)m_bXMGrid=true; else m_bXMGrid=false;
	m_bXAutoMin = m_GridPage.m_bAutoXMinUnit;

	m_YMClr   = m_GridPage.m_YMClr;
	m_YMStyle = m_GridPage.m_YMStyle;
	m_YMWidth = m_GridPage.m_YMWidth;
	if(m_GridPage.m_bYM)m_bYMGrid=true; else m_bYMGrid=false;
	m_bYAutoMin = m_GridPage.m_bAutoYMinUnit;

	m_XMinClr   = m_GridPage.m_XMinClr;
	m_XMinStyle = m_GridPage.m_XMinStyle;
	m_XMinWidth = m_GridPage.m_XMinWidth;
	m_XMinorUnit  = m_GridPage.m_XMinorUnit;
	if(m_GridPage.m_bXMin)m_bXMinGrid=true; else m_bXMinGrid=false;
	m_YMinClr   = m_GridPage.m_YMinClr;
	m_YMinStyle = m_GridPage.m_YMinStyle;
	m_YMinWidth = m_GridPage.m_YMinWidth;
	m_YMinorUnit  = m_GridPage.m_YMinorUnit;
	if(m_GridPage.m_bYMin)m_bYMinGrid=true; else m_bYMinGrid=false;
}

void CGraphDlg::GetFontPage()
{
	memcpy(&m_TitleLogFont, &m_FontPage.m_TitleLogFont, sizeof(LOGFONT));
	memcpy(&m_LabelLogFont, &m_FontPage.m_LabelLogFont, sizeof(LOGFONT));
	memcpy(&m_LegendLogFont, &m_FontPage.m_LegendLogFont, sizeof(LOGFONT));
	m_TitleFontColor = m_FontPage.m_TitleFontColor;
	m_LabelFontColor = m_FontPage.m_LabelFontColor;
	m_LegendFontColor = m_FontPage.m_LegendFontColor;	
}


BOOL CGraphDlg::PreTranslateMessage(MSG* pMsg) 
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)){
		CWnd* ppp = GetFocus();
		if (ppp == GetDlgItem(IDC_APPLY) )	
		{
			OnApply();
			m_ctrlOK.SetFocus();
			return true;
		}
		else if (ppp == &m_ctrlOK){
			//continue normally
			OnOK();
			return true;
		}
		else {
			m_ctrlApply.SetFocus();
			return true;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}



void CGraphDlg::OnClose() 
{
	CDialog::OnClose();
}
