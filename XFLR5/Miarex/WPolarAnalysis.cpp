/****************************************************************************

    CWPolarAnalysis Class
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

// WPolarAnalysis.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include <math.h>
#include ".\WPolarAnalysis.h"

/////////////////////////////////////////////////////////////////////////////
// CWPolarAnalysis dialog


CWPolarAnalysis::CWPolarAnalysis(CWnd* pParent /*=NULL*/)
	: CDialog(CWPolarAnalysis::IDD, pParent)
{
	m_BAutoName = TRUE;

	m_QInf       = 10.0;//m/s
	m_Weight     = 1.0;
	m_XCmRef     = 0.0;
	m_Alpha      = 0.0;
	m_Beta       = 0.0;
	m_Type       = 1;
	m_WingLoad   = 1.0;
	m_Density    = 1.225;
	m_Viscosity  = 1.5e-5;
	m_Height     = 0.0;
	m_pWing	     = NULL;
	m_pPlane     = NULL;

	m_AnalysisType = 1;

	m_bVLM1         = true;
	m_bThinSurfaces = true;
	m_bTiltedGeom   = false;
	m_bWakeRollUp   = false;
	m_bViscous      = true;
	m_bGround       = false;

	m_NXWakePanels    = 5;
	m_TotalWakeLength = 100.0;//x mac
	m_WakePanelFactor = 1.1;

	m_SymbolFont.CreatePointFont(100, "Symbol");
	m_UnitType  = 1;
}


void CWPolarAnalysis::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WAKEROLLUP, m_ctrlWakeRollUp);
	DDX_Control(pDX, IDC_VLM2, m_ctrlVLM2);
	DDX_Control(pDX, IDC_VLM1, m_ctrlVLM1);
	DDX_Control(pDX, IDC_LENGTHUNIT, m_ctrlLengthUnit);
	DDX_Control(pDX, IDC_LENGTHUNIT2, m_ctrlLengthUnit2);
	DDX_Control(pDX, IDC_XCMREF, m_ctrlXCmRef);
	DDX_Control(pDX, IDC_RHO, m_ctrlRho);
	DDX_Control(pDX, IDC_NU, m_ctrlNu);
	DDX_Control(pDX, IDC_METHOD1, m_ctrlMethod1);
	DDX_Control(pDX, IDC_METHOD3, m_ctrlMethod3);
	DDX_Control(pDX, IDC_QINFSTAT, m_ctrlQInfStat);
	DDX_Control(pDX, IDC_QINFCL, m_ctrlQInfCl);
	DDX_Control(pDX, IDC_RETIP, m_ctrlReTip);
	DDX_Control(pDX, IDC_REROOT, m_ctrlReRoot);
	DDX_Control(pDX, IDC_AUTONAME, m_ctrlAutoName);
	DDX_Control(pDX, IDC_DENSITY, m_ctrlDensity);
	DDX_Control(pDX, IDC_VISCOSITY, m_ctrlViscosity);
	DDX_Control(pDX, IDC_SVISCOSITY, m_ctrlSViscosity);
	DDX_Control(pDX, IDC_SDENSITY, m_ctrlSDensity);
	DDX_Control(pDX, IDC_HEIGHT, m_ctrlHeight);
	DDX_Control(pDX, IDC_WLUNIT, m_ctrlWLUnit);
	DDX_Control(pDX, IDC_WINGLOAD, m_ctrlWingLoad);
	DDX_Control(pDX, IDC_SPEEDUNIT, m_ctrlSpeedUnit);
	DDX_Control(pDX, IDC_WEIGHTUNIT, m_ctrlWeightUnit);
	DDX_Control(pDX, IDC_ALPHA, m_ctrlAlpha);
	DDX_Control(pDX, IDC_BETA, m_ctrlBeta);
	DDX_Control(pDX, IDC_WEIGHT, m_ctrlWeight);
	DDX_Control(pDX, IDC_WTYPE1, m_ctrlType1);
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_WPOLARNAME, m_ctrlWPolarName);
	DDX_Control(pDX, IDC_WINGNAME, m_ctrlWingName);
	DDX_Control(pDX, IDC_QINF, m_ctrlQInf);
	DDX_Control(pDX, IDC_SRE, m_ctrlSRe);
	DDX_Control(pDX, IDC_RRE, m_ctrlRRe);
	DDX_Control(pDX, IDC_VISCOUS, m_ctrlViscous);
	DDX_Control(pDX, IDC_TILTEDGEOM, m_ctrlTiltGeom);
	DDX_Check(pDX, IDC_AUTONAME, m_BAutoName);
	DDX_Control(pDX, IDC_GROUNDEFFECT, m_ctrlGroundEffect);
	DDX_Control(pDX, IDC_HEIGHT, m_ctrlHeight);
	DDX_Control(pDX, IDC_WAKEPARAMS, m_ctrlWakeParams);
	DDX_Control(pDX, IDC_THINSURFACES, m_ctrlThinSurfaces);
}


