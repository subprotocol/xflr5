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
		if(m_XUnit<=0.0001f) m_XUnit = (double)fabs((m_XMax-m_XMin)/2.f);
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
		if(m_YUnit<=0.0001f) m_YUnit = (double)fabs((m_YMax-m_YMin)/2.f);
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
	if(page<m_pGraphSheet->GetPageCount())	m_pGraphSheet->SetActivePage(page);

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

	m_pGraphSheet = new CPropertySheet;
	m_pScalePage  = new CScalePage(this);
	m_pGridPage   = new CGridPage(this);
	m_pFontPage   = new CFontPage(this);
	m_pBackPage   = new CBackPage(this);
	if (!m_pGraphSheet ||
		!m_pScalePage  ||
		!m_pGridPage   ||
		!m_pFontPage   || 
		!m_pScalePage){
		if(m_pGraphSheet) delete m_pGraphSheet;
		if(m_pScalePage) delete m_pScalePage;
		if(m_pBackPage)	delete m_pBackPage;
		if(m_pFontPage) delete m_pFontPage;
		if(m_pGridPage) delete m_pGridPage;
		return -1;
	}
	
	m_pGraphSheet->AddPage(m_pScalePage);
	m_pGraphSheet->AddPage(m_pGridPage);
	m_pGraphSheet->AddPage(m_pFontPage);
	m_pGraphSheet->AddPage(m_pBackPage);


	if(!m_pGraphSheet->Create(this, WS_CHILD | WS_VISIBLE | WS_TABSTOP,
							  WS_EX_CONTROLPARENT)){
		delete m_pGraphSheet;
		m_pGraphSheet = NULL;
		return -1;
	}

	CTabCtrl *pTabCtrl = m_pGraphSheet->GetTabControl();
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
	m_pGraphSheet->GetWindowRect(rectClient);

	m_pGraphSheet->SetWindowPos(NULL, 10, 10,
		rectClient.Width(), rectClient.Height(),
		SWP_NOZORDER | SWP_NOACTIVATE);

	return 0;
}


void CGraphDlg::SetBackPage()
{
	m_crBack   = m_pGraph->GetBackColor();
	m_crBorder = m_pGraph->GetBorderColor();
	m_bBorder  = m_pGraph->GetBorder();
	
	m_pBackPage->m_bBorder     = m_bBorder;
	m_pBackPage->m_BorderClr   = m_crBorder;
	m_pBackPage->m_GraphClr    = m_crBack;
	m_pBackPage->m_BorderStyle = m_pGraph->GetBorderStyle();
	m_pBackPage->m_BorderWidth = m_pGraph->GetBorderWidth();
}


void CGraphDlg::SetGridPage()
{

	m_pGridPage->m_pGraph = m_pGraph;

	m_crColor = m_pGraph->GetAxisColor();
	m_nStyle  = m_pGraph->GetAxisStyle();
	m_nWidth  = m_pGraph->GetAxisWidth();
	m_pGridPage->SetAxisStyle(m_crColor, m_nStyle, m_nWidth);

	bool bstate;
	m_pGraph->GetXMajGrid (&bstate, &m_XMClr, &m_XMStyle, &m_XMWidth);
	m_pGridPage->SetXMajGrid(bstate,m_XMClr,m_XMStyle,m_XMWidth);
	m_pGraph->GetYMajGrid (&bstate, &m_YMClr, &m_YMStyle, &m_YMWidth);
	m_pGridPage->SetYMajGrid(bstate,m_YMClr,m_YMStyle,m_YMWidth);


	m_pGraph->GetXMinGrid (&bstate, &m_bXAutoMin, &m_XMinClr, &m_XMinStyle, &m_XMinWidth, &m_XMinorUnit);
	m_pGridPage->SetXMinGrid(bstate,m_bXAutoMin, m_XMinClr,m_XMinStyle,m_XMinWidth,m_XMinorUnit);
	m_pGraph->GetYMinGrid (&bstate, &m_bYAutoMin, &m_YMinClr, &m_YMinStyle, &m_YMinWidth, &m_YMinorUnit);
	m_pGridPage->SetYMinGrid(bstate,m_bYAutoMin, m_YMinClr,m_YMinStyle,m_YMinWidth,m_YMinorUnit);

}

void CGraphDlg::SetScalePage()
{
	m_pScalePage->m_pGraph = m_pGraph;
	m_bAutoX = m_pGraph->GetAutoX();
	m_pScalePage->m_bAutoX = m_bAutoX;
	m_bAutoY = m_pGraph->GetAutoY();
	m_pScalePage->m_bAutoY = m_bAutoY;
	m_bYInverted = m_pGraph->GetInverted();
	m_pScalePage->m_bYInverted = m_bYInverted;
	m_pScalePage->SetValues();
}


void CGraphDlg::SetFontPage()
{
	memcpy(&m_pFontPage->m_TitleLogFont, &m_TitleLogFont, sizeof(LOGFONT));
	memcpy(&m_pFontPage->m_LabelLogFont, &m_LabelLogFont, sizeof(LOGFONT));
	memcpy(&m_pFontPage->m_LegendLogFont, &m_LegendLogFont, sizeof(LOGFONT));
	m_pFontPage->m_TitleFontColor = m_TitleFontColor;
	m_pFontPage->m_LabelFontColor = m_LabelFontColor;
	m_pFontPage->m_LegendFontColor = m_LegendFontColor;
}



void CGraphDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	SetGraphDlgPage(m_pGraphSheet->GetActiveIndex());
 	delete m_pGraphSheet;
	delete m_pScalePage;
	delete m_pFontPage;
	delete m_pGridPage;
	delete m_pBackPage;
}


void CGraphDlg::GetScalePage()
{
	m_XMin   = m_pScalePage->m_ctrlXMin.GetValue();
	m_XMax   = m_pScalePage->m_ctrlXMax.GetValue();
	m_XUnit  = m_pScalePage->m_ctrlXUnit.GetValue();
	m_X0     = m_pScalePage->m_ctrlX0.GetValue();
	m_YMin   = m_pScalePage->m_ctrlYMin.GetValue();
	m_YMax   = m_pScalePage->m_ctrlYMax.GetValue();
	m_YUnit  = m_pScalePage->m_ctrlYUnit.GetValue();
	m_Y0     = m_pScalePage->m_ctrlY0.GetValue();
	m_bAutoX = m_pScalePage->m_bAutoX;
	m_bAutoY = m_pScalePage->m_bAutoY;
	m_bYInverted = m_pScalePage->m_bYInverted;

	double XRange = (double)fabs(m_XMax-m_XMin);
	m_XUnit = (double) fabs(m_XUnit);
	if(m_XUnit < XRange/50.f) m_XUnit = XRange/50.f;

	double YRange = (double)fabs(m_YMax-m_YMin);
	m_YUnit = (double) fabs(m_YUnit);
	if(m_YUnit < YRange/50.f) m_YUnit = YRange/50.f;
}

void CGraphDlg::GetBackPage()
{

	m_crBack = m_pBackPage->m_GraphClr;
	m_crBorder = m_pBackPage->m_BorderClr;
	m_bBorder = m_pBackPage->m_bBorder;
	m_BorderWidth = m_pBackPage->m_BorderWidth;
	m_BorderStyle = m_pBackPage->m_BorderStyle;
}


void CGraphDlg::GetGridPage()
{
	m_XMinorUnit = m_pGridPage->m_XMinorUnit;
	m_YMinorUnit = m_pGridPage->m_YMinorUnit;

	if(m_XMinorUnit>m_XUnit) m_XMinorUnit = m_XUnit/2.f;// why not ?
	if(m_YMinorUnit>m_YUnit) m_YMinorUnit = m_YUnit/2.f;

	m_crColor = m_pGridPage->m_crColor;
	m_nStyle  = m_pGridPage->m_nStyle;
	m_nWidth  = m_pGridPage->m_nWidth;
	
	m_XMClr   = m_pGridPage->m_XMClr;
	m_XMStyle = m_pGridPage->m_XMStyle;
	m_XMWidth = m_pGridPage->m_XMWidth;
	if(m_pGridPage->m_bXM)m_bXMGrid=true; else m_bXMGrid=false;
	m_bXAutoMin = m_pGridPage->m_bAutoXMinUnit;

	m_YMClr   = m_pGridPage->m_YMClr;
	m_YMStyle = m_pGridPage->m_YMStyle;
	m_YMWidth = m_pGridPage->m_YMWidth;
	if(m_pGridPage->m_bYM)m_bYMGrid=true; else m_bYMGrid=false;
	m_bYAutoMin = m_pGridPage->m_bAutoYMinUnit;

	m_XMinClr   = m_pGridPage->m_XMinClr;
	m_XMinStyle = m_pGridPage->m_XMinStyle;
	m_XMinWidth = m_pGridPage->m_XMinWidth;
	m_XMinorUnit  = m_pGridPage->m_XMinorUnit;
	if(m_pGridPage->m_bXMin)m_bXMinGrid=true; else m_bXMinGrid=false;
	m_YMinClr   = m_pGridPage->m_YMinClr;
	m_YMinStyle = m_pGridPage->m_YMinStyle;
	m_YMinWidth = m_pGridPage->m_YMinWidth;
	m_YMinorUnit  = m_pGridPage->m_YMinorUnit;
	if(m_pGridPage->m_bYMin)m_bYMinGrid=true; else m_bYMinGrid=false;
}

void CGraphDlg::GetFontPage()
{
	memcpy(&m_TitleLogFont, &m_pFontPage->m_TitleLogFont, sizeof(LOGFONT));
	memcpy(&m_LabelLogFont, &m_pFontPage->m_LabelLogFont, sizeof(LOGFONT));
	memcpy(&m_LegendLogFont, &m_pFontPage->m_LegendLogFont, sizeof(LOGFONT));
	m_TitleFontColor = m_pFontPage->m_TitleFontColor;
	m_LabelFontColor = m_pFontPage->m_LabelFontColor;
	m_LegendFontColor = m_pFontPage->m_LegendFontColor;	
}


BOOL CGraphDlg::PreTranslateMessage(MSG* pMsg) 
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)){
		CWnd* ppp = GetFocus();
		if (ppp == GetDlgItem(IDC_APPLY) )	{
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
