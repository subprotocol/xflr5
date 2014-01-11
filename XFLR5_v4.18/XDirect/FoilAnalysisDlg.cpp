/****************************************************************************

	Foil Analysis dialog

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


// AnalysisDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "XDirect.h"
#include "FoilAnalysisDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CFoilAnalysisDlg dialog


CFoilAnalysisDlg::CFoilAnalysisDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFoilAnalysisDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFoilAnalysisDlg)
	m_fMach = 0.0f;
	//}}AFX_DATA_INIT
	m_Reynolds = 100000.f;
	m_ASpec = 0.f;
	m_pParent = pParent;
	m_bAutoName = true;
	m_NCrit = 9.0;
	m_XTopTr = 1.0;
	m_XBotTr = 1.0;
}


void CFoilAnalysisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFoilAnalysisDlg)
	DDX_Control(pDX, IDC_NCRIT, m_ctrlNCrit);
	DDX_Control(pDX, IDC_XTRIP1, m_ctrlXTrip1);
	DDX_Control(pDX, IDC_XTRIP2, m_ctrlXTrip2);
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_REYNOLDS, m_ctrlReNbr);
	DDX_Control(pDX, IDC_MATYP, m_ctrlMatyp);
	DDX_Control(pDX, IDC_NEWPLRNAME, m_ctrlNewPlrName);
	DDX_Control(pDX, IDC_RETYP, m_ctrlRetyp);
	DDX_Control(pDX, IDC_TYPE1, m_ctrlType);
	DDX_Control(pDX, IDC_MACH, m_ctrlMachNbr);
	DDX_Text(pDX, IDC_MACH, m_fMach);
	//}}AFX_DATA_MAP
}
 

BEGIN_MESSAGE_MAP(CFoilAnalysisDlg, CDialog)
	//{{AFX_MSG_MAP(CFoilAnalysisDlg)
	ON_BN_CLICKED(IDC_TYPE1, OnType1)
	ON_EN_SETFOCUS(IDC_NEWPLRNAME, OnSetfocusNewplrname)
	ON_BN_CLICKED(IDC_NAME1, OnName)
	ON_EN_KILLFOCUS(IDC_REYNOLDS, SetPlrName)
	ON_BN_CLICKED(IDC_TYPE2, OnType1)
	ON_BN_CLICKED(IDC_TYPE3, OnType1)
	ON_BN_CLICKED(IDC_NAME2, OnName)
	ON_EN_KILLFOCUS(IDC_MACH, SetPlrName)
	ON_BN_CLICKED(IDC_TYPE4, OnType1)
	ON_EN_KILLFOCUS(IDC_NCRIT, SetPlrName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFoilAnalysisDlg message handlers

BOOL CFoilAnalysisDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CXDirect* pXDirect = (CXDirect*)m_pParent;

	m_FoilName = pXDirect->m_pCurFoil->m_FoilName;
	CString str = "Analysis parameters for "+ m_FoilName;
	SetWindowText(str);

	m_ctrlReNbr.SetPrecision(0);

	CPolar *pPolar;
	int size = (int)pXDirect->m_poaPolar->GetSize();

	bool bFound = false;
	for (int j=0; j<size; j++){
		pPolar = (CPolar*)pXDirect->m_poaPolar->GetAt(j);
		if(pPolar->m_FoilName == m_FoilName) {
			bFound = true;
		}
	}

	m_ctrlReNbr.SetValue(m_Reynolds);
	m_ctrlMachNbr.SetValue(m_fMach);
	m_ctrlNCrit.SetValue(m_NCrit);
	m_ctrlXTrip1.SetValue(m_XTopTr);
	m_ctrlXTrip2.SetValue(m_XBotTr);

	m_bAutoName = true;
	CheckRadioButton(IDC_NAME1, IDC_NAME2, IDC_NAME1);
	switch(m_Type){
		case 1:		{
			CheckRadioButton(IDC_TYPE1,IDC_TYPE4, IDC_TYPE1);
			break;
		}
		case 2:		{
			CheckRadioButton(IDC_TYPE1,IDC_TYPE4, IDC_TYPE2);
			break;
		}
		case 3:		{
			CheckRadioButton(IDC_TYPE1,IDC_TYPE4, IDC_TYPE3);
			break;
		}
		case 4:		{
			CheckRadioButton(IDC_TYPE1,IDC_TYPE4, IDC_TYPE4);
			break;
		}
	}
	OnType1();


	SetPlrName();

	return TRUE;  
}


void CFoilAnalysisDlg::OnType1() 
{
	int idc = GetCheckedRadioButton(IDC_TYPE1, IDC_TYPE4);
	switch(idc)	{
		case IDC_TYPE1:
			CheckRadioButton(IDC_TYPE1, IDC_TYPE4, IDC_TYPE1);
			m_ctrlRetyp.SetWindowText("&Reynolds =");
			m_ctrlMatyp.SetWindowText("&Mach =");
			m_ctrlReNbr.SetPrecision(0);
			m_ctrlReNbr.SetValue(m_Reynolds);
			m_Type = 1;
			break;
		case IDC_TYPE2:
			CheckRadioButton(IDC_TYPE1, IDC_TYPE4, IDC_TYPE2);
			m_ctrlRetyp.SetWindowText("&Re.sqrt(Cl) =");
			m_ctrlMatyp.SetWindowText("&Ma.sqrt(Cl) =");
			m_ctrlReNbr.SetPrecision(0);
			m_ctrlReNbr.SetValue(m_Reynolds);
			m_Type = 2;
			break;
		case IDC_TYPE3:
			CheckRadioButton(IDC_TYPE1, IDC_TYPE4, IDC_TYPE3);
			m_ctrlRetyp.SetWindowText("&Re.Cl =");
			m_ctrlMatyp.SetWindowText("&Mach =");
			m_ctrlReNbr.SetPrecision(0);
			m_ctrlReNbr.SetValue(m_Reynolds);
			m_Type = 3;
			break;
		case IDC_TYPE4:
			CheckRadioButton(IDC_TYPE1, IDC_TYPE4, IDC_TYPE4);
			m_ctrlRetyp.SetWindowText("&Alpha =");
			m_ctrlMatyp.SetWindowText("&Mach =");
			m_ctrlReNbr.SetPrecision(2);
			m_ctrlReNbr.SetValue(m_ASpec);
			m_Type = 4;
			break;
		default:
			CheckRadioButton(IDC_TYPE1, IDC_TYPE4, IDC_TYPE1);
			m_ctrlRetyp.SetWindowText("&Reynolds =");
			m_ctrlMatyp.SetWindowText("&Mach =");
			m_ctrlReNbr.SetPrecision(0);
			m_ctrlReNbr.SetValue(m_Reynolds);
			m_Type = 1;
			break;
	}
	m_ctrlRetyp.Invalidate();
	m_ctrlMatyp.Invalidate();
	SetPlrName();
}


void CFoilAnalysisDlg::OnSetfocusNewplrname() 
{
	m_bAutoName = false;
	CheckRadioButton(IDC_NAME1, IDC_NAME2, IDC_NAME2);
	EnableControls(true);
}

 
void CFoilAnalysisDlg::OnOK() {
	CXDirect* pXDirect = (CXDirect*)m_pParent;

	CPolar * pOldPolar;
	// we don't want to exit on OK on CEdit
	int LineLength = m_ctrlNewPlrName.LineLength();
	if(!LineLength) {
		AfxMessageBox("Must enter a name", MB_OK);
		GetDlgItem(IDC_NEWPLRNAME)->SetFocus();
		return;
	}
	else{
		SetPlrName();
		m_ctrlNewPlrName.GetWindowText(m_PlrName);
//		m_ctrlNewPlrName.GetLine(0, m_PlrName.GetBuffer(LineLength));
//		m_PlrName.ReleaseBuffer(LineLength);
		int size = (int)pXDirect->m_poaPolar->GetSize();
		for (int j=0; j<size; j++){
			pOldPolar = (CPolar*)pXDirect->m_poaPolar->GetAt(j);
			if (pOldPolar->m_PlrName  == m_PlrName &&
				pOldPolar->m_FoilName == m_FoilName){
				AfxMessageBox("Polar already exists", MB_OK);
				return;
			}
		}
		ReadParams();
	}
	CDialog::OnOK();
}


void CFoilAnalysisDlg::EnableControls(bool bEnable)
{
	m_ctrlType.EnableWindow(bEnable);
	GetDlgItem(IDC_TYPE2)->EnableWindow(bEnable);
	GetDlgItem(IDC_TYPE3)->EnableWindow(bEnable);
	m_ctrlRetyp.EnableWindow(bEnable);
	m_ctrlMatyp.EnableWindow(bEnable);
	m_ctrlReNbr.EnableWindow(bEnable);
	m_ctrlMachNbr.EnableWindow(bEnable);
}




void CFoilAnalysisDlg::Initialize()
{
	m_Reynolds = m_ReDef;
	m_fMach     = m_MachDef;
	if (m_MaTypDef==1 && m_ReTypDef==1)	     m_Type = 1;
	else if (m_MaTypDef==2 && m_ReTypDef==2) m_Type = 2;
	else if (m_MaTypDef==1 && m_ReTypDef==3) m_Type = 3;
	else m_Type = 1;

}



void CFoilAnalysisDlg::OnName() 
{
	int sel = GetCheckedRadioButton(IDC_NAME1, IDC_NAME2);
	if (sel == IDC_NAME2) {
		m_bAutoName = false;
		m_ctrlNewPlrName.SetFocus();
		m_ctrlNewPlrName.SetSel(0,-1);
	}
	else {
		m_bAutoName = true;
		SetPlrName();
	}
}

void CFoilAnalysisDlg::SetPlrName()
{
	ReadParams();
	if(m_bAutoName){
		if(m_Type!=4){
			double Re = m_Reynolds/1000000.;
			m_PlrName.Format("T%d_Re%.2f_M%.2f", m_Type, Re, m_fMach);
		}
		else{
			m_PlrName.Format("T%d_Al%5.2f_M%.2f", m_Type, m_ASpec, m_fMach);
		}
		CString str;
		str.Format("_N%.1f", m_NCrit);
		m_PlrName += str;
	
		m_ctrlNewPlrName.SetWindowText(m_PlrName);
	}
}


void CFoilAnalysisDlg::ReadParams()
{
	if(m_Type==4) m_ASpec = m_ctrlReNbr.GetValue();
	else m_Reynolds = m_ctrlReNbr.GetValue();

	m_fMach     = m_ctrlMachNbr.GetValue();

	m_NCrit = m_ctrlNCrit.GetValue();
	m_XTopTr = m_ctrlXTrip1.GetValue();
	m_XBotTr = m_ctrlXTrip2.GetValue();
}



BOOL CFoilAnalysisDlg::PreTranslateMessage(MSG* pMsg) 
{
	// Prevent Return Key from closing App
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN){
		CWnd* pWnd = GetFocus();
		if(pWnd != GetDlgItem(IDOK) && pWnd != GetDlgItem(IDCANCEL) ){
			ReadParams();
			SetPlrName();
			m_ctrlOK.SetFocus();
			return TRUE ;
		}
	}
	//	
	return CDialog::PreTranslateMessage(pMsg);
}
