/****************************************************************************

    CPlane Class
	Copyright (C) 2006-2009 Andre Deperrois xflr5@yahoo.com

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

#include "Plane.h"
#include "../Miarex/Miarex.h"
#include "../Globals.h"
#include <QtDebug>
#include <math.h>

void*	CPlane::s_pMainFrame;
void*	CPlane::s_pMiarex ;

//
CPlane::CPlane()
{
	m_pBody   = NULL;

	m_Wing.m_WingName   = QObject::tr("Wing");
	m_Wing.ComputeGeometry();
	m_Wing2.m_WingName   = QObject::tr("2nd Wing");
	m_Wing2.ComputeGeometry();

	m_Stab.m_WingName      = QObject::tr("Elevator");
	m_Stab.m_bIsFin        = false;
	m_Stab.m_TChord[0]     = 0.100;
	m_Stab.m_TChord[1]     = 0.080;
	m_Stab.m_TPos[0]       =   0.0;
	m_Stab.m_TPos[1]       = 0.150;
	m_Stab.m_TLength[0]    =   0.0;
	m_Stab.m_TLength[1]    = 0.150;
	m_Stab.m_TOffset[0]    =   0.0;
	m_Stab.m_TOffset[1]    = 0.020;
	m_Stab.m_NXPanels[0]   = 3;
	m_Stab.m_NYPanels[0]   = 3;
	m_Stab.m_YPanelDist[0] = 1;
	m_Stab.m_YPanelDist[0] = 0;
	m_Stab.ComputeGeometry();

	m_Fin.m_WingName      = QObject::tr("Fin");
	m_Fin.m_bIsFin        = true;
	m_Fin.m_TChord[0]     = 0.100;
	m_Fin.m_TChord[1]     = 0.060;
	m_Fin.m_TPos[0]       = 0.000;
	m_Fin.m_TPos[1]       = 0.120;
	m_Fin.m_TLength[0]    = 0.000;
	m_Fin.m_TLength[1]    = 0.120;
	m_Fin.m_TOffset[0]    = 0.000;
	m_Fin.m_TOffset[1]    = 0.040;
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

	m_CoG.Set(0.0,0.0,0.0);
	m_CoGIxx = m_CoGIyy = m_CoGIzz = m_CoGIxz = 0.0;
	m_VolumeMass = m_TotalMass = 0.0;

	m_bActive       = false;
	m_bBody         = false;
	m_bFin          = true;
	m_bDoubleFin    = false;
	m_bSymFin       = false;
	m_bStab         = true;
	m_bDoubleSymFin = true;
	m_bBiplane      = false;

	m_NMass = 0;
	memset(m_MassValue,    0, sizeof(m_MassValue));
	memset(m_MassPosition, 0, sizeof(m_MassPosition));
	for(int i=0; i< MAXMASSES; i++) m_MassTag[i] = QString("Description %1").arg(i);


	m_PlaneName  = QObject::tr("Plane Name");
}


void CPlane::ComputeVolumeInertia(double &Mass, CVector & CoG, double &CoGIxx, double &CoGIyy, double &CoGIzz, double &CoGIxz)
{
	// the analysis uses each object inertia individually
	// the export to AVL format is done for each object individually
	// we add the volume inertias of the wings and body, excluding point masses

	//initialize
	double Ixx, Iyy, Izz, Ixz, PlaneMass;
	CVector Pt;
	CVector CoGBody;
	CVector WingPos[4], CoGWing[4];
	CWing *pWing[4];
	pWing[0] = pWing[1] = pWing[2] = pWing[3] = NULL;

	WingPos[0] = m_LEWing;
	WingPos[1] = m_LEWing2;
	WingPos[2] = m_LEStab;
	WingPos[3] = m_LEFin;

	pWing[0] = &m_Wing;
	if(m_bBiplane) pWing[1] = &m_Wing2;
	if(m_bStab)    pWing[2] = &m_Stab;
	if(m_bFin)     pWing[3] = &m_Fin;


	CoG.Set(0.0, 0.0, 0.0);
	CoGIxx = CoGIyy = CoGIzz = CoGIxz = 0.0;
	PlaneMass = 0.0;

	//get the wing's inertias
	for(int iw=0; iw<4; iw++)
	{
		if(pWing[iw] && pWing[iw]->m_VolumeMass>PRECISION)
		{
			pWing[iw]->ComputeVolumeInertia(CoGWing[iw], Ixx, Iyy, Izz, Ixz);
			CoG += CoGWing[iw] * pWing[iw]->m_VolumeMass;
			PlaneMass += pWing[iw]->m_VolumeMass;
			CoGIxx += Ixx;
			CoGIyy += Iyy;
			CoGIzz += Izz;
			CoGIxz += Ixz;
		}
	}

	if(m_bBody)
	{
		if(m_pBody->m_VolumeMass>PRECISION)
		{
			m_pBody->ComputeVolumeInertia(CoGBody, Ixx, Iyy, Izz, Ixz);
			CoG += CoGBody * m_pBody->m_VolumeMass;
			PlaneMass += m_pBody->m_VolumeMass;
			CoGIxx += Ixx;
			CoGIyy += Iyy;
			CoGIzz += Izz;
			CoGIxz += Ixz;
		}
	}

	if(PlaneMass>0.0) CoG *= 1.0/ PlaneMass;
	else              CoG.Set(0.0, 0.0, 0.0);


	//Deduce inertia tensor in plane CoG from Huyghens/Steiner theorem
	// we transfer the inertia of each component, defined in its own CG, 
	// to the new origin which is the plane's Volume CoG, excluding point masses

	for(int iw=0; iw<4; iw++)
	{
		if(pWing[iw])
		{
			Pt = CoGWing[iw] - CoG;
			CoGIxx +=  pWing[iw]->m_VolumeMass * (Pt.y*Pt.y + Pt.z*Pt.z);
			CoGIyy +=  pWing[iw]->m_VolumeMass * (Pt.x*Pt.x + Pt.z*Pt.z);
			CoGIzz +=  pWing[iw]->m_VolumeMass * (Pt.x*Pt.x + Pt.y*Pt.y);
			CoGIxz -=  pWing[iw]->m_VolumeMass *  Pt.x*Pt.z;
		}
	}

	if(m_bBody)
	{
		Pt = CoGBody - CoG;
		CoGIxx += m_pBody->m_VolumeMass * (Pt.y*Pt.y + Pt.z*Pt.z);
		CoGIyy += m_pBody->m_VolumeMass * (Pt.x*Pt.x + Pt.z*Pt.z);
		CoGIzz += m_pBody->m_VolumeMass * (Pt.x*Pt.x + Pt.y*Pt.y);
		CoGIxz -= m_pBody->m_VolumeMass *  Pt.x*Pt.z;
	}
	Mass = PlaneMass;
}



void CPlane::ComputeBodyAxisInertia()
{
	// Initializes the inertia tensor in geometrical (body) axis :
	//  - add the volume inertia AND the point masses of all components
	//  - the body axis is the frame in which all the geometry has been defined
	//  - the origin=BodyFrame;
	//  - the center of gravity is calculated from component masses and is NOT the CoG defined in the polar

	int i, iw;
	CVector LA, VolumeCoG, TotalCoG;
	CWing *pWing[4];
	CVector WingPos[4];
	pWing[0] = pWing[1] = pWing[2] = pWing[3] = NULL;
	double Ixx, Iyy, Izz, Ixz,  VolumeMass, TotalMass;
	Ixx = Iyy = Izz = Ixz = TotalMass = VolumeMass = 0.0;

	WingPos[0] = m_LEWing;
	WingPos[1] = m_LEWing2;
	WingPos[2] = m_LEStab;
	WingPos[3] = m_LEFin;

	pWing[0] = &m_Wing;
	if(m_bBiplane) pWing[1] = &m_Wing2;
	if(m_bStab)    pWing[2] = &m_Stab;
	if(m_bFin)     pWing[3] = &m_Fin;

	ComputeVolumeInertia(VolumeMass, VolumeCoG, Ixx, Iyy, Izz, Ixz);
	TotalMass = VolumeMass;

	TotalCoG = VolumeCoG *VolumeMass;

	// add point masses
	for(i=0; i<m_NMass; i++)
	{
		TotalMass += m_MassValue[i];
		TotalCoG += m_MassPosition[i] * m_MassValue[i];
	}

	for(iw=0; iw<4; iw++)
	{
		if(pWing[iw])
		{
			for(i=0; i<pWing[iw]->m_NMass; i++)
			{
				TotalMass += pWing[iw]->m_MassValue[i];
				TotalCoG +=  (pWing[iw]->m_MassPosition[i]+ WingPos[iw]) * pWing[iw]->m_MassValue[i];
			}
		}
	}

	if(m_bBody)
	{
		CBody *pBody = m_pBody;
		for(i=0; i<pBody->m_NMass; i++)
		{
			TotalMass += pBody->m_MassValue[i];
			TotalCoG  += (pBody->m_MassPosition[i]+m_BodyPos) * pBody->m_MassValue[i];
		}
	}
	if(TotalMass>PRECISION)	TotalCoG = TotalCoG/TotalMass;
	else                    TotalCoG.Set(0.0,0.0,0.0);

	// The CoG position is now available, so calculate the inertia w.r.t the TotalCoG, including point masses
	// TotalCoG is the new origin for this calculation, so we transfer the other inertias using Huyghens/Steiner theorem

	for(i=0; i<m_NMass; i++)
	{
		LA = m_MassPosition[i] - TotalCoG;
		Ixx += m_MassValue[i] * (LA.y*LA.y + LA.z*LA.z);
		Iyy += m_MassValue[i] * (LA.x*LA.x + LA.z*LA.z);
		Izz += m_MassValue[i] * (LA.x*LA.x + LA.y*LA.y);
		Ixz -= m_MassValue[i] * (LA.x*LA.z);
	}

	for(iw=0; iw<4; iw++)
	{
		if(pWing[iw])
		{
			for(i=0; i<pWing[iw]->m_NMass; i++)
			{
				LA = (pWing[iw]->m_MassPosition[i] + WingPos[i]) - TotalCoG;
				Ixx += pWing[iw]->m_MassValue[i] * (LA.y*LA.y + LA.z*LA.z);
				Iyy += pWing[iw]->m_MassValue[i] * (LA.x*LA.x + LA.z*LA.z);
				Izz += pWing[iw]->m_MassValue[i] * (LA.x*LA.x + LA.y*LA.y);
				Ixz -= pWing[iw]->m_MassValue[i] * (LA.x*LA.z);
			}
		}
	}

	if(m_bBody)
	{
		CBody *pBody = m_pBody;
		for(i=0; i<pBody->m_NMass; i++)
		{
			LA = (pBody->m_MassPosition[i] + m_BodyPos) - TotalCoG;
			Ixx += pBody->m_MassValue[i] * (LA.y*LA.y + LA.z*LA.z);
			Iyy += pBody->m_MassValue[i] * (LA.x*LA.x + LA.z*LA.z);
			Izz += pBody->m_MassValue[i] * (LA.x*LA.x + LA.y*LA.y);
			Ixz -= pBody->m_MassValue[i] * (LA.x*LA.z);
		}
	}

	m_TotalMass = TotalMass;
	m_CoG = TotalCoG;
	m_CoGIxx =  Ixx;
	m_CoGIyy =  Iyy;
	m_CoGIzz =  Izz;
	m_CoGIxz =  Ixz;
}




void CPlane::ComputePlane(void)
{
	int i;
	if(m_bStab)
	{
		double SLA = m_LEStab.x + m_Stab.m_TChord[0]/4.0 - m_Wing.m_TChord[0]/4.0;
		double area = m_Wing.m_ProjectedArea;
		if(m_bBiplane) area += m_Wing2.m_ProjectedArea;

		double ProjectedArea = 0.0;
		for (i=0;i<m_Stab.m_NPanel; i++)
		{
			ProjectedArea += m_Stab.m_TLength[i+1]*(m_Stab.m_TChord[i]+m_Stab.m_TChord[i+1])/2.0
							*cos(m_Stab.m_TDihedral[i]*PI/180.0)*cos(m_Stab.m_TDihedral[i]*PI/180.0);//m2
		
		}
		ProjectedArea *=2.0;
		m_TailVolume = ProjectedArea * SLA / area/m_Wing.m_MAChord ;
	}
	else m_TailVolume = 0.0;
	
	m_Fin.m_bDoubleFin = m_bDoubleFin;
	m_Fin.m_bSymFin    = m_bSymFin;
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

	m_TotalMass  = pPlane->m_TotalMass;
	m_CoG = pPlane->m_CoG;
	m_CoGIxx = pPlane->m_CoGIxx;
	m_CoGIyy = pPlane->m_CoGIyy;
	m_CoGIzz = pPlane->m_CoGIzz;
	m_CoGIxz = pPlane->m_CoGIxz;
	m_NMass = pPlane->m_NMass;
	for(int i=0; i<m_NMass;i++)
	{
		m_MassValue[i] = pPlane->m_MassValue[i];
		m_MassPosition[i].Copy(pPlane->m_MassPosition[i]);
		m_MassTag[i] = pPlane->m_MassTag[i];
	}

	m_bBody = pPlane->m_bBody ;
	m_pBody = pPlane->m_pBody;
}


double CPlane::GetTotalMass()
{
	static double Mass;
	
	Mass = m_Wing.GetTotalMass();
	if(m_bBiplane) Mass += m_Wing2.GetTotalMass();
	if(m_bStab)    Mass += m_Stab.GetTotalMass();
	if(m_bFin)     Mass += m_Fin.GetTotalMass();
	if(m_bBody && m_pBody)    Mass += m_pBody->GetTotalMass();
	
	for(int i=0; i<m_NMass; i++)
		Mass += m_MassValue[i];
	return Mass;
}


bool CPlane::SerializePlane(QDataStream &ar, bool bIsStoring, int ProjectFormat)
{
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	int i;
	float f,g,h;

	QString strong = "Nobody";
	int ArchiveFormat;// identifies the format of the file
	if (bIsStoring)
	{
		// storing code
		if(ProjectFormat>=5)      ar << 1012;
		else if(ProjectFormat==4) ar << 1010;
		//1012 : QFLR5 v0.03 : added mass properties for inertia calculations
		//1011 : QFLR5 v0.02 : added Plane description field
		//1010 : added body LE x and z position
		//1009 : added Main wing LE x and z position
		//1008 : added body data
		//1007 : added second wing data, CheckPanel
		//1006 : Converted lengths to m
		//1005 : stored double sym fin
		//1004 : suppressed colors
		//1003 : Added fin tilt;
		//1002 : Added doublefin;
		WriteCString(ar, m_PlaneName);
		if(ProjectFormat>=5)      WriteCString(ar, m_PlaneDescription);

		m_Wing.SerializeWing(ar, true, ProjectFormat);
		m_Wing2.SerializeWing(ar, true, ProjectFormat);
		m_Stab.SerializeWing(ar, true, ProjectFormat);
		m_Fin.SerializeWing(ar, true, ProjectFormat);
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

		if(m_bBody && m_pBody)
		{
			ar <<1;
			WriteCString(ar, m_pBody->m_BodyName);
		}
		else
		{
			ar << 0;
			WriteCString(ar, strong);
		}
		if(ProjectFormat>=5)
		{
			ar << m_NMass;
			for(i=0; i<m_NMass; i++) ar << (float)m_MassValue[i];
			for(i=0; i<m_NMass; i++) ar << (float)m_MassPosition[i].x << (float)m_MassPosition[i].y << (float)m_MassPosition[i].z;
			for(i=0; i<m_NMass; i++)  WriteCString(ar, m_MassTag[i]);
		}
		return true;
	}
	else
	{	// loading code
		int k;
		ar >> ArchiveFormat;
		if (ArchiveFormat <1001 || ArchiveFormat>1100)
		{
			m_PlaneName = "";
			return false;
		}

		ReadCString(ar,m_PlaneName);
		if (m_PlaneName.length() ==0) return false;

		if(ArchiveFormat>=1011) ReadCString(ar, m_PlaneDescription);

		m_Wing.SerializeWing(ar, false, ProjectFormat);
		if(ArchiveFormat>=1007) m_Wing2.SerializeWing(ar, false, ProjectFormat);
		m_Stab.SerializeWing(ar, false, ProjectFormat);
		m_Fin.SerializeWing(ar, false, ProjectFormat);

		ar >>k;
		if(k) m_bStab = true; else m_bStab = false;
		ar >>k;
		if(k) m_bFin = true;  else m_bFin = false;

		if(ArchiveFormat>=1002)
		{
			ar >>k;
			if(k) m_bDoubleFin = true;  else m_bDoubleFin = false;
			m_Fin.m_bDoubleFin = m_bDoubleFin;
			ar >>k;
			if(k) m_bSymFin = true;  else m_bSymFin = false;
			m_Fin.m_bSymFin = m_bSymFin;
		}
		if(ArchiveFormat>=1005)
		{
			ar >>k;
			if(k) m_bDoubleSymFin = true;  else m_bDoubleSymFin = false;
			m_Fin.m_bDoubleSymFin = m_bDoubleSymFin;
		}
		if(ArchiveFormat>=1007)
		{
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

		if(ArchiveFormat<1004)
		{
			QColor cr;
			ReadCOLORREF(ar,cr);
			ReadCOLORREF(ar,cr);
			ReadCOLORREF(ar,cr);
		}

		if(ArchiveFormat<1006)
		{
			//convert to m
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
			ReadCString(ar,strong);
			m_pBody = pMiarex->GetBody(strong);
		}
		else m_pBody = NULL;

		if(ArchiveFormat>=1012)
		{
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

		ComputePlane();

		return true;
	}
}


















