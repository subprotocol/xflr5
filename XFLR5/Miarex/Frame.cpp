/****************************************************************************

    CFrame Class
	Copyright (C) 2007 André Deperrois xflr5@yahoo.com

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

// Frame.cpp
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "Frame.h"
#include <math.h>

// CFrame
CRect CFrame::s_rViewRect;


CFrame::CFrame()
{
	memset(m_Point, 0, sizeof(m_Point));
	s_rViewRect.SetRectEmpty();
	m_NPoints = 5;
	m_Point[0].x = 0.0; m_Point[0].y = 0.00;   m_Point[0].z =  0.025;
	m_Point[1].x = 0.0; m_Point[1].y = 0.015;  m_Point[1].z =  0.020;
	m_Point[2].x = 0.0; m_Point[2].y = 0.020;  m_Point[2].z =  0.00;
	m_Point[3].x = 0.0; m_Point[3].y = 0.015;  m_Point[3].z = -0.020;
	m_Point[4].x = 0.0; m_Point[4].y = 0.00;   m_Point[4].z = -0.025;

	m_NPoints = 5;
	m_iHighlight = -1;
	m_iSelect    =  0;
}

CFrame::~CFrame()
{
}


int CFrame::IsPoint(CVector Point, double ZoomFactor)
{
	for(int l=0; l<m_NPoints; l++)
	{
		if (abs(Point.x-m_Point[l].y)<0.005/ZoomFactor &&
			abs(Point.y-m_Point[l].z)<0.005/ZoomFactor) 
			return l;
	}
	return -10;
}

bool CFrame::SerializeFrame(CArchive &ar)
{
	int ArchiveFormat;
	int k;
	float f;

	if(ar.IsStoring())
	{
		ar << 1000;
		//1000 : first format
		ar << m_NPoints;
		for(k=0; k<m_NPoints; k++)
		{
			ar << (float)m_Point[k].x << (float)m_Point[k].y << (float)m_Point[k].z;
		}
	}
	else {
		try{			
			ar >> ArchiveFormat;
			if(ArchiveFormat<1000 || ArchiveFormat>1100) return false;
			ar >> m_NPoints;
			for(k=0; k<m_NPoints; k++)
			{
				ar >> f; m_Point[k].x = f;
				ar >> f; m_Point[k].y = f;
				ar >> f; m_Point[k].z = f;
			}
		}
		catch (CArchiveException *ex)
		{
			ex->Delete();
			return false;
		}
	}
	return true;
}

void CFrame::RemoveSelectedPoint()
{
	int k;
	for (k=m_iSelect; k<m_NPoints; k++)
	{
		m_Point[k] = m_Point[k+1];
	}
	m_NPoints--;
	m_Point[m_NPoints].Set(0.0,0.0,0.0);
}

void CFrame::RemovePoint(int n)
{
	int k;
	for (k=n; k<m_NPoints; k++)
	{
		m_Point[k] = m_Point[k+1];
	}
	m_NPoints--;
	m_Point[m_NPoints].Set(0.0,0.0,0.0);
}

void CFrame::InsertPoint(int n)
{
	int k;
	if(n==0) n++;
	for (k=m_NPoints; k>n; k--)
	{
		m_Point[k] = m_Point[k-1];
	}
	m_Point[n] = (m_Point[n+1] + m_Point[n-1])/2.0;
	m_NPoints++;
	m_iSelect = n;
}

int CFrame::InsertPoint(CVector Real)
{
	int k, l;

	for (k=0; k<m_NPoints; k++)
	{
		if(Real.y>m_Point[k].z)
		{
			for (l=m_NPoints;l>k;l--)
			{
				m_Point[l] = m_Point[l-1];
			}
			m_Point[k].y = Real.x;
			m_Point[k].z = Real.y;
			if(k==0 || k==m_NPoints-1) m_Point[k].y =0.0;
			m_NPoints++;
			m_iSelect = k;
			return k;
		}
	}

	m_Point[m_NPoints].y = 0;
	m_Point[m_NPoints].z = Real.y;
	m_iSelect = m_NPoints;
	m_NPoints++;
	return m_iSelect;
}

void CFrame::CopyFrame(CFrame *pFrame)
{
	m_NPoints = pFrame->m_NPoints;
	memcpy(m_Point, pFrame->m_Point, sizeof(m_Point));
}






