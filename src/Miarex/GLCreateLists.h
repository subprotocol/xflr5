/****************************************************************************

	GLCreateLists 
	Copyright (C) 2010-2010 Andre Deperrois xflr5@yahoo.com

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
#include "../Objects/Wing.h"
#include "../Objects/POpp.h"
#include "../Objects/WPolar.h"

void GLCreateGeom(void *pQMiarex, CWing *pWing, int List);
void GLCreateCp(void *pQMiarex, CVector *pNode, CPanel *pPanel, CWOpp *pWOpp, CPOpp *pPOpp);
void GLCreateCpLegend(void *pQMiarex);
void GLCreateDownwash(void *pQMiarex, CWing *pWing, CWOpp *pWOpp, int List);
void GLCreateDrag(void *pQMiarex, CWing *pWing, CWPolar* pWPolar, CWOpp *pWOpp, int List);
void GLCreateMesh(void *pQMiarex, CVector *pNode, CPanel *pPanel);
void GLCreateCtrlPts(void *pQMiarex, CPanel *pPanel);
void GLCreateLiftStrip(void *pQMiarex, CWing *pWing, CWPolar *pWPolar, CWOpp *pWOpp, int List);
void GLCreateMoments(void *pQMiarex, CWing *pWing, CWPolar *pWPolar, CWOpp *pWOpp);
void GLCreateLiftForce(void *pQMiarex, CWPolar *pWPolar, CWOpp *pWOpp);
void GLCreateStreamLines(void *pQMiarex, CWing *Wing[4], CVector *pNode, CWPolar *pWPolar, CWOpp *pWOpp);
void GLCreateSurfSpeeds(void *pQMiarex, CPanel *pPanel, CWPolar *pWPolar, CWOpp *pWOpp);
void GLCreateVortices(void *pQMiarex, CPanel *pPanel, CVector *pNode, CWPolar *pWPolar);
void GLCreateTrans(void *pQMiarex, CWing *pWing, CWOpp *pWOpp, int List);
void GLCreateWingLegend(void *pQMiarex, CWing *pWing, CPlane *pPlane, CWPolar *pWPolar);		
void GLCreateWOppLegend(void* pQMiarex, CWing *pWing, CWPolar *pWPolar, CWOpp *pWOpp);
void GLCreateModeLegend(void *pQMiarex, CWing *pWing, CWOpp *pWOpp);

#endif // GLCREATELISTS_H
