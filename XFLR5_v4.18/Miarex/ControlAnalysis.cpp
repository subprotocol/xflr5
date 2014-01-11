/****************************************************************************

    ControlAnalysis Class
	Copyright (C) 2008 André Deperrois xflr5@yahoo.com

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


#include "stdafx.h"
#include "../X-FLR5.h"
#include ".\ControlAnalysis.h"
#include "WPolar.h"
#include "../main/Mainfrm.h"
#include <math.h>

IMPLEMENT_DYNAMIC(CControlAnalysis, CDialog)
CControlAnalysis::CControlAnalysis(CWnd* pParent /*=NULL*/)
	: CDialog(CControlAnalysis::IDD, pParent)
{
	m_XCmRef   = 0.0;
	m_WingTilt = 0.0;
	m_StabTilt = 0.0;
	m_pWing  = NULL;
	m_pWing2 = NULL;
	m_pStab  = NULL;
	m_pFin   = NULL;
	m_RefAreaType = 1;

	memset(m_MinControl, 0, sizeof(m_MinControl));
	memset(m_MaxControl, 0, sizeof(m_MaxControl));
	memset(m_bActiveControl, 0, sizeof(m_bActiveControl));

	m_iSelection = 0;
	m_UnitType = 1;
	m_Density    = 1.225;
	m_Viscosity  = 1.5e-5;

	m_bActiveControl[0] = true;
	m_MinControl[0] =  0.00;//xcmref
	m_MaxControl[0] =  0.100;

	m_bActiveControl[1] = false;
	m_MinControl[1] = 0.0;//wing tilt
	m_MaxControl[1] = 0.0;

	m_bActiveControl[2] = false;
	m_MinControl[2] =   0.0;//stab tilt
	m_MaxControl[2] =   0.0;

	m_bActiveControl[3] = false;
	m_MinControl[3] = 0.0;//flap angle
	m_MaxControl[3] = 0.0;

	m_bActiveControl[4] = false;
	m_MinControl[4] = 0.0;//flap angle
	m_MaxControl[4] = 0.0;

	m_bViscous = true;
	m_QInf = 10.0;
	m_Weight = 1.0;
	m_Type = 5;

	m_SymbolFont.CreatePointFont(100, "Symbol");
	
	m_WPolarName = "T5";
}

CControlAnalysis::~CControlAnalysis()
{
}

void CControlAnalysis::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONTROLLIST, m_ctrlControlList);
	DDX_Control(pDX, IDC_WPOLARNAME, m_ctrlWPolarName);
//	DDX_Control(pDX, IDC_WINGNAME, m_ctrlUFOName);
	DDX_Control(pDX, IDC_DENSITY, m_ctrlDensity);
	DDX_Control(pDX, IDC_VISCOSITY, m_ctrlViscosity);
	DDX_Control(pDX, IDC_SVISCOSITY, m_ctrlSViscosity);
	DDX_Control(pDX, IDC_SDENSITY, m_ctrlSDensity);
	DDX_Control(pDX, IDC_RHO, m_ctrlRho);
	DDX_Control(pDX, IDC_NU, m_ctrlNu);
	DDX_Control(pDX, IDC_VISCOUS, m_ctrlViscous);
	DDX_Control(pDX, IDC_SPEEDUNIT, m_ctrlSpeedUnit);
	DDX_Control(pDX, IDC_QINF, m_ctrlQInf);
	DDX_Control(pDX, IDC_WEIGHT, m_ctrlWeight);
	DDX_Control(pDX, IDC_WEIGHTUNIT, m_ctrlWeightUnit);
	DDX_Control(pDX, IDC_WTYPE5, m_ctrlWType5);
	DDX_Control(pDX, IDC_WTYPE6, m_ctrlWType6);
}


BEGIN_MESSAGE_MAP(CControlAnalysis, CDialog)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDC_UNIT1, OnUnit)
	ON_BN_CLICKED(IDC_UNIT2, OnUnit)
	ON_NOTIFY(NM_CLICK, IDC_CONTROLLIST, OnNMClickCtrlList)
	ON_NOTIFY(NM_RCLICK, IDC_CONTROLLIST, OnNMRClickCtrlList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_CONTROLLIST, OnLvnEndLabelEditControlList)
	ON_BN_CLICKED(IDC_AREA1, OnAreaType)
	ON_BN_CLICKED(IDC_AREA2, OnAreaType)
	ON_BN_CLICKED(IDC_WTYPE5, OnType)
	ON_BN_CLICKED(IDC_WTYPE6, OnType)
	ON_BN_CLICKED(IDC_VISCOUS, OnViscous)
	ON_EN_KILLFOCUS(IDC_VISCOSITY, ReadParams)
	ON_EN_KILLFOCUS(IDC_DENSITY, ReadParams)
	ON_EN_KILLFOCUS(IDC_CONTROLLIST, OnKillFocus)
	ON_EN_KILLFOCUS(IDC_WTYPE5, OnKillFocus)
	ON_EN_KILLFOCUS(IDC_WTYPE6, OnKillFocus)
	ON_EN_KILLFOCUS(IDC_WEIGHT, OnKillFocus)
	ON_EN_KILLFOCUS(IDC_QINF, OnKillFocus)
