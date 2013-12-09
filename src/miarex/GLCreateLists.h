/****************************************************************************

	GLCreateLists 
	Copyright (C) 2010-2010 Andre Deperrois adeperrois@xflr5.com

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
  * @file this file implements the methods used to create the OpenGL lists for the various objects.
  */

#ifndef GLCREATELISTS_H
#define GLCREATELISTS_H

#include <QtOpenGL>
#include "../objects/Plane.h"
#include "../objects/PlaneOpp.h"
#include "../objects/WPolar.h"

void GLCreateGeom(void *pQMiarex, Wing *pWing, int List, Body *pBody=NULL);
void GLCreateCp(void *pQMiarex, CVector *pNode, Panel *pPanel, WingOpp *pWOpp, PlaneOpp *pPOpp);
void GLDrawCpLegend(void *pQMiarex);
void GLCreateCpLegendClr(void *pQMiarex);
void GLCreateDownwash(void *pQMiarex, Wing *pWing, WingOpp *pWOpp, int List);
void GLCreateDrag(void *pQMiarex, Wing *pWing, WPolar* pWPolar, WingOpp *pWOpp, int List);
void GLCreateMesh(int iList, int size, Panel *pPanel, CVector *pNode, QColor PanelColor, QColor BackColor, bool bBack=true);
void GLCreateCtrlPts(void *pQMiarex, Panel *pPanel);
void GLCreateLiftStrip(void *pQMiarex, Wing *pWing, WPolar *pWPolar, WingOpp *pWOpp, int List);
void GLCreateMoments(void *pQMiarex, Wing *pWing, WPolar *pWPolar, WingOpp *pWOpp);
void GLCreateLiftForce(void *pQMiarex, WPolar *pWPolar, WingOpp *pWOpp);
void GLCreateStreamLines(void *pQMiarex, Wing *PlaneWing[MAXWINGS], CVector *pNode, WPolar *pWPolar, WingOpp *pWOpp);
void GLCreateSurfSpeeds(void *pQMiarex, Panel *pPanel, WPolar *pWPolar, WingOpp *pWOpp);
void GLCreateVortices(void *pQMiarex, Panel *pPanel, CVector *pNode, WPolar *pWPolar);
void GLCreateTrans(void *pQMiarex, Wing *pWing, WingOpp *pWOpp, int List);
void GLCreatePanelForce(void *pQMiarex, WPolar *pWPolar, WingOpp *pWOpp, PlaneOpp *pPOpp);
void GLDrawPanelForceLegend(void *pQMiarex, WPolar *pWPolar);

#endif // GLCREATELISTS_H
