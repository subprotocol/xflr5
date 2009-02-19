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


#include "Miarex.h"
#include "../MainFrame.h"
#include "../TwoDWidget.h"
#include "../Globals.h"
#include "../Misc/RenameDlg.h"
#include "../Graph/GraphVariableDlg.h"
#include "WingDlg.h"
#include "PlaneDlg.h"
#include "WPolarDlg.h"
#include "LLTAnalysisDlg.h"
#include "VLMAnalysisDlg.h"

#include <math.h>

 

QMiarex::QMiarex(QWidget *parent)
	: QWidget(parent)
{
	//construct evrything
	pi = 3.141592654;

	m_hcArrow.setShape(Qt::ArrowCursor);
	m_hcCross.setShape(Qt::CrossCursor);
	m_hcMove.setShape(Qt::ClosedHandCursor);

	m_pMainFrame  = NULL;
	m_pXFile      = NULL;
        m_pPanelDlg     = NULL;
        m_pVLMDlg     = NULL;

	m_pCurPlane   = NULL;
	m_pCurWing    = NULL;
	m_pCurWing2   = NULL;
	m_pCurStab    = NULL;
	m_pCurFin     = NULL;
	m_pCurPOpp    = NULL;
	m_pCurFrame   = NULL;
	m_pCurBody    = NULL;
	m_pCurWPolar  = NULL;
	m_pCurWOpp    = NULL;
	m_pCurGraph   = &m_WingGraph1;

	QRect r(100,100, 700, 600);
	m_GL3dView.setGeometry(r);
	m_GL3dView.SetPointers(m_pCurWing, m_pCurWing2, m_pCurStab, m_pCurFin, m_pCurBody, m_pCurPlane, m_pCurWPolar, m_pCurWOpp, m_pCurPOpp);
	m_GL3dView.m_pPanel = m_Panel;
	m_GL3dView.m_pNode = m_Node;

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

	m_nWakeNodes = 0;
	m_WakeSize   = 0;
	m_LastPoint.setX(0);
	m_LastPoint.setY(0);
	m_WPlrLegendOffset.setX(0);
	m_WPlrLegendOffset.setY(0);

	m_rCltRect.setWidth(0);
	m_rCltRect.setHeight(0);
//	m_rDrawRect.setWidth(0);
//	m_rDrawRect.setHeight(0);
	m_rSingleRect.setWidth(0);
	m_rSingleRect.setHeight(0);

	m_MinPanelSize = 0.0001; //.1 mm

	m_LastWOpp      = 0.0;
	m_CurSpanPos    = 0.0;
	m_WingScale     = 1.0;

	m_AlphaMin = 0.0;
	m_AlphaMax = 1.0;
	m_AlphaDelta = 0.5;
	m_QInfMin = 10.0;
	m_QInfMax = 50.0;
	m_QInfDelta = 10.0;
	m_ControlMin = 0.0;
	m_ControlMax = 1.0;
	m_ControlDelta = 0.1;


	m_NXPoints    = 30;
	m_NHoopPoints = 20;


	m_ptOffset.rx() = 0;
	m_ptOffset.ry() = 0;

	m_InducedDragPoint = 0;

	CPanel::s_pNode = m_Node;
//	CWing::s_pLLTDlg     = &m_LLTDlg;    //pointer to the VLM analysis dialog class
//	CWing::s_p3DPanelDlg = &m_PanelDlg;  //pointer to the 3DPanel analysis dialog class

	CWing::m_pWakePanel = m_WakePanel;
	CWing::m_pWakeNode = m_WakeNode;

	CSurface::s_pPanel = m_Panel;
	CSurface::s_pNode  = m_Node;

	GL3dViewDlg::s_pGLLightDlg = &m_GLLightDlg;
	GL3dBodyDlg::s_pGLLightDlg = &m_GLLightDlg;

	m_GLList = 0;

	m_bDirichlet         = true;
	m_bTrefftz           = true;

	m_bXTop              = true;
	m_bXBot              = true;
	m_bXCP               = true;
	m_bICd               = true;
	m_bVCd               = true;

	m_bXCmRef            = true;
	m_bLogFile           = false;
	m_bHalfWing          = true;
	m_bTransGraph        = true;
//	m_bIsPrinting        = false;
	m_bCurWOppOnly       = true;
	m_bStoreWOpp         = true;
	m_bKeepOutOpps       = false;
	m_bCurFrameOnly      = true;
	m_bTrans             = false;
	m_bDragPoint         = false;
	m_bType1             = true;
	m_bType2             = true;
	m_bType4             = true;
	m_bType5             = true;
	m_bType6             = true;
	m_bShowElliptic      = false;
	m_bShowWing2         = true;
	m_bShowStab          = true;
	m_bShowFin           = true;
	m_bIs2DScaleSet      = false;
	m_bAutoScales        = false;
	m_bAnimate           = false;
	m_bAnimatePlus       = true;

	m_posAnimate         = 0;

	m_pCurWingGraph = &m_WingGraph1;
	m_pCurWPlrGraph = &m_WPlrGraph1;

	m_WingGraph1.SetAutoX(false);
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

	m_WingGraph2.SetAutoX(false);
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

	m_WingGraph3.SetAutoX(false);
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

	m_WingGraph4.SetAutoX(false);
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
	m_WPlrGraph1.SetXTitle("Cd");
	m_WPlrGraph1.SetYTitle("Cl");
	m_WPlrGraph1.SetXMin(-0.0);
	m_WPlrGraph1.SetXMax( 0.1);
	m_WPlrGraph1.SetYMin(-0.01);
	m_WPlrGraph1.SetYMax( 0.01);
	m_WPlrGraph1.SetType(0);
	m_WPlrGraph1.SetMargin(50);

	m_WPlrGraph2.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_WPlrGraph2.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_WPlrGraph2.SetXTitle("Alpha");
	m_WPlrGraph2.SetYTitle("Cl");
	m_WPlrGraph2.SetXMin(-0.0);
	m_WPlrGraph2.SetXMax( 0.1);
	m_WPlrGraph2.SetYMin(-0.01);
	m_WPlrGraph2.SetYMax( 0.01);
	m_WPlrGraph2.SetType(0);
	m_WPlrGraph2.SetMargin(50);

	m_WPlrGraph3.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_WPlrGraph3.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_WPlrGraph3.SetXTitle("Alpha");
	m_WPlrGraph3.SetYTitle("Cl");
	m_WPlrGraph3.SetXMin(-0.0);
	m_WPlrGraph3.SetXMax( 0.1);
	m_WPlrGraph3.SetYMin(-0.01);
	m_WPlrGraph3.SetYMax( 0.01);
	m_WPlrGraph3.SetType(0);
	m_WPlrGraph3.SetMargin(50);

	m_WPlrGraph4.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_WPlrGraph4.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_WPlrGraph4.SetXTitle("Alpha");
	m_WPlrGraph4.SetYTitle("Cl");
	m_WPlrGraph4.SetXMin(-0.0);
	m_WPlrGraph4.SetXMax( 0.1);
	m_WPlrGraph4.SetYMin(-0.01);
	m_WPlrGraph4.SetYMax( 0.01);
	m_WPlrGraph4.SetType(0);
	m_WPlrGraph4.SetMargin(50);

	m_CpGraph.SetInverted(true);
	m_CpGraph.SetXMajGrid(true, QColor(120,120,120),2,1);
	m_CpGraph.SetYMajGrid(true, QColor(120,120,120),2,1);
	m_CpGraph.SetXTitle("x");
	m_CpGraph.SetYTitle("Cp");
	m_CpGraph.SetXMin( 0.0);
	m_CpGraph.SetXMax( 0.1);
	m_CpGraph.SetYMin(-0.01);
	m_CpGraph.SetYMax( 0.01);
	m_CpGraph.SetType(0);
	m_CpGraph.SetMargin(50);
	for(int i=0; i<4;i++) m_CpGraph.AddCurve(); // four curves for wing, wing2, stab and fin


	m_WOppVar1 = 0;
	m_WOppVar2 = 1;
	m_WOppVar3 = 2;
	m_WOppVar4 = 2;

	m_CpColor = QColor(255,100,150);
	m_CpStyle = 0;
	m_CpWidth = 1;
	m_bShowCpPoints = false;
	m_bShowCp       = true;

	m_XW1    =  4;
	m_YW1    =  1;
	m_XW2    =  0;
	m_YW2    =  1;
	m_XW3    =  0;
	m_YW3    = 11;
	m_XW4    =  0;
	m_YW4    = 10;

	m_iView     = 1;
	m_iWingView = 1;
	m_iWPlrView = 4;

	m_nNodes  = 0;
	m_MatSize = 0;

	m_WingColor =  QColor(0,130,130);
	m_StabColor =  QColor(30,50, 100);
	m_FinColor  =  QColor(60,60,180);
/*	m_BodyGridDlg.m_bGrid = false;
	m_BodyGridDlg.m_Color = QColor(200,200,200);
	m_BodyGridDlg.m_Style = 1DOT;
	m_BodyGridDlg.m_Width = 1;
	m_BodyGridDlg.m_Unit  = 0.1;

	m_BodyGridDlg.m_bMinGrid   = false;
	m_BodyGridDlg.m_MinColor   = QColor(70,70,70);
	m_BodyGridDlg.m_MinStyle   = 1;
	m_BodyGridDlg.m_MinWidth   = 1;
	m_BodyGridDlg.m_MinorUnit  = 0.01;*/

//	m_BodyGridDlg.m_bScale       = false;



/*	m_pPanelDlg->m_pPanel        = m_Panel;
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

	m_FlowLinesDlg.Create(IDD_FLOWLINESDLG,this);
	m_FlowLinesDlg.SetWindowPos(this, 20,80,0,0,SWP_NOSIZE);

	m_GLLightDlg.m_pMiarex = this;*/

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

	m_WingGraph1.SetGraphName("Wing Graph 1");
	m_WingGraph2.SetGraphName("Wing Graph 2");
	m_WingGraph3.SetGraphName("Wing Graph 3");
	m_WingGraph4.SetGraphName("Wing Graph 4");

	m_WPlrGraph1.SetGraphName("Wing Polar Graph 1");
	m_WPlrGraph2.SetGraphName("Wing Polar Graph 2");
	m_WPlrGraph3.SetGraphName("Wing Polar Graph 3");
	m_WPlrGraph4.SetGraphName("Wing Polar Graph 4");

	m_CpGraph.SetGraphName("Cp Graph");

	m_VLMStyle    = 0;
	m_VLMWidth    = 1;
	m_VLMColor    = QColor(100,100,100);

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

	SetupLayout();

	m_pctrlHalfWing->setChecked(m_bHalfWing);
	m_pctrlTrans->setChecked(m_bXTop);
	m_pctrlIDrag->setChecked(m_bICd);
	m_pctrlVDrag->setChecked(m_bVCd);
	m_pctrlLift->setChecked(m_bXCP);

	connect(m_pctrlSequence, SIGNAL(stateChanged(int)), this, SLOT(OnSequence(int)));
	connect(m_pctrlStoreWOpp, SIGNAL(stateChanged(int)), this, SLOT(OnStoreWOpp(int)));
	connect(m_pctrlInitLLTCalc, SIGNAL(stateChanged(int)), this, SLOT(OnInitLLTCalc(int)));
	connect(m_pctrlAnalyze, SIGNAL(clicked()), this, SLOT(OnAnalyze()));
	connect(m_pctrlCurveStyle, SIGNAL(activated(int)), this, SLOT(OnCurveStyle(int)));
	connect(m_pctrlCurveWidth, SIGNAL(activated(int)), this, SLOT(OnCurveWidth(int)));
	connect(m_pctrlCurveColor, SIGNAL(clicked()), this, SLOT(OnCurveColor()));
	connect(m_pctrlShowPoints, SIGNAL(clicked(bool)), this, SLOT(OnShowPoints(bool)));
	connect(m_pctrlShowCurve, SIGNAL(clicked(bool)), this, SLOT(OnShowCurve(bool)));

	connect(m_pctrlHalfWing, SIGNAL(stateChanged(int)), this, SLOT(OnHalfWing(int)));
	connect(m_pctrlLift, SIGNAL(stateChanged(int)), this, SLOT(OnShowLift(int)));
	connect(m_pctrlIDrag, SIGNAL(stateChanged(int)), this, SLOT(OnShowIDrag(int)));
	connect(m_pctrlVDrag, SIGNAL(stateChanged(int)), this, SLOT(OnShowVDrag(int)));
	connect(m_pctrlTrans, SIGNAL(stateChanged(int)), this, SLOT(OnShowTransitions(int)));
	connect(m_pctrlAnimate, SIGNAL(stateChanged(int)), this, SLOT(OnAnimate(int)));
}



QMiarex::~QMiarex()
{
}



