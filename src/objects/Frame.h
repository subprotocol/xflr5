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
 
#ifndef CFRAME_H
#define CFRAME_H

#include "CVector.h"
#include <QDataStream>
#include <QList>

class CFrame
{
public:
	CFrame(int nCtrlPts=0);
	~CFrame();
    bool SerializeFrame(QDataStream &ar, bool bIsStoring);

	int IsPoint(CVector const &Point, double const &ZoomFactor);
	void AppendPoint(CVector const& Pt);
	void InsertPoint(int n);
	void InsertPoint(int n, const CVector &Pt);
	int InsertPoint(CVector const &Real, int iAxis);
	bool RemovePoint(int n);

	void CopyFrame(CFrame *pFrame);
	void CopyPoints(QList<CVector> *m_pPointList);

	void SetPosition(CVector Pos);
	void SetuPosition(double u);
	void SetvPosition(double v);
	void SetwPosition(double w);

	void RotateFrameY(double Angle);

	int PointSize() {return m_CtrlPoint.size();};

	double Height();
	double zPos();

	int m_iHighlight, m_iSelect;

	CVector m_Position;
	QList <CVector> m_CtrlPoint;	// the point's positions
};


#endif

