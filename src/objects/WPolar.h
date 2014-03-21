/****************************************************************************

	WPolar Class
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


/** @file
 *
 * This class defines the polar object for the 3D analysis of wings and planes
 *
 */



#ifndef WPOLAR_H
#define WPOLAR_H


/**
*@brief
*	This class defines the polar object used in 2D and 3D calculations
*
	The class stores both the analysis parameters and the analysis results.

	Each instance of this class is uniquely associated to an instance of a Wing or a Plane object.
	The data is stored in International Standard Units, i.e. meters, seconds, kg, and Newtons.
	Angular data is stored in degrees
*/


#include "WingOpp.h"
#include "PlaneOpp.h"
#include <QVarLengthArray>

#define MAXPOLARPOINTS   1000     /**< The max number of points on a polar. */

class WPolar
{
	friend class Wing;
	friend class QMiarex;
	friend class Objects3D;
	friend class MainFrame;
	friend class WPolarDlg;
	friend class StabPolarDlg;
	friend class LLTAnalysisDlg;
	friend class LLTAnalysis;
	friend class PanelAnalysis;
	friend class PanelAnalysisDlg;
	friend class ObjectPropsDlg;
	friend class StabViewDlg;
	friend class ManageBodiesDlg;
	friend class EditPlrDlg;
	friend class WingOpp;

public:
	WPolar();

	void AddPlaneOpPoint(PlaneOpp* pPOpp);
	void ReplacePOppDataAt(int pos, PlaneOpp *pPOpp);
	void InsertPOppDataAt(int pos, PlaneOpp *pPOpp);
	void InsertDataAt(int pos, double Alpha, double Beta, double QInf, double Ctrl, double Cl, double CY, double ICd, double PCd, double GCm,
						  double ICm, double VCm, double GRm, double GYm, double IYm, double VYm, double XCP, double YCP,
						  double ZCP, double Cb, double XNP);
	void CalculatePoint(int i);
	void Copy(WPolar *pWPolar);
	void DuplicateSpec(WPolar *pWPolar);
	void Export(QTextStream &out, enumTextFileType FileType, bool bDataOnly=false);
	void GetPolarProperties(QString &Properties, bool bData=false);
	void *GetWPlrVariable(int iVar);
	void Remove(int i);
	void Remove(double alpha);
	void ClearData();
	void RetrieveInertia(void *ptr, bool bPlane);
	bool SerializeWPlrWPA(QDataStream &ar, bool bIsStoring);
	bool SerializeWPlrXFL(QDataStream &ar, bool bIsStoring);

	enumAnalysisMethod analysisMethod() {return m_AnalysisMethod;} /**< returns the analysis method of the polar as an index in the enumeration. */
	double density()                    {return m_Density;}        /**< returns the fluid's density, in IS units. */
	bool pointsVisible()                {return m_bShowPoints;}    /**< returns true if the polar curve's points should be displayed in the graphs. */
	QString polarName()                 {return m_WPlrName;}       /**< returns the polar's name as a QString object. */
	double sideSlip()                   {return m_BetaSpec;}       /**< returns the sideslip angle, in degrees. */
	QString planeName()                 {return m_PlaneName;}      /**< returns the name of the polar's parent object as a QString object. */
	double viscosity()                  {return m_Viscosity;}      /**< returns the fluid's kinematic viscosity, in IS units. */
	bool visible()                      {return m_bIsVisible;}     /**< returns true if the polar curve should be displayed the graphs. */
	bool thinSurfaces()                 {return m_bThinSurfaces;}  /**< returns true if the analysis if using thin surfaces, i.e. VLM, false if 3D Panels for the Wing objects. */

	enumPolarType polarType()           {return m_WPolarType;}       /**< returns the type of the polar as an index in the enumeration. */
	bool isSpeedPolar() {return m_WPolarType==FIXEDSPEEDPOLAR;}      /**< returns true if the polar is of the FIXEDSPEEDPOLAR type, false otherwise >*/
	bool isFixedLiftPolar() {return m_WPolarType==FIXEDLIFTPOLAR;}   /**< returns true if the polar is of the FIXEDLIFTPOLAR type, false otherwise >*/
	bool isFixedaoaPolar() {return m_WPolarType==FIXEDAOAPOLAR;}     /**< returns true if the polar is of the FIXEDAOAPOLAR type, false otherwise >*/
	bool isStabilityPolar() {return m_WPolarType==STABILITYPOLAR;}   /**< returns true if the polar is of the STABILITYPOLAR type, false otherwise >*/
	bool isBetaPolar() {return m_WPolarType==BETAPOLAR;}             /**< returns true if the polar is of the BETAPOLAR type, false otherwise >*/

