/****************************************************************************

    Graph Options class
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

//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../XDirect/XDirect.h"
#include "GraphOptions.h"



/////////////////////////////////////////////////////////////////////////////
// CGraphOptions dialog


CGraphOptions::CGraphOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CGraphOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGraphOptions)
	m_bXMaj = FALSE;
	m_bYMaj = FALSE;
	m_bXMin = FALSE;
	m_bYMin = FALSE;
	//}}AFX_DATA_INIT
	m_pParent= pParent;

	m_bAuto = true;

	m_ctrlAMax.SetPrecision(4);
	m_ctrlAMin.SetPrecision(4);
	m_ctrlAOrigin.SetPrecision(4);
	m_ctrlAUnit.SetPrecision(4);

	m_ctrlMMax.SetPrecision(4);
	m_ctrlMMin.SetPrecision(4);
	m_ctrlMOrigin.SetPrecision(4);
	m_ctrlMUnit.SetPrecision(4);

	m_ctrlLMax.SetPrecision(4);
	m_ctrlLMin.SetPrecision(4);
	m_ctrlLOrigin.SetPrecision(4);
	m_ctrlLUnit.SetPrecision(4);

	m_ctrlDMax.SetPrecision(4);
	m_ctrlDMin.SetPrecision(4);
	m_ctrlDOrigin.SetPrecision(4);
	m_ctrlDUnit.SetPrecision(4);
}


void CGraphOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraphOptions)
	DDX_Control(pDX, IDC_YMINSTYLE, m_ctrlYMin);
	DDX_Control(pDX, IDC_XMINSTYLE, m_ctrlXMin);
	DDX_Control(pDX, IDC_YMAINSTYLE, m_ctrlYMain);
	DDX_Control(pDX, IDC_XMAINSTYLE, m_ctrlXMain);
	DDX_Control(pDX, IDC_AXISSTYLE, m_ctrlAxis);
	DDX_Control(pDX, IDC_MORIGIN, m_ctrlMOrigin);
	DDX_Control(pDX, IDC_MMIN, m_ctrlMMin);
	DDX_Control(pDX, IDC_MMAX, m_ctrlMMax);
	DDX_Control(pDX, IDC_MUNIT, m_ctrlMUnit);
	DDX_Control(pDX, IDC_LORIGIN, m_ctrlLOrigin);
	DDX_Control(pDX, IDC_LMIN, m_ctrlLMin);
	DDX_Control(pDX, IDC_LMAX, m_ctrlLMax);
	DDX_Control(pDX, IDC_LUNIT, m_ctrlLUnit);
	DDX_Control(pDX, IDC_DORIGIN, m_ctrlDOrigin);
	DDX_Control(pDX, IDC_DMIN, m_ctrlDMin);
	DDX_Control(pDX, IDC_DMAX, m_ctrlDMax);
	DDX_Control(pDX, IDC_DUNIT, m_ctrlDUnit);
	DDX_Control(pDX, IDC_AORIGIN, m_ctrlAOrigin);
	DDX_Control(pDX, IDC_AMIN, m_ctrlAMin);
	DDX_Control(pDX, IDC_AMAX, m_ctrlAMax);
	DDX_Control(pDX, IDC_AUNIT, m_ctrlAUnit);
	DDX_Control(pDX, IDC_AUTOGRAPH, m_ctrlAuto);
	DDX_Check(pDX, IDC_XMAJCHECK, m_bXMaj);
	DDX_Check(pDX, IDC_YMAJCHECK, m_bYMaj);
	DDX_Check(pDX, IDC_XMINCHECK, m_bXMin);
	DDX_Check(pDX, IDC_YMINCHECK, m_bYMin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGraphOptions, CDialog)
	//{{AFX_MSG_MAP(CGraphOptions)
	ON_BN_CLICKED(IDC_AUTOGRAPH, OnAutoGraph)
	ON_BN_CLICKED(IDC_TITLEFONT, OnTitleFont)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_LABELFONT, OnLabelFont)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_XMAINSTYLE, OnXMainStyle)
	ON_BN_CLICKED(IDC_YMAINSTYLE, OnYMainStyle)
	ON_BN_CLICKED(IDC_AXISSTYLE, OnAxisStyle)
	ON_BN_CLICKED(IDC_XMINSTYLE, OnXMinStyle)
	ON_BN_CLICKED(IDC_YMINSTYLE, OnYMinStyle)
	ON_BN_CLICKED(IDC_LEGENDFONT, OnLegendFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphOptions message handlers

void CGraphOptions::OnAutoGraph() 
{
	if(m_ctrlAuto.GetCheck()) m_bAuto = true;
	else m_bAuto = false;
	m_ctrlMOrigin.EnableWindow(!m_bAuto);
	m_ctrlMMin.EnableWindow(!m_bAuto);
	m_ctrlMMax.EnableWindow(!m_bAuto);
	m_ctrlMUnit.EnableWindow(!m_bAuto);
	m_ctrlLOrigin.EnableWindow(!m_bAuto);
	m_ctrlLMin.EnableWindow(!m_bAuto);
	m_ctrlLMax.EnableWindow(!m_bAuto);
	m_ctrlLUnit.EnableWindow(!m_bAuto);
	m_ctrlDOrigin.EnableWindow(!m_bAuto);
	m_ctrlDMin.EnableWindow(!m_bAuto);
	m_ctrlDMax.EnableWindow(!m_bAuto);
	m_ctrlDUnit.EnableWindow(!m_bAuto);
	m_ctrlAOrigin.EnableWindow(!m_bAuto);
	m_ctrlAMin.EnableWindow(!m_bAuto);
	m_ctrlAMax.EnableWindow(!m_bAuto);
	m_ctrlAUnit.EnableWindow(!m_bAuto);
}

void CGraphOptions::OnOK() {
	// TODO: Add extra validation here
	AMax    = m_ctrlAMax.GetValue();
	AMin    = m_ctrlAMin.GetValue();
	AOrigin = m_ctrlAOrigin.GetValue();
	AUnit   = m_ctrlAUnit.GetValue();
	MMax    = m_ctrlMMax.GetValue();
	MMin    = m_ctrlMMin.GetValue();
	MOrigin = m_ctrlMOrigin.GetValue();
	MUnit   = m_ctrlMUnit.GetValue();
	LMax    = m_ctrlLMax.GetValue();
	LMin    = m_ctrlLMin.GetValue();
	LOrigin = m_ctrlLOrigin.GetValue();
	LUnit   = m_ctrlLUnit.GetValue();
	DMax    = m_ctrlDMax.GetValue();
	DMin    = m_ctrlDMin.GetValue();
	DOrigin = m_ctrlDOrigin.GetValue();
	DUnit   = m_ctrlDUnit.GetValue();

	if(m_ctrlAuto.GetCheck()) m_bAuto = true;
	else m_bAuto = false;

	OnApply();
	CDialog::OnOK();
}

BOOL CGraphOptions::OnInitDialog() {
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	m_ctrlAMax.SetValue(AMax);
	m_ctrlAMin.SetValue(AMin);
	m_ctrlAOrigin.SetValue(AOrigin);
	m_ctrlAUnit.SetValue(AUnit);

	m_ctrlMMax.SetValue(MMax);
	m_ctrlMMin.SetValue(MMin);
	m_ctrlMOrigin.SetValue(MOrigin);
	m_ctrlMUnit.SetValue(MUnit);

	m_ctrlLMax.SetValue(LMax);
	m_ctrlLMin.SetValue(LMin);
	m_ctrlLOrigin.SetValue(LOrigin);
	m_ctrlLUnit.SetValue(LUnit);

	m_ctrlDMax.SetValue(DMax);
	m_ctrlDMin.SetValue(DMin);
	m_ctrlDOrigin.SetValue(DOrigin);
	m_ctrlDUnit.SetValue(DUnit);	

	if(m_bAuto) m_ctrlAuto.SetCheck(1);
	else m_ctrlAuto.SetCheck(0);
	OnAutoGraph();

	Invalidate(true);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CGraphOptions::SetTitleFontColor(COLORREF crColor){
	m_TitleFontColor = crColor;
}

void CGraphOptions::SetLabelFontColor(COLORREF crColor){
	m_LabelFontColor = crColor;
}
void CGraphOptions::SetLegendFontColor(COLORREF crColor){
	m_LabelFontColor = crColor;
}

COLORREF CGraphOptions::GetTitleFontColor(){
	return m_TitleFontColor;
}

COLORREF CGraphOptions::GetLabelFontColor(){
	return m_LabelFontColor;
}
COLORREF CGraphOptions::GetLegendFontColor(){
	return m_LabelFontColor;
}

void CGraphOptions::SetTitleLogFont(LOGFONT *plgft){
	memcpy(&m_TitleLogFont, plgft, sizeof(LOGFONT));
}

void CGraphOptions::SetLabelLogFont(LOGFONT *plgft){
	memcpy(&m_LabelLogFont, plgft, sizeof(LOGFONT));
}
void CGraphOptions::SetLegendLogFont(LOGFONT *plgft){
	memcpy(&m_LegendLogFont, plgft, sizeof(LOGFONT));
}

void CGraphOptions::GetTitleLogFont(LOGFONT *plgft){
	memcpy(plgft, &m_TitleLogFont, sizeof(LOGFONT));
}

void CGraphOptions::GetLabelLogFont(LOGFONT *plgft){
	memcpy(plgft, &m_LabelLogFont, sizeof(LOGFONT));
}
void CGraphOptions::GetLegendLogFont(LOGFONT *plgft){
	memcpy(plgft, &m_LabelLogFont, sizeof(LOGFONT));
}

void CGraphOptions::OnApply() 
{
	CXDirect* pXDirect = (CXDirect*)m_pParent;
	UpdateData(true);
	LOGFONT lgft;
	GetTitleLogFont(&lgft);
	pXDirect->m_pPolarGraph->SetTitleLogFont(&lgft);
	pXDirect->m_pPolarGraph->SetTitleColor(GetTitleFontColor());
	pXDirect->m_pCmGraph->SetTitleLogFont(&lgft);
	pXDirect->m_pCmGraph->SetTitleColor(GetTitleFontColor());
	pXDirect->m_pCzGraph->SetTitleLogFont(&lgft);
	pXDirect->m_pCzGraph->SetTitleColor(GetTitleFontColor());
	pXDirect->m_pTrGraph->SetTitleLogFont(&lgft);
	pXDirect->m_pTrGraph->SetTitleColor(GetTitleFontColor());
	pXDirect->m_pUserGraph->SetTitleLogFont(&lgft);
	pXDirect->m_pUserGraph->SetTitleColor(GetTitleFontColor());

	GetLabelLogFont(&lgft);
	pXDirect->m_pPolarGraph->SetLabelLogFont(&lgft);
	pXDirect->m_pPolarGraph->SetLabelColor(GetLabelFontColor());
	pXDirect->m_pCmGraph->SetLabelLogFont(&lgft);
	pXDirect->m_pCmGraph->SetLabelColor(GetLabelFontColor());
	pXDirect->m_pCzGraph->SetLabelLogFont(&lgft);
	pXDirect->m_pCzGraph->SetLabelColor(GetLabelFontColor());
	pXDirect->m_pTrGraph->SetLabelLogFont(&lgft);
	pXDirect->m_pTrGraph->SetLabelColor(GetLabelFontColor());
	pXDirect->m_pUserGraph->SetLabelLogFont(&lgft);
	pXDirect->m_pUserGraph->SetLabelColor(GetLabelFontColor());
	

	pXDirect->m_pPolarGraph->SetAxisData(m_nStyle,m_nWidth,m_crColor);
	pXDirect->m_pCmGraph->SetAxisData(m_nStyle,m_nWidth,m_crColor);
	pXDirect->m_pCzGraph->SetAxisData(m_nStyle,m_nWidth,m_crColor);
	pXDirect->m_pTrGraph->SetAxisData(m_nStyle,m_nWidth,m_crColor);
	pXDirect->m_pUserGraph->SetAxisData(m_nStyle,m_nWidth,m_crColor);
	
	bool bx, by;
	if (m_bXMaj) bx =true ; else bx = false;
	if (m_bYMaj) by =true ; else by = false;

	pXDirect->m_pPolarGraph->SetXMajGrid(bx, m_XMClr, m_XMStyle, m_XMWidth); 
	pXDirect->m_pPolarGraph->SetYMajGrid(by, m_YMClr, m_YMStyle, m_YMWidth); 
	pXDirect->m_pCmGraph->SetXMajGrid(bx, m_XMClr, m_XMStyle, m_XMWidth); 
	pXDirect->m_pCmGraph->SetYMajGrid(by, m_YMClr, m_YMStyle, m_YMWidth); 
	pXDirect->m_pCzGraph->SetXMajGrid(bx, m_XMClr, m_XMStyle, m_XMWidth); 
	pXDirect->m_pCzGraph->SetYMajGrid(by, m_YMClr, m_YMStyle, m_YMWidth); 
	pXDirect->m_pTrGraph->SetXMajGrid(bx, m_XMClr, m_XMStyle, m_XMWidth); 
	pXDirect->m_pTrGraph->SetYMajGrid(by, m_YMClr, m_YMStyle, m_YMWidth); 
	pXDirect->m_pUserGraph->SetXMajGrid(bx, m_XMClr, m_XMStyle, m_XMWidth); 
	pXDirect->m_pUserGraph->SetYMajGrid(by, m_YMClr, m_YMStyle, m_YMWidth); 

	if (m_bXMin) bx =true ; else bx = false;
	if (m_bYMin) by =true ; else by = false;

	pXDirect->m_pPolarGraph->SetXMinGrid(bx, true, m_XMinClr, m_XMinStyle, m_XMinWidth); 
	pXDirect->m_pPolarGraph->SetYMinGrid(by, true, m_YMinClr, m_YMinStyle, m_YMinWidth); 
	pXDirect->m_pCmGraph->SetXMinGrid(bx, true, m_XMinClr, m_XMStyle, m_XMinWidth); 
	pXDirect->m_pCmGraph->SetYMinGrid(by, true, m_YMinClr, m_YMStyle, m_YMinWidth); 
	pXDirect->m_pCzGraph->SetXMinGrid(bx, true, m_XMinClr, m_XMStyle, m_XMinWidth); 
	pXDirect->m_pCzGraph->SetYMinGrid(by, true, m_YMinClr, m_YMStyle, m_YMinWidth); 
	pXDirect->m_pTrGraph->SetXMinGrid(bx, true, m_XMinClr, m_XMStyle, m_XMinWidth); 
	pXDirect->m_pTrGraph->SetYMinGrid(by, true, m_YMinClr, m_YMStyle, m_YMinWidth); 
	pXDirect->m_pUserGraph->SetXMinGrid(bx, true, m_XMinClr, m_XMinStyle, m_XMinWidth); 
	pXDirect->m_pUserGraph->SetYMinGrid(by, true, m_YMinClr, m_YMinStyle, m_YMinWidth); 

	if(m_ctrlAuto.GetCheck()) m_bAuto = true;
	else m_bAuto = false;
	pXDirect->m_pPolarGraph->SetAuto(m_bAuto);
	pXDirect->m_pCmGraph->SetAuto(m_bAuto);
	pXDirect->m_pCzGraph->SetAuto(m_bAuto);
	pXDirect->m_pTrGraph->SetAuto(m_bAuto);
	pXDirect->m_pUserGraph->SetAuto(m_bAuto);

	if(!m_bAuto)
	{
		AMax = m_ctrlAMax.GetValue();
		AMin = m_ctrlAMin.GetValue();
		AOrigin = m_ctrlAOrigin.GetValue();
		AUnit = m_ctrlAUnit.GetValue();
		if(AUnit<=0.0001) AUnit = abs((AMax-AMin)/2.0);

		MMax = m_ctrlMMax.GetValue();
		MMin = m_ctrlMMin.GetValue();
		MOrigin = m_ctrlMOrigin.GetValue();
		MUnit = m_ctrlMUnit.GetValue();
		if(MUnit<=0.0001) MUnit = abs((MMax-MMin)/2.0);

		LMax = m_ctrlLMax.GetValue();
		LMin = m_ctrlLMin.GetValue();
		LOrigin = m_ctrlLOrigin.GetValue();
		LUnit = m_ctrlLUnit.GetValue();
		if(LUnit<=0.0001) LUnit = abs((LMax-LMin)/2.0);

		DMax = m_ctrlDMax.GetValue();
		DMin = m_ctrlDMin.GetValue();
		DOrigin = m_ctrlDOrigin.GetValue();
		DUnit = m_ctrlDUnit.GetValue();
		if(DUnit<=0.0001) DUnit = abs((DMax-DMin)/2.0);


		pXDirect->m_pPolarGraph->SetX0(DOrigin);
		pXDirect->m_pPolarGraph->SetXMin(DMin);
		pXDirect->m_pPolarGraph->SetXMax(DMax);
		pXDirect->m_pPolarGraph->SetXUnit(DUnit);
		pXDirect->m_pPolarGraph->SetY0(LOrigin);
		pXDirect->m_pPolarGraph->SetYMin(LMin);
		pXDirect->m_pPolarGraph->SetYMax(LMax);
		pXDirect->m_pPolarGraph->SetYUnit(LUnit);
		
		pXDirect->m_pCmGraph->SetX0(AOrigin);
		pXDirect->m_pCmGraph->SetXMin(AMin);
		pXDirect->m_pCmGraph->SetXMax(AMax);
		pXDirect->m_pCmGraph->SetXUnit(AUnit);
		pXDirect->m_pCmGraph->SetY0(MOrigin);
		pXDirect->m_pCmGraph->SetYMin(MMin);
		pXDirect->m_pCmGraph->SetYMax(MMax);
		pXDirect->m_pCmGraph->SetYUnit(MUnit);
		
		pXDirect->m_pCzGraph->SetX0(AOrigin);
		pXDirect->m_pCzGraph->SetXMin(AMin);
		pXDirect->m_pCzGraph->SetXMax(AMax);
		pXDirect->m_pCzGraph->SetXUnit(AUnit);
		pXDirect->m_pCzGraph->SetY0(LOrigin);
		pXDirect->m_pCzGraph->SetYMin(LMin);
		pXDirect->m_pCzGraph->SetYMax(LMax);
		pXDirect->m_pCzGraph->SetYUnit(LUnit);
		
		pXDirect->m_pTrGraph->SetX0(0.0);
		pXDirect->m_pTrGraph->SetXMin(0.0);
		pXDirect->m_pTrGraph->SetXMax(1.f);
		pXDirect->m_pTrGraph->SetXUnit(0.5f);
		pXDirect->m_pTrGraph->SetY0(LOrigin);
		pXDirect->m_pTrGraph->SetYMin(LMin);
		pXDirect->m_pTrGraph->SetYMax(LMax);
		pXDirect->m_pTrGraph->SetYUnit(LUnit);
	}
	else 
	{
		pXDirect->m_pPolarGraph->SetXMin(0.0);
		pXDirect->m_pPolarGraph->SetXMax(0.0);
		pXDirect->m_pPolarGraph->SetYMin(0.0);
		pXDirect->m_pPolarGraph->SetYMax(0.0);

		pXDirect->m_pCmGraph->SetXMin(0.0);
		pXDirect->m_pCmGraph->SetXMax(0.0);
		pXDirect->m_pCmGraph->SetYMin(0.0);
		pXDirect->m_pCmGraph->SetYMax(0.0);

		pXDirect->m_pCzGraph->SetXMin(0.0);
		pXDirect->m_pCzGraph->SetXMax(0.0);
		pXDirect->m_pCzGraph->SetYMin(0.0);
		pXDirect->m_pCzGraph->SetYMax(0.0);

		pXDirect->m_pTrGraph->SetXMin(0.0);
		pXDirect->m_pTrGraph->SetXMax(0.0);
		pXDirect->m_pTrGraph->SetYMin(0.0);
		pXDirect->m_pTrGraph->SetYMax(0.0);

		pXDirect->m_pUserGraph->SetXMin(0.0);
		pXDirect->m_pUserGraph->SetXMax(0.0);
		pXDirect->m_pUserGraph->SetYMin(0.0);
		pXDirect->m_pUserGraph->SetYMax(0.0);
	}

	pXDirect->UpdateView();
}


void CGraphOptions::OnTitleFont() 
{
	CFontDialog FtDlg(&m_TitleLogFont);
	FtDlg.m_cf.rgbColors = m_TitleFontColor;
	if(IDOK==FtDlg.DoModal()){
		FtDlg.GetCurrentFont(&m_TitleLogFont);
		m_TitleFontColor = FtDlg.GetColor();
	}	
}


void CGraphOptions::OnLabelFont() 
{
	CFontDialog FtDlg(&m_LabelLogFont);
	FtDlg.m_cf.rgbColors = m_LabelFontColor;
	if(IDOK==FtDlg.DoModal()){
		FtDlg.GetCurrentFont(&m_LabelLogFont);
		m_LabelFontColor = FtDlg.GetColor();
	}	
}

void CGraphOptions::OnLegendFont() 
{
	CFontDialog FtDlg(&m_LegendLogFont);
	FtDlg.m_cf.rgbColors = m_LegendFontColor;
	if(IDOK==FtDlg.DoModal()){
		FtDlg.GetCurrentFont(&m_LegendLogFont);
		m_LegendFontColor = FtDlg.GetColor();
	}	
}


void CGraphOptions::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	if (!pDC) return;
	int nSavedDC = pDC->SaveDC();
	if(nIDCtl==IDC_AXISSTYLE){
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_AXISSTYLE)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor = m_crColor;
		CPen CurvePen(PS_GEOMETRIC | m_nStyle, GetPenWidth(m_nWidth,false), &lb);

		pDC->SelectObject(&CurvePen);
		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
	else if(nIDCtl==IDC_XMAINSTYLE){
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_XMAINSTYLE)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor = m_XMClr;
		CPen CurvePen(PS_GEOMETRIC | m_XMStyle, GetPenWidth(m_XMWidth,false), &lb);

		pDC->SelectObject(&CurvePen);
		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
	else if(nIDCtl==IDC_YMAINSTYLE){
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_YMAINSTYLE)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor = m_YMClr;
		CPen CurvePen(PS_GEOMETRIC | m_YMStyle, GetPenWidth(m_YMWidth,false), &lb);

		pDC->SelectObject(&CurvePen);
		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
	else if(nIDCtl==IDC_XMINSTYLE){
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
		lb.lbColor = m_XMinClr;
		CPen CurvePen(PS_GEOMETRIC | m_XMinStyle, GetPenWidth(m_XMinWidth,false), &lb);

		pDC->SelectObject(&CurvePen);
		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
	else if(nIDCtl==IDC_YMINSTYLE){
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
		lb.lbColor = m_YMinClr;
		CPen CurvePen(PS_GEOMETRIC | m_YMinStyle, GetPenWidth(m_YMinWidth,false), &lb);

		pDC->SelectObject(&CurvePen);
		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
	pDC->RestoreDC(nSavedDC);

//	CDialog::OnDrawItem(nIDCt, lpDrawItemStruct);
}


void CGraphOptions::SetAxisData(int Style, int Width, COLORREF Clr){
	m_nStyle  = Style;
	m_nWidth  = Width;
	m_crColor = Clr;
}



void CGraphOptions::OnAxisStyle() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_crColor);
	LPdlg.SetStyle(m_nStyle);
	LPdlg.SetWidth(m_nWidth);

	if (IDOK==LPdlg.DoModal()){
		m_crColor = LPdlg.GetColor();
		m_nStyle  = LPdlg.GetStyle();
		m_nWidth  = LPdlg.GetWidth();
	}

	m_ctrlAxis.Invalidate(TRUE);	
}

void CGraphOptions::SetXMajData(bool b, int s, int w, COLORREF clr){
	m_bXMaj = b;
	m_XMStyle = s;
	m_XMWidth = w;
	m_XMClr   = clr;
}

void CGraphOptions::SetYMajData(bool b, int s, int w, COLORREF clr){
	m_bYMaj = b;
	m_YMStyle = s;
	m_YMWidth = w;
	m_YMClr   = clr;
}

void CGraphOptions::SetXMinData(bool b, int s, int w, COLORREF clr, double f){
	m_bXMin = b;
	m_XMinStyle = s;
	m_XMinWidth = w;
	m_XMinClr   = clr;
	m_XMinorUnit = f;
}

void CGraphOptions::SetYMinData(bool b, int s, int w, COLORREF clr, double f){
	m_bYMin = b;
	m_YMinStyle = s;
	m_YMinWidth = w;
	m_YMinClr   = clr;
	m_YMinorUnit = f;
}

void CGraphOptions::OnXMainStyle() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_XMClr);
	LPdlg.SetStyle(m_XMStyle);
	LPdlg.SetWidth(m_XMWidth);

	if (IDOK==LPdlg.DoModal()){
		m_XMClr = LPdlg.GetColor();
		m_XMStyle  = LPdlg.GetStyle();
		m_XMWidth  = LPdlg.GetWidth();
	}

	m_ctrlXMain.Invalidate(TRUE);	
}

void CGraphOptions::OnYMainStyle() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_YMClr);
	LPdlg.SetStyle(m_YMStyle);
	LPdlg.SetWidth(m_YMWidth);

	if (IDOK==LPdlg.DoModal()){
		m_YMClr = LPdlg.GetColor();
		m_YMStyle  = LPdlg.GetStyle();
		m_YMWidth  = LPdlg.GetWidth();
	}

	m_ctrlYMain.Invalidate(TRUE);
}

void CGraphOptions::OnXMinStyle() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_XMinClr);
	LPdlg.SetStyle(m_XMinStyle);
	LPdlg.SetWidth(m_XMinWidth);

	if (IDOK==LPdlg.DoModal()){
		m_XMinClr = LPdlg.GetColor();
		m_XMinStyle  = LPdlg.GetStyle();
		m_XMinWidth  = LPdlg.GetWidth();
	}

	m_ctrlXMin.Invalidate(TRUE);
	
}

void CGraphOptions::OnYMinStyle() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_YMinClr);
	LPdlg.SetStyle(m_YMinStyle);
	LPdlg.SetWidth(m_YMinWidth);

	if (IDOK==LPdlg.DoModal()){
		m_YMinClr = LPdlg.GetColor();
		m_YMinStyle  = LPdlg.GetStyle();
		m_YMinWidth  = LPdlg.GetWidth();
	}

	m_ctrlYMin.Invalidate(TRUE);
	
}

