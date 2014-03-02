/****************************************************************************

	PlaneOpp Class
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



/** @file
 *
 * This class defines the operating point object for the 3D analysis of planes
 *
 */


#ifndef PLANEOPP_H
#define PLANEOPP_H


#include "WingOpp.h"
#include <QDataStream>

using namespace std;

/**
*@brief
*	This class defines the operating point object which stores the data of plane analysis
*
	Each instance of this class is uniquely associated to an instance of a WPolar, which is itself uniquely
	associated to a Wing or a Plane object.
	The results associated to each of the plane's wing is stored in WingOpp objects, declared as member variables.
	The data is stored in International Standard Units, i.e. meters, seconds, kg, and Newtons.
	Angular data is stored in degrees.
*/
class PlaneOpp
{
	friend class Objects3D;
	friend class QMiarex;
	friend class MainFrame;
	friend class WPolar;
	friend class PanelAnalysis;
	friend class LLTAnalysis;

public:
	PlaneOpp(void *pPlanePtr=NULL, void *pWPolarPtr=NULL, int PanelArraySize=0);
	~PlaneOpp();

	bool SerializePOppWPA(QDataStream &ar, bool bIsStoring);
	bool SerializePOppXFL(QDataStream &ar, bool bIsStoring);

	void AddWingOpp(int iw, int PanelArraySize);

	void Allocate(int PanelArraySize);
	void Release();

	void GetPlaneOppProperties(QString &PlaneOppProperties);

	QString title();
	double alpha() {return m_Alpha;}
	enumAnalysisMethod analysisMethod() {return m_AnalysisMethod;}
	QString planeName(){return m_PlaneName;}
	QString polarName(){return m_PlrName;}

	bool isOut(){return m_bOut;}

private:
	enumAnalysisMethod m_AnalysisMethod;   /**< defines by which type of method (LLT, VLM, PANEL), this WingOpp was calculated */

	QString m_PlaneName;       /**< the pPane's name to which the PlaneOpp is attached */
	QString m_PlrName;         /**< the WPolar's name to which the PlaneOpp is attached */
	
	double m_Alpha;            /**< the angle of attack*/
	double m_Beta;             /**< the sideslip angle */
	double m_Bank;             /**< the bank angle */
	double m_Ctrl;             /**< the value of the control variable */
	
	double m_Span;             /**< the parent's Wing span */
	double m_MAChord;          /**< the parent's Wing mean aerodynamic chord*/
	int m_NStation;            /**< unused */


//	bool m_bWing[MAXWINGS];    /**< true if respectively a main wing, 2nd wing, elevator, fin are part of the parent Plane object */
	bool m_bVLM1;              /**<  true if the PlaneOpp is the result of a horseshoe VLM analysis */
	bool m_bOut;               /**<  true if the interpolation of viscous properties was outside the Foil Polar mesh */

	bool m_bIsVisible;         /**< true if the PlaneOpp's curve is visible in the active view */
	bool m_bShowPoints;        /**< true if the PlaneOpp's curve points are visible in the active graphs */

	int m_Style;               /**< the index of the style with which to draw the PlaneOpp's curve */
	int m_Width;               /**< the width with which to draw the PlaneOpp's curve */
	QColor m_Color;            /**< the color with which to draw the PlaneOpp's curve */


public:
	enumPolarType m_WPolarType;		  /**< defines the type of the parent WPolar */
	WingOpp *m_pPlaneWOpp[MAXWINGS];  /**< An array of pointers to the four WingOpp objects associated to the four wings */
	double m_QInf;                    /**< the freestream velocity */
	double *m_dG;                       /**< the VLM vortex strengths, or the panel's doublet's strengths */
	double *m_dSigma;                   /**< the panel's source strengths */
	double *m_dCp;                      /**< the array of Cp coefficients */
	int m_NPanels;                    /**<  the number of VLM or 3D-panels */
	int m_nControls;	              /**< the number of control surfaces associated to the WingOpp */

	complex<double> m_EigenValue[8];      /**< the eigenvalues of the four longitudinal and four lateral modes */
	complex<double> m_EigenVector[8][4];  /**< the longitudinal and lateral eigenvectors (4 longitudinal + 4 lateral) x 4 components */

	//non dimensional stability derivatives
	double CXu, CZu, Cmu, CXa, CLa, Cma, CXq, CLq, Cmq, CYb, CYp, CYr, Clb, Clp, Clr, Cnb, Cnp, Cnr;
	//non-dimensional control derivatives
	double CXe,CYe,CZe;
	double CLe,CMe,CNe;

	double m_ALong[4][4];	/**< the longitudinal state matrix */
	double m_ALat[4][4];	/**< the lateral state matrix */
	double m_BLong[4];      /**< the longitudinal control vector */
	double m_BLat[4];       /**< the lateral control vector */

	double m_XNP;         /**< the neutral point position resulting from a stability calculations  */

	bool m_bThinSurface;        /**< true if the WingOpp is the results of a calculation on the middle surface */
	bool m_bTiltedGeom;         /**< true if the WingOpp is the results of a calculation on the tilted geometry */

	double m_Weight;

	double m_CL;          /**< the wing lift coefficient */
	double m_CX;          /**< the total drag coefficient */
	double m_CY;          /**< the side force coefficient */

	double m_VCD;         /**< the wing viscous drag coefficient */
	double m_ICD;         /**< the wing induced drag coefficient */

	double m_GCm;         /**< the wing pitching moment */
	double m_VCm;         /**< the pitching moment induced by the viscous drag forces */
	double m_ICm;         /**< the pitching moment induced by the pressure forces */

	double m_GRm;         /**< the wing rolling moment */

	double m_GYm;         /**< the total yawing moment */
	double m_VYm;         /**< the wing viscous yawing moment */
	double m_IYm;         /**< the wing induced yawing moment */

	CVector m_CP;         /**< the position of the centre of pressure */

	static bool s_bStoreOpps;       /**< true if the OpPoints should be added to the array at the end of the analysis*/
	static bool s_bKeepOutOpps;     /**< true if points with viscous propertiesinterpolated out of the polar mesh should be kept */

};
#endif