BEGIN_MESSAGE_MAP(CWPolarAnalysis, CDialog)
	ON_EN_KILLFOCUS(IDC_QINF, OnKillFocusQInf)
	ON_BN_CLICKED(IDC_WTYPE1, OnType)
	ON_EN_KILLFOCUS(IDC_WEIGHT, OnKillFocusWeight)
	ON_EN_KILLFOCUS(IDC_ALPHA, OnKillFocusAlpha)
	ON_EN_KILLFOCUS(IDC_BETA, OnKillFocusBeta)
	ON_EN_KILLFOCUS(IDC_HEIGHT, OnKillFocusHeight)
	ON_EN_KILLFOCUS(IDC_VISCOSITY, OnKillFocusViscosity)
	ON_EN_KILLFOCUS(IDC_DENSITY, OnKillFocusDensity)
	ON_BN_CLICKED(IDC_AUTONAME, OnAutoName)
	ON_EN_SETFOCUS(IDC_WPOLARNAME, OnSetFocusWPolarName)
	ON_BN_CLICKED(IDC_METHOD1, OnMethod)
	ON_BN_CLICKED(IDC_METHOD2, OnMethod)
	ON_BN_CLICKED(IDC_METHOD3, OnMethod)
	ON_EN_KILLFOCUS(IDC_XCMREF, OnKillFocusXCmRef)
	ON_BN_CLICKED(IDC_VLM1, OnVLMMethod)
	ON_BN_CLICKED(IDC_WTYPE2, OnType)
	ON_BN_CLICKED(IDC_WTYPE4, OnType)
	ON_BN_CLICKED(IDC_UNIT1, OnUnit)
	ON_BN_CLICKED(IDC_UNIT2, OnUnit)
	ON_BN_CLICKED(IDC_VLM2, OnVLMMethod)
	ON_BN_CLICKED(IDC_WAKEROLLUP, OnWakeRollUp)
	ON_BN_CLICKED(IDC_VISCOUS, OnViscous)
	ON_BN_CLICKED(IDC_TILTEDGEOM, OnTiltedGeom)
	ON_BN_CLICKED(IDC_GROUNDEFFECT, OnGroundEffect)
	ON_BN_CLICKED(IDC_WAKEPARAMS, OnWakeParams)
	ON_BN_CLICKED(IDC_THINSURFACES, OnThinSurfaces)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWPolarAnalysis message handlers


