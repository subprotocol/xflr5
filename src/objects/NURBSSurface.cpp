/****************************************************************************

		 SplineSurface Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/


#include "../params.h"
#include "../globals.h"
#include "NURBSSurface.h"
#include <QtDebug>


void *NURBSSurface::s_pMainFrame;


NURBSSurface::NURBSSurface(int iAxis)
{
	m_pFrame.clear();
	m_pFrame.reserve(10);

	m_uAxis = iAxis;//directed in x direction, mainly
	m_vAxis = 2;//directed in y direction, mainly

//	m_nuLines = 0;
	m_nvLines = 0;

	m_iuDegree = 2;
	m_ivDegree = 2;
	m_nuKnots = 0;
	m_nvKnots = 0;

	m_nuPanels = 19;
	m_nvPanels = 11;

	m_NElements = m_nuPanels * m_nvPanels * 2;

	m_iRes = 31;

	m_Bunch  = 0.0;
	m_EdgeWeightu = 1.0;
	m_EdgeWeightv = 1.0;

	eps = 1.0e-06;

	for(int i=0; i<MAXVLINES;i++) m_uPanels[i] = 1;
	for(int i=0; i<MAXULINES;i++) m_vPanels[i] = 1;

	m_np = 0;

//	SetKnots();
}



double NURBSSurface::Getu(double pos, double v)
{
	if(pos<=m_pFrame.first()->m_Position[m_uAxis]) return 0.0;
	if(pos>=m_pFrame.last()->m_Position[m_uAxis])  return 1.0;
	if(fabs(m_pFrame.last()->m_Position[m_uAxis] - m_pFrame.first()->m_Position[m_uAxis])<0.0000001) return 0.0;

	int iter=0;
	double u2, u1, b, c, u,  zz, zh;
	u1 = 0.0; u2 = 1.0;

//	v = 0.0;//use top line, but doesn't matter
	while(fabs(u2-u1)>1.0e-6 && iter<200)
	{
		u=(u1+u2)/2.0;
		zz = 0.0;
		for(int iu=0; iu<m_pFrame.size(); iu++) //browse all points
		{
			zh = 0.0;
			for(int jv=0; jv<m_nvLines; jv++)
			{
				c =  SplineBlend(jv, m_ivDegree, v, m_vKnots);
				zh += m_pFrame[iu]->m_Position[m_uAxis] * c;
			}
			b = SplineBlend(iu, m_iuDegree, u, m_uKnots);
			zz += zh * b;
		}
		if(zz>pos) u2 = u;
		else     u1 = u;
		iter++;
	}
	return (u1+u2)/2.0;
}



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

	while(fabs(sine)>1.0e-4 && iter<200)
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


void NURBSSurface::GetPoint(double u, double v, CVector &Pt)
{
	//returns the point corresponding to the parametric values u and v
	//assumes that the knots have been set previously

	//Scan u-direction first, then v-direction

	CVector V, Vv;
	double wx, weight;

	if(u>=1.0) u=0.99999999999;
	if(v>=1.0) v=0.99999999999;

	weight = 0.0;
	for(int iu=0; iu<m_pFrame.size(); iu++)
	{
		Vv.Set(0.0,0.0,0.0);
		wx = 0.0;
		for(int jv=0; jv<m_nvLines; jv++)
		{
			cs = SplineBlend(jv, m_ivDegree, v, m_vKnots) * Weight(m_EdgeWeightv, jv, m_nvLines);

			Vv.x += m_pFrame[iu]->m_CtrlPoint[jv].x * cs;
			Vv.y += m_pFrame[iu]->m_CtrlPoint[jv].y * cs;
			Vv.z += m_pFrame[iu]->m_CtrlPoint[jv].z * cs;

			wx += cs;
		}
		bs = SplineBlend(iu, m_iuDegree, u, m_uKnots) * Weight(m_EdgeWeightu, iu, m_pFrame.size());

		V.x += Vv.x * bs;
		V.y += Vv.y * bs;
		V.z += Vv.z * bs;

		weight += wx * bs;
	}

	Pt.x = V.x / weight;
	Pt.y = V.y / weight;
	Pt.z = V.z / weight;
}


double NURBSSurface::Weight(double const &d, const int &i, int const &N)
{
	// returns the weight of the control point
	// i is the index of the point along the edge
	// N is total number of points along the edge

	if(fabs(d-1.0)<PRECISION) return 1.0;
	if(i<(N+1)/2)             return pow(d, i);
	else                      return pow(d, N-i-1);
}


/*
double NURBSSurface::Weight(int i, int N)
{
	// returns the weight of the control point
	// i is the index of the point along the edge
	// N is total number of points along the edge

	if(fabs(m_EdgeWeight-1.0)<PRECISION) return 1.0;
	if(i<N/2)                            return 1./pow(m_EdgeWeight, (int)((N-1)/2-i));
	else                                 return 1./pow(m_EdgeWeight, i-(int)(N/2));
}*/