END_MESSAGE_MAP()


BOOL CControlAnalysis::OnInitDialog()
{
	CDialog::OnInitDialog();
	CMainFrame *pFrame = (CMainFrame*)m_pMainFrame;

	m_ctrlRho.SetFont(&m_SymbolFont);
	m_ctrlNu.SetFont(&m_SymbolFont);
	m_ctrlDensity.SetPrecision(5);
	m_ctrlViscosity.SetPrecision(3);

	CString str;
	GetSpeedUnit(str, pFrame->m_SpeedUnit);
	m_ctrlSpeedUnit.SetWindowText(str);
	m_ctrlQInf.SetValue(m_QInf*pFrame->m_mstoUnit);

	GetWeightUnit(str, pFrame->m_WeightUnit);
	m_ctrlWeightUnit.SetWindowText(str);
	m_ctrlWeight.SetValue(m_Weight*pFrame->m_kgtoUnit);

	if(m_Type==5)      CheckRadioButton(IDC_WTYPE5, IDC_WTYPE6, IDC_WTYPE5);
	else if(m_Type==6) CheckRadioButton(IDC_WTYPE5, IDC_WTYPE6, IDC_WTYPE6);
	else               CheckRadioButton(IDC_WTYPE5, IDC_WTYPE6, IDC_WTYPE5);

	if(m_UnitType==1) CheckRadioButton(IDC_UNIT1, IDC_UNIT2, IDC_UNIT1);
	else              CheckRadioButton(IDC_UNIT1, IDC_UNIT2, IDC_UNIT2);
	OnUnit();

	if(m_RefAreaType==1) CheckRadioButton(IDC_AREA1, IDC_AREA2, IDC_AREA1);
	else                 CheckRadioButton(IDC_AREA1, IDC_AREA2, IDC_AREA2);

	CRect CltRect;
	m_ctrlControlList.GetStyle();
	m_ctrlControlList.GetClientRect(&CltRect);
	int w = CltRect.Width();
	int w1 = (int)((w-215)/3);
	m_ctrlControlList.InsertColumn(0," ",LVCFMT_RIGHT, 15);
	m_ctrlControlList.InsertColumn(1,"Control Name",LVCFMT_LEFT, 200);
	m_ctrlControlList.InsertColumn(2,"Active (1/0)",LVCFMT_CENTER,w1);
	m_ctrlControlList.InsertColumn(3,"Min",LVCFMT_RIGHT,w1);
	m_ctrlControlList.InsertColumn(4,"Max",LVCFMT_RIGHT,w1);
	m_ctrlControlList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_ctrlControlList.m_nColumns = 5;

	for (int i=0; i<100; i++) m_ctrlControlList.m_iPrecision[i] = 4;

//	m_ctrlUFOName.SetWindowText(m_UFOName);
	m_ctrlWPolarName.SetWindowText(m_WPolarName);
	FillControlList();
	SetWPolarName();
	m_ctrlControlList.SetFocus();
	return FALSE;
}



void CControlAnalysis::ReadSectionData(int sel)
{
	double d;
	int res, k;
	CString strong;
	CMainFrame *pFrame = (CMainFrame*)m_pMainFrame;

	strong = m_ctrlControlList.GetItemText(sel,2);
	strong.Replace(" ","");
	res = sscanf(strong, "%d",&k);
	if(res==1)
	{
		if(k) m_bActiveControl[sel] = true; else m_bActiveControl[sel] = false;
	}
	strong = m_ctrlControlList.GetItemText(sel,3);
	strong.Replace(" ","");
	res = sscanf(strong, "%lf",&d);
	if(res==1) m_MinControl[sel] =d;
	if(sel==0) m_MinControl[sel] /= pFrame->m_mtoUnit;

	strong = m_ctrlControlList.GetItemText(sel,4);
	strong.Replace(" ","");
	res = sscanf(strong, "%lf",&d);
	if(res==1) m_MaxControl[sel] =d;
	if(sel==0) m_MaxControl[sel] /= pFrame->m_mtoUnit;
}


