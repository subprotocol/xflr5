/****************************************************************************

    CRectangle Class
	Copyright (C) 2008 Andre Deperrois XFLR5@yahoo.com

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

//////////////////////////////////////////////////////////////////////
//
// Re-writing of the MFC CRect class
//
//////////////////////////////////////////////////////////////////////
#include <math.h>

#include "./CRectangle.h"

CRectangle::CRectangle(void)
{
        left   = 0;
        right  = 0;
        top    = 0;
        bottom = 0;
}

CRectangle::~CRectangle(void)
{
}


CRectangle::CRectangle(double l,double t,double r,double b)
{
		left   = l;
		right  = r;
		top    = t;
        bottom = b;
}


CRectangle::CRectangle(CVector TopLeft, CVector BottomRight)
{
		left   = TopLeft.x;
		right  = BottomRight.x;
		top    = TopLeft.y;
		bottom = BottomRight.y;
}

CRectangle::CRectangle(CRectangle const &Rect)
{
	left   = Rect.left;
	right  = Rect.right;
	top    = Rect.top;
	bottom = Rect.bottom;
}

void CRectangle::CopyRect(CRectangle *pRect)
{
	//InflateRect subtracts units from the left and top and adds units to the right and bottom
	left   = pRect->left;
	right  = pRect->right;
	top    = pRect->top;
	bottom = pRect->bottom;
}



void CRectangle::DeflateRect(double x, double y)
{
	//DeflateRect adds units to the left and top and subtracts units from the right and bottom
	left   +=x;
	right  -=x;
	top    +=y;
	bottom -=y;
}

void CRectangle::DeflateRect(double l, double t, double r, double b)
{
	//DeflateRect adds units to the left and top and subtracts units from the right and bottom
	left   +=l;
	right  -=r;
	top    +=t;
	bottom -=b;
}

double CRectangle::height()
{
		return(top-bottom);
}

void CRectangle::InflateRect(double x, double y)
{
	//InflateRect subtracts units from the left and top and adds units to the right and bottom
	left   -=x;
	right  +=x;
	top    -=y;
	bottom +=y;
}

void CRectangle::InflateRect(double l, double t, double r, double b)
{
	//InflateRect subtracts units from the left and top and adds units to the right and bottom
	left   -=l;
	right  +=r;
	top    -=t;
	bottom +=b;
}

bool CRectangle::IsRectEmpty()
{
	if(bottom==top && right==left) return true;
	else                           return false;
}

void CRectangle::NormalizeRect()
{
	double tmp;
	if (left > right)
	{
		tmp = left;
		left = right;
		right = tmp;
	}
	if(bottom>top)
	{
		tmp = bottom;
		bottom = top;
		top = tmp;
	}
}




bool CRectangle::PtInRect(CVector pt)
{
    // Differs from the MFC class where, quote
    // "A point is within CRect if it lies on the left or top side or is within all four sides.
    // A point on the right or bottom side is outside CRect."

	if(left<pt.x && pt.x<right && bottom<pt.y && pt.y<top ) return true;
	return false;
}

void CRectangle::SetRect(double l, double t, double r, double b)
{
	left   =l;
	right  =r;
	top    =t;
	bottom =b;
}

void CRectangle::SetRect(CRectangle const &Rect)
{
	left   = Rect.left;
	right  = Rect.right;
	top    = Rect.top;
	bottom = Rect.bottom;
}


void CRectangle::SetRectEmpty()
{
	left   = 0;
	right  = 0;
	top    = 0;
	bottom = 0;
}

double CRectangle::width()
{
		return (right-left);
}
