/****************************************************************************

	Miarex
			Copyright (C) 2008-2014 Andre Deperrois adeperrois@xflr5.com

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


#include <QGLWidget>
#include <QAction>
#include <math.h>

#include "Miarex.h"
#include "./Objects3D.h"
#include "./view/GLCreateLists.h"
#include "./view/GLCreateBodyLists.h"
#include "../mainframe.h"
#include "../misc/Settings.h"
#include "../twodwidget.h"
#include "../globals.h"
#include "./mgt/ManagePlanesDlg.h"
#include "./view/StabViewDlg.h"
#include "./design/PlaneDlg.h"
#include "./design/GL3dWingDlg.h"
#include "./design/GL3dBodyDlg.h"
#include "./design/WingScaleDlg.h"
#include "./design/InertiaDlg.h"
#include "./analysis/PanelAnalysisDlg.h"
#include "./analysis/StabPolarDlg.h"
#include "./analysis/WAdvancedDlg.h"
#include "./analysis/WPolarDlg.h"
#include "./analysis/StabPolarDlg.h"
#include "../graph/GraphDlg.h"
#include "../misc/ProgressDlg.h"
#include "../misc/ModDlg.h"
#include "../misc/RenameDlg.h"
#include "../misc/PolarFilterDlg.h"
#include "../misc/ObjectPropsDlg.h"
#include "../misc/Units.h"
#include "../misc/W3dPrefsDlg.h"
#include "../misc/GLLightDlg.h"
#include "../misc/EditPlrDlg.h"

#ifdef Q_OS_WIN
#include <windows.h> // for Sleep
#endif



void *QMiarex::s_pMainFrame = NULL;
void *QMiarex::s_p2dWidget = NULL;
void *QMiarex::s_p3dWidget = NULL;


bool QMiarex::s_bAxes = true;
bool QMiarex::s_bOutline = true;
bool QMiarex::s_bSurfaces = true;
bool QMiarex::s_bShowMasses = false;
bool QMiarex::s_bFoilNames = false;
bool QMiarex::s_bVLMPanels = false;
bool QMiarex::s_bAutoCpScale = true;
double QMiarex::s_LegendMin = -1.0;
double QMiarex::s_LegendMax =  1.0;

double QMiarex::s_LiftScale     = 1.0;
double QMiarex::s_VelocityScale = 1.0;
double QMiarex::s_DragScale     = 1.0;

bool QMiarex::s_bICd = false;
bool QMiarex::s_bVCd = false;

bool QMiarex::m_bResetglGeom = true;
bool QMiarex::m_bResetglMesh = true;
bool QMiarex::m_bResetglWake = true;
bool QMiarex::m_bResetglOpp = true;
bool QMiarex::m_bResetglLift = true;
bool QMiarex::m_bResetglDrag = true;
bool QMiarex::m_bResetglDownwash = true;
bool QMiarex::m_bResetglPanelForce = true;
bool QMiarex::m_bResetglPanelCp = true;
bool QMiarex::m_bResetglStream = true;
bool QMiarex::m_bResetglLegend = true;
bool QMiarex::m_bResetglBody = true;
bool QMiarex::m_bResetglBodyMesh = true;
bool QMiarex::m_bResetglSurfVelocities = true;

bool QMiarex::s_bResetCurves = true;


QList<void *> *QMiarex::m_poaPlane = NULL;
QList<void *> *QMiarex::m_poaWPolar = NULL;
QList<void *> *QMiarex::m_poaPOpp = NULL;
QList<void *> *QMiarex::m_poaBody = NULL;


int QMiarex::s_GLList = 0;

CVector *QMiarex::s_pNode = NULL;
CVector *QMiarex::s_pMemNode = NULL;
CVector *QMiarex::s_pWakeNode = NULL;
CVector *QMiarex::s_pRefWakeNode = NULL;

Panel *QMiarex::s_pPanel = NULL;
Panel *QMiarex::s_pMemPanel = NULL;
Panel *QMiarex::s_pWakePanel = NULL;
Panel *QMiarex::s_pRefWakePanel = NULL;


/**
 * The public constructor.
 *
 * @param parent: a pointer to the parent window
 */
QMiarex::QMiarex(QWidget *parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	//construct and initialize everything
/*	int memsize=0;
	if(!Objects3D::AllocatePanelArrays(memsize))
	{
		QString strange = tr("This computer does not have enough RAM to run xflr5");
		Trace(strange);
		QMessageBox::warning(pMainFrame, tr("Warning"), strange);
	}
	else
	{
		QString strange = QString("QMiarex::Initial memory allocation for PanelAnalysis is %1 MB").arg((double)memsize/1024./1024., 7, 'f', 2);
		Trace(strange);
	}*/

	if(Objects3D::s_pPanelAnalysis) delete Objects3D::s_pPanelAnalysis;
	Objects3D::s_pPanelAnalysis = new PanelAnalysis; //construct on the heap and not on the stack to avoid memory overflow,


	m_pLLT    = new LLTAnalysis();
	m_pLLTDlg = new LLTAnalysisDlg(pMainFrame, m_pLLT);
	m_pPanelAnalysisDlg = new PanelAnalysisDlg(pMainFrame, Objects3D::s_pPanelAnalysis);


	m_pglLightDlg = new GLLightDlg(pMainFrame);

	m_pXFile      = NULL;
	m_pCurPlane   = NULL;
	m_pCurPOpp    = NULL;
	m_pCurWPolar  = NULL;
	m_pCurGraph   = m_WingGraph;


	for(int iw=0; iw<MAXWINGS; iw++)
	{
		m_pWingList[iw] = NULL;
		m_pWOpp[iw]     = NULL;
	}


	m_bXPressed = m_bYPressed = false;

	m_b3DCp              = false;
	m_bXCP               = false;
	m_bDownwash          = false;
	m_bMoments           = false;
	s_bICd               = false;
	s_bVCd               = false;
	m_bTrans             = false;
	m_bXTop              = false;
	m_bXBot              = false;
	m_bInitLLTCalc       = true;
	m_bXCmRef            = true;
	m_bLogFile           = true;
	m_bHalfWing          = false;
	m_bTransGraph        = true;
	s_bFoilNames         = false;
	m_bPanelForce        = false;
	m_bLongitudinal      = true;
	m_bCurPOppOnly       = true;
	m_bCurFrameOnly      = true;
	m_bType1 = m_bType2 = m_bType4 = m_bType7 = true;
	m_bShowElliptic      = false;
	m_bShowWingCurve[0] = m_bShowWingCurve[1] = m_bShowWingCurve[2] = m_bShowWingCurve[3] = true;
	m_bAutoScales        = false;
	m_bAutoScales        = false;
	m_bAnimateWOpp       = false;
	m_bAnimateWOppPlus   = true;
	m_bAnimateMode       = false;

	m_bArcball           = false;
	m_bStream            = false;
	m_bVortices          = false;
	m_bSurfVelocities            = false;
	m_bCrossPoint        = false;
	m_bPickCenter        = false;
	m_bShowCpScale       = true;
	m_bIs2DScaleSet      = false;
	m_bIs3DScaleSet      = false;
//	m_bForcedResponse    = true;



	m_LLTMaxIterations      = 100;
	LLTAnalysis::s_CvPrec    =   0.01;
	LLTAnalysis::s_RelaxMax  =  20.0;
	LLTAnalysis::s_NLLTStations = 20;

	Panel::s_VortexPos = 0.25;
	Panel::s_CtrlPos   = 0.75;

	m_CurveStyle = 0;
	m_CurveWidth = 1;
	m_CurveColor = QColor(127, 255, 70);
	m_bCurveVisible = true;
	m_bCurvePoints  = false;

	m_WakeInterNodes  = 6;
	m_bResetWake      = true;
	m_bSequence       = false;

	m_bHighlightOpp = false;

	m_bDirichlet = true;
	
	m_WPlrLegendOffset.setX(0);
	m_WPlrLegendOffset.setY(0);

	m_r2DCltRect.setWidth(0);
	m_r2DCltRect.setHeight(0);
	m_r3DCltRect.setWidth(0);
	m_r3DCltRect.setHeight(0);
	m_rSingleRect.setWidth(0);
	m_rSingleRect.setHeight(0);

	m_CurSpanPos    = 0.0;
	m_WingScale     = 1.0;

	m_AlphaMin     =  0.0;
	m_AlphaMax     =  1.0;
	m_AlphaDelta   =  0.5;
	m_BetaMin      =  0.0;
	m_BetaMax      =  1.0;
	m_BetaDelta    =  0.5;
	m_QInfMin      = 10.0;
	m_QInfMax      = 50.0;
	m_QInfDelta    = 10.0;
	m_ControlMin   =  0.0;
	m_ControlMax   =  1.0;
	m_ControlDelta =  0.1;

	m_ModeNorm = 1.0;
	m_ModeTime = 0.0;
	m_Modedt   = 0.01;


	m_ptOffset.rx() = 0;
	m_ptOffset.ry() = 0;

	m_InducedDragPoint = 0;


	s_GLList = 0;

	m_pTimerWOpp= new QTimer(this);
	m_posAnimateWOpp         = 0;

	m_pTimerMode= new QTimer(this);
	m_posAnimateMode         = 0;

	memset(m_ModeState, 0, 6*sizeof(double));
	m_TimeInput[0] = m_TimeInput[1] = m_TimeInput[2] = m_TimeInput[3] = 0.0;
	m_TotalTime = 10;//s
	m_Deltat    = 0.1;//s

	m_RampTime = .1;//s
	m_RampAmplitude = 1.;//CtrlUnit;

	m_pCurWingGraph = m_WingGraph;
	m_pCurWPlrGraph = m_WPlrGraph;
	m_pCurTimeGraph   = m_TimeGraph;
	m_pCurRLStabGraph = &m_LongRLGraph;

	for(int ig=0; ig<MAXGRAPHS; ig++)
	{
		m_WingGraph[ig].SetAutoX(true);
		m_WingGraph[ig].SetXUnit(2.0);
		m_WingGraph[ig].SetXMin(-1.0);
		m_WingGraph[ig].SetXMax( 1.0);
		m_WingGraph[ig].SetYMin(0.000);
		m_WingGraph[ig].SetYMax(0.001);
		m_WingGraph[ig].SetXMajGrid(true, QColor(120,120,120),2,1);
		m_WingGraph[ig].SetXMinGrid(false, true, QColor(80,80,80),2, 1, 100.0);
		m_WingGraph[ig].SetYMajGrid(true, QColor(120,120,120),2,1);
		m_WingGraph[ig].SetYMinGrid(false, true, QColor(80,80,80),2, 1, 0.1);
		m_WingGraph[ig].SetType(1);
		m_WingGraph[ig].SetMargin(50);

		m_WPlrGraph[ig].SetXMajGrid(true, QColor(120,120,120),2,1);
		m_WPlrGraph[ig].SetYMajGrid(true, QColor(120,120,120),2,1);
		m_WPlrGraph[ig].SetXMin(-0.0);
		m_WPlrGraph[ig].SetXMax( 0.1);
		m_WPlrGraph[ig].SetYMin(-0.01);
		m_WPlrGraph[ig].SetYMax( 0.01);
		m_WPlrGraph[ig].SetType(0);
		m_WPlrGraph[ig].SetMargin(50);
	}

	m_WPlrGraph[0].SetGraphName("Wing_Polar_Graph_1");
	m_WPlrGraph[1].SetGraphName("Wing_Polar_Graph_2");
	m_WPlrGraph[2].SetGraphName("Wing_Polar_Graph_3");
	m_WPlrGraph[3].SetGraphName("Wing_Polar_Graph_4");
	m_WPlrGraph[0].SetVariables(3,2);
	m_WPlrGraph[1].SetVariables(0,2);
	m_WPlrGraph[2].SetVariables(0,7);
	m_WPlrGraph[3].SetVariables(0,14);
	for(int ig=0; ig<MAXGRAPHS; ig++) SetWGraphTitles(m_WPlrGraph+ig);

	m_CpGraph.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_CpGraph.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_CpGraph.SetXTitle(tr("x"));
	m_CpGraph.SetYTitle(tr("Cp"));
	m_CpGraph.SetXMin( 0.0);
	m_CpGraph.SetXMax( 0.1);
	m_CpGraph.SetYMin(-0.01);
	m_CpGraph.SetYMax( 0.01);
	m_CpGraph.SetType(0);
	m_CpGraph.SetMargin(50);
	m_CpGraph.SetInverted(true);

	for(int i=0; i<MAXGRAPHS;i++) m_CpGraph.AddCurve(); // four curves, one for each of the plane's wings

	//set the default settings for the time response graphs
	for(int ig=0; ig<MAXGRAPHS; ig++)
	{
		m_TimeGraph[ig].SetXMajGrid(true, QColor(120,120,120),2,1);
		m_TimeGraph[ig].SetYMajGrid(true, QColor(120,120,120),2,1);
		m_TimeGraph[ig].SetXTitle("s");
		m_TimeGraph[ig].SetXMin( 0.0);
		m_TimeGraph[ig].SetXMax( 0.1);
		m_TimeGraph[ig].SetYMin(-0.01);
		m_TimeGraph[ig].SetYMax( 0.01);
		m_TimeGraph[ig].SetType(0);
		m_TimeGraph[ig].SetMargin(50);
		m_TimeGraph[ig].SetInverted(false);
		m_TimeGraph[ig].SetGraphName("Time Response");
	}

	//set the axis labels for the time graphs
	if(m_bLongitudinal)
	{
		m_TimeGraph[0].SetYTitle("u (m/s)");
		m_TimeGraph[1].SetYTitle("w (m/s)");
		m_TimeGraph[2].SetYTitle("q ("+QString::fromUtf8("°") +"/s)");
		m_TimeGraph[3].SetYTitle("theta ("+QString::fromUtf8("°") +"/s)");
	}
	else
	{
		m_TimeGraph[0].SetYTitle("v (m/s)");
		m_TimeGraph[1].SetYTitle("p ("+QString::fromUtf8("°") +"/s)");
		m_TimeGraph[2].SetYTitle("r ("+QString::fromUtf8("°") +"/s)");
		m_TimeGraph[3].SetYTitle("phi ("+QString::fromUtf8("°") +"/s)");
	}

	//set the default settings for the root locus graphs
	m_LongRLGraph.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_LongRLGraph.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_LongRLGraph.SetXTitle(tr("Real"));
	m_LongRLGraph.SetYTitle(tr("Imag/2.pi"));
	m_LongRLGraph.SetXMin( 0.0);
	m_LongRLGraph.SetXMax( 0.1);
	m_LongRLGraph.SetYMin(-0.01);
	m_LongRLGraph.SetYMax( 0.01);
	m_LongRLGraph.SetType(0);
	m_LongRLGraph.SetMargin(50);
	m_LongRLGraph.SetInverted(false);
	m_LongRLGraph.SetGraphName("Longitudinal Modes");
	m_LongRLGraph.m_bHighlightPoint = true;

	m_LatRLGraph.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_LatRLGraph.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_LatRLGraph.SetXTitle(tr("Real"));
	m_LatRLGraph.SetYTitle(tr("Imag/2.pi"));
	m_LatRLGraph.SetXMin( 0.0);
	m_LatRLGraph.SetXMax( 0.1);
	m_LatRLGraph.SetYMin(-0.01);
	m_LatRLGraph.SetYMax( 0.01);
	m_LatRLGraph.SetType(0);
	m_LatRLGraph.SetMargin(50);
	m_LatRLGraph.SetInverted(false);
	m_LatRLGraph.SetGraphName("Lateral Modes");
	m_LatRLGraph.m_bHighlightPoint = true;

	m_WingGraph[0].SetYVariable(0);
	m_WingGraph[1].SetYVariable(1);
	m_WingGraph[2].SetYVariable(2);
	m_WingGraph[3].SetYVariable(3);
	m_WingGraph[0].SetGraphName("Wing Graph 1");
	m_WingGraph[1].SetGraphName("Wing Graph 2");
	m_WingGraph[2].SetGraphName("Wing Graph 3");
	m_WingGraph[3].SetGraphName("Wing Graph 4");

	m_CpColor = QColor(255,100,150);
	m_CpStyle = 0;
	m_CpWidth = 1;
	m_bShowCpPoints = false;
	m_bShowCp       = true;

	m_iView = W3DVIEW;
	m_iWingView = ONEGRAPH;
	m_iWPlrView = ALLGRAPHS;
	m_iStabTimeView = ALLGRAPHS;

	m_CpGraph.SetGraphName(tr("Cp Graph"));

	m_CpColor = QColor(255,0,0);
	m_CpStyle = 0;
	m_CpWidth = 1;
	m_bShowCp = true;
	m_bShowCpPoints = false;

	s_GLList = 0;

	m_ClipPlanePos = 5.0;

	m_glViewportTrans.x  = 0.0;
	m_glViewportTrans.y  = 0.0;
	m_glViewportTrans.z  = 0.0;
	m_glScaled      = 1.0;
	m_GLScale       = 1.0;

	s_LiftScale = s_DragScale = s_VelocityScale = 0.7;


	m_StabilityResponseType = 0;
	
	m_LastPoint.setX(0);
	m_LastPoint.setY(0);
	m_PointDown.setX(0);
	m_PointDown.setY(0);

	memset(MatIn, 0, 16*sizeof(double));
	memset(MatOut, 0, 16*sizeof(double));

	m_ArcBall.m_pOffx     = &m_PlaneOffset.x;
	m_ArcBall.m_pOffy     = &m_PlaneOffset.y;
	m_ArcBall.m_pTransx   = &m_glViewportTrans.x;
	m_ArcBall.m_pTransy   = &m_glViewportTrans.y;
	m_ArcBall.m_pRect     = &m_r3DCltRect;


	SetupLayout();

	Connect();
}


/**
 * The public destructor.
 */
QMiarex::~QMiarex()
{
	Trace("Destroying Miarex");
	delete (GLLightDlg*)m_pglLightDlg;
	if(m_pLLTDlg) delete m_pLLTDlg;
	if(m_pLLT)    delete m_pLLT;
	if(m_pPanelAnalysisDlg) delete m_pPanelAnalysisDlg;

	Objects3D::deleteObjects();
	Objects3D::ReleasePanelMemory();
	delete Objects3D::s_pPanelAnalysis;
}


/**
 * Connect signals and slots
 */
void QMiarex::Connect()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	connect(this, SIGNAL(projectModified()), pMainFrame, SLOT(OnProjectModified()));

	connect(m_pctrlSequence, SIGNAL(clicked()), this, SLOT(OnSequence()));
	connect(m_pctrlStoreWOpp, SIGNAL(clicked()), this, SLOT(OnStoreWOpp()));
	connect(m_pctrlInitLLTCalc, SIGNAL(clicked()), this, SLOT(OnInitLLTCalc()));
	connect(m_pctrlAnalyze, SIGNAL(clicked()), this, SLOT(OnAnalyze()));
	connect(m_pctrlCurveStyle, SIGNAL(activated(int)), this, SLOT(OnCurveStyle(int)));
	connect(m_pctrlCurveWidth, SIGNAL(activated(int)), this, SLOT(OnCurveWidth(int)));
	connect(m_pctrlCurveColor, SIGNAL(clickedLB()), this, SLOT(OnCurveColor()));
	connect(m_pctrlShowPoints, SIGNAL(clicked()), this, SLOT(OnShowCurve()));
	connect(m_pctrlShowCurve, SIGNAL(clicked()), this, SLOT(OnShowCurve()));

	connect(m_pctrlPanelForce, SIGNAL(clicked()), this, SLOT(OnPanelForce()));
	connect(m_pctrlLift, SIGNAL(clicked()), this, SLOT(OnShowLift()));
	connect(m_pctrlIDrag, SIGNAL(clicked()), this, SLOT(OnShowIDrag()));
	connect(m_pctrlVDrag, SIGNAL(clicked()), this, SLOT(OnShowVDrag()));
	connect(m_pctrlTrans, SIGNAL(clicked()), this, SLOT(OnShowTransitions()));
	connect(m_pctrlCp, SIGNAL(clicked()),this, SLOT(On3DCp()));
	connect(m_pctrlMoment, SIGNAL(clicked()), this, SLOT(OnMoment()));
	connect(m_pctrlDownwash, SIGNAL(clicked()), this, SLOT(OnDownwash()));
	connect(m_pctrlStream, SIGNAL(clicked()), this, SLOT(OnStreamlines()));
	connect(m_pctrlSurfVel, SIGNAL(clicked()), this, SLOT(OnSurfaceSpeeds()));

	connect(m_pctrlWOppAnimate, SIGNAL(clicked()), this, SLOT(OnAnimateWOpp()));
	connect(m_pctrlAnimateWOppSpeed, SIGNAL(sliderMoved(int)), this, SLOT(OnAnimateWOppSpeed(int)));
	connect(m_pTimerWOpp, SIGNAL(timeout()), this, SLOT(OnAnimateWOppSingle()));
	connect(m_pTimerMode, SIGNAL(timeout()), this, SLOT(OnAnimateModeSingle()));

	connect(m_pctrlSurfaces,  SIGNAL(clicked()), SLOT(OnSurfaces()));
	connect(m_pctrlOutline,   SIGNAL(clicked()), SLOT(OnOutline()));
	connect(m_pctrlPanels,    SIGNAL(clicked()), SLOT(OnPanels()));
//	connect(m_pctrlVortices,  SIGNAL(clicked()), SLOT(OnVortices()));
	connect(m_pctrlFoilNames, SIGNAL(clicked()), SLOT(OnFoilNames()));
	connect(m_pctrlMasses,    SIGNAL(clicked()), SLOT(OnMasses()));
//	connect(m_pctrlLight,     SIGNAL(clicked()), SLOT(OnLight()));
	connect(m_pctrlClipPlanePos, SIGNAL(sliderMoved(int)), this, SLOT(OnClipPlane(int)));

	connect(m_pctrlKeepCpSection,  SIGNAL(clicked()), this, SLOT(OnKeepCpSection()));
	connect(m_pctrlResetCpSection, SIGNAL(clicked()), this, SLOT(OnResetCpSection()));
	connect(m_pctrlCpSectionSlider, SIGNAL(sliderMoved(int)), this, SLOT(OnCpSection(int)));
	connect(m_pctrlSpanPos, SIGNAL(editingFinished()), this, SLOT(OnCpPosition()));

	connect(m_pctrlAxes, SIGNAL(clicked()), SLOT(OnAxes()));
	connect(m_pctrlX, SIGNAL(clicked()), SLOT(On3DFront()));
	connect(m_pctrlY, SIGNAL(clicked()), SLOT(On3DLeft()));
	connect(m_pctrlZ, SIGNAL(clicked()), SLOT(On3DTop()));
	connect(m_pctrlIso, SIGNAL(clicked()), SLOT(On3DIso()));
	connect(m_pctrlReset, SIGNAL(clicked()), SLOT(On3DReset()));
	connect(m_pctrlPickCenter, SIGNAL(clicked()), SLOT(On3DPickCenter()));

	connect(m_pctrlAlphaMin, SIGNAL(editingFinished()), this, SLOT(OnReadAnalysisData()));
	connect(m_pctrlAlphaMax, SIGNAL(editingFinished()), this, SLOT(OnReadAnalysisData()));
	connect(m_pctrlAlphaDelta, SIGNAL(editingFinished()), this, SLOT(OnReadAnalysisData()));
}




/**
* Checks and enables all buttons and actions
* depending on the currently active objects
*/
void QMiarex::SetControls()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	QString str_translation;

	blockSignals(true);

	if(m_pCurPlane)     str_translation=tr("Current Plane");
	else                str_translation=tr("Current Object");
	pMainFrame->currentPlaneMenu->setTitle(str_translation);

	if(m_iView==W3DVIEW) m_pctrBottomControls->setCurrentIndex(1);
	else                 m_pctrBottomControls->setCurrentIndex(0);

	if(m_iView==WPOLARVIEW)     m_pctrlMiddleControls->setCurrentIndex(1);
	else if(m_iView==WCPVIEW)   m_pctrlMiddleControls->setCurrentIndex(2);
	else if(m_iView==STABTIMEVIEW || m_iView==STABPOLARVIEW) m_pctrlMiddleControls->setCurrentIndex(1);
	else                        m_pctrlMiddleControls->setCurrentIndex(0);

	if(m_pCurWPolar && m_pCurWPolar->polarType()==STABILITYPOLAR)
	{
		if (m_iView==W3DVIEW || m_iView==STABTIMEVIEW || m_iView==STABPOLARVIEW)
			pMainFrame->m_pctrlStabViewWidget->show();
		else pMainFrame->m_pctrlStabViewWidget->hide();
	}
	else pMainFrame->m_pctrlStabViewWidget->hide();

	if(m_pCurWPolar)
	{
		QString PolarProps;
		m_pCurWPolar->GetPolarProperties(PolarProps);
		m_pctrlPolarProps->setText(PolarProps);
	}
	else
	{
		m_pctrlPolarProps->clear();
	}

	m_pctrlInitLLTCalc->setEnabled(m_pCurWPolar && m_pCurWPolar->m_AnalysisMethod==LLTMETHOD);

	pMainFrame->WOppAct->setChecked(m_iView==WOPPVIEW);
	pMainFrame->WPolarAct->setChecked(m_iView==WPOLARVIEW);
	pMainFrame->W3DAct->setChecked(m_iView==W3DVIEW);
	pMainFrame->CpViewAct->setChecked(m_iView==WCPVIEW);

	pMainFrame->WOppAct->setChecked(m_iView==WOPPVIEW);
	pMainFrame->WPolarAct->setChecked(m_iView==WPOLARVIEW);
	pMainFrame->W3DAct->setChecked(m_iView==W3DVIEW);
	pMainFrame->CpViewAct->setChecked(m_iView==WCPVIEW);
	pMainFrame->StabTimeAct->setChecked(m_iView==STABTIMEVIEW);
	pMainFrame->RootLocusAct->setChecked(m_iView==STABPOLARVIEW);

	pMainFrame->halfWingAct->setChecked(m_bHalfWing);
	pMainFrame->showEllipticCurve->setChecked(m_bShowElliptic);
	pMainFrame->showXCmRefLocation->setChecked(m_bXCmRef);
	pMainFrame->showWing2Curve->setChecked(m_bShowWingCurve[1]);
	pMainFrame->showStabCurve->setChecked(m_bShowWingCurve[2]);
	pMainFrame->showFinCurve->setChecked(m_bShowWingCurve[3]);

	pMainFrame->showCurWOppOnly->setChecked(m_bCurPOppOnly);

	if(m_iView==WOPPVIEW)
	{
		pMainFrame->Graph1->setChecked(m_iWingView==ONEGRAPH && (m_pCurWingGraph == m_WingGraph));
		pMainFrame->Graph2->setChecked(m_iWingView==ONEGRAPH && (m_pCurWingGraph == m_WingGraph+1));
		pMainFrame->Graph3->setChecked(m_iWingView==ONEGRAPH && (m_pCurWingGraph == m_WingGraph+2));
		pMainFrame->Graph4->setChecked(m_iWingView==ONEGRAPH && (m_pCurWingGraph == m_WingGraph+3));
		pMainFrame->twoGraphs->setChecked(m_iWingView==TWOGRAPHS);
		pMainFrame->fourGraphs->setChecked(m_iWingView==ALLGRAPHS);
	}
	else if(m_iView==STABTIMEVIEW)
	{
		pMainFrame->Graph1->setChecked(m_iStabTimeView==ONEGRAPH && (m_pCurTimeGraph ==  m_TimeGraph));
		pMainFrame->Graph2->setChecked(m_iStabTimeView==ONEGRAPH && (m_pCurTimeGraph == (m_TimeGraph+1)));
		pMainFrame->Graph3->setChecked(m_iStabTimeView==ONEGRAPH && (m_pCurTimeGraph == (m_TimeGraph+2)));
		pMainFrame->Graph4->setChecked(m_iStabTimeView==ONEGRAPH && (m_pCurTimeGraph == (m_TimeGraph+3)));
		pMainFrame->twoGraphs->setChecked(m_iStabTimeView==TWOGRAPHS);
		pMainFrame->fourGraphs->setChecked(m_iStabTimeView==ALLGRAPHS);
	}
	else if(m_iView==WPOLARVIEW)
	{
		pMainFrame->Graph1->setChecked(m_iWPlrView==ONEGRAPH && (m_pCurWPlrGraph == m_WPlrGraph));
		pMainFrame->Graph2->setChecked(m_iWPlrView==ONEGRAPH && (m_pCurWPlrGraph == m_WPlrGraph+1));
		pMainFrame->Graph3->setChecked(m_iWPlrView==ONEGRAPH && (m_pCurWPlrGraph == m_WPlrGraph+2));
		pMainFrame->Graph4->setChecked(m_iWPlrView==ONEGRAPH && (m_pCurWPlrGraph == m_WPlrGraph+3));
		pMainFrame->twoGraphs->setChecked(m_iWPlrView==TWOGRAPHS);
		pMainFrame->fourGraphs->setChecked(m_iWPlrView==ALLGRAPHS);
	}

	pMainFrame->WPlrGraphMenu->setEnabled(m_iView==WPOLARVIEW || m_iView==STABPOLARVIEW);
	pMainFrame->hideAllWPlrs->setEnabled( m_iView==WPOLARVIEW || m_iView==STABPOLARVIEW);
	pMainFrame->showAllWPlrs->setEnabled( m_iView==WPOLARVIEW || m_iView==STABPOLARVIEW);

	m_pctrlAnalyze->setEnabled(m_pCurWPolar);
	m_pctrlAlphaMin->setEnabled(m_pCurWPolar);
	m_pctrlAlphaMax->setEnabled(m_pCurWPolar && m_bSequence);
	m_pctrlAlphaDelta->setEnabled(m_pCurWPolar && m_bSequence);
	m_pctrlSequence->setEnabled(m_pCurWPolar);

	m_pctrlStoreWOpp->setEnabled(m_pCurWPolar);

	pMainFrame->showCurWOppOnly->setEnabled(m_iView==WOPPVIEW);
	pMainFrame->showAllWOpps->setEnabled(m_iView==WOPPVIEW);
	pMainFrame->hideAllWOpps->setEnabled(m_iView==WOPPVIEW);
	pMainFrame->showEllipticCurve->setEnabled(m_iView==WOPPVIEW);
	pMainFrame->showXCmRefLocation->setEnabled(m_iView==WOPPVIEW);
	pMainFrame->showWing2Curve->setEnabled(m_pWingList[1] && (m_iView==WOPPVIEW || m_iView==WCPVIEW));
	pMainFrame->showStabCurve->setEnabled( m_pWingList[2] && (m_iView==WOPPVIEW || m_iView==WCPVIEW));
	pMainFrame->showFinCurve->setEnabled(  m_pWingList[3] && (m_iView==WOPPVIEW || m_iView==WCPVIEW));
	pMainFrame->showAllWPlrOpps->setEnabled(m_iView==WOPPVIEW);
	pMainFrame->hideAllWPlrOpps->setEnabled(m_iView==WOPPVIEW);
	pMainFrame->WOppGraphMenu->setEnabled(   m_iView==WOPPVIEW || m_iView==STABTIMEVIEW);
	pMainFrame->WOppCurGraphMenu->setEnabled(m_iView==WOPPVIEW || m_iView==WCPVIEW || m_iView==STABTIMEVIEW);

	m_pctrlLift->setEnabled((m_iView==WOPPVIEW||m_iView==W3DVIEW) && m_pCurPOpp);
	m_pctrlTrans->setEnabled((m_iView==WOPPVIEW||m_iView==W3DVIEW) && m_pCurPOpp);
	m_pctrlWOppAnimate->setEnabled((m_iView==WOPPVIEW||m_iView==W3DVIEW) && m_pCurPOpp && m_pCurWPolar->polarType()!=STABILITYPOLAR);
	m_pctrlAnimateWOppSpeed->setEnabled((m_iView==WOPPVIEW||m_iView==W3DVIEW) && m_pCurPOpp);
	m_pctrlIDrag->setEnabled(     m_iView==W3DVIEW && m_pCurPOpp);
	m_pctrlVDrag->setEnabled(     m_iView==W3DVIEW && m_pCurPOpp);
	m_pctrlDownwash->setEnabled(  m_iView==W3DVIEW && m_pCurPOpp);
//	m_pctrlVortices->setEnabled(  m_iView==W3DVIEW && m_pCurPOpp);
	m_pctrlMoment->setEnabled(    m_iView==W3DVIEW && m_pCurPOpp);
	m_pctrlPanelForce->setEnabled(m_iView==W3DVIEW && m_pCurPOpp && m_pCurWPolar && m_pCurWPolar->analysisMethod()!=LLTMETHOD);
	m_pctrlCp->setEnabled(        m_iView==W3DVIEW && m_pCurPOpp && m_pCurWPolar && m_pCurWPolar->analysisMethod()!=LLTMETHOD);
	m_pctrlStream->setEnabled(    m_iView==W3DVIEW && m_pCurPOpp && m_pCurWPolar && m_pCurWPolar->analysisMethod()!=LLTMETHOD);
	m_pctrlSurfVel->setEnabled(   m_iView==W3DVIEW && m_pCurPOpp && m_pCurWPolar && m_pCurWPolar->analysisMethod()!=LLTMETHOD);

	m_pctrlFoilNames->setChecked(s_bFoilNames);
	m_pctrlMasses->setChecked(s_bShowMasses);

	pMainFrame->highlightWOppAct->setChecked(m_bHighlightOpp);

	pMainFrame->defineWPolar->setEnabled(m_pCurPlane);
	pMainFrame->defineStabPolar->setEnabled(m_pCurPlane);

	pMainFrame->currentPlaneMenu->setEnabled(m_pCurPlane);
	pMainFrame->curWPlrMenu->setEnabled(m_pCurWPolar);
	pMainFrame->curWOppMenu->setEnabled(m_pCurPOpp);

	//	pMainFrame->CurBodyMenu->setVisible(m_pCurPlane!=NULL);
	pMainFrame->editCurBodyAct->setEnabled(m_pCurPlane && m_pCurPlane->body());

	StabViewDlg *pStabView = (StabViewDlg*)pMainFrame->m_pStabView;
	pStabView->SetControls();

	m_pctrlSpanPos->SetValue(m_CurSpanPos);
	m_pctrlCpSectionSlider->setValue((int)(m_CurSpanPos*100.0));

	pMainFrame->W3DScalesAct->setChecked(pMainFrame->m_pctrl3DScalesWidget->isVisible());

	m_pctrlOutline->setChecked(s_bOutline);
	m_pctrlPanels->setChecked(s_bVLMPanels);
	m_pctrlAxes->setChecked(s_bAxes);
	m_pctrlCp->setChecked(m_b3DCp);
	m_pctrlPanelForce->setChecked(m_bPanelForce);
	m_pctrlDownwash->setChecked(m_bDownwash);
	m_pctrlMoment->setChecked(m_bMoments);
	m_pctrlTrans->setChecked(m_bXTop);
	m_pctrlLift->setChecked(m_bXCP);
	m_pctrlIDrag->setChecked(s_bICd);
	m_pctrlVDrag->setChecked(s_bVCd);
	m_pctrlAxes->setChecked(s_bAxes);
//	m_pctrlLight->setChecked(GLLightDlg::IsLightOn());
	m_pctrlSurfaces->setChecked(s_bSurfaces);
	m_pctrlOutline->setChecked(s_bOutline);
	m_pctrlStream->setChecked(m_bStream);
	m_pctrlClipPlanePos->setValue((int)(m_ClipPlanePos*100.0));

	SetCurveParams();

	blockSignals(false);
}


/**
 * Sets the checkboxes of the x, y and z view to their default false value
 */
void QMiarex::SetViewControls()
{
	m_pctrlX->setChecked(false);
	m_pctrlY->setChecked(false);
	m_pctrlZ->setChecked(false);
	m_pctrlIso->setChecked(false);
}



/**
 * Creates the curves of the Cp graph at the selected span positions
 */
void QMiarex::CreateCpCurves()
{
	int p,pp,i;
	bool bFound;
	double SpanPos, SpanInc;

	Curve *pCurve = NULL;
	QString str2, str3;

	for (i=0; i<MAXWINGS; i++)
	{
		// the first four curves are necessarily the current opPoint's main wing, second wing, elevator and fin
		// the next are those the user has chosen to keep for display --> don't reset them
		pCurve = m_CpGraph.GetCurve(i);
		if(pCurve) pCurve->clear();
	}

	if(!m_pCurPOpp || !m_bShowCp)
	{
		return;
	}

	int coef = 2;
	if(m_pCurWPolar->m_bThinSurfaces) coef = 1;

	m_CurSpanPos = qMax(-1.0, m_CurSpanPos);
	m_CurSpanPos = qMin( 1.0, m_CurSpanPos);
	SpanPos = m_CurSpanPos*m_pCurPOpp->m_Span/2.000001;

//	str1 = m_pCurPlane->m_Wing[0].WingName();
	str2 = QString(" a=%1").arg(m_pCurPOpp->m_Alpha, 5, 'f', 2);
	str3 = QString(" y/b=%1").arg(m_CurSpanPos, 5, 'f', 2);

//	if(m_bCurWOppOnly)
	{
		p=0;
		bFound = false;
		if(m_pCurWPolar->m_bThinSurfaces) p+=m_pCurPlane->m_Wing[0].m_Surface[0]->m_NXPanels;

		SpanInc = -m_pCurPlane->planformSpan()/2.0;
		for (p=0; p<m_pCurPlane->m_Wing[0].m_MatSize; p++)
		{
			if(m_pCurPlane->m_Wing[0].m_pWingPanel[p].m_bIsTrailing && m_pCurPlane->m_Wing[0].m_pWingPanel[p].m_Pos<=MIDSURFACE)
			{
				SpanInc += m_pCurPlane->m_Wing[0].m_pWingPanel[p].Width();
				if(SpanPos<=SpanInc || qAbs(SpanPos-SpanInc)/m_pCurPlane->planformSpan()<0.001)
				{
					bFound = true;
					break;
				}
			}
		}
		for (int iw=0; iw<MAXWINGS; iw++)
		{
			if(m_pWingList[iw] && m_bShowWingCurve[iw])
			{
				p=0;
				bFound = false;
				if(m_pCurWPolar->m_bThinSurfaces) p+=m_pWingList[iw]->m_Surface.at(0)->m_NXPanels;

				SpanInc = -m_pWingList[iw]->m_PlanformSpan/2.0;
				for (p=0; p<m_pWingList[iw]->m_MatSize; p++)
				{
					if(m_pWingList[iw]->m_pWingPanel[p].m_bIsTrailing && m_pWingList[iw]->m_pWingPanel[p].m_Pos<=MIDSURFACE)
					{
						SpanInc += m_pWingList[iw]->m_pWingPanel[p].Width();
						if(SpanPos<=SpanInc || qAbs(SpanPos-SpanInc)/m_pWingList[iw]->m_PlanformSpan<0.001)
						{
							bFound = true;
							break;
						}
					}
				}

				if(bFound)
				{
					pCurve = m_CpGraph.GetCurve(iw);
					pCurve->SetColor(m_CpColor);
					pCurve->SetStyle(m_CpStyle);
					pCurve->SetWidth(m_CpWidth);
					pCurve->ShowPoints(m_bShowCpPoints);

					pCurve->SetTitle(m_pWingList[iw]->m_WingName+str2+str3);

					for (pp=p; pp<p+coef*m_pWingList[iw]->m_Surface.at(0)->m_NXPanels; pp++)
					{
						pCurve->AppendPoint(s_pPanel[pp].CollPt.x, m_pWOpp[iw]->m_dCp[pp]);
					}
				}
			}
		}
	}
}



/**
 * Creates the curves for the graphs in the operating point view.
 * @todo manage with a boolean flag to recreate only when necessary
*/
void QMiarex::CreateWOppCurves()
{
//	WingOpp *pWOpp = NULL;;
	PlaneOpp *pPOpp = NULL;
	Curve *pWingCurve[MAXWINGS][MAXGRAPHS];

	int i,k;

	for(int ig=0; ig<MAXGRAPHS; ig++) m_WingGraph[ig].DeleteCurves();

	// Browse through the array of plane operating points
	// add a curve for those selected, and fill them with data
	for (k=0; k<m_poaPOpp->size(); k++)
	{
		pPOpp = (PlaneOpp*)m_poaPOpp->at(k);
		if (pPOpp->m_bIsVisible && (!m_bCurPOppOnly || (m_pCurPOpp==pPOpp)))
		{
			for(int iw=0; iw<MAXWINGS; iw++)
			{
				if(m_bShowWingCurve[iw] && pPOpp->m_pPlaneWOpp[iw])
				{
					for(int ic=0; ic<MAXGRAPHS; ic++)
					{
						pWingCurve[iw][ic]  = m_WingGraph[ic].AddCurve();
						pWingCurve[iw][ic]->ShowPoints(pPOpp->m_bShowPoints);
						pWingCurve[iw][ic]->SetStyle(pPOpp->m_Style);
						pWingCurve[iw][ic]->SetColor(pPOpp->m_Color);
						pWingCurve[iw][ic]->SetWidth(pPOpp->m_Width);
						pWingCurve[iw][ic]->SetTitle(pPOpp->title());
						FillWOppCurve(pPOpp->m_pPlaneWOpp[iw], m_WingGraph+ic, pWingCurve[iw][ic]);
					}
				}
			}
		}
	}
	
	//if the optimal elliptic curve is requested, and if the graph variable is local lift, then add the curve
	if(m_bShowElliptic && m_pCurPOpp)
	{
		double maxlift, x, y;
		maxlift = m_pCurPOpp->m_pPlaneWOpp[0]->GetMaxLift();
		int nStart;
		if(m_pCurPOpp->m_AnalysisMethod==LLTMETHOD) nStart = 1;
		else nStart = 0;

		for(int ig=0; ig<MAXGRAPHS; ig++)
		{
			if(m_WingGraph[ig].GetYVariable()==3)
			{
				Curve *pCurve = m_WingGraph[ig].AddCurve();
				pCurve->SetStyle(1);
				pCurve->SetColor(QColor(150, 150, 150));
				for (i=nStart; i<m_pCurPOpp->m_NStation; i++)
				{
					x = m_pCurPOpp->m_pPlaneWOpp[0]->m_SpanPos[i];
					y = maxlift*sqrt(1.0-x*x/m_pCurPOpp->m_pPlaneWOpp[0]->m_Span/m_pCurPOpp->m_pPlaneWOpp[0]->m_Span*4.0);
					pCurve->AppendPoint(x*Units::mtoUnit(),y);
				}
			}
		}
	}
}


/**
* Resets and fills the polar graphs curves with the data from the CWPolar objects
* @todo manage with a boolean flag to recreate only when necessary
*/
void QMiarex::CreateWPolarCurves()
{
	WPolar *pWPolar;
	Curve *pCurve[MAXGRAPHS];

	for(int ig=0; ig<MAXGRAPHS; ig++) m_WPlrGraph[ig].DeleteCurves();

	for (int k=0; k<m_poaWPolar->size(); k++)
	{
		pWPolar = (WPolar*)m_poaWPolar->at(k);
		if (pWPolar->m_bIsVisible && pWPolar->m_Alpha.size()>0 &&
		    ((m_bType1 && pWPolar->polarType()==FIXEDSPEEDPOLAR) ||
			(m_bType2 && pWPolar->polarType()==FIXEDLIFTPOLAR) ||
			(m_bType4 && pWPolar->polarType()==FIXEDAOAPOLAR) ||
			(            pWPolar->polarType()==BETAPOLAR) ||
			(m_bType7 && pWPolar->polarType()==STABILITYPOLAR)))
		{

			for(int ig=0; ig<MAXGRAPHS; ig++)
			{
				pCurve[ig] = m_WPlrGraph[ig].AddCurve();
				pCurve[ig]->ShowPoints(pWPolar->m_bShowPoints);
				pCurve[ig]->SetStyle(pWPolar->m_Style);
				pCurve[ig]->SetColor(pWPolar->m_Color);
				pCurve[ig]->SetWidth(pWPolar->m_Width);
				FillWPlrCurve(pCurve[ig], pWPolar, m_WPlrGraph[ig].GetXVariable(), m_WPlrGraph[ig].GetYVariable());
				pCurve[ig]->SetTitle(pWPolar->polarName());
			}
		}
	}
}


/**
* Resets and fills the stability graphs curves with the data from the CWPolar objects
*/
void QMiarex::CreateStabilityCurves()
{
	if(m_iView==STABTIMEVIEW)
	{
		if(m_StabilityResponseType==1)  CreateStabRungeKuttaCurves();
		else                            CreateStabTimeCurves();
	}
	else
	{
		CreateStabRLCurves();
	}
}


/**
* Builds the initial condition response due to perturbations from steady state
* The time response is calculated analytically based on the knowledge of the eigenvalues and eigenvectors
*/
void QMiarex::CreateStabTimeCurves()
{
	complex<double> M[16];// the modal matrix
	complex<double> InvM[16];// the inverse of the modal matrix
	complex<double> q[4],q0[4],y[4];//the part of each mode in the solution
	int i,j,k;
	double t, dt, TotalPoints; // the input load
	complex<double> in[4];
	Curve *pCurve0, *pCurve1, *pCurve2, *pCurve3;
	QString strong, CurveTitle;


	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
	CurveTitle = pStabView->m_pctrlCurveList->currentText();
	
	pCurve0 = m_TimeGraph[0].GetCurve(CurveTitle);
	if(pCurve0) pCurve0->clear();
	else return;
	pCurve1 = m_TimeGraph[1].GetCurve(CurveTitle);
	if(pCurve1) pCurve1->clear();
	else return;
	pCurve2 = m_TimeGraph[2].GetCurve(CurveTitle);
	if(pCurve2) pCurve2->clear();
	else return;
	pCurve3 = m_TimeGraph[3].GetCurve(CurveTitle);
	if(pCurve3) pCurve3->clear();
	else return;
	
	if(!m_pCurPOpp || !m_pCurPOpp->m_bIsVisible) return;

	strong = pStabView->m_pctrlCurveList->currentText();

	m_Deltat = pStabView->m_pctrlDeltat->Value();
	m_TotalTime = pStabView->m_pctrlTotalTime->Value();
	dt = m_TotalTime/1000.;
	if(dt<m_Deltat) dt = m_Deltat;

	TotalPoints = qMin(1000, (int)(m_TotalTime/dt));
	//read the initial state condition
	m_TimeInput[0] = pStabView->m_pctrlStabVar1->Value();
	m_TimeInput[1] = pStabView->m_pctrlStabVar2->Value();
	m_TimeInput[2] = pStabView->m_pctrlStabVar3->Value();
	m_TimeInput[3] = 0.0;//we start with an initial 0.0 value for pitch or bank angles

	if(m_StabilityResponseType==0)
	{
		//start with the user input initial conditions
		in[0] = complex<double>(m_TimeInput[0]*PI/180.0, 0.0);
		in[1] = complex<double>(m_TimeInput[1]*PI/180.0, 0.0);
		in[2] = complex<double>(m_TimeInput[2]*PI/180.0, 0.0);
		in[3] = complex<double>(m_TimeInput[3]*PI/180.0, 0.0);
	}
	else if(m_StabilityResponseType==2)
	{
		//start with the initial conditions which will excite only the requested mode
		in[0] = m_pCurPOpp->m_EigenVector[pStabView->m_iCurrentMode][0];
		in[1] = m_pCurPOpp->m_EigenVector[pStabView->m_iCurrentMode][1];
		in[2] = m_pCurPOpp->m_EigenVector[pStabView->m_iCurrentMode][2];
		in[3] = m_pCurPOpp->m_EigenVector[pStabView->m_iCurrentMode][3];
	}	
	
	//fill the modal matrix
	if(m_bLongitudinal) k=0; else k=1;
	for (i=0; i<4; i++)
	{
		for(j=0;j<4;j++)
		{
			*(M+4*j+i) = m_pCurPOpp->m_EigenVector[k*4+i][j];
		}
	}

	//Invert the matrix
	if(!Invert44(M, InvM))
	{
	}
	else
	{
		//calculate the modal coefficients at t=0
		q0[0] = InvM[0] * in[0] + InvM[1] * in[1] + InvM[2] * in[2] + InvM[3] * in[3];
		q0[1] = InvM[4] * in[0] + InvM[5] * in[1] + InvM[6] * in[2] + InvM[7] * in[3];
		q0[2] = InvM[8] * in[0] + InvM[9] * in[1] + InvM[10]* in[2] + InvM[11]* in[3];
		q0[3] = InvM[12]* in[0] + InvM[13]* in[1] + InvM[14]* in[2] + InvM[15]* in[3];
		
		for(i=0; i<TotalPoints; i++)
		{
			t = (double)i * dt;
			q[0] = q0[0] * exp(m_pCurPOpp->m_EigenValue[0+k*4]*t);
			q[1] = q0[1] * exp(m_pCurPOpp->m_EigenValue[1+k*4]*t);
			q[2] = q0[2] * exp(m_pCurPOpp->m_EigenValue[2+k*4]*t);
			q[3] = q0[3] * exp(m_pCurPOpp->m_EigenValue[3+k*4]*t);
			y[0] = *(M+4*0+0) * q[0] +*(M+4*0+1) * q[1] +*(M+4*0+2) * q[2] +*(M+4*0+3) * q[3];
			y[1] = *(M+4*1+0) * q[0] +*(M+4*1+1) * q[1] +*(M+4*1+2) * q[2] +*(M+4*1+3) * q[3];
			y[2] = *(M+4*2+0) * q[0] +*(M+4*2+1) * q[1] +*(M+4*2+2) * q[2] +*(M+4*2+3) * q[3];
			y[3] = *(M+4*3+0) * q[0] +*(M+4*3+1) * q[1] +*(M+4*3+2) * q[2] +*(M+4*3+3) * q[3];
			if(abs(q[0])>1.e10 || abs(q[1])>1.e10 || abs(q[2])>1.e10  || abs(q[3])>1.e10 ) break;

			pCurve0->AppendPoint(t, y[0].real());
			if(m_bLongitudinal) pCurve1->AppendPoint(t, y[1].real());
			else                pCurve1->AppendPoint(t, y[1].real()*180.0/PI);
			pCurve2->AppendPoint(t, y[2].real()*180.0/PI);
			pCurve3->AppendPoint(t, y[3].real()*180.0/PI);
		}
	}
}



/**
* Builds the forced response from the state matrix and the forced input matrix
* using a Runge-Kutta integration scheme.
* The forced input is interpolated in the control history defined in the input table.
*/
void QMiarex::CreateStabRungeKuttaCurves()
{
	static int i, TotalPoints, PlotInterval;

	static double t, dt, ctrl_t;
	static Curve *pCurve0, *pCurve1, *pCurve2, *pCurve3;
	static double A[4][4], B[4];
	static double m[5][4];
	static double y[4], yp[4];

	QString CurveTitle;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
	CurveTitle = pStabView->m_pctrlCurveList->currentText();
	pCurve0 = m_TimeGraph[0].GetCurve(CurveTitle);
	if(pCurve0) pCurve0->clear();
	else return;
	pCurve1 = m_TimeGraph[1].GetCurve(CurveTitle);
	if(pCurve1) pCurve1->clear();
	else return;
	pCurve2 = m_TimeGraph[2].GetCurve(CurveTitle);
	if(pCurve2) pCurve2->clear();
	else return;
	pCurve3 = m_TimeGraph[3].GetCurve(CurveTitle);
	if(pCurve3) pCurve3->clear();
	else return;

	//We need a WOpp
	if(!m_pCurPOpp) return;//nothing to plot
	//Check that the current polar is of the stability type
	if(!m_pCurWPolar || m_pCurWPolar->polarType()!=STABILITYPOLAR) return;

	if(m_bLongitudinal)
	{
		memcpy(A, m_pCurPOpp->m_ALong, 4*4*sizeof(double));
		memcpy(B, m_pCurPOpp->m_BLong, 4*sizeof(double));
	}
	else
	{
		memcpy(A, m_pCurPOpp->m_ALat, 4*4*sizeof(double));
		memcpy(B, m_pCurPOpp->m_BLat, 4*sizeof(double));
	}

	// Rebuild the Forced Response matrix
	//read the initial step condition
//	pStabView->ReadForcedInput(time,input);
//	RampAmp     = m_RampAmplitude*PI/180.0;
//	RampTime    = m_RampTime;           //s

	m_Deltat    = pStabView->m_pctrlDeltat->Value();
	m_TotalTime = pStabView->m_pctrlTotalTime->Value();
	dt = m_TotalTime/1000.;
	if(dt<m_Deltat) dt = m_Deltat;

	TotalPoints  = qMin(1000, (int)(m_TotalTime/dt));
	PlotInterval = qMax(1,(int)(TotalPoints/200));

	//we are considering forced response from initial steady state, so set
	// initial conditions to 0
	t = 0.0;
	y[0] = y[1] = y[2] = y[3] = 0.0;
	pCurve0->AppendPoint(0.0, y[0]);
	pCurve1->AppendPoint(0.0, y[1]);
	pCurve2->AppendPoint(0.0, y[2]);
	pCurve3->AppendPoint(0.0, y[3]);

	//Runge-Kutta method
	for(i=0; i<TotalPoints; i++)
	{
		//initial slope m1
		m[0][0] = A[0][0]*y[0] + A[0][1]*y[1] + A[0][2]*y[2] + A[0][3]*y[3];
		m[0][1] = A[1][0]*y[0] + A[1][1]*y[1] + A[1][2]*y[2] + A[1][3]*y[3];
		m[0][2] = A[2][0]*y[0] + A[2][1]*y[1] + A[2][2]*y[2] + A[2][3]*y[3];
		m[0][3] = A[3][0]*y[0] + A[3][1]*y[1] + A[3][2]*y[2] + A[3][3]*y[3];

		ctrl_t = pStabView->GetControlInput(t);
		m[0][0] += B[0] * ctrl_t;
		m[0][1] += B[1] * ctrl_t;
		m[0][2] += B[2] * ctrl_t;
		m[0][3] += B[3] * ctrl_t;

		//middle point m2
		yp[0] = y[0] + dt/2.0 * m[0][0];
		yp[1] = y[1] + dt/2.0 * m[0][1];
		yp[2] = y[2] + dt/2.0 * m[0][2];
		yp[3] = y[3] + dt/2.0 * m[0][3];

		m[1][0] = A[0][0]*yp[0] + A[0][1]*yp[1] + A[0][2]*yp[2] + A[0][3]*yp[3];
		m[1][1] = A[1][0]*yp[0] + A[1][1]*yp[1] + A[1][2]*yp[2] + A[1][3]*yp[3];
		m[1][2] = A[2][0]*yp[0] + A[2][1]*yp[1] + A[2][2]*yp[2] + A[2][3]*yp[3];
		m[1][3] = A[3][0]*yp[0] + A[3][1]*yp[1] + A[3][2]*yp[2] + A[3][3]*yp[3];

		ctrl_t = pStabView->GetControlInput(t+dt/2.0);
		m[1][0] += B[0] * ctrl_t;
		m[1][1] += B[1] * ctrl_t;
		m[1][2] += B[2] * ctrl_t;
		m[1][3] += B[3] * ctrl_t;

		//second point m3
		yp[0] = y[0] + dt/2.0 * m[1][0];
		yp[1] = y[1] + dt/2.0 * m[1][1];
		yp[2] = y[2] + dt/2.0 * m[1][2];
		yp[3] = y[3] + dt/2.0 * m[1][3];

		m[2][0] = A[0][0]*yp[0] + A[0][1]*yp[1] + A[0][2]*yp[2] + A[0][3]*yp[3];
		m[2][1] = A[1][0]*yp[0] + A[1][1]*yp[1] + A[1][2]*yp[2] + A[1][3]*yp[3];
		m[2][2] = A[2][0]*yp[0] + A[2][1]*yp[1] + A[2][2]*yp[2] + A[2][3]*yp[3];
		m[2][3] = A[3][0]*yp[0] + A[3][1]*yp[1] + A[3][2]*yp[2] + A[3][3]*yp[3];

		ctrl_t = pStabView->GetControlInput(t+dt/2.0);

		m[2][0] += B[0] * ctrl_t;
		m[2][1] += B[1] * ctrl_t;
		m[2][2] += B[2] * ctrl_t;
		m[2][3] += B[3] * ctrl_t;

		//third point m4
		yp[0] = y[0] + dt * m[2][0];
		yp[1] = y[1] + dt * m[2][1];
		yp[2] = y[2] + dt * m[2][2];
		yp[3] = y[3] + dt * m[2][3];

		m[3][0] = A[0][0]*yp[0] + A[0][1]*yp[1] + A[0][2]*yp[2] + A[0][3]*yp[3];
		m[3][1] = A[1][0]*yp[0] + A[1][1]*yp[1] + A[1][2]*yp[2] + A[1][3]*yp[3];
		m[3][2] = A[2][0]*yp[0] + A[2][1]*yp[1] + A[2][2]*yp[2] + A[2][3]*yp[3];
		m[3][3] = A[3][0]*yp[0] + A[3][1]*yp[1] + A[3][2]*yp[2] + A[3][3]*yp[3];

		ctrl_t = pStabView->GetControlInput(t+dt);

		m[3][0] += B[0] * ctrl_t;
		m[3][1] += B[1] * ctrl_t;
		m[3][2] += B[2] * ctrl_t;
		m[3][3] += B[3] * ctrl_t;

		//final slope m5
		m[4][0] = 1./6. * (m[0][0] + 2.0*m[1][0] + 2.0*m[2][0] + m[3][0]);
		m[4][1] = 1./6. * (m[0][1] + 2.0*m[1][1] + 2.0*m[2][1] + m[3][1]);
		m[4][2] = 1./6. * (m[0][2] + 2.0*m[1][2] + 2.0*m[2][2] + m[3][2]);
		m[4][3] = 1./6. * (m[0][3] + 2.0*m[1][3] + 2.0*m[2][3] + m[3][3]);

		y[0] += m[4][0] * dt;
		y[1] += m[4][1] * dt;
		y[2] += m[4][2] * dt;
		y[3] += m[4][3] * dt;
		t +=dt;
		if(qAbs(y[0])>1.e10 || qAbs(y[1])>1.e10 || qAbs(y[2])>1.e10  || qAbs(y[3])>1.e10 ) break;

		if(i%PlotInterval==0)
		{
			if(m_bLongitudinal)
			{
				pCurve0->AppendPoint(t, y[0]*Units::mstoUnit());
				pCurve1->AppendPoint(t, y[1]*Units::mstoUnit());
				pCurve2->AppendPoint(t, y[2]*180.0/PI);//deg/s
				pCurve3->AppendPoint(t, y[3]*180.0/PI);//deg
			}
			else
			{
				pCurve0->AppendPoint(t, y[0]*Units::mstoUnit());
				pCurve1->AppendPoint(t, y[1]*180.0/PI);//deg/s
				pCurve2->AppendPoint(t, y[2]*180.0/PI);//deg/s
				pCurve3->AppendPoint(t, y[3]*180.0/PI);//deg
			}
		}
	}
	pCurve0->SetVisible(true);
	pCurve1->SetVisible(true);
	pCurve2->SetVisible(true);
	pCurve3->SetVisible(true);
}


/**
* Resets and fills the curves of the root locus graph with the data from the CWPolar objects
*/
void QMiarex::CreateStabRLCurves()
{
	WPolar *pWPolar;

	// we have eight modes, 4 longitudinal and 4 lateral
	// declare a curve for each
	static Curve *pLongCurve1,*pLongCurve2,*pLongCurve3,*pLongCurve4;
	static Curve *pLatCurve1,*pLatCurve2,*pLatCurve3,*pLatCurve4;//do it later
	
	m_LongRLGraph.DeleteCurves();
	m_LatRLGraph.DeleteCurves();

	for (int k=0; k<m_poaWPolar->size(); k++)
	{
		pWPolar = (WPolar*)m_poaWPolar->at(k);
		if ((pWPolar->m_bIsVisible||pWPolar->m_bShowPoints)
			&& pWPolar->m_Alpha.size()>0 && (m_bType7 && pWPolar->polarType()==STABILITYPOLAR))
		{
			pLongCurve1 = m_LongRLGraph.AddCurve();
			pLongCurve2 = m_LongRLGraph.AddCurve();
			pLongCurve3 = m_LongRLGraph.AddCurve();
			pLongCurve4 = m_LongRLGraph.AddCurve();
			pLongCurve1->SetVisible(pWPolar->m_bIsVisible);
			pLongCurve2->SetVisible(pWPolar->m_bIsVisible);
			pLongCurve3->SetVisible(pWPolar->m_bIsVisible);
			pLongCurve4->SetVisible(pWPolar->m_bIsVisible);

			pLongCurve1->ShowPoints(pWPolar->m_bShowPoints);
			pLongCurve2->ShowPoints(pWPolar->m_bShowPoints);
			pLongCurve3->ShowPoints(pWPolar->m_bShowPoints);
			pLongCurve4->ShowPoints(pWPolar->m_bShowPoints);
	
			pLongCurve1->SetStyle(pWPolar->m_Style);
			pLongCurve1->SetColor(pWPolar->m_Color);
			pLongCurve1->SetWidth(pWPolar->m_Width);
			pLongCurve2->SetStyle(pWPolar->m_Style);
			pLongCurve2->SetColor(pWPolar->m_Color);
			pLongCurve2->SetWidth(pWPolar->m_Width);
			pLongCurve3->SetStyle(pWPolar->m_Style);
			pLongCurve3->SetColor(pWPolar->m_Color);
			pLongCurve3->SetWidth(pWPolar->m_Width);
			pLongCurve4->SetStyle(pWPolar->m_Style);
			pLongCurve4->SetColor(pWPolar->m_Color);
			pLongCurve4->SetWidth(pWPolar->m_Width);

			FillStabCurve(pLongCurve1, pWPolar, 0);
			FillStabCurve(pLongCurve2, pWPolar, 1);
			FillStabCurve(pLongCurve3, pWPolar, 2);
			FillStabCurve(pLongCurve4, pWPolar, 3);

			pLongCurve1->SetTitle(pWPolar->polarName()+"_Mode_1");
			pLongCurve2->SetTitle(pWPolar->polarName()+"_Mode_2");
			pLongCurve3->SetTitle(pWPolar->polarName()+"_Mode_3");
			pLongCurve4->SetTitle(pWPolar->polarName()+"_Mode_4");

			//Lateral modes
			pLatCurve1 = m_LatRLGraph.AddCurve();
			pLatCurve2 = m_LatRLGraph.AddCurve();
			pLatCurve3 = m_LatRLGraph.AddCurve();
			pLatCurve4 = m_LatRLGraph.AddCurve();

			pLatCurve1->SetVisible(pWPolar->m_bIsVisible);
			pLatCurve2->SetVisible(pWPolar->m_bIsVisible);
			pLatCurve3->SetVisible(pWPolar->m_bIsVisible);
			pLatCurve4->SetVisible(pWPolar->m_bIsVisible);
			pLatCurve1->ShowPoints(pWPolar->m_bShowPoints);
			pLatCurve2->ShowPoints(pWPolar->m_bShowPoints);
			pLatCurve3->ShowPoints(pWPolar->m_bShowPoints);
			pLatCurve4->ShowPoints(pWPolar->m_bShowPoints);

			pLatCurve1->SetStyle(pWPolar->m_Style);
			pLatCurve1->SetColor(pWPolar->m_Color);
			pLatCurve1->SetWidth(pWPolar->m_Width);
			pLatCurve2->SetStyle(pWPolar->m_Style);
			pLatCurve2->SetColor(pWPolar->m_Color);
			pLatCurve2->SetWidth(pWPolar->m_Width);
			pLatCurve3->SetStyle(pWPolar->m_Style);
			pLatCurve3->SetColor(pWPolar->m_Color);
			pLatCurve3->SetWidth(pWPolar->m_Width);
			pLatCurve4->SetStyle(pWPolar->m_Style);
			pLatCurve4->SetColor(pWPolar->m_Color);
			pLatCurve4->SetWidth(pWPolar->m_Width);

			FillStabCurve(pLatCurve1, pWPolar, 4);
			FillStabCurve(pLatCurve2, pWPolar, 5);
			FillStabCurve(pLatCurve3, pWPolar, 6);
			FillStabCurve(pLatCurve4, pWPolar, 7);

			pLatCurve1->SetTitle(pWPolar->polarName()+"_Mode_1");
			pLatCurve2->SetTitle(pWPolar->polarName()+"_Mode_2");
			pLatCurve3->SetTitle(pWPolar->polarName()+"_Mode_3");
			pLatCurve4->SetTitle(pWPolar->polarName()+"_Mode_4");
		}
	}
}



/**
* Draws the legend of the Cp graph
*@param painter the instance of the QPainter object associated to the active view
*@param the top left postition where the legend is to be drawn
*@param the y coordinate of the bottom of the drawing rectangle
*/
void QMiarex::DrawCpLegend(QPainter &painter, QPoint place, int bottom)
{
	painter.save();
	int LegendSize, LegendWidth, dny, x1, i, ny;
	Curve *pCurve=NULL;
	QString strong;

	LegendSize = 30;
	LegendWidth = 350;
	dny = 14;
	bottom -= 15;//margin

	QPen CurvePen;
	QPen TextPen(Settings::s_TextColor);

	ny=-1;

	for (i=0; i<m_CpGraph.GetCurveCount(); i++)
	{
		pCurve = m_CpGraph.GetCurve(i);
		if(pCurve->size())
		{
			ny++;

			if(abs(bottom)<abs(place.y() + (int)(dny*(ny+1))))
			{
				//move right
				place.rx() += LegendWidth;
				ny=0;
			}

			CurvePen.setColor(pCurve->color());
			CurvePen.setStyle(getStyle(pCurve->style()));
			CurvePen.setWidth(pCurve->width());
			painter.setPen(CurvePen);

			painter.drawLine(place.x() + (int)(1.5*LegendSize), place.y() + (int)(1.*dny*ny),
							 place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.*dny*ny));

			if(pCurve->PointsVisible())
			{
				x1 = place.x() + (int)(2.0*LegendSize);
				painter.drawRect(x1-2, place.y() + (int)(1.*dny*ny)-2,4,4);
			}

			pCurve->title(strong);
			painter.setPen(TextPen);
			painter.drawText(place.x() + (int)(3*LegendSize),place.y() + (int)(1.*dny*ny), strong);
		}
	}
	painter.restore();
}



/**
* Draws the legend for the time response graph- 4 curves
*@param painter the instance of the QPainter object associated to the active view
*@param the top left postition where the legend is to be drawn
*@param the y coordinate of the bottom of the drawing rectangle
*/
void QMiarex::DrawStabTimeLegend(QPainter &painter, QPoint place, int bottom)
{
	painter.save();
	int LegendSize, LegendWidth, dny, x1, i, ny;
	Curve *pCurve=NULL;
	QString strong;

	LegendSize = 30;
	LegendWidth = 350;
	dny = 14;
	bottom -= 15;//margin

	QPen CurvePen;
	QPen TextPen(Settings::s_TextColor);

	ny=0;

	for (i=0; i<m_TimeGraph[0].GetCurveCount(); i++)
	{
		pCurve = m_TimeGraph[0].GetCurve(i);
		if(pCurve->size() && pCurve->IsVisible())
		{
			ny++;

			if(abs(bottom)<abs(place.y() + (int)(dny*(ny+1))))
			{
				//move right
				place.rx() += LegendWidth;
				ny=0;
			}

			CurvePen.setColor(pCurve->color());
			CurvePen.setStyle(getStyle(pCurve->style()));
			CurvePen.setWidth(pCurve->width());
			painter.setPen(CurvePen);

			painter.drawLine(place.x() + (int)(1.5*LegendSize), place.y() + (int)(1.*dny*ny),
							 place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.*dny*ny));

			if(pCurve->PointsVisible())
			{
				x1 = place.x() + (int)(2.0*LegendSize);
				painter.drawRect(x1-2, place.y() + (int)(1.*dny*ny)-2,4,4);
			}

			pCurve->title(strong);
			painter.setPen(TextPen);
			painter.drawText(place.x() + (int)(3*LegendSize),
							 place.y() + (int)(1.*dny*ny)+(int)(dny/3), strong);
		}
	}
	painter.restore();
}


/**
* Draws the curve legend for the graphs in the operatinf point view
* @param painter the instance of the QPainter object associated to the active view
* @param the top left postition where the legend is to be drawn
* @param the y coordinate of the bottom of the drawing rectangle
*/
void QMiarex::DrawOppGraphLegend(QPainter &painter, QPoint place, int bottom)
{
	painter.save();

	int LegendSize, LegendWidth, ypos;
	int i, j, k,l, x1, nc, ny;

	ny=0;

//	QString str1, str2, str3, str4, str5, str6;
	LegendSize = 30;
	LegendWidth = 300;

	QStringList str; // we need to make an inventory of wings
	bool bFound;
	PlaneOpp *pPOpp = NULL;


	for (i=0; i<m_poaPOpp->size(); i++)
	{
		bFound = false;
		pPOpp = (PlaneOpp*)m_poaPOpp->at(i);
		for (j=0; j<str.size(); j++)
		{
			if (pPOpp->planeName() == str.at(j))	bFound = true;
		}
		if (!bFound)
		{
			str.append(pPOpp->planeName());
		}
	}

	painter.setBackgroundMode(Qt::TransparentMode);

	painter.setFont(Settings::s_TextFont);

	QFontMetrics fm(Settings::s_TextFont);
	ypos = fm.height();

	QPen TextPen(Settings::s_TextColor);
	painter.setPen(TextPen);
	TextPen.setWidth(1);

	QBrush LegendBrush(Settings::s_BackgroundColor);
	painter.setBrush(LegendBrush);

	QPen LegendPen;
	LegendPen.setWidth(1);

	if(m_bCurPOppOnly)
	{
		if(!m_pCurPOpp || !m_pCurPOpp->m_bIsVisible)
		{
			painter.restore();
			return;
		}
		ny++ ;

		painter.setPen(TextPen);
		painter.drawText(place.x() + (int)(1*LegendSize), place.y() + ypos*ny-(int)(ypos/2), m_pCurPlane->planeName());

		LegendPen.setColor(m_pCurPOpp->m_Color);
		LegendPen.setStyle(getStyle(m_pCurPOpp->m_Style));
		LegendPen.setWidth(m_pCurPOpp->m_Width);
		painter.setPen(LegendPen);

		painter.drawLine(place.x() + (int)(1.5*LegendSize), place.y() + (int)(1.*ypos*ny),
						 place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.*ypos*ny));

		if(m_pCurPOpp->m_bShowPoints)
		{
			x1 = place.x() + (int)(2.0*LegendSize);
			painter.drawRect(x1-2, place.y() + (int)(1.*ypos*ny)-2, 4, 4);
		}

		painter.setPen(TextPen);
		painter.drawText(place.x() + (int)(3*LegendSize),
						 place.y() + (int)(1.*ypos*ny)+(int)(ypos/3),
						 m_pCurPOpp->title());
	}
	else
	{
		bool bStarted = false;
		for (k = 0; k<str.size(); k++)
		{
			int PlanePts = 0;
			for (l=0; l < m_poaPOpp->size(); l++)
			{
				pPOpp = (PlaneOpp*)m_poaPOpp->at(l);
				if (pPOpp->m_bIsVisible && pPOpp->planeName() == str.at(k)) PlanePts++;
			}
			if (PlanePts)
			{
				int YPos = place.y() + (ny+PlanePts+2) * ypos;// bottom line of this foil's legend

				painter.setPen(TextPen);
				if (!bStarted || (abs(bottom) > abs(YPos)))
				{
					ny++;
					painter.drawText(place.x() + (int)(1*LegendSize),
								  place.y() + ypos*ny-(int)(ypos/2), str.at(k));
				}
				else
				{
				// move rigth if outside screen
					place.rx() += LegendWidth;
					ny=1;
					painter.drawText(place.x() + (int)(1*LegendSize),
									 place.y() + ypos*ny-(int)(ypos/2), str.at(k));
				}

				bStarted = true;
				for (nc=0; nc < m_poaPOpp->size(); nc++)
				{
					pPOpp = (PlaneOpp*)m_poaPOpp->at(nc);
					if(str.at(k) == pPOpp->planeName() && pPOpp->m_bIsVisible)
					{
						if(abs(bottom)<abs(place.y() + (int)(1.*ypos*ny+ypos)))
						{
							//move right
							place.rx() += LegendWidth;
							ny=2;
						}

						LegendPen.setColor(pPOpp->m_Color);
						LegendPen.setStyle(getStyle(pPOpp->m_Style));
						LegendPen.setWidth(pPOpp->m_Width);
						painter.setPen(LegendPen);

						painter.drawLine(place.x() + (int)(1.5*LegendSize), place.y() + (int)(1.*ypos*ny),
									  place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.*ypos*ny));

						if(pPOpp->m_bShowPoints)
						{
							x1 = place.x() + (int)(2.0*LegendSize);
							painter.drawRect(x1-2, place.y() + (int)(1.*ypos*ny)-2, 4, 4);
						}


						painter.setPen(TextPen);
						painter.drawText(place.x() + (int)(3*LegendSize),
									  place.y() + (int)(1.*ypos*ny)+(int)(ypos/3),
									  pPOpp->title());
						ny++ ;
					}
				}
				if (PlanePts) ny++;
			}
		}
	}
	painter.restore();
}



/**
* Draws the legend of the polar graphs
*@param painter the instance of the QPainter object associated to the active view
*@param the top left postition where the legend is to be drawn
*@param the y coordinate of the bottom of the drawing rectangle
*/
void QMiarex::DrawWPolarLegend(QPainter &painter, QPoint place, int bottom)
{
	painter.save();

	int LegendSize, LegendWidth, ypos;
	int i,j,k,l, ny, x1;

	LegendSize = 30;
	LegendWidth = 280;


	QFontMetrics fm(Settings::s_TextFont);
	ypos = fm.height();

	painter.setFont(Settings::s_TextFont);


	QPen TextPen(Settings::s_TextColor);
	painter.setPen(TextPen);
	TextPen.setWidth(1);

	QStringList strPlaneList; // we need to make an inventory of wings
	WPolar * pWPolar;

	Plane *pPlane;


	for (j=0; j<m_poaPlane->size(); j++)
	{
		pPlane = (Plane*)m_poaPlane->at(j);
		for (i=0; i<m_poaWPolar->size(); i++)
		{
			pWPolar = (WPolar*)m_poaWPolar->at(i);
			if (pWPolar->planeName()==pPlane->planeName()&&
			   (pWPolar->visible() || (pWPolar->pointsVisible()&&m_iView==STABPOLARVIEW)))
			{
				strPlaneList.append(pPlane->planeName());
				break;
			}
		}// finished inventory
	}

#if QT_VERSION >= 0x050000
	strPlaneList.sort(Qt::CaseInsensitive);
#else
	strPlaneList.sort();
#endif

	painter.setBackgroundMode(Qt::TransparentMode);
	QBrush LegendBrush(Settings::s_BackgroundColor);
	painter.setBrush(LegendBrush);

	QPen LegendPen;
	LegendPen.setWidth(1);


	ny =0;
	for (k=0; k<strPlaneList.size(); k++)
	{
		int nPlanePlrs = 0;
		for (l=0; l < m_poaWPolar->size(); l++)
		{
			pWPolar = (WPolar*)m_poaWPolar->at(l);

			if ( pWPolar->m_Alpha.size() &&
				(pWPolar->visible() || (pWPolar->pointsVisible() && pWPolar->polarType()==STABILITYPOLAR)) &&
			   ((pWPolar->polarType()==STABILITYPOLAR && m_iView==STABPOLARVIEW) || m_iView!=STABPOLARVIEW) &&
				 pWPolar->planeName() == strPlaneList.at(k) &&
			   ((pWPolar->polarType()==FIXEDSPEEDPOLAR && m_bType1) ||
			    (pWPolar->polarType()==FIXEDLIFTPOLAR  && m_bType2) ||
			    (pWPolar->polarType()==FIXEDAOAPOLAR   && m_bType4) ||
			    (pWPolar->polarType()==BETAPOLAR                  ) ||
			    (pWPolar->polarType()==STABILITYPOLAR  && m_bType7)))
			{
					nPlanePlrs++;
			}
		}

		if (nPlanePlrs)
		{
			int YPos = place.y() + (ny+nPlanePlrs+2) * ypos;// bottom line of this Plane's legend
			if(abs(bottom) > abs(YPos))
			{
				ny++;
				painter.drawText(place.x(), place.y() + ypos*ny-(int)(ypos/2), strPlaneList.at(k));
			}
			else
			{
				// move rigth if outside screen
				place.rx() += LegendWidth;
				ny=1;
				painter.setPen(TextPen);
				painter.drawText(place.x(), place.y() + ypos*ny-(int)(ypos/2), strPlaneList.at(k));
			}

			for (int nc=0; nc<m_poaWPolar->size(); nc++)
			{
				pWPolar = (WPolar*)m_poaWPolar->at(nc);
				if(strPlaneList.at(k) == pWPolar->planeName())
				{
					if(!pWPolar->m_Alpha.size())
					{
					}
					else if(m_iView==WPOLARVIEW && !pWPolar->visible())
					{
					}
					else if(m_iView==STABPOLARVIEW && (!pWPolar->visible() && !pWPolar->pointsVisible()))
					{
					}
					else if(m_iView==STABPOLARVIEW && pWPolar->polarType()!=STABILITYPOLAR)
					{
					}
					else if((pWPolar->polarType()==FIXEDSPEEDPOLAR && !m_bType1) ||
						   (pWPolar->polarType()==FIXEDLIFTPOLAR  && !m_bType2) ||
						   (pWPolar->polarType()==STABILITYPOLAR  && !m_bType7))
					{
					}
					else
					{
						LegendPen.setColor(pWPolar->m_Color);
						LegendPen.setStyle(getStyle(pWPolar->m_Style));
						LegendPen.setWidth(pWPolar->m_Width);
						painter.setPen(LegendPen);

						painter.drawLine(place.x() + (int)(0.5*LegendSize), place.y() + (int)(1.*ypos*ny),
									  place.x() + (int)(1.5*LegendSize), place.y() + (int)(1.*ypos*ny));

						if(pWPolar->m_bShowPoints)
						{
							x1 = place.x() + (int)(1.0*LegendSize);
							painter.drawRect(x1-2, place.y()-2 + (int)(1.*ypos*ny), 4, 4);
						}
						painter.setPen(TextPen);
						painter.drawText(place.x() + (int)(2.0*LegendSize),
									  place.y() + (int)(1.*ypos*ny)+(int)(ypos/3), pWPolar->polarName());
						ny++ ;
					}
				}
			}
			if(nPlanePlrs) ny++;
		}
	}
	painter.restore();
}




/**
 * Initializes the style combo box for the graph curves
 * Selects the styles of the active curve
 */
void QMiarex::FillComboBoxes(bool bEnable)
{
	if(!bEnable)
	{
		m_pctrlCurveColor->setEnabled(false);
		m_pctrlCurveStyle->setEnabled(false);
		m_pctrlCurveWidth->setEnabled(false);
		m_pctrlShowCurve->setEnabled(false);
		m_pctrlShowPoints->setEnabled(false);
	}
	else
	{
		m_pctrlCurveColor->setEnabled(true);
		m_pctrlCurveStyle->setEnabled(true);
		m_pctrlCurveWidth->setEnabled(true);
		m_pctrlShowCurve->setEnabled(true);
		m_pctrlShowPoints->setEnabled(true);
	}
	int LineWidth[5];
	for (int i=0; i<5;i++) LineWidth[i] = m_CurveWidth;
	m_pStyleDelegate->SetLineWidth(LineWidth); // the same selected width for all styles
	m_pStyleDelegate->SetLineColor(m_CurveColor);

	int LineStyle[5];
	for (int i=0; i<5;i++) LineStyle[i] = m_CurveStyle;
	m_pWidthDelegate->SetLineStyle(LineStyle); //the same selected style for all widths
	m_pWidthDelegate->SetLineColor(m_CurveColor);

	m_pctrlCurveStyle->SetLine(m_CurveStyle, m_CurveWidth, m_CurveColor);
	m_pctrlCurveWidth->SetLine(m_CurveStyle, m_CurveWidth, m_CurveColor);

	m_pctrlCurveColor->SetColor(m_CurveColor);
	m_pctrlCurveColor->SetStyle(m_CurveStyle);
	m_pctrlCurveColor->SetWidth(m_CurveWidth);

	m_pctrlCurveStyle->update();
	m_pctrlCurveWidth->update();
	m_pctrlCurveColor->update();

	m_pctrlCurveStyle->setCurrentIndex(m_CurveStyle);
	m_pctrlCurveWidth->setCurrentIndex((int)m_CurveWidth-1);
}



/**
* Fills the existing active curve with the WOpp data
*@param pWOpp  a pointer to the instance of the CWOpp object from which the data is to be extracted
*@param pGraph a pointer to the instance of the Graph object to which the curve belongs
*@param pCurve a pointer to the instance of the CCurve object to be filled with the data from the CWOpp object
*/
void QMiarex::FillWOppCurve(WingOpp *pWOpp, Graph *pGraph, Curve *pCurve)
{
	if(!pCurve) return;
	int Var = pGraph->GetYVariable();
	int nStart, i;
	if(pWOpp->m_AnalysisMethod==LLTMETHOD) nStart = 1;
	else nStart = 0;

	switch(Var)
	{
		case 0:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AppendPoint(pWOpp->m_SpanPos[i]*Units::mtoUnit(), pWOpp->m_Ai[i]);
			}
			pGraph->SetYTitle(tr("Induced Angle"));
			break;
		}
		case 1:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AppendPoint(pWOpp->m_SpanPos[i]*Units::mtoUnit(),
					pWOpp->m_Alpha + pWOpp->m_Ai[i] + pWOpp->m_Twist[i]);
			}
			pGraph->SetYTitle(tr("Total Angle"));
			break;
		}
		case 2:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AppendPoint(pWOpp->m_SpanPos[i]*Units::mtoUnit(), pWOpp->m_Cl[i]);
			}
			pGraph->SetYTitle(tr("Cl"));
			break;
		}
		case 3:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AppendPoint(pWOpp->m_SpanPos[i]*Units::mtoUnit(), pWOpp->m_Cl[i] * pWOpp->m_Chord[i]/pWOpp->m_MAChord);
			}
			pGraph->SetYTitle(tr("Local lift"));
			break;
		}
		case 4:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AppendPoint(pWOpp->m_SpanPos[i]*Units::mtoUnit(), pWOpp->m_PCd[i]);
			}
			pGraph->SetYTitle(tr("Airfoil drag"));
			break;
		}
		case 5:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AppendPoint(pWOpp->m_SpanPos[i]*Units::mtoUnit(), pWOpp->m_ICd[i]);
			}
			pGraph->SetYTitle(tr("Induced drag"));
			break;
		}
		case 6:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AppendPoint(pWOpp->m_SpanPos[i]*Units::mtoUnit(), pWOpp->m_PCd[i]+ pWOpp->m_ICd[i]);
			}
			pGraph->SetYTitle(tr("Total drag"));
			break;
		}
		case 7:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AppendPoint(pWOpp->m_SpanPos[i]*Units::mtoUnit(), (pWOpp->m_PCd[i]+ pWOpp->m_ICd[i])* pWOpp->m_Chord[i]/pWOpp->m_MAChord);
			}
			pGraph->SetYTitle(tr("Local drag"));
			break;
		}
		case 8:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AppendPoint(pWOpp->m_SpanPos[i]*Units::mtoUnit(), pWOpp->m_CmAirf[i]);
			}
			pGraph->SetYTitle(tr("Cm Airfoil"));
			break;
		}
		case 9:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AppendPoint(pWOpp->m_SpanPos[i]*Units::mtoUnit(), pWOpp->m_Cm[i]);
			}
			pGraph->SetYTitle(tr("Cm total"));
			break;
		}
		case 10:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AppendPoint(pWOpp->m_SpanPos[i]*Units::mtoUnit(), pWOpp->m_Re[i]);
			}
			pGraph->SetYTitle(tr("Re"));
			break;
		}
		case 11:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AppendPoint(pWOpp->m_SpanPos[i]*Units::mtoUnit(), pWOpp->m_XTrTop[i]);
			}
			pGraph->SetYTitle(tr("Top Trans x-Pos %"));
			break;
		}
		case 12:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AppendPoint(pWOpp->m_SpanPos[i]*Units::mtoUnit(), pWOpp->m_XTrBot[i]);
			}
			pGraph->SetYTitle(tr("Bot Trans x-Pos %"));
			break;
		}
		case 13:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AppendPoint(pWOpp->m_SpanPos[i]*Units::mtoUnit(), pWOpp->m_XCPSpanRel[i]*100.0);
			}
			pGraph->SetYTitle(tr("CP x-Pos %"));
			break;
		}
		case 14:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AppendPoint(pWOpp->m_SpanPos[i]*Units::mtoUnit(),
								pWOpp->m_BendingMoment[i] * Units::NmtoUnit());
			}
			QString str;
			Units::getMomentUnitLabel(str);
			pGraph->SetYTitle(tr("BM (") + str + ")");
			break;
		}
		default:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AppendPoint(pWOpp->m_SpanPos[i]*Units::mtoUnit(), pWOpp->m_Ai[i]);
			}
			pGraph->SetYTitle(tr("Induced Angle"));
		}
	}
}


/**
* Fills the curve of the stability graph with the data from the pWPolar oject for the seleted mode
*@param pCurve  a pointer to the instance of the CCurve object to be filled with the data from the CWPolar object
*@param pWPolar a pointer to the instance of the CWPolar object from which the data is to be extracted
*@param iMode the index of the mode for which the curve is to be created
*/
void QMiarex::FillStabCurve(Curve *pCurve, WPolar *pWPolar, int iMode)
{
	static int i;
	static double x,y;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;

	pCurve->SetSelected(-1);

	for (i=0; i<pWPolar->m_Ctrl.size(); i++)
	{
		x = pWPolar->m_EigenValue[iMode][i].real();
		y = pWPolar->m_EigenValue[iMode][i].imag()/2./PI;

		pCurve->AppendPoint(x, y);
		if(m_pCurPlane && m_pCurPOpp && m_bHighlightOpp)
		{
			if(qAbs(pWPolar->m_Ctrl[i]-m_pCurPOpp->m_Ctrl)<0.0001)
			{
				if((pWPolar->planeName()==m_pCurPlane->planeName()) && (m_pCurPOpp->m_PlrName==pWPolar->polarName()))
				{
					if(iMode==pStabView->m_iCurrentMode) pCurve->SetSelected(i);
				}
			}
		}
	}
}


/**
* Fills the polar curve object which has been created with the variable data specified by XVar and YVar.
*@param pCurve a pointer to the curve to fill with the data
*@param pWPolar a pointer to the instance of the CWPolar object from which the data will be extracted
*@param XVar the index of the variable to appear on the x-axis
*@param YVar the index of the variable to appear on the y-axis
*/
void QMiarex::FillWPlrCurve(Curve *pCurve, WPolar *pWPolar, int XVar, int YVar)
{
	bool bAdd;
	int i;
	double x,y;
	QString PlaneName;
	if(m_pCurPlane)     PlaneName=m_pCurPlane->planeName();
	static QList <double> *pX;
	static QList <double> *pY;
	pX = (QList <double> *) pWPolar->GetWPlrVariable(XVar);
	pY = (QList <double> *) pWPolar->GetWPlrVariable(YVar);

	pCurve->SetSelected(-1);

	for (i=0; i<pWPolar->m_Alpha.size(); i++)
	{
		bAdd = true;

		x = (*pX)[i];
		y = (*pY)[i];

		if((XVar==14 || XVar==17 || XVar==18) && x<0) bAdd = false;
		if((YVar==14 || YVar==17 || YVar==18) && y<0) bAdd = false;

		//Set user units
		if(XVar==15 || XVar==16 || XVar==17)  x *= Units::NtoUnit(); //force
		if(YVar==15 || YVar==16 || YVar==17)  y *= Units::NtoUnit(); //force

		if(XVar==18 || XVar==19 || XVar==20)  x *= Units::mstoUnit();//speed
		if(YVar==18 || YVar==19 || YVar==20)  y *= Units::mstoUnit();//speed

		if(XVar==22 || XVar==23 || XVar==24)  x *= Units::NmtoUnit();//moment
		if(YVar==22 || YVar==23 || YVar==24)  y *= Units::NmtoUnit();//moment

		if(XVar==25 || XVar==26 || XVar==27 ) x *= Units::mtoUnit();//force
		if(YVar==25 || YVar==26 || YVar==27 ) y *= Units::mtoUnit();//force

		if(XVar==28)                          x *= Units::NmtoUnit();//moment
		if(YVar==28)                          y *= Units::NmtoUnit();//moment

		if(XVar==34)                          x *= Units::mtoUnit();//length
		if(YVar==34)                          y *= Units::mtoUnit();//length

		if(bAdd)
		{
			pCurve->AppendPoint(x,y);
			if(m_pCurPOpp && m_bHighlightOpp)
			{
				if(qAbs(pWPolar->m_Alpha[i]-m_pCurPOpp->m_pPlaneWOpp[0]->m_Alpha)<0.0001)
				{
					if(m_pCurPOpp && m_pCurPlane
					   && pWPolar->planeName()==m_pCurPlane->planeName()
					   && m_pCurPOpp->m_PlrName ==pWPolar->polarName())
					{
						pCurve->SetSelected(i);
					}
					else if(m_pCurPOpp && m_pCurPlane
							&& pWPolar->planeName() == m_pCurPlane->planeName()
							&& pWPolar->polarName() == m_pCurPOpp->m_PlrName)
						pCurve->SetSelected(i);
				}
			}
		}
	}
}





/**
* Returns a pointer to the graph in which the point pt lies
*@param a reference to the QPoint object which holds the mouse input in client coordinates
*@param a pointer to the instance of the QGraph which contains the client point
*/
QGraph* QMiarex::GetGraph(QPoint &pt)
{
	if(m_iView==WOPPVIEW)
	{
		if(m_iWingView==ONEGRAPH)
		{
			if(m_pCurWingGraph->IsInDrawRect(pt)) return m_pCurWingGraph;
			else return NULL;
		}
		else if (m_iWingView==TWOGRAPHS)
		{
			if(m_WingGraph[0].IsInDrawRect(pt)){return m_WingGraph;}
			if(m_WingGraph[1].IsInDrawRect(pt)){return m_WingGraph+1;}
			return NULL;
		}
		else
		{
			for(int ig=0; ig<MAXGRAPHS; ig++)
			{
				if(m_WingGraph[ig].IsInDrawRect(pt)){return m_WingGraph+ig;}
			}
			return NULL;
		}
	}
	else if(m_iView==WPOLARVIEW)
	{
		if(m_iWPlrView==ONEGRAPH)
		{
			return m_pCurWPlrGraph;
		}
		else if (m_iWPlrView==TWOGRAPHS)
		{
			if(m_WPlrGraph[0].IsInDrawRect(pt)){return m_WPlrGraph;}
			if(m_WPlrGraph[1].IsInDrawRect(pt)){return m_WPlrGraph+1;}
			return NULL;
		}
		else
		{
			for(int ig=0; ig<MAXGRAPHS; ig++)
				if(m_WPlrGraph[ig].IsInDrawRect(pt)) {return m_WPlrGraph+ig;}
		}
	}
	else if(m_iView==STABTIMEVIEW)
	{

		if(m_iStabTimeView==ONEGRAPH)
		{
			return m_pCurTimeGraph;
		}
		else
		{
			for(int ig=0; ig<MAXGRAPHS; ig++)
				if(m_TimeGraph[ig].IsInDrawRect(pt)) return m_TimeGraph+ig;
		}
		return NULL;
	}
	else if(m_iView==STABPOLARVIEW)
	{
		if(m_pCurRLStabGraph && m_pCurRLStabGraph->IsInDrawRect(pt)) return m_pCurRLStabGraph;
		return NULL;
	}

	else if(m_iView==WCPVIEW)
	{
		m_pCurGraph = &m_CpGraph;
		return m_pCurGraph;
	}
	return NULL;
}




/**
* Calls the existing OpenGL lists for display in the 3D view.
* The list have been created previously by a call to GLDraw3D
*/
void QMiarex::GLCallViewLists()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	if (m_pCurWPolar && m_pCurWPolar->m_AnalysisMethod==PANELMETHOD  && W3dPrefsDlg::s_bWakePanels)
		glCallList(WINGWAKEPANELS);

	if(m_bMoments && m_pCurPOpp) glCallList(VLMMOMENTS);

	if (m_pCurPOpp && m_bStream && m_pCurPOpp->m_AnalysisMethod>=VLMMETHOD && !m_bResetglStream && glIsList(VLMSTREAMLINES) )
		glCallList(VLMSTREAMLINES);//streamlines are not rotated

	if(m_pCurPOpp && m_bSurfVelocities && m_pCurPOpp->m_AnalysisMethod>=VLMMETHOD && !m_bResetglSurfVelocities)
		glCallList(SURFACESPEEDS);

	if (m_pCurPOpp) glRotated(m_pCurPOpp->m_pPlaneWOpp[0]->m_Alpha, 0.0, 1.0, 0.0);

	if(s_bVLMPanels && m_pCurPlane)
	{
		if(!(m_b3DCp&&m_pCurPOpp) && !s_bSurfaces) glCallList(MESHBACK);
		glCallList(MESHPANELS);
	}

	if(m_bVortices && m_pCurPlane)
	{
		glCallList(VLMCTRLPTS);
		glCallList(VLMVORTICES);
	}

	if(m_b3DCp && m_pCurPOpp && m_pCurPOpp->m_AnalysisMethod>=VLMMETHOD)
	{
		glCallList(PANELCP);
	}
	if(m_bPanelForce && m_pCurPOpp && m_pCurPOpp->m_AnalysisMethod>=VLMMETHOD)
	{
		glCallList(PANELFORCEARROWS);
	}

	if(m_pCurPOpp && qAbs(m_pCurPOpp->m_Beta)>PRECISION)
		glRotated(m_pCurPOpp->m_Beta, 0.0, 0.0, 1.0);
	else if(m_pCurWPolar && m_pCurWPolar->polarType()!=BETAPOLAR && qAbs(m_pCurWPolar->m_BetaSpec)>PRECISION)
		glRotated(m_pCurWPolar->m_BetaSpec, 0.0, 0.0, 1.0);

	if(m_bXCP && m_pCurPOpp)
	{
		for(int iw=0; iw<MAXWINGS; iw++)
			if(m_pWingList[iw]) glCallList(VLMWINGLIFT+iw);
		glCallList(LIFTFORCE);
	}

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	if(s_bOutline)
	{
		for(int iw=0; iw<MAXWINGS; iw++)
			if(m_pWingList[iw])  glCallList(WINGOUTLINE+iw);

		if(m_pCurPlane && m_pCurPlane->body())
		{
			glTranslated((m_pCurPlane)->BodyPos().x, 0.0, (m_pCurPlane)->BodyPos().z);
			glCallList(BODYGEOMBASE+MAXBODIES);
			glTranslated(-(m_pCurPlane)->BodyPos().x, 0.0, -(m_pCurPlane)->BodyPos().z);
		}
	}

	if(GLLightDlg::IsLightOn())
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}

	if(s_bSurfaces)
	{
		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(m_pWingList[iw])
			{
				glCallList(WINGSURFACES+iw);
			}
		}

		if(m_pCurPlane && m_pCurPlane->body())
		{
			if(m_pCurPlane)	glTranslated((m_pCurPlane)->BodyPos().x, 0.0, (m_pCurPlane)->BodyPos().z);
			glCallList(BODYGEOMBASE);
			if(m_pCurPlane)	glTranslated(-(m_pCurPlane)->BodyPos().x, 0.0, -(m_pCurPlane)->BodyPos().z);
		}
	}
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);


	if((s_bICd || s_bVCd) && m_pCurPOpp )
	{
		for (int iw=0; iw<MAXWINGS; iw++)
			if(m_pWingList[iw]) glCallList(VLMWINGDRAG+iw);
	}

	if(m_bDownwash && m_pCurPOpp)
	{
		for (int iw=0; iw<MAXWINGS; iw++)
			if(m_pWingList[iw]) glCallList(VLMWINGWASH+iw);
	}

	if(m_bXTop && m_pCurPOpp)
	{
		for (int iw=0; iw<MAXWINGS; iw++)
			if(m_pWingList[iw]) glCallList(VLMWINGTOPTRANS+iw);
	}
	if(m_bXBot && m_pCurPOpp)
	{
		for (int iw=0; iw<MAXWINGS; iw++)
			if(m_pWingList[iw]) glCallList(VLMWINGBOTTRANS+iw);
	}
}


/**
* Creates the OpenGL lists for 3D display
*/
void QMiarex::GLDraw3D()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	if (!m_pCurPlane)
	{
		m_bResetglGeom = true;
		m_bResetglMesh = true;
		m_bResetglOpp  = true;
	}

	Body *pCurBody = NULL;
	if(m_pCurPlane) pCurBody = m_pCurPlane->body();

	ThreeDWidget *p3dWidget = (ThreeDWidget*)s_p3dWidget;
	p3dWidget->makeCurrent();
	glClearColor(Settings::s_BackgroundColor.redF(), Settings::s_BackgroundColor.greenF(), Settings::s_BackgroundColor.blueF(),0.0);

	if(!glIsList(ARCBALL))
	{
		p3dWidget->CreateArcballList(m_ArcBall, m_GLScale);
		s_GLList++;
	}


	if(!glIsList(GLLISTSPHERE))
	{
		p3dWidget->GLCreateUnitSphere();
		s_GLList++;
	}


	if(m_bResetglBody && pCurBody)
	{
		if(glIsList(BODYGEOMBASE))
		{
			glDeleteLists(BODYGEOMBASE,1);
			glDeleteLists(BODYGEOMBASE+MAXBODIES,1);
			s_GLList -=2;
		}
		if(pCurBody->m_LineType==BODYPANELTYPE)	      GLCreateBody3DFlatPanels(BODYGEOMBASE, pCurBody);
		else if(pCurBody->m_LineType==BODYSPLINETYPE) GLCreateBody3DSplines(   BODYGEOMBASE, pCurBody, GL3dBodyDlg::s_NXPoints, GL3dBodyDlg::s_NHoopPoints);

		m_bResetglBody = false;
	}


	if(m_bResetglGeom  && m_iView==W3DVIEW)
	{
		Body TranslatedBody;
		if(pCurBody && m_pCurPlane)
		{
			TranslatedBody.Duplicate(pCurBody);
			TranslatedBody.Translate(m_pCurPlane->BodyPos());
		}
		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(glIsList(WINGSURFACES+iw))
			{
				glDeleteLists(WINGSURFACES+iw,1);
				glDeleteLists(WINGSURFACES+iw+4,1);
				s_GLList-=2;
			}
			if(m_pWingList[iw])
			{
				if(pCurBody && m_pCurPlane) GLCreateGeom(m_pWingList[iw], WINGSURFACES+iw, &TranslatedBody);
				else                        GLCreateGeom(m_pWingList[iw], WINGSURFACES+iw, pCurBody);
			}
		}
		m_bResetglGeom = false;
	}

	if((m_bResetglMesh||m_bResetglWake) && m_iView==W3DVIEW)
	{
		if(glIsList(WINGWAKEPANELS))
		{
			glDeleteLists(WINGWAKEPANELS,1);
			s_GLList-=1;
		}

		if (m_pCurWPolar && m_pCurWPolar->m_AnalysisMethod==PANELMETHOD)
		{
			GLCreateMesh(WINGWAKEPANELS, Objects3D::s_WakeSize, Objects3D::s_WakePanel, Objects3D::s_WakeNode, W3dPrefsDlg::s_WakeColor, Settings::s_BackgroundColor, false);
		}
		m_bResetglWake = false;
	}

	if(m_bResetglMesh && s_bVLMPanels && m_iView==W3DVIEW)
	{
		if(glIsList(MESHPANELS))
		{
			glDeleteLists(MESHPANELS,2);
			s_GLList-=2;
		}
		GLCreateMesh(MESHPANELS, Objects3D::s_MatSize, s_pPanel, s_pNode, W3dPrefsDlg::s_VLMColor, Settings::s_BackgroundColor);
		s_GLList+=2;

		if(glIsList(VLMCTRLPTS))
		{
			glDeleteLists(VLMCTRLPTS,2);
			s_GLList-=2;
		}
		GLCreateCtrlPts(Objects3D::s_MatSize, s_pPanel);
		GLCreateVortices(Objects3D::s_MatSize, s_pPanel, s_pNode, m_pCurWPolar);
		m_bResetglMesh = false;
	}


	if((m_bResetglLift || m_bResetglOpp) && m_iView==W3DVIEW)
	{
		if(glIsList(LIFTFORCE))
		{
			glDeleteLists(LIFTFORCE,1);
			s_GLList -=1;
		}
		if(glIsList(VLMMOMENTS))
		{
			glDeleteLists(VLMMOMENTS,1);
			s_GLList -=1;
		}

		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(glIsList(VLMWINGLIFT+iw))
			{
				glDeleteLists(VLMWINGLIFT+iw,1);
				s_GLList -=1;
			}
			if(glIsList(VLMWINGTOPTRANS+iw))
			{
				glDeleteLists(VLMWINGTOPTRANS+iw,1);
				glDeleteLists(VLMWINGTOPTRANS+iw+4,1);
				s_GLList-=2;
			}
		}

		if (m_pCurPOpp && m_pCurPOpp)
		{
			for(int iw=0; iw<MAXWINGS; iw++)
			{
				if(m_pWingList[iw])
				{
					GLCreateLiftStrip(m_pWingList[iw], m_pCurWPolar, m_pWOpp[iw], VLMWINGLIFT+iw);
					GLCreateTrans(m_pWingList[iw], m_pWOpp[iw], VLMWINGTOPTRANS+iw);
				}
			}

			GLCreateLiftForce(m_pCurWPolar, m_pCurPOpp);
			GLCreateMoments(m_pCurPlane->m_Wing, m_pCurWPolar, m_pCurPOpp);
		}

		m_bResetglLift = false;
	}


	if((m_bResetglDrag || m_bResetglOpp) && m_iView==W3DVIEW)
	{
		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(glIsList(VLMWINGDRAG+iw ))
			{
				glDeleteLists(VLMWINGDRAG+iw,1);
				s_GLList -=1;
			}
		}

		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(m_pWingList[iw] && m_pWOpp[iw])
			{
				GLCreateDrag(m_pWingList[iw], m_pCurWPolar, m_pWOpp[iw], VLMWINGDRAG+iw);
			}
		}

		m_bResetglDrag = false;
	}

	if((m_bResetglDownwash || m_bResetglOpp) && m_iView==W3DVIEW)
	{
		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(glIsList(VLMWINGWASH+iw))
			{
				glDeleteLists(VLMWINGWASH+iw,1);
				s_GLList--;
			}
		}
		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(m_pWingList[iw] && m_pWOpp[iw])
			{
				GLCreateDownwash(m_pWingList[iw],m_pWOpp[iw], VLMWINGWASH+iw);
			}
		}

		m_bResetglDownwash = false;
	}

	if((m_bResetglPanelForce || m_bResetglOpp)
		&& m_iView==W3DVIEW
		&& m_pCurWPolar && m_pCurWPolar->m_AnalysisMethod!=LLTMETHOD)
	{
		if(glIsList(PANELFORCEARROWS))
		{
			glDeleteLists(PANELFORCEARROWS,1);
			s_GLList -=1;
		}
		if (m_pCurPlane && m_pCurPOpp)
		{
			GLCreatePanelForce(Objects3D::s_MatSize, s_pPanel, m_pCurWPolar, m_pCurPOpp);
		}
		m_bResetglPanelForce = false;
	}

	if((m_bResetglPanelCp || m_bResetglOpp)
		&& m_iView==W3DVIEW
		 && m_pCurWPolar && m_pCurWPolar->m_AnalysisMethod!=LLTMETHOD)
	{
		if(glIsList(PANELCP))
		{
			glDeleteLists(PANELCP,1);
			s_GLList-=1;
		}

		if (m_pCurPlane && m_pCurPOpp)
		{
			GLCreateCp(Objects3D::s_MatSize, Objects3D::s_nNodes, s_pNode, s_pPanel, m_pCurPOpp);
		}

		m_bResetglPanelCp = false;
	}

	if((m_bResetglLegend || m_bResetglOpp || m_bResetglGeom) && m_iView==W3DVIEW)
	{
		if(glIsList(WOPPCPLEGENDCLR))
		{
			glDeleteLists(WOPPCPLEGENDCLR,1);
			s_GLList -= 1;
		}
		if(m_pCurPOpp)
		{
			GLCreateCpLegendClr(m_r3DCltRect);
		}
		m_bResetglLegend = false;
	}

	if((m_bResetglStream) && m_iView==W3DVIEW)
	{
		if(glIsList(VLMSTREAMLINES))
		{
			glDeleteLists(VLMSTREAMLINES,1);
			s_GLList -=1;
		}
		if(m_bStream)
		{
			m_bStream = false; //Disable temporarily during calculation
			//no need to recalculate if not showing
			if(m_pCurPlane && m_pCurPOpp && m_pCurPOpp->m_AnalysisMethod>=VLMMETHOD)
			{
				if(!GLCreateStreamLines(m_pWingList, s_pNode, m_pCurWPolar, m_pCurPOpp))
				{
					m_bStream  = false;
					m_bResetglStream = true;
					m_pctrlStream->blockSignals(true);
					m_pctrlStream->setChecked(false);
					m_pctrlStream->blockSignals(false);
				}
				else
				{
					m_bStream  = true;
					m_bResetglStream = false;
				}
			}
		}
	}

	if((m_bResetglSurfVelocities || m_bResetglOpp) && m_iView==W3DVIEW)
	{
		if(glIsList(SURFACESPEEDS))
		{
			glDeleteLists(SURFACESPEEDS,1);
			s_GLList -=1;
		}
		if(m_bSurfVelocities)
		{
			if(m_pCurPlane && m_pCurPOpp && m_pCurPOpp->m_AnalysisMethod>=VLMMETHOD)
			{
				if(!GLCreateSurfSpeeds(s_pPanel, m_pCurWPolar, m_pCurPOpp))
				{
					m_bSurfVelocities  = false;
					m_bResetglSurfVelocities = true;
					m_pctrlSurfVel->blockSignals(true);
					m_pctrlSurfVel->setChecked(false);
					m_pctrlSurfVel->blockSignals(false);
				}
				else
				{
					m_bResetglSurfVelocities = false;
				}
			}
		}
	}

	m_bResetglOpp = false;

	QApplication::restoreOverrideCursor();
}


/**
 * Draws the point masses, the object masses, and the CG position in the OpenGL viewport
*/
void QMiarex::GLDrawMasses()
{
	ThreeDWidget *p3dWidget = (ThreeDWidget*)s_p3dWidget;
	QString MassUnit;
	Units::getWeightUnitLabel(MassUnit);

	glColor3d(W3dPrefsDlg::s_MassColor.redF(), W3dPrefsDlg::s_MassColor.greenF(), W3dPrefsDlg::s_MassColor.blueF());

	double zdist = 25.0/(double)m_r3DCltRect.width();


	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pWingList[iw])
		{
			glPushMatrix();
			{
				if(m_pCurPlane)
				{
					glTranslated(m_pCurPlane->WingLE(iw).x,
								 m_pCurPlane->WingLE(iw).y,
								 m_pCurPlane->WingLE(iw).z);
					if(m_pWingList[iw]->m_bIsFin) glTranslated(0.0,0.0, m_pWingList[iw]->m_ProjectedSpan/4.0);
					else                          glTranslated(0.0, m_pWingList[iw]->m_ProjectedSpan/4.0,0.0);


					glColor3d(0.5, 1.0, 0.5);
					p3dWidget->renderText(0.0, 0.0, zdist,
										  m_pWingList[iw]->m_WingName+
										  QString(" %1").arg(m_pWingList[iw]->m_VolumeMass*Units::kgtoUnit(), 7,'g',3)+
										  MassUnit);
				}
			}
			glPopMatrix();

			for(int im=0; im<m_pWingList[iw]->m_PointMass.size(); im++)
			{
				glPushMatrix();
				{
					if(m_pCurPlane)
					{
						glTranslated(m_pCurPlane->WingLE(iw).x,
									 m_pCurPlane->WingLE(iw).y,
									 m_pCurPlane->WingLE(iw).z);
					}
					glTranslated(m_pWingList[iw]->m_PointMass[im]->position().x,
								 m_pWingList[iw]->m_PointMass[im]->position().y,
								 m_pWingList[iw]->m_PointMass[im]->position().z);
					glColor3d(W3dPrefsDlg::s_MassColor.redF(), W3dPrefsDlg::s_MassColor.greenF(), W3dPrefsDlg::s_MassColor.blueF());
					p3dWidget->GLRenderSphere(W3dPrefsDlg::s_MassRadius/m_glScaled);
					glColor3d(Settings::s_TextColor.redF(), Settings::s_TextColor.greenF(), Settings::s_TextColor.blueF());
					p3dWidget->renderText(0.0, 0.0, 0.0 +.02,
										  m_pWingList[iw]->m_PointMass[im]->tag()
										  +QString(" %1").arg(m_pWingList[iw]->m_PointMass[im]->mass()*Units::kgtoUnit(), 7,'g',3)
										  +MassUnit);
				}
				glPopMatrix();
			}
		}
	}

	if(m_pCurPlane)
	{
        glColor3d(W3dPrefsDlg::s_MassColor.redF(), W3dPrefsDlg::s_MassColor.greenF(), W3dPrefsDlg::s_MassColor.blueF());
		for(int im=0; im<m_pCurPlane->m_PointMass.size(); im++)
		{
			glPushMatrix();
			{
				glTranslated(m_pCurPlane->m_PointMass[im]->position().x,
							 m_pCurPlane->m_PointMass[im]->position().y,
							 m_pCurPlane->m_PointMass[im]->position().z);
				glColor3d(W3dPrefsDlg::s_MassColor.redF(), W3dPrefsDlg::s_MassColor.greenF(), W3dPrefsDlg::s_MassColor.blueF());
				p3dWidget->GLRenderSphere(W3dPrefsDlg::s_MassRadius/m_glScaled);
				glColor3d(Settings::s_TextColor.redF(), Settings::s_TextColor.greenF(), Settings::s_TextColor.blueF());
				p3dWidget->renderText(0.0,0.0,0.0+.02,
								  m_pCurPlane->m_PointMass[im]->tag()
								  +QString(" %1").arg(m_pCurPlane->m_PointMass[im]->mass()*Units::kgtoUnit(), 7,'g',3)
								  +MassUnit);
			}
			glPopMatrix();
		}

	}
	if(m_pCurPlane && m_pCurPlane->body())
	{
		Body *pCurBody = m_pCurPlane->body();
//		glColor3d(W3dPrefsDlg::s_MassColor.redF()*.75, W3dPrefsDlg::s_MassColor.greenF()*.75, W3dPrefsDlg::s_MassColor.blueF()*.75);
		glColor3d(0.0, 0.0, 0.7);

		glPushMatrix();
		{
			if(m_pCurPlane)
			{
				glTranslated(m_pCurPlane->BodyPos().x,
							 m_pCurPlane->BodyPos().y,
							 m_pCurPlane->BodyPos().z);

				glColor3d(0.5, 1.0, 0.5);
				p3dWidget->renderText(0.0, 0.0, zdist,
								  pCurBody->m_BodyName+
								  QString(" %1").arg(pCurBody->m_VolumeMass*Units::kgtoUnit(), 7,'g',3)+
								  MassUnit);
			}
		}
		glPopMatrix();
        glColor3d(W3dPrefsDlg::s_MassColor.redF(), W3dPrefsDlg::s_MassColor.greenF(), W3dPrefsDlg::s_MassColor.blueF());
		for(int im=0; im<pCurBody->m_PointMass.size(); im++)
		{
			glPushMatrix();
			{
				glTranslated(pCurBody->m_PointMass[im]->position().x,pCurBody->m_PointMass[im]->position().y,pCurBody->m_PointMass[im]->position().z);
				if(m_pCurPlane)
				{
					glTranslated(m_pCurPlane->BodyPos().x,
								 m_pCurPlane->BodyPos().y,
								 m_pCurPlane->BodyPos().z);
				}

				glColor3d(W3dPrefsDlg::s_MassColor.redF(), W3dPrefsDlg::s_MassColor.greenF(), W3dPrefsDlg::s_MassColor.blueF());
				p3dWidget->GLRenderSphere(W3dPrefsDlg::s_MassRadius/m_glScaled);

				glColor3d(Settings::s_TextColor.redF(), Settings::s_TextColor.greenF(), Settings::s_TextColor.blueF());
				p3dWidget->renderText(0.0, 0.0, 0.0+.02,
								  pCurBody->m_PointMass[im]->tag()
								  +QString(" %1").arg(pCurBody->m_PointMass[im]->mass()*Units::kgtoUnit(), 7,'g',3)
								  +MassUnit);
			}
			glPopMatrix();
		}
	}
	//plot CG
	if(m_pCurPlane)
	{
		CVector CoG;
		double Mass=0.0;
		if(m_pCurPlane)
		{
			CoG = m_pCurPlane->CoG();
			Mass = m_pCurPlane->TotalMass();
		}

		glPushMatrix();
		{
			glTranslated(CoG.x,CoG.y,CoG.z);
			glColor3d(1.0, 0.5, 0.5);
			p3dWidget->GLRenderSphere(W3dPrefsDlg::s_MassRadius*2.0/m_glScaled);
			glColor3d(Settings::s_TextColor.redF(), Settings::s_TextColor.greenF(), Settings::s_TextColor.blueF());
			p3dWidget->renderText(0.0, 0.0, 0.0+.02,
							  "CoG "+QString("%1").arg(Mass*Units::kgtoUnit(), 7,'g',3)
							  +MassUnit);
		}
		glPopMatrix();
	}
}


/**
* Prints the foil names on the OpenGl viewport
*/
void QMiarex::GLDrawFoils()
{
	ThreeDWidget *p3dWidget = (ThreeDWidget*)s_p3dWidget;
	int j;
	Foil *pFoil;
    double zdist = 25.0/(double)m_r3DCltRect.width();


	glColor3d(Settings::s_TextColor.redF(), Settings::s_TextColor.greenF(), Settings::s_TextColor.blueF());

	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pWingList[iw])
		{
			for(j=0; j<m_pWingList[iw]->m_Surface.size(); j++)
			{
				pFoil = m_pWingList[iw]->m_Surface.at(j)->m_pFoilA;

				if(pFoil)
				{
                    glPushMatrix();
                    {
						glTranslated(m_pWingList[iw]->m_Surface.at(j)->m_TA.x,
									 m_pWingList[iw]->m_Surface.at(j)->m_TA.y,
									 m_pWingList[iw]->m_Surface.at(j)->m_TA.z);

                        p3dWidget->renderText(0.0,
                                              0.0,
                                              zdist,
											  pFoil->m_FoilName, Settings::s_TextFont);
                    }
                    glPopMatrix();
				}
			}
			j = m_pWingList[iw]->m_Surface.size()-1;
			pFoil = m_pWingList[iw]->m_Surface.at(j)->m_pFoilB;
			if(pFoil)
			{
                glPushMatrix();
                {
					glTranslated(m_pWingList[iw]->m_Surface.at(j)->m_TB.x,
								 m_pWingList[iw]->m_Surface.at(j)->m_TB.y,
								 m_pWingList[iw]->m_Surface.at(j)->m_TB.z);
                    p3dWidget->renderText(0.0,
                                          0.0,
                                          zdist,
										  pFoil->m_FoilName, Settings::s_TextFont);
                }
                glPopMatrix();
            }
		}
	}
}



/**
* Inverses by transposition the 3x3 rotation matrix used in the 3D display
*/
void QMiarex::GLInverseMatrix()
{
	int i,j;

	for(i=0 ; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			MatOut[j][i] = MatIn[i][j];
		}
	}
}


/**
* Renders the OpenGl 3D view
*/
void QMiarex::GLRenderView()
{
	ThreeDWidget *p3dWidget = (ThreeDWidget*)s_p3dWidget;
	p3dWidget->makeCurrent();

	GLLightDlg *pgllDlg = (GLLightDlg*)m_pglLightDlg;

	double LightFactor = qMin(1.0, m_glScaled);

	static GLdouble pts[4];
	pts[0]= 0.0; pts[1]=0.0; pts[2]=-1.0; pts[3]= m_ClipPlanePos;  //x=m_VerticalSplit
	glClipPlane(GL_CLIP_PLANE1, pts);
	if(m_ClipPlanePos>4.9999) 	glDisable(GL_CLIP_PLANE1);
	else						glEnable(GL_CLIP_PLANE1);

	// Clear the viewport
	glFlush();
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	{
		if(m_ClipPlanePos>4.9999) 	glDisable(GL_CLIP_PLANE1);
		else						glEnable(GL_CLIP_PLANE1);

		p3dWidget->GLSetupLight(m_PlaneOffset.y, LightFactor);
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		if(m_pCurPOpp && m_pCurPOpp->m_WPolarType==STABILITYPOLAR)
		{
			QString strong = QString(tr("Time =")+"%1s").arg(m_ModeTime,6,'f',3);
			p3dWidget->renderText(15, 15, strong, Settings::s_TextFont);
		}

		if(pgllDlg->isVisible())
		{
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			glPushMatrix();
			{
				glTranslated(( pgllDlg->s_XLight+ m_PlaneOffset.x)*m_GLScale,
						   ( pgllDlg->s_YLight+ m_PlaneOffset.y)*m_GLScale,
							pgllDlg->s_ZLight*m_GLScale);
				double radius = (pgllDlg->s_ZLight+2.0)/40.0*m_GLScale;
				glColor3d(pgllDlg->s_Red, pgllDlg->s_Green, pgllDlg->s_Blue);
				p3dWidget->GLRenderSphere(radius/m_glScaled);
			}
			glPopMatrix();
		}

		glLoadIdentity();
		if(m_bCrossPoint && m_bArcball)
		{
			glPushMatrix();
			{
				glTranslated(m_PlaneOffset.x, m_PlaneOffset.y,  0.0);
				m_ArcBall.RotateCrossPoint();
				glRotated(m_ArcBall.angle, m_ArcBall.p.x, m_ArcBall.p.y, m_ArcBall.p.z);
				glCallList(ARCPOINT);
			}
			glPopMatrix();
		}

		if(m_bArcball)
		{
			glPushMatrix();
			{
				glTranslated(m_PlaneOffset.x, m_PlaneOffset.y,  0.0);
				m_ArcBall.Rotate();
				glCallList(ARCBALL);
			}
			glPopMatrix();
		}
		glTranslated(m_PlaneOffset.x, m_PlaneOffset.y,  0.0);

		m_ArcBall.Rotate();

		glScaled(m_glScaled, m_glScaled, m_glScaled);
		glTranslated(m_glRotCenter.x, m_glRotCenter.y, m_glRotCenter.z);

		if(s_bAxes)  p3dWidget->GLDrawAxes(1.0/m_glScaled, W3dPrefsDlg::s_3DAxisColor, W3dPrefsDlg::s_3DAxisStyle, W3dPrefsDlg::s_3DAxisWidth);


		if(m_pCurWPolar && m_pCurWPolar->polarType()==STABILITYPOLAR)
		{
			glTranslated(m_ModeState[0], m_ModeState[1], m_ModeState[2]);
			glRotated(m_ModeState[3]*180.0/PI, 1.0, 0.0 ,0.0);
			glRotated(m_ModeState[4]*180.0/PI, 0.0, 1.0 ,0.0);
			glRotated(m_ModeState[5]*180.0/PI, 0.0, 0.0 ,1.0);
			if(qAbs(m_pCurWPolar->m_BetaSpec)>0.001) glRotated(m_pCurWPolar->m_BetaSpec, 0.0, 0.0, 1.0);
		}

		GLCallViewLists();

		if(s_bFoilNames) GLDrawFoils();

		if(s_bShowMasses) GLDrawMasses();

		glLoadIdentity();
		glDisable(GL_CLIP_PLANE1);


		if (m_b3DCp && m_pCurPOpp && m_pCurPOpp->m_AnalysisMethod>=VLMMETHOD)
		{
			glCallList(WOPPCPLEGENDCLR);
		}
		else if (m_bPanelForce && m_pCurPOpp && m_pCurPOpp->m_AnalysisMethod>=VLMMETHOD)
		{
			glCallList(WOPPCPLEGENDCLR);
        }
	}
	glPopMatrix();
	glDisable(GL_CLIP_PLANE1);
}




/**
 * Overrides the QWidget's keyPressEvent method.
 * Dispatches the key press event
 * @param event the QKeyEvent
 */
void QMiarex::keyPressEvent(QKeyEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(event->key()==Qt::Key_0 || event->text()=="0")
	{
/*		if(m_iView==WSTABVIEW)
		{
			m_iStabilityView  = STABTIMEVIEW;
			SetWPlrLegendPos();
			UpdateView();
		}*/
		return;
	}
	else if(event->key()==Qt::Key_1 || event->text()=="1")
	{
		if(m_iView==WPOLARVIEW)
		{
			m_iWPlrView  = ONEGRAPH;
			m_pCurGraph = m_WPlrGraph;
			m_pCurWPlrGraph = m_pCurGraph;
			SetWPlrLegendPos();
		}
		else if(m_iView==WOPPVIEW)
		{
			m_iWingView =ONEGRAPH;
			m_pCurGraph = m_WingGraph;
			m_pCurWingGraph = m_pCurGraph;
			SetWingLegendPos();
		}
		else if(m_iView==STABTIMEVIEW)
		{
			m_iStabTimeView =ONEGRAPH;
			m_pCurGraph = m_TimeGraph;
			m_pCurTimeGraph = m_pCurGraph;
			SetWingLegendPos();
		}
		UpdateView();
		return;
	}
	else if(event->key()==Qt::Key_2 || event->text()=="2")
	{
		if(m_iView==WPOLARVIEW)
		{
			m_iWPlrView  = ONEGRAPH;
			m_pCurGraph = m_WPlrGraph+1;
			m_pCurWPlrGraph = m_pCurGraph;
			SetWPlrLegendPos();
		}
		else if(m_iView==WOPPVIEW)
		{
			m_iWingView =ONEGRAPH;
			m_pCurGraph = m_WingGraph+1;
			m_pCurWingGraph = m_pCurGraph;
			SetWingLegendPos();
		}
		else if(m_iView==STABTIMEVIEW)
		{
			m_iStabTimeView =ONEGRAPH;
			m_pCurGraph = m_TimeGraph+1;
			m_pCurTimeGraph = m_pCurGraph;
			SetWingLegendPos();
		}
		UpdateView();
		return;
	}
	else if(event->key()==Qt::Key_3 || event->text()=="3")
	{
		if(m_iView==WPOLARVIEW)
		{
			m_iWPlrView = ONEGRAPH;
			m_pCurGraph = m_WPlrGraph+2;
			m_pCurWPlrGraph = m_pCurGraph;
			SetWPlrLegendPos();
		}
		else if(m_iView==WOPPVIEW)
		{
			m_iWingView =ONEGRAPH;
			m_pCurGraph = m_WingGraph+2;
			m_pCurWingGraph = m_pCurGraph;
			SetWingLegendPos();
		}
		else if(m_iView==STABTIMEVIEW)
		{
			m_iStabTimeView =ONEGRAPH;
			m_pCurGraph = m_TimeGraph+2;
			m_pCurTimeGraph = m_pCurGraph;
			SetWingLegendPos();
		}
		UpdateView();
		return;
	}
	else if(event->key()==Qt::Key_4 || event->text()=="4")
	{
		if(m_iView==WPOLARVIEW)
		{
			m_iWPlrView = ONEGRAPH;
			m_pCurGraph = m_WPlrGraph+3;
			m_pCurWPlrGraph = m_pCurGraph;
			SetWPlrLegendPos();
		}
		else if(m_iView==WOPPVIEW)
		{
			m_iWingView =ONEGRAPH;
			m_pCurGraph = m_WingGraph+3;
			m_pCurWingGraph = m_pCurGraph;
			SetWingLegendPos();
		}
		else if(m_iView==STABTIMEVIEW)
		{
			m_iStabTimeView =ONEGRAPH;
			m_pCurGraph = m_TimeGraph+3;
			m_pCurTimeGraph = m_pCurGraph;
			SetWingLegendPos();
		}
		UpdateView();
		return;
	}

	switch (event->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
		{
			if (event->modifiers().testFlag(Qt::AltModifier))
			{
				OnWPolarProps();
				break;
			}
			if(!m_pctrlAnalyze->hasFocus())
			{
				activateWindow();
				m_pctrlAnalyze->setFocus();
			}
			else
			{
				OnAnalyze();
			}
			event->accept();
			break;
		}
		case Qt::Key_Escape:
		{

			StopAnimate();
			if(m_pCurGraph) m_pCurGraph->DeselectPoint();

			if(pMainFrame->m_pctrl3DScalesWidget->isVisible()) pMainFrame->m_pctrl3DScalesWidget->hide();
			UpdateView();
			break;
		}

		case Qt::Key_L:
		{
			pMainFrame->OnLogFile();
			break;
		}
		case Qt::Key_T:
		{
			if(m_iView==WPOLARVIEW)
			{
				m_iWPlrView = TWOGRAPHS;
				SetWPlrLegendPos();
			}
			else if(m_iView==WOPPVIEW)
			{
				m_iWingView = TWOGRAPHS;
				SetWingLegendPos();
			}
			else if(m_iView==STABTIMEVIEW)
			{
				m_iStabTimeView = TWOGRAPHS;
				SetWingLegendPos();
			}
			SetControls();
			UpdateView();
			break;
		}
		case Qt::Key_A:
		{
			if(m_iView==WPOLARVIEW)
			{
				m_iWPlrView = ALLGRAPHS;
				SetWPlrLegendPos();
			}
			else if(m_iView==WOPPVIEW)
			{
				m_iWingView =ALLGRAPHS;
				SetWingLegendPos();
			}
			else if(m_iView==STABTIMEVIEW)
			{
				m_iStabTimeView = ALLGRAPHS;
				SetWingLegendPos();
			}
			UpdateView();
			break;
		}
		case Qt::Key_V:
		{
			if (event->modifiers().testFlag(Qt::ControlModifier) & event->modifiers().testFlag(Qt::ShiftModifier))
			{
				m_bVortices = !m_bVortices;
				if(m_bVortices) m_bResetglMesh = true;
				UpdateView();
				break;
			}
			else if(m_pCurGraph)
			{
				GraphDlg::s_ActivePage=0;
				OnGraphSettings();
			}
			break;
		}
		case Qt::Key_X:
			m_bXPressed = true;
			break;
		case Qt::Key_Y:
			m_bYPressed = true;
			break;
		case Qt::Key_G:
		{
			if(m_pCurGraph)
			{
				OnGraphSettings();
			}
			break;
		}
		case Qt::Key_H:
		{
			if((m_iView==WPOLARVIEW || m_iView==STABPOLARVIEW) && event->modifiers().testFlag(Qt::ControlModifier))
			{
				OnHighlightWOpp();
			}
			break;
		}
		case Qt::Key_R:
		{
			if(m_pCurGraph)
			{
				m_pCurGraph->SetAuto(true);
				UpdateView();
			}
			else if(m_iView==WOPPVIEW)       OnResetWingScale();
			else if(m_iView==WPOLARVIEW)     OnResetWPlrLegend();
			else if(m_iView==STABPOLARVIEW)  OnResetWPlrLegend();
			else if(m_iView==W3DVIEW)        On3DReset();

			break;
		}
		case Qt::Key_F12:
		{
			OnPlaneInertia();
			break;
		}
		case Qt::Key_F2:
		{
			OnRenameCurPlane();
			break;
		}
		case Qt::Key_F3:
		{
			if (event->modifiers().testFlag(Qt::ShiftModifier)) OnEditCurPlane();
			else                                                OnNewPlane();
			break;
		}
		case Qt::Key_F4:
		{
			On3DView();
			break;
		}
		case Qt::Key_F5:
		{
			OnWOppView();
			break;
		}
		case Qt::Key_F6:
		{
			if (event->modifiers().testFlag(Qt::ShiftModifier))         OnDefineStabPolar();
			else if (event->modifiers().testFlag(Qt::ControlModifier))  OnEditCurWPolar();
			else                                                        OnDefineWPolar();
			break;
		}
		case Qt::Key_F8:
		{
			if (event->modifiers().testFlag(Qt::ShiftModifier))        OnRootLocusView();
			else if (event->modifiers().testFlag(Qt::ControlModifier)) OnTimeView();
			else                                                       OnWPolarView();
			break;
		}
		case Qt::Key_F9:
		{
			OnCpView();
			break;
		}
		case Qt::Key_Control:
		{
			m_bArcball = true;
			UpdateView();
			break;
		}

		default:
			//			QWidget::keyPressEvent(event);
			event->ignore();
	}
}

/**
 * Dispatches the key release event
 * @param event the QKeyEvent sent by Qt
 */
void QMiarex::keyReleaseEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Control:
		{
			m_bArcball = false;
			UpdateView();
			break;
		}
		case Qt::Key_X:
			if(!event->isAutoRepeat()) m_bXPressed = false;
			break;
		case Qt::Key_Y:
			if(!event->isAutoRepeat()) m_bYPressed = false;
			break;
		default:
			event->ignore();
	}
}


/**
 * Launches the LLT analysis and updates the display after the analysis
 *@param V0 : the start angle
 *@param VMax : the maximal angle
 *@param VDelta : the increment angle
 *@param bSequence : if true, the analysis will be run for the whole range between V0 and VMax;
 * if not, the analysis will be run for V0 only
 *@param bInitCalc : if true, the starting point for convergence iterations will be reset to the default
 * if not, the iterations will start at the last calculated point
 *
*/
void QMiarex::LLTAnalyze(double V0, double VMax, double VDelta, bool bSequence, bool bInitCalc)
{
	if(!m_pCurPlane || !m_pCurWPolar) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	LLTAnalysis::s_bInitCalc = bInitCalc;
	LLTAnalysis::s_IterLim = m_LLTMaxIterations;

	m_pLLT->setLLTRange(V0, VMax, VDelta, bSequence);
	m_pLLT->setLLTData(m_pCurPlane, m_pCurWPolar);

	m_pLLTDlg->initDialog();
	m_pLLTDlg->show();
	m_pLLTDlg->Analyze();

	if(m_bLogFile && (m_pLLT->m_bError || m_pLLT->m_bWarning))
	{
	}
	else
	{
		m_pLLTDlg->hide();
	}

	SetPlaneOpp(false, V0);
	pMainFrame->UpdatePOppListBox();

    emit projectModified();
}


/**
 * Loads the user's saved settings from the configuration file and maps the data.
 *@param a pointer to the QSettings object loaded in the MainFrame class
 *@return true if the settings have been loaded successfully
 */
bool QMiarex::LoadSettings(QSettings *pSettings)
{
	QString strong;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;

	pSettings->beginGroup("Miarex");
	{
		m_bXTop         = pSettings->value("bXTop", false).toBool();
		m_bXBot         = pSettings->value("bXBot", false).toBool();
		m_bXCP          = pSettings->value("bXCP", false).toBool();
		m_bPanelForce   = pSettings->value("bPanelForce", false).toBool();
		m_bXCmRef       = pSettings->value("bXCmRef", true).toBool();
		s_bICd          = pSettings->value("bICd", true).toBool();
		s_bVCd          = pSettings->value("bVCd", true).toBool();
		s_bSurfaces     = pSettings->value("bSurfaces").toBool();
		s_bOutline      = pSettings->value("bOutline").toBool();
		s_bVLMPanels    = pSettings->value("bVLMPanels").toBool();
		s_bAxes         = pSettings->value("bAxes").toBool();
		m_b3DCp         = pSettings->value("b3DCp").toBool();
		m_bDownwash     = pSettings->value("bDownwash").toBool();
		m_bMoments      = pSettings->value("bMoments").toBool();
		s_bAutoCpScale  = pSettings->value("bAutoCpScale").toBool();
		m_bShowCpScale  = pSettings->value("bShowCpScale").toBool();
		m_bCurPOppOnly  = pSettings->value("CurWOppOnly").toBool();
		m_bShowElliptic = pSettings->value("bShowElliptic").toBool();
		m_bLogFile      = pSettings->value("LogFile").toBool();
		m_bDirichlet    = pSettings->value("Dirichlet").toBool();
		m_bResetWake    = pSettings->value("ResetWake").toBool();
		m_bShowWingCurve[0]    = pSettings->value("ShowWing").toBool();
		m_bShowWingCurve[1]    = pSettings->value("ShowWing2").toBool();
		m_bShowWingCurve[2]    = pSettings->value("ShowStab").toBool();
		m_bShowWingCurve[3]    = pSettings->value("ShowFin").toBool();
		m_bShowWingCurve[0] = true;

		PlaneOpp::s_bStoreOpps    = pSettings->value("StoreWOpp").toBool();
		m_bSequence     = pSettings->value("Sequence").toBool();
		m_bHighlightOpp = pSettings->value("HighlightOpp").toBool();
//		m_bHighlightOpp = false;

		m_AlphaMin      = pSettings->value("AlphaMin").toDouble();
		m_AlphaMax      = pSettings->value("AlphaMax").toDouble();
		m_AlphaDelta    = pSettings->value("AlphaDelta").toDouble();
		m_BetaMin       = pSettings->value("BetaMin", 0.0).toDouble();
		m_BetaMax       = pSettings->value("BetaMax", 1.0).toDouble();
		m_BetaDelta     = pSettings->value("BetaDelta", 0.5).toDouble();
		m_QInfMin       = pSettings->value("QInfMin").toDouble();
		m_QInfMax       = pSettings->value("QInfMax").toDouble();
		m_QInfDelta     = pSettings->value("QInfDelta").toDouble();
		m_ControlMin    = pSettings->value("ControlMin").toDouble();
		m_ControlMax    = pSettings->value("ControlMax").toDouble();
		m_ControlDelta  = pSettings->value("ControlDelta").toDouble();

		m_CpStyle = pSettings->value("CpStyle").toInt();
		m_CpWidth = pSettings->value("CpWidth").toInt();
		m_CpColor = pSettings->value("CpColor").value<QColor>();

		int k = pSettings->value("iView").toInt();
		if(k==0)      m_iView = WOPPVIEW;
		else if(k==1) m_iView = WPOLARVIEW;
		else if(k==2) m_iView = W3DVIEW;
		else if(k==3) m_iView = WCPVIEW;
		else if(k==4) m_iView = STABTIMEVIEW;
		else if(k==5) m_iView = STABPOLARVIEW;

		k = pSettings->value("iWingView").toInt();
		if(k==0)      m_iWingView  = ALLGRAPHS;
		else if(k==1) m_iWingView  = ONEGRAPH;
		else if(k==2) m_iWingView  = TWOGRAPHS;

		k = pSettings->value("iWPlrView").toInt();
		if(k==0)      m_iWPlrView  = ALLGRAPHS;
		else if(k==1) m_iWPlrView  = ONEGRAPH;
		else if(k==2) m_iWPlrView  = TWOGRAPHS;

		k = pSettings->value("iStabTimeView").toInt();
		if(k==0)      m_iStabTimeView  = ALLGRAPHS;
		else if(k==1) m_iStabTimeView  = ONEGRAPH;
		else if(k==2) m_iStabTimeView  = TWOGRAPHS;

		k = pSettings->value("CurrentWingGraph").toInt();
		m_pCurWingGraph = m_WingGraph+k-1;

		k = pSettings->value("CurWPlrGraph").toInt();
		m_pCurWPlrGraph = m_WPlrGraph+k-1;

		k = pSettings->value("CurRLStabGraph").toInt();
		if     (k==1) m_pCurRLStabGraph = &m_LongRLGraph;
		else if(k==2) m_pCurRLStabGraph = &m_LatRLGraph;

		if(m_iView==WOPPVIEW)            m_pCurGraph=m_pCurWingGraph;
		else if(m_iView==WPOLARVIEW)     m_pCurGraph=m_pCurWPlrGraph;
		else if(m_iView==STABPOLARVIEW)  m_pCurGraph=m_pCurRLStabGraph;

		m_LLTMaxIterations         = pSettings->value("Iter").toInt();
		GL3dBodyDlg::s_NHoopPoints  = pSettings->value("NHoopPoints").toInt();
		GL3dBodyDlg::s_NXPoints     = pSettings->value("NXPoints").toInt();
		m_InducedDragPoint  = pSettings->value("InducedDragPoint").toInt();

		s_LiftScale     = pSettings->value("LiftScale").toDouble();
		s_DragScale     = pSettings->value("DragScale").toDouble();
		s_VelocityScale = pSettings->value("VelocityScale").toDouble();

		m_WakeInterNodes    = pSettings->value("WakeInterNodes").toInt();

		m_RampTime      = pSettings->value("RampTime", 0.1).toDouble();
		m_RampAmplitude = pSettings->value("RampAmplitude", 1.0).toDouble();

		m_TotalTime         = pSettings->value("TotalTime",10.0).toDouble();
		m_Deltat            = pSettings->value("Delta_t",0.01).toDouble();

		m_TimeInput[0]      = pSettings->value("TimeIn0",0.0).toDouble();
		m_TimeInput[1]      = pSettings->value("TimeIn1",0.0).toDouble();
		m_TimeInput[2]      = pSettings->value("TimeIn2",0.0).toDouble();
		m_TimeInput[3]      = pSettings->value("TimeIn3",0.0).toDouble();
		m_bLongitudinal     = pSettings->value("DynamicsMode").toBool();
		m_StabilityResponseType = pSettings->value("StabCurveType",0).toInt();
		k = pSettings->value("TimeGraph").toInt();
		m_pCurTimeGraph = m_TimeGraph+k-1;


		for(int i=0; i<20; i++)
		{
			strong = QString("ForcedTime%1").arg(i);
			pStabView->m_Time[i] = pSettings->value(strong, (double)i).toDouble();
		}
		for(int i=0; i<20; i++)
		{
			strong = QString("ForcedAmplitude%1").arg(i);
			pStabView->m_Amplitude[i] = pSettings->value(strong, 0.0).toDouble();
		}
		pStabView->UpdateControlModelData();

		StabPolarDlg::s_StabPolar.m_bAutoInertia = pSettings->value("StabPolarAutoInertia", true).toBool();
		StabPolarDlg::s_StabPolar.m_Mass   = pSettings->value("StabPolarMass", 0.0).toDouble();
		StabPolarDlg::s_StabPolar.m_CoG.x  = pSettings->value("StabPolarCoGx", 0.0).toDouble();
		StabPolarDlg::s_StabPolar.m_CoG.y  = pSettings->value("StabPolarCoGy", 0.0).toDouble();
		StabPolarDlg::s_StabPolar.m_CoG.z  = pSettings->value("StabPolarCoGz", 0.0).toDouble();
		StabPolarDlg::s_StabPolar.m_CoGIxx = pSettings->value("StabPolarCoGIxx", 0.0).toDouble();
		StabPolarDlg::s_StabPolar.m_CoGIyy = pSettings->value("StabPolarCoGIyy", 0.0).toDouble();
		StabPolarDlg::s_StabPolar.m_CoGIzz = pSettings->value("StabPolarCoGIzz", 0.0).toDouble();
		StabPolarDlg::s_StabPolar.m_CoGIxz = pSettings->value("StabPolarCoGIxz", 0.0).toDouble();


		WPolarDlg::s_WPolar.m_bAutoInertia =	pSettings->value("bAutoInertia", false).toBool();
		WPolarDlg::s_WPolar.m_bVLM1 = pSettings->value("bVLM1").toBool();

		PlaneOpp::s_bKeepOutOpps  = pSettings->value("KeepOutOpps").toBool();

		W3dPrefsDlg::s_MassColor = pSettings->value("MassColor", QColor(100, 100, 200)).value<QColor>();

		LLTAnalysis::s_CvPrec       = pSettings->value("CvPrec").toDouble();
		LLTAnalysis::s_RelaxMax     = pSettings->value("RelaxMax").toDouble();
		LLTAnalysis::s_NLLTStations = pSettings->value("NLLTStations").toInt();

		PanelAnalysis::s_bTrefftz      = pSettings->value("Trefftz").toBool();

		Objects3D::s_MaxWakeIter       = pSettings->value("MaxWakeIter").toInt();
		Panel::s_CtrlPos       = pSettings->value("CtrlPos").toDouble();
		Panel::s_VortexPos     = pSettings->value("VortexPos").toDouble();
		Panel::s_CoreSize      = pSettings->value("CoreSize", 0.000001).toDouble();
		Wing::s_MinPanelSize   = pSettings->value("MinPanelSize").toDouble();

	}

	pSettings->endGroup();

	GL3dBodyDlg::LoadSettings(pSettings);

	GLLightDlg::LoadSettings(pSettings);

	m_CpGraph.LoadSettings(pSettings);

	for(int ig=0; ig<MAXGRAPHS; ig++)
	{
		m_TimeGraph[ig].LoadSettings(pSettings);
		m_WingGraph[ig].LoadSettings(pSettings);
		m_WPlrGraph[ig].LoadSettings(pSettings);
		SetWGraphTitles(m_WPlrGraph+ig);
	}
	m_LongRLGraph.LoadSettings(pSettings);
	m_LatRLGraph.LoadSettings(pSettings);

	SetStabGraphTitles();

	return true;
}


/**
 * Dispatches the mouse double-click event
 * @param event a point to the QMouseEvent sent by Qt
 */
void QMiarex::doubleClickEvent(QPoint pos)
{
	if(m_iView==W3DVIEW)
	{
		ThreeDWidget *p3dWidget = (ThreeDWidget*)s_p3dWidget;
		p3dWidget->setCursor(Qt::CrossCursor);

		CVector Real;
		p3dWidget->ClientToGL(pos, Real);
		if(m_r3DCltRect.contains(pos)) p3dWidget->setFocus();

		Set3DRotationCenter(pos);
		m_bPickCenter = false;
		m_pctrlPickCenter->setChecked(false);
		UpdateView();
	}
	else if(m_pCurGraph) 
	{
		TwoDWidget *p2dWidget = (TwoDWidget*)s_p2dWidget;
		p2dWidget->setCursor(Qt::CrossCursor);
		Curve *pCloseCurve = NULL;
		int n=-1;

		pCloseCurve = m_pCurGraph->GetCurvePoint(pos.x(),pos.y(), n);
		if(pCloseCurve)
		{
			QString CurveName;
			pCloseCurve->title(CurveName);
			if(m_iView==WPOLARVIEW)
			{
			}
			else if(m_iView==STABPOLARVIEW)
			{
				if(m_pCurWPolar)
				{
					if(m_pCurWPolar->polarName()==CurveName.left(CurveName.length()-7))
					{
						//then we select and highlight the mode
						int iMode = CurveName.right(1).toInt();
						MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
						StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
						pStabView->SetMode(iMode-1);
					}
				}
			}
		}
		else OnGraphSettings();
	}
}


/**
 * Dispatches the mouse move event
 * @param event a point to the QMouseEvent sent by Qt
 */
void QMiarex::mouseMoveEvent(QMouseEvent *event)
{
	if(!hasFocus()) setFocus();
	static CVector Real;
	static bool bCtrl;
	static QPoint Delta, point;
	static double xu, yu, x1, y1, xmin, xmax, ymin, ymax;
	ThreeDWidget *p3dWidget = (ThreeDWidget*)s_p3dWidget;

	Delta.setX(event->pos().x() - m_LastPoint.x());
	Delta.setY(event->pos().y() - m_LastPoint.y());
	point = event->pos();
	m_pCurGraph = GetGraph(point);

	bCtrl = false;
	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;
	if(m_iView==W3DVIEW)
	{
		p3dWidget->ClientToGL(point, Real);

		if (event->buttons() & Qt::LeftButton)
		{
			if(bCtrl)//rotate
			{
				SetViewControls();
				m_ArcBall.Move(point.x(), m_r3DCltRect.height()-point.y());
				UpdateView();
			}
			else if(m_bTrans)
			{
				//translate
				m_glViewportTrans.x += (GLfloat)(Delta.x()*2.0*m_GLScale/m_glScaled/m_r3DCltRect.width());
				m_glViewportTrans.y += (GLfloat)(Delta.y()*2.0*m_GLScale/m_glScaled/m_r3DCltRect.width());

				m_glRotCenter.x = MatOut[0][0]*m_glViewportTrans.x + MatOut[0][1]*(-m_glViewportTrans.y) + MatOut[0][2]*m_glViewportTrans.z;
				m_glRotCenter.y = MatOut[1][0]*m_glViewportTrans.x + MatOut[1][1]*(-m_glViewportTrans.y) + MatOut[1][2]*m_glViewportTrans.z;
				m_glRotCenter.z = MatOut[2][0]*m_glViewportTrans.x + MatOut[2][1]*(-m_glViewportTrans.y) + MatOut[2][2]*m_glViewportTrans.z;

				UpdateView();
			}
		}
		else if ((event->buttons() & Qt::MidButton) && !bCtrl)
		{
			SetViewControls();
			m_ArcBall.Move(point.x(), m_r3DCltRect.height()-point.y());
			UpdateView();
		}
	}
	else
	{
		if ((event->buttons() & Qt::LeftButton) && m_bTrans &&
			(m_iView==WOPPVIEW || m_iView==WPOLARVIEW || m_iView==WCPVIEW || m_iView==STABTIMEVIEW || m_iView==STABPOLARVIEW))
		{
			if(m_pCurGraph && m_bTransGraph)
			{
				// we translate the curves inside the graph
				m_pCurGraph->SetAuto(false);
				x1 =  m_pCurGraph->ClientTox(m_LastPoint.x()) ;
				y1 =  m_pCurGraph->ClientToy(m_LastPoint.y()) ;

				xu = m_pCurGraph->ClientTox(point.x());
				yu = m_pCurGraph->ClientToy(point.y());

				xmin = m_pCurGraph->GetXMin() - xu+x1;
				xmax = m_pCurGraph->GetXMax() - xu+x1;
				ymin = m_pCurGraph->GetYMin() - yu+y1;
				ymax = m_pCurGraph->GetYMax() - yu+y1;

				m_pCurGraph->SetWindow(xmin, xmax, ymin, ymax);
				UpdateView();
			}
			else if (m_pCurPlane  && ((m_iView==WOPPVIEW && m_iWingView ==ONEGRAPH)))
			{
				// we translate the Plane

				m_ptOffset.rx() += Delta.x();
				m_ptOffset.ry() += Delta.y();
				UpdateView();

			}
			else if (m_pCurPlane  && m_iView==WOPPVIEW && m_iWingView !=ONEGRAPH)
			{
				// we translate the legend
				//horizontally only
				m_WingLegendOffset.rx() += Delta.x();
				m_WingLegendOffset.ry() += Delta.y();
				UpdateView();
			}
			else if (m_iView==WPOLARVIEW)
			{
				// we translate the legend
				if(m_iWPlrView==ONEGRAPH || m_iWPlrView==ALLGRAPHS)
				{
					m_WPlrLegendOffset.rx() += Delta.x();
					m_WPlrLegendOffset.ry() += Delta.y();
					UpdateView();
				}
				else if(m_iWPlrView==TWOGRAPHS)
				{
					m_WPlrLegendOffset.rx() += Delta.x();
					m_WPlrLegendOffset.ry() += Delta.y();
					UpdateView();
				}
			}
			else if(m_iView==STABTIMEVIEW)
			{
				m_WingLegendOffset.rx() += Delta.x();
				m_WingLegendOffset.ry() += Delta.y();
				UpdateView();

			}
			else if(m_iView==STABPOLARVIEW)
			{
				m_WPlrLegendOffset.rx() += Delta.x();
				m_WPlrLegendOffset.ry() += Delta.y();
				UpdateView();
			}
		}

		else if ((event->buttons() & Qt::MidButton) && !bCtrl)
		//scaling
		{
			// we zoom the graph or the wing
			if(m_iView ==WOPPVIEW ||m_iView==WPOLARVIEW || m_iView==WCPVIEW || m_iView==STABTIMEVIEW || m_iView==STABPOLARVIEW)
			{
				if(m_pCurGraph && m_pCurGraph->IsInDrawRect(point))
				{
					//zoom graph
					m_pCurGraph->SetAuto(false);
					if(point.y()-m_LastPoint.y()<0) m_pCurGraph->Scale(1.02);
					else                            m_pCurGraph->Scale(1.0/1.02);

					UpdateView();
				}
				else if(m_pCurPlane && m_iView==WOPPVIEW)
				{
					//zoom wing
					if(point.y()-m_LastPoint.y()<0)  m_WingScale /= 1.02;
					else                             m_WingScale *= 1.02;
					UpdateView();
				}
			}
		}
		else if(m_pCurGraph && m_pCurGraph->IsInDrawRect(point))
		{
			MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
			pMainFrame->statusBar()->showMessage(QString("X =%1, Y = %2").arg(m_pCurGraph->ClientTox(event->x())).arg(m_pCurGraph->ClientToy(event->y())));
		}
	}
	m_LastPoint = point;
	event->accept();
}



/**
 * Dispatches the mouse press event
 * @param event a point to the QMouseEvent sent by Qt
 */
void QMiarex::mousePressEvent(QMouseEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
	ThreeDWidget *p3dWidget = (ThreeDWidget*)s_p3dWidget;

	static complex<double> eigenvalue;
	static int i,j,k, isel, jsel,xval, yval;
	static double diff, dmax;
	QString str,suffix;

	if (event->buttons() & Qt::MidButton)
	{
		m_bArcball = true;
		m_ArcBall.Start(event->pos().x(), m_r3DCltRect.height()-event->pos().y());
		m_bCrossPoint = true;

		Set3DRotationCenter();
		UpdateView();
	}
	else if (event->buttons() & Qt::RightButton)
	{
		QPoint point = event->pos();
		m_pCurGraph = GetGraph(point);
	}
	else if (event->buttons() & Qt::LeftButton)
	{
		QPoint point = event->pos();

		if(m_iView==W3DVIEW)
		{
		//	point is in client coordinates

			CVector Real;
			bool bCtrl = false;
			if(event->modifiers() & Qt::ControlModifier)
			{
				m_bArcball = true;
				bCtrl =true;
			}

			p3dWidget->ClientToGL(point, Real);
			if(m_r3DCltRect.contains(point)) p3dWidget->setFocus();

			if(m_bPickCenter)
			{
				Set3DRotationCenter(point);
				m_bPickCenter = false;
				m_pctrlPickCenter->setChecked(false);
			}
			else
			{
				m_ArcBall.Start(point.x(), m_r3DCltRect.height()-point.y());
				m_bCrossPoint = true;

				Set3DRotationCenter();

				if (!bCtrl)
				{
					m_bTrans = true;
					p3dWidget->setCursor(Qt::ClosedHandCursor);
				}
			}

			UpdateView();

			m_bPickCenter = false;
		}
		else
		{
			TwoDWidget *p2dWidget = (TwoDWidget*)s_p2dWidget;
			m_pCurGraph = GetGraph(point);
			if(m_r2DCltRect.contains(point)) p2dWidget->setFocus();
			
			if(m_pCurGraph && m_iView==STABPOLARVIEW
			   && m_pCurWPolar && m_pCurWPolar->polarType()==STABILITYPOLAR)
			{
				//first we try to see if the user intended to select a mode;
				//if not, then we initiate the graph translation
				
				isel = jsel = -1;
				dmax = 1.0e40;
				eigenvalue = complex<double>(m_pCurGraph->ClientTox(event->x()), m_pCurGraph->ClientToy(event->y()));

				for(i=0; i<4; i++)
				{
					if(m_bLongitudinal)
					{
						for(j=0; j<m_pCurWPolar->m_Alpha.size(); j++)
						{
							diff = abs(eigenvalue-m_pCurWPolar->m_EigenValue[i][j])/abs(eigenvalue);
							if(diff<dmax)
							{
								dmax = diff;
								isel = i;
								jsel = j;
								xval = m_pCurGraph->xToClient(m_pCurWPolar->m_EigenValue[i][j].real());
								yval = m_pCurGraph->yToClient(m_pCurWPolar->m_EigenValue[i][j].imag());
							}
						}
					}
					else
					{
						for(j=0; j<m_pCurWPolar->m_Alpha.size(); j++)
						{
							diff = abs(eigenvalue-m_pCurWPolar->m_EigenValue[i+4][j])/abs(eigenvalue);
							if(diff<dmax)
							{
								dmax = diff;
								isel = i;
								jsel = j;
								xval = m_pCurGraph->xToClient(m_pCurWPolar->m_EigenValue[i+4][j].real());
								yval = m_pCurGraph->yToClient(m_pCurWPolar->m_EigenValue[i+4][j].imag());
							}
						}
					}
				}

				//is this mode close enough ? let's say less than 10 pixels away ?
				if((xval-event->x())+(xval-event->x()) + (yval-event->y())*(yval-event->y())<20.0)
				{
					//select the mode
					pStabView->SetMode(isel);
					
					//highlight the point
					suffix = QString("_Mode_%1").arg(isel+1);

					m_pCurRLStabGraph->DeselectPoint();
					for(k=0;k<m_pCurRLStabGraph->GetCurveCount(); k++)
					{
						m_pCurRLStabGraph->GetCurve(k)->title(str);
						if(str==m_pCurWPolar->polarName()+suffix)
						{
							m_pCurRLStabGraph->GetCurve(k)->SetSelected(jsel);
							break;
						}
					}
				}
			}

			if (m_pCurPlane || (m_pCurGraph && m_pCurGraph->IsInDrawRect(point)))
			{
				m_LastPoint.rx() = point.x();
				m_LastPoint.ry() = point.y();

				m_bTrans = true;
				m_bTransGraph = true;
				if(m_pCurGraph && m_pCurGraph->IsInDrawRect(point)) m_bTransGraph = true;
				else                                                m_bTransGraph = false;

				if(m_bTrans || m_bTransGraph) p2dWidget->setCursor(Qt::ClosedHandCursor);

			}
		}
		m_PointDown = point;
		m_LastPoint = point;
	}

	event->accept();
}


/**
 * Dispatches the mouse release event
 * @param event a point to the QMouseEvent sent by Qt
 */
void QMiarex::mouseReleaseEvent(QMouseEvent *event)
{
	if(m_iView==W3DVIEW)
	{
		QGLWidget* p3dWidget = (QGLWidget*)s_p3dWidget;
		p3dWidget->setCursor(Qt::CrossCursor);

		m_bArcball = false;
		m_bCrossPoint = false;
		UpdateView();

	//	We need to re-calculate the translation vector
		int i,j;
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
				MatIn[i][j] =  m_ArcBall.ab_quat[i*4+j];

		GLInverseMatrix();
		m_glViewportTrans.x =  (MatOut[0][0]*m_glRotCenter.x + MatOut[0][1]*m_glRotCenter.y + MatOut[0][2]*m_glRotCenter.z);
		m_glViewportTrans.y = -(MatOut[1][0]*m_glRotCenter.x + MatOut[1][1]*m_glRotCenter.y + MatOut[1][2]*m_glRotCenter.z);
		m_glViewportTrans.z =  (MatOut[2][0]*m_glRotCenter.x + MatOut[2][1]*m_glRotCenter.y + MatOut[2][2]*m_glRotCenter.z);
	}
	else
	{
		TwoDWidget *p2dWidget = (TwoDWidget*)s_p2dWidget;
		p2dWidget->setCursor(Qt::CrossCursor);
	}
	m_bTrans = false;

	event->accept();
}



/**
 * Updates the display after the user has requested a switch to the OpenGL 3D view
 */
void QMiarex::On3DView()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	m_bArcball = false;

	if(m_iView==W3DVIEW)
	{
		SetControls();
		UpdateView();
		if(m_pCurWPolar && m_pCurWPolar->polarType()==STABILITYPOLAR)
		{
			pMainFrame->m_pctrlStabViewWidget->show();
		}
		return;
	}

	m_bIs3DScaleSet = false;

	m_iView = W3DVIEW;
	SetControls();

	pMainFrame->SetCentralWidget();


	UpdateView();
	return;
}



/**
 * Updates the display after the user has toggled the switch for the display of Cp coefficients
 */
void QMiarex::On3DCp()
{
	m_b3DCp = m_pctrlCp->isChecked();

	if(m_b3DCp)
	{
		s_bSurfaces = false;
		m_pctrlSurfaces->setChecked(false);
		m_bPanelForce = false;
		m_pctrlPanelForce->setChecked(false);
	}
	UpdateView();
}


/**
 * Updates the display after the user has requested a perspective view in the 3D display
 */
void QMiarex::On3DIso()
{
	SetViewControls();
	m_pctrlIso->setChecked(true);
	m_ArcBall.ab_quat[0]	= -0.65987748f;
	m_ArcBall.ab_quat[1]	=  0.38526487f;
	m_ArcBall.ab_quat[2]	= -0.64508355f;
	m_ArcBall.ab_quat[3]	=  0.0f;
	m_ArcBall.ab_quat[4]	= -0.75137258f;
	m_ArcBall.ab_quat[5]	= -0.33720365f;
	m_ArcBall.ab_quat[6]	=  0.56721509f;
	m_ArcBall.ab_quat[7]	=  0.0f;
	m_ArcBall.ab_quat[8]	=  0.000f;
	m_ArcBall.ab_quat[9]	=  0.85899049f;
	m_ArcBall.ab_quat[10]	=  0.51199043f;
	m_ArcBall.ab_quat[11]	=  0.0f;
	m_ArcBall.ab_quat[12]	=  0.0f;
	m_ArcBall.ab_quat[13]	=  0.0f;
	m_ArcBall.ab_quat[14]	=  0.0f;
	m_ArcBall.ab_quat[15]	=  1.0f;

	Set3DRotationCenter();
	UpdateView();
}



/**
 *  Updates the display after the user has requested a top view in the 3D display
*/
void QMiarex::On3DTop()
{
	SetViewControls();
	m_pctrlZ->setChecked(true);
	m_ArcBall.SetQuat(sqrt(2.0)/2.0, 0.0, 0.0, -sqrt(2.0)/2.0);
	Set3DRotationCenter();
	UpdateView();
}


/**
 * Updates the display after the user has requested a left view in the 3D display
*/
void QMiarex::On3DLeft()
{
	SetViewControls();
	m_pctrlY->setChecked(true);
	m_ArcBall.SetQuat(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0, 0.0);// rotate by 90 deg around x
	Set3DRotationCenter();
	UpdateView();
}


/**
 * Updates the display after the user has requested a front view in the 3D display
*/
void QMiarex::On3DFront()
{
	SetViewControls();
	m_pctrlX->setChecked(true);

	Quaternion Qt1(sqrt(2.0)/2.0, 0.0,           -sqrt(2.0)/2.0, 0.0);// rotate by 90 deg around y
	Quaternion Qt2(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0,           0.0);// rotate by 90 deg around x

	m_ArcBall.SetQuat(Qt1 * Qt2);
	Set3DRotationCenter();
	UpdateView();
}


/**
 * Updates the display after the user has requested a reset of the scales in the 3D view
*/
void QMiarex::On3DReset()
{
	m_glViewportTrans.Set(0.0, 0.0, 0.0);
	m_bPickCenter   = false;
	m_bIs3DScaleSet = false;
	Set3DScale();
	Set3DRotationCenter();
	UpdateView();
}


/**
 * The user has requested a modification of the styles for the 3D view
 * Launhes the dialog box, reads the data, and updates the view
*/
void QMiarex::On3DPrefs()
{
	W3dPrefsDlg w3dDlg((MainFrame*)s_pMainFrame);
	w3dDlg.InitDialog();

	w3dDlg.exec();
	m_bResetglWake = true;
	m_bResetglBody = true;
	m_bResetglGeom = true;
	m_bResetglMesh = true;
	m_bResetglOpp  = true;
	m_bResetglStream = true;

	UpdateView();
}



/**
 * The user has pressed the button which initiates a pick of the rotation center in 3D view
 * Sets the internal flag to true
 * The action will be processed in the ::mousePressEvent() function
 */
void QMiarex::On3DPickCenter()
{
	m_bPickCenter = m_pctrlPickCenter->isChecked();
}



/**
 * Updates the display after the user has requested a reset of the scale of all WOpp graphs
 */
void QMiarex::OnAllWingGraphScales()
{
	if(m_iView == WOPPVIEW)
	{
		double halfspan = m_pCurPlane->planformSpan()/2.0;

		for(int ig=0; ig<MAXGRAPHS; ig++)
		{
			m_WingGraph[ig].SetAuto(true);
			m_WingGraph[ig].ResetXLimits();
			m_WingGraph[ig].ResetYLimits();
			m_WingGraph[ig].SetAutoX(false);
			if(m_bHalfWing) m_WingGraph[ig].SetXMin(0.0);
			else m_WingGraph[ig].SetXMin(-halfspan*Units::mtoUnit());
			m_WingGraph[ig].SetXMax( halfspan*Units::mtoUnit());
		}
	}
	else if(m_iView==STABTIMEVIEW)
	{
		for(int ig=0; ig<MAXGRAPHS; ig++)
		{
			m_TimeGraph[ig].SetAuto(true);
			m_TimeGraph[ig].ResetXLimits();
			m_TimeGraph[ig].ResetYLimits();
		}
	}
	UpdateView();
}


/**
 * Updates the display after the user has requested a reset of the scale of all WPolar graphs
*/
void QMiarex::OnAllWPolarGraphScales()
{
	if(m_iView==WPOLARVIEW)
	{
		for(int ig=0; ig<MAXGRAPHS; ig++)
		{
			m_WPlrGraph[ig].SetAuto(true);
			m_WPlrGraph[ig].ResetXLimits();
			m_WPlrGraph[ig].ResetYLimits();
		}
	}
	else if(m_iView==WOPPVIEW)
	{
		SetWGraphScale();

	}
	else if(m_iView==WCPVIEW)
	{
		m_CpGraph.SetAuto(true);
		m_CpGraph.ResetXLimits();
		m_CpGraph.ResetYLimits();
		m_CpGraph.SetInverted(true);
	}
	else if(m_iView==STABTIMEVIEW)
	{
		for(int ig=0; ig<MAXGRAPHS; ig++)
		{
			m_TimeGraph[ig].SetAuto(true);
			m_TimeGraph[ig].ResetXLimits();
			m_TimeGraph[ig].ResetYLimits();
		}
	}
	UpdateView();
}


/**
 * The user has requested an edition of the settings for all WOpp graphs
 * Launches the dialog box and updates the graphs
 */
void QMiarex::OnAllWingGraphSettings()
{
	QGraph graph;
	graph.CopySettings(m_WingGraph);
	GraphDlg grDlg((MainFrame*)s_pMainFrame);
	grDlg.m_pMemGraph = &graph;
	grDlg.m_pGraph    = m_WingGraph;
	grDlg.m_GraphArray[0] = m_WingGraph;
	grDlg.m_GraphArray[1] = m_WingGraph+1;
	grDlg.m_GraphArray[2] = m_WingGraph+2;
	grDlg.m_GraphArray[3] = m_WingGraph+3;
	grDlg.m_NGraph = 4;
	grDlg.SetParams();

	if(grDlg.exec() == QDialog::Accepted)
	{
	}
	else
	{
		for(int ig=0; ig<MAXGRAPHS; ig++) m_WingGraph[ig].CopySettings(&graph);
	}
	UpdateView();
}


/**
 * The user has requested an edition of the settings for all WPolar graphs
 * Launches the dialog box and updates the graphs
 */
void QMiarex::OnAllWPolarGraphSettings()
{
	QGraph graph;
	graph.CopySettings(m_WPlrGraph);
	GraphDlg grDlg((MainFrame*)s_pMainFrame);
	grDlg.m_pMemGraph = &graph;
	grDlg.m_pGraph    = m_WPlrGraph;
	grDlg.m_GraphArray[0] = m_WPlrGraph;
	grDlg.m_GraphArray[1] = m_WPlrGraph+1;
	grDlg.m_GraphArray[2] = m_WPlrGraph+2;
	grDlg.m_GraphArray[3] = m_WPlrGraph+3;
	grDlg.m_NGraph = 4;
	grDlg.SetParams();

	if(grDlg.exec() == QDialog::Accepted)
	{
	}
	else
	{
		for(int ig=0; ig<MAXGRAPHS; ig++)
			m_WPlrGraph[ig].CopySettings(&graph);
	}
	UpdateView();
}

/**
 * Updates the display following a user toggl of the axis the display in the 3D view
 */
void QMiarex::OnAxes()
{
	s_bAxes = m_pctrlAxes->isChecked();
	UpdateView();
}


/**
 * The user has requested a launch of the analysis
 * Reads a last time the intput parameters from th control box
 * Checks the foils
 * Launches the analysis
 * Updates the active view
*/
void QMiarex::OnAnalyze()
{
	int l;
	double V0, VMax, VDelta;
	bool bHigh = m_bHighlightOpp;
	m_bHighlightOpp = false;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(!m_pCurPlane)
	{
		QMessageBox::warning(pMainFrame, tr("Warning"), tr("Please define a plane object before running a calculation"));
		return;
	}
	if(!m_pCurWPolar)
	{
		QMessageBox::warning(pMainFrame, tr("Warning"), tr("Please define an analysis/polar before running a calculation"));
		return;
	}
	
	//prevent an automatic and lengthy redraw of the streamlines after the calculation
	m_bStream = m_bSurfVelocities = false;
	m_pctrlStream->setChecked(false);
	m_pctrlSurfVel->setChecked(false);

	// make sure that the latest parameters are loaded
	OnReadAnalysisData();

	if(m_pCurWPolar->polarType()==FIXEDAOAPOLAR)
	{
		V0     = m_QInfMin;
		VMax   = m_QInfMax;
		VDelta = m_QInfDelta;
	}
	else if(m_pCurWPolar->polarType()==STABILITYPOLAR)
	{
		V0     = m_ControlMin;
		VMax   = m_ControlMax;
		VDelta = m_ControlDelta;
	}
	else if(m_pCurWPolar->polarType()==BETAPOLAR)
	{
		V0     = m_BetaMin;
		VMax   = m_BetaMax;
		VDelta = m_BetaDelta;
	}
	else if(m_pCurWPolar->polarType() <FIXEDAOAPOLAR)
	{
		V0     = m_AlphaMin;
		VMax   = m_AlphaMax;
		VDelta = m_AlphaDelta;
	}
	else
	{
		V0 = VMax = VDelta = 0.0;
	}

	// check if all the foils are in the database...
	// ...could have been deleted or renamed or not imported with AVL wing or whatever
	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pWingList[iw])
		{
			for (l=0; l<m_pWingList[iw]->NWingSection(); l++)
			{
				if (!Foil::foil(m_pWingList[iw]->RightFoil(l)))
				{
					QString strong;
					strong = m_pWingList[iw]->m_WingName + ": "+tr("Could not find the wing's foil ")+ m_pWingList[iw]->RightFoil(l) +tr("...\nAborting Calculation");
					QMessageBox::warning(pMainFrame, tr("Warning"), strong);
					return;
				}
				if (!Foil::foil(m_pWingList[iw]->LeftFoil(l)))
				{
					QString strong;
					strong = m_pWingList[iw]->m_WingName + ": "+tr("Could not find the wing's foil ")+ m_pWingList[iw]->LeftFoil(l) +tr("...\nAborting Calculation");
					QMessageBox::warning(pMainFrame, tr("Warning"), strong);
					return;
				}
			}
		}
	}

	m_pctrlAnalyze->setEnabled(false);
	pMainFrame->m_pctrlPlane->setEnabled(false);
	pMainFrame->m_pctrlPlanePolar->setEnabled(false);
	pMainFrame->m_pctrlPlaneOpp->setEnabled(false);

	if(m_pCurWPolar->m_AnalysisMethod==LLTMETHOD)
	{
		LLTAnalyze(V0, VMax, VDelta, m_bSequence, m_bInitLLTCalc);
	}
	else if(m_pCurWPolar->m_AnalysisMethod==PANELMETHOD)
	{
		if(Objects3D::s_MatSize>0 && s_pPanel && s_pNode)
		{
			PanelAnalyze(V0, VMax, VDelta, m_bSequence);
		}
	}
	else if(m_pCurWPolar->polarType()==STABILITYPOLAR)
	{
		if(Objects3D::s_MatSize>0 && s_pPanel && s_pNode)
		{
			PanelAnalyze(V0, VMax, VDelta, m_bSequence);
		}
	}

	m_pctrlAnalyze->setEnabled(true);
	pMainFrame->m_pctrlPlane->setEnabled(true);
	pMainFrame->m_pctrlPlanePolar->setEnabled(true);
	pMainFrame->m_pctrlPlaneOpp->setEnabled(true);

	//restore things as they were
	m_bHighlightOpp = bHigh;

	//refresh the view
	s_bResetCurves = true;
	UpdateView();
	SetControls();
	pMainFrame->setFocus();
}



/**
 * Launches the animation of the WOpp display
 * Will display all the available WOpps for this WPolar in sequence
*/
void QMiarex::OnAnimateWOpp()
{
	if(!m_pCurPlane || !m_pCurWPolar || m_iView==WPOLARVIEW)
	{
		m_bAnimateWOpp = false;
		return;
	}
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	PlaneOpp*pPOpp;
	int l;

	if(m_pctrlWOppAnimate->isChecked())
	{
		if(m_pCurPlane)
		{
			for (l=0; l< m_poaPOpp->size(); l++)
			{
				pPOpp = (PlaneOpp*)m_poaPOpp->at(l);

				if (pPOpp &&
					pPOpp->polarName() == m_pCurWPolar->polarName() &&
					pPOpp->planeName() == m_pCurPlane->planeName())
				{
						if(m_pCurPOpp->m_Alpha - pPOpp->m_Alpha<0.0001)
							m_posAnimateWOpp = l;
				}
			}
		}

		m_bAnimateWOpp  = true;
		int speed = m_pctrlAnimateWOppSpeed->value();
		m_pTimerWOpp->setInterval(800-speed);
		m_pTimerWOpp->start();
	}
	else
	{
		StopAnimate();
	}
	QApplication::restoreOverrideCursor();
}


/**
 * A signal has been received from the timer to update the 3D mode display
 * So calculates the state corresponding to the time m_ModeTime and displays it
 *@param if true, the time position of the modal response will be incremented after the display
*/
void QMiarex::OnAnimateModeSingle(bool bStep)
{
	static double t, sigma, s2, omega, o2, theta_sum, psi_sum, norm;
	double *vabs, *phi;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;

	if(m_iView!=W3DVIEW)
	{
		m_pTimerMode->stop();
		return; //nothing to animate
	}
	if(!m_pCurPlane || !m_pCurWPolar || m_pCurWPolar->polarType()!=STABILITYPOLAR || !m_pCurPOpp)
	{
		m_pTimerMode->stop();
		return; //nothing to animate
	}

	//read the data, since the user may have been playing with it
	norm = m_ModeNorm * pStabView->m_ModeAmplitude;
	vabs = pStabView->m_vabs;
	phi  = pStabView->m_phi;

	// calculate the new state
	sigma = m_pCurPOpp->m_EigenValue[pStabView->m_iCurrentMode].real();
	omega = m_pCurPOpp->m_EigenValue[pStabView->m_iCurrentMode].imag();
	s2 = sigma*sigma;
	o2 = omega*omega;
	t=m_ModeTime;

	if(t>=100) StopAnimate();


	if(s2+o2>PRECISION)
	{
		if(m_bLongitudinal)
		{
			//x, z, theta are evaluated by direct integration of u, w, q
			m_ModeState[1] = 0.0;
			m_ModeState[3] = 0.0;
			m_ModeState[5] = 0.0;
			m_ModeState[0] = norm*vabs[0]*exp(sigma*t)/(s2+o2) * (sigma*cos(omega*t+phi[0])+omega*sin(omega*t+phi[0]));
			m_ModeState[2] = norm*vabs[1]*exp(sigma*t)/(s2+o2) * (sigma*cos(omega*t+phi[1])+omega*sin(omega*t+phi[1]));
			m_ModeState[4] = norm*vabs[2]*exp(sigma*t)/(s2+o2) * (sigma*cos(omega*t+phi[2])+omega*sin(omega*t+phi[2]));
	//		m_ModeState[4] = norm*vabs[3]*exp(sigma*t)*cos(omega*t+phi[3]);

			//add u0 x theta_sum to z component
			theta_sum      = norm*vabs[3]*exp(sigma*t)/(s2+o2) * (sigma*cos(omega*t+phi[3])+omega*sin(omega*t+phi[3]));
			m_ModeState[2] -= theta_sum *m_pCurPOpp->m_pPlaneWOpp[0]->m_QInf;
		}
		else
		{
			//y, phi, psi evaluation
			m_ModeState[0] = 0.0;
			m_ModeState[2] = 0.0;
			m_ModeState[4] = 0.0;

			// integrate (v+u0.psi.cos(theta0)) to get y
			m_ModeState[1] = norm*vabs[0]*exp(sigma*t)/(s2+o2) * (sigma*cos(omega*t+phi[0])+omega*sin(omega*t+phi[0]));

			//integrate psi = integrate twice r (thanks Matlab !)
			psi_sum =   sigma * ( sigma * cos(omega*t+phi[2]) + omega * sin(omega*t+phi[2]))
					  + omega * (-omega * cos(omega*t+phi[2]) + sigma * sin(omega*t+phi[2]));
			psi_sum *= vabs[2] * exp(sigma*t)/(s2+o2)/(s2+o2);

			m_ModeState[1] += norm * m_pCurPOpp->m_pPlaneWOpp[0]->m_QInf * psi_sum;

			// get directly phi from fourth eigenvector component (alternatively integrate p+r.tan(theta0));
			m_ModeState[3] = norm*vabs[3]*exp(sigma*t)*cos(omega*t+phi[3]);
	//		m_ModeState[3] = norm*vabs[1]*exp(sigma*t)/(s2+o2) * (sigma*cos(omega*t+phi[1])+omega*sin(omega*t+phi[1]));

			// integrate once 'p+r.sin(theta0)' to get heading angle
			m_ModeState[5] = norm*vabs[2]*exp(sigma*t)/(s2+o2) * (sigma*cos(omega*t+phi[2])+omega*sin(omega*t+phi[2]));
		}
	}
	else
	{
		//something went wrong somewhere
        m_ModeState[0] = 0.0;
        m_ModeState[1] = 0.0;
		m_ModeState[2] = 0.0;
		m_ModeState[3] = 0.0;
		m_ModeState[4] = 0.0;
		m_ModeState[5] = 0.0;
	}
	
	//increase the time for the next update
	if(bStep) m_ModeTime += m_Modedt;
	
	UpdateView();
}


/**
* A signal has been received from the timer to update the WOPP display
* So displays the next WOpp in the sequence.
*/
void QMiarex::OnAnimateWOppSingle()
{
	static bool bIsValid, bSkipOne;
    static int size;
	static PlaneOpp *pPOpp;

	//KickIdle
	if(m_iView!=W3DVIEW && m_iView !=WOPPVIEW) return; //nothing to animate
	if(!m_pCurPlane || !m_pCurWPolar) return;

	if(m_pCurPlane)	size = m_poaPOpp->size();
	if(size<=1) return;

	bIsValid = false;
	bSkipOne = false;

	while(!bIsValid)
	{
		pPOpp = NULL;
		//Find the current position to display

		if(m_pCurPlane)
		{
			pPOpp = (PlaneOpp*)m_poaPOpp->at(m_posAnimateWOpp);
			if(!pPOpp) return;
		}
		if(m_pCurPlane)
			bIsValid =(pPOpp->m_PlrName==m_pCurWPolar->polarName()  &&  pPOpp->planeName()==m_pCurPlane->planeName());

		if (bIsValid && !bSkipOne)
		{
			if(m_pCurPlane)
			{
				m_pCurPOpp = pPOpp;
				for(int iw=0; iw<MAXWINGS;iw++)
				{
					if(m_pCurPOpp->m_pPlaneWOpp[iw]) m_pWOpp[iw] = m_pCurPOpp->m_pPlaneWOpp[iw];
					else                             m_pWOpp[iw] = NULL;
				}

			}
			m_pCurPOpp = pPOpp;
			m_bCurPOppOnly = true;

			if (m_iView==WOPPVIEW)
			{
				CreateWOppCurves();
				UpdateView();
			}
			else if (m_iView==W3DVIEW)
			{
				m_bResetglOpp      = true;
				m_bResetglDownwash = true;
				m_bResetglLift     = true;
				m_bResetglDrag     = true;
				m_bResetglWake     = true;
				m_bResetglLegend   = true;

				UpdateView();
			}
			else if(m_iView==WCPVIEW)
			{
				CreateCpCurves();
				UpdateView();
			}

			//select the PlanePOpp in the top listbox
//			pMainFrame->SelectWOpp(m_pCurPOpp->m_pPlaneWOpp[0]);
		}
		else if(bIsValid) bSkipOne = false;

		if(m_bAnimateWOppPlus)
		{
			m_posAnimateWOpp++;
			if (m_posAnimateWOpp >= size)
			{
				m_posAnimateWOpp = size-1;
				m_bAnimateWOppPlus = false;
				bSkipOne = true;
			}
		}
		else
		{
			m_posAnimateWOpp--;
			if (m_posAnimateWOpp <0)
			{
				m_posAnimateWOpp = 0;
				m_bAnimateWOppPlus = true;
				bSkipOne = true;
			}
		}

		if(m_posAnimateWOpp<0 || m_posAnimateWOpp>=size) return;
	}
}


/**
* Modfies the animation after the user has changed the animation speed for the WOpp display
*/
void QMiarex::OnAnimateWOppSpeed(int val)
{
	if(m_pTimerWOpp->isActive())
	{
		m_pTimerWOpp->setInterval(800-val);
	}
}


/**
* In the pperating point view, adjusts the graph's scale to the wing's span
*/
void QMiarex::OnAdjustToWing()
{
	if(!m_pCurPlane) return;

	double halfspan = m_pCurPlane->planformSpan()/2.0;
	double xmin = -halfspan*Units::mtoUnit();
	if(m_bHalfWing) xmin = 0.0;
	for(int ig=0; ig<MAXGRAPHS; ig++)
	{
		m_WingGraph[ig].SetAutoX(false);
		m_WingGraph[ig].SetXMax( halfspan*Units::mtoUnit());
		m_WingGraph[ig].SetXMin(xmin);
	}
}


/**
 * The user has requested an edition of the advanced settings
 * Launches the dialog box and maps the returned data
*/
void QMiarex::OnAdvancedSettings()
{
	WAdvancedDlg waDlg((MainFrame*)s_pMainFrame);
	waDlg.m_MaxWakeIter     = Objects3D::s_MaxWakeIter;

	waDlg.m_MinPanelSize    = Wing::s_MinPanelSize;
	waDlg.m_AlphaPrec       = LLTAnalysis::s_CvPrec;
	waDlg.m_Relax           = LLTAnalysis::s_RelaxMax;
	waDlg.m_NLLTStation     = LLTAnalysis::s_NLLTStations;

	waDlg.m_bTrefftz        = PanelAnalysis::s_bTrefftz;

	waDlg.m_CoreSize        = Panel::s_CoreSize;
	waDlg.m_ControlPos      = Panel::s_CtrlPos;
	waDlg.m_VortexPos       = Panel::s_VortexPos;


	waDlg.m_Iter            = m_LLTMaxIterations;
	waDlg.m_bDirichlet      = m_bDirichlet;
	waDlg.m_bKeepOutOpps    = PlaneOpp::s_bKeepOutOpps;
	waDlg.m_bLogFile        = m_bLogFile;
	waDlg.m_WakeInterNodes  = m_WakeInterNodes;

	waDlg.InitDialog();
	if(waDlg.exec() == QDialog::Accepted)
	{
		Objects3D::s_MaxWakeIter     = waDlg.m_MaxWakeIter;
		Wing::s_MinPanelSize         = waDlg.m_MinPanelSize;
		LLTAnalysis::s_CvPrec        = waDlg.m_AlphaPrec;
		LLTAnalysis::s_RelaxMax      = waDlg.m_Relax;
		LLTAnalysis::s_NLLTStations  = waDlg.m_NLLTStation;

		PanelAnalysis::s_bTrefftz  = waDlg.m_bTrefftz;

		Panel::s_CoreSize          = waDlg.m_CoreSize;
		Panel::s_CtrlPos           = waDlg.m_ControlPos;
		Panel::s_VortexPos         = waDlg.m_VortexPos;

		PlaneOpp::s_bKeepOutOpps         = waDlg.m_bKeepOutOpps;

		m_LLTMaxIterations     = waDlg.m_Iter;
		m_bDirichlet           = waDlg.m_bDirichlet;
		m_WakeInterNodes       = waDlg.m_WakeInterNodes;
		m_InducedDragPoint     = waDlg.m_InducedDragPoint;


		if(waDlg.m_bLogFile) m_bLogFile = true; else m_bLogFile = false;
		m_bResetglWake    = true;
		UpdateView();
	}
}


/**
* The user has modified the position of the clip plane in the 3D view
*@param pos the new z position in viewport coordinates of the clipping plane
*/
void QMiarex::OnClipPlane(int pos)
{
	double planepos =  (double)pos/100.0;
	m_ClipPlanePos = sinh(planepos) * 0.5;
	UpdateView();
}



/**
* The user has modified the position span section to display in the Cp view
*/
void QMiarex::OnCpSection(int pos)
{
	m_CurSpanPos = (double)pos/100.0;
	m_pctrlSpanPos->SetValue(m_CurSpanPos);
	CreateCpCurves();
	UpdateView();
}


/**
* The user has modified the position span section to display in the Cp view
*/
void QMiarex::OnCpPosition()
{
	m_CurSpanPos = m_pctrlSpanPos->Value();
	m_pctrlCpSectionSlider->setValue((int)(m_CurSpanPos*100.0));
	CreateCpCurves();
	UpdateView();
}


/**
* The user has switched to the Cp view
*/
void QMiarex::OnCpView()
{
	if (m_bAnimateWOpp) StopAnimate();

	if(m_iView==WCPVIEW)
	{
		SetControls();
		UpdateView();
		return;
	}
	m_iView=WCPVIEW;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetCentralWidget();

//	SetWPlrLegendPos();
	m_pCurGraph = &m_CpGraph;

	CreateCpCurves();
	SetCurveParams();
	SetControls();
	UpdateView();
}



/**
* The user has requested a display only of the current operating point
*/
void QMiarex::OnCurWOppOnly()
{
	m_bCurPOppOnly = !m_bCurPOppOnly;
	s_bResetCurves = true;
	UpdateView();
	SetControls();
}


/**
* The user has requested a change to the color of the active curve.
* The curve may be for a polar curve or for an oppoint.
* Changes the style and modifies the content of the comboboxes
*/
void QMiarex::OnCurveColor()
{
	QColor Color = QColorDialog::getColor(m_CurveColor);
	if(Color.isValid()) m_CurveColor = Color;
	FillComboBoxes();

	UpdateCurve();
}


/**
* The user has requested a change to the style of the active curve.
* The curve may be for a polar curve or for an oppoint.
* Changes the style and modifies the content of the comboboxes
*/
void QMiarex::OnCurveStyle(int index)
{
	m_CurveStyle = index;
	FillComboBoxes();
	UpdateCurve();
}



/**
* The user has requested a change to the width of the active curve.
* The curve may be for a polar curve or for an oppoint.
* Changes the style and modifies the content of the comboboxes
*/
void QMiarex::OnCurveWidth(int index)
{
	m_CurveWidth = index+1;
	FillComboBoxes();
	UpdateCurve();
}


/**
* The user has requested the creation of a new stability polar
*/
void QMiarex::OnDefineStabPolar()
{
	if(!m_pCurPlane) return;
	StopAnimate();
	m_bArcball = false;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	StabPolarDlg::s_StabPolar.m_Viscosity     = WPolarDlg::s_WPolar.m_Viscosity;
	StabPolarDlg::s_StabPolar.m_Density       = WPolarDlg::s_WPolar.m_Density;
	StabPolarDlg::s_StabPolar.m_RefAreaType   = WPolarDlg::s_WPolar.m_RefAreaType;
	StabPolarDlg::s_StabPolar.m_bThinSurfaces = WPolarDlg::s_WPolar.m_bThinSurfaces;


	StabPolarDlg spDlg(pMainFrame);
	spDlg.InitDialog(m_pCurPlane);
	int res = spDlg.exec();

	if(res == QDialog::Accepted)
	{
		emit projectModified();

		WPolar* pNewStabPolar      = new WPolar;
		pNewStabPolar->setPlaneName(m_pCurPlane->planeName());
		pNewStabPolar->m_Color = MainFrame::GetColor(4);
		pNewStabPolar->m_Width = 2;
		pNewStabPolar->m_bShowPoints = true;
		pNewStabPolar->m_bIsVisible  = true;

		pNewStabPolar->m_WMAChord  = m_pCurPlane->mac();

		pNewStabPolar->DuplicateSpec(&StabPolarDlg::s_StabPolar);


		if(pNewStabPolar->polarName().length()>60)
		{
			pNewStabPolar->polarName() = pNewStabPolar->polarName().left(60)+"..."+QString("(%1)").arg(m_poaWPolar->size());
		}

		if(pNewStabPolar->m_RefAreaType==PLANFORMAREA)
		{
			pNewStabPolar->m_WArea        = m_pCurPlane->planformArea();
			if(m_pCurPlane && m_pCurPlane->BiPlane()) pNewStabPolar->m_WArea += m_pCurPlane->wing2()->m_PlanformArea;
			pNewStabPolar->m_WSpan        = m_pCurPlane->planformSpan();
		}
		else
		{
			pNewStabPolar->m_WArea        = m_pCurPlane->projectedArea();
			if(m_pCurPlane && m_pCurPlane->BiPlane()) pNewStabPolar->m_WArea += m_pCurPlane->wing2()->m_ProjectedArea;
			pNewStabPolar->m_WSpan        = m_pCurPlane->projectedSpan();
		}
		pNewStabPolar->m_bVLM1           = false;
		pNewStabPolar->m_bDirichlet      = m_bDirichlet;
		pNewStabPolar->m_bTiltedGeom     = false;
		pNewStabPolar->m_bWakeRollUp     = false;
		pNewStabPolar->m_AnalysisMethod  = PANELMETHOD;
		pNewStabPolar->m_bGround         = false;
		pNewStabPolar->m_AlphaSpec           = 0.0;
		pNewStabPolar->m_Height          = 0.0;

		m_pCurWPolar = Objects3D::insertNewWPolar(pNewStabPolar, m_pCurPlane);
		m_pCurPOpp = NULL;

		m_bResetglGeom = true;
		m_bResetglOpp  = true;
		m_bResetglMesh = true;
		m_bResetglWake = true;

		SetWPolar();
		pMainFrame->UpdateWPolarListBox();
		UpdateView();
	}

	SetControls();
}


/**
 * The user has requested the creation of a new performance polar.
 * A new WPolar object is created and is attached to the owning plane or wing.
 */
void QMiarex::OnDefineWPolar()
{
	if(!m_pCurPlane) return;

	StopAnimate();

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	WPolar* pNewWPolar  = new WPolar;

	WPolarDlg wpDlg(pMainFrame);
	wpDlg.InitDialog(m_pCurPlane);

	int res = wpDlg.exec();

	if (res == QDialog::Accepted)
	{
		//Then add WPolar to array
		emit projectModified();
		pNewWPolar->DuplicateSpec(&WPolarDlg::s_WPolar);
		pNewWPolar->setPlaneName(m_pCurPlane->planeName());
		pNewWPolar->setPolarName(wpDlg.s_WPolar.polarName());

		pNewWPolar->m_WMAChord = m_pCurPlane->mac();

		if(pNewWPolar->m_RefAreaType==PLANFORMAREA)
		{
			pNewWPolar->m_WSpan = m_pCurPlane->planformSpan();
			pNewWPolar->m_WArea = m_pCurPlane->planformArea();
			if(m_pCurPlane && m_pCurPlane->BiPlane()) pNewWPolar->m_WArea += m_pCurPlane->wing2()->m_PlanformArea;
		}
		else
		{
			pNewWPolar->m_WSpan = m_pCurPlane->projectedSpan();
			pNewWPolar->m_WArea = m_pCurPlane->projectedArea();
			if(m_pCurPlane && m_pCurPlane->BiPlane()) pNewWPolar->m_WArea += m_pCurPlane->wing2()->m_ProjectedArea;
		}

		pNewWPolar->m_bDirichlet      = m_bDirichlet;

		pNewWPolar->m_Color = MainFrame::GetColor(4);
		pNewWPolar->m_bIsVisible = true;

		m_pCurWPolar = Objects3D::insertNewWPolar(pNewWPolar, m_pCurPlane);
		m_pCurPOpp = NULL;

		m_bResetglGeom = true;
		m_bResetglMesh = true;
		m_bResetglOpp  = true;
		m_bResetglWake = true;

		SetWPolar();


		pMainFrame->UpdateWPolarListBox();
		UpdateView();
		m_pctrlAnalyze->setFocus();
	}
	else
	{
		delete pNewWPolar;
	}
	SetControls();
}


/**
 * The user wants to edit the analysis parameters of the currently selected polar.
 * A new WPolar object is created. The user may choose to overwrite or not the existing WPolar.
 */
void QMiarex::OnEditCurWPolar()
{
	StopAnimate();

	if(!m_pCurPlane || !m_pCurWPolar) return;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	QString WPolarName;
	int res;

	WPolar *pNewWPolar = new WPolar;

	if(m_pCurWPolar->polarType()!=STABILITYPOLAR)
	{
		WPolarDlg dlg(pMainFrame);
		dlg.InitDialog(m_pCurPlane, m_pCurWPolar);
		res = dlg.exec();
		pNewWPolar->DuplicateSpec(&dlg.s_WPolar);
		WPolarName=dlg.s_WPolar.polarName();
	}
	else
	{
		StabPolarDlg dlg(pMainFrame);
		dlg.InitDialog(m_pCurPlane, m_pCurWPolar);
		res = dlg.exec();
		pNewWPolar->DuplicateSpec(&dlg.s_StabPolar);
		WPolarName=dlg.s_StabPolar.polarName();
	}

	if (res == QDialog::Accepted)
	{
		emit projectModified();

		pNewWPolar->planeName() = m_pCurPlane->planeName();
		pNewWPolar->polarName() = WPolarName;

		if(pNewWPolar->m_RefAreaType==PLANFORMAREA)
		{
			pNewWPolar->m_WSpan = m_pCurPlane->planformSpan();
			pNewWPolar->m_WArea = m_pCurPlane->planformArea();
			if(m_pCurPlane && m_pCurPlane->BiPlane()) pNewWPolar->m_WArea += m_pCurPlane->wing2()->m_PlanformArea;
		}
		else
		{
			pNewWPolar->m_WSpan = m_pCurPlane->projectedSpan();
			pNewWPolar->m_WArea = m_pCurPlane->projectedArea();
			if(m_pCurPlane && m_pCurPlane->BiPlane()) pNewWPolar->m_WArea += m_pCurPlane->wing2()->m_ProjectedArea;
		}

		pNewWPolar->m_bDirichlet      = m_bDirichlet;
//		pNewWPolar->m_bAVLControls    = false;

		pNewWPolar->m_Color = MainFrame::GetColor(4);
		pNewWPolar->m_bIsVisible = true;

		m_pCurWPolar = Objects3D::insertNewWPolar(pNewWPolar, m_pCurPlane);
		m_pCurPOpp = NULL;

		m_bResetglGeom = true;
		m_bResetglMesh = true;
		m_bResetglOpp  = true;
		m_bResetglWake = true;

		SetWPolar();
		pMainFrame->UpdateWPolarListBox();
		UpdateView();
		m_pctrlAnalyze->setFocus();
	}
	else
	{
		delete pNewWPolar;
	}
	SetControls();
}



/**
 * The user wants to edit the analysis parameters of the currently selected polar.
 * A new WPolar object is created. The user may choose to overwrite or not the existing WPolar.
 */
void QMiarex::OnEditCurWPolarPts()
{
	StopAnimate();

	if(!m_pCurPlane || !m_pCurWPolar) return;

	WPolar *pMemWPolar = new WPolar;
	//Edit the current WPolar data
	if (!m_pCurWPolar) return;

	pMemWPolar->Copy(m_pCurWPolar);


	EditPlrDlg epDlg((MainFrame*)s_pMainFrame);
	epDlg.move(EditPlrDlg::s_Position);
	epDlg.resize(EditPlrDlg::s_WindowSize);
	if(EditPlrDlg::s_bWindowMaximized) epDlg.setWindowState(Qt::WindowMaximized);

	epDlg.InitDialog(NULL, NULL, this, m_pCurWPolar);


	bool bPoints = m_pCurWPolar->m_bShowPoints;
	m_pCurWPolar->m_bShowPoints = true;

	s_bResetCurves = true;
	UpdateView();

	if(epDlg.exec() == QDialog::Accepted)
	{
	   emit projectModified();
	}
	else
	{
		m_pCurWPolar->Copy(pMemWPolar);
	}
	m_pCurWPolar->m_bShowPoints = bPoints;

	s_bResetCurves = true;
	UpdateView();
	SetControls();
	delete pMemWPolar;
}



/**
 * The user has requested a deletion of all the WOpps or POpps associated to the active WPolar.
 */
void QMiarex::OnDeleteAllWPlrOpps()
{
	if(!m_pCurWPolar) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	emit projectModified();
	PlaneOpp* pPOpp;
	int i;
	if(m_pCurPlane)
	{
		for (i = m_poaPOpp->size()-1; i>=0; i--)
		{
			pPOpp = (PlaneOpp*) m_poaPOpp->at(i);
			if(pPOpp->m_PlrName == m_pCurWPolar->polarName() &&
			   pPOpp->planeName() == m_pCurPlane->planeName())
			{
				m_poaPOpp->removeAt(i);
				delete pPOpp;
			}
		}
	}

	m_pCurPOpp = NULL;
	m_bResetglMesh = true;
	pMainFrame->UpdatePOppListBox();
	SetPlaneOpp(true);
	SetControls();
	s_bResetCurves = true;
	UpdateView();
}


/**
 * The user has requested a deletion of all the WOpps or POpps
 */
void QMiarex::OnDeleteAllWOpps()
{

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	emit projectModified();
	PlaneOpp* pPOpp;
	int i;
	for (i = m_poaPOpp->size()-1; i>=0; i--)
	{
		pPOpp = (PlaneOpp*) m_poaPOpp->at(i);
		m_poaPOpp->removeAt(i);
		delete pPOpp;
	}

	m_pCurPOpp = NULL;
	pMainFrame->UpdatePOppListBox();

	SetPlaneOpp(true);

	SetControls();

	s_bResetCurves = true;
	UpdateView();
}


/**
* The user has requested a deletion of the current wing of plane
*/
void QMiarex::OnDeleteCurPlane()
{
	if(!m_pCurPlane) return;
	m_bAnimateWOpp = false;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong;
	if(m_pCurPlane) strong = tr("Are you sure you want to delete the plane :\n") +  m_pCurPlane->planeName() +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(pMainFrame, tr("Question"), strong, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;

	Objects3D::deletePlane(m_pCurPlane);
	m_pCurPlane = NULL;

	SetPlane();
	pMainFrame->UpdatePlaneListBox();
	SetControls();
	s_bResetCurves = true;
	emit projectModified();
	UpdateView();
}


/**
 * The user has requested a deletion of the current operating point
 */
void QMiarex::OnDeleteCurWOpp()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	int i;

	double alpha=-9999999;

	if(m_pCurPOpp)
	{
		alpha = m_pCurPOpp->m_Alpha;
		PlaneOpp* pPOpp;
		for (i = m_poaPOpp->size()-1; i>=0; i--)
		{
			pPOpp = (PlaneOpp*)m_poaPOpp->at(i);
			if(pPOpp == m_pCurPOpp)
			{
				m_poaPOpp->removeAt(i);
				delete pPOpp;
				break;
			}
		}
		m_pCurPOpp = NULL;
		pMainFrame->UpdatePOppListBox();
		SetPlaneOpp(true);
		emit projectModified();

		s_bResetCurves = true;
		UpdateView();
	}

	if(m_pCurWPolar)
	{
		m_pCurWPolar->Remove(alpha);
	}

	SetControls();
}




/**
* The user has requested a deletion of all operating point associated to the wing or plane
*/
void QMiarex::OnDeletePlaneOpps()
{
	PlaneOpp *pPOpp;
	int i;
	if(m_pCurPlane)
	{
		for (i=m_poaPOpp->size()-1; i>=0; i--)
		{
			pPOpp = (PlaneOpp*)m_poaPOpp->at(i);
			if (pPOpp->planeName() == m_pCurPlane->planeName())
			{
				m_poaPOpp->removeAt(i);
				delete pPOpp;
			}
		}
	}
	m_pCurPOpp = NULL;


	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	emit projectModified();
	pMainFrame->UpdatePOppListBox();
	SetControls();
	s_bResetCurves = true;
	UpdateView();
}



/**
* The user has requested a deletion of all WPolars associated to the wing or plane
*/
void QMiarex::OnDeletePlaneWPolars()
{
	if(!m_pCurPlane) return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	WPolar *pWPolar;
	QString PlaneName, strong;

	PlaneName = m_pCurPlane->planeName();

	strong = tr("Are you sure you want to delete the polars associated to :\n") +  PlaneName +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(pMainFrame, tr("Question"), strong, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;

	for(int j=m_poaWPolar->size()-1; j>=0; j--)
	{
		pWPolar  = (WPolar *)m_poaWPolar->at(j);
		if(pWPolar && pWPolar->planeName()==PlaneName)
		{
			//first remove all WOpps and POpps associated to the Wing Polar

			PlaneOpp * pPOpp;
			for (int i=m_poaPOpp->size()-1; i>=0; i--)
			{
				pPOpp = (PlaneOpp*)m_poaPOpp->at(i);
				if (pPOpp->m_PlrName == pWPolar->polarName()   &&  pPOpp->planeName()== PlaneName)
				{
					m_poaPOpp->removeAt(i);
					delete pPOpp;
				}
			}

			//then remove the polar
			m_poaWPolar->removeAt(j);
			delete pWPolar;
		}
	}
	m_pCurWPolar = NULL;
	SetWPolar();
	pMainFrame->UpdateWPolarListBox();
	emit projectModified();
	SetControls();
	UpdateView();
}


/**
 * The user has requested a deletion of the current WPolar object
 */
void QMiarex::OnDeleteCurWPolar()
{
	if(!m_pCurWPolar) return;
	m_bAnimateWOpp = false;
	int i;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QString strong, PlaneName;

	if(m_pCurPlane)     PlaneName = m_pCurPlane->planeName();
	else return;

	strong = tr("Are you sure you want to delete the polar :\n") +  m_pCurWPolar->polarName() +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(pMainFrame, tr("Question"), strong,
												  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;

	//first remove all WOpps associated to the Wing Polar

	//next remove all the POpps associated to the Wing Polar
	PlaneOpp * pPOpp;
	for (i=m_poaPOpp->size()-1; i>=0; i--)
	{
		pPOpp = (PlaneOpp*)m_poaPOpp->at(i);
		if (pPOpp->m_PlrName  == m_pCurWPolar->polarName()  &&  pPOpp->planeName() == PlaneName)
		{
			m_poaPOpp->removeAt(i);
			delete pPOpp;
		}
	}
	//next remove the WPolar
	WPolar* pWPolar;
	for (i=m_poaWPolar->size()-1;i>=0; i--)
	{
		pWPolar = (WPolar*)m_poaWPolar->at(i);
		if (pWPolar == m_pCurWPolar)
		{
			m_poaWPolar->removeAt(i);
			delete pWPolar;
			break;
		}
	}

	m_pCurPOpp = NULL;
	m_pCurWPolar = NULL;
	emit projectModified();
	SetWPolar();

	pMainFrame->UpdateWPolarListBox();
	SetControls();
	UpdateView();
}


/**
 * The user has toggled the checkbox for the display of the downwash
 */
void QMiarex::OnDownwash()
{
	m_bDownwash = m_pctrlDownwash->isChecked();
	UpdateView();
}


/**
 * The user has requested a duplication of the currently selected wing or plane
 */
void QMiarex::OnDuplicateCurPlane()
{
	if(!m_pCurPlane) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	m_pCurPlane = Objects3D::duplicatePlane(m_pCurPlane);
	pMainFrame->UpdatePlaneListBox();
	SetPlane(m_pCurPlane->planeName());
	emit projectModified();
}



/**
 * The user has requested an edition of the current body
 * Launch the edition interface, and on return, insert the body i.a.w. user instructions
 */
void QMiarex::OnEditCurBody()
{
	if(!m_pCurPlane || !m_pCurPlane->body()) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	Body *pCurBody = m_pCurPlane->body();

	bool bUsed = false;
	int i;
	Plane *pPlane;
	WPolar *pWPolar;
	for (i=0; i< m_poaPlane->size(); i++)
	{
		pPlane = (Plane*)m_poaPlane->at(i);
		if(pPlane->body() && pPlane->body()==pCurBody)
		{
			// Does this plane have results
			for(int j=0; j<m_poaWPolar->size(); j++)
			{
				pWPolar = (WPolar*)m_poaWPolar->at(j);
				if(pWPolar->planeName()==pPlane->planeName() && pWPolar->m_Alpha.size())
				{
					bUsed = true;
					break;
				}
			}
			if(bUsed) break;
		}
	}

	Plane *pModPlane = new Plane();
	pModPlane->Duplicate(m_pCurPlane);

	GL3dBodyDlg glbDlg(pMainFrame);
	glbDlg.m_bEnableName = false;
	glbDlg.InitDialog(pModPlane->body());
	glbDlg.move(GL3dBodyDlg::s_WindowPos);
	glbDlg.resize(GL3dBodyDlg::s_WindowSize);
	if(GL3dBodyDlg::s_bWindowMaximized) glbDlg.setWindowState(Qt::WindowMaximized);

	if(glbDlg.exec()!=QDialog::Accepted) return;

	ModDlg mdDlg(pMainFrame);

	if(bUsed)
	{
		mdDlg.m_Question = tr("The modification will erase all results associated to this Plane.\nContinue ?");
		mdDlg.InitDialog();
		int Ans = mdDlg.exec();

		if (Ans == QDialog::Rejected)
		{
			//restore geometry
			delete pModPlane; // clean up
			return;
		}
		else if(Ans==20)
		{
			//save mods to a new plane object
			m_pCurPlane = Objects3D::setModPlane(pModPlane);

			SetPlane();
			pMainFrame->UpdatePlaneListBox();
			UpdateView();
			return;
		}
	}

	//then modifications are automatically recorded
	m_pCurPlane->Duplicate(pModPlane);
	Objects3D::deletePlaneResults(m_pCurPlane);// will also set new surface and Aerochord in WPolars
	SetPlane();

	s_bResetCurves = true;
	UpdateView();
}



/**
 * The user has requested an edition of the current Plane
 * Launches the dialog box, and maps the data depending on whether the user wants to overwrite, create a new object, or has cancelled the request.
 */
void QMiarex::OnEditCurPlane()
{
	if(!m_pCurPlane)	return;

	int i;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(!m_pCurPlane) return;
	WPolar *pWPolar;
	PlaneOpp* pPOpp;
	bool bHasResults = false;
	for (i=0; i< m_poaWPolar->size(); i++)
	{
		pWPolar = (WPolar*)m_poaWPolar->at(i);
		if(pWPolar->m_Alpha.size() && pWPolar->planeName() == m_pCurPlane->planeName())
		{
			bHasResults = true;
			break;
		}
	}

	for (i=0; i<m_poaPOpp->size(); i++)
	{
		pPOpp = (PlaneOpp*)m_poaPOpp->at(i);
		if(pPOpp->planeName() == m_pCurPlane->planeName())
		{
			bHasResults = true;
			break;
		}
	}

	Plane* pModPlane= new Plane;

	pModPlane->Duplicate(m_pCurPlane);

	PlaneDlg plDlg(pMainFrame);
	plDlg.m_pPlane = pModPlane;
	plDlg.m_bAcceptName = false;
	plDlg.InitDialog();

	ModDlg mdDlg(pMainFrame);

	if(QDialog::Accepted == plDlg.exec())
	{
		if(plDlg.m_bDescriptionChanged)
		{
			emit projectModified();
			m_pCurPlane->rPlaneDescription() = pModPlane->description();
		}

		if(plDlg.m_bChanged)
		{
			if(bHasResults)
			{
				mdDlg.m_Question = tr("The modification will erase all results associated to this Plane.\nContinue ?");
				mdDlg.InitDialog();
				int Ans = mdDlg.exec();

				if (Ans == QDialog::Rejected)
				{
					//restore geometry
					delete pModPlane; // clean up
					return;
				}
				else if(Ans==20)
				{
					//save mods to a new plane object
					m_pCurPlane = Objects3D::setModPlane(pModPlane);

					SetPlane();
					pMainFrame->UpdatePlaneListBox();
					UpdateView();
					return;
				}
			}

			//then modifications are automatically recorded
			m_pCurPlane->Duplicate(pModPlane);

			Objects3D::deletePlaneResults(m_pCurPlane);// will also set new surface and Aerochord in WPolars
			m_bResetglGeom = true;
			m_bResetglMesh = true;
//			m_bResetglOpp  = true;
			s_bResetCurves = true;
		}

		SetPlane();
		pMainFrame->UpdatePlaneListBox();
		m_bIs2DScaleSet = false;
		SetScale();
		OnAdjustToWing();
		SetControls();

		UpdateView();
	}

	delete pModPlane; // clean up

	return;
}




/**
 * Exports the data from the active WOpp to the text file
 */
void QMiarex::OnExportCurWOpp()
{
	if(!m_pCurPOpp)return ;// is there anything to export ?

	int iStrip,j,k,l,p, coef;
	enumTextFileType exporttype;
	QString filter;
	if(Settings::s_ExportFileType==TXT) filter = "Text File (*.txt)";
	else                                  filter = "Comma Separated Values (*.csv)";

	QString FileName, sep, str, strong, Format;

	strong = QString("a=%1_v=%2").arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_Alpha, 5,'f',2).arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_QInf*Units::mstoUnit(),6,'f',2);
	Units::getSpeedUnitLabel(str);
	strong = m_pCurPOpp->m_pPlaneWOpp[0]->m_WingName+"_"+strong+str;

	strong.replace(" ","");
	strong.replace("/", "");
	FileName = QFileDialog::getSaveFileName(this, tr("Export Wing OpPoint"),
											Settings::s_LastDirName +'/'+strong,
											tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
											&filter);

	if(!FileName.length()) return;
	int pos = FileName.lastIndexOf("/");
	if(pos>0) Settings::s_LastDirName = FileName.left(pos);
	pos = FileName.lastIndexOf(".csv");
	if (pos>0) Settings::s_ExportFileType = CSV;
	else       Settings::s_ExportFileType = TXT;
	exporttype = Settings::s_ExportFileType;


	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);

	if(exporttype==TXT) sep = ""; else sep=",";


	out << VERSIONNAME;
	out << "\n\n";

	if(m_pCurPOpp)		out << m_pCurPOpp->planeName()<< "\n";

	strong = m_pCurPOpp->m_pPlaneWOpp[0]->m_PlrName + "\n";
	out << strong;
	strong = QString("QInf  ="+sep+" %1 "+sep).arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_QInf*Units::mstoUnit(),11, 'f', 6);
	Units::getSpeedUnitLabel(str);
	strong+=str+"\n";
	out << strong;

	strong = QString("Alpha = "+sep+"%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_Alpha,11, 'f', 6);
	out << strong;

	strong = QString("Beta  = "+sep+"%1").arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_Beta, 8,'f',3);
	strong += QString::fromUtf8("°\n");
	out << strong;

	strong = QString("Phi   = "+sep+"%1").arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_Phi, 8,'f',3);
	strong += QString::fromUtf8("°\n");
	out << strong;

	strong = QString("Ctrl  = "+sep+"%1\n").arg(m_pCurPOpp->m_Ctrl, 8,'f',3);
	out << strong;

	strong = QString("CL    = "+sep+"%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_CL,11, 'f', 6);
	out << strong;

	strong = QString("Cy    = "+sep+"%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_CY,11, 'f', 6);
	out << strong;

	if(exporttype==TXT) strong = QString(tr("Cd    = %1     ICd   = %2     PCd   = %3\n"))
		.arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_ICD+m_pCurPOpp->m_pPlaneWOpp[0]->m_VCD,11, 'f', 6)
		.arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_ICD,11, 'f', 6)
		.arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_VCD,11, 'f', 6);
	else        strong = QString(tr("Cd=,%1,ICd=, %2,PCd=, %3\n"))
		.arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_ICD+m_pCurPOpp->m_pPlaneWOpp[0]->m_VCD,11, 'f', 6)
		.arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_ICD,11, 'f', 6)
		.arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_VCD,11, 'f', 6);
	out << strong;

	strong = QString(tr("Cl   = ")+sep+"%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_GRm, 11,'g',6);
	out << strong;
	strong = QString(tr("Cm   =")+sep+" %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_GCm, 11,'g',6);
	out << strong;

	if(exporttype==TXT) strong = QString(tr("ICn   = %1     PCn   = %2 \n")).arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_IYm, 11, 'f', 6).arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_GYm, 11, 'f', 6);
	else                strong = QString(tr("ICn=, %1,PCn=, %2\n")).arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_IYm, 11, 'f', 6).arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_GYm, 11, 'f', 6);
	out << strong;

	if(exporttype==TXT) strong = QString("XCP   = %1     YCP   = %2     ZCP   = %3  \n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_CP.x, 11, 'f', 6).arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_CP.y, 11, 'f', 6).arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_CP.z, 11, 'f', 6);
	else                strong = QString("XCP=, %1, YCP=, %2, ZCP=, %3 \n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_CP.x, 11, 'f', 6).arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_CP.y, 11, 'f', 6).arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_CP.z, 11, 'f', 6);
	out << strong;

	if(exporttype==TXT) strong = QString("XNP   = %1\n").arg(m_pCurPOpp->m_XNP, 11, 'f', 6);
	else                strong = QString("XNP=, %1\n").arg(m_pCurPOpp->m_XNP, 11, 'f', 6);
	out << strong;


	strong = QString(tr("Bend. =")+sep+" %1\n\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_MaxBending, 11, 'f', 6);
	out << strong;

	if(m_pCurWPolar->polarType()==STABILITYPOLAR)
	{
		//export non dimensional stability derivatives
		if(exporttype==TXT)
		{
//			complex<double> c, angle;
			double u0 = m_pCurPOpp->m_pPlaneWOpp[0]->m_QInf;
			double mac = m_pCurWPolar->m_WArea;
			double b = m_pCurWPolar->m_WSpan;
			
			strong = "\n\n   ___Longitudinal modes____\n\n";
			out << strong;
		
			strong = QString("      Eigenvalue:  %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
				  .arg(m_pCurPOpp->m_EigenValue[0].real(),9, 'g', 4).arg(m_pCurPOpp->m_EigenValue[0].imag(),9, 'g', 4)
				  .arg(m_pCurPOpp->m_EigenValue[1].real(),9, 'g', 4).arg(m_pCurPOpp->m_EigenValue[1].imag(),9, 'g', 4)
				  .arg(m_pCurPOpp->m_EigenValue[2].real(),9, 'g', 4).arg(m_pCurPOpp->m_EigenValue[2].imag(),9, 'g', 4)
				  .arg(m_pCurPOpp->m_EigenValue[3].real(),9, 'g', 4).arg(m_pCurPOpp->m_EigenValue[3].imag(),9, 'g', 4);
			out << strong;
			strong=("                    _____________________________________________________________________________________________________\n");
			out << strong;
		
			strong = QString("             u/u0: %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(m_pCurPOpp->m_EigenVector[0][0].real()/u0, 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[0][0].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[1][0].real()/u0, 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[1][0].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[2][0].real()/u0, 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[2][0].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[3][0].real()/u0, 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[3][0].imag()/u0, 9, 'g', 4);
			out << strong;
			strong = QString("             w/u0: %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(m_pCurPOpp->m_EigenVector[0][1].real()/u0, 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[0][1].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[1][1].real()/u0, 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[1][1].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[2][1].real()/u0, 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[2][1].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[3][1].real()/u0, 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[3][1].imag()/u0, 9, 'g', 4);
			out << strong;
			strong = QString("     q/(2.u0/MAC): %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(m_pCurPOpp->m_EigenVector[0][2].real()/(2.*u0/mac), 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[0][2].imag()/(2.*u0/mac), 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[1][2].real()/(2.*u0/mac), 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[1][2].imag()/(2.*u0/mac), 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[2][2].real()/(2.*u0/mac), 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[2][2].imag()/(2.*u0/mac), 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[3][2].real()/(2.*u0/mac), 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[3][2].imag()/(2.*u0/mac), 9, 'g', 4);
			out << strong;
			strong = QString("       theta(rad): %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(1.0, 9, 'g', 4).arg(0.0, 9, 'g', 4)
					  .arg(1.0, 9, 'g', 4).arg(0.0, 9, 'g', 4)
					  .arg(1.0, 9, 'g', 4).arg(0.0, 9, 'g', 4)
					  .arg(1.0, 9, 'g', 4).arg(0.0, 9, 'g', 4);
			out << strong;

			strong = "\n";
			out << strong;
	
			
			strong = "\n\n   ___Lateral modes____\n\n";
			out << strong;
		
			strong = QString("      Eigenvalue:  %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
				  .arg(m_pCurPOpp->m_EigenValue[4].real(),9, 'g', 4).arg(m_pCurPOpp->m_EigenValue[4].imag(),9, 'g', 4)
				  .arg(m_pCurPOpp->m_EigenValue[5].real(),9, 'g', 4).arg(m_pCurPOpp->m_EigenValue[5].imag(),9, 'g', 4)
				  .arg(m_pCurPOpp->m_EigenValue[6].real(),9, 'g', 4).arg(m_pCurPOpp->m_EigenValue[6].imag(),9, 'g', 4)
				  .arg(m_pCurPOpp->m_EigenValue[7].real(),9, 'g', 4).arg(m_pCurPOpp->m_EigenValue[7].imag(),9, 'g', 4);
			out << strong;

			strong=("                    _____________________________________________________________________________________________________\n");
			out << strong;
		
			strong = QString("            v/u0 : %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(m_pCurPOpp->m_EigenVector[4][0].real()/u0, 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[4][0].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[5][0].real()/u0, 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[5][0].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[6][0].real()/u0, 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[6][0].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[7][0].real()/u0, 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[7][0].imag()/u0, 9, 'g', 4);
			out << strong;
			strong = QString("    p/(2.u0/Span): %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(m_pCurPOpp->m_EigenVector[4][1].real()/(2.0*u0/b), 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[4][1].imag()/(2.0*u0/b), 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[5][1].real()/(2.0*u0/b), 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[5][1].imag()/(2.0*u0/b), 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[6][1].real()/(2.0*u0/b), 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[6][1].imag()/(2.0*u0/b), 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[7][1].real()/(2.0*u0/b), 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[7][1].imag()/(2.0*u0/b), 9, 'g', 4);
			out << strong;
			strong = QString("    r/(2.u0/Span): %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(m_pCurPOpp->m_EigenVector[4][2].real()/(2.0*u0/b), 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[4][2].imag()/(2.0*u0/b), 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[5][2].real()/(2.0*u0/b), 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[5][2].imag()/(2.0*u0/b), 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[6][2].real()/(2.0*u0/b), 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[6][2].imag()/(2.0*u0/b), 9, 'g', 4)
					  .arg(m_pCurPOpp->m_EigenVector[7][2].real()/(2.0*u0/b), 9, 'g', 4).arg(m_pCurPOpp->m_EigenVector[7][2].imag()/(2.0*u0/b), 9, 'g', 4);
			out << strong;
			strong = QString("         phi(rad): %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(1.0, 9, 'g', 4).arg(0.0, 9, 'g', 4)
					  .arg(1.0, 9, 'g', 4).arg(0.0, 9, 'g', 4)
					  .arg(1.0, 9, 'g', 4).arg(0.0, 9, 'g', 4)
					  .arg(1.0, 9, 'g', 4).arg(0.0, 9, 'g', 4);
			out << strong;

			strong = "\n";
			out << strong;
			

/*			strong = QString("CLa = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CLa, 11, 'f', 6);		out << strong;
			strong = QString("CLq = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CLq, 11, 'f', 6);		out << strong;
			strong = QString("Cma = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->Cma, 11, 'f', 6);		out << strong;
			strong = QString("Cmq = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->Cmq, 11, 'f', 6);		out << strong;
			strong = QString("CYb = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CYb, 11, 'f', 6);		out << strong;
			strong = QString("CYp = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CYp, 11, 'f', 6);		out << strong;
			strong = QString("CYr = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CYr, 11, 'f', 6);		out << strong;
			strong = QString("Clb = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->Clb, 11, 'f', 6);		out << strong;
			strong = QString("Clp = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->Clp, 11, 'f', 6);		out << strong;
			strong = QString("Clr = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->Clr, 11, 'f', 6);		out << strong;
			strong = QString("Cnb = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->Cnb, 11, 'f', 6);		out << strong;
			strong = QString("Cnp = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->Cnp, 11, 'f', 6);		out << strong;
			strong = QString("Cnr = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->Cnr, 11, 'f', 6);		out << strong;
			if(m_pCurPOpp->m_pPlaneWOpp[0]->m_nControls>0)
			{
				strong = QString("CXe = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CXe, 11, 'f', 6);		out << strong;
				strong = QString("CYe = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CYe, 11, 'f', 6);		out << strong;
				strong = QString("CZe = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CZe, 11, 'f', 6);		out << strong;
				strong = QString("CLe = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CLe, 11, 'f', 6);		out << strong;
				strong = QString("CMe = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CMe, 11, 'f', 6);		out << strong;
				strong = QString("CNe = %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CNe, 11, 'f', 6);		out << strong;
			}*/
		}
		else
		{
/*			strong = QString("CLa =,%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CLa, 11, 'f', 6);		out << strong;
			strong = QString("CLq =,%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CLq, 11, 'f', 6);		out << strong;
			strong = QString("Cma =,%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->Cma, 11, 'f', 6);		out << strong;
			strong = QString("Cmq =,%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->Cmq, 11, 'f', 6);		out << strong;
			strong = QString("CYb =,%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CYb, 11, 'f', 6);		out << strong;
			strong = QString("CYp =,%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CYp, 11, 'f', 6);		out << strong;
			strong = QString("CYr =,%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CYr, 11, 'f', 6);		out << strong;
			strong = QString("Clb =,%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->Clb, 11, 'f', 6);		out << strong;
			strong = QString("Clp =,%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->Clp, 11, 'f', 6);		out << strong;
			strong = QString("Clr =,%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->Clr, 11, 'f', 6);		out << strong;
			strong = QString("Cnb =,%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->Cnb, 11, 'f', 6);		out << strong;
			strong = QString("Cnp =,%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->Cnp, 11, 'f', 6);		out << strong;
			strong = QString("Cnr =,%1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->Cnr, 11, 'f', 6);		out << strong;*/
			if(m_pCurPOpp->m_pPlaneWOpp[0]->m_nControls>0)
			{
/*				strong = QString("CXe =, %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CXe, 11, 'f', 6);		out << strong;
				strong = QString("CYe =, %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CYe, 11, 'f', 6);		out << strong;
				strong = QString("CZe =, %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CZe, 11, 'f', 6);		out << strong;
				strong = QString("CLe =, %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CLe, 11, 'f', 6);		out << strong;
				strong = QString("CMe =, %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CMe, 11, 'f', 6);		out << strong;
				strong = QString("CNe =, %1\n").arg(m_pCurPOpp->m_pPlaneWOpp[0]->CNe, 11, 'f', 6);		out << strong;*/
			}
		}
		out << "\n\n";
	}
	
	
	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pWingList[iw])
		{
			out << m_pWingList[iw]->m_WingName;
			for (l=0; l<m_pWOpp[iw]->m_nFlaps; l++)
			{
				strong = QString(tr("Flap ")+sep+"%1"+sep+" moment = "+sep+"%2 ").arg(l+1,4).arg(m_pWOpp[iw]->m_FlapMoment[l]*Units::NmtoUnit(), 9,'f',4);
				Units::getMomentUnitLabel(str);
				strong += str +"\n";
				out << strong;
			}
			out << ("\n");
			m_pWOpp[iw]->Export(out, exporttype);
		}
	}

	if(m_pCurPOpp->m_AnalysisMethod>=VLMMETHOD)
	{
		if(m_pCurPOpp) out << tr("Main Wing Cp Coefficients\n");
		else           out << tr("Wing Cp Coefficients\n");
		coef = 1;

		if(!m_pCurWPolar->m_bThinSurfaces)
		{
			coef = 2;
		}
		if(exporttype==1) out << tr(" Panel     CtrlPt.x        CtrlPt.y        CtrlPt.z             Cp\n");
		else              out << tr("Panel,CtrlPt.x,CtrlPt.y,CtrlPt.z,Cp\n");

		if(exporttype==1) Format = "%1     %2     %3     %4     %5\n";
		else              Format = "%1,%2,%3,%4,%5\n";


		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(m_pWingList[iw])
			{
				out << m_pWingList[iw]->m_WingName+ tr("Cp Coefficients")+"\n";
				p=0;
				iStrip = 0;
				for (j=0; j<m_pWingList[iw]->m_Surface.size(); j++)
				{
					if(m_pWingList[iw]->m_Surface.at(j)->m_bIsTipLeft && !m_pCurPOpp->m_bThinSurface) p+= m_pWingList[iw]->m_Surface.at(j)->m_NXPanels;

					for(k=0; k<m_pWingList[iw]->m_Surface.at(j)->m_NYPanels; k++)
					{
						iStrip++;
						strong = QString(tr("Strip %1\n")).arg(iStrip);
						out << strong;

						for(l=0; l<m_pWingList[iw]->m_Surface.at(j)->m_NXPanels * coef; l++)
						{
							if(m_pWingList[iw]->m_pWingPanel[p].m_Pos==MIDSURFACE)
							{
								strong = QString(Format).arg(p,4)
														 .arg(m_pWingList[iw]->m_pWingPanel[p].CtrlPt.x,11,'e',3)
														 .arg(m_pWingList[iw]->m_pWingPanel[p].CtrlPt.y,11,'e',3)
														 .arg(m_pWingList[iw]->m_pWingPanel[p].CtrlPt.z,11,'e',3)
														 .arg(m_pWOpp[iw]->m_dCp[p],11,'f',4);
							}
							else
							{
								strong = QString(Format).arg(p,4)
														.arg(m_pWingList[iw]->m_pWingPanel[p].CollPt.x,11,'e',3)
														.arg(m_pWingList[iw]->m_pWingPanel[p].CollPt.y,11,'e',3)
														.arg(m_pWingList[iw]->m_pWingPanel[p].CollPt.z,11,'e',3)
														.arg(m_pWOpp[iw]->m_dCp[p],11,'f',4);
							}
							out << strong;
							p++;
						}
					}
				}
			}
			out << ("\n\n");
		}
	}
	out << ("\n\n");

	XFile.close();

}


/**
 * Exports the data from the active polar to a text file
 */
void QMiarex::OnExportCurWPolar()
{
	if (!m_pCurWPolar) return;

	QString FileName, filter;

	if(Settings::s_ExportFileType==TXT) filter = "Text File (*.txt)";
	else                                filter = "Comma Separated Values (*.csv)";

	FileName = m_pCurWPolar->polarName();
	FileName.replace("/", " ");
	FileName = QFileDialog::getSaveFileName(this, tr("Export Polar"),
											Settings::s_LastDirName + "/"+FileName,
											tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
											&filter);

	if(!FileName.length()) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) Settings::s_LastDirName = FileName.left(pos);
	pos = FileName.lastIndexOf(".csv");

	if(filter.indexOf("*.txt")>0)
	{
		Settings::s_ExportFileType = TXT;
		if(FileName.indexOf(".txt")<0) FileName +=".txt";
	}
	else if(filter.indexOf("*.csv")>0)
	{
		Settings::s_ExportFileType = CSV;
		if(FileName.indexOf(".csv")<0) FileName +=".csv";
	}


	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return;

	QTextStream out(&XFile);
	m_pCurWPolar->Export(out, Settings::s_ExportFileType);
	XFile.close();

	UpdateView();
}


/**
 * Exports the geometrical data of the acitve wing or plane to a text file readable by AVL
 *@todo AVL expects consistency of the units, need to check all lines and cases
 */
void QMiarex::OnExporttoAVL()
{
	if (!m_pCurPlane) return;
	QString filter =".avl";

	QString FileName, strong;


	FileName = m_pCurPlane->planeName();
	FileName.replace("/", " ");
	FileName = QFileDialog::getSaveFileName(this, tr("Export Plane"),
									Settings::s_LastDirName + "/"+FileName,
									tr("AVL Text File (*.avl)"), &filter);
	if(!FileName.length()) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) Settings::s_LastDirName = FileName.left(pos);

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);

	strong = MainFrame::s_ProjectName;
	int len = strong.length();
	if (strong.right(1) == "*") strong = strong.left(len-1);
	if(!strong.length()) out << tr("Project");
	else out << strong;
	out << "\n";
	out << "0.0                          | Mach\n";
	if(m_pCurPlane->m_Wing[0].m_bSymetric) out << ("0     0     0.0              | iYsym  iZsym  Zsym\n");
	else                                   out << ("0     0     0.0              | iYsym  iZsym  Zsym\n");
	out << ("\n");

	strong = QString("%1   %2   %3  | Sref   Cref   Bref\n")
				  .arg(m_pCurPlane->planformArea()*Units::mtoUnit()*Units::mtoUnit(), 8, 'f', 3)
				  .arg(m_pCurPlane->m_Wing[0].m_MAChord*Units::mtoUnit(),       8, 'f', 3)
				  .arg(m_pCurPlane->planformSpan()*Units::mtoUnit(),  8, 'f', 3);
	out << strong;
    out << "# Note : check consistency of area unit above with length units of the file\n";

	if(m_pCurPlane)
		strong = QString("%1  %2  %3          | Xref   Yref   Zref\n")
						   .arg(m_pCurPlane->CoG().x*Units::mtoUnit(),8,'f',3)
						   .arg(m_pCurPlane->CoG().y*Units::mtoUnit(),8,'f',3)
						   .arg(m_pCurPlane->CoG().z*Units::mtoUnit(),8,'f',3);

	out << strong;

	out << (" 0.00                        | CDp  (optional)\n");

	out << ("\n\n\n");

	int index = rand();

	if(m_pCurPlane) m_pCurPlane->m_Wing[0].ExportAVLWing(out, index, 0.0, m_pCurPlane->WingTiltAngle(0));
	else            m_pCurPlane->m_Wing[0].ExportAVLWing(out, index, 0.0, 0.0);

	for(int iw=1; iw<MAXWINGS; iw++)
	{
		if(m_pWingList[iw]) m_pWingList[iw]->ExportAVLWing(out, index+iw, 0.0, m_pCurPlane->WingTiltAngle(iw));
	}
	XFile.close();
}


/**
 * The user has toggled the display switch for the fin curve in the OpPoint view
 */
void QMiarex::OnFinCurve()
{
	m_bShowWingCurve[3] = !m_bShowWingCurve[3];
//	CheckMenus();

	if (m_iView==WOPPVIEW)    CreateWOppCurves();
	else if(m_iView==WCPVIEW) CreateCpCurves();

	UpdateView();
}



/**
 * The user has toggled the display switch for the elevator curve in the OpPoint view
 */
void QMiarex::OnStabCurve()
{
	m_bShowWingCurve[2] = !m_bShowWingCurve[2];
//	CheckMenus();

	if (m_iView==WOPPVIEW)    CreateWOppCurves();
	else if(m_iView==WCPVIEW) CreateCpCurves();

	UpdateView();
}


/**
 * The user has toggled the display of the foil names in the 3D view
 */
void QMiarex::OnFoilNames()
{
	s_bFoilNames = m_pctrlFoilNames->isChecked();
	UpdateView();
}


/**
 * The user has toggled the display of the masses in the 3D view
 */
void QMiarex::OnMasses()
{
	s_bShowMasses = m_pctrlMasses->isChecked();
	UpdateView();
}


/**
 * The user has changed one of the scale in the GL3DScale widget
 */
void QMiarex::OnGL3DScale()
{
	if(m_iView != W3DVIEW)
	{
//		m_pctrl3DSettings->setChecked(false);
		return;
	}
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(pMainFrame->m_pctrl3DScalesWidget->isVisible()) pMainFrame->m_pctrl3DScalesWidget->hide();
	else                                               pMainFrame->m_pctrl3DScalesWidget->show();

	pMainFrame->W3DScalesAct->setChecked(pMainFrame->m_pctrl3DScalesWidget->isVisible());
//	if(m_pctrl3DSettings->isChecked()) pMainFrame->m_pctrl3DScalesWidget->show();
//	else                               pMainFrame->m_pctrl3DScalesWidget->hide();
}


/**
 * The user has requested an edition of the settings of the active graph
 */
void QMiarex::OnGraphSettings()
{
	QGraph *pGraph = NULL;
	GraphDlg grDlg((MainFrame*)s_pMainFrame);

	pGraph = m_pCurGraph;
	if(!pGraph) return;
	if(m_iView==WOPPVIEW)
	{
		grDlg.m_iGraphType = 61;
	}
	else if(m_iView==WPOLARVIEW)
	{
		grDlg.m_iGraphType = 62;
	}
	else if(m_iView==WCPVIEW) grDlg.m_iGraphType = 64;

	if(!pGraph) return;

	QGraph graph;
	graph.CopySettings(pGraph);
	grDlg.m_pMemGraph = &graph;
	grDlg.m_pGraph = pGraph;
	grDlg.SetParams();

	if(grDlg.exec() == QDialog::Accepted)
	{
		if(m_iView==WOPPVIEW)
		{

			if(grDlg.m_bVariableChanged)
			{
				m_pCurGraph->SetAutoY(true);
				m_pCurGraph->SetAutoYMinUnit(true);
			}
		}
		else if(m_iView==WPOLARVIEW)
		{
			if(pGraph ==      m_WPlrGraph)   SetWGraphTitles(m_WPlrGraph);
			else if(pGraph == m_WPlrGraph+1) SetWGraphTitles(m_WPlrGraph+1);
			else if(pGraph == m_WPlrGraph+2) SetWGraphTitles(m_WPlrGraph+2);
			else if(pGraph == m_WPlrGraph+3) SetWGraphTitles(m_WPlrGraph+3);

			if(grDlg.m_bVariableChanged)
			{
				m_pCurGraph->SetAuto(true);
				m_pCurGraph->SetAutoYMinUnit(true);
			}
		}
		else if(m_iView==STABTIMEVIEW || m_iView==STABPOLARVIEW)
		{
			s_bResetCurves = true;
			SetStabGraphTitles();
		}
	}
	else
	{
		pGraph->CopySettings(&graph);
	}
	s_bResetCurves = true;
	UpdateView();
}



/**
 * The user has toggled the display of the half-wing in the OpPoint view
 */
void QMiarex::OnHalfWing()
{
	m_bHalfWing = !m_bHalfWing;
	if(m_iView==WOPPVIEW)
	{
		m_bIs2DScaleSet = false;
		SetWGraphScale();
		Set2DScale();
		OnAdjustToWing();
		UpdateView();
	}
	SetControls();
}


/**
 * The user has requested that all polars curves be hidden
 */
void QMiarex::OnHideAllWPolars()
{
	int i;
	WPolar *pWPolar;
	for (i=0; i<m_poaWPolar->size(); i++)
	{
		pWPolar = (WPolar*)m_poaWPolar->at(i);
		pWPolar->m_bIsVisible = false;
		if(pWPolar->polarType()==STABILITYPOLAR) pWPolar->m_bShowPoints = false;
	}

	emit projectModified();
	SetCurveParams();
	s_bResetCurves = true;
	UpdateView();
}



/**
 * The user has requested that all curves of the oppoints associated to the active polar be hidden
 */
void QMiarex::OnHideAllWPlrOpps()
{
	int i;
	m_bCurPOppOnly = false;

	PlaneOpp *pPOpp;
	if(m_pCurPlane)
	{
		for (i=0; i< m_poaPOpp->size(); i++)
		{
			pPOpp = (PlaneOpp*)m_poaPOpp->at(i);
			if (pPOpp->planeName() == m_pCurWPolar->planeName() &&
				pPOpp->m_PlrName   == m_pCurWPolar->polarName())
			{
				pPOpp->m_bIsVisible = false;
			}
		}
	}
	emit projectModified();
	SetCurveParams();

	s_bResetCurves = true;
	UpdateView();
}


/**
 * The user has requested that all oppoint curves be hidden
 */
void QMiarex::OnHideAllWOpps()
{
	int i;
	m_bCurPOppOnly = false;

	PlaneOpp *pPOpp;
	for (i=0; i< m_poaPOpp->size(); i++)
	{
		pPOpp = (PlaneOpp*)m_poaPOpp->at(i);
		pPOpp->m_bIsVisible = false;
	}
	emit projectModified();
	SetCurveParams();

	s_bResetCurves = true;
	UpdateView();
}


/**
 * The user has requested that all curves of the oppoints associated to the active wing or plane be hidden
 */
void QMiarex::OnHidePlaneOpps()
{
	PlaneOpp *pPOpp;
	int i;
	for (i=0; i< m_poaPOpp->size(); i++)
	{
		pPOpp = (PlaneOpp*)m_poaPOpp->at(i);
		if (pPOpp->planeName() == m_pCurWPolar->planeName())
		{
			pPOpp->m_bIsVisible = false;
		}
	}

	emit projectModified();
	SetCurveParams();
	s_bResetCurves = true;
	UpdateView();
}


/**
 * The user has requested that all curves of the oppoints associated to the active polar be hidden
 */
void QMiarex::OnHidePlaneWPolars()
{
	if(!m_pCurPlane) return;
	int i;
	QString PlaneName;
	if(m_pCurPlane)     PlaneName = m_pCurPlane->planeName();
	else return;

	WPolar *pWPolar;
	for (i=0; i<m_poaWPolar->size(); i++)
	{
		pWPolar = (WPolar*)m_poaWPolar->at(i);
		if (pWPolar->planeName() == PlaneName)
		{
			pWPolar->m_bIsVisible = false;
			if(pWPolar->polarType()==STABILITYPOLAR) pWPolar->m_bShowPoints = false;
		}
	}

	SetCurveParams();
	emit projectModified();
	s_bResetCurves = true;
	UpdateView();
}


/**
 * The user has requested that the current point be highlighted in the selected graph
 *@todo not quite robust - check and improve
 */
void QMiarex::OnHighlightWOpp()
{
	if(m_iView!=WPOLARVIEW && m_iView!=STABPOLARVIEW) return;
	m_bHighlightOpp = !m_bHighlightOpp;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->highlightWOppAct->setChecked(m_bHighlightOpp);

	for(int ig=0; ig<MAXGRAPHS; ig++)
		m_WPlrGraph[ig].m_bHighlightPoint = m_bHighlightOpp;

	s_bResetCurves = true;
	UpdateView();
}


/**
 * The user has requested the import of a polar definition from a text file.
 * Createss a new CWPolar object, fills it with the data from the text file, and adds it to the array
 *@todo not used often, nor thouroughly tested
 */
void QMiarex::OnImportWPolar()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	WPolar *pWPolar = new WPolar;
	bool bOK;
	QString PolarName, PlaneName;
	QString strong, str;

	QString PathName;

	int Line;

	PathName = QFileDialog::getOpenFileName(pMainFrame, tr("Open File"),
											Settings::s_LastDirName,
											tr("Plane Polar Format (*.*)"));
	if(!PathName.length())		return ;
	int pos = PathName.lastIndexOf("/");
	if(pos>0) Settings::s_LastDirName = PathName.left(pos);

	QFile XFile(PathName);
	if (!XFile.open(QIODevice::ReadOnly))
	{
		QString strange = tr("Could not read the file\n")+PathName;
		QMessageBox::warning(pMainFrame, tr("Warning"), strange);
		return;
	}

	QTextStream in(&XFile);
	Line = 0;

	bool bRead;
	bRead  = ReadAVLString(in, Line, strong);// XFoil or XFLR5 version
	bRead  = ReadAVLString(in, Line, strong);// Plane Name

	PlaneName = strong.right(strong.length()-19);
	PlaneName = PlaneName.trimmed();

	Plane *pPlane = Objects3D::getPlane(PlaneName);

	if(!pPlane)
	{
		str = tr("No Plane with the name ")+PlaneName;
		str+= tr("\ncould be found. The polar(s) will not be stored");
		delete pWPolar;
		QMessageBox::warning(pMainFrame, tr("Warning"), str);
		return;
	}

	pWPolar->planeName() = PlaneName;

	bRead  = ReadAVLString(in, Line, strong);// Polar Name
	PolarName = strong.right(strong.length()-19);
	pWPolar->polarName() = PolarName;

	bRead  = ReadAVLString(in, Line, strong);// Freestream speed
	pWPolar->m_QInfSpec = strong.toDouble(&bOK)/Units::mstoUnit();

	bRead  = ReadAVLString(in, Line, strong);// "   alpha      CL          ICd   ..."
	bRead  = ReadAVLString(in, Line, strong);// " _________  ________   ________  ..."

	while( bRead)
	{
		bRead  = ReadAVLString(in, Line, strong);// polar data
		if(bRead)
		{
			if(strong.length())
			{
//				textline = strong.toLatin1();
//				text = textline.constData();
//				res = sscanf(text, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf", &alpha, &CL, &ICd, &PCd, &TCd, &CY, &GCm, &GRm, &GYm, &IYm, &QInf, &XCP);

				//Do this the C++ way
				QStringList values = strong.split(" ", QString::SkipEmptyParts);

				if(values.length()>=12)
				{
					PlaneOpp *pPOpp = new PlaneOpp;

					pPOpp->m_Alpha  = values.at(0).toDouble();
					pPOpp->m_CL     = values.at(1).toDouble();
					pPOpp->m_ICD    = values.at(2).toDouble();
					pPOpp->m_VCD    = values.at(3).toDouble();
//					TCd    = values.at(4).toDouble();
					pPOpp->m_CY     = values.at(5).toDouble();
					pPOpp->m_GCm    = values.at(6).toDouble();
					pPOpp->m_GRm    = values.at(7).toDouble();
					pPOpp->m_GYm    = values.at(8).toDouble();
					pPOpp->m_IYm    = values.at(9).toDouble();
					pPOpp->m_QInf   = values.at(10).toDouble();
					pPOpp->m_CP.x   = values.at(11).toDouble();

					pWPolar->AddPlaneOpPoint(pPOpp);
//					pWPolar->AddWingOpPoint(alpha, CL, ICd, PCd, CY, GCm, 0.0, 0.0, GRm, GYm, IYm, QInf, XCP);
				}
				else
				{
					bRead = false;
				}
			}
		}
	}

	pWPolar->m_Color = randomColor();

	Objects3D::addWPolar(pWPolar);
	m_pCurWPolar = pWPolar;

	SetWPolar(pWPolar);
	pMainFrame->UpdateWPolarListBox();
	UpdateView();
	emit projectModified();
}


/**
 * Toggles the flag which requests the initialization of the start parameters at the launch of an LLT analysis
 */
void QMiarex::OnInitLLTCalc()
{
	m_bInitLLTCalc = m_pctrlInitLLTCalc->isChecked();
}


/**
 * The user has requested to store the active curve in the Cp graph display
 * Duplicates the curve and adds it to the graph
 */
void QMiarex::OnKeepCpSection()
{
	Curve *pCurve, *pNewCurve;

	pCurve = m_CpGraph.GetCurve(0);
	pNewCurve = m_CpGraph.AddCurve();
	pNewCurve->CopyData(pCurve);
	pNewCurve->SetTitle(pCurve->title());

    m_CpColor = MainFrame::s_ColorList[(m_CpGraph.GetCurveCount())%24];
	pCurve->SetColor(m_CpColor);

	m_CpStyle = 0;
	m_CpWidth = 1;
	m_bShowCpPoints = false;
	SetCurveParams();

	CreateCpCurves();
	UpdateView();
}




/**
 * The user has toggled the switch of the light - much like in real life ?
 */
void QMiarex::OnLight()
{
//	GLLightDlg::s_bLight = m_pctrlLight->isChecked();
//	m_bResetglGeom = true;
	UpdateView();
}



/**
 * The user has requested the launch of the dialog box used to manage the array of planes
 */
void QMiarex::OnManagePlanes()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QString PlaneName = "";
	if(m_pCurPlane)     PlaneName = m_pCurPlane->planeName();

	ManagePlanesDlg uDlg(pMainFrame);
	uDlg.InitDialog(PlaneName);
	uDlg.exec();

	if(uDlg.m_pPlane)     SetPlane(uDlg.m_pPlane->planeName());
	else                  SetPlane();

	if(uDlg.m_bChanged) emit projectModified();

	pMainFrame->UpdatePlaneListBox();
	SetControls();

	m_bResetglGeom = true;
	m_bResetglMesh = true;
	UpdateView();
}


/**
 * The user has toggled the display of moments in the 3D view
 **/
void QMiarex::OnMoment()
{
	m_bMoments = m_pctrlMoment->isChecked();
	UpdateView();
}




/**
 * The user has requested the creation of a new plane.
 * Launches the dialog box, and stores the plane in the array i.a.w. user instructions
 */
void QMiarex::OnNewPlane()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	Plane* pPlane = new Plane;

	PlaneDlg plDlg(pMainFrame);
	plDlg.m_pPlane = pPlane;
	plDlg.m_bAcceptName = true;
	plDlg.InitDialog();

	if(QDialog::Accepted == plDlg.exec())
	{
		emit projectModified();

		if(Objects3D::getPlane(pPlane->planeName()))
			m_pCurPlane = Objects3D::setModPlane(pPlane);
		else
		{
			Objects3D::addPlane(pPlane);
			m_pCurPlane = pPlane;
		}
		SetPlane();
		pMainFrame->UpdatePlaneListBox();
		m_bResetglLegend = true;
	}
	else
	{
		delete pPlane;
	}

	SetControls();
	UpdateView();
}


/**
 *The user has toggled the switch for the display of the outline of the surfaces in 3D view
 */
void QMiarex::OnOutline()
{
	s_bOutline = m_pctrlOutline->isChecked();
	UpdateView();
}


/**
 *The user has toggled the switch for the display of the panels in 3D view
 */
void QMiarex::OnPanels()
{
	s_bVLMPanels = m_pctrlPanels->isChecked();
	UpdateView();
}

/**
 * Reads the analysis input from the dialog boxes
 */
void QMiarex::OnReadAnalysisData()
{
	m_bSequence = m_pctrlSequence->isChecked();
	m_bInitLLTCalc = m_pctrlInitLLTCalc->isChecked();

	if(m_pCurWPolar && m_pCurWPolar->polarType()==FIXEDAOAPOLAR)
	{
		m_QInfMin   = m_pctrlAlphaMin->Value()         /Units::mstoUnit();
		m_QInfMax   = m_pctrlAlphaMax->Value()         /Units::mstoUnit();
		m_QInfDelta = qAbs(m_pctrlAlphaDelta->Value()) /Units::mstoUnit();
		if(qAbs(m_QInfDelta)<0.1)
		{
			m_QInfDelta = 1.0;
			m_pctrlAlphaDelta->SetValue(1.0);
		}
	}
	else if(m_pCurWPolar && m_pCurWPolar->polarType()==BETAPOLAR)
	{
		m_BetaMin   = m_pctrlAlphaMin->Value();
		m_BetaMax   = m_pctrlAlphaMax->Value();
		m_BetaDelta = qAbs(m_pctrlAlphaDelta->Value());
		if(qAbs(m_BetaDelta)<0.01)
		{
			m_BetaDelta = 0.01;
			m_pctrlAlphaDelta->SetValue(0.01);
		}
	}
	else if(m_pCurWPolar && m_pCurWPolar->polarType()==STABILITYPOLAR)
	{
		m_ControlMin   = m_pctrlAlphaMin->Value();
		m_ControlMax   = m_pctrlAlphaMax->Value();
		m_ControlDelta = qAbs(m_pctrlAlphaDelta->Value());
		if(qAbs(m_ControlDelta)<0.001)
		{
			m_ControlDelta = 0.001;
			m_pctrlAlphaDelta->SetValue(0.001);
		}
	}
	else if(m_pCurWPolar)
	{
		m_AlphaMin   = m_pctrlAlphaMin->Value();
		m_AlphaMax   = m_pctrlAlphaMax->Value();
		m_AlphaDelta = qAbs(m_pctrlAlphaDelta->Value());

		if(qAbs(m_AlphaDelta)<0.01)
		{
			m_AlphaDelta = 0.01;
			m_pctrlAlphaDelta->SetValue(0.01);
		}
	}
}



/**
 * The user has requested a change to the type of polars which ought to be displayed
 */
void QMiarex::OnPolarFilter()
{
	PolarFilterDlg pfDlg((MainFrame*)s_pMainFrame);
	pfDlg.m_bMiarex = true;
	pfDlg.m_bType1 = m_bType1;
	pfDlg.m_bType2 = m_bType2;
	pfDlg.m_bType4 = m_bType4;
	pfDlg.m_bType7 = m_bType7;

	pfDlg.InitDialog();

	if(pfDlg.exec()==QDialog::Accepted)
	{
		m_bType1 = pfDlg.m_bType1;
		m_bType2 = pfDlg.m_bType2;
		m_bType4 = pfDlg.m_bType4;
		m_bType7 = pfDlg.m_bType7;
		s_bResetCurves = true;
		UpdateView();
	}
}


/**
 * The user has requested that the active polar be renames
 * Changes the polar name and updates the references in all child oppoints
 */
void QMiarex::OnRenameCurWPolar()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(!m_pCurWPolar) return;
	if(!m_pCurPlane) return;

	WPolar *pWPolar = NULL;
	WPolar *pOldWPolar = NULL;

	//make a list of existing WPolar names for that Plane
	QStringList NameList;
	for(int k=0; k<m_poaWPolar->size(); k++)
	{
		pWPolar = (WPolar*)m_poaWPolar->at(k);
		if(pWPolar->planeName()==m_pCurPlane->planeName())
			NameList.append(pWPolar->polarName());
	}

	RenameDlg dlg;
	dlg.InitDialog(&NameList, m_pCurWPolar->polarName(), QObject::tr("Enter the new name for the Polar:"));
	int resp = dlg.exec();
	if(resp==QDialog::Rejected)
	{
		return;
	}
	else if(resp==10)
	{
		//the user wants to overwrite an existing name
		if(dlg.newName()==m_pCurWPolar->polarName()) return; //what's the point ?

		// it's a real overwrite
		// so find the existing WPolar with the new name
		pWPolar = NULL;
		for(int ipb=0; ipb<m_poaWPolar->size(); ipb++)
		{
			pOldWPolar = (WPolar*)m_poaWPolar->at(ipb);
			if(pOldWPolar->polarName()==dlg.newName() && pOldWPolar->planeName()==m_pCurPlane->planeName())
			{
				pWPolar = pOldWPolar;
				m_poaWPolar->removeAt(ipb);
				break;
			}
		}

		//remove and delete its children POpps from the array
		if(pWPolar)
		{
			for (int l=m_poaPOpp->size()-1;l>=0; l--)
			{
				PlaneOpp *pPOpp = (PlaneOpp*)m_poaPOpp->at(l);
				if (pPOpp->planeName()==pWPolar->planeName() && pPOpp->polarName()==pWPolar->polarName())
				{
					m_poaPOpp->removeAt(l);
					delete pPOpp;
				}
			}
			//delete the old WPolar;
			delete pWPolar;
		}
	}

	//ready to insert
	//remove the WPolar from its current position in the array
	for (int l=0; l<m_poaWPolar->size();l++)
	{
		pOldWPolar = (WPolar*)m_poaWPolar->at(l);
		if(pOldWPolar==m_pCurWPolar)
		{
			m_poaWPolar->removeAt(l);
			break;
		}
	}
	//set the new name
	m_pCurWPolar->setPolarName(dlg.newName());

	//insert
	bool bInserted = false;
	for (int l=0; l<m_poaWPolar->size();l++)
	{
		pOldWPolar = (WPolar*)m_poaWPolar->at(l);

		if(pOldWPolar->polarName().compare(m_pCurWPolar->polarName(), Qt::CaseInsensitive) >0)
		{
			//then insert before
			m_poaWPolar->insert(l, m_pCurWPolar);
			bInserted = true;
			break;
		}
	}

	if(!bInserted) m_poaWPolar->append(m_pCurWPolar);


	pMainFrame->UpdateWPolarListBox();

	emit projectModified();

	s_bResetCurves = true;
	UpdateView();
}



/**
 * The user has requested that the active wing ot plane be renames
 * Changes the name and updates the references in all child polars and oppoints
 */
void QMiarex::OnRenameCurPlane()
{
	//Rename the currently selected Plane
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(!m_pCurPlane)	return;
	Objects3D::renamePlane(m_pCurPlane->planeName());
	pMainFrame->UpdatePlaneListBox();
	m_bResetglLegend = true;
	UpdateView();

    emit projectModified();
}


/**
 * The user has requested a deletion of all the previously stored curves in the Cp Graph
 */
void QMiarex::OnResetCpSection()
{
	for(int i=m_CpGraph.GetCurveCount()-1; i>3 ;i--)	m_CpGraph.DeleteCurve(i);
	CreateCpCurves();
	UpdateView();
}


/**
 * The user has requested that the results data of the current CWPolar object be deleted.
 * Deletes it and all its child operating points, and updates the graphs
 */
void QMiarex::OnResetCurWPolar()
{
	if (!m_pCurWPolar) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong = tr("Are you sure you want to reset the content of the polar :\n")+  m_pCurWPolar->polarName() +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(pMainFrame, tr("Question"), strong,
												  QMessageBox::Yes|QMessageBox::No,
												  QMessageBox::Yes)) return;

	m_pCurWPolar->ClearData();
	PlaneOpp *pPOpp;
	if(m_pCurPlane)
	{
		for(int i=m_poaPOpp->size()-1; i>=0; --i)
		{
			pPOpp = (PlaneOpp*) m_poaPOpp->at(i);
			if(pPOpp->m_PlrName==m_pCurWPolar->polarName() && pPOpp->planeName()==m_pCurPlane->planeName())
			{
				m_poaPOpp->removeAt(i);
				delete pPOpp;
			}
		}
	}
	pMainFrame->UpdatePOppListBox();
	m_pCurPOpp = NULL;

	if(m_iView==WPOLARVIEW)
	{
		if(m_pCurWPolar)
		{
			QString PolarProps;
			m_pCurWPolar->GetPolarProperties(PolarProps);
			m_pctrlPolarProps->setText(PolarProps);
		}
	}

	emit projectModified();
	s_bResetCurves = true;
	UpdateView();
}


/**
 * The user has requested that the wing scale be reset to its default value in the operating point graph view
 */
void QMiarex::OnResetWingGraphScale()
{
	//resets the scale of the current graph
	if(!m_pCurGraph) return;
	m_pCurGraph->SetAuto(true);
	m_pCurGraph->ResetXLimits();
	m_pCurGraph->ResetYLimits();
	if (m_pCurGraph == &m_WingGraph[0] ||
		m_pCurGraph == &m_WingGraph[1] ||
		m_pCurGraph == &m_WingGraph[2] ||
		m_pCurGraph == &m_WingGraph[3])
	{
		m_pCurGraph->SetAutoX(false);
		double halfspan = m_pCurPlane->planformSpan()/2.0;
		if(m_bHalfWing) m_pCurGraph->SetXMin(0.0);
		else            m_pCurGraph->SetXMin(-halfspan*Units::mtoUnit());
		m_pCurGraph->SetXMax( halfspan*Units::mtoUnit());
	}
	UpdateView();
}


/**
 * The user has requested that the wing scale be reset to its default value in the 3D view
 */
void QMiarex::OnResetWingScale()
{
	m_bIs2DScaleSet = false;
	Set2DScale();
	UpdateView();
}


/**
 * The user has requested that the position of the legend be reset to its default in the operating point graph view
 */
void QMiarex::OnResetWOppLegend()
{
	SetWingLegendPos();
	UpdateView();
}


/**
 * The user has requested that the position of the legend be reset to its default in the polar graph view
 */
void QMiarex::OnResetWPlrLegend()
{
	SetWPlrLegendPos();
	UpdateView();
}

/**
 * The user has requested that the size of the active wing be scaled.
 * Launches the dialog box, creates a new wing, and overwrites the existing wing or plane,
 * or creates a new one i.a.w. user instructions.
 */
void QMiarex::OnScaleWing()
{
	if(!m_pCurPlane) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	WingScaleDlg wsDlg(pMainFrame);
	wsDlg.InitDialog(m_pCurPlane->planformSpan(), m_pCurPlane->m_Wing[0].Chord(0), m_pCurPlane->m_Wing[0].AverageSweep(), m_pCurPlane->m_Wing[0].TipTwist());

	if(QDialog::Accepted == wsDlg.exec())
	{
		if (wsDlg.m_bSpan || wsDlg.m_bChord || wsDlg.m_bSweep || wsDlg.m_bTwist)
		{
			Plane *pNewPlane = new Plane;
			pNewPlane->Duplicate(m_pCurPlane);
			if(wsDlg.m_bSpan)  pNewPlane->wing()->ScaleSpan(wsDlg.m_NewSpan);
			if(wsDlg.m_bChord) pNewPlane->wing()->ScaleChord(wsDlg.m_NewChord);
			if(wsDlg.m_bSweep) pNewPlane->wing()->ScaleSweep(wsDlg.m_NewSweep);
			if(wsDlg.m_bTwist) pNewPlane->wing()->ScaleTwist(wsDlg.m_NewTwist);
			pNewPlane->ComputePlane();

			m_pCurPlane = Objects3D::setModPlane(pNewPlane);
			SetPlane();
			pMainFrame->UpdatePlaneListBox();
		}

		s_bResetCurves = true;
		UpdateView();
	}
}


/**
 * The user has toggled the switch for a sequential analyis
 */
void QMiarex::OnSequence()
{
	m_bSequence = m_pctrlSequence->isChecked();
	m_pctrlAlphaMax->setEnabled(m_bSequence);
	m_pctrlAlphaDelta->setEnabled(m_bSequence);
}



/**
 * The user has requested the display of all the operating point curves
 */
void QMiarex::OnShowAllWOpps()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	int i;
	//Switch all WOpps view to on for all Plane and WPolar
	m_bCurPOppOnly = false;
	pMainFrame->showCurWOppOnly->setChecked(false);

	PlaneOpp *pPOpp;
	for (i=0; i< m_poaPOpp->size(); i++)
	{
		pPOpp = (PlaneOpp*)m_poaPOpp->at(i);
		pPOpp->m_bIsVisible = true;
	}

	emit projectModified();
	SetCurveParams();

	s_bResetCurves = true;
	UpdateView();
}



/**
 * The user has requested the display of all the polar curves
 */
void QMiarex::OnShowAllWPolars()
{
	int i;
	WPolar *pWPolar;
	for (i=0; i<m_poaWPolar->size(); i++)
	{
		pWPolar = (WPolar*)m_poaWPolar->at(i);
		pWPolar->m_bIsVisible = true;
	}

	emit projectModified();
	SetCurveParams();
	s_bResetCurves = true;
	UpdateView();
}


/**
 * The user has requested the display exclusively of all the polar curves associated to the active wing or plane.
 * The display of all other polar curves is turned off
 */
void QMiarex::OnShowPlaneWPolarsOnly()
{
	if(!m_pCurPlane) return;
	int i;
	QString PlaneName;
	if(m_pCurPlane)     PlaneName = m_pCurPlane->planeName();
	else return;

	WPolar *pWPolar;
	for (i=0; i<m_poaWPolar->size(); i++)
	{
		pWPolar = (WPolar*)m_poaWPolar->at(i);
		pWPolar->m_bIsVisible = (pWPolar->planeName() == PlaneName);
	}

	SetCurveParams();
	emit projectModified();
	s_bResetCurves = true;
	UpdateView();
}



/**
 * The user has requested the display of all the polar curves associated to the active wing or plane
 */
void QMiarex::OnShowPlaneWPolars()
{
	if(!m_pCurPlane) return;
	int i;
	QString PlaneName;
	if(m_pCurPlane)     PlaneName = m_pCurPlane->planeName();
	else return;

	WPolar *pWPolar;
	for (i=0; i<m_poaWPolar->size(); i++)
	{
		pWPolar = (WPolar*)m_poaWPolar->at(i);
		if (pWPolar->planeName() == PlaneName) pWPolar->m_bIsVisible = true;
	}


	SetCurveParams();
	emit projectModified();
	s_bResetCurves = true;
	UpdateView();
}



/**
 * The user has requested the display of all the operating point curves for the active wing or plane
 */
void QMiarex::OnShowPlaneOpps()
{
	PlaneOpp *pPOpp;
	int i;
	for (i=0; i< m_poaPOpp->size(); i++)
	{
		pPOpp = (PlaneOpp*)m_poaPOpp->at(i);
		if (pPOpp->planeName() == m_pCurWPolar->planeName())
		{
			pPOpp->m_bIsVisible = true;
		}
	}

	emit projectModified();
	SetCurveParams();
	s_bResetCurves = true;
	UpdateView();
}


/**
 * The user has requested the display of all the operating point curves for the active polar
 */
void QMiarex::OnShowAllWPlrOpps()
{
	int i;
	//Switch all WOpps view to on for the current Plane and WPolar
	m_bCurPOppOnly = false;

	PlaneOpp *pPOpp;
	if(m_pCurPlane)
	{
		for (i=0; i< m_poaPOpp->size(); i++)
		{
			pPOpp = (PlaneOpp*)m_poaPOpp->at(i);
			if (pPOpp->planeName() == m_pCurWPolar->planeName() &&
				pPOpp->m_PlrName   == m_pCurWPolar->polarName())
			{
				pPOpp->m_bIsVisible = true;
			}
		}
	}
	emit projectModified();
	SetCurveParams();

	s_bResetCurves = true;
	UpdateView();
}

/**
 * The user has toggled the display of the elliptic curve in the lift graph in the operating point view
 */
void QMiarex::OnShowEllipticCurve()
{
	m_bShowElliptic = !m_bShowElliptic;
//	CheckMenus();

	CreateWOppCurves();
	UpdateView();
}


/**
 * The user has toggled the display of the moment reference point in the operating point view
 */
void QMiarex::OnShowXCmRef()
{
	//Show the Moment reference point
	m_bXCmRef = !m_bXCmRef;

	UpdateView();
}

/**
 * The user has toggled the display of the forces acting on the panels in the 3D view
 */
void QMiarex::OnPanelForce()
{
	m_bPanelForce	 = m_pctrlPanelForce->isChecked();
	if(m_bPanelForce)
	{
		m_b3DCp =false;
		m_pctrlCp->setChecked(false);
	}
	if(m_iView == W3DVIEW)
	{
		if(!m_bAnimateWOpp) UpdateView();
	}
}


/**
 * The user has toggled the display of the lift in the operating point view or in the 3D view
 */
void QMiarex::OnShowLift()
{
	m_bXCP	 = m_pctrlLift->isChecked();
	if(m_iView==WOPPVIEW || m_iView == W3DVIEW)
	{
		if(!m_bAnimateWOpp) UpdateView();
	}
}


/**
 * The user has toggled the display of the induced drag forces in the 3D view
 */
void QMiarex::OnShowIDrag()
{
	s_bICd = m_pctrlIDrag->isChecked();
	m_bResetglDrag = true;
	if(m_iView==WOPPVIEW || m_iView == W3DVIEW)
	{
		if(!m_bAnimateWOpp) UpdateView();
	}
}


/**
 * The user has toggled the display of the viscous drag forces in the 3D view
 */
void QMiarex::OnShowVDrag()
{
	s_bVCd = m_pctrlVDrag->isChecked();
	m_bResetglDrag = true;
	if(m_iView==WOPPVIEW || m_iView == W3DVIEW)
	{
		if(!m_bAnimateWOpp) UpdateView();
	}
}


/**
 * The user has toggled the display of the laminar to turbulent transition lines in the 3D view
 */
void QMiarex::OnShowTransitions()
{
	m_bXTop = m_pctrlTrans->isChecked();
	m_bXBot = m_pctrlTrans->isChecked();
	if(m_iView==WOPPVIEW || m_iView == W3DVIEW)
	{
		if(!m_bAnimateWOpp) UpdateView();
	}
}

/**
 * The user has toggled the display of the active curve
 */
void QMiarex::OnShowCurve()
{
	m_bCurveVisible = m_pctrlShowCurve->isChecked();
	m_bCurvePoints  = m_pctrlShowPoints->isChecked();
	UpdateCurve();
}


/**
 * The user has requested the display of the top left graph only
 **/
void QMiarex::OnSingleGraph1()
{
	if(m_iView==STABTIMEVIEW)
	{
		m_iStabTimeView = ONEGRAPH;
		m_pCurGraph = m_TimeGraph;
		m_pCurTimeGraph= m_pCurGraph;
		SetWingLegendPos();
		UpdateView();
	}
	else if(m_iView==WOPPVIEW)
	{
		m_iWingView = ONEGRAPH;
		m_pCurGraph = m_WingGraph;
		m_pCurWingGraph= m_pCurGraph;
		SetWingLegendPos();
		UpdateView();
	}
	else if(m_iView==WPOLARVIEW)
	{
		m_iWPlrView = ONEGRAPH;
		m_pCurGraph = m_WPlrGraph;
		m_pCurWPlrGraph = m_pCurGraph;
		SetWPlrLegendPos();
		UpdateView();
	}
	else return;
	SetControls();
}


/**
 * The user has requested the display of the top right graph only
 **/
void QMiarex::OnSingleGraph2()
{
	if(m_iView==STABTIMEVIEW)
	{
		m_iStabTimeView = ONEGRAPH;
		m_pCurGraph = m_TimeGraph+1;
		m_pCurTimeGraph= m_pCurGraph;
		SetWingLegendPos();
		UpdateView();
	}
	else if(m_iView==WOPPVIEW)
	{
		m_iWingView = ONEGRAPH;
		m_pCurGraph = m_WingGraph+1;
		m_pCurWingGraph= m_pCurGraph;
		SetWingLegendPos();
		UpdateView();
	}
	else if(m_iView==WPOLARVIEW)
	{
		m_iWPlrView = ONEGRAPH;
		m_pCurGraph = m_WPlrGraph+1;
		m_pCurWPlrGraph = m_pCurGraph;
		SetWPlrLegendPos();
		UpdateView();
	}
	else return;
	SetControls();
}


/**
 * The user has requested the display of the bottom left graph only
 **/
void QMiarex::OnSingleGraph3()
{
	if(m_iView==STABTIMEVIEW)
	{
		m_iStabTimeView = ONEGRAPH;
		m_pCurGraph = m_TimeGraph+2;
		m_pCurTimeGraph= m_pCurGraph;
		SetWingLegendPos();
	}
	else if(m_iView==WOPPVIEW)
	{
		m_iWingView = ONEGRAPH;
		m_pCurGraph = m_WingGraph+2;
		m_pCurWingGraph= m_pCurGraph;
		SetWingLegendPos();
	}
	else if(m_iView==WPOLARVIEW)
	{
		m_iWPlrView = ONEGRAPH;
		m_pCurGraph = m_WPlrGraph+2;
		m_pCurWPlrGraph = m_pCurGraph;
		SetWPlrLegendPos();
	}
	else return;
	UpdateView();
	SetControls();
}


/**
 * The user has requested the display of the bottom right graph only
 **/
void QMiarex::OnSingleGraph4()
{
	if(m_iView==STABTIMEVIEW)
	{
		m_iStabTimeView = ONEGRAPH;
		m_pCurGraph = m_TimeGraph+3;
		m_pCurTimeGraph= m_pCurGraph;
		SetWingLegendPos();
	}
	else if(m_iView==WOPPVIEW)
	{
		m_iWingView = ONEGRAPH;
		m_pCurGraph = m_WingGraph+3;
		m_pCurWingGraph= m_pCurGraph;
		SetWingLegendPos();
	}
	else if(m_iView==WPOLARVIEW)
	{
		m_iWPlrView = ONEGRAPH;
		m_pCurGraph = m_WPlrGraph+3;
		m_pCurWPlrGraph = m_pCurGraph;
		SetWPlrLegendPos();
	}
	else return;
	UpdateView();
	SetControls();
}


/**
 * The user has toggled the display of stability results between longitudinal and lateral directions
 */
void QMiarex::OnStabilityDirection()
{
	//the user has clicked either the longitudinal or lateral mode display
	//so update the view accordingly
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
	
	m_bLongitudinal = pStabView->m_pctrlLongDynamics->isChecked();

	for(int ig=0; ig<MAXGRAPHS; ig++) m_TimeGraph[ig].DeleteCurves();

	pStabView->m_pCurve = NULL;
	pStabView->FillCurveList();
	
	if(m_bLongitudinal) m_pCurRLStabGraph = &m_LongRLGraph;
	else                m_pCurRLStabGraph = &m_LatRLGraph;

	if(m_iView==STABPOLARVIEW) m_pCurGraph = m_pCurRLStabGraph;

	pStabView->SetMode();
	pStabView->SetControls();
	SetStabGraphTitles();
	SetWPlrLegendPos();

	SetCurveParams();
	SetControls();

	s_bResetCurves = true;
	UpdateView();
}


/**
 * The user has requested to change the display of stability results to the time view
 */
void QMiarex::OnTimeView()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
	StopAnimate();
	m_iView =  STABTIMEVIEW;
	pStabView->SetControls();
	SetWingLegendPos();
	
	pMainFrame->SetCentralWidget();
	SetCurveParams();
	SetControls();
	s_bResetCurves = true;
	UpdateView();
}

/**
 * The user has requested to change the display of stability results to the root locus view
 */
void QMiarex::OnRootLocusView()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StopAnimate();
	m_iView = STABPOLARVIEW;
	SetWPlrLegendPos();

	pMainFrame->SetCentralWidget();
	SetCurveParams();
	SetControls();
	s_bResetCurves = true;
	UpdateView();
}


/**
 * The user has requested to change the display of stability results to the modal view
 */
void QMiarex::OnModalView()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	m_iView = W3DVIEW;

	pMainFrame->SetCentralWidget();
	SetCurveParams();
	SetControls();
	UpdateView();
}


/**
 * The user has toggled the display of the streamlines in the 3D view
 */
void QMiarex::OnStreamlines()
{
	m_bStream = m_pctrlStream->isChecked();
	if(m_pctrlStream->isChecked())
	{
//		m_bResetglStream = true;
	}
	if(m_iView==W3DVIEW) UpdateView();
}



/**
 * The user has toggled the display of the surfaces in the 3D view
 */
void QMiarex::OnSurfaces()
{
	s_bSurfaces = m_pctrlSurfaces->isChecked();
	if(s_bSurfaces)
	{
		m_b3DCp = false;
		m_pctrlCp->setChecked(false);
	}
	UpdateView();
}



/**
 * The user has toggled the display of the velocity vectors on the surfaces in the 3D view
 */
void QMiarex::OnSurfaceSpeeds()
{
	m_bSurfVelocities = m_pctrlSurfVel->isChecked();
	if(m_pctrlSurfVel->isChecked())
	{
//		m_bResetglStream = true;
	}
	if(m_iView==W3DVIEW) UpdateView();
}

/**
 * The user has requested a modification of the light settings in the 3D view
 */
void QMiarex::OnSetupLight()
{
	if(m_iView!=W3DVIEW) return;
	ThreeDWidget *p3dWidget = (ThreeDWidget*)s_p3dWidget;

	GLLightDlg *pgllDlg = (GLLightDlg*)m_pglLightDlg;
	pgllDlg->m_p3DWidget= s_p3dWidget;
/*	QSize size = QSize(400,400);
	QPoint pos = QPoint(100,50);
	gllDlg.resize(size);
	gllDlg.move(pos); */
	pgllDlg->show();

	double LightFactor;
	if(m_pCurPlane) LightFactor =  (GLfloat)pow(m_pCurPlane->planformSpan()/2.0,0.1);
	else            LightFactor = 1.0;
	p3dWidget->GLSetupLight(m_PlaneOffset.y, LightFactor);
	UpdateView();
}


/**
 * The user has toggled the request to store or not the operating points of an analysis
 */
void QMiarex::OnStoreWOpp()
{
	PlaneOpp::s_bStoreOpps = m_pctrlStoreWOpp->isChecked();
}



/**
 * The user has requested to switch to the two graph view top-left and top-right
 */
void QMiarex::OnTwoGraphs()
{
	if(m_iView==WOPPVIEW)
	{
		m_iWingView = TWOGRAPHS;
		m_pCurGraph = m_WingGraph;
		m_pCurWingGraph = m_pCurGraph;

		SetWingLegendPos();
	}
	else if(m_iView==WPOLARVIEW)
	{
		m_iWPlrView = TWOGRAPHS;
		m_pCurGraph = m_WPlrGraph;
		m_pCurWPlrGraph = m_pCurGraph;
		SetWPlrLegendPos();
	}
	else if(m_iView==STABTIMEVIEW)
	{
		m_iStabTimeView= TWOGRAPHS;
		m_pCurGraph = m_TimeGraph;
		m_pCurTimeGraph = m_pCurGraph;

		SetWingLegendPos();
	}
	else return;
	UpdateView();
	SetControls();
}


/**
 * The user has requested to switch to the four graph view
 */
void QMiarex::OnFourGraphs()
{

	if(m_iView==WOPPVIEW)
	{
		m_iWingView = ALLGRAPHS;
		m_pCurGraph = m_WingGraph;
		m_pCurWingGraph = m_pCurGraph;
		SetWingLegendPos();
	}
	else if(m_iView==WPOLARVIEW)
	{
		m_iWPlrView = ALLGRAPHS;
		m_pCurGraph = m_WPlrGraph;
		m_pCurWPlrGraph = m_pCurGraph;
		SetWPlrLegendPos();
	}
	else if(m_iView==STABTIMEVIEW)
	{
		m_iStabTimeView= ALLGRAPHS;
		m_pCurGraph = m_TimeGraph;
		m_pCurTimeGraph = m_pCurGraph;
		SetWingLegendPos();
	}
	else return;
	UpdateView();
	SetControls();
}


/**
 * The user has toggled the display of the vortices in 3D view
 *@deprecated Option disabled, only used for development
 */
void QMiarex::OnVortices()
{
//	m_bVortices = m_pctrlVortices->isChecked();
	UpdateView();
}


/**
 * The user has toggled the display of the curves for the second wing in case of a bi-plane
 *@todo not thouroughly tested
 */
void QMiarex::OnWing2Curve()
{
	m_bShowWingCurve[1] = !m_bShowWingCurve[1];
//	CheckMenus();

	if (m_iView==WOPPVIEW)    CreateWOppCurves();
	else if(m_iView==WCPVIEW) CreateCpCurves();

	UpdateView();
}


/**
 * The user has requested the edition of the inertia data for the current wing or plane
 * Updates the inertia, resets the depending polars, and deletes the obsolete operating points
 * Updates the display
 */
void QMiarex::OnPlaneInertia()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(!m_pCurPlane) return;

	InertiaDlg iDlg(pMainFrame);
	iDlg.m_pPlane = NULL;
	iDlg.m_pWing  = NULL;
	iDlg.m_pBody  = NULL;

	Plane *pSavePlane = new Plane;
	Wing *pSaveWing = new Wing;
	WPolar *pWPolar;
	QString PlaneName;
	bool bHasResults = false;

	if(m_pCurPlane)
	{
		PlaneName = m_pCurPlane->planeName();
		pSavePlane->Duplicate(m_pCurPlane);
		iDlg.m_pPlane = m_pCurPlane;
	}

	for (int i=0; i< m_poaWPolar->size(); i++)
	{
		pWPolar = (WPolar*)m_poaWPolar->at(i);
		if(pWPolar->m_Alpha.size() && pWPolar->planeName()==PlaneName && pWPolar->m_bAutoInertia)
		{
//			if(pWPolar->polarType()==STABILITYPOLAR)
//			{
				bHasResults = true;
				break;
//			}
		}
	}

	iDlg.InitDialog();

	ModDlg mdDlg(pMainFrame);

	if(iDlg.exec()==QDialog::Accepted)
	{
		if(bHasResults)
		{
			mdDlg.m_Question = tr("The modification will erase all polar results associated to this Plane.\nContinue ?");
			mdDlg.InitDialog();
			int Ans = mdDlg.exec();

			if (Ans == QDialog::Rejected)
			{
				//restore saved Plane
				if(m_pCurPlane)     m_pCurPlane->Duplicate(pSavePlane);
				return;
			}
			else if(Ans==20)
			{
				if(m_pCurPlane)
				{
					//save mods to a new plane object
					Plane* pNewPlane= new Plane;
					pNewPlane->Duplicate(m_pCurPlane);

					//restore geometry for initial plane
					m_pCurPlane->Duplicate(pSavePlane);

					//set the new current plane
					m_pCurPlane = Objects3D::setModPlane(pNewPlane);
				}
				SetPlane();
				pMainFrame->UpdatePlaneListBox();
				UpdateView();
				return;
			}

			//last case, user wants to overwrite, so reset all polars, WOpps and POpps with autoinertia associated to the Plane

			for (int i=0; i<m_poaWPolar->size(); i++)
			{
				pWPolar = (WPolar*)m_poaWPolar->at(i);
				if(pWPolar && pWPolar->planeName()==PlaneName && pWPolar->m_bAutoInertia)
				{
					pWPolar->ClearData();
					if(m_pCurPlane)     pWPolar->RetrieveInertia(m_pCurPlane, true);

					for (int i=m_poaPOpp->size()-1; i>=0; i--)
					{
						PlaneOpp *pPOpp = (PlaneOpp*)m_poaPOpp->at(i);
						if(pPOpp && pPOpp->planeName()==PlaneName && pPOpp->m_PlrName==pWPolar->polarName())
						{
							m_poaPOpp->removeAt(i);
							delete pPOpp;
						}
					}
				}
			}

			m_pCurPOpp = NULL;
		}
		SetWPolar();
		emit projectModified();
		s_bResetCurves = true;
		UpdateView();
	}
	else
	{
		//restore saved Plane
		if(m_pCurPlane)    m_pCurPlane->Duplicate(pSavePlane);
	}
	delete pSavePlane;
	delete pSaveWing;
}


/**
 * The user ha requested to switch to the operating point view
 */
void QMiarex::OnWOppView()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	if(m_iView==WOPPVIEW)
	{
		SetControls();
		UpdateView();
		return;
	}

	m_pCurGraph = NULL;

	m_iView=WOPPVIEW;

	pMainFrame->SetCentralWidget();

	m_bIs2DScaleSet = false;
	Set2DScale();
	CreateWOppCurves();
	SetCurveParams();
	SetControls();

	UpdateView();
}



/**
 * The user has requested to switch to the polar view
 */
void QMiarex::OnWPolarView()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if (m_bAnimateWOpp) StopAnimate();

	if(m_iView==WPOLARVIEW)
	{
		SetControls();
		UpdateView();
		return;
	}

	m_iView=WPOLARVIEW;
	if(!m_pCurWPlrGraph) m_pCurGraph = NULL;
	else                 m_pCurGraph = m_pCurWPlrGraph;

	pMainFrame->SetCentralWidget();

	SetWPlrLegendPos();
	SetCurveParams();
	SetControls();

	s_bResetCurves = true;
	UpdateView();
}


/**
 * Dispatching method for the update of the client area of the active twodwidget
 * @param painter a reference to the QPainter object on which the view shall be drawn
 */
void QMiarex::PaintView(QPainter &painter)
{
	static int h,w,w2,h2, h23,h34, h38,w3,w23;
	static QRect Rect1, Rect2, Rect3, Rect4;
	static QPoint Place;
	QPen TextPen;
	QString GraphName;
	painter.save();
	w   = m_r2DCltRect.width();
	w2  = (int)(w/2);
	w3  = (int)(0.35*w);
	w23 = 2*w3;
	h   = m_r2DCltRect.height();
	h2  = (int)(h/2);
	h23 = (int)(2*h/3);
	h34 = (int)(3*h/4);
	h38 = (int)(3*h/8);
	//Refresh the active view
	painter.fillRect(m_r2DCltRect, Settings::s_BackgroundColor);

	if(m_r2DCltRect.width()<200 || m_r2DCltRect.height()<200)
	{
		painter.restore();
		return;//too small to paint
	}

	if (m_iView==WPOLARVIEW)
	{
		if(s_bResetCurves)
		{
			CreateWPolarCurves();
			s_bResetCurves = false;
		}

		if (m_iWPlrView == ONEGRAPH)
		{
			if(!m_pCurGraph)
			{
				m_pCurGraph     = m_WPlrGraph;
				m_pCurWPlrGraph = m_WPlrGraph;
			}
			Rect1.setRect(0,0,w23,m_r2DCltRect.bottom()-00);
			m_pCurGraph->DrawGraph(Rect1, painter);
			DrawWPolarLegend(painter, m_WPlrLegendOffset, Rect1.bottom());
		}
		else if(m_iWPlrView == TWOGRAPHS)
		{
//			PaintWCoupleGraphs(painter);
			Rect1.setRect(0,0,w2,h23);
			Rect2.setRect(w2,0,w2,h23);
	
			m_WPlrGraph[0].DrawGraph(Rect1, painter);
			m_WPlrGraph[1].DrawGraph(Rect2, painter);
	
			DrawWPolarLegend(painter, m_WPlrLegendOffset, m_r2DCltRect.bottom());
		}
		else if(m_iWPlrView == ALLGRAPHS)
		{
//			PaintWFourGraphs(painter);
			Rect1.setRect(0,0,w3,h2);
			Rect2.setRect(w3,0,w3,h2);
			Rect3.setRect(0,h2,w3,h2);
			Rect4.setRect(w3,h2,w3,h2);
	
			m_WPlrGraph[0].DrawGraph(Rect1, painter);
			m_WPlrGraph[1].DrawGraph(Rect2, painter);
			m_WPlrGraph[2].DrawGraph(Rect3, painter);
			m_WPlrGraph[3].DrawGraph(Rect4, painter);
	
			DrawWPolarLegend(painter, m_WPlrLegendOffset, m_r2DCltRect.bottom());
		}
	}
	else if (m_iView==WOPPVIEW)
	{
		if(s_bResetCurves)
		{
			CreateWOppCurves();
			s_bResetCurves = false;
		}
		if (m_iWingView == ONEGRAPH && m_pCurPlane)
		{
//			PaintSingleWingGraph(painter);
			if (m_pCurWingGraph && m_pCurPlane && m_r2DCltRect.width()/2>200 && m_r2DCltRect.height()>250)
			{
				QPoint Place(m_r2DCltRect.left()+10, m_r2DCltRect.top() +30);
				m_pCurWingGraph->DrawGraph(m_rSingleRect, painter);
				DrawOppGraphLegend(painter, Place, m_r2DCltRect.bottom());
			}
		
			painter.setFont(Settings::s_TextFont);
		
			QPen TextPen(Settings::s_TextColor);
			TextPen.setWidth(1);
			painter.setPen(TextPen);
		
			if(m_pCurPlane)
			{
				PaintWing(painter, m_ptOffset, m_WingScale);
				if(m_pCurPOpp && m_pCurPOpp->m_bIsVisible)
				{
					QPoint PtLegend;
					PtLegend.rx() = (int)(m_r2DCltRect.width()/2);
					PtLegend.ry() = m_r2DCltRect.bottom();
					PaintXTr(painter, m_ptOffset, m_WingScale);
					if (m_bXCP)    PaintXCP(painter, m_ptOffset, m_WingScale);
					if (m_bXCmRef) PaintXCmRef(painter, m_ptOffset, m_WingScale);
				}
				PaintPlaneLegend(painter, m_r2DCltRect);
				if(m_pCurPOpp) PaintPlaneOppLegend(painter, m_r2DCltRect);
			}
		}
		else if (m_iWingView == TWOGRAPHS && m_pCurPlane)
		{
			DrawOppGraphLegend(painter, m_WingLegendOffset, m_r2DCltRect.bottom());
	
			Rect1.setRect(m_r2DCltRect.left(),   m_r2DCltRect.top(), w2, h23);
			Rect2.setRect(w2, m_r2DCltRect.top(),                    w2, h23);
			m_WingGraph[0].DrawGraph(Rect1, painter);
			m_WingGraph[1].DrawGraph(Rect2, painter);
		}
		else if (m_iWingView == ALLGRAPHS && m_pCurPlane)
		{
			DrawOppGraphLegend(painter, m_WingLegendOffset, m_r2DCltRect.bottom());
		
			Rect1.setRect(0,0,w2,h38);
			Rect2.setRect(w2,0,w2,h38);
			Rect3.setRect(0,h38,w2,h38);
			Rect4.setRect(w2,h38,w2,h38);
			m_WingGraph[0].DrawGraph(Rect1, painter);
			m_WingGraph[1].DrawGraph(Rect2, painter);
			m_WingGraph[2].DrawGraph(Rect3, painter);
			m_WingGraph[3].DrawGraph(Rect4, painter);
		}
	}
	else if (m_iView==WCPVIEW)
	{
		if(s_bResetCurves)
		{
			CreateCpCurves();
			s_bResetCurves = false;
		}

		m_CpGraph.DrawGraph(painter);
		QPoint Place(50, h34+20);
		DrawCpLegend(painter, Place, m_r2DCltRect.bottom());
	}

	else	if(m_iView==STABTIMEVIEW)
	{
		if(s_bResetCurves)
		{
			if(m_StabilityResponseType==1)  CreateStabRungeKuttaCurves();
			else                            CreateStabTimeCurves();
			s_bResetCurves = false;
		}

		if(m_iStabTimeView==ONEGRAPH)
		{
//				PaintSingleTimeGraph(painter);
			if(!m_pCurTimeGraph) m_pCurTimeGraph = m_TimeGraph;

			Rect1.setRect(0,0,w,h34);
			m_pCurTimeGraph->DrawGraph(Rect1, painter);
			Place.setX((int)(w*2/5));
			Place.setY(m_pCurTimeGraph->GetMargin()/2);
			m_pCurTimeGraph->GetGraphName(GraphName);
			TextPen.setColor(m_pCurTimeGraph->GetTitleColor());
			painter.setPen(TextPen);
			painter.drawText(Place, GraphName);

			DrawStabTimeLegend(painter, m_WingLegendOffset, m_r2DCltRect.bottom());
		}
		else if(m_iStabTimeView==TWOGRAPHS)
		{
			//Paint a two graph time view
			DrawStabTimeLegend(painter, m_WingLegendOffset, m_r2DCltRect.bottom());
			Rect1.setRect(m_r2DCltRect.left(),   m_r2DCltRect.top(), w2, h23);
			Rect2.setRect(w2, m_r2DCltRect.top(),                    w2, h23);
			m_TimeGraph[0].DrawGraph(Rect1, painter);
			m_TimeGraph[1].DrawGraph(Rect2, painter);
		}
		else if(m_iStabTimeView==ALLGRAPHS)
		{
//				PaintFourTimeGraph(painter);
			//Paint a four graph time view
			DrawStabTimeLegend(painter, m_WingLegendOffset, m_r2DCltRect.bottom());
			Rect1.setRect(0,0,w2,h38);
			Rect2.setRect(w2,0,w2,h38);
			Rect3.setRect(0,h38,w2,h38);
			Rect4.setRect(w2,h38,w2,h38);
			m_TimeGraph[0].DrawGraph(Rect1, painter);
			m_TimeGraph[1].DrawGraph(Rect2, painter);
			m_TimeGraph[2].DrawGraph(Rect3, painter);
			m_TimeGraph[3].DrawGraph(Rect4, painter);
		}
	}
	else if(m_iView==STABPOLARVIEW)
	{
		if(s_bResetCurves)
		{
			CreateStabRLCurves();
			s_bResetCurves = false;
		}
		if(m_pCurRLStabGraph)
		{
			Rect1.setRect(0,0,w23,m_r2DCltRect.bottom()-00);
			m_pCurRLStabGraph->DrawGraph(Rect1, painter);
			Place.setX((int)(w23/3));
			Place.setY(m_pCurRLStabGraph->GetMargin()/2);
			m_pCurRLStabGraph->GetGraphName(GraphName);
			TextPen.setColor(m_pCurRLStabGraph->GetTitleColor());
			painter.setPen(TextPen);
			painter.drawText(Place, GraphName);
			Place.setX(w23+10);
			Place.setY(10);
			DrawWPolarLegend(painter, m_WPlrLegendOffset, m_r2DCltRect.bottom());
		}
	}

	painter.restore();
}


/**
 * Draws the wing in the 2D operating point view
 * @param painter a reference to the QPainter object on which the view shall be drawn
 * @param ORef the origin of the tip of the root chord, in client coordinates
 * @param scale the scaling factor with which to draw the wing
 */
void QMiarex::PaintWing(QPainter &painter, QPoint ORef, double scale)
{
	if(!m_pCurPlane)	return;
	static int i;
	static double scalex, scaley;

	scalex  = scale;
	scaley  = scale;

	Wing *pWing = m_pCurPlane->m_Wing;

	painter.save();
    QPen WingPen(W3dPrefsDlg::s_OutlineColor);
	WingPen.setStyle(getStyle(W3dPrefsDlg::s_OutlineStyle));
    WingPen.setWidth(W3dPrefsDlg::s_OutlineWidth);

	painter.setPen(WingPen);

	QPoint O(ORef);

	//Right Wing
	O.rx() = ORef.x();
	O.ry() = ORef.y();
	for (i=0; i<pWing->NWingSection()-1;i++)
	{
		O.rx() +=(int)(pWing->Length(i)*scalex);
		painter.drawLine(O.x(),                                        O.y()+(int)(pWing->Offset(i)*scaley),
						 O.x()+(int)(pWing->Length(i+1)*scalex),  O.y()+(int)(pWing->Offset(i+1)*scaley));

		painter.drawLine(O.x()+(int)(pWing->Length(i+1)*scalex),  O.y()+(int)(pWing->Offset(i+1)*scaley),
						 O.x()+(int)(pWing->Length(i+1)*scalex),  O.y()+(int)((pWing->Offset(i+1)+pWing->Chord(i+1))*scaley));

		painter.drawLine(O.x()+(int)(pWing->Length(i+1)*scalex),  O.y()+(int)((pWing->Offset(i+1)+pWing->Chord(i+1))*scaley),
						 O.x(),                                        O.y() +(int)((pWing->Offset(i)+pWing->Chord(i))*scaley));

		painter.drawLine(O.x(),                                        O.y() +(int)((pWing->Offset(i)+pWing->Chord(i))*scaley),
						 O.x(),                                        O.y()+(int)(pWing->Offset(i)*scaley));
	}
	if(!m_bHalfWing)
	{
		//LeftWing
		O.rx() = ORef.x();
		O.ry() = ORef.y();

		for (i=0; i<pWing->NWingSection()-1;i++)
		{
			O.rx() -= (int)(pWing->Length(i)*scalex);
			painter.drawLine(O.x(),						                     O.y()+(int)(pWing->Offset(i)*scaley),
							 O.x()-(int)(pWing->Length(i+1)*scalex), O.y()+(int)(pWing->Offset(i+1)*scaley));

			painter.drawLine(O.x()-(int)(pWing->Length(i+1)*scalex), O.y()+(int)(pWing->Offset(i+1)*scaley),
							 O.x()-(int)(pWing->Length(i+1)*scalex), O.y()+(int)((pWing->Offset(i+1)+pWing->Chord(i+1))*scaley));

			painter.drawLine(O.x()-(int)(pWing->Length(i+1)*scalex), O.y()+(int)((pWing->Offset(i+1)+pWing->Chord(i+1))*scaley),
							 O.x(),                                        O.y() +(int)((pWing->Offset(i)+pWing->Chord(i))*scaley));

			painter.drawLine(O.x(),                                        O.y() +(int)((pWing->Offset(i)+pWing->Chord(i))*scaley),
							 O.x(),                                        O.y()+(int)(pWing->Offset(i)*scaley));
		}
	}

	QPen SymPen(QColor(155,128,190));
	painter.setPen(SymPen);
	painter.setBackgroundMode(Qt::TransparentMode);

	painter.drawLine(ORef.x(), ORef.y()-20, ORef.x(), ORef.y()+75);
	painter.restore();
}


/**
 * Draws the wing legend in the 2D operating point view
 * @param painter a reference to the QPainter object on which the view shall be drawn
 */
void QMiarex::PaintPlaneLegend(QPainter &painter, QRect drawRect)
{
	if(!m_pCurPlane) return;
	painter.save();

	QString Result, str, strong;
	QString str1;
	static double Mass;
	static int margin,dheight;

	QPen textPen(Settings::s_TextColor);
	painter.setPen(textPen);
	painter.setFont(Settings::s_TextFont);
	painter.setRenderHint(QPainter::Antialiasing);

	margin = 10;

	QFontMetrics fm(Settings::s_TextFont);
	dheight = fm.height();
	int D = 0;
	int LeftPos = margin;
	int ZPos    = drawRect.height()-14*dheight;

//	double area = m_pCurPlane->m_Wing[0].s_RefArea;
	if(m_pCurPlane && m_pWingList[2]) ZPos -= dheight;

	painter.drawText(LeftPos, ZPos, m_pCurPlane->planeName());
	D+=dheight;
	QString length, surface;
	Units::getLengthUnitLabel(length);
	Units::getAreaUnitLabel(surface);

	str1 = QString(tr("Wing Span      =")+"%1 ").arg(m_pCurPlane->planformSpan()*Units::mtoUnit(),10,'f',3);
	str1 += length;
	painter.drawText(LeftPos,ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("xyProj. Span   =")+"%1 ").arg(m_pCurPlane->projectedSpan()*Units::mtoUnit(),10,'f',3);
	str1 += length;
	painter.drawText(LeftPos,ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("Wing Area      =")+"%1 ").arg(m_pCurPlane->planformArea() * Units::m2toUnit(),10,'f',3);
	str1 += surface;
	painter.drawText(LeftPos,ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("xyProj. Area   =")+"%1 ").arg(m_pCurPlane->projectedArea() * Units::m2toUnit(),10,'f',3);
	str1 += surface;
	painter.drawText(LeftPos,ZPos+D, str1);
	D+=dheight;

	if(m_pCurWPolar && !m_pCurWPolar->m_bAutoInertia) Mass = m_pCurWPolar->m_Mass;
	else                                              Mass = m_pCurPlane->TotalMass();

	Units::getWeightUnitLabel(str);
	Result = QString(tr("Plane Mass     =")+"%1 ").arg(Mass*Units::kgtoUnit(),10,'f',3);
	Result += str;
	painter.drawText(LeftPos, ZPos+D, Result);
	D+=dheight;

	Units::getAreaUnitLabel(strong);
	Result = QString(tr("Wing Load      =")+"%1 ").arg(Mass*Units::kgtoUnit()/m_pCurPlane->planformArea()/Units::m2toUnit(),10,'f',3);
	Result += str + "/" + strong;
	painter.drawText(LeftPos, ZPos+D, Result);
	D+=dheight;

	if(m_pCurPlane && m_pWingList[2])
	{
		str1 = QString(tr("Tail Volume    =")+"%1").arg(m_pCurPlane->TailVolume(),10,'f',3);
		painter.drawText(LeftPos, ZPos+D, str1);
		D+=dheight;
	}

	str1 = QString(tr("Root Chord     =")+"%1 ").arg(m_pCurPlane->m_Wing[0].RootChord()*Units::mtoUnit(), 10,'f', 3);
	Result = str1+length;
	painter.drawText(LeftPos, ZPos+D, Result);
	D+=dheight;

	str1 = QString(tr("MAC            =")+"%1 ").arg(m_pCurPlane->mac()*Units::mtoUnit(), 10,'f', 3);
	Result = str1+length;
	painter.drawText(LeftPos, ZPos+D, Result);
	D+=dheight;

	str1 = QString(tr("TipTwist       =")+"%1").arg(m_pCurPlane->m_Wing[0].TipTwist(), 10,'f', 3) + QString::fromUtf8("°");
	painter.drawText(LeftPos, ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("Aspect Ratio   =")+"%1").arg(m_pCurPlane->m_Wing[0].m_AR,10,'f',3);
	painter.drawText(LeftPos, ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("Taper Ratio    =")+"%1").arg(m_pCurPlane->m_Wing[0].m_TR,10,'f',3);
	painter.drawText(LeftPos, ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("Root-Tip Sweep =")+"%1").arg(m_pCurPlane->m_Wing[0].AverageSweep(), 10,'f',3) + QString::fromUtf8("°");
	painter.drawText(LeftPos, ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("Mesh elements  =")+"%1").arg(Objects3D::calculateMatSize(m_pCurPlane, m_pCurWPolar),6);
	painter.drawText(LeftPos, ZPos+D, str1);

	painter.restore();
}


/**
 * Draws the legend of the operating point in the 2D and 3D operating point views
 * @param painter a reference to the QPainter object on which the view shall be drawn
 */
void QMiarex::PaintPlaneOppLegend(QPainter &painter, QRect drawRect)
{
	if(!m_pCurPOpp) return;

	painter.save();

	QString Result, str;

	int i;
	int margin = 10;
	int dwidth, dheight;


	QPen textPen(Settings::s_TextColor);
	painter.setPen(textPen);

	painter.setFont(Settings::s_TextFont);
	painter.setRenderHint(QPainter::Antialiasing);


	QFontMetrics fm(Settings::s_TextFont);
	dheight = fm.height();
	dwidth = fm.averageCharWidth()*50;
	int D = 0;

	D = 0;
	int RightPos = drawRect.right()-margin - dwidth;
	int ZPos	 = drawRect.height()-13*dheight;

	if(m_pCurPOpp && m_pCurPOpp->m_WPolarType==STABILITYPOLAR) ZPos -= dheight;
	if(m_pCurPOpp && m_pCurPOpp->m_bOut)                       ZPos -= dheight;
	if(m_pCurPOpp)                                             ZPos -= dheight*m_pCurPOpp->m_pPlaneWOpp[0]->m_nFlaps;

	if(m_pCurPOpp && m_pCurPOpp->m_bIsVisible)
	{
		if(m_pCurPOpp->m_bOut)
		{
			Result = tr("Point is out of the flight envelope");
			painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);
			D+=dheight;
		}

		Units::getSpeedUnitLabel(str);
		int l = str.length();
		if(l==2)      Result = QString("V = %1 ").arg(m_pCurPOpp->m_QInf*Units::mstoUnit(),8,'f',3);
		else if(l==3) Result = QString("V = %1 ").arg(m_pCurPOpp->m_QInf*Units::mstoUnit(),7,'f',2);
		else if(l==4) Result = QString("V = %1 ").arg(m_pCurPOpp->m_QInf*Units::mstoUnit(),6,'f',1);
		else          Result = "No unit defined for speed...";

		Result += str;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		int e = 8, f=3;


		Result = QString("Alpha = %1").arg(m_pCurPOpp->m_Alpha, e,'f',f) + QString::fromUtf8("°  ");
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString("Beta = %1").arg(m_pCurPOpp->m_Beta, e,'f',f) + QString::fromUtf8("°  ");
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString("CL = %1   ").arg(m_pCurPOpp->m_CL, e,'f',f);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString("CD = %1   " ).arg(m_pCurPOpp->m_VCD+m_pCurPOpp->m_ICD, e,'f',f);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		/*		oswald=CZ^2/CXi/PI/allongement;*/
		double cxielli=m_pCurPOpp->m_CL*m_pCurPOpp->m_CL/PI/m_pCurPlane->m_Wing[0].m_AR;
		Result = QString("Efficiency = %1   ").arg(cxielli/m_pCurPOpp->m_ICD, e,'f',f);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString("CL/CD = %1   ").arg(m_pCurPOpp->m_CL/(m_pCurPOpp->m_ICD+m_pCurPOpp->m_VCD), e,'f',f);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString("Cm = %1   ").arg(m_pCurPOpp->m_GCm, e,'f',f);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString("Cl = %1   ").arg(m_pCurPOpp->m_GRm, e,'f',f);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString("Cn = %1   ").arg(m_pCurPOpp->m_GYm, e,'f',f);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);


		Units::getLengthUnitLabel(str);
		l = str.length();
		int c=8, d=3;
		if(l==1)  str+=" ";
		if(m_pCurPOpp->m_WPolarType==STABILITYPOLAR)
		{
			Result = QString("X_NP = %1 ").arg(m_pCurPOpp->m_XNP*Units::mtoUnit(), c,'f',d);
			Result += str;
			D+=dheight;
			painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);
		}

		Result = QString("X_CP = %1 ").arg(m_pCurPOpp->m_CP.x*Units::mtoUnit(), c, 'f', d);
		Result += str;
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString("X_CG = %1 ").arg(m_pCurWPolar->m_CoG.x*Units::mtoUnit(), c, 'f', d);
		Result += str;
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		for(i=0; i<m_pCurPOpp->m_pPlaneWOpp[0]->m_nFlaps; i++)
		{
			Result = QString("Flap %1 Moment =%2 ").arg(i+1).arg(m_pCurPOpp->m_pPlaneWOpp[0]->m_FlapMoment[i]*Units::NmtoUnit(),8,'f',4);
			Units::getMomentUnitLabel(str);
			Result += str;
			D+=dheight;
			painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);
		}
	}
	painter.restore();
}




/**
 * Draws the laminar to turbulent translition lines in the operating view
 * @param painter a reference to the QPainter object on which the view shall be drawn
 * @param ORef the origin of the tip of the root chord, in client coordinates
 * @param scale the scaling factor with which to draw the wing
 */
void QMiarex::PaintXTr(QPainter & painter, QPoint ORef, double scale)
{
	//Draws the transition lines on the 2D view
	if(!m_pCurPlane)	return;

	Wing *pWing = m_pCurPlane->m_Wing;
	painter.save();

	double y;
	int m,nStart;
	if(m_pCurPOpp->m_AnalysisMethod==LLTMETHOD) nStart = 1; else nStart = 0;
	if(m_bHalfWing) nStart = m_pCurPOpp->m_pPlaneWOpp[0]->m_NStation/2;
	QPoint O(ORef);
	QPoint offset, From, To;

	double offLE;
	double scalex, scaley;
	offset.rx() = ORef.x();
	offset.ry() = ORef.y();
	scalex  = scale;
	scaley  = scale;

	O.rx() = offset.x();
	O.ry() = offset.y();

    QPen TopPen(W3dPrefsDlg::s_TopColor);
	TopPen.setStyle(getStyle(W3dPrefsDlg::s_TopStyle));
    TopPen.setWidth(W3dPrefsDlg::s_TopWidth);
	painter.setPen(TopPen);

	if (m_bXTop)
	{
		offLE = pWing->getOffset(m_pCurPOpp->m_pPlaneWOpp[0]->m_SpanPos[nStart]*2.0/m_pCurPOpp->m_pPlaneWOpp[0]->m_Span);
		y = (offLE+m_pCurPOpp->m_pPlaneWOpp[0]->m_Chord[nStart]*m_pCurPOpp->m_pPlaneWOpp[0]->m_XTrTop[nStart])*scaley;
		From = QPoint(O.x()+(int)(m_pCurPOpp->m_pPlaneWOpp[0]->m_SpanPos[nStart]*scalex),	O.y()+(int)y);

		for (m=nStart; m<m_pCurPOpp->m_pPlaneWOpp[0]->m_NStation; m++)
		{
			offLE = pWing->getOffset(m_pCurPOpp->m_pPlaneWOpp[0]->m_SpanPos[m]*2.0/m_pCurPOpp->m_pPlaneWOpp[0]->m_Span);
			y = (offLE+m_pCurPOpp->m_pPlaneWOpp[0]->m_Chord[m]*m_pCurPOpp->m_pPlaneWOpp[0]->m_XTrTop[m])*scaley;

			To = QPoint(O.x()+(int)(m_pCurPOpp->m_pPlaneWOpp[0]->m_SpanPos[m]*scalex), O.y()+(int)y );
			painter.drawLine(From, To);
			From  = To;
		}


		int x = (int)(m_r2DCltRect.width()/2);
		int y = m_r2DCltRect.bottom();
		painter.drawLine(x-60,  y - 50, x-40,  y - 50);
		painter.drawText(x-35, y - 48, tr("Top transition"));

	}


    QPen BotPen(W3dPrefsDlg::s_BotColor);
	BotPen.setStyle(getStyle(W3dPrefsDlg::s_BotStyle));
    BotPen.setWidth(W3dPrefsDlg::s_BotWidth);

	painter.setPen(BotPen);
	if (m_bXBot)
	{
		offLE = pWing->getOffset(m_pCurPOpp->m_pPlaneWOpp[0]->m_SpanPos[nStart]*2.0/m_pCurPOpp->m_pPlaneWOpp[0]->m_Span);
		y = (offLE+m_pCurPOpp->m_pPlaneWOpp[0]->m_Chord[nStart]*m_pCurPOpp->m_pPlaneWOpp[0]->m_XTrBot[nStart])*scaley;
		From = QPoint(O.x() +(int)(m_pCurPOpp->m_pPlaneWOpp[0]->m_SpanPos[nStart]*scalex), O.y()+(int)y );
		for (m=nStart; m<m_pCurPOpp->m_pPlaneWOpp[0]->m_NStation; m++)
		{
			offLE = pWing->getOffset(m_pCurPOpp->m_pPlaneWOpp[0]->m_SpanPos[m]*2.0/m_pCurPOpp->m_pPlaneWOpp[0]->m_Span);
			y = (offLE+m_pCurPOpp->m_pPlaneWOpp[0]->m_Chord[m]*m_pCurPOpp->m_pPlaneWOpp[0]->m_XTrBot[m])*scaley;
			To = QPoint(O.x()+(int)(m_pCurPOpp->m_pPlaneWOpp[0]->m_SpanPos[m]*scalex), O.y()+(int)y );
			painter.drawLine(From, To);
			From  = To;
		}


		int x = (int)(m_r2DCltRect.width()/2);
		int y = m_r2DCltRect.bottom();
		painter.drawLine(x-60,  y - 35, x-40,  y - 35);
		painter.drawText(x-35, y - 33, tr("Bottom transition"));

	}
	painter.restore();
}



/**
 * Draws the lift line and the position of the center of pressure in the operating view
 * @param painter a reference to the QPainter object on which the view shall be drawn
 * @param ORef the origin of the tip of the root chord, in client coordinates
 * @param scale the scaling factor with which to draw the wing
 */
void QMiarex::PaintXCP(QPainter & painter, QPoint ORef, double scale)
{
	//Draws the lift line and center of pressure position on the the 2D view
	if(!m_pCurPlane)	return;

	Wing *pWing = m_pCurPlane->m_Wing;

	painter.save();

	QPoint From, To;

	double y;
	int nStart;
	double offLE;
	if(m_pCurPOpp->m_AnalysisMethod==LLTMETHOD) nStart = 1; else nStart = 0;
	if(m_bHalfWing) nStart = m_pCurPOpp->m_pPlaneWOpp[0]->m_NStation/2;

	QPoint O(ORef);
	QPoint offset;

	double scalex, scaley;
	offset.rx() = ORef.x();
	offset.ry() = ORef.y();
	scalex  = scale;
	scaley  = scale;
	O.rx() = offset.x();
	O.ry() = offset.y();

    QPen XCPPen(W3dPrefsDlg::s_XCPColor);
    XCPPen.setWidth(W3dPrefsDlg::s_XCPWidth);
	XCPPen.setStyle(getStyle(W3dPrefsDlg::s_XCPStyle));
	painter.setPen(XCPPen);

	int XCp = O.x() + (int)(m_pCurPOpp->m_pPlaneWOpp[0]->m_CP.y*scalex);
	int YCp = O.y() + (int)(m_pCurPOpp->m_pPlaneWOpp[0]->m_CP.x*scaley);
//    int ZCp = O.z() + (int)(m_pCurPOpp->m_pPlaneWOpp[0]->m_ZCP*scalez);
	int size = 3;
	QRect CP(XCp-size, YCp-size, 2*size, 2*size);
	painter.drawEllipse(CP);

	offLE = pWing->getOffset(m_pCurPOpp->m_pPlaneWOpp[0]->m_SpanPos[nStart]*2.0/m_pCurPOpp->m_pPlaneWOpp[0]->m_Span);
	y = (offLE+m_pCurPOpp->m_pPlaneWOpp[0]->m_Chord[nStart]*m_pCurPOpp->m_pPlaneWOpp[0]->m_XCPSpanRel[nStart])*scaley;
	From = QPoint(O.x()+(int)(m_pCurPOpp->m_pPlaneWOpp[0]->m_SpanPos[nStart]*scalex),	O.y()+(int)y );

	for (int m=nStart; m<m_pCurPOpp->m_pPlaneWOpp[0]->m_NStation; m++)
	{
		offLE = pWing->getOffset(m_pCurPOpp->m_pPlaneWOpp[0]->m_SpanPos[m]*2.0/m_pCurPOpp->m_pPlaneWOpp[0]->m_Span);
		y = (offLE+m_pCurPOpp->m_pPlaneWOpp[0]->m_Chord[m]*m_pCurPOpp->m_pPlaneWOpp[0]->m_XCPSpanRel[m])*scaley;
		To = QPoint(O.x()+(int)(m_pCurPOpp->m_pPlaneWOpp[0]->m_SpanPos[m]*scalex),	O.y()+(int)y );
		painter.drawLine(From, To);
		From = To;
	}

	int x = (int)(m_r2DCltRect.width()/2);
	int y1 = m_r2DCltRect.bottom();
	painter.drawLine(x-60,  y1- 20, x-40,  y1 - 20);
	painter.drawText(x-35, y1 - 18, tr("Centre of Pressure"));
	painter.restore();
}


/**
 * Draws the position of the reference point for the moments in the operating view
 * @param painter a reference to the QPainter object on which the view shall be drawn
 * @param ORef the origin of the tip of the root chord, in client coordinates
 * @param scale the scaling factor with which to draw the wing
 */
void QMiarex::PaintXCmRef(QPainter & painter, QPoint ORef, double scale)
{
	//Draws the moment reference point on the 2D view
	if(!m_pCurPlane || !m_pCurWPolar)	return;

	painter.save();
	QPoint O(ORef);
	QPoint offset;

	double scaley;

	offset.rx() = ORef.x();
	offset.ry() = ORef.y();
//	scalex  = scale;
	scaley  = scale;
	O.rx() = offset.x();
	O.ry() = offset.y();

	QPen XCmRefPen(Settings::s_TextColor);
	painter.setPen(XCmRefPen);

	int XCm = O.x() ;
	int YCm = O.y() + (int)(m_pCurWPolar->m_CoG.x*scaley);
	int size = 3;
	QRect CM(XCm-size, YCm-size, 2*size, 2*size);
	painter.drawEllipse(CM);

	painter.drawText(XCm+10, YCm-5, tr("Moment ref. location"));

	painter.restore();
}


/**
 * Launches a 3D panel analysis
 * @param V0 the initial aoa
 * @param VMax the final aoa
 * @param VDelta the increment
 * @param bSequence if true, the analysis will run for a sequence of aoa from V0 to Vmax, if not only V0 shall be calculated
 */
void QMiarex::PanelAnalyze(double V0, double VMax, double VDelta, bool bSequence)
{
	if(!m_pCurPlane || !m_pCurWPolar) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	int i,pl, pr;

	//Join surfaces together
	pl = 0;
	pr = Objects3D::s_SurfaceList.at(0)->m_NElements;
	for (i=0; i<Objects3D::s_SurfaceList.size()-1; i++)
	{
		if(!Objects3D::s_SurfaceList.at(i)->m_bIsTipRight)
		{
			if(Objects3D::s_SurfaceList.at(i)->m_bJoinRight)
				Objects3D::JoinSurfaces(m_pCurWPolar, Objects3D::s_SurfaceList.at(i), Objects3D::s_SurfaceList.at(i+1), pl, pr);
		}
		pl  = pr;
		pr += Objects3D::s_SurfaceList.at(i+1)->m_NElements;
	}


	Objects3D::initPanelAnalysis(m_pCurPlane, m_pCurWPolar, V0, VMax, VDelta, bSequence);

	if(!m_pPanelAnalysisDlg->InitDialog()) return;

	m_pPanelAnalysisDlg->show();

	m_pPanelAnalysisDlg->Analyze();

	if(!m_bLogFile || !PanelAnalysis::s_bWarning)
		m_pPanelAnalysisDlg->hide();


	SetPlaneOpp(false, Objects3D::s_pPanelAnalysis->m_OpAlpha);
	pMainFrame->UpdatePOppListBox();

	m_bResetglWake=true; //TODO remove

    emit projectModified();
}




/**
 * Saves the user settings to the QSettings object
 * @param pSettings a pointer to the QSettings object
 * @return true if the save was successfull, false if an error was encountered
 */
bool QMiarex::SaveSettings(QSettings *pSettings)
{
	QString strong;
	int k=0;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;

	OnReadAnalysisData();

	pSettings->beginGroup("Miarex");
	{
		pSettings->setValue("bXTop", m_bXTop);
		pSettings->setValue("bXBot", m_bXBot);
		pSettings->setValue("bXCP", m_bXCP);
		pSettings->setValue("bPanelForce", m_bPanelForce);
		pSettings->setValue("bXCmRef", m_bXCmRef);
		pSettings->setValue("bICd", s_bICd);
		pSettings->setValue("bVCd", s_bVCd);
		pSettings->setValue("bSurfaces", s_bSurfaces);
		pSettings->setValue("bOutline", s_bOutline);
		pSettings->setValue("bVLMPanels", s_bVLMPanels);
		pSettings->setValue("bAxes", s_bAxes);
		pSettings->setValue("b3DCp", m_b3DCp);
		pSettings->setValue("bDownwash", m_bDownwash);
		pSettings->setValue("bMoments", m_bMoments);
		pSettings->setValue("bAutoCpScale", s_bAutoCpScale);
		pSettings->setValue("bShowCpScale", m_bShowCpScale);
		pSettings->setValue("CurWOppOnly", m_bCurPOppOnly);
		pSettings->setValue("bShowElliptic", m_bShowElliptic);
		pSettings->setValue("LogFile", m_bLogFile);
		pSettings->setValue("bVLM1", WPolarDlg::s_WPolar.m_bVLM1);
		pSettings->setValue("Dirichlet", m_bDirichlet);
		pSettings->setValue("KeepOutOpps", PlaneOpp::s_bKeepOutOpps);
		pSettings->setValue("ResetWake", m_bResetWake );
		pSettings->setValue("ShowWing", m_bShowWingCurve[0]);
		pSettings->setValue("ShowWing2", m_bShowWingCurve[1]);
		pSettings->setValue("ShowStab", m_bShowWingCurve[2]);
		pSettings->setValue("ShowFin", m_bShowWingCurve[3]);
		pSettings->setValue("StoreWOpp", PlaneOpp::s_bStoreOpps);
		pSettings->setValue("Sequence", m_bSequence );
		pSettings->setValue("HighlightOpp", m_bHighlightOpp);
		pSettings->setValue("AlphaMin", m_AlphaMin);
		pSettings->setValue("AlphaMax", m_AlphaMax);
		pSettings->setValue("AlphaDelta", m_AlphaDelta);
		pSettings->setValue("BetaMin", m_BetaMin);
		pSettings->setValue("BetaMax", m_BetaMax);
		pSettings->setValue("BetaDelta", m_BetaDelta);
		pSettings->setValue("QInfMin", m_QInfMin );
		pSettings->setValue("QInfMax", m_QInfMax );
		pSettings->setValue("QInfDelta", m_QInfDelta );
		pSettings->setValue("ControlMin", m_ControlMin);
		pSettings->setValue("ControlMax", m_ControlMax);
		pSettings->setValue("ControlDelta", m_ControlDelta);
		pSettings->setValue("bAutoInertia", WPolarDlg::s_WPolar.m_bAutoInertia);

		pSettings->setValue("CpStyle", m_CpStyle);
		pSettings->setValue("CpWidth", m_CpWidth);
		pSettings->setValue("CpColor", m_CpColor);

		pSettings->setValue("CvPrec", LLTAnalysis::s_CvPrec);
		pSettings->setValue("RelaxMax", LLTAnalysis::s_RelaxMax);
		pSettings->setValue("NLLTStations", LLTAnalysis::s_NLLTStations);

		pSettings->setValue("Trefftz", PanelAnalysis::s_bTrefftz);


		switch(m_iView)
		{
			case WOPPVIEW:
			{
				pSettings->setValue("iView", 0);
				break;
			}
			case WPOLARVIEW:
			{
				pSettings->setValue("iView", 1);
				break;
			}
			case W3DVIEW:
			{
				pSettings->setValue("iView", 2);
				break;
			}
			case WCPVIEW:
			{
				pSettings->setValue("iView", 3);
				break;
			}
			case STABTIMEVIEW:
			{
				pSettings->setValue("iView", 4);
				break;
			}
			case STABPOLARVIEW:
			{
				pSettings->setValue("iView", 5);
				break;
			}
		}

		switch(m_iWingView)
		{
			case ONEGRAPH:
				pSettings->setValue("iWingView", 1);
				break;
			case TWOGRAPHS:
				pSettings->setValue("iWingView", 2);
				break;
			default:
				pSettings->setValue("iWingView", 0);
				break;
		}


		switch(m_iWPlrView)
		{
			case ONEGRAPH:
				pSettings->setValue("iWPlrView", 1);
				break;
			case TWOGRAPHS:
				pSettings->setValue("iWPlrView", 2);
				break;
			default:
				pSettings->setValue("iWPlrView", 0);
				break;
		}


		switch(m_iStabTimeView)
		{
			case ONEGRAPH:
				pSettings->setValue("iStabTimeView", 1);
				break;
			case TWOGRAPHS:
				pSettings->setValue("iStabTimeView", 2);
				break;
			default:
				pSettings->setValue("iStabTimeView", 0);
				break;
		}


		if     (m_pCurWingGraph==m_WingGraph)   k=1;
		else if(m_pCurWingGraph==m_WingGraph+1) k=2;
		else if(m_pCurWingGraph==m_WingGraph+2) k=3;
		else if(m_pCurWingGraph==m_WingGraph+3) k=4;
		pSettings->setValue("CurrentWingGraph", k);

		if     (m_pCurWPlrGraph==m_WPlrGraph)   k=1;
		else if(m_pCurWPlrGraph==m_WPlrGraph+1) k=2;
		else if(m_pCurWPlrGraph==m_WPlrGraph+2) k=3;
		else if(m_pCurWPlrGraph==m_WPlrGraph+3) k=4;
		pSettings->setValue("CurWPlrGraph", k);

		if     (m_pCurRLStabGraph == &m_LongRLGraph) k=1;
		else if(m_pCurRLStabGraph == &m_LatRLGraph)  k=2;
		pSettings->setValue("CurRLStabGraph",k);

		if(m_iView==WOPPVIEW)        m_pCurGraph=m_pCurWingGraph;
		else if(m_iView==WPOLARVIEW) m_pCurGraph=m_pCurWPlrGraph;

		pSettings->setValue("Iter", m_LLTMaxIterations);
		pSettings->setValue("InducedDragPoint", m_InducedDragPoint);
		pSettings->setValue("NHoopPoints", GL3dBodyDlg::s_NHoopPoints);
		pSettings->setValue("NXPoints", GL3dBodyDlg::s_NXPoints);

		pSettings->setValue("LiftScale", s_LiftScale);
		pSettings->setValue("DragScale", s_DragScale);
		pSettings->setValue("VelocityScale", s_VelocityScale);

		pSettings->setValue("WakeInterNodes", m_WakeInterNodes);
		pSettings->setValue("MaxWakeIter", Objects3D::s_MaxWakeIter);
		pSettings->setValue("CtrlPos",   Panel::s_CtrlPos);
		pSettings->setValue("VortexPos", Panel::s_VortexPos);
		pSettings->setValue("CoreSize", Panel::s_CoreSize);
		pSettings->setValue("MinPanelSize", Wing::s_MinPanelSize);
		pSettings->setValue("TotalTime", m_TotalTime);
		pSettings->setValue("Delta_t", m_Deltat);
		pSettings->setValue("RampTime", m_RampTime);
		pSettings->setValue("RampAmplitude", m_RampAmplitude);
		pSettings->setValue("TimeIn0", m_TimeInput[0]);
		pSettings->setValue("TimeIn1", m_TimeInput[1]);
		pSettings->setValue("TimeIn2", m_TimeInput[2]);
		pSettings->setValue("TimeIn3", m_TimeInput[3]);
		pSettings->setValue("DynamicsMode", m_bLongitudinal);
		pSettings->setValue("StabCurveType",m_StabilityResponseType);


		if(m_pCurTimeGraph == m_TimeGraph)        k=1;
		else if(m_pCurTimeGraph == m_TimeGraph+1) k=2;
		else if(m_pCurTimeGraph == m_TimeGraph+2) k=3;
		else if(m_pCurTimeGraph == m_TimeGraph+3) k=4;
		pSettings->setValue("TimeGraph",k);

//		pSettings->setValue("AVLControls", StabPolarDlg::s_StabPolar.m_bAVLControls);

		pStabView->ReadControlModelData();
		for(int i=0; i<20; i++)
		{
			strong = QString("ForcedTime%1").arg(i);
			pSettings->setValue(strong, pStabView->m_Time[i]);
		}
		for(int i=0; i<20; i++)
		{
			strong = QString("ForcedAmplitude%1").arg(i);
			pSettings->setValue(strong, pStabView->m_Amplitude[i]);
		}

		pSettings->setValue("StabPolarAutoInertia", StabPolarDlg::s_StabPolar.m_bAutoInertia);
		pSettings->setValue("StabPolarMass",   StabPolarDlg::s_StabPolar.m_Mass);
		pSettings->setValue("StabPolarCoGx",   StabPolarDlg::s_StabPolar.m_CoG.x);
		pSettings->setValue("StabPolarCoGy",   StabPolarDlg::s_StabPolar.m_CoG.y);
		pSettings->setValue("StabPolarCoGz",   StabPolarDlg::s_StabPolar.m_CoG.z);
		pSettings->setValue("StabPolarCoGIxx", StabPolarDlg::s_StabPolar.m_CoGIxx);
		pSettings->setValue("StabPolarCoGIyy", StabPolarDlg::s_StabPolar.m_CoGIyy);
		pSettings->setValue("StabPolarCoGIzz", StabPolarDlg::s_StabPolar.m_CoGIzz);
		pSettings->setValue("StabPolarCoGIxz", StabPolarDlg::s_StabPolar.m_CoGIxz);
	}
	pSettings->endGroup();

	m_CpGraph.SaveSettings(pSettings);
	m_LongRLGraph.SaveSettings(pSettings);
	m_LatRLGraph.SaveSettings(pSettings);
	for(int ig=0; ig<MAXGRAPHS; ig++)
	{
		m_WPlrGraph[ig].SaveSettings(pSettings);			// the WPolar graphs
		m_WingGraph[ig].SaveSettings(pSettings);			// the WOpp graphs
		m_TimeGraph[ig].SaveSettings(pSettings);
	}

	GLLightDlg::SaveSettings(pSettings);
	GL3dBodyDlg::SaveSettings(pSettings);

	return true;
}


/**
 * Sets an automatic scale for the wing in the operating point view, depending on wing span.
 */
void QMiarex::Set2DScale()
{
	if(m_bIs2DScaleSet && !m_bAutoScales) return;

	int w,h;

	TwoDWidget *p2dWidget = (TwoDWidget*)s_p2dWidget;

	m_r2DCltRect = p2dWidget->geometry();
//	m_rSingleRect.setRect(m_r2DCltRect.x(), m_r2DCltRect.y(), m_r2DCltRect.width(), m_r2DCltRect.height());
	m_rSingleRect = m_r2DCltRect;

	h = m_r2DCltRect.height();
	w = m_r2DCltRect.width();
	int w2  = (int)(w/2);
//	int w3  = (int)(w/3);
//	int h23 = (int)(2*h/3);
	int h38 = (int)(3*h/8);
	int h34  = (int)(3*h/4);

	QRect CpRect(0,0,w,h34);
	m_CpGraph.SetDrawRect(CpRect);
//	m_TimeGraph.SetDrawRect(CpRect);
	m_LongRLGraph.SetDrawRect(CpRect);
	m_LatRLGraph.SetDrawRect(CpRect);

	if(m_bHalfWing) m_ptOffset.rx() = m_rSingleRect.left() + m_WingGraph[0].GetMargin();
	else            m_ptOffset.rx() = (int)(m_r2DCltRect.width()/2.0);
	m_ptOffset.ry() = m_r2DCltRect.bottom()-185;

	m_rSingleRect.setRect(40,10,m_r2DCltRect.right()-80,m_r2DCltRect.bottom()-230);

	QRect Rect1(0,0,w2,h38);
	QRect Rect2(w2,0,w2,h38);
	QRect Rect3(0,h38,w2,h38);
	QRect Rect4(w2,h38,w2,h38);
	m_WingGraph[0].SetDrawRect(Rect1);
	m_WingGraph[1].SetDrawRect(Rect2);
	m_WingGraph[2].SetDrawRect(Rect3);
	m_WingGraph[3].SetDrawRect(Rect4);

	for(int ig=0; ig<MAXGRAPHS; ig++)
	{
		m_WingGraph[ig].Init();
		m_WingGraph[ig].SetAutoXUnit();
	}

	if(m_iView==WOPPVIEW)           SetWingLegendPos();
	else if(m_iView==WPOLARVIEW)    SetWPlrLegendPos();
	else if(m_iView==STABTIMEVIEW)  SetWingLegendPos();
	else if(m_iView==STABPOLARVIEW) SetWPlrLegendPos();


	if(m_pCurPlane)
	{
		m_WingScale = (m_rSingleRect.width()-2*m_WingGraph[0].GetMargin())/m_pCurPlane->planformSpan();
		if(m_bHalfWing) 	m_WingScale *= 2.0;
		m_bIs2DScaleSet = true;
	}
}


/**
 * Adjusts the new rotation center after a translation or a rotation
 */
void QMiarex::Set3DRotationCenter()
{
	int i,j;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			MatOut[i][j] =  m_ArcBall.ab_quat[i*4+j];

	m_glRotCenter.x = MatOut[0][0]*(m_glViewportTrans.x) + MatOut[0][1]*(-m_glViewportTrans.y) + MatOut[0][2]*m_glViewportTrans.z;
	m_glRotCenter.y = MatOut[1][0]*(m_glViewportTrans.x) + MatOut[1][1]*(-m_glViewportTrans.y) + MatOut[1][2]*m_glViewportTrans.z;
	m_glRotCenter.z = MatOut[2][0]*(m_glViewportTrans.x) + MatOut[2][1]*(-m_glViewportTrans.y) + MatOut[2][2]*m_glViewportTrans.z;
}


/**
* Adjusts the new rotation center after the user has picked a point on the screen
* Finds the closest panel under the point and changes the rotation vector and viewport translation
*@param the point clicked by the user, in client coordinates
*/
void QMiarex::Set3DRotationCenter(QPoint point)
{
	int  i, j, p;
	CVector I, A, B, AA, BB, PP, U;
	double dmin, dist;
	ThreeDWidget *p3dWidget = (ThreeDWidget*)s_p3dWidget;


	i=-1;
	dmin = 100000.0;

	p3dWidget->ClientToGL(point, B);

	B.x += -m_PlaneOffset.x - m_glViewportTrans.x*m_glScaled;
	B.y += -m_PlaneOffset.y + m_glViewportTrans.y*m_glScaled;

	B *= 1.0/m_glScaled;

	A.Set(B.x, B.y, +1.0);
	B.z = -1.0;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			MatIn[i][j] =  m_ArcBall.ab_quat[i*4+j];

	//convert screen to model coordinates
	AA.x = MatIn[0][0]*A.x + MatIn[0][1]*A.y + MatIn[0][2]*A.z;
	AA.y = MatIn[1][0]*A.x + MatIn[1][1]*A.y + MatIn[1][2]*A.z;
	AA.z = MatIn[2][0]*A.x + MatIn[2][1]*A.y + MatIn[2][2]*A.z;

	BB.x = MatIn[0][0]*B.x + MatIn[0][1]*B.y + MatIn[0][2]*B.z;
	BB.y = MatIn[1][0]*B.x + MatIn[1][1]*B.y + MatIn[1][2]*B.z;
	BB.z = MatIn[2][0]*B.x + MatIn[2][1]*B.y + MatIn[2][2]*B.z;

	if(m_pCurPOpp)
	{
		AA.RotateY(-m_pCurPOpp->m_pPlaneWOpp[0]->m_Alpha);
		BB.RotateY(-m_pCurPOpp->m_pPlaneWOpp[0]->m_Alpha);
	}

	U.Set(BB.x-AA.x, BB.y-AA.y, BB.z-AA.z);
	U.Normalize();

	bool bIntersect = false;

	if(m_iView==W3DVIEW )
	{
		for(p=0; p<Objects3D::s_MatSize; p++)
		{
			if(s_pPanel[p].Intersect(AA, U, I, dist))
			{
				if(dist < dmin)
				{
					dmin = dist;
					PP.Set(I);
					bIntersect = true;
				}
			}
		}
	}

	if(bIntersect)
	{
		if(m_pCurPOpp)
		{
			PP.RotateY(m_pCurPOpp->m_pPlaneWOpp[0]->m_Alpha);
		}
//		instantaneous visual transition
//		m_glRotCenter -= PP * m_glScaled;

//		smooth visual transition
		GLInverseMatrix();

		int nSteps = 50;
		int millis = 5;

		U.x = (-PP.x -m_glRotCenter.x)/(double)nSteps;
		U.y = (-PP.y -m_glRotCenter.y)/(double)nSteps;
		U.z = (-PP.z -m_glRotCenter.z)/(double)nSteps;

		for(i=0; i<nSteps; i++)
		{
			m_glRotCenter += U;
			m_glViewportTrans.x =  (MatOut[0][0]*m_glRotCenter.x + MatOut[0][1]*m_glRotCenter.y + MatOut[0][2]*m_glRotCenter.z);
			m_glViewportTrans.y = -(MatOut[1][0]*m_glRotCenter.x + MatOut[1][1]*m_glRotCenter.y + MatOut[1][2]*m_glRotCenter.z);
			m_glViewportTrans.z=   (MatOut[2][0]*m_glRotCenter.x + MatOut[2][1]*m_glRotCenter.y + MatOut[2][2]*m_glRotCenter.z);

			UpdateView();

			//sleep

			#ifdef Q_OS_WIN
				Sleep(uint(millis));
			#else
				struct timespec ts = { millis / 1000, (millis % 1000) * 1000 * 1000 };
				nanosleep(&ts, NULL);
			#endif

			qApp->processEvents();
		}
	}
}


/**
 * Sets an automatic scale for the wing or plane in the 3D view, depending on wing span.
 */
void QMiarex::Set3DScale()
{
	if(m_iView!=W3DVIEW ) return;
	if(m_iView==W3DVIEW) m_bResetglLegend = true;
	if(m_bIs3DScaleSet && !m_bAutoScales) return;

	ThreeDWidget *p3dWidget = (ThreeDWidget*)s_p3dWidget;
	m_r3DCltRect = p3dWidget->geometry();

	if(m_pCurPlane)
	{
		//wing along X axis will take 3/4 of the screen
		m_glScaled = (GLfloat)(3./4.*2.0*m_GLScale/m_pCurPlane->planformSpan());

		m_glViewportTrans.x = 0.0;
		m_glViewportTrans.y = 0.0;
		m_glViewportTrans.z = 0.0;
		m_bIs3DScaleSet = true;
	}
	else //(m_iView==3)
	{
		double gh = (double)m_r3DCltRect.height()/(double)m_r3DCltRect.width() ;
		if(gh<1.0)
		{
			m_PlaneOffset.x = 0.0;
			m_PlaneOffset.y = (GLfloat)(gh - 1.0)*m_GLScale;
		}
		else
		{
			m_PlaneOffset.x = (GLfloat)(1.0/gh - 1.0)*m_GLScale;
			m_PlaneOffset.y = 0.0;
		}
		m_PlaneOffset.x = 0.0;
		m_PlaneOffset.y = 0.0;
	}
}


/**
 * Initializes the input parameters depending onthe type of the active polar
 */
void QMiarex::SetAnalysisParams()
{
	m_pctrlSequence->setChecked(m_bSequence);

	m_pctrlAlphaMax->setEnabled(m_bSequence);
	m_pctrlAlphaDelta->setEnabled(m_bSequence);

	if (!m_pCurWPolar)
	{
		m_pctrlSequence->setEnabled(false);
		m_pctrlAlphaMin->setEnabled(false);
		m_pctrlAlphaMax->setEnabled(false);
		m_pctrlAlphaDelta->setEnabled(false);
		m_pctrlInitLLTCalc->setEnabled(false);
		m_pctrlStoreWOpp->setEnabled(false);
		return;
	}
	else
	{
		m_pctrlSequence->setEnabled(true);
		m_pctrlAlphaMin->setEnabled(true);

		m_pctrlAlphaMax->setEnabled(m_bSequence);
		m_pctrlAlphaDelta->setEnabled(m_bSequence);

		m_pctrlInitLLTCalc->setEnabled(true);
		m_pctrlStoreWOpp->setEnabled(true);
	}

	m_pctrlInitLLTCalc->setChecked(m_bInitLLTCalc);
	m_pctrlStoreWOpp->setChecked(PlaneOpp::s_bStoreOpps);

	if (!m_pCurWPolar || (m_pCurWPolar && m_pCurWPolar->polarType() < FIXEDAOAPOLAR))
	{
		m_pctrlAlphaMin->SetValue(m_AlphaMin);
		m_pctrlAlphaMax->SetValue(m_AlphaMax);
		m_pctrlAlphaDelta->SetValue(m_AlphaDelta);
	}
	else if(m_pCurWPolar  && m_pCurWPolar->polarType() == FIXEDAOAPOLAR)
	{
		m_pctrlAlphaMin->SetValue(m_QInfMin*Units::mstoUnit());
		m_pctrlAlphaMax->SetValue(m_QInfMax*Units::mstoUnit());
		m_pctrlAlphaDelta->SetValue(m_QInfDelta*Units::mstoUnit());
	}
	else if (m_pCurWPolar || (m_pCurWPolar && m_pCurWPolar->polarType() == BETAPOLAR))
	{
		m_pctrlAlphaMin->SetValue(m_BetaMin);
		m_pctrlAlphaMax->SetValue(m_BetaMax);
		m_pctrlAlphaDelta->SetValue(m_BetaDelta);
	}
	else if(m_pCurWPolar && (m_pCurWPolar->polarType() == STABILITYPOLAR))
	{
		m_pctrlAlphaMin->SetValue(m_ControlMin);
		m_pctrlAlphaMax->SetValue(m_ControlMax);
		m_pctrlAlphaDelta->SetValue(m_ControlDelta);
	}
}


/**
 * Initializes the input parameters in the style dialog boxes depending on the type of view and on the active polar or operating point
 */
void QMiarex::SetCurveParams()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(m_iView==WPOLARVIEW || m_iView==STABPOLARVIEW)
	{
		if(m_pCurWPolar)
		{
			m_pctrlShowCurve->setChecked(m_pCurWPolar->m_bIsVisible);
			m_pctrlShowPoints->setChecked(m_pCurWPolar->m_bShowPoints);

			m_CurveColor = m_pCurWPolar->m_Color;
			m_CurveStyle = m_pCurWPolar->m_Style;
			m_CurveWidth = m_pCurWPolar->m_Width;
			FillComboBoxes();
		}
		else
		{
			FillComboBoxes(false);
		}
	}
	else if(m_iView==STABTIMEVIEW)
	{
		StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
		if(pStabView->m_pCurve)
		{
			m_CurveColor = pStabView->m_pCurve->color();
			m_CurveStyle = pStabView->m_pCurve->style();
			m_CurveWidth = pStabView->m_pCurve->width();
			m_pctrlShowCurve->setChecked(pStabView->m_pCurve->IsVisible());
			m_pctrlShowPoints->setChecked(pStabView->m_pCurve->PointsVisible());
			FillComboBoxes();
		}		
	}
	else if(m_iView==WOPPVIEW)
	{
		//set OpPoint params
		if(m_pCurPOpp)
		{
			m_pctrlShowCurve->setChecked(m_pCurPOpp->m_bIsVisible);
			m_pctrlShowPoints->setChecked(m_pCurPOpp->m_bShowPoints);

			m_CurveColor = m_pCurPOpp->m_Color;
			m_CurveStyle = m_pCurPOpp->m_Style;
			m_CurveWidth = m_pCurPOpp->m_Width;
			FillComboBoxes();
		}
		else
		{
			FillComboBoxes(false);
		}
	}
	else if(m_iView==WCPVIEW)
	{
		//set Cp params
		if(m_pCurPOpp)
		{
			m_pctrlShowCurve->setChecked(true);
			m_pctrlShowPoints->setChecked(m_bShowCpPoints);

			m_CurveColor = m_CpColor;
			m_CurveStyle = m_CpStyle;
			m_CurveWidth = m_CpWidth;
			FillComboBoxes();
		}
		else
		{
			FillComboBoxes(false);
		}
	}
	else
	{
		FillComboBoxes(false);
	}


	if(m_pCurWPolar && m_pCurWPolar->polarType()==FIXEDAOAPOLAR)
	{
		QString str;
		Units::getSpeedUnitLabel(str);
		m_pctrlUnit1->setText(str);
		m_pctrlUnit2->setText(str);
		m_pctrlUnit3->setText(str);

		m_pctrlParameterName->setText("Freestream velocity");
		QFont fontSymbol(Settings::s_TextFont);
		fontSymbol.setBold(true);
		fontSymbol.setPointSize(Settings::s_TextFont.pointSize()+2);
		m_pctrlParameterName->setFont(fontSymbol);
	}
	else if(m_pCurWPolar && m_pCurWPolar->polarType()==STABILITYPOLAR)
	{
		m_pctrlUnit1->setText("");
		m_pctrlUnit2->setText("");
		m_pctrlUnit3->setText("");

		m_pctrlParameterName->setText("Control parameter");
		QFont fontSymbol(Settings::s_TextFont);
		fontSymbol.setBold(true);
		m_pctrlParameterName->setFont(fontSymbol);
	}
	else if(m_pCurWPolar && m_pCurWPolar->polarType()==BETAPOLAR)
	{
		m_pctrlUnit1->setText(QString::fromUtf8("°"));
		m_pctrlUnit2->setText(QString::fromUtf8("°"));
		m_pctrlUnit3->setText(QString::fromUtf8("°"));

		m_pctrlParameterName->setText("b");
		QFont fontSymbol("Symbol");
		fontSymbol.setBold(true);
		m_pctrlParameterName->setFont(fontSymbol);
	}
	else
	{
		m_pctrlUnit1->setText(QString::fromUtf8("°"));
		m_pctrlUnit2->setText(QString::fromUtf8("°"));
		m_pctrlUnit3->setText(QString::fromUtf8("°"));

		m_pctrlParameterName->setText("a");
		QFont fontSymbol("Symbol");
		fontSymbol.setBold(true);
		m_pctrlParameterName->setFont(fontSymbol);
	}
}



/**
 * Sets the scale for the 2d or 3d selected view
 */
void QMiarex::SetScale()
{
	if(m_iView==W3DVIEW) Set3DScale();
	else                 Set2DScale();
}


/**
 * Sets the active plane
 * Sets an active polar and operating point for this plane, if any are available
 * @param PlaneName the name of the plane to be set as active
 */
void QMiarex::SetPlane(QString PlaneName)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	//set the plane and initialize the pointers
	m_pCurPlane = Objects3D::setPlaneObject(PlaneName, m_pCurPlane);

	if(!m_pCurPlane)
	{
		// no plane,
		//clear the pointers
		for (int iw=0; iw<MAXWINGS; iw++)
		{
			m_pWOpp[iw]     = NULL;
			m_pWingList[iw] = NULL;
		}
		//clear the GUI
		for(int i=0; i<MAXWINGS; i++) m_pWingList[i] = NULL;
		m_pCurWPolar = NULL;
		m_pCurPOpp  = NULL;
		s_bResetCurves = true;
		UpdateView();
		QApplication::restoreOverrideCursor();
		return;
	}

	// we have a plane, initialize the pointers and the  GUI

	m_pWingList[0] = m_pCurPlane->wing();
	m_pWingList[1] = m_pCurPlane->wing2();
	m_pWingList[2] = m_pCurPlane->stab();
	if(m_pCurPlane->fin())
	{
		m_pWingList[3] = m_pCurPlane->fin();
		m_pWingList[3]->m_bDoubleSymFin = m_pCurPlane->m_bDoubleSymFin;
	}
	else
		m_pWingList[3]  = NULL;


	m_bResetglGeom   = true;
	m_bResetglMesh = true;

	if(m_pCurPlane->body()) m_bResetglBody   = true;
	else                    m_bResetglBody   = false;


	MainFrame*pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SelectPlane(m_pCurPlane);
	pMainFrame->UpdateWPolarListBox();

	if (m_pCurWPolar)
	{
		// try to set the same as the existing polar
		SetWPolar(false, m_pCurWPolar->polarName());
	}
	else
	{
		SetWPolar();
	}

	SetScale();
	SetWGraphScale();
	
	QApplication::restoreOverrideCursor();
}


/**
 * Constructs the layout of the QMiarex widget
 */
void QMiarex::SetupLayout()
{
	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::Expanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);

	QSizePolicy szPolicyMinimum;
	szPolicyMinimum.setHorizontalPolicy(QSizePolicy::Minimum);
	szPolicyMinimum.setVerticalPolicy(QSizePolicy::Minimum);

	QSizePolicy szPolicyMaximum;
	szPolicyMaximum.setHorizontalPolicy(QSizePolicy::Maximum);
	szPolicyMaximum.setVerticalPolicy(QSizePolicy::Maximum);

	QSizePolicy szPolicyPreferred;
	szPolicyPreferred.setHorizontalPolicy(QSizePolicy::Preferred);
	szPolicyPreferred.setVerticalPolicy(QSizePolicy::Preferred);

//_______________________Analysis

	QGroupBox *pAnalysisBox = new QGroupBox(tr("Analysis settings"));
	{
		QVBoxLayout *pAnalysisGroupLayout = new QVBoxLayout;
		{
			m_pctrlSequence = new QCheckBox(tr("Sequence"));
			QGridLayout *pSequenceGroupLayout = new QGridLayout;
			{
				m_pctrlParameterName = new QLabel("Alpha");
				QLabel *AlphaMinLab   = new QLabel(tr("Start="));
				QLabel *AlphaMaxLab   = new QLabel(tr("End="));
				QLabel *AlphaDeltaLab = new QLabel(tr("D="));
				AlphaDeltaLab->setFont(QFont("Symbol"));
				AlphaDeltaLab->setAlignment(Qt::AlignRight);
				AlphaMinLab->setAlignment(Qt::AlignRight);
				AlphaMaxLab->setAlignment(Qt::AlignRight);
                m_pctrlAlphaMin     = new DoubleEdit(0.0, 3);
                m_pctrlAlphaMax     = new DoubleEdit(1., 3);
                m_pctrlAlphaDelta   = new DoubleEdit(0.5, 3);

				m_pctrlUnit1 = new QLabel(QString::fromUtf8("°"));
				m_pctrlUnit2 = new QLabel(QString::fromUtf8("°"));
				m_pctrlUnit3 = new QLabel(QString::fromUtf8("°"));

				m_pctrlAlphaMin->setAlignment(Qt::AlignRight);
				m_pctrlAlphaMax->setAlignment(Qt::AlignRight);
				m_pctrlAlphaDelta->setAlignment(Qt::AlignRight);
				pSequenceGroupLayout->addWidget(m_pctrlParameterName,1,1,1,3, Qt::AlignVCenter|Qt::AlignCenter);
				pSequenceGroupLayout->addWidget(AlphaMinLab,2,1);
				pSequenceGroupLayout->addWidget(AlphaMaxLab,3,1);
				pSequenceGroupLayout->addWidget(AlphaDeltaLab,4,1);
				pSequenceGroupLayout->addWidget(m_pctrlAlphaMin,2,2);
				pSequenceGroupLayout->addWidget(m_pctrlAlphaMax,3,2);
				pSequenceGroupLayout->addWidget(m_pctrlAlphaDelta,4,2);
				pSequenceGroupLayout->addWidget(m_pctrlUnit1,2,3);
				pSequenceGroupLayout->addWidget(m_pctrlUnit2,3,3);
				pSequenceGroupLayout->addWidget(m_pctrlUnit3,4,3);
			}
			QHBoxLayout *pAnalysisSettingsLayout = new QHBoxLayout;
			{
				m_pctrlInitLLTCalc = new QCheckBox(tr("Init LLT"));
				m_pctrlStoreWOpp    = new QCheckBox(tr("Store OpPoint"));
				pAnalysisSettingsLayout->addWidget(m_pctrlInitLLTCalc);
				pAnalysisSettingsLayout->addWidget(m_pctrlStoreWOpp);
			}

			m_pctrlAnalyze     = new QPushButton(tr("Analyze"));

			pAnalysisGroupLayout->addWidget(m_pctrlSequence);
			pAnalysisGroupLayout->addLayout(pSequenceGroupLayout);
			pAnalysisGroupLayout->addStretch(1);
			pAnalysisGroupLayout->addLayout(pAnalysisSettingsLayout);
			pAnalysisGroupLayout->addWidget(m_pctrlAnalyze);
		}

		pAnalysisBox->setLayout(pAnalysisGroupLayout);
	}

//_______________________Display
	QGroupBox *pDisplayBox = new QGroupBox(tr("Results"));
	{
		QGridLayout *pCheckDispLayout = new QGridLayout;
		{
			m_pctrlPanelForce = new QCheckBox(tr("Panel Forces"));
			m_pctrlPanelForce->setToolTip(tr("Display the force 1/2.rho.V2.S.Cp acting on the panel"));
			m_pctrlLift           = new QCheckBox(tr("Lift"));
			m_pctrlIDrag          = new QCheckBox(tr("Ind. Drag"));
			m_pctrlVDrag          = new QCheckBox(tr("Visc. Drag"));
			m_pctrlTrans          = new QCheckBox(tr("Trans."));
			m_pctrlMoment         = new QCheckBox(tr("Moment"));
			m_pctrlDownwash       = new QCheckBox(tr("Downw."));
			m_pctrlCp             = new QCheckBox(tr("Cp"));
			m_pctrlSurfVel        = new QCheckBox(tr("Surf. Vel."));
			m_pctrlStream         = new QCheckBox(tr("Stream"));
			m_pctrlWOppAnimate    = new QCheckBox(tr("Animate"));

			m_pctrlAnimateWOppSpeed  = new QSlider(Qt::Horizontal);
			m_pctrlAnimateWOppSpeed->setSizePolicy(szPolicyMinimum);
			m_pctrlAnimateWOppSpeed->setMinimum(0);
			m_pctrlAnimateWOppSpeed->setMaximum(500);
			m_pctrlAnimateWOppSpeed->setSliderPosition(250);
			m_pctrlAnimateWOppSpeed->setTickInterval(50);
			m_pctrlAnimateWOppSpeed->setTickPosition(QSlider::TicksBelow);
			pCheckDispLayout->addWidget(m_pctrlCp,       1,1);
			pCheckDispLayout->addWidget(m_pctrlPanelForce, 1, 2);
			pCheckDispLayout->addWidget(m_pctrlLift,     2, 1);
			pCheckDispLayout->addWidget(m_pctrlMoment,   2, 2);
			pCheckDispLayout->addWidget(m_pctrlIDrag,    3, 1);
			pCheckDispLayout->addWidget(m_pctrlVDrag,    3, 2);
			pCheckDispLayout->addWidget(m_pctrlTrans,    4, 1);
			pCheckDispLayout->addWidget(m_pctrlDownwash, 4, 2);
			pCheckDispLayout->addWidget(m_pctrlSurfVel,  5, 1);
			pCheckDispLayout->addWidget(m_pctrlStream,   5, 2);
			pCheckDispLayout->addWidget(m_pctrlWOppAnimate,  6, 1);
			pCheckDispLayout->addWidget(m_pctrlAnimateWOppSpeed,6,2);
			pCheckDispLayout->setRowStretch(7,1);
		}
		pDisplayBox->setLayout(pCheckDispLayout);
	}

	QGroupBox *pPolarPropsBox = new QGroupBox(tr("Polar properties"));
	{
		m_pctrlPolarProps = new MinTextEdit(this);
		QHBoxLayout *pPolarPropsLayout = new QHBoxLayout;
		{
			pPolarPropsLayout->addWidget(m_pctrlPolarProps);
		}
		pPolarPropsBox->setLayout(pPolarPropsLayout);
	}

//_______________________Curve params
	QGroupBox *pCurveBox = new QGroupBox(tr("Curve settings"));
	{
		QVBoxLayout *CurveGroupLayout = new QVBoxLayout;
		{
			m_pctrlShowCurve  = new QCheckBox(tr("Curve"));
			m_pctrlShowPoints = new QCheckBox(tr("Points"));
			m_pctrlCurveStyle = new LineCbBox(this);
			m_pctrlCurveWidth = new LineCbBox(this);
			m_pctrlCurveColor = new LineBtn();

            for (int i=0; i<5; i++)
			{
				m_pctrlCurveStyle->addItem(tr("item"));
				m_pctrlCurveWidth->addItem(tr("item"));
			}
			m_pStyleDelegate = new LineDelegate(this);
			m_pWidthDelegate = new LineDelegate(this);
			m_pctrlCurveStyle->setItemDelegate(m_pStyleDelegate);
			m_pctrlCurveWidth->setItemDelegate(m_pWidthDelegate);

			QHBoxLayout *ShowCurve = new QHBoxLayout;
			{
				ShowCurve->addWidget(m_pctrlShowCurve);
				ShowCurve->addWidget(m_pctrlShowPoints);
			}

			QGridLayout *CurveStyleLayout = new QGridLayout;
			{
				QLabel *lab200 = new QLabel(tr("Style"));
				QLabel *lab201 = new QLabel(tr("Width"));
				QLabel *lab202 = new QLabel(tr("Color"));
				lab200->setAlignment(Qt::AlignRight |Qt::AlignVCenter);
				lab201->setAlignment(Qt::AlignRight |Qt::AlignVCenter);
				lab202->setAlignment(Qt::AlignRight |Qt::AlignVCenter);
				CurveStyleLayout->addWidget(lab200,1,1);
				CurveStyleLayout->addWidget(lab201,2,1);
				CurveStyleLayout->addWidget(lab202,3,1);
				CurveStyleLayout->addWidget(m_pctrlCurveStyle,1,2);
				CurveStyleLayout->addWidget(m_pctrlCurveWidth,2,2);
				CurveStyleLayout->addWidget(m_pctrlCurveColor,3,2);
				CurveStyleLayout->setColumnStretch(2,5);
			}

			CurveGroupLayout->addLayout(ShowCurve);
			CurveGroupLayout->addLayout(CurveStyleLayout);
			CurveGroupLayout->addStretch(1);
			SetCurveParams();
		}
		pCurveBox->setLayout(CurveGroupLayout);
	}
//_______________________Cp Params
	QGroupBox *pCpBox = new QGroupBox(tr("Cp Sections"));
	{
		QVBoxLayout *CpParams = new QVBoxLayout;
		{
			m_pctrlCpSectionSlider = new QSlider(Qt::Horizontal);
			m_pctrlCpSectionSlider->setSizePolicy(szPolicyMinimum);
			m_pctrlCpSectionSlider->setMinimum(-100);
			m_pctrlCpSectionSlider->setMaximum(100);
			m_pctrlCpSectionSlider->setSliderPosition(00);
			m_pctrlCpSectionSlider->setTickInterval(10);
			m_pctrlCpSectionSlider->setTickPosition(QSlider::TicksBelow);
			QHBoxLayout *CpPos = new QHBoxLayout;
			{
				QLabel *label1000 = new QLabel(tr("Span Position"));
                m_pctrlSpanPos = new DoubleEdit(0.0, 3);
				CpPos->addWidget(label1000);
				CpPos->addWidget(m_pctrlSpanPos);
			}
			QHBoxLayout *CpSections = new QHBoxLayout;
			{
				m_pctrlKeepCpSection  = new QPushButton(tr("Keep"));
				m_pctrlResetCpSection = new QPushButton(tr("Reset"));
				CpSections->addWidget(m_pctrlKeepCpSection);
				CpSections->addWidget(m_pctrlResetCpSection);
			}
			CpParams->addWidget(m_pctrlCpSectionSlider);
			CpParams->addLayout(CpPos);
			CpParams->addLayout(CpSections);
			CpParams->addStretch(1);
		}
		pCpBox->setLayout(CpParams);
	}


//_______________________3D view controls
	QGroupBox *pThreeDViewBox = new QGroupBox(tr("Display"));
	{
		QVBoxLayout *pThreeDViewControlsLayout = new QVBoxLayout;
		{
			QGridLayout *pThreeDParamsLayout = new QGridLayout;
			{
				m_pctrlAxes       = new QCheckBox(tr("Axes"), this);
//				m_pctrlLight      = new QCheckBox(tr("Light"), this);
				m_pctrlSurfaces   = new QCheckBox(tr("Surfaces"), this);
				m_pctrlOutline    = new QCheckBox(tr("Outline"), this);
				m_pctrlPanels     = new QCheckBox(tr("Panels"), this);
				m_pctrlFoilNames  = new QCheckBox(tr("Foil Names"), this);
//				m_pctrlVortices   = new QCheckBox(tr("Vortices"), this);
				m_pctrlMasses     = new QCheckBox(tr("Masses"), this);

				pThreeDParamsLayout->addWidget(m_pctrlAxes, 1,1);
				pThreeDParamsLayout->addWidget(m_pctrlPanels, 1,2);
				pThreeDParamsLayout->addWidget(m_pctrlSurfaces, 2,1);
				pThreeDParamsLayout->addWidget(m_pctrlOutline, 2,2);
				pThreeDParamsLayout->addWidget(m_pctrlFoilNames, 3,1);
				pThreeDParamsLayout->addWidget(m_pctrlMasses, 3,2);
			}

			QVBoxLayout *ThreeDView = new QVBoxLayout;
			{
				QHBoxLayout *AxisViewLayout = new QHBoxLayout;
				{
					m_pctrlX          = new QToolButton;
					m_pctrlY          = new QToolButton;
					m_pctrlZ          = new QToolButton;
					m_pctrlIso        = new QToolButton;
					if(m_pctrlX->iconSize().height()<=48)
					{
                        m_pctrlX->setIconSize(QSize(24,24));
                        m_pctrlY->setIconSize(QSize(24,24));
                        m_pctrlZ->setIconSize(QSize(24,24));
                        m_pctrlIso->setIconSize(QSize(24,24));
					}
					m_pXView   = new QAction(QIcon(":/images/OnXView.png"), tr("X View"), this);
					m_pYView   = new QAction(QIcon(":/images/OnYView.png"), tr("Y View"), this);
					m_pZView   = new QAction(QIcon(":/images/OnZView.png"), tr("Z View"), this);
					m_pIsoView = new QAction(QIcon(":/images/OnIsoView.png"), tr("Iso View"), this);
					m_pXView->setCheckable(true);
					m_pYView->setCheckable(true);
					m_pZView->setCheckable(true);
					m_pIsoView->setCheckable(true);

					m_pctrlX->setDefaultAction(m_pXView);
					m_pctrlY->setDefaultAction(m_pYView);
					m_pctrlZ->setDefaultAction(m_pZView);
					m_pctrlIso->setDefaultAction(m_pIsoView);
					AxisViewLayout->addWidget(m_pctrlX);
					AxisViewLayout->addWidget(m_pctrlY);
					AxisViewLayout->addWidget(m_pctrlZ);
					AxisViewLayout->addWidget(m_pctrlIso);
				}


				QHBoxLayout *ViewResetLayout = new QHBoxLayout;
				{
					m_pctrlPickCenter     = new QPushButton(tr("Pick Center"));
					m_pctrlPickCenter->setToolTip(tr("Activate the button, then click on the object to center it in the viewport; alternatively, double click on the object"));
					m_pctrlReset          = new QPushButton(tr("Reset"));
					m_pctrlPickCenter->setCheckable(true);

					ViewResetLayout->addWidget(m_pctrlReset);
					ViewResetLayout->addWidget(m_pctrlPickCenter);
				}
				ThreeDView->addLayout(AxisViewLayout);
				ThreeDView->addLayout(ViewResetLayout);
			}

			QHBoxLayout *ClipLayout = new QHBoxLayout;
			{
				QLabel *ClipLabel = new QLabel(tr("Clip:"));
				m_pctrlClipPlanePos = new QSlider(Qt::Horizontal);
				m_pctrlClipPlanePos->setSizePolicy(szPolicyMinimum);
				m_pctrlClipPlanePos->setMinimum(-300);
				m_pctrlClipPlanePos->setMaximum(300);
				m_pctrlClipPlanePos->setSliderPosition(0);
				m_pctrlClipPlanePos->setTickInterval(30);
				m_pctrlClipPlanePos->setTickPosition(QSlider::TicksBelow);
				ClipLayout->addWidget(ClipLabel);
				ClipLayout->addWidget(m_pctrlClipPlanePos,1);
			}
			pThreeDViewControlsLayout->addLayout(pThreeDParamsLayout);
			pThreeDViewControlsLayout->addStretch(1);
			pThreeDViewControlsLayout->addLayout(ThreeDView);
			pThreeDViewControlsLayout->addLayout(ClipLayout);
			pThreeDViewControlsLayout->addStretch(1);

		}
		pThreeDViewBox->setLayout(pThreeDViewControlsLayout);
	}

//_________________________Main Layout
	QVBoxLayout *mainLayout = new QVBoxLayout;
	{
		m_pctrlMiddleControls = new QStackedWidget;
		m_pctrlMiddleControls->addWidget(pDisplayBox);
		m_pctrlMiddleControls->addWidget(pPolarPropsBox);
		m_pctrlMiddleControls->addWidget(pCpBox);

		m_pctrBottomControls = new QStackedWidget;
		m_pctrBottomControls->addWidget(pCurveBox);
		m_pctrBottomControls->addWidget(pThreeDViewBox);

		mainLayout->addWidget(pAnalysisBox);
		mainLayout->addStretch();
		mainLayout->addWidget(m_pctrlMiddleControls);
		mainLayout->addStretch();
		mainLayout->addWidget(m_pctrBottomControls);
	}
	setLayout(mainLayout);
}



/**
 * Sets the scale of the graphs in the operating point view.
 *The scale is set i.a.w. with wing span, if any
 */
void QMiarex::SetWGraphScale()
{
	if(!m_pCurPlane)
	{
		for(int ig=0; ig<MAXGRAPHS; ig++)
		{
			m_WingGraph[ig].SetAuto(false);
			m_WingGraph[ig].SetXUnit(10.0);
			m_WingGraph[ig].SetXMin(-100.0);
			m_WingGraph[ig].SetXMax( 100.0);
		}
	}
	else
	{
		double halfspan = m_pCurPlane->planformSpan()/2.0;
		double xmin;
		if(m_bHalfWing) xmin = 0.0;
		else            xmin = -halfspan*Units::mtoUnit();

		for(int ig=0; ig<MAXGRAPHS; ig++)
		{
			m_WingGraph[ig].SetAutoX(false);
			m_WingGraph[ig].SetXMin(xmin);
			m_WingGraph[ig].SetXMax( halfspan*Units::mtoUnit());
			m_WingGraph[ig].SetAutoXUnit();
		}
	}
}

/**
 * Sets the active polar
 * Builds the array of panels depending on the polar type
 * @param bCurrent if true, the active polar is set anew
 * @param WPlrName the name of the polar to set for the active wing or plane
 */
void QMiarex::SetWPolar(bool bCurrent, QString WPlrName)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	WPolar *pWPolar = NULL;
	QString PlaneName;

	if(m_pCurPlane)     PlaneName = m_pCurPlane->planeName();
	else return;

	pWPolar = Objects3D::setWPolarObject(m_pCurPlane, m_pCurWPolar, bCurrent, WPlrName);
	m_pCurWPolar = pWPolar;

	if(m_pCurWPolar)
	{
		pMainFrame->SelectWPolar(m_pCurWPolar);
	}

/** @todo restore ?
	int i,j,k,m, NStation;
	double SpanPos;

	if(m_pCurWPolar->m_AnalysisMethod>LLTMETHOD)
	{
		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(s_pWingList[iw])
			{
				s_pWingList[iw]->ComputeChords();

				NStation = 0;
				m=0;
				SpanPos = 0;
				for (j=0; j<s_pWingList[iw]->m_Surface.size(); j++)	NStation += s_pWingList[iw]->m_Surface.at(j)->m_NYPanels;

				for (j=(int)(s_pWingList[iw]->m_Surface.size()/2); j<s_pWingList[iw]->m_Surface.size(); j++)
				{
					for(k=0; k<s_pWingList[iw]->m_Surface.at(j)->m_NYPanels; k++)
					{
						s_pWingList[iw]->m_SpanPos[m+NStation/2] = SpanPos + s_pWingList[iw]->m_Surface.at(j)->GetStripSpanPos(k);
						m++;
					}
					SpanPos += s_pWingList[iw]->m_Surface.at(j)->m_Length;
				}

				for(m=0; m<NStation/2; m++) s_pWingList[iw]->m_SpanPos[m] = -s_pWingList[iw]->m_SpanPos[NStation-m-1];
			}
		}
	}*/

	pMainFrame->UpdatePOppListBox();

	double x = 0.0;
	if(m_pCurPOpp)
	{
		if(m_pCurWPolar && !m_pCurWPolar->isBetaPolar())
			x = m_pCurPOpp->m_Alpha;
		else if(m_pCurWPolar && m_pCurWPolar->isBetaPolar())
			x = m_pCurPOpp->m_Beta;
	}

	SetPlaneOpp(false, x);


	if(m_pCurWPolar)
	{
		m_bCurveVisible = m_pCurWPolar->m_bIsVisible;
		m_bCurvePoints  = m_pCurWPolar->m_bShowPoints;

		if(m_pCurWPolar->m_bAutoInertia)
		{
			if(m_pCurPlane)
			{
				m_pCurWPolar->m_Mass = m_pCurPlane->TotalMass();
				m_pCurWPolar->m_CoG    = m_pCurPlane->CoG();
				m_pCurWPolar->m_CoGIxx = m_pCurPlane->m_CoGIxx;
				m_pCurWPolar->m_CoGIyy = m_pCurPlane->m_CoGIyy;
				m_pCurWPolar->m_CoGIzz = m_pCurPlane->m_CoGIzz;
				m_pCurWPolar->m_CoGIxz = m_pCurPlane->m_CoGIxz;
			}

			QString PolarProps;
			m_pCurWPolar->GetPolarProperties(PolarProps);
			m_pctrlPolarProps->setText(PolarProps);
		}
	}
	else m_pctrlPolarProps->clear();


	SetAnalysisParams();
	SetCurveParams();
	m_bResetglLegend = true;
	s_bResetCurves = true;


	if(m_pCurWPolar && m_pCurWPolar->polarType()==STABILITYPOLAR)
	{
		StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
		pStabView->SetControls();
		pStabView->SetMode();
		pMainFrame->m_pctrlStabViewWidget->show();
		pStabView->show();
	}

	QApplication::restoreOverrideCursor();
}



/**
 * Sets the position for the legend in the operating point view
 */
void QMiarex::SetWingLegendPos()
{
	int h  = m_r2DCltRect.height();
	int h23 = (int)(2*h/3);
	int h34 = (int)(3*h/4);

	int margin = 10;
	if(m_iView==WOPPVIEW)
	{
		if(m_iWingView == ONEGRAPH)
		{
			m_WingLegendOffset.rx() = m_r2DCltRect.right()-300+margin;
			m_WingLegendOffset.ry() = 0;
		}
		else if(m_iWingView == TWOGRAPHS)
		{
			m_WingLegendOffset.rx() = margin;
			m_WingLegendOffset.ry() = h23+margin;
		}
		else
		{
			m_WingLegendOffset.rx() = margin;
			m_WingLegendOffset.ry() = h34+margin;
		}
	}
	else if(m_iView==STABTIMEVIEW)
	{
		if(m_iStabTimeView < ALLGRAPHS)
		{
			m_WingLegendOffset.rx() = margin;
			m_WingLegendOffset.ry() = h34+margin;
		}
		else if(m_iStabTimeView == ALLGRAPHS)
		{
			m_WingLegendOffset.rx() = margin;
			m_WingLegendOffset.ry() = h34+margin;
		}		
	}
}


/**
 * Sets the position for the legend in the polar view
 */
void QMiarex::SetWPlrLegendPos()
{
	int h  = m_r2DCltRect.height();
	int w  = m_r2DCltRect.width();
	int h23 = (int)(2*h/3);

	int margin = 10;
	if(m_iView==WPOLARVIEW)
	{
		if(m_iWPlrView == ONEGRAPH)
		{
			m_WPlrLegendOffset.rx() = (int)(0.70*w)+margin;
			m_WPlrLegendOffset.ry() = margin;
		}
		else if (m_iWPlrView == TWOGRAPHS)
		{
			m_WPlrLegendOffset.rx() = margin;
			m_WPlrLegendOffset.ry() = h23+margin;
		}
		else if	(m_iWPlrView == ALLGRAPHS)
		{
			m_WPlrLegendOffset.rx() = (int)(0.70*w)+margin;
			m_WPlrLegendOffset.ry() = margin;
		}
	}
	else if(m_iView==STABPOLARVIEW)
	{
		m_WPlrLegendOffset.rx() = (int)(0.70*w)+margin;
		m_WPlrLegendOffset.ry() = margin;
	}
}



/**
 * Sets the y-axis titles of the stability time response graphs, depending on the selected units
 */
void QMiarex::SetStabGraphTitles()
{
	QString strLength;
	Units::getSpeedUnitLabel(strLength);

	if(m_bLongitudinal)
	{
		m_TimeGraph[0].SetYTitle("u ("+strLength+")");
		m_TimeGraph[1].SetYTitle("w ("+strLength+")");
		m_TimeGraph[2].SetYTitle("q ("+QString::fromUtf8("°") +"/s)");
		m_TimeGraph[3].SetYTitle("theta "+QString::fromUtf8("(°)"));
	}
	else
	{
		m_TimeGraph[0].SetYTitle("v ("+strLength+")");
		m_TimeGraph[1].SetYTitle("p ("+QString::fromUtf8("°") +"/s)");
		m_TimeGraph[2].SetYTitle("r ("+QString::fromUtf8("°") +"/s)");
		m_TimeGraph[3].SetYTitle("phi "+QString::fromUtf8("(°)"));
	}
}



/**
 * Sets the x and y axis titles of the polar graphs
 */
void QMiarex::SetWGraphTitles(Graph* pGraph)
{
	QString Title;

	Title  = WPolar::variableName(pGraph->GetXVariable());
	pGraph->SetXTitle(Title);

	Title  = WPolar::variableName(pGraph->GetYVariable());
	pGraph->SetYTitle(Title);
}


/**
 * Overrides the parent's widget showEvent method
 * Fills the main dialog box with default or selected data
 * @param event unused
 */
void QMiarex::showEvent(QShowEvent *event)
{
	SetAnalysisParams();
	SetCurveParams();
	event->accept();
}



/**
 * Captures the pixels of the client area and writes them to a file/
 * @deprecated QGLWidget::grabFrameBuffer() is used instead.
 * @param FileName the name of the destination image file.
 */
void QMiarex::SnapClient(QString const &FileName)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	ThreeDWidget *p3dWidget = (ThreeDWidget*)s_p3dWidget;
	QGLFormat GLFormat = p3dWidget->format();

	if(!GLFormat.rgba())
	{
		QMessageBox::warning(pMainFrame,tr("Warning"),tr("OpenGL color format is not recognized... Sorry"));
		return;
	}

	int NbBytes, bitsPerPixel;
	QSize size(m_r3DCltRect.width(),m_r3DCltRect.height());

	bitsPerPixel = 24;
	int width = size.width();
	switch(bitsPerPixel)
	{
		case 8:
		{
			QMessageBox::warning(pMainFrame,tr("Warning"),tr("Cannot (yet ?) save 8 bit depth opengl screen images... Sorry"));
			return;
		}
		case 16:
		{
			QMessageBox::warning(pMainFrame,tr("Warning"),tr("Cannot (yet ?) save 16 bit depth opengl screen images... Sorry"));
			size.setWidth(width - size.width() % 2);
			return;
		}
		case 24:
		{
			NbBytes = 4 * size.width() * size.height();//24 bits type BMP
//			size.setWidth(width - size.width() % 4);
			break;
		}
		case 32:
		{
			NbBytes = 4 * size.width() * size.height();//32 bits type BMP
			break;
		}
		default:
		{
			QMessageBox::warning(pMainFrame,tr("Warning"),tr("Unidentified bit depth... Sorry"));
			return;
		}
	}
	uchar *pPixelData = new uchar[NbBytes];

	// Copy from OpenGL
	glReadBuffer(GL_FRONT);
	switch(bitsPerPixel)
	{
		case 8: return;
		case 16: return;
		case 24:
		{
#if QT_VERSION >= 0x040400
			  glReadPixels(0,0,size.width(),size.height(),GL_RGB,GL_UNSIGNED_BYTE,pPixelData);
			  QImage Image(pPixelData, size.width(),size.height(), QImage::Format_RGB888);
			  QImage FlippedImaged;
			  FlippedImaged = Image.mirrored();	//flip vertically
			  FlippedImaged.save(FileName);
#else
			  QMessageBox::warning(pMainFrame,tr("Warning"),"The version of Qt used to compile the code is older than 4.4 and does not support 24 bit images... Sorry");
#endif
			  break;
		}
		case 32:
		{
			glReadPixels(0,0,size.width(),size.height(),GL_RGBA,GL_UNSIGNED_BYTE,pPixelData);
			QImage Image(pPixelData, size.width(),size.height(), QImage::Format_ARGB32);
			QImage FlippedImaged;
			FlippedImaged = Image.mirrored();	//flip vertically
			FlippedImaged.save(FileName);

			break;
		}
		default: break;
	}
}




/**
 * Cancels the animation whatever the active view
 */
void QMiarex::StopAnimate()
{
	m_bAnimateWOpp = false;
	m_pctrlWOppAnimate->setChecked(false);
	m_pTimerWOpp->stop();
	m_pTimerMode->stop();

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
	m_bAnimateMode = false;

	pStabView->m_pctrlAnimate->setChecked(false);

	if(!m_bAnimateWOpp) return;
//	if(m_iView!=WSTABVIEW)
	{
		if(m_pCurPlane)
		{
			SetPlaneOpp(true);
		}
	}
}


/**
 * Updates the active curve with the data from the currently active operating points or polars.
 */
void QMiarex::UpdateCurve()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(m_iView==WPOLARVIEW && m_pCurWPolar)
	{
		m_pCurWPolar->m_Color = m_CurveColor;
		m_pCurWPolar->m_Style = m_CurveStyle;
		m_pCurWPolar->m_Width = m_CurveWidth;
		m_pCurWPolar->m_bIsVisible  = m_bCurveVisible;
		m_pCurWPolar->m_bShowPoints = m_bCurvePoints;
	}
	else if(m_iView==STABTIMEVIEW && m_pCurWPolar)
	{
		StabViewDlg *pStabView = (StabViewDlg*)pMainFrame->m_pStabView;
		pStabView->SetTimeCurveStyle(m_CurveColor, m_CurveStyle, m_CurveWidth, m_bCurveVisible, m_bCurvePoints);
	}
	else if(m_iView==STABPOLARVIEW)
	{
		m_pCurWPolar->m_Color = m_CurveColor;
		m_pCurWPolar->m_Style = m_CurveStyle;
		m_pCurWPolar->m_Width = m_CurveWidth;
		m_pCurWPolar->m_bIsVisible  = m_bCurveVisible;
		m_pCurWPolar->m_bShowPoints = m_bCurvePoints;
	}
	else if (m_iView==WOPPVIEW)
	{
		if(m_pCurPOpp)
		{
			m_pCurPOpp->m_Color = m_CurveColor;
			m_pCurPOpp->m_Style = m_CurveStyle;
			m_pCurPOpp->m_Width = m_CurveWidth;			
			m_pCurPOpp->m_bIsVisible  = m_bCurveVisible;
			m_pCurPOpp->m_bShowPoints = m_bCurvePoints;
		}
	}
	else if (m_iView==WCPVIEW && m_pCurPOpp)
	{
		m_CpColor = m_CurveColor;
		m_CpStyle = m_CurveStyle;
		m_CpWidth = m_CurveWidth;
		m_bShowCp       = m_bCurveVisible;
		m_bShowCpPoints = m_bCurvePoints;
	}

	s_bResetCurves = true;
	UpdateView();
	emit projectModified();
}


/**
 * Updates the graphs and the views after a change of units
*/
void QMiarex::UpdateUnits()
{
	if(m_iView==WPOLARVIEW)
	{
		for(int ig=0; ig<MAXGRAPHS; ig++)
			SetWGraphTitles(m_WPlrGraph+ig);
	}
	else if(m_iView==STABTIMEVIEW || m_iView==STABPOLARVIEW)
	{
		SetStabGraphTitles();
	}
	else
	{
		if(!m_pCurPlane) return;
		if (m_iView==WOPPVIEW)
		{
			OnAdjustToWing();
		}
		else if(m_iView==WCPVIEW) CreateCpCurves();
		else if(m_iView==W3DVIEW) m_bResetglLegend = true;
		else if(m_iView==STABTIMEVIEW || m_iView==STABPOLARVIEW) m_bResetglLegend = true;
	}
	SetCurveParams();
	s_bResetCurves = true;
	UpdateView();
}


/**
 * Dispatches the drawing request depending on the type of the active view
 */
void QMiarex::UpdateView()
{
	TwoDWidget *p2dWidget = (TwoDWidget*)s_p2dWidget;
	ThreeDWidget *p3dWidget = (ThreeDWidget*)s_p3dWidget;
	if(m_iView==W3DVIEW)
	{
		if(p3dWidget) p3dWidget->update();
	}
	else
	{
		if(p2dWidget) p2dWidget->update();
	}
}



/**
 * Overrides the parent's widget wheel event
 * @param event the QWheelEvent sent by Qt
 */
void QMiarex::zoomEvent(QPoint pt, double zoomFactor)
{
	//The mouse button has been wheeled
	//Process the message


	if(m_iView==W3DVIEW)
	{
		if(m_pCurPlane)
		{
			//zoom wing
			m_glScaled *= zoomFactor;
			UpdateView();
		}
	}
	else if(m_iView ==WOPPVIEW || m_iView ==WPOLARVIEW || m_iView ==WCPVIEW || m_iView ==STABTIMEVIEW || m_iView==STABPOLARVIEW)
	{
		m_pCurGraph = GetGraph(pt);
		if(m_pCurGraph && m_pCurGraph->IsInDrawRect(pt))
		{
			if (m_bXPressed)
			{
				//zoom x scale
				m_pCurGraph->SetAutoX(false);
				m_pCurGraph->Scalex(1./zoomFactor);
			}
			else if(m_bYPressed)
			{
				//zoom y scale
				m_pCurGraph->SetAutoY(false);
				m_pCurGraph->Scaley(1./zoomFactor);
			}
			else
			{
				//zoom both
				m_pCurGraph->SetAuto(false);
				m_pCurGraph->Scale(1./zoomFactor);
			}

			m_pCurGraph->SetAutoXUnit();
			m_pCurGraph->SetAutoYUnit();
			UpdateView();

		}
		else if(m_pCurPlane && m_iView==WOPPVIEW)
		{
			m_WingScale *= zoomFactor;
			UpdateView();
		}
	}
}


/**
 * The user has requested a detailed display of the data of the active polar
 */
void QMiarex::OnWPolarProps()
{
	if(!m_pCurWPolar) return;

	ObjectPropsDlg opDlg((MainFrame*)s_pMainFrame);
	opDlg.m_pXDirect = NULL;
	opDlg.m_pOpp = NULL;
	opDlg.m_pPolar = NULL;
	opDlg.m_pMiarex = this;
	opDlg.m_pWOpp = NULL;
	opDlg.m_pWPolar = m_pCurWPolar;
	opDlg.InitDialog();
	opDlg.exec();
}


/**
 * The user has requested a detailed display of the data of the active operating point
 */
void QMiarex::OnWOppProps()
{
	if(!m_pCurPOpp) return;
	ObjectPropsDlg opDlg((MainFrame*)s_pMainFrame);
	opDlg.m_pXDirect = NULL;
	opDlg.m_pOpp = NULL;
	opDlg.m_pPolar = NULL;
	opDlg.m_pMiarex = this;
	opDlg.m_pWOpp = m_pCurPOpp->m_pPlaneWOpp[0];
	opDlg.m_pWPolar = NULL;
	opDlg.InitDialog();
	opDlg.exec();
}


/**
 * Overrides the parent's widget method
 * Displays the appropriate context menu depending on the view.
 * @param event
 */
void QMiarex::contextMenuEvent (QContextMenuEvent * event)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	TwoDWidget *p2dWidget = (TwoDWidget*)s_p2dWidget;
	ThreeDWidget *p3dWidget = (ThreeDWidget*)s_p3dWidget;

	if(pMainFrame->m_pctrlCentralWidget->currentIndex()==0) p2dWidget->contextMenuEvent(event);
	else                                                    p3dWidget->contextMenuEvent(event);
}


/**
 * Paints and overlays the labels associated to the Cp color scale in the 3D view
 * @param painter the painter associated to the 3D widget
 */
void QMiarex::PaintCpLegendText(QPainter &painter)
{
	if (!m_b3DCp || !m_pCurPOpp || m_pCurPOpp->m_AnalysisMethod<VLMMETHOD) return;

	int i;
	QString strong;

	double labellength;

	double f;
	double range, delta;

	painter.save();

	painter.setFont(Settings::s_TextFont);
	painter.setRenderHint(QPainter::Antialiasing);

	QFontMetrics fm(Settings::s_TextFont);
	int back = fm.averageCharWidth() * 5;

	double h = m_r3DCltRect.height();
	double y0 = 2.*h/5.0;


	int ixPos, iyPos, dy;

	ixPos  = m_r3DCltRect.width()-back;

	dy     = (int) (h/40.0);
	iyPos  = (int) (y0 - 12.0*dy);


	range = s_LegendMax - s_LegendMin;
	delta = range / 20;


	QPen textPen(Settings::s_TextColor);
	painter.setPen(textPen);
	painter.setRenderHint(QPainter::Antialiasing);

	strong = "Cp";
	labellength = fm.width(strong)+5;
	painter.drawText(ixPos-labellength, iyPos-dy, strong);

	for (i=0; i<=20; i ++)
	{
		f = s_LegendMax - (double)i * delta;
		strong = QString("%1").arg(f, 5,'f',2);
		labellength = (fm.width(strong)+5);
		painter.drawText(ixPos-labellength, iyPos+i*dy, strong);
	}

	painter.restore();
}



/**
 * Paints and overlays the labels associated to the Panel forces color scale in the 3D view
 * @param painter the painter associated to the 3D widget
 */
void QMiarex::PaintPanelForceLegendText(QPainter &painter)
{
	if(!m_pCurWPolar || !m_pCurPOpp) return;
	if(!m_bPanelForce || m_pCurPOpp->m_AnalysisMethod<VLMMETHOD) return;

	QString strForce, strong;
	int p, i;
	int labellength;
	double f;
	double rmin, rmax, range, delta;

	Units::getForceUnitLabel(strForce);

	painter.save();
	painter.setFont(Settings::s_TextFont);
	painter.setRenderHint(QPainter::Antialiasing);
	QPen textPen(Settings::s_TextColor);
	painter.setPen(textPen);

	Wing *pWingList[MAXWINGS];
	WingOpp *pWOppList[MAXWINGS];
	for(int ip=0; ip<MAXWINGS; ip++)
	{
		pWingList[ip] = m_pWingList[ip];
		pWOppList[ip] = m_pWOpp[ip];
	}


	//define the range of values to set the colors in accordance
	rmin = 1.e10;
	rmax = -rmin;
	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(pWingList[iw])
		{
			for (p=0; p<pWingList[iw]->m_MatSize; p++)
			{
				rmax = qMax(rmax, pWOppList[iw]->m_dCp[p]*pWingList[iw]->m_pWingPanel[p].GetArea());
				rmin = qMin(rmin, pWOppList[iw]->m_dCp[p]*pWingList[iw]->m_pWingPanel[p].GetArea());
			}
		}
	}
	rmin *= 0.5*m_pCurWPolar->m_Density *pWOppList[0]->m_QInf*pWOppList[0]->m_QInf  *Units::NtoUnit();
	rmax *= 0.5*m_pCurWPolar->m_Density *pWOppList[0]->m_QInf*pWOppList[0]->m_QInf  *Units::NtoUnit();
	range = rmax - rmin;


	QFontMetrics fm(Settings::s_TextFont);
	int back = fm.averageCharWidth() * 5;

	double h = (double)m_r3DCltRect.height();
	double y0 = 2.*h/5.0;


	int ixPos, iyPos, dy;

	ixPos  = m_r3DCltRect.width()-back;

	dy     = (int) (h/40.0);
	iyPos  = (int) (y0 - 12.0*dy);

	delta = range / 20.0;


	labellength = fm.width(strForce)+5;
	painter.drawText(ixPos-labellength, iyPos-dy, strForce);


	for (i=0; i<=20; i ++)
	{
		f = rmin + (double)i * delta;
		strong = QString("%1").arg(f, 5,'f',2);
		labellength = (fm.width(strong)+5);
		painter.drawText(ixPos-labellength, iyPos+i*dy, strong);
	}

	painter.restore();
}





/**
* Searches for an operating point with aoa or velocity or control paramter x, for the active polar
* Sets it as active, if valid
* else sets the current PlaneOpp, if any
* else sets the comboBox's first, if any
* else sets it to NULL
*@param bCurrent, if true, uses the x value of the current operating point; this is useful if the user has changed the polar, but wants to display the same aoa for instance
*@return true if a new valid operating point has been selected
*/
bool QMiarex::SetPlaneOpp(bool bCurrent, double x)
{
	if(!m_pCurPlane) return false;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	m_pCurPOpp = Objects3D::setPlaneOppObject(m_pCurPlane, m_pCurWPolar, m_pCurPOpp, bCurrent, x);

	SetCurveParams();

	if(m_pCurPOpp)
	{
		for(int iw=0; iw<MAXWINGS;iw++)
		{
			if(m_pCurPOpp->m_pPlaneWOpp[iw]) m_pWOpp[iw] = m_pCurPOpp->m_pPlaneWOpp[iw];
			else                             m_pWOpp[iw] = NULL;
		}

		//select m_pCurPOpp in the listbox
		pMainFrame->SelectPlaneOpp(m_pCurPOpp);
	}
	else
	{
		m_pWOpp[0] = m_pWOpp[1] = m_pWOpp[2] = m_pWOpp[3] = NULL;
	}

	if(m_iView==STABTIMEVIEW || m_iView==STABPOLARVIEW)
	{
		StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
		pStabView->SetControls();
		pStabView->SetMode();
	}
	else if(m_iView==W3DVIEW)
	{
		StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
		pStabView->SetControls();
		pStabView->SetMode();
	}

	m_bResetglMesh = true;
	m_bResetglOpp    = true;
	m_bResetglStream = true;
	m_bResetglSurfVelocities   = true;
	m_bResetglLegend = true;

	SetControls();

	s_bResetCurves = true;

	if(!m_pCurPOpp) return false;
	else if(m_iView==WOPPVIEW)
	{
		m_bCurveVisible = m_pCurPOpp->m_bIsVisible;
		m_bCurvePoints  = m_pCurPOpp->m_bShowPoints;
	}

	return true;
}