	void setPlaneName(QString PlaneName){m_PlaneName = PlaneName;}
	void setPolarName(QString PolarName){m_WPlrName = PolarName;}

	static QString variableName(int iVar);

public: //access methods
	bool bThinSurfaces() {return m_bThinSurfaces;}
	bool bWakeRollUp() {return m_bWakeRollUp;}
	bool bDirichlet() {return m_bDirichlet;}
	bool bTilted() {return m_bTiltedGeom; }
	bool bGround() {return m_bGround;}
	bool bIgnoreBodyPanels() {return m_bIgnoreBodyPanels;}
	bool bViscous() {return m_bViscous;}
	bool bVLM1() {return m_bVLM1;}

	double referenceArea()  {return m_referenceArea;}
	double referenceSpanLength()  {return m_referenceSpanLength;}
	double referenceChordLength() {return m_referenceChordLength;}


private:
	bool     m_bVLM1;              /**< true if the analysis is performed with horseshoe vortices, flase if quad rings */
	bool     m_bAutoInertia;       /**< true if the inertia to be taken into account is the one of the parent plane */
	bool     m_bDirichlet;         /**< true if Dirichlet boundary conditions should be applied, false if Neumann */
	bool     m_bGround;            /**< true if ground effect should be taken into account in the analysis */
	bool     m_bIgnoreBodyPanels;  /**< true if the body panels should be ignored in the analysis */
	bool     m_bIsVisible;         /**< true if the polar curve is visible in the graphs */
	bool     m_bShowPoints;        /**< true if the polar points are visible in the graphs */
	bool     m_bThinSurfaces;      /**< true if VLM, false if 3D-panels */
	bool     m_bTiltedGeom;        /**< true if the analysis should be performed on the tilted geometry */
	bool     m_bViscous;           /**< true if the analysis is viscous */
	bool     m_bWakeRollUp;        /**< true if wake roll-up  should be taken into account in the analysis */
	QColor   m_Color;              /**< the curve's color for the graphs */
	double   m_CoGIxx;             /**< The Ixx component of the inertia tensor, w.r.t. the CoG origin */
	double   m_CoGIxz;             /**< The Ixz component of the inertia tensor, w.r.t. the CoG origin */
	double   m_CoGIyy;             /**< The Iyy component of the inertia tensor, w.r.t. the CoG origin */
	double   m_CoGIzz;             /**< The Izz component of the inertia tensor, w.r.t. the CoG origin */
	QString  m_WPlrName;            /**< the polar's name */
	int      m_PolarFormat;        /**< the identification number which references the format used to serialize the data */
	enumRefDimension  m_ReferenceDim;        /**< Describes the origin of the refernce area : 1 if planform area, else projected area */
	int      m_Style;              /**< the index of the curve's style for the graphs */
	double   m_TotalWakeLength;    /**< the wake's length */
	QString  m_PlaneName;          /**< the name of the parent wing or plane */
	double   m_WakePanelFactor;    /**< the ratio between the length of two wake panels in the x direction */
	int      m_Width;              /**< the curve's width in pixels for the graphs */

	QList <double>  m_1Cl;        /**< 1/Cl, special for Matthieu */
	QList <double>  m_Alpha;      /**< the angle of attack */
	QList <double>  m_Beta;       /**< the sideslip angle */
	QList <double>  m_QInfinite;  /**< the free stream speed - type2 Wpolars */
	QList <double>  m_Cl32Cd;     /**< the power factor */
	QList <double>  m_ClCd;       /**< the glide ratio */
	QList <double>  m_CL;         /**< lift coef. */
	
	QList <double>  m_Ctrl;       /**< Ctrl variable */
	QList <double>  m_CY;         /**< Side Force */
	QList <double>  m_DutchRollDamping;        /**< the damping of the Dutch roll mode, as a result of stability analysis only */
	QList <double>  m_DutchRollFrequency;      /**< the frequency of the Dutch roll mode, as a result of stability analysis only */
	QList <double>  m_FX;         /**< the total drag */

	complex<double> m_EigenValue[8][MAXPOLARPOINTS]; /**< until we have a QList<complex<double>> ? */

