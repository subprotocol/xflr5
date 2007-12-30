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

#include "../X-FLR5.h"
#include "Miarex.h"
#include "stdafx.h"
#include ".\arcball.h"
//#include <GL\gl.h>
#include <GL\glu.h>
#include "math.h"


CArcBall::CArcBall(void)
{
	m_pGLScale = NULL;
	m_pOffx = NULL;
	m_pOffy = NULL;
	m_pTransx = NULL;
	m_pTransy = NULL;

	angle = 0.0;
	cross.Set(0.0,0.0,1.0);

	ax = 0.0; ay = 0.0; az = 0.0;

	ab_quat[0]	= -0.65987748f;
	ab_quat[1]	=  0.38526487f;
	ab_quat[2]	= -0.64508355f;
	ab_quat[3]	=  0.0f;
	ab_quat[4]	= -0.75137258f;
	ab_quat[5]	= -0.33720365f;
	ab_quat[6]	=  0.56721509f;
	ab_quat[7]	=  0.0f;
	ab_quat[8]	=  0.000f;
	ab_quat[9]	=  0.85899049f;
	ab_quat[10]	=  0.51199043f;
	ab_quat[11]	=  0.0f;
	ab_quat[12]	=  0.0f;
	ab_quat[13]	=  0.0f;
	ab_quat[14]	=  0.0f;
	ab_quat[15]	=  1.0f;

	memcpy(ab_last, ab_quat, 16*sizeof(double));
	memcpy(ab_next, ab_quat, 16*sizeof(double));

	// the distance from the origin to the eye
	ab_zoom  = 1.0;
	ab_zoom2 = 1.0;
	// the radius of the arcball
	ab_sphere  = 1.0;
	ab_sphere2 = 1.0;
	// the distance from the origin of the plane that intersects
	// the edge of the visible sphere (tangent to a ray from the eye)
	ab_edge = 1.0;
	// whether we are using a sphere or plane
	ab_planar = false;
	ab_planedist = 0.5;

	ab_start.Set(0.0,0.0,1.0);
	ab_curr.Set(0.0,0.0,1.0);
	ab_eye.Set(0.0,0.0,1.0);
	ab_eyedir.Set(0.0,0.0,1.0);
	ab_up.Set(0.0,1.0,0.0);
	ab_out.Set(1.0,0.0,0.0);

	ab_glp[0]  = 1.0; ab_glp[1]  = 0.0; ab_glp[2]  = 0.0; ab_glp[3]  = 0.0; 
	ab_glp[4]  = 0.0; ab_glp[5]  = 1.0; ab_glp[6]  = 0.0; ab_glp[7]  = 0.0; 
	ab_glp[8]  = 0.0; ab_glp[9]  = 0.0; ab_glp[10] = 1.0; ab_glp[11] = 0.0; 
	ab_glp[12] = 0.0; ab_glp[13] = 0.0; ab_glp[14] = 0.0; ab_glp[15] = 1.0; 

	memcpy(ab_glm, ab_glp, 16*sizeof(double));

	ab_glv[0] = 0;
	ab_glv[1] = 0;
	ab_glv[2] = 640;
	ab_glv[3] = 480;

	sc.Set(0.0,0.0,1.0);
	ec.Set(0.0,0.0,1.0);
}

// the distance from the origin to the eye

CArcBall::~CArcBall(void)
{
}

void CArcBall::SetZoom(double radius, CVector eye, CVector up)
{
	ab_eye     = eye; // store eye vector
	ab_zoom2   = ab_eye.dot(ab_eye);
	ab_zoom    = sqrt(ab_zoom2); // store eye distance
	ab_sphere  = radius;         // sphere radius
	ab_sphere2 = ab_sphere * ab_sphere;
	ab_eyedir  = ab_eye * (1.0 / ab_zoom); // distance to eye
	ab_edge    = ab_sphere2 / ab_zoom; // plane of visible edge

	if(ab_sphere <= 0.0) // trackball mode
	{
		ab_planar = true;
		ab_up = up;
		ab_out = (ab_eyedir * ab_up);
		ab_planedist = (0.0 - ab_sphere) * ab_zoom;
	} 
	else
		ab_planar = false;
}
void CArcBall::GetMatrix()
{
	glGetDoublev(GL_PROJECTION_MATRIX,ab_glp);
	glGetIntegerv(GL_VIEWPORT,ab_glv);
}

