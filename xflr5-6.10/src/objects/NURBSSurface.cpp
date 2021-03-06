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


#include "../params.h"
#include "../globals.h"
#include "NURBSSurface.h"


void *NURBSSurface::s_pMainFrame;

/**
 * The public constructor.
 * @param iAxis defines along which preferred axis the paramater u is directed; v is in the y-direction
 */
NURBSSurface::NURBSSurface(int iAxis)
{
	m_pFrame.clear();
#if QT_VERSION >= 0x040700
    m_pFrame.reserve(10);
#endif

	m_uAxis = iAxis;//directed in x direction, mainly
	m_vAxis = 2;//directed in y direction, mainly


	m_iuDegree = 2;
	m_ivDegree = 2;
	m_nuKnots = 0;
	m_nvKnots = 0;

	m_iRes = 31;

	m_Bunch  = 0.0;
	m_EdgeWeightu = 1.0;
	m_EdgeWeightv = 1.0;

	eps = 1.0e-06;
}

NURBSSurface::~NURBSSurface()
{
	for(int iFr=m_pFrame.size()-1; iFr>=0; iFr--)
	{
		delete m_pFrame.at(iFr);
		m_pFrame.removeAt(iFr);
	}
}


/**
 * Returns the u-parameter for a given value along the axis and a given v parameter
 * Proceeds by iteration - time consuming,
 * @param pos the point coordinate for which the parameter u is requested
 * @param v the specified value of the v-parameter
 * @return the value of the u-parameter
 */
double NURBSSurface::Getu(double pos, double v)
{
	if(pos<=m_pFrame.first()->m_Position[m_uAxis]) return 0.0;
	if(pos>=m_pFrame.last()->m_Position[m_uAxis])  return 1.0;
	if(qAbs(m_pFrame.last()->m_Position[m_uAxis] - m_pFrame.first()->m_Position[m_uAxis])<0.0000001) return 0.0;

	int iter=0;
	double u2, u1, b, c, u,  zz, zh;
	u1 = 0.0; u2 = 1.0;

//	v = 0.0;//use top line, but doesn't matter
	while(qAbs(u2-u1)>1.0e-6 && iter<200)
	{
		u=(u1+u2)/2.0;
		zz = 0.0;
		for(int iu=0; iu<FrameSize(); iu++) //browse all points
		{
			zh = 0.0;
			for(int jv=0; jv<FramePointCount(); jv++)
			{
				c =  SplineBlend(jv, m_ivDegree, v, m_vKnots);
				zh += m_pFrame[iu]->m_Position[m_uAxis] * c;
			}
			b = SplineBlend(iu, m_iuDegree, u, m_uKnots);
			zz += zh * b;
		}
		if(zz>pos) u2 = u;
		else       u1 = u;
		iter++;
	}
	return (u1+u2)/2.0;
}


/**
 * Returns the v-parameter for a given value  of u and a geometrical point
 * Proceeds by iteration - time consuming,
 * @param u the specified value of the u-parameter
 * @param r the point for which v is requested
 * @return the value of the v-parameter
 */

double NURBSSurface::Getv(double u, CVector r)
{
	double sine = 10000.0;

	if(u<=0.0)          return 0.0;
	if(u>=1.0)          return 0.0;
	if(r.VAbs()<1.0e-5) return 0.0;

	int iter=0;
	double v, v1, v2;

	r.Normalize();
	v1 = 0.0; v2 = 1.0;

	while(qAbs(sine)>1.0e-4 && iter<200)
	{
		v=(v1+v2)/2.0;
		GetPoint(u, v, t_R);
		t_R.x = 0.0;
		t_R.Normalize();//t_R is the unit radial vector for u,v

		sine = (r.y*t_R.z - r.z*t_R.y);

		if(sine>0.0) v1 = v;
		else         v2 = v;
		iter++;
	}

	return (v1+v2)/2.0;
}