	QList <double>  m_FY;         /**< the total side force */
	QList <double>  m_FZ;         /**< the total wing lift */
	QList <double>  m_Gamma;      /**< glide angle = Atan(Cx/Cz), in degrees */
	QList <double>  m_GCm;        /**< Total Pitching Moment coefficient */
	QList <double>  m_GRm;        /**< Total rolling moment */

	QList <double>  m_GYm;        /**< Total yawing moment coefficient */
	QList <double>  m_ICd;        /**< induced drag coef. */
	QList <double>  m_ICm;        /**< Induced Pitching Moment coefficient */
	QList <double>  m_IYm;        /**< induced yawing moment coefficient */
	QList <double>  m_Rm;         /**< the total rolling moment */
	QList <double>  m_Pm;         /**< the total pitching moment */
	QList <double>  m_MaxBending; /**< the max bending moment at the root chord */

	QList <double>  m_Oswald;     /**< Oswald's efficiency factor */
	QList <double>  m_PCd;        /**< profile drag coef. */

	QList <double>  m_PhugoidFrequency;        /**< the phugoid's frequency, as a result of stability analysis only */
	QList <double>  m_PhugoidDamping;          /**< the phugoid's damping factor, as a result of stability analysis only */
	QList <double>  m_RollDamping;             /**< the damping of the roll-damping mode, as a result of stability analysis only */
	QList <double>  m_ShortPeriodDamping;      /**< the damping of the short period mode, as a result of stability analysis only */
	QList <double>  m_ShortPeriodFrequency;    /**< the frequency of the short period mode, as a result of stability analysis only */

	QList <double>  m_SM;                      /**< pseudo Static Margin = (XCP-XCmRef)/m.a.c; */
	QList <double>  m_SpiralDamping;           /**< the damping of the spiral mode, as a result of stability analysis only */
	QList <double>  m_TCd;        /**< total drag coef. */
	QList <double>  m_VCm;        /**< Viscous Pitching Moment coefficient */
	QList <double>  m_VertPower;  /**< the power for steady horizontal flight = m.g.Vz */

	QList <double>  m_Vx;         /**< the horizontal component of the velocity */
	QList <double>  m_VYm;        /**< Profile yawing Moment coefficient */
	QList <double>  m_Vz;         /**< the sink speed = sqrt(2mg/rho/S)/powerfactor */
	QList <double>  m_XCP;        /**< the centre of pressure X-position relative to the wing's root LE */
	QList <double>  m_XNP;        /**< the position of the neutral point, as a result of stability analysis only */

	QList <double>  m_YCP;        /**< the centre of pressure Y-position relative to the wing's root LE */
	QList <double>  m_Ym;         /**< the total yawing moment */
	QList <double>  m_ZCP;        /**< the centre of pressure Z-position relative to the wing's root LE */

	double m_referenceArea;          /**< The reference area for the calculation of aero coefficients */
	double m_referenceChordLength;   /**< The reference length = the mean aero chord, for the calculation of aero coefficients */
	double m_referenceSpanLength;    /**< The reference span for the calculation of aero coefficients */


public:
	enumAnalysisMethod m_AnalysisMethod;  /**< The method used for the analysis. May be one of the following types : LLTMETHOD, VLMMETHOD, PANELMETHOD */
	enumPolarType      m_WPolarType;      /**< The type of analysis. May be one of the following types :FIXEDSPEEDPOLAR, FIXEDLIFTPOLAR, FIXEDAOAPOLAR, STABILITYPOLAR */
	CVector       m_CoG;                  /**< the position of the CoG */
	double        m_Density;              /**< The fluid's density */
	double        m_Mass;                 /**< The mass for type 2 and type 7 polars */

	int      m_nControls;          /**< the number of control surfaces for this wing or plane */
	int      m_NXWakePanels;       /**< the number of wake panels */
	double   m_AlphaSpec;          /**< the angle of attack for type 4 & 5 polars */
	double   m_BetaSpec;           /**< The sideslip angle for type 1,2, 4 polars */
	double   m_BankAngle;          /**< The bank angle */
	double   m_QInfSpec;           /**< the freestream velocity for type 1 & 5 polars */
	double   m_Height;             /**< The plane flight altitude, used if ground effect is to be taken into account*/
	double   m_Viscosity;          /**< The fluid's kinematic viscosity */
	QVarLengthArray<double> m_ControlGain;      /**< the scaling factor for each of the control surfaces */

};

#endif
