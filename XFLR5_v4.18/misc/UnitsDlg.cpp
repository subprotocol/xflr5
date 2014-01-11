/****************************************************************************

    CToolBarDlg Class
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

// UnitsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "UnitsDlg.h"



/////////////////////////////////////////////////////////////////////////////
// CUnitsDlg dialog


CUnitsDlg::CUnitsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUnitsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUnitsDlg)
	m_SForce = _T("");
	m_SLength = _T("");
	m_SSpeed = _T("");
	m_SSurface = _T("");
	m_SWeight = _T("");
	m_strQuestion = _T("Select units for this project :");
	m_SMoment = _T("");
	//}}AFX_DATA_INIT
	m_Length  = 0;
	m_Area = 1;
	m_Weight  = 1;
	m_Speed   = 0;
	m_Force   = 0;
	m_bLengthOnly = false;
}


void CUnitsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUnitsDlg)
	DDX_Control(pDX, IDC_MOMENT, m_ctrlMoment);
	DDX_Control(pDX, IDC_SURFACE, m_ctrlSurface);
	DDX_Control(pDX, IDC_WEIGHT, m_ctrlWeight);
	DDX_Control(pDX, IDC_SPEED, m_ctrlSpeed);
	DDX_Control(pDX, IDC_LENGTH, m_ctrlLength);
	DDX_Control(pDX, IDC_FORCE, m_ctrlForce);
	DDX_Text(pDX, IDC_SFORCE, m_SForce);
	DDX_Text(pDX, IDC_SLENGTH, m_SLength);
	DDX_Text(pDX, IDC_SSPEED, m_SSpeed);
	DDX_Text(pDX, IDC_SSURFACE, m_SSurface);
	DDX_Text(pDX, IDC_SWEIGHT, m_SWeight);
	DDX_Text(pDX, IDC_QUESTION, m_strQuestion);
	DDX_Text(pDX, IDC_SMOMENT, m_SMoment);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUnitsDlg, CDialog)
	//{{AFX_MSG_MAP(CUnitsDlg)
	ON_CBN_SELCHANGE(IDC_FORCE, OnSelChange)
	ON_CBN_SELCHANGE(IDC_LENGTH, OnSelChange)
	ON_CBN_SELCHANGE(IDC_SPEED, OnSelChange)
	ON_CBN_SELCHANGE(IDC_SURFACE, OnSelChange)
	ON_CBN_SELCHANGE(IDC_WEIGHT, OnSelChange)
	ON_CBN_SELCHANGE(IDC_MOMENT, OnSelChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnitsDlg message handlers

BOOL CUnitsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
//the system used for internal XFLR5 calculations is
//	length :	mm
//	surface :	dm²
//	speed:		m/s
//	Weight :	kg
//	Force :		N

	m_ctrlLength.AddString("mm");		//0
	m_ctrlLength.AddString("cm");		//1
	m_ctrlLength.AddString("dm");		//2
	m_ctrlLength.AddString("m");		//3
	m_ctrlLength.AddString("in");		//4
	m_ctrlLength.AddString("ft");		//5

	m_ctrlSurface.AddString("mm²");		//0
	m_ctrlSurface.AddString("cm²");		//1
	m_ctrlSurface.AddString("dm²");		//2
	m_ctrlSurface.AddString("m²");		//3
	m_ctrlSurface.AddString("in²");		//4
	m_ctrlSurface.AddString("ft²");		//5

	m_ctrlSpeed.AddString("m/s");		//0
	m_ctrlSpeed.AddString("km/h");		//1
	m_ctrlSpeed.AddString("ft/s");		//2
	m_ctrlSpeed.AddString("kt (int.)");	//3
	m_ctrlSpeed.AddString("mph");		//4

	m_ctrlWeight.AddString("g");		//0
	m_ctrlWeight.AddString("kg");		//1
	m_ctrlWeight.AddString("oz");		//2
	m_ctrlWeight.AddString("lb");		//3

	m_ctrlForce.AddString("N");			//0
	m_ctrlForce.AddString("lbf");		//1

	m_ctrlMoment.AddString("N.m");		//0
	m_ctrlMoment.AddString("lbf.in");	//1
	m_ctrlMoment.AddString("lbf.ft");	//2
	
	m_ctrlLength.SetCurSel(m_Length);
	m_ctrlWeight.SetCurSel(m_Weight);
	m_ctrlSurface.SetCurSel(m_Area);
	m_ctrlSpeed.SetCurSel(m_Speed);
	m_ctrlForce.SetCurSel(m_Force);
	m_ctrlMoment.SetCurSel(m_Moment);

	m_ctrlLength.SetFocus();
	OnSelChange();

	if(m_bLengthOnly){
		m_ctrlSpeed.EnableWindow(false);
		m_ctrlSurface.EnableWindow(false);
		m_ctrlWeight.EnableWindow(false);
		m_ctrlForce.EnableWindow(false);
		m_ctrlMoment.EnableWindow(false);
	}

	return FALSE; 
}


void CUnitsDlg::OnSelChange() 
{
	m_Length  = m_ctrlLength.GetCurSel();
	m_Area    = m_ctrlSurface.GetCurSel();
	m_Weight  = m_ctrlWeight.GetCurSel();
	m_Speed   = m_ctrlSpeed.GetCurSel();
	m_Force   = m_ctrlForce.GetCurSel();
	m_Moment  = m_ctrlMoment.GetCurSel();

	SetUnits(m_Length, m_Area, m_Speed, m_Weight, m_Force, m_Moment,
		     m_mtoUnit, m_m2toUnit, m_mstoUnit, m_kgtoUnit,
			 m_NtoUnit, m_NmtoUnit);

	CString str;

	GetLengthUnit(str,m_Length);
	m_SLength.Format("1 m = %.3f ",m_mtoUnit);
	m_SLength += str;

	GetAreaUnit(str,m_Area);
	m_SSurface.Format("1 m² = %.3f ",m_m2toUnit);
	m_SSurface += str;

	GetWeightUnit(str,m_Weight);
	m_SWeight.Format("1 kg = %.3f ",m_kgtoUnit);
	m_SWeight += str;

	GetSpeedUnit(str,m_Speed);
	m_SSpeed.Format("1 m/s = %.3f ",m_mstoUnit);
	m_SSpeed += str;

	GetForceUnit(str,m_Force);
	m_SForce.Format("1 N = %.3f ",m_NtoUnit);
	m_SForce += str;

	GetMomentUnit(str,m_Moment);
	m_SMoment.Format("1 N.m = %.3f ",m_NmtoUnit);
	m_SMoment += str;

	UpdateData(false);
}
