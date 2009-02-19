/****************************************************************************

	Miarex    Copyright (C) 2008-2009 Andre Deperrois XFLR5@yahoo.com

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

 
#ifndef QMIAREX_H
#define QMIAREX_H

#include "../Params.h"
#include <QWidget>
#include <QPixmap>
#include <QCheckBox>
#include <QList>
#include <QDialog>
#include <QDataStream>
#include "GL3dViewDlg.h"
#include "GL3dBodyDlg.h"
#include "GLLightDlg.h"
#include "WPolarDlg.h"
#include "PanelAnalysisDlg.h"
#include "VLMAnalysisDlg.h"
#include "../Misc/FloatEdit.h"
#include "../Misc/LineButton.h"
#include "../Misc/LineCbBox.h"
#include "../Misc/LineDelegate.h"
#include "../Objects/Body.h"
#include "../Objects/Wing.h"
#include "../Objects/Plane.h"
#include "../Objects/WPolar.h"
#include "../Objects/WOpp.h"
#include "../Objects/POpp.h"
#include "../Objects/Polar.h"
#include "../Objects/Polar.h"
#include "../Objects/Foil.h"
#include "../Objects/OpPoint.h"
#include "../Graph/QGraph.h"



class QMiarex : public QWidget
{
	friend class MainFrame;
	friend class GLWidget;
	friend class TwoDWidget;
	friend class GL3dViewDlg;
	friend class GL3dBodyDlg;
	friend class WingDlg;
	friend class CWing;
	friend class LLTAnalysisDlg;
	friend class VLMAnalysisDlg;
	friend class PanelAnalysisDlg;
	friend class PlaneDlg;

	Q_OBJECT


public:

	QMiarex(QWidget *parent = NULL);
	~QMiarex();

	CWPolar* AddWPolar(CWPolar* pWPolar);
	CWPolar* GetWPolar(QString WPolarName);
	CWing* AddWing(CWing *pWing);
	CWOpp* GetWOpp(double Alpha);
	CPOpp* GetPOpp(double Alpha);
	CBody* AddBody(CBody *pBody);

	CPlane * AddPlane(CPlane *pPlane);
	CPlane * GetPlane(QString PlaneName);
	CWing * GetWing(QString WingName);
	CBody *GetBody(QString BodyName);
	CWOpp* InsertWOpp(CWOpp *pNewPoint);
	void AddWOpp(bool bPointOut, double *Gamma = NULL, double *Sigma = NULL, double *Cp = NULL);
	void AddPOpp(bool bPointOut, double *Cp, double *Gamma = NULL, double *Sigma=NULL, CPOpp *pPOpp = NULL);
	void SetUFO(QString UFOName="");
	void SetWPlr(bool bCurrent = true, QString WPlrName = "");
	void OnLButtonDown(QMouseEvent *event);

	bool SetWOpp(bool bCurrent, double Alpha = 0.0);
	bool SetPOpp(bool bCurrent, double Alpha = 0.0);


private slots:
	void OnWPolars();
	void OnWOpps();
	void On3DView();
	void OnNewWing();
	void OnNewPlane();
	void OnEditUFO();
	void OnAnalyze();
	void OnCurveStyle(int index);
	void OnCurveWidth(int index);
	void OnCurveColor();
	void OnShowPoints(bool checked);
	void OnShowCurve(bool checked);
	void OnSingleWPlrGraph1();
	void OnSingleWPlrGraph2();
	void OnSingleWPlrGraph3();
	void OnSingleWPlrGraph4();
	void OnTwoWPlrGraphs();
	void OnFourWPlrGraphs();
	void OnDefineGraphVariables();
	void OnHideAllWPolars();
	void OnShowAllWPolars();
	void OnSingleWingGraph1();
	void OnSingleWingGraph2();
	void OnSingleWingGraph3();
	void OnSingleWingGraph4();
	void OnTwoWingGraphs();
	void OnFourWingGraphs();
	void OnExportCurWOpp();
	void OnCurWOppOnly();
	void OnDeleteAllWPlrOpps();
	void OnDeleteAllWOpps();
	void OnHideAllWOpps();
	void OnShowAllWOpps();
	void OnRenameCurUFO();
	void OnRenameCurWPolar();
	void OnDeleteCurUFO();
	void OnDeleteCurWPolar();
	void OnDeleteCurWOpp();
	void OnDefineWPolar();
	void OnSequence(int state);
	void OnInitLLTCalc(int state);
	void OnStoreWOpp(int state);
	void OnHalfWing(int state);
	void OnShowLift(int state);
	void OnShowIDrag(int state);
	void OnShowVDrag(int state);
	void OnShowTransitions(int state);
	void OnAnimate(int state);
	void OnAdjustToWing();
	void OnAdvancedSettings();
	void OnHideUFOWPolars();
	void OnShowUFOWPolars();
	void OnResetWOppLegend();
	void OnResetWPlrLegend();
	void OnNewBody();
	void OnEditCurBody();

private:
	void keyPressEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent (QWheelEvent *event);
	void showEvent(QShowEvent *event);

	void LLTAnalyze(double V0, double VMax, double VDelta, bool bSequence, bool bInitCalc);
	void CreateCpCurves();
	void UpdateUnits();
	void SetWGraphTitles(Graph* pGraph, int iX, int iY);
	void SetWingLegendPos();
	void SetupLayout();
	void CreateWPolarCurves();
	void CreateWOppCurves();
	void SetWGraphScale();
	void FillWPlrCurve(CCurve *pCurve, CWPolar *pWPolar, int XVar, int YVar);
	void FillWOppCurve(CWOpp *pWOpp, Graph *pGraph, CCurve *pCurve, int Var);
	void *GetUFOPlrVariable(CWPolar *pWPolar, int iVar);
	void JoinSurfaces(CSurface *pLeftSurf, CSurface *pRightSurf, int pl, int pr);
	void SetScale();
	void SetScale(QRect CltRect);
	void Set2DScale();
	void Set3DScale();
	void DrawWOppLegend(QPainter &painter, QPoint place, int bottom);
	void DrawWPolarLegend(QPainter &painter, QPoint place, int bottom);
	void PaintWSingleGraph(QPainter &painter);
	void PaintWCoupleGraphs(QPainter &painter);
	void PaintWFourGraphs(QPainter &painter);
	void PaintWOpp(QPainter &painter);
	void PaintXCmRef(QPainter &painter, QPoint ORef, double scale);
	void PaintXCP(QPainter &painter, QPoint ORef, QPoint OLegend, double scale);
	void PaintXTr(QPainter &painter, QPoint ORef, QPoint OLegend, double scale);
	void PaintCp(QPainter &painter);
	void PaintFourWingGraph(QPainter &painter);
	void PaintTwoWingGraph(QPainter &painter);
	void PaintSingleWingGraph(QPainter &painter);
	void PaintView(QPainter &painter);
	void FillComboBoxes(bool bEnable = true);
	void SetAnalysisParams();
	void SetCurveParams();
	void SetWPlrLegendPos();
	void StopAnimate();
	void UpdateView();
	void UpdateCurve();
	void PaintWing(QPainter &painter, QPoint ORef, double scale);
	void PanelAnalyze(double V0, double VMax, double VDelta, bool bSequence);
	void VLMAnalyze(double V0, double VMax, double VDelta, bool bSequence);
	void RotateGeomY(double const &Angle, CVector const &P);
	void RotateGeomZ(double const &Beta, CVector const &P);

	int CreateBodyElements(CPanel *pPanel);
	int CreateElements(CSurface *pSurface);
	int IsWakeNode(CVector &Pt);
	int IsNode(CVector &Pt);
	bool InitializePanels();
	bool CreateWakeElems(int PanelIndex);
	bool VLMIsSameSide(int p, int pp);
	void CreateWOpp(CWOpp *pWOpp, CWing *pWing);

	double GetCl(CFoil  *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, bool &bOutRe, bool &bError);
	double GetCm(CFoil  *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, bool &bOutRe, bool &bError);
	double GetCm0(CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau, bool &bOutRe, bool &bError);
	double GetCd(CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, double AR, bool &bOutRe, bool &bError);
	double GetXCp(CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, double AR, bool &bOutRe, bool &bError);
	double GetXTr(CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, bool bTop, bool &bOutRe, bool &bError);
	double GetZeroLiftAngle(CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau);
	double GetPlrPointFromAlpha(CFoil *pFoil, double Re, double Alpha, int PlrVar, bool &bOutRe, bool &bError);
	double GetPlrPointFromCl(CFoil *pFoil, double Re, double Cl, int PlrVar, bool &bOutRe, bool &bError);
	double GetVar(int nVar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Cl, double Tau, bool &bOutRe, bool &bError);
	void GetLinearizedPolar(CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau, double &Alpha0, double &Slope);
	void * GetPlrVariable(CPolar *pPolar, int iVar);

	bool Intersect(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB, CVector const &Normal, CVector const &A,  CVector const &U,  CVector &I, double &dist);
	bool SetModWing(CWing *pWing);
	bool SetModPlane(CPlane *pModPlane);

	bool LoadSettings(QDataStream &ar);
	bool SaveSettings(QDataStream &ar);

	bool EditPlane(CPlane *pPlane);

	QString RenameUFO(QString UFOName);
	QGraph* GetGraph(QPoint &pt);

//____________________Variables______________________________________
//
private:

	QCheckBox *m_pctrlSequence;
	FloatEdit *m_pctrlAlphaMin;
	FloatEdit *m_pctrlAlphaMax;
	FloatEdit *m_pctrlAlphaDelta;
	QCheckBox *m_pctrlInitLLTCalc;
	QCheckBox *m_pctrlStoreWOpp;
	QPushButton *m_pctrlAnalyze;

	QCheckBox *m_pctrlHalfWing, *m_pctrlLift, *m_pctrlIDrag, *m_pctrlVDrag;
	QCheckBox *m_pctrlTrans, *m_pctrlDownwash,  *m_pctrlAnimate;
	QSlider *m_pctrlAnimateSpeed;

	QCheckBox *m_pctrlShowCurve;
	QCheckBox *m_pctrlShowPoints;
	LineCbBox *m_pctrlCurveStyle;
	LineCbBox *m_pctrlCurveWidth;
	LineButton *m_pctrlCurveColor;
	LineDelegate *m_pStyleDelegate, *m_pWidthDelegate;

	int m_CurveStyle, m_CurveWidth;
	QColor m_CurveColor;

	GL3dBodyDlg m_GL3dBody;
	GL3dViewDlg m_GL3dView;
	GLLightDlg m_GLLightDlg;
	WPolarDlg m_WngAnalysis;

	void* m_pMainFrame ;			// a pointer to the frame class
	void *m_p2DWidget;

	PanelAnalysisDlg *m_pPanelDlg;			// the dialog class which manages the VLM calculations
	VLMAnalysisDlg *m_pVLMDlg;			// the dialog class which manages the VLM calculations

protected:
	CPanel m_Panel[VLMMATSIZE];		// the panel array for the currently loaded UFO
	CPanel m_WakePanel[VLMMATSIZE];	// the reference current wake panel array
	CPanel m_MemPanel[VLMMATSIZE];		// used if the analysis should be performed on the tilted geometry
	CPanel m_RefWakePanel[VLMMATSIZE]; 	// the reference wake panel array if wake needs to be reset
	CPanel *m_pPanel[VLMMATSIZE];		// an array to the re-ordered VLM panels for a calculation

	CVector m_Node[2*VLMMATSIZE];		// the node array for the currently loaded UFO
	CVector m_MemNode[2*VLMMATSIZE];	// used if the analysis should be performed on the tilted geometry
	CVector m_WakeNode[2*VLMMATSIZE];	// the reference current wake node array
	CVector m_RefWakeNode[2*VLMMATSIZE]; 	// the reference wake node array if wake needs to be reset
	CVector m_TempWakeNode[2*VLMMATSIZE];	// the temporary wake node array during relaxation calc

	double m_aij[VLMMATSIZE*VLMMATSIZE];    // coefficient matrix
	double m_aijRef[VLMMATSIZE*VLMMATSIZE]; // coefficient matrix
	double m_RHS[VLMMATSIZE*100];			// RHS vector
	double m_RHSRef[VLMMATSIZE*100];		// RHS vector

	CVector m_L[(MAXBODYFRAMES+1)*(MAXSIDELINES+1)]; //temporary points to save calculation times for body NURBS surfaces
	CVector m_T[(MAXBODYFRAMES+1)*(MAXSIDELINES+1)];

	CSurface *m_pSurface[2*MAXPANELS];	// An array with the pointers to the diferrent wing's surfaces



	QList<void *> *m_poaFoil;			// a pointer to the foil array
	QList<void *> *m_poaPolar;			// a pointer to the foil polar array
	QList<void *> *m_poaWing;			// a pointer to the wing array
	QList<void *> *m_poaPlane;			// a pointer to the plane array
	QList<void *> *m_poaWPolar;			// a pointer to the UFO polar array
	QList<void *> *m_poaWOpp;			// a pointer to the UFO OpPoint array
	QList<void *> *m_poaPOpp;			// a pointer to the Plane OpPoint array
	QList<void *> *m_poaBody;			// a pointer to the Body array

//	bool m_bIsPrinting;			// the view is being printed
	bool m_bTrans;				// the view is being dragged
	bool m_bDragPoint;				// a point is being dragged
	bool m_bType1, m_bType2, m_bType4, m_bType5, m_bType6;	// polar types to be displayed
	bool m_bShowElliptic;			// true if the elliptic loading should be displayed in the local lift graph
	bool m_bStoreWOpp;			// true if the WOpp should be stored after a calculation
	bool m_bKeepOutOpps;			// true if points out of hte polar mesh should be kept
	bool m_bCurWOppOnly;			// true if only the current WOpp is to be displayed
	bool m_bCurFrameOnly;			// true if only the currently selected body frame is to be displayed
	bool m_bHalfWing;			// true if only a half-wing should be displayed in the OpPoint view
	bool m_bAnimate;			// true if there is an animation going on,
	bool m_bAnimatePlus;		// true if the animation is going in aoa crescending order
	bool m_bTransGraph;			// true if a graph is being dragged
	bool m_bShowWing2;			// true if the biplane's second wing OpPoint results should be displayed
	bool m_bShowStab;			// true if the stabilisator (elevator) OpPoint results should be displayed
	bool m_bShowFin;			// true if the fin OpPoint results should be displayed
	bool m_bIs2DScaleSet;		// true if the 2D scale has been set, false if needs to be reset
	bool m_bAutoScales;			// true if the scale is to be reset after each UFO selection
	bool m_bLogFile;			// true if the log file warning is turned on
	bool m_bResetWake;
//	bool m_bVLMFinished;		// true if the VLM calculation is finished
	bool m_bSetNewWake;			// true if the wake needs to be reset
	bool m_bDirichlet;			// true if Dirichlet BC are applied in 3D panel analysis, false if Neumann
	bool m_bTrefftz;			// true if the induced drag should be calculated in the Trefftz plane, false if calculated by summation over panels
	bool m_bXTop, m_bXBot, m_bXCP, m_bXCmRef; 	// defines whether the corresponfing data should be displayed
	bool m_bICd, m_bVCd;  	// defines whether the corresponfing data should be displayed
	bool m_bSequence;
	bool m_bInitLLTCalc;


	int m_NSurfaces;
	int m_nNodes;				// the current number of nodes for the currently loaded UFO
	int m_MatSize;			// the matrix size
	int m_iView;				//1=opp, 2=plr, 3=3D, 4=Cp, 5=BodyDesign
	int m_GLList;				// number of current Open Gl Lists
	int m_iWingView;			// defines how many graphs will be displayed in WOpp view
	int m_iWPlrView;			// defines how many graphs will be displayed in WPolar view
	int m_XW1, m_YW1, m_XW2, m_YW2, m_XW3, m_YW3, m_XW4, m_YW4; 	// the WPolar graph variables
	int m_WOppVar1,m_WOppVar2, m_WOppVar3, m_WOppVar4; 		// the WOpp graph variables
	int m_Iter ;				// the number of iterations for LLT
	int m_NStation ;			// the number of stations for LLT
	int m_posAnimate;			// the current animation aoa index
	int m_nWakeNodes;			// Size of the node array if there is a wake
	int m_WakeSize;				// Size of the Matrix if there is a wake
	int m_NWakeColumn;			// number of wake columns
	int m_MaxWakeIter;			// wake roll-up iteration limit
	int m_WakeInterNodes;		// number of intermediate nodes between wake panels
	int m_InducedDragPoint;		// 0 if dwonwash is at panel's centroid, 1 if averaged over panel length //used in CWing::VLMTrefftz

	int m_NHoopPoints;			//hoop resolution for NURBS bodies
	int m_NXPoints;				//longitudinal resolution for NURBS Bodies

	double m_CurSpanPos;		//Span position for Cp Grpah
	double m_CoreSize;			// core size for VLM vortices
	double m_MinPanelSize;			// wing minimum panel size ; panels of less length are ignored
	double pi;				// ???
	double m_WingScale;			// scale for 2D display
	double m_LastWOpp;			// last WOPP selected, try to set the same if it exists, for the new polar

	double m_AlphaMin, m_AlphaMax, m_AlphaDelta;
	double m_QInfMin, m_QInfMax, m_QInfDelta;
	double m_ControlMin, m_ControlMax, m_ControlDelta;

	int m_3DAxisStyle, m_3DAxisWidth;
	QColor m_3DAxisColor;

	int m_VLMStyle, m_VLMWidth;
	QColor m_VLMColor;

	int m_OutlineStyle, m_OutlineWidth;
	QColor m_OutlineColor;

	int m_XCPStyle, m_XCPWidth;
	QColor m_XCPColor;

	int m_MomentStyle, m_MomentWidth;
	QColor m_MomentColor;

	int m_IDragStyle, m_IDragWidth;
	QColor m_IDragColor;

	int m_VDragStyle, m_VDragWidth;
	QColor m_VDragColor;

	int m_XTopStyle, m_XTopWidth;
	QColor m_XTopColor;

	int m_XBotStyle, m_XBotWidth;
	QColor m_XBotColor;

	int m_DownwashStyle, m_DownwashWidth;
	QColor m_DownwashColor;

	int m_WakeStyle, m_WakeWidth;
	QColor m_WakeColor;

	QColor m_CpColor;
	int m_CpStyle, m_CpWidth;
	bool m_bShowCp, m_bShowCpPoints;


	QFile* m_pXFile;			// a pointer to the output .log file

	QPoint m_LastPoint;			// last mouse position point
	QPoint m_PointDown;			//last place where the user clicked
	QPoint m_ptOffset;			// client offset position for wing display
	QPoint m_WPlrLegendOffset;		// client offset position for wing polar legend
	QPoint m_WingLegendOffset;		// client offset position for WOPP polar legend
	QPoint m_ptPopUp;
	CVector m_RealPopUp;


	QRect m_rCltRect; // the client and drawing rectangles
	QRect m_rSingleRect;


	QCursor m_hcArrow;
	QCursor m_hcCross;
	QCursor m_hcMove;

	QGraph m_WingGraph1;			// the WOpp graphs
	QGraph m_WingGraph2;
	QGraph m_WingGraph3;
	QGraph m_WingGraph4;
	QGraph m_WPlrGraph1;			// the WPolar graphs
	QGraph m_WPlrGraph2;
	QGraph m_WPlrGraph3;
	QGraph m_WPlrGraph4;
	QGraph m_CpGraph;			// cross flow Cp Graph in 3D panel analysis
	QGraph* m_pCurGraph;			// currently active graph
	QGraph* m_pCurWPlrGraph;			// currently active WPolar graph
	QGraph* m_pCurWingGraph;			// currently active WOpp graph

	CWing *m_pCurWing;			// the currently selected wing
	CWing * m_pCurWing2;			// the currently selected Plane's 2nd wing, if any
	CWing * m_pCurStab;			// the currently selected Plane's elevator
	CWing * m_pCurFin;			// the currently selected Plane's fin
	CWPolar * m_pCurWPolar;			// the currently selected WPolar
	CWOpp * m_pCurWOpp;			// the currently selected Wing Operating Point
	CPlane * m_pCurPlane;			// the currently selected Plane
	CPOpp * m_pCurPOpp;			// the currently selected Plane Operating Point
	CFrame *m_pCurFrame;
	CBody *m_pCurBody;

	QColor m_WingColor, m_StabColor, m_FinColor;

	CVector P,W,V,T;
};

#endif // QMIAREX_H
