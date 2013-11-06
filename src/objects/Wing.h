/****************************************************************************

	Wing Class
	Copyright (C) 2005-2013 Andre Deperrois adeperrois@xflr5.com

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

/*! \file
 *
 * This class :
 *	- defines the wing object
 *	- provides the methods for the calculation of the wing geometric properties
 *	- provides methods for LLT, VLM and Panel methods
 * The data is stored in International Standard Units, i.e. meters, kg, and seconds
 * Angular data is stored in degrees
 */


#ifndef WING_H
#define WING_H


/**
*@brief  This class defines the wing object, provides the methods for the calculation of the wing geometric properties, and
		 provides methods for LLT, VLM and Panel methods

 * The data is stored in International Standard Units, i.e. meters, kg, and seconds.
 * Angular data is stored in degrees
*/


#include "WPolar.h"
#include "Surface.h"
#include "Panel.h"
#include "WingSection.h"

class Wing
{
	friend class QMiarex;
	friend class MainFrame;
	friend class WPolar;
	friend class PlaneOpp;
	friend class Plane;
	friend class Surface;
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

	Wing();   // standard constructor

	void ImportDefinition(QString path_to_file);
	void ExportDefinition(QString path_to_file);

	void GetViewYZPos(double xrel, double y, double &yv, double &zv, int pos);

	double yrel(double SpanPos);
	double Chord(double yob);
	double Offset(double yob);
	double Dihedral(double yob);
	double Twist(double y);
	double AverageSweep();
	double TotalMass();
	double C4(double yob, double xRef);
	double ZPosition(double y);
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
							 WPolar *pWPolar, Panel *pWakePanel, CVector *pWakeNode);
    void PanelComputeOnBody(double QInf, double Alpha, double *Cp, double *Gamma, double &XCP, double &YCP, double &ZCP,
						 double &GCm, double &VCm, double &ICm, double &GRm, double &GYm, double &VYm,double &IYm,
						 WPolar *pWPolar, CVector CoG);


	void PanelComputeViscous(double QInf, double Alpha, WPolar *pWPolar, double &WingVDrag, bool bViscous, QString &OutString);
	void PanelComputeBending(bool bThinSurface);

	bool IsWingPanel(int nPanel);
	bool IsWingNode(int nNode);

	void GetFoils(Foil **pFoil0, Foil **pFoil1, double y, double &t);
	void Duplicate(Wing *pWing);
	void ComputeChords(int NStation=0);
	void ComputeChords(int NStation, double *chord, double *offset, double *twist);
	void ComputeGeometry();
	void ComputeVolumeInertia(CVector &CoG, double &CoGIxx, double &CoGIyy, double &CoGIzz, double &CoGIxz);
	void ComputeBodyAxisInertia();

//	void InsertSection(double TPos, double TChord, double TOffset, double TZPos, double Twist, QString Foil,int NChord, int NSpan, int SSpan);
	void ScaleSweep(double NewSweep);
	void ScaleTwist(double NewTwist);
	void ScaleSpan(double NewSpan);
	void ScaleChord(double NewChord);
	bool SerializeWing(QDataStream &ar, bool bIsStoring);
	bool ExportAVLWing(QTextStream &out, int index, double x, double y, double z, double Thetax, double Thetay);

	bool IsWingOut()      {return m_bWingOut;}
	bool IsFin()          {return m_bIsFin;}
	bool IsDoubleFin()    {return m_bDoubleFin;}
	bool IsSymFin()       {return m_bSymFin;}
	bool IsDoubleSymFin() {return m_bDoubleSymFin;}

	void InsertSection(int iSection);
	bool AppendWingSection();
	bool AppendWingSection(double Chord, double Twist, double Pos, double Dihedral, double Offset, int NXPanels, int NYPanels, int XPanelDist, int YPanelDist, QString RightFoilName, QString LeftFoilName);
	void RemoveWingSection(int const iSection);
	int NWingSection() {return m_WingSection.count();}

	double &Chord(const int &iSection);
	double &Twist(const int &iSection);
	double &YPosition(const int &iSection);
	double &Dihedral(const int &iSection);
	double &Offset(const int &iSection);
	double &Length(const int &iSection);
	double &YProj(const int &iSection);
	double &ZPosition(const int &iSection);

	int &NXPanels(const int &iSection);
	int &NYPanels(const int &iSection);
	int &XPanelDist(const int &iSection);
	int &YPanelDist(const int &iSection);

	QString &RightFoil(const int &iSection);
	QString &LeftFoil(const int &iSection);

	double RootChord()  {return m_WingSection.first()->m_Chord;}
	double TipChord()   {return m_WingSection.last()->m_Chord;}
	double TipTwist()   {return m_WingSection.last()->m_Twist;}
	double TipOffset()  {return m_WingSection.last()->m_Offset;}
	double TipPos()     {return m_WingSection.last()->m_YPosition;}

