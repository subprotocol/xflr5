/****************************************************************************

	Miarex    Copyright (C) 2008-2013 Andre Deperrois adeperrois@xflr5.com

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

/**
 *@file
 *
 * This file implementsthe QMiarex class associated to the GUI of 3D analysis
 *
 */


#ifndef QMIAREX_H
#define QMIAREX_H

#include "../params.h"
#include <QWidget>
#include <QPixmap>
#include <QCheckBox>
#include <QSlider>
#include <QStackedWidget>
#include <QToolButton>
#include <QPushButton>
#include <QRadioButton>
#include <QList>
#include <QLabel>
#include <QDialog>
#include <QDataStream>
#include <QSettings>
#include "../objects/ArcBall.h"
#include "PanelAnalysisDlg.h"
#include "LLTAnalysisDlg.h"
#include "../misc/LineBtn.h"
#include "../misc/DoubleEdit.h"
#include "../misc/LineCbBox.h"
#include "../misc/LineDelegate.h"
#include "../objects/Body.h"
#include "../objects/Wing.h"
#include "../objects/Plane.h"
#include "../objects/WPolar.h"
#include "../objects/WingOpp.h"
#include "../objects/PlaneOpp.h"
#include "../objects/Polar.h"
#include "../objects/Foil.h"
#include "../objects/OpPoint.h"
#include "../graph/QGraph.h"


/** This enumeration defines the options for the active view.
 *May be the operaring point view, the polar view, the 3D view, the Cp view, or the stability view*/
typedef enum {WOPPVIEW, WPOLARVIEW, W3DVIEW, WCPVIEW, WSTABVIEW} enumMiarexViews;

/** In the case of a stability view, this enumeration defines the display options.
 *May be the a time graph view, a root locus view, or a 3D view, */
typedef enum {STABTIMEVIEW, STABPOLARVIEW, STAB3DVIEW} enumStabilityViews;




/**
 *@class QMiarex
 *@brief This is the class associated to the 3D calculations

  It is the handling class for the Miarex toolbar and it dispatches user commands towards
  object definition, analysis and post-processing.
  The main methods of this class are:
	- Management of wings, planes, polars, and operating points
	- Construction of the panels for 3D calculations
	- Management of the display
	- Management of the LLT and Panel Analysis,
	- Mapping of the analyis results to the operating point and polar objects
*/
class QMiarex : public QWidget
{
	friend class MainFrame;
	friend class ThreeDWidget;
	friend class TwoDWidget;
	friend class GL3DScales;
	friend class GL3dBodyDlg;
	friend class GLLightDlg;
	friend class WingDlg;
	friend class Wing;
	friend class LLTAnalysisDlg;
	friend class StabPolarDlg;
	friend class StabViewDlg;
	friend class PanelAnalysisDlg;
	friend class Plane;
	friend class PlaneDlg;
	friend class ManageBodiesDlg;
	friend class ManageUFOsDlg;
	friend class GL3dWingDlg;
	friend class DisplaySettingsDlg;
	friend class UFOTableDelegate;
	friend class WPolarDlg;
	friend class WPolar;

	Q_OBJECT


public:

	QMiarex(QWidget *parent = NULL);
	~QMiarex();


private slots:

	void On3DCp();
	void On3DIso();
	void On3DTop();
	void On3DLeft();
	void On3DFront();
	void On3DPrefs();
	void On3DReset();
	void On3DPickCenter();
	void On3DView();
	void OnAdjustToWing();
	void OnAdvancedSettings();
	void OnAllWingGraphScales();
	void OnAllWingGraphSettings();
	void OnAllWPolarGraphScales();
	void OnAllWPolarGraphSettings();
	void OnAnalyze();
	void OnAnimateWOpp();
	void OnAnimateWOppSingle();
	void OnAnimateWOppSpeed(int val);
	void OnAnimateModeSingle(bool bStep=true);
	void OnAxes();
	void OnClipPlane(int pos);
	void OnCpSection(int pos);
	void OnCpPosition();
	void OnCpView();
	void OnCurveColor();
	void OnCurveStyle(int index);
	void OnCurveWidth(int index);
	void OnCurWOppOnly();
	void OnDefineStabPolar();
	void OnDefineWPolar();
	void OnDeleteAllWOpps();
	void OnDeleteAllWPlrOpps();
	void OnDeleteCurUFO();
	void OnDeleteCurWOpp();
	void OnDeleteCurWPolar();
	void OnDeleteUFOWOpps();
	void OnDeleteUFOWPolars();
	void OnDownwash();
	void OnDragScale(int pos);
	void OnDuplicateCurUFO();
	void OnEditCurBody();
	void OnEditCurWPolar();
	void OnEditUFO();
	void OnExportBodyDef();
	void OnExportBodyGeom();
	void OnExporttoAVL();
	void OnExportCurWOpp();
	void OnExportCurWPolar();
	void OnFinCurve();
	void OnFoilNames();
	void OnFourGraphs();
	void OnGL3DScale();
	void OnGraphSettings();
	void OnHalfWing();
	void OnHideAllWOpps();
	void OnHideAllWPlrOpps();
	void OnHideAllWPolars();
	void OnHideUFOWOpps();
	void OnHideUFOWPolars();
	void OnHighlightWOpp();
	void OnImportBody();
	void OnInitLLTCalc();
	void OnLiftScale(int pos);
	void OnLight();
	void OnImportWPolar();
	void OnKeepCpSection();
	void OnManageBodies();
	void OnManageUFOs();
	void OnMasses();
	void OnModalView();
	void OnMoment();
	void OnNewBody();
	void OnNewPlane();
	void OnNewWing();
	void OnOutline();
	void OnPanelForce();
	void OnPanels();
	void OnPolarFilter();
	void OnReadAnalysisData();
	void OnRenameCurUFO();
	void OnRenameCurWPolar();
	void OnResetCpSection();
	void OnResetWingGraphScale();
	void OnResetWOppLegend();
	void OnResetWPlrLegend();
	void OnRootLocusView();
	void OnResetCurWPolar();
	void OnResetWingScale();
	void OnSetupLight();
	void OnScaleWing();
	void OnSequence();
	void OnShowAllWOpps();
	void OnShowAllWPlrOpps();
	void OnShowAllWPolars();
	void OnShowCurve();
	void OnShowEllipticCurve();
	void OnShowLift();
	void OnShowIDrag();
	void OnShowTransitions();
	void OnShowUFOWPolars();
	void OnShowUFOWPolarsOnly();
	void OnShowUFOWOpps();
	void OnShowVDrag();
	void OnShowXCmRef();
	void OnSingleGraph1();
	void OnSingleGraph2();
	void OnSingleGraph3();
	void OnSingleGraph4();
	void OnStabCurve();
	void OnStabilityDirection();
	void OnStoreWOpp();
	void OnStreamlines();
	void OnSurfaces();
	void OnSurfaceSpeeds();
	void OnTimeView();
	void OnTwoGraphs();
	void OnUFOInertia();
	void OnVelocityScale(int pos);
	void OnVortices();
	void OnWing2Curve();
	void OnWOppProps();
	void OnWOpps();
	void OnWPolars();
	void OnWPolarProps();

public:
	//overrides
	void contextMenuEvent (QContextMenuEvent * event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mouseDoubleClickEvent (QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void showEvent(QShowEvent *event);
	void wheelEvent (QWheelEvent *event);


public:
	//class methods
	Body*   AddBody(Body *pBody);
	Plane*  AddPlane(Plane *pPlane);
	void     AddPOpp(bool bPointOut, double *Cp, double *Gamma = NULL, double *Sigma=NULL, PlaneOpp *pPOpp = NULL);
	Wing*   AddWing(Wing *pWing);
	void     AddWOpp(double QInf, double Alpha, bool bPointOut, double *Gamma = NULL, double *Sigma = NULL, double *Cp = NULL);
	WPolar* AddWPolar(WPolar* pWPolar);
	void   Connect();
	double coreSize();
	int  CreateBodyElements();
	void CreateCpCurves();
	void CreateWPolarCurves();
	void CreateWOppCurves();
	void CreateStabilityCurves();
	void CreateStabRLCurves();
	void CreateStabTimeCurves();
	void CreateStabRungeKuttaCurves();
	bool CreateWakeElems(int PanelIndex);
	int  CreateWingElements(Surface *pSurface);
	void CreateWOpp(WingOpp *pWOpp, Wing *pWing);
	void DeleteBody(Body *pThisBody);
	void DrawCpLegend(QPainter &painter, QPoint place, int bottom);
	void DrawWOppLegend(QPainter &painter, QPoint place, int bottom);
	void DrawWPolarLegend(QPainter &painter, QPoint place, int bottom);
	void DrawStabTimeLegend(QPainter &painter, QPoint place, int bottom);
	void DuplicatePlane();
	void EditCurPlane();
	void FillComboBoxes(bool bEnable = true);
	void FillWPlrCurve(Curve *pCurve, WPolar *pWPolar, int XVar, int YVar);
	void FillWOppCurve(WingOpp *pWOpp, Graph *pGraph, Curve *pCurve);
	void FillStabCurve(Curve *pCurve, WPolar *pWPolar, int iMode);
	Body*   GetBody(QString BodyName);
	QGraph*  GetGraph(QPoint &pt);
	Plane*  GetPlane(QString PlaneName);
	PlaneOpp*   GetPOpp(double x);
	Wing*   GetWing(QString WingName);
	WingOpp*   GetWOpp(double x);
	WPolar* GetWPolar(QString WPolarName);
	void GLCallViewLists();
	void GLDraw3D();
	void GLDrawFoils();
	void GLDrawMasses();
    void GLDrawWOppLegend(Wing *pWing, WingOpp *pWOpp);
    void GLDrawWingLegend(Wing *pWing, Plane *pPlane, WPolar *pWPolar);
	void GLInverseMatrix();
	void GLRenderView();
	bool InitializePanels();
	void InsertWOpp(WingOpp *pNewPoint);
	bool Intersect(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB, CVector const &Normal, CVector const &A,  CVector const &U,  CVector &I, double &dist);
	int  IsNode(CVector &Pt);
	int  IsWakeNode(CVector &Pt);
	void JoinSurfaces(Surface *pLeftSurf, Surface *pRightSurf, int pl, int pr);
	void LLTAnalyze(double V0, double VMax, double VDelta, bool bSequence, bool bInitCalc);	
	bool LoadSettings(QSettings *pSettings);
	void PaintXCmRef(QPainter &painter, QPoint ORef, double scale);
	void PaintXCP(QPainter &painter, QPoint ORef, double scale);
	void PaintXTr(QPainter &painter, QPoint ORef, double scale);
	void PaintView(QPainter &painter);
	void PaintWing(QPainter &painter, QPoint ORef, double scale);
	void PanelAnalyze(double V0, double VMax, double VDelta, bool bSequence);
	void PaintWingLegend(QPainter &painter);
	void PaintCurWOppLegend(QPainter &painter);
	void RenameUFO(QString UFOName);
	void RotateGeomY(double const &Angle, CVector const &P);
	void RotateGeomZ(Panel *pPanel, CVector *pNode, Panel *pWakePanel, CVector *pWakeNode, double const &Beta, CVector const &P);
	bool SaveSettings(QSettings *pSettings);
	void Set2DScale();
	void Set3DRotationCenter();
	void Set3DRotationCenter(QPoint point);
	void Set3DScale();
	void SetAnalysisParams();
	void SetControls();
	void SetControlPositions(Panel *pPanel, CVector *pNode, double t, int &NCtrls, QString &out, bool bBCOnly=true);
	void SetCoreSize(double CoreSize);
	void SetCurveParams();
	bool SetModBody(Body *pModBody);
	bool SetModPlane(Plane *pModPlane);
	bool SetModWing(Wing *pWing);
	bool SetModWPolar(WPolar *pModWPolar);
	bool SetPOpp(bool bCurrent, double x = 0.0);
	void SetScale();
	void SetStabGraphTitles();
	void SetUFO(QString UFOName="");
	void SetupLayout();
	void SetViewControls();
	void SetWGraphScale();
	void SetWGraphTitles(Graph* pGraph);
	void SetWingLegendPos();
	void SetWPlr(bool bCurrent = true, QString WPlrName = "");
	void SetWPlrLegendPos();
	bool SetWOpp(bool bCurrent, double x = 0.0);
	void SnapClient(QString const &FileName);
	void StopAnimate();
	QString UFOName();
	void UpdateCurve();
	void UpdateUnits();
	void UpdateView();


	/**
	 * Checks if two node defined by their indexes are on the same side (bottom, middle or top)
	 * @param p  the index of the first node
	 * @param pp the index of the second node
	 * @return true if the nodes are on the same side of the surfaces, false otherwise
	 */
	bool VLMIsSameSide(int p, int pp)
	{
		return m_Panel[p].m_Pos==m_Panel[pp].m_Pos;
	}

	bool Allocate(int &memsize);
	void Release();


//____________________Variables______________________________________
//

private:
	//  the dialog boxes are declared as member variables to enable translations... seems to be a Qt bug
    void *m_pWingDlg;
    void *m_pGL3dBody;
    void *m_pGLLightDlg;

public:
	LLTAnalysisDlg *m_pLLTDlg;                  /**< the dialog class which manages the LLT calculations */
	PanelAnalysisDlg *m_pPanelDlg;              /**< the dialog class which manages the Panel calculations */


	// Widget variables ... self explicit, not documented
	QPushButton *m_pctrlKeepCpSection, *m_pctrlResetCpSection;
	QSlider *m_pctrlCpSectionSlider;
	DoubleEdit *m_pctrlSpanPos;
	QCheckBox *m_pctrlSequence;
	DoubleEdit *m_pctrlAlphaMin;
	DoubleEdit *m_pctrlAlphaMax;
	DoubleEdit *m_pctrlAlphaDelta;
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
	LineBtn *m_pctrlCurveColor;
	LineDelegate *m_pStyleDelegate, *m_pWidthDelegate;

	QCheckBox *m_pctrlAxes, *m_pctrlLight, *m_pctrlSurfaces, *m_pctrlOutline, *m_pctrlPanels;
	QCheckBox *m_pctrlFoilNames, *m_pctrlVortices, *m_pctrlMasses;

	QAction *m_pXView, *m_pYView, *m_pZView, *m_pIsoView;
	QToolButton *m_pctrlX, *m_pctrlY, *m_pctrlZ, *m_pctrlIso;

	QPushButton *m_pctrlReset, *m_pctrlPickCenter;
	QSlider *m_pctrlClipPlanePos;

	QLabel *m_pctrlUnit1, *m_pctrlUnit2, *m_pctrlUnit3;

	//stability widgets
	QTextEdit *m_pctrlPolarProps1;
	QStackedWidget *m_pctrBottomControls, *m_pctrlMiddleControls;


public:

	static bool s_bOutline;                   /**< true if the surface outlines are to be displayed in the 3D view*/
	static bool s_bSurfaces;                  /**< true if the surfaces are to be displayed in the 3D view*/
	static bool s_bVLMPanels;                 /**< true if the panels are to be displayed in the 3D view*/
	static bool s_bAxes;                      /**< true if the axes are to be displayed in the 3D view*/
    static bool s_bShowMasses;                /**< true if the point masses are to be displayed on the openGL 3D view */
    static bool s_bFoilNames;                 /**< true if the foil names are to be displayed on the openGL 3D view */


	// Class variables

	ArcBall m_ArcBall;                 /**< the instance of the arcball object used for rotating the 3D view */

	bool m_b3DCp;                      /**< true if the Cp Colors are to be displayed on the 3D openGl view */
	bool m_bAnimateWOpp;               /**< true if there is an animation going on for an operating point */
	bool m_bAnimateMode;               /**< true if there is an animation going on for a Mode */
	bool m_bAnimateWOppPlus;           /**< true if the animation is going in aoa crescending order */
	bool m_bArcball;                   /**< true if the arcball is to be displayed */
	bool m_bAutoCpScale;		       /**< true if the Cp scale should be set automatically */
	bool m_bAutoScales;                /**< true if automatic scales are requested for the graphs */
	bool m_bCrossPoint;                /**< true if the control point on the arcball is to be displayed */
	bool m_bCurWOppOnly;               /**< true if only the current WOpp is to be displayed */
	bool m_bCurFrameOnly;              /**< true if only the currently selected body frame is to be displayed */
	bool m_bCurvePoints;               /**< true if the points of the active curve are to be displayed */
	bool m_bCurveVisible;              /**< true if the active curve is to be displayed */
	bool m_bDirichlet;                 /**< true if Dirichlet BC are applied in 3D panel analysis, false if Neumann */
	bool m_bDownwash;                  /**< true if the arrows represeting downwash are to be displayed on the 3D openGl view */
	bool m_bHalfWing;                  /**< true if only a half-wing should be displayed of the in the OpPoint view */
	bool m_bHighlightOpp;              /**< true if the currently selected operating point is to be highlighted on the polar graph */
	bool m_bICd;                       /**< true if the induced drag forces should be displayed in the operating point or 3D view*/
	bool m_bInitLLTCalc;               /**< true if the LLT parameters should be set to default prior to the analysis. Otherwise, the iterations will start at the results of the previous calculation */
	bool m_bIs2DScaleSet;              /**< true if the 3D scale has been set, false if needs to be reset */
	bool m_bIs3DScaleSet;              /**< true if the 3D scale has been set, false if needs to be reset */
	bool m_bKeepOutOpps;               /**< true if points out of the polar mesh should be kept */
	bool m_bLogFile;			       /**< true if the log file warning is turned on */
	bool m_bLongitudinal;              /**< true if longitudinal stability results are to be displayed, false if lateral */
	bool m_bMoments;                   /**< true if the arrows representing moments are to be displayed on the 3D openGl view */
	bool m_bPanelForce;                /**< true if the forces acting on the panels are to be displayed in the 3D view */
	bool m_bPickCenter;                /**< true if the user is in the process of picking a new center for OpenGL display */
	bool m_bResetglGeom;               /**< true if the geometry OpenGL list needs to be re-generated */
	bool m_bResetglMesh;               /**< true if the mesh OpenGL list needs to be re-generated */
	bool m_bResetglWake;               /**< true if the wake OpenGL list needs to be re-generated */
	bool m_bResetglOpp;                /**< true if the OpenGL lists need to be re-generated */
	bool m_bResetglLift;               /**< true if the OpenGL lists need to be re-generated */
	bool m_bResetglDrag;               /**< true if the OpenGL lists need to be re-generated */
	bool m_bResetglDownwash;           /**< true if the OpenGL lists need to be re-generated */
	bool m_bResetglPanelForce;         /**< true if the OpenGL lists need to be re-generated */
	bool m_bResetglPanelCp;            /**< true if the OpenGL lists need to be re-generated */
	bool m_bResetglStream;             /**< true if the streamlines OpenGL list needs to be re-generated */
	bool m_bResetglLegend;             /**< true if the legend needs to be reset if the window has been resized */
	bool m_bResetglBody;               /**< true if the openGL list for the body needs to be re-generated */
	bool m_bResetglBodyMesh;           /**< true if the openGL list for panel mesh needs to be re-generated */
	bool m_bResetglFlow;               /**< true if the crossflow OpenGL list needs to be refreshed */
	bool m_bResetWake;                 /**< true if the wake geometry should be reset to its default shape prior to the analysis */
	bool m_bSequence;                  /**< true if a sequential analysis is to be performed */
	bool m_bShowCp;                    /**< true if the active curve should be displayed in Cp view */
	bool m_bShowCpScale;               /**< true if the Cp Scale in Miarex is to be displayed */
	bool m_bShowCpPoints;              /**< true if the points of the active curve should be displayed in Cp view */
	bool m_bShowElliptic;              /**< true if the elliptic loading should be displayed in the local lift graph */
	bool m_bShowWingCurve[MAXWINGS];   /**< true if various plane's wing curves shoud be displayed*/
	bool m_bSpeeds;                    /**< true if the velocities should be displayed in the operating point or 3D view*/
	bool m_bStoreWOpp;                 /**< true if the WOpp should be stored after a calculation */
	bool m_bStream;                    /**< defines whether the streamlines should be displayed in the operating point or 3D view*/
	bool m_bTrans;                     /**< true if the view is being dragged */
	bool m_bTransGraph;	               /**< true if a graph is being dragged */
	bool m_bTrefftz;                   /**< true if the induced drag should be calculated in the Trefftz plane, false if calculated by summation over panels */
	bool m_bType1;                     /**< true if polars of type 1 are to be displayed */
	bool m_bType2;                     /**< true if polars of type 2 are to be displayed */
	bool m_bType4;                     /**< true if polars of type 4 are to be displayed */
	bool m_bType5;                     /**< true if polars of type 5 are to be displayed */
	bool m_bType6;                     /**< true if polars of type 6 are to be displayed */
	bool m_bType7;                     /**< true if polars of type 71 are to be displayed */
	bool m_bVCd ;                      /**< true if the viscous drag forces should be displayed in the operating point or 3D view*/
	bool m_bVLM1;                      /**< true if the horseshoe method is selected, false if the quad ring method is selected */
	bool m_bVortices;                  /**< true if the panel vortices should be displayed */
	bool m_bWakePanels;                /**< true if the Cp Scale in Miarex is to be displayed */
	bool m_bXBot;                      /**< true if the transition on the bottom surface should be displayed in the operating point or in 3D view*/
	bool m_bXCmRef; 	               /**< true if the position of the reference point for the moments should be displayed in the operating point view*/
	bool m_bXCP;                       /**< true if the lift curve should be displayed in the operating point or in the 3D view*/
	bool m_bXPressed;                  /**< true if the X key is pressed */
	bool m_bXTop;                      /**< true if the transition on the top surface should be displayed in the operating point or in 3D view */
	bool m_bYPressed;                  /**< true if the Y key is pressed */


	int m_MatSize;                     /**< the matrix size, wich is also the size of the panel array */
	int m_WakeSize;                    /**< the size of the wake matrix, if a wake is included in the analysis */
	int m_nNodes;                      /**< the number of nodes corresponding to the panel array */
	int m_nWakeNodes;                  /**< the number of nodes in the wake */

	double m_LegendMin;                /**< minimum value of the Cp scale in 3D view */
	double m_LegendMax;                /**< maximum value of the Cp scale in 3D view */
	double m_LiftScale;                /**< scaling factor for the lift display in 3D view */
	double m_VelocityScale;            /**< scaling factor for the velocity display in 3D view */
	double m_DragScale;                /**< scaling factor for the drag display in 3D view */
	double m_glScaled;                 /**< scaling factor for the object in the 3D view */

	QRect m_r2DCltRect;                /**< the client rectangle, in client coordinates. */
	QRect m_r3DCltRect;                /**< the drawing rectangle, in client coordinates .*/

	PlaneOpp * m_pCurPOpp;           /**< a pointer to the active Plane Operating Point, or NULL if none is active*/


	QPoint m_LastPoint;           /**< The client position of the previous mousepress event */
	QPoint m_PointDown;           /**< The client position of the current mousepress event */

	CVector m_glViewportTrans;    /**< the translation vector in gl viewport coordinates */
	CVector m_glRotCenter;        /**< the center of rotation in object coordinates... is also the opposite of the translation vector */
	CVector m_UFOOffset;          /**< The translation vector of objects in the 3D Open GL viewport */


	int m_CurveStyle;             /**< The style of the active curve */
	int m_CurveWidth;             /**< The width of the active curve */
	QColor m_CurveColor;          /**< The color of the active curve */


	QTimer *m_pTimerWOpp;         /**< A pointer to the timer which signals the animation in the operating point and 3D view */
	QTimer *m_pTimerMode;         /**< A pointer to the timer which signals the animation of the modes in the 3D view */

	CVector *m_Node;              /**< the node array for the currently loaded UFO*/
	CVector *m_MemNode;           /**< used if the analysis should be performed on the tilted geometry */
	CVector *m_WakeNode;          /**< the current wake node array */
	CVector *m_RefWakeNode;       /**< the reference wake node array if wake needs to be reset */

	Panel *m_Panel;               /**< the panel array for the currently loaded UFO */
	Panel *m_MemPanel;            /**< used if the analysis should be performed on the tilted geometry */
	Panel *m_WakePanel;           /**< the reference current wake panel array */
	Panel *m_RefWakePanel;        /**< the reference wake panel array if wake= new CVector needs to be reset */

	Wing *m_pWingList[MAXWINGS];  /**< an array of pointers to the four wings of the currently selected plane */
	WingOpp *m_pWOpp[MAXWINGS];   /**< an array of pointers to the operating points of the four wings of the currently selected plane */

	CVector m_L[(MAXBODYFRAMES+1)*(MAXSIDELINES+1)]; /**< Array of temporary points to save calculation times for body NURBS surfaces */
	CVector m_T[(MAXBODYFRAMES+1)*(MAXSIDELINES+1)]; /**< Array of temporary points to save calculation times for body NURBS surfaces */

	Surface *m_pSurface[8*MAXSPANSECTIONS];	/**< An array holding the pointers to the wings surfaces */

	QList<void *> *m_poaFoil;			/**< a pointer to the array of foil objects */
	QList<void *> *m_poaPolar;			/**< a pointer to the array of foil polar objects */
	QList<void *> *m_poaWing;			/**< a pointer to the array of wing objects */
	QList<void *> *m_poaPlane;			/**< a pointer to the array of plane objects */
	QList<void *> *m_poaWPolar;			/**< a pointer to the array of UFO polar objects */
	QList<void *> *m_poaWOpp;			/**< a pointer to the array of UFO OpPoint objects */
	QList<void *> *m_poaPOpp;			/**< a pointer to the array of Plane OpPoint objects */
	QList<void *> *m_poaBody;			/**< a pointer to the array of Body objects */


	Body *m_pCurBody;             /**< the currently active body */
	Wing *m_pCurWing;             /**< the currently active wing */
	WingOpp * m_pCurWOpp;			   /**< the currently active Wing Operating Point*/
	Plane * m_pCurPlane;          /**< the currently active Plane */
	WPolar * m_pCurWPolar;        /**< the currently active WPolar */


	int m_StabilityResponseType;   /**< 0 = initial conditions, 1=forced response, 2=modal response */

	enumStabilityViews m_iStabilityView;    /**< defines which, and how many, graphs will be displayed in Stability view */
	enumPolarGraphView m_iWPlrView;         /**< defines how many graphs will be displayed in WPolar view */

	int m_iStabTimeView;        /**< defines whether one or four graphs will be  displayed in time response view */
	int m_InducedDragPoint;		/**< 0 if downwash is at panel's centroid, 1 if averaged over panel length; used in CWing::VLMTrefftz */
	int m_Iter ;				/**< the number of iterations for LLT */
	int m_iWingView;			/**< defines how many graphs will be displayed in WOpp view */
	int m_MaxWakeIter;			/**< wake roll-up iteration limit */
	int m_NSurfaces;            /**< the number of surfaces which define the wings of the current wing or plane */
	int m_NStation ;			/**< the number of stations for LLT */
	int m_NWakeColumn;			/**< number of wake columns */
	int m_posAnimateWOpp;       /**< the current animation aoa ind ex for WOpp animation */
	int m_posAnimateMode;       /**< the current animation aoa index for Mode animation */
	int m_XW1; 	                /**< the index of x the variable for WPolar graph 1 */
	int m_XW2; 	                /**< the index of x the variable for WPolar graph 2 */
	int m_XW3; 	                /**< the index of x the variable for WPolar graph 3 */
	int m_XW4; 	                /**< the index of x the variable for WPolar graph 4 */
	int m_YW1; 	                /**< the index of y the variable for WPolar graph 1 */
	int m_YW2; 	                /**< the index of y the variable for WPolar graph 2 */
	int m_YW3; 	                /**< the index of y the variable for WPolar graph 3 */
	int m_YW4; 	                /**< the index of y the variable for WPolar graph 4 */
	int m_WakeInterNodes;		/**< number of intermediate nodes between wake panels */

	enumMiarexViews m_iView;    /**< defines the currently active view */


	double m_ClipPlanePos;      /**< the z-position of the clip plane in viewport coordinates */
	double MatIn[4][4];         /**< array used in the calculations of the transformation matrix in opengl*/
	double MatOut[4][4];        /**< array used in the calculations of the transformation matrix in opengl*/

	double m_glTop;                    /**< position of the top end of the OGL viewport, in viewport coordonates */
	double m_HorizontalSplit;          /**< horizontal screen split ratio for 3D view*/
	double m_VerticalSplit;            /**< vertical screen split ratio for 3D view*/
	double m_GLScale;	               /**< the OpenGl scale for the view frustrum with respect to the windows device context. Always et to 1.0 */


	double m_CurSpanPos;        /**< Span position for Cp Graph  */
	double m_WingScale;			/**< scale for 2D display */
	double m_LastWOpp;			/**< last WOPP selected, try to set the same if it exists, for the new polar */

	double m_AlphaMin;          /**< the min value of the aoa for sequential analysis*/
	double m_AlphaMax;          /**< the max value of the aoa for sequential analysis*/
	double m_AlphaDelta;        /**< the increment value of the aoa for sequential analysis*/
	double m_ControlMin;        /**< the min value of the control parameter for sequential stability analysis*/
	double m_ControlMax;        /**< the max value of the control parameter for sequential stability analysis*/
	double m_ControlDelta;      /**< the increment value of the control parameter for sequential stability analysis*/
	double m_QInfMin;           /**< the min value of the velocity for sequential analysis */
	double m_QInfMax;           /**< the max value of the velocity for sequential analysis */
	double m_QInfDelta;         /**< the increment value of the velocity for sequential analysis */

	// mode animation data
	double m_ModeState[6];      /**< defines the value of the 6 dofs (x, y, z, rx, ry, rz) to display the position and orientation of the geometry */
	double m_ModeNorm;          /**< defines the amplitude of the modal animation */
	double m_ModeTime;          /**< defines the time moment for the modal animation */
	double m_Modedt;            /**< defines the time increment for the modal animation */
	
	// time curve data
	double m_TimeInput[4];      /**< defines the initial state for the display of the dynamic response in stability time graphs */
	double m_TotalTime;         /**< defines the total time for the display of the dynamic response in stability time graphs */
	double m_Deltat;            /**< defines the time increment for the display of the dynamic response in stability time graphs */
	double m_RampTime;          /**< defines the ramp time for the display of the dynamic response in stability time graphs */
	double m_RampAmplitude;     /**< defines the ramp amplitude for the display of the dynamic response in stability time graphs */

	int m_GLList;               /**< the number of existing OpenGl list at a given time. @deprecated, for development purposes only */

	QFile* m_pXFile;            /**< a pointer to the output .log file */

	QPoint m_ptOffset;              /**< client offset position for wing display */
	QPoint m_WPlrLegendOffset;      /**< client offset position for wing polar legend */
	QPoint m_WingLegendOffset;      /**< client offset position for WOPP polar legend */

	QRect m_rSingleRect;            /**< the drawing rectangle for single graph view */

	QGraph m_CpGraph;                       /**< the Cp Graph in 3D panel analysis */
	QGraph m_WingGraph[MAXGRAPHS];          /**< the WOpp graphs */
	QGraph m_WPlrGraph[MAXGRAPHS];          /**< the WPolar graphs */
	QGraph m_TimeGraph[MAXGRAPHS];          /**< the time response in stability view */
	QGraph m_LongRLGraph;                   /**< the root locus graph for Longitudinal modes */
	QGraph m_LatRLGraph;                    /**< the root locus graph for Longitudinal modes */
	QGraph* m_pCurGraph;                    /**< a pointer to the currently active graph */
	QGraph* m_pCurWPlrGraph;                /**< a pointer to the currently active WPolar graph */
	QGraph* m_pCurWingGraph;                /**< a pointer to the currently active WOpp graph */
	QGraph* m_pCurRLStabGraph;              /**< a pointer to the currently active Root Locus Graph */
	QGraph* m_pCurTimeGraph;                /**< a pointer to the currently active time graph */

private:
	CVector P; /**< Temporary variable */
	CVector W; /**< Temporary variable */
	CVector V; /**< Temporary variable */
	CVector T; /**< Temporary variable */

public:
	static void *s_pMainFrame;          /**< a pointer to the frame class */
	static void *s_p2dWidget;           /**< a pointer to mainframe's central widget where 2D drawing is performed */
	static void *s_p3dWidget;           /**< a pointer to mainframe's central widget where 23 drawing is performed */
	static double s_CoreSize;          /**< core radius of the VLM vortices */
	static double s_MinPanelSize;      /**< wing minimum panel size ; panels of less length are ignored */

	static int s_MaxMatSize;
	static int s_MaxRHSSize;

public:
	// style variables... didn't bother to document
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
};

#endif // QMIAREX_H
