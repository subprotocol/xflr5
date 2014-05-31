/****************************************************************************

    CSurface Class
	Copyright (C) 2005-2009 Andre Deperrois adeperrois@xflr5.com

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


/*! @file
 *
 * @brief This class defines the surface object on which the panels are constructed for the VLM and 3d-panel calculations.
 *
 */


#ifndef SURFACE_H
#define SURFACE_H

#include <QVarLengthArray>
#include "Foil.h"
#include "Body.h"
#include "CVector.h"
#include "WingSection.h"

#define VLMHALF 2500

/**
*@class Surface
*@brief
 * The class which defines the surface object on which the panels are constructed for the VLM and 3d-panel calculations.
 * The mesh panels are not constructed using the Wing object, but on this proxy Surface object.
 *
 A surface extends from one left Foil to the next right Foil in the spanwise directions.

 * The Surface's geometry is defined by
 *    - its Leading edge  : m_LA, m_LB
 *    - its Trailing edge : m_TA, m_TB
 *    - its left and right twist
 *    - its left and right Foil objects
 *    - its Normal vector
 *    - its left and right normal vectors NormalA and NormalB are
 *      the average of the normals of the two continuous surfaces ; used to define junction between panels
 *
 * The points and other geometric data may be requested by calling methods on any of the top, middle, or bottom surfaces.

 Surfaces are constructed and indexed from left tip to right tip.

 For a half-wing, there will be one surface less than the number of foils defined by the user.


 The letter A refers to the Surface's left side, and B refers to the right side.
 The letter L refers to the Leading upstream side, T refers to the Trailing downstream side

 A surface is described by its geometric properties inherited from the wing's definition.
 Surfaces with a span length less than a minimum value are ignored.

 A surface may only supports trailing edge flaps.
 A surface may not be flat if different washout angles have been defined at the tips. However the deviation from
 the flat panel is assumed to be small, and corresponding approximations are made.

 The process of Surface construction starts by the creation of the array of master points at each end Foil.
 These points are defined i.a.w. the number of panels and the type of distribution specified by the user.
 The panel points are interpolated between the left and right Foil points.
 This implies that a wing is built with a fixed number of chordwise panels all along the span.
 The panels on the surface are added incrementally to the global array of panels used for the panel analysis.

 The panels are numbered from left tip to right tip in the span wise direction. Then in the chordwise direction:
		from T.E. to L.E in the case of VLM
		from lower surface TE, to leading edge, and to upper surface TE


 The data is stored in International Standard Units, i.e. meters, seconds, kg, and Newtons.
 Angular data is stored in degrees.




*/
class Surface
{
	friend class Wing;
	friend class QMiarex;
	friend class Objects3D;
	friend class MainFrame;
	friend class GL3dWingDlg;
	friend class PanelAnalysis;
	friend class PanelAnalysisDlg;
	friend class InertiaDlg;
	friend class WPolar;

public:

	Surface();

	void AddFlapPanel(Panel *pPanel);
	void Copy(Surface *pSurface);
	void GetC4(int k, CVector &Pt, double &tau);
	void GetLeadingPt(int k, CVector &C);
	void GetNormal(double yrel, CVector &N);
	void GetTrailingPt(int k, CVector &C);
	void GetPanel(int const &k, int const &l, const enumPanelPosition &pos);
	void GetSurfacePoint(double const &xArel, double const &xBrel, double const &yrel, CVector &Point, int const &pos=0);
	void GetSurfacePointNormal(double const &xArel, double const &xBrel, double const &yrel, CVector &Point, CVector &PtNormal, int const &pos=0);
	void GetSection(double const &tau, double &Chord, double &Area, CVector &PtC4);
	void GetyDist(int const &k, double &y1, double &y2);
	void Init();
	void ResetFlap();
	void RotateX(CVector const &O, double XTilt);
	void RotateY(CVector const &O, double YTilt);
	void RotateZ(CVector const &O, double ZTilt);
	void SetNormal();
	void SetFlap();
	void SetSidePoints(Body *pBody, double dx, double dz);
	void SetTwist1();
	void SetTwist2();
	void Translate(CVector const &T);
	void Translate(double tx, double ty, double tz);
	void CreateXPoints();

	bool IsCenterSurf() {return m_bIsCenterSurf;}
	bool IsLeftSurf()   {return m_bIsLeftSurf;}
	bool IsRightSurf()  {return m_bIsRightSurf;}
	bool IsTipLeft()    {return m_bIsTipLeft;}
	bool IsTipRight()   {return m_bIsTipRight;}
	bool IsInSymPlane() {return m_bIsInSymPlane;}

