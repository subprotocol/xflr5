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


#ifndef GLCREATELISTS_H
#define GLCREATELISTS_H

#include <QtOpenGL>
#include "../Objects/Plane.h"
#include "../Objects/POpp.h"
#include "../Objects/WPolar.h"

void GLCreateGeom(void *pQMiarex, CWing *pWing, int List, CBody *pBody=NULL);
void GLCreateCp(void *pQMiarex, CVector *pNode, CPanel *pPanel, CWOpp *pWOpp, CPOpp *pPOpp);
void GLDrawCpLegend(void *pQMiarex);
void GLCreateCpLegendClr(void *pQMiarex);
void GLCreateDownwash(void *pQMiarex, CWing *pWing, CWOpp *pWOpp, int List);
void GLCreateDrag(void *pQMiarex, CWing *pWing, CWPolar* pWPolar, CWOpp *pWOpp, int List);
void GLCreateMesh(void *pQMiarex, CVector *pNode, CPanel *pPanel);
void GLCreateCtrlPts(void *pQMiarex, CPanel *pPanel);
void GLCreateLiftStrip(void *pQMiarex, CWing *pWing, CWPolar *pWPolar, CWOpp *pWOpp, int List);
void GLCreateMoments(void *pQMiarex, CWing *pWing, CWPolar *pWPolar, CWOpp *pWOpp);
void GLCreateLiftForce(void *pQMiarex, CWPolar *pWPolar, CWOpp *pWOpp);
void GLCreateStreamLines(void *pQMiarex, CWing *pWing[MAXWINGS], CVector *pNode, CWPolar *pWPolar, CWOpp *pWOpp);
void GLCreateSurfSpeeds(void *pQMiarex, CPanel *pPanel, CWPolar *pWPolar, CWOpp *pWOpp);
void GLCreateVortices(void *pQMiarex, CPanel *pPanel, CVector *pNode, CWPolar *pWPolar);
void GLCreateTrans(void *pQMiarex, CWing *pWing, CWOpp *pWOpp, int List);
void GLDrawWingLegend(void *pQMiarex, CWing *pWing, CPlane *pPlane, CWPolar *pWPolar);
void GLDrawWOppLegend(void* pQMiarex, CWing *pWing, CWOpp *pWOpp);
void GLCreatePanelForce(void *pQMiarex, CWPolar *pWPolar, CWOpp *pWOpp, CPOpp *pPOpp);
void GLDrawPanelForceLegend(void *pQMiarex, CWPolar *pWPolar, CPanel *pPanel, CWOpp *pWOpp, CPOpp *pPOpp);

#endif // GLCREATELISTS_H
