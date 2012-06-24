/****************************************************************************

	Wing Class
	Copyright (C) 2005-2010 Andre Deperrois adeperrois@xflr5.com

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

//
// This class :
//	- defines the wing object
//	- provides the methods for the calculation of the wing geometric properties
//	- provides methods for LLT, VLM and Panel methods
//


#include  <math.h>
#include "Wing.h"
#include "../mainframe.h"
#include "../globals.h"
#include "../miarex/Miarex.h"
#include "../miarex/LLTAnalysisDlg.h"



CVector *CWing::m_pWakeNode;			//pointer to the VLM wake node array
CPanel  *CWing::m_pWakePanel;			//pointer to the VLM Wake Panel array

void* CWing::s_pMainFrame;		//pointer to the Frame window
void* CWing::s_pMiarex;	//pointer to the Miarex Application window
void* CWing::s_p3DPanelDlg;//pointer to the 3DPanel analysis dialog class
bool CWing::s_bVLMSymetric;



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
//	memset(m_GeomMoment, 0, sizeof(m_GeomMoment));

	m_CoG.Set(0.0,0.0,0.0);
	m_CoGIxx = m_CoGIyy = m_CoGIzz = m_CoGIxz = 0.0;
	m_VolumeMass = m_TotalMass = 0.0;
	m_NMass = 0;
	memset(m_MassValue,    0, sizeof(m_MassValue));
	memset(m_MassPosition, 0, sizeof(m_MassPosition));
	for(int i=0; i< MAXMASSES; i++) m_MassTag[i] = QString(QObject::tr("Description %1")).arg(i);

	m_bIsFin        = false;
	m_bDoubleFin    = false;
	m_bSymFin       = false;
	m_bDoubleSymFin = false;
	m_bSymetric     = true;
	m_bWingOut      = false;
	s_bVLMSymetric  = false;

	m_WingName        = QObject::tr("Wing Name");
	m_WingDescription = "";
	m_WingColor.setHsv((int)(((double)qrand()/(double)RAND_MAX)*255), 31, 203, 140);

	m_QInf0    = 0.0;

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

	m_AVLIndex = -(int)(qrand()/10000);//improbable value...

	m_MatSize   = 0;
	m_NSurfaces = 0;

	m_AR         = 0.0;// Aspect ratio
	m_TR         = 0.0;// Taper ratio
	m_GChord     = 0.0;// mean geometric chord
	m_MAChord    = 0.0;// mean aero chord
	m_yMac       = 0.0;
	m_ProjectedArea = 0.0;
	m_ProjectedSpan = 0.0;

	m_NPanel        =  1;
	m_nFlaps        =  0;
	m_NYPanels[0]   = 19;
	m_NXPanels[0]   = 13;
	m_NXPanels[1]   = 13;
	m_YPanelDist[0] = 0;

	m_TChord[0]  =  .180;
	m_TChord[1]  =  .120;
	m_TLength[0] =  .0;
	m_TLength[1] = 1.0;
	m_TOffset[0] = 0.;
	m_TOffset[1] = 0.060;

	double length = m_TLength[0];
	for (i=0; i<=MAXSPANSECTIONS; i++)
	{
		length += m_TLength[i];
		m_TPos[i]     = length;
		m_XPanelDist[i] =  1;
	}


	for (i=0;i<=MAXSPANSECTIONS; i++)
	{
		m_RFoil.append(" ");
		m_LFoil.append(" ");
	}
}



void CWing::ComputeDihedrals()
{
	for (int n=0; n<m_NPanel; n++){
		m_TDihedral[n] = 180.0/PI* atan2(m_TZPos[n+1]-m_TZPos[n], m_TPos[n+1]-m_TPos[n]);
	}
}

void CWing::ComputeGeometry()
{
	// Computes the wing's characteristics from the panel data
	//
	MainFrame *pMainFrame  = (MainFrame*)s_pMainFrame;
	QMiarex    *pMiarex = (QMiarex*)   s_pMiarex;
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
	{
		m_TYProj[i] = m_TYProj[i-1] + m_TLength[i] * cos(m_TDihedral[i-1]*PI/180.0);
	}

	m_PlanformSpan  = 2.0 * m_TPos[m_NPanel];
	m_ProjectedSpan = 0.0;
	m_MAChord = 0.0;
	m_yMac    = 0.0;

	for (k=0; k<m_NPanel; k++)
	{
		pFoilA = pMainFrame->GetFoil(m_RFoil[k]);
		pFoilB = pMainFrame->GetFoil(m_RFoil[k+1]);
		surface   += m_TLength[k+1]*(m_TChord[k]+m_TChord[k+1])/2.0;//m2
		xysurface += (m_TLength[k+1]*(m_TChord[k]+m_TChord[k+1])/2.0)*cos(m_TDihedral[k]*PI/180.0);
		m_ProjectedSpan += m_TLength[k+1]*cos(m_TDihedral[k]*PI/180.0);

		m_MAChord += IntegralC2(m_TPos[k], m_TPos[k+1], m_TChord[k], m_TChord[k+1]);
		m_yMac    += IntegralCy(m_TPos[k], m_TPos[k+1], m_TChord[k], m_TChord[k+1]);
	}

	m_ProjectedSpan *=2.0;
	if(!m_bIsFin || m_bSymFin || m_bDoubleFin)
	{
		m_PlanformArea    = 2.0 * surface;
		m_ProjectedArea = 2.0 * xysurface;
		m_MAChord = m_MAChord * 2.0 / m_PlanformArea;
		m_yMac    = m_yMac    * 2.0 / m_PlanformArea;

		m_GChord  = m_PlanformArea/m_PlanformSpan;
		m_AR      = m_PlanformSpan*m_PlanformSpan/m_PlanformArea;
	}
	else
	{
		m_PlanformArea = surface;
		m_ProjectedArea = xysurface;
		m_MAChord = m_MAChord / m_PlanformArea;
		m_yMac    = m_yMac    / m_PlanformArea;

		m_GChord  = m_PlanformArea/m_PlanformSpan*2.0;
		m_AR      = m_PlanformSpan*m_PlanformSpan/m_PlanformArea/2.0;
	}
	if(m_TChord[m_NPanel]>0.0)	m_TR = m_TChord[0]/m_TChord[m_NPanel];
	else						m_TR = 99999.0;

	//calculate the number of flaps
	m_nFlaps = 0;
	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
	else                            MinPanelSize = m_PlanformSpan;

	for (i=1; i<=m_NPanel; i++)
	{
		pFoilA = pMainFrame->GetFoil(m_RFoil[i-1]);
		pFoilB = pMainFrame->GetFoil(m_RFoil[i]);
		if(pFoilA && pFoilB && (!m_bIsFin || (m_bIsFin && m_bSymFin) || (m_bIsFin && m_bDoubleFin)))
		{
			if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && fabs(m_TPos[i]-m_TPos[i-1])>MinPanelSize)	m_nFlaps++;
		}
		pFoilA = pMainFrame->GetFoil(m_LFoil[i-1]);
		pFoilB = pMainFrame->GetFoil(m_LFoil[i]);
		if(pFoilA && pFoilB)
		{
			if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && fabs(m_TPos[i]-m_TPos[i-1])>MinPanelSize)	m_nFlaps++;
		}
	}
}

#define NXSTATIONS 20
#define NYSTATIONS 40

void CWing::ComputeVolumeInertia(CVector &CoG, double &CoGIxx, double &CoGIyy, double &CoGIzz, double &CoGIxz)
{
	//
	// Returns the inertia properties of the structure based on the object's mass and on the existing geometry
	//   in input:
	//     Mass = mass of the wing, excluding point masses
	//   in output:
	//     CoG  = center of gravity position
	//     CoGIxx, CoGIyy, CoGIzz, CoGIxz = inertia of properties calculated at the CoG
	//
	double ElemVolume[NXSTATIONS*NYSTATIONS*MAXSPANSECTIONS*2];
	CVector PtVolume[NXSTATIONS*NYSTATIONS*MAXSPANSECTIONS*2];
	int j,k,l;
	double rho, LocalSpan, LocalVolume;
	double LocalChord,  LocalArea,  tau;
	double LocalChord1, LocalArea1, tau1;
	double xrel, xrel1, yrel, ElemArea;
	CVector ATop, ABot, CTop, CBot, PointNormal, Diag1, Diag2;
	CVector PtC4, Pt, Pt1;
	CoG.Set(0.0, 0.0, 0.0);
	CoGIxx = CoGIyy = CoGIzz = CoGIxz = 0.0;

	//sanity check
//	CVector CoGCheck(0.0,0.0,0.0);
	double CoGIxxCheck, CoGIyyCheck, CoGIzzCheck, CoGIxzCheck;
	CoGIxxCheck = CoGIyyCheck = CoGIzzCheck = CoGIxzCheck = 0.0;
	double recalcMass = 0.0;
	double recalcVolume = 0.0;
	double checkVolume = 0.0;

	//use 20 spanwise stations per wing panel to discretize the weight
	//and 10 stations in the x direction
	int NYStations = NYSTATIONS;
	int NXStations = NXSTATIONS;

	ComputeGeometry();

	//the mass density is assumed to be homogeneous

	//the local weight is proportional to the chord x foil area
	//the foil's area is interpolated

	//we consider the whole wing, i.e. all left and right surfaces
	//note : in avl documentation, each side is considered separately

	//first get the CoG - necessary for future application of Huygens/Steiner theorem
	int p = 0;

	for (j=0; j<m_NSurfaces; j++)
	{
		LocalSpan = m_Surface[j].m_Length/(double)NYStations;
		for (k=0; k<NYStations; k++)
		{
			tau  = (double)k     / (double)NYStations;
			tau1 = (double)(k+1) / (double)NYStations;
			yrel = (tau+tau1)/2.0;

			m_Surface[j].GetSection(tau,  LocalChord,  LocalArea,  Pt);
			m_Surface[j].GetSection(tau1, LocalChord1, LocalArea1, Pt1);
			LocalVolume = (LocalArea+LocalArea1)/2.0 * LocalSpan;
			PtC4.x = (Pt.x + Pt1.x)/2.0;
			PtC4.y = (Pt.y + Pt1.y)/2.0;
			PtC4.z = (Pt.z + Pt1.z)/2.0;

//			CoGCheck += LocalVolume * PtC4;
			for(l=0; l<NXStations; l++)
			{
				//browse mid-section
				xrel  = 1.0 - 1.0/2.0 * (1.0-cos((double) l   *PI /(double)NXStations));
				xrel1 = 1.0 - 1.0/2.0 * (1.0-cos((double)(l+1)*PI /(double)NXStations));

				m_Surface[j].GetPoint(xrel, xrel, yrel, ATop,  1);
				m_Surface[j].GetPoint(xrel, xrel, yrel, ABot, -1);
				m_Surface[j].GetPoint(xrel1, xrel1, yrel, CTop,  1);
				m_Surface[j].GetPoint(xrel1, xrel1, yrel, CBot, -1);
				PtVolume[p] = (ATop+ABot+CTop+CBot)/4.0;
				Diag1 = ATop - CBot;
				Diag2 = ABot - CTop;
				PointNormal = Diag1 * Diag2;

				ElemArea = PointNormal.VAbs()/2.0;
				if(ElemArea>PRECISION)	ElemVolume[p] = ElemArea * LocalSpan;
				else
				{
					//no area, means that the foils have not yet been defined for this surface
					// so just count a unit volume, temporary
					ElemVolume[p] = 1.0;

				}
				checkVolume += ElemVolume[p];
				CoG.x += ElemVolume[p] * PtVolume[p].x;
				CoG.y += ElemVolume[p] * PtVolume[p].y;
				CoG.z += ElemVolume[p] * PtVolume[p].z;
				p++;
			}
		}
	}

	if(checkVolume>PRECISION) rho = m_VolumeMass/checkVolume;
	else                      rho = 0.0;

	if(checkVolume>0.0)  CoG *= 1.0/ checkVolume;
	else                 CoG.Set(0.0, 0.0, 0.0);


	// CoG is the new origin for inertia calculation
	p=0;
	for (j=0; j<m_NSurfaces; j++)
	{
		LocalSpan = m_Surface[j].m_Length/(double)NYStations;
		for (k=0; k<NYStations; k++)
		{
			tau  = (double)k     / (double)NYStations;
			tau1 = (double)(k+1) / (double)NYStations;
			m_Surface[j].GetSection(tau,  LocalChord,  LocalArea,  Pt);
			m_Surface[j].GetSection(tau1, LocalChord1, LocalArea1, Pt1);

			LocalVolume = (LocalArea+LocalArea1)/2.0 * LocalSpan;

			PtC4.x = (Pt.x + Pt1.x)/2.0;
			PtC4.y = (Pt.y + Pt1.y)/2.0;
			PtC4.z = (Pt.z + Pt1.z)/2.0;

			CoGIxxCheck += LocalVolume*rho * ( (PtC4.y-CoG.y)*(PtC4.y-CoG.y) + (PtC4.z-CoG.z)*(PtC4.z-CoG.z) );
			CoGIyyCheck += LocalVolume*rho * ( (PtC4.x-CoG.x)*(PtC4.x-CoG.x) + (PtC4.z-CoG.z)*(PtC4.z-CoG.z) );
			CoGIzzCheck += LocalVolume*rho * ( (PtC4.x-CoG.x)*(PtC4.x-CoG.x) + (PtC4.y-CoG.y)*(PtC4.y-CoG.y) );
			CoGIxzCheck -= LocalVolume*rho * ( (PtC4.x-CoG.x)*(PtC4.z-CoG.z) );

			recalcMass   += LocalVolume*rho;
			recalcVolume += LocalVolume;

			for(l=0; l<NXStations; l++)
			{
				//browse mid-section
				CoGIxx += ElemVolume[p]*rho * ( (PtVolume[p].y-CoG.y)*(PtVolume[p].y-CoG.y) + (PtVolume[p].z-CoG.z)*(PtVolume[p].z-CoG.z));
				CoGIyy += ElemVolume[p]*rho * ( (PtVolume[p].x-CoG.x)*(PtVolume[p].x-CoG.x) + (PtVolume[p].z-CoG.z)*(PtVolume[p].z-CoG.z));
				CoGIzz += ElemVolume[p]*rho * ( (PtVolume[p].x-CoG.x)*(PtVolume[p].x-CoG.x) + (PtVolume[p].y-CoG.y)*(PtVolume[p].y-CoG.y));
				CoGIxz -= ElemVolume[p]*rho * ( (PtVolume[p].x-CoG.x)*(PtVolume[p].z-CoG.z) );
				p++;
			}
		}
	}
}


void CWing::ComputeBodyAxisInertia()
{
	//
	// Calculates the inertia tensor in geometrical (body) axis :
	//  - adds the volume inertia AND the inertia of point masses of all components
	//  - the body axis is the frame in which the geometry has been defined
	//  - the origin is the plane's CoG, taking into account all masses
	//

	int i;
	CVector LA, VolumeCoG;
	double Ixx, Iyy, Izz, Ixz, VolumeMass;
	Ixx = Iyy = Izz = Ixz = VolumeMass = 0.0;

	//Get the volume inertia properties in the volume CoG frame of reference
	ComputeVolumeInertia(VolumeCoG, Ixx, Iyy, Izz, Ixz);
	m_TotalMass = m_VolumeMass;

	m_CoG = VolumeCoG *m_VolumeMass;

	// add point masses
	for(i=0; i<m_NMass; i++)
	{
		m_TotalMass += m_MassValue[i];
		m_CoG       += m_MassPosition[i] * m_MassValue[i];
	}

	if(m_TotalMass>0.0) m_CoG = m_CoG/m_TotalMass;
	else                m_CoG.Set(0.0,0.0,0.0);

	// The CoG position is now available, so calculate the inertia w.r.t the CoG
	// using Huygens theorem
	//LA is the displacement vector from the centre of mass to the new axis
	LA = m_CoG-VolumeCoG;
	m_CoGIxx = Ixx + m_VolumeMass * (LA.y*LA.y+ LA.z*LA.z);
	m_CoGIyy = Iyy + m_VolumeMass * (LA.x*LA.x+ LA.z*LA.z);
	m_CoGIzz = Izz + m_VolumeMass * (LA.x*LA.x+ LA.y*LA.y);
	m_CoGIxz = Ixz - m_VolumeMass *  LA.x*LA.z;

	//add the contribution of point masses to total inertia
	for(i=0; i<m_NMass; i++)
	{
		LA = m_MassPosition[i] - m_CoG;
		m_CoGIxx += m_MassValue[i] * (LA.y*LA.y + LA.z*LA.z);
		m_CoGIyy += m_MassValue[i] * (LA.x*LA.x + LA.z*LA.z);
		m_CoGIzz += m_MassValue[i] * (LA.x*LA.x + LA.y*LA.y);
		m_CoGIxz -= m_MassValue[i] * (LA.x*LA.z);
	}
}



void CWing::CreateSurfaces(CVector const &T, double XTilt, double YTilt)
{
	//
	// Constructs the surface objects based on the input data
	// The surfaces are constructed from root to tip, and re-ordered from let tip to right tip
	// One surface object for each of the wing's panels
	//A is the surface's left side, B is the right side
	//

	int j, nSurf;
	CVector PLA, PTA, PLB, PTB, Offset, T1;
	CVector Trans(T.x, 0.0, T.z);
	CVector O(0.0,0.0,0.0);
	CVector VNormal[MAXSPANSECTIONS+1], VNSide[MAXSPANSECTIONS+1];
	double MinPanelSize;

	MainFrame *pMainFrame  = (MainFrame*)s_pMainFrame;
	QMiarex   *pMiarex     = (QMiarex*)s_pMiarex;

	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
	else                            MinPanelSize = 0.0;

	m_NSurfaces = 0;
	m_MatSize = 0;

	//define the normal of each surface
	nSurf=0;
	VNormal[0].Set(0.0, 0.0, 1.0);
	VNSide[0].Set(0.0, 0.0, 1.0);

	for(j=0; j<m_NPanel;j++)
	{
		if (fabs(m_TPos[j]-m_TPos[j+1]) > MinPanelSize)
		{
			VNormal[nSurf].Set(0.0, 0.0, 1.0);
			VNormal[nSurf].RotateX(O, m_TDihedral[j]);
			nSurf++;
		}
	}
	m_NSurfaces = nSurf;
	//define the normals at panel junctions : average between the normals of the two connecting sufaces
	for(j=0; j<nSurf;j++)
	{
		VNSide[j+1] = VNormal[j]+VNormal[j+1];
		VNSide[j+1].Normalize();
	}

	//we start with the center panel, moving towards the left wing tip
	//however, the calculations are written for surfaces ordered from left tip to right tip,
	//so we number them the opposite way
	nSurf = 0;
	int is = m_NSurfaces-1;
	for (j=0; j<m_NPanel; j++)
	{
		if (fabs(m_TPos[j]-m_TPos[j+1]) > MinPanelSize)
		{
			m_Surface[is].m_pFoilA   = pMainFrame->GetFoil(m_LFoil[j+1]);
			m_Surface[is].m_pFoilB   = pMainFrame->GetFoil(m_LFoil[j]);

			m_Surface[is].m_Length   =  m_TPos[j+1] - m_TPos[j];

			PLA.x =  m_TOffset[j+1];			PLB.x =  m_TOffset[j];
			PLA.y = -m_TPos[j+1];			PLB.y = -m_TPos[j];
			PLA.z =  0.0;					PLB.z =  0.0;
			PTA.x =  PLA.x+m_TChord[j+1];		PTB.x = PLB.x+m_TChord[j];
			PTA.y =  PLA.y;				PTB.y = PLB.y;
			PTA.z =  0.0;					PTB.z =  0.0;

			m_Surface[is].m_LA.Copy(PLA);
			m_Surface[is].m_TA.Copy(PTA);
			m_Surface[is].m_LB.Copy(PLB);
			m_Surface[is].m_TB.Copy(PTB);

			m_Surface[is].SetNormal(); // is (0,0,1)

			m_Surface[is].RotateX(m_Surface[is].m_LB, -m_TDihedral[j]);
			m_Surface[is].NormalA.Set(VNSide[nSurf+1].x,   -VNSide[nSurf+1].y,   VNSide[nSurf+1].z);
			m_Surface[is].NormalB.Set(VNSide[nSurf].x, -VNSide[nSurf].y, VNSide[nSurf].z);

			m_Surface[is].m_TwistA   =  m_TTwist[j+1];
			m_Surface[is].m_TwistB   =  m_TTwist[j];
			m_Surface[is].SetTwist2();


			if(j>0)
			{
				//translate the surface to the left tip of the previous surface and merge points
				T1 = m_Surface[is+1].m_LA - m_Surface[is].m_LB;
				m_Surface[is].Translate(T1);
				m_Surface[is].m_LB = m_Surface[is+1].m_LA;
				m_Surface[is].m_TB = m_Surface[is+1].m_TA;
			}
			else
			{
				//translate the surface by the initial offset
				m_Surface[is].Translate(CVector(0.0,-m_TOffset[0],0.0));
			}

			nSurf++;

			m_Surface[is].m_NXPanels = m_NXPanels[j];
			m_Surface[is].m_NYPanels = m_NYPanels[j];

			//AVL coding + invert sine and -sine for left wing
			m_Surface[is].m_XDistType = m_XPanelDist[j];

			if(m_YPanelDist[j] ==2)        m_Surface[is].m_YDistType = -2;
			else if(m_YPanelDist[j] ==  1) m_Surface[is].m_YDistType =  1;
			else if(m_YPanelDist[j] == -2) m_Surface[is].m_YDistType =  2;
			else                           m_Surface[is].m_YDistType =  0;

			m_Surface[is].CreateXPoints();
			m_Surface[is].SetFlap();
			m_Surface[is].Init();
			m_Surface[is].m_bIsLeftSurf   = true;
			m_Surface[is].m_bIsInSymPlane = false;
			--is;
		}
	}
	m_Surface[m_NSurfaces-1].m_bIsCenterSurf = true;//previous left center surface
	m_Surface[m_NSurfaces].m_bIsCenterSurf   = true;//next right center surface

	// we only need a right wing in the following cases
	//   - if it's an 'ordinary wing'
	//   - if it's a fin, symetrical about the fuselage x-axis
	//   - if it's a symetrical double fin
	if(!m_bIsFin || (m_bIsFin && m_bSymFin) || (m_bIsFin && m_bDoubleFin))
	{
		is = nSurf;
		for (j=0; j<m_NPanel; j++)
		{
			if (fabs(m_TPos[j]-m_TPos[j+1]) > MinPanelSize)
			{
				m_Surface[is].m_pFoilA   = pMainFrame->GetFoil(m_RFoil[j]);
				m_Surface[is].m_pFoilB   = pMainFrame->GetFoil(m_RFoil[j+1]);

				m_Surface[is].m_Length   =  m_TPos[j+1] - m_TPos[j];

				PLA.x = m_TOffset[j];		PLB.x = m_TOffset[j+1];
				PLA.y = m_TPos[j];			PLB.y = m_TPos[j+1];
				PLA.z = 0.0;				PLB.z = 0.0;
				PTA.x = PLA.x+m_TChord[j];	PTB.x = PLB.x+m_TChord[j+1];
				PTA.y = PLA.y;				PTB.y = PLB.y;
				PTA.z = 0.0;				PTB.z = 0.0;

				m_Surface[is].m_LA.Copy(PLA);
				m_Surface[is].m_TA.Copy(PTA);
				m_Surface[is].m_LB.Copy(PLB);
				m_Surface[is].m_TB.Copy(PTB);

				m_Surface[is].SetNormal(); // is (0,0,1)

				m_Surface[is].RotateX(m_Surface[is].m_LA, m_TDihedral[j]);
				m_Surface[is].NormalA.Set(VNSide[is-nSurf].x,   VNSide[is-nSurf].y,   VNSide[is-nSurf].z);
				m_Surface[is].NormalB.Set(VNSide[is-nSurf+1].x, VNSide[is-nSurf+1].y, VNSide[is-nSurf+1].z);

				m_Surface[is].m_TwistA   =  m_TTwist[j];
				m_Surface[is].m_TwistB   =  m_TTwist[j+1];
				m_Surface[is].SetTwist2();

				if(j>0)
				{
					//translate the surface to the left tip of the previous surface and merge points
					T1 = m_Surface[is-1].m_LB - m_Surface[is].m_LA ;
					m_Surface[is].Translate(T1);
					m_Surface[is].m_LA = m_Surface[is-1].m_LB;
					m_Surface[is].m_TA = m_Surface[is-1].m_TB;
				}
				else
				{
					//translate the surface by the initial offset
					m_Surface[is].Translate(CVector(0.0,m_TOffset[0],0.0));
				}

				m_Surface[is].m_NXPanels = m_NXPanels[j];
				m_Surface[is].m_NYPanels = m_NYPanels[j];

				//AVL coding + invert sine and -sine for left wing
				m_Surface[is].m_XDistType = m_XPanelDist[j];

				if(m_YPanelDist[j] ==2)        m_Surface[is].m_YDistType = 2;
				else if(m_YPanelDist[j] ==  1) m_Surface[is].m_YDistType =  1;
				else if(m_YPanelDist[j] == -2) m_Surface[is].m_YDistType = -2;
				else                           m_Surface[is].m_YDistType =  0;

				m_Surface[is].CreateXPoints();
				m_Surface[is].SetFlap();
				m_Surface[is].Init();
				m_Surface[is].m_bIsLeftSurf   = false;
				m_Surface[is].m_bIsRightSurf  = true;
				m_Surface[is].m_bIsInSymPlane = false;
				is++;
			}
		}
	}
	CVector Or(0.0,0.0,0.0);
	if(!m_bIsFin || (m_bIsFin && m_bSymFin))
	{
		m_NSurfaces*=2;
		for (j=0; j<m_NSurfaces; j++)
		{
			m_Surface[j].RotateX(Or, XTilt);
			m_Surface[j].RotateY(Or, YTilt);
			m_Surface[j].Translate(Trans);
			if(m_bIsFin && m_bSymFin)
			{
				m_Surface[j].m_bIsInSymPlane = true;
				m_Surface[j].m_bIsLeftSurf   = true;
				m_Surface[j].m_bIsRightSurf  = false;
			}
		}
	}
	else
	{
		if(m_bDoubleFin)
		{
			m_NSurfaces*=2;
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
}



void CWing::ComputeChords(int NStation)
{
	//
	// Calculates the chord lengths at each position of the NStation in LLT calculations
	//
	int j,k,l,m;
	double y, yob, tau;
	double x0,y0,y1,y2;
	double SpanPosition[MAXSPANSTATIONS];
	CVector C;

	if(NStation !=0)
	{//LLT based
		m_NStation = NStation;

		for (k=0; k<=NStation; k++)
		{
			yob   = cos(k*PI/NStation);
			y = fabs(yob * m_PlanformSpan/2);
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
		for (m=0; m<m_NStation/2; m++)
		{
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



void CWing::ComputeChords(int NStation, double *Chord, double *Offset, double *Twist)
{
	//
	// Calculates the chord lengths at each position of the NStation in LLT calculations
	//
	int j,k,l,m;
	double y, yob, tau;
	double x0,y0,y1,y2;
	double SpanPosition[MAXSPANSTATIONS];
	CVector C;

	if(NStation !=0)
	{//LLT based
		m_NStation = NStation;

		for (k=0; k<=NStation; k++)
		{
			yob   = cos(k*PI/NStation);
			y = fabs(yob * m_PlanformSpan/2);
			for (l=0; l<=m_NPanel; l++)
			{
				if(m_TPos[l] < y && y <=m_TPos[l+1])
				{
					tau = (y-m_TPos[l])/(m_TPos[l+1]-m_TPos[l]);
					Chord[k]  = m_TChord[l]+(m_TChord[l+1]-m_TChord[l]) * tau;
					Offset[k] = m_TOffset[l]+(m_TOffset[l+1]-m_TOffset[l]) * tau;
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
		for (m=0; m<m_NStation/2; m++)
		{
			m_SpanPos[m] = -SpanPosition[m_NStation/2-m-1];
			m_SpanPos[m+m_NStation/2] = SpanPosition[m];
		}

		m=0;
		for (j=0; j<m_NSurfaces; j++)
		{
			for (k=0; k<m_Surface[j].m_NYPanels; k++)
			{
				//calculate chords and offsets at each station
				Chord[m]     = m_Surface[j].GetChord(k);
//				m_StripArea[m] = m_Chord[m]* m_Surface[j].Getdl(k);

				m_Surface[j].GetLeadingPt(k, C);
				Offset[m] = C.x-x0;

				Twist[m]  = m_Surface[j].GetTwist(k);
				m++;
			}
		}
		m_NStation = m;
	}
}



void CWing::Duplicate(CWing *pWing)
{
	//
	// Copies the wing data from an existing wing
	//
	int i;
	s_pMainFrame    = pWing->s_pMainFrame;
	s_pMiarex       = pWing->s_pMiarex;
	m_NStation      = pWing->m_NStation;
	m_NPanel        = pWing->m_NPanel;
	m_PlanformSpan  = pWing->m_PlanformSpan;
	m_ProjectedSpan = pWing->m_ProjectedSpan;
	m_PlanformArea  = pWing->m_PlanformArea;
	m_ProjectedArea = pWing->m_ProjectedArea;
	m_AR			 = pWing->m_AR;
	m_TR			 = pWing->m_TR;
	m_GChord		 = pWing->m_GChord;
	m_MAChord		 = pWing->m_MAChord;
	m_WingName      = pWing->m_WingName;
	m_bSymetric     = pWing->m_bSymetric;
	m_bIsFin        = pWing->m_bIsFin;
	m_bSymFin       = pWing->m_bSymFin;
	m_bDoubleFin    = pWing->m_bDoubleFin;
	m_bDoubleSymFin = pWing->m_bDoubleSymFin;

	for (i=0; i<=MAXSPANSECTIONS; i++)
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

	m_VolumeMass = pWing->m_VolumeMass;
	m_TotalMass  = pWing->m_TotalMass;
	m_CoG = pWing->m_CoG;
	m_CoGIxx = pWing->m_CoGIxx;
	m_CoGIyy = pWing->m_CoGIyy;
	m_CoGIzz = pWing->m_CoGIzz;
	m_CoGIxz = pWing->m_CoGIxz;
	m_NMass = pWing->m_NMass;
	for(i=0; i<m_NMass;i++)
	{
		m_MassValue[i] = pWing->m_MassValue[i];
		m_MassPosition[i].Copy(pWing->m_MassPosition[i]);
		m_MassTag[i] = pWing->m_MassTag[i];
	}

	m_WingDescription = pWing->m_WingDescription;
	m_WingColor       = pWing->m_WingColor;
}




bool CWing::ExportAVLWing(QTextStream &out, int index, double x, double y, double z, double Thetax, double Thetay)
{
	// Exports the current wing to AVL format

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	int j;
	QString strong, str;

	out << ("#=================================================\n");
	out << ("SURFACE                      | (keyword)\n");
	out << (m_WingName);
	out << ("\n");
	out << ("#Nchord    Cspace   [ Nspan Sspace ]\n");

	strong = QString("%1        %2\n").arg( m_NXPanels[0]).arg(1.0,3,'f',1);
	out << (strong);

	out << ("\n");
	out << ("INDEX                        | (keyword)\n");
	strong = QString("%1                          | Lsurf\n").arg(index,3);
	out << (strong);

	if(!m_bIsFin)
	{
		out << ("\n");
		out << ("YDUPLICATE\n");
		out << ("0.0\n");
	}
	else if(m_bDoubleFin)
	{
		out << ("\n");
		out << ("YDUPLICATE\n");
		strong = QString("%1\n").arg(y,9,'f',4);
		out << (strong);
	}

	out << ("\n");
	out << ("SCALE\n");
	out << ("1.0  1.0  1.0\n");

	out << ("\n");
	out << ("TRANSLATE\n");
	out << ("0.0  0.0  0.0\n");

	out << ("\n");
	out << ("ANGLE\n");
	strong = QString("%1                         | dAinc\n").arg(Thetay,8,'f',3);
	out << (strong);

	out << ("\n\n");

	//write only right wing surfaces since we have provided YDUPLICATE
	CSurface ASurface;
	int iFlap = 1;
//	int iSurface = 0;

        int startIndex = (m_bIsFin ? 0 : (int)(m_NSurfaces/2));

        for(j=startIndex; j<m_NSurfaces; j++)
	{
		ASurface.Copy(m_Surface[j]);

		//Remove the twist, since AVL processes it as a mod of the angle of attack thru the dAInc command
		ASurface.m_TwistA = ASurface.m_TwistB = 0.0;
		ASurface.SetTwist1();
		out << ("#______________\nSECTION                                                     |  (keyword)\n");

		strong = QString("%1 %2 %3 %4 %5  %6  %7   | Xle Yle Zle   Chord Ainc   [ Nspan Sspace ]\n")
				 .arg(ASurface.m_LA.x          *pMainFrame->m_mtoUnit,9,'f',4)
				 .arg(ASurface.m_LA.y          *pMainFrame->m_mtoUnit,9,'f',4)
				 .arg(ASurface.m_LA.z          *pMainFrame->m_mtoUnit,9,'f',4)
				 .arg(ASurface.GetChord(0.0)   *pMainFrame->m_mtoUnit,9,'f',4)
				 .arg(m_Surface[j].m_TwistA,7,'f',3)
				 .arg(ASurface.m_NYPanels,3)
				 .arg(ASurface.m_YDistType,3);
		out << (strong);
		out << ("\n");
		out << ("AFIL 0.0 1.0\n");
		out << (ASurface.m_pFoilA->m_FoilName +".dat\n");
		out << ("\n");
		if(ASurface.m_bTEFlap)
		{
			out << ("CONTROL                                                     |  (keyword)\n");
			str = QString("_Flap_%1  ").arg(iFlap);
			strong = m_WingName;
			strong.replace(" ", "_");
			strong += str;
			double mean_angle = (ASurface.m_pFoilA->m_TEFlapAngle + ASurface.m_pFoilB->m_TEFlapAngle)/2.0;
			if(fabs(mean_angle)>0.0) str = QString("%1  ").arg(1.0/mean_angle,5,'f',2);
			else                     str = "1.0   ";
			strong += str;
			str = QString("%1  %2  %3  %4  -1.0  ")
				  .arg(ASurface.m_pFoilA->m_TEXHinge/100.0,5,'f',3)
				  .arg(ASurface.m_HingeVector.x,10,'f',4)
				  .arg(ASurface.m_HingeVector.y,10,'f',4)
				  .arg(ASurface.m_HingeVector.z,10,'f',4);
			strong +=str + "| name, gain,  Xhinge,  XYZhvec,  SgnDup\n";
			out << (strong);

			//write the same flap at the other end
/*			out << ("\n#______________\nSECTION                                                     |  (keyword)\n");

			strong = QString("%1 %2 %3 %4 %5  %6  %7   | Xle Yle Zle   Chord Ainc   [ Nspan Sspace ]\n")
				 .arg(ASurface.m_LB.x          *pMainFrame->m_mtoUnit,9,'f',4)
				 .arg(ASurface.m_LB.y          *pMainFrame->m_mtoUnit,9,'f',4)
				 .arg(ASurface.m_LB.z          *pMainFrame->m_mtoUnit,9,'f',4)
				 .arg(ASurface.GetChord(1.0)   *pMainFrame->m_mtoUnit,9,'f',4)
				 .arg(m_Surface[j].m_TwistB,7,'f',3)
				 .arg(ASurface.m_NYPanels,3)
				 .arg(ASurface.m_YDistType,3);
			out << (strong);

			out << ("\n");
			out << ("AFIL 0.0 1.0\n");
			out << (ASurface.m_pFoilB->m_FoilName +".dat\n");
			out << ("\n");

			out << ("CONTROL                                                     |  (keyword)\n");
			str = QString("_Flap_%1  ").arg(iFlap);
			strong = m_WingName;
			strong.replace(" ", "_");
			strong += str;

			if(fabs(mean_angle)>0.0) str = QString("%1  ").arg(1.0/mean_angle,5,'f',2);
			else                     str = "1.0   ";
			strong += str;

			str = QString("%1  %2  %3  %4  -1.0  ")
				  .arg(ASurface.m_pFoilB->m_TEXHinge/100.0,5,'f',3)
				  .arg(ASurface.m_HingeVector.x,10,'f',4)
				  .arg(ASurface.m_HingeVector.y,10,'f',4)
				  .arg(ASurface.m_HingeVector.z,10,'f',4);
			strong +=str + "| name, gain,  Xhinge,  XYZhvec,  SgnDup\n\n";
			out << (strong);
*/
			iFlap++;
		}
	}
	//write last panel, if no flap before
	if(!ASurface.m_bTEFlap)
	{
		out << ("#______________\nSECTION                                                     |  (keyword)\n");
		strong = QString("%1 %2 %3 %4 %5  %6  %7   | Xle Yle Zle   Chord Ainc   [ Nspan Sspace ]\n")
				 .arg(ASurface.m_LB.x          *pMainFrame->m_mtoUnit,9,'f',4)
				 .arg(ASurface.m_LB.y          *pMainFrame->m_mtoUnit,9,'f',4)
				 .arg(ASurface.m_LB.z          *pMainFrame->m_mtoUnit,9,'f',4)
				 .arg(ASurface.GetChord(1.0)   *pMainFrame->m_mtoUnit,9,'f',4)
				 .arg(m_Surface[j-1].m_TwistB,7,'f',3)
				 .arg(ASurface.m_NYPanels,3)
				 .arg(ASurface.m_YDistType,3);

		out << (strong);
		out << ("\n");
		out << ("AFIL 0.0 1.0\n");
		out << (ASurface.m_pFoilB->m_FoilName +".dat\n");
		out << ("\n");
	}

	out << ("\n\n");

	return true;
}


