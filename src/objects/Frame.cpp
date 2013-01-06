/****************************************************************************

    CFrame Class
	Copyright (C) 2007-2012 Andre Deperrois adeperrois@xflr5.com

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

#include "Frame.h"
#include <math.h>
#include "../params.h"



CFrame::CFrame(int nCtrlPts)
{
	m_Position.Set(0.0,0.0,0.0);
	m_CtrlPoint.clear();
	for(int ic=0; ic<nCtrlPts; ic++)
	{
		m_CtrlPoint.append(CVector(0.0,0.0,0.0));
	}
	m_iHighlight = -1;
	m_iSelect    =  0;
}


CFrame::~CFrame()
{
}

int CFrame::IsPoint(const CVector &Point, const double &ZoomFactor)
{
    int l;
	for(l=0; l<m_CtrlPoint.size(); l++)
	{
		if(sqrt(  (Point.x-m_CtrlPoint[l].x)*(Point.x-m_CtrlPoint[l].x)
				+ (Point.y-m_CtrlPoint[l].y)*(Point.y-m_CtrlPoint[l].y)
				+ (Point.z-m_CtrlPoint[l].z)*(Point.z-m_CtrlPoint[l].z))<0.05*Height()/ZoomFactor)
			  return l;
//        if (fabs(Point.x-m_CtrlPoint[l].y)<0.005/ZoomFactor && fabs(Point.y-m_CtrlPoint[l].z)<0.005/ZoomFactor) return l;
	}
	return -10;
}


bool CFrame::SerializeFrame(QDataStream &ar, bool bIsStoring)
{
	int ArchiveFormat;
	int k,n;
	float fx, fy, fz;

    if(bIsStoring)
	{
		ar << 1000;
		//1000 : first format
		ar << m_CtrlPoint.size();
		for(k=0; k<m_CtrlPoint.size(); k++)
		{
			ar << (float)m_CtrlPoint[k].x << (float)m_CtrlPoint[k].y << (float)m_CtrlPoint[k].z;
		}
	}
    else
    {
        ar >> ArchiveFormat;
        if(ArchiveFormat<1000 || ArchiveFormat>1100) return false;
		ar >> n;
		m_CtrlPoint.clear();
		for(k=0; k<n; k++)
        {
			ar >> fx;
			ar >> fy;
			ar >> fz;
			m_CtrlPoint.append(CVector((double)fx, (double)fy, (double)fz));
		}
	}
	return true;
}


bool CFrame::RemovePoint(int n)
{
	if (n>=0 && n<m_CtrlPoint.size())
	{
		m_CtrlPoint.removeAt(n);
		return true;
	}
	return false;
}


void CFrame::InsertPoint(int n)
{
	m_CtrlPoint.insert(n, CVector(0.0,0.0,0.0));
	if(n>0 && n<m_CtrlPoint.size()-1)
	{
		m_CtrlPoint[n] = (m_CtrlPoint[n+1] + m_CtrlPoint[n-1])/2.0;
	}
	else if(n==m_CtrlPoint.size()-1)
	{
		m_CtrlPoint[n] = m_CtrlPoint[n-1] + (m_CtrlPoint[n-1] - m_CtrlPoint.first())/5.0;
	}
	m_iSelect = n;
}


void CFrame::InsertPoint(int n, CVector const& Pt)
{
	m_CtrlPoint.insert(n, Pt);
	m_iSelect = n;
}


int CFrame::InsertPoint(const CVector &Real, int iAxis)
{
	int k=0;
	if(iAxis==1)
	{
		if(Real.x>m_CtrlPoint.first().x)
		{
			for(k=0; k<m_CtrlPoint.size()-1; k++)
			{
				if(m_CtrlPoint[k].x<=Real.x && Real.x <m_CtrlPoint[k+1].x)
				{
					break;
				}
			}
		}
		else k=-1;
	}

	else if(iAxis==2)
	{
		if(Real.y>m_CtrlPoint.first().y)
		{
			for(k=0; k<m_CtrlPoint.size()-1; k++)
			{
				if(m_CtrlPoint[k].y<=Real.y && Real.y <m_CtrlPoint[k+1].y)
				{
					break;
				}
			}
		}
		else k=-1;
	}
	else if(iAxis==3)
	{
		if(Real.z>m_CtrlPoint.last().z)
		{
			for(k=0; k<m_CtrlPoint.size()-1; k++)
			{
				if(m_CtrlPoint[k].z>=Real.z && Real.z >m_CtrlPoint[k+1].z)
				{
					break;
				}
			}
		}
		else k=-1;
	}

	m_CtrlPoint.insert(k+1, Real);
	m_iSelect = k+1;
	return k+1;
}



double CFrame::Height()
{
	return (m_CtrlPoint.last() - m_CtrlPoint.first()).VAbs();
/*	double hmin	=  10.0;
	double hmax = -10.0;
	for(int k=0; k<m_CtrlPoint.size(); k++)
	{
		if(m_CtrlPoint[k].z<hmin) hmin = m_CtrlPoint[k].z;
		if(m_CtrlPoint[k].z>hmax) hmax = m_CtrlPoint[k].z;
	}
	return fabs(hmax-hmin);*/
}


double CFrame::zPos()
{
	double hmin	=  10.0;
	double hmax = -10.0;
	for(int k=0; k<m_CtrlPoint.size(); k++)
	{
		if(m_CtrlPoint[k].z<hmin) hmin = m_CtrlPoint[k].z;
		if(m_CtrlPoint[k].z>hmax) hmax = m_CtrlPoint[k].z;
	}
	return (hmax+hmin)/2.0;
}


void CFrame::CopyFrame(CFrame *pFrame)
{
	m_Position = pFrame->m_Position;
	CopyPoints(&pFrame->m_CtrlPoint);
}


void CFrame::CopyPoints(QList<CVector> *m_pPointList)
{
	m_CtrlPoint.clear();
	for(int ip=0; ip<m_pPointList->size(); ip++)
	{
		m_CtrlPoint.append(m_pPointList->at(ip));
	}
}


void CFrame::AppendPoint(CVector const& Pt)
{
	m_CtrlPoint.append(Pt);
}


void CFrame::SetPosition(CVector Pos)
{
	m_Position = Pos;
}

void CFrame::SetuPosition(double u)
{
	m_Position.x = u;
}

void CFrame::SetvPosition(double v)
{
	m_Position.y = v;
}

void CFrame::SetwPosition(double w)
{
	m_Position.z = w;
}



void CFrame::RotateFrameY(double Angle)
{
	if(!m_CtrlPoint.size()) return;

//	CVector RotationCenter = m_CtrlPoint.first();
	for(int ic=0; ic<m_CtrlPoint.size(); ic++)
	{
		m_CtrlPoint[ic].RotateY(m_Position, Angle);
	}
}









