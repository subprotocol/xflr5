/****************************************************************************

    CBody Class
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

#pragma once

#include "Panel.h"
#include "Frame.h"


class CBody : public CObject
{
public:
	CBody();
	virtual ~CBody();

	CFrame* GetFrame(int iSelect);
	CFrame * InsertFrame(CVector Real);
	bool SerializeBody(CArchive &ar);
	bool SetModified();
	int IsFramePos(CVector Real, double ZoomFactor);
	int RemoveFrame(int n);
	void Export();
	void RemoveActiveFrame();
	void ComputeCenterLine();
	void UpdateFramePos(int iFrame);
	void InsertPoint(CVector Real);
	void InsertSideLine(int SideLine);
	void RemoveSideLine(int SideLine);
	void Duplicate(CBody *pBody);
	void Scale(double XFactor, double YFactor, double ZFactor);
	void SetKnots();
	double GetLength();
	double Getu(double x);
	double SplineBlend(int index,  int p, double t, double *knots);
	CVector GetPoint(double u, double v);

	bool m_bLocked; //true is this body is used by a plane with results
	bool m_bClosedSurface;
	int m_NSideLines;
	int m_NStation;			// the number of stations along x-axis where frames are defined
	int m_iActiveFrame;		// the currently selected frame for display
	int m_iHighlight;		// the currently selected frame for display
	int m_LineType; //1=lines  2=B-Splines
	int m_iRes; //for splines
	int m_NElements;// = m_nxPanels * m_nhPanels
	int m_nxPanels, m_nhPanels;

	CString m_BodyName;

	COLORREF m_BodyColor;
	int m_BodyStyle, m_BodyWidth;

	CVector m_FramePosition[MAXBODYFRAMES];
	CFrame m_Frame[MAXBODYFRAMES];	// the frames at the stations

	CPanel *m_pPanel;

	int m_nxDegree, m_nhDegree, m_nxKnots, m_nhKnots;

	//avoid lengthy memory allocation times
	double value, eps,bs, cs;

	static double s_xKnots[MAXBODYFRAMES];
	static double s_hKnots[30];
	static	CRect s_rViewRect;
	static CWnd *s_pMainFrame;
};


