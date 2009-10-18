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

// 
#include "Wing.h"
#include "Body.h"

/////////////////////////////////////////////////////////////////////////////
// CPlane dialog

class CPlane
{
	friend class QMiarex;
	friend class MainFrame;
	friend class CBody;
	friend class CWing;
	friend class PlaneDlg;
	friend class VLMAnalysisDlg;
	friend class ManageBodiesDlg;
	friend class CtrlPolarDlg;
	friend class ManageUFOsDlg;
	friend class InertiaDlg;
// Construction
public:
	CPlane();   // standard constructor


protected:

	static void * s_pMiarex;
	static void * s_pMainFrame;
	CBody *m_pBody;
	CWing m_Wing, m_Wing2;
	CWing m_Stab;
	CWing m_Fin;
	bool m_bBody;
	bool m_bActive;
	bool m_bFin, m_bStab;
	bool m_bDoubleFin, m_bSymFin, m_bDoubleSymFin;
	bool m_bBiplane;
	double m_StabTilt;
	double m_FinTilt;
	double m_WingTilt, m_WingTilt2;
	double m_Volume;
	double m_XCmRef;
	double m_TailVolume;

	int m_NMass;
	double m_MassValue[MAXMASSES];
	CVector m_MassPosition[MAXMASSES];
	QString m_MassTag[MAXMASSES];
	CVector m_CoG;
	double m_CoGIxx, m_CoGIyy, m_CoGIzz, m_CoGIxz;

	CVector m_LEStab, m_LEFin, m_LEWing, m_LEWing2, m_BodyPos;
	QString m_PlaneName;
	QString m_PlaneDescription;

private:
	bool SerializePlane(QDataStream &ar, bool bIsStoring, int ProjectFormat);
//	bool HasResults();
	void Duplicate(CPlane *pPlane);
	void ComputePlane(void);
	void ComputeInertia(double const & Mass);


public:

};

#endif