//__________________________Variables_______________________
private:
	static void* s_pMainFrame;		//pointer to the Frame window
	static void* s_pMiarex;	//pointer to the Miarex Application window
	static void* s_p3DPanelDlg;//pointer to the 3DPanel analysis dialog class
	static CVector *m_pWakeNode;			//pointer to the VLM wake node array
	static Panel *m_pWakePanel;			//pointer to the VLM Wake Panel array


	static bool s_bVLMSymetric;	// true if the vlm calculation is symmetric

	bool m_bWingOut;	// true if the wing OpPoint is outside the flight envelope of the available Type1 polars
	bool m_bSymetric;	// true if the wing's geometry is symmetric
	bool m_bIsFin, m_bDoubleFin, m_bSymFin, m_bDoubleSymFin; //fin flags

	int m_NStation;		// number of stations for wing calculation
	int m_nNodes;		// the number of VLM panel nodes
	int m_AVLIndex;		// need to export to AVL

	int m_nFlaps;		// the number of T.E. flaps, numbered from left wing to right wing
	double m_FlapMoment[2*MAXSPANSECTIONS];

	double m_QInf0;

	double m_VolumeMass, m_TotalMass;    //for inertia calculations
//	int m_NMass; //number of point mass values
	QList<double> m_MassValue;
	QList<CVector> m_MassPosition;
	QStringList m_MassTag;

	double m_GChord;	// mean geometric chord
	double m_yMac;		// mean aerodynamic chord span position
	double m_CL;		//Lift
	double m_InducedDrag, m_ViscousDrag;
    double m_XCP, m_YCP, m_ZCP;	// Centre of pressure's position
	double m_Maxa; 		// Used in LLT
	double m_GCm, m_VCm, m_ICm; // Pitching Moment
	double m_GYm, m_VYm, m_IYm;		// Induced Yawing Moment
	double m_GRm;		// Geometric Rolling Moment


	// Span Coefficients  resulting from VLM or LLT calculation
	double m_Ai[MAXSPANSTATIONS+1];		//Induced angles, in degrees
	double m_Cl[MAXSPANSTATIONS+1];		//Lift coefficient at stations
	double m_ICd[MAXSPANSTATIONS+1];		//Induced Drag coefficient at stations
	double m_PCd[MAXSPANSTATIONS+1];		//Viscous Drag coefficient at stations
	double m_Re[MAXSPANSTATIONS+1];		//Reynolds number at stations
	double m_XTrTop[MAXSPANSTATIONS+1];		//Upper transition location at stations
	double m_XTrBot[MAXSPANSTATIONS+1];		//Lower transition location at stations
	double m_Cm[MAXSPANSTATIONS+1];			//Total pitching moment coefficient at stations
	double m_CmAirf[MAXSPANSTATIONS+1];		//Aill part of Pitching moment coefficient at stations
	double m_XCPSpanRel[MAXSPANSTATIONS+1];	//Center of Pressure pos at stations
	double m_XCPSpanAbs[MAXSPANSTATIONS+1];	//Center of Pressure pos at stations
	double m_Chord[MAXSPANSTATIONS+1];		//chord at stations
	double m_Offset[MAXSPANSTATIONS+1];		//offset at LLT stations
	double m_Twist[MAXSPANSTATIONS+1];		//twist at LLT stations
	double m_StripArea[MAXSPANSTATIONS+1];
	double m_BendingMoment[MAXSPANSTATIONS+1];	//bending moment at stations
	double m_SpanPos[MAXSPANSTATIONS+1];	//span positions of LLT stations
	double m_xHinge[MAXCHORDPANELS];		//chorwise position of flap hinges
	double m_xPanel[MAXCHORDPANELS];	//chorwise position of VLM panels

	QString m_WingDescription;

//	QStringList m_RightFoil;			// name of the right foils
//	QStringList m_LeftFoil;			// name of the left foils

	CVector m_Vd[MAXSPANSTATIONS];		// downwash vector at span stations
	CVector m_F[MAXSPANSTATIONS];		// lift vector at span stations

public:	
	QString m_WingName;	//the wing's name

	QColor m_WingColor;
	
	int m_NSurfaces; 	// The number of VLM Surfaces (=2 x Wing Panels)
//	int m_NPanel;		// number of span panels in wing definition

	Surface m_Surface[2*MAXSPANSECTIONS];

	QList<WingSection*> m_WingSection;
	
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
	Panel *m_pPanel;			//pointer to the VLM Panel array

};

#endif
