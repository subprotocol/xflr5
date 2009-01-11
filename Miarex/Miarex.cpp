/****************************************************************************

	Miarex    Copyright (C) 2008 Andre Deperrois XFLR5@yahoo.com

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
#include "MainFrame.h"
#include <math.h>

QMiarex::QMiarex(QWidget *parent)
	: QWidget(parent)
{
	//construct evrything
	pi = 3.141592654;

//	m_hcArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
//	m_hcCross = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
//	m_hcMove  = AfxGetApp()->LoadCursor(IDC_HMOVE);

	m_pMainFrame       = NULL;
	m_pXFile       = NULL;

	m_pCurPlane = NULL;
	m_pCurWing2 = NULL;
	m_pCurStab  = NULL;
	m_pCurFin   = NULL;
	m_pCurPOpp  = NULL;
	m_pCurFrame = NULL;
	m_pCurBody  = NULL;

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

	m_CoreSize        = 0.000001;
	m_MaxWakeIter     = 5;
	m_WakeInterNodes  = 6;
	m_bResetWake      = true;
	m_bSetNewWake     = true;
	m_bVLMFinished    = true;

	m_nWakeNodes = 0;
	m_WakeSize   = 0;
	m_LastPoint.setX(0);
	m_LastPoint.setY(0);
	m_WPlrLegendOffset.setX(0);
	m_WPlrLegendOffset.setY(0);

	m_BodyOffset.Set( 0.20, -0.12, 0.0);
	m_FrameOffset.Set(0.80, -0.50, 0.0);
	m_HorizontalSplit = -0.45;
	m_VerticalSplit   =  0.35;

	m_rCltRect.setWidth(0);
	m_rCltRect.setHeight(0);
	m_rDrawRect.setWidth(0);
	m_rDrawRect.setHeight(0);
	m_rSingleRect.setWidth(0);
	m_rSingleRect.setHeight(0);

	m_MinPanelSize = 0.001;//m

	m_LastWOpp      = 0.0;
	m_CurSpanPos    = 0.0;
	m_WingScale     = 1.0;
	m_BodyScale     = 1.0;
	m_BodyRefScale  = 1.0;
	m_FrameScale    = 1.0;
	m_FrameRefScale = 1.0;

	m_ClipPlanePos = 5.0;

	m_NXPoints    = 30;
	m_NHoopPoints = 20;

	m_LegendMax  =  1.0;
	m_LegendMin  = -1.0;

	m_glViewportTrans.x  = 0.0;
	m_glViewportTrans.y  = 0.0;
	m_glViewportTrans.z  = 0.0;
	m_glScaled   = 1.0;
	m_GLScale    = 1.0;

	m_ptOffset.rx() = 0;
	m_ptOffset.ry() = 0;

	m_InducedDragPoint = 0;

	CPanel::s_pNode = m_Node;
//	CWing::s_pLLTDlg     = &m_LLTDlg;    //pointer to the VLM analysis dialog class
//	CWing::s_pVLMDlg     = &m_VLMDlg;    //pointer to the VLM analysis dialog class
//	CWing::s_p3DPanelDlg = &m_PanelDlg;  //pointer to the 3DPanel analysis dialog class

	CWing::m_pWakePanel = m_WakePanel;
	CWing::m_pWakeNode = m_WakeNode;

	CSurface::s_pPanel = m_Panel;
	CSurface::s_pNode  = m_Node;

/*	m_ArcBall.m_pGLScale = &m_GLScale;
	m_ArcBall.m_pOffx    = &m_UFOOffset.x;
	m_ArcBall.m_pOffy    = &m_UFOOffset.y;
	m_ArcBall.m_pTransx  = &m_glViewportTrans.x;
	m_ArcBall.m_pTransy  = &m_glViewportTrans.y;*/

	m_GLList = 0;
	m_bBodyOverlay        = false;
	m_bResetglBody        = false;//otherwise endless repaint if no body present
	m_bResetglBody2D      = false;//
	m_bResetglBodyPoints  = false;
	m_bResetglBody2D      = false;
	m_bResetglBodyOverlay = true;
	m_bResetglBodyMesh    = true;

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

	m_bDirichlet         = true;
	m_bTrefftz           = true;
	m_bWakePanels        = false;
	m_bArcball           = false;
	m_bCrossPoint        = false;
	m_bSurfaces          = true;
	m_bOutline           = true;
	m_bVLMPanels         = false;
	m_bAxes              = true;
	m_bglLight           = true;
	m_b3DCp              = false;
	m_b3DDownwash        = true;
	m_bXTop              = true;
	m_bXBot              = true;
	m_bXCP               = true;
	m_bMoments           = true;
	m_bICd               = true;
	m_bVCd               = true;
	m_bStream            = false;
	m_bVortices          = false;
	m_bSpeeds            = false;
	m_bXCmRef            = true;
	m_bAutoCpScale	     = false;
	m_bShowCpScale       = true;
	m_bShowLight         = false;
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
	m_bIs3DScaleSet      = false;
	m_bAutoScales        = false;
	m_bAnimate           = false;
	m_bAnimatePlus       = true;
	m_bPickCenter        = false;

	m_posAnimate         = 0;

	m_pCurWPolar  = NULL;
	m_pCurWing    = NULL;
	m_pCurWOpp    = NULL;
	m_pCurGraph   = NULL;

