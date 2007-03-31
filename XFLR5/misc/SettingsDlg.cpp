/****************************************************************************

    SettingsDlg Class
    Copyright (C) 2006 André Deperrois XFLR5@yahoo.com

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

// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "LinePickerDlg.h"
#include "SettingsDlg.h"



/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog


CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingsDlg)
	//}}AFX_DATA_INIT
	m_pParent = pParent;
}

 
void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsDlg)
	DDX_Control(pDX, IDC_VLMMESH, m_ctrlVLMMesh);
//	DDX_Control(pDX, IDC_WINGSURFACES, m_ctrlWingSurfaces);
	DDX_Control(pDX, IDC_DOWNWASH, m_ctrlDownwash);
	DDX_Control(pDX, IDC_WAKE, m_ctrlWake);
	DDX_Control(pDX, IDC_XCP, m_ctrlXCP);
	DDX_Control(pDX, IDC_IDRAG, m_ctrlIDrag);
	DDX_Control(pDX, IDC_VDRAG, m_ctrlVDrag);
	DDX_Control(pDX, IDC_BOTTRANS, m_ctrlBotTrans);
	DDX_Control(pDX, IDC_TOPTRANS, m_ctrlTopTrans);
	DDX_Control(pDX, IDC_WINGOUTLINE, m_ctrlWingPanels);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CSettingsDlg)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_WINGOUTLINE, OnWingColor)
	ON_BN_CLICKED(IDC_TOPTRANS, OnTopTrans)
	ON_BN_CLICKED(IDC_BOTTRANS, OnBotTrans)
	ON_BN_CLICKED(IDC_XCP, OnXCP)
	ON_BN_CLICKED(IDC_IDRAG, OnIDrag)
	ON_BN_CLICKED(IDC_VDRAG, OnVDrag)
	ON_BN_CLICKED(IDC_DOWNWASH, OnDownwash)
	ON_BN_CLICKED(IDC_WAKE, OnWake)
	ON_BN_CLICKED(IDC_VLMMESH, OnVLMMesh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg message handlers




void CSettingsDlg::OnWingColor() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_OutlineColor);
	LPdlg.SetStyle(m_OutlineStyle);
	LPdlg.SetWidth(m_OutlineWidth);

	if (IDOK==LPdlg.DoModal()){
		m_OutlineColor = LPdlg.GetColor();
		m_OutlineStyle = LPdlg.GetStyle();
		m_OutlineWidth = LPdlg.GetWidth();
	}
	m_ctrlWingPanels.Invalidate(true);
	
}

void CSettingsDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	if(nIDCtl==IDC_WINGOUTLINE ){
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_WINGOUTLINE)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor =m_OutlineColor;
		CPen WingPen(PS_GEOMETRIC | m_OutlineStyle, GetPenWidth(m_OutlineWidth,false), &lb);

		pDC->SelectObject(&WingPen);

		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);

		pDC->SelectObject(pOldPen);
		WingPen.DeleteObject();

		pDC->SelectObject(pOldBrush);
//		FillBrush.DeleteObject();
//		pDC->Detach();

	}
	else if(nIDCtl==IDC_VLMMESH ){
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_VLMMESH)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor =m_VLMColor;
		CPen WingPen(PS_GEOMETRIC | m_VLMStyle, GetPenWidth(m_VLMWidth,false), &lb);

		pDC->SelectObject(&WingPen);

		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);

		pDC->SelectObject(pOldPen);
		WingPen.DeleteObject();

		pDC->SelectObject(pOldBrush);
//		FillBrush.DeleteObject();
//		pDC->Detach();
	}
	else if(nIDCtl==IDC_TOPTRANS ){
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_TOPTRANS)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor =m_TopColor;
		CPen TopPen(PS_GEOMETRIC | m_TopStyle, GetPenWidth(m_TopWidth,false), &lb);

		pDC->SelectObject(&TopPen);

		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);

		pDC->SelectObject(pOldPen);
		TopPen.DeleteObject();

		pDC->SelectObject(pOldBrush);
//		FillBrush.DeleteObject();
//		pDC->Detach();
	}
	else if(nIDCtl==IDC_BOTTRANS ){	
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_BOTTRANS)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor =m_BotColor;
		CPen BotPen(PS_GEOMETRIC | m_BotStyle, GetPenWidth(m_BotWidth,false), &lb);

		pDC->SelectObject(&BotPen);

		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);

		pDC->SelectObject(pOldPen);
		BotPen.DeleteObject();

		pDC->SelectObject(pOldBrush);
//		FillBrush.DeleteObject();
//		pDC->Detach();
	}

	else if(nIDCtl==IDC_XCP ){
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_XCP)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor =m_XCPColor;
		CPen BotPen(PS_GEOMETRIC | m_XCPStyle, GetPenWidth(m_XCPWidth,false), &lb);

		pDC->SelectObject(&BotPen);

		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);

		pDC->SelectObject(pOldPen);
		BotPen.DeleteObject();

		pDC->SelectObject(pOldBrush);
//		FillBrush.DeleteObject();
//		pDC->Detach();
	}
	else if(nIDCtl==IDC_VDRAG ){
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_VDRAG)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor =m_VDragColor;
		CPen BotPen(PS_GEOMETRIC | m_VDragStyle, GetPenWidth(m_VDragWidth,false), &lb);

		pDC->SelectObject(&BotPen);

		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);

		pDC->SelectObject(pOldPen);
		BotPen.DeleteObject();

		pDC->SelectObject(pOldBrush);
//		FillBrush.DeleteObject();
//		pDC->Detach();
	}
	else if(nIDCtl==IDC_IDRAG ){
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_IDRAG)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor =m_IDragColor;
		CPen BotPen(PS_GEOMETRIC | m_IDragStyle, GetPenWidth(m_IDragWidth,false), &lb);

		pDC->SelectObject(&BotPen);

		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);

		pDC->SelectObject(pOldPen);
		BotPen.DeleteObject();

		pDC->SelectObject(pOldBrush);
//		FillBrush.DeleteObject();
//		pDC->Detach();
	}	else if(nIDCtl==IDC_DOWNWASH){
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_DOWNWASH)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor =m_DownwashColor;
		CPen BotPen(PS_GEOMETRIC | m_DownwashStyle, 
					GetPenWidth(m_DownwashWidth,false), &lb);

		pDC->SelectObject(&BotPen);

		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);

		pDC->SelectObject(pOldPen);
		BotPen.DeleteObject();

		pDC->SelectObject(pOldBrush);
//		FillBrush.DeleteObject();
//		pDC->Detach();
	}
	else if(nIDCtl==IDC_WAKE){
		LOGBRUSH lb;
		lb.lbStyle = BS_SOLID;
		lb.lbColor = RGB(100,100,100);
		CPen DotPen(PS_GEOMETRIC | PS_DOT, 1, &lb);
		CPen *pOldPen =  pDC->SelectObject(&DotPen);
		CBrush FillBrush(GetSysColor(COLOR_3DFACE));
		CBrush* pOldBrush = pDC->SelectObject(&FillBrush);
		CRect SRect ;
		GetDlgItem(IDC_WAKE)->GetClientRect(&SRect);
		SRect.DeflateRect(2,2,2,2);
		pDC->Rectangle(&SRect);

		lb.lbStyle = BS_SOLID;
		lb.lbColor =m_WakeColor;
		CPen BotPen(PS_GEOMETRIC | m_WakeStyle, 
					GetPenWidth(m_WakeWidth,false), &lb);

		pDC->SelectObject(&BotPen);

		pDC->MoveTo(5,8);
		pDC->LineTo(SRect.right-5,8);

		pDC->SelectObject(pOldPen);
		BotPen.DeleteObject();

		pDC->SelectObject(pOldBrush);
//		FillBrush.DeleteObject();
//		pDC->Detach();
	}

	else {
		CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
	}
}

void CSettingsDlg::OnTopTrans() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_TopColor);
	LPdlg.SetStyle(m_TopStyle);
	LPdlg.SetWidth(m_TopWidth);

	if (IDOK==LPdlg.DoModal()){
		m_TopColor = LPdlg.GetColor();
		m_TopStyle = LPdlg.GetStyle();
		m_TopWidth = LPdlg.GetWidth();
	}
	m_ctrlTopTrans.Invalidate(true);	
}

void CSettingsDlg::OnBotTrans() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_BotColor);
	LPdlg.SetStyle(m_BotStyle);
	LPdlg.SetWidth(m_BotWidth);

	if (IDOK==LPdlg.DoModal()){
		m_BotColor = LPdlg.GetColor();
		m_BotStyle = LPdlg.GetStyle();
		m_BotWidth = LPdlg.GetWidth();
	}
	m_ctrlBotTrans.Invalidate(true);	
	
}
void CSettingsDlg::OnIDrag() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_IDragColor);
	LPdlg.SetStyle(m_IDragStyle);
	LPdlg.SetWidth(m_IDragWidth);

	if (IDOK==LPdlg.DoModal()){
		m_IDragColor = LPdlg.GetColor();
		m_IDragStyle = LPdlg.GetStyle();
		m_IDragWidth = LPdlg.GetWidth();
	}
	m_ctrlIDrag.Invalidate(true);
}

void CSettingsDlg::OnVDrag() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_VDragColor);
	LPdlg.SetStyle(m_VDragStyle);
	LPdlg.SetWidth(m_VDragWidth);

	if (IDOK==LPdlg.DoModal()){
		m_VDragColor = LPdlg.GetColor();
		m_VDragStyle = LPdlg.GetStyle();
		m_VDragWidth = LPdlg.GetWidth();
	}
	m_ctrlVDrag.Invalidate(true);
}

void CSettingsDlg::OnXCP() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_XCPColor);
	LPdlg.SetStyle(m_XCPStyle);
	LPdlg.SetWidth(m_XCPWidth);

	if (IDOK==LPdlg.DoModal()){
		m_XCPColor = LPdlg.GetColor();
		m_XCPStyle = LPdlg.GetStyle();
		m_XCPWidth = LPdlg.GetWidth();
	}
	m_ctrlXCP.Invalidate(true);
}

void CSettingsDlg::OnDownwash() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_DownwashColor);
	LPdlg.SetStyle(m_DownwashStyle);
	LPdlg.SetWidth(m_DownwashWidth);

	if (IDOK==LPdlg.DoModal()){
		m_DownwashColor = LPdlg.GetColor();
		m_DownwashStyle = LPdlg.GetStyle();
		m_DownwashWidth = LPdlg.GetWidth();
	}
	m_ctrlDownwash.Invalidate(true);		
}

void CSettingsDlg::OnWake() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_WakeColor);
	LPdlg.SetStyle(m_WakeStyle);
	LPdlg.SetWidth(m_WakeWidth);

	if (IDOK==LPdlg.DoModal()){
		m_WakeColor = LPdlg.GetColor();
		m_WakeStyle = LPdlg.GetStyle();
		m_WakeWidth = LPdlg.GetWidth();
	}
	m_ctrlWake.Invalidate(true);		
}



//DEL BOOL CSettingsDlg::OnInitDialog() 
//DEL {
//DEL 	CDialog::OnInitDialog();
//DEL 	
//DEL 	return TRUE;  
//DEL }

void CSettingsDlg::OnVLMMesh() 
{
	CLinePickerDlg LPdlg;
	LPdlg.SetColor(m_VLMColor);
	LPdlg.SetStyle(m_VLMStyle);
	LPdlg.SetWidth(m_VLMWidth);

	if (IDOK==LPdlg.DoModal()){
		m_VLMColor = LPdlg.GetColor();
		m_VLMStyle = LPdlg.GetStyle();
		m_VLMWidth = LPdlg.GetWidth();
	}
	m_ctrlVLMMesh.Invalidate(true);
}
