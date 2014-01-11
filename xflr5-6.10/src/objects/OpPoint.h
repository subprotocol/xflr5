/****************************************************************************

    OpPoint Class
	Copyright (C) 2003 Andre Deperrois adeperrois@xflr5.com

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


/**
 *@file
 *
 * This class implements the surface object on which the panels are constructed for the VLM and 3d-panel calculations.
 *
 */


#ifndef OPPOINT_H
#define OPPOINT_H




#include "../params.h"
#include <QString>
#include <QColor>
#include <QTextStream>
#include <QDataStream>

/**
*@class OpPoint
*@brief
 * The class which defines the operating point associated to Foil objects.

An OpPoint object stores the results of an XFoil calculation.
Each instance of an OpPoint is uniquely attached to a Polar object, which is itself attached uniquely to a Foil object.
The identification of the parent Polar and Foil are made using the QString names of the objects.
*/
class OpPoint
{
	friend class MainFrame;
	friend class Polar;
	friend class QXDirect;
	friend class ObjectPropsDlg;

public:
	OpPoint();

	static OpPoint *curOpp() {return s_pCurOpp;}
	static OpPoint *getOpp(double Alpha);
	static void setCurOpp(OpPoint* pOpp) {s_pCurOpp = pOpp;}
	static void deleteOpp(OpPoint *pOpp);
	static void insertOpPoint(OpPoint *pNewPoint);
	static OpPoint* addOpPoint(void *pFoilPtr, void *pPolarPtr, void *pXFoilPtr);

	void addXFoilData(void *pXFoilPtr);
	void setHingeMoments();



	void ExportOpp(QTextStream &out, QString Version, enumTextFileType FileType, bool bDataOnly=false);
	bool SerializeOppWPA(QDataStream &ar, bool bIsStoring, int ArchiveFormat=0);
	bool SerializeOppXFL(QDataStream &ar, bool bIsStoring, int ArchiveFormat=0);
	void GetOppProperties(QString &OpPointProperties, bool bData=false);

	QString foilName()  {return m_strFoilName;}
	QString polarName() {return m_strPlrName;}

	void setFoilName(QString newFoilName) {m_strFoilName = newFoilName;}


private:
	bool m_bViscResults;        /**< true if viscous results are stored in this OpPoint */
	bool m_bBL;                 /**< true if a boundary layer is stored in this OpPoint */
	bool m_bTEFlap;             /**< true if the parent foil has a flap on the trailing edge */
	bool m_bLEFlap;             /**< true if the parent foil has a flap on the leading edge */

	int n, nd1, nd2, nd3;
	double Reynolds;            /**< the Re number of the OpPoint */
	double Mach;                /**< the Mach number of the OpPoint */
	double Alpha;               /**< the aoa*/
	double Cl;                  /**< the lift coefficient */
	double Cm;                  /**< the pitching moment coefficient */
	double Cd;                  /**< the drag coefficient - viscous only, since we are dealing with 2D analysis */
	double Cdp;                 /**< @todo check significance in XFoil doc */
	double Xtr1;                /**< the laminar to turbulent transition point on the upper surface */
	double Xtr2;                /**< the laminar to turbulent transition point on the lower surface */
	double ACrit;               /**< the NCrit parameter which defines turbulent transition */
	double m_XCP;               /**< the x-position of the centre of pressure */
//	double x[IQX], y[IQX];      /**< the foil coordinates @todo check if still of use */

	double Cpv[IQX];            /**< the distribution of Cp on the surfaces for a viscous analysis */
	double Cpi[IQX];            /**< the distribution of Cp on the surfaces for an inviscid analysis */
	double Qv[IQX];             /**< the distribution of stream velocity on the surfaces for a viscous analysis */
	double Qi[IQX];             /**< the distribution of stream velocity on the surfaces for an inviscid analysis */
	double xd1[IQX];            /**< x-coordinate of the first part of the boundary layer */
	double yd1[IQX];            /**< y-coordinate of the first part of the boundary layer */
	double xd2[IWX];            /**< x-coordinate of the second part of the boundary layer */
	double yd2[IWX];            /**< y-coordinate of the second part of the boundary layer */
	double xd3[IWX];            /**< x-coordinate of the third part of the boundary layer */
	double yd3[IWX];            /**< y-coordinate of the third part of the boundary layer */
	double m_TEHMom;            /**< the moment on the foil's trailing edge flap */
	double m_LEHMom;            /**< the moment on the foil's leading edge flap */
	double XForce;              /**< the y-component of the pressure forces */
	double YForce;              /**< the y-component of the pressure forces */
	double Cpmn;                /**< @todo check significance in XFoil doc */


	bool m_bIsVisible;          /**< true if the OpPoint's curve is visible in the active view */
	bool m_bShowPoints;         /**< true if the OpPoint's curve points are visible in the active graphs */
	int m_Style;                /**< the index of the style with which to draw the OpPoint's curve */
	int m_Width;                /**< the width with which to draw the OpPoint's curve */
	QColor m_Color;             /**< the color with which to draw the OpPoint's curve */
	QString m_strFoilName;      /**< the name of the parent Foil */
	QString m_strPlrName;       /**< the name of the parent Polar */

	static OpPoint *s_pCurOpp;

public:
	static QList <void *> s_oaOpp;     /**< The array of void pointers to the foil operating point objects. */
	static bool s_bStoreOpp;          /**< true if operating points should be stored after an analysis */

};

#endif
