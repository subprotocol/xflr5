/****************************************************************************

    Wing Class
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

// Wing.cpp : implementation file
// This class is both the interface class associated to the wing definition dialog box
// and the class which implements all calculation routines associated to the wing,
// including LLT and VLM methods
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "../misc/UnitsDlg.h"
#include "Miarex.h"
#include "VLMDlg.h"
#include "Surface.h"
#include  <math.h>
#include ".\wing.h"


/////////////////////////////////////////////////////////////////////////////
// CWing dialog
CVector *CWing::m_pWakeNode;			//pointer to the VLM wake node array
CPanel  *CWing::m_pWakePanel;			//pointer to the VLM Wake Panel array

CWnd* CWing::s_pFrame;		//pointer to the Frame window
CWnd* CWing::s_pMiarex;	//pointer to the Miarex Application window
CWnd* CWing::s_pVLMDlg;	//pointer to the VLM analysis dialog class
CWnd* CWing::s_pLLTDlg;	//pointer to the VLM analysis dialog class
CWnd* CWing::s_p3DPanelDlg;//pointer to the 3DPanel analysis dialog class
int CWing::s_NLLTStations;//pointer to the 3DPanel analysis dialog class
double CWing::s_CvPrec;	// Precision required for LLT convergence
double CWing::s_RelaxMax;	// relaxation factor for LLT convergence


CWing::CWing(CWnd* pParent /*=NULL*/)
{
	int i;
	memset(m_Ai, 0, sizeof(m_Ai));
	memset(m_Twist, 0, sizeof(m_Twist));
	memset(m_Cl, 0, sizeof(m_Cl));
	memset(m_PCd, 0, sizeof(m_PCd));
	memset(m_ICd, 0, sizeof(m_ICd));
	memset(m_Cm, 0, sizeof(m_Cm));
	memset(m_CmAirf, 0, sizeof(m_CmAirf));
	memset(m_CmXRef, 0, sizeof(m_CmXRef));
	memset(m_XCPSpanAbs, 0, sizeof(m_XCPSpanAbs));
	memset(m_XCPSpanRel, 0, sizeof(m_XCPSpanRel));
	memset(m_Re, 0, sizeof(m_Re));
	memset(m_Chord, 0, sizeof(m_Chord));
	memset(m_Offset, 0, sizeof(m_Offset));
	memset(m_XTrTop, 0, sizeof(m_XTrTop));
	memset(m_XTrBot, 0, sizeof(m_XTrBot));
	memset(m_BendingMoment, 0, sizeof(m_BendingMoment));
	memset(m_Twist, 0, sizeof(m_Twist));
	memset(m_xPanel, 0, MAXCHORDPANELS*sizeof(double));
	memset(m_xHinge, 0, MAXCHORDPANELS*sizeof(double));

	memset(m_TPos, 0, sizeof(m_TPos));
	memset(m_TYProj, 0, sizeof(m_TYProj));
	memset(m_TChord, 0, sizeof(m_TChord));
	memset(m_TLength, 0, sizeof(m_TLength));
	memset(m_TOffset, 0, sizeof(m_TOffset));
	memset(m_TDihedral, 0, sizeof(m_TDihedral));
	memset(m_TZPos, 0, sizeof(m_TZPos));
	memset(m_TTwist, 0, sizeof(m_TTwist));
	memset(m_SpanPos, 0, sizeof(m_SpanPos));
	memset(m_StripArea, 0, sizeof(m_StripArea));

	memset(m_NXPanels, 0, sizeof(m_NXPanels));
	memset(m_NYPanels, 0, sizeof(m_NYPanels));
	memset(m_XPanelDist, 0, sizeof(m_XPanelDist));
	memset(m_YPanelDist, 0, sizeof(m_YPanelDist));
	memset(m_VLMQInf, 0, sizeof(m_VLMQInf));
//	memset(m_GeomMoment, 0, sizeof(m_GeomMoment));

	m_bIsFin        = false;
	m_bDoubleFin    = false;
	m_bSymFin       = false;
	m_bDoubleSymFin = false;
	m_bTrans        = false;
	m_bLLT          = false;
	m_bVLM1         = true;
	m_bVLMAutoMesh  = true;
	m_bCancel       = false;
	m_bInitCalc     = false;
	m_bSymetric     = true;
	m_bTrace        = true;
	m_bWingOut      = false;
	m_bConverged    = true;
	m_bVLMSymetric  = false;
	m_bChanged      = false;

	m_WingName  = "Wing Name";
	m_WingColor =  16762944;

	m_QInf     = 0.0;
	m_QInf0    = 0.0;
	m_Weight   = 0.0;
	m_Maxa     = 0.0;

	m_pXFile     = NULL;
	m_pPanel     = NULL;
	m_pWakeNode  = NULL; 
	m_pWakePanel = NULL;

	m_CL                = 0.0;
	m_ViscousDrag       = 0.0;
	m_InducedDrag       = 0.0;
//	m_RollingMoment     = 0.0;
//	m_PitchingMoment    = 0.0;
	m_GYm               = 0.0;
	m_IYm               = 0.0;
	m_GCm               = 0.0;
	m_GRm               = 0.0;
	m_VCm               = 0.0;
	m_VYm               = 0.0;
	m_XCP               = 0.0;
	m_YCP               = 0.0;

	m_Type = 1;
	m_Viscosity = 0.0;
	m_Density   = 0.0;

	m_AVLIndex = -987654;//improbable value...


	m_iSection   = 0;
	m_nNodes     = 0;
	m_MatSize    = 0;

	pi = 3.141592654;


	m_NSurfaces = 0;

	m_Alpha      = 0.0;
	m_Span       = 0.0;
	m_Area       = 0.0;
	m_Volume     = 0.0;
	m_AR         = 0.0;// Aspect ratio
	m_TR         = 0.0;// Taper ratio
	m_GChord     = 0.0;// mean geometric chord
	m_MAChord    = 0.0;// mean aero chord
	m_yMac       = 0.0;
//	s_CvPrec     = 0.01;
//	s_RelaxMax   = 10.0;
//	s_NLLTStations  = 20;

	m_NPanel        =  1;
	m_nFlaps        =  0;
	m_NYPanels[0]   = 12;
	m_NXPanels[0]   =  8;
	m_NXPanels[1]   =  8;
	m_YPanelDist[0] = 0;

	m_TChord[0]  =  .180;
	m_TChord[1]  =  .120;
	m_TLength[0] =  .0;
	m_TLength[1] = 1.0;
	m_TOffset[0] = 0.;
	m_TOffset[1] = 0.060;

	double length = m_TLength[0];
	for (i=0; i<=MAXPANELS; i++)
	{
		length += m_TLength[i];
		m_TPos[i]     = length;
		m_XPanelDist[i] =  1;
	}

	m_RFoil.SetSize(MAXPANELS+1);
	for (i=0;i<=MAXPANELS; i++){
		m_RFoil[i]    = "";
	}

	m_LFoil.SetSize(MAXPANELS+1);
	for (i=0;i<=MAXPANELS; i++){
		m_LFoil[i]    = "";
	}
}







bool CWing::SerializeWing(CArchive& ar) 
{
	//saves or loads the wing data to the archive ar

	int i;
//	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	int ArchiveFormat;// identifies the format of the file
	if (ar.IsStoring())
	{	// storing code
		ar << 1007;
			//1007 : Changed length units to m
			//1006 : Added Wing Color v2.99-15
			//1005 : Added Chordwise spacing (v2.99-00)
			//1004 : corrected m_NXPanels, m_NYPanels, m_YPanelDist to int (v1.99-33)
			//1003 : AVL Format (v1.99-18)
			//1002 : save VLM Mesh (v1.99-12)
			//1001 : initial format

		ar << m_WingName;
		ar << 0; //non elliptic...

		if(m_bSymetric) ar << 1; else ar<<0;

		ar << m_NPanel;

		for (i=0; i<=m_NPanel; i++) ar << m_RFoil[i];
		for (i=0; i<=m_NPanel; i++) ar << m_LFoil[i];
		for (i=0; i<=m_NPanel; i++) ar << (float)m_TChord[i];
		for (i=0; i<=m_NPanel; i++) ar << (float)m_TPos[i];
		for (i=0; i<=m_NPanel; i++) ar << (float)m_TOffset[i];
		for (i=0; i<=m_NPanel; i++) ar << (float)m_TDihedral[i];
		for (i=0; i<=m_NPanel; i++) ar << (float)m_TTwist[i];

//		ar << (float)m_XCmRef;
		ar << 0.0f;
		
		if(m_bVLMAutoMesh) ar << 1; else ar<<0;
		for (i=0; i<=m_NPanel; i++) ar << m_NXPanels[i];
		for (i=0; i<=m_NPanel; i++) ar << m_NYPanels[i];
		for (i=0; i<=m_NPanel; i++) ar << m_XPanelDist[i];//1005
		for (i=0; i<=m_NPanel; i++) ar << m_YPanelDist[i];

		ar << m_WingColor;
		return true;
	}
	else
	{	// loading code
		float f;
		int k;
		ar >> ArchiveFormat;
		if (ArchiveFormat <1001 || ArchiveFormat>1100) {
			m_WingName = "";
			return false;
		}

		ar >> m_WingName;
		if (m_WingName.GetLength() ==0) {
			return false;
		}
		ar >> k;
		if(k!=0){
			m_WingName = "";
			return false;
		}
		ar >> k;
		if (k==1) m_bSymetric = true;
		else if (k==0) m_bSymetric = false;
		else{
			m_WingName = "";
			return false;
		}
		m_bVLMSymetric = m_bSymetric;

		ar >> m_NPanel;
		if(m_NPanel <=0 || m_NPanel>=100) {
			m_WingName = "";
			return false;
		}
		for (i=0; i<=m_NPanel; i++) ar >> m_RFoil[i];
		for (i=0; i<=m_NPanel; i++) ar >> m_LFoil[i];
		for (i=0; i<=m_NPanel; i++) {
			ar >> f; m_TChord[i]=f;
			if (abs(m_TChord[i]) <0.0) {
				m_WingName = "";
				return false;
			}
		}

		for (i=0; i<=m_NPanel; i++) {
			ar >> f; m_TPos[i]=f;
			if (abs(m_TPos[i]) <0.0) {
				m_WingName = "";
				return false;
			}
		}
		for (i=0; i<=m_NPanel; i++) {
			ar >> f; m_TOffset[i]=f;
		}

		if(ArchiveFormat<1007){
			//convert mm to m
			for (i=0; i<=m_NPanel; i++) {
				m_TPos[i]    /= 1000.0;
				m_TChord[i]  /= 1000.0;
				m_TOffset[i] /= 1000.0;
			}

		}
		for (i=0; i<=m_NPanel; i++) {
			ar >> f; m_TDihedral[i]=f;
		}
		for (i=0; i<=m_NPanel; i++)	{
			ar >> f; m_TTwist[i]=f;
		}

		ar >> f; //m_XCmRef=f;
//		if(ArchiveFormat<1007) m_XCmRef /= 1000.0;

		ComputeGeometry();

		ar >> k;
		if (k==1)      m_bVLMAutoMesh = true;
		else if (k==0) m_bVLMAutoMesh = false;
		else{
			m_WingName = "";
			return false;
		}

		for (i=0; i<=m_NPanel; i++) {
			if(ArchiveFormat<=1003){
				ar >> f;
				m_NXPanels[i] = (int)f;
			}
			else 
				ar >> m_NXPanels[i];
			m_NXPanels[i] = max(1,m_NXPanels[i] );
			m_NXPanels[i] = min(MAXCHORDPANELS, m_NXPanels[i]);
		}
		for (i=0; i<=m_NPanel; i++) {
			if(ArchiveFormat<=1003){
				ar >> f;
				m_NYPanels[i] = (int)f;
			}
			else 	ar >> m_NYPanels[i];
			m_NYPanels[i] = max(1,m_NYPanels[i] );
			m_NYPanels[i] = min(50, m_NYPanels[i]);
		}
		int total = 0;
		for (i=0; i<m_NPanel; i++){
			total += m_NYPanels[i];
		}

		if(total*2>=MAXSTATIONS){
			double ratio = MAXSTATIONS/total/2.0;
			for (i=0; i<=m_NPanel; i++) {
				m_NYPanels[i] = (int) (ratio*m_NYPanels[i]);
			}
		}

		int spanpos = 0;
		int vlmpanels = 0;
		for (int l=0; l<m_NPanel; l++){
			spanpos += m_NYPanels[l];
			vlmpanels +=m_NXPanels[l]*m_NYPanels[l];
		}


		if (ArchiveFormat >=1005){
			for (i=0; i<=m_NPanel; i++) ar >> m_XPanelDist[i];
		}

		for (i=0; i<=m_NPanel; i++) {
			ar >> m_YPanelDist[i];
			if (ArchiveFormat <1003){
				if (m_YPanelDist[i] ==  2) m_YPanelDist[i] = -2;
				if (m_YPanelDist[i] == -2) m_YPanelDist[i] =  2;
			}
		}

		if(ArchiveFormat>=1006){
			ar >> m_WingColor;
		}
		return true;
	}
}


void CWing::ComputeDihedrals()
{
	for (int n=0; n<m_NPanel; n++){
		m_TDihedral[n] = 180.0/pi* atan2(m_TZPos[n+1]-m_TZPos[n], m_TPos[n+1]-m_TPos[n]);
	}
}

void CWing::Duplicate(CWing *pWing)
{
	//Copies the wing data froman existing wing
	int i;
	s_pFrame		= pWing->s_pFrame;
	s_pMiarex       = pWing->s_pMiarex;
	m_NStation		= pWing->m_NStation;
	m_NPanel		= pWing->m_NPanel;
	m_Span			= pWing->m_Span;
	m_Area		    = pWing->m_Area;
	m_ProjectedSpan = pWing->m_ProjectedSpan;
	m_ProjectedArea = pWing->m_ProjectedArea;
	m_AR			= pWing->m_AR;
	m_TR			= pWing->m_TR;
	m_GChord		= pWing->m_GChord;
	m_MAChord		= pWing->m_MAChord;
	m_bTrace		= pWing->m_bTrace;
	m_WingName		= pWing->m_WingName;
	m_bVLMAutoMesh	= pWing->m_bVLMAutoMesh;
	m_bSymetric     = pWing->m_bSymetric;
	m_bIsFin        = pWing->m_bIsFin;
	m_bSymFin       = pWing->m_bSymFin;
	m_bDoubleFin    = pWing->m_bDoubleFin;
	m_bDoubleSymFin = pWing->m_bDoubleSymFin;

	for (i=0; i<=MAXPANELS; i++)
	{
		m_TChord[i]     = pWing->m_TChord[i];
		m_TPos[i]       = pWing->m_TPos[i];
		m_TOffset[i]    = pWing->m_TOffset[i];
		m_TLength[i]    = pWing->m_TLength[i];
		m_NXPanels[i]   = pWing->m_NXPanels[i] ;
		m_NYPanels[i]   = pWing->m_NYPanels[i]; 
		m_XPanelDist[i] = pWing->m_XPanelDist[i]; 
		m_YPanelDist[i] = pWing->m_YPanelDist[i]; 
		m_RFoil[i]      = pWing->m_RFoil[i];
		m_LFoil[i]      = pWing->m_LFoil[i];
		m_TTwist[i]     = pWing->m_TTwist[i];
		m_TDihedral[i]  = pWing->m_TDihedral[i];
		m_TZPos[i]      = pWing->m_TZPos[i];
		m_TYProj[i]     = pWing->m_TYProj[i];
	}

	m_nFlaps = pWing->m_nFlaps;
}

