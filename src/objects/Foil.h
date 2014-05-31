/****************************************************************************

    Reference Foil Class
	Copyright (C) 2003-2014 Andre Deperrois adeperrois@xflr5.com

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
 * This class defines the foil object used in 2d and 3d calculations
 *
 */



#ifndef FOIL_H
#define FOIL_H

#include <QColor>
#include <QPainter>
#include <QPoint>
#include "../params.h"
#include "CVector.h"
#include "CRectangle.h"
#include <QTextStream>



/**
*@class Foil
*@brief
*	The class which defines the Foil object used in 2D and 3D calculations.

The class stores two geometries:
	- the base foil, which unless advised otherwise is unchanged from the moment it has been first loaded or created
	- the current foil, on which the geometrical modifications are applied
	   such as flap deflection, camber and thickness scaling
@todo One of the very early classes in this project. Would need a general revision.
Also it mixes the construction methods and the GUI; would be better to move the GUI to a derived child class for polymorphism.
*/
class Foil
{
	friend class QAFoil;
	friend class SplineFoil;
	friend class Surface;
	friend class ThreeDWidget;
	friend class QXDirect;
	friend class QXInverse;
	friend class QMiarex;
	friend class XFoil;
	friend class OpPoint;
	friend class FoilPolarDlg;
	friend class Wing;
	friend class MainFrame;
	friend class NacaFoilDlg;
	friend class FoilCoordDlg;
	friend class TwoDPanelDlg;
	friend class InverseOptionsDlg;
	friend class InterpolateFoilsDlg;
	friend class FoilGeomDlg;
	friend class TEGapDlg;
	friend class LEDlg;
	friend class FlapDlg;
	friend class CAddDlg;
	friend class BatchDlg;
	friend class BatchThreadDlg;
	friend class XFoilTask;
	friend class GL3dWingDlg;
	friend class WingDlg;
	friend class FoilSelectionDlg;
	friend class ManageFoilsDlg;
	friend class FoilTableDelegate;
	
public:
	Foil();

	QString foilName() {return m_FoilName;}
	void setFoilName(QString FoilName){m_FoilName = FoilName;}
	void renameThisFoil(QString newFoilName);

	int IsPoint(CVector const &Real);
	void DrawFoil(QPainter &painter, double const &alpha, double const &scalex, double const &scaley, QPoint const &Offset);
	void DrawPoints(QPainter &painter, double const &scalex, double const &scaley, QPoint const &Offset);
	void DrawMidLine(QPainter &painter, double const &scalex, double const &scaley, QPoint const &Offset);

	void GetLowerY(double x, double &y, double &normx, double &normy);
	void GetUpperY(double x, double &y, double &normx, double &normy);

	double DeRotate();
	double GetBaseUpperY(double x);
	double GetBaseLowerY(double x);
	double GetMidY(double xl);
	double GetLowerY(double x);
	double GetUpperY(double x);
	double GetCamber(double const &x);
	double GetCamberSlope(double const &x);
	double length();
	double GetArea();
	double GetTopSlope(double const &x);
	double GetBotSlope(double const &x);
	double NormalizeGeometry();
	void CompMidLine(bool bParams);

	bool ExportFoil(QTextStream &out);
	bool InitFoil();

	void CopyFoil(Foil *pSrcFoil);
	bool Serialize(QDataStream &ar, bool bIsStoring);
	bool SerializeFoilXFL(QDataStream &ar, bool bIsStoring);
	void SetFlap();
	void SetTEFlap();
	void SetLEFlap();
	void SetNaca009();
	void SetLEFlapData(bool bFlap, double xhinge, double yhinge, double angle);
	void SetTEFlapData(bool bFlap, double xhinge, double yhinge, double angle);

	bool Intersect(CVector const &A, CVector const &B, CVector const &C, CVector const &D, CVector *M);

	static Foil *foil(QString strFoilName);
	static Foil *deleteFoil(Foil *pFoil);
	void insertThisFoil();
	Foil * deleteThisFoil();

private:
	QString m_FoilDescription;	        /**< a free description */

	bool m_bVisible;                     /**< true if the foil is to be displayed */
	bool m_bCenterLine;                  /**< true if the foil mid camber line is to be displayed */
	bool m_bPoints;                      /**< true if the foil's panels are to be displayed */
	bool m_bSaved;                       /**< true if the design modifications have been saved */

	int m_iBaseInt;                      /**< the number of points on the lower surface of the base foil */
	int m_iBaseExt;                      /**< the number of points on the upper surface of the base foil */

	int m_iInt;                          /**< the number of points on the lower surface of the current foil */
	int m_iExt;                          /**< the number of points on the upper surface of the current foil */

	int m_FoilStyle;                     /**< the index of the style with which to draw the Foil */
	int m_FoilWidth;                     /**< the width with which to draw the Foil */
	QColor m_FoilColor;                  /**< the color with which to draw the Foil */

	int m_iHighLight;                    /**< the index of the point to highlight in the display */

	int n;                               /**<  the number of points of the current foil */
	double x[IBX];                       /**< the array of x-coordinates of the current foil points */
	double y[IBX];                       /**< the array of y-coordinates of the current foil points*/
	double nx[IBX];                      /**< the array of x-coordinates of the current foil normal vectors*/
	double ny[IBX];                      /**< the array of x-coordinates of the current foil normal vectors*/

	 // Base geometry;
	int nb;                              /**< the number of points of the base foil */
	double xb[IBX];                      /**< the array of x-coordinates of the base foil points */
	double yb[IBX];                      /**< the array of y-coordinates of the base foil points*/


	double m_fCamber;                    /**< the Foil's max camber */
	double m_fThickness;                 /**< the Foil's max thickness */
	double m_fXCamber;                   /**< the x-position of the Foil's max camber point */
	double m_fXThickness;                /**< the x-position of the Foil's max thickness point */

	double m_Gap;			            /**< the trailing edge gap */
	CVector m_TE;                        /**< the trailing edge point */
	CVector m_LE;                        /**< the leading edge point */

	CVector m_rpBaseMid[MIDPOINTCOUNT+1];          /**< the mid camber line points of the base geometry */
	CVector m_BaseExtrados[IQX];	       /**< the upper surface points of the base geometry */
	CVector m_BaseIntrados[IQX];    	  /**< the lower surface points of the base geometry */

	CVector m_rpMid[MIDPOINTCOUNT+1];              /**< the mid camber line points */
	CVector m_rpExtrados[IQX];	       /**< the upper surface points */
	CVector m_rpIntrados[IQX];	       /**< the lower surface points */

	QString m_FoilName;		            /**<  the foil's name... */

	static QList <void *> s_oaFoil;      /**< The array of void pointers to the Foil objects. */

public:

	bool m_bTEFlap;          /**< true if the foil has a trailing edge flap */
	double m_TEFlapAngle;    /**< the trailing edge flap angle, in degrees*/
	double m_TEXHinge;       /**< the x-position of the trailing edge flap, in chord % */
	double m_TEYHinge;       /**< the y-position of the trailng edge flap, in chord %*/

	bool m_bLEFlap;          /**< true if the foil has a leading edge flap */
	double m_LEFlapAngle;    /**< the leading edge flap angle, in degrees */
	double m_LEXHinge;       /**< the x-position of the leading edge flap, in chord % */
	double m_LEYHinge;       /**< the y-position of the leading edge flap, in chord %*/

	static Foil *s_pCurFoil;
	static Foil *curFoil(){return s_pCurFoil;}
	static void setCurFoil(Foil*pFoil){s_pCurFoil = pFoil;}
};

#endif