void NURBSSurface::SetPanelPos()
{
	int i;
	for(i=0; i<=m_nuPanels; i++)
	{
		m_vPanelPos[i] =(double)i/(double)m_nuPanels;
	}
	return;
	double y, x;
	double a = (m_Bunch+1.0)*.48 ;
	a = 1./(1.0-a);

	double norm = 1/(1+exp(0.5*a));

	for(i=0; i<=m_nuPanels; i++)
	{
		x = (double)(i)/(double)m_nuPanels;
		y = 1.0/(1.0+exp((0.5-x)*a));
		m_vPanelPos[i] =0.5-((0.5-y)/(0.5-norm))/2.0;
	}
}


bool NURBSSurface::IntersectNURBS(CVector A, CVector B, CVector &I)
{
	//intersect line AB with NURBS
	//intersection point is I
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

/*	if(!IsInNURBSBody(M1))
	{
		//consider no intersection (not quite true in special high dihedral cases)
		I = M1;
		return false;
	}*/

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
		dist = fabs(t-tp);
		iter++;
	}

	return dist<dmax;
}


void NURBSSurface::SetKnots()
{
	int j;
	double b;
	if(!m_pFrame.size())return;
	if(!m_pFrame.first()->m_CtrlPoint.size())return;

	// z-dir knots
	m_iuDegree = qMin(m_iuDegree, m_pFrame.size());
	m_nuKnots  = m_iuDegree + m_pFrame.size() + 1;
	b = (double)(m_nuKnots-2*m_iuDegree-1);

	for (j=0; j<m_nuKnots; j++)
	{
		if (j<m_iuDegree+1)  m_uKnots[j] = 0.0;
		else
		{
			if (j<m_pFrame.size())
			{
				if(fabs(b)>0.0) m_uKnots[j] = (double)(j-m_iuDegree)/b;
				else            m_uKnots[j] = 1.0;
			}
			else m_uKnots[j] = 1.0;
		}
	}

	//hoop knots = x-direction
	m_ivDegree = qMin(m_ivDegree, m_pFrame.first()->m_CtrlPoint.size());

	m_nvKnots  = m_ivDegree + m_pFrame.first()->m_CtrlPoint.size() + 1;
	b = (double)(m_nvKnots-2*m_ivDegree-1);

	for (j=0; j<m_nvKnots; j++)
	{
		if (j<m_ivDegree+1)  m_vKnots[j] = 0.0;
		else
		{
			if (j<m_nvLines)
			{
				if(fabs(b)>0.0) m_vKnots[j] = (double)(j-m_ivDegree)/b;
				else            m_vKnots[j] = 1.0;
			}
			else m_vKnots[j] = 1.0;
		}
	}
}


int NURBSSurface::SetvDegree(int nvDegree)
{
	if(m_nvLines>nvDegree) m_ivDegree = nvDegree;
	else                   m_ivDegree = m_nvLines-1;
	return m_ivDegree;
}



int NURBSSurface::SetuDegree(int nuDegree)
{
	if(FrameSize()>nuDegree) m_iuDegree = nuDegree;
	else                     m_iuDegree = FrameSize()-1;
	return m_iuDegree;
}


void NURBSSurface::RemoveFrame(int iFrame)
{
	delete m_pFrame.at(iFrame);
	m_pFrame.removeAt(iFrame);
}




void NURBSSurface::ClearFrames()
{
	if(!m_pFrame.size()) return;
	for(int ifr=m_pFrame.size()-1; ifr>=0; ifr--)
	{
		RemoveFrame(ifr);
	}
}


void NURBSSurface::InsertFrame(CFrame *pNewFrame)
{
	for(int ifr=0; ifr<m_pFrame.size(); ifr++)
	{
		if(pNewFrame->m_Position[m_uAxis] < m_pFrame.at(ifr)->m_Position[m_uAxis])
		{
			m_pFrame.insert(ifr, pNewFrame);
			return;
		}
	}

	m_pFrame.append(pNewFrame); //either the first if none, either the last...
}


CFrame * NURBSSurface::AppendFrame()
{
	m_pFrame.append(new CFrame);
	return m_pFrame.last();
}