/*	if(!m_bIsFin){
		m_Area    = 2.0 * surface;
		m_Volume *= 2.0;
		m_MAChord = m_MAChord * 2.0 / m_Area;
		m_yMac    = m_yMac   * 2.0 / m_Area;

		m_GChord  = m_Area/m_Span;
		m_AR      = m_Span*m_Span/m_Area;
	}
	else{ 
		m_Area = surface;
		m_MAChord = m_MAChord / m_Area;
		m_yMac    = m_yMac    / m_Area;

		m_GChord  = m_Area/m_Span*2.0;
		m_AR      = m_Span*m_Span/m_Area/2.0;
	}*/

void CWing::ComputeGeometry()
{
	// Computes the wing's characteristics from the panel data
	CMainFrame *pFrame  = (CMainFrame*)s_pFrame;
	CMiarex    *pMiarex = (CMiarex*)s_pMiarex;
	CFoil *pFoilA, *pFoilB;
	double MinPanelSize;
	int i, k;

	double surface = 0.0;
	double xysurface = 0.0;
	m_TLength[0] = 0.0;
	m_TYProj[0]  = m_TPos[0];
	for (i=1; i<=m_NPanel; i++)
		m_TLength[i] = m_TPos[i] - m_TPos[i-1];
	
	for (i=1; i<=m_NPanel; i++)
		m_TYProj[i] = m_TYProj[i-1] + m_TLength[i] * cos(m_TDihedral[i-1]*pi/180.0);
	

	m_Span    = 2.0 * m_TPos[m_NPanel]; 
	m_ProjectedSpan = 0.0;
	m_MAChord = 0.0;
	m_yMac    = 0.0;
	m_Volume  = 0.0;
	for (k=0; k<m_NPanel; k++)
	{
		pFoilA = pFrame->GetFoil(m_RFoil[k]);
		pFoilB = pFrame->GetFoil(m_RFoil[k+1]);
		surface   += m_TLength[k+1]*(m_TChord[k]+m_TChord[k+1])/2.0;//m²
		xysurface += (m_TLength[k+1]*(m_TChord[k]+m_TChord[k+1])/2.0) * cos(m_TDihedral[k]*pi/180.0);
		m_ProjectedSpan += m_TLength[k+1] * cos(m_TDihedral[k]*pi/180.0);
		if(pFoilA && pFoilB) 
			m_Volume  += m_TLength[k+1]*(pFoilA->GetArea()*m_TChord[k] + pFoilB->GetArea()*m_TChord[k+1])/2.0;//m3
		m_MAChord += IntegralC2(m_TPos[k], m_TPos[k+1], m_TChord[k], m_TChord[k+1]);
		m_yMac    += IntegralCy(m_TPos[k], m_TPos[k+1], m_TChord[k], m_TChord[k+1]);
	}

	m_ProjectedSpan *=2.0;

	if(!m_bIsFin || m_bSymFin || m_bDoubleFin)
	{
		m_Area    = 2.0 * surface;
		m_ProjectedArea = 2.0 * xysurface;
		m_Volume *= 2.0;
		m_MAChord = m_MAChord * 2.0 / m_Area;
		m_yMac    = m_yMac    * 2.0 / m_Area;

		m_GChord  = m_Area/m_Span;
		m_AR      = m_Span*m_Span/m_Area;
	}
	else
	{ 
		m_Area = surface;
		m_ProjectedArea = xysurface;
		m_MAChord = m_MAChord / m_Area;
		m_yMac    = m_yMac    / m_Area;

		m_GChord  = m_Area/m_Span*2.0;
		m_AR      = m_Span*m_Span/m_Area/2.0;
	}
	if(m_TChord[m_NPanel]>0.0)	m_TR = m_TChord[0]/m_TChord[m_NPanel];
	else						m_TR = 99999.0;

	//calculate the number of flaps
	m_nFlaps = 0;
	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
	else                            MinPanelSize = m_Span;

	for (i=1; i<=m_NPanel; i++)
	{
		pFoilA = pFrame->GetFoil(m_RFoil[i-1]);
		pFoilB = pFrame->GetFoil(m_RFoil[i]);
		if(pFoilA && pFoilB)
		{
			if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && abs(m_TPos[i]-m_TPos[i-1])>MinPanelSize)	m_nFlaps++;
		}

		pFoilA = pFrame->GetFoil(m_LFoil[i-1]);
		pFoilB = pFrame->GetFoil(m_LFoil[i]);
		if(pFoilA && pFoilB)
		{
			if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && abs(m_TPos[i]-m_TPos[i-1])>MinPanelSize)	m_nFlaps++;
		}
	}
}



void CWing::ComputeChords(int NStation)
{
	//Calculates the chord lengths for NStation in LLT calculations
	int j,k,l,m;
	double y, yob, tau;
	double x0,y0,y1,y2;
	double SpanPosition[MAXSTATIONS];
	CVector C;
		
	if(NStation !=0){//LLT based
		m_NStation = NStation;

		for (k=0; k<=NStation; k++)
		{
			yob   = cos(k*pi/NStation);
			y = abs(yob * m_Span/2);
			for (l=0; l<=m_NPanel; l++)
			{
				if(m_TPos[l] < y && y <=m_TPos[l+1])
				{
					tau = (y-m_TPos[l])/(m_TPos[l+1]-m_TPos[l]);
					m_Chord[k]  = m_TChord[l]+(m_TChord[l+1]-m_TChord[l]) * tau;
					m_Offset[k] = m_TOffset[l]+(m_TOffset[l+1]-m_TOffset[l]) * tau;
					break;
				}
			}
		}
	}
	else
	{
		//VLM Mesh based
		m_NStation    = 0;
		m = 0;

		x0 = m_Surface[m_NSurfaces/2].m_LA.x;
		y0 = m_Surface[m_NSurfaces/2].m_LA.y;

		for (j=m_NSurfaces/2; j<m_NSurfaces; j++)
		{
			for (k=0; k<m_Surface[j].m_NYPanels; k++)
			{
				//calculate span positions at each station
				m_Surface[j].GetyDist(k, y1, y2);
				SpanPosition[m] = y0 + (y1+y2)/2.0*m_Surface[j].m_Length;
				m++;
			}
			y0+=m_Surface[j].m_Length;
		}

		m_NStation = 2*m;
		for (m=0; m<m_NStation/2; m++){
			m_SpanPos[m] = -SpanPosition[m_NStation/2-m-1];
			m_SpanPos[m+m_NStation/2] = SpanPosition[m];
		}

		m=0;
		for (j=0; j<m_NSurfaces; j++)
		{
			for (k=0; k<m_Surface[j].m_NYPanels; k++)
			{
				//calculate chords and offsets at each station
				m_Chord[m]     = m_Surface[j].GetChord(k);
//				m_StripArea[m] = m_Chord[m]* m_Surface[j].Getdl(k);

				m_Surface[j].GetLeadingPt(k, C);
				m_Offset[m] = C.x-x0;
				
				m_Twist[m]  = m_Surface[j].GetTwist(k);
				m++;
			}
		}
		m_NStation = m;
	}
}



bool CWing::ExportAVLWing(CStdioFile *pXFile, int index, double x, double y, double z, double Thetax, double Thetay)
{ 
	// Exports the current wing to AVL format

	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	CStdioFile XFile;
	CFileException fe;
	int j;
	CString strong, str;

	pXFile->WriteString("#=================================================\n");
	pXFile->WriteString("SURFACE                      | (keyword)\n");
	pXFile->WriteString(m_WingName);
	pXFile->WriteString("\n");
	pXFile->WriteString("#Nchord    Cspace   [ Nspan Sspace ]\n");			

	strong.Format("%3d        %3.1f\n", m_NXPanels[0], 1.0);
	pXFile->WriteString(strong);


	pXFile->WriteString("\n");
	pXFile->WriteString("INDEX                        | (keyword)\n");
	strong.Format("%3d                          | Lsurf\n", index);
	pXFile->WriteString(strong);

	if(!m_bIsFin)
	{
		pXFile->WriteString("\n");
		pXFile->WriteString("YDUPLICATE\n");
		pXFile->WriteString("0.0\n");
	}
	else if(m_bDoubleFin)
	{
		pXFile->WriteString("\n");
		pXFile->WriteString("YDUPLICATE\n");
		strong.Format("%9.4f\n", y);
		pXFile->WriteString(strong);
	}

	pXFile->WriteString("\n");
	pXFile->WriteString("SCALE\n");
	pXFile->WriteString("1.0  1.0  1.0\n");

	pXFile->WriteString("\n");
	pXFile->WriteString("TRANSLATE\n");
	pXFile->WriteString("0.0  0.0  0.0\n");

	pXFile->WriteString("\n");
	pXFile->WriteString("ANGLE\n");
	strong.Format("%8.3f                         | dAinc\n", Thetay);
	pXFile->WriteString(strong);

	pXFile->WriteString("\n\n");

	//write only right wing surfaces since we have provided YDUPLICATE
	CSurface ASurface;
	int iFlap = 1;
	int iSurface = 0;
	for(j=(int)(m_NSurfaces/2); j<m_NSurfaces; j++)
	{
		ASurface.Copy(m_Surface[j]);

		//Remove twist, since AVL processes it as a mod of the angle of attack thru the dAInc command
		ASurface.m_TwistA = ASurface.m_TwistB = 0.0;
		ASurface.SetTwist();
		pXFile->WriteString("#______________\nSECTION                                                     |  (keyword)\n");
		
		strong.Format("%9.4f %9.4f %9.4f %9.4f %7.3f  %3d  %3d   | Xle Yle Zle   Chord Ainc   [ Nspan Sspace ]\n",
			ASurface.m_LA.x          *pFrame->m_mtoUnit,
			ASurface.m_LA.y          *pFrame->m_mtoUnit,
			ASurface.m_LA.z          *pFrame->m_mtoUnit,
			ASurface.GetChord(0.0)   *pFrame->m_mtoUnit,
			m_Surface[j].m_TwistA,
			ASurface.m_NYPanels,
			ASurface.m_YDistType);
		pXFile->WriteString(strong);
		pXFile->WriteString("\n");
		pXFile->WriteString("AFIL 0.0 1.0\n");
		pXFile->WriteString(ASurface.m_pFoilA->m_FoilName +".dat\n");
		pXFile->WriteString("\n");
		if(ASurface.m_bTEFlap) 
		{
			pXFile->WriteString("CONTROL                                                     |  (keyword)\n");
			str.Format("_Flap_%d  ",iFlap);
			strong = m_WingName;
			strong.Replace(" ", "_");
			strong += str;
			str.Format("%5.2f  ", 2.0/(ASurface.m_pFoilA->m_TEFlapAngle + ASurface.m_pFoilB->m_TEFlapAngle));
			strong += str;
			str.Format("%5.3f  %10.4f  %10.4f  %10.4f  -1.0  ", ASurface.m_pFoilA->m_TEXHinge/100.0, ASurface.m_HingeVector.x, ASurface.m_HingeVector.y, ASurface.m_HingeVector.z);
			strong +=str + "| name, gain,  Xhinge,  XYZhvec,  SgnDup\n";
			pXFile->WriteString(strong);

			//write the same flap at the other end
			pXFile->WriteString("\n#______________\nSECTION                                                     |  (keyword)\n");
			
			strong.Format("%9.4f %9.4f %9.4f %9.4f %7.3f  %3d  %3d   | Xle Yle Zle   Chord Ainc   [ Nspan Sspace ]\n",
				ASurface.m_LB.x          *pFrame->m_mtoUnit,
				ASurface.m_LB.y          *pFrame->m_mtoUnit,
				ASurface.m_LB.z          *pFrame->m_mtoUnit,
				ASurface.GetChord(1.0)   *pFrame->m_mtoUnit,
				m_Surface[j].m_TwistB,
				ASurface.m_NYPanels,
				ASurface.m_YDistType);
			pXFile->WriteString(strong);
			pXFile->WriteString("\n");
			pXFile->WriteString("AFIL 0.0 1.0\n");
			pXFile->WriteString(ASurface.m_pFoilB->m_FoilName +".dat\n");
			pXFile->WriteString("\n");

			pXFile->WriteString("CONTROL                                                     |  (keyword)\n");
			str.Format("_Flap_%d  ",iFlap);
			strong = m_WingName;
			strong.Replace(" ", "_");
			strong += str;
			str.Format("%5.2f  ", 2.0/(ASurface.m_pFoilA->m_TEFlapAngle + ASurface.m_pFoilB->m_TEFlapAngle));
			strong += str;
			str.Format("%5.3f  %10.4f  %10.4f  %10.4f  -1.0  ", ASurface.m_pFoilB->m_TEXHinge/100.0, ASurface.m_HingeVector.x, ASurface.m_HingeVector.y, ASurface.m_HingeVector.z);
			strong +=str + "| name, gain,  Xhinge,  XYZhvec,  SgnDup\n\n";
			pXFile->WriteString(strong);

			iFlap++;
		}
	}
	//write last panel, if no flap before
	if(!ASurface.m_bTEFlap)
	{
		pXFile->WriteString("#______________\nSECTION                                                     |  (keyword)\n");
		strong.Format("%9.4f %9.4f %9.4f %9.4f %7.3f  %3d  %3d   | Xle Yle Zle   Chord Ainc   [ Nspan Sspace ]\n",
			ASurface.m_LB.x          *pFrame->m_mtoUnit,
			ASurface.m_LB.y          *pFrame->m_mtoUnit,
			ASurface.m_LB.z          *pFrame->m_mtoUnit,
			ASurface.GetChord(1.0)   *pFrame->m_mtoUnit,
			m_Surface[j-1].m_TwistB,
			ASurface.m_NYPanels,
			ASurface.m_YDistType);
		pXFile->WriteString(strong);
		pXFile->WriteString("\n");
		pXFile->WriteString("AFIL 0.0 1.0\n");
		pXFile->WriteString(ASurface.m_pFoilB->m_FoilName +".dat\n");
		pXFile->WriteString("\n");

	}

	pXFile->WriteString("\n\n");

	return true;
}

 
double CWing::AlphaInduced(int k)
{
	// Calculates the induced angle from the lift coefficient and
	// from the Beta factor in LLT

	double ai = 0.0;

	for (int m=1; m<m_NStation; m++){
		ai += Beta(m,k) * m_Cl[m] * m_Chord[m]/m_Span;
	}
	return ai;
}


