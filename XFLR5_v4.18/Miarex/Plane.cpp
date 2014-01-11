/****************************************************************************

    CPlane Class
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
#include "WingDlg.h"
#include ".\plane.h"

CWnd*	CPlane::s_pFrame;
CWnd*	CPlane::s_pMiarex ;

//
CPlane::CPlane()
{
	m_pBody   = NULL;

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
	
	m_BodyPos.Set(0.0, 0.0, 0.0);

	m_Volume        =   0.0;

	m_bActive       = false;
	m_bBody         = false;
	m_bFin          = true;
	m_bDoubleFin    = false;
	m_bSymFin       = false;
	m_bStab         = true;
	m_bDoubleSymFin = true;
	m_bBiplane      = false;

	m_PlaneName  = "Plane Name";
}



bool CPlane::SerializePlane(CArchive& ar) 
{
	CMiarex *pMiarex = (CMiarex*)s_pMiarex;
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	CString strong = "Nobody";
	int ArchiveFormat;// identifies the format of the file
	if (ar.IsStoring())
	{	// storing code
		ar << 1010;
		//1010 : added body LE x and z position
		//1009 : added Main wing LE x and z position
		//1008 : added body data
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
//		if(m_bCheckPanels)   ar <<1; else ar <<0;
		ar << 0;
		if(m_bBiplane)       ar <<1; else ar <<0;
		
		ar << m_LEWing2.x << m_LEWing2.y << m_LEWing2.z << m_WingTilt2; 
		ar << m_LEStab.x << m_LEStab.y << m_LEStab.z; 
		ar << m_WingTilt << m_StabTilt << m_FinTilt;
		ar << m_LEFin.x << m_LEFin.y << m_LEFin.z; 
		ar << m_BodyPos.x << m_BodyPos.z; 

		ar << m_LEWing.x << m_LEWing.z;

		if(m_bBody && m_pBody)  {
			ar <<1; 
			ar << m_pBody->m_BodyName;
		}
		else{
			ar << 0;
			ar << strong;
		}
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
//			if(k) m_bCheckPanels = true;  else m_bCheckPanels = false;
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

		if(ArchiveFormat>=1010)
		{
			ar >> m_BodyPos.x >> m_BodyPos.z; 
		}
		if(ArchiveFormat>=1009)
		{
			ar>> m_LEWing.x >> m_LEWing.z;
		}

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
		if(ArchiveFormat>=1008)
		{
			ar >> k;
			if(k)  m_bBody=true; else m_bBody=false;
			ar >> strong;
			m_pBody = pMiarex->GetBody(strong);
		}
		else m_pBody = NULL;

		ComputePlane();
		return true;
	}
}


void CPlane::ComputePlane(void)
{
	int i;
	if(m_bStab)
	{
		double SLA = m_LEStab.x + m_Stab.m_TChord[0]/4.0 - m_Wing.m_TChord[0]/4.0;
		double area = m_Wing.m_Area;
		if(m_bBiplane) area += m_Wing2.m_Area;

		double ProjectedArea = 0.0;
		for (i=0;i<m_Stab.m_NPanel; i++)
		{
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
	if(m_bFin)	
	{
		m_Volume += m_Fin.m_Volume;
		if(m_bDoubleFin || m_bSymFin) m_Volume += m_Fin.m_Volume;
	}
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

	m_WingTilt   = pPlane->m_WingTilt;
	m_WingTilt2  = pPlane->m_WingTilt2;
	m_StabTilt   = pPlane->m_StabTilt;

	m_LEWing.Copy(pPlane->m_LEWing);
	m_LEWing2.Copy(pPlane->m_LEWing2);
	m_LEFin.Copy(pPlane->m_LEFin);
	m_LEStab.Copy(pPlane->m_LEStab);
	m_BodyPos.Copy(pPlane->m_BodyPos);

	m_Wing.Duplicate(&pPlane->m_Wing);
	m_Wing2.Duplicate(&pPlane->m_Wing2);
	m_Fin.Duplicate(&pPlane->m_Fin);
	m_Stab.Duplicate(&pPlane->m_Stab);

	m_Wing.m_WingColor  = pPlane->m_Wing.m_WingColor;
	m_Wing2.m_WingColor = pPlane->m_Wing2.m_WingColor;
	m_Stab.m_WingColor  = pPlane->m_Stab.m_WingColor;
	m_Fin.m_WingColor   = pPlane->m_Fin.m_WingColor;

	m_bBody = pPlane->m_bBody ;
	m_pBody = pPlane->m_pBody;

}


bool CPlane::HasResults()
{
	CWPolar *pWPolar;
	CMainFrame *pMainFrame = (CMainFrame*)s_pFrame;
	bool bHasResults = false;
	for (int i=0; i< pMainFrame->m_oaWPolar.GetSize(); i++)
	{
		pWPolar = (CWPolar*)pMainFrame->m_oaWPolar.GetAt(i);
		if (pWPolar->m_Alpha.GetSize() && pWPolar->m_UFOName == m_PlaneName)
		{
			return true;
		}
	}

	return false;
}






