BOOL CWPolarAnalysis::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CMainFrame* pFrame = (CMainFrame*)m_pParent;
	CString str;
	int i;

	if(m_pPlane) 
	{
		if(m_AnalysisType==0 || m_AnalysisType==1) m_AnalysisType=2;
		m_ctrlMethod1.EnableWindow(false);
	}
	
	bool bWarning = false;
	for (i=0; i<m_pWing->m_NPanel-1; i++)
	{
		if(m_pWing->m_NXPanels[i]!= m_pWing->m_NXPanels[i+1])
			{
				AfxMessageBox("The number of chordwise VLM-panels across the span is not uniform\n Panel method is disabled", MB_OK);
				bWarning = true;
				break;
			}
	}

	m_ctrlRho.SetFont(&m_SymbolFont);
	m_ctrlNu.SetFont(&m_SymbolFont);

	m_BAutoName = TRUE;
	m_ctrlAutoName.SetCheck(TRUE);

	m_ctrlQInf.SetValue(m_QInf);
	m_ctrlWeight.SetValue(m_Weight);

	if(m_UnitType==1) CheckRadioButton(IDC_UNIT1, IDC_UNIT2, IDC_UNIT1);
	else              CheckRadioButton(IDC_UNIT1, IDC_UNIT2, IDC_UNIT2);

	m_ctrlWeight.SetPrecision(3);
	m_ctrlDensity.SetPrecision(5);
	m_ctrlViscosity.SetPrecision(3);

	OnUnit();

	m_ctrlHeight.SetValue(m_Height*pFrame->m_mtoUnit);
	if(m_bGround)
	{
		m_ctrlHeight.EnableWindow(true);
		m_ctrlGroundEffect.SetCheck(TRUE);
	}
	else 
	{
		m_ctrlHeight.EnableWindow(false);
		m_ctrlGroundEffect.SetCheck(FALSE);
	}

	GetSpeedUnit(str, pFrame->m_SpeedUnit);
	m_ctrlSpeedUnit.SetWindowText(str);

	CString str1,str2;
	GetWeightUnit(str1, pFrame->m_WeightUnit);
	m_ctrlWeightUnit.SetWindowText(str1);
	GetAreaUnit(str2, pFrame->m_AreaUnit);
	m_ctrlWLUnit.SetWindowText(str1+"/"+str2);

	GetLengthUnit(str, pFrame->m_LengthUnit);
	m_ctrlLengthUnit.SetWindowText(str);
	m_ctrlLengthUnit2.SetWindowText(str);
	m_ctrlXCmRef.SetValue(m_XCmRef*pFrame->m_mtoUnit);

	m_ctrlQInf.SetValue(m_QInf*pFrame->m_mstoUnit);
	m_ctrlWeight.SetValue(m_Weight*pFrame->m_kgtoUnit);

	m_ctrlBeta.SetPrecision(2);
	m_ctrlBeta.SetValue(m_Beta);

	m_ctrlAlpha.SetValue(m_Alpha);
	SetWingLoad();
	SetReynolds();

	if(bWarning)
	{
		m_ctrlMethod3.EnableWindow(false);
		if(m_AnalysisType==3) m_AnalysisType=2;
	}

	if(m_bViscous)		m_ctrlViscous.SetCheck(TRUE);
	if(m_bTiltedGeom)	m_ctrlTiltGeom.SetCheck(TRUE);
	if(m_bWakeRollUp) 	m_ctrlWakeRollUp.SetCheck(TRUE);

	m_ctrlWakeRollUp.EnableWindow(FALSE);
	m_ctrlWakeParams.EnableWindow(FALSE);

	OnTiltedGeom();

	if(m_AnalysisType==0) m_AnalysisType=2;//former m_bLLT=false;
	if(m_AnalysisType==1) 
	{
		CheckRadioButton(IDC_METHOD1, IDC_METHOD3, IDC_METHOD1);
		m_ctrlViscous.SetCheck(TRUE);
		m_ctrlViscous.EnableWindow(false);
		m_ctrlWakeRollUp.SetCheck(FALSE);
		m_ctrlWakeParams.EnableWindow(FALSE);
	}
	else if(m_AnalysisType==2) 
	{
		CheckRadioButton(IDC_METHOD1, IDC_METHOD3, IDC_METHOD2);
		m_ctrlViscous.EnableWindow(true);
	}
	else if(m_AnalysisType==3) {

		CheckRadioButton(IDC_METHOD1, IDC_METHOD3, IDC_METHOD3);
		m_ctrlViscous.EnableWindow(true);
	}

	if(m_bVLM1)	{
		CheckRadioButton(IDC_VLM1, IDC_VLM2, IDC_VLM1);
	}
	else {
		CheckRadioButton(IDC_VLM1, IDC_VLM2, IDC_VLM2);
	}

	SetWPolarName();
	
	m_ctrlWingName.SetWindowText(m_UFOName);
	CheckType();
	EnableControls();
	OnMethod();

	m_ctrlThinSurfaces.EnableWindow(FALSE);
