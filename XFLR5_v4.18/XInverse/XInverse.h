/****************************************************************************

	CXInverse class
	Copyright (C) 2004 André Deperrois xflr5@yahoo.com

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

// XInverse.h : header file
//
#include "../Graph/Graph.h"
#include "../XDirect/XFoil.h"
#include "../XDirect/Foil.h"
#include "../Design/Spline.h"
#include "FInvCtrlBar.h"
#include "MInvCtrlBar.h"
#include "XInvClrDlg.h"
#include "atlimage.h"

/////////////////////////////////////////////////////////////////////////////
// CXInverse window

class CXInverse : public CWnd
{
	friend class CFInvCtrlBar;
	friend class CMInvCtrlBar;
	friend class CChildView;
	friend class CMainFrame;
// Construction
public:
	CXInverse();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXInverse)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void CheckMenu();
	virtual ~CXInverse();
private:
	bool m_bLoaded;
	bool m_bTrans;
	bool m_bModFoil, m_bRefFoil;
	bool m_bGetPos;
	bool m_bMark, m_bMarked;
	bool m_bSpline, m_bSplined;
	bool m_bSmooth;
	bool m_bZoomPlus;
	bool m_bFullInverse;// mixed inverse if false
	bool m_bSaved;
	bool m_bReflected;
	bool m_bShowPoints;
	bool m_bTransGraph;
	bool m_bRefCurves;
	bool m_bTangentSpline;


	int m_Mk1, m_Mk2;
	int m_tmpPos, m_Pos1, m_Pos2, m_nPos, m_SplineLeftPos, m_SplineRightPos;
	int m_LogPixelsY;
	int m_SplineStyle, m_SplineWidth;
	double xd, yd;
	double xu, yu;
	double m_fScale, m_fRefScale;

	int m_ReflectedStyle, m_ReflectedWidth;
	COLORREF m_ReflectedClr;


	CWnd * m_pChildWnd;
	CWnd * m_pFrame;
	
	CToolBar *m_pXInverseBar;
	CFInvCtrlBar * m_pFInvCtrlBar;
	CMInvCtrlBar * m_pMInvCtrlBar;
	XFoil * m_pXFoil;

	Graph m_QGraph;
	CCurve* m_pQCurve;
	CCurve* m_pQVCurve;
	CCurve* m_pMCurve;
	CCurve* m_pReflectedCurve;

	HCURSOR m_hcArrow;
	HCURSOR m_hcCross;
	HCURSOR m_hcMove;

	COLORREF m_SplineClr;

	CRect m_rCltRect, m_rGraphRect;
	CRect m_ZoomRect;

	CPoint m_PointDown;
	CPoint m_ptPopUp;
	CPoint m_ptOffset;


	CFoil* m_pRefFoil;
	CFoil* m_pModFoil;

	CSpline m_Spline;

// temporary allocations
	CPoint tanpt;
	CPoint P0, P1, P2;


	bool ExecQDES();
	bool PrintAll(CDC *pDC, CRect DrawRect);

	double qincom(double qc, double qinf, double tklam);
//	double qcomp(double g);

	void ApplySpline();
	void CreateMCurve();
	void CreateQCurve();
	void Clear();
	void DrawGrid(CDC *pDC, CRect* pCltRect, double scale, CPoint ptOffset, bool bIsPrinting);
	void Filter(double filt);
	void PaintGraph(CDC *pDC);
	void PaintFoil(CDC *pDC);
	void PaintImage(ATL::CImage *pImage, CString &FileName, int FileType);
	void ResetMixedQ();
	void Pertubate();
	void ResetQ();
	void ResetScale();
	void ReleaseZoom();
	void SetColors(Graph *pDefaultGraph);
	void SetFoil();
	void SetRect(CRect CltRect);
	void SetTGap(double tr, double ti);
	void SetTAngle(double a);
	void Smooth(int Pos1 = -1, int Pos2 = -1);
	void ExecMDES();
	bool SetParams();
	void UpdateView(CDC *pDC = NULL);

	void OnContextMenu(CPoint ScreenPoint, CPoint ClientPoint);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	// Generated message map functions
protected:
	void SetLogPixels(int LPY);
	void SaveSettings(CArchive &ar);
	bool InitXFoil(CFoil * pFoil);
	bool LoadSettings(CArchive &ar);
	bool SetXInverseCursor(CWnd* pWnd, CPoint ptMouse, UINT message) ;

	//{{AFX_MSG(CXInverse)
	afx_msg void OnZoomIn();
	afx_msg void OnZoomLess();
	afx_msg void OnGraphOptions();
	afx_msg void OnResetGraph();
	afx_msg void OnShowControls();
	afx_msg void OnPrint();
	afx_msg void OnQInit();
	afx_msg void OnQMod();
	afx_msg void OnShowPoints();
	afx_msg void OnExportFoil();
	afx_msg void OnExportGraphToFile();
	afx_msg void OnQVisc();
	afx_msg void OnColorStyles();
	afx_msg void OnAddPoint();
	afx_msg void OnDeletePoint();
	afx_msg void OnShowRef();
	afx_msg void OnShowMod();
	afx_msg void OnLoadFoil();
	afx_msg void OnRestore();
	afx_msg void OnStoreFoil();
	afx_msg void OnAbout();
	afx_msg void OnSelectFoil();
	afx_msg void OnResetFoil();
	afx_msg void OnLoad();
	afx_msg void OnResetRefCurves();
	afx_msg void OnShowRefCurves();
	afx_msg void OnStoreRefCurve();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowReflected();
};

/////////////////////////////////////////////////////////////////////////////

