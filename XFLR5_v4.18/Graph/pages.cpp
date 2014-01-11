/****************************************************************************

	Pages options
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


// BackPage.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../misc/LinePickerDlg.h"
#include "Pages.h"
#include "GraphDlg.h"
#include <math.h>


/////////////////////////////////////////////////////////////////////////////
// CBackPage property page

IMPLEMENT_DYNCREATE(CBackPage, CPropertyPage)

CBackPage::CBackPage() : CPropertyPage(CBackPage::IDD){
	//{{AFX_DATA_INIT(CBackPage)
	//}}AFX_DATA_INIT
}
CBackPage::CBackPage(CWnd* pParent) : CPropertyPage(CBackPage::IDD){
	m_pParent = pParent;
}

CBackPage::~CBackPage(){
}

void CBackPage::DoDataExchange(CDataExchange* pDX){
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackPage)
	DDX_Control(pDX, IDC_BORDERSTYLE, m_ctrlBorderStyle);
	DDX_Control(pDX, IDC_GRAPHCLR, m_ctrlGraphClr);
	DDX_Control(pDX, IDC_GRAPHBORDER, m_ctrlGraphBorder);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBackPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBackPage)
	ON_BN_CLICKED(IDC_GRAPHBORDER, OnGraphBorder)
	ON_BN_CLICKED(IDC_GRAPHCLR, OnGraphClr)
	ON_BN_CLICKED(IDC_BORDERSTYLE, OnBorderStyle)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackPage message handlers


void CBackPage::OnGraphBorder() 
{
	if(m_ctrlGraphBorder.GetCheck()) {
		m_ctrlBorderStyle.EnableWindow(true);
		m_bBorder = true;
	}
	else {
		m_ctrlBorderStyle.EnableWindow(false);
		m_bBorder = false;
	}
}

void CBackPage::OnGraphClr() 
{
	m_GraphClr = m_ctrlGraphClr.GetColor();
}

BOOL CBackPage::OnInitDialog() {
	CPropertyPage::OnInitDialog();
	
	m_ctrlGraphClr.SetColor(m_GraphClr);
	m_ctrlGraphBorder.SetCheck(m_bBorder);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBackPage::OnBorderStyle() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_BorderClr);
	LPdlg.SetStyle(m_BorderStyle);
	LPdlg.SetWidth(m_BorderWidth);

	if (IDOK==LPdlg.DoModal()){
		m_BorderClr = LPdlg.GetColor();
		m_BorderStyle = LPdlg.GetStyle();
		m_BorderWidth = LPdlg.GetWidth();
	}
	m_ctrlBorderStyle.Invalidate();
}

BOOL CBackPage::PreTranslateMessage(MSG* pMsg) 
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)){
		CGraphDlg* pDlg = (CGraphDlg*)m_pParent;
		pDlg->m_ctrlApply.SetFocus();
	}
	
	return CPropertyPage::PreTranslateMessage(pMsg);
}



void CBackPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	if (!pDC) return;
	int nSavedDC = pDC->SaveDC();

	if(nIDCtl==IDC_BORDERSTYLE)
	{
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = m_BorderClr;
		CPen BorderPen(PS_GEOMETRIC | m_BorderStyle, 
					   GetPenWidth(m_BorderWidth,false), &lb);
		CPen *pOldPen =  pDC->SelectObject(&BorderPen);
		pDC->MoveTo(5,8);
		pDC->LineTo(145,8);
		pDC->SelectObject(pOldPen);
	}
	else if(nIDCtl==IDC_GRAPHCLR)
	{
		// Store this for convenience
		int top    = lpDrawItemStruct->rcItem.top;
		int left   = lpDrawItemStruct->rcItem.left;
		int bottom = lpDrawItemStruct->rcItem.bottom;
		int right  = lpDrawItemStruct->rcItem.right;
		
		CPen NullPen;
		NullPen.CreateStockObject(NULL_PEN);
		CBrush colorbrush(m_GraphClr);
		CPen BlackPen(PS_SOLID, 1, RGB(0,0,0));
		CPen WhitePen(PS_SOLID, 1, RGB(255,255,255));
		CPen GrayPen(PS_SOLID,1,RGB(128,128,128));
		CBrush BackBrush(GetSysColor(COLOR_3DFACE));
		
		CPen *pOldPen = pDC->SelectObject(&NullPen);
		CBrush *pOldBrush= pDC->SelectObject(&BackBrush);
		
		
		// Clear the background using the 3DFACE color.
		
		pDC->Rectangle(&lpDrawItemStruct->rcItem);
		
		// Draw the border
		
		if (!(lpDrawItemStruct->itemState & ODS_SELECTED)){
			// Button is up
			pDC->SelectObject(&BlackPen);
			pDC->MoveTo(left,bottom-1);
			pDC->LineTo(right-1,bottom-1);
			pDC->LineTo(right-1,top);
			
			pDC->SelectObject(&GrayPen);
			pDC->MoveTo(left+1,bottom-2);
			pDC->LineTo(right-2,bottom-2);
			pDC->LineTo(right-2,top+1);
			
			pDC->SelectObject(&WhitePen);
			pDC->LineTo(left+1,top+1);
			pDC->LineTo(left+1,bottom-2);	
		}
		else{
			// Button is down
			
			pDC->SelectObject(&GrayPen);
			pDC->MoveTo(left,bottom-1);
			pDC->LineTo(left,top);
			pDC->LineTo(right-1,top);
			
			pDC->SelectObject(&WhitePen);
			pDC->MoveTo(right-1,top-1);
			pDC->LineTo(right-1,bottom-1);
			pDC->LineTo(left+1,bottom-1);
			
			pDC->SelectObject(&BlackPen);
			pDC->MoveTo(left+1,bottom-2);
			pDC->LineTo(left+1,top+1);
			pDC->LineTo(right-2,top+1);
			
			// by moving this, we get the things inside the button
			// to draw themselves one pixel down and one to the right.
			// This completes the "pushed" effect

			left++;
			right++;
			bottom++;
			top++;		
		}
		
		// The division
		
		pDC->SelectObject(&WhitePen);
		pDC->MoveTo(right-10,top+4);
		pDC->LineTo(right-10,bottom-4);
		
		pDC->SelectObject(&GrayPen);
		pDC->MoveTo(right-11,top+4);
		pDC->LineTo(right-11,bottom-4);
		
		// The triangle

		if (lpDrawItemStruct->itemState & ODS_DISABLED)
			pDC->SelectObject(&GrayPen);
		else
			pDC->SelectObject(&BlackPen);
		
		pDC->MoveTo(right-4,(bottom/2)-1);
		pDC->LineTo(right-9,(bottom/2)-1);
		
		pDC->MoveTo(right-5,(bottom/2));
		pDC->LineTo(right-8,(bottom/2));
		
		if (lpDrawItemStruct->itemState & ODS_DISABLED) {
			pDC->SetPixel(right-4,(bottom/2)-1,RGB(255,255,255));
			pDC->SetPixel(right-5,(bottom/2),RGB(255,255,255));
			pDC->SetPixel(right-6,(bottom/2)+1,RGB(255,255,255));
		}
		else  {
			pDC->SetPixel(right-6,(bottom/2)+1,RGB(0,0,0));
		}
		
		if (!(lpDrawItemStruct->itemState & ODS_DISABLED)) {
			// The color rectangle, only if enabled
			pDC->SelectObject(&colorbrush);
			pDC->Rectangle(left+5,top+4,right-15,bottom-4);    
		}
		
		if (lpDrawItemStruct->itemState & ODS_FOCUS) {
			// Draw the focus
			//
			// It would have been nice just to draw a rectangle using a pen created
			// with the PS_ALTERNATE style, but this is not supported by WIN95
			
			int i;
			
			for (i=left+3;i<right-4;i+=2)  {
				pDC->SetPixel(i,top+3,RGB(0,0,0));
				pDC->SetPixel(i,bottom-4,RGB(0,0,0));
			}
			
			for (i=top+3;i<bottom-4;i+=2){
				pDC->SetPixel(left+3,i,RGB(0,0,0));
				pDC->SetPixel(right-4,i,RGB(0,0,0));
			}       
		}	
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		
	}
	pDC->RestoreDC(nSavedDC);

//	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

/////////////////////////////////////////////////////////////////////////////
// CFontPage property page

IMPLEMENT_DYNCREATE(CFontPage, CPropertyPage)

CFontPage::CFontPage() : CPropertyPage(CFontPage::IDD)
{
	//{{AFX_DATA_INIT(CFontPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
CFontPage::CFontPage(CWnd* pParent) : CPropertyPage(CFontPage::IDD)
{
	m_pParent = pParent;
}

CFontPage::~CFontPage()
{
}

void CFontPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFontPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFontPage, CPropertyPage)
	//{{AFX_MSG_MAP(CFontPage)
	ON_BN_CLICKED(IDC_LABELFONT, OnLabelFont)
	ON_BN_CLICKED(IDC_TITLEFONT, OnTitleFont)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_LEGENDFONT, OnLegendFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontPage message handlers
void CFontPage::OnTitleFont() 
{

	CFontDialog FtDlg(&m_TitleLogFont);
	FtDlg.m_cf.rgbColors = m_TitleFontColor;
	
	if(IDOK==FtDlg.DoModal()){
		FtDlg.GetCurrentFont(&m_TitleLogFont);
		m_TitleFontColor = FtDlg.GetColor();
	}
	Invalidate();
}

void CFontPage::OnLabelFont() 
{
	CFontDialog FtDlg(&m_LabelLogFont);
	FtDlg.m_cf.rgbColors = m_LabelFontColor;
	if(IDOK==FtDlg.DoModal()){
		FtDlg.GetCurrentFont(&m_LabelLogFont);
		m_LabelFontColor = FtDlg.GetColor();
	}	
	Invalidate();
}



void CFontPage::OnLegendFont() 
{
	CFontDialog FtDlg(&m_LegendLogFont);
	FtDlg.m_cf.rgbColors = m_LegendFontColor;
	if(IDOK==FtDlg.DoModal()){
		FtDlg.GetCurrentFont(&m_LegendLogFont);
		m_LegendFontColor = FtDlg.GetColor();
	}	
	Invalidate();
}


void CFontPage::OnPaint() {
	CPaintDC dc(this); // device context for painting
	
	dc.SetTextAlign(TA_LEFT);
	dc.SetBkMode(TRANSPARENT);

	CFont TitleFont, LabelFont, LegendFont;

	TitleFont.CreateFontIndirect(&m_TitleLogFont);
	CFont* pOldFont = dc.SelectObject(&TitleFont);
	dc.SetTextColor(m_TitleFontColor);
	dc.TextOut(200,40,"AaBbCcDdEe 12345");

	LabelFont.CreateFontIndirect(&m_LabelLogFont);
	dc.SelectObject(&LabelFont);
	dc.SetTextColor(m_LabelFontColor);
	dc.TextOut(200,80,"AaBbCcDdEe 12345");

	LegendFont.CreateFontIndirect(&m_LegendLogFont);
	dc.SelectObject(&LegendFont);
	dc.SetTextColor(m_LegendFontColor);
	dc.TextOut(200,115,"AaBbCcDdEe 12345");

	dc.SelectObject(pOldFont);
	// Do not call CPropertyPage::OnPaint() for painting messages
}

BOOL CFontPage::PreTranslateMessage(MSG* pMsg) 
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)){
		CGraphDlg* pDlg = (CGraphDlg*)m_pParent;
		pDlg->m_ctrlApply.SetFocus();
	}
	
	return CPropertyPage::PreTranslateMessage(pMsg);
}


/////////////////////////////////////////////////////////////////////////////
// CGridPage property page

IMPLEMENT_DYNCREATE(CGridPage, CPropertyPage)

CGridPage::CGridPage() : CPropertyPage(CGridPage::IDD)
{
	//{{AFX_DATA_INIT(CGridPage)
	m_bXM = FALSE;
	m_bXMin = FALSE;
	m_bYM = FALSE;
	m_bYMin = FALSE;
	//}}AFX_DATA_INIT
}
CGridPage::CGridPage(CWnd* pParent) : CPropertyPage(CGridPage::IDD)
{
	//{{AFX_DATA_INIT(CGridPage)
	m_bXM = FALSE;
	m_bXMin = FALSE;
	m_bYM = FALSE;
	m_bYMin = FALSE;
	//}}AFX_DATA_INIT
	m_pParent = pParent;
}

CGridPage::~CGridPage()
{
}

void CGridPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGridPage)
	DDX_Control(pDX, IDC_AXISSTYLE, m_ctrlAxis);
	DDX_Control(pDX, IDC_YMINSTYLE, m_ctrlYMinStyle);
	DDX_Control(pDX, IDC_YMINCHECK, m_ctrlYMinCheck);
	DDX_Control(pDX, IDC_YMAJSTYLE, m_ctrlYMajStyle);
	DDX_Control(pDX, IDC_YMAJCHECK, m_ctrlYMajCheck);
	DDX_Control(pDX, IDC_XMINSTYLE, m_ctrlXMinStyle);
	DDX_Control(pDX, IDC_XMINCHECK, m_ctrlXMinCheck);
	DDX_Control(pDX, IDC_XMAJSTYLE, m_ctrlXMajStyle);
	DDX_Control(pDX, IDC_XMAJCHECK, m_ctrlXMajCheck);
	DDX_Control(pDX, IDC_YMINORUNIT, m_ctrlYMinorUnit);
	DDX_Control(pDX, IDC_XMINORUNIT, m_ctrlXMinorUnit);
	DDX_Check(pDX, IDC_XMAJCHECK, m_bXM);
	DDX_Check(pDX, IDC_XMINCHECK, m_bXMin);
	DDX_Check(pDX, IDC_YMAJCHECK, m_bYM);
	DDX_Check(pDX, IDC_YMINCHECK, m_bYMin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGridPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGridPage)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_XMAJSTYLE, OnXMajStyle)
	ON_BN_CLICKED(IDC_XMINSTYLE, OnXMinStyle)
	ON_BN_CLICKED(IDC_YMAJSTYLE, OnYMajStyle)
	ON_BN_CLICKED(IDC_YMINSTYLE, OnYMinStyle)
	ON_BN_CLICKED(IDC_AXISSTYLE, OnAxisStyle)
	ON_BN_CLICKED(IDC_XMAJCHECK, OnXMajCheck)
	ON_BN_CLICKED(IDC_XMINCHECK, OnXMinCheck)
	ON_BN_CLICKED(IDC_YMAJCHECK, OnYMajCheck)
	ON_BN_CLICKED(IDC_YMINCHECK, OnYMinCheck)
	ON_EN_CHANGE(IDC_XMINORUNIT, OnChangeXMinor)
	ON_EN_CHANGE(IDC_YMINORUNIT, OnChangeYMinor)
	ON_EN_KILLFOCUS(IDC_XMINORUNIT, OnKillFocusXMinorUnit)
	ON_EN_KILLFOCUS(IDC_YMINORUNIT, OnKillFocusYMinorUnit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridPage message handlers

void CGridPage::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
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
		CPen CurvePen(PS_GEOMETRIC | m_nStyle, m_nWidth, &lb);

		pDC->SelectObject(&CurvePen);
		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
	else if(nIDCtl==IDC_XMAJSTYLE){
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_XMAJSTYLE)->GetClientRect(&SRect);
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
	else if(nIDCtl==IDC_YMAJSTYLE){
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_YMAJSTYLE)->GetClientRect(&SRect);
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
		CPen CurvePen(PS_GEOMETRIC | m_YMinStyle,  GetPenWidth(m_YMinWidth,false), &lb);

		pDC->SelectObject(&CurvePen);
		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
	pDC->RestoreDC(nSavedDC);	
//	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CGridPage::SetXMajGrid(bool bstate, COLORREF clr, int style, int width)
{
	m_bXM = bstate;
	m_XMClr = clr;
	m_XMStyle = style;
	m_XMWidth = width;
}

void CGridPage::SetYMajGrid(bool bstate, COLORREF clr, int style, int width)
{
	m_bYM = bstate;
	m_YMClr = clr;
	m_YMStyle = style;
	m_YMWidth = width;
}

void CGridPage::SetXMinGrid(bool bstate, bool bAuto, COLORREF clr, int style, int width,
							double MinorUnit){
	m_bXMin = bstate;
	m_bAutoXMinUnit = bAuto;
	m_XMinClr = clr;
	m_XMinStyle = style;
	m_XMinWidth = width;
	m_XMinorUnit = MinorUnit;
}

void CGridPage::SetYMinGrid(bool bstate, bool bAuto, COLORREF clr, int style, int width,
							double MinorUnit)
{
	m_bYMin = bstate;
	m_bAutoYMinUnit = bAuto;
	m_YMinClr = clr;
	m_YMinStyle = style;
	m_YMinWidth = width;
	m_YMinorUnit = MinorUnit;
}

void CGridPage::SetAxisStyle(int cr, int s, int w)
{
	m_nStyle = s;
	m_nWidth = w;
	m_crColor = cr;
}


void CGridPage::OnXMajStyle() 
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

	m_ctrlXMajStyle.Invalidate(TRUE);	
}

void CGridPage::OnYMajStyle() 
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

	m_ctrlYMajStyle.Invalidate(TRUE);
}

void CGridPage::OnXMinStyle() 
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

	m_ctrlXMinStyle.Invalidate(TRUE);
}


void CGridPage::OnYMinStyle() 
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

	m_ctrlYMinStyle.Invalidate(TRUE);
	
}

void CGridPage::OnAxisStyle() 
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


BOOL CGridPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_ctrlXMajCheck.SetCheck(m_bXM);
	m_ctrlYMajCheck.SetCheck(m_bYM);
	m_ctrlXMinCheck.SetCheck(m_bXMin);
	m_ctrlYMinCheck.SetCheck(m_bYMin);

	m_ctrlXMinorUnit.EnableWindow(m_bXMin);
	m_ctrlXMinStyle.EnableWindow(m_bXMin);
	m_ctrlYMinorUnit.EnableWindow(m_bYMin);
	m_ctrlYMinStyle.EnableWindow(m_bYMin);
	if(!m_bXMin) m_ctrlXMinorUnit.SetValue(m_pGraph->GetXUnit()/5.f);
	if(!m_bYMin) m_ctrlYMinorUnit.SetValue(m_pGraph->GetYUnit()/5.f);


	return TRUE; 
}


void CGridPage::OnXMajCheck() 
{
	if(m_ctrlXMajCheck.GetCheck()) m_bXM = true;   else  m_bXM = false;
}

void CGridPage::OnXMinCheck()
{
	if(m_ctrlXMinCheck.GetCheck()) {
		m_bXMin = true;   
	}
	else  {
		m_bXMin = false;	
	}
	m_ctrlXMinorUnit.EnableWindow(m_bXMin);
	m_ctrlXMinStyle.EnableWindow(m_bXMin);
}

void CGridPage::OnYMajCheck()
{
	if(m_ctrlYMajCheck.GetCheck()) m_bYM = true; else  m_bYM = false;
}

void CGridPage::OnYMinCheck() 
{
	if(m_ctrlYMinCheck.GetCheck()) {
		m_bYMin = true; 
	}
	else  {
		m_bYMin = false;
	}
	m_ctrlYMinorUnit.EnableWindow(m_bYMin);
	m_ctrlYMinStyle.EnableWindow(m_bYMin);
	m_bAutoYMinUnit = true;
}

BOOL CGridPage::OnSetActive() 
{
	double mx = (double)__max(abs(m_pGraph->GetXMax()), abs(m_pGraph->GetXMin()));
	int exp = (int)log10(mx);
	int precision = 3-exp;
	if(precision<0) precision = 2;
	m_ctrlXMinorUnit.SetPrecision(precision);
	m_ctrlXMinorUnit.SetValue(m_XMinorUnit);


	double my = (double)__max(abs(m_pGraph->GetYMax()), abs(m_pGraph->GetYMin()));
	exp = (int)log10(my);
	precision = 3-exp;
	if(precision<0) precision = 2;
	m_ctrlYMinorUnit.SetPrecision(precision);
	m_ctrlYMinorUnit.SetValue(m_YMinorUnit);

	return CPropertyPage::OnSetActive();
}

BOOL CGridPage::OnKillActive() 
{
	SetGraphDlgPage(1);
	m_XMinorUnit = m_ctrlXMinorUnit.GetValue();
	m_YMinorUnit = m_ctrlYMinorUnit.GetValue();
	if(m_XMinorUnit<=0 || m_XMinorUnit > m_pGraph->GetXUnit()) {
		m_ctrlXMinorUnit.SetSel(0,-1);
		m_ctrlXMinorUnit.SetFocus();
		return FALSE;
	}
	if(m_YMinorUnit<=0|| m_YMinorUnit > m_pGraph->GetYUnit()) {
		m_ctrlYMinorUnit.SetSel(0,-1);
		m_ctrlYMinorUnit.SetFocus();
		return FALSE;
	}
	SetGraphDlgPage(1);
	return CPropertyPage::OnKillActive();
}

BOOL CGridPage::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN){ 
		CWnd* pWnd = GetFocus();
		if(GetDlgItem(IDC_XMINORUNIT) == pWnd){
			m_XMinorUnit = m_ctrlXMinorUnit.GetValue();
			if(m_XMinorUnit<=0 || m_XMinorUnit > m_pGraph->GetXUnit()) {
				m_ctrlXMinorUnit.SetSel(0,-1);
				m_ctrlXMinorUnit.SetFocus();
				return true;
			}
		}
		else if (GetDlgItem(IDC_YMINORUNIT) == pWnd){
			m_YMinorUnit = m_ctrlYMinorUnit.GetValue();
			if(m_YMinorUnit<=0|| m_YMinorUnit > m_pGraph->GetYUnit()) {
				m_ctrlYMinorUnit.SetSel(0,-1);
				m_ctrlYMinorUnit.SetFocus();
				return true;
			}
		}
		CGraphDlg* pDlg = (CGraphDlg*)m_pParent;
		pDlg->m_ctrlApply.SetFocus();

	}
	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CGridPage::OnKillFocusXMinorUnit() 
{
	m_XMinorUnit = m_ctrlXMinorUnit.GetValue();
	if(m_XMinorUnit<=0) {
		m_XMinorUnit =  m_pGraph->GetXUnit()/5.f;
		m_ctrlXMinorUnit.SetValue(m_XMinorUnit);
		m_ctrlXMinorUnit.SetSel(0,-1);
		m_ctrlXMinorUnit.SetFocus();
	}
	else if(m_XMinorUnit>=m_pGraph->GetXUnit()) {
		m_XMinorUnit =  m_pGraph->GetXUnit();
		m_ctrlXMinorUnit.SetValue(m_XMinorUnit);
		m_ctrlXMinorUnit.SetSel(0,-1);
		m_ctrlXMinorUnit.SetFocus();
	}
}

void CGridPage::OnKillFocusYMinorUnit() 
{
	m_YMinorUnit = m_ctrlYMinorUnit.GetValue();
	if(m_YMinorUnit<=0) {
		m_YMinorUnit =  m_pGraph->GetYUnit()/5.f;
		m_ctrlYMinorUnit.SetValue(m_YMinorUnit);
		m_ctrlYMinorUnit.SetSel(0,-1);
		m_ctrlYMinorUnit.SetFocus();
	}
	else if(m_YMinorUnit>=m_pGraph->GetYUnit()) {
		m_YMinorUnit =  m_pGraph->GetYUnit();
		m_ctrlYMinorUnit.SetValue(m_YMinorUnit);
		m_ctrlYMinorUnit.SetSel(0,-1);
		m_ctrlYMinorUnit.SetFocus();
	}
}
void CGridPage::OnChangeXMinor()
{
	m_bAutoXMinUnit = false;
}
void CGridPage::OnChangeYMinor()
{
	m_bAutoYMinUnit = false;
}
////////////////////////////////////////////////////////////////////////////
// CScalePage property page

IMPLEMENT_DYNCREATE(CScalePage, CPropertyPage)

CScalePage::CScalePage() : CPropertyPage(CScalePage::IDD)
{
	//{{AFX_DATA_INIT(CScalePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
CScalePage::CScalePage(CWnd* pParent) : CPropertyPage(CScalePage::IDD)
{
	m_pParent = pParent;
}

CScalePage::~CScalePage()
{
}

void CScalePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScalePage)
	DDX_Control(pDX, IDC_YUNIT, m_ctrlYUnit);
	DDX_Control(pDX, IDC_YO, m_ctrlY0);
	DDX_Control(pDX, IDC_YMIN, m_ctrlYMin);
	DDX_Control(pDX, IDC_YMAX, m_ctrlYMax);
	DDX_Control(pDX, IDC_XUNIT, m_ctrlXUnit);
	DDX_Control(pDX, IDC_XO, m_ctrlX0);
	DDX_Control(pDX, IDC_XMIN, m_ctrlXMin);
	DDX_Control(pDX, IDC_XMAX, m_ctrlXMax);
	DDX_Control(pDX, IDC_AUTOX, m_ctrlAutoX);
	DDX_Control(pDX, IDC_AUTOY, m_ctrlAutoY);
	DDX_Control(pDX, IDC_YINVERTED, m_ctrlYInverted);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScalePage, CPropertyPage)
	//{{AFX_MSG_MAP(CScalePage)
	ON_BN_CLICKED(IDC_AUTOX, OnAutoX)
	ON_BN_CLICKED(IDC_AUTOY, OnAutoY)
	ON_BN_CLICKED(IDC_YINVERTED, OnYInverted)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScalePage message handlers

BOOL CScalePage::OnSetActive() 
{
	EnableXAxisCtrls(!m_bAutoX);
	EnableYAxisCtrls(!m_bAutoY);
	return CPropertyPage::OnSetActive();
}


void CScalePage::SetValues()
{
	EnableXAxisCtrls(!m_bAutoX);
	EnableYAxisCtrls(!m_bAutoY);

	double mx = (double)__max(abs(m_pGraph->GetXMax()), abs(m_pGraph->GetXMin()));
	int exp = (int)log10(mx);
	int precision = 3-exp;
	if(precision<0) precision = 0;
	m_ctrlXMin.SetPrecision(precision);
	m_ctrlXMax.SetPrecision(precision);
	m_ctrlXUnit.SetPrecision(precision);
	m_ctrlX0.SetPrecision(precision);

	double my = (double)__max(abs(m_pGraph->GetYMax()), abs(m_pGraph->GetYMin()));
	exp = (int)log10(my);
	precision = 3-exp;
	if(precision<0) precision = 0;
	m_ctrlYMin.SetPrecision(precision);
	m_ctrlYMax.SetPrecision(precision);
	m_ctrlYUnit.SetPrecision(precision);
	m_ctrlY0.SetPrecision(precision);

	m_ctrlXMin.SetValue(m_pGraph->GetXMin());
	m_ctrlXMax.SetValue(m_pGraph->GetXMax());
	m_ctrlXUnit.SetValue(m_pGraph->GetXUnit());
	m_ctrlX0.SetValue(m_pGraph->GetX0());
	m_ctrlYMin.SetValue(m_pGraph->GetYMin());
	m_ctrlYMax.SetValue(m_pGraph->GetYMax());
	m_ctrlYUnit.SetValue(m_pGraph->GetYUnit());
	m_ctrlY0.SetValue(m_pGraph->GetY0());

	m_ctrlAutoX.SetCheck(m_bAutoX);
	m_ctrlAutoY.SetCheck(m_bAutoY);
	m_ctrlYInverted.SetCheck(m_bYInverted);
}


void CScalePage::EnableXAxisCtrls(bool bEnable){
	m_ctrlXMin.EnableWindow(bEnable);
	m_ctrlXMax.EnableWindow(bEnable);
	m_ctrlXUnit.EnableWindow(bEnable);
	m_ctrlX0.EnableWindow(bEnable);
}


void CScalePage::EnableYAxisCtrls(bool bEnable){
	m_ctrlYMin.EnableWindow(bEnable);
	m_ctrlYMax.EnableWindow(bEnable);
	m_ctrlYUnit.EnableWindow(bEnable);
	m_ctrlY0.EnableWindow(bEnable);
}


void CScalePage::OnAutoX() {
	if(m_ctrlAutoX.GetCheck()){
		m_bAutoX = true;
		EnableXAxisCtrls(false);
//		Invalidate();
	}
	else {
		m_bAutoX = false;
		SetValues();
		EnableXAxisCtrls(true);
	}
}


void CScalePage::OnAutoY() {
	if(m_ctrlAutoY.GetCheck()){
		m_bAutoY = true;
		EnableYAxisCtrls(false);
//		Invalidate();
	}
	else {
		m_bAutoY = false;
		SetValues();
		EnableYAxisCtrls(true);
	}
}

void CScalePage::OnYInverted() {
	if(m_ctrlYInverted.GetCheck()){
		m_bYInverted = true;
	}
	else {
		m_bYInverted = false;
	}
}

BOOL CScalePage::PreTranslateMessage(MSG* pMsg) 
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)){
		CGraphDlg* pDlg = (CGraphDlg*)m_pParent;
		pDlg->m_ctrlApply.SetFocus();
	}
	
	return CPropertyPage::PreTranslateMessage(pMsg);
}