double CWing::Beta(int m, int k)
{
	//Auxiliary calculation of the Beta factor in LLT
	double b;
	double fk = (double)k;
	double fm = (double)m;
	double fr = (double)m_NStation;

	if (m==k) b = 180.0*fr/8.0/pi/sin(fk*pi/fr);
	else if (IsEven(m+k)) b=0.0;
	else {
		double c1 = 180.0/4.0/pi/fr/sin(fk*pi/fr);
		double c2 =   1.0/(1.0-cos((fk+fm)*pi/fr)) 
					- 1.0/(1.0-cos((fk-fm)*pi/fr));
		b = c1 * c2;
	}
	return b;
}


double CWing::Eta(int m)
{
	//Auxiliary calculation of the Eta factor in LLT

	return pi/2.0/(double)m_NStation * sin((double)m*pi/(double)m_NStation) ;
}


double CWing::Sigma(int m)
{
	//Auxiliary calculation of the sigma factor in LLT

	return pi/8.0/(double)m_NStation * sin(2.*(double)m*pi/(double)m_NStation) ;
}


void CWing::LLTInitCl()
{
	//Initializes the Reynolds number and lift coefficient for the initial iteration in LLT

	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	CMiarex *pMiarex = (CMiarex*)s_pMiarex;
	CFoil *pFoil0 = NULL;
	CFoil *pFoil1 = NULL;
	double yob, tau;
	int k;

	bool bOutRe, bError;

	for (k=1; k<s_NLLTStations; k++)
	{
		yob   = cos(k*pi/s_NLLTStations);
		GetFoils(&pFoil0, &pFoil1, yob*m_Span/2.0, tau);
		m_Re[k] = m_Chord[k] * m_QInf /m_Viscosity;
		m_Cl[k] = pMiarex->GetCl(pFoil0, pFoil1, m_Re[k], m_Alpha + m_Ai[k] + m_Twist[k], tau, bOutRe, bError);
	}
	if(m_Type == 2)
	{
		double Lift=0.0;// required for Type 2
		for (k=1; k<s_NLLTStations; k++)
		{
			Lift += Eta(k) * m_Cl[k] * m_Chord[k] /m_Span;
		}
		if(Lift<=0.0) return;
		m_QInf  = m_QInf0 / sqrt(Lift);
		for (k=1; k<s_NLLTStations; k++)
		{
			m_Re[k] = m_Chord[k] * m_QInf /m_Viscosity;
		}
	}
}


void CWing::ScaleSpan(double NewSpan)
{
	// Scales the wing span-wise to the NewSpan value
	for (int i=0; i<=MAXPANELS; i++){
		m_TPos[i]      *= NewSpan/m_Span;
		m_TLength[i]   *= NewSpan/m_Span;
	}
	ComputeGeometry();

}

void CWing::ScaleChord(double NewChord)
{
	// Scales the wing chord-wise so that the root chord reaches the NewChord value

	double ratio = NewChord/m_TChord[0];
	for (int i=0; i<=MAXPANELS; i++){
		m_TChord[i]    *= ratio;
		m_TOffset[i]   *= ratio;
	}
	ComputeGeometry();
}

 
double CWing::GetAverageSweep()
{
	double xroot = m_TChord[0]/4.0;
	double xtip  = m_TOffset[m_NPanel] + m_TChord[m_NPanel]/4.0;
//	double sweep = (atan2(xtip-xroot, m_Span/2.0)) * 180.0/pi;
	return (atan2(xtip-xroot, m_Span/2.0)) * 180.0/pi;
}

void CWing::SetSweep(double Sweep)
{
//	double CurrentSweep = GetAverageSweep();
	double OldTipOffset = m_TOffset[m_NPanel];
	double NewTipOffset = m_TChord[0]/4.0 
						 + tan(Sweep*pi/180.0)*m_Span/2.0
						 - m_TChord[m_NPanel]/4.0;
	if(abs(OldTipOffset)>0.00001f){
		//scale each panel's offset
		double ratio = NewTipOffset/OldTipOffset;
		for(int i=1; i<=m_NPanel; i++){
			m_TOffset[i] *= ratio;
		}
	}
	else{
		//set each panel's offset
		for(int i=1; i<=m_NPanel; i++){
			m_TOffset[i] = NewTipOffset*m_TPos[i]/(m_Span/2.0);
		}
	}
	ComputeGeometry();
}

void CWing::SetTwist(double Twist)
{
	if(abs(m_TTwist[m_NPanel])>0.0001) 
	{
		//scale each panel's twist
		double ratio = Twist/m_TTwist[m_NPanel];

		for(int i=1; i<=m_NPanel; i++){
			m_TTwist[i] *= ratio;
		}
	}
	else{
		//Set each panel's twist in the ratio of the span position
		for(int i=1; i<=m_NPanel; i++){
			m_TTwist[i] = Twist*m_TPos[i]/(m_Span/2.0);
		}
	}
	ComputeGeometry();
}


void CWing::GetFoils(CFoil **pFoil0, CFoil **pFoil1, double y, double &t)
{
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	
	if  (y>0.0) 
	{
		//search Right wing
		for (int i=0; i<m_NPanel; i++){
			if (m_TPos[i]<=y && y<=m_TPos[i+1]){

				*pFoil0 = pFrame->GetFoil(m_RFoil[i]);
				*pFoil1 = pFrame->GetFoil(m_RFoil[i+1]);
				t = (y-m_TPos[i])/(m_TPos[i+1] - m_TPos[i]);
				return;
			}
		}
	}
	else
	{
		//search left wing
		y = -y;
		for (int i=0; i<m_NPanel; i++){
			if (m_TPos[i]<=y && y<m_TPos[i+1]){
				*pFoil0 = pFrame->GetFoil(m_LFoil[i]);
				*pFoil1 = pFrame->GetFoil(m_LFoil[i+1]);
				t = (y-m_TPos[i])/(m_TPos[i+1] - m_TPos[i]);
				return;
			}
		}	
	}
	t = 0;
	pFoil0 = NULL;// use linear
	pFoil1 = NULL;// use linear
}


int CWing::LLTIterate()
{
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	CMiarex *pMiarex = (CMiarex*)s_pMiarex;
	int k ;
	CFoil* pFoil0  = NULL;
	CFoil* pFoil1  = NULL;
	double a, yob, tau, anext;
	bool bOutRe, bError;

	m_Maxa = 0.0;

	for (k=1; k<s_NLLTStations; k++)
	{
		a        = m_Ai[k];
		anext    = -AlphaInduced(k);
		m_Ai[k]  = a +(anext-a)/s_RelaxMax;
		m_Maxa   = __max(m_Maxa, abs(a-anext));
	}

	double Lift=0.0;// required for Type 2
	for (k=1; k<s_NLLTStations; k++)
	{
		yob     = cos(k*pi/s_NLLTStations);
		GetFoils(&pFoil0, &pFoil1, yob*m_Span/2.0, tau);
		m_Cl[k] = pMiarex->GetCl(pFoil0, pFoil1, m_Re[k], 
								 m_Alpha + m_Ai[k]+ m_Twist[k], tau, bOutRe, bError);
		if (m_Type == 2)
		{
			Lift += Eta(k) * m_Cl[k] * m_Chord[k];
		}
	}

	if(m_Type == 2)
	{
		Lift *= m_AR /m_Span;
		if(Lift<=0.0) 
		{
			return -1;
		}
		m_QInf  = m_QInf0 / sqrt(Lift);
		
		for (k=1; k<s_NLLTStations; k++)
		{
			m_Re[k] = m_Chord[k] * m_QInf /m_Viscosity;
			yob     = cos(k*pi/s_NLLTStations);
			GetFoils(&pFoil0, &pFoil1, yob*m_Span/2.0, tau);
			m_Cl[k] = pMiarex->GetCl(pFoil0, pFoil1, m_Re[k], m_Alpha + m_Ai[k]+ m_Twist[k], tau, bOutRe, bError);
		}
	}

	if (m_Maxa<s_CvPrec) 
	{
		m_bConverged = true;
		return 1;
	}

	return 0;
}

double CWing::GetTwist(double y)
{
	int l;
	double tau;
	// calculate twist at each station
	if (y>=0.0)
	{
		//right wing
		for (l=0; l<=m_NPanel; l++)
		{
			if(m_TPos[l] <= y && y <=m_TPos[l+1])
			{
				tau = (y-m_TPos[l])/(m_TPos[l+1]-m_TPos[l]);
				return m_TTwist[l]+(m_TTwist[l+1]-m_TTwist[l]) * tau;
			}
		}
	}
	else
	{
		//left wing
		y=abs(y);
		for (l=0; l<m_NPanel; l++)
		{
			if(m_TPos[l] <= y && y <=m_TPos[l+1])
			{
				tau = (y-m_TPos[l])/(m_TPos[l+1]-m_TPos[l]);
				return m_TTwist[l]+(m_TTwist[l+1]-m_TTwist[l]) * tau;
			}
		}
	}

	return -1.0;
}

