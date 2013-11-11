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
	friend class QMiarex;
	friend class MainFrame;
	friend class WPolar;

public:
	PlaneOpp();
	bool SerializePOpp(QDataStream &ar, bool bIsStoring);

private:
	WingOpp m_PlaneWOpp[MAXWINGS];  /**< the four WingOpp objects associated to the four wings */
	enumPolarType m_WPolarType;     /**< the type of the parent WPolar object */

	QString m_PlaneName;       /**< the plane's name to which the PlaneOpp is attached */
	QString m_PlrName;         /**< the WPolar's name to which the PlaneOpp is attached */
	
	double m_Alpha;            /**< the angle of attack*/
	double m_QInf;             /**< the freestream velocity */
	double m_Beta;             /**< the sideslip angle */
	double m_Bank;             /**< the bank angle */
	double m_Ctrl;             /**< the value of the control Variable */
	
	int m_NStation;            /**< unused */
//	int m_VLMType;             /**< unused */


	bool m_bWing[MAXWINGS];    /**< true if respectively a main wing, 2nd wing, elevator, fin are part of the parent Plane object */
	bool m_bVLM1;              /**<  true if the PlaneOpp is the result of a horseshoe VLM analysis */
	bool m_bOut;               /**<  true if the interpolation of viscous properties was outside the Foil Polar mesh */

	bool m_bIsVisible;         /**< true if the PlaneOpp's curve is visible in the active view */
	bool m_bShowPoints;        /**< true if the PlaneOpp's curve points are visible in the active graphs */

	int m_Style;               /**< the index of the style with which to draw the PlaneOpp's curve */
	int m_Width;               /**< the width with which to draw the PlaneOpp's curve */
	QColor m_Color;            /**< the color with which to draw the PlaneOpp's curve */


public:
	float m_G[VLMMAXMATSIZE];        /**< the VLM vortex strengths, or the panel's doublet's strengths */
	float m_Sigma[VLMMAXMATSIZE];    /**< the panel's source strengths */
	float m_Cp[VLMMAXMATSIZE];       /**< the array of Cp coefficients */
	int m_NPanels;                   /**<  the number of VLM or 3D-panels */
};
#endif