// affect the arcball's orientation on openGL
void CArcBall::Rotate() 
{
	glMultMatrixf(ab_quat); 
}


// reset the arcball
void CArcBall::Reset()
{
	QuatIdentity(ab_quat);
	QuatIdentity(ab_last);
}

// begin arcball rotation
void CArcBall::Start(int mx, int my)
{
	// saves a copy of the current rotation for comparison
	QuatCopy(ab_last,ab_quat);
	if(ab_planar)
		ab_start = PlanarCoords((GLdouble)mx,(GLdouble)my);
	else
		ab_start = SphereCoords((GLdouble)mx,(GLdouble)my);

//TRACE("%.3f    %.3f    %.3f \n", ab_start.x, ab_start.y, ab_start.z);
}

// update current arcball rotation
void CArcBall::Move(int mx, int my)
{
	if(ab_planar)
	{
		ab_curr = PlanarCoords((double)mx,(double)my);
		if(ab_curr == ab_start) return;

		// d is motion since the last position
		d = ab_curr - ab_start;

		angle = d.VAbs() * 0.5;
		cosa = cos( angle );
		sina = sin( angle );
		// p is perpendicular to d
		p = (ab_out*d.x)-(ab_up*d.y);
		p.Normalize();
		p *= sina;

		Quaternion(ab_next,(float)p.x,(float)p.y,(float)p.z,(float)cosa);
		QuatNext(ab_quat,ab_last,ab_next);
		// planar style only ever relates to the last point
		QuatCopy(ab_last,ab_quat);
		ab_start = ab_curr;
	} 
	else
	{
		ab_curr = SphereCoords((GLdouble)mx,(GLdouble)my);
		if(ab_curr == ab_start)
		{ // avoid potential rare divide by tiny
			QuatCopy(ab_quat,ab_last);
			return;
		}

		// use a dot product to get the angle between them
		// use a cross product to get the vector to rotate around

		cos2a = ab_start.dot(ab_curr);
		sina = sqrt((1.0 - cos2a)*0.5);
		cosa = sqrt((1.0 + cos2a)*0.5);
		angle = acos(cosa)*180.0/3.141592654;
		cross = (ab_start*ab_curr);
		cross.Normalize();

		cross *=sina;
		Quaternion(ab_next,(float)cross.x,(float)cross.y,(float)cross.z,(float)cosa);

		// update the rotation matrix
		QuatNext(ab_quat,ab_last,ab_next);
	}
}

// reset the rotation matrix
void CArcBall::QuatIdentity(float* q)
{ 
	q[0] =1; q[1] =0; q[2] =0; q[3] =0;
	q[4] =0; q[5] =1; q[6] =0; q[7] =0;
	q[8] =0; q[9] =0; q[10]=1; q[11]=0;
	q[12]=0; q[13]=0; q[14]=0; q[15]=1; 
}

// copy a rotation matrix
void CArcBall::QuatCopy(float* dst, float* src)
{ 
	dst[0]=src[0]; dst[1]=src[1]; dst[2]=src[2];
	dst[4]=src[4]; dst[5]=src[5]; dst[6]=src[6];
	dst[8]=src[8]; dst[9]=src[9]; dst[10]=src[10];
}

// convert the quaternion into a rotation matrix
void CArcBall::Quaternion(float* q, float x, float y, float z, float w)
{
	x2 = x*x;
	y2 = y*y;
	z2 = z*z;
	xy = x*y;
	xz = x*z;
	yz = y*z;
	wx = w*x;
	wy = w*y;
	wz = w*z;

	q[0] = 1 - 2*y2 - 2*z2;
	q[1] = 2*xy + 2*wz;
	q[2] = 2*xz - 2*wy;

	q[4] = 2*xy - 2*wz;
	q[5] = 1 - 2*x2 - 2*z2;
	q[6] = 2*yz + 2*wx;

	q[8] = 2*xz + 2*wy;
	q[9] = 2*yz - 2*wx;
	q[10]= 1 - 2*x2 - 2*y2;
}

