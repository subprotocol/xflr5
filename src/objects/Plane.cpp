/****************************************************************************

    CPlane Class
	Copyright (C) 2006-2013 Andre Deperrois adeperrois@xflr5.com

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
#include "../miarex/Miarex.h"
#include "../globals.h"
#include <math.h>

void*	Plane::s_pMainFrame;
void*	Plane::s_pMiarex ;

/** The public constructor. */
Plane::Plane()
{
	m_pBody   = NULL;

	m_Wing[0].m_WingName   = QObject::tr("Wing");
	m_Wing[0].ComputeGeometry();
	m_Wing[1].m_WingName   = QObject::tr("2nd Wing");
	m_Wing[1].ComputeGeometry();

	m_Wing[2].m_WingName    = QObject::tr("Elevator");
	m_Wing[2].m_bIsFin      = false;
	m_Wing[2].Chord(0)      = 0.100;
	m_Wing[2].Chord(1)      = 0.080;
	m_Wing[2].YPosition(0)  =   0.0;
	m_Wing[2].YPosition(1)  = 0.150;
	m_Wing[2].Length(0)     =   0.0;
	m_Wing[2].Length(1)     = 0.150;
	m_Wing[2].Offset(0)     =   0.0;
	m_Wing[2].Offset(1)     = 0.020;
	m_Wing[2].NXPanels(0)   = 7;
	m_Wing[2].NYPanels(0)   = 7;
	m_Wing[2].XPanelDist(0) = SINE;
	m_Wing[2].YPanelDist(0) = UNIFORM;
	m_Wing[2].ComputeGeometry();

	m_Wing[3].m_WingName    = QObject::tr("Fin");
	m_Wing[3].m_bIsFin      = true;
	m_Wing[3].Chord(0)      = 0.100;
	m_Wing[3].Chord(1)      = 0.060;
	m_Wing[3].YPosition(0)  = 0.000;
	m_Wing[3].YPosition(1)  = 0.120;
	m_Wing[3].Length(0)     = 0.000;
	m_Wing[3].Length(1)     = 0.120;
	m_Wing[3].Offset(0)     = 0.000;
	m_Wing[3].Offset(1)     = 0.040;
	m_Wing[3].NXPanels(0)   = 7;
	m_Wing[3].NYPanels(0)   = 7;
	m_Wing[3].XPanelDist(0) = UNIFORM;
	m_Wing[3].YPanelDist(0) = SINE;

	m_Wing[3].ComputeGeometry();

	m_TailVolume       =   0.0;
	m_WingLE[2].x      = 0.600;
	m_WingLE[2].y      =   0.0;
	m_WingLE[2].z      =   0.0;

	m_WingLE[3].x       = 0.650;
	m_WingLE[3].y       =   0.0;
	m_WingLE[3].z       =   0.0;

	for(int iw=0; iw<MAXWINGS; iw++) m_WingTiltAngle[iw] = 0.0;
	
	m_BodyPos.Set(0.0, 0.0, 0.0);

	m_CoG.Set(0.0,0.0,0.0);
	m_CoGIxx = m_CoGIyy = m_CoGIzz = m_CoGIxz = 0.0;
	m_VolumeMass = m_TotalMass = 0.0;

//	m_bActive       = false;
	m_bBody         = false;
	m_bFin          = true;
	m_bDoubleFin    = false;
	m_bSymFin       = false;
	m_bStab         = true;
	m_bDoubleSymFin = true;
	m_bBiplane      = false;

	m_PointMass.clear();

	m_PlaneName  = QObject::tr("Plane Name");
}

