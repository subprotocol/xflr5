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
 
#ifndef FRAME_H
#define FRAME_H

#include "CVector.h"
#include <QDataStream>
#include <QList>

class Frame
{
public:
	Frame(int nCtrlPts=0);
	~Frame();

	void   AppendPoint(CVector const& Pt);
	void   CopyFrame(Frame *pFrame);
	void   CopyPoints(QList<CVector> *m_pPointList);
	double Height();
	int    IsPoint(CVector const &Point, double const &ZoomFactor);
	void   InsertPoint(int n);
	void   InsertPoint(int n, const CVector &Pt);
	int    InsertPoint(CVector const &Real, int iAxis);
	int    PointCount() {return m_CtrlPoint.size();}
	bool   RemovePoint(int n);
	void   RotateFrameY(double Angle);
	bool   SerializeFrame(QDataStream &ar, bool bIsStoring);
	void   SetPosition(CVector Pos);
	void   SetuPosition(double u);
	void   SetvPosition(double v);
	void   SetwPosition(double w);
	double zPos();


	QList <CVector> m_CtrlPoint;	// the point's positions
	int m_iHighlight, m_iSelect;
	CVector m_Position;
};


#endif

