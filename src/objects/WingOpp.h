/****************************************************************************

	WingOpp Class
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



/** @file
 *
 * This class defines the operating point object for the 3D analysis of wings.
 *
 */


#ifndef WINGOPP_H
#define WINGOPP_H


/**
*@brief
*	This class implements the operating point object which stores the data of plane analysis
*
	In the case of an analysis of an independant wing, the instance of this WingOpp class is
	uniquely associated to an instance of a WPolar, which is itself uniquely associated to the Wing object.
	Alternatively, the WingOpp may be a member variable of a PlaneOpp object.
	The data is stored in International Standard Units, i.e. meters, seconds, kg, and Newtons.
	Angular data is stored in degrees.
*/


#include "CVector.h"
#include "../params.h"
#include <QString>
#include <QColor>
#include <QTextStream>
#include <complex>

using namespace std;

class WingOpp
{
	friend class QMiarex;
	friend class MainFrame;
	friend class WPolar;
	friend class PlaneOpp;
	friend class StabViewDlg;
	friend class PanelAnalysisDlg;
	friend class ObjectPropsDlg;

public:
	WingOpp(int PanelArraySize=0);
	~WingOpp();

private:
//________________METHODS____________________________________
	bool SerializeWingOpp(QDataStream &ar, bool bIsStoring);
	bool Export(QTextStream &out, enumTextFileType FileType);
	void GetWingOppProperties(QString &WingOppProperties);
	double GetMaxLift();

	void Allocate(int PanelArraySize);
    void ReleasePanelSizeArrays();

private:
	QString m_WingName;	// the wing name to which the WingOpp belongs
	QString m_PlrName; 	// the polar name to which the WingOpp belongs

	bool m_bIsVisible;          /**< true if the WingOpp's curve is visible in the active view */
	bool m_bShowPoints;         /**< true if the WingOpp's curve points are visible in the active graphs */

	bool m_bVLM1;               /**< true if the WingOpp is the result of a horseshoe VLM calculation, flase if quad ring */
	bool m_bThinSurface;        /**< true if the WingOpp is the results of a calculation on the middle surface */
	bool m_bTiltedGeom;         /**< true if the WingOpp is the results of a calculation on the tilted geometry */

	int m_Style;                /**< the index of the style with which to draw the WingOpp's curve */
	int m_Width;                /**< the width with which to draw the WingOpp's curve */
	QColor m_Color;             /**< the color with which to draw the WingOpp's curve */

//	int m_WingType;		//0 for wing, 1 for elevator, 2 for fin
	int m_nWakeNodes;	        /**< the number of wake nodes */
	int m_NXWakePanels;	        /**< the number of wake panels */
	int m_nControls;	        /**< the number of control surfaces associated to the WingOpp */

	double m_FirstWakePanel;    /**< the size of the first wake panel in the array. */
	double m_WakeFactor;        /**< the geometric progression factor of the panel lengths in a column of wake panels */
	double m_Weight;
	float m_Twist[MAXSPANSTATIONS+1];		/**< the twist at span stations */

	//RESULTS
	float m_Re[MAXSPANSTATIONS+1];              /**< the calculated Reynolds number at span stations */
	float m_Cm[MAXSPANSTATIONS+1];              /**< the otal pitching moment coefficient at span stations */
	float m_CmAirf[MAXSPANSTATIONS+1];          /**< the airfoil pitching moment coefficient about 1/4 chord point, at span stations */
	float m_BendingMoment[MAXSPANSTATIONS+1];   /**< the bending moment at span stations */

	double m_CX;			/**< the total drag coefficient */
	double m_CY;            /**< the side force coefficient */
	float m_MaxBending;		/**< the bending moment at the root chord */


	//non dimensional stability derivatives
	double CXu, CZu, Cmu, CXa, CLa, Cma, CXq, CLq, Cmq, CYb, CYp, CYr, Clb, Clp, Clr, Cnb, Cnp, Cnr;
	//non-dimensional control derivatives
	double CXe,CYe,CZe;
	double CLe,CMe,CNe;
	