/**
 * Calculates and returns the inertia properties of the structure based on the Body and Wing masses and on the existing geometry.
 * The inertia is calculated in the CoG referential.
 * @param Mass = mass of the structure, excluding point masses
 * @param  &CoG a reference to the CoG point, as a result of the calculation
 * @param  &CoGIxx xx axis component of the inertia tensor, calculated at the Plane's CoG
 * @param  &CoGIyy yy axis component of the inertia tensor, calculated at the Plane's CoG
 * @param  &CoGIzz zz axis component of the inertia tensor, calculated at the Plane's CoG
 * @param  &CoGIxz xz axis component of the inertia tensor, calculated at the Plane's CoG
*/
void Plane::ComputeVolumeInertia(double &Mass, CVector & CoG, double &CoGIxx, double &CoGIyy, double &CoGIzz, double &CoGIxz)
{
	double Ixx, Iyy, Izz, Ixz, PlaneMass;
	CVector Pt;
	CVector CoGBody;
	CVector CoGWing[MAXWINGS];
	Wing *pWing[MAXWINGS];
	pWing[0] = pWing[1] = pWing[2] = pWing[3] = NULL;

	/** @todo use member array directly and NULL pointer testing*/
	pWing[0] = m_Wing;
	if(m_bBiplane) pWing[1] = m_Wing+1;
	if(m_bStab)    pWing[2] = m_Wing+2;
	if(m_bFin)     pWing[3] = m_Wing+3;

	CoG.Set(0.0, 0.0, 0.0);
	CoGIxx = CoGIyy = CoGIzz = CoGIxz = 0.0;
	PlaneMass = 0.0;

	//get the wing's inertias
	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(pWing[iw] && pWing[iw]->m_VolumeMass>PRECISION)
		{
			//the inertia of the wings are base on the surface geometry;
			//these surfaces have been translated to the LE position as they were created
			pWing[iw]->ComputeVolumeInertia(CoGWing[iw], Ixx, Iyy, Izz, Ixz);
			CoG += CoGWing[iw] * pWing[iw]->m_VolumeMass;// so we do not add again the LE position
			PlaneMass += pWing[iw]->m_VolumeMass;
			CoGIxx += Ixx;
			CoGIyy += Iyy;
			CoGIzz += Izz;
			CoGIxz += Ixz;
		}
	}

	if(body())
	{
		if(m_pBody->m_VolumeMass>PRECISION)
		{
			m_pBody->ComputeVolumeInertia(CoGBody, Ixx, Iyy, Izz, Ixz);
			CoG += (CoGBody+m_BodyPos) * m_pBody->m_VolumeMass;
			PlaneMass += m_pBody->m_VolumeMass;
			CoGIxx += Ixx;
			CoGIyy += Iyy;
			CoGIzz += Izz;
			CoGIxz += Ixz;
		}
	}

	if(PlaneMass>0.0) CoG *= 1.0/ PlaneMass;
	else              CoG.Set(0.0, 0.0, 0.0);


	// Deduce inertia tensor in plane CoG from Huyghens/Steiner theorem
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

	if(body())
	{
		Pt = CoGBody - CoG;
		CoGIxx += m_pBody->m_VolumeMass * (Pt.y*Pt.y + Pt.z*Pt.z);
		CoGIyy += m_pBody->m_VolumeMass * (Pt.x*Pt.x + Pt.z*Pt.z);
		CoGIzz += m_pBody->m_VolumeMass * (Pt.x*Pt.x + Pt.y*Pt.y);
		CoGIxz -= m_pBody->m_VolumeMass *  Pt.x*Pt.z;
	}
	Mass = PlaneMass;
}




