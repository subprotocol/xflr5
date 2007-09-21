/****************************************************************************

    CMiarex Class
	Copyright (C) 2005 André Deperrois xflr5@yahoo.com

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
// Miarex.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMiarex window

#include "WPolar.h"
#include "../Graph/Graph.h"
#include "../misc/UFOListDlg.h"
#include "WngAnalysis.h"
#include "VLMDlg.h"
#include "3DPanelDlg.h"
#include "Plane.h"
#include "GLLight.h"
#include "FlowLinesDlg.h"
#include "LLTDlg.h"


// Custom palette structure
typedef struct tagLogicalPalette 
{
	WORD wVersion;
	WORD wNumEntries;
	PALETTEENTRY peaEntries[256];
} 
LOGICALPALETTE;


/////////////////////////////////////////////////////////////////////////////
// CMiarex dialog

class CMiarex : public CWnd
{  
	friend class CXFLR5App;
	friend class CChildView;
	friend class CEditPlrDlg;
	friend class CMainFrame;
	friend class CWngAnalysis;
	friend class CCtrlAnalysis;
	friend class CWOperDlgBar;
	friend class CW3DBar;
	friend class CLLTThread;
	friend class CVLMThread;
	friend class CVLMDlg;
	friend class CFlowLinesDlg;
	friend class CFlowDlg;
	friend class CPlane;
	friend class CWing;
	friend class CWingPage;
	friend class CFoilsPage;
	friend class CGeomPage;
	friend class CCurveDlgBar;
	friend class CWPolar;
	friend class CPolar;
	friend class C3DPanelDlg;
	friend class CLLTDlg;
	friend class CListPlrDlg;
	friend class CListWing;
	friend class CWingScaleDlg;
	friend class CScaleOppBar;
	friend class CGLLight;
	friend class CUFOListDlg;

// Construction
public:
	CMiarex(CWnd* pParent = NULL);	// standard constructor
	virtual ~CMiarex();

// Dialog Data
	//{{AFX_DATA(CMiarex)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMiarex)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	afx_msg void OnReset3DView();
	afx_msg void OnSingleGraph1();
	afx_msg void OnSingleGraph2();
	afx_msg void OnSingleGraph3();
	afx_msg void OnSingleGraph4();
	afx_msg void OnDefinePlane();
	afx_msg void OnEditPlane();
	afx_msg void OnSaveUFO();
	afx_msg void OnShowWing2();
	afx_msg void OnShowElevator();
	afx_msg void OnShowFin();
	afx_msg void OnAutoScales();
	afx_msg void OnAutoWingScales();
	afx_msg void OnDeleteAll();
	afx_msg void OnExportPanels();
	afx_msg void OnWingGraph4();
	afx_msg void OnWingGraph3();
	afx_msg void OnWingGraph2();
	afx_msg void OnWingGraph1();
	afx_msg void OnStreamLines();
	afx_msg void OnGLLight();
	afx_msg void OnStreamOptions();
	afx_msg void OnManageUFO();
	afx_msg void OnCpView();

	DECLARE_MESSAGE_MAP()


private:

	BOOL OnEraseBkgnd(CDC* pDC);
	void OnExit();
	void OnWingAnalysis();
	void OnShowWPolar();
	void OnExportWing();
	void OnLoadProject();
	void OnSaveProject();
	void OnDeleteWPolar();
	void OnGraphDlg();
	void OnResetDef();
	void OnResetScales();
	void OnListPlrs();
	void OnDefineVar();
	void OnTwoGraphs();
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnDefineWing();
	void OnDelCurWOpp();
	void OnWOpp();
//	void OnShowPoints();
	void OnEditWing();
	void OnDuplicateWing();
	void OnRenameUFO();
	void OnDeleteWing();
	void OnPolarFilter();
	void OnRenameWPolar();
	void OnDeleteWPolars();
	void OnFourGraphs();
	void OnAbout();
	void OnExportWOpp();
	void OnAdjustToWing();
	void OnPrint();
	void OnWPolarReset();
	void OnSaveAs();
	void OnNewProject();
	void OnEditWPlr();
	void OnInsertProject();
	void OnWGraphVar();
	void OnSingleWingGraph();
	void OnFourWingGraphs();
	void OnTwoWingGraphs();
	void OnShowCurWOpp();
	void OnShowAllWOpps();
	void OnHideAllWOpps();
	void OnDelAllPlrWOpps();
	void OnAdvSettings();
	void OnShowXCmRef();
	void OnHideAllWPlrs();
	void OnShowAllWPlrs();
	void OnWngHideAll();
	void OnWngShowAll();
	void OnImportWing();
	void OnShowElliptic();
	void OnResetWingScale();
	void On3DView();
	void OnScaleWing();
	void OnResetWingLegend();
	void OnResetLegend();
	void OnResetAllScales();
	void OnHideWOpps();
	void OnShowWOpps();
	void OnDelAllWOpps();
	void OnHideWingOpps();
	void OnShowWingOpps();
	void OnWPolarExport();
	void GLCreateAxes();
	void GLCreateGeom(CWing *pWing, UINT List, COLORREF wingcolor);
	void GLCreateVLMMesh();
	void GLCreateWakePanels(int LIST, CWOpp *pWOpp);
	void GLCreateWakeLines(int LIST);
	void GLCreateWakeSurfaces(int LIST);
	void GLCreateCp();
	void GLCreateTrans(CWing *pWing, CWOpp *pWOpp, UINT List);
	void GLCreateDrag(CWing *pWing, CWOpp *pWOpp, UINT List);
	void GLCreateLift(CWing *pWing, CWOpp *pWOpp, UINT List);
	void GLCreateLiftForce();
	void GLCreateMoments();
	void GLCreateWingLegend();
	void GLCreateWOppLegend();
	void GLCreateSurfFlow();
	void GLCreateStreamLines();
	void GLCreateDownwash(CWing *pWing, CWOpp *pWOpp, UINT List, int surf0);
	void GLCreateCpLegend();
	void GLRenderView();
	void GLRenderSphere(COLORREF cr, double radius);
	void GLDraw3D(CDC *pDC);
	void GLSetupLight();
	void GLCreateCtrlPts();
	void GLCreateVortices();

	void AddWOpp(bool bPointOut, double *Gamma = NULL);
	void AddWOpp(CWOpp *pNewPoint);
	void AddPOpp(double *Gamma = NULL, CPOpp *pPOpp = NULL);
	void Animate(bool bAnimate);
	void CheckMenus();
	void CreateWPolarCurves();
	void CreateWOpp(CWOpp *pWOpp, CWing *pWing);
	void CreateWOppCurves();
	void CreateCpCurves();
	void DeleteProject();
	void DrawWOppLegend(CDC* pDC, bool bIsPrinting, CPoint place, int bottom);
	void DrawWPolarLegend(CDC *pDC, bool bIsPrinting, CPoint place, int bottom);
	void FillWOppCurve(CWOpp *pWOpp, Graph *pGraph, CCurve *pCurve, int Var);
	void FillWPlrCurve(CCurve *pCurve, CWPolar *pWPolar, int XVar, int YVar);
	void GetLinearizedPolar(CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau, double &Alpha0, double &Slope);
	void OnContextMenu(CPoint ScreenPoint, CPoint ClientPoint);
	void PaintWing(CDC *pDC, CPoint ORef, double scale, bool bIsPrinting);
	void PrintSingleWingGraph(CDC *pDC, CRect *pCltRect);
	void PaintSingleWingGraph(CDC *pDC, CRect *pCltRect, CRect *pDrawRect);
	void PaintTwoWingGraph(CDC *pDC, CRect *pCltRect, CRect *pDrawRect);
	void PrintTwoWingGraph(CDC *pDC, CRect *pCltRect);
	void PaintFourWingGraph(CDC *pDC, CRect *pCltRect, CRect *pDrawRect);
	void PrintFourWingGraph(CDC *pDC, CRect *pCltRect);
	void PaintXCmRef(CDC *pDC, CPoint ORef, double scale, bool bIsPrinting);
	void PaintXCP(CDC *pDC, CPoint ORef, CPoint OLegend, double scale, bool bIsPrinting);
	void PaintXTr(CDC *pDC, CPoint ORef, CPoint OLegend, double scale, bool bIsPrinting);
	void PaintWOpp(CDC *pDC, CRect *pCltRect, CRect *pDrawRect);
	void PaintCp(CDC *pDC, CRect *pCltRect, CRect *pDrawRect);
	void PaintWSingleGraph(CDC *pDC, CRect *pCltRect, CRect *pDrawRect);
	void PaintWCoupleGraphs(CDC *pDC, CRect *pCltRect, CRect *pDrawRect);
	void PaintWFourGraphs(CDC *pDC, CRect *pCltRect, CRect *pDrawRect);
	void PrintAll(CDC *pDC, CRect *pCltRect);
	void ResetElements();
	void SaveSettings(CArchive &ar);
	void StopAnimate();
	void SetParams();
//	void SetWing(bool bCurrent=true, CString WingName="");
	void SetWPlr(bool bCurrent = true, CString WPlrName = "");
	void SetWingLegendPos();
	void SetWPlrLegendPos();
	void SetColors(Graph* pDefaultGraph) ;
	void SetScale();
	void SetScale(CRect CltRect);
	void Set2DScale();
	void Set3DScale();
	void SetWGraphScale();
	void UpdateView(CDC *pDC=NULL);
	void UpdateWOpps();
	void UpdateWPlrs();
	void UpdateUFOs();
	void UpdateUnits();
	bool LoadSettings(CArchive &ar);
	bool SetMiarexCursor(CWnd* pWnd, CPoint ptMouse, UINT message);
	bool SetModWing(CWing *pWing);
	bool SetWOpp(bool bCurrent, double Alpha = 0.0);
	bool SetPOpp(bool bCurrent, double Alpha = 0.0);
	bool AVLImportFile(CString FileName);
	BOOL AVLReadSurface(CStdioFile *pXFile, int &Line, CWing *pWing, int &NSpan, int &NChord, double &Sspace);
	BOOL AVLReadSection(CStdioFile *pXFile, int &Line, CWing *pWing, int PanelPos);
	CString RenameUFO(CString UFOName);

	LRESULT KickIdle();

	double GetCd(CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, double AR, bool &bOutRe, bool &bError);
	double GetXCp(CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, double AR, bool &bOutRe, bool &bError);
	double GetXTr(CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, bool bTop, bool &bOutRe, bool &bError);
	double GetZeroLiftAngle(CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau);
	Graph* GetGraph(CPoint pt);
	CWPolar* AddWPolar(CWPolar* pWPolar);
	CWPolar* GetWPolar(CString WPolarName);
	CWing* AddWing(CWing *pWing);
	CWOpp* GetWOpp(double Alpha);
	CPOpp* GetPOpp(double Alpha);

	int ReadFoilPoints(CStdioFile *pXFile, double *x, double *y);

	bool CreateElements(CWing *pWing, CPanel *pPanel);
	bool CreateWakeElems(int pw);
	int  IsNode(CVector Pt);
	int  IsWakeNode(CVector Pt);
	bool VLMInitializePanels();
	bool VLMIsSameSide(int p, int pp);

	void Analyze(double V0, double VMax, double VDelta, bool bSequence, bool bInitCalc);
	void PanelAnalyze(double V0, double VMax, double VDelta, bool bSequence, bool bInitCalc);
	void VLMAnalyze(double V0, double VMax, double VDelta, bool bSequence, bool bInitCalc);
	void LLTAnalyze(double V0, double VMax, double VDelta, bool bSequence, bool bInitCalc);
	void NormalVector(GLdouble p1[3], GLdouble p2[3],  GLdouble p3[3], GLdouble n[3]);

	CPlane * CreatePlane();
	CPlane * AddPlane(CPlane *pPlane);
	CPlane * GetPlane(CString PlaneName);
	CWing * GetWing(CString WingName);
	bool SetModPlane(CPlane *pModPlane);
	void DuplicatePlane();
	void SetUFO(CString UFOName="");
	void CreateSurfaces(CWing *pWing, CVector T, double XTilt, double YTilt, COLORREF color);


//____________________Variables______________________________________
//
	CPanel m_Panel[VLMMATSIZE];		// the panel array for the currently loaded UFO
	CVector m_Node[2*VLMMATSIZE];		// the node array for the currently loaded UFO
	CPanel m_TiltedPanels[VLMMATSIZE];	// used if the analysis should be performed on the tilted geometry
	CVector m_TiltedNodes[2*VLMMATSIZE];	// used if the analysis should be performed on the tilted geometry
	CPanel  m_WakePanel[VLMMATSIZE];	// used for wake roll-up
	CVector m_WakeNode[2*VLMMATSIZE];	// used for wake roll-up
	CSurface m_Surface[MAXVLMSURFACES];	// the Surface array for the currently loaded UFO

	CWngAnalysis m_WngAnalysis;		// the dialog box for the polar definition
	
	CWnd* m_pChildWnd;			// a pointer to the view class
	CWnd* m_pFrame ;			// a pointer to the frame class
	CWnd * m_pW3DBar;			// a pointer to the 3D view dialog bar

	CVLMDlg m_VLMDlg;			// the dialog class which manages the VLM calculations
	C3DPanelDlg m_PanelDlg;			// the dialog class which manages the 3D panel calculations
	CLLTDlg m_IDlg;			// the dialog class which manages the LLT calculations
	CGLLight m_GLLightDlg;			// the dialog class for GL light options 
	CFlowLinesDlg m_FlowLinesDlg;		// the dialog class for streamline options

	CObArray *m_poaFoil;			// a pointer to the foil array
	CObArray *m_poaPolar;			// a pointer to the foil polar array
	CObArray *m_poaWing;			// a pointer to the wing array
	CObArray *m_poaPlane;			// a pointer to the plane array
	CObArray *m_poaWPolar;			// a pointer to the UFO polar array
	CObArray *m_poaWOpp;			// a pointer to the UFO OpPoint array
	CObArray *m_poaPOpp;			// a pointer to the Plane OpPoint array

//	CString m_VersionName;

	bool m_bIsPrinting;			// the view is being printed
	bool m_bTrans;				// the view is being dragged
	bool m_bType1, m_bType2, m_bType4;	// polar types to be displayed
	bool m_bShowElliptic;			// true if the elliptic loading should be displayed in the local lift graph
	bool m_bStoreWOpp;			// true if the WOpp should be stored after a calculation
	bool m_bKeepOutOpps;			// true if points out of hte polar mesh should be kept
	bool m_bCurWOppOnly;			// true if only the current WOpp is to be displayed
	bool m_bHalfWing;			// true if only a half-wing should be displayed in the OpPoint view
	bool m_bResetglGeom;			// true if the geometry OpenGL list needs to be refreshed
	bool m_bResetglMesh;			// true if the mesh OpenGL list needs to be refreshed
	bool m_bResetglWake;			// true if the wake OpenGL list needs to be refreshed
	bool m_bResetglOpp, m_bResetglLift, m_bResetglDrag, m_bResetglDownwash;			// true if the OpenGL lists need to be refreshed
	bool m_bResetglStream;			// true if the streamlines OpenGL list needs to be refreshed
	bool m_bResetglFlow;			// true if the crossflow OpenGL list needs to be refreshed
	bool m_bAnimate;			// true if there is an animation going on, 
	bool m_bAnimatePlus;			// true if the animation is going in aoa crescending order
	bool m_bTransGraph;			// true if a graph is being dragged
	bool m_bShowWing2;			// true if the biplane's second wing OpPoint results should be displayed
	bool m_bShowStab;			// true if the stabilisator (elevator) OpPoint results should be displayed
	bool m_bShowFin;			// true if the fin OpPoint results should be displayed
	bool m_bIs2DScaleSet;			// true if the 2D scale has been set, false if needs to be reset 
	bool m_bIs3DScaleSet;			// true if the 3D scale has been set, false if needs to be reset 
	bool m_bAutoScales;			// true if the scale is to be reset after each UFO selection
	bool m_bCheckPlanePanels;		// true if the plane elevator panels should be aligned with the wing's panels
	bool m_bLogFile;			// true if the log file warning is turned on
	bool m_bShowLight;			// true if the virtual light is to be displayed
	bool m_bResetWake;
	bool m_bVLMFinished;			// true if the VLM calculation is finished
	bool m_bSetNewWake;			// true if the wake needs to be reset

	int m_NSurfaces;
	int m_nNodes;				// the current number of nodes for the currently loaded UFO
	int m_VLMMatSize;			// the matrix size
	int m_iView;				//1=opp, 2=plr, 3=3D
	int m_GLList;				// number of current Open Gl Lists
	int m_iWingView;			// defines how many graphs will be displayed in WOpp view
	int m_iWPlrView;			// defines how many graphs will be displayed in WPolar view
	int m_XW1, m_YW1, m_XW2, m_YW2, m_XW3, m_YW3, m_XW4, m_YW4; 	// the WPolar graph variables
	int m_WOppVar1,m_WOppVar2, m_WOppVar3, m_WOppVar4; 		// the WOpp graph variables
	int m_Iter ;				// the number of iterations for LLT
	int m_NStation ;			// the number of stations for LLT
	int m_posAnimate;			// the current animation aoa index
	int m_nWakeNodes;			// Max Size for the node array if there is a wake
	int m_WakeSize;				// Max Size for the VLMMatrix if there is a wake
	int m_NXWakePanels;			// wake panel number
	int m_NWakeColumn;			// number of wake columns
	int m_MaxWakeIter;			// wake roll-up iteration limit
	int m_WakeInterNodes;		// number of intermediate nodes between wake panels

	double m_CurSpanPos;		//Span position for Cp Grpah
	double m_WakePanelFactor;	// incremental factor for wake lines
	double m_FirstPanelSize;	// wake lines first panel size
	double m_CoreSize;			// core size for VLM vortices
	double m_MinPanelSize;			// wing minimum panel size ; panels of less length are ignored
	double m_Relax;				// LLT relaxation factor
	double m_CvPrec;			// LLT iteration precision required for convergence
	double pi;				// ???
	double m_WingScale;			// scale for 2D display
	double m_LastWOpp;			// last WOPP selected, try to set the same if it exists, for the new polar

	CStdioFile* m_pXFile;			// a pointer to the output .log file
	CUFOListDlg m_UFOdlg;			// the dialog class for UFO management

	CPoint m_PointDown;			// last client left-click point
	CPoint m_ptOffset;			// client offset position for wing display
	CPoint m_WPlrLegendOffset;		// client offset position for wing polar legend
	CPoint m_WingLegendOffset;		// client offset position for WOPP polar legend


	CRect m_rCltRect, m_rDrawRect; // the client and drawing rectangles


	HCURSOR m_hcArrow;
	HCURSOR m_hcCross;
	HCURSOR m_hcMove;
	
	Graph m_WingGraph1;			// the WOpp graphs
	Graph m_WingGraph2;
	Graph m_WingGraph3;
	Graph m_WingGraph4;
	Graph m_WPlrGraph1;			// the WPolar graphs
	Graph m_WPlrGraph2;
	Graph m_WPlrGraph3;
	Graph m_WPlrGraph4;
	Graph m_CpGraph;			// provision for cross flow Cp Graph in 3D panel analysis
	Graph* m_pCurGraph;			// currently active graph
	Graph* m_pCurWPlrGraph;			// currently active WPolar graph
	Graph* m_pCurWingGraph;			// currently active WOpp graph
//	CCurve *m_pWingCurve1,*m_pWingCurve2,*m_pWingCurve3,*m_pWingCurve4; 


	CWing *m_pCurWing;			// the currently selected wing
	CWPolar * m_pCurWPolar;			// the currently selected WPolar
	CWOpp * m_pCurWOpp;			// the currently selected Wing Operating Point
	CPlane * m_pCurPlane;			// the currently selected Plane
	CWing * m_pCurWing2;			// the currently selected Plane's 2nd wing, if any
	CWing * m_pCurStab;			// the currently selected Plane's elevator
	CWing * m_pCurFin;			// the currently selected Plane's fin
	CPOpp * m_pCurPOpp;			// the currently selected Plane Operating Point

	COLORREF m_WingColor, m_StabColor, m_FinColor;
};