//	if(m_bThinSurfaces)	m_ctrlThinSurfaces.SetCheck(TRUE);
//	else                m_ctrlThinSurfaces.SetCheck(FALSE);

	m_WakeParamsdlg.m_NXWakePanels    = m_NXWakePanels;
	m_WakeParamsdlg.m_TotalWakeLength = m_TotalWakeLength;
	m_WakeParamsdlg.m_WakePanelFactor = m_WakePanelFactor;

	m_ctrlQInf.SetSel(0,-1);
	m_ctrlQInf.SetFocus();

	return FALSE; 
}


BOOL CWPolarAnalysis::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN ){ 
		CWnd* pWnd = GetFocus();
		SHORT sh1 = GetKeyState(VK_LCONTROL);
		SHORT sh2 = GetKeyState(VK_RCONTROL);
		SHORT sh3 = GetKeyState(VK_SHIFT);
		if(pMsg->wParam == VK_RETURN){

			if(GetDlgItem(IDCANCEL) != pWnd && GetDlgItem(IDOK) != pWnd){
				// we don't want to exit on OK on CEdit
				ReadParams();
				SetReynolds();
				SetWingLoad();
				SetWPolarName();
				m_ctrlOK.SetFocus();
				return true;
			}
			else if(GetDlgItem(IDOK) == pWnd ) OnOK();
			else if(GetDlgItem(IDCANCEL) == pWnd ) OnCancel();
			return true;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CWPolarAnalysis::ReadParams()
{
	CMainFrame* pFrame = (CMainFrame*)m_pParent;
	m_Alpha  = m_ctrlAlpha.GetValue();
	m_Beta   = m_ctrlBeta.GetValue();
	m_QInf   = abs(m_ctrlQInf.GetValue()) / pFrame->m_mstoUnit;
	m_Weight = abs(m_ctrlWeight.GetValue()) /pFrame->m_kgtoUnit;
	m_XCmRef = m_ctrlXCmRef.GetValue() /pFrame->m_mtoUnit;
	m_Height = m_ctrlHeight.GetValue() /pFrame->m_mtoUnit;


	if(IDC_UNIT1 == GetCheckedRadioButton(IDC_UNIT1,IDC_UNIT2))
	{
		m_Density   = m_ctrlDensity.GetValue();
		m_Viscosity = m_ctrlViscosity.GetValue(); 
	}
	else
	{
		m_Density   = m_ctrlDensity.GetValue() / 0.00194122;
		m_Viscosity = m_ctrlViscosity.GetValue() / 10.7182881;
	}

	SetDensity();

	if(GetCheckedRadioButton(IDC_VLM1, IDC_VLM2)==IDC_VLM1) m_bVLM1 = true;
	else m_bVLM1 = false;

	SetWingLoad();
}


void CWPolarAnalysis::SetWPolarName()
{
	if(!m_BAutoName) return;
	CString str, strong;

	CMainFrame* pFrame = (CMainFrame*)m_pParent;

	if (m_Type==1)
	{
		GetSpeedUnit(str, pFrame->m_SpeedUnit);
		m_WPolarName.Format("T1-%.1f ", m_QInf * pFrame->m_mstoUnit);
		m_WPolarName += str;
	}
	else if(m_Type==2)
	{
		GetWeightUnit(str, pFrame->m_WeightUnit);
		m_WPolarName.Format("T2-%.3f ", m_Weight*pFrame->m_kgtoUnit);
		m_WPolarName += str;
	}
	else if(m_Type==4)	
	{
		m_WPolarName.Format("T4-%.3f°", m_Alpha);
	}

	if(m_AnalysisType==1) m_WPolarName += "-LLT";
	else if(m_AnalysisType==2) 
	{
		if(m_bVLM1)	m_WPolarName += "-VLM1";
		else		m_WPolarName += "-VLM2";
	}
	else if(m_AnalysisType==3) m_WPolarName += "-Panel";

	if(m_bThinSurfaces && m_AnalysisType==3) 
	{
		m_WPolarName += "-Thin";
	}
	
	GetLengthUnit(str, pFrame->m_LengthUnit);
	strong.Format("-%6.2f", m_XCmRef*pFrame->m_mtoUnit);
	m_WPolarName += strong + str;

	if(abs(m_Beta) > .001)
	{
		strong.Format("-b%.2f°", m_Beta);
		m_WPolarName += strong;
	}

	if(m_bTiltedGeom) 
	{
		m_WPolarName += "-TG";
		if(m_bWakeRollUp) m_WPolarName += "-W";
	}

	if(!m_bViscous) {
		m_WPolarName += "-Inviscid";
	}

	if(m_bGround) {
		strong.Format("%.2f", m_Height),
		m_WPolarName += "-G"+strong;
	}
	m_ctrlWPolarName.SetWindowText(m_WPolarName);
}


void CWPolarAnalysis::OnOK() 
{
	CWPolar * pWPolarNew;

	m_ctrlWPolarName.GetWindowText(m_WPolarName);

	int LineLength = m_WPolarName.GetLength();
	if(!LineLength) 
	{
		AfxMessageBox("Must enter a name", MB_OK);
		GetDlgItem(IDC_WPOLARNAME)->SetFocus();
		return;
	}
	else
	{
		int size = (int)m_poaXPolar->GetSize();
		for (int j=0; j<size; j++)
		{
			pWPolarNew = (CWPolar*)m_poaXPolar->GetAt(j);
			if (pWPolarNew->m_PlrName == m_WPolarName &&
				pWPolarNew->m_UFOName  == m_UFOName){
				AfxMessageBox("The polar's name already exists", MB_OK);
//				m_ctrlWPolarName.SetSel(0,-1);
//				m_ctrlWPolarName.SetFocus();
				return;
			}
		}
	}
/* TODO : restore
	if(!m_bWakeRollUp && m_AnalysisType==3)
	{
		m_TotalWakeLength = 100.0;
		m_NXWakePanels    = 1;
	}*/
	CDialog::OnOK();
}


void CWPolarAnalysis::OnKillFocusQInf() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pParent;
	m_QInf = m_ctrlQInf.GetValue() / pFrame->m_mstoUnit;
	SetReynolds();
	SetWPolarName();
}


void CWPolarAnalysis::OnKillFocusWeight() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pParent;
	m_Weight = abs(m_ctrlWeight.GetValue()) /pFrame->m_kgtoUnit;
	SetWingLoad();
	SetReynolds();
	SetWPolarName();
}


