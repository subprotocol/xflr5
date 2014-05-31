/****************************************************************************

	GLCreateLists 
	Copyright (C) 2010-2014 Andre Deperrois adeperrois@xflr5.com

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
  * @file This file implements the methods used to create the OpenGL lists for the various objects.
  */

#ifndef GLCREATELISTS_H
#define GLCREATELISTS_H

#include <QtOpenGL>
#include "../../objects/Plane.h"
#include "../../objects/PlaneOpp.h"
#include "../../objects/WPolar.h"

void GLCreateGeom(Wing *pWing, int List, Body *pBody=NULL);
void GLCreateCp(int nPanels, int nNodes, CVector *pNode, Panel *pPanel, PlaneOpp *pPOpp);
void GLCreateCpLegendClr(QRect cltRect);
void GLCreateDownwash(Wing *pWing, WPolar *pWPolar, WingOpp *pWOpp, int List);
void GLCreateDrag(Wing *pWing, WPolar* pWPolar, WingOpp *pWOpp, int List);
void GLCreateMesh(int iList, int size, Panel *pPanel, CVector *pNode, QColor PanelColor, QColor BackColor, bool bBack=true);
void GLCreateCtrlPts(int size, Panel *pPanel, double normalLength);
void GLCreateLiftStrip(Wing *pWing, WPolar *pWPolar, WingOpp *pWOpp, int List);
void GLCreateMoments(Wing *pWing, WPolar *pWPolar, PlaneOpp *pPOpp);
void GLCreateLiftForce(WPolar *pWPolar, PlaneOpp *pPOpp);
bool GLCreateStreamLines(Wing *PlaneWing[MAXWINGS], CVector *pNode, WPolar *pWPolar, PlaneOpp *pPOpp);
bool GLCreateSurfSpeeds(Panel *pPanel, WPolar *pWPolar, PlaneOpp *pPOpp);
void GLCreateVortices(int size, Panel *pPanel, CVector *pNode, WPolar *pWPolar);
void GLCreateTrans(Wing *pWing, WPolar *pWPolar, WingOpp *pWOpp, int List);
void GLCreatePanelForce(int size, Panel *pPanel, WPolar *pWPolar, PlaneOpp *pPOpp);

#endif // GLCREATELISTS_H