void QMiarex::SetupLayout()
{
	m_pctrlSequence = new QCheckBox("Sequence");
	QGridLayout *SequenceGroup = new QGridLayout;
	QLabel *AlphaMinLab   = new QLabel("Start=");
	QLabel *AlphaMaxLab   = new QLabel("End=");
	QLabel *AlphaDeltaLab = new QLabel("D=");
	AlphaDeltaLab->setFont(QFont("Symbol"));
	AlphaDeltaLab->setAlignment(Qt::AlignRight);
	AlphaMinLab->setAlignment(Qt::AlignRight);
	AlphaMaxLab->setAlignment(Qt::AlignRight);
//	AlphaMinLab->setMaximumWidth(40);
//	AlphaMaxLab->setMaximumWidth(40);
//	AlphaDeltaLab->setMaximumWidth(40);
	m_pctrlAlphaMin     = new FloatEdit();
	m_pctrlAlphaMax     = new FloatEdit();
	m_pctrlAlphaDelta   = new FloatEdit();
//	m_pctrlAlphaMin->setMaximumWidth(70);
//	m_pctrlAlphaMax->setMaximumWidth(70);
//	m_pctrlAlphaDelta->setMaximumWidth(70);
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

	m_pctrlInitLLTCalc = new QCheckBox("Init LLT");
	m_pctrlStoreWOpp    = new QCheckBox("Store Opp");
	m_pctrlAnalyze     = new QPushButton("Analyze");

	QHBoxLayout *AnalysisSettings = new QHBoxLayout;
	AnalysisSettings->addWidget(m_pctrlInitLLTCalc);
	AnalysisSettings->addWidget(m_pctrlStoreWOpp);

	QVBoxLayout *AnalysisGroup = new QVBoxLayout;
	AnalysisGroup->addWidget(m_pctrlSequence);
	AnalysisGroup->addLayout(SequenceGroup);
	AnalysisGroup->addStretch(1);
	AnalysisGroup->addLayout(AnalysisSettings);
	AnalysisGroup->addWidget(m_pctrlAnalyze);

	QGroupBox *AnalysisBox = new QGroupBox("Analysis settings");
	AnalysisBox->setLayout(AnalysisGroup);


	QVBoxLayout *DisplayGroup = new QVBoxLayout;
	QGridLayout *CheckDispLayout = new QGridLayout;
	m_pctrlHalfWing      = new QCheckBox("1/2 wing");
	m_pctrlLift          = new QCheckBox("Lift");
	m_pctrlIDrag         = new QCheckBox("Ind. Drag");
	m_pctrlVDrag         = new QCheckBox("Visc. Drag");
	m_pctrlTrans         = new QCheckBox("Trans.");
	m_pctrlAnimate       = new QCheckBox("Anim.");
	CheckDispLayout->addWidget(m_pctrlHalfWing, 1, 1);
	CheckDispLayout->addWidget(m_pctrlLift,     1, 2);
	CheckDispLayout->addWidget(m_pctrlIDrag,    2, 1);
	CheckDispLayout->addWidget(m_pctrlVDrag,    2, 2);
	CheckDispLayout->addWidget(m_pctrlTrans,    3, 1);
	CheckDispLayout->addWidget(m_pctrlAnimate,  3, 2);

	m_pctrlAnimateSpeed  = new QSlider(Qt::Horizontal);
	m_pctrlAnimateSpeed->setMinimum(0);
	m_pctrlAnimateSpeed->setMaximum(500);
	m_pctrlAnimateSpeed->setSliderPosition(250);
	m_pctrlAnimateSpeed->setTickInterval(25);
	m_pctrlAnimateSpeed->setTickPosition(QSlider::TicksBelow);
	DisplayGroup->addLayout(CheckDispLayout);
	DisplayGroup->addWidget(m_pctrlAnimateSpeed);
	QGroupBox *DisplayBox = new QGroupBox("Display");
	DisplayBox->setLayout(DisplayGroup);

	QVBoxLayout *CurveGroup = new QVBoxLayout;
	m_pctrlShowCurve  = new QCheckBox("Show Curve");
	m_pctrlShowPoints = new QCheckBox("Show Points");
	m_pctrlCurveStyle = new LineCbBox();
	m_pctrlCurveWidth = new LineCbBox();
	m_pctrlCurveColor = new LineButton;
	for (int i=0; i<5; i++)
	{
		m_pctrlCurveStyle->addItem("item");
		m_pctrlCurveWidth->addItem("item");
	}
	m_pStyleDelegate = new LineDelegate;
	m_pWidthDelegate = new LineDelegate;
	m_pctrlCurveStyle->setItemDelegate(m_pStyleDelegate);
	m_pctrlCurveWidth->setItemDelegate(m_pWidthDelegate);

	CurveGroup->addWidget(m_pctrlShowCurve);
	CurveGroup->addWidget(m_pctrlShowPoints);
	CurveGroup->addWidget(m_pctrlCurveStyle);
	CurveGroup->addWidget(m_pctrlCurveWidth);
	CurveGroup->addWidget(m_pctrlCurveColor);
	QGroupBox *CurveBox = new QGroupBox("Curve settings");
	CurveBox->setLayout(CurveGroup);


	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addStretch(1);
	mainLayout->addWidget(AnalysisBox);
	mainLayout->addStretch(1);
	mainLayout->addWidget(DisplayBox);
	mainLayout->addStretch(1);
	mainLayout->addWidget(CurveBox);
	mainLayout->addStretch(1);

	setLayout(mainLayout);
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
			//make it simple for now
			delete pBody; return NULL;

			//Ask for user intentions

/*			if(SetModBody(pBody))
			{
				m_poaBody->append(pBody);
				return pBody;
			}
			else
			{
				delete pBody;
				return NULL;
			}*/

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
		if(pPOpp == NULL)
		{
			return;
		}

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

		if(m_pCurPlane->m_bBody && m_pCurPlane->m_pBody) m_pCurPlane->m_pBody->m_bLocked = true;

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

		if(m_pCurWPolar->m_AnalysisType==2)
		{
                        pPOpp->m_NPanels          = m_pVLMDlg->m_MatSize;
                        pPOpp->m_Alpha            = m_pVLMDlg->m_OpAlpha;
                        pPOpp->m_QInf             = m_pVLMDlg->m_QInfMin;
			if(m_pCurWPolar->m_Type>=5)
			{
				pPOpp->m_Ctrl = m_pVLMDlg->m_Ctrl;
				pWOpp->m_Ctrl = m_pVLMDlg->m_Ctrl;
			}
			else
			{
				pPOpp->m_Ctrl = 0.0;
				pWOpp->m_Ctrl = 0.0;
			}

                        pWOpp->m_Alpha          = m_pVLMDlg->m_OpAlpha;
                        pWOpp->m_QInf           = m_pVLMDlg->m_QInfMin;
                        pWOpp->m_CL             = m_pVLMDlg->m_CL;
                        pWOpp->m_CX             = m_pVLMDlg->m_CX;
                        pWOpp->m_CY             = m_pVLMDlg->m_CY;
                        pWOpp->m_InducedDrag    = m_pVLMDlg->m_InducedDrag;
                        pWOpp->m_ViscousDrag    = m_pVLMDlg->m_ViscousDrag;

			pWOpp->m_GCm                 = m_pVLMDlg->m_GCm;
			pWOpp->m_GRm                 = m_pVLMDlg->m_GRm;
			pWOpp->m_GYm                 = m_pVLMDlg->m_GYm;
			pWOpp->m_VYm                 = m_pVLMDlg->m_VYm;
			pWOpp->m_IYm                 = m_pVLMDlg->m_IYm;

			pWOpp->m_XCP                 = m_pVLMDlg->m_XCP;
			pWOpp->m_YCP                 = m_pVLMDlg->m_YCP;

			pWOpp->m_Beta	             = m_pCurWPolar->m_Beta;
		}
		else if(m_pCurWPolar->m_AnalysisType==3)
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
                        pWOpp->m_GRm                 = m_pPanelDlg->m_GRm;
                        pWOpp->m_GYm                 = m_pPanelDlg->m_GYm;
                        pWOpp->m_VYm                 = m_pPanelDlg->m_VYm;
                        pWOpp->m_IYm                 = m_pPanelDlg->m_IYm;

                        pWOpp->m_XCP                 = m_pPanelDlg->m_XCP;
                        pWOpp->m_YCP                 = m_pPanelDlg->m_YCP;

			pWOpp->m_Beta                = m_pCurWPolar->m_Beta;
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
		for(i=0; i<m_pCurWing->m_MatSize; i++)
		{
			pPOpp->m_WingWOpp.m_Cp[p] = pPOpp->m_Cp[p];
			p++;
		}
		if(m_pCurWing2)
		{
			for(i=0; i<m_pCurWing2->m_MatSize; i++)
			{
				pPOpp->m_Wing2WOpp.m_Cp[i] = pPOpp->m_Cp[p];
				p++;
			}
			memcpy(pPOpp->m_Wing2WOpp.m_Vd,       m_pCurWing2->m_Vd,  sizeof(pPOpp->m_Wing2WOpp.m_Vd));
		}
		if(m_pCurStab)
		{
			for(i=0; i<m_pCurStab->m_MatSize; i++)
			{
				pPOpp->m_StabWOpp.m_Cp[i] = pPOpp->m_Cp[p];
				p++;
			}
			memcpy(pPOpp->m_StabWOpp.m_Vd,        m_pCurStab->m_Vd,  sizeof(pPOpp->m_StabWOpp.m_Vd));
		}
		if(m_pCurFin)
		{
			for(i=0; i<m_pCurFin->m_MatSize; i++)
			{
				pPOpp->m_FinWOpp.m_Cp[i] = pPOpp->m_Cp[p];
				p++;
			}
			memcpy(pPOpp->m_FinWOpp.m_Vd,         m_pCurFin->m_Vd,  sizeof(pPOpp->m_FinWOpp.m_Vd));
		}

		m_pCurWPolar->AddPoint(pPOpp);

		m_pCurWOpp = &pPOpp->m_WingWOpp;
		if(m_iView==2)
		{
			CreateWPolarCurves();
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
		m_pCurPOpp = pPOpp;
		m_pCurWOpp = &m_pCurPOpp->m_WingWOpp;
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
			//Keep it simple for now
			delete pWing;
			return NULL;
/*			if(SetModWing(pWing))
			{
				m_poaWing->append(pWing);
				return pWing;
			}
			else{
				delete pWing;
				return NULL;
			}*/

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
		QMessageBox::warning(window(),"QFLR5","Not enough memory to store the OpPoint\n");
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

		pNewPoint->m_Span                = m_pCurWing->m_Span;
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
			pNewPoint->m_Alpha               = m_pCurWing->m_Alpha;
			pNewPoint->m_QInf                = m_pCurWing->m_QInf;
			pNewPoint->m_NVLMPanels          = m_pCurWing->m_MatSize;
			pNewPoint->m_bOut                = m_pCurWing->m_bWingOut;
			pNewPoint->m_CL                  = m_pCurWing->m_CL;
//			pNewPoint->m_CY                  = m_pCurWing->m_CY;
//			pNewPoint->m_CX                  = m_pCurWing->m_CX;
			pNewPoint->m_InducedDrag         = m_pCurWing->m_InducedDrag;

			pNewPoint->m_GCm                 = m_pCurWing->m_GCm;
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
				pNewPoint->m_CmXRef[l]        =  m_pCurWing->m_CmXRef[m_NStation-l];
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
		else if(m_pCurWPolar->m_AnalysisType==2)
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
			memcpy(pNewPoint->m_CmXRef,     m_pCurWing->m_CmXRef,     sizeof(m_pCurWing->m_CmXRef));
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
		else if(m_pCurWPolar->m_AnalysisType==3)
		{
/*			pNewPoint->m_Alpha               = m_pPanelDlg->m_OpAlpha;
                        pNewPoint->m_QInf                = m_pPanelDlg->m_QInf;
                        pNewPoint->m_NVLMPanels          = m_pPanelDlg->m_MatSize;
                        pNewPoint->m_bOut                = m_pPanelDlg->m_bPointOut;
                        pNewPoint->m_CL                  = m_pPanelDlg->m_CL;
                        pNewPoint->m_CY                  = m_pPanelDlg->m_CY;
                        pNewPoint->m_CX                  = m_pPanelDlg->m_CX;
                        pNewPoint->m_InducedDrag         = m_pPanelDlg->m_InducedDrag;
                        pNewPoint->m_ViscousDrag         = m_pPanelDlg->m_ViscousDrag;

                        pNewPoint->m_GCm                 = m_pPanelDlg->m_GCm;
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
			memcpy(pNewPoint->m_CmXRef,        m_pCurWing->m_CmXRef, sizeof(m_pCurWing->m_CmXRef));
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
				pNewPoint->m_FirstWakePanel = m_pCurWing->m_Span;
				pNewPoint->m_WakeFactor     = 1.0;
			}
			for (i=0; i<m_pCurWing->m_nFlaps; i++)
				pNewPoint->m_FlapMoment[i] = m_pCurWing->m_FlapMoment[i];

			pNewPoint->m_nFlaps = m_pCurWing->m_nFlaps;*/
		}
		pNewPoint->m_MaxBending = Cb;
	}

	m_pCurWPolar->AddPoint(pNewPoint);
	if(m_iView==2)
	{
		CreateWPolarCurves();
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
		m_pCurWOpp = pNewPoint;
	}
	else
	{
		delete pNewPoint;
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



int QMiarex::CreateBodyElements(CPanel *pPanel)
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
	int FullSize;

	lnx = 0;

	if(m_iView!=5 && m_pCurPlane && m_pCurPlane->m_pBody)
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
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

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

		SpanInc = -m_pCurWing->m_Span/2.0;
		for (p=0; p<m_pCurWing->m_MatSize; p++)
		{
			if(m_pCurWing->m_pPanel[p].m_bIsTrailing && m_pCurWing->m_pPanel[p].m_iPos<=0)
			{
				SpanInc += m_pCurWing->m_pPanel[p].GetWidth();
//TRACE("%10.3f     %10.3f\n", SpanPos, SpanInc);
				if(SpanPos<=SpanInc || fabs(SpanPos-SpanInc)/m_pCurWing->m_Span<0.001)
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

			SpanInc = -m_pCurWing2->m_Span/2.0;
			for (p=0; p<m_pCurWing2->m_MatSize; p++)
			{
				if(m_pCurWing2->m_pPanel[p].m_bIsTrailing && m_pCurWing2->m_pPanel[p].m_iPos<=0)
				{
					SpanInc += m_pCurWing2->m_pPanel[p].GetWidth();
					if(SpanPos<=SpanInc || fabs(SpanPos-SpanInc)/m_pCurWing2->m_Span<0.001)
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
				pCurve->SetTitle("Wing2_"+str2+str3);

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

			SpanInc = -m_pCurStab->m_Span/2.0;
			for (p=0; p<m_pCurStab->m_MatSize; p++)
			{
				if(m_pCurStab->m_pPanel[p].m_bIsTrailing && m_pCurStab->m_pPanel[p].m_iPos<=0)
				{
					SpanInc += m_pCurStab->m_pPanel[p].GetWidth();
					if(SpanPos<=SpanInc || fabs(SpanPos-SpanInc)/m_pCurStab->m_Span<0.001)
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
				pCurve->SetTitle("Elevator_"+str2+str3);

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

			SpanInc = -m_pCurFin->m_Span/2.0;
			for (p=0; p<m_pCurFin->m_MatSize; p++)
			{
				if(m_pCurFin->m_pPanel[p].m_bIsTrailing && m_pCurFin->m_pPanel[p].m_iPos<=0)
				{
					SpanInc += m_pCurFin->m_pPanel[p].GetWidth();
					if(SpanPos<=SpanInc || fabs(SpanPos-SpanInc)/m_pCurFin->m_Span<0.001)
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
				pCurve->SetTitle("Fin_"+str2+str3);

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
	CVector N, LATB, TALB;
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
			if(m_pCurWPolar->m_AnalysisType==2 || m_pCurWPolar->m_bThinSurfaces)
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
				if(m_pCurWPolar->m_AnalysisType==2 || m_pCurWPolar->m_bThinSurfaces)
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

	if(m_pCurWPolar->m_AnalysisType==2)
	{
		pWOpp->m_bOut                = m_pVLMDlg->m_bPointOut;
		pWOpp->m_Alpha               = m_pVLMDlg->m_AlphaMin;
		pWOpp->m_QInf                = m_pVLMDlg->m_OpQInf;
	}
	else
	{
//		pWOpp->m_bOut                = m_pPanelDlg->m_bPointOut;
//		pWOpp->m_Alpha               = m_pPanelDlg->m_Alpha;
//		pWOpp->m_QInf                = m_pPanelDlg->m_QInf;
	}
	pWOpp->m_Weight              = m_pCurWPolar->m_Weight;
	pWOpp->m_Span                = pWing->m_Span;
	pWOpp->m_MAChord             = pWing->m_MAChord;
	pWOpp->m_CL                  = pWing->m_CL;
	pWOpp->m_InducedDrag         = pWing->m_InducedDrag;
	pWOpp->m_ViscousDrag         = pWing->m_ViscousDrag;

	pWOpp->m_GCm                 = pWing->m_GCm;
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
	memcpy(pWOpp->m_CmXRef,        pWing->m_CmXRef,        sizeof(pWing->m_CmXRef));
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
	CWOpp *pWOpp = NULL;;
	CPOpp *pPOpp = NULL;
	CCurve *pCurve1, *pCurve2, *pCurve3, *pCurve4;
	CCurve *pWing2Curve1, *pWing2Curve2, *pWing2Curve3, *pWing2Curve4;
	CCurve *pStabCurve1, *pStabCurve2, *pStabCurve3, *pStabCurve4;
	CCurve *pFinCurve1, *pFinCurve2, *pFinCurve3, *pFinCurve4;
	QString str;

	int i,k;

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

		FillWOppCurve(m_pCurWOpp, &m_WingGraph1, pCurve1, m_WOppVar1);
		FillWOppCurve(m_pCurWOpp, &m_WingGraph2, pCurve2, m_WOppVar2);
		FillWOppCurve(m_pCurWOpp, &m_WingGraph3, pCurve3, m_WOppVar3);
		FillWOppCurve(m_pCurWOpp, &m_WingGraph4, pCurve4, m_WOppVar4);

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

			FillWOppCurve(&m_pCurPOpp->m_Wing2WOpp, &m_WingGraph1, pWing2Curve1, m_WOppVar1);
			FillWOppCurve(&m_pCurPOpp->m_Wing2WOpp, &m_WingGraph2, pWing2Curve2, m_WOppVar2);
			FillWOppCurve(&m_pCurPOpp->m_Wing2WOpp, &m_WingGraph3, pWing2Curve3, m_WOppVar3);
			FillWOppCurve(&m_pCurPOpp->m_Wing2WOpp, &m_WingGraph4, pWing2Curve4, m_WOppVar4);
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

			FillWOppCurve(&m_pCurPOpp->m_StabWOpp, &m_WingGraph1, pStabCurve1, m_WOppVar1);
			FillWOppCurve(&m_pCurPOpp->m_StabWOpp, &m_WingGraph2, pStabCurve2, m_WOppVar2);
			FillWOppCurve(&m_pCurPOpp->m_StabWOpp, &m_WingGraph3, pStabCurve3, m_WOppVar3);
			FillWOppCurve(&m_pCurPOpp->m_StabWOpp, &m_WingGraph4, pStabCurve4, m_WOppVar4);
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

			FillWOppCurve(&m_pCurPOpp->m_FinWOpp, &m_WingGraph1, pFinCurve1, m_WOppVar1);
			FillWOppCurve(&m_pCurPOpp->m_FinWOpp, &m_WingGraph2, pFinCurve2, m_WOppVar2);
			FillWOppCurve(&m_pCurPOpp->m_FinWOpp, &m_WingGraph3, pFinCurve3, m_WOppVar3);
			FillWOppCurve(&m_pCurPOpp->m_FinWOpp, &m_WingGraph4, pFinCurve4, m_WOppVar4);
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

				FillWOppCurve(pWOpp, &m_WingGraph1, pCurve1, m_WOppVar1);
				FillWOppCurve(pWOpp, &m_WingGraph2, pCurve2, m_WOppVar2);
				FillWOppCurve(pWOpp, &m_WingGraph3, pCurve3, m_WOppVar3);
				FillWOppCurve(pWOpp, &m_WingGraph4, pCurve4, m_WOppVar4);
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

				FillWOppCurve(&pPOpp->m_WingWOpp, &m_WingGraph1, pCurve1, m_WOppVar1);
				FillWOppCurve(&pPOpp->m_WingWOpp, &m_WingGraph2, pCurve2, m_WOppVar2);
				FillWOppCurve(&pPOpp->m_WingWOpp, &m_WingGraph3, pCurve3, m_WOppVar3);
				FillWOppCurve(&pPOpp->m_WingWOpp, &m_WingGraph4, pCurve4, m_WOppVar4);

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

					FillWOppCurve(&pPOpp->m_Wing2WOpp, &m_WingGraph1, pWing2Curve1, m_WOppVar1);
					FillWOppCurve(&pPOpp->m_Wing2WOpp, &m_WingGraph2, pWing2Curve2, m_WOppVar2);
					FillWOppCurve(&pPOpp->m_Wing2WOpp, &m_WingGraph3, pWing2Curve3, m_WOppVar3);
					FillWOppCurve(&pPOpp->m_Wing2WOpp, &m_WingGraph4, pWing2Curve4, m_WOppVar4);
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

					FillWOppCurve(&pPOpp->m_StabWOpp, &m_WingGraph1, pStabCurve1, m_WOppVar1);
					FillWOppCurve(&pPOpp->m_StabWOpp, &m_WingGraph2, pStabCurve2, m_WOppVar2);
					FillWOppCurve(&pPOpp->m_StabWOpp, &m_WingGraph3, pStabCurve3, m_WOppVar3);
					FillWOppCurve(&pPOpp->m_StabWOpp, &m_WingGraph4, pStabCurve4, m_WOppVar4);
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

					FillWOppCurve(&pPOpp->m_FinWOpp, &m_WingGraph1, pFinCurve1, m_WOppVar1);
					FillWOppCurve(&pPOpp->m_FinWOpp, &m_WingGraph2, pFinCurve2, m_WOppVar2);
					FillWOppCurve(&pPOpp->m_FinWOpp, &m_WingGraph3, pFinCurve3, m_WOppVar3);
					FillWOppCurve(&pPOpp->m_FinWOpp, &m_WingGraph4, pFinCurve4, m_WOppVar4);
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
		if(m_WOppVar1==3)
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
		if(m_WOppVar2==3)
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
		if(m_WOppVar3==3)
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
		if(m_WOppVar4==3)
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
	CWPolar *pWPolar;

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
                         (m_bType6 && pWPolar->m_Type == 6)))
		{

			CCurve* pWPolarCurve = m_WPlrGraph1.AddCurve();
			CCurve* pWCzCurve    = m_WPlrGraph2.AddCurve();
			CCurve* pWCmCurve    = m_WPlrGraph3.AddCurve();
			CCurve* pWLDCurve    = m_WPlrGraph4.AddCurve();

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

			FillWPlrCurve(pWPolarCurve, pWPolar, m_XW1, m_YW1);
			FillWPlrCurve(pWCzCurve, pWPolar, m_XW2, m_YW2);
			FillWPlrCurve(pWCmCurve, pWPolar, m_XW3, m_YW3);
			FillWPlrCurve(pWLDCurve, pWPolar, m_XW4, m_YW4);

			pWPolarCurve->SetTitle(pWPolar->m_PlrName);
			pWCzCurve->SetTitle(pWPolar->m_PlrName);
			pWCmCurve->SetTitle(pWPolar->m_PlrName);
			pWLDCurve->SetTitle(pWPolar->m_PlrName);
		}
	}
}


void QMiarex::DrawWPolarLegend(QPainter &painter, QPoint place, int bottom)
{
	//draws the WPolar legend to the device context,
	// bottom is the lower limit not to exceed for the legend

	painter.save();

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	int LegendSize, LegendWidth, ypos;
	int i,j,k,l, ny, x1;

	QColor color;
	QFont TempFont;
	m_WPlrGraph1.GetLegendLogFont(&TempFont);

	LegendSize = 30;
	LegendWidth = 280;
	ypos = 14;


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
	painter.setFont(pMainFrame->m_TextFont);
	QPen TextPen(pMainFrame->m_TextColor);
	TextPen.setWidth(1);
	painter.setPen(TextPen);
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
                                 (pWPolar->m_Type == 6 && m_bType6)))

					UFOPlrs++;
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
					if (pWPolar->m_Alpha.size() &&	pWPolar->m_PlrName.length() &&	pWPolar->m_bIsVisible &&
                                                ((pWPolar->m_Type == 1 && m_bType1) || (pWPolar->m_Type == 2 && m_bType2) || (pWPolar->m_Type == 4 && m_bType4) || (pWPolar->m_Type == 5 && m_bType5) || (pWPolar->m_Type == 6 && m_bType6)))
					{
						//is there anything to draw ?

						LegendPen.setColor(pWPolar->m_Color);
						LegendPen.setStyle(GetStyle(pWPolar->m_Style));
						LegendPen.setWidth(pWPolar->m_Width);
						painter.setPen(LegendPen);

						painter.drawLine(place.x() + (int)(0.5*LegendSize), place.y() + (int)(1.*ypos*ny),
										 place.x() + (int)(1.3*LegendSize), place.y() + (int)(1.*ypos*ny));

						if(pWPolar->m_bShowPoints)
						{
							x1 = place.x() + (int)(0.9*LegendSize);
							painter.drawRect(x1-2, place.y() + (int)(1.*ypos*ny), 4, 4);
						}
						ny++ ;
						painter.setPen(TextPen);
						painter.drawText(place.x() + (int)(1.4*LegendSize), place.y() + (int)(1.*ypos*ny)-(int)(ypos/2), pWPolar->m_PlrName);
					}
				}
			}
			if (UFOPlrs) ny++;
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

	QColor color;
	QString strong, str1, str2, str3, str4;

	LegendSize = 30;
	LegendWidth = 260;
	ypos = 20;

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

	QPen TextPen(pMainFrame->m_TextColor);
	TextPen.setWidth(1);
	painter.setPen(TextPen);

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
		painter.setPen(LegendPen);

		painter.drawLine(place.x() + (int)(1.5*LegendSize), place.y() + (int)(1.2*ypos*ny),
						 place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.2*ypos*ny));

		if(m_pCurWOpp->m_bShowPoints)
		{
			x1 = place.x() + (int)(2.0*LegendSize);
			painter.drawRect(x1-2, place.y() + (int)(1.*ypos*ny)-2, 4, 4);
		}

		str1 = QString("V=%1").arg(m_pCurWOpp->m_QInf*pMainFrame->m_mstoUnit,5,'f',2);
		GetSpeedUnit(str2, pMainFrame->m_SpeedUnit);
		str3 = QString("_a=%1").arg(m_pCurWOpp->m_Alpha,5,'f',2);

		if(m_pCurWOpp->m_AnalysisType==1)  str4="-LLT";
		else if(m_pCurWOpp->m_AnalysisType==2)
		{
			if(m_pCurWOpp->m_bVLM1)	str4="_VLM1";
			else 					str4="_VLM2";
		}
		else if(m_pCurWOpp->m_AnalysisType==3)
		{
			str4="_Panel";
			if(m_pCurWOpp->m_bThinSurface) str4 += "_Thin";
		}

		if(m_pCurWOpp->m_bTiltedGeom) str4 += "_TG";

		if(m_pCurWOpp->m_bOut) str4+=" (Out)";

		painter.setPen(TextPen);
		painter.drawText(place.x() + (int)(3*LegendSize), place.y() + (int)(1.*ypos*ny)+(int)(ypos/2), str1+str2+str3+str4);
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
					painter.drawText(place.x() + (int)(1*LegendSize), place.y() + ypos*ny-(int)(ypos/2), str.at(k));
				}
				else
				{
				// move rigth if outside screen
					place.rx() += LegendWidth;
					ny=1;
					painter.drawText(place.x() + (int)(1*LegendSize), place.y() + ypos*ny-(int)(ypos/2), str.at(k));
				}

				bStarted = true;
				for (nc=0; nc < m_poaWOpp->size(); nc++)
				{
					pWOpp = (CWOpp*)m_poaWOpp->at(nc);
					if(str.at(k) == pWOpp->m_WingName && pWOpp->m_bIsVisible)
					{
						ny++ ;
						if(abs(bottom)<abs(place.y() + (int)(1.*ypos*ny+ypos)))
						{
							//move right
							place.rx() += LegendWidth;
							ny=2;
						}

						LegendPen.setColor(pWOpp->m_Color);
						painter.setPen(LegendPen);
						painter.drawLine(place.x() + (int)(1.5*LegendSize), place.y() + (int)(1.2*ypos*ny),
										 place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.2*ypos*ny));

						if(pWOpp->m_bShowPoints)
						{
							x1 = place.x() + (int)(2.0*LegendSize);
							painter.drawRect(x1-2, place.y() + (int)(1.*ypos*ny)-2, 4,4);
						}

						str1 = QString("V=%1").arg(pWOpp->m_QInf*pMainFrame->m_mstoUnit,5,'f',2);
						GetSpeedUnit(str2, pMainFrame->m_SpeedUnit);
						str3 = QString("_a=%1").arg(pWOpp->m_Alpha,5,'f',2);

						if(pWOpp->m_AnalysisType==1) str4 ="-LLT";
						else if(pWOpp->m_AnalysisType==2)
						{
							if(pWOpp->m_bVLM1)       str4 ="_VLM1";
							else                     str4 ="_VLM2";
						}
						else if(pWOpp->m_AnalysisType==3)
						{
							str4="_Panel";
							if(pWOpp->m_bThinSurface) str4 += "_Thin";
						}

						if(pWOpp->m_bTiltedGeom) str4 += "_TG";
						if(pWOpp->m_bOut)        str4+=" (Out)";

						painter.drawText(place.x() + (int)(3*LegendSize), place.y() + (int)(1.*ypos*ny)+(int)(ypos/2), str1+str2+str3+str4);
					}
				}

				for (nc=0; nc < m_poaPOpp->size(); nc++)
				{
					pPOpp = (CPOpp*)m_poaPOpp->at(nc);
					if(str.at(k) == pPOpp->m_PlaneName && pPOpp->m_bIsVisible)
					{
						ny++ ;
						if(abs(bottom)<abs(place.y() + (int)(1.*ypos*ny+ypos)))
						{
							//move right
							place.rx() += LegendWidth;
							ny=2;
						}


						LegendPen.setColor(pPOpp->m_Color);
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
						str3 = QString("-a=%1").arg(pPOpp->m_Alpha,5,'f',2);

						if(pPOpp->m_WingWOpp.m_AnalysisType==2)
						{
							if(pPOpp->m_bVLM1)      str4 ="-VLM1";
							else                    str4 ="-VLM2";
						}
						else if(pPOpp->m_WingWOpp.m_AnalysisType==3)
						{
							str4="-Panel";
							if(pPOpp->m_WingWOpp.m_bThinSurface) str4 += "_Thin";
						}

						if(pPOpp->m_WingWOpp.m_bTiltedGeom) str4 += "_TG";
						if(pPOpp->m_WingWOpp.m_bOut)         str4+=" (Out)";

						painter.drawText(place.x() + (int)(3*LegendSize), place.y() + (int)(1.*ypos*ny)-(int)(ypos/2), str1+str2+str3+str4);

					}
				}
				if (UFOPts) ny++;
			}
		}
	}
	painter.restore();
}


bool QMiarex::EditPlane(CPlane *pPlane)
{
	int i;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(!m_pCurPlane) return false;
	if(m_iView==5)   return false;
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
	PlaneDlg.m_pPlane = pPlane;

	pSavePlane->Duplicate(m_pCurPlane);
	PlaneDlg.m_bAcceptName = false;
	PlaneDlg.InitDialog();

	if(QDialog::Accepted == PlaneDlg.exec())
	{
		if(PlaneDlg.m_bChanged)
		{
			if(bHasResults) SetModPlane(pPlane);

			//then modifications are automatically recorded
			pMainFrame->DeletePlane(m_pCurPlane, true);// will also set new surface and Aerochord in WPolars
			m_GL3dView.m_bResetglGeom = true;
			m_GL3dView.m_bResetglMesh = true;
//			m_bResetglOpp  = true;
			if(m_iView==2)		CreateWPolarCurves();
			else if(m_iView==1)	CreateWOppCurves();
//			else if(m_iView==4)	CreateCpCurves();

		}
		SetUFO();
		pMainFrame->UpdateWPolars();
		OnAdjustToWing();
		UpdateView();
	}
	else
	{
		// restore original
		pPlane->Duplicate(pSavePlane);
		delete pSavePlane; // clean up
		return false;
	}
	delete pSavePlane; // clean up
	return true;
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



void QMiarex::FillWOppCurve(CWOpp *pWOpp, Graph *pGraph, CCurve *pCurve, int Var)
{
	//The curve has been created, fill the curve with the WOpp data
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
			pGraph->SetYTitle("Induced Angle");
			break;
		}
		case 1:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit,
					pWOpp->m_Alpha + pWOpp->m_Ai[i] + pWOpp->m_Twist[i]);
			}
			pGraph->SetYTitle("Total Angle");
			break;
		}
		case 2:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_Cl[i]);
			}
			pGraph->SetYTitle("Cl");
			break;
		}
		case 3:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_Cl[i] * pWOpp->m_Chord[i]/pWOpp->m_MAChord);
			}
			pGraph->SetYTitle("Local lift");
			break;
		}
		case 4:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_PCd[i]);
			}
			pGraph->SetYTitle("Airfoil drag");
			break;
		}
		case 5:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_ICd[i]);
			}
			pGraph->SetYTitle("Induced drag");
			break;
		}
		case 6:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_PCd[i]+ pWOpp->m_ICd[i]);
			}
			pGraph->SetYTitle("Total drag");
			break;
		}
		case 7:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, (pWOpp->m_PCd[i]+ pWOpp->m_ICd[i])* pWOpp->m_Chord[i]/pWOpp->m_MAChord);
			}
			pGraph->SetYTitle("Local drag");
			break;
		}
		case 8:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_CmAirf[i]);
			}
			pGraph->SetYTitle("Cm Airfoil");
			break;
		}
		case 9:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_CmXRef[i]);
			}
			pGraph->SetYTitle("Cm XRef");
			break;
		}
		case 10:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_Cm[i]);
			}
			pGraph->SetYTitle("Cm");
			break;
		}
		case 11:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_Re[i]);
			}
			pGraph->SetYTitle("Re");
			break;
		}
		case 12:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_XTrTop[i]);
			}
			pGraph->SetYTitle("Top Trans x-Pos %");
			break;
		}
		case 13:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_XTrBot[i]);
			}
			pGraph->SetYTitle("Bot Trans x-Pos %");
			break;
		}
		case 14:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_XCPSpanRel[i]*100.0);
			}
			pGraph->SetYTitle("CP x-Pos %");
			break;
		}
		case 15:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit,
					pWOpp->m_BendingMoment[i] * pMainFrame->m_NmtoUnit);
			}
			QString str;
			GetMomentUnit(str, pMainFrame->m_MomentUnit);
			pGraph->SetYTitle("BM (" + str + ")" );
			break;
		}
		default:
		{
			for (i=nStart; i<pWOpp->m_NStation; i++)
			{
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pMainFrame->m_mtoUnit, pWOpp->m_Ai[i]);
			}
			pGraph->SetYTitle("Induced Angle");
		}
	}
}


