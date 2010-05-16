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

#include <QGLWidget>
#include <QAction>
#include "Miarex.h"
#include "../MainFrame.h"
#include "../TwoDWidget.h"
#include "../Globals.h"
#include "../Misc/ModDlg.h"
#include "../Misc/RenameDlg.h"
#include "../Misc/ProgressDlg.h"
#include "../Misc/EditPlrDlg.h"
#include "../Misc/PolarFilterDlg.h"
#include "../Graph/GraphDlg.h"
#include "GLCreateLists.h"
#include "WAdvancedDlg.h"
#include "ManageBodiesDlg.h"
#include "ManageUFOsDlg.h"
#include "GLLightDlg.h"
#include "GL3DScales.h"
#include "StabViewDlg.h"
#include "PlaneDlg.h"
#include "WingScaleDlg.h"
#include "W3dPrefsDlg.h"
#include "InertiaDlg.h"
#include <math.h>



QMiarex::QMiarex(QWidget *parent)
	: QWidget(parent)
{
	//construct evrything
	m_pMainFrame  = NULL;
	m_pXFile      = NULL;
	m_pPanelDlg   = NULL;
	m_pStabDlg    = NULL;
	m_pVLMDlg     = NULL;
	m_pLLTDlg     = NULL;
	m_pCurPlane   = NULL;
	m_pCurWing    = NULL;
	m_pCurWing2   = NULL;
	m_pCurStab    = NULL;
	m_pCurFin     = NULL;
	m_pCurPOpp    = NULL;
	m_pCurBody    = NULL;
	m_pCurWPolar  = NULL;
	m_pCurWOpp    = NULL;
	m_pCurGraph   = &m_WingGraph1;

	QRect r(100,100, 700, 600);

	m_NSurfaces =   0;
	m_NStation  =  20;
	m_Iter      = 100;
	CWing::s_CvPrec    =   0.01;
	CWing::s_RelaxMax  =  20.0;
	CWing::s_NLLTStations = 20;

	m_Panel[0].m_VortexPos = 0.25;
	m_Panel[0].m_CtrlPos   = 0.75;

//	m_WngAnalysis.m_WakePanelFactor = 1.10;
//	m_WngAnalysis.m_TotalWakeLength = 100.00;
//	m_WngAnalysis.m_NXWakePanels    = 1;

	m_CurveStyle = 0;
	m_CurveWidth = 1;
	m_CurveColor = QColor(127, 255, 70);

	m_CoreSize        = 0.000001;
	m_MaxWakeIter     = 5;
	m_WakeInterNodes  = 6;
	m_bResetWake      = true;
	m_bSetNewWake     = true;
	m_bSequence       = false;

	m_bHighlightOpp = false;


	m_nWakeNodes = 0;
	m_WakeSize   = 0;
	m_LastPoint.setX(0);
	m_LastPoint.setY(0);
	m_WPlrLegendOffset.setX(0);
	m_WPlrLegendOffset.setY(0);

	m_rCltRect.setWidth(0);
	m_rCltRect.setHeight(0);
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


	m_NXPoints    = 30;
	m_NHoopPoints = 20;


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

	m_pCurWingGraph = &m_WingGraph1;
	m_pCurWPlrGraph = &m_WPlrGraph1;
	m_pCurRLStabGraph = &m_LongRLGraph;
	m_pCurTimeGraph   = &m_TimeGraph1;

	m_WingGraph1.SetAutoX(true);
//	m_WingGraph1.SetXUnit(2.0);
	m_WingGraph1.SetXMin(-1.0);
	m_WingGraph1.SetXMax( 1.0);
	m_WingGraph1.SetYMin(0.000);
	m_WingGraph1.SetYMax(0.001);
	m_WingGraph1.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_WingGraph1.SetXMinGrid(false, true, QColor(80,80,80),2, 1, 100.0);
	m_WingGraph1.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_WingGraph1.SetYMinGrid(false, true, QColor(80,80,80),2, 1, 0.1);
	m_WingGraph1.SetType(1);
	m_WingGraph1.SetMargin(50);

	m_WingGraph2.SetAutoX(true);
	m_WingGraph2.SetXUnit(2.0);
	m_WingGraph2.SetXMin(-10.0);
	m_WingGraph2.SetXMax( 10.0);
	m_WingGraph2.SetYMin(0.000);
	m_WingGraph2.SetYMax(0.001);
	m_WingGraph2.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_WingGraph2.SetXMinGrid(false, true, QColor(80,80,80),2, 1, 100.0);
	m_WingGraph2.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_WingGraph2.SetYMinGrid(false, true, QColor(80,80,80),2, 1, 0.1);
	m_WingGraph2.SetType(1);
	m_WingGraph2.SetMargin(50);

	m_WingGraph3.SetAutoX(true);
	m_WingGraph3.SetXUnit(2.0);
	m_WingGraph3.SetXMin(-10.0);
	m_WingGraph3.SetXMax( 10.0);
	m_WingGraph3.SetYMin(0.000);
	m_WingGraph3.SetYMax(0.001);
	m_WingGraph3.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_WingGraph3.SetXMinGrid(false, true, QColor(80,80,80),2, 1, 100.0);
	m_WingGraph3.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_WingGraph3.SetYMinGrid(false, true, QColor(80,80,80),2, 1, 0.1);
	m_WingGraph3.SetType(1);
	m_WingGraph3.SetMargin(50);

	m_WingGraph4.SetAutoX(true);
	m_WingGraph4.SetXUnit(2.0);
	m_WingGraph4.SetXMin(-10.0);
	m_WingGraph4.SetXMax( 10.0);
	m_WingGraph4.SetYMin(0.000);
	m_WingGraph4.SetYMax(0.001);
	m_WingGraph4.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_WingGraph4.SetXMinGrid(false, true, QColor(80,80,80),2, 1, 100.0);
	m_WingGraph4.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_WingGraph4.SetYMinGrid(false, true, QColor(80,80,80),2, 1, 0.1);
	m_WingGraph4.SetType(1);
	m_WingGraph4.SetMargin(50);

	m_WPlrGraph1.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_WPlrGraph1.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_WPlrGraph1.SetXTitle(tr("Cd"));
	m_WPlrGraph1.SetYTitle(tr("Cl"));
	m_WPlrGraph1.SetXMin(-0.0);
	m_WPlrGraph1.SetXMax( 0.1);
	m_WPlrGraph1.SetYMin(-0.01);
	m_WPlrGraph1.SetYMax( 0.01);
	m_WPlrGraph1.SetType(0);
	m_WPlrGraph1.SetMargin(50);

	m_WPlrGraph2.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_WPlrGraph2.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_WPlrGraph2.SetXTitle(tr("Alpha"));
	m_WPlrGraph2.SetYTitle(tr("Cl"));
	m_WPlrGraph2.SetXMin(-0.0);
	m_WPlrGraph2.SetXMax( 0.1);
	m_WPlrGraph2.SetYMin(-0.01);
	m_WPlrGraph2.SetYMax( 0.01);
	m_WPlrGraph2.SetType(0);
	m_WPlrGraph2.SetMargin(50);

	m_WPlrGraph3.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_WPlrGraph3.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_WPlrGraph3.SetXTitle(tr("Alpha"));
	m_WPlrGraph3.SetYTitle(tr("Cm"));
	m_WPlrGraph3.SetXMin(-0.0);
	m_WPlrGraph3.SetXMax( 0.1);
	m_WPlrGraph3.SetYMin(-0.01);
	m_WPlrGraph3.SetYMax( 0.01);
	m_WPlrGraph3.SetType(0);
	m_WPlrGraph3.SetMargin(50);

	m_WPlrGraph4.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_WPlrGraph4.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_WPlrGraph4.SetXTitle(tr("Alpha"));
	m_WPlrGraph4.SetYTitle(tr("Cl/Cd"));
	m_WPlrGraph4.SetXMin(-0.0);
	m_WPlrGraph4.SetXMax( 0.1);
	m_WPlrGraph4.SetYMin(-0.01);
	m_WPlrGraph4.SetYMax( 0.01);
	m_WPlrGraph4.SetType(0);
	m_WPlrGraph4.SetMargin(50);

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

	for(int i=0; i<4;i++) m_CpGraph.AddCurve(); // four curves for wing, wing2, stab and fin

	m_TimeGraph1.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_TimeGraph1.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_TimeGraph1.SetXTitle("s");
	m_TimeGraph1.SetXMin( 0.0);
	m_TimeGraph1.SetXMax( 0.1);
	m_TimeGraph1.SetYMin(-0.01);
	m_TimeGraph1.SetYMax( 0.01);
	m_TimeGraph1.SetType(0);
	m_TimeGraph1.SetMargin(50);
	m_TimeGraph1.SetInverted(false);
	m_TimeGraph1.SetGraphName("Time Response");
	m_TimeGraph1.AddCurve();//1 curve undeletable for current POpp/WOpp

	m_TimeGraph2.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_TimeGraph2.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_TimeGraph2.SetXTitle("s");
	m_TimeGraph2.SetXMin( 0.0);
	m_TimeGraph2.SetXMax( 0.1);
	m_TimeGraph2.SetYMin(-0.01);
	m_TimeGraph2.SetYMax( 0.01);
	m_TimeGraph2.SetType(0);
	m_TimeGraph2.SetMargin(50);
	m_TimeGraph2.SetInverted(false);
	m_TimeGraph2.SetGraphName("Time Response");
	m_TimeGraph2.AddCurve();//1 curve undeletable for current POpp/WOpp

	m_TimeGraph3.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_TimeGraph3.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_TimeGraph3.SetXTitle("s");
	m_TimeGraph3.SetXMin( 0.0);
	m_TimeGraph3.SetXMax( 0.1);
	m_TimeGraph3.SetYMin(-0.01);
	m_TimeGraph3.SetYMax( 0.01);
	m_TimeGraph3.SetType(0);
	m_TimeGraph3.SetMargin(50);
	m_TimeGraph3.SetInverted(false);
	m_TimeGraph3.SetGraphName("Time Response");
	m_TimeGraph3.AddCurve();//1 curve undeletable for current POpp/WOpp

	m_TimeGraph4.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_TimeGraph4.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_TimeGraph4.SetXTitle("s");
	m_TimeGraph4.SetXMin( 0.0);
	m_TimeGraph4.SetXMax( 0.1);
	m_TimeGraph4.SetYMin(-0.01);
	m_TimeGraph4.SetYMax( 0.01);
	m_TimeGraph4.SetType(0);
	m_TimeGraph4.SetMargin(50);
	m_TimeGraph4.SetInverted(false);
	m_TimeGraph4.SetGraphName("Time Response");
	m_TimeGraph4.AddCurve();//1 curve undeletable for current POpp/WOpp

	if(m_bLongitudinal)
	{
		m_TimeGraph1.SetYTitle("u");
		m_TimeGraph2.SetYTitle("w");
		m_TimeGraph3.SetYTitle("q");
		m_TimeGraph4.SetYTitle("theta");
	}
	else
	{
		m_TimeGraph1.SetYTitle("v");
		m_TimeGraph2.SetYTitle("p");
		m_TimeGraph3.SetYTitle("r");
		m_TimeGraph4.SetYTitle("phi");
	}


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

	m_WingGraph1.SetYVariable(0);
	m_WingGraph2.SetYVariable(1);
	m_WingGraph3.SetYVariable(2);
	m_WingGraph4.SetYVariable(3);

	m_CpColor = QColor(255,100,150);
	m_CpStyle = 0;
	m_CpWidth = 1;
	m_bShowCpPoints = false;
	m_bShowCp       = true;

	m_WPlrGraph1.SetVariables(4,1);
	m_WPlrGraph2.SetVariables(0,1);
	m_WPlrGraph3.SetVariables(0,5);
	m_WPlrGraph4.SetVariables(0,12);

	m_iView = W3DVIEW;
	m_iWingView = 1;
	m_iWPlrView = 4;
	m_iStabilityView = 0;
	m_iStabTimeView = 4;
	m_nNodes  = 0;
	m_MatSize = 0;

	m_WingColor =  QColor(203,222, 222);
	m_StabColor =  QColor(200, 230, 250);
	m_FinColor  =  QColor(207, 227, 255);

	m_poaFoil    = NULL;
	m_poaPolar   = NULL;
	m_poaWing    = NULL;
	m_poaPlane   = NULL;
	m_poaWPolar  = NULL;
	m_poaWOpp    = NULL;
	m_poaPOpp    = NULL;

	memset(m_pPanel, 0, sizeof(m_pPanel));
	memset(m_aij, 0, sizeof(m_aij));
	memset(m_aijRef, 0, sizeof(m_aijRef));
	memset(m_RHS, 0, sizeof(m_RHS));
	memset(m_RHSRef, 0, sizeof(m_RHSRef));

	m_WingGraph1.SetGraphName(tr("Wing Graph 1"));
	m_WingGraph2.SetGraphName(tr("Wing Graph 2"));
	m_WingGraph3.SetGraphName(tr("Wing Graph 3"));
	m_WingGraph4.SetGraphName(tr("Wing Graph 4"));

	m_WPlrGraph1.SetGraphName(tr("Wing Polar Graph 1"));
	m_WPlrGraph2.SetGraphName(tr("Wing Polar Graph 2"));
	m_WPlrGraph3.SetGraphName(tr("Wing Polar Graph 3"));
	m_WPlrGraph4.SetGraphName(tr("Wing Polar Graph 4"));

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

	m_bDirichlet         = true;
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
	m_bLogFile           = false;
	m_bHalfWing          = false;
	m_bTransGraph        = true;
	m_bFoilNames         = false;
	m_bLongitudinal      = true;
	m_bCurWOppOnly       = true;
	m_bStoreWOpp         = true;
	m_bKeepOutOpps       = false;
	m_bCurFrameOnly      = true;
	m_bType1 = m_bType2 = m_bType4 = m_bType5 = m_bType6 = m_bType7 = true;
	m_bShowElliptic      = false;
	m_bShowWing2         = true;
	m_bShowStab          = true;
	m_bShowFin           = true;
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
	m_bResetglModeLegend = true;

	m_bDragPoint         = false;
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
	m_bForcedResponse    = true;
	
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
	m_ArcBall.m_pRect     = &m_rCltRect;

	m_pLLTDlg = new LLTAnalysisDlg;
	CWing::s_pLLTDlg        = m_pLLTDlg;

	m_pVLMDlg = new VLMAnalysisDlg;
	CWing::s_pVLMDlg     = m_pVLMDlg;
	m_pVLMDlg->m_pNode         = m_Node;
	m_pVLMDlg->m_pPanel        = m_Panel;
	m_pVLMDlg->m_ppPanel       = m_pPanel;
	m_pVLMDlg->m_pWakeNode     = m_WakeNode;
	m_pVLMDlg->m_pWakePanel    = m_WakePanel;
	m_pVLMDlg->m_pMemNode      = m_MemNode;
	m_pVLMDlg->m_pMemPanel     = m_MemPanel;
	m_pVLMDlg->m_pTempWakeNode = m_TempWakeNode;
	m_pVLMDlg->m_pRefWakeNode  = m_RefWakeNode;
	m_pVLMDlg->m_pRefWakePanel = m_RefWakePanel;
	m_pVLMDlg->m_RHS           = m_RHS;
	m_pVLMDlg->m_Gamma         = m_RHSRef;
	m_pVLMDlg->m_aij           = m_aij;
	m_pVLMDlg->m_pCoreSize     = &m_CoreSize;

	m_pStabDlg = new StabAnalysisDlg;
	CWing::s_pStabDlg     = m_pStabDlg;
	m_pStabDlg->m_pNode         = m_Node;
	m_pStabDlg->m_pPanel        = m_Panel;
//	m_pStabDlg->m_ppPanel       = m_pPanel;
	m_pStabDlg->m_pMemNode      = m_MemNode;
	m_pStabDlg->m_pMemPanel     = m_MemPanel;
	m_pStabDlg->m_RHS           = m_RHS;
	m_pStabDlg->m_Gamma         = m_RHSRef;
	m_pStabDlg->m_aij           = m_aij;
	m_pStabDlg->m_aijRef        = m_aijRef;
	m_pStabDlg->m_pCoreSize     = &m_CoreSize;


	m_pPanelDlg = new PanelAnalysisDlg;
	CWing::s_p3DPanelDlg = m_pPanelDlg;  //pointer to the 3DPanel analysis dialog class
	m_pPanelDlg->m_pPanel        = m_Panel;
	m_pPanelDlg->m_ppPanel       = m_pPanel;
	m_pPanelDlg->m_pNode         = m_Node;
	m_pPanelDlg->m_pWakePanel    = m_WakePanel;
	m_pPanelDlg->m_pWakeNode     = m_WakeNode;
	m_pPanelDlg->m_pMemNode      = m_MemNode;
	m_pPanelDlg->m_pMemPanel     = m_MemPanel;
	m_pPanelDlg->m_pTempWakeNode = m_TempWakeNode;
	m_pPanelDlg->m_pRefWakeNode  = m_RefWakeNode;
	m_pPanelDlg->m_pRefWakePanel = m_RefWakePanel;
	m_pPanelDlg->m_aij           = m_aij;
	m_pPanelDlg->m_aijRef        = m_aijRef;
	m_pPanelDlg->m_RHS           = m_RHS;
	m_pPanelDlg->m_RHSRef        = m_RHSRef;
	m_pPanelDlg->m_pCoreSize     = &m_CoreSize;


	GL3dWingDlg::s_pGLLightDlg = &m_GLLightDlg;
	GL3dBodyDlg::s_pGLLightDlg = &m_GLLightDlg;

	SetupLayout();

	m_pctrlHalfWing->setChecked(m_bHalfWing);
	m_pctrlTrans->setChecked(m_bXTop);
	m_pctrlIDrag->setChecked(m_bICd);
	m_pctrlVDrag->setChecked(m_bVCd);
	m_pctrlLift->setChecked(m_bXCP);

	connect(m_pctrlAlphaMin, SIGNAL(editingFinished()), this, SLOT(OnReadAnalysisData()));
	connect(m_pctrlAlphaMax, SIGNAL(editingFinished()), this, SLOT(OnReadAnalysisData()));
	connect(m_pctrlAlphaDelta, SIGNAL(editingFinished()), this, SLOT(OnReadAnalysisData()));

	connect(m_pctrlSequence, SIGNAL(clicked()), this, SLOT(OnSequence()));
	connect(m_pctrlStoreWOpp, SIGNAL(clicked()), this, SLOT(OnStoreWOpp()));
	connect(m_pctrlInitLLTCalc, SIGNAL(clicked()), this, SLOT(OnInitLLTCalc()));
	connect(m_pctrlAnalyze, SIGNAL(clicked()), this, SLOT(OnAnalyze()));
	connect(m_pctrlCurveStyle, SIGNAL(activated(int)), this, SLOT(OnCurveStyle(int)));
	connect(m_pctrlCurveWidth, SIGNAL(activated(int)), this, SLOT(OnCurveWidth(int)));
	connect(m_pctrlCurveColor, SIGNAL(clicked()), this, SLOT(OnCurveColor()));
	connect(m_pctrlShowPoints, SIGNAL(clicked()), this, SLOT(OnShowPoints()));
	connect(m_pctrlShowCurve, SIGNAL(clicked()), this, SLOT(OnShowCurve()));

	connect(m_pctrlHalfWing, SIGNAL(clicked()), this, SLOT(OnHalfWing()));
	connect(m_pctrlLift, SIGNAL(clicked()), this, SLOT(OnShowLift()));
	connect(m_pctrlIDrag, SIGNAL(clicked()), this, SLOT(OnShowIDrag()));
	connect(m_pctrlVDrag, SIGNAL(clicked()), this, SLOT(OnShowVDrag()));
	connect(m_pctrlTrans, SIGNAL(clicked()), this, SLOT(OnShowTransitions()));
	connect(m_pctrlCp, SIGNAL(clicked()),this, SLOT(On3DCp()));
	connect(m_pctrlMoment, SIGNAL(clicked()), this, SLOT(OnMoment()));
	connect(m_pctrlDownwash, SIGNAL(clicked()), this, SLOT(OnDownwash()));
	connect(m_pctrlStream, SIGNAL(clicked()), this, SLOT(OnStreamlines()));
	connect(m_pctrlSurfVel, SIGNAL(clicked()), this, SLOT(OnSurfaceSpeeds()));
	connect(m_pctrlHighlightOpp, SIGNAL(clicked()), this, SLOT(OnHighlightOpp()));

	connect(m_pctrlWOppAnimate, SIGNAL(clicked()), this, SLOT(OnAnimateWOpp()));
	connect(m_pctrlAnimateWOppSpeed, SIGNAL(sliderMoved(int)), this, SLOT(OnAnimateWOppSpeed(int)));
	connect(m_pTimerWOpp, SIGNAL(timeout()), this, SLOT(OnAnimateWOppSingle()));
	connect(m_pTimerMode, SIGNAL(timeout()), this, SLOT(OnAnimateModeSingle()));

	connect(m_pctrlSurfaces, SIGNAL(clicked()), SLOT(OnSurfaces()));
	connect(m_pctrlOutline, SIGNAL(clicked()), SLOT(OnOutline()));
	connect(m_pctrlPanels, SIGNAL(clicked()), SLOT(OnPanels()));
	connect(m_pctrlVortices, SIGNAL(clicked()), SLOT(OnVortices()));
	connect(m_pctrlFoilNames, SIGNAL(clicked()), SLOT(OnFoilNames()));
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

	connect(m_pctrlLongDynamics, SIGNAL(clicked()), SLOT(OnStabilityDirection()));
	connect(m_pctrlLatDynamics, SIGNAL(clicked()), SLOT(OnStabilityDirection()));
	connect(m_pctrlTimeView, SIGNAL(clicked()), this, SLOT(OnStabilityViewType()));
	connect(m_pctrlRLView,   SIGNAL(clicked()), this, SLOT(OnStabilityViewType()));
	connect(m_pctrl3DView,   SIGNAL(clicked()), this, SLOT(OnStabilityViewType()));

}




CBody* QMiarex::AddBody(CBody *pBody)
{
	//adds the pBody to the m_oaBody array
	//places it in alphabetical order
	bool bExists   = false;
	bool bInserted = false;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	CBody *pOldBody;
	QString strong;
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
	int i,j;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	CPlane *pOldPlane;
	QString strong;
	bool bExists   = false;
	bool bInserted = false;

	for (i=0; i<m_poaPlane->size(); i++)
	{
		pOldPlane = (CPlane*)m_poaPlane->at(i);
		if (pOldPlane->m_PlaneName == pPlane->m_PlaneName)
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
				if (pPlane->m_PlaneName < pOldPlane->m_PlaneName)
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
				if (pOldPlane->m_PlaneName == pPlane->m_PlaneName)
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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
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

		pWOpp = &pPOpp->m_WingWOpp;


		CreateWOpp(&pPOpp->m_WingWOpp, m_pCurWing);
		if(m_pCurWing2) CreateWOpp(&pPOpp->m_Wing2WOpp, m_pCurWing2);
		if(m_pCurStab)  CreateWOpp(&pPOpp->m_StabWOpp, m_pCurStab);
		if(m_pCurFin)   CreateWOpp(&pPOpp->m_FinWOpp,  m_pCurFin);

		if(m_pCurWing2)  pPOpp->m_bBiplane = true; else pPOpp->m_bBiplane = false;
		if(m_pCurStab)   pPOpp->m_bStab = true;    else pPOpp->m_bStab = false;
		if(m_pCurFin )   pPOpp->m_bFin  = true;    else pPOpp->m_bFin = false;

		pPOpp->m_PlaneName = m_pCurPlane->m_PlaneName;

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

		if(m_pCurWPolar->m_AnalysisType==2 && m_pVLMDlg)
		{
			pPOpp->m_NPanels          = m_pVLMDlg->m_MatSize;
			pPOpp->m_Alpha            = m_pVLMDlg->m_OpAlpha;
			pPOpp->m_Bank             = 0.0;
			pPOpp->m_QInf             = m_pVLMDlg->m_OpQInf;
			
			if(m_pCurWPolar->m_Type==5 || m_pCurWPolar->m_Type==6)
			{
				pPOpp->m_Ctrl = m_pVLMDlg->m_Ctrl;
				pWOpp->m_Ctrl = m_pVLMDlg->m_Ctrl;
			}
			else if(m_pCurWPolar->m_Type==7)
			{
				pPOpp->m_Ctrl = m_pStabDlg->m_Ctrl;
				pWOpp->m_Ctrl = m_pStabDlg->m_Ctrl;
			}
			else
			{
				pPOpp->m_Ctrl = 0.0;
				pWOpp->m_Ctrl = 0.0;
			}

			pWOpp->m_Alpha               = m_pVLMDlg->m_OpAlpha;
			pWOpp->m_QInf                = m_pVLMDlg->m_OpQInf;
			pWOpp->m_Phi                 = 0.0;
			pWOpp->m_CL                  = m_pVLMDlg->m_CL;
			pWOpp->m_CX                  = m_pVLMDlg->m_CX;
			pWOpp->m_CY                  = m_pVLMDlg->m_CY;
			pWOpp->m_InducedDrag         = m_pVLMDlg->m_InducedDrag;
			pWOpp->m_ViscousDrag         = m_pVLMDlg->m_ViscousDrag;

			pWOpp->m_GCm                 = m_pVLMDlg->m_GCm;
			pWOpp->m_VCm                 = m_pVLMDlg->m_VCm;
			pWOpp->m_ICm                 = m_pVLMDlg->m_ICm;
			pWOpp->m_GRm                 = m_pVLMDlg->m_GRm;
			pWOpp->m_GYm                 = m_pVLMDlg->m_GYm;
			pWOpp->m_VYm                 = m_pVLMDlg->m_VYm;
			pWOpp->m_IYm                 = m_pVLMDlg->m_IYm;

			pWOpp->m_XCP                 = m_pVLMDlg->m_XCP;
			pWOpp->m_YCP                 = m_pVLMDlg->m_YCP;

			pPOpp->m_Beta                = m_pCurWPolar->m_Beta;
			pWOpp->m_Beta	             = m_pCurWPolar->m_Beta;
		}
		else if(m_pCurWPolar->m_AnalysisType==3 && m_pPanelDlg)
		{
			pPOpp->m_NPanels             = m_pPanelDlg->m_MatSize;
			pPOpp->m_Alpha               = m_pPanelDlg->m_OpAlpha;
			pPOpp->m_QInf                = m_pPanelDlg->m_QInf;
			pWOpp->m_Alpha               = m_pPanelDlg->m_OpAlpha;
			pWOpp->m_QInf                = m_pPanelDlg->m_QInf;
			pWOpp->m_CL                  = m_pPanelDlg->m_CL;
			pWOpp->m_CX                  = m_pPanelDlg->m_CX;
			pWOpp->m_CY                  = m_pPanelDlg->m_CY;
			pWOpp->m_InducedDrag         = m_pPanelDlg->m_InducedDrag;
			pWOpp->m_ViscousDrag         = m_pPanelDlg->m_ViscousDrag;

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
		}
		else if(m_pCurWPolar->m_AnalysisType==4 && m_pStabDlg)
		{
			pPOpp->m_NPanels          = m_pStabDlg->m_MatSize;
			pPOpp->m_Alpha            = m_pStabDlg->m_AlphaEq;
			pPOpp->m_QInf             = m_pStabDlg->u0;
			if(m_pCurWPolar->m_Type==7)
			{
				pPOpp->m_Ctrl = m_pStabDlg->m_Ctrl;
				pWOpp->m_Ctrl = m_pStabDlg->m_Ctrl;
			}
			else
			{
				pPOpp->m_Ctrl = 0.0;
				pWOpp->m_Ctrl = 0.0;
			}

			pWOpp->m_Alpha          = m_pStabDlg->m_AlphaEq;
			pWOpp->m_QInf           = m_pStabDlg->u0;
			pWOpp->m_CL             = m_pStabDlg->m_CL;
			pWOpp->m_CX             = m_pStabDlg->m_CX;
			pWOpp->m_CY             = m_pStabDlg->m_CY;
			pWOpp->m_InducedDrag    = m_pStabDlg->m_InducedDrag;
			pWOpp->m_ViscousDrag    = m_pStabDlg->m_ViscousDrag;

			pWOpp->m_GCm            = m_pStabDlg->m_GCm;
			pWOpp->m_VCm            = m_pStabDlg->m_VCm;
			pWOpp->m_ICm            = m_pStabDlg->m_ICm;
			pWOpp->m_GRm            = m_pStabDlg->m_GRm;
			pWOpp->m_GYm            = m_pStabDlg->m_GYm;
			pWOpp->m_VYm            = m_pStabDlg->m_VYm;
			pWOpp->m_IYm            = m_pStabDlg->m_IYm;

			pWOpp->m_XCP            = m_pStabDlg->m_XCP;
			pWOpp->m_YCP            = m_pStabDlg->m_YCP;

			pPOpp->m_Beta           = m_pCurWPolar->m_Beta;
			pWOpp->m_Beta	        = m_pCurWPolar->m_Beta;
			
			for(i=0; i<4; i++)
			{
				pWOpp->m_EigenValue[i]   = m_pStabDlg->m_rLong[i];
				pWOpp->m_EigenValue[i+4] = m_pStabDlg->m_rLat[i];
				for(l=0;l<4; l++)
				{
					pWOpp->m_EigenVector[i][l]   = m_pStabDlg->m_vLong[4*i+l];
					pWOpp->m_EigenVector[i+4][l] = m_pStabDlg->m_vLat[4*i+l];
				}
			}
			m_pStabDlg->GetNDStabDerivatives(&pPOpp->m_WingWOpp);
			memcpy(pWOpp->m_ALong, m_pStabDlg->m_ALong, 16*sizeof(double));
			memcpy(pWOpp->m_ALat,  m_pStabDlg->m_ALat,  16*sizeof(double));
			memcpy(pWOpp->m_BLong, m_pStabDlg->m_BLong, MAXCONTROLS * 4*sizeof(double));
			memcpy(pWOpp->m_BLat , m_pStabDlg->m_BLat,  MAXCONTROLS * 4*sizeof(double));
		}

		pPOpp->m_Wing2WOpp.m_Alpha = pPOpp->m_Alpha;
		pPOpp->m_StabWOpp.m_Alpha  = pPOpp->m_Alpha;
		pPOpp->m_FinWOpp.m_Alpha   = pPOpp->m_Alpha;
		pPOpp->m_Wing2WOpp.m_QInf  = pPOpp->m_QInf;
		pPOpp->m_StabWOpp.m_QInf   = pPOpp->m_QInf;
		pPOpp->m_FinWOpp.m_QInf    = pPOpp->m_QInf;

		if(Cp) memcpy(pPOpp->m_Cp, Cp,  sizeof(pPOpp->m_Cp));

		if(Gamma)	memcpy(pPOpp->m_G,  Gamma,   sizeof(pPOpp->m_G));

		if(Sigma)	memcpy(pPOpp->m_Sigma,  Sigma,   sizeof(pPOpp->m_G));


		p = 0;

		memcpy(pPOpp->m_WingWOpp.m_Cp, pPOpp->m_Cp, m_pCurWing->m_MatSize*sizeof(double));
		p+=m_pCurWing->m_MatSize;

		if(m_pCurWing2)
		{
			memcpy(pPOpp->m_Wing2WOpp.m_Cp, pPOpp->m_Cp+p, m_pCurWing2->m_MatSize*sizeof(double));
			memcpy(pPOpp->m_Wing2WOpp.m_Vd, m_pCurWing2->m_Vd,  sizeof(pPOpp->m_Wing2WOpp.m_Vd));
			p += m_pCurWing2->m_MatSize;
		}
		if(m_pCurStab)
		{
			memcpy(pPOpp->m_StabWOpp.m_Cp, pPOpp->m_Cp+p, m_pCurStab->m_MatSize*sizeof(double));
			memcpy(pPOpp->m_StabWOpp.m_Vd, m_pCurStab->m_Vd,  sizeof(pPOpp->m_StabWOpp.m_Vd));
			p += m_pCurStab->m_MatSize;
		}
		if(m_pCurFin)
		{
			memcpy(pPOpp->m_FinWOpp.m_Cp, pPOpp->m_Cp+p, m_pCurFin->m_MatSize*sizeof(double));
			memcpy(pPOpp->m_FinWOpp.m_Vd, m_pCurFin->m_Vd,  sizeof(pPOpp->m_FinWOpp.m_Vd));
			p+= m_pCurFin->m_MatSize;
		}


		m_pCurWPolar->AddPoint(pPOpp);

//		m_pCurWOpp = &pPOpp->m_WingWOpp;
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
							i = (int)m_poaPOpp->size();// to break
						}
						else if (pPOpp->m_Alpha > pOldPOpp->m_Alpha)
						{
							//insert point
							m_poaPOpp->insert(i, pPOpp);
							bIsInserted = true;
							i = (int)m_poaPOpp->size();// to break
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
							i = (int)m_poaPOpp->size();// to break
						}
						else if (pPOpp->m_QInf > pOldPOpp->m_QInf)
						{
							//insert point
							m_poaPOpp->insert(i, pPOpp);
							bIsInserted = true;
							i = (int)m_poaPOpp->size();// to break
						}
					}
				}
			}
		}

		if (!bIsInserted) 	m_poaPOpp->append(pPOpp);

		pPOpp->m_WingWOpp.m_Color    = pPOpp->m_Color;
		pPOpp->m_Wing2WOpp.m_Color   = pPOpp->m_Color;
		pPOpp->m_StabWOpp.m_Color    = pPOpp->m_Color;
		pPOpp->m_FinWOpp.m_Color     = pPOpp->m_Color;
		pPOpp->m_WingWOpp.m_Style    = pPOpp->m_Style;
		pPOpp->m_Wing2WOpp.m_Style   = pPOpp->m_Style;
		pPOpp->m_StabWOpp.m_Style    = pPOpp->m_Style;
		pPOpp->m_FinWOpp.m_Style     = pPOpp->m_Style;
		pPOpp->m_WingWOpp.m_Width    = pPOpp->m_Width;
		pPOpp->m_Wing2WOpp.m_Width   = pPOpp->m_Width;
		pPOpp->m_StabWOpp.m_Width    = pPOpp->m_Width;
		pPOpp->m_FinWOpp.m_Width     = pPOpp->m_Width;
//		m_pCurPOpp = pPOpp;
//		m_pCurWOpp = &m_pCurPOpp->m_WingWOpp;
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
	//adds the pWing to the m_oaWing array
	//places it in alphabetical order
	int i,j;
	bool bExists   = false;
	bool bInserted = false;
	CWing *pOldWing;
	QString strong;
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
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



void QMiarex::AddWOpp(bool bPointOut, double *Gamma, double *Sigma, double *Cp)
{
	// creates a new WOpp, and if successful
	// fills the WOpp's variables with the current analysis results
	// adds the WOpp to the array
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	int i,j,l;
	if(!m_bKeepOutOpps && bPointOut) return;

	CWOpp *pWOpp;
	CPOpp *pPOpp;
	CWOpp * pNewPoint;
	pNewPoint = new CWOpp();
	if(pNewPoint == NULL)
	{
		QMessageBox::warning(window(),tr("Warning"),tr("Not enough memory to store the OpPoint\n"));
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

		pNewPoint->m_WingName  = m_pCurWing->m_WingName;

		pNewPoint->m_Span                = m_pCurWPolar->m_WSpan;
		pNewPoint->m_MAChord             = m_pCurWing->m_MAChord;
		pNewPoint->m_NStation            = m_pCurWing->m_NStation;

		pNewPoint->m_PlrName             = m_pCurWPolar->m_PlrName;
		pNewPoint->m_AnalysisType        = m_pCurWPolar->m_AnalysisType;
		pNewPoint->m_bVLM1               = m_pCurWPolar->m_bVLM1;
		pNewPoint->m_bThinSurface        = m_pCurWPolar->m_bThinSurfaces;
		pNewPoint->m_bTiltedGeom         = m_pCurWPolar->m_bTiltedGeom;

		pNewPoint->m_Type                = m_pCurWPolar->m_Type;
		pNewPoint->m_Weight              = m_pCurWPolar->m_Weight;

		double Cb =0.0;

		if(m_pCurWPolar->m_AnalysisType==1)
		{
			pNewPoint->m_Alpha               = CWing::s_Alpha;
			pNewPoint->m_QInf                = CWing::s_QInf;
			pNewPoint->m_NVLMPanels          = m_pCurWing->m_MatSize;
			pNewPoint->m_bOut                = m_pCurWing->m_bWingOut;
			pNewPoint->m_CL                  = m_pCurWing->m_CL;
//			pNewPoint->m_CY                  = m_pCurWing->m_CY;
//			pNewPoint->m_CX                  = m_pCurWing->m_CX;
			pNewPoint->m_InducedDrag         = m_pCurWing->m_InducedDrag;

			pNewPoint->m_GCm                 = m_pCurWing->m_GCm;
			pNewPoint->m_VCm                 = m_pCurWing->m_VCm;
			pNewPoint->m_ICm                 = m_pCurWing->m_ICm;
			pNewPoint->m_GRm                 = m_pCurWing->m_GRm;
			pNewPoint->m_GYm                 = m_pCurWing->m_GYm;
			pNewPoint->m_VYm                 = m_pCurWing->m_VYm;
			pNewPoint->m_IYm                 = m_pCurWing->m_IYm;

			pNewPoint->m_XCP                 = m_pCurWing->m_XCP;
			pNewPoint->m_YCP                 = m_pCurWing->m_YCP;
			pNewPoint->m_ViscousDrag         = m_pCurWing->m_ViscousDrag;

			for (l=1; l<m_pCurWing->m_NStation; l++)
			{
				pNewPoint->m_SpanPos[l]       = -m_pCurWing->m_SpanPos[l];
				pNewPoint->m_StripArea[l]     =  m_pCurWing->m_StripArea[l];
				pNewPoint->m_Ai[l]            =  m_pCurWing->m_Ai[m_NStation-l];
				pNewPoint->m_Cl[l]            =  m_pCurWing->m_Cl[m_NStation-l];
				pNewPoint->m_PCd[l]           =  m_pCurWing->m_PCd[m_NStation-l];
				pNewPoint->m_ICd[l]           =  m_pCurWing->m_ICd[m_NStation-l];
				pNewPoint->m_Cm[l]            =  m_pCurWing->m_Cm[m_NStation-l];
				pNewPoint->m_CmAirf[l]        =  m_pCurWing->m_CmAirf[m_NStation-l];
				pNewPoint->m_XCPSpanRel[l]    =  m_pCurWing->m_XCPSpanRel[m_NStation-l];
				pNewPoint->m_XCPSpanAbs[l]    =  m_pCurWing->m_XCPSpanAbs[m_NStation-l];
				pNewPoint->m_Re[l]            =  m_pCurWing->m_Re[m_NStation-l];
				pNewPoint->m_Chord[l]         =  m_pCurWing->m_Chord[m_NStation-l];
				pNewPoint->m_Twist[l]         =  m_pCurWing->m_Twist[m_NStation-l];
				pNewPoint->m_XTrTop[l]        =  m_pCurWing->m_XTrTop[m_NStation-l];
				pNewPoint->m_XTrBot[l]        =  m_pCurWing->m_XTrBot[m_NStation-l];
				pNewPoint->m_BendingMoment[l] =  m_pCurWing->m_BendingMoment[m_NStation-l];
				memset(pNewPoint->m_Cp, 0, sizeof(pNewPoint->m_Cp));
				if(fabs(m_pCurWing->m_BendingMoment[l])>fabs(Cb))	Cb = m_pCurWing->m_BendingMoment[l];
			}
		}
		else if(m_pCurWPolar->m_AnalysisType==2 && m_pVLMDlg)
		{
			pNewPoint->m_Alpha               = m_pVLMDlg->m_OpAlpha;
			pNewPoint->m_QInf                = m_pVLMDlg->m_OpQInf;
			pNewPoint->m_NVLMPanels          = m_pVLMDlg->m_MatSize;
			pNewPoint->m_bOut                = m_pVLMDlg->m_bPointOut;
			pNewPoint->m_CL                  = m_pVLMDlg->m_CL;
			pNewPoint->m_CY                  = m_pVLMDlg->m_CY;
			pNewPoint->m_CX                  = m_pVLMDlg->m_CX;
			pNewPoint->m_InducedDrag         = m_pVLMDlg->m_InducedDrag;
			pNewPoint->m_ViscousDrag         = m_pVLMDlg->m_ViscousDrag;

			pNewPoint->m_GCm                 = m_pVLMDlg->m_GCm;
			pNewPoint->m_VCm                 = m_pVLMDlg->m_VCm;
			pNewPoint->m_ICm                 = m_pVLMDlg->m_ICm;
			pNewPoint->m_GRm                 = m_pVLMDlg->m_GRm;
			pNewPoint->m_GYm                 = m_pVLMDlg->m_GYm;
			pNewPoint->m_VYm                 = m_pVLMDlg->m_VYm;
			pNewPoint->m_IYm                 = m_pVLMDlg->m_IYm;

			pNewPoint->m_XCP                 = m_pVLMDlg->m_XCP;
			pNewPoint->m_YCP                 = m_pVLMDlg->m_YCP;

			pNewPoint->m_Beta                = m_pCurWPolar->m_Beta;
			
			if(m_pCurWPolar->m_Type==5 || m_pCurWPolar->m_Type==6) pNewPoint->m_Ctrl = m_pVLMDlg->m_Ctrl;
			else                                                   pNewPoint->m_Ctrl = 0.0;

			for (l=0; l<m_pCurWing->m_NStation; l++)
			{
				pNewPoint->m_SpanPos[l]   = m_pCurWing->m_SpanPos[l];
				pNewPoint->m_StripArea[l] = m_pCurWing->m_StripArea[l];
				if(fabs(m_pCurWing->m_BendingMoment[l])>fabs(Cb))	Cb = m_pCurWing->m_BendingMoment[l];
			}
			memcpy(pNewPoint->m_Cl,         m_pCurWing->m_Cl,         sizeof(m_pCurWing->m_Cl));
			memcpy(pNewPoint->m_PCd,        m_pCurWing->m_PCd,        sizeof(m_pCurWing->m_PCd));
			memcpy(pNewPoint->m_Cm,         m_pCurWing->m_Cm,         sizeof(m_pCurWing->m_Cm));
			memcpy(pNewPoint->m_CmAirf,     m_pCurWing->m_CmAirf,     sizeof(m_pCurWing->m_CmAirf));
			memcpy(pNewPoint->m_XCPSpanRel, m_pCurWing->m_XCPSpanRel, sizeof(m_pCurWing->m_XCPSpanRel));
			memcpy(pNewPoint->m_XCPSpanAbs, m_pCurWing->m_XCPSpanAbs, sizeof(m_pCurWing->m_XCPSpanAbs));
			memcpy(pNewPoint->m_Re,         m_pCurWing->m_Re,         sizeof(m_pCurWing->m_Re));
			memcpy(pNewPoint->m_Chord,      m_pCurWing->m_Chord,      sizeof(m_pCurWing->m_Chord));
			memcpy(pNewPoint->m_Twist,      m_pCurWing->m_Twist,      sizeof(m_pCurWing->m_Twist));
			memcpy(pNewPoint->m_XTrTop,     m_pCurWing->m_XTrTop,     sizeof(m_pCurWing->m_XTrTop));
			memcpy(pNewPoint->m_XTrBot,     m_pCurWing->m_XTrBot,     sizeof(m_pCurWing->m_XTrBot));
			memcpy(pNewPoint->m_Vd,         m_pCurWing->m_Vd,         sizeof(pNewPoint->m_Vd));
			memcpy(pNewPoint->m_F,          m_pCurWing->m_F,          sizeof(pNewPoint->m_F));
			memcpy(pNewPoint->m_BendingMoment, m_pCurWing->m_BendingMoment, sizeof(m_pCurWing->m_BendingMoment));
			memcpy(pNewPoint->m_ICd,        m_pCurWing->m_ICd, sizeof(pNewPoint->m_ICd));
			memcpy(pNewPoint->m_Ai,         m_pCurWing->m_Ai,  sizeof(pNewPoint->m_Ai));

			memcpy(pNewPoint->m_Cp,         m_pVLMDlg->m_Cp,  sizeof(pNewPoint->m_Cp));

			memcpy(pNewPoint->m_G, Gamma, sizeof(pNewPoint->m_G));

			if(m_pCurWPolar->m_bWakeRollUp)
			{
				pNewPoint->m_nWakeNodes     = m_nWakeNodes;
				pNewPoint->m_NXWakePanels   = m_pCurWPolar->m_NXWakePanels;
				pNewPoint->m_FirstWakePanel = m_pCurWPolar->m_TotalWakeLength;
				pNewPoint->m_WakeFactor     = m_pCurWPolar->m_WakePanelFactor;
			}
			for (i=0; i<m_pCurWing->m_nFlaps; i++)
				pNewPoint->m_FlapMoment[i] = m_pCurWing->m_FlapMoment[i];

			pNewPoint->m_nFlaps = m_pCurWing->m_nFlaps;
		}
		else if(m_pCurWPolar->m_AnalysisType==3 && m_pPanelDlg)
		{
			pNewPoint->m_Alpha               = m_pPanelDlg->m_OpAlpha;
			pNewPoint->m_QInf                = m_pPanelDlg->m_QInf;
			pNewPoint->m_NVLMPanels          = m_pPanelDlg->m_MatSize;
			pNewPoint->m_bOut                = m_pPanelDlg->m_bPointOut;
			pNewPoint->m_CL                  = m_pPanelDlg->m_CL;
			pNewPoint->m_CY                  = m_pPanelDlg->m_CY;
			pNewPoint->m_CX                  = m_pPanelDlg->m_CX;
			pNewPoint->m_InducedDrag         = m_pPanelDlg->m_InducedDrag;
			pNewPoint->m_ViscousDrag         = m_pPanelDlg->m_ViscousDrag;

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
		}
		else if(m_pCurWPolar->m_AnalysisType==4 && m_pCurWPolar->m_Type==7 && m_pStabDlg)
		{
			pNewPoint->m_Alpha               = m_pStabDlg->m_AlphaEq;
			pNewPoint->m_Beta                = m_pCurWPolar->m_Beta;
			pNewPoint->m_Phi                 = m_pCurWPolar->m_BankAngle;
			pNewPoint->m_QInf                = m_pStabDlg->u0;
			pNewPoint->m_NVLMPanels          = m_pStabDlg->m_MatSize;
			pNewPoint->m_bOut                = m_pStabDlg->m_bPointOut;
			pNewPoint->m_CL                  = m_pStabDlg->m_CL;
			pNewPoint->m_CY                  = m_pStabDlg->m_CY;
			pNewPoint->m_CX                  = m_pStabDlg->m_CX;
			pNewPoint->m_InducedDrag         = m_pStabDlg->m_InducedDrag;
			pNewPoint->m_ViscousDrag         = m_pStabDlg->m_ViscousDrag;

			pNewPoint->m_GCm                 = m_pStabDlg->m_GCm;
			pNewPoint->m_VCm                 = m_pStabDlg->m_VCm;
			pNewPoint->m_ICm                 = m_pStabDlg->m_ICm;
			pNewPoint->m_GRm                 = m_pStabDlg->m_GRm;
			pNewPoint->m_GYm                 = m_pStabDlg->m_GYm;
			pNewPoint->m_VYm                 = m_pStabDlg->m_VYm;
			pNewPoint->m_IYm                 = m_pStabDlg->m_IYm;

			pNewPoint->m_XCP                 = m_pStabDlg->m_XCP;
			pNewPoint->m_YCP                 = m_pStabDlg->m_YCP;

			if(m_pCurWPolar->m_Type==7) pNewPoint->m_Ctrl = m_pStabDlg->m_Ctrl;
			else                        pNewPoint->m_Ctrl = 0.0;

			for (l=0; l<m_pCurWing->m_NStation; l++)
			{
				pNewPoint->m_SpanPos[l]   = m_pCurWing->m_SpanPos[l];
				pNewPoint->m_StripArea[l] = m_pCurWing->m_StripArea[l];
				if(fabs(m_pCurWing->m_BendingMoment[l])>fabs(Cb))	Cb = m_pCurWing->m_BendingMoment[l];
			}
			memcpy(pNewPoint->m_Cl,         m_pCurWing->m_Cl,         sizeof(m_pCurWing->m_Cl));
			memcpy(pNewPoint->m_PCd,        m_pCurWing->m_PCd,        sizeof(m_pCurWing->m_PCd));
			memcpy(pNewPoint->m_Cm,         m_pCurWing->m_Cm,         sizeof(m_pCurWing->m_Cm));
			memcpy(pNewPoint->m_CmAirf,     m_pCurWing->m_CmAirf,     sizeof(m_pCurWing->m_CmAirf));
			memcpy(pNewPoint->m_XCPSpanRel, m_pCurWing->m_XCPSpanRel, sizeof(m_pCurWing->m_XCPSpanRel));
			memcpy(pNewPoint->m_XCPSpanAbs, m_pCurWing->m_XCPSpanAbs, sizeof(m_pCurWing->m_XCPSpanAbs));
			memcpy(pNewPoint->m_Re,         m_pCurWing->m_Re,         sizeof(m_pCurWing->m_Re));
			memcpy(pNewPoint->m_Chord,      m_pCurWing->m_Chord,      sizeof(m_pCurWing->m_Chord));
			memcpy(pNewPoint->m_Twist,      m_pCurWing->m_Twist,      sizeof(m_pCurWing->m_Twist));
			memcpy(pNewPoint->m_XTrTop,     m_pCurWing->m_XTrTop,     sizeof(m_pCurWing->m_XTrTop));
			memcpy(pNewPoint->m_XTrBot,     m_pCurWing->m_XTrBot,     sizeof(m_pCurWing->m_XTrBot));
			memcpy(pNewPoint->m_Vd,         m_pCurWing->m_Vd,         sizeof(pNewPoint->m_Vd));
			memcpy(pNewPoint->m_F,          m_pCurWing->m_F,          sizeof(pNewPoint->m_F));
			memcpy(pNewPoint->m_BendingMoment, m_pCurWing->m_BendingMoment, sizeof(m_pCurWing->m_BendingMoment));
			memcpy(pNewPoint->m_ICd,        m_pCurWing->m_ICd, sizeof(pNewPoint->m_ICd));
			memcpy(pNewPoint->m_Ai,         m_pCurWing->m_Ai,  sizeof(pNewPoint->m_Ai));

			memcpy(pNewPoint->m_Cp,         m_pStabDlg->m_Cp,  sizeof(pNewPoint->m_Cp));

			memcpy(pNewPoint->m_G, Gamma, sizeof(pNewPoint->m_G));

			if(m_pCurWPolar->m_bWakeRollUp)
			{
				pNewPoint->m_nWakeNodes     = m_nWakeNodes;
				pNewPoint->m_NXWakePanels   = m_pCurWPolar->m_NXWakePanels;
				pNewPoint->m_FirstWakePanel = m_pCurWPolar->m_TotalWakeLength;
				pNewPoint->m_WakeFactor     = m_pCurWPolar->m_WakePanelFactor;
			}
			for (i=0; i<m_pCurWing->m_nFlaps; i++)
				pNewPoint->m_FlapMoment[i] = m_pCurWing->m_FlapMoment[i];

			pNewPoint->m_nFlaps = m_pCurWing->m_nFlaps;
			for(i=0; i<4; i++)
			{
				pNewPoint->m_EigenValue[i]   = m_pStabDlg->m_rLong[i];
				pNewPoint->m_EigenValue[i+4] = m_pStabDlg->m_rLat[i];
				for(l=0;l<4; l++)
				{
					pNewPoint->m_EigenVector[i][l]   = m_pStabDlg->m_vLong[4*i+l];
					pNewPoint->m_EigenVector[i+4][l] = m_pStabDlg->m_vLat[4*i+l];
				}
			}
			m_pStabDlg->GetNDStabDerivatives(pNewPoint);

			memcpy(pNewPoint->m_ALong, m_pStabDlg->m_ALong, 16*sizeof(double));
			memcpy(pNewPoint->m_ALat,  m_pStabDlg->m_ALat,  16*sizeof(double));
			memcpy(pNewPoint->m_BLong, m_pStabDlg->m_BLong, MAXCONTROLS * 4*sizeof(double));
			memcpy(pNewPoint->m_BLat , m_pStabDlg->m_BLat,  MAXCONTROLS * 4*sizeof(double));
		}
		pNewPoint->m_MaxBending = Cb;
	}

	m_pCurWPolar->AddPoint(pNewPoint);
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

	pMainFrame->SetSaveState(false);

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
							i = (int)m_poaWOpp->size();// to break
						}
						else if (pNewPoint->m_Alpha > pWOpp->m_Alpha)
						{
							//insert point
							m_poaWOpp->insert(i, pNewPoint);
							bIsInserted = true;
							i = (int)m_poaWOpp->size();// to break
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
							i = (int)m_poaWOpp->size();// to break
						}
						else if (pNewPoint->m_QInf > pWOpp->m_QInf)
						{
							//insert point
							m_poaWOpp->insert(i, pNewPoint);
							bIsInserted = true;
							i = (int)m_poaWOpp->size();// to break
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
	//Add a WPolar to the m_oaWPolar array
	//Insert the WPolar in alphabetical order
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
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	if(m_iView==W3DVIEW)        m_pctrBottomControls->setCurrentIndex(1);
	else if(m_iView==WSTABVIEW)
	{
		if(m_iStabilityView == 3) m_pctrBottomControls->setCurrentIndex(1);
		else                      m_pctrBottomControls->setCurrentIndex(0);
	}
	else						m_pctrBottomControls->setCurrentIndex(0);

	if(m_iView==WCPVIEW)        m_pctrlMiddleControls->setCurrentIndex(1);
	else if(m_iView==WSTABVIEW) m_pctrlMiddleControls->setCurrentIndex(2);
	else                        m_pctrlMiddleControls->setCurrentIndex(0);

	if(m_iView==WSTABVIEW) pMainFrame->m_pctrlStabViewWidget->show();
	else                   pMainFrame->m_pctrlStabViewWidget->hide();

	if(m_iView==WSTABVIEW)
	{
		m_pctrlTimeView->setChecked(m_iStabilityView==0);
		m_pctrlRLView->setChecked(m_iStabilityView==1);
		m_pctrl3DView->setChecked(m_iStabilityView==3);
	}

	pMainFrame->m_pctrlWOppView->setChecked(m_iView==WOPPVIEW);
	pMainFrame->m_pctrlWPolarView->setChecked(m_iView==WPOLARVIEW);
	pMainFrame->m_pctrl3dView->setChecked(m_iView==W3DVIEW);
	pMainFrame->m_pctrlCpView->setChecked(m_iView==WCPVIEW);
	pMainFrame->m_pctrlStabViewButton->setChecked(m_iView==WSTABVIEW);

	pMainFrame->WOppAct->setChecked(m_iView==WOPPVIEW);
	pMainFrame->WPolarAct->setChecked(m_iView==WPOLARVIEW);
	pMainFrame->W3DAct->setChecked(m_iView==W3DVIEW);
	pMainFrame->CpViewAct->setChecked(m_iView==WCPVIEW);
	pMainFrame->StabPolarAct->setChecked(m_iView==WSTABVIEW);

	pMainFrame->showEllipticCurve->setChecked(m_bShowElliptic);
	pMainFrame->showXCmRefLocation->setChecked(m_bXCmRef);
	pMainFrame->showStabCurve->setChecked(m_bShowStab);
	pMainFrame->showFinCurve->setChecked(m_bShowFin);
	pMainFrame->showWing2Curve->setChecked(m_bShowWing2);

	pMainFrame->showCurWOppOnly->setChecked(m_bCurWOppOnly);

	pMainFrame->WingGraph1->setChecked(m_iWingView==1 && (m_pCurWingGraph == &m_WingGraph1));
	pMainFrame->WingGraph2->setChecked(m_iWingView==1 && (m_pCurWingGraph == &m_WingGraph2));
	pMainFrame->WingGraph3->setChecked(m_iWingView==1 && (m_pCurWingGraph == &m_WingGraph3));
	pMainFrame->WingGraph4->setChecked(m_iWingView==1 && (m_pCurWingGraph == &m_WingGraph4));
	pMainFrame->twoWingGraphs->setChecked(m_iWingView==2);
	pMainFrame->fourWingGraphs->setChecked(m_iWingView==4);

	pMainFrame->WPlrGraph1->setChecked(m_iWPlrView==1 && (m_pCurWPlrGraph == &m_WPlrGraph1));
	pMainFrame->WPlrGraph2->setChecked(m_iWPlrView==1 && (m_pCurWPlrGraph == &m_WPlrGraph2));
	pMainFrame->WPlrGraph3->setChecked(m_iWPlrView==1 && (m_pCurWPlrGraph == &m_WPlrGraph3));
	pMainFrame->WPlrGraph4->setChecked(m_iWPlrView==1 && (m_pCurWPlrGraph == &m_WPlrGraph4));
	pMainFrame->twoWPlrGraphs->setChecked(m_iWPlrView==2);
	pMainFrame->allWPlrGraphs->setChecked(m_iWPlrView==4);


	m_pctrlHalfWing->setEnabled(m_iView==WOPPVIEW);
	m_pctrlLift->setEnabled((m_iView==WOPPVIEW||m_iView==W3DVIEW) && m_pCurWOpp);
	m_pctrlTrans->setEnabled((m_iView==WOPPVIEW||m_iView==W3DVIEW) && m_pCurWOpp);
	m_pctrlWOppAnimate->setEnabled((m_iView==WOPPVIEW||m_iView==W3DVIEW) && m_pCurWOpp);
	m_pctrlAnimateWOppSpeed->setEnabled((m_iView==WOPPVIEW||m_iView==W3DVIEW) && m_pCurWOpp);
	m_pctrlIDrag->setEnabled(m_iView==W3DVIEW && m_pCurWOpp);
	m_pctrlVDrag->setEnabled(m_iView==W3DVIEW && m_pCurWOpp);
	m_pctrlDownwash->setEnabled(m_iView==W3DVIEW && m_pCurWOpp);
	m_pctrlVortices->setEnabled(m_iView==W3DVIEW && m_pCurWOpp);
	m_pctrlMoment->setEnabled(m_iView==W3DVIEW && m_pCurWOpp);
	m_pctrlCp->setEnabled(m_iView==W3DVIEW && m_pCurWOpp && m_pCurWOpp->m_AnalysisType!=1);
	m_pctrlStream->setEnabled(m_iView==W3DVIEW && m_pCurWOpp && m_pCurWOpp->m_AnalysisType!=1);
	m_pctrlSurfVel->setEnabled(m_iView==W3DVIEW && m_pCurWOpp && m_pCurWOpp->m_AnalysisType!=1);
	m_pctrlHighlightOpp->setEnabled(m_iView==WPOLARVIEW);

	pMainFrame->CurBodyMenu->setEnabled(m_pCurBody);
	pMainFrame->EditCurBody->setEnabled(m_pCurBody);
	pMainFrame->exportBodyDef->setEnabled(m_pCurBody);
	pMainFrame->exportBodyGeom->setEnabled(m_pCurBody);

//	pMainFrame->defineCtrlPolar->setEnabled(m_pCurWing);
	pMainFrame->defineWPolar->setEnabled(m_pCurWing);
	pMainFrame->defineStabPolar->setEnabled(m_pCurWing);

	pMainFrame->currentUFOMenu->setEnabled(m_pCurWing);
	pMainFrame->CurWPlrMenu->setEnabled(m_pCurWPolar);
	pMainFrame->CurWOppMenu->setEnabled(m_pCurWOpp);

	m_pctrlLongDynamics->setChecked(m_bLongitudinal);
	m_pctrlLatDynamics->setChecked(!m_bLongitudinal);

	StabViewDlg *pStabView = (StabViewDlg*)pMainFrame->m_pStabView;
	pStabView->SetControls();

	m_pctrlSpanPos->SetValue(m_CurSpanPos);
	m_pctrlCpSectionSlider->setValue((int)(m_CurSpanPos*100.0));
}


void QMiarex::ClientToGL(QPoint const &point, CVector &real)
{
	static double h2, w2;
	h2 = (double)m_rCltRect.height() /2.0;
	w2 = (double)m_rCltRect.width()  /2.0;

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



int QMiarex::CreateBodyElements()
{
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

	if(m_pCurPlane && m_pCurPlane->m_pBody)
	{
		dpx = m_pCurPlane->m_BodyPos.x;
		dpz = m_pCurPlane->m_BodyPos.z;
	}
	else dpx=dpz=0.0;

	if(m_pCurBody->m_LineType==1)
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
						m_Panel[m_MatSize].m_iPos = 100;
						m_Panel[m_MatSize].m_iElement = m_MatSize;
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
	else if(m_pCurBody->m_LineType==2)
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
				m_Panel[m_MatSize].m_iPos = 100;
				m_Panel[m_MatSize].m_iElement = m_MatSize;
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

void QMiarex::CreateCpCurves()
{
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
	if(m_pCurWPolar->m_AnalysisType==2 || m_pCurWPolar->m_bThinSurfaces)
	{
		b2Sides = false;
		coef = 1;
	}

	m_CurSpanPos = qMax(-1.0, m_CurSpanPos);
	m_CurSpanPos = qMin( 1.0, m_CurSpanPos);
	SpanPos = m_CurSpanPos*m_pCurWOpp->m_Span/2.000001;

	str1 = m_pCurWing->m_WingName;
	str2 = QString(" a=%1").arg(m_pCurWOpp->m_Alpha,5,'f',2);
	str3 = QString(" x/c=%1").arg(m_CurSpanPos,5,'f',2);

//	if(m_bCurWOppOnly)
	{
		p=0;
		bFound = false;
		if(m_pCurWPolar->m_AnalysisType==2 || m_pCurWPolar->m_bThinSurfaces) p+=m_pCurWing->m_Surface[0].m_NXPanels;

		SpanInc = -m_pCurWing->m_PlanformSpan/2.0;
		for (p=0; p<m_pCurWing->m_MatSize; p++)
		{
			if(m_pCurWing->m_pPanel[p].m_bIsTrailing && m_pCurWing->m_pPanel[p].m_iPos<=0)
			{
				SpanInc += m_pCurWing->m_pPanel[p].GetWidth();
				if(SpanPos<=SpanInc || fabs(SpanPos-SpanInc)/m_pCurWing->m_PlanformSpan<0.001)
				{
					bFound = true;
					break;
				}
			}
		}

/*		for (p=0; p<m_pCurWing->m_MatSize; p++)
		{
			iTA = m_pCurWing->m_pPanel[p].m_iTA;
			iTB = m_pCurWing->m_pPanel[p].m_iTB;

			if(m_Node[iTA].y<=SpanPos && SpanPos<=m_Node[iTB].y)
			{
				bFound = true;
				break;
			}
		}*/

		if(bFound)
		{
			pCurve = m_CpGraph.GetCurve(0);
			if(m_pCurPOpp)
			{
				pCurve->SetColor(m_pCurPOpp->m_Color);
				pCurve->SetStyle(m_pCurPOpp->m_Style);
				pCurve->SetWidth(m_pCurPOpp->m_Width);
				pCurve->ShowPoints(m_pCurPOpp->m_bShowPoints);
			}
			else
			{
				pCurve->SetColor(m_pCurWOpp->m_Color);
				pCurve->SetStyle(m_pCurWOpp->m_Style);
				pCurve->SetWidth(m_pCurWOpp->m_Width);
				pCurve->ShowPoints(m_pCurWOpp->m_bShowPoints);
			}
			pCurve->SetColor(m_CpColor);
			pCurve->SetStyle(m_CpStyle);
			pCurve->SetWidth(m_CpWidth);
			pCurve->ShowPoints(m_bShowCpPoints);

			pCurve->SetTitle(str1+str2+str3);
			for (pp=p; pp<p+coef*m_pCurWing->m_Surface[0].m_NXPanels; pp++)
			{
				pCurve->AddPoint(m_Panel[pp].CollPt.x, m_pCurWOpp->m_Cp[pp]);
			}
		}

		if(m_pCurWing2 && m_bShowWing2)
		{
			p=0;
			bFound = false;
			if(m_pCurWPolar->m_AnalysisType==2 || m_pCurWPolar->m_bThinSurfaces) p+=m_pCurWing2->m_Surface[0].m_NXPanels;

			SpanInc = -m_pCurWing2->m_PlanformSpan/2.0;
			for (p=0; p<m_pCurWing2->m_MatSize; p++)
			{
				if(m_pCurWing2->m_pPanel[p].m_bIsTrailing && m_pCurWing2->m_pPanel[p].m_iPos<=0)
				{
					SpanInc += m_pCurWing2->m_pPanel[p].GetWidth();
					if(SpanPos<=SpanInc || fabs(SpanPos-SpanInc)/m_pCurWing2->m_PlanformSpan<0.001)
					{
						bFound = true;
						break;
					}
				}
			}

			if(bFound)
			{
				pCurve = m_CpGraph.GetCurve(1);
				pCurve->SetColor(m_CpColor);
				pCurve->SetStyle(m_CpStyle);
				pCurve->SetWidth(m_CpWidth);
				pCurve->ShowPoints(m_bShowCpPoints);
				pCurve->SetTitle(tr("Wing2_")+str2+str3);

				for (pp=p; pp<p+coef*m_pCurWing2->m_Surface[0].m_NXPanels; pp++)
				{
					pCurve->AddPoint(m_Panel[pp].CollPt.x, m_pCurPOpp->m_Wing2WOpp.m_Cp[pp]);
				}
			}
		}

		if(m_pCurStab && m_bShowStab)
		{
			p=0;
			bFound = false;
			if(m_pCurWPolar->m_AnalysisType==2 || m_pCurWPolar->m_bThinSurfaces) p+=m_pCurStab->m_Surface[0].m_NXPanels;

			SpanInc = -m_pCurStab->m_PlanformSpan/2.0;
			for (p=0; p<m_pCurStab->m_MatSize; p++)
			{
				if(m_pCurStab->m_pPanel[p].m_bIsTrailing && m_pCurStab->m_pPanel[p].m_iPos<=0)
				{
					SpanInc += m_pCurStab->m_pPanel[p].GetWidth();
					if(SpanPos<=SpanInc || fabs(SpanPos-SpanInc)/m_pCurStab->m_PlanformSpan<0.001)
					{
						bFound = true;
						break;
					}
				}
			}


			if(bFound)
			{
				pCurve = m_CpGraph.GetCurve(2);
				pCurve->SetColor(m_CpColor);
				pCurve->SetStyle(m_CpStyle);
				pCurve->SetWidth(m_CpWidth);
				pCurve->ShowPoints(m_bShowCpPoints);
				pCurve->SetTitle(tr("Elevator_")+str2+str3);

				for (pp=p; pp<p+coef*m_pCurStab->m_Surface[0].m_NXPanels; pp++)
				{
					pCurve->AddPoint(m_Panel[pp].CollPt.x, m_pCurPOpp->m_StabWOpp.m_Cp[pp]);
				}
			}

		}
		if(m_pCurFin && m_bShowFin)
		{
			p=0;
			bFound = false;
			if(m_pCurWPolar->m_AnalysisType==2 || m_pCurWPolar->m_bThinSurfaces) p+=m_pCurFin->m_Surface[0].m_NXPanels;

			SpanInc = -m_pCurFin->m_PlanformSpan/2.0;
			for (p=0; p<m_pCurFin->m_MatSize; p++)
			{
				if(m_pCurFin->m_pPanel[p].m_bIsTrailing && m_pCurFin->m_pPanel[p].m_iPos<=0)
				{
					SpanInc += m_pCurFin->m_pPanel[p].GetWidth();
					if(SpanPos<=SpanInc || fabs(SpanPos-SpanInc)/m_pCurFin->m_PlanformSpan<0.001)
					{
						bFound = true;
						break;
					}
				}
			}



			if(bFound)
			{
				pCurve = m_CpGraph.GetCurve(3);
				pCurve->SetColor(m_CpColor);
				pCurve->SetStyle(m_CpStyle);
				pCurve->SetWidth(m_CpWidth);
				pCurve->ShowPoints(m_bShowCpPoints);
				pCurve->SetTitle(tr("Fin_")+str2+str3);

				for (pp=p; pp<p+coef*m_pCurFin->m_Surface[0].m_NXPanels; pp++)
				{
					pCurve->AddPoint(m_Panel[pp].CollPt.x, m_pCurPOpp->m_FinWOpp.m_Cp[pp]);
				}
			}
		}
	}
}


int QMiarex::CreateElements(CSurface *pSurface)
{
	//TODO : for  a gap at the wing's center, need to separate m_iPL and m_iPR at the tips;
	bool bNoJoinFlap ;
	int k,l;
	int n0, n1, n2, n3;

	int InitialSize = m_MatSize;
	int side;
	CVector LA, LB, TA, TB;

	if ((!m_pCurWPolar && pSurface->m_bIsTipLeft) ||
		( m_pCurWPolar && m_pCurWPolar->m_AnalysisType==3 && pSurface->m_bIsTipLeft && !m_pCurWPolar->m_bThinSurfaces))
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
			m_Panel[m_MatSize].SetFrame(LA, LB, TA, TB);
			m_Panel[m_MatSize].m_iWake = -1;

			m_Panel[m_MatSize].m_iPD = m_MatSize-1;
			m_Panel[m_MatSize].m_iPU = m_MatSize+1;
			if(l==0)						m_Panel[m_MatSize].m_iPD = -1;// no panel downstream
			if(l==pSurface->m_NXPanels-1)	m_Panel[m_MatSize].m_iPU = -1;// no panel upstream
			m_Panel[m_MatSize].m_iPL = -1;
			m_Panel[m_MatSize].m_iPR = -1;

			m_MatSize++;
		}
	}

	for (k=0; k<pSurface->m_NYPanels; k++)
	{
		//add "horizontal" panels, mid side, or following a strip from bot to top if 3D Panel
		if(!m_pCurWPolar)                        side = -1;
		else if(m_pCurWPolar->m_AnalysisType==1) side =  0;
		else
		{
			if(m_pCurWPolar->m_AnalysisType==2 || m_pCurWPolar->m_AnalysisType==4 || m_pCurWPolar->m_bThinSurfaces)
				side = 0;
			else if (m_pCurWPolar->m_AnalysisType==3)
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

			if(l==0)						m_Panel[m_MatSize].m_bIsTrailing = true;
			if(l==pSurface->m_NXPanels-1) 	m_Panel[m_MatSize].m_bIsLeading  = true;
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

			if(side==0)			m_Panel[m_MatSize].SetFrame(pSurface->LA, pSurface->LB, pSurface->TA, pSurface->TB);
			else if (side==-1)  m_Panel[m_MatSize].SetFrame(pSurface->LB, pSurface->LA, pSurface->TB, pSurface->TA);

			// set neighbour panels
			// valid only for Panel 2-sided Analysis
			// we are on the bottom or middle surface
			m_Panel[m_MatSize].m_iPD = m_MatSize-1;
			m_Panel[m_MatSize].m_iPU = m_MatSize+1;
			if(l==0)						            m_Panel[m_MatSize].m_iPD = -1;// no panel downstream
			if(l==pSurface->m_NXPanels-1 && side==0)	m_Panel[m_MatSize].m_iPU = -1;// no panel upstream

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

			if(m_pCurWPolar && m_Panel[m_MatSize].m_bIsTrailing && m_pCurWPolar->m_AnalysisType>=2)
			{
				m_Panel[m_MatSize].m_iWake = m_WakeSize;//next wake element
				m_Panel[m_MatSize].m_iWakeColumn = m_NWakeColumn;
				if(m_pCurWPolar->m_AnalysisType==2 || m_pCurWPolar->m_AnalysisType==4 || m_pCurWPolar->m_bThinSurfaces)
				{
					CreateWakeElems(m_MatSize);
					m_NWakeColumn++;
				}
			}

			if(l<pSurface->m_NXFlap) pSurface->AddFlapPanel(m_Panel+m_MatSize);

			m_MatSize++;
		}

		if (!m_pCurWPolar ||
			(m_pCurWPolar && m_pCurWPolar->m_AnalysisType==3 && !m_pCurWPolar->m_bThinSurfaces))
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

				if(l==0)					  m_Panel[m_MatSize].m_bIsTrailing = true;
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

				m_Panel[m_MatSize].SetFrame(pSurface->LA, pSurface->LB, pSurface->TA, pSurface->TB);

				// set neighbour panels
				// valid only for Panel 2-sided Analysis
				// we are on the top surface
				m_Panel[m_MatSize].m_iPD = m_MatSize+1;
				m_Panel[m_MatSize].m_iPU = m_MatSize-1;
				if(l==0)						m_Panel[m_MatSize].m_iPD = -1;// no panel downstream
//				if(l==pSurface->m_NXPanels-1)	m_Panel[m_MatSize].m_iPU = -1;// no panel upstream

				m_Panel[m_MatSize].m_iPL = m_MatSize - 2*pSurface->m_NXPanels;//assuming all wing panels have same chordwise distribution
				m_Panel[m_MatSize].m_iPR = m_MatSize + 2*pSurface->m_NXPanels;//assuming all wing panels have same chordwise distribution

				if(k==0                      && pSurface->m_bIsTipLeft)		m_Panel[m_MatSize].m_iPL = -1;
				if(k==pSurface->m_NYPanels-1 && pSurface->m_bIsTipRight)	m_Panel[m_MatSize].m_iPR = -1;

				//do not link to next surfaces... will be done in JoinSurfaces() if surfaces are continuous
				if(k==0)                      m_Panel[m_MatSize].m_iPL = -1;
				if(k==pSurface->m_NYPanels-1) m_Panel[m_MatSize].m_iPR = -1;


				if(m_pCurWPolar && m_Panel[m_MatSize].m_bIsTrailing && m_pCurWPolar->m_AnalysisType==3)
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
		( m_pCurWPolar && m_pCurWPolar->m_AnalysisType==3 && pSurface->m_bIsTipRight && !m_pCurWPolar->m_bThinSurfaces))
	{	//right tip surface
		k = pSurface->m_NYPanels-1;
		for (l=0; l< pSurface->m_NXPanels; l++){

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
			if(l==0)						m_Panel[m_MatSize].m_iPD = -1;// no panel downstream
			if(l==pSurface->m_NXPanels-1)	m_Panel[m_MatSize].m_iPU = -1;// no panel upstream
			m_Panel[m_MatSize].m_iPL = -1;
			m_Panel[m_MatSize].m_iPR = -1;

			m_Panel[m_MatSize].m_iPos = 50;
			m_Panel[m_MatSize].m_iElement = m_MatSize;
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
	int i,j;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
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
	pWOpp->m_AnalysisType        = m_pCurWPolar->m_AnalysisType;

	if(m_pCurWPolar->m_AnalysisType==2 && m_pVLMDlg)
	{
		pWOpp->m_bOut                = m_pVLMDlg->m_bPointOut;
		pWOpp->m_Alpha               = m_pVLMDlg->m_AlphaMin;
		pWOpp->m_QInf                = m_pVLMDlg->m_OpQInf;
	}
	else if(m_pCurWPolar->m_AnalysisType==3 && m_pPanelDlg)
	{
		pWOpp->m_bOut                = m_pPanelDlg->m_bPointOut;
		pWOpp->m_Alpha               = m_pPanelDlg->m_Alpha;
		pWOpp->m_QInf                = m_pPanelDlg->m_QInf;
	}
	else if(m_pCurWPolar->m_AnalysisType==4 && m_pStabDlg)
	{
		pWOpp->m_bOut                = m_pStabDlg->m_bPointOut;
		pWOpp->m_Alpha               = m_pStabDlg->m_AlphaEq;
		pWOpp->m_QInf                = m_pStabDlg->u0;
	}
	pWOpp->m_Beta                = m_pCurWPolar->m_Beta;
	pWOpp->m_Phi                 = m_pCurWPolar->m_BankAngle;

	pWOpp->m_Weight              = m_pCurWPolar->m_Weight;
	pWOpp->m_Span                = m_pCurWPolar->m_WSpan;
	pWOpp->m_MAChord             = pWing->m_MAChord;
	pWOpp->m_CL                  = pWing->m_CL;
	pWOpp->m_InducedDrag         = pWing->m_InducedDrag;
	pWOpp->m_ViscousDrag         = pWing->m_ViscousDrag;

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


void QMiarex::CreateWOppCurves()
{
	//Create the WOpp graph curves
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	
	//declare the variables as static to save memory allocation times
	static CWOpp *pWOpp = NULL;;
	static CPOpp *pPOpp = NULL;
	static CCurve *pCurve1, *pCurve2, *pCurve3, *pCurve4;
	static CCurve *pWing2Curve1, *pWing2Curve2, *pWing2Curve3, *pWing2Curve4;
	static CCurve *pStabCurve1, *pStabCurve2, *pStabCurve3, *pStabCurve4;
	static CCurve *pFinCurve1, *pFinCurve2, *pFinCurve3, *pFinCurve4;
	static QString str;
	static int i,k;

	m_WingGraph1.DeleteCurves();
	m_WingGraph2.DeleteCurves();
	m_WingGraph3.DeleteCurves();
	m_WingGraph4.DeleteCurves();


	if(m_bCurWOppOnly)
	{
		if(!m_pCurWOpp || !m_pCurWOpp->m_bIsVisible) return;
		pCurve1    = m_WingGraph1.AddCurve();
		pCurve2    = m_WingGraph2.AddCurve();
		pCurve3    = m_WingGraph3.AddCurve();
		pCurve4    = m_WingGraph4.AddCurve();
		if(m_pCurWOpp->m_bShowPoints)
		{
			pCurve1->ShowPoints(true);
			pCurve2->ShowPoints(true);
			pCurve3->ShowPoints(true);
			pCurve4->ShowPoints(true);
		}
		pCurve1->SetStyle(m_pCurWOpp->m_Style);
		pCurve1->SetColor(m_pCurWOpp->m_Color);
		pCurve1->SetWidth(m_pCurWOpp->m_Width);
		pCurve2->SetStyle(m_pCurWOpp->m_Style);
		pCurve2->SetColor(m_pCurWOpp->m_Color);
		pCurve2->SetWidth(m_pCurWOpp->m_Width);
		pCurve3->SetStyle(m_pCurWOpp->m_Style);
		pCurve3->SetColor(m_pCurWOpp->m_Color);
		pCurve3->SetWidth(m_pCurWOpp->m_Width);
		pCurve4->SetStyle(m_pCurWOpp->m_Style);
		pCurve4->SetColor(m_pCurWOpp->m_Color);
		pCurve4->SetWidth(m_pCurWOpp->m_Width);
		str = QString("Q=%1 - Alpha=%2").arg(m_pCurWOpp->m_QInf,5,'f',2).arg(m_pCurWOpp->m_Alpha,5,'f',2);

		pCurve1->SetTitle(str);
		pCurve2->SetTitle(str);
		pCurve3->SetTitle(str);
		pCurve4->SetTitle(str);

		FillWOppCurve(m_pCurWOpp, &m_WingGraph1, pCurve1);
		FillWOppCurve(m_pCurWOpp, &m_WingGraph2, pCurve2);
		FillWOppCurve(m_pCurWOpp, &m_WingGraph3, pCurve3);
		FillWOppCurve(m_pCurWOpp, &m_WingGraph4, pCurve4);

		if(m_pCurPOpp && m_pCurPOpp->m_bIsVisible && m_bShowWing2 && m_pCurPOpp->m_bBiplane)
		{
			pWing2Curve1    = m_WingGraph1.AddCurve();
			pWing2Curve2    = m_WingGraph2.AddCurve();
			pWing2Curve3    = m_WingGraph3.AddCurve();
			pWing2Curve4    = m_WingGraph4.AddCurve();
			if(m_pCurPOpp->m_bShowPoints)
			{
				pWing2Curve1->ShowPoints(true);
				pWing2Curve2->ShowPoints(true);
				pWing2Curve3->ShowPoints(true);
				pWing2Curve4->ShowPoints(true);
			}
			pWing2Curve1->SetStyle(m_pCurPOpp->m_Style);
			pWing2Curve1->SetColor(m_pCurPOpp->m_Color);
			pWing2Curve1->SetWidth(m_pCurPOpp->m_Width);
			pWing2Curve2->SetStyle(m_pCurPOpp->m_Style);
			pWing2Curve2->SetColor(m_pCurPOpp->m_Color);
			pWing2Curve2->SetWidth(m_pCurPOpp->m_Width);
			pWing2Curve3->SetStyle(m_pCurPOpp->m_Style);
			pWing2Curve3->SetColor(m_pCurPOpp->m_Color);
			pWing2Curve3->SetWidth(m_pCurPOpp->m_Width);
			pWing2Curve4->SetStyle(m_pCurPOpp->m_Style);
			pWing2Curve4->SetColor(m_pCurPOpp->m_Color);
			pWing2Curve4->SetWidth(m_pCurPOpp->m_Width);

			pWing2Curve1->SetTitle(str);
			pWing2Curve2->SetTitle(str);
			pWing2Curve3->SetTitle(str);
			pWing2Curve4->SetTitle(str);

			FillWOppCurve(&m_pCurPOpp->m_Wing2WOpp, &m_WingGraph1, pWing2Curve1);
			FillWOppCurve(&m_pCurPOpp->m_Wing2WOpp, &m_WingGraph2, pWing2Curve2);
			FillWOppCurve(&m_pCurPOpp->m_Wing2WOpp, &m_WingGraph3, pWing2Curve3);
			FillWOppCurve(&m_pCurPOpp->m_Wing2WOpp, &m_WingGraph4, pWing2Curve4);
		}
		if(m_pCurPOpp && m_pCurPOpp->m_bIsVisible && m_bShowStab && m_pCurPOpp->m_bStab)
		{
			pStabCurve1    = m_WingGraph1.AddCurve();
			pStabCurve2    = m_WingGraph2.AddCurve();
			pStabCurve3    = m_WingGraph3.AddCurve();
			pStabCurve4    = m_WingGraph4.AddCurve();
			if(m_pCurPOpp->m_bShowPoints)
			{
				pStabCurve1->ShowPoints(true);
				pStabCurve2->ShowPoints(true);
				pStabCurve3->ShowPoints(true);
				pStabCurve4->ShowPoints(true);
			}
			pStabCurve1->SetStyle(m_pCurPOpp->m_Style);
			pStabCurve1->SetColor(m_pCurPOpp->m_Color);
			pStabCurve1->SetWidth(m_pCurPOpp->m_Width);
			pStabCurve2->SetStyle(m_pCurPOpp->m_Style);
			pStabCurve2->SetColor(m_pCurPOpp->m_Color);
			pStabCurve2->SetWidth(m_pCurPOpp->m_Width);
			pStabCurve3->SetStyle(m_pCurPOpp->m_Style);
			pStabCurve3->SetColor(m_pCurPOpp->m_Color);
			pStabCurve3->SetWidth(m_pCurPOpp->m_Width);
			pStabCurve4->SetStyle(m_pCurPOpp->m_Style);
			pStabCurve4->SetColor(m_pCurPOpp->m_Color);
			pStabCurve4->SetWidth(m_pCurPOpp->m_Width);

			pStabCurve1->SetTitle(str);
			pStabCurve2->SetTitle(str);
			pStabCurve3->SetTitle(str);
			pStabCurve4->SetTitle(str);

			FillWOppCurve(&m_pCurPOpp->m_StabWOpp, &m_WingGraph1, pStabCurve1);
			FillWOppCurve(&m_pCurPOpp->m_StabWOpp, &m_WingGraph2, pStabCurve2);
			FillWOppCurve(&m_pCurPOpp->m_StabWOpp, &m_WingGraph3, pStabCurve3);
			FillWOppCurve(&m_pCurPOpp->m_StabWOpp, &m_WingGraph4, pStabCurve4);
		}
		if(m_pCurPOpp && m_pCurPOpp->m_bIsVisible && m_bShowFin && m_pCurPOpp->m_bFin)
		{
			pFinCurve1    = m_WingGraph1.AddCurve();
			pFinCurve2    = m_WingGraph2.AddCurve();
			pFinCurve3    = m_WingGraph3.AddCurve();
			pFinCurve4    = m_WingGraph4.AddCurve();
			if(m_pCurPOpp->m_bShowPoints)
			{
				pFinCurve1->ShowPoints(true);
				pFinCurve2->ShowPoints(true);
				pFinCurve3->ShowPoints(true);
				pFinCurve4->ShowPoints(true);
			}
			pFinCurve1->SetStyle(m_pCurPOpp->m_Style);
			pFinCurve1->SetColor(m_pCurPOpp->m_Color);
			pFinCurve1->SetWidth(m_pCurPOpp->m_Width);
			pFinCurve2->SetStyle(m_pCurPOpp->m_Style);
			pFinCurve2->SetColor(m_pCurPOpp->m_Color);
			pFinCurve2->SetWidth(m_pCurPOpp->m_Width);
			pFinCurve3->SetStyle(m_pCurPOpp->m_Style);
			pFinCurve3->SetColor(m_pCurPOpp->m_Color);
			pFinCurve3->SetWidth(m_pCurPOpp->m_Width);
			pFinCurve4->SetStyle(m_pCurPOpp->m_Style);
			pFinCurve4->SetColor(m_pCurPOpp->m_Color);
			pFinCurve4->SetWidth(m_pCurPOpp->m_Width);
			pFinCurve1->SetTitle(str);
			pFinCurve2->SetTitle(str);
			pFinCurve3->SetTitle(str);
			pFinCurve4->SetTitle(str);

			FillWOppCurve(&m_pCurPOpp->m_FinWOpp, &m_WingGraph1, pFinCurve1);
			FillWOppCurve(&m_pCurPOpp->m_FinWOpp, &m_WingGraph2, pFinCurve2);
			FillWOppCurve(&m_pCurPOpp->m_FinWOpp, &m_WingGraph3, pFinCurve3);
			FillWOppCurve(&m_pCurPOpp->m_FinWOpp, &m_WingGraph4, pFinCurve4);
		}
	}
	else
	{
		for (k=0; k<m_poaWOpp->size(); k++)
		{
			pWOpp = (CWOpp*)m_poaWOpp->at(k);
			if (pWOpp->m_bIsVisible)
			{
				pCurve1    = m_WingGraph1.AddCurve();
				pCurve2    = m_WingGraph2.AddCurve();
				pCurve3    = m_WingGraph3.AddCurve();
				pCurve4    = m_WingGraph4.AddCurve();

				if(pWOpp->m_bShowPoints)
				{
					pCurve1->ShowPoints(true);
					pCurve2->ShowPoints(true);
					pCurve3->ShowPoints(true);
					pCurve4->ShowPoints(true);
				}

				pCurve1->SetStyle(pWOpp->m_Style);
				pCurve1->SetColor(pWOpp->m_Color);
				pCurve1->SetWidth(pWOpp->m_Width);
				pCurve2->SetStyle(pWOpp->m_Style);
				pCurve2->SetColor(pWOpp->m_Color);
				pCurve2->SetWidth(pWOpp->m_Width);
				pCurve3->SetStyle(pWOpp->m_Style);
				pCurve3->SetColor(pWOpp->m_Color);
				pCurve3->SetWidth(pWOpp->m_Width);
				pCurve4->SetStyle(pWOpp->m_Style);
				pCurve4->SetColor(pWOpp->m_Color);
				pCurve4->SetWidth(pWOpp->m_Width);

				str = QString("Q=%1 - Alpha=%2").arg(pWOpp->m_QInf,5,'f',2).arg(pWOpp->m_Alpha,5,'f',2);

				pCurve1->SetTitle(str);
				pCurve2->SetTitle(str);
				pCurve3->SetTitle(str);
				pCurve4->SetTitle(str);

				FillWOppCurve(pWOpp, &m_WingGraph1, pCurve1);
				FillWOppCurve(pWOpp, &m_WingGraph2, pCurve2);
				FillWOppCurve(pWOpp, &m_WingGraph3, pCurve3);
				FillWOppCurve(pWOpp, &m_WingGraph4, pCurve4);
			}
		}
		for (k=0; k<m_poaPOpp->size(); k++)
		{
			pPOpp = (CPOpp*)m_poaPOpp->at(k);

			if (pPOpp->m_bIsVisible)
			{
				pCurve1    = m_WingGraph1.AddCurve();
				pCurve2    = m_WingGraph2.AddCurve();
				pCurve3    = m_WingGraph3.AddCurve();
				pCurve4    = m_WingGraph4.AddCurve();

				if(pPOpp->m_bShowPoints)
				{
					pCurve1->ShowPoints(true);
					pCurve2->ShowPoints(true);
					pCurve3->ShowPoints(true);
					pCurve4->ShowPoints(true);
				}

				pCurve1->SetStyle(pPOpp->m_Style);
				pCurve1->SetColor(pPOpp->m_Color);
				pCurve1->SetWidth(pPOpp->m_Width);
				pCurve2->SetStyle(pPOpp->m_Style);
				pCurve2->SetColor(pPOpp->m_Color);
				pCurve2->SetWidth(pPOpp->m_Width);
				pCurve3->SetStyle(pPOpp->m_Style);
				pCurve3->SetColor(pPOpp->m_Color);
				pCurve3->SetWidth(pPOpp->m_Width);
				pCurve4->SetStyle(pPOpp->m_Style);
				pCurve4->SetColor(pPOpp->m_Color);
				pCurve4->SetWidth(pPOpp->m_Width);

				str = QString("Q=%1 - Alpha=%2").arg(pPOpp->m_QInf,5,'f',2).arg(pPOpp->m_Alpha,5,'f',2);

				pCurve1->SetTitle(str);
				pCurve2->SetTitle(str);
				pCurve3->SetTitle(str);
				pCurve4->SetTitle(str);

				FillWOppCurve(&pPOpp->m_WingWOpp, &m_WingGraph1, pCurve1);
				FillWOppCurve(&pPOpp->m_WingWOpp, &m_WingGraph2, pCurve2);
				FillWOppCurve(&pPOpp->m_WingWOpp, &m_WingGraph3, pCurve3);
				FillWOppCurve(&pPOpp->m_WingWOpp, &m_WingGraph4, pCurve4);

				if(m_bShowWing2 && pPOpp->m_bBiplane)
				{
					pWing2Curve1    = m_WingGraph1.AddCurve();
					pWing2Curve2    = m_WingGraph2.AddCurve();
					pWing2Curve3    = m_WingGraph3.AddCurve();
					pWing2Curve4    = m_WingGraph4.AddCurve();
					if(pPOpp->m_bShowPoints)
					{
						pWing2Curve1->ShowPoints(true);
						pWing2Curve2->ShowPoints(true);
						pWing2Curve3->ShowPoints(true);
						pWing2Curve4->ShowPoints(true);
					}
					pWing2Curve1->SetStyle(pPOpp->m_Style);
					pWing2Curve1->SetColor(pPOpp->m_Color);
					pWing2Curve1->SetWidth(pPOpp->m_Width);
					pWing2Curve2->SetStyle(pPOpp->m_Style);
					pWing2Curve2->SetColor(pPOpp->m_Color);
					pWing2Curve2->SetWidth(pPOpp->m_Width);
					pWing2Curve3->SetStyle(pPOpp->m_Style);
					pWing2Curve3->SetColor(pPOpp->m_Color);
					pWing2Curve3->SetWidth(pPOpp->m_Width);
					pWing2Curve4->SetStyle(pPOpp->m_Style);
					pWing2Curve4->SetColor(pPOpp->m_Color);
					pWing2Curve4->SetWidth(pPOpp->m_Width);

					pWing2Curve1->SetTitle(str);
					pWing2Curve2->SetTitle(str);
					pWing2Curve3->SetTitle(str);
					pWing2Curve4->SetTitle(str);

					FillWOppCurve(&pPOpp->m_Wing2WOpp, &m_WingGraph1, pWing2Curve1);
					FillWOppCurve(&pPOpp->m_Wing2WOpp, &m_WingGraph2, pWing2Curve2);
					FillWOppCurve(&pPOpp->m_Wing2WOpp, &m_WingGraph3, pWing2Curve3);
					FillWOppCurve(&pPOpp->m_Wing2WOpp, &m_WingGraph4, pWing2Curve4);
				}
				if(m_bShowStab && pPOpp->m_bStab)
				{
					pStabCurve1    = m_WingGraph1.AddCurve();
					pStabCurve2    = m_WingGraph2.AddCurve();
					pStabCurve3    = m_WingGraph3.AddCurve();
					pStabCurve4    = m_WingGraph4.AddCurve();
					if(pPOpp->m_bShowPoints)
					{
						pStabCurve1->ShowPoints(true);
						pStabCurve2->ShowPoints(true);
						pStabCurve3->ShowPoints(true);
						pStabCurve4->ShowPoints(true);
					}
					pStabCurve1->SetStyle(pPOpp->m_Style);
					pStabCurve1->SetColor(pPOpp->m_Color);
					pStabCurve1->SetWidth(pPOpp->m_Width);
					pStabCurve2->SetStyle(pPOpp->m_Style);
					pStabCurve2->SetColor(pPOpp->m_Color);
					pStabCurve2->SetWidth(pPOpp->m_Width);
					pStabCurve3->SetStyle(pPOpp->m_Style);
					pStabCurve3->SetColor(pPOpp->m_Color);
					pStabCurve3->SetWidth(pPOpp->m_Width);
					pStabCurve4->SetStyle(pPOpp->m_Style);
					pStabCurve4->SetColor(pPOpp->m_Color);
					pStabCurve4->SetWidth(pPOpp->m_Width);

					pStabCurve1->SetTitle(str);
					pStabCurve2->SetTitle(str);
					pStabCurve3->SetTitle(str);
					pStabCurve4->SetTitle(str);

					FillWOppCurve(&pPOpp->m_StabWOpp, &m_WingGraph1, pStabCurve1);
					FillWOppCurve(&pPOpp->m_StabWOpp, &m_WingGraph2, pStabCurve2);
					FillWOppCurve(&pPOpp->m_StabWOpp, &m_WingGraph3, pStabCurve3);
					FillWOppCurve(&pPOpp->m_StabWOpp, &m_WingGraph4, pStabCurve4);
				}
				if(m_bShowFin && pPOpp->m_bFin)
				{
					pFinCurve1    = m_WingGraph1.AddCurve();
					pFinCurve2    = m_WingGraph2.AddCurve();
					pFinCurve3    = m_WingGraph3.AddCurve();
					pFinCurve4    = m_WingGraph4.AddCurve();
					if(pPOpp->m_bShowPoints)
					{
						pFinCurve1->ShowPoints(true);
						pFinCurve2->ShowPoints(true);
						pFinCurve3->ShowPoints(true);
						pFinCurve4->ShowPoints(true);
					}
					pFinCurve1->SetStyle(pPOpp->m_Style);
					pFinCurve1->SetColor(pPOpp->m_Color);
					pFinCurve1->SetWidth(pPOpp->m_Width);
					pFinCurve2->SetStyle(pPOpp->m_Style);
					pFinCurve2->SetColor(pPOpp->m_Color);
					pFinCurve2->SetWidth(pPOpp->m_Width);
					pFinCurve3->SetStyle(pPOpp->m_Style);
					pFinCurve3->SetColor(pPOpp->m_Color);
					pFinCurve3->SetWidth(pPOpp->m_Width);
					pFinCurve4->SetStyle(pPOpp->m_Style);
					pFinCurve4->SetColor(pPOpp->m_Color);
					pFinCurve4->SetWidth(pPOpp->m_Width);

					pFinCurve1->SetTitle(str);
					pFinCurve2->SetTitle(str);
					pFinCurve3->SetTitle(str);
					pFinCurve4->SetTitle(str);

					FillWOppCurve(&pPOpp->m_FinWOpp, &m_WingGraph1, pFinCurve1);
					FillWOppCurve(&pPOpp->m_FinWOpp, &m_WingGraph2, pFinCurve2);
					FillWOppCurve(&pPOpp->m_FinWOpp, &m_WingGraph3, pFinCurve3);
					FillWOppCurve(&pPOpp->m_FinWOpp, &m_WingGraph4, pFinCurve4);
				}
			}
		}
	}
	if(m_bShowElliptic && m_pCurWOpp)
	{
		double maxlift, x, y;
		maxlift = m_pCurWOpp->GetMaxLift();
		int nStart;
		if(m_pCurWOpp->m_AnalysisType==1) nStart = 1;
		else nStart = 0;
		if(m_WingGraph1.GetYVariable()==3)
		{
			CCurve *pCurve = m_WingGraph1.AddCurve();
			pCurve->SetStyle(1);
			pCurve->SetColor(QColor(150, 150, 150));
			for (i=nStart; i<m_pCurWOpp->m_NStation; i++)
			{
				x = m_pCurWOpp->m_SpanPos[i];
				y = maxlift*sqrt(1.0-x*x/m_pCurWOpp->m_Span/m_pCurWOpp->m_Span*4.0);
				pCurve->AddPoint(x*pMainFrame->m_mtoUnit,y);
			}
		}
		if(m_WingGraph2.GetYVariable()==3)
		{
			CCurve *pCurve = m_WingGraph2.AddCurve();
			pCurve->SetStyle(1);
			pCurve->SetColor(QColor(150, 150, 150));
			for (i=nStart; i<m_pCurWOpp->m_NStation; i++)
			{
				x = m_pCurWOpp->m_SpanPos[i];
				y = maxlift*sqrt(1.0-x*x/m_pCurWOpp->m_Span/m_pCurWOpp->m_Span*4.0);
				pCurve->AddPoint(x*pMainFrame->m_mtoUnit,y);
			}
		}
		if(m_WingGraph3.GetYVariable()==3)
		{
			CCurve *pCurve = m_WingGraph3.AddCurve();
			pCurve->SetStyle(1);
			pCurve->SetColor(QColor(150, 150, 150));
			for (i=nStart; i<m_pCurWOpp->m_NStation; i++)
			{
				x = m_pCurWOpp->m_SpanPos[i];
				y = maxlift*sqrt(1.0-x*x/m_pCurWOpp->m_Span/m_pCurWOpp->m_Span*4.0);
				pCurve->AddPoint(x*pMainFrame->m_mtoUnit,y);
			}
		}
		if(m_WingGraph4.GetYVariable()==3)
		{
			CCurve *pCurve = m_WingGraph4.AddCurve();
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


void QMiarex::CreateWPolarCurves()
{
	//resets and creates the WPolar graphs curves
	static CWPolar *pWPolar;
	static CCurve* pWPolarCurve, *pWCzCurve, *pWCmCurve, *pWLDCurve;

	m_WPlrGraph1.DeleteCurves();
	m_WPlrGraph2.DeleteCurves();
	m_WPlrGraph3.DeleteCurves();
	m_WPlrGraph4.DeleteCurves();

	for (int k=0; k<m_poaWPolar->size(); k++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(k);
		if (pWPolar->m_bIsVisible && pWPolar->m_Alpha.size()>0 &&
		   ((m_bType1 && pWPolar->m_Type == 1) ||
		    (m_bType2 && pWPolar->m_Type == 2) ||
		    (m_bType4 && pWPolar->m_Type == 4) ||
		    (m_bType5 && pWPolar->m_Type == 5) ||
		    (m_bType6 && pWPolar->m_Type == 6) ||
		    (m_bType7 && pWPolar->m_Type == 7)))
		{

			pWPolarCurve = m_WPlrGraph1.AddCurve();
			pWCzCurve    = m_WPlrGraph2.AddCurve();
			pWCmCurve    = m_WPlrGraph3.AddCurve();
			pWLDCurve    = m_WPlrGraph4.AddCurve();

			if(pWPolar->m_bShowPoints)
			{
				pWPolarCurve->ShowPoints(true);
				pWCzCurve->ShowPoints(true);
				pWCmCurve->ShowPoints(true);
				pWLDCurve->ShowPoints(true);
			}

			pWPolarCurve->SetStyle(pWPolar->m_Style);
			pWPolarCurve->SetColor(pWPolar->m_Color);
			pWPolarCurve->SetWidth(pWPolar->m_Width);
			pWCzCurve->SetStyle(pWPolar->m_Style);
			pWCzCurve->SetColor(pWPolar->m_Color);
			pWCzCurve->SetWidth(pWPolar->m_Width);
			pWCmCurve->SetStyle(pWPolar->m_Style);
			pWCmCurve->SetColor(pWPolar->m_Color);
			pWCmCurve->SetWidth(pWPolar->m_Width);
			pWLDCurve->SetStyle(pWPolar->m_Style);
			pWLDCurve->SetColor(pWPolar->m_Color);
			pWLDCurve->SetWidth(pWPolar->m_Width);

			FillWPlrCurve(pWPolarCurve, pWPolar, m_WPlrGraph1.GetXVariable(), m_WPlrGraph1.GetYVariable());
			FillWPlrCurve(pWCzCurve, pWPolar, m_WPlrGraph2.GetXVariable(), m_WPlrGraph2.GetYVariable());
			FillWPlrCurve(pWCmCurve, pWPolar, m_WPlrGraph3.GetXVariable(), m_WPlrGraph3.GetYVariable());
			FillWPlrCurve(pWLDCurve, pWPolar, m_WPlrGraph4.GetXVariable(), m_WPlrGraph4.GetYVariable());

			pWPolarCurve->SetTitle(pWPolar->m_PlrName);
			pWCzCurve->SetTitle(pWPolar->m_PlrName);
			pWCmCurve->SetTitle(pWPolar->m_PlrName);
			pWLDCurve->SetTitle(pWPolar->m_PlrName);
		}
	}
}

void QMiarex::CreateStabilityCurves()
{
	if(m_iStabilityView==0)
	{
//		MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
//		StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
		if(m_bForcedResponse) CreateStabRungeKuttaCurves();
		else                  CreateStabTimeCurves();
	}
	else
	{
		CreateStabRLCurves();
	}
}


void QMiarex::CreateStabTimeCurves()
{
	// Builds the initial condition response due to perturbations from steady state
	// The time response is calculated analytically based on the knowledge 
	// of the eigenvalues and eigenvectors

	static complex<double> M[16];// the modal matrix
	static complex<double> InvM[16];// the inverse of the modal matrix
	static int i,j,k;
	static double t, dt, TotalPoints; // the input load
	static double *in;
	static complex<double> q[4],q0[4],y[4];//the part of each mode in the solution
	static CCurve *pCurve0, *pCurve1, *pCurve2, *pCurve3;
	QString strong;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
	
	pCurve0 = m_TimeGraph1.GetCurve(0);
	if(pCurve0) pCurve0->ResetCurve();
	pCurve1 = m_TimeGraph2.GetCurve(0);
	if(pCurve1) pCurve1->ResetCurve();
	pCurve2 = m_TimeGraph3.GetCurve(0);
	if(pCurve2) pCurve2->ResetCurve();
	pCurve3 = m_TimeGraph4.GetCurve(0);
	if(pCurve3) pCurve3->ResetCurve();

	strong = pStabView->m_pctrlCurveTitle->text();

	m_Deltat = pStabView->m_pctrlDeltat->GetValue();
	m_TotalTime = pStabView->m_pctrlTotalTime->GetValue();
	dt = m_TotalTime/1000.;
	if(dt<m_Deltat) dt = m_Deltat;

	TotalPoints = qMin(1000, (int)(m_TotalTime/dt));

	//read the initial state condition
	m_TimeInput[0] = pStabView->m_pctrlStabVar1->GetValue();
	m_TimeInput[1] = pStabView->m_pctrlStabVar2->GetValue();
	m_TimeInput[2] = pStabView->m_pctrlStabVar3->GetValue();
	m_TimeInput[3] = 0.0;//we start with an initial 0.0 value for pitch or bank angles
	in = m_TimeInput;


	if(!m_pCurWOpp || !m_pCurWOpp->m_bIsVisible) return;

	pCurve0->SetColor(m_pCurWOpp->m_Color);
	pCurve1->SetColor(m_pCurWOpp->m_Color);
	pCurve2->SetColor(m_pCurWOpp->m_Color);
	pCurve3->SetColor(m_pCurWOpp->m_Color);
	pCurve0->SetStyle(m_pCurWOpp->m_Style);
	pCurve1->SetStyle(m_pCurWOpp->m_Style);
	pCurve2->SetStyle(m_pCurWOpp->m_Style);
	pCurve3->SetStyle(m_pCurWOpp->m_Style);
	pCurve0->SetWidth(m_pCurWOpp->m_Width);
	pCurve1->SetWidth(m_pCurWOpp->m_Width);
	pCurve2->SetWidth(m_pCurWOpp->m_Width);
	pCurve3->SetWidth(m_pCurWOpp->m_Width);
	pCurve0->ShowPoints(m_pCurWOpp->m_bShowPoints);
	pCurve1->ShowPoints(m_pCurWOpp->m_bShowPoints);
	pCurve2->ShowPoints(m_pCurWOpp->m_bShowPoints);
	pCurve3->ShowPoints(m_pCurWOpp->m_bShowPoints);

	if(m_bLongitudinal)
	{ 
		pCurve0->SetTitle(strong);
		pCurve1->SetTitle(strong);
		pCurve2->SetTitle(strong);
		pCurve3->SetTitle(strong);
	}
	else
	{
		pCurve0->SetTitle(strong);
		pCurve1->SetTitle(strong);
		pCurve2->SetTitle(strong);
		pCurve3->SetTitle(strong);
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
		
		for(i=1; i<TotalPoints; i++)
		{
			t = (double)i * dt;
			q[0] = q0[0] * exp(m_pCurWOpp->m_EigenValue[0]*t);
			q[1] = q0[1] * exp(m_pCurWOpp->m_EigenValue[1]*t);
			q[2] = q0[2] * exp(m_pCurWOpp->m_EigenValue[2]*t);
			q[3] = q0[3] * exp(m_pCurWOpp->m_EigenValue[3]*t);
			y[0] = *(M+4*0+0) * q[0] +*(M+4*0+1) * q[1] +*(M+4*0+2) * q[2] +*(M+4*0+3) * q[3];
			y[1] = *(M+4*1+0) * q[0] +*(M+4*1+1) * q[1] +*(M+4*1+2) * q[2] +*(M+4*1+3) * q[3];
			y[2] = *(M+4*2+0) * q[0] +*(M+4*2+1) * q[1] +*(M+4*2+2) * q[2] +*(M+4*2+3) * q[3];
			y[3] = *(M+4*3+0) * q[0] +*(M+4*3+1) * q[1] +*(M+4*3+2) * q[2] +*(M+4*3+3) * q[3];
	
			pCurve0->AddPoint(t, y[0].real());
			pCurve1->AddPoint(t, y[1].real());
			pCurve2->AddPoint(t, y[2].real());
			pCurve3->AddPoint(t, y[3].real()*180.0/PI);
		}
	}
}


void QMiarex::CreateStabRungeKuttaCurves()
{
	// Builds the forced response from the state matrix and the forced input matrix
	// using a Runge-Kutta integration scheme
	// The forced input is a ramp of all controls simultaneaously
	// from 0 to amp x Control_amplitude in a time RampTime
	// i.e.:
	//	u(t) = min( amp x Control_amplitude, 
	//                  t * amp x Control_amplitude/RampTime)
	//	with t>0

	static int i, j, TotalPoints, PlotInterval;
	static double in[MAXCONTROLS], t, dt, RampTime, u_t;
	static CCurve *pCurve0, *pCurve1, *pCurve2, *pCurve3;
	static double A[4][4], B[MAXCONTROLS][4];
	static double m[5][4];
	static double y[4], yp[4];
	QString strong;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;

	pCurve0 = m_TimeGraph1.GetCurve(0);
	if(pCurve0) pCurve0->ResetCurve();
	pCurve1 = m_TimeGraph2.GetCurve(0);
	if(pCurve1) pCurve1->ResetCurve();
	pCurve2 = m_TimeGraph3.GetCurve(0);
	if(pCurve2) pCurve2->ResetCurve();
	pCurve3 = m_TimeGraph4.GetCurve(0);
	if(pCurve3) pCurve3->ResetCurve();

	//We need a WOpp
	if(!m_pCurWOpp) return;//nothing to plot
	//Check that the current polar is of the stability type
	if(!m_pCurWPolar || m_pCurWPolar->m_Type!=7) return;

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
	pStabView->ReadForcedInput(in);
	
	pCurve0->SetColor(m_pCurWOpp->m_Color);
	pCurve1->SetColor(m_pCurWOpp->m_Color);
	pCurve2->SetColor(m_pCurWOpp->m_Color);
	pCurve3->SetColor(m_pCurWOpp->m_Color);
	pCurve0->SetStyle(m_pCurWOpp->m_Style);
	pCurve1->SetStyle(m_pCurWOpp->m_Style);
	pCurve2->SetStyle(m_pCurWOpp->m_Style);
	pCurve3->SetStyle(m_pCurWOpp->m_Style);
	pCurve0->SetWidth(m_pCurWOpp->m_Width);
	pCurve1->SetWidth(m_pCurWOpp->m_Width);
	pCurve2->SetWidth(m_pCurWOpp->m_Width);
	pCurve3->SetWidth(m_pCurWOpp->m_Width);
	pCurve0->ShowPoints(m_pCurWOpp->m_bShowPoints);
	pCurve1->ShowPoints(m_pCurWOpp->m_bShowPoints);
	pCurve2->ShowPoints(m_pCurWOpp->m_bShowPoints);
	pCurve3->ShowPoints(m_pCurWOpp->m_bShowPoints);

	strong = pStabView->m_pctrlCurveTitle->text();
	if(m_bLongitudinal)
	{
		pCurve0->SetTitle(strong);
		pCurve1->SetTitle(strong);
		pCurve2->SetTitle(strong);
		pCurve3->SetTitle(strong);
	}
	else
	{
		pCurve0->SetTitle(strong);
		pCurve1->SetTitle(strong);
		pCurve2->SetTitle(strong);
		pCurve3->SetTitle(strong);
	}
	
	RampTime    = pStabView->m_pctrlRampTime->GetValue();           //s
	m_Deltat    = pStabView->m_pctrlDeltat->GetValue();
	m_TotalTime = pStabView->m_pctrlTotalTime->GetValue();
	dt = m_TotalTime/1000.;
	if(dt<m_Deltat) dt = m_Deltat;

	TotalPoints  = qMin(1000, (int)(m_TotalTime/dt));
	PlotInterval = qMax(1,(int)(TotalPoints/200));

	t           = 0.0;
	
	//we are considering forced response from initial steady state, so set
	// initial conditions to 0
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

		if(RampTime<=0.0) u_t = 1.0;
		else              u_t = qMin(1.0, t/RampTime);
		for(j=0; j<m_pCurWPolar->m_nControls; j++)
		{
			m[0][0] += B[j][0] * in[j]*u_t;
			m[0][1] += B[j][1] * in[j]*u_t;
			m[0][2] += B[j][2] * in[j]*u_t;
			m[0][3] += B[j][3] * in[j]*u_t;
		}
		
		//middle point m2
		yp[0] = y[0] + dt/2.0 * m[0][0];
		yp[1] = y[1] + dt/2.0 * m[0][1];
		yp[2] = y[2] + dt/2.0 * m[0][2];
		yp[3] = y[3] + dt/2.0 * m[0][3];

		m[1][0] = A[0][0]*yp[0] + A[0][1]*yp[1] + A[0][2]*yp[2] + A[0][3]*yp[3];
		m[1][1] = A[1][0]*yp[0] + A[1][1]*yp[1] + A[1][2]*yp[2] + A[1][3]*yp[3];
		m[1][2] = A[2][0]*yp[0] + A[2][1]*yp[1] + A[2][2]*yp[2] + A[2][3]*yp[3];
		m[1][3] = A[3][0]*yp[0] + A[3][1]*yp[1] + A[3][2]*yp[2] + A[3][3]*yp[3];

		if(RampTime<=0.0) u_t = 1.0;
		else              u_t = qMin(1.0, (t+dt/2.0)/RampTime);
		for(j=0; j<m_pCurWPolar->m_nControls; j++)
		{
			m[1][0] += B[j][0] * in[j]*u_t;
			m[1][1] += B[j][1] * in[j]*u_t;
			m[1][2] += B[j][2] * in[j]*u_t;
			m[1][3] += B[j][3] * in[j]*u_t;
		}
		//second point m3
		yp[0] = y[0] + dt/2.0 * m[1][0];
		yp[1] = y[1] + dt/2.0 * m[1][1];
		yp[2] = y[2] + dt/2.0 * m[1][2];
		yp[3] = y[3] + dt/2.0 * m[1][3];

		m[2][0] = A[0][0]*yp[0] + A[0][1]*yp[1] + A[0][2]*yp[2] + A[0][3]*yp[3];
		m[2][1] = A[1][0]*yp[0] + A[1][1]*yp[1] + A[1][2]*yp[2] + A[1][3]*yp[3];
		m[2][2] = A[2][0]*yp[0] + A[2][1]*yp[1] + A[2][2]*yp[2] + A[2][3]*yp[3];
		m[2][3] = A[3][0]*yp[0] + A[3][1]*yp[1] + A[3][2]*yp[2] + A[3][3]*yp[3];

		if(RampTime<=0.0) u_t = 1.0;
		else              u_t = qMin(1.0, (t+dt/2.0)/RampTime);
		for(j=0; j<m_pCurWPolar->m_nControls; j++)
		{
			m[2][0] += B[j][0] * in[j]*u_t;
			m[2][1] += B[j][1] * in[j]*u_t;
			m[2][2] += B[j][2] * in[j]*u_t;
			m[2][3] += B[j][3] * in[j]*u_t;
		}

		//third point m4
		yp[0] = y[0] + dt * m[2][0];
		yp[1] = y[1] + dt * m[2][1];
		yp[2] = y[2] + dt * m[2][2];
		yp[3] = y[3] + dt * m[2][3];

		m[3][0] = A[0][0]*yp[0] + A[0][1]*yp[1] + A[0][2]*yp[2] + A[0][3]*yp[3];
		m[3][1] = A[1][0]*yp[0] + A[1][1]*yp[1] + A[1][2]*yp[2] + A[1][3]*yp[3];
		m[3][2] = A[2][0]*yp[0] + A[2][1]*yp[1] + A[2][2]*yp[2] + A[2][3]*yp[3];
		m[3][3] = A[3][0]*yp[0] + A[3][1]*yp[1] + A[3][2]*yp[2] + A[3][3]*yp[3];

		if(RampTime<=0.0) u_t = 1.0;
		else              u_t = qMin(1.0, (t+dt)/RampTime);
		for(j=0; j<m_pCurWPolar->m_nControls; j++)
		{
			m[3][0] += B[j][0] * in[j]*u_t;
			m[3][1] += B[j][1] * in[j]*u_t;
			m[3][2] += B[j][2] * in[j]*u_t;
			m[3][3] += B[j][3] * in[j]*u_t;
		}

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

		if(i%PlotInterval==0)
		{
			pCurve0->AddPoint(t, y[0]);
			pCurve1->AddPoint(t, y[1]);
			pCurve2->AddPoint(t, y[2]);
			pCurve3->AddPoint(t, y[3]);
		}

	}
	pCurve0->SetVisible(true);
	pCurve1->SetVisible(true);
	pCurve2->SetVisible(true);
	pCurve3->SetVisible(true);
}


void QMiarex::CreateStabRLCurves()
{
	//resets and creates the WPolar graphs curves
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

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
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
	//draws the WOpps legend to the device context,
	// bottom is the lower limit not to exceed for the legend
	painter.save();
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
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
	//draws the legend for the time response - 4 curves
	// bottom is the lower limit not to exceed for the legend
	painter.save();
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
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

	for (i=0; i<m_TimeGraph1.GetCurveCount(); i++)
	{
		pCurve = m_TimeGraph1.GetCurve(i);
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
	//draws the WOpps legend to the device context,
	// bottom is the lower limit not to exceed for the legend
	painter.save();

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
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

	QFont TextFont;
	m_WingGraph1.GetLegendLogFont(&TextFont);
	painter.setFont(TextFont);

	QFontMetrics fm(TextFont);
	ypos = fm.height();

	QPen TextPen(m_WingGraph1.GetLegendColor());
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
		painter.drawText(place.x() + (int)(1*LegendSize), place.y() + ypos*ny-(int)(ypos/2), m_pCurWing->m_WingName);

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


		if(m_pCurWOpp->m_AnalysisType==1)  str6="_LLT";
		else if(m_pCurWOpp->m_AnalysisType==2)
		{
			if(m_pCurWOpp->m_bVLM1)	str6="_VLM1";
			else 					str6="_VLM2";
		}
		else if(m_pCurWOpp->m_AnalysisType==3)
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

						if(pWOpp->m_AnalysisType==1) str6 ="_LLT";
						else if(pWOpp->m_AnalysisType==2)
						{
							if(pWOpp->m_bVLM1)       str6 ="_VLM1";
							else                     str6 ="_VLM2";
						}
						else if(pWOpp->m_AnalysisType==3)
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
						
						if(pPOpp->m_WingWOpp.m_AnalysisType==2)
						{
							if(pPOpp->m_bVLM1)      str6 ="_VLM1";
							else                    str6 ="_VLM2";
						}
						else if(pPOpp->m_WingWOpp.m_AnalysisType==3)
						{
							str6="_Panel";
							if(pPOpp->m_WingWOpp.m_bThinSurface) str4 += "_Thin";
						}

						if(pPOpp->m_WingWOpp.m_bTiltedGeom) str6+= "_TG";
						if(pPOpp->m_WingWOpp.m_bOut)        str6+=" (Out)";

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
	//draws the WPolar legend to the device context,
	// bottom is the lower limit not to exceed for the legend

	painter.save();

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	int LegendSize, LegendWidth, ypos;
	int i,j,k,l, ny, x1;


	LegendSize = 30;
	LegendWidth = 280;

	QFont TextFont;
	m_WPlrGraph1.GetLegendLogFont(&TextFont);
	painter.setFont(TextFont);

	QFontMetrics fm(TextFont);
	ypos = fm.height();

	QPen TextPen(m_WPlrGraph1.GetLegendColor());
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
			if (pWPolar->m_UFOName == pPlane->m_PlaneName)
			{
				str.append(pPlane->m_PlaneName);
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
				pWPolar->m_PlrName.length() &&
				pWPolar->m_bIsVisible &&
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
					if ( pWPolar->m_Alpha.size() &&	pWPolar->m_PlrName.length() &&	pWPolar->m_bIsVisible &&
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
			if (UFOPlrs) ny++;
		}
	}
	painter.restore();
}


void QMiarex::DuplicatePlane()
{
	//Duplicate the currently selected plane, if any
	if(!m_pCurPlane) return;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
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
	int i;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(!m_pCurPlane) return;
	CWPolar *pWPolar;
	CPOpp* pPOpp;
	bool bHasResults = false;

	for (i=0; i< m_poaWPolar->size(); i++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(i);
		if(pWPolar->m_Alpha.size() && pWPolar->m_UFOName == m_pCurPlane->m_PlaneName)
		{
			bHasResults = true;
			break;
		}
	}

	for (i=0; i< m_poaPOpp->size(); i++)
	{
		pPOpp = (CPOpp*)m_poaPOpp->at(i);
		if(pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName)
		{
			bHasResults = true;
			break;
		}
	}
	//save a copy just in case user changes his mind (users !...)
	CPlane* pSavePlane= new CPlane;

	PlaneDlg PlaneDlg;
	PlaneDlg.m_pPlane = m_pCurPlane;

	pSavePlane->Duplicate(m_pCurPlane);
	PlaneDlg.m_bAcceptName = false;
	PlaneDlg.InitDialog();
	if(QDialog::Accepted == PlaneDlg.exec())
	{
		if(PlaneDlg.m_bChanged)
		{
			if(bHasResults)
			{
//				SetModPlane(m_pCurPlane);
				ModDlg dlg;
				dlg.m_Question = tr("The modification will erase all results associated to this Plane.\nContinue ?");
				dlg.InitDialog();
				int Ans = dlg.exec();
				if (Ans == QDialog::Rejected)
				{
					//restore geometry
					m_pCurPlane->Duplicate(pSavePlane);
					delete pSavePlane; // clean up
					return;
				}
				else if(Ans==20)
				{
					//save mods to a new plane object
					CPlane* pNewPlane= new CPlane();
					pNewPlane->Duplicate(m_pCurPlane);
					//restore geometry for initial plane
					m_pCurPlane->Duplicate(pSavePlane);

					if(!SetModPlane(pNewPlane))
					{
						delete pNewPlane;
					}
					else
					{
						m_pCurPlane = AddPlane(pNewPlane);
					}
					SetUFO();
					pMainFrame->UpdateUFOs();
					UpdateView();
					delete pSavePlane; // clean up
					return;
				}
			}


			//then modifications are automatically recorded
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
		m_pCurPlane->Duplicate(pSavePlane);
		delete pSavePlane; // clean up
		return ;
	}
	delete pSavePlane; // clean up
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
	//The curve has been created, fill the curve with the WOpp data
	int Var = pGraph->GetYVariable();
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	int nStart, i;
	if(pWOpp->m_AnalysisType==1) nStart = 1;
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
	//The curve object has been created
	//Fill it with the variable data specified by iMode

	static int i;
	static double x,y;
	static QString UFOName;
	if(m_pCurPlane)     UFOName=m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName=m_pCurWing->m_WingName;

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
				if(m_pCurPOpp && m_pCurPlane && pWPolar->m_UFOName==m_pCurPlane->m_PlaneName && m_pCurPOpp->m_PlrName ==pWPolar->m_PlrName)
					pCurve->SetSelected(i);
				else if(m_pCurWOpp && m_pCurWing && pWPolar->m_UFOName==m_pCurWing->m_WingName && m_pCurWOpp->m_PlrName ==pWPolar->m_PlrName)
					pCurve->SetSelected(i);
			}
		}
	}
}


void QMiarex::FillWPlrCurve(CCurve *pCurve, CWPolar *pWPolar, int XVar, int YVar)
{
	//The Wpolar curve object has been created
	//Fill it with the variable data specified by XVar and YVar
	static bool bAdd;
	static int i;
	static double x,y;
	static QString UFOName;
	if(m_pCurPlane)     UFOName=m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName=m_pCurWing->m_WingName;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
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

		if(bAdd)
		{
			pCurve->AddPoint(x,y);
			if(m_pCurWOpp && m_bHighlightOpp)
			{
				if(fabs(pWPolar->m_Alpha[i]-m_pCurWOpp->m_Alpha)<0.0001)
				{
					if(m_pCurPOpp && m_pCurPlane
					   && pWPolar->m_UFOName==m_pCurPlane->m_PlaneName
					   && m_pCurPOpp->m_PlrName ==pWPolar->m_PlrName)
						pCurve->SetSelected(i);
					else if(m_pCurWOpp && m_pCurWing
							&& pWPolar->m_UFOName==m_pCurWing->m_WingName
							&& m_pCurWOpp->m_PlrName ==pWPolar->m_PlrName)
						pCurve->SetSelected(i);

				}
			}
		}
	}
}


CBody * QMiarex::GetBody(QString BodyName)
{
	int i;
	CBody* pBody;
	for (i=0; i<m_poaBody->size(); i++)
	{
		pBody = (CBody*)m_poaBody->at(i);
		if (pBody->m_BodyName == BodyName) return pBody;
	}
	return NULL;
}



CPOpp * QMiarex::GetPOpp(double Alpha)
{
	// returns a pointer to the WOpp corresponding to aoa Alpha,
	// and with the name of the current plane and current WPolar

	int i;
	if(!m_pCurPlane || !m_pCurWPolar) return NULL;

	CPOpp* pPOpp;

	for (i=0; i<m_poaPOpp->size(); i++)
	{
		pPOpp = (CPOpp*)m_poaPOpp->at(i);
		if ((pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName) &&	(pPOpp->m_PlrName   == m_pCurWPolar->m_PlrName))
		{
			if(m_pCurWPolar->m_Type==4 && fabs(pPOpp->m_QInf - Alpha)<0.01)	return pPOpp;
			else if(fabs(pPOpp->m_Alpha - Alpha)<0.01)	                    return pPOpp;
		}
	}
	return NULL;
}


QGraph* QMiarex::GetGraph(QPoint &pt)
{
	//returns a pointer to the graph in which the point pt lies
	if(m_iView==WOPPVIEW)
	{
		if(m_iWingView==1)
		{
			if(m_pCurWingGraph->IsInDrawRect(pt)) return m_pCurWingGraph;
			else return NULL;
		}
		else if (m_iWingView==2)
		{
			if(m_WingGraph1.IsInDrawRect(pt)){return &m_WingGraph1;}
			if(m_WingGraph2.IsInDrawRect(pt)){return &m_WingGraph2;}
			return NULL;
		}
		else
		{
			if(m_WingGraph1.IsInDrawRect(pt)){return &m_WingGraph1;}
			else if(m_WingGraph2.IsInDrawRect(pt)){return &m_WingGraph2;}
			else if(m_WingGraph3.IsInDrawRect(pt)){return &m_WingGraph3;}
			else if(m_WingGraph4.IsInDrawRect(pt)){return &m_WingGraph4;}
			else return NULL;
		}
	}
	else if(m_iView==WPOLARVIEW)
	{
		if(m_iWPlrView==1)
		{
//			m_pCurWPlrGraph = m_pCurGraph;
//			return m_pCurGraph;
			return m_pCurWPlrGraph;
		}
		else if (m_iWPlrView==2)
		{
			if(m_WPlrGraph1.IsInDrawRect(pt)){return &m_WPlrGraph1;}
			if(m_WPlrGraph2.IsInDrawRect(pt)){return &m_WPlrGraph2;}
			return NULL;
		}
		else
		{
			if(m_WPlrGraph1.IsInDrawRect(pt)){return &m_WPlrGraph1;}
			if(m_WPlrGraph2.IsInDrawRect(pt)){return &m_WPlrGraph2;}
			if(m_WPlrGraph3.IsInDrawRect(pt)){return &m_WPlrGraph3;}
			if(m_WPlrGraph4.IsInDrawRect(pt)){return &m_WPlrGraph4;}
		}
	}
	else if(m_iView==WSTABVIEW)
	{
		if(m_iStabilityView==0)
		{
			if(m_TimeGraph1.IsInDrawRect(pt))      return &m_TimeGraph1;
			else if(m_TimeGraph2.IsInDrawRect(pt)) return &m_TimeGraph2;
			else if(m_TimeGraph3.IsInDrawRect(pt)) return &m_TimeGraph3;
			else if(m_TimeGraph4.IsInDrawRect(pt)) return &m_TimeGraph4;
			return NULL;
		}
		else if(m_iStabilityView==1)
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
	int i;
	CPlane* pPlane;
	for (i=0; i<m_poaPlane->size(); i++)
	{
		pPlane = (CPlane*)m_poaPlane->at(i);
		if (pPlane->m_PlaneName == PlaneName) return pPlane;
	}
	return NULL;
}




CWing * QMiarex::GetWing(QString WingName)
{
	int i;
	CWing* pWing;
	for (i=0; i<m_poaWing->size(); i++)
	{
		pWing = (CWing*)m_poaWing->at(i);
		if (pWing->m_WingName == WingName) return pWing;
	}
	return NULL;
}


CWOpp* QMiarex::GetWOpp(double Alpha)
{
	// returns a pointer to the WOpp corresponding to aoa Alpha,
	// and with the name of the current wing and current WPolar
	if(!m_pCurWing || !m_pCurWPolar) return NULL;
	int i;
	CWOpp* pWOpp;

	for (i=0; i<m_poaWOpp->size(); i++)
	{
		pWOpp = (CWOpp*)m_poaWOpp->at(i);
		if ((pWOpp->m_WingName == m_pCurWing->m_WingName) &&(pWOpp->m_PlrName == m_pCurWPolar->m_PlrName))
		{
			if(m_pCurWPolar->m_Type==4 && fabs(pWOpp->m_QInf - Alpha)<0.01)  return pWOpp;
			else if(fabs(pWOpp->m_Alpha - Alpha)<0.01)                       return pWOpp;
		}
	}
	return NULL;
}



CWPolar* QMiarex::GetWPolar(QString WPolarName)
{
	//return a pointer to the WPolar with name WPolarName
	//or return NULL if non with taht name for the current UFO
	CWPolar *pWPolar;
	QString UFOName;
	int i;
	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
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
	if(!m_pGLWidget) return;
	static double dx, dy, h2, w2;

	GLWidget *pGLWidget = (GLWidget*)m_pGLWidget;
	h2 = pGLWidget->m_GLViewRect.height() /2.0;
	w2 = pGLWidget->m_GLViewRect.width()  /2.0;

	dx =  real.x + w2;
	dy = -real.y + h2;

	point.setX((int)(dx * m_rCltRect.width()));
	point.setY((int)(dy * m_rCltRect.height()));
}


void QMiarex::GLCallModeLists()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	if(m_bOutline)
	{
		if(m_pCurWing)   glCallList(WINGOUTLINE);
		if(m_pCurWing2)  glCallList(WING2OUTLINE);
		if(m_pCurStab)   glCallList(STABOUTLINE);
		if(m_pCurFin)    glCallList(FINOUTLINE);
		if(m_pCurPlane)  glTranslated((m_pCurPlane)->m_BodyPos.x, 0.0, (m_pCurPlane)->m_BodyPos.z);
		if(m_pCurBody)	 glCallList(BODYGEOM);
		if(m_pCurPlane)  glTranslated(-(m_pCurPlane)->m_BodyPos.x, 0.0, -(m_pCurPlane)->m_BodyPos.z);
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
		if(m_pCurWing)  glCallList(WINGSURFACES);
		if(m_pCurWing2) glCallList(WING2SURFACES);
		if(m_pCurStab)  glCallList(STABSURFACES);
		if(m_pCurFin)   glCallList(FINSURFACES);
		if(m_pCurBody)
		{
			if(m_pCurPlane)	glTranslated((m_pCurPlane)->m_BodyPos.x, 0.0, (m_pCurPlane)->m_BodyPos.z);
			glCallList(BODYSURFACES);
			if(m_pCurPlane)	glTranslated(-(m_pCurPlane)->m_BodyPos.x, 0.0, -(m_pCurPlane)->m_BodyPos.z);
		}
	}
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
}


void QMiarex::GLCallViewLists()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	if (m_pCurWPolar && m_pCurWPolar->m_AnalysisType ==3 && m_pCurWPolar->m_bTiltedGeom && m_bWakePanels)
		glCallList(WINGWAKEPANELS);


	if(m_bMoments && m_pCurWOpp)			glCallList(VLMMOMENTS);

	if (m_pCurWOpp && m_bStream && m_pCurWOpp->m_AnalysisType>=2 && !m_bResetglStream)
		glCallList(VLMSTREAMLINES);//streamlines are not rotated

	if(m_pCurWOpp && m_bSpeeds && m_pCurWOpp->m_AnalysisType>=2 && !m_bResetglFlow)
		glCallList(SURFACESPEEDS);


	if (m_pCurWOpp)                                      glRotated(m_pCurWOpp->m_Alpha, 0.0, 1.0, 0.0);


	if (m_pCurWPolar && m_pCurWPolar->m_AnalysisType ==3 && !m_pCurWPolar->m_bTiltedGeom && m_bWakePanels)
		glCallList(WINGWAKEPANELS);

	if(m_bVLMPanels && m_pCurWing)
	{
		if(!(m_b3DCp&&m_pCurWOpp) && !m_bSurfaces) glCallList(MESHBACK);
		glCallList(MESHPANELS);
	}
	if(m_b3DCp && m_pCurWOpp && m_pCurWOpp->m_AnalysisType>=2)
	{
		glCallList(PANELCP);
	}

	//the lift position has been calculated in absolute coordinates, so we don't rotate it with beta
	if(m_bXCP && m_pCurWOpp)
	{
		if(m_pCurWing) glCallList(VLMWINGLIFT);
		if(m_pCurPOpp)
		{
			if(m_pCurWing2) glCallList(VLMWING2LIFT);
			if(m_pCurStab) 	glCallList(VLMSTABLIFT);
			if(m_pCurFin) 	glCallList(VLMFINLIFT);
		}
		glCallList(LIFTFORCE);
	}

	if (m_pCurWPolar && fabs(m_pCurWPolar->m_Beta)>0.001) glRotated(-m_pCurWPolar->m_Beta, 0.0, 0.0, 1.0);

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	if(m_bOutline)
	{
		if(m_pCurWing)   glCallList(WINGOUTLINE);
		if(m_pCurWing2)  glCallList(WING2OUTLINE);
		if(m_pCurStab)   glCallList(STABOUTLINE);
		if(m_pCurFin)    glCallList(FINOUTLINE);
		if(m_pCurPlane)  glTranslated((m_pCurPlane)->m_BodyPos.x, 0.0, (m_pCurPlane)->m_BodyPos.z);
		if(m_pCurBody)	 glCallList(BODYGEOM);
		if(m_pCurPlane)  glTranslated(-(m_pCurPlane)->m_BodyPos.x, 0.0, -(m_pCurPlane)->m_BodyPos.z);
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
		if(m_pCurWing)  glCallList(WINGSURFACES);
		if(m_pCurWing2) glCallList(WING2SURFACES);
		if(m_pCurStab)  glCallList(STABSURFACES);
		if(m_pCurFin)   glCallList(FINSURFACES);
		if(m_pCurBody)
		{
			if(m_pCurPlane)	glTranslated((m_pCurPlane)->m_BodyPos.x, 0.0, (m_pCurPlane)->m_BodyPos.z);
			glCallList(BODYSURFACES);
			if(m_pCurPlane)	glTranslated(-(m_pCurPlane)->m_BodyPos.x, 0.0, -(m_pCurPlane)->m_BodyPos.z);
		}
	}
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	if(m_bVortices && m_pCurWing)
	{
		glCallList(VLMCTRLPTS);
		glCallList(VLMVORTICES);
	}



	if((m_bICd || m_bVCd) && m_pCurWOpp )
	{
		if(m_pCurWing) glCallList(VLMWINGDRAG);
		if(m_pCurPOpp)
		{
			if(m_pCurWing2) glCallList(VLMWING2DRAG);
			if(m_pCurStab)  glCallList(VLMSTABDRAG);
			if(m_pCurFin)   glCallList(VLMFINDRAG);
		}
	}

	if(m_bDownwash && m_pCurWOpp)
	{
		if(m_pCurWing) glCallList(VLMWINGWASH);
		if(m_pCurPOpp)
		{
			if(m_pCurWing2) glCallList(VLMWING2WASH);
			if(m_pCurStab) 	glCallList(VLMSTABWASH);
			if(m_pCurFin) 	glCallList(VLMFINWASH);
		}
	}

	if(m_bXTop && m_pCurWOpp)
	{
		if(m_pCurWing) glCallList(VLMWINGTOPTRANS);
		if(m_pCurPOpp){
			if(m_pCurWing2) glCallList(VLMWING2TOPTRANS);
			if(m_pCurStab) 	glCallList(VLMSTABTOPTRANS);
			if(m_pCurFin) 	glCallList(VLMFINTOPTRANS);
		}
	}
	if(m_bXBot && m_pCurWOpp)
	{
		if(m_pCurWing) glCallList(VLMWINGBOTTRANS);
		if(m_pCurPOpp)
		{
			if(m_pCurWing2) 	glCallList(VLMWING2BOTTRANS);
			if(m_pCurStab) 		glCallList(VLMSTABBOTTRANS);
			if(m_pCurFin) 		glCallList(VLMFINBOTTRANS);
		}
	}
}



void QMiarex::GLDrawAxes()
{
	MainFrame * pMainFrame =(MainFrame*)m_pMainFrame;
	double l = .8;
//	if(m_pCurWing) l=1.1*m_pCurwing->m_PlanformSpan/2.0;
	GLWidget *pGLWidget = (GLWidget*)m_pGLWidget;
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
	if (!m_pCurWing)
	{
		m_bResetglGeom = true;
		m_bResetglMesh = true;
		m_bResetglOpp  = true;
	}

	MainFrame * pMainFrame =(MainFrame*)m_pMainFrame;

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
		if(m_pCurBody->m_LineType==1)	   m_GL3dBody.GLCreateBody3DFlatPanels(m_pCurBody);
		else if(m_pCurBody->m_LineType==2) m_GL3dBody.GLCreateBody3DSplines(m_pCurBody);

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
		m_ArcBall.SetZoom(0.3,eye,up);

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
		if(m_pCurWing)
		{
			if(glIsList(WINGSURFACES))
			{
				glDeleteLists(WINGSURFACES,2);
				m_GLList-=2;
			}
//			GLCreateGeom(m_pCurWing,WINGSURFACES);
			GLCreateGeom(this, m_pCurWing, WINGSURFACES);
		}

		if(m_pCurWing2)
		{
			if(glIsList(WING2SURFACES))
			{
				glDeleteLists(WING2SURFACES,2);
				m_GLList-=2;
			}
			GLCreateGeom(this, m_pCurWing2,WING2SURFACES);
		}

		if(m_pCurStab)
		{
			if(glIsList(STABSURFACES))
			{
				glDeleteLists(STABSURFACES,2);
				m_GLList-=2;
			}
			GLCreateGeom(this, m_pCurStab,STABSURFACES);
		}

		if(m_pCurFin)
		{
			if(glIsList(FINSURFACES))
			{
				glDeleteLists(FINSURFACES,2);
				m_GLList-=2;
			}
			GLCreateGeom(this, m_pCurFin,FINSURFACES);
		}

		m_bResetglGeom = false;
	}

	if(m_bResetglMesh && m_bVLMPanels && m_iView==W3DVIEW)
	{
		if(glIsList(MESHPANELS))
		{
			glDeleteLists(MESHPANELS,2);
			m_GLList-=2;
		}
		GLCreateMesh(this, m_Node, m_Panel);
		if(glIsList(VLMCTRLPTS))
		{
			glDeleteLists(VLMCTRLPTS,1);
			m_GLList-=1;
		}
		GLCreateCtrlPts(this, m_Panel);
		if(glIsList(VLMVORTICES))
		{
			glDeleteLists(VLMVORTICES,1);
			m_GLList-=1;
		}
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
		if (m_pCurWPolar && m_pCurWPolar->m_AnalysisType==3)
		{
//			GLCreateWakePanels(WINGWAKEPANELS);
		}
		m_bResetglWake = false;
	}

	if((m_bResetglLift || m_bResetglOpp) && m_iView==W3DVIEW)
	{

		if(glIsList(VLMWINGLIFT))
		{
			glDeleteLists(VLMWINGLIFT,3);
			m_GLList -=3;
		}
		if(glIsList(VLMWING2LIFT))
		{
			glDeleteLists(VLMWING2LIFT,1);
			m_GLList -=1;
		}
		if(glIsList(VLMSTABLIFT))
		{
			glDeleteLists(VLMWING2LIFT,1);
			m_GLList -=1;
		}
		if(glIsList(VLMSTABLIFT))
		{
			glDeleteLists(VLMSTABLIFT,1);
			m_GLList -=1;
		}
		if(glIsList(VLMFINLIFT))
		{
			glDeleteLists(VLMFINLIFT,1);
			m_GLList -=1;
		}
		if(glIsList(VLMWINGTOPTRANS))
		{
			glDeleteLists(VLMWINGTOPTRANS,2);
			m_GLList-=2;
		}
		if(glIsList(VLMWING2TOPTRANS))
		{
			glDeleteLists(VLMWING2TOPTRANS,2);
			m_GLList-=2;
		}
		if(glIsList(VLMSTABTOPTRANS))
		{
			glDeleteLists(VLMSTABTOPTRANS,2);
			m_GLList-=2;
		}
		if(glIsList(VLMFINTOPTRANS))
		{
			glDeleteLists(VLMFINTOPTRANS,2);
			m_GLList-=2;
		}
				if ((m_pCurWing && m_pCurWOpp) || (m_pCurPOpp &&m_pCurWOpp) )
		{
			GLCreateLiftStrip(this, m_pCurWing, m_pCurWPolar, m_pCurWOpp, VLMWINGLIFT);
			GLCreateTrans(this, m_pCurWing, m_pCurWOpp, VLMWINGTOPTRANS);
			int surf = (m_pCurWing)->m_NSurfaces;
			if(m_pCurPOpp)
			{
				if(m_pCurWing2)
				{
					GLCreateLiftStrip(this, m_pCurWing2, m_pCurWPolar, & (m_pCurPOpp)->m_Wing2WOpp, VLMWING2LIFT);
					GLCreateTrans(this, m_pCurWing2, &(m_pCurPOpp)->m_Wing2WOpp, VLMWING2TOPTRANS);
					surf += (m_pCurWing2)->m_NSurfaces;
				}
				if(m_pCurStab)
				{
					GLCreateLiftStrip(this, m_pCurStab, m_pCurWPolar, &(m_pCurPOpp)->m_StabWOpp, VLMSTABLIFT);
					GLCreateTrans(this, m_pCurStab, &(m_pCurPOpp)->m_StabWOpp, VLMSTABTOPTRANS);
					surf += (m_pCurStab)->m_NSurfaces;
				}
				if(m_pCurFin)
				{
					GLCreateLiftStrip(this, m_pCurFin, m_pCurWPolar, &(m_pCurPOpp)->m_FinWOpp, VLMFINLIFT);
					GLCreateTrans(this, m_pCurFin, &(m_pCurPOpp)->m_FinWOpp, VLMFINTOPTRANS);
				}
			}
			GLCreateLiftForce(this, m_pCurWPolar, m_pCurWOpp);
			GLCreateMoments(this, m_pCurWing, m_pCurWPolar, m_pCurWOpp);
		}
		m_bResetglLift = false;
	}

	if((m_bResetglDrag || m_bResetglOpp) && m_iView==W3DVIEW)
	{
		if(glIsList(VLMWINGDRAG))
		{
			glDeleteLists(VLMWINGDRAG,1);
			m_GLList -=1;
		}
		if(glIsList(VLMWING2DRAG))
		{
			glDeleteLists(VLMWING2DRAG,1);
			m_GLList -=1;
		}
		if(glIsList(VLMSTABDRAG))
		{
			glDeleteLists(VLMSTABDRAG,1);
			m_GLList -=1;
		}
		if(glIsList(VLMFINDRAG))
		{
			glDeleteLists(VLMFINDRAG,1);
			m_GLList -=1;
		}

				if ((m_pCurWing && m_pCurWOpp) || (m_pCurPOpp && m_pCurWOpp))
		{
			GLCreateDrag(this, m_pCurWing, m_pCurWPolar, m_pCurWOpp, VLMWINGDRAG);
			int surf = (m_pCurWing)->m_NSurfaces;
			if(m_pCurPOpp)
			{
				if(m_pCurWing2)
				{
					GLCreateDrag(this, m_pCurWing2, m_pCurWPolar, &(m_pCurPOpp)->m_Wing2WOpp, VLMWING2DRAG);
					surf += (m_pCurWing2)->m_NSurfaces;
				}
				if(m_pCurStab)
				{
					GLCreateDrag(this, m_pCurStab, m_pCurWPolar, &(m_pCurPOpp)->m_StabWOpp, VLMSTABDRAG);
					surf += (m_pCurStab)->m_NSurfaces;
				}
				if(m_pCurFin)
				{
					GLCreateDrag(this, m_pCurFin, m_pCurWPolar, &(m_pCurPOpp)->m_FinWOpp, VLMFINDRAG);
				}
			}
		}
		m_bResetglDrag = false;
	}

	if((m_bResetglDownwash || m_bResetglOpp) && m_iView==W3DVIEW)
	{
		if(glIsList(VLMWINGWASH))
		{
			glDeleteLists(VLMWINGWASH,1);
			m_GLList--;
		}
		if(glIsList(VLMWING2WASH))
		{
			glDeleteLists(VLMWING2WASH,1);
			m_GLList--;
		}
		if(glIsList(VLMSTABWASH))
		{
			glDeleteLists(VLMSTABWASH,1);
			m_GLList--;
		}
		if(glIsList(VLMFINWASH))
		{
			glDeleteLists(VLMFINWASH,1);
			m_GLList--;
		}
		if (m_pCurWing && m_pCurWOpp)
		{
			GLCreateDownwash(this, m_pCurWing, m_pCurWOpp, VLMWINGWASH);
			int surf = m_pCurWing->m_NSurfaces;
			if(m_pCurPOpp)
			{
				if(m_pCurWing2)
				{
					GLCreateDownwash(this, m_pCurWing2, &(m_pCurPOpp->m_Wing2WOpp), VLMWING2WASH);
					surf += m_pCurWing2->m_NSurfaces;
				}
				if(m_pCurStab)
				{
					GLCreateDownwash(this, m_pCurStab, &(m_pCurPOpp->m_StabWOpp), VLMSTABWASH);
					surf += m_pCurStab->m_NSurfaces;
				}
				if(m_pCurFin) GLCreateDownwash(this, m_pCurFin, &(m_pCurPOpp->m_FinWOpp), VLMFINWASH);
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
				m_GLList--;
			}

			GLCreateCp(this, m_Node, m_Panel, m_pCurWOpp, m_pCurPOpp);
		}

		m_bResetglOpp = false;
	}

	if((m_bResetglLegend || m_bResetglOpp || m_bResetglGeom) && m_iView==W3DVIEW)
	{
		if(glIsList(WINGLEGEND))
		{
			glDeleteLists(WINGLEGEND,1);
			m_GLList -= 1;
		}
		GLCreateWingLegend(this, m_pCurWing, m_pCurPlane, m_pCurWPolar);

		if(m_pCurWOpp)
		{
			glDeleteLists(WOPPLEGEND,3);
			m_GLList -= 3;
			GLCreateWOppLegend(this, m_pCurWing, m_pCurWOpp);
			GLCreateCpLegend(this);
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
			if(m_pCurWing && m_pCurWOpp && m_pCurWOpp->m_AnalysisType>=2)
			{
				if(glIsList(VLMSTREAMLINES))
				{
					glDeleteLists(VLMSTREAMLINES,1);
					m_GLList -=1;
				}
				CWing *Wing[4];
				Wing[0] = m_pCurWing;
				Wing[1] = m_pCurWing2;
				Wing[2] = m_pCurStab;
				Wing[3] = m_pCurFin;
				//back-up the current geometry, before it is tilted for streamlines calculation
				memcpy(m_MemPanel, m_Panel, m_MatSize * sizeof(CPanel));
				memcpy(m_MemNode,  m_Node,  m_nNodes * sizeof(CVector));
				memcpy(m_RefWakePanel, m_WakePanel, m_WakeSize * sizeof(CPanel));
				memcpy(m_RefWakeNode,  m_WakeNode,  m_nWakeNodes * sizeof(CVector));
				GLCreateStreamLines(this, Wing, m_Panel, m_Node, m_pCurWPolar, m_pCurWOpp);
				//restore the initial geometry
				memcpy(m_Panel, m_MemPanel, m_MatSize * sizeof(CPanel));
				memcpy(m_Node,  m_MemNode,  m_nNodes  * sizeof(CVector));
				memcpy(m_WakePanel, m_RefWakePanel, m_WakeSize   * sizeof(CPanel));
				memcpy(m_WakeNode,  m_RefWakeNode,  m_nWakeNodes * sizeof(CVector));				
				m_bResetglStream = false;
			}
		}
	}

	if((m_bResetglFlow || m_bResetglOpp) && m_iView==W3DVIEW)
	{
		if(m_bSpeeds)
		{
			if(m_pCurWing && m_pCurWOpp && m_pCurWOpp->m_AnalysisType>=2)
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


void QMiarex::GLDrawFoils()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	GLWidget *pGLWidget = (GLWidget*)m_pGLWidget;
	int j;
	CFoil *pFoil;
	CWing *pWing[4];
	pWing[0] = m_pCurWing;
	pWing[1] = m_pCurWing2;
	pWing[2] = m_pCurStab;
	pWing[3] = m_pCurFin;


	glColor3d(pMainFrame->m_TextColor.redF(), pMainFrame->m_TextColor.greenF(), pMainFrame->m_TextColor.blueF());

	for(int i=0; i<4; i++)
	{
		if(pWing[i])
		{
			for(j=0; j<pWing[i]->m_NSurfaces; j++)
			{
				pFoil = pWing[i]->m_Surface[j].m_pFoilA;

				if(pFoil)
				{
					pGLWidget->renderText(pWing[i]->m_Surface[j].m_TA.x, pWing[i]->m_Surface[j].m_TA.y, pWing[i]->m_Surface[j].m_TA.z,
											pFoil->m_FoilName, pMainFrame->m_TextFont);
				}
			}
			j = pWing[i]->m_NSurfaces-1;
			pFoil = pWing[i]->m_Surface[j].m_pFoilB;
			if(pFoil)
			{
				pGLWidget->renderText(pWing[i]->m_Surface[j].m_TB.x, pWing[i]->m_Surface[j].m_TB.y, pWing[i]->m_Surface[j].m_TB.z,
										pFoil->m_FoilName, pMainFrame->m_TextFont);
			}
		}
	}
}


void QMiarex::GLInverseMatrix()
{
	//Step 1. Transpose the 3x3 rotation portion of the 4x4 matrix to get the inverse rotation
	int i,j;

	for(i=0 ; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			MatOut[j][i] = MatIn[i][j];
		}
	}
}


void QMiarex::GLRenderMode()
{
	MainFrame * pMainFrame = (MainFrame*)m_pMainFrame;
	GLWidget *pGLWidget = (GLWidget*)m_pGLWidget;
	QString strong = QString("Time =%1s").arg(m_ModeTime,6,'f',3);

	GLCreateModeLegend(this, m_pCurWing, m_pCurWOpp, m_pCurPOpp);

	glDisable(GL_CLIP_PLANE1);

	// Clear the viewport
	glFlush();
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	{
		GLSetupLight();
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		glCallList(MODELEGEND);

//		pGLWidget->renderText(m_rCltRect.left(), m_rCltRect.top(), strong, pMainFrame->m_TextFont);
		pGLWidget->renderText(15, 15, strong, pMainFrame->m_TextFont);

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

		glTranslated(m_ModeState[0], m_ModeState[1], m_ModeState[2]);
		glRotated(m_ModeState[3]*180.0/PI, 1.0, 0.0 ,0.0);
		glRotated(m_ModeState[4]*180.0/PI, 0.0, 1.0 ,0.0);
		glRotated(m_ModeState[5]*180.0/PI, 0.0, 0.0 ,1.0);
		if (m_pCurWPolar && fabs(m_pCurWPolar->m_Beta)>0.001) glRotated(-m_pCurWPolar->m_Beta, 0.0, 0.0, 1.0);

		GLCallModeLists();
	}
	glPopMatrix();
}


void QMiarex::GLRenderView()
{
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
		if (m_b3DCp && m_pCurWOpp && m_pCurWOpp->m_AnalysisType>=2 )
		{
			glCallList(WOPPCPLEGENDTXT);
			glCallList(WOPPCPLEGENDCLR);
		}

		if(m_pCurWing)			glCallList(WINGLEGEND);
		if(m_pCurWOpp)			glCallList(WOPPLEGEND);


		GLSetupLight();
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
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

		GLCallViewLists();
		if(m_bFoilNames) GLDrawFoils();
	}
	glPopMatrix();
	glDisable(GL_CLIP_PLANE1);
}


void QMiarex::GLRenderSphere(QColor cr, double radius, int NumLongitudes, int NumLatitudes)
{
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
	if(!m_pCurWing) return false;
	int j,k,l;
	QProgressDialog dlg(this);
	dlg.setLabelText(tr("Creating Elements... please Wait"));
	dlg.setMinimum(0);
	dlg.setMaximum(100);
	dlg.setWindowModality(Qt::WindowModal);
	dlg.setValue(0);

	int p, pp;
	int Nel;
	m_MatSize = 0;

	for (j=0; j<m_NSurfaces; j++)
	{
		m_MatSize += m_pSurface[j]->m_NXPanels * m_pSurface[j]->m_NYPanels ;
	}

	if(m_pCurWPolar && m_pCurWPolar->m_AnalysisType==3 && !m_pCurWPolar->m_bThinSurfaces)
	{
		m_MatSize *= 2;
		for (j=0; j<m_NSurfaces; j++)
		{
			if(m_pSurface[j]->m_bIsTipLeft || m_pSurface[j]->m_bIsTipRight)
				m_MatSize += m_pSurface[j]->m_NXPanels;//tip patches
		}
	}

	bool bBodyEl = false;
	if(m_pCurBody)
	{
		m_MatSize += 2 * m_pCurBody->m_nxPanels * m_pCurBody->m_nhPanels;

		//create the body elements only if there is a body, and the analysis is not of the VLM Type
		if(!m_pCurWPolar)                         bBodyEl = true;//no risk...
		else if(m_pCurWPolar->m_AnalysisType==3)  bBodyEl = true;
		else                                      bBodyEl = false;
	}

	if(m_MatSize>VLMMATSIZE)
	{
		QString strong;
		strong = QString(tr("The total number of panels is %1\n The Max Number is %2\nA reduction of the number of panels is required"))
						  .arg(m_MatSize).arg(VLMMATSIZE);
		QMessageBox::warning(this,tr("Warning"),strong);
		m_MatSize = 0;
		m_nNodes  = 0;
		return false;
	}

	m_MatSize     = 0;
	m_nNodes      = 0;
	m_NWakeColumn = 0;
	m_nWakeNodes  = 0;
	m_WakeSize    = 0;

	memset(m_Panel, 0, sizeof(m_Panel));
	memset(m_Node,  0, sizeof(m_Node));

	p = 0;
	CPanel *ptr = m_Panel;

	dlg.setValue(5);
	for (j=0; j<m_NSurfaces; j++)
	{
		m_pSurface[j]->ResetFlap();

		Nel = CreateElements(*(m_pSurface+j));
		//reference in pPanel array the left side panels for all surfaces
		if((*(m_pSurface+j))->m_bIsLeftSurf)
		{
			for(pp=0; pp<m_pSurface[j]->m_NElements; pp++)
			{
				m_pPanel[p] = ptr + pp;
				p++;
			}
		}
		ptr += Nel;
		dlg.setValue(5+j*90/m_NSurfaces);
	}
	dlg.setValue(95);

	if(bBodyEl)
	{
		Nel = CreateBodyElements();
		for(pp=0; pp<m_pCurBody->m_NElements/2; pp++)
		{
			m_pPanel[p] = ptr + pp;
			p++;
		}
	}

	ptr = m_Panel;

	//initialize the MatSize and the pointer to the first panel for each wing or body
	if(m_pCurWing)
	{
		m_pCurWing->m_MatSize = 0;
		for(j=0; j<m_pCurWing->m_NSurfaces;j++) m_pCurWing->m_MatSize += m_pCurWing->m_Surface[j].m_NElements;
		m_pCurWing->m_pPanel = ptr ;
		ptr += m_pCurWing->m_MatSize;
	}

	if(m_pCurWing2)
	{
		m_pCurWing2->m_MatSize = 0;
		for(j=0; j<m_pCurWing2->m_NSurfaces;j++) m_pCurWing2->m_MatSize += m_pCurWing2->m_Surface[j].m_NElements;
		m_pCurWing2->m_pPanel = ptr ;
		ptr += m_pCurWing2->m_MatSize;
	}

	if(m_pCurStab)
	{
		m_pCurStab->m_MatSize = 0;
		for(j=0; j<m_pCurStab->m_NSurfaces;j++)  m_pCurStab->m_MatSize  += m_pCurStab->m_Surface[j].m_NElements;
		m_pCurStab->m_pPanel = ptr ;
		ptr += m_pCurStab->m_MatSize;
	}

	if(m_pCurFin)
	{
		m_pCurFin->m_MatSize = 0;
		for(j=0; j<m_pCurFin->m_NSurfaces;j++)   m_pCurFin->m_MatSize   += m_pCurFin->m_Surface[j].m_NElements;
		m_pCurFin->m_pPanel = ptr ;
		ptr += m_pCurFin->m_MatSize;
	}

	if(bBodyEl)
	{
		m_pCurBody->m_pPanel = ptr;
		ptr +=  2 * m_pCurBody->m_nxPanels * m_pCurBody->m_nhPanels;
	}

	// at this point, ptr points to the last+1 panel of the full array

	// we reference the right side panels in reverse order, from right to left, for symmetry purposes

	int coef=1;
	if(m_pCurWPolar && m_pCurWPolar->m_AnalysisType==3 &&!m_pCurWPolar->m_bThinSurfaces) coef = 2;

	if(bBodyEl)
	{
		ptr = m_pCurBody->m_pPanel + m_pCurBody->m_nxPanels*m_pCurBody->m_nhPanels;
		for (k = m_pCurBody->m_nxPanels-1; k>=0; k--)
		{
			// the body's rigth side panels have been generated in reverse order... so just copy
			for (l=0; l<m_pCurBody->m_nhPanels; l++)
			{
				m_pPanel[p] = ptr+l;
				p++;
			}
			ptr += m_pCurBody->m_nhPanels;
		}
	}

	if(m_pCurFin)
	{
		ptr = m_pCurFin->m_pPanel;
		for (j=0; j<m_pCurFin->m_NSurfaces; j++)
		{
			if(m_pCurFin->m_Surface[j].m_bIsLeftSurf)  ptr += m_pCurFin->m_Surface[j].m_NElements;
			else
			{
				if(m_pCurWPolar && m_pCurWPolar->m_AnalysisType==3 && m_pCurFin->m_Surface[j].m_bIsTipLeft)
				{
					// process left tip patch
					for (l=m_pCurFin->m_Surface[j].m_NXPanels-1;l>=0; l--)
					{
						m_pPanel[p] = ptr+l;
						p++;
					}
					ptr += m_pCurFin->m_Surface[j].m_NXPanels;
				}
				for (k=0; k<m_pCurFin->m_Surface[j].m_NYPanels; k++)
				{
					// process the surface's "horizontal" panels
					for (l=m_pCurFin->m_Surface[j].m_NXPanels*coef-1; l>=0; l--)
					{
						m_pPanel[p] = ptr+l;
						p++;
					}
					ptr += m_pCurFin->m_Surface[j].m_NXPanels * coef;
				}

				if (m_pCurWPolar && m_pCurWPolar->m_AnalysisType==3
					&& m_pCurFin->m_Surface[j].m_bIsTipRight && !m_pCurWPolar->m_bThinSurfaces)
				{
					// process right tip patch
					for (l=m_pCurFin->m_Surface[j].m_NXPanels-1;l>=0; l--)
					{
						m_pPanel[p] = ptr+l;
						p++;
					}
					ptr += m_pCurFin->m_Surface[j].m_NXPanels;
				}
			}
		}
	}

	if(m_pCurStab)
	{
		ptr = m_pCurStab->m_pPanel;
		for (j=0; j<m_pCurStab->m_NSurfaces; j++)
		{
			if(m_pCurStab->m_Surface[j].m_bIsLeftSurf)  ptr += m_pCurStab->m_Surface[j].m_NElements;
			else
			{
				for (k=0; k<m_pCurStab->m_Surface[j].m_NYPanels; k++)
				{
					// process the surface's "horizontal" panels
					for (l=m_pCurStab->m_Surface[j].m_NXPanels*coef-1; l>=0; l--)
					{
						m_pPanel[p] = ptr+l;
						p++;
					}
					ptr += m_pCurStab->m_Surface[j].m_NXPanels * coef;
				}

				if (m_pCurWPolar && m_pCurWPolar->m_AnalysisType==3
					&& m_pCurStab->m_Surface[j].m_bIsTipRight && !m_pCurWPolar->m_bThinSurfaces)
				{
					// process right tip patch
					for (l=m_pCurStab->m_Surface[j].m_NXPanels-1;l>=0; l--)
					{
						m_pPanel[p] = ptr+l;
						p++;
					}
					ptr += m_pCurStab->m_Surface[j].m_NXPanels;
				}
			}
		}
	}

	if(m_pCurWing2)
	{
		ptr = m_pCurWing2->m_pPanel;
		for (j=0; j<m_pCurWing2->m_NSurfaces; j++)
		{
			if(m_pCurWing2->m_Surface[j].m_bIsLeftSurf)  ptr += m_pCurWing2->m_Surface[j].m_NElements;
			else
			{
				for (k=0; k<m_pCurWing2->m_Surface[j].m_NYPanels; k++)
				{
					// process the surface's "horizontal" panels
					for (l=m_pCurWing2->m_Surface[j].m_NXPanels*coef-1; l>=0; l--)
					{
						m_pPanel[p] = ptr+l;
						p++;
					}
					ptr += m_pCurWing2->m_Surface[j].m_NXPanels * coef;
				}

				if (m_pCurWPolar && m_pCurWPolar->m_AnalysisType==3
					&& m_pCurWing2->m_Surface[j].m_bIsTipRight&& !m_pCurWPolar->m_bThinSurfaces)
				{
					// process right tip patch
					for (l=m_pCurWing2->m_Surface[j].m_NXPanels-1;l>=0; l--)
					{
						m_pPanel[p] = ptr+l;
						p++;
					}
					ptr += m_pCurWing2->m_Surface[j].m_NXPanels;
				}
			}
		}
	}


	if(m_pCurWing)
	{
		ptr = m_pCurWing->m_pPanel;
		for (j=0; j<m_pCurWing->m_NSurfaces; j++)
		{
			if(m_pCurWing->m_Surface[j].m_bIsLeftSurf)  ptr += m_pCurWing->m_Surface[j].m_NElements;
			else
			{
				for (k=0; k<m_pCurWing->m_Surface[j].m_NYPanels; k++)
				{
					// process the surface's "horizontal" panels
					for (l=m_pCurWing->m_Surface[j].m_NXPanels*coef-1; l>=0; l--)
					{
						m_pPanel[p] = ptr+l;
						p++;
					}
					ptr += m_pCurWing->m_Surface[j].m_NXPanels * coef;
				}

				if (m_pCurWPolar && m_pCurWPolar->m_AnalysisType==3
					&& m_pCurWing->m_Surface[j].m_bIsTipRight && !m_pCurWPolar->m_bThinSurfaces)
				{
					// process right tip patch
					for (l=m_pCurWing->m_Surface[j].m_NXPanels-1;l>=0; l--)
					{
						m_pPanel[p] = ptr+l;
						p++;
					}
					ptr += m_pCurWing->m_Surface[j].m_NXPanels;
				}
			}
		}
	}
	dlg.setValue(100);
	return true;
}


CWOpp* QMiarex::InsertWOpp(CWOpp *pNewPoint)
{
	// loads a WOpPoint from a file
	// adds it to the WOpp array

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
						i = (int)m_poaWOpp->size();// to break
					}
					else if (pNewPoint->m_Alpha > pWOpp->m_Alpha)
					{
						//insert point
						m_poaWOpp->insert(i, pNewPoint);
						bIsInserted = true;
						i = (int)m_poaWOpp->size();// to break
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
						i = (int)m_poaWOpp->size();// to break
					}
					else if (pNewPoint->m_QInf > pWOpp->m_QInf)
					{
						//insert point
						m_poaWOpp->insert(i, pNewPoint);
						bIsInserted = true;
						i = (int)m_poaWOpp->size();// to break
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
//	A is the ray's origin,
//	U is the ray's direction
//	LA, LB, TA, TB define a quadrangle in 3D space.
//	N is the normal to the quadrangle
//	I is the resulting intersection point of the ray and the quadrangle, if inside the quadrangle
//	dist = |AI|
//	The return value is true if intersection inside the quadrangle, false otherwise

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
	int in;
	for (in=0; in<m_nWakeNodes; in++)
	{
		if(Pt.IsSame(m_WakeNode[in])) return in;
	}
	return -1;
}


int QMiarex::IsNode(CVector &Pt)
{
	int in;
	for (in=0; in<m_nNodes; in++)
	{
		if(Pt.IsSame(m_Node[in])) return in;
	}
	return -1;
}



void QMiarex::JoinSurfaces(CSurface *pLeftSurf, CSurface *pRightSurf, int pl, int pr)
{
	if(!m_pCurWPolar || m_pCurWPolar->m_AnalysisType!=3) return;//panel analysis only

	// In the case where the number of chordwise panels is different between two adjacent surfaces,
	// We need to correct the ideal connection that was set in the CreateElements() method.
	// This is the case for instance for a flap.
	// The algorith below is not robust... ideally the connections should be set manually
	//
	// Use VSAERO method

	//pl and pr are respectively the left surface's and the right surface's first panel index
	int ls, lr, lclose, ppl, ppr;
	double dist, x,y,z, mindist;
	lclose=0;
	CVector MidNormal = pLeftSurf->Normal + pRightSurf->Normal;
	MidNormal.Normalize();

	int coef = 1;
	if(m_pCurWPolar && m_pCurWPolar->m_AnalysisType==3 && !m_pCurWPolar->m_bThinSurfaces) coef = 2;

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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	switch (event->key())
	{
		case Qt::Key_Return:
		{
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
			if(m_iView!=WSTABVIEW) StopAnimate();
			if(m_pCurGraph) m_pCurGraph->DeselectPoint();
			
			if(pMainFrame->m_pctrl3DScalesWidget->isVisible()) pMainFrame->m_pctrl3DScalesWidget->hide();
			UpdateView();
			break;
		}
		case Qt::Key_0:
		{
			if(m_iView==WSTABVIEW)
			{
				m_iStabilityView  = 0;
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
				m_pCurGraph = &m_WPlrGraph1;
				m_pCurWPlrGraph = m_pCurGraph;
				SetWPlrLegendPos();
			}
			else if(m_iView==WOPPVIEW)
			{
				m_iWingView =1;
				m_pCurGraph = &m_WingGraph1;
				m_pCurWingGraph = m_pCurGraph;
				SetWingLegendPos();
			}
			else if(m_iView==WSTABVIEW)
			{
				if(m_iStabilityView==0)
				{
					m_iStabTimeView =1;
					m_pCurGraph = &m_TimeGraph1;
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
				m_pCurGraph = &m_WPlrGraph2;
				m_pCurWPlrGraph = m_pCurGraph;
				SetWPlrLegendPos();
			}
			else if(m_iView==WOPPVIEW)
			{
				m_iWingView =1;
				m_pCurGraph = &m_WingGraph2;
				m_pCurWingGraph = m_pCurGraph;
				SetWingLegendPos();
			}
			else if(m_iView==WSTABVIEW)
			{
				if(m_iStabilityView==0)
				{
					m_iStabTimeView =1;
					m_pCurGraph = &m_TimeGraph2;
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
				m_pCurGraph = &m_WPlrGraph3;
				m_pCurWPlrGraph = m_pCurGraph;
				SetWPlrLegendPos();
			}
			else if(m_iView==WOPPVIEW)
			{
				m_iWingView =1;
				m_pCurGraph = &m_WingGraph3;
				m_pCurWingGraph = m_pCurGraph;
				SetWingLegendPos();
			}
			else if(m_iView==WSTABVIEW)
			{
				if(m_iStabilityView==0)
				{
					m_iStabTimeView =1;
					m_pCurGraph = &m_TimeGraph3;
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
				m_pCurGraph = &m_WPlrGraph4;
				m_pCurWPlrGraph = m_pCurGraph;
				SetWPlrLegendPos();
			}
			else if(m_iView==WOPPVIEW)
			{
				m_iWingView =1;
				m_pCurGraph = &m_WingGraph4;
				m_pCurWingGraph = m_pCurGraph;
				SetWingLegendPos();
			}
			else if(m_iView==WSTABVIEW)
			{
				if(m_iStabilityView==0)
				{
					m_iStabTimeView =1;
					m_pCurGraph = &m_TimeGraph4;
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
			else
			{
				m_iWingView =2;
				SetWingLegendPos();
			}
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
				if(m_iStabilityView==0)
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
			if(m_pCurGraph)
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
		case Qt::Key_R:
		{
			if(m_pCurGraph)
			{
				m_pCurGraph->SetAuto(true);
				UpdateView();
			}
			else if(m_iView== WOPPVIEW)
			{
				OnResetWingScale();
			}
			else if(m_iView==W3DVIEW) On3DReset();

			break;
		}
		case Qt::Key_F2:
		{
			OnRenameCurUFO();
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
//			else if (event->modifiers().testFlag(Qt::ControlModifier)) OnDefineCtrlPolar();
			else                                                       OnDefineWPolar();
			break;
		}
		case Qt::Key_F8:
		{
			if (event->modifiers().testFlag(Qt::ShiftModifier)) OnStabilityView();
			else                                                OnWPolars();
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
	if(!m_pCurWing || !m_pCurWPolar) return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pCurWing->m_bInitCalc = bInitCalc;

	m_pLLTDlg->m_pWing      = m_pCurWing;
	m_pLLTDlg->m_pWPolar    = m_pCurWPolar;
	m_pLLTDlg->m_bSequence  = bSequence;

	m_pLLTDlg->m_AlphaMin      = V0;
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
	int k;
	int r,g,b;

	pSettings->beginGroup("Miarex");
	{
		m_bXTop         = pSettings->value("bXTop", false).toBool();
		m_bXBot         = pSettings->value("bXBot", false).toBool();
		m_bXCP          = pSettings->value("bXCP", true).toBool();
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
		m_bLogFile      = pSettings->value("LogFile").toBool();
		m_bDirichlet    = pSettings->value("Dirichlet").toBool();
		m_bTrefftz      = pSettings->value("Trefftz").toBool();
		m_bKeepOutOpps  = pSettings->value("KeepOutOpps").toBool();
		m_bResetWake    = pSettings->value("ResetWake").toBool();
		m_bShowWing2    = pSettings->value("ShowWing2").toBool();
		m_bShowStab     = pSettings->value("ShowStab").toBool();
		m_bShowFin      = pSettings->value("ShowFin").toBool();
		m_bStoreWOpp    = pSettings->value("StoreWOpp").toBool();
		m_bSequence     = pSettings->value("Sequence").toBool();
		m_bHighlightOpp = pSettings->value("HighlightOpp").toBool();
		m_bHighlightOpp = false;

		m_AlphaMin      = pSettings->value("AlphaMin").toDouble();
		m_AlphaMax      = pSettings->value("AlphaMax").toDouble();
		m_AlphaDelta    = pSettings->value("AlphaDelta").toDouble();
		m_QInfMin       = pSettings->value("QInfMin").toDouble();
		m_QInfMax       = pSettings->value("QInfMax").toDouble();
		m_QInfDelta     = pSettings->value("QInfDelta").toDouble();
		m_ControlMin    = pSettings->value("ControlMin").toDouble();
		m_ControlMax    = pSettings->value("ControlMax").toDouble();
		m_ControlDelta  = pSettings->value("ControlDelta").toDouble();
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

		CWing::s_CvPrec       = pSettings->value("CvPrec").toDouble();
		CWing::s_RelaxMax     = pSettings->value("RelaxMax").toDouble();
		CWing::s_NLLTStations = pSettings->value("NLLTStations").toInt();
		m_iView     = pSettings->value("iView").toInt();
		m_iWingView = pSettings->value("iWingView").toInt();
		m_iWPlrView = pSettings->value("iWPlrView").toInt();
		m_iStabilityView = pSettings->value("iWStabView").toInt();

		k = pSettings->value("CurrentWingGraph").toInt();
		if     (k==1) m_pCurWingGraph = &m_WingGraph1;
		else if(k==2) m_pCurWingGraph = &m_WingGraph2;
		else if(k==3) m_pCurWingGraph = &m_WingGraph3;
		else if(k==4) m_pCurWingGraph = &m_WingGraph4;

		k = pSettings->value("CurWPlrGraph").toInt();
		if     (k==1) m_pCurWPlrGraph = &m_WPlrGraph1;
		else if(k==2) m_pCurWPlrGraph = &m_WPlrGraph2;
		else if(k==3) m_pCurWPlrGraph = &m_WPlrGraph3;
		else if(k==4) m_pCurWPlrGraph = &m_WPlrGraph4;

		k = pSettings->value("CurRLStabGraph").toInt();
		if     (k==1) m_pCurRLStabGraph = &m_LongRLGraph;
		else if(k==2) m_pCurRLStabGraph = &m_LatRLGraph;

		if(m_iView==WOPPVIEW)        m_pCurGraph=m_pCurWingGraph;
		else if(m_iView==WPOLARVIEW) m_pCurGraph=m_pCurWPlrGraph;
		else if(m_iView==WSTABVIEW)  m_pCurGraph=m_pCurRLStabGraph;

		m_Iter         = pSettings->value("Iter").toInt();
		m_NStation     = pSettings->value("NStation").toInt();
		m_NHoopPoints  = pSettings->value("NHoopPoints").toInt();
		m_NXPoints     = pSettings->value("NXPoints").toInt();
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

		m_TotalTime         = pSettings->value("TotalTime",10.0).toDouble();
		m_Deltat            = pSettings->value("Delta_t",0.01).toDouble();
		m_TimeInput[0]      = pSettings->value("TimeIn0",0.0).toDouble();
		m_TimeInput[1]      = pSettings->value("TimeIn1",0.0).toDouble();
		m_TimeInput[2]      = pSettings->value("TimeIn2",0.0).toDouble();
		m_TimeInput[3]      = pSettings->value("TimeIn3",0.0).toDouble();
		m_bForcedResponse   = pSettings->value("ForcedResponse").toBool();
		m_bLongitudinal     = pSettings->value("DynamicsMode").toBool();
		m_iStabTimeView     = pSettings->value("StabTimeView",4).toInt();
		k = pSettings->value("TimeGraph").toInt();
		if(k==1)m_pCurTimeGraph = &m_TimeGraph1;
		else if(k==2)m_pCurTimeGraph = &m_TimeGraph2;
		else if(k==3)m_pCurTimeGraph = &m_TimeGraph3;
		else if(k==4)m_pCurTimeGraph = &m_TimeGraph4;
		
		if(m_bLongitudinal)
		{
			m_TimeGraph1.SetYTitle("u");
			m_TimeGraph2.SetYTitle("w");
			m_TimeGraph3.SetYTitle("q");
			m_TimeGraph4.SetYTitle("theta");
		}
		else
		{
			m_TimeGraph1.SetYTitle("v");
			m_TimeGraph2.SetYTitle("p");
			m_TimeGraph3.SetYTitle("r");
			m_TimeGraph4.SetYTitle("phi");
		}
 	}
	pSettings->endGroup();

	m_GL3dBody.LoadSettings(pSettings);
	m_GLLightDlg.LoadSettings(pSettings);

	m_CpGraph.LoadSettings(pSettings);
	m_TimeGraph1.LoadSettings(pSettings);
	m_TimeGraph2.LoadSettings(pSettings);
	m_TimeGraph3.LoadSettings(pSettings);
	m_TimeGraph4.LoadSettings(pSettings);
	m_LongRLGraph.LoadSettings(pSettings);
	m_LatRLGraph.LoadSettings(pSettings);
	m_WingGraph1.LoadSettings(pSettings);
	m_WingGraph2.LoadSettings(pSettings);
	m_WingGraph3.LoadSettings(pSettings);
	m_WingGraph4.LoadSettings(pSettings);
	m_WPlrGraph1.LoadSettings(pSettings);
	m_WPlrGraph2.LoadSettings(pSettings);
	m_WPlrGraph3.LoadSettings(pSettings);
	m_WPlrGraph4.LoadSettings(pSettings);
	SetWGraphTitles(&m_WPlrGraph1);
	SetWGraphTitles(&m_WPlrGraph2);
	SetWGraphTitles(&m_WPlrGraph3);
	SetWGraphTitles(&m_WPlrGraph4);

	return true;
}


void QMiarex::mouseDoubleClickEvent ( QMouseEvent * event )
{
	if(m_iView==W3DVIEW || !m_pCurGraph) return;
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	OnGraphSettings();
}


void QMiarex::mouseMoveEvent(QMouseEvent *event)
{
	if(!hasFocus()) setFocus();
	static CVector Real;
	static QPoint Delta, point;
	static bool bCtrl;
	static double xu, yu, x1, y1, xmin, xmax, ymin, ymax;

	Delta.setX(event->pos().x() - m_LastPoint.x());
	Delta.setY(event->pos().y() - m_LastPoint.y());
	point = event->pos();
	m_pCurGraph = GetGraph(point);
	bCtrl = false;
	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;
	if(m_iView==W3DVIEW || (m_iView==WSTABVIEW && m_iStabilityView==3))
	{
		ClientToGL(point, Real);

		if (event->buttons() & Qt::LeftButton)
		{
			if(bCtrl)//rotate
			{
				m_ArcBall.Move(point.x(), m_rCltRect.height()-point.y());
				UpdateView();
			}
			else if(m_bTrans)
			{
				//translate
				m_glViewportTrans.x += (GLfloat)(Delta.x()*2.0*m_GLScale/m_glScaled/m_rCltRect.width());
				m_glViewportTrans.y += (GLfloat)(Delta.y()*2.0*m_GLScale/m_glScaled/m_rCltRect.width());

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

				m_ArcBall.Move(point.x(), m_rCltRect.height()-point.y());
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
				if(m_iStabilityView==0)
				{
					m_WingLegendOffset.rx() += Delta.x();
					m_WingLegendOffset.ry() += Delta.y();
					UpdateView();
					
				}
				else if(m_iStabilityView==1)
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
			MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
			pMainFrame->statusBar()->showMessage(QString("X = %1, Y = %2").arg(m_pCurGraph->ClientTox(event->x())).arg(m_pCurGraph->ClientToy(event->y())));
		}
	}
	m_LastPoint = point;
}



void QMiarex::mousePressEvent(QMouseEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
	static complex<double> eigenvalue;
	static int i,j,k, isel, jsel,xval, yval;
	static double diff, dmax;
	QString str,suffix;

	if (event->buttons() & Qt::MidButton)
	{
		m_bArcball = true;
		m_ArcBall.Start(event->pos().x(), m_rCltRect.height()-event->pos().y());
		m_bCrossPoint = true;

		Set3DRotationCenter();
		UpdateView();
	}
	else if (event->buttons() & Qt::LeftButton)
	{
		QPoint point = event->pos();

		if(m_iView==W3DVIEW || (m_iView==WSTABVIEW && m_iStabilityView==3))
		{
			GLWidget *pGLWidget = (GLWidget*)m_pGLWidget;
		//	point is in client coordinates

			CVector Real;
			bool bCtrl = false;
			if(event->modifiers() & Qt::ControlModifier) bCtrl =true;

			ClientToGL(point, Real);
			if(m_rCltRect.contains(point)) pGLWidget->setFocus();

			if(m_bPickCenter)
			{
				Set3DRotationCenter(point);
				m_bPickCenter = false;
				m_pctrlPickCenter->setChecked(false);
			}
			else
			{
				m_ArcBall.Start(point.x(), m_rCltRect.height()-point.y());
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
			TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;
			m_pCurGraph = GetGraph(point);
			if(m_rCltRect.contains(point)) p2DWidget->setFocus();
			
			if(m_pCurGraph && m_iView==WSTABVIEW && (m_iStabilityView==1)
			   && m_pCurWPolar && m_pCurWPolar->m_AnalysisType==4)
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
	if(m_iView==W3DVIEW || (m_iView==WSTABVIEW && m_iStabilityView==3))
	{
		QGLWidget* pGLWidget = (QGLWidget*)m_pGLWidget;
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
		TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;
		p2DWidget->setCursor(Qt::CrossCursor);
	}
	m_bTrans = false;
	m_bDragPoint  = false;
}



void QMiarex::NormalVector(GLdouble p1[3], GLdouble p2[3],  GLdouble p3[3], GLdouble n[3])
{
	GLdouble v1[3], v2[3], d;
	// calculate two vectors, using the middle point
	// as the common origin
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
	m_bArcball = false;
	if(m_iView==W3DVIEW)
	{
//		m_pctrlMiddleControls->setCurrentIndex(0);
//		m_pctrBottomControls->setCurrentIndex(1);
		SetControls();
		UpdateView();
		return;
	}

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	m_bIs3DScaleSet = false;

	if(m_pCurPlane)
	{
		if(m_pCurPlane->m_bBody) m_pCurBody = m_pCurPlane->m_pBody;
		else                     m_pCurBody = NULL;
	}
	else m_pCurBody = NULL;

	m_iView =W3DVIEW;

	if(m_pCurPlane && m_pCurPlane->m_bBody && m_pCurPlane->m_bBody)
	{
		m_pCurBody = m_pCurPlane->m_pBody;
		m_bResetglBody = true;
	}
	else m_pCurBody = NULL;

	pMainFrame->SetCentralWidget();
//	m_pctrlMiddleControls->setCurrentIndex(0);
//	m_pctrBottomControls->setCurrentIndex(1);
	SetControls();
	UpdateView();
}




void QMiarex::On3DCp()
{
	m_b3DCp = m_pctrlCp->isChecked();
	if(m_b3DCp)
	{
		m_bSurfaces = false;
		m_pctrlSurfaces->setChecked(false);
	}
	UpdateView();
}

void QMiarex::On3DIso()
{
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
	m_ArcBall.SetQuat(sqrt(2.0)/2.0, 0.0, 0.0, -sqrt(2.0)/2.0);
	Set3DRotationCenter();
	UpdateView();
}


void QMiarex::On3DLeft()
{
	m_ArcBall.SetQuat(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0, 0.0);// rotate by 90 deg around x
	Set3DRotationCenter();
	UpdateView();
}


void QMiarex::On3DFront()
{
	Quaternion Qt1(sqrt(2.0)/2.0, 0.0,           -sqrt(2.0)/2.0, 0.0);// rotate by 90 deg around y
	Quaternion Qt2(sqrt(2.0)/2.0, -sqrt(2.0)/2.0, 0.0,           0.0);// rotate by 90 deg around x

	m_ArcBall.SetQuat(Qt1 * Qt2);
	Set3DRotationCenter();
	UpdateView();
}


void QMiarex::On3DReset()
{
	m_glViewportTrans.Set(0.0, 0.0, 0.0);
	m_bPickCenter   = false;
	m_bIs3DScaleSet = false;
	Set3DScale();
	Set3DRotationCenter();
	UpdateView();
}


void QMiarex::On3DPrefs()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	W3dPrefsDlg SDlg;
	SDlg.move(pMainFrame->m_DlgPos);
	SDlg.m_pMiarex = this;
	SDlg.m_bWakePanels    = m_bWakePanels;
	SDlg.m_3DAxisColor    = m_3DAxisColor;
	SDlg.m_3DAxisStyle    = m_3DAxisStyle;
	SDlg.m_3DAxisWidth    = m_3DAxisWidth;
	SDlg.m_VLMColor       = m_VLMColor;
	SDlg.m_VLMStyle       = m_VLMStyle;
	SDlg.m_VLMWidth       = m_VLMWidth;
	SDlg.m_OutlineColor   = m_OutlineColor;
	SDlg.m_OutlineStyle   = m_OutlineStyle;
	SDlg.m_OutlineWidth   = m_OutlineWidth;
	SDlg.m_TopColor       = m_XTopColor;
	SDlg.m_TopStyle       = m_XTopStyle;
	SDlg.m_TopWidth       = m_XTopWidth;
	SDlg.m_BotColor       = m_XBotColor;
	SDlg.m_BotStyle       = m_XBotStyle;
	SDlg.m_BotWidth       = m_XBotWidth;
	SDlg.m_XCPColor       = m_XCPColor;
	SDlg.m_XCPStyle       = m_XCPStyle;
	SDlg.m_XCPWidth       = m_XCPWidth;
	SDlg.m_MomentColor    = m_MomentColor;
	SDlg.m_MomentStyle    = m_MomentStyle;
	SDlg.m_MomentWidth    = m_MomentWidth;
	SDlg.m_IDragColor     = m_IDragColor;
	SDlg.m_IDragStyle     = m_IDragStyle;
	SDlg.m_IDragWidth     = m_IDragWidth;
	SDlg.m_VDragColor     = m_VDragColor;
	SDlg.m_VDragStyle     = m_VDragStyle;
	SDlg.m_VDragWidth     = m_VDragWidth;
	SDlg.m_DownwashColor  = m_DownwashColor;
	SDlg.m_DownwashStyle  = m_DownwashStyle;
	SDlg.m_DownwashWidth  = m_DownwashWidth;
	SDlg.m_WakeColor      = m_WakeColor;
	SDlg.m_WakeStyle      = m_WakeStyle;
	SDlg.m_WakeWidth      = m_WakeWidth;
	SDlg.InitDialog();

	if(SDlg.exec() == QDialog::Accepted)
	{
		m_bWakePanels    = SDlg.m_bWakePanels;
		m_3DAxisColor    = SDlg.m_3DAxisColor;
		m_3DAxisStyle    = SDlg.m_3DAxisStyle;
		m_3DAxisWidth    = SDlg.m_3DAxisWidth;
		m_VLMColor     = SDlg.m_VLMColor;
		m_VLMStyle     = SDlg.m_VLMStyle;
		m_VLMWidth     = SDlg.m_VLMWidth;
		m_OutlineColor = SDlg.m_OutlineColor;
		m_OutlineStyle = SDlg.m_OutlineStyle;
		m_OutlineWidth = SDlg.m_OutlineWidth;
		m_XTopColor    = SDlg.m_TopColor;
		m_XTopStyle    = SDlg.m_TopStyle;
		m_XTopWidth    = SDlg.m_TopWidth;
		m_XBotColor    = SDlg.m_BotColor;
		m_XBotStyle    = SDlg.m_BotStyle;
		m_XBotWidth    = SDlg.m_BotWidth;
		m_XCPColor     = SDlg.m_XCPColor;
		m_XCPStyle     = SDlg.m_XCPStyle;
		m_XCPWidth     = SDlg.m_XCPWidth;
		m_MomentColor  = SDlg.m_MomentColor;
		m_MomentStyle  = SDlg.m_MomentStyle;
		m_MomentWidth  = SDlg.m_MomentWidth;
		m_IDragColor     = SDlg.m_IDragColor;
		m_IDragStyle     = SDlg.m_IDragStyle;
		m_IDragWidth     = SDlg.m_IDragWidth;
		m_VDragColor     = SDlg.m_VDragColor;
		m_VDragStyle     = SDlg.m_VDragStyle;
		m_VDragWidth     = SDlg.m_VDragWidth;
		m_DownwashColor  = SDlg.m_DownwashColor;
		m_DownwashStyle  = SDlg.m_DownwashStyle;
		m_DownwashWidth  = SDlg.m_DownwashWidth;
		m_WakeColor      = SDlg.m_WakeColor;
		m_WakeStyle      = SDlg.m_WakeStyle;
		m_WakeWidth      = SDlg.m_WakeWidth;
		m_bResetglWake = true;
		m_bResetglBody = true;
		m_bResetglGeom = true;
		m_bResetglMesh = true;
		m_bResetglOpp  = true;

		UpdateView();
	}
	pMainFrame->m_DlgPos = SDlg.pos();
}



void QMiarex::On3DPickCenter()
{
	m_bPickCenter = true;
	m_pctrlPickCenter->setChecked(true);
}



void QMiarex::OnAllWingGraphScales()
{
	//resets the scale of the current graph
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	double halfspan = m_pCurWing->m_PlanformSpan/2.0;

	m_WingGraph1.SetAuto(true);
	m_WingGraph1.ResetXLimits();
	m_WingGraph1.ResetYLimits();
	m_WingGraph1.SetAutoX(false);
	m_WingGraph2.SetAuto(true);
	m_WingGraph2.ResetXLimits();
	m_WingGraph2.ResetYLimits();
	m_WingGraph2.SetAutoX(false);
	m_WingGraph3.SetAuto(true);
	m_WingGraph3.ResetXLimits();
	m_WingGraph3.ResetYLimits();
	m_WingGraph3.SetAutoX(false);
	m_WingGraph4.SetAuto(true);
	m_WingGraph4.ResetXLimits();
	m_WingGraph4.ResetYLimits();
	m_WingGraph4.SetAutoX(false);

	if(m_bHalfWing)
	{
		m_WingGraph1.SetXMin(0.0);
		m_WingGraph2.SetXMin(0.0);
		m_WingGraph3.SetXMin(0.0);
		m_WingGraph4.SetXMin(0.0);
	}
	else
	{
		m_WingGraph1.SetXMin(-halfspan*pMainFrame->m_mtoUnit);
		m_WingGraph2.SetXMin(-halfspan*pMainFrame->m_mtoUnit);
		m_WingGraph3.SetXMin(-halfspan*pMainFrame->m_mtoUnit);
		m_WingGraph4.SetXMin(-halfspan*pMainFrame->m_mtoUnit);
	}
	m_WingGraph1.SetXMax( halfspan*pMainFrame->m_mtoUnit);
	m_WingGraph2.SetXMax( halfspan*pMainFrame->m_mtoUnit);
	m_WingGraph3.SetXMax( halfspan*pMainFrame->m_mtoUnit);
	m_WingGraph4.SetXMax( halfspan*pMainFrame->m_mtoUnit);

	UpdateView();
}


void QMiarex::OnAllWPolarGraphScales()
{
	if(m_iView==WPOLARVIEW)
	{
		m_WPlrGraph1.SetAuto(true);
		m_WPlrGraph1.ResetXLimits();
		m_WPlrGraph1.ResetYLimits();
		m_WPlrGraph2.SetAuto(true);
		m_WPlrGraph2.ResetXLimits();
		m_WPlrGraph2.ResetYLimits();
		m_WPlrGraph3.SetAuto(true);
		m_WPlrGraph3.ResetXLimits();
		m_WPlrGraph3.ResetYLimits();
		m_WPlrGraph4.SetAuto(true);
		m_WPlrGraph4.ResetXLimits();
		m_WPlrGraph4.ResetYLimits();
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
	UpdateView();
}

void QMiarex::OnAllWingGraphSettings()
{
	GraphDlg dlg;
	QGraph graph;
	graph.CopySettings(&m_WingGraph1);
	dlg.m_pMemGraph = &graph;
	dlg.m_pGraph    = &m_WingGraph1;
	dlg.m_GraphArray[0] = &m_WingGraph1;
	dlg.m_GraphArray[1] = &m_WingGraph2;
	dlg.m_GraphArray[2] = &m_WingGraph3;
	dlg.m_GraphArray[3] = &m_WingGraph4;
	dlg.m_NGraph = 4;
	dlg.SetParams();

	if(dlg.exec() == QDialog::Accepted)
	{
	}
	else
	{
		m_WingGraph1.CopySettings(&graph);
		m_WingGraph2.CopySettings(&graph);
		m_WingGraph3.CopySettings(&graph);
		m_WingGraph4.CopySettings(&graph);
	}
	UpdateView();
}


void QMiarex::OnAllWPolarGraphSettings()
{
	GraphDlg dlg;
	QGraph graph;
	graph.CopySettings(&m_WPlrGraph1);
	dlg.m_pMemGraph = &graph;
	dlg.m_pGraph    = &m_WPlrGraph1;
	dlg.m_GraphArray[0] = &m_WPlrGraph1;
	dlg.m_GraphArray[1] = &m_WPlrGraph2;
	dlg.m_GraphArray[2] = &m_WPlrGraph3;
	dlg.m_GraphArray[3] = &m_WPlrGraph4;
	dlg.m_NGraph = 4;
	dlg.SetParams();

	if(dlg.exec() == QDialog::Accepted)
	{
	}
	else
	{
		m_WPlrGraph1.CopySettings(&graph);
		m_WPlrGraph2.CopySettings(&graph);
		m_WPlrGraph3.CopySettings(&graph);
		m_WPlrGraph4.CopySettings(&graph);
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
	int l;
	double V0, VMax, VDelta;
	bool bHigh = m_bHighlightOpp;
	m_bHighlightOpp = false;

		if(!m_pCurWing)
	{
		QMessageBox::warning(this, tr("Warning"), tr("Please define a wing or a plane object before running a calculation"));
		return;
	}
	if(!m_pCurWPolar)
	{
		QMessageBox::warning(this, tr("Warning"), tr("Please define an analysis/polar before running a calculation"));
		return;
	}

	m_bStream = m_bSpeeds = false;
	m_pctrlStream->setChecked(false);
	m_pctrlSurfVel->setChecked(false);

//	Read Analysis Params;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	if(m_pCurWPolar->m_Type <4)
	{
		V0     = m_AlphaMin;
		VMax   = m_AlphaMax;
		VDelta = m_AlphaDelta;
	}
	else if(m_pCurWPolar->m_Type==4)
	{
		V0     = m_QInfMin;
		VMax   = m_QInfMax;
		VDelta = m_QInfDelta;
	}
	else if(m_pCurWPolar->m_Type==5 || m_pCurWPolar->m_Type==6 || m_pCurWPolar->m_Type==7)
	{
		V0     = m_ControlMin;
		VMax   = m_ControlMax;
		VDelta = m_ControlDelta;
	}
	else
	{
		V0 = VMax = VDelta = 0.0;
	}


	// check if all the foils are there...
	// ...could have been deleted or renamed or not imported with AVL wing or whatever
	for (l=0; l<=m_pCurWing->m_NPanel; l++)
	{
		if (!pMainFrame->GetFoil(m_pCurWing->m_RFoil[l]))
		{
			QString strong;
			strong = tr("Could not find the wing's foil ")+ m_pCurWing->m_RFoil[l] +tr("...\nAborting Calculation");
			QMessageBox::warning(this, tr("Warning"), strong);
			return;
		}
		if (!pMainFrame->GetFoil(m_pCurWing->m_LFoil[l]))
		{
			QString strong;
			strong = tr("Could not find the wing's foil ")+ m_pCurWing->m_LFoil[l] +tr("...\nAborting Calculation");
			QMessageBox::warning(this, tr("Warning"), strong);
			return;
		}
	}

	if(m_pCurStab)
	{
		for (l=0; l<=m_pCurStab->m_NPanel; l++){
			if (!pMainFrame->GetFoil(m_pCurStab->m_RFoil[l]))
			{
				QString strong;
				strong = tr("Could not find the elevator's foil ")+ m_pCurStab->m_RFoil[l] +tr("...\nAborting Calculation");
				QMessageBox::warning(this, tr("Warning"), strong);
				return;
			}
			if (!pMainFrame->GetFoil(m_pCurStab->m_LFoil[l]))
			{
				QString strong;
				strong = tr("Could not find the elevator's foil ")+ m_pCurStab->m_LFoil[l] +tr("...\nAborting Calculation");
				QMessageBox::warning(this, tr("Warning"), strong);
				return;
			}
		}
	}

	if(m_pCurFin)
	{
		for (l=0; l<=m_pCurFin->m_NPanel; l++)
		{
			if (!pMainFrame->GetFoil(m_pCurFin->m_RFoil[l]))
			{
				QString strong;
				strong = tr("Could not find the fin's foil ")+ m_pCurFin->m_RFoil[l] +tr("...\nAborting Calculation");
				QMessageBox::warning(this, tr("Warning"), strong);
				return;
			}
		}
	}

	if(m_pCurWPolar->m_AnalysisType==1)
	{
		LLTAnalyze(V0, VMax, VDelta, m_bSequence, m_bInitLLTCalc);
	}
	else if(m_pCurWPolar->m_AnalysisType==2)
	{
		VLMAnalyze(V0, VMax, VDelta, m_bSequence);
	}
	else if(m_pCurWPolar->m_AnalysisType==3)
	{
		PanelAnalyze(V0, VMax, VDelta, m_bSequence);
	}
	else if(m_pCurWPolar->m_AnalysisType==4)
	{
		StabAnalyze(V0, VMax, VDelta, m_bSequence);
	}

	m_bHighlightOpp = bHigh;
	if(m_iView==WPOLARVIEW)     CreateWPolarCurves();
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();
	UpdateView();
	SetControls();
	pMainFrame->setFocus();
}



void QMiarex::OnAnimateWOpp()
{
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
					pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName)
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
					pWOpp->m_WingName == m_pCurWing->m_WingName)
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
	if(m_iView!=WSTABVIEW) return; //nothing to animate
	if(!m_pCurWing || !m_pCurWPolar || !m_pCurWPolar->m_Type==7) return;
	if(!m_pCurWOpp) return;

	static double t, sigma, s2, omega, o2, theta_sum, psi_sum, norm;
	double *vabs, *phi;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
	norm = m_ModeNorm * pStabView->m_ModeAmplitude;
	vabs = pStabView->m_vabs;
	phi  = pStabView->m_phi;

	sigma = m_pCurWOpp->m_EigenValue[pStabView->m_iCurrentMode].real();
	omega = m_pCurWOpp->m_EigenValue[pStabView->m_iCurrentMode].imag();
	s2 = sigma*sigma;
	o2 = omega*omega;
	t=m_ModeTime;

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
	
	if(bStep) m_ModeTime += m_Modedt;
	
	UpdateView();
}



void QMiarex::OnAnimateWOppSingle()
{
	static bool bIsValid, bSkipOne;
	static int size;
	static CPOpp *pPOpp;
	static CWOpp *pWOpp;
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
			bIsValid =(pPOpp->m_PlrName==m_pCurWPolar->m_PlrName &&
					  pPOpp->m_PlaneName==m_pCurPlane->m_PlaneName);
		else
			bIsValid =(pWOpp->m_PlrName==m_pCurWPolar->m_PlrName &&
					  pWOpp->m_WingName==m_pCurWing->m_WingName);

		if (bIsValid && !bSkipOne)
		{
			if(m_pCurPlane)
			{
				m_pCurPOpp = pPOpp;
				m_pCurWOpp = &pPOpp->m_WingWOpp;
			}
			else
			{
				m_pCurPOpp = NULL;
				m_pCurWOpp = pWOpp;
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
	if(m_pTimerWOpp->isActive())
	{
		m_pTimerWOpp->setInterval(800-val);
	}
}


void QMiarex::OnAdjustToWing()
{
	//Adjust the graph's scale to the wing
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(!m_pCurWing) return;

	double halfspan = m_pCurWing->m_PlanformSpan/2.0;
	m_WingGraph1.SetAutoX(false);
	m_WingGraph1.SetXMax( halfspan*pMainFrame->m_mtoUnit);
	m_WingGraph2.SetAutoX(false);
	m_WingGraph2.SetXMax( halfspan*pMainFrame->m_mtoUnit);
	m_WingGraph3.SetAutoX(false);
	m_WingGraph3.SetXMax( halfspan*pMainFrame->m_mtoUnit);
	m_WingGraph4.SetAutoX(false);
	m_WingGraph4.SetXMax( halfspan*pMainFrame->m_mtoUnit);

	double xmin = -halfspan*pMainFrame->m_mtoUnit;
	if(m_bHalfWing) xmin = 0.0;
	m_WingGraph1.SetXMin(xmin);
	m_WingGraph2.SetXMin(xmin);
	m_WingGraph3.SetXMin(xmin);
	m_WingGraph4.SetXMin(xmin);
}


void QMiarex::OnAdvancedSettings()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	WAdvancedDlg dlg;
	dlg.move(pMainFrame->m_DlgPos);
	dlg.m_AlphaPrec       = CWing::s_CvPrec;
	dlg.m_Relax           = CWing::s_RelaxMax;
	dlg.m_NStation        = CWing::s_NLLTStations;
	dlg.m_Iter            = m_Iter;
	dlg.m_MaxWakeIter     = m_MaxWakeIter;
	dlg.m_CoreSize        = m_CoreSize;
	dlg.m_bResetWake      = m_bResetWake;
	dlg.m_bDirichlet      = m_bDirichlet;
	dlg.m_bTrefftz        = m_bTrefftz;
	dlg.m_bKeepOutOpps    = m_bKeepOutOpps;
	dlg.m_bLogFile        = m_bLogFile;
	dlg.m_MinPanelSize    = m_MinPanelSize;
	dlg.m_ControlPos      = CPanel::m_CtrlPos;
	dlg.m_VortexPos       = CPanel::m_VortexPos;
	dlg.m_WakeInterNodes  = m_WakeInterNodes;
	dlg.m_pMainFrame = m_pMainFrame;

	dlg.InitDialog();
	if(dlg.exec() == QDialog::Accepted)
	{
		CWing::s_CvPrec        = dlg.m_AlphaPrec;
		CWing::s_RelaxMax      = dlg.m_Relax;
		CWing::s_NLLTStations  = dlg.m_NStation;
		m_Iter                 = dlg.m_Iter;
		m_MaxWakeIter          = dlg.m_MaxWakeIter;
		m_CoreSize             = dlg.m_CoreSize;
		m_bResetWake           = dlg.m_bResetWake;
		m_bDirichlet           = dlg.m_bDirichlet;
		m_bTrefftz             = dlg.m_bTrefftz;
		m_bKeepOutOpps         = dlg.m_bKeepOutOpps;
		m_WakeInterNodes       = dlg.m_WakeInterNodes;
		m_MinPanelSize         = dlg.m_MinPanelSize;

		m_InducedDragPoint     = dlg.m_InducedDragPoint;

		CPanel::m_CtrlPos      = dlg.m_ControlPos;
		CPanel::m_VortexPos    = dlg.m_VortexPos;

		if(dlg.m_bLogFile) m_bLogFile = true; else m_bLogFile = false;
		m_bResetglWake    = true;
		UpdateView();
	}
	pMainFrame->m_DlgPos = dlg.pos();
}


void QMiarex::OnBodyInertia()
{
	if(!m_pCurBody) return;
	InertiaDlg dlg;
	dlg.m_pWing = NULL;
	dlg.m_pMainFrame = m_pMainFrame;
	dlg.m_pBody = m_pCurBody;
	dlg.InitDialog();
	dlg.exec();
}


void QMiarex::OnClipPlane(int pos)
{
	double planepos =  (double)pos/100.0;
	m_ClipPlanePos = sinh(planepos) * 0.5;
	UpdateView();
}



void QMiarex::OnCpSection(int pos)
{
	m_CurSpanPos = (double)pos/100.0;
	m_pctrlSpanPos->SetValue(m_CurSpanPos);
	CreateCpCurves();
	UpdateView();
}


void QMiarex::OnCpPosition()
{
	m_CurSpanPos = m_pctrlSpanPos->GetValue();
	m_pctrlCpSectionSlider->setValue((int)(m_CurSpanPos*100.0));
	CreateCpCurves();
	UpdateView();
}

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

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->SetCentralWidget();

//	SetWPlrLegendPos();
	m_pCurGraph = &m_CpGraph;

//	m_pctrlMiddleControls->setCurrentIndex(1);
//	m_pctrBottomControls->setCurrentIndex(0);

	CreateCpCurves();
	SetCurveParams();
	SetControls();
	UpdateView();
}



void QMiarex::OnCurWOppOnly()
{
	m_bCurWOppOnly = !m_bCurWOppOnly;
	if(m_iView==WOPPVIEW)
	{
		CreateWOppCurves();
		UpdateView();
	}
	else if(m_iView==WSTABVIEW && m_iStabilityView==0)
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



void QMiarex::OnDefineCtrlPolar()
{
	m_bArcball = false;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	m_CtrlPolarDlg.m_QInf          = m_WngAnalysis.m_QInf;
	m_CtrlPolarDlg.m_Mass          = m_WngAnalysis.m_Weight;
	m_CtrlPolarDlg.m_Viscosity     = m_WngAnalysis.m_Viscosity;
	m_CtrlPolarDlg.m_Density       = m_WngAnalysis.m_Density;
	m_CtrlPolarDlg.m_RefAreaType   = m_WngAnalysis.m_RefAreaType;


	m_CtrlPolarDlg.m_pPlane = m_pCurPlane;
	m_CtrlPolarDlg.m_pWing  = m_pCurWing;
	m_CtrlPolarDlg.m_pWing2 = m_pCurWing;
	m_CtrlPolarDlg.m_pStab  = m_pCurStab;
	m_CtrlPolarDlg.m_pFin   = m_pCurFin;
	m_CtrlPolarDlg.m_pMainFrame = m_pMainFrame;
	m_CtrlPolarDlg.m_poaXPolar  = m_poaWPolar;


	CWPolar* pCurWPolar       = new CWPolar;
	if (m_pCurPlane) pCurWPolar->m_UFOName = m_pCurPlane->m_PlaneName;
	else             pCurWPolar->m_UFOName = m_pCurWing->m_WingName;

	pCurWPolar->m_WMAChord     = m_pCurWing->m_MAChord;

	m_CtrlPolarDlg.InitDialog();
	m_CtrlPolarDlg.move(pMainFrame->m_DlgPos);
	int res = m_CtrlPolarDlg.exec();
	pMainFrame->m_DlgPos = m_CtrlPolarDlg.pos();
	if(res == QDialog::Accepted)
	{
		m_WngAnalysis.m_QInf          = m_CtrlPolarDlg.m_QInf;
		m_WngAnalysis.m_Weight        = m_CtrlPolarDlg.m_Mass;
		m_WngAnalysis.m_Viscosity     = m_CtrlPolarDlg.m_Viscosity;
		m_WngAnalysis.m_Density       = m_CtrlPolarDlg.m_Density;
		m_WngAnalysis.m_RefAreaType   = m_CtrlPolarDlg.m_RefAreaType;

		//Then add WPolar to array
		pCurWPolar->m_Type            = m_CtrlPolarDlg.m_Type;
		pCurWPolar->m_QInf            = m_CtrlPolarDlg.m_QInf;
		pCurWPolar->m_Weight          = m_CtrlPolarDlg.m_Mass;
		pCurWPolar->m_PlrName         = m_CtrlPolarDlg.m_WPolarName;
		pCurWPolar->m_Density         = m_CtrlPolarDlg.m_Density;
		pCurWPolar->m_Viscosity       = m_CtrlPolarDlg.m_Viscosity;
		pCurWPolar->m_bViscous        = m_CtrlPolarDlg.m_bViscous;

		pCurWPolar->m_RefAreaType     = m_CtrlPolarDlg.m_RefAreaType;
		if(pCurWPolar->m_RefAreaType==1)
		{
			pCurWPolar->m_WArea        = m_pCurWing->m_PlanformArea;
			pCurWPolar->m_WSpan        = m_pCurWing->m_PlanformSpan;
		}
		else
		{
			pCurWPolar->m_WArea        = m_pCurWing->m_ProjectedArea;
			pCurWPolar->m_WSpan        = m_pCurWing->m_ProjectedSpan;
		}
		pCurWPolar->m_bVLM1           = true;
		pCurWPolar->m_bTiltedGeom     = false;
		pCurWPolar->m_bWakeRollUp     = false;
		pCurWPolar->m_AnalysisType    = 2; //vlm
		pCurWPolar->m_bThinSurfaces   = false;
		pCurWPolar->m_bGround         = false;
		pCurWPolar->m_CoG.x          = 0.0;
		pCurWPolar->m_ASpec           = 0.0;
		pCurWPolar->m_Beta            = 0.0;
		pCurWPolar->m_Height          = 0.0;
		pCurWPolar->m_TotalWakeLength = 10.0;
		pCurWPolar->m_WakePanelFactor = 1.1;
		pCurWPolar->m_NXWakePanels    = 1;

		pCurWPolar->m_nControls = m_CtrlPolarDlg.m_nControls;
		for(int i=0; i<m_CtrlPolarDlg.m_nControls; i++)
		{
			pCurWPolar->m_bActiveControl[i] = m_CtrlPolarDlg.m_bActiveControl[i];
			pCurWPolar->m_MinControl[i]     = m_CtrlPolarDlg.m_MinControl[i];
			pCurWPolar->m_MaxControl[i]     = m_CtrlPolarDlg.m_MaxControl[i];
		}

		for(int i=m_CtrlPolarDlg.m_nControls; i<4*MAXCONTROLS; i++)
		{
			pCurWPolar->m_bActiveControl[i] = false;
		}

//		pCurWPolar->m_bPolar = true;

		pCurWPolar->m_Color = pMainFrame->GetColor(4);
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
		pCurWPolar->m_bIsVisible = true;

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
		UpdateView();
	}
	else
	{
		delete pCurWPolar;
	}
	SetControls();
}


void QMiarex::OnDefineStabPolar()
{
	m_bArcball = false;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	m_StabPolarDlg.m_Viscosity     = m_WngAnalysis.m_Viscosity;
	m_StabPolarDlg.m_Density       = m_WngAnalysis.m_Density;
	m_StabPolarDlg.m_RefAreaType   = m_WngAnalysis.m_RefAreaType;

	m_StabPolarDlg.m_pPlane = m_pCurPlane;
	m_StabPolarDlg.m_pWing  = m_pCurWing;
	m_StabPolarDlg.m_pWing2 = m_pCurWing;
	m_StabPolarDlg.m_pStab  = m_pCurStab;
	m_StabPolarDlg.m_pFin   = m_pCurFin;
	m_StabPolarDlg.m_pMainFrame = m_pMainFrame;
	m_StabPolarDlg.m_poaXPolar  = m_poaWPolar;

	CWPolar* pCurWPolar       = new CWPolar;
	if (m_pCurPlane) pCurWPolar->m_UFOName = m_pCurPlane->m_PlaneName;
	else             pCurWPolar->m_UFOName = m_pCurWing->m_WingName;

	pCurWPolar->m_WMAChord     = m_pCurWing->m_MAChord;

	m_StabPolarDlg.InitDialog();
	m_StabPolarDlg.move(pMainFrame->m_DlgPos);
	int res = m_StabPolarDlg.exec();
	pMainFrame->m_DlgPos = m_StabPolarDlg.pos();

	if(res == QDialog::Accepted)
	{
		m_WngAnalysis.m_Viscosity     = m_StabPolarDlg.m_Viscosity;
		m_WngAnalysis.m_Density       = m_StabPolarDlg.m_Density;
		m_WngAnalysis.m_RefAreaType   = m_StabPolarDlg.m_RefAreaType;

		//Then add WPolar to array
		pCurWPolar->m_Weight          = m_StabPolarDlg.m_Mass;

		pCurWPolar->m_bAutoInertia    = m_StabPolarDlg.m_bAutoInertia;
		pCurWPolar->m_CoG.x           = m_StabPolarDlg.m_CoG.x;
		pCurWPolar->m_CoG.z           = m_StabPolarDlg.m_CoG.z;

		pCurWPolar->m_CoGIxx          = m_StabPolarDlg.m_CoGIxx;
		pCurWPolar->m_CoGIyy          = m_StabPolarDlg.m_CoGIyy;
		pCurWPolar->m_CoGIzz          = m_StabPolarDlg.m_CoGIzz;
		pCurWPolar->m_CoGIxz          = m_StabPolarDlg.m_CoGIxz;
		
		pCurWPolar->m_Type            = m_StabPolarDlg.m_Type;
		pCurWPolar->m_Beta            = m_StabPolarDlg.m_SideSlip;
		pCurWPolar->m_BankAngle       = m_StabPolarDlg.m_BankAngle;
		pCurWPolar->m_PlrName         = m_StabPolarDlg.m_WPolarName;
		pCurWPolar->m_Density         = m_StabPolarDlg.m_Density;
		pCurWPolar->m_Viscosity       = m_StabPolarDlg.m_Viscosity;
		pCurWPolar->m_bViscous        = m_StabPolarDlg.m_bViscous;

		pCurWPolar->m_RefAreaType     = m_StabPolarDlg.m_RefAreaType;
		if(pCurWPolar->m_RefAreaType==1)
		{
			pCurWPolar->m_WArea        = m_pCurWing->m_PlanformArea;
			pCurWPolar->m_WSpan        = m_pCurWing->m_PlanformSpan;
		}
		else
		{
			pCurWPolar->m_WArea        = m_pCurWing->m_ProjectedArea;
			pCurWPolar->m_WSpan        = m_pCurWing->m_ProjectedSpan;
		}
		pCurWPolar->m_bVLM1           = true;
		pCurWPolar->m_bTiltedGeom     = false;
		pCurWPolar->m_bWakeRollUp     = false;
		pCurWPolar->m_AnalysisType    = 4; //Stability analysis
		pCurWPolar->m_bThinSurfaces   = false;
		pCurWPolar->m_bGround         = false;
		pCurWPolar->m_ASpec           = 0.0;
		pCurWPolar->m_Height          = 0.0;
		pCurWPolar->m_TotalWakeLength = 10.0;
		pCurWPolar->m_WakePanelFactor = 1.1;
		pCurWPolar->m_NXWakePanels    = 1;

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
		m_iView = WSTABVIEW;
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
	if(!m_pCurWing) return;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	int i,j;

	CWPolar* pNewWPolar       = new CWPolar;
	if (m_pCurPlane) pNewWPolar->m_UFOName = m_pCurPlane->m_PlaneName;
	else             pNewWPolar->m_UFOName = m_pCurWing->m_WingName;

	pNewWPolar->m_WMAChord     = m_pCurWing->m_MAChord;

	m_WngAnalysis.m_pMainFrame = m_pMainFrame;
	m_WngAnalysis.m_pWing       = m_pCurWing;
	m_WngAnalysis.m_pPlane      = m_pCurPlane;
	m_WngAnalysis.m_pMainFrame  = m_pMainFrame;
	m_WngAnalysis.m_poaWPolar   = m_poaWPolar;
	if(m_pCurPlane)	m_WngAnalysis.m_UFOName   = m_pCurPlane->m_PlaneName;
	else            m_WngAnalysis.m_UFOName   = m_pCurWing->m_WingName;
	m_WngAnalysis.InitDialog();
	m_WngAnalysis.move(pMainFrame->m_DlgPos);
	int res = m_WngAnalysis.exec();
	pMainFrame->m_DlgPos = m_WngAnalysis.pos();
	if (res == QDialog::Accepted)
	{
		//Then add WPolar to array
		pNewWPolar->m_RefAreaType     = m_WngAnalysis.m_RefAreaType;
		if(pNewWPolar->m_RefAreaType==1)
		{
			pNewWPolar->m_WArea = m_pCurWing->m_PlanformArea;
			pNewWPolar->m_WSpan = m_pCurWing->m_PlanformSpan;
		}
		else
		{
			pNewWPolar->m_WArea = m_pCurWing->m_ProjectedArea;
			pNewWPolar->m_WSpan = m_pCurWing->m_ProjectedSpan;
		}
		pNewWPolar->m_Type            = m_WngAnalysis.m_Type;
		pNewWPolar->m_QInf            = m_WngAnalysis.m_QInf;
		pNewWPolar->m_Weight          = m_WngAnalysis.m_Weight;
		pNewWPolar->m_CoG             = m_WngAnalysis.m_CoG;
		pNewWPolar->m_Beta            = m_WngAnalysis.m_Beta;
		pNewWPolar->m_ASpec           = m_WngAnalysis.m_Alpha;
		pNewWPolar->m_PlrName         = m_WngAnalysis.m_WPolarName;
		pNewWPolar->m_Density         = m_WngAnalysis.m_Density;
		pNewWPolar->m_Viscosity       = m_WngAnalysis.m_Viscosity;
		pNewWPolar->m_bVLM1           = m_WngAnalysis.m_bVLM1;
		pNewWPolar->m_bTiltedGeom     = m_WngAnalysis.m_bTiltedGeom;
		pNewWPolar->m_bWakeRollUp     = m_WngAnalysis.m_bWakeRollUp;
		pNewWPolar->m_bViscous        = m_WngAnalysis.m_bViscous;
		pNewWPolar->m_AnalysisType    = m_WngAnalysis.m_AnalysisType;
		pNewWPolar->m_bThinSurfaces   = m_WngAnalysis.m_bThinSurfaces;
		pNewWPolar->m_bGround         = m_WngAnalysis.m_bGround;
		pNewWPolar->m_Height          = m_WngAnalysis.m_Height;
		pNewWPolar->m_TotalWakeLength = m_WngAnalysis.m_TotalWakeLength;
		pNewWPolar->m_WakePanelFactor = m_WngAnalysis.m_WakePanelFactor;
		pNewWPolar->m_NXWakePanels    = m_WngAnalysis.m_NXWakePanels;

//		pNewWPolar->m_bPolar       = true;

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
	//Delete all the WOpps or POpps associated to the current WPolar

	if(!m_pCurWPolar) return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->SetSaveState(false);
	CWOpp* pWOpp;
	CPOpp* pPOpp;
	int i;
	if(m_pCurPlane)
	{
		for (i = (int)m_poaPOpp->size()-1; i>=0; i--)
		{
			pPOpp = (CPOpp*) m_poaPOpp->at(i);
			if(pPOpp->m_PlrName == m_pCurWPolar->m_PlrName &&
			   pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName)
			{
				m_poaPOpp->removeAt(i);
				delete pPOpp;
			}
		}
	}
	else if(m_pCurWing)
	{
		for (i = (int)m_poaWOpp->size()-1; i>=0; i--)
		{
			pWOpp = (CWOpp*) m_poaWOpp->at(i);
			if(pWOpp->m_PlrName == m_pCurWPolar->m_PlrName&&
			   pWOpp->m_WingName == m_pCurWing->m_WingName)
			{
				m_poaWOpp->removeAt(i);
				delete pWOpp;
			}
		}
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


void QMiarex::OnDeleteAllWOpps()
{
	//Delete all the WOpps or POpps
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->SetSaveState(false);
	CWOpp* pWOpp;
	CPOpp* pPOpp;
	int i;
	for (i = (int)m_poaPOpp->size()-1; i>=0; i--)
	{
		pPOpp = (CPOpp*) m_poaPOpp->at(i);
		m_poaPOpp->removeAt(i);
		delete pPOpp;
	}
	for (i = (int)m_poaWOpp->size()-1; i>=0; i--)
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
	//Delete the currently selected wing, if any
	if(!m_pCurWing) return;
	m_bAnimateWOpp = false;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString strong;
	if(m_pCurPlane)	strong = tr("Are you sure you want to delete the plane :\n") +  m_pCurPlane->m_PlaneName +"?\n";
	else 	        strong = tr("Are you sure you want to delete the wing :\n") +   m_pCurWing->m_WingName +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(window(), tr("Question"), strong,
												  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;

	if(m_pCurPlane) pMainFrame->DeletePlane(m_pCurPlane);
	else            pMainFrame->DeleteWing(m_pCurWing, false);

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
	//Deletes the currently selected WOpp or POpp
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
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
	//Delete all Opps for the current UFO
	CPOpp *pPOpp;
	CWOpp *pWOpp;
	int i;
	if(m_pCurPlane)
	{
		for (i=m_poaPOpp->size()-1; i>=0; i--)
		{
			pPOpp = (CPOpp*)m_poaPOpp->at(i);
			if (pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName)
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
			if (pWOpp->m_WingName == m_pCurWing->m_WingName)
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

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->SetSaveState(false);
	pMainFrame->UpdateWOpps();
	SetControls();
	UpdateView();
}


void QMiarex::OnDeleteUFOWPolars()
{
	if(!m_pCurWing) return;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	QString Name;

	if(m_pCurPlane) Name = m_pCurPlane->m_PlaneName;
	else            Name = m_pCurWing->m_WingName;

	CWPolar *pWPolar;
	for(int i=m_poaWPolar->size()-1; i>=0; i--)
	{
		pWPolar  = (CWPolar *)m_poaWPolar->at(i);
		if(pWPolar && pWPolar->m_UFOName==Name)
		{
			m_poaWPolar->removeAt(i);
			delete pWPolar;
		}
	}
	SetWPlr();
	pMainFrame->UpdateWPolars();
	SetControls();
	UpdateView();
}


void QMiarex::OnDeleteCurWPolar()
{
	//delete the current WPolar
	if(!m_pCurWPolar) return;
	m_bAnimateWOpp = false;
	int i;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	QString strong, UFOName;

	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return;

	strong = tr("Are you sure you want to delete the polar :\n") +  m_pCurWPolar->m_PlrName +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(window(), tr("Question"), strong,
												  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;

	//first remove all WOpps associated to the Wing Polar
	CWOpp * pWOpp;
	for (i=m_poaWOpp->size()-1; i>=0; i--)
	{
		pWOpp = (CWOpp*)m_poaWOpp->at(i);
		if (pWOpp->m_PlrName  == m_pCurWPolar->m_PlrName &&	pWOpp->m_WingName == UFOName)
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
		if (pPOpp->m_PlrName  == m_pCurWPolar->m_PlrName &&	pPOpp->m_PlaneName == UFOName)
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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	//Duplicate the currently selected wing
	if(!m_pCurWing)	return;

	if(m_pCurPlane)
	{
		DuplicatePlane();
		return;
	}

	QString UFOName;
	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	bool bUsed = false;
	int i;
	CPlane *pPlane;
	CWPolar *pWPolar;
	for (i=0; i< m_poaPlane->size(); i++)
	{
		pPlane = (CPlane*)m_poaPlane->at(i);
		if(pPlane->m_bBody && pPlane->m_pBody==m_pCurBody)
		{
			// Does this plane have results
			for(int j=0; j<m_poaWPolar->size(); j++)
			{
				pWPolar = (CWPolar*)m_poaWPolar->at(j);
				if(pWPolar->m_UFOName==pPlane->m_PlaneName && pWPolar->m_Alpha.size())
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
	m_GL3dBody.setWindowState(Qt::WindowMaximized);
	m_GL3dBody.InitDialog();

	if(m_GL3dBody.exec() == QDialog::Accepted)
	{
		if(bUsed)
		{
			ModDlg dlg;
			dlg.m_Question = tr("The modification will erase all results for the planes using this body.\nContinue ?");
			dlg.InitDialog();
			int Ans = dlg.exec();
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
					if(pPlane->m_pBody == m_pCurBody)
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

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	CWPolar MemWPolar;
	MemWPolar.Copy(m_pCurWPolar);

	EditPlrDlg dlg;
	dlg.m_pWPolar = m_pCurWPolar;
	dlg.m_pMiarex = this;
	dlg.InitDialog();

	bool bPoints = m_pCurWPolar->m_bShowPoints;
	m_pCurWPolar->m_bShowPoints = true;

	if(m_iView==WPOLARVIEW)     CreateWPolarCurves();
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();

	UpdateView();

	if(dlg.exec() == QDialog::Accepted)
	{
		pMainFrame->SetSaveState(false);
	}
	else
	{
		m_pCurWPolar->Copy(&MemWPolar);
	}
	m_pCurWPolar->m_bShowPoints = bPoints;

	if(m_iView==WPOLARVIEW)     CreateWPolarCurves();
	else if(m_iView==WSTABVIEW) CreateStabilityCurves();

//	pMainFrame->UpdateWOpps();
//	SetWOpp(true);
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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	CWPolar *pWPolar;
	CWOpp* pWOpp;
	m_WingDlg.m_bAcceptName= false;
	m_WingDlg.InitDialog(m_pCurWing);

	bool bHasResults = false;
	for (i=0; i< m_poaWPolar->size(); i++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(i);
		if (pWPolar->m_Alpha.size() && pWPolar->m_UFOName == m_pCurWing->m_WingName)
		{
			bHasResults = true;
			break;
		}
	}

	for (i=0; i< m_poaWOpp->size(); i++)
	{
		pWOpp = (CWOpp*)m_poaWOpp->at(i);
		if(pWOpp->m_WingName == m_pCurWing->m_WingName)
		{
			bHasResults = true;
			break;
		}
	}

	//save a copy just in case user changes his mind (users !...)
	CWing* pSaveWing= new CWing;
	pSaveWing->Duplicate(m_pCurWing);


	if(QDialog::Accepted == m_WingDlg.exec())
	{
		if(m_WingDlg.m_bChanged)
		{
			if(bHasResults)
			{
				ModDlg dlg;
				dlg.m_Question = tr("The modification will erase all results associated to this Wing.\nContinue ?");
				dlg.InitDialog();
				int Ans = dlg.exec();
				if (Ans == QDialog::Rejected)
				{
					//restore geometry for initial wing
					m_pCurWing->Duplicate(pSaveWing);
					delete pSaveWing; // clean up
					return;
				}
				else if(Ans==20)
				{
					//create new wing
					CWing* pNewWing= new CWing();
					pNewWing->Duplicate(m_pCurWing);
					//restore geometry for initial wing
					m_pCurWing->Duplicate(pSaveWing);
					delete pSaveWing; // clean up

					if(!SetModWing(pNewWing))
					{
						delete pNewWing;
					}
					else
					{
						m_pCurWing = AddWing(pNewWing);
					}
					SetUFO();
					pMainFrame->UpdateUFOs();
					UpdateView();
					return;
				}
			}

			//then modifications are automatically recorded
			pMainFrame->DeleteWing(m_pCurWing, true);// will also set new surface and Aerochord in WPolars
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
		SetUFO();
		pMainFrame->UpdateUFOs();
		m_bResetglStream = true;
		m_bIs2DScaleSet = false;
		SetScale();
		SetWGraphScale();
		OnAdjustToWing() ;
		UpdateView();
	}
	else
	{
		// restore original
		m_pCurWing->Duplicate(pSaveWing);
		m_pCurWing->CreateSurfaces(CVector(0.0,0.0,0.0), 0.0, 0.0);
		for (int i=0; i<m_pCurWing->m_NSurfaces; i++)
			m_pCurWing->m_Surface[i].SetSidePoints(NULL, 0.0, 0.0);

		UpdateView();
	}
	delete pSaveWing; // clean up
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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString filter =".csv";
	QString FileName, sep, str, strong, Format;
	int type = 1;

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
	if (pos>0) type = 2;

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
	strong = QString("QInf  ="+sep+" %1 "+sep).arg(m_pCurWOpp->m_QInf*pMainFrame->m_mstoUnit,9,'f',4);
	GetSpeedUnit(str, pMainFrame->m_SpeedUnit);
	strong+=str+"\n";
	out << strong;

	strong = QString("Alpha = "+sep+"%1\n").arg(m_pCurWOpp->m_Alpha,9,'f',4);
	out << strong;

	strong = QString("Beta  = "+sep+"%1").arg(m_pCurWOpp->m_Beta, 8,'f',3);
	strong += QString::fromUtf8("°\n");
	out << strong;

	strong = QString("Phi   = "+sep+"%1").arg(m_pCurWOpp->m_Phi, 8,'f',3);
	strong += QString::fromUtf8("°\n");
	out << strong;

	strong = QString("Ctrl  = "+sep+"%1\n").arg(m_pCurWOpp->m_Ctrl, 8,'f',3);
	out << strong;

	strong = QString("Cl    = "+sep+"%1\n").arg(m_pCurWOpp->m_CL,9,'f',4);
	out << strong;

	strong = QString("Cy    = "+sep+"%1\n").arg(m_pCurWOpp->m_CY,9,'f',4);
	out << strong;

	if(type==1) strong = QString(tr("Cd    = %1     ICd   = %2     PCd   = %3\n"))
		.arg(m_pCurWOpp->m_InducedDrag+m_pCurWOpp->m_ViscousDrag,9,'f',4)
		.arg(m_pCurWOpp->m_InducedDrag,9,'f',4)
		.arg(m_pCurWOpp->m_ViscousDrag,9,'f',4);
	else        strong = QString(tr("Cd=,%1,ICd=, %2,PCd=, %3\n"))
		.arg(m_pCurWOpp->m_InducedDrag+m_pCurWOpp->m_ViscousDrag,9,'f',4)
		.arg(m_pCurWOpp->m_InducedDrag,9,'f',4)
		.arg(m_pCurWOpp->m_ViscousDrag,9,'f',4);
	out << strong;

	strong = QString(tr("GCm   =")+sep+" %1\n").arg(m_pCurWOpp->m_GCm, 9,'g',4);
	out << strong;

	strong = QString(tr("GRm   = ")+sep+"%1\n").arg(m_pCurWOpp->m_GRm, 9,'g',4);
	out << strong;

	if(type==1) strong = QString(tr("IYm   = %1     PYm   = %2 \n")).arg(m_pCurWOpp->m_IYm, 9,'f',4).arg(m_pCurWOpp->m_GYm, 9,'f',4);
	else        strong = QString(tr("IYm=, %1,PYm=, %2\n")).arg(m_pCurWOpp->m_IYm, 9,'f',4).arg(m_pCurWOpp->m_GYm, 9,'f',4);
	out << strong;

	if(type==1) strong = QString(tr("XCP   = %1     YCP   = %2 \n")).arg(m_pCurWOpp->m_XCP, 9,'f',4).arg(m_pCurWOpp->m_YCP, 9,'f',4);
	else        strong = QString(tr("XCP=, %1, YCP=, %2 \n")).arg(m_pCurWOpp->m_XCP, 9,'f',4).arg(m_pCurWOpp->m_YCP, 9,'f',4);
	out << strong;

	strong = QString(tr("Bend. =")+sep+" %1\n\n").arg(m_pCurWOpp->m_MaxBending, 9,'f',4);
	out << strong;

	if(m_pCurWOpp->m_AnalysisType==4)
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
			

			strong = QString("CLa   =%1\n").arg(m_pCurWOpp->CLa, 10, 'f', 4);		out << strong;
			strong = QString("CLq   =%1\n").arg(m_pCurWOpp->CLq, 10, 'f', 4);		out << strong;
			strong = QString("Cma   =%1\n").arg(m_pCurWOpp->Cma, 10, 'f', 4);		out << strong;
			strong = QString("Cmq   =%1\n").arg(m_pCurWOpp->Cmq, 10, 'f', 4);		out << strong;
			strong = QString("CYb   =%1\n").arg(m_pCurWOpp->CYb, 10, 'f', 4);		out << strong;
			strong = QString("CYp   =%1\n").arg(m_pCurWOpp->CYp, 10, 'f', 4);		out << strong;
			strong = QString("CYr   =%1\n").arg(m_pCurWOpp->CYr, 10, 'f', 4);		out << strong;
			strong = QString("Clb   =%1\n").arg(m_pCurWOpp->Clb, 10, 'f', 4);		out << strong;
			strong = QString("Clp   =%1\n").arg(m_pCurWOpp->Clp, 10, 'f', 4);		out << strong;
			strong = QString("Clr   =%1\n").arg(m_pCurWOpp->Clr, 10, 'f', 4);		out << strong;
			strong = QString("Cnb   =%1\n").arg(m_pCurWOpp->Cnb, 10, 'f', 4);		out << strong;
			strong = QString("Cnp   =%1\n").arg(m_pCurWOpp->Cnp, 10, 'f', 4);		out << strong;
			strong = QString("Cnr   =%1\n").arg(m_pCurWOpp->Cnr, 10, 'f', 4);		out << strong;
		}
		else
		{
			strong = QString("CLa =,%1\n").arg(m_pCurWOpp->CLa, 10, 'f', 4);		out << strong;
			strong = QString("CLq =,%1\n").arg(m_pCurWOpp->CLq, 10, 'f', 4);		out << strong;
			strong = QString("Cma =,%1\n").arg(m_pCurWOpp->Cma, 10, 'f', 4);		out << strong;
			strong = QString("Cmq =,%1\n").arg(m_pCurWOpp->Cmq, 10, 'f', 4);		out << strong;
			strong = QString("CYb =,%1\n").arg(m_pCurWOpp->CYb, 10, 'f', 4);		out << strong;
			strong = QString("CYp =,%1\n").arg(m_pCurWOpp->CYp, 10, 'f', 4);		out << strong;
			strong = QString("CYr =,%1\n").arg(m_pCurWOpp->CYr, 10, 'f', 4);		out << strong;
			strong = QString("Clb =,%1\n").arg(m_pCurWOpp->Clb, 10, 'f', 4);		out << strong;
			strong = QString("Clp =,%1\n").arg(m_pCurWOpp->Clp, 10, 'f', 4);		out << strong;
			strong = QString("Clr =,%1\n").arg(m_pCurWOpp->Clr, 10, 'f', 4);		out << strong;
			strong = QString("Cnb =,%1\n").arg(m_pCurWOpp->Cnb, 10, 'f', 4);		out << strong;
			strong = QString("Cnp =,%1\n").arg(m_pCurWOpp->Cnp, 10, 'f', 4);		out << strong;
			strong = QString("Cnr =,%1\n").arg(m_pCurWOpp->Cnr, 10, 'f', 4);		out << strong;			
		}
		out << "\n\n";
	}
	
	
	out << tr("Main Wing Data\n");
	for (l=0; l<m_pCurWOpp->m_nFlaps; l++)
	{
		strong = QString(tr("Flap ")+sep+"%1"+sep+tr(" moment = ")+sep+"%2").arg(l+1).arg(m_pCurWOpp->m_FlapMoment[l]*pMainFrame->m_NmtoUnit, 9,'f',4);
		GetMomentUnit(str, pMainFrame->m_MomentUnit);
		strong += str +"\n";
		out << strong;
	}
	out << ("\n");
	m_pCurWOpp->Export(out, type);

	if(m_pCurWing2)
	{
		out << tr("Secondary Wing Data\n");
		for (l=0; l<m_pCurPOpp->m_Wing2WOpp.m_nFlaps; l++)
		{
			strong = QString(tr("Flap ")+sep+"%1"+sep+" moment = "+sep+"%2 ").arg(l+1,4).arg(m_pCurPOpp->m_Wing2WOpp.m_FlapMoment[l]*pMainFrame->m_NmtoUnit, 9,'f',4);
			GetMomentUnit(str, pMainFrame->m_MomentUnit);
			strong += str +"\n";
			out << strong;
		}
		out << ("\n");
		m_pCurPOpp->m_Wing2WOpp.Export(out, type);
	}

	if(m_pCurStab)
	{
		out << tr("Elevator Data\n");
		for (l=0; l<m_pCurPOpp->m_StabWOpp.m_nFlaps; l++)
		{
			strong = QString(tr("Flap ")+sep+"%1"+sep+" moment = "+sep+"%2 ").arg(l+1,4).arg(m_pCurPOpp->m_StabWOpp.m_FlapMoment[l]*pMainFrame->m_NmtoUnit, 9,'f',4);
			GetMomentUnit(str, pMainFrame->m_MomentUnit);
			strong += str +"\n";
			out << strong;
		}
		out << ("\n");
		m_pCurPOpp->m_StabWOpp.Export(out, type);
	}

	if(m_pCurFin)
	{
		out << tr("Fin Data\n");
		for (l=0; l<m_pCurPOpp->m_FinWOpp.m_nFlaps; l++)
		{
			strong = QString(tr("Flap ")+sep+"%1"+sep+" moment ="+sep+" %2").arg(l+1).arg(m_pCurPOpp->m_FinWOpp.m_FlapMoment[l]*pMainFrame->m_NmtoUnit, 9,'f',4);
			GetMomentUnit(str, pMainFrame->m_MomentUnit);
			strong += str +"\n";
			out << strong;
		}
		out << ("\n");
		m_pCurPOpp->m_FinWOpp.Export(out, type);
	}

	if(m_pCurWOpp->m_AnalysisType>=2)
	{
		if(m_pCurPOpp) out << tr("Main Wing Cp Coefficients\n");
		else           out << tr("Wing Cp Coefficients\n");
		coef = 1;

		if(m_pCurWOpp->m_AnalysisType==2 || m_pCurWOpp->m_AnalysisType==4)
		{
			if(type==1) out << tr(" Panel     CtrlPt.x        CtrlPt.y        CtrlPt.z             Cp\n");
			else        out << tr("Panel,CtrlPt.x,CtrlPt.y,CtrlPt.z,Cp\n");
		}
		else if(m_pCurWOpp->m_AnalysisType==3)
		{
			coef = 2;
			if(type==1) out << tr(" Panel     CollPt.x        CollPt.y        CollPt.z             Cp\n");
			else        out << tr("Panel,CollPt.x,CollPt.y,CollPt.z,Cp\n");
		}

		if(type==1) Format = "%1     %2     %3     %4     %5\n";
		else        Format = "%1,%2,%3,%4,%5\n";

		p=0;
		iStrip = 0;
		for (j=0; j<m_pCurWing->m_NSurfaces; j++)
		{
			if(m_pCurWing->m_Surface[j].m_bIsTipLeft) p+= m_pCurWing->m_Surface[j].m_NXPanels;

			for(k=0; k<m_pCurWing->m_Surface[j].m_NYPanels; k++)
			{
				iStrip++;
				strong = QString(tr("Strip %1\n")).arg(iStrip);
				out << strong;

				for(l=0; l<m_pCurWing->m_Surface[j].m_NXPanels * coef; l++)
				{
					if(m_pCurWOpp->m_AnalysisType==2 || m_pCurWOpp->m_AnalysisType==4)
					{
						strong = QString(Format).arg(p,4).arg(m_Panel[p].CtrlPt.x,11,'e',3).arg(m_Panel[p].CtrlPt.y,11,'e',3).arg(m_Panel[p].CtrlPt.z,11,'e',3).arg(m_pCurWOpp->m_Cp[p],11,'f',4);
					}
					else if(m_pCurWOpp->m_AnalysisType==3)
					{
						strong = QString(Format).arg(p,4).arg(m_Panel[p].CollPt.x,11,'e',3).arg(m_Panel[p].CollPt.y,11,'e',3).arg(m_Panel[p].CollPt.z,11,'e',3).arg(m_pCurWOpp->m_Cp[p],11,'f',4);
					}
					out << strong;
					p++;
				}
			}
		}
		out << ("\n\n");

		if(m_pCurWing2)
		{
			out << tr("Wing2 Cp Coefficients\n");
			p=0;
			iStrip = 0;
			for (j=0; j<m_pCurWing2->m_NSurfaces; j++)
			{
				if(m_pCurWing2->m_Surface[j].m_bIsTipLeft) p+= m_pCurWing2->m_Surface[j].m_NXPanels;

				for(k=0; k<m_pCurWing2->m_Surface[j].m_NYPanels; k++)
				{
					iStrip++;
					strong = QString(tr("Strip %1\n")).arg(iStrip);
					out << strong;

					for(l=0; l<m_pCurWing2->m_Surface[j].m_NXPanels * coef; l++)
					{
						if(m_pCurWOpp->m_AnalysisType==2 || m_pCurWOpp->m_AnalysisType==4)
						{
							strong = QString(Format).arg(p,4).arg(m_pCurWing2->m_pPanel[p].CtrlPt.x,11,'e',3).arg(m_pCurWing2->m_pPanel[p].CtrlPt.y,11,'e',3).arg(m_pCurWing2->m_pPanel[p].CtrlPt.z,11,'e',3).arg(m_pCurPOpp->m_Wing2WOpp.m_Cp[p],11,'f',4);
						}
						else if(m_pCurWOpp->m_AnalysisType==3)
						{
							strong = QString(Format).arg(p,4).arg(m_pCurWing2->m_pPanel[p].CollPt.x,11,'e',3).arg(m_pCurWing2->m_pPanel[p].CollPt.y,11,'e',3).arg(m_pCurWing2->m_pPanel[p].CollPt.z,11,'e',3).arg(m_pCurPOpp->m_Wing2WOpp.m_Cp[p],11,'f',4);
						}
						out << strong;
						p++;
					}
				}
			}
		}
		out << ("\n\n");

		if(m_pCurStab)
		{
			out << tr("Elevator Cp Coefficients\n");
			p=0;
			iStrip = 0;
			for (j=0; j<m_pCurStab->m_NSurfaces; j++)
			{
				if(m_pCurWing->m_Surface[j].m_bIsTipLeft) p+= m_pCurStab->m_Surface[j].m_NXPanels;

				for(k=0; k<m_pCurStab->m_Surface[j].m_NYPanels; k++)
				{
					iStrip++;
					strong = QString(tr("Strip %1\n")).arg(iStrip);
					out << strong;

					for(l=0; l<m_pCurStab->m_Surface[j].m_NXPanels * coef; l++)
					{
						if(m_pCurWOpp->m_AnalysisType==2 || m_pCurWOpp->m_AnalysisType==4)
						{
							strong = QString(Format).arg(p,4).arg(m_pCurStab->m_pPanel[p].CtrlPt.x,11,'e',3).arg(m_pCurStab->m_pPanel[p].CtrlPt.y,11,'e',3).arg(m_pCurStab->m_pPanel[p].CtrlPt.z,11,'e',3).arg(m_pCurPOpp->m_StabWOpp.m_Cp[p],11,'f',4);
						}
						else if(m_pCurWOpp->m_AnalysisType==3)
						{
							strong = QString(Format).arg(p,4).arg(m_pCurStab->m_pPanel[p].CollPt.x,11,'e',3).arg(m_pCurStab->m_pPanel[p].CollPt.y,11,'e',3).arg(m_pCurStab->m_pPanel[p].CollPt.z,11,'e',3).arg(m_pCurPOpp->m_StabWOpp.m_Cp[p],11,'f',4);
						}
						out << strong;
						p++;
					}
				}
			}
		}
		out << ("\n\n");
		if(m_pCurFin)
		{
			out << tr("Fin Cp Coefficients\n");
			p=0;
			iStrip = 0;
			for (j=0; j<m_pCurFin->m_NSurfaces; j++)
			{
				if(m_pCurFin->m_Surface[j].m_bIsTipLeft) p+= m_pCurFin->m_Surface[j].m_NXPanels;

				for(k=0; k<m_pCurFin->m_Surface[j].m_NYPanels; k++)
				{
					iStrip++;
					strong = QString(tr("Strip %1\n")).arg(iStrip);
					out << strong;

					for(l=0; l<m_pCurFin->m_Surface[j].m_NXPanels * coef; l++)
					{
						if(m_pCurWOpp->m_AnalysisType==2 || m_pCurWOpp->m_AnalysisType==4)
						{
							strong = QString(Format).arg(p,4).arg(m_pCurFin->m_pPanel[p].CtrlPt.x,11,'e',3).arg(m_pCurFin->m_pPanel[p].CtrlPt.y,11,'e',3).arg(m_pCurFin->m_pPanel[p].CtrlPt.z,11,'e',3).arg(m_pCurPOpp->m_FinWOpp.m_Cp[p],11,'f',4);
						}
						else if(m_pCurWOpp->m_AnalysisType==3)
						{
							strong = QString(Format).arg(p,4).arg(m_pCurFin->m_pPanel[p].CollPt.x,11,'e',3).arg(m_pCurFin->m_pPanel[p].CollPt.y,11,'e',3).arg(m_pCurFin->m_pPanel[p].CollPt.z,11,'e',3).arg(m_pCurPOpp->m_FinWOpp.m_Cp[p],11,'f',4);
						}
						out << strong;
						p++;
					}
				}
			}
		}
	}
	out << ("\n\n");

	XFile.close();

}



void QMiarex::OnExportCurWPolar()
{
	if (!m_pCurWPolar) return;
	QString filter =".csv";

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString FileName, DestFileName, OutString;
	QFile DestFile;
	int type = 1;

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
	if (pos>0) type = 2;

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);
	m_pCurWPolar->Export(out, type);
	XFile.close();

	UpdateView();
}



void QMiarex::OnExporttoAVL()
{
	if (!m_pCurWing) return;
	QString filter =".avl";

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString FileName, DestFileName, OutString, strong;
	QFile DestFile;

	if(m_pCurPlane) FileName = m_pCurPlane->m_PlaneName;
	else            FileName = m_pCurWing->m_WingName;
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
			 .arg(m_pCurWing->m_PlanformArea*pMainFrame->m_m2toUnit, 8, 'f', 3)
			 .arg(m_pCurWing->m_MAChord*pMainFrame->m_mtoUnit,       8, 'f', 3)
			 .arg(m_pCurWing->m_PlanformSpan*pMainFrame->m_mtoUnit,  8, 'f', 3);
	out << strong;

	strong = QString("%1  %2  %3          | Xref   Yref   Zref\n")
			 .arg(0.0,8,'f',3)
			 .arg(0.0,8,'f',3)
			 .arg(0.0,8,'f',3);
	out << strong;

	out << (" 0.00                        | CDp  (optional)\n");

	out << ("\n\n\n");

	int index = rand();

	if(m_pCurPlane) m_pCurWing->ExportAVLWing(out, index, 0.0, 0.0, 0.0, 0.0, m_pCurPlane->m_WingTilt);
	else            m_pCurWing->ExportAVLWing(out, index, 0.0, 0.0, 0.0, 0.0, 0.0);

	if(m_pCurWing2) m_pCurWing2->ExportAVLWing(out, index+1, 0.0, 0.0, 0.0, 0.0, m_pCurPlane->m_WingTilt2);
	if(m_pCurStab)  m_pCurStab->ExportAVLWing(out, index+2, 0.0, 0.0, 0.0, 0.0, m_pCurPlane->m_StabTilt);
	if(m_pCurFin)   m_pCurFin->ExportAVLWing(out, index+3, 0.0, m_pCurPlane->m_LEFin.y, 0.0, 0.0, m_pCurPlane->m_StabTilt);

	XFile.close();
}



void QMiarex::OnFinCurve()
{
	m_bShowFin = !m_bShowFin;
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


void QMiarex::OnFourWingGraphs()
{
	m_iWingView = 4;
	m_pCurGraph = &m_WingGraph1;
	m_pCurWingGraph = m_pCurGraph;

	if(m_iView!=WOPPVIEW) OnWOpps();
	else
	{
		SetWingLegendPos();
		UpdateView();
	}
	SetControls();
}



void QMiarex::OnFourWPlrGraphs()
{
	m_iWPlrView = 4;
	m_pCurGraph = &m_WPlrGraph1;
	m_pCurWPlrGraph = m_pCurGraph;

	if(m_iView!=WPOLARVIEW) OnWPolars();
	else
	{
		SetWPlrLegendPos();
		UpdateView();
	}
	SetControls();
}





void QMiarex::OnGL3DScale()
{
	if(m_iView != W3DVIEW)
	{
//		m_pctrl3DSettings->setChecked(false);
		return;
	}
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(pMainFrame->m_pctrl3DScalesWidget->isVisible())	pMainFrame->m_pctrl3DScalesWidget->hide();
	else                                               	pMainFrame->m_pctrl3DScalesWidget->show();

	pMainFrame->W3DScalesAct->setChecked(pMainFrame->m_pctrl3DScalesWidget->isVisible());
//	if(m_pctrl3DSettings->isChecked()) pMainFrame->m_pctrl3DScalesWidget->show();
//	else                               pMainFrame->m_pctrl3DScalesWidget->hide();
}


void QMiarex::OnGraphSettings()
{
	QGraph *pGraph = NULL;
	GraphDlg dlg;

	pGraph = m_pCurGraph;
	if(!pGraph) return;
	if(m_iView==WOPPVIEW)
	{
		dlg.m_iGraphType = 61;
	}
	else if(m_iView==WPOLARVIEW)
	{
		dlg.m_iGraphType = 62;
	}
	else if(m_iView==WCPVIEW) dlg.m_iGraphType = 64;

	if(!pGraph) return;

	QGraph graph;
	graph.CopySettings(pGraph);
	dlg.m_pMemGraph = &graph;
	dlg.m_pGraph = pGraph;
	dlg.SetParams();

	if(dlg.exec() == QDialog::Accepted)
	{
		if(m_iView==WOPPVIEW)
		{

			if(dlg.m_bVariableChanged)
			{
				m_pCurGraph->SetAutoY(true);
				m_pCurGraph->SetAutoYMinUnit(true);
			}
			CreateWOppCurves();
		}
		else if(m_iView==WPOLARVIEW)
		{
			if(pGraph == &m_WPlrGraph1)
			{
				SetWGraphTitles(&m_WPlrGraph1);
			}
			else if(pGraph == &m_WPlrGraph2)
			{
				SetWGraphTitles(&m_WPlrGraph2);
			}
			else if(pGraph == &m_WPlrGraph3)
			{
				SetWGraphTitles(&m_WPlrGraph3);
			}
			else if(pGraph == &m_WPlrGraph4)
			{
				SetWGraphTitles(&m_WPlrGraph4);
			}
			if(dlg.m_bVariableChanged)
			{
				m_pCurGraph->SetAuto(true);
				m_pCurGraph->SetAutoYMinUnit(true);
			}
			CreateWPolarCurves();
		}
		else if(m_iView==WSTABVIEW)
		{
			CreateStabilityCurves();
		}
	}
	else
	{
		pGraph->CopySettings(&graph);
	}
	UpdateView();
}




void QMiarex::OnHalfWing()
{
	m_bHalfWing = m_pctrlHalfWing->isChecked();
	if(m_iView==WOPPVIEW)
	{
		m_bIs2DScaleSet = false;
		SetWGraphScale();
		Set2DScale();
		OnAdjustToWing();
		UpdateView();
	}
}



void QMiarex::OnHideAllWPolars()
{
	int i;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	CWPolar *pWPolar;
	for (i=0; i<m_poaWPolar->size(); i++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(i);
		pWPolar->m_bIsVisible = false;
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
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
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
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
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

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->SetSaveState(false);
	SetCurveParams();
	UpdateView();
}


void QMiarex::OnHideUFOWPolars()
{
	if(!m_pCurWing) return;
	int i;
	QString UFOName;
	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	CWPolar *pWPolar;
	for (i=0; i<m_poaWPolar->size(); i++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(i);
		if (pWPolar->m_UFOName == UFOName) pWPolar->m_bIsVisible = false;
	}

	if(m_iView==WPOLARVIEW)		CreateWPolarCurves();
	else if(m_iView==WSTABVIEW)	CreateStabilityCurves();

	SetCurveParams();
	pMainFrame->SetSaveState(false);
	UpdateView();
}



void QMiarex::OnHighlightOpp()
{
	if(m_iView!=WPOLARVIEW) return;
	m_bHighlightOpp = m_pctrlHighlightOpp->isChecked();
	m_WPlrGraph1.m_bHighlightPoint = m_bHighlightOpp;
	m_WPlrGraph2.m_bHighlightPoint = m_bHighlightOpp;
	m_WPlrGraph3.m_bHighlightPoint = m_bHighlightOpp;
	m_WPlrGraph4.m_bHighlightPoint = m_bHighlightOpp;

	if(m_iView==WPOLARVIEW)		CreateWPolarCurves();
	else if(m_iView==WSTABVIEW)	CreateStabilityCurves();
	
	UpdateView();
}



void QMiarex::OnImportBody()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
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
		m_GL3dBody.setWindowState(Qt::WindowMaximized);

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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	pCurve = m_CpGraph.GetCurve(0);
	pNewCurve = m_CpGraph.AddCurve();
	pNewCurve->Copy(pCurve);

	m_CpColor = pMainFrame->m_crColors[(m_CpGraph.GetCurveCount()-1)%24];
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
	UpdateView();
}


void QMiarex::OnManageBodies()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	ManageBodiesDlg dlg;
	dlg.move(pMainFrame->m_DlgPos);
	dlg.m_pMiarex = this;
	dlg.m_pMainFrame = m_pMainFrame;
	dlg.m_pGL3dBodyDlg = &m_GL3dBody;
	dlg.m_poaBody = m_poaBody;
	dlg.m_poaPlane = m_poaPlane;
	dlg.InitDialog();
	dlg.exec();
	pMainFrame->m_DlgPos = dlg.pos();
}

void QMiarex::OnManageUFOs()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	ManageUFOsDlg dlg;
	dlg.m_pMainFrame = m_pMainFrame;
	dlg.m_pMiarex    = this;

	QString UFOName = "";
	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;

	dlg.InitDialog(UFOName);
	dlg.exec();

	if(dlg.m_pPlane)     SetUFO(dlg.m_pPlane->m_PlaneName);
	else if(dlg.m_pWing) SetUFO(dlg.m_pWing->m_WingName);
	else                 SetUFO();

	pMainFrame->UpdateUFOs();
	SetControls();
//	pMainFrame->SetSaveState(false);

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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	CBody *pBody = new CBody;

	m_GL3dBody.SetBody(pBody);
	m_GL3dBody.setWindowState(Qt::WindowMaximized);
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
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	CPlane* pPlane = new CPlane;
	PlaneDlg PDlg;
	PDlg.m_pPlane = pPlane;
	CPlane* pOldPlane = NULL;
	PDlg.m_bAcceptName = true;
	PDlg.InitDialog();

	pPlane->m_Wing.m_WingColor = m_WingColor;
	pPlane->m_Stab.m_WingColor = m_StabColor;
	pPlane->m_Fin.m_WingColor  = m_FinColor;

	if(QDialog::Accepted == PDlg.exec())
	{
		m_WingColor = pPlane->m_Wing.m_WingColor ;
		m_StabColor = pPlane->m_Stab.m_WingColor;
		m_FinColor  = pPlane->m_Fin.m_WingColor;

		pMainFrame->SetSaveState(false);

		bool bExists = false;
		for(i=0; i<m_poaPlane->size(); i++)
		{
			pOldPlane = (CPlane*)m_poaPlane->at(i);
			if(pPlane->m_PlaneName == pOldPlane->m_PlaneName)
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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	m_bSequence = m_pctrlSequence->isChecked();
	m_bInitLLTCalc = m_pctrlInitLLTCalc->isChecked();

	if(m_pCurWPolar->m_Type <4)
	{
		m_AlphaMin   = m_pctrlAlphaMin->GetValue();
		m_AlphaMax   = m_pctrlAlphaMax->GetValue();
		m_AlphaDelta = fabs(m_pctrlAlphaDelta->GetValue());
		if(fabs(m_AlphaDelta)<0.01)
		{
			m_AlphaDelta = 0.01;
			m_pctrlAlphaDelta->SetValue(0.01);
		}
	}
	else if(m_pCurWPolar->m_Type==4)
	{
		m_QInfMin   = m_pctrlAlphaMin->GetValue()         /pMainFrame->m_mstoUnit;
		m_QInfMax   = m_pctrlAlphaMax->GetValue()         /pMainFrame->m_mstoUnit;
		m_QInfDelta = fabs(m_pctrlAlphaDelta->GetValue()) /pMainFrame->m_mstoUnit;
		if(fabs(m_QInfDelta)<0.1)
		{
			m_QInfDelta = 1.0;
			m_pctrlAlphaDelta->SetValue(1.0);
		}
	}
	else if(m_pCurWPolar->m_Type==5 || m_pCurWPolar->m_Type==6 || m_pCurWPolar->m_Type==7)
	{
		m_ControlMin   = m_pctrlAlphaMin->GetValue()         /pMainFrame->m_mstoUnit;
		m_ControlMax   = m_pctrlAlphaMax->GetValue()         /pMainFrame->m_mstoUnit;
		m_ControlDelta = fabs(m_pctrlAlphaDelta->GetValue()) /pMainFrame->m_mstoUnit;
		if(fabs(m_ControlDelta)<0.001)
		{
			m_ControlDelta = 0.001;
			m_pctrlAlphaDelta->SetValue(0.001);
		}
	}
}


void QMiarex::OnPolarFilter()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	PolarFilterDlg dlg;
	dlg.move(pMainFrame->m_DlgPos);
	dlg.m_bMiarex = true;
	dlg.m_bType1 = m_bType1;
	dlg.m_bType2 = m_bType2;
	dlg.m_bType4 = m_bType4;
	dlg.m_bType5 = m_bType5;
	dlg.m_bType6 = m_bType6;
	dlg.m_bType7 = m_bType7;
	dlg.InitDialog();
	if(dlg.exec()==QDialog::Accepted)
	{
		m_bType1 = dlg.m_bType1;
		m_bType2 = dlg.m_bType2;
		m_bType4 = dlg.m_bType4;
		m_bType5 = dlg.m_bType5;
		m_bType6 = dlg.m_bType6;
		m_bType7 = dlg.m_bType7;
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
	pMainFrame->m_DlgPos = dlg.pos();
}


void QMiarex::OnRenameCurWPolar()
{
	//Rename the currently selected Wing Polar

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(!m_pCurWPolar) return;
	if(!m_pCurWing) return;
	int resp, k,l;
	CWPolar* pWPolar = NULL;
	CPOpp * pPOpp = NULL;
	CWOpp * pWOpp = NULL;
	QString OldName = m_pCurWPolar->m_PlrName;
	QString UFOName;

	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;

	QStringList NameList;
	for(k=0; k<m_poaWPolar->size(); k++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->at(k);
		if(pWPolar->m_UFOName==UFOName) NameList.append(pWPolar->m_PlrName);
	}
	RenameDlg dlg(this);
	dlg.move(pMainFrame->m_DlgPos);
	dlg.m_pstrArray = & NameList;

	dlg.m_strQuestion = tr("Enter the new name for the wing polar :");
	dlg.m_strName     = m_pCurWPolar->m_PlrName;
	dlg.InitDialog();

	bool bExists = true;

	while (bExists)
	{
		resp = dlg.exec();
		pMainFrame->m_DlgPos = dlg.pos();
		if(resp==QDialog::Accepted)
		{
			if (OldName == dlg.m_strName) return;
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaWPolar->size(); k++)
			{
				pWPolar = (CWPolar*)m_poaWPolar->at(k);
				if (pWPolar->m_PlrName == dlg.m_strName &&	pWPolar->m_UFOName == m_pCurWing->m_WingName)
				{
					bExists = true;
					break;
				}
			}
			if(!bExists)
			{
				m_pCurWPolar->m_PlrName = dlg.m_strName;
				if(m_pCurPlane)
				{
					for (l=m_poaPOpp->size()-1;l>=0; l--)
					{
						pPOpp = (CPOpp*)m_poaPOpp->at(l);
						if (pPOpp->m_PlrName == OldName && pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName)
						{
							pPOpp->m_PlrName = dlg.m_strName;
						}
					}
				}
				for (l=m_poaWOpp->size()-1;l>=0; l--)
				{
					pWOpp = (CWOpp*)m_poaWOpp->at(l);
					if (pWOpp->m_PlrName == OldName && pWOpp->m_WingName == m_pCurWing->m_WingName)
					{
						pWOpp->m_PlrName = dlg.m_strName;
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
						pWPolar->m_UFOName == m_pCurWing->m_WingName)
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
			if (OldName == dlg.m_strName) return;
			for (k=0; k<m_poaWPolar->size(); k++)
			{
				pWPolar = (CWPolar*)m_poaWPolar->at(k);
				if (pWPolar->m_PlrName == dlg.m_strName &&	pWPolar->m_UFOName == m_pCurWing->m_WingName)
				{
					bExists = true;
					break;
				}
			}
			for (l=m_poaWOpp->size()-1;l>=0; l--)
			{
				pWOpp = (CWOpp*)m_poaWOpp->at(l);
				if (pWOpp->m_PlrName == pWPolar->m_PlrName && pWOpp->m_WingName == m_pCurWing->m_WingName)
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
//				pMainFrame->m_WOperDlgBar.EnableAnalysis(false);
			}
			delete pWPolar;

			//and rename everything
			m_pCurWPolar->m_PlrName = dlg.m_strName;

			for (l=(int)m_poaWOpp->size()-1;l>=0; l--)
			{
				pWOpp = (CWOpp*)m_poaWOpp->at(l);
				if (pWOpp->m_PlrName == OldName &&	pWOpp->m_WingName == m_pCurWing->m_WingName)
				{
					pWOpp->m_PlrName = dlg.m_strName;
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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	if(!m_pCurWing)	return;
	if(m_pCurPlane)      RenameUFO(m_pCurPlane->m_PlaneName);
	else if (m_pCurWing) RenameUFO(m_pCurWing->m_WingName);
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
	QString strong = tr("Are you sure you want to reset the content of the polar :\n")+  m_pCurWPolar->m_PlrName +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(window(), tr("Question"), strong,
												  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
												  QMessageBox::Cancel)) return;

	m_pCurWPolar->ResetWPlr();

	if(m_iView==WPOLARVIEW)		CreateWPolarCurves();
	else if(m_iView==WSTABVIEW)	CreateStabilityCurves();
	else if(m_iView==WOPPVIEW)	CreateWOppCurves();
	else if(m_iView==WCPVIEW)	CreateCpCurves();

	UpdateView();

}


void QMiarex::OnResetWingGraphScale()
{
	//resets the scale of the current graph
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(!m_pCurGraph) return;
	m_pCurGraph->SetAuto(true);
	m_pCurGraph->ResetXLimits();
	m_pCurGraph->ResetYLimits();
	if (m_pCurGraph == &m_WingGraph1 ||
		m_pCurGraph == &m_WingGraph2 ||
		m_pCurGraph == &m_WingGraph3 ||
		m_pCurGraph == &m_WingGraph4)
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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	WingScaleDlg dlg;
	dlg.move(pMainFrame->m_DlgPos);
	dlg.m_pMainFrame = m_pMainFrame;
	dlg.InitDialog(m_pCurWing->m_PlanformSpan, m_pCurWing->m_TChord[0], m_pCurWing->GetAverageSweep(), m_pCurWing->m_TTwist[m_pCurWing->m_NPanel]);

	if(QDialog::Accepted == dlg.exec())
	{
		if (dlg.m_bSpan || dlg.m_bChord || dlg.m_bSweep || dlg.m_bTwist)
		{
			if(m_pCurPlane)
			{
				if(dlg.m_bSpan)  m_pCurWing->ScaleSpan(dlg.m_NewSpan);
				if(dlg.m_bChord) m_pCurWing->ScaleChord(dlg.m_NewChord);
				if(dlg.m_bSweep) m_pCurWing->SetSweep(dlg.m_NewSweep);
				if(dlg.m_bTwist) m_pCurWing->SetTwist(dlg.m_NewTwist);
			}
			else
			{
				CWing* pNewWing= new CWing;
				pNewWing->Duplicate(m_pCurWing);
				if(dlg.m_bSpan)  pNewWing->ScaleSpan(dlg.m_NewSpan);
				if(dlg.m_bChord) pNewWing->ScaleChord(dlg.m_NewChord);
				if(dlg.m_bSweep) pNewWing->SetSweep(dlg.m_NewSweep);
				if(dlg.m_bTwist) pNewWing->SetTwist(dlg.m_NewTwist);
				if(SetModWing(pNewWing))
				{
					m_pCurWing = AddWing(pNewWing);
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
	pMainFrame->m_DlgPos = dlg.pos();
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

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->SetSaveState(false);
	SetCurveParams();
	UpdateView();
}



void QMiarex::OnShowAllWOpps()
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
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
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
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
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
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
	//user has toggled visible switch
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	if(m_iView==WPOLARVIEW)
	{
		if (m_pCurWPolar)
		{
			m_pCurWPolar->m_bIsVisible = m_pctrlShowCurve->isChecked();
		}
		CreateWPolarCurves();
	}
	else if(m_iView==WSTABVIEW)
	{
		if (m_pCurWPolar)
		{
			m_pCurWPolar->m_bIsVisible = m_pctrlShowCurve->isChecked();
		}
		CreateStabilityCurves();
	}
	else if (m_pCurWOpp)
	{
		if(m_iView==WOPPVIEW)
		{
			m_pCurWOpp->m_bIsVisible = m_pctrlShowCurve->isChecked();
			if(m_pCurPOpp) m_pCurPOpp->m_bIsVisible = m_pctrlShowCurve->isChecked();
			CreateWOppCurves();
		}
		else if (m_iView==WCPVIEW)
		{
			m_bShowCp = m_pctrlShowCurve->isChecked();
			CreateCpCurves();
		}
	}
	pMainFrame->SetSaveState(false);
	UpdateView();
}


void QMiarex::OnShowPoints()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	if(m_iView ==WPOLARVIEW)
	{
		if (m_pCurWPolar)
		{
			m_pCurWPolar->m_bShowPoints = m_pctrlShowPoints->isChecked();
		}
		CreateWPolarCurves();
	}
	if(m_iView ==WSTABVIEW)
	{
		if (m_iStabilityView==1 && m_pCurWPolar)
		{
			m_pCurWPolar->m_bShowPoints = m_pctrlShowPoints->isChecked();
		}
		else if(m_iStabilityView==0 && m_pCurWOpp)
		{
			if(m_pCurPOpp) 
			{
				m_pCurPOpp->m_bShowPoints            = m_pctrlShowPoints->isChecked();
				m_pCurPOpp->m_WingWOpp.m_bShowPoints = m_pctrlShowPoints->isChecked();
			}
			else if(m_pCurWOpp)
			{
				m_pCurWOpp->m_bShowPoints = m_pctrlShowPoints->isChecked();
			}
		}
		CreateStabilityCurves();
	}
	else if (m_iView==WOPPVIEW && m_pCurWOpp)
	{
		m_pCurWOpp->m_bShowPoints = m_pctrlShowPoints->isChecked();
		if(m_pCurPOpp) m_pCurPOpp->m_bShowPoints = m_pctrlShowPoints->isChecked();
		CreateWOppCurves();
	}
	else if (m_iView==WCPVIEW && m_pCurWOpp)
	{
		m_bShowCpPoints = m_pctrlShowPoints->isChecked();
		CreateCpCurves();
	}

	pMainFrame->SetSaveState(false);
	UpdateView();
}



void QMiarex::OnShowUFOWPolars()
{
	if(!m_pCurWing) return;
	int i;
	QString UFOName;
	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
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




void QMiarex::OnSingleWPlrGraph1()
{
	m_iWPlrView = 1;
	m_pCurGraph = &m_WPlrGraph1;
	m_pCurWPlrGraph = m_pCurGraph;

	if(m_iView!=WPOLARVIEW) OnWPolars();
	else
	{
		SetWPlrLegendPos();
		UpdateView();
	}
	SetControls();
}


void QMiarex::OnSingleWPlrGraph2()
{
	m_iWPlrView = 1;
	m_pCurGraph = &m_WPlrGraph2;
	m_pCurWPlrGraph = m_pCurGraph;

	if(m_iView!=WPOLARVIEW) OnWPolars();
	else
	{
		SetWPlrLegendPos();
		UpdateView();
	}
	SetControls();
}


void QMiarex::OnSingleWPlrGraph3()
{
	m_iWPlrView = 1;
	m_pCurGraph = &m_WPlrGraph3;
	m_pCurWPlrGraph = m_pCurGraph;

	if(m_iView!=WPOLARVIEW) OnWPolars();
	else
	{
		SetWPlrLegendPos();
		UpdateView();
	}
	SetControls();
}


void QMiarex::OnSingleWPlrGraph4()
{
	m_iWPlrView = 1;
	m_pCurGraph = &m_WPlrGraph4;
	m_pCurWPlrGraph = m_pCurGraph;

	if(m_iView!=WPOLARVIEW) OnWPolars();
	else
	{
		SetWPlrLegendPos();
		UpdateView();
	}
	SetControls();
}



void QMiarex::OnSingleWingGraph1()
{
	m_iWingView = 1;
	m_pCurGraph = &m_WingGraph1;
	m_pCurWingGraph = m_pCurGraph;

	if(m_iView!=WOPPVIEW) OnWOpps();
	else
	{
		SetWingLegendPos();
		UpdateView();
	}
	SetControls();
}


void QMiarex::OnSingleWingGraph2()
{
	m_iWingView = 1;
	m_pCurGraph = &m_WingGraph2;
	m_pCurWingGraph = m_pCurGraph;

	if(m_iView!=WOPPVIEW) OnWOpps();
	else
	{
		SetWingLegendPos();
		UpdateView();
	}
	SetControls();
}


void QMiarex::OnSingleWingGraph3()
{
	m_iWingView = 1;
	m_pCurGraph = &m_WingGraph3;
	m_pCurWingGraph = m_pCurGraph;

	if(m_iView!=WOPPVIEW) OnWOpps();
	else
	{
		SetWingLegendPos();
		UpdateView();
	}
	SetControls();

}


void QMiarex::OnSingleWingGraph4()
{
	m_iWingView = 1;
	m_pCurGraph = &m_WingGraph4;
	m_pCurWingGraph = m_pCurGraph;

	if(m_iView!=WOPPVIEW) OnWOpps();
	else
	{
		SetWingLegendPos();
		UpdateView();
	}
	SetControls();

}


void QMiarex::OnStabCurve()
{
	m_bShowStab = !m_bShowStab;
//	CheckMenus();

	if (m_iView==WOPPVIEW)    CreateWOppCurves();
	else if(m_iView==WCPVIEW) CreateCpCurves();

	UpdateView();
}




void QMiarex::OnStabilityDirection()
{
	//the user has clicked either the longitudinal or lateral mode display
	//so update the view accordingly
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;

	m_bLongitudinal = m_pctrlLongDynamics->isChecked();
	if(m_bLongitudinal)
	{
		m_TimeGraph1.SetYTitle("u");
		m_TimeGraph2.SetYTitle("w");
		m_TimeGraph3.SetYTitle("q");
		m_TimeGraph4.SetYTitle("theta");
	}
	else
	{
		m_TimeGraph1.SetYTitle("v");
		m_TimeGraph2.SetYTitle("p");
		m_TimeGraph3.SetYTitle("r");
		m_TimeGraph4.SetYTitle("phi");
	}
	
	m_TimeGraph1.ResetCurves();
	m_TimeGraph2.ResetCurves();
	m_TimeGraph3.ResetCurves();
	m_TimeGraph4.ResetCurves();

	if(m_bLongitudinal) m_pCurRLStabGraph = &m_LongRLGraph;
	else                m_pCurRLStabGraph = &m_LatRLGraph;
	if(m_iStabilityView==1)
	{
		m_pCurGraph = m_pCurRLStabGraph;
	}

	pStabView->SetMode();
	pStabView->SetControls();
	SetWPlrLegendPos();
	CreateStabilityCurves();
	SetCurveParams();

	SetControls();
	UpdateView();
}




void QMiarex::OnStabilityGraph()
{
	//the user has clicked either the time response or the root locus graph
	//so update the view accordingly
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
//	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
//	pStabView->SetControls();
	if(m_pctrlTimeView->isChecked())
	{
		m_pCurGraph = &m_TimeGraph1;
		m_iStabilityView = 0;
		CreateStabilityCurves();
	}
	else if(m_pctrlRLView->isChecked())
	{
		m_iStabilityView = 1;
		CreateStabilityCurves();
	}
	SetCurveParams();
	UpdateView();
}



void QMiarex::OnStabilityView()
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	if (m_bAnimateWOpp) StopAnimate();

	if(m_iView==WSTABVIEW)
	{
		SetControls();
		UpdateView();
		return;
	}

	m_iView=WSTABVIEW;
	m_pCurGraph = NULL;

	pMainFrame->SetCentralWidget();

	if(m_iStabilityView==0)      SetWingLegendPos();
	else if(m_iStabilityView==1) SetWPlrLegendPos();
	CreateStabilityCurves();
	SetCurveParams();
	SetControls();

	UpdateView();
}


void QMiarex::OnStabilityViewType()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;

	if(m_pctrlTimeView->isChecked())
	{
		m_iStabilityView = 0;
		pStabView->SetControls();
		SetWingLegendPos();
		CreateStabilityCurves();
	}
	else if(m_pctrlRLView->isChecked())
	{
		m_iStabilityView = 1;
		SetWPlrLegendPos();
		CreateStabilityCurves();
	}
	else if(m_pctrl3DView->isChecked())
	{
		m_iStabilityView = 3;
	}

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
	m_GLLightDlg.m_pMiarex = this;
	m_GLLightDlg.exec();

	m_bShowLight = false;

	GLSetupLight();
	UpdateView();
}



void QMiarex::OnStoreWOpp()
{
	m_bStoreWOpp = m_pctrlStoreWOpp->isChecked();
}


void QMiarex::OnTimeGraph()
{
	m_iStabilityView = 0;
	m_pCurGraph = &m_TimeGraph1;
	CreateStabilityCurves();

	if(m_iView!=WSTABVIEW) OnStabilityView();
	else
	{
		SetWPlrLegendPos();
		UpdateView();
	}
	SetControls();
}



void QMiarex::OnTwoWingGraphs()
{
	m_iWingView = 2;
	m_pCurGraph = &m_WingGraph1;
	m_pCurWingGraph = m_pCurGraph;

	if(m_iView!=WOPPVIEW) OnWOpps();
	else
	{
		SetWingLegendPos();
		UpdateView();
	}
	SetControls();
}



void QMiarex::OnTwoWPlrGraphs()
{
	m_iWPlrView = 2;
	m_pCurGraph = &m_WPlrGraph1;
	m_pCurWPlrGraph = m_pCurGraph;

	if(m_iView!=WPOLARVIEW) OnWPolars();
	else
	{
		SetWPlrLegendPos();
		UpdateView();
	}
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
	m_bShowWing2 = !m_bShowWing2;
//	CheckMenus();

	if (m_iView==WOPPVIEW)    CreateWOppCurves();
	else if(m_iView==WCPVIEW) CreateCpCurves();

	UpdateView();
}



void QMiarex::OnUFOInertia()
{
	if(!m_pCurWing) return;
	InertiaDlg dlg;
	dlg.m_pMainFrame = m_pMainFrame;
	dlg.m_pPlane = NULL;
	dlg.m_pWing  = NULL;
	dlg.m_pBody  = NULL;
	if(m_pCurPlane)     dlg.m_pPlane = m_pCurPlane;
	else if(m_pCurWing) dlg.m_pWing  = m_pCurWing;
	dlg.InitDialog();
	dlg.exec();
}



void QMiarex::OnWOpps()
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	if(m_iView==WOPPVIEW)
	{
		SetControls();
		UpdateView();
		return;
	}

	m_pCurGraph = NULL;
	m_iView=WOPPVIEW;

	pMainFrame->SetCentralWidget();
//	m_pctrlMiddleControls->setCurrentIndex(0);
//	m_pctrBottomControls->setCurrentIndex(0);

	m_bIs2DScaleSet = false;
	Set2DScale();
	CreateWOppCurves();
	SetCurveParams();
	SetControls();

	UpdateView();
}


void QMiarex::OnWPolars()
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	if (m_bAnimateWOpp) StopAnimate();

	if(m_iView==WPOLARVIEW)
	{
		SetControls();
		UpdateView();
		return;
	}

	m_iView=WPOLARVIEW;
	if(!m_pCurWPlrGraph)	m_pCurGraph = NULL;
	else                    m_pCurGraph = m_pCurWPlrGraph;

	pMainFrame->SetCentralWidget();
//	m_pctrlMiddleControls->setCurrentIndex(0);
//	m_pctrBottomControls->setCurrentIndex(0);

	SetWPlrLegendPos();
	CreateWPolarCurves();
	SetCurveParams();
	SetControls();

	UpdateView();
}



void QMiarex::PaintView(QPainter &painter)
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	static int h,w,w2,h2, h23,h34, h38,w3,w23;
	static QRect Rect1, Rect2, Rect3, Rect4;
	static QPoint Place;
	QPen TextPen;
	QString GraphName;
	painter.save();
	w   = m_rCltRect.width();
	w2  = (int)(w/2);
	w3  = (int)(0.35*w);
	w23 = 2*w3;
	h   = m_rCltRect.height();
	h2  = (int)(h/2);
	h23 = (int)(2*h/3);
	h34 = (int)(3*h/4);
	h38 = (int)(3*h/8);
	//Refresh the active view
	painter.fillRect(m_rCltRect, pMainFrame->m_BackgroundColor);

	if(m_rCltRect.width()<200 || m_rCltRect.height()<200) 
	{
		painter.restore();
		return;//too small to paint
	}

	if (m_iView==WPOLARVIEW)
	{
		if (m_iWPlrView == 1)
		{
//			PaintWSingleGraph(painter);
			if(!m_pCurGraph)
			{
				m_pCurGraph     = &m_WPlrGraph1;
				m_pCurWPlrGraph = &m_WPlrGraph1;
			}
				
			Rect1.setRect(0,0,w23,m_rCltRect.bottom()-00);
			DrawWPolarLegend(painter, m_WPlrLegendOffset, Rect1.bottom());
			m_pCurGraph->DrawGraph(Rect1, painter);
		}
		else if(m_iWPlrView == 2)
		{
//			PaintWCoupleGraphs(painter);
			Rect1.setRect(0,0,w2,h23);
			Rect2.setRect(w2,0,w2,h23);
	
			m_WPlrGraph1.DrawGraph(Rect1, painter);
			m_WPlrGraph2.DrawGraph(Rect2, painter);
	
			DrawWPolarLegend(painter, m_WPlrLegendOffset, m_rCltRect.bottom());
		}
		else if(m_iWPlrView == 4)
		{
//			PaintWFourGraphs(painter);
			Rect1.setRect(0,0,w3,h2);
			Rect2.setRect(w3,0,w3,h2);
			Rect3.setRect(0,h2,w3,h2);
			Rect4.setRect(w3,h2,w3,h2);
	
			m_WPlrGraph1.DrawGraph(Rect1, painter);
			m_WPlrGraph2.DrawGraph(Rect2, painter);
			m_WPlrGraph3.DrawGraph(Rect3, painter);
			m_WPlrGraph4.DrawGraph(Rect4, painter);
	
			DrawWPolarLegend(painter, m_WPlrLegendOffset, m_rCltRect.bottom());	
		}
	}
	else if (m_iView==WOPPVIEW)
	{
		if (m_iWingView == 1 && m_pCurWing)  
		{
//			PaintSingleWingGraph(painter);
			if (m_pCurWingGraph && m_pCurWing && m_rCltRect.width()/2>200 && m_rCltRect.height()>250)
			{

				m_pCurWingGraph->DrawGraph(m_rSingleRect, painter);
				QPoint Place(m_rCltRect.left()+10, m_rCltRect.top() +30);
				DrawWOppLegend(painter, Place, m_rCltRect.bottom());
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
					PtLegend.rx() = (int)(m_rCltRect.width()/2);
					PtLegend.ry() = m_rCltRect.bottom();
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
			DrawWOppLegend(painter, m_WingLegendOffset, m_rCltRect.bottom());
	
			Rect1.setRect(m_rCltRect.left(),   m_rCltRect.top(), w2, h23);
			Rect2.setRect(w2, m_rCltRect.top(),                  w2, h23);
			m_WingGraph1.DrawGraph(Rect1, painter);
			m_WingGraph2.DrawGraph(Rect2, painter);
		}
		else if (m_iWingView == 4 && m_pCurWing) 
		{
//			PaintFourWingGraph(painter);
			DrawWOppLegend(painter, m_WingLegendOffset, m_rCltRect.bottom());
		
			Rect1.setRect(0,0,w2,h38);
			Rect2.setRect(w2,0,w2,h38);
			Rect3.setRect(0,h38,w2,h38);
			Rect4.setRect(w2,h38,w2,h38);
			m_WingGraph1.DrawGraph(Rect1, painter);
			m_WingGraph2.DrawGraph(Rect2, painter);
			m_WingGraph3.DrawGraph(Rect3, painter);
			m_WingGraph4.DrawGraph(Rect4, painter);
		}
	}
	else if (m_iView==WCPVIEW)
	{
//		PaintCp(painter);
		m_CpGraph.DrawGraph(painter);
		QPoint Place(50, h34+20);
		DrawCpLegend(painter, Place, m_rCltRect.bottom());
	}
	else if (m_iView==WSTABVIEW)
	{
		if(m_iStabilityView==0)
		{
			if(m_iStabTimeView==1) 
			{
//				PaintSingleTimeGraph(painter);
				if(!m_pCurTimeGraph) m_pCurTimeGraph = &m_TimeGraph1;
		
				Rect1.setRect(0,0,w,h34);
				m_pCurTimeGraph->DrawGraph(Rect1, painter);
				Place.setX((int)(w*2/5));
				Place.setY(m_pCurTimeGraph->GetMargin()/2);
				m_pCurTimeGraph->GetGraphName(GraphName);
				TextPen.setColor(m_pCurTimeGraph->GetTitleColor());
				painter.setPen(TextPen);
				painter.drawText(Place, GraphName);

				DrawStabTimeLegend(painter, m_WingLegendOffset, m_rCltRect.bottom());
				painter.restore();
			}
			else if(m_iStabTimeView==4)
			{
//				PaintFourTimeGraph(painter);
				//Paint a four graph time view			
				DrawStabTimeLegend(painter, m_WingLegendOffset, m_rCltRect.bottom());
				Rect1.setRect(0,0,w2,h38);
				Rect2.setRect(w2,0,w2,h38);
				Rect3.setRect(0,h38,w2,h38);
				Rect4.setRect(w2,h38,w2,h38);
				m_TimeGraph1.DrawGraph(Rect1, painter);
				m_TimeGraph2.DrawGraph(Rect2, painter);
				m_TimeGraph3.DrawGraph(Rect3, painter);
				m_TimeGraph4.DrawGraph(Rect4, painter);
			
			}
		}
		else 
		{
//			PaintRLStabGraphs(painter);
			if(m_iStabilityView==1)
			{
				if(m_pCurRLStabGraph)
				{
					Rect1.setRect(0,0,w23,m_rCltRect.bottom()-00);
					m_pCurRLStabGraph->DrawGraph(Rect1, painter);
					Place.setX((int)(w23/3));
					Place.setY(m_pCurRLStabGraph->GetMargin()/2);
					m_pCurRLStabGraph->GetGraphName(GraphName);
					TextPen.setColor(m_pCurRLStabGraph->GetTitleColor());
					painter.setPen(TextPen);
					painter.drawText(Place, GraphName);
					Place.setX(w23+10);
					Place.setY(10);
					DrawWPolarLegend(painter, m_WPlrLegendOffset, m_rCltRect.bottom());
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
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

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

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
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
	int ZPos    = m_rCltRect.height()-11*dheight;

//	double area = m_pCurWing->s_RefArea;
	if(m_pCurPlane && m_pCurStab) ZPos -= dheight;
	if(m_pCurWPolar)              ZPos -= dheight*2;

	painter.drawText(LeftPos, ZPos, m_pCurWing->m_WingName);
	D+=dheight;
	QString length, surface;
	GetLengthUnit(length, pMainFrame->m_LengthUnit);
	GetAreaUnit(surface,  pMainFrame->m_AreaUnit);

	str1 = QString(tr("Wing Span =   %1 ")).arg(m_pCurWing->m_PlanformSpan*pMainFrame->m_mtoUnit,12,'f',3);
	str1 += length;
	painter.drawText(LeftPos,ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("xyProj. Span = %1 ")).arg(m_pCurWing->m_ProjectedSpan*pMainFrame->m_mtoUnit,11,'f',3);
	str1 += length;
	painter.drawText(LeftPos,ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("Wing Area =  %1 ")).arg(m_pCurWing->m_PlanformArea * pMainFrame->m_m2toUnit,13,'f',3);
	str1 += surface;
	painter.drawText(LeftPos,ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("xyProj. Area = %1 ")).arg(m_pCurWing->m_ProjectedArea * pMainFrame->m_m2toUnit,11,'f',3);
	str1 += surface;
	painter.drawText(LeftPos,ZPos+D, str1);
	D+=dheight;

	if(m_pCurWPolar)
	{
		if(m_pCurWPolar->m_AnalysisType!=4)  Mass = m_pCurWPolar->m_Weight;
		else
		{
			if(m_pCurPlane)
			{
				Mass = m_pCurPlane->GetTotalMass();
			}
			else if(m_pCurWing)
			{
				Mass = m_pCurWing->GetTotalMass();				
			}
		}
		GetWeightUnit(str, pMainFrame->m_WeightUnit);
		Result = QString(tr("Plane Mass =    %1 ")).arg(Mass*pMainFrame->m_kgtoUnit,10,'f',3);
		Result += str;
		painter.drawText(LeftPos, ZPos+D, Result);
		D+=dheight;

		GetAreaUnit(strong, pMainFrame->m_AreaUnit);
		Result = QString(tr("Wing Load =  %1 ")).arg(Mass*pMainFrame->m_kgtoUnit/m_pCurWPolar->m_WArea/pMainFrame->m_m2toUnit,13,'f',3);
		Result += str + "/" + strong;
		painter.drawText(LeftPos, ZPos+D, Result);
		D+=dheight;
	}

	if(m_pCurPlane && m_pCurStab)
	{
		str1 = QString(tr("Tail Volume =      %1")).arg(m_pCurPlane->m_TailVolume,7,'f',3);
		painter.drawText(LeftPos, ZPos+D, str1);
		D+=dheight;
	}

	str1 = QString(tr("Root Chord =   %1 ")).arg(m_pCurWing->m_TChord[0]*pMainFrame->m_mtoUnit, 11,'f', 3);
	Result = str1+length;
	painter.drawText(LeftPos, ZPos+D, Result);
	D+=dheight;

	str1 = QString(tr("MAC =          %1 ")).arg(m_pCurWing->m_MAChord*pMainFrame->m_mtoUnit, 11,'f', 3);
	Result = str1+length;
	painter.drawText(LeftPos, ZPos+D, Result);
	D+=dheight;

	str1 = QString(tr("TipTwist =   %1 deg")).arg(m_pCurWing->m_TTwist[m_pCurWing->m_NPanel], 13,'f', 3);
	painter.drawText(LeftPos, ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("Aspect Ratio =   %1")).arg(m_pCurWing->m_AR,9,'f',3);
	painter.drawText(LeftPos, ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("Taper Ratio =   %1")).arg(m_pCurWing->m_TR,10,'f',3);
	painter.drawText(LeftPos, ZPos+D, str1);
	D+=dheight;

	str1 = QString(tr("Root-Tip Sweep = %1 deg")).arg(m_pCurWing->GetAverageSweep(), 9,'f',3);
	painter.drawText(LeftPos, ZPos+D, str1);

	painter.restore();
}


void QMiarex::PaintCurWOppLegend(QPainter &painter)
{
	if(!m_pCurWOpp) return;
	painter.save();
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString Result, str;

	int i;
	int margin = 10;
	int dwidth, dheight;
	
	QFontMetrics fm(pMainFrame->m_TextFont);
	dheight = fm.height();
	dwidth = fm.width(tr("abcdefghijklmnopqrstuvwxyz012345678"));
	int D = 0;
	int ZPos    = m_rCltRect.height()-11*dheight;

	D = 0;
	int RightPos = m_rCltRect.right()-margin-fm.width(tr("abcdefghijklmnopqrstuvwxyz01234567"));
	ZPos	 = m_rCltRect.height()-11*dheight;
	if(m_pCurWOpp && m_pCurWOpp->m_bOut) ZPos -= dheight;
	if(m_pCurWOpp) ZPos -= dheight*m_pCurWOpp->m_nFlaps;

	if(m_pCurWOpp && m_pCurWOpp->m_bIsVisible)
	{
		GetSpeedUnit(str, pMainFrame->m_SpeedUnit);
		int l = str.length();
		if(l==2)      Result = QString(tr("QInf = %1 ")).arg(m_pCurWOpp->m_QInf*pMainFrame->m_mstoUnit,7,'f',2);
		else if(l==3) Result = QString(tr("QInf = %1 ")).arg(m_pCurWOpp->m_QInf*pMainFrame->m_mstoUnit,6,'f',1);
		else if(l==4) Result = QString(tr("QInf = %1 ")).arg(m_pCurWOpp->m_QInf*pMainFrame->m_mstoUnit,5,'f',1);
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


		Result = QString(tr("Drag Coef. = %1 ")).arg(m_pCurWOpp->m_ViscousDrag+m_pCurWOpp->m_InducedDrag, 9,'f',4);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		/*		oswald=CZ^2/CXi/PI/allongement;*/
		double cxielli=m_pCurWOpp->m_CL*m_pCurWOpp->m_CL/PI/m_pCurWing->m_AR;
		Result = QString(tr("Efficiency = %1 ")).arg(cxielli/m_pCurWOpp->m_InducedDrag, 9,'f',4);
		D+=dheight;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString(tr("Cl/Cd = %1 ")).arg(m_pCurWOpp->m_CL/(m_pCurWOpp->m_InducedDrag+m_pCurWOpp->m_ViscousDrag), 9,'f',4);
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
		if (l==1)     Result = QString(tr("XCP = %1 ")).arg(m_pCurWOpp->m_XCP*pMainFrame->m_mtoUnit,8,'f',3);
		else if(l==2) Result = QString(tr("XCP = %1 ")).arg(m_pCurWOpp->m_XCP*pMainFrame->m_mtoUnit,7,'f',2);
		else if(l>=3) Result = QString(tr("XCP = %1 ")).arg(m_pCurWOpp->m_XCP*pMainFrame->m_mtoUnit,7,'f',2);
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
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	double y;
	int m,nStart;
	if(m_pCurWOpp->m_AnalysisType==1) nStart = 1; else nStart = 0;
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
		offLE = m_pCurWing->GetOffset(m_pCurWOpp->m_SpanPos[nStart]*2.0/m_pCurWOpp->m_Span);
		y = (offLE+m_pCurWOpp->m_Chord[nStart]*m_pCurWOpp->m_XTrTop[nStart])*scaley;
		From = QPoint(O.x()+(int)(m_pCurWOpp->m_SpanPos[nStart]*scalex),	O.y()+(int)y);

		for (m=nStart; m<m_pCurWOpp->m_NStation; m++)
		{
			offLE = m_pCurWing->GetOffset(m_pCurWOpp->m_SpanPos[m]*2.0/m_pCurWOpp->m_Span);
			y = (offLE+m_pCurWOpp->m_Chord[m]*m_pCurWOpp->m_XTrTop[m])*scaley;

			To = QPoint(O.x()+(int)(m_pCurWOpp->m_SpanPos[m]*scalex), O.y()+(int)y );
			painter.drawLine(From, To);
			From  = To;
		}


		int x = (int)(m_rCltRect.width()/2);
		int y = m_rCltRect.bottom();
		painter.drawLine(x-60,  y - 50, x-40,  y - 50);
		painter.drawText(x-35, y - 48, tr("Top transition"));

	}


	QPen BotPen(m_XBotColor);
	BotPen.setStyle(GetStyle(m_XBotStyle));
	BotPen.setWidth(m_XBotWidth);

	painter.setPen(BotPen);
	if (m_bXBot)
	{
		offLE = m_pCurWing->GetOffset(m_pCurWOpp->m_SpanPos[nStart]*2.0/m_pCurWOpp->m_Span);
		y = (offLE+m_pCurWOpp->m_Chord[nStart]*m_pCurWOpp->m_XTrBot[nStart])*scaley;
		From = QPoint(O.x() +(int)(m_pCurWOpp->m_SpanPos[nStart]*scalex), O.y()+(int)y );
		for (m=nStart; m<m_pCurWOpp->m_NStation; m++)
		{
			offLE = m_pCurWing->GetOffset(m_pCurWOpp->m_SpanPos[m]*2.0/m_pCurWOpp->m_Span);
			y = (offLE+m_pCurWOpp->m_Chord[m]*m_pCurWOpp->m_XTrBot[m])*scaley;
			To = QPoint(O.x()+(int)(m_pCurWOpp->m_SpanPos[m]*scalex), O.y()+(int)y );
			painter.drawLine(From, To);
			From  = To;
		}


		int x = (int)(m_rCltRect.width()/2);
		int y = m_rCltRect.bottom();
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
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QPoint From, To;

	double y;
	int nStart;
	double offLE;
	if(m_pCurWOpp->m_AnalysisType==1) nStart = 1; else nStart = 0;
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

	offLE = m_pCurWing->GetOffset(m_pCurWOpp->m_SpanPos[nStart]*2.0/m_pCurWOpp->m_Span);
	y = (offLE+m_pCurWOpp->m_Chord[nStart]*m_pCurWOpp->m_XCPSpanRel[nStart])*scaley;
	From = QPoint(O.x()+(int)(m_pCurWOpp->m_SpanPos[nStart]*scalex),	O.y()+(int)y );

	for (int m=nStart; m<m_pCurWOpp->m_NStation; m++)
	{
		offLE = m_pCurWing->GetOffset(m_pCurWOpp->m_SpanPos[m]*2.0/m_pCurWOpp->m_Span);
		y = (offLE+m_pCurWOpp->m_Chord[m]*m_pCurWOpp->m_XCPSpanRel[m])*scaley;
		To = QPoint(O.x()+(int)(m_pCurWOpp->m_SpanPos[m]*scalex),	O.y()+(int)y );
		painter.drawLine(From, To);
		From = To;
	}

	int x = (int)(m_rCltRect.width()/2);
	int y1 = m_rCltRect.bottom();
	painter.drawLine(x-60,  y1- 20, x-40,  y1 - 20);
	painter.drawText(x-35, y1 - 18, tr("Centre of Pressure"));
	painter.restore();
}

void QMiarex::PaintXCmRef(QPainter & painter, QPoint ORef, double scale)
{
	//Draws the moment reference point on the 2D view
	if(!m_pCurWing || !m_pCurWPolar)	return;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
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

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	int i,pl, pr;

	m_pPanelDlg->m_ppBody        = & m_pCurBody;

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

	if(m_pCurWPolar->m_Type!=4)
	{
		m_pPanelDlg->m_QInf       = m_pCurWPolar->m_QInf;
		m_pPanelDlg->m_Alpha      = V0;
		m_pPanelDlg->m_AlphaMax   = VMax;
		m_pPanelDlg->m_AlphaDelta = VDelta;
	}
	else
	{
		m_pPanelDlg->m_Alpha      = m_pCurWPolar->m_ASpec;
		m_pPanelDlg->m_QInf       = V0;
		m_pPanelDlg->m_QInfMax    = VMax;
		m_pPanelDlg->m_QInfDelta  = VDelta;
	}

	m_pPanelDlg->m_pWing   = m_pCurWing;
	m_pPanelDlg->m_pWing2  = m_pCurWing2;
	m_pPanelDlg->m_pStab   = m_pCurStab;
	m_pPanelDlg->m_pFin    = m_pCurFin;
	m_pPanelDlg->m_pPlane  = m_pCurPlane;
	m_pPanelDlg->m_MatSize = m_MatSize;
	m_pPanelDlg->m_NWakeColumn  = m_NWakeColumn;
	m_pPanelDlg->m_bDirichlet   = m_bDirichlet;


	m_pPanelDlg->m_AlphaMin   = V0;
	m_pPanelDlg->m_AlphaMax   = VMax;
	m_pPanelDlg->m_AlphaDelta = VDelta;

	m_pPanelDlg->m_nNodes     = m_nNodes;
	m_pPanelDlg->m_NSurfaces  = m_NSurfaces;

	m_pPanelDlg->move(pMainFrame->m_DlgPos);
	m_pPanelDlg->InitDialog();
	m_pPanelDlg->show();
	m_pPanelDlg->StartAnalysis();

//	if(m_bLogFile && m_pPanelDlg->m_bWarning) pMainFrame->OnLogFile();
	if(!m_bLogFile || !m_pPanelDlg->m_bWarning) m_pPanelDlg->hide();

	pMainFrame->m_DlgPos = m_pPanelDlg->pos();
	
	pMainFrame->UpdateWOpps();

	if(m_pCurPlane)     SetPOpp(false, V0);
	else if(m_pCurWing) SetWOpp(false, V0);

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
		OldName = pPlane->m_PlaneName;
		SetModPlane(pPlane);

		pPlane->m_Wing.m_WingName  = pPlane->m_PlaneName+tr("_Wing");
		pPlane->m_Wing2.m_WingName = pPlane->m_PlaneName+tr("_Wing2");
		pPlane->m_Stab.m_WingName  = pPlane->m_PlaneName+tr("_Elev");
		pPlane->m_Fin.m_WingName   = pPlane->m_PlaneName+tr("_Fin");

		for (l=(int)m_poaWPolar->size()-1;l>=0; l--)
		{
			pWPolar = (CWPolar*)m_poaWPolar->at(l);
			if (pWPolar->m_UFOName == OldName)
			{
				pWPolar->m_UFOName = pPlane->m_PlaneName;
			}
		}
		for (l=(int)m_poaPOpp->size()-1;l>=0; l--)
		{
			pPOpp = (CPOpp*)m_poaPOpp->at(l);
			if (pPOpp->m_PlaneName == OldName)
			{
				pPOpp->m_PlaneName = pPlane->m_PlaneName;
			}
		}
		return pPlane->m_PlaneName;
	}
	else if(pWing)
	{
		OldName = pWing->m_WingName;
		SetModWing(pWing);
		for (l=(int)m_poaWPolar->size()-1;l>=0; l--)
		{
			pWPolar = (CWPolar*)m_poaWPolar->at(l);
			if (pWPolar->m_UFOName == OldName){
				pWPolar->m_UFOName = pWing->m_WingName;
			}
		}
		for (l=(int)m_poaWOpp->size()-1;l>=0; l--)
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


void QMiarex::RotateGeomZ(double const &Beta, CVector const &P)
{
	int n, p, pw, kw, lw;
//	double cosb = cos(Beta*PI/180.0);
//	double sinb = sin(Beta*PI/180.0);
	int iLA, iLB, iTA, iTB;
	CVector LATB, TALB, Pt, Ptr, Trans;

	for (n=0; n< m_nNodes; n++)	m_Node[n].RotateZ(P, Beta);

	for (p=0; p< m_MatSize; p++)
	{
		iLA = m_Panel[p].m_iLA; iLB = m_Panel[p].m_iLB;
		iTA = m_Panel[p].m_iTA; iTB = m_Panel[p].m_iTB;

		LATB = m_Node[iLA] - m_Node[iTB];
		TALB = m_Node[iTA] - m_Node[iLB];

		if(m_Panel[p].m_iPos>=0)		m_Panel[p].SetFrame(m_Node[iLA], m_Node[iLB], m_Node[iTA], m_Node[iTB]);
		else if (m_Panel[p].m_iPos==-1)	m_Panel[p].SetFrame(m_Node[iLB], m_Node[iLA], m_Node[iTB], m_Node[iTA]);
	}

	// the wake array is not rotated but translated to remain at the wing's trailing edge
	pw=0;

	if(!m_pCurWPolar) return;
	for (kw=0; kw<m_NWakeColumn; kw++)
	{
		//consider the first panel of the column;
		Pt = m_WakeNode[m_WakePanel[pw].m_iLA];
		Pt.RotateZ(P, Beta);
		//define the translation to be applied to the column's left points
		Trans = Pt-m_WakeNode[m_WakePanel[pw].m_iLA] ;

		//each wake column has m_NXWakePanels ... translate all left A nodes
		for(lw=0; lw<m_pCurWPolar->m_NXWakePanels; lw++)
		{
			if(lw==0) m_WakeNode[m_WakePanel[pw].m_iLA] += Trans;
			m_WakeNode[m_WakePanel[pw].m_iTA] += Trans;
			pw++;
		}
	}
	pw -= m_pCurWPolar->m_NXWakePanels;
	//consider the first panel of the column;
	Pt = m_WakeNode[m_WakePanel[pw].m_iLB];
	Pt.RotateZ(P, Beta);
	//define the translation to be applied to the column's left points
	Trans = Pt - m_WakeNode[m_WakePanel[pw].m_iLB];

	//each wake column has m_NXWakePanels ... translate all left A nodes
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


bool QMiarex::SaveSettings(QSettings *pSettings)
{
	int k=0;
	pSettings->beginGroup("Miarex");
	{
		pSettings->setValue("bXTop", m_bXTop);
		pSettings->setValue("bXBot", m_bXBot  );
		pSettings->setValue("bXCP", m_bXCP);
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
		pSettings->setValue("LogFile", m_bLogFile  );
		pSettings->setValue("Dirichlet", m_bDirichlet  );
		pSettings->setValue("Trefftz", m_bTrefftz  );
		pSettings->setValue("KeepOutOpps", m_bKeepOutOpps  );
		pSettings->setValue("ResetWake", m_bResetWake );
		pSettings->setValue("ShowWing2", m_bShowWing2 );
		pSettings->setValue("ShowStab", m_bShowStab  );
		pSettings->setValue("ShowFin", m_bShowFin );
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
		
		pSettings->setValue("CvPrec", CWing::s_CvPrec);
		pSettings->setValue("RelaxMax", CWing::s_RelaxMax);
		pSettings->setValue("NLLTStations", CWing::s_NLLTStations);
		pSettings->setValue("iView", m_iView);
		pSettings->setValue("iWingView", m_iWingView);
		pSettings->setValue("iWPlrView", m_iWPlrView);
		pSettings->setValue("iWStabView", m_iStabilityView);

		if     (m_pCurWingGraph==&m_WingGraph1) k=1;
		else if(m_pCurWingGraph==&m_WingGraph2) k=2;
		else if(m_pCurWingGraph==&m_WingGraph3) k=3;
		else if(m_pCurWingGraph==&m_WingGraph4) k=4;
		pSettings->setValue("CurrentWingGraph", k);

		if     (m_pCurWPlrGraph==&m_WPlrGraph1) k=1;
		else if(m_pCurWPlrGraph==&m_WPlrGraph2) k=2;
		else if(m_pCurWPlrGraph==&m_WPlrGraph3) k=3;
		else if(m_pCurWPlrGraph==&m_WPlrGraph4) k=4;
		pSettings->setValue("CurWPlrGraph", k);

		if     (m_pCurRLStabGraph == &m_LongRLGraph) k=1;
		else if(m_pCurRLStabGraph == &m_LatRLGraph)  k=2;
		pSettings->setValue("CurRLStabGraph",k);

		if(m_iView==WOPPVIEW)        m_pCurGraph=m_pCurWingGraph;
		else if(m_iView==WPOLARVIEW) m_pCurGraph=m_pCurWPlrGraph;

		pSettings->setValue("Iter", m_Iter);
		pSettings->setValue("NStation", m_NStation);
		pSettings->setValue("InducedDragPoint", m_InducedDragPoint);
		pSettings->setValue("NHoopPoints", m_NHoopPoints);
		pSettings->setValue("NXPoints", m_NXPoints);

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
		pSettings->setValue("TimeIn0", m_TimeInput[0]);
		pSettings->setValue("TimeIn1", m_TimeInput[1]);
		pSettings->setValue("TimeIn2", m_TimeInput[2]);
		pSettings->setValue("TimeIn3", m_TimeInput[3]);
		pSettings->setValue("DynamicsMode", m_bLongitudinal);
		pSettings->setValue("ForcedResponse", m_bForcedResponse);

		pSettings->setValue("StabTimeView", m_iStabTimeView);
		if(m_pCurTimeGraph == &m_TimeGraph1)      k=1;
		else if(m_pCurTimeGraph == &m_TimeGraph2) k=2;
		else if(m_pCurTimeGraph == &m_TimeGraph3) k=3;
		else if(m_pCurTimeGraph == &m_TimeGraph4) k=4;
		pSettings->setValue("TimeGraph",k);
 	}
	pSettings->endGroup();


	m_WingGraph1.SaveSettings(pSettings);			// the WOpp graphs
	m_WingGraph2.SaveSettings(pSettings);
	m_WingGraph3.SaveSettings(pSettings);
	m_WingGraph4.SaveSettings(pSettings);
	m_WPlrGraph1.SaveSettings(pSettings);			// the WPolar graphs
	m_WPlrGraph2.SaveSettings(pSettings);
	m_WPlrGraph3.SaveSettings(pSettings);
	m_WPlrGraph4.SaveSettings(pSettings);
	m_CpGraph.SaveSettings(pSettings);
	m_LongRLGraph.SaveSettings(pSettings);
	m_LatRLGraph.SaveSettings(pSettings);
	m_TimeGraph1.SaveSettings(pSettings);
	m_TimeGraph2.SaveSettings(pSettings);
	m_TimeGraph3.SaveSettings(pSettings);
	m_TimeGraph4.SaveSettings(pSettings);

	m_GLLightDlg.SaveSettings(pSettings);
	m_GL3dBody.SaveSettings(pSettings);

	return true;
}



void QMiarex::Set2DScale()
{
	if(m_bIs2DScaleSet && !m_bAutoScales) return;

	int w,h;

	h = m_rCltRect.height();
	w = m_rCltRect.width();
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

	if(m_bHalfWing) m_ptOffset.rx() = m_rSingleRect.left() + m_WingGraph1.GetMargin();
	else            m_ptOffset.rx() = (int)(m_rCltRect.width()/2.0);
	m_ptOffset.ry() = m_rCltRect.bottom()-185;

	m_rSingleRect.setRect(40,10,m_rCltRect.right()-80,m_rCltRect.bottom()-230);

	QRect Rect1(0,0,w2,h38);
	QRect Rect2(w2,0,w2,h38);
	QRect Rect3(0,h38,w2,h38);
	QRect Rect4(w2,h38,w2,h38);
	m_WingGraph1.SetDrawRect(Rect1);
	m_WingGraph2.SetDrawRect(Rect2);
	m_WingGraph3.SetDrawRect(Rect3);
	m_WingGraph4.SetDrawRect(Rect4);

	m_WingGraph1.Init();
	m_WingGraph2.Init();
	m_WingGraph3.Init();
	m_WingGraph4.Init();

	m_WingGraph1.SetAutoXUnit();
	m_WingGraph2.SetAutoXUnit();
	m_WingGraph3.SetAutoXUnit();
	m_WingGraph4.SetAutoXUnit();


	if(m_iView==WOPPVIEW)        SetWingLegendPos();
	else if(m_iView==WPOLARVIEW) SetWPlrLegendPos();
	else if(m_iView==WSTABVIEW) 
	{
		if(m_iStabilityView==0) SetWingLegendPos();
		else                    SetWPlrLegendPos();
	}

	if(m_pCurWing)
	{
		m_WingScale = (m_rSingleRect.width()-2*m_WingGraph1.GetMargin())/m_pCurWing->m_PlanformSpan;
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

	if(m_iView==W3DVIEW)
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
	if(m_iView==W3DVIEW) m_bResetglLegend = true;
	if(m_bIs3DScaleSet && !m_bAutoScales) return;

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
		double gh = (double)m_rCltRect.height()/(double)m_rCltRect.width() ;
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
	m_bResetglModeLegend = true;
}



void QMiarex::SetAnalysisParams()
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
//	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
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
	else if(m_pCurWPolar  && m_pCurWPolar->m_Type ==4)
	{
		m_pctrlAlphaMin->SetValue(m_QInfMin*pMainFrame->m_mstoUnit);
		m_pctrlAlphaMax->SetValue(m_QInfMax*pMainFrame->m_mstoUnit);
		m_pctrlAlphaDelta->SetValue(m_QInfDelta*pMainFrame->m_mstoUnit);
	}
	else if(m_pCurWPolar && (m_pCurWPolar->m_Type==5 || m_pCurWPolar->m_Type==6 || m_pCurWPolar->m_Type==7))
	{
		m_pctrlAlphaMin->SetValue(m_ControlMin);
		m_pctrlAlphaMax->SetValue(m_ControlMax);
		m_pctrlAlphaDelta->SetValue(m_ControlDelta);
	}

	m_pctrlOutline->setChecked(m_bOutline);
	m_pctrlPanels->setChecked(m_bVLMPanels);
	m_pctrlAxes->setChecked(m_bAxes);
	m_pctrlCp->setChecked(m_b3DCp);
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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(m_iView==WPOLARVIEW || (m_iView==WSTABVIEW && m_iStabilityView==1))
	{
		if(m_pCurWPolar)
		{
			if(m_pCurWPolar->m_bIsVisible)  m_pctrlShowCurve->setChecked(true);  else  m_pctrlShowCurve->setChecked(false);
			if(m_pCurWPolar->m_bShowPoints) m_pctrlShowPoints->setChecked(true); else  m_pctrlShowPoints->setChecked(false);

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
	else if(m_iView==WOPPVIEW || (m_iView==WSTABVIEW&& m_iStabilityView==0))
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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
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


	RenameDlg RDlg(this);
	RDlg.m_pstrArray = & NameList;
	RDlg.m_strQuestion = tr("Enter the new name for the Body :");
	RDlg.m_strName = pModBody->m_BodyName;
	RDlg.InitDialog();


	while (bExists)
	{
		resp = RDlg.exec();
		if(resp==QDialog::Accepted)
		{
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaBody->size(); k++)
			{
				pBody = (CBody*)m_poaBody->at(k);
				if (pBody->m_BodyName == RDlg.m_strName)
				{
					bExists = true;
					break;
				}
			}
			if(!bExists)
			{
				pModBody->m_BodyName = RDlg.m_strName;
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
			pOldBody  = GetBody(RDlg.m_strName);
			if(pOldBody)
			{
				//first we check if this old body is used by one or more planes
				bool bIsInUse = false;
				resp = QMessageBox::Yes;
				for(k=0; k<m_poaPlane->size(); k++)
				{
					pPlane = (CPlane*)m_poaPlane->at(k);
					if(pPlane->m_bBody && pPlane->m_pBody==pOldBody)
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
						if (pBody->m_BodyName == RDlg.m_strName)
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
						if(pPlane->m_bBody && pPlane->m_pBody==pOldBody)
						{
							pPlane->m_pBody = pModBody;
							pMainFrame->DeletePlane(pPlane, true);
							pMainFrame->UpdateWOpps();
						}
					}
				}
			}

			pModBody->m_BodyName = RDlg.m_strName;
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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(!pModPlane) pModPlane = m_pCurPlane;
	CPlane * pPlane;
	CWing *pWing;
	CWPolar* pWPolar;
	CPOpp * pPOpp;
	CWOpp *pWOpp;
	bool bExists = true;
	int resp, k, l;

	QString OldName = pModPlane->m_PlaneName;

	QStringList NameList;
	for(k=0; k<m_poaPlane->size(); k++)
	{
		pPlane = (CPlane*)m_poaPlane->at(k);
		NameList.append(pPlane->m_PlaneName);
	}
	for(k=0; k<m_poaWing->size(); k++)
	{
		pWing = (CWing*)m_poaWing->at(k);
		NameList.append(pWing->m_WingName);
	}
	RenameDlg RDlg(this);
	RDlg.move(pMainFrame->m_DlgPos);
	RDlg.m_pstrArray = & NameList;
	RDlg.m_strQuestion = tr("Enter the new name for the Plane :");
	RDlg.m_strName     = pModPlane->m_PlaneName;
	RDlg.InitDialog();

	while (bExists)
	{
		resp = (int)RDlg.exec();
		pMainFrame->m_DlgPos = RDlg.pos();
		if(resp==QDialog::Accepted)
		{
			if (OldName == RDlg.m_strName) return true;
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaPlane->size(); k++)
			{
				pPlane = (CPlane*)m_poaPlane->at(k);
				if (pPlane->m_PlaneName == RDlg.m_strName)
				{
					bExists = true;
					break;
				}
			}
			for (k=0; k<m_poaWing->size(); k++)
			{
				pWing = (CWing*)m_poaWing->at(k);
				if (pWing->m_WingName == RDlg.m_strName)
				{
					bExists = true;
					break;
				}
			}
			if(!bExists)
			{
				pModPlane->m_PlaneName = RDlg.m_strName;
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
							if(pPlane->m_PlaneName.compare(pModPlane->m_PlaneName, Qt::CaseInsensitive) >0)
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
			if (OldName == RDlg.m_strName) return true;//don't bother to overwrite itself
			if(GetPlane(RDlg.m_strName))
			{
				for (k=0; k<m_poaPlane->size(); k++)
				{
					pPlane = (CPlane*)m_poaPlane->at(k);
					if (pPlane->m_PlaneName == RDlg.m_strName)
					{
						for (l=m_poaWPolar->size()-1;l>=0; l--)
						{
							pWPolar = (CWPolar*)m_poaWPolar->at(l);
							if (pWPolar->m_UFOName == pPlane->m_PlaneName)
							{
								m_poaWPolar->removeAt(l);
								if(pWPolar==m_pCurWPolar)
								{
									m_pCurWPolar = NULL;
//									pMainFrame->m_WOperDlgBar.EnableAnalysis(false);
								}
								delete pWPolar;
							}
						}
						for (l=m_poaPOpp->size()-1;l>=0; l--)
						{
							pPOpp = (CPOpp*)m_poaPOpp->at(l);
							if (pPOpp->m_PlaneName == pPlane->m_PlaneName)
							{
								m_poaPOpp->removeAt(l);
								if(pPOpp==m_pCurPOpp)
								{
									m_pCurPOpp = NULL;
									m_pCurWOpp = NULL;
								}
								delete pPOpp;
							}
						}
						m_poaPlane->removeAt(k);
						if(pPlane==m_pCurPlane) m_pCurPlane = NULL;
						delete pPlane;

					}
				}
			}
			else if (GetWing(RDlg.m_strName))
			{
				//delete the wing the user wants to overwrite
				for (k=0; k<m_poaWing->size(); k++)
				{
					pWing = (CWing*)m_poaWing->at(k);
					if (pWing->m_WingName == RDlg.m_strName)
					{
						for (l=(int)m_poaWPolar->size()-1;l>=0; l--)
						{
							pWPolar = (CWPolar*)m_poaWPolar->at(l);
							if (pWPolar->m_UFOName == pWing->m_WingName){
								m_poaWPolar->removeAt(l);
								if(pWPolar==m_pCurWPolar)
								{
									m_pCurWPolar = NULL;
//									pMainFrame->m_WOperDlgBar.EnableAnalysis(false);
								}
								delete pWPolar;
							}
						}
						for (l=(int)m_poaWOpp->size()-1;l>=0; l--)
						{
							pWOpp = (CWOpp*)m_poaWOpp->at(l);
							if (pWOpp->m_WingName == pWing->m_WingName)
							{
								m_poaWOpp->removeAt(l);
								if(pWOpp==m_pCurWOpp)
								{
									m_pCurWOpp = NULL;
								}
								delete pWOpp;
							}
						}
						m_poaWing->removeAt(k);
						if(pWing==m_pCurWing) m_pCurWing = NULL;
						delete pWing;
					}
				}
			}

			pModPlane->m_PlaneName = RDlg.m_strName;
			pModPlane->m_Wing.m_WingName = pModPlane->m_PlaneName+"_Wing";
			pModPlane->m_Wing2.m_WingName = pModPlane->m_PlaneName+"_Wing2";
			pModPlane->m_Stab.m_WingName  = pModPlane->m_PlaneName+"_Elev";
			pModPlane->m_Fin.m_WingName   = pModPlane->m_PlaneName+"_Fin";
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
				if(pPlane->m_PlaneName.compare(m_pCurPlane->m_PlaneName, Qt::CaseInsensitive) <0)
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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
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
		NameList.append(pPlane->m_PlaneName);
	}
	RenameDlg RDlg(this);
	RDlg.move(pMainFrame->m_DlgPos);
	RDlg.m_pstrArray = & NameList;
	RDlg.m_strQuestion = tr("Enter the new name for the wing :");
	RDlg.m_strName = pModWing->m_WingName;
	RDlg.InitDialog();

	while (bExists)
	{
		resp = RDlg.exec();
		pMainFrame->m_DlgPos = RDlg.pos();
		if(resp==QDialog::Accepted)
		{
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaWing->size(); k++)
			{
				pWing = (CWing*)m_poaWing->at(k);
				if (pWing->m_WingName == RDlg.m_strName)
				{
					bExists = true;
					break;
				}
			}
			for (k=0; k<m_poaPlane->size(); k++)
			{
				pPlane = (CPlane*)m_poaPlane->at(k);
				if (pPlane->m_PlaneName == RDlg.m_strName)
				{
					bExists = true;
					break;
				}
			}

			if(!bExists)
			{
				pModWing->m_WingName = RDlg.m_strName;
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
		else if(resp ==10)
		{
			//user wants to overwrite
			pOldWing  = GetWing(RDlg.m_strName);
			pOldPlane = GetPlane(RDlg.m_strName);
			if(pOldWing)
			{
				for (k=0; k<m_poaWing->size(); k++)
				{
					pWing = (CWing*)m_poaWing->at(k);
					if (pWing->m_WingName == RDlg.m_strName)
					{
						for (l=(int)m_poaWPolar->size()-1;l>=0; l--)
						{
							pWPolar = (CWPolar*)m_poaWPolar->at(l);
							if (pWPolar->m_UFOName == pWing->m_WingName)
							{
								m_poaWPolar->removeAt(l);
								if(pWPolar==m_pCurWPolar)
								{
									m_pCurWPolar = NULL;
									m_pCurPOpp   = NULL;
									m_pCurWOpp   = NULL;
//									pMainFrame->m_WOperDlgBar.EnableAnalysis(false);
								}
								delete pWPolar;
							}
						}
						for (l=(int)m_poaWOpp->size()-1;l>=0; l--)
						{
							pWOpp = (CWOpp*)m_poaWOpp->at(l);
							if (pWOpp->m_WingName == pWing->m_WingName)
							{
								m_poaWOpp->removeAt(l);
								if(pWOpp==m_pCurWOpp){
									m_pCurWOpp = NULL;
									m_pCurPOpp = NULL;
								}
								delete pWOpp;
							}
						}
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
				if(pOldPlane->m_bActive) QMessageBox::warning(window(), tr("Warning"),tr("Cannot overwrite current plane"));
				else
				{
					for (k=0; k<m_poaPlane->size(); k++)
					{
						pPlane = (CPlane*)m_poaPlane->at(k);
						if (pPlane->m_PlaneName == RDlg.m_strName)
						{
							for (l=(int)m_poaWPolar->size()-1;l>=0; l--)
							{
								pWPolar = (CWPolar*)m_poaWPolar->at(l);
								if (pWPolar->m_UFOName == pPlane->m_PlaneName)
								{
									m_poaWPolar->removeAt(l);
									if(pWPolar==m_pCurWPolar)
									{
										m_pCurWPolar = NULL;
										m_pCurPOpp   = NULL;
										m_pCurWOpp   = NULL;
//										pMainFrame->m_WOperDlgBar.EnableAnalysis(false);
									}
									delete pWPolar;
								}
							}
							for (l=(int)m_poaPOpp->size()-1;l>=0; l--)
							{
								pPOpp = (CPOpp*)m_poaPOpp->at(l);
								if (pPOpp->m_PlaneName == pPlane->m_PlaneName)
								{
									m_poaPOpp->removeAt(l);
									if(pPOpp==m_pCurPOpp)
									{
										m_pCurPOpp = NULL;
										m_pCurPOpp = NULL;
									}
									delete pPOpp;
								}
							}
							m_poaPlane->removeAt(k);
							delete pPlane;
							bExists = false;
							break;
						}
					}
				}
			}
			pModWing->m_WingName = RDlg.m_strName;
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


bool QMiarex::SetPOpp(bool bCurrent, double Alpha)
{
	// set the WOpp, if valid
	// else set the current WOpp, if any
	// else set the comboBox's first, if any
	// else set it to NULL
	if(!m_pCurPlane) return false;

	CPOpp *pPOpp = NULL;
	if(bCurrent) pPOpp = m_pCurPOpp;
	else         pPOpp = GetPOpp(Alpha);
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	QString strong;
	double x;
	bool bOK;

	m_bResetglOpp    = true;
	m_bResetglWake   = true;
	m_bResetglStream = true;
	m_bResetglFlow   = true;
	m_bResetglLegend = true;

	if(!pPOpp)
	{
		if(m_pCurWPolar)
			pPOpp = GetPOpp(m_pCurWPolar->m_AMem);
		else
			pPOpp = GetPOpp(m_LastWOpp);
	}

//	if(!pPOpp) pPOpp = m_pCurPOpp;

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
		m_pCurWOpp = &m_pCurPOpp->m_WingWOpp;
		if(m_pCurWPolar) m_pCurWPolar->m_AMem = m_pCurPOpp->m_Alpha;

		//select m_pCurPOpp in the listbox
		if(m_pCurWPolar->m_Type != 4) strong = QString("%1").arg(m_pCurPOpp->m_Alpha,8,'f',2);
		else                          strong = QString("%1").arg(m_pCurPOpp->m_QInf,8,'f',2);

		int pos = pMainFrame->m_pctrlWOpp->findText(strong);
		if(pos >=0)
		{
			pMainFrame->m_pctrlWOpp->setCurrentIndex(pos);
		}
		else
		{
			pMainFrame->m_pctrlWOpp->setCurrentIndex(0);
		}
	}
	else
	{
		m_pCurWOpp = NULL;
	}

	if (m_iView==WOPPVIEW)      CreateWOppCurves();
	else if(m_iView==WCPVIEW)   CreateCpCurves();
	else if(m_iView==WSTABVIEW)
	{
		StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
		pStabView->SetControls();
		pStabView->SetMode();
		CreateStabilityCurves();
	}

	if(!m_pCurPOpp) return false;
	else            return true;
}


void QMiarex::SetScale()
{
	Set2DScale();
	Set3DScale();
}

void QMiarex::SetScale(QRect CltRect)
{
	m_rCltRect = CltRect;
//	m_rCltRect.setRect(CltRect.x(), CltRect.y(), CltRect.width(), CltRect.height());
	m_rSingleRect.setRect(CltRect.x(), CltRect.y(), CltRect.width(), CltRect.height());

//	m_rSingleRect.adjust(60,20,-60,-600);
//	SetBodyScale();
	Set2DScale();
	Set3DScale();
}



void QMiarex::SetUFO(QString UFOName)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	int i;
	CWing *pWing;
	CPlane *pPlane;
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	if(!UFOName.length())
	{
		if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
		else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
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
				if(pPlane) FirstPlaneName = pPlane->m_PlaneName;
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
				m_pCurPlane = NULL;
				m_pCurBody  = NULL;
				m_pCurWing  = NULL;
				m_pCurWing2 = NULL;
				m_pCurStab  = NULL;
				m_pCurFin   = NULL;
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
		m_pCurBody  = NULL;
		m_pCurWing2 = NULL;
		m_pCurStab  = NULL;
		m_pCurFin   = NULL;
		m_pCurWOpp  = NULL;
		m_pCurPOpp  = NULL;

		QApplication::restoreOverrideCursor();
		return;
	}

	if(m_pCurPlane)
	{
		m_pCurWing = &m_pCurPlane->m_Wing;
		if(m_pCurPlane->m_bBody)    m_pCurBody  = m_pCurPlane->m_pBody;
		else						m_pCurBody  = NULL;
		if(m_pCurPlane->m_bBiplane) m_pCurWing2 = &m_pCurPlane->m_Wing2;
		else						m_pCurWing2 = NULL;
		if(m_pCurPlane->m_bStab)	m_pCurStab  = &m_pCurPlane->m_Stab;
		else						m_pCurStab  = NULL;
		if(m_pCurPlane->m_bFin)
		{
			m_pCurFin  = &m_pCurPlane->m_Fin;
			m_pCurFin->m_bDoubleSymFin = m_pCurPlane->m_bDoubleSymFin;
		}
		else
			m_pCurFin  = NULL;
	}
	else
	{
		m_pCurPOpp  = NULL;
		m_pCurBody  = NULL;
		m_pCurWing2 = NULL;
		m_pCurStab  = NULL;
		m_pCurFin   = NULL;
	}

	m_bResetglGeom   = true;
	if(m_pCurBody)
	{
		m_bResetglBody   = true;
	}
	else
	{
		m_bResetglBody   = false;
	}

	m_bResetglMesh = true;

	double WingTilt = 0.0;
	QColor WingColor = m_WingColor;
	CVector T(0.0,0.0,0.0);
	m_NSurfaces = 0;

	double dx, dz;
	dx=dz=0.0;

	if(m_pCurPlane)
	{
		WingTilt  = m_pCurPlane->m_WingTilt;
		WingColor = m_pCurWing->m_WingColor;
		if(m_pCurPlane->m_bBody)
		{
			dx = m_pCurPlane->m_BodyPos.x;
			dz = m_pCurPlane->m_BodyPos.z;
		}
	}

	if(m_pCurBody)
	{
		m_pCurBody->SetKnots();
		m_pCurBody->SetPanelPos();
	}
	if(m_pCurWing)
	{
		if(m_pCurPlane)	m_pCurWing->CreateSurfaces(m_pCurPlane->m_LEWing, 0.0, WingTilt);
		else            m_pCurWing->CreateSurfaces(T, 0.0, WingTilt);
		for (i=0; i<m_pCurWing->m_NSurfaces; i++)
		{
			m_pCurWing->m_Surface[i].SetSidePoints(m_pCurBody, dx, dz);
			m_pSurface[m_NSurfaces] = &m_pCurWing->m_Surface[i];
			m_NSurfaces++;
		}
	}

	if(m_pCurWing2)
	{
		m_pCurWing2->CreateSurfaces(m_pCurPlane->m_LEWing2, 0.0, m_pCurPlane->m_WingTilt2);
		for (i=0; i<m_pCurWing2->m_NSurfaces; i++)
		{
			m_pCurWing2->m_Surface[i].SetSidePoints(m_pCurBody, dx, dz);
			m_pSurface[m_NSurfaces] = &m_pCurWing2->m_Surface[i];
			m_NSurfaces++;
		}
	}

	if(m_pCurStab)
	{
		m_pCurStab->CreateSurfaces(m_pCurPlane->m_LEStab, 0.0, m_pCurPlane->m_StabTilt);
		for (i=0; i<m_pCurStab->m_NSurfaces; i++)
		{
			m_pCurStab->m_Surface[i].SetSidePoints(m_pCurBody, dx, dz);
			m_pSurface[m_NSurfaces] = &m_pCurStab->m_Surface[i];
			m_NSurfaces++;
		}
	}

	if(m_pCurFin)
	{
		m_pCurFin->CreateSurfaces(m_pCurPlane->m_LEFin, -90.0, m_pCurPlane->m_FinTilt);
		for (i=0; i<m_pCurFin->m_NSurfaces; i++)
		{
			m_pCurFin->m_Surface[i].SetSidePoints(m_pCurBody, dx, dz);
			m_pSurface[m_NSurfaces] = &m_pCurFin->m_Surface[i];
			m_NSurfaces++;
		}
	}

	if (m_pCurWPolar)
	{
		// try to set the same as the existing polar... Special for Marc
		SetWPlr(false, m_pCurWPolar->m_PlrName);
	}
	else
	{
		SetWPlr();
	}
//	pMainFrame->UpdateWPolars();

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

//	setSizePolicy(szPolicyMaximum);
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
	m_pctrlAlphaMin     = new FloatEdit(0.0, 2);
	m_pctrlAlphaMax     = new FloatEdit(1., 2);
	m_pctrlAlphaDelta   = new FloatEdit(0.5, 2);

	m_pctrlUnit1 = new QLabel(QString::fromUtf8("°"));
	m_pctrlUnit2 = new QLabel(QString::fromUtf8("°"));
	m_pctrlUnit3 = new QLabel(QString::fromUtf8("°"));

	m_pctrlAlphaMin->setMinimumHeight(20);
	m_pctrlAlphaMax->setMinimumHeight(20);
	m_pctrlAlphaDelta->setMinimumHeight(20);
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
	m_pctrlHalfWing      = new QCheckBox(tr("1/2 wing"));
	m_pctrlLift          = new QCheckBox(tr("Lift"));
	m_pctrlIDrag         = new QCheckBox(tr("Ind. Drag"));
	m_pctrlVDrag         = new QCheckBox(tr("Visc. Drag"));
	m_pctrlTrans         = new QCheckBox(tr("Trans."));
	m_pctrlMoment        = new QCheckBox(tr("Moment"));
	m_pctrlDownwash      = new QCheckBox(tr("Downw."));
	m_pctrlCp            = new QCheckBox(tr("Cp"));
	m_pctrlSurfVel       = new QCheckBox(tr("Surf. Vel."));
	m_pctrlStream        = new QCheckBox(tr("Stream"));
	m_pctrlWOppAnimate   = new QCheckBox(tr("Animate"));
	m_pctrlHighlightOpp  = new QCheckBox(tr("Highlight OpPoint"));
	m_pctrlHalfWing->setSizePolicy(szPolicyMaximum);
	m_pctrlLift->setSizePolicy(szPolicyMaximum);
	m_pctrlIDrag->setSizePolicy(szPolicyMaximum);
	m_pctrlVDrag->setSizePolicy(szPolicyMaximum);
	m_pctrlTrans->setSizePolicy(szPolicyMaximum);
	m_pctrlMoment->setSizePolicy(szPolicyMaximum);
	m_pctrlDownwash->setSizePolicy(szPolicyMaximum);
	m_pctrlCp->setSizePolicy(szPolicyMaximum);
	m_pctrlSurfVel->setSizePolicy(szPolicyMaximum);
	m_pctrlStream->setSizePolicy(szPolicyMaximum);
	m_pctrlWOppAnimate->setSizePolicy(szPolicyMaximum);
	m_pctrlHighlightOpp->setSizePolicy(szPolicyMaximum);

	m_pctrlAnimateWOppSpeed  = new QSlider(Qt::Horizontal);
	m_pctrlAnimateWOppSpeed->setMinimum(0);
	m_pctrlAnimateWOppSpeed->setMaximum(500);
	m_pctrlAnimateWOppSpeed->setSliderPosition(250);
	m_pctrlAnimateWOppSpeed->setTickInterval(50);
	m_pctrlAnimateWOppSpeed->setTickPosition(QSlider::TicksBelow);
	CheckDispLayout->addWidget(m_pctrlHalfWing, 1, 1);
	CheckDispLayout->addWidget(m_pctrlCp,       1, 2);
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
//	CheckDispLayout->addWidget(m_pctrlHighlightOpp,   7, 1,1,2);
//	m_pctrl3DSettings =new QPushButton(tr("3D Scales"));
//	m_pctrl3DSettings->setCheckable(true);
//	QVBoxLayout *DisplayLayout = new QVBoxLayout;
//	DisplayLayout->addLayout(CheckDispLayout);
//	DisplayLayout->addWidget(m_pctrl3DSettings);
	QGroupBox *DisplayBox = new QGroupBox(tr("Display"));
	DisplayBox->setLayout(CheckDispLayout);


//_______________________Curve params
	QVBoxLayout *CurveGroup = new QVBoxLayout;
	m_pctrlShowCurve  = new QCheckBox(tr("Curve"));
	m_pctrlShowPoints = new QCheckBox(tr("Points"));
	m_pctrlShowCurve->setMinimumHeight(10);
	m_pctrlShowPoints->setMinimumHeight(10);
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
	m_pctrlKeepCpSection = new QPushButton(tr("Keep"));
	m_pctrlResetCpSection = new QPushButton(tr("Reset"));
	CpSections->addWidget(m_pctrlKeepCpSection);
	CpSections->addWidget(m_pctrlResetCpSection);
	CpParams->addWidget(m_pctrlCpSectionSlider);
	CpParams->addLayout(CpPos);
	CpParams->addLayout(CpSections);
	CpParams->addStretch(1);
	QGroupBox *CpBox = new QGroupBox(tr("Cp Sections"));
	CpBox->setLayout(CpParams);

//_______________________Stability Params
	QVBoxLayout	 *StabParams = new QVBoxLayout;
	m_pctrlLongDynamics = new QRadioButton(tr("Longitudinal"));
	m_pctrlLatDynamics = new QRadioButton(tr("Lateral"));
	m_pctrlLongDynamics->setSizePolicy(szPolicyMaximum);
	m_pctrlLatDynamics->setSizePolicy(szPolicyMaximum);
	m_pctrlLongDynamics->setMinimumHeight(10);
	m_pctrlLatDynamics->setMinimumHeight(10);
	StabParams->addWidget(m_pctrlLongDynamics);
	StabParams->addWidget(m_pctrlLatDynamics);
	StabParams->addStretch(1);
	QGroupBox *StabBox = new QGroupBox(tr("Stability"));
	StabBox->setLayout(StabParams);

	QVBoxLayout *StabViewTypeLayout = new QVBoxLayout;
	m_pctrlTimeView = new QRadioButton(tr("Time Response"));
	m_pctrlRLView   = new QRadioButton(tr("Root Locus"));
	m_pctrl3DView   = new QRadioButton(tr("3D Mode"));
	m_pctrlTimeView->setSizePolicy(szPolicyMaximum);
	m_pctrlRLView->setSizePolicy(szPolicyMaximum);
	m_pctrl3DView->setSizePolicy(szPolicyMaximum);
	m_pctrlTimeView->setMinimumHeight(10);
	m_pctrlRLView->setMinimumHeight(10);
	m_pctrl3DView->setMinimumHeight(10);

	StabViewTypeLayout->addWidget(m_pctrlTimeView);
	StabViewTypeLayout->addWidget(m_pctrlRLView);
	StabViewTypeLayout->addWidget(m_pctrl3DView);
	QGroupBox *StabViewTypeBox = new QGroupBox(tr("View Type"));
	StabViewTypeBox->setLayout(StabViewTypeLayout);
	QVBoxLayout *StabilityLayout = new QVBoxLayout;
	StabilityLayout->addWidget(StabBox);
	StabilityLayout->addWidget(StabViewTypeBox);
	StabilityLayout->addStretch(1);
	QGroupBox *StabilityBox = new QGroupBox(tr("Stability Params"));
	StabilityBox->setLayout(StabilityLayout);

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
	m_pctrlAxes->setMinimumHeight(10);
	m_pctrlLight->setMinimumHeight(10);
	m_pctrlSurfaces->setMinimumHeight(10);
	m_pctrlOutline->setMinimumHeight(10);
	m_pctrlPanels->setMinimumHeight(10);
	m_pctrlFoilNames->setMinimumHeight(10);
	m_pctrlVortices->setMinimumHeight(10);
	m_pctrlLight->setSizePolicy(szPolicyMaximum);
	m_pctrlSurfaces->setSizePolicy(szPolicyMaximum);
	m_pctrlOutline->setSizePolicy(szPolicyMaximum);
	m_pctrlPanels->setSizePolicy(szPolicyMaximum);
	m_pctrlVortices->setSizePolicy(szPolicyMaximum);
	m_pctrlFoilNames->setSizePolicy(szPolicyMaximum);
	ThreeDParams->addWidget(m_pctrlAxes, 1,1);
	ThreeDParams->addWidget(m_pctrlLight, 1,2);
	ThreeDParams->addWidget(m_pctrlSurfaces, 2,1);
	ThreeDParams->addWidget(m_pctrlOutline, 2,2);
	ThreeDParams->addWidget(m_pctrlPanels, 3,1);
	ThreeDParams->addWidget(m_pctrlFoilNames, 3,2);

	QGridLayout *ThreeDView = new QGridLayout;
	m_pctrlX          = new QPushButton("X");
	m_pctrlY          = new QPushButton("Y");
	m_pctrlZ          = new QPushButton("Z");
	m_pctrlIso        = new QPushButton("Iso");
	m_pctrlX->setSizePolicy(szPolicyMaximum);
	m_pctrlY->setSizePolicy(szPolicyMaximum);
	m_pctrlZ->setSizePolicy(szPolicyMaximum);
	m_pctrlIso->setSizePolicy(szPolicyMaximum);

	ThreeDView->addWidget(m_pctrlX,1,1);
	ThreeDView->addWidget(m_pctrlY,1,2);
	ThreeDView->addWidget(m_pctrlZ,2,1);
	ThreeDView->addWidget(m_pctrlIso,2,2);

	m_pctrlPickCenter     = new QPushButton(tr("Pick Center"));
	m_pctrlReset          = new QPushButton(tr("Reset"));
	m_pctrlReset->setSizePolicy(szPolicyMaximum);
	m_pctrlPickCenter->setCheckable(true);
	m_pctrlPickCenter->setSizePolicy(szPolicyMaximum);
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
	m_pctrlClipPlanePos->setSizePolicy(szPolicyMinimum);
	ClipLayout->addWidget(ClipLabel);
	ClipLayout->addWidget(m_pctrlClipPlanePos,1);

	ThreeDViewControls->addLayout(ThreeDParams);
	ThreeDViewControls->addLayout(ThreeDView);
	ThreeDViewControls->addLayout(ClipLayout);
	ThreeDViewControls->addStretch(1);
	QGroupBox *ThreeDViewBox = new QGroupBox;
	ThreeDViewBox->setLayout(ThreeDViewControls);

//_________________________Main Layout
	m_pctrlMiddleControls = new QStackedWidget;
	m_pctrlMiddleControls->addWidget(DisplayBox);
	m_pctrlMiddleControls->addWidget(CpBox);
	m_pctrlMiddleControls->addWidget(StabilityBox);

	m_pctrBottomControls = new QStackedWidget;
	m_pctrBottomControls->addWidget(CurveBox);
	m_pctrBottomControls->addWidget(ThreeDViewBox);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addStretch(1);
	mainLayout->addWidget(AnalysisBox);
	mainLayout->addStretch(1);
	mainLayout->addWidget(m_pctrlMiddleControls);
	mainLayout->addStretch(1);
	mainLayout->addWidget(m_pctrBottomControls);
	mainLayout->addStretch(1);

	setLayout(mainLayout);
}




void QMiarex::SetWGraphScale()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(!m_pCurWing)
	{
		m_WingGraph1.SetAuto(false);
		m_WingGraph1.SetXUnit(10.0);
		m_WingGraph1.SetXMin(-100.0);
		m_WingGraph1.SetXMax( 100.0);
		m_WingGraph2.SetAuto(false);
		m_WingGraph2.SetXUnit(10.0);
		m_WingGraph2.SetXMin(-100.0);
		m_WingGraph2.SetXMax( 100.0);
		m_WingGraph3.SetAuto(false);
		m_WingGraph3.SetXUnit(10.0);
		m_WingGraph3.SetXMin(-100.0);
		m_WingGraph3.SetXMax( 100.0);
		m_WingGraph4.SetAuto(false);
		m_WingGraph4.SetXUnit(10.0);
		m_WingGraph4.SetXMin(-100.0);
		m_WingGraph4.SetXMax( 100.0);
	}
	else
	{
		double halfspan = m_pCurWing->m_PlanformSpan/2.0;
		double xmin;
		if(m_bHalfWing) xmin = 0.0;
		else            xmin = -halfspan*pMainFrame->m_mtoUnit;

		m_WingGraph1.SetAutoX(false);
		m_WingGraph1.SetXMin(xmin);
		m_WingGraph1.SetXMax( halfspan*pMainFrame->m_mtoUnit);
		m_WingGraph1.SetAutoXUnit();

		m_WingGraph2.SetAutoX(false);
		m_WingGraph2.SetXMin(xmin);
		m_WingGraph2.SetXMax( halfspan*pMainFrame->m_mtoUnit);
		m_WingGraph2.SetAutoXUnit();

		m_WingGraph3.SetAutoX(false);
		m_WingGraph3.SetXMin(xmin);
		m_WingGraph3.SetXMax( halfspan*pMainFrame->m_mtoUnit);
		m_WingGraph3.SetAutoXUnit();

		m_WingGraph4.SetAutoX(false);
		m_WingGraph4.SetXMin(xmin);
		m_WingGraph4.SetXMax( halfspan*pMainFrame->m_mtoUnit);
		m_WingGraph4.SetAutoXUnit();
	}
}


void QMiarex::SetWPlr(bool bCurrent, QString WPlrName)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	CWPolar *pWPolar = NULL;
	CWPolar *pOldWPolar = NULL;
	QString UFOName;

	int i,j,k,m, NStation;
	double SpanPos;

	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	if(bCurrent) pWPolar = m_pCurWPolar;
	else
	{
		for (i=0; i<m_poaWPolar->size(); i++)
		{
			pOldWPolar = (CWPolar*) m_poaWPolar->at(i);
			if (pOldWPolar->m_UFOName == UFOName &&
				pOldWPolar->m_PlrName == WPlrName)
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
		RotateGeomZ(m_pCurWPolar->m_Beta, RefPoint);
	}

	if(m_pCurWPolar && m_pCurWPolar->m_UFOName==UFOName)
	{
		m_pCurWing->ComputeChords();
		m_pCurWing->m_Type = m_pCurWPolar->m_Type;
		CWing::s_Alpha        = m_pCurWPolar->m_ASpec;//in case its a Type 4 polar
		CWing::s_QInf         = m_pCurWPolar->m_QInf;
		CWing::s_Density      = m_pCurWPolar->m_Density;
		CWing::s_Viscosity    = m_pCurWPolar->m_Viscosity;
		CWing::s_bVLM1        = m_pCurWPolar->m_bVLM1;
//		m_pCurWing->m_bVLMSymetric = m_pCurWing->m_bSymetric;

		if(m_pCurWPolar->m_AnalysisType>1)
		{
			NStation = 0;
			m=0;
			SpanPos = 0;
			for (j=0; j<m_pCurWing->m_NSurfaces; j++)	NStation += m_pCurWing->m_Surface[j].m_NYPanels;

			for (j=(int)(m_pCurWing->m_NSurfaces/2); j<m_pCurWing->m_NSurfaces; j++)
			{
				for(k=0; k<m_pCurWing->m_Surface[j].m_NYPanels; k++)
				{
					m_pCurWing->m_SpanPos[m+NStation/2] = SpanPos + m_pCurWing->m_Surface[j].GetStripSpanPos(k);
					m++;
				}
				SpanPos += m_pCurWing->m_Surface[j].m_Length;
			}

			for(m=0; m<NStation/2; m++) m_pCurWing->m_SpanPos[m] = -m_pCurWing->m_SpanPos[NStation-m-1];

			if(m_pCurWing2)
			{
				m_pCurWing2->ComputeChords();
				m_pCurWing2->m_Type         = m_pCurWPolar->m_Type;

				NStation = 0;
				m=0;
				SpanPos = 0;
				for (j=0; j<m_pCurWing2->m_NSurfaces; j++)	NStation += m_pCurWing2->m_Surface[j].m_NYPanels;

				for (j=(int)m_pCurWing2->m_NSurfaces/2; j<m_pCurWing2->m_NSurfaces; j++)
				{
					for(k=0; k<m_pCurWing2->m_Surface[j].m_NYPanels; k++)
					{
						m_pCurWing2->m_SpanPos[m+NStation/2] = SpanPos + m_pCurWing2->m_Surface[j].GetStripSpanPos(k);
						m++;
					}
					SpanPos += m_pCurWing2->m_Surface[j].m_Length;
				}

				for(m=0; m<NStation/2; m++) m_pCurWing2->m_SpanPos[m] = -m_pCurWing2->m_SpanPos[NStation-m-1];
			}

			if(m_pCurStab)
			{
				m_pCurStab->ComputeChords();
				m_pCurStab->m_Type         = m_pCurWPolar->m_Type;

				NStation = 0;
				m=0;
				SpanPos = 0;
				for (j=0; j<m_pCurStab->m_NSurfaces; j++)	NStation += m_pCurStab->m_Surface[j].m_NYPanels;

				for (j=(int)m_pCurStab->m_NSurfaces/2; j<m_pCurStab->m_NSurfaces; j++)
				{
					for(k=0; k<m_pCurStab->m_Surface[j].m_NYPanels; k++)
					{
						m_pCurStab->m_SpanPos[m+NStation/2] = SpanPos + m_pCurStab->m_Surface[j].GetStripSpanPos(k);
						m++;
					}
					SpanPos += m_pCurStab->m_Surface[j].m_Length;
				}

				for(m=0; m<NStation/2; m++) m_pCurStab->m_SpanPos[m] = -m_pCurStab->m_SpanPos[NStation-m-1];
			}

			if(m_pCurFin)
			{
				m_pCurFin->ComputeChords();
				m_pCurFin->m_Type         = m_pCurWPolar->m_Type;

				NStation = 0;
				m=0;
				SpanPos = 0;
				for (j=0; j<m_pCurFin->m_NSurfaces; j++)	NStation += m_pCurFin->m_Surface[j].m_NYPanels;

				for (j=0; j<m_pCurFin->m_NSurfaces; j++)
				{
					for(k=0; k<m_pCurFin->m_Surface[j].m_NYPanels; k++)
					{
						m_pCurFin->m_SpanPos[m] = m_pCurFin->m_PlanformSpan/2.0 -
															 SpanPos - m_pCurFin->m_Surface[j].GetStripSpanPos(k);
						m++;
					}
					SpanPos += m_pCurFin->m_Surface[j].m_Length;
				}

//				for(m=0; m<NStation/2; m++) m_pCurFin->m_SpanPos[m] = -m_pCurFin->m_SpanPos[NStation-m-1];
			}
		}
		else if(m_pCurWPolar->m_AnalysisType==1)
		{
//			m_pCurWing->m_NStation  = m_NStation;
			m_pCurWing->m_bLLT      = true;
		}

		SetAnalysisParams();
		pMainFrame->UpdateWOpps();

		double Alpha = 0.0;
		if(m_pCurWOpp) Alpha = m_pCurWOpp->m_Alpha;

		if(m_pCurPlane)
		{
			SetPOpp(false, Alpha);
		}
		else if(m_pCurWing)
		{
			SetWOpp(false, Alpha);
		}

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

	if(m_iView==WPOLARVIEW)		CreateWPolarCurves();
	else if(m_iView==WSTABVIEW)	CreateStabilityCurves();
	else if(m_iView==WOPPVIEW)	CreateWOppCurves();
	else if(m_iView==WCPVIEW)	CreateCpCurves();


	SetAnalysisParams();
	SetCurveParams();
	m_bResetglLegend = true;

	m_pVLMDlg->m_pWPolar       = m_pCurWPolar;
	m_pVLMDlg->m_MatSize       = m_MatSize;
	m_pVLMDlg->m_nNodes        = m_nNodes;
	m_pVLMDlg->m_NSurfaces     = m_NSurfaces;

	m_pPanelDlg->m_pWPolar     = m_pCurWPolar;
	m_pPanelDlg->m_MatSize     = m_MatSize;
	m_pPanelDlg->m_WakeSize    = m_WakeSize;
	m_pPanelDlg->m_nNodes      = m_nNodes;
	m_pPanelDlg->m_NSurfaces   = m_NSurfaces;
	m_pPanelDlg->m_NWakeColumn = m_NWakeColumn;
	QApplication::restoreOverrideCursor();
}



void QMiarex::SetWingLegendPos()
{
	int h  = m_rCltRect.height();
	int h23 = (int)(2*h/3);
	int h34 = (int)(3*h/4);

	int margin = 10;
	if(m_iView==WOPPVIEW)
	{
		if(m_iWingView == 1)
		{
			m_WingLegendOffset.rx() = m_rCltRect.right()-300+margin;
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
	int h  = m_rCltRect.height();
	int w  = m_rCltRect.width();
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
		if(m_iStabilityView==1)
		{
			m_WPlrLegendOffset.rx() = (int)(0.70*w)+margin;
			m_WPlrLegendOffset.ry() = margin;
			
		}

	}
}


bool QMiarex::SetWOpp(bool bCurrent, double Alpha)
{
	if(!m_pCurWing || !m_pCurWPolar)
	{
		m_pCurWOpp = NULL;
		m_pCurPOpp = NULL;
		SetCurveParams();
		return false;
	}
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	// set set the current WOpp, if any
	// else set the comboBox's first, if any
	// else set it to NULL
	QString strong;

	if(m_pCurPlane)	  return SetPOpp(bCurrent, Alpha);

	CWOpp *pWOpp = NULL;
	if(bCurrent)
	{
		pWOpp = m_pCurWOpp;
		if(pWOpp)
		{
			//Check it is consistent with wing and polar
			if(pWOpp->m_WingName != m_pCurWing->m_WingName || pWOpp->m_PlrName!=m_pCurWPolar->m_PlrName) pWOpp=NULL;
		}
	}
	else         pWOpp = GetWOpp(Alpha);

	m_bResetglOpp    = true;
	m_bResetglWake   = true;
	m_bResetglStream = true;
	m_bResetglFlow   = true;
	m_bResetglLegend = true;

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
		if(m_pCurWPolar->m_Type != 4) strong = QString("%1").arg(m_pCurWOpp->m_Alpha,8,'f',2);
		else                          strong = QString("%1").arg(m_pCurWOpp->m_QInf,8,'f',2);

		int pos = pMainFrame->m_pctrlWOpp->findText(strong);
		if(pos >=0)
		{
			pMainFrame->m_pctrlWOpp->setCurrentIndex(pos);
		}
		else
		{
			pMainFrame->m_pctrlWOpp->setCurrentIndex(0);
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
	else            return true;
}


void QMiarex::SetWGraphTitles(Graph* pGraph)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
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
			pGraph->SetXTitle(tr("Cl"));
			break;
		case 2:
			pGraph->SetXTitle(tr("VCd"));
			break;
		case 3:
			pGraph->SetXTitle(tr("ICd"));
			break;
		case 4:
			pGraph->SetXTitle(tr("Cd"));
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
			pGraph->SetXTitle(tr("Rm"));// Total Rolling moment coef.
			break;
		case 9:
			pGraph->SetXTitle(tr("Ym"));// Total Yawing moment coef.
			break;
		case 10:
			pGraph->SetXTitle(tr("VYm"));// Profile yawing moment
			break;
		case 11:
			pGraph->SetXTitle(tr("IYm"));// Induced yawing moment
			break;
		case 12:
			pGraph->SetXTitle(tr("Cl/Cd"));
			break;
		case 13:
			pGraph->SetXTitle(tr("Cl^(3/2)/Cd"));
			break;
		case 14:
			pGraph->SetXTitle(tr("1/Rt(Cl)"));
			break;
		case 15:
			if(pMainFrame->m_ForceUnit==0)	pGraph->SetXTitle(tr("Lift (N)"));
			else pGraph->SetXTitle(tr("Lift (lbf)"));
			break;
		case 16:
			if(pMainFrame->m_ForceUnit==0)	pGraph->SetXTitle(tr("Drag (N)"));
			else pGraph->SetXTitle(tr("Drag (lbf)"));
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
			pGraph->SetXTitle(tr("PM"));
			break;
		case 22:
			pGraph->SetXTitle(tr("RM"));
			break;
		case 23:
			pGraph->SetXTitle(tr("YM"));
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
			pGraph->SetYTitle(tr("Cl"));
			break;
		case 2:
			pGraph->SetYTitle(tr("VCd"));
			break;
		case 3:
			pGraph->SetYTitle(tr("ICd"));
			break;
		case 4:
			pGraph->SetYTitle(tr("Cd"));
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
			pGraph->SetYTitle(tr("Rm"));// Total Rolling moment coef.
			break;
		case 9:
			pGraph->SetYTitle(tr("Ym"));// Total Yawing moment coef.
			break;
		case 10:
			pGraph->SetYTitle(tr("VYm"));// Profile yawing moment
			break;
		case 11:
			pGraph->SetYTitle(tr("IYm"));// Induced yawing moment
			break;
		case 12:
			pGraph->SetYTitle(tr("Cl/Cd"));
			break;
		case 13:
			pGraph->SetYTitle(tr("Cl^(3/2)/Cd"));
			break;
		case 14:
			pGraph->SetYTitle(tr("1/Rt(Cl)"));
			break;
		case 15:
			if(pMainFrame->m_ForceUnit==0)	pGraph->SetYTitle(tr("Lift (N)"));
			else pGraph->SetYTitle(tr("Lift (lbf)"));
			break;
		case 16:
			if(pMainFrame->m_ForceUnit==0)	pGraph->SetYTitle(tr("Drag (N)"));
			else pGraph->SetYTitle(tr("Drag (lbf)"));
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
			pGraph->SetYTitle(tr("PM"));
			break;
		case 22:
			pGraph->SetYTitle(tr("RM"));
			break;
		case 23:
			pGraph->SetYTitle(tr("YM"));
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

	QSize size(m_rCltRect.width(),m_rCltRect.height());
	GLWidget * pGLWidget = (GLWidget*)m_pGLWidget;
	QGLFormat GLFormat = pGLWidget->format();

	if(!GLFormat.rgba())
	{
		QMessageBox::warning(this,tr("Warning"),tr("OpenGL color format is not recognized... Sorry"));
		return;
	}

	bitsPerPixel = 24;
	int width = size.width();
	switch(bitsPerPixel)
	{
		case 8:
		{
			QMessageBox::warning(this,tr("Warning"),tr("Cannot (yet ?) save 8 bit depth opengl screen images... Sorry"));
			return;
		}
		case 16:
		{
			QMessageBox::warning(this,tr("Warning"),tr("Cannot (yet ?) save 16 bit depth opengl screen images... Sorry"));
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
			QMessageBox::warning(this,tr("Warning"),tr("Unidentified bit depth... Sorry"));
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
			  QMessageBox::warning(this,tr("Warning"),"The version of Qt used to compile the code is older than 4.4 and does not support 24 bit images... Sorry");
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




void QMiarex::StabAnalyze(double V0, double VMax, double VDelta, bool bSequence)
{
	if(!m_pCurWing || !m_pCurWPolar) return;
	if(m_pCurWPolar->m_Type<7)       return;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pStabDlg->m_pWPolar    = m_pCurWPolar;
	m_pStabDlg->m_MatSize    = m_MatSize;
	m_pStabDlg->m_nNodes     = m_nNodes;
	m_pStabDlg->m_NSurfaces  = m_NSurfaces;
	m_pStabDlg->m_ppSurface  = m_pSurface;

	m_pStabDlg->m_pPlane     = m_pCurPlane;
	m_pStabDlg->m_pWing      = m_pCurWing;
	m_pStabDlg->m_pWing2     = m_pCurWing2;
	m_pStabDlg->m_pStab      = m_pCurStab;
	m_pStabDlg->m_pFin       = m_pCurFin;
	m_pStabDlg->m_pWPolar    = m_pCurWPolar;
	m_pStabDlg->m_bSequence  = bSequence;
	m_pStabDlg->m_bTrefftz   = m_bTrefftz;

	m_pStabDlg->m_ControlMin    = V0;
	m_pStabDlg->m_ControlMax    = VMax;
	m_pStabDlg->m_ControlDelta  = VDelta;

	m_pStabDlg->m_MatSize    = m_MatSize;
	m_pStabDlg->m_nNodes     = m_nNodes;
	m_pStabDlg->m_NSurfaces  = m_NSurfaces;

	m_pStabDlg->move(pMainFrame->m_DlgPos);
	m_pStabDlg->InitDialog();
	m_pStabDlg->show();
	m_pStabDlg->StartAnalysis();

	if(!m_bLogFile || !m_pStabDlg->m_bWarning) m_pStabDlg->hide();

	pMainFrame->UpdateWOpps();
	if(m_pCurPlane)     SetPOpp(false, V0);
	else if(m_pCurWing) SetWOpp(false, V0);

	if(m_iView==WPOLARVIEW)      CreateWPolarCurves();
	else if(m_iView==WSTABVIEW)  CreateStabilityCurves();
	else if(m_iView==WOPPVIEW)   CreateWOppCurves();
	else if(m_iView==WCPVIEW)    CreateCpCurves();

	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
	pStabView->SetControls();
	pStabView->SetMode();

	UpdateView();
}


void QMiarex::StopAnimate()
{
	m_bAnimateWOpp = false;
	m_pctrlWOppAnimate->setChecked(false);
	m_pTimerWOpp->stop();

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	StabViewDlg *pStabView =(StabViewDlg*)pMainFrame->m_pStabView;
	m_bAnimateMode = false;
	m_pTimerMode->stop();
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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(m_iView==WPOLARVIEW && m_pCurWPolar)
	{
		m_pCurWPolar->m_Color = m_CurveColor;
		m_pCurWPolar->m_Style = m_CurveStyle;
		m_pCurWPolar->m_Width = (int)m_CurveWidth;
		CreateWPolarCurves();
	}
	if(m_iView==WSTABVIEW && m_pCurWPolar)
	{
		if(m_iStabilityView==0)
		{
			if(m_pCurPOpp)
			{
				m_pCurPOpp->m_Color = m_CurveColor;
				m_pCurPOpp->m_Style = m_CurveStyle;
				m_pCurPOpp->m_Width = m_CurveWidth;			
			}
			if(m_pCurWOpp)
			{
				m_pCurWOpp->m_Color = m_CurveColor;
				m_pCurWOpp->m_Style = m_CurveStyle;
				m_pCurWOpp->m_Width = m_CurveWidth;
			}
		}
		else if(m_iStabilityView==1)
		{
			m_pCurWPolar->m_Color = m_CurveColor;
			m_pCurWPolar->m_Style = m_CurveStyle;
			m_pCurWPolar->m_Width = m_CurveWidth;
		}
		CreateStabilityCurves();
	}
	else if (m_iView==WOPPVIEW)
	{
		if(m_pCurPOpp)
		{
			m_pCurPOpp->m_Color = m_CurveColor;
			m_pCurPOpp->m_Style = m_CurveStyle;
			m_pCurPOpp->m_Width = m_CurveWidth;			
		}
		else if(m_pCurWOpp)
		{
			m_pCurWOpp->m_Color = m_CurveColor;
			m_pCurWOpp->m_Style = m_CurveStyle;
			m_pCurWOpp->m_Width = m_CurveWidth;
		}
		CreateWOppCurves();
	}
	else if (m_iView==WCPVIEW && m_pCurWOpp)
	{
		m_CpColor = m_CurveColor;
		m_CpStyle = m_CurveStyle;
		m_CpWidth = m_CurveWidth;
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
		SetWGraphTitles(&m_WPlrGraph1);
		SetWGraphTitles(&m_WPlrGraph2);
		SetWGraphTitles(&m_WPlrGraph3);
		SetWGraphTitles(&m_WPlrGraph4);
	}
	else if(m_iView==WSTABVIEW)
	{
		CreateStabilityCurves();
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
	}
	SetCurveParams();
	UpdateView();
}



void QMiarex::UpdateView()
{
	TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;
	GLWidget * pGLWidget = (GLWidget*)m_pGLWidget;
	if(m_iView==W3DVIEW || (m_iView==WSTABVIEW && m_iStabilityView==3))
	{
		if(pGLWidget) pGLWidget->updateGL();
	}
	else
	{
		if(p2DWidget) p2DWidget->update();
	}
}




void QMiarex::VLMAnalyze(double V0, double VMax, double VDelta, bool bSequence)
{
	if(!m_pCurWing || !m_pCurWPolar) return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pVLMDlg->m_pPlane     = m_pCurPlane;
	m_pVLMDlg->m_pWing      = m_pCurWing;
	m_pVLMDlg->m_pWing2     = m_pCurWing2;
	m_pVLMDlg->m_pStab      = m_pCurStab;
	m_pVLMDlg->m_pFin       = m_pCurFin;
	m_pVLMDlg->m_pWPolar    = m_pCurWPolar;
	m_pVLMDlg->m_bSequence  = bSequence;

	m_pVLMDlg->m_bWakeRollUp    = m_pCurWPolar->m_bWakeRollUp;
	m_pVLMDlg->m_MaxWakeIter    = m_MaxWakeIter;
	m_pVLMDlg->m_WakeInterNodes = m_WakeInterNodes;
	m_pVLMDlg->m_NWakeColumn    = m_NWakeColumn;
	m_pVLMDlg->m_nWakeNodes     = m_nWakeNodes;
	m_pVLMDlg->m_WakeSize       = m_WakeSize;
	m_pVLMDlg->m_bTrefftz       = m_bTrefftz;

	if(m_pCurWPolar->m_Type<3)
	{
		m_pVLMDlg->m_AlphaMin   = V0;
		m_pVLMDlg->m_AlphaMax   = VMax;
		m_pVLMDlg->m_AlphaDelta = VDelta;
	}
	else if(m_pCurWPolar->m_Type==4)
	{
		m_pVLMDlg->m_AlphaMin      = m_pCurWPolar->m_ASpec;
		m_pVLMDlg->m_QInfMin       = V0;
		m_pVLMDlg->m_QInfMax       = VMax;
		m_pVLMDlg->m_QInfDelta     = VDelta;
	}
	else if(m_pCurWPolar->m_Type==5 || m_pCurWPolar->m_Type==6)
	{
		m_pVLMDlg->m_AlphaMin      = m_pCurWPolar->m_ASpec;
		m_pVLMDlg->m_QInfMin       = m_pCurWPolar->m_QInf;
		m_pVLMDlg->m_ControlMin    = V0;
		m_pVLMDlg->m_ControlMax    = VMax;
		m_pVLMDlg->m_ControlDelta  = VDelta;
	}

	m_pVLMDlg->m_MatSize    = m_MatSize;
	m_pVLMDlg->m_nNodes     = m_nNodes;
	m_pVLMDlg->m_NSurfaces  = m_NSurfaces;

	m_pVLMDlg->move(pMainFrame->m_DlgPos);
	m_pVLMDlg->InitDialog();
	m_pVLMDlg->show();
	m_pVLMDlg->StartAnalysis();

//	if(m_bLogFile && m_pVLMDlg->m_bWarning) pMainFrame->OnLogFile();
	if(!m_bLogFile || !m_pVLMDlg->m_bWarning) m_pVLMDlg->hide();

	pMainFrame->m_DlgPos = m_pVLMDlg->pos();
	pMainFrame->UpdateWOpps();
	if(m_pCurPlane)     SetPOpp(false, V0);
	else if(m_pCurWing) SetWOpp(false, V0);

	if(m_iView==WPOLARVIEW) CreateWPolarCurves();

	UpdateView();
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
	QPoint pt(event->x(), event->y()); //client coordinates

	if(m_iView==W3DVIEW || (m_iView==WSTABVIEW&&m_iStabilityView==3))
	{
		if(m_pCurWing)
		{
			//zoom wing
			if(event->delta()>0) m_glScaled /= (GLfloat)1.06;
			else                 m_glScaled *= (GLfloat)1.06;
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
				if(event->delta()>0) m_pCurGraph->Scalex(1.06);
				else                 m_pCurGraph->Scalex(1.0/1.06);
			}
			else if(m_bYPressed)
			{
				//zoom y scale
				m_pCurGraph->SetAutoY(false);
				if(event->delta()>0) m_pCurGraph->Scaley(1.06);
				else                 m_pCurGraph->Scaley(1.0/1.06);
			}
			else
			{
				//zoom both
				m_pCurGraph->SetAuto(false);
				if(event->delta()>0) m_pCurGraph->Scale(1.06);
				else                 m_pCurGraph->Scale(1.0/1.06);
			}

			m_pCurGraph->SetAutoXUnit();
			m_pCurGraph->SetAutoYUnit();
			UpdateView();

		}
		else if(m_pCurWing && m_iView==WOPPVIEW)
		{
			if(event->delta()<0) m_WingScale *= 1.06;
			else                 m_WingScale /= 1.06;
			UpdateView();
		}
	}
}