void CWPolarAnalysis::OnKillFocusXCmRef() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pParent;
	m_XCmRef = m_ctrlXCmRef.GetValue() /pFrame->m_mtoUnit;
	SetWPolarName();	
}


void CWPolarAnalysis::OnKillFocusAlpha() 
{
	m_Alpha = m_ctrlAlpha.GetValue();
	SetWPolarName();
}

void CWPolarAnalysis::OnKillFocusBeta() 
{
	m_Beta = m_ctrlBeta.GetValue();
	SetWPolarName();
}


void CWPolarAnalysis::OnKillFocusHeight()
{
	CMainFrame* pFrame = (CMainFrame*)m_pParent;
	m_Height = m_ctrlHeight.GetValue() /pFrame->m_mtoUnit;
	SetWPolarName();
}

void CWPolarAnalysis::OnType() 
{	
	int sel = GetCheckedRadioButton(IDC_WTYPE1, IDC_WTYPE4);
	if (sel == IDC_WTYPE1) 
	{
		m_Type = 1;
	}
	else if(sel == IDC_WTYPE2) 
	{
		m_Type = 2;
	}
	else if(sel == IDC_WTYPE4) 
	{
		m_Type = 4;
	}
	EnableControls();
	SetReynolds();
	SetWPolarName();
}


void CWPolarAnalysis::CheckType()
{
	switch (m_Type){
		case 1:{
			CheckRadioButton(IDC_WTYPE1, IDC_WTYPE4, IDC_WTYPE1);
			break;
		}
		case 2:{
			CheckRadioButton(IDC_WTYPE1, IDC_WTYPE4, IDC_WTYPE2);
			break;
		}
		case 4:{
			CheckRadioButton(IDC_WTYPE1, IDC_WTYPE4, IDC_WTYPE4);
			break;
		}
		default:{
			CheckRadioButton(IDC_WTYPE1, IDC_WTYPE4, IDC_WTYPE1);
			break;
		}
	}
}