void QMiarex::FillWPlrCurve(CCurve *pCurve, CWPolar *pWPolar, int XVar, int YVar)
{
	//The Wpolar curve object has been created
	//Fill it with the variable data specified by XVar and YVar
	bool bAdd;
	int i;
	double x,y;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QList <double> *pX;
	QList <double> *pY;
	pX = (QList <double> *) GetUFOPlrVariable(pWPolar, XVar);
	pY = (QList <double> *) GetUFOPlrVariable(pWPolar, YVar);

	for (i=0; i<pWPolar->m_Alpha.size(); i++)
	{
		bAdd = true;
		x = (*pX)[i];
		y = (*pY)[i];

		if((XVar==12 || XVar==15 || XVar==16) && x<0) bAdd = false;
		if((YVar==12 || YVar==15 || YVar==16) && y<0) bAdd = false;

		if(XVar==13 || XVar==14)              x *= pMainFrame->m_NtoUnit; //force
		if(YVar==13 || YVar==14)              y *= pMainFrame->m_NtoUnit; //force
		if(XVar==15 || XVar==16 || XVar==17)  x *= pMainFrame->m_mstoUnit;//speed
		if(YVar==15 || YVar==16 || YVar==17)  y *= pMainFrame->m_mstoUnit;//speed

		if(XVar==19 || XVar==20 || XVar==21)  x *= pMainFrame->m_NmtoUnit;//moment
		if(YVar==19 || YVar==20 || YVar==21)  y *= pMainFrame->m_NmtoUnit;//moment

		if(XVar==22 || XVar==23)              x *= pMainFrame->m_mtoUnit;//force
		if(YVar==22 || YVar==23)              y *= pMainFrame->m_mtoUnit;//force

		if(XVar==24)                          x *= pMainFrame->m_NmtoUnit;//moment
		if(YVar==24)                          y *= pMainFrame->m_NmtoUnit;//moment

		if(bAdd) pCurve->AddPoint(x,y);
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



double QMiarex::GetCl(CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, bool &bOutRe, bool &bError)
{

	double Cl0, Cl1;
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	if(!pFoil0)
		Cl0 = 2.0*pi*(Alpha*pi/180.0);
	else
		Cl0 = GetPlrPointFromAlpha(pFoil0, Re, Alpha, 1, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;
	if(!pFoil1)
		Cl1 = 2.0*pi*(Alpha*pi/180.0);
	else
		Cl1 = GetPlrPointFromAlpha(pFoil1, Re, Alpha, 1, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;

	return ((1-Tau) * Cl0 + Tau * Cl1);
}



double QMiarex::GetCm0(CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau, bool &bOutRe, bool &bError)
{
	//Find 0-lift angle for local foil
	double Alpha;
	double Cm0, Cm1;
	double Cl0 = 1.0;
	double Cl1;
	bOutRe = false;
	bError = false;
	bool IsOutRe;
	bool IsError;

	bOutRe = false;
	for (int i=-10; i<10; i++)
	{
		Alpha = (double)i;
		Cl1 = GetCl(pFoil0, pFoil1, Re, Alpha, Tau, IsOutRe, IsError);
		if(Cl1>0.0) {
			if(IsOutRe) bOutRe = true;
			if(IsError) bError = true;
			break;
		}
		Cl0 = Cl1;
	}
	if(Cl0>0.0)
	{
		return 0.0;
	}
	Cm0 = GetCm(pFoil0, pFoil1, Re, Alpha-1.0, Tau, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;
	Cm1 = GetCm(pFoil0, pFoil1, Re, Alpha,     Tau, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;

	double Res = Cm0 + (Cm1-Cm0)*(0.0-Cl0)/(Cl1-Cl0);

	return Res;
}


double QMiarex::GetCm(CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, bool &bOutRe, bool &bError)
{
	double Cm0, Cm1;
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	if(!pFoil0)
		Cm0 = 0.0;
	else
		Cm0 = GetPlrPointFromAlpha(pFoil0, Re, Alpha, 4, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if(!pFoil1)
		Cm1 = 0.0;
	else
		Cm1 = GetPlrPointFromAlpha(pFoil1, Re, Alpha, 4, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;
	return ((1-Tau) * Cm0 + Tau * Cm1);
}

double QMiarex::GetCd(CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, double AR, bool &bOutRe, bool &bError)
{
	//For LLT calculations
	//returns the interpolated viscous drag
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	double Cd0, Cd1, Cl;
	if(!pFoil0) {
		Cl = 2.0*pi*(Alpha*pi/180.0);
		Cd0 = Cl*Cl/pi/AR;
	}
	else Cd0 = GetPlrPointFromAlpha(pFoil0, Re, Alpha,2, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;
	if(!pFoil1)
	{
		Cl = 2.0*pi*(Alpha*pi/180.0);
		Cd1 = Cl*Cl/pi/AR;
	}
	else Cd1 = GetPlrPointFromAlpha(pFoil1, Re, Alpha,2, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;
	return ((1-Tau) * Cd0 + Tau * Cd1);
}


double QMiarex::GetXCp(CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, double AR,  bool &bOutRe, bool &bError)
{
	//For LLT calculations
	//returns the interpolated center of pressure position

//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	double XCp0, XCp1;

	if(!pFoil0) return 0.0;
	else XCp0 = GetPlrPointFromAlpha(pFoil0, Re, Alpha, 11, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if(!pFoil1) return 0.0;
	else XCp1 = GetPlrPointFromAlpha(pFoil1, Re, Alpha, 11, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;

	return ((1-Tau) * XCp0 + Tau * XCp1);
}

double QMiarex::GetXTr(CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, bool bTop, bool &bOutRe, bool &bError)
{
	//For LLT calculations
	//returns the interpolated position of the transition on the  surface specified by bTop

//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	double Tr0, Tr1;
	if(!pFoil0)
	{
		Tr0 = 1.0;
	}
	else
	{
		if(bTop) Tr0 = GetPlrPointFromAlpha(pFoil0, Re, Alpha, 5, IsOutRe, IsError);
		else     Tr0 = GetPlrPointFromAlpha(pFoil0, Re, Alpha, 6, IsOutRe, IsError);
	}
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;
	if(!pFoil1)
	{
		Tr1 = 1.0;
	}
	else
	{
		if(bTop) Tr1 = GetPlrPointFromAlpha(pFoil1, Re, Alpha, 5, IsOutRe, IsError);
		else     Tr1 = GetPlrPointFromAlpha(pFoil1, Re, Alpha, 6, IsOutRe, IsError);
	}
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;
	return ((1-Tau) * Tr0 + Tau * Tr1);
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
			if(m_pCurWPolar->m_Type<=3 && fabs(pPOpp->m_Alpha -Alpha)<0.01)			return pPOpp;
			else if(m_pCurWPolar->m_Type==4 && fabs(pPOpp->m_QInf - Alpha)<0.01)	return pPOpp;
			else if(m_pCurWPolar->m_Type==5 && fabs(pPOpp->m_Alpha - Alpha)<0.01)	return pPOpp;
			else if(m_pCurWPolar->m_Type==6 && fabs(pPOpp->m_Alpha - Alpha)<0.01)	return pPOpp;
		}
	}
	return NULL;
}


void * QMiarex::GetPlrVariable(CPolar *pPolar, int iVar)
{
	// returns a pointer to the variable array defined by its index iVar

	void * pVar;
	switch (iVar)
	{
		case 0:
			pVar = &pPolar->m_Alpha;
			break;
		case 1:
			pVar = &pPolar->m_Cl;
			break;
		case 2:
			pVar = &pPolar->m_Cd;
			break;
		case 3:
			pVar = &pPolar->m_Cdp;
			break;
		case 4:
			pVar = &pPolar->m_Cm;
			break;
		case 5:
			pVar = &pPolar->m_XTr1;
			break;
		case 6:
			pVar = &pPolar->m_XTr2;
			break;
		case 7:
			pVar = &pPolar->m_HMom;
			break;
		case 8:
			pVar = &pPolar->m_Cpmn;
			break;
		case 9:
			pVar = &pPolar->m_ClCd;
			break;
		case 10:
			pVar = &pPolar->m_Cl32Cd;
			break;
		case 11:
			pVar = &pPolar->m_XCp;
			break;
		default:
			pVar = &pPolar->m_Alpha;
			break;
	}
	return pVar;
}


double QMiarex::GetPlrPointFromAlpha(CFoil *pFoil, double Re, double Alpha, int PlrVar, bool &bOutRe, bool &bError)
{
/*	Var
	0 =	m_Alpha;
	1 = m_Cl;
	2 = m_Cd;
	3 = m_Cdp;
	4 = m_Cm;
	5, 6 = m_XTr1, m_XTr2;
	7, 8 = m_HMom, m_Cpmn;
	9,10 = m_ClCd, m_Cl32Cd;
	11 = m_XCp
*/

	QList <double> *pX;
	double amin, amax;
	double Var1, Var2, u;
	int i;
	CPolar *pPolar;

	bOutRe = false;
	bError = false;

	if(!pFoil)
	{
		bOutRe = true;
		bError = true;
		return 0.000;
	}


	int size;
	int n = 0;

	// Are there any Type 1 polars available for this foil ?
	for (i = 0; i< m_poaPolar->size(); i++)
	{
		pPolar = (CPolar*)m_poaPolar->at(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName))
		{
			n++;
			if(n>=2) break;
		}
	}

//more than one polar - interpolate between  - tough job

	//First Find the two polars with Reynolds number surrounding wanted Re
	CPolar * pPolar1 = NULL;
	CPolar * pPolar2 = NULL;
	int nPolars = (int)m_poaPolar->size();
	//Type 1 Polars are sorted by crescending Re Number

	//if Re is less than that of the first polar, use this one
	for (i=0; i< nPolars; i++)
	{
		pPolar = (CPolar*)m_poaPolar->at(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName))
		{
			// we have found the first type 1 polar for this foil
			if (Re < pPolar->m_Reynolds)
			{
				bOutRe = true;
				//interpolate Alpha on this polar
				pX = (QList <double> *) GetPlrVariable(pPolar, PlrVar);
				size = (int)pPolar->m_Alpha.size();
				if(Alpha < pPolar->m_Alpha[0])
				{
					return (*pX)[0];
				}
				if(Alpha > pPolar->m_Alpha[size-1])
				{
					return (*pX)[size-1];
				}
				for (i=0; i<size-1; i++)
				{
					if(pPolar->m_Alpha[i] <= Alpha && Alpha < pPolar->m_Alpha[i+1])
					{
						//interpolate
						if(pPolar->m_Alpha[i+1]-pPolar->m_Alpha[i] < 0.00001)//do not divide by zero
							return (*pX)[i];
						else
						{
							u = (Alpha - pPolar->m_Alpha[i])
									 /(pPolar->m_Alpha[i+1]-pPolar->m_Alpha[i]);
							return ((*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]));
						}
					}
				}
				break;
			}
			break;
		}
	}

	// if not Find the two polars
	for (i=0; i< nPolars; i++)
	{
		pPolar = (CPolar*)m_poaPolar->at(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName))
		{
			// we have found the first type 1 polar for this foil
			pPolar->GetAlphaLimits(amin, amax);
			if (pPolar->m_Reynolds <= Re)
			{
				if(amin <= Alpha && Alpha <= amax)
				{
					pPolar1 = pPolar;
				}
			}
			else {
				if(amin <= Alpha && Alpha <= amax)
				{
					pPolar2 = pPolar;
					break;
				}
			}
		}
	}

	if (!pPolar2)
	{
		//then Re is greater than that of any polar
		// so use last polar and interpolate alphas on this polar
		bOutRe = true;
		if(!pPolar1)
		{
			bError = true;
			return 0.000;
		}
		size = (int)pPolar1->m_Alpha.size();
		if(!size)
		{
			bError = true;
			return 0.000;
		}

		pX = (QList <double> *) GetPlrVariable(pPolar1, PlrVar);
		if(Alpha < pPolar1->m_Alpha[0])	     return (*pX)[0];
		if(Alpha > pPolar1->m_Alpha[size-1]) return (*pX)[size-1];
		for (i=0; i<size-1; i++)
		{
			if(pPolar1->m_Alpha[i] <= Alpha && Alpha < pPolar1->m_Alpha[i+1])
			{
				//interpolate
				if(pPolar1->m_Alpha[i+1]-pPolar1->m_Alpha[i] < 0.00001){//do not divide by zero
					return (*pX)[i];
				}
				else
				{
					u = (Alpha - pPolar1->m_Alpha[i])
							 /(pPolar1->m_Alpha[i+1]-pPolar1->m_Alpha[i]);
					return (*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]);
				}
			}
		}
		//Out in Re, out in alpha...
		return (*pX)[size-1] ;

	}
	else
	{
		// Re is between that of polars 1 and 2
		// so interpolate alphas for each

		if(!pPolar1)
		{
			bOutRe = true;
			bError = true;
			return 0.000;
		}
		size = (int)pPolar1->m_Alpha.size();
		if(!size) {
			bOutRe = true;
			bError = true;
			return 0.000;
		}
		pX = (QList <double> *) GetPlrVariable(pPolar1, PlrVar);
		if(Alpha < pPolar1->m_Alpha[0])	          Var1 = (*pX)[0];
		else if(Alpha > pPolar1->m_Alpha[size-1]) Var1 = (*pX)[size-1];
		else
		{
			for (i=0; i<size-1; i++)
			{
				if(pPolar1->m_Alpha[i] <= Alpha && Alpha < pPolar1->m_Alpha[i+1]){
					//interpolate
					if(pPolar1->m_Alpha[i+1]-pPolar1->m_Alpha[i] < 0.00001)//do not divide by zero
						Var1 = (*pX)[i];
					else
					{
						u = (Alpha - pPolar1->m_Alpha[i])
								 /(pPolar1->m_Alpha[i+1]-pPolar1->m_Alpha[i]);
						Var1 = (*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]);
					}
				}
			}
		}

		size = (int)pPolar2->m_Alpha.size();
		if(!size)
		{
			bOutRe = true;
			bError = true;
			return 0.000;
		}
		pX = (QList <double> *) GetPlrVariable(pPolar2, PlrVar);
		if(Alpha < pPolar2->m_Alpha[0])
		{
			bOutRe = true;
			bError = true;
			Var2 = (*pX)[0];
		}
		else if(Alpha > pPolar2->m_Alpha[size-1])
		{
			bOutRe = true;
			bError = true;
			Var2 = (*pX)[size-1];
		}
		else{
			for (i=0; i<size-1; i++)
			{
				if(pPolar2->m_Alpha[i] <= Alpha && Alpha < pPolar2->m_Alpha[i+1])
				{
					//interpolate
					pX = (QList <double> *) GetPlrVariable(pPolar2, PlrVar);
					if(pPolar2->m_Alpha[i+1]-pPolar2->m_Alpha[i] < 0.00001)//do not divide by zero
						Var2 = (*pX)[i];
					else{
						u = (Alpha - pPolar2->m_Alpha[i])
								 /(pPolar2->m_Alpha[i+1]-pPolar2->m_Alpha[i]);
						Var2 = (*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]);
					}
				}
			}
		}
		// then interpolate Variable

		double v =   (Re - pPolar1->m_Reynolds)
				  / (pPolar2->m_Reynolds - pPolar1->m_Reynolds);
		double Var = Var1 + v * (Var2-Var1);
		return Var;
	}

//	AfxMessageBox("Error interpolating", MB_OK);
//	bOutRe = true;
//	bError = true;
//	return 0.000;// we missed something somewhere...
}


double QMiarex::GetPlrPointFromCl(CFoil *pFoil, double Re, double Cl, int PlrVar, bool &bOutRe, bool &bError)
{
	//TODO : check this GetPlrPoint duplicate with CMAinFrame
/*	Var
	0 =	m_Alpha;
	1 = m_Cl;
	2 = m_Cd;
	3 = m_Cdp;
	4 = m_Cm;
	5, 6 = m_XTr1, m_XTr2;
	7, 8 = m_HMom, m_Cpmn;
	9,10 = m_ClCd, m_Cl32Cd;
*/
	QList <double> *pX;
	double Clmin, Clmax;
	CPolar *pPolar;
	double Var1, Var2, u, dist;
	int pt;
	int size;
	int n, i;

	bOutRe = false;
	bError = false;

	if(!pFoil)
	{
		bOutRe = true;
		bError = true;
		return 0.000;
	}


	n=0;
	// Are there any Type 1 polars available for this foil ?
	for (i = 0; i< m_poaPolar->size(); i++)
	{
		pPolar = (CPolar*)m_poaPolar->at(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName))
		{
			n++;
			if(n>=2) break;
		}
	}

//more than one polar - interpolate between  - tough job

	//First Find the two polars with Reynolds number surrounding wanted Re
	CPolar * pPolar1 = NULL;
	CPolar * pPolar2 = NULL;
	int nPolars = (int)m_poaPolar->size();
	//Type 1 Polars are sorted by crescending Re Number

	//if Re is less than that of the first polar, use this one
	for (i=0; i< nPolars; i++)
	{
		pPolar = (CPolar*)m_poaPolar->at(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName) && pPolar->m_Cl.size()>0)
		{
			// we have found the first type 1 polar for this foil
			if (Re < pPolar->m_Reynolds)
			{
				bOutRe = true;
				//interpolate Cl on this polar
				pX = (QList <double> *) GetPlrVariable(pPolar, PlrVar);
				size = (int)pPolar->m_Cl.size();
				if(Cl < pPolar->m_Cl[0])
				{
					return (*pX)[0];
				}
				if(Cl > pPolar->m_Cl[size-1])
				{
					return (*pX)[size-1];
				}
				for (i=0; i<size-1; i++)
				{
					if(pPolar->m_Cl[i] <= Cl && Cl < pPolar->m_Cl[i+1])
					{
					//interpolate
						if(pPolar->m_Cl[i+1]-pPolar->m_Cl[i] < 0.00001)//do not divide by zero
							return (*pX)[i];
						else {
							u = (Cl - pPolar->m_Cl[i])
									 /(pPolar->m_Cl[i+1]-pPolar->m_Cl[i]);
							return ((*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]));
						}
					}
				}
				break;
			}
			break;
		}
	}

	// if not Find the two polars
	for (i=0; i< nPolars; i++)
	{
		pPolar = (CPolar*)m_poaPolar->at(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName)  && pPolar->m_Cl.size()>0)
		{
			// we have found the first type 1 polar for this foil
			pPolar->GetClLimits(Clmin, Clmax);
			if (pPolar->m_Reynolds <= Re)
			{
				if(Clmin <= Cl && Cl <= Clmax)
				{
					pPolar1 = pPolar;
				}
			}
			else
			{
				if(Clmin <= Cl && Cl <= Clmax)
				{
					pPolar2 = pPolar;
					break;
				}
			}
		}
	}

	if (!pPolar2)
	{
		//then Re is greater than that of any polar
		// so use last polar and interpolate Cls on this polar
		bOutRe = true;
		if(!pPolar1)
		{
			bOutRe = true;
			bError = true;
			return 0.000;
		}
		size = (int)pPolar1->m_Cl.size();
		if(!size)
		{
			bOutRe = true;
			bError = true;
			return 0.000;
		}

		pX = (QList <double> *) GetPlrVariable(pPolar1, PlrVar);
		if(Cl < pPolar1->m_Cl[0])	   return (*pX)[0];
		if(Cl > pPolar1->m_Cl[size-1]) return (*pX)[size-1];
		for (i=0; i<size-1; i++)
		{
			if(pPolar1->m_Cl[i] <= Cl && Cl < pPolar1->m_Cl[i+1])
			{
				//interpolate
				if(pPolar1->m_Cl[i+1]-pPolar1->m_Cl[i] < 0.00001)
				{//do not divide by zero
					return (*pX)[i];
				}
				else
				{
					u = (Cl - pPolar1->m_Cl[i])
							 /(pPolar1->m_Cl[i+1]-pPolar1->m_Cl[i]);
					return ((*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]));
				}
			}
		}
		//Out in Re, out in Cl...
		return (*pX)[size-1];
	}
	else
	{
		// Re is between that of polars 1 and 2
		// so interpolate Cls for each
		if(!pPolar1)
		{
			bOutRe = true;
			bError = true;
			return 0.000;
		}
		size = (int)pPolar1->m_Cl.size();
		if(!size)
		{
			bOutRe = true;
			bError = true;
			return 0.000;
		}

		pX = (QList <double> *) GetPlrVariable(pPolar1, PlrVar);
		pPolar1->GetClLimits(Clmin, Clmax);
		if(Cl < Clmin)
		{
			Var1 = (*pX)[0];
			bOutRe = true;
		}
		else if(Cl > Clmax)
		{
			Var1 = (*pX)[size-1];
			bOutRe = true;
		}
		else
		{
			//first Find the point closest to Cl=0
			pt = 0;
			dist = fabs(pPolar1->m_Cl[0]);
			for (i=1; i<size;i++)
			{
				if (fabs(pPolar1->m_Cl[i])< dist)
				{
					dist = fabs(pPolar1->m_Cl[i]);
					pt = i;
				}
			}
			if(Cl<pPolar1->m_Cl[pt])
			{
				for (i=pt; i>0; i--)
				{
					if(Cl<= pPolar1->m_Cl[i] && Cl > pPolar1->m_Cl[i-1])
					{
						//interpolate
						if(fabs(pPolar1->m_Cl[i]-pPolar1->m_Cl[i-1]) < 0.00001)
						{
							//do not divide by zero
							Var1 = (*pX)[i];
							break;
						}
						else
						{
							u = (Cl - pPolar1->m_Cl[i-1])
									 /(pPolar1->m_Cl[i]-pPolar1->m_Cl[i-1]);
							Var1 = (*pX)[i-1] + u * ((*pX)[i]-(*pX)[i-1]);
							break;
						}
					}
				}
			}
			else
			{
				for (i=pt; i<size-1; i++)
				{
					if(pPolar1->m_Cl[i] <=Cl && Cl < pPolar1->m_Cl[i+1])
					{
						//interpolate
						if(fabs(pPolar1->m_Cl[i+1]-pPolar1->m_Cl[i]) < 0.00001){//do not divide by zero
							Var1 = (*pX)[i];
							break;
						}
						else
						{
							u = (Cl - pPolar1->m_Cl[i])
									 /(pPolar1->m_Cl[i+1]-pPolar1->m_Cl[i]);
							Var1 = (*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]);
							break;
						}
					}
				}
			}
		}
		size = (int)pPolar2->m_Cl.size();
		if(!size)
		{
			bOutRe = true;
			bError = true;
			return 0.000;
		}

		pX = (QList <double> *) GetPlrVariable(pPolar2, PlrVar);
		pPolar2->GetClLimits(Clmin, Clmax);

		if(Cl < Clmin)
		{
			Var2 = (*pX)[0];
			bOutRe = true;
		}
		else if(Cl > Clmax)
		{
			Var2 = (*pX)[size-1];
			bOutRe = true;
		}
		else
		{
			//first Find the point closest to Cl=0
			pt = 0;
			dist = fabs(pPolar2->m_Cl[0]);
			for (i=1; i<size;i++)
			{
				if (fabs(pPolar2->m_Cl[i])< dist)
				{
					dist = fabs(pPolar2->m_Cl[i]);
					pt = i;
				}
			}
			if(Cl<pPolar2->m_Cl[pt])
			{
				for (i=pt; i>0; i--)
				{
					if(Cl<= pPolar2->m_Cl[i] && Cl > pPolar2->m_Cl[i-1])
					{
						//interpolate
						if(fabs(pPolar2->m_Cl[i]-pPolar2->m_Cl[i-1]) < 0.00001)
						{//do not divide by zero
							Var2 = (*pX)[i];
							break;
						}
						else
						{
							u = (Cl - pPolar2->m_Cl[i-1])
									 /(pPolar2->m_Cl[i]-pPolar2->m_Cl[i-1]);
							Var2 = (*pX)[i-1] + u * ((*pX)[i]-(*pX)[i-1]);
							break;
						}
					}
				}
			}
			else
			{
				for (i=pt; i<size-1; i++)
				{
					if(pPolar2->m_Cl[i] <=Cl && Cl < pPolar2->m_Cl[i+1])
					{
						//interpolate
						if(fabs(pPolar2->m_Cl[i+1]-pPolar2->m_Cl[i]) < 0.00001)
						{
							//do not divide by zero
							Var2 = (*pX)[i];
							break;
						}
						else
						{
							u = (Cl - pPolar2->m_Cl[i])
									 /(pPolar2->m_Cl[i+1]-pPolar2->m_Cl[i]);
							Var2 = (*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]);
							break;
						}
					}
				}
			}
		}

		// then interpolate Variable

		double v =   (Re - pPolar1->m_Reynolds) / (pPolar2->m_Reynolds - pPolar1->m_Reynolds);
		double Var = Var1 + v * (Var2-Var1);
		return Var;
	}

