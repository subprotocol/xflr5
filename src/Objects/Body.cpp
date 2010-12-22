/****************************************************************************

    CBody Class
	Copyright (C) 2007-2008 Andre Deperrois xflr5@yahoo.com

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


#include "../MainFrame.h"
#include "Body.h"
#include "../Globals.h"
#include "../Misc/UnitsDlg.h"
#include <math.h>
#include <QMessageBox>
#include <QFileDialog>
#include <QtDebug>

void *CBody::s_pMainFrame;
double CBody::s_xKnots[MAXBODYFRAMES*2];
double CBody::s_hKnots[MAXSIDELINES*2];
double CBody::s_XPanelPos[300];
QRect CBody::s_rViewRect;



CBody::CBody()
{
	int i;

	m_BodyName = UnitsDlg::tr("BodyName");

	m_BodyColor = QColor(210,240,240);
	m_BodyStyle = 0;
	m_BodyWidth = 1;

	m_FramePosition[0].Set( -0.10, 0.0,  0.0);
	m_FramePosition[1].Set( -0.07, 0.0,  0.0);
	m_FramePosition[2].Set(  0.00, 0.0,  0.0);
	m_FramePosition[3].Set(  0.12, 0.0,  0.0);
	m_FramePosition[4].Set(  0.18, 0.0,  0.0);
	m_FramePosition[5].Set(  0.60, 0.0,  0.0);
	m_FramePosition[6].Set(  0.63, 0.0,  0.0);

	m_bClosedSurface = false;

	m_CoG.Set(0.0,0.0,0.0);
	m_CoGIxx = m_CoGIyy = m_CoGIzz = m_CoGIxz = 0.0;
	m_VolumeMass = m_TotalMass = 0.0;
	m_NMass = 0;
	memset(m_MassValue,    0, sizeof(m_MassValue));
	memset(m_MassPosition, 0, sizeof(m_MassPosition));
	for(int i=0; i< MAXMASSES; i++) m_MassTag[i] = QString("Description %1").arg(i);

	m_NStations  = 7;
	m_NSideLines = 5;

	m_iActiveFrame =  1;
	m_iHighlight   = -1;
	m_LineType     =  2;

	m_nxDegree = 3;
	m_nhDegree = 3;
	m_nxKnots = 0;
	m_nhKnots = 0;

	m_nxPanels = 20;
	m_nhPanels = 10;

	m_pPanel = NULL; 
	m_NElements = m_nxPanels * m_nhPanels * 2;

	m_iRes = 30;

	m_Bunch  = 0.0;

	eps = 1.0e-06;

	m_Frame[0].m_Point[0].Set(-0.110, 0.0, -0.0124);
	m_Frame[0].m_Point[1].Set(-0.110, 0.0, -0.0124);
	m_Frame[0].m_Point[2].Set(-0.110, 0.0, -0.0124);
	m_Frame[0].m_Point[3].Set(-0.110, 0.0, -0.0124);
	m_Frame[0].m_Point[4].Set(-0.110, 0.0, -0.0124);

	m_Frame[1].m_Point[0].Set(-0.0936, 0.000,  0.0035);
	m_Frame[1].m_Point[1].Set(-0.0936, 0.011,  0.0003);
	m_Frame[1].m_Point[2].Set(-0.0936, 0.013, -0.0136);
	m_Frame[1].m_Point[3].Set(-0.0936, 0.011, -0.0257);
	m_Frame[1].m_Point[4].Set(-0.0936, 0.000, -0.0266);

	m_Frame[2].m_Point[0].Set(-0.0067, 0.000,  0.0378);
	m_Frame[2].m_Point[1].Set(-0.0067, 0.028,  0.0406);
	m_Frame[2].m_Point[2].Set(-0.0067, 0.037,  0.0016);
	m_Frame[2].m_Point[3].Set(-0.0067, 0.034, -0.0408);
	m_Frame[2].m_Point[4].Set(-0.0067, 0.000, -0.0445);

	m_Frame[3].m_Point[0].Set(0.0943, 0.000,  0.0252);
	m_Frame[3].m_Point[1].Set(0.0943, 0.012,  0.0192);
	m_Frame[3].m_Point[2].Set(0.0943, 0.018,  0.0012);
	m_Frame[3].m_Point[3].Set(0.0943, 0.012, -0.0168);
	m_Frame[3].m_Point[4].Set(0.0943, 0.000, -0.0228);

	m_Frame[4].m_Point[0].Set(0.242, 0.000,  0.0075);
	m_Frame[4].m_Point[1].Set(0.242, 0.006,  0.0090);
	m_Frame[4].m_Point[2].Set(0.242, 0.009,  0.0011);
	m_Frame[4].m_Point[3].Set(0.242, 0.007, -0.0059);
	m_Frame[4].m_Point[4].Set(0.242, 0.000, -0.0051);

	m_Frame[5].m_Point[0].Set(0.636, 0.000,  0.0138);
	m_Frame[5].m_Point[1].Set(0.636, 0.010,  0.0132);
	m_Frame[5].m_Point[2].Set(0.636, 0.012, -0.0001);
	m_Frame[5].m_Point[3].Set(0.636, 0.010, -0.0098);
	m_Frame[5].m_Point[4].Set(0.636, 0.000, -0.0106);

	m_Frame[6].m_Point[0].Set(0.660, 0.00,  0.0);
	m_Frame[6].m_Point[1].Set(0.660, 0.00,  0.0);
	m_Frame[6].m_Point[2].Set(0.660, 0.00,  0.0);
	m_Frame[6].m_Point[3].Set(0.660, 0.00, -0.0);
	m_Frame[6].m_Point[4].Set(0.660, 0.00, -0.0);

	ComputeCenterLine();

	for(i=0; i<MAXBODYFRAMES;i++) m_xPanels[i] = 1;
	for(i=0; i<MAXSIDELINES;i++)  m_hPanels[i] = 1;

	memset(m_x, 0, sizeof(m_x));
	memset(m_y, 0, sizeof(m_y));
	m_np = 0;

	SetKnots();
}



void CBody::ComputeAero(double *Cp, double &XCP, double &YCP,
						double &GCm, double &GRm, double &GYm, double &Alpha, CVector &CoG)
{
	int p;
	double cosa, sina, PanelLift;
	CVector PanelForce, LeverArm, WindNormal, WindDirection;
	CVector GeomMoment;

	cosa = cos(Alpha*PI/180.0);
	sina = sin(Alpha*PI/180.0);


	//   Define wind axis
	WindNormal.Set(   -sina, 0.0, cosa);
	WindDirection.Set( cosa, 0.0, sina);

	for (p=0; p<m_NElements; p++)
	{
		PanelForce.x = m_pPanel[p].Normal.x * (-Cp[p]) * m_pPanel[p].Area;
		PanelForce.y = m_pPanel[p].Normal.y * (-Cp[p]) * m_pPanel[p].Area;
		PanelForce.z = m_pPanel[p].Normal.z * (-Cp[p]) * m_pPanel[p].Area; // N/q

		PanelLift = PanelForce.dot(WindNormal);
		XCP   += m_pPanel[p].CollPt.x * PanelLift;
		YCP   += m_pPanel[p].CollPt.y * PanelLift;

		LeverArm.Set(m_pPanel[p].CollPt.x - CoG.x, m_pPanel[p].CollPt.y, m_pPanel[p].CollPt.z-CoG.z);
		GeomMoment = LeverArm * PanelForce; // N.m/q

		GCm  += GeomMoment.y;
		GRm  += GeomMoment.dot(WindDirection);
		GYm  += GeomMoment.dot(WindNormal);
	}
}



void CBody::ComputeCenterLine()
{
	int i;
	for(i=0; i<m_NStations; i++)
	{
		m_FramePosition[i].z=(m_Frame[i].m_Point[0].z + m_Frame[i].m_Point[m_NSideLines-1].z)/2.0;
	}
}

void CBody::ComputeBodyAxisInertia()
{
	// Calculates the inertia tensor in geometrical (body) axis :
	//  - adds the volume inertia AND the point masses of all components
	//  - the body axis is the frame in which all the geometry has been defined
	//  - the origin=BodyFrame;
	//  - the center of gravity is calculated from component masses and is NOT the CoG defined in the polar

	int i;
	CVector LA, VolumeCoG, TotalCoG;
	double Ixx, Iyy, Izz, Ixz,  VolumeMass, TotalMass;
	Ixx = Iyy = Izz = Ixz = TotalMass = VolumeMass = 0.0;

	ComputeVolumeInertia(VolumeCoG, Ixx, Iyy, Izz, Ixz);
	TotalMass = m_VolumeMass;

	TotalCoG = VolumeCoG *m_VolumeMass;

	// add point masses
	for(i=0; i<m_NMass; i++)
	{
		TotalMass += m_MassValue[i];
		TotalCoG += m_MassPosition[i] * m_MassValue[i];
	}

	TotalCoG = TotalCoG/TotalMass;

	// The CoG position is now available, so calculate the inertia w.r.t the CoG
	for(i=0; i<m_NMass; i++)
	{
		LA = m_MassPosition[i] - TotalCoG;
		Ixx += m_MassValue[i] * (LA.y*LA.y + LA.z*LA.z);
		Iyy += m_MassValue[i] * (LA.x*LA.x + LA.z*LA.z);
		Izz += m_MassValue[i] * (LA.x*LA.x + LA.y*LA.y);
		Ixz -= m_MassValue[i] * (LA.x*LA.z);
	}

	m_TotalMass = TotalMass;
	m_CoG = TotalCoG;
	m_CoGIxx =  Ixx;
	m_CoGIyy =  Iyy;
	m_CoGIzz =  Izz;
	m_CoGIxz =  Ixz;
}

void CBody::ComputeVolumeInertia(CVector &CoG, double &CoGIxx, double &CoGIyy, double &CoGIzz, double &CoGIxz)
{
	// Assume that the mass is distributed homogeneously in the body's skin
	// Homogeneity is questionable, but is a rather handy assumption
	// Mass in the body's skin is reasonable, given that the point masses
	// are added manually

	//evaluate roughly the Body's wetted area
	int i,j,k;
	double ux, rho;
	double dj, dj1;
	CVector Pt, LATB, TALB, N, PLA, PTA, PLB, PTB, Top, Bot;
	double BodyArea = 0.0;
	double SectionArea;
	double xpos, dl;
	CoG.Set(0.0, 0.0, 0.0);
	CoGIxx = CoGIyy = CoGIzz = CoGIxz = 0.0;

	if(m_LineType==1)
	{
		// we use the panel division
		//first get the wetted area

		for (i=0; i<m_NStations-1; i++)
		{
			for (k=0; k<m_NSideLines-1; k++)
			{
				//build the four corner points of the strips
				PLA.x =  m_FramePosition[i].x     ;
				PLA.y =  m_Frame[i].m_Point[k].y  ;
				PLA.z =  m_Frame[i].m_Point[k].z  ;

				PLB.x = m_FramePosition[i].x      ;
				PLB.y = m_Frame[i].m_Point[k+1].y ;
				PLB.z = m_Frame[i].m_Point[k+1].z ;

				PTA.x = m_FramePosition[i+1].x    ;
				PTA.y = m_Frame[i+1].m_Point[k].y ;
				PTA.z = m_Frame[i+1].m_Point[k].z ;

				PTB.x = m_FramePosition[i+1].x     ;
				PTB.y = m_Frame[i+1].m_Point[k+1].y;
				PTB.z = m_Frame[i+1].m_Point[k+1].z;

				LATB = PTB - PLA;
				TALB = PLB - PTA;
				N = TALB * LATB;//panel area x2
				BodyArea += N.VAbs() /2.0;
			}
		}

		BodyArea *= 2.0;
		rho = m_VolumeMass/BodyArea;
		//First get the CoG position
		for (i=0; i<m_NStations-1; i++)
		{
			for (j=0; j<m_xPanels[i]; j++)
			{
				dj  = (double) j   /(double)(m_xPanels[i]);
				dj1 = (double)(j+1)/(double)(m_xPanels[i]);
				SectionArea = 0.0;

				PLA.x = PLB.x = (1.0- dj) * m_FramePosition[i].x   +  dj * m_FramePosition[i+1].x;
				PTA.x = PTB.x = (1.0-dj1) * m_FramePosition[i].x   + dj1 * m_FramePosition[i+1].x;

				for (k=0; k<m_NSideLines-1; k++)
				{
					//build the four corner points of the strips
					PLB.y = (1.0- dj) * m_Frame[i].m_Point[k].y   +  dj * m_Frame[i+1].m_Point[k].y;
					PLB.z = (1.0- dj) * m_Frame[i].m_Point[k].z   +  dj * m_Frame[i+1].m_Point[k].z;

					PTB.y = (1.0-dj1) * m_Frame[i].m_Point[k].y   + dj1 * m_Frame[i+1].m_Point[k].y;
					PTB.z = (1.0-dj1) * m_Frame[i].m_Point[k].z   + dj1 * m_Frame[i+1].m_Point[k].z;

					PLA.y = (1.0- dj) * m_Frame[i].m_Point[k+1].y +  dj * m_Frame[i+1].m_Point[k+1].y;
					PLA.z = (1.0- dj) * m_Frame[i].m_Point[k+1].z +  dj * m_Frame[i+1].m_Point[k+1].z;

					PTA.y = (1.0-dj1) * m_Frame[i].m_Point[k+1].y + dj1 * m_Frame[i+1].m_Point[k+1].y;
					PTA.z = (1.0-dj1) * m_Frame[i].m_Point[k+1].z + dj1 * m_Frame[i+1].m_Point[k+1].z;

					LATB = PTB - PLA;
					TALB = PLB - PTA;
					N = TALB * LATB;//panel area x2
					SectionArea += N.VAbs() /2.0;
				}
				SectionArea *= 2.0;// to account for right side;

				// get center point for this section
				Pt.x = (PLA.x + PTA.x)/2.0;
				Pt.y = 0.0;
				Pt.z = ((1.0-dj)  * m_FramePosition[i].z + dj  * m_FramePosition[i+1].z
					   +(1.0-dj1) * m_FramePosition[i].z + dj1 * m_FramePosition[i+1].z)/2.0;

				CoG.x += SectionArea*rho * Pt.x;
				CoG.y += SectionArea*rho * Pt.y;
				CoG.z += SectionArea*rho * Pt.z;
			}
		}
		if(m_VolumeMass>PRECISION) CoG *= 1.0/ m_VolumeMass;
		else                       CoG.Set(0.0, 0.0, 0.0);

		//Then Get Inertias
		// we could do it one calculation, for CG and inertia, by using Hyghens/steiner theorem
		for (i=0; i<m_NStations-1; i++)
		{
			for (j=0; j<m_xPanels[i]; j++)
			{
				dj  = (double) j   /(double)(m_xPanels[i]);
				dj1 = (double)(j+1)/(double)(m_xPanels[i]);
				SectionArea = 0.0;

				PLA.x = PLB.x = (1.0- dj) * m_FramePosition[i].x   +  dj * m_FramePosition[i+1].x;
				PTA.x = PTB.x = (1.0-dj1) * m_FramePosition[i].x   + dj1 * m_FramePosition[i+1].x;

				for (k=0; k<m_NSideLines-1; k++)
				{
					//build the four corner points of the strips
					PLB.y = (1.0- dj) * m_Frame[i].m_Point[k].y   +  dj * m_Frame[i+1].m_Point[k].y;
					PLB.z = (1.0- dj) * m_Frame[i].m_Point[k].z   +  dj * m_Frame[i+1].m_Point[k].z;

					PTB.y = (1.0-dj1) * m_Frame[i].m_Point[k].y   + dj1 * m_Frame[i+1].m_Point[k].y;
					PTB.z = (1.0-dj1) * m_Frame[i].m_Point[k].z   + dj1 * m_Frame[i+1].m_Point[k].z;

					PLA.y = (1.0- dj) * m_Frame[i].m_Point[k+1].y +  dj * m_Frame[i+1].m_Point[k+1].y;
					PLA.z = (1.0- dj) * m_Frame[i].m_Point[k+1].z +  dj * m_Frame[i+1].m_Point[k+1].z;

					PTA.y = (1.0-dj1) * m_Frame[i].m_Point[k+1].y + dj1 * m_Frame[i+1].m_Point[k+1].y;
					PTA.z = (1.0-dj1) * m_Frame[i].m_Point[k+1].z + dj1 * m_Frame[i+1].m_Point[k+1].z;

					LATB = PTB - PLA;
					TALB = PLB - PTA;
					N = TALB * LATB;//panel area x2
					SectionArea += N.VAbs() /2.0;
				}
				SectionArea *= 2.0;// to account for right side;

				// get center point for this section
				Pt.x = (PLA.x + PTA.x)/2.0;
				Pt.y = 0.0;
				Pt.z = ((1.0-dj)  * m_FramePosition[i].z + dj  * m_FramePosition[i+1].z
					   +(1.0-dj1) * m_FramePosition[i].z + dj1 * m_FramePosition[i+1].z)/2.0;

				CoGIxx += SectionArea*rho * ( (Pt.y-CoG.y)*(Pt.y-CoG.y) + (Pt.z-CoG.z)*(Pt.z-CoG.z) );
				CoGIyy += SectionArea*rho * ( (Pt.x-CoG.x)*(Pt.x-CoG.x) + (Pt.z-CoG.z)*(Pt.z-CoG.z) );
				CoGIzz += SectionArea*rho * ( (Pt.x-CoG.x)*(Pt.x-CoG.x) + (Pt.y-CoG.y)*(Pt.y-CoG.y) );
				CoGIxz -= SectionArea*rho * ( (Pt.x-CoG.x)*(Pt.z-CoG.z) );
			}
		}
	}
	else if(m_LineType==2)
	{
		int NSections = 20;//why not ?
		xpos = m_FramePosition[0].x;

		dl = GetLength()/(double)(NSections-1);

		for (j=0; j<NSections-1; j++)
		{
			BodyArea += dl * (GetSectionArcLength(xpos)+ GetSectionArcLength(xpos+dl)) /2.0;
			xpos += dl;
		}

		rho = m_VolumeMass / BodyArea;

		// First evaluate CoG, assuming each section is a point mass
		xpos = m_FramePosition[0].x;
		for (j=0; j<NSections-1; j++)
		{
			SectionArea = dl * (GetSectionArcLength(xpos)+ GetSectionArcLength(xpos+dl))/2.0;
			Pt.x = xpos + dl/2.0;
			Pt.y = 0.0;
			ux = Getu(Pt.x);
			GetPoint(ux, 0.0, true, Top);
			GetPoint(ux, 1.0, true, Bot);
			Pt.z = (Top.z + Bot.z)/2.0;
			xpos += dl;

			CoG.x += SectionArea*rho * Pt.x;
			CoG.y += SectionArea*rho * Pt.y;
			CoG.z += SectionArea*rho * Pt.z;
		}
		if(m_VolumeMass>PRECISION) CoG *= 1.0/ m_VolumeMass;
		else                       CoG.Set(0.0, 0.0, 0.0);

		// Next evaluate inertia, assuming each section is a point mass
		xpos = m_FramePosition[0].x;
		for (j=0; j<NSections-1; j++)
		{
			SectionArea = dl * (GetSectionArcLength(xpos)+ GetSectionArcLength(xpos+dl))/2.0;
			Pt.x = xpos + dl/2.0;
			Pt.y = 0.0;
			ux = Getu(Pt.x);
			GetPoint(ux, 0.0, true, Top);
			GetPoint(ux, 1.0, true, Bot);
			Pt.z = (Top.z + Bot.z)/2.0;

			CoGIxx += SectionArea*rho * ( (Pt.y-CoG.y)*(Pt.y-CoG.y) + (Pt.z-CoG.z)*(Pt.z-CoG.z) );
			CoGIyy += SectionArea*rho * ( (Pt.x-CoG.x)*(Pt.x-CoG.x) + (Pt.z-CoG.z)*(Pt.z-CoG.z) );
			CoGIzz += SectionArea*rho * ( (Pt.x-CoG.x)*(Pt.x-CoG.x) + (Pt.y-CoG.y)*(Pt.y-CoG.y) );
			CoGIxz -= SectionArea*rho * ( (Pt.x-CoG.x)*(Pt.z-CoG.z) );

			xpos += dl;
		}
	}
}




void CBody::Duplicate(CBody *pBody)
{
	m_BodyColor      = pBody->m_BodyColor;
	m_NSideLines     = pBody->m_NSideLines;
	m_NStations      = pBody->m_NStations;
	m_nxDegree       = pBody->m_nxDegree;
	m_nhDegree       = pBody->m_nhDegree;
	m_nxPanels       = pBody->m_nxPanels;
	m_nhPanels       = pBody->m_nhPanels;
	m_LineType       = pBody->m_LineType;
	m_bClosedSurface = pBody->m_bClosedSurface;

	m_VolumeMass = pBody->m_VolumeMass;
	m_TotalMass  = pBody->m_TotalMass;
	m_CoG = pBody->m_CoG;
	m_CoGIxx = pBody->m_CoGIxx;
	m_CoGIyy = pBody->m_CoGIyy;
	m_CoGIzz = pBody->m_CoGIzz;
	m_CoGIxz = pBody->m_CoGIxz;

	m_NMass = pBody->m_NMass;
	for(int i=0; i<m_NMass;i++)
	{
		m_MassValue[i] = pBody->m_MassValue[i];
		m_MassPosition[i].Copy(pBody->m_MassPosition[i]);
		m_MassTag[i] = pBody->m_MassTag[i];
	}

	for(int i=0; i<m_NStations; i++)
	{
		m_Frame[i].CopyFrame(pBody->m_Frame+i);
		m_FramePosition[i].Copy(pBody->m_FramePosition[i]);
		m_xPanels[i] = pBody->m_xPanels[i];
	}
	for(int i=0; i<m_NSideLines; i++)
	{
		m_hPanels[i] = pBody->m_hPanels[i];
	}

	SetKnots();
}


bool CBody::ExportDefinition()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	int i, j;
	QString strong,  FileName;

	FileName = m_BodyName;
	FileName.replace("/", " ");

	FileName = QFileDialog::getSaveFileName(pMainFrame, UnitsDlg::tr("Export Body Definition"),
											pMainFrame->m_LastDirName,
											UnitsDlg::tr("Text Format (*.txt)"));
	if(!FileName.length()) return false;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

	QTextStream out(&XFile);

	out << (m_BodyName+"\n\n");
	out << ("BODYTYPE\n");
	if(m_LineType==1) out << ("1        # Flat Panels\n\n");
	if(m_LineType==2) out << ("2        # B-Splines\n\n");

	out << ("OFFSET\n");
	out << ("0.0     0.0     0.0     #Total body offset (Y-coord is ignored)\n\n");

	for(i=0; i<m_NStations; i++)
	{
		out << ("FRAME\n\n");
		for(j=0;j<m_NSideLines; j++)
		{
			strong = QString("%1     %2    %3\n")
					 .arg(m_FramePosition[i].x    * pMainFrame->m_mtoUnit,14,'f',7)
					 .arg(m_Frame[i].m_Point[j].y * pMainFrame->m_mtoUnit,14,'f',7)
					 .arg(m_Frame[i].m_Point[j].z * pMainFrame->m_mtoUnit,14,'f',7);
			out << (strong);
		}
		out << ("\n");
	}

	XFile.close();
	return true;

}


void CBody::ExportGeometry(int nx, int nh)
{
	QString strong, LengthUnit,str, FileName;
	int k,l;
	double u, v;
	CVector Point;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	GetLengthUnit(LengthUnit, pMainFrame->m_LengthUnit);

	FileName = m_BodyName;
	FileName.replace("/", " ");

	int type = 1;

	QString filter =".csv";

	FileName = QFileDialog::getSaveFileName(pMainFrame, UnitsDlg::tr("Export Body Geometry"),
											pMainFrame->m_LastDirName ,
											UnitsDlg::tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
											&filter);
	if(!FileName.length()) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);
	pos = FileName.lastIndexOf(".csv");
	if (pos>0) type = 2;

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);

	if(type==1)	str="";
	else		str=", ";

	out  << (m_BodyName);
	out  << ("\n\n");

	out  << (("Right Surface Points\n"));
	if(type==1) strong = "        x("+LengthUnit+")          y("+LengthUnit+")          z("+LengthUnit+")\n";
	else        strong = " x("+LengthUnit+"),"+"y("+LengthUnit+"),"+"z("+LengthUnit+")\n";
	out  << (strong);

	for (k=0; k<nx; k++)
	{
		strong = QString(("  Cross Section ")+str+"%1\n").arg(k+1,3);
		out  << (strong);

		u = (double)k / (double)(nx-1);

		for (l=0; l<nh; l++)
		{
			v = (double)l / (double)(nh-1);
			GetPoint(u,  v, true, Point);

			strong = QString("   %1"+str+"     %2"+str+"     %3\n")
					 .arg(Point.x * pMainFrame->m_mtoUnit,10,'f',3)
					 .arg(Point.y * pMainFrame->m_mtoUnit,10,'f',3)
					 .arg(Point.z * pMainFrame->m_mtoUnit,10,'f',3);
			out  << (strong);
		}
		out  << ("\n");
	}

	out  << ("\n\n");
}


bool CBody::Gauss(double *A, int n, double *B, int m)
{
	int row, i, j, pivot_row, k;
	double max, dum, *pa, *pA, *A_pivot_row;
	// for each variable find pivot row and perform forward substitution
	pa = A;
	for (row = 0; row < (n - 1); row++, pa += n)
	{
		//  find the pivot row
		A_pivot_row = pa;
		max = fabs(*(pa + row));
		pA = pa + n;
		pivot_row = row;
		for (i=row+1; i < n; pA+=n, i++)
			if ((dum = fabs(*(pA+row))) > max)
			{
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



CFrame * CBody::GetFrame(int iSelect)
{
	if(iSelect>=0 && iSelect<m_NStations)
	{
		return m_Frame + iSelect;
	}
	return NULL;
}

double CBody::GetLength()
{
	return fabs(m_FramePosition[m_NStations-1].x - m_FramePosition[0].x);
}


void CBody::GetPoint(double u, double v, bool bRight, CVector &Pt)
{
	//returns the point corresponding to the parametric values u and v
	//assumes that the knots have been set previously
	CVector V, Vh;
	int i,j;

	if(u>=1.0) u=0.99999999999;
	if(v>=1.0) v=0.99999999999;
	for(i=0; i<m_NStations; i++)
	{
		Vh.Set(0.0,0.0,0.0);
		for(j=0; j<m_NSideLines; j++)
		{
			cs =  SplineBlend(j, m_nhDegree, v, s_hKnots);
			Vh.x += m_FramePosition[i].x    * cs;
			Vh.y += m_Frame[i].m_Point[j].y * cs;
			Vh.z += m_Frame[i].m_Point[j].z * cs;
		}
		bs = SplineBlend(i, m_nxDegree, u, s_xKnots);

		V.x += Vh.x * bs;
		if(bRight) V.y += Vh.y * bs;
		else       V.y -= Vh.y * bs;
		V.z += Vh.z * bs;
	}
	Pt = V;
}


double CBody::GetSectionArcLength(double x)
{
	//NURBS only
	if(m_LineType==1) return 0.0;
	// aproximate arc length, used for inertia estimations
	double length = 0.0;
	double ux = Getu(x);
	CVector Pt, Pt1;
	GetPoint(ux, 0.0, true, Pt1);

	int NPoints = 10;//why not ?
	for(int i=1; i<=NPoints; i++)
	{
		GetPoint(ux, (double)i/(double)NPoints, true, Pt);
		length += sqrt((Pt.y-Pt1.y)*(Pt.y-Pt1.y) + (Pt.z-Pt1.z)*(Pt.z-Pt1.z));
		Pt1.y = Pt.y;
		Pt1.z = Pt.z;
	}
	return length*2.0; //to account for left side.
}


double CBody::Getu(double x)
{
	if(x<=m_FramePosition[0].x)             return 0.0;
	if(x>=m_FramePosition[m_NStations-1].x) return 1.0;
	if(fabs(m_FramePosition[m_NStations-1].x-m_FramePosition[0].x)<0.0000001) return 0.0;

	int i,j, iter=0;
	double u2, u1, b, c, u, v, xx, xh;
	u1 = 0.0; u2 = 1.0;

	v = 0.0;//use top line, but doesn't matter
	while(fabs(u2-u1)>1.0e-6 && iter<200)
	{
		u=(u1+u2)/2.0;
		xx = 0.0;
		for(i=0; i<m_NStations; i++) //browse all points
		{
			xh = 0.0;
			for(j=0; j<m_NSideLines; j++)
			{
				c =  SplineBlend(j, m_nhDegree, v, s_hKnots);
				xh += m_FramePosition[i].x * c;
			}
			b = SplineBlend(i, m_nxDegree, u, s_xKnots);
			xx += xh * b;
		}
		if(xx>x) u2 = u;
		else     u1 = u;
		iter++;
	}
	return (u1+u2)/2.0;
}


double CBody::Getv(double u, CVector r, bool bRight)
{
	double sine = 10000.0;

	if(u<=0.0)             return 0.0;
	if(u>=1.0)             return 0.0;
	if(r.VAbs()<1.0e-5) return 0.0;

	int iter=0;
	double v, v1, v2;

	r.Normalize();
	v1 = 0.0; v2 = 1.0;

	while(fabs(sine)>1.0e-4 && iter<200)
	{
		v=(v1+v2)/2.0;
		GetPoint(u, v, bRight, t_R);
		t_R.x = 0.0;
		t_R.Normalize();//t_R is the unit radial vector for u,v

		sine = (r.y*t_R.z - r.z*t_R.y);

		if(bRight)
		{
			if(sine>0.0) v1 = v;
			else         v2 = v;
		}
		else
		{
			if(sine>0.0) v2 = v;
			else         v1 = v;
		}
		iter++;
	}

	return (v1+v2)/2.0;
}



bool CBody::ImportDefinition()
{
	CFrame NewFrame;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	int res, i, j, Line, NSideLines;
	QString strong, FileName;
	bool bRead;

	double mtoUnit,xo,yo,zo;
	xo = yo = zo = 0.0;

	m_NStations = 0;

	UnitsDlg Dlg;

	Dlg.m_bLengthOnly = true;
	Dlg.m_Length    = pMainFrame->m_LengthUnit;
	Dlg.m_Area      = pMainFrame->m_AreaUnit;
	Dlg.m_Speed     = pMainFrame->m_SpeedUnit;
	Dlg.m_Weight    = pMainFrame->m_WeightUnit;
	Dlg.m_Force     = pMainFrame->m_ForceUnit;
	Dlg.m_Moment    = pMainFrame->m_MomentUnit;
	Dlg.m_Question = UnitsDlg::tr("Choose the length unit to read this file :");
	Dlg.InitDialog();

	if(Dlg.exec() == QDialog::Accepted)
	{
		switch(Dlg.m_Length)
		{
			case 0:{//mdm
				mtoUnit  = 1000.0;
				break;
			}
			case 1:{//cm
				mtoUnit  = 100.0;
				break;
			}
			case 2:{//dm
				mtoUnit  = 10.0;
				break;
			}
			case 3:{//m
				mtoUnit  = 1.0;
				break;
			}
			case 4:{//in
				mtoUnit  = 1000.0/25.4;
				break;
			}
			case 5:{///ft
				mtoUnit  = 1000.0/25.4/12.0;
				break;
			}
			default:{//m
				mtoUnit  = 1.0;
				break;
			}
		}
	}
	else return false;

	QString PathName;
	bool bOK;
	QByteArray textline;
	const char *text;

	PathName = QFileDialog::getOpenFileName(pMainFrame, UnitsDlg::tr("Open File"),
											pMainFrame->m_LastDirName,
											UnitsDlg::tr("Text file (*.txt)"));
	if(!PathName.length())		return false;
	int pos = PathName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = PathName.left(pos);

	QFile XFile(PathName);
	if (!XFile.open(QIODevice::ReadOnly))
	{
		QString strange = UnitsDlg::tr("Could not read the file\n")+PathName;
		QMessageBox::warning(pMainFrame, UnitsDlg::tr("Warning"), strange);
		return false;
	}

	QTextStream in(&XFile);

	Line = 0;
	bRead  = ReadAVLString(in, Line, strong);
	m_BodyName = strong.trimmed();
	//Header data

	bRead = true;
	while (bRead)
	{
		bRead  = ReadAVLString(in, Line, strong);
		if(!bRead) break;
		if (strong.indexOf("BODYTYPE") >=0)
		{
			bRead  = ReadAVLString(in, Line, strong);
			if(!bRead) break;
			res = strong.toInt(&bOK);
			if(bOK)
			{
				m_LineType = res;
				if(m_LineType !=1 && m_LineType !=2) m_LineType = 1;
			}
		}
		else if (strong.indexOf("OFFSET") >=0)
		{
			bRead  = ReadAVLString(in, Line, strong);
			if(!bRead) break;

			textline = strong.toAscii();
			text = textline.constData();
			res = sscanf(text, "%lf  %lf  %lf", &xo, &yo, &zo);

			if(res==3)
			{
				xo /= mtoUnit;
				zo /= mtoUnit;
			}
			//y0 is ignored, body is assumed centered along x-z plane
		}
		else if (strong.indexOf("FRAME", 0)  >=0)
		{
			NSideLines = ReadFrame(in, Line, &NewFrame, mtoUnit);

			if (NSideLines)
			{
				//we need to insert this frame at the proper place in the body
				for(i=0; i<m_NStations; i++)
				{
					if(NewFrame.m_Point[0].x < m_Frame[i].m_Point[0].x)
					{
						for(j=m_NStations; j>i; j--)
						{
							m_Frame[j].CopyFrame(m_Frame+j-1);
						}
						break;
					}
				}
				m_Frame[i].CopyFrame(&NewFrame);
				m_NSideLines = NSideLines;
				m_NStations++;
			}
		}
	}

	XFile.close();

	for(i=1; i<m_NStations; i++)
	{
		if(m_Frame[i].m_NPoints != m_Frame[i-1].m_NPoints)
		{
			QString strong = UnitsDlg::tr("Error reading ")+m_BodyName+UnitsDlg::tr("\nFrames have different number of side points");
			QMessageBox::warning(pMainFrame, UnitsDlg::tr("Error"),strong);
			return false;
		}
	}
	for(i=0; i<m_NStations; i++)
	{
		m_FramePosition[i].x =  m_Frame[i].m_Point[0].x                                             + xo;
		m_FramePosition[i].z = (m_Frame[i].m_Point[0].z + m_Frame[i].m_Point[m_NSideLines-1].z)/2.0 + zo;
		for(j=0; j<m_NSideLines; j++)
		{
			m_Frame[i].m_Point[j].x = 0.0;
			m_Frame[i].m_Point[j].z += zo;
		}
	}

	return true;
}


void CBody::InsertSideLine(int SideLine)
{
	if(SideLine==0) SideLine++;
	for (int i=0; i<m_NStations; i++)
	{
		m_Frame[i].InsertPoint(SideLine);
	}
	m_NSideLines++;
	SetKnots();
}

int CBody::InsertPoint(CVector Real)
{
	//Real is to be inserted in the current frame
	if(m_iActiveFrame<0)
	{
        QMessageBox msgBox;
        msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setWindowTitle(UnitsDlg::tr("Warning"));
		msgBox.setText(UnitsDlg::tr("Please select a Frame before inserting a point"));
        msgBox.exec();

		return -1;
	}
	int i, n;
	n = (m_Frame+m_iActiveFrame)->InsertPoint(Real);
	for (i=0; i<m_NStations; i++)
	{
		if(i!=m_iActiveFrame)
		{
			(m_Frame+i)->InsertPoint(n);
		}
	}
	m_NSideLines++;
	SetKnots();
	return n;
}


int CBody::InsertFrame(CVector Real)
{
	int k, n;

	bool bFound = false;

	if(Real.x<m_FramePosition[0].x)
	{
		for(k=m_NStations; k>0; k--)	
		{
            m_Frame[k].CopyFrame(m_Frame+k-1);
			m_FramePosition[k] = m_FramePosition[k-1];
		}
		n = -1;
		for (k=0; k<m_NSideLines; k++)
		{
			m_Frame[n+1].m_Point[k].x = 0.0;
			m_Frame[n+1].m_Point[k].y = 0.0;
			m_Frame[n+1].m_Point[k].z = 0.0;
		}
		m_FramePosition[0] = Real;
	}
	else if(Real.x>m_FramePosition[m_NStations-1].x)
	{
		n=m_NStations-1;
        m_Frame[n+1].CopyFrame(m_Frame+n);
		
		for (k=0; k<m_NSideLines; k++)
		{
			m_Frame[n+1].m_Point[k].x = 0.0;
			m_Frame[n+1].m_Point[k].y = 0.0;
			m_Frame[n+1].m_Point[k].z = 0.0;
		}
		m_FramePosition[n+1] = Real;
	}
	else
	{
		for (n=0; n<m_NStations-1; n++)
		{
			if(m_FramePosition[n].x<=Real.x  && Real.x <m_FramePosition[n+1].x)
			{
				bFound = true;
				break;
			}
		}
		if (bFound)
		{
			for(k=m_NStations; k>n; k--)	
			{
				m_Frame[k].CopyFrame(m_Frame+k-1);
				m_FramePosition[k] = m_FramePosition[k-1];
			}
			m_FramePosition[n+1] = Real;

			for (k=0; k<m_NSideLines; k++)
			{
				m_Frame[n+1].m_Point[k].x = (m_Frame[n].m_Point[k].x + m_Frame[n+2].m_Point[k].x)/2.0;
				m_Frame[n+1].m_Point[k].y = (m_Frame[n].m_Point[k].y + m_Frame[n+2].m_Point[k].y)/2.0;
				m_Frame[n+1].m_Point[k].z = (m_Frame[n].m_Point[k].z + m_Frame[n+2].m_Point[k].z)/2.0;
			}
		}
	}

	double trans = Real.z - (m_Frame[n+1].m_Point[0].z + m_Frame[n+1].m_Point[m_NSideLines-1].z)/2.0;
		           
	for (k=0; k<m_NSideLines; k++)
	{
		m_Frame[n+1].m_Point[k].z += trans;
	}

	m_iActiveFrame = n+1;
	m_NStations++;
	if(n>=m_NStations)	m_iActiveFrame = m_NStations;
	if(n<=0)			m_iActiveFrame = 0;
	m_iHighlight = -1;
	SetKnots();

	return n+1;
}

void CBody::InterpolateCurve(CVector *D, CVector *P, double *v, double *knots, int degree, int Size)
{
	int i,j;
	double Nij[MAXBODYFRAMES* MAXBODYFRAMES];//MAXBODYFRAMES is greater than MAXSIDELINES
	double RHS[3 * MAXBODYFRAMES];//x, y and z RHS

	//create the matrix
	for(i=0; i<Size; i++)
	{
		for(j=0; j<Size; j++)
		{
			*(Nij+i*Size + j) = SplineBlend(j, degree, v[i], knots);
		}
	}
	//create the RHS
	for(i=0; i<Size; i++)
	{
		RHS[i]        = D[i].x;
		RHS[i+Size]   = D[i].y;
		RHS[i+Size*2] = D[i].z;
	}
	//solve for the new control point coordinates
	Gauss(Nij, Size, RHS, 3);
	
	//reconstruct the control points
	for(i=0; i<Size; i++)
	{
		P[i].x = RHS[i];
		P[i].y = RHS[i+Size];
		P[i].z = RHS[i+Size*2];
	}
}


void CBody::InterpolateSurface()
{
	int i,k;
	double u[MAXBODYFRAMES];
	double v[MAXSIDELINES];
	CVector D[MAXSIDELINES];//are the existing control points
	CVector Q[MAXBODYFRAMES * MAXSIDELINES];//are the intermediate control points after interpolation on each frame
	CVector P[MAXBODYFRAMES * MAXSIDELINES];//are the new resulting control points

	m_nxDegree = 3; m_nhDegree = 3;
	SetKnots();//just to make sure

	for(k=0; k<m_NStations; k++)
	{
		u[k] = Getu(m_FramePosition[k].x);
	}
	u[m_NStations-1] =  0.9999999999;

	for(i=0; i<m_NSideLines; i++)
	{
		v[i] = (double)i/(double)(m_NSideLines-1);
	}
	v[m_NSideLines-1] = 0.9999999999;


	//compute intermediate Control Points Q for Frame k
	for(k=0; k<m_NStations; k++)
	{
		//first create the input points
		for(i=0; i<m_NSideLines; i++)
		{
			D[i].x = m_FramePosition[k].x;
			D[i].y = m_Frame[k].m_Point[i].y;
			D[i].z = m_Frame[k].m_Point[i].z;
		}

		t_R.Set(0.0, 0.0, 1.0);
		for(i=0; i<m_NSideLines-1; i++)
		{
			t_r.Set(0.0, m_Frame[k].m_Point[i].y, m_Frame[k].m_Point[i].z);
			t_r.Normalize();
			if(t_r.VAbs()<1.0e-10) v[i] = 0.0;
			else                   v[i] = acos(t_r.dot(t_R))/PI;
		}
		v[m_NSideLines-1] = 0.9999999999;

		InterpolateCurve(D, Q+k*m_NSideLines, v, s_hKnots, m_nhDegree, m_NSideLines);
	}

	//from the intermediate control points Q, interpolate the final control points P
	for(i=0; i<m_NSideLines; i++)
	{
		for(k=0; k<m_NStations; k++)	//first create the input points
		{
			D[k] = Q[k*m_NSideLines+i];
		}
		InterpolateCurve(D, P+i*m_NStations, u, s_xKnots, m_nxDegree, m_NStations);

		// Copy P array into control points
		for(k=0; k<m_NStations; k++)
		{
			m_Frame[k].m_Point[i] = P[i*m_NStations+k];
		}
	}
}


bool CBody::Intersect(CVector A, CVector B, CVector &I, bool bRight)
{
	if(m_LineType==1) return IntersectPanels(A,B,I);
	else              return IntersectNURBS(A,B,I, bRight);
}


bool CBody::IntersectNURBS(CVector A, CVector B, CVector &I, bool bRight)
{
	//intersect line AB with right or left body surface
	//intersection point is I
	CVector N, tmp, M0, M1;
	double u, v, dist, t, tp;
	int iter = 0;
	int itermax = 20;
	double dmax = 1.0e-6;
	dist = 1000.0;//m

	M0.Set(0.0, A.y, A.z);
	M1.Set(0.0, B.y, B.z);

	if(M0.VAbs()<M1.VAbs())
	{
		tmp = A;		A   = B;		B   = tmp;
	}
	//M0 is the outside Point, M1 is the inside point
	M0 = A; M1 = B; 

	//define which side to intersect with
	if(M0.y>=0.0) bRight = true; else bRight = false;

	if(!IsInNURBSBody(M1))
	{
		//consider no intersection (not quite true in special high dihedral cases)
		I = M1;
		return false;
	}
 
	I = (M0+M1)/2.0; t=0.5;

	while(dist>dmax && iter<itermax)
	{
		//first we get the u parameter corresponding to point I
		tp = t;
		u = Getu(I.x);
		t_Q.Set(I.x, 0.0, 0.0);
		t_r = (I-t_Q);
		v = Getv(u, t_r, bRight);
		GetPoint(u, v, bRight, t_N);

		//project t_N on M0M1 line
		t = - ( (M0.x - t_N.x) * (M1.x-M0.x) + (M0.y - t_N.y) * (M1.y-M0.y) + (M0.z - t_N.z)*(M1.z-M0.z))
			 /( (M1.x -  M0.x) * (M1.x-M0.x) + (M1.y -  M0.y) * (M1.y-M0.y) + (M1.z -  M0.z)*(M1.z-M0.z));

		I.x = M0.x + t * (M1.x-M0.x);
		I.y = M0.y + t * (M1.y-M0.y);
		I.z = M0.z + t * (M1.z-M0.z);

//		dist = sqrt((t_N.x-I.x)*(t_N.x-I.x) + (t_N.y-I.y)*(t_N.y-I.y) + (t_N.z-I.z)*(t_N.z-I.z));
        dist = fabs(t-tp);
		iter++; 
	}
	return dist<dmax;
}

bool CBody::IntersectPanels(CVector A, CVector B, CVector &I)
{
	bool b1, b2, b3, b4, b5;
	int i,k;
	double r,s,t;
	CVector LA, TA, LB, TB, U, V, W, H, D1, D2, N, C, P;
	bool bIntersect = false;

	U = B-A;
	U.Normalize();

	for (i=0; i<m_NStations-1; i++)
	{
		for (k=0; k<m_NSideLines-1; k++)
		{
			//build the four corner points of the Quad Panel
			LB.x =  m_FramePosition[i].x     ;
			LB.y =  m_Frame[i].m_Point[k].y  ;
			LB.z =  m_Frame[i].m_Point[k].z  ;

			TB.x =  m_FramePosition[i+1].x;
			TB.y =  m_Frame[i+1].m_Point[k].y;
			TB.z =  m_Frame[i+1].m_Point[k].z;

			LA.x =  m_FramePosition[i].x     ;
			LA.y =  m_Frame[i].m_Point[k+1].y;
			LA.z =  m_Frame[i].m_Point[k+1].z;

			TA.x =  m_FramePosition[i+1].x;
			TA.y =  m_Frame[i+1].m_Point[k+1].y;
			TA.z =  m_Frame[i+1].m_Point[k+1].z;

			//does it intersect the right panel ?
			C = (LA + LB + TA + TB)/4.0;

			D1 = LA - TB;
			D2 = LB - TA;

			N = D2 * D1;
			N.Normalize();
			
			r = (C.x-A.x)*N.x + (C.y-A.y)*N.y + (C.z-A.z)*N.z ;
			s = (U.x*N.x + U.y*N.y + U.z*N.z);
            if(fabs(s)>0.0)
			{
				t = r/s;
				P = A + U * t;

				// P is inside panel if on left side of each panel side
				W = P  - TA;
				V = TB - TA;
				t_Prod = V*W;
				if(t_Prod.VAbs() <1.0e-4 || t_Prod.dot(N)>=0.0) b1 = true; else b1 = false;

				W = P  - TB;
				V = LB - TB;
				t_Prod = V*W;
				if(t_Prod.VAbs() <1.0e-4 || t_Prod.dot(N)>=0.0) b2 = true; else b2 = false;

				W = P  - LB;
				V = LA - LB;
				t_Prod = V*W;
				if(t_Prod.VAbs() <1.0e-4 || t_Prod.dot(N)>=0.0) b3 = true; else b3 = false;

				W = P  - LA;
				V = TA - LA;
				t_Prod = V*W;
				if(t_Prod.VAbs() <1.0e-4 || t_Prod.dot(N)>=0.0) b4 = true; else b4 = false;

				W = A-P;
				V = B-P;
				if(W.dot(V)<=0.0)      b5 = true; else b5 = false;

				if(b1 && b2 && b3 && b4 && b5)
				{
					bIntersect = true;
					break;
				}
			}

			//does it intersect the left panel ?

			LB.y = -LB.y;
			LA.y = -LA.y;
			TB.y = -TB.y;
			TA.y = -TA.y;

			C = (LA + LB + TA + TB)/4.0;

			D1 = LA - TB;
			D2 = LB - TA;

			N = D2 * D1;
			N.Normalize();
			
			r = (C.x-A.x)*N.x + (C.y-A.y)*N.y + (C.z-A.z)*N.z ;
			s = (U.x*N.x + U.y*N.y + U.z*N.z);
            if(fabs(s)>0.0)
			{
				t = r/s;
				P = A + U * t;

				// P is inside panel if on left side of each panel side
				W = P  - TA;
				V = TB - TA;
				t_Prod = V*W;
				if(t_Prod.VAbs() <1.0e-4 || t_Prod.dot(N)>=0.0) b1 = true; else b1 = false;

				W = P  - TB;
				V = LB - TB;
				t_Prod = V*W;
				if(t_Prod.VAbs() <1.0e-4 || t_Prod.dot(N)>=0.0) b2 = true; else b2 = false;

				W = P  - LB;
				V = LA - LB;
				t_Prod = V*W;
				if(t_Prod.VAbs() <1.0e-4 || t_Prod.dot(N)>=0.0) b3 = true; else b3 = false;

				W = P  - LA;
				V = TA - LA;
				t_Prod = V*W;
				if(t_Prod.VAbs() <1.0e-4 || t_Prod.dot(N)>=0.0) b4 = true; else b4 = false;

				W = A-P;
				V = B-P;
				if(W.dot(V)<=0.0)       b5 = true; else b5 = false;

				if(b1 && b2 && b3 && b4 && b5)
				{
					bIntersect = true;
					break;
				}
			}
		}
		if(bIntersect) break;
	}
	if(bIntersect) I = P;
	return bIntersect;
}



int CBody::IsFramePos(CVector Real, double ZoomFactor)
{
	int k;
	for (k=0; k<m_NStations; k++)
	{
		if (fabs(Real.x-m_FramePosition[k].x)<0.006/ZoomFactor &&
			fabs(Real.y-m_FramePosition[k].z)<0.006/ZoomFactor) return k;
	}
	return -10;
}


bool CBody::IsInNURBSBody(CVector Pt)
{
	double u, v;
	bool bRight;

	u = Getu(Pt.x);
	t_r.Set(0.0, Pt.y, Pt.z);

	if(Pt.y>=0.0) bRight = true;	else bRight = false;

	v = Getv(u, t_r, bRight);
	GetPoint(u, v, bRight, t_N);

	t_N.x = 0.0;

	if(t_r.VAbs()>t_N.VAbs()) return false;
	return true;
}



int CBody::ReadFrame(QTextStream &in, int &Line, CFrame *pFrame, double const &Unit)
{
	double x,y,z;
	double theta[MAXSIDELINES];
	double angle;
	QByteArray textline;
//	const char *text;
	QString strong;
	int i, j, k, res;
	i = 0;
	x=y=z=0.0;

	memset(theta, 0, sizeof(theta));

	bool bRead =true;
	while (bRead)
	{
		if(!ReadAVLString(in, Line,  strong)) bRead = false;
		ReadValues(strong, res, x,y,z);

		if(res!=3)
		{
			bRead = false;
			Rewind1Line(in, Line, strong);
		}
		else 
		{
			angle = atan2(z, y);
			for (j=0; j<i; j++)
			{
				if(angle> theta[j]) 
				{
					for(k=i; k>j; k--)
					{
						theta[k] = theta [k-1];
						pFrame->m_Point[k].x = pFrame->m_Point[k-1].x;
						pFrame->m_Point[k].y = pFrame->m_Point[k-1].y;
						pFrame->m_Point[k].z = pFrame->m_Point[k-1].z;
					}
					break;
				}
			}
			// insert at proper place
			pFrame->m_Point[j].x = x / Unit;
			pFrame->m_Point[j].y = y / Unit;
			pFrame->m_Point[j].z = z / Unit;
			theta[j] = angle;
			i++;
		}
		if(i>=MAXSIDELINES)
		{
			bRead = false;
		}
	}

	pFrame->m_NPoints = i;
	return i;
}


int CBody::RemoveFrame(int n)
{
	int k;
//	m_BodyLine.RemovePoint(n);
	for(k=n; k<m_NStations; k++)
	{
		m_FramePosition[k] = m_FramePosition[k+1];
		m_Frame[k].CopyFrame(m_Frame+k+1);
	}
	m_NStations--;
	m_iActiveFrame = n;
	if(n>=m_NStations)	m_iActiveFrame = m_NStations;
	if(n<=0)			m_iActiveFrame = 0;
	m_iHighlight = -1;
	SetKnots();
	return m_iActiveFrame;
}


void CBody::RemoveActiveFrame()
{
	int k;

	for(k=m_iActiveFrame; k<m_NStations; k++)
	{
		m_FramePosition[k] = m_FramePosition[k+1];
		m_Frame[k].CopyFrame(m_Frame+k+1);
	}
	m_NStations--;

	m_iHighlight = -1;
	SetKnots();
}


void CBody::RemoveSideLine(int SideLine)
{
	for (int i=0; i<m_NStations; i++)
	{
		m_Frame[i].RemovePoint(SideLine);
	}
	m_NSideLines--;
	SetKnots();
}



void CBody::Scale(double XFactor, double YFactor, double ZFactor, bool bFrameOnly, int FrameID)
{
	int i,j;
	for (i=0; i<m_NStations; i++)
	{
		if((bFrameOnly &&  i==FrameID) || !bFrameOnly)
		{
			if(!bFrameOnly) m_FramePosition[i].x *= XFactor;
			m_FramePosition[i].y *= YFactor;
			m_FramePosition[i].z *= ZFactor;

			for(j=0; j<m_Frame[i].m_NPoints; j++)
			{
				m_Frame[i].m_Point[j].y *= YFactor;
				m_Frame[i].m_Point[j].z *= ZFactor;
			}
		}
	}
	ComputeCenterLine();
}


bool CBody::SerializeBody(QDataStream &ar, bool bIsStoring, int ProjectFormat)
{
	int ArchiveFormat;
	int i,k;
	float f,g,h;

	if(bIsStoring)
	{
		if(ProjectFormat>=5)      ar << 1004;
		else if(ProjectFormat==4) ar << 1002;
		//1004 : QFLRv0.03	: added mass properties for inertia calculations
		//1003 : QFLR5 v0.02 : added body description field
		//1002 : Added axial and hoop mesh panel numbers for linetype fuselage
		//1001 : Added bunching parameter
		//1000 : first format
		WriteCString(ar, m_BodyName);
		if(ProjectFormat>=5) WriteCString(ar, m_BodyDescription);

		WriteCOLORREF(ar, m_BodyColor);
		ar << m_LineType;
		ar << m_NSideLines;
		ar << m_NStations;
		ar << m_iRes;
		ar << m_nxDegree << m_nhDegree;
		ar << m_nxPanels << m_nhPanels;
		ar << (float)m_Bunch;

		for(k=0; k<m_NStations; k++)	  ar << m_xPanels[k];
		for(k=0; k<m_NSideLines; k++) ar << m_hPanels[k];

		if(m_bClosedSurface) ar<<1; else ar <<0;

		for(k=0; k<m_NStations; k++)
		{
			m_Frame[k].SerializeFrame(ar, bIsStoring);
		}
		ar << m_NStations;
		for (k=0; k<m_NStations;k++){
			ar << (float)m_FramePosition[k].x << (float)m_FramePosition[k].z;
		}
		if(ProjectFormat>=5)
		{
			ar << (float)m_VolumeMass;
			ar << m_NMass;
			for(i=0; i<m_NMass; i++) ar << (float)m_MassValue[i];
			for(i=0; i<m_NMass; i++) ar << (float)m_MassPosition[i].x << (float)m_MassPosition[i].y << (float)m_MassPosition[i].z;
			for(i=0; i<m_NMass; i++)  WriteCString(ar, m_MassTag[i]);
		}
		ar << 0.0f << 0.0f;
	}
	else
	{
		ar >> ArchiveFormat;
		if(ArchiveFormat<1000 || ArchiveFormat>1100) return false;

		ReadCString(ar, m_BodyName);
		if(ArchiveFormat>=1003) ReadCString(ar, m_BodyDescription);

		ReadCOLORREF(ar, m_BodyColor);
		ar >> m_LineType;
		ar >> m_NSideLines;
		ar >> m_NStations;
		ar >> m_iRes;
		ar >> m_nxDegree >> m_nhDegree;
		ar >> m_nxPanels >> m_nhPanels;

		if(ArchiveFormat>=1001)
		{
			ar >> f; m_Bunch = f;
		}

		if(ArchiveFormat>=1002)
		{
			for(k=0; k<m_NStations; k++)	  ar >> m_xPanels[k];
			for(k=0; k<m_NSideLines; k++) ar >> m_hPanels[k];
		}

		ar >> k;
		if(k!=0 && k!=1)
		{
			return false;
		}
		if (k) m_bClosedSurface = true; else m_bClosedSurface = false;

		for(k=0; k<m_NStations; k++)
		{
			m_Frame[k].SerializeFrame(ar, bIsStoring);
		}
		//Serialize Bodyline
		ar >> m_NStations;
		for (k=0; k<m_NStations;k++)
		{
			ar >> f; m_FramePosition[k].x =f;
			ar >> f; m_FramePosition[k].z =f;
		}
		if(ArchiveFormat>=1004)
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
		ar >> f;
		ar >> f;

		SetKnots();

	}
	return true;
}



void CBody::SetPanelPos()
{
	int i;
	for(i=0; i<=m_nxPanels; i++)
	{
		s_XPanelPos[i] =(double)i/(double)m_nxPanels;
	}
	return;
	double y, x;
	double a = (m_Bunch+1.0)*.48 ;
	a = 1./(1.0-a);

	double norm = 1/(1+exp(0.5*a));

	for(i=0; i<=m_nxPanels; i++)
	{
		x = (double)(i)/(double)m_nxPanels;
		y = 1.0/(1.0+exp((0.5-x)*a));
		s_XPanelPos[i] =0.5-((0.5-y)/(0.5-norm))/2.0;
	}
}


void CBody::SetKnots()
{
	int j;
	double b;
	m_nxDegree = qMin(m_nxDegree, m_NStations);
	m_nxKnots  = m_nxDegree + m_NStations   + 1;
	b = (double)(m_nxKnots-2*m_nxDegree-1);


	// x-dir knots
	for (j=0; j<m_nxKnots; j++)
	{
		if (j<m_nxDegree+1)  s_xKnots[j] = 0.0;
		else
		{
			if (j<m_NStations)
			{
				if(fabs(b)>0.0) s_xKnots[j] = (double)(j-m_nxDegree)/b;
				else           s_xKnots[j] = 1.0;
			}
			else s_xKnots[j] = 1.0;
		}
	}

	//hoop knots

	m_nhDegree = qMin(m_nhDegree, m_NSideLines);
	m_nhKnots  = m_nhDegree + m_NSideLines + 1;
	b = (double)(m_nhKnots-2*m_nhDegree-1);
	for (j=0; j<m_nhKnots; j++)
	{
		if (j<m_nhDegree+1)  s_hKnots[j] = 0.0;
		else
		{
			if (j<m_NSideLines)
			{
				if(fabs(b)>0.0) s_hKnots[j] = (double)(j-m_nhDegree)/b;
				else           s_hKnots[j] = 1.0;
			}
			else s_hKnots[j] = 1.0;
		}
	}
}

double CBody::SplineBlend(int const &index, int const &p, double const &t, double *knots)
{
//	Calculate the blending value, this is done recursively.
//	If the numerator and denominator are 0 the expression is 0.
//	If the denominator is 0 the expression is 0
//
//	   index   is the control point's index
//	   p       is the spline's degree
//	   t       is the spline parameter
//

	if (p == 0)
	{
		if ((knots[index] <= t) && (t < knots[index+1]) )	value = 1.0;
//		else if (abs(knots[index]-knots[index+1])<pres)	    value = 0.0;
		else 						                value = 0.0;
	}
	else
	{
		if (fabs(knots[index+p] - knots[index])<eps && fabs(knots[index+p+1] - knots[index+1])<eps)
			value = 0.0;
		else if (fabs(knots[index+p] - knots[index])<eps)
			value = (knots[index+p+1]-t) / (knots[index+p+1]-knots[index+1])  * SplineBlend(index+1, p-1, t, knots);
		else if (fabs(knots[index+p+1] - knots[index+1])<eps)
			value = (t-knots[index])     / (knots[index+p] - knots[index])    * SplineBlend(index,   p-1, t, knots);
		else
			value = (t-knots[index])     / (knots[index+p]  -knots[index])    * SplineBlend(index,   p-1, t, knots) +
					(knots[index+p+1]-t) / (knots[index+p+1]-knots[index+1])  * SplineBlend(index+1 ,p-1, t, knots);
	}
	return value;
}



void CBody::Translate(double XTrans, double YTrans, double ZTrans, bool bFrameOnly, int FrameID)
{
	int i,j;
	for (i=0; i<m_NStations; i++)
	{
		if((bFrameOnly &&  i==FrameID) || !bFrameOnly)
		{
			m_FramePosition[i].x += XTrans;
//			m_FramePosition[i].y += YTrans;
			m_FramePosition[i].z += ZTrans;

			for(j=0; j<m_Frame[i].m_NPoints; j++)
			{
//				m_Frame[i].m_Point[j].x += XTrans;
				m_Frame[i].m_Point[j].z += ZTrans;
			}
		}
	}
	ComputeCenterLine();
}


void CBody::UpdateFramePos(int iFrame)
{
	int i;
	double z;
	z  = m_FramePosition[iFrame].z;
	z -= (m_Frame[iFrame].m_Point[0].z + m_Frame[iFrame].m_Point[m_NSideLines-1].z)/2.0;

	for(i=0; i<m_NSideLines; i++)
	{
		m_Frame[iFrame].m_Point[i].z += z;
	}
}

double CBody::GetTotalMass()
{
	double TotalMass = m_VolumeMass;
	for(int i=0; i<m_NMass; i++)
		TotalMass += m_MassValue[i];
	return TotalMass;
}



