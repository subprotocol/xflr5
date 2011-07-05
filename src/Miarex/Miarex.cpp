/****************************************************************************

	Miarex
			Copyright (C) 2008-2010 Andre Deperrois XFLR5@yahoo.com

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

//
// This class is associated to the MMI of 3D analysis
// It dispatches user commands towards object definition, analysis and post-processing
//

#include <QGLWidget>
#include <QAction>
#include "Miarex.h"
#include "GLCreateLists.h"
#include "../MainFrame.h"
#include "../TwoDWidget.h"
#include "../Globals.h"
#include <math.h>


void *QMiarex::s_pMainFrame;
void *QMiarex::s_p2DWidget;
void *QMiarex::s_pGLWidget;



QMiarex::QMiarex(QWidget *parent)
	: QWidget(parent)
{
	//construct evrything
	m_pXFile      = NULL;
	m_pPanelDlg   = NULL;
	m_pLLTDlg     = NULL;
	m_pCurPlane   = NULL;
	m_pCurWing    = NULL;
	m_pCurPOpp    = NULL;
	m_pCurBody    = NULL;
	m_pCurWPolar  = NULL;
	m_pCurWOpp    = NULL;
	m_pCurGraph   = m_WingGraph;

	for(int iw=0; iw<MAXWINGS; iw++)
	{
		m_pWingList[iw] = NULL;
		m_pWOpp[iw]     = NULL;
	}

	m_NSurfaces =   0;
	m_NStation  =  20;
	m_Iter      = 100;
	LLTAnalysis::s_CvPrec    =   0.01;
	LLTAnalysis::s_RelaxMax  =  20.0;
	LLTAnalysis::s_NLLTStations = 20;

	m_Panel[0].m_VortexPos = 0.25;
	m_Panel[0].m_CtrlPos   = 0.75;

//	m_WngAnalysis.m_WakePanelFactor = 1.10;
//	m_WngAnalysis.m_TotalWakeLength = 100.00;
//	m_WngAnalysis.m_NXWakePanels    = 1;

	m_CurveStyle = 0;
	m_CurveWidth = 1;
	m_CurveColor = QColor(127, 255, 70);
	m_bCurveVisible = true;
	m_bCurvePoints  = false;

	m_CoreSize        = 0.000001;
	m_MaxWakeIter     = 5;
	m_WakeInterNodes  = 6;
	m_bResetWake      = true;
	m_bSetNewWake     = true;
	m_bSequence       = false;
	m_bBreak          = false;

	m_bHighlightOpp = false;

	m_bDirichlet = true;
	
	m_nWakeNodes = 0;
	m_WakeSize   = 0;
	m_LastPoint.setX(0);
	m_LastPoint.setY(0);
	m_WPlrLegendOffset.setX(0);
	m_WPlrLegendOffset.setY(0);

	m_r2DCltRect.setWidth(0);
	m_r2DCltRect.setHeight(0);
	m_r3DCltRect.setWidth(0);
	m_r3DCltRect.setHeight(0);
	m_rSingleRect.setWidth(0);
	m_rSingleRect.setHeight(0);

	m_MinPanelSize = 0.0001; //.1 mm

	m_LastWOpp      = 0.0;
	m_CurSpanPos    = 0.0;
	m_WingScale     = 1.0;

	m_AlphaMin     =  0.0;
	m_AlphaMax     =  1.0;
	m_AlphaDelta   =  0.5;
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

	CPanel::s_pNode = m_Node;
	CWing::m_pWakePanel = m_WakePanel;
	CWing::m_pWakeNode = m_WakeNode;

	CSurface::s_pPanel = m_Panel;
	CSurface::s_pNode  = m_Node;

	m_GLList = 0;

	m_pTimerWOpp= new QTimer(this);
	m_posAnimateWOpp         = 0;

	m_pTimerMode= new QTimer(this);
	m_posAnimateMode         = 0;

	memset(m_ModeState, 0, 6*sizeof(double));
	m_TimeInput[0] = m_TimeInput[1] = m_TimeInput[2] = m_TimeInput[3];
	m_TotalTime = 10;//s
	m_Deltat    = 0.1;//s

	m_RampTime = .1;//s
	m_RampAmplitude = 1.;//CtrlUnit;


	m_pCurWingGraph = m_WingGraph;
	m_pCurWPlrGraph = m_WPlrGraph;
	m_pCurTimeGraph   = m_TimeGraph;
	m_pCurRLStabGraph = &m_LongRLGraph;

	for(int ig=0; ig<4; ig++)
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
		m_WPlrGraph[ig].SetXTitle(tr("Cd"));
		m_WPlrGraph[ig].SetYTitle(tr("Cl"));
		m_WPlrGraph[ig].SetXMin(-0.0);
		m_WPlrGraph[ig].SetXMax( 0.1);
		m_WPlrGraph[ig].SetYMin(-0.01);
		m_WPlrGraph[ig].SetYMax( 0.01);
		m_WPlrGraph[ig].SetType(0);
		m_WPlrGraph[ig].SetMargin(50);
	}

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

	for(int i=0; i<4;i++) m_CpGraph.AddCurve(); // four curves, one for each of the plane's wings

	//set the default settings for the time response graphs
	for(int ig=0; ig<4; ig++)
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

	m_CpColor = QColor(255,100,150);
	m_CpStyle = 0;
	m_CpWidth = 1;
	m_bShowCpPoints = false;
	m_bShowCp       = true;

	m_WPlrGraph[0].SetVariables(4,1);
	m_WPlrGraph[1].SetVariables(0,1);
	m_WPlrGraph[2].SetVariables(0,5);
	m_WPlrGraph[3].SetVariables(0,12);

	m_iView = W3DVIEW;
	m_iWingView = 1;
	m_iWPlrView = 4;
	m_iStabilityView = STABTIMEVIEW;
	m_iStabTimeView = 4;
	m_nNodes  = 0;
	m_MatSize = 0;

	m_poaFoil    = NULL;
	m_poaPolar   = NULL;
	m_poaWing    = NULL;
	m_poaPlane   = NULL;
	m_poaWPolar  = NULL;
	m_poaWOpp    = NULL;
	m_poaPOpp    = NULL;

//	memset(m_pPanel, 0, sizeof(m_pPanel));
	memset(m_aij, 0, sizeof(m_aij));
	memset(m_aijRef, 0, sizeof(m_aijRef));
	memset(m_RHS, 0, sizeof(m_RHS));
	memset(m_RHSRef, 0, sizeof(m_RHSRef));

	m_WingGraph[0].SetGraphName("Wing Graph 1");
	m_WingGraph[1].SetGraphName("Wing Graph 2");
	m_WingGraph[2].SetGraphName("Wing Graph 3");
	m_WingGraph[3].SetGraphName("Wing Graph 4");

	m_WPlrGraph[0].SetGraphName(tr("Wing Polar Graph 1"));
	m_WPlrGraph[1].SetGraphName(tr("Wing Polar Graph 2"));
	m_WPlrGraph[2].SetGraphName(tr("Wing Polar Graph 3"));
	m_WPlrGraph[3].SetGraphName(tr("Wing Polar Graph 4"));

	m_CpGraph.SetGraphName(tr("Cp Graph"));

	m_VLMStyle    = 0;
	m_VLMWidth    = 1;
	m_VLMColor    = QColor(180,180,180);

	m_3DAxisStyle    = 3;
	m_3DAxisWidth    = 1;
	m_3DAxisColor    = QColor(150,150,150);
	m_OutlineStyle   = 0;
	m_OutlineWidth   = 1;
	m_OutlineColor   = QColor(120,120,120);
	m_XTopStyle      = 0;
	m_XTopWidth      = 1;
	m_XTopColor      = QColor(200, 0, 255);
	m_XBotStyle      = 1;
	m_XBotWidth      = 1;
	m_XBotColor      = QColor(200, 0, 255);
	m_XCPStyle       = 0;
	m_XCPWidth       = 1;
	m_XCPColor       = QColor(50, 150, 50);
	m_MomentStyle    = 0;
	m_MomentWidth    = 1;
	m_MomentColor    = QColor(200, 100, 100);
	m_IDragStyle     = 0;
	m_IDragWidth     = 1;
	m_IDragColor     = QColor(255,200,0);
	m_VDragStyle     = 0;
	m_VDragWidth     = 1;
	m_VDragColor     = QColor(255,000,200);
	m_DownwashStyle  = 0;
	m_DownwashWidth  = 1;
	m_DownwashColor  = QColor(255, 100, 100);
	m_WakeStyle      = 0;
	m_WakeWidth      = 1;
	m_WakeColor      = QColor(0, 150, 200);
	m_StreamLinesStyle  = 0;
	m_StreamLinesWidth  = 1;
	m_StreamLinesColor  = QColor(200, 150, 255);
	m_MassColor = QColor(150, 150, 200);


	m_CpColor = QColor(255,0,0);
	m_CpStyle = 0;
	m_CpWidth = 1;
	m_bShowCp = true;
	m_bShowCpPoints = false;


	m_HorizontalSplit = -0.45;
	m_VerticalSplit   =  0.35;

	m_GLList = 0;

	m_ClipPlanePos = 5.0;

	m_LegendMax  =  1.0;
	m_LegendMin  = -1.0;

	m_glViewportTrans.x  = 0.0;
	m_glViewportTrans.y  = 0.0;
	m_glViewportTrans.z  = 0.0;
	m_glScaled      = 1.0;
	m_GLScale       = 1.0;

	m_LiftScale = m_DragScale = m_VelocityScale = 0.7;

	m_bXPressed = m_bYPressed = false;

	m_bVLM1              = true;
	m_bTrefftz           = true;

	m_b3DCp              = false;
	m_bXCP               = true;
	m_bDownwash          = true;
	m_bMoments           = false;
	m_bICd               = true;
	m_bVCd               = true;
	m_bTrans             = false;
	m_bXTop              = false;
	m_bXBot              = false;

	m_bXCmRef            = true;
	m_bLogFile           = true;
	m_bHalfWing          = false;
	m_bTransGraph        = true;
	m_bFoilNames         = false;
	m_bShowMasses        = false;
	m_bPanelForce    = false;
	m_bLongitudinal      = true;
	m_bCurWOppOnly       = true;
	m_bStoreWOpp         = true;
	m_bKeepOutOpps       = false;
	m_bCurFrameOnly      = true;
	m_bType1 = m_bType2 = m_bType4 = m_bType5 = m_bType6 = m_bType7 = true;
	m_bShowElliptic      = false;
	m_bShowWingCurve[0] = m_bShowWingCurve[1] = m_bShowWingCurve[2] = m_bShowWingCurve[3] = true;
	m_bAutoScales        = false;
	m_bAutoScales        = false;
	m_bAnimateWOpp       = false;
	m_bAnimateWOppPlus   = true;
	m_bAnimateMode       = false;

	m_bResetglBody       = false;
	m_bResetglBodyMesh   = true;

	m_bResetglGeom       = true;
	m_bResetglOpp        = true;
	m_bResetglLift       = true;
	m_bResetglDrag       = true;
	m_bResetglDownwash   = true;
	m_bResetglStream     = true;
	m_bResetglMesh       = true;
	m_bResetglWake       = true;
	m_bResetglLegend     = true;
	m_bResetglFlow       = true;

	m_bArcball           = false;
	m_bStream            = false;
	m_bVortices          = false;
	m_bSpeeds            = false;
	m_bWakePanels        = false;
	m_bCrossPoint        = false;
	m_bSurfaces          = true;
	m_bOutline           = true;
	m_bVLMPanels         = false;
	m_bAxes              = true;
	m_bglLight           = true;
	m_bPickCenter        = false;
	m_bAutoCpScale	     = false;
	m_bShowCpScale       = true;
	m_bShowLight         = false;
	m_bIs2DScaleSet      = false;
	m_bIs3DScaleSet      = false;
//	m_bForcedResponse    = true;
	m_StabilityResponseType = 0;
	
	m_LastPoint.setX(0);
	m_LastPoint.setY(0);
	m_PointDown.setX(0);
	m_PointDown.setY(0);

	memset(MatIn, 0, 16*sizeof(double));
	memset(MatOut, 0, 16*sizeof(double));

	m_ArcBall.m_pOffx     = &m_UFOOffset.x;
	m_ArcBall.m_pOffy     = &m_UFOOffset.y;
	m_ArcBall.m_pTransx   = &m_glViewportTrans.x;
	m_ArcBall.m_pTransy   = &m_glViewportTrans.y;
	m_ArcBall.m_pRect     = &m_r3DCltRect;

	m_pLLTDlg = new LLTAnalysisDlg;

	m_pPanelDlg = new PanelAnalysisDlg;
	CWing::s_p3DPanelDlg = m_pPanelDlg;  //pointer to the 3DPanel analysis dialog class
	m_pPanelDlg->m_pPanel        = m_Panel;
//	m_pPanelDlg->m_ppPanel       = m_pPanel;
	m_pPanelDlg->m_pNode         = m_Node;
	m_pPanelDlg->m_pWakePanel    = m_WakePanel;
	m_pPanelDlg->m_pWakeNode     = m_WakeNode;
	m_pPanelDlg->m_pMemNode      = m_MemNode;
	m_pPanelDlg->m_pMemPanel     = m_MemPanel;
	m_pPanelDlg->m_pTempWakeNode = m_TempWakeNode;
	m_pPanelDlg->m_pRefWakeNode  = m_RefWakeNode;
	m_pPanelDlg->m_pRefWakePanel = m_RefWakePanel;
	m_pPanelDlg->m_aij           = m_aij;
	m_pPanelDlg->m_aijWake       = m_aijRef;
	m_pPanelDlg->m_RHS           = m_RHS;
	m_pPanelDlg->m_RHSRef        = m_RHSRef;
	m_pPanelDlg->m_pCoreSize     = &m_CoreSize;


	GL3dWingDlg::s_pGLLightDlg = &m_GLLightDlg;
	GL3dBodyDlg::s_pGLLightDlg = &m_GLLightDlg;

	SetupLayout();

//	m_pctrlHalfWing->setChecked(m_bHalfWing);
//	m_pctrlTrans->setChecked(m_bXTop);
//	m_pctrlIDrag->setChecked(m_bICd);
//	m_pctrlVDrag->setChecked(m_bVCd);
//	m_pctrlLift->setChecked(m_bXCP);

	//Connect signals to slots
	connect(m_pctrlSequence, SIGNAL(clicked()), this, SLOT(OnSequence()));
	connect(m_pctrlStoreWOpp, SIGNAL(clicked()), this, SLOT(OnStoreWOpp()));
	connect(m_pctrlInitLLTCalc, SIGNAL(clicked()), this, SLOT(OnInitLLTCalc()));
	connect(m_pctrlAnalyze, SIGNAL(clicked()), this, SLOT(OnAnalyze()));
	connect(m_pctrlCurveStyle, SIGNAL(activated(int)), this, SLOT(OnCurveStyle(int)));
	connect(m_pctrlCurveWidth, SIGNAL(activated(int)), this, SLOT(OnCurveWidth(int)));
	connect(m_pctrlCurveColor, SIGNAL(clicked()), this, SLOT(OnCurveColor()));
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

	connect(m_pctrlSurfaces, SIGNAL(clicked()), SLOT(OnSurfaces()));
	connect(m_pctrlOutline, SIGNAL(clicked()), SLOT(OnOutline()));
	connect(m_pctrlPanels, SIGNAL(clicked()), SLOT(OnPanels()));
	connect(m_pctrlVortices, SIGNAL(clicked()), SLOT(OnVortices()));
	connect(m_pctrlFoilNames, SIGNAL(clicked()), SLOT(OnFoilNames()));
	connect(m_pctrlMasses, SIGNAL(clicked()), SLOT(OnMasses()));
	connect(m_pctrlClipPlanePos, SIGNAL(sliderMoved(int)), this, SLOT(OnClipPlane(int)));
	connect(m_pctrlLight, SIGNAL(clicked()), SLOT(OnLight()));

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


CBody* QMiarex::AddBody(CBody *pBody)
{
	//
	// Adds the body pointed by pBody to the array of body objects
	// Places it in alphabetical order
	//
	bool bExists   = false;
	bool bInserted = false;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	CBody *pOldBody;
	int i,j;


	if(pBody->m_BodyName.length())
	{
		for (i=0; i<m_poaBody->size(); i++)
		{
			pOldBody = (CBody*)m_poaBody->at(i);
			if (pOldBody->m_BodyName == pBody->m_BodyName)
			{
				bExists = true;
				break;
			}
		}
	}
	else bExists = true;

	while(!bInserted)
	{
		if(!bExists)
		{
			for (j=0; j<m_poaBody->size(); j++)
			{
				pOldBody = (CBody*)m_poaBody->at(j);
				if (pBody->m_BodyName.compare(pOldBody->m_BodyName, Qt::CaseInsensitive)<0)
				{
					m_poaBody->insert(j, pBody);
					bInserted = true;
					break;
				}
			}
			if(!bInserted)
			{
				m_poaBody->append(pBody);
				bInserted = true;
			}
			pMainFrame->SetSaveState(false);
			return pBody;
		}
		else
		{
			//Ask for user intentions
			if(!SetModBody(pBody))
			{
				delete pBody;
				return NULL;
			}

			bExists = false;
		}
	}
	return NULL;
}



CPlane* QMiarex::AddPlane(CPlane *pPlane)
{
	//
	// Adds the plane pointed by pPlane to the array of plane objects
	//
	int i,j;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	CPlane *pOldPlane;
	QString strong;
	bool bExists   = false;
	bool bInserted = false;

	for (i=0; i<m_poaPlane->size(); i++)
	{
		pOldPlane = (CPlane*)m_poaPlane->at(i);
		if (pOldPlane->PlaneName() == pPlane->PlaneName())
		{
			bExists = true;
			break;
		}
	}

	while(!bInserted)
	{
		if(!bExists)
		{
			for (j=0; j<m_poaPlane->size(); j++)
			{
				pOldPlane = (CPlane*)m_poaPlane->at(j);
				if (pPlane->PlaneName() < pOldPlane->PlaneName())
				{
					m_poaPlane->insert(j, pPlane);
					bInserted = true;
					break;
				}
			}
			if(!bInserted)
			{
				m_poaPlane->append(pPlane);
				bInserted = true;
			}
			return pPlane;
		}
		else
		{
			//overwrite it all
			for (int l=0; l<m_poaPlane->size(); l++)
			{
				pOldPlane = (CPlane*)m_poaPlane->at(l);
				if (pOldPlane->PlaneName() == pPlane->PlaneName())
				{
					pMainFrame->DeletePlane(pOldPlane, false);
				}
			}
			bExists = false;
		}
	}
	return NULL;
}


void QMiarex::AddPOpp(bool bPointOut, double *Cp, double *Gamma, double *Sigma, CPOpp *pPOpp)
{
	//
	// Creates the plane's operating point,
	// fills it with the input resulting from the VLM or 3D-panel analysis,
	// and inserts it in the array of plane operating points
	//
	// In input, takes 
	//   - the array of Cp distribution
	//   - the array of circulation or doublet strengths Gamma
	//   - the array of source strengths Sigma
	//   - the data stored in the PanelAnalysisDlg current instances
	//
	// In output, fills the pPOpp object and returns the pointer
	// 

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	CPOpp *pOldPOpp;
	CWOpp *pWOpp;
	int i,j,l,p;
	double Cb = 0.0;

	if(!pPOpp)
	{
		// loads PAnalysis results in the POpPoint
		// else we're adding from serialization
		if(!m_bKeepOutOpps && bPointOut) return;

		pPOpp = new CPOpp();
		if(pPOpp == NULL) return;

		pPOpp->m_Color = pMainFrame->GetColor(6);
		bool bFound;
		for(i=0; i<30;i++)
		{
			bFound = false;
			for (j=0; j<m_poaWOpp->size();j++)
			{
				pWOpp = (CWOpp*)m_poaWOpp->at(j);
				if(pWOpp->m_Color == pMainFrame->m_crColors[i]) bFound = true;
			}
			for (j=0; j<m_poaPOpp->size();j++)
			{
				pOldPOpp = (CPOpp*)m_poaPOpp->at(j);
				if(pOldPOpp->m_Color == pMainFrame->m_crColors[i]) bFound = true;
			}
			if(!bFound)
			{
				pPOpp->m_Color = pMainFrame->m_crColors[i];
				break;
			}
		}

		pWOpp = &pPOpp->m_PlaneWOpp[0];

		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(m_pWingList[iw]) CreateWOpp(&pPOpp->m_PlaneWOpp[iw], m_pWingList[iw]);
			if(m_pWingList[iw]) pPOpp->m_bWing[iw] = true; else pPOpp->m_bWing[iw] = false;
		}

		pPOpp->m_PlaneName = m_pCurPlane->PlaneName();

		pPOpp->m_bVLM1               = m_pCurWPolar->m_bVLM1;
		pPOpp->m_PlrName             = m_pCurWPolar->m_PlrName;
		pWOpp->m_Type                = m_pCurWPolar->m_Type;

		pPOpp->m_NStation            = m_pCurWing->m_NStation;

		pWOpp->m_nFlaps              = m_pCurWing->m_nFlaps;
		for (i=0; i<m_pCurWing->m_nFlaps; i++)
		{
			pWOpp->m_FlapMoment[i] = m_pCurWing->m_FlapMoment[i];
		}

		for (l=0; l<m_pCurWing->m_NStation; l++)
		{
			if(fabs(m_pCurWing->m_BendingMoment[l])>fabs(Cb))	Cb = m_pCurWing->m_BendingMoment[l];
		}


		pWOpp->m_MaxBending = Cb;

		if(m_pCurWPolar->m_AnalysisMethod==VLMMETHOD)
		{
			QMessageBox::warning(pMainFrame,tr("Warning"),"OldVLM polar\n");

		}
		else if(m_pCurWPolar->m_AnalysisMethod==PANELMETHOD && m_pPanelDlg)
		{
			//get the data from the PanelAnalysis dialog, and from the plane object
			pPOpp->m_NPanels             = m_pPanelDlg->m_MatSize;
			pPOpp->m_Alpha               = m_pPanelDlg->m_OpAlpha;
			pPOpp->m_QInf                = m_pPanelDlg->m_QInf;

			pWOpp->m_Alpha               = m_pPanelDlg->m_OpAlpha;
			pWOpp->m_QInf                = m_pPanelDlg->m_QInf;
			pWOpp->m_CL                  = m_pPanelDlg->m_CL;
			pWOpp->m_CX                  = m_pPanelDlg->m_CX;
			pWOpp->m_CY                  = m_pPanelDlg->m_CY;
			pWOpp->m_ICD                 = m_pPanelDlg->m_InducedDrag;
			pWOpp->m_VCD                 = m_pPanelDlg->m_ViscousDrag;

			pWOpp->m_GCm                 = m_pPanelDlg->m_GCm;
			pWOpp->m_VCm                 = m_pPanelDlg->m_VCm;
			pWOpp->m_ICm                 = m_pPanelDlg->m_ICm;
			pWOpp->m_GRm                 = m_pPanelDlg->m_GRm;
			pWOpp->m_GYm                 = m_pPanelDlg->m_GYm;
			pWOpp->m_VYm                 = m_pPanelDlg->m_VYm;
			pWOpp->m_IYm                 = m_pPanelDlg->m_IYm;

			pWOpp->m_XCP                 = m_pPanelDlg->m_XCP;
			pWOpp->m_YCP                 = m_pPanelDlg->m_YCP;

			pPOpp->m_Beta                = m_pCurWPolar->m_Beta;
			pWOpp->m_Beta                = m_pCurWPolar->m_Beta;

			if(m_pCurWPolar->m_Type==STABILITYPOLAR)
			{
				pPOpp->m_Alpha            = m_pPanelDlg->m_AlphaEq;
				pPOpp->m_QInf             = m_pPanelDlg->u0;
				pPOpp->m_Ctrl             = m_pPanelDlg->m_Ctrl;

				pWOpp->m_Ctrl             = m_pPanelDlg->m_Ctrl;
				pWOpp->m_QInf             = m_pPanelDlg->u0;
				pWOpp->m_Alpha            = m_pPanelDlg->m_AlphaEq;

				for(i=0; i<4; i++)
				{
					pWOpp->m_EigenValue[i]   = m_pPanelDlg->m_rLong[i];
					pWOpp->m_EigenValue[i+4] = m_pPanelDlg->m_rLat[i];
					for(l=0;l<4; l++)
					{
						pWOpp->m_EigenVector[i][l]   = m_pPanelDlg->m_vLong[4*i+l];
						pWOpp->m_EigenVector[i+4][l] = m_pPanelDlg->m_vLat[4*i+l];
					}
				}

				pPOpp->m_PlaneWOpp[0].m_XNP = m_pPanelDlg->XNP;

				pPOpp->m_PlaneWOpp[0].CXu =  m_pPanelDlg->CXu;
				pPOpp->m_PlaneWOpp[0].CZu =  m_pPanelDlg->CZu;
				pPOpp->m_PlaneWOpp[0].Cmu =  m_pPanelDlg->Cmu;

				pPOpp->m_PlaneWOpp[0].CXa =  m_pPanelDlg->CXa;
				pPOpp->m_PlaneWOpp[0].CLa = -m_pPanelDlg->CZa;
				pPOpp->m_PlaneWOpp[0].Cma =  m_pPanelDlg->Cma;

				pPOpp->m_PlaneWOpp[0].CXq =  m_pPanelDlg->CXq;
				pPOpp->m_PlaneWOpp[0].CLq = -m_pPanelDlg->CZq;
				pPOpp->m_PlaneWOpp[0].Cmq =  m_pPanelDlg->Cmq;

				pPOpp->m_PlaneWOpp[0].CYb =  m_pPanelDlg->CYb;
				pPOpp->m_PlaneWOpp[0].Clb =  m_pPanelDlg->Clb;
				pPOpp->m_PlaneWOpp[0].Cnb =  m_pPanelDlg->Cnb;

				pPOpp->m_PlaneWOpp[0].CYp =  m_pPanelDlg->CYp;
				pPOpp->m_PlaneWOpp[0].Clp =  m_pPanelDlg->Clp;
				pPOpp->m_PlaneWOpp[0].Cnp =  m_pPanelDlg->Cnp;

				pPOpp->m_PlaneWOpp[0].CYr =  m_pPanelDlg->CYr;
				pPOpp->m_PlaneWOpp[0].Clr =  m_pPanelDlg->Clr;
				pPOpp->m_PlaneWOpp[0].Cnr =  m_pPanelDlg->Cnr;

				//Only one control derivative for all the controls of the polar
				pPOpp->m_PlaneWOpp[0].m_nControls = 1;
				pPOpp->m_PlaneWOpp[0].CXe[0] = m_pPanelDlg->CXe;
				pPOpp->m_PlaneWOpp[0].CYe[0] = m_pPanelDlg->CYe;
				pPOpp->m_PlaneWOpp[0].CZe[0] = m_pPanelDlg->CZe;
				pPOpp->m_PlaneWOpp[0].CLe[0] = m_pPanelDlg->Cle;
				pPOpp->m_PlaneWOpp[0].CMe[0] = m_pPanelDlg->Cme;
				pPOpp->m_PlaneWOpp[0].CNe[0] = m_pPanelDlg->Cne;


				memcpy(pWOpp->m_ALong, m_pPanelDlg->m_ALong, 16*sizeof(double));
				memcpy(pWOpp->m_ALat,  m_pPanelDlg->m_ALat,  16*sizeof(double));
				memcpy(pWOpp->m_BLong, m_pPanelDlg->m_BLong, MAXCONTROLS * 4*sizeof(double));
				memcpy(pWOpp->m_BLat , m_pPanelDlg->m_BLat,  MAXCONTROLS * 4*sizeof(double));
			}
			else
			{
				pPOpp->m_Ctrl = 0.0;
				pWOpp->m_Ctrl = 0.0;
				memset(pWOpp->m_EigenValue, 0, sizeof(pWOpp->m_EigenValue));
				memset(pWOpp->m_EigenVector, 0, sizeof(pWOpp->m_EigenVector));
				memset(pWOpp->m_ALong, 0, 16*sizeof(double));
				memset(pWOpp->m_ALat,  0,  16*sizeof(double));
				memset(pWOpp->m_BLong, 0, MAXCONTROLS * 4*sizeof(double));
				memset(pWOpp->m_BLat , 0,  MAXCONTROLS * 4*sizeof(double));
			}

		}

		pPOpp->m_PlaneWOpp[1].m_Alpha = pPOpp->m_Alpha;
		pPOpp->m_PlaneWOpp[2].m_Alpha  = pPOpp->m_Alpha;
		pPOpp->m_PlaneWOpp[3].m_Alpha   = pPOpp->m_Alpha;
		pPOpp->m_PlaneWOpp[1].m_QInf  = pPOpp->m_QInf;
		pPOpp->m_PlaneWOpp[2].m_QInf   = pPOpp->m_QInf;
		pPOpp->m_PlaneWOpp[3].m_QInf    = pPOpp->m_QInf;

		if(Cp) memcpy(pPOpp->m_Cp, Cp,  sizeof(pPOpp->m_Cp));

		if(Gamma)	memcpy(pPOpp->m_G,  Gamma,   sizeof(pPOpp->m_G));

		if(Sigma)	memcpy(pPOpp->m_Sigma,  Sigma,   sizeof(pPOpp->m_G));


		p = 0;

		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(m_pWingList[iw])
			{
				memcpy(pPOpp->m_PlaneWOpp[iw].m_Cp, pPOpp->m_Cp+p, m_pWingList[iw]->m_MatSize*sizeof(double));
				p+=m_pWingList[iw]->m_MatSize;
			}
		}

		//add the data to the polar object
		m_pCurWPolar->AddPoint(pPOpp);
	}
	pMainFrame->SetSaveState(false);

	bool bIsInserted = false;

	// add the POpPoint to the POpPoint Array for the current PlaneName
	if(m_bStoreWOpp)
	{
		for (i=0; i<m_poaPOpp->size(); i++)
		{
			pOldPOpp = (CPOpp*)m_poaPOpp->at(i);
			if (pPOpp->m_PlaneName == pOldPOpp->m_PlaneName)
			{
				if (pPOpp->m_PlrName == pOldPOpp->m_PlrName)
				{
					if(pPOpp->m_Type <3 || pPOpp->m_Type==5 || pPOpp->m_Type==6)
					{
						if(fabs(pPOpp->m_Alpha - pOldPOpp->m_Alpha)<0.005)
						{
							//replace existing point
							pPOpp->m_Color = pOldPOpp->m_Color;
							pPOpp->m_Style = pOldPOpp->m_Style;
							pPOpp->m_Width = pOldPOpp->m_Width;
							pPOpp->m_bIsVisible  = pOldPOpp->m_bIsVisible;
							pPOpp->m_bShowPoints = pOldPOpp->m_bShowPoints;
							if (m_pCurPOpp == pOldPOpp)
							{
								m_pCurPOpp = NULL;
								m_pCurWOpp = NULL;
							}
							m_poaPOpp->removeAt(i);
							delete pOldPOpp;
							m_poaPOpp->insert(i, pPOpp);
							bIsInserted = true;
							i = m_poaPOpp->size();// to break
						}
						else if (pPOpp->m_Alpha > pOldPOpp->m_Alpha)
						{
							//insert point
							m_poaPOpp->insert(i, pPOpp);
							bIsInserted = true;
							i = m_poaPOpp->size();// to break
						}
					}
					else if (pPOpp->m_Type==4)
					{
						if(fabs(pPOpp->m_QInf - pOldPOpp->m_QInf)<0.001)
						{
							//replace existing point
							pPOpp->m_Color = pOldPOpp->m_Color;
							pPOpp->m_Style = pOldPOpp->m_Style;
							pPOpp->m_Width = pOldPOpp->m_Width;
							pPOpp->m_bIsVisible  = pOldPOpp->m_bIsVisible;
							pPOpp->m_bShowPoints = pOldPOpp->m_bShowPoints;
							if (m_pCurPOpp == pOldPOpp)
							{
								m_pCurPOpp = NULL;
								m_pCurWOpp = NULL;
							}
							m_poaPOpp->removeAt(i);
							delete pOldPOpp;
							m_poaPOpp->insert(i, pPOpp);
							bIsInserted = true;
							i = m_poaPOpp->size();// to break
						}
						else if (pPOpp->m_QInf > pOldPOpp->m_QInf)
						{
							//insert point
							m_poaPOpp->insert(i, pPOpp);
							bIsInserted = true;
							i = m_poaPOpp->size();// to break
						}
					}
				}
			}
		}

		if (!bIsInserted) 	m_poaPOpp->append(pPOpp);

		pPOpp->m_PlaneWOpp[0].m_Color    = pPOpp->m_Color;
		pPOpp->m_PlaneWOpp[1].m_Color   = pPOpp->m_Color;
		pPOpp->m_PlaneWOpp[2].m_Color    = pPOpp->m_Color;
		pPOpp->m_PlaneWOpp[3].m_Color     = pPOpp->m_Color;
		pPOpp->m_PlaneWOpp[0].m_Style    = pPOpp->m_Style;
		pPOpp->m_PlaneWOpp[1].m_Style   = pPOpp->m_Style;
		pPOpp->m_PlaneWOpp[2].m_Style    = pPOpp->m_Style;
		pPOpp->m_PlaneWOpp[3].m_Style     = pPOpp->m_Style;
		pPOpp->m_PlaneWOpp[0].m_Width    = pPOpp->m_Width;
		pPOpp->m_PlaneWOpp[1].m_Width   = pPOpp->m_Width;
		pPOpp->m_PlaneWOpp[2].m_Width    = pPOpp->m_Width;
		pPOpp->m_PlaneWOpp[3].m_Width     = pPOpp->m_Width;
//		m_pCurPOpp = pPOpp;
//		m_pCurWOpp = &m_pCurPOpp->m_PlaneWOpp[0];
	}
	else
	{
		delete pPOpp;
		m_pCurPOpp = NULL;
		m_pCurWOpp = NULL;
	}
}



CWing* QMiarex::AddWing(CWing *pWing)
{
	//
	// Adds the wing pointed by pWing to the array of wing objects
	// places it in alphabetical order
	//
	int i,j;
	bool bExists   = false;
	bool bInserted = false;
	CWing *pOldWing;

//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(pWing->m_WingName.length())
	{
		for (i=0; i<m_poaWing->size(); i++)
		{
			pOldWing = (CWing*)m_poaWing->at(i);
			if (pOldWing->m_WingName == pWing->m_WingName)
			{
				bExists = true;
				break;
			}
		}
	}
	else bExists = true;

	while(!bInserted)
	{
		if(!bExists)
		{
			for (j=0; j<m_poaWing->size(); j++)
			{
				pOldWing = (CWing*)m_poaWing->at(j);
				if (pWing->m_WingName.compare(pOldWing->m_WingName, Qt::CaseInsensitive)<0)
				{
					m_poaWing->insert(j, pWing);
					bInserted = true;
					break;
				}
			}
			if(!bInserted)
			{
				m_poaWing->append(pWing);
				bInserted = true;
			}
			return pWing;
		}
		else
		{
			//Ask for user intentions

			if(SetModWing(pWing))
			{
				m_poaWing->append(pWing);
				return pWing;
			}
			else
			{
				delete pWing;
				return NULL;
			}

			bExists = false;
		}
	}
	return NULL;
}



void QMiarex::AddWOpp(double QInf, double Alpha, bool bPointOut, double *Gamma, double *Sigma, double *Cp)
{
	//
	// Creates the wing's operating point
	// Fills it with the input resulting from the LLT, VLM or 3D-panel analysis,
	// and inserts it in the array of wing operating points
	//
	// In input, takes 
	//   - the array of Cp distribution
	//   - the array of circulation or doublet strengths Gamma
	//   - the array of source strengths Sigma
	//   - the data stored in the LLTAnalysisDlg or PanelAnalysisDlg current instances
	//   - the data stored in the current wing object
	//
	// In output, fills the pWOpp object and returns the pointer
	// 

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int i,j,l;
	if(!m_bKeepOutOpps && bPointOut) return;
	pMainFrame->SetSaveState(false);

	CWOpp *pWOpp;
	CPOpp *pPOpp;
	CWOpp * pNewPoint;
	pNewPoint = new CWOpp();
	if(pNewPoint == NULL)
	{
		QMessageBox::warning(pMainFrame,tr("Warning"),tr("Not enough memory to store the OpPoint\n"));
		return;
	}
	else
	{
		//load WOpp with data
		pNewPoint->m_Color = pMainFrame->GetColor(5);
		bool bFound;
		for(i=0; i<30;i++)
		{
			bFound = false;
			for (j=0; j<m_poaWOpp->size();j++)
			{
				pWOpp = (CWOpp*)m_poaWOpp->at(j);
				if(pWOpp->m_Color == pMainFrame->m_crColors[i]) bFound = true;
			}
			for (j=0; j<m_poaPOpp->size();j++)
			{
				pPOpp = (CPOpp*)m_poaPOpp->at(j);
				if(pPOpp->m_Color == pMainFrame->m_crColors[i]) bFound = true;
			}
			if(!bFound)
			{
				pNewPoint->m_Color = pMainFrame->m_crColors[i];
				break;
			}
		}

		pNewPoint->m_WingName  = m_pCurWing->WingName();

		pNewPoint->m_Span                = m_pCurWPolar->m_WSpan;
		pNewPoint->m_MAChord             = m_pCurWing->m_MAChord;
		pNewPoint->m_NStation            = m_pCurWing->m_NStation;

		pNewPoint->m_PlrName             = m_pCurWPolar->m_PlrName;
		pNewPoint->m_AnalysisMethod      = m_pCurWPolar->m_AnalysisMethod;
		pNewPoint->m_bVLM1               = m_pCurWPolar->m_bVLM1;
		pNewPoint->m_bThinSurface        = m_pCurWPolar->m_bThinSurfaces;
		pNewPoint->m_bTiltedGeom         = m_pCurWPolar->m_bTiltedGeom;

		pNewPoint->m_Type                = m_pCurWPolar->m_Type;
		pNewPoint->m_Weight              = m_pCurWPolar->m_Mass;

		double Cb =0.0;

		if(m_pCurWPolar->m_AnalysisMethod==LLTMETHOD)
		{
			//get the data from the LLTAnalysis dialog, and from the wing object
			pNewPoint->m_Alpha               = Alpha;
			pNewPoint->m_QInf                = QInf;
			pNewPoint->m_NVLMPanels          = m_pCurWing->m_MatSize;
			pNewPoint->m_bOut                = m_pLLTDlg->m_LLT.m_bWingOut;
			pNewPoint->m_CL                  = m_pLLTDlg->m_LLT.m_CL;
//			pNewPoint->m_CY                  = m_pCurWing->m_CY;
//			pNewPoint->m_CX                  = m_pCurWing->m_CX;
			pNewPoint->m_ICD                 = m_pLLTDlg->m_LLT.m_InducedDrag;

			pNewPoint->m_GCm                 = m_pLLTDlg->m_LLT.m_GCm;
			pNewPoint->m_VCm                 = m_pLLTDlg->m_LLT.m_VCm;
			pNewPoint->m_ICm                 = m_pLLTDlg->m_LLT.m_ICm;
			pNewPoint->m_GRm                 = m_pLLTDlg->m_LLT.m_GRm;
			pNewPoint->m_GYm                 = m_pLLTDlg->m_LLT.m_GYm;
			pNewPoint->m_VYm                 = m_pLLTDlg->m_LLT.m_VYm;
			pNewPoint->m_IYm                 = m_pLLTDlg->m_LLT.m_IYm;

			pNewPoint->m_XCP                 = m_pLLTDlg->m_LLT.m_XCP;
			pNewPoint->m_YCP                 = m_pLLTDlg->m_LLT.m_YCP;
			pNewPoint->m_VCD                 = m_pLLTDlg->m_LLT.m_ViscousDrag;

			for (l=1; l<m_pCurWing->m_NStation; l++)
			{
				pNewPoint->m_SpanPos[l]       = -m_pLLTDlg->m_LLT.m_SpanPos[l];
				pNewPoint->m_StripArea[l]     =  m_pLLTDlg->m_LLT.m_StripArea[l];
				pNewPoint->m_Ai[l]            =  m_pLLTDlg->m_LLT.m_Ai[m_NStation-l];
				pNewPoint->m_Cl[l]            =  m_pLLTDlg->m_LLT.m_Cl[m_NStation-l];
				pNewPoint->m_PCd[l]           =  m_pLLTDlg->m_LLT.m_PCd[m_NStation-l];
				pNewPoint->m_ICd[l]           =  m_pLLTDlg->m_LLT.m_ICd[m_NStation-l];
				pNewPoint->m_Cm[l]            =  m_pLLTDlg->m_LLT.m_Cm[m_NStation-l];
				pNewPoint->m_CmAirf[l]        =  m_pLLTDlg->m_LLT.m_CmAirf[m_NStation-l];
				pNewPoint->m_XCPSpanRel[l]    =  m_pLLTDlg->m_LLT.m_XCPSpanRel[m_NStation-l];
				pNewPoint->m_XCPSpanAbs[l]    =  m_pLLTDlg->m_LLT.m_XCPSpanAbs[m_NStation-l];
				pNewPoint->m_Re[l]            =  m_pLLTDlg->m_LLT.m_Re[m_NStation-l];
				pNewPoint->m_Chord[l]         =  m_pLLTDlg->m_LLT.m_Chord[m_NStation-l];
				pNewPoint->m_Twist[l]         =  m_pLLTDlg->m_LLT.m_Twist[m_NStation-l];
				pNewPoint->m_XTrTop[l]        =  m_pLLTDlg->m_LLT.m_XTrTop[m_NStation-l];
				pNewPoint->m_XTrBot[l]        =  m_pLLTDlg->m_LLT.m_XTrBot[m_NStation-l];
				pNewPoint->m_BendingMoment[l] =  m_pLLTDlg->m_LLT.m_BendingMoment[m_NStation-l];
				memset(pNewPoint->m_Cp, 0, sizeof(pNewPoint->m_Cp));
				if(fabs(m_pLLTDlg->m_LLT.m_BendingMoment[l])>fabs(Cb))	Cb = m_pLLTDlg->m_LLT.m_BendingMoment[l];
			}
		}
		else if(m_pCurWPolar->m_AnalysisMethod==VLMMETHOD)
		{
			//obsolete
			QMessageBox::warning(pMainFrame,tr("Warning"),"OldVLM Polar\n");
		}
		else if(m_pCurWPolar->m_AnalysisMethod==PANELMETHOD && m_pPanelDlg)
		{
			//get the data from the PanelAnalysis dialog, and from the wing object
			pNewPoint->m_Alpha               = m_pPanelDlg->m_OpAlpha;
			pNewPoint->m_QInf                = m_pPanelDlg->m_QInf;
			pNewPoint->m_Ctrl                = m_pPanelDlg->m_Ctrl;
			pNewPoint->m_NVLMPanels          = m_pPanelDlg->m_MatSize;
			pNewPoint->m_bOut                = m_pPanelDlg->m_bPointOut;
			pNewPoint->m_CL                  = m_pPanelDlg->m_CL;
			pNewPoint->m_CY                  = m_pPanelDlg->m_CY;
			pNewPoint->m_CX                  = m_pPanelDlg->m_CX;
			pNewPoint->m_ICD                 = m_pPanelDlg->m_InducedDrag;
			pNewPoint->m_VCD                 = m_pPanelDlg->m_ViscousDrag;

			pNewPoint->m_GCm                 = m_pPanelDlg->m_GCm;
			pNewPoint->m_VCm                 = m_pPanelDlg->m_VCm;
			pNewPoint->m_ICm                 = m_pPanelDlg->m_ICm;
			pNewPoint->m_GRm                 = m_pPanelDlg->m_GRm;
			pNewPoint->m_GYm                 = m_pPanelDlg->m_GYm;
			pNewPoint->m_VYm                 = m_pPanelDlg->m_VYm;
			pNewPoint->m_IYm                 = m_pPanelDlg->m_IYm;

			pNewPoint->m_XCP                 = m_pPanelDlg->m_XCP;
			pNewPoint->m_YCP                 = m_pPanelDlg->m_YCP;

			pNewPoint->m_Beta                = m_pCurWPolar->m_Beta;

			for (l=0; l<m_pCurWing->m_NStation; l++)
			{
				pNewPoint->m_SpanPos[l]   = m_pCurWing->m_SpanPos[l];
				pNewPoint->m_StripArea[l] =  m_pCurWing->m_StripArea[l];
				if(fabs(m_pCurWing->m_BendingMoment[l])>fabs(Cb))	Cb = m_pCurWing->m_BendingMoment[l];
			}
			memcpy(pNewPoint->m_Cl,            m_pCurWing->m_Cl, sizeof(m_pCurWing->m_Cl));
			memcpy(pNewPoint->m_PCd,           m_pCurWing->m_PCd, sizeof(m_pCurWing->m_PCd));
			memcpy(pNewPoint->m_Cm,            m_pCurWing->m_Cm, sizeof(m_pCurWing->m_Cm));
			memcpy(pNewPoint->m_CmAirf,        m_pCurWing->m_CmAirf, sizeof(m_pCurWing->m_CmAirf));
			memcpy(pNewPoint->m_XCPSpanRel,    m_pCurWing->m_XCPSpanRel, sizeof(m_pCurWing->m_XCPSpanRel));
			memcpy(pNewPoint->m_XCPSpanAbs,    m_pCurWing->m_XCPSpanAbs, sizeof(m_pCurWing->m_XCPSpanAbs));
			memcpy(pNewPoint->m_Re,            m_pCurWing->m_Re, sizeof(m_pCurWing->m_Re));
			memcpy(pNewPoint->m_Chord,         m_pCurWing->m_Chord, sizeof(m_pCurWing->m_Chord));
			memcpy(pNewPoint->m_Twist,         m_pCurWing->m_Twist, sizeof(m_pCurWing->m_Twist));
			memcpy(pNewPoint->m_XTrTop,        m_pCurWing->m_XTrTop, sizeof(m_pCurWing->m_XTrTop));
			memcpy(pNewPoint->m_XTrBot,        m_pCurWing->m_XTrBot, sizeof(m_pCurWing->m_XTrBot));
			memcpy(pNewPoint->m_BendingMoment, m_pCurWing->m_BendingMoment, sizeof(m_pCurWing->m_BendingMoment));
			memcpy(pNewPoint->m_ICd,           m_pCurWing->m_ICd, sizeof(pNewPoint->m_ICd));
			memcpy(pNewPoint->m_Vd,            m_pCurWing->m_Vd, sizeof(pNewPoint->m_Vd));
			memcpy(pNewPoint->m_F,             m_pCurWing->m_F,  sizeof(pNewPoint->m_F));
			memcpy(pNewPoint->m_Ai,            m_pCurWing->m_Ai, sizeof(pNewPoint->m_Ai));

			memcpy(pNewPoint->m_Cp,    Cp,    sizeof(pNewPoint->m_Cp));
			memcpy(pNewPoint->m_G,     Gamma, sizeof(pNewPoint->m_G));
			memcpy(pNewPoint->m_Sigma, Sigma, sizeof(pNewPoint->m_G));

			if(m_pCurWPolar->m_bWakeRollUp)
			{
				pNewPoint->m_nWakeNodes     = m_nWakeNodes;
				pNewPoint->m_NXWakePanels   = m_pCurWPolar->m_NXWakePanels;
				pNewPoint->m_FirstWakePanel = m_pCurWPolar->m_TotalWakeLength;
				pNewPoint->m_WakeFactor     = m_pCurWPolar->m_WakePanelFactor;
			}
			else
			{
				pNewPoint->m_nWakeNodes     = 0;
				pNewPoint->m_NXWakePanels   = 1;
				pNewPoint->m_FirstWakePanel = m_pCurWPolar->m_WSpan;
				pNewPoint->m_WakeFactor     = 1.0;
			}
			for (i=0; i<m_pCurWing->m_nFlaps; i++)
				pNewPoint->m_FlapMoment[i] = m_pCurWing->m_FlapMoment[i];

			pNewPoint->m_nFlaps = m_pCurWing->m_nFlaps;

			for(i=0; i<4; i++)
			{
				pNewPoint->m_EigenValue[i]   = m_pPanelDlg->m_rLong[i];
				pNewPoint->m_EigenValue[i+4] = m_pPanelDlg->m_rLat[i];
				for(l=0;l<4; l++)
				{
					pNewPoint->m_EigenVector[i][l]   = m_pPanelDlg->m_vLong[4*i+l];
					pNewPoint->m_EigenVector[i+4][l] = m_pPanelDlg->m_vLat[4*i+l];
				}
			}

			pNewPoint->m_XNP = m_pPanelDlg->XNP;

			pNewPoint->CXu =  m_pPanelDlg->CXu;
			pNewPoint->CZu =  m_pPanelDlg->CZu;
			pNewPoint->Cmu =  m_pPanelDlg->Cmu;

			pNewPoint->CXa =  m_pPanelDlg->CXa;
			pNewPoint->CLa = -m_pPanelDlg->CZa;
			pNewPoint->Cma =  m_pPanelDlg->Cma;

			pNewPoint->CXq =  m_pPanelDlg->CXq;
			pNewPoint->CLq = -m_pPanelDlg->CZq;
			pNewPoint->Cmq =  m_pPanelDlg->Cmq;

			pNewPoint->CYb =  m_pPanelDlg->CYb;
			pNewPoint->Clb =  m_pPanelDlg->Clb;
			pNewPoint->Cnb =  m_pPanelDlg->Cnb;

			pNewPoint->CYp =  m_pPanelDlg->CYp;
			pNewPoint->Clp =  m_pPanelDlg->Clp;
			pNewPoint->Cnp =  m_pPanelDlg->Cnp;

			pNewPoint->CYr =  m_pPanelDlg->CYr;
			pNewPoint->Clr =  m_pPanelDlg->Clr;
			pNewPoint->Cnr =  m_pPanelDlg->Cnr;

			//Only one control derivative for all the controls of the polar
			pNewPoint->m_nControls = 1;
			pNewPoint->CXe[0] = m_pPanelDlg->CXe;
			pNewPoint->CYe[0] = m_pPanelDlg->CYe;
			pNewPoint->CZe[0] = m_pPanelDlg->CZe;
			pNewPoint->CLe[0] = m_pPanelDlg->Cle;
			pNewPoint->CMe[0] = m_pPanelDlg->Cme;
			pNewPoint->CNe[0] = m_pPanelDlg->Cne;

			memcpy(pNewPoint->m_ALong, m_pPanelDlg->m_ALong, 16*sizeof(double));
			memcpy(pNewPoint->m_ALat,  m_pPanelDlg->m_ALat,  16*sizeof(double));
			memcpy(pNewPoint->m_BLong, m_pPanelDlg->m_BLong, MAXCONTROLS * 4*sizeof(double));
			memcpy(pNewPoint->m_BLat , m_pPanelDlg->m_BLat,  MAXCONTROLS * 4*sizeof(double));
		}

		pNewPoint->m_MaxBending = Cb;
	}

	//add the data to the polar object
	m_pCurWPolar->AddPoint(pNewPoint);

	bool bIsInserted = false;

	// add the WOpPoint to the WOpPoint Array for the current WingName
	if(m_bStoreWOpp)
	{
		for (i=0; i<m_poaWOpp->size(); i++)
		{
			pWOpp = (CWOpp*)m_poaWOpp->at(i);
			if (pNewPoint->m_WingName == pWOpp->m_WingName)
			{
				if (pNewPoint->m_PlrName == pWOpp->m_PlrName)
				{
					if(pNewPoint->m_Type !=4)
					{
						if(fabs(pNewPoint->m_Alpha - pWOpp->m_Alpha)<0.005)
						{
							//replace existing point
							pNewPoint->m_Color = pWOpp->m_Color;
							pNewPoint->m_Style = pWOpp->m_Style;
							pNewPoint->m_Width = pWOpp->m_Width;
							pNewPoint->m_bIsVisible  = pWOpp->m_bIsVisible;
							pNewPoint->m_bShowPoints = pWOpp->m_bShowPoints;
							if (m_pCurWOpp == pWOpp) m_pCurWOpp = NULL;
							m_poaWOpp->removeAt(i);
							delete pWOpp;
							m_poaWOpp->insert(i, pNewPoint);
							bIsInserted = true;
							i = m_poaWOpp->size();// to break
						}
						else if (pNewPoint->m_Alpha > pWOpp->m_Alpha)
						{
							//insert point
							m_poaWOpp->insert(i, pNewPoint);
							bIsInserted = true;
							i = m_poaWOpp->size();// to break
						}
					}
					else
					{
						if(fabs(pNewPoint->m_QInf - pWOpp->m_QInf)<0.005)
						{
							//replace existing point
							pNewPoint->m_Color = pWOpp->m_Color;
							pNewPoint->m_Style = pWOpp->m_Style;
							pNewPoint->m_Width = pWOpp->m_Width;
							pNewPoint->m_bIsVisible  = pWOpp->m_bIsVisible;
							pNewPoint->m_bShowPoints = pWOpp->m_bShowPoints;
							if (m_pCurWOpp == pWOpp) m_pCurWOpp = NULL;
							m_poaWOpp->removeAt(i);
							delete pWOpp;
							m_poaWOpp->insert(i, pNewPoint);
							bIsInserted = true;
							i = m_poaWOpp->size();// to break
						}
						else if (pNewPoint->m_QInf > pWOpp->m_QInf)
						{
							//insert point
							m_poaWOpp->insert(i, pNewPoint);
							bIsInserted = true;
							i = m_poaWOpp->size();// to break
						}
					}
				}
			}
		}
		if (!bIsInserted) 	m_poaWOpp->append(pNewPoint);
//		m_pCurWOpp = pNewPoint;
	}
	else
	{
		delete pNewPoint;
		pNewPoint = NULL;
	}
}


CWPolar* QMiarex::AddWPolar(CWPolar *pWPolar)
{
	// 
	// Adds the WPolar pointed by pWPolar to the m_oaWPolar array
	// Inserts it in alphabetical order
	//
	int i,j,k,l,p;
	bool bExists   = false;
	bool bInserted = false;
	CWPolar *pOldWPlr;
	CWPolar *pOld2WPlr;
	QString strong;

	for (i=0; i<m_poaWPolar->size(); i++)
	{
		pOldWPlr = (CWPolar*)m_poaWPolar->at(i);
		if (pOldWPlr->m_PlrName == pWPolar->m_PlrName && pOldWPlr->m_UFOName == pWPolar->m_UFOName)
		{
			bExists = true;
			break;
		}
	}

	while(!bInserted)
	{
		if(!bExists)
		{
			//if it doesn't exist, find its place in alphabetical order and insert it
			for (j=0; j<m_poaWPolar->size(); j++)
			{
				pOldWPlr = (CWPolar*)m_poaWPolar->at(j);
				if(pWPolar->m_UFOName.compare(pOldWPlr->m_UFOName, Qt::CaseInsensitive)<0)
				{
					m_poaWPolar->insert(j, pWPolar);
					bInserted = true;
					break;
				}
				else if (pWPolar->m_UFOName == pOldWPlr->m_UFOName)
				{
					//first sort by Type
					if(pWPolar->m_PlrName.compare(pOldWPlr->m_PlrName, Qt::CaseInsensitive)<0)
					{
						m_poaWPolar->insert(j, pWPolar);
						bInserted = true;
						break;
					}
				}
			}
			if(!bInserted)
			{
				m_poaWPolar->append(pWPolar);
				bInserted = true;
			}
			return pWPolar;
		}
		else
		{
			// if it exists, append an index to the WPolar, change the existence flag, and re-enter the loop
			for (l=0; l<m_poaWPolar->size(); l++)
			{
				pOldWPlr = (CWPolar*)m_poaWPolar->at(l);
				if (pOldWPlr->m_UFOName == pWPolar->m_UFOName && pOldWPlr->m_PlrName == pWPolar->m_PlrName)
				{
					p = 2;
					bool bFound = true;
					while(bFound)
					{
						strong=QString(" (%1)").arg( p);
						strong = pWPolar->m_PlrName + strong;
						for (k=0; k<m_poaWPolar->size(); k++)
						{
							bFound = false;
							pOld2WPlr = (CWPolar*)m_poaWPolar->at(k);
							if (pOld2WPlr->m_UFOName == pWPolar->m_UFOName &&
								pOld2WPlr->m_PlrName == strong)
							{
								p++;
								bFound = true;
								break;
							}
						}
					}
					pWPolar->m_PlrName = strong;
				}
			}
			bExists = false;
		}
	}
	return NULL;
}



void QMiarex::SetControls()
{
	//
	// Checks and enables all buttons and actions
	// depending on the currently selected objects
	//
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	QString str_translation;
	if(pMainFrame->m_UFOType=="Plane")     str_translation=tr("Current Plane");
	else if(pMainFrame->m_UFOType=="Wing") str_translation=tr("Current Wing");
	else                                   str_translation=tr("Current Object");
	pMainFrame->currentUFOMenu->setTitle(str_translation);

	if(m_iView==W3DVIEW)        m_pctrBottomControls->setCurrentIndex(1);
	else if(m_iView==WSTABVIEW)
	{
		if(m_iStabilityView == STAB3DVIEW) m_pctrBottomControls->setCurrentIndex(1);
		else                               m_pctrBottomControls->setCurrentIndex(0);
	}
	else m_pctrBottomControls->setCurrentIndex(0);

	if(m_iView==WPOLARVIEW)     m_pctrlMiddleControls->setCurrentIndex(1);
	else if(m_iView==WCPVIEW)   m_pctrlMiddleControls->setCurrentIndex(2);
	else if(m_iView==WSTABVIEW) m_pctrlMiddleControls->setCurrentIndex(1);
	else                        m_pctrlMiddleControls->setCurrentIndex(0);

	if(m_iView==WSTABVIEW || (m_iView==W3DVIEW && this->m_pCurWPolar && this->m_pCurWPolar->m_Type==STABILITYPOLAR)) pMainFrame->m_pctrlStabViewWidget->show();
	else pMainFrame->m_pctrlStabViewWidget->hide();

//	pMainFrame->StabTimeAct->setEnabled(m_pCurWPolar && m_pCurWPolar->m_Type==STABILITYPOLAR);
//	pMainFrame->RootLocusAct->setEnabled(m_pCurWPolar && m_pCurWPolar->m_Type==STABILITYPOLAR);

	if(m_pCurWPolar)
	{
		QString PolarProps;
		m_pCurWPolar->GetPolarProperties(PolarProps);
		m_pctrlPolarProps1->setText(PolarProps);
	}
	else
	{
		m_pctrlPolarProps1->clear();
	}

	m_pctrlInitLLTCalc->setEnabled(m_pCurWPolar && m_pCurWPolar->m_AnalysisMethod==LLTMETHOD);

	pMainFrame->m_pctrlWOppView->setChecked(m_iView==WOPPVIEW);
	pMainFrame->m_pctrlWPolarView->setChecked(m_iView==WPOLARVIEW);
	pMainFrame->m_pctrl3dView->setChecked(m_iView==W3DVIEW);
	pMainFrame->m_pctrlCpView->setChecked(m_iView==WCPVIEW);

	pMainFrame->WOppAct->setChecked(m_iView==WOPPVIEW);
	pMainFrame->WPolarAct->setChecked(m_iView==WPOLARVIEW);
	pMainFrame->W3DAct->setChecked(m_iView==W3DVIEW || (m_iView==WSTABVIEW && m_iStabilityView==STAB3DVIEW));
	pMainFrame->CpViewAct->setChecked(m_iView==WCPVIEW);
	pMainFrame->StabTimeAct->setChecked(m_iView==WSTABVIEW && m_iStabilityView==STABTIMEVIEW);
	pMainFrame->RootLocusAct->setChecked(m_iView==WSTABVIEW && m_iStabilityView==STABPOLARVIEW);

	pMainFrame->HalfWingAct->setChecked(m_bHalfWing);
	pMainFrame->showEllipticCurve->setChecked(m_bShowElliptic);
	pMainFrame->showXCmRefLocation->setChecked(m_bXCmRef);
	pMainFrame->showWing2Curve->setChecked(m_bShowWingCurve[1]);
	pMainFrame->showStabCurve->setChecked(m_bShowWingCurve[2]);
	pMainFrame->showFinCurve->setChecked(m_bShowWingCurve[3]);

	pMainFrame->showCurWOppOnly->setChecked(m_bCurWOppOnly);

	if(m_iView==WOPPVIEW)
	{
		pMainFrame->Graph1->setChecked(m_iWingView==1 && (m_pCurWingGraph == m_WingGraph));
		pMainFrame->Graph2->setChecked(m_iWingView==1 && (m_pCurWingGraph == m_WingGraph+1));
		pMainFrame->Graph3->setChecked(m_iWingView==1 && (m_pCurWingGraph == m_WingGraph+2));
		pMainFrame->Graph4->setChecked(m_iWingView==1 && (m_pCurWingGraph == m_WingGraph+3));
		pMainFrame->twoGraphs->setChecked(m_iWingView==2);
		pMainFrame->fourGraphs->setChecked(m_iWingView==4);
	}
	else if(m_iView==WSTABVIEW)
	{
		pMainFrame->Graph1->setChecked(m_iStabTimeView==1 && (m_pCurTimeGraph== m_TimeGraph));
		pMainFrame->Graph2->setChecked(m_iStabTimeView==1 && (m_pCurTimeGraph == (m_TimeGraph+1)));
		pMainFrame->Graph3->setChecked(m_iStabTimeView==1 && (m_pCurTimeGraph == (m_TimeGraph+2)));
		pMainFrame->Graph4->setChecked(m_iStabTimeView==1 && (m_pCurTimeGraph == (m_TimeGraph+3)));
		pMainFrame->twoGraphs->setChecked(m_iStabTimeView==2);
		pMainFrame->fourGraphs->setChecked(m_iStabTimeView==4);
	}
	else if(m_iView==WPOLARVIEW)
	{
		pMainFrame->Graph1->setChecked(m_iWPlrView==1 && (m_pCurWPlrGraph == m_WPlrGraph));
		pMainFrame->Graph2->setChecked(m_iWPlrView==1 && (m_pCurWPlrGraph == m_WPlrGraph+1));
		pMainFrame->Graph3->setChecked(m_iWPlrView==1 && (m_pCurWPlrGraph == m_WPlrGraph+2));
		pMainFrame->Graph4->setChecked(m_iWPlrView==1 && (m_pCurWPlrGraph == m_WPlrGraph+3));
		pMainFrame->twoGraphs->setChecked(m_iWPlrView==2);
		pMainFrame->fourGraphs->setChecked(m_iWPlrView==4);
	}

	pMainFrame->WPlrGraphMenu->setEnabled(m_iView==WPOLARVIEW || m_iView==WSTABVIEW);
	pMainFrame->hideAllWPlrs->setEnabled(m_iView==WPOLARVIEW || m_iView==WSTABVIEW);
	pMainFrame->showAllWPlrs->setEnabled(m_iView==WPOLARVIEW || m_iView==WSTABVIEW);

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
	pMainFrame->showStabCurve->setEnabled( m_pWingList[2]  && (m_iView==WOPPVIEW || m_iView==WCPVIEW));
	pMainFrame->showFinCurve->setEnabled(  m_pWingList[3]   && (m_iView==WOPPVIEW || m_iView==WCPVIEW));
	pMainFrame->showAllWPlrOpps->setEnabled(m_iView==WOPPVIEW);
	pMainFrame->hideAllWPlrOpps->setEnabled(m_iView==WOPPVIEW);
	pMainFrame->WOppGraphMenu->setEnabled(   m_iView==WOPPVIEW || m_iView==WSTABVIEW);
	pMainFrame->WOppCurGraphMenu->setEnabled(m_iView==WOPPVIEW || m_iView==WCPVIEW || m_iView==WSTABVIEW);

	m_pctrlLift->setEnabled((m_iView==WOPPVIEW||m_iView==W3DVIEW) && m_pCurWOpp);
	m_pctrlTrans->setEnabled((m_iView==WOPPVIEW||m_iView==W3DVIEW) && m_pCurWOpp);
	m_pctrlWOppAnimate->setEnabled((m_iView==WOPPVIEW||m_iView==W3DVIEW) && m_pCurWOpp);
	m_pctrlAnimateWOppSpeed->setEnabled((m_iView==WOPPVIEW||m_iView==W3DVIEW) && m_pCurWOpp);
	m_pctrlIDrag->setEnabled(m_iView==W3DVIEW && m_pCurWOpp);
	m_pctrlVDrag->setEnabled(m_iView==W3DVIEW && m_pCurWOpp);
	m_pctrlDownwash->setEnabled(m_iView==W3DVIEW && m_pCurWOpp);
	m_pctrlVortices->setEnabled(m_iView==W3DVIEW && m_pCurWOpp);
	m_pctrlMoment->setEnabled(m_iView==W3DVIEW && m_pCurWOpp);
	m_pctrlPanelForce->setEnabled(m_iView==W3DVIEW && m_pCurWOpp && m_pCurWOpp->m_AnalysisMethod!=LLTMETHOD);
	m_pctrlCp->setEnabled(m_iView==W3DVIEW && m_pCurWOpp && m_pCurWOpp->m_AnalysisMethod!=LLTMETHOD);
	m_pctrlStream->setEnabled(m_iView==W3DVIEW && m_pCurWOpp && m_pCurWOpp->m_AnalysisMethod!=LLTMETHOD);
	m_pctrlSurfVel->setEnabled(m_iView==W3DVIEW && m_pCurWOpp && m_pCurWOpp->m_AnalysisMethod!=LLTMETHOD);

	pMainFrame->highlightWOppAct->setChecked(m_bHighlightOpp);
	pMainFrame->CurBodyMenu->setEnabled(m_pCurBody);
	pMainFrame->EditCurBody->setEnabled(m_pCurBody);
	pMainFrame->exportBodyDef->setEnabled(m_pCurBody);
	pMainFrame->exportBodyGeom->setEnabled(m_pCurBody);

	pMainFrame->defineWPolar->setEnabled(m_pCurWing);
	pMainFrame->defineStabPolar->setEnabled(m_pCurWing);

	pMainFrame->currentUFOMenu->setEnabled(m_pCurWing);
	pMainFrame->CurWPlrMenu->setEnabled(m_pCurWPolar);
	pMainFrame->CurWOppMenu->setEnabled(m_pCurWOpp);


	StabViewDlg *pStabView = (StabViewDlg*)pMainFrame->m_pStabView;
	pStabView->SetControls();

	m_pctrlSpanPos->SetValue(m_CurSpanPos);
	m_pctrlCpSectionSlider->setValue((int)(m_CurSpanPos*100.0));

	SetCurveParams();
}


void QMiarex::ClientToGL(QPoint const &point, CVector &real)
{
	//
	// In input, takes the 2D point in screen client area coordinates
	// In output, returns the 2D OpenGL point
	//
	static double h2, w2;
	h2 = (double)m_r3DCltRect.height() /2.0;
	w2 = (double)m_r3DCltRect.width()  /2.0;

	if(w2>h2)
	{
		real.x =  ((double)point.x() - w2) / w2;
		real.y = -((double)point.y() - h2) / w2;
	}
	else
	{
		real.x =  ((double)point.x() - w2) / h2;
		real.y = -((double)point.y() - h2) / h2;
	}
}


void QMiarex::CreateCpCurves()
{
	//
	// Creates the curves of the Cp graph at the selected span positions
	//
	//
	int p,pp,i;
	bool bFound;
	double SpanPos, SpanInc;

	CVector N;
	CCurve *pCurve = NULL;
	QString str1, str2, str3;

	for (i=0; i<4; i++)
	{
		// the first four curves are necessarily the current opPoint's main wing, second wing, elevator and fin
		// the next are those the user has chosen to keep for display --> don't reset them
		pCurve = m_CpGraph.GetCurve(i);
		if(pCurve) pCurve->ResetCurve();
	}

	if(!m_pCurWOpp || !m_bShowCp)
	{
		return;
	}

	int coef = 2;
	bool b2Sides = true;
	if(m_pCurWPolar->m_bThinSurfaces)
	{
		b2Sides = false;
		coef = 1;
	}

	m_CurSpanPos = qMax(-1.0, m_CurSpanPos);
	m_CurSpanPos = qMin( 1.0, m_CurSpanPos);
	SpanPos = m_CurSpanPos*m_pCurWOpp->m_Span/2.000001;

	str1 = m_pCurWing->WingName();
	str2 = QString(" a=%1").arg(m_pCurWOpp->m_Alpha,5,'f',2);
	str3 = QString(" x/c=%1").arg(m_CurSpanPos,5,'f',2);

//	if(m_bCurWOppOnly)
	{
		p=0;
		bFound = false;
		if(m_pCurWPolar->m_bThinSurfaces) p+=m_pCurWing->m_Surface[0].m_NXPanels;

		SpanInc = -m_pCurWing->m_PlanformSpan/2.0;
		for (p=0; p<m_pCurWing->m_MatSize; p++)
		{
			if(m_pCurWing->m_pPanel[p].m_bIsTrailing && m_pCurWing->m_pPanel[p].m_iPos<=0)
			{
				SpanInc += m_pCurWing->m_pPanel[p].Width();
				if(SpanPos<=SpanInc || fabs(SpanPos-SpanInc)/m_pCurWing->m_PlanformSpan<0.001)
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
				if(m_pCurWPolar->m_bThinSurfaces) p+=m_pWingList[iw]->m_Surface[0].m_NXPanels;

				SpanInc = -m_pWingList[iw]->m_PlanformSpan/2.0;
				for (p=0; p<m_pWingList[iw]->m_MatSize; p++)
				{
					if(m_pWingList[iw]->m_pPanel[p].m_bIsTrailing && m_pWingList[iw]->m_pPanel[p].m_iPos<=0)
					{
						SpanInc += m_pWingList[iw]->m_pPanel[p].Width();
						if(SpanPos<=SpanInc || fabs(SpanPos-SpanInc)/m_pWingList[iw]->m_PlanformSpan<0.001)
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

					for (pp=p; pp<p+coef*m_pWingList[iw]->m_Surface[0].m_NXPanels; pp++)
					{
						pCurve->AddPoint(m_Panel[pp].CollPt.x, m_pWOpp[iw]->m_Cp[pp]);
					}
				}
			}
		}
	}
}



int QMiarex::CreateBodyElements()
{
	//
	// Creates the panel elements at the body's surface
	//
	if(!m_pCurBody) return 0;
	int i,j,k,l;
	double uk, uk1, v, dj, dj1, dl1;
	double dpx, dpz;
	CVector LATB, TALB;
	CVector LA, LB, TA, TB;
	CVector PLA, PTA, PLB, PTB;

	int n0, n1, n2, n3, lnx, lnh;
	int nx = m_pCurBody->m_nxPanels;
	int nh = m_pCurBody->m_nhPanels;
	int p = 0;

	int InitialSize = m_MatSize;
	int FullSize =0;

	lnx = 0;

	if(m_pCurPlane && m_pCurPlane->Body())
	{
		dpx = m_pCurPlane->BodyPos().x;
		dpz = m_pCurPlane->BodyPos().z;
	}
	else dpx=dpz=0.0;

	if(m_pCurBody->m_LineType==BODYPANELTYPE)
	{
		nx = 0;
		for(i=0; i<m_pCurBody->m_NStations-1; i++) nx+=m_pCurBody->m_xPanels[i];
		nh = 0;
		for(i=0; i<m_pCurBody->m_NSideLines-1; i++) nh+=m_pCurBody->m_hPanels[i];
		FullSize = nx*nh*2;
		m_pCurBody->m_nxPanels = nx;
		m_pCurBody->m_nhPanels = nh;

		for (i=0; i<m_pCurBody->m_NStations-1; i++)
		{
			for (j=0; j<m_pCurBody->m_xPanels[i]; j++)
			{
				dj  = (double) j   /(double)(m_pCurBody->m_xPanels[i]);
				dj1 = (double)(j+1)/(double)(m_pCurBody->m_xPanels[i]);

				//body left side
				lnh = 0;
				for (k=0; k<m_pCurBody->m_NSideLines-1; k++)
				{
					//build the four corner points of the strips
					PLB.x =  (1.0- dj) * m_pCurBody->m_FramePosition[i].x      +  dj * m_pCurBody->m_FramePosition[i+1].x       +dpx;
					PLB.y = -(1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k].y   -  dj * m_pCurBody->m_Frame[i+1].m_Point[k].y;
					PLB.z =  (1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k].z   +  dj * m_pCurBody->m_Frame[i+1].m_Point[k].z    +dpz;

					PTB.x =  (1.0-dj1) * m_pCurBody->m_FramePosition[i].x      + dj1 * m_pCurBody->m_FramePosition[i+1].x       +dpx;
					PTB.y = -(1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k].y   - dj1 * m_pCurBody->m_Frame[i+1].m_Point[k].y;
					PTB.z =  (1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k].z   + dj1 * m_pCurBody->m_Frame[i+1].m_Point[k].z    +dpz;

					PLA.x =  (1.0- dj) * m_pCurBody->m_FramePosition[i].x      +  dj * m_pCurBody->m_FramePosition[i+1].x       +dpx;
					PLA.y = -(1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k+1].y -  dj * m_pCurBody->m_Frame[i+1].m_Point[k+1].y;
					PLA.z =  (1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k+1].z +  dj * m_pCurBody->m_Frame[i+1].m_Point[k+1].z  +dpz;

					PTA.x =  (1.0-dj1) * m_pCurBody->m_FramePosition[i].x      + dj1 * m_pCurBody->m_FramePosition[i+1].x       +dpx;
					PTA.y = -(1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k+1].y - dj1 * m_pCurBody->m_Frame[i+1].m_Point[k+1].y;
					PTA.z =  (1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k+1].z + dj1 * m_pCurBody->m_Frame[i+1].m_Point[k+1].z  +dpz;

					LB = PLB;
					TB = PTB;

					for (l=0; l<m_pCurBody->m_hPanels[k]; l++)
					{
						dl1  = (double)(l+1) / (double)(m_pCurBody->m_hPanels[k]);
						LA = PLB * (1.0- dl1) + PLA * dl1;
						TA = PTB * (1.0- dl1) + PTA * dl1;

						n0 = IsNode(LA);
						n1 = IsNode(TA);
						n2 = IsNode(LB);
						n3 = IsNode(TB);

						if(n0>=0) {
							m_Panel[m_MatSize].m_iLA = n0;
						}
						else {
							m_Panel[m_MatSize].m_iLA = m_nNodes;
							m_Node[m_nNodes].Copy(LA);
							m_nNodes++;
						}

						if(n1>=0) {
							m_Panel[m_MatSize].m_iTA = n1;
						}
						else {
							m_Panel[m_MatSize].m_iTA = m_nNodes;
							m_Node[m_nNodes].Copy(TA);
							m_nNodes++;
						}

						if(n2>=0) {
							m_Panel[m_MatSize].m_iLB = n2;
						}
						else {
							m_Panel[m_MatSize].m_iLB = m_nNodes;
							m_Node[m_nNodes].Copy(LB);
							m_nNodes++;
						}

						if(n3 >=0) {
							m_Panel[m_MatSize].m_iTB = n3;
						}
						else {
							m_Panel[m_MatSize].m_iTB = m_nNodes;
							m_Node[m_nNodes].Copy(TB);
							m_nNodes++;
						}

						LATB = TB - LA;
						TALB = LB - TA;
						m_Panel[m_MatSize].Normal = LATB * TALB;
						m_Panel[m_MatSize].Area =  m_Panel[m_MatSize].Normal.VAbs()/2.0;
						m_Panel[m_MatSize].Normal.Normalize();

						m_Panel[m_MatSize].m_bIsInSymPlane  = false;
						m_Panel[m_MatSize].m_bIsLeading     = false;
						m_Panel[m_MatSize].m_bIsTrailing    = false;
						m_Panel[m_MatSize].m_iPos     = 100;
						m_Panel[m_MatSize].m_iElement = m_MatSize;
						m_Panel[m_MatSize].m_iSym     = -1;
						m_Panel[m_MatSize].m_bIsLeftPanel  = true;
						m_Panel[m_MatSize].SetFrame(LA, LB, TA, TB);

						// set neighbour panels

						m_Panel[m_MatSize].m_iPD = m_MatSize + nh;
						m_Panel[m_MatSize].m_iPU = m_MatSize - nh;

						if(lnx==0)      m_Panel[m_MatSize].m_iPU = -1;// no panel downstream
						if(lnx==nx-1)	m_Panel[m_MatSize].m_iPD = -1;// no panel upstream

						m_Panel[m_MatSize].m_iPL = m_MatSize + 1;
						m_Panel[m_MatSize].m_iPR = m_MatSize - 1;

						if(lnh==0)     m_Panel[m_MatSize].m_iPR = InitialSize + FullSize - p - 1;
						if(lnh==nh-1)  m_Panel[m_MatSize].m_iPL = InitialSize + FullSize - p - 1;

						m_MatSize++;
						p++;
						LB = LA;
						TB = TA;
						lnh++;
					}
				}
				lnx++;
			}
		}
	}
	else if(m_pCurBody->m_LineType==BODYSPLINETYPE)
	{
		FullSize = 2*nx*nh;
		//start with left side... same as for wings
		for (k=0; k<nx; k++)
		{
			uk  = m_pCurBody->s_XPanelPos[k];
			uk1 = m_pCurBody->s_XPanelPos[k+1];

			m_pCurBody->GetPoint(uk,  0, false, LB);
			m_pCurBody->GetPoint(uk1, 0, false, TB);

			LB.x += dpx;
			LB.z += dpz;
			TB.x += dpx;
			TB.z += dpz;

			for (l=0; l<nh; l++)
			{
				//start with left side... same as for wings
				v = (double)(l+1) / (double)(nh);
				m_pCurBody->GetPoint(uk,  v, false, LA);
				m_pCurBody->GetPoint(uk1, v, false, TA);

				LA.x += dpx;
				LA.z += dpz;
				TA.x += dpx;
				TA.z += dpz;

				n0 = IsNode(LA);
				n1 = IsNode(TA);
				n2 = IsNode(LB);
				n3 = IsNode(TB);

				if(n0>=0) {
					m_Panel[m_MatSize].m_iLA = n0;
				}
				else {
					m_Panel[m_MatSize].m_iLA = m_nNodes;
					m_Node[m_nNodes].Copy(LA);
					m_nNodes++;
				}

				if(n1>=0) {
					m_Panel[m_MatSize].m_iTA = n1;
				}
				else {
					m_Panel[m_MatSize].m_iTA = m_nNodes;
					m_Node[m_nNodes].Copy(TA);
					m_nNodes++;
				}

				if(n2>=0) {
					m_Panel[m_MatSize].m_iLB = n2;
				}
				else {
					m_Panel[m_MatSize].m_iLB = m_nNodes;
					m_Node[m_nNodes].Copy(LB);
					m_nNodes++;
				}

				if(n3 >=0) {
					m_Panel[m_MatSize].m_iTB = n3;
				}
				else {
					m_Panel[m_MatSize].m_iTB = m_nNodes;
					m_Node[m_nNodes].Copy(TB);
					m_nNodes++;
				}

				LATB = TB - LA;
				TALB = LB - TA;
				m_Panel[m_MatSize].Normal = LATB * TALB;
				m_Panel[m_MatSize].Area =  m_Panel[m_MatSize].Normal.VAbs()/2.0;
				m_Panel[m_MatSize].Normal.Normalize();

				m_Panel[m_MatSize].m_bIsInSymPlane  = false;
				m_Panel[m_MatSize].m_bIsLeading     = false;
				m_Panel[m_MatSize].m_bIsTrailing    = false;
				m_Panel[m_MatSize].m_iPos     = 100;
				m_Panel[m_MatSize].m_iElement = m_MatSize;
				m_Panel[m_MatSize].m_iSym     = -1;
				m_Panel[m_MatSize].m_bIsLeftPanel  = true;
				m_Panel[m_MatSize].SetFrame(LA, LB, TA, TB);

				// set neighbour panels

				m_Panel[m_MatSize].m_iPD = m_MatSize + nh;
				m_Panel[m_MatSize].m_iPU = m_MatSize - nh;

				if(k==0)    m_Panel[m_MatSize].m_iPU = -1;// no panel downstream
				if(k==nx-1)	m_Panel[m_MatSize].m_iPD = -1;// no panel upstream

				m_Panel[m_MatSize].m_iPL = m_MatSize + 1;
				m_Panel[m_MatSize].m_iPR = m_MatSize - 1;

				if(l==0)     m_Panel[m_MatSize].m_iPR = InitialSize + FullSize - p - 1;
				if(l==nh-1)  m_Panel[m_MatSize].m_iPL = InitialSize + FullSize - p - 1;

				m_MatSize++;
				p++;
				LB = LA;
				TB = TA;
			}
		}
	}

	//right side next
	i = m_MatSize;
	//right side next
	for (k=nx-1; k>=0; k--)
	{
		for (l=nh-1; l>=0; l--)
		{
			i--;
			LA = m_Node[m_Panel[i].m_iLB];
			TA = m_Node[m_Panel[i].m_iTB];
			LB = m_Node[m_Panel[i].m_iLA];
			TB = m_Node[m_Panel[i].m_iTA];

			LA.y = -LA.y;
			LB.y = -LB.y;
			TA.y = -TA.y;
			TB.y = -TB.y;

			n0 = IsNode(LA);
			n1 = IsNode(TA);
			n2 = IsNode(LB);
			n3 = IsNode(TB);

			if(n0>=0) {
				m_Panel[m_MatSize].m_iLA = n0;
			}
			else {
				m_Panel[m_MatSize].m_iLA = m_nNodes;
				m_Node[m_nNodes].Copy(LA);
				m_nNodes++;
			}

			if(n1>=0) {
				m_Panel[m_MatSize].m_iTA = n1;
			}
			else {
				m_Panel[m_MatSize].m_iTA = m_nNodes;
				m_Node[m_nNodes].Copy(TA);
				m_nNodes++;
			}

			if(n2>=0) {
				m_Panel[m_MatSize].m_iLB = n2;
			}
			else {
				m_Panel[m_MatSize].m_iLB = m_nNodes;
				m_Node[m_nNodes].Copy(LB);
				m_nNodes++;
			}

			if(n3 >=0) {
				m_Panel[m_MatSize].m_iTB = n3;
			}
			else {
				m_Panel[m_MatSize].m_iTB = m_nNodes;
				m_Node[m_nNodes].Copy(TB);
				m_nNodes++;
			}

			LATB = TB - LA;
			TALB = LB - TA;
			m_Panel[m_MatSize].Normal = LATB * TALB;
			m_Panel[m_MatSize].Area =  m_Panel[m_MatSize].Normal.VAbs()/2.0;
			m_Panel[m_MatSize].Normal.Normalize();

			m_Panel[m_MatSize].m_bIsInSymPlane  = false;
			m_Panel[m_MatSize].m_bIsLeading     = false;
			m_Panel[m_MatSize].m_bIsTrailing    = false;
			m_Panel[m_MatSize].m_iPos = 100;
			m_Panel[m_MatSize].m_iElement = m_MatSize;
			m_Panel[m_MatSize].m_iSym = -1;
			m_Panel[i].m_iSym = m_MatSize;
			m_Panel[m_MatSize].m_bIsLeftPanel  = false;
			m_Panel[m_MatSize].SetFrame(LA, LB, TA, TB);

			// set neighbour panels
			// valid only for Panel Analysis

			m_Panel[m_MatSize].m_iPD = m_MatSize - nh;
			m_Panel[m_MatSize].m_iPU = m_MatSize + nh;

			if(k==0)	m_Panel[m_MatSize].m_iPU = -1;// no panel downstream
			if(k==nx-1)	m_Panel[m_MatSize].m_iPD = -1;// no panel upstream

			m_Panel[m_MatSize].m_iPL = m_MatSize + 1;
			m_Panel[m_MatSize].m_iPR = m_MatSize - 1;

			if(l==0)     m_Panel[m_MatSize].m_iPL = InitialSize + FullSize - p - 1;
			if(l==nh-1)  m_Panel[m_MatSize].m_iPR = InitialSize + FullSize - p - 1;

			m_MatSize++;
			p++;
			LB = LA;
			TB = TA;
		}
	}
	m_pCurBody->m_NElements = m_MatSize-InitialSize;
	return m_pCurBody->m_NElements;
}


int QMiarex::CreateElements(CSurface *pSurface)
{
	//
	// Creates the panel elements that will be used either by the VLM or the Panel method.
	// The panels are created from left to right on a surface
	// The panels are created depending on the current WPolar:
	// No WPolar --> panel elements on top & bottom surfaces, just for cosmetics
	// VLM       --> panel elements on mid camber line from T.E. to L.E.
	// 3D Panels --> panel elements on left tip surface
	//               panel elements on each strip, starting on the bottom T.E. to the L.E back to the opt T.E
	//               panel elements on right tip surface
	
	//TODO : for  a gap at the wing's center, need to separate m_iPL and m_iPR at the tips;
	bool bNoJoinFlap ;
	int k,l;
	int n0, n1, n2, n3;

	int InitialSize = m_MatSize;
	int side;
	CVector LA, LB, TA, TB;

	if ((!m_pCurWPolar && pSurface->m_bIsTipLeft) ||
		( m_pCurWPolar && m_pCurWPolar->m_AnalysisMethod==PANELMETHOD && pSurface->m_bIsTipLeft && !m_pCurWPolar->m_bThinSurfaces))
	{
		//then left tip surface, add side panels
		for (l=0; l< pSurface->m_NXPanels; l++)
		{
			m_Panel[m_MatSize].m_bIsLeading     = false;
			m_Panel[m_MatSize].m_bIsTrailing    = false;
			m_Panel[m_MatSize].m_bIsWakePanel   = false;
			m_Panel[m_MatSize].m_bIsInSymPlane  = false; //even if part of a fin

			pSurface->GetPanel(0, l, -1);
			LA.Copy(pSurface->LA);
			TA.Copy(pSurface->TA);

			pSurface->GetPanel(0, l, +1);
			LB.Copy(pSurface->LA);
			TB.Copy(pSurface->TA);

			n0 = IsNode(LA);
			if(n0>=0) 	m_Panel[m_MatSize].m_iLA = n0;
			else {
				m_Panel[m_MatSize].m_iLA = m_nNodes;
				m_Node[m_nNodes].Copy(LA);
				m_nNodes++;
			}

			n1 = IsNode(TA);
			if(n1>=0) 	m_Panel[m_MatSize].m_iTA = n1;
			else {
				m_Panel[m_MatSize].m_iTA = m_nNodes;
				m_Node[m_nNodes].Copy(TA);
				m_nNodes++;
			}

			n2 = IsNode(LB);
			if(n2>=0) 	m_Panel[m_MatSize].m_iLB = n2;
			else {
				m_Panel[m_MatSize].m_iLB = m_nNodes;
				m_Node[m_nNodes].Copy(LB);
				m_nNodes++;
			}

			n3 = IsNode(TB);
			if(n3>=0) 	m_Panel[m_MatSize].m_iTB = n3;
			else {
				m_Panel[m_MatSize].m_iTB = m_nNodes;
				m_Node[m_nNodes].Copy(TB);
				m_nNodes++;
			}

			m_Panel[m_MatSize].m_iPos = 50;
			m_Panel[m_MatSize].m_iElement = m_MatSize;
			m_Panel[m_MatSize].m_iSym  = -1;
			m_Panel[m_MatSize].m_bIsLeftPanel  = pSurface->m_bIsLeftSurf;
			m_Panel[m_MatSize].SetFrame(LA, LB, TA, TB);
			m_Panel[m_MatSize].m_iWake = -1;

			m_Panel[m_MatSize].m_iPD = m_MatSize-1;
			m_Panel[m_MatSize].m_iPU = m_MatSize+1;
			if(l==0)                      m_Panel[m_MatSize].m_iPD = -1;// no panel downstream
			if(l==pSurface->m_NXPanels-1) m_Panel[m_MatSize].m_iPU = -1;// no panel upstream
			m_Panel[m_MatSize].m_iPL = -1;
			m_Panel[m_MatSize].m_iPR = -1;

			m_MatSize++;
		}
	}

	for (k=0; k<pSurface->m_NYPanels; k++)
	{
		//add "horizontal" panels, mid side, or following a strip from bot to top if 3D Panel
		if(!m_pCurWPolar)                        side = -1;
		else if(m_pCurWPolar->m_AnalysisMethod==LLTMETHOD) side =  0;
		else
		{
			if(m_pCurWPolar->m_bThinSurfaces)
				side = 0;
			else if (m_pCurWPolar->m_AnalysisMethod==PANELMETHOD)
				side =-1; //start with lower surf, as recommended by K&P
		}
		//from T.E. to L.E.
		for (l=0; l<pSurface->m_NXPanels; l++)
		{
			pSurface->GetPanel(k,l,side);

			n0 = IsNode(pSurface->LA);
			n1 = IsNode(pSurface->TA);
			n2 = IsNode(pSurface->LB);
			n3 = IsNode(pSurface->TB);

			if(l==0)                      m_Panel[m_MatSize].m_bIsTrailing = true;
			if(l==pSurface->m_NXPanels-1) m_Panel[m_MatSize].m_bIsLeading  = true;
			m_Panel[m_MatSize].m_bIsWakePanel   = false;
			m_Panel[m_MatSize].m_bIsInSymPlane  = pSurface->m_bIsInSymPlane;

			bNoJoinFlap = side==0 &&  l<pSurface->m_NXFlap && k==0;

			if(n0>=0 && !bNoJoinFlap) // do not merge nodes if we are at a flap's side in VLM
			{
				m_Panel[m_MatSize].m_iLA = n0;
			}
			else {
				m_Panel[m_MatSize].m_iLA = m_nNodes;
				m_Node[m_nNodes].Copy(pSurface->LA);
				m_nNodes++;
			}

			if(n1>=0 && !bNoJoinFlap) // do not merge nodes if we are at a flap's side in VLM
			{
				m_Panel[m_MatSize].m_iTA = n1;
			}
			else {
				m_Panel[m_MatSize].m_iTA = m_nNodes;
				m_Node[m_nNodes].Copy(pSurface->TA);
				m_nNodes++;
			}

			bNoJoinFlap = side==0 &&  l<pSurface->m_NXFlap && k==pSurface->m_NYPanels-1;

			if(n2>=0 && !bNoJoinFlap) // do not merge nodes if we are at a flap's side in VLM
			{
				m_Panel[m_MatSize].m_iLB = n2;
			}
			else {
				m_Panel[m_MatSize].m_iLB = m_nNodes;
				m_Node[m_nNodes].Copy(pSurface->LB);
				m_nNodes++;
			}

			if(n3>=0 && !bNoJoinFlap) // do not merge nodes if we are at a flap's side in VLM
			{
				m_Panel[m_MatSize].m_iTB = n3;
			}
			else {
				m_Panel[m_MatSize].m_iTB = m_nNodes;
				m_Node[m_nNodes].Copy(pSurface->TB);
				m_nNodes++;
			}

			m_Panel[m_MatSize].m_iPos = side;
			m_Panel[m_MatSize].m_iElement = m_MatSize;
			m_Panel[m_MatSize].m_iSym  = -1;
			m_Panel[m_MatSize].m_bIsLeftPanel  = pSurface->m_bIsLeftSurf;

			if(side==0)         m_Panel[m_MatSize].SetFrame(pSurface->LA, pSurface->LB, pSurface->TA, pSurface->TB);
			else if (side==-1)  m_Panel[m_MatSize].SetFrame(pSurface->LB, pSurface->LA, pSurface->TB, pSurface->TA);

			// set neighbour panels
			// valid only for Panel 2-sided Analysis
			// we are on the bottom or middle surface
			m_Panel[m_MatSize].m_iPD = m_MatSize-1;
			m_Panel[m_MatSize].m_iPU = m_MatSize+1;
			if(l==0)                                 m_Panel[m_MatSize].m_iPD = -1;// no panel downstream
			if(l==pSurface->m_NXPanels-1 && side==0) m_Panel[m_MatSize].m_iPU = -1;// no panel upstream

			if(side !=0)
			{
				//wings are modelled as thick surfaces
				m_Panel[m_MatSize].m_iPL = m_MatSize + 2*pSurface->m_NXPanels;
				m_Panel[m_MatSize].m_iPR = m_MatSize - 2*pSurface->m_NXPanels;
				//todo : do not link to right wing if there is a gap in between
				if(k==0                      && pSurface->m_bIsTipLeft)		m_Panel[m_MatSize].m_iPR = -1;
				if(k==pSurface->m_NYPanels-1 && pSurface->m_bIsTipRight)	m_Panel[m_MatSize].m_iPL = -1;
			}
			else
			{
				//wings are modelled as thin surfaces
				m_Panel[m_MatSize].m_iPR = m_MatSize + pSurface->m_NXPanels;
				m_Panel[m_MatSize].m_iPL = m_MatSize - pSurface->m_NXPanels;
				if(k==0                      && pSurface->m_bIsTipLeft)		m_Panel[m_MatSize].m_iPL = -1;
				if(k==pSurface->m_NYPanels-1 && pSurface->m_bIsTipRight)	m_Panel[m_MatSize].m_iPR = -1;
			}

			//do not link to next surfaces... will be done in JoinSurfaces() if surfaces are continuous
			if(k==0)                      m_Panel[m_MatSize].m_iPR = -1;
			if(k==pSurface->m_NYPanels-1) m_Panel[m_MatSize].m_iPL = -1;

			if(m_pCurWPolar && m_Panel[m_MatSize].m_bIsTrailing && m_pCurWPolar->m_AnalysisMethod>=2)
			{
				m_Panel[m_MatSize].m_iWake = m_WakeSize;//next wake element
				m_Panel[m_MatSize].m_iWakeColumn = m_NWakeColumn;
				if(m_pCurWPolar->m_bThinSurfaces)
				{
					CreateWakeElems(m_MatSize);
					m_NWakeColumn++;
				}
			}

			if(l<pSurface->m_NXFlap) pSurface->AddFlapPanel(m_Panel+m_MatSize);

			m_MatSize++;
		}

		if (!m_pCurWPolar ||
			(m_pCurWPolar && m_pCurWPolar->m_AnalysisMethod==PANELMETHOD && !m_pCurWPolar->m_bThinSurfaces))
		{ //add top side if 3D Panels

			side = 1; //next upper surf, as recommended by K&P
			//from L.E. to T.E.
			for (l=pSurface->m_NXPanels-1;l>=0; l--)
			{
				pSurface->GetPanel(k,l,side);
				n0 = IsNode(pSurface->LA);
				n1 = IsNode(pSurface->TA);
				n2 = IsNode(pSurface->LB);
				n3 = IsNode(pSurface->TB);

				if(l==0)                      m_Panel[m_MatSize].m_bIsTrailing = true;
				if(l==pSurface->m_NXPanels-1) m_Panel[m_MatSize].m_bIsLeading  = true;
				m_Panel[m_MatSize].m_bIsWakePanel   = false;
				m_Panel[m_MatSize].m_bIsInSymPlane  = pSurface->m_bIsInSymPlane;

				if(n0>=0)
					m_Panel[m_MatSize].m_iLA = n0;
				else {
					m_Panel[m_MatSize].m_iLA = m_nNodes;
					m_Node[m_nNodes].Copy(pSurface->LA);
					m_nNodes++;
				}

				if(n1>=0)
					m_Panel[m_MatSize].m_iTA = n1;
				else {
					m_Panel[m_MatSize].m_iTA = m_nNodes;
					m_Node[m_nNodes].Copy(pSurface->TA);
					m_nNodes++;
				}

				if(n2>=0)
					m_Panel[m_MatSize].m_iLB = n2;
				else {
					m_Panel[m_MatSize].m_iLB = m_nNodes;
					m_Node[m_nNodes].Copy(pSurface->LB);
					m_nNodes++;
				}

				if(n3 >=0)
					m_Panel[m_MatSize].m_iTB = n3;
				else {
					m_Panel[m_MatSize].m_iTB = m_nNodes;
					m_Node[m_nNodes].Copy(pSurface->TB);
					m_nNodes++;
				}

				m_Panel[m_MatSize].m_iPos = side;
				m_Panel[m_MatSize].m_iElement = m_MatSize;
				m_Panel[m_MatSize].m_iSym  = -1;
				m_Panel[m_MatSize].m_bIsLeftPanel  = pSurface->m_bIsLeftSurf;

				m_Panel[m_MatSize].SetFrame(pSurface->LA, pSurface->LB, pSurface->TA, pSurface->TB);

				// set neighbour panels
				// valid only for Panel 2-sided Analysis
				// we are on the top surface
				m_Panel[m_MatSize].m_iPD = m_MatSize+1;
				m_Panel[m_MatSize].m_iPU = m_MatSize-1;
				if(l==0)                      m_Panel[m_MatSize].m_iPD = -1;// no panel downstream
//				if(l==pSurface->m_NXPanels-1) m_Panel[m_MatSize].m_iPU = -1;// no panel upstream

				m_Panel[m_MatSize].m_iPL = m_MatSize - 2*pSurface->m_NXPanels;//assuming all wing panels have same chordwise distribution
				m_Panel[m_MatSize].m_iPR = m_MatSize + 2*pSurface->m_NXPanels;//assuming all wing panels have same chordwise distribution

				if(k==0                      && pSurface->m_bIsTipLeft)		m_Panel[m_MatSize].m_iPL = -1;
				if(k==pSurface->m_NYPanels-1 && pSurface->m_bIsTipRight)	m_Panel[m_MatSize].m_iPR = -1;

				//do not link to next surfaces... will be done in JoinSurfaces() if surfaces are continuous
				if(k==0)                      m_Panel[m_MatSize].m_iPL = -1;
				if(k==pSurface->m_NYPanels-1) m_Panel[m_MatSize].m_iPR = -1;


				if(m_pCurWPolar && m_Panel[m_MatSize].m_bIsTrailing && m_pCurWPolar->m_AnalysisMethod==PANELMETHOD)
				{
					m_Panel[m_MatSize].m_iWake = m_WakeSize;//next wake element
					m_Panel[m_MatSize].m_iWakeColumn = m_NWakeColumn;
					CreateWakeElems(m_MatSize);
				}

				if(l<pSurface->m_NXFlap) pSurface->AddFlapPanel(m_Panel+m_MatSize);
				m_MatSize++;
			}
			m_NWakeColumn++;
		}
	}

	if ((!m_pCurWPolar && pSurface->m_bIsTipRight) ||
		( m_pCurWPolar && m_pCurWPolar->m_AnalysisMethod==PANELMETHOD && pSurface->m_bIsTipRight && !m_pCurWPolar->m_bThinSurfaces))
	{	//right tip surface
		k = pSurface->m_NYPanels-1;
		for (l=0; l< pSurface->m_NXPanels; l++)
		{
			m_Panel[m_MatSize].m_bIsTrailing    = false;
			m_Panel[m_MatSize].m_bIsLeading     = false;
			m_Panel[m_MatSize].m_bIsWakePanel   = false;
			m_Panel[m_MatSize].m_bIsInSymPlane  = false;//even if part of a fin

			pSurface->GetPanel(k,l,1);
			LA.Copy(pSurface->LB);
			TA.Copy(pSurface->TB);

			pSurface->GetPanel(k,l,-1);
			LB.Copy(pSurface->LB);
			TB.Copy(pSurface->TB);

			n0 = IsNode(LA);//answer should be yes
			if(n0>=0) 				m_Panel[m_MatSize].m_iLA = n0;
			else {
				m_Panel[m_MatSize].m_iLA = m_nNodes;
				m_Node[m_nNodes].Copy(LA);
				m_nNodes++;
			}
			n1 = IsNode(TA);//answer should be yes
			if(n1>=0) 				m_Panel[m_MatSize].m_iTA = n1;
			else {
				m_Panel[m_MatSize].m_iTA = m_nNodes;
				m_Node[m_nNodes].Copy(TA);
				m_nNodes++;
			}
			n2 = IsNode(LB);//answer should be yes
			if(n2>=0) 				m_Panel[m_MatSize].m_iLB = n2;
			else {
				m_Panel[m_MatSize].m_iLB = m_nNodes;
				m_Node[m_nNodes].Copy(LB);
				m_nNodes++;
			}
			n3 = IsNode(TB);//answer should be yes
			if(n3>=0)				m_Panel[m_MatSize].m_iTB = n3;
			else {
				m_Panel[m_MatSize].m_iTB = m_nNodes;
				m_Node[m_nNodes].Copy(TB);
				m_nNodes++;
			}


			m_Panel[m_MatSize].m_iPD = m_MatSize-1;
			m_Panel[m_MatSize].m_iPU = m_MatSize+1;
			if(l==0)                      m_Panel[m_MatSize].m_iPD = -1;// no panel downstream
			if(l==pSurface->m_NXPanels-1) m_Panel[m_MatSize].m_iPU = -1;// no panel upstream
			m_Panel[m_MatSize].m_iPL = -1;
			m_Panel[m_MatSize].m_iPR = -1;

			m_Panel[m_MatSize].m_iPos = 50;
			m_Panel[m_MatSize].m_iElement = m_MatSize;
			m_Panel[m_MatSize].m_iSym  = -1;
			m_Panel[m_MatSize].m_bIsLeftPanel  = pSurface->m_bIsLeftSurf;
			m_Panel[m_MatSize].SetFrame(LA, LB, TA, TB);
			m_Panel[m_MatSize].m_iWake = -1;
			m_MatSize++;
		}
	}

	pSurface->m_NElements = m_MatSize-InitialSize;
	return pSurface->m_NElements;
}


bool QMiarex::CreateWakeElems(int PanelIndex)
{
	//
	// Creates the wake elements for the 3D panel method.
	//
	if(!m_pCurWPolar) return false;
	if(!m_Panel[PanelIndex].m_bIsTrailing) return false;
	//creates elements trailing panel p
	int l, n0, n1,n2, n3;
	double dxA, dxB;
	int mw = m_WakeSize;// number of wake panels
	CVector N, LATB, TALB;
	CVector LA, LB, TA,TB;//wake panel's corner points

	int NXWakePanels;
	double WakePanelFactor;
	double TotalWakeLength;

//	double slope = m_pSurface[0]->m_pFoilA->GetCamberSlope(1.0);
//	CVector TE(1.0,0.0,slope);
//	TE.Normalize();

	NXWakePanels    = m_pCurWPolar->m_NXWakePanels;
	WakePanelFactor = m_pCurWPolar->m_WakePanelFactor;
	TotalWakeLength = m_pCurWPolar->m_TotalWakeLength;

	TA = m_Node[m_Panel[PanelIndex].m_iTA];
	TB = m_Node[m_Panel[PanelIndex].m_iTB];
	dxA = TotalWakeLength*m_pCurWing->m_MAChord - m_Node[m_Panel[PanelIndex].m_iTA].x;
	dxB = TotalWakeLength*m_pCurWing->m_MAChord - m_Node[m_Panel[PanelIndex].m_iTB].x;

	if(WakePanelFactor==1.0)
	{
		dxA /= NXWakePanels;
		dxB /= NXWakePanels;
	}
	else
	{
		dxA *= (1.0-WakePanelFactor)/(1.0-pow(WakePanelFactor, NXWakePanels));
		dxB *= (1.0-WakePanelFactor)/(1.0-pow(WakePanelFactor, NXWakePanels));
	}

	for (l=0; l<NXWakePanels; l++)
	{
		LA = TA;
		LB = TB;
		TA.x += dxA;
		TB.x += dxB;
//		TA += TE * dxA;
//		TB += TE * dxB;
		dxA *= WakePanelFactor;
		dxB *= WakePanelFactor;

		n0 = IsWakeNode(LA);
		n1 = IsWakeNode(TA);
		n2 = IsWakeNode(LB);
		n3 = IsWakeNode(TB);

		if(n0>=0) {
			m_WakePanel[mw].m_iLA = n0;
		}
		else {
			m_WakePanel[mw].m_iLA = m_nWakeNodes;
			m_WakeNode[m_nWakeNodes].Copy(LA);
			m_nWakeNodes++;
		}

		if(n1>=0) {
			m_WakePanel[mw].m_iTA = n1;
		}
		else {
			m_WakePanel[mw].m_iTA = m_nWakeNodes;
			m_WakeNode[m_nWakeNodes].Copy(TA);
			m_nWakeNodes++;
		}

		if(n2>=0) {
			m_WakePanel[mw].m_iLB = n2;
		}
		else {
			m_WakePanel[mw].m_iLB = m_nWakeNodes;
			m_WakeNode[m_nWakeNodes].Copy(LB);
			m_nWakeNodes++;
		}

		if(n3 >=0) {
			m_WakePanel[mw].m_iTB = n3;
		}
		else {
			m_WakePanel[mw].m_iTB = m_nWakeNodes;
			m_WakeNode[m_nWakeNodes].Copy(TB);
			m_nWakeNodes++;
		}

		LATB = TB - LA;
		TALB = LB - TA;

		m_WakePanel[mw].Normal = LATB * TALB;

		m_WakePanel[mw].m_iPos = 0;
		m_WakePanel[mw].m_bIsWakePanel = true;
		m_WakePanel[mw].Area =  m_WakePanel[mw].Normal.VAbs()/2.0;
		m_WakePanel[mw].Normal.Normalize();
		m_WakePanel[mw].SetFrame(LA,LB, TA, TB);
		m_WakePanel[mw].m_iSym     = -1;
		m_WakePanel[mw].m_bIsLeftPanel  = false;

		if(l==0)					m_WakePanel[mw].m_iPD = -1;// no panel downstream
		else if(l==NXWakePanels)	m_WakePanel[mw].m_iPU = -1;// no panel upstream
		else						m_WakePanel[mw].m_iPD = mw+1;

		m_WakePanel[mw].m_iPL = -1;
		m_WakePanel[mw].m_iPR = -1;

		mw++;
	}

	m_WakeSize = mw;
	return true;
}



void QMiarex::CreateWOpp(CWOpp *pWOpp, CWing *pWing)
{
	//
	// Creates the operating point associated to a plane's wing pointed by pWing
	// Uses the wing's geometric data
	// and the current instance of the LLT, VLM or Panel analysis dialog box
	// Returns the pointer pWOpp to the operating point which has been created
	//
	int i,j;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	CWOpp *pOldWOpp = NULL;
	bool bFound;

	for(i=0; i<30;i++)
	{
		bFound = false;
		for (j=0; j<m_poaWOpp->size();j++)
		{
			pOldWOpp = (CWOpp*)m_poaWOpp->at(j);
			if(pOldWOpp->m_Color == pMainFrame->m_crColors[i]) bFound = true;
		}
		if(!bFound)
		{
			pWOpp->m_Color = pMainFrame->m_crColors[i];
			break;
		}
	}
	
	pWOpp->m_WingName            = pWing->m_WingName;
	pWOpp->m_NStation            = pWing->m_NStation;
	pWOpp->m_NVLMPanels          = pWing->m_MatSize;
	pWOpp->m_PlrName             = m_pCurWPolar->m_PlrName;
	pWOpp->m_Type                = m_pCurWPolar->m_Type;
	pWOpp->m_bVLM1               = m_pCurWPolar->m_bVLM1;
	pWOpp->m_bThinSurface        = m_pCurWPolar->m_bThinSurfaces;
	pWOpp->m_bTiltedGeom         = m_pCurWPolar->m_bTiltedGeom;
	pWOpp->m_AnalysisMethod      = m_pCurWPolar->m_AnalysisMethod;

	if(m_pCurWPolar->m_AnalysisMethod==PANELMETHOD && m_pPanelDlg)
	{
		pWOpp->m_bOut                = m_pPanelDlg->m_bPointOut;
		pWOpp->m_Alpha               = m_pPanelDlg->m_Alpha;
		pWOpp->m_QInf                = m_pPanelDlg->m_QInf;
	}

	pWOpp->m_Beta                = m_pCurWPolar->m_Beta;
	pWOpp->m_Phi                 = m_pCurWPolar->m_BankAngle;

	pWOpp->m_Weight              = m_pCurWPolar->m_Mass;
	pWOpp->m_Span                = m_pCurWPolar->m_WSpan;
	pWOpp->m_MAChord             = pWing->m_MAChord;
	pWOpp->m_CL                  = pWing->m_CL;
	pWOpp->m_ICD                 = pWing->m_InducedDrag;
	pWOpp->m_VCD                 = pWing->m_ViscousDrag;

	pWOpp->m_GCm                 = pWing->m_GCm;
	pWOpp->m_VCm                 = pWing->m_VCm;
	pWOpp->m_ICm                 = pWing->m_ICm;
	pWOpp->m_GRm                 = pWing->m_GRm;
	pWOpp->m_GYm                 = pWing->m_GYm;
	pWOpp->m_VYm                 = pWing->m_VYm;
	pWOpp->m_IYm                 = pWing->m_IYm;

	pWOpp->m_XCP                 = pWing->m_XCP;
	pWOpp->m_YCP                 = pWing->m_YCP;

	memcpy(pWOpp->m_Ai,            pWing->m_Ai,            sizeof(pWing->m_Ai));
	memcpy(pWOpp->m_Cl,            pWing->m_Cl,            sizeof(pWing->m_Cl));
	memcpy(pWOpp->m_PCd,           pWing->m_PCd,           sizeof(pWing->m_PCd));
	memcpy(pWOpp->m_ICd,           pWing->m_ICd,           sizeof(pWing->m_ICd));
	memcpy(pWOpp->m_Cm,            pWing->m_Cm,            sizeof(pWing->m_Cm));
	memcpy(pWOpp->m_CmAirf,        pWing->m_CmAirf,        sizeof(pWing->m_CmAirf));
	memcpy(pWOpp->m_XCPSpanRel,    pWing->m_XCPSpanRel,    sizeof(pWing->m_XCPSpanRel));
	memcpy(pWOpp->m_XCPSpanAbs,    pWing->m_XCPSpanAbs,    sizeof(pWing->m_XCPSpanAbs));
	memcpy(pWOpp->m_Re,            pWing->m_Re,            sizeof(pWing->m_Re));
	memcpy(pWOpp->m_Chord,         pWing->m_Chord,         sizeof(pWing->m_Chord));
	memcpy(pWOpp->m_Twist,         pWing->m_Twist,         sizeof(pWing->m_Twist));
	memcpy(pWOpp->m_XTrTop,        pWing->m_XTrTop,        sizeof(pWing->m_XTrTop));
	memcpy(pWOpp->m_XTrBot,        pWing->m_XTrBot,        sizeof(pWing->m_XTrBot));
	memcpy(pWOpp->m_BendingMoment, pWing->m_BendingMoment, sizeof(pWing->m_BendingMoment));
	memcpy(pWOpp->m_Vd,            pWing->m_Vd,            sizeof(pWOpp->m_Vd));
	memcpy(pWOpp->m_F,             pWing->m_F,             sizeof(pWOpp->m_F));

	double Cb =0.0;

	for (int l=0; l<pWing->m_NStation; l++)
	{
		pWOpp->m_SpanPos[l]   = pWing->m_SpanPos[l];
		pWOpp->m_StripArea[l] = pWing->m_StripArea[l];
		Cb = qMax(Cb, pWing->m_BendingMoment[l]);
	}
	pWOpp->m_MaxBending = Cb;
}

#define MAXGRAPHS 4
void QMiarex::CreateWOppCurves()
{
	//
	// Creates the Graph curves for the graphs in the operating point view
	//

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	
	CWOpp *pWOpp = NULL;;
	CPOpp *pPOpp = NULL;
	CCurve *pWingCurve[MAXWINGS][MAXGRAPHS];
	QString str;
	int i,k;

	for(int ig=0; ig<4; ig++) m_WingGraph[ig].DeleteCurves();

	// Browse through the array of wing operating points
	// add a curve for those selected, and fill them with data
	for (k=0; k<m_poaWOpp->size(); k++)
	{
		pWOpp = (CWOpp*)m_poaWOpp->at(k);
		if (pWOpp->m_bIsVisible && (!m_bCurWOppOnly || (m_pCurWOpp==pWOpp)))
		{
			str = QString("Q=%1 - Alpha=%2").arg(pWOpp->m_QInf,5,'f',2).arg(pWOpp->m_Alpha,5,'f',2);
			for(int ic=0; ic<MAXGRAPHS; ic++)
			{
				pWingCurve[0][ic] = m_WingGraph[ic].AddCurve();
				pWingCurve[0][ic]->ShowPoints(pWOpp->m_bShowPoints);
				pWingCurve[0][ic]->SetStyle(pWOpp->m_Style);
				pWingCurve[0][ic]->SetColor(pWOpp->m_Color);
				pWingCurve[0][ic]->SetWidth(pWOpp->m_Width);	
				pWingCurve[0][ic]->SetTitle(str);	
				FillWOppCurve(pWOpp, m_WingGraph+ic, pWingCurve[0][ic]);
			}
		}
	}
	// Browse through the array of plane operating points
	// add a curve for those selected, and fill them with data
	for (k=0; k<m_poaPOpp->size(); k++)
	{
		pPOpp = (CPOpp*)m_poaPOpp->at(k);
		if (pPOpp->m_bIsVisible && (!m_bCurWOppOnly || (m_pCurPOpp==pPOpp)))
		{
			for(int iw=0; iw<MAXWINGS; iw++)
			{
				if(m_bShowWingCurve[iw] && pPOpp->m_bWing[iw])
				{
					for(int ic=0; ic<MAXGRAPHS; ic++)
					{
						pWingCurve[iw][ic]  = m_WingGraph[ic].AddCurve();
						pWingCurve[iw][ic]->ShowPoints(pPOpp->m_bShowPoints);
						pWingCurve[iw][ic]->SetStyle(pPOpp->m_Style);
						pWingCurve[iw][ic]->SetColor(pPOpp->m_Color);
						pWingCurve[iw][ic]->SetWidth(pPOpp->m_Width);
						pWingCurve[iw][ic]->SetTitle(str);
						FillWOppCurve(&pPOpp->m_PlaneWOpp[iw], m_WingGraph+ic, pWingCurve[iw][ic]);
					}
				}
			}
		}
	}
	
	//if the optimal elliptic curve is requested, and if the graph variable is local lift, then add the curve
	if(m_bShowElliptic && m_pCurWOpp)
	{
		double maxlift, x, y;
		maxlift = m_pCurWOpp->GetMaxLift();
		int nStart;
		if(m_pCurWOpp->m_AnalysisMethod==LLTMETHOD) nStart = 1;
		else nStart = 0;

		for(int ig=0; ig<MAXGRAPHS; ig++)
		{
			if(m_WingGraph[ig].GetYVariable()==3)
			{
				CCurve *pCurve = m_WingGraph[ig].AddCurve();
				pCurve->SetStyle(1);
				pCurve->SetColor(QColor(150, 150, 150));
				for (i=nStart; i<m_pCurWOpp->m_NStation; i++)
				{
					x = m_pCurWOpp->m_SpanPos[i];
					y = maxlift*sqrt(1.0-x*x/m_pCurWOpp->m_Span/m_pCurWOpp->m_Span*4.0);
					pCurve->AddPoint(x*pMainFrame->m_mtoUnit,y);
				}
			}
		}
	}
}


void QMiarex::CreateWPolarCurves()
{
	//
	//resets and creates the WPolar graphs curves
	//
	CWPolar *pWPolar;
	CCurve *pCurve[4];

	for(int ig=0; ig<4; ig++) m_WPlrGraph[ig].DeleteCurves();

	for (int k=0; k<m_poaWPolar->size(); k++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(k);
		if (pWPolar->m_bIsVisible && pWPolar->m_Alpha.size()>0 &&
		   ((m_bType1 && pWPolar->m_Type == FIXEDSPEEDPOLAR) ||
			(m_bType2 && pWPolar->m_Type == FIXEDLIFTPOLAR) ||
			(m_bType4 && pWPolar->m_Type == FIXEDAOAPOLAR) ||
			(m_bType5 && pWPolar->m_Type == 5) ||
			(m_bType6 && pWPolar->m_Type == 6) ||
			(m_bType7 && pWPolar->m_Type == STABILITYPOLAR)))
		{

			for(int ig=0; ig<4; ig++)
			{
				pCurve[ig] = m_WPlrGraph[ig].AddCurve();
				pCurve[ig]->ShowPoints(pWPolar->m_bShowPoints);
				pCurve[ig]->SetStyle(pWPolar->m_Style);
				pCurve[ig]->SetColor(pWPolar->m_Color);
				pCurve[ig]->SetWidth(pWPolar->m_Width);
				FillWPlrCurve(pCurve[ig], pWPolar, m_WPlrGraph[ig].GetXVariable(), m_WPlrGraph[ig].GetYVariable());
				pCurve[ig]->SetTitle(pWPolar->m_PlrName);
			}
		}
	}
}


void QMiarex::CreateStabilityCurves()
{
	//
	// Creates the curves of the stability graphs
	//
	if(m_iStabilityView==STABTIMEVIEW)
	{
		if(m_StabilityResponseType==1)  CreateStabRungeKuttaCurves();
		else                            CreateStabTimeCurves();
	}
	else
	{
		CreateStabRLCurves();
	}
}


void QMiarex::CreateStabTimeCurves()
{
	//
	// Builds the initial condition response due to perturbations from steady state
	// The time response is calculated analytically based on the knowledge 
	// of the eigenvalues and eigenvectors
	//

	complex<double> M[16];// the modal matrix
	complex<double> InvM[16];// the inverse of the modal matrix
	complex<double> q[4],q0[4],y[4];//the part of each mode in the solution
	int i,j,k;
	double t, dt, TotalPoints; // the input load
	complex<double> in[4];
	CCurve *pCurve0, *pCurve1, *pCurve2, *pCurve3;
	QString strong, CurveTitle;

	m_bBreak = false;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
	CurveTitle = pStabView->m_pctrlCurveList->currentText();
	
	pCurve0 = m_TimeGraph[0].GetCurve(CurveTitle);
	if(pCurve0) pCurve0->ResetCurve();
	else return;
	pCurve1 = m_TimeGraph[1].GetCurve(CurveTitle);
	if(pCurve1) pCurve1->ResetCurve();
	else return;
	pCurve2 = m_TimeGraph[2].GetCurve(CurveTitle);
	if(pCurve2) pCurve2->ResetCurve();
	else return;
	pCurve3 = m_TimeGraph[3].GetCurve(CurveTitle);
	if(pCurve3) pCurve3->ResetCurve();
	else return;
	
	if(!m_pCurWOpp || !m_pCurWOpp->m_bIsVisible) return;

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
		in[0] = m_pCurWOpp->m_EigenVector[pStabView->m_iCurrentMode][0];
		in[1] = m_pCurWOpp->m_EigenVector[pStabView->m_iCurrentMode][1];
		in[2] = m_pCurWOpp->m_EigenVector[pStabView->m_iCurrentMode][2];
		in[3] = m_pCurWOpp->m_EigenVector[pStabView->m_iCurrentMode][3];
	}	
	
	//fill the modal matrix
	if(m_bLongitudinal) k=0; else k=1;
	for (i=0; i<4; i++)
	{
		for(j=0;j<4;j++)
		{
			*(M+4*j+i) = m_pCurWOpp->m_EigenVector[k*4+i][j];
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
			q[0] = q0[0] * exp(m_pCurWOpp->m_EigenValue[0+k*4]*t);
			q[1] = q0[1] * exp(m_pCurWOpp->m_EigenValue[1+k*4]*t);
			q[2] = q0[2] * exp(m_pCurWOpp->m_EigenValue[2+k*4]*t);
			q[3] = q0[3] * exp(m_pCurWOpp->m_EigenValue[3+k*4]*t);
			y[0] = *(M+4*0+0) * q[0] +*(M+4*0+1) * q[1] +*(M+4*0+2) * q[2] +*(M+4*0+3) * q[3];
			y[1] = *(M+4*1+0) * q[0] +*(M+4*1+1) * q[1] +*(M+4*1+2) * q[2] +*(M+4*1+3) * q[3];
			y[2] = *(M+4*2+0) * q[0] +*(M+4*2+1) * q[1] +*(M+4*2+2) * q[2] +*(M+4*2+3) * q[3];
			y[3] = *(M+4*3+0) * q[0] +*(M+4*3+1) * q[1] +*(M+4*3+2) * q[2] +*(M+4*3+3) * q[3];
			if(abs(q[0])>1.e10 || abs(q[1])>1.e10 || abs(q[2])>1.e10  || abs(q[3])>1.e10 ) break;

			pCurve0->AddPoint(t, y[0].real());
			if(m_bLongitudinal) pCurve1->AddPoint(t, y[1].real());
			else                pCurve1->AddPoint(t, y[1].real()*180.0/PI);
			pCurve2->AddPoint(t, y[2].real()*180.0/PI);
			pCurve3->AddPoint(t, y[3].real()*180.0/PI);
		}
	}
}



void QMiarex::CreateStabRungeKuttaCurves()
{
	// Builds the forced response from the state matrix and the forced input matrix
	// using a Runge-Kutta integration scheme
	// The forced input is interpolated in the control history defined in the input table

	static int i, j, TotalPoints, PlotInterval;

	static double t, dt, ctrl_t;
	static CCurve *pCurve0, *pCurve1, *pCurve2, *pCurve3;
	static double A[4][4], B[MAXCONTROLS][4];
	static double m[5][4];
	static double y[4], yp[4];
	QString CurveTitle;
	m_bBreak = false;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
	CurveTitle = pStabView->m_pctrlCurveList->currentText();
	pCurve0 = m_TimeGraph[0].GetCurve(CurveTitle);
	if(pCurve0) pCurve0->ResetCurve();
	else return;
	pCurve1 = m_TimeGraph[1].GetCurve(CurveTitle);
	if(pCurve1) pCurve1->ResetCurve();
	else return;
	pCurve2 = m_TimeGraph[2].GetCurve(CurveTitle);
	if(pCurve2) pCurve2->ResetCurve();
	else return;
	pCurve3 = m_TimeGraph[3].GetCurve(CurveTitle);
	if(pCurve3) pCurve3->ResetCurve();
	else return;

	//We need a WOpp
	if(!m_pCurWOpp) return;//nothing to plot
	//Check that the current polar is of the stability type
	if(!m_pCurWPolar || m_pCurWPolar->m_Type!=STABILITYPOLAR) return;

	if(m_bLongitudinal) 
	{
		memcpy(A, m_pCurWOpp->m_ALong, 4*4*sizeof(double));
		memcpy(B, m_pCurWOpp->m_BLong, MAXCONTROLS*4*sizeof(double));
	}
	else
	{
		memcpy(A, m_pCurWOpp->m_ALat, 4*4*sizeof(double));
		memcpy(B, m_pCurWOpp->m_BLat, MAXCONTROLS*4*sizeof(double));
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
	pCurve0->AddPoint(0.0, y[0]);
	pCurve1->AddPoint(0.0, y[1]);
	pCurve2->AddPoint(0.0, y[2]);
	pCurve3->AddPoint(0.0, y[3]);

	//Runge-Kutta method
	for(i=0; i<TotalPoints; i++)
	{
		//initial slope m1
		m[0][0] = A[0][0]*y[0] + A[0][1]*y[1] + A[0][2]*y[2] + A[0][3]*y[3];
		m[0][1] = A[1][0]*y[0] + A[1][1]*y[1] + A[1][2]*y[2] + A[1][3]*y[3];
		m[0][2] = A[2][0]*y[0] + A[2][1]*y[1] + A[2][2]*y[2] + A[2][3]*y[3];
		m[0][3] = A[3][0]*y[0] + A[3][1]*y[1] + A[3][2]*y[2] + A[3][3]*y[3];

		ctrl_t = pStabView->GetControlInput(t);
		m[0][0] += B[j][0] * ctrl_t;
		m[0][1] += B[j][1] * ctrl_t;
		m[0][2] += B[j][2] * ctrl_t;
		m[0][3] += B[j][3] * ctrl_t;

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
		m[1][0] += B[j][0] * ctrl_t;
		m[1][1] += B[j][1] * ctrl_t;
		m[1][2] += B[j][2] * ctrl_t;
		m[1][3] += B[j][3] * ctrl_t;

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

		m[2][0] += B[j][0] * ctrl_t;
		m[2][1] += B[j][1] * ctrl_t;
		m[2][2] += B[j][2] * ctrl_t;
		m[2][3] += B[j][3] * ctrl_t;

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

		m[3][0] += B[j][0] * ctrl_t;
		m[3][1] += B[j][1] * ctrl_t;
		m[3][2] += B[j][2] * ctrl_t;
		m[3][3] += B[j][3] * ctrl_t;

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
		if(fabs(y[0])>1.e10 || fabs(y[1])>1.e10 || fabs(y[2])>1.e10  || fabs(y[3])>1.e10 ) break;

		if(i%PlotInterval==0)
		{
			if(m_bLongitudinal)
			{
				pCurve0->AddPoint(t, y[0]*pMainFrame->m_mstoUnit);
				pCurve1->AddPoint(t, y[1]*pMainFrame->m_mstoUnit);
				pCurve2->AddPoint(t, y[2]*180.0/PI);//deg/s
				pCurve3->AddPoint(t, y[3]*180.0/PI);//deg
			}
			else
			{
				pCurve0->AddPoint(t, y[0]*pMainFrame->m_mstoUnit);
				pCurve1->AddPoint(t, y[1]*180.0/PI);//deg/s
				pCurve2->AddPoint(t, y[2]*180.0/PI);//deg/s
				pCurve3->AddPoint(t, y[3]*180.0/PI);//deg
			}
		}
	}
	pCurve0->SetVisible(true);
	pCurve1->SetVisible(true);
	pCurve2->SetVisible(true);
	pCurve3->SetVisible(true);
}


void QMiarex::CreateStabRLCurves()
{
	//
	// Resets and creates the curves of the root locus graph
	//
	CWPolar *pWPolar;

	// we have eight modes, 4 longitudinal and 4 lateral
	// declare a curve for each
	static CCurve *pLongCurve1,*pLongCurve2,*pLongCurve3,*pLongCurve4;
	static CCurve *pLatCurve1,*pLatCurve2,*pLatCurve3,*pLatCurve4;//do it later
	
	m_LongRLGraph.DeleteCurves();
	m_LatRLGraph.DeleteCurves();

	for (int k=0; k<m_poaWPolar->size(); k++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(k);
		if ((pWPolar->m_bIsVisible||pWPolar->m_bShowPoints)
			&& pWPolar->m_Alpha.size()>0 && (m_bType7 && pWPolar->m_Type == 7))
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

			pLongCurve1->SetTitle(pWPolar->m_PlrName+"_Mode_1");
			pLongCurve2->SetTitle(pWPolar->m_PlrName+"_Mode_2");
			pLongCurve3->SetTitle(pWPolar->m_PlrName+"_Mode_3");
			pLongCurve4->SetTitle(pWPolar->m_PlrName+"_Mode_4");

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

			pLatCurve1->SetTitle(pWPolar->m_PlrName+"_Mode_1");
			pLatCurve2->SetTitle(pWPolar->m_PlrName+"_Mode_2");
			pLatCurve3->SetTitle(pWPolar->m_PlrName+"_Mode_3");
			pLatCurve4->SetTitle(pWPolar->m_PlrName+"_Mode_4");
		}
	}
}


void QMiarex::DeleteBody(CBody *pThisBody)
{
	if(!pThisBody)	return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);
	int i;

	// ... Find the Body in the object array and remove it...
	CBody* pBody;
	for (i=m_poaBody->size()-1; i>=0; i--)
	{
		pBody = (CBody*)m_poaBody->at(i);
		if (pBody == pThisBody)
		{
			m_poaBody->removeAt(i);
			delete pBody;
			if(pBody == m_pCurBody)
			{
				m_pCurBody = NULL;
				SetUFO();
			}
			break;
		}
	}
}


void QMiarex::DrawCpLegend(QPainter &painter, QPoint place, int bottom)
{
	// 
	// Draws the legend of the Cp graph
	//
	painter.save();
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	int LegendSize, LegendWidth, dny, x1, i, ny;
	CCurve *pCurve=NULL;
	QColor color;
	QString strong;

	LegendSize = 30;
	LegendWidth = 350;
	dny = 14;
	bottom -= 15;//margin

	QPen CurvePen;
	QPen TextPen(pMainFrame->m_TextColor);

	ny=-1;

	for (i=0; i<m_CpGraph.GetCurveCount(); i++)
	{
		pCurve = m_CpGraph.GetCurve(i);
		if(pCurve->n)
		{
			ny++;

			if(abs(bottom)<abs(place.y() + (int)(dny*(ny+1))))
			{
				//move right
				place.rx() += LegendWidth;
				ny=0;
			}

			CurvePen.setColor(pCurve->GetColor());
			CurvePen.setStyle(GetStyle(pCurve->GetStyle()));
			CurvePen.setWidth(pCurve->GetWidth());
			painter.setPen(CurvePen);

			painter.drawLine(place.x() + (int)(1.5*LegendSize), place.y() + (int)(1.*dny*ny),
							 place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.*dny*ny));

			if(pCurve->m_bShowPoints)
			{
				x1 = place.x() + (int)(2.0*LegendSize);
				painter.drawRect(x1-2, place.y() + (int)(1.*dny*ny)-2,4,4);
			}

			pCurve->GetTitle(strong);
			painter.setPen(TextPen);
			painter.drawText(place.x() + (int)(3*LegendSize),place.y() + (int)(1.*dny*ny), strong);
		}
	}
	painter.restore();
}



void QMiarex::DrawStabTimeLegend(QPainter &painter, QPoint place, int bottom)
{
	//
	// Draws the legend for the time response graph- 4 curves
	//
	painter.save();
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	int LegendSize, LegendWidth, dny, x1, i, ny;
	CCurve *pCurve=NULL;
	QString strong;

	LegendSize = 30;
	LegendWidth = 350;
	dny = 14;
	bottom -= 15;//margin

	QPen CurvePen;
	QPen TextPen(pMainFrame->m_TextColor);

	ny=0;

	for (i=0; i<m_TimeGraph[0].GetCurveCount(); i++)
	{
		pCurve = m_TimeGraph[0].GetCurve(i);
		if(pCurve->n && pCurve->IsVisible())
		{
			ny++;

			if(abs(bottom)<abs(place.y() + (int)(dny*(ny+1))))
			{
				//move right
				place.rx() += LegendWidth;
				ny=0;
			}

			CurvePen.setColor(pCurve->GetColor());
			CurvePen.setStyle(GetStyle(pCurve->GetStyle()));
			CurvePen.setWidth(pCurve->GetWidth());
			painter.setPen(CurvePen);

			painter.drawLine(place.x() + (int)(1.5*LegendSize), place.y() + (int)(1.*dny*ny),
							 place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.*dny*ny));

			if(pCurve->m_bShowPoints)
			{
				x1 = place.x() + (int)(2.0*LegendSize);
				painter.drawRect(x1-2, place.y() + (int)(1.*dny*ny)-2,4,4);
			}

			pCurve->GetTitle(strong);
			painter.setPen(TextPen);
			painter.drawText(place.x() + (int)(3*LegendSize),
							 place.y() + (int)(1.*dny*ny)+(int)(dny/3), strong);
		}
	}
	painter.restore();
}


void QMiarex::DrawWOppLegend(QPainter &painter, QPoint place, int bottom)
{
	//
	// Draws the legend for the WOpp graph
	//
	painter.save();

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	int LegendSize, LegendWidth, ypos;
	int i, j, k,l, x1, nc, ny;

	ny=0;

	QString str1, str2, str3, str4, str5, str6;
	LegendSize = 30;
	LegendWidth = 300;

	QStringList str; // we need to make an inventory of wings
	bool bFound;
	CWOpp * pWOpp = NULL;
	CPOpp *pPOpp = NULL;

	for (i=0; i<m_poaWOpp->size(); i++)
	{
		bFound = false;
		pWOpp = (CWOpp*)m_poaWOpp->at(i);
		for (j=0; j<str.size(); j++)
		{
			if (pWOpp->m_WingName == str.at(j)) bFound = true;
		}
		if (!bFound)
		{
			str.append(pWOpp->m_WingName);
		}
	}

	for (i=0; i<m_poaPOpp->size(); i++)
	{
		bFound = false;
		pPOpp = (CPOpp*)m_poaPOpp->at(i);
		for (j=0; j<str.size(); j++)
		{
			if (pPOpp->m_PlaneName == str.at(j))	bFound = true;
		}
		if (!bFound)
		{
			str.append(pPOpp->m_PlaneName);
		}
	}

	painter.setBackgroundMode(Qt::TransparentMode);

	painter.setFont(pMainFrame->m_TextFont);

	QFontMetrics fm(pMainFrame->m_TextFont);
	ypos = fm.height();

	QPen TextPen(pMainFrame->m_TextColor);
	painter.setPen(TextPen);
	TextPen.setWidth(1);

	QBrush LegendBrush(pMainFrame->m_BackgroundColor);
	painter.setBrush(LegendBrush);

	QPen LegendPen;
	LegendPen.setWidth(1);

	if(m_bCurWOppOnly)
	{
		if(!m_pCurWOpp || !m_pCurWOpp->m_bIsVisible)
		{
			painter.restore();
			return;
		}
		ny++ ;

		painter.setPen(TextPen);
		painter.drawText(place.x() + (int)(1*LegendSize), place.y() + ypos*ny-(int)(ypos/2), m_pCurWing->WingName());

		LegendPen.setColor(m_pCurWOpp->m_Color);
		LegendPen.setStyle(GetStyle(m_pCurWOpp->m_Style));
		LegendPen.setWidth(m_pCurWOpp->m_Width);
		painter.setPen(LegendPen);

		painter.drawLine(place.x() + (int)(1.5*LegendSize), place.y() + (int)(1.*ypos*ny),
						 place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.*ypos*ny));

		if(m_pCurWOpp->m_bShowPoints)
		{
			x1 = place.x() + (int)(2.0*LegendSize);
			painter.drawRect(x1-2, place.y() + (int)(1.*ypos*ny)-2, 4, 4);
		}

		str1 = QString("V=%1").arg(m_pCurWOpp->m_QInf*pMainFrame->m_mstoUnit,5,'f',2);
		GetSpeedUnit(str2, pMainFrame->m_SpeedUnit);
		str3 = QString("_a=%1").arg(m_pCurWOpp->m_Alpha,5,'f',2);
		
		if(fabs(m_pCurWOpp->m_Beta)>0.0) str4 = QString("_b=%1").arg(m_pCurWOpp->m_Beta,5,'f',2);
		else                             str4 ="";
		if(m_pCurWOpp->m_Type>=5)        str5 = QString("_Ctrl=%1").arg(m_pCurWOpp->m_Ctrl,5,'f',2);
		else                             str5 ="";


		if(m_pCurWOpp->m_AnalysisMethod==LLTMETHOD)  str6="_LLT";
		else if(m_pCurWPolar->m_bThinSurfaces)
		{
			if(m_pCurWOpp->m_bVLM1)	str6="_VLM1";
			else 					str6="_VLM2";
		}
		else if(m_pCurWOpp->m_AnalysisMethod==PANELMETHOD)
		{
			str6="_Panel";
			if(m_pCurWOpp->m_bThinSurface) str6 += "_Thin";
		}

		if(m_pCurWOpp->m_bTiltedGeom) str6 += "_TG";

		if(m_pCurWOpp->m_bOut) str6+=" (Out)";

		painter.setPen(TextPen);
		painter.drawText(place.x() + (int)(3*LegendSize),
						 place.y() + (int)(1.*ypos*ny)+(int)(ypos/3),
						 str1+str2+str3+str4+str5+str6);
	}
	else
	{
		bool bStarted = false;
		for (k = 0; k<str.size(); k++)
		{
			int UFOPts = 0;
			for (l=0; l < m_poaWOpp->size(); l++)
			{
				pWOpp = (CWOpp*)m_poaWOpp->at(l);
				if (pWOpp->m_bIsVisible && pWOpp->m_WingName == str.at(k)) UFOPts++;
			}
			for (l=0; l < m_poaPOpp->size(); l++)
			{
				pPOpp = (CPOpp*)m_poaPOpp->at(l);
				if (pPOpp->m_bIsVisible && pPOpp->m_PlaneName == str.at(k)) UFOPts++;
			}
			if (UFOPts)
			{
				int YPos = place.y() + (ny+UFOPts+2) * ypos;// bottom line of this foil's legend

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
				for (nc=0; nc < m_poaWOpp->size(); nc++)
				{
					pWOpp = (CWOpp*)m_poaWOpp->at(nc);
					if(str.at(k) == pWOpp->m_WingName && pWOpp->m_bIsVisible)
					{
						if(abs(bottom)<abs(place.y() + (int)(1.*ypos*ny+ypos)))
						{
							//move right
							place.rx() += LegendWidth;
							ny=1;
						}

						LegendPen.setColor(pWOpp->m_Color);
						LegendPen.setStyle(GetStyle(pWOpp->m_Style));
						LegendPen.setWidth(pWOpp->m_Width);
						painter.setPen(LegendPen);
						painter.drawLine(place.x() + (int)(1.5*LegendSize), place.y() + (int)(1.*ypos*ny),
										 place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.*ypos*ny));

						if(pWOpp->m_bShowPoints)
						{
							x1 = place.x() + (int)(2.0*LegendSize);
							painter.drawRect(x1-2, place.y() + (int)(1.*ypos*ny)-2, 4,4);
						}

						str1 = QString("V=%1").arg(pWOpp->m_QInf*pMainFrame->m_mstoUnit,5,'f',2);
						GetSpeedUnit(str2, pMainFrame->m_SpeedUnit);
						str3 = QString("_a=%1").arg(pWOpp->m_Alpha,5,'f',2);
						
						if(fabs(pWOpp->m_Beta)>0.0) str4 = QString("_b=%1").arg(pWOpp->m_Beta,5,'f',2);
						else                        str4 ="";
						if(pWOpp->m_Type>=5)        str5 = QString("_Ctrl=%1").arg(pWOpp->m_Ctrl,5,'f',2);
						else                        str5 ="";

						if(pWOpp->m_AnalysisMethod==LLTMETHOD) str6 ="_LLT";
						else if(pWOpp->m_AnalysisMethod==VLMMETHOD)
						{
							if(pWOpp->m_bVLM1)       str6 ="_VLM1";
							else                     str6 ="_VLM2";
						}
						else if(pWOpp->m_AnalysisMethod==PANELMETHOD)
						{
							str6="_Panel";
							if(pWOpp->m_bThinSurface) str6 += "_Thin";
						}

						if(pWOpp->m_bTiltedGeom) str6 += "_TG";
						if(pWOpp->m_bOut)        str6+=" (Out)";

						painter.setPen(TextPen);
						painter.drawText(place.x() + (int)(3*LegendSize),
										 place.y() + (int)(1.*ypos*ny)+(int)(ypos/3), str1+str2+str3+str4+str5+str6);
						ny++ ;
					}
				}
				for (nc=0; nc < m_poaPOpp->size(); nc++)
				{
					pPOpp = (CPOpp*)m_poaPOpp->at(nc);
					if(str.at(k) == pPOpp->m_PlaneName && pPOpp->m_bIsVisible)
					{
						if(abs(bottom)<abs(place.y() + (int)(1.*ypos*ny+ypos)))
						{
							//move right
							place.rx() += LegendWidth;
							ny=2;
						}

						LegendPen.setColor(pPOpp->m_Color);
						LegendPen.setStyle(GetStyle(pPOpp->m_Style));
						LegendPen.setWidth(pPOpp->m_Width);
						painter.setPen(LegendPen);

						painter.drawLine(place.x() + (int)(1.5*LegendSize), place.y() + (int)(1.*ypos*ny),
										 place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.*ypos*ny));

						if(pPOpp->m_bShowPoints)
						{
							x1 = place.x() + (int)(2.0*LegendSize);
							painter.drawRect(x1-2, place.y() + (int)(1.*ypos*ny)-2, 4, 4);
						}

						str1 = QString("V=%1").arg(pPOpp->m_QInf*pMainFrame->m_mstoUnit,5,'f',2);
						GetSpeedUnit(str2, pMainFrame->m_SpeedUnit);
						str3 = QString("_a=%1").arg(pPOpp->m_Alpha,5,'f',2);
						
						if(fabs(pPOpp->m_Beta)>0.0) str4 = QString("_b=%1").arg(pPOpp->m_Beta,5,'f',2);
						else                        str4 ="";
						if(pPOpp->m_Type>=5)        str5 = QString("_Ctrl=%1").arg(pPOpp->m_Ctrl,5,'f',2);
						else                        str5 ="";
						
						if(pPOpp->m_PlaneWOpp[0].m_AnalysisMethod==VLMMETHOD)
						{
							if(pPOpp->m_bVLM1)      str6 ="_VLM1";
							else                    str6 ="_VLM2";
						}
						else if(pPOpp->m_PlaneWOpp[0].m_AnalysisMethod==PANELMETHOD)
						{
							str6="_Panel";
							if(pPOpp->m_PlaneWOpp[0].m_bThinSurface) str4 += "_Thin";
						}

						if(pPOpp->m_PlaneWOpp[0].m_bTiltedGeom) str6+= "_TG";
						if(pPOpp->m_PlaneWOpp[0].m_bOut)        str6+=" (Out)";

						painter.setPen(TextPen);
						painter.drawText(place.x() + (int)(3*LegendSize),
										 place.y() + (int)(1.*ypos*ny)+(int)(ypos/3), str1+str2+str3+str4+str5+str6);
						ny++ ;
					}
				}
				if (UFOPts) ny++;
			}
		}
	}
	painter.restore();
}




void QMiarex::DrawWPolarLegend(QPainter &painter, QPoint place, int bottom)
{
	//
	// Draws the legend of the polar graphs
	//

	painter.save();

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int LegendSize, LegendWidth, ypos;
	int i,j,k,l, ny, x1;

	LegendSize = 30;
	LegendWidth = 280;

	painter.setFont(pMainFrame->m_TextFont);

	QFontMetrics fm(pMainFrame->m_TextFont);
	ypos = fm.height();

	QPen TextPen(pMainFrame->m_TextColor);
	painter.setPen(TextPen);
	TextPen.setWidth(1);

	QStringList str; // we need to make an inventory of wings
	CWPolar * pWPolar;
	CWing *pWing;
	CPlane *pPlane;

	for (j=0; j<m_poaWing->size(); j++)
	{
		pWing = (CWing*)m_poaWing->at(j);
		for (i=0; i<m_poaWPolar->size(); i++)
		{
			pWPolar = (CWPolar*)m_poaWPolar->at(i);
			if (pWPolar->m_UFOName == pWing->m_WingName)
			{
				str.append(pWing->m_WingName);
				break;
			}
		}// finished inventory
	}

	for (j=0; j<m_poaPlane->size(); j++)
	{
		pPlane = (CPlane*)m_poaPlane->at(j);
		for (i=0; i<m_poaWPolar->size(); i++)
		{
			pWPolar = (CWPolar*)m_poaWPolar->at(i);
			if (pWPolar->m_UFOName == pPlane->PlaneName())
			{
				str.append(pPlane->PlaneName());
				break;
			}
		}// finished inventory
	}

	int nUFOs= str.size();

	painter.setBackgroundMode(Qt::TransparentMode);
	QBrush LegendBrush(pMainFrame->m_BackgroundColor);
	painter.setBrush(LegendBrush);

	QPen LegendPen;
	LegendPen.setWidth(1);


	ny =0;
	for (k=0; k<nUFOs; k++)
	{
		int UFOPlrs = 0;
		for (l=0; l < m_poaWPolar->size(); l++)
		{
			pWPolar = (CWPolar*)m_poaWPolar->at(l);

			if (pWPolar->m_Alpha.size() &&
				(pWPolar->m_bIsVisible ||(pWPolar->m_bShowPoints&&pWPolar->m_Type==STABILITYPOLAR)) &&
				((pWPolar->m_Type==STABILITYPOLAR && m_iView==WSTABVIEW) || m_iView!=WSTABVIEW) &&
				  pWPolar->m_UFOName == str.at(k) &&
				((pWPolar->m_Type == 1 && m_bType1) ||
				 (pWPolar->m_Type == 2 && m_bType2) ||
				 (pWPolar->m_Type == 4 && m_bType4) ||
				 (pWPolar->m_Type == 5 && m_bType5) ||
				 (pWPolar->m_Type == 6 && m_bType6) ||
				 (pWPolar->m_Type == 7 && m_bType7)))
			{
					UFOPlrs++;
			}
		}

		if (UFOPlrs)
		{
			int YPos = place.y() + (ny+UFOPlrs+2) * ypos;// bottom line of this foil's legend
			if(abs(bottom) > abs(YPos))
			{
				ny++;
				painter.drawText(place.x(), place.y() + ypos*ny-(int)(ypos/2), str.at(k));
			}
			else
			{
				// move rigth if outside screen
				place.rx() += LegendWidth;
				ny=1;
				painter.setPen(TextPen);
				painter.drawText(place.x() , place.y() + ypos*ny-(int)(ypos/2), str.at(k));
			}

			for (int nc=0; nc < m_poaWPolar->size(); nc++)
			{
				pWPolar = (CWPolar*)m_poaWPolar->at(nc);
				if(str.at(k) == pWPolar->m_UFOName)
				{
					if (pWPolar->m_Alpha.size() &&
						(pWPolar->m_bIsVisible ||(pWPolar->m_bShowPoints&&pWPolar->m_Type==STABILITYPOLAR)) &&
						((pWPolar->m_Type==STABILITYPOLAR && m_iView==WSTABVIEW) || m_iView!=WSTABVIEW) &&
						  pWPolar->m_UFOName == str.at(k) &&
						((pWPolar->m_Type == 1 && m_bType1) ||
						 (pWPolar->m_Type == 2 && m_bType2) ||
						 (pWPolar->m_Type == 4 && m_bType4) ||
						 (pWPolar->m_Type == 5 && m_bType5) ||
						 (pWPolar->m_Type == 6 && m_bType6) ||
						 (pWPolar->m_Type == 7 && m_bType7)))
					{

						LegendPen.setColor(pWPolar->m_Color);
						LegendPen.setStyle(GetStyle(pWPolar->m_Style));
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
										 place.y() + (int)(1.*ypos*ny)+(int)(ypos/3), pWPolar->m_PlrName);
						ny++ ;
					}
				}
			}
			if(UFOPlrs) ny++;
		}
	}
	painter.restore();
}


void QMiarex::DuplicatePlane()
{
	//
	// Duplicate the currently selected plane, if any
	// and opens it for edition
	//
	if(!m_pCurPlane) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	CPlane* pNewPlane= new CPlane;
	pNewPlane->Duplicate(m_pCurPlane);

	if(!SetModPlane(pNewPlane))
	{
		delete pNewPlane;
		SetUFO();
		pMainFrame->UpdateUFOs();
		UpdateView();
	}
	else
	{
		m_pCurPlane = AddPlane(pNewPlane);
		SetUFO();
		pMainFrame->UpdateUFOs();
		UpdateView();
		EditCurPlane();
	}
}


void QMiarex::EditCurPlane()
{
	//
	// Opens the edition dialog box for the currently selected plane
	//
	int i;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(!m_pCurPlane) return;
	CWPolar *pWPolar;
	CPOpp* pPOpp;
	bool bHasResults = false;
	for (i=0; i< m_poaWPolar->size(); i++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(i);
		if(pWPolar->m_Alpha.size() && pWPolar->m_UFOName == m_pCurPlane->PlaneName())
		{
			bHasResults = true;
			break;
		}
	}

	for (i=0; i< m_poaPOpp->size(); i++)
	{
		pPOpp = (CPOpp*)m_poaPOpp->at(i);
		if(pPOpp->m_PlaneName == m_pCurPlane->PlaneName())
		{
			bHasResults = true;
			break;
		}
	}

	CPlane* pModPlane= new CPlane;

	pModPlane->Duplicate(m_pCurPlane);
	m_PlaneDlg.m_pPlane = pModPlane;

	m_PlaneDlg.m_bAcceptName = false;
	m_PlaneDlg.InitDialog();

	if(QDialog::Accepted == m_PlaneDlg.exec())
	{
		if(m_PlaneDlg.m_bChanged)
		{
			if(bHasResults)
			{
				m_ModDlg.m_Question = tr("The modification will erase all results associated to this Plane.\nContinue ?");
				m_ModDlg.InitDialog();
				int Ans = m_ModDlg.exec();

				if (Ans == QDialog::Rejected)
				{
					//restore geometry
					delete pModPlane; // clean up
					return;
				}
				else if(Ans==20)
				{
					//save mods to a new plane object
					if(!SetModPlane(pModPlane))
					{
						delete pModPlane;
					}
					else
					{
						m_pCurPlane = AddPlane(pModPlane);
					}
					SetUFO();
					pMainFrame->UpdateUFOs();
					UpdateView();
					return;
				}
			}

			//then modifications are automatically recorded
			m_pCurPlane->Duplicate(pModPlane);
			pMainFrame->DeletePlane(m_pCurPlane, true);// will also set new surface and Aerochord in WPolars
			m_bResetglGeom = true;
			m_bResetglMesh = true;
//			m_bResetglOpp  = true;
			if(m_iView==WPOLARVIEW)		CreateWPolarCurves();
			else if(m_iView==WSTABVIEW)	CreateStabilityCurves();
			else if(m_iView==WOPPVIEW)	CreateWOppCurves();
			else if(m_iView==WCPVIEW)	CreateCpCurves();

		}
		SetUFO();
		pMainFrame->UpdateUFOs();
		m_bIs2DScaleSet = false;
		SetScale();
		OnAdjustToWing();
		UpdateView();
	}
	else
	{
		// restore original
//		m_pCurPlane->Duplicate(pSavePlane);
		delete pModPlane; // clean up
		return ;
	}
	delete pModPlane; // clean up
}



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



void QMiarex::FillWOppCurve(CWOpp *pWOpp, Graph *pGraph, CCurve *pCurve)
{
	//
	// The curve has been created, fill the curve with the WOpp data
	//
	int Var = pGraph->GetYVariable();
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int nStart, i;
	if(pWOpp->m_AnalysisMethod==LLTMETHOD) nStart = 1;
	else nStart = 0;

	switch(Var)
	{
		case 0:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_Ai[i]);
			}
			pGraph->SetYTitle(tr("Induced Angle"));
			break;
		}
		case 1:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit,
					pWOpp->m_Alpha + pWOpp->m_Ai[i] + pWOpp->m_Twist[i]);
			}
			pGraph->SetYTitle(tr("Total Angle"));
			break;
		}
		case 2:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_Cl[i]);
			}
			pGraph->SetYTitle(tr("Cl"));
			break;
		}
		case 3:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_Cl[i] * pWOpp->m_Chord[i]/pWOpp->m_MAChord);
			}
			pGraph->SetYTitle(tr("Local lift"));
			break;
		}
		case 4:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_PCd[i]);
			}
			pGraph->SetYTitle(tr("Airfoil drag"));
			break;
		}
		case 5:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_ICd[i]);
			}
			pGraph->SetYTitle(tr("Induced drag"));
			break;
		}
		case 6:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_PCd[i]+ pWOpp->m_ICd[i]);
			}
			pGraph->SetYTitle(tr("Total drag"));
			break;
		}
		case 7:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, (pWOpp->m_PCd[i]+ pWOpp->m_ICd[i])* pWOpp->m_Chord[i]/pWOpp->m_MAChord);
			}
			pGraph->SetYTitle(tr("Local drag"));
			break;
		}
		case 8:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_CmAirf[i]);
			}
			pGraph->SetYTitle(tr("Cm Airfoil"));
			break;
		}
		case 9:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_Cm[i]);
			}
			pGraph->SetYTitle(tr("Cm total"));
			break;
		}
		case 10:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_Re[i]);
			}
			pGraph->SetYTitle(tr("Re"));
			break;
		}
		case 11:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_XTrTop[i]);
			}
			pGraph->SetYTitle(tr("Top Trans x-Pos %"));
			break;
		}
		case 12:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_XTrBot[i]);
			}
			pGraph->SetYTitle(tr("Bot Trans x-Pos %"));
			break;
		}
		case 13:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_XCPSpanRel[i]*100.0);
			}
			pGraph->SetYTitle(tr("CP x-Pos %"));
			break;
		}
		case 14:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit,
					pWOpp->m_BendingMoment[i] * pMainFrame->m_NmtoUnit);
			}
			QString str;
			GetMomentUnit(str, pMainFrame->m_MomentUnit);
			pGraph->SetYTitle(tr("BM (") + str + ")");
			break;
		}
		default:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_Ai[i]);
			}
			pGraph->SetYTitle(tr("Induced Angle"));
		}
	}
}



void QMiarex::FillStabCurve(CCurve *pCurve, CWPolar *pWPolar, int iMode)
{
	//
	// The curve object has been created
	// Fill it with the variable data specified by iMode
	//

	static int i;
	static double x,y;
	static QString UFOName;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;

	if(m_pCurPlane)     UFOName=m_pCurPlane->PlaneName();
	else if(m_pCurWing) UFOName=m_pCurWing->WingName();

	pCurve->SetSelected(-1);

	for (i=0; i<pWPolar->m_Ctrl.size(); i++)
	{
		x = pWPolar->m_EigenValue[iMode][i].real();
		y = pWPolar->m_EigenValue[iMode][i].imag()/2./PI;

		pCurve->AddPoint(x, y);
		if(m_pCurWOpp && m_bHighlightOpp)
		{
			if(fabs(pWPolar->m_Ctrl[i]-m_pCurWOpp->m_Ctrl)<0.0001)
			{
				if(m_pCurPOpp && m_pCurPlane && (pWPolar->m_UFOName==m_pCurPlane->PlaneName()) && (m_pCurPOpp->m_PlrName==pWPolar->m_PlrName))
				{
					if(iMode==pStabView->m_iCurrentMode) pCurve->SetSelected(i);
				}
				else if(m_pCurWOpp && m_pCurWing  && (pWPolar->m_UFOName==m_pCurWing->WingName()) && (m_pCurWOpp->m_PlrName ==pWPolar->m_PlrName))
				{
					if(iMode==pStabView->m_iCurrentMode) pCurve->SetSelected(i);
				}
			}
		}
	}
}


void QMiarex::FillWPlrCurve(CCurve *pCurve, CWPolar *pWPolar, int XVar, int YVar)
{
	//
	//The Wpolar curve object has been created
	//Fill it with the variable data specified by XVar and YVar
	//
	bool bAdd;
	int i;
	double x,y;
	QString UFOName;
	if(m_pCurPlane)     UFOName=m_pCurPlane->PlaneName();
	else if(m_pCurWing) UFOName=m_pCurWing->WingName();
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	static QList <double> *pX;
	static QList <double> *pY;
	pX = (QList <double> *) pWPolar->GetUFOPlrVariable(XVar);
	pY = (QList <double> *) pWPolar->GetUFOPlrVariable(YVar);

	pCurve->SetSelected(-1);

	for (i=0; i<pWPolar->m_Alpha.size(); i++)
	{
		bAdd = true;

		x = (*pX)[i];
		y = (*pY)[i];

		if((XVar==14 || XVar==17 || XVar==18) && x<0) bAdd = false;
		if((YVar==14 || YVar==17 || YVar==18) && y<0) bAdd = false;

		//Set user units
		if(XVar==15 || XVar==16)              x *= pMainFrame->m_NtoUnit; //force
		if(YVar==15 || YVar==16)              y *= pMainFrame->m_NtoUnit; //force
		if(XVar==17 || XVar==18 || XVar==19)  x *= pMainFrame->m_mstoUnit;//speed
		if(YVar==17 || YVar==18 || YVar==19)  y *= pMainFrame->m_mstoUnit;//speed

		if(XVar==21 || XVar==22 || XVar==23)  x *= pMainFrame->m_NmtoUnit;//moment
		if(YVar==21 || YVar==22 || YVar==23)  y *= pMainFrame->m_NmtoUnit;//moment

		if(XVar==24 || XVar==25)              x *= pMainFrame->m_mtoUnit;//force
		if(YVar==24 || YVar==25)              y *= pMainFrame->m_mtoUnit;//force

		if(XVar==26)                          x *= pMainFrame->m_NmtoUnit;//moment
		if(YVar==26)                          y *= pMainFrame->m_NmtoUnit;//moment

		if(XVar==32)                          x *= pMainFrame->m_mtoUnit;//length
		if(YVar==32)                          y *= pMainFrame->m_mtoUnit;//length


		if(bAdd)
		{
			pCurve->AddPoint(x,y);
			if(m_pCurWOpp && m_bHighlightOpp)
			{
				if(fabs(pWPolar->m_Alpha[i]-m_pCurWOpp->m_Alpha)<0.0001)
				{
					if(m_pCurPOpp && m_pCurPlane
					   && pWPolar->m_UFOName==m_pCurPlane->PlaneName()
					   && m_pCurPOpp->m_PlrName ==pWPolar->m_PlrName)
					{
						pCurve->SetSelected(i);
					}
					else if(m_pCurWOpp && m_pCurWing
							&& pWPolar->m_UFOName==m_pCurWing->WingName()
							&& m_pCurWOpp->m_PlrName ==pWPolar->m_PlrName)
						pCurve->SetSelected(i);
				}
			}
		}
	}
}


CBody * QMiarex::GetBody(QString BodyName)
{
	//
	// returns a pointer to the body with the name BodyName
	//
	int i;
	CBody* pBody;
	for (i=0; i<m_poaBody->size(); i++)
	{
		pBody = (CBody*)m_poaBody->at(i);
		if (pBody->m_BodyName == BodyName) return pBody;
	}
	return NULL;
}



CPOpp * QMiarex::GetPOpp(double x)
{
	//
	// returns a pointer to the WOpp corresponding to aoa Alpha,
	// and with the name of the current plane and current WPolar
	//
	int i;
	if(!m_pCurPlane || !m_pCurWPolar) return NULL;

	CPOpp* pPOpp;

	for (i=0; i<m_poaPOpp->size(); i++)
	{
		pPOpp = (CPOpp*)m_poaPOpp->at(i);
		if ((pPOpp->m_PlaneName == m_pCurPlane->PlaneName()) && (pPOpp->m_PlrName == m_pCurWPolar->m_PlrName))
		{
			if     (m_pCurWPolar->m_Type<4 && fabs(pPOpp->m_Alpha - x)<0.005)               return pPOpp;
			else if(m_pCurWPolar->m_Type==FIXEDAOAPOLAR  && fabs(pPOpp->m_QInf - x)<0.005)  return pPOpp;
			else if(m_pCurWPolar->m_Type==STABILITYPOLAR && fabs(pPOpp->m_Ctrl - x)<0.005)  return pPOpp;
		}
	}
	return NULL;
}


QGraph* QMiarex::GetGraph(QPoint &pt)
{
	//
	// returns a pointer to the graph in which the point pt lies
	//
	if(m_iView==WOPPVIEW)
	{
		if(m_iWingView==1)
		{
			if(m_pCurWingGraph->IsInDrawRect(pt)) return m_pCurWingGraph;
			else return NULL;
		}
		else if (m_iWingView==2)
		{
			if(m_WingGraph[0].IsInDrawRect(pt)){return m_WingGraph;}
			if(m_WingGraph[1].IsInDrawRect(pt)){return m_WingGraph+1;}
			return NULL;
		}
		else
		{
			for(int ig=0; ig<4; ig++)
			{
				if(m_WingGraph[ig].IsInDrawRect(pt)){return m_WingGraph+ig;}
			}
			return NULL;
		}
	}
	else if(m_iView==WPOLARVIEW)
	{
		if(m_iWPlrView==1)
		{
			return m_pCurWPlrGraph;
		}
		else if (m_iWPlrView==2)
		{
			if(m_WPlrGraph[0].IsInDrawRect(pt)){return m_WPlrGraph;}
			if(m_WPlrGraph[1].IsInDrawRect(pt)){return m_WPlrGraph+1;}
			return NULL;
		}
		else
		{
			for(int ig=0; ig<4; ig++)
				if(m_WPlrGraph[ig].IsInDrawRect(pt)) {return m_WPlrGraph+ig;}
		}
	}
	else if(m_iView==WSTABVIEW)
	{
		if(m_iStabilityView==STABTIMEVIEW)
		{
			if(m_iStabTimeView==1) 
			{
				return m_pCurTimeGraph;
			}
			else
			{
				for(int ig=0; ig<4; ig++)
					if(m_TimeGraph[ig].IsInDrawRect(pt)) return m_TimeGraph+ig;
			}
			return NULL;
		}
		else if(m_iStabilityView==STABPOLARVIEW)
		{
			if(m_pCurRLStabGraph && m_pCurRLStabGraph->IsInDrawRect(pt)) return m_pCurRLStabGraph;
			return NULL;
		}

	}
	else if(m_iView==WCPVIEW)
	{
		m_pCurGraph = &m_CpGraph;
		return m_pCurGraph;
	}
	return NULL;
}



CPlane * QMiarex::GetPlane(QString PlaneName)
{
	//
	// returns a pointer to the plane with the name PlaneName
	//
	int i;
	CPlane* pPlane;
	for (i=0; i<m_poaPlane->size(); i++)
	{
		pPlane = (CPlane*)m_poaPlane->at(i);
		if (pPlane->PlaneName() == PlaneName) return pPlane;
	}
	return NULL;
}




CWing * QMiarex::GetWing(QString WingName)
{
	//
	// returns a pointer to the wing with the name WingName
	//
	if(!WingName.length()) return NULL;

	int i;
	CWing* pWing;
	for (i=0; i<m_poaWing->size(); i++)
	{
		pWing = (CWing*)m_poaWing->at(i);
		if (pWing->m_WingName == WingName) return pWing;
	}
	return NULL;
}


CWOpp* QMiarex::GetWOpp(double x)
{
	//
	// returns a pointer to the WOpp corresponding to aoa Alpha,
	// and with the name of the current wing and current WPolar
	//
	if(!m_pCurWing || !m_pCurWPolar) return NULL;
	int i;
	CWOpp* pWOpp;

	for (i=0; i<m_poaWOpp->size(); i++)
	{
		pWOpp = (CWOpp*)m_poaWOpp->at(i);
		if ((pWOpp->m_WingName==m_pCurWing->WingName()) &&(pWOpp->m_PlrName==m_pCurWPolar->m_PlrName))
		{
			if(m_pCurWPolar->m_Type<4 && fabs(pWOpp->m_Alpha - x)<0.005)                   return pWOpp;
			else if(m_pCurWPolar->m_Type==FIXEDAOAPOLAR && fabs(pWOpp->m_QInf - x)<0.005)  return pWOpp;
			else if(m_pCurWPolar->m_Type==STABILITYPOLAR && fabs(pWOpp->m_Ctrl - x)<0.005) return pWOpp;
		}
	}
	return NULL;
}



CWPolar* QMiarex::GetWPolar(QString WPolarName)
{
	//
	// returns a pointer to the WPolar with name WPolarName
	// or returns NULL if non with that name for the current UFO
	//
	CWPolar *pWPolar;
	QString UFOName;
	int i;
	if(m_pCurPlane)     UFOName = m_pCurPlane->PlaneName();
	else if(m_pCurWing) UFOName = m_pCurWing->WingName();
	else return NULL;

	for (i=0; i<m_poaWPolar->size(); i++)
	{
		pWPolar = (CWPolar*) m_poaWPolar->at(i);
		if (pWPolar->m_UFOName == UFOName && pWPolar->m_PlrName == WPolarName) return pWPolar;
	}
	return NULL;
}



void QMiarex::GLToClient(CVector const &real, QPoint &point)
{
	//
	//converts an opengl 2D vector to screen client coordinates
	//
	if(!s_pGLWidget) return;
	static double dx, dy, h2, w2;

	GLWidget *pGLWidget = (GLWidget*)s_pGLWidget;
	h2 = pGLWidget->m_GLViewRect.height() /2.0;
	w2 = pGLWidget->m_GLViewRect.width()  /2.0;

	dx =  real.x + w2;
	dy = -real.y + h2;

	point.setX((int)(dx * m_r3DCltRect.width()));
	point.setY((int)(dy * m_r3DCltRect.height()));
}



void QMiarex::GLCallViewLists()
{
	//
	// Calls the existing OpenGL lists for display in the 3D view
	// the list have been created previously by a call to GLDraw3D
	//

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	if (m_pCurWPolar && m_pCurWPolar->m_AnalysisMethod ==3 && m_pCurWPolar->m_bTiltedGeom && m_bWakePanels)
		glCallList(WINGWAKEPANELS);

	if(m_bMoments && m_pCurWOpp) glCallList(VLMMOMENTS);

	if (m_pCurWOpp && m_bStream && m_pCurWOpp->m_AnalysisMethod>=2 && !m_bResetglStream)
		glCallList(VLMSTREAMLINES);//streamlines are not rotated

	if(m_pCurWOpp && m_bSpeeds && m_pCurWOpp->m_AnalysisMethod>=2 && !m_bResetglFlow)
		glCallList(SURFACESPEEDS);

	if (m_pCurWOpp) glRotated(m_pCurWOpp->m_Alpha, 0.0, 1.0, 0.0);


	if (m_pCurWPolar && m_pCurWPolar->m_AnalysisMethod==3 && !m_pCurWPolar->m_bTiltedGeom && m_bWakePanels)
		glCallList(WINGWAKEPANELS);


	if(m_bVLMPanels && m_pCurWing)
	{
		if(!(m_b3DCp&&m_pCurWOpp) && !m_bSurfaces) glCallList(MESHBACK);
		glCallList(MESHPANELS);
	}

	if(m_bVortices && m_pCurWing)
	{
		glCallList(VLMCTRLPTS);
		glCallList(VLMVORTICES);
	}

	if(m_b3DCp && m_pCurWOpp && m_pCurWOpp->m_AnalysisMethod>=2)
	{
		glCallList(PANELCP);
	}
	if(m_bPanelForce && m_pCurWOpp && m_pCurWOpp->m_AnalysisMethod>=2)
	{
		glCallList(PANELFORCEARROWS);
	}

	if (m_pCurWPolar && fabs(m_pCurWPolar->m_Beta)>0.001) glRotated(-m_pCurWPolar->m_Beta, 0.0, 0.0, 1.0);

	if(m_bXCP && m_pCurWOpp)
	{
		for(int iw=0; iw<MAXWINGS; iw++)
			if(m_pWingList[iw]) glCallList(VLMWINGLIFT+iw);
		glCallList(LIFTFORCE);
	}


	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	if(m_bOutline)
	{
		for(int iw=0; iw<MAXWINGS; iw++)
			if(m_pWingList[iw])  glCallList(WINGOUTLINE+iw);

		if(m_pCurPlane)  glTranslated((m_pCurPlane)->BodyPos().x, 0.0, (m_pCurPlane)->BodyPos().z);
		if(m_pCurBody)	  glCallList(BODYGEOM);
		if(m_pCurPlane)  glTranslated(-(m_pCurPlane)->BodyPos().x, 0.0, -(m_pCurPlane)->BodyPos().z);
	}

	if(m_bglLight)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}

	if(m_bSurfaces)
	{
		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(m_pWingList[iw]) glCallList(WINGSURFACES+iw);
		}

		if(m_pCurBody)
		{
			if(m_pCurPlane)	glTranslated((m_pCurPlane)->BodyPos().x, 0.0, (m_pCurPlane)->BodyPos().z);
			glCallList(BODYSURFACES);
			if(m_pCurPlane)	glTranslated(-(m_pCurPlane)->BodyPos().x, 0.0, -(m_pCurPlane)->BodyPos().z);
		}
	}
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);


	if((m_bICd || m_bVCd) && m_pCurWOpp )
	{
		for (int iw=0; iw<MAXWINGS; iw++)
			if(m_pWingList[iw]) glCallList(VLMWINGDRAG+iw);
	}

	if(m_bDownwash && m_pCurWOpp)
	{
		for (int iw=0; iw<MAXWINGS; iw++)
			if(m_pWingList[iw]) glCallList(VLMWINGWASH+iw);
	}

	if(m_bXTop && m_pCurWOpp)
	{
		for (int iw=0; iw<MAXWINGS; iw++)
			if(m_pWingList[iw]) glCallList(VLMWINGTOPTRANS+iw);
	}
	if(m_bXBot && m_pCurWOpp)
	{
		for (int iw=0; iw<MAXWINGS; iw++)
			if(m_pWingList[iw]) glCallList(VLMWINGBOTTRANS+iw);
	}
}



void QMiarex::GLDrawAxes()
{
	//
	// Draws the axis in the OpenGL view
	//

	MainFrame * pMainFrame =(MainFrame*)s_pMainFrame;
	double l = .8;
//	if(m_pCurWing) l=1.1*m_pCurwing->m_PlanformSpan/2.0;
	GLWidget *pGLWidget = (GLWidget*)s_pGLWidget;
	glPolygonMode(GL_FRONT,GL_LINE);
	glLineWidth((GLfloat)(m_3DAxisWidth));

	glColor3d(m_3DAxisColor.redF(),m_3DAxisColor.greenF(),m_3DAxisColor.blueF());

	glEnable (GL_LINE_STIPPLE);
	if(m_3DAxisStyle == 1) 		glLineStipple (1, 0x1111);
	else if(m_3DAxisStyle== 2) 	glLineStipple (1, 0x0F0F);
	else if(m_3DAxisStyle== 3) 	glLineStipple (1, 0x1C47);
	else						glLineStipple (1, 0xFFFF);// Solid

	// X axis____________
	glBegin(GL_LINES);
		glVertex3d(-.8, 0.0, 0.0);
		glVertex3d( .8, 0.0, 0.0);
	glEnd();
	//Arrow
	glBegin(GL_LINES);
		glVertex3d( 1.0*l,   0.0,   0.0);
		glVertex3d( 0.98*l,  0.015*l, 0.015*l);
	glEnd();
	glBegin(GL_LINES);
		glVertex3d( 1.0*l,  0.0,    0.0);
		glVertex3d( 0.98*l,-0.015*l,-0.015*l);
	glEnd();
	glDisable (GL_LINE_STIPPLE);
	//XLabel
	pGLWidget->renderText( l, 0.0, 0.0, "X", pMainFrame->m_TextFont);


	// Y axis____________
	glEnable (GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex3d(0.0, -l, 0.0);
		glVertex3d(0.0,  l, 0.0);
	glEnd();

	//Arrow
	glBegin(GL_LINES);
		glVertex3d( 0.0,     1.0*l,  0.0);
		glVertex3d( 0.015*l, 0.98*l, 0.015*l);
	glEnd();
	glBegin(GL_LINES);
		glVertex3d( 0.0,     1.0*l,  0.0);
		glVertex3d(-0.015*l, 0.98*l,-0.015*l);
	glEnd();
	glDisable (GL_LINE_STIPPLE);
	//Y Label
	pGLWidget->renderText( 0.0, l, 0.0, "Y", pMainFrame->m_TextFont);


	// Z axis____________
	glEnable (GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex3d(0.0, 0.0, -l);
		glVertex3d(0.0, 0.0,  l);
	glEnd();

	//Arrow
	glBegin(GL_LINES);
		glVertex3d(  0.0,   0.0, 1.0*l);
		glVertex3d( 0.015*l,  0.015*l,  0.98*l);
	glEnd();
	glBegin(GL_LINES);
		glVertex3d( 0.0,    0.0, 1.0*l);
		glVertex3d(-0.015*l, -0.015*l,  0.98*l);
	glEnd();
	glDisable (GL_LINE_STIPPLE);
	//ZLabel
	pGLWidget->renderText( 0.0, 0.0, l, "Z", pMainFrame->m_TextFont);

	glDisable (GL_LINE_STIPPLE);
}


void QMiarex::GLDraw3D()
{
	//
	// creates the OPenGL lists for 3D display
	//
	if (!m_pCurWing)
	{
		m_bResetglGeom = true;
		m_bResetglMesh = true;
		m_bResetglOpp  = true;
	}

	MainFrame * pMainFrame =(MainFrame*)s_pMainFrame;

	glClearColor(pMainFrame->m_BackgroundColor.redF(), pMainFrame->m_BackgroundColor.greenF(), pMainFrame->m_BackgroundColor.blueF(),0.0);

	if(m_bResetglBody && m_pCurBody)
	{
		m_ArcBall.GetMatrix();
		CVector eye(0.0,0.0,1.0);
		CVector up(0.0,1.0,0.0);
		m_ArcBall.SetZoom(0.3,eye,up);

		if(glIsList(BODYGEOM))
		{
			glDeleteLists(BODYGEOM,2);
			m_GLList -=2;
		}
		if(m_pCurBody->m_LineType==BODYPANELTYPE)	   m_GL3dBody.GLCreateBody3DFlatPanels(m_pCurBody);
		else if(m_pCurBody->m_LineType==BODYSPLINETYPE) m_GL3dBody.GLCreateBody3DSplines(m_pCurBody);

		m_bResetglBody = false;
		if(glIsList(BODYMESHPANELS))
		{
			glDeleteLists(BODYMESHPANELS,2);
			m_GLList -=2;
		}
		m_GL3dBody.GLCreateBodyMesh(m_pCurBody);
		m_bResetglBodyMesh = false;
	}

	if(m_bResetglBodyMesh && m_pCurBody)
	{
		if(glIsList(BODYMESHPANELS))
		{
			glDeleteLists(BODYMESHPANELS,2);
			m_GLList -=2;
		}
//		GLCreateBodyMesh();
		m_bResetglBodyMesh = false;
	}

	if(m_bResetglBody || m_bResetglGeom)
	{
		int row, col, NumAngles, NumCircles;
		double R, lat_incr, lon_incr, phi, theta;
		m_ArcBall.GetMatrix();
		CVector eye(0.0,0.0,1.0);
		CVector up(0.0,1.0,0.0);
		m_ArcBall.SetZoom(0.45,eye,up);

		if(glIsList(ARCBALL))
		{
			glDeleteLists(ARCBALL,2);
			m_GLList-=2;
		}
		glNewList(ARCBALL,GL_COMPILE);
		{
			m_GLList++;
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

			glColor3d(0.3,0.3,.5);
			glLineWidth(1.0);

			R = m_ArcBall.ab_sphere;

			NumAngles  = 50;
			NumCircles =  6;
			lat_incr =  90.0 / NumAngles;
			lon_incr = 360.0 / NumCircles;

			for (col = 0; col < NumCircles; col++)
			{
				glBegin(GL_LINE_STRIP);
				{
					phi = (col * lon_incr) * PI/180.0;

					for (row = 1; row < NumAngles-1; row++)
					{
						theta = (row * lat_incr) * PI/180.0;
						glVertex3d(R*cos(phi)*cos(theta)*m_GLScale, R*sin(theta)*m_GLScale, R*sin(phi)*cos(theta)*m_GLScale);
					}
				}
				glEnd();
				glBegin(GL_LINE_STRIP);
				{
					phi = (col * lon_incr ) * PI/180.0;

					for (row = 1; row < NumAngles-1; row++)
					{
						theta = -(row * lat_incr) * PI/180.0;
						glVertex3d(R*cos(phi)*cos(theta)*m_GLScale, R*sin(theta)*m_GLScale, R*sin(phi)*cos(theta)*m_GLScale);
					}
				}
				glEnd();
			}


			glBegin(GL_LINE_STRIP);
			{
				theta = 0.;
				for(col=1; col<35; col++)
				{
					phi = (0.0 + (double)col*360.0/72.0) * PI/180.0;
					glVertex3d(R * cos(phi) * cos(theta)*m_GLScale, R * sin(theta)*m_GLScale, R * sin(phi) * cos(theta)*m_GLScale);
				}
			}
			glEnd();

			glBegin(GL_LINE_STRIP);
			{
				theta = 0.;
				for(col=1; col<35; col++)
				{
					phi = (0.0 + (double)col*360.0/72.0) * PI/180.0;
					glVertex3d(R * cos(-phi) * cos(theta)*m_GLScale, R * sin(theta)*m_GLScale, R * sin(-phi) * cos(theta)*m_GLScale);
				}
			}
			glEnd();
		}
		glEndList();

		glNewList(ARCPOINT,GL_COMPILE);
		{
			m_GLList++;
			glPolygonMode(GL_FRONT,GL_LINE);

			glColor3d(0.3,0.1,.2);
			glLineWidth(2.0);

			NumAngles  = 10;

			lat_incr = 30.0 / NumAngles;
			lon_incr = 30.0 / NumAngles;

			glBegin(GL_LINE_STRIP);
			{
				phi = 0.0;//longitude

				for (row = -NumAngles; row < NumAngles; row++)
				{
					theta = (row * lat_incr) * PI/180.0;
					glVertex3d(R*cos(phi)*cos(theta)*m_GLScale, R*sin(theta)*m_GLScale, R*sin(phi)*cos(theta)*m_GLScale);
				}
			}
			glEnd();

			glBegin(GL_LINE_STRIP);
			{
				theta = 0.;
				for(col=-NumAngles; col<NumAngles; col++)
				{
					phi = (0.0 + (double)col*30.0/NumAngles) * PI/180.0;
					glVertex3d(R * cos(phi) * cos(theta)*m_GLScale, R * sin(theta)*m_GLScale, R * sin(phi) * cos(theta)*m_GLScale);
				}
			}
			glEnd();
		}
		glEndList();
	}

	if(m_bResetglGeom  && (m_iView==W3DVIEW || m_iView==WSTABVIEW))
	{
		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(m_pWingList[iw])
			{
				if(glIsList(WINGSURFACES+iw))
				{
					glDeleteLists(WINGSURFACES+iw,1);
					glDeleteLists(WINGSURFACES+iw+4,1);
					m_GLList-=2;
				}
				GLCreateGeom(this, m_pWingList[iw], WINGSURFACES+iw);
			}
		}
		m_bResetglGeom = false;
	}

	if(m_bResetglMesh && m_bVLMPanels && (m_iView==W3DVIEW || m_iView==WSTABVIEW))
	{
		if(glIsList(MESHPANELS))
		{
			glDeleteLists(MESHPANELS,2);
			m_GLList-=2;
		}
		GLCreateMesh(this, m_Node, m_Panel);
		if(glIsList(VLMCTRLPTS))
		{
			glDeleteLists(VLMCTRLPTS,2);
			m_GLList-=2;
		}
		GLCreateCtrlPts(this, m_Panel);
		GLCreateVortices(this, m_Panel, m_Node, m_pCurWPolar);
		m_bResetglMesh = false;
	}

	if(m_bResetglWake && m_iView==W3DVIEW)
	{
		if(glIsList(WINGWAKEPANELS))
		{
			glDeleteLists(WINGWAKEPANELS,1);
			m_GLList-=1;
		}
		if (m_pCurWPolar && m_pCurWPolar->m_AnalysisMethod==PANELMETHOD)
		{
//			GLCreateWakePanels(WINGWAKEPANELS);
		}
		m_bResetglWake = false;
	}

	if((m_bResetglLegend || m_bResetglLift || m_bResetglOpp) && m_iView==W3DVIEW)
	{
		if(glIsList(PANELFORCEARROWS))
		{
                        glDeleteLists(PANELFORCEARROWS,1);
                        m_GLList -=1;
		}
		GLCreatePanelForce(this, m_pCurWPolar,m_Panel,m_pCurWOpp, m_pCurPOpp);
	}

	if((m_bResetglLift || m_bResetglOpp) && m_iView==W3DVIEW)
	{
		if(glIsList(LIFTFORCE))
		{
			glDeleteLists(LIFTFORCE,1);
			m_GLList -=1;
		}
		if(glIsList(VLMMOMENTS))
		{
			glDeleteLists(VLMMOMENTS,1);
			m_GLList -=1;
		}

		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(glIsList(VLMWINGLIFT))
			{
				glDeleteLists(VLMWINGLIFT+iw,1);
				m_GLList -=1;
			}
			if(glIsList(VLMWINGTOPTRANS+iw))
			{
				glDeleteLists(VLMWINGTOPTRANS+iw,1);
				glDeleteLists(VLMWINGTOPTRANS+iw+4,1);
				m_GLList-=2;
			}
		}

		if ((m_pCurWing && m_pCurWOpp) || (m_pCurPOpp &&m_pCurWOpp) )
		{
			for(int iw=0; iw<MAXWINGS; iw++)
			{
				if(m_pWingList[iw])
				{
					GLCreateLiftStrip(this, m_pWingList[iw], m_pCurWPolar, m_pWOpp[iw], VLMWINGLIFT+iw);
					GLCreateTrans(this, m_pWingList[iw], m_pWOpp[iw], VLMWINGTOPTRANS+iw);
				}
			}

			GLCreateLiftForce(this, m_pCurWPolar, m_pCurWOpp);
			GLCreateMoments(this, m_pCurWing, m_pCurWPolar, m_pCurWOpp);
		}


		m_bResetglLift = false;
	}

	if((m_bResetglDrag || m_bResetglOpp) && m_iView==W3DVIEW)
	{
		if (m_pWingList[0] && m_pWOpp[0])
		{
			for(int iw=0; iw<MAXWINGS; iw++)
			{
				if(glIsList(VLMWINGDRAG+iw ))
				{
					glDeleteLists(VLMWINGDRAG+iw,1);
					m_GLList -=1;
				}
				if(m_pWingList[iw])
				{
					GLCreateDrag(this, m_pWingList[iw], m_pCurWPolar, m_pWOpp[iw], VLMWINGDRAG+iw);
				}
			}
		}
		m_bResetglDrag = false;
	}

	if((m_bResetglDownwash || m_bResetglOpp) && m_iView==W3DVIEW)
	{
		if(m_pWingList[0] && m_pWOpp[0])
		{
			for(int iw=0; iw<MAXWINGS; iw++)
			{
				if(glIsList(VLMWINGWASH+iw))
				{
					glDeleteLists(VLMWINGWASH+iw,1);
					m_GLList--;
				}
				if(m_pWingList[iw])
				{
					GLCreateDownwash(this, m_pWingList[iw],m_pWOpp[iw], VLMWINGWASH+iw);
				}
			}
		}

		m_bResetglDownwash = false;
	}

	if(m_bResetglOpp && m_iView==W3DVIEW)
	{
		if (m_pCurWing && m_pCurWOpp)
		{
			if(glIsList(PANELCP))
			{
				glDeleteLists(PANELCP,1);
				m_GLList-=1;
			}

			GLCreateCp(this, m_Node, m_Panel, m_pCurWOpp, m_pCurPOpp);
		}

		m_bResetglOpp = false;
	}

	if((m_bResetglLegend || m_bResetglOpp || m_bResetglGeom) && (m_iView==W3DVIEW || m_iView==WSTABVIEW))
	{
                //if(glIsList(WINGLEGEND))
                //{
                //	glDeleteLists(WINGLEGEND,1);
                //	m_GLList -= 1;
                //}
                //GLCreateWingLegend(this, m_pCurWing, m_pCurPlane, m_pCurWPolar);

                if(m_pCurWOpp)
                {
                        glDeleteLists(WOPPCPLEGENDCLR,1);
                        m_GLList -= 1;
                        GLCreateCpLegendClr(this);
                }
		m_bResetglLegend = false;
	}

	// draw once without streamlines to update the screen
	// then draw with streamlines
	if((m_bResetglStream || m_bResetglOpp) && m_iView==W3DVIEW)
	{
		if(m_bStream)
		{
			//no need to recalculate if not showing
			if(m_pCurWing && m_pCurWOpp && m_pCurWOpp->m_AnalysisMethod>=2)
			{
				if(glIsList(VLMSTREAMLINES))
				{
					glDeleteLists(VLMSTREAMLINES,1);
					m_GLList -=1;
				}


/*				//back-up the current geometry, before it is tilted for streamlines calculation
				memcpy(m_MemPanel, m_Panel, m_MatSize * sizeof(CPanel));
				memcpy(m_MemNode,  m_Node,  m_nNodes * sizeof(CVector));
				memcpy(m_RefWakePanel, m_WakePanel, m_WakeSize * sizeof(CPanel));
				memcpy(m_RefWakeNode,  m_WakeNode,  m_nWakeNodes * sizeof(CVector));*/

				GLCreateStreamLines(this, m_pWingList, m_Node, m_pCurWPolar, m_pCurWOpp);
				m_bResetglStream = false;

/*				//restore the initial geometry
				memcpy(m_Panel, m_MemPanel, m_MatSize * sizeof(CPanel));
				memcpy(m_Node,  m_MemNode,  m_nNodes  * sizeof(CVector));
				memcpy(m_WakePanel, m_RefWakePanel, m_WakeSize   * sizeof(CPanel));
				memcpy(m_WakeNode,  m_RefWakeNode,  m_nWakeNodes * sizeof(CVector));*/

			}
		}
	}

	if((m_bResetglFlow || m_bResetglOpp) && m_iView==W3DVIEW)
	{
		if(m_bSpeeds)
		{
			if(m_pCurWing && m_pCurWOpp && m_pCurWOpp->m_AnalysisMethod>=2)
			{
				if(glIsList(SURFACESPEEDS))
				{
					glDeleteLists(SURFACESPEEDS,1);
					m_GLList -=1;
				}
				GLCreateSurfSpeeds(this, m_Panel, m_pCurWPolar, m_pCurWOpp);
				m_bResetglFlow = false;
			}
		}
	}
}


void QMiarex::GLDrawMasses()
{
	//
	// draws the point masses, the object masses, and the CG position on the OpenGL viewport
	//
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	GLWidget *pGLWidget = (GLWidget*)s_pGLWidget;
	QString MassUnit;
	GetWeightUnit(MassUnit, pMainFrame->m_WeightUnit);
	glColor3d(m_MassColor.redF(), m_MassColor.greenF(), m_MassColor.blueF());
	double radius = .01;//2cm
	double zdist = 25.0/(double)m_r3DCltRect.width();

	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pWingList[iw])
		{
//			glColor3d(m_MassColor.redF()*.75, m_MassColor.greenF()*.75, m_MassColor.blueF()*.75);
			glColor3d(0.0, 0.0, 0.7);
			glPushMatrix();
			{
				if(m_pCurPlane)
				{
					glTranslated(m_pCurPlane->WingLE(iw).x,
							   m_pCurPlane->WingLE(iw).y,
							   m_pCurPlane->WingLE(iw).z);
					if(m_pWingList[iw]->m_bIsFin) glTranslated(0.0,0.0, m_pWingList[iw]->m_ProjectedSpan/4.0);
					else                          glTranslated(0.0, m_pWingList[iw]->m_ProjectedSpan/4.0,0.0);


					pGLWidget->renderText(0.0, 0.0, zdist,
									  m_pWingList[iw]->m_WingName+
									  QString(" %1").arg(m_pWingList[iw]->m_VolumeMass*pMainFrame->m_kgtoUnit, 7,'g',3)+
									  MassUnit);
				}
			}
			glPopMatrix();

			glColor3d(m_MassColor.redF(), m_MassColor.greenF(), m_MassColor.blueF());
			for(int im=0; im<m_pWingList[iw]->m_NMass; im++)
			{
				glPushMatrix();
				{
					if(m_pCurPlane)
					{
						glTranslated(m_pCurPlane->WingLE(iw).x,
								   m_pCurPlane->WingLE(iw).y,
								   m_pCurPlane->WingLE(iw).z);
					}
					glTranslated(m_pWingList[iw]->m_MassPosition[im].x,
							   m_pWingList[iw]->m_MassPosition[im].y,
							   m_pWingList[iw]->m_MassPosition[im].z);
					GLRenderSphere(m_MassColor,radius,18,18);
					pGLWidget->renderText(0.0, 0.0, 0.0 +.02,
									  m_pWingList[iw]->m_MassTag[im]
									  +QString(" %1").arg(m_pWingList[iw]->m_MassValue[im]*pMainFrame->m_kgtoUnit, 7,'g',3)
									  +MassUnit);
				}
				glPopMatrix();
			}
		}
	}
	if(m_pCurPlane)
	{
		glColor3d(m_MassColor.redF(), m_MassColor.greenF(), m_MassColor.blueF());
		for(int im=0; im<m_pCurPlane->m_NMass; im++)
		{
			glPushMatrix();
			{
				glTranslated(m_pCurPlane->m_MassPosition[im].x,m_pCurPlane->m_MassPosition[im].y,m_pCurPlane->m_MassPosition[im].z);
				GLRenderSphere(m_MassColor,radius,18,18);
				pGLWidget->renderText(0.0,0.0,0.0+.02,
								  m_pCurPlane->m_MassTag[im]
								  +QString(" %1").arg(m_pCurPlane->m_MassValue[im]*pMainFrame->m_kgtoUnit, 7,'g',3)
								  +MassUnit);
			}
			glPopMatrix();
		}

	}
	if(m_pCurBody)
	{
//		glColor3d(m_MassColor.redF()*.75, m_MassColor.greenF()*.75, m_MassColor.blueF()*.75);
		glColor3d(0.0, 0.0, 0.7);

		glPushMatrix();
		{
			if(m_pCurPlane)
			{
				glTranslated(m_pCurPlane->BodyPos().x+m_pCurBody->m_FramePosition[0].x,
						   m_pCurPlane->BodyPos().y+m_pCurBody->m_FramePosition[0].y,
						   m_pCurPlane->BodyPos().z+m_pCurBody->m_FramePosition[0].z);

				pGLWidget->renderText(0.0, 0.0, zdist,
								  m_pCurBody->m_BodyName+
								  QString(" %1").arg(m_pCurBody->m_VolumeMass*pMainFrame->m_kgtoUnit, 7,'g',3)+
								  MassUnit);
			}
		}
		glPopMatrix();
		glColor3d(m_MassColor.redF(), m_MassColor.greenF(), m_MassColor.blueF());
		for(int im=0; im<m_pCurBody->m_NMass; im++)
		{
			glPushMatrix();
			{
				glTranslated(m_pCurBody->m_MassPosition[im].x,m_pCurBody->m_MassPosition[im].y,m_pCurBody->m_MassPosition[im].z);
				if(m_pCurPlane)
				{
					glTranslated(m_pCurPlane->BodyPos().x,
							   m_pCurPlane->BodyPos().y,
							   m_pCurPlane->BodyPos().z);
				}

				GLRenderSphere(m_MassColor,radius,18,18);

				pGLWidget->renderText(0.0, 0.0, 0.0+.02,
								  m_pCurBody->m_MassTag[im]
								  +QString(" %1").arg(m_pCurBody->m_MassValue[im]*pMainFrame->m_kgtoUnit, 7,'g',3)
								  +MassUnit);
			}
			glPopMatrix();
		}
	}
	//plot CG
	if(m_pCurPlane || m_pCurWing)
	{
		CVector CoG;
		double Mass=0.0;
		if(m_pCurPlane)
		{
			CoG = m_pCurPlane->CoG();
			Mass = m_pCurPlane->TotalMass();
		}
		else if(m_pCurWing)
		{
			CoG = m_pCurWing->m_CoG;
			Mass = m_pCurWing->m_TotalMass;
		}
		glPushMatrix();
		{
			glTranslated(CoG.x,CoG.y,CoG.z);
			GLRenderSphere(QColor(255,0,0),radius,18,18);
			pGLWidget->renderText(0.0, 0.0, 0.0+.02,
							  "CoG "+QString("%1").arg(Mass*pMainFrame->m_kgtoUnit, 7,'g',3)
							  +MassUnit);
		}
		glPopMatrix();
	}
}


void QMiarex::GLDrawFoils()
{
	//
	// Prints the foil names on the OpenGl viewport
	//
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	GLWidget *pGLWidget = (GLWidget*)s_pGLWidget;
	int j;
	CFoil *pFoil;


	glColor3d(pMainFrame->m_TextColor.redF(), pMainFrame->m_TextColor.greenF(), pMainFrame->m_TextColor.blueF());

	for(int iw=0; iw<4; iw++)
	{
		if(m_pWingList[iw])
		{
			for(j=0; j<m_pWingList[iw]->m_NSurfaces; j++)
			{
				pFoil = m_pWingList[iw]->m_Surface[j].m_pFoilA;

				if(pFoil)
				{
					pGLWidget->renderText(m_pWingList[iw]->m_Surface[j].m_TA.x, m_pWingList[iw]->m_Surface[j].m_TA.y, m_pWingList[iw]->m_Surface[j].m_TA.z,
											pFoil->m_FoilName, pMainFrame->m_TextFont);
				}
			}
			j = m_pWingList[iw]->m_NSurfaces-1;
			pFoil = m_pWingList[iw]->m_Surface[j].m_pFoilB;
			if(pFoil)
			{
				pGLWidget->renderText(m_pWingList[iw]->m_Surface[j].m_TB.x, m_pWingList[iw]->m_Surface[j].m_TB.y, m_pWingList[iw]->m_Surface[j].m_TB.z,
										pFoil->m_FoilName, pMainFrame->m_TextFont);
			}
		}
	}
}


void QMiarex::GLInverseMatrix()
{
	//
	// inverses the rotation matrix by transposition
	//
	int i,j;

	for(i=0 ; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			MatOut[j][i] = MatIn[i][j];
		}
	}
}

void QMiarex::GLRenderView()
{
	//
	// Renders the OpenGl 3D view
	//
	GLWidget *pGLWidget = (GLWidget*)s_pGLWidget;
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

		GLSetupLight();
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		if(m_pCurWOpp && m_pCurWOpp->m_Type==STABILITYPOLAR)
		{
			MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
			QString strong = QString(tr("Time =")+"%1s").arg(m_ModeTime,6,'f',3);
			pGLWidget->renderText(15, 15, strong, pMainFrame->m_TextFont);
		}

		if(m_bShowLight)
		{
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			glPushMatrix();
			{
				glTranslated(( m_GLLightDlg.m_XLight+ m_UFOOffset.x)*m_GLScale,
								 ( m_GLLightDlg.m_YLight+ m_UFOOffset.y)*m_GLScale,
									m_GLLightDlg.m_ZLight*m_GLScale);
				double radius = (m_GLLightDlg.m_ZLight+2.0)/40.0*m_GLScale;
				QColor color;
				color = QColor((int)(m_GLLightDlg.m_Red  *255),
							(int)(m_GLLightDlg.m_Green*255),
							(int)(m_GLLightDlg.m_Blue *255));
				GLRenderSphere(color,radius,18,18);
			}
			glPopMatrix();
		}

		glLoadIdentity();
		if(m_bCrossPoint && m_bArcball)
		{
			glPushMatrix();
			{
				glTranslated(m_UFOOffset.x, m_UFOOffset.y,  0.0);
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
				glTranslated(m_UFOOffset.x, m_UFOOffset.y,  0.0);
				m_ArcBall.Rotate();
				glCallList(ARCBALL);
			}
			glPopMatrix();
		}
		glTranslated(m_UFOOffset.x, m_UFOOffset.y,  0.0);

		m_ArcBall.Rotate();

		glScaled(m_glScaled, m_glScaled, m_glScaled);
		glTranslated(m_glRotCenter.x, m_glRotCenter.y, m_glRotCenter.z);
		if(m_bAxes)  GLDrawAxes();

		if(m_pCurWPolar && m_pCurWPolar->m_Type==STABILITYPOLAR)
		{
			glTranslated(m_ModeState[0], m_ModeState[1], m_ModeState[2]);
			glRotated(m_ModeState[3]*180.0/PI, 1.0, 0.0 ,0.0);
			glRotated(m_ModeState[4]*180.0/PI, 0.0, 1.0 ,0.0);
			glRotated(m_ModeState[5]*180.0/PI, 0.0, 0.0 ,1.0);
			if(fabs(m_pCurWPolar->m_Beta)>0.001) glRotated(-m_pCurWPolar->m_Beta, 0.0, 0.0, 1.0);
		}

		GLCallViewLists();

		if(m_bFoilNames) GLDrawFoils();

		if(m_bShowMasses) GLDrawMasses();


		glLoadIdentity();
		glDisable(GL_CLIP_PLANE1);
                if(m_pCurWing)			GLDrawWingLegend(this, m_pCurWing, m_pCurPlane, m_pCurWPolar); //glCallList(WINGLEGEND);
		if(m_pCurWOpp)			GLDrawWOppLegend(this, m_pCurWing, m_pCurWOpp); // glCallList(WOPPLEGEND);
		if (m_b3DCp && m_pCurWOpp && m_pCurWOpp->m_AnalysisMethod>=2 )
		{
                        GLDrawCpLegend(this);
                        //glCallList(WOPPCPLEGENDTXT);
                        glCallList(WOPPCPLEGENDCLR);
		}
		else if (m_bPanelForce && m_pCurWOpp && m_pCurWOpp->m_AnalysisMethod>=2 )
		{
                        GLDrawPanelForceLegend(this, m_pCurWPolar,m_Panel,m_pCurWOpp, m_pCurPOpp);
                        //glCallList(PANELFORCELEGENDTXT);
                        glCallList(WOPPCPLEGENDCLR);
		}
	}
	glPopMatrix();
	glDisable(GL_CLIP_PLANE1);
}


void QMiarex::GLRenderSphere(QColor cr, double radius, int NumLongitudes, int NumLatitudes)
{
	//
	// Render the sphere representing the light or point masses
	//
	static double start_lat, start_lon,lat_incr, lon_incr, R;
	static double phi1, phi2, theta1, theta2;
	static GLdouble u[3], v[3], w[3], n[3];
	static int row, col;

	glDisable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT,GL_FILL);
	glBegin(GL_TRIANGLES);
	glColor3d(cr.redF(),cr.greenF(),cr.blueF());

	start_lat = -90;
	start_lon = 0.0;
	R = radius;

	lat_incr = 180.0 / NumLatitudes;
	lon_incr = 360.0 / NumLongitudes;

	for (col = 0; col < NumLongitudes; col++)
	{
		phi1 = (start_lon + col * lon_incr) * PI/180.0;
		phi2 = (start_lon + (col + 1) * lon_incr) * PI/180.0;

		for (row = 0; row < NumLatitudes; row++)
		{
			theta1 = (start_lat + row * lat_incr) * PI/180.0;
			theta2 = (start_lat + (row + 1) * lat_incr) * PI/180.0;

			u[0] = R * cos(phi1) * cos(theta1);//x
			u[1] = R * sin(theta1);//y
			u[2] = R * sin(phi1) * cos(theta1);//z

			v[0] = R * cos(phi1) * cos(theta2);//x
			v[1] = R * sin(theta2);//y
			v[2] = R * sin(phi1) * cos(theta2);//z

			w[0] = R * cos(phi2) * cos(theta2);//x
			w[1] = R * sin(theta2);//y
			w[2] = R * sin(phi2) * cos(theta2);//z

			NormalVector(u,v,w,n);

			glNormal3dv(n);
			glVertex3dv(u);
			glVertex3dv(v);
			glVertex3dv(w);

			v[0] = R * cos(phi2) * cos(theta1);//x
			v[1] = R * sin(theta1);//y
			v[2] = R * sin(phi2) * cos(theta1);//z

			NormalVector(u,w,v,n);
			glNormal3dv(n);
			glVertex3dv(u);
			glVertex3dv(w);
			glVertex3dv(v);
		}
	}
	glEnd();
}


void QMiarex::GLSetupLight()
{
	//
	// Sets the light parameters for the OpenGl display
	//

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);    // the ambient light
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHT4);
	glDisable(GL_LIGHT5);
	glDisable(GL_LIGHT6);
	glDisable(GL_LIGHT7);

	float fLightAmbient0[4];
	float fLightDiffuse0[4];
	float fLightSpecular0[4];
	float fLightPosition0[4];

	float LightFactor = 1.0f;
	if(m_pCurWing)      LightFactor =  (GLfloat)pow(m_pCurWing->m_PlanformSpan/2.0,0.1);
//	if(LightFactor>1.0) LightFactor = 1.0f;

	// the ambient light conditions.
	fLightAmbient0[0] = LightFactor*m_GLLightDlg.m_Ambient * m_GLLightDlg.m_Red; // red component
	fLightAmbient0[1] = LightFactor*m_GLLightDlg.m_Ambient * m_GLLightDlg.m_Green; // green component
	fLightAmbient0[2] = LightFactor*m_GLLightDlg.m_Ambient * m_GLLightDlg.m_Blue; // blue component
	fLightAmbient0[3] = 1.0; // alpha

	fLightDiffuse0[0] = LightFactor*m_GLLightDlg.m_Diffuse * m_GLLightDlg.m_Red; // red component
	fLightDiffuse0[1] = LightFactor*m_GLLightDlg.m_Diffuse * m_GLLightDlg.m_Green; // green component
	fLightDiffuse0[2] = LightFactor*m_GLLightDlg.m_Diffuse * m_GLLightDlg.m_Blue; // blue component
	fLightDiffuse0[3] = 1.0; // alpha

	fLightSpecular0[0] = LightFactor*m_GLLightDlg.m_Specular * m_GLLightDlg.m_Red; // red component
	fLightSpecular0[1] = LightFactor*m_GLLightDlg.m_Specular * m_GLLightDlg.m_Green; // green component
	fLightSpecular0[2] = LightFactor*m_GLLightDlg.m_Specular * m_GLLightDlg.m_Blue; // blue component
	fLightSpecular0[3] = 1.0; // alpha

	// And finally, its position

	fLightPosition0[0] = (GLfloat)((m_GLLightDlg.m_XLight));
	fLightPosition0[1] = (GLfloat)((m_GLLightDlg.m_YLight + m_UFOOffset.y));
	fLightPosition0[2] = (GLfloat)((m_GLLightDlg.m_ZLight));
	fLightPosition0[3] = 1.0; // W (positional light)



	// Enable the basic light
	glLightfv(GL_LIGHT0, GL_AMBIENT,  fLightAmbient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  fLightDiffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, fLightSpecular0);
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPosition0);


	float fMatAmbient[4]   = {m_GLLightDlg.m_MatAmbient,  m_GLLightDlg.m_MatAmbient,   m_GLLightDlg.m_MatAmbient,  1.0f};
	float fMatSpecular[4]  = {m_GLLightDlg.m_MatSpecular, m_GLLightDlg.m_MatSpecular,  m_GLLightDlg.m_MatSpecular, 1.0f};
	float fMatDiffuse[4]   = {m_GLLightDlg.m_MatDiffuse,  m_GLLightDlg.m_MatDiffuse,   m_GLLightDlg.m_MatDiffuse,  1.0f};
	float fMatEmission[4]  = {m_GLLightDlg.m_MatEmission, m_GLLightDlg.m_MatEmission,  m_GLLightDlg.m_MatEmission, 1.0f};

	if(m_GLLightDlg.m_bColorMaterial)
	{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
//		glColorMaterial(GL_FRONT, GL_AMBIENT);
//		glColorMaterial(GL_FRONT, GL_DIFFUSE);
//		glColorMaterial(GL_FRONT, GL_SPECULAR);

	}
	else
	{
		glDisable(GL_COLOR_MATERIAL);

	}
	glMaterialfv(GL_FRONT, GL_SPECULAR,  fMatSpecular);
	glMaterialfv(GL_FRONT, GL_AMBIENT,   fMatAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   fMatDiffuse);
	glMaterialfv(GL_FRONT, GL_EMISSION,  fMatEmission);
	glMateriali( GL_FRONT, GL_SHININESS, m_GLLightDlg.m_iMatShininess);

	if(m_GLLightDlg.m_bDepthTest)  glEnable(GL_DEPTH_TEST);     else glDisable(GL_DEPTH_TEST);
	if(m_GLLightDlg.m_bCullFaces)  glEnable(GL_CULL_FACE);      else glDisable(GL_CULL_FACE);
	if(m_GLLightDlg.m_bSmooth)     glEnable(GL_POLYGON_SMOOTH); else glDisable(GL_POLYGON_SMOOTH);
	if(m_GLLightDlg.m_bShade)      glShadeModel(GL_SMOOTH);     else glShadeModel(GL_FLAT);

	if(m_GLLightDlg.m_bLocalView) glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,0);
	else                          glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,1);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);
}



bool QMiarex::InitializePanels()
{
	//
	// Following the selection of a wing or a plane, this subroutine creates the panels
	// associated to the surface objects.
	//
	// The surface objects have already been created
	//
	// m_Panel is the array of panels in the following order
	//         main wing left side, main wing right side
	//         elevator 
	//         fin
	//         body
	// Each panel gets the index of its symmetric, if any, else this index is set to -1
	//
	// A copy of the panels is saved to the MemPanel and MemNode arrays
	//

	if(!m_pCurWing) return false;
	int j, k, l, Nel, p, pp, HalfSize, nx, nh;
	QString strong;

/*	QProgressDialog dlg(this);
	dlg.setLabelText(tr("Creating Elements... please Wait"));
	dlg.setMinimum(0);
	dlg.setMaximum(100);
	dlg.setWindowModality(Qt::WindowModal);
	dlg.setValue(0);*/

	// first check that the total number of panels that will be created does not exceed
	// the limit set by the parameter VLMMAXMATSIZE
	m_MatSize = 0;

	//Count the wing panels
	for (j=0; j<m_NSurfaces; j++)
	{
		m_MatSize += m_pSurface[j]->m_NXPanels * m_pSurface[j]->m_NYPanels ;
	}

	if(m_pCurWPolar && m_pCurWPolar->m_AnalysisMethod==PANELMETHOD && !m_pCurWPolar->m_bThinSurfaces)
	{
		m_MatSize *= 2;
		for (j=0; j<m_NSurfaces; j++)
		{
			if(m_pSurface[j]->m_bIsTipLeft || m_pSurface[j]->m_bIsTipRight)
				m_MatSize += m_pSurface[j]->m_NXPanels;//tip patches
		}
	}

	// add the number of body panels
	bool bBodyEl = false;
	if(m_pCurBody)
	{
		if(m_pCurBody->m_LineType==BODYPANELTYPE)
		{
			nx = 0;
			for(int i=0; i<m_pCurBody->m_NStations-1; i++) nx+=m_pCurBody->m_xPanels[i];
			nh = 0;
			for(int i=0; i<m_pCurBody->m_NSideLines-1; i++) nh+=m_pCurBody->m_hPanels[i];
			m_MatSize += nx*nh*2;
		}
		else m_MatSize += 2 * m_pCurBody->m_nxPanels * m_pCurBody->m_nhPanels;

		//create the body elements only if there is a body, and the analysis is not of the VLM Type
		if(!m_pCurWPolar)                                     bBodyEl = true;//no risk...
		else if(m_pCurWPolar->m_AnalysisMethod==PANELMETHOD)  bBodyEl = true;
		else                                                  bBodyEl = false;
	}
	if(m_MatSize>VLMMAXMATSIZE)
	{
		strong = QString(tr("The total number of panels is %1. The Max Number is %2.\nA reduction of the number of panels is required"))
						  .arg(m_MatSize).arg(VLMMAXMATSIZE);
		m_MatSize = 0;
		m_nNodes  = 0;

		MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
		QMessageBox::warning(pMainFrame, "Warning", strong);
		return false;
	}

	// all set to create the panels
	
	m_MatSize     = 0;
	m_nNodes      = 0;
	m_NWakeColumn = 0;
	m_nWakeNodes  = 0;
	m_WakeSize    = 0;

	memset(m_Panel, 0, sizeof(m_Panel));
	memset(m_Node,  0, sizeof(m_Node));

	CPanel *ptr = m_Panel;

//	dlg.setValue(5);
	int NXWakePanels;
	if(m_pCurWPolar)	NXWakePanels = m_pCurWPolar->m_NXWakePanels;
	else                NXWakePanels = 1;

	int coef = 1;
	if(m_pCurWPolar)
	{
		if(m_pCurWPolar->m_bThinSurfaces) coef = 1;
		else                              coef = 2;
	}

	for(int iw=0; iw<4; iw++)
	{
		if(m_pWingList[iw])
		{
			m_pWingList[iw]->m_MatSize = 0;
			for(j=0; j<m_pWingList[iw]->m_NSurfaces;j++)
			{
				m_pWingList[iw]->m_Surface[j].ResetFlap();
				Nel = CreateElements(m_pWingList[iw]->m_Surface+j);
				m_pWingList[iw]->m_MatSize += Nel;
			}
			m_pWingList[iw]->m_pPanel = ptr;
			ptr += m_pWingList[iw]->m_MatSize;

			//create symetry properties between panels
			if((m_pWingList[iw]->m_bIsFin && !m_pWingList[iw]->m_bDoubleFin) || !m_pWingList[iw]->m_bSymetric)
			{
				for(p=0; p<m_pWingList[iw]->m_MatSize; p++) m_pWingList[iw]->m_pPanel[p].m_iSym=-1;

			}

			HalfSize = m_pWingList[iw]->m_MatSize/2;
			p  = HalfSize-1;
			pp = HalfSize;
			for(j=0; j<m_pWingList[iw]->m_NSurfaces; j++)
			{
				if(m_pWingList[iw]->m_Surface[j].m_bIsRightSurf)
				{
					for(k=0; k<m_pWingList[iw]->m_Surface[j].m_NYPanels; k++)
					{
						for(l=0; l<m_pWingList[iw]->m_Surface[j].m_NXPanels*coef; l++)
						{
							m_pWingList[iw]->m_pPanel[p].m_iSym    = m_pWingList[iw]->m_pPanel[pp+m_pWingList[iw]->m_Surface[j].m_NXPanels*coef-l-1].m_iElement;
							p--;
						}
						pp += m_pWingList[iw]->m_Surface[j].m_NXPanels*coef;
					}
					if(m_pWingList[iw]->m_Surface[j].m_bIsTipRight && (m_pCurWPolar && !m_pCurWPolar->m_bThinSurfaces))
					{
						for(l=0; l<m_pWingList[iw]->m_Surface[j].m_NXPanels; l++)
						{
							m_pWingList[iw]->m_pPanel[p].m_iSym    = m_pWingList[iw]->m_pPanel[pp+m_pWingList[iw]->m_Surface[j].m_NXPanels-l-1].m_iElement;
							p--;
						}
					}
				}
			}
		}
	}

	if(bBodyEl)
	{
		Nel = CreateBodyElements();

		m_pCurBody->m_pPanel = ptr;
/*		HalfSize = m_pCurBody->m_NElements/2;
		p  = HalfSize-1;
		pp = HalfSize;

		for(k=0; k<m_pCurBody->m_nxPanels; k++)
		{
			for(l=0; l<m_pCurBody->m_nhPanels; l++)
			{
				m_pCurBody->m_pPanel[p].m_iSym    = m_pCurBody->m_pPanel[pp+m_pCurBody->m_nhPanels-l-1].m_iElement;
				p--;
			}
			pp += m_pCurBody->m_nhPanels;
		}*/
	}

	//back-up the current geometry
	memcpy(&m_MemPanel, &m_Panel, m_MatSize* sizeof(CPanel));
	memcpy(&m_MemNode,  &m_Node,  m_nNodes * sizeof(CVector));


//	dlg.setValue(100);

	return true;
}


CWOpp* QMiarex::InsertWOpp(CWOpp *pNewPoint)
{
	//
	// loads a WOpPoint from a file
	// adds it to the WOpp array
	//
	int i;
	bool bIsInserted = false;
	CWOpp* pWOpp;

	// add the WOpPoint to the WOpPoint Array for the current WingName
	for (i=0; i<m_poaWOpp->size(); i++)
	{
		pWOpp = (CWOpp*)m_poaWOpp->at(i);
		if (pNewPoint->m_WingName == pWOpp->m_WingName)
		{
			if (pNewPoint->m_PlrName == pWOpp->m_PlrName)
			{
				if(pNewPoint->m_Type !=4)
				{
					if(fabs(pNewPoint->m_Alpha - pWOpp->m_Alpha)<0.005)
					{
						//replace existing point
						if (m_pCurWOpp == pWOpp) m_pCurWOpp = NULL;
						m_poaWOpp->removeAt(i);
						delete pWOpp;
						m_poaWOpp->insert(i, pNewPoint);
						bIsInserted = true;
						i = m_poaWOpp->size();// to break
					}
					else if (pNewPoint->m_Alpha > pWOpp->m_Alpha)
					{
						//insert point
						m_poaWOpp->insert(i, pNewPoint);
						bIsInserted = true;
						i = m_poaWOpp->size();// to break
					}
				}
				else
				{
					if(fabs(pNewPoint->m_QInf - pWOpp->m_QInf)<0.005)
					{
						//replace existing point
						if (m_pCurWOpp == pWOpp) m_pCurWOpp = NULL;
						m_poaWOpp->removeAt(i);
						delete pWOpp;
						m_poaWOpp->insert(i, pNewPoint);
						bIsInserted = true;
						i = m_poaWOpp->size();// to break
					}
					else if (pNewPoint->m_QInf > pWOpp->m_QInf)
					{
						//insert point
						m_poaWOpp->insert(i, pNewPoint);
						bIsInserted = true;
						i = m_poaWOpp->size();// to break
					}
				}
			}
		}
	}
	if (!bIsInserted) 	m_poaWOpp->append(pNewPoint);
	return pNewPoint;
}




bool QMiarex::Intersect(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB, CVector const &Normal,
			   CVector const &A,  CVector const &U,  CVector &I, double &dist)
{
	//
	// Returns the intersection of a ray with the object's panels
	// The ray is defined by a mouse click and is perpendicular to the viewport
	//	A is the ray's origin,
	//	U is the ray's direction
	//	LA, LB, TA, TB define a quadrangle in 3D space.
	//	N is the normal to the quadrangle
	//	I is the resulting intersection point of the ray and the quadrangle, if inside the quadrangle
	//	dist = |AI|
	//	The return value is true if intersection inside the quadrangle, false otherwise
	//
	bool b1, b2, b3, b4;
	double r,s;

	r = (LA.x-A.x)*Normal.x + (LA.y-A.y)*Normal.y + (LA.z-A.z)*Normal.z ;
	s = U.x*Normal.x + U.y*Normal.y + U.z*Normal.z;

	dist = 10000.0;

	if(fabs(s)>0.0)
	{
		dist = r/s;

		//inline operations to save time
		P.x = A.x + U.x * dist;
		P.y = A.y + U.y * dist;
		P.z = A.z + U.z * dist;

		// P is inside the panel if on left side of each panel side
		W.x = P.x  - TA.x;
		W.y = P.y  - TA.y;
		W.z = P.z  - TA.z;
		V.x = TB.x - TA.x;
		V.y = TB.y - TA.y;
		V.z = TB.z - TA.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		if(T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0) b1 = true; else b1 = false;

		W.x = P.x  - TB.x;
		W.y = P.y  - TB.y;
		W.z = P.z  - TB.z;
		V.x = LB.x - TB.x;
		V.y = LB.y - TB.y;
		V.z = LB.z - TB.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		if(T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0) b2 = true; else b2 = false;

		W.x = P.x  - LB.x;
		W.y = P.y  - LB.y;
		W.z = P.z  - LB.z;
		V.x = LA.x - LB.x;
		V.y = LA.y - LB.y;
		V.z = LA.z - LB.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		if(T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0) b3 = true; else b3 = false;

		W.x = P.x  - LA.x;
		W.y = P.y  - LA.y;
		W.z = P.z  - LA.z;
		V.x = TA.x - LA.x;
		V.y = TA.y - LA.y;
		V.z = TA.z - LA.z;
		T.x =  V.y * W.z - V.z * W.y;
		T.y = -V.x * W.z + V.z * W.x;
		T.z =  V.x * W.y - V.y * W.x;
		if(T.x*T.x+T.y*T.y+T.z*T.z <1.0e-10 || T.x*Normal.x+T.y*Normal.y+T.z*Normal.z>=0.0) b4 = true; else b4 = false;

		if(b1 && b2 && b3 && b4)
		{
			I.Set(P.x, P.y, P.z);
			return true;
		}
	}
	return false;
}



int QMiarex::IsWakeNode(CVector &Pt)
{
	//
	// returns the index of a wake node if found, else returns NULL
	//
	int in;
	for (in=0; in<m_nWakeNodes; in++)
	{
		if(Pt.IsSame(m_WakeNode[in])) return in;
	}
	return -1;
}


int QMiarex::IsNode(CVector &Pt)
{
	//
	// returns the index of a node if found, else returns NULL
	//
	int in;
//	for (in=0; in<m_nNodes; in++)
	// explore in reverse order, since we have better chance of 
	// finding the node close to the last point when creating elements
	// 
	for (in=m_nNodes-1; in>=0; in--)
	{
		if(Pt.IsSame(m_Node[in])) return in;
	}
	return -1;
}



void QMiarex::JoinSurfaces(CSurface *pLeftSurf, CSurface *pRightSurf, int pl, int pr)
{
	// 
	// At panels on the side of the surfaces, connects the element tothe next surface
	//
	// In the case where the number of chordwise panels is different between two adjacent surfaces,
	// We need to correct the ideal connection that was set in the CreateElements() method.
	// This is the case for instance for a flap.
	// The algorith below is not robust... ideally the connections should be set manually
	//
	// Use VSAERO method
	//

	if(!m_pCurWPolar || m_pCurWPolar->m_AnalysisMethod!=3) return;//panel analysis only

	//pl and pr are respectively the left surface's and the right surface's first panel index
	int ls, lr, lclose, ppl, ppr;
	double dist, x,y,z, mindist;
	lclose=0;
	CVector MidNormal = pLeftSurf->Normal + pRightSurf->Normal;
	MidNormal.Normalize();

	int coef = 1;
	if(m_pCurWPolar && m_pCurWPolar->m_AnalysisMethod==PANELMETHOD && !m_pCurWPolar->m_bThinSurfaces) coef = 2;

	//left surface's right side
	ppl = pl;
	ppr = pr;
	if(pLeftSurf->m_bIsTipLeft && !m_pCurWPolar->m_bThinSurfaces) ppl+= pLeftSurf->m_NXPanels;//left tip patch
	ppl += (pLeftSurf->m_NYPanels-1) * coef*pLeftSurf->m_NXPanels;
	//ppl is now set at left surface's first bottom panel of tip right strip

	//Process left bottom side first
	for (ls=0; ls<pLeftSurf->m_NXPanels; ls++)
	{
		if(ls>=pLeftSurf->m_NXFlap) //flaps are not connected
		{
			mindist = 1.0e100;
			for (lr=0; lr<pRightSurf->m_NXPanels; lr++)
			{
				//get distance from panel's normal plane as per NASA 4023 VSAERO fig.10
				x = m_Panel[ppr+lr].CollPt.x - m_Panel[ppl+ls].CollPt.x;
				y = m_Panel[ppr+lr].CollPt.y - m_Panel[ppl+ls].CollPt.y;
				z = m_Panel[ppr+lr].CollPt.z - m_Panel[ppl+ls].CollPt.z;
				dist = fabs(x*m_Panel[ppl+ls].l.x + y*m_Panel[ppl+ls].l.y + z*m_Panel[ppl+ls].l.z);
				if(dist<mindist)
				{
					lclose = lr;
					mindist = dist ;
				}
			}
			if(lclose>=pRightSurf->m_NXFlap)
			{
				m_Panel[ppl+ls].m_iPL = ppr+lclose;

			}
			else
				m_Panel[ppl+ls].m_iPL = -1;
		}
		else
			m_Panel[ppl+ls].m_iPL = -1;//flap is not connected
	}

	//Process left top side next
	for (ls=pLeftSurf->m_NXPanels;ls<coef*pLeftSurf->m_NXPanels; ls++)
	{
		if(ls < coef*pLeftSurf->m_NXPanels-pLeftSurf->m_NXFlap)
		{
			mindist = 1.0e100;
			for (lr=pRightSurf->m_NXPanels; lr<coef*pRightSurf->m_NXPanels; lr++)
			{
				//get distance from panel's normal plane as per NASA 4023 VSAERO fig.10
				x = m_Panel[ppr+lr].CollPt.x - m_Panel[ppl+ls].CollPt.x;
				y = m_Panel[ppr+lr].CollPt.y - m_Panel[ppl+ls].CollPt.y;

				z = m_Panel[ppr+lr].CollPt.z - m_Panel[ppl+ls].CollPt.z;
				dist = fabs(x*m_Panel[ppl+ls].l.x + y*m_Panel[ppl+ls].l.y + z*m_Panel[ppl+ls].l.z);
				if(dist<mindist)
				{
					lclose = lr;
					mindist = dist ;
				}
			}
			if(lclose< coef*pRightSurf->m_NXPanels-pRightSurf->m_NXFlap)
			{
				m_Panel[ppl+ls].m_iPR = ppr+lclose;
			}
			else
				m_Panel[ppl+ls].m_iPR = -1;
		}
		else
			m_Panel[ppl+ls].m_iPR = -1;
	}

	//Move on to right surface's left connections
	//ppr is set at right surface's first bottom panel of tip left strip
	ppl = pl;
	if(pLeftSurf->m_bIsTipLeft && !m_pCurWPolar->m_bThinSurfaces) ppl+= pLeftSurf->m_NXPanels;//left tip patch
	ppl += (pLeftSurf->m_NYPanels-1) * coef*pLeftSurf->m_NXPanels;

	//Process right bottom side
	for (lr=0;lr<pRightSurf->m_NXPanels; lr++)
	{
		if(lr>=pRightSurf->m_NXFlap)
		{
			mindist = 1.0e100;
			for (ls=0; ls<pLeftSurf->m_NXPanels; ls++)
			{
				//get distance from panel's normal plane as per NASA 4023 VSAERO fig.10
				x = m_Panel[ppl+ls].CollPt.x - m_Panel[ppr+lr].CollPt.x;
				y = m_Panel[ppl+ls].CollPt.y - m_Panel[ppr+lr].CollPt.y;
				z = m_Panel[ppl+ls].CollPt.z - m_Panel[ppr+lr].CollPt.z;
				dist = fabs(x*m_Panel[ppr+lr].l.x + y*m_Panel[ppr+lr].l.y + z*m_Panel[ppr+lr].l.z);
				if(dist<mindist)
				{
					lclose = ls;
					mindist = dist ;
				}
			}
			if(lclose>=pLeftSurf->m_NXFlap)
			{
				m_Panel[ppr+lr].m_iPR = ppl+lclose;
			}
			else
				m_Panel[ppr+lr].m_iPR = -1;
		}
		else
			m_Panel[ppr+lr].m_iPR = -1;
	}


	//Process right top side
	for (lr=pRightSurf->m_NXPanels;lr<coef*pRightSurf->m_NXPanels; lr++)
	{
		if(lr < 2*pRightSurf->m_NXPanels-pRightSurf->m_NXFlap)
		{
			mindist = 1.0e100;
			for (ls=pLeftSurf->m_NXPanels; ls<2*pLeftSurf->m_NXPanels; ls++)
			{
				//get distance from panel's normal plane as per NASA 4023 VSAERO fig.10
				x = m_Panel[ppl+ls].CollPt.x - m_Panel[ppr+lr].CollPt.x;
				y = m_Panel[ppl+ls].CollPt.y - m_Panel[ppr+lr].CollPt.y;
				z = m_Panel[ppl+ls].CollPt.z - m_Panel[ppr+lr].CollPt.z;
				dist = fabs(x*m_Panel[ppr+lr].l.x + y*m_Panel[ppr+lr].l.y + z*m_Panel[ppr+lr].l.z);
				if(dist<mindist)
				{
					lclose =  ls;
					mindist = dist ;
				}
			}
			if(lclose < 2*pLeftSurf->m_NXPanels-pLeftSurf->m_NXFlap)
			{
				m_Panel[ppr+lr].m_iPL = ppl+lclose;
			}
			else
				m_Panel[ppr+lr].m_iPL = -1;
		}
		else
			m_Panel[ppr+lr].m_iPL = -1;
	}
}


void QMiarex::keyPressEvent(QKeyEvent *event)
{
	//
	// Capture and dispatch user keyboard input
	//
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	switch (event->key())
	{
		case Qt::Key_Return:
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
			m_bBreak = true;
			if(m_pCurGraph) m_pCurGraph->DeselectPoint();
			
			if(pMainFrame->m_pctrl3DScalesWidget->isVisible()) pMainFrame->m_pctrl3DScalesWidget->hide();
			UpdateView();
			break;
		}
		case Qt::Key_0:
		{
			if(m_iView==WSTABVIEW)
			{
				m_iStabilityView  = STABTIMEVIEW;
//				m_pCurGraph = &m_TimeGraph1;
//				m_pCurTimeGraph= m_pCurGraph;
				SetWPlrLegendPos();
				UpdateView();
			}
			break;
		}
		case Qt::Key_1:
		{
			if(m_iView==WPOLARVIEW)
			{
				m_iWPlrView  = 1;
				m_pCurGraph = m_WPlrGraph;
				m_pCurWPlrGraph = m_pCurGraph;
				SetWPlrLegendPos();
			}
			else if(m_iView==WOPPVIEW)
			{
				m_iWingView =1;
				m_pCurGraph = m_WingGraph;
				m_pCurWingGraph = m_pCurGraph;
				SetWingLegendPos();
			}
			else if(m_iView==WSTABVIEW)
			{
				if(m_iStabilityView==STABTIMEVIEW)
				{
					m_iStabTimeView =1;
					m_pCurGraph = m_TimeGraph;
					m_pCurTimeGraph = m_pCurGraph;
					SetWingLegendPos();
				}
			}
			UpdateView();
			break;
		}
		case Qt::Key_2:
		{
			if(m_iView==WPOLARVIEW)
			{
				m_iWPlrView  = 1;
				m_pCurGraph = m_WPlrGraph+1;
				m_pCurWPlrGraph = m_pCurGraph;
				SetWPlrLegendPos();
			}
			else if(m_iView==WOPPVIEW)
			{
				m_iWingView =1;
				m_pCurGraph = m_WingGraph+1;
				m_pCurWingGraph = m_pCurGraph;
				SetWingLegendPos();
			}
			else if(m_iView==WSTABVIEW)
			{
				if(m_iStabilityView==STABTIMEVIEW)
				{
					m_iStabTimeView =1;
					m_pCurGraph = m_TimeGraph+1;
					m_pCurTimeGraph = m_pCurGraph;
					SetWingLegendPos();
				}
			}
			UpdateView();
			break;
		}
		case Qt::Key_3:
		{
			if(m_iView==WPOLARVIEW)
			{
				m_iWPlrView  = 1;
				m_pCurGraph = m_WPlrGraph+2;
				m_pCurWPlrGraph = m_pCurGraph;
				SetWPlrLegendPos();
			}
			else if(m_iView==WOPPVIEW)
			{
				m_iWingView =1;
				m_pCurGraph = m_WingGraph+2;
				m_pCurWingGraph = m_pCurGraph;
				SetWingLegendPos();
			}
			else if(m_iView==WSTABVIEW)
			{
				if(m_iStabilityView==STABTIMEVIEW)
				{
					m_iStabTimeView =1;
					m_pCurGraph = m_TimeGraph+2;
					m_pCurTimeGraph = m_pCurGraph;
					SetWingLegendPos();
				}
			}
			UpdateView();
			break;
		}
		case Qt::Key_4:
		{
			if(m_iView==WPOLARVIEW)
			{
				m_iWPlrView  = 1;
				m_pCurGraph = m_WPlrGraph+3;
				m_pCurWPlrGraph = m_pCurGraph;
				SetWPlrLegendPos();
			}
			else if(m_iView==WOPPVIEW)
			{
				m_iWingView =1;
				m_pCurGraph = m_WingGraph+3;
				m_pCurWingGraph = m_pCurGraph;
				SetWingLegendPos();
			}
			else if(m_iView==WSTABVIEW)
			{
				if(m_iStabilityView==STABTIMEVIEW)
				{
					m_iStabTimeView =1;
					m_pCurGraph = m_TimeGraph+3;
					m_pCurTimeGraph = m_pCurGraph;
					SetWingLegendPos();
				}
			}
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
				m_iWPlrView = 2;
				SetWPlrLegendPos();
			}
			else if(m_iView==WOPPVIEW)
			{
				m_iWingView =2;
				SetWingLegendPos();
			}
			else if(m_iView==WSTABVIEW)
			{
				m_iStabTimeView =2;
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
				m_iWPlrView = 4;
				SetWPlrLegendPos();
			}
			else if(m_iView==WOPPVIEW)
			{
				m_iWingView =4;
				SetWingLegendPos();
			}
			else if(m_iView==WSTABVIEW)
			{
				if(m_iStabilityView==STABTIMEVIEW)
				{
					m_iStabTimeView = 4;
					SetWingLegendPos();
				}
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
			if((m_iView==WPOLARVIEW ||(m_iView==WSTABVIEW&&m_iStabilityView==STABPOLARVIEW)) && event->modifiers().testFlag(Qt::ControlModifier))
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
			else if(m_iView==WOPPVIEW)   OnResetWingScale();
			else if(m_iView==WPOLARVIEW) OnResetWPlrLegend();
			else if(m_iView==WSTABVIEW)  OnResetWPlrLegend();
			else if(m_iView==W3DVIEW)    On3DReset();

			break;
		}
		case Qt::Key_F2:
		{
			OnRenameCurUFO();
			break;
		}
		case Qt::Key_F3:
		{
			if (event->modifiers().testFlag(Qt::ShiftModifier))        OnEditUFO();
			else if (event->modifiers().testFlag(Qt::ControlModifier)) OnNewPlane();
			else                                                       OnNewWing();
			break;
		}
		case Qt::Key_F4:
		{
			On3DView();
			break;
		}
		case Qt::Key_F5:
		{
			OnWOpps();
			break;
		}
		case Qt::Key_F6:
		{
			if (event->modifiers().testFlag(Qt::ShiftModifier))        OnDefineStabPolar();
			else                                                       OnDefineWPolar();
			break;
		}
		case Qt::Key_F8:
		{
			if (event->modifiers().testFlag(Qt::ShiftModifier))        OnRootLocusView();
			else if (event->modifiers().testFlag(Qt::ControlModifier)) OnTimeView();
			else                                                       OnWPolars();
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


void QMiarex::LLTAnalyze(double V0, double VMax, double VDelta, bool bSequence, bool bInitCalc)
{
	//
	// Lanuches the LLT analysis in the LLTAnalysisDlg class
	// 

	if(!m_pCurWing || !m_pCurWPolar) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	m_pLLTDlg->m_bInitCalc = bInitCalc;

	m_pLLTDlg->m_pWing      = m_pCurWing;
	m_pLLTDlg->m_pWPolar    = m_pCurWPolar;
	m_pLLTDlg->m_bSequence  = bSequence;

	m_pLLTDlg->m_AlphaMin   = V0;
	m_pLLTDlg->m_AlphaMax   = VMax;
	m_pLLTDlg->m_AlphaDelta = VDelta;

	m_pLLTDlg->m_IterLim = m_Iter;

	m_pLLTDlg->move(pMainFrame->m_DlgPos);
	m_pLLTDlg->InitDialog();
	m_pLLTDlg->show();
	m_pLLTDlg->StartAnalysis();

	if(!m_bLogFile || !(m_pLLTDlg->m_bError || m_pLLTDlg->m_bWarning)) m_pLLTDlg->hide();

	pMainFrame->m_DlgPos= m_pLLTDlg->pos();
	pMainFrame->UpdateWOpps();
	SetWOpp(false, V0);

	if(m_iView==WPOLARVIEW)     CreateWPolarCurves();

	UpdateView();
}


bool QMiarex::LoadSettings(QSettings *pSettings)
{
	//
	// Loads the user's saved settings for 3D analysis
	//
	int k;
	int r,g,b;
	QString strong;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;

	pSettings->beginGroup("Miarex");
	{
		m_bXTop         = pSettings->value("bXTop", false).toBool();
		m_bXBot         = pSettings->value("bXBot", false).toBool();
		m_bXCP          = pSettings->value("bXCP", false).toBool();
		m_bPanelForce   = pSettings->value("bPanelForce", false).toBool();
		m_bXCmRef       = pSettings->value("bXCmRef").toBool();
		m_bICd          = pSettings->value("bICd", true).toBool();
		m_bVCd          = pSettings->value("bVCd", true).toBool();
		m_bWakePanels   = pSettings->value("bWakePanels").toBool();
		m_bSurfaces     = pSettings->value("bSurfaces").toBool();
		m_bOutline      = pSettings->value("bOutline").toBool();
		m_bVLMPanels    = pSettings->value("bVLMPanels").toBool();
		m_bAxes         = pSettings->value("bAxes").toBool();
		m_b3DCp         = pSettings->value("b3DCp").toBool();
		m_bDownwash     = pSettings->value("bDownwash").toBool();
		m_bMoments      = pSettings->value("bMoments").toBool();
		m_bAutoCpScale  = pSettings->value("bAutoCpScale").toBool();
		m_bShowCpScale  = pSettings->value("bShowCpScale").toBool();
		m_bCurWOppOnly  = pSettings->value("CurWOppOnly").toBool();
		m_bShowElliptic = pSettings->value("bShowElliptic").toBool();
		m_bLogFile      = pSettings->value("LogFile").toBool();
		m_bDirichlet    = pSettings->value("Dirichlet").toBool();
		m_bVLM1         = pSettings->value("bVLM1").toBool();
		m_bTrefftz      = pSettings->value("Trefftz").toBool();
		m_bKeepOutOpps  = pSettings->value("KeepOutOpps").toBool();
		m_bResetWake    = pSettings->value("ResetWake").toBool();
		m_bShowWingCurve[0]    = pSettings->value("ShowWing").toBool();
		m_bShowWingCurve[1]    = pSettings->value("ShowWing2").toBool();
		m_bShowWingCurve[2]    = pSettings->value("ShowStab").toBool();
		m_bShowWingCurve[3]    = pSettings->value("ShowFin").toBool();
		m_bShowWingCurve[0] = true;

		m_bStoreWOpp    = pSettings->value("StoreWOpp").toBool();
		m_bSequence     = pSettings->value("Sequence").toBool();
		m_bHighlightOpp = pSettings->value("HighlightOpp").toBool();
//		m_bHighlightOpp = false;

		m_AlphaMin      = pSettings->value("AlphaMin").toDouble();
		m_AlphaMax      = pSettings->value("AlphaMax").toDouble();
		m_AlphaDelta    = pSettings->value("AlphaDelta").toDouble();
		m_QInfMin       = pSettings->value("QInfMin").toDouble();
		m_QInfMax       = pSettings->value("QInfMax").toDouble();
		m_QInfDelta     = pSettings->value("QInfDelta").toDouble();
		m_ControlMin    = pSettings->value("ControlMin").toDouble();
		m_ControlMax    = pSettings->value("ControlMax").toDouble();
		m_ControlDelta  = pSettings->value("ControlDelta").toDouble();
		m_WngAnalysis.m_bPlaneInertia =	pSettings->value("bPlaneInertia", false).toBool();


		m_3DAxisStyle   = pSettings->value("3DAxisStyle").toInt();
		m_3DAxisWidth   = pSettings->value("3DAXisWidth").toInt();
		r = pSettings->value("3DAxisColorRed").toInt();
		g = pSettings->value("3DAxisColorGreen").toInt();
		b = pSettings->value("3DAxisColorBlue").toInt();
		m_3DAxisColor   = QColor(r,g,b);

		m_VLMStyle = pSettings->value("VLMStyle").toInt();
		m_VLMWidth = pSettings->value("VLMWidth").toInt();
		r = pSettings->value("VLMColorRed").toInt();
		g = pSettings->value("VLMColorGreen").toInt();
		b = pSettings->value("VLMColorBlue").toInt();
		m_VLMColor = QColor(r,g,b);

		m_OutlineStyle = pSettings->value("OutlineStyle").toInt();
		m_OutlineWidth = pSettings->value("OutlineWidth").toInt();
		r = pSettings->value("OutlineColorRed").toInt();
		g = pSettings->value("OutlineColorGreen").toInt();
		b = pSettings->value("OutlineColorBlue").toInt();
		m_OutlineColor = QColor(r,g,b);

		m_XCPStyle = pSettings->value("XCPStyle").toInt();
		m_XCPWidth = pSettings->value("XCPWidth").toInt();
		r = pSettings->value("XCPColorRed").toInt();
		g = pSettings->value("XCPColorGreen").toInt();
		b = pSettings->value("XCPColorBlue").toInt();
		m_XCPColor = QColor(r,g,b);

		m_MomentStyle = pSettings->value("MomentStyle").toInt();
		m_MomentWidth = pSettings->value("MomentWidth").toInt();
		r = pSettings->value("MomentColorRed").toInt();
		g = pSettings->value("MomentColorGreen").toInt();
		b = pSettings->value("MomentColorBlue").toInt();
		m_MomentColor = QColor(r,g,b);

		m_IDragStyle = pSettings->value("IDragStyle").toInt();
		m_IDragWidth = pSettings->value("IDragWidth").toInt();
		r = pSettings->value("IDragColorRed").toInt();
		g = pSettings->value("IDragColorGreen").toInt();
		b = pSettings->value("IDragColorBlue").toInt();
		m_IDragColor = QColor(r,g,b);

		m_VDragStyle = pSettings->value("VDragStyle").toInt();
		m_VDragWidth = pSettings->value("VDragWidth").toInt();
		r = pSettings->value("VDragColorRed").toInt();
		g = pSettings->value("VDragColorGreen").toInt();
		b = pSettings->value("VDragColorBlue").toInt();
		m_VDragColor = QColor(r,g,b);

		m_XTopStyle = pSettings->value("XTopStyle").toInt();
		m_XTopWidth = pSettings->value("XTopWidth").toInt();
		r = pSettings->value("XTopColorRed").toInt();
		g = pSettings->value("XTopColorGreen").toInt();
		b = pSettings->value("XTopColorBlue").toInt();
		m_XTopColor = QColor(r,g,b);

		m_XBotStyle = pSettings->value("XBotStyle").toInt();
		m_XBotWidth = pSettings->value("XBotWidth").toInt();
		r = pSettings->value("XBotColorRed").toInt();
		g = pSettings->value("XBotColorGreen").toInt();
		b = pSettings->value("XBotColorBlue").toInt();
		m_XBotColor = QColor(r,g,b);

		m_DownwashStyle = pSettings->value("DownwashStyle").toInt();
		m_DownwashWidth = pSettings->value("DownwashWidth").toInt();
		r = pSettings->value("DownwashColorRed").toInt();
		g = pSettings->value("DownwashColorGreen").toInt();
		b = pSettings->value("DownwashColorBlue").toInt();
		m_DownwashColor = QColor(r,g,b);

		m_WakeStyle = pSettings->value("WakeStyle").toInt();
		m_WakeWidth = pSettings->value("WakeWidth").toInt();
		r = pSettings->value("WakeColorRed").toInt();
		g = pSettings->value("WakeColorGreen").toInt();
		b = pSettings->value("WakeColorBlue").toInt();
		m_WakeColor = QColor(r,g,b);

		m_CpStyle = pSettings->value("CpStyle").toInt();
		m_CpWidth = pSettings->value("CpWidth").toInt();
		r = pSettings->value("CpColorRed").toInt();
		g = pSettings->value("CpColorGreen").toInt();
		b = pSettings->value("CpColorBlue").toInt();
		m_CpColor = QColor(r,g,b);

		m_StreamLinesStyle = pSettings->value("StreamLinesStyle", 0).toInt();
		m_StreamLinesWidth = pSettings->value("StreamLinesWidth", 1).toInt();
		m_StreamLinesColor = pSettings->value("StreamLinesColor", QColor(150, 140, 255)).value<QColor>();

		m_MassColor = pSettings->value("MassColor", QColor(100, 100, 200)).value<QColor>();

		LLTAnalysis::s_CvPrec       = pSettings->value("CvPrec").toDouble();
		LLTAnalysis::s_RelaxMax     = pSettings->value("RelaxMax").toDouble();
		LLTAnalysis::s_NLLTStations = pSettings->value("NLLTStations").toInt();
		m_iView     = pSettings->value("iView").toInt();
		m_iWingView = pSettings->value("iWingView").toInt();
		m_iWPlrView = pSettings->value("iWPlrView").toInt();
		m_iStabilityView = pSettings->value("iWStabView").toInt();

		k = pSettings->value("CurrentWingGraph").toInt();
		m_pCurWingGraph = m_WingGraph+k-1;

		k = pSettings->value("CurWPlrGraph").toInt();
		m_pCurWPlrGraph = m_WPlrGraph+k-1;

		k = pSettings->value("CurRLStabGraph").toInt();
		if     (k==1) m_pCurRLStabGraph = &m_LongRLGraph;
		else if(k==2) m_pCurRLStabGraph = &m_LatRLGraph;

		if(m_iView==WOPPVIEW)        m_pCurGraph=m_pCurWingGraph;
		else if(m_iView==WPOLARVIEW) m_pCurGraph=m_pCurWPlrGraph;
		else if(m_iView==WSTABVIEW)  m_pCurGraph=m_pCurRLStabGraph;

		m_Iter         = pSettings->value("Iter").toInt();
		m_NStation     = pSettings->value("NStation").toInt();
		m_GL3dBody.m_NHoopPoints  = pSettings->value("NHoopPoints").toInt();
		m_GL3dBody.m_NXPoints     = pSettings->value("NXPoints").toInt();
		m_InducedDragPoint  = pSettings->value("InducedDragPoint").toInt();

		m_LiftScale     = pSettings->value("LiftScale").toDouble();
		m_DragScale     = pSettings->value("DragScale").toDouble();
		m_VelocityScale = pSettings->value("VelocityScale").toDouble();

		m_WakeInterNodes    = pSettings->value("WakeInterNodes").toInt();
		m_MaxWakeIter       = pSettings->value("MaxWakeIter").toInt();
		CPanel::m_CtrlPos   = pSettings->value("CtrlPos").toDouble();
		CPanel::m_VortexPos = pSettings->value("VortexPos").toDouble();
		m_CoreSize          = pSettings->value("CoreSize").toDouble();
		m_MinPanelSize      = pSettings->value("MinPanelSize").toDouble();

		m_RampTime      = pSettings->value("RampTime", 0.1).toDouble();
		m_RampAmplitude = pSettings->value("RampAmplitude", 1.0).toDouble();

		m_TotalTime         = pSettings->value("TotalTime",10.0).toDouble();
		m_Deltat            = pSettings->value("Delta_t",0.01).toDouble();

		m_TimeInput[0]      = pSettings->value("TimeIn0",0.0).toDouble();
		m_TimeInput[1]      = pSettings->value("TimeIn1",0.0).toDouble();
		m_TimeInput[2]      = pSettings->value("TimeIn2",0.0).toDouble();
		m_TimeInput[3]      = pSettings->value("TimeIn3",0.0).toDouble();
//		m_bForcedResponse   = pSettings->value("ForcedResponse").toBool();
		m_bLongitudinal     = pSettings->value("DynamicsMode").toBool();
		m_iStabTimeView     = pSettings->value("StabTimeView",4).toInt();
		m_StabilityResponseType = pSettings->value("StabCurveType",0).toInt();
		k = pSettings->value("TimeGraph").toInt();
		m_pCurTimeGraph = m_TimeGraph+k-1;

		m_StabPolarDlg.m_bAVLControls = pSettings->value("AVLControls", true).toBool();

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
	}

	pSettings->endGroup();

	m_GL3dBody.LoadSettings(pSettings);
	m_GLLightDlg.LoadSettings(pSettings);

	m_CpGraph.LoadSettings(pSettings);
	for(int ig=0; ig<4; ig++)
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


void QMiarex::mouseDoubleClickEvent (QMouseEvent * event)
{
	//
	// capture and dispatch user mouse input
	//
	if(m_iView==W3DVIEW || (m_iView==WSTABVIEW && m_iStabilityView==STAB3DVIEW))
	{
		QPoint point = event->pos();

		GLWidget *pGLWidget = (GLWidget*)s_pGLWidget;

		CVector Real;
		ClientToGL(point, Real);
		if(m_r3DCltRect.contains(point)) pGLWidget->setFocus();

		Set3DRotationCenter(point);
		m_bPickCenter = false;
		m_pctrlPickCenter->setChecked(false);
		UpdateView();
	}
	else if(m_pCurGraph) 
	{
		CCurve *pCloseCurve = NULL;
		int n=-1;

		pCloseCurve = m_pCurGraph->GetCurvePoint(event->pos().x(),event->pos().y(), n);
		if(pCloseCurve)
		{
			QString CurveName;
			pCloseCurve->GetTitle(CurveName);
			if(m_iView==WPOLARVIEW)
			{
			}
			else if(m_iView==WSTABVIEW && m_iStabilityView==STABPOLARVIEW)
			{
				if(m_pCurWPolar)
				{
					if(m_pCurWPolar->m_PlrName==CurveName.left(CurveName.length()-7))
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


void QMiarex::mouseMoveEvent(QMouseEvent *event)
{
	//
	// capture and dispatch user mouse input
	//
	if(!hasFocus()) setFocus();
	static CVector Real;
	static bool bCtrl;
	static QPoint Delta, point;
	static double xu, yu, x1, y1, xmin, xmax, ymin, ymax;

	Delta.setX(event->pos().x() - m_LastPoint.x());
	Delta.setY(event->pos().y() - m_LastPoint.y());
	point = event->pos();
	m_pCurGraph = GetGraph(point);

	bCtrl = false;
	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;
	if(m_iView==W3DVIEW || (m_iView==WSTABVIEW && m_iStabilityView==STAB3DVIEW))
	{
		ClientToGL(point, Real);

		if (event->buttons() & Qt::LeftButton)
		{
			if(bCtrl)//rotate
			{
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
		//scaling
		{
			// we zoom the graph or the wing

			if(m_pCurWing)
			{	//zoom 3D wing
//				if(pt.y()-m_LastPoint.y()>0) m_glScaled *= (GLfloat)1.02;
//				else                         m_glScaled /= (GLfloat)1.02;

				m_ArcBall.Move(point.x(), m_r3DCltRect.height()-point.y());
				UpdateView();
			}
		}
	}
	else
	{
		if ((event->buttons() & Qt::LeftButton) && m_bTrans &&
			(m_iView==WOPPVIEW || m_iView==WPOLARVIEW || m_iView==WCPVIEW || m_iView==WSTABVIEW))
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
			else if (m_pCurWing  && ((m_iView==WOPPVIEW && m_iWingView ==1)))
			{
				// we translate the UFO

				m_ptOffset.rx() += Delta.x();
				m_ptOffset.ry() += Delta.y();
				UpdateView();

			}
			else if (m_pCurWing  && m_iView==WOPPVIEW && m_iWingView !=1)
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
				if(m_iWPlrView==1 || m_iWPlrView==4)
				{
					m_WPlrLegendOffset.rx() += Delta.x();
					m_WPlrLegendOffset.ry() += Delta.y();
					UpdateView();
				}
				else if(m_iWPlrView==2)
				{
					m_WPlrLegendOffset.rx() += Delta.x();
					m_WPlrLegendOffset.ry() += Delta.y();
					UpdateView();
				}
			}
			else if (m_iView==WSTABVIEW)
			{
				// we translate the legend
				if(m_iStabilityView==STABTIMEVIEW)
				{
					m_WingLegendOffset.rx() += Delta.x();
					m_WingLegendOffset.ry() += Delta.y();
					UpdateView();
					
				}
				else if(m_iStabilityView==STABPOLARVIEW)
				{
					m_WPlrLegendOffset.rx() += Delta.x();
					m_WPlrLegendOffset.ry() += Delta.y();
					UpdateView();					
				}				
			}
		}

		else if ((event->buttons() & Qt::MidButton) && !bCtrl)
		//scaling
		{
			// we zoom the graph or the wing
			if(m_iView ==WOPPVIEW ||m_iView==WPOLARVIEW || m_iView==WCPVIEW || m_iView==WSTABVIEW)
			{
				if(m_pCurGraph && m_pCurGraph->IsInDrawRect(point))
				{
					//zoom graph
					m_pCurGraph->SetAuto(false);
					if(point.y()-m_LastPoint.y()<0) m_pCurGraph->Scale(1.02);
					else                            m_pCurGraph->Scale(1.0/1.02);

					UpdateView();
				}
				else if(m_pCurWing && m_iView==WOPPVIEW)
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
}



void QMiarex::mousePressEvent(QMouseEvent *event)
{
	//
	// capture and dispatch user mouse input
	//
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
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

		if(m_iView==W3DVIEW || (m_iView==WSTABVIEW && m_iStabilityView==STAB3DVIEW))
		{
			GLWidget *pGLWidget = (GLWidget*)s_pGLWidget;
		//	point is in client coordinates

			CVector Real;
			bool bCtrl = false;
			if(event->modifiers() & Qt::ControlModifier)
			{
				m_bArcball = true;
				bCtrl =true;
			}

			ClientToGL(point, Real);
			if(m_r3DCltRect.contains(point)) pGLWidget->setFocus();

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
					pGLWidget->setCursor(Qt::ClosedHandCursor);
				}
			}

			UpdateView();

			m_bPickCenter = false;
		}
		else
		{
			TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;
			m_pCurGraph = GetGraph(point);
			if(m_r2DCltRect.contains(point)) p2DWidget->setFocus();
			
			if(m_pCurGraph && m_iView==WSTABVIEW && (m_iStabilityView==STABPOLARVIEW)
			   && m_pCurWPolar && m_pCurWPolar->m_Type==STABILITYPOLAR)
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
						m_pCurRLStabGraph->GetCurve(k)->GetTitle(str);
						if(str==m_pCurWPolar->m_PlrName+suffix)
						{
							m_pCurRLStabGraph->GetCurve(k)->SetSelected(jsel);
							break;
						}
					}
				}
			}

			if (m_pCurWing || (m_pCurGraph && m_pCurGraph->IsInDrawRect(point)))
			{
				m_LastPoint.rx() = point.x();
				m_LastPoint.ry() = point.y();

				m_bTrans = true;
				m_bTransGraph = true;
				if(m_pCurGraph && m_pCurGraph->IsInDrawRect(point)) m_bTransGraph = true;
				else                                                m_bTransGraph = false;
				if(m_bTrans || m_bTransGraph) p2DWidget->setCursor(Qt::ClosedHandCursor);

			}
		}
		m_PointDown = point;
		m_LastPoint = point;
	}
}


void QMiarex::mouseReleaseEvent(QMouseEvent *event)
{	
	//
	// capture and dispatch user mouse input
	//
	if(m_iView==W3DVIEW || (m_iView==WSTABVIEW && m_iStabilityView==STAB3DVIEW))
	{
		QGLWidget* pGLWidget = (QGLWidget*)s_pGLWidget;
		pGLWidget->setCursor(Qt::CrossCursor);

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
		TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;
		p2DWidget->setCursor(Qt::CrossCursor);
	}
	m_bTrans = false;
}



void QMiarex::NormalVector(GLdouble p1[3], GLdouble p2[3],  GLdouble p3[3], GLdouble n[3])
{
	//
	// calculate two vectors, using the middle point as the common origin
	//

	GLdouble v1[3], v2[3], d;
	v1[0] = p3[0] - p1[0];
	v1[1] = p3[1] - p1[1];
	v1[2] = p3[2] - p1[2];
	v2[0] = p3[0] - p2[0];
	v2[1] = p3[1] - p2[1];
	v2[2] = p3[2] - p2[2];

	// calculate the cross product of the two vectors
	n[0] = v1[1] * v2[2] - v2[1] * v1[2];
	n[1] = v1[2] * v2[0] - v2[2] * v1[0];
	n[2] = v1[0] * v2[1] - v2[0] * v1[1];

	// normalize the vector
	d = ( n[0] * n[0] + n[1] * n[1] + n[2] * n[2] );
	// try to catch very small vectors
	if (d < (GLdouble)0.00000001)
	{
		d = (GLdouble)100000000.0;
	}
	else
	{
		d = (GLdouble)1.0 / sqrt(d);
	}

	n[0] *= d;
	n[1] *= d;
	n[2] *= d;
}


void QMiarex::On3DView()
{
	//

	// The user has requested a switch to the OpenGL 3D view
	//
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	m_bArcball = false;
	m_iStabilityView = 3;
	if(m_iView==W3DVIEW)
	{
		SetControls();
		UpdateView();
		if(m_pCurWPolar && m_pCurWPolar->m_Type==STABILITYPOLAR)
		{
			pMainFrame->m_pctrlStabViewWidget->show();
		}
		return;
	}

	m_bIs3DScaleSet = false;

	if(m_pCurPlane) m_pCurBody = m_pCurPlane->Body();
	else            m_pCurBody = NULL;

	m_iView =W3DVIEW;

	if(m_pCurPlane && m_pCurPlane->Body())
	{
		m_pCurBody = m_pCurPlane->Body();
		m_bResetglBody = true;
	}
	else m_pCurBody = NULL;
	SetControls();

	pMainFrame->SetCentralWidget();

	if(m_pCurWPolar && m_pCurWPolar->m_Type==STABILITYPOLAR)
	{
		pMainFrame->m_pctrlStabViewWidget->show();
	}

	UpdateView();
	return;
}



void QMiarex::On3DCp()
{
	//
	// The user has toggled the switch for the display of Cp coefficients
	//
	m_b3DCp = m_pctrlCp->isChecked();

	if(m_b3DCp)
	{
		m_bSurfaces = false;
		m_pctrlSurfaces->setChecked(false);
		m_bPanelForce = false;
		m_pctrlPanelForce->setChecked(false);
	}
	UpdateView();
}


void QMiarex::On3DIso()
{
	//
	// The user has requested a perspective view in the 3D display
	//
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



void QMiarex::On3DTop()
{
	//
	// The user has requested a top view in the 3D display
	//
	m_ArcBall.SetQuat(sqrt(2.0)/2.0, 0.0, 0.0, -sqrt(2.0)/2.0);
	Set3DRotationCenter();
	UpdateView();
}


void QMiarex::On3DLeft()
{
	//
	// The user has requested a left view in the 3D display
	//
	m_ArcBall.SetQuat(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0, 0.0);// rotate by 90 deg around x
	Set3DRotationCenter();
	UpdateView();
}


void QMiarex::On3DFront()
{
	//
	// The user has requested a front view in the 3D display
	//
	Quaternion Qt1(sqrt(2.0)/2.0, 0.0,           -sqrt(2.0)/2.0, 0.0);// rotate by 90 deg around y
	Quaternion Qt2(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0,           0.0);// rotate by 90 deg around x

	m_ArcBall.SetQuat(Qt1 * Qt2);
	Set3DRotationCenter();
	UpdateView();
}


void QMiarex::On3DReset()
{
	//
	// The user has requested a reset of the scales in the 3D view
	//
	m_glViewportTrans.Set(0.0, 0.0, 0.0);
	m_bPickCenter   = false;
	m_bIs3DScaleSet = false;
	Set3DScale();
	Set3DRotationCenter();
	UpdateView();
}


void QMiarex::On3DPrefs()
{
	//
	// The user has requested a modification of the styles for the 3D view
	//
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	m_W3dPrefsDlg.move(pMainFrame->m_DlgPos);
	m_W3dPrefsDlg.m_pMiarex = this;
	m_W3dPrefsDlg.m_bWakePanels    = m_bWakePanels;
	m_W3dPrefsDlg.m_3DAxisColor    = m_3DAxisColor;
	m_W3dPrefsDlg.m_3DAxisStyle    = m_3DAxisStyle;
	m_W3dPrefsDlg.m_3DAxisWidth    = m_3DAxisWidth;
	m_W3dPrefsDlg.m_VLMColor       = m_VLMColor;
	m_W3dPrefsDlg.m_VLMStyle       = m_VLMStyle;
	m_W3dPrefsDlg.m_VLMWidth       = m_VLMWidth;
	m_W3dPrefsDlg.m_OutlineColor   = m_OutlineColor;
	m_W3dPrefsDlg.m_OutlineStyle   = m_OutlineStyle;
	m_W3dPrefsDlg.m_OutlineWidth   = m_OutlineWidth;
	m_W3dPrefsDlg.m_TopColor       = m_XTopColor;
	m_W3dPrefsDlg.m_TopStyle       = m_XTopStyle;
	m_W3dPrefsDlg.m_TopWidth       = m_XTopWidth;
	m_W3dPrefsDlg.m_BotColor       = m_XBotColor;
	m_W3dPrefsDlg.m_BotStyle       = m_XBotStyle;
	m_W3dPrefsDlg.m_BotWidth       = m_XBotWidth;
	m_W3dPrefsDlg.m_XCPColor       = m_XCPColor;
	m_W3dPrefsDlg.m_XCPStyle       = m_XCPStyle;
	m_W3dPrefsDlg.m_XCPWidth       = m_XCPWidth;
	m_W3dPrefsDlg.m_MomentColor    = m_MomentColor;
	m_W3dPrefsDlg.m_MomentStyle    = m_MomentStyle;
	m_W3dPrefsDlg.m_MomentWidth    = m_MomentWidth;
	m_W3dPrefsDlg.m_IDragColor     = m_IDragColor;
	m_W3dPrefsDlg.m_IDragStyle     = m_IDragStyle;
	m_W3dPrefsDlg.m_IDragWidth     = m_IDragWidth;
	m_W3dPrefsDlg.m_VDragColor     = m_VDragColor;
	m_W3dPrefsDlg.m_VDragStyle     = m_VDragStyle;
	m_W3dPrefsDlg.m_VDragWidth     = m_VDragWidth;
	m_W3dPrefsDlg.m_DownwashColor  = m_DownwashColor;
	m_W3dPrefsDlg.m_DownwashStyle  = m_DownwashStyle;
	m_W3dPrefsDlg.m_DownwashWidth  = m_DownwashWidth;
	m_W3dPrefsDlg.m_StreamLinesColor      = m_StreamLinesColor;
	m_W3dPrefsDlg.m_StreamLinesStyle      = m_StreamLinesStyle;
	m_W3dPrefsDlg.m_StreamLinesWidth      = m_StreamLinesWidth;
	m_W3dPrefsDlg.m_WakeColor      = m_WakeColor;
	m_W3dPrefsDlg.m_WakeStyle      = m_WakeStyle;
	m_W3dPrefsDlg.m_WakeWidth      = m_WakeWidth;
	m_W3dPrefsDlg.m_MassColor      = m_MassColor;

	m_W3dPrefsDlg.InitDialog();

	if(m_W3dPrefsDlg.exec() == QDialog::Accepted)
	{
		m_bWakePanels    = m_W3dPrefsDlg.m_bWakePanels;
		m_3DAxisColor    = m_W3dPrefsDlg.m_3DAxisColor;
		m_3DAxisStyle    = m_W3dPrefsDlg.m_3DAxisStyle;
		m_3DAxisWidth    = m_W3dPrefsDlg.m_3DAxisWidth;
		m_VLMColor     = m_W3dPrefsDlg.m_VLMColor;
		m_VLMStyle     = m_W3dPrefsDlg.m_VLMStyle;
		m_VLMWidth     = m_W3dPrefsDlg.m_VLMWidth;
		m_OutlineColor = m_W3dPrefsDlg.m_OutlineColor;
		m_OutlineStyle = m_W3dPrefsDlg.m_OutlineStyle;
		m_OutlineWidth = m_W3dPrefsDlg.m_OutlineWidth;
		m_XTopColor    = m_W3dPrefsDlg.m_TopColor;
		m_XTopStyle    = m_W3dPrefsDlg.m_TopStyle;
		m_XTopWidth    = m_W3dPrefsDlg.m_TopWidth;
		m_XBotColor    = m_W3dPrefsDlg.m_BotColor;
		m_XBotStyle    = m_W3dPrefsDlg.m_BotStyle;
		m_XBotWidth    = m_W3dPrefsDlg.m_BotWidth;
		m_XCPColor     = m_W3dPrefsDlg.m_XCPColor;
		m_XCPStyle     = m_W3dPrefsDlg.m_XCPStyle;
		m_XCPWidth     = m_W3dPrefsDlg.m_XCPWidth;
		m_MomentColor  = m_W3dPrefsDlg.m_MomentColor;
		m_MomentStyle  = m_W3dPrefsDlg.m_MomentStyle;
		m_MomentWidth  = m_W3dPrefsDlg.m_MomentWidth;
		m_IDragColor     = m_W3dPrefsDlg.m_IDragColor;
		m_IDragStyle     = m_W3dPrefsDlg.m_IDragStyle;
		m_IDragWidth     = m_W3dPrefsDlg.m_IDragWidth;
		m_VDragColor     = m_W3dPrefsDlg.m_VDragColor;
		m_VDragStyle     = m_W3dPrefsDlg.m_VDragStyle;
		m_VDragWidth     = m_W3dPrefsDlg.m_VDragWidth;
		m_DownwashColor  = m_W3dPrefsDlg.m_DownwashColor;
		m_DownwashStyle  = m_W3dPrefsDlg.m_DownwashStyle;
		m_DownwashWidth  = m_W3dPrefsDlg.m_DownwashWidth;
		m_WakeColor      = m_W3dPrefsDlg.m_WakeColor;
		m_WakeStyle      = m_W3dPrefsDlg.m_WakeStyle;
		m_WakeWidth      = m_W3dPrefsDlg.m_WakeWidth;
		m_StreamLinesColor      = m_W3dPrefsDlg.m_StreamLinesColor;
		m_StreamLinesStyle      = m_W3dPrefsDlg.m_StreamLinesStyle;
		m_StreamLinesWidth      = m_W3dPrefsDlg.m_StreamLinesWidth;
		m_MassColor      = m_W3dPrefsDlg.m_MassColor;

		m_bResetglWake = true;
		m_bResetglBody = true;
		m_bResetglGeom = true;
		m_bResetglMesh = true;
		m_bResetglOpp  = true;
		m_bResetglStream = true;

		UpdateView();
	}
	pMainFrame->m_DlgPos = m_W3dPrefsDlg.pos();
}



void QMiarex::On3DPickCenter()
{
	//
	// The user has toggled the switch for a pick of the rotation center in 3D view
	//
	m_bPickCenter = m_pctrlPickCenter->isChecked();
}



void QMiarex::OnAllWingGraphScales()
{
	//
	// The user has requested a reset of the scale of all WOpp graphs
	//
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(m_iView == WOPPVIEW)
	{
		double halfspan = m_pCurWing->m_PlanformSpan/2.0;

		for(int ig=0; ig<4; ig++)
		{
			m_WingGraph[ig].SetAuto(true);
			m_WingGraph[ig].ResetXLimits();
			m_WingGraph[ig].ResetYLimits();
			m_WingGraph[ig].SetAutoX(false);
			if(m_bHalfWing) m_WingGraph[ig].SetXMin(0.0);
			else m_WingGraph[ig].SetXMin(-halfspan*pMainFrame->m_mtoUnit);
			m_WingGraph[ig].SetXMax( halfspan*pMainFrame->m_mtoUnit);
		}
	}
	else if(m_iView==WSTABVIEW && m_iStabilityView==STABTIMEVIEW)
	{
		for(int ig=0; ig<4; ig++)
		{
			m_TimeGraph[ig].SetAuto(true);
			m_TimeGraph[ig].ResetXLimits();
			m_TimeGraph[ig].ResetYLimits();
		}
	}
	UpdateView();
}


void QMiarex::OnAllWPolarGraphScales()
{
	//
	// The user has requested a reset of the scale of all WPolar graphs
	//
	if(m_iView==WPOLARVIEW)
	{
		for(int ig=0; ig<4; ig++)
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
	else if(m_iView==WSTABVIEW)
	{
		for(int ig=0; ig<4; ig++)
		{
			m_TimeGraph[ig].SetAuto(true);
			m_TimeGraph[ig].ResetXLimits();
			m_TimeGraph[ig].ResetYLimits();
		}
	}
	UpdateView();
}



void QMiarex::OnAllWingGraphSettings()
{
	//
	// The user has requested an edition of the settings for all WOpp graphs
	//
	QGraph graph;
	graph.CopySettings(m_WingGraph);
	m_GraphDlg.m_pMemGraph = &graph;
	m_GraphDlg.m_pGraph    = m_WingGraph;
	m_GraphDlg.m_GraphArray[0] = m_WingGraph;
	m_GraphDlg.m_GraphArray[1] = m_WingGraph+1;
	m_GraphDlg.m_GraphArray[2] = m_WingGraph+2;
	m_GraphDlg.m_GraphArray[3] = m_WingGraph+3;
	m_GraphDlg.m_NGraph = 4;
	m_GraphDlg.SetParams();

	if(m_GraphDlg.exec() == QDialog::Accepted)
	{
	}
	else
	{
		for(int ig=0; ig<4; ig++) m_WingGraph[ig].CopySettings(&graph);
	}
	UpdateView();
}


void QMiarex::OnAllWPolarGraphSettings()
{
	//
	// The user has requested an edition of the settings for all WPolar graphs
	//
	QGraph graph;
	graph.CopySettings(m_WPlrGraph);
	m_GraphDlg.m_pMemGraph = &graph;
	m_GraphDlg.m_pGraph    = m_WPlrGraph;
	m_GraphDlg.m_GraphArray[0] = m_WPlrGraph;
	m_GraphDlg.m_GraphArray[1] = m_WPlrGraph+1;
	m_GraphDlg.m_GraphArray[2] = m_WPlrGraph+2;
	m_GraphDlg.m_GraphArray[3] = m_WPlrGraph+3;
	m_GraphDlg.m_NGraph = 4;
	m_GraphDlg.SetParams();

	if(m_GraphDlg.exec() == QDialog::Accepted)
	{
	}
	else
	{
		for(int ig=0; ig<4; ig++)
			m_WPlrGraph[ig].CopySettings(&graph);
	}
	UpdateView();
}


void QMiarex::OnAxes()
{
	m_bAxes = m_pctrlAxes->isChecked();
	UpdateView();
}


void QMiarex::OnAnalyze()
{
	//
	// The user has requested a launch of the analysis
	//

	int l;
	double V0, VMax, VDelta;
	bool bHigh = m_bHighlightOpp;
	m_bHighlightOpp = false;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(!m_pCurWing)
	{
		QMessageBox::warning(pMainFrame, tr("Warning"), tr("Please define a wing or a plane object before running a calculation"));
		return;
	}
	if(!m_pCurWPolar)
	{
		QMessageBox::warning(pMainFrame, tr("Warning"), tr("Please define an analysis/polar before running a calculation"));
		return;
	}
	
	//prevent an automatic and lengthy redraw of the streamlines after the calculation
	m_bStream = m_bSpeeds = false;
	m_pctrlStream->setChecked(false);
	m_pctrlSurfVel->setChecked(false);


	// may sure that the latest parameters are loaded
	OnReadAnalysisData();

	if(m_pCurWPolar->m_Type==FIXEDAOAPOLAR)
	{
		V0     = m_QInfMin;
		VMax   = m_QInfMax;
		VDelta = m_QInfDelta;
	}
	else if(m_pCurWPolar->m_Type==STABILITYPOLAR)
	{
		V0     = m_ControlMin;
		VMax   = m_ControlMax;
		VDelta = m_ControlDelta;
	}
	else if(m_pCurWPolar->m_Type <4)
	{
		V0     = m_AlphaMin;
		VMax   = m_AlphaMax;
		VDelta = m_AlphaDelta;
	}
	else
	{
		V0 = VMax = VDelta = 0.0;
	}

	// check if all the foils are in hte database...
	// ...could have been deleted or renamed or not imported with AVL wing or whatever
	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pWingList[iw])
		{
			for (l=0; l<=m_pWingList[iw]->m_NPanel; l++)
			{
				if (!pMainFrame->GetFoil(m_pWingList[iw]->m_RFoil[l]))
				{
					QString strong;
					strong = m_pWingList[iw]->m_WingName + ": "+tr("Could not find the wing's foil ")+ m_pWingList[iw]->m_RFoil[l] +tr("...\nAborting Calculation");
					QMessageBox::warning(pMainFrame, tr("Warning"), strong);
					return;
				}
				if (!pMainFrame->GetFoil(m_pWingList[iw]->m_LFoil[l]))
				{
					QString strong;
					strong = m_pWingList[iw]->m_WingName + ": "+tr("Could not find the wing's foil ")+ m_pWingList[iw]->m_LFoil[l] +tr("...\nAborting Calculation");
					QMessageBox::warning(pMainFrame, tr("Warning"), strong);
					return;
				}
			}
		}
	}

	if(m_pCurWPolar->m_AnalysisMethod==LLTMETHOD)
	{
		LLTAnalyze(V0, VMax, VDelta, m_bSequence, m_bInitLLTCalc);
	}
	else if(m_pCurWPolar->m_AnalysisMethod==PANELMETHOD)
	{
		if(m_pCurWPolar->m_Type==5 || m_pCurWPolar->m_Type==6)
		{
			QString strong;
			strong = tr("Control polars are not supported in XFLR5 v6.\nPlease use stability polars instead.");
			QMessageBox::warning(pMainFrame, tr("Warning"), strong+"\n");
			return;
		}
		PanelAnalyze(V0, VMax, VDelta, m_bSequence);
	}
	else if(m_pCurWPolar->m_Type==STABILITYPOLAR)
	{
		PanelAnalyze(V0, VMax, VDelta, m_bSequence);
	}

	//restore things as they were
	m_bHighlightOpp = bHigh;

	//refresh the view
	if(m_iView==WPOLARVIEW)     CreateWPolarCurves();
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();
	UpdateView();
	SetControls();
	pMainFrame->setFocus();
}



void QMiarex::OnAnimateWOpp()
{
	//
	// Launch the animation of the WOpp display
	// Will display all the available WOpps for this WPolar in sequence
	//
	if(!m_pCurWing || !m_pCurWPolar || m_iView==WPOLARVIEW)
	{
		m_bAnimateWOpp = false;
		return;
	}
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	CWOpp* pWOpp;
	CPOpp*pPOpp;
	int l;

	if(m_pctrlWOppAnimate->isChecked())
	{
		if(m_pCurPlane)
		{
			for (l=0; l< m_poaPOpp->size(); l++)
			{
				pPOpp = (CPOpp*)m_poaPOpp->at(l);

				if (pPOpp &&
					pPOpp->m_PlrName  == m_pCurWPolar->m_PlrName &&
					pPOpp->m_PlaneName == m_pCurPlane->PlaneName())
				{
						if(m_pCurPOpp->m_Alpha - pPOpp->m_Alpha<0.0001)
							m_posAnimateWOpp = l;
				}
			}
		}
		else if(m_pCurWing)
		{
			for (l=0; l< m_poaWOpp->size(); l++)
			{
				pWOpp = (CWOpp*)m_poaWOpp->at(l);

				if (pWOpp &&
					pWOpp->m_PlrName  == m_pCurWPolar->m_PlrName &&
					pWOpp->m_WingName == m_pCurWing->WingName())
				{
						if(m_pCurWOpp->m_Alpha - pWOpp->m_Alpha<0.0001)
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


void QMiarex::OnAnimateModeSingle(bool bStep)
{
	//
	// A signal has been received from the timer to update the 3D mode display
	// So calculate the state corresponding to the time m_ModeTime
	// and display it
	//

	static double t, sigma, s2, omega, o2, theta_sum, psi_sum, norm;
	double *vabs, *phi;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;

	if(m_iView!=WSTABVIEW && m_iView!=W3DVIEW)
	{
		m_pTimerMode->stop();
		return; //nothing to animate
	}
	if(!m_pCurWing || !m_pCurWPolar || !m_pCurWPolar->m_Type==STABILITYPOLAR || !m_pCurWOpp)
	{
		m_pTimerMode->stop();
		return; //nothing to animate
	}

	//read the data, since the user may have been playing with it
	norm = m_ModeNorm * pStabView->m_ModeAmplitude;
	vabs = pStabView->m_vabs;
	phi  = pStabView->m_phi;

	// calculate the new state
	sigma = m_pCurWOpp->m_EigenValue[pStabView->m_iCurrentMode].real();
	omega = m_pCurWOpp->m_EigenValue[pStabView->m_iCurrentMode].imag();
	s2 = sigma*sigma;
	o2 = omega*omega;
	t=m_ModeTime;

	if(t>=100) StopAnimate();


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
		m_ModeState[2] -= theta_sum *m_pCurWOpp->m_QInf;
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

		m_ModeState[1] += norm * m_pCurWOpp->m_QInf * psi_sum;

		// get directly phi from fourth eigenvector component (alternatively integrate p+r.tan(theta0));
		m_ModeState[3] = norm*vabs[3]*exp(sigma*t)*cos(omega*t+phi[3]);
//		m_ModeState[3] = norm*vabs[1]*exp(sigma*t)/(s2+o2) * (sigma*cos(omega*t+phi[1])+omega*sin(omega*t+phi[1]));

		// integrate once 'p+r.sin(theta0)' to get heading angle
		m_ModeState[5] = norm*vabs[2]*exp(sigma*t)/(s2+o2) * (sigma*cos(omega*t+phi[2])+omega*sin(omega*t+phi[2]));
	}
	
	//increase the time for the next update
	if(bStep) m_ModeTime += m_Modedt;
	
	UpdateView();
}



void QMiarex::OnAnimateWOppSingle()
{
	//
	// A signal has been received from the timer to update the WOPP display
	// So display the next WOpp in the sequence
	//

	static bool bIsValid, bSkipOne;
	static int size, pos;
	static CPOpp *pPOpp;
	static CWOpp *pWOpp;
	static QString str;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	//KickIdle
	if(m_iView!=W3DVIEW && m_iView !=WOPPVIEW) return; //nothing to animate
	if(!m_pCurWing || !m_pCurWPolar) return;

	if(m_pCurPlane)	size = m_poaPOpp->size();
	else            size = m_poaWOpp->size();
	if(size<=1) return;

	bIsValid = false;
	bSkipOne = false;

	while(!bIsValid)
	{
		pPOpp = NULL;
		pWOpp = NULL;
		//Find the current position to display

		if(m_pCurPlane)
		{
			pPOpp = (CPOpp*)m_poaPOpp->at(m_posAnimateWOpp);
			if(!pPOpp) return;
		}
		else
		{
			pWOpp = (CWOpp*)m_poaWOpp->at(m_posAnimateWOpp);
			if(!pWOpp) return;
		}
		if(m_pCurPlane)
			bIsValid =(pPOpp->m_PlrName==m_pCurWPolar->m_PlrName  &&  pPOpp->m_PlaneName==m_pCurPlane->PlaneName());
		else
			bIsValid =(pWOpp->m_PlrName==m_pCurWPolar->m_PlrName  &&  pWOpp->m_WingName==m_pCurWing->WingName());

		if (bIsValid && !bSkipOne)
		{
			if(m_pCurPlane)
			{
				m_pCurPOpp = pPOpp;
				m_pCurWOpp = &pPOpp->m_PlaneWOpp[0];
				for(int iw=0; iw<MAXWINGS;iw++)
				{
					if(m_pCurPOpp->m_bWing[iw]) m_pWOpp[iw] = &m_pCurPOpp->m_PlaneWOpp[iw];
					else                        m_pWOpp[iw] = NULL;
				}

			}
			else
			{
				m_pCurPOpp = NULL;
				m_pCurWOpp = pWOpp;
				m_pWOpp[0] = m_pCurWOpp;
				m_pWOpp[1] = m_pWOpp[2] = m_pWOpp[3] = NULL;
			}
			m_pCurPOpp = pPOpp;
			m_bCurWOppOnly = true;

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

			//select current WOpp in Combobox
			if(m_pCurWPolar->m_Type != 4) str = QString("%1").arg(m_pCurWOpp->m_Alpha,8,'f',2);
			else                          str = QString("%1").arg(m_pCurWOpp->m_QInf,8,'f',2);
			pos = pMainFrame->m_pctrlWOpp->findText(str);
			if(pos>=0) pMainFrame->m_pctrlWOpp->setCurrentIndex(pos);
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


void QMiarex::OnAnimateWOppSpeed(int val)
{
	//
	// The user has changed the animation speed for the WOpp display
	//
	if(m_pTimerWOpp->isActive())
	{
		m_pTimerWOpp->setInterval(800-val);
	}
}


void QMiarex::OnAdjustToWing()
{
	//
	// The user has requested an adjustment of the graph's scale to the wing's span
	//
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(!m_pCurWing) return;

	double halfspan = m_pCurWing->m_PlanformSpan/2.0;
	double xmin = -halfspan*pMainFrame->m_mtoUnit;
	if(m_bHalfWing) xmin = 0.0;
	for(int ig=0; ig<4; ig++)
	{
		m_WingGraph[ig].SetAutoX(false);
		m_WingGraph[ig].SetXMax( halfspan*pMainFrame->m_mtoUnit);
		m_WingGraph[ig].SetXMin(xmin);
	}
}


void QMiarex::OnAdvancedSettings()
{
	//
	// The user has requested an edition of the advanced settings
	//
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	m_WAdvancedDlg.move(pMainFrame->m_DlgPos);
	m_WAdvancedDlg.m_AlphaPrec       = LLTAnalysis::s_CvPrec;
	m_WAdvancedDlg.m_Relax           = LLTAnalysis::s_RelaxMax;
	m_WAdvancedDlg.m_NStation        = LLTAnalysis::s_NLLTStations;
	m_WAdvancedDlg.m_Iter            = m_Iter;
	m_WAdvancedDlg.m_MaxWakeIter     = m_MaxWakeIter;
	m_WAdvancedDlg.m_CoreSize        = m_CoreSize;
	m_WAdvancedDlg.m_bResetWake      = m_bResetWake;
	m_WAdvancedDlg.m_bDirichlet      = m_bDirichlet;
	m_WAdvancedDlg.m_bTrefftz        = m_bTrefftz;
	m_WAdvancedDlg.m_bKeepOutOpps    = m_bKeepOutOpps;
	m_WAdvancedDlg.m_bLogFile        = m_bLogFile;
	m_WAdvancedDlg.m_MinPanelSize    = m_MinPanelSize;
	m_WAdvancedDlg.m_ControlPos      = CPanel::m_CtrlPos;
	m_WAdvancedDlg.m_VortexPos       = CPanel::m_VortexPos;
	m_WAdvancedDlg.m_WakeInterNodes  = m_WakeInterNodes;
	m_WAdvancedDlg.m_bVLM1           = m_bVLM1;

	m_WAdvancedDlg.InitDialog();
	if(m_WAdvancedDlg.exec() == QDialog::Accepted)
	{
		LLTAnalysis::s_CvPrec        = m_WAdvancedDlg.m_AlphaPrec;
		LLTAnalysis::s_RelaxMax      = m_WAdvancedDlg.m_Relax;
		LLTAnalysis::s_NLLTStations  = m_WAdvancedDlg.m_NStation;
		m_Iter                 = m_WAdvancedDlg.m_Iter;
		m_MaxWakeIter          = m_WAdvancedDlg.m_MaxWakeIter;
		m_CoreSize             = m_WAdvancedDlg.m_CoreSize;
		m_bResetWake           = m_WAdvancedDlg.m_bResetWake;
		m_bDirichlet           = m_WAdvancedDlg.m_bDirichlet;
		m_bTrefftz             = m_WAdvancedDlg.m_bTrefftz;
		m_bKeepOutOpps         = m_WAdvancedDlg.m_bKeepOutOpps;
		m_WakeInterNodes       = m_WAdvancedDlg.m_WakeInterNodes;
		m_MinPanelSize         = m_WAdvancedDlg.m_MinPanelSize;
		m_bVLM1                = m_WAdvancedDlg.m_bVLM1;
		m_InducedDragPoint     = m_WAdvancedDlg.m_InducedDragPoint;

		CPanel::m_CtrlPos      = m_WAdvancedDlg.m_ControlPos;
		CPanel::m_VortexPos    = m_WAdvancedDlg.m_VortexPos;

		if(m_WAdvancedDlg.m_bLogFile) m_bLogFile = true; else m_bLogFile = false;
		m_bResetglWake    = true;
		UpdateView();
	}
	pMainFrame->m_DlgPos = m_WAdvancedDlg.pos();
}


void QMiarex::OnClipPlane(int pos)
{
	//
	// The user has modified the position of the clip plane in the 3D view
	//

	double planepos =  (double)pos/100.0;
	m_ClipPlanePos = sinh(planepos) * 0.5;
	UpdateView();
}



void QMiarex::OnCpSection(int pos)
{
	//
	// The user has modified the position span section to display in the Cp view
	//

	m_CurSpanPos = (double)pos/100.0;
	m_pctrlSpanPos->SetValue(m_CurSpanPos);
	CreateCpCurves();
	UpdateView();
}


void QMiarex::OnCpPosition()
{
	//
	// The user has modified the position span section to display in the Cp view
	//
	m_CurSpanPos = m_pctrlSpanPos->Value();
	m_pctrlCpSectionSlider->setValue((int)(m_CurSpanPos*100.0));
	CreateCpCurves();
	UpdateView();
}


void QMiarex::OnCpView()
{
	//
	// The user has switched to the Cp view
	//

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



void QMiarex::OnCurWOppOnly()
{
	//
	// The user has requested a display only of the current operating point
	//

	m_bCurWOppOnly = !m_bCurWOppOnly;
	if(m_iView==WOPPVIEW)
	{
		CreateWOppCurves();
		UpdateView();
	}
	else if(m_iView==WSTABVIEW && m_iStabilityView==STABTIMEVIEW)
	{
		CreateStabilityCurves();
		UpdateView();
	}
	SetControls();
}


void QMiarex::OnCurveColor()
{
	bool bOK;
	QRgb rgb = m_CurveColor.rgba();
	rgb = QColorDialog::getRgba(rgb, &bOK);
	m_CurveColor = QColor::fromRgba(rgb);

	FillComboBoxes();

	UpdateCurve();
}


void QMiarex::OnCurveStyle(int index)
{
	m_CurveStyle = index;
	FillComboBoxes();
	UpdateCurve();
}


void QMiarex::OnCurveWidth(int index)
{
	m_CurveWidth = index+1;
	FillComboBoxes();
	UpdateCurve();
}



void QMiarex::OnDefineStabPolar()
{
	//
	// The user has requested the creation of a new stability polar
	//

	StopAnimate();
	m_bArcball = false;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	m_StabPolarDlg.m_Viscosity     = m_WngAnalysis.m_Viscosity;
	m_StabPolarDlg.m_Density       = m_WngAnalysis.m_Density;
	m_StabPolarDlg.m_RefAreaType   = m_WngAnalysis.m_RefAreaType;
	m_StabPolarDlg.m_bThinSurfaces = m_WngAnalysis.m_bThinSurfaces;

	m_StabPolarDlg.m_pPlane = m_pCurPlane;
	m_StabPolarDlg.m_pWing  = m_pWingList[0];
	m_StabPolarDlg.m_pWing2 = m_pWingList[1];
	m_StabPolarDlg.m_pStab  = m_pWingList[2];
	m_StabPolarDlg.m_pFin   = m_pWingList[3];
	m_StabPolarDlg.m_poaXPolar  = m_poaWPolar;

	CWPolar* pCurWPolar       = new CWPolar;
	if (m_pCurPlane) pCurWPolar->m_UFOName = m_pCurPlane->PlaneName();
	else             pCurWPolar->m_UFOName = m_pCurWing->WingName();

	pCurWPolar->m_WMAChord     = m_pCurWing->m_MAChord;

	m_StabPolarDlg.InitDialog();
	m_StabPolarDlg.move(pMainFrame->m_DlgPos);
	int res = m_StabPolarDlg.exec();
	pMainFrame->m_DlgPos = m_StabPolarDlg.pos();

	if(res == QDialog::Accepted)
	{
		m_WngAnalysis.m_Viscosity       = m_StabPolarDlg.m_Viscosity;
		m_WngAnalysis.m_Density         = m_StabPolarDlg.m_Density;
		m_WngAnalysis.m_RefAreaType     = m_StabPolarDlg.m_RefAreaType;
		m_WngAnalysis.m_bThinSurfaces   = m_StabPolarDlg.m_bThinSurfaces;

		//Then add WPolar to array
		pCurWPolar->m_Mass          = m_StabPolarDlg.m_Mass;

		pCurWPolar->m_bAutoInertia    = m_StabPolarDlg.m_bAutoInertia;
		pCurWPolar->m_CoG.x           = m_StabPolarDlg.m_CoG.x;
		pCurWPolar->m_CoG.z           = m_StabPolarDlg.m_CoG.z;

		pCurWPolar->m_CoGIxx          = m_StabPolarDlg.m_CoGIxx;
		pCurWPolar->m_CoGIyy          = m_StabPolarDlg.m_CoGIyy;
		pCurWPolar->m_CoGIzz          = m_StabPolarDlg.m_CoGIzz;
		pCurWPolar->m_CoGIxz          = m_StabPolarDlg.m_CoGIxz;
		
		pCurWPolar->m_Type            = m_StabPolarDlg.m_PolarType;
		pCurWPolar->m_Beta            = m_StabPolarDlg.m_SideSlip;
		pCurWPolar->m_BankAngle       = m_StabPolarDlg.m_BankAngle;
		pCurWPolar->m_PlrName         = m_StabPolarDlg.m_WPolarName;
		if(pCurWPolar->m_PlrName.length()>60)
		{
			pCurWPolar->m_PlrName = pCurWPolar->m_PlrName.left(60)+"..."+QString("(%1)").arg(m_poaWPolar->size());
		}
		pCurWPolar->m_Density         = m_StabPolarDlg.m_Density;
		pCurWPolar->m_Viscosity       = m_StabPolarDlg.m_Viscosity;
		pCurWPolar->m_bViscous        = m_StabPolarDlg.m_bViscous;
		pCurWPolar->m_RefAreaType     = m_StabPolarDlg.m_RefAreaType;
		pCurWPolar->m_bThinSurfaces   = m_StabPolarDlg.m_bThinSurfaces;

		pCurWPolar->m_bAVLControls    = m_StabPolarDlg.m_bAVLControls;

		if(pCurWPolar->m_RefAreaType==PLANFORMAREA)
		{
			pCurWPolar->m_WArea        = m_pCurWing->m_PlanformArea;
			if(m_pCurPlane && m_pCurPlane->BiPlane()) pCurWPolar->m_WArea += m_pCurPlane->Wing2()->m_PlanformArea;
			pCurWPolar->m_WSpan        = m_pCurWing->m_PlanformSpan;
		}
		else
		{
			pCurWPolar->m_WArea        = m_pCurWing->m_ProjectedArea;
			if(m_pCurPlane && m_pCurPlane->BiPlane()) pCurWPolar->m_WArea += m_pCurPlane->Wing2()->m_ProjectedArea;
			pCurWPolar->m_WSpan        = m_pCurWing->m_ProjectedSpan;
		}
		pCurWPolar->m_bVLM1           = m_bVLM1;
		pCurWPolar->m_bDirichlet      = m_bDirichlet;
		pCurWPolar->m_bTiltedGeom     = false;
		pCurWPolar->m_bWakeRollUp     = false;
		pCurWPolar->m_AnalysisMethod  = PANELMETHOD;
		pCurWPolar->m_bGround         = false;
		pCurWPolar->m_ASpec           = 0.0;
		pCurWPolar->m_Height          = 0.0;

		pCurWPolar->m_TotalWakeLength = m_WngAnalysis.m_TotalWakeLength;
		pCurWPolar->m_WakePanelFactor = m_WngAnalysis.m_WakePanelFactor;
		pCurWPolar->m_NXWakePanels    = m_WngAnalysis.m_NXWakePanels;

		pCurWPolar->m_nControls = m_StabPolarDlg.m_nControls;
		for(int i=0; i<m_StabPolarDlg.m_nControls; i++)
		{
			pCurWPolar->m_bActiveControl[i] = m_StabPolarDlg.m_bActiveControl[i];
			pCurWPolar->m_MinControl[i]     = m_StabPolarDlg.m_MinControl[i];
			pCurWPolar->m_MaxControl[i]     = m_StabPolarDlg.m_MaxControl[i];
		}

		for(int i=m_StabPolarDlg.m_nControls; i<4*MAXCONTROLS; i++)
		{
			pCurWPolar->m_bActiveControl[i] = false;
		}

		pCurWPolar->m_Color = pMainFrame->GetColor(4);
		pCurWPolar->m_Width = 2;
		pCurWPolar->m_bShowPoints = true;

		CWPolar *pWPolar;
		bool bFound;
		for(int i=0; i<30;i++)
		{
			bFound = false;
			for (int j=0; j<m_poaWPolar->size();j++)
			{
				pWPolar = (CWPolar*)m_poaWPolar->at(j);
				if(pWPolar->m_Color == pMainFrame->m_crColors[i]) bFound = true;
			}
			if(!bFound)
			{
				pCurWPolar->m_Color = pMainFrame->m_crColors[i];
				break;
			}
		}
		pCurWPolar->m_bIsVisible  = true;
		pCurWPolar->m_bShowPoints = true;

		pWPolar = GetWPolar(pCurWPolar->m_PlrName);
		pMainFrame->SetSaveState(false);
		if(pWPolar)
		{
			QString strange = tr("The polar already exists");
			QMessageBox::warning(pMainFrame, tr("Warning"), strange);
			delete pCurWPolar;
			m_pCurWPolar = pWPolar;
		}
		else
		{
			m_pCurWPolar = AddWPolar(pCurWPolar);
		}
		
		m_pCurPOpp = NULL;
		m_pCurWOpp = NULL;

		m_bResetglGeom = true;
		m_bResetglOpp  = true;
		m_bResetglMesh = true;
		m_bResetglWake = true;

		SetWPlr();
		pMainFrame->UpdateWPolars();
//		m_iView = WSTABVIEW;
		UpdateView();
	}
	else
	{
		delete pCurWPolar;
	}
	SetControls();
}


void QMiarex::OnDefineWPolar()
{
	//
	// The user has requested the creation of a new performance polar
	//

	StopAnimate();

	if(!m_pCurWing) return;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	int i,j;

	CWPolar* pNewWPolar       = new CWPolar;
	if (m_pCurPlane) pNewWPolar->m_UFOName = m_pCurPlane->PlaneName();
	else             pNewWPolar->m_UFOName = m_pCurWing->WingName();

	pNewWPolar->m_WMAChord     = m_pCurWing->m_MAChord;

	m_WngAnalysis.m_pWing       = m_pCurWing;
	m_WngAnalysis.m_pPlane      = m_pCurPlane;
	m_WngAnalysis.s_pMainFrame  = s_pMainFrame;
	m_WngAnalysis.m_poaWPolar   = m_poaWPolar;
	if(m_pCurPlane) m_WngAnalysis.m_UFOName = m_pCurPlane->PlaneName();
	else            m_WngAnalysis.m_UFOName = m_pCurWing->WingName();

	m_WngAnalysis.InitDialog();
	m_WngAnalysis.move(pMainFrame->m_DlgPos);
	int res = m_WngAnalysis.exec();
	pMainFrame->m_DlgPos = m_WngAnalysis.pos();
	if (res == QDialog::Accepted)
	{
		//Then add WPolar to array
		pNewWPolar->m_RefAreaType     = m_WngAnalysis.m_RefAreaType;
		if(pNewWPolar->m_RefAreaType==PLANFORMAREA)
		{
			pNewWPolar->m_WSpan = m_pCurWing->m_PlanformSpan;
			pNewWPolar->m_WArea = m_pCurWing->m_PlanformArea;
			if(m_pCurPlane && m_pCurPlane->BiPlane()) pNewWPolar->m_WArea += m_pCurPlane->Wing2()->m_PlanformArea;
		}
		else
		{
			pNewWPolar->m_WSpan = m_pCurWing->m_ProjectedSpan;
			pNewWPolar->m_WArea = m_pCurWing->m_ProjectedArea;
			if(m_pCurPlane && m_pCurPlane->BiPlane()) pNewWPolar->m_WArea += m_pCurPlane->Wing2()->m_ProjectedArea;
		}
		pNewWPolar->m_Type            = m_WngAnalysis.m_PolarType;
		pNewWPolar->m_QInf            = m_WngAnalysis.m_QInf;
		pNewWPolar->m_Mass            = m_WngAnalysis.m_Weight;
		pNewWPolar->m_CoG             = m_WngAnalysis.m_CoG;
		pNewWPolar->m_Beta            = m_WngAnalysis.m_Beta;
		pNewWPolar->m_ASpec           = m_WngAnalysis.m_Alpha;
		pNewWPolar->m_PlrName         = m_WngAnalysis.m_WPolarName;
		pNewWPolar->m_Density         = m_WngAnalysis.m_Density;
		pNewWPolar->m_Viscosity       = m_WngAnalysis.m_Viscosity;
		pNewWPolar->m_bTiltedGeom     = m_WngAnalysis.m_bTiltedGeom;
		pNewWPolar->m_bWakeRollUp     = m_WngAnalysis.m_bWakeRollUp;
		pNewWPolar->m_bViscous        = m_WngAnalysis.m_bViscous;
		pNewWPolar->m_AnalysisMethod  = m_WngAnalysis.m_AnalysisMethod;
		pNewWPolar->m_bThinSurfaces   = m_WngAnalysis.m_bThinSurfaces;
		pNewWPolar->m_bAutoInertia    = m_WngAnalysis.m_bPlaneInertia;
		pNewWPolar->m_bGround         = m_WngAnalysis.m_bGround;
		pNewWPolar->m_Height          = m_WngAnalysis.m_Height;
		pNewWPolar->m_TotalWakeLength = m_WngAnalysis.m_TotalWakeLength;
		pNewWPolar->m_WakePanelFactor = m_WngAnalysis.m_WakePanelFactor;
		pNewWPolar->m_NXWakePanels    = m_WngAnalysis.m_NXWakePanels;

		pNewWPolar->m_bVLM1           = m_bVLM1;
		pNewWPolar->m_bDirichlet      = m_bDirichlet;
		pNewWPolar->m_bAVLControls    = false;

		pNewWPolar->m_Color = pMainFrame->GetColor(4);
		CWPolar *pWPolar;
		bool bFound;
		for(i=0; i<30;i++)
		{
			bFound = false;
			for (j=0; j<m_poaWPolar->size();j++)
			{
				pWPolar = (CWPolar*)m_poaWPolar->at(j);
				if(pWPolar->m_Color == pMainFrame->m_crColors[i]) bFound = true;
			}
			if(!bFound)
			{
				pNewWPolar->m_Color = pMainFrame->m_crColors[i];
				break;
			}
		}
		pNewWPolar->m_bIsVisible = true;

		pWPolar = GetWPolar(pNewWPolar->m_PlrName);
		pMainFrame->SetSaveState(false);
		if(pWPolar)
		{
			QString strange = tr("The polar already exists");
			QMessageBox::warning(pMainFrame, tr("Warning"), strange);

			delete pNewWPolar;
			m_pCurWPolar = pWPolar;
		}
		else
		{
			m_pCurWPolar = AddWPolar(pNewWPolar);
		}
		m_pCurPOpp = NULL;
		m_pCurWOpp = NULL;

		m_bResetglGeom = true;
		m_bResetglMesh = true;
		m_bResetglOpp  = true;
		m_bResetglWake = true;

		SetWPlr();
		pMainFrame->UpdateWPolars();
		UpdateView();
		m_pctrlAnalyze->setFocus();
	}
	else
	{
		delete pNewWPolar;
	}
	SetControls();
}


void QMiarex::OnDeleteAllWPlrOpps()
{
	//
	// The user has requested a deletion of all the WOpps or POpps associated to the current WPolar
	//

	if(!m_pCurWPolar) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);
	CWOpp* pWOpp;
	CPOpp* pPOpp;
	int i;
	if(m_pCurPlane)
	{
		for (i = m_poaPOpp->size()-1; i>=0; i--)
		{
			pPOpp = (CPOpp*) m_poaPOpp->at(i);
			if(pPOpp->m_PlrName == m_pCurWPolar->m_PlrName &&
			   pPOpp->m_PlaneName == m_pCurPlane->PlaneName())
			{
				m_poaPOpp->removeAt(i);
				delete pPOpp;
			}
		}
	}
	else if(m_pCurWing)
	{
		for (i = m_poaWOpp->size()-1; i>=0; i--)
		{
			pWOpp = (CWOpp*) m_poaWOpp->at(i);
			if(pWOpp->m_PlrName == m_pCurWPolar->m_PlrName&&
			   pWOpp->m_WingName == m_pCurWing->WingName())
			{
				m_poaWOpp->removeAt(i);
				delete pWOpp;
			}
		}
	}

	m_pCurWOpp = NULL;
	m_pCurPOpp = NULL;
	m_bResetglMesh = true;
	pMainFrame->UpdateWOpps();
	SetWOpp(true);
	if (m_iView==WOPPVIEW)      CreateWOppCurves();
	else if(m_iView==WCPVIEW)   CreateCpCurves();
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();
	SetControls();
	UpdateView();
}


void QMiarex::OnDeleteAllWOpps()
{
	//
	// The user has requested a deletion of all the WOpps or POpps
	//

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);
	CWOpp* pWOpp;
	CPOpp* pPOpp;
	int i;
	for (i = m_poaPOpp->size()-1; i>=0; i--)
	{
		pPOpp = (CPOpp*) m_poaPOpp->at(i);
		m_poaPOpp->removeAt(i);
		delete pPOpp;
	}
	for (i = m_poaWOpp->size()-1; i>=0; i--)
	{
		pWOpp = (CWOpp*) m_poaWOpp->at(i);
		m_poaWOpp->removeAt(i);
		delete pWOpp;
	}


	m_pCurWOpp = NULL;
	m_pCurPOpp = NULL;
	pMainFrame->UpdateWOpps();
	SetWOpp(true);
	if (m_iView==WOPPVIEW)      CreateWOppCurves();
	else if(m_iView==WCPVIEW)   CreateCpCurves();
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();

	SetControls();
	UpdateView();
}


void QMiarex::OnDeleteCurUFO()
{
	//
	// The user has requested a deletion of the current wing of plane
	//

	if(!m_pCurWing) return;
	m_bAnimateWOpp = false;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong;
	if(m_pCurPlane) strong = tr("Are you sure you want to delete the plane :\n") +  m_pCurPlane->PlaneName() +"?\n";
	else 	      strong = tr("Are you sure you want to delete the wing :\n") +   m_pCurWing->WingName() +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(window(), tr("Question"), strong, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;

	if(m_pCurPlane) pMainFrame->DeletePlane(m_pCurPlane);
	else            pMainFrame->DeleteWing(m_pCurWing);

	SetUFO();
	pMainFrame->UpdateUFOs();
	if(m_iView==WPOLARVIEW)     CreateWPolarCurves();
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();
	else if(m_iView==WOPPVIEW)  CreateWOppCurves();
	else if(m_iView==WCPVIEW)   CreateCpCurves();
	SetControls();
	UpdateView();
}


void QMiarex::OnDeleteCurWOpp()
{
	//
	// The user has requested a deletion of the current operating point
	//

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	int i;
	if(m_pCurPOpp)
	{
		CPOpp* pPOpp;
		for (i = m_poaPOpp->size()-1; i>=0; i--)
		{
			pPOpp = (CPOpp*)m_poaPOpp->at(i);
			if(pPOpp == m_pCurPOpp)
			{
				m_poaPOpp->removeAt(i);
				delete pPOpp;
				break;
			}
		}
		m_pCurPOpp = NULL;
		m_pCurWOpp = NULL;
		pMainFrame->UpdateWOpps();
		SetWOpp(true);
		pMainFrame->SetSaveState(false);
		if (m_iView==WOPPVIEW)      CreateWOppCurves();
		else if(m_iView==WCPVIEW)   CreateCpCurves();
		else if(m_iView==WSTABVIEW) CreateStabilityCurves();

		UpdateView();
	}
	else if(m_pCurWOpp)
	{
		CWOpp* pWOpp;
		for (i = m_poaWOpp->size()-1; i>=0; i--)
		{
			pWOpp = (CWOpp*)m_poaWOpp->at(i);
			if(pWOpp == m_pCurWOpp)
			{
				m_poaWOpp->removeAt(i);
				delete pWOpp;
				m_pCurWOpp = NULL;
				break;
			}
		}
		pMainFrame->UpdateWOpps();
		SetWOpp(true);
		if(pMainFrame->m_pctrlWOpp->count())
		{
			QString strong;
			double x;
			pMainFrame->m_pctrlWOpp->setCurrentIndex(0);
			strong = pMainFrame->m_pctrlWOpp->itemText(0);
			bool bRes;
			x = strong.toDouble(&bRes);
			if(bRes)
			{
				m_pCurWOpp = GetWOpp(x);
			}
			else m_pCurWOpp = NULL;
		}
		else
		{
			m_pCurWOpp = NULL;
		}
		pMainFrame->SetSaveState(false);
		if (m_iView==WOPPVIEW)      CreateWOppCurves();
		else if(m_iView==WCPVIEW)   CreateCpCurves();
		else if(m_iView==WSTABVIEW) CreateStabilityCurves();

		UpdateView();
	}

	SetControls();
}




void QMiarex::OnDeleteUFOWOpps()
{
	//
	// The user has requested a deletion of all operating point associated to the wing or plane
	//

	CPOpp *pPOpp;
	CWOpp *pWOpp;
	int i;
	if(m_pCurPlane)
	{
		for (i=m_poaPOpp->size()-1; i>=0; i--)
		{
			pPOpp = (CPOpp*)m_poaPOpp->at(i);
			if (pPOpp->m_PlaneName == m_pCurPlane->PlaneName())
			{
				m_poaPOpp->removeAt(i);
				delete pPOpp;
			}
		}
	}
	else if(m_pCurWing)
	{

		for (i=m_poaWOpp->size()-1; i>=0; i--)
		{
			pWOpp = (CWOpp*)m_poaWOpp->at(i);
			if (pWOpp->m_WingName == m_pCurWing->WingName())
			{
				m_poaWOpp->removeAt(i);
				delete pWOpp;
			}
		}
	}

	m_pCurWOpp = NULL;
	m_pCurPOpp = NULL;

	if (m_iView==WOPPVIEW)      CreateWOppCurves();
	else if(m_iView==WCPVIEW)   CreateCpCurves();
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);
	pMainFrame->UpdateWOpps();
	SetControls();
	UpdateView();
}


void QMiarex::OnDeleteUFOWPolars()
{
	//
	// The user has requested a deletion of all WPolars associated to the wing or plane
	//

	if(!m_pCurWing) return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	CWPolar *pWPolar;
	QString UFOName, strong;

	if(m_pCurPlane) UFOName = m_pCurPlane->PlaneName();
	else            UFOName = m_pCurWing->WingName();

	strong = tr("Are you sure you want to delete the polars associated to :\n") +  UFOName +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(window(), tr("Question"), strong, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;

	for(int j=m_poaWPolar->size()-1; j>=0; j--)
	{
		pWPolar  = (CWPolar *)m_poaWPolar->at(j);
		if(pWPolar && pWPolar->m_UFOName==UFOName)
		{
			//first remove all WOpps and POpps associated to the Wing Polar
			CWOpp * pWOpp;
			for (int i=m_poaWOpp->size()-1; i>=0; i--)
			{
				pWOpp = (CWOpp*)m_poaWOpp->at(i);
				if (pWOpp->m_PlrName == pWPolar->m_PlrName   &&  pWOpp->m_WingName == UFOName)
				{
					m_poaWOpp->removeAt(i);
					delete pWOpp;
				}
			}

			CPOpp * pPOpp;
			for (int i=m_poaPOpp->size()-1; i>=0; i--)
			{
				pPOpp = (CPOpp*)m_poaPOpp->at(i);
				if (pPOpp->m_PlrName == pWPolar->m_PlrName   &&  pPOpp->m_PlaneName== UFOName)
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
	SetWPlr();
	pMainFrame->UpdateWPolars();
	pMainFrame->SetSaveState(false);
	SetControls();
	UpdateView();
}


void QMiarex::OnDeleteCurWPolar()
{
	//
	// The user has requested a deletion of the current WPolar object
	//

	if(!m_pCurWPolar) return;
	m_bAnimateWOpp = false;
	int i;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QString strong, UFOName;

	if(m_pCurPlane)     UFOName = m_pCurPlane->PlaneName();
	else if(m_pCurWing) UFOName = m_pCurWing->WingName();
	else return;

	strong = tr("Are you sure you want to delete the polar :\n") +  m_pCurWPolar->m_PlrName +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(window(), tr("Question"), strong,
												  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;

	//first remove all WOpps associated to the Wing Polar
	CWOpp * pWOpp;
	for (i=m_poaWOpp->size()-1; i>=0; i--)
	{
		pWOpp = (CWOpp*)m_poaWOpp->at(i);
		if (pWOpp->m_PlrName == m_pCurWPolar->m_PlrName   &&  pWOpp->m_WingName == UFOName)
		{
			m_poaWOpp->removeAt(i);
			delete pWOpp;
		}
	}

	//next remove all the POpps associated to the Wing Polar
	CPOpp * pPOpp;
	for (i=m_poaPOpp->size()-1; i>=0; i--)
	{
		pPOpp = (CPOpp*)m_poaPOpp->at(i);
		if (pPOpp->m_PlrName  == m_pCurWPolar->m_PlrName  &&  pPOpp->m_PlaneName == UFOName)
		{
			m_poaPOpp->removeAt(i);
			delete pPOpp;
		}
	}
	//next remove the WPolar
	CWPolar* pWPolar;
	for (i=m_poaWPolar->size()-1;i>=0; i--)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(i);
		if (pWPolar == m_pCurWPolar)
		{
			m_poaWPolar->removeAt(i);
			delete pWPolar;
			break;
		}
	}

	m_pCurPOpp = NULL;
	m_pCurWOpp = NULL;
	m_pCurWPolar = NULL;
	pMainFrame->SetSaveState(false);
	SetWPlr();

	pMainFrame->UpdateWPolars();
	SetControls();
	UpdateView();
}



void QMiarex::OnDownwash()
{
	//
	// The user has toggled the switch for the display of the downwash
	//

	m_bDownwash = m_pctrlDownwash->isChecked();
	UpdateView();
}


void QMiarex::OnDragScale(int pos)
{
	m_DragScale    = pos/100.0/sqrt(1.01-pos/100.0);
	m_bResetglDrag = true;
	UpdateView();
}



void QMiarex::OnDuplicateCurUFO()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	//Duplicate the currently selected wing
	if(!m_pCurWing)	return;

	if(m_pCurPlane)
	{
		DuplicatePlane();
		return;
	}

	QString UFOName;
	if(m_pCurPlane)     UFOName = m_pCurPlane->PlaneName();
	else if(m_pCurWing) UFOName = m_pCurWing->WingName();
	else return;

	CWing* pNewWing= new CWing;
	pNewWing->Duplicate(m_pCurWing);

	if(!SetModWing(pNewWing))
	{
		delete pNewWing;
		UpdateView();
	}
	else
	{
		m_pCurWing = AddWing(pNewWing);
		SetUFO();
		pMainFrame->UpdateUFOs();
		OnEditUFO();
	}
}


void QMiarex::OnEditCurBody()
{
	if(!m_pCurBody || !m_pCurPlane) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	bool bUsed = false;
	int i;
	CPlane *pPlane;
	CWPolar *pWPolar;
	for (i=0; i< m_poaPlane->size(); i++)
	{
		pPlane = (CPlane*)m_poaPlane->at(i);
		if(pPlane->Body() && pPlane->Body()==m_pCurBody)
		{
			// Does this plane have results
			for(int j=0; j<m_poaWPolar->size(); j++)
			{
				pWPolar = (CWPolar*)m_poaWPolar->at(j);
				if(pWPolar->m_UFOName==pPlane->PlaneName() && pWPolar->m_Alpha.size())
				{
					bUsed = true;
					break;
				}
			}
			if(bUsed) break;
		}
	}

	CBody memBody;
	memBody.Duplicate(m_pCurBody);
	m_GL3dBody.SetBody(m_pCurBody);
	m_GL3dBody.m_bEnableName = false;
	m_GL3dBody.InitDialog();
	m_GL3dBody.move(GL3dBodyDlg::s_WindowPos);
	m_GL3dBody.resize(GL3dBodyDlg::s_WindowSize);
        if(GL3dBodyDlg::s_bWindowMaximized) m_GL3dBody.setWindowState(Qt::WindowMaximized);

	if(m_GL3dBody.exec() == QDialog::Accepted)
	{
		if(bUsed)
		{
			m_ModDlg.m_Question = tr("The modification will erase all results for the planes using this body.\nContinue ?");
			m_ModDlg.InitDialog();
			int Ans = m_ModDlg.exec();
			if (Ans == QDialog::Rejected)
			{
				//restore geometry
				m_pCurBody->Duplicate(&memBody);
				return;
			}
			else if(Ans==20)
			{
				CBody* pNewBody= new CBody();
				pNewBody->Duplicate(m_pCurBody);
				m_pCurBody->Duplicate(&memBody);
				if(!SetModBody(pNewBody))
				{
					delete pNewBody;
				}

				return;
			}
			else
			{
				//delete all results associated to planes using this body
				for (i=0; i<m_poaPlane->count();i++)
				{
					pPlane = (CPlane*)m_poaPlane->at(i);
					if(pPlane->Body() == m_pCurBody)
					{
						pMainFrame->DeletePlane(pPlane, true);
					}
				}
			}
		}

		if(m_iView==WPOLARVIEW)     CreateWPolarCurves();
		else if(m_iView==WSTABVIEW) CreateStabilityCurves();
		else if(m_iView==WOPPVIEW)  CreateWOppCurves();
		else if(m_iView==WCPVIEW)   CreateCpCurves();

		SetUFO();
		m_bResetglBody     = true;
		m_bResetglBodyMesh = true;
		m_bResetglGeom     = true;
		m_bResetglMesh     = true;
		pMainFrame->UpdateWOpps();
		pMainFrame->SetSaveState(false);
		m_bIs2DScaleSet = false;
		SetScale();
		UpdateView();
	}
	else m_pCurBody->Duplicate(&memBody);
}


void QMiarex::OnEditCurWPolar()
{
	//Edit the current WPolar data
	if (!m_pCurWPolar) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	CWPolar MemWPolar;
	MemWPolar.Copy(m_pCurWPolar);

	m_EditPlrDlg.move(pMainFrame->m_DlgPos);
	m_EditPlrDlg.m_pWPolar = m_pCurWPolar;
	m_EditPlrDlg.m_pMiarex = this;
	m_EditPlrDlg.InitDialog();

	bool bPoints = m_pCurWPolar->m_bShowPoints;
	m_pCurWPolar->m_bShowPoints = true;

	if(m_iView==WPOLARVIEW)     CreateWPolarCurves();
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();

	UpdateView();

	if(m_EditPlrDlg.exec() == QDialog::Accepted)
	{
		pMainFrame->SetSaveState(false);
	}
	else
	{
		m_pCurWPolar->Copy(&MemWPolar);
	}
	pMainFrame->m_DlgPos = m_EditPlrDlg.pos();
	m_pCurWPolar->m_bShowPoints = bPoints;

	if(m_iView==WPOLARVIEW)
	{
		CreateWPolarCurves();
		if(m_pCurWPolar)
		{
			QString PolarProps;
			m_pCurWPolar->GetPolarProperties(PolarProps);
			m_pctrlPolarProps1->setText(PolarProps);
		}
	}
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();

	UpdateView();
}



void QMiarex::OnEditUFO()
{
	//Edit the currently selected wing
	if(!m_pCurWing)	return;

	if(m_pCurPlane)
	{
		EditCurPlane();
		return;
	}

	int i;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	CWPolar *pWPolar;
	CWOpp* pWOpp;

	bool bHasResults = false;
	for (i=0; i< m_poaWPolar->size(); i++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(i);
		if (pWPolar->m_Alpha.size() && pWPolar->m_UFOName == m_pCurWing->WingName())
		{
			bHasResults = true;
			break;
		}
	}
	for (i=0; i< m_poaWOpp->size(); i++)
	{
		pWOpp = (CWOpp*)m_poaWOpp->at(i);
		if(pWOpp->m_WingName == m_pCurWing->WingName())
		{
			bHasResults = true;
			break;
		}
	}

	CWing* pModWing= new CWing;
	pModWing->Duplicate(m_pCurWing);

	m_WingDlg.m_bAcceptName= false;
	m_WingDlg.InitDialog(pModWing);


	if(QDialog::Accepted == m_WingDlg.exec())
	{
		if(m_WingDlg.m_bChanged)
		{
			if(bHasResults)
			{
				m_ModDlg.m_Question = tr("The modification will erase all results associated to this Wing.\nContinue ?");
				m_ModDlg.InitDialog();
				int Ans = m_ModDlg.exec();
				if (Ans == QDialog::Rejected)
				{
					delete pModWing; // clean up
					return;
				}
				else if(Ans==20)
				{
					//user wants to save as a new wing
					if(!SetModWing(pModWing))
					{
						delete pModWing;
					}
					else
					{
						m_pCurWing = AddWing(pModWing);
					}
					SetUFO();
					pMainFrame->UpdateUFOs();
					UpdateView();
					return;
				}
				else
				{
					//user wants to overwrite
					pMainFrame->DeleteWing(m_pCurWing,true);
					m_pCurWing->Duplicate(pModWing);
				}
			}
			else
			{
				//No results, record the changes without prompting the user
				m_pCurWing->Duplicate(pModWing);
			}

			if(m_iView==W3DVIEW)
			{
				m_bResetglGeom = true;
				m_bResetglMesh = true;
//				m_bResetglOpp  = true;
			}
			else
			{
				if(m_iView==WPOLARVIEW)     CreateWPolarCurves();
				else if(m_iView==WSTABVIEW) CreateStabilityCurves();
				else if(m_iView==WOPPVIEW)  CreateWOppCurves();
				else if(m_iView==WCPVIEW)   CreateCpCurves();
			}
		}

		m_pCurWing->m_WingColor = pModWing->m_WingColor;
		SetUFO();
		pMainFrame->UpdateUFOs();
		m_bResetglStream = true;
		m_bIs2DScaleSet = false;
		SetScale();
		SetWGraphScale();
		OnAdjustToWing();
		UpdateView();
	}
	else
	{
		delete pModWing; // clean up
	}
}



void QMiarex::OnExportBodyDef()
{
	if(!m_pCurBody) return;
	m_pCurBody->ExportDefinition();
}


void QMiarex::OnExportBodyGeom()
{
	if(!m_pCurBody) return;
	m_pCurBody->ExportGeometry(m_GL3dBody.m_NXPoints, m_GL3dBody.m_NHoopPoints);
}


void QMiarex::OnExportCurWOpp()
{
	//Exprt the currently selected WOpp to the text file
	if(!m_pCurWOpp)return ;// is there anything to export ?

	int iStrip,j,k,l,p, coef;
	int type;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString filter;
	if(pMainFrame->m_ExportFileType==1) filter = "Text File (*.txt)";
	else                                filter = "Comma Separated Values (*.csv)";

	QString FileName, sep, str, strong, Format;

	strong = QString("a=%1_v=%2").arg(m_pCurWOpp->m_Alpha, 5,'f',2).arg(m_pCurWOpp->m_QInf*pMainFrame->m_mstoUnit,6,'f',2);
	GetSpeedUnit(str, pMainFrame->m_SpeedUnit);
	strong = m_pCurWOpp->m_WingName+"_"+strong+str;

	strong.replace(" ","");
	strong.replace("/", "");
	FileName = QFileDialog::getSaveFileName(this, tr("Export Wing OpPoint"),
											pMainFrame->m_LastDirName +'/'+strong,
											tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
											&filter);

	if(!FileName.length()) return;
	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);
	pos = FileName.lastIndexOf(".csv");
	if (pos>0) pMainFrame->m_ExportFileType = 2;
	else       pMainFrame->m_ExportFileType = 1;
	type = pMainFrame->m_ExportFileType;


	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);

	if(type==1) sep = ""; else sep=",";


	out << pMainFrame->m_VersionName;
	out << "\n\n";

	if(m_pCurPOpp)		out << m_pCurPOpp->m_PlaneName<< "\n";
	else if(m_pCurWOpp)	out << m_pCurWOpp->m_WingName<< "\n";

	strong = m_pCurWOpp->m_PlrName + "\n";
	out << strong;
	strong = QString("QInf  ="+sep+" %1 "+sep).arg(m_pCurWOpp->m_QInf*pMainFrame->m_mstoUnit,11, 'f', 6);
	GetSpeedUnit(str, pMainFrame->m_SpeedUnit);
	strong+=str+"\n";
	out << strong;

	strong = QString("Alpha = "+sep+"%1\n").arg(m_pCurWOpp->m_Alpha,11, 'f', 6);
	out << strong;

	strong = QString("Beta  = "+sep+"%1").arg(m_pCurWOpp->m_Beta, 8,'f',3);
	strong += QString::fromUtf8("°\n");
	out << strong;

	strong = QString("Phi   = "+sep+"%1").arg(m_pCurWOpp->m_Phi, 8,'f',3);
	strong += QString::fromUtf8("°\n");
	out << strong;

	strong = QString("Ctrl  = "+sep+"%1\n").arg(m_pCurWOpp->m_Ctrl, 8,'f',3);
	out << strong;

	strong = QString("CL    = "+sep+"%1\n").arg(m_pCurWOpp->m_CL,11, 'f', 6);
	out << strong;

	strong = QString("Cy    = "+sep+"%1\n").arg(m_pCurWOpp->m_CY,11, 'f', 6);
	out << strong;

	if(type==1) strong = QString(tr("Cd    = %1     ICd   = %2     PCd   = %3\n"))
		.arg(m_pCurWOpp->m_ICD+m_pCurWOpp->m_VCD,11, 'f', 6)
		.arg(m_pCurWOpp->m_ICD,11, 'f', 6)
		.arg(m_pCurWOpp->m_VCD,11, 'f', 6);
	else        strong = QString(tr("Cd=,%1,ICd=, %2,PCd=, %3\n"))
		.arg(m_pCurWOpp->m_ICD+m_pCurWOpp->m_VCD,11, 'f', 6)
		.arg(m_pCurWOpp->m_ICD,11, 'f', 6)
		.arg(m_pCurWOpp->m_VCD,11, 'f', 6);
	out << strong;

	strong = QString(tr("Cl   = ")+sep+"%1\n").arg(m_pCurWOpp->m_GRm, 11,'g',6);
	out << strong;
	strong = QString(tr("Cm   =")+sep+" %1\n").arg(m_pCurWOpp->m_GCm, 11,'g',6);
	out << strong;

	if(type==1) strong = QString(tr("ICn   = %1     PCn   = %2 \n")).arg(m_pCurWOpp->m_IYm, 11, 'f', 6).arg(m_pCurWOpp->m_GYm, 11, 'f', 6);
	else        strong = QString(tr("ICn=, %1,PCn=, %2\n")).arg(m_pCurWOpp->m_IYm, 11, 'f', 6).arg(m_pCurWOpp->m_GYm, 11, 'f', 6);
	out << strong;

	if(type==1) strong = QString(tr("XCP   = %1     YCP   = %2 \n")).arg(m_pCurWOpp->m_XCP, 11, 'f', 6).arg(m_pCurWOpp->m_YCP, 11, 'f', 6);
	else        strong = QString(tr("XCP=, %1, YCP=, %2 \n")).arg(m_pCurWOpp->m_XCP, 11, 'f', 6).arg(m_pCurWOpp->m_YCP, 11, 'f', 6);
	out << strong;

	if(type==1) strong = QString(tr("XNP   = %1\n")).arg(m_pCurWOpp->m_XNP, 11, 'f', 6);
	else        strong = QString(tr("XNP=, %1\n")).arg(m_pCurWOpp->m_XNP, 11, 'f', 6);
	out << strong;


	strong = QString(tr("Bend. =")+sep+" %1\n\n").arg(m_pCurWOpp->m_MaxBending, 11, 'f', 6);
	out << strong;

	if(m_pCurWPolar->m_Type==STABILITYPOLAR)
	{
		//export non dimensional stability derivatives
		if(type==1) 
		{
//			complex<double> c, angle;
			double u0 = m_pCurWOpp->m_QInf;
			double mac = m_pCurWPolar->m_WArea;
			double b = m_pCurWPolar->m_WSpan;
			
			strong = "\n\n   ___Longitudinal modes____\n\n";
			out << strong;
		
			strong = QString("      Eigenvalue:  %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
				  .arg(m_pCurWOpp->m_EigenValue[0].real(),9, 'g', 4).arg(m_pCurWOpp->m_EigenValue[0].imag(),9, 'g', 4)
				  .arg(m_pCurWOpp->m_EigenValue[1].real(),9, 'g', 4).arg(m_pCurWOpp->m_EigenValue[1].imag(),9, 'g', 4)
				  .arg(m_pCurWOpp->m_EigenValue[2].real(),9, 'g', 4).arg(m_pCurWOpp->m_EigenValue[2].imag(),9, 'g', 4)
				  .arg(m_pCurWOpp->m_EigenValue[3].real(),9, 'g', 4).arg(m_pCurWOpp->m_EigenValue[3].imag(),9, 'g', 4);
			out << strong;
			strong=("                    _____________________________________________________________________________________________________\n");
			out << strong;
		
			strong = QString("             u/u0: %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(m_pCurWOpp->m_EigenVector[0][0].real()/u0, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[0][0].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[1][0].real()/u0, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[1][0].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[2][0].real()/u0, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[2][0].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[3][0].real()/u0, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[3][0].imag()/u0, 9, 'g', 4);
			out << strong;
			strong = QString("             w/u0: %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(m_pCurWOpp->m_EigenVector[0][1].real()/u0, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[0][1].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[1][1].real()/u0, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[1][1].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[2][1].real()/u0, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[2][1].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[3][1].real()/u0, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[3][1].imag()/u0, 9, 'g', 4);
			out << strong;
			strong = QString("     q/(2.u0.MAC): %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(m_pCurWOpp->m_EigenVector[0][2].real()/2/u0/mac, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[0][2].imag()/2/u0/mac, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[1][2].real()/2/u0/mac, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[1][2].imag()/2/u0/mac, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[2][2].real()/2/u0/mac, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[2][2].imag()/2/u0/mac, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[3][2].real()/2/u0/mac, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[3][2].imag()/2/u0/mac, 9, 'g', 4);
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
				  .arg(m_pCurWOpp->m_EigenValue[4].real(),9, 'g', 4).arg(m_pCurWOpp->m_EigenValue[4].imag(),9, 'g', 4)
				  .arg(m_pCurWOpp->m_EigenValue[5].real(),9, 'g', 4).arg(m_pCurWOpp->m_EigenValue[5].imag(),9, 'g', 4)
				  .arg(m_pCurWOpp->m_EigenValue[6].real(),9, 'g', 4).arg(m_pCurWOpp->m_EigenValue[6].imag(),9, 'g', 4)
				  .arg(m_pCurWOpp->m_EigenValue[7].real(),9, 'g', 4).arg(m_pCurWOpp->m_EigenValue[7].imag(),9, 'g', 4);
			out << strong;

			strong=("                    _____________________________________________________________________________________________________\n");
			out << strong;
		
			strong = QString("            v/u0 : %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(m_pCurWOpp->m_EigenVector[4][0].real()/u0, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[4][0].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[5][0].real()/u0, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[5][0].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[6][0].real()/u0, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[6][0].imag()/u0, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[7][0].real()/u0, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[7][0].imag()/u0, 9, 'g', 4);
			out << strong;
			strong = QString("    p/(2.u0.Span): %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(m_pCurWOpp->m_EigenVector[4][1].real()/2.0/u0/b, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[4][1].imag()/2.0/u0/b, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[5][1].real()/2.0/u0/b, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[5][1].imag()/2.0/u0/b, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[6][1].real()/2.0/u0/b, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[6][1].imag()/2.0/u0/b, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[7][1].real()/2.0/u0/b, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[7][1].imag()/2.0/u0/b, 9, 'g', 4);
			out << strong;
			strong = QString("    r/(2.u0.Span): %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(m_pCurWOpp->m_EigenVector[4][2].real()/2.0/u0/b, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[4][2].imag()/2.0/u0/b, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[5][2].real()/2.0/u0/b, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[5][2].imag()/2.0/u0/b, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[6][2].real()/2.0/u0/b, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[6][2].imag()/2.0/u0/b, 9, 'g', 4)
					  .arg(m_pCurWOpp->m_EigenVector[7][2].real()/2.0/u0/b, 9, 'g', 4).arg(m_pCurWOpp->m_EigenVector[7][2].imag()/2.0/u0/b, 9, 'g', 4);
			out << strong;
			strong = QString("         phi(rad): %1+%2i   |   %3+%4i   |   %5+%6i   |   %7+%8i\n")
					  .arg(1.0, 9, 'g', 4).arg(0.0, 9, 'g', 4)
					  .arg(1.0, 9, 'g', 4).arg(0.0, 9, 'g', 4)
					  .arg(1.0, 9, 'g', 4).arg(0.0, 9, 'g', 4)
					  .arg(1.0, 9, 'g', 4).arg(0.0, 9, 'g', 4);
			out << strong;

			strong = "\n";
			out << strong;
			

			strong = QString("CLa = %1\n").arg(m_pCurWOpp->CLa, 11, 'f', 6);		out << strong;
			strong = QString("CLq = %1\n").arg(m_pCurWOpp->CLq, 11, 'f', 6);		out << strong;
			strong = QString("Cma = %1\n").arg(m_pCurWOpp->Cma, 11, 'f', 6);		out << strong;
			strong = QString("Cmq = %1\n").arg(m_pCurWOpp->Cmq, 11, 'f', 6);		out << strong;
			strong = QString("CYb = %1\n").arg(m_pCurWOpp->CYb, 11, 'f', 6);		out << strong;
			strong = QString("CYp = %1\n").arg(m_pCurWOpp->CYp, 11, 'f', 6);		out << strong;
			strong = QString("CYr = %1\n").arg(m_pCurWOpp->CYr, 11, 'f', 6);		out << strong;
			strong = QString("Clb = %1\n").arg(m_pCurWOpp->Clb, 11, 'f', 6);		out << strong;
			strong = QString("Clp = %1\n").arg(m_pCurWOpp->Clp, 11, 'f', 6);		out << strong;
			strong = QString("Clr = %1\n").arg(m_pCurWOpp->Clr, 11, 'f', 6);		out << strong;
			strong = QString("Cnb = %1\n").arg(m_pCurWOpp->Cnb, 11, 'f', 6);		out << strong;
			strong = QString("Cnp = %1\n").arg(m_pCurWOpp->Cnp, 11, 'f', 6);		out << strong;
			strong = QString("Cnr = %1\n").arg(m_pCurWOpp->Cnr, 11, 'f', 6);		out << strong;
			if(m_pCurWOpp->m_nControls>0)
			{
				strong = QString("CXe = %1\n").arg(m_pCurWOpp->CXe[0], 11, 'f', 6);		out << strong;
				strong = QString("CYe = %1\n").arg(m_pCurWOpp->CYe[0], 11, 'f', 6);		out << strong;
				strong = QString("CZe = %1\n").arg(m_pCurWOpp->CZe[0], 11, 'f', 6);		out << strong;
				strong = QString("CLe = %1\n").arg(m_pCurWOpp->CLe[0], 11, 'f', 6);		out << strong;
				strong = QString("CMe = %1\n").arg(m_pCurWOpp->CMe[0], 11, 'f', 6);		out << strong;
				strong = QString("CNe = %1\n").arg(m_pCurWOpp->CNe[0], 11, 'f', 6);		out << strong;
			}
		}
		else
		{
			strong = QString("CLa =,%1\n").arg(m_pCurWOpp->CLa, 11, 'f', 6);		out << strong;
			strong = QString("CLq =,%1\n").arg(m_pCurWOpp->CLq, 11, 'f', 6);		out << strong;
			strong = QString("Cma =,%1\n").arg(m_pCurWOpp->Cma, 11, 'f', 6);		out << strong;
			strong = QString("Cmq =,%1\n").arg(m_pCurWOpp->Cmq, 11, 'f', 6);		out << strong;
			strong = QString("CYb =,%1\n").arg(m_pCurWOpp->CYb, 11, 'f', 6);		out << strong;
			strong = QString("CYp =,%1\n").arg(m_pCurWOpp->CYp, 11, 'f', 6);		out << strong;
			strong = QString("CYr =,%1\n").arg(m_pCurWOpp->CYr, 11, 'f', 6);		out << strong;
			strong = QString("Clb =,%1\n").arg(m_pCurWOpp->Clb, 11, 'f', 6);		out << strong;
			strong = QString("Clp =,%1\n").arg(m_pCurWOpp->Clp, 11, 'f', 6);		out << strong;
			strong = QString("Clr =,%1\n").arg(m_pCurWOpp->Clr, 11, 'f', 6);		out << strong;
			strong = QString("Cnb =,%1\n").arg(m_pCurWOpp->Cnb, 11, 'f', 6);		out << strong;
			strong = QString("Cnp =,%1\n").arg(m_pCurWOpp->Cnp, 11, 'f', 6);		out << strong;
			strong = QString("Cnr =,%1\n").arg(m_pCurWOpp->Cnr, 11, 'f', 6);		out << strong;
			if(m_pCurWOpp->m_nControls>0)
			{
				strong = QString("CXe =, %1\n").arg(m_pCurWOpp->CXe[0], 11, 'f', 6);		out << strong;
				strong = QString("CYe =, %1\n").arg(m_pCurWOpp->CYe[0], 11, 'f', 6);		out << strong;
				strong = QString("CZe =, %1\n").arg(m_pCurWOpp->CZe[0], 11, 'f', 6);		out << strong;
				strong = QString("CLe =, %1\n").arg(m_pCurWOpp->CLe[0], 11, 'f', 6);		out << strong;
				strong = QString("CMe =, %1\n").arg(m_pCurWOpp->CMe[0], 11, 'f', 6);		out << strong;
				strong = QString("CNe =, %1\n").arg(m_pCurWOpp->CNe[0], 11, 'f', 6);		out << strong;
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
				strong = QString(tr("Flap ")+sep+"%1"+sep+" moment = "+sep+"%2 ").arg(l+1,4).arg(m_pWOpp[iw]->m_FlapMoment[l]*pMainFrame->m_NmtoUnit, 9,'f',4);
				GetMomentUnit(str, pMainFrame->m_MomentUnit);
				strong += str +"\n";
				out << strong;
			}
			out << ("\n");
			m_pWOpp[iw]->Export(out, type);
		}
	}

	if(m_pCurWOpp->m_AnalysisMethod>=2)
	{
		if(m_pCurPOpp) out << tr("Main Wing Cp Coefficients\n");
		else           out << tr("Wing Cp Coefficients\n");
		coef = 1;

		if(!m_pCurWPolar->m_bThinSurfaces)
		{
			coef = 2;
		}
		if(type==1) out << tr(" Panel     CtrlPt.x        CtrlPt.y        CtrlPt.z             Cp\n");
		else        out << tr("Panel,CtrlPt.x,CtrlPt.y,CtrlPt.z,Cp\n");

		if(type==1) Format = "%1     %2     %3     %4     %5\n";
		else        Format = "%1,%2,%3,%4,%5\n";


		for(int iw=0; iw<MAXWINGS; iw++)
		{
			if(m_pWingList[iw])
			{
				out << m_pWingList[iw]->m_WingName+ tr("Cp Coefficients\n");
				p=0;
				iStrip = 0;
				for (j=0; j<m_pWingList[iw]->m_NSurfaces; j++)
				{
					if(m_pWingList[iw]->m_Surface[j].m_bIsTipLeft) p+= m_pWingList[iw]->m_Surface[j].m_NXPanels;

					for(k=0; k<m_pWingList[iw]->m_Surface[j].m_NYPanels; k++)
					{
						iStrip++;
						strong = QString(tr("Strip %1\n")).arg(iStrip);
						out << strong;

						for(l=0; l<m_pWingList[iw]->m_Surface[j].m_NXPanels * coef; l++)
						{
							if(m_pWingList[iw]->m_pPanel[p].m_iPos==0)
							{
								strong = QString(Format).arg(p,4).arg(m_pWingList[iw]->m_pPanel[p].CtrlPt.x,11,'e',3).arg(m_pWingList[iw]->m_pPanel[p].CtrlPt.y,11,'e',3).arg(m_pWingList[iw]->m_pPanel[p].CtrlPt.z,11,'e',3).arg(m_pWOpp[iw]->m_Cp[p],11,'f',4);
							}
							else
							{
								strong = QString(Format).arg(p,4).arg(m_pWingList[iw]->m_pPanel[p].CollPt.x,11,'e',3).arg(m_pWingList[iw]->m_pPanel[p].CollPt.y,11,'e',3).arg(m_pWingList[iw]->m_pPanel[p].CollPt.z,11,'e',3).arg(m_pWOpp[iw]->m_Cp[p],11,'f',4);
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



void QMiarex::OnExportCurWPolar()
{
	if (!m_pCurWPolar) return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString FileName, filter;

	if(pMainFrame->m_ExportFileType==1) filter = "Text File (*.txt)";
	else                                filter = "Comma Separated Values (*.csv)";

	FileName = m_pCurWPolar->m_PlrName;
	FileName.replace("/", " ");
	FileName = QFileDialog::getSaveFileName(this, tr("Export Polar"),
											pMainFrame->m_LastDirName + "/"+FileName,
											tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
											&filter);

	if(!FileName.length()) return;
	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);
	pos = FileName.lastIndexOf(".csv");
	if (pos>0) pMainFrame->m_ExportFileType = 2;
	else       pMainFrame->m_ExportFileType = 1;

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);
	m_pCurWPolar->Export(out, pMainFrame->m_ExportFileType);
	XFile.close();

	UpdateView();
}



void QMiarex::OnExporttoAVL()
{
	if (!m_pCurWing) return;
	QString filter =".avl";

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString FileName, strong;


	if(m_pCurPlane) FileName = m_pCurPlane->PlaneName();
	else            FileName = m_pCurWing->WingName();
	FileName.replace("/", " ");
	FileName = QFileDialog::getSaveFileName(this, tr("Export UFO"),
                                                        pMainFrame->m_LastDirName + "/"+FileName,
							tr("AVL Text File (*.avl)"),
							&filter);
	if(!FileName.length()) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);

	strong = pMainFrame->m_ProjectName;
	int len = strong.length();
	if (strong.right(1) == "*") strong = strong.left(len-1);
	if(!strong.length()) out << tr("Project");
	else out << strong;
	out << "\n";
	out << "0.0                          | Mach\n";
	if(m_pCurWing->m_bSymetric) out << ("0     0     0.0              | iYsym  iZsym  Zsym\n");
	else                        out << ("0     0     0.0              | iYsym  iZsym  Zsym\n");
	out << ("\n");

	strong = QString("%1   %2   %3  | Sref   Cref   Bref\n")
                         .arg(m_pCurWing->m_PlanformArea*pMainFrame->m_mtoUnit*pMainFrame->m_mtoUnit, 8, 'f', 3)
			 .arg(m_pCurWing->m_MAChord*pMainFrame->m_mtoUnit,       8, 'f', 3)
			 .arg(m_pCurWing->m_PlanformSpan*pMainFrame->m_mtoUnit,  8, 'f', 3);
	out << strong;

	if(m_pCurPlane)
		strong = QString("%1  %2  %3          | Xref   Yref   Zref\n")
                                 .arg(m_pCurPlane->CoG().x*pMainFrame->m_mtoUnit,8,'f',3)
                                 .arg(m_pCurPlane->CoG().y*pMainFrame->m_mtoUnit,8,'f',3)
                                 .arg(m_pCurPlane->CoG().z*pMainFrame->m_mtoUnit,8,'f',3);
	else if(m_pCurWing)
		strong = QString("%1  %2  %3          | Xref   Yref   Zref\n")
                                 .arg(m_pCurWing->m_CoG.x*pMainFrame->m_mtoUnit,8,'f',3)
                                 .arg(m_pCurWing->m_CoG.y*pMainFrame->m_mtoUnit,8,'f',3)
                                 .arg(m_pCurWing->m_CoG.z*pMainFrame->m_mtoUnit,8,'f',3);

	out << strong;

	out << (" 0.00                        | CDp  (optional)\n");

	out << ("\n\n\n");

	int index = rand();

	if(m_pCurPlane) m_pCurWing->ExportAVLWing(out, index, 0.0, 0.0, 0.0, 0.0, m_pCurPlane->WingTiltAngle(0));
	else            m_pCurWing->ExportAVLWing(out, index, 0.0, 0.0, 0.0, 0.0, 0.0);

	for(int iw=1; iw<MAXWINGS; iw++)
	{
		if(m_pWingList[iw]) m_pWingList[iw]->ExportAVLWing(out, index+iw, 0.0, 0.0, 0.0, 0.0, m_pCurPlane->WingTiltAngle(iw));
	}
	XFile.close();
}



void QMiarex::OnFinCurve()
{
	m_bShowWingCurve[3] = !m_bShowWingCurve[3];
//	CheckMenus();

	if (m_iView==WOPPVIEW)    CreateWOppCurves();
	else if(m_iView==WCPVIEW) CreateCpCurves();

	UpdateView();
}


void QMiarex::OnFoilNames()
{
	m_bFoilNames = m_pctrlFoilNames->isChecked();
	UpdateView();
}


void QMiarex::OnMasses()
{
	m_bShowMasses = m_pctrlMasses->isChecked();
	UpdateView();
}



void QMiarex::OnGL3DScale()
{
	if(m_iView != W3DVIEW)
	{
//		m_pctrl3DSettings->setChecked(false);
		return;
	}
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(pMainFrame->m_pctrl3DScalesWidget->isVisible())	pMainFrame->m_pctrl3DScalesWidget->hide();
	else                                               pMainFrame->m_pctrl3DScalesWidget->show();

	pMainFrame->W3DScalesAct->setChecked(pMainFrame->m_pctrl3DScalesWidget->isVisible());
//	if(m_pctrl3DSettings->isChecked()) pMainFrame->m_pctrl3DScalesWidget->show();
//	else                               pMainFrame->m_pctrl3DScalesWidget->hide();
}


void QMiarex::OnGraphSettings()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QGraph *pGraph = NULL;

	pGraph = m_pCurGraph;
	if(!pGraph) return;
	if(m_iView==WOPPVIEW)
	{
		m_GraphDlg.m_iGraphType = 61;
	}
	else if(m_iView==WPOLARVIEW)
	{
		m_GraphDlg.m_iGraphType = 62;
	}
	else if(m_iView==WCPVIEW) m_GraphDlg.m_iGraphType = 64;

	if(!pGraph) return;

	QGraph graph;
	graph.CopySettings(pGraph);
	m_GraphDlg.move(pMainFrame->m_DlgPos);
	m_GraphDlg.m_pMemGraph = &graph;
	m_GraphDlg.m_pGraph = pGraph;
	m_GraphDlg.SetParams();

	if(m_GraphDlg.exec() == QDialog::Accepted)
	{
		if(m_iView==WOPPVIEW)
		{

			if(m_GraphDlg.m_bVariableChanged)
			{
				m_pCurGraph->SetAutoY(true);
				m_pCurGraph->SetAutoYMinUnit(true);
			}
			CreateWOppCurves();
		}
		else if(m_iView==WPOLARVIEW)
		{
			if(pGraph ==      m_WPlrGraph)   SetWGraphTitles(m_WPlrGraph);
			else if(pGraph == m_WPlrGraph+1) SetWGraphTitles(m_WPlrGraph+1);
			else if(pGraph == m_WPlrGraph+2) SetWGraphTitles(m_WPlrGraph+2);
			else if(pGraph == m_WPlrGraph+3) SetWGraphTitles(m_WPlrGraph+3);

			if(m_GraphDlg.m_bVariableChanged)
			{
				m_pCurGraph->SetAuto(true);
				m_pCurGraph->SetAutoYMinUnit(true);
			}
			CreateWPolarCurves();
		}
		else if(m_iView==WSTABVIEW)
		{
			CreateStabilityCurves();
			SetStabGraphTitles();
		}
	}
	else
	{
		pGraph->CopySettings(&graph);
	}
	pMainFrame->m_DlgPos = m_GraphDlg.pos();
	UpdateView();
}




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



void QMiarex::OnHideAllWPolars()
{
	int i;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	CWPolar *pWPolar;
	for (i=0; i<m_poaWPolar->size(); i++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(i);
		pWPolar->m_bIsVisible = false;
		if(pWPolar->m_Type==STABILITYPOLAR) pWPolar->m_bShowPoints = false;
	}
	if(m_iView==WPOLARVIEW)		CreateWPolarCurves();
	else if(m_iView==WSTABVIEW)	CreateStabilityCurves();
	pMainFrame->SetSaveState(false);
	SetCurveParams();
	UpdateView();
}



void QMiarex::OnHideAllWPlrOpps()
{
	int i;
	//Switch all WOpps view to on for the current UFO and WPolar
	m_bCurWOppOnly = false;

	CPOpp *pPOpp;
	CWOpp *pWOpp;
	if(m_pCurPlane)
	{
		for (i=0; i< m_poaPOpp->size(); i++)
		{
			pPOpp = (CPOpp*)m_poaPOpp->at(i);
			if (pPOpp->m_PlaneName == m_pCurWPolar->m_UFOName &&
				pPOpp->m_PlrName   == m_pCurWPolar->m_PlrName)
			{
				pPOpp->m_bIsVisible = false;
			}
		}
	}
	else if (m_pCurWing)
	{
		for (i=0; i< m_poaWOpp->size(); i++)
		{
			pWOpp = (CWOpp*)m_poaWOpp->at(i);
			if (pWOpp->m_WingName == m_pCurWPolar->m_UFOName &&
				pWOpp->m_PlrName  == m_pCurWPolar->m_PlrName)
			{
				pWOpp->m_bIsVisible = false;
			}
		}
	}
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);
	SetCurveParams();

	if (m_iView==WOPPVIEW)      CreateWOppCurves();
	else if(m_iView==WCPVIEW)   CreateCpCurves();
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();

	UpdateView();
}


void QMiarex::OnHideAllWOpps()
{
	int i;
	//Switch all WOpps view to on for the current UFO and WPolar
	m_bCurWOppOnly = false;

	CPOpp *pPOpp;
	CWOpp *pWOpp;
	for (i=0; i< m_poaWOpp->size(); i++)
	{
		pWOpp = (CWOpp*)m_poaWOpp->at(i);
		pWOpp->m_bIsVisible = false;

	}
	for (i=0; i< m_poaPOpp->size(); i++)
	{
		pPOpp = (CPOpp*)m_poaPOpp->at(i);
		pPOpp->m_bIsVisible = false;
	}
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);
	SetCurveParams();

	if (m_iView==WOPPVIEW)      CreateWOppCurves();
	else if(m_iView==WCPVIEW)   CreateCpCurves();
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();

	UpdateView();
}


void QMiarex::OnHideUFOWOpps()
{
	//Switch all WOpps view to off for the current UFO
	CPOpp *pPOpp;
	CWOpp *pWOpp;
	int i;
	for (i=0; i< m_poaWOpp->size(); i++)
	{
		pWOpp = (CWOpp*)m_poaWOpp->at(i);
		if (pWOpp->m_WingName == m_pCurWPolar->m_UFOName)
		{
			pWOpp->m_bIsVisible = false;
		}
	}
	for (i=0; i< m_poaPOpp->size(); i++)
	{
		pPOpp = (CPOpp*)m_poaPOpp->at(i);
		if (pPOpp->m_PlaneName == m_pCurWPolar->m_UFOName)
		{
			pPOpp->m_bIsVisible = false;
		}
	}

	if (m_iView==WOPPVIEW)      CreateWOppCurves();
	else if(m_iView==WCPVIEW)   CreateCpCurves();
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);
	SetCurveParams();
	UpdateView();
}


void QMiarex::OnHideUFOWPolars()
{
	if(!m_pCurWing) return;
	int i;
	QString UFOName;
	if(m_pCurPlane)     UFOName = m_pCurPlane->PlaneName();
	else if(m_pCurWing) UFOName = m_pCurWing->WingName();
	else return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	CWPolar *pWPolar;
	for (i=0; i<m_poaWPolar->size(); i++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(i);
		if (pWPolar->m_UFOName == UFOName)
		{
			pWPolar->m_bIsVisible = false;
			if(pWPolar->m_Type==STABILITYPOLAR) pWPolar->m_bShowPoints = false;
		}
	}

	if(m_iView==WPOLARVIEW)		CreateWPolarCurves();
	else if(m_iView==WSTABVIEW)	CreateStabilityCurves();

	SetCurveParams();
	pMainFrame->SetSaveState(false);
	UpdateView();
}



void QMiarex::OnHighlightWOpp()
{
	if(m_iView!=WPOLARVIEW && !(m_iView==WSTABVIEW && m_iStabilityView==STABPOLARVIEW)) return;
	m_bHighlightOpp = !m_bHighlightOpp;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->highlightWOppAct->setChecked(m_bHighlightOpp);

	for(int ig=0; ig<4; ig++)
		m_WPlrGraph[ig].m_bHighlightPoint = m_bHighlightOpp;

	if(m_iView==WPOLARVIEW)		CreateWPolarCurves();
	else if(m_iView==WSTABVIEW)	CreateStabilityCurves();

	UpdateView();
}



void QMiarex::OnImportBody()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	CBody *pNewBody = new CBody();
	if(!pNewBody) return;
	if(!pNewBody->ImportDefinition())
	{
		delete pNewBody;
		return;
	}

	if(!SetModBody(pNewBody))
	{
		delete pNewBody;
		UpdateView();
	}
	else
	{
//		if(!m_pCurBody) return;
		m_pCurBody = pNewBody;
		CBody memBody;
		memBody.Duplicate(m_pCurBody);
		m_GL3dBody.SetBody(m_pCurBody);
		m_GL3dBody.m_bEnableName = false;
		m_GL3dBody.move(GL3dBodyDlg::s_WindowPos);
		m_GL3dBody.resize(GL3dBodyDlg::s_WindowSize);
		if(GL3dBodyDlg::s_bWindowMaximized) m_GL3dBody.setWindowState(Qt::WindowMaximized);

		if(m_GL3dBody.exec() == QDialog::Accepted)
		{
			m_bResetglBody = true;
			m_bResetglBodyMesh = true;
			m_bResetglGeom = true;
			m_bResetglMesh = true;
		}
		else m_pCurBody->Duplicate(&memBody);

		m_pCurBody = NULL;
		UpdateView();
		pMainFrame->SetSaveState(false);
	}
}


void QMiarex::OnImportWPolar()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	CWPolar *pWPolar = new CWPolar;
	bool bOK;
	QString PolarName, UFOName;
	QString strong, str;

	QString PathName;

	QByteArray textline;
	const char *text;

	PathName = QFileDialog::getOpenFileName(pMainFrame, tr("Open File"),
											pMainFrame->m_LastDirName,
											tr("UFO Polar Format (*.*)"));
	if(!PathName.length())		return ;
	int pos = PathName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = PathName.left(pos);

	QFile XFile(PathName);
	if (!XFile.open(QIODevice::ReadOnly))
	{
		QString strange = tr("Could not read the file\n")+PathName;
		QMessageBox::warning(pMainFrame, tr("Warning"), strange);
		return;
	}

	QTextStream in(&XFile);
	int res, Line;

	bool bRead;
	bRead  = ReadAVLString(in, Line, strong);// XFoil or XFLR5 version
	bRead  = ReadAVLString(in, Line, strong);// UFO Name

	UFOName = strong.right(strong.length()-19);
	UFOName = UFOName.trimmed();

	CPlane *pPlane = GetPlane(UFOName);
	CWing *pWing   = GetWing(UFOName);

	if(!pWing && !pPlane)
	{
		str = tr("No UFO with the name ")+UFOName;
		str+= tr("\ncould be found. The polar(s) will not be stored");
		delete pWPolar;
		QMessageBox::warning(pMainFrame, tr("Warning"), str);
		return;
	}

	pWPolar->m_UFOName = UFOName;

	bRead  = ReadAVLString(in, Line, strong);// Polar Name
	PolarName = strong.right(strong.length()-19);
	pWPolar->m_PlrName = PolarName;

	bRead  = ReadAVLString(in, Line, strong);// Freestream speed
	pWPolar->m_QInf = strong.toDouble(&bOK)/pMainFrame->m_mstoUnit;

	bRead  = ReadAVLString(in, Line, strong);// "   alpha      CL          ICd   ..."
	bRead  = ReadAVLString(in, Line, strong);// " _________  ________   ________  ..."

	double alpha, CL, ICd, PCd, TCd, CY, GCm, GRm, GYm, IYm, QInf, XCP;
	while( bRead)
	{
		bRead  = ReadAVLString(in, Line, strong);// polar data
		if(bRead)
		{
			if(strong.length())
			{
				textline = strong.toAscii();
				text = textline.constData();

				res = sscanf(text, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf", &alpha, &CL, &ICd, &PCd, &TCd, &CY, &GCm, &GRm, &GYm, &IYm, &QInf, &XCP);

				if (res >0)
				{
					pWPolar->AddPoint(alpha, CL, ICd, PCd, CY, GCm, 0.0, 0.0, GRm, GYm, IYm, QInf, XCP);
				}
				else
				{
					bRead = false;
				}
			}
		}
	}

	pWPolar->m_Color = pMainFrame->GetColor(4);

	m_pCurWPolar = AddWPolar(pWPolar);

	m_pCurWOpp = NULL;
	SetWPlr(pWPolar);
	pMainFrame->UpdateWPolars();
	UpdateView();
	pMainFrame->SetSaveState(false);
}


void QMiarex::OnInitLLTCalc()
{
	m_bInitLLTCalc = m_pctrlInitLLTCalc->isChecked();
}


void QMiarex::OnKeepCpSection()
{
	CCurve *pCurve, *pNewCurve;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	pCurve = m_CpGraph.GetCurve(0);
	pNewCurve = m_CpGraph.AddCurve();
	pNewCurve->Copy(pCurve);

	m_CpColor = pMainFrame->m_crColors[(m_CpGraph.GetCurveCount())%24];
	pCurve->SetColor(m_CpColor);

	m_CpStyle = 0;
	m_CpWidth = 1;
	m_bShowCpPoints = false;
	SetCurveParams();

//	m_CurSpanPos = m_SpanPos;
	CreateCpCurves();
	UpdateView();
}



void QMiarex::OnLiftScale(int pos)
{
	m_LiftScale    = pos/100.0/sqrt(1.01-pos/100.0);
	m_bResetglLift = true;
	UpdateView();
}



void QMiarex::OnLight()
{
	m_bglLight = m_pctrlLight->isChecked();
//	m_bResetglGeom = true;
	UpdateView();
}


void QMiarex::OnManageBodies()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	m_ManageBodiesDlg.move(pMainFrame->m_DlgPos);
	m_ManageBodiesDlg.m_pGL3dBodyDlg = &m_GL3dBody;
	m_ManageBodiesDlg.m_poaBody = m_poaBody;
	m_ManageBodiesDlg.m_poaPlane = m_poaPlane;
	m_ManageBodiesDlg.InitDialog();
	m_ManageBodiesDlg.exec();
	pMainFrame->m_DlgPos = m_ManageBodiesDlg.pos();
}

void QMiarex::OnManageUFOs()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QString UFOName = "";
	if(m_pCurPlane)     UFOName = m_pCurPlane->PlaneName();
	else if(m_pCurWing) UFOName = m_pCurWing->WingName();

	m_ManageUFOsDlg.InitDialog(UFOName);
	m_ManageUFOsDlg.exec();

	if(m_ManageUFOsDlg.m_pPlane)     SetUFO(m_ManageUFOsDlg.m_pPlane->PlaneName());
	else if(m_ManageUFOsDlg.m_pWing) SetUFO(m_ManageUFOsDlg.m_pWing->m_WingName);
	else                             SetUFO();

	pMainFrame->UpdateUFOs();
	SetControls();

	m_bResetglGeom = true;
	m_bResetglMesh = true;
	UpdateView();
}



void QMiarex::OnMoment()
{
	m_bMoments = m_pctrlMoment->isChecked();
	UpdateView();
}


void QMiarex::OnNewBody()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	CBody *pBody = new CBody;

	m_GL3dBody.SetBody(pBody);
	m_GL3dBody.move(GL3dBodyDlg::s_WindowPos);
	m_GL3dBody.resize(GL3dBodyDlg::s_WindowSize);
	if(GL3dBodyDlg::s_bWindowMaximized) m_GL3dBody.setWindowState(Qt::WindowMaximized);
	m_GL3dBody.InitDialog();

	if(m_GL3dBody.exec() == QDialog::Accepted)
	{
		AddBody(pBody);
		pMainFrame->SetSaveState(false);
	}
	else delete pBody;
}


void QMiarex::OnNewWing()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	//Define a wing from scratch
	//On exit, check if the wing's name is already used
	int i;
	CWing *pOldWing;
	CWing* pWing = new CWing;

//	WingDlg WingDlg;
	m_WingDlg.m_bAcceptName= true;
	if(!m_WingDlg.InitDialog(pWing)) return;

	if(QDialog::Accepted == m_WingDlg.exec())
	{
		pMainFrame->SetSaveState(false);
		bool bExists = false;
		for(i=0; i<m_poaWing->size(); i++)
		{
			pOldWing = (CWing*)m_poaWing->at(i);
			if(pWing->m_WingName == pOldWing->m_WingName)
			{
				bExists = true;
				break;
			}
		}

		if(bExists)
		{
			if(!SetModWing(pWing))
			{
				delete pWing;
				SetControls();
				return;
			}
		}

		m_pCurWing = AddWing(pWing);
		pMainFrame->SetSaveState(false);
		m_pCurPlane = NULL;
		SetUFO();
		pMainFrame->UpdateUFOs();
		m_bIs2DScaleSet = false;
		SetScale();
		if(m_iView==WOPPVIEW)	OnAdjustToWing();
		SetWGraphScale();

		UpdateView();
	}
	else
	{
		delete pWing;
	}
	SetControls();
}


void QMiarex::OnNewPlane()
{
	int i;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	CPlane* pPlane = new CPlane;
	PlaneDlg PDlg;
	PDlg.m_pPlane = pPlane;
	CPlane* pOldPlane = NULL;
	PDlg.m_bAcceptName = true;
	PDlg.InitDialog();

	if(QDialog::Accepted == PDlg.exec())
	{
		pMainFrame->SetSaveState(false);

		bool bExists = false;
		for(i=0; i<m_poaPlane->size(); i++)
		{
			pOldPlane = (CPlane*)m_poaPlane->at(i);
			if(pPlane->PlaneName() == pOldPlane->PlaneName())
			{
				bExists = true;
				break;
			}
		}

		if(bExists)
		{
			if(!SetModPlane(pPlane))
			{
				delete pPlane;
				SetControls();
				return;
			}
		}

		m_pCurPlane = AddPlane(pPlane);
		SetUFO();
		m_bResetglLegend = true;
		pMainFrame->UpdateUFOs();
	}
	else
	{
		delete pPlane;
	}
	SetControls();
	UpdateView();
}


void QMiarex::OnOutline()
{
	m_bOutline = m_pctrlOutline->isChecked();
	UpdateView();
}


void QMiarex::OnPanels()
{
	m_bVLMPanels = m_pctrlPanels->isChecked();
	UpdateView();
}


void QMiarex::OnReadAnalysisData()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	m_bSequence = m_pctrlSequence->isChecked();
	m_bInitLLTCalc = m_pctrlInitLLTCalc->isChecked();

	if(m_pCurWPolar && m_pCurWPolar->m_Type==FIXEDAOAPOLAR)
	{
		m_QInfMin   = m_pctrlAlphaMin->Value()         /pMainFrame->m_mstoUnit;
		m_QInfMax   = m_pctrlAlphaMax->Value()         /pMainFrame->m_mstoUnit;
		m_QInfDelta = fabs(m_pctrlAlphaDelta->Value()) /pMainFrame->m_mstoUnit;
		if(fabs(m_QInfDelta)<0.1)
		{
			m_QInfDelta = 1.0;
			m_pctrlAlphaDelta->SetValue(1.0);
		}
	}
	else if(m_pCurWPolar && (m_pCurWPolar->m_Type==5 || m_pCurWPolar->m_Type==6 || m_pCurWPolar->m_Type==STABILITYPOLAR))
	{
		m_ControlMin   = m_pctrlAlphaMin->Value()         /pMainFrame->m_mstoUnit;
		m_ControlMax   = m_pctrlAlphaMax->Value()         /pMainFrame->m_mstoUnit;
		m_ControlDelta = fabs(m_pctrlAlphaDelta->Value()) /pMainFrame->m_mstoUnit;
		if(fabs(m_ControlDelta)<0.001)
		{
			m_ControlDelta = 0.001;
			m_pctrlAlphaDelta->SetValue(0.001);
		}
	}
	else if(m_pCurWPolar)
	{
		m_AlphaMin   = m_pctrlAlphaMin->Value();
		m_AlphaMax   = m_pctrlAlphaMax->Value();
		m_AlphaDelta = fabs(m_pctrlAlphaDelta->Value());

		if(fabs(m_AlphaDelta)<0.01)
		{
			m_AlphaDelta = 0.01;
			m_pctrlAlphaDelta->SetValue(0.01);
		}
	}
}


void QMiarex::OnPolarFilter()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	m_PolarFilterDlg.move(pMainFrame->m_DlgPos);
	m_PolarFilterDlg.m_bMiarex = true;
	m_PolarFilterDlg.m_bType1 = m_bType1;
	m_PolarFilterDlg.m_bType2 = m_bType2;
	m_PolarFilterDlg.m_bType4 = m_bType4;
	m_PolarFilterDlg.m_bType5 = m_bType5;
	m_PolarFilterDlg.m_bType6 = m_bType6;
	m_PolarFilterDlg.m_bType7 = m_bType7;
	m_PolarFilterDlg.InitDialog();
	if(m_PolarFilterDlg.exec()==QDialog::Accepted)
	{
		m_bType1 = m_PolarFilterDlg.m_bType1;
		m_bType2 = m_PolarFilterDlg.m_bType2;
		m_bType4 = m_PolarFilterDlg.m_bType4;
		m_bType5 = m_PolarFilterDlg.m_bType5;
		m_bType6 = m_PolarFilterDlg.m_bType6;
		m_bType7 = m_PolarFilterDlg.m_bType7;
		if(m_iView==WPOLARVIEW)
		{
			CreateWPolarCurves();
			UpdateView();
		}
		else if(m_iView==WSTABVIEW)
		{
			CreateStabilityCurves();
			UpdateView();
		}
	}
	pMainFrame->m_DlgPos = m_PolarFilterDlg.pos();
}


void QMiarex::OnRenameCurWPolar()
{
	//Rename the currently selected Wing Polar

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(!m_pCurWPolar) return;
	if(!m_pCurWing) return;
	int resp, k,l;
	CWPolar* pWPolar = NULL;
	CPOpp * pPOpp = NULL;
	CWOpp * pWOpp = NULL;
	QString OldName = m_pCurWPolar->m_PlrName;
	QString UFOName;

	if(m_pCurPlane)     UFOName = m_pCurPlane->PlaneName();
	else if(m_pCurWing) UFOName = m_pCurWing->WingName();

	QStringList NameList;
	for(k=0; k<m_poaWPolar->size(); k++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(k);
		if(pWPolar->m_UFOName==UFOName) NameList.append(pWPolar->m_PlrName);
	}


	m_RenameDlg.move(pMainFrame->m_DlgPos);
	m_RenameDlg.m_pstrArray = & NameList;

	m_RenameDlg.m_strQuestion = tr("Enter the new name for the wing polar :");
	m_RenameDlg.m_strName     = m_pCurWPolar->m_PlrName;
	m_RenameDlg.InitDialog();

	bool bExists = true;

	while (bExists)
	{
		resp = m_RenameDlg.exec();
		pMainFrame->m_DlgPos = m_RenameDlg.pos();
		if(resp==QDialog::Accepted)
		{
			if (OldName == m_RenameDlg.m_strName) return;
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaWPolar->size(); k++)
			{
				pWPolar = (CWPolar*)m_poaWPolar->at(k);
				if (pWPolar->m_PlrName == m_RenameDlg.m_strName &&	pWPolar->m_UFOName == m_pCurWing->WingName())
				{
					bExists = true;
					break;
				}
			}
			if(!bExists)
			{
				m_pCurWPolar->m_PlrName = m_RenameDlg.m_strName;
				if(m_pCurPlane)
				{
					for (l=m_poaPOpp->size()-1;l>=0; l--)
					{
						pPOpp = (CPOpp*)m_poaPOpp->at(l);
						if (pPOpp->m_PlrName == OldName && pPOpp->m_PlaneName == m_pCurPlane->PlaneName())
						{
							pPOpp->m_PlrName = m_RenameDlg.m_strName;
						}
					}
				}
				for (l=m_poaWOpp->size()-1;l>=0; l--)
				{
					pWOpp = (CWOpp*)m_poaWOpp->at(l);
					if (pWOpp->m_PlrName == OldName && pWOpp->m_WingName == m_pCurWing->WingName())
					{
						pWOpp->m_PlrName = m_RenameDlg.m_strName;
					}
				}
			}
			//put the Wplr at its new place in alphabetical order
			//so find the current polar's index
			for (k=0; k<m_poaWPolar->size(); k++)
				if(m_pCurWPolar==m_poaWPolar->at(k)) break;

			if(k<m_poaWPolar->size())//you never know
			{

				//remove the WPolar
				m_poaWPolar->removeAt(k);
				//And re-insert it
				bool bInserted = false;
				for (k=0; k<m_poaWPolar->size(); k++)
				{
					pWPolar = (CWPolar*)m_poaWPolar->at(k);
					if (m_pCurWPolar->m_PlrName.compare(pWPolar->m_PlrName, Qt::CaseInsensitive)<0 &&
						pWPolar->m_UFOName == m_pCurWing->WingName())
					{
						m_poaWPolar->insert(k, m_pCurWPolar);
						bInserted = true;
						break;
					}
				}
				if(!bInserted)
				{
					m_poaWPolar->append(m_pCurWPolar);
				}
			}

			pMainFrame->SetSaveState(false);
		}
		else if(resp ==10)
		{
			//user wants to overwrite
			if (OldName == m_RenameDlg.m_strName) return;
			for (k=0; k<m_poaWPolar->size(); k++)
			{
				pWPolar = (CWPolar*)m_poaWPolar->at(k);
				if (pWPolar->m_PlrName == m_RenameDlg.m_strName &&	pWPolar->m_UFOName == m_pCurWing->WingName())
				{
					bExists = true;
					break;
				}
			}
			for (l=m_poaWOpp->size()-1;l>=0; l--)
			{
				pWOpp = (CWOpp*)m_poaWOpp->at(l);
				if (pWOpp->m_PlrName == pWPolar->m_PlrName && pWOpp->m_WingName == m_pCurWing->WingName())
				{
					m_poaWOpp->removeAt(l);
					if(pWOpp==m_pCurWOpp)
					{
						m_pCurPOpp = NULL;
						m_pCurWOpp = NULL;
					}
					delete pWOpp;
				}
			}
			m_poaWPolar->removeAt(k);
			if(pWPolar==m_pCurWPolar)
			{
				m_pCurWPolar = NULL;
			}
			delete pWPolar;

			//and rename everything
			m_pCurWPolar->m_PlrName = m_RenameDlg.m_strName;

			for (l=m_poaWOpp->size()-1;l>=0; l--)
			{
				pWOpp = (CWOpp*)m_poaWOpp->at(l);
				if (pWOpp->m_PlrName == OldName &&	pWOpp->m_WingName == m_pCurWing->WingName())
				{
					pWOpp->m_PlrName = m_RenameDlg.m_strName;
				}
			}

			bExists = false;
			pMainFrame->SetSaveState(false);
		}
		else
		{
			return ;//cancelled
		}
	}
//	SetWPlr();
	pMainFrame->UpdateWPolars();
	UpdateView();
}



void QMiarex::OnRenameCurUFO()
{
	//Rename the currently selected UFO
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(!m_pCurWing)	return;
	if(m_pCurPlane)      RenameUFO(m_pCurPlane->PlaneName());
	else if (m_pCurWing) RenameUFO(m_pCurWing->WingName());
	pMainFrame->UpdateUFOs();
	m_bResetglLegend = true;
	UpdateView();
}


void QMiarex::OnResetCpSection()
{
	for(int i=m_CpGraph.GetCurveCount()-1; i>3 ;i--)	m_CpGraph.DeleteCurve(i);
//	m_CurSpanPos = m_SpanPos;
	CreateCpCurves();
	UpdateView();
}


void QMiarex::OnResetCurWPolar()
{
	if (!m_pCurWPolar) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong = tr("Are you sure you want to reset the content of the polar :\n")+  m_pCurWPolar->m_PlrName +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(window(), tr("Question"), strong,
												  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
												  QMessageBox::Cancel)) return;

	m_pCurWPolar->ResetWPlr();
	CPOpp *pPOpp;
	CWOpp *pWOpp;
	if(m_pCurWing)
	{
		for(int i=m_poaWOpp->size()-1; i>=0; --i)
		{
			pWOpp = (CWOpp*) m_poaWOpp->at(i);
			if(pWOpp->m_PlrName==m_pCurWPolar->m_PlrName && pWOpp->m_WingName==m_pCurWing->WingName())
			{
				m_poaWOpp->removeAt(i);
				delete pWOpp;
			}
		}
	}
	if(m_pCurPlane)
	{
		for(int i=m_poaPOpp->size()-1; i>=0; --i)
		{
			pPOpp = (CPOpp*) m_poaPOpp->at(i);
			if(pPOpp->m_PlrName==m_pCurWPolar->m_PlrName && pPOpp->m_PlaneName==m_pCurPlane->PlaneName())
			{
				m_poaPOpp->removeAt(i);
				delete pPOpp;
			}
		}
	}
	pMainFrame->UpdateWOpps();
	m_pCurWOpp = NULL;
	m_pCurPOpp = NULL;

	if(m_iView==WPOLARVIEW)
	{
		CreateWPolarCurves();
		if(m_pCurWPolar)
		{
			QString PolarProps;
			m_pCurWPolar->GetPolarProperties(PolarProps);
			m_pctrlPolarProps1->setText(PolarProps);
		}
	}
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();
	else if(m_iView==WOPPVIEW)  CreateWOppCurves();
	else if(m_iView==WCPVIEW)   CreateCpCurves();


	pMainFrame->SetSaveState(false);
	UpdateView();
}


void QMiarex::OnResetWingGraphScale()
{
	//resets the scale of the current graph
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
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
		double halfspan = m_pCurWing->m_PlanformSpan/2.0;
		if(m_bHalfWing) m_pCurGraph->SetXMin(0.0);
		else            m_pCurGraph->SetXMin(-halfspan*pMainFrame->m_mtoUnit);
		m_pCurGraph->SetXMax( halfspan*pMainFrame->m_mtoUnit);
	}
	UpdateView();
}


void QMiarex::OnResetWingScale()
{
	m_bIs2DScaleSet = false;
	Set2DScale();
	UpdateView();
}


void QMiarex::OnResetWOppLegend()
{
	SetWingLegendPos();
	UpdateView();
}


void QMiarex::OnResetWPlrLegend()
{
	SetWPlrLegendPos();
	UpdateView();
}


void QMiarex::OnScaleWing()
{
	if(!m_pCurWing) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	m_WingScaleDlg.move(pMainFrame->m_DlgPos);
	m_WingScaleDlg.InitDialog(m_pCurWing->m_PlanformSpan, m_pCurWing->m_TChord[0], m_pCurWing->AverageSweep(), m_pCurWing->m_TTwist[m_pCurWing->m_NPanel]);

	if(QDialog::Accepted == m_WingScaleDlg.exec())
	{
		if (m_WingScaleDlg.m_bSpan || m_WingScaleDlg.m_bChord || m_WingScaleDlg.m_bSweep || m_WingScaleDlg.m_bTwist)
		{
			if(m_pCurPlane)
			{
				CPlane *pNewPlane = new CPlane; 
				pNewPlane->Duplicate(m_pCurPlane);
				if(m_WingScaleDlg.m_bSpan)  pNewPlane->Wing()->ScaleSpan(m_WingScaleDlg.m_NewSpan);
				if(m_WingScaleDlg.m_bChord) pNewPlane->Wing()->ScaleChord(m_WingScaleDlg.m_NewChord);
				if(m_WingScaleDlg.m_bSweep) pNewPlane->Wing()->SetSweep(m_WingScaleDlg.m_NewSweep);
				if(m_WingScaleDlg.m_bTwist) pNewPlane->Wing()->SetTwist(m_WingScaleDlg.m_NewTwist);
				pNewPlane->ComputePlane();
				if(SetModPlane(pNewPlane))
				{
					m_pCurPlane = AddPlane(pNewPlane);
					m_pCurWPolar = NULL;
					m_pCurPOpp = NULL;
					m_pCurWOpp = NULL;
				}
				else
					delete pNewPlane;
			}
			else
			{
				CWing* pNewWing = new CWing;
				pNewWing->Duplicate(m_pCurWing);
				if(m_WingScaleDlg.m_bSpan)  pNewWing->ScaleSpan(m_WingScaleDlg.m_NewSpan);
				if(m_WingScaleDlg.m_bChord) pNewWing->ScaleChord(m_WingScaleDlg.m_NewChord);
				if(m_WingScaleDlg.m_bSweep) pNewWing->SetSweep(m_WingScaleDlg.m_NewSweep);
				if(m_WingScaleDlg.m_bTwist) pNewWing->SetTwist(m_WingScaleDlg.m_NewTwist);
				if(AddWing(pNewWing))
				{
					m_pCurWing = pNewWing;
					m_pCurWPolar = NULL;
					m_pCurPOpp = NULL;
					m_pCurWOpp = NULL;
				}
				else
					delete pNewWing;
			}
			SetUFO();
			pMainFrame->UpdateUFOs();
		}
		if(m_iView==WPOLARVIEW)		CreateWPolarCurves();
		else if(m_iView==WSTABVIEW)	CreateStabilityCurves();
		else if(m_iView==WOPPVIEW)	CreateWOppCurves();
		else if(m_iView==WCPVIEW)	CreateCpCurves();
		UpdateView();
	}
	pMainFrame->m_DlgPos = m_WingScaleDlg.pos();
}


void QMiarex::OnSequence()
{
	m_bSequence = m_pctrlSequence->isChecked();
	m_pctrlAlphaMax->setEnabled(m_bSequence);
	m_pctrlAlphaDelta->setEnabled(m_bSequence);
}


void QMiarex::OnShowUFOWOpps()
{
	//Switch all WOpps view to off for the current UFO
	CPOpp *pPOpp;
	CWOpp *pWOpp;
	int i;
	for (i=0; i< m_poaWOpp->size(); i++)
	{
		pWOpp = (CWOpp*)m_poaWOpp->at(i);
		if (pWOpp->m_WingName == m_pCurWPolar->m_UFOName)
		{
			pWOpp->m_bIsVisible = true;
		}
	}
	for (i=0; i< m_poaPOpp->size(); i++)
	{
		pPOpp = (CPOpp*)m_poaPOpp->at(i);
		if (pPOpp->m_PlaneName == m_pCurWPolar->m_UFOName)
		{
			pPOpp->m_bIsVisible = true;
		}
	}

	if (m_iView==WOPPVIEW)      CreateWOppCurves();
	else if(m_iView==WCPVIEW)   CreateCpCurves();
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);
	SetCurveParams();
	UpdateView();
}



void QMiarex::OnShowAllWOpps()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	int i;
	//Switch all WOpps view to on for all UFO and WPolar
	m_bCurWOppOnly = false;
	pMainFrame->showCurWOppOnly->setChecked(false);

	CPOpp *pPOpp;
	CWOpp *pWOpp;
	for (i=0; i< m_poaWOpp->size(); i++)
	{
		pWOpp = (CWOpp*)m_poaWOpp->at(i);
		pWOpp->m_bIsVisible = true;
	}
	for (i=0; i< m_poaPOpp->size(); i++)
	{
		pPOpp = (CPOpp*)m_poaPOpp->at(i);
		pPOpp->m_bIsVisible = true;
	}

	pMainFrame->SetSaveState(false);
	SetCurveParams();

	if (m_iView==WOPPVIEW)      CreateWOppCurves();
	else if(m_iView==WCPVIEW)   CreateCpCurves();
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();

	UpdateView();
}



void QMiarex::OnShowAllWPolars()
{
	int i;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	CWPolar *pWPolar;
	for (i=0; i<m_poaWPolar->size(); i++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(i);
		pWPolar->m_bIsVisible = true;
	}
	if(m_iView==WPOLARVIEW)		CreateWPolarCurves();
	else if(m_iView==WSTABVIEW)	CreateStabilityCurves();

	pMainFrame->SetSaveState(false);
	SetCurveParams();
	UpdateView();
}


void QMiarex::OnShowAllWPlrOpps()
{
	int i;
	//Switch all WOpps view to on for the current UFO and WPolar
	m_bCurWOppOnly = false;

	CPOpp *pPOpp;
	CWOpp *pWOpp;
	if(m_pCurPlane)
	{
		for (i=0; i< m_poaPOpp->size(); i++)
		{
			pPOpp = (CPOpp*)m_poaPOpp->at(i);
			if (pPOpp->m_PlaneName == m_pCurWPolar->m_UFOName &&
				pPOpp->m_PlrName   == m_pCurWPolar->m_PlrName)
			{
				pPOpp->m_bIsVisible = true;
			}
		}
	}
	else if (m_pCurWing)
	{
		for (i=0; i< m_poaWOpp->size(); i++)
		{
			pWOpp = (CWOpp*)m_poaWOpp->at(i);
			if (pWOpp->m_WingName == m_pCurWPolar->m_UFOName &&
				pWOpp->m_PlrName  == m_pCurWPolar->m_PlrName)
			{
				pWOpp->m_bIsVisible = true;
			}
		}
	}
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->SetSaveState(false);
	SetCurveParams();

	if (m_iView==WOPPVIEW)      CreateWOppCurves();
	else if(m_iView==WCPVIEW)   CreateCpCurves();
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();

	UpdateView();
}


void QMiarex::OnShowEllipticCurve()
{
	m_bShowElliptic = !m_bShowElliptic;
//	CheckMenus();

	CreateWOppCurves();
	UpdateView();
}



void QMiarex::OnShowXCmRef()
{
	//Show the Moment reference point
	m_bXCmRef = !m_bXCmRef;

	UpdateView();
}


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


void QMiarex::OnShowLift()
{
	m_bXCP	 = m_pctrlLift->isChecked();
	if(m_iView==WOPPVIEW || m_iView == W3DVIEW)
	{
		if(!m_bAnimateWOpp) UpdateView();
	}
}

void QMiarex::OnShowIDrag()
{
	m_bICd = m_pctrlIDrag->isChecked();
	m_bResetglDrag = true;
	if(m_iView==WOPPVIEW || m_iView == W3DVIEW)
	{
		if(!m_bAnimateWOpp) UpdateView();
	}
}


void QMiarex::OnShowVDrag()
{
	m_bVCd = m_pctrlVDrag->isChecked();
	m_bResetglDrag = true;
	if(m_iView==WOPPVIEW || m_iView == W3DVIEW)
	{
		if(!m_bAnimateWOpp) UpdateView();
	}
}


void QMiarex::OnShowTransitions()
{
	m_bXTop = m_pctrlTrans->isChecked();
	m_bXBot = m_pctrlTrans->isChecked();
	if(m_iView==WOPPVIEW || m_iView == W3DVIEW)
	{
		if(!m_bAnimateWOpp) UpdateView();
	}
}


void QMiarex::OnShowCurve()
{
	m_bCurveVisible = m_pctrlShowCurve->isChecked();
	m_bCurvePoints  = m_pctrlShowPoints->isChecked();
	UpdateCurve();
}





void QMiarex::OnShowUFOWPolars()
{
	if(!m_pCurWing) return;
	int i;
	QString UFOName;
	if(m_pCurPlane)     UFOName = m_pCurPlane->PlaneName();
	else if(m_pCurWing) UFOName = m_pCurWing->WingName();
	else return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	CWPolar *pWPolar;
	for (i=0; i<m_poaWPolar->size(); i++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(i);
		if (pWPolar->m_UFOName == UFOName) pWPolar->m_bIsVisible = true;
	}

	if(m_iView==WPOLARVIEW)		CreateWPolarCurves();
	else if(m_iView==WSTABVIEW)	CreateStabilityCurves();

	SetCurveParams();
	pMainFrame->SetSaveState(false);
	UpdateView();
}



void QMiarex::OnSingleGraph1()
{
	if(m_iView==WSTABVIEW)
	{
		m_iStabilityView = STABTIMEVIEW;
		m_iStabTimeView = 1;
		m_pCurGraph = m_TimeGraph;
		m_pCurTimeGraph= m_pCurGraph;
		SetWingLegendPos();
		UpdateView();
	}
	else if(m_iView==WOPPVIEW)
	{
		m_iWingView = 1;
		m_pCurGraph = m_WingGraph;
		m_pCurWingGraph= m_pCurGraph;
		SetWingLegendPos();
		UpdateView();
	}
	else if(m_iView==WPOLARVIEW)
	{
		m_iWPlrView = 1;
		m_pCurGraph = m_WPlrGraph;
		m_pCurWPlrGraph = m_pCurGraph;
		SetWPlrLegendPos();
		UpdateView();
	}
	else return;
	SetControls();
}



void QMiarex::OnSingleGraph2()
{
	if(m_iView==WSTABVIEW)
	{
		m_iStabilityView = STABTIMEVIEW;
		m_iStabTimeView = 1;
		m_pCurGraph = m_TimeGraph+1;
		m_pCurTimeGraph= m_pCurGraph;
		SetWingLegendPos();
		UpdateView();
	}
	else if(m_iView==WOPPVIEW)
	{
		m_iWingView = 1;
		m_pCurGraph = m_WingGraph+1;
		m_pCurWingGraph= m_pCurGraph;
		SetWingLegendPos();
		UpdateView();
	}
	else if(m_iView==WPOLARVIEW)
	{
		m_iWPlrView = 1;
		m_pCurGraph = m_WPlrGraph+1;
		m_pCurWPlrGraph = m_pCurGraph;
		SetWPlrLegendPos();
		UpdateView();
	}
	else return;
	SetControls();
}


void QMiarex::OnSingleGraph3()
{
	if(m_iView==WSTABVIEW)
	{
		m_iStabilityView = STABTIMEVIEW;
		m_iStabTimeView = 1;
		m_pCurGraph = m_TimeGraph+2;
		m_pCurTimeGraph= m_pCurGraph;
		SetWingLegendPos();
	}
	else if(m_iView==WOPPVIEW)
	{
		m_iWingView = 1;
		m_pCurGraph = m_WingGraph+2;
		m_pCurWingGraph= m_pCurGraph;
		SetWingLegendPos();
	}
	else if(m_iView==WPOLARVIEW)
	{
		m_iWPlrView = 1;
		m_pCurGraph = m_WPlrGraph+2;
		m_pCurWPlrGraph = m_pCurGraph;
		SetWPlrLegendPos();
	}
	else return;
	UpdateView();
	SetControls();
}


void QMiarex::OnSingleGraph4()
{
	if(m_iView==WSTABVIEW)
	{
		m_iStabilityView = STABTIMEVIEW;
		m_iStabTimeView = 1;
		m_pCurGraph = m_TimeGraph+3;
		m_pCurTimeGraph= m_pCurGraph;
		SetWingLegendPos();
	}
	else if(m_iView==WOPPVIEW)
	{
		m_iWingView = 1;
		m_pCurGraph = m_WingGraph+3;
		m_pCurWingGraph= m_pCurGraph;
		SetWingLegendPos();
	}
	else if(m_iView==WPOLARVIEW)
	{
		m_iWPlrView = 1;
		m_pCurGraph = m_WPlrGraph+3;
		m_pCurWPlrGraph = m_pCurGraph;
		SetWPlrLegendPos();
	}
	else return;
	UpdateView();
	SetControls();
}


void QMiarex::OnStabCurve()
{
	m_bShowWingCurve[2] = !m_bShowWingCurve[2];
//	CheckMenus();

	if (m_iView==WOPPVIEW)    CreateWOppCurves();
	else if(m_iView==WCPVIEW) CreateCpCurves();

	UpdateView();
}


void QMiarex::OnStabilityDirection()
{
	//the user has clicked either the longitudinal or lateral mode display
	//so update the view accordingly
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
	
	m_bLongitudinal = pStabView->m_pctrlLongDynamics->isChecked();

	for(int ig=0; ig<4; ig++) m_TimeGraph[ig].DeleteCurves();

	pStabView->m_pCurve = NULL;
	pStabView->FillCurveList();
	
	if(m_bLongitudinal) m_pCurRLStabGraph = &m_LongRLGraph;
	else                m_pCurRLStabGraph = &m_LatRLGraph;
	if(m_iStabilityView==STABPOLARVIEW)
	{
		m_pCurGraph = m_pCurRLStabGraph;
	}

	pStabView->SetMode();
	pStabView->SetControls();
	SetStabGraphTitles();
	SetWPlrLegendPos();
	CreateStabilityCurves();
	SetCurveParams();

	SetControls();
	UpdateView();
}




void QMiarex::OnTimeView()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
	StopAnimate();
	m_iView = WSTABVIEW;
	m_iStabilityView = STABTIMEVIEW;
	pStabView->SetControls();
	SetWingLegendPos();
	CreateStabilityCurves();
	
	pMainFrame->SetCentralWidget();
	SetCurveParams();
	SetControls();
	UpdateView();
}


void QMiarex::OnRootLocusView()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StopAnimate();
	m_iView = WSTABVIEW;
	m_iStabilityView = STABPOLARVIEW;
	SetWPlrLegendPos();
	CreateStabilityCurves();
	pMainFrame->SetCentralWidget();
	SetCurveParams();
	SetControls();
	UpdateView();
}


void QMiarex::OnModalView()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	m_iView = WSTABVIEW;
	m_iStabilityView = STAB3DVIEW;

	pMainFrame->SetCentralWidget();
	SetCurveParams();
	SetControls();
	UpdateView();
}



void QMiarex::OnStreamlines()
{
	m_bStream = m_pctrlStream->isChecked();
	if(m_pctrlStream->isChecked())
	{
//		m_bResetglStream = true;
	}
	if(m_iView==W3DVIEW) UpdateView();
}

void QMiarex::OnSurfaces()
{
	m_bSurfaces = m_pctrlSurfaces->isChecked();
	if(m_bSurfaces)
	{
		m_b3DCp = false;
		m_pctrlCp->setChecked(false);
	}
	UpdateView();
}



void QMiarex::OnSurfaceSpeeds()
{
	m_bSpeeds = m_pctrlSurfVel->isChecked();
	if(m_pctrlSurfVel->isChecked())
	{
//		m_bResetglStream = true;
	}
	if(m_iView==W3DVIEW) UpdateView();
}


void QMiarex::OnSetupLight()
{
	if(m_iView!=W3DVIEW && m_iView!=WSTABVIEW) return;
	m_bShowLight = true;
	UpdateView();
	m_GLLightDlg.m_bLight = m_bglLight;
	m_GLLightDlg.m_pMiarex = this;
	m_GLLightDlg.exec();

	m_bglLight = m_GLLightDlg.m_bLight;

	m_bShowLight = false;

	GLSetupLight();
	UpdateView();
}



void QMiarex::OnStoreWOpp()
{
	m_bStoreWOpp = m_pctrlStoreWOpp->isChecked();
}




void QMiarex::OnTwoGraphs()
{
	if(m_iView==WOPPVIEW)
	{
		m_iWingView = 2;
		m_pCurGraph = m_WingGraph;
		m_pCurWingGraph = m_pCurGraph;

		SetWingLegendPos();
	}
	else if(m_iView==WPOLARVIEW)
	{
		m_iWPlrView = 2;
		m_pCurGraph = m_WPlrGraph;
		m_pCurWPlrGraph = m_pCurGraph;
		SetWPlrLegendPos();
	}
	else if(m_iView==WSTABVIEW)
	{
		m_iStabTimeView= 2;
		m_pCurGraph = m_TimeGraph;
		m_pCurTimeGraph = m_pCurGraph;

		SetWingLegendPos();
	}
	else return;
	UpdateView();
	SetControls();
}


void QMiarex::OnFourGraphs()
{

	if(m_iView==WOPPVIEW)
	{
		m_iWingView = 4;
		m_pCurGraph = m_WingGraph;
		m_pCurWingGraph = m_pCurGraph;
		SetWingLegendPos();
	}
	else if(m_iView==WPOLARVIEW)
	{
		m_iWPlrView = 4;
		m_pCurGraph = m_WPlrGraph;
		m_pCurWPlrGraph = m_pCurGraph;
		SetWPlrLegendPos();
	}
	else if(m_iView==WSTABVIEW)
	{
		m_iStabTimeView= 4;
		m_pCurGraph = m_TimeGraph;
		m_pCurTimeGraph = m_pCurGraph;
		SetWingLegendPos();
	}
	else return;
	UpdateView();
	SetControls();
}



void QMiarex::OnVelocityScale(int pos)
{
	m_VelocityScale    = pos/100.0/sqrt(1.01-pos/100.0);
	m_bResetglDownwash = true;
	UpdateView();
}


void QMiarex::OnVortices()
{
	m_bVortices = m_pctrlVortices->isChecked();
	UpdateView();
}


void QMiarex::OnWing2Curve()
{
	m_bShowWingCurve[1] = !m_bShowWingCurve[1];
//	CheckMenus();

	if (m_iView==WOPPVIEW)    CreateWOppCurves();
	else if(m_iView==WCPVIEW) CreateCpCurves();

	UpdateView();
}



void QMiarex::OnUFOInertia()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(!m_pCurWing) return;

	m_InertiaDlg.move(pMainFrame->m_DlgPos);
	m_InertiaDlg.m_pPlane = NULL;
	m_InertiaDlg.m_pWing  = NULL;
	m_InertiaDlg.m_pBody  = NULL;

	CPlane *pSavePlane = new CPlane;
	CWing *pSaveWing = new CWing;
	CWPolar *pWPolar;
	QString UFOName;
	bool bHasResults = false;

	if(m_pCurPlane)
	{
		UFOName = m_pCurPlane->PlaneName();
		pSavePlane->Duplicate(m_pCurPlane);
		m_InertiaDlg.m_pPlane = m_pCurPlane;
	}
	else if(m_pCurWing)
	{
		UFOName = m_pCurWing->WingName();
		pSaveWing->Duplicate(m_pCurWing);
		m_InertiaDlg.m_pWing  = m_pCurWing;
	}

	for (int i=0; i< m_poaWPolar->size(); i++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(i);
		if(pWPolar->m_Alpha.size() && pWPolar->m_UFOName==UFOName)
		{
//			if(pWPolar->m_Type==STABILITYPOLAR)
//			{
				bHasResults = true;
				break;
//			}
		}
	}

	m_InertiaDlg.InitDialog();

	if(m_InertiaDlg.exec()==QDialog::Accepted)
	{
		if(bHasResults)
		{
			m_ModDlg.m_Question = tr("The modification will erase all polar results associated to this Plane.\nContinue ?");
			m_ModDlg.InitDialog();
			int Ans = m_ModDlg.exec();

			if (Ans == QDialog::Rejected)
			{
				//restore saved UFO
				if(m_pCurPlane)     m_pCurPlane->Duplicate(pSavePlane);
				else if(m_pCurWing) m_pCurWing->Duplicate(pSaveWing);
				return;
			}
			else if(Ans==20)
			{
				if(m_pCurPlane)
				{
					//save mods to a new plane object
					CPlane* pNewPlane= new CPlane;
					pNewPlane->Duplicate(m_pCurPlane);
					//restore geometry for initial plane
					m_pCurPlane->Duplicate(pSavePlane);

					if(!SetModPlane(pNewPlane)) delete pNewPlane;
					else                        m_pCurPlane = AddPlane(pNewPlane);
				}
				else if(m_pCurWing)
				{
					//save mods to a new wing object
					CWing* pNewWing= new CWing;
					pNewWing->Duplicate(m_pCurWing);
					//restore geometry for initial plane
					m_pCurWing->Duplicate(pSaveWing);

					if(!SetModWing(pNewWing)) delete pNewWing;
					else                      m_pCurWing = AddWing(pNewWing);

				}
				SetUFO();
				pMainFrame->UpdateUFOs();
				UpdateView();
				return;
			}

			//last case, user wants to overwrite, so reset all type 7 polars and WOpps and POpps associated to the UFO
			for (int i=0; i<m_poaWPolar->size(); i++)
			{
				pWPolar = (CWPolar*)m_poaWPolar->at(i);
				if(pWPolar && pWPolar->m_UFOName==UFOName && pWPolar->m_bAutoInertia)
				{
					pWPolar->ResetWPlr();
					if(m_pCurPlane)     pWPolar->SetInertia(m_pCurPlane, true);
					else if(m_pCurWing) pWPolar->SetInertia(m_pCurWing, false);
				}
			}
			for (int i=m_poaWOpp->size()-1; i>=0; i--)
			{
				CWOpp *pWOpp = (CWOpp*)m_poaWOpp->at(i);
				if(pWOpp && pWOpp->m_WingName==UFOName)
				{
					m_poaWOpp->removeAt(i);
					delete pWOpp;
				}
			}
			for (int i=m_poaPOpp->size()-1; i>=0; i--)
			{
				CPOpp *pPOpp = (CPOpp*)m_poaPOpp->at(i);
				if(pPOpp && pPOpp->m_PlaneName==UFOName)
				{
					m_poaPOpp->removeAt(i);
					delete pPOpp;
				}
			}
			m_pCurWOpp = NULL;
			m_pCurPOpp = NULL;
		}
		SetWPlr();
		pMainFrame->SetSaveState(false);
		if(m_iView==WPOLARVIEW) CreateWPolarCurves();
		UpdateView();
	}
	else
	{
		//restore saved UFO
		if(m_pCurPlane)    m_pCurPlane->Duplicate(pSavePlane);
		else if(m_pCurWing) m_pCurWing->Duplicate(pSaveWing);
	}
	delete pSavePlane;
	delete pSaveWing;
	pMainFrame->m_DlgPos = m_InertiaDlg.pos();
}



void QMiarex::OnWOpps()
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



void QMiarex::OnWPolars()
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
	CreateWPolarCurves();
	SetCurveParams();
	SetControls();

	UpdateView();
}



void QMiarex::PaintView(QPainter &painter)
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
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
	painter.fillRect(m_r2DCltRect, pMainFrame->m_BackgroundColor);

	if(m_r2DCltRect.width()<200 || m_r2DCltRect.height()<200)
	{
		painter.restore();
		return;//too small to paint
	}

	if (m_iView==WPOLARVIEW)
	{
		if (m_iWPlrView == 1)
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
		else if(m_iWPlrView == 2)
		{
//			PaintWCoupleGraphs(painter);
			Rect1.setRect(0,0,w2,h23);
			Rect2.setRect(w2,0,w2,h23);
	
			m_WPlrGraph[0].DrawGraph(Rect1, painter);
			m_WPlrGraph[1].DrawGraph(Rect2, painter);
	
			DrawWPolarLegend(painter, m_WPlrLegendOffset, m_r2DCltRect.bottom());
		}
		else if(m_iWPlrView == 4)
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
		if (m_iWingView == 1 && m_pCurWing)  
		{
//			PaintSingleWingGraph(painter);
			if (m_pCurWingGraph && m_pCurWing && m_r2DCltRect.width()/2>200 && m_r2DCltRect.height()>250)
			{
				QPoint Place(m_r2DCltRect.left()+10, m_r2DCltRect.top() +30);
				DrawWOppLegend(painter, Place, m_r2DCltRect.bottom());
				m_pCurWingGraph->DrawGraph(m_rSingleRect, painter);
			}
		
			painter.setFont(pMainFrame->m_TextFont);
		
			QPen TextPen(pMainFrame->m_TextColor);
			TextPen.setWidth(1);
			painter.setPen(TextPen);
		
			int dwidth, dheight;
			QFontMetrics fm(pMainFrame->m_TextFont);
			dheight = fm.height();
			dwidth = fm.width(tr("abcdefghijklmnopqrstuvwxyz012345678"));
		
			if(m_pCurWing)
			{
				PaintWing(painter, m_ptOffset, m_WingScale);
				if(m_pCurWOpp && m_pCurWOpp->m_bIsVisible)
				{
					QPoint PtLegend;
					PtLegend.rx() = (int)(m_r2DCltRect.width()/2);
					PtLegend.ry() = m_r2DCltRect.bottom();
					PaintXTr(painter, m_ptOffset, m_WingScale);
					if (m_bXCP)    PaintXCP(painter, m_ptOffset, m_WingScale);
					if (m_bXCmRef) PaintXCmRef(painter, m_ptOffset, m_WingScale);
				}
				PaintWingLegend(painter);
				if(m_pCurWOpp) PaintCurWOppLegend(painter);
			}
		}
		else if (m_iWingView == 2 && m_pCurWing) 
		{
//			PaintTwoWingGraph(painter);
			DrawWOppLegend(painter, m_WingLegendOffset, m_r2DCltRect.bottom());
	
			Rect1.setRect(m_r2DCltRect.left(),   m_r2DCltRect.top(), w2, h23);
			Rect2.setRect(w2, m_r2DCltRect.top(),                    w2, h23);
			m_WingGraph[0].DrawGraph(Rect1, painter);
			m_WingGraph[1].DrawGraph(Rect2, painter);
		}
		else if (m_iWingView == 4 && m_pCurWing) 
		{
//			PaintFourWingGraph(painter);
			DrawWOppLegend(painter, m_WingLegendOffset, m_r2DCltRect.bottom());
		
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
//		PaintCp(painter);
		m_CpGraph.DrawGraph(painter);
		QPoint Place(50, h34+20);
		DrawCpLegend(painter, Place, m_r2DCltRect.bottom());
	}
	else if (m_iView==WSTABVIEW)
	{
		if(m_iStabilityView==STABTIMEVIEW)
		{
			if(m_iStabTimeView==1) 
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
			else if(m_iStabTimeView==2)
			{
				//Paint a two graph time view
				DrawStabTimeLegend(painter, m_WingLegendOffset, m_r2DCltRect.bottom());
				Rect1.setRect(m_r2DCltRect.left(),   m_r2DCltRect.top(), w2, h23);
				Rect2.setRect(w2, m_r2DCltRect.top(),                    w2, h23);
				m_TimeGraph[0].DrawGraph(Rect1, painter);
				m_TimeGraph[1].DrawGraph(Rect2, painter);
			}
			else if(m_iStabTimeView==4)
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
		else 
		{
//			PaintRLStabGraphs(painter);
			if(m_iStabilityView==STABPOLARVIEW)
			{
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
		}
	}
	painter.restore();
}


void QMiarex::PaintWing(QPainter &painter, QPoint ORef, double scale)
{
	//Draws the wing on the 2D WOpp view
	if(!m_pCurWing)	return;
	static int i;
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	static double scalex, scaley;

	scalex  = scale;
	scaley  = scale;

	painter.save();
	QPen WingPen(m_OutlineColor);
	WingPen.setStyle(GetStyle(m_OutlineStyle));
	WingPen.setWidth(m_OutlineWidth);

	painter.setPen(WingPen);

	QPoint O(ORef);

	//Right Wing
	O.rx() = ORef.x();
	O.ry() = ORef.y();
	for (i=0; i<m_pCurWing->m_NPanel;i++)
	{
		O.rx() +=(int)(m_pCurWing->m_TLength[i]*scalex);
		painter.drawLine(O.x(),                                           O.y()+(int)(m_pCurWing->m_TOffset[i]*scaley),
						 O.x()+(int)(m_pCurWing->m_TLength[i+1]*scalex),  O.y()+(int)(m_pCurWing->m_TOffset[i+1]*scaley));

		painter.drawLine(O.x()+(int)(m_pCurWing->m_TLength[i+1]*scalex),  O.y()+(int)(m_pCurWing->m_TOffset[i+1]*scaley),
						 O.x()+(int)(m_pCurWing->m_TLength[i+1]*scalex),  O.y()+(int)((m_pCurWing->m_TOffset[i+1]+m_pCurWing->m_TChord[i+1])*scaley));

		painter.drawLine(O.x()+(int)(m_pCurWing->m_TLength[i+1]*scalex),  O.y()+(int)((m_pCurWing->m_TOffset[i+1]+m_pCurWing->m_TChord[i+1])*scaley),
						 O.x(),                                           O.y() +(int)((m_pCurWing->m_TOffset[i]+m_pCurWing->m_TChord[i])*scaley));

		painter.drawLine(O.x(),                                           O.y() +(int)((m_pCurWing->m_TOffset[i]+m_pCurWing->m_TChord[i])*scaley),
						 O.x(),                                           O.y()+(int)(m_pCurWing->m_TOffset[i]*scaley));
	}
	if(!m_bHalfWing)
	{
		//LeftWing
		O.rx() = ORef.x();
		O.ry() = ORef.y();

		for (i=0; i<m_pCurWing->m_NPanel;i++)
		{
			O.rx() -= (int)(m_pCurWing->m_TLength[i]*scalex);
			painter.drawLine(O.x(),						                     O.y()+(int)(m_pCurWing->m_TOffset[i]*scaley),
							 O.x()-(int)(m_pCurWing->m_TLength[i+1]*scalex), O.y()+(int)(m_pCurWing->m_TOffset[i+1]*scaley));

			painter.drawLine(O.x()-(int)(m_pCurWing->m_TLength[i+1]*scalex), O.y()+(int)(m_pCurWing->m_TOffset[i+1]*scaley),
							 O.x()-(int)(m_pCurWing->m_TLength[i+1]*scalex), O.y()+(int)((m_pCurWing->m_TOffset[i+1]+m_pCurWing->m_TChord[i+1])*scaley));

			painter.drawLine(O.x()-(int)(m_pCurWing->m_TLength[i+1]*scalex), O.y()+(int)((m_pCurWing->m_TOffset[i+1]+m_pCurWing->m_TChord[i+1])*scaley),
							 O.x(),                                          O.y() +(int)((m_pCurWing->m_TOffset[i]+m_pCurWing->m_TChord[i])*scaley));

			painter.drawLine(O.x(),                                          O.y() +(int)((m_pCurWing->m_TOffset[i]+m_pCurWing->m_TChord[i])*scaley),
							 O.x(),                                          O.y()+(int)(m_pCurWing->m_TOffset[i]*scaley));
		}
	}

	QPen SymPen(QColor(155,128,190));
	painter.setPen(SymPen);
	painter.setBackgroundMode(Qt::TransparentMode);

	painter.drawLine(ORef.x(), ORef.y()-20, ORef.x(), ORef.y()+75);
	painter.restore();
}


void QMiarex::PaintWingLegend(QPainter &painter)
{
	if(!m_pCurWing) return;
	painter.save();

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString Result, str, strong;
	QString str1;
	static double Mass;
	static int margin, dwidth, dheight;

	margin = 10;

	QFontMetrics fm(pMainFrame->m_TextFont);
	dheight = fm.height();
	dwidth = fm.width(tr("abcdefghijklmnopqrstuvwxyz012345678"));
	int D = 0;
	int LeftPos = margin;
	int ZPos    = m_r2DCltRect.height()-11*dheight;

//	double area = m_pCurWing->s_RefArea;
	if(m_pCurPlane && m_pWingList[2]) ZPos -= dheight;
	if(m_pCurWPolar)                  ZPos -= dheight*2;

	painter.drawText(LeftPos, ZPos, m_pCurWing->WingName());
	D+=dheight;
	QString length, surface;
	GetLengthUnit(length, pMainFrame->m_LengthUnit);
	GetAreaUnit(surface,  pMainFrame->m_AreaUnit);

	str1 = QString(tr("Wing Span =")+"   %1 ").arg(m_pCurWing->m_PlanformSpan*pMainFrame->m_mtoUnit,12,'f',3);
	str1 += length;
	painter.drawText(LeftPos,ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("xyProj. Span =")+" %1 ").arg(m_pCurWing->m_ProjectedSpan*pMainFrame->m_mtoUnit,11,'f',3);
	str1 += length;
	painter.drawText(LeftPos,ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("Wing Area =")+"  %1 ").arg(m_pCurWing->m_PlanformArea * pMainFrame->m_m2toUnit,13,'f',3);
	str1 += surface;
	painter.drawText(LeftPos,ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("xyProj. Area =")+" %1 ").arg(m_pCurWing->m_ProjectedArea * pMainFrame->m_m2toUnit,11,'f',3);
	str1 += surface;
	painter.drawText(LeftPos,ZPos+D, str1);
	D+=dheight;

	if(m_pCurWPolar)
	{
		if(!m_pCurWPolar->m_bAutoInertia)  Mass = m_pCurWPolar->m_Mass;
		else
		{
			if(m_pCurPlane)     Mass = m_pCurPlane->TotalMass();
			else if(m_pCurWing) Mass = m_pCurWing->TotalMass();
		}
		GetWeightUnit(str, pMainFrame->m_WeightUnit);
		Result = QString(tr("Plane Mass =")+"    %1 ").arg(Mass*pMainFrame->m_kgtoUnit,10,'f',3);
		Result += str;
		painter.drawText(LeftPos, ZPos+D, Result);
		D+=dheight;

		GetAreaUnit(strong, pMainFrame->m_AreaUnit);
		Result = QString(tr("Wing Load =")+"  %1 ").arg(Mass*pMainFrame->m_kgtoUnit/m_pCurWPolar->m_WArea/pMainFrame->m_m2toUnit,13,'f',3);
		Result += str + "/" + strong;
		painter.drawText(LeftPos, ZPos+D, Result);
		D+=dheight;
	}

	if(m_pCurPlane && m_pWingList[2])
	{
		str1 = QString(tr("Tail Volume =")+"      %1").arg(m_pCurPlane->TailVolume(),7,'f',3);
		painter.drawText(LeftPos, ZPos+D, str1);
		D+=dheight;
	}

	str1 = QString(tr("Root Chord =")+"   %1 ").arg(m_pCurWing->m_TChord[0]*pMainFrame->m_mtoUnit, 11,'f', 3);
	Result = str1+length;
	painter.drawText(LeftPos, ZPos+D, Result);
	D+=dheight;

	str1 = QString(tr("MAC =")+"          %1 ").arg(m_pCurWing->m_MAChord*pMainFrame->m_mtoUnit, 11,'f', 3);
	Result = str1+length;
	painter.drawText(LeftPos, ZPos+D, Result);
	D+=dheight;

	str1 = QString(tr("TipTwist =")+"   %1 deg").arg(m_pCurWing->m_TTwist[m_pCurWing->m_NPanel], 13,'f', 3);
	painter.drawText(LeftPos, ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("Aspect Ratio =")+"   %1").arg(m_pCurWing->m_AR,9,'f',3);
	painter.drawText(LeftPos, ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("Taper Ratio =")+"   %1").arg(m_pCurWing->m_TR,10,'f',3);
	painter.drawText(LeftPos, ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("Root-Tip Sweep =")+" %1 deg").arg(m_pCurWing->AverageSweep(), 9,'f',3);
	painter.drawText(LeftPos, ZPos+D, str1);

	painter.restore();
}


void QMiarex::PaintCurWOppLegend(QPainter &painter)
{
	if(!m_pCurWOpp) return;
	painter.save();
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString Result, str;

	int i;
	int margin = 10;
	int dwidth, dheight;


	QFontMetrics fm(pMainFrame->m_TextFont);
	dheight = fm.height();
	dwidth = fm.width(tr("abcdefghijklmnopqrstuvwxyz012345678"));
	int D = 0;

	D = 0;
	int RightPos = m_r2DCltRect.right()-margin-fm.width(tr("abcdefghijklmnopqrstuvwxyz01234567"));
	int ZPos	 = m_r2DCltRect.height()-13*dheight;

	if(m_pCurWOpp && m_pCurWOpp->m_Type==STABILITYPOLAR) ZPos -=dheight;
	if(m_pCurWOpp && m_pCurWOpp->m_bOut) ZPos -= dheight;
	if(m_pCurWOpp) ZPos -= dheight*m_pCurWOpp->m_nFlaps;

	if(m_pCurWOpp && m_pCurWOpp->m_bIsVisible)
	{
		GetSpeedUnit(str, pMainFrame->m_SpeedUnit);
		int l = str.length();
		if(l==2)      Result = QString(tr("V = %1 ")).arg(m_pCurWOpp->m_QInf*pMainFrame->m_mstoUnit,7,'f',2);
		else if(l==3) Result = QString(tr("V = %1 ")).arg(m_pCurWOpp->m_QInf*pMainFrame->m_mstoUnit,6,'f',1);
		else if(l==4) Result = QString(tr("V = %1 ")).arg(m_pCurWOpp->m_QInf*pMainFrame->m_mstoUnit,5,'f',1);
		else          Result = tr("No unit defined for speed...");

		Result += str;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString(tr("Alpha = %1 ")).arg(m_pCurWOpp->m_Alpha, 9,'f',4);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		if(m_pCurWOpp->m_bOut)
		{
			Result = tr("Point is out of the flight envelope");
			D+=dheight;
			painter.drawText(RightPos, ZPos+D, Result);
		}

		Result = QString(tr("Lift Coef. = %1 ")).arg(m_pCurWOpp->m_CL, 9,'f',4);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);


		Result = QString(tr("Drag Coef. = %1 ")).arg(m_pCurWOpp->m_VCD+m_pCurWOpp->m_ICD, 9,'f',4);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		/*		oswald=CZ^2/CXi/PI/allongement;*/
		double cxielli=m_pCurWOpp->m_CL*m_pCurWOpp->m_CL/PI/m_pCurWing->m_AR;
		Result = QString(tr("Efficiency = %1 ")).arg(cxielli/m_pCurWOpp->m_ICD, 9,'f',4);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString(tr("Cl/Cd = %1 ")).arg(m_pCurWOpp->m_CL/(m_pCurWOpp->m_ICD+m_pCurWOpp->m_VCD), 9,'f',4);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString(tr("GCm = %1 ")).arg(m_pCurWOpp->m_GCm,9,'f',4);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);


		Result = QString(tr("Rolling Moment Coef. = %1 ")).arg(m_pCurWOpp->m_GRm, 9,'f',4);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);


		Result = QString(tr("Induced Moment Coef = %1 ")).arg(m_pCurWOpp->m_IYm, 9,'f',4);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString(tr("Profile Yawing Moment = %1 ")).arg(m_pCurWOpp->m_GYm, 9,'f',4);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		GetLengthUnit(str, pMainFrame->m_LengthUnit);
		l = str.length();
		int c, d;
		if(l==1) {c=8, d=3;}
		else if(l==2) {c=7, d=3;}
		else {c=6, d=3;}
		if(m_pCurWOpp->m_Type==STABILITYPOLAR)
		{
			Result = QString(QObject::tr("X_NP = %1 ")).arg(m_pCurWOpp->m_XNP*pMainFrame->m_mtoUnit, c,'f',d);
			Result += str;
			D+=dheight;
			painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);
		}

		Result = QString(QObject::tr("X_CP = %1 ")).arg(m_pCurWOpp->m_XCP*pMainFrame->m_mtoUnit, c, 'f', d);
		Result += str;
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString(QObject::tr("X_CG = %1 ")).arg(m_pCurWPolar->m_CoG.x*pMainFrame->m_mtoUnit, c, 'f', d);
		Result += str;
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		for(i=0; i<m_pCurWOpp->m_nFlaps; i++)
		{
			Result = QString(tr("Flap %1 Moment =%2")).arg(i+1).arg(m_pCurWOpp->m_FlapMoment[i]*pMainFrame->m_NmtoUnit,9,'f',4);
			GetMomentUnit(str, pMainFrame->m_MomentUnit);
			Result += str;
			D+=dheight;
			painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);
		}
	}
	painter.restore();
}



void QMiarex::PaintXTr(QPainter & painter, QPoint ORef, double scale)
{
	//Draws the transition lines on the 2D view
	if(!m_pCurWing)	return;
	painter.save();
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	double y;
	int m,nStart;
	if(m_pCurWOpp->m_AnalysisMethod==LLTMETHOD) nStart = 1; else nStart = 0;
	if(m_bHalfWing) nStart = m_pCurWOpp->m_NStation/2;
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

	QPen TopPen(m_XTopColor);
	TopPen.setStyle(GetStyle(m_XTopStyle));
	TopPen.setWidth(m_XTopWidth);
	painter.setPen(TopPen);

	if (m_bXTop)
	{
		offLE = m_pCurWing->Offset(m_pCurWOpp->m_SpanPos[nStart]*2.0/m_pCurWOpp->m_Span);
		y = (offLE+m_pCurWOpp->m_Chord[nStart]*m_pCurWOpp->m_XTrTop[nStart])*scaley;
		From = QPoint(O.x()+(int)(m_pCurWOpp->m_SpanPos[nStart]*scalex),	O.y()+(int)y);

		for (m=nStart; m<m_pCurWOpp->m_NStation; m++)
		{
			offLE = m_pCurWing->Offset(m_pCurWOpp->m_SpanPos[m]*2.0/m_pCurWOpp->m_Span);
			y = (offLE+m_pCurWOpp->m_Chord[m]*m_pCurWOpp->m_XTrTop[m])*scaley;

			To = QPoint(O.x()+(int)(m_pCurWOpp->m_SpanPos[m]*scalex), O.y()+(int)y );
			painter.drawLine(From, To);
			From  = To;
		}


		int x = (int)(m_r2DCltRect.width()/2);
		int y = m_r2DCltRect.bottom();
		painter.drawLine(x-60,  y - 50, x-40,  y - 50);
		painter.drawText(x-35, y - 48, tr("Top transition"));

	}


	QPen BotPen(m_XBotColor);
	BotPen.setStyle(GetStyle(m_XBotStyle));
	BotPen.setWidth(m_XBotWidth);

	painter.setPen(BotPen);
	if (m_bXBot)
	{
		offLE = m_pCurWing->Offset(m_pCurWOpp->m_SpanPos[nStart]*2.0/m_pCurWOpp->m_Span);
		y = (offLE+m_pCurWOpp->m_Chord[nStart]*m_pCurWOpp->m_XTrBot[nStart])*scaley;
		From = QPoint(O.x() +(int)(m_pCurWOpp->m_SpanPos[nStart]*scalex), O.y()+(int)y );
		for (m=nStart; m<m_pCurWOpp->m_NStation; m++)
		{
			offLE = m_pCurWing->Offset(m_pCurWOpp->m_SpanPos[m]*2.0/m_pCurWOpp->m_Span);
			y = (offLE+m_pCurWOpp->m_Chord[m]*m_pCurWOpp->m_XTrBot[m])*scaley;
			To = QPoint(O.x()+(int)(m_pCurWOpp->m_SpanPos[m]*scalex), O.y()+(int)y );
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

void QMiarex::PaintXCP(QPainter & painter, QPoint ORef, double scale)
{
	//Draws the lift line and center of pressure position on the the 2D view
	if(!m_pCurWing)	return;
	painter.save();
//	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QPoint From, To;

	double y;
	int nStart;
	double offLE;
	if(m_pCurWOpp->m_AnalysisMethod==LLTMETHOD) nStart = 1; else nStart = 0;
	if(m_bHalfWing) nStart = m_pCurWOpp->m_NStation/2;

	QPoint O(ORef);
	QPoint offset;

	double scalex, scaley;
	offset.rx() = ORef.x();
	offset.ry() = ORef.y();
	scalex  = scale;
	scaley  = scale;
	O.rx() = offset.x();
	O.ry() = offset.y();

	QPen XCPPen(m_XCPColor);
	XCPPen.setWidth(m_XCPWidth);
	XCPPen.setStyle(GetStyle(m_XCPStyle));
	painter.setPen(XCPPen);

	int XCp = O.x() + (int)(m_pCurWOpp->m_YCP*scalex);
	int YCp = O.y() + (int)(m_pCurWOpp->m_XCP*scaley);
	int size = 3;
	QRect CP(XCp-size, YCp-size, 2*size, 2*size);
	painter.drawEllipse(CP);

	offLE = m_pCurWing->Offset(m_pCurWOpp->m_SpanPos[nStart]*2.0/m_pCurWOpp->m_Span);
	y = (offLE+m_pCurWOpp->m_Chord[nStart]*m_pCurWOpp->m_XCPSpanRel[nStart])*scaley;
	From = QPoint(O.x()+(int)(m_pCurWOpp->m_SpanPos[nStart]*scalex),	O.y()+(int)y );

	for (int m=nStart; m<m_pCurWOpp->m_NStation; m++)
	{
		offLE = m_pCurWing->Offset(m_pCurWOpp->m_SpanPos[m]*2.0/m_pCurWOpp->m_Span);
		y = (offLE+m_pCurWOpp->m_Chord[m]*m_pCurWOpp->m_XCPSpanRel[m])*scaley;
		To = QPoint(O.x()+(int)(m_pCurWOpp->m_SpanPos[m]*scalex),	O.y()+(int)y );
		painter.drawLine(From, To);
		From = To;
	}

	int x = (int)(m_r2DCltRect.width()/2);
	int y1 = m_r2DCltRect.bottom();
	painter.drawLine(x-60,  y1- 20, x-40,  y1 - 20);
	painter.drawText(x-35, y1 - 18, tr("Centre of Pressure"));
	painter.restore();
}


void QMiarex::PaintXCmRef(QPainter & painter, QPoint ORef, double scale)
{
	//Draws the moment reference point on the 2D view
	if(!m_pCurWing || !m_pCurWPolar)	return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	painter.save();
	QPoint O(ORef);
	QPoint offset;

	double scalex, scaley;

	offset.rx() = ORef.x();
	offset.ry() = ORef.y();
	scalex  = scale;
	scaley  = scale;
	O.rx() = offset.x();
	O.ry() = offset.y();

	QPen XCmRefPen(pMainFrame->m_TextColor);
	painter.setPen(XCmRefPen);

	int XCm = O.x() ;
	int YCm = O.y() + (int)(m_pCurWPolar->m_CoG.x*scaley);
	int size = 3;
	QRect CM(XCm-size, YCm-size, 2*size, 2*size);
	painter.drawEllipse(CM);

	painter.drawText(XCm+10, YCm-5, tr("Moment ref. location"));

	painter.restore();
}


void QMiarex::PanelAnalyze(double V0, double VMax, double VDelta, bool bSequence)
{
	if(!m_pCurWing || !m_pCurWPolar) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	int i,pl, pr;

	//Join surfaces together
	pl = 0;
	pr = m_pSurface[0]->m_NElements;
	for (i=0; i<m_NSurfaces-1; i++)
	{
		if(!m_pSurface[i]->m_bIsTipRight)
		{
			if(m_pSurface[i]->m_bJoinRight) JoinSurfaces(m_pSurface[i], m_pSurface[i+1], pl, pr);
		}
		pl  = pr;
		pr += m_pSurface[i+1]->m_NElements;
	}

	m_pPanelDlg->m_bSequence      = bSequence;
	m_pPanelDlg->m_pWPolar        = m_pCurWPolar;
	m_pPanelDlg->m_MaxWakeIter    = m_MaxWakeIter;
	m_pPanelDlg->m_WakeInterNodes = m_WakeInterNodes;
	m_pPanelDlg->m_bSequence      = bSequence;
	m_pPanelDlg->m_nWakeNodes     = m_nWakeNodes;
	m_pPanelDlg->m_WakeSize       = m_WakeSize;
	m_pPanelDlg->m_bTrefftz       = m_bTrefftz;
	m_pPanelDlg->m_nNodes         = m_nNodes;
	m_pPanelDlg->m_NSurfaces      = m_NSurfaces;
	m_pPanelDlg->m_ppSurface      = m_pSurface;
	m_pPanelDlg->m_MatSize        = m_MatSize;
	m_pPanelDlg->m_NWakeColumn    = m_NWakeColumn;
	m_pPanelDlg->m_pPlane         = m_pCurPlane;
	m_pPanelDlg->m_pBody          = m_pCurBody;
	m_pPanelDlg->m_pWing          = m_pCurWing;
	for(int iw=0; iw<4; iw++) m_pPanelDlg->m_pWingList[iw] = m_pWingList[iw];

	if(m_pCurWPolar->m_Type==FIXEDAOAPOLAR)
	{
		m_pPanelDlg->m_Alpha      = m_pCurWPolar->m_ASpec;
		m_pPanelDlg->m_QInf       = V0;
		m_pPanelDlg->m_QInfMax    = VMax;
		m_pPanelDlg->m_QInfDelta  = VDelta;
	}
	else if(m_pCurWPolar->m_Type==STABILITYPOLAR)
	{
		m_pPanelDlg->m_Alpha      = m_pCurWPolar->m_ASpec;

		m_pPanelDlg->m_ControlMin    = V0;
		m_pPanelDlg->m_ControlMax    = VMax;
		m_pPanelDlg->m_ControlDelta  = VDelta;
	}
	else
	{
		m_pPanelDlg->m_QInf       = m_pCurWPolar->m_QInf;
		m_pPanelDlg->m_Alpha      = V0;
		m_pPanelDlg->m_AlphaMax   = VMax;
		m_pPanelDlg->m_AlphaDelta = VDelta;
	}


	m_pPanelDlg->move(pMainFrame->m_DlgPos);

	m_pPanelDlg->InitDialog();
	m_pPanelDlg->show();
	m_pPanelDlg->StartAnalysis();
	m_bResetglMesh = true; //TODO remove

	if(!m_bLogFile || !m_pPanelDlg->m_bWarning) m_pPanelDlg->hide();

	pMainFrame->m_DlgPos = m_pPanelDlg->pos();
	
	pMainFrame->UpdateWOpps();

	if(m_pCurPlane)     SetPOpp(false, m_pPanelDlg->m_Alpha);
	else if(m_pCurWing) SetWOpp(false, m_pPanelDlg->m_Alpha);

	if(m_iView==WPOLARVIEW)		CreateWPolarCurves();
	else if(m_iView==WSTABVIEW)	CreateStabilityCurves();

	UpdateView();

}



QString QMiarex::RenameUFO(QString UFOName)
{
	//Rename the currently selected UFO
	QString OldName;
	CWOpp *pWOpp;
	CPOpp *pPOpp;
	int l;
	CWPolar *pWPolar;
	CPlane *pPlane = GetPlane(UFOName);
	CWing *pWing   = GetWing(UFOName);
	if(pPlane)
	{
		OldName = pPlane->PlaneName();
		SetModPlane(pPlane);

		pPlane->RenameWings();

		for (l=m_poaWPolar->size()-1;l>=0; l--)
		{
			pWPolar = (CWPolar*)m_poaWPolar->at(l);
			if (pWPolar->m_UFOName == OldName)
			{
				pWPolar->m_UFOName = pPlane->PlaneName();
			}
		}
		for (l=m_poaPOpp->size()-1;l>=0; l--)
		{
			pPOpp = (CPOpp*)m_poaPOpp->at(l);
			if (pPOpp->m_PlaneName == OldName)
			{
				pPOpp->m_PlaneName = pPlane->PlaneName();
			}
		}
		return pPlane->PlaneName();
	}
	else if(pWing)
	{
		OldName = pWing->m_WingName;
		SetModWing(pWing);
		for (l=m_poaWPolar->size()-1;l>=0; l--)
		{
			pWPolar = (CWPolar*)m_poaWPolar->at(l);
			if (pWPolar->m_UFOName == OldName){
				pWPolar->m_UFOName = pWing->m_WingName;
			}
		}
		for (l=m_poaWOpp->size()-1;l>=0; l--)
		{
			pWOpp = (CWOpp*)m_poaWOpp->at(l);
			if (pWOpp->m_WingName == OldName)
			{
				pWOpp->m_WingName = pWing->m_WingName;
			}
		}
		return pWing->m_WingName;
	}
	return "";
}


void QMiarex::RotateGeomY(double const &Angle, CVector const &P)
{
	int n, p, pw, kw, lw;

	int iLA, iLB, iTA, iTB;
	CVector LATB, TALB, Pt, Trans;

	for (n=0; n< m_nNodes; n++)
		m_Node[n].RotateY(P, Angle);

	for (p=0; p< m_MatSize; p++)
	{
		iLA = m_Panel[p].m_iLA; iLB = m_Panel[p].m_iLB;
		iTA = m_Panel[p].m_iTA; iTB = m_Panel[p].m_iTB;

		LATB = m_Node[iLA] - m_Node[iTB];
		TALB = m_Node[iTA] - m_Node[iLB];

		if(m_Panel[p].m_iPos==0 || m_Panel[p].m_iPos==1)	m_Panel[p].SetFrame(m_Node[iLA], m_Node[iLB], m_Node[iTA], m_Node[iTB]);
				else if (m_Panel[p].m_iPos==-1)					m_Panel[p].SetFrame(m_Node[iLB], m_Node[iLA], m_Node[iTB], m_Node[iTA]);
	}

	// the wake array is not rotated but translated to remain at the wing's trailing edge
	pw=0;

	if(!m_pCurWPolar) return;
	for (kw=0; kw<m_NWakeColumn; kw++)
	{
		//consider the first panel of the column;
		Pt = m_WakeNode[m_WakePanel[pw].m_iLA];
		Pt.RotateY(P, Angle);
		//define the translation to be applied to the column's left points
		Trans = Pt - m_WakeNode[m_WakePanel[pw].m_iLA];

		//each wake column has m_NXWakePanels ... translate all left A nodes
		for(lw=0; lw<m_pCurWPolar->m_NXWakePanels; lw++)
		{
			if(lw==0) m_WakeNode[m_WakePanel[pw].m_iLA] += Trans;
			m_WakeNode[m_WakePanel[pw].m_iTA] += Trans;
			pw++;
		}
	}

	//do the same for the very last right wake node column
	pw -= m_pCurWPolar->m_NXWakePanels;
	Pt = m_WakeNode[m_WakePanel[pw].m_iLB];
	Pt.RotateY(P, Angle);
	//define the translation to be applied to the column's left points
	Trans = Pt-m_WakeNode[m_WakePanel[pw].m_iLB];

	//each wake column has m_NXWakePanels ... translate all right B nodes
	for(lw=0; lw<m_pCurWPolar->m_NXWakePanels; lw++)
	{
		if(lw==0) m_WakeNode[m_WakePanel[pw].m_iLB] += Trans;
		m_WakeNode[m_WakePanel[pw].m_iTB] += Trans;
		pw++;
	}

	//Reset panel frame : CollPt has been translated
	for (pw=0; pw< m_WakeSize; pw++)
	{
		iLA = m_WakePanel[pw].m_iLA; iLB = m_WakePanel[pw].m_iLB;
		iTA = m_WakePanel[pw].m_iTA; iTB = m_WakePanel[pw].m_iTB;
		m_WakePanel[pw].SetFrame(m_WakeNode[iLA], m_WakeNode[iLB], m_WakeNode[iTA], m_WakeNode[iTB]);
	}
}


void QMiarex::RotateGeomZ(CPanel *pPanel, CVector *pNode, CPanel *pWakePanel, CVector *pWakeNode, double const &Beta, CVector const &P)
{
	int n, p, pw, kw, lw;
//	double cosb = cos(Beta*PI/180.0);
//	double sinb = sin(Beta*PI/180.0);
	int iLA, iLB, iTA, iTB;
	CVector LATB, TALB, Pt, Trans;

	for (n=0; n< m_nNodes; n++)	pNode[n].RotateZ(P, Beta);

	for (p=0; p< m_MatSize; p++)
	{
		iLA = pPanel[p].m_iLA; iLB = pPanel[p].m_iLB;
		iTA = pPanel[p].m_iTA; iTB = pPanel[p].m_iTB;

		LATB = pNode[iLA] - pNode[iTB];
		TALB = pNode[iTA] - pNode[iLB];

		if(pPanel[p].m_iPos>=0)	       pPanel[p].SetFrame(pNode[iLA], pNode[iLB], pNode[iTA], pNode[iTB]);
		else if (pPanel[p].m_iPos==-1) pPanel[p].SetFrame(pNode[iLB], pNode[iLA], pNode[iTB], pNode[iTA]);
	}

	// the wake array is not rotated but translated to remain at the wing's trailing edge
	pw=0;

	if(!m_pCurWPolar) return;
	for (kw=0; kw<m_NWakeColumn; kw++)
	{
		//consider the first panel of the column;
		Pt = pWakeNode[pWakePanel[pw].m_iLA];
		Pt.RotateZ(P, Beta);
		//define the translation to be applied to the column's left points
		Trans = Pt-pWakeNode[pWakePanel[pw].m_iLA] ;

		//each wake column has m_NXWakePanels ... translate all left A nodes
		for(lw=0; lw<m_pCurWPolar->m_NXWakePanels; lw++)
		{
			if(lw==0) pWakeNode[pWakePanel[pw].m_iLA] += Trans;
			pWakeNode[pWakePanel[pw].m_iTA] += Trans;
			pw++;
		}
	}
	pw -= m_pCurWPolar->m_NXWakePanels;
	//consider the first panel of the column;
	Pt = pWakeNode[pWakePanel[pw].m_iLB];
	Pt.RotateZ(P, Beta);
	//define the translation to be applied to the column's left points
	Trans = Pt - pWakeNode[pWakePanel[pw].m_iLB];

	//each wake column has m_NXWakePanels ... translate all left A nodes
	for(lw=0; lw<m_pCurWPolar->m_NXWakePanels; lw++)
	{
		if(lw==0) pWakeNode[pWakePanel[pw].m_iLB] += Trans;
		pWakeNode[pWakePanel[pw].m_iTB] += Trans;
		pw++;
	}

	//Reset panel frame : CollPt has been translated
	for (pw=0; pw< m_WakeSize; pw++)
	{
		iLA = pWakePanel[pw].m_iLA; iLB = pWakePanel[pw].m_iLB;
		iTA = pWakePanel[pw].m_iTA; iTB = pWakePanel[pw].m_iTB;
		pWakePanel[pw].SetFrame(pWakeNode[iLA], pWakeNode[iLB], pWakeNode[iTA], pWakeNode[iTB]);
	}
}


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
		pSettings->setValue("bXBot", m_bXBot  );
		pSettings->setValue("bXCP", m_bXCP);
		pSettings->setValue("bPanelForce", m_bPanelForce);
		pSettings->setValue("bXCmRef", m_bXCmRef  );
		pSettings->setValue("bICd", m_bICd  );
		pSettings->setValue("bVCd", m_bVCd  );
		pSettings->setValue("bWakePanels", m_bWakePanels  );
		pSettings->setValue("bSurfaces", m_bSurfaces  );
		pSettings->setValue("bOutline", m_bOutline  );
		pSettings->setValue("bVLMPanels", m_bVLMPanels  );
		pSettings->setValue("bAxes", m_bAxes   );
		pSettings->setValue("b3DCp", m_b3DCp  );
		pSettings->setValue("bDownwash", m_bDownwash  );
		pSettings->setValue("bMoments", m_bMoments  );
		pSettings->setValue("bAutoCpScale", m_bAutoCpScale  );
		pSettings->setValue("bShowCpScale", m_bShowCpScale  );
		pSettings->setValue("CurWOppOnly", m_bCurWOppOnly  );
		pSettings->setValue("bShowElliptic", m_bShowElliptic);
		pSettings->setValue("LogFile", m_bLogFile  );
		pSettings->setValue("bVLM1", m_bVLM1);
		pSettings->setValue("Dirichlet", m_bDirichlet  );
		pSettings->setValue("Trefftz", m_bTrefftz  );
		pSettings->setValue("KeepOutOpps", m_bKeepOutOpps  );
		pSettings->setValue("ResetWake", m_bResetWake );
		pSettings->setValue("ShowWing", m_bShowWingCurve[0]);
		pSettings->setValue("ShowWing2", m_bShowWingCurve[1]);
		pSettings->setValue("ShowStab", m_bShowWingCurve[2]);
		pSettings->setValue("ShowFin", m_bShowWingCurve[3]);
		pSettings->setValue("StoreWOpp", m_bStoreWOpp );
		pSettings->setValue("Sequence", m_bSequence );
		pSettings->setValue("HighlightOpp", m_bHighlightOpp);
		pSettings->setValue("AlphaMin", m_AlphaMin);
		pSettings->setValue("AlphaMax", m_AlphaMax);
		pSettings->setValue("AlphaDelta", m_AlphaDelta);
		pSettings->setValue("QInfMin", m_QInfMin );
		pSettings->setValue("QInfMax", m_QInfMax );
		pSettings->setValue("QInfDelta", m_QInfDelta );
		pSettings->setValue("ControlMin", m_ControlMin );
		pSettings->setValue("ControlMax", m_ControlMax );
		pSettings->setValue("ControlDelta", m_ControlDelta );
		pSettings->setValue("bPlaneInertia", m_WngAnalysis.m_bPlaneInertia);

		pSettings->setValue("3DAxisStyle", m_3DAxisStyle );
		pSettings->setValue("3DAXisWidth", m_3DAxisWidth );
		pSettings->setValue("3DAxisColorRed", m_3DAxisColor.red());
		pSettings->setValue("3DAxisColorGreen", m_3DAxisColor.green());
		pSettings->setValue("3DAxisColorBlue", m_3DAxisColor.blue());
	
		pSettings->setValue("VLMStyle", m_VLMStyle );
		pSettings->setValue("VLMWidth", m_VLMWidth );
		pSettings->setValue("VLMColorRed", m_VLMColor.red());
		pSettings->setValue("VLMColorGreen", m_VLMColor.green());
		pSettings->setValue("VLMColorBlue", m_VLMColor.blue());

		pSettings->setValue("OutlineStyle", m_OutlineStyle );
		pSettings->setValue("OutlineWidth", m_OutlineWidth );
		pSettings->setValue("OutlineColorRed", m_OutlineColor.red());
		pSettings->setValue("OutlineColorGreen", m_OutlineColor.green());
		pSettings->setValue("OutlineColorBlue", m_OutlineColor.blue());

		pSettings->setValue("XCPStyle", m_XCPStyle );
		pSettings->setValue("XCPWidth", m_XCPWidth );
		pSettings->setValue("XCPColorRed", m_XCPColor.red());
		pSettings->setValue("XCPColorGreen", m_XCPColor.green() );
		pSettings->setValue("XCPColorBlue", m_XCPColor.blue() );

		pSettings->setValue("MomentStyle", m_MomentStyle );
		pSettings->setValue("MomentWidth", m_MomentWidth );
		pSettings->setValue("MomentColorRed", m_MomentColor.red() );
		pSettings->setValue("MomentColorGreen", m_MomentColor.green() );
		pSettings->setValue("MomentColorBlue", m_MomentColor.blue() );
	
		pSettings->setValue("IDragStyle", m_IDragStyle );
		pSettings->setValue("IDragWidth", m_IDragWidth );
		pSettings->setValue("IDragColorRed", m_IDragColor.red() );
		pSettings->setValue("IDragColorGreen", m_IDragColor.green() );
		pSettings->setValue("IDragColorBlue", m_IDragColor.blue() );
	
		pSettings->setValue("VDragStyle", m_VDragStyle );
		pSettings->setValue("VDragWidth", m_VDragWidth );
		pSettings->setValue("VDragColorRed", m_VDragColor.red() );
		pSettings->setValue("VDragColorGreen", m_VDragColor.green() );
		pSettings->setValue("VDragColorBlue", m_VDragColor.blue() );
	
		pSettings->setValue("XTopStyle", m_XTopStyle );
		pSettings->setValue("XTopWidth", m_XTopWidth );
		pSettings->setValue("XTopColorRed", m_XTopColor.red() );
		pSettings->setValue("XTopColorGreen", m_XTopColor.green() );
		pSettings->setValue("XTopColorBlue", m_XTopColor.blue() );

		pSettings->setValue("XBotStyle", m_XBotStyle );
		pSettings->setValue("XBotWidth", m_XBotWidth );
		pSettings->setValue("XBotColorRed", m_XBotColor.red() );
		pSettings->setValue("XBotColorGreen", m_XBotColor.green() );
		pSettings->setValue("XBotColorBlue", m_XBotColor.blue() );
		
		pSettings->setValue("DownwashStyle", m_DownwashStyle );
		pSettings->setValue("DownwashWidth", m_DownwashWidth );
		pSettings->setValue("DownwashColorRed", m_DownwashColor.red() );
		pSettings->setValue("DownwashColorGreen", m_DownwashColor.green() );
		pSettings->setValue("DownwashColorBlue", m_DownwashColor.blue() );

		pSettings->setValue("WakeStyle", m_WakeStyle );
		pSettings->setValue("WakeWidth", m_WakeWidth );
		pSettings->setValue("WakeColorRed", m_WakeColor.red() );
		pSettings->setValue("WakeColorGreen", m_WakeColor.green() );
		pSettings->setValue("WakeColorBlue", m_WakeColor.blue());
	
		pSettings->setValue("CpStyle", m_CpStyle );
		pSettings->setValue("CpWidth", m_CpWidth );
		pSettings->setValue("CpColorRed", m_CpColor.red() );
		pSettings->setValue("CpColorGreen", m_CpColor.green() );
		pSettings->setValue("CpColorBlue", m_CpColor.blue() );

		pSettings->setValue("StreamLinesStyle", m_StreamLinesStyle);
		pSettings->setValue("StreamLinesWidth", m_StreamLinesWidth);
		pSettings->setValue("StreamLinesColor", m_StreamLinesColor);

		pSettings->setValue("MassColor", m_MassColor);

		pSettings->setValue("CvPrec", LLTAnalysis::s_CvPrec);
		pSettings->setValue("RelaxMax", LLTAnalysis::s_RelaxMax);
		pSettings->setValue("NLLTStations", LLTAnalysis::s_NLLTStations);
		pSettings->setValue("iView", m_iView);
		pSettings->setValue("iWingView", m_iWingView);
		pSettings->setValue("iWPlrView", m_iWPlrView);
		pSettings->setValue("iWStabView", m_iStabilityView);

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

		pSettings->setValue("Iter", m_Iter);
		pSettings->setValue("NStation", m_NStation);
		pSettings->setValue("InducedDragPoint", m_InducedDragPoint);
		pSettings->setValue("NHoopPoints", m_GL3dBody.m_NHoopPoints);
		pSettings->setValue("NXPoints", m_GL3dBody.m_NXPoints);

		pSettings->setValue("LiftScale", m_LiftScale);
		pSettings->setValue("DragScale", m_DragScale);
		pSettings->setValue("VelocityScale", m_VelocityScale);

		pSettings->setValue("WakeInterNodes", m_WakeInterNodes);
		pSettings->setValue("MaxWakeIter", m_MaxWakeIter);
		pSettings->setValue("CtrlPos", CPanel::m_CtrlPos);
		pSettings->setValue("VortexPos", CPanel::m_VortexPos);
		pSettings->setValue("CoreSize", m_CoreSize);
		pSettings->setValue("MinPanelSize", m_MinPanelSize);
		pSettings->setValue("TotalTime", m_TotalTime);
		pSettings->setValue("Delta_t", m_Deltat);
		pSettings->setValue("RampTime", m_RampTime);
		pSettings->setValue("RampAmplitude", m_RampAmplitude);
		pSettings->setValue("TimeIn0", m_TimeInput[0]);
		pSettings->setValue("TimeIn1", m_TimeInput[1]);
		pSettings->setValue("TimeIn2", m_TimeInput[2]);
		pSettings->setValue("TimeIn3", m_TimeInput[3]);
		pSettings->setValue("DynamicsMode", m_bLongitudinal);
//		pSettings->setValue("ForcedResponse", m_bForcedResponse);
		pSettings->setValue("StabCurveType",m_StabilityResponseType);
		
		pSettings->setValue("StabTimeView", m_iStabTimeView);
		if(m_pCurTimeGraph == m_TimeGraph)        k=1;
		else if(m_pCurTimeGraph == m_TimeGraph+1) k=2;
		else if(m_pCurTimeGraph == m_TimeGraph+2) k=3;
		else if(m_pCurTimeGraph == m_TimeGraph+3) k=4;
		pSettings->setValue("TimeGraph",k);

		pSettings->setValue("AVLControls", m_StabPolarDlg.m_bAVLControls);

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

 	}
	pSettings->endGroup();

	m_CpGraph.SaveSettings(pSettings);
	m_LongRLGraph.SaveSettings(pSettings);
	m_LatRLGraph.SaveSettings(pSettings);
	for(int ig=0; ig<4; ig++)
	{
		m_WPlrGraph[ig].SaveSettings(pSettings);			// the WPolar graphs
		m_WingGraph[ig].SaveSettings(pSettings);			// the WOpp graphs
		m_TimeGraph[ig].SaveSettings(pSettings);
	}

	m_GLLightDlg.SaveSettings(pSettings);
	m_GL3dBody.SaveSettings(pSettings);

	return true;
}



void QMiarex::Set2DScale()
{
	if(m_bIs2DScaleSet && !m_bAutoScales) return;

	int w,h;

	TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;

	m_r2DCltRect = p2DWidget->geometry();
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

	for(int ig=0; ig<4; ig++)
	{
		m_WingGraph[ig].Init();
		m_WingGraph[ig].SetAutoXUnit();
	}

	if(m_iView==WOPPVIEW)        SetWingLegendPos();
	else if(m_iView==WPOLARVIEW) SetWPlrLegendPos();
	else if(m_iView==WSTABVIEW) 
	{
		if(m_iStabilityView==STABTIMEVIEW) SetWingLegendPos();
		else                    SetWPlrLegendPos();
	}

	if(m_pCurWing)
	{
		m_WingScale = (m_rSingleRect.width()-2*m_WingGraph[0].GetMargin())/m_pCurWing->m_PlanformSpan;
		if(m_bHalfWing) 	m_WingScale *= 2.0;
		m_bIs2DScaleSet = true;
	}
}



void QMiarex::Set3DRotationCenter()
{
	//adjust the new rotation center after a translation or a rotation
	int i,j;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			MatOut[i][j] =  m_ArcBall.ab_quat[i*4+j];

	m_glRotCenter.x = MatOut[0][0]*(m_glViewportTrans.x) + MatOut[0][1]*(-m_glViewportTrans.y) + MatOut[0][2]*m_glViewportTrans.z;
	m_glRotCenter.y = MatOut[1][0]*(m_glViewportTrans.x) + MatOut[1][1]*(-m_glViewportTrans.y) + MatOut[1][2]*m_glViewportTrans.z;
	m_glRotCenter.z = MatOut[2][0]*(m_glViewportTrans.x) + MatOut[2][1]*(-m_glViewportTrans.y) + MatOut[2][2]*m_glViewportTrans.z;
}



void QMiarex::Set3DRotationCenter(QPoint point)
{
	//adjusts the new rotation center after the user has picked a point on the screen
	//finds the closest panel under the point,
	//and changes the rotation vector and viewport translation
	int  i, j, p;
	CVector I, A, B, AA, BB, PP, U;
	double dmin, dist;

	i=-1;
	dmin = 100000.0;

	ClientToGL(point, B);

	B.x += -m_UFOOffset.x - m_glViewportTrans.x*m_glScaled;
	B.y += -m_UFOOffset.y + m_glViewportTrans.y*m_glScaled;

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

	if(m_pCurWOpp)
	{
		AA.RotateY(-m_pCurWOpp->m_Alpha);
		BB.RotateY(-m_pCurWOpp->m_Alpha);
	}

	U.Set(BB.x-AA.x, BB.y-AA.y, BB.z-AA.z);
	U.Normalize();

	bool bIntersect = false;

	if(m_iView==W3DVIEW || (m_iView==WSTABVIEW && m_iStabilityView==STAB3DVIEW))
	{
		for(p=0; p<m_MatSize; p++)
		{
			if(m_Panel[p].Intersect(AA, U, I, dist))
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
		if(m_pCurWOpp)
		{
			PP.RotateY(m_pCurWOpp->m_Alpha);
		}
//		instantaneous visual transition
//		m_glRotCenter -= PP * m_glScaled;

//		smooth visual transition
		GLInverseMatrix();

		U.x = (-PP.x -m_glRotCenter.x)/30.0;
		U.y = (-PP.y -m_glRotCenter.y)/30.0;
		U.z = (-PP.z -m_glRotCenter.z)/30.0;

		for(i=0; i<30; i++)
		{
			m_glRotCenter +=U;
			m_glViewportTrans.x =  (MatOut[0][0]*m_glRotCenter.x + MatOut[0][1]*m_glRotCenter.y + MatOut[0][2]*m_glRotCenter.z);
			m_glViewportTrans.y = -(MatOut[1][0]*m_glRotCenter.x + MatOut[1][1]*m_glRotCenter.y + MatOut[1][2]*m_glRotCenter.z);
			m_glViewportTrans.z=   (MatOut[2][0]*m_glRotCenter.x + MatOut[2][1]*m_glRotCenter.y + MatOut[2][2]*m_glRotCenter.z);

			UpdateView();
		}
	}
}


void QMiarex::Set3DScale()
{
	if(m_iView!=W3DVIEW && (m_iView!=WSTABVIEW || m_iStabilityView!=STAB3DVIEW)) return;
	if(m_iView==W3DVIEW) m_bResetglLegend = true;
	if(m_bIs3DScaleSet && !m_bAutoScales) return;

	GLWidget* pGLWidget = (GLWidget*)s_pGLWidget;
	m_r3DCltRect = pGLWidget->geometry();

	if(m_pCurWing)
	{
		//wing along X axis will take 3/4 of the screen
		m_glScaled = (GLfloat)(3./4.*2.0*m_GLScale/m_pCurWing->m_PlanformSpan);
		m_glViewportTrans.x = 0.0;
		m_glViewportTrans.y = 0.0;
		m_glViewportTrans.z = 0.0;
		m_bIs3DScaleSet = true;
	}
	else if(m_pCurBody)
	{
		m_glScaled = (GLfloat)(3./4.*2.0*m_GLScale/m_pCurBody->GetLength());
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
			m_UFOOffset.x = 0.0;
			m_UFOOffset.y = (GLfloat)(gh - 1.0)*m_GLScale;
		}
		else
		{
			m_UFOOffset.x = (GLfloat)(1.0/gh - 1.0)*m_GLScale;
			m_UFOOffset.y = 0.0;
		}
		m_UFOOffset.x = 0.0;
		m_UFOOffset.y = 0.0;
	}
}



void QMiarex::SetAnalysisParams()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
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
	m_pctrlStoreWOpp->setChecked(m_bStoreWOpp);

	if (!m_pCurWPolar || (m_pCurWPolar && m_pCurWPolar->m_Type <3))
	{
		m_pctrlAlphaMin->SetValue(m_AlphaMin);
		m_pctrlAlphaMax->SetValue(m_AlphaMax);
		m_pctrlAlphaDelta->SetValue(m_AlphaDelta);
	}
	else if(m_pCurWPolar  && m_pCurWPolar->m_Type ==FIXEDAOAPOLAR)
	{
		m_pctrlAlphaMin->SetValue(m_QInfMin*pMainFrame->m_mstoUnit);
		m_pctrlAlphaMax->SetValue(m_QInfMax*pMainFrame->m_mstoUnit);
		m_pctrlAlphaDelta->SetValue(m_QInfDelta*pMainFrame->m_mstoUnit);
	}
	else if(m_pCurWPolar && (m_pCurWPolar->m_Type==STABILITYPOLAR))
	{
		m_pctrlAlphaMin->SetValue(m_ControlMin);
		m_pctrlAlphaMax->SetValue(m_ControlMax);
		m_pctrlAlphaDelta->SetValue(m_ControlDelta);
	}

	m_pctrlOutline->setChecked(m_bOutline);
	m_pctrlPanels->setChecked(m_bVLMPanels);
	m_pctrlAxes->setChecked(m_bAxes);
	m_pctrlCp->setChecked(m_b3DCp);
	m_pctrlPanelForce->setChecked(m_bPanelForce);
	m_pctrlDownwash->setChecked(m_bDownwash);
	m_pctrlMoment->setChecked(m_bMoments);
	m_pctrlTrans->setChecked(m_bXTop);
	m_pctrlLift->setChecked(m_bXCP);
	m_pctrlIDrag->setChecked(m_bICd);
	m_pctrlVDrag->setChecked(m_bVCd);
	m_pctrlAxes->setChecked(m_bAxes);
	m_pctrlLight->setChecked(m_bglLight);
	m_pctrlSurfaces->setChecked(m_bSurfaces);
	m_pctrlOutline->setChecked(m_bOutline);
	m_pctrlStream->setChecked(m_bStream);
	m_pctrlClipPlanePos->setValue((int)(m_ClipPlanePos*100.0));
}


void QMiarex::SetCurveParams()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(m_iView==WPOLARVIEW || (m_iView==WSTABVIEW && m_iStabilityView==STABPOLARVIEW))
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
	else if(m_iView==WSTABVIEW&& m_iStabilityView==STABTIMEVIEW)
	{
		StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
		if(pStabView->m_pCurve)
		{
			m_CurveColor = pStabView->m_pCurve->GetColor();
			m_CurveStyle = pStabView->m_pCurve->GetStyle();
			m_CurveWidth = pStabView->m_pCurve->GetWidth();
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
		else if(m_pCurWOpp)
		{
			m_pctrlShowCurve->setChecked(m_pCurWOpp->m_bIsVisible);
			m_pctrlShowPoints->setChecked(m_pCurWOpp->m_bShowPoints);

			m_CurveColor = m_pCurWOpp->m_Color;
			m_CurveStyle = m_pCurWOpp->m_Style;
			m_CurveWidth = m_pCurWOpp->m_Width;
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
		if(m_pCurWOpp)
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

	
	if(m_pCurWPolar)
	{
		if(m_pCurWPolar->m_Type<4)
		{
			m_pctrlUnit1->setText(QString::fromUtf8("°"));
			m_pctrlUnit2->setText(QString::fromUtf8("°"));
			m_pctrlUnit3->setText(QString::fromUtf8("°"));
		}
		else if(m_pCurWPolar->m_Type==4)
		{
			QString str;
			GetSpeedUnit(str, pMainFrame->m_SpeedUnit);
			m_pctrlUnit1->setText(str);
			m_pctrlUnit2->setText(str);
			m_pctrlUnit3->setText(str);
		}
		else
		{
			m_pctrlUnit1->setText("");
			m_pctrlUnit2->setText("");
			m_pctrlUnit3->setText("");
		}
	}
}



bool QMiarex::SetModBody(CBody *pModBody)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(!pModBody) pModBody = m_pCurBody;
	CBody * pBody, *pOldBody;
	CPlane *pPlane;
	QString strong;

	bool bExists = true;
	int resp, k, l;

	QStringList NameList;
	for(k=0; k<m_poaBody->size(); k++)
	{
		pBody = (CBody*)m_poaBody->at(k);
		NameList.append(pBody->m_BodyName);
	}


	m_RenameDlg.m_pstrArray = & NameList;
	m_RenameDlg.m_strQuestion = tr("Enter the new name for the Body :");
	m_RenameDlg.m_strName = pModBody->m_BodyName;
	m_RenameDlg.InitDialog();


	while (bExists)
	{
		resp = m_RenameDlg.exec();
		if(resp==QDialog::Accepted)
		{
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaBody->size(); k++)
			{
				pBody = (CBody*)m_poaBody->at(k);
				if (pBody->m_BodyName == m_RenameDlg.m_strName)
				{
					bExists = true;
					break;
				}
			}
			if(!bExists)
			{
				pModBody->m_BodyName = m_RenameDlg.m_strName;
				//replace the Body in alphabetical order in the array
				//remove the current Body from the array
				bool bInserted = false;
				for (l=0; l<m_poaBody->size();l++)
				{
					pBody = (CBody*)m_poaBody->at(l);
					if(pBody == pModBody)
					{
						m_poaBody->removeAt(l);
						// but don't delete it ! we need to re-insert it
						break;
					}
				}
				for (l=0; l<m_poaBody->size();l++)
				{
					pBody = (CBody*)m_poaBody->at(l);

					if (pBody->m_BodyName.compare(pModBody->m_BodyName, Qt::CaseInsensitive)>0)
					{
						//then insert before
						m_poaBody->insert(l, pModBody);
						bInserted = true;
						break;
					}
				}
				if(!bInserted)	m_poaBody->append(pModBody);
				pMainFrame->SetSaveState(false);
				return true;
			}
		}
		else if(resp ==10)
		{
			//user wants to overwrite
			pOldBody  = GetBody(m_RenameDlg.m_strName);
			if(pOldBody)
			{
				//first we check if this old body is used by one or more planes
				bool bIsInUse = false;
				resp = QMessageBox::Yes;
				for(k=0; k<m_poaPlane->size(); k++)
				{
					pPlane = (CPlane*)m_poaPlane->at(k);
					if(pPlane->Body() && pPlane->Body()==pOldBody)
					{
						bIsInUse = true;
						break;
					}
				}
				if(bIsInUse)
				{
					strong = tr("The body ") + pOldBody->m_BodyName+ tr(" is used by one or more planes.\n Overwrite anyway ? (Results will be lost)");
					resp = QMessageBox::question(this, tr("Question"), strong,
												 QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
												 QMessageBox::Cancel);
				}
				if(resp == QMessageBox::Yes)
				{
					for (k=0; k<m_poaBody->size(); k++)
					{
						pBody = (CBody*)m_poaBody->at(k);
						if (pBody->m_BodyName == m_RenameDlg.m_strName)
						{
							m_poaBody->removeAt(k);
							if(pBody==m_pCurBody)
							{
								m_pCurBody = NULL;
							}
							delete pBody;
							bExists = false;
							break;
						}
					}
					for(k=0; k<m_poaPlane->size(); k++)
					{
						pPlane = (CPlane*)m_poaPlane->at(k);
						if(pPlane->Body() && pPlane->Body()==pOldBody)
						{
							pPlane->SetBody(pModBody);
							pMainFrame->DeletePlane(pPlane, true);
							pMainFrame->UpdateWOpps();
						}
					}
				}
			}

			pModBody->m_BodyName = m_RenameDlg.m_strName;
			m_pCurBody = pModBody;

			pMainFrame->SetSaveState(false);
			return true;
		}
		else
		{
			return false;//cancelled
		}
	}
	return false ;//useless...
}




bool QMiarex::SetModPlane(CPlane *pModPlane)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(!pModPlane) pModPlane = m_pCurPlane;
	CPlane * pPlane;
	CWing *pWing;
	CWPolar* pWPolar;
	CPOpp * pPOpp;
	CWOpp *pWOpp;
	bool bExists = true;
	int resp, k, l;

	QString OldName = pModPlane->PlaneName();

	QStringList NameList;
	for(k=0; k<m_poaPlane->size(); k++)
	{
		pPlane = (CPlane*)m_poaPlane->at(k);
		NameList.append(pPlane->PlaneName());
	}
	for(k=0; k<m_poaWing->size(); k++)
	{
		pWing = (CWing*)m_poaWing->at(k);
		NameList.append(pWing->m_WingName);
	}

	m_RenameDlg.move(pMainFrame->m_DlgPos);
	m_RenameDlg.m_pstrArray = & NameList;
	m_RenameDlg.m_strQuestion = tr("Enter the new name for the Plane :");
	m_RenameDlg.m_strName     = pModPlane->PlaneName();
	m_RenameDlg.InitDialog();

	while (bExists)
	{
		resp = m_RenameDlg.exec();
		pMainFrame->m_DlgPos = m_RenameDlg.pos();
		if(resp==QDialog::Accepted)
		{
			if (OldName == m_RenameDlg.m_strName) return true;
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaPlane->size(); k++)
			{
				pPlane = (CPlane*)m_poaPlane->at(k);
				if (pPlane->PlaneName() == m_RenameDlg.m_strName)
				{
					bExists = true;
					break;
				}
			}
			for (k=0; k<m_poaWing->size(); k++)
			{
				pWing = (CWing*)m_poaWing->at(k);
				if (pWing->m_WingName == m_RenameDlg.m_strName)
				{
					bExists = true;
					break;
				}
			}
			if(!bExists)
			{
				pModPlane->rPlaneName() = m_RenameDlg.m_strName;
				//replace the Plane in alphabetical order in the array
				//remove the current Plane from the array
				bool bInserted = false;
				for (l=0; l<m_poaPlane->size();l++)
				{
					pPlane = (CPlane*)m_poaPlane->at(l);
					if(pPlane == pModPlane)
					{
						m_poaPlane->removeAt(l);
						// but don't delete it !
						//and re-insert it
						for (l=0; l<m_poaPlane->size();l++)
						{
							pPlane = (CPlane*)m_poaPlane->at(l);
							if(pPlane->PlaneName().compare(pModPlane->PlaneName(), Qt::CaseInsensitive) >0)
							{
								//then insert before
								m_poaPlane->insert(l, pModPlane);
								bInserted = true;
								break;
							}
						}
						if(!bInserted)	m_poaPlane->append(pModPlane);
						break;
					}
				}

			}

			pMainFrame->SetSaveState(false);
		}
		else if(resp ==10)
		{
			//user wants to overwrite the old plane/wing
			if (OldName == m_RenameDlg.m_strName) return true;//don't bother to overwrite itself
			if(GetPlane(m_RenameDlg.m_strName))
			{
				for (k=0; k<m_poaPlane->size(); k++)
				{
					pPlane = (CPlane*)m_poaPlane->at(k);
					if (pPlane->PlaneName() == m_RenameDlg.m_strName)
					{
						for (l=m_poaWPolar->size()-1;l>=0; l--)
						{
							pWPolar = (CWPolar*)m_poaWPolar->at(l);
							if (pWPolar->m_UFOName == pPlane->PlaneName())
							{
/*								m_poaWPolar->removeAt(l);
								if(pWPolar==m_pCurWPolar) m_pCurWPolar = NULL;
								delete pWPolar;*/
								pWPolar->ResetWPlr();
							}
						}

						for (l=m_poaPOpp->size()-1;l>=0; l--)
						{
							pPOpp = (CPOpp*)m_poaPOpp->at(l);
							if (pPOpp->m_PlaneName == pPlane->PlaneName())
							{
								m_poaPOpp->removeAt(l);
								delete pPOpp;
							}
						}

						m_pCurPOpp = NULL;
						m_pCurWOpp = NULL;
						m_poaPlane->removeAt(k);
						if(pPlane==m_pCurPlane) m_pCurPlane = NULL;
						delete pPlane;

					}
				}
			}
			else if (GetWing(m_RenameDlg.m_strName))
			{
				//delete the wing the user wants to overwrite
				for (k=0; k<m_poaWing->size(); k++)
				{
					pWing = (CWing*)m_poaWing->at(k);
					if (pWing->m_WingName == m_RenameDlg.m_strName)
					{
						for (l=m_poaWPolar->size()-1;l>=0; l--)
						{
							pWPolar = (CWPolar*)m_poaWPolar->at(l);
							if (pWPolar->m_UFOName == pWing->m_WingName)
							{
								pWPolar->ResetWPlr();
/*								m_poaWPolar->removeAt(l);
								if(pWPolar==m_pCurWPolar) m_pCurWPolar = NULL;
								delete pWPolar;*/
							}
						}
						for (l=m_poaWOpp->size()-1;l>=0; l--)
						{
							pWOpp = (CWOpp*)m_poaWOpp->at(l);
							if (pWOpp->m_WingName == pWing->m_WingName)
							{
								m_poaWOpp->removeAt(l);
								if(pWOpp==m_pCurWOpp) m_pCurWOpp = NULL;
								delete pWOpp;
							}
						}
						m_poaWing->removeAt(k);
						if(pWing==m_pCurWing) m_pCurWing = NULL;
						delete pWing;
					}
				}
			}

			pModPlane->rPlaneName() = m_RenameDlg.m_strName;
			pModPlane->Wing()->m_WingName = pModPlane->PlaneName()+"_Wing";
			if(pModPlane->Wing2()) pModPlane->Wing2()->m_WingName = pModPlane->PlaneName()+"_Wing2";
			if(pModPlane->Stab())  pModPlane->Stab()->m_WingName  = pModPlane->PlaneName()+"_Elev";
			if(pModPlane->Fin())   pModPlane->Fin()->m_WingName   = pModPlane->PlaneName()+"_Fin";

			//place the Plane in alphabetical order in the array
			//remove the current Plane from the array
			for (l=0; l<m_poaPlane->size();l++)
			{
				pPlane = (CPlane*)m_poaPlane->at(l);
				if(pPlane == m_pCurPlane)
				{
					m_poaPlane->removeAt(l);
					// but don't delete it !
					break;
				}
			}
			//and re-insert it
			bool bInserted = false;
			for (l=0; l<m_poaPlane->size();l++)
			{
				pPlane = (CPlane*)m_poaPlane->at(l);
				if(pPlane->PlaneName().compare(m_pCurPlane->PlaneName(), Qt::CaseInsensitive) <0)
				{
					//then insert before
					m_poaPlane->insert(l, m_pCurPlane);
					bInserted = true;
					break;
				}
			}
			if(!bInserted) m_poaPlane->append(m_pCurPlane);
			bExists = false;
			pMainFrame->SetSaveState(false);
		}
		else
		{
			return false;//cancelled
		}

	}
	return true;
}



bool QMiarex::SetModWing(CWing *pModWing)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(!pModWing) pModWing = m_pCurWing;
	CWing * pWing, *pOldWing;
	CPlane *pPlane, *pOldPlane;
	CWPolar* pWPolar;
	CWOpp * pWOpp;
	CPOpp * pPOpp;
	bool bExists = true;
	int resp, k, l;

	QStringList NameList;
	for(k=0; k<m_poaWing->size(); k++)
	{
		pWing = (CWing*)m_poaWing->at(k);
		NameList.append(pWing->m_WingName);
	}
	for(k=0; k<m_poaPlane->size(); k++)
	{
		pPlane = (CPlane*)m_poaPlane->at(k);
		NameList.append(pPlane->PlaneName());
	}
	m_RenameDlg.move(pMainFrame->m_DlgPos);
	m_RenameDlg.m_pstrArray = & NameList;
	m_RenameDlg.m_strQuestion = tr("Enter the new name for the wing :");
	m_RenameDlg.m_strName = pModWing->m_WingName;
	m_RenameDlg.InitDialog();

	while (bExists)
	{
		resp = m_RenameDlg.exec();
		pMainFrame->m_DlgPos = m_RenameDlg.pos();
		if(resp==QDialog::Accepted)
		{
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaWing->size(); k++)
			{
				pWing = (CWing*)m_poaWing->at(k);
				if (pWing->m_WingName == m_RenameDlg.m_strName)
				{
					bExists = true;
					break;
				}
			}
			for (k=0; k<m_poaPlane->size(); k++)
			{
				pPlane = (CPlane*)m_poaPlane->at(k);
				if (pPlane->PlaneName() == m_RenameDlg.m_strName)
				{
					bExists = true;
					break;
				}
			}

			if(!bExists)
			{
				pModWing->m_WingName = m_RenameDlg.m_strName;
				//replace the Wing in alphabetical order in the array
				//remove the current Wing from the array
				bool bInserted = false;
				for (l=0; l<m_poaWing->size();l++)
				{
					pWing = (CWing*)m_poaWing->at(l);
					if(pWing == pModWing)
					{
						m_poaWing->removeAt(l);
						// but don't delete it !
						//and re-insert it
						for (l=0; l<m_poaWing->size();l++)
						{
							pWing = (CWing*)m_poaWing->at(l);
							if(pWing->m_WingName.compare(pModWing->m_WingName, Qt::CaseInsensitive) >0)
							{
								//then insert before
								m_poaWing->insert(l, pModWing);
								bInserted = true;
								break;
							}
						}
						if(!bInserted)	m_poaWing->append(pModWing);
						break;
					}
				}
				pMainFrame->SetSaveState(false);
				return true;
			}
		}
		else if(resp==10)
		{
			//user wants to overwrite
			pOldWing  = GetWing(m_RenameDlg.m_strName);
			pOldPlane = GetPlane(m_RenameDlg.m_strName);
			if(pOldWing)
			{
				for (k=0; k<m_poaWing->size(); k++)
				{
					pWing = (CWing*)m_poaWing->at(k);
					if (pWing->m_WingName == m_RenameDlg.m_strName)
					{
						for (l=m_poaWPolar->size()-1;l>=0; l--)
						{
							pWPolar = (CWPolar*)m_poaWPolar->at(l);
							if (pWPolar->m_UFOName == pWing->m_WingName)
							{
								pWPolar->ResetWPlr();
/*								m_poaWPolar->removeAt(l);
								if(pWPolar==m_pCurWPolar)
								{
									m_pCurWPolar = NULL;
								}
								delete pWPolar;*/
							}
						}
						for (l=m_poaWOpp->size()-1;l>=0; l--)
						{
							pWOpp = (CWOpp*)m_poaWOpp->at(l);
							if (pWOpp->m_WingName == pWing->m_WingName)
							{
								m_poaWOpp->removeAt(l);
								delete pWOpp;
							}
						}
						m_pCurWOpp = NULL;
						m_pCurPOpp = NULL;
						m_poaWing->removeAt(k);
						if(pWing==m_pCurWing) m_pCurWing = NULL;
						delete pWing;
						bExists = false;
						break;
					}
				}
			}
			else if(pOldPlane)
			{
//				if(pOldPlane->m_bActive) QMessageBox::warning(window(), tr("Warning"),tr("Cannot overwrite current plane"));
//				else
				{
					for (k=0; k<m_poaPlane->size(); k++)
					{
						pPlane = (CPlane*)m_poaPlane->at(k);
						if (pPlane->PlaneName() == m_RenameDlg.m_strName)
						{
							for (l=m_poaWPolar->size()-1;l>=0; l--)
							{
								pWPolar = (CWPolar*)m_poaWPolar->at(l);
								if (pWPolar->m_UFOName == pPlane->PlaneName())
								{
									pWPolar->ResetWPlr();
/*									m_poaWPolar->removeAt(l);
									if(pWPolar==m_pCurWPolar)
									{
										m_pCurWPolar = NULL;
										m_pCurPOpp   = NULL;
										m_pCurWOpp   = NULL;
									}
									delete pWPolar;*/
								}
							}
							for (l=m_poaPOpp->size()-1;l>=0; l--)
							{
								pPOpp = (CPOpp*)m_poaPOpp->at(l);
								if (pPOpp->m_PlaneName == pPlane->PlaneName())
								{
									m_poaPOpp->removeAt(l);
									delete pPOpp;
								}
							}
							m_pCurPOpp = NULL;
							m_pCurPOpp = NULL;
							m_poaPlane->removeAt(k);
							delete pPlane;
							bExists = false;
							break;
						}
					}
				}
			}
			pModWing->m_WingName = m_RenameDlg.m_strName;
//			m_pCurWing = pModWing;

			pMainFrame->SetSaveState(false);
			return true;
		}
		else
		{
			return false;//cancelled
		}
	}
	return false ;//useless...
}



bool QMiarex::SetPOpp(bool bCurrent, double x)
{
	// set the WOpp, if valid
	// else set the current WOpp, if any
	// else set the comboBox's first, if any
	// else set it to NULL
	if(!m_pCurPlane) return false;

	CPOpp *pPOpp = NULL;
	if(bCurrent) pPOpp = m_pCurPOpp;
	else         pPOpp = GetPOpp(x);
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong;
	bool bOK;

	m_bResetglMesh   = true;
	m_bResetglOpp    = true;
	m_bResetglWake   = true;
	m_bResetglStream = true;
	m_bResetglFlow   = true;
	m_bResetglLegend = true;

	// first restore the panel geometry
	memcpy(&m_Panel, &m_MemPanel, m_MatSize* sizeof(CPanel));
	memcpy(&m_Node,  &m_MemNode,  m_nNodes * sizeof(CVector));

	if(!pPOpp)
	{
		if(m_pCurWPolar)
			pPOpp = GetPOpp(m_pCurWPolar->m_AMem);
		else
			pPOpp = GetPOpp(m_LastWOpp);
	}

	if(!pPOpp)
	{
		//try to select the first in the ListBox
		if(pMainFrame->m_pctrlWOpp->count())
		{
			pMainFrame->m_pctrlWOpp->setCurrentIndex(0);
			strong = pMainFrame->m_pctrlWOpp->itemText(0);
			x = strong.toDouble(&bOK);
			if(bOK)
			{
				pPOpp = GetPOpp(x);
			}
			else pPOpp = NULL;
		}
		else{
			pPOpp = NULL;
		}
	}

	m_pCurPOpp = pPOpp;

	SetCurveParams();

	if(m_pCurPOpp)
	{
		m_LastWOpp = m_pCurPOpp->m_Alpha;
		m_pCurWOpp = &m_pCurPOpp->m_PlaneWOpp[0];

		for(int iw=0; iw<MAXWINGS;iw++)
		{
			if(m_pCurPOpp->m_bWing[iw]) m_pWOpp[iw] = &m_pCurPOpp->m_PlaneWOpp[iw];
			else                        m_pWOpp[iw] = NULL;
		}

		if(m_pCurWPolar) m_pCurWPolar->m_AMem = m_pCurPOpp->m_Alpha;

		//select m_pCurPOpp in the listbox
		if(m_pCurWPolar->m_Type < 4)                    pMainFrame->SelectWOpp(m_pCurPOpp->m_Alpha);
		else if(m_pCurWPolar->m_Type == FIXEDAOAPOLAR)  pMainFrame->SelectWOpp(m_pCurPOpp->m_QInf);
		else if(m_pCurWPolar->m_Type == STABILITYPOLAR) pMainFrame->SelectWOpp(m_pCurPOpp->m_Ctrl);
		;

		//if we have a type 7 polar, set the panels in the control's position
		if(m_pCurWPolar && m_pCurWPolar->m_Type==STABILITYPOLAR)
		{
			//set the controls
			m_pPanelDlg->m_pWPolar        = m_pCurWPolar;
			int nCtrls;
			QString strong;
			SetControlPositions(m_Panel, m_Node, m_pCurPOpp->m_Ctrl, nCtrls,strong, false);
			m_bResetglMesh = true;
		}
	}
	else
	{
		m_pCurWOpp = NULL;
		m_pWOpp[0] = m_pWOpp[1] = m_pWOpp[2] = m_pWOpp[3] = NULL;
	}

	if (m_iView==WOPPVIEW)       CreateWOppCurves();
	else if(m_iView==WCPVIEW)    CreateCpCurves();
	else if(m_iView==WPOLARVIEW) CreateWPolarCurves();
	else if(m_iView==WSTABVIEW)
	{
		StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
		pStabView->SetControls();
		pStabView->SetMode();
		CreateStabilityCurves();
	}

	if(!m_pCurPOpp) return false;
	else if(m_iView==WOPPVIEW)
	{
		m_bCurveVisible = m_pCurPOpp->m_bIsVisible;
		m_bCurvePoints  = m_pCurPOpp->m_bShowPoints;
	}
	return true;
}


void QMiarex::SetScale()
{
	if(m_iView==W3DVIEW || (m_iView==WSTABVIEW&& m_iStabilityView==STAB3DVIEW)) Set3DScale();
	else                                                                        Set2DScale();
}


void QMiarex::SetUFO(QString UFOName, bool bNoPolar)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	int i;
	CWing *pWing;
	CPlane *pPlane;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(!UFOName.length())
	{
		if(m_pCurPlane)     UFOName = m_pCurPlane->PlaneName();
		else if(m_pCurWing) UFOName = m_pCurWing->WingName();
		else
		{
			// Find out which is first in Alphabetical order
			QString FirstWingName, FirstPlaneName;
			if(m_poaWing->size())
			{
				pWing = (CWing*)m_poaWing->at(0);
				if(pWing) FirstWingName = pWing->m_WingName;
			}
			if(m_poaPlane->size())
			{
				pPlane = (CPlane*)m_poaPlane->at(0);
				if(pPlane) FirstPlaneName = pPlane->PlaneName();
			}
			if(FirstPlaneName.length() && FirstWingName.length())
			{
				if(FirstWingName.compare(FirstPlaneName, Qt::CaseInsensitive) >0)
					UFOName = FirstPlaneName;
				else
					UFOName = FirstWingName;
			}
			else if(FirstPlaneName.length()) UFOName = FirstPlaneName;
			else if(FirstWingName.length())  UFOName = FirstWingName;

			if(!UFOName.size())
			{
				for(int i=0; i<4; i++) m_pWingList[i] = NULL;
				m_pCurPlane = NULL;
				m_pCurBody  = NULL;
				m_pCurWing  = NULL;
				m_pCurWOpp  = NULL;
				m_pCurPOpp  = NULL;
				if(m_iView==WPOLARVIEW)		CreateWPolarCurves();
				else if(m_iView==WSTABVIEW)	CreateStabilityCurves();
				else if(m_iView==WOPPVIEW)	CreateWOppCurves();
				else if(m_iView==WCPVIEW)	CreateCpCurves();

				QApplication::restoreOverrideCursor();

				return;
			}
		}
	}

	m_pCurPlane = GetPlane(UFOName);
	m_pCurWing  = GetWing(UFOName);

	if(!m_pCurWing && !m_pCurPlane)
	{
		pMainFrame->m_UFOType = "";
		for(int i=0; i<4; i++) m_pWingList[i] = NULL;
		m_pCurBody  = NULL;
		m_pCurWOpp  = NULL;
		m_pCurPOpp  = NULL;

		QApplication::restoreOverrideCursor();
		return;
	}

	if(m_pCurPlane)
	{
		pMainFrame->m_UFOType = "Plane";
		m_pCurWing = m_pCurPlane->Wing();
		m_pWingList[0] = m_pCurPlane->Wing();
		m_pWingList[1] = m_pCurPlane->Wing2();
		m_pWingList[2]  = m_pCurPlane->Stab();
		if(m_pCurPlane->Fin())
		{
			m_pWingList[3] = m_pCurPlane->Fin();
			m_pWingList[3]->m_bDoubleSymFin = m_pCurPlane->m_bDoubleSymFin;
		}
		else
			m_pWingList[3]  = NULL;

		m_pCurBody     = m_pCurPlane->Body();
	}
	else
	{
		pMainFrame->m_UFOType = "Wing";
		m_pCurPOpp  = NULL;
		m_pCurBody  = NULL;
		m_pWingList[0] = m_pCurWing;
		m_pWingList[1] = m_pWingList[2] =m_pWingList[3] = NULL;
	}


	m_bResetglGeom   = true;
	if(m_pCurBody) m_bResetglBody   = true;
	else           m_bResetglBody   = false;

	m_bResetglMesh = true;

//	QColor WingColor = m_WingColor;
	CVector T(0.0,0.0,0.0);

	double dx, dz;
	dx=dz=0.0;

	if(m_pCurPlane)
	{
//		WingColor = m_pWingList[0]->m_WingColor;
		if(m_pCurPlane->Body())
		{
			dx = m_pCurPlane->BodyPos().x;
			dz = m_pCurPlane->BodyPos().z;
		}
	}

	if(m_pCurBody)
	{
		m_pCurBody->SetKnots();
		m_pCurBody->SetPanelPos();
	}
	
	m_NSurfaces = 0;

	for(int iw=0; iw<MAXWINGS; iw++)
	{
		if(m_pWingList[iw])
		{
			if(!m_pCurPlane && iw==0)  m_pWingList[iw]->CreateSurfaces(T, 0.0, 0.0);
			else if(iw<3)   m_pWingList[iw]->CreateSurfaces(m_pCurPlane->WingLE(iw),   0.0, m_pCurPlane->WingTiltAngle(iw));
			else if(iw==3)  m_pWingList[iw]->CreateSurfaces(m_pCurPlane->WingLE(iw), -90.0, m_pCurPlane->WingTiltAngle(iw));
			for (i=0; i<m_pWingList[iw]->m_NSurfaces; i++)
			{
				m_pWingList[iw]->m_Surface[i].SetSidePoints(m_pCurBody, dx, dz);
				m_pSurface[m_NSurfaces] = &m_pWingList[iw]->m_Surface[i];
				m_NSurfaces++;
			}
			m_pWingList[iw]->ComputeBodyAxisInertia();
		}
	}

	if(m_pCurBody)  m_pCurBody->ComputeBodyAxisInertia();
	if(m_pCurPlane) m_pCurPlane->ComputeBodyAxisInertia();

	CWPolar *pWPolar;
	for(i=0; i<m_poaWPolar->size(); i++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(i);
		if(pWPolar->m_Type==STABILITYPOLAR)
		{
			pWPolar->m_AnalysisMethod=PANELMETHOD;
			if(fabs(pWPolar->m_Mass)<PRECISION)
			{
				//very special case, the polar was generated by v600 or v601
				// need to reload inertia data from plane inertia
				pWing  = GetWing(pWPolar->m_UFOName);
				pPlane = GetPlane(pWPolar->m_UFOName);

				if(pWing)
				{
					pWPolar->m_Mass = pWing->TotalMass();
					pWPolar->m_CoG    = pWing->m_CoG;
					pWPolar->m_CoGIxx = pWing->m_CoGIxx;
					pWPolar->m_CoGIyy = pWing->m_CoGIyy;
					pWPolar->m_CoGIzz = pWing->m_CoGIzz;
					pWPolar->m_CoGIxz = pWing->m_CoGIxz;
				}
				else if(pPlane)
				{
					pWPolar->m_Mass = pPlane->TotalMass();
					pWPolar->m_CoG    = pPlane->CoG();
					pWPolar->m_CoGIxx = pPlane->m_CoGIxx;
					pWPolar->m_CoGIyy = pPlane->m_CoGIyy;
					pWPolar->m_CoGIzz = pPlane->m_CoGIzz;
					pWPolar->m_CoGIxz = pPlane->m_CoGIxz;
				}
			}
		}
	}


	if(!bNoPolar)
	{
		if (m_pCurWPolar)
		{
			// try to set the same as the existing polar... Special for Marc
			SetWPlr(false, m_pCurWPolar->m_PlrName);
		}
		else
		{
			SetWPlr();
		}
	}

	SetScale();
	SetWGraphScale();
	
	SetControls();
	QApplication::restoreOverrideCursor();
}



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

	setSizePolicy(szPolicyMaximum);
//_______________________Analysis
	m_pctrlSequence = new QCheckBox(tr("Sequence"));
	QGridLayout *SequenceGroup = new QGridLayout;
	QLabel *AlphaMinLab   = new QLabel(tr("Start="));
	QLabel *AlphaMaxLab   = new QLabel(tr("End="));
	QLabel *AlphaDeltaLab = new QLabel(tr("D="));
	AlphaDeltaLab->setFont(QFont("Symbol"));
	AlphaDeltaLab->setAlignment(Qt::AlignRight);
	AlphaMinLab->setAlignment(Qt::AlignRight);
	AlphaMaxLab->setAlignment(Qt::AlignRight);
	m_pctrlAlphaMin     = new FloatEdit(0.0, 3);
	m_pctrlAlphaMax     = new FloatEdit(1., 3);
	m_pctrlAlphaDelta   = new FloatEdit(0.5, 3);

	m_pctrlUnit1 = new QLabel(QString::fromUtf8("°"));
	m_pctrlUnit2 = new QLabel(QString::fromUtf8("°"));
	m_pctrlUnit3 = new QLabel(QString::fromUtf8("°"));

/*	m_pctrlAlphaMin->setMinimumHeight(20);
	m_pctrlAlphaMax->setMinimumHeight(20);
	m_pctrlAlphaDelta->setMinimumHeight(20);*/
	m_pctrlAlphaMin->setAlignment(Qt::AlignRight);
	m_pctrlAlphaMax->setAlignment(Qt::AlignRight);
	m_pctrlAlphaDelta->setAlignment(Qt::AlignRight);
	SequenceGroup->addWidget(AlphaMinLab,1,1);
	SequenceGroup->addWidget(AlphaMaxLab,2,1);
	SequenceGroup->addWidget(AlphaDeltaLab,3,1);
	SequenceGroup->addWidget(m_pctrlAlphaMin,1,2);
	SequenceGroup->addWidget(m_pctrlAlphaMax,2,2);
	SequenceGroup->addWidget(m_pctrlAlphaDelta,3,2);
	SequenceGroup->addWidget(m_pctrlUnit1,1,3);
	SequenceGroup->addWidget(m_pctrlUnit2,2,3);
	SequenceGroup->addWidget(m_pctrlUnit3,3,3);

	m_pctrlInitLLTCalc = new QCheckBox(tr("Init LLT"));
	m_pctrlStoreWOpp    = new QCheckBox(tr("Store OpPoint"));
	m_pctrlAnalyze     = new QPushButton(tr("Analyze"));

	QHBoxLayout *AnalysisSettings = new QHBoxLayout;
	AnalysisSettings->addWidget(m_pctrlInitLLTCalc);
	AnalysisSettings->addWidget(m_pctrlStoreWOpp);

	QVBoxLayout *AnalysisGroup = new QVBoxLayout;
	AnalysisGroup->addWidget(m_pctrlSequence);
	AnalysisGroup->addLayout(SequenceGroup);
        AnalysisGroup->addStretch(1);
	AnalysisGroup->addLayout(AnalysisSettings);
	AnalysisGroup->addWidget(m_pctrlAnalyze);

	QGroupBox *AnalysisBox = new QGroupBox(tr("Analysis settings"));
	AnalysisBox->setLayout(AnalysisGroup);

//_______________________Display
	QGridLayout *CheckDispLayout = new QGridLayout;
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
//	m_pctrlHighlightOpp   = new QCheckBox(tr("Highlight OpPoint"));


	m_pctrlAnimateWOppSpeed  = new QSlider(Qt::Horizontal);
	m_pctrlAnimateWOppSpeed->setMinimum(0);
	m_pctrlAnimateWOppSpeed->setMaximum(500);
	m_pctrlAnimateWOppSpeed->setSliderPosition(250);
	m_pctrlAnimateWOppSpeed->setTickInterval(50);
	m_pctrlAnimateWOppSpeed->setTickPosition(QSlider::TicksBelow);
	CheckDispLayout->addWidget(m_pctrlCp,       1,1);
	CheckDispLayout->addWidget(m_pctrlPanelForce, 1, 2);
	CheckDispLayout->addWidget(m_pctrlLift,     2, 1);
	CheckDispLayout->addWidget(m_pctrlMoment,   2, 2);
	CheckDispLayout->addWidget(m_pctrlIDrag,    3, 1);
	CheckDispLayout->addWidget(m_pctrlVDrag,    3, 2);
	CheckDispLayout->addWidget(m_pctrlTrans,    4, 1);
	CheckDispLayout->addWidget(m_pctrlDownwash, 4, 2);
	CheckDispLayout->addWidget(m_pctrlSurfVel,  5, 1);
	CheckDispLayout->addWidget(m_pctrlStream,   5, 2);
	CheckDispLayout->addWidget(m_pctrlWOppAnimate,  6, 1);
	CheckDispLayout->addWidget(m_pctrlAnimateWOppSpeed,6,2);

	QGroupBox *DisplayBox = new QGroupBox(tr("Results"));
	DisplayBox->setLayout(CheckDispLayout);

	QGroupBox *PolarPropsBox = new QGroupBox(tr("Polar properties"));
	m_pctrlPolarProps1 = new QTextEdit;
	m_pctrlPolarProps1->setReadOnly(true);
//	m_pctrlPolarProps1->setWordWrapMode(QTextOption::NoWrap);
	QHBoxLayout *PolarPropsLayout = new QHBoxLayout;
	PolarPropsLayout->addWidget(m_pctrlPolarProps1);
        PolarPropsLayout->addStretch(1);
	PolarPropsBox->setLayout(PolarPropsLayout);

//_______________________Curve params
	QVBoxLayout *CurveGroup = new QVBoxLayout;
	m_pctrlShowCurve  = new QCheckBox(tr("Curve"));
	m_pctrlShowPoints = new QCheckBox(tr("Points"));
//	m_pctrlShowCurve->setMinimumHeight(10);
//	m_pctrlShowPoints->setMinimumHeight(10);
	m_pctrlCurveStyle = new LineCbBox();
	m_pctrlCurveWidth = new LineCbBox();
	m_pctrlCurveColor = new LineButton;
	for (int i=0; i<5; i++)
	{
		m_pctrlCurveStyle->addItem(tr("item"));
		m_pctrlCurveWidth->addItem(tr("item"));
	}
	m_pStyleDelegate = new LineDelegate;
	m_pWidthDelegate = new LineDelegate;
	m_pctrlCurveStyle->setItemDelegate(m_pStyleDelegate);
	m_pctrlCurveWidth->setItemDelegate(m_pWidthDelegate);

	QHBoxLayout *ShowCurve = new QHBoxLayout;
	ShowCurve->addWidget(m_pctrlShowCurve);
	ShowCurve->addWidget(m_pctrlShowPoints);

	QGridLayout *CurveStyleLayout = new QGridLayout;
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

	CurveGroup->addLayout(ShowCurve);
	CurveGroup->addLayout(CurveStyleLayout);
        CurveGroup->addStretch(1);
	QGroupBox *CurveBox = new QGroupBox(tr("Curve settings"));
	CurveBox->setLayout(CurveGroup);

//_______________________Cp Params
	QVBoxLayout *CpParams = new QVBoxLayout;
	m_pctrlCpSectionSlider = new QSlider(Qt::Horizontal);
	m_pctrlCpSectionSlider->setMinimum(-100);
	m_pctrlCpSectionSlider->setMaximum(100);
	m_pctrlCpSectionSlider->setSliderPosition(00);
	m_pctrlCpSectionSlider->setTickInterval(10);
	m_pctrlCpSectionSlider->setTickPosition(QSlider::TicksBelow);
	QHBoxLayout *CpPos = new QHBoxLayout;
	QLabel *label1000 = new QLabel(tr("Span Position"));
	m_pctrlSpanPos = new FloatEdit(0.0, 3);
	CpPos->addWidget(label1000);
	CpPos->addWidget(m_pctrlSpanPos);
	QHBoxLayout *CpSections = new QHBoxLayout;
	m_pctrlKeepCpSection  = new QPushButton(tr("Keep"));
	m_pctrlResetCpSection = new QPushButton(tr("Reset"));
	CpSections->addWidget(m_pctrlKeepCpSection);
	CpSections->addWidget(m_pctrlResetCpSection);
	CpParams->addWidget(m_pctrlCpSectionSlider);
	CpParams->addLayout(CpPos);
	CpParams->addLayout(CpSections);
	CpParams->addStretch(1);
	QGroupBox *CpBox = new QGroupBox(tr("Cp Sections"));
	CpBox->setLayout(CpParams);


//_______________________3D view controls
	QVBoxLayout *ThreeDViewControls = new QVBoxLayout;
	QGridLayout *ThreeDParams = new QGridLayout;
	m_pctrlAxes       = new QCheckBox(tr("Axes"));
	m_pctrlLight      = new QCheckBox(tr("Light"));
	m_pctrlSurfaces   = new QCheckBox(tr("Surfaces"));
	m_pctrlOutline    = new QCheckBox(tr("Outline"));
	m_pctrlPanels     = new QCheckBox(tr("Panels"));
	m_pctrlFoilNames  = new QCheckBox(tr("Foil Names"));
	m_pctrlVortices   = new QCheckBox(tr("Vortices"));
	m_pctrlMasses     = new QCheckBox(tr("Masses"));

	ThreeDParams->addWidget(m_pctrlAxes, 1,1);
//	ThreeDParams->addWidget(m_pctrlLight, 1,2);
	ThreeDParams->addWidget(m_pctrlPanels, 1,2);
	ThreeDParams->addWidget(m_pctrlSurfaces, 2,1);
	ThreeDParams->addWidget(m_pctrlOutline, 2,2);
	ThreeDParams->addWidget(m_pctrlFoilNames, 3,1);
	ThreeDParams->addWidget(m_pctrlMasses, 3,2);

	QGridLayout *ThreeDView = new QGridLayout;
	m_pctrlX          = new QPushButton("X");
	m_pctrlY          = new QPushButton("Y");
	m_pctrlZ          = new QPushButton("Z");
	m_pctrlIso        = new QPushButton("Iso");


	ThreeDView->addWidget(m_pctrlX,1,1);
	ThreeDView->addWidget(m_pctrlY,1,2);
	ThreeDView->addWidget(m_pctrlZ,2,1);
	ThreeDView->addWidget(m_pctrlIso,2,2);

	m_pctrlPickCenter     = new QPushButton(tr("Pick Center"));
	m_pctrlPickCenter->setToolTip(tr("Activate the button, then click on the object to center it in the viewport; alternatively, double click on the object"));
	m_pctrlReset          = new QPushButton(tr("Reset"));
	m_pctrlPickCenter->setCheckable(true);

	ThreeDView->addWidget(m_pctrlReset,3,1);
	ThreeDView->addWidget(m_pctrlPickCenter,3,2);

	QHBoxLayout *ClipLayout = new QHBoxLayout;
	QLabel *ClipLabel = new QLabel(tr("Clip:"));
	m_pctrlClipPlanePos = new QSlider(Qt::Horizontal);
	m_pctrlClipPlanePos->setMinimum(-300);
	m_pctrlClipPlanePos->setMaximum(300);
	m_pctrlClipPlanePos->setSliderPosition(0);
	m_pctrlClipPlanePos->setTickInterval(30);
	m_pctrlClipPlanePos->setTickPosition(QSlider::TicksBelow);
	ClipLayout->addWidget(ClipLabel);
	ClipLayout->addWidget(m_pctrlClipPlanePos,1);

	ThreeDViewControls->addLayout(ThreeDParams);
	ThreeDViewControls->addLayout(ThreeDView);
	ThreeDViewControls->addLayout(ClipLayout);
	ThreeDViewControls->addStretch(1);
	QGroupBox *ThreeDViewBox = new QGroupBox(tr("Display"));
	ThreeDViewBox->setLayout(ThreeDViewControls);

//_________________________Main Layout
	m_pctrlMiddleControls = new QStackedWidget;
	m_pctrlMiddleControls->addWidget(DisplayBox);
	m_pctrlMiddleControls->addWidget(PolarPropsBox);
	m_pctrlMiddleControls->addWidget(CpBox);

	m_pctrBottomControls = new QStackedWidget;
	m_pctrBottomControls->addWidget(CurveBox);
	m_pctrBottomControls->addWidget(ThreeDViewBox);

	QVBoxLayout *mainLayout = new QVBoxLayout;
        //mainLayout->addStretch(1);
	mainLayout->addWidget(AnalysisBox);
        //mainLayout->addStretch(1);
	mainLayout->addWidget(m_pctrlMiddleControls);
        //mainLayout->addStretch(1);
	mainLayout->addWidget(m_pctrBottomControls);
        //mainLayout->addStretch(1);

	setLayout(mainLayout);
}




void QMiarex::SetWGraphScale()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(!m_pCurWing)
	{
		for(int ig=0; ig<4; ig++)
		{
			m_WingGraph[ig].SetAuto(false);
			m_WingGraph[ig].SetXUnit(10.0);
			m_WingGraph[ig].SetXMin(-100.0);
			m_WingGraph[ig].SetXMax( 100.0);
		}
	}
	else
	{
		double halfspan = m_pCurWing->m_PlanformSpan/2.0;
		double xmin;
		if(m_bHalfWing) xmin = 0.0;
		else            xmin = -halfspan*pMainFrame->m_mtoUnit;

		for(int ig=0; ig<4; ig++)
		{
			m_WingGraph[ig].SetAutoX(false);
			m_WingGraph[ig].SetXMin(xmin);
			m_WingGraph[ig].SetXMax( halfspan*pMainFrame->m_mtoUnit);
			m_WingGraph[ig].SetAutoXUnit();
		}
	}
}


void QMiarex::SetWPlr(bool bCurrent, QString WPlrName)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	CWPolar *pWPolar = NULL;
	CWPolar *pOldWPolar = NULL;
	QString UFOName;

	int i,j,k,m, NStation;
	double SpanPos;


	if(m_pCurPlane)     UFOName = m_pCurPlane->PlaneName();
	else if(m_pCurWing) UFOName = m_pCurWing->WingName();
	else return;

	if(!WPlrName.length() && m_pCurWPolar)	WPlrName = m_pCurWPolar->m_PlrName;

	if(bCurrent) pWPolar = m_pCurWPolar;
	else
	{
		for (i=0; i<m_poaWPolar->size(); i++)
		{
			pOldWPolar = (CWPolar*) m_poaWPolar->at(i);
			if (pOldWPolar->m_UFOName == UFOName &&	pOldWPolar->m_PlrName == WPlrName)
			{
				pWPolar = pOldWPolar;
				break;
			}
		}
	}

	if(pWPolar && pWPolar == m_pCurWPolar) SetWOpp(true);
	else
	{
		m_bResetglMesh = true;
		if(!m_pCurWing)
		{
			pWPolar = NULL;
			m_pCurWPolar = NULL;
		}

		if(pWPolar) m_pCurWPolar = pWPolar;
		else if(m_pCurWing)
		{
			// set the first one in the array, if any
			m_pCurWPolar = NULL;
			for(i=0; i< m_poaWPolar->size(); i++)
			{
				pWPolar = (CWPolar*)m_poaWPolar->at(i);
				if(pWPolar && pWPolar->m_UFOName==UFOName)
				{
					m_pCurWPolar = pWPolar;
					break;
				}
			}
		}
	}

	if(m_pCurWPolar)
	{
		int pos = pMainFrame->m_pctrlWPolar->findText(m_pCurWPolar->m_PlrName);
		if (pos>=0)		pMainFrame->m_pctrlWPolar->setCurrentIndex(pos);
	}
	InitializePanels();


	//set sideslip
	CVector RefPoint(0.0, 0.0, 0.0);
	if(m_pCurWPolar && fabs(m_pCurWPolar->m_Beta)>0.001)
	{
		RotateGeomZ(m_MemPanel, m_MemNode, m_WakePanel, m_WakeNode, m_pCurWPolar->m_Beta, RefPoint);
	}

	if(m_pCurWPolar && m_pCurWPolar->m_UFOName==UFOName)
	{
		if(m_pCurWPolar->m_AnalysisMethod>1)
		{
			for(int iw=0; iw<MAXWINGS; iw++)
			{
				if(m_pWingList[iw])
				{
					m_pWingList[iw]->ComputeChords();

					NStation = 0;
					m=0;
					SpanPos = 0;
					for (j=0; j<m_pWingList[iw]->m_NSurfaces; j++)	NStation += m_pWingList[iw]->m_Surface[j].m_NYPanels;

					for (j=(int)(m_pWingList[iw]->m_NSurfaces/2); j<m_pWingList[iw]->m_NSurfaces; j++)
					{
						for(k=0; k<m_pWingList[iw]->m_Surface[j].m_NYPanels; k++)
						{
							m_pWingList[iw]->m_SpanPos[m+NStation/2] = SpanPos + m_pWingList[iw]->m_Surface[j].GetStripSpanPos(k);
							m++;
						}
						SpanPos += m_pWingList[iw]->m_Surface[j].m_Length;
					}

					for(m=0; m<NStation/2; m++) m_pWingList[iw]->m_SpanPos[m] = -m_pWingList[iw]->m_SpanPos[NStation-m-1];
				}
			}
		}
		else if(m_pCurWPolar->m_AnalysisMethod==LLTMETHOD)
		{
//			m_pCurWing->m_NStation  = m_NStation;
//			m_pCurWing->m_bLLT      = true;
		}

		pMainFrame->UpdateWOpps();

		double Alpha = 0.0;
		if(m_pCurWOpp) Alpha = m_pCurWOpp->m_Alpha;

		if(m_pCurPlane)     SetPOpp(false, Alpha);
		else if(m_pCurWing) SetWOpp(false, Alpha);

/*		if (m_pCurPOpp){
			// try to set the same as the existing WOpp... Special for Marc
			if(!SetPOpp(false, m_pCurPOpp->m_Alpha))
				SetPOpp(true);// try again from scratch
		}
		else if (m_pCurWOpp){
			// try to set the same as the existing WOpp... Special for Marc
			if(!SetWOpp(false, m_pCurWOpp->m_Alpha))
				SetWOpp(true);// try again from scratch
		}
		else
			SetWOpp(true);*/
	}
	else
	{
		m_pCurWPolar = NULL;
		m_pCurPOpp = NULL;
		m_pCurWOpp = NULL;
		pMainFrame->UpdateWOpps();
	}

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
			else if(m_pCurWing)
			{
				m_pCurWPolar->m_Mass = m_pCurWing->TotalMass();
				m_pCurWPolar->m_CoG    = m_pCurWing->m_CoG;
				m_pCurWPolar->m_CoGIxx = m_pCurWing->m_CoGIxx;
				m_pCurWPolar->m_CoGIyy = m_pCurWing->m_CoGIyy;
				m_pCurWPolar->m_CoGIzz = m_pCurWing->m_CoGIzz;
				m_pCurWPolar->m_CoGIxz = m_pCurWing->m_CoGIxz;

			}

			QString PolarProps;
			m_pCurWPolar->GetPolarProperties(PolarProps);
			m_pctrlPolarProps1->setText(PolarProps);
		}
	}
	else m_pctrlPolarProps1->clear();

	if(m_iView==WPOLARVIEW)		CreateWPolarCurves();
	else if(m_iView==WSTABVIEW)	CreateStabilityCurves();
	else if(m_iView==WOPPVIEW)	CreateWOppCurves();
	else if(m_iView==WCPVIEW)	CreateCpCurves();


	SetAnalysisParams();
	SetCurveParams();
	m_bResetglLegend = true;

	m_pPanelDlg->m_pWPolar     = m_pCurWPolar;
	m_pPanelDlg->m_MatSize     = m_MatSize;
	m_pPanelDlg->m_WakeSize    = m_WakeSize;
	m_pPanelDlg->m_nNodes      = m_nNodes;
	m_pPanelDlg->m_NSurfaces   = m_NSurfaces;
	m_pPanelDlg->m_NWakeColumn = m_NWakeColumn;

	if(m_pCurWPolar) pMainFrame->m_pctrlStabViewWidget->setVisible(m_pCurWPolar->m_Type==STABILITYPOLAR);

	QApplication::restoreOverrideCursor();
}



void QMiarex::SetWingLegendPos()
{
	int h  = m_r2DCltRect.height();
	int h23 = (int)(2*h/3);
	int h34 = (int)(3*h/4);

	int margin = 10;
	if(m_iView==WOPPVIEW)
	{
		if(m_iWingView == 1)
		{
			m_WingLegendOffset.rx() = m_r2DCltRect.right()-300+margin;
			m_WingLegendOffset.ry() = 0;
		}
		else if(m_iWingView == 2)
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
	else if(m_iView==WSTABVIEW)
	{
		if(m_iStabTimeView < 4)
		{
			m_WingLegendOffset.rx() = margin;
			m_WingLegendOffset.ry() = h34+margin;
		}
		else if(m_iStabTimeView ==4)
		{
			m_WingLegendOffset.rx() = margin;
			m_WingLegendOffset.ry() = h34+margin;
		}		
	}
}


void QMiarex::SetWPlrLegendPos()
{
	int h  = m_r2DCltRect.height();
	int w  = m_r2DCltRect.width();
	int h23 = (int)(2*h/3);

	int margin = 10;
	if(m_iView==WPOLARVIEW)
	{
		if(m_iWPlrView == 1)
		{
			m_WPlrLegendOffset.rx() = (int)(0.70*w)+margin;
			m_WPlrLegendOffset.ry() = margin;
		}
		else if (m_iWPlrView == 2)
		{
			m_WPlrLegendOffset.rx() = margin;
			m_WPlrLegendOffset.ry() = h23+margin;
		}
		else if	(m_iWPlrView == 4)
		{
			m_WPlrLegendOffset.rx() = (int)(0.70*w)+margin;
			m_WPlrLegendOffset.ry() = margin;
		}
	}
	else if(m_iView==WSTABVIEW)
	{
		if(m_iStabilityView==STABPOLARVIEW)
		{
			m_WPlrLegendOffset.rx() = (int)(0.70*w)+margin;
			m_WPlrLegendOffset.ry() = margin;
			
		}

	}
}


bool QMiarex::SetWOpp(bool bCurrent, double x)
{
	m_bResetglMesh   = true;
	m_bResetglOpp    = true;
	m_bResetglWake   = true;
	m_bResetglStream = true;
	m_bResetglFlow   = true;
	m_bResetglLegend = true;

	// first restore the panel geometry
	memcpy(&m_Panel, &m_MemPanel, m_MatSize* sizeof(CPanel));
	memcpy(&m_Node,  &m_MemNode,  m_nNodes * sizeof(CVector));

	if(!m_pCurWing || !m_pCurWPolar)
	{
		m_pCurWOpp = NULL;
		m_pCurPOpp = NULL;
		m_pWOpp[0] = m_pWOpp[1] = m_pWOpp[2] = m_pWOpp[3] = NULL;
		SetCurveParams();
		return false;
	}

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	// set set the current WOpp, if any
	// else set the comboBox's first, if any
	// else set it to NULL
	QString strong;
	if(m_pCurPlane)	  return SetPOpp(bCurrent, x);

	CWOpp *pWOpp = NULL;
	if(bCurrent)
	{
		pWOpp = m_pCurWOpp;
		if(pWOpp)
		{
			//Check it is consistent with wing and polar
			if(pWOpp->m_WingName != m_pCurWing->WingName() || pWOpp->m_PlrName!=m_pCurWPolar->m_PlrName) pWOpp=NULL;
		}
	}
	else pWOpp = GetWOpp(x);


	if(!pWOpp)
	{
	//first try to set the last alpha which has been selected
		if(m_pCurWPolar)
			pWOpp = GetWOpp(m_pCurWPolar->m_AMem);
		else
			pWOpp = GetWOpp(m_LastWOpp);
	}

	if(!pWOpp)
	{
		//try to select the first in the ListBox
		if(pMainFrame->m_pctrlWOpp->count())
		{
			double x;
			pMainFrame->m_pctrlWOpp->setCurrentIndex(0);
			strong = pMainFrame->m_pctrlWOpp->itemText(0);
			bool bOK;
			x = strong.toDouble(&bOK);

			if(bOK)
			{
				pWOpp = GetWOpp(x);
			}
			else pWOpp = NULL;
		}
		else
		{
			pWOpp = NULL;
		}
	}
	m_pCurWOpp = pWOpp;
	m_pWOpp[0] = pWOpp;
	m_pWOpp[1] = m_pWOpp[2] = m_pWOpp[3] = NULL;

	SetCurveParams();
	if(m_pCurWOpp)
	{
		m_LastWOpp = m_pCurWOpp->m_Alpha;
		m_pCurWPolar->m_AMem = m_pCurWOpp->m_Alpha;
//		m_nWakeNodes   = m_pCurWOpp->m_nWakeNodes;
//		m_NXWakePanels = m_pCurWOpp->m_NXWakePanels;
//		m_TotalWakeLength  =		m_pCurWOpp->m_FirstWakePanel;
//		m_WakePanelFactor =		m_pCurWOpp->m_WakeFactor;

		//select m_pCurWOpp in the listbox
		if(m_pCurWPolar->m_Type < 4)                    pMainFrame->SelectWOpp(m_pCurWOpp->m_Alpha);
		else if(m_pCurWPolar->m_Type == FIXEDAOAPOLAR)  pMainFrame->SelectWOpp(m_pCurWOpp->m_QInf);
		else if(m_pCurWPolar->m_Type == STABILITYPOLAR) pMainFrame->SelectWOpp(m_pCurWOpp->m_Ctrl);

		//if we have a type 7 polar, set the panels in the control's position
		if(m_pCurWPolar && m_pCurWPolar->m_Type==STABILITYPOLAR)
		{
			//set the controls
			m_pPanelDlg->m_pWPolar        = m_pCurWPolar;
			m_pPanelDlg->m_pPlane         = m_pCurPlane;
			for(int iw=0; iw<MAXWINGS; iw++) m_pPanelDlg->m_pWingList[iw] = m_pWingList[iw];
			m_pPanelDlg->m_pBody          = m_pCurBody;
			int  nCtrls;
			QString strong;
			SetControlPositions(m_Panel, m_Node, m_pCurWOpp->m_Ctrl, nCtrls, strong, false);
			m_bResetglMesh = true;
		}
	}

	if(m_iView==WPOLARVIEW)
	{
		if(m_bHighlightOpp) CreateWPolarCurves();
	}
	else if(m_iView==WSTABVIEW)
	{
		StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
		pStabView->SetControls();
		pStabView->SetMode();
		CreateStabilityCurves();
	}
	else if(m_iView==WOPPVIEW)	CreateWOppCurves();
	else if(m_iView==WCPVIEW)	CreateCpCurves();


	if(!m_pCurWOpp) return false;
	else  if(m_iView==WOPPVIEW)
	{
		m_bCurveVisible = m_pCurWOpp->m_bIsVisible;
		m_bCurvePoints  = m_pCurWOpp->m_bShowPoints;
	}
	return true;
}


void QMiarex::SetStabGraphTitles()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString strLength;
	GetSpeedUnit(strLength, pMainFrame->m_SpeedUnit);

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


void QMiarex::SetWGraphTitles(Graph* pGraph)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString StrLength;
	QString StrSpeed;
	QString StrMoment;
	GetLengthUnit(StrLength, pMainFrame->m_LengthUnit);
	GetSpeedUnit(StrSpeed, pMainFrame->m_SpeedUnit);
	GetMomentUnit(StrMoment, pMainFrame->m_MomentUnit);

	switch (pGraph->GetXVariable())
	{
		case 0:
			pGraph->SetXTitle(tr("Alpha"));
			break;
		case 1:
			pGraph->SetXTitle(tr("CL"));
			break;
		case 2:
			pGraph->SetXTitle(tr("VCD"));
			break;
		case 3:
			pGraph->SetXTitle(tr("ICD"));
			break;
		case 4:
			pGraph->SetXTitle(tr("CD"));
			break;
		case 5:
			pGraph->SetXTitle(tr("Cm"));// Total Pitching moment coef.
			break;
		case 6:
			pGraph->SetXTitle(tr("VCm"));// Viscous Pitching moment coef.
			break;
		case 7:
			pGraph->SetXTitle(tr("ICm"));// Induced Pitching moment coef.
			break;
		case 8:
			pGraph->SetXTitle(tr("Cl"));// Total Rolling moment coef.
			break;
		case 9:
			pGraph->SetXTitle(tr("Cn"));// Total Yawing moment coef.
			break;
		case 10:
			pGraph->SetXTitle(tr("VCn"));// Profile yawing moment
			break;
		case 11:
			pGraph->SetXTitle(tr("ICn"));// Induced yawing moment
			break;
		case 12:
			pGraph->SetXTitle(tr("CL/CD"));
			break;
		case 13:
			pGraph->SetXTitle(tr("CL^(3/2)/CD"));
			break;
		case 14:
			pGraph->SetXTitle(tr("1/Rt(CL)"));
			break;
		case 15:
			if(pMainFrame->m_ForceUnit==0)	pGraph->SetXTitle(tr("Lift (N)"));
			else                            pGraph->SetXTitle(tr("Lift (lbf)"));
			break;
		case 16:
			if(pMainFrame->m_ForceUnit==0)	pGraph->SetXTitle(tr("Drag (N)"));
			else                            pGraph->SetXTitle(tr("Drag (lbf)"));
			break;
		case 17:
			pGraph->SetXTitle(tr("Vx ")+StrSpeed);
			break;
		case 18:
			pGraph->SetXTitle(tr("Vz ")+StrSpeed);
			break;
		case 19:
			pGraph->SetXTitle(tr("V ")+StrSpeed);
			break;
		case 20:
			pGraph->SetXTitle(tr("Gamma"));
			break;
		case 21:
			pGraph->SetXTitle(tr("Pitching Moment"));
			break;
		case 22:
			pGraph->SetXTitle(tr("Rolling Moment"));
			break;
		case 23:
			pGraph->SetXTitle(tr("Yawing Moment"));
			break;
		case 24:
			pGraph->SetXTitle(tr("XCP ")+ StrLength);
			break;
		case 25:
			pGraph->SetXTitle(tr("YCP ")+ StrLength);
			break;
		case 26:
			pGraph->SetXTitle(tr("BM ")+ StrMoment);
			break;
		case 27:
			pGraph->SetXTitle(tr("m.g.Vz (W)"));
			break;
		case 28:
			pGraph->SetXTitle(tr("Efficiency"));
			break;
		case 29:
			pGraph->SetXTitle(tr("(XCp-XCG)/MAC(%)"));
			break;
		case 30:
			pGraph->SetXTitle(tr("ctrl"));
			break;
		case 31:
			pGraph->SetXTitle(tr("CY"));
			break;
		case 32:
			pGraph->SetXTitle(tr("XNP"));
			break;
		case 33:
			pGraph->SetXTitle(tr("Ph. Freq(Hz)"));
			break;
		case 34:
			pGraph->SetXTitle(tr("Ph. Damping"));
			break;
		case 35:
			pGraph->SetXTitle(tr("SP Freq (Hz)"));
			break;
		case 36:
			pGraph->SetXTitle(tr("SP Damping"));
			break;
		case 37:
			pGraph->SetXTitle(tr("DR Freq(Hz)"));
			break;
		case 38:
			pGraph->SetXTitle(tr("DR Damping"));
			break;
		case 39:
			pGraph->SetXTitle(tr("Roll Damping"));
			break;
		case 40:
			pGraph->SetXTitle(tr("Spiral Damping"));
			break;
		default:
			pGraph->SetXTitle(tr("Alpha"));
			break;
	}

	switch (pGraph->GetYVariable())
	{
		case 0:
			pGraph->SetYTitle(tr("Alpha"));
			break;
		case 1:
			pGraph->SetYTitle(tr("CL"));
			break;
		case 2:
			pGraph->SetYTitle(tr("VCD"));
			break;
		case 3:
			pGraph->SetYTitle(tr("ICD"));
			break;
		case 4:
			pGraph->SetYTitle(tr("CD"));
			break;
		case 5:
			pGraph->SetYTitle(tr("Cm"));// Total Pitching moment coef.
			break;
		case 6:
			pGraph->SetYTitle(tr("VCm"));// Viscous Pitching moment coef.
			break;
		case 7:
			pGraph->SetYTitle(tr("ICm"));// Induced Pitching moment coef.
			break;
		case 8:
			pGraph->SetYTitle(tr("Cl"));// Total Rolling moment coef.
			break;
		case 9:
			pGraph->SetYTitle(tr("Cn"));// Total Yawing moment coef.
			break;
		case 10:
			pGraph->SetYTitle(tr("VCn"));// Profile yawing moment
			break;
		case 11:
			pGraph->SetYTitle(tr("ICn"));// Induced yawing moment
			break;
		case 12:
			pGraph->SetYTitle(tr("CL/CD"));
			break;
		case 13:
			pGraph->SetYTitle(tr("CL^(3/2)/CD"));
			break;
		case 14:
			pGraph->SetYTitle(tr("1/Rt(CL)"));
			break;
		case 15:
			if(pMainFrame->m_ForceUnit==0)	pGraph->SetYTitle(tr("Lift (N)"));
			else                            pGraph->SetYTitle(tr("Lift (lbf)"));
			break;
		case 16:
			if(pMainFrame->m_ForceUnit==0)	pGraph->SetYTitle(tr("Drag (N)"));
			else                            pGraph->SetYTitle(tr("Drag (lbf)"));
			break;
		case 17:
			pGraph->SetYTitle(tr("Vx ")+StrSpeed);
			break;
		case 18:
			pGraph->SetYTitle(tr("Vz ")+StrSpeed);
			break;
		case 19:
			pGraph->SetYTitle(tr("V ")+StrSpeed);
			break;
		case 20:
			pGraph->SetYTitle(tr("Gamma"));
			break;
		case 21:
			pGraph->SetYTitle(tr("Pitching Moment"));
			break;
		case 22:
			pGraph->SetYTitle(tr("Rolling Moment"));
			break;
		case 23:
			pGraph->SetYTitle(tr("Yawing Moment"));
			break;
		case 24:
			pGraph->SetYTitle(tr("XCP ")+ StrLength);
			break;
		case 25:
			pGraph->SetYTitle(tr("YCP ")+ StrLength);
			break;
		case 26:
			pGraph->SetYTitle(tr("BM ")+ StrMoment);
			break;
		case 27:
			pGraph->SetYTitle(tr("m.g.Vz (W)"));
			break;
		case 28:
			pGraph->SetYTitle(tr("Efficiency"));
			break;
		case 29:
			pGraph->SetYTitle(tr("(XCp-XCG)/MAC(%)"));
			break;
		case 30:
			pGraph->SetYTitle(tr("ctrl"));
			break;
		case 31:
			pGraph->SetYTitle(tr("CY"));
			break;
		case 32:
			pGraph->SetYTitle(tr("XNP"));
			break;
		case 33:
			pGraph->SetYTitle(tr("Ph. Freq(Hz)"));
			break;
		case 34:
			pGraph->SetYTitle(tr("Ph. Damping"));
			break;
		case 35:
			pGraph->SetYTitle(tr("SP Freq (Hz)"));
			break;
		case 36:
			pGraph->SetYTitle(tr("SP Damping"));
			break;
		case 37:
			pGraph->SetYTitle(tr("DR Freq(Hz)"));
			break;
		case 38:
			pGraph->SetYTitle(tr("DR Damping"));
			break;
		case 39:
			pGraph->SetYTitle(tr("Roll Damping"));
			break;
		case 40:
			pGraph->SetYTitle(tr("Spiral Damping"));
			break;
		default:
			pGraph->SetYTitle(tr("Alpha"));
			break;
	}
}


void QMiarex::showEvent(QShowEvent *event)
{
	SetAnalysisParams();
	SetCurveParams();
}




void QMiarex::SnapClient(QString const &FileName)
{
	int NbBytes, bitsPerPixel;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	QSize size(m_r3DCltRect.width(),m_r3DCltRect.height());
	GLWidget * pGLWidget = (GLWidget*)s_pGLWidget;
	QGLFormat GLFormat = pGLWidget->format();

	if(!GLFormat.rgba())
	{
		QMessageBox::warning(pMainFrame,tr("Warning"),tr("OpenGL color format is not recognized... Sorry"));
		return;
	}

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
	if(m_iView!=WSTABVIEW)
	{
		if(m_pCurPlane)
		{
			SetPOpp(true);
		}
		else if(m_pCurWing)
		{
			SetWOpp(true);
		}
	}
}



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
		CreateWPolarCurves();
	}
	else if(m_iView==WSTABVIEW && m_pCurWPolar)
	{
		if(m_iStabilityView==STABTIMEVIEW)
		{
			StabViewDlg *pStabView = (StabViewDlg*)pMainFrame->m_pStabView;
			pStabView->SetTimeCurveStyle(m_CurveColor, m_CurveStyle, m_CurveWidth, m_bCurveVisible, m_bCurvePoints);
		}
		else if(m_iStabilityView==STABPOLARVIEW)
		{
			m_pCurWPolar->m_Color = m_CurveColor;
			m_pCurWPolar->m_Style = m_CurveStyle;
			m_pCurWPolar->m_Width = m_CurveWidth;
			m_pCurWPolar->m_bIsVisible  = m_bCurveVisible;
			m_pCurWPolar->m_bShowPoints = m_bCurvePoints;
			CreateStabilityCurves();
		}
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
			m_pCurWOpp->m_Color = m_CurveColor;
			m_pCurWOpp->m_Style = m_CurveStyle;
			m_pCurWOpp->m_Width = m_CurveWidth;
			m_pCurWOpp->m_bIsVisible  = m_bCurveVisible;
			m_pCurWOpp->m_bShowPoints = m_bCurvePoints;
		}
		else if(m_pCurWOpp)
		{
			m_pCurWOpp->m_Color = m_CurveColor;
			m_pCurWOpp->m_Style = m_CurveStyle;
			m_pCurWOpp->m_Width = m_CurveWidth;
			m_pCurWOpp->m_bIsVisible  = m_bCurveVisible;
			m_pCurWOpp->m_bShowPoints = m_bCurvePoints;
		}
		CreateWOppCurves();
	}
	else if (m_iView==WCPVIEW && m_pCurWOpp)
	{
		m_CpColor = m_CurveColor;
		m_CpStyle = m_CurveStyle;
		m_CpWidth = m_CurveWidth;
		m_bShowCp       = m_bCurveVisible;
		m_bShowCpPoints = m_bCurvePoints;
		CreateCpCurves();
	}
	UpdateView();
	pMainFrame->SetSaveState(false);
}



void QMiarex::UpdateUnits()
{
	//The units have been changed, so update the graphs and the views
	if(m_iView==WPOLARVIEW)
	{
		CreateWPolarCurves();
		for(int ig=0; ig<4; ig++)
			SetWGraphTitles(m_WPlrGraph+ig);
	}
	else if(m_iView==WSTABVIEW)
	{
		CreateStabilityCurves();
		SetStabGraphTitles();
	}
	else
	{
		if(!m_pCurWing) return;
		if (m_iView==WOPPVIEW)
		{
			CreateWOppCurves();
			OnAdjustToWing();
		}
		else if(m_iView==WCPVIEW) CreateCpCurves();
		else if(m_iView==W3DVIEW) m_bResetglLegend = true;
		else if(m_iView==WSTABVIEW) m_bResetglLegend = true;
	}
	SetCurveParams();
	UpdateView();
}



void QMiarex::UpdateView()
{
	TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;
	GLWidget * pGLWidget = (GLWidget*)s_pGLWidget;
	if(m_iView==W3DVIEW || (m_iView==WSTABVIEW && m_iStabilityView==STAB3DVIEW))
	{
		if(pGLWidget) pGLWidget->updateGL();
	}
	else
	{
		if(p2DWidget) p2DWidget->update();
	}
}



bool QMiarex::VLMIsSameSide(int p, int pp)
{
	if(m_Panel[p].m_iPos==m_Panel[pp].m_iPos) return true;
	return false;
}


void QMiarex::wheelEvent(QWheelEvent *event)
{
	//The mouse button has been wheeled
	//Process the message
	MainFrame * pMainFrame = (MainFrame*)s_pMainFrame;
	QPoint pt(event->x(), event->y()); //client coordinates
	static double ZoomFactor;
	if(event->delta()>0)
	{
		if(!pMainFrame->m_bReverseZoom) ZoomFactor = 1./1.06;
		else                            ZoomFactor = 1.06;
	}
	else
	{
		if(!pMainFrame->m_bReverseZoom) ZoomFactor = 1.06;
		else                           ZoomFactor = 1./1.06;
	}

	if(m_iView==W3DVIEW || (m_iView==WSTABVIEW&&m_iStabilityView==STAB3DVIEW))
	{
		if(m_pCurWing)
		{
			//zoom wing
			m_glScaled *= ZoomFactor;
			UpdateView();
		}
	}
	else if(m_iView ==WOPPVIEW || m_iView ==WPOLARVIEW || m_iView ==WCPVIEW || m_iView ==WSTABVIEW)
	{
		m_pCurGraph = GetGraph(pt);
		if(m_pCurGraph && m_pCurGraph->IsInDrawRect(pt))
		{
			if (m_bXPressed)
			{
				//zoom x scale
				m_pCurGraph->SetAutoX(false);
				m_pCurGraph->Scalex(1./ZoomFactor);
			}
			else if(m_bYPressed)
			{
				//zoom y scale
				m_pCurGraph->SetAutoY(false);
				m_pCurGraph->Scaley(1./ZoomFactor);
			}
			else
			{
				//zoom both
				m_pCurGraph->SetAuto(false);
				m_pCurGraph->Scale(1./ZoomFactor);
			}

			m_pCurGraph->SetAutoXUnit();
			m_pCurGraph->SetAutoYUnit();
			UpdateView();

		}
		else if(m_pCurWing && m_iView==WOPPVIEW)
		{
			m_WingScale *= ZoomFactor;
			UpdateView();
		}
	}
}




void QMiarex::OnWPolarProps()
{
	if(!m_pCurWPolar) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	ObjectPropsDlg dlg;
	dlg.m_pMiarex = this;
	dlg.m_pWPolar = m_pCurWPolar;
	dlg.InitDialog();
	dlg.move(pMainFrame->m_DlgPos);
	dlg.exec();
	pMainFrame->m_DlgPos = dlg.pos();
}


void QMiarex::OnWOppProps()
{
	if(!m_pCurWOpp) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	ObjectPropsDlg dlg;
	dlg.m_pMiarex = this;
	dlg.m_pWOpp = m_pCurWOpp;
	dlg.InitDialog();
	dlg.move(pMainFrame->m_DlgPos);
	dlg.exec();
	pMainFrame->m_DlgPos = dlg.pos();
}


void QMiarex::SetControlPositions(CPanel *pPanel, CVector *pNode, double t, int &NCtrls, QString &out, bool bBCOnly)
{
	// Modifies the panels and nodes by setting the control positions to the specified position t
	// The panels and nodes to be rotated are pointed by pPanel and pNode
	// if bBCOnly, then only the control points and normal vector and rotates
	// if not,     the whole geometry is rotated
	if(!m_pCurWPolar) return;
	if(!m_pCurPlane && ! m_pCurWing) return;

	QString strange;
	Quaternion Quat;
	int j, nFlap;
	double angle, TotalAngle;
	CWing *pWing;
	CVector YVector(0.0, 1.0, 0.0);
	CVector W;

	// update the variables & geometry
	// if plane : WingTilt, elevator Tilt
	// if flaps : wing flaps, elevator flaps

	//the CG position is fixed for this analysis

	NCtrls = 0;

	if(m_pCurPlane)
	{
		//wing incidence
		if((!m_pCurWPolar->m_bAVLControls&&m_pCurWPolar->m_bActiveControl[0]) ||
		   ( m_pCurWPolar->m_bAVLControls&&m_pCurWPolar->m_MaxControl[0]))
		{
			//wing tilt
			if(m_pCurWPolar->m_bAVLControls)
			{
				angle = m_pCurWPolar->m_MaxControl[0] * t; //maxcontrol is the gain
					TotalAngle = m_pCurPlane->WingTiltAngle(0) + angle;
				strange = QString::fromUtf8("         Rotating the wing by %1°, total angle is %2°").arg(angle, 5, 'f',2).arg(TotalAngle, 5, 'f',2);
			}
			else
			{
				angle = m_pCurWPolar->m_MinControl[0] + t * (m_pCurWPolar->m_MaxControl[0] - m_pCurWPolar->m_MinControl[0]);
				strange = QString("         Setting the wing tilt to %1").arg(angle, 5, 'f',2);
				angle -= m_pCurPlane->WingTiltAngle(0);//cancel initial tilt set in plane definition
			}
			strange += "\n";
			out +=strange;

			Quat.Set(angle, YVector);

			if(bBCOnly)
			{
				for(int p=0; p<m_pCurPlane->Wing()->m_MatSize; p++)
				{
					memcpy(pPanel+p, m_MemPanel+p, sizeof(CPanel));
					(pPanel+p)->RotateBC(m_pCurPlane->WingLE(0), Quat);
				}
			}
			else
			{
				for(int n=0; n<m_nNodes; n++)
				{
					if(m_pCurPlane->Wing()->IsWingNode(n))
					{
							pNode[n].Copy(m_MemNode[n]);
							W = pNode[n] - m_pCurPlane->WingLE(0);
							Quat.Conjugate(W);
							pNode[n] = W + m_pCurPlane->WingLE(0);
					}
				}
				for(int p=0; p<m_MatSize; p++)
				{
					if(m_pCurPlane->Wing()->IsWingPanel(p)) m_Panel[p].SetFrame();
				}
			}
		}
		NCtrls=1;

		if(m_pCurPlane->Stab())
		{
			//elevator incidence
			if((!m_pCurWPolar->m_bAVLControls&&m_pCurWPolar->m_bActiveControl[1]) ||
			   ( m_pCurWPolar->m_bAVLControls&&m_pCurWPolar->m_MaxControl[1]))
			{
				//Elevator tilt
				if(m_pCurWPolar->m_bAVLControls)
				{
					angle = m_pCurWPolar->m_MaxControl[1] * t; //maxcontrol is the gain
					TotalAngle = m_pCurPlane->WingTiltAngle(2) + angle;
					strange = QString::fromUtf8("         Rotating the elevator by %1°, total angle is %2°").arg(angle, 5, 'f',2).arg(TotalAngle, 5, 'f',2);
				}
				else
				{
					angle = m_pCurWPolar->m_MinControl[1] + t * (m_pCurWPolar->m_MaxControl[1] - m_pCurWPolar->m_MinControl[1]);
					strange = QString("         Setting the elevator tilt to %1").arg(angle, 5, 'f',2);
					angle -= m_pCurPlane->WingTiltAngle(2);//cancel initial tilt set in plane definition
				}

				strange += "\n";
				out +=strange;

				Quat.Set(angle, YVector);
				if(!bBCOnly)
				{
					for(int n=0; n<m_nNodes; n++)
					{
						if(m_pCurPlane->Stab()->IsWingNode(n))
						{
								pNode[n].Copy(m_MemNode[n]);
								W = pNode[n] - m_pCurPlane->WingLE(2);
								Quat.Conjugate(W);
								pNode[n] = W + m_pCurPlane->WingLE(2);
						}
					}
					for(int p=0; p<m_MatSize; p++)
					{
						if(m_pWingList[2]->IsWingPanel(p)) m_Panel[p].SetFrame();
					}
				}
				else
				{
					for(int p=0; p<m_pCurPlane->Stab()->m_MatSize; p++)
					{
						m_pCurPlane->Stab()->m_pPanel[p].RotateBC(m_pCurPlane->WingLE(2), Quat);
					}
				}
			}
			NCtrls = 2;
		}
	}

	// flap controls
	for(int iw=0; iw<4; iw++)
	{
		pWing = m_pWingList[iw];
		if(pWing)
		{
			nFlap = 0;
			for (j=0; j<pWing->m_NSurfaces; j++)
			{
				if(pWing->m_Surface[j].m_bTEFlap)
				{
					if((!m_pCurWPolar->m_bAVLControls&&m_pCurWPolar->m_bActiveControl[NCtrls]) ||
					   ( m_pCurWPolar->m_bAVLControls&&m_pCurWPolar->m_MaxControl[NCtrls]))
					{

						if(m_pCurWPolar->m_bAVLControls)
						{
							angle = m_pCurWPolar->m_MaxControl[NCtrls] * t; //maxcontrol is the gain

							if(pWing->m_Surface[j].m_pFoilA->m_TEFlapAngle && pWing->m_Surface[j].m_pFoilA->m_TEFlapAngle)
								TotalAngle = angle + (pWing->m_Surface[j].m_pFoilA->m_TEFlapAngle + pWing->m_Surface[j].m_pFoilB->m_TEFlapAngle)/2.0;
							else
								TotalAngle = angle;

							strange = QString::fromUtf8("         Rotating the flap by %1°, total angle is %2°").arg(angle, 5, 'f',2).arg(TotalAngle, 5, 'f',2);
						}
						else
						{
							angle = m_pCurWPolar->m_MinControl[NCtrls] + t * (m_pCurWPolar->m_MaxControl[NCtrls] - m_pCurWPolar->m_MinControl[NCtrls]);
							strange =  "        Setting "+pWing->m_WingName +" ";
							strange += QString("flap %1 angle to %2").arg(nFlap+1).arg(angle, 5, 'f',2);
							//cancel initial angle, if any
							if(pWing->m_Surface[j].m_pFoilA->m_TEFlapAngle && pWing->m_Surface[j].m_pFoilA->m_TEFlapAngle)
								angle -= (pWing->m_Surface[j].m_pFoilA->m_TEFlapAngle + pWing->m_Surface[j].m_pFoilB->m_TEFlapAngle)/2.0;
						}
						strange += "\n";
						out +=strange;

						if(fabs(angle)>PRECISION)
						{
//							pWing->m_Surface[j].RotateFlap(angle, bBCOnly);
							Quat.Set(angle, pWing->m_Surface[j].m_HingeVector);
							if(bBCOnly)
							{
								for(int p=0; p<m_MatSize; p++)
								{
									if(pWing->m_Surface[j].IsFlapPanel(p))
									{
										memcpy(pPanel+p, m_MemPanel+p, sizeof(CPanel));
										pPanel[p].RotateBC(pWing->m_Surface[j].m_HingePoint, Quat);
									}
								}
							}
							else
							{
								for(int n=0; n<m_nNodes; n++)
								{
									if(pWing->m_Surface[j].IsFlapNode(n))
									{
										pNode[n].Copy(m_MemNode[n]);
										W = pNode[n] - pWing->m_Surface[j].m_HingePoint;
										Quat.Conjugate(W);
										pNode[n] = W + pWing->m_Surface[j].m_HingePoint;
									}
								}
								for(int p=0; p<m_MatSize; p++)
								{
									if(pWing->m_Surface[j].IsFlapPanel(p)) m_Panel[p].SetFrame();
								}
							}
						}
					}
					nFlap++;
					NCtrls++;
				}
			}
		}
	}
}


double QMiarex::GetCoreSize()
{
	return m_CoreSize;
}

void QMiarex::SetCoreSize(double CoreSize)
{
	m_CoreSize = CoreSize;
}


void QMiarex::contextMenuEvent (QContextMenuEvent * event)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;
	GLWidget *pGLWidget = (GLWidget*)s_pGLWidget;

	if(pMainFrame->m_pctrlCentralWidget->currentIndex()==0) p2DWidget->contextMenuEvent(event);
	else                                                    pGLWidget->contextMenuEvent(event);
}