//	AfxMessageBox("Error interpolating", MB_OK);
//	bOutRe = true;
//	bError = true;
//	return 0.000;// we missed something somewhere...
}


void QMiarex::GetLinearizedPolar(CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau, double &Alpha0, double &Slope)
{
	//returns the 0-lift angle of the foil, at Reynolds=Re
	//if the polar doesn't reach to 0-lift, returns Alpha0 = 0;
	double Alpha00, Alpha01;
	double Slope0, Slope1;
	double AlphaTemp1, AlphaTemp2, SlopeTemp1, SlopeTemp2;
	int i;

//Find the two polars which enclose Reynolds
	int size = 0;
	CPolar *pPolar, *pPolar1, *pPolar2;

	if(!pFoil0)
	{
		Alpha00 = 0.0;
		Slope0 = 2.0 * pi *pi/180.0;
	}
	else{
		pPolar1 = NULL;
		pPolar2 = NULL;
		for (i=0; i<m_poaPolar->size(); i++)
		{
			pPolar = (CPolar*)m_poaPolar->at(i);
			if(pPolar->m_FoilName == pFoil0->m_FoilName) size++;
		}
		if(size){
			for (i=0; i<m_poaPolar->size(); i++)
			{
				pPolar = (CPolar*)m_poaPolar->at(i);
				if(pPolar->m_FoilName == pFoil0->m_FoilName)
				{
					if(pPolar->m_Reynolds < Re) pPolar1 = pPolar;
				}
			}
			for (i=0; i<m_poaPolar->size(); i++)
			{
				pPolar = (CPolar*)m_poaPolar->at(i);
				if(pPolar->m_FoilName == pFoil0->m_FoilName)
				{
					if(pPolar->m_Reynolds > Re)
					{
						pPolar2 = pPolar;
						break;
					}
				}
			}
		}
		if(pPolar1 && pPolar2)
		{
			pPolar1->GetLinearizedCl(AlphaTemp1, SlopeTemp1);
			pPolar2->GetLinearizedCl(AlphaTemp2, SlopeTemp2);
			Alpha00 = AlphaTemp1 +
					 (AlphaTemp2-AlphaTemp1) * (Re-pPolar1->m_Reynolds)/(pPolar2->m_Reynolds-pPolar1->m_Reynolds);
			Slope0  = SlopeTemp1 +
					 (SlopeTemp2-SlopeTemp1) * (Re-pPolar1->m_Reynolds)/(pPolar2->m_Reynolds-pPolar1->m_Reynolds);
		}
		else
		{
			Alpha00 = 0.0;
			Slope0  = 2.0 * pi *pi/180.0;
		}
	}

	if(!pFoil1)
	{
		Alpha01 = 0.0;
		Slope1 = 2.0*pi *pi/180.0;
	}
	else
	{
		pPolar1 = NULL;
		pPolar2 = NULL;
		for (i=0; i<m_poaPolar->size(); i++)
		{
			pPolar = (CPolar*)m_poaPolar->at(i);
			if(pPolar->m_FoilName == pFoil1->m_FoilName) size++;
		}
		if(size){
			for (i=0; i<m_poaPolar->size(); i++)
			{
				pPolar = (CPolar*)m_poaPolar->at(i);
				if(pPolar->m_FoilName == pFoil1->m_FoilName)
				{
					if(pPolar->m_Reynolds < Re) pPolar1 = pPolar;
				}
			}
			for (i=0; i<m_poaPolar->size(); i++)
			{
				pPolar = (CPolar*)m_poaPolar->at(i);
				if(pPolar->m_FoilName == pFoil1->m_FoilName)
				{
					if(pPolar->m_Reynolds > Re)
					{
						pPolar2 = pPolar;
						break;
					}
				}
			}
		}
		if(pPolar1 && pPolar2){
			pPolar1->GetLinearizedCl(AlphaTemp1, SlopeTemp1);
			pPolar2->GetLinearizedCl(AlphaTemp2, SlopeTemp2);
			Alpha01 = AlphaTemp1 +
					 (AlphaTemp2-AlphaTemp1) * (Re-pPolar1->m_Reynolds)/(pPolar2->m_Reynolds-pPolar1->m_Reynolds);
			Slope1  = SlopeTemp1 +
					 (SlopeTemp2-SlopeTemp1) * (Re-pPolar1->m_Reynolds)/(pPolar2->m_Reynolds-pPolar1->m_Reynolds);
		}
		else {
			Alpha01 = 0.0;
			Slope1 = 2.0*pi *pi/180.0;
		}
	}

	Alpha0 = ((1-Tau) * Alpha00 + Tau * Alpha01);
	Slope  = ((1-Tau) * Slope0  + Tau * Slope1);
}


QGraph* QMiarex::GetGraph(QPoint &pt)
{
	//returns a pointer to the graph in which the point pt lies
	if(m_iView==1)
	{
		if(m_iWingView==1)
		{
//			m_pCurWingGraph = m_pCurGraph;
			return m_pCurWingGraph;
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
			if(m_WingGraph2.IsInDrawRect(pt)){return &m_WingGraph2;}
			if(m_WingGraph3.IsInDrawRect(pt)){return &m_WingGraph3;}
			if(m_WingGraph4.IsInDrawRect(pt)){return &m_WingGraph4;}
		}
	}
	else if(m_iView==2)
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
	else if(m_iView==4)
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


void * QMiarex::GetUFOPlrVariable(CWPolar *pWPolar, int iVar)
{
	// returns a pointer to the variable array defined by its index iVar
	void * pVar;
	switch (iVar)
	{
		case 0:
			pVar = &pWPolar->m_Alpha;
			break;
		case 1:
			pVar = &pWPolar->m_Cl;
			break;
		case 2:
			pVar = &pWPolar->m_PCd;
			break;
		case 3:
			pVar = &pWPolar->m_ICd;
			break;
		case 4:
			pVar = &pWPolar->m_TCd;
			break;
		case 5:
			pVar = &pWPolar->m_GCm;
			break;
		case 6:
			pVar = &pWPolar->m_GRm;
			break;
		case 7:
			pVar = &pWPolar->m_GYm;
			break;
		case 8:
			pVar = &pWPolar->m_VYm;
			break;
		case 9:
			pVar = &pWPolar->m_IYm;
			break;
		case 10:
			pVar = &pWPolar->m_ClCd;
			break;
		case 11:
			pVar = &pWPolar->m_Cl32Cd;
			break;
		case 12:
			pVar = &pWPolar->m_Cl;
			break;
		case 13:
			pVar = &pWPolar->m_L;
			break;
		case 14:
			pVar = &pWPolar->m_D;
			break;
		case 15:
			pVar = &pWPolar->m_Vx;
			break;
		case 16:
			pVar = &pWPolar->m_Vz;
			break;
		case 17:
			pVar = &pWPolar->m_QInfinite;
			break;
		case 18:
			pVar = &pWPolar->m_Gamma;
			break;
		case 19:
			pVar = &pWPolar->m_Pm;
			break;
		case 20:
			pVar = &pWPolar->m_Rm;
			break;
		case 21:
			pVar = &pWPolar->m_Ym;
			break;
		case 22:
			pVar = &pWPolar->m_XCP;
			break;
		case 23:
			pVar = &pWPolar->m_YCP;
			break;
		case 24:
			pVar = &pWPolar->m_MaxBending;
			break;
		case 25:
			pVar = &pWPolar->m_VertPower;
			break;
		case 26:
			pVar = &pWPolar->m_Oswald;
			break;
		case 27:
			pVar = &pWPolar->m_SM;
			break;
		case 28:
			pVar = &pWPolar->m_Ctrl;
			break;
		case 29:
			pVar = &pWPolar->m_CY;
			break;
		default:
			pVar = &pWPolar->m_Alpha;
			break;
	}
	return pVar;
}


double QMiarex::GetZeroLiftAngle(CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau)
{
	//returns the 0-lift angle of the foil, at Reynolds=Re
	//if the polar doesn't reach to 0-lift, returns Alpha0 = 0;
	double a01, a02;
	double Alpha00, Alpha01;
	int i;
	//Find the two polars which enclose Reynolds
	int size = 0;
	CPolar *pPolar, *pPolar1, *pPolar2;

	if(!pFoil0) Alpha00 = 0.0;
	else
	{
		pPolar1 = NULL;
		pPolar2 = NULL;
		for (i=0; i<m_poaPolar->size(); i++)
		{
			pPolar = (CPolar*)m_poaPolar->at(i);
			if(pPolar->m_FoilName == pFoil0->m_FoilName) size++;
		}
		if(size)
		{
			for (i=0; i<m_poaPolar->size(); i++)
			{
				pPolar = (CPolar*)m_poaPolar->at(i);
				if(pPolar->m_FoilName == pFoil0->m_FoilName)
				{
					if(pPolar->m_Reynolds < Re) pPolar1 = pPolar;
				}
			}
			for (i=0; i<m_poaPolar->size(); i++)
			{
				pPolar = (CPolar*)m_poaPolar->at(i);
				if(pPolar->m_FoilName == pFoil0->m_FoilName)
				{
					if(pPolar->m_Reynolds > Re)
					{
						pPolar2 = pPolar;
						break;
					}
				}
			}
		}
		if(pPolar1 && pPolar2)
		{
			a01 = pPolar1->GetZeroLiftAngle();
			a02 = pPolar2->GetZeroLiftAngle();
			Alpha00 = a01 + (a02-a01) * (Re-pPolar1->m_Reynolds)/(pPolar2->m_Reynolds-pPolar1->m_Reynolds);
		}
		else Alpha00 = 0.0;
	}

	if(!pFoil1) Alpha01 = 0.0;
	else
	{
		pPolar1 = NULL;
		pPolar2 = NULL;
		for (i=0; i<m_poaPolar->size(); i++)
		{
			pPolar = (CPolar*)m_poaPolar->at(i);
			if(pPolar->m_FoilName == pFoil1->m_FoilName) size++;
		}
		if(size)
		{
			for (i=0; i<m_poaPolar->size(); i++)
			{
				pPolar = (CPolar*)m_poaPolar->at(i);
				if(pPolar->m_FoilName == pFoil1->m_FoilName)
				{
					if(pPolar->m_Reynolds < Re) pPolar1 = pPolar;
				}
			}
			for (i=0; i<m_poaPolar->size(); i++)
			{
				pPolar = (CPolar*)m_poaPolar->at(i);
				if(pPolar->m_FoilName == pFoil1->m_FoilName)
				{
					if(pPolar->m_Reynolds > Re)
					{
						pPolar2 = pPolar;
						break;
					}
				}
			}
		}
		if(pPolar1 && pPolar2)
		{
			a01 = pPolar1->GetZeroLiftAngle();
			a02 = pPolar2->GetZeroLiftAngle();
			Alpha01 = a01 + (a02-a01) * (Re-pPolar1->m_Reynolds)/(pPolar2->m_Reynolds-pPolar1->m_Reynolds);
		}
		else Alpha01 = 0.0;
	}

	return ((1-Tau) * Alpha00 + Tau * Alpha01);
}


double QMiarex::GetVar(int nVar, CFoil *pFoil0, CFoil *pFoil1, double Re, double Cl, double Tau, bool &bOutRe, bool &bError)
{
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	double Var0, Var1;
	if(!pFoil0)
	{
		Cl = 0.0;
		Var0 = 0.0;
	}
	else Var0 = GetPlrPointFromCl(pFoil0, Re, Cl,nVar, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;


	if(!pFoil1)
	{
		Cl = 0.0;
		Var1 = 0.0;
	}
	else Var1 = GetPlrPointFromCl(pFoil1, Re, Cl,nVar, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;

	return ((1-Tau) * Var0 + Tau * Var1);
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
			if(m_pCurWPolar->m_Type<3 && fabs(pWOpp->m_Alpha -Alpha)<0.01)        return pWOpp;
			else if(m_pCurWPolar->m_Type==4 && fabs(pWOpp->m_QInf - Alpha)<0.01)  return pWOpp;
			else if(m_pCurWPolar->m_Type==5 && fabs(pWOpp->m_Alpha - Alpha)<0.01) return pWOpp;
			else if(m_pCurWPolar->m_Type==6 && fabs(pWOpp->m_Alpha - Alpha)<0.01) return pWOpp;
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

bool QMiarex::InitializePanels()
{
	if(!m_pCurWing) return false;
	int j,k,l;

/*	QProgressDialog ProgressDlg;
	ProgressDlg.setMinimum(0);
	ProgressDlg.setMinimum(100);
	ProgressDlg.setValue(0);
	ProgressDlg.setWindowModality(Qt::WindowModal);*/

//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
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
		strong = QString("The total number of panels is %1\n The Max Number is %2\nA reduction of the number of panels is required")
						  .arg(m_MatSize).arg(VLMMATSIZE);
		QMessageBox::warning(this,"QFLR5",strong);
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
	}


	if(bBodyEl)
	{
		Nel = CreateBodyElements(ptr);
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
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!m_pctrlAnalyze->hasFocus()) m_pctrlAnalyze->setFocus();
			break;
		}
		case Qt::Key_1:
		{
			if(m_iView==2)
			{
				m_iWPlrView  = 1;
				m_pCurGraph = &m_WPlrGraph1;
				m_pCurWPlrGraph = m_pCurGraph;
				SetWPlrLegendPos();
			}
			else if(m_iView==1)
			{
				m_iWingView =1;
				m_pCurGraph = &m_WingGraph1;
				m_pCurWingGraph = m_pCurGraph;
				SetWingLegendPos();
			}
			UpdateView();
			break;
		}
		case Qt::Key_2:
		{
			if(m_iView==2)
			{
				m_iWPlrView  = 1;
				m_pCurGraph = &m_WPlrGraph2;
				m_pCurWPlrGraph = m_pCurGraph;
				SetWPlrLegendPos();
			}
			else if(m_iView==1)
			{
				m_iWingView =1;
				m_pCurGraph = &m_WingGraph2;
				m_pCurWingGraph = m_pCurGraph;
				SetWingLegendPos();
			}
			UpdateView();
			break;
		}
		case Qt::Key_3:
		{
			if(m_iView==2)
			{
				m_iWPlrView  = 1;
				m_pCurGraph = &m_WPlrGraph3;
				m_pCurWPlrGraph = m_pCurGraph;
				SetWPlrLegendPos();
			}
			else if(m_iView==1)
			{
				m_iWingView =1;
				m_pCurGraph = &m_WingGraph3;
				m_pCurWingGraph = m_pCurGraph;
				SetWingLegendPos();
			}
			UpdateView();
			break;
		}
		case Qt::Key_4:
		{
			if(m_iView==2)
			{
				m_iWPlrView  = 1;
				m_pCurGraph = &m_WPlrGraph4;
				m_pCurWPlrGraph = m_pCurGraph;
				SetWPlrLegendPos();
			}
			else if(m_iView==1)
			{
				m_iWingView =1;
				m_pCurGraph = &m_WingGraph4;
				m_pCurWingGraph = m_pCurGraph;
				SetWingLegendPos();
			}
			UpdateView();
			break;
		}
		case Qt::Key_T:
		{
			if(m_iView==2)
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
			if(m_iView==2)
			{
				m_iWPlrView = 4;
				SetWPlrLegendPos();
			}
			else
			{
				m_iWingView =4;
				SetWingLegendPos();
			}
			UpdateView();
			break;
		}
		case Qt::Key_F4:
		{
			m_GL3dView.show();
			break;
		}
		case Qt::Key_F5:
		{
			OnWOpps();			break;
		}
		case Qt::Key_F6:
		{
			OnDefineWPolar();
			break;
		}
		case Qt::Key_F8:
		{
			OnWPolars();
			break;
		}
		default:
			QWidget::keyPressEvent(event);
	}
}



void QMiarex::LLTAnalyze(double V0, double VMax, double VDelta, bool bSequence, bool bInitCalc)
{
	if(!m_pCurWing || !m_pCurWPolar) return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	LLTAnalysisDlg LLTDlg;
	CWing::s_pLLTDlg = &LLTDlg;
	m_pCurWing->m_bInitCalc = bInitCalc;

	LLTDlg.m_pMiarex    = this;
	LLTDlg.m_pMainFrame = m_pMainFrame;
	
	LLTDlg.m_pWing      = m_pCurWing;
	LLTDlg.m_pWPolar    = m_pCurWPolar;
	LLTDlg.m_bSequence  = bSequence;

	LLTDlg.m_AlphaMin      = V0;
	LLTDlg.m_AlphaMax   = VMax;
	LLTDlg.m_AlphaDelta = VDelta;

	LLTDlg.InitDialog();

	LLTDlg.show();
	LLTDlg.StartAnalysis();

	if(m_iView!=2) SetWOpp(false, V0);
	
	else CreateWPolarCurves();
	pMainFrame->UpdateWOpps();
	SetWOpp(true);
	UpdateView();
}


bool QMiarex::LoadSettings(QDataStream &ar)
{
	int k;
	if(!m_GLLightDlg.LoadSettings(ar)) return false;

	ar >> m_AlphaMin   >> m_AlphaMax   >> m_AlphaDelta    >> m_QInfMin >> m_QInfMax >> m_QInfDelta;
	ar >> m_ControlMin >> m_ControlMax >> m_ControlDelta;

	ar >> m_3DAxisStyle >> m_3DAxisWidth >> m_3DAxisColor;
	ar >> m_VLMStyle >> m_VLMWidth >> m_VLMColor;
	ar >> m_OutlineStyle >> m_OutlineWidth >> m_OutlineColor;
	ar >> m_XCPStyle >> m_XCPWidth >> m_XCPColor;
	ar >> m_MomentStyle >> m_MomentWidth >> m_MomentColor;
	ar >> m_IDragStyle >> m_IDragWidth >> m_IDragColor;
	ar >> m_VDragStyle >> m_VDragWidth >> m_VDragColor;
	ar >> m_XTopStyle >> m_XTopWidth >> m_XTopColor;
	ar >> m_XBotStyle >> m_XBotWidth >> m_XBotColor;
	ar >> m_DownwashStyle >> m_DownwashWidth >> m_DownwashColor;
	ar >> m_WakeStyle >> m_WakeWidth >> m_WakeColor;
	ar >> m_CpColor >> m_CpStyle >> m_CpWidth;

	ar >> CWing::s_CvPrec >> CWing::s_RelaxMax >> CWing::s_NLLTStations;

	ar >> m_iView >> m_iWingView >> m_iWPlrView;	

	ar >>k;
	if     (k==1) m_pCurWingGraph = &m_WingGraph1;
	else if(k==2) m_pCurWingGraph = &m_WingGraph2;
	else if(k==3) m_pCurWingGraph = &m_WingGraph3;
	else if(k==4) m_pCurWingGraph = &m_WingGraph4;

	ar >>k;
	if     (k==1) m_pCurWPlrGraph = &m_WPlrGraph1;
	else if(k==2) m_pCurWPlrGraph = &m_WPlrGraph2;
	else if(k==3) m_pCurWPlrGraph = &m_WPlrGraph3;
	else if(k==4) m_pCurWPlrGraph = &m_WPlrGraph4;
	if(m_iView==1)      m_pCurGraph=m_pCurWingGraph;
	else if(m_iView==2) m_pCurGraph=m_pCurWPlrGraph;

	ar >> m_XW1 >> m_YW1 >> m_XW2 >> m_YW2 >> m_XW3 >> m_YW3 >> m_XW4 >> m_YW4;
	ar >> m_WOppVar1 >> m_WOppVar2 >> m_WOppVar3 >> m_WOppVar4; 
	ar >> m_Iter >> m_NStation ;			
	ar >> m_InducedDragPoint;		
	ar >> m_NHoopPoints >> m_NXPoints;

	m_WingGraph1.Serialize(ar, false);
	m_WingGraph2.Serialize(ar, false);
	m_WingGraph3.Serialize(ar, false);
	m_WingGraph4.Serialize(ar, false);
	m_WPlrGraph1.Serialize(ar, false);
	m_WPlrGraph2.Serialize(ar, false);
	m_WPlrGraph3.Serialize(ar, false);
	m_WPlrGraph4.Serialize(ar, false);
	m_CpGraph.Serialize(ar, false);

	ar >> m_bXTop >> m_bXBot >> m_bXCP >> m_bXCmRef >> m_bICd >> m_bVCd;

	ar >> m_GL3dView.m_bWakePanels >> m_GL3dView.m_bSurfaces >> m_GL3dView.m_bOutline >> m_GL3dView.m_bVLMPanels;
	ar >> m_GL3dView.m_bAxes  >> m_GL3dView.m_b3DCp >> m_GL3dView.m_bDownwash >> m_GL3dView.m_bMoments;
	ar >> m_GL3dView.m_bAutoCpScale >> m_GL3dView.m_bShowCpScale >> m_GL3dView.m_bXTop >> m_GL3dView.m_bXBot >> m_GL3dView.m_bXCP;

	m_GL3dBody.LoadSettings(ar);


	return true;
}


void QMiarex::mousePressEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton) OnLButtonDown(event);
}

void QMiarex::mouseMoveEvent(QMouseEvent *event)
{
 	TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	CVector Real;
	QPoint Delta(event->pos().x() - m_LastPoint.x(), event->pos().y() - m_LastPoint.y());

	QPoint point = event->pos();

	if(!p2DWidget->hasFocus()) p2DWidget->setFocus();

	bool bCtrl = false;
	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;

	if ((event->buttons() & Qt::LeftButton) && m_bTrans && (m_iView==1 || m_iView==2 || m_iView==4))
	{
		m_pCurGraph = GetGraph(point);
		if(m_pCurGraph && m_bTransGraph)
		{ 
			// we translate the curves inside the graph
			double xu, yu;
			m_pCurGraph->SetAuto(false);
			double x1 =  m_pCurGraph->ClientTox(m_LastPoint.x()) ;
			double y1 =  m_pCurGraph->ClientToy(m_LastPoint.y()) ;
			
			xu = m_pCurGraph->ClientTox(point.x());
			yu = m_pCurGraph->ClientToy(point.y());

			double xmin = m_pCurGraph->GetXMin() - xu+x1;
			double xmax = m_pCurGraph->GetXMax() - xu+x1;
			double ymin = m_pCurGraph->GetYMin() - yu+y1;
			double ymax = m_pCurGraph->GetYMax() - yu+y1;

			m_pCurGraph->SetWindow(xmin, xmax, ymin, ymax);
			UpdateView();
		}
		else if (m_pCurWing  && ((m_iView==1 && m_iWingView ==1)))
		{
			// we translate the UFO

			m_ptOffset.rx() += Delta.x();
			m_ptOffset.ry() += Delta.y();
			UpdateView();

		}
		else if (m_pCurWing  && m_iView!=2 && m_iWingView !=1)
		{
			// we translate the legend
			//horizontally only
			m_WingLegendOffset.rx() += Delta.x();
			UpdateView();
		}
		else if (m_iView==2)
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
	}


	else if ((event->buttons() & Qt::MidButton) && !bCtrl) 
	//scaling
	{
		// we zoom the graph or the wing	
		if(m_iView ==1 ||m_iView==2 || m_iView==4)
		{
			m_pCurGraph = GetGraph(point);
			if(m_pCurGraph && m_pCurGraph->IsInDrawRect(point))
			{ 
				//zoom graph

/*				if (shX & 0x8000)
				{
					//zoom x scale
					m_pCurGraph->SetAutoX(false);
					m_pCurGraph->SetAutoX(false);
					if(point.x()-m_LastPoint.x()<0) m_pCurGraph->Scalex(1.04);
					else                            m_pCurGraph->Scalex(1.0/1.04);
				}
				else if(shY & 0x8000)
				{
					//zoom y scale
					m_pCurGraph->SetAutoY(false);
					m_pCurGraph->SetAutoY(false);
					if(point.y()-m_LastPoint.y()<0) m_pCurGraph->Scaley(1.04);
					else                            m_pCurGraph->Scaley(1.0/1.04);
				}
				else*/
				{
					//zoom both
					m_pCurGraph->SetAuto(false);
					if(point.y()-m_LastPoint.y()<0) m_pCurGraph->Scale(1.06);
					else                            m_pCurGraph->Scale(1.0/1.06);
				}
				UpdateView();
			}
			else if(m_pCurWing && m_iView==1)
			{
				//zoom wing
				if(point.y()-m_LastPoint.y()<0)  m_WingScale /=1.04;
				else                             m_WingScale *= 1.04;
				UpdateView();
			}
		}
	}

	m_LastPoint = point;
}



