/****************************************************************************

	Wing Class
	Copyright (C) 2005-2008 Andre Deperrois adeperrois@xflr5.com

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

#ifndef CWING_H
#define CWING_H

#include "WPolar.h"
#include "Surface.h"
#include "Panel.h"


class CWing
{
	friend class QMiarex;
	friend class MainFrame;
	friend class CWPolar;
	friend class CPOpp;
	friend class CPlane;
	friend class CSurface;
	friend class WingDlg;
	friend class LLTAnalysisDlg;
	friend class LLTAnalysis;
	friend class PanelAnalysisDlg;
	friend class PlaneDlg;
	friend class ImportWingDlg;
	friend class WPolarDlg;
	friend class StabPolarDlg;
	friend class GL3dWingDlg;
	friend class ManageUFOsDlg;
	friend class InertiaDlg;
	friend class StabViewDlg;

	// Construction
public:

	CWing();   // standard constructor

	void GetViewYZPos(double xrel, double y, double &yv, double &zv, int pos);

	double yrel(double SpanPos);
	double Chord(double yob);
	double Offset(double yob);
	double Dihedral(double yob);
	double Twist(double y);
	double AverageSweep();
	double TotalMass();
	double C4(double yob, double xRef);
	double ZPos(double y);
	double Beta(int m, int k);

	CVector CoG() {return m_CoG;}
	const QString& WingName() const {return m_WingName;}
	QString& rWingName() {return m_WingName;}
	const QString& WingDescription() const {return m_WingDescription;}
	QString& rWingDescription() {return m_WingDescription;}


	void CreateSurfaces(CVector const &T, double XTilt, double YTilt);//generic surface, LLT, VLM or Panel

	int  VLMGetPanelTotal(void);
	void VLMSetBending();

	void PanelTrefftz(double QInf, double Alpha, double *Mu, double *Sigma, int pos, CVector &Force, double &WingIDrag,
							 CWPolar *pWPolar, CPanel *pWakePanel, CVector *pWakeNode);
	void PanelComputeOnBody(double QInf, double Alpha, double *Cp, double *Gamma, double &XCP, double &YCP,
						 double &GCm, double &VCm, double &ICm, double &GRm, double &GYm, double &VYm,double &IYm,
						 CWPolar *pWPolar, CVector CoG);


	void PanelComputeViscous(double QInf, double Alpha, CWPolar *pWPolar, double &WingVDrag, bool bViscous, QString &OutString);
	void PanelSetBending(bool bThinSurface);

	bool IsWingPanel(int nPanel);
	bool IsWingNode(int nNode);

	void GetFoils(CFoil **pFoil0, CFoil **pFoil1, double y, double &t);
	void Duplicate(CWing *pWing);
	void ComputeChords(int NStation=0);
	void ComputeChords(int NStation, double *Chord, double *Offset, double *Twist);
	void ComputeDihedrals();
	void ComputeGeometry();
	void ComputeVolumeInertia(CVector &CoG, double &CoGIxx, double &CoGIyy, double &CoGIzz, double &CoGIxz);
	void ComputeBodyAxisInertia();

	void InsertSection(double TPos, double TChord, double TOffset, double TZPos, double Twist, QString Foil,int NChord, int NSpan, int SSpan);
	void SetSweep(double Sweep);
	void SetTwist(double Twist);
	void ScaleSpan(double NewSpan);
	void ScaleChord(double NewChord);
	bool SerializeWing(QDataStream &ar, bool bIsStoring, int ProjectFormat);
	bool ExportAVLWing(QTextStream &out, int index, double x, double y, double z, double Thetax, double Thetay);

	bool IsWingOut()      {return m_bWingOut;}
	bool IsFin()          {return m_bIsFin;}
	bool IsDoubleFin()    {return m_bDoubleFin;}
	bool IsSymFin()       {return m_bSymFin;}
	bool IsDoubleSymFin() {return m_bDoubleSymFin;}

//	bool SplineInterpolation(int n, double *x, double *y,  double *a, double *b, double *c, double *d);
//	double GetInterpolation(double t, double *y, int m, double *a, double *b, double *c, double *d);


//__________________________Variables_______________________
private:
	static void* s_pMainFrame;		//pointer to the Frame window
	static void* s_pMiarex;	//pointer to the Miarex Application window
	static void* s_p3DPanelDlg;//pointer to the 3DPanel analysis dialog class
	static CVector *m_pWakeNode;			//pointer to the VLM wake node array
	static CPanel *m_pWakePanel;			//pointer to the VLM Wake Panel array

	QFile * m_pXFile;	// a pointer to the output .log file

	static bool s_bVLMSymetric;	// true if the vlm calculation is symmetric

	bool m_bWingOut;	// true if the wing OpPoint is outside the flight envelope of the available Type1 polars
	bool m_bSymetric;	// true if the wing's geometry is symmetric
	bool m_bIsFin, m_bDoubleFin, m_bSymFin, m_bDoubleSymFin; //fin flags

	int m_NStation;		// number of stations for wing calculation
	int m_nNodes;		// the number of VLM panel nodes
	int m_AVLIndex;		// need to export to AVL

	int m_nFlaps;		// the number of T.E. flaps, numbered from left wing to right wing
	double m_FlapMoment[2*MAXPANELS];

	double m_QInf0;

	double m_VolumeMass, m_TotalMass;    //for inertia calculations
	int m_NMass;
	double m_MassValue[MAXMASSES];
	CVector m_MassPosition[MAXMASSES];
	QString m_MassTag[MAXMASSES];

	double m_GChord;	// mean geometric chord
	double m_yMac;		// mean aerodynamic chord span position
	double m_CL;		//Lift
	double m_InducedDrag, m_ViscousDrag;
	double m_XCP, m_YCP;	// Centre of pressure's position
	double m_Maxa; 		// Used in LLT
	double m_GCm, m_VCm, m_ICm; // Pitching Moment
	double m_GYm, m_VYm, m_IYm;		// Induced Yawing Moment
	double m_GRm;		// Geometric Rolling Moment

	int m_NXPanels[MAXPANELS+1]; 		// VLM Panels along chord, for each Wing Panel
	int m_NYPanels[MAXPANELS+1]; 		// VLM Panels along span, for each Wing Panel
	int m_XPanelDist[MAXPANELS+1];		// VLM Panel distribution type, for each Wing Panel
	int m_YPanelDist[MAXPANELS+1];		// VLM Panel distribution type, for each Wing Panel


	// Span Coefficients  resulting from VLM or LLT calculation
	double m_Ai[MAXSTATIONS+1];		//Induced angles, in degrees
	double m_Cl[MAXSTATIONS+1];		//Lift coefficient at stations
	double m_ICd[MAXSTATIONS+1];		//Induced Drag coefficient at stations
	double m_PCd[MAXSTATIONS+1];		//Viscous Drag coefficient at stations
	double m_Re[MAXSTATIONS+1];		//Reynolds number at stations
	double m_XTrTop[MAXSTATIONS+1];		//Upper transition location at stations
	double m_XTrBot[MAXSTATIONS+1];		//Lower transition location at stations
	double m_Cm[MAXSTATIONS+1];			//Total pitching moment coefficient at stations
	double m_CmAirf[MAXSTATIONS+1];		//Aill part of Pitching moment coefficient at stations
	double m_XCPSpanRel[MAXSTATIONS+1];	//Center of Pressure pos at stations
	double m_XCPSpanAbs[MAXSTATIONS+1];	//Center of Pressure pos at stations
	double m_Chord[MAXSTATIONS+1];		//chord at stations
	double m_Offset[MAXSTATIONS+1];		//offset at LLT stations
	double m_Twist[MAXSTATIONS+1];		//twist at LLT stations
	double m_StripArea[MAXSTATIONS+1];
	double m_BendingMoment[MAXSTATIONS+1];	//bending moment at stations
	double m_SpanPos[MAXSTATIONS+1];	//span positions of LLT stations
	double m_xHinge[MAXCHORDPANELS];		//chorwise position of flap hinges
	double m_xPanel[MAXCHORDPANELS];	//chorwise position of VLM panels

	QString m_WingDescription;

	QStringList m_RFoil;			// name of the right foils
	QStringList m_LFoil;			// name of the left foils

	CVector m_Vd[MAXSTATIONS];		// downwash vector at span stations
	CVector m_F[MAXSTATIONS];		// lift vector at span stations

public:	
	QString m_WingName;	//the wing's name

	QColor m_WingColor;
	
	int m_NSurfaces; 	// The number of VLM Surfaces (=2 x Wing Panels)
	int m_NPanel;		// number of span panels in wing definition

	CSurface m_Surface[2*MAXPANELS];

	double m_TChord[MAXPANELS+1];		// Chord length at each panel side
	double m_TLength[MAXPANELS+1];		// the length of each panel
	double m_TPos[MAXPANELS+1];		// b-position of each panel end on developed surface
	double m_TYProj[MAXPANELS+1];		// b-position of each panel end projected on horizontal surface
	double m_TOffset[MAXPANELS+1];		// b-position of each panel end
	double m_TDihedral[MAXPANELS+1];	// b-position of each panel end
	double m_TZPos[MAXPANELS+1];		// vertical offset - calculation result only
	double m_TTwist[MAXPANELS+1];		//Twist value of each foil (measured to the wing root)
	
	double m_MAChord;	// mean aerodynamic chord
	double m_PlanformSpan;
	double m_ProjectedSpan;
	double m_PlanformArea;		// wing surface projected on xy plane;
	double m_ProjectedArea;		// wing surface projected on xy plane;
	double m_AR;		// Aspect ratio
	double m_TR;		// Taper ratio
	double m_CoGIxx,m_CoGIyy,m_CoGIzz,m_CoGIxz;
	CVector m_CoG;

	int m_MatSize;	// Max Size for the VLMMatrix
	CPanel *m_pPanel;			//pointer to the VLM Panel array

};

#endif