void CControlAnalysis::OnUnit() 
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
void CControlAnalysis::SetDensity() 
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

bool CControlAnalysis::ReadData()
{
	double d;
	int i, res, k;
	CString strong;
	CMainFrame *pFrame = (CMainFrame*)m_pMainFrame;

	for(i=0; i<m_nControls; i++)
	{
		strong = m_ctrlControlList.GetItemText(i,2);
		res = sscanf(strong, "%d",&k);
		if(res==1) 
		{
			if(k) m_bActiveControl[i] = true;
			else  m_bActiveControl[i] = false;
		}
		else
		{
			strong.Format("Couldn't read activation parameter for control %d", i);
			AfxMessageBox(strong);
			return false;
		}

		strong = m_ctrlControlList.GetItemText(i,3);
		strong.Replace(" ","");
		res = sscanf(strong, "%lf",&d);
		if(res==1) m_MinControl[i] =d;
		else
		{
			strong.Format("Unrecognized minimum value for control %d", i);
			AfxMessageBox(strong);
			return false;
		}

		strong = m_ctrlControlList.GetItemText(i,4);
		strong.Replace(" ","");
		res = sscanf(strong, "%lf",&d);
		if(res==1) m_MaxControl[i] =d;
		else
		{
			strong.Format("Unrecognized maximum value for control %d", i);
			AfxMessageBox(strong);
			return false;
		}
	}

	for(i=m_nControls; i<4*MAXCONTROLS; i++)
	{
		m_bActiveControl[i]=false;
	}

	m_MinControl[0] /= pFrame->m_mtoUnit;
	m_MaxControl[0] /= pFrame->m_mtoUnit;
	return true;
}


void CControlAnalysis::OnNMClickCtrlList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem == -1 || pNMListView->iSubItem == -1)
	{
		*pResult =0;
		return ;
	}
	m_iSelection = pNMListView->iItem;
	*pResult = 0;
}



void CControlAnalysis::OnNMRClickCtrlList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	CMenu menu;
	if (menu.LoadMenu(IDR_CTXWNGMENU)){
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	pt.x, pt.y, this); 
	}
}


void CControlAnalysis::OnLvnEndLabelEditControlList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;
	if(pDispInfo->item.iItem == -1 || pDispInfo->item.iSubItem == -1 )
	{
		*pResult =0;
		return ;
	}
//	m_iSelection = pDispInfo->item.iItem;
	POSITION pos = m_ctrlControlList.GetFirstSelectedItemPosition();
	m_iSelection = m_ctrlControlList.GetNextSelectedItem(pos);

	//first read the section
	
	ReadSectionData(m_iSelection);

	SetWPolarName();
	FillControlList();
}

void CControlAnalysis::OnOK()
{
	if(!ReadData()) return;

	ReadParams();

	bool bActive = false;

	for(int i=0; i<m_nControls; i++)
	{
		bActive = bActive || m_bActiveControl[i];
		if(bActive) break;
	}
	if(!bActive)
	{
		if(IDYES!=AfxMessageBox("No Active Control. Continue ?", MB_YESNOCANCEL)) return;
	}

	CWPolar * pWPolarNew;
	m_ctrlWPolarName.GetWindowText(m_WPolarName);

	int LineLength = m_WPolarName.GetLength();
	if(!LineLength) {
		AfxMessageBox("Must enter a name", MB_OK);
		GetDlgItem(IDC_WPOLARNAME)->SetFocus();
		return;
	}
	else{
		int size = (int)m_poaXPolar->GetSize();
		for (int j=0; j<size; j++){
			pWPolarNew = (CWPolar*)m_poaXPolar->GetAt(j);
			if (pWPolarNew->m_PlrName == m_WPolarName &&
				pWPolarNew->m_UFOName  == m_UFOName){
				AfxMessageBox("The polar's name already exists", MB_OK);
				return;
			}
		}
	}

	if(m_ctrlViscous.GetCheck()) m_bViscous = true;
	m_QInf = m_ctrlQInf.GetValue();

	if(GetCheckedRadioButton(IDC_WTYPE5, IDC_WTYPE6)==IDC_WTYPE5)      m_Type=5;
	else if(GetCheckedRadioButton(IDC_WTYPE5, IDC_WTYPE6)==IDC_WTYPE6) m_Type=6;


	CDialog::OnOK();
}


void CControlAnalysis::ReadParams()
{
	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;

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

	m_QInf   = abs(m_ctrlQInf.GetValue()) / pFrame->m_mstoUnit;
	m_Weight = abs(m_ctrlWeight.GetValue()) /pFrame->m_kgtoUnit;
	
	SetDensity();
}


