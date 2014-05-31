/****************************************************************************

    Spline Foil Class
	Copyright (C) 2003-2010 Andre Deperrois adeperrois@xflr5.com

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
 *@file This class defines the splined foil object used in foil design.
 */


#ifndef SPLINEFOIL_H
#define SPLINEFOIL_H


#include "Spline.h"


/**
*@class SplineFoil
*@brief  The class which defines the splined foil object.

The foil is contructed based on one spline for the upper surface and one spline for the lower surface.
@todo This class mixes the construction methods and the GUI; would be better to move the GUI to a derived child class for polymorphism.
*/
class SplineFoil
{
	friend class QAFoil;
	friend class MainFrame;
	friend class SplineCtrlsDlg;
	friend class FoilTableDelegate;


public:
	SplineFoil();
	SplineFoil(SplineFoil *pSF);

private:
	void CompMidLine();

	void InitSplineFoil();

	bool Serialize(QDataStream &ar, bool bIsStoring);
	bool SerializeXFL(QDataStream &ar, bool bIsStoring);

	void Copy(SplineFoil* pSF);
	void DrawCtrlPoints(QPainter &painter, double scalex, double scaley, QPoint Offset);
	void DrawMidLine(QPainter &painter, double scalex, double scaley, QPoint Offset);
	void DrawFoil(QPainter &painter, double scalex, double scaley, QPoint Offset);
	void DrawOutPoints(QPainter &painter, double scalex, double scaley, QPoint Offset);
	void ExportToBuffer(void *pFoilPtr);
	void ExportToFile(QTextStream &out);
	void UpdateSplineFoil();
	void SetCurveParams(int style, int width, QColor color);

	bool m_bModified;                /**< false if the SplineFoil has been serialized in its current dtate, false otherwise */
	bool m_bVisible;                 /**< true if this SplineFoil object is visible */
	bool m_bOutPoints;               /**< true if the ouput line points should be displayed */
	bool m_bCenterLine;              /**< true if the SplineFoil's mean camber line is to be displayed */
	bool m_bSymetric;                /**< true if the SplineFoil is symetric. In which case the lower surface is set as symetric of the upper surface. */

	int m_OutPoints;                 /**< the number of output points with which to draw the SplineFoil. */

	int m_FoilStyle;                 /**< the index of the style with which to draw the SplineFoil */
	int m_FoilWidth;                 /**< the width with which to draw the SplineFoil */
	QColor m_FoilColor;              /**< the color with which to draw the SplineFoil */

	double m_fCamber;                /**< the SplineFoil's max camber */
	double m_fThickness;             /**< the SplineFoil's max thickness */
	double m_fxCambMax;              /**< the x-position of the SplineFoil's max camber point */
	double m_fxThickMax;             /**< the x-position of the SplineFoil's max thickness point */
	QString m_strFoilName;           /**< the SplineFoil's name */
	Spline m_Extrados;               /**< the spline which defines the upper surface */
	Spline m_Intrados;               /**< the spline which defines the lower surface */
	CVector m_rpMid[MIDPOINTCOUNT];  /**< the points on the SplineFoil's mid camber line @todo replace with a QVarLengthArray */
};
#endif
