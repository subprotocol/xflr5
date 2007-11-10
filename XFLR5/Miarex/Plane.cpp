/****************************************************************************

    CPlane Class
	Copyright (C) 2006 André Deperrois xflr5@yahoo.com

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
#include ".\plane.h"

bool CPlane::m_bCheckPanels = false;
/////////////////////////////////////////////////////////////////////////////
// CPlane dialog

CPlane::CPlane(CWnd* pParent, CWnd* pMiarex)
	: CDialog(CPlane::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlane)
	//}}AFX_DATA_INIT
	m_pFrame = pParent;
	m_pMiarex = pMiarex;
	m_bActive = false;

	m_Wing.m_pMiarex  = pMiarex;
	m_Wing2.m_pMiarex = pMiarex;
	m_Stab.m_pMiarex  = pMiarex;
	m_Fin.m_pMiarex   = pMiarex;

	m_Wing.m_pFrame  = pParent;
	m_Wing2.m_pFrame = pParent;
	m_Stab.m_pFrame  = pParent;
	m_Fin.m_pFrame   = pParent;

	m_poaWing = NULL;

	m_Wing.m_WingName   = "Wing";
	m_Wing.ComputeGeometry();
	m_Wing2.m_WingName   = "2nd Wing";
	m_Wing2.ComputeGeometry();

	m_Stab.m_WingName      = "Elevator";
	m_Stab.m_bIsFin        = false;
	m_Stab.m_TChord[0]     = 0.100;
	m_Stab.m_TChord[1]     = 0.080;
	m_Stab.m_TPos[0]       =   0.0;
	m_Stab.m_TPos[1]       = 0.150;
	m_Stab.m_TLength[0]    =   0.0;
	m_Stab.m_TLength[1]    = 0.150;
	m_Stab.m_TOffset[0]    =   0.0;
	m_Stab.m_TOffset[1]    = 0.020;
	m_Stab.m_bVLMAutoMesh  = false;
	m_Stab.m_NXPanels[0]   = 3;
	m_Stab.m_NYPanels[0]   = 3;
	m_Stab.m_YPanelDist[0] = 1;
	m_Stab.m_YPanelDist[0] = 0;
	m_Stab.ComputeGeometry();
	
	m_Fin.m_WingName      = "Fin";
	m_Fin.m_bIsFin        = true;
	m_Fin.m_TChord[0]     = 0.100;
	m_Fin.m_TChord[1]     = 0.060;
	m_Fin.m_TPos[0]       = 0.000;
	m_Fin.m_TPos[1]       = 0.120;
	m_Fin.m_TLength[0]    = 0.000;
	m_Fin.m_TLength[1]    = 0.120;
	m_Fin.m_TOffset[0]    = 0.000;
	m_Fin.m_TOffset[1]    = 0.040;
	m_Fin.m_bVLMAutoMesh  = false;
	m_Fin.m_NXPanels[0]   = 3;
	m_Fin.m_NYPanels[0]   = 3;
	m_Fin.m_YPanelDist[0] = 1;
	m_Fin.m_YPanelDist[0] = 0;

	m_Fin.ComputeGeometry();

	m_XCmRef        =   0.0;
	m_TailVolume    =   0.0;
	m_WingTilt      =   0.0;
	m_WingTilt2     =   0.0;
	m_LEStab.x      = 0.600;
	m_LEStab.y      =   0.0;
	m_LEStab.z      =   0.0;
	m_StabTilt      =   0.0;
	m_FinTilt       =   0.0;
	m_LEFin.x       = 0.650;
	m_LEFin.y       =   0.0;
	m_LEFin.z       =   0.0;
	m_Volume        =   0.0;

	m_bCheckName    = true;
	m_bFin          = true;
	m_bDoubleFin    = false;
	m_bSymFin       = false;
	m_bStab         = true;
	m_bDoubleSymFin = false;
	m_bChanged      = false;
//	m_bCheckPanels  = true;
	m_bBiplane      = false;

	m_PlaneName  = "Plane Name";

	m_poaPlane = NULL;
}


void CPlane::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CHECKPANELS, m_ctrlCheckPanels);
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
	DDX_Control(pDX, IDC_DEFINEELEV, m_ctrlDefineStab);
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
	DDX_Control(pDX, IDC_STAB, m_ctrlStabCheck);
	DDX_Control(pDX, IDC_FIN, m_ctrlFinCheck);
	DDX_Control(pDX, IDC_VLMTOTALPANELS, m_ctrlVLMTotalPanels);
	DDX_Control(pDX, IDC_DOUBLEFIN, m_ctrlDoubleFin);
	DDX_Control(pDX, IDC_RADIO1, m_ctrlRadio1);
	DDX_Control(pDX, IDC_RADIO2, m_ctrlRadio2);
	DDX_Control(pDX, IDC_FINTILT, m_ctrlFinTilt);
	DDX_Control(pDX, IDC_BIPLANE, m_ctrlBiplane);
	DDX_Control(pDX, IDOK, m_ctrlOK);
}


BEGIN_MESSAGE_MAP(CPlane, CDialog)
	//{{AFX_MSG_MAP(CPlane)
	ON_BN_CLICKED(IDC_FIN, OnFin)
	ON_BN_CLICKED(IDC_STAB, OnStab)
	ON_BN_CLICKED(IDC_DEFINEFIN, OnDefineFin)
	ON_BN_CLICKED(IDC_DEFINEELEV, OnDefineStab)
	ON_EN_KILLFOCUS(IDC_XLESTAB, OnChanged)
	ON_BN_CLICKED(IDC_DEFINEWING, OnDefineWing)
	ON_BN_CLICKED(IDC_IMPORTWING, OnImportWing)
	ON_EN_UPDATE(IDC_PLANENAME, OnPlaneName)
	ON_BN_CLICKED(IDC_SYMFIN, OnSymFin)
	ON_BN_CLICKED(IDC_RADIO1, OnDoubleSymFin)
	ON_BN_CLICKED(IDC_RADIO2, OnDoubleSymFin)
	ON_BN_CLICKED(IDC_CHECKPANELS, OnCheckPanels)
	ON_EN_KILLFOCUS(IDC_WINGTILT, OnChanged)
	ON_EN_KILLFOCUS(IDC_STABTILT, OnChanged)
	ON_EN_KILLFOCUS(IDC_FINTILT, OnChanged)
	ON_EN_KILLFOCUS(IDC_ZLESTAB, OnChanged)
	ON_EN_KILLFOCUS(IDC_XLEFIN, OnChanged)
	ON_EN_KILLFOCUS(IDC_ZLEFIN, OnChanged)
	ON_EN_KILLFOCUS(IDC_PLANENAME, OnPlaneName)
	ON_BN_CLICKED(IDC_EXPORTWING, OnExportWing)
	ON_BN_CLICKED(IDC_DOUBLEFIN, OnDoubleFin)
	ON_BN_CLICKED(IDC_BIPLANE, OnBiplane)
	ON_BN_CLICKED(IDC_DEFINEWING2, OnDefineWing2)
	ON_BN_CLICKED(IDC_IMPORTWING2, OnImportWing2)
	ON_BN_CLICKED(IDC_EXPORTWING2, OnExportWing2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlane message handlers


BOOL CPlane::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

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
		
	m_ctrlSurf1.SetWindowText(surf);
	m_ctrlSurf2.SetWindowText(surf);
	m_ctrlSurf3.SetWindowText(surf);

	m_ctrlVolumeUnit.SetWindowText(len+"3");

	m_ctrlPlaneName.SetWindowText(m_PlaneName);

	SetParams();
	SetResults(); 

	if(!m_bCheckName) m_ctrlPlaneName.EnableWindow(false);
	m_bChanged = false;

	m_bActive = true;

	m_ctrlCheckPanels.SetCheck(m_bCheckPanels);
	return FALSE;
}



void CPlane::OnFin() 
{
	m_bChanged = true;
	if(m_ctrlFinCheck.GetCheck()){
		m_ctrlDoubleFin.EnableWindow(true);
		m_ctrlXLEFin.EnableWindow(true);
		if (m_ctrlDoubleFin.GetCheck()) m_ctrlYLEFin.EnableWindow(true);
		else                            m_ctrlYLEFin.EnableWindow(false);

		m_ctrlZLEFin.EnableWindow(true);
		m_ctrlDefineFin.EnableWindow(true);
		m_bFin = true;
	}
	else {
		m_ctrlDoubleFin.EnableWindow(false);
		m_ctrlXLEFin.EnableWindow(false);
		m_ctrlYLEFin.EnableWindow(false);
		m_ctrlZLEFin.EnableWindow(false);
		m_ctrlDefineFin.EnableWindow(false);
		m_bFin = false;
	}
	SetResults();
}

void CPlane::OnStab() 
{
	m_bChanged = true;
	if(m_ctrlStabCheck.GetCheck()){
		m_ctrlDefineStab.EnableWindow(true);
		m_ctrlXLEStab.EnableWindow(true);
		m_ctrlZLEStab.EnableWindow(true);
		m_ctrlStabTilt.EnableWindow(true);
		m_bStab = true;
	}
	else {
		m_ctrlDefineStab.EnableWindow(false);
		m_ctrlXLEStab.EnableWindow(false);
		m_ctrlZLEStab.EnableWindow(false);
		m_ctrlStabTilt.EnableWindow(false);
		m_bStab = false;
	}
	SetResults();
}

void CPlane::OnOK() 
{
	CMiarex *pMiarex = (CMiarex*)m_pMiarex;
	CString strong;
	ReadParams();
	ComputePlane();

	int n = m_Wing.VLMGetPanelTotal();
	if(m_bStab) n+= m_Stab.VLMGetPanelTotal();
	if(m_bFin)  n+= m_Fin.VLMGetPanelTotal();
	if(n>VLMMATSIZE){
		strong.Format("Total number of VLM panels =%d\n Max Number =%d\nA reduction of the number of VLM panels is required",n, VLMMATSIZE);
		AfxMessageBox(strong, MB_OK);
		return ;
	}

	//check the number of surfaces
	int nSurfaces = 0;
	for (int j=0; j<m_Wing.m_NPanel; j++){
		if(fabs(m_Wing.m_TPos[j]-m_Wing.m_TPos[j+1]) > pMiarex->m_MinPanelSize) nSurfaces+=2;
	}
	if(m_bStab){
		for (j=0; j<m_Stab.m_NPanel; j++){
			if(fabs(m_Stab.m_TPos[j]-m_Stab.m_TPos[j+1]) > pMiarex->m_MinPanelSize) nSurfaces+=2;
		}
	}
	if(m_bFin){
		for (j=0; j<m_Fin.m_NPanel; j++){
			if(fabs(m_Fin.m_TPos[j]-m_Fin.m_TPos[j+1]) > pMiarex->m_MinPanelSize) {
				if((m_bSymFin) || (m_bDoubleFin && m_bDoubleSymFin))
					nSurfaces += 2;
				else 
					nSurfaces += 1;
			}
		}
	}
	if(nSurfaces >MAXVLMSURFACES){
		strong.Format("Total number of wing panels =%d\n Max Number =%d\nA reduction of the number of wing panels is required",
			nSurfaces, MAXVLMSURFACES);
		AfxMessageBox(strong, MB_OK);
		return ;
	}

	if(m_bCheckPanels)	CheckPanelDisposition();

	m_bActive = false;
	CDialog::OnOK();
}

void CPlane::OnCancel() 
{
	m_bActive = false;
	CDialog::OnCancel();
}


bool CPlane::SerializePlane(CArchive& ar) 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString strong = " ";
	int ArchiveFormat;// identifies the format of the file
	if (ar.IsStoring())
	{	// storing code
		ar << 1007;
		//1007 : added second wing data, CheckPanel
		//1006 : Converted lengths to m
		//1005 : stored double sym fin
		//1004 : suppressed colors
		//1003 : Added fin tilt;
		//1002 : Added doublefin;
		ar << m_PlaneName;
		m_Wing.SerializeWing(ar);
		m_Wing2.SerializeWing(ar);
		m_Stab.SerializeWing(ar);
		m_Fin.SerializeWing(ar);
		if(m_bStab)          ar <<1; else ar <<0;
		if(m_bFin)           ar <<1; else ar <<0;
		if(m_bDoubleFin)     ar <<1; else ar <<0;
		if(m_bSymFin)		 ar <<1; else ar <<0;
		if(m_bDoubleSymFin)  ar <<1; else ar <<0;
		if(m_bCheckPanels)   ar <<1; else ar <<0;
		if(m_bBiplane)       ar <<1; else ar <<0;
		ar << m_LEWing2.x << m_LEWing2.y << m_LEWing2.z << m_WingTilt2; 
		ar << m_LEStab.x << m_LEStab.y << m_LEStab.z; 
		ar << m_WingTilt << m_StabTilt << m_FinTilt;
		ar << m_LEFin.x << m_LEFin.y << m_LEFin.z; 
		return true;
	}
	else
	{	// loading code
		int k;
		ar >> ArchiveFormat;
		if (ArchiveFormat <1001 || ArchiveFormat>1100) {
			m_PlaneName = "";
			return false;
		}

		ar >> m_PlaneName;
		if (m_PlaneName.GetLength() ==0) {
			return false;
		}

		m_Wing.SerializeWing(ar);
		if(ArchiveFormat>=1007) m_Wing2.SerializeWing(ar);
		m_Stab.SerializeWing(ar);
		m_Fin.SerializeWing(ar);

		ar >>k;
		if(k) m_bStab = true; else m_bStab = false;
		ar >>k;
		if(k) m_bFin = true;  else m_bFin = false;

		if(ArchiveFormat>=1002){
			ar >>k;
			if(k) m_bDoubleFin = true;  else m_bDoubleFin = false;
			m_Fin.m_bDoubleFin = m_bDoubleFin;
			ar >>k;
			if(k) m_bSymFin = true;  else m_bSymFin = false;
			m_Fin.m_bSymFin = m_bSymFin;
		}
		if(ArchiveFormat>=1005){
			ar >>k;
			if(k) m_bDoubleSymFin = true;  else m_bDoubleSymFin = false;
			m_Fin.m_bDoubleSymFin = m_bDoubleSymFin;
		}
		if(ArchiveFormat>=1007){
			ar >>k;
			if(k) m_bCheckPanels = true;  else m_bCheckPanels = false;
			ar >>k;
			if(k) m_bBiplane = true;  else m_bBiplane = false;
			ar >> m_LEWing2.x >> m_LEWing2.y >> m_LEWing2.z >> m_WingTilt2;
		}
		ar >> m_LEStab.x >> m_LEStab.y >> m_LEStab.z; 
		ar >> m_WingTilt >> m_StabTilt;
		if(ArchiveFormat>=1003){
			ar >> m_FinTilt;
		}
		ar >> m_LEFin.x >> m_LEFin.y >> m_LEFin.z; 
		if(ArchiveFormat<1004){
			COLORREF cr;
			ar >> cr ;
			ar >> cr ;
			ar >> cr ;
		}

		if(ArchiveFormat<1006){//convert to m
			m_LEStab.x /= 1000.0;
			m_LEStab.y /= 1000.0;
			m_LEStab.z /= 1000.0;
			m_LEFin.x  /= 1000.0;
			m_LEFin.y  /= 1000.0;
			m_LEFin.z  /= 1000.0;
		}

		ComputePlane();
		return true;
	}
}


void CPlane::OnDefineFin() 
{
	CWing SaveWing;
	SaveWing.Duplicate(&m_Fin);
	m_Fin.m_bCheckName= false;
	if(m_Fin.DoModal() ==IDOK){
		SetResults();	
		if(m_Fin.m_bChanged) m_bChanged = true;
	}
	else                         m_Fin.Duplicate(&SaveWing);
}


void CPlane::OnDefineStab() 
{
	CWing SaveWing;
	SaveWing.Duplicate(&m_Stab);
	m_Stab.m_bCheckName= false;
	if(m_Stab.DoModal() ==IDOK)	 {
		SetResults();	
		if(m_Stab.m_bChanged) m_bChanged = true;
	}
	else                         m_Stab.Duplicate(&SaveWing);
}


void CPlane::OnDefineWing()
{
	CWing SaveWing;
	SaveWing.Duplicate(&m_Wing);
	m_Wing.m_bCheckName= false;
	if(m_Wing.DoModal() ==IDOK)	 {
		SetResults();
		if(m_Wing.m_bChanged) m_bChanged = true;
	}
	else                         m_Wing.Duplicate(&SaveWing);
}

void CPlane::OnDefineWing2()
{
	CWing SaveWing;
	SaveWing.Duplicate(&m_Wing2);
	m_Wing2.m_bCheckName= false;
	if(m_Wing2.DoModal() ==IDOK)	 {
		SetResults();
		if(m_Wing2.m_bChanged) m_bChanged = true;
	}
	else                         m_Wing2.Duplicate(&SaveWing);
}


void CPlane::Duplicate(CPlane *pPlane)
{
	m_PlaneName     = pPlane->m_PlaneName;
	m_bFin          = pPlane->m_bFin;
	m_bSymFin       = pPlane->m_bSymFin;
	m_bDoubleFin    = pPlane->m_bDoubleFin;
	m_bDoubleSymFin = pPlane->m_bDoubleSymFin;
	m_bStab         = pPlane->m_bStab;
	m_bBiplane      = pPlane->m_bBiplane;
	m_bCheckPanels  = pPlane->m_bCheckPanels;

	m_WingTilt   = pPlane->m_WingTilt;
	m_WingTilt2  = pPlane->m_WingTilt2;
	m_StabTilt   = pPlane->m_StabTilt;
	m_LEWing2.Copy(pPlane->m_LEWing2);
	m_LEFin.Copy(pPlane->m_LEFin);
	m_LEStab.Copy(pPlane->m_LEStab);

	m_Wing.Duplicate(&pPlane->m_Wing);
	m_Wing2.Duplicate(&pPlane->m_Wing2);
	m_Fin.Duplicate(&pPlane->m_Fin);
	m_Stab.Duplicate(&pPlane->m_Stab);

	m_Wing.m_WingColor  = pPlane->m_Wing.m_WingColor;
	m_Wing2.m_WingColor = pPlane->m_Wing2.m_WingColor;
	m_Stab.m_WingColor  = pPlane->m_Stab.m_WingColor;
	m_Fin.m_WingColor   = pPlane->m_Fin.m_WingColor;

	m_Wing.m_pFrame  = m_pFrame;
	m_Wing2.m_pFrame = m_pFrame;
	m_Stab.m_pFrame  = m_pFrame;
	m_Fin.m_pFrame   = m_pFrame;

	m_poaWing = pPlane->m_poaWing;
}


void CPlane::OnChanged() 
{
	m_bChanged = true;
//	ComputePlane();
	ReadParams();
	SetResults();
}


void CPlane::ReadParams()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	OnPlaneName();
	m_WingTilt      = m_ctrlWingTilt.GetValue();
	m_WingTilt2     = m_ctrlWingTilt2.GetValue();
	m_StabTilt      = m_ctrlStabTilt.GetValue();
	m_FinTilt       = m_ctrlFinTilt.GetValue();
	
	m_LEStab.x      = m_ctrlXLEStab.GetValue() / pFrame->m_mtoUnit;
	m_LEStab.z      = m_ctrlZLEStab.GetValue() / pFrame->m_mtoUnit;

	m_LEWing2.x      = m_ctrlXLEWing2.GetValue() / pFrame->m_mtoUnit;
	m_LEWing2.z      = m_ctrlZLEWing2.GetValue() / pFrame->m_mtoUnit;

	m_LEFin.x       = m_ctrlXLEFin.GetValue() / pFrame->m_mtoUnit;
	m_LEFin.y       = m_ctrlYLEFin.GetValue() / pFrame->m_mtoUnit;
	m_LEFin.z       = m_ctrlZLEFin.GetValue() / pFrame->m_mtoUnit;


	if(m_ctrlStabCheck.GetCheck()) m_bStab = true;
	else                           m_bStab = false;
	if(m_ctrlFinCheck.GetCheck())  m_bFin  = true;
	else                           m_bFin  = false;
}

void CPlane::SetParams()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString strong;

	m_ctrlPlaneName.SetWindowText(m_PlaneName);
	m_ctrlWingTilt.SetValue(m_WingTilt);
	m_ctrlWingTilt2.SetValue(m_WingTilt2);
	m_ctrlStabTilt.SetValue(m_StabTilt);
	m_ctrlFinTilt.SetValue(m_FinTilt);
	
	m_ctrlXLEStab.SetValue(m_LEStab.x * pFrame->m_mtoUnit);
	m_ctrlZLEStab.SetValue(m_LEStab.z* pFrame->m_mtoUnit);

	m_ctrlXLEWing2.SetValue(m_LEWing2.x * pFrame->m_mtoUnit);
	m_ctrlZLEWing2.SetValue(m_LEWing2.z* pFrame->m_mtoUnit);

	m_ctrlBiplane.SetCheck(m_bBiplane);
	OnBiplane();
    
	m_ctrlXLEFin.SetValue(m_LEFin.x* pFrame->m_mtoUnit);
	m_ctrlYLEFin.SetValue(m_LEFin.y* pFrame->m_mtoUnit);
	m_ctrlZLEFin.SetValue(m_LEFin.z* pFrame->m_mtoUnit);
	m_ctrlFinCheck.SetCheck(m_bFin);
	m_ctrlDoubleFin.SetCheck(m_bDoubleFin);
	m_ctrlSymFin.SetCheck(m_bSymFin);
	OnFin();
	m_ctrlStabCheck.SetCheck(m_bStab);
	OnStab();
	if(m_bDoubleFin){
		m_ctrlRadio1.EnableWindow(true);
		m_ctrlRadio2.EnableWindow(true);
	}
	else{
		m_ctrlRadio1.EnableWindow(false);
		m_ctrlRadio2.EnableWindow(false);
	}
	if(m_bDoubleSymFin)	CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);
	else                CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
}


void CPlane::SetResults()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString str;

	double area = m_Wing.m_Area;
	if(m_bBiplane) area += m_Wing2.m_Area;

	str.Format("%7.2f", area*pFrame->m_m2toUnit);
	m_ctrlWingSurface.SetWindowText(str);

	if(m_bStab)	str.Format("%7.2f", m_Stab.m_Area*pFrame->m_m2toUnit);
	else str = " ";
	m_ctrlStabSurface.SetWindowText(str);

	if(m_bFin) str.Format("%7.2f", m_Fin.m_Area*pFrame->m_m2toUnit);
	else str=" ";
	m_ctrlFinSurface.SetWindowText(str);

	double span = m_Wing.m_Span;
	if(m_bBiplane) span = max(m_Wing.m_Span, m_Wing2.m_Span);
	str.Format("%5.2f", span*pFrame->m_mtoUnit);
	m_ctrlWingSpan.SetWindowText(str);

	ComputePlane();
	if(m_bStab){
		double SLA = m_LEStab.x + m_Stab.m_TChord[0]/4.0 - m_Wing.m_TChord[0]/4.0;
		str.Format("%5.2f", SLA*pFrame->m_mtoUnit);
	}
	else  str=" ";
	m_ctrlStabLeverArm.SetWindowText(str);

	if(m_bStab){
		str.Format("%5.2f", m_TailVolume);
	}
	else str =" ";
	m_ctrlStabVolume.SetWindowText(str);

	int total = m_Wing.VLMGetPanelTotal();
	if(m_bBiplane)	total += m_Wing2.VLMGetPanelTotal();
	if(m_bStab)		total += m_Stab.VLMGetPanelTotal();
	if(m_bFin)  {
		if(m_bDoubleFin ||m_bSymFin) total += 2*m_Fin.VLMGetPanelTotal();
		else total += m_Fin.VLMGetPanelTotal();
	}
	m_ctrlVLMTotalPanels.SetValue(total);
	str.Format("%5.2e", m_Volume*pFrame->m_mtoUnit*pFrame->m_mtoUnit*pFrame->m_mtoUnit);
	m_ctrlVolume.SetWindowText(str);
}


void CPlane::ComputePlane(void)
{
	if(m_bStab){
		double SLA = m_LEStab.x + m_Stab.m_TChord[0]/4.0 - m_Wing.m_TChord[0]/4.0;
		double area = m_Wing.m_Area;
		if(m_bBiplane) area += m_Wing2.m_Area;

		double ProjectedArea = 0.0;
		for (int i=0;i<m_Stab.m_NPanel; i++){
			ProjectedArea += m_Stab.m_TLength[i+1]*(m_Stab.m_TChord[i]+m_Stab.m_TChord[i+1])/2.0
							*cos(m_Stab.m_TDihedral[i])*cos(m_Stab.m_TDihedral[i]);//m²
		
		}
		ProjectedArea *=2.0;
		m_TailVolume = ProjectedArea * SLA / area/m_Wing.m_MAChord ;
	}
	else m_TailVolume = 0.0;
	
	m_Fin.m_bDoubleFin = m_bDoubleFin;
	m_Fin.m_bSymFin    = m_bSymFin;
	m_Volume = m_Wing.m_Volume;
	if(m_bBiplane)	m_Volume += m_Wing2.m_Volume;
	if(m_bStab)		m_Volume += m_Stab.m_Volume;
	if(m_bFin)	{
		m_Volume += m_Fin.m_Volume;
		if(m_bDoubleFin || m_bSymFin) m_Volume += m_Fin.m_Volume;
	}
}


void CPlane::CheckPanelDisposition()
{
	m_Wing.ComputeGeometry();
	if(!m_bStab) return;

/*	for (int i=0; i<=m_Stab.m_NPanel; i++){
		for(int j=0; j<m_Wing.m_NPanel; j++){
			if(m_Wing.m_TPos[j]>m_Stab.m_TPos[i] || fabs(m_Wing.m_TPos[j]-m_Stab.m_TPos[i])<1.0){
				if(fabs(m_Wing.m_TPos[j]-m_Stab.m_TPos[i])>0.001){
					if(m_Wing.m_TPos[j]<m_Stab.m_TPos[i]) m_Wing.m_TPos[j] = m_Stab.m_TPos[i];
					else {
						m_Wing.SplitPanel(j, m_Stab.m_TPos[i]/m_Wing.m_Span*2.0);
						m_Wing.m_TPos[j] = m_Stab.m_TPos[i];
					}
				}
			}
		}
	}*/
	for (int i=1; i<=m_Stab.m_NPanel; i++){
		m_Wing.InsertSection(m_Stab.m_TPos[i]);
	}
	m_Wing.ComputeGeometry();
	for (i=0; i<m_Stab.m_NPanel; i++){
		m_Wing.m_YPanelDist[i] = m_Stab.m_YPanelDist[i];
		m_Wing.m_NYPanels[i]   = m_Stab.m_NYPanels[i];
	}
	for (i=0; i<m_Wing.m_NPanel; i++){
		if(m_Wing.m_NYPanels[i] == 0) m_Wing.m_NYPanels[i] = 1;
	}
	m_Wing.m_bVLMAutoMesh = false;
}