double CWing::GetOffset(double yob)
{
	double tau, y;
	double Offset = 0.0;

	y= abs(yob*m_Span/2.0);
	for(int i=0; i<m_NPanel; i++){
		if(m_TPos[i]<= y && y <=m_TPos[i+1]){
			tau = (y - m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			Offset = m_TOffset[i] + tau * (m_TOffset[i+1] - m_TOffset[i]);
			return Offset;
		}
	}
	
	return -1.0;
}

double CWing::Getyrel(double SpanPos)
{
	double y = abs(SpanPos);
	for(int i=0; i<m_NPanel; i++)
	{
		if(m_TPos[i]<=y && y <m_TPos[i+1])
		{
			if(SpanPos>0) return  (y-m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			else          return  (y-m_TPos[i+1])/(m_TPos[i]-m_TPos[i+1]);
		}
	}
	return 1.0;
}

double CWing::GetZPos(double y)
{
	double tau;
	double ZPos =0.0;

	y= abs(y);
	if(y<=0.0) return 0.0;
	for (int i=0; i<m_NPanel; i++)
	{
		if(m_TPos[i]< y && y<=m_TPos[i+1])
		{
			for (int k=0; k<i; k++)
			{
				ZPos+=m_TLength[k+1] * sin(m_TDihedral[k]*pi/180.0);
			}
			tau = (y - m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			ZPos += tau * m_TLength[i+1] * sin(m_TDihedral[i]*pi/180.0);
			return ZPos;
		}
	}
	return 0.0;
}

void CWing::GetViewYZPos(double xrel, double y, double &yv, double &zv, int pos)
{
	double tau;
	double twist, chord;
	zv = 0.0;
	yv = 0.0;
	double fy = abs(y);
	double sign;
	if(fy<1.0e-10) sign = 1.0;
	else sign = y/fy;

//	if(fy<=0.0) return 0.0;
	for (int i=0; i<m_NPanel; i++)
	{
		if(m_TPos[i]< fy && fy<=m_TPos[i+1])
		{
			for (int k=0; k<i; k++)
			{
				yv += m_TLength[k+1] * cos(m_TDihedral[k]*pi/180.0);
				zv += m_TLength[k+1] * sin(m_TDihedral[k]*pi/180.0);
			}
			tau = (fy - m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			yv += tau * m_TLength[i+1] * cos(m_TDihedral[i]*pi/180.0);
			zv += tau * m_TLength[i+1] * sin(m_TDihedral[i]*pi/180.0);

			yv *= sign;
			//	add washout calculated about chord quarter line :
			twist = GetTwist(fy)*pi/180.;
			chord = GetChord(fy*2./m_Span);
			zv -= (xrel-0.25)*chord*sin(twist);

			CFoil *pFoil0 = NULL;
			CFoil *pFoil1 = NULL;
			GetFoils(&pFoil0, &pFoil1, y,  tau);

			if(!pFoil0 || !pFoil1) return;

			if(pos==1)
			{
				double z0 = pFoil0->GetUpperY(xrel) * chord;
				double z1 = pFoil1->GetUpperY(xrel) * chord;
				zv += z0 + (z1-z0)*tau;
			}
			else if(pos==-1)
			{
				double z0 = pFoil0->GetLowerY(xrel) * chord;
				double z1 = pFoil1->GetLowerY(xrel) * chord;
				zv += z0 + (z1-z0)*tau;
			}
		}
	}
}


double CWing::GetC4(double yob, double xRef)
{	
	//returns the quarter-chord point xposition relative to XCmRef
	double Chord, Offset, tau;
	double C4 = 0.0;
	double y = abs(yob*m_Span/2.0);
	for(int i=0; i<m_NPanel; i++){
		if(m_TPos[i]<= y && y <=m_TPos[i+1]){
			tau = (y - m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			Chord  = m_TChord[i]  + tau * (m_TChord[i+1] - m_TChord[i]);
			Offset = m_TOffset[i] + tau * (m_TOffset[i+1] - m_TOffset[i]);
			C4 = Offset + Chord/4.0 - xRef;
			return C4;
		}
	}
	
	return C4;
}

double CWing::GetChord(double yob)
{
	double Chord = 0.0;
	double tau;
	double y;

	y= abs(yob*m_Span/2.0);//geometry is symetric
	for(int i=0; i<m_NPanel; i++){
		if(m_TPos[i]<=y && y <=m_TPos[i+1]){
			tau = (y - m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			Chord = m_TChord[i] + tau * (m_TChord[i+1] - m_TChord[i]);
			return Chord;
		}
	}
	return -1.0;
}


double CWing::GetDihedral(double yob)
{
//	double Dih = 0.0;
	double y;

	y= abs(yob*m_Span/2.0);//geometry is symetric
	for(int i=0; i<m_NPanel; i++){
		if(m_TPos[i]<= y && y <=m_TPos[i+1]){
			if(yob>=0) return m_TDihedral[i];
			else  return -m_TDihedral[i];
		}
	}
	return 0.0;
}



void CWing::LLTComputeWing()
{
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	CMiarex *pMiarex = (CMiarex*)s_pMiarex;
	CLLTDlg *pLLTDlg = (CLLTDlg*)s_pLLTDlg;
	CWPolar* pWPolar = pMiarex->m_pCurWPolar;

	CFoil* pFoil0 = NULL;
	CFoil* pFoil1 = NULL;
	CString strong, string;
	int m;

	double yob, tau, c4, arad, zpos;
	double Integral0           = 0.0;
	double Integral1           = 0.0;
	double Integral2           = 0.0;
	double InducedDrag         = 0.0;
	double ViscousDrag         = 0.0;
	double InducedYawingMoment = 0.0;
	double ViscousYawingMoment = 0.0;
	double PitchingMoment      = 0.0;
	double PCm                 = 0.0;
	double GCm                 = 0.0;
	double eta, sigma;
	double Cm0;
	bool bOutRe, bError;
	bool bPointOutRe, bPointOutAlpha;
	m_bWingOut = false;

	for (m=1; m<s_NLLTStations; m++)
	{
		bPointOutRe    = false;
		bPointOutAlpha = false;
		yob   = cos((double)m*pi/(double)s_NLLTStations);
		GetFoils(&pFoil0, &pFoil1, yob*m_Span/2.0, tau);

		m_Cl[m]     = pMiarex->GetCl(pFoil0, pFoil1, m_Re[m],
									m_Alpha+m_Ai[m]+m_Twist[m], tau, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) 
			bPointOutAlpha = true;
		
		m_PCd[m]    = pMiarex->GetCd(pFoil0, pFoil1, m_Re[m], m_Alpha+m_Ai[m]+m_Twist[m], tau, m_AR, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		m_ICd[m]    = -m_Cl[m] * (m_Ai[m]* pi/180.0);

		m_XTrTop[m] = pMiarex->GetXTr(pFoil0, pFoil1, m_Re[m],
									m_Alpha+m_Ai[m] + m_Twist[m], tau, true, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;
		
		m_XTrBot[m] = pMiarex->GetXTr(pFoil0, pFoil1, m_Re[m],m_Alpha+m_Ai[m]+m_Twist[m], tau, false, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;
		
		m_CmAirf[m] = pMiarex->GetCm(pFoil0, pFoil1, m_Re[m], m_Alpha+m_Ai[m]+m_Twist[m], tau, bOutRe, bError);
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		m_XCPSpanRel[m]  = pMiarex->GetXCp(pFoil0, pFoil1, m_Re[m], m_Alpha+m_Ai[m]+m_Twist[m], tau, m_AR, bOutRe, bError);

		if(abs(m_XCPSpanRel[m])<0.000001)
		{
			//plr mesh was generated prior to v3.15, i.e., without XCp calculations
			Cm0 = pMiarex->GetCm0(pFoil0, pFoil1, m_Re[m],tau, bOutRe, bError);
			if(m_Cl[m]!=0.0) m_XCPSpanRel[m] = 0.25 - Cm0/m_Cl[m];
			else             m_XCPSpanRel[m] = 0.25;
//			bError = false;
		}
		if(bOutRe) bPointOutRe = true;
		if(bError) bPointOutAlpha = true;

		arad = (m_Alpha+m_Ai[m]+m_Twist[m])*pi/180.0;
//		arad = (m_Alpha-m_Ai[m])*pi/180.0;
		c4   = GetC4(yob, pWPolar->m_XCmRef)/m_Chord[m];
		zpos = GetZPos(yob*m_Span/2.0)/m_Chord[m];

		m_CmXRef[m] = - c4  * (m_Cl[m]*cos(arad) + m_PCd[m]*sin(arad)) - zpos* (m_Cl[m]*sin(arad) - m_PCd[m]*cos(arad));
		m_Cm[m]     = m_CmAirf[m]+m_CmXRef[m];

		eta = Eta(m);
		sigma = Sigma(m);
		Integral0           += eta   * m_Cl[m]  * m_Chord[m];
		Integral1           += sigma * m_Cl[m]  * m_Chord[m];
		Integral2           += eta   * m_Cl[m]  * m_Chord[m] * (m_Offset[m]+m_XCPSpanRel[m]*m_Chord[m]);
		InducedDrag         += eta   * m_Cl[m]  * m_Chord[m] * (-m_Ai[m]);
		ViscousDrag         += eta   * m_PCd[m] * m_Chord[m];
		InducedYawingMoment += sigma * m_Cl[m]  * m_Chord[m] * (-m_Ai[m]);
		ViscousYawingMoment += sigma * m_PCd[m] * m_Chord[m];
		PitchingMoment      += eta   * m_Cm[m]     * m_Chord[m] * m_Chord[m];
		PCm                 += eta   * m_CmAirf[m] * m_Chord[m] * m_Chord[m];
		GCm                 += eta   * m_CmXRef[m] * m_Chord[m] * m_Chord[m];

		if(bPointOutAlpha)
		{
			GetLengthUnit(string, pFrame->m_LengthUnit);
			strong.Format("       Span pos = %9.2f ",	cos(m*pi/s_NLLTStations)*m_Span/2.0*pFrame->m_mtoUnit);
			strong += string;
			strong += ",  Re = ";
			ReynoldsFormat(string, m_Re[m]);
			strong += string;

			string.Format(" ,  A+Ai+Twist = %6.1f could not be interpolated\r\n", m_Alpha+m_Ai[m] + m_Twist[m]);
			strong+=string;

			pLLTDlg->AddString(strong);
			m_bWingOut = true;
			m_bConverged = false;
		}
		else if(bPointOutRe)
		{
			GetLengthUnit(string, pFrame->m_LengthUnit);
			strong.Format("       Span pos = %9.2f ",	cos(m*pi/s_NLLTStations)*m_Span/2.0*pFrame->m_mtoUnit);
			strong += string;
			strong += ",  Re = ";
			ReynoldsFormat(string, m_Re[m]);
			strong += string;

			string.Format(" ,  A+Ai+Twist = %6.1f is outside the flight envelope\r\n", m_Alpha+m_Ai[m] + m_Twist[m]);
			strong+=string;

			pLLTDlg->AddString(strong);
			m_bWingOut = true;
		}
	}

	m_CL            =  Integral0   * m_AR /m_Span;
	m_InducedDrag   =  InducedDrag * m_AR /m_Span  * pi / 180.0;
	m_ViscousDrag   =  ViscousDrag / m_GChord;

	m_VYm = ViscousYawingMoment /m_GChord;
	m_IYm = InducedYawingMoment /m_Span * pi * m_AR /180.0;
	m_GYm = m_VYm + m_IYm;
	m_GCm = PitchingMoment / m_GChord / m_MAChord;
	m_GRm = -Integral1   * m_AR /m_Span;
//	m_GCm = GCm / m_GChord / m_MAChord;

	if(m_CL !=0.0)	m_XCP = Integral2 * m_AR /m_Span/m_CL;
	else            m_XCP = 0.0;
	if(m_bSymetric) m_YCP = 0.0;
	else            m_YCP = m_AR/m_CL * Integral1; 

	//bending moment

	int j,jj;

	double yj, yjm, yjp;
	double dy;

	for (j=0; j<=s_NLLTStations; j++)		m_SpanPos[j] = m_Span/2.0 * cos(j*pi/s_NLLTStations);

	for (j=1; j<s_NLLTStations; j++)
	{
		yjp = m_SpanPos[j-1];
		yjm = m_SpanPos[j+1];
		yj  = m_SpanPos[j];

		dy = (yjp-yj)/2.0 + (yj-yjm)/2.0;

		m_StripArea[j] = m_Chord[j]*dy;//m²
	}

	//dynamic pressure, kg/m3
	double q = 0.5*pWPolar->m_Density * m_QInf * m_QInf;

	double bm;
	double y, yy;

	for (j=1; j<s_NLLTStations; j++)
	{
		y = m_SpanPos[j];
		bm = 0.0;
		if (y>=0)
		{
			for (jj=0; jj<j; jj++)
			{
				yy =  m_SpanPos[jj];
				bm += (yy-y) * m_Cl[jj] * m_StripArea[jj];
			}
		}
		else
		{
			for (jj=j+1; jj<s_NLLTStations; jj++)
			{
				yy =  m_SpanPos[jj];
				bm += (y-yy) * m_Cl[jj] * m_StripArea[jj];
			}
		}
		m_BendingMoment[j] = bm*q;
	}
}

bool CWing::LLTSetLinearSolution()
{
	CMiarex *pMiarex = (CMiarex*)s_pMiarex;
	double aij[MAXSTATIONS*MAXSTATIONS];// coefficient matrix
	double rhs[MAXSTATIONS+1];//right hand side

	memset(aij, 0, sizeof(aij));
	memset(rhs, 0, sizeof(rhs));

	CFoil *pFoil0, *pFoil1;
	int i,j,p;
	double fr  = s_NLLTStations;
	double fj, t0, st0, snt0, c, a0, slope, tau, yob;
	double cs = m_TChord[0];

	for (i=1; i<s_NLLTStations; i++)
	{
		c   = m_Chord[i];
		t0  = i * pi/fr;
		st0 = sin(t0);
		for (j=1; j<s_NLLTStations; j++)
		{
			fj = double(j);
			snt0 = sin(fj*t0);
			p = (i-1)*(s_NLLTStations-1)+j-1;
			aij[p]  = snt0 + c*pi/m_Span/2.0* fj*snt0/st0;
		}
		yob   = cos(i*pi/s_NLLTStations);
		GetFoils(&pFoil0, &pFoil1, yob*m_Span/2.0, tau);
		a0 = pMiarex->GetZeroLiftAngle(pFoil0, pFoil1, m_Re[i], tau);
		rhs[i] = c/cs * (m_Alpha-a0+m_Twist[i])/180.0*pi;
	}

	if(Gauss(aij,s_NLLTStations-1, rhs+1,0))
	{
		for (i=1; i<s_NLLTStations; i++){
			t0  = i * pi/fr;
			m_Cl[i] = 0.0;
			for (j=1; j<s_NLLTStations; j++)
			{
				fj = double(j);
				snt0 = sin(fj*t0);
				m_Cl[i] += rhs[j]* snt0;
			}
			yob   = cos(i*pi/s_NLLTStations);
			GetFoils(&pFoil0, &pFoil1, yob*m_Span/2.0, tau);
			pMiarex->GetLinearizedPolar(pFoil0, pFoil1, m_Re[i], tau, a0, slope);
			a0 = pMiarex->GetZeroLiftAngle(pFoil0, pFoil1, m_Re[i], tau);//better apprixmation ?
			m_Cl[i] *= slope*180.0/pi*cs/m_Chord[i];
			m_Ai[i]  = -(m_Alpha-a0+m_Twist[i]) + m_Cl[i]/slope;
		}
		return true;
	}
	return false;
}


bool CWing::Gauss(double *A, int n, double *B, int m)
{
////////////////////////////////////////////////////////////////////////////////
//  int Gaussian_Elimination(double *A, int n, double *B)                     //
//                                                                            //
//     Solve the linear system of equations AX=B where A is an n x n matrix   //
//     B is an n-dimensional column vector (n x 1 matrix) for the             //
//     n-dimensional column vector (n x 1 matrix) X.                          //
//                                                                            //
//     This routine performs partial pivoting and the elements of A are       //
//     modified during computation.  The result X is returned in B.           //
//     If the matrix A is singular, the return value of the function call is  //
//     false. If the solution was found, the function return value is true    //
//                                                                            //
//  Arguments:                                                                //
//     double *A      On input, the pointer to the first element of the       //
//                    matrix A[n][n].  On output, the matrix A is destroyed.  //
//     int     n      The number of rows and columns of the matrix A and the  //
//                    dimension of B.                                         //
//     double *B      On input, the pointer to the first element of the       //
//                    vector B[n].  On output, the vector B is replaced by the//
//                    vector X, the solution of AX = B.                       //
//     int m          The number of right hand side vectors                   //
//                    The default is m=0                                      //
//                                                                            //
//  Return Values:                                                            //
//     true :  Success                                                        //
//     false :  Failure - The matrix A is singular.                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
	int row, i, j, pivot_row, k;
	double max, dum, *pa, *pA, *A_pivot_row;
	// for each variable find pivot row and perform forward substitution
	pa = A;
	for (row = 0; row < (n - 1); row++, pa += n) {
		//  find the pivot row
		A_pivot_row = pa;
		max = abs(*(pa + row));
		pA = pa + n;
		pivot_row = row;
		for (i=row+1; i < n; pA+=n, i++)
			if ((dum = abs(*(pA+row))) > max) { 
				max = dum; 
				A_pivot_row = pA; 
				pivot_row = i; 
			}
		if (max <= 0.0) 
			return false;                // the matrix A is singular
		
			// and if it differs from the current row, interchange the two rows.
			
		if (pivot_row != row) {
			for (i = row; i < n; i++) {
				dum = *(pa + i);
				*(pa + i) = *(A_pivot_row + i);
				*(A_pivot_row + i) = dum;
			}
			for(k=0; k<=m; k++){
				dum = B[row+k*n];
				B[row+k*n] = B[pivot_row+k*n];
				B[pivot_row+k*n] = dum;
			}
		}
		
		// Perform forward substitution
		
		for (i = row+1; i<n; i++) {
			pA = A + i * n;
			dum = - *(pA + row) / *(pa + row);
			*(pA + row) = 0.0;
			for (j=row+1; j<n; j++) *(pA+j) += dum * *(pa + j);
			for (k=0; k<=m; k++) 
				B[i+k*n] += dum * B[row+k*n];
		}
	}
	
	// Perform backward substitution
	
	pa = A + (n - 1) * n;
	for (row = n - 1; row >= 0; pa -= n, row--) {
		if ( *(pa + row) == 0.0 ) 
			return false;           // matrix is singular
		dum = 1.0 / *(pa + row);
		for ( i = row + 1; i < n; i++) *(pa + i) *= dum; 
		for(k=0; k<=m; k++) B[row+k*n] *= dum;
		for ( i = 0, pA = A; i < row; pA += n, i++) {
			dum = *(pA + row);
			for ( j = row + 1; j < n; j++) *(pA + j) -= dum * *(pa + j);
			for(k=0; k<=m; k++) 
				B[i+k*n] -= dum * B[row+k*n];
		}
	}
	return true;
}


bool CWing::LLTInitialize()
{
	CMiarex *pMiarex = (CMiarex*)s_pMiarex;
	double y ;
	int k;
	CString str;
	str.Format("Initializing analysis...\r\n");
	if(m_bTrace) m_pXFile->WriteString(str);

	if(m_Type == 2)	m_QInf0 = sqrt(2.*m_Weight* 9.81 /m_Density/m_Area);
	else m_QInf0 = 0.0;

	m_bConverged = true;
	m_bWingOut = false;

	if(m_bTrace) m_pXFile->WriteString("Initializing chords...\r\n");
	ComputeChords(s_NLLTStations);
	for (k=0; k<=s_NLLTStations; k++)
	{
		y   = cos(k*pi/s_NLLTStations)* m_Span/2.0;
		m_Twist[k] = GetTwist(y);
	}
	
	if(m_bTrace) m_pXFile->WriteString("Initializing Reynolds numbers...\r\n");
	
	for (k=0; k<=s_NLLTStations; k++) 
	{
		m_Re[k] = m_Chord[k] * m_QInf/m_Viscosity;
	}

	return true;
}



int CWing::VLMGetPanelTotal()
{
	CMiarex    *pMiarex = (CMiarex*)   s_pMiarex;
	double MinPanelSize;
	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
	else                            MinPanelSize = m_Span/1000.0;

	int total = 0;
	for (int i=0; i<m_NPanel; i++){
			//do not create a surface if its length is less than the critical size
//			if(abs(m_TPos[j]-m_TPos[j+1])/m_Span >0.001){
			if (abs(m_TPos[i]-m_TPos[i+1]) > MinPanelSize)
				total +=m_NXPanels[i]*m_NYPanels[i];
	}
//	if(!m_bMiddle) total *=2;
	if(!m_bIsFin) return total*2;
	else          return total;
}

void CWing::InsertSection(double TPos, double TChord, double TOffset,
						  double TZPos, double Twist, CString Foil,
						  int NChord, int NSpan, int SSpan)
{
	// Inserts a section before an existing sections
	// If the section already exists, does nothing and returns
	// Otherwise copies the data from section n+1
	// and inserts a mid panel

	int n;
	bool bIsInserted = false;
	
	for (n=0; n<=m_NPanel; n++){
		if(abs(TPos-m_TPos[n])<0.0000001) {

			m_NXPanels[n]  = NChord;
			m_NYPanels[n]  = NSpan;
			m_XPanelDist[n] = 1;
			m_YPanelDist[n] = SSpan;
			bIsInserted  = true;
			break;
		}
		if(TPos < m_TPos[n]){
			for (int m=m_NPanel;m>=n; m--){
				m_TPos[m+1]      = m_TPos[m];
				m_TChord[m+1]    = m_TChord[m];
				m_TOffset[m+1]   = m_TOffset[m];
				m_TZPos[m+1]     = m_TZPos[m];
				m_TDihedral[m+1] = m_TDihedral[m];
				m_TTwist[m+1]    = m_TTwist[m];
				m_RFoil[m+1]     = m_RFoil[m];
				m_LFoil[m+1]     = m_LFoil[m];
				m_NXPanels[m+1]  = m_NXPanels[m];
				m_NYPanels[m+1]  = m_NYPanels[m];
				m_XPanelDist[m+1] = m_XPanelDist[m];
				m_YPanelDist[m+1] = m_YPanelDist[m];
			}
			m_TPos[n]      = TPos;
			m_TChord[n]    = TChord;
			m_TOffset[n]   = TOffset;
			m_TZPos[n]     = TZPos;
			m_TDihedral[n] = 180.0/pi *
							 atan2(m_TZPos[n+1]-m_TZPos[n], m_TPos[n+1]-m_TPos[n]);
			m_TTwist[n]    = Twist;
			m_RFoil[n]     = Foil;
			m_LFoil[n]     = Foil;
			m_NXPanels[n]  = NChord;
			m_NYPanels[n]  = NSpan;
			m_YPanelDist[n] = SSpan;
			bIsInserted  = true;
			m_NPanel++;
			break;
		}
	}
	if(!bIsInserted)
	{
		if(TPos > m_TPos[m_NPanel])
		{
			n = m_NPanel +1;
			m_TPos[n]    = TPos;
			m_TChord[n]  = TChord;
			m_TOffset[n] = TOffset;
			m_TZPos[n]   = TZPos;
			m_TDihedral[n-1] = 180.0/pi *  atan2(m_TZPos[n]-m_TZPos[n-1], m_TPos[n]-m_TPos[n-1]);
			m_TTwist[n]  = Twist;
			m_RFoil[n]   = Foil;
			m_LFoil[n]   = Foil;
			m_NXPanels[n]  = NChord;
			m_NYPanels[n]  = NSpan;
			m_YPanelDist[n] = SSpan;
			m_NPanel++;
		}
		else 
		{//no need to overwrite
		}
	}
}


bool CWing::VLMSetAutoMesh(int total)
{
	//set automatic mesh : keep it simple
	m_bVLMAutoMesh = true;
	m_bChanged = true;
	//split (NYTotal) panels on each side proportionnaly to length, and space evenly
	//Set VLMMATSIZE/NYTotal panels along chord
	int NYTotal, size;
	
	if(!total)
	{
		size = (int)(VLMMATSIZE/4);//why not ? Too much refinement isn't worthwile
		NYTotal = 22;
	}
	else{
		size = total;
		NYTotal = (int)sqrt((float)size);
	}

	NYTotal *= 2;

	double d1, d2; //spanwise panel densities at i and i+1

	for (int i=0; i<m_NPanel;i++)
	{
		d1 = 5./2./m_Span/m_Span/m_Span *8. * pow(m_TPos[i],  3) + 0.5;
		d2 = 5./2./m_Span/m_Span/m_Span *8. * pow(m_TPos[i+1],3) + 0.5;
		m_NYPanels[i] = (int) (NYTotal * (0.8*d1+0.2*d2)* (m_TPos[i+1]-m_TPos[i])/m_Span);
		m_NXPanels[i] = (int) (size/NYTotal);
		m_NXPanels[i] = min(m_NXPanels[i], MAXCHORDPANELS);

		if(m_NYPanels[i]==0) m_NYPanels[i] = 1;
		if(m_NXPanels[i]==0) m_NXPanels[i] = 1;
	}

	return true;
}


bool CWing::CreateSurfaces(CVector const &T, double XTilt, double YTilt)
{
	//generic surface, LLT, VLM or Panel
	int j, nSurf;
	CFoil *pFoilA, *pFoilB;
	CVector PLA, PTA, PLB, PTB, Offset, T1;
	CVector Trans(T.x, 0.0, T.z);
	CVector O(0.0,0.0,0.0);
	CVector VNormal[MAXPANELS+1], VNSide[MAXPANELS+1], VN, VNP1;
	double MinPanelSize;

	CMainFrame *pFrame  = (CMainFrame*)s_pFrame;
	CMiarex    *pMiarex = (CMiarex*)   s_pMiarex;

	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
	else                            MinPanelSize = m_Span;

	//first generate the surfaces - from left tip to right tip
	// so start with left wing, in reverse order
	m_NSurfaces = 0;
	m_MatSize = 0;

	VNormal[0].Set(0.0, 0.0, 1.0);
	VNSide[0].Set(0.0, 0.0, 1.0);

	nSurf=0;
	for(j=0; j<m_NPanel;j++)
	{
		if (abs(m_TPos[j]-m_TPos[j+1]) > MinPanelSize)
		{
			VNormal[nSurf].Set(0.0, 0.0, 1.0);
			VNormal[nSurf].RotateX(O, m_TDihedral[j]);
			nSurf++;
		}
	}

	for(j=0; j<nSurf;j++)
	{
		VNSide[j+1] = VNormal[j]+VNormal[j+1];
		VNSide[j+1].Normalize();
	}

	for (j=m_NPanel-1; j>=0; j--)
	{
		//do not create a surface if its length is less than the critical size
		if (abs(m_TPos[j]-m_TPos[j+1]) > MinPanelSize)
		{
			m_Surface[m_NSurfaces].m_pFoilA   = pFrame->GetFoil(m_LFoil[j+1]);
			m_Surface[m_NSurfaces].m_pFoilB   = pFrame->GetFoil(m_LFoil[j]);

			m_Surface[m_NSurfaces].m_Dihedral = -m_TDihedral[j];
			m_Surface[m_NSurfaces].m_Length   =  m_TPos[j+1] - m_TPos[j];

			PLB.x =  m_TOffset[j];		PLA.x =  m_TOffset[j+1];
			PLB.y = -m_TPos[j];			PLA.y = -m_TPos[j+1];
			PLB.z =  0.0;				PLA.z =  0.0;

			PTB.x = PLB.x+m_TChord[j];	PTA.x = PLA.x+m_TChord[j+1];
			PTB.y = PLB.y;				PTA.y = PLA.y;
			PTB.z =  0.0;				PTA.z =  0.0;

			m_Surface[m_NSurfaces].m_TwistA   =  m_TTwist[j+1];
			m_Surface[m_NSurfaces].m_TwistB   =  m_TTwist[j];

			m_Surface[m_NSurfaces].m_LA.Copy(PLA);
			m_Surface[m_NSurfaces].m_LB.Copy(PLB);
			m_Surface[m_NSurfaces].m_TA.Copy(PTA);
			m_Surface[m_NSurfaces].m_TB.Copy(PTB);

			m_Surface[m_NSurfaces].SetNormal();
			m_Surface[m_NSurfaces].RotateX(PLB, -m_TDihedral[j]);		

			m_Surface[m_NSurfaces].NormalA.Set(VNSide[nSurf].x,   -VNSide[nSurf].y,   VNSide[nSurf].z);
			m_Surface[m_NSurfaces].NormalB.Set(VNSide[nSurf-1].x, -VNSide[nSurf-1].y, VNSide[nSurf-1].z);
			
			nSurf--;

			m_Surface[m_NSurfaces].SetTwist();

			T1.x = 0.0;
			T1.y = -m_TYProj[j] - PLB.y;
			T1.z = GetZPos(m_TPos[j]);
			m_Surface[m_NSurfaces].Translate(T1);

			m_Surface[m_NSurfaces].m_NXPanels = m_NXPanels[j];
			m_Surface[m_NSurfaces].m_NYPanels = m_NYPanels[j];

			//AVL coding + invert sine and -sine for left wing
			m_Surface[m_NSurfaces].m_XDistType = m_XPanelDist[j];

			if(m_YPanelDist[j] ==2)        m_Surface[m_NSurfaces].m_YDistType = -2;
			else if(m_YPanelDist[j] ==  1) m_Surface[m_NSurfaces].m_YDistType =  1;
			else if(m_YPanelDist[j] == -2) m_Surface[m_NSurfaces].m_YDistType =  2;
			else                           m_Surface[m_NSurfaces].m_YDistType =  0;
			m_MatSize += m_NXPanels[j]*m_NYPanels[j];

			pFoilA = m_Surface[m_NSurfaces].m_pFoilA;
			pFoilB = m_Surface[m_NSurfaces].m_pFoilB;

			CreateXPoints(m_NXPanels[j], m_XPanelDist[j], pFoilA,  pFoilB,
                          m_Surface[m_NSurfaces].m_xPointA, m_Surface[m_NSurfaces].m_xPointB,
                          m_Surface[m_NSurfaces].m_NXLead,  m_Surface[m_NSurfaces].m_NXFlap);
			m_Surface[m_NSurfaces].SetFlap();
			m_Surface[m_NSurfaces].Init();
			m_Surface[m_NSurfaces].m_bIsLeftSurf   = true;
			m_Surface[m_NSurfaces].m_bIsInSymPlane = false;
			m_NSurfaces++;
		}
	}
	m_Surface[m_NSurfaces-1].m_bIsCenterSurf = true;//previous left center surface
	m_Surface[m_NSurfaces].m_bIsCenterSurf   = true;//next right center surface

	//we need a right wwing only in the follwing cases
	// if its an 'ordinary wing'
	// if its a fin, symetrical about the fuselage x-axis
	// if its a symetrical double fin
	if(!m_bIsFin || (m_bIsFin && m_bSymFin) || (m_bIsFin && m_bDoubleFin))
	{
		for (j=0; j<m_NPanel; j++)
		{
			if(abs(m_TPos[j]-m_TPos[j+1]) > MinPanelSize)
			{
				m_Surface[m_NSurfaces].m_Dihedral = m_TDihedral[j];
				m_Surface[m_NSurfaces].m_Length   = m_TPos[j+1] - m_TPos[j];
				m_Surface[m_NSurfaces].m_TwistA   = m_TTwist[j];
				m_Surface[m_NSurfaces].m_TwistB   = m_TTwist[j+1];
				m_Surface[m_NSurfaces].m_pFoilA   = pFrame->GetFoil(m_RFoil[j]);
				m_Surface[m_NSurfaces].m_pFoilB   = pFrame->GetFoil(m_RFoil[j+1]);

				PLA.x =  m_TOffset[j];		PLB.x =  m_TOffset[j+1];
				PLA.y =  m_TPos[j];			PLB.y =  m_TPos[j+1];
				PLA.z =  0.0;				PLB.z =  0.0;

				PTA.x = PLA.x+m_TChord[j];	PTB.x = PLB.x+m_TChord[j+1];
				PTA.y = PLA.y;				PTB.y = PLB.y;
				PTA.z = 0.0;				PTB.z = 0.0;

				m_Surface[m_NSurfaces].m_LA.Copy(PLA);
				m_Surface[m_NSurfaces].m_LB.Copy(PLB);
				m_Surface[m_NSurfaces].m_TA.Copy(PTA);
				m_Surface[m_NSurfaces].m_TB.Copy(PTB);

				//Set surface normals
				m_Surface[m_NSurfaces].SetNormal();
				m_Surface[m_NSurfaces].RotateX(PLA, m_TDihedral[j]);

				m_Surface[m_NSurfaces].NormalA.Set(VNSide[nSurf].x,   VNSide[nSurf].y,   VNSide[nSurf].z);
				m_Surface[m_NSurfaces].NormalB.Set(VNSide[nSurf+1].x, VNSide[nSurf+1].y, VNSide[nSurf+1].z);
				
				nSurf++;

				m_Surface[m_NSurfaces].SetTwist();

				T1.x = 0.0;
				T1.y = +m_TYProj[j] - PLA.y;
				T1.z = GetZPos(m_TPos[j]);
				m_Surface[m_NSurfaces].Translate(T1);

				m_Surface[m_NSurfaces].m_NXPanels   = m_NXPanels[j];
				m_Surface[m_NSurfaces].m_NYPanels   = m_NYPanels[j];
				m_Surface[m_NSurfaces].m_XDistType  = m_XPanelDist[j];
				m_Surface[m_NSurfaces].m_YDistType  = m_YPanelDist[j];//AVL coding

				m_MatSize += m_NXPanels[j]*m_NYPanels[j];

				pFoilA = m_Surface[m_NSurfaces].m_pFoilA;
				pFoilB = m_Surface[m_NSurfaces].m_pFoilB;

				CreateXPoints(m_NXPanels[j], m_XPanelDist[j], pFoilA,  pFoilB,
                                              m_Surface[m_NSurfaces].m_xPointA,  m_Surface[m_NSurfaces].m_xPointB,
                                              m_Surface[m_NSurfaces].m_NXLead,   m_Surface[m_NSurfaces].m_NXFlap);
				m_Surface[m_NSurfaces].SetFlap();
				m_Surface[m_NSurfaces].Init();
				m_Surface[m_NSurfaces].m_bIsRightSurf  = true;
				m_Surface[m_NSurfaces].m_bIsInSymPlane = false;
				m_NSurfaces++;
			}
		}
	}
	CVector Or(0.0,0.0,0.0);
	if(!m_bIsFin || (m_bIsFin && m_bSymFin))
	{
		for (j=0; j<m_NSurfaces; j++)
		{
			m_Surface[j].RotateX(Or, XTilt);
			m_Surface[j].RotateY(Or, YTilt);
			m_Surface[j].Translate(Trans);
		}
	}
	else 
	{
		if(m_bDoubleFin)
		{
			//rotate surfaces symetrically
			int ns2 = (int)(m_NSurfaces/2);
			Offset.Set(0.0, -T.y, 0.0);
			for(j=0; j<ns2; j++)
			{
				m_Surface[j].RotateX(Or, +XTilt);
				m_Surface[j].RotateZ(Or, YTilt);
				m_Surface[j].Translate(Trans);
				m_Surface[j].Translate(Offset);
				m_Surface[j].m_bIsInSymPlane = false;
			}
			Offset.y = -Offset.y;
			for(j=ns2; j< m_NSurfaces; j++)
			{
				m_Surface[j].RotateX(Or, -XTilt);
				m_Surface[j].RotateZ(Or, -YTilt);
				m_Surface[j].Translate(Trans);
				m_Surface[j].Translate(Offset);
				m_Surface[j].m_bIsInSymPlane = false;
			}
			m_Surface[ns2].m_bIsTipRight = true;
			m_Surface[ns2].m_bIsTipLeft  = true;
			m_Surface[0].m_bIsTipRight   = true;
			m_Surface[0].m_bIsTipLeft    = true;

			m_Surface[ns2+1].m_bIsTipRight          = true;
			m_Surface[ns2+1].m_bIsTipLeft           = true;
			m_Surface[m_NSurfaces-1].m_bIsTipRight  = true;
			m_Surface[m_NSurfaces-1].m_bIsTipLeft   = true;
		}
		else
		{
			//Not a double fin, so just a simple single-sided fin
			for (j=0; j<m_NSurfaces; j++)
			{
				m_Surface[j].RotateX(Or, XTilt);
				m_Surface[j].RotateZ(Or, YTilt);
				m_Surface[j].Translate(Trans);
				m_Surface[j].m_bIsLeftSurf   = true;
				m_Surface[j].m_bIsRightSurf  = false;
				m_Surface[j].m_bIsInSymPlane = true;
			}
			m_Surface[0].m_bIsTipLeft  = true;
			m_Surface[j].m_bIsTipRight = false;
		}
	}

	m_Surface[0].m_bIsTipLeft              = true;
	if(m_NSurfaces>=1) m_Surface[m_NSurfaces-1].m_bIsTipRight = true;

	if(m_NSurfaces>1) m_Surface[(int)(m_NSurfaces/2)-1].m_bJoinRight   = true;
	//check for a center gap greater than 1/10mm
	if(m_TPos[0]>0.0001) 	m_Surface[(int)(m_NSurfaces/2)-1].m_bJoinRight   = false;

	if(m_bIsFin && m_bDoubleFin) m_Surface[(int)(m_NSurfaces/2)-1].m_bJoinRight   = false;

	m_bWingOut = false;

	return true;
}


void CWing::VLMComputeWing(double *Gamma, double *Cp,  double &VDrag, double &XCP, double &YCP,
		                   double &GCm,   double &VCm, double &GRm,   double &GYm, double &IYm, double &VYm, 
						   bool bViscous, bool bTilted, int RefAreaType)
{
	//calculates :
	// - the moments and the centre of pressure positions by summation over the apnels
	// - the viscous characteristics by interpolation
	// 
	CMainFrame *pFrame = (CMainFrame*)s_pFrame;
	CMiarex *pMiarex   = (CMiarex*)s_pMiarex;
	CVLMDlg *pVLMDlg   = (CVLMDlg*)s_pVLMDlg;
	CWPolar* pWPolar = pMiarex->m_pCurWPolar;

	int  j, k, l, p, m, nFlap;
	bool bOutRe, bError, bPointOutRe, bPointOutCl;
	double CPStrip, tau, NForce, q, Alpha, cosa, sina;
	CString string, strong;
	CFoil *pFoil0, *pFoil1;
	CVector DragVector, VInf, WindNormal, WindDirection;
	CVector Force, SurfaceNormal, LeverArm, LeverArmC4, PanelLeverArm, PtC4, PtLE, PanelForce, StripForce, Moment0, Moment1;
	CVector TotalMoment, DragMoment, GeomMoment;
	CVector H, HA, HB, V1, HingeLeverArm, HingeMoment;

	//dynamic pressure, kg/m3
	q = 0.5 * m_Density * m_QInf * m_QInf;

	if(!bTilted) Alpha = m_Alpha;
	else         Alpha = 0.0; 

	cosa = cos(Alpha*pi/180.0);
	sina = sin(Alpha*pi/180.0);

	//   Define wind axis
	WindNormal.Set(   -sina, 0.0, cosa);
	WindDirection.Set( cosa, 0.0, sina);

	VInf = WindDirection * m_QInf;

	m_GRm = m_GCm = 0.0;
	m_GYm = m_IYm = m_VCm = m_VYm = 0.0;

	for (m=0; m< m_NStation; m++)	m_Re[m] = m_Chord[m] * m_QInf /m_Viscosity;

	bOutRe = bError = false;
	m = p = nFlap = 0;

	for (j=0; j<m_NSurfaces; j++)
	{
		pFoil0 = m_Surface[j].m_pFoilA;
		pFoil1 = m_Surface[j].m_pFoilB;

		if(pFoil0->m_bTEFlap && pFoil1->m_bTEFlap)
		{
			//create a hinge unit vector and initialize hinge moment
			m_Surface[j].GetPoint(m_Surface[j].m_posATE, m_Surface[j].m_posBTE, 0.0, HA, 0);
			m_Surface[j].GetPoint(m_Surface[j].m_posATE, m_Surface[j].m_posBTE, 1.0, HB, 0);
			H = HB-HA;
			H.Normalize();
			m_FlapMoment[nFlap] = 0.0;
		}

		SurfaceNormal = m_Surface[j].Normal;
		for (k=0; k<m_Surface[j].m_NYPanels; k++)
		{
			bPointOutRe = false;
			bPointOutCl = false;
			StripForce.Set(0.0,0.0,0.0);//sum lift of all panels along chord
			CPStrip        = 0.0;
			m_CmAirf[m]    = 0.0;
			GeomMoment.Set(0.0,0.0,0.0);

			m_Surface[j].GetLeadingPt(k, PtLE);
			m_Surface[j].GetC4(k, PtC4, tau);

			LeverArm   = PtC4;
			LeverArm.x -= pWPolar->m_XCmRef;

			m_StripArea[m] = 0.0;

			for (l=0; l<m_Surface[j].m_NXPanels; l++)
			{
				m_StripArea[m] += m_pPanel[p].Area;
				LeverArmC4  = m_pPanel[p].VortexPos - PtC4;
				PanelLeverArm.x = m_pPanel[p].VortexPos.x - pWPolar->m_XCmRef;
				PanelLeverArm.y = m_pPanel[p].VortexPos.y;
				PanelLeverArm.z = m_pPanel[p].VortexPos.z;

				// for each panel along the chord, add the lift coef
				PanelForce  = VInf * m_pPanel[p].Vortex;
				PanelForce *= Gamma[p] * m_Density;                 //Newtons

				if(!m_bVLM1 && !m_pPanel[p].m_bIsLeading)
				{
					Force       = VInf* m_pPanel[p].Vortex;
					Force      *= Gamma[p+1] * m_Density;       //Newtons
					PanelForce -= Force;
				}
				Moment0 = LeverArmC4 * PanelForce;
				m_CmAirf[m]  += Moment0.y;                          //N.m
				GeomMoment   += PanelLeverArm * PanelForce;         //N.m

				StripForce += PanelForce;
				NForce = PanelForce.dot(SurfaceNormal);
				
				XCP       += m_pPanel[p].VortexPos.x * PanelForce.dot(WindNormal); //global center of pressure (N.m)
				YCP       += m_pPanel[p].VortexPos.y * PanelForce.dot(WindNormal); 
				CPStrip   += m_pPanel[p].VortexPos.x * NForce;

				Cp[p]  = -2.0 * PanelForce.dot(m_pPanel[p].Normal) /m_QInf/m_pPanel[p].Area/m_Density;

//				if(pFoil0->m_bTEFlap && pFoil1->m_bTEFlap)
				if(m_Surface[j].m_bTEFlap)
				{
/*					//add hinge moment contribution
					V1 = m_pPanel[p].VortexPos - HA;
					HingeLeverArm = V1 - H * V1.dot(H);
					if(HingeLeverArm.x>0.0)
					{
						HingeMoment = HingeLeverArm * PanelForce;//N.m
						m_FlapMoment[nFlap] += HingeMoment.dot(H);
					}*/
					if(m_Surface[j].IsFlapPanel(p))
					{
						//then p is on the flap, so add its contribution
						
						HingeLeverArm = m_pPanel[p].VortexPos - m_Surface[j].m_HingePoint;
						HingeMoment = HingeLeverArm * PanelForce;//N.m
						m_FlapMoment[nFlap] += HingeMoment.dot(m_Surface[j].m_HingeVector);
					}
				}
				p++;
			}

			NForce = StripForce.dot(SurfaceNormal);

			m_XCPSpanRel[m]    = (CPStrip/NForce - PtLE.x)/m_Chord[m];
			m_XCPSpanAbs[m]    =  CPStrip/NForce ;


			if(bViscous)
			{
				m_PCd[m]    = pMiarex->GetVar(2, pFoil0, pFoil1, m_Re[m], m_Cl[m], tau, bOutRe, bError);
				bPointOutRe = bOutRe || bPointOutRe;
				if(bError) bPointOutCl = true;

				m_XTrTop[m] = pMiarex->GetVar(5, pFoil0, pFoil1, m_Re[m], m_Cl[m], tau, bOutRe, bError);
				bPointOutRe = bOutRe || bPointOutRe;
				if(bError) bPointOutCl = true;
				
				m_XTrBot[m] = pMiarex->GetVar(6, pFoil0, pFoil1, m_Re[m], m_Cl[m], tau, bOutRe, bError);
				bPointOutRe = bOutRe || bPointOutRe;
				if(bError) bPointOutCl = true;
				m_ViscousDrag  = m_PCd[m] * m_StripArea[m];
				VDrag         += m_PCd[m] * m_StripArea[m];
				DragVector.x   = m_PCd[m] * m_StripArea[m] * q;//Newtons
			}
			else
			{
				m_PCd[m]     = 0.0;
				m_XTrBot[m]  = 1.0;
				m_XTrTop[m]  = 1.0;
				DragVector.x = 0.0;
			}

			//_______________Local Moment coefficients________________
//			m_CmAirf[m]     = Cm4*2.0      /StripArea/m_QInf/100.0;//error up to v2.00, corrected in v2.01
//			m_CmAirf[m]     = Cm4*2.0/chord/StripArea/m_QInf;//up to v2.04

			// ______________Global moments, in N.m __________________
			DragMoment = LeverArm * DragVector;

			m_GCm += GeomMoment.y ;
			m_GRm += GeomMoment.dot(WindDirection);
			m_GYm += GeomMoment.dot(WindNormal);// This is necessarily zero i.a.w. Kutta Jukowski's theorem

			m_VCm += DragMoment.y;
			m_VYm += DragMoment.dot(WindNormal);
			m_IYm += -m_ICd[m] * m_StripArea[m] * PtC4.y / 2.0 * m_Density *m_QInf*m_QInf; 

			m_CmAirf[m]    *= 1.0/m_Chord[m]/m_StripArea[m]/q;
			m_CmXRef[m]     = (GeomMoment.y+DragMoment.y)/m_Chord[m]/m_StripArea[m]/q;
			m_Cm[m]         = m_CmAirf[m]+m_CmXRef[m];


			if(bPointOutCl)
			{
				GetLengthUnit(string, pFrame->m_LengthUnit);
				strong.Format("       Span pos = %9.2f ",	m_SpanPos[m]*pFrame->m_mtoUnit);
				strong += string;
				strong += ",  Re = ";
				ReynoldsFormat(string, m_Re[m]);
				strong += string;

				string.Format(",  Cl = %6.2f could not be interpolated\r\n", m_Cl[m]);
				strong+=string;
				if(m_bTrace) pVLMDlg->AddString(strong);
				m_bWingOut = true;

			}
			else if(bPointOutRe)
			{
				GetLengthUnit(string, pFrame->m_LengthUnit);
				strong.Format("       Span pos = %9.2f ",	m_SpanPos[m]*pFrame->m_mtoUnit);
				strong += string;
				strong += ",  Re = ";
				ReynoldsFormat(string, m_Re[m]);
				strong += string;

				string.Format(",  Cl = %6.2f is outside the flight envelope\r\n", m_Cl[m]);
				strong +=string;
				if(m_bTrace) pVLMDlg->AddString(strong);
				m_bWingOut = true;
			}
			m++;
		}
		if(pFoil0->m_bTEFlap && pFoil1->m_bTEFlap) nFlap++;
	} 

	//global plane dimensionless coefficients
	GCm += m_GCm + m_VCm;
	 //sign convention for rolling and yawing is opposite to algebric results
	GRm -= m_GRm;
	GYm -= m_GYm;
	VYm -= m_VYm;
	IYm -= m_IYm;

	//Note : the following results are unused, for information only
	double Area, Span;
	if(RefAreaType==1) 
	{
		Area = m_Area;
		Span = m_Span;
	}
	else 
	{
		Area = m_ProjectedArea;
		Span = m_ProjectedSpan;
	}

	// wing dimensionless coefficients
	m_GCm *=  1.0/ q / Area /m_MAChord;
	m_VCm *=  1.0/ q / Area /m_MAChord;
	 //sign convention for rolling and yawing is opposite to algebric results
	m_GRm *= -1.0/ q / Area /Span;
	m_GYm *= -1.0/ q / Area /Span;
	m_VYm *= -1.0/ q / Area /Span;
	m_IYm *= -1.0/ q / Area /Span;

}


void CWing::VLMSetBending()
{
	double ypos[MAXSTATIONS+1], zpos[MAXSTATIONS+1];
	int j,k,jj;
	double bm;
	CVector Dist(0.0,0.0,0.0);
	CVector Moment;
	int m = 0;
	int p = 0;

	for (j=0; j<m_NSurfaces; j++){//All surfaces
		for (k=0; k<m_Surface[j].m_NYPanels; k++)
		{
			ypos[m] = m_pPanel[p].CollPt.y;
			zpos[m] = m_pPanel[p].CollPt.z;

			p+=m_Surface[j].m_NXPanels;
			m++;
		}
	}

	for (j=0; j<m_NStation; j++)
	{
		bm = 0.0;
		if (ypos[j]<=0)
		{
			for (jj=0; jj<j; jj++)
			{
				Dist.y =  -ypos[jj]+ypos[j];
				Dist.z =  -zpos[jj]+zpos[j];
				Moment = Dist * m_F[jj];
				bm += Moment.x ;
			}
		}
		else
		{
			for (jj=j+1; jj<m_NStation; jj++)
			{
				Dist.y =  ypos[jj]-ypos[j];
				Dist.z =  zpos[jj]-zpos[j];
				Moment = Dist * m_F[jj];
				bm += Moment.x ;
			}
		}
		m_BendingMoment[j] = bm;
	}
}


void CWing::PanelComputeWing(double *Cp, double &VDrag, double &XCP, double &YCP,
                             double &GCm, double &GRm, double &GYm, double &VCm, double &VYm, double &IYm,
                             bool bViscous, bool bThinSurface, bool bTilted, int RefAreaType)
{
	CMainFrame *pFrame    = (CMainFrame*)s_pFrame;
	CMiarex *pMiarex      = (CMiarex*)s_pMiarex;
	C3DPanelDlg *p3DDlg   = (C3DPanelDlg*)s_p3DPanelDlg;
	CWPolar* pWPolar = pMiarex->m_pCurWPolar;

	// calculates the lift coefficients from the vortices strengths
	int  j, k, l, p, m, nFlap, coef;
	
	bool bPointOutRe, bPointOutCl, bOutRe, bError;	CString string, strong;
	CFoil *pFoil0, *pFoil1;
	CVector H, HA, HB, V1, HingeLeverArm, HingeMoment, DragMoment, GeomMoment, PtC4, PtLE, DragVector;
	CVector Force, SurfaceNormal, LeverArm, LeverArmC4, PanelLeverArm, PanelForce, StripForce, Moment0, WindNormal, WindDirection;
	double CPStrip, tau, NForce, Alpha, cosa, sina;

	bOutRe = bError = false;

	coef = 2;
	if(bThinSurface) coef = 1;

	if(!bTilted) Alpha = m_Alpha;
	else         Alpha = 0.0;

	cosa = cos(Alpha*pi/180.0);
	sina = sin(Alpha*pi/180.0);

	//   Define wind axis
	WindNormal.Set(   -sina, 0.0, cosa);
	WindDirection.Set( cosa, 0.0, sina);

	m_GRm = m_GCm = 0.0;
	m_GYm = m_IYm = m_VCm = m_VYm = 0.0;

	for (m=0; m< m_NStation; m++) m_Re[m] = m_Chord[m] * m_QInf /m_Viscosity;

	m = p = nFlap = 0;

	for (j=0; j<m_NSurfaces; j++)//All surfaces
	{
		if(!bThinSurface && m_Surface[j].m_bIsTipLeft) p += m_Surface[j].m_NXPanels;//do not consider left tip patch

		pFoil0 = m_Surface[j].m_pFoilA;
		pFoil1 = m_Surface[j].m_pFoilB;

		if(pFoil0->m_bTEFlap && pFoil1->m_bTEFlap)
		{
			//create a hinge unit vector and initialize hinge moment
			m_Surface[j].GetPoint(m_Surface[j].m_posATE, m_Surface[j].m_posBTE, 0.0, HA, 0);
			m_Surface[j].GetPoint(m_Surface[j].m_posATE, m_Surface[j].m_posBTE, 1.0, HB, 0);
			H = HB-HA;
			H.Normalize();
			m_FlapMoment[nFlap] = 0.0;
		}

		SurfaceNormal = m_Surface[j].Normal;
		for (k=0; k<m_Surface[j].m_NYPanels; k++)
		{
			bPointOutRe = false;
			bPointOutCl = false;
			StripForce.Set(0.0,0.0,0.0);//sum lift of all panels along chord
			m_StripArea[m] = 0.0;
			CPStrip        = 0.0;
			m_CmAirf[m]    = 0.0;

			m_Surface[j].GetLeadingPt(k, PtLE);
			m_Surface[j].GetC4(k, PtC4, tau);

			LeverArm = PtC4;
			LeverArm.x -= pWPolar->m_XCmRef;

			GeomMoment.Set(0.0,0.0,0.0);

			for (l=0; l<coef*m_Surface[j].m_NXPanels; l++)
			{
				PanelForce = m_pPanel[p].Normal * (-Cp[p]) * m_pPanel[p].Area;      // Newtons/q
				StripForce += PanelForce;                                           // Newtons/q
				NForce = PanelForce.dot(SurfaceNormal);                             // Newtons/q

				LeverArmC4 = m_pPanel[p].CollPt- PtC4;

				Moment0 = LeverArmC4 * PanelForce;                                  // N.m/q
				m_CmAirf[m]  += Moment0.y;                                          // N.m/q

				PanelLeverArm.x = m_pPanel[p].CollPt.x - pWPolar->m_XCmRef;
				PanelLeverArm.y = m_pPanel[p].CollPt.y;
				PanelLeverArm.z = m_pPanel[p].CollPt.z;
				GeomMoment += PanelLeverArm * PanelForce;

				m_StripArea[m]    += m_pPanel[p].Area;
				
				XCP       += m_pPanel[p].CollPt.x * PanelForce.dot(WindNormal); //global center of pressure
				YCP       += m_pPanel[p].CollPt.y * PanelForce.dot(WindNormal); 
				CPStrip   += m_pPanel[p].CollPt.x * NForce;

/*				if(pFoil0->m_bTEFlap && pFoil1->m_bTEFlap)
				{
					//add hinge moment contribution
					V1 = m_pPanel[p].VortexPos - HA;
					HingeLeverArm = V1 - H * V1.dot(H);
					if(HingeLeverArm.x>0.0)
					{
						HingeMoment = HingeLeverArm * PanelForce;				// N.m/q
						m_FlapMoment[nFlap] += HingeMoment.dot(H) * m_Density * m_QInf * m_QInf/2.0;//N
					}
				}*/
				if(m_Surface[j].m_bTEFlap)
				{
					if(m_Surface[j].IsFlapPanel(p))
					{
						//then p is on the flap, so add its contribution
						
						HingeLeverArm = m_pPanel[p].CollPt - m_Surface[j].m_HingePoint;
						HingeMoment = HingeLeverArm * PanelForce;//N.m/q
						m_FlapMoment[nFlap] += HingeMoment.dot(m_Surface[j].m_HingeVector)* m_Density * m_QInf * m_QInf/2.0;//N
					}
				}
				p++;
			}

			m_StripArea[m] /=(double)coef; //average over top and bottom

			NForce = StripForce.dot(SurfaceNormal);
			
			m_XCPSpanRel[m]    = (CPStrip/NForce - PtLE.x)/m_Chord[m];
			m_XCPSpanAbs[m]    =  CPStrip/NForce ;

			if(bViscous)
			{
				m_PCd[m]    = pMiarex->GetVar(2, pFoil0, pFoil1, m_Re[m], m_Cl[m], tau, bOutRe, bError);
				bPointOutRe = bOutRe || bPointOutRe;
				if(bError) bPointOutCl = true;

				m_XTrTop[m] = pMiarex->GetVar(5, pFoil0, pFoil1, m_Re[m], m_Cl[m], tau, bOutRe, bError);
				bPointOutRe = bOutRe || bPointOutRe;
				if(bError) bPointOutCl = true;
				
				m_XTrBot[m] = pMiarex->GetVar(6, pFoil0, pFoil1, m_Re[m], m_Cl[m], tau, bOutRe, bError);
				bPointOutRe = bOutRe || bPointOutRe;
				if(bError) bPointOutCl = true;
				m_ViscousDrag = m_PCd[m] * m_StripArea[m];
				VDrag         += m_PCd[m] * m_StripArea[m];
				DragVector.x = m_PCd[m]*m_StripArea[m];// N/q
			}
			else
			{
				m_PCd[m]     = 0.0;
				m_XTrBot[m]  = 1.0;
				m_XTrTop[m]  = 1.0;
				DragVector.x = 0.0;
			}

			//global moments, in N.m/q
			DragMoment = LeverArm * DragVector;

			m_GCm += GeomMoment.y;
			m_GRm += GeomMoment.dot(WindDirection);
			m_GYm += GeomMoment.dot(WindNormal);

			m_VCm += DragMoment.y;
			m_VYm += DragMoment.dot(WindNormal);

			m_IYm += -m_ICd[m] * m_StripArea[m] * PtC4.y ; 

			m_CmAirf[m]    *= 1.0/m_Chord[m]/m_StripArea[m];//vectorial formulation
			m_CmXRef[m]     = (GeomMoment.y+DragMoment.y)/m_Chord[m]/m_StripArea[m];
			m_Cm[m]         = m_CmAirf[m]+m_CmXRef[m];

			if(bPointOutCl)
			{
				GetLengthUnit(string, pFrame->m_LengthUnit);
				strong.Format("       Span pos = %9.2f ",	m_SpanPos[m]*pFrame->m_mtoUnit);
				strong += string;
				strong += ",  Re = ";
				ReynoldsFormat(string, m_Re[m]);
				strong += string;

				string.Format(",  Cl = %6.2f could not be interpolated\r\n", m_Cl[m]);
				strong+=string;
				if(m_bTrace) p3DDlg->AddString(strong);
				m_bWingOut = true;

			}
			else if(bPointOutRe)
			{
				GetLengthUnit(string, pFrame->m_LengthUnit);
				strong.Format("       Span pos = %9.2f ",	m_SpanPos[m]*pFrame->m_mtoUnit);
				strong += string;
				strong += ",  Re = ";
				ReynoldsFormat(string, m_Re[m]);
				strong += string;

				string.Format(",  Cl = %6.2f is outside the flight envelope\r\n", m_Cl[m]);
				strong +=string;
				if(m_bTrace) p3DDlg->AddString(strong);
				m_bWingOut = true;
			}
			m++;
		}
		if(!bThinSurface && m_Surface[j].m_bIsTipRight) p += m_Surface[j].m_NXPanels;//do not consider right tip patch
		if(pFoil0->m_bTEFlap && pFoil1->m_bTEFlap) nFlap++;
	}

	//global plane dimensionless coefficients
	GCm += m_GCm + m_VCm;

	//sign convention for rolling and yawing is opposite to algebric results
	GRm -= m_GRm;
	GYm -= m_GYm;
	VYm -= m_VYm;
	IYm -= m_IYm;


	//Note : the following results are unused,for information only
	double Area, Span;
	if(RefAreaType==1) 
	{
		Area = m_Area;
		Span = m_Span;
	}
	else 
	{
		Area = m_ProjectedArea;
		Span = m_ProjectedSpan;
	}
	// wing dimensionless coefficients
	m_GCm *=  1.0 / Area /m_MAChord;
	m_VCm *=  1.0 / Area /m_MAChord;
	//sign convention for rolling and yawing is opposite to algebric results
	m_GRm *= -1.0 / Area /Span;
	m_GYm *= -1.0 / Area /Span;
	m_VYm *= -1.0 / Area /Span;
	m_IYm *= -1.0 / Area /Span;

}



void CWing::VLMTrefftz(double *Gamma, int pos, CVector &Force, double & Drag, bool bTilted)
{
	// calculates the lift and induced drag from the vortices strengths
	// using a farfield method
	// Downwash is evaluated at a distance 10 times the span downstream (i.e. infinite)
	//

	CVLMDlg *pVLMDlg = (CVLMDlg*)s_pVLMDlg;
	CMiarex * pMiarex = (CMiarex*)s_pMiarex;

	int j , k, l,  p,  m;
	double alpha, cosa, sina;
	CVector  C, V, dF, StripForce, WindDirection, WindNormal;
	CVector Wg (0.0,0.0,0.0);
	CVector VInf;
	double GamShed[MAXSTATIONS];

	if(!bTilted) alpha = m_Alpha;
	else         alpha = 0.0;
	
	cosa = cos(alpha*pi/180.0);
	sina = sin(alpha*pi/180.0);

	//   Define wind axis
	WindNormal.Set(   -sina, 0.0, cosa);
	WindDirection.Set( cosa, 0.0, sina);

	VInf = WindDirection * m_QInf;

	m_CL = 0.0;
	m_InducedDrag = 0.0;

	p=0;
	m=0;
	int n=0;
	int N = 50;

	for (j=0; j<m_NSurfaces; j++)
	{
		for (k=0; k<m_Surface[j].m_NYPanels; k++)
		{
			StripForce.Set(0.0,0.0,0.0);
			m_StripArea[m] = 0.0;
			m_Cl[m]        = 0.0;
			m_ICd[m]       = 0.0;
			GamShed[m]     = 0.0;

			for(l=0; l<m_Surface[j].m_NXPanels; l++)
			{
				m_StripArea[m]+=m_pPanel[p].Area;
				if(m_bVLM1 || m_pPanel[p].m_bIsTrailing)
				{
					C = m_pPanel[p].CtrlPt;
//					C = m_pPanel[p].VortexPos;//Makes absolutely no difference on results
					C.x = m_Span * 100.0;

					Wg  = pVLMDlg->GetSpeedVector(C, Gamma);
					Wg += VInf; //total speed vector

					//induced force
					dF  = Wg * m_pPanel[p].Vortex; 
					dF *= Gamma[p+pos];			// N/rho

					//project on wind axes
					m_Cl[m]       += dF.dot(WindNormal);
					m_CL          += dF.dot(WindNormal);
					m_ICd[m]      += dF.dot(WindDirection);
					m_InducedDrag += dF.dot(WindDirection);

					StripForce += dF;			// N/rho

					GamShed[m] += Gamma[p+pos];
				}
				p++;
			}

//TRACE("%12.5f      %12.5f\n", m_SpanPos[m],GamShed);

			m_Cl[m]  *= 2.0/m_StripArea[m] /m_QInf/m_QInf;
			m_ICd[m] *= 1.0/m_StripArea[m] /m_QInf/m_QInf;

			Force += StripForce;                    //N/rho
			m_F[m] = StripForce * m_Density;		//Newtons : F = rho * (V x Gamma) Kutta_Joukowski 
			if(bTilted) m_F[m].RotateY(-m_Alpha);
			m++;
		}
	}


	Drag       +=  m_InducedDrag;

	m_InducedDrag *= -1.0       /m_Area/m_QInf;// is the wing's induced drag unused afterwards
	m_CL          *=  1.0       /m_Area/m_QInf;// is the wing's lift... unused afterwards
}


void CWing::PanelTrefftz(double *Cp, double *Mu, double *Sigma, int pos, CVector &Force, double &Drag, 
						 bool bTilted, bool bThinSurf, CPanel *pWakePanel, CVector *pWakeNode)
{
	// calculates the induced lift and drag from the vortices or wake panels strength
	// using a farfield method
	// Downwash is evaluated at a distance 100 times the span downstream (i.e. infinite)

	C3DPanelDlg *p3DDlg = (C3DPanelDlg*)s_p3DPanelDlg;

	int nw, iTA, iTB;
	int j, k, l, p, pp, m;
	double StripArea, InducedAngle, IYm, cosa, sina;
	double GammaStrip[MAXSTATIONS];
	CVector C, V, Wg, StripForce, dF, WindDirection, WindNormal, VInf;

	if(bTilted)
	{
		cosa = 1.0;
		sina = 0.0;
	}
	else
	{
		cosa = cos(m_Alpha*pi/180.0);
		sina = sin(m_Alpha*pi/180.0);
	}


	//   Define wind axis
	WindNormal.Set(   -sina, 0.0, cosa);
	WindDirection.Set( cosa, 0.0, sina);

	VInf = WindDirection * m_QInf;

	//dynamic pressure, kg/m3
	double q = 0.5 * m_Density * m_QInf * m_QInf;

	m_CL          = 0.0;
	m_InducedDrag = 0.0;
	IYm           = 0.0;

	int coef = 2;
	if (bThinSurf) coef = 1;

	p=0;
	m=0;
	for (j=0; j<m_NSurfaces; j++)
	{
		if(m_Surface[j].m_bIsTipLeft && !bThinSurf) p+=m_Surface[j].m_NXPanels;//tip patch panels

		for (k=0; k<m_Surface[j].m_NYPanels; k++)
		{
			nw  = m_pPanel[p].m_iWake;
			iTA = pWakePanel[nw].m_iTA;
			iTB = pWakePanel[nw].m_iTB;
			C = (pWakeNode[iTA] + pWakeNode[iTB])/2.0;


            pp = p;
			StripArea = 0.0;
			for (l=0; l<coef*m_Surface[j].m_NXPanels; l++)
			{
				StripArea  += m_pPanel[pp].Area;
				pp++;
			}
			StripArea /= (double)coef;
			
			// ____________________________
			// Downwash calculation
			//
			// Since we place the trailing point at the end of the wake panels, it sees only the effect 
			// of the upstream part of the wake because the downstream part isn't modelled.
			// If we were to model the downstream part, the total induced speed would be twice larger,
			// so just add a factor 2 to account for this.

			p3DDlg->GetSpeedVector(C, Mu, Sigma, Wg);
			InducedAngle = atan2(Wg.dot(WindNormal), m_QInf);
			m_Ai[m]      = 2.0 * InducedAngle*180/pi;

			// ____________________________
			// Lift calculation
			//
			// Method 1 : Sum panel pressure forces over the top and bottom strip.
			// The induced drag is calculated by projection of the strip force on the wind direction
			// General experience in published literature shows this isn't such a good idea

/*			StripForce.Set(0.0, 0.0, 0.0);
			for (l=0; l<coef*m_Surface[j].m_NXPanels; l++)
			{
				StripForce += m_pPanel[p].Normal * (-Cp[p]) * m_pPanel[p].Area;  // N/q
				p++;
			}*/

			// Method 2 : Far-field plane integration
			// This is the method generally recommended

			GammaStrip[m] = (-Mu[pos+p+coef*m_Surface[j].m_NXPanels-1] + Mu[pos+p]) *4.0*pi;
			Wg += VInf;

			StripForce  = m_pPanel[p].Vortex * Wg;
			StripForce *= GammaStrip[m] * m_Density / q;  // N/q

			p  += coef*m_Surface[j].m_NXPanels;
			//____________________________
			//
			// The results are given in wind axis, whether the geom is tilted or not,
			// so project on wind axes			

			m_Cl[m]        = StripForce.dot(WindNormal)   /StripArea;
			m_ICd[m]       = StripForce.dot(WindDirection)/StripArea;

			m_CL          += StripForce.dot(WindNormal);                // N/q
			m_InducedDrag += StripForce.dot(WindDirection);             // N/q

			// Calculate resulting vector force
			Force         += StripForce;                                // N/q
			m_F[m]         = StripForce * q;	                        //Newtons
					
			if(bTilted) m_F[m].RotateY(-m_Alpha);

			m++;
		}		
		if(m_Surface[j].m_bIsTipRight && !bThinSurf) p+=m_Surface[j].m_NXPanels;//tip patch panels
	}

	Drag      += m_InducedDrag;

	m_CL          *=  1.0       /m_Area;  // unused
	m_InducedDrag *= -2.0       /m_Area;   // unused
}



void CWing::PanelSetBending()
{
	double ypos[MAXSTATIONS+1], zpos[MAXSTATIONS+1];
	int j,k,jj;
	double bm;
	CVector Dist(0.0,0.0,0.0);
	CVector Moment;
	int m =0;
	int p= m_Surface[0].m_NXPanels;

	for (j=0; j<m_NSurfaces; j++)
	{	
		for (k=0; k<m_Surface[j].m_NYPanels; k++)
		{
			ypos[m] = m_pPanel[p].CollPt.y;
			zpos[m] = m_pPanel[p].CollPt.z;

			p += 2*m_Surface[j].m_NXPanels;
			m++;
		}
	}

	for (j=0; j<m_NStation; j++)
	{
		bm = 0.0;
		if (ypos[j]<=0)
		{
			for (jj=0; jj<j; jj++)
			{
				Dist.y =  -ypos[jj]+ypos[j];
				Dist.z =  -zpos[jj]+zpos[j];
				Moment = Dist * m_F[jj];
				bm += Moment.x ;
			}
		}
		else
		{
			for (jj=j+1; jj<m_NStation; jj++)
			{
				Dist.y =  ypos[jj]-ypos[j];
				Dist.z =  zpos[jj]-zpos[j];
				Moment = Dist * m_F[jj];
				bm += Moment.x ;
			}
		}
		m_BendingMoment[j] = bm;
	}
}


void CWing::CreateXPoints(int NXPanels, int XDist, CFoil *pFoilA, CFoil *pFoilB,
                          double *xPointA, double *xPointB, int &NXLead, int &NXFlap)
{
	// the chordwise panel distribution is set i.a.w. with the flap hinges;

	int l;
	int NXFlapA, NXFlapB, NXLeadA, NXLeadB;
	double dl, dl2;
	double xHingeA, xHingeB;
	
	if(pFoilA->m_bTEFlap) xHingeA=pFoilA->m_TEXHinge/100.0; else xHingeA=1.0;
	if(pFoilB->m_bTEFlap) xHingeB=pFoilB->m_TEXHinge/100.0; else xHingeB=1.0;

	NXFlapA = (int)((1.0-xHingeA) * NXPanels); 
	NXFlapB = (int)((1.0-xHingeB) * NXPanels);
	if(pFoilA->m_bTEFlap && NXFlapA==0) NXFlapA++;
	if(pFoilB->m_bTEFlap && NXFlapB==0) NXFlapB++;
	NXLeadA = NXPanels - NXFlapA;
	NXLeadB = NXPanels - NXFlapB;

	NXFlap  = max(NXFlapA, NXFlapB);
	if(NXFlap>NXPanels/2) NXFlap=(int)NXPanels/2;
	NXLead  = NXPanels - NXFlap;

	for(l=0; l<NXFlapA; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXFlapA;
		if(XDist==1) 
			xPointA[l] = 1.0 - (1.0-xHingeA)/2.0 * (1.0-cos(dl*pi /dl2));
		else 
			xPointA[l] = 1.0 - (1.0-xHingeA) * (dl/dl2);
	}
	for(l=0; l<NXLeadA; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXLeadA;
		if(XDist==1)
			xPointA[l+NXFlapA] = xHingeA - (xHingeA)/2.0 * (1.0-cos(dl*pi /dl2));
		else
			xPointA[l+NXFlapA] = xHingeA - (xHingeA) * (dl/dl2);
	}
	
	for(l=0; l<NXFlapB; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXFlapB;
		if(XDist==1) 
			xPointB[l] = 1.0 - (1.0-xHingeB)/2.0 * (1.0-cos(dl*pi /dl2));
		else 
			xPointB[l] = 1.0 - (1.0-xHingeB) * (dl/dl2);
	}
	for(l=0; l<NXLeadB; l++)
	{
		dl =  (double)l;
		dl2 = (double)NXLeadB;
		if(XDist==1)
			xPointB[l+NXFlapB] = xHingeB - (xHingeB)/2.0 * (1.0-cos(dl*pi /dl2));
		else
			xPointB[l+NXFlapB] = xHingeB - (xHingeB) * (dl/dl2);
	}
	
	xPointA[NXPanels] = 0.0;
	xPointB[NXPanels] = 0.0;
}




bool CWing::SplineInterpolation(int n, double *x, double *y, double *a, double *b, double *c, double *d)
{
//
// Given an array of n+1 pairs (x[i], y[i]), with i ranging from 0 to n, 
// this function calculates the 3rd order cubic spline which interpolate the pairs.
//
// The spline is defined for each interval [x[j], x[j+1]) by n third order polynomial functions
//              p_j(x) = ax3 + bx2 + cx + d
//
// The equations to determine the coefficients a,b,c,d are
//
// Interpolation : 2n conditions
//    p_j(x[j])   = y[j];
//    p_j(x[j+1]) = y[j+1];
//
// Continuity of 1st and 2nd order derivatives at internal points: 2(n-1) conditions
//    p_j'(x[j]) = p_j+1'(x[j])
//    p_j"(x[j]) = p_j+1"(x[j])
//
// Second order derivative is zero at the end points : 2 conditions
//    p_j"(x[0]) =  p_j"(x[n]) =0
//
//
// This sets a linear system of size 4n which is solved by the Gauss algorithm for coefs a,b,c and d
// The RHS vector is 
//	  a[0]
//	  b[0]
//	  c[0]
//	  d[0]
//	  a[1]
//    ...
//	  d[n-1]
//

	if(n>50) return false;
	int i,size;

	double *M;// size is 4 coefs x maxstations
	double *RHS;
	CVLMDlg *pVLMDlg   = (CVLMDlg*)s_pVLMDlg;

	M = pVLMDlg->m_aij;
	RHS = pVLMDlg->m_RHS;

/*	for(i=0;i<4*n;i++)
	{
		for(j=0; j<4*n; j++) M[i*4*n+j] = 0.0;
		RHS[i] = 0.0;
	}*/

	memset(M, 0, 16*n*n*sizeof(double));
	memset(RHS, 0, 4*n*sizeof(double));

	size = 4*n;
//	Interpolation conditions
	for (i=0; i<n; i++)
	{	
		//pj(x[i]) = y[i] 
		M[2*i*size +4*i]     = x[i]*x[i]*x[i];
		M[2*i*size +4*i + 1] = x[i]*x[i];
		M[2*i*size +4*i + 2] = x[i];
		M[2*i*size +4*i + 3] = 1.0;

		//pj(x[i+1]) = y[i+1] 
		M[(2*i+1)*size +4*i]     = x[i+1]*x[i+1]*x[i+1];
		M[(2*i+1)*size +4*i + 1] = x[i+1]*x[i+1];
		M[(2*i+1)*size +4*i + 2] = x[i+1];
		M[(2*i+1)*size +4*i + 3] = 1.0;
		
		RHS[2*i]   = y[i];
		RHS[2*i+1] = y[i+1];
	}

//  Derivation conditions
	for (i=1; i<n; i++)
	{
		//continuity of 1st order derivatives
		
		M[(2*n+i)*size + 4*(i-1)]     =  3.0*x[i]*x[i];
		M[(2*n+i)*size + 4*(i-1)+1]   =  2.0     *x[i];
		M[(2*n+i)*size + 4*(i-1)+2]   =  1.0;

		M[(2*n+i)*size + 4*i]   = -3.0*x[i]*x[i];
		M[(2*n+i)*size + 4*i+1] = -2.0     *x[i];
		M[(2*n+i)*size + 4*i+2] = -1.0;
		
		RHS[2*n+i]   = 0.0;

		//continuity of 2nd order derivatives
		M[(3*n+i)*size + 4*(i-1)]     =  6.0*x[i];
		M[(3*n+i)*size + 4*(i-1)+1]   =  2.0     ;

		M[(3*n+i)*size + 4*i]   = -6.0*x[i];
		M[(3*n+i)*size + 4*i+1] = -2.0     ;
		
		RHS[3*n+i]   = 0.0;
	}

//	second order derivative is zero at end points = "natural spline"
	M[2*n*size]     = 6.0*x[0];
	M[2*n*size+1]   = 2.0;
	RHS[2*n]        = 0.0;

	M[3*n*size + size-4]   = 6.0*x[n];
	M[3*n*size + size-3]   = 2.0;
	RHS[3*n+1]             = 0.0;

	if(!Gauss(M, 4*n, RHS, 1)) 
		return false;

	for(i=0; i<n; i++)
	{
		a[i] = RHS[4*i];
		b[i] = RHS[4*i+1];
		c[i] = RHS[4*i+2];
		d[i] = RHS[4*i+3];
	}

	return true;
}


void CWing::VLMCubicSplines(double *Gamma)
{
	CVLMDlg *pVLMDlg = (CVLMDlg*)s_pVLMDlg;
	CVector C, Wg;
	int m,p,j;

	double a[MAXSTATIONS], b[MAXSTATIONS], c[MAXSTATIONS], d[MAXSTATIONS];

	p=0;
	m=0;
/*
int k,l;
	double y[MAXSTATIONS],Wgz[MAXSTATIONS];
	for (j=0; j<m_NSurfaces; j++)
	{
		for (k=0; k<m_Surface[j].m_NYPanels; k++)
		{
			for(l=0; l<m_Surface[j].m_NXPanels; l++)
			{
				if(m_pPanel[p].m_bIsTrailing)
				{
						C = m_pPanel[p].CtrlPt;
						C.x = m_Span * 100.0;
						Wg  = pVLMDlg->GetSpeedVector(C, Gamma);
				}
				p++;
			}
			Wgz[m] = Wg.z;
			y[m] = C.y;
//TRACE("%14.5f    %14.5f    \n", y[m], Wgz[m]);
			m++;
		}
	}*/

	if(!SplineInterpolation(m_NStation, m_SpanPos, m_ICd, a,b,c,d)) return;

	double t=-m_Span/2.0;
	double dt = m_Span/100.0;
	double res;

	for(j=0; j<=100; j++)
	{
		res = GetInterpolation(t, m_SpanPos, m_NStation, a,b,c,d);
//TRACE("%14.5f    %14.5f    \n", t, res);
		t += dt;
	}
}



double CWing::GetInterpolation(double t, double *y, int m, double *a, double *b, double *c, double *d)
{
	if(t<=y[0]) 
		return a[0] * t*t*t + b[0] *t*t + c[0] * t +d[0];

	for (int k=1; k<m; k++)
	{
		if(t<=y[k]) 
			return a[k-1] * t*t*t + b[k-1] *t*t + c[k-1] * t +d[k-1];
		
	}
	return a[m-1] * t*t*t + b[m-1] *t*t + c[m-1] * t +d[m-1];
}