//	m_PanelDlg.m_ppBody = &m_pCurBody;

	m_pCurWingGraph = &m_WingGraph1;
	m_pCurWPlrGraph = &m_WPlrGraph1;

/*	m_WingGraph1.SetAutoX(false);
	m_WingGraph1.SetXUnit(10.0);
	m_WingGraph1.SetXMin(-10.0);
	m_WingGraph1.SetXMax( 10.0);
	m_WingGraph1.SetYMin(0.000);
	m_WingGraph1.SetYMax(0.001);
	m_WingGraph1.SetXMajGrid(true, RGB(120,120,120),2,1);
	m_WingGraph1.SetXMinGrid(false, true, RGB(80,80,80),2, 1, 100.0);
	m_WingGraph1.SetYMajGrid(true, RGB(120,120,120),2,1);
	m_WingGraph1.SetYMinGrid(false, true, RGB(80,80,80),2, 1, 0.1);
	m_WingGraph1.SetType(1);

	m_WingGraph2.SetAutoX(false);
	m_WingGraph2.SetXUnit(50.0);
	m_WingGraph2.SetXMin(-10.0);
	m_WingGraph2.SetXMax( 10.0);
	m_WingGraph2.SetYMin(0.000);
	m_WingGraph2.SetYMax(0.001);
	m_WingGraph2.SetXMajGrid(true, RGB(120,120,120),2,1);
	m_WingGraph2.SetXMinGrid(false, true, RGB(80,80,80),2, 1, 100.0);
	m_WingGraph2.SetYMajGrid(true, RGB(120,120,120),2,1);
	m_WingGraph2.SetYMinGrid(false, true, RGB(80,80,80),2, 1, 0.1);
	m_WingGraph2.SetType(1);

	m_WingGraph3.SetAutoX(false);
	m_WingGraph3.SetXUnit(50.0);
	m_WingGraph3.SetXMin(-10.0);
	m_WingGraph3.SetXMax( 10.0);
	m_WingGraph3.SetYMin(0.000);
	m_WingGraph3.SetYMax(0.001);
	m_WingGraph3.SetXMajGrid(true, RGB(120,120,120),2,1);
	m_WingGraph3.SetXMinGrid(false, true, RGB(80,80,80),2, 1, 100.0);
	m_WingGraph3.SetYMajGrid(true, RGB(120,120,120),2,1);
	m_WingGraph3.SetYMinGrid(false, true, RGB(80,80,80),2, 1, 0.1);
	m_WingGraph3.SetType(1);

	m_WingGraph4.SetAutoX(false);
	m_WingGraph4.SetXUnit(50.0);
	m_WingGraph4.SetXMin(-10.0);
	m_WingGraph4.SetXMax( 10.0);
	m_WingGraph4.SetYMin(0.000);
	m_WingGraph4.SetYMax(0.001);
	m_WingGraph4.SetXMajGrid(true, RGB(120,120,120),2,1);
	m_WingGraph4.SetXMinGrid(false, true, RGB(80,80,80),2, 1, 100.0);
	m_WingGraph4.SetYMajGrid(true, RGB(120,120,120),2,1);
	m_WingGraph4.SetYMinGrid(false, true, RGB(80,80,80),2, 1, 0.1);
	m_WingGraph4.SetType(1);

	m_WPlrGraph1.SetXMajGrid(true, RGB(120,120,120),2,1);
	m_WPlrGraph1.SetYMajGrid(true, RGB(120,120,120),2,1);
	m_WPlrGraph1.SetXTitle("Cd");
	m_WPlrGraph1.SetYTitle("Cl");
	m_WPlrGraph1.SetXMin(-0.0);
	m_WPlrGraph1.SetXMax( 0.1);
	m_WPlrGraph1.SetYMin(-0.01);
	m_WPlrGraph1.SetYMax( 0.01);
	m_WPlrGraph1.SetType(0);

	m_WPlrGraph2.SetXMajGrid(true, RGB(120,120,120),2,1);
	m_WPlrGraph2.SetYMajGrid(true, RGB(120,120,120),2,1);
	m_WPlrGraph2.SetXTitle("Alpha");
	m_WPlrGraph2.SetYTitle("Cl");
	m_WPlrGraph2.SetXMin(-0.0);
	m_WPlrGraph2.SetXMax( 0.1);
	m_WPlrGraph2.SetYMin(-0.01);
	m_WPlrGraph2.SetYMax( 0.01);
	m_WPlrGraph2.SetType(0);

	m_WPlrGraph3.SetXMajGrid(true, RGB(120,120,120),2,1);
	m_WPlrGraph3.SetYMajGrid(true, RGB(120,120,120),2,1);
	m_WPlrGraph3.SetXTitle("Alpha");
	m_WPlrGraph3.SetYTitle("Cl");
	m_WPlrGraph3.SetXMin(-0.0);
	m_WPlrGraph3.SetXMax( 0.1);
	m_WPlrGraph3.SetYMin(-0.01);
	m_WPlrGraph3.SetYMax( 0.01);
	m_WPlrGraph3.SetType(0);

	m_WPlrGraph4.SetXMajGrid(true, RGB(120,120,120),2,1);
	m_WPlrGraph4.SetYMajGrid(true, RGB(120,120,120),2,1);
	m_WPlrGraph4.SetXTitle("Alpha");
	m_WPlrGraph4.SetYTitle("Cl");
	m_WPlrGraph4.SetXMin(-0.0);
	m_WPlrGraph4.SetXMax( 0.1);
	m_WPlrGraph4.SetYMin(-0.01);
	m_WPlrGraph4.SetYMax( 0.01);
	m_WPlrGraph4.SetType(0);

	m_CpGraph.SetInverted(true);
	m_CpGraph.SetXMajGrid(true, RGB(120,120,120),2,1);
	m_CpGraph.SetYMajGrid(true, RGB(120,120,120),2,1);
	m_CpGraph.SetXTitle("x");
	m_CpGraph.SetYTitle("Cp");
	m_CpGraph.SetXMin( 0.0);
	m_CpGraph.SetXMax( 0.1);
	m_CpGraph.SetYMin(-0.01);
	m_CpGraph.SetYMax( 0.01);
	m_CpGraph.SetType(0);
	for(int i=0; i<4;i++) m_CpGraph.AddCurve(); */// four curves for wing, wing2, stab and fin