void CPlane::OnImportWing() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CImportWingDlg dlg;
	dlg.m_WingName = m_Wing.m_WingName;
	dlg.m_poaWing = m_poaWing;

	if(dlg.DoModal() ==IDOK){
		m_bChanged = true;
		CWing *pWing = pFrame->Miarex.GetWing(dlg.m_WingName);
		if(pWing) {
			m_Wing.Duplicate(pWing);
			m_Wing.m_WingColor = pWing->m_WingColor;
		}
	}
}


void CPlane::OnExportWing() 
{

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CWing *pNewWing = new CWing();
	pNewWing->Duplicate(&m_Wing);
	pNewWing->m_WingName = "";
	pFrame->Miarex.AddWing(pNewWing);
	pFrame->Miarex.UpdateUFOs();
	pFrame->Miarex.UpdateView();

}
void CPlane::OnImportWing2() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CImportWingDlg dlg;
	dlg.m_WingName = m_Wing2.m_WingName;
	dlg.m_poaWing = m_poaWing;

	if(dlg.DoModal() ==IDOK){
		m_bChanged = true;
		CWing *pWing = pFrame->Miarex.GetWing(dlg.m_WingName);
		if(pWing) {
			m_Wing2.Duplicate(pWing);
			m_Wing2.m_WingColor = pWing->m_WingColor;
		}
	}
}


