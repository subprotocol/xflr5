/****************************************************************************

    CBody Class
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

#pragma once

#include "Panel.h"
#include "Frame.h"


class CBody : public CObject
{
public:
	CBody();
	virtual ~CBody();

	CFrame* GetFrame(int iSelect);
	
	bool Gauss(double *A, int n, double *B, int m);
	bool IsInNURBSBody(CVector Pt);
	bool Intersect(CVector A, CVector B, CVector &I, bool bRight);
	bool IntersectPanels(CVector A, CVector B, CVector &I, bool bRight);
	bool IntersectNURBS(CVector A, CVector B, CVector &I, bool bRight);
	bool SerializeBody(CArchive &ar);
	bool SetModified();
	bool ImportDefinition() ;
	bool ExportDefinition() ;


	int InsertFrame(CVector Real);
	int InsertPoint(CVector Real);
	int IsFramePos(CVector Real, double ZoomFactor);
	int RemoveFrame(int n);
	int ReadFrame(CStdioFile *pXFile, int &Line, CFrame *pFrame, double const &Unit);

	double GetLength();
	double Getu(double x);
	double Getv(double u, CVector r, bool bRight);
	double SplineBlend(int const &index, int const &p, double const &t, double *knots);

	void ComputeAero(double *Cp, double &XCP, double &YCP,
		             double &GCm, double &GRm, double &GYm, double &Alpha, double &XCmRef, bool bTilted);
	void ComputeCenterLine();
	void Duplicate(CBody *pBody);
	void ExportGeometry(int nx, int nh);
	void GetPoint(double u, double v, bool bRight, CVector &Pt);
	void InsertSideLine(int SideLine);
	void InterpolateCurve(CVector *D, CVector *P, double *v, double *knots, int degree, int Size);
	void InterpolateSurface();
	void RemoveActiveFrame();
	void RemoveSideLine(int SideLine);
	void Scale(double XFactor, double YFactor, double ZFactor, bool bFrameOnly, int FrameID);
	void Translate(double XTrans, double YTrans, double ZTrans, bool bFrameOnly, int FrameID);
	void SetKnots();
	void SetPanelPos();
	void UpdateFramePos(int iFrame);


//____________________VARIABLES_____________________________________________


	bool m_bLocked; //true is this body is used by a plane with results
	bool m_bClosedSurface;

	int m_NSideLines;
	int m_NStations;			// the number of stations along x-axis where frames are defined
	int m_iActiveFrame;		// the currently selected frame for display
	int m_iHighlight;		// the currently selected frame for display
	int m_LineType; //1=lines  2=B-Splines
	int m_iRes; //for splines
	int m_NElements;// = m_nxPanels * m_nhPanels *2
	int m_nxPanels, m_nhPanels;
	int m_nxDegree, m_nhDegree, m_nxKnots, m_nhKnots;
	int m_BodyStyle, m_BodyWidth;

	double m_Bunch;
	double pi;

	int m_np;
	double  m_x[IBX], m_y[IBX];	// the point coordinates of the overlayed body

	CString m_BodyName;

	COLORREF m_BodyColor;

	CVector m_FramePosition[MAXBODYFRAMES];
	CFrame m_Frame[MAXBODYFRAMES];	// the frames at the stations

	int m_xPanels[MAXBODYFRAMES];
	int m_hPanels[MAXSIDELINES];

	CPanel *m_pPanel;

	//allocate temporary variables to
	//avoid lengthy memory allocation times on the stack
	double value, eps, bs, cs;
	CVector t_R, t_Prod, t_Q, t_r, t_N;
//	CVector P0, P1, P2, PI;
	static double s_xKnots[MAXBODYFRAMES*2];
	static double s_hKnots[MAXSIDELINES*2];
	static double s_XPanelPos[300];
	static	CRect s_rViewRect;
	static CWnd *s_pMainFrame;
};


