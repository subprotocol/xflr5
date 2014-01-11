/****************************************************************************

	Wing Class
	Copyright (C) 2005-2014 Andre Deperrois adeperrois@xflr5.com

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


#include <math.h>
#include <QFile>
#include <QMessageBox>
#include <QtDebug>

#include "Wing.h"
#include "../globals.h"
#include "../miarex/Objects3D.h"
#include "../misc/Units.h"


double Wing::s_MinPanelSize = 0.0001;


/**
 * The public constructor.
 */
Wing::Wing()
{
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


	memset(m_SpanPos, 0, sizeof(m_SpanPos));
	memset(m_StripArea, 0, sizeof(m_StripArea));

	m_xHinge.clear();
	m_xHinge.insert(0, 1000, 0);
	m_xPanel.clear();
	m_xPanel.insert(0, 1000, 0);


	m_CoG.Set(0.0,0.0,0.0);
	m_CoGIxx = m_CoGIyy = m_CoGIzz = m_CoGIxz = 0.0;
	m_VolumeMass = m_TotalMass = 0.0;

	ClearPointMasses();

	m_bIsFin        = false;
	m_bDoubleFin    = false;
	m_bSymFin       = false;
	m_bDoubleSymFin = false;
	m_bSymetric     = true;
	m_bWingOut      = false;

	m_WingName        = QObject::tr("Wing Name");
	m_WingDescription = "";
	m_WingColor       = randomColor();

	m_QInf0    = 0.0;

	m_pWingPanel     = NULL;

	m_WingCL                = 0.0;
	m_CDv               = 0.0;
	m_CDi               = 0.0;
	m_GYm               = 0.0;
	m_IYm               = 0.0;
	m_GCm               = 0.0;
	m_GRm               = 0.0;
	m_VCm               = 0.0;
	m_VYm               = 0.0;

	m_CP.Set(0.0, 0.0, 0.0);

	m_AVLIndex = -(int)(qrand()/10000);//improbable value...

	m_MatSize   = 0;

	m_AR         = 0.0;// Aspect ratio
	m_TR         = 0.0;// Taper ratio
	m_GChord     = 0.0;// mean geometric chord
	m_MAChord    = 0.0;// mean aero chord
	m_yMac       = 0.0;
	m_ProjectedArea = 0.0;
	m_ProjectedSpan = 0.0;

	m_nFlaps        =  0;
	ClearWingSections();
	AppendWingSection(.180, .0, 0.0, 1.0, 0.000, 13, 19, COSINE, INVERSESINE, "", "");
	AppendWingSection(.110, .0, 1.0, 1.0, 0.070, 13, 5,  COSINE, UNIFORM, "", "");

	ComputeGeometry();

	double length = Length(0);
	for (int is=0; is<m_WingSection.size(); is++)
	{
		length += Length(is);
		YPosition(is)  = length;
		XPanelDist(is) =  COSINE;
	}
}

/** The public destructor */
Wing::~Wing()
{
	ClearWingSections();
	ClearPointMasses();
	ClearSurfaces();
}

/** Destroys the WingSection objects in good order to avoid memory leaks */
void Wing::ClearWingSections()
{
	for(int iws=m_WingSection.size()-1; iws>=0; iws--)
	{
		delete m_WingSection.at(iws);
		m_WingSection.removeAt(iws);
	}
}


/** Destroys the WingSection objects in good order to avoid memory leaks */
void Wing::ClearSurfaces()
{
	for(int is=m_Surface.size()-1; is>=0; is--)
	{
		delete m_Surface.at(is);
		m_Surface.removeAt(is);
	}
}

/** Destroys the PointMass objects in good order to avoid memory leaks */
void Wing::ClearPointMasses()
{
	for(int ipm=m_PointMass.size()-1; ipm>=0; ipm--)
	{
		delete m_PointMass.at(ipm);
		m_PointMass.removeAt(ipm);
	}
}


/**
 * Imports the wing geometry from a text file.
 * @param path_to_file the path to the filename as a QString
 */
void Wing::ImportDefinition(QString path_to_file)
{
	QFile fp(path_to_file);
	double ypos;
	double chord;
	double offset;
	double dihedral;
	double twist;
	int nx;
	int ny;
	int px, py;
	enumPanelDistribution x_pan_dist;
	enumPanelDistribution y_pan_dist;
	char right_buff[512];
	char left_buff[512];

	unsigned counter = 0;

	
	try{
		if (!fp.open(QIODevice::ReadOnly)) {
			QMessageBox::warning(0, QObject::tr("Warning"), QObject::tr("Could not open the file for reading"));
			return;
		} else {
			QTextStream infile(&fp);
			ClearWingSections();
			this->m_WingName = infile.readLine();
			while (true)
			{
				counter++;
				infile >> ypos >> chord >> offset >> dihedral >> twist >> nx >> ny;

				infile >> px >> py;

				if(px ==2)         x_pan_dist = INVERSESINE;
				else if(px ==  1)  x_pan_dist  = COSINE;
				else if(px == -2)  x_pan_dist  = SINE;
				else               x_pan_dist  = UNIFORM;

				if(py ==2)         y_pan_dist = INVERSESINE;
				else if(py ==  1)  y_pan_dist  = COSINE;
				else if(py == -2)  y_pan_dist  = SINE;
				else               y_pan_dist  = UNIFORM;

				infile >> right_buff >> left_buff;

				if (infile.atEnd())
				{
					fp.close();
					break;
				}
				//Append the sections convert from mm to m
				AppendWingSection(chord,
								  twist,
								  ypos,
								  dihedral,
								  offset,
								  nx,
								  ny,
								  x_pan_dist,
								  y_pan_dist,
								  QString(QString(QLatin1String(right_buff)).replace(QString("/_/"), QString(" "))),
								  QString(QString(QLatin1String(left_buff)).replace(QString("/_/"), QString(" ")))
								  );

			}
		}

		//Build the Geometry
		ComputeGeometry();
		double length = Length(0);
		for (int is=0; is<m_WingSection.size(); is++)
		{
			length += Length(is);
			YPosition(is)     = length;
			XPanelDist(is) =  COSINE;
		}
	}
	catch (iostream::failure e){
		QMessageBox::warning(0,QObject::tr("Warning"),QObject::tr("Unable to import wing definition\n"));
	}
}


/**
 * Exports the wing geometry to a text file.
 * @param path_to_file the path to the filename as a QString
 */
void Wing::ExportDefinition(QString path_to_file)
{
	try{
		QFile fp(path_to_file);
		if (!fp.open(QIODevice::WriteOnly)) {
			QMessageBox::warning(0, QObject::tr("Warning"), QObject::tr("Could not open the file for writing"));
			return;
		} else {
			QTextStream out_file(&fp);
			//Iterate the wing sections are write out the data...
			out_file << this->m_WingName << endl;
			for (int is=0; is<m_WingSection.size(); is++)
			{
				out_file << YPosition(is) << " " << Chord(is) << " " << Offset(is) \
						<< " " << Dihedral(is) << " " << Twist(is) << " " << NXPanels(is) \
						<< " " << NYPanels(is) << " ";

				switch(XPanelDist(is))
				{
					case COSINE:
						out_file <<  1;
						break;
					case SINE:
						out_file <<  2;
						break;
					case INVERSESINE:
						out_file << -2;
						break;
					default:
						out_file <<  0; //uniform
						break;
				}

				out_file << " " ;

				switch(YPanelDist(is))
				{
					case COSINE:
						out_file <<  1;
						break;
					case SINE:
						out_file <<  2;
						break;
					case INVERSESINE:
						out_file << -2;
						break;
					default:
						out_file <<  0; //uniform
						break;
				}


				if(RightFoil(is).isEmpty()){
					out_file  << " " << "/_/";
				} else {
					out_file  << " " << RightFoil(is).replace(QString(" "), QString("/_/")).toLatin1().data();
				}
				if(LeftFoil(is).isEmpty()) {
					out_file  << " " << "/_/";
				} else {
					out_file  << " " << LeftFoil(is).replace(QString(" "), QString("/_/")).toLatin1().data();
				}
				out_file << endl;
			}
			fp.close();
		}
	}
	catch (iostream::failure e){
		QMessageBox::warning(0,QObject::tr("Warning"),QObject::tr("Unable to import wing definition\n"));
	}
}


/**
 * Calculates the properties of the wing based on the input data.
 * Stores the results in the member variables.
 * Enables the user to see the properties of the wing in real time as the geometry is modified.
 */
void Wing::ComputeGeometry()
{
	Foil *pFoilA, *pFoilB;
	double MinPanelSize;
	int is;

	double surface = 0.0;
	double xysurface = 0.0;
	Length(0) = 0.0;
	YProj(0)  = YPosition(0);
	for (is=1; is<NWingSection(); is++)
		Length(is) = YPosition(is) - YPosition(is-1);
	for (is=1; is<NWingSection(); is++)
	{
		YProj(is) = YProj(is-1) + Length(is) * cos(Dihedral(is-1)*PI/180.0);
	}

	m_PlanformSpan  = 2.0 * TipPos();
	m_ProjectedSpan = 0.0;
	m_MAChord = 0.0;
	m_yMac    = 0.0;

	for (is=0; is<NWingSection()-1; is++)
	{
		pFoilA = Foil::foil(RightFoil(is));
		pFoilB = Foil::foil(RightFoil(is+1));
		surface   += Length(is+1)*(Chord(is)+Chord(is+1))/2.0;//m2
		xysurface += (Length(is+1)*(Chord(is)+Chord(is+1))/2.0)*cos(Dihedral(is)*PI/180.0);
		m_ProjectedSpan += Length(is+1)*cos(Dihedral(is)*PI/180.0);

		m_MAChord += IntegralC2(YPosition(is), YPosition(is+1), Chord(is), Chord(is+1));
		m_yMac    += IntegralCy(YPosition(is), YPosition(is+1), Chord(is), Chord(is+1));
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
	if(TipChord()>0.0)	m_TR = RootChord()/TipChord();
	else				m_TR = 99999.0;

	//calculate the number of flaps
	m_nFlaps = 0;
	if(s_MinPanelSize>0.0) MinPanelSize = s_MinPanelSize;
	else                   MinPanelSize = m_PlanformSpan;

	for (int is=1; is<NWingSection(); is++)
	{
		pFoilA = Foil::foil(RightFoil(is-1));
		pFoilB = Foil::foil(RightFoil(is));
		if(pFoilA && pFoilB && (!m_bIsFin || (m_bIsFin && m_bSymFin) || (m_bIsFin && m_bDoubleFin)))
		{
			if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && qAbs(YPosition(is)-YPosition(is-1))>MinPanelSize)	m_nFlaps++;
		}
		pFoilA = Foil::foil(LeftFoil(is-1));
		pFoilB = Foil::foil(LeftFoil(is));
		if(pFoilA && pFoilB)
		{
			if(pFoilA->m_bTEFlap && pFoilB->m_bTEFlap && qAbs(YPosition(is)-YPosition(is-1))>MinPanelSize)	m_nFlaps++;
		}
	}
}