void CPlane::OnExportWing2() 
{

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CWing *pNewWing = new CWing();
	pNewWing->Duplicate(&m_Wing2);
	pNewWing->m_WingName = "";
	pFrame->Miarex.AddWing(pNewWing);
	pFrame->Miarex.UpdateUFOs();
	pFrame->Miarex.UpdateView();

}
void CPlane::OnPlaneName() 
{
	m_ctrlPlaneName.GetWindowText(m_PlaneName);
	m_Wing.m_WingName = m_PlaneName+"_Wing";
	m_Stab.m_WingName = m_PlaneName+"_Elev";
	m_Fin.m_WingName = m_PlaneName+"_Fin";
}


void CPlane::OnDoubleFin()
{
	if (m_ctrlDoubleFin.GetCheck()) {
		m_ctrlYLEFin.EnableWindow(true);
		m_ctrlRadio1.EnableWindow(true);
		m_ctrlRadio2.EnableWindow(true);
		m_bDoubleFin = true;
		m_bSymFin    = false;
		m_ctrlSymFin.SetCheck(false);
		if(m_bDoubleSymFin){
			CheckRadioButton(IDC_RADIO1,IDC_RADIO2,IDC_RADIO1);
		}
		else{
			CheckRadioButton(IDC_RADIO1,IDC_RADIO2,IDC_RADIO2);
		}
	}
	else{
        m_ctrlYLEFin.EnableWindow(false);
		m_ctrlRadio1.EnableWindow(false);
		m_ctrlRadio2.EnableWindow(false);
		m_bDoubleFin = false;
	}
	m_bChanged = true;
	SetResults();
}

