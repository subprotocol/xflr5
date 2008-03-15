/****************************************************************************

    CWGraphVarDlg Class
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

// WGraphVarDlg.cpp : implementation file
// The class associated to the dialog box to select the wing polar's variable to display
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "WGraphVarDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CWGraphVarDlg dialog


CWGraphVarDlg::CWGraphVarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWGraphVarDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWGraphVarDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iWVar = 0;
}


void CWGraphVarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWGraphVarDlg)
	DDX_Control(pDX, IDC_WVAR, m_ctrlWVar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWGraphVarDlg, CDialog)
	//{{AFX_MSG_MAP(CWGraphVarDlg)
	ON_LBN_DBLCLK(IDC_WVAR, OnDblclkWvar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWGraphVarDlg message handlers

void CWGraphVarDlg::OnOK() 
{
	m_iWVar = m_ctrlWVar.GetCurSel();
	CDialog::OnOK();
}
 
BOOL CWGraphVarDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ctrlWVar.AddString("Induced Angle");						//0
	m_ctrlWVar.AddString("Total Angle");						//1
	m_ctrlWVar.AddString("Local lift coef.");					//2
	m_ctrlWVar.AddString("Local Lift C.Cl/M.A.C.");				//3
	m_ctrlWVar.AddString("Airfoil viscous drag coef.");			//4
	m_ctrlWVar.AddString("Induced drag coef.");					//5
	m_ctrlWVar.AddString("Total drag coef.");					//6
	m_ctrlWVar.AddString("Local Drag C.Cd/M.A.C.");				//7
	m_ctrlWVar.AddString("Airfoil Pitching moment coef.");		//8
	m_ctrlWVar.AddString("Geom. Pitching moment coef.");		//9
	m_ctrlWVar.AddString("Total Pitching moment coef.");		//10
	m_ctrlWVar.AddString("Reynolds");							//11
	m_ctrlWVar.AddString("Top Transition x-pos%");				//12
	m_ctrlWVar.AddString("Bottom Transition x-pos%");			//13
	m_ctrlWVar.AddString("Centre of Pressure x-pos%");			//14
	m_ctrlWVar.AddString("Bending moment");						//15
	m_ctrlWVar.SetCurSel(m_iWVar);	

	m_ctrlWVar.SetFocus();
	return FALSE;  
}

void CWGraphVarDlg::OnDblclkWvar() 
{
	OnOK();	
}