double CWing::AverageSweep()
{
	//returns the wing's average sweep from root to tip
	//measured at the quarter chord
	double xroot = m_TChord[0]/4.0;
	double xtip  = m_TOffset[m_NPanel] + m_TChord[m_NPanel]/4.0;
//	double sweep = (atan2(xtip-xroot, m_PlanformSpan/2.0)) * 180.0/PI;
	return (atan2(xtip-xroot, m_PlanformSpan/2.0)) * 180.0/PI;
}


double CWing::C4(double yob, double xRef)
{
	//returns the quarter-chord point xposition relative to XCmRef
	double Chord, Offset, tau;
	double C4 = 0.0;
	double y = fabs(yob*m_PlanformSpan/2.0);
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


double CWing::Chord(double yob)
{
	double Chord = 0.0;
	double tau;
	double y;

	y= fabs(yob*m_PlanformSpan/2.0);//geometry is symetric
	for(int i=0; i<m_NPanel; i++){
		if(m_TPos[i]<=y && y <=m_TPos[i+1]){
			tau = (y - m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			Chord = m_TChord[i] + tau * (m_TChord[i+1] - m_TChord[i]);
			return Chord;
		}
	}
	return -1.0;
}


double CWing::Dihedral(double yob)
{
	int i;
	double y;

	y= fabs(yob*m_PlanformSpan/2.0);//geometry is symetric
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
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if  (y>0.0)
	{
		//search Right wing
		for (i=0; i<m_NPanel; i++)
		{
			if (m_TPos[i]<=y && y<=m_TPos[i+1])
			{

				*pFoil0 = pMainFrame->GetFoil(m_RFoil[i]);
				*pFoil1 = pMainFrame->GetFoil(m_RFoil[i+1]);
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
				*pFoil0 = pMainFrame->GetFoil(m_LFoil[i]);
				*pFoil1 = pMainFrame->GetFoil(m_LFoil[i+1]);
				t = (y-m_TPos[i])/(m_TPos[i+1] - m_TPos[i]);
				return;
			}
		}
	}
	t = 0;
	pFoil0 = NULL;// use linear
	pFoil1 = NULL;// use linear
}





double CWing::TotalMass()
{
	double TotalMass = m_VolumeMass;
	for(int i=0; i<m_NMass; i++)
		TotalMass += m_MassValue[i];
	return TotalMass;
}




double CWing::Offset(double yob)
{
	double tau, y;
	double Offset = 0.0;

	y= fabs(yob*m_PlanformSpan/2.0);
	for(int i=0; i<m_NPanel; i++){
		if(m_TPos[i]<= y && y <=m_TPos[i+1]){
			tau = (y - m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			Offset = m_TOffset[i] + tau * (m_TOffset[i+1] - m_TOffset[i]);
			return Offset;
		}
	}

	return -1.0;
}



double CWing::Twist(double y)
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
	//returns the wing's absolute positions yv and zv from the relative value xrel and the planform span y
	double tau;
	double twist, chord;
	double z0, z1, nx, ny;
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
				yv += m_TLength[k+1] * cos(m_TDihedral[k]*PI/180.0);
				zv += m_TLength[k+1] * sin(m_TDihedral[k]*PI/180.0);
			}
			tau = (fy - m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			yv += tau * m_TLength[i+1] * cos(m_TDihedral[i]*PI/180.0);
			zv += tau * m_TLength[i+1] * sin(m_TDihedral[i]*PI/180.0);

			yv *= sign;
			//	add washout calculated about chord quarter line :
			twist = Twist(fy)*PI/180.;
			chord = Chord(fy*2./m_PlanformSpan);
			zv -= (xrel-0.25)*chord*sin(twist);

			CFoil *pFoil0 = NULL;
			CFoil *pFoil1 = NULL;
			GetFoils(&pFoil0, &pFoil1, y,  tau);

			if(!pFoil0 || !pFoil1) return;

			if(pos==1)
			{
				pFoil0->GetUpperY(xrel, z0, nx, ny) ;
				pFoil1->GetUpperY(xrel, z1, nx, ny) ;
				z0 *= chord;
				z1 *= chord;
				zv += z0 + (z1-z0)*tau;
			}
			else if(pos==-1)
			{
				pFoil0->GetLowerY(xrel, z0, nx, ny);
				pFoil1->GetLowerY(xrel, z1, nx, ny);
				z0 *= chord;
				z1 *= chord;
				zv += z0 + (z1-z0)*tau;
			}
		}
	}
}


