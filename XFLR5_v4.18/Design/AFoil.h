/****************************************************************************

	CAFoil class
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

// AFoil.h : header file
//

#include "AFoilCtrlBar.h"
#include "../XDirect/Foil.h"
#include "../XDirect/XFoil.h"
#include "atlimage.h"

struct Picture{
	public:
		double xExt[SPLINECONTROLSIZE];
		double yExt[SPLINECONTROLSIZE];
		double xInt[SPLINECONTROLSIZE];
		double yInt[SPLINECONTROLSIZE];
		CVector ExtRearPt, IntRearPt;
		int m_iExt, m_iInt;
};

/////////////////////////////////////////////////////////////////////////////
// CAFoil window

class CAFoil : public CWnd
{
	friend class CAFoilCtrlBar;
	friend class CSplinesCtrlDlg;
	friend class CLECircleDlg;
	friend class CMainFrame;
	friend class CChildView;
	friend class CXFLR5App;
// Construction
public:
	CAFoil();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAFoil)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
private:
	bool m_bScale;
	bool m_bSF;//else m_bPF
	bool m_bZoomPlus;
	bool m_bZoomYOnly;
	bool m_bNeutralLine;
	bool m_bTrans;
	bool m_bCircle;
	bool m_bStored;
	bool m_bXGrid,m_bYGrid;
	bool m_bXMinGrid, m_bYMinGrid;
	bool m_bShowLegend;

	int m_XGridStyle, m_YGridStyle;
	int m_XGridWidth, m_YGridWidth;
	int m_XMinStyle, m_YMinStyle;
	int m_XMinWidth, m_YMinWidth;
	int m_NeutralStyle, m_NeutralWidth;
	int m_StackPos, m_StackSize;// undo : stack position and stack size

	double m_LERad;
	double m_fScale;
	double m_fScaleY;//ratio between y and x scales;
	double m_fRefScale;
	double m_XGridUnit, m_YGridUnit;
	double m_XMinorUnit, m_YMinorUnit;

	CObArray *m_poaFoil;
	XFoil *m_pXFoil;

	CSF *m_pSF;
	CPF *m_pPF;
	CPoint m_ptOffset;
	CPoint m_ptPopUp;
	CPoint m_PointDown;
	CRect m_rDrawRect;
	CRect m_rCltRect;
	CRect m_ZoomRect;
	CVector m_MousePos;
	CFoil *m_pBufferFoil;

	COLORREF m_XGridColor,m_YGridColor;
	COLORREF m_XMinColor,m_YMinColor;
	COLORREF m_NeutralColor;

	HCURSOR m_hcArrow;
	HCURSOR m_hcMove;
	HCURSOR m_hcCross;
//	HICON m_hIcon;

	Picture m_TmpPic;
	Picture m_UndoPic[50];

	CWnd * m_pFrame;//Mainframe
	CWnd* m_pChildWnd;//ChildView
	CToolBar *m_pAFoilBar;
	CAFoilCtrlBar* m_pACtrl;

	
	bool LoadSettings(CArchive& ar);
	bool PrintAll(CDC * pDC, CRect pRect);
	bool SetAFoilCursor(CWnd* pWnd, CPoint ptMouse, UINT message);
	int GetFoilCount();
	int ReadData(CStdioFile *pXFile, CFoil *pFoil);
	void DrawLegend(CDC *pDC, CPoint Place, bool bIsPrinting);
	void DrawScale(CDC *pDC, CRect* pCltRect, double scale, CPoint ptOffset, bool bIsPrinting);
	void DrawXGrid(CDC *pDC, double scalex, double scaley, CPoint Offset, CRect dRect, bool bIsPrinting = false);
	void DrawXMinGrid(CDC *pDC, double scalex, double scaley, CPoint Offset, CRect dRect, bool bIsPrinting = false);
	void DrawYGrid(CDC *pDC, double scalex, double scaley, CPoint Offset, CRect dRect, bool bIsPrinting = false);
	void DrawYMinGrid(CDC *pDC, double scalex, double scaley, CPoint Offset, CRect dRect, bool bIsPrinting = false);
	void PaintGrids(CDC* pDC); 
	void PaintFoils(CDC* pDC); 
	void PaintSplines(CDC* pDC); 
	void PaintLegend(CDC* pDC); 
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnContextMenu(CPoint ScreenPoint, CPoint ClientPoint);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnMButtonDown(UINT nFlags, CPoint point);
	void PaintImage(ATL::CImage *pImage, CString &FileName, int FileType);
	void PFNew();
	void PFSave();
	void ReleaseZoom();
	void SaveProject();
	void SaveSettings(CArchive& ar);
	void SetCurFoil(CFoil *pFoil);
	void SetFoils(CFoil *pCurFoil=NULL);
	void SetParams(CFoil *pFoil);
	void SetSaveState(bool bState);
	void SetScale();
	void SetScale(CRect CltRect);
	void SetPicture();
	void SFNew();
	void SFSave();
	void ShowFoil(CFoil* pFoil, bool bShow);
	void StorePicture();
	void TakePicture();
	void UpdateView(CDC* pDC = NULL);

	CFoil* GetFoil(CString FoilName);
	CVector MousetoReal(CPoint *point);

public:
	virtual ~CAFoil();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAFoil)
	afx_msg void OnSFSave();
//	afx_msg void OnLoad();
	afx_msg void OnExportSplines();
	afx_msg void OnNewSplines();
	afx_msg void OnPrint();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomLess();
	afx_msg void OnZoomOut();
	afx_msg void OnDeletePoint();
	afx_msg void OnAddPoint();
	afx_msg void OnUndo();
	afx_msg void OnRedo();
	afx_msg void OnGrid();
	afx_msg void OnStoreFoil();
	afx_msg void OnResetView();
	afx_msg void OnAbout();
	afx_msg void OnHideAllFoils();
	afx_msg void OnShowAllFoils();
	afx_msg void OnHideFoil();
	afx_msg void OnDeleteFoil();
	afx_msg void OnShowFoil();
	afx_msg void OnRenameFoil();
	afx_msg void OnZoomYOnly();
	afx_msg void OnResetYScale();
	afx_msg void OnNormalize();
	afx_msg void OnDeRotate();
	afx_msg void OnLERad();
	afx_msg void OnTEGap();
	afx_msg void OnGeom();
	afx_msg void OnPanel();
	afx_msg void OnCadd();
	afx_msg void OnInterpolate();
	afx_msg void OnDuplicate();
	afx_msg void OnExportFoil();
	afx_msg void OnEditCoord();
	afx_msg void OnSetFlap();
	afx_msg void OnNacaFoils();
	afx_msg void OnLECircle();
	afx_msg void OnSplineControls();
	afx_msg void OnShowLegend();
	afx_msg void OnBSplines();
	afx_msg void OnSplinedpoints();
	afx_msg void OnEditCtrlPoints();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

