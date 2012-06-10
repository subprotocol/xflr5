/****************************************************************************

		 SplineSurface Class
		 Copyright (C) 2012 Andre Deperrois sail7@xflr5.com
		 All rights reserved

*****************************************************************************/


#ifndef SPLINESURFACE_H
#define SPLINESURFACE_H


//#include "../params.h"
#include "Frame.h"

#define MAXVLINES      17
#define MAXULINES 19

class NURBSSurface
{

public:
	NURBSSurface(int iAxis=0);
	void SetKnots();
	double Getu(double pos, double v);
	double Getv(double u, CVector r);
	void GetPoint(double u, double v, CVector &Pt);
	void SetPanelPos();
	bool IntersectNURBS(CVector A, CVector B, CVector &I);
	int SetvDegree(int nvDegree);
	int SetuDegree(int nuDegree);

	void ClearFrames();
	void RemoveFrame(int iFrame);
	void InsertFrame(CFrame *pNewFrame);
	CFrame *AppendFrame();

	double Weight(const double &d, int const &i, int const &N);

	int FrameSize() {return m_pFrame.size();};


	static void* s_pMainFrame;

//	int m_nuLines;			// the number of stations along x-axis where frames are defined
	int m_nvLines;          // the number of control points in each frame
	int m_iuDegree, m_ivDegree, m_nuKnots, m_nvKnots;

	double m_uKnots[MAXVLINES*2];
	double m_vKnots[MAXULINES*2];

	int m_uPanels[MAXVLINES];
	int m_vPanels[MAXULINES];

	int m_iRes;
	int m_NElements;// = m_nxPanels * m_nhPanels *2
	int m_nuPanels, m_nvPanels;
	int m_np;

	double m_Bunch;
	double m_EdgeWeightu, m_EdgeWeightv; // for a full NURBS. Unused, though, not practical

	QList<CFrame*> m_pFrame;	// the frames at the stations

	//allocate temporary variables to
	//avoid lengthy memory allocation times on the stack
	double value, eps, bs, cs;
	CVector t_R, t_Prod, t_Q, t_r, t_N;
//	CVector P0, P1, P2, PI;
	double m_vPanelPos[300];

	int m_uAxis, m_vAxis;
};

#endif // SPLINESURFACE_H
