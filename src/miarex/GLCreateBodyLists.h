/****************************************************************************

	GLCreateLists 

	Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/


#ifndef GLCREATEBODYLISTS_H
#define GLCREATEBODYLISTS_H

#include <QtOpenGL>
#include "../objects/Body.h"

void GLCreateBody3DFlatPanels(void *pParent, int iList, Body *pBody);
void GLCreateBody3DSplines(void *pParent, int iList, Body *pBody, int nx, int nh);
void GLCreateBodyMesh(void *pParent, int iList, Body *pBody);

#endif