/**
* Calculates the inertia tensor in geometrical (body) axis :
*  - adds the volume inertia AND the inertia of point masses of all components
*  - the body axis is the frame in which the geometry has been defined
*  - the origin is the plane's CoG, taking into account all masses
*/
void Plane::ComputeBodyAxisInertia()
{
	int i, iw;
	CVector VolumeCoG, MassPos;
	Wing *pWing[MAXWINGS];
	double Ixx, Iyy, Izz, Ixz,  VolumeMass;
	Ixx = Iyy = Izz = Ixz = VolumeMass = 0.0;


	/** @todo use member array directly and NULL pointer testing*/
	pWing[0] = m_Wing;
	if(m_bBiplane) pWing[1] = m_Wing+1; else pWing[1] = NULL;
	if(m_bStab)    pWing[2] = m_Wing+2; else pWing[2] = NULL;
	if(m_bFin)     pWing[3] = m_Wing+3; else pWing[3] = NULL;

	ComputeVolumeInertia(VolumeMass, VolumeCoG, Ixx, Iyy, Izz, Ixz);
	m_TotalMass = VolumeMass;

	m_CoG = VolumeCoG *VolumeMass;

	// add point masses
	for(i=0; i<m_PointMass.size(); i++)
	{
		m_TotalMass += m_PointMass[i]->mass();
		m_CoG       += m_PointMass[i]->position() * m_PointMass[i]->mass();
	}

	for(iw=0; iw<MAXWINGS; iw++)
	{
		if(pWing[iw])
		{
			for(i=0; i<pWing[iw]->m_PointMass.size(); i++)
			{
				m_TotalMass +=  pWing[iw]->m_PointMass[i]->mass();
				m_CoG       += (pWing[iw]->m_PointMass[i]->position()+ m_WingLE[iw]) * pWing[iw]->m_PointMass[i]->mass();
			}
		}
	}

	if(body())
	{
		for(i=0; i<m_pBody->m_PointMass.size(); i++)
		{
			m_TotalMass +=  m_pBody->m_PointMass[i]->mass();
			m_CoG       += (m_pBody->m_PointMass[i]->position()+m_BodyPos) * m_pBody->m_PointMass[i]->mass();
		}
	}
	if(m_TotalMass>PRECISION) m_CoG = m_CoG/m_TotalMass;
	else                      m_CoG.Set(0.0,0.0,0.0);


	// The CoG position is now available, so calculate the inertia w.r.t the Total CoG, including point masses
	// The total CoG is the new origin for this calculation, so we transfer the other inertias using Huygens/Steiner theorem
	// LA is the displacement vector from the centre of mass to the new axis
	MassPos = m_CoG - VolumeCoG;
	m_CoGIxx = Ixx + VolumeMass * (MassPos.y*MassPos.y+ MassPos.z*MassPos.z);
	m_CoGIyy = Iyy + VolumeMass * (MassPos.x*MassPos.x+ MassPos.z*MassPos.z);
	m_CoGIzz = Izz + VolumeMass * (MassPos.x*MassPos.x+ MassPos.y*MassPos.y);
	m_CoGIxz = Ixz - VolumeMass *  MassPos.x*MassPos.z;

	for(i=0; i<m_PointMass.size(); i++)
	{
		MassPos = m_CoG - m_PointMass[i]->position();
		m_CoGIxx += m_PointMass[i]->mass() * (MassPos.y*MassPos.y + MassPos.z*MassPos.z);
		m_CoGIyy += m_PointMass[i]->mass() * (MassPos.x*MassPos.x + MassPos.z*MassPos.z);
		m_CoGIzz += m_PointMass[i]->mass() * (MassPos.x*MassPos.x + MassPos.y*MassPos.y);
		m_CoGIxz -= m_PointMass[i]->mass() * (MassPos.x*MassPos.z);
	}

	for(iw=0; iw<MAXWINGS; iw++)
	{
		if(pWing[iw])
		{
			for(i=0; i<pWing[iw]->m_PointMass.size(); i++)
			{
				MassPos = m_CoG - (pWing[iw]->m_PointMass[i]->position() + m_WingLE[iw]);
				m_CoGIxx += pWing[iw]->m_PointMass[i]->mass() * (MassPos.y*MassPos.y + MassPos.z*MassPos.z);
				m_CoGIyy += pWing[iw]->m_PointMass[i]->mass() * (MassPos.x*MassPos.x + MassPos.z*MassPos.z);
				m_CoGIzz += pWing[iw]->m_PointMass[i]->mass() * (MassPos.x*MassPos.x + MassPos.y*MassPos.y);
				m_CoGIxz -= pWing[iw]->m_PointMass[i]->mass() * (MassPos.x*MassPos.z);
			}
		}
	}
	if(body())
	{
		Body *pBody = m_pBody;
		for(i=0; i<pBody->m_PointMass.size(); i++)
		{
			MassPos = m_CoG - (pBody->m_PointMass[i]->position() + m_BodyPos);
			m_CoGIxx += pBody->m_PointMass[i]->mass() * (MassPos.y*MassPos.y + MassPos.z*MassPos.z);
			m_CoGIyy += pBody->m_PointMass[i]->mass() * (MassPos.x*MassPos.x + MassPos.z*MassPos.z);
			m_CoGIzz += pBody->m_PointMass[i]->mass() * (MassPos.x*MassPos.x + MassPos.y*MassPos.y);
			m_CoGIxz -= pBody->m_PointMass[i]->mass() * (MassPos.x*MassPos.z);
		}
	}
}



