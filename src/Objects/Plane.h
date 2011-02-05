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

#ifndef CPLANE_H
#define CPLANE_H

#include "Wing.h"
#include "Body.h"


class CPlane
{
//	friend class QMiarex;
//	friend class MainFrame;
//	friend class CBody;
//	friend class CWing;
//	friend class CWPolar;
//	friend class WPolarDlg;
//	friend class InertiaDlg;
	friend class PlaneDlg;
//	friend class PanelAnalysisDlg;
//	friend class ManageBodiesDlg;
//	friend class StabPolarDlg;
//	friend class ManageUFOsDlg;


public:
	CPlane();
	static void SetParents(void *pMainFrame, void*pMiarex);


	double TotalMass();
	double TailVolume();

	bool BiPlane(){return m_bBiplane;}
	const QString& PlaneName() const {return m_PlaneName;}
	QString& rPlaneName() {return m_PlaneName;}
	const QString& PlaneDescription() const {return m_PlaneDescription;}
	QString& rPlaneDescription() {return m_PlaneDescription;}
	CWing *Wing()  {return &m_Wing;}
	CWing *Wing2() {if(m_bBiplane) return &m_Wing2; else return NULL;}
	CWing *Stab()  {if(m_bStab)    return &m_Stab;  else return NULL;}
	CWing *Fin()   {if(m_bFin)     return &m_Fin;   else return NULL;}
	CBody *Body()  {if(m_bBody)    return m_pBody;  else return NULL;}
	CVector CoG()  {return m_CoG;}

	void Duplicate(CPlane *pPlane);
	bool SerializePlane(QDataStream &ar, bool bIsStoring, int ProjectFormat);
	void ComputePlane(void);
	void CreateSurfaces();
	void RenameWings();
	void SetBody(CBody *pBody);

	void ComputeVolumeInertia(double &Mass, CVector &CoG, double &Ixx, double &Iyy, double &Izz, double &Ixz);
	void ComputeBodyAxisInertia();

	CVector BodyPos();
	CVector WingLE(int iw);
	double WingTiltAngle(int iw);

private:
	static void * s_pMiarex;
	static void * s_pMainFrame;
	CWing m_Wing, m_Wing2;
	CWing m_Stab;
	CWing m_Fin;
	CBody *m_pBody;

	bool m_bBody;
	bool m_bBiplane;

	double m_VolumeMass, m_TotalMass;

	QString m_PlaneDescription;
	bool m_bFin, m_bStab;
	double m_TailVolume;
	CVector m_CoG;
	QString m_PlaneName;

	CVector m_WingLE[MAXWINGS];
	double m_WingTiltAngle[MAXWINGS];
	CVector m_BodyPos;

public:
	double m_CoGIxx,m_CoGIyy,m_CoGIzz,m_CoGIxz;
	int m_NMass;
	QString m_MassTag[MAXMASSES];
	double m_MassValue[MAXMASSES];
	CVector m_MassPosition[MAXMASSES];
	bool m_bDoubleFin, m_bSymFin, m_bDoubleSymFin;

};

#endif
