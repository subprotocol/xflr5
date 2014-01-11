/****************************************************************************

	Body Class
	Copyright (C) 2007-2014 Andre Deperrois adeperrois@xflr5.com

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
 * This file implements the definition of the Body class.
 */



#ifndef BODY_H
#define BODY_H

#include "Panel.h"
#include "NURBSSurface.h"
#include "PointMass.h"
#include <QTextStream>
#include <QVarLengthArray>
#include <QColor>

typedef enum  {BODYPANELTYPE, BODYSPLINETYPE }     enumBodyLineType;     /**< Label for a spline type body */

/**
 * This class :
 *	 - defines the body object,
 * 	 - provides the methods for the calculation of the plane's geometric properties,
 *   - porvides methods for the panel calculations.
 * The data is stored in International Standard Units, i.e. meters, kg, and seconds.
 * Angular data is stored in degrees.
 * @todo see if an abstract Body class can be defined, and subclass the NURBS and Panel body classes.
 */
class Body
{
public:
	Body();

	bool IsInNURBSBody(CVector Pt);
	bool Intersect(CVector A, CVector B, CVector &I, bool bRight);
	bool IntersectPanels(CVector A, CVector B, CVector &I);
	bool IntersectNURBS(CVector A, CVector B, CVector &I, bool bRight);
	bool SerializeBodyWPA(QDataStream &ar, bool bIsStoring);
	bool SerializeBodyXFL(QDataStream &ar, bool bIsStoring);
	bool ImportDefinition(QTextStream &inStream, double mtoUnit);
	bool ExportBodyDefinition(QTextStream &outStream, double mtoUnit);

	int InsertFrame(CVector Real);
	int InsertPoint(CVector Real);
	int IsFramePos(CVector Real, double ZoomFactor);
	int RemoveFrame(int n);
	int ReadFrame(QTextStream &in, int &Line, Frame *pFrame, double const &Unit);

	double Length();

	double Getu(double x);
	double Getv(double u, CVector r, bool bRight);
	double GetSectionArcLength(double x);

	CVector LeadingPoint();

	void ClearPointMasses();
    void ComputeAero(double *Cp, double &XCP, double &YCP, double &ZCP,
					 double &GCm, double &GRm, double &GYm, double &Alpha, CVector &CoG);
	void Duplicate(Body *pBody);
	void ExportGeometry(QTextStream &outStream, int type, double mtoUnit, int nx, int nh);
	void GetPoint(double u, double v, bool bRight, CVector &Pt);
	void InsertSideLine(int SideLine);
	void RemoveActiveFrame();
	void RemoveSideLine(int SideLine);
	void Scale(double XFactor, double YFactor, double ZFactor, bool bFrameOnly=false, int FrameID=0);
	void Translate(double XTrans, double, double ZTrans, bool bFrameOnly=false, int FrameID=0);
	void Translate(CVector T, bool bFrameOnly=false, int FrameID=0);
	void SetKnots();
	void SetPanelPos();
	void SetEdgeWeight(double uw, double vw);

	Frame *getFrame(int iFrame);
	Frame *activeFrame();
	void SetActiveFrame(Frame *pFrame);
	double FramePosition(int iFrame);
	int FrameSize()       {return m_SplineSurface.FrameSize();}
	int FramePointCount() {return m_SplineSurface.FramePointCount();}
	int SideLineCount()   {return m_SplineSurface.FramePointCount();}// same as FramePointCount();

	void ComputeBodyAxisInertia();
	void ComputeVolumeInertia(CVector &CoG, double &CoGIxx, double &CoGIyy, double &CoGIzz, double &CoGIxz);
	double TotalMass();


	//____________________VARIABLES_____________________________________________
	static void* s_pMainFrame;                /**< a void pointer to the application's MainFrame window */

	QString m_BodyName;                       /**< the Body's name, used as its reference */
	QString m_BodyDescription;                /**< a free description for the Body */


	NURBSSurface m_SplineSurface;             /**< the spline surface which defines the left (port) side of the body */

	enumBodyLineType m_LineType;              /**< the type of body definition 1=lines  2=B-Splines */

	int m_iActiveFrame;		                  /**< the currently selected frame for display */
	int m_iHighlight;                         /**< the currently selected point to highlight */
	int m_iRes;                               /**< the number of output points in one direction of the NURBS surface */
	int m_NElements;                          /**< the number of mesh elements for this Body object = m_nxPanels * m_nhPanels *2 */
	int m_nxPanels;                           /**< the number of mesh elements in the direction of the x-axis */
	int m_nhPanels;                           /**< the number of mesh elements in the hoop direction */

	int m_BodyStyle;                          /**< the index of the spline's style */
	int m_BodyWidth;                          /**< the width of the spline */
	QColor m_BodyColor;                       /**< the Body's display color */

	double m_Bunch;                            /**< a bunch parameter to set the density of the points of the NURBS surface; unused */

	double m_VolumeMass;                       /**< the mass of the Body's structure, excluding point masses */
	double m_TotalMass;                        /**< the wing's total mass, i.e. the sum of the volume mass and of the point masses */
	QList<PointMass*> m_PointMass;             /**< the array of PointMass objects */

	double m_CoGIxx;                           /**< the Ixx component of the inertia tensor, calculated at the CoG */
	double m_CoGIyy;                           /**< the Ixx component of the inertia tensor, calculated at the CoG */
	double m_CoGIzz;                           /**< the Ixx component of the inertia tensor, calculated at the CoG */
	double m_CoGIxz;                           /**< the Ixx component of the inertia tensor, calculated at the CoG */
	CVector m_CoG;                             /**< the position of the CoG */


	QVarLengthArray<int> m_xPanels;              /**< the number of mesh panels between two frames */
	QVarLengthArray<int> m_hPanels;              /**< the number of mesh panels in the hoop direction, on one side of the Body */


	Panel *m_pBodyPanel;                       /** A pointer to the first body panel in the array */

	//allocate temporary variables to
	//avoid lengthy memory allocation times on the stack
	double value, eps, bs, cs;
	CVector t_R, t_Prod, t_Q, t_r, t_N;
//	CVector P0, P1, P2, PI;
	static double s_XPanelPos[300];

};
#endif