void CWPolarAnalysis::EnableControls()
{
	switch (m_Type)
	{
		case 1:
		{
			m_ctrlQInf.EnableWindow(true);
			m_ctrlAlpha.EnableWindow(false);
			break;
		}
		case 2:
		{
			m_ctrlQInf.EnableWindow(false);
			m_ctrlRRe.SetWindowText(" ");
			m_ctrlSRe.SetWindowText(" ");
			m_ctrlAlpha.EnableWindow(false);
			break;
		}
		case 4:
		{
			m_ctrlQInf.EnableWindow(false);
			m_ctrlRRe.SetWindowText(" ");
			m_ctrlSRe.SetWindowText(" ");
			m_ctrlAlpha.EnableWindow(true);
			break;
		}
		default:
		{
			m_ctrlQInf.EnableWindow(true);
			break;
		}
	}
	if(!m_pPlane) m_ctrlMethod1.EnableWindow(TRUE);

	if (GetCheckedRadioButton(IDC_METHOD1, IDC_METHOD3)==IDC_METHOD1)
	{
		m_ctrlVLM1.EnableWindow(false);
		m_ctrlVLM2.EnableWindow(false);
		m_ctrlViscous.EnableWindow(false);
	}
	else if (GetCheckedRadioButton(IDC_METHOD1, IDC_METHOD3)==IDC_METHOD2)
	{ 
		m_ctrlVLM1.EnableWindow(true);
		m_ctrlVLM2.EnableWindow(true);
		m_ctrlViscous.EnableWindow(true);
	}
	else if (GetCheckedRadioButton(IDC_METHOD1, IDC_METHOD3)==IDC_METHOD3)
	{
		m_ctrlVLM1.EnableWindow(false);
		m_ctrlVLM2.EnableWindow(false);
		m_ctrlViscous.EnableWindow(true);
	}
}


void CWPolarAnalysis::SetReynolds()
{
	CString str, strUnit;
	CMainFrame* pFrame = (CMainFrame*)m_pParent;
	GetSpeedUnit(strUnit, pFrame->m_SpeedUnit);
	
	if (m_Type ==1){
		double RRe = m_pWing->m_TChord[0] * m_QInf/m_Viscosity;
		ReynoldsFormat(str, RRe);
		m_ctrlRRe.SetWindowText(str);

		double SRe = m_pWing->m_TChord[m_pWing->m_NPanel] * m_QInf/m_Viscosity;
		ReynoldsFormat(str, SRe);
		m_ctrlSRe.SetWindowText(str);

		m_ctrlReRoot.SetWindowText("Root Re =");
		m_ctrlReTip.SetWindowText("Tip Re =");
		m_ctrlQInfStat.SetWindowText(" ");
		m_ctrlQInfCl.SetWindowText(" ");
	}
	else if (m_Type ==2){
		double QCl =  sqrt(2.* 9.81 /m_Density* m_Weight /m_pWing->m_Area) 
					* pFrame->m_mstoUnit;
		str.Format("%5.2f", QCl);
		str += strUnit;
		m_ctrlQInfCl.SetWindowText(str);
		m_ctrlQInfStat.SetWindowText("Qinf.sqrt(Cl) =");
		
		double RRe = m_pWing->m_TChord[0] * QCl/m_Viscosity;
		ReynoldsFormat(str, RRe);
		m_ctrlRRe.SetWindowText(str);

		double SRe = m_pWing->m_TChord[m_pWing->m_NPanel] * QCl/m_Viscosity;
		ReynoldsFormat(str, SRe);
		m_ctrlSRe.SetWindowText(str);

		m_ctrlReRoot.SetWindowText("Root Re.sqrt(Cl) =");
		m_ctrlReTip.SetWindowText("Tip Re.sqrt(Cl) =");
	}
	else if (m_Type ==4){
		m_ctrlQInfStat.SetWindowText(" ");
		m_ctrlQInfCl.SetWindowText(" ");
		m_ctrlRRe.SetWindowText(" ");
		m_ctrlSRe.SetWindowText(" ");
		m_ctrlReRoot.SetWindowText(" ");
		m_ctrlReTip.SetWindowText(" ");
	}
}