void CControlAnalysis::FillControlList()
{
	CString str, strong;
	int i;
	CMainFrame *pFrame = (CMainFrame*)m_pMainFrame;
	GetLengthUnit(str, pFrame->m_LengthUnit);

	m_ctrlControlList.DeleteAllItems();

	m_ctrlControlList.InsertItem(0, "1");
	m_ctrlControlList.SetItemText(0,1,"XCmRef ("+str+")");
	if(m_bActiveControl[0])	m_ctrlControlList.SetItemText(0,2,"1");
	else                    m_ctrlControlList.SetItemText(0,2,"0");

	strong.Format("%8.3f", m_MinControl[0]*pFrame->m_mtoUnit);
	m_ctrlControlList.SetItemText(0,3,strong);
	strong.Format("%8.3f", m_MaxControl[0]*pFrame->m_mtoUnit);
	m_ctrlControlList.SetItemText(0,4,strong);

	m_nControls = 1;
	if(m_pPlane)
	{
		m_ctrlControlList.InsertItem(1, "2");
		m_ctrlControlList.SetItemText(1,1,"Wing Tilt (°)");
		if(m_bActiveControl[1])	m_ctrlControlList.SetItemText(1,2,"1");
		else                    m_ctrlControlList.SetItemText(1,2,"0");
		m_ctrlControlList.SetItemText(1,3,"-1.0°");
		m_ctrlControlList.SetItemText(1,4,"+1.0");
		strong.Format("%6.1f", m_MinControl[1]);
		m_ctrlControlList.SetItemText(1,3,strong);
		strong.Format("%6.1f", m_MaxControl[1]);
		m_ctrlControlList.SetItemText(1,4,strong);
		++m_nControls;

		if(m_pStab)
		{
			m_ctrlControlList.InsertItem(2, "3");
			m_ctrlControlList.SetItemText(2,1,"Elevator Tilt (°)");
			if(m_bActiveControl[2])	m_ctrlControlList.SetItemText(2,2,"1");
			else                    m_ctrlControlList.SetItemText(2,2,"0");
			strong.Format("%6.1f", m_MinControl[2]);
			m_ctrlControlList.SetItemText(2,3,strong);
			strong.Format("%6.1f", m_MaxControl[2]);
			m_ctrlControlList.SetItemText(2,4,strong);
			++m_nControls;
		}
	}

	for(i=0; i<m_pWing->m_nFlaps; i++)
	{
		strong.Format("%d", i+m_nControls+1);
		m_ctrlControlList.InsertItem(i+m_nControls, strong);
		strong.Format("Wing Flap angle %d (°)", i+1);
		m_ctrlControlList.SetItemText(i+m_nControls,1,strong);
		if(m_bActiveControl[i+m_nControls]) m_ctrlControlList.SetItemText(i+m_nControls,2,"1");
		else                                m_ctrlControlList.SetItemText(i+m_nControls,2,"0");
		strong.Format("%6.1f", m_MinControl[i+m_nControls]);
		m_ctrlControlList.SetItemText(i+m_nControls,3,strong);
		strong.Format("%6.1f", m_MaxControl[i+m_nControls]);
		m_ctrlControlList.SetItemText(i+m_nControls,4,strong);
	}
	m_nControls += m_pWing->m_nFlaps;

	if(m_pStab)
	{
		for(i=0; i<m_pStab->m_nFlaps; i++)
		{
			strong.Format("%d", i+m_nControls+1);
			m_ctrlControlList.InsertItem(i+m_nControls, strong);
			strong.Format("Elevator Flap %d (°)", i+1);
			m_ctrlControlList.SetItemText(i+m_nControls,1,strong);
			if(m_bActiveControl[i+m_nControls]) m_ctrlControlList.SetItemText(i+m_nControls,2,"1");
			else                                  m_ctrlControlList.SetItemText(i+m_nControls,2,"0");
			strong.Format("%6.1f", m_MinControl[i+m_nControls]);
			m_ctrlControlList.SetItemText(i+m_nControls,3,strong);
			strong.Format("%6.1f", m_MaxControl[i+m_nControls]);
			m_ctrlControlList.SetItemText(i+m_nControls,4,strong);
		}
		m_nControls += m_pStab->m_nFlaps;
	}
	bool bViscous=true;
	for(i=3; i<m_nControls; i++)
	{
		if(m_bActiveControl[i]) bViscous = false;
	}
	if(!bViscous) 
	{
		m_ctrlViscous.SetCheck(false);
		m_ctrlViscous.EnableWindow(false);
		m_bViscous = false;
	}
	else
	{
		if(m_bViscous) m_ctrlViscous.SetCheck(true);
		m_ctrlViscous.EnableWindow(true);
	}
}	