double CWing::yrel(double SpanPos)
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


double CWing::ZPos(double y)
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
				ZPos+=m_TLength[k+1] * sin(m_TDihedral[k]*PI/180.0);
			}
			tau = (y - m_TPos[i])/(m_TPos[i+1]-m_TPos[i]);
			ZPos += tau * m_TLength[i+1] * sin(m_TDihedral[i]*PI/180.0);
			return ZPos;
		}
	}
	return 0.0;
}




void CWing::PanelTrefftz(double QInf, double Alpha, double *Mu, double *Sigma, int pos, CVector &Force, double &WingIDrag,
						 CWPolar *pWPolar, CPanel *pWakePanel, CVector *pWakeNode)
{
	// calculates the induced lift and drag from the vortices or wake panels strength
	// using a farfield method
	// Downwash is evaluated at a distance 100 times the span downstream (i.e. infinite)

	PanelAnalysisDlg *pPanelDlg = (PanelAnalysisDlg*)s_p3DPanelDlg;

	int nw, iTA, iTB;
	int j, k, l, p, pp, m, mm;
	double InducedAngle, IYm, cosa, sina;
	double GammaStrip[MAXSPANSTATIONS];
	CVector C, Wg, dF, StripForce, WindDirection, WindNormal, VInf;

/*	if(pWPolar->m_bTiltedGeom)
	{
		cosa = 1.0;
		sina = 0.0;
	}
	else
	{*/
		cosa = cos(Alpha*PI/180.0);
		sina = sin(Alpha*PI/180.0);
//	}

	//   Define wind axis
	WindNormal.Set(   -sina, 0.0, cosa);
	WindDirection.Set( cosa, 0.0, sina);

	VInf = WindDirection * QInf;

	//dynamic pressure, kg/m3
	double q = 0.5 * pWPolar->m_Density * QInf * QInf;

	m_CL      = 0.0;
	WingIDrag = 0.0;
	IYm       = 0.0;

	int coef = 2;
	if (pWPolar->m_bThinSurfaces) coef = 1;

	int NSurfaces = m_NSurfaces;
	if(s_bVLMSymetric)
	{
		if(!m_bIsFin || (m_bIsFin && m_bDoubleFin))		NSurfaces=m_NSurfaces/2;
	}

	p=0;
	m=0;
	for (j=0; j<NSurfaces; j++)
	{
		if(m_Surface[j].m_bIsTipLeft && !pWPolar->m_bThinSurfaces) p+=m_Surface[j].m_NXPanels;//tip patch panels

		for (k=0; k<m_Surface[j].m_NYPanels; k++)
		{
			pp = p;
			m_StripArea[m] = 0.0;
			StripForce.Set(0.0,0.0,0.0);
			for (l=0; l<coef*m_Surface[j].m_NXPanels; l++)
			{
				m_StripArea[m]  += m_pPanel[pp].Area;
				pp++;
			}
			m_StripArea[m] /= (double)coef;

			if(!pWPolar->m_bThinSurfaces)
			{
				// ____________________________
				// Downwash calculation
				//
				// Since we place the trailing point at the end of the wake panels, it sees only the effect
				// of the upstream part of the wake because the downstream part isn't modelled.
				// If we were to model the downstream part, the total induced speed would be twice larger,
				// so just add a factor 2 to account for this.
				nw  = m_pPanel[p].m_iWake;
				iTA = pWakePanel[nw].m_iTA;
				iTB = pWakePanel[nw].m_iTB;
				C = (pWakeNode[iTA] + pWakeNode[iTB])/2.0;

				pPanelDlg->GetSpeedVector(C, Mu, Sigma, Wg, false);
				m_Vd[m] = Wg;
				InducedAngle = atan2(Wg.dot(WindNormal), QInf);
				m_Ai[m]      =       InducedAngle*180/PI;

				// ____________________________
				// Lift calculation
				//
				// Method 1 : Sum panel pressure forces over the top and bottom strip.
				// The induced drag is calculated by projection of the strip force on the wind direction
				// General experience in published literature shows that this isn't such a good idea

				// Method 2 : Far-field plane integration
				// This is the method generally recommended
				GammaStrip[m] = (-Mu[pos+p+coef*m_Surface[j].m_NXPanels-1] + Mu[pos+p]) *4.0*PI;
				Wg += VInf;

				StripForce  = m_pPanel[p].Vortex * Wg;
				StripForce *= GammaStrip[m] * pWPolar->m_Density / q;  // N/q

				//____________________________
				// Project on wind axes
				m_Cl[m]    = StripForce.dot(WindNormal)   /m_StripArea[m];
				m_ICd[m]   = StripForce.dot(WindDirection)/m_StripArea[m];
				WingIDrag += StripForce.dot(WindDirection);          // N/q
			}
			else
			{
				pp=p;
				for(l=0; l<m_Surface[j].m_NXPanels; l++)
				{
					if(pWPolar->m_bVLM1 || m_pPanel[pp].m_bIsTrailing)
					{
						C = m_pPanel[pp].CtrlPt;
						C.x = m_PlanformSpan * 1000.0;

						pPanelDlg->GetSpeedVector(C, Mu, Sigma, Wg, false);

						if(m_pPanel[pp].m_bIsTrailing)
						{
							m_Vd[m]      = Wg;
							InducedAngle = atan2(Wg.dot(WindNormal), QInf);
							m_Ai[m]      = InducedAngle*180/PI;
						}

						Wg += VInf; //total speed vector

						//induced force
						dF  = Wg * m_pPanel[pp].Vortex;
						dF *= Mu[pp+pos];       // N/rho
						StripForce += dF;       // N/rho
					}

					pp++;
				}
				StripForce *= 2./QInf/QInf; //N/q

				//____________________________
				// Project on wind axes
				m_Cl[m]    = StripForce.dot(WindNormal)   /m_StripArea[m];
				m_ICd[m]   = StripForce.dot(WindDirection)/m_StripArea[m]/2.;
				m_CL      += StripForce.dot(WindNormal);                // N/q
				WingIDrag += StripForce.dot(WindDirection)/2.;          // N/q
			}
			p  += coef*m_Surface[j].m_NXPanels;

			// Calculate resulting vector force
			Force     += StripForce;                          // N/q
			m_F[m]     = StripForce * q;	                    // Newtons

//			if(pWPolar->m_bTiltedGeom) m_F[m].RotateY(-Alpha);

			if(s_bVLMSymetric)
			{
				if(!m_bIsFin || (m_bIsFin && m_bDoubleFin))
				{
					//construct right side results
					mm = m_NStation-m-1;
					m_StripArea[mm]  = m_StripArea[m];
					m_Ai[mm]  = m_Ai[m];
					m_Cl[mm]  = m_Cl[m];
					m_ICd[mm] = m_ICd[m];

					m_F[mm].x  =  m_F[m].x;
					m_F[mm].y  = -m_F[m].y;
					m_F[mm].z  =  m_F[m].z;
					m_Vd[mm].x =  m_Vd[m].x;
					m_Vd[mm].y = -m_Vd[m].y;
					m_Vd[mm].z =  m_Vd[m].z;
					StripForce.y = -StripForce.y;

					// add to global force
					Force += StripForce;
					m_CL      += StripForce.dot(WindNormal);                // N/q
					WingIDrag += StripForce.dot(WindDirection)/(3-coef);          // N/q
				}
			}
			m++;
		}
//		if(s_bVLMSymetric) p+=m_Surface[j].m_NXPanels * m_Surface[j].m_NYPanels;
		if(m_Surface[j].m_bIsTipRight && !pWPolar->m_bThinSurfaces) p+=m_Surface[j].m_NXPanels;//tip patch panels
	}

	m_InducedDrag = WingIDrag; // save this wing's induced drag (unused though...)
}