void CWPolarAnalysis::SetWingLoad()
{
	CString str;
	CMainFrame* pFrame = (CMainFrame*)m_pParent;
	m_WingLoad = m_Weight/m_pWing->m_Area;//kg/dm²

	str.Format("%7.3f",   m_WingLoad * pFrame->m_kgtoUnit / pFrame->m_m2toUnit);

	m_ctrlWingLoad.SetWindowText(str);
	m_ctrlWingLoad.Invalidate();
}


void CWPolarAnalysis::OnKillFocusViscosity() 
{
	ReadParams();
	SetReynolds();
}

void CWPolarAnalysis::OnKillFocusDensity() 
{
	ReadParams();
	SetReynolds();
}

void CWPolarAnalysis::OnUnit() 
{
	if(IDC_UNIT1 == GetCheckedRadioButton(IDC_UNIT1, IDC_UNIT2)){
		m_UnitType   = 1;
		m_ctrlViscosity.SetValue(m_Viscosity);
		m_ctrlSDensity.SetWindowText("kg/m3");
		m_ctrlSViscosity.SetWindowText("m²/s");
	}
	else {
		m_UnitType   = 2;
		m_ctrlViscosity.SetValue(m_Viscosity* 10.7182881);
		m_ctrlSDensity.SetWindowText("slugs/ft3");
		m_ctrlSViscosity.SetWindowText("ft²/s");
	}
	SetDensity();
}

void CWPolarAnalysis::SetDensity() 
{
	int exp, precision;
	if(IDC_UNIT1 == GetCheckedRadioButton(IDC_UNIT1, IDC_UNIT2)){
		exp = (int)log(m_Density);
		if(exp>1) precision = 1;
		else if(exp<-4) precision = 4;
		else precision = 3-exp;
		m_ctrlDensity.SetPrecision(precision);
		m_ctrlDensity.SetValue(m_Density);
	}
	else {
		exp = (int)log(m_Density* 0.00194122);
		if(exp>1) precision = 1;
		else if(exp<-4) precision = 4;
		else precision = 3-exp;
		m_ctrlDensity.SetPrecision(precision);
		m_ctrlDensity.SetValue(m_Density* 0.00194122);
	}	
}


void CWPolarAnalysis::OnAutoName() 
{
	if (m_ctrlAutoName.GetCheck()){
		m_BAutoName = TRUE;
		SetWPolarName();	
	}
	else {
		m_BAutoName = FALSE;
		m_ctrlWPolarName.SetSel(0,-1);
		m_ctrlWPolarName.SetFocus();
	}
}

 
void CWPolarAnalysis::OnSetFocusWPolarName() 
{
 	m_BAutoName = false;
 	m_ctrlAutoName.SetCheck(FALSE);
}

