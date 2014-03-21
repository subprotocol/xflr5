/****************************************************************************

    Panel Class
	Copyright (C) 2006-2013 Andre Deperrois adeperrois@xflr5.com

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
 * This file defines the classes for quad panel object used both in VLM and in 3d-panel analysis
 *
 */

#ifndef PANEL_H
#define PANEL_H

#include "Quaternion.h"
#include "CVector.h"

/**
 * enumeration used to identify the type of surface on which the panel lies.
 * May be on a bottom, mid, top, side, or body surface.
 */
typedef enum {BOTSURFACE, MIDSURFACE, TOPSURFACE, SIDESURFACE, BODYSURFACE} enumPanelPosition;

/**
*@class Panel
*@brief	This class defines the quad panel object used both in VLM and in 3d-panel analysis

*	The class provides member variables which define the geometric properties of the panel, and functions used in the 3D analysis.
*
*	The name of the variables follows closely the naming used in the document NASA Contractor report 4023 "Program VSAERO Theory Document".
	Refer to this document for detailed explanations on the description of the panel and the meaning of the variables.
	The nodes are defined in a separate global array. The index of the nodes at the four corners are stored as
	member variables of this panel.
*
*	For VLM calculations, the position and length vector of the bound vortex at the panel's quarter-chord are
	stored as member variables.
*/
class Panel
{
	friend class Surface;
	friend class Wing;
	friend class QMiarex;
	friend class Objects3D;
	friend class Body;
	friend class PanelAnalysis;
	friend class PanelAnalysisDlg;
	friend class GL3dBodyDlg;
	friend class GL3dWingDlg;

public:
	Panel();

	void VLMCmn(CVector const &C, CVector &VTest, bool const &bAll);
	void DoubletNASA4023(CVector const &C, CVector &VTest, double &phi, bool bWake);
	void SourceNASA4023(CVector const &C, CVector &VTest, double &phi);

	void RotateBC(CVector const &HA, Quaternion & Qt);
	void Reset();
	void SetPanelFrame();
	void SetPanelFrame(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB);
	bool Intersect(CVector const &A, CVector const &U, CVector &I, double &dist);
	bool Invert33(double *l);
	CVector GlobalToLocal(CVector const &VTest);
	CVector GlobalToLocal(double const &Vx, double const &Vy, double const &Vz);
	CVector LocalToGlobal(CVector const &VTest);

	double Width();
	double GetArea();

	static void setCoreSize(double CoreSize) { s_CoreSize=CoreSize;	}
	static double coreSize() { return s_CoreSize; }


protected:	
	bool m_bIsInSymPlane;    /**< true if the panel lies in the plane's xz plane of symetry at y=0*/
	bool m_bIsLeftPanel;     /**< true if the panel lies on the left (port) wing */
	bool m_bIsWakePanel;     /**< true if the panel lies on the wake of a winf */

	int m_iElement;          /**< panel identification number ; used when the panel array is re-arranged in non sequential order to reduce the matrix size in symetrical calculations */
	int m_iPL;               /**< index of the panel which lies left of this panel, or -1 if none */
	int m_iPR;               /**< index of the panel which lies right of this panel, or -1 if none */
	int m_iPU;               /**< index of the panel which lies upstream of this panel, or -1 if none */
	int m_iPD;               /**< index of the panel which lies downstream of this panel, or -1 if none */
	int m_iWake;             /**< -1 if not followed by a wake panel, else equal to wake panel number */
	int m_iWakeColumn;       /**< index of the wake column shed by this panel, numbered from left tip to right tip, or -1 if none */

	//Local frame of refernce
	CVector VortexPos;       /**< the absolute position of the mid point of the bound vortex at the panel's quarter chord */
	CVector Vortex;          /**< the bound vortex vector at the panel's quarter chord */
	CVector P1;              /**< the coordinates of the panel's corners, in local coordinates */
	CVector P2;              /**< the coordinates of the panel's corners, in local coordinates */
	CVector P3;              /**< the coordinates of the panel's corners, in local coordinates */
	CVector P4;              /**< the coordinates of the panel's corners, in local coordinates */
	CVector m, l;            /**< the unit vectors which lie in the panel's plane. Cf. document NACA 4023 */

	double dl;               /**< The length of the bound vector */
	double Area;             /**< The panel's area; */
	double SMP, SMQ;
	double Size;             /**< = SMP + SMQ and provides an estimation of the panel's size.
	                              This is used to determine if the far-field approximation can be used in 
								  the evaluation of the source and doublet influent at a distant point */
	double lij[9];           /**< The 3x3 matrix used to transform local coordinates in absolute coordinates */

	static CVector *s_pNode;       /**< A static pointer to the global array of panel nodes */
	static CVector *s_pWakeNode;   /**< A static pointer to the global array of wake panel nodes */

	static double s_VortexPos; /**< Defines the relative position of the bound vortex in the streamwise direction. Usually the vortex is positioned at the panel's quarter chord i.e. s_VortexPos=0.25 */
	static double s_CtrlPos;   /**< Defines the relative position of the panel's control point in VLM. Usually the control point is positioned at the panel's 3/4 chord : s_VortexPos=0.75 */
	static double det;         /**< temporary variable */
	static double mat[9];      /**< temporary array  */


public:
	enumPanelPosition m_Pos;   /**< defines if the panel is positioned on a top, middle, bottom, side or body surface */
	bool m_bIsLeading;         /**< true if the panel is positioned on a leading edge */
	bool m_bIsTrailing;        /**< true if the panel is positioned on a trailing edge */
	int m_iLA;                 /**< index of the leading left node in the node array */
	int m_iLB;                 /**< index of the leading right node in the node array */
	int m_iTA;                 /**< index of the trailing left node in the node array */
	int m_iTB;                 /**< index of the trailing right node in the node array */
	CVector Normal;            /**< the unit vector normal to the panel */
	CVector CtrlPt;            /**< the position of the control point for VLM analysis or 3D/Thin panels analysis */
	CVector CollPt;            /**< the collocation point for 3d panel analysis */
	CVector VA;                /**< the left end point of the bound quarter-chord vortex on this panel */
	CVector VB;                /**< the rightt end point of the bound quarter-chord vortex on this panel */

	static double s_CoreSize;
};

#endif