	double m_ALong[4][4];	/**< the longitudinal state matrix */
	double m_ALat[4][4];	/**< the lateral state matrix */
	double m_BLong[4];      /**< the longitudinal control vector */
	double m_BLat[4];       /**< the lateral control vector */
	

public:
	bool m_bOut;		/**< true if there was an interpolation error of the viscous properties for this WingOpp */

	int m_NVLMPanels;	/**< the number of panels */
	int m_NStation;		/**< the number of stations along the span */
	int m_nFlaps;		/**< the number of trailing edge flaps */

	enumAnalysisMethod m_AnalysisMethod;   /**< defines by whih type of method (LLT, VLM, PANEL), this WingOpp was calculated */
	enumPolarType m_WPolarType;		       /**< defines the type of the parent WPolar */

	float *m_Cp;                           /**< a pointer to the array of lift coefficient for each panel */
	float *m_G;                            /**< a pointer to the array of vortice or doublet strengths */
	float *m_Sigma;                        /**< a pointer to the array of source strengths */


	double m_Span;                         /**< the parent's Wing span */
	double m_MAChord;                      /**< the parent's Wing mean aerodynamic chord*/
	double m_QInf;                         /**< the freestream velocity */
	double m_Alpha;                        /**< the aoa */
	double m_Beta;                         /**< the sideslip angle */
	double m_Phi;                          /**< the bank angle */
	float m_SpanPos[MAXSPANSTATIONS+1];    /**< the spanwise position of the stations */
	float m_Ai[MAXSPANSTATIONS+1];         /**< the calculated induced angles, in degrees */
	float m_Chord[MAXSPANSTATIONS+1];      /**< the chord lengths at stations */
	float m_ICd[MAXSPANSTATIONS+1];        /**< the induced drag coefficient at stations */
	float m_PCd[MAXSPANSTATIONS+1];        /**< the viscous drag coefficient at stations */
	float m_Cl[MAXSPANSTATIONS+1];         /**< the lift coefficient at stations */
	float m_XCPSpanRel[MAXSPANSTATIONS+1]; /**< the relative position of the centre of pressure on each chordwise strip, in chord % */
	float m_XCPSpanAbs[MAXSPANSTATIONS+1]; /**< the absolute position of the centre of pressure on each chordwise strip */
	float m_StripArea[MAXSPANSTATIONS+1];  /**< the area of the chordwise strips */
	float m_XTrTop[MAXSPANSTATIONS+1];     /**< the transition location on the top surface*/
	float m_XTrBot[MAXSPANSTATIONS+1];     /**< the transition location on the bottom suface */
	CVector m_Vd[MAXSPANSTATIONS];         /**< the downwash at the trailing edge */
	CVector m_F[MAXSPANSTATIONS];          /**< the force acting on the chordwise = sum of the panel forces on this strip*/
	double m_FlapMoment[MAXSPANSECTIONS];  /**< the flap hinge moments */

	double m_CL;          /**< the wing lift coefficient */
	double m_VCD;         /**< the wing viscous drag coefficient */
	double m_ICD;         /**< the wing induced drag coefficient */
	double m_GCm;         /**< the wing pitching moment */
	double m_GRm;         /**< the wing rolling moment */
	double m_VCm;         /**< the pitching moment induced by the viscous drag forces */
	double m_ICm;         /**< the pitching moment induced by the pressure forces */
	double m_GYm;         /**< the total yawing moment */
	double m_VYm;         /**< the wing viscous yawing moment */
	double m_IYm;         /**< the wing induced yawing moment */
	double m_Ctrl;        /**< the control variable */
	double m_XNP;         /**< the neutral point position resulting from a stability calculations  */

	CVector m_CP;         /**< the position of the centre of pressure */

	complex<double> m_EigenValue[8];      /**< the eigenvalues of the four longitudinal and four lateral modes */
	complex<double> m_EigenVector[8][4];  /**< the mongitudinal and lateral eigenvectors (4 longitudinal + 4 lateral) x 4 components */
};
#endif