void CWing::PanelSetBending(bool bThinSurface)
{
	double ypos[MAXSPANSTATIONS+1], zpos[MAXSPANSTATIONS+1];
	int j,k,jj,coef,p;
	double bm;
	CVector Dist(0.0,0.0,0.0);
	CVector Moment;
	int m =0;


	if(bThinSurface)
	{
		coef = 1;
		p    = 0;
	}
	else
	{
		coef = 2;
		p= m_Surface[0].m_NXPanels;
	}

	for (j=0; j<m_NSurfaces; j++)
	{
		for (k=0; k<m_Surface[j].m_NYPanels; k++)
		{
			if(!bThinSurface)
			{
				ypos[m] = m_pPanel[p].CollPt.y;
				zpos[m] = m_pPanel[p].CollPt.z;
 			}
			else
			{
				ypos[m] = m_pPanel[p].VortexPos.y;
				zpos[m] = m_pPanel[p].Vortex.z;
 			}

			p += coef*m_Surface[j].m_NXPanels;
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


void CWing::ScaleChord(double NewChord)
{
	// Scales the wing chord-wise so that the root chord is set to the NewChord value

	double ratio = NewChord/m_TChord[0];
	for (int i=0; i<=MAXSPANSECTIONS; i++)
	{
		m_TChord[i]    *= ratio;
		m_TOffset[i]   *= ratio;
	}
	ComputeGeometry();
}


void CWing::ScaleSpan(double NewSpan)
{
	// Scales the wing span-wise to the NewSpan value

	for (int i=0; i<=MAXSPANSECTIONS; i++)
	{
		m_TPos[i]      *= NewSpan/m_PlanformSpan;
		m_TLength[i]   *= NewSpan/m_PlanformSpan;
	}
	ComputeGeometry();
}


bool CWing::SerializeWing(QDataStream &ar, bool bIsStoring, int ProjectFormat)
{
	//saves or loads the wing data to the archive ar

	int i;
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int ArchiveFormat;// identifies the format of the file

	if(bIsStoring)
	{	// storing code
		if(ProjectFormat>5)       ar << 1010;
		else if(ProjectFormat==5) ar << 1009;
			//1010 : added storage of alpha channel + added a provision for ints and floats
			//1009 : QFLR5 v0.03 : added mass properties for inertia calculations
			//1008 : QFLR5 v0.02 : Added wing description field
			//1007 : Changed length units to m
			//1006 : Added Wing Color v2.99-15
			//1005 : Added Chordwise spacing (v2.99-00)
			//1004 : corrected m_NXPanels, m_NYPanels, m_YPanelDist to int (v1.99-33)
			//1003 : AVL Format (v1.99-18)
			//1002 : save VLM Mesh (v1.99-12)
			//1001 : initial format
		WriteCString(ar, m_WingName);
		if(ProjectFormat>=5) WriteCString(ar, m_WingDescription);

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

//		if(m_bVLMAutoMesh) ar << 1; else ar<<0;
		ar<<0;
		for (i=0; i<=m_NPanel; i++) ar << m_NXPanels[i];
		for (i=0; i<=m_NPanel; i++) ar << m_NYPanels[i];
		for (i=0; i<=m_NPanel; i++) ar << m_XPanelDist[i];//1005
		for (i=0; i<=m_NPanel; i++) ar << m_YPanelDist[i];

		WriteCOLORREF(ar,m_WingColor);

		if(ProjectFormat>=5)
		{
			ar << (float)m_VolumeMass;
			ar << m_NMass;
			for(i=0; i<m_NMass; i++) ar << (float)m_MassValue[i];
			for(i=0; i<m_NMass; i++) ar << (float)m_MassPosition[i].x << (float)m_MassPosition[i].y << (float)m_MassPosition[i].z;
			for(i=0; i<m_NMass; i++)  WriteCString(ar, m_MassTag[i]);
		}
		if(ProjectFormat>5)
		{
			ar << m_WingColor.alpha();
			for(int i=0; i<20; i++) ar<<(float)0.0f;
			for(int i=0; i<20; i++) ar<<0;
		}
		return true;
	}
	else
	{
		// loading code
		float f,g,h;
		int k;
		ar >> ArchiveFormat;

		if (ArchiveFormat <1001 || ArchiveFormat>1100) {
			m_WingName = "";
			return false;
		}

		ReadCString(ar,m_WingName);
		if (m_WingName.length() ==0) return false;

		if (ArchiveFormat >=1008)
		{
			ReadCString(ar, m_WingDescription);
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
//		m_bVLMSymetric = m_bSymetric;


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
			if (fabs(m_TPos[i]) <0.0)
			{
				m_WingName = "";
				return false;
			}
		}
		for (i=0; i<=m_NPanel; i++)
		{
			ar >> f; m_TOffset[i]=f;
		}

		if(ArchiveFormat<1007)
		{
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


		ar >> k;
//		if (k==1)      m_bVLMAutoMesh = true;
//		else if (k==0) m_bVLMAutoMesh = false;
//		else{
//			m_WingName = "";
//			return false;
//		}

		for (i=0; i<=m_NPanel; i++)
		{
			if(ArchiveFormat<=1003)
			{
				ar >> f;
				m_NXPanels[i] = (int)f;
			}
			else
				ar >> m_NXPanels[i];
//			m_NXPanels[i] = qMax(1,m_NXPanels[i] );
//			m_NXPanels[i] = qMin(MAXCHORDPANELS, m_NXPanels[i]);
		}

		for (i=0; i<=m_NPanel; i++)
		{
			if(ArchiveFormat<=1003)
			{
				ar >> f;
				m_NYPanels[i] = (int)f;
			}
			else 	ar >> m_NYPanels[i];
//			m_NYPanels[i] = qMax(1,m_NYPanels[i] );
//			m_NYPanels[i] = qMin(100, m_NYPanels[i]);
		}
		int total = 0;
		for (i=0; i<m_NPanel; i++)
		{
			total += m_NYPanels[i];
		}

		if(total*2>=MAXSPANSTATIONS)
		{
			double ratio = MAXSPANSTATIONS/total/2.0;
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

		if(ArchiveFormat>=1009)
		{
			ar >> f;  m_VolumeMass = f;
			ar >> m_NMass;
			for(i=0; i<m_NMass; i++)
			{
				ar >> f;
				m_MassValue[i] = f;
			}
			for(i=0; i<m_NMass; i++)
			{
				ar >> f >> g >> h;
				m_MassPosition[i].x = f;
				m_MassPosition[i].y = g;
				m_MassPosition[i].z = h;
			}
			for(i=0; i<m_NMass; i++) ReadCString(ar, m_MassTag[i]);
		}

		if(ArchiveFormat>=1010)
		{
			ar >> k; m_WingColor.setAlpha(k);
			for(int i=0; i<20; i++) ar>>f;
			for(int i=0; i<20; i++) ar>>k;
		}

		ComputeGeometry();
		return true;
	}
}



void CWing::ScaleSweep(double Sweep)
{
	//
	// Sets the average sweep measured at the quarter-chord to the new value Sweep
	//
	int i;
	double OldTipOffset = m_TOffset[m_NPanel];
	double NewTipOffset = m_TChord[0]/4.0
						 + tan(Sweep*PI/180.0)*m_PlanformSpan/2.0
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
			m_TOffset[i] = NewTipOffset*m_TPos[i]/(m_PlanformSpan/2.0);
		}
	}
	ComputeGeometry();
}


void CWing::ScaleTwist(double Twist)
{
	//
	// Scales the twist to the new value
	//
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
		for(int i=1; i<=m_NPanel; i++){
			m_TTwist[i] = Twist*m_TPos[i]/(m_PlanformSpan/2.0);
		}
	}
	ComputeGeometry();
}




int CWing::VLMGetPanelTotal()
{
	QMiarex *pMiarex = (QMiarex*) s_pMiarex;
	double MinPanelSize;

	if(pMiarex->m_MinPanelSize>0.0) MinPanelSize = pMiarex->m_MinPanelSize;
	else                            MinPanelSize = m_PlanformSpan/1000.0;
	int total = 0;
	for (int i=0; i<m_NPanel; i++)
	{
		//do not create a surface if its length is less than the critical size
		if (fabs(m_TPos[i]-m_TPos[i+1]) > MinPanelSize)	total +=m_NXPanels[i]*m_NYPanels[i];
	}
//	if(!m_bMiddle) total *=2;
	if(!m_bIsFin) return total*2;
	else          return total;
}


void CWing::PanelComputeOnBody(double QInf, double Alpha, double *Cp, double *Gamma, double &XCP, double &YCP,
						 double &GCm, double &VCm, double &ICm, double &GRm, double &GYm, double &VYm,double &IYm,
						 CWPolar *pWPolar, CVector CoG)

{
	//  Calculates the wing aero coefficients
	//  Uses Cp distribution in input for thick surfaces
	//  Uses Gamma distribution in input for VLM method
	//
	//  Input data:
	//    Freestream speed Qinf
	//    Angle of attack Alpha
	//    Cp dstribution for thick wings
	//    Mu or Gamma distribution, depending on the analysis type
	//    Type of surface :
	//        - Thin Surface, i.e. VLM type surfaces, with vortex distribution
	//        - Thick Surfaces;, i.e. 3D Panels with source+doublet distribution on panels
	//    Type of analysis : viscous or inviscid
	//
	//  Output
	//    centre of pressure position (XCP, YCP)
	//    moment coefficients GCm, VCm, ICm, GRm, GYm, VYm, IYm
	//


	int  j, k, l, p, m, nFlap, coef;
	double CPStrip, tau, NForce, cosa, sina;
	CVector HingeLeverArm,  PtC4Strip, PtLEStrip, ForcePt, SurfaceNormal, LeverArmC4CoG, LeverArmPanelC4, LeverArmPanelCoG;
	CVector Force, PanelForce, StripForce, DragVector, Moment0, HingeMoment, DragMoment, GeomMoment;
	CVector WindNormal, WindDirection;
	CVector Origin(0.0,0.0,0.0);

	//initialize
	m_GRm =0.0;
	m_GCm = m_VCm = m_ICm = 0.0;
	m_GYm = m_VYm = m_IYm = 0.0;

	// Define the number of panels to consider on each strip
	if(pWPolar->m_bThinSurfaces) coef = 1;    // only mid-surface
	else                         coef = 2;    // top and bottom surfaces

	// Define the wind axis
	cosa = cos(Alpha*PI/180.0);
	sina = sin(Alpha*PI/180.0);
	WindNormal.Set(   -sina, 0.0, cosa);
	WindDirection.Set( cosa, 0.0, sina);

	// Calculate the Reynolds number on each strip
	for (m=0; m< m_NStation; m++) m_Re[m] = m_Chord[m] * QInf /pWPolar->m_Viscosity;

	m = p = nFlap = 0;

	// For each of the wing's surfaces, calculate the coefficients on each strip
	// and sum them up to get the wing's overall coefficients
	for (j=0; j<m_NSurfaces; j++)
	{
		//do not consider left tip patch, if any
		if(!pWPolar->m_bThinSurfaces && m_Surface[j].m_bIsTipLeft) p += m_Surface[j].m_NXPanels;

		if(m_Surface[j].m_bTEFlap) m_FlapMoment[nFlap] = 0.0;

		SurfaceNormal = m_Surface[j].Normal;

		// consider each strip in turn
		for (k=0; k<m_Surface[j].m_NYPanels; k++)
		{
			//initialize
			DragVector.Set(0.0,0.0,0.0);
			StripForce.Set(0.0,0.0,0.0);
			GeomMoment.Set(0.0,0.0,0.0);

			m_CmAirf[m]    = 0.0;
			CPStrip        = 0.0;

			m_Surface[j].GetLeadingPt(k, PtLEStrip);
			m_Surface[j].GetC4(k, PtC4Strip, tau);
			if(pWPolar->m_Beta>0.0)
			{
				PtC4Strip.RotateZ(Origin, pWPolar->m_Beta);
				PtLEStrip.RotateZ(Origin, pWPolar->m_Beta);
			}

			LeverArmC4CoG = PtC4Strip - CoG;


			for (l=0; l<coef*m_Surface[j].m_NXPanels; l++)
			{
				// Get the force acting on the panel
				if(m_pPanel[p].m_Pos!=MIDSURFACE)
				{
					ForcePt = m_pPanel[p].CollPt;
					PanelForce = m_pPanel[p].Normal * (-Cp[p]) * m_pPanel[p].Area;      // Newtons/q
				}
				else
				{
					// for each panel along the chord, add the lift coef
					ForcePt = m_pPanel[p].VortexPos;
					PanelForce  = WindDirection * m_pPanel[p].Vortex;
					PanelForce *= 2.0 * Gamma[p] /QInf;                                 //Newtons/q

					if(!pWPolar->m_bVLM1 && !m_pPanel[p].m_bIsLeading)
					{
						Force       = WindDirection * m_pPanel[p].Vortex;
						Force      *= 2.0 * Gamma[p+1] /QInf;                          //Newtons/q
						PanelForce -= Force;
					}
					Cp[p] = PanelForce.dot(m_pPanel[p].Normal)/m_pPanel[p].Area;    //
				}
				StripForce += PanelForce;                                           // Newtons/q
				NForce = PanelForce.dot(SurfaceNormal);                             // Newtons/q

				LeverArmPanelC4    = ForcePt - PtC4Strip;                           // m
				LeverArmPanelCoG   = ForcePt - CoG;                                 // m


				Moment0 = LeverArmPanelC4 * PanelForce;                             // N.m/q
				m_CmAirf[m]  += Moment0.y;                                          // N.m/q

				GeomMoment += LeverArmPanelCoG * PanelForce;                        // N.m/q

				XCP       += ForcePt.x * PanelForce.dot(WindNormal); //global center of pressure
				YCP       += ForcePt.y * PanelForce.dot(WindNormal);
				CPStrip   += ForcePt.x * NForce;

				if(m_Surface[j].m_bTEFlap)
				{
					if(m_Surface[j].IsFlapPanel(p))
					{
						//then p is on the flap, so add its contribution
						HingeLeverArm = ForcePt - m_Surface[j].m_HingePoint;
						HingeMoment = HingeLeverArm * PanelForce;//N.m/q
						m_FlapMoment[nFlap] += HingeMoment.dot(m_Surface[j].m_HingeVector)* pWPolar->m_Density * QInf * QInf/2.0;  //N.m
					}
				}
				p++;
			}

			// calculate center of pressure position
			NForce = StripForce.dot(SurfaceNormal);
			m_XCPSpanRel[m]    = (CPStrip/NForce - PtLEStrip.x)/m_Chord[m];
			m_XCPSpanAbs[m]    =  CPStrip/NForce ;

			// add viscous properties, if required
			if(pWPolar->m_bViscous) DragVector.x = m_PCd[m] * m_StripArea[m];// N/q //TODO : orient along wind direction rather than x-axis
			else                    DragVector.x = 0.0;
			// global moments, in N.m/q
			DragMoment =  LeverArmC4CoG * DragVector;

			m_GRm += GeomMoment.dot(WindDirection);

			m_VYm += DragMoment.dot(WindNormal);

//			m_IYm += -m_ICd[m] * m_StripArea[m] * PtC4Strip.y ;
			m_IYm += GeomMoment.dot(WindNormal);

			m_VCm += DragMoment.y;
			m_ICm += GeomMoment.y;

			m_CmAirf[m]    *= 1.0                          /m_Chord[m]/m_StripArea[m];
			m_Cm[m]         = (GeomMoment.y + DragMoment.y)/m_Chord[m]/m_StripArea[m];
			m++;
		}
		//do not consider right tip patch
		if(!pWPolar->m_bThinSurfaces && m_Surface[j].m_bIsTipRight) p += m_Surface[j].m_NXPanels;
		if(m_Surface[j].m_bTEFlap) nFlap++;
	}

	//global plane dimensionless coefficients
	GCm += m_VCm + m_ICm;
	VCm += m_VCm;
	ICm += m_ICm;

	//sign convention for rolling and yawing is opposite to algebric results
	GRm -= m_GRm;
	GYm -= (m_VYm + m_IYm);
	VYm -= m_VYm;
	IYm -= m_IYm;
}


void CWing::PanelComputeViscous(double QInf, double Alpha, CWPolar *pWPolar, double &WingVDrag, bool bViscous, QString &OutString)
{
	// In input, takes the speed QInf and the distribution of lift coefficients m_Cl[] along the span
	// In output, returns for each span station
	// 	- The Reynolds number m_Re[]
	//	- The viscous drag coefficient m_PCd[]
	//      - The top and bottom transition points m_XTrtop[] and m_XTrBot[]
	//
	QString string, strong, strLength;
	int m;
	bool bPointOutRe, bPointOutCl, bOutRe, bError;
	double tau = 0.0;
	CVector PtC4;

	OutString.clear();

	WingVDrag = 0.0;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	bOutRe = bError = bPointOutRe = bPointOutCl = false;

	GetLengthUnit(strLength, pMainFrame->m_LengthUnit);

	// Calculate the Reynolds number on each strip
	for (m=0; m<m_NStation; m++)  m_Re[m] = m_Chord[m] * QInf /pWPolar->m_Viscosity;

	if(!bViscous)
	{
		for(m=0; m <m_NStation; m++)
		{
			m_PCd[m] = m_XTrTop[m] = m_XTrBot[m] = 0.0;
		}
		return;
	}

	//Interpolate the viscous properties from the foil's type 1 polar mesh
	m=0;
	for (int j=0; j<m_NSurfaces; j++)
	{
		for(int k=0; k<m_Surface[j].m_NYPanels; k++)
		{
			bOutRe = bPointOutRe = false;
			bPointOutCl = false;
			m_Surface[j].GetC4(k, PtC4, tau);

			m_PCd[m]    = GetVar(pMiarex->m_poaPolar, 2, m_Surface[j].m_pFoilA, m_Surface[j].m_pFoilB, m_Re[m], m_Cl[m], tau, bOutRe, bError);
			bPointOutRe = bOutRe || bPointOutRe;
			if(bError) bPointOutCl = true;
			m_XTrTop[m] = GetVar(pMiarex->m_poaPolar, 5, m_Surface[j].m_pFoilA, m_Surface[j].m_pFoilB, m_Re[m], m_Cl[m], tau, bOutRe, bError);
			bPointOutRe = bOutRe || bPointOutRe;
			if(bError) bPointOutCl = true;
			m_XTrBot[m] = GetVar(pMiarex->m_poaPolar, 6, m_Surface[j].m_pFoilA, m_Surface[j].m_pFoilB, m_Re[m], m_Cl[m], tau, bOutRe, bError);
			bPointOutRe = bOutRe || bPointOutRe;
			if(bError) bPointOutCl = true;
			if(bPointOutCl)
			{
				GetLengthUnit(string, pMainFrame->m_LengthUnit);
				strong = QString(QObject::tr("           Span pos = %1 ")).arg(m_SpanPos[m]*pMainFrame->m_mtoUnit, 9,'f',2);
				strong += string;
				strong += ",  Re = ";
				ReynoldsFormat(string, m_Re[m]);
				strong += string;

				string = QString(QObject::tr(",  Cl = %1 could not be interpolated")+"\n").arg(m_Cl[m],6,'f',2);
				strong+= string;
				OutString += strong;

				m_bWingOut = true;
			}
			else if(bPointOutRe)
			{
				GetLengthUnit(string, pMainFrame->m_LengthUnit);
				strong = QString(QObject::tr("           Span pos = %1 ")).arg(m_SpanPos[m]*pMainFrame->m_mtoUnit,9,'f',2);
				strong += string;
				strong += ",  Re = ";
				ReynoldsFormat(string, m_Re[m]);
				strong += string;
				string = QString(QObject::tr(",  Cl = %1 is outside the flight envelope")+"\n").arg(m_Cl[m],6,'f',2);
				strong += string;
				OutString += strong;

				m_bWingOut = true;
			}

			// Sum the total viscous drag of this wing
			WingVDrag  += m_PCd[m] * m_StripArea[m];
			m++;
		}
	}
}


bool CWing::IsWingPanel(int nPanel)
{
	for(int p=0; p<m_MatSize; p++)
	{
		if(nPanel==m_pPanel[p].m_iElement) return true;
	}
	return false;
}


bool CWing::IsWingNode(int nNode)
{
	for(int p=0; p<m_MatSize; p++)
	{
		if(nNode==m_pPanel[p].m_iLA) return true;
		if(nNode==m_pPanel[p].m_iLB) return true;
		if(nNode==m_pPanel[p].m_iTA) return true;
		if(nNode==m_pPanel[p].m_iTB) return true;
	}
	return false;
}
