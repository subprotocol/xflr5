/****************************************************************************

    CXDirect Class
	Copyright (C) 2003 André Deperrois xflr5@yahoo.com

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

// XDirect/XDirect.h : header file
//

#include "../Graph/Graph.h"
#include "../misc/LinePickerDlg.h"
#include "../misc/UFOListDlg.h"
#include "./OpPoint.h"
#include "./Polar.h"
#include "./FoilAnalysisDlg.h"
#include "./ViscDlg.h"
#include "./Foil.h"
#include "atlimage.h"

/////////////////////////////////////////////////////////////////////////////
// CXDirect window

class CXDirect : public CWnd
{ 
	friend class CXFLR5App;
	friend class CMainFrame; 
	friend class CChildView;
	friend class CEditPlrDlg;
	friend class CInterpolateDlg;
	friend class CCAddDlg;
	friend class C2DPanelDlg;
	friend class CGeomDlg;
	friend class CRenameFoilDlg;
	friend class CGraphOptions;
	friend class CGraphDlg;
	friend class CCurveDlgBar;
	friend class COperDlgBar;
	friend class CPolar;
	friend class CViscDlg;
	friend class CBatchDlg;
	friend class CBatchThread;
	friend class CFoilAnalysisDlg;
	friend class CFoilSettingsDlg;
	friend class CFlapDlg;
	friend class CTEGapDlg;
	friend class CLEDlg;
	friend class CUFOListDlg;
	friend class OpPoint;


// Construction
public:
	CXDirect(CWnd* pWnd = NULL);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXDirect)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual ~CXDirect();

private:
	bool m_bAlpha;			//true if performing an analysis based on aoa, false if based on Cl
	bool m_bStoreOpp;		// true if operating points should be stored after an analysis
	bool m_bViscous;		// true if performing a viscous calculation, false if inviscid
	bool m_bInitBL;			// true if the boundary layer should be initialized for the next xfoil calculation
	bool m_bBL;				// true if the Boundary layer shoud be displayed
	bool m_bPressure;		// true if the pressure distirbution should be displayed
	bool m_bPolar;			// true if the polar view is selected, false if the operating point view is selected
	bool m_bShowUserGraph;	// true if the 5th polar graph should be displayed
	bool m_bAnimate;		// true if a result animation is underway
	bool m_bAnimatePlus;	// true if the animation is going from lower to higher alpha, false if decreasing
	bool m_bShowPanels;		// true if the panels should be displayed on the foil surface
	bool m_bType1, m_bType2, m_bType3, m_bType4; // filter for polar diplay
	bool m_bAutoInitBL;		// true if the BL initialization is left to the code's decision
	bool m_bTrans;			// true if the user is dragging a view
	bool m_bTransGraph;		// true if the user is dragging a graph
	bool m_bFromList;		// true if the batch analysis is based on a list of Re values
	bool m_bFromZero;		// true if the batch analysis should start from Alpha=0
	bool m_bShowTextOutput;	// true if the batch analysis should display text result output
	bool m_bNeutralLine;	// true if the neutral line should be displayed
	bool m_bCurOppOnly;		// true if only the current operating point should be displayed
	bool m_bShowInviscid;	// true if the inviscid results should be displayed
	bool m_bCpGraph;		// true if the Cp graph should be displayed

	int m_posAnimate;		// the current aoa in the animation
	int m_OppVar;			// defined the variable to be displayed in the Cp graph
	int m_IterLim;			// max iteratio limit for XFoil
	int m_FoilYPos;			// y position for the foil display
	int m_XPolar, m_YPolar;	// variables for the first polar graph
	int m_XCz, m_YCz;		// variables for the second polar graph
	int m_XCm, m_YCm;		// variables for the third polar graph
	int m_XTr, m_YTr;		// variables for the fourth polar graph
	int m_XUser, m_YUser;	// variables for the fifth polar graph
	int m_iPlrGraph;		// defines whch polar graph is selected if m_iPlrView=1
	int m_iPlrView;			// 0 is all, 1 is single, 2 is two !

	double m_fFoilScale;	// foil display scale
	double m_ReMax, m_ReInc;// for batch analysis
	double m_ReList[30];	// for batch analysis
	double m_MachList[30];	// for batch analysis
	double m_NCritList[30];	// for batch analysis
	int m_NRe;				// number of Re values in the ReList


	CFoil m_BufferFoil;		// used for screen drawing and temporary geometric mods
	CFoil *m_pCurFoil;		// pointer to the currently selected foil
	CPolar* m_pCurPolar;	// pointer to the currently selected foil polar
	OpPoint * m_pCurOpp;	// pointer to the currently selected foil operating point

	CObArray *m_poaFoil;	// pointer to the foil object array
	CObArray *m_poaPolar;	// pointer to the polar object array
	CObArray *m_poaOpp;		// pointer to the OpPoint object array

	Graph* m_pCpGraph;		//pointers to the various graphs
	Graph* m_pPolarGraph;
	Graph* m_pCmGraph;
	Graph* m_pCzGraph;
	Graph* m_pTrGraph;
	Graph* m_pUserGraph;
	Graph* m_pCurGraph;

	CStdioFile m_XFile;		//output file for the analysis

	HCURSOR m_hcMove;
	HCURSOR m_hcCross;
	HCURSOR m_hcArrow;

	CWnd * m_pChildWnd;		// pointer to the view window
	CWnd * m_pFrame;		// pointer to the frame window

	CMenu m_XFLR5Menu;	

	COLORREF m_crFoilColor, m_crBLColor, m_crPressureColor, m_crNeutralColor; //foil display parameters
	int m_iFoilStyle, m_iFoilWidth;
	int m_iBLStyle, m_iBLWidth;
	int m_iPressureStyle, m_iPressureWidth;
	int m_iNeutralStyle, m_iNeutralWidth;

	CRect m_rCltRect;		// the client area
	CRect m_rDrawRect;		// the drawing rectangle
	CRect m_rSingleRect;	// for the polar single graph display
	CPoint m_PointDown;		// the client point for the last mouse left-click
	CPoint m_ptOffset;		// the screen offset position for the foil display

	CFoilAnalysisDlg m_Adlg;	// the polar definition dialog box
	CUFOListDlg m_UFOdlg;		// the foil management dialog box
	CViscDlg m_ViscDlg;		// the dialog class which manages the xfoil calculation and display output

	XFoil *m_pXFoil;		// a pointer to the XFoil object

	double pi;		// no idea 

//_____________________________METHODS____________________
	bool LoadSettings(CArchive &ar);
	bool SetXFLR5Cursor(CWnd* pWnd, CPoint ptMouse, UINT message) ;
	bool InitXFoil(CFoil * pFoil = NULL);
	CFoil* SetFoil(CFoil* pFoil = NULL);
	CFoil* SetFoil(CString FoilName);

	void Analysis1(double Alpha, double AlphaMax, double DeltaAlpha, bool bSequence);
	void AnalysisAlpha(double AlphaMin, double AlphaMax, double DeltaAlpha, bool bSequence);
	void AnalysisCl(double ClMin, double ClMax, double DeltaCl, bool bSequence);
	void AnalysisRe(double ReMin, double ReMax, double DeltaRe, bool bSequence);
	void Animate(bool bAnimate);

	void CheckMenu();
	void CreatePolarCurves();
	void CreateOppCurves(OpPoint* pOpp = NULL);
	void DeleteFoil(bool bAsk = false);
	void * GetVariable(CPolar* pPolar, int iVar);
	void InitXFoil2();
	void RemoveOpPoint(bool bCurrent);
	void SetPolar(CPolar *pPolar = NULL);
	void SetPolar(CString PlrName);
	void SetOpp(double Alpha);
	void AddOpData(OpPoint* pOpPoint);
	void DiscardOpPoints();
	void DrawCpv(CDC* pDC, OpPoint* pOpPoint, double scale, int offx, int offy, bool bIsPrinting);
	void DrawBL(CDC* pDC, OpPoint* pOpPoint, double scale, int offx, int offy, bool bIsPrinting);
	void DrawFoil(CDC *pDC, double alpha, double scale, int offx, int offy, bool bIsPrinting);
	void DrawPolarLegend(CDC* pDC, bool bIsPrinting, CPoint place, int bottom);
	void FillPolarCurve(CCurve *pCurve, CPolar* pPolar, int XVar, int YVar);
	void FillOppCurve(OpPoint *pOpp, Graph *pGraph, CCurve *pCurve, bool bInviscid=false);
	void OnContextMenu(CPoint ScreenPoint, CPoint ClientPoint) ;
	void OnCpCalcError();
	void PaintImage(ATL::CImage *pImage, CString &FileName, int FileType);
	void PaintCoupleGraph(CDC *pDC, CRect *pDrawRect);
	void PaintOpPoint(CDC *pDC, CRect* pCltRect, CRect* pDrawRect);
	void PaintPolarGraphs(CDC *pDC, CRect *pCltRect);
	void PaintSingleGraph(CDC *pDC, CRect *pDrawRect);
	void PrintAll(CDC *pDC, CRect *pCltRect);
	void SaveSettings(CArchive &ar);
	void SetBufferFoil();
	void SetFoilScale(CRect CltRect);
	void SetDisplayDefault();
	void SetHingeMoments(OpPoint *pOpPoint);
	void SetLogPixels(int LPY);
	void SetOpp(OpPoint *pOpp = NULL);
	void SetParams();
	void SetTitles(Graph* pGraph, int iX, int iY);
	void SetColors(Graph *pDefaultGraph);
	void SetFileHeader();
	void UpdateFoils();
	void UpdatePlrs();
	void UpdateOpps();
	void UpdateView(CDC *pDC = NULL);

	Graph* GetGraph(CPoint &pt); // returns the graph in which the point is

	CFoil* AddBufferFoil();
	OpPoint* GetCurOpPoint();// old way, with Oper
	OpPoint* GetOpPoint(double Alpha);
	OpPoint* AddOpPoint(OpPoint *pNewPoint = NULL);
	CFoil* AddFoil(CString strFoilName, double x[], double y[], int nf, double thickness=0.f, double camber=0.f);
	CFoil* GetFoil(CString strFoilName);
	CPolar* AddPolar(CPolar* pPolar);
	CPolar* GetPolar(CString PlrName);

	LRESULT KickIdle();
	
	void OnAdvSettings();
	void OnExportFoil();
	void OnPolars();
	void OnShowPanels();
	void OnRenameFoil();
	void OnDeleteFoil();
	void OnClCd();
	void OnClAlpha();
	void OnClXtr();
	void OnCmAlpha();
	void OnAllFour();
	void OnCpGraph();
	void OnQGraph();
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnResetGraph();
	void OnSingleAnalysis();
	void OnBatchAnalysis();

	void OnRenamePlr();
	void OnDeletePlr();
	void OnDeleteOpp();
	void OnExportPlr();
	void OnDefineGraph();
	void OnCoupleGraph();
	void OnDefineUserGraph();
	void OnAllGraphOptions();
	void OnDelFoilPolars();
	void OnDelCurOpp();
	void OnRestDefGraphs();
	void OnGeom();
	void OnPanel();
	void OnCadd();
	void OnInterpolate();
	void OnFoilStyle();
	void OnRestoreDef();
	void OnReadDef();
	void OnWriteDef();
	void OnSavePolars();
	void OnPolarFilter();
	void OnShowBL();
	void OnShowPressure();
	void OnRestoreVariables();
	void OnGraphOptions();
	void OnAnalyze();
	void OnOper();
	void OnResetScale();
	void OnPrint();
	void OnLoad();
	void OnExportOpps();
	void OnInitXFoil();
	void OnAbout();
	void OnPolarReset();
	void OnShowUserGraph();
	void OnUserGraph();
	void OnEditPolar();
	void OnDuplicateFoil();
	void OnNormalize();
	void OnHidePolars();
	void OnShowPolars();
	void OnDeRotateFoil();
	void OnCpGraphDlg();
	void OnHideAllPolars();
	void OnShowAllPolars();
	void OnSavePlrFile();
	void OnZoomGraph();
	void OnTEGap();
	void OnLERad();
	void OnEditCoord();
	void OnSetFlap();
	void OnResetAllScales();
	void OnExportCurOpp();
	void OnNacaFoils();
	void OnHPlot();
	void OnDtPlot();
	void OnDbPlot();
	void OnUePlot();
	void OnCfPlot();
	void OnCdPlot();
	void OnNPlot();
	void OnCtPlot();
	void OnRtPlot();
	void OnRtLPlot();
	
	
	// Generated message map functions
protected:
	
	afx_msg void OnImportXFoilPolar();
	afx_msg void OnImportJavaFoilPolar();
	afx_msg void OnShowCurOpp();
	afx_msg void OnShowOpps();
	afx_msg void OnHideOpps();
	afx_msg void OnShowFoilOpps();
	afx_msg void OnHideFoilOpps();
	afx_msg void OnCpi();
	afx_msg void OnManageFoils();
	afx_msg void OnExportResults();
	afx_msg void OnShowCpGraph();
	afx_msg void OnExportCurrentResults();
	afx_msg void OnExportGraphToFile();
	
	DECLARE_MESSAGE_MAP()
};