/**
* Calculates the Plane's tail volume = lever_arm_elev x Area_Elev / MAC_Wing / Area_Wing 
*/
void Plane::ComputePlane(void)
{
	int i;
	if(m_bStab)
	{
		double SLA = m_WingLE[2].x + m_Wing[2].Chord(0)/4.0 - m_Wing[0].Chord(0)/4.0;
		double area = m_Wing[0].m_ProjectedArea;
		if(m_bBiplane) area += m_Wing[1].m_ProjectedArea;

		double ProjectedArea = 0.0;
		for (i=0;i<m_Wing[2].NWingSection()-1; i++)
		{
			ProjectedArea += m_Wing[2].Length(i+1)*(m_Wing[2].Chord(i)+m_Wing[2].Chord(i+1))/2.0
							*cos(m_Wing[2].Dihedral(i)*PI/180.0)*cos(m_Wing[2].Dihedral(i)*PI/180.0);//m2
		
		}
		ProjectedArea *=2.0;
		m_TailVolume = ProjectedArea * SLA / area/m_Wing[0].m_MAChord ;
	}
	else m_TailVolume = 0.0;
	m_Wing[3].m_bDoubleFin = m_bDoubleFin;
	m_Wing[3].m_bSymFin    = m_bSymFin;
}



/**
* Copies the data from an existing Plane
*@param pPlane a pointer to the instance of the source Plane object
*/
void Plane::Duplicate(Plane *pPlane)
{
	m_PlaneName        = pPlane->m_PlaneName;
	m_PlaneDescription = pPlane->m_PlaneDescription;
	RenameWings();

	m_bFin          = pPlane->m_bFin;
	m_bSymFin       = pPlane->m_bSymFin;
	m_bDoubleFin    = pPlane->m_bDoubleFin;
	m_bDoubleSymFin = pPlane->m_bDoubleSymFin;
	m_bStab         = pPlane->m_bStab;
	m_bBiplane      = pPlane->m_bBiplane;

	m_TailVolume    = pPlane->m_TailVolume;

	for(int iw=0; iw<MAXWINGS; iw++)
	{
		m_WingTiltAngle[iw] = pPlane->m_WingTiltAngle[iw];
		m_WingLE[iw]        = pPlane->m_WingLE[iw];
		m_Wing[iw].Duplicate(pPlane->m_Wing+iw);
	}

	m_BodyPos.Copy(pPlane->m_BodyPos);

	m_TotalMass  = pPlane->m_TotalMass;
	m_CoG = pPlane->m_CoG;
	m_CoGIxx = pPlane->m_CoGIxx;
	m_CoGIyy = pPlane->m_CoGIyy;
	m_CoGIzz = pPlane->m_CoGIzz;
	m_CoGIxz = pPlane->m_CoGIxz;

	m_PointMass.clear();
	for(int i=0; i<pPlane->m_PointMass.size();i++)
	{
		m_PointMass.append(new PointMass(pPlane->m_PointMass[i]));
	}

	m_bBody = pPlane->m_bBody ;
	m_pBody = pPlane->m_pBody;

}

/**
* Returns the plane's tail volume
* @return the plane's tail volume
*/
double Plane::TailVolume()
{
	if(m_bStab) return m_TailVolume;
	else        return 0.0;
}