void QMiarex::mouseReleaseEvent(QMouseEvent *event)
{
	TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;
	p2DWidget->setCursor(Qt::CrossCursor);
	m_bTrans = false;
	m_bDragPoint  = false;
}


void QMiarex::On3DView()
{
	m_GL3dView.show();
}



void QMiarex::OnAnalyze()
{
	int l;
	double V0, VMax, VDelta;
	if(!m_pCurWing || !m_pCurWPolar) return;

//	Read Analysis Params;
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
		V0     = m_AlphaMin;
		VMax   = m_AlphaMax;
		VDelta = m_AlphaDelta;
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
		V0     = m_QInfMin;
		VMax   = m_QInfMax;
		VDelta = m_QInfDelta;
	}
	else if(m_pCurWPolar->m_Type==5 || m_pCurWPolar->m_Type==6)
	{
		m_ControlMin   = m_pctrlAlphaMin->GetValue()        /pMainFrame->m_mstoUnit;
		m_ControlMax   = m_pctrlAlphaMax->GetValue()        /pMainFrame->m_mstoUnit;
		m_ControlDelta = fabs(m_pctrlAlphaDelta->GetValue()) /pMainFrame->m_mstoUnit;
		if(fabs(m_ControlDelta)<0.001)
		{
			m_ControlDelta = 0.001;
			m_pctrlAlphaDelta->SetValue(0.001);
		}
		V0     = m_ControlMin;
		VMax   = m_ControlMax;
		VDelta = m_ControlDelta;
	}

/*	if(m_pCurWPolar->m_Type <3)
		Analyze(m_Alpha0, m_AlphaMax, m_AlphaDelta, m_bSequence, m_bInitCalc);
	else if(m_pCurWPolar->m_Type==4)
		Analyze(m_QInf0, m_QInfMax, m_DeltaQInf, m_bSequence, m_bInitCalc);
	else if(m_pCurWPolar->m_Type==5 || m_pCurWPolar->m_Type==6)
		Analyze(m_Control0, m_ControlMax, m_DeltaControl, m_bSequence, m_bInitCalc);
*/

	// check if all the foils are there...
	// ...could have been deleted or renamed or not imported with AVL wing or whatever
	for (l=0; l<=m_pCurWing->m_NPanel; l++)
	{
		if (!pMainFrame->GetFoil(m_pCurWing->m_RFoil[l]))
		{
			QString strong;
			strong = "Could not find the wing's foil "+ m_pCurWing->m_RFoil[l] +"...\nAborting Calculation";
			QMessageBox::warning(this, "QFLR5", strong);
			return;
		}
		if (!pMainFrame->GetFoil(m_pCurWing->m_LFoil[l]))
		{
			QString strong;
			strong = "Could not find the wing's foil "+ m_pCurWing->m_LFoil[l] +"...\nAborting Calculation";
			QMessageBox::warning(this, "QFLR5", strong);
			return;
		}
	}

	if(m_pCurStab)
	{
		for (l=0; l<=m_pCurStab->m_NPanel; l++){
			if (!pMainFrame->GetFoil(m_pCurStab->m_RFoil[l]))
			{
				QString strong;
				strong = "Could not find the elevator's foil "+ m_pCurStab->m_RFoil[l] +"...\nAborting Calculation";
				QMessageBox::warning(this, "QFLR5", strong);
				return;
			}
			if (!pMainFrame->GetFoil(m_pCurStab->m_LFoil[l]))
			{
				QString strong;
				strong = "Could not find the elevator's foil "+ m_pCurStab->m_LFoil[l] +"...\nAborting Calculation";
				QMessageBox::warning(this, "QFLR5", strong);
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
				strong = "Could not find the fin's foil "+ m_pCurFin->m_RFoil[l] +"...\nAborting Calculation";
				QMessageBox::warning(this, "QFLR5", strong);
				return;
			}
		}
	}

	if(m_pCurWPolar->m_AnalysisType==1)      LLTAnalyze(V0, VMax, VDelta, m_bSequence, m_bInitLLTCalc);
	else if(m_pCurWPolar->m_AnalysisType==2) VLMAnalyze(V0, VMax, VDelta, m_bSequence);
        else if(m_pCurWPolar->m_AnalysisType==3) PanelAnalyze(V0, VMax, VDelta, m_bSequence);
}



void QMiarex::OnAnimate(int state)
{
}


void QMiarex::OnAdjustToWing()
{
	//Adjust the graph's scale to the wing
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(!m_pCurWing) return;

	double halfspan = m_pCurWing->m_Span/2.0;
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
	for (i = (int)m_poaPOpp->size()-1; i>=0; i--)
	{
		pPOpp = (CPOpp*) m_poaPOpp->at(i);
		if(pPOpp->m_PlrName == m_pCurWPolar->m_PlrName)
		{
			m_poaPOpp->removeAt(i);
			delete pPOpp;
		}
	}
	for (i = (int)m_poaWOpp->size()-1; i>=0; i--)
	{
		pWOpp = (CWOpp*) m_poaWOpp->at(i);
		if(pWOpp->m_PlrName == m_pCurWPolar->m_PlrName)
		{
			m_poaWOpp->removeAt(i);
			delete pWOpp;
		}
	}


	m_pCurWOpp = NULL;
	m_pCurPOpp = NULL;
	pMainFrame->UpdateWOpps();
	SetWOpp(true);
	if (m_iView==1)     CreateWOppCurves();
//	else if(m_iView==4)	CreateCpCurves();
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
	if (m_iView==1)     CreateWOppCurves();
//	else if(m_iView==4)	CreateCpCurves();
	UpdateView();
}

void QMiarex::OnExportCurWOpp()
{
	//Exprt the currently selected WOpp to the text file
	if(!m_pCurWOpp)return ;// is there anything to export ?

	int l,p;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString filter =".csv";
	QString FileName, DestFileName, OutString,sep,str;
	QFile DestFile;
	int type = 1;

	FileName = m_pCurWOpp->m_PlrName;
	FileName.replace("/", " ");
	FileName = QFileDialog::getSaveFileName(this, "Export Wing Opp",
											pMainFrame->m_LastDirName + "/"+FileName,
											"Text File (*.txt; *.csv)",
											&filter);

	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);
	pos = FileName.lastIndexOf(".csv");
	if (pos>0) type = 2;

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);

	if(type==1) sep = ""; else sep=",";


	QString strOut, Header, strong;
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

	strong = QString("Cl    = "+sep+"%1\n").arg(m_pCurWOpp->m_CL,9,'f',4);
	out << strong;

	strong = QString("Cy    = "+sep+"%1\n").arg(m_pCurWOpp->m_CY,9,'f',4);
	out << strong;

	if(type==1) strong = QString("Cd    = %1     ICd   = %2     PCd   = %3\n")
		.arg(m_pCurWOpp->m_InducedDrag+m_pCurWOpp->m_ViscousDrag,9,'f',4)
		.arg(m_pCurWOpp->m_InducedDrag,9,'f',4)
		.arg(m_pCurWOpp->m_ViscousDrag,9,'f',4);
	else        strong = QString("Cd=,%1,ICd=, %2,PCd=, %3\n")
		.arg(m_pCurWOpp->m_InducedDrag+m_pCurWOpp->m_ViscousDrag,9,'f',4)
		.arg(m_pCurWOpp->m_InducedDrag,9,'f',4)
		.arg(m_pCurWOpp->m_ViscousDrag,9,'f',4);
	out << strong;

	strong = QString("GCm   ="+sep+" %1\n").arg(m_pCurWOpp->m_GCm, 9,'f',4);
	out << strong;

	strong = QString("GRm   = "+sep+"%1\n").arg(m_pCurWOpp->m_GRm, 9,'f',4);
	out << strong;

	if(type==1) strong = QString("IYm   = %1     PYm   = %2 \n").arg(m_pCurWOpp->m_IYm, 9,'f',4).arg(m_pCurWOpp->m_GYm, 9,'f',4);
	else        strong = QString("IYm=, %1,PYm=, %2\n").arg(m_pCurWOpp->m_IYm, 9,'f',4).arg(m_pCurWOpp->m_GYm, 9,'f',4);
	out << strong;

	if(type==1) strong = QString("XCP   = %1     YCP   = %2 \n").arg(m_pCurWOpp->m_XCP, 9,'f',4).arg(m_pCurWOpp->m_YCP, 9,'f',4);
	else        strong = QString("XCP=, %1, YCP=, %2 \n").arg(m_pCurWOpp->m_XCP, 9,'f',4).arg(m_pCurWOpp->m_YCP, 9,'f',4);
	out << strong;

	strong = QString("Bend. ="+sep+" %1\n\n").arg(m_pCurWOpp->m_MaxBending, 9,'f',4);
	out << strong;

	out << ("Main Wing Data\n");
	for (l=0; l<m_pCurWOpp->m_nFlaps; l++)
	{
		strong = QString("Flap "+sep+"%1"+sep+" moment = "+sep+"%2").arg(l+1).arg(m_pCurWOpp->m_FlapMoment[l]*pMainFrame->m_NmtoUnit, 9,'f',4);
		GetMomentUnit(str, pMainFrame->m_MomentUnit);
		strong += str +"\n";
		out << strong;
	}
	out << ("\n");
	m_pCurWOpp->Export(&XFile, type);

	if(m_pCurWing2)
	{
		out << ("Secondary Wing Data\n");
		for (l=0; l<m_pCurPOpp->m_Wing2WOpp.m_nFlaps; l++)
		{
			strong = QString("Flap "+sep+"%1"+sep+" moment = "+sep+"%2 ").arg(l+1,4).arg(m_pCurPOpp->m_Wing2WOpp.m_FlapMoment[l]*pMainFrame->m_NmtoUnit, 9,'f',4);
			GetMomentUnit(str, pMainFrame->m_MomentUnit);
			strong += str +"\n";
			out << strong;
		}
		out << ("\n");
		m_pCurPOpp->m_Wing2WOpp.Export(&XFile, type);
	}

	if(m_pCurStab)
	{
		out << ("Elevator Data\n");
		for (l=0; l<m_pCurPOpp->m_StabWOpp.m_nFlaps; l++)
		{
			strong = QString("Flap "+sep+"%1"+sep+" moment = "+sep+"%2 ").arg(l+1,4).arg(m_pCurPOpp->m_StabWOpp.m_FlapMoment[l]*pMainFrame->m_NmtoUnit, 9,'f',4);
			GetMomentUnit(str, pMainFrame->m_MomentUnit);
			strong += str +"\n";
			out << strong;
		}
		out << ("\n");
		m_pCurPOpp->m_StabWOpp.Export(&XFile, type);
	}

	if(m_pCurFin)
	{
		out << ("Fin Data\n");
		for (l=0; l<m_pCurPOpp->m_FinWOpp.m_nFlaps; l++)
		{
			strong = QString("Flap "+sep+"%1"+sep+" moment ="+sep+" %2").arg(l+1).arg(m_pCurPOpp->m_FinWOpp.m_FlapMoment[l]*pMainFrame->m_NmtoUnit, 9,'f',4);
			GetMomentUnit(str, pMainFrame->m_MomentUnit);
			strong += str +"\n";
			out << strong;
		}
		out << ("\n");
		m_pCurPOpp->m_FinWOpp.Export(&XFile, type);
	}


	if(m_pCurWOpp->m_AnalysisType>=2)
	{
		if(m_pCurPOpp)
		{
			out << ("Main Wing Cp Coefficients\n");

			if(type==1)
			{
				if(m_pCurWOpp->m_AnalysisType==2)		out << (" Panel     CtrlPt.x        CtrlPt.y        CtrlPt.z             Cp\n");
				else if(m_pCurWOpp->m_AnalysisType==3)	out << (" Panel     CollPt.x        CollPt.y        CollPt.z             Cp\n");
				for(p=0; p<m_pCurWing->m_MatSize; p++)
				{

					if(m_pCurWOpp->m_AnalysisType==2)
						strong = QString("%1     %2     %3     %4     %5\n")
						.arg(p,4).arg(m_Panel[p].CtrlPt.x,11,'e',3).arg(m_Panel[p].CtrlPt.y,11,'e',3).arg(m_Panel[p].CtrlPt.z,11,'e',3).arg( m_pCurWOpp->m_Cp[p],11,'f',3);
					else if(m_pCurWOpp->m_AnalysisType==3)
						strong = QString("%1     %2     %3     %4     %5\n")
						.arg(p,4).arg(m_Panel[p].CollPt.x,11,'e',3).arg(m_Panel[p].CollPt.y,11,'e',3).arg( m_Panel[p].CollPt.z,11,'e',3).arg(m_pCurWOpp->m_Cp[p],11,'f',3);

					out << strong;
				}
			}
			else
			{
				if(m_pCurWOpp->m_AnalysisType==2)		out << ("Panel,CtrlPt.x,CtrlPt.y,CtrlPt.z,Cp\n");
				else if(m_pCurWOpp->m_AnalysisType==3)	out << ("Panel,CollPt.x,CollPt.y,CollPt.z,Cp\n");
				for(p=0; p<m_pCurWing->m_MatSize; p++)
				{

					if(m_pCurWOpp->m_AnalysisType==2)
						strong = QString("%1,%2,%3,%4,%5\n")
										 .arg(p,4).arg(m_Panel[p].CtrlPt.x,11,'e',3).arg(m_Panel[p].CtrlPt.y,11,'e',3).arg(m_Panel[p].CtrlPt.z,11,'e',3).arg(m_pCurWOpp->m_Cp[p],11,'f',3);
					else if(m_pCurWOpp->m_AnalysisType==3)
						strong = QString("%1,%2,%3,%4,%5\n")
										 .arg(p,4).arg(m_Panel[p].CollPt.x,11,'e',3).arg( m_Panel[p].CollPt.y,11,'e',3).arg(m_Panel[p].CollPt.z,11,'e',3).arg(m_pCurWOpp->m_Cp[p],11,'f',3);

					out << strong;
				}
			}
			out << ("\n");

			if(m_pCurWing2)
			{
				out << ("Secondary Wing Cp Coefficients\n");

				if(type==1)
				{

					if(m_pCurWOpp->m_AnalysisType==2)		out << (" Panel     CtrlPt.x        CtrlPt.y        CtrlPt.z             Cp\n");
					else if(m_pCurWOpp->m_AnalysisType==3)	out << (" Panel     CollPt.x        CollPt.y        CollPt.z             Cp\n");
					for(p=0; p<m_pCurWing2->m_MatSize; p++)
					{

						if(m_pCurWOpp->m_AnalysisType==2)
							strong = QString("%1     %2     %3     %4     %5\n")
											 .arg(p,4).arg(m_pCurWing2->m_pPanel[p].CtrlPt.x,11,'e',3).arg(m_pCurWing2->m_pPanel[p].CtrlPt.y,11,'e',3).arg(m_pCurWing2->m_pPanel[p].CtrlPt.z,11,'e',3).arg(m_pCurWOpp->m_Cp[p],11,'f',3);
						else if(m_pCurWOpp->m_AnalysisType==3)
							strong = QString("%1     %2     %3     %4     %5\n")
											 .arg(p,4).arg(m_pCurWing2->m_pPanel[p].CollPt.x,11,'e',3).arg(m_pCurWing2->m_pPanel[p].CollPt.y,11,'e',3).arg(m_pCurWing2->m_pPanel[p].CollPt.z,11,'e',3).arg(m_pCurWOpp->m_Cp[p],11,'f',3);
						out << strong;
					}
				}
				else
				{
					if(m_pCurWOpp->m_AnalysisType==2)		out << ("Panel,CtrlPt.x,CtrlPt.y,CtrlPt.z,Cp\n");
					else if(m_pCurWOpp->m_AnalysisType==3)	out << ("Panel,CollPt.x,CollPt.y,CollPt.z             Cp\n");
					for(p=0; p<m_pCurWing2->m_MatSize; p++)
					{

						if(m_pCurWOpp->m_AnalysisType==2)
							strong = QString("%1,%11.3e,%11.3e,%11.3e,%11.3f\n").arg(p)
							.arg(m_pCurWing2->m_pPanel[p].CtrlPt.x).arg(m_pCurWing2->m_pPanel[p].CtrlPt.y).arg(m_pCurWing2->m_pPanel[p].CtrlPt.z).arg( m_pCurPOpp->m_Wing2WOpp.m_Cp[p]);
						else if(m_pCurWOpp->m_AnalysisType==3)
							strong = QString("%4d,%11.3e,%11.3e,%11.3e,%11.3f\n").arg(p)
							.arg(m_pCurWing2->m_pPanel[p].CollPt.x).arg(m_pCurWing2->m_pPanel[p].CollPt.y).arg(m_pCurWing2->m_pPanel[p].CollPt.z).arg(m_pCurPOpp->m_Wing2WOpp.m_Cp[p]);

						out << strong;
					}
				}
			}
			out << ("\n");

			if(m_pCurStab)
			{
				out << ("Elevator Cp Coefficients\n");

				if(type==1)
				{

					if(m_pCurWOpp->m_AnalysisType==2)		out << (" Panel     CtrlPt.x        CtrlPt.y        CtrlPt.z             Cp\n");
					else if(m_pCurWOpp->m_AnalysisType==3)	out << (" Panel     CollPt.x        CollPt.y        CollPt.z             Cp\n");
					for(p=0; p<m_pCurStab->m_MatSize; p++)
					{

						if(m_pCurWOpp->m_AnalysisType==2)
							strong = QString("%1     %2     %3     %4     %5\n")
											 .arg(p,4).arg(m_pCurStab->m_pPanel[p].CtrlPt.x,11,'e',3).arg(m_pCurStab->m_pPanel[p].CtrlPt.y,11,'e',3).arg(m_pCurStab->m_pPanel[p].CtrlPt.z,11,'e',3).arg(m_pCurWOpp->m_Cp[p],11,'f',3);
						else if(m_pCurWOpp->m_AnalysisType==3)
							strong = QString("%1     %2     %3     %4     %5\n")
											 .arg(p,4).arg(m_pCurStab->m_pPanel[p].CollPt.x,11,'e',3).arg(m_pCurStab->m_pPanel[p].CollPt.y,11,'e',3).arg(m_pCurStab->m_pPanel[p].CollPt.z,11,'e',3).arg(m_pCurWOpp->m_Cp[p],11,'f',3);
						out << strong;
					}
				}
				else
				{
					if(m_pCurWOpp->m_AnalysisType==2)		out << ("Panel,CtrlPt.x,CtrlPt.y,CtrlPt.z,Cp\n");
					else if(m_pCurWOpp->m_AnalysisType==3)	out << ("Panel,CollPt.x,CollPt.y,CollPt.z             Cp\n");
					for(p=0; p<m_pCurStab->m_MatSize; p++)
					{

						if(m_pCurWOpp->m_AnalysisType==2)
							strong = QString("%1,%11.3e,%11.3e,%11.3e,%11.3f\n").arg(p)
							.arg(m_pCurStab->m_pPanel[p].CtrlPt.x).arg(m_pCurStab->m_pPanel[p].CtrlPt.y).arg(m_pCurStab->m_pPanel[p].CtrlPt.z).arg( m_pCurPOpp->m_StabWOpp.m_Cp[p]);
						else if(m_pCurWOpp->m_AnalysisType==3)
							strong = QString("%4d,%11.3e,%11.3e,%11.3e,%11.3f\n").arg(p)
							.arg(m_pCurStab->m_pPanel[p].CollPt.x).arg(m_pCurStab->m_pPanel[p].CollPt.y).arg(m_pCurStab->m_pPanel[p].CollPt.z).arg(m_pCurPOpp->m_StabWOpp.m_Cp[p]);

						out << strong;
					}
				}
			}
			out << ("\n");

			if(m_pCurFin)
			{
				out << ("Elevator Cp Coefficients\n");

				if(type==1)
				{

					if(m_pCurWOpp->m_AnalysisType==2)		out << (" Panel     CtrlPt.x        CtrlPt.y        CtrlPt.z             Cp\n");
					else if(m_pCurWOpp->m_AnalysisType==3)	out << (" Panel     CollPt.x        CollPt.y        CollPt.z             Cp\n");
					for(p=0; p<m_pCurFin->m_MatSize; p++)
					{

						if(m_pCurWOpp->m_AnalysisType==2)
							strong = QString("%1     %2     %3     %4     %5\n")
											 .arg(p,4).arg(m_pCurFin->m_pPanel[p].CtrlPt.x,11,'e',3).arg(m_pCurFin->m_pPanel[p].CtrlPt.y,11,'e',3).arg(m_pCurFin->m_pPanel[p].CtrlPt.z,11,'e',3).arg(m_pCurWOpp->m_Cp[p],11,'f',3);
						else if(m_pCurWOpp->m_AnalysisType==3)
							strong = QString("%1     %2     %3     %4     %5\n")
											 .arg(p,4).arg(m_pCurFin->m_pPanel[p].CollPt.x,11,'e',3).arg(m_pCurFin->m_pPanel[p].CollPt.y,11,'e',3).arg(m_pCurFin->m_pPanel[p].CollPt.z,11,'e',3).arg(m_pCurWOpp->m_Cp[p],11,'f',3);
						out << strong;
					}
				}
				else
				{
					if(m_pCurWOpp->m_AnalysisType==2)		out << ("Panel,CtrlPt.x,CtrlPt.y,CtrlPt.z,Cp\n");
					else if(m_pCurWOpp->m_AnalysisType==3)	out << ("Panel,CollPt.x,CollPt.y,CollPt.z             Cp\n");
					for(p=0; p<m_pCurFin->m_MatSize; p++)
					{

						if(m_pCurWOpp->m_AnalysisType==2)
							strong = QString("%1,%11.3e,%11.3e,%11.3e,%11.3f\n").arg(p)
							.arg(m_pCurFin->m_pPanel[p].CtrlPt.x).arg(m_pCurFin->m_pPanel[p].CtrlPt.y).arg(m_pCurFin->m_pPanel[p].CtrlPt.z).arg( m_pCurPOpp->m_FinWOpp.m_Cp[p]);
						else if(m_pCurWOpp->m_AnalysisType==3)
							strong = QString("%4d,%11.3e,%11.3e,%11.3e,%11.3f\n").arg(p)
							.arg(m_pCurFin->m_pPanel[p].CollPt.x).arg(m_pCurFin->m_pPanel[p].CollPt.y).arg(m_pCurFin->m_pPanel[p].CollPt.z).arg(m_pCurPOpp->m_FinWOpp.m_Cp[p]);

						out << strong;
					}
				}
			}

			out << ("\n");

		}
		else if(m_pCurWOpp)
		{

			if(type==1)
			{
				if(m_pCurWOpp->m_AnalysisType==2)		out << (" Panel     CtrlPt.x        CtrlPt.y        CtrlPt.z             Cp\n");
				else if(m_pCurWOpp->m_AnalysisType==3)	out << (" Panel     CollPt.x        CollPt.y        CollPt.z             Cp\n");
				for(p=0; p<m_pCurWing->m_MatSize; p++)
				{

					if(m_pCurWOpp->m_AnalysisType==2)
						strong = QString("%1     %2     %3     %4     %5\n")
						.arg(p,4).arg(m_Panel[p].CtrlPt.x,11,'e',3).arg(m_Panel[p].CtrlPt.y,11,'e',3).arg(m_Panel[p].CtrlPt.z,11,'e',3).arg( m_pCurWOpp->m_Cp[p],11,'f',3);
					else if(m_pCurWOpp->m_AnalysisType==3)
						strong = QString("%1     %2     %3     %4     %5\n")
						.arg(p,4).arg(m_Panel[p].CollPt.x,11,'e',3).arg(m_Panel[p].CollPt.y,11,'e',3).arg( m_Panel[p].CollPt.z,11,'e',3).arg(m_pCurWOpp->m_Cp[p],11,'f',3);

					out << strong;
				}
			}
			else
			{
				if(m_pCurWOpp->m_AnalysisType==2)		out << ("Panel,CtrlPt.x,CtrlPt.y,CtrlPt.z,Cp\n");
				else if(m_pCurWOpp->m_AnalysisType==3)	out << ("Panel,CollPt.x,CollPt.y,CollPt.z,Cp\n");
				for(p=0; p<m_pCurWing->m_MatSize; p++)
				{

					if(m_pCurWOpp->m_AnalysisType==2)
						strong = QString("%1,%2,%3,%4,%5\n")
										 .arg(p,4).arg(m_Panel[p].CtrlPt.x,11,'e',3).arg(m_Panel[p].CtrlPt.y,11,'e',3).arg(m_Panel[p].CtrlPt.z,11,'e',3).arg(m_pCurWOpp->m_Cp[p],11,'f',3);
					else if(m_pCurWOpp->m_AnalysisType==3)
						strong = QString("%1,%2,%3,%4,%5\n")
										 .arg(p,4).arg(m_Panel[p].CollPt.x,11,'e',3).arg( m_Panel[p].CollPt.y,11,'e',3).arg(m_Panel[p].CollPt.z,11,'e',3).arg(m_pCurWOpp->m_Cp[p],11,'f',3);

					out << strong;
				}
			}
		}
	}
	out << ("\n\n");

	XFile.close();

}