/**
 * Returns the point corresponding to the pair of parameters (u,v)
 * Assumes that the knots have been set previously
 *
 * Scans the u-direction first, then v-direction
 * @param u the specified u-parameter
 * @param v the specified v-parameter
 * @param Pt a reference to the point defined by the pair (u,v)
*/
void NURBSSurface::GetPoint(double u, double v, CVector &Pt)
{
	CVector V, Vv;
	double wx, weight;

	if(u>=1.0) u=0.99999999999;
	if(v>=1.0) v=0.99999999999;

	weight = 0.0;
	for(int iu=0; iu<FrameSize(); iu++)
	{
		Vv.Set(0.0,0.0,0.0);
		wx = 0.0;
		for(int jv=0; jv<FramePointCount(); jv++)
		{
			cs = SplineBlend(jv, m_ivDegree, v, m_vKnots) * Weight(m_EdgeWeightv, jv, FramePointCount());

			Vv.x += m_pFrame[iu]->m_CtrlPoint[jv].x * cs;
			Vv.y += m_pFrame[iu]->m_CtrlPoint[jv].y * cs;
			Vv.z += m_pFrame[iu]->m_CtrlPoint[jv].z * cs;

			wx += cs;
		}
		bs = SplineBlend(iu, m_iuDegree, u, m_uKnots) * Weight(m_EdgeWeightu, iu, FrameSize());

		V.x += Vv.x * bs;
		V.y += Vv.y * bs;
		V.z += Vv.z * bs;

		weight += wx * bs;
	}

	Pt.x = V.x / weight;
	Pt.y = V.y / weight;
	Pt.z = V.z / weight;
}


/**
 * Returns the weight of the control point
 * @param i the index of the point along the edge
*  @param N the total number of points along the edge
*  @return the point's weight
**/
double NURBSSurface::Weight(double const &d, const int &i, int const &N)
{
	if(qAbs(d-1.0)<PRECISION) return 1.0;
	if(i<(N+1)/2)             return pow(d, i);
	else                      return pow(d, N-i-1);
}


/*
double NURBSSurface::Weight(int i, int N)
{
	// returns the weight of the control point
	// i is the index of the point along the edge
	// N is total number of points along the edge

	if(qAbs(m_EdgeWeight-1.0)<PRECISION) return 1.0;
	if(i<N/2)                            return 1./pow(m_EdgeWeight, (int)((N-1)/2-i));
	else                                 return 1./pow(m_EdgeWeight, i-(int)(N/2));
}*/

/**
 * Intersects a line segment AB with the NURBS surface. The points are expected to be on each side of the NURBS surface.
 *@param A the first point which defines the ray
 *@param B the second point which defines the ray
 *@param I the intersection point
 *@return true if an intersection point could be determined
 */
bool NURBSSurface::IntersectNURBS(CVector A, CVector B, CVector &I)
{
	CVector  tmp, M0, M1;
	double u, v, dist, t, tp;
	int iter = 0;
	int itermax = 20;
	double dmax = 1.0e-5;
	dist = 1000.0;//m

	M0.Set(0.0, A.y, A.z);
	M1.Set(0.0, B.y, B.z);

	if(M0.VAbs()<M1.VAbs())
	{
		tmp = A;		A = B;		  B = tmp;
	}

	//M0 is the outside Point, M1 is the inside point
	M0 = A; M1 = B;


	I = (M0+M1)/2.0; t=0.5;

	while(dist>dmax && iter<itermax)
	{
		//first we get the u parameter corresponding to point I
		tp = t;
		u = Getu(I.z, 0.0);
		t_Q.Set(I.x, 0.0, 0.0);
		t_r = (I-t_Q);
		v = Getv(u, t_r);
		GetPoint(u, v, t_N);

		//project t_N on M0M1 line
		t = - ( (M0.x - t_N.x) * (M1.x-M0.x) + (M0.y - t_N.y) * (M1.y-M0.y) + (M0.z - t_N.z)*(M1.z-M0.z))
			 /( (M1.x -  M0.x) * (M1.x-M0.x) + (M1.y -  M0.y) * (M1.y-M0.y) + (M1.z -  M0.z)*(M1.z-M0.z));

		I.x = M0.x + t * (M1.x-M0.x);
		I.y = M0.y + t * (M1.y-M0.y);
		I.z = M0.z + t * (M1.z-M0.z);

//		dist = sqrt((t_N.x-I.x)*(t_N.x-I.x) + (t_N.y-I.y)*(t_N.y-I.y) + (t_N.z-I.z)*(t_N.z-I.z));
		dist = qAbs(t-tp);
		iter++;
	}

	return dist<dmax;
}


