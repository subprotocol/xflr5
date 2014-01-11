/****************************************************************************

    LinePickerDlg Class
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
#include "LinePickerDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CLinePickerDlg dialog


CLinePickerDlg::CLinePickerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLinePickerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinePickerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pParent = pParent;
	m_nStyle = PS_SOLID;
	m_nWidth = 1;
	m_crColor =RGB(0,0,0);
}


void CLinePickerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinePickerDlg)
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_WIDTH, m_ctrlWidth);
	DDX_Control(pDX, IDC_STYLE, m_ctrlStyle);
	DDX_Control(pDX, IDC_COLORBTN, m_ctrlColorBtn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinePickerDlg, CDialog)
	//{{AFX_MSG_MAP(CLinePickerDlg)
	ON_CBN_SELCHANGE(IDC_WIDTH, OnSelChangeWidth)
	ON_CBN_SELCHANGE(IDC_STYLE, OnSelChangeStyle)
	ON_BN_CLICKED(IDC_COLORBTN, OnColorBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinePickerDlg message handlers


BOOL CLinePickerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	int i,j;
	m_ctrlColorBtn.SetColor(m_crColor);

	for (i=0; i<5;i++)
	{
		m_ctrlWidth.m_Lines[i].nWidth = i;
		m_ctrlWidth.m_Lines[i].crColor = m_crColor;
		m_ctrlWidth.m_Lines[i].nPenStyle = m_nStyle;
	}

	for (i=0; i<5;i++)
	{
		m_ctrlStyle.m_Lines[i].nWidth = m_nWidth;
		m_ctrlStyle.m_Lines[i].crColor = m_crColor;
	}

	m_ctrlStyle.m_Lines[0].nPenStyle = PS_SOLID;
	m_ctrlStyle.m_Lines[1].nPenStyle = PS_DASH;
	m_ctrlStyle.m_Lines[2].nPenStyle = PS_DOT;
	m_ctrlStyle.m_Lines[3].nPenStyle = PS_DASHDOT;
	m_ctrlStyle.m_Lines[4].nPenStyle = PS_DASHDOTDOT;

	m_ctrlStyle.AddLine();
	m_ctrlStyle.AddLine();
	m_ctrlStyle.AddLine();
	m_ctrlStyle.AddLine();
	m_ctrlStyle.AddLine();

	m_ctrlWidth.AddLine();
	m_ctrlWidth.AddLine();
	m_ctrlWidth.AddLine();
	m_ctrlWidth.AddLine();
	m_ctrlWidth.AddLine();
	m_ctrlWidth.m_bWidth = true;

	m_ctrlStyle.SetCurSel(0);
	m_ctrlWidth.SetCurSel(0);

	LineData* pLineData;
	for (j=0; j<m_ctrlStyle.GetCount(); j++)
	{
		pLineData = (LineData*)m_ctrlStyle.GetItemDataPtr(j);
		if(pLineData->nPenStyle == m_nStyle){
			m_ctrlStyle.SetCurSel(j);
		}
	}
	for (j=0; j<m_ctrlWidth.GetCount(); j++)
	{
		pLineData = (LineData*)m_ctrlWidth.GetItemDataPtr(j);
		if(pLineData->nWidth == m_nWidth){
			m_ctrlWidth.SetCurSel(j);
		}
	}
	Invalidate(true);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CLinePickerDlg::OnColorBtn() 
{
	int i;
	m_crColor = m_ctrlColorBtn.GetColor();

	for (i=0; i<5;i++){
		m_ctrlWidth.m_Lines[i].crColor = m_crColor;
	}

	for (i=0; i<5;i++){
		m_ctrlStyle.m_Lines[i].crColor = m_crColor;
	}
	m_ctrlOK.SetFocus();
	Invalidate(true);
}


void CLinePickerDlg::OnSelChangeWidth()
{
	int j = m_ctrlWidth.GetCurSel();
	if(j != CB_ERR){
		LineData* pLineData = (LineData*)m_ctrlWidth.GetItemDataPtr(j);
		m_nWidth = pLineData->nWidth;

		for (int i=0; i<5;i++){
			m_ctrlStyle.m_Lines[i].nWidth = m_nWidth;
		}

		m_ctrlOK.SetFocus();
		Invalidate(true);
	}
}


void CLinePickerDlg::OnSelChangeStyle() 
{
	int j = m_ctrlStyle.GetCurSel();
	if(j != CB_ERR){
		LineData* pLineData = (LineData*)m_ctrlStyle.GetItemDataPtr(j);
		m_nStyle = pLineData->nPenStyle;
		for (int i=0; i<5;i++){
			m_ctrlWidth.m_Lines[i].nPenStyle = m_nStyle;
		}
		m_ctrlOK.SetFocus();
		Invalidate(true);
	}
}


COLORREF CLinePickerDlg::GetColor(){
	return m_crColor;
}


int CLinePickerDlg::GetStyle(){
	return m_nStyle;
}


int CLinePickerDlg::GetWidth(){
	return m_nWidth;
}


void CLinePickerDlg::SetColor(COLORREF crColor){
	m_crColor = crColor;
}


void CLinePickerDlg::SetStyle(int nStyle){
	m_nStyle = nStyle;
}

void CLinePickerDlg::SetWidth(int nWidth){
	m_nWidth = nWidth;
}


