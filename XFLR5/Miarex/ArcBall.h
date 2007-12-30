/****************************************************************************

    CArcBall Class
	Copyright (C)  Bradley Smith, March 24, 2006
	Copyright (C) 2007-2008 André Deperrois xflr5@yahoo.com

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

/* Arcball, written by Bradley Smith, March 24, 2006
 *
 * Using the arcball:
 *   Call arcball_setzoom after setting up the projection matrix.
 *
 *     The arcball, by default, will act as if a sphere with the given
 *     radius, centred on the origin, can be directly manipulated with
 *     the mouse. Clicking on a point should drag that point to rest under
 *     the current mouse position. eye is the position of the eye relative
 *     to the origin. up is unused.
 *
 *     Alternatively, pass the value: (-radius/|eye|)
 *     This puts the arcball in a mode where the distance the mouse moves
 *     is equivalent to rotation along the axes. This acts much like a
 *     trackball. (It is for this mode that the up vector is required,
 *     which must be a unit vector.)
 *
 *     You should call arcball_setzoom after use of gluLookAt.
 *     gluLookAt(eye.x,eye.y,eye.z, ?,?,?, up.x,up.y,up.z);
 *     The arcball derives its transformation information from the
 *     openGL projection and viewport matrices. (modelview is ignored)
 *
 *     If looking at a point different from the origin, the arcball will still
 *     act as if it centred at (0,0,0). (You can use this to translate
 *     the arcball to some other part of the screen.)
 *
 *   Call arcball_start with a mouse position, and the arcball will
 *     be ready to manipulate. (Call on mouse button down.)
 *   Call arcball_move with a mouse position, and the arcball will
 *     find the rotation necessary to move the start mouse position to
 *     the current mouse position on the sphere. (Call on mouse move.)
 *   Call arcball_rotate after resetting the modelview matrix in your
 *     drawing code. It will call glRotate with its current rotation.
 *   Call arcball_reset if you wish to reset the arcball rotation.
 */

#pragma once
#include "../misc/Vector.h"

class CArcBall
{
public:
	CArcBall(void);
	~CArcBall(void);


	void SetZoom(double radius, CVector eye, CVector up);
	void Rotate();
	void Reset();
	void Start(int mx, int my);
	void Move(int mx, int my);
	void GetMatrix();
	CVector SphereCoords(double mx, double my);// find the intersection with the sphere
	CVector PlanarCoords(double mx, double my);// get intersection with plane for "trackball" style rotation
	CVector EdgeCoords(CVector m);	// find the intersection with the plane through the visible edge

	void QuatIdentity(float* q);	// reset the rotation matrix
	void QuatCopy(float* dst, float* src);// copy a rotation matrix
	void Quaternion(float* q, float x, float y, float z, float w);// convert the quaternion into a rotation matrix
	void QuatNext(float* dest, float* left, float* right);// multiply two rotation matrices

	float ab_quat[16];
	float ab_last[16];
	float ab_next[16];

	// the distance from the origin to the eye
	double ab_zoom;
	double ab_zoom2;
	// the radius of the arcball
	double ab_sphere;
	double ab_sphere2;
	// the distance from the origin of the plane that intersects
	// the edge of the visible sphere (tangent to a ray from the eye)
	double ab_edge;
	// whether we are using a sphere or plane
	bool ab_planar;
	double ab_planedist;

	CVector ab_start;
	CVector ab_curr;
	CVector ab_eye;
	CVector ab_eyedir;
	CVector ab_up;
	CVector ab_out;

	double ab_glp[16];
	double ab_glm[16];
	int ab_glv[4];

	//	object offset
	double *m_pOffx, *m_pOffy;
	float *m_pTransx, *m_pTransy;
	double *m_pGLScale;
	//avoid length recurring memory allocations
	CVector aa, c, m, ec, sc, p, d, cross;
	double angle, cosa, sina, cos2a, t, ac, c2, q, b, delta, a;
	float x2, y2, z2, xy, xz, yz, wx, wy, wz;
	double ax,ay,az;

};

 