void CWPolarAnalysis::OnMethod() 
{
	if (GetCheckedRadioButton(IDC_METHOD1, IDC_METHOD3)==IDC_METHOD1)
	{
		m_bViscous      = true;
		m_bThinSurfaces = true;
		m_bWakeRollUp   = false;
		m_bTiltedGeom   = false;
		m_AnalysisType  = 1;
		m_ctrlViscous.SetCheck(TRUE);
		m_ctrlWakeRollUp.SetCheck(FALSE);
		m_ctrlTiltGeom.SetCheck(FALSE);
		m_ctrlTiltGeom.EnableWindow(false);
		m_ctrlBeta.EnableWindow(false);
//		m_ctrlThinSurfaces.SetCheck(TRUE);
//		m_ctrlThinSurfaces.EnableWindow(false);

//		m_ctrlWakeRollUp.EnableWindow(FALSE);
//		m_ctrlWakeParams.EnableWindow(FALSE);
	}
	else if (GetCheckedRadioButton(IDC_METHOD1, IDC_METHOD3)==IDC_METHOD2)
	{ 
		m_bThinSurfaces = true;
		m_AnalysisType=2;
		m_ctrlTiltGeom.EnableWindow(true);
		m_ctrlBeta.EnableWindow(true);
//		m_ctrlThinSurfaces.SetCheck(TRUE);
//		m_ctrlThinSurfaces.EnableWindow(false);

//		m_ctrlWakeRollUp.EnableWindow(TRUE);
//		if(m_bWakeRollUp)	m_ctrlWakeParams.EnableWindow(TRUE);
//		else				m_ctrlWakeParams.EnableWindow(FALSE);
	}
	else if (GetCheckedRadioButton(IDC_METHOD1, IDC_METHOD3)==IDC_METHOD3)
	{ 
		m_AnalysisType=3;
		m_ctrlTiltGeom.EnableWindow(true);
		m_bThinSurfaces = false;
		m_ctrlBeta.EnableWindow(true);
//		m_ctrlThinSurfaces.SetCheck(false);
//		m_ctrlThinSurfaces.EnableWindow(true);

//		m_ctrlWakeRollUp.EnableWindow(TRUE);
//		if(m_bWakeRollUp)	m_ctrlWakeParams.EnableWindow(TRUE);
//		else				m_ctrlWakeParams.EnableWindow(FALSE);

	}
	EnableControls();
	SetWPolarName();
}

void CWPolarAnalysis::OnVLMMethod() 
{
	if(GetCheckedRadioButton(IDC_VLM1,IDC_VLM2)==IDC_VLM1)	{
		m_bVLM1 = true;
//		m_ctrlWakeRollUp.EnableWindow(false);
	}
	else{
		m_bVLM1 = false;
//		m_ctrlWakeRollUp.EnableWindow(true);
	}
	SetWPolarName();
}


void CWPolarAnalysis::OnWakeRollUp() 
{
	if(m_ctrlWakeRollUp.GetCheck()) 
	{
		m_bWakeRollUp = true;
		m_ctrlWakeParams.EnableWindow(TRUE);
	}
	else
	{
		m_bWakeRollUp = false;
		m_ctrlWakeParams.EnableWindow(FALSE);
	}
	SetWPolarName();
}


void CWPolarAnalysis::OnTiltedGeom()
{
	if(m_ctrlTiltGeom.GetCheck()) 
	{
		m_bTiltedGeom = true;
//		m_ctrlWakeRollUp.EnableWindow(true);
//		if(m_bWakeRollUp)	m_ctrlWakeParams.EnableWindow(TRUE);
//		else				m_ctrlWakeParams.EnableWindow(FALSE);
	}
	else 
	{
		m_bTiltedGeom = false;
//		m_bWakeRollUp = false;
//		m_ctrlWakeRollUp.SetCheck(false);
//		m_ctrlWakeRollUp.EnableWindow(false);
	}

	SetWPolarName();
	EnableControls();
}


void CWPolarAnalysis::OnViscous()
{
	if(m_ctrlViscous.GetCheck()) m_bViscous = true;
	else                         m_bViscous = false;
	SetWPolarName();
	EnableControls();
}


void CWPolarAnalysis::OnGroundEffect()
{
	if(m_ctrlGroundEffect.GetCheck())
	{
		m_bGround = true;
		m_ctrlHeight.EnableWindow(TRUE);
	}
	else 
	{
		m_bGround = false;
		m_ctrlHeight.EnableWindow(FALSE);
	}
	SetWPolarName();
}


void CWPolarAnalysis::OnWakeParams()
{
	m_WakeParamsdlg.DoModal();

	m_WakePanelFactor = m_WakeParamsdlg.m_WakePanelFactor;
	m_TotalWakeLength = m_WakeParamsdlg.m_TotalWakeLength;
	m_NXWakePanels = m_WakeParamsdlg.m_NXWakePanels;
}

void CWPolarAnalysis::OnThinSurfaces()
{
	if(m_ctrlThinSurfaces.GetCheck()) m_bThinSurfaces = true;
	else                              m_bThinSurfaces = false;

	SetWPolarName();
}