/**
* Returns the Plane's total mass, i.e. the sum of Volume and Point masses of all its components.
* @return the Plane's total mass.
*/
double Plane::TotalMass()
{
	static double Mass;
	
	Mass = m_Wing[0].TotalMass();
	if(m_bBiplane) Mass += m_Wing[1].TotalMass();
	if(m_bStab)    Mass += m_Wing[2].TotalMass();
	if(m_bFin)     Mass += m_Wing[3].TotalMass();
	if(body())  Mass += m_pBody->TotalMass();
	
	for(int i=0; i<m_PointMass.size(); i++)
		Mass += m_PointMass[i]->mass();
		
	return Mass;
}



/**
 * Loads or Saves the data of this Plane to a binary file.
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool Plane::SerializePlane(QDataStream &ar, bool bIsStoring)
{
	QMiarex *pMiarex = (QMiarex*)s_pMiarex;

	int i, nMass;
	float f,g,h;

	QString strong = "";
	int ArchiveFormat;// identifies the format of the file
	if (bIsStoring)
	{
		// storing code
		ar << 1012;
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
		WriteCString(ar, m_PlaneDescription);

		m_Wing[0].SerializeWing(ar, true);
		m_Wing[1].SerializeWing(ar, true);
		m_Wing[2].SerializeWing(ar, true);
		m_Wing[3].SerializeWing(ar, true);
		if(m_bStab)          ar <<1; else ar <<0;
		if(m_bFin)           ar <<1; else ar <<0;
		if(m_bDoubleFin)     ar <<1; else ar <<0;
		if(m_bSymFin)		 ar <<1; else ar <<0;
		if(m_bDoubleSymFin)  ar <<1; else ar <<0;
//		if(m_bCheckPanels)   ar <<1; else ar <<0;
		ar << 0;
		if(m_bBiplane)       ar <<1; else ar <<0;

		ar << m_WingLE[1].x << m_WingLE[1].y << m_WingLE[1].z << m_WingTiltAngle[1];
		ar << m_WingLE[2].x << m_WingLE[2].y << m_WingLE[2].z;
		ar << m_WingTiltAngle[0] << m_WingTiltAngle[2] << m_WingTiltAngle[3] ;
		ar << m_WingLE[3].x << m_WingLE[3].y << m_WingLE[3].z;
		ar << m_BodyPos.x << m_BodyPos.z;

		ar << m_WingLE[0].x << m_WingLE[0].z;

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


		ar << m_PointMass.size();
		for(i=0; i<m_PointMass.size(); i++) ar << (float)m_PointMass[i]->mass();
		for(i=0; i<m_PointMass.size(); i++) ar << (float)m_PointMass[i]->position().x << (float)m_PointMass[i]->position().y << (float)m_PointMass[i]->position().z;
		for(i=0; i<m_PointMass.size(); i++)  WriteCString(ar, m_PointMass[i]->tag());

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

		/**@todo remove non active wings from serialization */
		m_Wing[0].SerializeWing(ar, false);
		if(ArchiveFormat>=1007) m_Wing[1].SerializeWing(ar, false);
		m_Wing[2].SerializeWing(ar, false);
		m_Wing[3].SerializeWing(ar, false);

		ar >>k;
		if(k) m_bStab = true; else m_bStab = false;
		ar >>k;
		if(k) m_bFin = true;  else m_bFin = false;

		if(ArchiveFormat>=1002)
		{
			ar >>k;
			if(k) m_bDoubleFin = true;  else m_bDoubleFin = false;
			m_Wing[3].m_bDoubleFin = m_bDoubleFin;
			ar >>k;
			if(k) m_bSymFin = true;  else m_bSymFin = false;
			m_Wing[3].m_bSymFin = m_bSymFin;
		}
		if(ArchiveFormat>=1005)
		{
			ar >>k;
			if(k) m_bDoubleSymFin = true;  else m_bDoubleSymFin = false;
			m_Wing[3].m_bDoubleSymFin = m_bDoubleSymFin;
		}
		if(ArchiveFormat>=1007)
		{
			ar >>k;
//			if(k) m_bCheckPanels = true;  else m_bCheckPanels = false;
			ar >>k;
			if(k) m_bBiplane = true;  else m_bBiplane = false;
			ar >> m_WingLE[1].x >> m_WingLE[1].y >> m_WingLE[1].z >> m_WingTiltAngle[1];
		}
		ar >>  m_WingLE[2].x >>  m_WingLE[2].y >>  m_WingLE[2].z;
		ar >> m_WingTiltAngle[0] >> m_WingTiltAngle[2];
		if(ArchiveFormat>=1003){
			ar >> m_WingTiltAngle[3];
		}
		ar >> m_WingLE[3].x >> m_WingLE[3].y >> m_WingLE[3].z;

		if(ArchiveFormat>=1010)
		{
			ar >> m_BodyPos.x >> m_BodyPos.z;
		}
		if(ArchiveFormat>=1009)
		{
			ar>> m_WingLE[0].x >> m_WingLE[0].z;
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
			m_WingLE[2].x /= 1000.0;
			m_WingLE[2].y /= 1000.0;
			m_WingLE[2].z /= 1000.0;
			m_WingLE[3].x  /= 1000.0;
			m_WingLE[3].y  /= 1000.0;
			m_WingLE[3].z  /= 1000.0;
		}
		if(ArchiveFormat>=1008)
		{
			ar >> k;
			if(k)  m_bBody=true; else m_bBody=false;
			ReadCString(ar,strong);
			m_pBody = pMiarex->GetBody(strong);
			if(!m_pBody) m_bBody=false;
		}
		else m_pBody = NULL;

		if(ArchiveFormat>=1012)
		{
			ar >> nMass;
			QVarLengthArray<double> mass;
			QVarLengthArray<CVector> position;
			QVarLengthArray<QString> tag;

			for(int im=0; im<nMass; im++)
			{
				ar >> f;
				mass.append(f);
			}
			for(int im=0; im<nMass; im++)
			{
				ar >> f >> g >> h;
				position.append(CVector(f,g,h));
			}
			for(int im=0; im<nMass; im++)
			{
				tag.append("");
				ReadCString(ar, tag[im]);
			}

			m_PointMass.clear();
			for(int im=0; im<nMass; im++)
			{
				m_PointMass.append(new PointMass(mass[im], position[im], tag[im]));
			}
		}

		ComputePlane();

		return true;
	}
}