#define NXSTATIONS 20
#define NYSTATIONS 40
#define MAXSPANSECTIONS 1000
/**
* Calculates and returns the inertia properties of the structure based on the object's mass and on the existing geometry
* The mass is assumed to have been set previously.
* Mass = mass of the structure, excluding point masses
* @param  &CoG a reference to the CoG point, as a result of the calculation
* @param  &CoGIxx xx axis component of the inertia tensor, calculated at the CoG
* @param  &CoGIyy yy axis component of the inertia tensor, calculated at the CoG
* @param  &CoGIzz zz axis component of the inertia tensor, calculated at the CoG
* @param  &CoGIxz xz axis component of the inertia tensor, calculated at the CoG
*/
void Wing::ComputeVolumeInertia(CVector &CoG, double &CoGIxx, double &CoGIyy, double &CoGIzz, double &CoGIxz)
{
	static double ElemVolume[NXSTATIONS*NYSTATIONS*MAXSPANSECTIONS*2];
	static CVector PtVolume[NXSTATIONS*NYSTATIONS*MAXSPANSECTIONS*2];
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

	ComputeGeometry();

	//the mass density is assumed to be homogeneous

	//the local weight is proportional to the chord x foil area
	//the foil's area is interpolated

	//we consider the whole wing, i.e. all left and right surfaces
	//note : in avl documentation, each side is considered separately

	//first get the CoG - necessary for future application of Huygens/Steiner theorem
	int p = 0;

	for (j=0; j<m_Surface.size(); j++)
	{
		LocalSpan = m_Surface.at(j)->m_Length/(double)NYSTATIONS;
		for (k=0; k<NYSTATIONS; k++)
		{
			tau  = (double)k     / (double)NYSTATIONS;
			tau1 = (double)(k+1) / (double)NYSTATIONS;
			yrel = (tau+tau1)/2.0;

			m_Surface.at(j)->GetSection(tau,  LocalChord,  LocalArea,  Pt);
			m_Surface.at(j)->GetSection(tau1, LocalChord1, LocalArea1, Pt1);
			LocalVolume = (LocalArea+LocalArea1)/2.0 * LocalSpan;
			PtC4.x = (Pt.x + Pt1.x)/2.0;
			PtC4.y = (Pt.y + Pt1.y)/2.0;
			PtC4.z = (Pt.z + Pt1.z)/2.0;

//			CoGCheck += LocalVolume * PtC4;
			for(l=0; l<NXSTATIONS; l++)
			{
				//browse mid-section
				xrel  = 1.0 - 1.0/2.0 * (1.0-cos((double) l   *PI /(double)NXSTATIONS));
				xrel1 = 1.0 - 1.0/2.0 * (1.0-cos((double)(l+1)*PI /(double)NXSTATIONS));

				m_Surface.at(j)->GetSurfacePoint(xrel, xrel, yrel, ATop,  1);
				m_Surface.at(j)->GetSurfacePoint(xrel, xrel, yrel, ABot, -1);
				m_Surface.at(j)->GetSurfacePoint(xrel1, xrel1, yrel, CTop,  1);
				m_Surface.at(j)->GetSurfacePoint(xrel1, xrel1, yrel, CBot, -1);
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
	for (j=0; j<m_Surface.size(); j++)
	{
		LocalSpan = m_Surface.at(j)->m_Length/(double)NYSTATIONS;
		for (k=0; k<NYSTATIONS; k++)
		{
			tau  = (double)k     / (double)NYSTATIONS;
			tau1 = (double)(k+1) / (double)NYSTATIONS;
			m_Surface.at(j)->GetSection(tau,  LocalChord,  LocalArea,  Pt);
			m_Surface.at(j)->GetSection(tau1, LocalChord1, LocalArea1, Pt1);

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

			for(l=0; l<NXSTATIONS; l++)
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


/**
* Calculates the inertia tensor in geometrical (body) axis :
*  - adds the volume inertia AND the inertia of point masses of all components
*  - the body axis is the frame in which the geometry has been defined
*  - the origin is the Wing's CoG, taking into account all masses
*/
void Wing::ComputeBodyAxisInertia()
{
	CVector LA, VolumeCoG;
	double Ixx, Iyy, Izz, Ixz, VolumeMass;
	Ixx = Iyy = Izz = Ixz = VolumeMass = 0.0;

	//Get the volume inertia properties in the volume CoG frame of reference
	ComputeVolumeInertia(VolumeCoG, Ixx, Iyy, Izz, Ixz);
	m_TotalMass = m_VolumeMass;

	m_CoG = VolumeCoG *m_VolumeMass;

	// add point masses
	for(int im=0; im<m_PointMass.size(); im++)
	{
		m_TotalMass += m_PointMass[im]->mass();
		m_CoG       += m_PointMass[im]->position() * m_PointMass[im]->mass();
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
	for(int im=0; im<m_PointMass.size(); im++)
	{
		LA = m_PointMass[im]->position() - m_CoG;
		m_CoGIxx += m_PointMass[im]->mass() * (LA.y*LA.y + LA.z*LA.z);
		m_CoGIyy += m_PointMass[im]->mass() * (LA.x*LA.x + LA.z*LA.z);
		m_CoGIzz += m_PointMass[im]->mass() * (LA.x*LA.x + LA.y*LA.y);
		m_CoGIxz -= m_PointMass[im]->mass() * (LA.x*LA.z);
	}
}


/**
 * Constructs the surface objects based on the WingSection data.
 * The position and orientation are defined in the plane object
 * The surfaces are constructed from root to tip, and re-ordered from let tip to right tip
 * One surface object for each of the wing's panels
 * A is the surface's left side, B is the right side
 * @param T the translation to be appied to the wing geometry
 * @param XTilt  the rotation in degrees around the x-axis; used in the case of fins
 * @param YTilt  the rotation in degrees arouns the y-axi; used for wing or elevator tilt
 */
void Wing::CreateSurfaces(CVector const &T, double XTilt, double YTilt)
{
	int nSurf;
	CVector PLA, PTA, PLB, PTB, offset, T1;
	CVector Trans(T.x, 0.0, T.z);
	CVector O(0.0,0.0,0.0);
	CVector *VNormal = new CVector[NWingSection()+1];
	CVector *VNSide  = new CVector[NWingSection()+1];
	double MinPanelSize;


	if(s_MinPanelSize>0.0) MinPanelSize = s_MinPanelSize;
	else                              MinPanelSize = 0.0;

	m_MatSize = 0;

	//define the normal to each surface
	nSurf=0;
	VNormal[0].Set(0.0, 0.0, 1.0);
	VNSide[0].Set(0.0, 0.0, 1.0);

	for(int is=0; is<NWingSection()-1;is++)
	{
		if (qAbs(YPosition(is)-YPosition(is+1)) > MinPanelSize)
		{
			VNormal[nSurf].Set(0.0, 0.0, 1.0);
			VNormal[nSurf].RotateX(O, Dihedral(is));
			nSurf++;
		}
	}

	ClearSurfaces();
	for(int jss=0; jss<nSurf; jss++)
	{
		m_Surface.append(new Surface);
	}

	if(!m_bIsFin || (m_bIsFin && m_bSymFin) || (m_bIsFin && m_bDoubleFin))
	{
		for(int jss=0; jss<nSurf; jss++)
		{
			m_Surface.append(new Surface);
		}
	}


	int NSurfaces = nSurf;
	//define the normals at panel junctions : average between the normals of the two connecting sufaces
	for(int jss=0; jss<nSurf; jss++)
	{
		VNSide[jss+1] = VNormal[jss]+VNormal[jss+1];
		VNSide[jss+1].Normalize();
	}

	//we start with the center panel, moving towards the left wing tip
	//however, the calculations are written for surfaces ordered from left tip to right tip,
	//so we number them the opposite way
	nSurf = 0;
	int iSurf = NSurfaces-1;


	for(int jss=0; jss<NWingSection()-1; jss++)
	{
		if(qAbs(YPosition(jss)-YPosition(jss+1)) > MinPanelSize)
		{
			m_Surface[iSurf]->m_pFoilA   = Foil::foil(LeftFoil(jss+1));
			m_Surface[iSurf]->m_pFoilB   = Foil::foil(LeftFoil(jss));

			m_Surface[iSurf]->m_Length   =  YPosition(jss+1) - YPosition(jss);

			PLA.x =  Offset(jss+1);         PLB.x =  Offset(jss);
			PLA.y = -YPosition(jss+1);      PLB.y = -YPosition(jss);
			PLA.z =  0.0;                   PLB.z =  0.0;
			PTA.x =  PLA.x+Chord(jss+1);    PTB.x = PLB.x+Chord(jss);
			PTA.y =  PLA.y;	                PTB.y = PLB.y;
			PTA.z =  0.0;                   PTB.z =  0.0;

			m_Surface[iSurf]->m_LA.Copy(PLA);
			m_Surface[iSurf]->m_TA.Copy(PTA);
			m_Surface[iSurf]->m_LB.Copy(PLB);
			m_Surface[iSurf]->m_TB.Copy(PTB);

			m_Surface[iSurf]->SetNormal(); // is (0,0,1)

			m_Surface[iSurf]->RotateX(m_Surface[iSurf]->m_LB, -Dihedral(jss));
			m_Surface[iSurf]->NormalA.Set(VNSide[nSurf+1].x, -VNSide[nSurf+1].y, VNSide[nSurf+1].z);
			m_Surface[iSurf]->NormalB.Set(VNSide[nSurf].x,   -VNSide[nSurf].y,   VNSide[nSurf].z);

			m_Surface[iSurf]->m_TwistA   =  Twist(jss+1);
			m_Surface[iSurf]->m_TwistB   =  Twist(jss);
			m_Surface[iSurf]->SetTwist1();


			if(jss>0)
			{
				//translate the surface to the left tip of the previous surface
				T1 = m_Surface[iSurf+1]->m_LA - m_Surface[iSurf]->m_LB;
				m_Surface[iSurf]->Translate(0.0,T1.y,T1.z);
//				m_Surface[is].m_LB = m_Surface[is+1].m_LA;
//				m_Surface[is].m_TB = m_Surface[is+1].m_TA;
			}

			nSurf++;

			m_Surface[iSurf]->m_NXPanels = NXPanels(jss);
			m_Surface[iSurf]->m_NYPanels = NYPanels(jss);


			//AVL coding + invert sine and -sine for left wing
			m_Surface[iSurf]->m_XDistType = XPanelDist(jss);
			if(YPanelDist(jss) ==SINE)               m_Surface[iSurf]->m_YDistType = INVERSESINE;
			else if(YPanelDist(jss) ==  COSINE)      m_Surface[iSurf]->m_YDistType =  COSINE;
			else if(YPanelDist(jss) == INVERSESINE)  m_Surface[iSurf]->m_YDistType =  SINE;
			else                                     m_Surface[iSurf]->m_YDistType =  UNIFORM;

			m_Surface[iSurf]->CreateXPoints();
			m_Surface[iSurf]->SetFlap();
			m_Surface[iSurf]->Init();
			m_Surface[iSurf]->m_bIsLeftSurf   = true;
			m_Surface[iSurf]->m_bIsInSymPlane = false;
			--iSurf;
		}
	}
	m_Surface[NSurfaces-1]->m_bIsCenterSurf = true;//previous left center surface

	// we only need a right wing in the following cases
	//   - if it's an 'ordinary wing'
	//   - if it's a fin, symetrical about the fuselage x-axis
	//   - if it's a symetrical double fin
	if(!m_bIsFin || (m_bIsFin && m_bSymFin) || (m_bIsFin && m_bDoubleFin))
	{
		m_Surface[NSurfaces]->m_bIsCenterSurf   = true;//next right center surface
		iSurf = nSurf;
		for (int jss=0; jss<NWingSection()-1; jss++)
		{
			if (qAbs(YPosition(jss)-YPosition(jss+1)) > MinPanelSize)
			{
				m_Surface[iSurf]->m_pFoilA   = Foil::foil(RightFoil(jss));
				m_Surface[iSurf]->m_pFoilB   = Foil::foil(RightFoil(jss+1));

				m_Surface[iSurf]->m_Length   =  YPosition(jss+1) - YPosition(jss);

				PLA.x = Offset(jss);        PLB.x = Offset(jss+1);
				PLA.y = YPosition(jss);     PLB.y = YPosition(jss+1);
				PLA.z = 0.0;                PLB.z = 0.0;
				PTA.x = PLA.x+Chord(jss);   PTB.x = PLB.x+Chord(jss+1);
				PTA.y = PLA.y;              PTB.y = PLB.y;
				PTA.z = 0.0;                PTB.z = 0.0;

				m_Surface[iSurf]->m_LA.Copy(PLA);
				m_Surface[iSurf]->m_TA.Copy(PTA);
				m_Surface[iSurf]->m_LB.Copy(PLB);
				m_Surface[iSurf]->m_TB.Copy(PTB);

				m_Surface[iSurf]->SetNormal(); // is (0,0,1)

				m_Surface[iSurf]->RotateX(m_Surface[iSurf]->m_LA, Dihedral(jss));
				m_Surface[iSurf]->NormalA.Set(VNSide[iSurf-nSurf].x,   VNSide[iSurf-nSurf].y,   VNSide[iSurf-nSurf].z);
				m_Surface[iSurf]->NormalB.Set(VNSide[iSurf-nSurf+1].x, VNSide[iSurf-nSurf+1].y, VNSide[iSurf-nSurf+1].z);

				m_Surface[iSurf]->m_TwistA   =  Twist(jss);
				m_Surface[iSurf]->m_TwistB   =  Twist(jss+1);
				m_Surface[iSurf]->SetTwist1();

				if(jss>0)
				{
					//translate the surface to the left tip of the previous surface and merge points
					T1 = m_Surface[iSurf-1]->m_LB - m_Surface[iSurf]->m_LA ;
					m_Surface[iSurf]->Translate(0.0, T1.y, T1.z);
//					m_Surface[is].m_LA = m_Surface[is-1].m_LB;
//					m_Surface[is].m_TA = m_Surface[is-1].m_TB;
				}

				m_Surface[iSurf]->m_NXPanels = NXPanels(jss);
				m_Surface[iSurf]->m_NYPanels = NYPanels(jss);

				//AVL coding + invert sine and -sine for left wing
				m_Surface[iSurf]->m_XDistType = XPanelDist(jss);
				m_Surface[iSurf]->m_YDistType = YPanelDist(jss);

				m_Surface[iSurf]->CreateXPoints();
				m_Surface[iSurf]->SetFlap();
				m_Surface[iSurf]->Init();
				m_Surface[iSurf]->m_bIsLeftSurf   = false;
				m_Surface[iSurf]->m_bIsRightSurf  = true;
				m_Surface[iSurf]->m_bIsInSymPlane = false;
				iSurf++;
			}
		}
	}

	CVector Or(0.0,0.0,0.0);
	if(!m_bIsFin || (m_bIsFin && m_bSymFin))
	{
		NSurfaces*=2;
		for (int jSurf=0; jSurf<NSurfaces; jSurf++)
		{
			m_Surface[jSurf]->RotateX(Or, XTilt);
			m_Surface[jSurf]->RotateY(Or, YTilt);
			m_Surface[jSurf]->Translate(Trans);
			if(m_bIsFin && m_bSymFin)
			{
				m_Surface[jSurf]->m_bIsInSymPlane = true;
				m_Surface[jSurf]->m_bIsLeftSurf   = true;
				m_Surface[jSurf]->m_bIsRightSurf  = false;
			}
		}
	}
	else
	{
		if(m_bDoubleFin)
		{
			NSurfaces*=2;
			//rotate surfaces symetrically
			int ns2 = (int)(NSurfaces/2);
			offset.Set(0.0, -T.y, 0.0);
			for(int jSurf=0; jSurf<ns2; jSurf++)
			{
				m_Surface[jSurf]->RotateX(Or, +XTilt);
				m_Surface[jSurf]->RotateZ(Or, YTilt);
				m_Surface[jSurf]->Translate(Trans);
				m_Surface[jSurf]->Translate(offset);
				m_Surface[jSurf]->m_bIsInSymPlane = false;
			}
			offset.y = -offset.y;
			for(int jSurf=ns2; jSurf< NSurfaces; jSurf++)
			{
				m_Surface[jSurf]->RotateX(Or, -XTilt);
				m_Surface[jSurf]->RotateZ(Or, -YTilt);
				m_Surface[jSurf]->Translate(Trans);
				m_Surface[jSurf]->Translate(offset);
				m_Surface[jSurf]->m_bIsInSymPlane = false;
			}
			m_Surface[ns2-1]->m_bIsTipRight = true;
			m_Surface[ns2-1]->m_bIsTipLeft  = true;
			m_Surface[0]->m_bIsTipRight   = true;
			m_Surface[0]->m_bIsTipLeft    = true;

			m_Surface[ns2]->m_bIsTipRight        = true;
			m_Surface[ns2]->m_bIsTipLeft         = true;
			m_Surface[NSurfaces-1]->m_bIsTipRight  = true;
			m_Surface[NSurfaces-1]->m_bIsTipLeft   = true;
		}
		else
		{
			//Not a double fin, so just a simple single-sided fin
			for (int jSurf=0; jSurf<NSurfaces; jSurf++)
			{
				m_Surface[jSurf]->RotateX(Or, XTilt);
				m_Surface[jSurf]->RotateZ(Or, YTilt);
				m_Surface[jSurf]->Translate(Trans);
				m_Surface[jSurf]->m_bIsLeftSurf   = true;
				m_Surface[jSurf]->m_bIsRightSurf  = false;
				m_Surface[jSurf]->m_bIsInSymPlane = true;
			}
		}
	}

	m_Surface[0]->m_bIsTipLeft              = true;
	if(NSurfaces>=1) m_Surface[NSurfaces-1]->m_bIsTipRight = true;

	if(NSurfaces>1) m_Surface[(int)(NSurfaces/2)-1]->m_bJoinRight   = true;
	//check for a center gap greater than 1/10mm
	if(YPosition(0)>0.0001) 	m_Surface[(int)(NSurfaces/2)-1]->m_bJoinRight   = false;

	if(m_bIsFin && m_bDoubleFin) m_Surface[(int)(NSurfaces/2)-1]->m_bJoinRight   = false;

	m_bWingOut = false;

	delete [] VNormal;
	delete [] VNSide;
}



/**
* Calculates the chord lengths at each position of the NStation defined by the LLT or the Panel analysis
*@param NStation the number of stations required by the analysis
*/
void Wing::ComputeChords(int NStation)
{
	int j,k,m;
	double y, yob, tau;
	double x0,y0,y1,y2;
	CVector C;

	int NSurfaces = m_Surface.size();

	if(NStation !=0)
	{//LLT based
		m_NStation = NStation;

		for (k=0; k<=NStation; k++)
		{
			yob   = cos(k*PI/NStation);
			y = qAbs(yob * m_PlanformSpan/2);
			for (int is=0; is<NWingSection(); is++)
			{
				if(YPosition(is) < y && y <=YPosition(is+1))
				{
					tau = (y-YPosition(is))/(YPosition(is+1)-YPosition(is));
					m_Chord[k]  = Chord(is)+(Chord(is+1)-Chord(is)) * tau;
					m_Offset[k] = Offset(is)+(Offset(is+1)-Offset(is)) * tau;
					break;
				}
			}
		}
	}
	else
	{
		//VLM Mesh based
		QList<double> SpanPosition;
		m_NStation = 0;
		m = 0;

		x0 = m_Surface[NSurfaces/2]->m_LA.x;
		y0 = m_Surface[NSurfaces/2]->m_LA.y;

		for (j=NSurfaces/2; j<NSurfaces; j++)
		{
			for (k=0; k<m_Surface.at(j)->m_NYPanels; k++)
			{
				//calculate span positions at each station
				m_Surface.at(j)->GetyDist(k, y1, y2);
				SpanPosition.append(y0 + (y1+y2)/2.0*m_Surface.at(j)->m_Length);
				m++;
			}
			y0+=m_Surface.at(j)->m_Length;
		}

		m_NStation = 2*m;
		for (m=0; m<m_NStation/2; m++)
		{
			m_SpanPos[m] = -SpanPosition[m_NStation/2-m-1];
			m_SpanPos[m+m_NStation/2] = SpanPosition[m];
		}

		m=0;
		for (j=0; j<NSurfaces; j++)
		{
			for (k=0; k<m_Surface.at(j)->m_NYPanels; k++)
			{
				//calculate chords and offsets at each station
				m_Chord[m]     = m_Surface.at(j)->GetChord(k);
//				m_StripArea[m] = m_Chord[m]* m_Surface.at(j)->Getdl(k);

				m_Surface.at(j)->GetLeadingPt(k, C);
				m_Offset[m] = C.x-x0;

				m_Twist[m]  = m_Surface.at(j)->GetTwist(k);
				m++;
			}
		}
		m_NStation = m;
	}
}



/**
* Calculates the chord lengths at each position of the NStation defined by the LLT or the Panel analysis
* Overloaded function
*@param NStation the number of stations required by the analysis
*@param *chord pointer to the array of chords lengths at the span stations
*@param *offset pointer to the array of offset positions at the span stations
*@param *twist pointer to the array of twist values at the span stations
*/
void Wing::ComputeChords(int NStation, double *chord, double *offset, double *twist)
{
	double y, yob, tau;

	if(NStation !=0)
	{//LLT based
		m_NStation = NStation;

		for (int k=0; k<=NStation; k++)
		{
			yob   = cos(k*PI/NStation);
			y = qAbs(yob * m_PlanformSpan/2);
			for (int is=0; is<NWingSection(); is++)
			{
				if(YPosition(is) < y && y <=YPosition(is+1))
				{
					tau = (y-YPosition(is))/(YPosition(is+1)-YPosition(is));
					chord[k]  = Chord(is)  + (Chord(is+1) -Chord(is))  * tau;
					offset[k] = Offset(is) + (Offset(is+1)-Offset(is)) * tau;
					twist[k]  = Twist(is)  + (Twist(is+1) -Twist(is))  * tau;;
					break;
				}
			}
		}
	}
}


/**
* Copies the gemetrical data from an existing Wing
*@param pWing a pointer to the instance of the source Wing object
*/
void Wing::Duplicate(Wing *pWing)
{
	m_NStation      = pWing->m_NStation;
	m_PlanformSpan  = pWing->m_PlanformSpan;
	m_ProjectedSpan = pWing->m_ProjectedSpan;
	m_PlanformArea  = pWing->m_PlanformArea;
	m_ProjectedArea = pWing->m_ProjectedArea;
	m_AR            = pWing->m_AR;
	m_TR            = pWing->m_TR;
	m_GChord        = pWing->m_GChord;
	m_MAChord       = pWing->m_MAChord;
	m_WingName      = pWing->m_WingName;
	m_bSymetric     = pWing->m_bSymetric;
	m_bIsFin        = pWing->m_bIsFin;
	m_bSymFin       = pWing->m_bSymFin;
	m_bDoubleFin    = pWing->m_bDoubleFin;
	m_bDoubleSymFin = pWing->m_bDoubleSymFin;

	ClearWingSections();

	for (int is=0; is<pWing->m_WingSection.size(); is++)
	{
		AppendWingSection();
		Chord(is)      = pWing->Chord(is);
		YPosition(is)  = pWing->YPosition(is);
		Offset(is)     = pWing->Offset(is);
		Length(is)     = pWing->Length(is);
		NXPanels(is)   = pWing->NXPanels(is) ;
		NYPanels(is)   = pWing->NYPanels(is);
		XPanelDist(is) = pWing->XPanelDist(is);
		YPanelDist(is) = pWing->YPanelDist(is);
		Twist(is)      = pWing->Twist(is);
		Dihedral(is)   = pWing->Dihedral(is);
		ZPosition(is)  = pWing->ZPosition(is);
		YProj(is)      = pWing->YProj(is);

		RightFoil(is)  = pWing->RightFoil(is);
		LeftFoil(is)   = pWing->LeftFoil(is);
	}


	m_nFlaps = pWing->m_nFlaps;

	m_VolumeMass = pWing->m_VolumeMass;
	m_TotalMass  = pWing->m_TotalMass;
	m_CoG = pWing->m_CoG;
	m_CoGIxx = pWing->m_CoGIxx;
	m_CoGIyy = pWing->m_CoGIyy;
	m_CoGIzz = pWing->m_CoGIzz;
	m_CoGIxz = pWing->m_CoGIxz;

	ClearPointMasses();

	for(int im=0; im<pWing->m_PointMass.size();im++)
	{
		m_PointMass.append(new PointMass(pWing->m_PointMass[im]->mass(), pWing->m_PointMass[im]->position(), pWing->m_PointMass[im]->tag()));
	}

	m_WingDescription = pWing->m_WingDescription;
	m_WingColor       = pWing->m_WingColor;
}



/**
 * Export the wing geometry to a text file readable by AVL.
 * @param out the instance of the QTextStream to which the output will be directed
 * @param index a reference number used by AVL to idnitfy the wing
 * @param x the x value of the translation to be applied to the wing's geometry -unused
 * @param y the y value of the translation to be applied to the wing's geometry
 * @param z the z value of the translation to be applied to the wing's geometry -unused
 * @param Thetax the rotation about the x-axis to be applied to the geometry -unused
 * @param Thetay the rotation about the y-axis to be applied to the geometry
 * @return true if successful, false otherwise
 */
bool Wing::ExportAVLWing(QTextStream &out, int index, double y, double Thetay)
{
	int j;
	QString strong, str;

	out << ("#=================================================\n");
	out << ("SURFACE                      | (keyword)\n");
	out << (m_WingName);
	out << ("\n");
	out << ("#Nchord    Cspace   [ Nspan Sspace ]\n");

	strong = QString("%1        %2\n").arg( NXPanels(0)).arg(1.0,3,'f',1);
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
	Surface ASurface;
	int iFlap = 1;

	int NSurfaces = m_Surface.size();


	int startIndex = (m_bIsFin ? 0 : (int)(NSurfaces/2));

	for(j=startIndex; j<NSurfaces; j++)
	{
		ASurface.Copy(m_Surface.at(j));

		//Remove the twist, since AVL processes it as a mod of the angle of attack thru the dAInc command
		ASurface.m_TwistA = ASurface.m_TwistB = 0.0;
		ASurface.SetTwist1();
		out << ("#______________\nSECTION                                                     |  (keyword)\n");

		strong = QString("%1 %2 %3 %4 %5  %6  %7   | Xle Yle Zle   Chord Ainc   [ Nspan Sspace ]\n")
				 .arg(ASurface.m_LA.x          *Units::mtoUnit(),9,'f',4)
				 .arg(ASurface.m_LA.y          *Units::mtoUnit(),9,'f',4)
				 .arg(ASurface.m_LA.z          *Units::mtoUnit(),9,'f',4)
				 .arg(ASurface.GetChord(0.0)   *Units::mtoUnit(),9,'f',4)
				 .arg(m_Surface.at(j)->m_TwistA,7,'f',3)
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
			if(qAbs(mean_angle)>0.0) str = QString("%1  ").arg(1.0/mean_angle,5,'f',2);
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
				 .arg(ASurface.m_LB.x          *MainFrame::m_mtoUnit,9,'f',4)
				 .arg(ASurface.m_LB.y          *MainFrame::m_mtoUnit,9,'f',4)
				 .arg(ASurface.m_LB.z          *MainFrame::m_mtoUnit,9,'f',4)
				 .arg(ASurface.GetChord(1.0)   *MainFrame::m_mtoUnit,9,'f',4)
				 .arg(m_Surface.at(j)->m_TwistB,7,'f',3)
				 .arg(ASurface.NYPanels,3)
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

			if(qAbs(mean_angle)>0.0) str = QString("%1  ").arg(1.0/mean_angle,5,'f',2);
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
				 .arg(ASurface.m_LB.x          *Units::mtoUnit(),9,'f',4)
				 .arg(ASurface.m_LB.y          *Units::mtoUnit(),9,'f',4)
				 .arg(ASurface.m_LB.z          *Units::mtoUnit(),9,'f',4)
				 .arg(ASurface.GetChord(1.0)   *Units::mtoUnit(),9,'f',4)
				 .arg(m_Surface[j-1]->m_TwistB,7,'f',3)
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

/**
* Returns the wing's average sweep from root to tip measured at the quarter chord
* The sweep is calulated as the arctangent of the root and tip quarter-chord points
*@return the value of the average sweep, in degrees
*/
double Wing::AverageSweep()
{
	double xroot = Chord(0)/4.0;
	double xtip  = TipOffset() + TipChord()/4.0;
//	double sweep = (atan2(xtip-xroot, m_PlanformSpan/2.0)) * 180.0/PI;
	return (atan2(xtip-xroot, m_PlanformSpan/2.0)) * 180.0/PI;
}


/**
 * Returns the x-position of the quarter-chord point at a given span position, relative to a reference x-value
 *@param yob the span position where the quarter-chord point will be calculated
 *@param xRef the reference position
 *@return the quarter-chord position
 */
double Wing::C4(double yob, double xRef)
{
	double chord, offset, tau;
	double C4 = 0.0;
	double y = qAbs(yob*m_PlanformSpan/2.0);
	for(int is=0; is<NWingSection()-1; is++)
	{
		if(YPosition(is)<= y && y <=YPosition(is+1))
		{
			tau = (y - YPosition(is))/(YPosition(is+1)-YPosition(is));
			chord  = Chord(is)  + tau * (Chord(is+1) - Chord(is));
			offset = Offset(is) + tau * (Offset(is+1) - Offset(is));
			C4 = offset + chord/4.0 - xRef;
			return C4;
		}
	}
	return C4;
}

/**
 * Calculates and returns the chord length at a given relative span position
 * @param yob the relative span position in %, where the chord length will be calculated
 * @return the chord length
 */
double Wing::getChord(double yob)
{
	double chord = 0.0;
	double tau;
	double y;

	y= qAbs(yob*m_PlanformSpan/2.0);//geometry is symetric
	for(int is=0; is<NWingSection()-1; is++)
	{
		if(YPosition(is)<=y && y <=YPosition(is+1))
		{
			tau = (y - YPosition(is))/(YPosition(is+1)-YPosition(is));
			chord = Chord(is) + tau * (Chord(is+1) - Chord(is));
			return chord;
		}
	}
	return -1.0;
}


/**
 * Calculates and returns the offste value at a given relative span position
 * @param yob the relative span position in %, where the offset will be calculated
 * @return the offset value
 */
double Wing::getOffset(double yob)
{
	double tau, y;
	double offset = 0.0;

	y= qAbs(yob*m_PlanformSpan/2.0);
	for(int is=0; is<NWingSection()-1; is++)
	{
		if(YPosition(is)<= y && y <=YPosition(is+1))
		{
			tau = (y - YPosition(is))/(YPosition(is+1)-YPosition(is));
			offset = Offset(is) + tau * (Offset(is+1) - Offset(is));
			return offset;
		}
	}

	return -1.0;
}



/**
 * Calculates and returns the twist angle at a given relative span position
 * @param yob the relative position where the twist angle will be calculated
 * @return the twist angle in degrees
 */
double Wing::getTwist(double yob)
{
	double tau;

	double y= qAbs(yob*m_PlanformSpan/2.0);//geometry is symetric

	// calculate twist at each station
	if (y>=0.0)
	{
		//right wing
		for (int is=0; is<NWingSection()-1; is++)
		{
			if(YPosition(is) <= y && y <=YPosition(is+1))
			{
				tau = (y-YPosition(is))/(YPosition(is+1)-YPosition(is));
				return Twist(is)+(Twist(is+1)-Twist(is)) * tau;
			}
		}
	}
	return 0.0;
}



/**
 * Calculates and returns the dihedral angle at a given relative span position
 * @param yob the relative position where the dihedral angle will be calculated
 * @return the dihedral angle in degrees
 */
double Wing::getDihedral(double yob)
{
	double y= qAbs(yob*m_PlanformSpan/2.0);//geometry is symetric
	for(int is=0; is<NWingSection()-1; is++)
	{
		if(YPosition(is)<= y && y <=YPosition(is+1))
		{
			if(yob>=0) return Dihedral(is);
			else  return -Dihedral(is);
		}
	}
	return 0.0;
}

/**
 * Returns pointers to the left and right foils of a given span position, and the relative position of the span position between these two foils
 * @param pFoil0 the pointer to the pointer of the left foil
 * @param pFoil1 the pointer to the pointer of the right foil
 * @param y the reference span position
 * @param t the ratio between the position of the two foils
 */
void Wing::GetFoils(Foil **pFoil0, Foil **pFoil1, double y, double &t)
{
	if  (y>0.0)
	{
		//search Right wing
		for (int is=0; is<NWingSection()-1; is++)
		{
			if (YPosition(is)<=y && y<=YPosition(is+1))
			{

				*pFoil0 = Foil::foil(RightFoil(is));
				*pFoil1 = Foil::foil(RightFoil(is+1));
				t = (y-YPosition(is))/(YPosition(is+1) - YPosition(is));
				return;
			}
		}
	}
	else
	{
		//search left wing
		y = -y;
		for (int is=0; is<NWingSection()-1; is++)
		{
			if (YPosition(is)<=y && y<YPosition(is+1))
			{
				*pFoil0 = Foil::foil(LeftFoil(is));
				*pFoil1 = Foil::foil(LeftFoil(is+1));
				t = (y-YPosition(is))/(YPosition(is+1) - YPosition(is));
				return;
			}
		}
	}
	t = 0;
	pFoil0 = NULL;// use linear
	pFoil1 = NULL;// use linear
}




/**
 * Returns the total mass of the wing, as the sum of volume and point masses
 * @return the total mass
 */
double Wing::TotalMass()
{
	double TotalMass = m_VolumeMass;
	for(int im=0; im<m_PointMass.size(); im++)
		TotalMass += m_PointMass[im]->mass();
	return TotalMass;
}




/**
*Returns the wing's absolute positions yv and zv from the relative value xrel and the planform span y
*Used for display purposes only
*@param xrel the relative position along the chord, in %
*@param y the planform (2D) span position where the y and z positions will be calculated
*@param &yv the 3D y-position of the point
*@param &zv the 3D z-position of the point
*/
void Wing::GetViewYZPos(double xrel, double y, double &yv, double &zv, int pos)
{
	double tau;
	double twist, chord;
	double z0, z1, nx, ny;
	zv = 0.0;
	yv = 0.0;
	double fy = qAbs(y);
	double sign;
	if(fy<1.0e-10) sign = 1.0;
	else sign = y/fy;

//	if(fy<=0.0) return 0.0;
	for (int is=0; is<NWingSection()-1; is++)
	{
		if(YPosition(is)< fy && fy<=YPosition(is+1))
		{
			for (int ks=0; ks<is; ks++)
			{
				//TODO Potential bug, check original
				yv += Length(ks+1) * cos(Dihedral(ks)*PI/180.0);
				zv += Length(ks+1) * sin(Dihedral(ks)*PI/180.0);
			}
			tau = (fy - YPosition(is))/(YPosition(is+1)-YPosition(is));
			yv += tau * Length(is+1) * cos(Dihedral(is)*PI/180.0);
			zv += tau * Length(is+1) * sin(Dihedral(is)*PI/180.0);

			yv *= sign;
			//	add washout calculated about chord quarter line :
			twist = getTwist(fy)*PI/180.;
			chord = getChord(fy*2./m_PlanformSpan);
			zv -= (xrel-0.25)*chord*sin(twist);

			Foil *pFoil0 = NULL;
			Foil *pFoil1 = NULL;
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

/**
 * Returns the relative position in % of a given absolute span position
 * @param SpanPos the absolute span position
 * @return the relative position, in %
 */
double Wing::yrel(double SpanPos)
{
	double y = qAbs(SpanPos);
	for(int is=0; is<NWingSection()-1; is++)
	{
		if(YPosition(is)<=y && y <YPosition(is+1))
		{
			if(SpanPos>0) return  (y-YPosition(is))/(YPosition(is+1)-YPosition(is));
			else          return  (y-YPosition(is+1))/(YPosition(is)-YPosition(is+1));
		}
	}
	return 1.0;
}

/**
 * The z-position of a specified absolute span position.
 * Used for moment evaluations in LLT, where the wing is defined as a 2D planform
 * @param y the abolute span position
 * @return the absolute z-position
 */
double Wing::ZPosition(double y)
{
	double tau;
	double ZPos =0.0;

	y= qAbs(y);
	if(y<=0.0) return 0.0;
	for (int is=0; is<NWingSection()-1; is++)
	{
		if(YPosition(is)< y && y<=YPosition(is+1))
		{
			for (int iss=0; iss<is; iss++)
			{
				ZPos+=Length(iss+1) * sin(Dihedral(iss)*PI/180.0);
			}
			tau = (y - YPosition(is))/(YPosition(is+1)-YPosition(is));
			ZPos += tau * Length(is+1) * sin(Dihedral(is)*PI/180.0);
			return ZPos;
		}
	}
	return 0.0;
}



/**
* Calculates the induced lift and drag from the vortices or wake panels strength using a farfield method
* Downwash is evaluated at a distance 100 times the span downstream (i.e. infinite)
*/
void Wing::PanelTrefftz(void *pAnalysis, double QInf, double Alpha, double *Mu, double *Sigma, int pos, CVector &Force, double &WingIDrag,
						 WPolar *pWPolar, Panel *pWakePanel, CVector *pWakeNode)
{
	PanelAnalysis *pPanelAnalysis = (PanelAnalysis*)pAnalysis;

	int nw, iTA, iTB;
	int j, k, l, p, pp, m;
	double InducedAngle, cosa, sina;
	QList<double> GammaStrip;
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

	m_WingCL      = 0.0;
	WingIDrag = 0.0;


	int coef = 2;
	if (pWPolar->m_bThinSurfaces) coef = 1;

	int NSurfaces = m_Surface.size();
//	if(s_bVLMSymetric)
//	{
//		if(!m_bIsFin || (m_bIsFin && m_bDoubleFin))		NSurfaces=m_NSurfaces/2;
//	}

	p=0;
	m=0;
	for (j=0; j<NSurfaces; j++)
	{
		if(m_Surface.at(j)->m_bIsTipLeft && !pWPolar->m_bThinSurfaces) p+=m_Surface.at(j)->m_NXPanels;//tip patch panels

		for (k=0; k<m_Surface.at(j)->m_NYPanels; k++)
		{
			pp = p;
			m_StripArea[m] = 0.0;
			StripForce.Set(0.0,0.0,0.0);
			for (l=0; l<coef*m_Surface.at(j)->m_NXPanels; l++)
			{
				m_StripArea[m]  += m_pWingPanel[pp].Area;
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
				nw  = m_pWingPanel[p].m_iWake;
				iTA = pWakePanel[nw].m_iTA;
				iTB = pWakePanel[nw].m_iTB;
				C = (pWakeNode[iTA] + pWakeNode[iTB])/2.0;
				pPanelAnalysis->GetSpeedVector(C, Mu, Sigma, Wg, false);
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
				GammaStrip.append((-Mu[pos+p+coef*m_Surface.at(j)->m_NXPanels-1] + Mu[pos+p]) *4.0*PI);
				Wg += VInf;

				StripForce  = m_pWingPanel[p].Vortex * Wg;
				StripForce *= GammaStrip.at(m) * pWPolar->m_Density / q;  // N/q

				//____________________________
				// Project on wind axes
				m_Cl[m]    = StripForce.dot(WindNormal)   /m_StripArea[m];
				m_ICd[m]   = StripForce.dot(WindDirection)/m_StripArea[m];
				WingIDrag += StripForce.dot(WindDirection);          // N/q
			}
			else
			{
				pp=p;
				for(l=0; l<m_Surface.at(j)->m_NXPanels; l++)
				{
					if(pWPolar->m_bVLM1 || m_pWingPanel[pp].m_bIsTrailing)
					{
						C = m_pWingPanel[pp].CtrlPt;
						C.x = m_PlanformSpan * 1000.0;

						pPanelAnalysis->GetSpeedVector(C, Mu, Sigma, Wg, false);

						if(m_pWingPanel[pp].m_bIsTrailing)
						{
							m_Vd[m]      = Wg;
							InducedAngle = atan2(Wg.dot(WindNormal), QInf);
							m_Ai[m]      = InducedAngle*180/PI;
						}

						Wg += VInf; //total speed vector

						//induced force
						dF  = Wg * m_pWingPanel[pp].Vortex;
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
				m_WingCL      += StripForce.dot(WindNormal);                // N/q
				WingIDrag += StripForce.dot(WindDirection)/2.;          // N/q
			}
			p  += coef*m_Surface.at(j)->m_NXPanels;

			// Calculate resulting vector force
			Force     += StripForce;                          // N/q
			m_F[m]     = StripForce * q;	                    // Newtons

//			if(pWPolar->m_bTiltedGeom) m_F[m].RotateY(-Alpha);

/*			if(s_bVLMSymetric)
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
			}*/
			m++;
		}
//		if(s_bVLMSymetric) p+=m_Surface.at(j)->NXPanels * m_Surface.at(j)->NYPanels;
		if(m_Surface.at(j)->m_bIsTipRight && !pWPolar->m_bThinSurfaces) p+=m_Surface.at(j)->m_NXPanels;//tip patch panels
	}

	m_CDi = WingIDrag; // save this wing's induced drag (unused though...)
}


/**
 * Computes the bending moment at each span position based on the results of the panel analysis
 * Assumes the array of force vectors has been calculated previously
 * @param bThinSurface true if the calculation has been performed on thin VLM surfaces, false in the case of a 3D-panelanalysis
 */
void Wing::PanelComputeBending(bool bThinSurface)
{
	QList<double> ypos, zpos;
	int j,k,jj,coef,p;
	double bm;
	CVector Dist(0.0,0.0,0.0);
	CVector Moment;


	if(bThinSurface)
	{
		coef = 1;
		p    = 0;
	}
	else
	{
		coef = 2;
		p= m_Surface[0]->m_NXPanels;
	}

	int NSurfaces = m_Surface.size();

	for (j=0; j<NSurfaces; j++)
	{
		for (k=0; k<m_Surface.at(j)->m_NYPanels; k++)
		{
			if(!bThinSurface)
			{
				ypos.append(m_pWingPanel[p].CollPt.y);
				zpos.append(m_pWingPanel[p].CollPt.z);
 			}
			else
			{
				ypos.append(m_pWingPanel[p].VortexPos.y);
				zpos.append(m_pWingPanel[p].Vortex.z);
 			}

			p += coef*m_Surface.at(j)->m_NXPanels;
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

/**
* Scales the wing chord-wise so that the root chord is set to the NewChord value
*@param NewChord the new value of the root chord
*/
void Wing::ScaleChord(double NewChord)
{
	double ratio = NewChord/Chord(0);
	for (int is=0; is<m_WingSection.size(); is++)
	{
		Chord(is)    *= ratio;
		Offset(is)   *= ratio;
	}
	ComputeGeometry();
}


/**
* Scales the wing span-wise so that the span is set to the NewSpan value
*@param NewSpan the new value of the span
*/
void Wing::ScaleSpan(double NewSpan)
{
	for (int is=0; is<m_WingSection.size(); is++)
	{
		YPosition(is)      *= NewSpan/m_PlanformSpan;
		Length(is)   *= NewSpan/m_PlanformSpan;
	}
	ComputeGeometry();
}

/**
* Scales the wing's sweep so that the sweep is set to the NewSweep value
*@param NewSweep the new value of the average quarter-chord sweep, in degrees
*/
void Wing::ScaleSweep(double NewSweep)
{
	double OldTipOffset = TipOffset();
	double NewTipOffset = Chord(0)/4.0
						 + tan(NewSweep*PI/180.0)*m_PlanformSpan/2.0
						 - TipChord()/4.0;
	if(qAbs(OldTipOffset)>0.00001)
	{
		//scale each panel's offset
		double ratio = NewTipOffset/OldTipOffset;
		for(int is=1; is<NWingSection(); is++)
		{
			Offset(is) *= ratio;
		}
	}
	else
	{
		//set each panel's offset
		for(int is=1; is<NWingSection(); is++)
		{
			Offset(is) = NewTipOffset*YPosition(is)/(m_PlanformSpan/2.0);
		}
	}
	ComputeGeometry();
}


/**
* Scales the wing's twist angles so that the tip twist is set to the NewTwist value.
*@param NewTwist the new value of the average quarter-chord twist, in degrees
*/
void Wing::ScaleTwist(double NewTwist)
{
	if(qAbs(TipTwist())>0.0001)
	{
		//scale each panel's twist
		double ratio = NewTwist/TipTwist();

		for(int is=1; is<NWingSection(); is++)
		{
			Twist(is) *= ratio;
		}
	}
	else
	{
		//Set each panel's twist in the ratio of the span position
		for(int is=1; is<NWingSection(); is++)
		{
			Twist(is) = NewTwist*YPosition(is)/(m_PlanformSpan/2.0);
		}
	}
	ComputeGeometry();
}



/**
 * Loads or Saves the data of this Wing to a binary file.
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool Wing::SerializeWingWPA(QDataStream &ar, bool bIsStoring)
{
	int ArchiveFormat;// identifies the format of the file

	if(bIsStoring)
	{
		//not storing to .wpa format anymore
		return true;
	}
	else
	{
		// loading code
		float f,g,h;
		int k;
		//1010 : added storage of alpha channel + added a provision for ints and floats
		//1009 : QFLR5 v0.03 : added mass properties for inertia calculations
		//1008 : QFLR5 v0.02 : Added wing description field
		//1007 : Changed length units to m
		//1006 : Added Wing Color v2.99-15
		//1005 : Added Chordwise spacing (v2.99-00)
		//1004 : corrected NXPanels, NYPanels, YPanelDist to int (v1.99-33)
		//1003 : AVL Format (v1.99-18)
		//1002 : save VLM Mesh (v1.99-12)
		//1001 : initial format

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

		int NPanel;
		ar >> NPanel;
		if(NPanel <0 || NPanel>1000) return false;

		ClearWingSections();
		for(int is=0; is<=NPanel; is++) m_WingSection.append(new WingSection);

		QString strFoil;

		for (int is=0; is<=NPanel; is++)
		{
			ReadCString(ar, strFoil);
			RightFoil(is) = strFoil;
		}
		for (int is=0; is<=NPanel; is++)
		{
			ReadCString(ar, strFoil);
			LeftFoil(is) = strFoil;
		}

		for (int is=0; is<=NPanel; is++)
		{
			ar >> f; Chord(is)=f;
			if (qAbs(Chord(is)) <0.0)
			{
				m_WingName = "";
				return false;
			}
		}

		for (int is=0; is<=NPanel; is++)
		{
			ar >> f; YPosition(is)=f;
			if (qAbs(YPosition(is)) <0.0)
			{
				m_WingName = "";
				return false;
			}
		}
		for (int is=0; is<=NPanel; is++)
		{
			ar >> f; Offset(is)=f;
		}

		if(ArchiveFormat<1007)
		{
			//convert mm to m
			for (int is=0; is<=NPanel; is++)
			{
				YPosition(is)    /= 1000.0;
				Chord(is)  /= 1000.0;
				Offset(is) /= 1000.0;
			}

		}
		for (int is=0; is<=NPanel; is++)
		{
			ar >> f; Dihedral(is)=f;
		}
		for (int is=0; is<=NPanel; is++)
		{
			ar >> f; Twist(is)=f;
		}

		ar >> f; //m_XCmRef=f;

		ar >> k;

		for (int is=0; is<=NPanel; is++)
		{
			if(ArchiveFormat<=1003)
			{
				ar >> f;
				NXPanels(is) = (int)f;
			}
			else
				ar >> NXPanels(is);
		}

		for (int is=0; is<=NPanel; is++)
		{
			if(ArchiveFormat<=1003)
			{
				ar >> f;
				NYPanels(is) = (int)f;
			}
			else 	ar >> NYPanels(is);
		}
		int total = 0;
		for (int is=0; is<NPanel; is++)
		{
			total += NYPanels(is);
		}


		int spanpos = 0;
		int vlmpanels = 0;
		for (int is=0; is<NPanel; is++)
		{
			spanpos  += NYPanels(is);
			vlmpanels +=NXPanels(is)*NYPanels(is);
		}


		if (ArchiveFormat >=1005)
		{
			for(int is=0; is<=NPanel; is++)
			{
				ar >> k;
				if(k==1)       XPanelDist(is) = COSINE;
				else if(k==2)  XPanelDist(is) = SINE;
				else if(k==-2) XPanelDist(is) = INVERSESINE;
				else           XPanelDist(is) = UNIFORM;  //case 0
			}
		}

		for (int is=0; is<=NPanel; is++)
		{
			ar >> k;
			if(k==1)       YPanelDist(is) = COSINE;
			else if(k==2)  YPanelDist(is) = SINE;
			else if(k==-2) YPanelDist(is) = INVERSESINE;
			else           YPanelDist(is) = UNIFORM;  //case 0
		}

		if(ArchiveFormat>=1006)
		{
			ReadCOLORREF(ar, m_WingColor);
		}

		if(ArchiveFormat>=1009)
		{
			ar >> f;  m_VolumeMass = f;
			int nMass;

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

			ClearPointMasses();
			for(int im=0; im<nMass; im++)
			{
				m_PointMass.append(new PointMass(mass[im], position[im], tag[im]));
			}
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



/**
 * Loads or Saves the data of this Wing to a binary file.
 * @param ar the QDataStream object from/to which the data should be serialized
 * @param bIsStoring true if saving the data, false if loading
 * @return true if the operation was successful, false otherwise
 */
bool Wing::SerializeWingXFL(QDataStream &ar, bool bIsStoring)
{
	QString tag;
	QString rightFoil, leftFoil;
	int nx, ny;
	int i, k, n, is;
	int ArchiveFormat;// identifies the format of the file
	double dble, dm, px, py, pz;
	double chord, twist, pos, dihedral, offset;
	enumPanelDistribution xDist, yDist;

	if(bIsStoring)
	{
		ar << 100001;
		ar << m_WingName;
		ar << m_WingDescription;

		ar << m_WingColor;

		ar << m_bSymetric;

		ar << NWingSection();

		for (is=0; is<NWingSection(); is++)
		{
			ar << RightFoil(is);
			ar << LeftFoil(is);
			ar << Chord(is);
			ar << YPosition(is);
			ar << Offset(is);
			ar << Dihedral(is);
			ar << Twist(is);
			ar << NXPanels(is);
			ar << NYPanels(is);

			switch(XPanelDist(is))
			{
				case COSINE:
					ar <<  1;
					break;
				case SINE:
					ar <<  2;
					break;
				case INVERSESINE:
					ar << -2;
					break;
				default:
					ar <<  0; //uniform
					break;
			}

			switch(YPanelDist(is))
			{
				case COSINE:
					ar <<  1;
					break;
				case SINE:
					ar <<  2;
					break;
				case INVERSESINE:
					ar << -2;
					break;
				default:
					ar <<  0; //uniform
					break;
			}
		}

		ar << m_VolumeMass;
		ar << m_PointMass.size();
		for(i=0; i<m_PointMass.size(); i++)
		{
			ar << m_PointMass.at(i)->m_Mass;
			ar << m_PointMass.at(i)->m_Position.x << m_PointMass.at(i)->m_Position.y << m_PointMass.at(i)->m_Position.z;
			ar << m_PointMass.at(i)->tag();
		}

		// space allocation for the future storage of more data, without need to change the format
		for (int i=0; i<20; i++) ar << i;
		for (int i=0; i<50; i++) ar << (double)i;

		return true;
	}
	else
	{
		ar >> ArchiveFormat;
		if(ArchiveFormat<100000 || ArchiveFormat>100001) return false;

		ar >> m_WingName;
		ar >> m_WingDescription;

		ar >> m_WingColor;

		ar >> m_bSymetric;

		ClearWingSections();
		ar >> n;
		for (i=0; i<n; i++)
		{
			ar >> rightFoil;
			ar >> leftFoil;
			ar >> chord;
			ar >> pos;
			ar >> offset;
			ar >> dihedral;
			ar >> twist;
			ar >> nx;
			ar >> ny;

			ar >> k;
			if(k==1)       xDist = COSINE;
			else if(k== 2) xDist = SINE;
			else if(k==-2) xDist = INVERSESINE;
			else           xDist = UNIFORM;

			ar >> k;
			if(k==1)       yDist = COSINE;
			else if(k== 2) yDist = SINE;
			else if(k==-2) yDist = INVERSESINE;
			else           yDist = UNIFORM;

			AppendWingSection(chord, twist, pos, dihedral, offset, nx, ny, xDist, yDist, rightFoil, leftFoil);
		}

		ar >> m_VolumeMass;
		ClearPointMasses();
		ar >> n;
		for(i=0; i<n; i++)
		{
			ar >> dm >> px >> py >> pz;
			ar >> tag;
			m_PointMass.append(new PointMass(dm, CVector(px, py, pz), tag));
		}

		// space allocation
		for (int i=0; i<20; i++) ar >> k;
		for (int i=0; i<50; i++) ar >> dble;

		ComputeGeometry();
		return true;
	}
}



/**
 * Returns the number of mesh panels defined on this Wing's surfaces; the number is given for a double-side mesh of the wing
 * @return the total number of panels
 */
int Wing::VLMPanelTotal(bool bThinSurface)
{
	double MinPanelSize;

	if(s_MinPanelSize>0.0) MinPanelSize = s_MinPanelSize;
	else                              MinPanelSize = m_PlanformSpan/1000.0;
	int total = 0;
	for (int is=0; is<NWingSection()-1; is++)
	{
		//do not create a surface if its length is less than the critical size
		if (qAbs(YPosition(is)-YPosition(is+1)) > MinPanelSize)	total += NXPanels(is)*NYPanels(is);
	}
	if(!m_bIsFin) total *=2;

	if(!bThinSurface)
	{
		total *= 2;
		total += 2*NXPanels(0); // assuming a uniform number of chordwise panels over the wing
	}

	return total;
}


/**
*  Calculates the wing aero coefficients
*  Uses Cp distribution in input for thick surfaces
*  Uses Gamma distribution in input for VLM method
*
*  Input data:
*    Freestream speed Qinf
*    Angle of attack Alpha
*    Cp dstribution for thick wings
*    Mu or Gamma distribution, depending on the analysis type
*    Type of surface :
*        - Thin Surface, i.e. VLM type surfaces, with vortex distribution
*        - Thick Surfaces;, i.e. 3D Panels with source+doublet distribution on panels
*    Type of analysis : viscous or inviscid
*
*  Output
*    centre of pressure position (XCP, YCP)
*    moment coefficients GCm, VCm, ICm, GRm, GYm, VYm, IYm
*/
void Wing::PanelComputeOnBody(double QInf, double Alpha, double *Cp, double *Gamma, double &XCP, double &YCP, double &ZCP,
						 double &GCm, double &VCm, double &ICm, double &GRm, double &GYm, double &VYm,double &IYm,
						 WPolar *pWPolar, CVector CoG)

{
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

	int NSurfaces = m_Surface.size();

	for (j=0; j<NSurfaces; j++)
	{
		//do not consider left tip patch, if any
		if(!pWPolar->m_bThinSurfaces && m_Surface.at(j)->m_bIsTipLeft) p += m_Surface.at(j)->m_NXPanels;

		if(m_Surface.at(j)->m_bTEFlap) m_FlapMoment.append(0.0);

		SurfaceNormal = m_Surface.at(j)->Normal;

		// consider each strip in turn
		for (k=0; k<m_Surface.at(j)->m_NYPanels; k++)
		{
			//initialize
			DragVector.Set(0.0,0.0,0.0);
			StripForce.Set(0.0,0.0,0.0);
			GeomMoment.Set(0.0,0.0,0.0);

			m_CmAirf[m]    = 0.0;
			CPStrip        = 0.0;

			m_Surface.at(j)->GetLeadingPt(k, PtLEStrip);
			m_Surface.at(j)->GetC4(k, PtC4Strip, tau);
			if(pWPolar->m_Beta>0.0)
			{
				PtC4Strip.RotateZ(Origin, pWPolar->m_Beta);
				PtLEStrip.RotateZ(Origin, pWPolar->m_Beta);
			}

			LeverArmC4CoG = PtC4Strip - CoG;


			for (l=0; l<coef*m_Surface.at(j)->m_NXPanels; l++)
			{
				// Get the force acting on the panel
				if(m_pWingPanel[p].m_Pos!=MIDSURFACE)
				{
					ForcePt = m_pWingPanel[p].CollPt;
					PanelForce = m_pWingPanel[p].Normal * (-Cp[p]) * m_pWingPanel[p].Area;      // Newtons/q
				}
				else
				{
					// for each panel along the chord, add the lift coef
					ForcePt = m_pWingPanel[p].VortexPos;
					PanelForce  = WindDirection * m_pWingPanel[p].Vortex;
					PanelForce *= 2.0 * Gamma[p] /QInf;                                 //Newtons/q

					if(!pWPolar->m_bVLM1 && !m_pWingPanel[p].m_bIsLeading)
					{
						Force       = WindDirection * m_pWingPanel[p].Vortex;
						Force      *= 2.0 * Gamma[p+1] /QInf;                          //Newtons/q
						PanelForce -= Force;
					}
					Cp[p] = PanelForce.dot(m_pWingPanel[p].Normal)/m_pWingPanel[p].Area;    //
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
                ZCP       += ForcePt.z * PanelForce.dot(WindNormal);
				CPStrip   += ForcePt.x * NForce;

				if(m_Surface.at(j)->m_bTEFlap)
				{
					if(m_Surface.at(j)->IsFlapPanel(p))
					{
						//then p is on the flap, so add its contribution
						HingeLeverArm = ForcePt - m_Surface.at(j)->m_HingePoint;
						HingeMoment = HingeLeverArm * PanelForce;//N.m/q
						m_FlapMoment[nFlap] += HingeMoment.dot(m_Surface.at(j)->m_HingeVector)* pWPolar->m_Density * QInf * QInf/2.0;  //N.m
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
		if(!pWPolar->m_bThinSurfaces && m_Surface.at(j)->m_bIsTipRight) p += m_Surface.at(j)->m_NXPanels;
		if(m_Surface.at(j)->m_bTEFlap) nFlap++;
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



/**
* In input, takes the speed QInf and the distribution of lift coefficients m_Cl[] along the span
* In output, returns for each span station
* 	- The Reynolds number m_Re[]
*	- The viscous drag coefficient m_PCd[]
*      - The top and bottom transition points m_XTrtop[] and m_XTrBot[]
*/
void Wing::PanelComputeViscous(double QInf, WPolar *pWPolar, double &WingVDrag, bool bViscous, QString &OutString)
{
	QString string, strong, strLength;
	int m;
	bool bPointOutRe, bPointOutCl, bOutRe, bError;
	double tau = 0.0;
	CVector PtC4;

	OutString.clear();

	WingVDrag = 0.0;

	bOutRe = bError = bPointOutRe = bPointOutCl = false;

	Units::getLengthUnitLabel(strLength);

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
	for (int j=0; j<m_Surface.size(); j++)
	{
		for(int k=0; k<m_Surface.at(j)->m_NYPanels; k++)
		{
			bOutRe = bPointOutRe = false;
			bPointOutCl = false;
			m_Surface.at(j)->GetC4(k, PtC4, tau);

			m_PCd[m]    = GetVar(2, m_Surface.at(j)->m_pFoilA, m_Surface.at(j)->m_pFoilB, m_Re[m], m_Cl[m], tau, bOutRe, bError);
			bPointOutRe = bOutRe || bPointOutRe;
			if(bError) bPointOutCl = true;

			m_XTrTop[m] = GetVar(5, m_Surface.at(j)->m_pFoilA, m_Surface.at(j)->m_pFoilB, m_Re[m], m_Cl[m], tau, bOutRe, bError);
			bPointOutRe = bOutRe || bPointOutRe;
			if(bError) bPointOutCl = true;

			m_XTrBot[m] = GetVar(6, m_Surface.at(j)->m_pFoilA, m_Surface.at(j)->m_pFoilB, m_Re[m], m_Cl[m], tau, bOutRe, bError);
			bPointOutRe = bOutRe || bPointOutRe;
			if(bError) bPointOutCl = true;

			if(bPointOutCl)
			{
				Units::getLengthUnitLabel(string);
				strong = QString(QObject::tr("           Span pos = %1 ")).arg(m_SpanPos[m]*Units::mtoUnit(), 9,'f',2);
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
				Units::getLengthUnitLabel(string);
				strong = QString(QObject::tr("           Span pos = %1 ")).arg(m_SpanPos[m]*Units::mtoUnit(),9,'f',2);
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


/**
 * Identifies if a given index of a panel belongs to this wing or not
 * @param nPanel the index of the panel
 * @return true if the panel belongs to the wing, false otherwise
 */
bool Wing::IsWingPanel(int nPanel)
{
	for(int p=0; p<m_MatSize; p++)
	{
		if(nPanel==m_pWingPanel[p].m_iElement) return true;
	}
	return false;
}


/**
 * Identifies if a given index of a node belongs to this wing or not
 * @param nNode the index of a node
 * @return true if the node belongs to the wing, false otherwise
 */
bool Wing::IsWingNode(int nNode)
{
	for(int p=0; p<m_MatSize; p++)
	{
		if(nNode==m_pWingPanel[p].m_iLA) return true;
		if(nNode==m_pWingPanel[p].m_iLB) return true;
		if(nNode==m_pWingPanel[p].m_iTA) return true;
		if(nNode==m_pWingPanel[p].m_iTB) return true;
	}
	return false;
}


/** Returns the offset at a span section identified by its index
*@param iSection the index of the section
*@return the value of the offset
*/
double & Wing::Offset(const int &iSection)    {return m_WingSection[iSection]->m_Offset;}

/** Returns the dihedral angle at a span section identified by its index
*@param iSection the index of the section
*@return the value of the dihedral angle, in degrees
*/
double & Wing::Dihedral(const int &iSection)  {return m_WingSection[iSection]->m_Dihedral;}

/** Returns the chord length at a span section identified by its index
*@param iSection the index of the section
*@return the value of the chord length
*/
double & Wing::Chord(const int &iSection)     {return m_WingSection[iSection]->m_Chord;}

/** Returns the twost angle at a span section identified by its index
*@param iSection the index of the section
*@return the value of the twist angle, in degrees
*/
double & Wing::Twist(const int &iSection)     {return m_WingSection[iSection]->m_Twist;}

/** Returns the span position at a span section identified by its index
*@param iSection the index of the section
*@return the value of the span position
*/
double & Wing::YPosition(const int &iSection) {return m_WingSection[iSection]->m_YPosition;}

/** Returns the length betwee a span section identified by its index and the next spanwise section
*@param iSection the index of the section
*@return the value of the length of the panel
*/
double & Wing::Length(const int &iSection)    {return m_WingSection[iSection]->m_Length;}

/** Returns the span position of a span section identified by its index, projecteed on the x-y plane
*@param iSection the index of the section
*@return the value of the projected span position
*/
double & Wing::YProj(const int &iSection)     {return m_WingSection[iSection]->m_YProj;}

/** Returns the z-position at a span section identified by its index
*@param iSection the index of the section
*@return the value of the z-position
*/
double & Wing::ZPosition(const int &iSection) {return m_WingSection[iSection]->m_ZPos;}


/** Returns the number of chordwise panels at a span section identified by its index
*@param iSection the index of the section
*@return the number of chordwise panels
*/
int & Wing::NXPanels(const int &iSection)   {return m_WingSection[iSection]->m_NXPanels;}

/** Returns the number of spanwise panels at a span section identified by its index
*@param iSection the index of the section
*@return the number of spanwise panels
*/
int & Wing::NYPanels(const int &iSection)   {return m_WingSection[iSection]->m_NYPanels;}

/** Returns the type of distribution of chordwise panels at a span section identified by its index - always cosine type
*@param iSection the index of the section
*@return the type of distribution of chordwise panels - always cosine type
*/
enumPanelDistribution & Wing::XPanelDist(const int &iSection) {return m_WingSection[iSection]->m_XPanelDist;}

/** Returns the type of distribution of spanwise panels at a span section identified by its index
*@param iSection the index of the section
*@return the type of distribution of spanwise panels
*/
enumPanelDistribution & Wing::YPanelDist(const int &iSection) {return m_WingSection[iSection]->m_YPanelDist;}

/**
 * Returns the name of the foil on the right side of a span section
 * @param iSection the index of the section
 * @return the name of the foil on the right side of the section
 */
QString & Wing::RightFoil(const int &iSection) {return m_WingSection[iSection]->m_RightFoilName;}

/**
 * Returns the name of the foil on the left side of a span section
 * @param iSection the index of the section
 * @return the name of the foil on the left side of the section
 */
QString & Wing::LeftFoil(const int &iSection)  {return m_WingSection[iSection]->m_LeftFoilName;}


/**
 * Removes the section in the geometry of the wing identified by its index
 * @param iSection the index of the section
 */
void Wing::RemoveWingSection(int const iSection)
{
	if(iSection<0 || iSection>=m_WingSection.size()) return;
	m_WingSection.removeAt(iSection);
}


/**
 * Inserts a section in the geometry of the wing at a postion identified by its index
 * @param iSection the index of the section
 */
void Wing::InsertSection(int iSection)
{
	if(iSection==0)                          m_WingSection.prepend(new WingSection);
	else if(iSection>=m_WingSection.size())  m_WingSection.append(new WingSection);
	else                                     m_WingSection.insert(iSection, new WingSection);
}


/**
 * Appends a new section at the tip of the wing, with default values
 *@
 */
bool Wing::AppendWingSection()
{
	m_WingSection.append(new WingSection());
	return true;
}


/**
 * Appends a new section at the tip of the wing, with values specified as input parameters
 */
bool Wing::AppendWingSection(double Chord, double Twist, double Pos, double Dihedral, double Offset,
							 int NXPanels, int NYPanels, enumPanelDistribution XPanelDist, enumPanelDistribution YPanelDist,
							 QString RightFoilName, QString LeftFoilName)
{
	WingSection *pWS = new WingSection();
	m_WingSection.append(pWS);
	pWS->m_Chord      = Chord;
	pWS->m_Twist      = Twist;
	pWS->m_YPosition  = Pos ;
	pWS->m_Dihedral   = Dihedral;
	pWS->m_Offset     = Offset ;

	pWS->m_NXPanels   = NXPanels ;
	pWS->m_NYPanels   = NYPanels;
	pWS->m_XPanelDist = XPanelDist;
	pWS->m_YPanelDist = YPanelDist;

	pWS->m_RightFoilName  = RightFoilName;
	pWS->m_LeftFoilName   = LeftFoilName;

	return true;
}


bool Wing::isWingFoil(Foil *pFoil)
{
	if(!pFoil) return false;

	for (int iws=0; iws<NWingSection(); iws++)
	{
		if(pFoil->foilName() == m_WingSection.at(iws)->m_RightFoilName)
		{
			return true;
		}
	}

	if(!m_bSymetric)
	{
		for (int iws=0; iws<NWingSection(); iws++)
		{
			if(pFoil->foilName() == m_WingSection.at(iws)->m_LeftFoilName)
			{
				return true;
			}
		}
	}
	return false;
}