// multiply two rotation matrices
void CArcBall::QuatNext(float* dest, float* left, float* right)
{
	dest[0] = left[0]*right[0] + left[1]*right[4] + left[2] *right[8];
	dest[1] = left[0]*right[1] + left[1]*right[5] + left[2] *right[9];
	dest[2] = left[0]*right[2] + left[1]*right[6] + left[2] *right[10];
	dest[4] = left[4]*right[0] + left[5]*right[4] + left[6] *right[8];
	dest[5] = left[4]*right[1] + left[5]*right[5] + left[6] *right[9];
	dest[6] = left[4]*right[2] + left[5]*right[6] + left[6] *right[10];
	dest[8] = left[8]*right[0] + left[9]*right[4] + left[10]*right[8];
	dest[9] = left[8]*right[1] + left[9]*right[5] + left[10]*right[9];
	dest[10]= left[8]*right[2] + left[9]*right[6] + left[10]*right[10];
}



// get intersection with plane for "trackball" style rotation
CVector CArcBall::PlanarCoords(GLdouble mx, GLdouble my)
{
	gluUnProject(mx,my,0,ab_glm,ab_glp,ab_glv,&ax,&ay,&az);
	m.Set(ax- ab_eye.x, ay- ab_eye.y, az- ab_eye.z);
	// intersect the point with the trackball plane
	t = (ab_planedist - ab_zoom)*1.0 / (ab_eyedir.dot(m));
	d = ab_eye + m*t;

	return CVector(d.dot(ab_up),d.dot(ab_out),0.0);
}


// find the intersection with the plane through the visible edge
CVector CArcBall::EdgeCoords(CVector m)
{
	// find the intersection of the edge plane and the ray
	t = (ab_edge - ab_zoom) / (ab_eyedir.dot(m));
	aa = ab_eye + (m*t);
	// find the direction of the eye-axis from that point
	// along the edge plane
	c = (ab_eyedir * ab_edge) - aa;

	// find the intersection of the sphere with the ray going from
	// the plane outside the sphere toward the eye-axis.
	ac = aa.dot(c);
	c2 = c.dot(c);
	q = ( 0.0 - ac - sqrt( ac*ac - c2*(aa.dot(aa)-ab_sphere2 ))) / c2;
	ec = aa+(c*q);
	ec.Normalize();

	return ec;
}


CVector CArcBall::SphereCoords(GLdouble mx, GLdouble my)
{
	// find the intersection with the sphere

	gluUnProject(mx,my,0.0,
		         ab_glm,ab_glp, ab_glv,
				 &ax,&ay,&az);

	ax -= *m_pOffx - *m_pTransx;
	ay -= *m_pOffy + *m_pTransy;

	m.Set(ax-ab_eye.x, ay-ab_eye.y, az-ab_eye.z);
/*
	// mouse position represents ray: eye + t*m
	// intersecting with a sphere centered at the origin
	a = m.x*m.x+m.y*m.y+m.z*m.z;
	b = ab_eye.dot(m);
	delta = (b*b) - a*(ab_zoom2 - ab_sphere2);
	if(delta <= 0.0)
		return EdgeCoords(m);
	t = (-b - sqrt(delta)) / a;

	sc = ab_eye+(m*t);*/

	//more intuitive with ray to z-axis in ortho frustrum
	if(ab_sphere2>ax*ax+ay*ay) sc.Set(ax,ay,sqrt(ab_sphere2-ax*ax-ay*ay));
	else                       sc.Set(ax,ay,0.0);
//	else return EdgeCoords(ax, ay);

	sc.Normalize();
	return sc;
}