void QMiarex::OnHalfWing(int state)
{
	m_bHalfWing = m_pctrlHalfWing->isChecked();
	if(m_iView==1)
	{
		SetWGraphScale();
		Set2DScale();
		OnAdjustToWing();
		UpdateView();
	}
}



void QMiarex::OnCurWOppOnly()
{
	m_bCurWOppOnly = !m_bCurWOppOnly;
	if(m_iView==1)
	{
		CreateWOppCurves();
		UpdateView();
	}
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

void QMiarex::OnDefineGraphVariables()
{
	//define the variables for the current graph
	GraphVariableDlg dlg;
	dlg.InitDialog(2);


	if(m_pCurGraph == &m_WPlrGraph1)
	{
		dlg.SetSelection(m_XW1, m_YW1);
		if(QDialog::Accepted == dlg.exec())
		{
			m_XW1 = dlg.m_XSel;
			m_YW1 = dlg.m_YSel;
//			dlg.GetSelection(m_XW1, m_YW1);
			SetWGraphTitles(&m_WPlrGraph1,m_XW1, m_YW1);
			m_WPlrGraph1.SetAuto(true);
			m_WPlrGraph1.SetAutoYMinUnit(true);
		}
	}
	else if(m_pCurGraph == &m_WPlrGraph2)
	{
		dlg.SetSelection(m_XW2, m_YW2);
		if(QDialog::Accepted == dlg.exec())
		{
			m_XW2 = dlg.m_XSel;
			m_YW2 = dlg.m_YSel;
			SetWGraphTitles(&m_WPlrGraph2,m_XW2, m_YW2);
			m_WPlrGraph2.SetAuto(true);
			m_WPlrGraph2.SetAutoYMinUnit(true);
		}
	}
	else if(m_pCurGraph == &m_WPlrGraph3)
	{
		dlg.SetSelection(m_XW3, m_YW3);
		if(QDialog::Accepted == dlg.exec())
		{
			m_XW3 = dlg.m_XSel;
			m_YW3 = dlg.m_YSel;
			SetWGraphTitles(&m_WPlrGraph3,m_XW3, m_YW3);
			m_WPlrGraph3.SetAuto(true);
			m_WPlrGraph3.SetAutoYMinUnit(true);
		}
	}
	else if(m_pCurGraph == &m_WPlrGraph4)
	{
		dlg.SetSelection(m_XW4, m_YW4);
		if(QDialog::Accepted == dlg.exec())
		{
			m_XW4 = dlg.m_XSel;
			m_YW4 = dlg.m_YSel;
			SetWGraphTitles(&m_WPlrGraph4,m_XW4, m_YW4);
			m_WPlrGraph4.SetAuto(true);
			m_WPlrGraph4.SetAutoYMinUnit(true);
		}
	}
	CreateWPolarCurves();
	UpdateView();
}

void QMiarex::OnDefineWPolar()
{
	if(!m_pCurWing) return;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	int i,j;

	CWPolar* pNewWPolar       = new CWPolar;
	if (m_pCurPlane) pNewWPolar->m_UFOName = m_pCurPlane->m_PlaneName;
	else             pNewWPolar->m_UFOName = m_pCurWing->m_WingName;
	pNewWPolar->m_WArea        = m_pCurWing->m_Area;
	pNewWPolar->m_WMAChord     = m_pCurWing->m_MAChord;
	pNewWPolar->m_WSpan        = m_pCurWing->m_Span;
//	m_WngAnalysis.m_pWPolar    = pNewWPolar;

	m_WngAnalysis.m_pMainFrame = m_pMainFrame;
	m_WngAnalysis.m_pWing       = m_pCurWing;
	m_WngAnalysis.m_pPlane      = m_pCurPlane;
	m_WngAnalysis.m_pMainFrame  = m_pMainFrame;
	m_WngAnalysis.m_poaWPolar   = m_poaWPolar;
	if(m_pCurPlane)		m_WngAnalysis.m_UFOName   = m_pCurPlane->m_PlaneName;
	else				m_WngAnalysis.m_UFOName   = m_pCurWing->m_WingName;
	m_WngAnalysis.InitDialog();

	if (m_WngAnalysis.exec() == QDialog::Accepted)
	{
/*		m_Ctrlm_WngAnalysis.m_QInf      = m_WngAnalysis.m_QInf;
		m_Ctrlm_WngAnalysis.m_Weight    = m_WngAnalysis.m_Weight;
		m_Ctrlm_WngAnalysis.m_Viscosity = m_WngAnalysis.m_Viscosity;
		m_Ctrlm_WngAnalysis.m_Density   = m_WngAnalysis.m_Density;*/

		//Then add WPolar to array
		pNewWPolar->m_Type            = m_WngAnalysis.m_Type;
		pNewWPolar->m_QInf            = m_WngAnalysis.m_QInf;
		pNewWPolar->m_Weight          = m_WngAnalysis.m_Weight;
		pNewWPolar->m_XCmRef          = m_WngAnalysis.m_XCmRef;
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

		pNewWPolar->m_bPolar       = true;

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
			delete pNewWPolar;
			m_pCurWPolar = pWPolar;
		}
		else
		{
			m_pCurWPolar = AddWPolar(pNewWPolar);
		}
		m_pCurPOpp = NULL;
		m_pCurWOpp = NULL;

		m_GL3dView.m_bResetglGeom = true;
		m_GL3dView.m_bResetglMesh = true;
		m_GL3dView.m_bResetglOpp  = true;
		m_GL3dView.m_bResetglWake = true;

		pMainFrame->UpdateWPolars();
		SetWPlr();
		UpdateView();
		m_pctrlAnalyze->setFocus();
	}
	else
	{
		delete pNewWPolar;
	}

}


void QMiarex::OnDeleteCurUFO()
{
	//Delete the currently selected wing, if any
	if(!m_pCurWing) return;
	m_bAnimate = false;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString strong;
	if(m_pCurPlane)	strong = "Are you sure you want to delete the plane :\n" +  m_pCurPlane->m_PlaneName +"?\n";
	else 	        strong = "Are you sure you want to delete the wing :\n" +   m_pCurWing->m_WingName +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(window(), "QFLR5", strong, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;


	if(m_pCurPlane) pMainFrame->DeletePlane(m_pCurPlane);
	else            pMainFrame->DeleteWing(m_pCurWing, false);

	pMainFrame->UpdateUFOs();
	SetUFO();
	CreateWPolarCurves();

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
		if (m_iView==1)     CreateWOppCurves();
//		else if(m_iView==4)	CreateCpCurves();

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
		if (m_iView==1)     CreateWOppCurves();
//		else if(m_iView==4)	CreateCpCurves();

		UpdateView();
	}
}

void QMiarex::OnDeleteCurWPolar()
{
	//delete the current WPolar
	if(!m_pCurWPolar) return;
	m_bAnimate = false;
	int i;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	QString strong, UFOName;

	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return;

	strong = "Are you sure you want to delete the polar :\n" +  m_pCurWPolar->m_PlrName +"?\n";
	if (QMessageBox::Yes != QMessageBox::question(window(), "QFLR5", strong, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel)) return;

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
	for (i=(int)m_poaWPolar->size()-1;i>=0; i--)
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
	pMainFrame->UpdateWPolars();
	SetWPlr();
	UpdateView();
}


void QMiarex::OnEditCurBody()
{
	if(!m_pCurBody) return;
}

void QMiarex::OnEditUFO()
{
	//Edit the currently selected wing
	if(!m_pCurWing)	return;
	if(m_iView==5) return;

	if(m_pCurPlane)
	{
		EditPlane(m_pCurPlane);
		return;
	}

	int i;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	CWPolar *pWPolar;
	CWOpp* pWOpp;
	WingDlg WingDlg;
	WingDlg.m_pWing = m_pCurWing;
	WingDlg.m_bAcceptName= false;
	WingDlg.InitDialog();

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


	if(QDialog::Accepted == WingDlg.exec())
	{
		if(WingDlg.m_bChanged)
		{
			if(bHasResults)
			{
				QString str = "The modification will erase all results associated to this Wing.\nContinue ?";

				int Ans = QMessageBox::question(this, "QFLR5", str, QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
				if (Ans == QMessageBox::Cancel)
				{
					//restore geometry for initial wing
					m_pCurWing->Duplicate(pSaveWing);
					delete pSaveWing; // clean up
					return;
				}
				else if(QMessageBox::No)
				{
					//create new wing
					CWing* pNewWing= new CWing();
					pNewWing->Duplicate(m_pCurWing);
					//restore geometry for initial wing
					m_pCurWing->Duplicate(pSaveWing);
					delete pSaveWing; // clean up

					if(!SetModWing(pNewWing)){

						delete pNewWing;
					}
					else //QMessageBox::Yes
					{
						m_pCurWing = AddWing(pNewWing);
						pMainFrame->SetSaveState(false);

					}
					SetUFO();
					pMainFrame->UpdateUFOs();
					UpdateView();
					return;
				}
			}

			//then modifications are automatically recorded
//			pMainFrame->DeleteWing(m_pCurWing, true);// will also set new surface and Aerochord in WPolars
			if(m_iView==3)
			{
				m_GL3dView.m_bResetglGeom = true;
				m_GL3dView.m_bResetglMesh = true;
//				m_bResetglOpp  = true;
			}
			else
			{
				if(m_iView==2)		CreateWPolarCurves();
				else if(m_iView==1)	CreateWOppCurves();
//				else if(m_iView==4)	CreateCpCurves();
			}
		}
		SetUFO();
		pMainFrame->UpdateWPolars();
		SetWPlr();
		SetScale();
		SetWGraphScale();
		OnAdjustToWing() ;
		UpdateView();
	}
	else
	{
		// restore original
		m_pCurWing->Duplicate(pSaveWing);
		UpdateView();
	}
	delete pSaveWing; // clean up
}



void QMiarex::OnFourWingGraphs()
{
	//The user has requested four wing graphs
	m_iWingView = 4;
	m_iView = 1;
//	CheckMenus();
	CreateWOppCurves();
	SetWingLegendPos();
	UpdateView();
}



void QMiarex::OnFourWPlrGraphs()
{
	m_iWPlrView = 4;

//	CheckMenus();
	CreateWPolarCurves();
	SetWPlrLegendPos();
	UpdateView();
}


void QMiarex::OnLButtonDown(QMouseEvent *event)
{
	TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QPoint point = event->pos();

	m_pCurGraph = GetGraph(point);
	if(m_rCltRect.contains(point)) p2DWidget->setFocus();

	if (m_pCurWing || (m_pCurGraph && m_pCurGraph->IsInDrawRect(point)))
	{
		m_LastPoint.rx() = point.x();
		m_LastPoint.ry() = point.y();

		m_bTrans = true;
		m_bTransGraph = true;
		if(m_pCurGraph && m_pCurGraph->IsInDrawRect(point)) m_bTransGraph = true;
		else                                                m_bTransGraph = false;
		if(m_bTrans || m_bTransGraph) 	p2DWidget->setCursor(Qt::ClosedHandCursor);
	}

	m_PointDown = point;
	m_LastPoint = point;
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
	CreateWPolarCurves();
	pMainFrame->SetSaveState(false);
	SetCurveParams();
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
		if (pWOpp->m_WingName == m_pCurWPolar->m_UFOName &&
			pWOpp->m_PlrName  == m_pCurWPolar->m_PlrName)
		{
			pWOpp->m_bIsVisible = false;
		}
	}
	for (i=0; i< m_poaPOpp->size(); i++)
	{
		pPOpp = (CPOpp*)m_poaPOpp->at(i);
		if (pPOpp->m_PlaneName == m_pCurWPolar->m_UFOName &&
			pPOpp->m_PlrName   == m_pCurWPolar->m_PlrName)
		{
			pPOpp->m_bIsVisible = false;
		}
	}
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->SetSaveState(false);
	SetCurveParams();

	if (m_iView==1)     CreateWOppCurves();
//	else if(m_iView==4)	CreateCpCurves();

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
	CreateWPolarCurves();
	SetCurveParams();
	pMainFrame->SetSaveState(false);
	UpdateView();
}



void QMiarex::OnInitLLTCalc(int state)
{
	m_bInitLLTCalc = m_pctrlInitLLTCalc->isChecked();
}



void QMiarex::OnNewBody()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	CBody *pBody = new CBody;

	m_GL3dBody.m_pBody = pBody;
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
	WingDlg WingDlg;
	WingDlg.m_pWing = pWing;
	WingDlg.m_bAcceptName= true;
	WingDlg.InitDialog();

	if(QDialog::Accepted == WingDlg.exec())
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
				return;
			}
		}

		m_pCurWing = AddWing(pWing);
		pMainFrame->SetSaveState(false);
		m_pCurPlane = NULL;
		SetUFO();
		if(m_iView==1)
		{
			SetScale();
			OnAdjustToWing();
		}
		SetScale();
		SetWGraphScale();

		pMainFrame->UpdateUFOs();
		UpdateView();
	}
	else
	{
		delete pWing;
	}
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
				return;
			}
		}

		m_pCurPlane = AddPlane(pPlane);
		pMainFrame->UpdateUFOs();
		SetUFO();
	}
	else
	{
		delete pPlane;
	}

	UpdateView();
}


void QMiarex::OnRenameCurWPolar()
{
	//Rename the currently selected Wing Polar

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(!m_pCurWPolar) return;
	if(!m_pCurWing) return;
	int resp, k,l;
	CWPolar* pWPolar;
	CPOpp * pPOpp;
	CWOpp * pWOpp;
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
	dlg.m_pstrArray = & NameList;

	dlg.m_strQuestion = "Enter the new name for the wing polar :";
	dlg.m_strName     = m_pCurWPolar->m_PlrName;

	bool bExists = true;

	while (bExists)
	{
		resp = dlg.exec();
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
	pMainFrame->UpdateWPolars();
//	SetWPlr();
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
//	m_bResetglLegend = true;
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


void QMiarex::OnSequence(int state)
{
	m_bSequence = m_pctrlSequence->isChecked();
	m_pctrlAlphaMax->setEnabled(m_bSequence);
	m_pctrlAlphaDelta->setEnabled(m_bSequence);
}
void QMiarex::OnStoreWOpp(int state)
{
	m_bStoreWOpp = m_pctrlStoreWOpp->isChecked();
}

void QMiarex::OnShowAllWOpps()
{
	int i;
	//Switch all WOpps view to on for the current UFO and WPolar
	m_bCurWOppOnly = false;

	CPOpp *pPOpp;
	CWOpp *pWOpp;
	for (i=0; i< m_poaWOpp->size(); i++)
	{
		pWOpp = (CWOpp*)m_poaWOpp->at(i);
		if (pWOpp->m_WingName == m_pCurWPolar->m_UFOName &&
			pWOpp->m_PlrName  == m_pCurWPolar->m_PlrName)
		{
			pWOpp->m_bIsVisible = true;
		}
	}
	for (i=0; i< m_poaPOpp->size(); i++)
	{
		pPOpp = (CPOpp*)m_poaPOpp->at(i);
		if (pPOpp->m_PlaneName == m_pCurWPolar->m_UFOName &&
			pPOpp->m_PlrName   == m_pCurWPolar->m_PlrName)
		{
			pPOpp->m_bIsVisible = true;
		}
	}
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->SetSaveState(false);
	SetCurveParams();

	if (m_iView==1)     CreateWOppCurves();
//	else if(m_iView==4)	CreateCpCurves();

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
	CreateWPolarCurves();
	pMainFrame->SetSaveState(false);
	SetCurveParams();
	UpdateView();
}


void QMiarex::OnShowLift(int state)
{
	m_bXCP	 = m_pctrlLift->isChecked();
	if(m_iView==1)
	{
		if(!m_bAnimate) UpdateView();
	}
}


void QMiarex::OnShowIDrag(int state)
{
	m_bICd = m_pctrlIDrag->isChecked();
	if(m_iView==1)
	{
		if(!m_bAnimate) UpdateView();
	}
}


void QMiarex::OnShowVDrag(int state)
{
	m_bVCd = m_pctrlVDrag->isChecked();
	if(m_iView==1)
	{
		if(!m_bAnimate) UpdateView();
	}
}


void QMiarex::OnShowTransitions(int state)
{
	m_bXTop = m_pctrlTrans->isChecked();
	m_bXBot = m_pctrlTrans->isChecked();
	if(m_iView==1)
	{
		if(!m_bAnimate) UpdateView();
	}
}

void QMiarex::OnShowCurve(bool checked)
{
	//user has toggled visible switch
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	if(m_iView==2)
	{
		if (m_pCurWPolar)
		{
			m_pCurWPolar->m_bIsVisible = checked;
		}
		CreateWPolarCurves();
	}
	else if (m_pCurWOpp)
	{
		m_pCurWOpp->m_bIsVisible = checked;
		CreateWOppCurves();
	}
	pMainFrame->SetSaveState(false);
	UpdateView();
}

void QMiarex::OnShowPoints(bool checked)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	if(m_iView ==2)
	{
		if (m_pCurWPolar)
		{
			m_pCurWPolar->m_bShowPoints = checked;
		}
		CreateWPolarCurves();
	}
	else if (m_iView==1 && m_pCurWOpp)
	{
		m_pCurWOpp->m_bShowPoints = checked;
		CreateWOppCurves();
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
	CreateWPolarCurves();
	SetCurveParams();
	pMainFrame->SetSaveState(false);
	UpdateView();
}



void QMiarex::OnSingleWPlrGraph1()
{
	m_iWPlrView = 1;
	m_pCurGraph = &m_WPlrGraph1;
	m_pCurWPlrGraph = m_pCurGraph;

	if(m_iView!=2) OnWPolars();
	else
	{
		SetWPlrLegendPos();
		UpdateView();
	}
}


void QMiarex::OnSingleWPlrGraph2()
{
	m_iWPlrView = 1;
	m_pCurGraph = &m_WPlrGraph2;
	m_pCurWPlrGraph = m_pCurGraph;

	if(m_iView!=2) OnWPolars();
	else
	{
		SetWPlrLegendPos();
		UpdateView();
	}
}


void QMiarex::OnSingleWPlrGraph3()
{
	m_iWPlrView = 1;
	m_pCurGraph = &m_WPlrGraph3;
	m_pCurWPlrGraph = m_pCurGraph;

	if(m_iView!=2) OnWPolars();
	else
	{
		SetWPlrLegendPos();
		UpdateView();
	}
}


void QMiarex::OnSingleWPlrGraph4()
{
	m_iWPlrView = 1;
	m_pCurGraph = &m_WPlrGraph4;
	m_pCurWPlrGraph = m_pCurGraph;

	if(m_iView!=2) OnWPolars();
	else
	{
		SetWPlrLegendPos();
		UpdateView();
	}
}


void QMiarex::OnTwoWPlrGraphs()
{
	m_iWPlrView = 2;
//	CheckMenus();
	CreateWPolarCurves();
	SetWPlrLegendPos();
	UpdateView();
}



void QMiarex::OnSingleWingGraph1()
{
	//The user has requested a single wing graph
	m_iWingView = 1;
	m_pCurGraph = &m_WingGraph1;
	m_iView = 1;
//	CheckMenus();
	CreateWOppCurves();
	SetWingLegendPos();
	UpdateView();
}

void QMiarex::OnSingleWingGraph2()
{
	//The user has requested a single wing graph
	m_iWingView = 1;
	m_pCurGraph = &m_WingGraph2;
	m_iView = 1;
//	CheckMenus();
	CreateWOppCurves();
	SetWingLegendPos();
	UpdateView();
}

void QMiarex::OnSingleWingGraph3()
{
	//The user has requested a single wing graph
	m_iWingView = 1;
	m_pCurGraph = &m_WingGraph3;
	m_iView = 1;
//	CheckMenus();
	CreateWOppCurves();
	SetWingLegendPos();
	UpdateView();
}

void QMiarex::OnSingleWingGraph4()
{
	//The user has requested a single wing graph
	m_iWingView = 1;
	m_pCurGraph = &m_WingGraph4;
	m_iView = 1;
//	CheckMenus();
	CreateWOppCurves();
	SetWingLegendPos();
	UpdateView();
}



void QMiarex::OnTwoWingGraphs()
{
	//The user has requested two wing graphs
	m_iWingView = 2;
	m_iView = 1;
//	CheckMenus();
	CreateWOppCurves();
	SetWingLegendPos();
	UpdateView();
}


void QMiarex::OnWOpps()
{
//	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	if(m_iView==1)
	{
		UpdateView();
		return;
	}


	m_pCurGraph = m_pCurWingGraph;
	m_iView=1;

	m_bIs2DScaleSet = false;
	Set2DScale();
	CreateWOppCurves();
	SetCurveParams();
	UpdateView();
}


void QMiarex::OnWPolars()
{
	if (m_bAnimate) StopAnimate();

	if(m_iView==2)
	{
		UpdateView();
		return;
	}

	SetWPlrLegendPos();//TODO remove
	m_iView=2;

	CreateWPolarCurves();
	SetCurveParams();
	UpdateView();
}



void QMiarex::PaintFourWingGraph(QPainter &painter)
{
	//Paint a four graph WOpp view
	int h  = m_rCltRect.height();
	int w  = m_rCltRect.width();
//	int h34 = (int)(3*h/4);
	int h38 = (int)(3*h/8);
	int w2 = (int)(w/2);
	int w3 = (int)(w/3);

	if(w3>150 && h>250)
	{

		if(m_rCltRect.width()<200 || m_rCltRect.height()<200) return;

		DrawWOppLegend(painter, m_WingLegendOffset, m_rCltRect.bottom());

		QRect Rect1(0,0,w2,h38);
		QRect Rect2(w2,0,w2,h38);
		QRect Rect3(0,h38,w2,h38);
		QRect Rect4(w2,h38,w2,h38);
		m_WingGraph1.DrawGraph(Rect1, painter);
		m_WingGraph2.DrawGraph(Rect2, painter);
		m_WingGraph3.DrawGraph(Rect3, painter);
		m_WingGraph4.DrawGraph(Rect4, painter);

	}
}


void QMiarex::PaintSingleWingGraph(QPainter &painter)
{
	painter.save();
	//Paint the current WOpp view
	int i;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	int margin = 10;
	int h  = m_rCltRect.height();
	int w  = m_rCltRect.width();
	int w2 = (int)(w/2);

// Write wing data

	QString Result, str, strong;
	QString str1;

	if (m_pCurGraph && m_pCurWing && w2>200 && h>250)
	{
		if(m_rCltRect.width()<200 || m_rCltRect.height()<200)
		{
			painter.restore();
			return;
		}
		m_pCurGraph->DrawGraph(m_rSingleRect, painter);
		QPoint Place(m_rCltRect.left()+10, m_rCltRect.top() +30);
		DrawWOppLegend(painter, Place, m_rCltRect.bottom());
	}

	painter.setFont(pMainFrame->m_TextFont);

	QPen TextPen(pMainFrame->m_TextColor);
	TextPen.setWidth(1);
	painter.setPen(TextPen);

	int D = 0;
	int LeftPos = margin;
	int ZPos    = m_rCltRect.height()-10*12;
	if(m_pCurWing)
	{
		double area = m_pCurWing->m_Area;
		if(m_pCurWing2)               area += m_pCurWing2->m_Area;
		if(m_pCurPlane && m_pCurStab) ZPos -= 14;
		if(m_pCurWPolar)              ZPos -= 24;
		if(m_iView!=2)
		{
			PaintWing(painter, m_ptOffset, m_WingScale);
			if(m_pCurWOpp && m_pCurWOpp->m_bIsVisible)
			{
				QPoint PtLegend;
				PtLegend.rx() = (int)(m_rCltRect.width()/2);
				PtLegend.ry() = m_rCltRect.bottom();
				PaintXTr(painter, m_ptOffset, PtLegend, m_WingScale);
				if (m_bXCP)    PaintXCP(painter, m_ptOffset,PtLegend, m_WingScale);
				if (m_bXCmRef) PaintXCmRef(painter, m_ptOffset, m_WingScale);
			}

			painter.drawText(LeftPos, ZPos, m_pCurWing->m_WingName);
			D+=12;
			QString length, surface;
			GetLengthUnit(length, pMainFrame->m_LengthUnit);
			GetAreaUnit(surface,  pMainFrame->m_AreaUnit);

			str1 = QString("Wing Span =   %1 ").arg(m_pCurWing->m_Span*pMainFrame->m_mtoUnit,12,'f',3);
			str1 += length;
			painter.drawText(LeftPos,ZPos+D, str1);
			D+=12;

			str1 = QString("WingArea =   %1 ").arg(area * pMainFrame->m_m2toUnit,13,'f',3);
			str1 += surface;
			painter.drawText(LeftPos,ZPos+D, str1);
			D+=12;

			if(m_pCurWPolar)
			{
				GetWeightUnit(str, pMainFrame->m_WeightUnit);
				Result = QString("Plane Weight =  %1 ").arg(m_pCurWing->m_Weight*pMainFrame->m_kgtoUnit,10,'f',3);
				Result += str;
				painter.drawText(LeftPos, ZPos+D, Result);
				D+=12;

				GetAreaUnit(strong, pMainFrame->m_AreaUnit);
				Result = QString("Wing Load =  %1 ").arg(m_pCurWing->m_Weight*pMainFrame->m_kgtoUnit/area/pMainFrame->m_m2toUnit,13,'f',3);
				Result += str + "/" + strong;
				painter.drawText(LeftPos, ZPos+D, Result);
				D+=12;
			}

			if(m_pCurPlane && m_pCurStab)
			{
				str1 = QString("Tail Volume =   %1").arg(m_pCurPlane->m_TailVolume,7,'f',2);
				painter.drawText(LeftPos, ZPos+D, str1);
				D+=12;
			}

			str1 = QString("Root Chord =   %1 ").arg(m_pCurWing->m_TChord[0]*pMainFrame->m_mtoUnit, 11,'f', 2);
			Result = str1+length;
			painter.drawText(LeftPos, ZPos+D, Result);
			D+=12;

			str1 = QString("MAC =          %1 ").arg(m_pCurWing->m_MAChord*pMainFrame->m_mtoUnit, 11,'f', 2);
			Result = str1+length;
			painter.drawText(LeftPos, ZPos+D, Result);
			D+=12;

			str1 = QString("TipTwist =   %1 deg").arg(m_pCurWing->m_TTwist[m_pCurWing->m_NPanel], 13,'f', 2);
			painter.drawText(LeftPos, ZPos+D, str1);
			D+=12;

			str1 = QString("Aspect Ratio =   %1").arg(m_pCurWing->m_AR,9,'f',2);
			painter.drawText(LeftPos, ZPos+D, str1);
			D+=12;

			str1 = QString("Taper Ratio =   %1").arg(m_pCurWing->m_TR,10,'f',2);
			painter.drawText(LeftPos, ZPos+D, str1);
			D+=12;

			str1 = QString("Root to Tip Sweep = %1 deg").arg(m_pCurWing->GetAverageSweep(),6,'f',2);
			painter.drawText(LeftPos, ZPos+D, str1);
		}
	}


	int dwidth,dheight;
	dwidth  = 220;
	dheight = 12;

	D = 0;
	int RightPos = m_rCltRect.right()-margin-dwidth;
	ZPos	 = m_rCltRect.height()-13*12;
	if(m_pCurWOpp && m_pCurWOpp->m_bOut) ZPos -= 12;
	if(m_pCurWOpp) ZPos -= 12*m_pCurWOpp->m_nFlaps;

	if(m_pCurWOpp && m_pCurWOpp->m_bIsVisible)
	{
		GetSpeedUnit(str, pMainFrame->m_SpeedUnit);
		int l = str.length();
		if(l==2)      Result = QString("QInf = %1 ").arg(m_pCurWOpp->m_QInf*pMainFrame->m_mstoUnit,7,'f',2);
		else if(l==3) Result = QString("QInf = %1 ").arg(m_pCurWOpp->m_QInf*pMainFrame->m_mstoUnit,6,'f',1);
		else if(l==4) Result = QString("QInf = %1 ").arg(m_pCurWOpp->m_QInf*pMainFrame->m_mstoUnit,5,'f',1);

		Result += str;
		D+=12;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString("Alpha = %1").arg(m_pCurWOpp->m_Alpha,8,'f',2);
		D+=12;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		if(m_pCurWOpp->m_bOut)
		{
			Result = "POint is out of the flight envelope";
			D+=12;
			painter.drawText(RightPos, ZPos+D, Result);
		}


		Result = QString("Lift Coef. = %1").arg(m_pCurWOpp->m_CL,8,'f',2);
		D+=12;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);


		Result = QString("Drag Coef. = %1").arg(m_pCurWOpp->m_ViscousDrag+m_pCurWOpp->m_InducedDrag,8,'f',2);
		D+=12;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		/*		oswald=CZ^2/CXi/pi/allongement;*/
		double cxielli=m_pCurWOpp->m_CL*m_pCurWOpp->m_CL/pi/m_pCurWing->m_AR;
		Result = QString("Efficiency = %1 ").arg(cxielli/m_pCurWOpp->m_InducedDrag,9,'f',4);
		D+=12;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString("Cl/Cd = %1 ").arg(m_pCurWOpp->m_CL/(m_pCurWOpp->m_InducedDrag+m_pCurWOpp->m_ViscousDrag),9,'f',4);
		D+=12;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString("GCm = %1 ").arg(m_pCurWOpp->m_GCm,9,'f',4);
		D+=12;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);


		Result = QString("Rolling Moment Coef. =%1").arg(m_pCurWOpp->m_GRm,9,'f',4);
		D+=12;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);


		Result = QString("Induced Moment Coef = %1").arg(m_pCurWOpp->m_IYm,9,'f',4);
		D+=12;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		Result = QString("Profile Yawing Moment = %1").arg(m_pCurWOpp->m_GYm,9,'f',4);
		D+=12;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		GetLengthUnit(str, pMainFrame->m_LengthUnit);
		l = str.length();
		if (l==1)     Result = QString("XCP = %8.3f ").arg(m_pCurWOpp->m_XCP*pMainFrame->m_mtoUnit,8,'f',3);
		else if(l==2) Result = QString("XCP = %7.2f ").arg(m_pCurWOpp->m_XCP*pMainFrame->m_mtoUnit,7,'f',2);
		else if(l>=3) Result = QString("XCP = %7.2f ").arg(m_pCurWOpp->m_XCP*pMainFrame->m_mtoUnit,7,'f',2);
		Result += str;
		D+=12;
		painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);

		for(i=0; i<m_pCurWOpp->m_nFlaps; i++)
		{
			Result = QString("Flap %1 Moment =%2").arg(i+1).arg(m_pCurWOpp->m_FlapMoment[i]*pMainFrame->m_NmtoUnit,9,'f',4);
			GetMomentUnit(str, pMainFrame->m_MomentUnit);
			Result += str;
			D+=12;
			painter.drawText(RightPos, ZPos+D, dwidth, dheight, Qt::AlignRight | Qt::AlignTop, Result);
		}
	}
	painter.restore();
}