void CControlAnalysis::OnType()
{
	if(GetCheckedRadioButton(IDC_WTYPE5, IDC_WTYPE6)== IDC_WTYPE5)	m_Type=5;
	else                                                            m_Type=6;
	SetWPolarName();
}

void CControlAnalysis::SetWPolarName()
{
	CString str, strong;
	int i, nCtrl;
	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;

	if (GetCheckedRadioButton(IDC_WTYPE5, IDC_WTYPE6)== IDC_WTYPE5)
	{
		GetSpeedUnit(str, pFrame->m_SpeedUnit);
		m_WPolarName.Format("T5-%.1f ", m_QInf * pFrame->m_mstoUnit);
		m_WPolarName += str;
	}
	else 
	{
		GetWeightUnit(str, pFrame->m_WeightUnit);
		m_WPolarName.Format("T6-%.3f ", m_Weight*pFrame->m_kgtoUnit);
		m_WPolarName += str;
	}

	if(m_bActiveControl[0])
	{
		GetLengthUnit(str, pFrame->m_LengthUnit);
		strong.Format("-CG(%.2f/%.2f)", m_MinControl[0]*pFrame->m_mtoUnit, m_MaxControl[0]*pFrame->m_mtoUnit);
		m_WPolarName += strong + str ;
	}
	else
	{
		GetLengthUnit(str, pFrame->m_LengthUnit);
		strong.Format("-CG(%.2f)", m_MinControl[0]*pFrame->m_mtoUnit);
		m_WPolarName += strong + str;
	}

	nCtrl = 1;//CG control is always present

	if(m_pPlane)
	{
		if(m_bActiveControl[1] && m_pPlane)
		{
			strong.Format("-W(%.1f°/%.1f°)", m_MinControl[1], m_MaxControl[1]);
			m_WPolarName += strong;
		}
		nCtrl++;
	}

	if(m_pPlane && m_pStab)
	{
		if(m_bActiveControl[2])
		{
			strong.Format("-E(%.1f°/%.1f°)", m_MinControl[2], m_MaxControl[2]);
			m_WPolarName += strong;
		}
		nCtrl++;
	}

	for(i=0; i<m_pWing->m_nFlaps; i++)
	{
		if(m_bActiveControl[i+nCtrl])
		{
			strong.Format("-WF%d(%.1f°/%.1f°)", i+1, m_MinControl[i+nCtrl], m_MaxControl[i+nCtrl]);
			m_WPolarName += strong;
		}
	}
	nCtrl += m_pWing->m_nFlaps;

	if(m_pPlane && m_pStab)
	{
		for(i=0; i<m_pStab->m_nFlaps; i++)
		{
			if(m_bActiveControl[i+nCtrl])
			{
				strong.Format("-EF%d(%.1f°/%.1f°)", i+1, m_MinControl[i+nCtrl], m_MaxControl[i+nCtrl]);
				m_WPolarName += strong;
			}
		}
	}
	if(m_RefAreaType==2) m_WPolarName += "-Proj_area";
	m_ctrlWPolarName.SetWindowText(m_WPolarName);
}




void CControlAnalysis::OnViscous()
{
	if(m_ctrlViscous.GetCheck()) m_bViscous = true;
	else                         m_bViscous = false;
	SetWPolarName();
}

BOOL CControlAnalysis::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN )
	{ 
		CWnd* pWnd = GetFocus();
		if(pMsg->wParam == VK_RETURN)
		{
			if(GetDlgItem(IDCANCEL) != pWnd && GetDlgItem(IDOK) != pWnd)
			{
				// we don't want to exit on OK on CEdit
				ReadData();
				ReadParams();
				SetWPolarName();
				GetDlgItem(IDOK)->SetFocus();
				return true;
			}
			else if(GetDlgItem(IDOK) == pWnd ) OnOK();
			else if(GetDlgItem(IDCANCEL) == pWnd ) OnCancel();
			return true;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CControlAnalysis::OnKillFocus()
{
	ReadParams();
	ReadData();
	SetWPolarName();
}

void CControlAnalysis::OnAreaType() 
{
	if(GetCheckedRadioButton(IDC_AREA1, IDC_AREA2)==IDC_AREA1) m_RefAreaType = 1;
	else                                                       m_RefAreaType = 2;
	SetWPolarName();
}


