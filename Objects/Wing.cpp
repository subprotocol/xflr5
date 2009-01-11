/****************************************************************************

    Wing Class
	Copyright (C) 2005-2009 Andre Deperrois xflr5@yahoo.com

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


#include  <math.h>
#include "Wing.h"
#include "MainFrame.h"
#include "Globals.h"

/////////////////////////////////////////////////////////////////////////////
// CWing dialog
CVector *CWing::m_pWakeNode;			//pointer to the VLM wake node array
CPanel  *CWing::m_pWakePanel;			//pointer to the VLM Wake Panel array

void* CWing::s_pFrame;		//pointer to the Frame window
void* CWing::s_pMiarex;	//pointer to the Miarex Application window
void* CWing::s_pVLMDlg;	//pointer to the VLM analysis dialog class
void* CWing::s_pLLTDlg;	//pointer to the VLM analysis dialog class
void* CWing::s_p3DPanelDlg;//pointer to the 3DPanel analysis dialog class
int CWing::s_NLLTStations;//pointer to the 3DPanel analysis dialog class
double CWing::s_CvPrec;	// Precision required for LLT convergence
double CWing::s_RelaxMax;	// relaxation factor for LLT convergence


CWing::CWing()
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
	m_VYm               = 0.0;
	m_XCP               = 0.0;
	m_YCP               = 0.0;

	m_Type = 1;
	m_Viscosity = 0.0;
	m_Density   = 0.0;

	m_AVLIndex = -987654;//improbable value...

	m_MinPanelSize = .01;//change !

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


	for (i=0;i<=MAXPANELS; i++)
	{
		m_RFoil.append(" ");
		m_LFoil.append(" ");
	}
}


void CWing::ComputeDihedrals()
{
	for (int n=0; n<m_NPanel; n++){
		m_TDihedral[n] = 180.0/pi* atan2(m_TZPos[n+1]-m_TZPos[n], m_TPos[n+1]-m_TPos[n]);
	}
}

void CWing::ComputeGeometry()
{
	// Computes the wing's characteristics from the panel data
	MainFrame *pFrame  = (MainFrame*)s_pFrame;
	CFoil *pFoilA, *pFoilB;
	double MinPanelSize;
	int i, k;

	double surface = 0.0;
	m_TLength[0] = 0.0;
	m_TYProj[0]  = m_TPos[0];
	for (i=1; i<=m_NPanel; i++)
		m_TLength[i] = m_TPos[i] - m_TPos[i-1];

	for (i=1; i<=m_NPanel; i++)
		m_TYProj[i] = m_TYProj[i-1] + m_TLength[i] * cos(m_TDihedral[i-1]*pi/180.0);

	m_Span    = 2.0 * m_TPos[m_NPanel];
	m_MAChord = 0.0;
	m_yMac    = 0.0;
	m_Volume  = 0.0;
	for (k=0; k<m_NPanel; k++)
	{
		pFoilA = pFrame->GetFoil(m_RFoil[k]);
		pFoilB = pFrame->GetFoil(m_RFoil[k+1]);
		surface   += m_TLength[k+1]*(m_TChord[k]+m_TChord[k+1])/2.0;//m²
		if(pFoilA && pFoilB)
			m_Volume  += m_TLength[k+1]*(pFoilA->GetArea()*m_TChord[k] + pFoilB->GetArea()*m_TChord[k+1])/2.0;//m3
//		m_MAChord += IntegralC2(m_TPos[k], m_TPos[k+1], m_TChord[k], m_TChord[k+1]);
//		m_yMac    += IntegralCy(m_TPos[k], m_TPos[k+1], m_TChord[k], m_TChord[k+1]);
	}
	if(!m_bIsFin || m_bSymFin || m_bDoubleFin)
	{
		m_Area    = 2.0 * surface;
		m_Volume *= 2.0;
		m_MAChord = m_MAChord * 2.0 / m_Area;
		m_yMac    = m_yMac    * 2.0 / m_Area;

		m_GChord  = m_Area/m_Span;
		m_AR      = m_Span*m_Span/m_Area;
	}
	else
	{
		m_Area = surface;
		m_MAChord = m_MAChord / m_Area;
		m_yMac    = m_yMac    / m_Area;

		m_GChord  = m_Area/m_Span*2.0;
		m_AR      = m_Span*m_Span/m_Area/2.0;
	}
	if(m_TChord[m_NPanel]>0.0)	m_TR = m_TChord[0]/m_TChord[m_NPanel];
	else						m_TR = 99999.0;

	//calculate the number of flaps
	m_nFlaps = 0;
//	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
//	else                            MinPanelSize = m_Span;

	MinPanelSize = m_MinPanelSize;
	for (i=1; i<=m_NPanel; i++)
	{
		pFoilA = pFrame->GetFoil(m_RFoil[i-1]);
		pFoilB = pFrame->GetFoil(m_RFoil[i]);
		if(pFoilA && pFoilB)
		{
			if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && qAbs(m_TPos[i]-m_TPos[i-1])>MinPanelSize)	m_nFlaps++;
		}
		pFoilA = pFrame->GetFoil(m_LFoil[i-1]);
		pFoilB = pFrame->GetFoil(m_LFoil[i]);
		if(pFoilA && pFoilB)
		{
			if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && qAbs(m_TPos[i]-m_TPos[i-1])>MinPanelSize)	m_nFlaps++;
		}
	}
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

	MainFrame *pFrame  = (MainFrame*)s_pFrame;
//	CMiarex    *pMiarex = (CMiarex*)   s_pMiarex;

//	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
	//else                            MinPanelSize = m_Span;
	MinPanelSize = m_MinPanelSize;//TODO

	//first generate the surfaces - from left tip to right tip
	// so start with left wing, in reverse order
	m_NSurfaces = 0;
	m_MatSize = 0;

	VNormal[0].Set(0.0, 0.0, 1.0);
	VNSide[0].Set(0.0, 0.0, 1.0);

	nSurf=0;
	for(j=0; j<m_NPanel;j++)
	{
		if (fabs(m_TPos[j]-m_TPos[j+1]) > MinPanelSize)
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
		if (fabs(m_TPos[j]-m_TPos[j+1]) > MinPanelSize)
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
			if(fabs(m_TPos[j]-m_TPos[j+1]) > MinPanelSize)
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

	NXFlap  = qMax(NXFlapA, NXFlapB);
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
            y = fabs(yob * m_Span/2);
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


/*
bool CWing::ExportAVLWing(CStdioFile *pXFile, int index, double x, double y, double z, double Thetax, double Thetay)
{ 
	// Exports the current wing to AVL format

	MainFrame *pFrame = (MainFrame*)s_pFrame;
	CStdioFile XFile;
	CFileException fe;
	int j;
    QString strong, str;

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
*/



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
		max = fabs(*(pa + row));
		pA = pa + n;
		pivot_row = row;
		for (i=row+1; i < n; pA+=n, i++)
			if ((dum = fabs(*(pA+row))) > max) {
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

 
double CWing::GetAverageSweep()
{
	double xroot = m_TChord[0]/4.0;
	double xtip  = m_TOffset[m_NPanel] + m_TChord[m_NPanel]/4.0;
//	double sweep = (atan2(xtip-xroot, m_Span/2.0)) * 180.0/pi;
	return (atan2(xtip-xroot, m_Span/2.0)) * 180.0/pi;
}

double CWing::GetC4(double yob, double xRef)
{
	//returns the quarter-chord point xposition relative to XCmRef
	double Chord, Offset, tau;
	double C4 = 0.0;
	double y = fabs(yob*m_Span/2.0);
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

	y= fabs(yob*m_Span/2.0);//geometry is symetric
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
	int i;
	double y;

	y= fabs(yob*m_Span/2.0);//geometry is symetric
	for(i=0; i<m_NPanel; i++){
		if(m_TPos[i]<= y && y <=m_TPos[i+1]){
			if(yob>=0) return m_TDihedral[i];
			else  return -m_TDihedral[i];
		}
	}
	return 0.0;
}


void CWing::GetFoils(CFoil **pFoil0, CFoil **pFoil1, double y, double &t)
{
    int i;
	MainFrame *pFrame = (MainFrame*)s_pFrame;
	
	if  (y>0.0) 
	{
		//search Right wing
        for (i=0; i<m_NPanel; i++)
        {
            if (m_TPos[i]<=y && y<=m_TPos[i+1])
            {

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
        for (i=0; i<m_NPanel; i++)
        {
            if (m_TPos[i]<=y && y<m_TPos[i+1])
            {
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

double CWing::GetOffset(double yob)
{
	double tau, y;
	double Offset = 0.0;

	y= fabs(yob*m_Span/2.0);
	for(int i=0; i<m_NPanel; i++){
		if(m_TPos[i]<= y && y <=m_TPos[i+1]){
			tau = (y - m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			Offset = m_TOffset[i] + tau * (m_TOffset[i+1] - m_TOffset[i]);
			return Offset;
		}
	}

	return -1.0;
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
        y=fabs(y);
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


void CWing::GetViewYZPos(double xrel, double y, double &yv, double &zv, int pos)
{
	double tau;
	double twist, chord;
	zv = 0.0;
	yv = 0.0;
	double fy = fabs(y);
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


double CWing::Getyrel(double SpanPos)
{
    double y = fabs(SpanPos);
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

    y= fabs(y);
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


void CWing::ScaleChord(double NewChord)
{
	// Scales the wing chord-wise so that the root chord reaches the NewChord value

	double ratio = NewChord/m_TChord[0];
	for (int i=0; i<=MAXPANELS; i++){
		m_TChord[i]    *= ratio;
		m_TOffset[i]   *= ratio;
	}
//	//ComputeGeometry();
}

void CWing::ScaleSpan(double NewSpan)
{
	// Scales the wing span-wise to the NewSpan value
	for (int i=0; i<=MAXPANELS; i++){
		m_TPos[i]      *= NewSpan/m_Span;
		m_TLength[i]   *= NewSpan/m_Span;
	}
	//ComputeGeometry();

}


bool CWing::SerializeWing(QDataStream &ar, bool bIsStoring)
{
	//saves or loads the wing data to the archive ar

	int i;
//	MainFrame *pFrame = (MainFrame*)s_pFrame;
	int ArchiveFormat;// identifies the format of the file

	if(bIsStoring)
	{	// storing code
		ar << 1007;
			//1007 : Changed length units to m
			//1006 : Added Wing Color v2.99-15
			//1005 : Added Chordwise spacing (v2.99-00)
			//1004 : corrected m_NXPanels, m_NYPanels, m_YPanelDist to int (v1.99-33)
			//1003 : AVL Format (v1.99-18)
			//1002 : save VLM Mesh (v1.99-12)
			//1001 : initial format
		WriteCString(ar, m_WingName);

		ar << 0; //non elliptic...

		if(m_bSymetric) ar << 1; else ar<<0;

		ar << m_NPanel;

		for (i=0; i<=m_NPanel; i++) WriteCString(ar, m_RFoil[i]);
		for (i=0; i<=m_NPanel; i++) WriteCString(ar, m_LFoil[i]);
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

		WriteCOLORREF(ar,m_WingColor);
		return true;
	}
	else
	{
		// loading code
		float f;
		int k;
		ar >> ArchiveFormat;
		if (ArchiveFormat <1001 || ArchiveFormat>1100) {
			m_WingName = "";
			return false;
		}

		ReadCString(ar,m_WingName);
		if (m_WingName.length() ==0)
		{
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
		if(m_NPanel <=0 || m_NPanel>=100)
		{
			m_WingName = "";
			return false;
		}
		for (i=0; i<=m_NPanel; i++) ReadCString(ar, m_RFoil[i]);
		for (i=0; i<=m_NPanel; i++) ReadCString(ar, m_LFoil[i]);
		for (i=0; i<=m_NPanel; i++)
		{
			ar >> f; m_TChord[i]=f;
			if (fabs(m_TChord[i]) <0.0)
			{
				m_WingName = "";
				return false;
			}
		}

		for (i=0; i<=m_NPanel; i++)
		{
			ar >> f; m_TPos[i]=f;
			if (fabs(m_TPos[i]) <0.0) {
				m_WingName = "";

				return false;
			}
		}
		for (i=0; i<=m_NPanel; i++)
		{
			ar >> f; m_TOffset[i]=f;
		}

		if(ArchiveFormat<1007){
			//convert mm to m
			for (i=0; i<=m_NPanel; i++)
			{
				m_TPos[i]    /= 1000.0;
				m_TChord[i]  /= 1000.0;
				m_TOffset[i] /= 1000.0;
			}

		}
		for (i=0; i<=m_NPanel; i++)
		{
			ar >> f; m_TDihedral[i]=f;
		}
		for (i=0; i<=m_NPanel; i++)
		{
			ar >> f; m_TTwist[i]=f;
		}

		ar >> f; //m_XCmRef=f;
//		if(ArchiveFormat<1007) m_XCmRef /= 1000.0;

//		//ComputeGeometry();

		ar >> k;
		if (k==1)      m_bVLMAutoMesh = true;
		else if (k==0) m_bVLMAutoMesh = false;
		else{
			m_WingName = "";
			return false;
		}

		for (i=0; i<=m_NPanel; i++)
		{
			if(ArchiveFormat<=1003)
			{
				ar >> f;
				m_NXPanels[i] = (int)f;
			}
			else
				ar >> m_NXPanels[i];
			m_NXPanels[i] = max(1,m_NXPanels[i] );
			m_NXPanels[i] = min(MAXCHORDPANELS, m_NXPanels[i]);
		}

		for (i=0; i<=m_NPanel; i++)
		{
			if(ArchiveFormat<=1003)
			{
				ar >> f;
				m_NYPanels[i] = (int)f;
			}
			else 	ar >> m_NYPanels[i];
			m_NYPanels[i] = max(1,m_NYPanels[i] );
			m_NYPanels[i] = min(50, m_NYPanels[i]);
		}
		int total = 0;
		for (i=0; i<m_NPanel; i++)
		{
			total += m_NYPanels[i];
		}

		if(total*2>=MAXSTATIONS)
		{
			double ratio = MAXSTATIONS/total/2.0;
			for (i=0; i<=m_NPanel; i++)
			{
				m_NYPanels[i] = (int) (ratio*m_NYPanels[i]);
			}
		}

		int spanpos = 0;
		int vlmpanels = 0;
		for (int l=0; l<m_NPanel; l++)
		{
			spanpos  += m_NYPanels[l];
			vlmpanels +=m_NXPanels[l]*m_NYPanels[l];
		}


		if (ArchiveFormat >=1005)
		{
			for (i=0; i<=m_NPanel; i++) ar >> m_XPanelDist[i];
		}

		for (i=0; i<=m_NPanel; i++)
		{
			ar >> m_YPanelDist[i];
		}

		if(ArchiveFormat>=1006)
		{
			ReadCOLORREF(ar, m_WingColor);
		}
		return true;
	}
}



void CWing::SetSweep(double Sweep)
{
	int i;
//	double CurrentSweep = GetAverageSweep();
	double OldTipOffset = m_TOffset[m_NPanel];
	double NewTipOffset = m_TChord[0]/4.0
						 + tan(Sweep*pi/180.0)*m_Span/2.0
						 - m_TChord[m_NPanel]/4.0;
	if(fabs(OldTipOffset)>0.00001)
	{
		//scale each panel's offset
		double ratio = NewTipOffset/OldTipOffset;
		for(i=1; i<=m_NPanel; i++)
		{
			m_TOffset[i] *= ratio;
		}
	}
	else
	{
		//set each panel's offset
		for(i=1; i<=m_NPanel; i++)
		{
			m_TOffset[i] = NewTipOffset*m_TPos[i]/(m_Span/2.0);
		}
	}
	//ComputeGeometry();
}

void CWing::SetTwist(double Twist)
{
	if(fabs(m_TTwist[m_NPanel])>0.0001)
	{
		//scale each panel's twist
		double ratio = Twist/m_TTwist[m_NPanel];

		for(int i=1; i<=m_NPanel; i++)
		{
			m_TTwist[i] *= ratio;
		}
	}
	else
	{
		//Set each panel's twist in the ratio of the span position
		for(int i=1; i<=m_NPanel; i++)
		{
			m_TTwist[i] = Twist*m_TPos[i]/(m_Span/2.0);
		}
	}
	//ComputeGeometry();
}