	bool IsFlapPanel(int const &p);
	bool IsFlapNode(int const &nNode);
	bool RotateFlap(double const &Angle);
//	bool RotateFlap(double const &Angle, CPanel *pPanel, CVector *pNode);
	double GetTwist(int const &k);
	double GetChord(int const &k);
	double GetChord(double const &tau);
	double GetOffset(double const &tau);
	double GetStripSpanPos(int const &k);
	double GetFoilArea(double const &tau);
	double GetStripWidth(int const &k);


private :
	QList<CVector> SideA;      /**< the array of points on the left foil's mid-line*/
	QList<CVector> SideB;      /**< the array of points on the right foil's mid-line*/
	QList<CVector> SideA_T;    /**< the array of points on the left foil's top-line*/
	QList<CVector> SideB_T;    /**< the array of points on the right foil's top-line*/
	QList<CVector> SideA_B;    /**< the array of points on the left foil's bottom-line*/
	QList<CVector> SideB_B;    /**< the array of points on the right foil's bottom-line*/
	static CVector VTemp;
	static Panel *s_pPanel;    /**< a pointer to the array of this Surface's panels, This array is a sub-array of the total array.*/
	static CVector *s_pNode;   /**< a pointer to the array of this panel nodes.*/

	bool m_bIsInSymPlane;      /**< true if the Surface is positioned in the symetry xz plane defined by y=0. Case of a single fin. */
	bool m_bTEFlap;            /**< true if the Surface has a flap on the trailing edge */
	bool m_bIsLeftSurf;        /**< true if the Surface is built on the left wing */
	bool m_bIsRightSurf;       /**< true if the Surface is built on the right wing */
	bool m_bIsTipLeft;         /**< true if the Surface is built on the tip left wing */
	bool m_bIsTipRight;        /**< true if the Surface is built on the tip right wing */
	bool m_bIsCenterSurf;      /**< true if the Surface is either a left or right center surface... need to connect to body */

	CVector m_LA;              /**< the Surface's leading left point */
	CVector m_LB;              /**< the Surface's leading right point */
	CVector m_TA;              /**< the Surface's trailing left point */
	CVector m_TB;              /**< the Surface's trailing right point */
	CVector Normal;            /**< the Surface's normal vector */
	CVector NormalA;           /**< the normal at the left tip, defined as the average of this Surface's normal and of the one adjacent on the left side, if any */
	CVector NormalB;           /**< the normal at the right tip, defined as the average of this Surface's normal and of the one adjacent on the right side, if any */

	double m_TwistA;           /**< the twist at side A in degrees */
	double m_TwistB;           /**< the twist at side B in degrees */
	double m_Length;           /**< the Surface's planform length from A to B*/
	double chordA;             /**< the chord length at tip A */
	double chordB;             /**< the chord length at tip B */

	double m_posATE, m_posBTE;      /**< the relative flap hinge positions at sides A and B */
	QVarLengthArray<double> m_xPointA;        /**< the chordwise relative position of the VLM panel left corner points at side A */
	QVarLengthArray<double> m_xPointB;        /**< the chordwise relative position of the VLM panel right corner points at side B */

	double y1, y2, xLA, xTA, xLB, xTB;


	enumPanelDistribution m_XDistType;            /**< the type of distribution along the Surface's x axis @todo replace with an enumeration*/
	enumPanelDistribution m_YDistType;            /**< the type of distribution along the Surface's y axis */
	int m_NXLead;               /**< the number of panels upstream of the flap, i.e. between the leading edge and the hinge */
	int m_NXFlap;               /**< the number of panels on the flap, i.e. between the hinge and the trailing edge */
	int m_NElements;            /**< the number of panel elements constructer on this Surface. */

	int m_nFlapNodes;           /**< the number of nodes defined on the trailing edge flap */
	int m_nFlapPanels;          /**< the number of panels defined on the trailing edge flap */
	int m_FlapNode[VLMHALF];    /**< the array of flap node indexes, used to avoid defining two nodes at the same location */
	int m_FlapPanel[VLMHALF];   /**< the array of flap panel indexes */

	CVector m_HingePoint;       /**< a point on the trailing flap hinge */
	CVector m_HingeVector;      /**< a vector which defines the axis of the hinge */

	Surface *m_pLeftSurface;    /**< a pointer to this Surface's left neighbour, or NULL if none */
	Surface *m_pRightSurface;   /**< a pointer to this Surface's right neighbour, or NULL if none */


public:
	bool m_bJoinRight;             /**< true if the surface's right side should be connected to the next right surface's right left side - for panel analysis only */
	static CVector LA, LB, TA, TB; /**< leading and trailing corners of strip k */
	int m_NYPanels;                /**< the number of spanwise panels of this surface */
	int m_NXPanels;                /**< the number of chordwise panels of this surface */
	Foil *m_pFoilA;                /**< a pointer to the Surface's left Foil object */
	Foil *m_pFoilB;                /**< a pointer to the Surface's right Foil object */
	
};
#endif