void CPlane::OnDoubleSymFin() 
{	
	if (GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO2)==IDC_RADIO1) {
		m_bDoubleSymFin    = false;
	}
	else{
		m_bDoubleSymFin    = true;
	}
	m_bChanged = true;
}


void CPlane::OnSymFin() 
{	
	if (m_ctrlSymFin.GetCheck()) {
		m_ctrlYLEFin.EnableWindow(false);
		m_bSymFin    = true;
		m_bDoubleFin = false;
		m_ctrlDoubleFin.SetCheck(false);
		m_ctrlYLEFin.EnableWindow(false);
		m_ctrlRadio1.EnableWindow(false);
		m_ctrlRadio2.EnableWindow(false);
	}
	else{
		m_ctrlDoubleFin.EnableWindow(true);
		m_ctrlRadio1.EnableWindow(true);
		m_ctrlRadio2.EnableWindow(true);
		m_ctrlYLEFin.EnableWindow(true);
		m_bSymFin = false;
	}
	m_bChanged = true;
	SetResults();
}

void CPlane::OnCheckPanels() 
{
	if(m_ctrlCheckPanels.GetCheck())
		m_bCheckPanels = true;
	else
		m_bCheckPanels = false;

}

BOOL CPlane::PreTranslateMessage(MSG* pMsg) 
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

void CPlane::OnBiplane()
{
	if(m_ctrlBiplane.GetCheck()) m_bBiplane = true; else m_bBiplane = false;
	if(m_bBiplane){
		m_ctrlDefineWing2.EnableWindow(TRUE);
		m_ctrlImportWing2.EnableWindow(TRUE);
		m_ctrlExportWing2.EnableWindow(TRUE);
		m_ctrlWingTilt2.EnableWindow(TRUE);	
		m_ctrlZLEWing2.EnableWindow(TRUE);
		m_ctrlXLEWing2.EnableWindow(TRUE);
	}
	else{
		m_ctrlDefineWing2.EnableWindow(FALSE);
		m_ctrlImportWing2.EnableWindow(FALSE);
		m_ctrlExportWing2.EnableWindow(FALSE);
		m_ctrlWingTilt2.EnableWindow(FALSE);	
		m_ctrlZLEWing2.EnableWindow(FALSE);
		m_ctrlXLEWing2.EnableWindow(FALSE);
	}
}

