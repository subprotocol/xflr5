/****************************************************************************

	Objects3D    Copyright (C) 2014 Andre Deperrois adeperrois@xflr5.com

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
  * @file This file implements the variables and methods used to manage 3D objects
  */


#ifndef PREPOST3D_H
#define PREPOST3D_H

#include <QList>
#include "../objects/Body.h"
#include "../objects/Plane.h"
#include "./analysis/PanelAnalysis.h"

class Objects3D
{
public:
	Objects3D();

	static bool      AllocatePanelArrays(int &memsize);
	static void      addBody(Body *pBody);
	static void      addPlane(Plane *pPlane);
	static void      addWPolar(WPolar *pWPolar);
	static int       calculateMatSize(Plane *pPlane, WPolar *pWPolar);
	static int       createBodyElements(Plane *pCurPlane);
	static bool      createWakeElems(int PanelIndex, Plane *pPlane, WPolar *pWPolar);
	static int       createWingElements(Plane *pPlane, WPolar *pWPolar, Surface *pSurface);
	static void      deleteObjects();
	static void      deletePlane(Plane *pPlane);
	static void      deletePlaneResults(Plane *pPlane);
	static Plane *   duplicatePlane(Plane *pPlane);
	static Body*     getBody(QString BodyName);
	static Plane*    getPlane(QString PlaneName);
	static Wing*     getWing(QString PlaneName);
	static PlaneOpp* getPlaneOpp(Plane *pPlane, WPolar *pWPolar, double x);
	static WPolar*   getWPolar(Plane *pPlane, QString WPolarName);
	static bool      initializePanels(Plane *pPlane, WPolar *pWPolar);
	static void      initPanelAnalysis(Plane* pPlane, WPolar* pWPolar, double V0, double VMax, double VDelta, bool bSequence);
	static void      InsertPOpp(PlaneOpp *pPOpp);
	static int       IsNode(CVector &Pt);
	static int       IsWakeNode(CVector &Pt);
	static void      JoinSurfaces(WPolar*pWPolar, Surface *pLeftSurf, Surface *pRightSurf, int pl, int pr);
	static void      rotateGeomY(Panel *pPanel, CVector *pNode, Panel *pWakePanel, CVector *pWakeNode, int nPanels, int nWakePanels, int nNodes, int nWakeNodes, int NWakeColumn, double const &Alpha, CVector const &P, int NXWakePanels);
	static void      rotateGeomZ(Panel *pPanel, CVector *pNode, Panel *pWakePanel, CVector *pWakeNode, int nPanels, int nWakePanels, int nNodes, int nWakeNodes, int NWakeColumn, double const &Beta, CVector const &P, int NXWakePanels);
	static void      ReleasePanelMemory();
	static void      renamePlane(QString PlaneName);
	static Plane *   setModPlane(Plane *pModPlane);
	static WPolar *  insertNewWPolar(WPolar *pModWPolar, Plane *pCurPlane);
	static void      setStaticPointers();
	static WPolar *  setWPolarObject(Plane *pCurPlane, WPolar *pCurWPolar, bool bCurrentWPolar, QString WPlrName);
	static PlaneOpp* setPlaneOppObject(Plane *pPlane, WPolar *pWPolar, PlaneOpp *pCurPOpp, bool bCurrent, double x=0);
	static Plane *   setPlaneObject(QString PlaneName, Plane *pCurPlane);
	static void      setControlPositions(Plane *pPlane, WPolar *pWPolar, Panel *pPanel, CVector *pNode, double t, int &NCtrls, QString &out, bool bBCOnly);


public:
	// object variable lists

	static QList <void *> s_oaPlane;   /**< The array of void pointers to the Plane objects. */
	static QList <void *> s_oaBody;    /**< The array of void pointers to the Body objects. */
	static QList <void *> s_oaWPolar;  /**< The array of void pointers to the WPolar objects. */
	static QList <void *> s_oaPOpp;    /**< The array of void pointers to the PlaneOpp objects. */

	//data arrays
	static CVector *s_Node;              /**< the node array for the currently loaded UFO*/
	static CVector *s_MemNode;           /**< used if the analysis should be performed on the tilted geometry */
	static CVector *s_WakeNode;          /**< the current wake node array */
	static CVector *s_RefWakeNode;       /**< the reference wake node array if wake needs to be reset */

	static Panel *s_Panel;               /**< the panel array for the currently loaded UFO */
	static Panel *s_MemPanel;            /**< used if the analysis should be performed on the tilted geometry */
	static Panel *s_WakePanel;           /**< the reference current wake panel array */
	static Panel *s_RefWakePanel;        /**< the reference wake panel array if wake= new CVector needs to be reset */

	static QList<Surface *> s_SurfaceList;	    /**< An array holding the pointers to the wings Surface objects */


	static int s_MaxPanelSize;                  /**< the maximum matrix size consistent <ith the current memory allocation */
//	static int s_MaxRHSSize;                  /**< the maximum RHS size consistent <ith the current memory allocation */

	static int s_MatSize;                     /**< the matrix size, wich is also the size of the panel array */
	static int s_WakeSize;                    /**< the size of the wake matrix, if a wake is included in the analysis */
	static int s_nNodes;                      /**< the number of nodes corresponding to the panel array */
	static int s_nWakeNodes;                  /**< the number of nodes in the wake */
	static int s_NWakeColumn;                 /**< the number of wake columns */


	static double s_LastAlpha;          /**< last angle of attack selected in the top list box>*/
	static double s_LastBeta;           /**< last sideslip angle selected in the top list box>*/

	static PanelAnalysis *s_pPanelAnalysis;   /**< the one and only instance of the 3D analysis class. */
	static int s_MaxWakeIter;                 /**< wake roll-up iteration limit */
};

#endif // PREPOST3D_H