void QMiarex::PaintTwoWingGraph(QPainter &painter)
{
	//Paint a double graph WOpp view
	int h   = m_rCltRect.height();
	int w   = m_rCltRect.width();
	int w2  = (int)(w/2);
	int w3  = (int)(w/3);
	int h23 = (int)(2*h/3);


	if(w3>150 && h>250)
	{

		if(m_rCltRect.width()<200 || m_rCltRect.height()<200) return;

		DrawWOppLegend(painter, m_WingLegendOffset, m_rCltRect.bottom());

		QRect Rect1(m_rCltRect.left(),   m_rCltRect.top(), w2, h23);
		QRect Rect2(w2, m_rCltRect.top(),                  w2, h23);
		m_WingGraph1.DrawGraph(Rect1, painter);
		m_WingGraph2.DrawGraph(Rect2, painter);

	}
}

void QMiarex::PaintView(QPainter &painter)
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	//Refresh the active view
	if(m_bAnimate)	return;// painting is performed elsewhere

	painter.fillRect(m_rCltRect, pMainFrame->m_BackgroundColor);

	if (m_iView==2)
	{
		if     (m_iWPlrView == 1) PaintWSingleGraph(painter);
		else if(m_iWPlrView == 2) PaintWCoupleGraphs(painter);
		else if(m_iWPlrView == 4) PaintWFourGraphs(painter);		
	}
	else if (m_iView==1)
	{
		PaintWOpp(painter);
	}
	else if (m_iView==4)
	{
//		PaintCp(painter);
	}
}


void QMiarex::PaintWing(QPainter &painter, QPoint ORef, double scale)
{
	//Draws the wing on the 2D WOpp view
	if(!m_pCurWing)	return;
	int i;
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	double scalex, scaley;

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



void QMiarex::PaintWSingleGraph(QPainter &painter)
{
	if(!m_pCurGraph) return;

	int h  = m_rCltRect.height();
	int w  = m_rCltRect.width();
	int w2 = (int)(w/2);

	int w3 = (int)(0.35*w);
	int w23 = 2*w3;

	QRect Rect1(0,0,w23,m_rCltRect.bottom()-00);
	if(w2>200 && h>250)
	{
		DrawWPolarLegend(painter, m_WPlrLegendOffset, Rect1.bottom());
		if(m_rCltRect.width()<200 || m_rCltRect.height()<200) return;//too small to paint
		m_pCurGraph->DrawGraph(Rect1, painter);
	}
}



void QMiarex::PaintWCoupleGraphs(QPainter &painter)
{
	int h  = m_rCltRect.height();
	int w  = m_rCltRect.width();
	int w2  = (int)(w/2);
	int h23 = (int)(2*h/3);

	if(w2>200 && h>250)
	{

		QRect Rect1(0,0,w2,h23);
		QRect Rect2(w2,0,w2,h23);
		if(m_rCltRect.width()<200 || m_rCltRect.height()<200) return;

		m_WPlrGraph1.DrawGraph(Rect1, painter);
		m_WPlrGraph2.DrawGraph(Rect2, painter);

		DrawWPolarLegend(painter, m_WPlrLegendOffset, m_rCltRect.bottom());
	}
}


void QMiarex::PaintWFourGraphs(QPainter &painter)
{
	int h  = m_rCltRect.height();
	int w  = m_rCltRect.width();
	int h2 = (int)(h/2);
	int w3 = (int)(0.35*w);
//	int w23 = 2*w3;

	if(w3>150 && h>250)
	{
		QRect Rect1(0,0,w3,h2);
		QRect Rect2(w3,0,w3,h2);
		QRect Rect3(0,h2,w3,h2);
		QRect Rect4(w3,h2,w3,h2);
		if(m_rCltRect.width()<200 || m_rCltRect.height()<200) return;

		m_WPlrGraph1.DrawGraph(Rect1, painter);
		m_WPlrGraph2.DrawGraph(Rect2, painter);
		m_WPlrGraph3.DrawGraph(Rect3, painter);
		m_WPlrGraph4.DrawGraph(Rect4, painter);

		DrawWPolarLegend(painter, m_WPlrLegendOffset, m_rCltRect.bottom());
	}
}



void QMiarex::PaintXTr(QPainter & painter, QPoint ORef, QPoint OLegend, double scale)
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
		painter.drawText(x-35, y - 48, "Top transition");

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
		painter.drawText(x-35, y - 33, "Bottom transition");

	}
	painter.restore();
}

void QMiarex::PaintXCP(QPainter & painter, QPoint ORef, QPoint OLegend, double scale)
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
	painter.drawText(x-35, y1 - 18, "Centre of Pressure");
	painter.restore();
}

void QMiarex::PaintXCmRef(QPainter & painter, QPoint ORef, double scale)
{
	//Draws the moment reference point on the 2D view
	if(!m_pCurWing || m_pCurWPolar)	return;
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
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

	QPen XCmRefPen;
	painter.setPen(XCmRefPen);


	int XCm = O.x() ;
	int YCm = O.y() + (int)(m_pCurWPolar->m_XCmRef*scaley);
	int size = 3;
	QRect CM(XCm-size, YCm-size, 2*size, 2*size);
	painter.drawEllipse(CM);

	painter.drawText(XCm+10, YCm-5, "Moment ref. location");

	painter.restore();
}

void QMiarex::PaintWOpp(QPainter &painter)
{
	//Paint the current WOpp view
	if (m_iWingView == 1 && m_pCurWing)      PaintSingleWingGraph(painter);
	else if (m_iWingView == 2 && m_pCurWing) PaintTwoWingGraph(painter);
	else if (m_iWingView == 4 && m_pCurWing) PaintFourWingGraph(painter);
}


void QMiarex::PanelAnalyze(double V0, double VMax, double VDelta, bool bSequence)
{
	if(!m_pCurWing || !m_pCurWPolar) return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	int i,pl, pr;


	m_pPanelDlg = new PanelAnalysisDlg;
	CWing::s_p3DPanelDlg     = m_pPanelDlg;

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

	m_pPanelDlg->m_pMiarex    = this;
	m_pPanelDlg->m_pMainFrame = m_pMainFrame;
	m_pPanelDlg->m_VersionName = pMainFrame->m_VersionName;


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


	m_pPanelDlg->m_pNode         = m_Node;
	m_pPanelDlg->m_pPanel        = m_Panel;
	m_pPanelDlg->m_ppPanel       = m_pPanel;
	m_pPanelDlg->m_pWakeNode     = m_WakeNode;
	m_pPanelDlg->m_pWakePanel    = m_WakePanel;
	m_pPanelDlg->m_pMemNode      = m_MemNode;
	m_pPanelDlg->m_pMemPanel     = m_MemPanel;
	m_pPanelDlg->m_pTempWakeNode = m_TempWakeNode;
	m_pPanelDlg->m_pRefWakeNode  = m_RefWakeNode;
	m_pPanelDlg->m_pRefWakePanel = m_RefWakePanel;
	m_pPanelDlg->m_RHS           = m_RHS;
	m_pPanelDlg->m_aij           = m_aij;
	m_pPanelDlg->m_pCoreSize     = &m_CoreSize;


	m_pPanelDlg->m_AlphaMin   = V0;
	m_pPanelDlg->m_AlphaMax   = VMax;
	m_pPanelDlg->m_AlphaDelta = VDelta;

	m_pPanelDlg->m_nNodes     = m_nNodes;
	m_pPanelDlg->m_NSurfaces  = m_NSurfaces;

	m_pPanelDlg->InitDialog();

	m_pPanelDlg->show();
	m_pPanelDlg->StartAnalysis();

	if(m_iView!=2) SetWOpp(false, V0);

	else CreateWPolarCurves();
	pMainFrame->UpdateWOpps();
	SetWOpp(true);
	delete m_pPanelDlg;
	m_pPanelDlg = NULL;

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

		pPlane->m_Wing.m_WingName = pPlane->m_PlaneName+"_Wing";
		pPlane->m_Wing2.m_WingName = pPlane->m_PlaneName+"_Wing2";
		pPlane->m_Stab.m_WingName = pPlane->m_PlaneName+"_Elev";
		pPlane->m_Fin.m_WingName = pPlane->m_PlaneName+"_Fin";

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
	double cosa = cos(Angle*pi/180.0);
	double sina = sin(Angle*pi/180.0);
	int iLA, iLB, iTA, iTB;
	CVector LATB, TALB, Pt, Ptr, Trans;

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
	double cosb = cos(Beta*pi/180.0);
	double sinb = sin(Beta*pi/180.0);
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


bool QMiarex::SaveSettings(QDataStream &ar)
{
	if(!m_GLLightDlg.SaveSettings(ar)) return false;

	ar << m_AlphaMin << m_AlphaMax << m_AlphaDelta << m_QInfMin << m_QInfMax << m_QInfDelta;
	ar << m_ControlMin << m_ControlMax << m_ControlDelta;

	ar << m_3DAxisStyle << m_3DAxisWidth << m_3DAxisColor;
	ar << m_VLMStyle << m_VLMWidth << m_VLMColor;
	ar << m_OutlineStyle << m_OutlineWidth << m_OutlineColor;
	ar << m_XCPStyle << m_XCPWidth << m_XCPColor;
	ar << m_MomentStyle << m_MomentWidth << m_MomentColor;
	ar << m_IDragStyle << m_IDragWidth << m_IDragColor;
	ar << m_VDragStyle << m_VDragWidth << m_VDragColor;
	ar << m_XTopStyle << m_XTopWidth << m_XTopColor;
	ar << m_XBotStyle << m_XBotWidth << m_XBotColor;
	ar << m_DownwashStyle << m_DownwashWidth << m_DownwashColor;
	ar << m_WakeStyle << m_WakeWidth << m_WakeColor;
	ar << m_CpColor << m_CpStyle << m_CpWidth;

	ar << CWing::s_CvPrec << CWing::s_RelaxMax << CWing::s_NLLTStations;

	ar << m_iView << m_iWingView << m_iWPlrView;	

	if      (m_pCurWingGraph==&m_WingGraph1) ar <<1;
	else if (m_pCurWingGraph==&m_WingGraph2) ar <<2;
	else if (m_pCurWingGraph==&m_WingGraph3) ar <<3;
	else if (m_pCurWingGraph==&m_WingGraph4) ar <<4;

	if      (m_pCurWPlrGraph==&m_WPlrGraph1) ar <<1;
	else if (m_pCurWPlrGraph==&m_WPlrGraph2) ar <<2;
	else if (m_pCurWPlrGraph==&m_WPlrGraph3) ar <<3;
	else if (m_pCurWPlrGraph==&m_WPlrGraph4) ar <<4;

	ar << m_XW1 << m_YW1 << m_XW2 << m_YW2 << m_XW3 << m_YW3 << m_XW4 << m_YW4;
	ar << m_WOppVar1 << m_WOppVar2 << m_WOppVar3 << m_WOppVar4; 
	ar << m_Iter << m_NStation ;			
	ar << m_InducedDragPoint;		
	ar << m_NHoopPoints << m_NXPoints;

	m_WingGraph1.Serialize(ar, true);			// the WOpp graphs
	m_WingGraph2.Serialize(ar, true);
	m_WingGraph3.Serialize(ar, true);
	m_WingGraph4.Serialize(ar, true);
	m_WPlrGraph1.Serialize(ar, true);			// the WPolar graphs
	m_WPlrGraph2.Serialize(ar, true);
	m_WPlrGraph3.Serialize(ar, true);
	m_WPlrGraph4.Serialize(ar, true);
	m_CpGraph.Serialize(ar, true);

	ar << m_bXTop << m_bXBot << m_bXCP << m_bXCmRef << m_bICd << m_bVCd;

	ar << m_GL3dView.m_bWakePanels << m_GL3dView.m_bSurfaces << m_GL3dView.m_bOutline << m_GL3dView.m_bVLMPanels;
	ar << m_GL3dView.m_bAxes  << m_GL3dView.m_b3DCp << m_GL3dView.m_bDownwash << m_GL3dView.m_bMoments;
	ar << m_GL3dView.m_bAutoCpScale << m_GL3dView.m_bShowCpScale << m_GL3dView.m_bXTop << m_GL3dView.m_bXBot << m_GL3dView.m_bXCP;

	m_GL3dBody.SaveSettings(ar);

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


	if(m_bHalfWing) m_ptOffset.rx() = m_rSingleRect.left() + m_WingGraph1.GetMargin();
	else            m_ptOffset.rx() = (int)(m_rCltRect.width()/2.0);
	m_ptOffset.ry() = m_rCltRect.bottom()-185;

	m_rSingleRect.setRect(40,10,m_rCltRect.right()-80,m_rCltRect.bottom()-230);

/*	if(m_iView==1 && m_iWingView==1)
	{
		m_WingGraph1.SetDrawRect(m_rSingleRect);
		m_WingGraph2.SetDrawRect(m_rSingleRect);
		m_WingGraph3.SetDrawRect(m_rSingleRect);
		m_WingGraph4.SetDrawRect(m_rSingleRect);
	}
	else if(m_iView==1 && m_iWingView==2)
	{
		QRect Rect1(m_rCltRect.left(),   m_rCltRect.top(), w2, h23);
		QRect Rect2(w2, m_rCltRect.top(),                  w2, h23);
		m_WingGraph1.SetDrawRect(Rect1);
		m_WingGraph2.SetDrawRect(Rect2);
	}
	else if(m_iView==1 && m_iWingView==4)
	{*/
	QRect Rect1(0,0,w2,h38);
	QRect Rect2(w2,0,w2,h38);
	QRect Rect3(0,h38,w2,h38);
	QRect Rect4(w2,h38,w2,h38);
	m_WingGraph1.SetDrawRect(Rect1);
	m_WingGraph2.SetDrawRect(Rect2);
	m_WingGraph3.SetDrawRect(Rect3);
	m_WingGraph4.SetDrawRect(Rect4);
//	}
	m_WingGraph1.Init();
	m_WingGraph2.Init();
	m_WingGraph3.Init();
	m_WingGraph4.Init();

	if(m_iView==1)      SetWingLegendPos();
	else if(m_iView==2) SetWPlrLegendPos();

	if(m_pCurWing)
	{
		m_WingScale = (m_rSingleRect.width()-2*m_WingGraph1.GetMargin())/m_pCurWing->m_Span;
		if(m_bHalfWing) 	m_WingScale *= 2.0;
		m_bIs2DScaleSet = true;
	}
}



void QMiarex::SetAnalysisParams()
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
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
//		m_pctrlSpec->setText("a");
//		m_pctrlSpec.SetFont(&m_SymbolFont);
		m_pctrlAlphaMin->SetValue(m_AlphaMin);
		m_pctrlAlphaMax->SetValue(m_AlphaMax);
		m_pctrlAlphaDelta->SetValue(m_AlphaDelta);
	}
	else if(m_pCurWPolar  && m_pCurWPolar->m_Type ==4)
	{
//		m_pctrlSpec->setText("VInf");
//		m_pctrlSpec.SetFont(&m_StdFont);
		m_pctrlAlphaMin->SetValue(m_QInfMin*pMainFrame->m_mstoUnit);
		m_pctrlAlphaMax->SetValue(m_QInfMax*pMainFrame->m_mstoUnit);
		m_pctrlAlphaDelta->SetValue(m_QInfDelta*pMainFrame->m_mstoUnit);
	}
	else if(m_pCurWPolar && (m_pCurWPolar->m_Type==5 || m_pCurWPolar->m_Type==6))
	{
//		m_pctrlSpec->setText("t");
//		m_pctrlSpec.SetFont(&m_StdFont);
		m_pctrlAlphaMin->SetValue(m_ControlMin);
		m_pctrlAlphaMax->SetValue(m_ControlMax);
		m_pctrlAlphaDelta->SetValue(m_ControlDelta);
	}
}


