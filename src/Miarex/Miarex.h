/****************************************************************************

	Miarex    Copyright (C) 2008-2012 Andre Deperrois adeperrois@xflr5.com

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
#include "WAdvancedDlg.h"
#include <QPixmap>
#include <QCheckBox>
#include <QSlider>
#include <QStackedWidget>
#include <QToolButton>
#include <QPushButton>
#include <QRadioButton>
#include <QList>
#include <QDialog>
#include <QDataStream>
#include <QSettings>
#include "ArcBall.h"
#include "GL3dWingDlg.h"
#include "GL3dBodyDlg.h"
#include "GLLightDlg.h"
#include "PanelAnalysisDlg.h"
#include "LLTAnalysisDlg.h"

#include "ManageBodiesDlg.h"
#include "ManageUFOsDlg.h"
#include "../Misc/ModDlg.h"
#include "../Misc/RenameDlg.h"
#include "../Misc/ProgressDlg.h"
#include "../Misc/EditPlrDlg.h"
#include "../Misc/ObjectPropsDlg.h"
#include "../Misc/PolarFilterDlg.h"
#include "../Graph/GraphDlg.h"
#include "GLLightDlg.h"
#include "GL3DScales.h"
#include "StabViewDlg.h"
#include "PlaneDlg.h"
#include "WingScaleDlg.h"
#include "W3dPrefsDlg.h"
#include "InertiaDlg.h"

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
	friend class GL3DScales;
	friend class GL3dBodyDlg;
	friend class GLLightDlg;
	friend class WingDlg;
	friend class CWing;
	friend class LLTAnalysisDlg;
	friend class StabPolarDlg;
	friend class StabViewDlg;
	friend class PanelAnalysisDlg;
	friend class CPlane;
	friend class PlaneDlg;
	friend class ManageBodiesDlg;
	friend class ManageUFOsDlg;
	friend class EditPlrDlg;
	friend class GL3dWingDlg;
	friend class DisplaySettingsDlg;
	friend class UFOTableDelegate;
	friend class WPolarDlg;
	friend class CWPolar;

	Q_OBJECT


public:

	QMiarex(QWidget *parent = NULL);


private slots:
	void OnAllWingGraphScales();
	void OnAllWPolarGraphScales();
	void OnAllWingGraphSettings();
	void OnAllWPolarGraphSettings();
	void OnResetWingGraphScale();
	void On3DPrefs();
	void OnShowEllipticCurve();
	void OnWing2Curve();
	void OnStabCurve();
	void OnStabilityDirection();
	void OnFinCurve();
	void OnShowXCmRef();
	void OnWPolars();
	void OnExporttoAVL();
	void OnWOpps();
	void On3DView();
	void OnCpView();
	void OnNewWing();
	void OnNewPlane();
	void OnEditUFO();
	void OnAnalyze();
	void OnCurveStyle(int index);
	void OnCurveWidth(int index);
	void OnCurveColor();
	void OnHighlightWOpp();
	void OnShowCurve();
	void OnHideAllWPolars();
	void OnShowAllWPolars();
	void OnSingleGraph1();
	void OnSingleGraph2();
	void OnSingleGraph3();
	void OnSingleGraph4();
	void OnTwoGraphs();
	void OnFourGraphs();
	void OnExportCurWOpp();
	void OnCurWOppOnly();
	void OnDeleteAllWPlrOpps();
	void OnHideAllWPlrOpps();
	void OnShowAllWPlrOpps();
	void OnDeleteAllWOpps();
	void OnHideAllWOpps();
	void OnShowAllWOpps();
	void OnRenameCurUFO();
	void OnRenameCurWPolar();
	void OnDeleteCurUFO();
	void OnDeleteCurWPolar();
	void OnExportCurWPolar();
	void OnResetCurWPolar();
	void OnDeleteCurWOpp();
	void OnDefineWPolar();
	void OnDefineStabPolar();
	void OnEditCurWPolar();
	void OnDeleteUFOWPolars();
	void OnDuplicateCurUFO();
	void OnAnimateWOpp();
	void OnAnimateWOppSingle();
	void OnAnimateWOppSpeed(int val);
	void OnAnimateModeSingle(bool bStep=true);
	void OnSequence();
	void OnInitLLTCalc();
	void OnStoreWOpp();
	void OnHalfWing();
	void OnPanelForce();
	void OnScaleWing();
	void OnShowLift();
	void OnShowIDrag();
	void OnShowVDrag();
	void OnShowTransitions();
	void OnAdjustToWing();
	void OnAdvancedSettings();
	void OnHideUFOWPolars();
	void OnShowUFOWPolars();
	void OnHideUFOWOpps();
	void OnShowUFOWOpps();
	void OnDeleteUFOWOpps();
	void OnResetWOppLegend();
	void OnResetWPlrLegend();
	void OnNewBody();
	void OnEditCurBody();
	void OnImportBody();
	void OnExportBodyDef();
	void OnExportBodyGeom();
	void OnManageBodies();
	void OnAxes();
	void On3DCp();
	void On3DIso();
	void On3DTop();
	void On3DLeft();
	void On3DFront();
	void On3DReset();
	void On3DPickCenter();
	void OnSetupLight();
	void OnClipPlane(int pos);
	void OnLight();
	void OnSurfaces();
	void OnOutline();
	void OnPanels();
	void OnVortices();
	void OnFoilNames();
	void OnMasses();
	void OnDownwash();
	void OnMoment();
	void OnResetWingScale();
	void OnStreamlines();
	void OnSurfaceSpeeds();
	void OnCpSection(int pos);
	void OnKeepCpSection();
	void OnResetCpSection();
	void OnCpPosition();
	void OnGraphSettings();
	void OnManageUFOs();
	void OnPolarFilter();
	void OnLiftScale(int pos);
	void OnDragScale(int pos);
	void OnVelocityScale(int pos);
	void OnGL3DScale();
	void OnImportWPolar();
	void OnUFOInertia();
	void OnWPolarProps();
	void OnWOppProps();
	void OnReadAnalysisData();

	void OnRootLocusView();
	void OnModalView();
	void OnTimeView();

public:
	void contextMenuEvent (QContextMenuEvent * event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent (QWheelEvent *event);
	void showEvent(QShowEvent *event);
	void mouseDoubleClickEvent (QMouseEvent *event);

	void GLDrawFoils();
	void GLDrawMasses();
	void GLInverseMatrix();
	void GLRenderView();
	void GLCallViewLists();
	void GLDraw3D();
	void Set3DRotationCenter();
	void Set3DRotationCenter(QPoint point);

	CWPolar* AddWPolar(CWPolar* pWPolar);
	CWPolar* GetWPolar(QString WPolarName);
	CWing* AddWing(CWing *pWing);
	CWOpp* GetWOpp(double x);
	CPOpp* GetPOpp(double x);
	CBody* AddBody(CBody *pBody);

	CPlane * AddPlane(CPlane *pPlane);
	CPlane * GetPlane(QString PlaneName);
	CWing * GetWing(QString WingName);
	CBody *GetBody(QString BodyName);
	CWOpp* InsertWOpp(CWOpp *pNewPoint);
	void AddWOpp(double QInf, double Alpha, bool bPointOut, double *Gamma = NULL, double *Sigma = NULL, double *Cp = NULL);
	void AddPOpp(bool bPointOut, double *Cp, double *Gamma = NULL, double *Sigma=NULL, CPOpp *pPOpp = NULL);
	void SetUFO(QString UFOName="", bool bNoPolar=false);
	void SetWPlr(bool bCurrent = true, QString WPlrName = "");
	void DeleteBody(CBody *pThisBody);
	void SnapClient(QString const &FileName);
	void SetControlPositions(CPanel *pPanel, CVector *pNode, double t, int &NCtrls, QString &out, bool bBCOnly=true);
	void RotateGeomY(double const &Angle, CVector const &P);

	void DuplicatePlane();
	void LLTAnalyze(double V0, double VMax, double VDelta, bool bSequence, bool bInitCalc);
	void UpdateUnits();
	void SetWGraphTitles(Graph* pGraph);
	void SetStabGraphTitles();
	void SetWingLegendPos();
	void SetupLayout();
	void SetControls();
	void SetViewControls();
	void CreateCpCurves();
	void CreateWPolarCurves();
	void CreateWOppCurves();
	void CreateStabilityCurves();
	void CreateStabRLCurves();
	void CreateStabTimeCurves();
	void CreateStabRungeKuttaCurves();
	void SetWGraphScale();
	void FillWPlrCurve(CCurve *pCurve, CWPolar *pWPolar, int XVar, int YVar);
	void FillWOppCurve(CWOpp *pWOpp, Graph *pGraph, CCurve *pCurve);
	void FillStabCurve(CCurve *pCurve, CWPolar *pWPolar, int iMode);
	void JoinSurfaces(CSurface *pLeftSurf, CSurface *pRightSurf, int pl, int pr);
	void SetScale();
//	void SetScale(QRect CltRect);
	void Set2DScale();
	void Set3DScale();
	void DrawCpLegend(QPainter &painter, QPoint place, int bottom);
	void DrawWOppLegend(QPainter &painter, QPoint place, int bottom);
	void DrawWPolarLegend(QPainter &painter, QPoint place, int bottom);
	void DrawStabTimeLegend(QPainter &painter, QPoint place, int bottom);
	void PaintXCmRef(QPainter &painter, QPoint ORef, double scale);
	void PaintXCP(QPainter &painter, QPoint ORef, double scale);
	void PaintXTr(QPainter &painter, QPoint ORef, double scale);
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
	void RotateGeomZ(CPanel *pPanel, CVector *pNode, CPanel *pWakePanel, CVector *pWakeNode, double const &Beta, CVector const &P);
	void CreateWOpp(CWOpp *pWOpp, CWing *pWing);
	void EditCurPlane();
	void PaintWingLegend(QPainter &painter);
	void PaintCurWOppLegend(QPainter &painter);

	int CreateBodyElements();
	int CreateWingElements(CSurface *pSurface);
	int IsWakeNode(CVector &Pt);
	int IsNode(CVector &Pt);

	double GetCoreSize();
	void SetCoreSize(double CoreSize);

	QString UFOName();

//	double GetZeroLiftAngle(CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau);
//	double GetVar(int nVar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Cl, double Tau, bool &bOutRe, bool &bError);
//	void GetLinearizedPolar(CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau, double &Alpha0, double &Slope);
//	void * GetPlrVariable(CPolar *pPolar, int iVar);

	bool InitializePanels();
	bool CreateWakeElems(int PanelIndex);
	bool Intersect(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB, CVector const &Normal, CVector const &A,  CVector const &U,  CVector &I, double &dist);
	bool SetModWing(CWing *pWing);
	bool SetModPlane(CPlane *pModPlane);
	bool SetModBody(CBody *pModBody);
	bool SetModWPolar(CWPolar *pModWPolar);
	bool LoadSettings(QSettings *pSettings);
	bool SaveSettings(QSettings *pSettings);
	bool SetWOpp(bool bCurrent, double x = 0.0);
	bool SetPOpp(bool bCurrent, double x = 0.0);

	bool VLMIsSameSide(int p, int pp) {return m_Panel[p].m_Pos==m_Panel[pp].m_Pos;};

	QString RenameUFO(QString UFOName);
	QGraph* GetGraph(QPoint &pt);

//____________________Variables______________________________________
//
private:

	//the dialog boxes are declared as member variables to enable translations... seems to be a Qt bug
	WAdvancedDlg m_WAdvancedDlg;
	ManageBodiesDlg m_ManageBodiesDlg;
	ManageUFOsDlg m_ManageUFOsDlg;
	PolarFilterDlg m_PolarFilterDlg;
	InertiaDlg m_InertiaDlg;
	PlaneDlg m_PlaneDlg;
	ModDlg m_ModDlg;
	RenameDlg m_RenameDlg;
	EditPlrDlg m_EditPlrDlg;
	GraphDlg m_GraphDlg;
	WingScaleDlg m_WingScaleDlg;
	W3dPrefsDlg m_W3dPrefsDlg;

	QTimer *m_pTimerWOpp, *m_pTimerMode;

	QPushButton *m_pctrlKeepCpSection, *m_pctrlResetCpSection;
	QSlider *m_pctrlCpSectionSlider;
	FloatEdit *m_pctrlSpanPos;
	QCheckBox *m_pctrlSequence;
	FloatEdit *m_pctrlAlphaMin;
	FloatEdit *m_pctrlAlphaMax;
	FloatEdit *m_pctrlAlphaDelta;
	QCheckBox *m_pctrlInitLLTCalc;
	QCheckBox *m_pctrlStoreWOpp;
	QPushButton *m_pctrlAnalyze;

	QCheckBox *m_pctrlPanelForce, *m_pctrlLift, *m_pctrlIDrag, *m_pctrlVDrag, *m_pctrlTrans, *m_pctrlWOppAnimate;
	QSlider *m_pctrlAnimateWOppSpeed;
	QCheckBox *m_pctrlMoment,  *m_pctrlDownwash, *m_pctrlCp,*m_pctrlSurfVel, *m_pctrlStream;

	QCheckBox *m_pctrlShowCurve;
	QCheckBox *m_pctrlShowPoints;
	LineCbBox *m_pctrlCurveStyle;
	LineCbBox *m_pctrlCurveWidth;
	LineButton *m_pctrlCurveColor;
	LineDelegate *m_pStyleDelegate, *m_pWidthDelegate;

	QCheckBox *m_pctrlAxes, *m_pctrlLight, *m_pctrlSurfaces, *m_pctrlOutline, *m_pctrlPanels;
	QCheckBox *m_pctrlFoilNames, *m_pctrlVortices, *m_pctrlMasses;

	QAction *m_pXView, *m_pYView, *m_pZView, *m_pIsoView;
	QToolButton *m_pctrlX, *m_pctrlY, *m_pctrlZ, *m_pctrlIso;

	QPushButton *m_pctrlReset, *m_pctrlPickCenter;
	QSlider *m_pctrlClipPlanePos;

	//stability widgets
	QTextEdit *m_pctrlPolarProps1;
	QStackedWidget *m_pctrBottomControls, *m_pctrlMiddleControls;


	ArcBall m_ArcBall;

	QPoint m_LastPoint, m_PointDown;

	CVector m_UFOOffset;
	CVector m_glViewportTrans;// the translation vector in gl viewport coordinates
	CVector m_glRotCenter;    // the center of rotation in object coordinates... is also the opposite of the translation vector


	int m_CurveStyle, m_CurveWidth;
	QColor m_CurveColor;
	bool m_bCurveVisible, m_bCurvePoints;

	GL3dWingDlg  m_WingDlg;
	GL3dBodyDlg  m_GL3dBody;
	GLLightDlg   m_GLLightDlg;


private:
	QLabel *m_pctrlUnit1, *m_pctrlUnit2, *m_pctrlUnit3;
protected:

	CVector m_Node[2*VLMMAXMATSIZE];		// the node array for the currently loaded UFO
	CVector m_TempWakeNode[2*VLMMAXMATSIZE];	// the temporary wake node array during relaxation calc

	double m_aij[VLMMAXMATSIZE*VLMMAXMATSIZE];    // coefficient matrix
	double m_aijRef[VLMMAXMATSIZE*VLMMAXMATSIZE]; // coefficient matrix
	double m_RHS[VLMMAXMATSIZE*VLMMAXRHS];			// RHS vector
	double m_RHSRef[VLMMAXMATSIZE*VLMMAXRHS];		// RHS vector

	CVector m_L[(MAXBODYFRAMES+1)*(MAXSIDELINES+1)]; //temporary points to save calculation times for body NURBS surfaces
	CVector m_T[(MAXBODYFRAMES+1)*(MAXSIDELINES+1)];

	CSurface *m_pSurface[8*MAXSPANSECTIONS];	// An array with the pointers to the wings surfaces

	QList<void *> *m_poaFoil;			// a pointer to the foil array
	QList<void *> *m_poaPolar;			// a pointer to the foil polar array
	QList<void *> *m_poaWing;			// a pointer to the wing array
	QList<void *> *m_poaPlane;			// a pointer to the plane array
	QList<void *> *m_poaWPolar;			// a pointer to the UFO polar array
	QList<void *> *m_poaWOpp;			// a pointer to the UFO OpPoint array
	QList<void *> *m_poaPOpp;			// a pointer to the Plane OpPoint array
	QList<void *> *m_poaBody;			// a pointer to the Body array


	bool m_bArcball;                   //true if the arcball is to be displayed
	bool m_bCrossPoint;                //true if the control point on the arcball is to be displayed
	bool m_bPickCenter;                //true if the user is in the process of picking a new center for OpenGL display
	bool m_b3DCp;                      // true if the Cp Colors are to be displayed on the 3D openGl view
	bool m_bDownwash;                  // true if the arrows represeting downwash are to be displayed on the 3D openGl view
	bool m_bMoments;                   // true if the arrows representing moments are to be displayed on the 3D openGl view
	bool m_bFoilNames;                 // true if the foil names are to be displayed on the openGL 3D view
	bool m_bShowMasses;                // true if the point masses are to be displayed on the openGL 3D view
	bool m_bVortices;				// defines whether the corresponfing data should be displayed
	bool m_bSurfaces, m_bOutline, m_bAxes, m_bVLMPanels; // true if the surfaces, outline, axes or panels are to be displayed
	bool m_bXTop, m_bXBot, m_bXCP;     // defines whether the corresponfing data should be displayed
	bool m_bHighlightOpp;              // true if the currently selected operating point is to be highlighted on the polar graph
	bool m_bPanelForce;
	bool m_bBreak;

	bool m_bResetglGeom;			// true if the geometry OpenGL list needs to be re-generated
	bool m_bResetglMesh;			// true if the mesh OpenGL list needs to be re-generated
	bool m_bResetglWake;			// true if the wake OpenGL list needs to be re-generated
	bool m_bResetglOpp, m_bResetglLift, m_bResetglDrag, m_bResetglDownwash;			// true if the OpenGL lists need to be re-generated
	bool m_bResetglPanelForce, m_bResetglPanelCp;
	bool m_bResetglStream;			// true if the streamlines OpenGL list needs to be re-generated
	bool m_bResetglLegend;          //needs to be reset is window has been resized
	bool m_bResetglBody;          // true if the openGL list for the body needs to be re-generated
	bool m_bResetglBodyMesh;      // true if the openGL list for panel mesh needs to be re-generated
	bool m_bResetglFlow;          // true if the crossflow OpenGL list needs to be refreshed
	bool m_bWakePanels;           // true if the Cp Scale in Miarex is to be displayed
	bool m_bShowCpScale;          // true if the Cp Scale in Miarex is to be displayed
	bool m_bIs2DScaleSet;         // true if the 3D scale has been set, false if needs to be reset
	bool m_bIs3DScaleSet;         // true if the 3D scale has been set, false if needs to be reset
	bool m_bAutoScales;
	bool m_bXPressed, m_bYPressed; //true if the corresponding key is pressed

	bool m_bVLM1;
	bool m_bTrans;				// the view is being dragged
	bool m_bType1, m_bType2, m_bType4, m_bType5, m_bType6, m_bType7;	// polar types to be displayed
	bool m_bShowElliptic;			// true if the elliptic loading should be displayed in the local lift graph
	bool m_bStoreWOpp;			// true if the WOpp should be stored after a calculation
	bool m_bKeepOutOpps;			// true if points out of hte polar mesh should be kept
	bool m_bCurWOppOnly;			// true if only the current WOpp is to be displayed
	bool m_bCurFrameOnly;			// true if only the currently selected body frame is to be displayed
	bool m_bHalfWing;			// true if only a half-wing should be displayed in the OpPoint view
	bool m_bAnimateWOpp;			// true if there is an animation going on for an operating point
	bool m_bAnimateMode;			// true if there is an animation going on for a Mode
	bool m_bAnimateWOppPlus;		// true if the animation is going in aoa crescending order
	bool m_bTransGraph;			// true if a graph is being dragged

	bool m_bShowWingCurve[MAXWINGS];
//	bool m_bShowWing2;			// true if the biplane's second wing OpPoint results should be displayed
//	bool m_bShowStab;			// true if the stabilisator (elevator) OpPoint results should be displayed
//	bool m_bShowFin;			// true if the fin OpPoint results should be displayed
	bool m_bLogFile;			// true if the log file warning is turned on
	bool m_bResetWake;

	bool m_bSetNewWake;			// true if the wake needs to be reset
	bool m_bDirichlet;			// true if Dirichlet BC are applied in 3D panel analysis, false if Neumann
	bool m_bTrefftz;			// true if the induced drag should be calculated in the Trefftz plane, false if calculated by summation over panels
	bool m_bXCmRef; 	// defines whether the corresponfing data should be displayed
	bool m_bSequence;
	bool m_bInitLLTCalc;
//	bool m_bForcedResponse;     // true if the time graph view is the forced response, false if initial conditions response
	int m_StabilityResponseType; //0 = initial conditions, 1=forced response, 2=modal response

	int m_NSurfaces;
	int m_iWingView;			// defines how many graphs will be displayed in WOpp view
	int m_iWPlrView;			// defines how many graphs will be displayed in WPolar view
	int m_iStabilityView;			// defines which, and how many, graphs will be displayed in Stability view
	int m_iStabTimeView;         // defines whether one or four graphs will be  displayed in time response view
	int m_XW1, m_YW1, m_XW2, m_YW2, m_XW3, m_YW3, m_XW4, m_YW4; 	// the WPolar graph variables
	int m_Iter ;				// the number of iterations for LLT
	int m_NStation ;			// the number of stations for LLT
	int m_posAnimateWOpp;			// the current animation aoa index for WOpp animation
	int m_posAnimateMode;			// the current animation aoa index for Mode animation
	int m_NWakeColumn;			// number of wake columns
	int m_MaxWakeIter;			// wake roll-up iteration limit
	int m_WakeInterNodes;		// number of intermediate nodes between wake panels
	int m_InducedDragPoint;		// 0 if dwonwash is at panel's centroid, 1 if averaged over panel length //used in CWing::VLMTrefftz

	int m_iView;


	double m_ClipPlanePos;
	double MatIn[4][4], MatOut[4][4];


	double m_glTop, m_HorizontalSplit, m_VerticalSplit;//screen split ratio for body 3D view
	double m_GLScale;	// the OpenGl scale for the view frustrum with respect to the windows device context
						// this is not the scale to display the model in the OpenGL view

	double m_CurSpanPos;		//Span position for Cp Grpah
	double m_CoreSize;			// core size for VLM vortices
	double m_MinPanelSize;			// wing minimum panel size ; panels of less length are ignored
	double m_WingScale;			// scale for 2D display
	double m_LastWOpp;			// last WOPP selected, try to set the same if it exists, for the new polar

	//the min/max/delta values for sequential analysis, depending on the polar type
	double m_AlphaMin, m_AlphaMax, m_AlphaDelta;
	double m_QInfMin, m_QInfMax, m_QInfDelta;
	double m_ControlMin, m_ControlMax, m_ControlDelta;

	// mode animation data
	double m_ModeState[6];
	double m_ModeNorm, m_ModeTime, m_Modedt;
	
	// time curve data
	double m_TimeInput[4];
	double m_TotalTime, m_Deltat;
	double m_RampTime, m_RampAmplitude;


	QFile* m_pXFile;			// a pointer to the output .log file

	QPoint m_ptOffset;			// client offset position for wing display
	QPoint m_WPlrLegendOffset;		// client offset position for wing polar legend
	QPoint m_WingLegendOffset;		// client offset position for WOPP polar legend

	QRect m_rSingleRect;			// the drawing rectangle for single graph view

	QGraph m_WingGraph[4];			// the WOpp graphs
	QGraph m_WPlrGraph[4];			// the WPolar graphs
	QGraph  m_TimeGraph[4];				// time response in stability view
	QGraph m_CpGraph;			// cross flow Cp Graph in 3D panel analysis
	QGraph m_LongRLGraph;			// root locus graph for Longitudinal modes
	QGraph m_LatRLGraph;			// root locus graph for Longitudinal modes
	QGraph* m_pCurGraph;			// currently active graph
	QGraph* m_pCurWPlrGraph;			// currently active WPolar graph
	QGraph* m_pCurWingGraph;			// currently active WOpp graph
	QGraph* m_pCurRLStabGraph;      // currently active Root Locus Graph
	QGraph* m_pCurTimeGraph;			// currently active time graph

	CWing *m_pCurWing;             // the currently selected wing
	CWPolar * m_pCurWPolar;        // the currently selected WPolar
	CWOpp * m_pCurWOpp;			 // the currently selected Wing Operating Point
	CPlane * m_pCurPlane;          // the currently selected Plane
	CBody *m_pCurBody;             // the currently selected body


	CVector P,W,V,T;
	
public:
	int m_GLList;
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

	int m_StreamLinesStyle, m_StreamLinesWidth;
	QColor m_StreamLinesColor;

	QColor m_MassColor;

	QColor m_CpColor;
	int m_CpStyle, m_CpWidth;
	bool m_bShowCp, m_bShowCpPoints;
	bool m_bglLight;
	bool m_bAutoCpScale;		//true if the Cp scale should be set automatically
	bool m_bLongitudinal;         // true if longitudinal stability results are to be displayed, false if lateral
	bool m_bICd, m_bVCd, m_bStream, m_bSpeeds;  	// defines whether the corresponfing data should be displayed

	static void *s_pMainFrame;          // a pointer to the frame class
	static void *s_p2DWidget;
	static void *s_pGLWidget;

	int m_MatSize;			    // the matrix size
	int m_WakeSize;				// Size of the Matrix if there is a wake
	int m_nNodes;				// the current number of nodes for the currently loaded UFO
	int m_nWakeNodes;			// Size of the node array if there is a wake

	double m_LegendMin, m_LegendMax;                   //minimum value of hte Cp scale in 3D view
	double m_LiftScale, m_DragScale, m_VelocityScale;  //maximum value of hte Cp scale in 3D view
	double m_glScaled; //zoom factor for UFO

	QRect m_r2DCltRect, m_r3DCltRect; // the client and drawing rectangle
	CPOpp * m_pCurPOpp;			// the currently selected Plane Operating Point
	LLTAnalysisDlg *m_pLLTDlg;                  // the dialog class which manages the LLT calculations
	PanelAnalysisDlg *m_pPanelDlg;              // the dialog class which manages the Panel calculations
	CVector m_MemNode[2*VLMMAXMATSIZE];         // used if the analysis should be performed on the tilted geometry
	CVector m_WakeNode[2*VLMMAXMATSIZE];        // the reference current wake node array
	CVector m_RefWakeNode[2*VLMMAXMATSIZE];     // the reference wake node array if wake needs to be reset
	CPanel m_Panel[VLMMAXMATSIZE];		        // the panel array for the currently loaded UFO
	CPanel m_MemPanel[VLMMAXMATSIZE];           // used if the analysis should be performed on the tilted geometry
	CPanel m_WakePanel[VLMMAXMATSIZE];          // the reference current wake panel array
	CPanel m_RefWakePanel[VLMMAXMATSIZE];       // the reference wake panel array if wake needs to be reset

	CWing *m_pWingList[MAXWINGS];         // pointers to the four wings of the currently selected plane
	CWOpp *m_pWOpp[MAXWINGS];             // pointers to the operating points of the four wings of the currently selected plane

};

#endif // QMIAREX_H