/*	m_BodyGridDlg.m_bGrid = false;
	m_BodyGridDlg.m_Color = RGB(200,200,200);
	m_BodyGridDlg.m_Style = PS_DASHDOT;
	m_BodyGridDlg.m_Width = 1;
	m_BodyGridDlg.m_Unit  = 0.1;

	m_BodyGridDlg.m_bMinGrid   = false;
	m_BodyGridDlg.m_MinColor   = RGB(70,70,70);
	m_BodyGridDlg.m_MinStyle   = PS_DASH;
	m_BodyGridDlg.m_MinWidth   = 1;
	m_BodyGridDlg.m_MinorUnit  = 0.01;*/

//	m_BodyGridDlg.m_bScale       = false;


	m_WOppVar1 = 0;
	m_WOppVar2 = 1;
	m_WOppVar3 = 2;
	m_WOppVar4 = 3;

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

/*	m_VLMDlg.m_pNode         = m_Node;
	m_VLMDlg.m_pPanel        = m_Panel;
	m_VLMDlg.m_ppPanel       = m_pPanel;
	m_VLMDlg.m_pWakeNode     = m_WakeNode;
	m_VLMDlg.m_pWakePanel    = m_WakePanel;
	m_VLMDlg.m_pMemNode      = m_MemNode;
	m_VLMDlg.m_pMemPanel     = m_MemPanel;
	m_VLMDlg.m_pTempWakeNode = m_TempWakeNode;
	m_VLMDlg.m_pRefWakeNode  = m_RefWakeNode;
	m_VLMDlg.m_pRefWakePanel = m_RefWakePanel;
	m_VLMDlg.m_RHS           = m_RHS;
	m_VLMDlg.m_Gamma         = m_RHSRef;
	m_VLMDlg.m_aij           = m_aij;
	m_VLMDlg.m_pCoreSize     = &m_CoreSize;

	m_PanelDlg.m_pPanel        = m_Panel;
	m_PanelDlg.m_ppPanel       = m_pPanel;
	m_PanelDlg.m_pNode         = m_Node;
	m_PanelDlg.m_pWakePanel    = m_WakePanel;
	m_PanelDlg.m_pWakeNode     = m_WakeNode;
	m_PanelDlg.m_pMemNode      = m_MemNode;
	m_PanelDlg.m_pMemPanel     = m_MemPanel;
	m_PanelDlg.m_pTempWakeNode = m_TempWakeNode;
	m_PanelDlg.m_pRefWakeNode  = m_RefWakeNode;
	m_PanelDlg.m_pRefWakePanel = m_RefWakePanel;
	m_PanelDlg.m_aij           = m_aij;
	m_PanelDlg.m_aijRef        = m_aijRef;
	m_PanelDlg.m_RHS           = m_RHS;
	m_PanelDlg.m_RHSRef        = m_RHSRef;
	m_PanelDlg.m_pCoreSize     = &m_CoreSize;

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
	memset(MatIn, 0, 16*sizeof(double));
	memset(MatOut, 0, 16*sizeof(double));

	m_WingGraph1.SetGraphName("Wing Graph 1");
	m_WingGraph2.SetGraphName("Wing Graph 2");
	m_WingGraph3.SetGraphName("Wing Graph 3");
	m_WingGraph4.SetGraphName("Wing Graph 4");

	m_WPlrGraph1.SetGraphName("Wing Polar Graph 1");
	m_WPlrGraph2.SetGraphName("Wing Polar Graph 2");
	m_WPlrGraph3.SetGraphName("Wing Polar Graph 3");
	m_WPlrGraph4.SetGraphName("Wing Polar Graph 4");

	m_CpGraph.SetGraphName("Cp Graph");
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

//		CreateWOpp(&pPOpp->m_WingWOpp, m_pCurWing);
//		if(m_pCurWing2) CreateWOpp(&pPOpp->m_Wing2WOpp, m_pCurWing2);
//		if(m_pCurStab)  CreateWOpp(&pPOpp->m_StabWOpp, m_pCurStab);
//		if(m_pCurFin)   CreateWOpp(&pPOpp->m_FinWOpp,  m_pCurFin);

		if(m_pCurWing2)  pPOpp->m_bBiplane = true; else pPOpp->m_bBiplane = false;
		if(m_pCurStab)   pPOpp->m_bStab = true; else pPOpp->m_bStab = false;
		if(m_pCurFin )   pPOpp->m_bFin  = true; else pPOpp->m_bFin = false;

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
			//TODO
/*			pPOpp->m_NPanels             = m_VLMDlg.m_MatSize;
			pPOpp->m_Alpha               = m_VLMDlg.m_OpAlpha;
			pPOpp->m_QInf                = m_VLMDlg.m_QInf;
			if(m_pCurWPolar->m_Type>=5)
			{
				pPOpp->m_Ctrl = m_VLMDlg.m_Ctrl;
				pWOpp->m_Ctrl = m_VLMDlg.m_Ctrl;
			}
			else
			{
				pPOpp->m_Ctrl = 0.0;
				pWOpp->m_Ctrl = 0.0;
			}

			pWOpp->m_Alpha               = m_VLMDlg.m_OpAlpha;
			pWOpp->m_QInf                = m_VLMDlg.m_QInf;
			pWOpp->m_CL                  = m_VLMDlg.m_CL;
			pWOpp->m_CX                  = m_VLMDlg.m_CX;
			pWOpp->m_CY                  = m_VLMDlg.m_CY;
			pWOpp->m_InducedDrag         = m_VLMDlg.m_InducedDrag;
			pWOpp->m_ViscousDrag         = m_VLMDlg.m_ViscousDrag;

			pWOpp->m_GCm                 = m_VLMDlg.m_GCm;
			pWOpp->m_GRm                 = m_VLMDlg.m_GRm;
			pWOpp->m_GYm                 = m_VLMDlg.m_GYm;
			pWOpp->m_VYm                 = m_VLMDlg.m_VYm;
			pWOpp->m_IYm                 = m_VLMDlg.m_IYm;

			pWOpp->m_XCP                 = m_VLMDlg.m_XCP;
			pWOpp->m_YCP                 = m_VLMDlg.m_YCP;*/

			pWOpp->m_Beta	             = m_pCurWPolar->m_Beta;
		}
		else if(m_pCurWPolar->m_AnalysisType==3)
		{
/*			pPOpp->m_NPanels             = m_PanelDlg.m_MatSize;
			pPOpp->m_Alpha               = m_PanelDlg.m_OpAlpha;
			pPOpp->m_QInf                = m_PanelDlg.m_QInf;

			pWOpp->m_Alpha               = m_PanelDlg.m_OpAlpha;
			pWOpp->m_QInf                = m_PanelDlg.m_QInf;
			pWOpp->m_CL                  = m_PanelDlg.m_CL;
			pWOpp->m_CX                  = m_PanelDlg.m_CX;
			pWOpp->m_CY                  = m_PanelDlg.m_CY;
			pWOpp->m_InducedDrag         = m_PanelDlg.m_InducedDrag;
			pWOpp->m_ViscousDrag         = m_PanelDlg.m_ViscousDrag;

			pWOpp->m_GCm                 = m_PanelDlg.m_GCm;
			pWOpp->m_GRm                 = m_PanelDlg.m_GRm;
			pWOpp->m_GYm                 = m_PanelDlg.m_GYm;
			pWOpp->m_VYm                 = m_PanelDlg.m_VYm;
			pWOpp->m_IYm                 = m_PanelDlg.m_IYm;

			pWOpp->m_XCP                 = m_PanelDlg.m_XCP;
			pWOpp->m_YCP                 = m_PanelDlg.m_YCP;*/

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
		for(i=0; i<m_pCurWing->m_MatSize; i++){
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
//			CreateWPolarCurves();
//			UpdateView();
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
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

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
//qDebug() << "Appending One Wing " << pWing->m_WingName << "\n";
			}
			pMainFrame->SetSaveState(false);
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
		if (pOldWPlr->m_PlrName == pWPolar->m_PlrName &&
			pOldWPlr->m_UFOName == pWPolar->m_UFOName)
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








