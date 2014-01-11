/****************************************************************************

    CPrintOptions Class
	Copyright (C) 2005 André Deperrois xflr5@yahoo.com

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

// PrintOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "PrintOptionsDlg.h"



/////////////////////////////////////////////////////////////////////////////
// CPrintOptionsDlg dialog


CPrintOptionsDlg::CPrintOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintOptionsDlg)
	m_PrinterName = _T("");
	m_PaperSize = _T("");
	//}}AFX_DATA_INIT
	m_bColor = true;
	m_Right  = 10.0;
	m_Left   = 10.0;
	m_Top    = 10.0;
	m_Bottom = 10.0;
}


void CPrintOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintOptionsDlg)
	DDX_Control(pDX, IDC_PENWIDTH, m_ctrlPenWidth);
	DDX_Control(pDX, IDC_RADIO1, m_ctrlRadio1);
	DDX_Control(pDX, IDC_TOP, m_ctrlTop);
	DDX_Control(pDX, IDC_RIGHT, m_ctrlRight);
	DDX_Control(pDX, IDC_LEFT, m_ctrlLeft);
	DDX_Control(pDX, IDC_BOTTOM, m_ctrlBottom);
	DDX_Control(pDX, IDC_WINDOWBK, m_ctrlWindowBk);
	DDX_Control(pDX, IDC_GRAPHBK, m_ctrlGraphBk);
	DDX_Text(pDX, IDC_PRINTERNAME, m_PrinterName);
	DDX_Text(pDX, IDC_PAPERSIZE, m_PaperSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CPrintOptionsDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio1)
	ON_BN_CLICKED(IDC_WINDOWBK, OnWindowBk)
	ON_BN_CLICKED(IDC_GRAPHBK, OnGraphBk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintOptionsDlg message handlers

BOOL CPrintOptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_PaperSize.Format("Width = %.1f mm   Height = %.1f mm", m_Width/100.0, m_Height/100.0);

	m_ctrlLeft.SetPrecision(1);
	m_ctrlRight.SetPrecision(1);
	m_ctrlTop.SetPrecision(1);
	m_ctrlBottom.SetPrecision(1);

	m_ctrlLeft.SetValue(m_Left);
	m_ctrlRight.SetValue(m_Right);
	m_ctrlTop.SetValue(m_Top);
	m_ctrlBottom.SetValue(m_Bottom);
	
	m_ctrlPenWidth.SetValue(m_PenWidth/100.0);

	if(!m_bColor){
		CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
		m_ctrlGraphBk.EnableWindow(false);
		m_ctrlWindowBk.EnableWindow(false);
	}
	else{
		CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);
		m_ctrlGraphBk.EnableWindow(true);
		m_ctrlWindowBk.EnableWindow(true);
	}

	if(m_bGraphBk) m_ctrlGraphBk.SetCheck(1);
	if(m_bWindowBk) m_ctrlWindowBk.SetCheck(1);

	m_ctrlRadio1.SetFocus();

	UpdateData(false);
	return FALSE;  
}

void CPrintOptionsDlg::OnRadio1() 
{
	if(GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO2)==IDC_RADIO1){
		m_bColor = false;
		m_ctrlGraphBk.EnableWindow(false);
		m_ctrlWindowBk.EnableWindow(false);
	}
	else{
		m_bColor = true;
		m_ctrlGraphBk.EnableWindow(true);
		m_ctrlWindowBk.EnableWindow(true);
	}
}


void CPrintOptionsDlg::OnWindowBk() 
{
	if(m_ctrlWindowBk.GetCheck()) m_bWindowBk = true;
	else						  m_bWindowBk = false;
}


void CPrintOptionsDlg::OnGraphBk() 
{
	if(m_ctrlGraphBk.GetCheck()) m_bGraphBk = true;
	else  m_bGraphBk = false;	
}


void CPrintOptionsDlg::OnOK() 
{
	m_Left   = m_ctrlLeft.GetValue();	
	m_Right  = m_ctrlRight.GetValue();	
	m_Top    = m_ctrlTop.GetValue();	
	m_Bottom = m_ctrlBottom.GetValue();	
	m_PenWidth = m_ctrlPenWidth.GetValue()*100.0;
	CDialog::OnOK();
}