void QMiarex::SetCurveParams()
{
	if(m_iView==2)
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
	else if(m_iView==1)
	{
		//set Opoint params
		if(m_pCurWOpp)
		{
			if(m_pCurWOpp->m_bIsVisible)  m_pctrlShowCurve->setChecked(true);  else  m_pctrlShowCurve->setChecked(false);
			if(m_pCurWOpp->m_bShowPoints) m_pctrlShowPoints->setChecked(true); else  m_pctrlShowPoints->setChecked(false);

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
	RDlg.m_pstrArray = & NameList;
	RDlg.m_strQuestion = "Enter the new name for the Plane :";
	RDlg.m_strName     = pModPlane->m_PlaneName;
	RDlg.InitDialog();

	while (bExists)
	{
		resp = (int)RDlg.exec();
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
		else {
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
	RDlg.m_pstrArray = & NameList;
	RDlg.m_strQuestion = "Enter the new name for the wing :";
	RDlg.m_strName = pModWing->m_WingName;
	RDlg.InitDialog();

	while (bExists)
	{
		resp = RDlg.exec();
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
		{//user wants to overwrite
			pOldWing  = GetWing(RDlg.m_strName);
			pOldPlane = GetPlane(RDlg.m_strName);
			if(pOldWing){
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
				if(pOldPlane->m_bActive) QMessageBox::warning(window(), "QFLR5","Cannot overwrite current plane");
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
			m_pCurWing = pModWing;

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

	m_GL3dView.m_bResetglOpp    = true;
	m_GL3dView.m_bResetglWake   = true;
	m_GL3dView.m_bResetglStream = true;
	m_GL3dView.m_bResetglFlow   = true;
	m_GL3dView.m_bResetglLegend = true;

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

	if (m_iView==1)     CreateWOppCurves();
//	else if(m_iView==4)	CreateCpCurves();
	if(!m_pCurPOpp) return false;
	else            return true;
}


void QMiarex::SetScale()
{
	Set2DScale();
	if(m_GL3dView.isVisible()) m_GL3dView.Set3DScale();
}

void QMiarex::SetScale(QRect CltRect)
{
	m_rCltRect = CltRect;
//	m_rCltRect.setRect(CltRect.x(), CltRect.y(), CltRect.width(), CltRect.height());
	m_rSingleRect.setRect(CltRect.x(), CltRect.y(), CltRect.width(), CltRect.height());

//	m_rSingleRect.adjust(60,20,-60,-600);
//	SetBodyScale();
	Set2DScale();
	if(m_GL3dView.isVisible()) m_GL3dView.Set3DScale();
}



void QMiarex::SetUFO(QString UFOName)
{
	int i;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(!UFOName.length())
	{
		if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
		else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
		else
		{
			if(pMainFrame->m_pctrlUFO->count())
			{
				pMainFrame->m_pctrlUFO->setCurrentIndex(0);
				UFOName = pMainFrame->m_pctrlUFO->itemText(0);
			}
			else
			{
				m_pCurPlane = NULL;
				m_pCurBody  = NULL;
				m_pCurWing  = NULL;
				m_pCurWing2 = NULL;
				m_pCurStab  = NULL;
				m_pCurFin   = NULL;
				m_pCurWOpp  = NULL;
				m_pCurPOpp  = NULL;
				if(m_iView==2)      CreateWPolarCurves();
				else if(m_iView==1) CreateWOppCurves();
				m_GL3dView.SetPointers();
				return;
			}
		}
	}
	int pos = pMainFrame->m_pctrlUFO->findText(UFOName);
	if (pos>=0)
	{
		m_pCurPlane = GetPlane(UFOName);
		m_pCurWing  = GetWing(UFOName);
		pMainFrame->m_pctrlUFO->setCurrentIndex(pos);
	}
	pMainFrame->UpdateWPolars();

	if(!m_pCurWing && !m_pCurPlane)
	{
		m_pCurBody  = NULL;
		m_pCurWing2 = NULL;
		m_pCurStab  = NULL;
		m_pCurFin   = NULL;
		m_pCurWOpp  = NULL;
		m_pCurPOpp  = NULL;
		m_GL3dView.SetPointers();
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

	m_GL3dView.m_bResetglGeom   = true;
	if(m_pCurBody)
	{
		m_GL3dView.m_bResetglBody   = true;
		m_GL3dView.m_bResetglBody2D = true;
	}
	else
	{
		m_GL3dView.m_bResetglBody   = false;
		m_GL3dView.m_bResetglBody2D = false;
	}

	m_GL3dView.m_bResetglMesh = true;

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
		// try to set the same as the existing polar... Special for Marc
		SetWPlr(false, m_pCurWPolar->m_PlrName);
	else
		SetWPlr();

	m_GL3dView.SetPointers(m_pCurWing, m_pCurWing2, m_pCurStab, m_pCurFin, m_pCurBody, m_pCurPlane, m_pCurWPolar, m_pCurWOpp, m_pCurPOpp);
	SetScale();
	SetWGraphScale();
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
		double halfspan = m_pCurWing->m_Span/2.0;
		double xmin;
		if(m_bHalfWing) xmin = 0.0;
		else            xmin = -halfspan*pMainFrame->m_mtoUnit;

//first iteration is to set Min and Maj units

		m_WingGraph1.SetAuto(true);
		m_WingGraph1.ResetXLimits();
		m_WingGraph1.ResetYLimits();
		m_WingGraph2.SetAuto(true);
		m_WingGraph2.ResetXLimits();
		m_WingGraph2.ResetYLimits();
		m_WingGraph3.SetAuto(true);
		m_WingGraph3.ResetXLimits();
		m_WingGraph3.ResetYLimits();
		m_WingGraph4.SetAuto(true);
		m_WingGraph4.ResetXLimits();
		m_WingGraph4.ResetYLimits();

		m_WingGraph1.SetAutoX(false);
		m_WingGraph1.SetXMin(xmin);
		m_WingGraph1.SetXMax( halfspan*pMainFrame->m_mtoUnit);
		m_WingGraph1.SetXScale();
		m_WingGraph1.SetAutoXUnit();
		m_WingGraph1.SetXMax( halfspan*pMainFrame->m_mtoUnit);

		m_WingGraph2.SetAutoX(false);
		m_WingGraph2.SetXMin(xmin);
		m_WingGraph2.SetXMax( halfspan*pMainFrame->m_mtoUnit);
		m_WingGraph2.SetXScale();
		m_WingGraph2.SetAutoXUnit();
		m_WingGraph2.SetXMax( halfspan*pMainFrame->m_mtoUnit);

		m_WingGraph3.SetAutoX(false);
		m_WingGraph3.SetXMin(xmin);
		m_WingGraph3.SetXMax( halfspan*pMainFrame->m_mtoUnit);
		m_WingGraph3.SetXScale();
		m_WingGraph3.SetAutoXUnit();
		m_WingGraph3.SetXMax( halfspan*pMainFrame->m_mtoUnit);

		m_WingGraph4.SetAutoX(false);
		m_WingGraph4.SetXMin(xmin);
		m_WingGraph4.SetXMax( halfspan*pMainFrame->m_mtoUnit);
		m_WingGraph4.SetXScale();
		m_WingGraph4.SetAutoXUnit();
		m_WingGraph4.SetXMax( halfspan*pMainFrame->m_mtoUnit);
	}
}

void QMiarex::SetWPlr(bool bCurrent, QString WPlrName)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	CWPolar *pWPolar = NULL;
	CWPolar *pOldWPolar = NULL;
	QString UFOName;
	QString strong;
	int i,j,k,m, NStation;
	double SpanPos;

	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return;

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

	if(pWPolar && pWPolar == m_pCurWPolar)
	{
		SetWOpp(true);
	}
	else
	{
//		m_bResetglOpp  = true;
		m_GL3dView.m_bResetglMesh = true;
//		m_bResetglWake = true;

		if(!m_pCurWing)
		{
			pWPolar = NULL;
			m_pCurWPolar = NULL;
			SetAnalysisParams();
		}

		if(pWPolar) m_pCurWPolar = pWPolar;
		else if(m_pCurWing)
		{
			if (pMainFrame->m_pctrlWPolar->count())
			{
				// try to set the first in the list, if any
				strong = pMainFrame->m_pctrlWPolar->itemText(0);
				m_pCurWPolar = GetWPolar(strong);
			}
			else
			{
				m_pCurWPolar = NULL;
				SetAnalysisParams();
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
//		RotateGeomZ(m_pCurWPolar->m_Beta, RefPoint);
	}

	if(m_pCurWPolar && m_pCurWPolar->m_UFOName==UFOName)
	{
		m_pCurWing->ComputeChords();
		m_pCurWing->m_Type         = m_pCurWPolar->m_Type;
		m_pCurWing->m_Alpha        = m_pCurWPolar->m_ASpec;//in case its a Type 4 polar
		m_pCurWing->m_QInf         = m_pCurWPolar->m_QInf;
		m_pCurWing->m_Weight       = m_pCurWPolar->m_Weight;
		m_pCurWing->m_Density      = m_pCurWPolar->m_Density;
		m_pCurWing->m_Viscosity    = m_pCurWPolar->m_Viscosity;
		m_pCurWing->m_bVLM1        = m_pCurWPolar->m_bVLM1;
		m_pCurWing->m_bVLMSymetric = m_pCurWing->m_bSymetric;

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
				m_pCurWing2->m_Alpha        = m_pCurWPolar->m_ASpec;//in case its a Type 4 polar
				m_pCurWing2->m_QInf         = m_pCurWPolar->m_QInf;
				m_pCurWing2->m_Weight       = m_pCurWPolar->m_Weight;
				m_pCurWing2->m_Density      = m_pCurWPolar->m_Density;
				m_pCurWing2->m_Viscosity    = m_pCurWPolar->m_Viscosity;
				m_pCurWing2->m_bVLM1        = m_pCurWPolar->m_bVLM1;
				m_pCurWing2->m_bVLMSymetric = m_pCurWing2->m_bSymetric;

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
				m_pCurStab->m_Alpha        = m_pCurWPolar->m_ASpec;//in case its a Type 4 polar
				m_pCurStab->m_QInf         = m_pCurWPolar->m_QInf;
				m_pCurStab->m_Weight       = m_pCurWPolar->m_Weight;
				m_pCurStab->m_Density      = m_pCurWPolar->m_Density;
				m_pCurStab->m_Viscosity    = m_pCurWPolar->m_Viscosity;
				m_pCurStab->m_bVLM1        = m_pCurWPolar->m_bVLM1;
				m_pCurStab->m_bVLMSymetric = m_pCurStab->m_bSymetric;

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
				m_pCurFin->m_Alpha        = m_pCurWPolar->m_ASpec;//in case its a Type 4 polar
				m_pCurFin->m_QInf         = m_pCurWPolar->m_QInf;
				m_pCurFin->m_Weight       = m_pCurWPolar->m_Weight;
				m_pCurFin->m_Density      = m_pCurWPolar->m_Density;
				m_pCurFin->m_Viscosity    = m_pCurWPolar->m_Viscosity;
				m_pCurFin->m_bVLM1        = m_pCurWPolar->m_bVLM1;
				m_pCurFin->m_bVLMSymetric = m_pCurFin->m_bSymetric;

				NStation = 0;
				m=0;
				SpanPos = 0;
				for (j=0; j<m_pCurFin->m_NSurfaces; j++)	NStation += m_pCurFin->m_Surface[j].m_NYPanels;

				for (j=(int)m_pCurFin->m_NSurfaces/2; j<m_pCurFin->m_NSurfaces; j++)
				{
					for(k=0; k<m_pCurFin->m_Surface[j].m_NYPanels; k++)
					{
						m_pCurFin->m_SpanPos[m+NStation/2] = SpanPos + m_pCurFin->m_Surface[j].GetStripSpanPos(k);
						m++;
					}
					SpanPos += m_pCurFin->m_Surface[j].m_Length;
				}

				for(m=0; m<NStation/2; m++) m_pCurFin->m_SpanPos[m] = -m_pCurFin->m_SpanPos[NStation-m-1];
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

	SetAnalysisParams();
	SetCurveParams();
	CreateWPolarCurves();
	m_GL3dView.SetWPlrPtr(m_pCurWPolar);


/*	m_pPanelDlg->m_pWPolar     = m_pCurWPolar;
        m_pPanelDlg->m_MatSize     = m_MatSize;
        m_pPanelDlg->m_nNodes      = m_nNodes;
        m_pPanelDlg->m_NSurfaces   = m_NSurfaces;
        m_pPanelDlg->m_NWakeColumn = m_NWakeColumn;*/
}



void QMiarex::SetWingLegendPos()
{
	int h  = m_rCltRect.height();
	int h23 = (int)(2*h/3);
	int h34 = (int)(3*h/4);

	int margin = 10;
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

void QMiarex::SetWPlrLegendPos()
{
	int h  = m_rCltRect.height();
	int w  = m_rCltRect.width();
	int h23 = (int)(2*h/3);
//	int w23 = (int)(2*w/3);

	int margin = 10;
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


bool QMiarex::SetWOpp(bool bCurrent, double Alpha)
{
	if(!m_pCurWPolar)
	{
		m_pCurWOpp = NULL;
		SetCurveParams();
		m_GL3dView.SetWOppPtr(m_pCurWOpp);
		return false;
	}
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	// set set the current WOpp, if any
	// else set the comboBox's first, if any
	// else set it to NULL
	QString strong;

	if(!m_pCurWing || !m_pCurWPolar)
	{
		m_pCurWOpp = NULL;
		m_pCurPOpp = NULL;
		return false;
	}
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

	m_GL3dView.m_bResetglOpp    = true;
	m_GL3dView.m_bResetglWake   = true;
	m_GL3dView.m_bResetglStream = true;
	m_GL3dView.m_bResetglFlow   = true;
	m_GL3dView.m_bResetglLegend = true;

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

	if (m_iView==1)     CreateWOppCurves();
//	else if(m_iView==4)	CreateCpCurves();
	if(!m_pCurWOpp) return false;
	else            return true;
}


void QMiarex::SetWGraphTitles(Graph* pGraph, int iX, int iY)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString StrLength;
	QString StrSpeed;
	QString StrMoment;
	GetLengthUnit(StrLength, pMainFrame->m_LengthUnit);
	GetSpeedUnit(StrSpeed, pMainFrame->m_SpeedUnit);
	GetMomentUnit(StrMoment, pMainFrame->m_MomentUnit);

	switch (iX)
	{
		case 0:
			pGraph->SetXTitle("Alpha");
			break;
		case 1:
			pGraph->SetXTitle("Cl");
			break;
		case 2:
			pGraph->SetXTitle("VCd");
			break;
		case 3:
			pGraph->SetXTitle("ICd");
			break;
		case 4:
			pGraph->SetXTitle("Cd");
			break;
		case 5:
			pGraph->SetXTitle("GCm");// Total Pitching moment coef.
			break;
		case 6:
			pGraph->SetXTitle("GRm");// Total Rolling moment coef.
			break;
		case 7:
			pGraph->SetXTitle("GYm");// Total Yawing moment coef.
			break;
		case 8:
			pGraph->SetXTitle("VYm");// Profile yawing moment
			break;
		case 9:
			pGraph->SetXTitle("IYm");// Induced yawing moment
			break;
		case 10:
			pGraph->SetXTitle("Cl/Cd");
			break;
		case 11:
			pGraph->SetXTitle("Cl^(3/2)/Cd");
			break;
		case 12:
			pGraph->SetXTitle("1/Rt(Cl)");
			break;
		case 13:
			if(pMainFrame->m_ForceUnit==0)	pGraph->SetXTitle("Lift (N)");
			else pGraph->SetXTitle("Lift (lbf)");
			break;
		case 14:
			if(pMainFrame->m_ForceUnit==0)	pGraph->SetXTitle("Drag (N)");
			else pGraph->SetXTitle("Drag (lbf)");
			break;
		case 15:
			pGraph->SetXTitle("Vx "+StrSpeed);
			break;
		case 16:
			pGraph->SetXTitle("Vz "+StrSpeed);
			break;
		case 17:
			pGraph->SetXTitle("V "+StrSpeed);
			break;
		case 18:
			pGraph->SetXTitle("Gamma");
			break;
		case 19:
			pGraph->SetXTitle("PM");
			break;
		case 20:
			pGraph->SetXTitle("RM");
			break;
		case 21:
			pGraph->SetXTitle("YM");
			break;
		case 22:
			pGraph->SetXTitle("XCP "+ StrLength);
			break;
		case 23:
			pGraph->SetXTitle("YCP "+ StrLength);
			break;
		case 24:
			pGraph->SetXTitle("BM "+ StrMoment);
			break;
		case 25:
			pGraph->SetXTitle("m.g.Vz (W)");
			break;
		case 26:
			pGraph->SetXTitle("Efficiency");
			break;
		case 27:
			pGraph->SetXTitle("(XCp-XCG)/MAC(%)");
			break;
		case 28:
			pGraph->SetXTitle("ctrl");
			break;
		case 29:
			pGraph->SetXTitle("CY");
			break;
		default:
			pGraph->SetXTitle("Alpha");
			break;
	}

	switch (iY)
	{
		case 0:
			pGraph->SetYTitle("Alpha");
			break;
		case 1:
			pGraph->SetYTitle("Cl");
			break;
		case 2:
			pGraph->SetYTitle("VCd");
			break;
		case 3:
			pGraph->SetYTitle("ICd");
			break;
		case 4:
			pGraph->SetYTitle("Cd");
			break;
		case 5:
			pGraph->SetYTitle("GCm");// Total Pitching moment coef.
			break;
		case 6:
			pGraph->SetYTitle("GRm");// Total Rolling moment coef.
			break;
		case 7:
			pGraph->SetYTitle("GYm");// Total Yawing moment coef.
			break;
		case 8:
			pGraph->SetYTitle("VYm");// Profile yawing moment
			break;
		case 9:
			pGraph->SetYTitle("IYm");// Induced yawing moment
			break;
		case 10:
			pGraph->SetYTitle("Cl/Cd");
			break;
		case 11:
			pGraph->SetYTitle("Cl^(3/2)/Cd");
			break;
		case 12:
			pGraph->SetYTitle("1/Rt(Cl)");
			break;
		case 13:
			if(pMainFrame->m_ForceUnit==0)	pGraph->SetYTitle("Lift (N)");
			else pGraph->SetYTitle("Lift (lbf)");
			break;
		case 14:
			if(pMainFrame->m_ForceUnit==0)	pGraph->SetYTitle("Drag (N)");
			else pGraph->SetYTitle("Drag (lbf)");
			break;
		case 15:
			pGraph->SetYTitle("Vx "+StrSpeed);
			break;
		case 16:
			pGraph->SetYTitle("Vz "+StrSpeed);
			break;
		case 17:
			pGraph->SetYTitle("V "+StrSpeed);
			break;
		case 18:
			pGraph->SetYTitle("Gamma");
			break;
		case 19:
			pGraph->SetYTitle("PM");
			break;
		case 20:
			pGraph->SetYTitle("RM");
			break;
		case 21:
			pGraph->SetYTitle("YM");
			break;
		case 22:
			pGraph->SetYTitle("XCP "+ StrLength);
			break;
		case 23:
			pGraph->SetYTitle("YCP "+ StrLength);
			break;
		case 24:
			pGraph->SetYTitle("BM "+ StrMoment);
			break;
		case 25:
			pGraph->SetYTitle("m.g.Vz (W)");
			break;
		case 26:
			pGraph->SetYTitle("Efficiency");
			break;
		case 27:
			pGraph->SetYTitle("(XCp-XCG)/MAC(%)");
			break;
		case 28:
			pGraph->SetYTitle("ctrl var");
			break;
		case 29:
			pGraph->SetYTitle("CY");
			break;
		default:
			pGraph->SetYTitle("Alpha");
			break;
	}
}

void QMiarex::showEvent(QShowEvent *event)
{
	SetAnalysisParams();
	SetCurveParams();
}

void QMiarex::StopAnimate()
{
	if(!m_bAnimate) return;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	m_bAnimate = false;
	m_pctrlAnimate->setChecked(false);

	if(m_posAnimate<0 || m_posAnimate>=m_poaWOpp->size()) return ;
	CWOpp* pWOpp = (CWOpp*)m_poaWOpp->at(m_posAnimate);
	if(!pWOpp) return;
	SetWOpp(false, pWOpp->m_Alpha);

	if(m_pCurWOpp)
	{
		QString str;
		if(m_pCurWPolar->m_Type != 4) str = QString("%1").arg(m_pCurWOpp->m_Alpha,8,'f',2);
		else                          str = QString("%1").arg(m_pCurWOpp->m_QInf, 8,'f',2);

		int pos = pMainFrame->m_pctrlWOpp->findText(str);
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
		pMainFrame->m_pctrlWOpp->setCurrentIndex(0);
	}
}




void QMiarex::UpdateCurve()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(m_iView==2 && m_pCurWPolar)
	{
		m_pCurWPolar->m_Color = m_CurveColor;
		m_pCurWPolar->m_Style = m_CurveStyle;
		m_pCurWPolar->m_Width = (int)m_CurveWidth;
		CreateWPolarCurves();
	}
	else if (m_iView==1 && m_pCurWOpp)
	{
		m_pCurWOpp->m_Color = m_CurveColor;
		m_pCurWOpp->m_Style = m_CurveStyle;
		m_pCurWOpp->m_Width = (int)m_CurveWidth;
		CreateWOppCurves();
	}
	UpdateView();
	pMainFrame->SetSaveState(false);
}



void QMiarex::UpdateUnits()
{
	//The units have been changed, so update the graphs and the views
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(m_iView==2)
	{
		CreateWPolarCurves();
		SetWGraphTitles(&m_WPlrGraph1, m_XW1,m_YW1);
		SetWGraphTitles(&m_WPlrGraph2, m_XW2,m_YW2);
		SetWGraphTitles(&m_WPlrGraph3, m_XW3,m_YW3);
		SetWGraphTitles(&m_WPlrGraph4, m_XW4,m_YW4);
		UpdateView();
	}
	else
	{
		if(!m_pCurWing) return;
		if (m_iView==1)
		{
			CreateWOppCurves();
			OnAdjustToWing();
		}
//		else if(m_iView==4)	CreateCpCurves();
	}
//	m_bResetglOpp = true;
//	m_bResetglGeom = true;
	UpdateView();
//	m_FlowLinesDlg.SetUnits();
//	m_FlowLinesDlg.SetParams();
}

void QMiarex::UpdateView()
{
	TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;
	if(p2DWidget) p2DWidget->update();
	if(m_GL3dView.isVisible()) m_GL3dView.UpdateView();
}




void QMiarex::VLMAnalyze(double V0, double VMax, double VDelta, bool bSequence)
{
	if(!m_pCurWing || !m_pCurWPolar) return;

        MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

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

	m_pVLMDlg->m_pMiarex    = this;
	m_pVLMDlg->m_pMainFrame = m_pMainFrame;

	m_pVLMDlg->m_pPlane     = m_pCurPlane;
	m_pVLMDlg->m_pWing      = m_pCurWing;
	m_pVLMDlg->m_pWPolar    = m_pCurWPolar;
	m_pVLMDlg->m_bSequence  = bSequence;

	m_pVLMDlg->m_AlphaMin   = V0;
	m_pVLMDlg->m_AlphaMax   = VMax;
	m_pVLMDlg->m_AlphaDelta = VDelta;

	m_pVLMDlg->m_MatSize    = m_MatSize;
	m_pVLMDlg->m_nNodes     = m_nNodes;
	m_pVLMDlg->m_NSurfaces  = m_NSurfaces;

	m_pVLMDlg->InitDialog();

	m_pVLMDlg->show();
	m_pVLMDlg->StartAnalysis();
//	m_pVLMDlg->exec();

	if(m_iView!=2) SetWOpp(false, V0);

	else CreateWPolarCurves();
	pMainFrame->UpdateWOpps();
	SetWOpp(true);
	delete m_pVLMDlg;
	m_pVLMDlg = NULL;

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

	if(m_iView ==1 || m_iView ==2 || m_iView ==4 )
	{
		m_pCurGraph = GetGraph(pt);
		if(m_pCurGraph && m_pCurGraph->IsInDrawRect(pt))
		{
//			SHORT sh1 = GetKeyState('X');
//			SHORT sh2 = GetKeyState('Y');

/*			if (sh1 & 0x8000)
			{
				//zoom x scale
				m_pCurGraph->SetAutoX(false);
				if(zDelta>0) m_pCurGraph->Scalex(1.06);
				else m_pCurGraph->Scalex(1.0/1.06);
			}
			else if(sh2 & 0x8000)
			{
				//zoom y scale
				m_pCurGraph->SetAutoY(false);
				if(zDelta>0) m_pCurGraph->Scaley(1.06);
				else m_pCurGraph->Scaley(1.0/1.06);
			}
			else*/
			{
				//zoom both
				m_pCurGraph->SetAuto(false);
				if(event->delta()>0) m_pCurGraph->Scale(1.06);
				else                 m_pCurGraph->Scale(1.0/1.06);
			}
			UpdateView();

		}
		else if(m_pCurWing && m_iView==1)
		{
			if(event->delta()<0) m_WingScale *= 1.06;
			else                 m_WingScale /= 1.06;
			UpdateView();
		}
	}
}













