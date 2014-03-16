/****************************************************************************

    CPlane Class
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
 * This file implements the class for the Plane object.
 */



#ifndef PLANE_H
#define PLANE_H


/**
 *@class Plane
 *@brief
 * The class which defines the Plane object used in 3D calculations.
 *  - defines the plane object
 *  - provides the methods for the calculation of the plane's geometric properties
 * The data is stored in International Standard Units, i.e. meters, kg, and seconds
 * Angular data is stored in degrees
*/

#include "Wing.h"
#include "Body.h"


class Plane
{
	friend class PlaneDlg;

public:
	Plane();
	~Plane();

	double TotalMass();
	double TailVolume();

	void Duplicate(Plane *pPlane);
	bool SerializePlane(QDataStream &ar, bool bIsStoring);
	bool SerializePlaneXFL(QDataStream &ar, bool bIsStoring);
	void ComputePlane(void);
	void CreateSurfaces();
	void RenameWings();

	void ClearPointMasses();
	void ComputeVolumeInertia(double &Mass, CVector &CoG, double &Ixx, double &Iyy, double &Izz, double &Ixz);
	void ComputeBodyAxisInertia();

	void setAutoBodyName();
	void setPlaneName(QString planeName);

	void setWings(bool bWing2, bool bStab, bool bFin);
	void setBody(Body *pBody);


	int VLMPanelTotal();

	/**
	* Returns the translation to be applied to the Body object.
	* @return the translation to be applied to the Body object.
	*/
	CVector BodyPos(){ return m_BodyPos; }
	
	/**
	* Returns the leading edge, root position of a specified Wing.
	* @param iw the index of the Wing for which the LE position will be returned
	* @return the LE position of the Wing
	*/
	CVector WingLE(int iw){return m_WingLE[iw];}
	
	/**
	* Returns the tilt angle of a specified Wing.
	* @param iw the index of the Wing for which the tilt angle will be returned
	* @return the LE position of the Wing
	*/
	double WingTiltAngle(int iw){ return m_WingTiltAngle[iw];}
	
	/** Returns true if the plane has a secondary main wing, false otherwise.*/ 
	bool BiPlane(){return m_bBiplane;}
	
	/** Returns the Plane's name. */
	const QString& planeName() const {return m_PlaneName;}
	
	/** Returns a reference to the QString holding the Plane's name. */
	QString& rPlaneName() {return m_PlaneName;}

	/** Returns the Plane's description. */
	const QString& description() const {return m_PlaneDescription;}

	/** Returns a reference to the QString holding the Plane's description. */
	QString& rPlaneDescription() {return m_PlaneDescription;}
	

	/** Returns a pointer to the Plane's main wing. Never NULL, a Plane always has a main Wing. */
	Wing *wing()  {return m_Wing;}

	/** Returns a pointer to the Plane's secondary wing, or NULL if none. */
	Wing *wing2() {if(m_bBiplane) return m_Wing+1; else return NULL;}

	/** Returns a pointer to the Plane's elevator, or NULL if none. */
	Wing *stab()  {if(m_bStab) return m_Wing+2; else return NULL;}

	/** Returns a pointer to the Plane's fin, or NULL if none. */
	Wing *fin()   {if(m_bFin) return m_Wing+3; else return NULL;}

	/** Returns a pointer to the Plane's Body, or NULL if none. */
	Body *body()  {if(m_bBody)    return m_pBody; else return NULL;}
	
	/** Returns the Plane's CoG position */
	CVector CoG()  {return m_CoG;}

	double mac()           {return m_Wing[0].m_MAChord;}
	double span()          {return m_Wing[0].m_PlanformSpan;}
	double rootChord()     {return m_Wing[0].m_Chord[0];}
	double tipChord()      {return m_Wing[0].TipChord();}

	double projectedArea() {return m_Wing[0].m_ProjectedArea;}
	double planformArea()  {return m_Wing[0].m_PlanformArea;}

	double projectedSpan() {return m_Wing[0].m_ProjectedSpan;}
	double planformSpan()  {return m_Wing[0].m_PlanformSpan;}

	bool isWing() {return !m_bBiplane&& !m_bStab && !m_bFin && !m_bBody;}

	double aspectRatio()  {return m_Wing[0].m_AR;}
	double taperRatio()   {return m_Wing[0].m_TR;}

	Wing *getWing(int iw);

public:
	Wing m_Wing[MAXWINGS];                      /**< the array of Wing objects used to define this Plane */
	static void * s_pMainFrame;                 /**< a void static pointer to the MainFrame object */

private:

	Body *m_pBody;                              /**< a pointer to the Body object, a NULL if none */

	bool m_bBody;                               /**< true if a Body has been selected for this plane */
	bool m_bBiplane;                            /**< true if this Plane is a bi-plane */
	bool m_bFin;                                /**< true if this Plane has a fin*/
	bool m_bStab;                               /**< true if this Plane has an elevator */

	double m_VolumeMass;                        /**< the mass of the Plane's structure, excluding point masses */
	double m_TotalMass;                         /**< the Plane's total mass, i.e. the sum of the volume mass and of the point masses */

	QString m_PlaneName;                        /**< the Plane's name; this name is used to identify the object and as a reference for child WPolar and PlaneOpp objects. */
	QString m_PlaneDescription;                 /**< a free description */
	double m_TailVolume;                        /**< the tail volume, i.e lever_arm_elev x Area_Elev / MAC_wing / Area_wing */
	CVector m_CoG;                              /**< the position of the CoG */

	CVector m_WingLE[MAXWINGS];                 /**< the array of the leading edge postion of each Wing */
	double m_WingTiltAngle[MAXWINGS];           /**< the rotation in degrees of each Wing about the y-axis */
	CVector m_BodyPos;                          /**< the translation vector to apply to the Body */

public:
	QString m_BodyName;                         /**< identifies this plane's body */


	QList<PointMass*> m_PointMass;              /**< the array of PointMass objects */
	double m_CoGIxx;                            /**< the Ixx component of the inertia tensor, calculated at the CoG */
	double m_CoGIyy;                            /**< the Ixx component of the inertia tensor, calculated at the CoG */
	double m_CoGIzz;                            /**< the Ixx component of the inertia tensor, calculated at the CoG */
	double m_CoGIxz;                            /**< the Ixx component of the inertia tensor, calculated at the CoG */

	bool m_bDoubleFin;                          /**< true if the plane has a double fin, i.e. left and right */
	bool m_bSymFin;                             /**< true if the plane has a symetric fin, i.e. top and bottom */
	bool m_bDoubleSymFin;                       /**< true if the plane has a double and symetric fin */


};

#endif
