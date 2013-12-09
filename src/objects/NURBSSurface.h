/****************************************************************************

		 SplineSurface Class
		 Copyright (C) 2012 Andre Deperrois adeperrois@xflr5.com

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
  * @file this class implements the NURBSSurface class.
  */

#ifndef SPLINESURFACE_H
#define SPLINESURFACE_H


//#include "../params.h"
#include "Frame.h"

#define MAXVLINES      17
#define MAXULINES      19


/**
 * @class NURBSSurface
 * This class implements a 3D NURBSsurface built on an array of Frame objects.
 * The NURBS surface is described by two parameters u and v with range in [0,1].
 *
 * The NURBS control points are those of the array of Frames objects.
 * When used to describe a half body, u describes the NURBS in the x direction, and v in the z direction.
 *
*/
class NURBSSurface
{

public:
	NURBSSurface(int iAxis=0);
	~NURBSSurface();

	Frame *AppendFrame();
	void   ClearFrames();
	int    FrameSize() {return m_pFrame.size();}
	int    FramePointCount() {return m_pFrame.first()->PointCount();}
	double Getu(double pos, double v);
	double Getv(double u, CVector r);
	void   GetPoint(double u, double v, CVector &Pt);
	void   InsertFrame(Frame *pNewFrame);
	bool   IntersectNURBS(CVector A, CVector B, CVector &I);
	void   RemoveFrame(int iFrame);
	void   SetKnots();
	int    SetvDegree(int nvDegree);
	int    SetuDegree(int nuDegree);
	double Weight(const double &d, int const &i, int const &N);

	static void* s_pMainFrame;      /**< a static pointer to the instance of the MainFrame object */
	QList<Frame*> m_pFrame;	        /**< a pointer to the array of Frame objects */

	int m_iuDegree;                 /**< the degree of the NURBS in the u direction */
	int m_ivDegree;                 /**< the degree of the NURBS in the v direction */
	int m_nuKnots;                  /**< the number of knots in the u direction */
	int m_nvKnots;                  /**< the number of knots in the v direction */

	double m_uKnots[MAXVLINES*2];   /**< the array of knots in the u direction */
	double m_vKnots[MAXULINES*2];   /**< the array of knots in the v direction */

	int m_iRes;                     /**< the number of output points to draw the NURBS in both directions */

	double m_Bunch;                 /**< a bunch parameter used to modify the output of point density at the end or at the middle of the NURBS */
	double m_EdgeWeightu;           /**< for a full NURBS. Unused, though, not practical */
	double m_EdgeWeightv;           /**< for a full NURBS. Unused, though, not practical */

	int m_uAxis;                    /**< used to identify along which axis parameter u is set; 0=x, 1=y, 2=z */
	int m_vAxis;                    /**< used to identify along which axis parameter u is set; 0=x, 1=y, 2=z */


	//use temporary variables to avoid lengthy memory allocation times on the stack
	double value, eps, bs, cs;
	CVector t_R, t_Prod, t_Q, t_r, t_N;
};

#endif // SPLINESURFACE_H