/**
 * Creates the knot array for the two directions
 */
void NURBSSurface::SetKnots()
{
	int j;
	double b;
	if(!FrameSize())return;
	if(!FramePointCount())return;

	m_iuDegree = qMin(m_iuDegree, FrameSize());
	m_nuKnots  = m_iuDegree + FrameSize() + 1;
	b = (double)(m_nuKnots-2*m_iuDegree-1);

	for (j=0; j<m_nuKnots; j++)
	{
		if (j<m_iuDegree+1)  m_uKnots[j] = 0.0;
		else
		{
			if (j<FrameSize())
			{
				if(qAbs(b)>0.0) m_uKnots[j] = (double)(j-m_iuDegree)/b;
				else            m_uKnots[j] = 1.0;
			}
			else m_uKnots[j] = 1.0;
		}
	}

	m_ivDegree = qMin(m_ivDegree, m_pFrame.first()->m_CtrlPoint.size());

	m_nvKnots  = m_ivDegree + FramePointCount() + 1;
	b = (double)(m_nvKnots-2*m_ivDegree-1);

	for (j=0; j<m_nvKnots; j++)
	{
		if (j<m_ivDegree+1)  m_vKnots[j] = 0.0;
		else
		{
			if (j<FramePointCount())
			{
				if(qAbs(b)>0.0) m_vKnots[j] = (double)(j-m_ivDegree)/b;
				else            m_vKnots[j] = 1.0;
			}
			else m_vKnots[j] = 1.0;
		}
	}
}


/**
 * Specifies the degree in the u-direction.
 * If the degree specified by the input parameter is equal or greater than the Frame count,
 * then the degree is set to the number of frames minus 1.
 * @param nvDegree the specified degree
 * @return the degree which has been set
 */
int NURBSSurface::SetvDegree(int nvDegree)
{
	if(FramePointCount()>nvDegree) m_ivDegree = nvDegree;
	else                           m_ivDegree = FramePointCount()-1;
	return m_ivDegree;
}





/**
 * Specifies the degree in the u-direction.
 * If the degree specified by the input parameter is equal or greater than the Frame count,
 * then the degree is set to the number of frames minus 1.
 * @param nuDegree the specified degree
 * @return the degree which has been set
 */
int NURBSSurface::SetuDegree(int nuDegree)
{
	if(FrameSize()>nuDegree) m_iuDegree = nuDegree;
	else                     m_iuDegree = FrameSize()-1;
	return m_iuDegree;
}


/**
 * Removes a Frame from the array
 * @param iFrame the index of the frame to remove
 */
void NURBSSurface::RemoveFrame(int iFrame)
{
	delete m_pFrame.at(iFrame);
	m_pFrame.removeAt(iFrame);
}



/**
 * Removes all the Frame objects from the array
 */
void NURBSSurface::ClearFrames()
{
	if(!FrameSize()) return;
	for(int ifr=FrameSize()-1; ifr>=0; ifr--)
	{
		RemoveFrame(ifr);
	}
}

/**
 * Inserts a Frame in the array. The Frame is positioned in crescending position along the u-axis
 * @param pNewFrame a pointer to the Frame object to insert.
 */
void NURBSSurface::InsertFrame(Frame *pNewFrame)
{
	for(int ifr=0; ifr<FrameSize(); ifr++)
	{
		if(pNewFrame->m_Position[m_uAxis] < m_pFrame.at(ifr)->m_Position[m_uAxis])
		{
			m_pFrame.insert(ifr, pNewFrame);
			return;
		}
	}

	m_pFrame.append(pNewFrame); //either the first if none, either the last...
}

/**
 * Appends a new Frame at the end of the array
 * @return a pointer to the Frame which has been created.
 */
Frame * NURBSSurface::AppendFrame()
{
	m_pFrame.append(new Frame);
	return m_pFrame.last();
}








