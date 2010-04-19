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

#ifndef CRECTANGLE_H
#define CRECTANGLE_H

#include "CVector.h"

class CRectangle
{
public:
    CRectangle(void);
    ~CRectangle(void);
    CRectangle(double l,double t,double r,double b);
    CRectangle(CVector TopLeft, CVector BottomRight);
	CRectangle(CRectangle const &Rect);

    double left;
    double top;
    double right;
    double bottom;

    bool IsRectEmpty();
    bool PtInRect(CVector pt);
    double width();
    double height();
    void SetRectEmpty();
    void DeflateRect(double x, double y);
    void DeflateRect(double l, double t, double r, double b);
    void InflateRect(double x, double y);
	void InflateRect(double l, double t, double r, double b);
	void SetRect(double l, double t, double r, double b);
	void SetRect(CRectangle const &Rect);
	void NormalizeRect();
    void CopyRect(CRectangle *pRect);
};
#endif
