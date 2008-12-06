/****************************************************************************

    CPlaneDlg Class
	Copyright (C) 2006-2008 André Deperrois xflr5@yahoo.com

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
// Plane.cpp : implementation file
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "ImportWingDlg.h"
#include "PlaneDlg.h"
#include ".\plane.h"
#include ".\planedlg.h"
#include "WingDlg.h"

CObArray *	CPlaneDlg::s_poaWing = NULL;
CObArray *	CPlaneDlg::s_poaBody  = NULL;
CWnd * CPlaneDlg::s_pMiarex = NULL;
CWnd * CPlaneDlg::s_pMainFrame = NULL;


CPlaneDlg::CPlaneDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlaneDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlaneDlg)
	//}}AFX_DATA_INIT
	
	m_pPlane = NULL;

	m_bCheckName    = true;
	m_bChanged      = false;

}


void CPlaneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SYMFIN, m_ctrlSymFin);
	DDX_Control(pDX, IDC_SURF3, m_ctrlSurf3);
	DDX_Control(pDX, IDC_SURF2, m_ctrlSurf2);
	DDX_Control(pDX, IDC_SURF1, m_ctrlSurf1);
	DDX_Control(pDX, IDC_LEN1, m_ctrlLen1);
	DDX_Control(pDX, IDC_LEN2, m_ctrlLen2);
	DDX_Control(pDX, IDC_LEN3, m_ctrlLen3);
	DDX_Control(pDX, IDC_LEN4, m_ctrlLen4);
	DDX_Control(pDX, IDC_LEN5, m_ctrlLen5);
	DDX_Control(pDX, IDC_LEN6, m_ctrlLen6);
	DDX_Control(pDX, IDC_LEN7, m_ctrlLen7);
	DDX_Control(pDX, IDC_LEN8, m_ctrlLen8);
	DDX_Control(pDX, IDC_LEN9, m_ctrlLen9);
	DDX_Control(pDX, IDC_LEN10, m_ctrlLen10);
	DDX_Control(pDX, IDC_LEN11, m_ctrlLen11);
	DDX_Control(pDX, IDC_LEN12, m_ctrlLen12);
	DDX_Control(pDX, IDC_LEN13, m_ctrlLen13);
	DDX_Control(pDX, IDC_VOLUME, m_ctrlVolume);
	DDX_Control(pDX, IDC_VOLUMEUNIT, m_ctrlVolumeUnit);
	DDX_Control(pDX, IDC_WINGSPAN, m_ctrlWingSpan);
	DDX_Control(pDX, IDC_WINGSURFACE, m_ctrlWingSurface);
	DDX_Control(pDX, IDC_WINGTILT, m_ctrlWingTilt);
	DDX_Control(pDX, IDC_WINGTILT2, m_ctrlWingTilt2);
	DDX_Control(pDX, IDC_DEFINEWING2, m_ctrlDefineWing2);
	DDX_Control(pDX, IDC_IMPORTWING2, m_ctrlImportWing2);
	DDX_Control(pDX, IDC_EXPORTWING2, m_ctrlExportWing2);
	DDX_Control(pDX, IDC_DEFINEFIN, m_ctrlDefineFin);
	DDX_Control(pDX, IDC_DEFINESTAB, m_ctrlDefineStab);
	DDX_Control(pDX, IDC_PLANENAME, m_ctrlPlaneName);
	DDX_Control(pDX, IDC_STABVOLUME, m_ctrlStabVolume);
	DDX_Control(pDX, IDC_FINSURFACE, m_ctrlFinSurface);
	DDX_Control(pDX, IDC_STABLEVERARM, m_ctrlStabLeverArm);
	DDX_Control(pDX, IDC_STABSURFACE, m_ctrlStabSurface);
	DDX_Control(pDX, IDC_XLEFIN, m_ctrlXLEFin);
	DDX_Control(pDX, IDC_YLEFIN, m_ctrlYLEFin);
	DDX_Control(pDX, IDC_ZLEFIN, m_ctrlZLEFin);
	DDX_Control(pDX, IDC_STABTILT, m_ctrlStabTilt);
	DDX_Control(pDX, IDC_ZLESTAB, m_ctrlZLEStab);
	DDX_Control(pDX, IDC_XLESTAB, m_ctrlXLEStab);
	DDX_Control(pDX, IDC_XLEWING2, m_ctrlXLEWing2);
	DDX_Control(pDX, IDC_ZLEWING2, m_ctrlZLEWing2);
	DDX_Control(pDX, IDC_XLEWING, m_ctrlXLEWing);
	DDX_Control(pDX, IDC_ZLEWING, m_ctrlZLEWing);
	DDX_Control(pDX, IDC_XBODY, m_ctrlXBody);
	DDX_Control(pDX, IDC_ZBODY, m_ctrlZBody);
	DDX_Control(pDX, IDC_STAB, m_ctrlStabCheck);
	DDX_Control(pDX, IDC_FIN, m_ctrlFinCheck);
	DDX_Control(pDX, IDC_VLMTOTALPANELS, m_ctrlVLMTotalPanels);
	DDX_Control(pDX, IDC_DOUBLEFIN, m_ctrlDoubleFin);
	DDX_Control(pDX, IDC_FINTILT, m_ctrlFinTilt);
	DDX_Control(pDX, IDC_BIPLANE, m_ctrlBiplane);
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_BODYLIST, m_ctrlBodyList);
	DDX_Control(pDX, IDC_BODY, m_ctrlBody);
}


BEGIN_MESSAGE_MAP(CPlaneDlg, CDialog)
	//{{AFX_MSG_MAP(CPlaneDlg)
	ON_BN_CLICKED(IDC_FIN, OnFin)
	ON_BN_CLICKED(IDC_STAB, OnStab)
	ON_BN_CLICKED(IDC_DEFINEFIN, OnDefineFin)
	ON_BN_CLICKED(IDC_DEFINESTAB, OnDefineStab)
	ON_EN_KILLFOCUS(IDC_XLESTAB, OnChanged)
	ON_BN_CLICKED(IDC_IMPORTWING, OnImportWing)
	ON_EN_UPDATE(IDC_PLANENAME, OnPlaneName)
	ON_BN_CLICKED(IDC_SYMFIN, OnSymFin)
	ON_EN_KILLFOCUS(IDC_WINGTILT, OnChanged)
	ON_EN_KILLFOCUS(IDC_STABTILT, OnChanged)
	ON_EN_KILLFOCUS(IDC_FINTILT, OnChanged)
	ON_EN_KILLFOCUS(IDC_ZLESTAB, OnChanged)
	ON_EN_KILLFOCUS(IDC_XLEFIN, OnChanged)
	ON_EN_KILLFOCUS(IDC_ZLEFIN, OnChanged)
	ON_EN_KILLFOCUS(IDC_XBODY, OnChanged)
	ON_EN_KILLFOCUS(IDC_ZBODY, OnChanged)
	ON_EN_KILLFOCUS(IDC_PLANENAME, OnPlaneName)
	ON_BN_CLICKED(IDC_EXPORTWING, OnExportWing)
	ON_BN_CLICKED(IDC_DOUBLEFIN, OnDoubleFin)
	ON_BN_CLICKED(IDC_BIPLANE, OnBiplane)
	ON_BN_CLICKED(IDC_DEFINEWING, OnDefineWing)
	ON_BN_CLICKED(IDC_DEFINEWING2, OnDefineWing2)
	ON_BN_CLICKED(IDC_IMPORTWING2, OnImportWing2)
	ON_BN_CLICKED(IDC_EXPORTWING2, OnExportWing2)
	ON_BN_CLICKED(IDC_BODY, OnBodyCheck)
	ON_CBN_SELCHANGE(IDC_BODYLIST, OnSelChangeBodyList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlaneDlg message handlers


BOOL CPlaneDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CMainFrame *pFrame = (CMainFrame*)s_pMainFrame;

	m_ctrlWingTilt.SetPrecision(1);
	m_ctrlStabTilt.SetPrecision(1);
	m_ctrlFinTilt.SetPrecision(1);

	CString len, surf;
	GetLengthUnit(len,pFrame->m_LengthUnit);
	GetAreaUnit(surf, pFrame->m_AreaUnit);
	
	m_ctrlLen1.SetWindowText(len);
	m_ctrlLen2.SetWindowText(len);
	m_ctrlLen3.SetWindowText(len);
	m_ctrlLen4.SetWindowText(len);
	m_ctrlLen5.SetWindowText(len);
	m_ctrlLen6.SetWindowText(len);
	m_ctrlLen7.SetWindowText(len);
	m_ctrlLen8.SetWindowText(len);
	m_ctrlLen9.SetWindowText(len);
	m_ctrlLen10.SetWindowText(len);
	m_ctrlLen11.SetWindowText(len);
	m_ctrlLen12.SetWindowText(len);
	m_ctrlLen13.SetWindowText(len);

	m_ctrlSurf1.SetWindowText(surf);
	m_ctrlSurf2.SetWindowText(surf);
	m_ctrlSurf3.SetWindowText(surf);

	m_ctrlVolumeUnit.SetWindowText(len+"3");

	m_ctrlPlaneName.SetWindowText(m_pPlane->m_PlaneName);

	SetParams();
	SetResults(); 

	if(!m_bCheckName) m_ctrlPlaneName.EnableWindow(false);
	m_bChanged = false;

	m_pPlane->m_bDoubleSymFin = true;

	return FALSE;
}



void CPlaneDlg::OnFin() 
{
	m_bChanged = true;
	if(m_ctrlFinCheck.GetCheck())
	{
		m_ctrlSymFin.EnableWindow(true);
		m_ctrlDoubleFin.EnableWindow(true);
		m_ctrlFinTilt.EnableWindow(true);
		m_ctrlXLEFin.EnableWindow(true);
		if (m_ctrlDoubleFin.GetCheck()) m_ctrlYLEFin.EnableWindow(true);
		else                            m_ctrlYLEFin.EnableWindow(false);

		m_ctrlZLEFin.EnableWindow(true);
		m_ctrlDefineFin.EnableWindow(true);
		m_pPlane->m_bFin = true;
	}
	else 
	{
		m_ctrlSymFin.EnableWindow(false);
		m_ctrlDoubleFin.EnableWindow(false);
		m_ctrlFinTilt.EnableWindow(false);
		m_ctrlXLEFin.EnableWindow(false);
		m_ctrlYLEFin.EnableWindow(false);
		m_ctrlZLEFin.EnableWindow(false);
		m_ctrlDefineFin.EnableWindow(false);
		m_pPlane->m_bFin = false;
	}
	SetResults();
}

void CPlaneDlg::OnStab() 
{
	m_bChanged = true;
	if(m_ctrlStabCheck.GetCheck()){
		m_ctrlDefineStab.EnableWindow(true);
		m_ctrlXLEStab.EnableWindow(true);
		m_ctrlZLEStab.EnableWindow(true);
		m_ctrlStabTilt.EnableWindow(true);
		m_pPlane->m_bStab = true;
	}
	else {
		m_ctrlDefineStab.EnableWindow(false);
		m_ctrlXLEStab.EnableWindow(false);
		m_ctrlZLEStab.EnableWindow(false);
		m_ctrlStabTilt.EnableWindow(false);
		m_pPlane->m_bStab = false;
	}
	SetResults();
}

void CPlaneDlg::OnOK() 
{
	int j, n;
	CMiarex *pMiarex = (CMiarex*)s_pMiarex;
	CString strong;
	ReadParams();
	ComputePlane();

	n = m_pPlane->m_Wing.VLMGetPanelTotal();
	if(m_pPlane->m_bStab) n+= m_pPlane->m_Stab.VLMGetPanelTotal();
	if(m_pPlane->m_bFin)  n+= m_pPlane->m_Fin.VLMGetPanelTotal();
	if(n>VLMMATSIZE)
	{
		strong.Format("Total number of VLM panels =%d\n Max Number =%d\nA reduction of the number of VLM panels is required",n, VLMMATSIZE);
		AfxMessageBox(strong, MB_OK);
		return ;
	}

	//check the number of surfaces
	int nSurfaces = 0;
	for (j=0; j<m_pPlane->m_Wing.m_NPanel; j++)
	{
		if(abs(m_pPlane->m_Wing.m_TPos[j]-m_pPlane->m_Wing.m_TPos[j+1]) > pMiarex->m_MinPanelSize) nSurfaces+=2;
	}
	if(m_pPlane->m_bStab){
		for (j=0; j<m_pPlane->m_Stab.m_NPanel; j++)
		{
			if(abs(m_pPlane->m_Stab.m_TPos[j]-m_pPlane->m_Stab.m_TPos[j+1]) > pMiarex->m_MinPanelSize) nSurfaces+=2;
		}
	}
	if(m_pPlane->m_bFin)
	{
		for (j=0; j<m_pPlane->m_Fin.m_NPanel; j++)
		{
			if(abs(m_pPlane->m_Fin.m_TPos[j]-m_pPlane->m_Fin.m_TPos[j+1]) > pMiarex->m_MinPanelSize) 
			{
				if((m_pPlane->m_bSymFin) || (m_pPlane->m_bDoubleFin && m_pPlane->m_bDoubleSymFin))
					nSurfaces += 2;
				else 
					nSurfaces += 1;
			}
		}
	}
	if(nSurfaces >2*MAXPANELS)
	{
		strong.Format("Total number of wing panels =%d\n Max Number =%d\nA reduction of the number of wing panels is required",
			nSurfaces, MAXPANELS);
		AfxMessageBox(strong, MB_OK);
		return ;
	}

	CDialog::OnOK();
}

void CPlaneDlg::OnCancel() 
{
	CDialog::OnCancel();
}


void CPlaneDlg::OnDefineWing()
{
	CWing SaveWing;
	SaveWing.Duplicate(&m_pPlane->m_Wing);
	CWingDlg WingDlg(s_pMiarex, &m_pPlane->m_Wing);
	WingDlg.m_bCheckName= false;
	if(WingDlg.DoModal() ==IDOK)	
	{
		SetResults();
		if(WingDlg.m_bChanged) m_bChanged = true;
	}
	else   m_pPlane->m_Wing.Duplicate(&SaveWing);
}

void CPlaneDlg::OnDefineFin() 
{
	CWing SaveWing;
	SaveWing.Duplicate(&m_pPlane->m_Fin);
	CWingDlg WingDlg(s_pMiarex, &m_pPlane->m_Fin);
	WingDlg.m_bCheckName= false;

	if(WingDlg.DoModal() ==IDOK)
	{
		SetResults();	
		if(WingDlg.m_bChanged) m_bChanged = true;
	}
	else   m_pPlane->m_Fin.Duplicate(&SaveWing);
}


void CPlaneDlg::OnDefineStab() 
{
	CWing SaveWing;
	SaveWing.Duplicate(&m_pPlane->m_Stab);
	CWingDlg WingDlg(s_pMiarex, &m_pPlane->m_Stab);
	WingDlg.m_bCheckName= false;
	if(WingDlg.DoModal() ==IDOK)	 
	{
		SetResults();	
		if(WingDlg.m_bChanged) m_bChanged = true;
	}
	else  m_pPlane->m_Stab.Duplicate(&SaveWing);
}



void CPlaneDlg::OnDefineWing2()
{
	CWing SaveWing;
	SaveWing.Duplicate(&m_pPlane->m_Wing2);
	CWingDlg WingDlg(s_pMiarex, &m_pPlane->m_Wing2);
	WingDlg.m_bCheckName= false;
	if(WingDlg.DoModal() ==IDOK)	
	{
		SetResults();
		if(WingDlg.m_bChanged) m_bChanged = true;
	}
	else   m_pPlane->m_Wing2.Duplicate(&SaveWing);
}


void CPlaneDlg::OnChanged() 
{
	m_bChanged = true;
	ReadParams();
	SetResults();
}


void CPlaneDlg::ReadParams()
{
	CMainFrame *pFrame = (CMainFrame*)s_pMainFrame;
	CMiarex *pMiarex = (CMiarex*)s_pMiarex;
	int sel;
	CString strong;

	OnPlaneName();
	m_pPlane->m_WingTilt      = m_ctrlWingTilt.GetValue();
	m_pPlane->m_WingTilt2     = m_ctrlWingTilt2.GetValue();
	m_pPlane->m_StabTilt      = m_ctrlStabTilt.GetValue();
	m_pPlane->m_FinTilt       = m_ctrlFinTilt.GetValue();
	
	m_pPlane->m_LEWing.x      = m_ctrlXLEWing.GetValue() / pFrame->m_mtoUnit;
	m_pPlane->m_LEWing.z      = m_ctrlZLEWing.GetValue() / pFrame->m_mtoUnit;

	m_pPlane->m_LEStab.x      = m_ctrlXLEStab.GetValue() / pFrame->m_mtoUnit;
	m_pPlane->m_LEStab.z      = m_ctrlZLEStab.GetValue() / pFrame->m_mtoUnit;

	m_pPlane->m_LEWing2.x     = m_ctrlXLEWing2.GetValue() / pFrame->m_mtoUnit;
	m_pPlane->m_LEWing2.z     = m_ctrlZLEWing2.GetValue() / pFrame->m_mtoUnit;

	m_pPlane->m_LEFin.x       = m_ctrlXLEFin.GetValue() / pFrame->m_mtoUnit;
	m_pPlane->m_LEFin.y       = m_ctrlYLEFin.GetValue() / pFrame->m_mtoUnit;
	m_pPlane->m_LEFin.z       = m_ctrlZLEFin.GetValue() / pFrame->m_mtoUnit;

	m_pPlane->m_BodyPos.x      = m_ctrlXBody.GetValue() / pFrame->m_mtoUnit;
	m_pPlane->m_BodyPos.z      = m_ctrlZBody.GetValue() / pFrame->m_mtoUnit;

	if(m_ctrlStabCheck.GetCheck()) m_pPlane->m_bStab = true;
	else                           m_pPlane->m_bStab = false;
	if(m_ctrlFinCheck.GetCheck())  m_pPlane->m_bFin  = true;
	else                           m_pPlane->m_bFin  = false;

	sel = m_ctrlBodyList.GetCurSel();
	if (sel !=CB_ERR) m_ctrlBodyList.GetLBText(sel, strong);
	m_pPlane->m_pBody = pMiarex->GetBody(strong);
}

void CPlaneDlg::SetParams()
{
	CMainFrame *pFrame = (CMainFrame*)s_pMainFrame;
	CString strong;
	int i, pos;
	CBody *pBody = NULL;

	if(m_pPlane->m_bBody) m_ctrlBody.SetCheck(true);
	else        m_ctrlBody.SetCheck(false);

	for(i=0; i<s_poaBody->GetSize(); i++)
	{
		pBody = (CBody*)s_poaBody->GetAt(i);
		m_ctrlBodyList.AddString(pBody->m_BodyName);
	}

	if(m_pPlane->m_bBody && pBody)
	{
		pos = m_ctrlBodyList.FindStringExact(0, m_pPlane->m_pBody->m_BodyName);
		m_ctrlBodyList.SetCurSel(pos);
	}
	else 
	{
		if(m_ctrlBodyList.GetCount())	m_ctrlBodyList.SetCurSel(0);
		else							m_ctrlBody.EnableWindow(false);
		m_ctrlBodyList.EnableWindow(false);
		m_ctrlXBody.EnableWindow(false);
		m_ctrlZBody.EnableWindow(false);
	}

	m_ctrlPlaneName.SetWindowText(m_pPlane->m_PlaneName);
	m_ctrlWingTilt.SetValue(m_pPlane->m_WingTilt);
	m_ctrlWingTilt2.SetValue(m_pPlane->m_WingTilt2);
	m_ctrlStabTilt.SetValue(m_pPlane->m_StabTilt);
	m_ctrlFinTilt.SetValue(m_pPlane->m_FinTilt);
	
	m_ctrlXLEWing.SetValue(m_pPlane->m_LEWing.x * pFrame->m_mtoUnit);
	m_ctrlZLEWing.SetValue(m_pPlane->m_LEWing.z * pFrame->m_mtoUnit);

	m_ctrlXLEStab.SetValue(m_pPlane->m_LEStab.x * pFrame->m_mtoUnit);
	m_ctrlZLEStab.SetValue(m_pPlane->m_LEStab.z * pFrame->m_mtoUnit);

	m_ctrlXLEWing2.SetValue(m_pPlane->m_LEWing2.x * pFrame->m_mtoUnit);
	m_ctrlZLEWing2.SetValue(m_pPlane->m_LEWing2.z * pFrame->m_mtoUnit);

	m_ctrlXBody.SetValue(m_pPlane->m_BodyPos.x * pFrame->m_mtoUnit);
	m_ctrlZBody.SetValue(m_pPlane->m_BodyPos.z * pFrame->m_mtoUnit);

	m_ctrlBiplane.SetCheck(m_pPlane->m_bBiplane);
	OnBiplane();
    
	m_ctrlXLEFin.SetValue(m_pPlane->m_LEFin.x* pFrame->m_mtoUnit);
	m_ctrlYLEFin.SetValue(m_pPlane->m_LEFin.y* pFrame->m_mtoUnit);
	m_ctrlZLEFin.SetValue(m_pPlane->m_LEFin.z* pFrame->m_mtoUnit);
	m_ctrlFinCheck.SetCheck(m_pPlane->m_bFin);
	m_ctrlDoubleFin.SetCheck(m_pPlane->m_bDoubleFin);
	m_ctrlSymFin.SetCheck(m_pPlane->m_bSymFin);
	OnFin();
	m_ctrlStabCheck.SetCheck(m_pPlane->m_bStab);
	OnStab();
}


void CPlaneDlg::SetResults()
{
	CMainFrame *pFrame = (CMainFrame*)s_pMainFrame;
	CString str;

	double area = m_pPlane->m_Wing.m_Area;
	if(m_pPlane->m_bBiplane) area += m_pPlane->m_Wing2.m_Area;

	str.Format("%7.2f", area*pFrame->m_m2toUnit);
	m_ctrlWingSurface.SetWindowText(str);

	if(m_pPlane->m_bStab)	str.Format("%7.2f", m_pPlane->m_Stab.m_Area*pFrame->m_m2toUnit);
	else str = " ";
	m_ctrlStabSurface.SetWindowText(str);

	if(m_pPlane->m_bFin) str.Format("%7.2f", m_pPlane->m_Fin.m_Area*pFrame->m_m2toUnit);
	else str=" ";
	m_ctrlFinSurface.SetWindowText(str);

	double span = m_pPlane->m_Wing.m_Span;
	if(m_pPlane->m_bBiplane) span = max(m_pPlane->m_Wing.m_Span, m_pPlane->m_Wing2.m_Span);
	str.Format("%5.2f", span*pFrame->m_mtoUnit);
	m_ctrlWingSpan.SetWindowText(str);

	ComputePlane();
	if(m_pPlane->m_bStab){
		double SLA = m_pPlane->m_LEStab.x + m_pPlane->m_Stab.m_TChord[0]/4.0 - m_pPlane->m_Wing.m_TChord[0]/4.0;
		str.Format("%5.2f", SLA*pFrame->m_mtoUnit);
	}
	else  str=" ";
	m_ctrlStabLeverArm.SetWindowText(str);

	if(m_pPlane->m_bStab){
		str.Format("%5.2f", m_pPlane->m_TailVolume);
	}
	else str =" ";
	m_ctrlStabVolume.SetWindowText(str);

	int total = m_pPlane->m_Wing.VLMGetPanelTotal();
	total += m_pPlane->m_Wing.m_NXPanels[m_pPlane->m_Wing.m_NPanel-1];
	if(m_pPlane->m_bBiplane)	
	{
		total += m_pPlane->m_Wing2.VLMGetPanelTotal();
		total += m_pPlane->m_Wing2.m_NXPanels[m_pPlane->m_Wing2.m_NPanel-1];
	}
	if(m_pPlane->m_bStab)	
	{
		total += m_pPlane->m_Stab.VLMGetPanelTotal();
		total += m_pPlane->m_Stab.m_NXPanels[m_pPlane->m_Stab.m_NPanel-1];
	}
	if(m_pPlane->m_bFin) 
	{
		if(m_pPlane->m_bDoubleFin ||m_pPlane->m_bSymFin)  total += 2*m_pPlane->m_Fin.VLMGetPanelTotal();
		else                          total +=   m_pPlane->m_Fin.VLMGetPanelTotal();
		total += m_pPlane->m_Fin.m_NXPanels[m_pPlane->m_Fin.m_NPanel-1];
	}

	total *=2;//in case we have a panel analysis

	if(m_pPlane->m_bBody && m_pPlane->m_pBody) total += m_pPlane->m_pBody->m_nxPanels * m_pPlane->m_pBody->m_nhPanels * 2;

	m_ctrlVLMTotalPanels.SetValue(total);
	str.Format("%5.2e", m_pPlane->m_Volume*pFrame->m_mtoUnit*pFrame->m_mtoUnit*pFrame->m_mtoUnit);
	m_ctrlVolume.SetWindowText(str);
}


void CPlaneDlg::ComputePlane(void)
{
	if(m_pPlane->m_bStab)
	{
		double SLA = m_pPlane->m_LEStab.x + m_pPlane->m_Stab.m_TChord[0]/4.0 - m_pPlane->m_Wing.m_TChord[0]/4.0;
		double area = m_pPlane->m_Wing.m_Area;
		if(m_pPlane->m_bBiplane) area += m_pPlane->m_Wing2.m_Area;

		double ProjectedArea = 0.0;
		for (int i=0;i<m_pPlane->m_Stab.m_NPanel; i++){
			ProjectedArea += m_pPlane->m_Stab.m_TLength[i+1]*(m_pPlane->m_Stab.m_TChord[i]+m_pPlane->m_Stab.m_TChord[i+1])/2.0
							*cos(m_pPlane->m_Stab.m_TDihedral[i])*cos(m_pPlane->m_Stab.m_TDihedral[i]);//m²
		
		}
		ProjectedArea *=2.0;
		m_pPlane->m_TailVolume = ProjectedArea * SLA / area/m_pPlane->m_Wing.m_MAChord ;
	}
	else m_pPlane->m_TailVolume = 0.0;
	
	m_pPlane->m_Fin.m_bDoubleFin = m_pPlane->m_bDoubleFin;
	m_pPlane->m_Fin.m_bSymFin    = m_pPlane->m_bSymFin;
	m_pPlane->m_Volume = m_pPlane->m_Wing.m_Volume;
	if(m_pPlane->m_bBiplane)	m_pPlane->m_Volume += m_pPlane->m_Wing2.m_Volume;
	if(m_pPlane->m_bStab)		m_pPlane->m_Volume += m_pPlane->m_Stab.m_Volume;
	if(m_pPlane->m_bFin)	{
		m_pPlane->m_Volume += m_pPlane->m_Fin.m_Volume;
		if(m_pPlane->m_bDoubleFin || m_pPlane->m_bSymFin) m_pPlane->m_Volume += m_pPlane->m_Fin.m_Volume;
	}
}



void CPlaneDlg::OnImportWing() 
{
	CMainFrame *pFrame = (CMainFrame*)s_pMainFrame;
	CImportWingDlg dlg;
	dlg.m_WingName = m_pPlane->m_Wing.m_WingName;
	dlg.m_poaWing = s_poaWing;

	if(dlg.DoModal() ==IDOK)
	{
		m_bChanged = true;
		CWing *pWing = pFrame->Miarex.GetWing(dlg.m_WingName);
		if(pWing)
		{
			m_pPlane->m_Wing.Duplicate(pWing);
			m_pPlane->m_Wing.m_WingColor = pWing->m_WingColor;
		}
	}
}


void CPlaneDlg::OnExportWing() 
{
	CMainFrame *pFrame = (CMainFrame*)s_pMainFrame;
	CWing *pNewWing = new CWing();
	pNewWing->Duplicate(&m_pPlane->m_Wing);
	pNewWing->m_WingName = "";
	pFrame->Miarex.AddWing(pNewWing);
	pFrame->UpdateUFOs();
	pFrame->Miarex.UpdateView();

}
void CPlaneDlg::OnImportWing2() 
{
	CMainFrame *pFrame = (CMainFrame*)s_pMainFrame;
	CImportWingDlg dlg;
	dlg.m_WingName = m_pPlane->m_Wing2.m_WingName;
	dlg.m_poaWing = s_poaWing;

	if(dlg.DoModal() ==IDOK)
	{
		m_bChanged = true;
		CWing *pWing = pFrame->Miarex.GetWing(dlg.m_WingName);
		if(pWing) {
			m_pPlane->m_Wing2.Duplicate(pWing);
			m_pPlane->m_Wing2.m_WingColor = pWing->m_WingColor;
		}
	}
}


void CPlaneDlg::OnExportWing2() 
{
	CMainFrame *pFrame = (CMainFrame*)s_pMainFrame;
	CWing *pNewWing = new CWing();
	pNewWing->Duplicate(&m_pPlane->m_Wing2);
	pNewWing->m_WingName = "";
	pFrame->Miarex.AddWing(pNewWing);
	pFrame->UpdateUFOs();
	pFrame->Miarex.UpdateView();

}
void CPlaneDlg::OnPlaneName() 
{
	m_ctrlPlaneName.GetWindowText(m_pPlane->m_PlaneName);
	m_pPlane->m_Wing.m_WingName = m_pPlane->m_PlaneName+"_Wing";
	m_pPlane->m_Stab.m_WingName = m_pPlane->m_PlaneName+"_Elev";
	m_pPlane->m_Fin.m_WingName = m_pPlane->m_PlaneName+"_Fin";
}


void CPlaneDlg::OnDoubleFin()
{
	if (m_ctrlDoubleFin.GetCheck()) 
	{
		m_ctrlYLEFin.EnableWindow(true);
		m_pPlane->m_bDoubleFin = true;
		m_pPlane->m_bSymFin    = false;
		m_ctrlSymFin.SetCheck(false);
	}
	else{
        m_ctrlYLEFin.EnableWindow(false);
		m_pPlane->m_bDoubleFin = false;
	}
	m_bChanged = true;
	SetResults();
}



void CPlaneDlg::OnSymFin() 
{	
	if (m_ctrlSymFin.GetCheck()) 
	{
		m_ctrlYLEFin.EnableWindow(false);
		m_pPlane->m_bSymFin    = true;
		m_pPlane->m_bDoubleFin = false;
		m_ctrlDoubleFin.SetCheck(false);
		m_ctrlYLEFin.EnableWindow(false);
	}
	else
	{
//		m_ctrlDoubleFin.EnableWindow(true);
		m_ctrlYLEFin.EnableWindow(true);
		m_pPlane->m_bSymFin = false;
	}
	m_bChanged = true;
	SetResults();
}

BOOL CPlaneDlg::PreTranslateMessage(MSG* pMsg) 
{
	CWnd* pWnd = GetFocus();
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN){ 
		if(GetDlgItem(IDCANCEL) != pWnd && GetDlgItem(IDOK) != pWnd) {
			m_ctrlOK.SetFocus();
//			ReadParams();
//			SetResults();
		}
		else if(GetDlgItem(IDOK) == pWnd ) OnOK();
		else if(GetDlgItem(IDCANCEL) == pWnd ) OnCancel();

		return true;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CPlaneDlg::OnBiplane()
{
	if(m_ctrlBiplane.GetCheck()) m_pPlane->m_bBiplane = true; else m_pPlane->m_bBiplane = false;
	if(m_pPlane->m_bBiplane)
	{
		m_ctrlDefineWing2.EnableWindow(TRUE);
		m_ctrlImportWing2.EnableWindow(TRUE);
		m_ctrlExportWing2.EnableWindow(TRUE);
		m_ctrlWingTilt2.EnableWindow(TRUE);	
		m_ctrlZLEWing2.EnableWindow(TRUE);
		m_ctrlXLEWing2.EnableWindow(TRUE);
	}
	else
	{
		m_ctrlDefineWing2.EnableWindow(FALSE);
		m_ctrlImportWing2.EnableWindow(FALSE);
		m_ctrlExportWing2.EnableWindow(FALSE);
		m_ctrlWingTilt2.EnableWindow(FALSE);	
		m_ctrlZLEWing2.EnableWindow(FALSE);
		m_ctrlXLEWing2.EnableWindow(FALSE);
	}
}


void CPlaneDlg::OnBodyCheck()
{
	m_bChanged = true;
	if(m_ctrlBody.GetCheck()) 
	{
		if(m_ctrlBodyList.GetCount())
		{
			m_ctrlBodyList.EnableWindow(true);
			m_ctrlXBody.EnableWindow(true);
			m_ctrlZBody.EnableWindow(true);
			m_pPlane->m_bBody=true;
			OnSelChangeBodyList();
		}
		else
		{
			m_pPlane->m_bBody=false;
			m_ctrlBodyList.EnableWindow(false);
			m_ctrlXBody.EnableWindow(false);
			m_ctrlZBody.EnableWindow(false);
		}
	}
	else
	{
		m_pPlane->m_bBody=false;
		m_ctrlBodyList.EnableWindow(false);
		m_ctrlXBody.EnableWindow(false);
		m_ctrlZBody.EnableWindow(false);
	}
	SetResults();
}

void CPlaneDlg::OnSelChangeBodyList()
{
	CString strong;
	m_bChanged = true;
	CMiarex *pMiarex = (CMiarex*)s_pMiarex;
	int sel = m_ctrlBodyList.GetCurSel();
	if (sel !=CB_ERR) m_ctrlBodyList.GetLBText(sel, strong);

	m_pPlane->m_pBody = pMiarex->GetBody(strong);
}





