/**
* Initializes the pointers to the application's widgets.
*/
void Plane::SetParents(void *pMainFrame, void*pMiarex)
{
	s_pMainFrame = pMainFrame;
	s_pMiarex    = pMiarex;
}

/**
* Renames each of the Plane's Wing objects with an automatic name.
*/
void Plane::RenameWings()
{
	m_Wing[0].m_WingName  = m_PlaneName+"_Wing";
	m_Wing[1].m_WingName = m_PlaneName+"_Wing2";
	m_Wing[2].m_WingName  = m_PlaneName+"_Elev";
	m_Wing[3].m_WingName   = m_PlaneName+"_Fin";
}

/**
* Creates the Surface objects associated to each of the Plane's Wing objects.
*/
void Plane::CreateSurfaces()
{
	m_Wing[0].CreateSurfaces(m_WingLE[0],  0.0, m_WingTiltAngle[0]);
	m_Wing[1].CreateSurfaces(m_WingLE[1], 0.0, m_WingTiltAngle[1]);
	m_Wing[2].CreateSurfaces(m_WingLE[2],  0.0, m_WingTiltAngle[2]);
	m_Wing[3].CreateSurfaces(m_WingLE[3], -90.0, m_WingTiltAngle[3]);
}

/**
* Initiliazes the pointer to an existing Body object
* @param pBody the pointer to the existing Body object
*/
void Plane::SetBody(Body *pBody)
{
	m_pBody = pBody;
	if(!m_pBody) m_bBody = false;
}





