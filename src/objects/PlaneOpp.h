/****************************************************************************

    POpp Class
	Copyright (C) 2006 Andre Deperrois adeperrois@xflr5.com

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
	bool SerializePOpp(QDataStream &ar, bool bIsStoring, int ProjectFormat);

private:
	WingOpp m_PlaneWOpp[MAXWINGS];

	QString m_PlaneName;		// the plane's name to which the POpp belongs
	QString m_PlrName;		// the polar's name to which the POpp belongs
	
	double m_Alpha, m_QInf; 	// angle of attack and speed 
	double m_Beta;
	double m_Bank;
	double m_Ctrl;			//Control Variable
	
	int m_NStation;			// unused
	enumPolarType m_WPolarType;			// analysis type
	int m_VLMType;			// for future use

	bool m_bWing[MAXWINGS];  // main wing, 2nd wing, elevator, fin
	bool m_bVLM1;			// true if the POpp is the result of a classic VLM analysis
	bool m_bOut;			// true if the point is unconverged (interpolation outside the polar mesh)
	bool m_bIsVisible;
	bool m_bShowPoints;

	int m_Style;
	int m_Width;
	QColor m_Color;

public:
	float m_G[VLMMAXMATSIZE];		// the VLM vortex strengths, or the panel's doublet's strengths
	float m_Sigma[VLMMAXMATSIZE];		// the panel's source strengths
	float m_Cp[VLMMAXMATSIZE];	// the Cp array
	int m_NPanels;		// the number of VLM or 3D-panels
};
#endif
