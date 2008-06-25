/****************************************************************************

    Miarex Class
	Copyright (C) 2006-2008 André Deperrois xflr5@yahoo.com

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

// MiarexDlg.cpp : implementation file
// The main program which will process messages related to Wing Analysis
// Despite the class's name, all the algorithms have been re-written 
// independently of Matthieu's program
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../misc/NameDlg.h"
#include "../misc/PolarFilter.h"
#include "../misc/ModDlg.h"
#include "../misc/PrintOptionsDlg.h"
#include "../misc/EditPlrDlg.h"
#include "../misc/UnitsDlg.h"
#include "../misc/MessageDlg.h"
#include "../Graph/GraphDlg.h"
#include "../Graph/UserVarDlg.h"
#include "../Graph/WGraphVarDlg.h"
#include "../main/ChildView.h"
#include "../main/MainFrm.h"
#include "ListPlrDlg.h"
#include "CpScaleDlg.h"
#include "Quaternion.h"
#include "WAdvDlg.h"
#include "WingDlg.h"
#include "PlaneDlg.h"
#include "BodyNURBSDlg.h"
#include "BodyScaleDlg.h"
#include "WingScaleDlg.h"
#include "LLTThread.h"
#include "W3DBar.h"
#include <math.h>
#include <gl\glaux.h>
#include ".\miarex.h"


//Define the references for the OpenGL lists

#define PANELCP				1227

#define VLMWINGLIFT			1230
#define LIFTFORCE			1231
#define VLMMOMENTS			1232

#define VLMWING2LIFT		1233
#define VLMSTABLIFT			1234
#define VLMFINLIFT			1235

#define VLMWINGDRAG			1236
#define VLMWING2DRAG		1237
#define VLMSTABDRAG			1238
#define VLMFINDRAG			1239

#define VLMWINGWASH			1241
#define VLMWING2WASH		1242
#define VLMSTABWASH			1243
#define VLMFINWASH			1244

#define VLMCTRLPTS			1250
#define VLMVORTICES			1251

#define VLMSTREAMLINES		1255
#define SURFACESPEEDS		1256

#define WINGSURFACES		1258
#define WINGOUTLINE			1259

#define WING2SURFACES		1260
#define WING2OUTLINE		1261

#define STABSURFACES		1265
#define STABOUTLINE			1266

#define FINSURFACES 		1270
#define FINOUTLINE			1271

#define MESHPANELS			1272
#define MESHBACK			1273


#define WINGLEGEND			1276
#define WOPPLEGEND			1277
#define WOPPCPLEGEND		1278

#define WINGWAKEPANELS		1283

#define VLMWINGTOPTRANS     1293
#define VLMWINGBOTTRANS     1294
#define VLMWING2TOPTRANS    1295
#define VLMWING2BOTTRANS    1296
#define VLMSTABTOPTRANS     1297
#define VLMSTABBOTTRANS     1298
#define VLMFINTOPTRANS      1299
#define VLMFINBOTTRANS      1300

//3D
#define BODYGEOM            1302
#define BODYSURFACES        1303
//2D
#define BODYAXIALLINES      1304
#define BODYFRAME		    1305
#define BODYFRAME3D         1306
#define BODYFRAMEGRID		1307
#define BODYLINEGRID		1308
#define BODYPOINTS			1309
#define FRAMEPOINTS			1310

#define BODYMESHPANELS		1311
#define BODYMESHBACK		1312

#define ARCBALL             1313
#define ARCPOINT            1314

/////////////////////////////////////////////////////////////////////////////
// CMiarex dialog

CMiarex::CMiarex(CWnd* pParent /*=NULL*/)
{
	//construct evrything
	pi = 3.141592654;

	m_hcArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_hcCross = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
	m_hcMove  = AfxGetApp()->LoadCursor(IDC_HMOVE);

	m_pFrame       = NULL;
	m_pChildWnd    = NULL;
	m_pXFile       = NULL;
	m_pBodyCtrlBar = NULL;

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
	m_CvPrec    =   0.01;
	m_Relax     =  20.0;

	m_Panel[0].m_VortexPos = 0.25;
	m_Panel[0].m_CtrlPos   = 0.75;

	m_WngAnalysis.m_WakePanelFactor = 1.10;
	m_WngAnalysis.m_TotalWakeLength  = 1.00;
	m_WngAnalysis.m_NXWakePanels    = 1;

	m_CoreSize        = 0.0;
	m_MaxWakeIter     = 5;
	m_WakeInterNodes  = 6;
	m_bResetWake      = true;
	m_bSetNewWake     = true;
	m_bVLMFinished    = true;

	m_nWakeNodes = 0;
	m_WakeSize   = 0;
	m_LastPoint.SetPoint(0,0);
	m_WPlrLegendOffset.SetPoint(0,0);
	m_WingLegendOffset.SetPoint(0,0);

	m_BodyOffset.Set( 0.20, -0.12, 0.0);
	m_FrameOffset.Set(0.80, -0.50, 0.0);
	m_HorizontalSplit = -0.45;
	m_VerticalSplit   =  0.35;

	m_rCltRect.SetRectEmpty();
	m_rDrawRect.SetRectEmpty();
	m_rSingleRect.SetRectEmpty();

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

	m_ptOffset.x = 0;
	m_ptOffset.y = 0;

	CPanel::s_pNode = m_Node;
	CWing::s_pVLMDlg     = &m_VLMDlg;    //pointer to the VLM analysis dialog class
	CWing::s_p3DPanelDlg = &m_PanelDlg;  //pointer to the 3DPanel analysis dialog class

	CWing::m_pWakePanel = m_WakePanel;
	CWing::m_pWakeNode = m_WakeNode;

	m_ArcBall.m_pGLScale = &m_GLScale;
	m_ArcBall.m_pOffx    = &m_UFOOffset.x;
	m_ArcBall.m_pOffy    = &m_UFOOffset.y;
	m_ArcBall.m_pTransx  = &m_glViewportTrans.x;
	m_ArcBall.m_pTransy  = &m_glViewportTrans.y;

	m_GLList = 0;
	m_bResetglBody       = false;//otherwise endless repaint if no body present
	m_bResetglBody2D     = false;//
	m_bResetglBodyPoints = false;
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
	m_b3DVLMCl           = false;
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
	m_bIsPrinting        = false;
	m_bCurWOppOnly       = true;
	m_bStoreWOpp         = true;
	m_bKeepOutOpps       = false;
	m_bCurFrameOnly      = true;
	m_bTrans             = false;
	m_bDragPoint         = false;
	m_bType1             = true;
	m_bType2             = true;
	m_bType4             = true;
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

	m_PanelDlg.m_ppBody = &m_pCurBody;

	m_pCurWingGraph = &m_WingGraph1;
	m_pCurWPlrGraph = &m_WPlrGraph1;

	m_WingGraph1.SetAutoX(false);
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

	m_BodyGridDlg.m_bGrid = false;	
	m_BodyGridDlg.m_Color = RGB(200,200,200);
	m_BodyGridDlg.m_Style = PS_DASHDOT;
	m_BodyGridDlg.m_Width = 1;
	m_BodyGridDlg.m_Unit  = 0.1;

	m_BodyGridDlg.m_bMinGrid   = false;
	m_BodyGridDlg.m_MinColor   = RGB(70,70,70);
	m_BodyGridDlg.m_MinStyle   = PS_DASH;
	m_BodyGridDlg.m_MinWidth   = 1;
	m_BodyGridDlg.m_MinorUnit  = 0.01;

	m_BodyGridDlg.m_bScale       = false;


	m_WOppVar1 = 0;
	m_WOppVar2 = 1;
	m_WOppVar3 = 2;
	m_WOppVar4 = 3;

	m_NCpCurves = 0;
	m_CpColor = RGB(255,100,150);
	m_CpStyle = PS_SOLID;
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

	m_WingColor =  RGB(0,130,130);
	m_StabColor = 12477630;
	m_FinColor  =  RGB(60,60,180);

	m_VLMDlg.m_pNode         = m_Node;
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

	m_GLLightDlg.m_pMiarex = this;
	
	m_pW3DBar    = NULL;
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
}

CMiarex::~CMiarex()
{
	//destroy evrything
	if(glIsList(PANELCP)) {
		glDeleteLists(PANELCP,1);
		m_GLList-=1;
	}	
	if(glIsList(VLMWINGLIFT)) {
		glDeleteLists(VLMWINGLIFT,3);
		m_GLList-=3;
	}	
	if(glIsList(VLMWING2LIFT)) {
		glDeleteLists(VLMWING2LIFT,1);
		m_GLList-=1;
	}	
	if(glIsList(VLMSTABLIFT)) {
		glDeleteLists(VLMSTABLIFT,1);
		m_GLList-=1;
	}	
	if(glIsList(VLMFINLIFT)) {
		glDeleteLists(VLMFINLIFT,1);
		m_GLList-=1;
	}
	if(glIsList(VLMWINGDRAG)) {
		glDeleteLists(VLMWINGDRAG,1);
		m_GLList -=1;
	}
	if(glIsList(VLMWING2DRAG)) {
		glDeleteLists(VLMWING2DRAG,1);
		m_GLList -=1;
	}
	if(glIsList(VLMSTABDRAG)) {
		glDeleteLists(VLMSTABDRAG,1);
		m_GLList -=1;
	}
	if(glIsList(VLMFINDRAG)) {
		glDeleteLists(VLMFINDRAG,1);
		m_GLList -=1;
	}

	if(glIsList(VLMWINGTOPTRANS)) {
		glDeleteLists(VLMWINGTOPTRANS,2);
		m_GLList-=2;
	}	
	if(glIsList(VLMWING2TOPTRANS)) {
		glDeleteLists(VLMWING2TOPTRANS,2);
		m_GLList-=2;
	}	
	if(glIsList(VLMSTABTOPTRANS)) {
		glDeleteLists(VLMSTABTOPTRANS,2);
		m_GLList-=2;
	}	
	if(glIsList(VLMFINTOPTRANS)) {
		glDeleteLists(VLMFINTOPTRANS,2);
		m_GLList-=2;
	}	
	if(glIsList(VLMWINGWASH)) {
		glDeleteLists(VLMWINGWASH,1);
		m_GLList--;
	}
	if(glIsList(VLMWING2WASH)) {
		glDeleteLists(VLMWING2WASH,1);
		m_GLList--;
	}
	if(glIsList(VLMSTABWASH)) {
		glDeleteLists(VLMSTABWASH,1);
		m_GLList--;
	}
	if(glIsList(VLMFINWASH)) {
		glDeleteLists(VLMFINWASH,1);
		m_GLList--;
	}
	if(glIsList(VLMCTRLPTS)) {
		glDeleteLists(VLMCTRLPTS,1);
		m_GLList-=1;
	}	
	if(glIsList(VLMVORTICES)) {
		glDeleteLists(VLMVORTICES,1);
		m_GLList-=1;
	}	
	if(glIsList(WINGSURFACES)) {
		glDeleteLists(WINGSURFACES,2);
		m_GLList-=2;
	}
	if(glIsList(WING2SURFACES)) {
		glDeleteLists(WING2SURFACES,2);
		m_GLList-=2;
	}
	if(glIsList(STABSURFACES)) {
		glDeleteLists(STABSURFACES,2);
		m_GLList-=2;
	}
	if(glIsList(FINSURFACES)) {
		glDeleteLists(FINSURFACES,2);
		m_GLList-=2;
	}
	if(glIsList(MESHPANELS)) {
		glDeleteLists(MESHPANELS,2);
		m_GLList-=2;
	}
	if(glIsList(WINGWAKEPANELS)) {
		glDeleteLists(WINGWAKEPANELS,1);
		m_GLList-=1;
	}
	if(glIsList(VLMSTREAMLINES)) {
		glDeleteLists(VLMSTREAMLINES,1);
		m_GLList-=1;
	}
	if(glIsList(SURFACESPEEDS)) {
		glDeleteLists(SURFACESPEEDS,1);
		m_GLList-=1;
	}
	if(glIsList(WINGLEGEND)) {
		glDeleteLists(WINGLEGEND,3);
		m_GLList-=3;
	}
	if(glIsList(BODYGEOM)) {
		glDeleteLists(BODYGEOM,9);
		m_GLList-=9;
	}
	if(glIsList(BODYMESHPANELS)) 
	{
		glDeleteLists(BODYMESHPANELS,2);
		m_GLList -=2;
	}
	if(glIsList(ARCBALL)) {
		glDeleteLists(ARCBALL,2);
		m_GLList-=2;
	}
}



BEGIN_MESSAGE_MAP(CMiarex, CWnd)

	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(IDM_RESET3DVIEW, OnReset3DView)
	ON_COMMAND(IDM_SINGLEGRAPH1, OnSingleGraph1)
	ON_COMMAND(IDM_SINGLEGRAPH2, OnSingleGraph2)
	ON_COMMAND(IDM_SINGLEGRAPH3, OnSingleGraph3)
	ON_COMMAND(IDM_SINGLEGRAPH4, OnSingleGraph4)
	ON_COMMAND(IDM_DEFINEPLANE, OnDefinePlane)
	ON_COMMAND(IDM_SAVEWING, OnSaveUFO)
	ON_COMMAND(IDM_SHOWWING2, OnShowWing2)
	ON_COMMAND(IDM_SHOWSTAB, OnShowElevator)
	ON_COMMAND(IDM_SHOWFIN, OnShowFin)
	ON_COMMAND(IDM_AUTOWINGSCALES, OnAutoWingScales)
	ON_COMMAND(IDM_DELETEALL, OnDeleteAll)
	ON_COMMAND(IDM_WINGGRAPH4, OnWingGraph4)
	ON_COMMAND(IDM_WINGGRAPH3, OnWingGraph3)
	ON_COMMAND(IDM_WINGGRAPH2, OnWingGraph2)
	ON_COMMAND(IDM_WINGGRAPH1, OnWingGraph1)
	ON_COMMAND(IDM_CPLEGEND, OnCpLegend)
	ON_COMMAND(IDM_WANALYSIS, OnDefineAnalysis)
	ON_COMMAND(IDM_SHOWPOLAR, OnWPolar)
	ON_COMMAND(IDM_EXPORTWING, OnExportWing)
	ON_COMMAND(IDM_LOADWPLRS, OnLoadProject)
	ON_COMMAND(IDM_SAVEWPLRS, OnSaveProject)
	ON_COMMAND(IDM_DELETEWPOLAR, OnDeleteWPolar)
	ON_COMMAND(IDM_DELETEWPOLARS, OnDeleteWPolars)
	ON_COMMAND(IDM_GRAPHDLG, OnGraphDlg)
	ON_COMMAND(IDM_RESETDEF, OnResetDef)
	ON_COMMAND(IDM_RESETSCALES, OnResetScales)
	ON_COMMAND(IDM_WPOLAREXPORT, OnWPolarExport)
	ON_COMMAND(IDM_LISTPLRS, OnListPlrs)
	ON_COMMAND(IDM_DEFINEVAR, OnDefineVar)
	ON_COMMAND(IDM_TWOGRAPHS, OnTwoGraphs)
	ON_COMMAND(IDM_PRINT, OnPrint)
	ON_COMMAND(IDM_DEFINEWING, OnDefineWing)
	ON_COMMAND(IDM_DELCURWOPP, OnDelCurWOpp)
	ON_COMMAND(IDM_WOPP, OnWOpp)
	ON_COMMAND(IDM_EDWING, OnEditWing)
	ON_COMMAND(IDM_DUPLICATEWING, OnDuplicateWing)
	ON_COMMAND(IDM_RENAMEWING, OnRenameUFO)
	ON_COMMAND(IDM_DELETEWING, OnDeleteWing)
	ON_COMMAND(IDM_POLARFILTER, OnPolarFilter)
	ON_COMMAND(IDM_RENAMEWPOLAR, OnRenameWPolar)
	ON_COMMAND(IDM_FOURGRAPHS, OnFourGraphs)
	ON_COMMAND(IDM_ABOUTBOX, OnAbout)
	ON_COMMAND(IDM_EXPORTWOPP, OnExportWOpp)
	ON_COMMAND(IDM_ADJUSTTOWING, OnAdjustToWing)
	ON_COMMAND(IDM_WPOLARRESET, OnWPolarReset)
	ON_COMMAND(IDM_MIAREXSAVEAS, OnSaveAs)
	ON_COMMAND(IDM_NEWPROJECT, OnNewProject)
	ON_COMMAND(IDM_EDITWPLR, OnEditWPlr)
	ON_COMMAND(IDM_INSERTPROJECT, OnInsertProject)
	ON_COMMAND(IDM_WGRAPHVAR, OnWGraphVar)
	ON_COMMAND(IDM_FOURWINGGRAPHS, OnFourWingGraphs)
	ON_COMMAND(IDM_TWOWINGGRAPHS, OnTwoWingGraphs)
	ON_COMMAND(IDM_SHOWCURWOPP, OnShowCurWOpp)
	ON_COMMAND(IDM_SHOWALLWOPPS, OnShowAllWOpps)
	ON_COMMAND(IDM_DELALLPLRWOPPS, OnDelAllPlrWOpps)
	ON_COMMAND(IDM_WADVSETTINGS, OnWAdvSettings)
	ON_COMMAND(IDM_SHOWXCMREF, OnShowXCmRef)
	ON_COMMAND(IDM_HIDEALLWPLRS, OnHideAllWPlrs)
	ON_COMMAND(IDM_SHOWALLWPLRS, OnShowAllWPlrs)
	ON_COMMAND(IDM_WNGHIDEALL, OnWngHideAll)
	ON_COMMAND(IDM_WNGSHOWALL, OnWngShowAll)
	ON_COMMAND(IDM_IMPORTWING, OnImportWing)
	ON_COMMAND(IDM_SHOWELLIPTIC, OnShowElliptic)
	ON_COMMAND(IDM_RESETWINGSCALE, OnResetWingScale)
	ON_COMMAND(IDM_3DVIEW, On3DView)
	ON_COMMAND(IDM_BODYDESIGN, OnBodyDesign)
	ON_COMMAND(IDM_SCALEWING, OnScaleWing)
	ON_COMMAND(IDM_RESETWINGLEGEND, OnResetWingLegend)
	ON_COMMAND(IDM_RESETLEGEND, OnResetLegend)
	ON_COMMAND(IDM_RESETALLSCALES, OnResetAllScales)
	ON_COMMAND(IDM_HIDEALLWOPPS, OnHideAllWOpps)
	ON_COMMAND(IDM_HIDEWOPPS, OnHideWOpps)
	ON_COMMAND(IDM_SHOWWOPPS, OnShowWOpps)
	ON_COMMAND(IDM_DELALLWOPPS, OnDelAllWOpps)
	ON_COMMAND(IDM_HIDEWINGOPPS, OnHideWingOpps)
	ON_COMMAND(IDM_SAVEPROJECTAS, OnSaveAs)
	ON_COMMAND(IDM_LOADPROJECT, OnLoadProject)
	ON_COMMAND(IDM_SHOWWINGOPPS, OnShowWingOpps)
	ON_COMMAND(IDM_GLLIGHT, OnGLLight)
	ON_COMMAND(IDM_STREAMOPTIONS, OnStreamOptions)
	ON_COMMAND(IDM_MANAGEWINGS, OnManageUFO)
	ON_COMMAND(IDM_EXPORTVLMPANELS, OnExportPanels)
	ON_COMMAND(IDM_CPVIEW, OnCpView)
	ON_COMMAND(IDM_INSERTFRAME, OnInsertBodyPoint)
	ON_COMMAND(IDM_DELETEFRAME, OnDeleteBodyPoint)
	ON_COMMAND(IDM_INSERTPOINT, OnInsertBodyPoint)
	ON_COMMAND(IDM_DELETEPOINT, OnDeleteBodyPoint)
	ON_COMMAND(IDM_NEWBODY, OnNewBody)
	ON_COMMAND(IDM_RENAMECURBODY, OnRenameCurBody)
	ON_COMMAND(IDM_DELETECURBODY, OnDeleteCurBody)
	ON_COMMAND(IDM_SCALECURBODY, OnScaleCurBody)
	ON_COMMAND(IDM_DUPLICATECURBODY, OnDuplicateCurBody)
	ON_COMMAND(IDM_EXPORTCURBODY, OnExportCurBody)
	ON_COMMAND(IDM_SAVECURBODYASPROJECT, OnSaveCurBodyAsProject)
	ON_COMMAND(IDM_RESETBODYSCALE, OnResetBodyScale)
	ON_COMMAND(IDM_RESETFRAMESCALE, OnResetBodyScale)
	ON_COMMAND(IDM_BODYGRIDOPTIONS, OnBodyGrid)
	ON_COMMAND(IDT_WOPP, OnWOpp)
	ON_COMMAND(IDT_WPOLARS, OnWPolar)
	ON_COMMAND(IDT_UNDO, OnUndo	)
	ON_COMMAND(IDT_REDO, OnRedo)
	ON_COMMAND(IDM_BODYRESOLUTION, OnBodyResolution)
	ON_COMMAND(IDM_INTERPOLATE, OnInterpolateBodyPoints)
	ON_COMMAND(IDM_SCALEFRAME, OnScaleFrame)
	ON_COMMAND(IDM_SHOWONLYACTIVEFRAME, OnShowOnlyActiveFrame)
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMiarex message handlers



void CMiarex::OnExit() 
{
	PostMessage(WM_QUIT);	
}


BOOL CMiarex::OnEraseBkgnd(CDC* pDC) 
{
	//the code will take care of the background
	return TRUE;	
}



CWing* CMiarex::AddWing(CWing *pWing)
{
	//adds the pWing to the m_oaWing array
	//places it in alphabetical order 
	int i,j;
	bool bExists   = false;
	bool bInserted = false;
	CWing *pOldWing;
	CString strong;
	
	if(pWing->m_WingName.GetLength()){
		for (i=0; i<m_poaWing->GetSize(); i++){
			pOldWing = (CWing*)m_poaWing->GetAt(i);
			if (pOldWing->m_WingName == pWing->m_WingName){
				bExists = true;
				break;
			}
		}
	}
	else bExists = true;

	while(!bInserted){
		if(!bExists){ 
			for (j=0; j<m_poaWing->GetSize(); j++){
				pOldWing = (CWing*)m_poaWing->GetAt(j);
				if (pWing->m_WingName.CompareNoCase(pOldWing->m_WingName)<0) {
					m_poaWing->InsertAt(j, pWing);
					bInserted = true;
					break;
				}
			}	
			if(!bInserted)	{
				m_poaWing->Add(pWing);
				bInserted = true;
			}
			CMainFrame *pFrame = (CMainFrame*)m_pFrame;
			pFrame->SetSaveState(false);
			return pWing;
		}
		else {
			//Ask for user intentions
			if(SetModWing(pWing)) {
				m_poaWing->Add(pWing);
				return pWing;
			}
			else{
				delete pWing;
				return NULL;
			}

			bExists = false;
		}
	}
	return NULL;
}



void CMiarex::PaintWSingleGraph(CDC *pDC, CRect *pCltRect, CRect *pDrawRect)
{
	// Paint a single WPolar graph to the device context pDC
	// pCltRect is the Client Rectangle
	// Do not paint if the Drawing rectangle pDrawRect is too small
	if(!m_pCurGraph) return;

	int h  = pCltRect->Height();
	int w  = pCltRect->Width();
	int w2 = (int)(w/2);

	int w3 = (int)(0.35*w);
	int w23 = 2*w3;

	CRect Rect1(0,0,w23,pCltRect->bottom-00);
	if(w2>200 && h>250){
		DrawWPolarLegend(pDC, false,m_WPlrLegendOffset,Rect1.bottom);
		if(pDrawRect->Width()<200 || pDrawRect->Height()<200) return;
		m_pCurGraph->DrawGraph(pDC, &Rect1, false);
	}
}

void CMiarex::PaintWCoupleGraphs(CDC *pDC, CRect *pCltRect, CRect *pDrawRect)
{
	// Paint  two  WPolar graph to the device context pDC
	// pCltRect is the Client Rectangle
	// Do not paint if the Drawing rectangle pDrawRect is too small
	int h  = pCltRect->Height();
	int w  = pCltRect->Width();
	int w2 = (int)(w/2);
	int h23 = (int)(2*h/3);

	if(w2>200 && h>250){
	
		DrawWPolarLegend(pDC, false,m_WPlrLegendOffset,pCltRect->bottom);
		CRect Rect1(0,0,w2,h23);
		CRect Rect2(w2,0,w,h23);
		if(pDrawRect->Width()<200 || pDrawRect->Height()<200) return;

		m_WPlrGraph1.DrawGraph(pDC, &Rect1, false);
		m_WPlrGraph2.DrawGraph(pDC, &Rect2, false);
	}
}

void CMiarex::PaintWFourGraphs(CDC *pDC, CRect *pCltRect, CRect *pDrawRect)
{
	// Paint four  WPolar graph to the device context pDC
	// pCltRect is the Client Rectangle
	// Do not paint if the Drawing rectangle pDrawRect is too small

	int h  = pCltRect->Height();
	int w  = pCltRect->Width();
	int h2 = (int)(h/2);
	int w3 = (int)(0.35*w);
	int w23 = 2*w3;

	if(w3>150 && h>250)
	{
		DrawWPolarLegend(pDC, false,m_WPlrLegendOffset,pCltRect->bottom);
		CRect Rect1(0,0,w3,h2);
		CRect Rect2(w3,0,w23,h2);
		CRect Rect3(0,h2,w3,h);
		CRect Rect4(w3,h2,w23,h);
		if(pDrawRect->Width()<200 || pDrawRect->Height()<200) return;

		m_WPlrGraph1.DrawGraph(pDC, &Rect1, false);
		m_WPlrGraph2.DrawGraph(pDC, &Rect2, false);
		m_WPlrGraph3.DrawGraph(pDC, &Rect3, false);
		m_WPlrGraph4.DrawGraph(pDC, &Rect4, false);
	}
}


CWPolar* CMiarex::AddWPolar(CWPolar *pWPolar)
{
	//Add a WPolar to the m_oaWPolar array
	//Insert the WPolar in alphabetical order
	int i,j;
 	bool bExists   = false;
 	bool bInserted = false;
 	CWPolar *pOldWPlr;
 	CWPolar *pOld2WPlr;
 	CString strong;

 	for (i=0; i<m_poaWPolar->GetSize(); i++){
 		pOldWPlr = (CWPolar*)m_poaWPolar->GetAt(i);
 		if (pOldWPlr->m_PlrName == pWPolar->m_PlrName &&
			pOldWPlr->m_UFOName == pWPolar->m_UFOName) {
			bExists = true;
			break;
		}
 	}

 	while(!bInserted){
 		if(!bExists){ 
 			for (j=0; j<m_poaWPolar->GetSize(); j++){
 				pOldWPlr = (CWPolar*)m_poaWPolar->GetAt(j);
				if(pWPolar->m_UFOName.CompareNoCase(pOldWPlr->m_UFOName)<0){
 					m_poaWPolar->InsertAt(j, pWPolar);
 					bInserted = true;
 					break;
 				}
 				else if (pWPolar->m_UFOName == pOldWPlr->m_UFOName){
					//first sort by Type
					if(pWPolar->m_PlrName.CompareNoCase(pOldWPlr->m_PlrName)<0){
 						m_poaWPolar->InsertAt(j, pWPolar);
						bInserted = true;
 						break;
					}
 				}
 			}	
 			if(!bInserted)	{
				m_poaWPolar->Add(pWPolar);
				bInserted = true;
 			}
 			return pWPolar;
 		}
 		else {
 			for (int l=0; l<m_poaWPolar->GetSize(); l++){
 				pOldWPlr = (CWPolar*)m_poaWPolar->GetAt(l);
 				if (pOldWPlr->m_UFOName == pWPolar->m_UFOName &&
 					pOldWPlr->m_PlrName == pWPolar->m_PlrName){

 					int p = 2;
 					bool bFound = true;
 					while(bFound){
 						strong.Format(" (%d)", p);
 						strong = pWPolar->m_PlrName + strong;
 						for (int k=0; k<m_poaWPolar->GetSize(); k++){
 							bFound = false;
 							pOld2WPlr = (CWPolar*)m_poaWPolar->GetAt(k);
 							if (pOld2WPlr->m_UFOName == pWPolar->m_UFOName &&
 								pOld2WPlr->m_PlrName == strong){
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


CWPolar* CMiarex::GetWPolar(CString WPolarName)
{
	//return a pointer to the WPolar with name WPolarName
	//or return NULL if non with taht name for the current UFO
  	CWPolar *pWPolar;
	CString UFOName;
	int i;
	if(m_pCurPlane) UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return NULL;
  	for (i=0; i<m_poaWPolar->GetSize(); i++){
 		pWPolar = (CWPolar*) m_poaWPolar->GetAt(i);
 		if (pWPolar->m_UFOName == UFOName &&
			pWPolar->m_PlrName == WPolarName) return pWPolar;
	}
  	return NULL;
}


void CMiarex::SetWPlr(bool bCurrent, CString WPlrName)
{
	//if bCurrent, make active the current polar,
	// else make active the Wpolar with the name WPlrName
	CWaitCursor wait;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CWPolar *pWPolar = NULL;
	CWPolar *pOldWPolar = NULL;
	CString UFOName;
	CString strong;
	int i,j,k,m, NStation;
	double SpanPos;

	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return;

	if(bCurrent) pWPolar = m_pCurWPolar;
	else 
	{
  		for (i=0; i<m_poaWPolar->GetSize(); i++)
		{
 			pOldWPolar = (CWPolar*) m_poaWPolar->GetAt(i);
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
		m_bResetglOpp  = true;
		m_bResetglMesh = true;
		m_bResetglWake = true;
	
		if(!m_pCurWing) 
		{
			pWPolar = NULL;
			m_pCurWPolar = NULL;
			pFrame->m_WOperDlgBar.EnableAnalysis(false);
		}

		if(pWPolar) m_pCurWPolar = pWPolar;
		else if(m_pCurWing)
		{
			if (pFrame->m_ctrlWPlr.GetCount())
			{
				// try to set the first in the list, if any
				pFrame->m_ctrlWPlr.GetLBText(0, strong);
				m_pCurWPolar = GetWPolar(strong);
			}
			else {
				m_pCurWPolar = NULL;
				pFrame->m_WOperDlgBar.EnableAnalysis(false);
			}
		}
	}

	if(m_pCurWPolar)
	{
		int pos = pFrame->m_ctrlWPlr.FindStringExact(0, m_pCurWPolar->m_PlrName);
		if (pos!=CB_ERR)		pFrame->m_ctrlWPlr.SetCurSel(pos);
	}
	
	InitializePanels();

/*	CVector H(0.0, 1.0, 0.0);
	Quaternion Quat;
	Quat.Set(45.0, H);

	if(m_pCurPlane && m_pCurWPolar)
	{
		//wing tilt 
		for(j=0; j<m_pCurWing->m_MatSize; j++)
		{
			(m_pCurWing->m_pPanel+j)->Rotate(m_pCurPlane->m_LEWing, Quat);
		}
		//Stab tilt 
		for(j=0; j<m_pCurStab->m_MatSize; j++)
		{
			(m_pCurStab->m_pPanel+j)->Rotate(m_pCurPlane->m_LEStab, Quat);
		}
	}


	if(m_pCurWing && m_pCurWing->m_nFlaps>0)
	{
		RotateFlap(m_pCurWing, 0, -45.0, m_Panel, m_Node);
		RotateFlap(m_pCurWing, 1, -45.0, m_Panel, m_Node);
		if(m_pCurStab)
		{
			RotateFlap(m_pCurStab, 0, 45, m_Panel, m_Node);
			RotateFlap(m_pCurStab, 1, 45, m_Panel, m_Node);
		}
	}*/


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

			for (j=(int)m_pCurWing->m_NSurfaces/2; j<m_pCurWing->m_NSurfaces; j++)
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
			m_pCurWing->m_NStation  = m_NStation;
			m_pCurWing->m_bLLT      = true;
		}

		pFrame->m_WOperDlgBar.EnableAnalysis(true);
		pFrame->UpdateWOpps();

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
		pFrame->UpdateWOpps();
		pFrame->m_WOperDlgBar.EnableAnalysis(false);
	}

	if(m_pCurWing){
		m_pCurWing->m_CvPrec    = m_CvPrec;
		m_pCurWing->m_RelaxMax  = m_Relax;
	}

	pFrame->m_WOperDlgBar.SetParams(m_pCurWPolar);
	pFrame->m_PolarDlgBar.SetParams();
	CreateWPolarCurves();

	m_VLMDlg.m_pWPolar       = m_pCurWPolar;
	m_VLMDlg.m_MatSize       = m_MatSize;
	m_VLMDlg.m_nNodes        = m_nNodes;
	m_VLMDlg.m_NSurfaces     = m_NSurfaces;

	m_PanelDlg.m_pWPolar     = m_pCurWPolar;
	m_PanelDlg.m_MatSize     = m_MatSize;
	m_PanelDlg.m_nNodes      = m_nNodes;
	m_PanelDlg.m_NSurfaces   = m_NSurfaces;
	m_PanelDlg.m_NWakeColumn = m_NWakeColumn;
}


void CMiarex::AddWOpp(bool bPointOut, double *Gamma, double *Sigma, double *Cp)
{ 
	// creates a new WOpp, and if successful
	// fills the WOpp's variables with the current analysis results
	// adds the WOpp to the array
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	int i,j,l;

	if(!m_bKeepOutOpps && bPointOut) return;

	CWOpp *pWOpp;	
	CPOpp *pPOpp;	
	CWOpp * pNewPoint;
	pNewPoint = new CWOpp();

	if(pNewPoint == NULL) 
	{
		AfxMessageBox("Not enough memory to store the OpPoint\n", MB_OK);
		return;
	}
	else
	{
		//load WOpp with data
		pNewPoint->m_Color = pFrame->GetColor(5);
		bool bFound;
		for(i=0; i<30;i++)
		{
			bFound = false;
			for (j=0; j<m_poaWOpp->GetSize();j++)
			{
				pWOpp = (CWOpp*)m_poaWOpp->GetAt(j);
				if(pWOpp->m_Color == pFrame->m_crColors[i]) bFound = true;
			}
			for (j=0; j<m_poaPOpp->GetSize();j++)
			{
				pPOpp = (CPOpp*)m_poaPOpp->GetAt(j);
				if(pPOpp->m_Color == pFrame->m_crColors[i]) bFound = true;
			}
			if(!bFound) 
			{
				pNewPoint->m_Color = pFrame->m_crColors[i];
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
				if(abs(m_pCurWing->m_BendingMoment[l])>abs(Cb))	Cb = m_pCurWing->m_BendingMoment[l];
			}
		}
		else if(m_pCurWPolar->m_AnalysisType==2)
		{
			pNewPoint->m_Alpha               = m_VLMDlg.m_OpAlpha;
			pNewPoint->m_QInf                = m_VLMDlg.m_QInf;
			pNewPoint->m_NVLMPanels          = m_VLMDlg.m_MatSize;
			pNewPoint->m_bOut                = m_VLMDlg.m_bPointOut;
			pNewPoint->m_CL                  = m_VLMDlg.m_CL;
			pNewPoint->m_InducedDrag         = m_VLMDlg.m_InducedDrag;
			pNewPoint->m_ViscousDrag         = m_VLMDlg.m_ViscousDrag;

			pNewPoint->m_GCm                 = m_VLMDlg.m_GCm;
			pNewPoint->m_GRm                 = m_VLMDlg.m_GRm;
			pNewPoint->m_GYm                 = m_VLMDlg.m_GYm;
			pNewPoint->m_VYm                 = m_VLMDlg.m_VYm;
			pNewPoint->m_IYm                 = m_VLMDlg.m_IYm;

			pNewPoint->m_XCP                 = m_VLMDlg.m_XCP;
			pNewPoint->m_YCP                 = m_VLMDlg.m_YCP;

			if(m_pCurWPolar->m_Type==5) pNewPoint->m_Ctrl = m_VLMDlg.m_Ctrl;
			else                        pNewPoint->m_Ctrl = 0.0;

			for (l=0; l<m_pCurWing->m_NStation; l++)
			{
				pNewPoint->m_SpanPos[l]   = m_pCurWing->m_SpanPos[l];
				pNewPoint->m_StripArea[l] = m_pCurWing->m_StripArea[l];
				if(abs(m_pCurWing->m_BendingMoment[l])>abs(Cb))	Cb = m_pCurWing->m_BendingMoment[l];
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

			memcpy(pNewPoint->m_Cp,         m_VLMDlg.m_Cp,  sizeof(pNewPoint->m_Cp));

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
			pNewPoint->m_Alpha               = m_PanelDlg.m_OpAlpha;
			pNewPoint->m_QInf                = m_PanelDlg.m_QInf;
			pNewPoint->m_NVLMPanels          = m_PanelDlg.m_MatSize;
			pNewPoint->m_bOut                = m_PanelDlg.m_bPointOut;
			pNewPoint->m_CL                  = m_PanelDlg.m_CL;
			pNewPoint->m_InducedDrag         = m_PanelDlg.m_InducedDrag;
			pNewPoint->m_ViscousDrag         = m_PanelDlg.m_ViscousDrag;

			pNewPoint->m_GCm                 = m_PanelDlg.m_GCm;
			pNewPoint->m_GRm                 = m_PanelDlg.m_GRm;
			pNewPoint->m_GYm                 = m_PanelDlg.m_GYm;
			pNewPoint->m_VYm                 = m_PanelDlg.m_VYm;
			pNewPoint->m_IYm                 = m_PanelDlg.m_IYm;

			pNewPoint->m_XCP                 = m_PanelDlg.m_XCP;
			pNewPoint->m_YCP                 = m_PanelDlg.m_YCP;
			for (l=0; l<m_pCurWing->m_NStation; l++)
			{
				pNewPoint->m_SpanPos[l]   = m_pCurWing->m_SpanPos[l];
				pNewPoint->m_StripArea[l] =  m_pCurWing->m_StripArea[l];
				if(abs(m_pCurWing->m_BendingMoment[l])>abs(Cb))	Cb = m_pCurWing->m_BendingMoment[l];
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
			
			pNewPoint->m_nFlaps = m_pCurWing->m_nFlaps;
		}
		pNewPoint->m_MaxBending = Cb;
	}

	m_pCurWPolar->AddPoint(pNewPoint);
	if(m_iView==2) 
	{
		CreateWPolarCurves();
		UpdateView();
	}

	pFrame->SetSaveState(false);

	bool bIsInserted = false;

	// add the WOpPoint to the WOpPoint Array for the current WingName
	if(m_bStoreWOpp){
		for (i=0; i<m_poaWOpp->GetSize(); i++){
			pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
			if (pNewPoint->m_WingName == pWOpp->m_WingName)
			{
				if (pNewPoint->m_PlrName == pWOpp->m_PlrName)
				{
					if(pNewPoint->m_Type !=4)
					{
						if(abs(pNewPoint->m_Alpha - pWOpp->m_Alpha)<0.005)
						{
							//replace existing point
							pNewPoint->m_Color = pWOpp->m_Color;
							pNewPoint->m_Style = pWOpp->m_Style;
							pNewPoint->m_Width = pWOpp->m_Width;
							pNewPoint->m_bIsVisible  = pWOpp->m_bIsVisible;
							pNewPoint->m_bShowPoints = pWOpp->m_bShowPoints;
							if (m_pCurWOpp == pWOpp) m_pCurWOpp = NULL;
							m_poaWOpp->RemoveAt(i);
							delete pWOpp;
							m_poaWOpp->InsertAt(i, pNewPoint);
							bIsInserted = true;
							i = (int)m_poaWOpp->GetSize();// to break
						}
						else if (pNewPoint->m_Alpha > pWOpp->m_Alpha){
							//insert point
							m_poaWOpp->InsertAt(i, pNewPoint);
							bIsInserted = true;
							i = (int)m_poaWOpp->GetSize();// to break
						}
					}
					else
					{
						if(abs(pNewPoint->m_QInf - pWOpp->m_QInf)<0.005)
						{
							//replace existing point
							pNewPoint->m_Color = pWOpp->m_Color;
							pNewPoint->m_Style = pWOpp->m_Style;
							pNewPoint->m_Width = pWOpp->m_Width;
							pNewPoint->m_bIsVisible  = pWOpp->m_bIsVisible;
							pNewPoint->m_bShowPoints = pWOpp->m_bShowPoints;
							if (m_pCurWOpp == pWOpp) m_pCurWOpp = NULL;
							m_poaWOpp->RemoveAt(i);
							delete pWOpp;
							m_poaWOpp->InsertAt(i, pNewPoint);
							bIsInserted = true;
							i = (int)m_poaWOpp->GetSize();// to break
						}
						else if (pNewPoint->m_QInf > pWOpp->m_QInf){
							//insert point
							m_poaWOpp->InsertAt(i, pNewPoint);
							bIsInserted = true;
							i = (int)m_poaWOpp->GetSize();// to break
						}
					}
				}
			}
		}
		if (!bIsInserted) 	m_poaWOpp->Add(pNewPoint);
		m_pCurWOpp = pNewPoint;
	}
	else 
	{
		delete pNewPoint;
	}
}


void CMiarex::InsertWOpp(CWOpp *pNewPoint)
{
	// loads a WOpPoint from a file	
	// adds it to the WOpp array

	int i;
	bool bIsInserted = false;
	CWOpp* pWOpp;

	// add the WOpPoint to the WOpPoint Array for the current WingName
	for (i=0; i<m_poaWOpp->GetSize(); i++)
	{
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		if (pNewPoint->m_WingName == pWOpp->m_WingName)
		{
			if (pNewPoint->m_PlrName == pWOpp->m_PlrName)
			{
				if(pNewPoint->m_Type !=4)
				{
					if(abs(pNewPoint->m_Alpha - pWOpp->m_Alpha)<0.005)
					{
						//replace existing point
						if (m_pCurWOpp == pWOpp) m_pCurWOpp = NULL;
						m_poaWOpp->RemoveAt(i);
						delete pWOpp;
						m_poaWOpp->InsertAt(i, pNewPoint);
						bIsInserted = true;
						i = (int)m_poaWOpp->GetSize();// to break
					}
					else if (pNewPoint->m_Alpha > pWOpp->m_Alpha)
					{
						//insert point
						m_poaWOpp->InsertAt(i, pNewPoint);
						bIsInserted = true;
						i = (int)m_poaWOpp->GetSize();// to break
					}
				}
				else
				{
					if(abs(pNewPoint->m_QInf - pWOpp->m_QInf)<0.005)
					{
						//replace existing point
						if (m_pCurWOpp == pWOpp) m_pCurWOpp = NULL;
						m_poaWOpp->RemoveAt(i);
						delete pWOpp;
						m_poaWOpp->InsertAt(i, pNewPoint);
						bIsInserted = true;
						i = (int)m_poaWOpp->GetSize();// to break
					}
					else if (pNewPoint->m_QInf > pWOpp->m_QInf)
					{
						//insert point
						m_poaWOpp->InsertAt(i, pNewPoint);
						bIsInserted = true;
						i = (int)m_poaWOpp->GetSize();// to break
					}
				}
			}
		}
	}
	if (!bIsInserted) 	m_poaWOpp->Add(pNewPoint);
} 


CWOpp* CMiarex::GetWOpp(double Alpha)
{
	// returns a pointer to the WOpp corresponding to aoa Alpha,
	// and with the name of the current wing and current WPolar
	if(!m_pCurWing || !m_pCurWPolar) return NULL;
	int i;
	CWOpp* pWOpp;

	for (i=0; i<m_poaWOpp->GetSize(); i++)
	{
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		if ((pWOpp->m_WingName == m_pCurWing->m_WingName) &&(pWOpp->m_PlrName == m_pCurWPolar->m_PlrName))
		{
			if(m_pCurWPolar->m_Type<3 && abs(pWOpp->m_Alpha -Alpha)<0.001)       return pWOpp;
			else if(m_pCurWPolar->m_Type==4 && abs(pWOpp->m_QInf - Alpha)<0.001) return pWOpp;
			else if(m_pCurWPolar->m_Type==5 && abs(pWOpp->m_Ctrl - Alpha)<0.0001) return pWOpp;
		}
	}
	return NULL;
}


CPOpp* CMiarex::GetPOpp(double Alpha)
{
	// returns a pointer to the WOpp corresponding to aoa Alpha,
	// and with the name of the current plane and current WPolar

	int i;
	if(!m_pCurPlane || !m_pCurWPolar) return NULL;

	CPOpp* pPOpp;

	for (i=0; i<m_poaPOpp->GetSize(); i++)
	{
		pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
		if ((pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName) &&	(pPOpp->m_PlrName   == m_pCurWPolar->m_PlrName))
		{
			if(m_pCurWPolar->m_Type<=3 && abs(pPOpp->m_Alpha -Alpha)<0.001)         return pPOpp;
			else if(m_pCurWPolar->m_Type==4 && abs(pPOpp->m_QInf - Alpha)<0.001)	return pPOpp;
			else if(m_pCurWPolar->m_Type==5 && abs(pPOpp->m_Ctrl - Alpha)<0.001)	return pPOpp;

		}
	}
	return NULL;
}


bool CMiarex::SetPOpp(bool bCurrent, double Alpha)
{
	// set the WOpp, if valid
	// else set the current WOpp, if any
	// else set the comboBox's first, if any
	// else set it to NULL
	if(!m_pCurPlane) return false;

	CPOpp *pPOpp = NULL;
	if(bCurrent) pPOpp = m_pCurPOpp;
	else         pPOpp = GetPOpp(Alpha);

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CString strong;
	m_bResetglOpp    = true;
	m_bResetglWake   = true;
	m_bResetglStream = true;
	m_bResetglFlow   = true;
	m_bResetglLegend = true;

	if(!pPOpp) {
		if(m_pCurWPolar)
			pPOpp = GetPOpp(m_pCurWPolar->m_AMem);
		else 
			pPOpp = GetPOpp(m_LastWOpp);
	}

//	if(!pPOpp) pPOpp = m_pCurPOpp;
	

	if(!pPOpp){//try to select the first in the ListBox
		if(pFrame->m_ctrlWOpp.GetCount()){
			double x;
			pFrame->m_ctrlWOpp.SetCurSel(0);
			pFrame->m_ctrlWOpp.GetLBText(0, strong);
			int res = sscanf(strong, "%lf", &x);
			if(res==1) {
				pPOpp = GetPOpp(x);
			}
			else pPOpp = NULL;
		}
		else{
			pPOpp = NULL;
		}
	}
	m_pCurPOpp = pPOpp;
	pFrame->m_PolarDlgBar.SetParams();
	if(m_pCurPOpp) {
		m_LastWOpp = m_pCurPOpp->m_Alpha;
		m_pCurWOpp = &m_pCurPOpp->m_WingWOpp;
		if(m_pCurWPolar) m_pCurWPolar->m_AMem = m_pCurPOpp->m_Alpha;

		//select m_pCurPOpp in the listbox
		if(m_pCurWPolar->m_Type != 4) strong.Format("%8.2f", m_pCurPOpp->m_Alpha);
		else                          strong.Format("%8.2f", m_pCurPOpp->m_QInf);

		int pos = pFrame->m_ctrlWOpp.FindStringExact(-1,strong);
		if(pos != CB_ERR) {
			pFrame->m_ctrlWOpp.SetCurSel(pos);
		}
		else {
			pFrame->m_ctrlWOpp.SetCurSel(0);
		}
	}
	else 
	{
		m_pCurWOpp = NULL;
	}

	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();
	if(!m_pCurPOpp) return false;
	else            return true;
}


bool CMiarex::SetWOpp(bool bCurrent, double Alpha)
{
	// set set the current WOpp, if any
	// else set the comboBox's first, if any
	// else set it to NULL
	CWaitCursor wait;
	CString strong;

	if(!m_pCurWing)   return false;
	if(!m_pCurWPolar) return false;
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
	
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	m_bResetglOpp    = true;
	m_bResetglWake   = true;
	m_bResetglStream = true;
	m_bResetglFlow   = true;
	m_bResetglLegend = true;

	if(!pWOpp) {
	//first try to set the last alpha which has been selected
		if(m_pCurWPolar)
			pWOpp = GetWOpp(m_pCurWPolar->m_AMem);
		else 
			pWOpp = GetWOpp(m_LastWOpp);
	}

	if(!pWOpp){//try to select the first in the ListBox
		if(pFrame->m_ctrlWOpp.GetCount()){
			double x;
			pFrame->m_ctrlWOpp.SetCurSel(0);
			pFrame->m_ctrlWOpp.GetLBText(0, strong);
			int res = sscanf(strong, "%lf", &x);
			if(res==1) {
				pWOpp = GetWOpp(x);
			}
			else pWOpp = NULL;
		}
		else{
			pWOpp = NULL;
		}
	}
	m_pCurWOpp = pWOpp;
	pFrame->m_PolarDlgBar.SetParams();
	if(m_pCurWOpp) 
	{
		m_LastWOpp = m_pCurWOpp->m_Alpha;
		m_pCurWPolar->m_AMem = m_pCurWOpp->m_Alpha;
//		m_nWakeNodes   = m_pCurWOpp->m_nWakeNodes;
//		m_NXWakePanels = m_pCurWOpp->m_NXWakePanels;
//		m_TotalWakeLength  =		m_pCurWOpp->m_FirstWakePanel;
//		m_WakePanelFactor =		m_pCurWOpp->m_WakeFactor;
	
		//select m_pCurWOpp in the listbox
		if(m_pCurWPolar->m_Type != 4) strong.Format("%8.2f", m_pCurWOpp->m_Alpha);
		else                          strong.Format("%8.2f", m_pCurWOpp->m_QInf);

		int pos = pFrame->m_ctrlWOpp.FindStringExact(-1,strong);
		if(pos != CB_ERR) {
			pFrame->m_ctrlWOpp.SetCurSel(pos);
		}
		else {
			pFrame->m_ctrlWOpp.SetCurSel(0);
		}
	}

	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();
	if(!m_pCurWOpp) return false;
	else            return true;
}


void CMiarex::DrawWOppLegend(CDC *pDC, bool bIsPrinting, CPoint place, int bottom)
{
	//draws the WOpps legend to the device context,
	// bottom is the lower limit not to exceed for the legend

	CChildView * pChildView = (CChildView*)m_pChildWnd;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	int LegendSize, LegendWidth, ypos;
	int i, j, k,l, x1, nc;

	COLORREF color;
	int style, width;
	CString strong, str1, str2, str3, str4;

	LOGFONT TempFont;
	memcpy(&TempFont, &pChildView->m_WndLogFont,sizeof(LOGFONT));

	if(bIsPrinting) {
		TempFont.lfHeight = TempFont.lfHeight*30;
		LegendSize = 1500;
		LegendWidth = 9000;
		ypos = -400;
		bottom -= 1000;//margin
	}
	else{ 
		LegendSize = 30;
		LegendWidth = 260;
		ypos = 14;
	}
	CFont ThisFont;
	ThisFont.CreateFontIndirect(&TempFont);

	CStringArray str; // we need to make an inventory of wings
	bool bFound;
	CWOpp * pWOpp = NULL;
	CPOpp *pPOpp = NULL;

	for (i=0; i<m_poaWOpp->GetSize(); i++){
		bFound = false;
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		for (j=0; j<str.GetSize(); j++){
			if (pWOpp->m_WingName == str.GetAt(j))
					bFound = true;
		}
		if (!bFound){
			str.Add(pWOpp->m_WingName);
		}
	}
	int nWings= (int)str.GetSize();

	for (i=0; i<m_poaPOpp->GetSize(); i++){
		bFound = false;
		pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
		for (j=0; j<str.GetSize(); j++){
			if (pPOpp->m_PlaneName == str.GetAt(j))
					bFound = true;
		}
		if (!bFound){
			str.Add(pPOpp->m_PlaneName);
		}
	}
	int nPlanes= (int)str.GetSize() - nWings;

	pDC->SetTextAlign(TA_LEFT);
	pDC->SetBkMode(TRANSPARENT);

	if(IsBlackAndWhite()){
		pDC->SetTextColor(0);
	}
	else{
//		if(m_iWingView==1 && m_iView==1)	pDC->SetTextColor(m_WingGraph1.GetLegendColor());
//		else 	                          
			pDC->SetTextColor(pChildView->m_WndTextColor);
	}

	CFont* pOldFont = pDC->SelectObject(&ThisFont);
	int ny =0;
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	CBrush LegendBrush(pChildView->m_crBackColor);
	CBrush* pOldBrush = pDC->SelectObject(&LegendBrush);

	if(m_bCurWOppOnly)
	{
		if(!m_pCurWOpp) return;
		if(!m_pCurWOpp->m_bIsVisible) return;
		pDC->TextOut(place.x + (int)(1*LegendSize), place.y + ypos*ny-(int)(ypos/2),
					 m_pCurWing->m_WingName);
		ny++ ;

		m_pCurWOpp->GetBWStyle(color, style, width);
		lb.lbColor = color;

		CPen LegendPen(PS_GEOMETRIC | style,
					   GetPenWidth(width,bIsPrinting), &lb);

		CPen *pOldPen = pDC->SelectObject(&LegendPen);

		pDC->MoveTo(place.x + (int)(1.5*LegendSize),
					place.y + (int)(1.*ypos*ny));
		pDC->LineTo(place.x + (int)(2.5*LegendSize),
					place.y + (int)(1.*ypos*ny));
		if(m_pCurWOpp->m_bShowPoints){
			x1 = place.x + (int)(2.0*LegendSize);
			if(!bIsPrinting)
				pDC->Rectangle(x1-2, place.y + (int)(1.*ypos*ny)-2,
							   x1+2, place.y + (int)(1.*ypos*ny)+2);
			else{
				CBrush PrintBrush;
				if(IsBlackAndWhite()){
					PrintBrush.CreateSolidBrush(RGB(255,255,255));
				}
				else{
					if(GetWindowBk()) PrintBrush.CreateSolidBrush(pChildView->m_crBackColor);
					else PrintBrush.CreateSolidBrush(RGB(255,255,255));
				}
				CBrush *pOldBrush1 = pDC->SelectObject(&PrintBrush);
				pDC->Rectangle(x1-50, place.y + (int)(1.*ypos*ny)-50,
							   x1+50, place.y + (int)(1.*ypos*ny)+50);
				pDC->SelectObject(pOldBrush1);
			}
		}
		
		str1.Format("V=%5.2f", m_pCurWOpp->m_QInf*pFrame->m_mstoUnit);
		GetSpeedUnit(str2, pFrame->m_SpeedUnit);
		str3.Format("_a=%5.2f", m_pCurWOpp->m_Alpha);

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

		pDC->TextOut(place.x + (int)(3*LegendSize),
					 place.y + (int)(1.*ypos*ny)-(int)(ypos/2),
					 str1+str2+str3+str4);
		pDC->SelectObject(pOldPen);			
	}
	else 
	{
		bool bStarted = false;
		for (k = 0; k<str.GetSize(); k++){
			int UFOPts = 0;
			for (l=0; l < m_poaWOpp->GetSize(); l++){
				pWOpp = (CWOpp*)m_poaWOpp->GetAt(l);
				if (pWOpp->m_bIsVisible &&	pWOpp->m_WingName == str.GetAt(k) )
						UFOPts++;
			}
			for (l=0; l < m_poaPOpp->GetSize(); l++){
				pPOpp = (CPOpp*)m_poaPOpp->GetAt(l);
				if (pPOpp->m_bIsVisible &&	pPOpp->m_PlaneName == str.GetAt(k) )
						UFOPts++;
			}
			if (UFOPts) {
				int YPos = place.y+ (ny+UFOPts+2) * ypos;// bottom line of this foil's legend
				if (!bStarted || (abs(bottom) > abs(YPos))){
					ny++;
					pDC->TextOut(place.x + (int)(1*LegendSize), place.y + ypos*ny-(int)(ypos/2),
								 str.GetAt(k));
				}
				else{
				// move rigth if outside screen
					place.x += LegendWidth;
					ny=1;
					pDC->TextOut(place.x + (int)(1*LegendSize), place.y + ypos*ny-(int)(ypos/2),
								 str.GetAt(k));
				}

				bStarted = true;
				for (nc=0; nc < m_poaWOpp->GetSize(); nc++){
					pWOpp = (CWOpp*)m_poaWOpp->GetAt(nc);
					if(str.GetAt(k) == pWOpp->m_WingName && pWOpp->m_bIsVisible){
						ny++ ;
						if(abs(bottom)<abs(place.y + (int)(1.*ypos*ny+ypos))){
							//move right
							place.x += LegendWidth;
							ny=2;
						}

						pWOpp->GetBWStyle(color, style, width);
						lb.lbColor = color;

						CPen LegendPen(PS_GEOMETRIC | style, GetPenWidth(width,bIsPrinting), &lb);
						CPen *pOldPen = pDC->SelectObject(&LegendPen);

						pDC->MoveTo(place.x + (int)(1.5*LegendSize),
									place.y + (int)(1.*ypos*ny));
						pDC->LineTo(place.x + (int)(2.5*LegendSize),
									place.y + (int)(1.*ypos*ny));

						if(pWOpp->m_bShowPoints && !bIsPrinting){
							x1 = place.x + (int)(2.0*LegendSize);
							pDC->Rectangle(x1-2, place.y + (int)(1.*ypos*ny)-2,
										   x1+2, place.y + (int)(1.*ypos*ny)+2);
						}
						
						str1.Format("V=%5.2f", pWOpp->m_QInf*pFrame->m_mstoUnit);
						GetSpeedUnit(str2, pFrame->m_SpeedUnit);
						str3.Format("_a=%5.2f", pWOpp->m_Alpha);
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
						if(pWOpp->m_bOut)         str4+=" (Out)";


						pDC->TextOut(place.x + (int)(3*LegendSize),




									 place.y + (int)(1.*ypos*ny)-(int)(ypos/2),
									 str1+str2+str3+str4);

						pDC->SelectObject(pOldPen);	
					}
				}
				for (nc=0; nc < m_poaPOpp->GetSize(); nc++){
					pPOpp = (CPOpp*)m_poaPOpp->GetAt(nc);
					if(str.GetAt(k) == pPOpp->m_PlaneName && pPOpp->m_bIsVisible){
						ny++ ;
						if(abs(bottom)<abs(place.y + (int)(1.*ypos*ny+ypos))){
							//move right
							place.x += LegendWidth;
							ny=2;
						}

						pPOpp->GetBWStyle(color, style, width);
						lb.lbColor = color;

						CPen LegendPen(PS_GEOMETRIC | style, GetPenWidth(width,bIsPrinting), &lb);
						CPen *pOldPen = pDC->SelectObject(&LegendPen);

						pDC->MoveTo(place.x + (int)(1.5*LegendSize),
									place.y + (int)(1.*ypos*ny));
						pDC->LineTo(place.x + (int)(2.5*LegendSize),
									place.y + (int)(1.*ypos*ny));

						if(pPOpp->m_bShowPoints && !bIsPrinting){
							x1 = place.x + (int)(2.0*LegendSize);
							pDC->Rectangle(x1-2, place.y + (int)(1.*ypos*ny)-2,
										   x1+2, place.y + (int)(1.*ypos*ny)+2);
						}
						
						str1.Format("V=%5.2f", pPOpp->m_QInf*pFrame->m_mstoUnit);
						GetSpeedUnit(str2, pFrame->m_SpeedUnit);
						str3.Format("-a=%5.2f", pPOpp->m_Alpha);

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

						pDC->TextOut(place.x + (int)(3*LegendSize),
									 place.y + (int)(1.*ypos*ny)-(int)(ypos/2),
									 str1+str2+str3+str4);

						pDC->SelectObject(pOldPen);	
					}
				}
				if (UFOPts) ny++;
			}
		}
	}
	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldBrush);
}

void CMiarex::DrawCpLegend(CDC *pDC, bool bIsPrinting, CPoint place, int bottom)
{
	//draws the WOpps legend to the device context,
	// bottom is the lower limit not to exceed for the legend

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	int LegendSize, LegendWidth, dny, x1, i, style, width, ny;
	CCurve *pCurve=NULL;
	COLORREF color;
	CString strong, str1, str2, str3, str4;
	CChildView * pChildView = (CChildView*)m_pChildWnd;
	CPen  *pOldPen = NULL;
	CBrush  *pOldBrush = NULL;
	LOGBRUSH lb;
	CFont ThisFont, *pOldFont = NULL;
	LOGFONT TempFont;
	memcpy(&TempFont, &pChildView->m_WndLogFont,sizeof(LOGFONT));
	CBrush LegendBrush(pChildView->m_crBackColor);
	pOldBrush = pDC->SelectObject(&LegendBrush);

	if(bIsPrinting) {
		TempFont.lfHeight = TempFont.lfHeight*30;
		LegendSize = 1500;
		LegendWidth = 12000;
		dny = -400;
		bottom -= 1000;//margin
	}
	else{ 
		LegendSize = 30;
		LegendWidth = 350;
		dny = 14;
		bottom -= 15;//margin
	}
	ThisFont.CreateFontIndirect(&TempFont);

	pDC->SetTextAlign(TA_LEFT);
	pDC->SetBkMode(TRANSPARENT);

	if(IsBlackAndWhite()){
		pDC->SetTextColor(0);
	}
	else{
		pDC->SetTextColor(pChildView->m_WndTextColor);
	}

	pOldFont = pDC->SelectObject(&ThisFont);

	lb.lbStyle = BS_SOLID;

	ny=-1;
	int nada = m_CpGraph.GetCurveCount();
	for (i=0; i<m_CpGraph.GetCurveCount(); i++)
	{
		pCurve = m_CpGraph.GetCurve(i);
		ny++;
		
		if(abs(bottom)<abs(place.y + (int)(dny*(ny+1)))){
			//move right
			place.x += LegendWidth;
			ny=0;
		}
	
		color = pCurve->GetColor();
		style = pCurve->GetStyle();
		width = pCurve->GetWidth();
		lb.lbColor = color;

		CPen LegendPen(PS_GEOMETRIC | style, GetPenWidth(width,bIsPrinting), &lb);
		pOldPen = pDC->SelectObject(&LegendPen);

		pDC->MoveTo(place.x + (int)(1.5*LegendSize),
					place.y + (int)(1.*dny*ny));
		pDC->LineTo(place.x + (int)(2.5*LegendSize),
					place.y + (int)(1.*dny*ny));

		if(pCurve->m_bShowPoints)
		{
			x1 = place.x + (int)(2.0*LegendSize);
			if(!bIsPrinting)
				pDC->Rectangle(x1-2, place.y + (int)(1.*dny*ny)-2,
							   x1+2, place.y + (int)(1.*dny*ny)+2);
			else{
				CBrush PrintBrush;
				if(IsBlackAndWhite()){
					PrintBrush.CreateSolidBrush(RGB(255,255,255));
				}
				else{
					if(GetWindowBk()) PrintBrush.CreateSolidBrush(pChildView->m_crBackColor);
					else PrintBrush.CreateSolidBrush(RGB(255,255,255));
				}
				CBrush *pOldBrush1 = pDC->SelectObject(&PrintBrush);
				pDC->Rectangle(x1-50, place.y + (int)(1.*dny*ny)-50,
							   x1+50, place.y + (int)(1.*dny*ny)+50);
				pDC->SelectObject(pOldBrush1);
			}
		}

		pDC->TextOut(place.x + (int)(3*LegendSize),
					 place.y + (int)(1.*dny*ny)-(int)(dny/2),
					 pCurve->GetTitle());
	}
	if(pOldPen)   pDC->SelectObject(pOldPen);			
	if(pOldFont)  pDC->SelectObject(pOldFont);
	if(pOldBrush) pDC->SelectObject(pOldBrush);
}

void CMiarex::DrawWPolarLegend(CDC *pDC, bool bIsPrinting, CPoint place, int bottom)
{
	//draws the WPolar legend to the device context,
	// bottom is the lower limit not to exceed for the legend

	int LegendSize, LegendWidth, ypos;
	int i,j,x1;

	COLORREF color;
	int style, width;

	CChildView * pChildView = (CChildView*)m_pChildWnd;

	LOGFONT TempFont;
	m_WPlrGraph1.GetLegendLogFont(&TempFont);

	if(bIsPrinting) {
		TempFont.lfHeight = TempFont.lfHeight*30;
		LegendSize = 1500;
		LegendWidth = 7500;
		ypos = -400;
//		bottom -= 1000;//margin
	}
	else{ 
		LegendSize = 30;
		LegendWidth = 280;
		ypos = 14;
	}
	CFont ThisFont;
	ThisFont.CreateFontIndirect(&TempFont);

	CStringArray str; // we need to make an inventory of wings
	CWPolar * pWPolar;
	CWing *pWing;
	CPlane *pPlane;
	for (j=0; j<m_poaWing->GetSize(); j++){
		pWing = (CWing*)m_poaWing->GetAt(j);
		for (i=0; i<m_poaWPolar->GetSize(); i++){
			pWPolar = (CWPolar*)m_poaWPolar->GetAt(i);
			if (pWPolar->m_UFOName == pWing->m_WingName){
				str.Add(pWing->m_WingName);
				break;
			}
		}// finished inventory
	}

	for (j=0; j<m_poaPlane->GetSize(); j++){
		pPlane = (CPlane*)m_poaPlane->GetAt(j);
		for (i=0; i<m_poaWPolar->GetSize(); i++){
			pWPolar = (CWPolar*)m_poaWPolar->GetAt(i);
			if (pWPolar->m_UFOName == pPlane->m_PlaneName){
				str.Add(pPlane->m_PlaneName);
				break;
			}
		}// finished inventory
	}

	int nUFOs= (int)str.GetSize();

	pDC->SetTextAlign(TA_LEFT);
	pDC->SetBkMode(TRANSPARENT);
	if(IsBlackAndWhite()){
		pDC->SetBkColor(RGB(255,255,255));
		pDC->SetTextColor(0);
	}
	else{
		pDC->SetBkColor(pChildView->m_crBackColor);
		pDC->SetTextColor(m_WPlrGraph1.GetLegendColor());
	}
	CFont* pOldFont = pDC->SelectObject(&ThisFont);

	CBrush LegendBrush(pChildView->m_crBackColor);
	CBrush* pOldBrush = pDC->SelectObject(&LegendBrush);

	int ny =0;
	for (int k = 0; k<nUFOs; k++){
		int UFOPlrs = 0;
		for (int l=0; l < m_poaWPolar->GetSize(); l++){
			pWPolar = (CWPolar*)m_poaWPolar->GetAt(l);
			if (pWPolar->m_Alpha.GetSize() &&
				pWPolar->m_PlrName.GetLength() &&
				pWPolar->m_bIsVisible &&
				pWPolar->m_UFOName == str.GetAt(k) &&
				(pWPolar->m_Type == 1 && m_bType1 ||
				 pWPolar->m_Type == 2 && m_bType2 ||
				 pWPolar->m_Type == 4 && m_bType4 ||
				 pWPolar->m_Type == 5))

					UFOPlrs++;
		}
		if (UFOPlrs) {
			int YPos = place.y+ (ny+UFOPlrs+2) * ypos;// bottom line of this foil's legend
			if(abs(bottom) > abs(YPos)){
				ny++;
				pDC->TextOut(place.x, place.y + ypos*ny-(int)(ypos/2),
							 str.GetAt(k));
			}
			else{
			// move rigth if outside screen
				place.x += LegendWidth;
				ny=1;
				pDC->TextOut(place.x , place.y + ypos*ny-(int)(ypos/2),
							 str.GetAt(k));
			}

			for (int nc=0; nc < m_poaWPolar->GetSize(); nc++){
				pWPolar = (CWPolar*)m_poaWPolar->GetAt(nc);
				if(str.GetAt(k) == pWPolar->m_UFOName){
					if (pWPolar->m_Alpha.GetSize() &&
						pWPolar->m_PlrName.GetLength() &&
						pWPolar->m_bIsVisible &&
						(pWPolar->m_Type == 1 && m_bType1 ||
						 pWPolar->m_Type == 2 && m_bType2 ||
						 pWPolar->m_Type == 4 && m_bType4 ||
						 pWPolar->m_Type == 5)){//is there anything to draw ?

						ny++ ;

						pWPolar->GetBWStyle(color, style, width);
						LOGBRUSH lb;
						lb.lbStyle = BS_SOLID;
						lb.lbColor = color;

						CPen LegendPen(PS_GEOMETRIC | style,
									   GetPenWidth(width,bIsPrinting), &lb);

						CPen *pOldPen = pDC->SelectObject(&LegendPen);

						pDC->MoveTo(place.x + (int)(0.5*LegendSize),
									place.y + (int)(1.*ypos*ny));
						pDC->LineTo(place.x + (int)(1.3*LegendSize),
									place.y + (int)(1.*ypos*ny));

						if(pWPolar->m_bShowPoints){
							x1 = place.x + (int)(0.9*LegendSize);
							if(!bIsPrinting)
								pDC->Rectangle(x1-2, place.y + (int)(1.*ypos*ny)-2,
											   x1+2, place.y + (int)(1.*ypos*ny)+2);
							else{
								CBrush PrintBrush;
								if(IsBlackAndWhite()){
									PrintBrush.CreateSolidBrush(RGB(255,255,255));
								}
								else{
									if(GetWindowBk()) PrintBrush.CreateSolidBrush(pChildView->m_crBackColor);
									else PrintBrush.CreateSolidBrush(RGB(255,255,255));
								}
								CBrush *pOldBrush1 = pDC->SelectObject(&PrintBrush);
								pDC->Rectangle(x1-50, place.y + (int)(1.*ypos*ny)-50,
											   x1+50, place.y + (int)(1.*ypos*ny)+50);
								pDC->SelectObject(pOldBrush1);
							}
						}

						pDC->TextOut(place.x + (int)(1.4*LegendSize),
									 place.y + (int)(1.*ypos*ny)-(int)(ypos/2),
									 pWPolar->m_PlrName);
						pDC->SelectObject(pOldPen);
					}
				}
			}
			if (UFOPlrs) ny++;
		}
	}
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldFont);
}


double CMiarex::GetCd(CFoil *pFoil0, CFoil *pFoil1, 
						double Re, double Alpha, double Tau, double AR,
						bool &bOutRe, bool &bError)
{
	//For LLT calculations
	//returns the interpolated viscous drag
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	double Cd0, Cd1, Cl;
	if(!pFoil0) {
		Cl = 2.0*pi*(Alpha*pi/180.0);
		Cd0 = Cl*Cl/pi/AR;
	}
	else Cd0 = pFrame->GetPlrPoint(pFoil0, Re, Alpha,2, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;
	if(!pFoil1) {
		Cl = 2.0*pi*(Alpha*pi/180.0);
		Cd1 = Cl*Cl/pi/AR;
	}
	else Cd1 = pFrame->GetPlrPoint(pFoil1, Re, Alpha,2, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;
	return ((1-Tau) * Cd0 + Tau * Cd1);
}


double CMiarex::GetXCp(CFoil *pFoil0, CFoil *pFoil1, double Re, double Alpha, double Tau, double AR,
					   bool &bOutRe, bool &bError)
{
	//For LLT calculations
	//returns the interpolated center of pressure position

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	double XCp0, XCp1;

	if(!pFoil0) return 0.0;
	else XCp0 = pFrame->GetPlrPoint(pFoil0, Re, Alpha, 11, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if(!pFoil1) return 0.0;
	else XCp1 = pFrame->GetPlrPoint(pFoil1, Re, Alpha, 11, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;

	return ((1-Tau) * XCp0 + Tau * XCp1);
}

double CMiarex::GetXTr(CFoil *pFoil0, CFoil *pFoil1, double Re,
					  double Alpha, double Tau, bool bTop, bool &bOutRe, bool &bError)
{
	//For LLT calculations
	//returns the interpolated position of the transition on the  surface specified by bTop

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	double Tr0, Tr1;
	if(!pFoil0) {
		Tr0 = 1.0;
	}
	else {
		if(bTop) Tr0 = pFrame->GetPlrPoint(pFoil0, Re, Alpha, 5, IsOutRe, IsError);
		else     Tr0 = pFrame->GetPlrPoint(pFoil0, Re, Alpha, 6, IsOutRe, IsError);
	}
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;
	if(!pFoil1) {
		Tr1 = 1.0;
	}
	else {
		if(bTop) Tr1 = pFrame->GetPlrPoint(pFoil1, Re, Alpha, 5, IsOutRe, IsError);
		else     Tr1 = pFrame->GetPlrPoint(pFoil1, Re, Alpha, 6, IsOutRe, IsError);
	}
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;
	return ((1-Tau) * Tr0 + Tau * Tr1);
}



Graph* CMiarex::GetGraph(CPoint pt)
{
	//returns a pointer to the graph in which the point pt lies
	if(m_iView==1)
	{
		if(m_iWingView==1) 
		{
			m_pCurWingGraph = m_pCurGraph;
			return m_pCurGraph;
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
			m_pCurWPlrGraph = m_pCurGraph;
			return m_pCurGraph;
		}
		else if (m_iWPlrView==2){
			if(m_WPlrGraph1.IsInDrawRect(pt)){return &m_WPlrGraph1;}
			if(m_WPlrGraph2.IsInDrawRect(pt)){return &m_WPlrGraph2;}
			return NULL;
		}
		else{
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


void CMiarex::OnContextMenu(CPoint ScreenPoint, CPoint ClientPoint) 
{
	//displays the context menu
	if(!m_rCltRect.PtInRect(ClientPoint))return;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CMenu menu;
	BOOL BMenu;

	m_ptPopUp.x = ClientPoint.x;
	m_ptPopUp.y = ClientPoint.y;

	m_pCurGraph = GetGraph(ClientPoint);

	if(m_iView==5)
	{
		ClientToGL(m_ptPopUp, m_RealPopUp);
		if(m_BodyRect.PtInRect(ClientPoint))            BMenu = menu.LoadMenu(IDR_CTXBODYCTRLMENU);
		else if(m_BodyLineRect.PtInRect(ClientPoint))   BMenu = menu.LoadMenu(IDR_CTXBODYFRAMECTRLMENU);
		else if(m_FrameRect.PtInRect(ClientPoint))      BMenu = menu.LoadMenu(IDR_CTXBODYPOINTCTRLMENU);
		else BMenu = FALSE;
		if (BMenu)
		{
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);

			if(m_FrameRect.PtInRect(ClientPoint) && m_bCurFrameOnly)
				pPopup->CheckMenuItem(IDM_SHOWONLYACTIVEFRAME,     MF_BYCOMMAND | MF_CHECKED);

			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				ScreenPoint.x, ScreenPoint.y, m_pFrame); // use main window for cmds
		}
	}
	else if(m_iView==3)
	{
		if (menu.LoadMenu(IDR_CTXWING3DMENU)){
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);

			pPopup->CheckMenuItem(IDM_3DVIEW,     MF_BYCOMMAND | MF_CHECKED);
			if (m_bXCmRef) pPopup->CheckMenuItem(IDM_SHOWXCMREF, MF_BYCOMMAND | MF_CHECKED);

			
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				ScreenPoint.x, ScreenPoint.y, m_pFrame); // use main window for cmds
		}
	}
	else if(m_iView==1 || m_iView==4)
	{
		if (menu.LoadMenu(IDR_CTXWINGOPPMENU)){
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);

			if(m_bShowWing2)	pPopup->CheckMenuItem(IDM_SHOWWING2, MF_BYCOMMAND | MF_CHECKED);
			if(m_bShowStab)		pPopup->CheckMenuItem(IDM_SHOWSTAB, MF_BYCOMMAND | MF_CHECKED);
			if(m_bShowFin)		pPopup->CheckMenuItem(IDM_SHOWFIN, MF_BYCOMMAND | MF_CHECKED);

			if (m_bXCmRef) pPopup->CheckMenuItem(IDM_SHOWXCMREF, MF_BYCOMMAND | MF_CHECKED);

			if(m_bCurWOppOnly)
				  pPopup->CheckMenuItem(IDM_SHOWCURWOPP,MF_BYCOMMAND | MF_CHECKED);
			if(m_bShowElliptic)
				  pPopup->CheckMenuItem(IDM_SHOWELLIPTIC,MF_BYCOMMAND | MF_CHECKED);

			if(m_iWingView ==1){
				if(m_pCurGraph==&m_WingGraph1)
					pPopup->CheckMenuItem(IDM_SINGLEGRAPH1, MF_BYCOMMAND | MF_CHECKED);
				if(m_pCurGraph==&m_WingGraph2) 
					pPopup->CheckMenuItem(IDM_SINGLEGRAPH2, MF_BYCOMMAND | MF_CHECKED);
				if(m_pCurGraph==&m_WingGraph3) 
					pPopup->CheckMenuItem(IDM_SINGLEGRAPH3, MF_BYCOMMAND | MF_CHECKED);
				if(m_pCurGraph==&m_WingGraph4) 
					pPopup->CheckMenuItem(IDM_SINGLEGRAPH4, MF_BYCOMMAND | MF_CHECKED);
			}
			else if (m_iWingView == 2)
				pPopup->CheckMenuItem(IDM_TWOWINGGRAPHS,   MF_BYCOMMAND | MF_CHECKED);
			else if (m_iWingView == 4)
				pPopup->CheckMenuItem(IDM_FOURWINGGRAPHS,  MF_BYCOMMAND | MF_CHECKED);
			
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				ScreenPoint.x, ScreenPoint.y, m_pFrame); // use main window for cmds
		}
	}
	else if(m_iView==2)
	{
		if (menu.LoadMenu(IDR_CTXWINGPLRMENU)){
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);

			if (m_iWPlrView == 1) {
				if(m_pCurGraph==&m_WPlrGraph1)
					pPopup->CheckMenuItem(IDM_SINGLEGRAPH1, MF_BYCOMMAND | MF_CHECKED);
				if(m_pCurGraph==&m_WPlrGraph2) 
					pPopup->CheckMenuItem(IDM_SINGLEGRAPH2, MF_BYCOMMAND | MF_CHECKED);
				if(m_pCurGraph==&m_WPlrGraph3) 
					pPopup->CheckMenuItem(IDM_SINGLEGRAPH3, MF_BYCOMMAND | MF_CHECKED);
				if(m_pCurGraph==&m_WPlrGraph4) 
					pPopup->CheckMenuItem(IDM_SINGLEGRAPH4, MF_BYCOMMAND | MF_CHECKED);
			}
			if (m_iWPlrView == 2) pPopup->CheckMenuItem(IDM_TWOGRAPHS,MF_BYCOMMAND   | MF_CHECKED);
			if (m_iWPlrView == 4) pPopup->CheckMenuItem(IDM_FOURGRAPHS,MF_BYCOMMAND  | MF_CHECKED);
			
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				ScreenPoint.x, ScreenPoint.y, m_pFrame); // use main window for cmds
		}
	}
}


void CMiarex::OnGraphDlg() 
{
	//displays the graph options dialog box
	if(m_pCurGraph){
		CGraphDlg dlg(m_pChildWnd);

		dlg.m_pGraph = m_pCurGraph;

		if(IDOK == dlg.DoModal()){
			UpdateView();
		}
	}	
}

void CMiarex::SaveSettings(CArchive &ar)
{
	//save the Miarex settings to the archive ar
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	int i;
	CString str;
	//  we're saving/storing

	m_WingGraph1.Serialize(ar);
	m_WingGraph2.Serialize(ar);
	m_WingGraph3.Serialize(ar);
	m_WingGraph4.Serialize(ar);

	m_WPlrGraph1.Serialize(ar);
	m_WPlrGraph2.Serialize(ar);
	m_WPlrGraph3.Serialize(ar);
	m_WPlrGraph4.Serialize(ar);
	m_IDlg.m_IterGraph.Serialize(ar);

	ar << m_XW1 << m_YW1 << m_XW2 << m_YW2;
	ar << m_XW3 << m_YW3 << m_XW4 << m_YW4;

	if(m_iView==5) m_iView=3;
	ar << m_iView << m_iWingView << m_iWPlrView;

	if      (m_pCurWingGraph==&m_WingGraph1) ar <<1;
	else if (m_pCurWingGraph==&m_WingGraph2) ar <<2;
	else if (m_pCurWingGraph==&m_WingGraph3) ar <<3;
	else if (m_pCurWingGraph==&m_WingGraph4) ar <<4;

	if      (m_pCurWPlrGraph==&m_WPlrGraph1) ar <<1;
	else if (m_pCurWPlrGraph==&m_WPlrGraph2) ar <<2;
	else if (m_pCurWPlrGraph==&m_WPlrGraph3) ar <<3;
	else if (m_pCurWPlrGraph==&m_WPlrGraph4) ar <<4;

	ar << m_WOppVar1 << m_WOppVar2 << m_WOppVar3 << m_WOppVar4 ;
	ar << m_Iter  << m_Relax << m_CvPrec;
	ar << m_NStation;

	ar << m_WngAnalysis.m_WakePanelFactor << m_WngAnalysis.m_TotalWakeLength << m_WngAnalysis.m_NXWakePanels;
	ar << m_MaxWakeIter     << m_WakeInterNodes << m_CoreSize;

	ar << m_MinPanelSize;

	if(m_bCurWOppOnly)      ar << 1; else ar<<0;
	if(m_bShowElliptic)     ar << 1; else ar<<0;
	if(m_bHalfWing)         ar << 1; else ar<<0;
	if(m_bAutoScales)       ar << 1; else ar<<0;
	if(m_bLogFile)          ar << 1; else ar<<0;

	if(m_bType1)            ar << 1; else ar<<0;
	if(m_bType2)            ar << 1; else ar<<0;
	if(m_bType4)            ar << 1; else ar<<0;

	if(m_bShowWing2)        ar << 1; else ar<<0;
	if(m_bShowStab)         ar << 1; else ar<<0;
	if(m_bShowFin)          ar << 1; else ar<<0;
	if(m_bResetWake)        ar << 1; else ar<<0;
	if(m_bDirichlet)        ar << 1; else ar<<0;
	if(m_bTrefftz)          ar << 1; else ar<<0;
	if(m_bWakePanels)       ar << 1; else ar<<0;

	if(m_bSurfaces)  ar << 1; else ar<<0;
	if(m_bOutline)   ar << 1; else ar<<0;
	if(m_bVLMPanels) ar << 1; else ar<<0;
	if(m_bAxes)      ar << 1; else ar<<0;
	if(m_bglLight)   ar << 1; else ar<<0;
	if(m_bXTop)	              ar << 1; else ar<<0;
	if(m_bXBot)               ar << 1; else ar<<0;
	if(m_bXCP)                ar << 1; else ar<<0;
	if(m_bMoments)            ar << 1; else ar<<0;
	if(m_bStream)             ar << 1; else ar<<0;
	if(m_bSpeeds)             ar << 1; else ar<<0;
	if(m_bICd)                ar << 1; else ar<<0;
	if(m_bVCd)                ar << 1; else ar<<0;
	if(m_bXCmRef)             ar << 1; else ar<<0;
	if(m_b3DVLMCl)            ar << 1; else ar<<0;
	if(m_b3DDownwash)         ar << 1; else ar<<0;
	if(m_bShowCpPoints)       ar << 1; else ar<<0;
	if(m_bAutoCpScale)        ar << 1; else ar<<0;

	ar << m_CpColor << m_CpStyle << m_CpWidth;
	ar << m_WngAnalysis.m_UnitType;
	ar << m_WingColor << m_StabColor << m_FinColor;

	ar << m_FlowLinesDlg.m_NX      << m_FlowLinesDlg.m_DeltaL;
	ar << m_FlowLinesDlg.m_XFactor << m_FlowLinesDlg.m_XOffset << m_FlowLinesDlg.m_ZOffset ;
	ar << m_FlowLinesDlg.m_pos;
	
	m_GLLightDlg.SaveSettings(ar);

	ar << pFrame->m_WOperDlgBar.m_Alpha0;
	ar << pFrame->m_WOperDlgBar.m_AlphaMax;
	ar << pFrame->m_WOperDlgBar.m_DeltaAlpha;
	ar << pFrame->m_WOperDlgBar.m_QInf0;
	ar << pFrame->m_WOperDlgBar.m_QInfMax;
	ar << pFrame->m_WOperDlgBar.m_DeltaQInf;
	ar << pFrame->m_WOperDlgBar.m_Control0;
	ar << pFrame->m_WOperDlgBar.m_ControlMax;
	ar << pFrame->m_WOperDlgBar.m_DeltaControl;

	ar << m_LegendMin << m_LegendMax;

	if(pFrame->m_WOperDlgBar.m_bSequence)   ar<<1; else ar <<0;
	if(m_bStoreWOpp)                        ar<<1; else ar <<0;
	if(m_bKeepOutOpps)                      ar<<1; else ar <<0;

	for (i=0; i<20; i++){
		ar << m_UFOdlg.m_ColSize[i];
	}

	if(m_BodyGridDlg.m_bScale)        ar << 1; else ar<<0;

	if(m_BodyGridDlg.m_bGrid)         ar << 1; else ar<<0;
	if(m_BodyGridDlg.m_bMinGrid)      ar << 1; else ar<<0;

	ar << m_BodyGridDlg.m_Color ;
	ar << m_BodyGridDlg.m_Style;
	ar << m_BodyGridDlg.m_Width ;
	ar << m_BodyGridDlg.m_Unit ;

	ar << m_BodyGridDlg.m_MinColor;
	ar << m_BodyGridDlg.m_MinStyle;
	ar << m_BodyGridDlg.m_MinWidth;
	ar << m_BodyGridDlg.m_MinorUnit;

//________________

	if(m_BodyGridDlg.m_bGrid2)         ar << 1; else ar<<0;
	if(m_BodyGridDlg.m_bMinGrid2)      ar << 1; else ar<<0;

	ar << m_BodyGridDlg.m_Color2;
	ar << m_BodyGridDlg.m_Style2 ;
	ar << m_BodyGridDlg.m_Width2 ;
	ar << m_BodyGridDlg.m_Unit2 ;

	ar << m_BodyGridDlg.m_MinColor2 ;
	ar << m_BodyGridDlg.m_MinStyle2 ;
	ar << m_BodyGridDlg.m_MinWidth2 ;
	ar << m_BodyGridDlg.m_MinorUnit2;

	ar << m_NHoopPoints;
	ar << m_NXPoints;
}


bool CMiarex::LoadSettings(CArchive &ar)
{
	//loads the Miarex settings from the archive ar
	CFile fp;
	int i,k;
	COLORREF c;
	CString str;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	try{
		//  we're reading/loading

		if(!m_WingGraph1.Serialize(ar)) return false;
		if(!m_WingGraph2.Serialize(ar)) return false;
		if(!m_WingGraph3.Serialize(ar)) return false;
		if(!m_WingGraph4.Serialize(ar)) return false;
		if(!m_WPlrGraph1.Serialize(ar)) return false;
		if(!m_WPlrGraph2.Serialize(ar)) return false;
		if(!m_WPlrGraph3.Serialize(ar)) return false;
		if(!m_WPlrGraph4.Serialize(ar)) return false;
		if(!m_IDlg.m_IterGraph.Serialize(ar)) return false;

		ar >> m_XW1 >> m_YW1 >> m_XW2 >> m_YW2;
		ar >> m_XW3 >> m_YW3 >> m_XW4 >> m_YW4;
		SetWGraphTitles(&m_WPlrGraph1, m_XW1,m_YW1);
		SetWGraphTitles(&m_WPlrGraph2, m_XW2,m_YW2);
		SetWGraphTitles(&m_WPlrGraph3, m_XW3,m_YW3);
		SetWGraphTitles(&m_WPlrGraph4, m_XW4,m_YW4);

		ar >> m_iView >> m_iWingView >> m_iWPlrView;
		if(m_iView <0 || m_iView>10 || m_iWingView<0|| m_iWingView>10|| m_iWPlrView<0|| m_iWPlrView>10){
			m_WOppVar1= 0;
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}

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

		ar >> m_WOppVar1;
		if(m_WOppVar1<0 || m_WOppVar1>20){
			m_WOppVar1= 0;
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		ar >> m_WOppVar2;
		if(m_WOppVar2<0 || m_WOppVar2>20){
			m_WOppVar2= 1;
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		ar >> m_WOppVar3;
		if(m_WOppVar3<0 || m_WOppVar3>20){
			m_WOppVar3= 2;
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		ar >> m_WOppVar4;
		if(m_WOppVar4<0 || m_WOppVar4>20){
			m_WOppVar4= 3;
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}

		ar >> m_Iter;
		if(m_Iter<0 || m_Iter>1000){
			m_Iter = 100;
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}

		ar >> m_Relax;
		if(m_Relax<0.5 || m_Relax>1000.0){
			m_Relax = 20.0;
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}

		ar >> m_CvPrec;
		if(m_CvPrec<=0.0 || m_CvPrec>1.0){
			m_CvPrec = 0.01;
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}

		ar >> k;
		if(k<0 || k>MAXSTATIONS){
			m_NStation = 20;
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}			
		m_NStation  = k;

		ar >> m_WngAnalysis.m_WakePanelFactor >>  m_WngAnalysis.m_TotalWakeLength >>  m_WngAnalysis.m_NXWakePanels;
		ar >> m_MaxWakeIter     >> m_WakeInterNodes >> m_CoreSize;
		if (m_WngAnalysis.m_WakePanelFactor<0.0 || m_WngAnalysis.m_TotalWakeLength<0.0 || m_WakeInterNodes<0 ||
			m_WngAnalysis.m_NXWakePanels <0  || m_MaxWakeIter <0 || m_CoreSize<0){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}

		ar >> m_MinPanelSize;
		if(m_MinPanelSize<0.0){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}			

		ar >>k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}			
		if(k) m_bCurWOppOnly = true; else m_bCurWOppOnly = false;


		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bShowElliptic = true; else m_bShowElliptic = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bHalfWing = true; else m_bHalfWing = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bAutoScales = true; else m_bAutoScales = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bLogFile = true; else m_bLogFile = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_bType1 = true; else m_bType1 = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_bType2 = true; else m_bType2 = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_bType4 = true; else m_bType4 = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_bShowWing2 = true; else m_bShowWing2= false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_bShowStab = true; else m_bShowStab= false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_bShowFin = true; else m_bShowFin = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_bResetWake = true; else m_bResetWake = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_bDirichlet = true; else m_bDirichlet = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_bTrefftz = true; else m_bTrefftz = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_bWakePanels = true; else m_bWakePanels = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bSurfaces = true; else m_bSurfaces = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bOutline = true; else m_bOutline = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bVLMPanels = true; else m_bVLMPanels = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bAxes = true; else m_bAxes = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bglLight = true; else m_bglLight = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bXTop = true; else m_bXTop = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bXBot = true; else m_bXBot = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bXCP = true; else m_bXCP = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bMoments = true; else m_bMoments = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bStream = true; else m_bStream = false;
		m_bStream  = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bSpeeds = true; else m_bSpeeds = false;
			m_bSpeeds  = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bICd = true; else m_bICd = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bVCd = true; else m_bVCd = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bXCmRef = true; else m_bXCmRef = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_b3DVLMCl = true; else m_b3DVLMCl = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_b3DDownwash = true; else m_b3DDownwash = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bShowCpPoints = true; else m_b3DDownwash = false;

		ar >> k;
		if(k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if(k) m_bAutoCpScale = true; else m_bAutoCpScale = false;


		ar >> m_CpColor ;
		if(m_CpColor<0 || m_CpColor>RGB(255,255,255)){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			m_CpColor = 1;
			throw pfe;
		}
		ar >> m_CpStyle ;
		if(m_CpStyle<0 || m_CpStyle>100){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			m_CpStyle = PS_SOLID;
			throw pfe;
		}
		ar >> m_CpWidth;
		if(m_CpWidth<0 || m_CpWidth>100){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			m_CpWidth = 1;
			throw pfe;
		}


		ar >> k;
		if(k<0 || k>2){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			m_WngAnalysis.m_UnitType = 1;
			throw pfe;
		}
		m_WngAnalysis.m_UnitType = k;

		ar >> c;
		if(c<0 || c>RGB(255,255,255)){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		m_WingColor = c;
		ar >> c;
		if(c<0 || c>RGB(255,255,255)){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		m_StabColor = c;
		ar >> c;
		if(c<0 || c>RGB(255,255,255)){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		m_FinColor = c;

		ar >> m_FlowLinesDlg.m_NX >> m_FlowLinesDlg.m_DeltaL >> m_FlowLinesDlg.m_XFactor;
		ar >> m_FlowLinesDlg.m_XOffset >> m_FlowLinesDlg.m_ZOffset;
		ar >> k;
		if(k<0 || k>=10){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		m_FlowLinesDlg.m_pos = k;

		m_FlowLinesDlg.SetParams();

		if(!m_GLLightDlg.LoadSettings(ar)){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		ar >> pFrame->m_WOperDlgBar.m_Alpha0;
		ar >> pFrame->m_WOperDlgBar.m_AlphaMax;
		ar >> pFrame->m_WOperDlgBar.m_DeltaAlpha;
		ar >> pFrame->m_WOperDlgBar.m_QInf0;
		ar >> pFrame->m_WOperDlgBar.m_QInfMax;
		ar >> pFrame->m_WOperDlgBar.m_DeltaQInf;
		ar >> pFrame->m_WOperDlgBar.m_Control0;
		ar >> pFrame->m_WOperDlgBar.m_ControlMax;
		ar >> pFrame->m_WOperDlgBar.m_DeltaControl;

		ar >> m_LegendMin >> m_LegendMax;

		ar>>k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) pFrame->m_WOperDlgBar.m_bSequence = true; else pFrame->m_WOperDlgBar.m_bSequence = false;


		ar>>k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_bStoreWOpp = true; else m_bStoreWOpp = false;

		ar>>k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_bKeepOutOpps = true; else m_bKeepOutOpps = false;

		for (i=0; i<20; i++){
			ar >> m_UFOdlg.m_ColSize[i];
		}
//__________________________________________________________________________________
		ar>>k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_BodyGridDlg.m_bScale = true; else m_BodyGridDlg.m_bScale = false;

		ar>>k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_BodyGridDlg.m_bGrid = true; else m_BodyGridDlg.m_bGrid = false;

		ar>>k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_BodyGridDlg.m_bMinGrid = true; else m_BodyGridDlg.m_bMinGrid = false;


		ar >> m_BodyGridDlg.m_Color ;
		ar >> m_BodyGridDlg.m_Style;
		ar >> m_BodyGridDlg.m_Width;
		ar >> m_BodyGridDlg.m_Unit;

		ar >> m_BodyGridDlg.m_MinColor ;
		ar >> m_BodyGridDlg.m_MinStyle  ;
		ar >> m_BodyGridDlg.m_MinWidth ;
		ar >> m_BodyGridDlg.m_MinorUnit ;

//__________

		ar>>k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_BodyGridDlg.m_bGrid2 = true; else m_BodyGridDlg.m_bGrid2 = false;

		ar>>k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_BodyGridDlg.m_bMinGrid2 = true; else m_BodyGridDlg.m_bMinGrid2 = false;

	
		ar >> m_BodyGridDlg.m_Color2;
		ar >> m_BodyGridDlg.m_Style2;
		ar >> m_BodyGridDlg.m_Width2;
		ar >> m_BodyGridDlg.m_Unit2;

		ar >> m_BodyGridDlg.m_MinColor2;
		ar >> m_BodyGridDlg.m_MinStyle2;
		ar >> m_BodyGridDlg.m_MinWidth2;
		ar >> m_BodyGridDlg.m_MinorUnit2;

		ar >> m_NHoopPoints;
		ar >> m_NXPoints;

		pFrame->m_WOperDlgBar.SetParams(NULL);
		m_FlowLinesDlg.SetUnits();

		return true;

//-------------
		
	}
	catch (CArchiveException *ex){
		ex->Delete();
		return false;
	}
	catch (CException *ex){
		ex->Delete();
		return false;
	}
}

void CMiarex::CheckMenus()
{
	//Updates the menu checks i.a.w. the various flags
	CMenu* pMenu;
	pMenu = m_pFrame->GetMenu()->GetSubMenu(2);
	if(m_bAutoScales) pMenu->CheckMenuItem(IDM_AUTOWINGSCALES, MF_BYCOMMAND | MF_CHECKED);
	else              pMenu->CheckMenuItem(IDM_AUTOWINGSCALES, MF_BYCOMMAND | MF_UNCHECKED);

	if(m_iView==3)
	{
		pMenu->CheckMenuItem(IDM_WOPP,      MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_SHOWPOLAR, MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_3DVIEW,    MF_BYCOMMAND | MF_CHECKED);
		pMenu->CheckMenuItem(IDM_CPVIEW,    MF_BYCOMMAND | MF_UNCHECKED);
	}
	else if(m_iView==2)
	{
		pMenu->CheckMenuItem(IDM_3DVIEW,    MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_WOPP,      MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_SHOWPOLAR, MF_BYCOMMAND | MF_CHECKED);
		pMenu->CheckMenuItem(IDM_CPVIEW,    MF_BYCOMMAND | MF_UNCHECKED);
	}

	else if(m_iView==1)
	{
		pMenu->CheckMenuItem(IDM_3DVIEW,    MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_WOPP,      MF_BYCOMMAND | MF_CHECKED);
		pMenu->CheckMenuItem(IDM_SHOWPOLAR, MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_CPVIEW,    MF_BYCOMMAND | MF_UNCHECKED);
	}

	else if(m_iView==4)
	{
		pMenu->CheckMenuItem(IDM_3DVIEW,    MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_WOPP,      MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_SHOWPOLAR, MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_CPVIEW,    MF_BYCOMMAND | MF_CHECKED);
	}

	pMenu = m_pFrame->GetMenu()->GetSubMenu(6);
	if(m_bShowWing2)	pMenu->CheckMenuItem(IDM_SHOWWING2, MF_BYCOMMAND | MF_CHECKED);
	else				pMenu->CheckMenuItem(IDM_SHOWWING2, MF_BYCOMMAND | MF_UNCHECKED);
	if(m_bShowStab)		pMenu->CheckMenuItem(IDM_SHOWSTAB, MF_BYCOMMAND | MF_CHECKED);
	else				pMenu->CheckMenuItem(IDM_SHOWSTAB, MF_BYCOMMAND | MF_UNCHECKED);
	if(m_bShowFin)		pMenu->CheckMenuItem(IDM_SHOWFIN, MF_BYCOMMAND | MF_CHECKED);
	else				pMenu->CheckMenuItem(IDM_SHOWFIN, MF_BYCOMMAND | MF_UNCHECKED);
	if(m_bShowElliptic)	pMenu->CheckMenuItem(IDM_SHOWELLIPTIC, MF_BYCOMMAND | MF_CHECKED);
	else				pMenu->CheckMenuItem(IDM_SHOWELLIPTIC, MF_BYCOMMAND | MF_UNCHECKED);

	if(m_iView==1)
	{
		if (m_iWingView==1){
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH1, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH2, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH3, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH4, MF_BYCOMMAND | MF_UNCHECKED);
			if(m_pCurGraph==&m_WingGraph1)
				pMenu->CheckMenuItem(IDM_SINGLEGRAPH1, MF_BYCOMMAND | MF_CHECKED);
			if(m_pCurGraph==&m_WingGraph2) 
				pMenu->CheckMenuItem(IDM_SINGLEGRAPH2, MF_BYCOMMAND | MF_CHECKED);
			if(m_pCurGraph==&m_WingGraph3) 
				pMenu->CheckMenuItem(IDM_SINGLEGRAPH3, MF_BYCOMMAND | MF_CHECKED);
			if(m_pCurGraph==&m_WingGraph4) 
				pMenu->CheckMenuItem(IDM_SINGLEGRAPH4, MF_BYCOMMAND | MF_CHECKED);
			pMenu->CheckMenuItem(IDM_TWOWINGGRAPHS,   MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_FOURWINGGRAPHS,  MF_BYCOMMAND | MF_UNCHECKED);
		}
		else if (m_iWingView==2){
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH1, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH2, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH3, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH4, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_TWOWINGGRAPHS,   MF_BYCOMMAND | MF_CHECKED);
			pMenu->CheckMenuItem(IDM_FOURWINGGRAPHS,  MF_BYCOMMAND | MF_UNCHECKED);
		}
		else if (m_iWingView==4){
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH1, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH2, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH3, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH4, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_TWOWINGGRAPHS,   MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_FOURWINGGRAPHS,  MF_BYCOMMAND | MF_CHECKED);
		}
	}
	else if(m_iView==2){
		pMenu = m_pFrame->GetMenu()->GetSubMenu(5);
		if (m_iWPlrView==1){
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH1, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH2, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH3, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH4, MF_BYCOMMAND | MF_UNCHECKED);
			if(m_pCurGraph==&m_WPlrGraph1)
				pMenu->CheckMenuItem(IDM_SINGLEGRAPH1, MF_BYCOMMAND | MF_CHECKED);
			if(m_pCurGraph==&m_WPlrGraph2) 
				pMenu->CheckMenuItem(IDM_SINGLEGRAPH2, MF_BYCOMMAND | MF_CHECKED);
			if(m_pCurGraph==&m_WPlrGraph3) 
				pMenu->CheckMenuItem(IDM_SINGLEGRAPH3, MF_BYCOMMAND | MF_CHECKED);
			if(m_pCurGraph==&m_WPlrGraph4) 
				pMenu->CheckMenuItem(IDM_SINGLEGRAPH4, MF_BYCOMMAND | MF_CHECKED);
			
			pMenu->CheckMenuItem(IDM_TWOGRAPHS,   MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_FOURGRAPHS,  MF_BYCOMMAND | MF_UNCHECKED);
		}
		else if (m_iWPlrView==2)
		{
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH1, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH2, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH3, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH4, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_TWOGRAPHS,   MF_BYCOMMAND | MF_CHECKED);
			pMenu->CheckMenuItem(IDM_FOURGRAPHS,  MF_BYCOMMAND | MF_UNCHECKED);
		}
		else if (m_iWPlrView==4)
		{
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH1, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH2, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH3, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH4, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_TWOGRAPHS,   MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_FOURGRAPHS,  MF_BYCOMMAND | MF_CHECKED);
		}
	}
	else if(m_iView==5)
	{
		pMenu = m_pFrame->GetMenu()->GetSubMenu(3);
		if(m_bCurFrameOnly) pMenu->CheckMenuItem(IDM_SHOWONLYACTIVEFRAME,  MF_BYCOMMAND | MF_CHECKED);
		else                pMenu->CheckMenuItem(IDM_SHOWONLYACTIVEFRAME,  MF_BYCOMMAND | MF_UNCHECKED);
	}
}


void CMiarex::OnResetDef() 
{
	//resets the graphs default settings
	if(!m_pCurGraph) return;	
	m_pCurGraph->SetDefaults();
	UpdateView();
}


void CMiarex::OnResetScales() 
{
	//resets the scale of the current graph
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
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
		double halfspan = m_pCurWing->m_Span/2.0;
		if(m_bHalfWing) m_pCurGraph->SetXMin(0.0);
		else            m_pCurGraph->SetXMin(-halfspan*pFrame->m_mtoUnit);
		m_pCurGraph->SetXMax( halfspan*pFrame->m_mtoUnit);
	}
	UpdateView();	
}

void CMiarex::OnListPlrs() 
{
	//list the polars associated to the foils
	CListPlrDlg LPDlg(m_pFrame);
	LPDlg.DoModal();
}

void CMiarex::OnDefineVar() 
{
	//define the variables for the current graph
	CUserVarDlg UVDlg(this);
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(m_pCurGraph == &m_WPlrGraph1)
	{
		UVDlg.m_XSel = m_XW1;
		UVDlg.m_YSel = m_YW1;
		if(IDOK == UVDlg.DoModal())
		{
			m_XW1 = UVDlg.m_XSel;
			m_YW1 = UVDlg.m_YSel;
			SetWGraphTitles(&m_WPlrGraph1,m_XW1, m_YW1);
			m_WPlrGraph1.SetAuto(true);
			m_WPlrGraph1.SetAutoYMinUnit(true);
		}
	}
	else if(m_pCurGraph == &m_WPlrGraph2)
	{
		UVDlg.m_XSel = m_XW2;
		UVDlg.m_YSel = m_YW2;
		if(IDOK == UVDlg.DoModal()){
			m_XW2 = UVDlg.m_XSel;
			m_YW2 = UVDlg.m_YSel;
			SetWGraphTitles(&m_WPlrGraph2,m_XW2, m_YW2);
			m_WPlrGraph2.SetAuto(true);
			m_WPlrGraph2.SetAutoYMinUnit(true);
		}
	}
	else if(m_pCurGraph == &m_WPlrGraph3)
	{
		UVDlg.m_XSel = m_XW3;
		UVDlg.m_YSel = m_YW3;
		if(IDOK == UVDlg.DoModal()){
			m_XW3 = UVDlg.m_XSel;
			m_YW3 = UVDlg.m_YSel;
			SetWGraphTitles(&m_WPlrGraph3,m_XW3, m_YW3);
			m_WPlrGraph3.SetAuto(true);
			m_WPlrGraph3.SetAutoYMinUnit(true);
		}
	}
	else if(m_pCurGraph == &m_WPlrGraph4){
		UVDlg.m_XSel = m_XW4;
		UVDlg.m_YSel = m_YW4;
		if(IDOK == UVDlg.DoModal())
		{
			m_XW4 = UVDlg.m_XSel;
			m_YW4 = UVDlg.m_YSel;
			SetWGraphTitles(&m_WPlrGraph4,m_XW4, m_YW4);
			m_WPlrGraph4.SetAuto(true);
			m_WPlrGraph4.SetAutoYMinUnit(true);
		}
	}
	CreateWPolarCurves();
	UpdateView();
}


void CMiarex::CreateWPolarCurves()
{
	//resets and creates the WPolar graphs curves
	CWPolar *pWPolar;
	
	m_WPlrGraph1.DeleteCurves();
	m_WPlrGraph2.DeleteCurves();
	m_WPlrGraph3.DeleteCurves();
	m_WPlrGraph4.DeleteCurves();

	for (int k=0; k<m_poaWPolar->GetSize(); k++){
		pWPolar = (CWPolar*)m_poaWPolar->GetAt(k);

		if (pWPolar->m_bIsVisible && pWPolar->m_Alpha.GetSize()>0 &&
			(m_bType1 && pWPolar->m_Type == 1 ||
			 m_bType2 && pWPolar->m_Type == 2 ||
			 m_bType4 && pWPolar->m_Type == 4 ||
			 pWPolar->m_Type==5 )){

			CCurve* pWPolarCurve = m_WPlrGraph1.AddCurve();
			CCurve* pWCzCurve    = m_WPlrGraph2.AddCurve();
			CCurve* pWCmCurve    = m_WPlrGraph3.AddCurve();
			CCurve* pWLDCurve    = m_WPlrGraph4.AddCurve();

			if(pWPolar->m_bShowPoints){
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


void CMiarex::FillWPlrCurve(CCurve *pCurve, CWPolar *pWPolar, int XVar, int YVar)
{
	//The Wpolar curve object has been created
	//Fill it with the variable data specified by XVar and YVar
	bool bAdd;
	int i;	
	double x,y;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CArray <double, double> *pX;
	CArray <double, double> *pY;
	pX = (CArray <double, double> *) GetUFOPlrVariable(pWPolar, XVar);
	pY = (CArray <double, double> *) GetUFOPlrVariable(pWPolar, YVar);

	for (i=0; i<pWPolar->m_Alpha.GetSize(); i++)
	{
		bAdd = true;
		x = (*pX)[i];
		y = (*pY)[i];

		if((XVar==12 || XVar==15 || XVar==16) && x<0) bAdd = false;
		if((YVar==12 || YVar==15 || YVar==16) && y<0) bAdd = false;

		if(XVar==13 || XVar==14)              x *= pFrame->m_NtoUnit; //force
		if(YVar==13 || YVar==14)              y *= pFrame->m_NtoUnit; //force
		if(XVar==15 || XVar==16 || XVar==17)  x *= pFrame->m_mstoUnit;//speed
		if(YVar==15 || YVar==16 || YVar==17)  y *= pFrame->m_mstoUnit;//speed

		if(XVar==19 || XVar==20 || XVar==21)  x *= pFrame->m_NmtoUnit;//moment
		if(YVar==19 || YVar==20 || YVar==21)  y *= pFrame->m_NmtoUnit;//moment

		if(XVar==22 || XVar==23)              x *= pFrame->m_mtoUnit;//force
		if(YVar==22 || YVar==23)              y *= pFrame->m_mtoUnit;//force

		if(XVar==24)                          x *= pFrame->m_NmtoUnit;//moment
		if(YVar==24)                          y *= pFrame->m_NmtoUnit;//moment

		if(bAdd) pCurve->AddPoint(x,y);
	}
}

void CMiarex::OnLButtonDblClk(UINT nFlags, CPoint point)
{
//	point is in client coordinates
	m_pCurGraph = GetGraph(point);

	if (m_pCurGraph && m_pCurGraph->IsInDrawRect(point)){
		CGraphDlg dlg(m_pChildWnd);

		dlg.m_pGraph = m_pCurGraph;

		if(IDOK == dlg.DoModal()){
			UpdateView();
		}
	}	
}
	
void CMiarex::OnLButtonDown(UINT nFlags, CPoint point) 
{
//	point is in client coordinates
	int iF;
	CVector Real;
	ClientToGL(point, Real);
	m_pCurGraph = GetGraph(point);
	if(m_rCltRect.PtInRect(point)) m_pChildWnd->SetFocus();
 
	if(m_iView==3) 
	{
		if(m_bPickCenter)
		{
			Set3DRotationCenter(point);
			m_bPickCenter = false;
			CW3DBar* pW3DBar = (CW3DBar*)m_pW3DBar;
			pW3DBar->m_ctrlPickCenter.SetCheck(0);
		}
		else
		{
			m_ArcBall.Start(point.x, m_rCltRect.Height()-point.y);
			m_bCrossPoint = true;
			Set3DRotationCenter();

			SHORT sh1  = GetKeyState(VK_LCONTROL);
			SHORT sh2  = GetKeyState(VK_RCONTROL);
			if (!(sh1 & 0x8000)&& !(sh2 & 0x8000)) 
			{
				m_bTrans = true;
				SetCursor(m_hcMove);
			}
		}

		UpdateView();
	}
	else if(m_iView==5) 
	{
		if(m_bPickCenter)
		{
			Set3DRotationCenter(point);
			m_bPickCenter = false;
			CW3DBar* pW3DBar = (CW3DBar*)m_pW3DBar;
			pW3DBar->m_ctrlPickCenter.SetCheck(0);
		}
		else
		{
			m_bTrans=true;

			if(m_pCurBody && m_BodyRect.PtInRect(point))
			{
				m_ArcBall.Start(point.x, m_rCltRect.Height()-point.y);
				m_bCrossPoint = true;
				Set3DRotationCenter();

				SHORT sh1  = GetKeyState(VK_LCONTROL);
				SHORT sh2  = GetKeyState(VK_RCONTROL);
				if (!(sh1 & 0x8000)&& !(sh2 & 0x8000)) 
				{
					m_bTrans = true;
					SetCursor(m_hcMove);
				}

				UpdateView();
			}
			if(m_pCurBody && m_BodyLineRect.PtInRect(point))
			{
				Real.x =  (Real.x - m_BodyOffset.x)/m_BodyScale;
				Real.y =  (Real.y - m_BodyOffset.y)/m_BodyScale;
				Real.z = 0.0;
				iF = m_pCurBody->IsFramePos(Real, m_BodyScale);
				if(iF >=0) 
				{
					m_pBodyCtrlBar->TakePicture();
					m_pBodyCtrlBar->StorePicture();
					m_pCurBody->m_iActiveFrame = iF;
					m_pCurFrame = m_pCurBody->GetFrame(m_pCurBody->m_iActiveFrame);

					m_pBodyCtrlBar->SetFrame(m_pCurBody->m_iActiveFrame);
					m_bTrans = false;
					m_bDragPoint  = true;
					UpdateView();
				}
			}
			else if(m_pCurFrame && m_FrameRect.PtInRect(point))
			{
				Real.x =  (Real.x - m_FrameOffset.x)/m_FrameScale;
				Real.y =  (Real.y - m_FrameOffset.y)/m_FrameScale;
				Real.z = 0.0;
				m_pCurFrame->m_iSelect = m_pCurFrame->IsPoint(Real, m_FrameScale);
				if(m_pCurFrame->m_iSelect >=0) 
				{
					m_pBodyCtrlBar->TakePicture();
					m_pBodyCtrlBar->StorePicture();
					m_bTrans = false;
					m_bDragPoint  = true;
					m_pBodyCtrlBar->SetPointSel(m_pCurFrame->m_iSelect);
				}
			}
			if(m_bTrans) SetCursor(m_hcMove);		
		}
	}
	else if (m_pCurWing || (m_pCurGraph && m_pCurGraph->IsInDrawRect(point)))
	{
		m_LastPoint.x = point.x;
		m_LastPoint.y = point.y;

		m_hcMove = AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_HMOVE));
		m_bTrans = true;
		SetCursor(m_hcMove);
		m_bTransGraph = true;
		if(m_pCurGraph && m_pCurGraph->IsInDrawRect(point))
			m_bTransGraph = true;		
		else 
			m_bTransGraph = false;
	}
	m_bPickCenter = false;
	m_PointDown = point;
}


void CMiarex::OnReset3DView() 
{
	//Reset the 3D OpenGL view
	CW3DBar* pW3DBar = (CW3DBar*)m_pW3DBar;
	pW3DBar->On3DReset();
}

void CMiarex::OnMouseMove(UINT nFlags, CPoint point) 
{
	//The mouse has moved
	//Process the message
 
	CW3DBar* pW3DBar = (CW3DBar*)m_pW3DBar;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CVector Real;
	CPoint Delta(point.x - m_LastPoint.x,point.y - m_LastPoint.y);
	ClientToGL(point, Real);

	if(GetFocus()!=m_pChildWnd) m_pChildWnd->SetFocus();

	int n;
	bool bCtrl = false;
	SHORT shX  = GetKeyState('X');
	SHORT shY  = GetKeyState('Y');
	SHORT shZ  = GetKeyState('Z');
	SHORT sh1  = GetKeyState(VK_LCONTROL);
	SHORT sh2  = GetKeyState(VK_RCONTROL);

	if ((sh1 & 0x8000)||(sh2 & 0x8000)) bCtrl =true;

	if ((nFlags & MK_LBUTTON) && m_bTrans && (m_iView==1 || m_iView==2 || m_iView==4))
	{
		m_pCurGraph = GetGraph(point);
		if(m_pCurGraph && m_bTransGraph)
		{ 
			// we translate the curves inside the graph
			double xu, yu;
			m_pCurGraph->SetAuto(false);
			double x1 =  m_pCurGraph->ClientTox(m_LastPoint.x) ;
			double y1 =  m_pCurGraph->ClientToy(m_LastPoint.y) ;
			
			xu = m_pCurGraph->ClientTox(point.x);
			yu = m_pCurGraph->ClientToy(point.y);

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
			
			m_ptOffset.x += Delta.x;
			m_ptOffset.y += Delta.y;
			UpdateView();

		}
		else if (m_pCurWing  && m_iView!=2 && m_iWingView !=1)
		{
			// we translate the legend
			//horizontally only
			m_WingLegendOffset.x += Delta.x;
			UpdateView();
		}
		else if (m_iView==2)
		{
		
			// we translate the legend
			if(m_iWPlrView==1 || m_iWPlrView==4)
			{
				m_WPlrLegendOffset.y += Delta.y;
				UpdateView();
			}
			else if(m_iWPlrView==2)
			{
				m_WPlrLegendOffset.x += Delta.x;
				UpdateView();
			}
		}
	}
	else if (nFlags & MK_LBUTTON && m_iView==3)
	{
		if(bCtrl)//rotate
		{
			m_ArcBall.Move(point.x, m_rCltRect.Height()-point.y);

			UpdateView();

			//flush the event queue

			MSG msg;
			while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
				GetMessage(&msg, NULL, 0, 0);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else if(m_bTrans)
		{	
			//translate
			m_glViewportTrans.x += (GLfloat)(Delta.x*2.0*m_GLScale/m_glScaled/m_rCltRect.Width());
			m_glViewportTrans.y += (GLfloat)(Delta.y*2.0*m_GLScale/m_glScaled/m_rCltRect.Width());

			m_glRotCenter.x = MatOut[0][0]*m_glViewportTrans.x + MatOut[0][1]*(-m_glViewportTrans.y) + MatOut[0][2]*m_glViewportTrans.z;
			m_glRotCenter.y = MatOut[1][0]*m_glViewportTrans.x + MatOut[1][1]*(-m_glViewportTrans.y) + MatOut[1][2]*m_glViewportTrans.z;
			m_glRotCenter.z = MatOut[2][0]*m_glViewportTrans.x + MatOut[2][1]*(-m_glViewportTrans.y) + MatOut[2][2]*m_glViewportTrans.z;

			UpdateView();
		}
	}
	else if (nFlags & MK_LBUTTON && m_iView==5 )
	{
		if(bCtrl&& m_BodyRect.PtInRect(point))
		{
			//rotate
			m_ArcBall.Move(point.x, m_rCltRect.Height()-point.y);
			UpdateView();

			//flush the event queue
			MSG msg;
			while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
				GetMessage(&msg, NULL, 0, 0);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else if(m_bTrans)
		{
			//translate
			if(m_BodyRect.PtInRect(point))
			{
				m_glViewportTrans.x += (GLfloat)(Delta.x*2.0*m_GLScale/m_glScaled/m_rCltRect.Width());
				m_glViewportTrans.y += (GLfloat)(Delta.y*2.0*m_GLScale/m_glScaled/m_rCltRect.Width());

				m_glRotCenter.x = MatOut[0][0]*(m_glViewportTrans.x) + MatOut[0][1]*(-m_glViewportTrans.y) + MatOut[0][2]*m_glViewportTrans.z;
				m_glRotCenter.y = MatOut[1][0]*(m_glViewportTrans.x) + MatOut[1][1]*(-m_glViewportTrans.y) + MatOut[1][2]*m_glViewportTrans.z;
				m_glRotCenter.z = MatOut[2][0]*(m_glViewportTrans.x) + MatOut[2][1]*(-m_glViewportTrans.y) + MatOut[2][2]*m_glViewportTrans.z;

				UpdateView();
			}
			else if (m_BodyLineRect.PtInRect(point) && m_pCurBody)
			{
				m_BodyOffset.x  +=  (double)Delta.x*2.0/(double)m_rCltRect.Width() * m_GLScale;
				m_BodyOffset.y  += -(double)Delta.y*2.0/(double)m_rCltRect.Width() * m_GLScale;
				UpdateView();
			}
			else if(m_FrameRect.PtInRect(point) && m_pCurFrame)
			{
				m_FrameOffset.x +=  (double)Delta.x*2.0/(double)m_rCltRect.Width() * m_GLScale;
				m_FrameOffset.y += -(double)Delta.y*2.0/(double)m_rCltRect.Width() * m_GLScale;
				UpdateView();
			}
		}
		else if (m_BodyLineRect.PtInRect(point) && m_pCurBody)
		{
			Real.x =  (Real.x - m_BodyOffset.x)/m_BodyScale;
			Real.y =  (Real.y - m_BodyOffset.y)/m_BodyScale;
			Real.z = 0.0;

			int n = m_pCurBody->m_iActiveFrame;
			if (n>=0 && n<=m_pCurBody->m_NStations && !m_bTrans && m_bDragPoint) 
			{	
				if(!m_pCurBody->m_bLocked)
				{
					//dragging a point
					m_pCurFrame = m_pCurBody->m_Frame + m_pCurBody->m_iActiveFrame;
					m_pCurBody->m_FramePosition[n].x = Real.x;
					m_pCurBody->m_FramePosition[n].z = Real.y;
					m_pCurBody->UpdateFramePos(n);
					m_bTrans = false;
					pFrame->SetSaveState(false);
					m_bResetglBody2D = true;
				}
			}

			UpdateView();
		}
		else if(m_FrameRect.PtInRect(point) && m_pCurFrame)
		{
			Real.x =  (Real.x - m_FrameOffset.x)/m_FrameScale;
			Real.y =  (Real.y - m_FrameOffset.y)/m_FrameScale;
			Real.z =   0.0;

			if(m_pCurFrame)	n = m_pCurFrame->m_iSelect;
			else			n = -10;
			if (n>0 && n<m_pCurFrame->m_NPoints-1 && !m_bTrans && m_bDragPoint) 
			{	//dragging a point
				if(!m_pCurBody->m_bLocked)
				{
					
					if(Real.x<0.0) 	m_pCurFrame->m_Point[n].y = 0.0;
					else            m_pCurFrame->m_Point[n].y = Real.x;
					m_pCurFrame->m_Point[n].z = Real.y;
					m_pCurBody->ComputeCenterLine();
					m_bTrans = false;
					pFrame->SetSaveState(false);
					m_bResetglBody2D = true;
				}
			}
			else if ((n==0 || n==m_pCurFrame->m_NPoints-1)  && !m_bTrans && m_bDragPoint) 
			{	
				if(!m_pCurBody->m_bLocked)
				{
					//dragging a point
					m_pCurFrame->m_Point[n].y = 0.0;
					m_pCurFrame->m_Point[n].z = Real.y;
					m_pCurBody->ComputeCenterLine();
					m_bTrans = false;
					pFrame->SetSaveState(false);
					m_bResetglBody2D = true;
				}
			}

			UpdateView();
		}
	}
	else if (((nFlags & MK_MBUTTON) || (shZ & 0x8000)) && !bCtrl) 
	{
		// we zoom the graph or the wing	
		if(m_iView ==1 ||m_iView==2 || m_iView==4)
		{
			m_pCurGraph = GetGraph(point);
			if(m_pCurGraph && m_pCurGraph->IsInDrawRect(point))
			{ 
				//zoom graph

				if (shX & 0x8000){
					//zoom x scale
					m_pCurGraph->SetAutoX(false);
					m_pCurGraph->SetAutoX(false);
					if(point.x-m_LastPoint.x<0) m_pCurGraph->Scalex(1.04);
					else						m_pCurGraph->Scalex(1.0/1.04);
				}
				else if(shY & 0x8000){
					//zoom y scale
					m_pCurGraph->SetAutoY(false);
					m_pCurGraph->SetAutoY(false);
					if(point.y-m_LastPoint.y<0) m_pCurGraph->Scaley(1.04);
					else						m_pCurGraph->Scaley(1.0/1.04);
				}
				else{
					//zoom both
					m_pCurGraph->SetAuto(false);
					if(point.y-m_LastPoint.y<0) m_pCurGraph->Scale(1.06);
					else						m_pCurGraph->Scale(1.0/1.06);
				}
				UpdateView();
			}
			else if(m_pCurWing && m_iView!=2)
			{
				//zoom wing
				if(point.y-m_LastPoint.y<0) m_WingScale /=1.04;
				else                        m_WingScale *= 1.04;
				UpdateView();
			}
		}
		else if (m_iView==5)
		{
			if(m_BodyLineRect.PtInRect(point))
			{
				if(point.y-m_LastPoint.y<0) m_BodyScale /= 1.04;
				else                        m_BodyScale *= 1.04;
				m_bResetglBody2D = true;
				UpdateView();
			}
			else if(m_FrameRect.PtInRect(point))
			{
				if(point.y-m_LastPoint.y<0) m_FrameScale /= 1.04;
				else                        m_FrameScale *= 1.04;
				m_bResetglBody2D = true;
				UpdateView();
			}
			else if(m_pCurBody && m_BodyRect.PtInRect(point))
			{	//zoom 3D Body
				if(point.y-m_LastPoint.y>0) 
					m_glScaled *= (GLfloat)1.04;
				else	
					m_glScaled /= (GLfloat)1.04;

				UpdateView();
			}
		}
		else if(m_iView==3)
		{
			if(m_pCurWing )
			{	//zoom 3D wing
				if(point.y-m_LastPoint.y>0) 
					m_glScaled *= (GLfloat)1.06;
				else	
					m_glScaled /= (GLfloat)1.06;

				UpdateView();
			}
		}
	}
	else if(m_iView==5)
	{	
		if (m_BodyLineRect.PtInRect(point) && m_pCurBody)
		{
			Real.x =  (Real.x - m_BodyOffset.x)/m_BodyScale;
			Real.y =  (Real.y - m_BodyOffset.y)/m_BodyScale;
			Real.z = 0.0;
			int n = m_pCurBody->IsFramePos(Real, m_BodyScale);
			m_pCurBody->m_iHighlight = -10;
			if (n>=0 && n<=m_pCurBody->m_NStations) 
			{
				m_pCurBody->m_iHighlight = n;
			}
			m_bResetglBodyPoints = true;
			UpdateView();
		}
		else if (m_pCurFrame && m_FrameRect.PtInRect(point))
		{
			Real.x =  (Real.x - m_FrameOffset.x)/m_FrameScale;
			Real.y =  (Real.y - m_FrameOffset.y)/m_FrameScale;
			Real.z = 0.0;
			int n = m_pCurFrame->IsPoint(Real, m_FrameScale);
			m_pCurFrame->m_iHighlight = -10;
			if (n>=0 && n<=m_pCurFrame->m_NPoints) 
			{
				m_pCurFrame->m_iHighlight = n;
			}
			m_bResetglBodyPoints = true;
			UpdateView();
		}
	}
	m_LastPoint = point;
	
//	CWnd::OnMouseMove(nFlags, point);
}


BOOL CMiarex::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	//The mouse button has been wheeled
	//Process the message
//	point is in client coordinates
	if(m_iView ==1 || m_iView ==2 ){
		m_pCurGraph = GetGraph(pt);
		if(m_pCurGraph && m_pCurGraph->IsInDrawRect(pt)){

			SHORT sh1 = GetKeyState('X');
			SHORT sh2 = GetKeyState('Y');

			if (sh1 & 0x8000){
				//zoom x scale
				m_pCurGraph->SetAutoX(false);
				if(zDelta>0) m_pCurGraph->Scalex(1.06);
				else m_pCurGraph->Scalex(1.0/1.06);
			}
			else if(sh2 & 0x8000){
				//zoom y scale
				m_pCurGraph->SetAutoY(false);
				if(zDelta>0) m_pCurGraph->Scaley(1.06);
				else m_pCurGraph->Scaley(1.0/1.06);
			}
			else{
				//zoom both
				m_pCurGraph->SetAuto(false);
				if(zDelta>0) m_pCurGraph->Scale(1.06);
				else m_pCurGraph->Scale(1.0/1.06);
			}
			UpdateView();
		
		}
		else if(m_pCurWing && m_iView!=2)
		{
			if(zDelta<0) m_WingScale *= 1.06;
			else         m_WingScale /= 1.06;
			UpdateView();
		}
	}
	else if(m_iView==3)
	{
		if(m_pCurWing){//zoom wing
			
			if(zDelta<0) m_glScaled *=(GLfloat)1.06;
			else         m_glScaled /= (GLfloat)1.06;
			
			UpdateView();
		}
	}
	else if (m_iView==5)
	{
		if(m_BodyRect.PtInRect(pt))
		{
			if(zDelta<0) m_glScaled *=(GLfloat)1.06;
			else         m_glScaled /= (GLfloat)1.06;
		}
		else if(m_BodyLineRect.PtInRect(pt))
		{
			if(zDelta<0) m_BodyScale *= 1.06;
			else         m_BodyScale /= 1.06;
			m_bResetglBody2D=true;
		}
		else if(m_FrameRect.PtInRect(pt))
		{
			if(zDelta<0) m_FrameScale *= 1.06;
			else         m_FrameScale /= 1.06;
			m_bResetglBody2D=true;
		}
		UpdateView();
	}

	return TRUE;
	
//	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}



void CMiarex::PrintAll(CDC *pDC, CRect *pCltRect)
{
	//Print whatever is on the screen
	CChildView *pChildView = (CChildView*)m_pChildWnd;
	pDC->SetMapMode(MM_HIMETRIC);
	if(GetWindowBk() && !IsBlackAndWhite()){
		pDC->SetBkMode(OPAQUE);

		CPen WhitePen;
		WhitePen.CreateStockObject(WHITE_PEN);
		CPen *pOldPen = pDC->SelectObject(&WhitePen);

		CRect BkRect(pCltRect);
		BkRect.top = -BkRect.top;
		BkRect.bottom = -BkRect.bottom;
		CBrush BkBrush(pChildView->m_crBackColor);
		CBrush *pOldBrush = pDC->SelectObject(&BkBrush);
		pDC->Rectangle(&BkRect);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
		WhitePen.DeleteObject();
		BkBrush.DeleteObject();
	}
	CPoint Offset;

	pDC->SetBkMode(TRANSPARENT);

	CWOpp * pWOpp = m_pCurWOpp;
	double alpha;
	if(pWOpp) alpha = -pWOpp->m_Alpha*3.141592654/180.0;
	else	  alpha = 0.0;

	// user has requested a polar plot
	if(m_iView==2)
	{
		if(m_iWPlrView ==1)
		{
			CRect SingleRect(pCltRect->left,
							 pCltRect->top,
							 pCltRect->left+pCltRect->Width()-10000,
							 pCltRect->top+pCltRect->Height());
			m_pCurGraph->DrawGraph(pDC, &SingleRect, true);

			CPoint Place(SingleRect.right+500,-SingleRect.top);
			DrawWPolarLegend(pDC, true, Place, pCltRect->bottom);
		}
		else if(m_iWPlrView ==2){
			int w   = pCltRect->Width();
			int h   = pCltRect->Height();
			int w2  = int(w/2);
			int h34 = int(3*h/4);

			CRect FirstRect(pCltRect->left,     pCltRect->top, pCltRect->left+w2, pCltRect->top+h34);
			CRect SecondRect(pCltRect->left+w2, pCltRect->top, pCltRect->left+w,  pCltRect->top+h34);

			m_WPlrGraph1.DrawGraph(pDC, &FirstRect, true);
			m_WPlrGraph2.DrawGraph(pDC, &SecondRect, true);

			CPoint Place(pCltRect->left,-pCltRect->top-h34);
			DrawWPolarLegend(pDC, true, Place, pCltRect->bottom);
		}
		else if(m_iWPlrView ==4){
			int w   = pCltRect->Width();
			int h   = pCltRect->Height();
			int w3  = int(w/3);
			int w23 = int(2*w/3);
			int h2  = int(h/2);

			CRect Rect1(pCltRect->left,    pCltRect->top,   pCltRect->left+w3, pCltRect->top+h2);
			CRect Rect2(pCltRect->left+w3, pCltRect->top,   pCltRect->left+w23,pCltRect->top+h2);
			CRect Rect3(pCltRect->left,    pCltRect->top+h2,pCltRect->left+w3, pCltRect->top+h);
			CRect Rect4(pCltRect->left+w3, pCltRect->top+h2,pCltRect->left+w23,pCltRect->top+h);

			m_WPlrGraph1.DrawGraph(pDC, &Rect1, true);
			m_WPlrGraph2.DrawGraph(pDC, &Rect2, true);
			m_WPlrGraph3.DrawGraph(pDC, &Rect3, true);
			m_WPlrGraph4.DrawGraph(pDC, &Rect4, true);

			CPoint Place(pCltRect->left+w23+500,-pCltRect->top);
			DrawWPolarLegend(pDC, true, Place, pCltRect->bottom);
		}
	}
	else if(m_iView==4)
	{
		CRect SingleRect(pCltRect->left+2000,
						 pCltRect->top,
						 pCltRect->right-2000,
						 pCltRect->bottom-5000);
		m_CpGraph.DrawGraph(pDC, &SingleRect, true);

		CPoint Place(pCltRect->left,-SingleRect.bottom-1000);
		DrawCpLegend(pDC, true, Place, pCltRect->bottom);
	}
	else if(m_pCurWOpp)
	{
		if (m_iWingView == 1 && m_pCurWing)      PrintSingleWingGraph(pDC, pCltRect);
		else if (m_iWingView == 2 && m_pCurWing) PrintTwoWingGraph(pDC, pCltRect);
		else if (m_iWingView == 4 && m_pCurWing) PrintFourWingGraph(pDC, pCltRect);

	}
	if(m_iView==2 || m_iView==4 || m_pCurWOpp){
		CFont VFont;
		pDC->SetTextAlign(TA_RIGHT);
		pDC->SetTextColor(0);// black version name
		VFont.CreateFont(280,0,0,0,FW_NORMAL,FALSE,FALSE,0,
				ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, FF_SWISS+VARIABLE_PITCH, "Arial");

		CFont * pOldFont = pDC->SelectObject(&VFont);

		CMainFrame *pFrame = (CMainFrame*)m_pFrame;
		pDC->TextOut(pCltRect->right,-pCltRect->bottom, pFrame->m_VersionName);

		pDC->SelectObject(pOldFont);
		VFont.DeleteObject();
	}
}

void CMiarex::OnPrint() 
{
	//The user has given a print order
	CMainFrame * pFrame = (CMainFrame*) m_pFrame;

	bool bStatus = IsBlackAndWhite();

	if(m_iView!=2 && !m_pCurWOpp)
		return;
	
	if(m_pCurWOpp && m_iView==3){
		AfxMessageBox("Printing of 3D views is not implemented", MB_OK);
		return;
	}
	CString JobName;
	if(m_iView==2) {
		 JobName = "XFLR5 - WingPolars";
	}
	else {
		JobName = "XFLR5 - " + m_pCurWing->m_WingName ;
	}
	CDC dc;
	CPrintDialog dlgPrint(false);

	if(IDOK==dlgPrint.DoModal()){
		//General Options
		DOCINFO di;
		ZeroMemory(&di, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);
		di.lpszDocName = JobName;
		dc.Attach(dlgPrint.GetPrinterDC());
		
		// set landscape mode
		DEVMODE *pDevMode = dlgPrint.GetDevMode();
		int h, w;
		GetPaperSize(pDevMode, w, h);

		//Specific Options
		CPrintOptionsDlg dlg;
		dlg.m_Left   = pFrame->m_LeftMargin;
		dlg.m_Right  = pFrame->m_RightMargin;
		dlg.m_Top    = pFrame->m_TopMargin;
		dlg.m_Bottom = pFrame->m_BottomMargin;
		dlg.m_PenWidth = ((CXFLR5App*)AfxGetApp())->m_PenWidth;
		dlg.m_Width  = w;
		dlg.m_Height = h;
		dlg.m_PrinterName = dlgPrint.GetDeviceName();

		dlg.m_bColor = GetPrintColor();
		dlg.m_bWindowBk = GetWindowBk();
		dlg.m_bGraphBk = GetGraphBk();

		if (bStatus) dlg.m_bColor = false;

		if(dlg.DoModal()!=IDOK) {
			dc.Detach();
			return;
		}

		pFrame->m_LeftMargin   = dlg.m_Left;
		pFrame->m_RightMargin  = dlg.m_Right;
		pFrame->m_TopMargin    = dlg.m_Top;
		pFrame->m_BottomMargin = dlg.m_Bottom;
		((CXFLR5App*)AfxGetApp())->m_PenWidth = dlg.m_PenWidth;

		SetPrintOptions(dlg.m_bWindowBk, dlg.m_bGraphBk, dlg.m_bColor);

		if(!dlg.m_bColor) SetBlackAndWhite(true);

		CRect CltRect(  (int)(dlg.m_Left*100.0),
						(int)(dlg.m_Top*100.0),
						(int)(h*10-dlg.m_Right*100.0),
						(int)(w*10-dlg.m_Bottom*100.0));// x 0.01 mm

		if (pDevMode != NULL) { 
			pDevMode->dmFields = DM_ORIENTATION;
			pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
			dc.ResetDC(pDevMode);
			GlobalFree(pDevMode);
		}
		ASSERT(dc.m_hDC != NULL);
		dc.StartDoc(&di);
		dc.StartPage();
		PrintAll(&dc, &CltRect);
		dc.EndPage();
		dc.EndDoc();
		dc.Detach();
	}
	SetBlackAndWhite(bStatus);
	UpdateView();
}



void CMiarex::PrintTwoWingGraph(CDC *pDC, CRect *pCltRect)
{	
	//Print the two WOpp graphs
	//
//	int margin = 1500;
	int margin = 00;
	int h  = pCltRect->Height()-2*margin;
//	int w  = pCltRect->Width()-2*margin-1000;
	int w  = pCltRect->Width()-2*margin-0000;
	int w2 = (int)(w/2);
	int h23 = (int)(2*h/3);

	CRect Rect1(pCltRect->left, pCltRect->top,
				pCltRect->left+w2+margin, pCltRect->top+h23);
	CRect Rect2(pCltRect->left+w2+margin, pCltRect->top,
				pCltRect->right, pCltRect->top+h23);

	m_WingGraph1.DrawGraph(pDC, &Rect1, true);
	m_WingGraph2.DrawGraph(pDC, &Rect2, true);

	CPoint Place(pCltRect->left, -pCltRect->top-h23-500);
	DrawWOppLegend(pDC, true, Place, pCltRect->bottom);
}


void CMiarex::PrintFourWingGraph(CDC *pDC, CRect *pCltRect)
{
	//Print the four WOpp graphs
	int h   = pCltRect->Height();
	int w   = pCltRect->Width();
	int h34 = (int)(3*h/4);
	int h38 = (int)(3*h/8);
	int w2  = (int)(w/2);

	CRect Rect1(pCltRect->left, pCltRect->top,pCltRect->left+w2,pCltRect->top+h38);
	CRect Rect2(pCltRect->left+w2,pCltRect->top,pCltRect->right,pCltRect->top+h38);
	CRect Rect3(pCltRect->left,pCltRect->top+h38,pCltRect->left+w2,pCltRect->top+h34);
	CRect Rect4(pCltRect->left+w2,pCltRect->top+h38,pCltRect->right,pCltRect->top+h34);
	m_WingGraph1.DrawGraph(pDC, &Rect1, true);
	m_WingGraph2.DrawGraph(pDC, &Rect2, true);
	m_WingGraph3.DrawGraph(pDC, &Rect3, true);
	m_WingGraph4.DrawGraph(pDC, &Rect4, true);

	CPoint Place(0, -pCltRect->top-h34-500);
	DrawWOppLegend(pDC, true, Place, pCltRect->bottom);
}

void CMiarex::PrintSingleWingGraph(CDC *pDC, CRect *pCltRect)
{
	//Print the currently selected WOpp graph
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CChildView *pChildView = (CChildView*)m_pChildWnd;
	int l;
	if(!m_pCurGraph) return;

	double area = 0.0;
	CFont LegendFont;
	LOGFONT TempFont;
	memcpy(&TempFont, &pChildView->m_WndLogFont,sizeof(LOGFONT));
	TempFont.lfHeight = TempFont.lfHeight*30;
	LegendFont.CreateFontIndirect(&TempFont);
	CFont* pOldFont = pDC->SelectObject(&LegendFont);

	CRect WOppRect;
	CRect DrawRect( pCltRect->left, pCltRect->top, 
					pCltRect->right, pCltRect->bottom-6000);
	WOppRect.CopyRect(&DrawRect);

	CPoint ORef((int)((WOppRect.right+WOppRect.left)/2.0),
				pCltRect->bottom-5000);
	if(m_bHalfWing) ORef.x = WOppRect.left +1500;

	double sc1 = m_WingScale * m_pCurWing->m_Span 
				/ (m_rDrawRect.Width()-2*m_WingGraph1.GetMargin());

	double scale = (WOppRect.Width()-3000.0)/m_pCurWing->m_Span * sc1;

	PaintWing(pDC, ORef, scale, true);
//user has requested a specific operating point
	CWOpp *pWOpp = m_pCurWOpp;

	if(IsBlackAndWhite()) pDC->SetTextColor(RGB(0,0,0));
	else                  pDC->SetTextColor(pChildView->m_WndTextColor);


	if(m_pCurWOpp && m_pCurWOpp->m_bIsVisible){
			CPoint PtLegend;
		PtLegend.x = (int)(pCltRect->Width()/2);
		PtLegend.y = -pCltRect->bottom+2000;

		PaintXTr(pDC, ORef, PtLegend, scale, true);
		if (m_bXCP)     PaintXCP(pDC, ORef, PtLegend, scale, true);
	}
	if (m_bXCmRef)  PaintXCmRef(pDC, ORef, scale, true);
	m_pCurGraph->DrawGraph(pDC, &WOppRect, true);

	pDC->SetTextAlign(TA_RIGHT);
	CString Result; 

	int RightPos = DrawRect.right-100;
	int ZPos = -pCltRect->bottom + 12*400;

	int D = 00; 
	if(pWOpp){
		CString str;
		GetSpeedUnit(str, pFrame->m_SpeedUnit);			
		l = str.GetLength();
		if(l==1) Result.Format("QInf = %6.2f "+str, m_pCurWOpp->m_QInf*pFrame->m_mstoUnit);
		else if(l==2) Result.Format("QInf = %5.2f "+str, m_pCurWOpp->m_QInf*pFrame->m_mstoUnit);
		else if(l==3) Result.Format("QInf = %4.1f "+str, m_pCurWOpp->m_QInf*pFrame->m_mstoUnit);
		else if(l>=4) Result.Format("QInf = %4.1f "+str, m_pCurWOpp->m_QInf*pFrame->m_mstoUnit);

		pDC->TextOut(RightPos, ZPos-D, Result);
		D+=400;

		Result.Format("Alpha = %7.3f°", pWOpp->m_Alpha);
		pDC->TextOut(RightPos, ZPos-D, Result);

		D+=400;
		Result.Format("CL = %8.4f", pWOpp->m_CL);
		pDC->TextOut(RightPos, ZPos-D, Result);

		D+=400;
		Result.Format("VCd = %8.4f", pWOpp->m_ViscousDrag);
		pDC->TextOut(RightPos, ZPos-D, Result);

		D+=400;
		Result.Format("ICd = %8.4f", pWOpp->m_InducedDrag);
		pDC->TextOut(RightPos, ZPos-D, Result);

		D+=400;

		double cxielli=pWOpp->m_CL*pWOpp->m_CL/pi/m_pCurWing->m_AR/pWOpp->m_InducedDrag;
		Result.Format("Oswald = %8.4f", cxielli);
		pDC->TextOut(RightPos, ZPos-D, Result);
		D+=400;
		Result.Format("L/D = %8.4f", pWOpp->m_CL/(pWOpp->m_ViscousDrag+pWOpp->m_InducedDrag));
		pDC->TextOut(RightPos, ZPos-D, Result);

		D+=400;
		GetLengthUnit(str, pFrame->m_LengthUnit);
		int l = str.GetLength();
		if (l==1)     Result.Format("XCP = %6.2f ", m_pCurWOpp->m_XCP);
		else if(l==2) Result.Format("XCP = %5.1f ", m_pCurWOpp->m_XCP);
		else if(l>=3) Result.Format("XCP = %4.1f ", m_pCurWOpp->m_XCP);
		Result += str;
		pDC->TextOut(RightPos, ZPos-D, Result);

		D+=400;
		Result.Format("Pitching Moment coef. = %8.4f", pWOpp->m_GCm);
		pDC->TextOut(RightPos, ZPos-D, Result);

		D+=400;
		Result.Format("Rolling Moment coef. = %8.4f", pWOpp->m_GRm);
		pDC->TextOut(RightPos, ZPos-D, Result);

		D+=400;
		Result.Format("Yawing Moment coef. = %8.4f", pWOpp->m_GYm);
		pDC->TextOut(RightPos, ZPos-D, Result);

		D+=400;
		Result.Format("Induced Yawing Moment coef. = %8.4f", pWOpp->m_IYm);
		pDC->TextOut(RightPos, ZPos-D, Result);
	}

	pDC->SetTextAlign(TA_LEFT);
	CPoint Place(WOppRect.left +500, -WOppRect.top -1000);
	DrawWOppLegend(pDC, true, Place, WOppRect.bottom);
	
	int LeftPos = pCltRect->left+0000;
	ZPos = -pCltRect->bottom +9*400;

	if(m_pCurWPolar) ZPos +=800;
	D = 000;

	if(m_pCurWing){
		area = m_pCurWing->m_Area;
		if(m_pCurWing2) area += m_pCurWing2->m_Area;
		pDC->TextOut(LeftPos,ZPos, m_pCurWing->m_WingName);
		D+=400;

		CString length, surface, str1, str2;
		GetLengthUnit(length, pFrame->m_LengthUnit);
		GetAreaUnit(surface, pFrame->m_AreaUnit);

		Result.Format("Wing span    = %6.2f "+length, m_pCurWing->m_Span*pFrame->m_mtoUnit);
		pDC->TextOut(LeftPos,ZPos-D, Result);
		D+=400;
		Result.Format("Wing area    = %6.2f "+surface, area * pFrame->m_m2toUnit);
		pDC->TextOut(LeftPos,ZPos-D, Result);
		D+=400;

		if(m_pCurWPolar){
			GetWeightUnit(str1, pFrame->m_WeightUnit);
			GetAreaUnit(str2, pFrame->m_AreaUnit);
			Result.Format("Plane weight = %6.2f ", m_pCurWing->m_Weight*pFrame->m_kgtoUnit);
			Result += str1;
			pDC->TextOut(LeftPos,ZPos-D, Result);
			D+=400;

			Result.Format("Wing load    = %6.3f ",
						  m_pCurWing->m_Weight*pFrame->m_kgtoUnit
						  /area/pFrame->m_m2toUnit);
			Result += str1 + "/" + str2;
			pDC->TextOut(LeftPos,ZPos-D, Result);
			D+=400;
		}

		Result.Format("Root chord   = %6.2f "+length, m_pCurWing->m_TChord[0]*pFrame->m_mtoUnit);
		pDC->TextOut(LeftPos,ZPos-D, Result);
		D+=400;
	
		Result.Format("M.A.C        = %6.2f "+length, m_pCurWing->m_MAChord*pFrame->m_mtoUnit);
		pDC->TextOut(LeftPos,ZPos-D, Result);
		D+=400;
	
		Result.Format("Twist at tip = %6.1f °", m_pCurWing->m_TTwist[m_pCurWing->m_NPanel]);
		pDC->TextOut(LeftPos,ZPos-D, Result);
		D+=400;
		Result.Format("Aspect Ratio = %6.1f", m_pCurWing->m_AR);
		pDC->TextOut(LeftPos,ZPos-D, Result);

		D+=400;
		Result.Format("Taper Ratio  = %6.1f", m_pCurWing->m_TR);
		pDC->TextOut(LeftPos,ZPos-D, Result);

		D+=400;
		Result.Format("Rt-Tip Sweep = %6.1f °", m_pCurWing->GetAverageSweep());
		pDC->TextOut(LeftPos,ZPos-D, Result);
	}

	pDC->SelectObject(pOldFont);
	LegendFont.DeleteObject();
}

void CMiarex::OnDefineWing() 
{
	//Define a wing from scratch
	//On exit, check if the wing's name is already used
	int i;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CWing *pOldWing;
	CWing* pWing = new CWing(m_pFrame);
	CWingDlg WingDlg;
	WingDlg.m_pWing = pWing;
	WingDlg.CopyDataFrom(pWing);
	WingDlg.m_bCheckName= true;

	if(IDOK == WingDlg.DoModal())
	{
		CMainFrame *pFrame = (CMainFrame*)m_pFrame;
		pFrame->SetSaveState(false);
		bool bExists = false;
		for(i=0; i<m_poaWing->GetSize(); i++)
		{
			pOldWing = (CWing*)m_poaWing->GetAt(i);
			if(pWing->m_WingName == pOldWing->m_WingName)
			{
				bExists = true;
				AfxMessageBox("The name "+pWing->m_WingName+" already exists", MB_OK);
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
		m_pCurPlane = NULL;
		SetUFO();
		if(m_iView==1)
		{
			SetScale();
			OnAdjustToWing();
		}
		SetScale();
		SetWGraphScale();
	
		pFrame->UpdateUFOs();
		UpdateView();
	}
	else
	{
		delete pWing;
	}
}


void CMiarex::PaintWing(CDC *pDC, CPoint ORef, double scale, bool bIsPrinting)
{
	//Draws the wing on the 2D WOpp view
	if(!m_pCurWing)	return;
	int i;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CPoint offset;
	double scalex, scaley;
	if(!bIsPrinting)
	{
		offset.x = ORef.x;
		offset.y = ORef.y;
		scalex  = scale;
		scaley  = scale;
	}
	else
	{
		offset.x =  ORef.x;
		offset.y = -ORef.y;
		scalex   =  scale;
		scaley   = -scale;
	}
		
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	lb.lbColor = pFrame->m_OutlineColor;

	CPen WingPen(PS_GEOMETRIC | pFrame->m_OutlineStyle,
				 GetPenWidth(pFrame->m_OutlineWidth, bIsPrinting), &lb);

	CPen *pOldPen = pDC->SelectObject(&WingPen);
	CPoint O(ORef);

	//Right Wing
	O.x = offset.x;
	O.y = offset.y;
	for (i=0; i<m_pCurWing->m_NPanel;i++)
	{
		O.x+=(int)(m_pCurWing->m_TLength[i]*scalex);
		pDC->MoveTo(O.x, 
					O.y+(int)(m_pCurWing->m_TOffset[i]*scaley));
		pDC->LineTo(O.x+(int)(m_pCurWing->m_TLength[i+1]*scalex),
					O.y+(int)(m_pCurWing->m_TOffset[i+1]*scaley));
		pDC->LineTo(O.x+(int)(m_pCurWing->m_TLength[i+1]*scalex),
					O.y+(int)((m_pCurWing->m_TOffset[i+1]+m_pCurWing->m_TChord[i+1])*scaley));
		pDC->LineTo(O.x, 
					O.y +(int)((m_pCurWing->m_TOffset[i]+m_pCurWing->m_TChord[i])*scaley));
		pDC->LineTo(O.x, 
					O.y+(int)(m_pCurWing->m_TOffset[i]*scaley));
	}
	if(!m_bHalfWing)
	{
		//LeftWing
		O.x = offset.x;
		O.y = offset.y;
		for (i=0; i<m_pCurWing->m_NPanel;i++){
			O.x-=(int)(m_pCurWing->m_TLength[i]*scalex);
			pDC->MoveTo(O.x, 
						O.y+(int)(m_pCurWing->m_TOffset[i]*scaley));
			pDC->LineTo(O.x-(int)(m_pCurWing->m_TLength[i+1]*scalex),
						O.y+(int)(m_pCurWing->m_TOffset[i+1]*scaley));
			pDC->LineTo(O.x-(int)(m_pCurWing->m_TLength[i+1]*scalex),
						O.y+(int)((m_pCurWing->m_TOffset[i+1]+m_pCurWing->m_TChord[i+1])*scaley));
			pDC->LineTo(O.x, 
						O.y +(int)((m_pCurWing->m_TOffset[i]+m_pCurWing->m_TChord[i])*scaley));
			pDC->LineTo(O.x, 
						O.y+(int)(m_pCurWing->m_TOffset[i]*scaley));
		}
	}
	lb.lbStyle = BS_SOLID;
	lb.lbColor = RGB(155,128,190);

	CPen SymPen(PS_GEOMETRIC | PS_DASHDOT,GetPenWidth(1, bIsPrinting), &lb);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SelectObject(&SymPen);
	if(!bIsPrinting){
		pDC->MoveTo(offset.x, offset.y-20);
		pDC->LineTo(offset.x, offset.y+75);
	}
	else{
		pDC->MoveTo(offset.x, offset.y-1000);
		pDC->LineTo(offset.x, offset.y+3000);
	}

	pDC->SelectObject(pOldPen);
	SymPen.DeleteObject();
	WingPen.DeleteObject();
}


void CMiarex::PaintXTr(CDC *pDC, CPoint ORef, CPoint OLegend, double scale, bool bIsPrinting)
{
	//Draws the transition lines on the 2D view
	if(!m_pCurWing){
		return;
	}
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(bIsPrinting && IsBlackAndWhite()) 	pDC->SetTextColor(RGB(0,0,0));
	double y;
	int nStart;
	if(m_pCurWOpp->m_AnalysisType==1) nStart = 1; else nStart = 0;
	if(m_bHalfWing) nStart = m_pCurWOpp->m_NStation/2;
	CPoint O(ORef);
	CPoint offset;

	double offLE;

	double scalex, scaley;
	if(!bIsPrinting){
		offset.x = ORef.x;
		offset.y = ORef.y;
		scalex  = scale;
		scaley  = scale;
	}
	else{
		offset.x =  ORef.x;
		offset.y = -ORef.y;
		scalex   =  scale;
		scaley   = -scale;
	}
	O.x = offset.x;
	O.y = offset.y;

	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	COLORREF color;
	int style, width;
	color = pFrame->m_XTopColor;
	style = pFrame->m_XTopStyle;
	width = pFrame->m_XTopWidth;
	GetBWColor(color, style, width);

	lb.lbColor = color;

	CPen TopPen;
	TopPen.CreatePen(PS_GEOMETRIC | style, GetPenWidth(width,m_bIsPrinting), &lb);
	CPen *pOldPen = pDC->SelectObject(&TopPen);

	if (m_bXTop){// top transition

		offLE = m_pCurWing->GetOffset(m_pCurWOpp->m_SpanPos[nStart]*2.0/m_pCurWOpp->m_Span);
		y = (offLE+m_pCurWOpp->m_Chord[nStart]*m_pCurWOpp->m_XTrTop[nStart])*scaley;
		pDC->MoveTo(O.x+(int)(m_pCurWOpp->m_SpanPos[nStart]*scalex),
					O.y+(int)y);

	for (int m=nStart; m<m_pCurWOpp->m_NStation; m++){
			offLE = m_pCurWing->GetOffset(m_pCurWOpp->m_SpanPos[m]*2.0/m_pCurWOpp->m_Span);
			y = (offLE+m_pCurWOpp->m_Chord[m]*m_pCurWOpp->m_XTrTop[m])*scaley;

			if(m_bHalfWing) pDC->LineTo(O.x+(int)(m_pCurWOpp->m_SpanPos[m]*scalex),
										O.y+(int)y );
			else			pDC->LineTo(O.x+(int)(m_pCurWOpp->m_SpanPos[m]*scalex),
										O.y+(int)y );
		}
	
		if(!bIsPrinting){
			int x = (int)(m_rCltRect.Width()/2);
			int y = m_rCltRect.bottom;
			pDC->MoveTo(x-60,  y - 50);
			pDC->LineTo(x-40,  y - 50);
			pDC->TextOut(x-35, y - 57, "Top transition");
		}
		else{
			pDC->MoveTo(OLegend.x , OLegend.y );
			pDC->LineTo(OLegend.x -750, OLegend.y);
			pDC->TextOut(OLegend.x+1500,OLegend.y +200, "Top transition");
		}
	}
	color = pFrame->m_XBotColor;
	style = pFrame->m_XBotStyle;
	width = pFrame->m_XBotWidth;
	GetBWColor(color, style, width);

	lb.lbColor = color;

	CPen BotPen;
	BotPen.CreatePen(PS_GEOMETRIC | style, GetPenWidth(width,m_bIsPrinting), &lb);
//	CPen BotPen(PS_GEOMETRIC | m_XBotStyle,GetPenWidth(m_XBotWidth,bIsPrinting), &lb);
	pDC->SelectObject(&BotPen);
	if (m_bXBot){// Bottom transition

		offLE = m_pCurWing->GetOffset(m_pCurWOpp->m_SpanPos[nStart]*2.0/m_pCurWOpp->m_Span);
		y = (offLE+m_pCurWOpp->m_Chord[nStart]*m_pCurWOpp->m_XTrBot[nStart])*scaley;
		pDC->MoveTo(O.x+(int)(m_pCurWOpp->m_SpanPos[nStart]*scalex),
					O.y+(int)y );
	for (int m=nStart; m<m_pCurWOpp->m_NStation; m++){
			offLE = m_pCurWing->GetOffset(m_pCurWOpp->m_SpanPos[m]*2.0/m_pCurWOpp->m_Span);
			y = (offLE+m_pCurWOpp->m_Chord[m]*m_pCurWOpp->m_XTrBot[m])*scaley;
			if(m_bHalfWing) pDC->LineTo(O.x+(int)(m_pCurWOpp->m_SpanPos[m]*scalex),
										O.y+(int)y );
			else			pDC->LineTo(O.x+(int)(m_pCurWOpp->m_SpanPos[m]*scalex),
										O.y+(int)y );
		}
	

		if(!bIsPrinting){
			int x = (int)(m_rCltRect.Width()/2);
			int y = m_rCltRect.bottom;
			pDC->MoveTo(x-60,  y - 35);
			pDC->LineTo(x-40,  y - 35);
			pDC->TextOut(x-35, y - 42, "Bottom transition");
		}
		else{
			pDC->MoveTo(OLegend.x , OLegend.y -750 );
			pDC->LineTo(OLegend.x -750, OLegend.y-750);
			pDC->TextOut(OLegend.x+1500,OLegend.y -550, "Bot transition");
		}
	}
	pDC->SelectObject(pOldPen);
	TopPen.DeleteObject();
	BotPen.DeleteObject();
}

void CMiarex::PaintXCP(CDC *pDC, CPoint ORef, CPoint OLegend, double scale, bool bIsPrinting)
{
	//Draws the lift line and center of pressure position on the the 2D view
	if(!m_pCurWing)	return;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	if(bIsPrinting && IsBlackAndWhite()) 	pDC->SetTextColor(RGB(0,0,0));

	double y;
	int nStart;
	double offLE;
	if(m_pCurWOpp->m_AnalysisType==1) nStart = 1; else nStart = 0;
	if(m_bHalfWing) nStart = m_pCurWOpp->m_NStation/2;

	CPoint O(ORef);
	CPoint offset;

	double scalex, scaley;
	if(!bIsPrinting){
		offset.x = ORef.x;
		offset.y = ORef.y;
		scalex  = scale;
		scaley  = scale;
	}
	else{
		offset.x =  ORef.x;
		offset.y = -ORef.y;
		scalex   =  scale;
		scaley   = -scale;
	}
	O.x = offset.x;
	O.y = offset.y;

	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	COLORREF color;
	int style, width;
	color = pFrame->m_XCPColor;
	style = pFrame->m_XCPStyle;
	width = pFrame->m_XCPWidth;
	GetBWColor(color, style, width);

	lb.lbColor = color;

	CPen XCPPen;
	XCPPen.CreatePen(PS_GEOMETRIC | style, GetPenWidth(width,m_bIsPrinting), &lb);
	CPen *pOldPen = pDC->SelectObject(&XCPPen);
	
	CBrush FillBrush;
	FillBrush.CreateSolidBrush(color);
	CBrush *pOldBrush = pDC->SelectObject(&FillBrush);

	int XCp = O.x + (int)(m_pCurWOpp->m_YCP*scalex);
	int YCp = O.y + (int)(m_pCurWOpp->m_XCP*scaley);
	int size = GetPenWidth(3,bIsPrinting);
	CRect CP(XCp-size, YCp-size, XCp+size, YCp+size);
	pDC->Ellipse(&CP);

	offLE = m_pCurWing->GetOffset(m_pCurWOpp->m_SpanPos[nStart]*2.0/m_pCurWOpp->m_Span);
	y = (offLE+m_pCurWOpp->m_Chord[nStart]*m_pCurWOpp->m_XCPSpanRel[nStart])*scaley;
	pDC->MoveTo(O.x+(int)(m_pCurWOpp->m_SpanPos[nStart]*scalex),
				O.y+(int)y );

	for (int m=nStart; m<m_pCurWOpp->m_NStation; m++){
		offLE = m_pCurWing->GetOffset(m_pCurWOpp->m_SpanPos[m]*2.0/m_pCurWOpp->m_Span);
		y = (offLE+m_pCurWOpp->m_Chord[m]*m_pCurWOpp->m_XCPSpanRel[m])*scaley;
		if(m_bHalfWing) pDC->LineTo(O.x+(int)(m_pCurWOpp->m_SpanPos[m]*scalex),
									O.y+(int)y );
		else			pDC->LineTo(O.x+(int)(m_pCurWOpp->m_SpanPos[m]*scalex),
									O.y+(int)y );
	}
	
	if(!bIsPrinting){
		int x = (int)(m_rCltRect.Width()/2);
		int y = m_rCltRect.bottom;
		pDC->MoveTo(x-60,  y - 20);
		pDC->LineTo(x-40,  y - 20);
		pDC->TextOut(x-35, y - 27, "Centre of Pressure");
	}
	else{
		int x = (int)(m_rCltRect.Width()/2);
		pDC->MoveTo(OLegend.x , OLegend.y -1500 );
		pDC->LineTo(OLegend.x -750, OLegend.y-1500);
		pDC->TextOut(OLegend.x+1500,OLegend.y -1300, "Centre of Pressure");
	}

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);

	XCPPen.DeleteObject();
}

void CMiarex::PaintXCmRef(CDC *pDC, CPoint ORef, double scale, bool bIsPrinting)
{
	//Draws the moment reference point on the 2D view
	if(!m_pCurWing || m_pCurWPolar)	return;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	CPoint O(ORef);
	CPoint offset;

	double scalex, scaley;
	if(!bIsPrinting){
		offset.x = ORef.x;
		offset.y = ORef.y;
		scalex  = scale;
		scaley  = scale;
	}
	else{
		offset.x =  ORef.x;
		offset.y = -ORef.y;
		scalex   =  scale;
		scaley   = -scale;
	}
	O.x = offset.x;
	O.y = offset.y;

	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	lb.lbColor = RGB(0,0,0);
	CPen XCmRefPen(PS_GEOMETRIC | 0,GetPenWidth(1,bIsPrinting), &lb);
	CPen *pOldPen = pDC->SelectObject(&XCmRefPen);

	CBrush FillBrush;
	if(IsBlackAndWhite())	FillBrush.CreateSolidBrush(RGB(200,200,200));
	else 	                FillBrush.CreateSolidBrush(RGB(0,0,255));
	
	CBrush *pOldBrush = pDC->SelectObject(&FillBrush);

	int XCm = O.x ;
	int YCm = O.y + (int)(m_pCurWPolar->m_XCmRef*scaley);
	int size = GetPenWidth(3,bIsPrinting);
	CRect CM(XCm-size, YCm-size, XCm+size, YCm+size);
	pDC->Ellipse(&CM);

	if(!bIsPrinting){
		pDC->TextOut(XCm+10, YCm-5, "Moment ref. location");
	}
	else{
		pDC->TextOut(XCm+250, YCm, "Moment ref. location");
	}

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);

	XCmRefPen.DeleteObject();
}

void CMiarex::OnExportPanels()
{
	//Export the VLM panels to a text file
	if(!m_pCurWing)	return;
	CString FileName;
	CStdioFile XFile;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	CFileException fe;
	CString strong;

	FileName = m_pCurWing->m_WingName;
	FileName.Replace("/", " ");
	CFileDialog XFileDlg(false, "txt", FileName, OFN_OVERWRITEPROMPT,
		_T("Text Format (.txt)|*.txt|"));

	if(IDOK==XFileDlg.DoModal()) {
		FileName = XFileDlg.GetFileName();
		BOOL bOpen = XFile.Open(FileName, CFile::modeCreate | CFile::modeWrite, &fe);

		if(bOpen){
			strong = pFrame->m_ProjectName;
			int len = strong.GetLength();
			if (strong.Right(1) == "*") strong = strong.Left(len-1);
			if(!strong.GetLength()) XFile.WriteString("Project");
			else XFile.WriteString(strong);
			XFile.WriteString("\n");
			XFile.WriteString("Node          x            y            z\n");
			for (int n=0; n<m_nNodes; n++){
				strong.Format("%4d   %10.4f   %10.4f   %10.4f\n",n,m_Node[n].x,m_Node[n].y,m_Node[n].z);
				XFile.WriteString(strong);
			}
			XFile.WriteString("\n\n");
			XFile.WriteString("\n");
			XFile.WriteString("Panel     Trailing.Left    Leading.Left    Trailing.Right   Leading.Right    \n");
			for (int p=0; p<m_MatSize; p++){
				strong.Format("%4d         %4d             %4d             %4d             %4d\n",p,
					m_Panel[p].m_iTA, m_Panel[p].m_iLA, m_Panel[p].m_iTB, m_Panel[p].m_iLB);
				XFile.WriteString(strong);
			}
			XFile.WriteString("\n\n");
		}
	}
}


void CMiarex::OnExportWing() 
{
	//Export the wing data to AVL format
	if(!m_pCurWing)	return;
	
	CString FileName;
	CStdioFile XFile;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	CFileException fe;
	CString strong;

	FileName = m_pCurWing->m_WingName;
	FileName.Replace("/", " ");
	CFileDialog XFileDlg(false, "avl", FileName, OFN_OVERWRITEPROMPT,
		_T("Text Format (.avl)|*.avl|"));

	if(IDOK==XFileDlg.DoModal()) {
		FileName = XFileDlg.GetFileName();
		BOOL bOpen = XFile.Open(FileName, CFile::modeCreate | CFile::modeWrite, &fe);

		if(bOpen){
			strong = pFrame->m_ProjectName;
			int len = strong.GetLength();
			if (strong.Right(1) == "*") strong = strong.Left(len-1);
			if(!strong.GetLength()) XFile.WriteString("Project");
			else XFile.WriteString(strong);
			XFile.WriteString("\n");
			XFile.WriteString("0.0                          | Mach\n");
			if(m_pCurWing->m_bSymetric) XFile.WriteString("0     0     0.0              | iYsym  iZsym  Zsym\n");
			else                        XFile.WriteString("0     0     0.0              | iYsym  iZsym  Zsym\n");
			XFile.WriteString("\n");
			
			strong.Format("%7.2f   %7.2f   %7.2f  | Sref   Cref   Bref\n",
				m_pCurWing->m_Area*pFrame->m_m2toUnit,
				m_pCurWing->m_MAChord*pFrame->m_mtoUnit,
				m_pCurWing->m_Span*pFrame->m_mtoUnit);
			XFile.WriteString(strong);

			strong.Format("%5.2f  %5.2f  %5.2f          | Xref   Yref   Zref\n", 0.0, 0.0, 0.0);
			XFile.WriteString(strong);

			XFile.WriteString(" 0.00                        | CDp  (optional)\n");

			XFile.WriteString("\n\n\n");

			m_pCurWing->ExportAVLWing(&XFile, 0.0, 0.0, 0.0, 0.0, false);

			XFile.Close();
		}
	}
}
/*
void CMiarex::OnShowPoints() 
{
	CCurve *pCurve;
	for (i=0; i<m_WingGraph1.GetCurveCount();i++){
		pCurve = m_WingGraph1.GetCurve(i);
		pCurve->m_bShowPoints = !pCurve->m_bShowPoints;
	}
	UpdateView();
}*/

void CMiarex::OnShowXCmRef() 
{
	//Show the Moment reference point
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	m_bXCmRef = !m_bXCmRef;
	CMenu *pMenu = pFrame->GetMenu();
	if(m_bXCmRef)	pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWXCMREF, MF_BYCOMMAND | MF_CHECKED);
	else			pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWXCMREF, MF_BYCOMMAND | MF_UNCHECKED);

	UpdateView();
}

void CMiarex::OnEditWing() 
{
	//Edit the currently selected wing
	if(!m_pCurWing)	return;
	if(m_iView==5) return;

	if(m_pCurPlane)
	{
		OnEditPlane();
		return;
	}
	int i;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CWPolar *pWPolar;
	CWOpp* pWOpp;
	CWingDlg WingDlg;
	WingDlg.CopyDataFrom(m_pCurWing);

	bool bHasResults = false;
	for (i=0; i< m_poaWPolar->GetSize(); i++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->GetAt(i);
		if (pWPolar->m_Alpha.GetSize() && pWPolar->m_UFOName == m_pCurWing->m_WingName)
		{
			bHasResults = true;
			break;
		}
	}

	for (i=0; i< m_poaWOpp->GetSize(); i++)
	{
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		if(pWOpp->m_WingName == m_pCurWing->m_WingName)
		{
			bHasResults = true;
			break;
		}
	}

	//save a copy just in case user changes his mind (users !...)
	CWing* pSaveWing= new CWing(m_pFrame);
	pSaveWing->s_pMiarex = this;
	pSaveWing->Duplicate(m_pCurWing);

	WingDlg.m_bCheckName= false;

	if(IDOK == WingDlg.DoModal())
	{
		if(WingDlg.m_bChanged)
		{
			if(bHasResults)
			{
				CModDlg dlg;
				dlg.m_Question = "The modification will erase all results associated to this Wing.\nContinue ?";
				int Ans = (int)dlg.DoModal();			
				if (Ans == IDCANCEL) 
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

					if(!SetModWing(pNewWing)){
						
						delete pNewWing;
					}
					else {
						m_pCurWing = AddWing(pNewWing);
					}
					SetUFO();
					pFrame->UpdateUFOs();
					UpdateView();
					return;
				}
			}

			//then modifications are automatically recorded
			pFrame->DeleteWing(m_pCurWing, true);// will also set new surface and Aerochord in WPolars
			if(m_iView==3) 
			{
				m_bResetglGeom = true;
				m_bResetglMesh = true;
				m_bResetglOpp  = true;
			}
			else
			{
				if(m_iView==2)		CreateWPolarCurves();
				else if(m_iView==1)	CreateWOppCurves();
				else if(m_iView==4)	CreateCpCurves();
			}
		}
		SetUFO();	
		pFrame->UpdateWPlrs();
		SetWPlr();
		SetScale();
		SetWGraphScale();
		OnAdjustToWing();
	}
	else 
	{
		// restore original
		m_pCurWing->Duplicate(pSaveWing);
		UpdateView();
	}
	delete pSaveWing; // clean up
}


void CMiarex::OnDuplicateWing() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	//Duplicate the currently selected wing
	if(!m_pCurWing)	return;
	
	if(m_pCurPlane) 
	{
		DuplicatePlane();
		return;
	}

	CString UFOName;
	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return;

	CWing* pNewWing= new CWing(m_pFrame);
	pNewWing->s_pMiarex = this;
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
		pFrame->UpdateUFOs();
		OnEditWing();
	}
}

void CMiarex::DuplicatePlane() 
{
	//Duplicate the currently selected plane, if any
	if(!m_pCurPlane) return;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CPlane* pNewPlane= new CPlane();
	pNewPlane->Duplicate(m_pCurPlane);

	if(!SetModPlane(pNewPlane)){
		delete pNewPlane;
		pFrame->UpdateUFOs();
		SetUFO();
		UpdateView();
	}
	else {
		m_pCurPlane = AddPlane(pNewPlane);
		pFrame->UpdateUFOs();
		SetUFO();
		UpdateView();
		OnEditPlane();
	}
}

void CMiarex::OnScaleWing() 
{
	//Scale the currently selected wing, if any
	if(!m_pCurWing)	return;
	if(m_pCurPlane)
	{
		AfxMessageBox("No scaling function is implemented for plane objects", MB_OK);
		return;
	}
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CWingScaleDlg dlg(this);
	dlg.m_pMiarex = this;
	dlg.m_pFrame  = m_pFrame;
	dlg.m_InitialSpan  = m_pCurWing->m_Span;
	dlg.m_InitialChord = m_pCurWing->m_TChord[0];

	dlg.m_InitialSweep = m_pCurWing->GetAverageSweep();
	dlg.m_Twist = m_pCurWing->m_TTwist[m_pCurWing->m_NPanel];

	if(IDOK == dlg.DoModal())
	{
		if (dlg.m_bScaleSpan || dlg.m_bScaleChord || dlg.m_bScaleSweep || dlg.m_bScaleTwist)
		{
			CWing* pNewWing= new CWing(m_pFrame);
			pNewWing->s_pMiarex = this;
			pNewWing->Duplicate(m_pCurWing);
			if(dlg.m_bScaleSpan)  pNewWing->ScaleSpan(dlg.m_NewSpan);
			if(dlg.m_bScaleChord) pNewWing->ScaleChord(dlg.m_NewChord);
			if(dlg.m_bScaleSweep) pNewWing->SetSweep(dlg.m_NewSweep);
			if(dlg.m_bScaleTwist) pNewWing->SetTwist(dlg.m_Twist);
			if(SetModWing(pNewWing)){
				m_pCurWing = AddWing(pNewWing);
				m_pCurWPolar = NULL;
				pFrame->m_WOperDlgBar.EnableAnalysis(false);
				m_pCurPOpp = NULL;
				m_pCurWOpp = NULL;
			}
			else 
				delete pNewWing;
			pFrame->UpdateUFOs();
			SetUFO();
		}
		UpdateView();
	}
}
void CMiarex::SetBody(CString BodyName)
{
	CWaitCursor wait;

	//Set the Body specified by BodyName as the current Body
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(!BodyName.GetLength()) 
	{
		if(m_pCurBody) BodyName = m_pCurBody->m_BodyName;
		else {
			if(m_pBodyCtrlBar->m_ctrlBodyList.GetCount())
			{
				m_pBodyCtrlBar->m_ctrlBodyList.SetCurSel(0);
				m_pBodyCtrlBar->m_ctrlBodyList.GetLBText(0, BodyName);
			}
			else
			{
				m_pCurBody  = NULL;
				m_pCurFrame = NULL;
				m_pBodyCtrlBar->SetBody(m_pCurBody);
				return;
			}
		}
	}
	m_pCurBody = GetBody(BodyName);

	if(!m_pCurBody) 
	{
		m_pCurFrame = NULL;
		m_pBodyCtrlBar->SetBody(m_pCurBody);
		return;
	}

	m_bResetglBody   = true;
	m_bResetglBody2D = true;

	SetBodyScale();
	
	if(m_pCurBody->m_iActiveFrame>0)
	{
		m_pCurFrame = m_pCurBody->m_Frame+m_pCurBody->m_iActiveFrame;
		m_pCurFrame->m_iSelect = 0;
	}
	else m_pCurFrame=NULL;

	m_pBodyCtrlBar->SetBody(m_pCurBody);
}

void CMiarex::SetUFO(CString UFOName)
{
	CWaitCursor wait;
	int i;
	//Set the UFO specified by UFOName as the current UFO

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(!UFOName.GetLength()) 
	{
		if(m_pCurPlane) UFOName = m_pCurPlane->m_PlaneName;
		else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
		else {
			if(pFrame->m_ctrlUFO.GetCount())
			{
				pFrame->m_ctrlUFO.SetCurSel(0);
				pFrame->m_ctrlUFO.GetLBText(0, UFOName);
			}
			else {
				m_pCurPlane = NULL;
				m_pCurBody  = NULL;
				m_pCurWing  = NULL;
				m_pCurWing2 = NULL;
				m_pCurStab  = NULL;
				m_pCurFin   = NULL;
				return;
			}
		}
	}
	int pos = pFrame->m_ctrlUFO.FindStringExact(0, UFOName);
	if (pos!=CB_ERR)
	{
		m_pCurPlane = GetPlane(UFOName);
		m_pCurWing  = GetWing(UFOName);
		pFrame->m_ctrlUFO.SetCurSel(pos);
	}

	pFrame->UpdateWPlrs();
	if(!m_pCurWing && !m_pCurPlane) return;


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
		m_bResetglBody2D = true;
	}
	else
	{
		m_bResetglBody   = false;
		m_bResetglBody2D = false;
	}

	m_bResetglMesh = true;

	double WingTilt = 0.0;
	COLORREF WingColor = m_WingColor;
	CVector T(0.0,0.0,0.0);
	m_NSurfaces = 0;

	if(m_pCurPlane)
	{
		WingTilt  = m_pCurPlane->m_WingTilt;
		WingColor = m_pCurWing->m_WingColor;
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
			m_pCurWing->m_Surface[i].SetSidePoints(m_pCurBody);
			m_pSurface[m_NSurfaces] = &m_pCurWing->m_Surface[i];
			m_NSurfaces++;
		}
	}

	if(m_pCurWing2) 
	{
		m_pCurWing2->CreateSurfaces(m_pCurPlane->m_LEWing2, 0.0, m_pCurPlane->m_WingTilt2);
		for (i=0; i<m_pCurWing2->m_NSurfaces; i++)
		{
			m_pCurWing2->m_Surface[i].SetSidePoints(m_pCurBody);
			m_pSurface[m_NSurfaces] = &m_pCurWing2->m_Surface[i];
			m_NSurfaces++;
		}
	}


	if(m_pCurStab) 
	{
		m_pCurStab->CreateSurfaces(m_pCurPlane->m_LEStab, 0.0, m_pCurPlane->m_StabTilt);
		for (i=0; i<m_pCurStab->m_NSurfaces; i++)
		{
			m_pCurStab->m_Surface[i].SetSidePoints(m_pCurBody);
			m_pSurface[m_NSurfaces] = &m_pCurStab->m_Surface[i];
			m_NSurfaces++;
		}
	}

	if(m_pCurFin)
	{
		m_pCurFin->CreateSurfaces(m_pCurPlane->m_LEFin, -90.0, m_pCurPlane->m_FinTilt);

		for (i=0; i<m_pCurFin->m_NSurfaces; i++)
		{
			m_pCurFin->m_Surface[i].SetSidePoints(m_pCurBody);
			m_pSurface[m_NSurfaces] = &m_pCurFin->m_Surface[i];
			m_NSurfaces++;
		}
	}


	if (m_pCurWPolar)
		// try to set the same as the existing polar... Special for Marc
		SetWPlr(false, m_pCurWPolar->m_PlrName);
	else 
		SetWPlr();

	SetScale();
	SetWGraphScale();

}


void CMiarex::OnDeleteWing() 
{
	//Delete the currently selected wing, if any
	if(!m_pCurWing) return;
	m_bAnimate = false;

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString strong;
	if(m_pCurPlane)	strong = "Are you sure you want to delete the plane :\n" +  m_pCurPlane->m_PlaneName +"?\n";
	else 	        strong = "Are you sure you want to delete the wing :\n" +   m_pCurWing->m_WingName +"?\n";
	if(IDYES != AfxMessageBox(strong, MB_YESNOCANCEL)) return;
	if(m_pCurPlane) pFrame->DeletePlane(m_pCurPlane);
	else            pFrame->DeleteWing(m_pCurWing, false);
	pFrame->UpdateUFOs();
	SetUFO();
	UpdateView();
}

void CMiarex::OnPolarFilter() 
{
	//Select the polar types to be displayed
	CPolarFilter dlg;
	dlg.m_bWing = true;
	if (m_bType1) dlg.m_bType1 = TRUE;
	if (m_bType2) dlg.m_bType2 = TRUE;
	if (m_bType4) dlg.m_bType4 = TRUE;

	if(dlg.DoModal() == IDOK){
		if (dlg.m_bType1) m_bType1 = true;
		else m_bType1  = false;
		if (dlg.m_bType2) m_bType2 = true;
		else m_bType2  = false;
		if (dlg.m_bType4) m_bType4 = true;
		else m_bType4  = false;
		CreateWPolarCurves();
		UpdateView();
	}
}

void CMiarex::UpdateUnits()
{
	//The units have been changed, so update the graphs and the views
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(m_iView==2){
		CreateWPolarCurves();
		SetWGraphTitles(&m_WPlrGraph1, m_XW1,m_YW1);
		SetWGraphTitles(&m_WPlrGraph2, m_XW2,m_YW2);
		SetWGraphTitles(&m_WPlrGraph3, m_XW3,m_YW3);
		SetWGraphTitles(&m_WPlrGraph4, m_XW4,m_YW4);
		UpdateView();
	}
	else {
		if(!m_pCurWing) return;
		if (m_iView==1)   {
			CreateWOppCurves();
			OnAdjustToWing();
		}
		else if(m_iView==4)	CreateCpCurves();
	}
	m_bResetglOpp = true; 
	m_bResetglGeom = true;
	UpdateView();
	m_FlowLinesDlg.SetUnits();
	m_FlowLinesDlg.SetParams();
}

void CMiarex::UpdateView(CDC *pDC)
{
	//Refresh the active view
	if(m_bAnimate)
		return;// painting is performed elsewhere

	CChildView * pChildView = (CChildView*)m_pChildWnd;

	if(!pDC) pDC = pChildView->GetDC();
	if(!pDC) return;

	if(m_iView==3)		
	{
		GLDraw3D(pDC);
		GLRenderView(pDC);
	}
	else if(m_iView==5)		
	{
		GLDraw3D(pDC);
		GLRenderBody(pDC);
	}
	else
	{
		CDC memdc; 
		memdc.CreateCompatibleDC(pDC);

		CBitmap  bmp;
		bmp.CreateCompatibleBitmap(pDC, m_rCltRect.Width(), m_rCltRect.Height());
		CBitmap * pOldBmp = memdc.SelectObject(&bmp);

		memdc.FillSolidRect(&m_rCltRect,pChildView->m_crBackColor);
		memdc.SetBkMode(TRANSPARENT);

		if (m_iView==2)
		{
			if     (m_iWPlrView == 1) PaintWSingleGraph(&memdc, &m_rCltRect, &m_rDrawRect);
			else if(m_iWPlrView == 2) PaintWCoupleGraphs(&memdc, &m_rCltRect, &m_rDrawRect);
			else if(m_iWPlrView == 4) PaintWFourGraphs(&memdc, &m_rCltRect, &m_rDrawRect);
			pDC->BitBlt(m_rCltRect.left,m_rCltRect.top,m_rCltRect.Width(),m_rCltRect.Height(),
					&memdc,0,0,SRCCOPY);
		} 
		else if (m_iView==1) 
		{
			PaintWOpp(&memdc, &m_rCltRect, &m_rDrawRect);
			pDC->BitBlt(m_rCltRect.left,m_rCltRect.top,m_rCltRect.Width(),m_rCltRect.Height(),
					&memdc,0,0,SRCCOPY);
		}
		else if (m_iView==4)
		{
			PaintCp(&memdc, &m_rCltRect, &m_rSingleRect);
			pDC->BitBlt(m_rCltRect.left,m_rCltRect.top,m_rCltRect.Width(),m_rCltRect.Height(),
					&memdc,0,0,SRCCOPY);
		}


		memdc.SelectObject(pOldBmp);
		memdc.DeleteDC();
		bmp.DeleteObject();
	}
	pChildView->ReleaseDC(pDC);
}


void CMiarex::SetColors(Graph *pDefaultGraph)
{
	//Update the graph colors and settings after the user has changed the macro settings
	m_WingGraph1.CopySettings(pDefaultGraph);
	m_WingGraph2.CopySettings(pDefaultGraph);
	m_WingGraph3.CopySettings(pDefaultGraph);
	m_WingGraph4.CopySettings(pDefaultGraph);

	m_WPlrGraph1.CopySettings(pDefaultGraph);
	m_WPlrGraph2.CopySettings(pDefaultGraph);
	m_WPlrGraph3.CopySettings(pDefaultGraph);
	m_WPlrGraph4.CopySettings(pDefaultGraph);
	m_IDlg.m_IterGraph.CopySettings(pDefaultGraph);
}


BOOL CMiarex::PreTranslateMessage(MSG* pMsg) 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	if (pMsg->message == WM_KEYUP)
	{
		if (pMsg->wParam == VK_CONTROL)
		{
			if(m_iView==3 || m_iView==5)
			{
				m_bArcball = false;
				UpdateView();
			}
		}
	}
	else if (pMsg->message == WM_KEYDOWN)
	{
		CWnd* pWnd = GetFocus();
		if (pMsg->wParam == VK_TAB){
			if (pWnd == &pFrame->m_ctrlUFO) {
				pFrame->m_ctrlWPlr.SetFocus();
				return true;
			}
			else if (pWnd == &pFrame->m_ctrlWPlr) {
				pFrame->m_ctrlWOpp.SetFocus();
				return true;
			}
			else if (pWnd == &pFrame->m_ctrlWOpp) {
				pFrame->m_ctrlUFO.SetFocus();
				return true;
			}
		}
		if (pMsg->wParam == VK_RETURN)
		{
			if (pWnd != &pFrame->m_ctrlUFO &&
				pWnd != &pFrame->m_ctrlWPlr &&
				pWnd != &pFrame->m_ctrlWOpp) 
			{
				if(pFrame->m_WOperDlgBar.IsWindowVisible()) 
					pFrame->m_WOperDlgBar.m_ctrlAnalyze.SetFocus();
				return true;
			}
		}
		if (pMsg->wParam == VK_CONTROL)
		{
			if(m_iView==3 || m_iView==5)
			{
				if(!m_bArcball)
				{
					m_bArcball = true;

					UpdateView();
				}
			}
		}
		if (pMsg->wParam == VK_ESCAPE){
			if (m_bAnimate) {
				StopAnimate();
				UpdateView();
				return true;
			}
		}
		SHORT sh   = GetKeyState(VK_SHIFT);
		SHORT sh1 = GetKeyState(VK_LCONTROL);
		SHORT sh2 = GetKeyState(VK_RCONTROL);

		if (pMsg->wParam == VK_F2 ){
			OnRenameUFO();
			return true;
		}
		if (pMsg->wParam == VK_F7 )
		{
			OnManageUFO();
			return true;
		}
		if (pMsg->wParam == VK_F3)
		{
			if((sh1 & 0x8000)||(sh2 & 0x8000)) 
			{
				OnDefinePlane();
				return true;
			}
			if(!(sh & 0x8000))	OnDefineWing();
			else                OnEditWing();
			return true;
		}
		if (pMsg->wParam == VK_F4){
			On3DView();
			return true;
		}
		if (pMsg->wParam == VK_F5){
			OnWOpp();
			return true;
		}
		if (pMsg->wParam == VK_F6)
		{
			if((sh1 & 0x8000)||(sh2 & 0x8000)) 
			{
				OnControlAnalysis();
				return true;
			}
			OnDefineAnalysis();
			return true;
		}

		if (pMsg->wParam == VK_F8){
			OnWPolar();
			return true;
		}
		if (pMsg->wParam == VK_F9){
			OnCpView();
			return true;
		}

		if (pMsg->wParam == VK_F11){
			OnNewBody();
			return true;
		}

		if (pMsg->wParam == 'F' && ((sh1 & 0x8000)||(sh2 & 0x8000))) { 
			m_bSpeeds = !m_bSpeeds;
			UpdateView();
			return true;
		}

		if (pMsg->wParam == 'P' && ((sh1 & 0x8000)||(sh2 & 0x8000))) { 
			OnPrint();
			return true;
		}
		if ((pMsg->wParam == '1' || pMsg->wParam == 97)&&
			pWnd ==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))){
			if(m_iView==2) {
				m_iWPlrView = 1;
				m_pCurGraph = &m_WPlrGraph1;
				m_pCurWPlrGraph = m_pCurGraph;
				SetWPlrLegendPos();
				CheckMenus();
			}
			else{
				m_iWingView =1;
				m_pCurGraph = &m_WingGraph1;
				m_pCurWingGraph = m_pCurGraph;
				SetWingLegendPos();
				CheckMenus();
			}
			UpdateView();
			return true;
		}
		if ((pMsg->wParam == '2' || pMsg->wParam == 98)&&
			pWnd ==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))){
			if(m_iView==2) {
				m_iWPlrView = 1;
				m_pCurGraph = &m_WPlrGraph2;
				m_pCurWPlrGraph = m_pCurGraph;
				SetWPlrLegendPos();
				CheckMenus();
			}
			else{
				m_iWingView =1;
				m_pCurGraph = &m_WingGraph2;
				m_pCurWingGraph = m_pCurGraph;
				SetWingLegendPos();
				CheckMenus();
			}
			UpdateView();
			return true;
		}
		if ((pMsg->wParam == '3' || pMsg->wParam == 99)&&
			pWnd ==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))){
			if(m_iView==2) {
				m_iWPlrView = 1;
				m_pCurGraph = &m_WPlrGraph3;
				m_pCurWPlrGraph = m_pCurGraph;
				SetWPlrLegendPos();
				CheckMenus();
			}
			else{
				m_iWingView =1;
				m_pCurGraph = &m_WingGraph3;
				m_pCurWingGraph = m_pCurGraph;
				SetWingLegendPos();
				CheckMenus();
			}
			UpdateView();
			return true;
		}
		if ((pMsg->wParam == '4' || pMsg->wParam == 100)&&
			pWnd ==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))){
			if(m_iView==2) {
				m_iWPlrView = 1;
				m_pCurGraph = &m_WPlrGraph4;
				m_pCurWPlrGraph = m_pCurGraph;
				SetWPlrLegendPos();
				CheckMenus();
			}
			else{
				m_iWingView =1;
				m_pCurGraph = &m_WingGraph4;
				m_pCurWingGraph = m_pCurGraph;
				SetWingLegendPos();
				CheckMenus();
			}
			UpdateView();
			return true;
		}
		if (pMsg->wParam == 'T'&& pWnd ==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))){
			if(m_iView==2) {
				m_iWPlrView = 2;
				CheckMenus();
				SetWPlrLegendPos();
			}
			else{
				m_iWingView =2;
				CheckMenus();
				SetWingLegendPos();
			}
			UpdateView();
			return true;
		}
		if (pMsg->wParam == 'A'&& pWnd ==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))){
			if(m_iView==2) {
				m_iWPlrView = 4;
				CheckMenus();
				SetWPlrLegendPos();
			}
			else{
				m_iWingView =4;
				CheckMenus();
				SetWingLegendPos();
			}
			UpdateView();
			return true;
		}

		if (pMsg->wParam == 'N' && !((sh1 & 0x8000)||(sh2 & 0x8000))){
			pFrame->m_WOperDlgBar.OnAnalyze();
			return true;
		}
		if (pMsg->wParam == 'Z' && 
			( (sh1 & 0x8000)||(sh2 & 0x8000) )) { 
			m_pBodyCtrlBar->OnUndo();
			return true;
		} 
		if (pMsg->wParam == 'Y' && 
			( (sh1 & 0x8000)||(sh2 & 0x8000) )) { 
			m_pBodyCtrlBar->OnRedo();
			return true;
		} 

		if (pMsg->wParam == 'Z') { 
			return true;//User is zooming with 'Z' key instead of mouse midle button
		} 
		if (pMsg->wParam == 'V'&& pWnd ==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))){
			CPoint pt;
			GetCursorPos(&pt);
			m_pChildWnd->ScreenToClient( &pt);
			m_pCurGraph = GetGraph(pt);
			if(m_pCurGraph) {
				if (m_iView==2) OnDefineVar();
				else OnWGraphVar();
			}
			return true;
		}
		if (pMsg->wParam == 'G'&& pWnd ==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))){
			CPoint pt;
			GetCursorPos(&pt);
			m_pChildWnd->ScreenToClient( &pt);
			m_pCurGraph = GetGraph(pt);
			if(m_pCurGraph) OnGraphDlg();
			return true;
		}

	}

	return CWnd::PreTranslateMessage(pMsg);
}


void CMiarex::SetParams()
{
	// The Miarex application has been called
	// Update the data
	CMenu *pMenu = m_pFrame->GetMenu();
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pMenu->GetSubMenu(1)->CheckMenuItem(IDM_BSPLINES,      MF_BYCOMMAND | MF_UNCHECKED);
	pMenu->GetSubMenu(1)->CheckMenuItem(IDM_SPLINEDPOINTS, MF_BYCOMMAND | MF_UNCHECKED);
	pMenu->GetSubMenu(1)->CheckMenuItem(IDM_MIXEDINVERSE,  MF_BYCOMMAND | MF_UNCHECKED);
	pMenu->GetSubMenu(1)->CheckMenuItem(IDM_FULLINVERSE,   MF_BYCOMMAND | MF_UNCHECKED);
	pMenu->GetSubMenu(1)->CheckMenuItem(IDM_XFLR5,         MF_BYCOMMAND | MF_UNCHECKED);
	pMenu->GetSubMenu(1)->CheckMenuItem(IDM_MIAREX,        MF_BYCOMMAND | MF_CHECKED);

	if(m_bXTop)	pMenu->GetSubMenu(4)->CheckMenuItem(IDM_TOPTRANS, MF_BYCOMMAND | MF_CHECKED);
	else        pMenu->GetSubMenu(4)->CheckMenuItem(IDM_TOPTRANS, MF_BYCOMMAND | MF_UNCHECKED);

	if(m_bXBot)	pMenu->GetSubMenu(4)->CheckMenuItem(IDM_BOTTRANS, MF_BYCOMMAND | MF_CHECKED);
	else        pMenu->GetSubMenu(4)->CheckMenuItem(IDM_BOTTRANS, MF_BYCOMMAND | MF_UNCHECKED);

	if(m_bXCP)	pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWXCP, MF_BYCOMMAND | MF_CHECKED);
	else        pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWXCP, MF_BYCOMMAND | MF_UNCHECKED);

	if(m_bXCmRef)
		pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWXCMREF, MF_BYCOMMAND | MF_CHECKED);
	else
		pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWXCMREF, MF_BYCOMMAND | MF_UNCHECKED);

	if(m_bCurWOppOnly)
		pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWCURWOPP, MF_BYCOMMAND | MF_CHECKED);
	else
		pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWCURWOPP, MF_BYCOMMAND | MF_UNCHECKED);
	
	if(m_bShowElliptic)
		pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWELLIPTIC, MF_BYCOMMAND | MF_CHECKED);
	else
		pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWELLIPTIC, MF_BYCOMMAND | MF_UNCHECKED);

	CheckMenus();

	if      (m_iWingView==1 && m_iView!=2) m_pCurGraph = m_pCurWingGraph;
	else if (m_iWPlrView==1 && m_iView==2) m_pCurGraph = m_pCurWPlrGraph;

	SetWPlrLegendPos();
	SetWingLegendPos();

	int h   = m_rCltRect.Height();
	int w   = m_rCltRect.Width();
	int h34 = (int)(3*h/4);
	int h38 = (int)(3*h/8);
	int w2  = (int)(w/2);

	CRect Rect1(0,0,w2,h38);
	CRect Rect2(w2,0,w,h38);
	CRect Rect3(0,h38,w2,h34);
	CRect Rect4(w2,h38,w,h34);

	m_WingGraph1.SetDrawRect(Rect1);
	m_WingGraph2.SetDrawRect(Rect2);
	m_WingGraph3.SetDrawRect(Rect3);
	m_WingGraph4.SetDrawRect(Rect4);

	m_WingGraph1.Init();
	m_WingGraph2.Init();
	m_WingGraph3.Init();
	m_WingGraph4.Init();

	CToolBarCtrl *pTB = &(pFrame->m_MiarexBar.GetToolBarCtrl());
	if(m_iView==1){
		pTB->PressButton(IDT_WOPP, true);
		pTB->PressButton(IDT_WPOLARS, false);
		pTB->PressButton(IDM_3DVIEW, false);
		pTB->PressButton(IDM_CPVIEW, false);
		pTB->PressButton(IDM_BODYDESIGN, false);
	}
	else if(m_iView==2){
		pTB->PressButton(IDT_WOPP, false);
		pTB->PressButton(IDT_WPOLARS, true);
		pTB->PressButton(IDM_3DVIEW, false);
		pTB->PressButton(IDM_CPVIEW, false);
		pTB->PressButton(IDM_BODYDESIGN, false);
	}
	else if(m_iView==3){
		pTB->PressButton(IDM_3DVIEW, true);
		pTB->PressButton(IDT_WOPP, false);
		pTB->PressButton(IDT_WPOLARS, false);
		pTB->PressButton(IDM_CPVIEW, false);
		pTB->PressButton(IDM_BODYDESIGN, false);
	}
	else if(m_iView==4){
		pTB->PressButton(IDT_WOPP, false);
		pTB->PressButton(IDT_WPOLARS, false);
		pTB->PressButton(IDM_3DVIEW, false);
		pTB->PressButton(IDM_CPVIEW, true);
		pTB->PressButton(IDM_BODYDESIGN, false);
	}
	else if(m_iView==5){
		pTB->PressButton(IDT_WOPP, false);
		pTB->PressButton(IDT_WPOLARS, false);
		pTB->PressButton(IDM_3DVIEW, false);
		pTB->PressButton(IDM_CPVIEW, false);
		pTB->PressButton(IDM_BODYDESIGN, true);
	}
		
	CheckMenus();

	SetWGraphTitles(&m_WPlrGraph1, m_XW1,m_YW1);
	SetWGraphTitles(&m_WPlrGraph2, m_XW2,m_YW2);
	SetWGraphTitles(&m_WPlrGraph3, m_XW3,m_YW3);
	SetWGraphTitles(&m_WPlrGraph4, m_XW4,m_YW4);

	m_bStream = false;
	m_FlowLinesDlg.m_pChildWnd = m_pChildWnd;
	m_FlowLinesDlg.SetParams();

	StopAnimate();
}


void CMiarex::OnRenameWPolar() 
{
	//Rename the currently selected Wing Polar

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(!m_pCurWPolar) return;
	if(!m_pCurWing) return;
	int resp, k,l;
	CWPolar* pWPolar;
	CPOpp * pPOpp;
	CWOpp * pWOpp;
	CString OldName = m_pCurWPolar->m_PlrName;
	CString UFOName;

	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;

	CStringArray NameList;
	for(k=0; k<m_poaWPolar->GetSize(); k++){
		pWPolar = (CWPolar*)m_poaWPolar->GetAt(k);
		if(pWPolar->m_UFOName==UFOName) NameList.Add(pWPolar->m_PlrName);
	}
	CNameDlg dlg(this);
	dlg.m_pStrArray = & NameList;

	dlg.m_strQuestion = "Enter the new name for the wing polar :";
	dlg.m_strName     = m_pCurWPolar->m_PlrName;

	bool bExists = true;

	while (bExists){
		resp = (int)dlg.DoModal();
		if(resp==IDOK){
			if (OldName == dlg.m_strName) return;
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaWPolar->GetSize(); k++){
				pWPolar = (CWPolar*)m_poaWPolar->GetAt(k);
				if (pWPolar->m_PlrName == dlg.m_strName &&
					pWPolar->m_UFOName == m_pCurWing->m_WingName){
					bExists = true;
					break;
				}
			}
			if(!bExists){
				m_pCurWPolar->m_PlrName = dlg.m_strName;
				if(m_pCurPlane){
					for (l=(int)m_poaPOpp->GetSize()-1;l>=0; l--){
						pPOpp = (CPOpp*)m_poaPOpp->GetAt(l);
						if (pPOpp->m_PlrName == OldName &&
							pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName){
							pPOpp->m_PlrName = dlg.m_strName;
						}
					}
				}
				for (l=(int)m_poaWOpp->GetSize()-1;l>=0; l--){
					pWOpp = (CWOpp*)m_poaWOpp->GetAt(l);
					if (pWOpp->m_PlrName == OldName &&
						pWOpp->m_WingName == m_pCurWing->m_WingName){
						pWOpp->m_PlrName = dlg.m_strName;
					}
				}
			}
			//put the Wplr at its new place in alphabetical order
			//so find the current polar's index
			for (k=0; k<m_poaWPolar->GetSize(); k++)
				if(m_pCurWPolar==m_poaWPolar->GetAt(k)) break;
			
			if(k<m_poaWPolar->GetSize()){//you never know
				//remove the WPolar
				m_poaWPolar->RemoveAt(k);
				//And re-insert it
				bool bInserted = false;
				for (k=0; k<m_poaWPolar->GetSize(); k++){
					pWPolar = (CWPolar*)m_poaWPolar->GetAt(k);
					if (m_pCurWPolar->m_PlrName.CompareNoCase(pWPolar->m_PlrName)<0 &&
						pWPolar->m_UFOName == m_pCurWing->m_WingName){
						m_poaWPolar->InsertAt(k, m_pCurWPolar);
						bInserted = true;
						break;
					}
				}
				if(!bInserted){
					m_poaWPolar->Add(m_pCurWPolar);
				}
			}


			pFrame->SetSaveState(false);
		}
		else if(resp ==10){//user wants to overwrite
			if (OldName == dlg.m_strName) return;
			for (k=0; k<m_poaWPolar->GetSize(); k++){
				pWPolar = (CWPolar*)m_poaWPolar->GetAt(k);
				if (pWPolar->m_PlrName == dlg.m_strName &&
					pWPolar->m_UFOName == m_pCurWing->m_WingName){
					bExists = true;
					break;
				}
			}
			for (l=(int)m_poaWOpp->GetSize()-1;l>=0; l--){
				pWOpp = (CWOpp*)m_poaWOpp->GetAt(l);
				if (pWOpp->m_PlrName == pWPolar->m_PlrName &&
					pWOpp->m_WingName == m_pCurWing->m_WingName){
					m_poaWOpp->RemoveAt(l);
					if(pWOpp==m_pCurWOpp) {
						m_pCurPOpp = NULL;
						m_pCurWOpp = NULL;
					}
					delete pWOpp;
				}
			}
			m_poaWPolar->RemoveAt(k);
			if(pWPolar==m_pCurWPolar){
				m_pCurWPolar = NULL;
				pFrame->m_WOperDlgBar.EnableAnalysis(false);
			}
			delete pWPolar;

			//and rename everything
			m_pCurWPolar->m_PlrName = dlg.m_strName;

			for (l=(int)m_poaWOpp->GetSize()-1;l>=0; l--){
				pWOpp = (CWOpp*)m_poaWOpp->GetAt(l);
				if (pWOpp->m_PlrName == OldName &&
					pWOpp->m_WingName == m_pCurWing->m_WingName){
					pWOpp->m_PlrName = dlg.m_strName;
				}
			}

			bExists = false;
			pFrame->SetSaveState(false);
		}
		else {
			return ;//cancelled 
		}
	}
	pFrame->UpdateWPlrs();
//	SetWPlr();
	UpdateView();
}

void CMiarex::OnRenameUFO()
{
	//Rename the currently selected UFO
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
 
	if(!m_pCurWing)	return;
	if(m_pCurPlane)		 RenameUFO(m_pCurPlane->m_PlaneName);
	else if (m_pCurWing) RenameUFO(m_pCurWing->m_WingName);
	pFrame->UpdateUFOs();
	m_bResetglLegend = true;
	UpdateView();
}

CString CMiarex::RenameUFO(CString UFOName)
{
	//Rename the currently selected UFO
	CString OldName;
	CWOpp *pWOpp;
	CPOpp *pPOpp;
	int l;
	CWPolar *pWPolar;
	CPlane *pPlane = GetPlane(UFOName);
	CWing *pWing   = GetWing(UFOName);
	if(pPlane) {
		OldName = pPlane->m_PlaneName;
		SetModPlane(pPlane);
		for (l=(int)m_poaWPolar->GetSize()-1;l>=0; l--){
			pWPolar = (CWPolar*)m_poaWPolar->GetAt(l);
			if (pWPolar->m_UFOName == OldName){
				pWPolar->m_UFOName = pPlane->m_PlaneName;
			}
		}
		for (l=(int)m_poaPOpp->GetSize()-1;l>=0; l--){
			pPOpp = (CPOpp*)m_poaPOpp->GetAt(l);
			if (pPOpp->m_PlaneName == OldName){
				pPOpp->m_PlaneName = pPlane->m_PlaneName;
			}
		}
		return pPlane->m_PlaneName;
	}
	else if(pWing) {
		OldName = pWing->m_WingName;
		SetModWing(pWing);
		for (l=(int)m_poaWPolar->GetSize()-1;l>=0; l--){
			pWPolar = (CWPolar*)m_poaWPolar->GetAt(l);
			if (pWPolar->m_UFOName == OldName){
				pWPolar->m_UFOName = pWing->m_WingName;
			}
		}
		for (l=(int)m_poaWOpp->GetSize()-1;l>=0; l--){
			pWOpp = (CWOpp*)m_poaWOpp->GetAt(l);
			if (pWOpp->m_WingName == OldName){
				pWOpp->m_WingName = pWing->m_WingName;
			}
		}
		return pWing->m_WingName;
	}
	return "";
}

void CMiarex::OnDelCurWOpp()
{
	//Deletes the currently selected WOpp or POpp
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	int i;
	if(m_pCurPOpp){
		CPOpp* pPOpp;
		for (i = (int)m_poaPOpp->GetSize()-1; i>=0; i--){
			pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
			if(pPOpp == m_pCurPOpp){
				m_poaPOpp->RemoveAt(i);
				delete pPOpp;
				break;
			}
		}
		m_pCurPOpp = NULL;
		m_pCurWOpp = NULL;
		pFrame->UpdateWOpps();
		SetWOpp(true);
		pFrame->SetSaveState(false);
		if (m_iView==1)     CreateWOppCurves();
		else if(m_iView==4)	CreateCpCurves();

		UpdateView();	
	}

	else if(m_pCurWOpp) {
		CWOpp* pWOpp;
		for (i = (int)m_poaWOpp->GetSize()-1; i>=0; i--){
			pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
			if(pWOpp == m_pCurWOpp){
				m_poaWOpp->RemoveAt(i);
				delete pWOpp;
				m_pCurWOpp = NULL;
				break;
			}
		}
		pFrame->UpdateWOpps();
		SetWOpp(true);
		if(pFrame->m_ctrlWOpp.GetCount()){
			CString strong;
			double x;
			pFrame->m_ctrlWOpp.SetCurSel(0);
			pFrame->m_ctrlWOpp.GetLBText(0, strong);
			int res = sscanf(strong, "%lf", &x);
			if(res==1) {
				m_pCurWOpp = GetWOpp(x);
			}
			else m_pCurWOpp = NULL;
		}
		else{
			m_pCurWOpp = NULL;
		}
		pFrame->SetSaveState(false);
		if (m_iView==1)     CreateWOppCurves();
		else if(m_iView==4)	CreateCpCurves();

		UpdateView();	
	}
}

void CMiarex::OnDeleteAll() 
{
	//Delete all the WOpps or POpps
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(false);
	CWOpp* pWOpp;
	CPOpp* pPOpp;
	int i;
	for (i = (int)m_poaPOpp->GetSize()-1; i>=0; i--){
		pPOpp = (CPOpp*) m_poaPOpp->GetAt(i);
		m_poaPOpp->RemoveAt(i);
		delete pPOpp;
	}
	for (i = (int)m_poaWOpp->GetSize()-1; i>=0; i--){
		pWOpp = (CWOpp*) m_poaWOpp->GetAt(i);
		m_poaWOpp->RemoveAt(i);
		delete pWOpp;
	}

	
	m_pCurWOpp = NULL;
	m_pCurPOpp = NULL;
	pFrame->UpdateWOpps();
	SetWOpp(true);
	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();
	UpdateView();	
}

void CMiarex::OnDelAllPlrWOpps() 
{
	//Delete all the WOpps or POpps corresponding to the current UFO
	if(!m_pCurPlane && !m_pCurWing) return;
	if(!m_pCurWPolar) return;
	m_bAnimate = false;
	int i;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(false);
	CWOpp* pWOpp;
	CPOpp* pPOpp;

	if(m_pCurPlane){
		for (i = (int)m_poaPOpp->GetSize()-1; i>=0; i--){
			pPOpp = (CPOpp*) m_poaPOpp->GetAt(i);
			if (pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName &&
				pPOpp->m_PlrName  == m_pCurWPolar->m_PlrName){
				m_poaPOpp->RemoveAt(i);
				delete pPOpp;
			}
		}
	}
	else if (m_pCurWing){
		for (i = (int)m_poaWOpp->GetSize()-1; i>=0; i--){
			pWOpp = (CWOpp*) m_poaWOpp->GetAt(i);
			if (pWOpp->m_WingName == m_pCurWing->m_WingName &&
				pWOpp->m_PlrName  == m_pCurWPolar->m_PlrName){
				m_poaWOpp->RemoveAt(i);
				delete pWOpp;
			}
		}
	}
	m_pCurWOpp = NULL;
	m_pCurPOpp = NULL;
	pFrame->UpdateWOpps();
	SetWOpp(true);
	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();
	UpdateView();
}


void CMiarex::OnDelAllWOpps() 
{
	//Delete all the WOpps or POpps
	int i;
	if(!m_pCurPlane && !m_pCurWing) return;
	if(!m_pCurWPolar) return;
	if(!m_pCurWPolar) return;
	m_bAnimate = false;

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(false);
	CWOpp* pWOpp;
	CPOpp* pPOpp;
	if(m_pCurPlane){
		for (i = (int)m_poaPOpp->GetSize()-1; i>=0; i--){
			pPOpp = (CPOpp*) m_poaPOpp->GetAt(i);
			if (pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName){
				m_poaPOpp->RemoveAt(i);
				delete pPOpp;
			}
		}
	}
	else if (m_pCurWing){
		for (i = (int)m_poaWOpp->GetSize()-1; i>=0; i--){
			pWOpp = (CWOpp*) m_poaWOpp->GetAt(i);
			if (pWOpp->m_WingName == m_pCurWing->m_WingName){
				m_poaWOpp->RemoveAt(i);
				delete pWOpp;
			}
		}
	}
	m_pCurPOpp = NULL;
	m_pCurWOpp = NULL;
	pFrame->UpdateWOpps();
	SetWOpp(true);
	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();
	UpdateView();
}


void CMiarex::OnDeleteWPolars() 
{
	//Delete the current's UFO WOpps or POpps
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	m_bAnimate = false;
	int i;
	//delete all WPolars associated to the current wing/plane
	if(!m_pCurWing)		return;
	
	CString UFOName, strong;
	if(m_pCurPlane) {
		UFOName = m_pCurPlane->m_PlaneName;
		strong = "Are you sure you want to delete the polars associated to the plane :\n" +  UFOName +"?\n";
	}
	else      {
		UFOName = m_pCurWing->m_WingName;
		strong = "Are you sure you want to delete the polars associated to the wing :\n" +  UFOName +"?\n";
	}

	if(IDYES != AfxMessageBox(strong, MB_YESNOCANCEL)) return;

	//first remove all POpps and WOpps associated to the wing/plane
	CWOpp * pWOpp;
	CPOpp * pPOpp;

	for (i=(int)m_poaPOpp->GetSize()-1; i>=0; i--){
		pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
		if(pPOpp->m_PlaneName == UFOName){
			m_poaPOpp->RemoveAt(i);
			delete pPOpp;
		}
	}


	for (i=(int)m_poaWOpp->GetSize()-1; i>=0; i--){
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		if(pWOpp->m_WingName == UFOName){
			m_poaWOpp->RemoveAt(i);
			delete pWOpp;
		}
	}
	m_pCurPOpp = NULL;
	m_pCurWOpp = NULL;

	//next remove all WPolars associated to the wing
	CWPolar* pWPolar;
	for (i=(int)m_poaWPolar->GetSize()-1; i>=0; i--){
		pWPolar = (CWPolar*)m_poaWPolar->GetAt(i);
		if (pWPolar->m_UFOName == UFOName){
			m_poaWPolar->RemoveAt(i);
			delete pWPolar;

		}
	}
	m_pCurWPolar = NULL;
	pFrame->m_WOperDlgBar.EnableAnalysis(false);
	pFrame->SetSaveState(false);
	pFrame->UpdateWPlrs();
	SetWPlr();
	UpdateView();
}


void CMiarex::OnDeleteWPolar() 
{
	//delete the current WPolar
	m_bAnimate = false;
	int i;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(!m_pCurWPolar) return;
	CString strong;
	CString UFOName;
	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return;

	strong = "Are you sure you want to delete the polar :\n" +  m_pCurWPolar->m_PlrName +"?\n";
	if(IDYES != AfxMessageBox(strong, MB_YESNOCANCEL)) return;

	//first remove all WOpps associated to the Wing Polar
	CWOpp * pWOpp;
	for (i=(int)m_poaWOpp->GetSize()-1; i>=0; i--)
	{
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		if (pWOpp->m_PlrName  == m_pCurWPolar->m_PlrName &&	pWOpp->m_WingName == UFOName)
		{
			m_poaWOpp->RemoveAt(i);
			delete pWOpp;
		}
	}
	//next remove all the POpps associated to the Wing Polar
	CPOpp * pPOpp;
	for (i=(int)m_poaPOpp->GetSize()-1; i>=0; i--)
	{
		pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
		if (pPOpp->m_PlrName  == m_pCurWPolar->m_PlrName &&	pPOpp->m_PlaneName == UFOName)
		{
			m_poaPOpp->RemoveAt(i);
			delete pPOpp;
		}
	}
	//next remove the WPolar
	CWPolar* pWPolar;
	for (i=(int)m_poaWPolar->GetSize()-1;i>=0; i--)
	{
		pWPolar = (CWPolar*)m_poaWPolar->GetAt(i);
		if (pWPolar == m_pCurWPolar)
		{
			m_poaWPolar->RemoveAt(i);
			delete pWPolar;

			pFrame->m_WOperDlgBar.EnableAnalysis(false);
			break;
		}
	}
	m_pCurPOpp = NULL;
	m_pCurWOpp = NULL;
	m_pCurWPolar = NULL;
	pFrame->SetSaveState(false);
	pFrame->UpdateWPlrs();
	SetWPlr();
	UpdateView();
}


bool CMiarex::SetMiarexCursor(CWnd* pWnd, CPoint ptMouse, UINT message) 
{
	//Sets the cursor depending on the view
	if (pWnd != m_pChildWnd) return false;

	if(m_iView==1){
		if(m_bTrans) {
			SetCursor(m_hcMove);
			return TRUE;
		}
		bool bZ = false;
		if(m_iWingView == 1) {
			if(m_WingGraph1.IsInDrawRect(ptMouse)) 
				bZ = true;
		}
		else if(m_iWingView == 2){
			if (m_WingGraph1.IsInDrawRect(ptMouse) ||
				m_WingGraph2.IsInDrawRect(ptMouse) ){
					bZ = true;
			}
		}
		else if (m_iWingView == 4){
			if (m_WingGraph1.IsInDrawRect(ptMouse) ||
				m_WingGraph2.IsInDrawRect(ptMouse) ||
				m_WingGraph3.IsInDrawRect(ptMouse) ||
				m_WingGraph4.IsInDrawRect(ptMouse)){
					bZ = true;
			}
		}		
		if(bZ){
			SetCursor(m_hcCross);
			return TRUE;
		}
		else {
			SetCursor(m_hcArrow);
			return TRUE;	
		}
	}

	else if(m_iView==2){
		if(m_bTrans) {
			SetCursor(m_hcMove);
			return TRUE;
		}
		bool bZ = false;
		if(m_iWPlrView == 1) {
			if(m_WPlrGraph1.IsInDrawRect(ptMouse)) 
				bZ = true;
		}
		else if(m_iWPlrView == 2){
			if (m_WPlrGraph1.IsInDrawRect(ptMouse) ||
				 m_WPlrGraph2.IsInDrawRect(ptMouse) ){
					bZ = true;
			}
		}
		else if (m_iWPlrView == 4){
			if (m_WPlrGraph1.IsInDrawRect(ptMouse) ||
				 m_WPlrGraph2.IsInDrawRect(ptMouse)    ||
				 m_WPlrGraph3.IsInDrawRect(ptMouse)    ||
				 m_WPlrGraph4.IsInDrawRect(ptMouse)){
					bZ = true;
			}
		}		
		if(bZ){
			SetCursor(m_hcCross);
			return TRUE;
		}
		else {
			SetCursor(m_hcArrow);
			return TRUE;	
		}
	}

	else if(m_iView==4)
	{
		if(m_bTrans) 
		{
			SetCursor(m_hcMove);
			return TRUE;
		}
	
		if(m_WPlrGraph1.IsInDrawRect(ptMouse)){
			SetCursor(m_hcCross);
			return TRUE;
		}
		else {
			SetCursor(m_hcArrow);
			return TRUE;	
		}
	}

	else{//m_iView=3 || 5
		if(m_bTrans) 
		{
			SetCursor(m_hcMove);
			return TRUE;
		}
		else
		{
			SetCursor(m_hcCross);
			return TRUE;
		}
	}
//	return true;
}


void CMiarex::OnAbout() 
{
	((CXFLR5App*)AfxGetApp())->OnAppAbout();
}


void CMiarex::OnExportWOpp() 
{
	//Exprt the currently selected WOpp to the text file
	if(!m_pCurWOpp)return ;// is there anything to export ?

	int l,p;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString FileName,str;
	CStdioFile XFile;
	CFileException fe;
	CFileDialog XFileDlg(false, "txt", NULL, OFN_OVERWRITEPROMPT, _T("Text Format (.txt)|*.txt|"));
	if(IDOK == XFileDlg.DoModal()){
		FileName = XFileDlg.GetFileName();
		BOOL bOpen = XFile.Open(FileName, CFile::modeCreate | CFile::modeWrite, &fe);//erase and write
		if(!bOpen) return;
		
		CString strOut;
		CString Header, strong;
		XFile.WriteString(pFrame->m_VersionName);
		XFile.WriteString("\n\n");

		if(m_pCurPOpp)		XFile.WriteString(m_pCurPOpp->m_PlaneName+ "\n");
		else if(m_pCurWOpp)	XFile.WriteString(m_pCurWOpp->m_WingName+ "\n");
		
		strong = m_pCurWOpp->m_PlrName + "\n";
		XFile.WriteString(strong);
		strong.Format("QInf  = %9.4f ",	m_pCurWOpp->m_QInf*pFrame->m_mstoUnit);
		GetSpeedUnit(str, pFrame->m_SpeedUnit);
		strong+=str+"\n";

		XFile.WriteString(strong);
		strong.Format("Alpha = %9.4f\n",	m_pCurWOpp->m_Alpha);
		XFile.WriteString(strong);

		strong.Format("Cl    = %9.4f\n",	m_pCurWOpp->m_CL);
		XFile.WriteString(strong);

		strong.Format("Cd    = %9.4f     ICd   = %9.4f     PCd   = %9.4f\n",
			m_pCurWOpp->m_InducedDrag+m_pCurWOpp->m_ViscousDrag, m_pCurWOpp->m_InducedDrag,	m_pCurWOpp->m_ViscousDrag);
		XFile.WriteString(strong);

		strong.Format("GCm    = %9.4f\n", m_pCurWOpp->m_GCm);
		XFile.WriteString(strong);

		strong.Format("GRm    = %9.4f\n", m_pCurWOpp->m_GRm);
		XFile.WriteString(strong);

		strong.Format("IYm   = %9.4f     PYm   = %9.4f \n",
			m_pCurWOpp->m_IYm, m_pCurWOpp->m_GYm);
		XFile.WriteString(strong);

		strong.Format("XCP   = %9.4f     YCP   = %9.4f \n",
			m_pCurWOpp->m_XCP, m_pCurWOpp->m_YCP);
		XFile.WriteString(strong);

		strong.Format("Bend. = %9.4f\n\n",m_pCurWOpp->m_MaxBending);
		XFile.WriteString(strong);
		
		XFile.WriteString("Main Wing Data\n");
		for (l=0; l<m_pCurWOpp->m_nFlaps; l++){
	 		strong.Format("Flap %d moment = %9.4f",l+1, m_pCurWOpp->m_FlapMoment[l]*pFrame->m_NmtoUnit);
			GetMomentUnit(str, pFrame->m_MomentUnit);
			strong += str +"\n";
			XFile.WriteString(strong);      
		}
		XFile.WriteString("\n");
		m_pCurWOpp->Export(&XFile);

		if(m_pCurWing2) {
			XFile.WriteString("Secondary Wing Data\n");
			for (l=0; l<m_pCurPOpp->m_Wing2WOpp.m_nFlaps; l++){
	 			strong.Format("Flap %d moment = %9.4f ",l+1, m_pCurPOpp->m_Wing2WOpp.m_FlapMoment[l]*pFrame->m_NmtoUnit);
				GetMomentUnit(str, pFrame->m_MomentUnit);
				strong += str +"\n";
				XFile.WriteString(strong);      
			}
			XFile.WriteString("\n");
			m_pCurPOpp->m_Wing2WOpp.Export(&XFile);
		}

		if(m_pCurStab) {
			XFile.WriteString("Elevator Data\n");
			for (l=0; l<m_pCurPOpp->m_StabWOpp.m_nFlaps; l++){
	 			strong.Format("Flap %d moment = %9.4f ",l+1, m_pCurPOpp->m_StabWOpp.m_FlapMoment[l]*pFrame->m_NmtoUnit);
				GetMomentUnit(str, pFrame->m_MomentUnit);
				strong += str +"\n";
				XFile.WriteString(strong);      
			}
			XFile.WriteString("\n");
			m_pCurPOpp->m_StabWOpp.Export(&XFile);
		}

		if(m_pCurFin) {
			XFile.WriteString("Fin Data\n");
			for (l=0; l<m_pCurPOpp->m_FinWOpp.m_nFlaps; l++){
	 			strong.Format("Flap %d moment = %9.4f",l+1, m_pCurPOpp->m_FinWOpp.m_FlapMoment[l]*pFrame->m_NmtoUnit);
				GetMomentUnit(str, pFrame->m_MomentUnit);
				strong += str +"\n";
				XFile.WriteString(strong);      
			}
			XFile.WriteString("\n");
			m_pCurPOpp->m_FinWOpp.Export(&XFile);
		}


		if(m_pCurWOpp->m_AnalysisType>=2)
		{
			if(m_pCurPOpp)
			{
				XFile.WriteString("Main Wing Cp Coefficients\n");

				if(m_pCurWOpp->m_AnalysisType==2)		XFile.WriteString(" Panel     CtrlPt.x        CtrlPt.y        CtrlPt.z             Cp\n");
				else if(m_pCurWOpp->m_AnalysisType==3)	XFile.WriteString(" Panel     CollPt.x        CollPt.y        CollPt.z             Cp\n");

				for(p=0; p<m_pCurWing->m_MatSize; p++)
				{
					if(m_pCurWOpp->m_AnalysisType==2)
						strong.Format("%4d     %11.3e     %11.3e     %11.3e     %11.3f\n", p, 
							m_Panel[p].CtrlPt.x, m_Panel[p].CtrlPt.y, m_Panel[p].CtrlPt.z, 
							m_pCurWOpp->m_Cp[p]);
					else if(m_pCurWOpp->m_AnalysisType==3)
						strong.Format("%4d     %11.3e     %11.3e     %11.3e     %11.3f\n", p, 
							m_Panel[p].CollPt.x, m_Panel[p].CollPt.y, m_Panel[p].CollPt.z, 
							m_pCurWOpp->m_Cp[p]);
					XFile.WriteString(strong);
				}
				XFile.WriteString("\n");

				if(m_pCurWing2)
				{
					XFile.WriteString("Secondary Wing Cp Coefficients\n");

					if(m_pCurWOpp->m_AnalysisType==2)		XFile.WriteString(" Panel     CtrlPt.x        CtrlPt.y        CtrlPt.z             Cp\n");
					else if(m_pCurWOpp->m_AnalysisType==3)	XFile.WriteString(" Panel     CollPt.x        CollPt.y        CollPt.z             Cp\n");
					for(p=0; p<m_pCurWing2->m_MatSize; p++)
					{
						if(m_pCurWOpp->m_AnalysisType==2)
							strong.Format("%4d     %11.3e     %11.3e     %11.3e     %11.3f\n", p, 
								m_pCurWing2->m_pPanel[p].CtrlPt.x, m_pCurWing2->m_pPanel[p].CtrlPt.y, m_pCurWing2->m_pPanel[p].CtrlPt.z, 
								m_pCurPOpp->m_Wing2WOpp.m_Cp[p]);
						else if(m_pCurWOpp->m_AnalysisType==3)
							strong.Format("%4d     %11.3e     %11.3e     %11.3e     %11.3f\n", p, 
								m_pCurWing2->m_pPanel[p].CollPt.x, m_pCurWing2->m_pPanel[p].CollPt.y, m_pCurWing2->m_pPanel[p].CollPt.z, 
								m_pCurPOpp->m_Wing2WOpp.m_Cp[p]);
						XFile.WriteString(strong);
					}
				}
				XFile.WriteString("\n");

				if(m_pCurStab)
				{
					XFile.WriteString("Elevator Cp Coefficients\n");

					if(m_pCurWOpp->m_AnalysisType==2)		XFile.WriteString(" Panel     CtrlPt.x        CtrlPt.y        CtrlPt.z             Cp\n");
					else if(m_pCurWOpp->m_AnalysisType==3)	XFile.WriteString(" Panel     CollPt.x        CollPt.y        CollPt.z             Cp\n");
					for(p=0; p<m_pCurStab->m_MatSize; p++)
					{
						if(m_pCurWOpp->m_AnalysisType==2)
							strong.Format("%4d     %11.3e     %11.3e     %11.3e     %11.3f\n", p, 
								m_pCurStab->m_pPanel[p].CtrlPt.x, m_pCurStab->m_pPanel[p].CtrlPt.y, m_pCurStab->m_pPanel[p].CtrlPt.z, 
								m_pCurPOpp->m_StabWOpp.m_Cp[p]);
						else if(m_pCurWOpp->m_AnalysisType==3)
							strong.Format("%4d     %11.3e     %11.3e     %11.3e     %11.3f\n", p, 
								m_pCurStab->m_pPanel[p].CollPt.x, m_pCurStab->m_pPanel[p].CollPt.y, m_pCurStab->m_pPanel[p].CollPt.z, 
								m_pCurPOpp->m_StabWOpp.m_Cp[p]);
						XFile.WriteString(strong);
					}
				}
				XFile.WriteString("\n");

				if(m_pCurFin)
				{
					XFile.WriteString("Fin Cp Coefficients\n");

					if(m_pCurWOpp->m_AnalysisType==2)		XFile.WriteString(" Panel     CtrlPt.x        CtrlPt.y        CtrlPt.z             Cp\n");
					else if(m_pCurWOpp->m_AnalysisType==3)	XFile.WriteString(" Panel     CollPt.x        CollPt.y        CollPt.z             Cp\n");
					for(p=0; p<m_pCurFin->m_MatSize; p++)
					{
						if(m_pCurWOpp->m_AnalysisType==2)
							strong.Format("%4d     %11.3e     %11.3e     %11.3e     %11.3f\n", p, 
								m_pCurFin->m_pPanel[p].CtrlPt.x, m_pCurFin->m_pPanel[p].CtrlPt.y, m_pCurFin->m_pPanel[p].CtrlPt.z, 
								m_pCurPOpp->m_FinWOpp.m_Cp[p]);
						else if(m_pCurWOpp->m_AnalysisType==3)
							strong.Format("%4d     %11.3e     %11.3e     %11.3e     %11.3f\n", p, 
								m_pCurFin->m_pPanel[p].CollPt.x, m_pCurFin->m_pPanel[p].CollPt.y, m_pCurFin->m_pPanel[p].CollPt.z, 
								m_pCurPOpp->m_FinWOpp.m_Cp[p]);
						XFile.WriteString(strong);
					}
				}
				XFile.WriteString("\n");

			}
			else if(m_pCurWOpp)
			{
				if(m_pCurWOpp->m_AnalysisType==2)		XFile.WriteString(" Panel     CtrlPt.x        CtrlPt.y        CtrlPt.z             Cp\n");
				else if(m_pCurWOpp->m_AnalysisType==3)	XFile.WriteString(" Panel     CollPt.x        CollPt.y        CollPt.z             Cp\n");
				for(p=0; p<m_MatSize; p++)
				{
					if(m_pCurWOpp->m_AnalysisType==2)
						strong.Format("%4d     %11.3e     %11.3e     %11.3e     %11.3f\n", p, 
							m_Panel[p].CtrlPt.x, m_Panel[p].CtrlPt.y, m_Panel[p].CtrlPt.z, 
							m_pCurWOpp->m_Cp[p]);
					else if(m_pCurWOpp->m_AnalysisType==3)
						strong.Format("%4d     %11.3e     %11.3e     %11.3e     %11.3f\n", p, 
							m_Panel[p].CollPt.x, m_Panel[p].CollPt.y, m_Panel[p].CollPt.z, 
							m_pCurWOpp->m_Cp[p]);
					XFile.WriteString(strong);
				}
			}
		}
		XFile.WriteString("\n\n");

		XFile.Close();
	}
}


void CMiarex::OnWPolarReset() 
{
	//Reset the current WPolar's data
	if(m_pCurWPolar) {
		if(IDYES==AfxMessageBox("Reset the polar's content ?", MB_YESNOCANCEL)){
			m_pCurWPolar->ResetWPlr();
			CreateWPolarCurves();
			if(m_iView==2) UpdateView();
		}
	}
}


void CMiarex::DeleteProject()
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->DeleteProject();
}

void CMiarex::OnInsertProject() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->OnInsertProject();
}

void CMiarex::OnLoadProject() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->OnLoadProject();
}

void CMiarex::OnSaveProject() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->OnSaveProject();
}

void CMiarex::OnSaveAs() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->OnSaveProjectAs();
}

void CMiarex::OnNewProject() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->OnNewProject();
}

void CMiarex::OnAdjustToWing() 
{
	//Adjust the graph's scale to the wing
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(!m_pCurWing) return;

	double halfspan = m_pCurWing->m_Span/2.0;
	m_WingGraph1.SetAutoX(false);
	m_WingGraph1.SetXMax( halfspan*pFrame->m_mtoUnit);
	m_WingGraph2.SetAutoX(false);
	m_WingGraph2.SetXMax( halfspan*pFrame->m_mtoUnit);
	m_WingGraph3.SetAutoX(false);
	m_WingGraph3.SetXMax( halfspan*pFrame->m_mtoUnit);
	m_WingGraph4.SetAutoX(false);
	m_WingGraph4.SetXMax( halfspan*pFrame->m_mtoUnit);

	double xmin = -halfspan*pFrame->m_mtoUnit;
	if(m_bHalfWing) xmin = 0.0;
	m_WingGraph1.SetXMin(xmin);
	m_WingGraph2.SetXMin(xmin);
	m_WingGraph3.SetXMin(xmin);
	m_WingGraph4.SetXMin(xmin);

	UpdateView();
}

void CMiarex::OnEditWPlr() 
{
	//Edit the current WPolar data 
	if (!m_pCurWPolar) return;
	if(m_iView==5) return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CWPolar MemWPolar;
	MemWPolar.Copy(m_pCurWPolar);
	CEditPlrDlg dlg(this);
	dlg.m_pWPolar = m_pCurWPolar;
	dlg.m_pParent = this;

	bool bPoints = m_pCurWPolar->m_bShowPoints;
	m_pCurWPolar->m_bShowPoints = true;
	CreateWPolarCurves();
	UpdateView();

	if(dlg.DoModal() == IDOK){
		pFrame->SetSaveState(false);
	}
	else{
		m_pCurWPolar->Copy(&MemWPolar);
	}
	m_pCurWPolar->m_bShowPoints = bPoints;
	CreateWPolarCurves();
	pFrame->UpdateWOpps();
	SetWOpp(true);
	UpdateView();
}



double CMiarex::GetZeroLiftAngle(CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau)
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
	else{
		pPolar1 = NULL;
		pPolar2 = NULL;
		for (i=0; i<m_poaPolar->GetSize(); i++){
			pPolar = (CPolar*)m_poaPolar->GetAt(i);
			if(pPolar->m_FoilName == pFoil0->m_FoilName) size++;
		}
		if(size){
			for (i=0; i<m_poaPolar->GetSize(); i++){
				pPolar = (CPolar*)m_poaPolar->GetAt(i);
				if(pPolar->m_FoilName == pFoil0->m_FoilName) {
					if(pPolar->m_Reynolds < Re) pPolar1 = pPolar;
				}
			}
			for (i=0; i<m_poaPolar->GetSize(); i++){
				pPolar = (CPolar*)m_poaPolar->GetAt(i);
				if(pPolar->m_FoilName == pFoil0->m_FoilName) {
					if(pPolar->m_Reynolds > Re) {
						pPolar2 = pPolar;
						break;
					}
				}
			}
		}
		if(pPolar1 && pPolar2){
			a01 = pPolar1->GetZeroLiftAngle();
			a02 = pPolar2->GetZeroLiftAngle();
			Alpha00 = a01 + (a02-a01) * (Re-pPolar1->m_Reynolds)/(pPolar2->m_Reynolds-pPolar1->m_Reynolds);
		}
		else Alpha00 = 0.0;
	}

	if(!pFoil1) Alpha01 = 0.0;
	else{
		pPolar1 = NULL;
		pPolar2 = NULL;
		for (i=0; i<m_poaPolar->GetSize(); i++){
			pPolar = (CPolar*)m_poaPolar->GetAt(i);
			if(pPolar->m_FoilName == pFoil1->m_FoilName) size++;
		}
		if(size){
			for (i=0; i<m_poaPolar->GetSize(); i++){
				pPolar = (CPolar*)m_poaPolar->GetAt(i);
				if(pPolar->m_FoilName == pFoil1->m_FoilName) {
					if(pPolar->m_Reynolds < Re) pPolar1 = pPolar;
				}
			}
			for (i=0; i<m_poaPolar->GetSize(); i++){
				pPolar = (CPolar*)m_poaPolar->GetAt(i);
				if(pPolar->m_FoilName == pFoil1->m_FoilName) {
					if(pPolar->m_Reynolds > Re) {
						pPolar2 = pPolar;
						break;
					}
				}
			}
		}
		if(pPolar1 && pPolar2){
			a01 = pPolar1->GetZeroLiftAngle();
			a02 = pPolar2->GetZeroLiftAngle();
			Alpha01 = a01 + (a02-a01) * (Re-pPolar1->m_Reynolds)/(pPolar2->m_Reynolds-pPolar1->m_Reynolds);
		}
		else Alpha01 = 0.0;
	}
	
	return ((1-Tau) * Alpha00 + Tau * Alpha01);
}


void CMiarex::GetLinearizedPolar(CFoil *pFoil0, CFoil *pFoil1, double Re, double Tau,
								 double &Alpha0, double &Slope)
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

	if(!pFoil0) {
		Alpha00 = 0.0;
		Slope0 = 2.0 * pi *pi/180.0;
	}
	else{
		pPolar1 = NULL;
		pPolar2 = NULL;
		for (i=0; i<m_poaPolar->GetSize(); i++){
			pPolar = (CPolar*)m_poaPolar->GetAt(i);
			if(pPolar->m_FoilName == pFoil0->m_FoilName) size++;
		}
		if(size){
			for (i=0; i<m_poaPolar->GetSize(); i++){
				pPolar = (CPolar*)m_poaPolar->GetAt(i);
				if(pPolar->m_FoilName == pFoil0->m_FoilName) {
					if(pPolar->m_Reynolds < Re) pPolar1 = pPolar;
				}
			}
			for (i=0; i<m_poaPolar->GetSize(); i++){
				pPolar = (CPolar*)m_poaPolar->GetAt(i);
				if(pPolar->m_FoilName == pFoil0->m_FoilName) {
					if(pPolar->m_Reynolds > Re) {
						pPolar2 = pPolar;
						break;
					}
				}
			}
		}
		if(pPolar1 && pPolar2){
			pPolar1->GetLinearizedCl(AlphaTemp1, SlopeTemp1);
			pPolar2->GetLinearizedCl(AlphaTemp2, SlopeTemp2);
			Alpha00 = AlphaTemp1 +
					 (AlphaTemp2-AlphaTemp1) * (Re-pPolar1->m_Reynolds)/(pPolar2->m_Reynolds-pPolar1->m_Reynolds);
			Slope0  = SlopeTemp1 +
					 (SlopeTemp2-SlopeTemp1) * (Re-pPolar1->m_Reynolds)/(pPolar2->m_Reynolds-pPolar1->m_Reynolds);
		}
		else{
			Alpha00 = 0.0;
			Slope0  = 2.0 * pi *pi/180.0;
		}
	}

	if(!pFoil1) {
		Alpha01 = 0.0;
		Slope1 = 2.0*pi *pi/180.0;
	}
	else{
		pPolar1 = NULL;
		pPolar2 = NULL;
		for (i=0; i<m_poaPolar->GetSize(); i++){
			pPolar = (CPolar*)m_poaPolar->GetAt(i);
			if(pPolar->m_FoilName == pFoil1->m_FoilName) size++;
		}
		if(size){
			for (i=0; i<m_poaPolar->GetSize(); i++){
				pPolar = (CPolar*)m_poaPolar->GetAt(i);
				if(pPolar->m_FoilName == pFoil1->m_FoilName) {
					if(pPolar->m_Reynolds < Re) pPolar1 = pPolar;
				}
			}
			for (i=0; i<m_poaPolar->GetSize(); i++){
				pPolar = (CPolar*)m_poaPolar->GetAt(i);
				if(pPolar->m_FoilName == pFoil1->m_FoilName) {
					if(pPolar->m_Reynolds > Re) {
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


void CMiarex::OnWGraphVar() 
{
	//define the curernt WPolar graph's vaiable
	if(!m_pCurGraph && m_iView!=2) return;
	CWGraphVarDlg dlg;
	int i;
	int Var = m_WOppVar1;
	if     (m_pCurGraph==&m_WingGraph1) Var = m_WOppVar1;
	else if(m_pCurGraph==&m_WingGraph2) Var = m_WOppVar2;
	else if(m_pCurGraph==&m_WingGraph3) Var = m_WOppVar3;
	else if(m_pCurGraph==&m_WingGraph4) Var = m_WOppVar4;

	dlg.m_iWVar = Var;
	if(dlg.DoModal() == IDOK)
	{
		for (i=m_WingGraph1.GetCurveCount()-1; i>0; i--){//0 is curretn curve
			m_WingGraph1.DeleteCurve(i);
		}
		for (i=m_WingGraph2.GetCurveCount()-1; i>0; i--){//0 is curretn curve
			m_WingGraph2.DeleteCurve(i);
		}
		for (i=m_WingGraph2.GetCurveCount()-1; i>0; i--){//0 is curretn curve
			m_WingGraph2.DeleteCurve(i);
		}
		for (i=m_WingGraph3.GetCurveCount()-1; i>0; i--){//0 is curretn curve
			m_WingGraph3.DeleteCurve(i);
		}
		if (m_pCurGraph==&m_WingGraph1){
			m_WOppVar1 = dlg.m_iWVar;
			m_WingGraph1.SetAutoY(true);
			m_WingGraph1.SetAutoYMinUnit(true);
		}
		else if (m_pCurGraph==&m_WingGraph2){
			m_WOppVar2 = dlg.m_iWVar;
			m_WingGraph2.SetAutoY(true);
			m_WingGraph2.SetAutoYMinUnit(true);
		}
		else if (m_pCurGraph==&m_WingGraph3){
			m_WOppVar3 = dlg.m_iWVar;
			m_WingGraph3.SetAutoY(true);
			m_WingGraph3.SetAutoYMinUnit(true);
		}
		else if (m_pCurGraph==&m_WingGraph4){
			m_WOppVar4 = dlg.m_iWVar;
			m_WingGraph4.SetAutoY(true);
			m_WingGraph4.SetAutoYMinUnit(true);
		}
		if (m_iView==1)     CreateWOppCurves();
		UpdateView();
	}
}

void CMiarex::PaintCp(CDC *pDC, CRect *pCltRect, CRect *pDrawRect)
{	
	//provision for 3D Panel Dlg analysis
	//To print the Cp anlong a chord strip
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CChildView * pChildView = (CChildView*)m_pChildWnd;
	int margin = 10;
	int h  = pDrawRect->Height();
	int w  = pDrawRect->Width();
	int w2 = (int)(w/2);

	pDC->SetTextColor(pChildView->m_WndTextColor);
	CString Result, str, strong;

	CFont LegendFont;
	LegendFont.CreateFontIndirect(&pChildView->m_WndLogFont);

	CFont* pOldFont = pDC->SelectObject(&LegendFont);
	pDC->SetTextAlign(TA_LEFT);

	int D = 0;
	int LeftPos = margin;
	int ZPos    = pCltRect->Height()-10*12;

	if(pDrawRect->Width()<200 || pDrawRect->Height()<200) return;
	m_CpGraph.DrawGraph(pDC, pDrawRect, false);
	CPoint Place(m_rSingleRect.left, m_rSingleRect.bottom+20);
	DrawCpLegend(pDC, false, Place, pCltRect->bottom);

	pDC->SetTextColor(pChildView->m_WndTextColor);
	pDC->SetTextAlign(TA_RIGHT);

	D = 0;
	int RightPos = pCltRect->Width()-margin;
	ZPos	 = pCltRect->Height()-13*12;
	if(m_pCurWOpp && m_pCurWOpp->m_bOut) ZPos -= 12;

	
	pDC->SelectObject(pOldFont);
	LegendFont.DeleteObject();
}


void CMiarex::PaintWOpp(CDC *pDC, CRect *pCltRect, CRect *pDrawRect)
{	
	//Paint the current WOpp view
	if (m_iWingView == 1 && m_pCurWing)      PaintSingleWingGraph(pDC, pCltRect, pDrawRect);
	else if (m_iWingView == 2 && m_pCurWing) PaintTwoWingGraph(pDC, pCltRect, pDrawRect);
	else if (m_iWingView == 4 && m_pCurWing) PaintFourWingGraph(pDC, pCltRect, pDrawRect);
}


void CMiarex::PaintSingleWingGraph(CDC *pDC, CRect *pCltRect, CRect *pDrawRect)
{
	//Paint the current WOpp view
	int i;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CChildView * pChildView = (CChildView*)m_pChildWnd;
	int margin = 10;
	int h  = pDrawRect->Height();
	int w  = pDrawRect->Width();
	int w2 = (int)(w/2);

// Write wing data
	pDC->SetTextColor(pChildView->m_WndTextColor);
	CString Result, str, strong;
	CString str1;

	//END Write wing data
	if (m_pCurGraph && m_pCurWing && w2>200 && h>250){
		if(pDrawRect->Width()<200 || pDrawRect->Height()<200) return;
		m_pCurGraph->DrawGraph(pDC, pDrawRect, false);
		CPoint Place(pDrawRect->left+10, pDrawRect->top +30);
		DrawWOppLegend(pDC, false, Place, pCltRect->bottom);
	}

	CFont LegendFont;
	LegendFont.CreateFontIndirect(&pChildView->m_WndLogFont);
	CFont* pOldFont = pDC->SelectObject(&LegendFont);
	pDC->SetTextAlign(TA_LEFT);


	int D = 0;
	int LeftPos = margin;
	int ZPos    = pCltRect->Height()-10*12;
	if(m_pCurWing){
		double area = m_pCurWing->m_Area;
		if(m_pCurWing2) area += m_pCurWing2->m_Area;
		if(m_pCurPlane && m_pCurStab) ZPos-=14;
		if(m_pCurWPolar) ZPos-=24;
		if(m_iView!=2) {
			PaintWing(pDC, m_ptOffset, m_WingScale, false);
			if(m_pCurWOpp && m_pCurWOpp->m_bIsVisible) {
				CPoint PtLegend;
				PtLegend.x = (int)(m_rCltRect.Width()/2);
				PtLegend.y = m_rCltRect.bottom;
				PaintXTr(pDC, m_ptOffset, PtLegend, m_WingScale, false);
				if (m_bXCP) PaintXCP(pDC, m_ptOffset,PtLegend, m_WingScale, false);
				if (m_bXCmRef) PaintXCmRef(pDC, m_ptOffset, m_WingScale, false);
			}
			pDC->TextOut(LeftPos,ZPos, m_pCurWing->m_WingName);
			D+=12;
			CString length, surface;
			GetLengthUnit(length,pFrame->m_LengthUnit);
			GetAreaUnit(surface,pFrame->m_AreaUnit);

			str1.LoadString(IDS_WINGSPAN);
			strong.Format(str1+length, m_pCurWing->m_Span*pFrame->m_mtoUnit);
			pDC->TextOut(LeftPos,ZPos+D, strong);
			D+=12;

			str1.LoadString(IDS_WINGAREA);
			Result.Format(str1+surface, area * pFrame->m_m2toUnit);
			pDC->TextOut(LeftPos,ZPos+D, Result);
			D+=12;
			if(m_pCurWPolar){
				GetWeightUnit(str, pFrame->m_WeightUnit);
				str1.LoadString(IDS_PLANEWEIGHT);
				Result.Format(str1,  m_pCurWing->m_Weight*pFrame->m_kgtoUnit);
				Result += str;
				pDC->TextOut(LeftPos,ZPos+D, Result);
				D+=12;
				GetAreaUnit(strong, pFrame->m_AreaUnit);
				str1.LoadString(IDS_WINGLOAD);
				Result.Format(str1,
							  m_pCurWing->m_Weight*pFrame->m_kgtoUnit/area/pFrame->m_m2toUnit);
				Result += str + "/" + strong;
				pDC->TextOut(LeftPos,ZPos+D, Result);
				D+=12;
			}

			if(m_pCurPlane && m_pCurStab) {
				str1.LoadString(IDS_TAILVOLUME);
				Result.Format(str1, m_pCurPlane->m_TailVolume);
				pDC->TextOut(LeftPos,ZPos+D, Result);
				D+=12;
			}
			str1.LoadString(IDS_ROOTCHORD);
			Result.Format(str1+length, m_pCurWing->m_TChord[0]*pFrame->m_mtoUnit);
			pDC->TextOut(LeftPos,ZPos+D, Result);
			D+=12;
			str1.LoadString(IDS_MAC);
			Result.Format(str1+length, m_pCurWing->m_MAChord*pFrame->m_mtoUnit);
			pDC->TextOut(LeftPos,ZPos+D, Result);
			D+=12;				
			str1.LoadString(IDS_TWISTTIP);
			Result.Format(str1, m_pCurWing->m_TTwist[m_pCurWing->m_NPanel]);
			pDC->TextOut(LeftPos,ZPos+D, Result);
			D+=12;
			str1.LoadString(IDS_AR);
			Result.Format(str1, m_pCurWing->m_AR);
			pDC->TextOut(LeftPos,ZPos+D, Result);
			D+=12;
			str1.LoadString(IDS_TR);
			Result.Format(str1, m_pCurWing->m_TR);
			pDC->TextOut(LeftPos,ZPos+D, Result);
			D+=12;
			str1.LoadString(IDS_RTTIPSWEEP);
			Result.Format(str1, m_pCurWing->GetAverageSweep());
			pDC->TextOut(LeftPos,ZPos+D, Result);
		}
	}

	pDC->SetTextColor(pChildView->m_WndTextColor);
	pDC->SetTextAlign(TA_RIGHT);


	D = 0;
	int RightPos = pCltRect->Width()-margin;
	ZPos	 = pCltRect->Height()-13*12;
	if(m_pCurWOpp && m_pCurWOpp->m_bOut) ZPos -= 12;
	if(m_pCurWOpp) ZPos -= 12*m_pCurWOpp->m_nFlaps;

	if(m_pCurWOpp && m_pCurWOpp->m_bIsVisible){
		GetSpeedUnit(str, pFrame->m_SpeedUnit);
		int l = str.GetLength();
		if(l==2) Result.Format("QInf = %7.2f ", m_pCurWOpp->m_QInf*pFrame->m_mstoUnit);
		else if(l==3) Result.Format("QInf = %6.1f ", m_pCurWOpp->m_QInf*pFrame->m_mstoUnit);
		else if(l==4) Result.Format("QInf = %5.1f ", m_pCurWOpp->m_QInf*pFrame->m_mstoUnit);

		Result += str;
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		str1.LoadString(IDS_ALPHA);
		Result.Format(str1, m_pCurWOpp->m_Alpha);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		if(m_pCurWOpp->m_bOut){
			Result.LoadString(IDS_POINTOUT);
			D+=12;
			pDC->TextOut(RightPos,ZPos+D, Result);
		}
		str1.LoadString(IDS_LIFTCOEF);
		Result.Format(str1, m_pCurWOpp->m_CL);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		str1.LoadString(IDS_DRAGCOEF);
		Result.Format(str1, m_pCurWOpp->m_ViscousDrag+m_pCurWOpp->m_InducedDrag);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		/*		oswald=CZ^2/CXi/pi/allongement;*/
		double cxielli=m_pCurWOpp->m_CL*m_pCurWOpp->m_CL/pi/m_pCurWing->m_AR;
		Result.Format("Oswald = %9.4f ", cxielli/m_pCurWOpp->m_InducedDrag);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		Result.Format("Cl/Cd = %9.4f ", 
			m_pCurWOpp->m_CL/(m_pCurWOpp->m_InducedDrag+m_pCurWOpp->m_ViscousDrag));
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		Result.Format("GCm = %9.4f ", 	m_pCurWOpp->m_GCm);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		str1.LoadString(IDS_ROLLINGMOMENT);
		Result.Format(str1, m_pCurWOpp->m_GRm);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		str1.LoadString(IDS_INDUCEDMOMENT);
		Result.Format(str1, m_pCurWOpp->m_IYm);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		str1.LoadString(IDS_AIRFOILYAWING);
		Result.Format(str1, m_pCurWOpp->m_GYm);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		GetLengthUnit(str, pFrame->m_LengthUnit);
		l = str.GetLength();
		if (l==1)     Result.Format("XCP = %8.3f ", m_pCurWOpp->m_XCP*pFrame->m_mtoUnit);
		else if(l==2) Result.Format("XCP = %7.2f ", m_pCurWOpp->m_XCP*pFrame->m_mtoUnit);
		else if(l>=3) Result.Format("XCP = %7.2f ", m_pCurWOpp->m_XCP*pFrame->m_mtoUnit);
		Result += str;
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		for(i=0; i<m_pCurWOpp->m_nFlaps; i++){
			str1.LoadString(IDS_FLAPMOMENT);
			Result.Format(str1, i+1, m_pCurWOpp->m_FlapMoment[i]*pFrame->m_NmtoUnit);
			GetMomentUnit(str, pFrame->m_MomentUnit);
			Result += str;
			D+=12;
			pDC->TextOut(RightPos,ZPos+D, Result);
		}
	}
	pDC->SelectObject(pOldFont);
	LegendFont.DeleteObject();
}


void CMiarex::PaintTwoWingGraph(CDC *pDC, CRect *pCltRect, CRect *pDrawRect)
{	
	//Paint a double graph WOpp view
	int h  = pCltRect->Height();
	int w  = pCltRect->Width();
	int w2 = (int)(w/2);
	int w3 = (int)(w/3);
	int h23 = (int)(2*h/3);


	if(w3>150 && h>250){
	
		if(pDrawRect->Width()<200 || pDrawRect->Height()<200) return;
		DrawWOppLegend(pDC, false, m_WingLegendOffset, pCltRect->bottom);
/*		CRect Rect1(pCltRect->left,pCltRect->top,
					pCltRect->right,h38);
		CRect Rect2(pCltRect->left,h38,
					pCltRect->right,h34);*/
		CRect Rect1(pCltRect->left,pCltRect->top,
					w2,h23);
		CRect Rect2(w2,pCltRect->top,
					pCltRect->right,h23);
		m_WingGraph1.DrawGraph(pDC, &Rect1, false);
		m_WingGraph2.DrawGraph(pDC, &Rect2, false);

	}
}

void CMiarex::PaintFourWingGraph(CDC *pDC, CRect *pCltRect, CRect *pDrawRect)
{
	//Paint a four graph WOpp view
	int h  = pCltRect->Height();
	int w  = pCltRect->Width();
	int h34 = (int)(3*h/4);
	int h38 = (int)(3*h/8);
	int w2 = (int)(w/2);
	int w3 = (int)(w/3);

	if(w3>150 && h>250){
	
		if(pDrawRect->Width()<200 || pDrawRect->Height()<200) return;

		DrawWOppLegend(pDC, false, m_WingLegendOffset, pCltRect->bottom);
		CRect Rect1(0,0,w2,h38);
		CRect Rect2(w2,0,w,h38);
		CRect Rect3(0,h38,w2,h34);
		CRect Rect4(w2,h38,w,h34);
		m_WingGraph1.DrawGraph(pDC, &Rect1, false);
		m_WingGraph2.DrawGraph(pDC, &Rect2, false);
		m_WingGraph3.DrawGraph(pDC, &Rect3, false);
		m_WingGraph4.DrawGraph(pDC, &Rect4, false);

	}
}
void CMiarex::CreateCpCurves()
{
	int p,pp,i;
	int iTA, iTB;
	bool bFound;
	double SpanPos;
	CVector N;
	CCurve *pCurve = NULL;
	CString strong, str1, str2, str3, str4;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	for (i=m_CpGraph.GetCurveCount()-1; i>=0; i--)
	{
		m_CpGraph.DeleteCurve(i);
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

	m_CurSpanPos = max(-1.0, m_CurSpanPos);
	m_CurSpanPos = min( 1.0, m_CurSpanPos);
	SpanPos = m_CurSpanPos*m_pCurWOpp->m_Span/2.000001;

	str1 = m_pCurWing->m_WingName;
	str2.Format(" a=%5.2f", m_pCurWOpp->m_Alpha);
	str3.Format(" x/c=%5.2f", m_CurSpanPos);

//	if(m_bCurWOppOnly)
	{
		p=0;
		bFound = false;
		if(m_pCurWPolar->m_AnalysisType==2 || m_pCurWPolar->m_bThinSurfaces) p+=m_pCurWing->m_Surface[0].m_NXPanels;
		for (p=0; p<m_pCurWing->m_MatSize; p++)
		{
			iTA = m_pCurWing->m_pPanel[p].m_iTA;
			iTB = m_pCurWing->m_pPanel[p].m_iTB;
			
			if(m_Node[iTA].y<=SpanPos && SpanPos<=m_Node[iTB].y) 
			{
				bFound = true;
				break;
			}
		}

		if(bFound)
		{
			pCurve = m_CpGraph.AddCurve();
			pCurve->SetColor(m_CpColor);
			pCurve->SetStyle(m_CpStyle);
			pCurve->SetWidth(m_CpWidth);
			pCurve->ShowPoints(m_bShowCpPoints);
			strong = m_pCurWing->m_WingName;

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
			for (p=0; p<m_pCurWing2->m_MatSize; p++)
			{
				iTA = m_pCurWing2->m_pPanel[p].m_iTA;
				iTB = m_pCurWing2->m_pPanel[p].m_iTB;
				
				if(m_Node[iTA].y<=SpanPos && SpanPos<=m_Node[iTB].y) 
				{
					bFound = true;
					break;
				}
			}
			if(bFound)
			{
				pCurve = m_CpGraph.AddCurve();
				pCurve->SetColor(m_pCurPOpp->m_Color);
				pCurve->SetStyle(m_pCurPOpp->m_Style);
				pCurve->SetWidth(m_pCurPOpp->m_Width);
				pCurve->ShowPoints(m_pCurPOpp->m_bShowPoints);
				pCurve->SetTitle("Cp");

				for (pp=p; pp<p+coef*m_pCurWing2->m_Surface[0].m_NXPanels; pp++)
				{
					pCurve->AddPoint(m_Panel[pp].CollPt.x, m_pCurWOpp->m_Cp[pp]);
				}
			}
		}
		if(m_pCurStab && m_bShowStab)
		{
			p=0;
			bFound = false;
			if(m_pCurWPolar->m_AnalysisType==2 || m_pCurWPolar->m_bThinSurfaces) p+=m_pCurStab->m_Surface[0].m_NXPanels;
			for (p=0; p<m_pCurStab->m_MatSize; p++)
			{
				iTA = m_pCurStab->m_pPanel[p].m_iTA;
				iTB = m_pCurStab->m_pPanel[p].m_iTB;
				
				if(m_Node[iTA].y<=SpanPos && SpanPos<=m_Node[iTB].y) 
				{
					bFound = true;
					break;
				}
			}
			if(bFound)
			{
				pCurve = m_CpGraph.AddCurve();
				pCurve->SetColor(m_pCurPOpp->m_Color);
				pCurve->SetStyle(m_pCurPOpp->m_Style);
				pCurve->SetWidth(m_pCurPOpp->m_Width);
				pCurve->ShowPoints(m_pCurPOpp->m_bShowPoints);
				pCurve->SetTitle("Cp");

				for (pp=p; pp<p+coef*m_pCurStab->m_Surface[0].m_NXPanels; pp++)
				{
					pCurve->AddPoint(m_Panel[pp].CollPt.x, m_pCurWOpp->m_Cp[pp]);
				}
			}

		}
		if(m_pCurFin && m_bShowFin)
		{
			p=0;
			bFound = false;
			if(m_pCurWPolar->m_AnalysisType==2 || m_pCurWPolar->m_bThinSurfaces) p+=m_pCurFin->m_Surface[0].m_NXPanels;
			for (p=0; p<m_pCurFin->m_MatSize; p++)
			{
				iTA = m_pCurFin->m_pPanel[p].m_iTA;
				iTB = m_pCurFin->m_pPanel[p].m_iTB;
				
				if(m_Node[iTA].y<=SpanPos && SpanPos<=m_Node[iTB].y) 
				{
					bFound = true;
					break;
				}
			}
			if(bFound)
			{
				pCurve = m_CpGraph.AddCurve();
				pCurve->SetColor(m_pCurPOpp->m_Color);
				pCurve->SetStyle(m_pCurPOpp->m_Style);
				pCurve->SetWidth(m_pCurPOpp->m_Width);
				pCurve->ShowPoints(m_pCurPOpp->m_bShowPoints);
				pCurve->SetTitle("Cp");

				for (pp=p; pp<p+coef*m_pCurFin->m_Surface[0].m_NXPanels; pp++)
				{
					pCurve->AddPoint(m_Panel[pp].CollPt.x, m_pCurWOpp->m_Cp[pp]);
				}
			}
		}
	}
}

void CMiarex::CreateWOppCurves()
{
	//Create the WOpp graph curves
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CWOpp *pWOpp = NULL;;
	CPOpp *pPOpp = NULL;
	CCurve *pCurve1, *pCurve2, *pCurve3, *pCurve4;
	CCurve *pWing2Curve1, *pWing2Curve2, *pWing2Curve3, *pWing2Curve4;
	CCurve *pStabCurve1, *pStabCurve2, *pStabCurve3, *pStabCurve4;
	CCurve *pFinCurve1, *pFinCurve2, *pFinCurve3, *pFinCurve4;
	CString str;

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
		str.Format("Q=%5.2f - Alpha=%5.2f", m_pCurWOpp->m_QInf,m_pCurWOpp->m_Alpha);

		pCurve1->SetTitle(str);
		pCurve2->SetTitle(str);
		pCurve3->SetTitle(str);
		pCurve4->SetTitle(str);

		FillWOppCurve(m_pCurWOpp, &m_WingGraph1, pCurve1, m_WOppVar1);
		FillWOppCurve(m_pCurWOpp, &m_WingGraph2, pCurve2, m_WOppVar2);
		FillWOppCurve(m_pCurWOpp, &m_WingGraph3, pCurve3, m_WOppVar3);
		FillWOppCurve(m_pCurWOpp, &m_WingGraph4, pCurve4, m_WOppVar4);

		if(m_pCurPOpp && m_bShowWing2 && m_pCurPOpp->m_bBiplane){
			pWing2Curve1    = m_WingGraph1.AddCurve();
			pWing2Curve2    = m_WingGraph2.AddCurve();
			pWing2Curve3    = m_WingGraph3.AddCurve();
			pWing2Curve4    = m_WingGraph4.AddCurve();
			if(m_pCurPOpp->m_bShowPoints){
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
		if(m_pCurPOpp && m_bShowStab && m_pCurPOpp->m_bStab){
			pStabCurve1    = m_WingGraph1.AddCurve();
			pStabCurve2    = m_WingGraph2.AddCurve();
			pStabCurve3    = m_WingGraph3.AddCurve();
			pStabCurve4    = m_WingGraph4.AddCurve();
			if(m_pCurPOpp->m_bShowPoints){
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
		if(m_pCurPOpp && m_bShowFin && m_pCurPOpp->m_bFin){
			pFinCurve1    = m_WingGraph1.AddCurve();
			pFinCurve2    = m_WingGraph2.AddCurve();
			pFinCurve3    = m_WingGraph3.AddCurve();
			pFinCurve4    = m_WingGraph4.AddCurve();
			if(m_pCurPOpp->m_bShowPoints){
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
	else{
		for (k=0; k<m_poaWOpp->GetSize(); k++){
			pWOpp = (CWOpp*)m_poaWOpp->GetAt(k);
			if (pWOpp->m_bIsVisible){

				pCurve1    = m_WingGraph1.AddCurve();
				pCurve2    = m_WingGraph2.AddCurve();
				pCurve3    = m_WingGraph3.AddCurve();
				pCurve4    = m_WingGraph4.AddCurve();

				if(pWOpp->m_bShowPoints){
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

				str.Format("Q=%5.2f - Alpha=%5.2f", pWOpp->m_QInf,pWOpp->m_Alpha);

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
		for (k=0; k<m_poaPOpp->GetSize(); k++){
			pPOpp = (CPOpp*)m_poaPOpp->GetAt(k);
			if (pPOpp->m_bIsVisible){

				pCurve1    = m_WingGraph1.AddCurve();
				pCurve2    = m_WingGraph2.AddCurve();
				pCurve3    = m_WingGraph3.AddCurve();
				pCurve4    = m_WingGraph4.AddCurve();

				if(pPOpp->m_bShowPoints){
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

				str.Format("Q=%5.2f - Alpha=%5.2f", pPOpp->m_QInf,pPOpp->m_Alpha);

				pCurve1->SetTitle(str);
				pCurve2->SetTitle(str);
				pCurve3->SetTitle(str);
				pCurve4->SetTitle(str);

				FillWOppCurve(&pPOpp->m_WingWOpp, &m_WingGraph1, pCurve1, m_WOppVar1);
				FillWOppCurve(&pPOpp->m_WingWOpp, &m_WingGraph2, pCurve2, m_WOppVar2);
				FillWOppCurve(&pPOpp->m_WingWOpp, &m_WingGraph3, pCurve3, m_WOppVar3);
				FillWOppCurve(&pPOpp->m_WingWOpp, &m_WingGraph4, pCurve4, m_WOppVar4);
			
				if(m_bShowWing2 && pPOpp->m_bBiplane){
					pWing2Curve1    = m_WingGraph1.AddCurve();
					pWing2Curve2    = m_WingGraph2.AddCurve();
					pWing2Curve3    = m_WingGraph3.AddCurve();
					pWing2Curve4    = m_WingGraph4.AddCurve();
					if(pPOpp->m_bShowPoints){
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
				if(m_bShowStab && pPOpp->m_bStab){
					pStabCurve1    = m_WingGraph1.AddCurve();
					pStabCurve2    = m_WingGraph2.AddCurve();
					pStabCurve3    = m_WingGraph3.AddCurve();
					pStabCurve4    = m_WingGraph4.AddCurve();
					if(pPOpp->m_bShowPoints){
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
				if(m_bShowFin && pPOpp->m_bFin){
					pFinCurve1    = m_WingGraph1.AddCurve();
					pFinCurve2    = m_WingGraph2.AddCurve();
					pFinCurve3    = m_WingGraph3.AddCurve();
					pFinCurve4    = m_WingGraph4.AddCurve();
					if(pPOpp->m_bShowPoints){
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
	if(m_bShowElliptic && m_pCurWOpp){
		double maxlift, x, y;
		maxlift = m_pCurWOpp->GetMaxLift();
		int nStart;
		if(m_pCurWOpp->m_AnalysisType==1) nStart = 1;
		else nStart = 0;
		if(m_WOppVar1==3){
			CCurve *pCurve = m_WingGraph1.AddCurve();
			pCurve->SetStyle(PS_DASH);
			pCurve->SetColor(RGB(150, 150, 150));
			for (i=nStart; i<m_pCurWOpp->m_NStation; i++){
				x = m_pCurWOpp->m_SpanPos[i];
				y = maxlift*sqrt(1.0-x*x/m_pCurWOpp->m_Span/m_pCurWOpp->m_Span*4.0);
				pCurve->AddPoint(x*pFrame->m_mtoUnit,y);
			}
		}
		if(m_WOppVar2==3){
			CCurve *pCurve = m_WingGraph2.AddCurve();
			pCurve->SetStyle(PS_DASH);
			pCurve->SetColor(RGB(150, 150, 150));
			for (i=nStart; i<m_pCurWOpp->m_NStation; i++){
				x = m_pCurWOpp->m_SpanPos[i];
				y = maxlift*sqrt(1.0-x*x/m_pCurWOpp->m_Span/m_pCurWOpp->m_Span*4.0);
				pCurve->AddPoint(x*pFrame->m_mtoUnit,y);
			}
		}
		if(m_WOppVar3==3){
			CCurve *pCurve = m_WingGraph3.AddCurve();
			pCurve->SetStyle(PS_DASH);
			pCurve->SetColor(RGB(150, 150, 150));
			for (i=nStart; i<m_pCurWOpp->m_NStation; i++){
				x = m_pCurWOpp->m_SpanPos[i];
				y = maxlift*sqrt(1.0-x*x/m_pCurWOpp->m_Span/m_pCurWOpp->m_Span*4.0);
				pCurve->AddPoint(x*pFrame->m_mtoUnit,y);
			}
		}
		if(m_WOppVar4==3){
			CCurve *pCurve = m_WingGraph4.AddCurve();
			pCurve->SetStyle(PS_DASH);
			pCurve->SetColor(RGB(150, 150, 150));
			for (i=nStart; i<m_pCurWOpp->m_NStation; i++){
				x = m_pCurWOpp->m_SpanPos[i];
				y = maxlift*sqrt(1.0-x*x/m_pCurWOpp->m_Span/m_pCurWOpp->m_Span*4.0);
				pCurve->AddPoint(x*pFrame->m_mtoUnit,y);
			}
		}
	}
}


void CMiarex::FillWOppCurve(CWOpp *pWOpp, Graph *pGraph, CCurve *pCurve, int Var)
{
	//The curve has been created, fill the curve with the WOpp data
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	int nStart, i;
	if(pWOpp->m_AnalysisType==1) nStart = 1;
	else nStart = 0;
	
	switch(Var)
	{
		case 0:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_Ai[i]);
			}
			pGraph->SetYTitle("Induced Angle");
			break;
		}
		case 1:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, 
					pWOpp->m_Alpha + pWOpp->m_Ai[i] + pWOpp->m_Twist[i]);
			}
			pGraph->SetYTitle("Total Angle");
			break;
		}
		case 2:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_Cl[i]);
			}
			pGraph->SetYTitle("Cl");
			break;
		}
		case 3:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_Cl[i] * pWOpp->m_Chord[i]/pWOpp->m_MAChord);
			}
			pGraph->SetYTitle("Local lift");
			break;
		}
		case 4:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_PCd[i]);
			}
			pGraph->SetYTitle("Airfoil drag");
			break;
		}
		case 5:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_ICd[i]);
			}
			pGraph->SetYTitle("Induced drag");
			break;
		}
		case 6:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_PCd[i]+ pWOpp->m_ICd[i]);
			}
			pGraph->SetYTitle("Total drag");
			break;
		}
		case 7:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, (pWOpp->m_PCd[i]+ pWOpp->m_ICd[i])* pWOpp->m_Chord[i]/pWOpp->m_MAChord);
			}
			pGraph->SetYTitle("Local drag");
			break;
		}
		case 8:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_CmAirf[i]);
			}
			pGraph->SetYTitle("Cm Airfoil");
			break;
		}
		case 9:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_CmXRef[i]);
			}
			pGraph->SetYTitle("Cm XRef");
			break;
		}
		case 10:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_Cm[i]);
			}
			pGraph->SetYTitle("Cm");
			break;
		}
		case 11:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_Re[i]);
			}
			pGraph->SetYTitle("Re");
			break;
		}
		case 12:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_XTrTop[i]);
			}
			pGraph->SetYTitle("Top Trans x-Pos %");
			break;
		}
		case 13:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_XTrBot[i]);
			}
			pGraph->SetYTitle("Bot Trans x-Pos %");
			break;
		}
		case 14:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_XCPSpanRel[i]);
			}
			pGraph->SetYTitle("CP x-Pos %");
			break;
		}
		case 15:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit,
					pWOpp->m_BendingMoment[i] * pFrame->m_NmtoUnit);
			}
			CString str;
			GetMomentUnit(str, pFrame->m_MomentUnit);
			pGraph->SetYTitle("BM (" + str + ")" );
			break;
		}
		default:{
			for (i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_Ai[i]);
			}
			pGraph->SetYTitle("Induced Angle");
		}
	}
}


void CMiarex::OnSingleWingGraph() 
{
	//The user has requested a single wing graph
	m_iWingView = 1;
	m_pCurGraph = &m_WingGraph1;
	m_iView = 1;
	CheckMenus();
	CreateWOppCurves();
	OnResetWingLegend();//updates view
}



void CMiarex::OnTwoWingGraphs() 
{
	//The user has requested two wing graphs
	m_iWingView = 2;
	m_iView = 1;
	CheckMenus();
	CreateWOppCurves();
	OnResetWingLegend();//updates view
}

void CMiarex::OnFourWingGraphs() 
{
	//The user has requested four wing graphs
	m_iWingView = 4;
	m_iView = 1;
	CheckMenus();
	CreateWOppCurves();
	OnResetWingLegend();//updates view
}


void CMiarex::OnShowWOpps() 
{
	int i;
	//Switch all WOpps view to on
	m_bCurWOppOnly = false;		
	CMenu *pMenu = m_pFrame->GetMenu();
	pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWCURWOPP, MF_BYCOMMAND | MF_UNCHECKED);
	CWOpp *pWOpp;
	CPOpp *pPOpp;
	for (i=0; i< m_poaWOpp->GetSize(); i++){
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		pWOpp->m_bIsVisible = true;
	}
	for (i=0; i< m_poaPOpp->GetSize(); i++){
		pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
		pPOpp->m_bIsVisible = true;
	}
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(false);
	pFrame->m_PolarDlgBar.SetParams();

	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();

	UpdateView();	
}

void CMiarex::OnShowAllWOpps() 
{
	int i;
	//Switch all WOpps view to on for the current UFO and WPolar
	m_bCurWOppOnly = false;		
	CMenu *pMenu = m_pFrame->GetMenu();
	pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWCURWOPP, MF_BYCOMMAND | MF_UNCHECKED);
	CPOpp *pPOpp;
	CWOpp *pWOpp;
	for (i=0; i< m_poaWOpp->GetSize(); i++){
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		if (pWOpp->m_WingName == m_pCurWPolar->m_UFOName &&
			pWOpp->m_PlrName  == m_pCurWPolar->m_PlrName){
			pWOpp->m_bIsVisible = true;
		}
	}
	for (i=0; i< m_poaPOpp->GetSize(); i++){
		pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
		if (pPOpp->m_PlaneName == m_pCurWPolar->m_UFOName &&
			pPOpp->m_PlrName   == m_pCurWPolar->m_PlrName){
			pPOpp->m_bIsVisible = true;
		}
	}
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(false);
	pFrame->m_PolarDlgBar.SetParams();

	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();

	UpdateView();
}



void CMiarex::OnHideWingOpps() 
{
	//Switch all WOpps view to off for the current UFO 
	CPOpp *pPOpp;
	CWOpp *pWOpp;
	int i;
	for (i=0; i< m_poaWOpp->GetSize(); i++){
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		if (pWOpp->m_WingName == m_pCurWPolar->m_UFOName){
			pWOpp->m_bIsVisible = false;
		}
	}
	for (i=0; i< m_poaPOpp->GetSize(); i++){
		pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
		if (pPOpp->m_PlaneName == m_pCurWPolar->m_UFOName){
			pPOpp->m_bIsVisible = false;
		}
	}

	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(false);
	pFrame->m_PolarDlgBar.SetParams();
	UpdateView();
}


void CMiarex::OnShowWingOpps() 
{
	//Switch all WOpps view to on for the current UFO 
	CPOpp *pPOpp;
	CWOpp *pWOpp;
	int i;

	for (i=0; i< m_poaWOpp->GetSize(); i++){
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		if (pWOpp->m_WingName == m_pCurWPolar->m_UFOName){
			pWOpp->m_bIsVisible = true;
		}
	}
	for (i=0; i< m_poaPOpp->GetSize(); i++){
		pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
		if (pPOpp->m_PlaneName == m_pCurWPolar->m_UFOName){
			pPOpp->m_bIsVisible = true;
		}
	}
	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(false);
	pFrame->m_PolarDlgBar.SetParams();
	UpdateView();	
}


void CMiarex::OnHideWOpps() 
{
	int i;
	CPOpp *pPOpp;
	CWOpp *pWOpp;
	for (i=0; i< m_poaWOpp->GetSize(); i++){
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		pWOpp->m_bIsVisible = false;
	}
	for (i=0; i< m_poaPOpp->GetSize(); i++){
		pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
		pPOpp->m_bIsVisible = false;
	}
	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(false);
	pFrame->m_PolarDlgBar.SetParams();
	UpdateView();
	
}


void CMiarex::OnHideAllWOpps() 
{
	CPOpp *pPOpp;
	CWOpp *pWOpp;
	int i;
	for (i=0; i< m_poaWOpp->GetSize(); i++){
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		if (pWOpp->m_WingName == m_pCurWPolar->m_UFOName &&
			pWOpp->m_PlrName  == m_pCurWPolar->m_PlrName){
			pWOpp->m_bIsVisible = false;
		}
	}
	for (i=0; i< m_poaPOpp->GetSize(); i++){
		pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
		if (pPOpp->m_PlaneName == m_pCurWPolar->m_UFOName &&
			pPOpp->m_PlrName   == m_pCurWPolar->m_PlrName){
			pPOpp->m_bIsVisible = false;
		}
	}
	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(false);
	pFrame->m_PolarDlgBar.SetParams();
	UpdateView();
}


void CMiarex::OnShowCurWOpp() 
{
	m_bCurWOppOnly = !m_bCurWOppOnly;	
	CMenu *pMenu = m_pFrame->GetMenu();
	if(m_bCurWOppOnly) pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWCURWOPP, MF_BYCOMMAND | MF_CHECKED);
	else               pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWCURWOPP, MF_BYCOMMAND | MF_UNCHECKED);

	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
//	pFrame->SetSaveState(false);
	pFrame->m_PolarDlgBar.SetParams();
	UpdateView();
}


void CMiarex::OnWAdvSettings() 
{
	CWAdvDlg dlg;
	dlg.m_AlphaPrec       = m_CvPrec;
	dlg.m_Iter            = m_Iter;
	dlg.m_Relax           = m_Relax;
	dlg.m_NStation        = m_NStation;
	dlg.m_MaxWakeIter     = m_MaxWakeIter;
	dlg.m_CoreSize        = m_CoreSize;
	dlg.m_bResetWake      = m_bResetWake;
	dlg.m_bDirichlet      = m_bDirichlet;
	dlg.m_bTrefftz        = m_bTrefftz;
	dlg.m_bKeepOutOpps    = m_bKeepOutOpps;
	dlg.m_BLogFile        = m_bLogFile;
	dlg.m_MinPanelSize    = m_MinPanelSize;
	dlg.m_ControlPos      = CPanel::m_CtrlPos;
	dlg.m_VortexPos       = CPanel::m_VortexPos;
	dlg.m_WakeInterNodes  = m_WakeInterNodes;
	dlg.m_pFrame          = m_pFrame;
	dlg.m_pMiarex         = this;

	if(dlg.DoModal() == IDOK)
	{
		m_CvPrec               = dlg.m_AlphaPrec;
		m_Iter                 = dlg.m_Iter;
		m_Relax                = dlg.m_Relax;
		m_NStation             = dlg.m_NStation;
		m_MaxWakeIter          = dlg.m_MaxWakeIter;
		m_CoreSize             = dlg.m_CoreSize;
		m_bResetWake           = dlg.m_bResetWake;
		m_bDirichlet           = dlg.m_bDirichlet;
		m_bTrefftz             = dlg.m_bTrefftz;
		m_bKeepOutOpps         = dlg.m_bKeepOutOpps;
		m_WakeInterNodes       = dlg.m_WakeInterNodes;
		m_MinPanelSize         = dlg.m_MinPanelSize;
		CPanel::m_CtrlPos      = dlg.m_ControlPos;
		CPanel::m_VortexPos    = dlg.m_VortexPos;

		if(dlg.m_BLogFile) m_bLogFile = true; else m_bLogFile = false;
		m_bResetglWake    = true;
		UpdateView();
	}
}


void CMiarex::OnHideAllWPlrs() 
{
	int i;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CWPolar *pWPolar;
	for (i=0; i<m_poaWPolar->GetSize(); i++){
		pWPolar = (CWPolar*)m_poaWPolar->GetAt(i);
		pWPolar->m_bIsVisible = false;
	}
	CreateWPolarCurves();
	pFrame->m_PolarDlgBar.SetParams();
	pFrame->SetSaveState(false);
	UpdateView();
}

void CMiarex::OnShowAllWPlrs() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CWPolar *pWPolar;
	int i;
	for (i=0; i<m_poaWPolar->GetSize(); i++){
		pWPolar = (CWPolar*)m_poaWPolar->GetAt(i);
		pWPolar->m_bIsVisible = true;
	}
	CreateWPolarCurves();
	pFrame->m_PolarDlgBar.SetParams();
	pFrame->SetSaveState(false);
	UpdateView();	
}

void CMiarex::OnWngHideAll() 
{
	if(!m_pCurWing) return;
	int i;
	CString UFOName;
	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CWPolar *pWPolar;
	for (i=0; i<m_poaWPolar->GetSize(); i++){
		pWPolar = (CWPolar*)m_poaWPolar->GetAt(i);
		if (pWPolar->m_UFOName == UFOName) pWPolar->m_bIsVisible = false;
	}
	CreateWPolarCurves();
	pFrame->m_PolarDlgBar.SetParams();
	pFrame->SetSaveState(false);
	UpdateView();
}

void CMiarex::OnWngShowAll() 
{
	if(!m_pCurWing) return;
	int i;
	CString UFOName;
	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CWPolar *pWPolar;
	for (i=0; i<m_poaWPolar->GetSize(); i++){
		pWPolar = (CWPolar*)m_poaWPolar->GetAt(i);
		if (pWPolar->m_UFOName == UFOName) pWPolar->m_bIsVisible = true;
	}
	CreateWPolarCurves();
	pFrame->m_PolarDlgBar.SetParams();
	UpdateView();	
}


void CMiarex::OnShowElliptic() 
{
	m_bShowElliptic = !m_bShowElliptic;
	CheckMenus();
		
	CreateWOppCurves();
	UpdateView();
}

void CMiarex::SetScale()
{
	Set2DScale();
	Set3DScale();
}
void CMiarex::SetScale(CRect CltRect)
{
	m_rCltRect.CopyRect(CltRect);

	m_rDrawRect.CopyRect(&m_rCltRect);
	m_rDrawRect.DeflateRect(60,20,60,230);

	SetBodyScale();
	Set2DScale();
	Set3DScale();
}


void CMiarex::Set2DScale()
{
	if(m_bIs2DScaleSet && !m_bAutoScales) return;

	int w,h;

	h = m_rCltRect.Height();
	w = m_rCltRect.Width();

	if(m_bHalfWing) m_ptOffset.x = m_rDrawRect.left + m_WingGraph1.GetMargin();
	else            m_ptOffset.x = (int)(m_rCltRect.Width()/2.0);
	m_ptOffset.y = m_rCltRect.bottom-185;

	m_rSingleRect.SetRect(40,10,m_rCltRect.right-40,m_rCltRect.bottom-120);

	if(m_pCurWing)
	{
		m_WingScale = (m_rDrawRect.Width()-2*m_WingGraph1.GetMargin())/m_pCurWing->m_Span;
		if(m_bHalfWing) 	m_WingScale *= 2.0;
		m_bIs2DScaleSet = true;
	}
}


void CMiarex::Set3DScale()
{
	if(m_iView==3) m_bResetglLegend = true;

	if(m_bIs3DScaleSet && !m_bAutoScales) return;

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	if(m_pCurWing)
	{
		//wing along X axis will take 3/4 of the screen
		m_glScaled = (GLfloat)(3./4.*2.0*m_GLScale/m_pCurWing->m_Span);
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

	if (m_iView==5)
	{
		m_UFOOffset.x=(-1.0+m_VerticalSplit)*m_GLScale/2.0;
		m_UFOOffset.y=-(GLfloat)0.6*m_GLScale;
	}
	else //(m_iView==3)
	{
		double gh = (double)m_rCltRect.Height()/(double)m_rCltRect.Width() *m_GLScale;
		m_UFOOffset.x = 0.0;
		m_UFOOffset.y = (GLfloat)(gh - m_GLScale);
	}

//	m_ArcBall.GetMatrix();
	CVector eye(0.0,0.0,1.0);
	CVector up(0.0,1.0,0.0);
	m_ArcBall.SetZoom(0.3,eye,up);
	Set3DRotationCenter();
}

void CMiarex::OnResetWingScale() 
{
	if(m_pCurWing)	{
		m_WingScale = (m_rDrawRect.Width()-2*m_WingGraph1.GetMargin())/m_pCurWing->m_Span;
		if(m_bHalfWing) {
			m_WingScale *= 2.0;
			m_ptOffset.x = m_rDrawRect.left + m_WingGraph1.GetMargin();
		}
		else m_ptOffset.x = (int)(m_rCltRect.Width()/2.0);
		m_ptOffset.y = m_rCltRect.bottom-185;
	}
	UpdateView();
}

void CMiarex::OnImportWing() 
{
	CString FileName;
	CFileDialog XFileDlg(TRUE, "avl", NULL, OFN_HIDEREADONLY, _T("Athena VLM (.avl)|*.avl|"));
	
	if(IDOK==XFileDlg.DoModal()) {
		
		FileName = XFileDlg.GetFileName();
		if(AVLImportFile(FileName)){
		}
	}
	UpdateView();
}

bool CMiarex::AVLImportFile(CString FileName) 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CStdioFile XFile;
	CFileException fe;
	int res, m;
	bool bSymetric;
	double a;
	int i,j,l;
	int NSpan, NChord;
	double Sspace;

	int LengthUnit, SurfaceUnit, SpeedUnit, WeightUnit, ForceUnit, MomentUnit;
	double mtoUnit, dm2toUnit, mstoUnit, kgtoUnit, NtoUnit, NmtoUnit;

	CUnitsDlg Dlg;

	Dlg.m_bLengthOnly = true;
	Dlg.m_Length    = 3;
	Dlg.m_Area      = pFrame->m_AreaUnit;
	Dlg.m_Speed     = pFrame->m_SpeedUnit;
	Dlg.m_Weight    = pFrame->m_WeightUnit;
	Dlg.m_Force     = pFrame->m_ForceUnit;
	Dlg.m_Moment    = pFrame->m_MomentUnit;
	Dlg.m_strQuestion = "Choose length unit to read this file :";

	if(Dlg.DoModal() == IDOK){
		LengthUnit  = Dlg.m_Length;
		SurfaceUnit = Dlg.m_Area;
		SpeedUnit   = Dlg.m_Speed;
		WeightUnit  = Dlg.m_Weight;
		ForceUnit   = Dlg.m_Force;
		MomentUnit  = Dlg.m_Moment;
		SetUnits(LengthUnit, SurfaceUnit, SpeedUnit, WeightUnit, ForceUnit,MomentUnit,
				 mtoUnit, dm2toUnit, mstoUnit, kgtoUnit, NtoUnit, NmtoUnit);
	}
	else return false;

	try{
		BOOL bOpen = XFile.Open(FileName, CFile::modeRead, &fe);

		if (bOpen){
			CWing* pOldWing;
			for (l=0; l<m_poaWing->GetSize(); l++){
				pOldWing = (CWing*)m_poaWing->GetAt(l);
				pOldWing->m_AVLIndex = -987654;//improbable value
			}

			int Line = 0;
			
			CString Strong, StrTemp;

			//Header data
			ReadAVLString(&XFile, Line,  Strong);//case title
			ReadAVLString(&XFile, Line,  Strong);//Mach Number, unused
			ReadAVLString(&XFile, Line,  Strong);//iYsym  iZsym  Zsym
			res = sscanf(Strong, "%d%d%lf", &i, &j, &a);
			if(res==3){
				if (i) bSymetric = true; else bSymetric = false;
			}
			else{
				
				return false;
			}
			ReadAVLString(&XFile, Line, Strong);// Sref   Cref   Bref, unused  - XFLR5 recalculates
			ReadAVLString(&XFile, Line,  Strong);// Xref   Yref   Zref, unused  - XFLR5 recalculates

			//Surface and Body data
			//Find the SURFACE or BODY keywords
			BOOL bRead = TRUE;
			while (bRead){
				bRead  = ReadAVLString(&XFile, Line,  Strong);

				if (Strong.Find("SURF", 0) >=0 || Strong.Find("surf", 0) >=0 ) {
					CWing* pWing= new CWing(m_pFrame);
					pWing->s_pMiarex = this;
					pWing->m_bSymetric = bSymetric;
					bRead = AVLReadSurface(&XFile, Line, pWing, NSpan, NChord, Sspace);
					if (bRead) {
						for(i=0; i<=pWing->m_NPanel;i++){
							//set scale
							pWing->m_TPos[i]    /= mtoUnit;
							pWing->m_TChord[i]  /= mtoUnit;
							pWing->m_TOffset[i] /= mtoUnit;
							pWing->m_TZPos[i]   /= mtoUnit;
							pWing->m_bVLMAutoMesh = false;
							pWing->ComputeDihedrals();
							pWing->ComputeGeometry();
							pWing->m_NXPanels[i]  = NChord;
						}

						if(NSpan!=0){
							for(i=0; i<pWing->m_NPanel;i++){
								pWing->m_NYPanels[i] = (int) (NSpan * (pWing->m_TPos[i+1]-pWing->m_TPos[i])*4/pWing->m_Span);
								pWing->m_NYPanels[i] = __max(pWing->m_NYPanels[i],1);
								pWing->m_YPanelDist[i] = (int)Sspace;
							}
						}

						//last check if this wing has the same index as an old one...
						bool bMerged = false;
						for (l=0; l<m_poaWing->GetSize(); l++){
							pOldWing = (CWing*)m_poaWing->GetAt(l);
							if (pOldWing->m_AVLIndex == pWing->m_AVLIndex &&
								pWing->m_AVLIndex  != -987654){
								//... and if so merge them
								bMerged = true;

								for (m=0; m<=pWing->m_NPanel; m++){
									pOldWing->InsertSection(pWing->m_TPos[m],
															pWing->m_TChord[m],
															pWing->m_TOffset[m],
															pWing->m_TZPos[m],
															pWing->m_TTwist[m],
															pWing->m_RFoil[m],
															pWing->m_NXPanels[m],
															pWing->m_NYPanels[m],
															pWing->m_YPanelDist[m]);
								}

								delete pWing;
								m_pCurWing = pOldWing;
								pOldWing->ComputeGeometry();
								if(pOldWing->VLMGetPanelTotal()>VLMMATSIZE){
									AfxMessageBox("Excessive number of panels\nPerforming automatic remesh", MB_OK);
									pOldWing->VLMSetAutoMesh();
								}
								break;								
							}
						}

						if(!bMerged) {
							pWing->ComputeGeometry();
							m_pCurWing = AddWing(pWing);
						}
					}
					else delete pWing;
				}
			}

			XFile.Close();
			//Check the number of VLM panels
			//
			int spanpos = 0;
			int VLMPanels = 0;
			for (l=0; l<m_pCurWing->m_NPanel; l++){
				spanpos += m_pCurWing->m_NYPanels[l];
				VLMPanels +=m_pCurWing->m_NXPanels[l]*m_pCurWing->m_NYPanels[l];
			}

			if(spanpos*2>=MAXSTATIONS || VLMPanels*2 >=VLMMATSIZE){
				//repanel
				m_pCurWing->VLMSetAutoMesh();				
			}
			
			pFrame->SetSaveState(false);
			pFrame->UpdateUFOs();
			SetUFO();
			UpdateView();
			return true;
		}
		else{
			CFileException *pEx = new CFileException(CFileException::invalidFile);
			pEx->m_strFileName = FileName;
			throw(pEx);
		}
	}
	catch (CFileException *ex){
		TCHAR   szCause[255];
		CString str;
		ex->GetErrorMessage(szCause, 255);
		str = _T("Error importing wing : ");
		str += szCause;
		AfxMessageBox(str);
		ex->Delete();
		return false;
	}
}
 
BOOL CMiarex::AVLReadSurface(CStdioFile *pXFile, int &Line, CWing *pWing, 
							 int &NSpan, int &NChord, double &Sspace)
{ 
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString WingName;
	int i, res, index;
	NChord = 0;
	NSpan  = 0;
	double Cspace = 0;//always of the cosine type in XFLR5
	Sspace = 0.0;
	double sx = 1.0;
	double sy = 1.0;
	double sz = 1.0;
	double dx = 0.0;
	double dy = 0.0;
	double dz = 0.0;
	double dAinc = 0.0;

	CString strong;
	//SURFACE keyword is followed by WingName
	if(!ReadAVLString(pXFile, Line,  strong)) return false;//Wing Name
	// remove fore and aft spaces
	strong.TrimLeft();
	strong.TrimRight();

	if(GetWing(strong)){
		AfxMessageBox("The wing " + strong + " already exists", MB_OK);
		return false;
	}
	else pWing->m_WingName = strong;

	//Wing name is followed by VLM Mesh data
	if(!ReadAVLString(pXFile, Line,  strong)) return false;//| Nchord  Cspace   [ Nspan Sspace ]
	res = sscanf(strong, "%d%f%d%lf", &NChord, &Cspace, &NSpan, &Sspace);

	if(res!=2 && res !=4){
		pXFile->Close();
		return false;
	}

	// read keywords until next Surface or Body keyword, or EOF

	// "At least two SECTION keywords must be used for each surface"
	// "All the sections in the surface must be defined in order across the span."
	// So start at Panel Position 0 and increment...
	int PanelPos = 0;

	bool bRead =true;
	while (bRead){
		if(!ReadAVLString(pXFile, Line,  strong)) bRead = false;
		if(strong.Find("INDE", 0)>=0 || strong.Find("inde", 0)>0 ){
			if(!ReadAVLString(pXFile, Line,  strong)) bRead = false;//index number
			res = sscanf(strong, "%d", &index);
			if(res!=1) bRead = false;
			else pWing->m_AVLIndex = index;
		}
		else if(strong.Find("YDUP", 0)>=0 || strong.Find("ydup", 0)>=0 ){
			if(!ReadAVLString(pXFile, Line,  strong)) bRead =false;//Y sym plane
			pWing->m_bSymetric = true;
		}
		else if(strong.Find("SCAL", 0)>=0 || strong.Find("scal", 0)>=0 ){
			if(!ReadAVLString(pXFile, Line,  strong)) bRead =false;
			res = sscanf(strong, "%lf%lf%lf", &sx, &sy, &sz);//Xscale  Yscale  Zscale
			if(res!=3) bRead = false;
		}
		else if(strong.Find("TRAN", 0)>=0 || strong.Find("tran", 0)>=0 ){
			if(!ReadAVLString(pXFile, Line, strong)) bRead =false;
			res = sscanf(strong, "%lf%lf%lf", &dx, &dy, &dz);//dX  dY  dZ

		}
		else if(strong.Find("ANGL", 0)>=0 || strong.Find("angl", 0)>=0 ){
			if(!ReadAVLString(pXFile, Line,  strong)) bRead =false;
			res = sscanf(strong, "%lf", &dAinc);//dX  dY  dZ
			if(res!=1)                               bRead = false;
		}
		else if(strong.Find("SECT", 0)>=0 || strong.Find("sect", 0)>=0 ){
			AVLReadSection(pXFile, Line, pWing, PanelPos);
			PanelPos++;
		}
		else if(strong.Find("BODY", 0)>=0 || strong.Find("body", 0)>=0 ||
			    strong.Find("SURF", 0)>=0 || strong.Find("surf", 0)>=0 ){
			Rewind1Line(pXFile, Line, strong);
			bRead = false;
		}
	}

	if (PanelPos<=1){
		CString msg;
		msg.Format("Only %d section(s) could be read\nTwo at least are required to define a wing", PanelPos);
		AfxMessageBox(msg);
		return false;
	}
	else {
		pWing->m_NPanel = PanelPos-1;
		for (i=0; i<=pWing->m_NPanel; i++){
			//TODO : scale or translate first ?
			pWing->m_TPos[i]    *= sy;
			pWing->m_TChord[i]  *= sx;
			pWing->m_TOffset[i] *= sx;
			pWing->m_TZPos[i]   *= sz;

			pWing->m_TPos[i]    += dy;
			pWing->m_TChord[i]  += dx;
			pWing->m_TOffset[i] += dx;
			pWing->m_TZPos[i]   += dz;

		}
	}
	//TODO : process ANGL or ignore it ? 
	return TRUE;
}


BOOL CMiarex::AVLReadSection(CStdioFile *pXFile, int &Line, CWing *pWing, int PanelPos)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString strong;
	int res;
	double Xle, Yle, Zle, Chord, Ainc;
	double Sspace = 0;
	int NSpan = 0;
	int Naca;

	//Section keyword is followed by VLM Mesh data
	if(!ReadAVLString(pXFile, Line,  strong)) return false;//Xle Yle Zle   Chord Ainc   [ Nspan Sspace ]
	res = sscanf(strong, "%lf%lf%lf%lf%lf%d%lf", 
				&Xle, &Yle, &Zle, &Chord, &Ainc, &NSpan, &Sspace);
	pWing->m_TOffset[PanelPos] = Xle;
	pWing->m_TPos[PanelPos]    = Yle;
	pWing->m_TZPos[PanelPos]   = Zle;
	pWing->m_TChord[PanelPos]  = Chord;
	pWing->m_TTwist[PanelPos]  = Ainc;// TODO : increment over all panels ???

	if(res == 7) {
		pWing->m_NYPanels[PanelPos] = NSpan;
		pWing->m_YPanelDist[PanelPos] = (int)Sspace;
	}

	//read keywords until next SURFace, BODY or SECTion keyword, or EOF
	bool bRead =true;
	while (bRead){
		if(!ReadAVLString(pXFile, Line,  strong)) return false;
		if(strong.Find("SECT", 0)>=0 || strong.Find("sect", 0)>=0 ){
			Rewind1Line(pXFile, Line, strong);
			bRead = false;
		}
		else if(strong.Find("BODY", 0)>=0 || strong.Find("body", 0)>=0 ){
			Rewind1Line(pXFile, Line, strong);
			bRead = false;
		}
		else if(strong.Find("SURF", 0)>=0 || strong.Find("surf", 0)>=0 ){
			Rewind1Line(pXFile, Line, strong);
			bRead = false;
		}
		else if (strong.Find("NACA", 0)>=0 || strong.Find("naca", 0)>=0 ){
			if(!ReadAVLString(pXFile, Line,  strong)) return false;
			res = sscanf(strong, "%d", &Naca);
			if(Naca<10000){
				pWing->m_RFoil[PanelPos].Format("NACA %4d", Naca);
				pWing->m_LFoil[PanelPos].Format("NACA %4d", Naca);// TODO : what if Asymetric ?!
			}
			else{
				pWing->m_RFoil[PanelPos].Format("NACA %6d", Naca);
				pWing->m_LFoil[PanelPos].Format("NACA %6d", Naca);// TODO : what if Asymetric ?!
			}
		}
		else if (strong.Find("AIRF", 0)>=0 || strong.Find("airf", 0)>=0 ){
			CString FoilName;
			FoilName.Format("AirFoil%d", PanelPos);
			double x[IQX], y[IQX];
			int n = ReadFoilPoints(pXFile, x, y);
			n = ReadFoilPoints(pXFile, x, y);
			CFoil * pFoil = pFrame->AddFoil(FoilName, x, y, n);
			if (pFoil)pFoil ->NormalizeGeometry();
			pWing->m_RFoil[PanelPos] = FoilName;
			pWing->m_LFoil[PanelPos] = FoilName;// TODO : what if Asymetric ?!	
		}
		else if (strong.Find("AFIL", 0)>=0 || strong.Find("afil", 0)>=0 ){
			if(!ReadAVLString(pXFile, Line,  strong)) return false;
			CFoil *pFoil = pFrame->ReadFoilFile(strong, true);
			if (pFoil){
				pWing->m_RFoil[PanelPos] = pFoil->m_FoilName;
				pWing->m_LFoil[PanelPos] = pFoil->m_FoilName;// TODO : what if Asymetric ?!	
			}
			else{
				strong = strong.Left(strong.GetLength()-4);
				pWing->m_RFoil[PanelPos] = strong;
				pWing->m_LFoil[PanelPos] = strong;// TODO : what if Asymetric ?!	
			}
		}
		else if (strong.Find("CLAF", 0)>=0 || strong.Find("claf", 0)>=0 ){
			if(!ReadAVLString(pXFile, Line,  strong)) return false;//unused
		}
		else if (strong.Find("CDCL", 0)>=0 || strong.Find("cdcl", 0)>=0 ){
			if(!ReadAVLString(pXFile, Line,  strong)) return false;//unused
		}
		else if (strong.Find("CONT", 0)>=0 || strong.Find("cont", 0)>=0 ){
			if(!ReadAVLString(pXFile, Line,  strong)) return false;//unused
		}
	}
	PanelPos++;
	if(PanelPos>MAXPANELS)
		return false;
	return TRUE;
}

int CMiarex::ReadFoilPoints(CStdioFile *pXFile, double *x, double *y)
{	
	//Read the foil points from an avl text file

	int res, pos;
	double X, Y;
	CString strong;
	BOOL bRead = TRUE;
	int n=0;

	while (bRead) {
		bRead = pXFile->ReadString(strong);
		pos = strong.Find("#",0);

		if (bRead && pos<0) {
			res = sscanf(strong, "%lf%lf", &X,&Y);
			if(res==2){
				x[n] = X;
				y[n] = Y;
				n++;
			}
			else bRead = false;
		}
	}
	return n;
}

void CMiarex::OnTwoGraphs() 
{
	m_iWPlrView = 2;	
	CheckMenus();
	CreateWPolarCurves();
	SetWPlrLegendPos();
	UpdateView();
}



void CMiarex::OnFourGraphs() 
{
	m_iWPlrView = 4;	
	CheckMenus();
	CreateWPolarCurves();
	SetWPlrLegendPos();
	UpdateView();
}

void CMiarex::SetWingLegendPos()
{
	int h  = m_rCltRect.Height();
	int h23 = (int)(2*h/3);
	int h34 = (int)(3*h/4);

	int margin = 10;
	if(m_iWingView == 1){
		m_WingLegendOffset.x = m_rCltRect.right-300+margin;
		m_WingLegendOffset.y = 0;
	}
	else if(m_iWingView == 2){
		m_WingLegendOffset.x = margin;
		m_WingLegendOffset.y = h23+margin;
	}
	else{
		m_WingLegendOffset.x = margin;
		m_WingLegendOffset.y = h34+margin;
	}
}

void CMiarex::SetWPlrLegendPos()
{
	int h  = m_rCltRect.Height();
	int w  = m_rCltRect.Width();
	int h23 = (int)(2*h/3);
	int w23 = (int)(2*w/3);

	int margin = 10;
	if(m_iWPlrView == 1){
		m_WPlrLegendOffset.x = (int)(0.70*w)+margin;
		m_WPlrLegendOffset.y = margin;
	}
	else if (m_iWPlrView == 2){
		m_WPlrLegendOffset.x = margin;
		m_WPlrLegendOffset.y = h23+margin;
	}
	else if	(m_iWPlrView == 4){
		m_WPlrLegendOffset.x = (int)(0.70*w)+margin;
		m_WPlrLegendOffset.y = margin;
	}
}


void CMiarex::OnResetLegend() 
{
	SetWPlrLegendPos();
	UpdateView();
}


void CMiarex::OnResetWingLegend() 
{
	SetWingLegendPos();
	UpdateView();
}

void CMiarex::Animate(bool bAnimate)
{
	if(!m_pCurWOpp || !m_pCurWPolar) 
	{
		m_bAnimate = false;
		return;
	}

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CWOpp* pWOpp;

	if(bAnimate) 
	{
		for (int l=0; l< m_poaWOpp->GetSize(); l++)
		{
			pWOpp = (CWOpp*)m_poaWOpp->GetAt(l);
			if (pWOpp->m_PlrName  == m_pCurWPolar->m_PlrName &&
				pWOpp->m_WingName == m_pCurWing->m_WingName) 
			{
					if(m_pCurWOpp->m_Alpha - pWOpp->m_Alpha<0.0001) 
						m_posAnimate = l-1;
			}
		}
		m_bAnimate  = bAnimate;
	}
	else  
	{
		m_bAnimate = bAnimate;
		if(m_posAnimate<0 || m_posAnimate>=m_poaWOpp->GetSize()) return;
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(m_posAnimate);
		SetWOpp(false, pWOpp->m_Alpha);
		pFrame->UpdateWOpps();
		SetWOpp(true);
		UpdateView();
	}
}

void CMiarex::StopAnimate()
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	m_bAnimate = false;
	if(pFrame->m_WOperDlgBar.IsWindowVisible()) 
		pFrame->m_WOperDlgBar.m_ctrlAnimate.SetCheck(false);
 
	if(m_posAnimate<0 || m_posAnimate>=m_poaWOpp->GetSize()) return ;
	CWOpp* pWOpp = (CWOpp*)m_poaWOpp->GetAt(m_posAnimate);
	if(!pWOpp) return;
	SetWOpp(false, pWOpp->m_Alpha);

	if(m_pCurWOpp)
	{
		CString str;
		if(m_pCurWPolar->m_Type != 4) str.Format("%8.2f", m_pCurWOpp->m_Alpha);
		else  str.Format("%8.2f", m_pCurWOpp->m_QInf);

		int pos = pFrame->m_ctrlWOpp.FindStringExact(-1,str);
		if(pos != CB_ERR) 
		{
			pFrame->m_ctrlWOpp.SetCurSel(pos);
		}
		else 
		{
			pFrame->m_ctrlWOpp.SetCurSel(0);
		}
	}
	else
	{
		pFrame->m_ctrlWOpp.SetCurSel(0);
	}
}

LRESULT CMiarex::KickIdle()
{	
	if(m_bAnimate)
	{
		CChildView * pChildView = (CChildView*)m_pChildWnd;
		bool IsValid;

		if(!m_pCurWing || !m_pCurWPolar) return 0;

		int size;
		if(m_pCurPlane)	size = (int)m_poaPOpp->GetSize();
		else            size = (int)m_poaWOpp->GetSize();

		if(size<=1) return 0;

		CPOpp* pPOpp = NULL;
		CWOpp* pWOpp = NULL;
		//Find the current position to display

		if(m_bAnimatePlus)
		{
			m_posAnimate++;
			if (m_posAnimate >= size)
			{
				m_posAnimate = size-2;
				m_bAnimatePlus = false;
			}
		}
		else
		{
			m_posAnimate--;
			if (m_posAnimate <0)
			{
				m_posAnimate = 1;
				m_bAnimatePlus = true;
			}
		}

		if(m_posAnimate<0 || m_posAnimate>=size) return 0;
		if(m_pCurPlane)
		{
			pPOpp = (CPOpp*)m_poaPOpp->GetAt(m_posAnimate);
			if(!pPOpp) return 0;
		}
		else
		{
			pWOpp = (CWOpp*)m_poaWOpp->GetAt(m_posAnimate);
			if(!pWOpp) return 0;
		}

		if(m_pCurPlane) 
			IsValid =(pPOpp->m_PlrName   == m_pCurWPolar->m_PlrName && pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName);
		else 
			IsValid =(pWOpp->m_PlrName  == m_pCurWPolar->m_PlrName && pWOpp->m_WingName == m_pCurWing->m_WingName);

		if (IsValid)
		{
			m_pCurWOpp = pWOpp;
			if(m_pCurPlane){
				m_pCurPOpp = pPOpp;
				m_pCurWOpp = &pPOpp->m_WingWOpp;
			}
			else{
				m_pCurPOpp = NULL;
				m_pCurWOpp = pWOpp;
			}
			m_pCurPOpp = pPOpp;
			m_bCurWOppOnly = true;

			if (m_iView==1)     CreateWOppCurves();
			else if(m_iView==4)	CreateCpCurves();

			CDC *pDC = pChildView->GetDC();
			if(!pDC) return 0;

			if(m_iView==3)
			{
				m_bResetglOpp      = true;
				m_bResetglDownwash = true;
				m_bResetglLift     = true;
				m_bResetglDrag     = true;
				m_bResetglWake     = true;
				m_bResetglLegend   = true;
				GLDraw3D(pDC);
				GLRenderView(pDC);
			}
			else 
			{
				CDC memdc; 

				memdc.CreateCompatibleDC (pDC);

				CBitmap  bmp;
				bmp.CreateCompatibleBitmap(pDC, m_rCltRect.Width(), m_rCltRect.Height());
				CBitmap * pOldBmp = memdc.SelectObject(&bmp);

				memdc.FillSolidRect(&m_rCltRect,pChildView->m_crBackColor);
				memdc.SetBkMode(TRANSPARENT);

				PaintWOpp(&memdc, &m_rCltRect, &m_rDrawRect);

				// Blit back
				pDC->BitBlt(m_rCltRect.left,m_rCltRect.top,m_rCltRect.Width(),m_rCltRect.Height(),
						 &memdc,0,0,SRCCOPY);


				memdc.SelectObject(pOldBmp);
				memdc.DeleteDC();
				bmp.DeleteObject();

			}
			CMainFrame *pFrame = (CMainFrame*)m_pFrame;
			int speed = pFrame->m_WOperDlgBar.m_ctrlSpeed.GetPos();
			Sleep(500-speed);
			pChildView->ReleaseDC(pDC);
		}
		return 1;
	}
	else return(0);
}


void CMiarex::GLCreateLiftForce()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	int style, width;
	COLORREF color;
	double forcez,forcex,glx, gly;

	double sign;
	double r,g,b;

	glNewList(LIFTFORCE, GL_COMPILE);
	{
		m_GLList++;
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = pFrame->m_XCPColor;
		style = pFrame->m_XCPStyle;
		width = pFrame->m_XCPWidth;
		GetBWColor(color, style, width);

		if(style == PS_DOT) 			glLineStipple (1, 0x1111);
		else if(style == PS_DASH) 		glLineStipple (1, 0x0F0F);
		else if(style == PS_DASHDOT) 	glLineStipple (1, 0x1C47);
		else							glLineStipple (1, 0xFFFF);// Solid

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);
		glLineWidth((GLfloat)GetPenWidth(pFrame->m_XCPWidth, false));

		//Resulting force vector
		glLineWidth((GLfloat)(GetPenWidth(width, false)*2.0));

		double force = 0.5*m_pCurWPolar->m_Density * m_pCurWing->m_Area
						  *m_pCurWOpp->m_QInf*m_pCurWOpp->m_QInf
						  *m_pCurWOpp->m_CL;
	
		force *= pFrame->m_LiftScale/500.0;

		forcez =  force * cos(m_pCurWOpp->m_Alpha * pi/180.0);
		forcex = -force * sin(m_pCurWOpp->m_Alpha * pi/180.0);

		if (force>0.0) sign = 1.0; else sign = -1.0;
		glLineStipple (1, 0xFFFF);// Solid
		glLineWidth(3.0);

		glx = (GLfloat)m_pCurWOpp->m_XCP;
		gly = (GLfloat)m_pCurWOpp->m_YCP;

		glBegin(GL_LINES);
			glVertex3d(glx,gly,0.0);
			glVertex3d(glx+forcex,gly,forcez);

		glEnd();

		glBegin(GL_LINES);
			glVertex3d(glx+forcex, gly, forcez);
			glVertex3d(glx+forcex+0.008, gly+0.008, forcez-0.012*sign/m_glScaled);
		glEnd();

		glBegin(GL_LINES);
			glVertex3d(glx+forcex, gly, forcez);
			glVertex3d(glx+forcex-0.008, gly-0.008, forcez-0.012*sign/m_glScaled);
		glEnd();

		glDisable (GL_LINE_STIPPLE);

	}
	glEndList();
}

void CMiarex::GLCreateMoments()
{
//	The most common aeronautical convention defines 
//	- the roll as acting about the longitudinal axis, positive with the starboard wing down. 
//	- The yaw is about the vertical body axis, positive with the nose to starboard. 
//	- Pitch is about an axis perpendicular to the longitudinal plane of symmetry, positive nose up.
//	-- Wikipedia flight dynamics --

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	int i;
	int style, width;
	COLORREF color;
	
	double sign, amp, radius;
	double angle;//radian
	double endx, endy, endz, dx, dy, dz,xae, yae, zae;
	double factor = 10.0;
	double r,g,b;

	glNewList(VLMMOMENTS, GL_COMPILE);
	{
		m_GLList++;
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = pFrame->m_MomentColor;
		style = pFrame->m_MomentStyle;
		width = pFrame->m_MomentWidth;
		GetBWColor(color, style, width);

		if(style == PS_DOT)          glLineStipple (1, 0x1111);
		else if(style == PS_DASH)    glLineStipple (1, 0x0F0F);
		else if(style == PS_DASHDOT) glLineStipple (1, 0x1C47);
		else						 glLineStipple (1, 0xFFFF);// Solid


		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);
		glLineWidth((GLfloat)(GetPenWidth(width, false)*2.0));

		amp = 0.5*m_pCurWPolar->m_Density * m_pCurWing->m_Area
						*m_pCurWOpp->m_QInf*m_pCurWOpp->m_QInf
						*m_pCurWOpp->m_GCm/3.0;
	
		amp *= pFrame->m_LiftScale*factor;
		
		radius= m_pCurWing->m_Span/4.0;

		if (amp>0.0) sign = 1.0; else sign = +1.0;

		glBegin(GL_LINE_STRIP);
		{
			for (i=0; i<=int(abs(amp)); i++)
			{
				angle = sign*(double)i/500.0*pi;
				glVertex3d(radius*cos(angle),0.0,radius*sin(angle));
			}
		}
		glEnd();

		endx = radius*cos(angle);
		endz = radius*sin(angle);

		dx = 0.03;
		dz = 0.03*sign;

		xae = (radius-dx)*cos(angle) +dz *sin(angle);
		zae = (radius-dx)*sin(angle) -dz *cos(angle);
		glBegin(GL_LINES);
		{
			glVertex3d(endx, 0.0, endz);
			glVertex3d(xae,  0.0, zae);
		}
		glEnd();

		xae = (radius+dx)*cos(angle) +dz *sin(angle);
		zae = (radius+dx)*sin(angle) -dz *cos(angle);
		glBegin(GL_LINES);
			glVertex3d(endx, 0.0, endz);
			glVertex3d(xae,  0.0, zae);
		glEnd();

		//Resulting Rolling Moment Arc vector
		glLineWidth((GLfloat)(GetPenWidth(width, false)*2.0));

		amp = 0.5*m_pCurWPolar->m_Density * m_pCurWing->m_Area
						*m_pCurWOpp->m_QInf*m_pCurWOpp->m_QInf
						*m_pCurWOpp->m_GRm/3.0;
	
		amp *= pFrame->m_LiftScale*factor;
		
		if (amp>0.0) sign = -1.0; else sign = 1.0;

		glBegin(GL_LINE_STRIP);
			for (i=0; i<=int(abs(amp)); i++)
			{
				angle = sign*(double)i/500.0*pi;
				glVertex3d(0.0,radius*cos(angle),radius*sin(angle));
			}
		glEnd();

		endy = radius*cos(angle);
		endz = radius*sin(angle);

		dy = 0.03;
		dz = 0.03*sign;

		yae = (radius-dy)*cos(angle) +dz *sin(angle);
		zae = (radius-dy)*sin(angle) -dz *cos(angle);
		glBegin(GL_LINES);
			glVertex3d(0.0, endy, endz);
			glVertex3d(0.0, yae,  zae);
		glEnd();

		yae = (radius+dy)*cos(angle) +dz *sin(angle);
		zae = (radius+dy)*sin(angle) -dz *cos(angle);
		glBegin(GL_LINES);
			glVertex3d(0.0, endy, endz);
			glVertex3d(0.0, yae,  zae);
		glEnd();

		//Resulting Yawing Moment Arc vector
		glLineWidth((GLfloat)(GetPenWidth(width, false)*2.0));

		amp = 0.5*m_pCurWPolar->m_Density * m_pCurWing->m_Area
						*m_pCurWOpp->m_QInf*m_pCurWOpp->m_QInf
						*(m_pCurWOpp->m_GYm)/3.0;
	
		amp *= pFrame->m_LiftScale*factor;
		
		if (amp>0.0) sign = -1.0; else sign = +1.0;

		glBegin(GL_LINE_STRIP);
		{
			for (i=0; i<=int(abs(amp)); i++){
				angle = sign*(double)i/500.0*pi;
				glVertex3d(-radius*cos(angle),-radius*sin(angle),0.0);
			}
		}
		glEnd();

		endx = -radius*cos(angle);
		endy = -radius*sin(angle);

		dx =   0.03;
		dy =  -0.03*sign;

		xae = (-radius+dx)*cos(angle) +dy *sin(angle);
		yae = (-radius+dx)*sin(angle) -dy *cos(angle);
		glBegin(GL_LINES);
		{
			glVertex3d(endx, endy, 0.0);
			glVertex3d(xae,  yae, 0.0);
		}
		glEnd();

		xae = (-radius-dx)*cos(angle) +dy *sin(angle);
		yae = (-radius-dx)*sin(angle) -dy *cos(angle);
		glBegin(GL_LINES);
		{
			glVertex3d(endx, endy, 0.0);
			glVertex3d(xae,  yae, 0.0);
		}
		glEnd();

		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}

void CMiarex::GLCreateLiftStrip(CWing *pWing, CWOpp *pWOpp, UINT List)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	int i,j,k;
	int style, width;
	CVector C, CL;

	COLORREF color;
	
	double r,g,b;
	double amp, yob, xt, yt, zt, dih;
	double cosa =  cos(pWOpp->m_Alpha * pi/180.0);
	double sina = -sin(pWOpp->m_Alpha * pi/180.0);

	//LIFTLINE
	glNewList(List,GL_COMPILE);
	{
		m_GLList++;
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = pFrame->m_XCPColor;
		style = pFrame->m_XCPStyle;
		width = pFrame->m_XCPWidth;
		GetBWColor(color, style, width);

		if(style == PS_DOT) 			glLineStipple (1, 0x1111);
		else if(style == PS_DASH) 		glLineStipple (1, 0x0F0F);
		else if(style == PS_DASHDOT) 	glLineStipple (1, 0x1C47);
		else							glLineStipple (1, 0xFFFF);// Solid

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);
		glLineWidth((GLfloat)GetPenWidth(width, false));

		//dynamic pressure x area
		double q0 = 0.5 * m_pCurWPolar->m_Density * pWOpp->m_QInf * pWOpp->m_QInf;

		if(pWOpp)
		{
			if(pWOpp->m_AnalysisType==1)
			{
				for (i=1; i<pWOpp->m_NStation; i++)
				{
					yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
					xt = pWing->GetOffset(yob) + pWOpp->m_XCPSpanRel[i]*pWOpp->m_Chord[i];
					pWing->GetViewYZPos(pWOpp->m_XCPSpanRel[i], pWOpp->m_SpanPos[i], yt, zt, 0);
					dih = -pWing->GetDihedral(yob)*pi/180.0;
					amp = q0*pWOpp->m_Cl[i]*pWOpp->m_Chord[i]/pWOpp->m_MAChord;
					amp *= pFrame->m_LiftScale/1000.0;

					glBegin(GL_LINES);
					{
						glVertex3d(xt, yt, zt);
						glVertex3d((xt + amp * cos(dih)*sina),
									yt + amp * sin(dih),
									zt + amp * cos(dih)*cosa);
					}
					glEnd();
				}
				glBegin(GL_LINE_STRIP);
				{
					for (i=1; i<pWOpp->m_NStation; i++)
					{
						yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
						xt = pWing->GetOffset(yob) + pWOpp->m_XCPSpanRel[i]*pWOpp->m_Chord[i];
						pWing->GetViewYZPos(pWOpp->m_XCPSpanRel[i], pWOpp->m_SpanPos[i],yt,zt,0);
		
						dih = -pWing->GetDihedral(yob)*pi/180.0;
						amp = q0*pWOpp->m_Cl[i]*pWOpp->m_Chord[i]/pWOpp->m_MAChord;
						amp *= pFrame->m_LiftScale/1000.0;

						glVertex3d(xt + amp * cos(dih)*sina,
								   yt + amp * sin(dih),
								   zt + amp * cos(dih)*cosa);
					}
				}
				glEnd();
			}
			else
			{
				i = 0;
				for (j=0; j<pWing->m_NSurfaces; j++)
				{
					for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++)
					{
						pWing->m_Surface[j].GetLeadingPt(k, C);
						amp = pWOpp->m_Chord[i] / pWOpp->m_StripArea[i] / m_pCurWing->m_MAChord * pFrame->m_LiftScale/1000.0;
						C.x = pWOpp->m_XCPSpanAbs[i];

						glBegin(GL_LINES);
						{
							glVertex3d(C.x, C.y, C.z);
							glVertex3d(C.x + pWOpp->m_F[i].x*amp,//F is in Body axes
									   C.y + pWOpp->m_F[i].y*amp,
									   C.z + pWOpp->m_F[i].z*amp);
						}
						glEnd();
						i++;
					}
				}

				//Lift strip on each surface
				i = 0;
				for (j=0; j<pWing->m_NSurfaces; j++)
				{
					if(j>0 && pWing->m_Surface[j-1].m_bJoinRight)
					{
						//then connect strip to previous surface's last point
						glBegin(GL_LINES);
						{
							glVertex3d(CL.x, CL.y, CL.z);

							k=0;
							pWing->m_Surface[j].GetLeadingPt(k, C);
							amp = pWOpp->m_Chord[i] / pWOpp->m_StripArea[i] / m_pCurWing->m_MAChord * pFrame->m_LiftScale/1000.0;
							C.x = pWOpp->m_XCPSpanAbs[i];
							glVertex3d(C.x + pWOpp->m_F[i].x*amp,
									   C.y + pWOpp->m_F[i].y*amp,
									   C.z + pWOpp->m_F[i].z*amp);

						}
						glEnd();
					}
					glBegin(GL_LINE_STRIP);
					{
						for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++)
						{
							pWing->m_Surface[j].GetLeadingPt(k, C);
							amp = pWOpp->m_Chord[i] / pWOpp->m_StripArea[i] / m_pCurWing->m_MAChord * pFrame->m_LiftScale/1000.0;
							C.x = pWOpp->m_XCPSpanAbs[i];
							CL.x = C.x + pWOpp->m_F[i].x*amp;
							CL.y = C.y + pWOpp->m_F[i].y*amp;
							CL.z = C.z + pWOpp->m_F[i].z*amp;
							glVertex3d(CL.x, CL.y, CL.z);
							i++;
						}
					}
					glEnd();
				}
			}
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}


void CMiarex::GLCreateDrag(CWing *pWing, CWOpp *pWOpp, UINT List)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	CVector C;
	int i,j,k;
	int Istyle, Iwidth, Vstyle, Vwidth;
	COLORREF Icolor, Vcolor;

	GLushort IDash, VDash;
	
	double Ir,Ig,Ib, Vr, Vg, Vb;
	double amp, amp1, amp2;
	double yob, xt, yt, zt, dih;
	double cosa =  cos(pWOpp->m_Alpha * pi/180.0);
	double sina = -sin(pWOpp->m_Alpha * pi/180.0);

	Icolor = pFrame->m_IDragColor;
	Istyle = pFrame->m_IDragStyle;
	Iwidth = pFrame->m_IDragWidth;
	GetBWColor(Icolor, Istyle, Iwidth);

	Vcolor = pFrame->m_VDragColor;
	Vstyle = pFrame->m_VDragStyle;
	Vwidth = pFrame->m_VDragWidth;
	GetBWColor(Vcolor, Vstyle, Vwidth);

	if(Istyle == PS_DOT)			IDash = 0x1111;
	else if(Istyle == PS_DASH)		IDash = 0x0F0F;
	else if(Istyle == PS_DASHDOT) 	IDash = 0x1C47;
	else							IDash = 0xFFFF;// Solid

	if(Vstyle == PS_DOT)			VDash = 0x1111;
	else if(Vstyle == PS_DASH) 		VDash = 0x0F0F;
	else if(Vstyle == PS_DASHDOT)	VDash = 0x1C47;
	else							VDash = 0xFFFF;// Solid

	DecompRGB(Icolor,Ir,Ig,Ib);
	DecompRGB(Vcolor,Vr,Vg,Vb);

	glLineStipple (1, IDash);// Solid
	glLineWidth((GLfloat)GetPenWidth(Iwidth, false));

	//DRAGLINE
	glNewList(List,GL_COMPILE);
	{
		m_GLList++;
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);


		double q0 = 0.5*m_pCurWPolar->m_Density*m_pCurWing->m_Area*pWOpp->m_QInf*pWOpp->m_QInf;

		if(pWOpp){
			if(pWOpp->m_AnalysisType==1){
				for (i=1; i<pWOpp->m_NStation; i++){
					yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
					xt = pWing->GetOffset(yob) + pWOpp->m_Chord[i];
					pWing->GetViewYZPos(1.0, pWOpp->m_SpanPos[i],yt,zt,0);
					dih = pWing->GetDihedral(yob)*pi/180.0;
					amp1 = q0*pWOpp->m_ICd[i]*pWOpp->m_Chord[i]/pWOpp->m_MAChord*pFrame->m_DragScale/1.0;
					amp2 = q0*pWOpp->m_PCd[i]*pWOpp->m_Chord[i]/pWOpp->m_MAChord*pFrame->m_DragScale/1.0;
					if(m_bICd){
						glColor3f((GLfloat)Ir,(GLfloat)Ig,(GLfloat)Ib);
						glLineStipple (1, IDash);// Solid
						glLineWidth((GLfloat)GetPenWidth(Iwidth, false));
						glBegin(GL_LINES);
							glVertex3d(xt, yt, zt);
							glVertex3d(	xt + amp1 * cos(dih)*cosa,
										yt,
										zt - amp1 * cos(dih)*sina);
						glEnd();
					}
					if(m_bVCd){
						glColor3f((GLfloat)Vr,(GLfloat)Vg,(GLfloat)Vb);
						glLineStipple (1, VDash);// Solid
						glLineWidth((GLfloat)GetPenWidth(Vwidth, false));
						glBegin(GL_LINES);
							if(!m_bICd){
								glVertex3d(xt, yt,zt);
								glVertex3d(xt + amp2 * cos(dih)*cosa,
										   yt,
										   zt - amp2 * cos(dih)*sina);
							}
							else {
								glVertex3d(xt + amp1 * cos(dih)*cosa,
										   yt,
										   zt - amp1 * cos(dih)*sina);
								glVertex3d(xt + (amp1+amp2) * cos(dih)*cosa,
										   yt,
										   zt - (amp1+amp2) * cos(dih)*sina);
							}
						glEnd();
					}
				}
				if(m_bICd)
				{
					glColor3f((GLfloat)Ir,(GLfloat)Ig,(GLfloat)Ib);
					glLineStipple (1, IDash);// Solid
					glLineWidth((GLfloat)GetPenWidth(Iwidth, false));
					glBegin(GL_LINE_STRIP);
					{
						for (i=1; i<pWOpp->m_NStation; i++)
						{
							yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
							xt = pWing->GetOffset(yob) + pWOpp->m_Chord[i];
							pWing->GetViewYZPos(1.0, pWOpp->m_SpanPos[i],yt,zt,0);
			
							dih = pWing->GetDihedral(yob)*pi/180.0;

							amp  = q0*pWOpp->m_ICd[i]*pWOpp->m_Chord[i]/pWOpp->m_MAChord;
							amp *= pFrame->m_DragScale/1.0;

							glVertex3d(xt + amp * cos(dih)*cosa,
									   yt,
									   zt - amp * cos(dih)*sina);
						}
					}
					glEnd();
				}
				if(m_bVCd){
					glColor3f((GLfloat)Vr,(GLfloat)Vg,(GLfloat)Vb);
					glLineStipple (1, VDash);// Solid
					glLineWidth((GLfloat)GetPenWidth(Vwidth, false));
					glBegin(GL_LINE_STRIP);
						for (i=1; i<pWOpp->m_NStation; i++){
							yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
							xt = pWing->GetOffset(yob) + pWOpp->m_Chord[i];
							pWing->GetViewYZPos(1.0, pWOpp->m_SpanPos[i],yt,zt,0);
			
							dih = pWing->GetDihedral(yob)*pi/180.0;
							amp=0.0;
							if(m_bICd) amp+=pWOpp->m_ICd[i];
							amp +=pWOpp->m_PCd[i];
							amp *= q0*pWOpp->m_Chord[i]/pWOpp->m_MAChord;
							amp *= pFrame->m_DragScale/1.0;

							glVertex3d( xt + amp * cos(dih)*cosa,
										yt ,
										zt - amp * cos(dih)*sina);
						}
					glEnd();
				}
			}
			else{//VLM type drag
				i = 0;
				for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
					for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
						pWing->m_Surface[j].GetTrailingPt(k, C);
						amp1 = q0*pWOpp->m_ICd[i]*pWOpp->m_Chord[i]/m_pCurWing->m_MAChord*pFrame->m_DragScale/30.0;
						amp2 = q0*pWOpp->m_PCd[i]*pWOpp->m_Chord[i]/m_pCurWing->m_MAChord*pFrame->m_DragScale/30.0;
						if(m_bICd){
							glColor3f((GLfloat)Ir,(GLfloat)Ig,(GLfloat)Ib);
							glLineStipple (1, IDash);// Solid
							glLineWidth((GLfloat)GetPenWidth(Iwidth, false));
							glBegin(GL_LINES);
								glVertex3d(C.x, C.y, C.z);
								glVertex3d(C.x + amp1*cosa,
										   C.y ,
										   C.z - amp1*sina);
							glEnd();
						}
						if(m_bVCd){
							glColor3f((GLfloat)Vr,(GLfloat)Vg,(GLfloat)Vb);
							glLineStipple (1, VDash);// Solid
							glLineWidth((GLfloat)GetPenWidth(Vwidth, false));
							glBegin(GL_LINES);
								if(!m_bICd){
									glVertex3d(C.x, C.y, C.z);
									glVertex3d(C.x + amp2*cosa,
											   C.y ,
											   C.z - amp2*sina);
								}
								else
									glVertex3d(C.x + amp1*cosa,
											   C.y ,
											   C.z - amp1*sina);
									glVertex3d(C.x + (amp1+amp2)*cosa,
											   C.y ,
											   C.z - (amp1+amp2)*sina);
							glEnd();
						}
							
						i++;
					}
				}
				if(!pWing->m_bIsFin){
					if(m_bICd){
						glColor3f((GLfloat)Ir,(GLfloat)Ig,(GLfloat)Ib);
						glLineStipple (1, IDash);// Solid
						glLineWidth((GLfloat)GetPenWidth(Iwidth, false));
						glBegin(GL_LINE_STRIP);
							i = 0;
							for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
								for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
									pWing->m_Surface[j].GetTrailingPt(k, C);
									amp = q0*(pWOpp->m_ICd[i])*pWOpp->m_Chord[i]/m_pCurWing->m_MAChord;
									amp *= pFrame->m_DragScale/30.0;
									glVertex3d(C.x + amp*cosa,
											   C.y,
											   C.z - amp*sina);
									i++;
								}
							}
						glEnd();
					}
					if(m_bVCd){
						glColor3f((GLfloat)Vr,(GLfloat)Vg,(GLfloat)Vb);
						glLineStipple (1, VDash);// Solid
						glLineWidth((GLfloat)GetPenWidth(Vwidth, false));
						glBegin(GL_LINE_STRIP);
							i = 0;
							for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
								for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
									pWing->m_Surface[j].GetTrailingPt(k, C);
									amp=0.0;
									if(m_bICd) amp+=pWOpp->m_ICd[i];
									amp +=pWOpp->m_PCd[i];
									amp *= q0*pWOpp->m_Chord[i]/m_pCurWing->m_MAChord;
									amp *= pFrame->m_DragScale/30.0;

									glVertex3d(C.x + amp*cosa,
											   C.y,
											   C.z - amp*sina);
									i++;
								}
							}
						glEnd();
					}
				}
				else {
					if(m_bICd){
						glColor3f((GLfloat)Ir,(GLfloat)Ig,(GLfloat)Ib);
						glLineStipple (1, IDash);// Solid
						glLineWidth((GLfloat)GetPenWidth(Iwidth, false));
						i = 0;
						for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
							glBegin(GL_LINE_STRIP);
								for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
									pWing->m_Surface[j].GetTrailingPt(k, C);
									amp = q0*(pWOpp->m_ICd[i])*pWOpp->m_Chord[i]/m_pCurWing->m_MAChord;
									amp *= pFrame->m_DragScale/30.0;
									glVertex3d(C.x + amp*cosa,
											   C.y,
											   C.z - amp*sina);
									i++;
								}
							glEnd();
						}
					}
					if(m_bVCd)
					{
						glColor3f((GLfloat)Vr,(GLfloat)Vg,(GLfloat)Vb);
						glLineStipple (1, VDash);// Solid
						glLineWidth((GLfloat)GetPenWidth(Vwidth, false));
						i = 0;
						for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
							glBegin(GL_LINE_STRIP);
							{
								for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
									pWing->m_Surface[j].GetTrailingPt(k, C);
									amp=0.0;
									if(m_bICd) amp+=pWOpp->m_ICd[i];
									amp +=pWOpp->m_PCd[i];
									amp *= q0*pWOpp->m_Chord[i]/m_pCurWing->m_MAChord;
									amp *= pFrame->m_DragScale/30.0;

									glVertex3d(C.x + amp*cosa,
											   C.y,
											   C.z - amp*sina);
									i++;
								}
							}
							glEnd();
						}
					}
				}
			}
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}


void CMiarex::GLCreateSurfSpeeds()
{
	if(!m_pCurWOpp || m_pCurWOpp->m_AnalysisType==1) 
	{
		glNewList(SURFACESPEEDS, GL_COMPILE);
		glEndList();
		return;
	}
	CWaitCursor wait;
	CMessageDlg dlg;
	dlg.Create(IDD_MESSAGEDLG,this);
	dlg.m_bCancel = false;
	dlg.ShowWindow(SW_SHOW);
	dlg.SetMessage("Calculating Surface speed vectors...\nPlease wait");
	dlg.SetProgress(0);

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	COLORREF color;
	int style, width, p;
	double factor;
	double r,g,b, length, sinT, cosT, beta, *Gamma, *Mu, *Sigma;
	double x1, x2, y1, y2, z1, z2, xe, ye, ze, dlx, dlz;
	CVector C, V, VT;
	CVector RefPoint(0.0,0.0,0.0);

	factor = 0.2;
	if(m_pCurPOpp)     
	{
		Gamma = m_pCurPOpp->m_G;
		Mu    = m_pCurPOpp->m_G;
		Sigma = m_pCurPOpp->m_Sigma;
	}
	else if (m_pCurWOpp)  
	{
		Gamma = m_pCurWOpp->m_G;
		Mu    = m_pCurWOpp->m_G;
		Sigma = m_pCurWOpp->m_Sigma;
	}
	else 
	{
		Gamma = NULL;
		Mu    = NULL;
		Sigma = NULL;
	}

	glNewList(SURFACESPEEDS, GL_COMPILE);
	{
		m_GLList++;

		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = pFrame->m_WakeColor;
		style = pFrame->m_WakeStyle;
		width = pFrame->m_WakeWidth;

		glLineWidth((GLfloat)GetPenWidth(width, false));
		GetBWColor(color, style, width);

		if(style == PS_DOT)          glLineStipple (1, 0x1111);
		else if(style == PS_DASH)    glLineStipple (1, 0x0F0F);
		else if(style == PS_DASHDOT) glLineStipple (1, 0x1C47);
		else						 glLineStipple (1, 0xFFFF);// Solid

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		if(Gamma)
		{
			for (p=0; p<m_MatSize; p++)
			{
				VT.Set(m_pCurWOpp->m_QInf,0.0,0.0);

				if(m_pCurWPolar->m_AnalysisType==2)	
				{
					C.Copy(m_Panel[p].CtrlPt);
					VT += m_VLMDlg.GetSpeedVector(C, Gamma);
					VT *= pFrame->m_DownwashScale/100.0;
//					if(!m_pCurWPolar->m_bTiltedGeom)
						C.RotateY(RefPoint, m_pCurWOpp->m_Alpha);
				}
				else if(m_pCurWPolar->m_AnalysisType==3)
				{
					C.Copy(m_Panel[p].CollPt);
					m_PanelDlg.GetSpeedVector(C, Mu, Sigma, V);
					VT += V;
					VT *= pFrame->m_DownwashScale/100.0;
//					if(!m_pCurWPolar->m_bTiltedGeom)
						C.RotateY(RefPoint, m_pCurWOpp->m_Alpha);
				}

				length = VT.VAbs()*factor;
				xe     = C.x+factor*VT.x;
				ye     = C.y+factor*VT.y;
				ze     = C.z+factor*VT.z;
				if(length>0.0)
				{
					cosT   = (xe-C.x)/length;
					sinT   = (ze-C.z)/length;
					dlx     = 0.15*length;
					dlz     = 0.07*length;
					beta   = atan((ye-C.y)/length)*180.0/pi;
				}
				else {
					cosT   = 0.0;
					sinT   = 0.0;
					dlx    = 0.0;
					dlz    = 0.0;
				}

				x1 = xe -dlx*cosT - dlz*sinT;
				y1 = ye;
				z1 = ze -dlx*sinT + dlz*cosT;

				x2 = xe -dlx*cosT + dlz*sinT;
				y2 = ye;
				z2 = ze -dlx*sinT - dlz*cosT;
				
				glBegin(GL_LINES);
					glVertex3d(C.x, C.y, C.z);
					glVertex3d(xe,ye,ze);
				glEnd();

				glBegin(GL_LINES);
					glVertex3d(xe, ye, ze);
					glVertex3d(x1, y1, z1);		
				glEnd();

				glBegin(GL_LINES);
					glVertex3d(xe, ye, ze);
					glVertex3d(x2, y2, z2);
				glEnd();

				dlg.SetProgress((int)(100.0*((double)p/(double)m_MatSize)));
			}	
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
	dlg.ShowWindow(SW_HIDE);
}


void CMiarex::GLCreateStreamLines()
{
	CWaitCursor wait;
	if(!m_pCurWOpp || !m_pCurWPolar || m_pCurWPolar->m_AnalysisType==1) 
	{
		glNewList(VLMSTREAMLINES,GL_COMPILE); glEndList();
		return;
	}
	CMessageDlg dlg;
	dlg.Create(IDD_MESSAGEDLG,this);
	dlg.m_bCancel = false;
	dlg.ShowWindow(SW_SHOW);
	dlg.SetMessage("Calculating Streamlines...\nPlease wait");
	dlg.SetProgress(0);

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	bool bFound;
	int i;
	int m, p, style, width, iWing;
	double r,g,b, ds, *Gamma, *Mu, *Sigma;
	COLORREF color;

	CVector C, D, D1, V, VT, VInf, V1, V2;
	CVector RefPoint(0.0,0.0,0.0);
	CVector t;

	D1.Set(987654321.0, 0.0, 0.0);

	CWing *Wing[4], *pWing;
	Wing[0] = m_pCurWing;
	Wing[1] = m_pCurWing2;
	Wing[2] = m_pCurStab;
	Wing[3] = m_pCurFin;
 
	if(m_pCurPOpp)      
	{
		Gamma = m_pCurPOpp->m_G;
		Mu    = m_pCurPOpp->m_G;
		Sigma = m_pCurPOpp->m_Sigma;
	}
	else if (m_pCurWOpp)  
	{
		Gamma = m_pCurWOpp->m_G;
		Mu    = m_pCurWOpp->m_G;
		Sigma = m_pCurWOpp->m_Sigma;
	}
	else 
	{
		Gamma = NULL;
		Mu    = NULL;
		Sigma = NULL;
	}

	m_PanelDlg.m_bCancel = false;
	m_VLMDlg.m_bCancel   = false;

	//back-up the current geometry, before it is tilted for streamlines calculation
	memcpy(m_MemPanel, m_Panel, m_MatSize * sizeof(CPanel));
	memcpy(m_MemNode,  m_Node,  m_nNodes * sizeof(CVector));
	memcpy(m_RefWakePanel, m_WakePanel, m_WakeSize * sizeof(CPanel));
	memcpy(m_RefWakeNode,  m_WakeNode,  m_nWakeNodes * sizeof(CVector));

	//Tilt the geometry w.r.t. aoa
	RotateGeomY(m_pCurWOpp->m_Alpha, RefPoint);

	glNewList(VLMSTREAMLINES,GL_COMPILE);
	{
		m_GLList++;

		glEnable (GL_LINE_STIPPLE);

		color = RGB(100,255,190);
		style = pFrame->m_WakeStyle;
		width = pFrame->m_WakeWidth;

		GetBWColor(color, style, width);
		glLineWidth((GLfloat)GetPenWidth(width, false));

		if(style == PS_DOT) 			glLineStipple (1, 0x1111);
		else if(style == PS_DASH) 		glLineStipple (1, 0x0F0F);
		else if(style == PS_DASHDOT) 	glLineStipple (1, 0x1C47);
		else  							glLineStipple (1, 0xFFFF);// Solid
		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		VInf.Set(m_pCurWOpp->m_QInf,0.0,0.0);
		m = 0;
		for (iWing=0; iWing<4; iWing++)
		{
			if(Wing[iWing])
			{	
				pWing = Wing[iWing];
				
				for (p=0; p<pWing->m_MatSize; p++)
				{
					bFound = false;

					if(m_FlowLinesDlg.m_pos==0 && pWing->m_pPanel[p].m_bIsLeading && pWing->m_pPanel[p].m_iPos<=0)
					{
						C.Set(m_Node[pWing->m_pPanel[p].m_iLA]);
						D.Set(m_Node[pWing->m_pPanel[p].m_iLB]);
						bFound = true;
					}
					else if(m_FlowLinesDlg.m_pos==1 && pWing->m_pPanel[p].m_bIsTrailing && pWing->m_pPanel[p].m_iPos<=0)
					{
						C.Set(m_Node[pWing->m_pPanel[p].m_iTA]);
						D.Set(m_Node[pWing->m_pPanel[p].m_iTB]);
						bFound = true;
					}
					else if(m_FlowLinesDlg.m_pos==2 && pWing->m_pPanel[p].m_bIsLeading && pWing->m_pPanel[p].m_iPos<=0)
					{
						C.Set(0.0, m_Node[pWing->m_pPanel[p].m_iLA].y, 0.0);
						D.Set(0.0, m_Node[pWing->m_pPanel[p].m_iLB].y, 0.0);
						bFound = true;
					}


					if(bFound)
					{
						if(!C.IsSame(D1))
						{
							C *= 1.0001;// to avoid alignment with the panel's edge, which creates a singularity
							C.x += m_FlowLinesDlg.m_XOffset;
							C.z += m_FlowLinesDlg.m_ZOffset;
							ds = m_FlowLinesDlg.m_DeltaL;

							//apply Kutta's condition : initial speed vector is parallel to the T.E. bisector angle
							V1.Set(m_Node[pWing->m_pPanel[p].m_iTA] - m_Node[pWing->m_pPanel[p].m_iLA]);
							V1. Normalize();
			
							if(pWing->m_pPanel[p].m_iPos ==-1)
							{
								//corresponding upper panel is the next one coming up
								for (i=p; i<pWing->m_MatSize;i++)
									if(pWing->m_pPanel[i].m_iPos>0 && pWing->m_pPanel[i].m_bIsTrailing) break;
								V2 = m_Node[pWing->m_pPanel[i].m_iTA] - m_Node[pWing->m_pPanel[i].m_iLA];
								V2.Normalize();
								V1 = V1+V2;
								V1.Normalize();//V1 is parallel to the bisector angle
							}

							glBegin(GL_LINE_STRIP);
							{
								glVertex3d(C.x, C.y, C.z);
								for (i=0; i< m_FlowLinesDlg.m_NX ;i++)
								{
									if(m_pCurWPolar->m_AnalysisType==2)
										VT = m_VLMDlg.GetSpeedVector(C, Gamma);
									else if(m_pCurWPolar->m_AnalysisType==3)
										m_PanelDlg.GetSpeedVector(C, Mu, Sigma, VT);

									VT += VInf;
									VT.Normalize();
									C   += VT* ds;
									glVertex3d(C.x, C.y, C.z);
									ds *= m_FlowLinesDlg.m_XFactor;
								}
							}
							glEnd();
						}

						D1 = D;
						D *= 1.0001;// to avoid alignment with the panel's edge, which creates a singularity
						D.x += m_FlowLinesDlg.m_XOffset;
						D.z += m_FlowLinesDlg.m_ZOffset;
						ds = m_FlowLinesDlg.m_DeltaL;

						//apply Kutta's condition : initial speed vector is parallel to the T.E. bisector angle
						V1 = m_Node[pWing->m_pPanel[p].m_iTB] - m_Node[pWing->m_pPanel[p].m_iLB];
						V1. Normalize();

						//corresponding upper panel is the next one coming up
						for (i=p; i<pWing->m_MatSize; i++)
							if(pWing->m_pPanel[i].m_iPos>0 && pWing->m_pPanel[i].m_bIsTrailing) break;
						V2 = m_Node[pWing->m_pPanel[i].m_iTB] - m_Node[pWing->m_pPanel[i].m_iLB];
						V2.Normalize();
						V1 = (V1+V2);
						V1.Normalize();//V1 is parallel to the bisector angle

						glBegin(GL_LINE_STRIP);
						{
							glVertex3d(D.x, D.y, D.z);
							for (i=0; i< m_FlowLinesDlg.m_NX ;i++)
							{
								if(m_pCurWPolar->m_AnalysisType==2)
									VT = m_VLMDlg.GetSpeedVector(D, Gamma);
								else if(m_pCurWPolar->m_AnalysisType==3)
									m_PanelDlg.GetSpeedVector(D, Mu, Sigma, VT);
								VT += VInf;
								VT.Normalize();
								D   += VT* ds;
								glVertex3d(D.x, D.y, D.z);
								ds *= m_FlowLinesDlg.m_XFactor;
							}
						}
						glEnd();
					}

					dlg.SetProgress((int)(100.0*((double)m/(double)m_MatSize)));
					m++;
				}
			}
		}
	
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();

	//restore the initial geometry
	memcpy(m_Panel, m_MemPanel, m_MatSize * sizeof(CPanel));
	memcpy(m_Node,  m_MemNode,  m_nNodes  * sizeof(CVector));
	memcpy(m_WakePanel, m_RefWakePanel,  m_WakeSize   * sizeof(CPanel));
	memcpy(m_WakeNode,  m_RefWakeNode,   m_nWakeNodes * sizeof(CVector));

	dlg.ShowWindow(SW_HIDE);
}


void CMiarex::GLCreateDownwash(CWing *pWing, CWOpp *pWOpp, UINT List, int surf0)
{
	// pWing is either the Wing, the stab, or the fin
	// pWOpp is related to the pWing
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	COLORREF color;
	int style, width;
	int i,j,k,p;
	double dih, xt, yt, zt, yob;
	double y1, y2, z1, z2, xs, ys, zs;
	CVector C;
	double factor;
	double r,g,b;
	double amp;
	CW3DBar *pW3DBar   = (CW3DBar*)m_pW3DBar;
	double sina = -sin(pWOpp->m_Alpha*pi/180.0);
	double cosa =  cos(pWOpp->m_Alpha*pi/180.0);
	double sign;

	//DOWNWASH
	glNewList(List,GL_COMPILE);
	{
		m_GLList++;

		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);
		glLineWidth((GLfloat)GetPenWidth(pFrame->m_DownwashWidth, false));

		color = pFrame->m_DownwashColor;
		style = pFrame->m_DownwashStyle;
		width = pFrame->m_DownwashWidth;
		GetBWColor(color, style, width);

		if(style == PS_DOT) 
			glLineStipple (1, 0x1111);
		else if(style == PS_DASH) 
			glLineStipple (1, 0x0F0F);
		else if(style == PS_DASHDOT) 
			glLineStipple (1, 0x1C47);
		else  
			glLineStipple (1, 0xFFFF);// Solid

		DecompRGB(color,r,g,b);
		glColor3f((GLfloat)r,(GLfloat)g,(GLfloat)b);

		if(pWOpp){
			if(pWOpp->m_AnalysisType==1){

				for (i=1; i<pWOpp->m_NStation; i++){
					yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
					xt = pWing->GetOffset(yob) + pWing->GetChord(yob);
					pWing->GetViewYZPos(1., pWOpp->m_SpanPos[i], yt,zt,0);

					dih = -pWing->GetDihedral(yob)*pi/180.0;
					amp = pWOpp->m_QInf*sin(pWOpp->m_Ai[i]*pi/180.0);
					amp *= pFrame->m_DownwashScale/2.0;
					glBegin(GL_LINES);
						glVertex3d(xt, yt, zt);
						glVertex3d(xt + amp * cos(dih)* sina,
								   yt + amp * sin(dih),
								   zt + amp * cos(dih)* cosa);
					glEnd();
				}
				glBegin(GL_LINE_STRIP);
				{
					for (i=1; i<pWOpp->m_NStation; i++){
						yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
						xt = pWing->GetOffset(yob) + pWing->GetChord(yob);
						pWing->GetViewYZPos(1., pWOpp->m_SpanPos[i], yt,zt,0);

						dih = -pWing->GetDihedral(yob)*pi/180.0;
						amp = pWOpp->m_QInf*sin(pWOpp->m_Ai[i]*pi/180.0);
						amp *= pFrame->m_DownwashScale/2.0;

						glVertex3d(xt + amp * cos(dih)* sina,
								   yt + amp * sin(dih),
				                   zt + amp * cos(dih)* cosa);
					}
				}
				glEnd();
			}
			else
			{
				factor = pFrame->m_DownwashScale*pWOpp->m_QInf/100.0;

				p = 0;
				i = 0;
				for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
					for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
						m_pSurface[j+surf0]->GetTrailingPt(k, C);
						if (pWOpp->m_Vd[i].z>0) sign = 1.0; else sign = -1.0;
						glBegin(GL_LINES);
						{
							glVertex3d(C.x, C.y, C.z);
							glVertex3d(C.x+factor*pWOpp->m_Vd[i].z * sina,
									   C.y+factor*pWOpp->m_Vd[i].y,
									   C.z+factor*pWOpp->m_Vd[i].z * cosa);
						}
						glEnd();
						xs = C.x+factor*pWOpp->m_Vd[i].z*sina;
						ys = C.y+factor*pWOpp->m_Vd[i].y;
						zs = C.z+factor*pWOpp->m_Vd[i].z*cosa;
						y1 = ys - 0.085*factor*pWOpp->m_Vd[i].y      + 0.05*factor*pWOpp->m_Vd[i].z*cosa;
						z1 = zs - 0.085*factor*pWOpp->m_Vd[i].z*cosa - 0.05*factor*pWOpp->m_Vd[i].y;
						y2 = ys - 0.085*factor*pWOpp->m_Vd[i].y      - 0.05*factor*pWOpp->m_Vd[i].z*cosa;
						z2 = zs - 0.085*factor*pWOpp->m_Vd[i].z*cosa + 0.05*factor*pWOpp->m_Vd[i].y;
			
						glBegin(GL_LINES);
						{
							glVertex3d(xs, ys, zs);
							glVertex3d(xs, y1, z1);
						}
						glEnd();
						glBegin(GL_LINES);
						{
							glVertex3d(xs, ys, zs);
							glVertex3d(xs, y2, z2);
						}
						glEnd();

						i++;
					}
					p++;
				}
			}
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}

void CMiarex::GLCreateCp()
{
	CWaitCursor wait;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	int p, pp, n, averageInf, averageSup, average100;
	int nPanels;

	CVector LA,LB,TA,TB, Pt;
	CVector C,N;
	double color, lmin, lmax, range;
	double *tab;
	double CpInf[2*VLMMATSIZE], CpSup[2*VLMMATSIZE], Cp100[2*VLMMATSIZE];

	glNewList(PANELCP,GL_COMPILE);
	{
		m_GLList++;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);	
		if(m_pCurWOpp){
			if(m_pCurPOpp){
				tab = m_pCurPOpp->m_Cp;
				nPanels = m_pCurPOpp->m_NPanels;
			}
			else{
				tab = m_pCurWOpp->m_Cp;
				nPanels = m_pCurWOpp->m_NVLMPanels;
			}

			lmin = 10000.0; 
			lmax = -10000.0;

			// find min and max Cp for scale set
			for (n=0; n<m_nNodes; n++)
			{
				averageInf = 0; averageSup = 0; average100 = 0;
				CpInf[n] = 0.0; CpSup[n] = 0.0; Cp100[n] = 0.0;
				for (pp=0; pp< nPanels; pp++){
					if (m_Node[m_Panel[pp].m_iLA].IsSame(m_Node[n]) ||	
						m_Node[m_Panel[pp].m_iTA].IsSame(m_Node[n]) ||
						m_Node[m_Panel[pp].m_iTB].IsSame(m_Node[n]) ||
						m_Node[m_Panel[pp].m_iLB].IsSame(m_Node[n]) )
					{
						if(m_Panel[pp].m_iPos==1){
							CpSup[n] +=tab[pp];
							averageSup++;
						}
						else if(m_Panel[pp].m_iPos<=0){
							CpInf[n] +=tab[pp];
							averageInf++;
						}
						else if(m_Panel[pp].m_iPos==100){
							Cp100[n] +=tab[pp];
							average100++;
						}
					}
				}
				if(averageSup>0) 
				{
					CpSup[n] /= averageSup; 
					if(CpSup[n]<lmin) lmin = CpSup[n];
					if(lmax<CpSup[n]) lmax = CpSup[n];
				}
				if(averageInf>0) 
				{
					CpInf[n] /= averageInf;
					if(CpInf[n]<lmin) lmin = CpInf[n];
					if(lmax<CpInf[n]) lmax = CpInf[n];
				}
				if(average100>0)
				{
					Cp100[n] /= average100;
					if(Cp100[n]<lmin) lmin = Cp100[n];
					if(lmax<Cp100[n]) lmax = Cp100[n];
				}
			}

			if(m_bAutoCpScale)
			{
				m_LegendMin = lmin;
				m_LegendMax = lmax;
			}
			else
			{
				lmin = m_LegendMin;
				lmax = m_LegendMax;
			}

			range = lmax - lmin;

			glLineWidth(1.0);
			for (p=0; p<m_MatSize; p++)
			{
				glBegin(GL_QUADS);
				{
						TA.Copy(m_Node[m_Panel[p].m_iTA]);
						TB.Copy(m_Node[m_Panel[p].m_iTB]);
						LA.Copy(m_Node[m_Panel[p].m_iLA]);
						LB.Copy(m_Node[m_Panel[p].m_iLB]);

						if(m_Panel[p].m_iPos==1)		color = (CpSup[m_Panel[p].m_iLA]-lmin)/range;
						else if(m_Panel[p].m_iPos<=0)	color = (CpInf[m_Panel[p].m_iLA]-lmin)/range;
						else							color = (Cp100[m_Panel[p].m_iLA]-lmin)/range;
						if(IsBlackAndWhite())	glColor3d(color,color,color);
						else					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
						glVertex3d(LA.x, LA.y, LA.z);

						if(m_Panel[p].m_iPos==1)		color = (CpSup[m_Panel[p].m_iTA]-lmin)/range;
						else if(m_Panel[p].m_iPos<=0)	color = (CpInf[m_Panel[p].m_iTA]-lmin)/range;
						else							color = (Cp100[m_Panel[p].m_iTA]-lmin)/range;
						if(IsBlackAndWhite())	glColor3d(color,color,color);
						else					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
						glVertex3d(TA.x, TA.y, TA.z);

						if(m_Panel[p].m_iPos==1)		color = (CpSup[m_Panel[p].m_iTB]-lmin)/range;
						else if(m_Panel[p].m_iPos<=0)	color = (CpInf[m_Panel[p].m_iTB]-lmin)/range;
						else							color = (Cp100[m_Panel[p].m_iTB]-lmin)/range;
						if(IsBlackAndWhite())	glColor3d(color,color,color);
						else					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
						glVertex3d(TB.x, TB.y, TB.z);

						if(m_Panel[p].m_iPos==1)		color = (CpSup[m_Panel[p].m_iLB]-lmin)/range;
						else if(m_Panel[p].m_iPos<=0)	color = (CpInf[m_Panel[p].m_iLB]-lmin)/range;
						else							color = (Cp100[m_Panel[p].m_iLB]-lmin)/range;
						if(IsBlackAndWhite())	glColor3d(color,color,color);
						else					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
						glVertex3d(LB.x, LB.y, LB.z);

				}
				glEnd();
			}
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
	}
	glEndList();
}



void CMiarex::GLCreateTrans(CWing *pWing, CWOpp *pWOpp, UINT List)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	int i,j,k,m;
	double r,g,b, yrel, xt, yt, zt, yob, dih;
	CVector A,B, Pt;
	CVector C,N;

	//TOP TRANSITION
	glNewList(List,GL_COMPILE);
	{
		m_GLList++;
		//Trace("GL lists = %5d", m_GLList);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);
		glLineWidth((GLfloat)GetPenWidth(pFrame->m_XTopWidth, false));
		if(pFrame->m_XTopStyle == PS_DOT) 
			glLineStipple (1, 0x1111);
		else if(pFrame->m_XTopStyle == PS_DASH) 
			glLineStipple (1, 0x0F0F);
		else if(pFrame->m_XTopStyle == PS_DASHDOT) 
			glLineStipple (1, 0x1C47);
		else  
			glLineStipple (1, 0xFFFF);

		if(IsBlackAndWhite())
			DecompRGB(RGB(200,200,200), r,g,b);
		else
			DecompRGB(pFrame->m_XTopColor,r,g,b);

		glColor3d(r,g,b);
		glLineWidth((GLfloat)pFrame->m_XTopWidth);
		if(pWOpp){
			if(pWOpp->m_AnalysisType==1)
			{
				glBegin(GL_LINE_STRIP);
				{
					for (i=1; i<pWOpp->m_NStation; i++)
					{
						yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
						xt = pWing->GetOffset(yob) + pWOpp->m_XTrTop[i]*pWOpp->m_Chord[i];
						pWing->GetViewYZPos(pWOpp->m_XTrTop[i], pWOpp->m_SpanPos[i],yt,zt,0);
		
						dih = pWing->GetDihedral(yob)*pi/180.0;
						glVertex3d(xt,yt,zt);
					}
				}
				glEnd();
			}
			else{
				if(!pWing->m_bIsFin)
				{
					glBegin(GL_LINE_STRIP);
					{
						m = 0;
						for(j=0; j<pWing->m_NSurfaces; j++)
						{
							for(k=0; k<pWing->m_Surface[j].m_NYPanels; k++)
							{
								yrel = pWing->Getyrel(pWOpp->m_SpanPos[m]);
								pWing->m_Surface[j].GetPoint(pWOpp->m_XTrTop[m],pWOpp->m_XTrTop[m],yrel,Pt,1);
								glVertex3d(Pt.x, Pt.y, Pt.z);
								m++;
							}
						}
					}
					glEnd();
				}
				else
				{
					m = 0;
					for(j=0; j<pWing->m_NSurfaces; j++)
					{
						glBegin(GL_LINE_STRIP);
						{
							for(k=0; k<pWing->m_Surface[j].m_NYPanels; k++){
								yrel = pWing->Getyrel(pWOpp->m_SpanPos[m]);
								pWing->m_Surface[j].GetPoint(pWOpp->m_XTrTop[m],pWOpp->m_XTrTop[m],yrel,Pt,1);
								glVertex3d(Pt.x, Pt.y, Pt.z);

								m++;
							}
						}
						glEnd();
					}
				}
			}
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();

	//BOTTOM TRANSITION
	glNewList(List+1,GL_COMPILE);
	{
		m_GLList++;
		//Trace("GL lists = %5d", m_GLList);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);
		glLineWidth((GLfloat)GetPenWidth(pFrame->m_XBotWidth, false));
		if(pFrame->m_XBotStyle == PS_DOT) 
			glLineStipple (1, 0x1111);
		else if(pFrame->m_XBotStyle == PS_DASH) 
			glLineStipple (1, 0x0F0F);
		else if(pFrame->m_XBotStyle == PS_DASHDOT) 
			glLineStipple (1, 0x1C47);
		else  
			glLineStipple (1, 0xFFFF);

		if(IsBlackAndWhite())
			DecompRGB(RGB(200,200,200), r,g,b);
		else
			DecompRGB(pFrame->m_XBotColor,r,g,b);

		glColor3d(r,g,b);
		glLineWidth((GLfloat)pFrame->m_XBotWidth);
		if(pWOpp){
			if(pWOpp->m_AnalysisType==1){
				glBegin(GL_LINE_STRIP);
					for (i=1; i<pWOpp->m_NStation; i++){
						yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
						xt = pWing->GetOffset(yob) + pWOpp->m_XTrBot[i]*pWOpp->m_Chord[i];
						pWing->GetViewYZPos(pWOpp->m_XTrTop[i], pWOpp->m_SpanPos[i],yt,zt,0);
		
						dih = pWing->GetDihedral(yob)*pi/180.0;

						glVertex3d(xt,yt, zt);
					}
				glEnd();
			}
			else{
				if(!pWing->m_bIsFin){
					glBegin(GL_LINE_STRIP);
						int m = 0;
						for(j=0; j<pWing->m_NSurfaces; j++){
							for(k=0; k<pWing->m_Surface[j].m_NYPanels; k++){
								yrel = pWing->Getyrel(pWOpp->m_SpanPos[m]);
								pWing->m_Surface[j].GetPoint(pWOpp->m_XTrBot[m],pWOpp->m_XTrBot[m],yrel,Pt,-1);
								glVertex3d(Pt.x, Pt.y, Pt.z);
								m++;
							}
						}
					glEnd();
				}
				else{
					int m = 0;
					for(j=0; j<pWing->m_NSurfaces; j++){
						glBegin(GL_LINE_STRIP);
							for(k=0; k<pWing->m_Surface[j].m_NYPanels; k++){
								yrel = pWing->Getyrel(pWOpp->m_SpanPos[m]);
								pWing->m_Surface[j].GetPoint(pWOpp->m_XTrBot[m],pWOpp->m_XTrBot[m],yrel,Pt,-1);
								glVertex3d(Pt.x, Pt.y, Pt.z);
								m++;
							}
						glEnd();
					}
				}
			}
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}


void CMiarex::GLDraw3D(CDC* pDC)
{
	if (!m_pCurWing) {
		m_bResetglGeom = true;
		m_bResetglMesh = true;
		m_bResetglOpp  = true;
	}
	int row, col, NumAngles, NumCircles;
	double R, lat_incr, lon_incr, phi, theta;

	CChildView * pChildView = (CChildView*)m_pChildWnd;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

    wglMakeCurrent(pDC->m_hDC, pChildView->m_hRC);
	double r,g,b;
	DecompRGB(pChildView->m_crBackColor,r,g,b);
	glClearColor((GLfloat)r,(GLfloat)g,(GLfloat)b,0.0);

	if((m_bResetglBody2D || m_bResetglBodyPoints) && m_pCurBody)
	{
		if(glIsList(BODYPOINTS)) 
		{
			glDeleteLists(BODYPOINTS,2);
			m_GLList -=2;
		}

		GLCreateBodyPoints();
		m_bResetglBodyPoints = false;
	}

	if(m_bResetglBody2D && m_pCurBody)
	{
		if(glIsList(BODYAXIALLINES)) 
		{
			glDeleteLists(BODYAXIALLINES,5);
			m_GLList -=5;
		}

		GLCreateBodyAxialLines();
		GLCreateBodyFrame();
		GLCreateBodyGrid();
		m_bResetglBody2D = false;
	}


	if(m_bResetglBody && m_pCurBody)
	{
		m_ArcBall.GetMatrix();
		CVector eye(0.0,0.0,1.0);
		CVector up(0.0,1.0,0.0);
		m_ArcBall.SetZoom(0.3,eye,up);

		if(glIsList(BODYGEOM)) {
			glDeleteLists(BODYGEOM,2);
			m_GLList -=2;
		}
		if(m_pCurBody->m_LineType==1)	    GLCreateBodyLines();
		else if(m_pCurBody->m_LineType==2)	
		{
			GLCreateBodySurface();
		}
		m_bResetglBody = false;
		if(glIsList(BODYMESHPANELS)) 
		{
			glDeleteLists(BODYMESHPANELS,2);
			m_GLList -=2;
		}
		GLCreateBodyMesh();
		m_bResetglBodyMesh = false;
	}

	if(m_bResetglBodyMesh && m_pCurBody) 
	{
		if(glIsList(BODYMESHPANELS)) {
			glDeleteLists(BODYMESHPANELS,2);
			m_GLList -=2;
		}
		GLCreateBodyMesh();
		m_bResetglBodyMesh = false;
	}

	if(m_bResetglBody || m_bResetglGeom)
	{
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
					phi = (col * lon_incr) * pi/180.0;

					for (row = 1; row < NumAngles-1; row++)
					{
						theta = (row * lat_incr) * pi/180.0;
						glVertex3d(R*cos(phi)*cos(theta)*m_GLScale, R*sin(theta)*m_GLScale, R*sin(phi)*cos(theta)*m_GLScale);
					}
				}
				glEnd();
				glBegin(GL_LINE_STRIP);
				{
					phi = (col * lon_incr ) * pi/180.0;

					for (row = 1; row < NumAngles-1; row++)
					{
						theta = -(row * lat_incr) * pi/180.0;
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
					phi = (0.0 + (double)col*360.0/72.0) * pi/180.0;
					glVertex3d(R * cos(phi) * cos(theta)*m_GLScale, R * sin(theta)*m_GLScale, R * sin(phi) * cos(theta)*m_GLScale);
				}
			}
			glEnd();

			glBegin(GL_LINE_STRIP);
			{
				theta = 0.;
				for(col=1; col<35; col++)
				{
					phi = (0.0 + (double)col*360.0/72.0) * pi/180.0;
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
					theta = (row * lat_incr) * pi/180.0;
					glVertex3d(R*cos(phi)*cos(theta)*m_GLScale, R*sin(theta)*m_GLScale, R*sin(phi)*cos(theta)*m_GLScale);
				}
			}
			glEnd();
		
			glBegin(GL_LINE_STRIP);
			{
				theta = 0.;
				for(col=-NumAngles; col<NumAngles; col++)
				{
					phi = (0.0 + (double)col*30.0/NumAngles) * pi/180.0;
					glVertex3d(R * cos(phi) * cos(theta)*m_GLScale, R * sin(theta)*m_GLScale, R * sin(phi) * cos(theta)*m_GLScale);
				}
			}
			glEnd();
		}
		glEndList();
	}


	if(m_bResetglGeom  && m_iView==3) 
	{

		if(m_pCurWing){
			if(glIsList(WINGSURFACES)) {
				glDeleteLists(WINGSURFACES,2);
				m_GLList-=2;
			}
			GLCreateGeom(m_pCurWing,WINGSURFACES,m_pCurWing->m_WingColor);
		}

		if(m_pCurWing2){
			if(glIsList(WING2SURFACES)) {
				glDeleteLists(WING2SURFACES,2);
				m_GLList-=2;
			}
			GLCreateGeom(m_pCurWing2,WING2SURFACES,m_pCurWing2->m_WingColor);
		}

		if(m_pCurStab){
			if(glIsList(STABSURFACES)) {
				glDeleteLists(STABSURFACES,2);
				m_GLList-=2;
			}
			GLCreateGeom(m_pCurStab,STABSURFACES, m_pCurStab->m_WingColor);
		}

		if(m_pCurFin){
			if(glIsList(FINSURFACES)) {
				glDeleteLists(FINSURFACES,2);
				m_GLList-=2;
			}
			GLCreateGeom(m_pCurFin,FINSURFACES, m_pCurFin->m_WingColor);
		}

		m_bResetglGeom = false;
	}
	if(m_bResetglMesh && m_bVLMPanels && m_iView==3)
	{
		if(glIsList(MESHPANELS)) 
		{
			glDeleteLists(MESHPANELS,2);
			m_GLList-=2;
		}
		GLCreateMesh();
		if(glIsList(VLMCTRLPTS)) 
		{
			glDeleteLists(VLMCTRLPTS,1);
			m_GLList-=1;
		}
		GLCreateCtrlPts();
		if(glIsList(VLMVORTICES)) 
		{
			glDeleteLists(VLMVORTICES,1);
			m_GLList-=1;
		}
		GLCreateVortices();
		m_bResetglMesh = false;
	}
	if(m_bResetglWake && m_iView==3)
	{
		if(glIsList(WINGWAKEPANELS)) {
			glDeleteLists(WINGWAKEPANELS,1);
			m_GLList-=1;
		}
		if (m_pCurWPolar && m_pCurWPolar->m_AnalysisType==3){
			GLCreateWakePanels(WINGWAKEPANELS);
		}
		m_bResetglWake = false;
	}

	if((m_bResetglLift || m_bResetglOpp) && m_iView==3) 
	{

		if(glIsList(VLMWINGLIFT)) {
			glDeleteLists(VLMWINGLIFT,3);
			m_GLList -=3;
		}
		if(glIsList(VLMWING2LIFT)) {
			glDeleteLists(VLMWING2LIFT,1);
			m_GLList -=1;
		}
		if(glIsList(VLMSTABLIFT)) {
			glDeleteLists(VLMWING2LIFT,1);
			m_GLList -=1;
		}
		if(glIsList(VLMSTABLIFT)) {
			glDeleteLists(VLMSTABLIFT,1);
			m_GLList -=1;
		}
		if(glIsList(VLMFINLIFT)) {
			glDeleteLists(VLMFINLIFT,1);
			m_GLList -=1;
		}
		if(glIsList(VLMWINGTOPTRANS)) {
			glDeleteLists(VLMWINGTOPTRANS,2);
			m_GLList-=2;
		}	
		if(glIsList(VLMWING2TOPTRANS)) {
			glDeleteLists(VLMWING2TOPTRANS,2);
			m_GLList-=2;
		}	
		if(glIsList(VLMSTABTOPTRANS)) {
			glDeleteLists(VLMSTABTOPTRANS,2);
			m_GLList-=2;
		}	
		if(glIsList(VLMFINTOPTRANS)) {
			glDeleteLists(VLMFINTOPTRANS,2);
			m_GLList-=2;
		}	
		if (m_pCurWing && m_pCurWOpp || (m_pCurPOpp &&m_pCurWOpp) )
		{
			GLCreateLiftStrip(m_pCurWing, m_pCurWOpp, VLMWINGLIFT);
			GLCreateTrans(m_pCurWing, m_pCurWOpp, VLMWINGTOPTRANS);
			int surf = m_pCurWing->m_NSurfaces;
			if(m_pCurPOpp){
				if(m_pCurWing2) 
				{
					GLCreateLiftStrip(m_pCurWing2, &m_pCurPOpp->m_Wing2WOpp, VLMWING2LIFT);
					GLCreateTrans(m_pCurWing2, &m_pCurPOpp->m_Wing2WOpp, VLMWING2TOPTRANS);
					surf += m_pCurWing2->m_NSurfaces;
				}
				if(m_pCurStab) 
				{
					GLCreateLiftStrip(m_pCurStab, &m_pCurPOpp->m_StabWOpp, VLMSTABLIFT);
					GLCreateTrans(m_pCurStab, &m_pCurPOpp->m_StabWOpp, VLMSTABTOPTRANS);
					surf += m_pCurStab->m_NSurfaces;
				}
				if(m_pCurFin) 
				{
					GLCreateLiftStrip(m_pCurFin, &m_pCurPOpp->m_FinWOpp, VLMFINLIFT);
					GLCreateTrans(m_pCurFin, &m_pCurPOpp->m_FinWOpp, VLMFINTOPTRANS);
				}
			}
			GLCreateLiftForce();
			GLCreateMoments();
		}
		m_bResetglLift = false;
	}
	if((m_bResetglDrag || m_bResetglOpp) && m_iView==3)
	{
		if(glIsList(VLMWINGDRAG)) {
			glDeleteLists(VLMWINGDRAG,1);
			m_GLList -=1;
		}
		if(glIsList(VLMWING2DRAG)) {
			glDeleteLists(VLMWING2DRAG,1);
			m_GLList -=1;
		}
		if(glIsList(VLMSTABDRAG)) {
			glDeleteLists(VLMSTABDRAG,1);
			m_GLList -=1;
		}
		if(glIsList(VLMFINDRAG)) {
			glDeleteLists(VLMFINDRAG,1);
			m_GLList -=1;
		}

		if (m_pCurWing && m_pCurWOpp || (m_pCurPOpp &&m_pCurWOpp)) {
			GLCreateDrag(m_pCurWing, m_pCurWOpp, VLMWINGDRAG);
			int surf = m_pCurWing->m_NSurfaces;
			if(m_pCurPOpp){
				if(m_pCurWing2) {
					GLCreateDrag(m_pCurWing2, &m_pCurPOpp->m_Wing2WOpp, VLMWING2DRAG);
					surf += m_pCurWing2->m_NSurfaces;
				}
				if(m_pCurStab) {
					GLCreateDrag(m_pCurStab, &m_pCurPOpp->m_StabWOpp, VLMSTABDRAG);
					surf += m_pCurStab->m_NSurfaces;
				}
				if(m_pCurFin) {
					GLCreateDrag(m_pCurFin, &m_pCurPOpp->m_FinWOpp, VLMFINDRAG);
				}
			}
		}
		m_bResetglDrag = false;
	}
	if((m_bResetglDownwash || m_bResetglOpp) && m_iView==3) {
		if(glIsList(VLMWINGWASH)) {
			glDeleteLists(VLMWINGWASH,1);
			m_GLList--;
		}
		if(glIsList(VLMWING2WASH)) {
			glDeleteLists(VLMWING2WASH,1);
			m_GLList--;
		}
		if(glIsList(VLMSTABWASH)) {
			glDeleteLists(VLMSTABWASH,1);
			m_GLList--;
		}
		if(glIsList(VLMFINWASH)) {
			glDeleteLists(VLMFINWASH,1);
			m_GLList--;
		}
		if (m_pCurWing && m_pCurWOpp){
			GLCreateDownwash(m_pCurWing, m_pCurWOpp, VLMWINGWASH,0);
			int surf = m_pCurWing->m_NSurfaces;
			if(m_pCurPOpp){
				if(m_pCurWing2) {
					GLCreateDownwash(m_pCurWing2, &m_pCurPOpp->m_Wing2WOpp, VLMWING2WASH, surf);
					surf += m_pCurWing2->m_NSurfaces;
				}
				if(m_pCurStab) {
					GLCreateDownwash(m_pCurStab, &m_pCurPOpp->m_StabWOpp, VLMSTABWASH, surf);
					surf += m_pCurStab->m_NSurfaces;
				}
				if(m_pCurFin) 
					GLCreateDownwash(m_pCurFin, &m_pCurPOpp->m_FinWOpp, VLMFINWASH, surf);
			}
		}
		m_bResetglDownwash = false;
	}
	if(m_bResetglOpp && m_iView==3)
	{
		if (m_pCurWing && m_pCurWOpp)
		{
			if(glIsList(PANELCP)) 
			{
				glDeleteLists(PANELCP,1);
				m_GLList--;
			}

			GLCreateCp();
		}

		m_bResetglOpp = false;
	}

	if((m_bResetglLegend || m_bResetglOpp || m_bResetglGeom) && m_iView==3)
	{
		if(glIsList(WINGLEGEND)) 
		{
			glDeleteLists(WINGLEGEND,3);
			m_GLList -= 3;
		}
		GLCreateWingLegend();
		GLCreateWOppLegend();
		GLCreateCpLegend();
		m_bResetglLegend = false;
	}
 

	// draw once without streamlines to update the screen
	// then draw with streamlines
	if((m_bResetglStream || m_bResetglOpp) && m_iView==3) 
	{
		if(m_bStream){//no need to recalculate if not showing
			if(m_pCurWing && m_pCurWOpp && m_pCurWOpp->m_AnalysisType>=2){

				if(glIsList(VLMSTREAMLINES)) {
					glDeleteLists(VLMSTREAMLINES,1);
					m_GLList -=1;
				}
				GLCreateStreamLines();
				m_bResetglStream = false;
				UpdateView();
			}
		}
	}
	if((m_bResetglFlow || m_bResetglOpp) && m_iView==3) 
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
				GLCreateSurfSpeeds();

				m_bResetglFlow = false;
				UpdateView();
			}
		}
	}

	wglMakeCurrent(pDC->m_hDC, NULL);
}


void CMiarex::GLSetupLight()
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
	if(m_pCurWing)      LightFactor =  (GLfloat)pow(m_pCurWing->m_Span/2.0,0.1);
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


	float fMatAmbient[4]   = {m_GLLightDlg.m_MatAmbient,  m_GLLightDlg.m_MatAmbient,   m_GLLightDlg.m_MatAmbient,1.0f};
	float fMatSpecular[4]  = {m_GLLightDlg.m_MatSpecular, m_GLLightDlg.m_MatSpecular,  m_GLLightDlg.m_MatSpecular,1.0f};
	float fMatDiffuse[4]   = {m_GLLightDlg.m_MatDiffuse,  m_GLLightDlg.m_MatDiffuse,   m_GLLightDlg.m_MatDiffuse,1.0f};
	float fMatEmission[4]  = {m_GLLightDlg.m_MatEmission, m_GLLightDlg.m_MatEmission,  m_GLLightDlg.m_MatEmission,1.0f};

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
	glMateriali(GL_FRONT, GL_SHININESS,  m_GLLightDlg.m_iMatShininess);

    if(m_GLLightDlg.m_bDepthTest)  glEnable(GL_DEPTH_TEST);     else glDisable(GL_DEPTH_TEST);
	if(m_GLLightDlg.m_bCullFaces)  glEnable(GL_CULL_FACE);      else glDisable(GL_CULL_FACE);
	if(m_GLLightDlg.m_bSmooth)     glEnable(GL_POLYGON_SMOOTH); else glDisable(GL_POLYGON_SMOOTH);
	if(m_GLLightDlg.m_bShade)      glShadeModel(GL_SMOOTH);     else glShadeModel(GL_FLAT);

	if(m_GLLightDlg.m_bLocalView) glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,0);
	else                          glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,1);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);
}




void CMiarex::GLRenderBody(CDC *pDC)
{
	int width;

	CW3DBar *pW3DBar = (CW3DBar*)m_pW3DBar;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	CChildView *pChildView = (CChildView*)m_pChildWnd;
	wglMakeCurrent(pDC->m_hDC, pChildView->m_hRC);

	GLdouble pts[4];

	pts[0]= 1.0; pts[1]=0.0; pts[2]=0.0; pts[3]=-m_VerticalSplit*m_GLScale;  //x=m_VerticalSplit
	glClipPlane(GL_CLIP_PLANE1, pts);

	pts[0]=0.0; pts[1]= 1.0; pts[2]=0.0; pts[3]=-m_HorizontalSplit*m_GLScale;//y=m_HorizontalSplit
	glClipPlane(GL_CLIP_PLANE2, pts);	

	pts[0]=-1.0; pts[1]=0.0; pts[2]=0.0; pts[3]=m_VerticalSplit*m_GLScale;   //x=m_VerticalSplit
	glClipPlane(GL_CLIP_PLANE3, pts);

	pts[0]=0.0; pts[1]=-1.0; pts[2]=0.0; pts[3]=m_HorizontalSplit*m_GLScale; //y=m_HorizontalSplit
	glClipPlane(GL_CLIP_PLANE4, pts);	

	pts[0]= 0.0; pts[1]=0.0; pts[2]=-1.0; pts[3]= m_ClipPlanePos;  //x=m_VerticalSplit
	glClipPlane(GL_CLIP_PLANE5, pts);
	if(m_ClipPlanePos>4.9999) 	glDisable(GL_CLIP_PLANE5);
	else						glEnable(GL_CLIP_PLANE5);

	width = m_rCltRect.Width();

	glPushMatrix();
	{
		glFlush();
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//Display 2D View
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		GLDrawBodyLegend();

		glDisable(GL_CLIP_PLANE1);
		glDisable(GL_CLIP_PLANE2);
		glDisable(GL_CLIP_PLANE3);
		glDisable(GL_CLIP_PLANE4);
		if(m_pCurBody)	
		{
			glEnable(GL_CLIP_PLANE3);
			glEnable(GL_CLIP_PLANE2);
			glPushMatrix();
			{
				glTranslated(m_BodyOffset.x, m_BodyOffset.y, 0.0);
				glScaled(m_BodyScale,m_BodyScale,m_BodyScale);
				glCallList(BODYPOINTS);
				glCallList(BODYAXIALLINES);
				glCallList(BODYLINEGRID);
			}
			glPopMatrix();
		}
		glDisable(GL_CLIP_PLANE1);
		glDisable(GL_CLIP_PLANE2);
		glDisable(GL_CLIP_PLANE3);
		glDisable(GL_CLIP_PLANE4);

		if(m_pCurFrame)	
		{
			glEnable(GL_CLIP_PLANE1);
			glPushMatrix();
			{
				glTranslated(m_FrameOffset.x, m_FrameOffset.y, 0.0);
				glScaled(m_FrameScale,m_FrameScale,m_FrameScale);
				glCallList(FRAMEPOINTS);
				glCallList(BODYFRAME);
				glCallList(BODYFRAMEGRID);
			}
			glPopMatrix();
		}
		glDisable(GL_CLIP_PLANE1);
		glDisable(GL_CLIP_PLANE2);
		glDisable(GL_CLIP_PLANE3);
		glDisable(GL_CLIP_PLANE4);

		glEnable(GL_CLIP_PLANE3);
		glEnable(GL_CLIP_PLANE4);

		glPushMatrix();
		{
			GLSetupLight();

			glLoadIdentity();

			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);

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


			if(m_bglLight)	
			{
				glEnable(GL_LIGHTING);
				glEnable(GL_LIGHT0);
			}
			else {
				glDisable(GL_LIGHTING);
				glDisable(GL_LIGHT0);
			}

			if(m_bSurfaces && m_pCurBody)	glCallList(BODYSURFACES);

			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			if(m_pCurBody && m_pCurFrame && (m_bOutline||m_bSurfaces)) glCallList(BODYFRAME3D);
			if(m_bOutline && m_pCurBody)	glCallList(BODYGEOM);
			if(m_bVLMPanels && m_pCurBody)
			{
				if(!m_bSurfaces)//else panels will be filled by Cp color
					glCallList(BODYMESHBACK);
				glCallList(BODYMESHPANELS);
			}
		}
		glPopMatrix();

		glDisable(GL_CLIP_PLANE1);
		glDisable(GL_CLIP_PLANE2);
		glDisable(GL_CLIP_PLANE3);
		glDisable(GL_CLIP_PLANE4);
	}
	glPopMatrix();

	glFinish();
	SwapBuffers(pDC->m_hDC);
	wglMakeCurrent(pDC->m_hDC, NULL);
}


void CMiarex::GLRenderSphere(COLORREF cr, double radius, int NumLongitudes, int NumLatitudes)
{
	glDisable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT,GL_FILL);
	glBegin(GL_TRIANGLES);
	glColor3ub(GetRValue(cr),GetGValue(cr),GetBValue(cr));

	double start_lat = -90;
	double start_lon = 0.0;
	double R = radius;

	double lat_incr = 180.0 / NumLatitudes;
	double lon_incr = 360.0 / NumLongitudes;

	double phi1, phi2, theta1, theta2;
	GLdouble u[3], v[3], w[3], n[3];

	int row, col;

	for (col = 0; col < NumLongitudes; col++)
	{
		phi1 = (start_lon + col * lon_incr) * pi/180.0;
		phi2 = (start_lon + (col + 1) * lon_incr) * pi/180.0;

		for (row = 0; row < NumLatitudes; row++)
		{
			theta1 = (start_lat + row * lat_incr) * pi/180.0;
			theta2 = (start_lat + (row + 1) * lat_incr) * pi/180.0;

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

void CMiarex::NormalVector(GLdouble p1[3], GLdouble p2[3],  GLdouble p3[3], GLdouble n[3])
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


void CMiarex::GLCreateWingLegend()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CW3DBar *pW3DBar = (CW3DBar*)m_pW3DBar;
	CChildView * pChildView = (CChildView*)m_pChildWnd;
	double r,g,b;

	glListBase(GLF_START_LIST); 

// Write wing data
	CString Result, str, strong, str1;
	int width  = m_rCltRect.Width();
	int height = m_rCltRect.Height();

	double dD      = 12.0/(double)width*2.0*m_GLScale;
	double LeftPos = -0.98*m_GLScale;
	double ZPos = -m_GLScale + 8.0*dD +0.02*m_GLScale;

	glNewList(WINGLEGEND,GL_COMPILE);
	{
		m_GLList++;
		if(IsBlackAndWhite())	DecompRGB(0, r,g,b);
		else					DecompRGB(pChildView->m_WndTextColor, r,g,b);
		glColor3d(r,g,b);
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		if(m_pCurWing){
			double area = m_pCurWing->m_Area;
			if(m_pCurWing2) area += m_pCurWing2->m_Area;
			if(m_pCurPlane && m_pCurStab) ZPos+=dD;
			if(m_pCurWPolar) ZPos+=2.0*dD;
			CString length, surface;
			GetLengthUnit(length,pFrame->m_LengthUnit);
			GetAreaUnit(surface,pFrame->m_AreaUnit);
			CString UFOName;
			if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
			else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
			glRasterPos2d(LeftPos,ZPos);
			glCallLists(UFOName.GetLength(), GL_UNSIGNED_BYTE, UFOName);

			ZPos -=dD;

			str1.LoadString(IDS_WINGSPAN);
			Result.Format(str1+length, m_pCurWing->m_Span*pFrame->m_mtoUnit);
			glRasterPos2d(LeftPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);

			ZPos -=dD;
			str1.LoadString(IDS_WINGAREA);
			Result.Format(str1+surface, area * pFrame->m_m2toUnit);
			glRasterPos2d(LeftPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);
			ZPos -=dD;
			if(m_pCurWPolar){
				GetWeightUnit(str, pFrame->m_WeightUnit);
				str1.LoadString(IDS_PLANEWEIGHT);
				Result.Format(str1,  m_pCurWing->m_Weight*pFrame->m_kgtoUnit);
				Result += str;
				glRasterPos2d(LeftPos,ZPos);
				glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);
				ZPos -=dD;
				GetAreaUnit(strong, pFrame->m_AreaUnit);
				str1.LoadString(IDS_WINGLOAD);
				Result.Format(str1,
							m_pCurWing->m_Weight*pFrame->m_kgtoUnit
							/area/pFrame->m_m2toUnit);
				Result += str + "/" + strong;
				glRasterPos2d(LeftPos,ZPos);
				glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);
				ZPos -=dD;
			}

			if(m_pCurPlane && m_pCurStab) {
				str1.LoadString(IDS_TAILVOLUME);
				Result.Format(str1, m_pCurPlane->m_TailVolume);
				glRasterPos2d(LeftPos,ZPos);
				glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);
				ZPos -=dD;
			}

			str1.LoadString(IDS_ROOTCHORD);
			Result.Format(str1+length, m_pCurWing->m_TChord[0]*pFrame->m_mtoUnit);
			glRasterPos2d(LeftPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);
			ZPos -=dD;
			str1.LoadString(IDS_MAC);
			Result.Format(str1+length, m_pCurWing->m_MAChord*pFrame->m_mtoUnit);
			glRasterPos2d(LeftPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);
			ZPos -=dD;				
			str1.LoadString(IDS_TWISTTIP);
			Result.Format(str1, m_pCurWing->m_TTwist[m_pCurWing->m_NPanel]);
			glRasterPos2d(LeftPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);
			ZPos -=dD;
			str1.LoadString(IDS_AR);
			Result.Format(str1, m_pCurWing->m_AR);
			glRasterPos2d(LeftPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);
			ZPos -=dD;
			str1.LoadString(IDS_TR);
			Result.Format(str1, m_pCurWing->m_TR);
			glRasterPos2d(LeftPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);
			ZPos -=dD;
			str1.LoadString(IDS_RTTIPSWEEP);
			Result.Format(str1, m_pCurWing->GetAverageSweep());
			glRasterPos2d(LeftPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);
		}

		//END Write wing data
	}
	glEndList();

	if(m_bglLight){
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
}

void CMiarex::GLDrawBodyLegend()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CChildView * pChildView = (CChildView*)(m_pChildWnd);
	CString strong;
	CString strLengthUnit;
	CW3DBar *pW3DBar = (CW3DBar*)m_pW3DBar;
	int style, width;
	double r,g,b;
	COLORREF color;

	GetLengthUnit(strLengthUnit, pFrame->m_LengthUnit);
	int unitlength = strLengthUnit.GetLength();
	double h = (double)m_rCltRect.Height();
	double w = (double)m_rCltRect.Width();
	double dD      = 12.0/w*2.0*m_GLScale;

//	glNewList(BODYLEGEND,GL_COMPILE);//create 2D Splines or Lines

		//draw view rectangles

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = pChildView->m_WndTextColor;
		style = 0;
		width = 3;
		GetBWColor(color, style, width);
		glLineWidth((float)width);

		glLineStipple (1, 0xFFFF);// Solid

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		glBegin(GL_LINES);
			glVertex2d(                -m_GLScale, m_HorizontalSplit*m_GLScale);
			glVertex2d(m_VerticalSplit* m_GLScale, m_HorizontalSplit*m_GLScale);
		glEnd();

		glBegin(GL_LINES);
			glVertex2d(m_VerticalSplit*m_GLScale, (-1.0+2.0*h/w)*m_GLScale);
			glVertex2d(m_VerticalSplit*m_GLScale,  -1.0         *m_GLScale);
		glEnd();

		if(m_pCurBody)
		{
			//draw the legend
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);

			DecompRGB(pChildView->m_WndTextColor, r,g,b);
			glColor3d(r,g,b);
			glListBase(GLF_START_LIST); 

			// Draw the labels
		
			double XFrameLegendPos     = (m_VerticalSplit +dD) * m_GLScale;
			double YFrameLegendPos     =  (-1.0+2.0*h/w -dD)    * m_GLScale;
			double XBodyLineLegendPos  = (-1.0 + dD)        * m_GLScale;
			double YBodyLineLegendPos  = (-1.0+2.0*h/w -0.03)  * m_GLScale;
	//		double dy        = -0.02                           * m_GLScale;

			CVector real;
			CPoint point;
			GetCursorPos(&point);
			m_pChildWnd->ScreenToClient(&point);
			ClientToGL(point, real);


			glRasterPos2d(XFrameLegendPos, YFrameLegendPos);
			strong.Format("Frame %2d", m_pCurBody->m_iActiveFrame+1);
			glCallLists(8, GL_UNSIGNED_BYTE, strong);

			YFrameLegendPos -= dD;
			glRasterPos2d(XFrameLegendPos, YFrameLegendPos);
			strong.Format("Scale = %4.2f", m_FrameScale/m_BodyRefScale);
			glCallLists(12, GL_UNSIGNED_BYTE, strong);


			glRasterPos2d(XBodyLineLegendPos, YBodyLineLegendPos);
			strong.Format("Scale = %4.2f", m_BodyScale/m_BodyRefScale);
			glCallLists(12, GL_UNSIGNED_BYTE, strong);


			if(m_FrameRect.PtInRect(point))
			{
				real.x =  (real.x - m_FrameOffset.x)/m_FrameScale;
				real.y =  (real.y - m_FrameOffset.y)/m_FrameScale;

				YFrameLegendPos -= dD;
				glRasterPos2d(XFrameLegendPos,YFrameLegendPos);
				strong.Format("x = %9.3f ", real.x * pFrame->m_mtoUnit);
				strong += strLengthUnit;
				glCallLists(14 + unitlength, GL_UNSIGNED_BYTE, strong);

				YFrameLegendPos -= dD;
				glRasterPos2d(XFrameLegendPos,YFrameLegendPos);
				strong.Format("y = %9.3f ", real.y * pFrame->m_mtoUnit);
				strong += strLengthUnit;
				glCallLists(14 + unitlength, GL_UNSIGNED_BYTE, strong);
			}
			else if(m_BodyLineRect.PtInRect(point))
			{
				real.x =  (real.x - m_BodyOffset.x)/m_BodyScale;
				real.y =  (real.y - m_BodyOffset.y)/m_BodyScale;

				YBodyLineLegendPos -= dD;
				glRasterPos2d(XBodyLineLegendPos,YBodyLineLegendPos);
				strong.Format("x = %9.3f ", real.x * pFrame->m_mtoUnit);
				strong += strLengthUnit;
				glCallLists(14 + unitlength, GL_UNSIGNED_BYTE, strong);

				YBodyLineLegendPos -= dD;
				glRasterPos2d(XBodyLineLegendPos,YBodyLineLegendPos);
				strong.Format("y = %9.3f ", real.y * pFrame->m_mtoUnit);
				strong += strLengthUnit;
				glCallLists(14 + unitlength, GL_UNSIGNED_BYTE, strong);
			}
		}

//	glEndList();
}

void CMiarex::ClientToGL(CPoint const &point, CVector &real)
{
	double h = (double)m_rCltRect.Height();
	double w = (double)m_rCltRect.Width();
	double middle = ((double)m_rCltRect.left + (double)m_rCltRect.right)/2.0;

	real.x = (((double)point.x - middle) / (double)m_rCltRect.Width()*2.0)  * m_GLScale;
	real.y = -1.0 +2.0 *(h - (double)point.y)/w;

}

void CMiarex::GLToClient(CVector const &real, CPoint &point)
{
	double h = (double)m_rCltRect.Height();
	double w = (double)m_rCltRect.Width();

//	real.x /= m_GLScale;
//	real.y /= m_GLScale;
	int x   = (int)( real.x/m_GLScale * m_rCltRect.Width()/2.0);

	point.x = (int)(          m_rCltRect.Width()/2.0) + x;
	point.y = -(int)((real.y/m_GLScale+1.0)*w/2.0 - h);

}

void CMiarex::GLCreateCpLegend()
{
	int i;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString strong;
	CW3DBar *pW3DBar = (CW3DBar*)m_pW3DBar;
	CChildView * pChildView = (CChildView*)(m_pChildWnd);
	int maxlength;
	double r,g,b, f, fi,gh, dD, ZPos,dz,Right1, Right2, RightLetter, width, height;
	double color = 0.0;
	double range, delta;
	glNewList(WOPPCPLEGEND,GL_COMPILE);
		m_GLList++;
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		glPolygonMode(GL_FRONT,GL_FILL);
		if(IsBlackAndWhite())	DecompRGB(0, r, g, b);
		else					DecompRGB(pChildView->m_WndTextColor, r,g,b);
		glColor3d(r,g,b);
		glListBase(GLF_START_LIST); 
		// Draw the labels
		width  = (double)pChildView->m_rCltRect.Width();
		height = (double)pChildView->m_rCltRect.Height();
		maxlength = 7;
		gh = height/width;
		dD      = 12.0/width*2.0     *m_GLScale;
		ZPos    = (-1.0 + gh/2.0)    *m_GLScale;
		dz      = gh*1.2/20.0        *m_GLScale;
		Right1  = .94                *m_GLScale;
		Right2  = .98                *m_GLScale;
		RightLetter = m_GLScale * (1.0- (double)(maxlength*pChildView->m_LetterWidth) *2.0/(double)width - 0.07) *m_GLScale;

		range = (m_LegendMax - m_LegendMin);
		delta = range / 20;
		for (i=0; i<=20; i ++)
		{
			f = m_LegendMin + (double)i * delta;
			fi = (double)i*dz;
			strong.Format("%7.2f",f);
			glRasterPos2d(RightLetter,ZPos+fi);
			glCallLists(maxlength, GL_UNSIGNED_BYTE, strong);
		}
		glRasterPos2d(Right1,ZPos+21.0*dz);
		glCallLists(2, GL_UNSIGNED_BYTE, "Cp");

		glBegin(GL_QUAD_STRIP);
			for (i=0; i<=20; i++)
			{
				fi = (double)i*dz;
				color += 0.05;
				if(IsBlackAndWhite())	glColor3d(color,color,color);
				else					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
				glVertex3d(Right1, ZPos+fi, 0.0);
				glVertex3d(Right2, ZPos+fi, 0.0);
			}
		glEnd();
	glEndList();
}
			

void CMiarex::GLCreateWOppLegend()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CW3DBar *pW3DBar = (CW3DBar*)m_pW3DBar;
	CChildView * pChildView = (CChildView*)m_pChildWnd;
	double r,g,b,dD, ZPos, XPos;
	int i,l,len;

	glListBase(GLF_START_LIST); 

// Write wing data
	CString Result, str, strong, str1;
	double width  = (double)m_rCltRect.Width();
	double height = (double)m_rCltRect.Height();
	int maxlength = 40;

	dD   = 12.0/width*2.0*m_GLScale;
	ZPos = -m_GLScale + 11.0*dD +0.02*m_GLScale;
	XPos =  m_GLScale * (1.0 - (double)(maxlength*pChildView->m_LetterWidth)*2.0/width);

	glNewList(WOPPLEGEND,GL_COMPILE);
	{
		m_GLList++;
		if(IsBlackAndWhite())	DecompRGB(0, r,g,b);
		else					DecompRGB(pChildView->m_WndTextColor, r,g,b);
		glColor3d(r,g,b);
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		if(m_pCurWOpp)
		{
			ZPos += m_pCurWOpp->m_nFlaps * dD;
			if(m_pCurWOpp->m_bOut) 
			{
				ZPos += dD;
				Result.LoadString(IDS_POINTOUT);
				ZPos -=dD;
				len = Result.GetLength();
				for(l=0; l<maxlength-len; l++) Result =" "+Result;
				glRasterPos2d(XPos,ZPos);
				glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);
			}
			GetSpeedUnit(str, pFrame->m_SpeedUnit);
			l = str.GetLength();
			if     (l==2) Result.Format("QInf = %7.2f ", m_pCurWOpp->m_QInf*pFrame->m_mstoUnit);
			else if(l==3) Result.Format("QInf = %6.1f ", m_pCurWOpp->m_QInf*pFrame->m_mstoUnit);
			else if(l==4) Result.Format("QInf = %5.1f ", m_pCurWOpp->m_QInf*pFrame->m_mstoUnit);

			Result += str;
			ZPos -=dD;
			len = Result.GetLength();
			for(l=0; l<maxlength-len; l++) Result =" "+Result;
			glRasterPos2d(XPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);

			str1.LoadString(IDS_ALPHA);
			Result.Format(str1, m_pCurWOpp->m_Alpha);
			ZPos -=dD;
			len = Result.GetLength();
			for(l=0; l<maxlength-len; l++) Result =" "+Result;
			glRasterPos2d(XPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);

			str1.LoadString(IDS_LIFTCOEF);
			Result.Format(str1, m_pCurWOpp->m_CL);
			ZPos -=dD;
			len = Result.GetLength();
			for(l=0; l<maxlength-len; l++) Result =" "+Result;
			glRasterPos2d(XPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);

			str1.LoadString(IDS_DRAGCOEF);
			Result.Format(str1, m_pCurWOpp->m_ViscousDrag+m_pCurWOpp->m_InducedDrag);
			ZPos -=dD;
			len = Result.GetLength();
			for(l=0; l<maxlength-len; l++) Result =" "+Result;
			glRasterPos2d(XPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);

			double cxielli=m_pCurWOpp->m_CL*m_pCurWOpp->m_CL/pi/m_pCurWing->m_AR;
			Result.Format("Oswald = %9.4f ", cxielli/m_pCurWOpp->m_InducedDrag);
			ZPos -=dD;
			len = Result.GetLength();
			for(l=0; l<maxlength-len; l++) Result =" "+Result;
			glRasterPos2d(XPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);

			Result.Format("Cl/Cd = %9.4f ", 
				m_pCurWOpp->m_CL/(m_pCurWOpp->m_InducedDrag+m_pCurWOpp->m_ViscousDrag));
			ZPos -=dD;
			len = Result.GetLength();
			for(l=0; l<maxlength-len; l++) Result =" "+Result;
			glRasterPos2d(XPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);

			Result.Format("GCm = %9.4f ", 	m_pCurWOpp->m_GCm);
			ZPos -=dD;
			len = Result.GetLength();
			for(l=0; l<maxlength-len; l++) Result =" "+Result;
			glRasterPos2d(XPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);

			str1.LoadString(IDS_ROLLINGMOMENT);
			Result.Format(str1, m_pCurWOpp->m_GRm);
			ZPos -=dD;
			len = Result.GetLength();
			for(l=0; l<maxlength-len; l++) Result =" "+Result;
			glRasterPos2d(XPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);

			str1.LoadString(IDS_INDUCEDMOMENT);
			Result.Format(str1, m_pCurWOpp->m_IYm);
			ZPos -=dD;
			len = Result.GetLength();
			for(l=0; l<maxlength-len; l++) Result =" "+Result;
			glRasterPos2d(XPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);

			str1.LoadString(IDS_AIRFOILYAWING);
			Result.Format(str1, m_pCurWOpp->m_GYm);
			ZPos -=dD;
			len = Result.GetLength();
			for(l=0; l<maxlength-len; l++) Result =" "+Result;
			glRasterPos2d(XPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);

			GetLengthUnit(str, pFrame->m_LengthUnit);
			l = str.GetLength();
			if (l==1)     Result.Format("XCP = %8.3f ", m_pCurWOpp->m_XCP*pFrame->m_mtoUnit);
			else if(l==2) Result.Format("XCP = %7.2f ", m_pCurWOpp->m_XCP*pFrame->m_mtoUnit);
			else if(l>=3) Result.Format("XCP = %7.2f ", m_pCurWOpp->m_XCP*pFrame->m_mtoUnit);
			Result += str;
			ZPos -=dD;
			len = Result.GetLength();
			for(l=0; l<maxlength-len; l++) Result =" "+Result;
			glRasterPos2d(XPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);

			for(i=0; i<m_pCurWOpp->m_nFlaps; i++)
			{
				str1.LoadString(IDS_FLAPMOMENT);
				Result.Format(str1, i+1, m_pCurWOpp->m_FlapMoment[i]*pFrame->m_NmtoUnit);
				GetMomentUnit(str, pFrame->m_MomentUnit);
				Result += str;
				ZPos -=dD;
				glRasterPos2d(XPos,ZPos);
				len = Result.GetLength();
				for(l=0; l<maxlength-len; l++) Result =" "+Result;
				glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);
			}
		}
	}
	glEndList();

	if(m_bglLight)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
}


void CMiarex::GLCreateCtrlPts()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	glNewList(VLMCTRLPTS,GL_COMPILE);
	{
		m_GLList++;
//		glPolygonMode(GL_FRONT,GL_FILL);
		glLineWidth(1.0);
		glColor3d(0.0,1.0,0.0);
		for (int p=0; p<m_MatSize; p++)
		{
			//All panels
			glBegin(GL_LINES);
			{
				glVertex3d(m_Panel[p].CtrlPt.x, m_Panel[p].CtrlPt.y, m_Panel[p].CtrlPt.z);
				glVertex3d((m_Panel[p].CtrlPt.x + m_Panel[p].Normal.x * 0.04),
						   (m_Panel[p].CtrlPt.y + m_Panel[p].Normal.y * 0.04),
						   (m_Panel[p].CtrlPt.z + m_Panel[p].Normal.z * 0.04));
/*				glVertex3d(m_Panel[p].CollPt.x, m_Panel[p].CollPt.y, m_Panel[p].CollPt.z);
				glVertex3d(m_Panel[p].CollPt.x+m_Panel[p].Normal.x *0.01,
					       m_Panel[p].CollPt.y+m_Panel[p].Normal.y *0.01, 
						   m_Panel[p].CollPt.z+m_Panel[p].Normal.z *0.01);
				glVertex3d(m_Panel[p].CollPt.x, m_Panel[p].CollPt.y, m_Panel[p].CollPt.z);
				glVertex3d(m_Panel[p].CollPt.x+m_Panel[p].l.x *0.01,
					       m_Panel[p].CollPt.y+m_Panel[p].l.y *0.01, 
						   m_Panel[p].CollPt.z+m_Panel[p].l.z *0.01);*/
			}
			glEnd();
		}
	}
	glEndList();
}

void CMiarex::GLCreateVortices()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	int p;
	CVector A, B, C, D, AC, BD;
	glEnable (GL_LINE_STIPPLE);
	glLineStipple (1, 0xFFFF);

	glNewList(VLMVORTICES,GL_COMPILE);
	{
		m_GLList++;

		glPolygonMode(GL_FRONT,GL_LINE);
		glLineWidth(1.0);
		glColor3d(0.7,0.0,0.0);
		for (p=0; p<m_MatSize; p++)
		{
			if(!m_Panel[p].m_bIsTrailing)
			{
				if(m_Panel[p].m_iPos<=0)
				{
					A = m_Panel[p].A;
					B = m_Panel[p].B;
					C = m_Panel[p-1].B;
					D = m_Panel[p-1].A;
				}
				else 
				{
					A = m_Panel[p].A;
					B = m_Panel[p].B;
					C = m_Panel[p+1].B;
					D = m_Panel[p+1].A;
				}
			}
			else
			{
				A = m_Panel[p].A;
				B = m_Panel[p].B;
				// we define point AA=A+1 and BB=B+1
				C.x =  m_Node[m_Panel[p].m_iTB].x 
					+ (m_Node[m_Panel[p].m_iTB].x-m_Panel[p].B.x)/3.0;
				C.y =  m_Node[m_Panel[p].m_iTB].y;
				C.z =  m_Node[m_Panel[p].m_iTB].z;
				D.x =  m_Node[m_Panel[p].m_iTA].x 
					+ (m_Node[m_Panel[p].m_iTA].x-m_Panel[p].A.x)/3.0;
				D.y =  m_Node[m_Panel[p].m_iTA].y;
				D.z =  m_Node[m_Panel[p].m_iTA].z;
			}

			//next we "shrink" the points to avoid confusion with panels sides
			AC = C-A;
			BD = D-B;
	
			AC *= 0.03;
			A  += AC;
			C  -= AC; 
			BD *= 0.03;
			B  += BD;
			D  -= BD;


			if(m_pCurWPolar && m_pCurWPolar->m_bVLM1)
			{
				glLineStipple (1, 0xFFFF);
				glBegin(GL_LINES);
				{
					glVertex3d(A.x, A.y, A.z);
					glVertex3d(B.x, B.y, B.z);
				}
				glEnd();
				glLineStipple (1, 0x0F0F);
				glBegin(GL_LINES);
				{
					glVertex3d(A.x, A.y, A.z);
					glVertex3d(D.x, D.y, D.z);
				}
				glEnd();
				glBegin(GL_LINES);
				{
					glVertex3d(B.x, B.y, B.z);
					glVertex3d(C.x, C.y, C.z);
				}
				glEnd();
			}
			else if(!m_pCurWPolar || (m_pCurWPolar && !m_pCurWPolar->m_bVLM1))
			{
				glBegin(GL_LINE_STRIP);
				{
					glVertex3d(A.x, A.y, A.z);
					glVertex3d(B.x, B.y, B.z);
					glVertex3d(C.x, C.y, C.z);
					glVertex3d(D.x, D.y, D.z);
					glVertex3d(A.x, A.y, A.z);
				}
				glEnd();
			}
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}

void CMiarex::GLCreateBodyBezier()
{
	int i,j,l;
	l=0;
	double r,g,b;
	COLORREF color;
	int style, width;
	CMainFrame *pMainFrame = (CMainFrame*)m_pFrame;
	CFrame *pFrame;
	GLfloat rightpts[MAXBODYFRAMES*30*3];
	GLfloat leftpts[MAXBODYFRAMES*30*3];

	if(!m_pCurBody)
		return;
	
	for(i=0;i<m_pCurBody->m_NStations; i++)
	{
		pFrame = m_pCurBody->m_Frame+i;
		for(j=0;j<pFrame->m_NPoints; j++)
		{
			if(pFrame->m_NPoints != m_pCurBody->m_NSideLines) ASSERT(FALSE);

			rightpts[l+0]= (GLfloat)(m_pCurBody->m_FramePosition[i].x);
			rightpts[l+1]= (GLfloat)pFrame->m_Point[j].y;
			rightpts[l+2]= (GLfloat)pFrame->m_Point[j].z;

			l += 3;
		}
	}

	l=0;
	for(i=0;i<m_pCurBody->m_NStations; i++)
	{
		pFrame = m_pCurBody->m_Frame+i;
		for(j=pFrame->m_NPoints-1;j>=0; j--)
		{
			if(pFrame->m_NPoints != m_pCurBody->m_NSideLines) ASSERT(FALSE);

			leftpts[l+0]=  (GLfloat)(m_pCurBody->m_FramePosition[i].x);
			leftpts[l+1]= -(GLfloat)pFrame->m_Point[j].y;
			leftpts[l+2]=  (GLfloat)pFrame->m_Point[j].z;

			l += 3;
		}
	}

	glNewList(BODYSURFACES,GL_COMPILE);
		m_GLList++;
//		glShadeModel(GL_FLAT);
		color = m_pCurBody->m_BodyColor;
		GetBWColor(color, style, width);
		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, m_pCurBody->m_NSideLines*3, m_pCurBody->m_NStations, 
								  0.0, 1.0, 3,                          m_pCurBody->m_NSideLines,
				rightpts);

		glPolygonMode(GL_FRONT,GL_FILL);
		glEnable(GL_MAP2_VERTEX_3);
		glEnable(GL_AUTO_NORMAL);
		glMapGrid2f(8, 0.0, 1.0, 8, 0.0, 1.0);

//		glPushMatrix ();
			glEvalMesh2(GL_FILL, 0, 8, 0, 8);
	/*		for (j = 0; j <= 8; j++) {
				glBegin(GL_LINE_STRIP);
					for (i = 0; i <= 15; i++)
						glEvalCoord2f((GLfloat)i/15.0, (GLfloat)j/8.0);
				glEnd();
				glBegin(GL_LINE_STRIP);
					for (i = 0; i <= 15; i++)
						glEvalCoord2f((GLfloat)j/8.0, (GLfloat)i/15.0);
				glEnd();
			}*/
//		glPopMatrix();

		glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, m_pCurBody->m_NSideLines*3, m_pCurBody->m_NStations, 
								  0.0, 1.0, 3,                          m_pCurBody->m_NSideLines,
				leftpts);

//		glEnable(GL_MAP2_VERTEX_3);
//		glEnable(GL_AUTO_NORMAL);
//		glMapGrid2f(8, 0.0, 1.0, 8, 0.0, 1.0);

//		glPushMatrix ();
			glEvalMesh2(GL_FILL, 0, 8, 0, 8);
	/*		for (j = 0; j <= 8; j++) {
				glBegin(GL_LINE_STRIP);
					for (i = 0; i <= 15; i++)
						glEvalCoord2f((GLfloat)i/15.0, (GLfloat)j/8.0);
				glEnd();
				glBegin(GL_LINE_STRIP);
					for (i = 0; i <= 15; i++)
						glEvalCoord2f((GLfloat)j/8.0, (GLfloat)i/15.0);
				glEnd();
			}*/
//		glPopMatrix();

//		glFlush();
	glEndList();

	glNewList(BODYGEOM,GL_COMPILE);
		m_GLList++;

		glLineWidth((GLfloat)GetPenWidth(pMainFrame->m_OutlineWidth, false));

		color = pMainFrame->m_OutlineColor;
		style = pMainFrame->m_OutlineStyle;
		width = pMainFrame->m_OutlineWidth;
		GetBWColor(color, style, width);

		if     (style == PS_DOT) 		glLineStipple (1, 0x1111);
		else if(style == PS_DASH) 		glLineStipple (1, 0x0F0F);
		else if(style == PS_DASHDOT) 	glLineStipple (1, 0x1C47);
		else							glLineStipple (1, 0xFFFF);

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, m_pCurBody->m_NSideLines*3, m_pCurBody->m_NStations, 
								  0.0, 1.0, 3,                          m_pCurBody->m_NSideLines,
				rightpts);

		glPolygonMode(GL_FRONT,GL_LINE);
		glEnable(GL_MAP2_VERTEX_3);
		glEnable(GL_AUTO_NORMAL);
		glMapGrid2f(8, 0.0, 1.0, 8, 0.0, 1.0);

		glEvalMesh2(GL_LINE, 0, 8, 0, 8);


		glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, m_pCurBody->m_NSideLines*3, m_pCurBody->m_NStations, 
								  0.0, 1.0, 3,                          m_pCurBody->m_NSideLines,
				leftpts);

		glEvalMesh2(GL_LINE, 0, 8, 0, 8);
	glEndList();
}


void CMiarex::GLCreateBodySurface()
{
	int i,j,k,l;
	int p, style, width, nx, nh;
	double v;

	CVector Point;
	double xinc, hinc, u;
	CVector N, LATB, TALB;
	CVector LA, LB, TA, TB;
	double r,g,b;
	COLORREF color;
	CMainFrame *pMainFrame = (CMainFrame*)m_pFrame;
	CChildView *pView = (CChildView*)m_pChildWnd;

	nx = min(500, m_NXPoints);
	nh = max(m_NHoopPoints, 3);

	if(nx*nh>=10000)
	{
		CString strong;
		strong.Format("Resolution cannot exceed %d", 10000);
		AfxMessageBox(strong, MB_OK);
		return;
	}

	p = 0;
	for (k=0; k<=nx; k++)
	{
		u = (double)k / (double)nx;	
		for (l=0; l<=nh; l++)
		{
			v = (double)l / (double)nh; 
			m_pCurBody->GetPoint(u,  v, true, m_T[p]);
			p++;
		}
	}

	glNewList(BODYSURFACES,GL_COMPILE);
	{
		m_GLList++;

		color = m_pCurBody->m_BodyColor;
		GetBWColor(color, style, width);
		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);
		glLineWidth(1.0);
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT,GL_FILL);

		//right side first;
		p=0;
		for (k=0; k<nx; k++)
		{
			glBegin(GL_QUAD_STRIP);
			{
				LATB = m_T[p+nh+1] - m_T[p+1];     //	LATB = TB - LA;
				TALB = m_T[p]  - m_T[p+nh+2];      //	TALB = LB - TA;
				N = TALB * LATB;
				N.Normalize();

				glNormal3d(N.x, N.y, N.z);

				glVertex3d(m_T[p].x, m_T[p].y, m_T[p].z);
				glVertex3d(m_T[p+nh+1].x, m_T[p+nh+1].y, m_T[p+nh+1].z);

				p++;

				for (l=1; l<=nh; l++)
				{
					LATB = m_T[p+nh] - m_T[p];     //	LATB = TB - LA;
					TALB = m_T[p-1]  - m_T[p+nh+1];//	TALB = LB - TA;
					N = TALB * LATB;
					N.Normalize();

					glNormal3d(N.x, N.y, N.z);
					glVertex3d(m_T[p].x,      m_T[p].y,      m_T[p].z);
					glVertex3d(m_T[p+nh+1].x, m_T[p+nh+1].y, m_T[p+nh+1].z);
					TB = TA;
					LB = LA;
					p++;
				}
			}
			glEnd();
		}
		//left side next;
		p=0;
		for (k=0; k<nx; k++)
		{
			glBegin(GL_QUAD_STRIP);
			{
				LATB = m_T[p+nh+1] - m_T[p+1];//	LATB = TB - LA;
				TALB = m_T[p]  - m_T[p+nh+2]; //	TALB = LB - TA;
				N = TALB * LATB;
				N.Normalize();

				glNormal3d(N.x, -N.y, N.z);

				glVertex3d(m_T[p+nh+1].x, -m_T[p+nh+1].y, m_T[p+nh+1].z);
				glVertex3d(m_T[p].x,      -m_T[p].y, m_T[p].z);

				p++;

				for (l=1; l<=nh; l++)
				{
					LATB = m_T[p+nh] - m_T[p];     //	LATB = TB - LA;
					TALB = m_T[p-1]  - m_T[p+nh+1];//	TALB = LB - TA;
					N = TALB * LATB;
					N.Normalize();

					glNormal3d(N.x, -N.y, N.z);
					glVertex3d(m_T[p+nh+1].x, -m_T[p+nh+1].y, m_T[p+nh+1].z);
					glVertex3d(m_T[p].x,      -m_T[p].y,      m_T[p].z);
					TB = TA;
					LB = LA;
					p++;
				}
			}
			glEnd();
		}

		glDisable(GL_DEPTH_TEST);
	}
	glEndList();

	glNewList(BODYGEOM,GL_COMPILE);
	{
		m_GLList++;

		glLineWidth((GLfloat)GetPenWidth(pMainFrame->m_OutlineWidth, false));

		color = pMainFrame->m_OutlineColor;
		style = pMainFrame->m_OutlineStyle;
		width = pMainFrame->m_OutlineWidth;
		GetBWColor(color, style, width);

		if     (style == PS_DOT) 		glLineStipple (1, 0x1111);
		else if(style == PS_DASH) 		glLineStipple (1, 0x0F0F);
		else if(style == PS_DASHDOT) 	glLineStipple (1, 0x1C47);
		else							glLineStipple (1, 0xFFFF);

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		xinc = 0.1; hinc=1./(double)(nh-1);
		u=0.0; v = 0.0;

		// sides
		for (i=1; i<m_pCurBody->m_NStations-1; i++)
		{
			u = m_pCurBody->Getu(m_pCurBody->m_FramePosition[i].x);

			glBegin(GL_LINE_STRIP);
			{
				v = 0.0;
				for (j=0; j<nh; j++)
				{
					m_pCurBody->GetPoint(u,v,true, Point);
					glVertex3d(Point.x, Point.y, Point.z);
					v += hinc; 
				}
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			{
				v = 0.0;
				for (j=0; j<nh; j++)
				{
					m_pCurBody->GetPoint(u,v,false, Point);
					glVertex3d(Point.x,Point.y, Point.z);
					v += hinc; 
				}
			}
			glEnd();
		}

		//top line
		u=0.0; 
		glBegin(GL_LINE_STRIP);
		{
			v = 0.0;
			for (i=0; i<=nh; i++)
			{
				m_pCurBody->GetPoint(u,v, true, Point);
				glVertex3d(Point.x, Point.y, Point.z);
				u += xinc;
			}
		}
		glEnd();
		
		//bot line
		u=0.0; 
		glBegin(GL_LINE_STRIP);
		{
			v = 1.0;
			for (i=0; i<=nh; i++)
			{
				m_pCurBody->GetPoint(u,v, true, Point);
				glVertex3d(Point.x, Point.y, Point.z);
				u += xinc;
			}
		}
		glEnd();
	}
	glEndList();
}



void CMiarex::GLCreateBodyMesh()
{
	if(!m_pCurBody)
	{
		glNewList(BODYMESHPANELS,GL_COMPILE);
			m_GLList++;
		glEndList();
		return;
	}

	int i,j,k,l;
	int p, style, width, nx, nh;
	double uk, v, dj, dj1, dl1;
	CVector Point, N, LATB, TALB, LA, LB, TA, TB;
	CVector PLA, PLB, PTA, PTB;
	double r,g,b;
	COLORREF color;

	CMainFrame *pMainFrame = (CMainFrame*)m_pFrame;
	CChildView *pView = (CChildView*)m_pChildWnd;
	
	nx = m_pCurBody->m_nxPanels;
	nh = m_pCurBody->m_nhPanels;

	if(m_pCurBody->m_LineType==1) //LINES
	{
		glNewList(BODYMESHPANELS,GL_COMPILE);
		{
			m_GLList++;

			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable (GL_LINE_STIPPLE);

			color = pMainFrame->m_VLMColor;
			style = pMainFrame->m_VLMStyle;
			width = pMainFrame->m_VLMWidth;
			GetBWColor(color, style, width);
			DecompRGB(color,r,g,b);
			glColor3d(r,g,b);

			glLineWidth(1.0);

			for (i=0; i<m_pCurBody->m_NStations-1; i++)
			{
				for (j=0; j<m_pCurBody->m_xPanels[i]; j++)
				{
					dj  = (double) j   /(double)(m_pCurBody->m_xPanels[i]);
					dj1 = (double)(j+1)/(double)(m_pCurBody->m_xPanels[i]);

					//body left side
					for (k=0; k<m_pCurBody->m_NSideLines-1; k++)
					{
						//build the four corner points of the strips 
						PLB.x =  (1.0- dj) * m_pCurBody->m_FramePosition[i].x      +  dj * m_pCurBody->m_FramePosition[i+1].x;
						PLB.y = -(1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k].y   -  dj * m_pCurBody->m_Frame[i+1].m_Point[k].y;
						PLB.z =  (1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k].z   +  dj * m_pCurBody->m_Frame[i+1].m_Point[k].z;

						PTB.x =  (1.0-dj1) * m_pCurBody->m_FramePosition[i].x      + dj1 * m_pCurBody->m_FramePosition[i+1].x;
						PTB.y = -(1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k].y   - dj1 * m_pCurBody->m_Frame[i+1].m_Point[k].y;
						PTB.z =  (1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k].z   + dj1 * m_pCurBody->m_Frame[i+1].m_Point[k].z;

						PLA.x =  (1.0- dj) * m_pCurBody->m_FramePosition[i].x      +  dj * m_pCurBody->m_FramePosition[i+1].x;
						PLA.y = -(1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k+1].y -  dj * m_pCurBody->m_Frame[i+1].m_Point[k+1].y;
						PLA.z =  (1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k+1].z +  dj * m_pCurBody->m_Frame[i+1].m_Point[k+1].z;

						PTA.x =  (1.0-dj1) * m_pCurBody->m_FramePosition[i].x      + dj1 * m_pCurBody->m_FramePosition[i+1].x;
						PTA.y = -(1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k+1].y - dj1 * m_pCurBody->m_Frame[i+1].m_Point[k+1].y;
						PTA.z =  (1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k+1].z + dj1 * m_pCurBody->m_Frame[i+1].m_Point[k+1].z;

						glBegin(GL_QUAD_STRIP);
						{
							N.Set(0.0, 0.0, 1.0);//top line normal is vertical

							LB = PLB; 
							TB = PTB;
							glVertex3d(LB.x, LB.y, LB.z);
							glVertex3d(TB.x, TB.y, TB.z);

							for (l=0; l<m_pCurBody->m_hPanels[k]; l++)
							{
								dl1  = (double) (l+1)   /(double)(m_pCurBody->m_hPanels[k]);
								LA = PLB * (1.0- dl1) + PLA * dl1;
								TA = PTB * (1.0- dl1) + PTA * dl1;
								
								LATB = TB - LA;
								TALB = LB - TA;
								N = TALB * LATB;
								N.Normalize();

								glNormal3d(N.x, N.y, N.z);
								glVertex3d(LA.x, LA.y, LA.z);
								glVertex3d(TA.x, TA.y, TA.z);
								TB = TA;
								LB = LA;
							}
						}
						glEnd();
					}
					//body right side
					for (k=m_pCurBody->m_NSideLines-2; k>=0; k--)
					{
						//build the four corner points of the strips 
						PLA.x = (1.0- dj) * m_pCurBody->m_FramePosition[i].x      +  dj * m_pCurBody->m_FramePosition[i+1].x;
						PLA.y = (1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k].y   +  dj * m_pCurBody->m_Frame[i+1].m_Point[k].y;
						PLA.z = (1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k].z   +  dj * m_pCurBody->m_Frame[i+1].m_Point[k].z;

						PTA.x = (1.0-dj1) * m_pCurBody->m_FramePosition[i].x      + dj1 * m_pCurBody->m_FramePosition[i+1].x;
						PTA.y = (1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k].y   + dj1 * m_pCurBody->m_Frame[i+1].m_Point[k].y;
						PTA.z = (1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k].z   + dj1 * m_pCurBody->m_Frame[i+1].m_Point[k].z;

						PLB.x = (1.0- dj) * m_pCurBody->m_FramePosition[i].x      +  dj * m_pCurBody->m_FramePosition[i+1].x;
						PLB.y = (1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k+1].y +  dj * m_pCurBody->m_Frame[i+1].m_Point[k+1].y;
						PLB.z = (1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k+1].z +  dj * m_pCurBody->m_Frame[i+1].m_Point[k+1].z;

						PTB.x = (1.0-dj1) * m_pCurBody->m_FramePosition[i].x      + dj1 * m_pCurBody->m_FramePosition[i+1].x;
						PTB.y = (1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k+1].y + dj1 * m_pCurBody->m_Frame[i+1].m_Point[k+1].y;
						PTB.z = (1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k+1].z + dj1 * m_pCurBody->m_Frame[i+1].m_Point[k+1].z;

						glBegin(GL_QUAD_STRIP);
						{
							N.Set(0.0, 0.0, 1.0);//top line normal is vertical

							LB = PLB; 
							TB = PTB;
							glVertex3d(LB.x, LB.y, LB.z);
							glVertex3d(TB.x, TB.y, TB.z);

							for (l=0; l<m_pCurBody->m_hPanels[k]; l++)
							{
								dl1  = (double) (l+1)   /(double)(m_pCurBody->m_hPanels[k]);
								LA = PLB * (1.0- dl1) + PLA * dl1;
								TA = PTB * (1.0- dl1) + PTA * dl1;
								
								LATB = TB - LA;
								TALB = LB - TA;
								N = TALB * LATB;
								N.Normalize();

								glNormal3d(N.x, N.y, N.z);
								glVertex3d(LA.x, LA.y, LA.z);
								glVertex3d(TA.x, TA.y, TA.z);
								TB = TA;
								LB = LA;
							}
						}
						glEnd();
					}
				}
			}
			glDisable (GL_LINE_STIPPLE);
		}
		glEndList();
		glNewList(BODYMESHBACK,GL_COMPILE);
		{
			m_GLList++;

			glDisable (GL_LINE_STIPPLE);
			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);	

			color = pView->m_crBackColor;
			style = pMainFrame->m_VLMStyle;
			width = pMainFrame->m_VLMWidth;
			GetBWColor(color, style, width);

			DecompRGB(color,r,g,b);
			glColor3d(r,g,b);

			glLineWidth(1.0);

			for (i=0; i<m_pCurBody->m_NStations-1; i++)
			{
				for (j=0; j<m_pCurBody->m_xPanels[i]; j++)
				{
					dj  = (double) j   /(double)(m_pCurBody->m_xPanels[i]);
					dj1 = (double)(j+1)/(double)(m_pCurBody->m_xPanels[i]);

					//body left side
					for (k=0; k<m_pCurBody->m_NSideLines-1; k++)
					{
						//build the four corner points of the strips 
						PLB.x =  (1.0- dj) * m_pCurBody->m_FramePosition[i].x      +  dj * m_pCurBody->m_FramePosition[i+1].x;
						PLB.y = -(1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k].y   -  dj * m_pCurBody->m_Frame[i+1].m_Point[k].y;
						PLB.z =  (1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k].z   +  dj * m_pCurBody->m_Frame[i+1].m_Point[k].z;

						PTB.x =  (1.0-dj1) * m_pCurBody->m_FramePosition[i].x      + dj1 * m_pCurBody->m_FramePosition[i+1].x;
						PTB.y = -(1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k].y   - dj1 * m_pCurBody->m_Frame[i+1].m_Point[k].y;
						PTB.z =  (1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k].z   + dj1 * m_pCurBody->m_Frame[i+1].m_Point[k].z;

						PLA.x =  (1.0- dj) * m_pCurBody->m_FramePosition[i].x      +  dj * m_pCurBody->m_FramePosition[i+1].x;
						PLA.y = -(1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k+1].y -  dj * m_pCurBody->m_Frame[i+1].m_Point[k+1].y;
						PLA.z =  (1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k+1].z +  dj * m_pCurBody->m_Frame[i+1].m_Point[k+1].z;

						PTA.x =  (1.0-dj1) * m_pCurBody->m_FramePosition[i].x      + dj1 * m_pCurBody->m_FramePosition[i+1].x;
						PTA.y = -(1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k+1].y - dj1 * m_pCurBody->m_Frame[i+1].m_Point[k+1].y;
						PTA.z =  (1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k+1].z + dj1 * m_pCurBody->m_Frame[i+1].m_Point[k+1].z;

						glBegin(GL_QUAD_STRIP);
						{
							N.Set(0.0, 0.0, 1.0);//top line normal is vertical

							LB = PLB; 
							TB = PTB;
							glVertex3d(LB.x, LB.y, LB.z);
							glVertex3d(TB.x, TB.y, TB.z);

							for (l=0; l<m_pCurBody->m_hPanels[k]; l++)
							{
								dl1  = (double) (l+1)   /(double)(m_pCurBody->m_hPanels[k]);
								LA = PLB * (1.0- dl1) + PLA * dl1;
								TA = PTB * (1.0- dl1) + PTA * dl1;
								
								LATB = TB - LA;
								TALB = LB - TA;
								N = TALB * LATB;
								N.Normalize();

								glNormal3d(N.x, N.y, N.z);
								glVertex3d(LA.x, LA.y, LA.z);
								glVertex3d(TA.x, TA.y, TA.z);
								TB = TA;
								LB = LA;
							}
						}
						glEnd();
					}
					//body right side
					for (k=m_pCurBody->m_NSideLines-1; k>=0; k--)
					{
						//build the four corner points of the strips 
						PLA.x = (1.0- dj) * m_pCurBody->m_FramePosition[i].x      +  dj * m_pCurBody->m_FramePosition[i+1].x;
						PLA.y = (1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k].y   +  dj * m_pCurBody->m_Frame[i+1].m_Point[k].y;
						PLA.z = (1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k].z   +  dj * m_pCurBody->m_Frame[i+1].m_Point[k].z;

						PTA.x = (1.0-dj1) * m_pCurBody->m_FramePosition[i].x      + dj1 * m_pCurBody->m_FramePosition[i+1].x;
						PTA.y = (1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k].y   + dj1 * m_pCurBody->m_Frame[i+1].m_Point[k].y;
						PTA.z = (1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k].z   + dj1 * m_pCurBody->m_Frame[i+1].m_Point[k].z;

						PLB.x = (1.0- dj) * m_pCurBody->m_FramePosition[i].x      +  dj * m_pCurBody->m_FramePosition[i+1].x;
						PLB.y = (1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k+1].y +  dj * m_pCurBody->m_Frame[i+1].m_Point[k+1].y;
						PLB.z = (1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k+1].z +  dj * m_pCurBody->m_Frame[i+1].m_Point[k+1].z;

						PTB.x = (1.0-dj1) * m_pCurBody->m_FramePosition[i].x      + dj1 * m_pCurBody->m_FramePosition[i+1].x;
						PTB.y = (1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k+1].y + dj1 * m_pCurBody->m_Frame[i+1].m_Point[k+1].y;
						PTB.z = (1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k+1].z + dj1 * m_pCurBody->m_Frame[i+1].m_Point[k+1].z;

						glBegin(GL_QUAD_STRIP);
						{
							N.Set(0.0, 0.0, 1.0);//top line normal is vertical

							LB = PLB; 
							TB = PTB;
							glVertex3d(LB.x, LB.y, LB.z);
							glVertex3d(TB.x, TB.y, TB.z);

							for (l=0; l<m_pCurBody->m_hPanels[k]; l++)
							{
								dl1  = (double) (l+1)   /(double)(m_pCurBody->m_hPanels[k]);
								LA = PLB * (1.0- dl1) + PLA * dl1;
								TA = PTB * (1.0- dl1) + PTA * dl1;
								
								LATB = TB - LA;
								TALB = LB - TA;
								N = TALB * LATB;
								N.Normalize();

								glNormal3d(N.x, N.y, N.z);
								glVertex3d(LA.x, LA.y, LA.z);
								glVertex3d(TA.x, TA.y, TA.z);
								TB = TA;
								LB = LA;
							}
						}
						glEnd();
					}
				}
			}
			glDisable (GL_LINE_STIPPLE);
		}
		glEndList();
	}
	else if(m_pCurBody->m_LineType==2) //NURBS
	{
		p = 0;
		for (k=0; k<=nx; k++)
		{
			uk  = m_pCurBody->s_XPanelPos[k];
			for (l=0; l<=nh; l++)
			{
				v = (double)l / (double)(nh); 
				m_pCurBody->GetPoint(uk,  v, true, m_L[p]);
				p++;
			}
		}
		glNewList(BODYMESHPANELS,GL_COMPILE);
		{
			m_GLList++;

			glDisable (GL_LINE_STIPPLE);
			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			color = pMainFrame->m_VLMColor;
			style = pMainFrame->m_VLMStyle;
			width = pMainFrame->m_VLMWidth;
			GetBWColor(color, style, width);
			DecompRGB(color,r,g,b);
			glColor3d(r,g,b);

			glLineWidth(1.0);

			//left side first;
			p=0;
			for (k=0; k<nx; k++)
			{
				glBegin(GL_QUAD_STRIP);
					N.Set(0.0, 0.0, 1.0);//top line normal is vertical

					glVertex3d(m_L[p].x, m_L[p].y, m_L[p].z);
					glVertex3d(m_L[p+nh+1].x, m_L[p+nh+1].y, m_L[p+nh+1].z);

					p++;

					for (l=1; l<=nh; l++)
					{
						glVertex3d(m_L[p].x,      m_L[p].y,      m_L[p].z);
						glVertex3d(m_L[p+nh+1].x, m_L[p+nh+1].y, m_L[p+nh+1].z);
						TB = TA;
						LB = LA;
						p++;
					}
				glEnd();
			}
			//right side next;
			p=0;
			for (k=0; k<nx; k++)
			{
				glBegin(GL_QUAD_STRIP);
					N.Set(0.0, 0.0, 1.0);//top line normal is vertical

					glVertex3d(m_L[p].x,      -m_L[p].y, m_L[p].z);
					glVertex3d(m_L[p+nh+1].x, -m_L[p+nh+1].y, m_L[p+nh+1].z);

					p++;

					for (l=1; l<=nh; l++)
					{
/*						LATB = m_L[p+nh] - m_L[p];     //					LATB = TB - LA;
						TALB = m_L[p-1]  - m_L[p+nh+1];//					TALB = LB - TA;
						N = TALB * LATB;
						N.Normalize();

						glNormal3d(N.x, -N.y, N.z);*/
						glVertex3d(m_L[p].x,      -m_L[p].y,      m_L[p].z);
						glVertex3d(m_L[p+nh+1].x, -m_L[p+nh+1].y, m_L[p+nh+1].z);
						TB = TA;
						LB = LA;
						p++;
					}
				glEnd();
			}
		}	
		glEndList();

		glNewList(BODYMESHBACK,GL_COMPILE);
		{
			m_GLList++;

			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);	
			glDisable (GL_LINE_STIPPLE);

			color = pView->m_crBackColor;
			style = pMainFrame->m_VLMStyle;
			width = pMainFrame->m_VLMWidth;
			GetBWColor(color, style, width);

			DecompRGB(color,r,g,b);
			glColor3d(r,g,b);

			glLineWidth(1.0);


			//left side first;
			p=0;
			for (k=0; k<nx; k++)
			{
				glBegin(GL_QUAD_STRIP);
					N.Set(0.0, 0.0, 1.0);//top line normal is vertical

					glVertex3d(m_L[p].x, m_L[p].y, m_L[p].z);
					glVertex3d(m_L[p+nh+1].x, m_L[p+nh+1].y, m_L[p+nh+1].z);

					p++;

					for (l=1; l<=nh; l++)
					{
/*						LATB = m_L[p+nh] - m_L[p];     //					LATB = TB - LA;
						TALB = m_L[p-1]  - m_L[p+nh+1];//					TALB = LB - TA;
						N = TALB * LATB;
						N.Normalize();

						glNormal3d(N.x, N.y, N.z);*/
						glVertex3d(m_L[p].x,      m_L[p].y,      m_L[p].z);
						glVertex3d(m_L[p+nh+1].x, m_L[p+nh+1].y, m_L[p+nh+1].z);
						TB = TA;
						LB = LA;
						p++;
					}
				glEnd();
			}
			//right side next;
			p=0;
			for (k=0; k<nx; k++)
			{
				glBegin(GL_QUAD_STRIP);
					N.Set(0.0, 0.0, 1.0);//top line normal is vertical

					glVertex3d(m_L[p].x,      -m_L[p].y, m_L[p].z);
					glVertex3d(m_L[p+nh+1].x, -m_L[p+nh+1].y, m_L[p+nh+1].z);

					p++;

					for (l=1; l<=nh; l++)
					{
/*						LATB = m_L[p+nh] - m_L[p];     //					LATB = TB - LA;
						TALB = m_L[p-1]  - m_L[p+nh+1];//					TALB = LB - TA;
						N = TALB * LATB;
						N.Normalize();

						glNormal3d(N.x, -N.y, N.z);*/
						glVertex3d(m_L[p].x,      -m_L[p].y,      m_L[p].z);
						glVertex3d(m_L[p+nh+1].x, -m_L[p+nh+1].y, m_L[p+nh+1].z);
						TB = TA;
						LB = LA;
						p++;
					}
				glEnd();
			}
		}	
		glEndList();
	}
}

void CMiarex::GLCreateBodyLines()
{
	int j,k;
	double r,g,b;
	COLORREF color;
	int style, width;
	CVector P1, P2, P3, P4, N, P1P3, P2P4, Tj, Tjp1;
	CMainFrame *pMainFrame = (CMainFrame*)m_pFrame;

	glNewList(BODYSURFACES,GL_COMPILE);
	{
		m_GLList++;

		glDisable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0,1.0);	
		glLineWidth(1.0);

		color = m_pCurBody->m_BodyColor;
		GetBWColor(color, style, width);
		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		for (k=0; k<m_pCurBody->m_NSideLines-1;k++)
		{
			for (j=0; j<m_pCurBody->m_NStations-1;j++)
			{
				Tj.Set(m_pCurBody->m_FramePosition[j].x,     0.0, 0.0);
				Tjp1.Set(m_pCurBody->m_FramePosition[j+1].x, 0.0, 0.0);

				glBegin(GL_QUADS);
				{
					P1 = m_pCurBody->m_Frame[j].m_Point[k];			P1.Translate(Tj);
					P2 = m_pCurBody->m_Frame[j+1].m_Point[k];		P2.Translate(Tjp1);
					P3 = m_pCurBody->m_Frame[j+1].m_Point[k+1];		P3.Translate(Tjp1);
					P4 = m_pCurBody->m_Frame[j].m_Point[k+1];		P4.Translate(Tj);

					P1P3 = P3-P1; 
					P2P4 = P4-P2;
					N = P1P3 * P2P4;
					N.Normalize();
					glNormal3d(N.x,  N.y,  N.z);
					glVertex3d(P1.x, P1.y, P1.z);
					glVertex3d(P2.x, P2.y, P2.z);
					glVertex3d(P3.x, P3.y, P3.z);
					glVertex3d(P4.x, P4.y, P4.z);
				}
				glEnd();
				glBegin(GL_QUADS);
				{
					//and symetric quad
					P1.y = -P1.y;
					P2.y = -P2.y;
					P3.y = -P3.y;
					P4.y = -P4.y;
					P1P3 = P3-P1; 
					P2P4 = P4-P2;
					N = P2P4 * P1P3;
					N.Normalize();
					glNormal3d(N.x,  N.y,  N.z);
					glVertex3d(P4.x, P4.y, P4.z);
					glVertex3d(P3.x, P3.y, P3.z);
					glVertex3d(P2.x, P2.y, P2.z);
					glVertex3d(P1.x, P1.y, P1.z);
				}
				glEnd();
			}
		}

		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	glEndList();

	glNewList(BODYGEOM,GL_COMPILE);
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		color = pMainFrame->m_OutlineColor;
		style = pMainFrame->m_OutlineStyle;
		width = pMainFrame->m_OutlineWidth;
		GetBWColor(color, style, width);
		glLineWidth((GLfloat)GetPenWidth(width, false));

		if(style == PS_DOT)          glLineStipple (1, 0x1111);
		else if(style == PS_DASH)    glLineStipple (1, 0x0F0F);
		else if(style == PS_DASHDOT) glLineStipple (1, 0x1C47);
		else						 glLineStipple (1, 0xFFFF);// Solid

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		for (k=0; k<m_pCurBody->m_NSideLines-1;k++)
		{
			for (j=0; j<m_pCurBody->m_NStations-1;j++)
			{
				Tj.Set(m_pCurBody->m_FramePosition[j].x,     0.0, 0.0);
				Tjp1.Set(m_pCurBody->m_FramePosition[j+1].x, 0.0, 0.0);

				glBegin(GL_QUADS);
					P1 = m_pCurBody->m_Frame[j].m_Point[k];			P1.Translate(Tj);
					P2 = m_pCurBody->m_Frame[j+1].m_Point[k];		P2.Translate(Tjp1);
					P3 = m_pCurBody->m_Frame[j+1].m_Point[k+1];		P3.Translate(Tjp1);
					P4 = m_pCurBody->m_Frame[j].m_Point[k+1];		P4.Translate(Tj);

					P1P3 = P3-P1; 
					P2P4 = P4-P2;
					N = P1P3 * P2P4;
					N.Normalize();
					glNormal3d(N.x,  N.y,  N.z);
					glVertex3d(P1.x, P1.y, P1.z);
					glVertex3d(P2.x, P2.y, P2.z);
					glVertex3d(P3.x, P3.y, P3.z);
					glVertex3d(P4.x, P4.y, P4.z);

				glEnd();
				glBegin(GL_QUADS);
					//and symetric quad
					P1.y = -P1.y;
					P2.y = -P2.y;
					P3.y = -P3.y;
					P4.y = -P4.y;
					P1P3 = P3-P1; 
					P2P4 = P4-P2;
					N = P1P3 * P2P4;
					N.Normalize();
					glNormal3d(N.x,  N.y,  N.z);
					glVertex3d(P4.x, P4.y, P4.z);
					glVertex3d(P3.x, P3.y, P3.z);
					glVertex3d(P2.x, P2.y, P2.z);
					glVertex3d(P1.x, P1.y, P1.z);

				glEnd();
			}
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}


void CMiarex::GLSetViewport()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CChildView *pView = (CChildView*)m_pChildWnd;

	int cx, cy, x, y;

	x  = 0;
	y  = 0;
	cx = m_rCltRect.Width();
	cy = m_rCltRect.Height();

	// Change OpenGL's rendering context size and viewing frustum.
	CClientDC dcClient(m_pChildWnd);
	wglMakeCurrent(dcClient.m_hDC,pView->m_hRC);

	glViewport(x,y,max(cx,cy),max(cx,cy));
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	double s = m_GLScale;
	glOrtho(-s,s,-s,s,-100.0*s,100.0*s);

//		glViewport(x,y,cx,cy);
//		double aspect;
//		aspect = (cy == 0) ? (double)cx : (double)cx/(double)cy;
//		glMatrixMode(GL_PROJECTION);
//		glLoadIdentity();
//		gluPerspective(75,0.7,-1000.0,1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.0,0.0,0.0);

	wglMakeCurrent(dcClient.m_hDC,NULL);

}

void CMiarex::GLCreateBodyAxialLines()
{
	int i,k,style, width;
	double r,g,b;
	COLORREF color;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CChildView *pChildView = (CChildView*)m_pChildWnd;

	if(!m_pCurBody) 
	{
		glNewList(BODYAXIALLINES,GL_COMPILE); glEndList();
		return;
	}

	glNewList(BODYAXIALLINES,GL_COMPILE);
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = m_pCurBody->m_BodyColor;
		style = 0;
		width = 1;
		GetBWColor(color, style, width);
		glLineWidth((GLfloat)GetPenWidth(width, false));

		if(style == PS_DOT)          glLineStipple (1, 0x1111);
		else if(style == PS_DASH)    glLineStipple (1, 0x0F0F);
		else if(style == PS_DASHDOT) glLineStipple (1, 0x1C47);
		else						 glLineStipple (1, 0xFFFF);// Solid

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		//Middle Line
		glBegin(GL_LINE_STRIP);
			for (k=0; k<m_pCurBody->m_NStations;k++)
				glVertex3d(m_pCurBody->m_FramePosition[k].x,
				           m_pCurBody->m_FramePosition[k].z,
						   m_pCurBody->m_FramePosition[k].y);
		glEnd();

		if(m_pCurBody->m_LineType==1)
		{
			//Top Line
			glBegin(GL_LINE_STRIP);
				for (k=0; k<m_pCurBody->m_NStations;k++)
					glVertex3d(m_pCurBody->m_FramePosition[k].x,
							   m_pCurBody->m_Frame[k].m_Point[0].z,
							   m_pCurBody->m_FramePosition[k].y);
			glEnd();

			//Bottom Line
			glBegin(GL_LINE_STRIP);
				for (k=0; k<m_pCurBody->m_NStations;k++)
					glVertex3d(m_pCurBody->m_FramePosition[k].x,
							   m_pCurBody->m_Frame[k].m_Point[ m_pCurBody->m_Frame[k].m_NPoints-1].z,
							   m_pCurBody->m_FramePosition[k].y);
			glEnd();
		}
		else
		{
			CVector Point;
			double xinc, u, v;
			
			int nh = 20;
			xinc = 1./(double)(nh-1);
			u=0.0; v = 0.0;

			//top line
			u=0.0; 
			glBegin(GL_LINE_STRIP);
				v = 0.0;
				for (i=0; i<=nh; i++)
				{
					m_pCurBody->GetPoint(u,v,true, Point);
					glVertex3d(Point.x, Point.z, Point.y);
					u += xinc;
				}
			glEnd();
			
			//bot line
			u=0.0; 
			glBegin(GL_LINE_STRIP);
				v = 1.0;
				for (i=0; i<=nh; i++)
				{
					m_pCurBody->GetPoint(u,v,true, Point);
					glVertex3d(Point.x, Point.z, Point.y);
					u += xinc;
				}
			glEnd();
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}


void CMiarex::GLCreateBodyPoints()
{
	int k,style, width;
	double r,g,b;
	COLORREF color;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CChildView *pChildView = (CChildView*)m_pChildWnd;

	if(!m_pCurBody) 
	{
		glNewList(BODYPOINTS,GL_COMPILE); glEndList();
		glNewList(FRAMEPOINTS,GL_COMPILE); glEndList();
		return;
	}

	glNewList(BODYPOINTS,GL_COMPILE);
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = m_pCurBody->m_BodyColor;
		style = 0;
		width = 1;
		GetBWColor(color, style, width);
		glLineWidth((GLfloat)GetPenWidth(width, false));

		glDisable(GL_LINE_STIPPLE);
		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		for (k=0; k<m_pCurBody->m_NStations;k++)
		{
			if(m_pCurBody->m_iActiveFrame==k)
			{
				glLineWidth(2.0);
				glColor3d(0.,0.,1.0);
			}
			else if(m_pCurBody->m_iHighlight==k)
			{
				glLineWidth(2.0);
				glColor3d(1.,0.,0.0);
			}
			else {
				glLineWidth(1.0);
				glColor3d(r,g,b);
			}
			glRectd(m_pCurBody->m_FramePosition[k].x-0.006/m_BodyScale*m_GLScale,
			        m_pCurBody->m_FramePosition[k].z-0.006/m_BodyScale*m_GLScale,
			        m_pCurBody->m_FramePosition[k].x+0.006/m_BodyScale*m_GLScale,
					m_pCurBody->m_FramePosition[k].z+0.006/m_BodyScale*m_GLScale);
		}

	glEndList();

	if(!m_pCurFrame) return;

	glNewList(FRAMEPOINTS,GL_COMPILE);
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = m_pCurBody->m_BodyColor;
		style = 0;
		width = 1;
		GetBWColor(color, style, width);
		glLineWidth((GLfloat)GetPenWidth(width, false));

		glDisable(GL_LINE_STIPPLE);
		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		for (k=0; k<m_pCurFrame->m_NPoints;k++)
		{
			if(m_pCurFrame->m_iSelect==k)
			{
				glLineWidth(2.0);
				glColor3d(0.0,0.0,1.0);
			}
			else if(m_pCurFrame->m_iHighlight==k)
			{
				glLineWidth(2.0);
				glColor3d(1.,0.,0.0);
			}
			else {
				glLineWidth(1.0);
				glColor3d(r,g,b);
			}
			glRectd(m_pCurFrame->m_Point[k].y-0.006/m_FrameScale*m_GLScale,
			        m_pCurFrame->m_Point[k].z-0.006/m_FrameScale*m_GLScale,
			        m_pCurFrame->m_Point[k].y+0.006/m_FrameScale*m_GLScale,
					m_pCurFrame->m_Point[k].z+0.006/m_FrameScale*m_GLScale);
		}
	}
	glEndList();

}


void CMiarex::GLCreateBodyFrame()
{
	int j,k;
	CVector Point;
	double xinc, hinc, u, v;
	int nh,style, width;
	double r,g,b;
	COLORREF color;
	CMainFrame *pMainFrame = (CMainFrame*)m_pFrame;
	CChildView *pChildView = (CChildView*)m_pChildWnd;

	if(!m_pCurBody || ! m_pCurFrame || (m_pCurBody && m_pCurBody->m_iActiveFrame<0)) 
	{
		glNewList(BODYFRAME,GL_COMPILE); glEndList();
		return;
	}

	nh = 20;
	xinc = 0.1; 
	hinc = 1.0/(double)(nh-1);

	glNewList(BODYFRAME,GL_COMPILE);//create 2D Splines or Lines
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = m_pCurBody->m_BodyColor;
		style = 0;
		width = 1;
		GetBWColor(color, style, width);
		glLineWidth((GLfloat)GetPenWidth(width, false));

		if(style == PS_DOT)          glLineStipple (1, 0x1111);
		else if(style == PS_DASH)    glLineStipple (1, 0x0F0F);
		else if(style == PS_DASHDOT) glLineStipple (1, 0x1C47);
		else						 glLineStipple (1, 0xFFFF);// Solid
 
		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		if(m_pCurBody->m_LineType ==2)
		{
			u = m_pCurBody->Getu(m_pCurBody->m_FramePosition[m_pCurBody->m_iActiveFrame].x);

			glBegin(GL_LINE_STRIP);
			{
				v = 0.0;
				for (k=0; k<nh; k++)
				{
					m_pCurBody->GetPoint(u,v,true, Point);
					glVertex3d(Point.y, Point.z, 0.0);
					v += hinc; 
				}
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			{
				v = 0.0;
				for (k=0; k<nh; k++)
				{
					m_pCurBody->GetPoint(u,v,false, Point);
					glVertex3d(Point.y, Point.z, 0.0);
					v += hinc; 
				}
			}
			glEnd();
		}
		else
		{
			glBegin(GL_LINE_STRIP);
			{
				for (k=0; k<m_pCurFrame->m_NPoints;k++)
					glVertex3d(m_pCurFrame->m_Point[k].y,
							   m_pCurFrame->m_Point[k].z,
							   0.0);
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			{
				for (k=0; k<m_pCurFrame->m_NPoints;k++)
					glVertex3d(-m_pCurFrame->m_Point[k].y,
							    m_pCurFrame->m_Point[k].z,
							    0.0);
			}
			glEnd();
		}

		glDisable(GL_LINE_STIPPLE);

		if(!m_bCurFrameOnly) 
		{
			glEnable (GL_LINE_STIPPLE);
			glLineStipple (1, 0x0F0F);
			glLineWidth(1.0);
			for(j=0; j<m_pCurBody->m_NStations; j++)
			{
				if(j!=m_pCurBody->m_iActiveFrame)
				{
					if(m_pCurBody->m_LineType ==2)
					{
						u = m_pCurBody->Getu(m_pCurBody->m_FramePosition[j].x);

						glBegin(GL_LINE_STRIP);
						{
							v = 0.0;
							for (k=0; k<nh; k++)
							{
								m_pCurBody->GetPoint(u,v,true, Point);
								glVertex3d(Point.y, Point.z, 0.0);
								v += hinc; 
							}
						}
						glEnd();
						glBegin(GL_LINE_STRIP);
						{
							v = 0.0;
							for (k=0; k<nh; k++)
							{
								m_pCurBody->GetPoint(u,v,false, Point);
								glVertex3d(Point.y, Point.z, 0.0);
								v += hinc; 
							}
						}
						glEnd();
					}
					else
					{
						glBegin(GL_LINE_STRIP);
						{
							for (k=0; k<m_pCurBody->m_NSideLines;k++)
								glVertex3d(m_pCurBody->m_Frame[j].m_Point[k].y,
											m_pCurBody->m_Frame[j].m_Point[k].z,
											0.0);
						}
						glEnd();
						glBegin(GL_LINE_STRIP);
						{
							for (k=0; k<m_pCurBody->m_NSideLines;k++)
								glVertex3d(m_pCurBody->m_Frame[j].m_Point[k].y,
											m_pCurBody->m_Frame[j].m_Point[k].z,
											0.0);
						}
						glEnd();
					}
				}
			}
			glDisable(GL_LINE_STIPPLE);
		}
	}
	glEndList();
	
	
	//create 3D Splines or Lines to overlay on the body
	glNewList(BODYFRAME3D,GL_COMPILE);
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		color = RGB(0,0,255);
		style = 0;
		width = 3;
		GetBWColor(color, style, width);
		glLineWidth((GLfloat)GetPenWidth(width, false));

		if(style == PS_DOT)          glLineStipple (1, 0x1111);
		else if(style == PS_DASH)    glLineStipple (1, 0x0F0F);
		else if(style == PS_DASHDOT) glLineStipple (1, 0x1C47);
		else						 glLineStipple (1, 0xFFFF);// Solid
 
		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		if(m_pCurBody->m_LineType ==2)
		{
			u = m_pCurBody->Getu(m_pCurBody->m_FramePosition[m_pCurBody->m_iActiveFrame].x);

			glBegin(GL_LINE_STRIP);
				v = 0.0;
				for (k=0; k<nh; k++)
				{
					m_pCurBody->GetPoint(u,v,true, Point);
					glVertex3d(Point.x, Point.y, Point.z);
					v += hinc; 
				}
			glEnd();
			glBegin(GL_LINE_STRIP);
				v = 0.0;
				for (k=0; k<nh; k++)
				{
					m_pCurBody->GetPoint(u,v,false, Point);
					glVertex3d(Point.x, Point.y, Point.z);
					v += hinc; 
				}
			glEnd();
		}
		else
		{
			glBegin(GL_LINE_STRIP);
				for (k=0; k<m_pCurFrame->m_NPoints;k++)
					glVertex3d( m_pCurBody->m_FramePosition[m_pCurBody->m_iActiveFrame].x,
								m_pCurFrame->m_Point[k].y,
								m_pCurFrame->m_Point[k].z);
			glEnd();
			glBegin(GL_LINE_STRIP);
				for (k=0; k<m_pCurFrame->m_NPoints;k++)
					glVertex3d( m_pCurBody->m_FramePosition[m_pCurBody->m_iActiveFrame].x,
							   -m_pCurFrame->m_Point[k].y,
								m_pCurFrame->m_Point[k].z);
			glEnd();
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();
}


void CMiarex::GLCreateBodyGrid()
{
	int i;
	int style, width;
	double r,g,b;
	COLORREF color;
	CMainFrame *pMainFrame = (CMainFrame*)m_pFrame;
	CChildView *pChildView = (CChildView*)m_pChildWnd;
	double h = (double)m_rCltRect.Height();
	double w = (double)m_rCltRect.Width();
	double glTop = -1.0+2.0*h/w;
	int start = 0;
	if(m_bAxes) start = 1;

	glNewList(BODYFRAMEGRID,GL_COMPILE);
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		if(m_bAxes)
		{
			// Frame axis____________
			color = pChildView->m_WndTextColor;
			width = 1;
			GetBWColor(color, style, width);
			DecompRGB(color,r,g,b);
			glColor3d(r,g,b);
			glLineWidth((GLfloat)GetPenWidth(width, false));
			glLineStipple (1, 0x1C47);

			glBegin(GL_LINES);
				//vertical line
				glVertex2d(0.0, (-1.0-m_FrameOffset.y)/m_FrameScale);
				glVertex2d(0.0,	(glTop-m_FrameOffset.y)     /m_FrameScale);
				//horizontal Line
				glVertex2d((m_VerticalSplit-m_FrameOffset.x)/m_FrameScale, 0.0);
				glVertex2d(( 1.0 -m_FrameOffset.x)          /m_FrameScale, 0.0);
			glEnd();
		}

		// Frame grid____________

		//Main Grid
		color = m_BodyGridDlg.m_Color2;
		GetBWColor(color,  m_BodyGridDlg.m_Style2, m_BodyGridDlg.m_Width2);
		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);
		glLineWidth((GLfloat)GetPenWidth(m_BodyGridDlg.m_Width2, false));

		if     (m_BodyGridDlg.m_Style2 == PS_DOT)     glLineStipple (1, 0x1111);
		else if(m_BodyGridDlg.m_Style2 == PS_DASH)    glLineStipple (1, 0x0F0F);
		else if(m_BodyGridDlg.m_Style2 == PS_DASHDOT) glLineStipple (1, 0x1C47);
		else										  glLineStipple (1, 0xFFFF);

		glBegin(GL_LINES);
		{
			if(m_BodyGridDlg.m_bGrid2)
			{
				//Main X Grid
				for(i=start; i<abs(1.0-m_FrameOffset.x)/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
				{
					glVertex2d(+(double)i*m_BodyGridDlg.m_Unit2,
						       (-1.0-m_FrameOffset.y)/m_FrameScale);
					glVertex2d(+(double)i*m_BodyGridDlg.m_Unit2,
						       (glTop-m_FrameOffset.y)     /m_FrameScale);
				}
				for(i=start; i<abs(m_FrameOffset.x-(m_VerticalSplit))/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
				{
					glVertex2d((-(double)i*m_BodyGridDlg.m_Unit2),
						       (-1.0-m_FrameOffset.y)  /m_FrameScale);
					glVertex2d((-(double)i*m_BodyGridDlg.m_Unit2),
						       (glTop-m_FrameOffset.y) /m_FrameScale);
				}
				//Main Y Grid

				for(i=start; i<abs(glTop-m_FrameOffset.y)/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
				{
					glVertex2d((m_VerticalSplit-m_FrameOffset.x)/m_FrameScale,
						       +(double)i*m_BodyGridDlg.m_Unit2);
					glVertex2d(( 1.0 -m_FrameOffset.x)          /m_FrameScale,
						       +(double)i*m_BodyGridDlg.m_Unit2);
				}

				for(i=start; i<abs(-1.0-m_FrameOffset.y)/m_BodyGridDlg.m_Unit2/m_FrameScale; i++)
				{
					glVertex2d((m_VerticalSplit-m_FrameOffset.x)/m_FrameScale,
						       -(double)i*m_BodyGridDlg.m_Unit2);
					glVertex2d(( 1.0 -m_FrameOffset.x)          /m_FrameScale,
						       -(double)i*m_BodyGridDlg.m_Unit2);
				}
			}
		}	
		glEnd();

		//Minor Grid
		color = m_BodyGridDlg.m_MinColor2;
		GetBWColor(color,  m_BodyGridDlg.m_MinStyle2, m_BodyGridDlg.m_MinWidth2);
		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);
		glLineWidth((GLfloat)GetPenWidth(m_BodyGridDlg.m_MinWidth2, false));

		if     (m_BodyGridDlg.m_MinStyle2 == PS_DOT)     glLineStipple (1, 0x1111);
		else if(m_BodyGridDlg.m_MinStyle2 == PS_DASH)    glLineStipple (1, 0x0F0F);
		else if(m_BodyGridDlg.m_MinStyle2 == PS_DASHDOT) glLineStipple (1, 0x1C47);
		else											 glLineStipple (1, 0xFFFF);

		glBegin(GL_LINES);
		{
			if(m_BodyGridDlg.m_bMinGrid2)
			{
				//Minor X Grid
				for(i=start; i<abs(1.0-m_FrameOffset.x)/m_BodyGridDlg.m_MinorUnit2/m_FrameScale; i++)
				{
					glVertex2d(+(double)i*m_BodyGridDlg.m_MinorUnit2,
						       (-1.0-m_FrameOffset.y)/m_FrameScale);
					glVertex2d(+(double)i*m_BodyGridDlg.m_MinorUnit2,
						       (glTop-m_FrameOffset.y)     /m_FrameScale);
				}
				for(i=start; i<abs(m_FrameOffset.x-(m_VerticalSplit))/m_BodyGridDlg.m_MinorUnit2/m_FrameScale; i++)
				{
					glVertex2d((-(double)i*m_BodyGridDlg.m_MinorUnit2),
						       (-1.0-m_FrameOffset.y)  /m_FrameScale);
					glVertex2d((-(double)i*m_BodyGridDlg.m_MinorUnit2),
						       (glTop-m_FrameOffset.y) /m_FrameScale);
				}
				//Minor Y Grid

				for(i=start; i<abs(glTop-m_FrameOffset.y)/m_BodyGridDlg.m_MinorUnit2/m_FrameScale; i++)
				{
					glVertex2d((m_VerticalSplit-m_FrameOffset.x)/m_FrameScale,
						       +(double)i*m_BodyGridDlg.m_MinorUnit2);
					glVertex2d(( 1.0 -m_FrameOffset.x)          /m_FrameScale,
						       +(double)i*m_BodyGridDlg.m_MinorUnit2);
				}

				for(i=start; i<abs(-1.0-m_FrameOffset.y)/m_BodyGridDlg.m_MinorUnit2/m_FrameScale; i++)
				{
					glVertex2d((m_VerticalSplit-m_FrameOffset.x)/m_FrameScale,
						       -(double)i*m_BodyGridDlg.m_MinorUnit2);
					glVertex2d(( 1.0 -m_FrameOffset.x)          /m_FrameScale,
						       -(double)i*m_BodyGridDlg.m_MinorUnit2);
				}
			}
		}	
		glEnd();

		glDisable(GL_LINE_STIPPLE);
	}
	glEndList();

	glNewList(BODYLINEGRID,GL_COMPILE);
	{
		m_GLList++;

		glEnable(GL_DEPTH_TEST);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT,GL_LINE);

		if(m_bAxes)
		{
			color = pChildView->m_WndTextColor;
			width = 1;
			GetBWColor(color, style, width);
			glLineWidth((GLfloat)GetPenWidth(width, false));

			glLineStipple (1, 0x1C47);

			DecompRGB(color,r,g,b);
			glColor3d(r,g,b);

			// BodyLine axis____________

			glBegin(GL_LINES);
				//horizontal Line
				glVertex2d((m_VerticalSplit-m_BodyOffset.x)/m_BodyScale, 0.0);
				glVertex2d((-1.0 -m_BodyOffset.x)          /m_BodyScale, 0.0);
				//vertical Line
				glVertex2d(0.0,	(m_HorizontalSplit-m_BodyOffset.y)/m_BodyScale);
				glVertex2d(0.0,	(-1.0+2.0*h/w-m_BodyOffset.y)     /m_BodyScale);
			glEnd();
		}

		// BodyLine grid____________

		//Main Grid
		color = m_BodyGridDlg.m_Color;
		GetBWColor(color,  m_BodyGridDlg.m_Style, m_BodyGridDlg.m_Width);
		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);
		glLineWidth((GLfloat)GetPenWidth(m_BodyGridDlg.m_Width, false));

		if     (m_BodyGridDlg.m_Style == PS_DOT)     glLineStipple (1, 0x1111);
		else if(m_BodyGridDlg.m_Style == PS_DASH)    glLineStipple (1, 0x0F0F);
		else if(m_BodyGridDlg.m_Style == PS_DASHDOT) glLineStipple (1, 0x1C47);
		else									     glLineStipple (1, 0xFFFF);

		glBegin(GL_LINES);

			if(m_BodyGridDlg.m_bGrid)
			{
				//Main X Grid
				for(i=start; i<abs(m_VerticalSplit-m_BodyOffset.x)/m_BodyGridDlg.m_Unit/m_BodyScale; i++)
				{
					glVertex2d(+(double)i*m_BodyGridDlg.m_Unit,
						       (m_HorizontalSplit-m_BodyOffset.y)/m_BodyScale);
					glVertex2d(+(double)i*m_BodyGridDlg.m_Unit,
						       (glTop-m_BodyOffset.y)            /m_BodyScale);
				}
				for(i=start; i<abs(m_BodyOffset.x-(-1.0))/m_BodyGridDlg.m_Unit/m_BodyScale; i++)
				{
					glVertex2d((-(double)i*m_BodyGridDlg.m_Unit),
						       (m_HorizontalSplit-m_BodyOffset.y)    /m_BodyScale);
					glVertex2d((-(double)i*m_BodyGridDlg.m_Unit),
						       (glTop-m_BodyOffset.y)                /m_BodyScale);
				}

				//Main Y Grid

				for(i=start; i<abs(glTop-m_BodyOffset.y)/m_BodyGridDlg.m_Unit/m_BodyScale; i++)
				{
					glVertex2d((m_VerticalSplit-m_BodyOffset.x)/m_BodyScale,
						       +(double)i*m_BodyGridDlg.m_Unit);
					glVertex2d((-1.0 -m_BodyOffset.x)          /m_BodyScale,
						       +(double)i*m_BodyGridDlg.m_Unit);
				}

				for(i=start; i<abs(m_HorizontalSplit-m_BodyOffset.y)/m_BodyGridDlg.m_Unit/m_BodyScale; i++)
				{
					glVertex2d((m_VerticalSplit-m_BodyOffset.x)/m_BodyScale,
						       -(double)i*m_BodyGridDlg.m_Unit);
					glVertex2d((-1.0 -m_BodyOffset.x)          /m_BodyScale,
						       -(double)i*m_BodyGridDlg.m_Unit);
				}
			}
			
		glEnd();

		//Minor Grid
		color = m_BodyGridDlg.m_MinColor;
		GetBWColor(color,  m_BodyGridDlg.m_MinStyle, m_BodyGridDlg.m_MinWidth);
		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);
		glLineWidth((GLfloat)GetPenWidth(m_BodyGridDlg.m_MinWidth, false));

		if     (m_BodyGridDlg.m_MinStyle == PS_DOT)     glLineStipple (1, 0x1111);
		else if(m_BodyGridDlg.m_MinStyle == PS_DASH)    glLineStipple (1, 0x0F0F);
		else if(m_BodyGridDlg.m_MinStyle == PS_DASHDOT) glLineStipple (1, 0x1C47);
		else									        glLineStipple (1, 0xFFFF);

		glBegin(GL_LINES);

			if(m_BodyGridDlg.m_bMinGrid)
			{
				//Minor X Grid
				for(i=start; i<abs(m_VerticalSplit-m_BodyOffset.x)/m_BodyGridDlg.m_MinorUnit/m_BodyScale; i++)
				{
					glVertex2d(+(double)i*m_BodyGridDlg.m_MinorUnit,
						       (m_HorizontalSplit-m_BodyOffset.y)/m_BodyScale);
					glVertex2d(+(double)i*m_BodyGridDlg.m_MinorUnit,
						       (glTop-m_BodyOffset.y)            /m_BodyScale);
				}
				for(i=start; i<abs(m_BodyOffset.x-(-1.0))/m_BodyGridDlg.m_MinorUnit/m_BodyScale; i++)
				{
					glVertex2d((-(double)i*m_BodyGridDlg.m_MinorUnit),
						       (m_HorizontalSplit-m_BodyOffset.y)    /m_BodyScale);
					glVertex2d((-(double)i*m_BodyGridDlg.m_MinorUnit),
						       (glTop-m_BodyOffset.y)                /m_BodyScale);
				}

				//Minor Y Grid

				for(i=start; i<abs(glTop-m_BodyOffset.y)/m_BodyGridDlg.m_MinorUnit/m_BodyScale; i++)
				{
					glVertex2d((m_VerticalSplit-m_BodyOffset.x)/m_BodyScale,
						       +(double)i*m_BodyGridDlg.m_MinorUnit);
					glVertex2d((-1.0 -m_BodyOffset.x)          /m_BodyScale,
						       +(double)i*m_BodyGridDlg.m_MinorUnit);
				}

				for(i=start; i<abs(m_HorizontalSplit-m_BodyOffset.y)/m_BodyGridDlg.m_MinorUnit/m_BodyScale; i++)
				{
					glVertex2d((m_VerticalSplit-m_BodyOffset.x)/m_BodyScale,
						       -(double)i*m_BodyGridDlg.m_MinorUnit);
					glVertex2d((-1.0 -m_BodyOffset.x)          /m_BodyScale,
						       -(double)i*m_BodyGridDlg.m_MinorUnit);
				}
			}
			
		glEnd();
		glDisable(GL_LINE_STIPPLE);
	}
	glEndList();
}


void CMiarex::SetBodyScale()
{
	int k;
	double length, height;
	double h = (double)m_rCltRect.Height();
	double w = (double)m_rCltRect.Width();
	double glTop = -1.0+2.0*h/w;

	if(m_bIs3DScaleSet && !m_bAutoScales) return;
	
	m_UFOOffset.x = (GLfloat)(-1.0+m_VerticalSplit)*m_GLScale/2.0;
	m_UFOOffset.y = (GLfloat)(-.6)*m_GLScale;

	m_VerticalSplit     =  0.35;
	m_HorizontalSplit   = -0.45;

	CPoint P1, P2;
	CVector V1, V2;
	V1.Set(-1.0*m_GLScale,0.0,0.0);
	V2.Set(m_VerticalSplit*m_GLScale, m_HorizontalSplit*m_GLScale,0.0);
	GLToClient(V1, P1);
	GLToClient(V2, P2);
	m_BodyLineRect.SetRect(P1, P2);

	V1.Set(m_VerticalSplit * m_GLScale,  0.0,             0.0);
	V2.Set(1.0             * m_GLScale, -1.0 * m_GLScale, 0.0);
	GLToClient(V1, P1);
	GLToClient(V2, P2);
	m_FrameRect.SetRect(P1, P2);

	V1.Set(-1.0*m_GLScale,             m_HorizontalSplit*m_GLScale, 0.0);
	V2.Set(m_VerticalSplit*m_GLScale, -1.0              *m_GLScale, 0.0);
	GLToClient(V1, P1);
	GLToClient(V2, P2);
	m_BodyRect.SetRect(P1, P2);


	m_FrameOffset.Set((1.0+m_VerticalSplit)/2.0 * m_GLScale,  (glTop-1.0)              /2.0 * m_GLScale,   0.0);
	m_BodyOffset.Set(  -0.7                     * m_GLScale,  (glTop+m_HorizontalSplit)/2.0 * m_GLScale,   0.0);

	if(m_pCurBody)
	{
		length =   m_pCurBody->m_FramePosition[m_pCurBody->m_NStations-1].x  - m_pCurBody->m_FramePosition[0].x;
		m_BodyScale = (m_VerticalSplit-(-1.0) - 0.3)/length*m_GLScale;

		height = 0.0;
		for(k=0; k<m_pCurBody->m_NStations; k++)
		{
			height = max(height,abs( m_pCurBody->m_Frame[k].m_Point[0].z - m_pCurBody->m_Frame[k].m_Point[m_pCurBody->m_Frame[k].m_NPoints-1].z)); 
		}
		m_FrameScale = (1.0-0.5)/height*m_GLScale;
	}
	else
	{
		m_BodyScale  = 1.0;
		m_FrameScale = 1.0;
	}

	CFrame::s_rViewRect.CopyRect(m_FrameRect);
	CBody::s_rViewRect.CopyRect(m_BodyLineRect);
	m_BodyRefScale  = m_BodyScale;
	m_FrameRefScale = m_FrameScale;
}


void CMiarex::GLCreateGeom(CWing *pWing, UINT List, COLORREF wingcolor)
{
	int j,l;
	int style, width;
	double r,g,b;
	CVector Pt, A, B, C, D, N, BD, AC, LATB, TALB;
	COLORREF color;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;	
	
	N.Set(0.0, 0.0, 0.0);
	glNewList(List,GL_COMPILE);
	{
		m_GLList++;
		glLineWidth(1.0);

		color = wingcolor;
		style = 0;
		width = 0;
		GetBWColor(color, style, width);
		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		glPolygonMode(GL_FRONT, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);	
		glEnable(GL_DEPTH_TEST);

		//top surface
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
			{
				glBegin(GL_QUADS);
				{
					pWing->m_Surface[j].GetPanel(0,l, 1);

					LATB = pWing->m_Surface[j].TB - pWing->m_Surface[j].LA;
					TALB = pWing->m_Surface[j].LB - pWing->m_Surface[j].TA;

					N = LATB *TALB;
					N. Normalize();

					glNormal3d(N.x, N.y, N.z);
					glVertex3d(pWing->m_Surface[j].LA.x, 
							   pWing->m_Surface[j].LA.y, 
							   pWing->m_Surface[j].LA.z);
					glVertex3d(pWing->m_Surface[j].TA.x, 
							   pWing->m_Surface[j].TA.y,
							   pWing->m_Surface[j].TA.z);	

					pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1,l, 1);
					glVertex3d(pWing->m_Surface[j].TB.x,
							   pWing->m_Surface[j].TB.y,
							   pWing->m_Surface[j].TB.z);
					glVertex3d(pWing->m_Surface[j].LB.x,
							   pWing->m_Surface[j].LB.y,
							   pWing->m_Surface[j].LB.z);
				}
				glEnd();
			}
		}

		//bottom surface
		for (j=0; j<pWing->m_NSurfaces; j++)
		{			
			for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
			{
				glBegin(GL_QUADS);
				{
					pWing->m_Surface[j].GetPanel(0,l, -1);

					LATB = pWing->m_Surface[j].TB - pWing->m_Surface[j].LA;
					TALB = pWing->m_Surface[j].LB - pWing->m_Surface[j].TA;

					N = TALB * LATB;
					N. Normalize();

					glNormal3d( N.x,  N.y,  N.z);
					glVertex3d( pWing->m_Surface[j].TA.x, 
								pWing->m_Surface[j].TA.y,
								pWing->m_Surface[j].TA.z);	
					glVertex3d( pWing->m_Surface[j].LA.x, 
								pWing->m_Surface[j].LA.y, 
								pWing->m_Surface[j].LA.z);

					pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1,l, -1);
					glVertex3d( pWing->m_Surface[j].LB.x,
								pWing->m_Surface[j].LB.y,
								pWing->m_Surface[j].LB.z);
					glVertex3d( pWing->m_Surface[j].TB.x,
								pWing->m_Surface[j].TB.y,
								pWing->m_Surface[j].TB.z);
				}
				glEnd();
			}
		}

		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			//All surfaces
			//Left surface
			if(pWing->m_Surface[j].m_bIsTipLeft)
			{
				glBegin(GL_QUAD_STRIP);		
				{
					pWing->m_Surface[j].GetPanel(0, 0, -1);
					C. Copy(pWing->m_Surface[0].LA);
					D. Copy(pWing->m_Surface[0].TA);
					pWing->m_Surface[j].GetPanel(0, 0, 1);
					A. Copy(pWing->m_Surface[0].TA);
					B. Copy(pWing->m_Surface[0].LA);

					BD = D-B;
					AC = C-A;
					N  = AC*BD;
					N.Normalize();
					glNormal3d( N.x, N.y, N.z);

					glVertex3d(A.x, A.y, A.z);
					glVertex3d(D.x, D.y, D.z);

					for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
					{
						pWing->m_Surface[j].GetPanel(0,l,-1);
						glVertex3d( pWing->m_Surface[0].LA.x,
									pWing->m_Surface[0].LA.y,
									pWing->m_Surface[0].LA.z);
						pWing->m_Surface[j].GetPanel(0,l,1);
						glVertex3d( pWing->m_Surface[0].LA.x,
									pWing->m_Surface[0].LA.y,
									pWing->m_Surface[0].LA.z);
					}
				}
				glEnd();
			}
			if(pWing->m_Surface[j].m_bIsTipRight)
			{
				//right surface
				glBegin(GL_QUAD_STRIP);
				{
					pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1,0, 1);
					A. Copy(pWing->m_Surface[0].TB);
					B. Copy(pWing->m_Surface[0].LB);
					pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1,0, -1);
					C. Copy(pWing->m_Surface[0].LB);
					D. Copy(pWing->m_Surface[0].TB);

					BD = D-B;
					AC = C-A;
					N  = BD * AC;
					N.Normalize();
					glNormal3d( N.x,  N.y,  N.z);

					glVertex3d(A.x, A.y, A.z);
					glVertex3d(D.x, D.y, D.z);
				
					for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
					{
						pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1, l, 1);
						glVertex3d(pWing->m_Surface[j].LB.x,
								   pWing->m_Surface[j].LB.y,
								   pWing->m_Surface[j].LB.z);
						pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1, l, -1);
						glVertex3d(pWing->m_Surface[j].LB.x,
								   pWing->m_Surface[j].LB.y,
								   pWing->m_Surface[j].LB.z);
					}
				}
				glEnd();
			}
		}
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();

	//OUTLINE
	glNewList(List+1,GL_COMPILE);
	{
		m_GLList++;

		glPolygonMode(GL_FRONT,GL_LINE);
		glEnable (GL_LINE_STIPPLE);
		glLineWidth((GLfloat)GetPenWidth(pFrame->m_OutlineWidth, false));

		color = pFrame->m_OutlineColor;
		style = pFrame->m_OutlineStyle;
		width = pFrame->m_OutlineWidth;
		GetBWColor(color, style, width);

		if     (style == PS_DOT) 		glLineStipple (1, 0x1111);
		else if(style == PS_DASH) 		glLineStipple (1, 0x0F0F);
		else if(style == PS_DASHDOT) 	glLineStipple (1, 0x1C47);
		else							glLineStipple (1, 0xFFFF);

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);
		glLineWidth((GLfloat)GetPenWidth(width, false));

		//TOP outline
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			glBegin(GL_LINE_STRIP);
			{
				for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
				{
					pWing->m_Surface[j].GetPanel(0, l, 1);
					glVertex3d(pWing->m_Surface[j].TA.x,
							   pWing->m_Surface[j].TA.y,
							   pWing->m_Surface[j].TA.z);
				}
		
				glVertex3d(pWing->m_Surface[j].LA.x,
						   pWing->m_Surface[j].LA.y,
						   pWing->m_Surface[j].LA.z);
			}
			glEnd();

			glBegin(GL_LINE_STRIP);
			{
				for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
				{
					pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1, l, 1);
					glVertex3d(pWing->m_Surface[j].TB.x,
							   pWing->m_Surface[j].TB.y,
							   pWing->m_Surface[j].TB.z);
				}

				glVertex3d(pWing->m_Surface[j].LB.x,
						   pWing->m_Surface[j].LB.y,
						   pWing->m_Surface[j].LB.z);
			}
			glEnd();
		}

		//BOTTOM outline
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			glBegin(GL_LINE_STRIP);
			{
				for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
				{
					pWing->m_Surface[j].GetPanel(0, l, -1);
					glVertex3d(pWing->m_Surface[j].TA.x,
							   pWing->m_Surface[j].TA.y,
							   pWing->m_Surface[j].TA.z);
				}
				glVertex3d(pWing->m_Surface[j].LA.x,
						   pWing->m_Surface[j].LA.y,
						   pWing->m_Surface[j].LA.z);
			}
			glEnd();
			glBegin(GL_LINE_STRIP);
			{
				for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++)
				{
					pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1, l, -1);
					glVertex3d(pWing->m_Surface[j].TB.x,
							   pWing->m_Surface[j].TB.y,
							   pWing->m_Surface[j].TB.z);
				}

				glVertex3d(pWing->m_Surface[j].LB.x,
						   pWing->m_Surface[j].LB.y,
						   pWing->m_Surface[j].LB.z);
			}
			glEnd();
		}
		//WingContour
		//Leading edge outline
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			glBegin(GL_LINES);
			{
				pWing->m_Surface[j].GetPanel(0,pWing->m_Surface[j].m_NXPanels-1, 0);
				glVertex3d(pWing->m_Surface[j].LA.x,
						   pWing->m_Surface[j].LA.y,
						   pWing->m_Surface[j].LA.z);
				pWing->m_Surface[j].GetPanel( pWing->m_Surface[j].m_NYPanels-1,pWing->m_Surface[j].m_NXPanels-1, 0);
				glVertex3d(pWing->m_Surface[j].LB.x,
						   pWing->m_Surface[j].LB.y,
						   pWing->m_Surface[j].LB.z);
			}
			glEnd();
		}
		//Trailing edge outline
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			glBegin(GL_LINES);
			{
				pWing->m_Surface[j].GetPanel(0,0, 0);
				glVertex3d(pWing->m_Surface[j].TA.x,
						   pWing->m_Surface[j].TA.y,
						   pWing->m_Surface[j].TA.z);
				pWing->m_Surface[j].GetPanel( pWing->m_Surface[j].m_NYPanels-1, 0, 0);
				glVertex3d(pWing->m_Surface[j].TB.x,
						   pWing->m_Surface[j].TB.y,
						   pWing->m_Surface[j].TB.z);
			}
			glEnd();
		}
		//flap outline....
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			if(pWing->m_Surface[j].m_pFoilA->m_bTEFlap && pWing->m_Surface[j].m_pFoilB->m_bTEFlap) 
			{
				glBegin(GL_LINES);
				{
					if(pWing->m_Surface[j].m_pFoilA->m_bTEFlap)
						pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0, 
													 pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0, 
													 0.0, Pt, 1);
					else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, 1);	
					glVertex3d(Pt.x, Pt.y, Pt.z);

					if(pWing->m_Surface[j].m_pFoilB->m_bTEFlap)
						pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0, 
													 pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0, 
													 1.0, Pt, 1);
					else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, 1);	
					glVertex3d(Pt.x, Pt.y, Pt.z);
				}
				glEnd();
				glBegin(GL_LINES);
				{
					if(pWing->m_Surface[j].m_pFoilA->m_bTEFlap)
						pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0, 
													 pWing->m_Surface[j].m_pFoilA->m_TEXHinge/100.0, 
													 0.0, Pt, -1);
					else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, -1);	
					glVertex3d(Pt.x, Pt.y, Pt.z);

					if(pWing->m_Surface[j].m_pFoilB->m_bTEFlap)
						pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0, 
													 pWing->m_Surface[j].m_pFoilB->m_TEXHinge/100.0, 
													 1.0, Pt, -1);
					else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, -1);	
					glVertex3d(Pt.x, Pt.y, Pt.z);
				}
				glEnd();
			}
		}
		for (j=0; j<pWing->m_NSurfaces; j++)
		{
			if(pWing->m_Surface[j].m_pFoilA->m_bLEFlap && pWing->m_Surface[j].m_pFoilB->m_bLEFlap) 
			{
				glBegin(GL_LINES);
				{
					if(pWing->m_Surface[j].m_pFoilA->m_bLEFlap)
						pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0, 
													 pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0, 
													 0.0, Pt, 1);
					else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, 1);	
					glVertex3d(Pt.x, Pt.y, Pt.z);

					if(pWing->m_Surface[j].m_pFoilB->m_bLEFlap)
						pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0, 
													 pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0, 
													 1.0, Pt, 1);
					else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, 1);	
					glVertex3d(Pt.x, Pt.y, Pt.z);
				}
				glEnd();
				glBegin(GL_LINES);
				{
					if(pWing->m_Surface[j].m_pFoilA->m_bLEFlap)
						pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0, 
													 pWing->m_Surface[j].m_pFoilA->m_LEXHinge/100.0, 
													 0.0, Pt, -1);
					else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 0.0, Pt, -1);	
					glVertex3d(Pt.x, Pt.y, Pt.z);

					if(pWing->m_Surface[j].m_pFoilB->m_bLEFlap)
						pWing->m_Surface[j].GetPoint(pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0, 
													 pWing->m_Surface[j].m_pFoilB->m_LEXHinge/100.0, 
													 1.0, Pt, -1);
					else 	pWing->m_Surface[j].GetPoint(1.0, 1.0, 1.0, Pt, -1);	
					glVertex3d(Pt.x, Pt.y, Pt.z);
				}
				glEnd();
			}
		}
		glDisable (GL_LINE_STIPPLE);
	}
	glEndList();

}

void CMiarex::GLCreateMesh()
{
	double r,g,b;
	COLORREF color;
	int iLA, iLB, iTA, iTB;
	int style, width, p;
	
	CVector A, B, N;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;	
	CChildView *pView = (CChildView*)m_pChildWnd;
	CW3DBar* pW3DBar = (CW3DBar*)m_pW3DBar;

	N.Set(0.0, 0.0, 0.0);

	glNewList(MESHPANELS,GL_COMPILE);
	{
		m_GLList++;
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//		glEnable(GL_POLYGON_OFFSET_FILL);
//		glPolygonOffset(1.0, 1.0);	

		color = pFrame->m_VLMColor;
		style = pFrame->m_VLMStyle;
		width = pFrame->m_VLMWidth;
		GetBWColor(color, style, width);

		glLineWidth(1.0);

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);
		for (p=0; p<m_MatSize; p++)
		{
				glBegin(GL_QUADS);
				{
					iLA = m_Panel[p].m_iLA;
					iLB = m_Panel[p].m_iLB;
					iTA = m_Panel[p].m_iTA;
					iTB = m_Panel[p].m_iTB;

					glNormal3d(m_Panel[p].Normal.x, m_Panel[p].Normal.y, m_Panel[p].Normal.z);
					glVertex3d(m_Node[iLA].x, m_Node[iLA].y, m_Node[iLA].z);
					glVertex3d(m_Node[iTA].x, m_Node[iTA].y, m_Node[iTA].z);
					glVertex3d(m_Node[iTB].x, m_Node[iTB].y, m_Node[iTB].z);
					glVertex3d(m_Node[iLB].x, m_Node[iLB].y, m_Node[iLB].z);
				}
				glEnd();
		}
	}
	glEndList();

	glNewList(MESHBACK,GL_COMPILE);
	{
		m_GLList++;
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);	

		color = pView->m_crBackColor;
		style = pFrame->m_VLMStyle;
		width = pFrame->m_VLMWidth;
		GetBWColor(color, style, width);

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		glLineWidth((GLfloat)GetPenWidth(width, false));
		glDisable (GL_LINE_STIPPLE);
		
		for (p=0; p<m_MatSize; p++)
		{
				glBegin(GL_QUADS);
				{
					iLA = m_Panel[p].m_iLA;
					iLB = m_Panel[p].m_iLB;
					iTA = m_Panel[p].m_iTA;
					iTB = m_Panel[p].m_iTB;

					glVertex3d(m_Node[iLA].x, m_Node[iLA].y, m_Node[iLA].z);
					glVertex3d(m_Node[iTA].x, m_Node[iTA].y, m_Node[iTA].z);
					glVertex3d(m_Node[iTB].x, m_Node[iTB].y, m_Node[iTB].z);
					glVertex3d(m_Node[iLB].x, m_Node[iLB].y, m_Node[iLB].z);
					glNormal3d(m_Panel[p].Normal.x, m_Panel[p].Normal.y, m_Panel[p].Normal.z);
				}
				glEnd();
		}
		
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	glEndList();
}

void CMiarex::GLCreateWakePanels(int LIST)
{
	if(!m_pCurWPolar || m_pCurWPolar->m_AnalysisType<=1) return;
	if(m_nWakeNodes==0 || m_WakeSize==0) return;
	int pw;
	CVector APt, BPt, TransA, TransB;
	int iWLA, iWLB, iWTA, iWTB;
	double r,g,b;

	CVector *pWNode;
	pWNode = m_WakeNode;
	if(m_pCurWOpp && m_pCurWPolar->m_bWakeRollUp) pWNode = m_TempWakeNode;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	glNewList(LIST,GL_COMPILE);
	{
		m_GLList++;

		glLineWidth((GLfloat)GetPenWidth(1,false));
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		DecompRGB(pFrame->m_WakeColor,r,g,b);
		glColor3d(r,g,b);
		glEnable (GL_LINE_STIPPLE);
		glLineWidth((GLfloat)GetPenWidth(pFrame->m_WakeWidth, false));

		if(pFrame->m_WakeStyle == PS_DOT)			glLineStipple (1, 0x1111);
		else if(pFrame->m_WakeStyle == PS_DASH)		glLineStipple (1, 0x0F0F);
		else if(pFrame->m_WakeStyle == PS_DASHDOT) 	glLineStipple (1, 0x1C47);
		else										glLineStipple (1, 0xFFFF);

		for (pw=0; pw<m_WakeSize; pw++)
		{
			iWLA = m_WakePanel[pw].m_iLA;
			iWLB = m_WakePanel[pw].m_iLB;
			APt = pWNode[iWLA];
			BPt = pWNode[iWLB];
			TransA = APt;
			TransB = BPt;
			if(m_pCurWPolar->m_bTiltedGeom && !m_pCurWPolar->m_bWakeRollUp && m_pCurWOpp )
			{
				APt.RotateY(m_pCurWOpp->m_Alpha);
				BPt.RotateY(m_pCurWOpp->m_Alpha);
			}
			TransA = APt - TransA;
			TransB = BPt - TransB;

			iWLA = m_WakePanel[pw].m_iLA;
			iWTA = m_WakePanel[pw].m_iTA;
			iWLB = m_WakePanel[pw].m_iLB;
			iWTB = m_WakePanel[pw].m_iTB;

			glBegin(GL_QUADS);
			{
				glVertex3d(pWNode[iWLA].x + TransA.x,
						   pWNode[iWLA].y + TransA.y, 
						   pWNode[iWLA].z + TransA.z);
				glVertex3d(pWNode[iWLB].x + TransB.x,
						   pWNode[iWLB].y + TransB.y, 
						   pWNode[iWLB].z + TransB.z);
				glVertex3d(pWNode[iWTB].x + TransB.x,
						   pWNode[iWTB].y + TransB.y, 
						   pWNode[iWTB].z + TransB.z);
				glVertex3d(pWNode[iWTA].x + TransA.x,
						   pWNode[iWTA].y + TransA.y, 
						   pWNode[iWTA].z + TransA.z);
			}
			glEnd();
		}	
	}
	glEndList();
}


void CMiarex::GLDrawAxes()
{
	double r,g,b;

	double l = .8;
//	if(m_pCurWing) l=1.1*m_pCurWing->m_Span/2.0;
	CChildView * pChildView = (CChildView*)m_pChildWnd;

	glPolygonMode(GL_FRONT,GL_LINE);
	glLineWidth(1.0);
	DecompRGB(pChildView->m_WndTextColor,r,g,b);
	glColor3d(r,g,b);

// X axis____________
	glEnable (GL_LINE_STIPPLE);
	glLineStipple (1, 0x1C47);// Dash-Dot

//	glBegin(GL_LINE_STRIP);
//		for(i=-9; i<=10; i++){
//			glVertex3d(0.1*(double)i*l, 0.0, 0.0);
//		}
//	glEnd();
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
	glBegin(GL_LINES); 
		glVertex3d(1.0*l, -0.050*l, -0.020*l);
		glVertex3d(1.0*l, -0.020*l, -0.050*l);
	glEnd();
	glBegin(GL_LINES); 
		glVertex3d(1.0*l, -0.020*l, -0.020*l);
		glVertex3d(1.0*l, -0.050*l, -0.050*l);
	glEnd();

// Y axis____________
	glEnable (GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex3d(0.0, -.8, 0.0);
		glVertex3d(0.0,  .8, 0.0);
	glEnd();
//	glBegin(GL_LINE_STRIP);
//		for(i=-9; i<=10; i++){
//			glVertex3d(0.0, 0.1*(double)i*l, 0.0);
//		}
//	glEnd();
	
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
	glBegin(GL_LINES); 
		glVertex3d(-0.020*l, 1.0*l, -0.020*l);
		glVertex3d(-0.050*l, 1.0*l, -0.050*l);
	glEnd();
	glBegin(GL_LINES); 
		glVertex3d(-0.050*l, 1.0*l,-0.020*l);
		glVertex3d(-0.035*l, 1.0*l,-0.035*l);
	glEnd();

// Z axis____________
	glEnable (GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex3d(0.0, 0.0, -.8);
		glVertex3d(0.0, 0.0,  .8);
	glEnd();

//	glBegin(GL_LINE_STRIP);
//		for(i=-9; i<=10; i++){
//			glVertex3d(0.0, 0.0, 0.1*(double)i*l);
//		}
//	glEnd();

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
	glBegin(GL_LINE_STRIP); 
		glVertex3d(-0.050*l, -0.050*l,1.0*l);
		glVertex3d(-0.050*l, -0.020*l,1.0*l);

		glVertex3d(-0.020*l, -0.050*l,1.0*l);
		glVertex3d(-0.020*l, -0.020*l,1.0*l);
	glEnd();
	glDisable (GL_LINE_STIPPLE);
}



void CMiarex::OnWPolarExport()
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->ExportWPlr(m_pCurWPolar);
}



void CMiarex::OnCpView() 
{
//	if(!m_pCurWing || !m_pCurWOpp || !m_pCurWPolar || m_pCurWPolar->m_AnalysisType != 3 ) return;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	CToolBarCtrl *pTB = &(pFrame->m_MiarexBar.GetToolBarCtrl());
	pTB->PressButton(IDT_WOPP,    false);
	pTB->PressButton(IDT_WPOLARS, false);
	pTB->PressButton(IDM_3DVIEW,  false);
	pTB->PressButton(IDM_CPVIEW, true);
	pTB->PressButton(IDM_BODYDESIGN, false);
	m_pChildWnd->SetFocus();

	m_iView=4;

	if(m_iView==5)
	{
		SetUFO();//just in case the body has been modified
		m_bResetglMesh = true;
	}

	pFrame->DockMiarexBars();
	m_pCurGraph = &m_CpGraph;
	pFrame->m_PolarDlgBar.SetParams();
	m_FlowLinesDlg.ShowWindow(SW_HIDE);
	CheckMenus();

	CreateCpCurves();
	UpdateView();
}


void CMiarex::OnWOpp() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CToolBarCtrl *pTB = &(pFrame->m_MiarexBar.GetToolBarCtrl());
	pTB->PressButton(IDT_WOPP,    true);
	pTB->PressButton(IDT_WPOLARS, false);
	pTB->PressButton(IDM_3DVIEW,  false);
	pTB->PressButton(IDM_CPVIEW, false);
	pTB->PressButton(IDM_BODYDESIGN, false);
	m_pChildWnd->SetFocus();

	if(m_iView==1) 
	{
		UpdateView();
		return;
	}

	if(m_iView==5)
	{
		SetUFO();//just in case the body has been modified
		m_bResetglMesh = true;
	}

	m_pCurGraph = m_pCurWingGraph;
	m_iView=1;
	pFrame->DockMiarexBars();
	pFrame->m_PolarDlgBar.SetParams();
	m_FlowLinesDlg.ShowWindow(SW_HIDE);
	m_bIs2DScaleSet = false;
	Set2DScale();
	CreateWOppCurves();
	CheckMenus();

	UpdateView();
}

void CMiarex::OnWingGraph1() 
{
	m_iWingView = 1;
	m_pCurGraph = &m_WingGraph1;
	m_pCurWingGraph = m_pCurGraph;
	if(m_iView!=1) OnWOpp();
	else {
		CreateWOppCurves();
		CheckMenus();
		UpdateView();	
	}
}

void CMiarex::OnWingGraph2() 
{	
	m_iWingView = 1;
	m_pCurGraph = &m_WingGraph2;
	m_pCurWingGraph = m_pCurGraph;
	if(m_iView!=1) OnWOpp();
	else {
		CreateWOppCurves();
		CheckMenus();
		UpdateView();	
	}
}

void CMiarex::OnWingGraph3() 
{
	m_iWingView = 1;
	m_pCurGraph = &m_WingGraph3;
	m_pCurWingGraph = m_pCurGraph;
	if(m_iView!=1) OnWOpp();
	else {
		CreateWOppCurves();
		CheckMenus();
		UpdateView();	
	}
}

void CMiarex::OnWingGraph4() 
{
	m_iWingView = 1;
	m_pCurGraph = &m_WingGraph4;
	m_pCurWingGraph = m_pCurGraph;
	if(m_iView!=1) OnWOpp();
	else {
		CreateWOppCurves();
		CheckMenus();
		UpdateView();	
	}
}


void CMiarex::OnWPolar() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	if (m_bAnimate) 
	{
		StopAnimate();
	}

	CToolBarCtrl *pTB = &(pFrame->m_MiarexBar.GetToolBarCtrl());
	pTB->PressButton(IDT_WOPP,    false);
	pTB->PressButton(IDT_WPOLARS, true);
	pTB->PressButton(IDM_3DVIEW,  false);
	pTB->PressButton(IDM_CPVIEW, false);
	pTB->PressButton(IDM_BODYDESIGN, false);
	m_pChildWnd->SetFocus();

	if(m_iView==2) 
	{
		UpdateView();
		return;
	}
	if(m_iView==5)
	{
		SetUFO();//just in case the body has been modified
		m_bResetglMesh = true;
	}

	m_iView=2;
	m_pCurGraph = m_pCurWPlrGraph;
	pFrame->DockMiarexBars();
	pFrame->m_PolarDlgBar.SetParams();
	m_FlowLinesDlg.ShowWindow(SW_HIDE);

	CreateWPolarCurves();
	CheckMenus();

	UpdateView();	
}


void CMiarex::OnSingleGraph1()
{
	m_iWPlrView = 1;
	m_pCurGraph = &m_WPlrGraph1;
	m_pCurWPlrGraph = m_pCurGraph;

	if(m_iView!=2) OnWPolar();
	else {
		SetWPlrLegendPos();
		CheckMenus();
		UpdateView();	
	}
}		

void CMiarex::OnSingleGraph2()
{
	m_iWPlrView = 1;
	m_pCurGraph = &m_WPlrGraph2;
	m_pCurWPlrGraph = m_pCurGraph;
	if(m_iView!=2) OnWPolar();
	else {
		SetWPlrLegendPos();
		CheckMenus();
		UpdateView();	
	}
}

void CMiarex::OnSingleGraph3()
{
	m_iWPlrView = 1;
	m_pCurGraph = &m_WPlrGraph3;
	m_pCurWPlrGraph = m_pCurGraph;
	if(m_iView!=2) OnWPolar();
	else {
		SetWPlrLegendPos();
		CheckMenus();
		UpdateView();	
	}
}

void CMiarex::OnSingleGraph4()
{
	m_iWPlrView = 1;
	m_pCurGraph = &m_WPlrGraph4;
	m_pCurWPlrGraph = m_pCurGraph;
	if(m_iView!=2) OnWPolar();
	else {
		SetWPlrLegendPos();
		CheckMenus();
		UpdateView();	
	}
}

 

void CMiarex::LLTAnalyze(double V0, double VMax, double VDelta, bool bSequence, bool bInitCalc)
{
	if(!m_pCurWing || !m_pCurWPolar) return;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
// check if all the foils are there...
// ...could have been deleted or renamed or not imported with AVL wing or whatever
	for (int l=0; l<=m_pCurWing->m_NPanel; l++){
		if (!pFrame->GetFoil(m_pCurWing->m_RFoil[l])){
			CString strong;
			strong = "Could not find the wing's foil "+ m_pCurWing->m_RFoil[l] +"...\nAborting Calculation";
			AfxMessageBox(strong);
			return;
		}
		if (!pFrame->GetFoil(m_pCurWing->m_LFoil[l])){
			CString strong;
			strong = "Could not find the wing's foil "+ m_pCurWing->m_LFoil[l] +"...\nAborting Calculation";
			AfxMessageBox(strong);
			return;
		}
	}

	m_pCurWing->m_bInitCalc = bInitCalc;

	m_IDlg.m_pMiarex   = this;
	m_IDlg.m_pFrame    = m_pFrame;
	
	m_IDlg.m_pWing     = m_pCurWing;
	m_IDlg.m_bSequence = bSequence;

	m_IDlg.m_Alpha      = V0;
	m_IDlg.m_AlphaMax   = VMax;
	m_IDlg.m_DeltaAlpha = VDelta;

	if(m_pCurWPolar->m_Type != 4) m_IDlg.m_bType4 = false;
	else                          m_IDlg.m_bType4 = true;
	
	m_IDlg.DoModal();

	if(m_iView!=2) SetWOpp(false, V0);
	
	else CreateWPolarCurves();
	pFrame->UpdateWOpps();
	SetWOpp(true);
	UpdateView();
}


bool CMiarex::InitializePanels()
{
	if(!m_pCurWing) return false;
	int j,k,l;

	CWaitCursor wait;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
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
		CString strong;
		strong.Format("The total number of panels is %d\n The Max Number is %d\nA reduction of the number of panels is required",m_MatSize, VLMMATSIZE);
		AfxMessageBox(strong, MB_OK);
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
		Nel = CreateElements(*(m_pSurface+j), ptr);

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

int CMiarex::CreateBodyElements(CPanel *pPanel)
{
	if(!m_pCurBody) return 0;
	int i,j,k,l;
	double uk, uk1, v, dj, dj1, dl1;
	CVector LATB, TALB;
	CVector LA, LB, TA, TB;
	CVector PLA, PTA, PLB, PTB;

	int n0, n1, n2, n3, lnx, lnh;
	int nx = m_pCurBody->m_nxPanels;
	int nh = m_pCurBody->m_nhPanels;
	int p = 0;
	int nNode = 0;
	int InitialSize = m_MatSize;
	int FullSize;

	lnx = 0; 

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
					PLB.x =  (1.0- dj) * m_pCurBody->m_FramePosition[i].x      +  dj * m_pCurBody->m_FramePosition[i+1].x;
					PLB.y = -(1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k].y   -  dj * m_pCurBody->m_Frame[i+1].m_Point[k].y;
					PLB.z =  (1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k].z   +  dj * m_pCurBody->m_Frame[i+1].m_Point[k].z;

					PTB.x =  (1.0-dj1) * m_pCurBody->m_FramePosition[i].x      + dj1 * m_pCurBody->m_FramePosition[i+1].x;
					PTB.y = -(1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k].y   - dj1 * m_pCurBody->m_Frame[i+1].m_Point[k].y;
					PTB.z =  (1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k].z   + dj1 * m_pCurBody->m_Frame[i+1].m_Point[k].z;

					PLA.x =  (1.0- dj) * m_pCurBody->m_FramePosition[i].x      +  dj * m_pCurBody->m_FramePosition[i+1].x;
					PLA.y = -(1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k+1].y -  dj * m_pCurBody->m_Frame[i+1].m_Point[k+1].y;
					PLA.z =  (1.0- dj) * m_pCurBody->m_Frame[i].m_Point[k+1].z +  dj * m_pCurBody->m_Frame[i+1].m_Point[k+1].z;

					PTA.x =  (1.0-dj1) * m_pCurBody->m_FramePosition[i].x      + dj1 * m_pCurBody->m_FramePosition[i+1].x;
					PTA.y = -(1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k+1].y - dj1 * m_pCurBody->m_Frame[i+1].m_Point[k+1].y;
					PTA.z =  (1.0-dj1) * m_pCurBody->m_Frame[i].m_Point[k+1].z + dj1 * m_pCurBody->m_Frame[i+1].m_Point[k+1].z;

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

			for (l=0; l<nh; l++)
			{
				//start with left side... same as for wings
				v = (double)(l+1) / (double)(nh); 
				m_pCurBody->GetPoint(uk,  v, false, LA);
				m_pCurBody->GetPoint(uk1, v, false, TA);
			
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


int CMiarex::CreateElements(CSurface *pSurface, CPanel *pPanel)
{
	//TODO : for  a gap at the wing's center, need to separate m_iPL and m_iPR at the tips;
	bool bNoJoinFlap ;
	int k,l;
	int n0, n1, n2, n3;
	int nNode = 0;
	int NWakeColumn = 0;	// number of wake columns
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
		else if(m_pCurWPolar->m_AnalysisType==1) side =0;
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

			if(side==0)     
			{
				m_Panel[m_MatSize].SetFrame(pSurface->LA, pSurface->LB, pSurface->TA, pSurface->TB);
			}
			else if (side==-1)   m_Panel[m_MatSize].SetFrame(pSurface->LB, pSurface->LA, pSurface->TB, pSurface->TA);

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
			if(k==pSurface->m_NYPanels-1) m_Panel[m_MatSize].m_iPL =-1;

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


				if(m_pCurWPolar && m_Panel[m_MatSize].m_bIsTrailing && m_pCurWPolar->m_AnalysisType==3){
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

bool CMiarex::CreateWakeElems(int PanelIndex)
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

int CMiarex::IsWakeNode(CVector &Pt)
{
	int in;
	for (in=0; in<m_nWakeNodes; in++)
	{
		if(Pt.IsSame(m_WakeNode[in])) return in;
	}
	return -1;
}

bool CMiarex::VLMIsSameSide(int p, int pp)
{
	if(m_Panel[p].m_iPos==m_Panel[pp].m_iPos) return true;
	return false;
}

int CMiarex::IsNode(CVector  &Pt)
{
	int in;
	for (in=0; in<m_nNodes; in++)
	{
		if(Pt.IsSame(m_Node[in])) return in;
	}
	return -1;
}

void CMiarex::Analyze(double V0, double VMax, double VDelta, bool bSequence, bool bInitCalc)
{
	int l;
	if(!m_pCurWing || !m_pCurWPolar) return;
	CMainFrame *pFrame =(CMainFrame*)m_pFrame;
// check if all the foils are there...
// ...could have been deleted or renamed or not imported with AVL wing or whatever
	for (l=0; l<=m_pCurWing->m_NPanel; l++){
		if (!pFrame->GetFoil(m_pCurWing->m_RFoil[l])){
			CString strong;
			strong = "Could not find the wing's foil "+ m_pCurWing->m_RFoil[l] +"...\nAborting Calculation";
			AfxMessageBox(strong);
			return;
		}
		if (!pFrame->GetFoil(m_pCurWing->m_LFoil[l])){
			CString strong;
			strong = "Could not find the wing's foil "+ m_pCurWing->m_LFoil[l] +"...\nAborting Calculation";
			AfxMessageBox(strong);
			return;
		}
	}

	if(m_pCurStab)
	{
		for (l=0; l<=m_pCurStab->m_NPanel; l++){
			if (!pFrame->GetFoil(m_pCurStab->m_RFoil[l])){
				CString strong;
				strong = "Could not find the elevator's foil "+ m_pCurStab->m_RFoil[l] +"...\nAborting Calculation";
				AfxMessageBox(strong);
				return;
			}
			if (!pFrame->GetFoil(m_pCurStab->m_LFoil[l])){
				CString strong;
				strong = "Could not find the elevator's foil "+ m_pCurStab->m_LFoil[l] +"...\nAborting Calculation";
				AfxMessageBox(strong);
				return;
			}
		}
	}

	if(m_pCurFin){
		for (l=0; l<=m_pCurFin->m_NPanel; l++){
			if (!pFrame->GetFoil(m_pCurFin->m_RFoil[l])){
				CString strong;
				strong = "Could not find the fin's foil "+ m_pCurFin->m_RFoil[l] +"...\nAborting Calculation";
				AfxMessageBox(strong);
				return;
			}
		}
	}
	if(m_pCurWPolar->m_AnalysisType==1)      LLTAnalyze(V0, VMax, VDelta, bSequence, bInitCalc);
	else if(m_pCurWPolar->m_AnalysisType==2) VLMAnalyze(V0, VMax, VDelta, bSequence, bInitCalc);
	else if(m_pCurWPolar->m_AnalysisType==3) PanelAnalyze(V0, VMax, VDelta, bSequence, bInitCalc);
}

CWing * CMiarex::GetWing(CString WingName)
{
	int i;
	CWing* pWing;
	for (i=0; i<m_poaWing->GetSize(); i++){
		pWing = (CWing*)m_poaWing->GetAt(i);
		if (pWing->m_WingName == WingName) return pWing;
	}
	return NULL;
}


CPlane * CMiarex::GetPlane(CString PlaneName)
{
	int i;
	CPlane* pPlane;
	for (i=0; i<m_poaPlane->GetSize(); i++){
		pPlane = (CPlane*)m_poaPlane->GetAt(i);
		if (pPlane->m_PlaneName == PlaneName) return pPlane;
	}
	return NULL;
}
 
CPlane * CMiarex::CreatePlane()
{
	CPlane* pPlane = new CPlane();
	if(!pPlane){
		AfxMessageBox("Not enough memory available to create the plane", MB_OK);
		return NULL;
	}
	return pPlane;
}


CPlane* CMiarex::AddPlane(CPlane *pPlane)
{
	int i,j;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CPlane *pOldPlane;
	CString strong;
	bool bExists   = false;
	bool bInserted = false;

	for (i=0; i<m_poaPlane->GetSize(); i++){
		pOldPlane = (CPlane*)m_poaPlane->GetAt(i);
		if (pOldPlane->m_PlaneName == pPlane->m_PlaneName) {
			bExists = true;
			break;
		}
	}

	while(!bInserted){
		if(!bExists){ 
			for (j=0; j<m_poaPlane->GetSize(); j++){
				pOldPlane = (CPlane*)m_poaPlane->GetAt(j);
				if (pPlane->m_PlaneName < pOldPlane->m_PlaneName) {
					m_poaPlane->InsertAt(j, pPlane);
					bInserted = true;
					break;
				}
			}	
			if(!bInserted)	{
				m_poaPlane->Add(pPlane);
				bInserted = true;
			}
			return pPlane;
		}
		else 
		{
			//overwrite it all
			for (int l=0; l<m_poaPlane->GetSize(); l++)
			{
				pOldPlane = (CPlane*)m_poaPlane->GetAt(l);
				if (pOldPlane->m_PlaneName == pPlane->m_PlaneName)
				{
					pFrame->DeletePlane(pOldPlane, false);
				}
			}
			bExists = false;
		}
	}
	return NULL;
}


void CMiarex::OnDefinePlane()
{
	int i;
	CPlane* pPlane = CreatePlane();
	CPlaneDlg PlaneDlg;
	PlaneDlg.m_pPlane = pPlane;
	CPlane* pOldPlane = NULL;
	PlaneDlg.m_bCheckName = true;
	pPlane->m_Wing.m_WingColor = m_WingColor;
	pPlane->m_Stab.m_WingColor = m_StabColor;
	pPlane->m_Fin.m_WingColor  = m_FinColor;

	if(IDOK == PlaneDlg.DoModal())
	{
		m_WingColor = pPlane->m_Wing.m_WingColor ;
		m_StabColor = pPlane->m_Stab.m_WingColor;
		m_FinColor  = pPlane->m_Fin.m_WingColor;

		CMainFrame *pFrame = (CMainFrame*)m_pFrame;
		pFrame->SetSaveState(false);

		bool bExists = false;
		for(i=0; i<m_poaPlane->GetSize(); i++)
		{
			pOldPlane = (CPlane*)m_poaPlane->GetAt(i);
			if(pPlane->m_PlaneName == pOldPlane->m_PlaneName)
			{
				bExists = true;
				AfxMessageBox("The name "+pPlane->m_PlaneName+" already exists", MB_OK);
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
		pFrame->UpdateUFOs();
		SetUFO();
	}
	else 
	{
		delete pPlane;
	}

	UpdateView();
}


bool CMiarex::SetModWing(CWing *pModWing)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(!pModWing) pModWing = m_pCurWing;
	CWing * pWing, *pOldWing;
	CPlane *pPlane, *pOldPlane;
	CWPolar* pWPolar;
	CWOpp * pWOpp;
	CPOpp * pPOpp;
	bool bExists = true;
	int resp, k, l;

	CStringArray NameList;
	for(k=0; k<m_poaWing->GetSize(); k++){
		pWing = (CWing*)m_poaWing->GetAt(k);
		NameList.Add(pWing->m_WingName);
	}
	for(k=0; k<m_poaPlane->GetSize(); k++){
		pPlane = (CPlane*)m_poaPlane->GetAt(k);
		NameList.Add(pPlane->m_PlaneName);
	}
	CNameDlg RDlg(this);
	RDlg.m_pStrArray = & NameList;
	RDlg.m_strQuestion = "Enter the new name for the wing :";
	RDlg.m_strName = pModWing->m_WingName;

	while (bExists){
		resp = (int)RDlg.DoModal();
		if(resp==IDOK){
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaWing->GetSize(); k++){
				pWing = (CWing*)m_poaWing->GetAt(k);
				if (pWing->m_WingName == RDlg.m_strName){
					bExists = true;
					break;
				}
			}
			for (k=0; k<m_poaPlane->GetSize(); k++){
				pPlane = (CPlane*)m_poaPlane->GetAt(k);
				if (pPlane->m_PlaneName == RDlg.m_strName){
					bExists = true;
					break;
				}
			}



			if(!bExists){
				pModWing->m_WingName = RDlg.m_strName;
				//replace the Wing in alphabetical order in the array
				//remove the current Wing from the array
				bool bInserted = false;
				for (l=0; l<m_poaWing->GetSize();l++){
					pWing = (CWing*)m_poaWing->GetAt(l);
					if(pWing == pModWing) {
						m_poaWing->RemoveAt(l);
						// but don't delete it !
						//and re-insert it
						for (l=0; l<m_poaWing->GetSize();l++){
							pWing = (CWing*)m_poaWing->GetAt(l);
							if(pWing->m_WingName.CompareNoCase(pModWing->m_WingName) >0) {
								//then insert before
								m_poaWing->InsertAt(l, pModWing);
								bInserted = true;
								break;
							}
						}
						if(!bInserted)	m_poaWing->Add(pModWing);
						break;
					}
				}
				pFrame->SetSaveState(false);
				return true;
			}





		}
		else if(resp ==10){//user wants to overwrite
			pOldWing  = GetWing(RDlg.m_strName);
			pOldPlane = GetPlane(RDlg.m_strName);
			if(pOldWing){
				for (k=0; k<m_poaWing->GetSize(); k++){
					pWing = (CWing*)m_poaWing->GetAt(k);
					if (pWing->m_WingName == RDlg.m_strName){
						for (l=(int)m_poaWPolar->GetSize()-1;l>=0; l--){
							pWPolar = (CWPolar*)m_poaWPolar->GetAt(l);
							if (pWPolar->m_UFOName == pWing->m_WingName){
								m_poaWPolar->RemoveAt(l);
								if(pWPolar==m_pCurWPolar){
									m_pCurWPolar = NULL;
									m_pCurPOpp   = NULL;
									m_pCurWOpp   = NULL;
									pFrame->m_WOperDlgBar.EnableAnalysis(false);
								}
								delete pWPolar;
							}
						}
						for (l=(int)m_poaWOpp->GetSize()-1;l>=0; l--){
							pWOpp = (CWOpp*)m_poaWOpp->GetAt(l);
							if (pWOpp->m_WingName == pWing->m_WingName){
								m_poaWOpp->RemoveAt(l);
								if(pWOpp==m_pCurWOpp){
									m_pCurWOpp = NULL;
									m_pCurPOpp = NULL;
								}
								delete pWOpp;
							}
						}
						m_poaWing->RemoveAt(k);
						if(pWing==m_pCurWing) m_pCurWing = NULL;
						delete pWing;
						bExists = false;
						break;
					}
				}
			}
			else if(pOldPlane){
				if(pOldPlane->m_bActive) AfxMessageBox("Cannot overwrite current plane", MB_OK);
				else{
					for (k=0; k<m_poaPlane->GetSize(); k++){
						pPlane = (CPlane*)m_poaPlane->GetAt(k);
						if (pPlane->m_PlaneName == RDlg.m_strName){
							for (l=(int)m_poaWPolar->GetSize()-1;l>=0; l--){
								pWPolar = (CWPolar*)m_poaWPolar->GetAt(l);
								if (pWPolar->m_UFOName == pPlane->m_PlaneName){
									m_poaWPolar->RemoveAt(l);
									if(pWPolar==m_pCurWPolar){
										m_pCurWPolar = NULL;
										m_pCurPOpp   = NULL;
										m_pCurWOpp   = NULL;
										pFrame->m_WOperDlgBar.EnableAnalysis(false);
									}
									delete pWPolar;
								}
							}
							for (l=(int)m_poaPOpp->GetSize()-1;l>=0; l--){
								pPOpp = (CPOpp*)m_poaPOpp->GetAt(l);
								if (pPOpp->m_PlaneName == pPlane->m_PlaneName){
									m_poaPOpp->RemoveAt(l);
									if(pPOpp==m_pCurPOpp){
										m_pCurPOpp = NULL;
										m_pCurPOpp = NULL;
									}
									delete pPOpp;
								}
							}
							m_poaPlane->RemoveAt(k);
							delete pPlane;
							bExists = false;
							break;
						}
					}
				}
			}
			pModWing->m_WingName = RDlg.m_strName;
			m_pCurWing = pModWing;

			pFrame->SetSaveState(false);
			return true;
		}
		else {
			return false;//cancelled 
		}
	}
	return false ;//useless...
}

bool CMiarex::SetModPlane(CPlane *pModPlane)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(!pModPlane) pModPlane = m_pCurPlane;
	CPlane * pPlane;
	CWing *pWing;
	CWPolar* pWPolar;
	CPOpp * pPOpp;
	CWOpp *pWOpp;
	bool bExists = true;
	int resp, k, l;

	CString OldName = pModPlane->m_PlaneName;

	CStringArray NameList;
	for(k=0; k<m_poaPlane->GetSize(); k++){
		pPlane = (CPlane*)m_poaPlane->GetAt(k);
		NameList.Add(pPlane->m_PlaneName);
	}
	for(k=0; k<m_poaWing->GetSize(); k++){
		pWing = (CWing*)m_poaWing->GetAt(k);
		NameList.Add(pWing->m_WingName);
	}
	CNameDlg RDlg(this);
	RDlg.m_pStrArray = & NameList;
	RDlg.m_strQuestion = "Enter the new name for the Plane :";
	RDlg.m_strName     = pModPlane->m_PlaneName;

	while (bExists){
		resp = (int)RDlg.DoModal();
		if(resp==IDOK){
			if (OldName == RDlg.m_strName) return true;
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaPlane->GetSize(); k++){
				pPlane = (CPlane*)m_poaPlane->GetAt(k);
				if (pPlane->m_PlaneName == RDlg.m_strName){
					bExists = true;
					break;
				}
			}
			for (k=0; k<m_poaWing->GetSize(); k++){
				pWing = (CWing*)m_poaWing->GetAt(k);
				if (pWing->m_WingName == RDlg.m_strName){
					bExists = true;
					break;
				}
			}
			if(!bExists){
				pModPlane->m_PlaneName = RDlg.m_strName;
				//replace the Plane in alphabetical order in the array
				//remove the current Plane from the array
				bool bInserted = false;
				for (l=0; l<m_poaPlane->GetSize();l++){
					pPlane = (CPlane*)m_poaPlane->GetAt(l);
					if(pPlane == pModPlane) {
						m_poaPlane->RemoveAt(l);
						// but don't delete it !
						//and re-insert it
						for (l=0; l<m_poaPlane->GetSize();l++){
							pPlane = (CPlane*)m_poaPlane->GetAt(l);
							if(pPlane->m_PlaneName.CompareNoCase(pModPlane->m_PlaneName) >0) {
								//then insert before
								m_poaPlane->InsertAt(l, pModPlane);
								bInserted = true;
								break;
							}
						}
						if(!bInserted)	m_poaPlane->Add(pModPlane);
						break;
					}
				}

			}

			pFrame->SetSaveState(false);
		}
		else if(resp ==10){//user wants to overwrite the old plane/wing
			if (OldName == RDlg.m_strName) return true;//don't bother to overwrite itself
			if(GetPlane(RDlg.m_strName)){
				for (k=0; k<m_poaPlane->GetSize(); k++){
					pPlane = (CPlane*)m_poaPlane->GetAt(k);
					if (pPlane->m_PlaneName == RDlg.m_strName){
						for (l=(int)m_poaWPolar->GetSize()-1;l>=0; l--){
							pWPolar = (CWPolar*)m_poaWPolar->GetAt(l);
							if (pWPolar->m_UFOName == pPlane->m_PlaneName){
								m_poaWPolar->RemoveAt(l);
								if(pWPolar==m_pCurWPolar) {
									m_pCurWPolar = NULL;
									pFrame->m_WOperDlgBar.EnableAnalysis(false);
								}
								delete pWPolar;
							}
						}
						for (l=(int)m_poaPOpp->GetSize()-1;l>=0; l--){
							pPOpp = (CPOpp*)m_poaPOpp->GetAt(l);
							if (pPOpp->m_PlaneName == pPlane->m_PlaneName){
								m_poaPOpp->RemoveAt(l);
								if(pPOpp==m_pCurPOpp) {
									m_pCurPOpp = NULL;
									m_pCurWOpp = NULL;
								}
								delete pPOpp;
							}
						}
						m_poaPlane->RemoveAt(k);
						if(pPlane==m_pCurPlane) m_pCurPlane = NULL;
						delete pPlane;
						
					}
				}
			}
			else if (GetWing(RDlg.m_strName)){//delete the wing the user wants to overwrite
				for (k=0; k<m_poaWing->GetSize(); k++){
					pWing = (CWing*)m_poaWing->GetAt(k);
					if (pWing->m_WingName == RDlg.m_strName){
						for (l=(int)m_poaWPolar->GetSize()-1;l>=0; l--){
							pWPolar = (CWPolar*)m_poaWPolar->GetAt(l);
							if (pWPolar->m_UFOName == pWing->m_WingName){
								m_poaWPolar->RemoveAt(l);
								if(pWPolar==m_pCurWPolar) {
									m_pCurWPolar = NULL;
									pFrame->m_WOperDlgBar.EnableAnalysis(false);
								}
								delete pWPolar;
							}
						}
						for (l=(int)m_poaWOpp->GetSize()-1;l>=0; l--){
							pWOpp = (CWOpp*)m_poaWOpp->GetAt(l);
							if (pWOpp->m_WingName == pWing->m_WingName){
								m_poaWOpp->RemoveAt(l);
								if(pWOpp==m_pCurWOpp) {
									m_pCurWOpp = NULL;
								}
								delete pWOpp;
							}
						}
						m_poaWing->RemoveAt(k);
						if(pWing==m_pCurWing) m_pCurWing = NULL;
						delete pWing;
					}				
				}
			}

			pModPlane->m_PlaneName = RDlg.m_strName;
			//place the Plane in alphabetical order in the array
			//remove the current Plane from the array
			for (l=0; l<m_poaPlane->GetSize();l++){
				pPlane = (CPlane*)m_poaPlane->GetAt(l);
				if(pPlane == m_pCurPlane) {
					m_poaPlane->RemoveAt(l);
					// but don't delete it !
					break;
				}
			}
			//and re-insert it
			bool bInserted = false;
			for (l=0; l<m_poaPlane->GetSize();l++){
				pPlane = (CPlane*)m_poaPlane->GetAt(l);
				if(pPlane->m_PlaneName.CompareNoCase(m_pCurPlane->m_PlaneName) <0) {
					//then insert before
					m_poaPlane->InsertAt(l, m_pCurPlane);
					bInserted = true;
					break;
				}
			}
			if(!bInserted) m_poaPlane->Add(m_pCurPlane);
			bExists = false;
			pFrame->SetSaveState(false);
		}
		else {
			return false;//cancelled 
		}

	}
	return true;
}

void CMiarex::OnEditPlane()
{
	int i;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(!m_pCurPlane)	return;
	if(m_iView==5) return;
	CWPolar *pWPolar;
	CPOpp* pPOpp;
	bool bHasResults = false;

	for (i=0; i< m_poaWPolar->GetSize(); i++)
	{
		pWPolar = (CWPolar*)m_poaWPolar->GetAt(i);
		if(pWPolar->m_Alpha.GetSize() && pWPolar->m_UFOName == m_pCurPlane->m_PlaneName)
		{
			bHasResults = true;
			break;
		}
	}

	for (i=0; i< m_poaPOpp->GetSize(); i++)
	{
		pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
		if(pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName)
		{
			bHasResults = true;
			break;
		}
	}
 
	//save a copy just in case user changes his mind (users !...)
	CPlane* pSavePlane= new CPlane();
	CPlaneDlg PlaneDlg;
	PlaneDlg.m_pPlane = m_pCurPlane;
	pSavePlane->Duplicate(m_pCurPlane);
	PlaneDlg.m_bCheckName = false;

	if(IDOK == PlaneDlg.DoModal())
	{
		CWaitCursor wait;
		if(PlaneDlg.m_bChanged)
		{
			if(bHasResults)
			{
				CModDlg dlg;
				dlg.m_Question = "The modification will erase all results associated to this Plane.\nContinue ?";
				int Ans = (int)dlg.DoModal();			
				if (Ans == IDCANCEL) {
					//restore geometry
					m_pCurPlane->Duplicate(pSavePlane);
					delete pSavePlane; // clean up	
					return;
				}
				else if(Ans==20)
				{
					CPlane* pNewPlane= new CPlane();
					pNewPlane->Duplicate(m_pCurPlane);
					if(!SetModPlane(pNewPlane)){
						delete pNewPlane;
					}
					else {
						m_pCurPlane = AddPlane(pNewPlane);
					}
					pFrame->UpdateUFOs();
					SetUFO();
					UpdateView();
					delete pSavePlane; // clean up	
					return;
				}
			}

			//then modifications are automatically recorded
			pFrame->DeletePlane(m_pCurPlane, true);// will also set new surface and Aerochord in WPolars
			m_bResetglGeom = true;
			m_bResetglMesh = true;
			m_bResetglOpp  = true;
			if(m_iView==2)		CreateWPolarCurves();
			else if(m_iView==1)	CreateWOppCurves();
			else if(m_iView==4)	CreateCpCurves();
					
		}
		SetUFO();
		pFrame->UpdateWPlrs();
		OnAdjustToWing();

	}
	else 
	{
		// restore original
		m_pCurPlane->Duplicate(pSavePlane);
	}
	delete pSavePlane; // clean up
}


void CMiarex::AddPOpp(bool bPointOut, double *Cp, double *Gamma, double *Sigma, CPOpp *pPOpp)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
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
			AfxMessageBox("Not enough memory to store the OpPoint\n", MB_OK);
			return;
		}

		pPOpp->m_Color = pFrame->GetColor(6);
		bool bFound;
		for(i=0; i<30;i++)
		{
			bFound = false;
			for (j=0; j<m_poaWOpp->GetSize();j++)
			{
				pWOpp = (CWOpp*)m_poaWOpp->GetAt(j);
				if(pWOpp->m_Color == pFrame->m_crColors[i]) bFound = true;
			}
			for (j=0; j<m_poaPOpp->GetSize();j++)
			{
				pOldPOpp = (CPOpp*)m_poaPOpp->GetAt(j);
				if(pOldPOpp->m_Color == pFrame->m_crColors[i]) bFound = true;
			}
			if(!bFound) 
			{
				pPOpp->m_Color = pFrame->m_crColors[i];
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
		if(m_pCurStab)   pPOpp->m_bStab = true; else pPOpp->m_bStab = false;
		if(m_pCurFin )   pPOpp->m_bFin  = true; else pPOpp->m_bFin = false;
		
		pPOpp->m_PlaneName = m_pCurPlane->m_PlaneName;
		
		pPOpp->m_bVLM1               = m_pCurWPolar->m_bVLM1;
		pPOpp->m_PlrName             = m_pCurWPolar->m_PlrName;
		pWOpp->m_Type                = m_pCurWPolar->m_Type;

		pPOpp->m_NStation            = m_pCurWing->m_NStation;

		pWOpp->m_nFlaps              = m_pCurWing->m_nFlaps;
		for (i=0; i<m_pCurWing->m_nFlaps; i++){
			pWOpp->m_FlapMoment[i] = m_pCurWing->m_FlapMoment[i];
		}

		for (l=0; l<m_pCurWing->m_NStation; l++){
			if(abs(m_pCurWing->m_BendingMoment[l])>abs(Cb))	Cb = m_pCurWing->m_BendingMoment[l];
		}

		pWOpp->m_MaxBending = Cb;

		if(m_pCurWPolar->m_AnalysisType==2)
		{
			pPOpp->m_NPanels             = m_VLMDlg.m_MatSize;
			pPOpp->m_Alpha               = m_VLMDlg.m_OpAlpha;
			pPOpp->m_QInf                = m_VLMDlg.m_QInf;
			pPOpp->m_Ctrl                = m_VLMDlg.m_Ctrl;

			pWOpp->m_Alpha               = m_VLMDlg.m_OpAlpha;
			pWOpp->m_QInf                = m_VLMDlg.m_QInf;
			pWOpp->m_Ctrl                = m_VLMDlg.m_Ctrl;
			pWOpp->m_CL                  = m_VLMDlg.m_CL;
			pWOpp->m_InducedDrag         = m_VLMDlg.m_InducedDrag;
			pWOpp->m_ViscousDrag         = m_VLMDlg.m_ViscousDrag;

			pWOpp->m_GCm                 = m_VLMDlg.m_GCm;
			pWOpp->m_GRm                 = m_VLMDlg.m_GRm;
			pWOpp->m_GYm                 = m_VLMDlg.m_GYm;
			pWOpp->m_VYm                 = m_VLMDlg.m_VYm;
			pWOpp->m_IYm                 = m_VLMDlg.m_IYm;

			pWOpp->m_XCP                 = m_VLMDlg.m_XCP;
			pWOpp->m_YCP                 = m_VLMDlg.m_YCP;

//			memcpy(pPOpp->m_WingWOpp.m_ICd, m_VLMDlg.m_ICd, sizeof(pPOpp->m_WingWOpp.m_ICd));
//			memcpy(pPOpp->m_WingWOpp.m_Ai,  m_VLMDlg.m_Ai,  sizeof(pPOpp->m_WingWOpp.m_Ai));
		}
		else if(m_pCurWPolar->m_AnalysisType==3)
		{
			pPOpp->m_NPanels             = m_PanelDlg.m_MatSize;
			pPOpp->m_Alpha               = m_PanelDlg.m_OpAlpha;
			pPOpp->m_QInf                = m_PanelDlg.m_QInf;

			pWOpp->m_Alpha               = m_PanelDlg.m_OpAlpha;
			pWOpp->m_QInf                = m_PanelDlg.m_QInf;
			pWOpp->m_CL                  = m_PanelDlg.m_CL;
			pWOpp->m_InducedDrag         = m_PanelDlg.m_InducedDrag;
			pWOpp->m_ViscousDrag         = m_PanelDlg.m_ViscousDrag;

			pWOpp->m_GCm                 = m_PanelDlg.m_GCm;
			pWOpp->m_GRm                 = m_PanelDlg.m_GRm;
			pWOpp->m_GYm                 = m_PanelDlg.m_GYm;
			pWOpp->m_VYm                 = m_PanelDlg.m_VYm;
			pWOpp->m_IYm                 = m_PanelDlg.m_IYm;

			pWOpp->m_XCP                 = m_PanelDlg.m_XCP;
			pWOpp->m_YCP                 = m_PanelDlg.m_YCP;

//			memcpy(pPOpp->m_WingWOpp.m_ICd, m_PanelDlg.m_ICd, sizeof(pPOpp->m_WingWOpp.m_ICd));
//			memcpy(pPOpp->m_WingWOpp.m_Ai,  m_PanelDlg.m_Ai,  sizeof(pPOpp->m_WingWOpp.m_Ai));
		}

		if(Cp) memcpy(pPOpp->m_Cp, Cp,  sizeof(pPOpp->m_Cp));

		if(Gamma)	memcpy(pPOpp->m_G,  Gamma,   sizeof(pPOpp->m_G));

		if(Sigma)	memcpy(pPOpp->m_Sigma,  Sigma,   sizeof(pPOpp->m_G));

		p = 0;
		for(i=0; i<m_pCurWing->m_MatSize; i++){
			pPOpp->m_WingWOpp.m_Cp[p] = pPOpp->m_Cp[p];
			p++;
		}
		if(m_pCurWing2){
			for(i=0; i<m_pCurWing2->m_MatSize; i++){
				pPOpp->m_Wing2WOpp.m_Cp[i] = pPOpp->m_Cp[p];
				p++;
			}
		}
		if(m_pCurStab){
			for(i=0; i<m_pCurStab->m_MatSize; i++){
				pPOpp->m_StabWOpp.m_Cp[i] = pPOpp->m_Cp[p];
				p++;
			}
		}
		if(m_pCurFin){
			for(i=0; i<m_pCurFin->m_MatSize; i++){
				pPOpp->m_FinWOpp.m_Cp[i] = pPOpp->m_Cp[p];
				p++;
			}
		}

		m_pCurWPolar->AddPoint(pPOpp);

		m_pCurWOpp = &pPOpp->m_WingWOpp;
		if(m_iView==2) 
		{
			CreateWPolarCurves();
			UpdateView();
		}
	}
	pFrame->SetSaveState(false);

	bool bIsInserted = false;
	
	// add the POpPoint to the POpPoint Array for the current PlaneName
	if(m_bStoreWOpp)
	{
		for (i=0; i<m_poaPOpp->GetSize(); i++)
		{
			pOldPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
			if (pPOpp->m_PlaneName == pOldPOpp->m_PlaneName)
			{
				if (pPOpp->m_PlrName == pOldPOpp->m_PlrName)
				{
					if(pPOpp->m_Type <3)
					{
						if(abs(pPOpp->m_Alpha - pOldPOpp->m_Alpha)<0.001)
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
							m_poaPOpp->RemoveAt(i);
							delete pOldPOpp;
							m_poaPOpp->InsertAt(i, pPOpp);
							bIsInserted = true;
							i = (int)m_poaPOpp->GetSize();// to break
						}
						else if (pPOpp->m_Alpha > pOldPOpp->m_Alpha)
						{
							//insert point
							m_poaPOpp->InsertAt(i, pPOpp);
							bIsInserted = true;
							i = (int)m_poaPOpp->GetSize();// to break
						}
					}
					else if (pPOpp->m_Type==4)
					{
						if(abs(pPOpp->m_QInf - pOldPOpp->m_QInf)<0.001){
							//replace existing point
							pPOpp->m_Color = pOldPOpp->m_Color;
							pPOpp->m_Style = pOldPOpp->m_Style;
							pPOpp->m_Width = pOldPOpp->m_Width;
							pPOpp->m_bIsVisible  = pOldPOpp->m_bIsVisible;
							pPOpp->m_bShowPoints = pOldPOpp->m_bShowPoints;
							if (m_pCurPOpp == pOldPOpp) {
								m_pCurPOpp = NULL;
								m_pCurWOpp = NULL;
							}
							m_poaPOpp->RemoveAt(i);
							delete pOldPOpp;
							m_poaPOpp->InsertAt(i, pPOpp);
							bIsInserted = true;
							i = (int)m_poaPOpp->GetSize();// to break
						}
						else if (pPOpp->m_QInf > pOldPOpp->m_QInf){
							//insert point
							m_poaPOpp->InsertAt(i, pPOpp);
							bIsInserted = true;
							i = (int)m_poaPOpp->GetSize();// to break
						}
					}
				}
			}
		}

		if (!bIsInserted) 	m_poaPOpp->Add(pPOpp);
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

	}
	else {
		delete pPOpp;
		m_pCurPOpp = NULL;
		m_pCurWOpp = NULL;
	}
}


void CMiarex::CreateWOpp(CWOpp *pWOpp, CWing *pWing)
{
	int i,j;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CWOpp *pOldWOpp = NULL;
	bool bFound;
	for(i=0; i<30;i++){
		bFound = false;
		for (j=0; j<m_poaWOpp->GetSize();j++)
		{
			pOldWOpp = (CWOpp*)m_poaWOpp->GetAt(j);
			if(pOldWOpp->m_Color == pFrame->m_crColors[i]) bFound = true;
		}
		if(!bFound) 
		{
			pWOpp->m_Color = pFrame->m_crColors[i];
			break;
		}
	}
	pWOpp->m_WingName            = pWing->m_WingName;
	pWOpp->m_PlrName             = m_pCurWPolar->m_PlrName;
	pWOpp->m_Type                = m_pCurWPolar->m_Type;
	pWOpp->m_bVLM1               = m_pCurWPolar->m_bVLM1;
	pWOpp->m_bThinSurface        = m_pCurWPolar->m_bThinSurfaces;
	pWOpp->m_bTiltedGeom         = m_pCurWPolar->m_bTiltedGeom;
	pWOpp->m_NStation            = pWing->m_NStation;
	pWOpp->m_NVLMPanels          = pWing->m_MatSize;
	pWOpp->m_AnalysisType        = m_pCurWPolar->m_AnalysisType;



	if(m_pCurWPolar->m_AnalysisType==2)
	{
		pWOpp->m_bOut                = m_VLMDlg.m_bPointOut;
		pWOpp->m_Alpha               = m_VLMDlg.m_Alpha;
		pWOpp->m_QInf                = m_VLMDlg.m_QInf;
	}
	else
	{
		pWOpp->m_bOut                = m_PanelDlg.m_bPointOut;
		pWOpp->m_Alpha               = m_PanelDlg.m_Alpha;
		pWOpp->m_QInf                = m_PanelDlg.m_QInf;
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
		Cb = __max(Cb, pWing->m_BendingMoment[l]);
	}
	pWOpp->m_MaxBending = Cb;
}

void CMiarex::OnSaveUFO() 
{
	if(!m_pCurWing) return;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pFrame->SaveUFOProject();
}

void CMiarex::OnShowWing2() 
{
	m_bShowWing2 = !m_bShowWing2;
	CheckMenus();
	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();

	UpdateView();
}

void CMiarex::OnShowElevator() 
{
	m_bShowStab = !m_bShowStab;
	CheckMenus();
	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();

	UpdateView();
}

void CMiarex::OnShowFin() 
{
	m_bShowFin = !m_bShowFin;
	CheckMenus();
	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();

	UpdateView();	
}


void CMiarex::OnAutoWingScales()
{
	m_bAutoScales = !m_bAutoScales;
	CMenu* pMenu;
	pMenu = m_pFrame->GetMenu()->GetSubMenu(2);
	if(m_bAutoScales) pMenu->CheckMenuItem(IDM_AUTOWINGSCALES, MF_BYCOMMAND | MF_CHECKED);
	else              pMenu->CheckMenuItem(IDM_AUTOWINGSCALES, MF_BYCOMMAND | MF_UNCHECKED);
}


void CMiarex::SetWGraphScale()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(!m_pCurWing ){
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
	else{
		double halfspan = m_pCurWing->m_Span/2.0;
		double xmin;
		if(m_bHalfWing) xmin = 0.0;
		else            xmin = -halfspan*pFrame->m_mtoUnit;

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
		m_WingGraph1.SetXMax( halfspan*pFrame->m_mtoUnit);
		m_WingGraph1.SetXScale();
		m_WingGraph1.SetAutoXUnit();
		m_WingGraph1.SetXMax( halfspan*pFrame->m_mtoUnit);

		m_WingGraph2.SetAutoX(false);
		m_WingGraph2.SetXMin(xmin);
		m_WingGraph2.SetXMax( halfspan*pFrame->m_mtoUnit);
		m_WingGraph2.SetXScale();
		m_WingGraph2.SetAutoXUnit();
		m_WingGraph2.SetXMax( halfspan*pFrame->m_mtoUnit);

		m_WingGraph3.SetAutoX(false);
		m_WingGraph3.SetXMin(xmin);
		m_WingGraph3.SetXMax( halfspan*pFrame->m_mtoUnit);
		m_WingGraph3.SetXScale();
		m_WingGraph3.SetAutoXUnit();
		m_WingGraph3.SetXMax( halfspan*pFrame->m_mtoUnit);

		m_WingGraph4.SetAutoX(false);
		m_WingGraph4.SetXMin(xmin);
		m_WingGraph4.SetXMax( halfspan*pFrame->m_mtoUnit);
		m_WingGraph4.SetXScale();
		m_WingGraph4.SetAutoXUnit();
		m_WingGraph4.SetXMax( halfspan*pFrame->m_mtoUnit);
	}
}


void CMiarex::OnResetAllScales() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(m_iView==2){
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
	else if(m_iView==1)
	{
		SetWGraphScale();

	}
	else if(m_iView==4)
	{
		double halfspan = m_pCurWing->m_Span/2.0;
		m_CpGraph.SetAuto(true);
		m_CpGraph.ResetXLimits();
		m_CpGraph.ResetYLimits();
	}
	UpdateView();	
}

void CMiarex::OnGLLight()
{
	m_bShowLight = true;
	UpdateView();
	m_GLLightDlg.DoModal();
	m_bShowLight = false;

	GLSetupLight();
	UpdateView();
}


void CMiarex::OnStreamOptions()
{
	m_FlowLinesDlg.SetUnits();
	m_FlowLinesDlg.ShowWindow(SW_SHOW);
}

void CMiarex::OnStreamLines() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(!m_pCurWOpp ||
	   (m_pCurWOpp && m_pCurWOpp->m_AnalysisType==1) ||
	   !m_bStream){
		m_FlowLinesDlg.ShowWindow(SW_HIDE);
	}
	else{
		if(m_iView==3){
			m_FlowLinesDlg.SetUnits();
			m_FlowLinesDlg.ShowWindow(SW_SHOW);
		}
	}
}

void CMiarex::OnManageUFO()
{
	m_UFOdlg.m_pFrame    = m_pFrame;
	m_UFOdlg.m_pMiarex   = this;
	m_UFOdlg.m_poaWing   = m_poaWing;
	m_UFOdlg.m_poaPlane  = m_poaPlane;
	m_UFOdlg.m_poaWPolar = m_poaWPolar;
	m_UFOdlg.m_bWing = true;

	m_UFOdlg.DoModal();
	UpdateView();
}


void CMiarex::PanelAnalyze(double V0, double VMax, double VDelta, bool bSequence, bool bInitCalc)
{
	if(!m_pCurWing || !m_pCurWPolar) return;

	int i,pl, pr;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

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

	m_PanelDlg.m_pMiarex = this;
	m_PanelDlg.m_pFrame  = m_pFrame;
	m_PanelDlg.m_bSequence      = bSequence;
	m_PanelDlg.m_pWPolar        = m_pCurWPolar;
	m_PanelDlg.m_MaxWakeIter    = m_MaxWakeIter;
	m_PanelDlg.m_WakeInterNodes = m_WakeInterNodes;
	m_PanelDlg.m_bSequence      = bSequence;
	m_PanelDlg.m_nWakeNodes     = m_nWakeNodes;
	m_PanelDlg.m_WakeSize       = m_WakeSize;
	m_PanelDlg.m_bTrefftz       = m_bTrefftz;

	if(m_pCurWPolar->m_Type!=4)
	{
		m_PanelDlg.m_QInf       = m_pCurWPolar->m_QInf;
		m_PanelDlg.m_Alpha      = V0;
		m_PanelDlg.m_AlphaMax   = VMax;
		m_PanelDlg.m_DeltaAlpha = VDelta;
	}
	else
	{
		m_PanelDlg.m_Alpha      = m_pCurWPolar->m_ASpec;
		m_PanelDlg.m_QInf       = V0;
		m_PanelDlg.m_QInfMax    = VMax;
		m_PanelDlg.m_DeltaQInf  = VDelta;
	}

	m_PanelDlg.m_pWing   = m_pCurWing;
	m_PanelDlg.m_pWing2  = m_pCurWing2;
	m_PanelDlg.m_pStab   = m_pCurStab;
	m_PanelDlg.m_pFin    = m_pCurFin;
	m_PanelDlg.m_pPlane  = m_pCurPlane;
	m_PanelDlg.m_MatSize = m_MatSize;
	m_PanelDlg.m_NWakeColumn  = m_NWakeColumn;
	m_PanelDlg.m_bDirichlet   = m_bDirichlet;

	m_PanelDlg.m_VersionName = pFrame->m_VersionName;

	m_PanelDlg.DoModal();

	if(m_iView!=2) SetWOpp(false, V0);
 
	else CreateWPolarCurves();
	pFrame->UpdateWOpps();
	SetWOpp(true);
	UpdateView();
}

void CMiarex::VLMAnalyze(double V0, double VMax, double VDelta, bool bSequence, bool bInitCalc)
{
	if(!m_pCurWing || !m_pCurWPolar) return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	m_bVLMFinished = false;
	if(m_pCurWPolar->m_bWakeRollUp && (m_bResetWake||bInitCalc))
	{
		m_bSetNewWake  = false;
		m_bResetglWake = true;
		UpdateView();
	}

	m_VLMDlg.m_pMiarex        = this;
	m_VLMDlg.m_pFrame         = m_pFrame;
	m_VLMDlg.m_bWakeRollUp    = m_pCurWPolar->m_bWakeRollUp;
	m_VLMDlg.m_MaxWakeIter    = m_MaxWakeIter;
	m_VLMDlg.m_WakeInterNodes = m_WakeInterNodes;
	m_VLMDlg.m_bSequence      = bSequence;
	m_VLMDlg.m_NWakeColumn    = m_NWakeColumn;
	m_VLMDlg.m_nWakeNodes     = m_nWakeNodes;
	m_VLMDlg.m_WakeSize       = m_WakeSize;
	m_VLMDlg.m_bTrefftz       = m_bTrefftz;
	m_VLMDlg.m_pWPolar        = m_pCurWPolar;
	m_VLMDlg.m_pPlane         = m_pCurPlane;
	m_VLMDlg.m_pWing          = m_pCurWing;
	m_VLMDlg.m_pWing2         = m_pCurWing2;
	m_VLMDlg.m_pStab          = m_pCurStab;
	m_VLMDlg.m_pFin           = m_pCurFin;

	if(m_pCurWPolar->m_Type<3)
	{
		m_VLMDlg.m_QInf       = m_pCurWPolar->m_QInf;
		m_VLMDlg.m_Alpha      = V0;
		m_VLMDlg.m_AlphaMax   = VMax;
		m_VLMDlg.m_DeltaAlpha = VDelta;
	}
	else if(m_pCurWPolar->m_Type==4)
	{
		m_VLMDlg.m_Alpha      = m_pCurWPolar->m_ASpec;
		m_VLMDlg.m_QInf       = V0;
		m_VLMDlg.m_QInfMax    = VMax;
		m_VLMDlg.m_DeltaQInf  = VDelta;
	}
	else if(m_pCurWPolar->m_Type==5)
	{
		m_VLMDlg.m_Alpha      = m_pCurWPolar->m_ASpec;
		m_VLMDlg.m_Control       = V0;
		m_VLMDlg.m_ControlMax    = VMax;
		m_VLMDlg.m_DeltaControl  = VDelta;
	}

	m_VLMDlg.DoModal();

	if(m_iView!=2) SetWOpp(false, V0);
 
	else CreateWPolarCurves();
	pFrame->UpdateWOpps();
	SetWOpp(true);
	UpdateView();
}

void CMiarex::OnDefineAnalysis() 
{
	if(!m_pCurWing) return;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	int i,j;

	m_WngAnalysis.m_pWing       = m_pCurWing;
	m_WngAnalysis.m_pPlane      = m_pCurPlane;
	m_WngAnalysis.m_pParent     = m_pFrame;
	m_WngAnalysis.m_poaXPolar   = &pFrame->m_oaWPolar;

	if(m_pCurPlane)		m_WngAnalysis.m_UFOName   = m_pCurPlane->m_PlaneName;
	else				m_WngAnalysis.m_UFOName   = m_pCurWing->m_WingName;

	CWPolar* pCurWPolar       = new CWPolar(m_pFrame);
	if (m_pCurPlane) pCurWPolar->m_UFOName = m_pCurPlane->m_PlaneName;
	else             pCurWPolar->m_UFOName = m_pCurWing->m_WingName;
	pCurWPolar->m_WArea        = m_pCurWing->m_Area;
	pCurWPolar->m_WMAChord     = m_pCurWing->m_MAChord;
	pCurWPolar->m_WSpan        = m_pCurWing->m_Span;
//	m_WngAnalysis.m_pWPolar    = pCurWPolar;

	if (m_WngAnalysis.DoModal() == IDOK)
	{
		//Then add WPolar to array
		pCurWPolar->m_Type            = m_WngAnalysis.m_Type;
		pCurWPolar->m_QInf            = m_WngAnalysis.m_QInf;
		pCurWPolar->m_Weight          = m_WngAnalysis.m_Weight;
		pCurWPolar->m_XCmRef          = m_WngAnalysis.m_XCmRef;
		pCurWPolar->m_ASpec           = m_WngAnalysis.m_Alpha;
		pCurWPolar->m_PlrName         = m_WngAnalysis.m_WPolarName;
		pCurWPolar->m_Density         = m_WngAnalysis.m_Density;
		pCurWPolar->m_Viscosity       = m_WngAnalysis.m_Viscosity;
		pCurWPolar->m_bVLM1           = m_WngAnalysis.m_bVLM1;
		pCurWPolar->m_bTiltedGeom     = m_WngAnalysis.m_bTiltedGeom;
		pCurWPolar->m_bWakeRollUp     = m_WngAnalysis.m_bWakeRollUp;
		pCurWPolar->m_bViscous        = m_WngAnalysis.m_bViscous;
		pCurWPolar->m_AnalysisType    = m_WngAnalysis.m_AnalysisType;
		pCurWPolar->m_bThinSurfaces   = m_WngAnalysis.m_bThinSurfaces;
		pCurWPolar->m_bGround         = m_WngAnalysis.m_bGround;
		pCurWPolar->m_Height          = m_WngAnalysis.m_Height;
		pCurWPolar->m_TotalWakeLength = m_WngAnalysis.m_TotalWakeLength;
		pCurWPolar->m_WakePanelFactor = m_WngAnalysis.m_WakePanelFactor;
		pCurWPolar->m_NXWakePanels    = m_WngAnalysis.m_NXWakePanels;
		
		pCurWPolar->m_bPolar       = true;

		pCurWPolar->m_Color = pFrame->GetColor(4);
		CWPolar *pWPolar;
		bool bFound;
		for(i=0; i<30;i++){
			bFound = false;
			for (j=0; j<m_poaWPolar->GetSize();j++){
				pWPolar = (CWPolar*)m_poaWPolar->GetAt(j);
				if(pWPolar->m_Color == pFrame->m_crColors[i]) bFound = true;
			}
			if(!bFound) {
				pCurWPolar->m_Color = pFrame->m_crColors[i];
				break;
			}
		}
		pCurWPolar->m_bIsVisible = true;

		pWPolar = GetWPolar(pCurWPolar->m_PlrName);
		pFrame->SetSaveState(false);
		if(pWPolar){
			delete pCurWPolar;
			m_pCurWPolar = pWPolar;
		}
		else {
			m_pCurWPolar = AddWPolar(pCurWPolar);
		}
		m_pCurPOpp = NULL;
		m_pCurWOpp = NULL;

		m_bResetglGeom = true;
		m_bResetglOpp  = true;
		m_bResetglMesh = true;
		m_bResetglWake = true;
		
		pFrame->UpdateWPlrs();
		SetWPlr();
		UpdateView();
		pFrame->m_WOperDlgBar.m_ctrlAnalyze.SetFocus();
	}
	else {
		delete pCurWPolar;
	}
}


void CMiarex::JoinSurfaces(CSurface *pLeftSurf, CSurface *pRightSurf, int pl, int pr)
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
				dist = abs(x*m_Panel[ppl+ls].l.x + y*m_Panel[ppl+ls].l.y + z*m_Panel[ppl+ls].l.z);
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
				dist = abs(x*m_Panel[ppl+ls].l.x + y*m_Panel[ppl+ls].l.y + z*m_Panel[ppl+ls].l.z);
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
				dist = abs(x*m_Panel[ppr+lr].l.x + y*m_Panel[ppr+lr].l.y + z*m_Panel[ppr+lr].l.z);
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
				dist = abs(x*m_Panel[ppr+lr].l.x + y*m_Panel[ppr+lr].l.y + z*m_Panel[ppr+lr].l.z);
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



void CMiarex::On3DView() 
{
	CWaitCursor wait;

	// The user has requested a 3D view
	CMainFrame* pMainFrame = (CMainFrame*)m_pFrame;
	CW3DBar* pW3DBar = (CW3DBar*)m_pW3DBar;
	CToolBarCtrl *pTB = &(pMainFrame->m_MiarexBar.GetToolBarCtrl());
	pTB->PressButton(IDT_WOPP, false);
	pTB->PressButton(IDT_WPOLARS, false);
	pTB->PressButton(IDM_3DVIEW, true);
	pTB->PressButton(IDM_CPVIEW, false);
	pTB->PressButton(IDM_BODYDESIGN, false);
	m_pChildWnd->SetFocus();

	if(m_iView==3) 
	{
		UpdateView();
		return;
	}
	m_bIs3DScaleSet = false;

	if(m_pCurPlane)
	{
		if(m_pCurPlane->m_bBody) m_pCurBody = m_pCurPlane->m_pBody;
		else                     m_pCurBody = NULL;
	}
	else m_pCurBody = NULL;

	if(m_iView==5)
	{
		SetUFO();//just in case the body has been modified
		m_bResetglMesh = true;
	}

	m_iView =3;

	GLSetViewport();
	pMainFrame->DockMiarexBars();
	pMainFrame->m_W3DBar.SetChecks();

	if(m_pCurPlane && m_pCurPlane->m_bBody && m_pCurPlane->m_bBody)
	{
		m_pCurBody = m_pCurPlane->m_pBody;
		m_bResetglBody = true;
	}
	else m_pCurBody = NULL;

	m_FlowLinesDlg.ShowWindow(SW_HIDE);

	UpdateView();
	CheckMenus();
}


void CMiarex::OnBodyDesign() 
{
	CWaitCursor wait;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CW3DBar* pW3DBar = (CW3DBar*)m_pW3DBar;
	if (m_bAnimate) StopAnimate();
	int iOldView = m_iView;
	m_FlowLinesDlg.ShowWindow(SW_HIDE);

	if(m_iView==5) 
	{
		UpdateView();
		return;
	}

	m_bIs3DScaleSet = false;

	CToolBarCtrl *pTB = &(pFrame->m_MiarexBar.GetToolBarCtrl());
	pTB->PressButton(IDT_WOPP,    false);
	pTB->PressButton(IDT_WPOLARS, false);
	pTB->PressButton(IDM_3DVIEW,  false);
	pTB->PressButton(IDM_CPVIEW, false);
	pTB->PressButton(IDM_BODYDESIGN, true);
	m_pChildWnd->SetFocus();

	m_pBodyCtrlBar->SetColumnTitle();

	m_iView=5;
	pFrame->DockMiarexBars();
	m_pBodyCtrlBar->UpdateBodies();
	SetBody();
	SetBodyScale();
	
	GLSetViewport();
	m_pCurGraph = NULL;

	UpdateView();	
	CheckMenus();
}

void CMiarex::OnRedo()
{
	if(m_iView==5)	m_pBodyCtrlBar->OnRedo();
}

void CMiarex::OnUndo()
{
	if(m_iView==5)	m_pBodyCtrlBar->OnUndo();
}

void CMiarex::OnInsertBodyPoint()
{
	CVector Real;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	if(m_BodyLineRect.PtInRect(m_ptPopUp))
	{
		m_pBodyCtrlBar->TakePicture();
		m_pBodyCtrlBar->StorePicture();

		Real.x = (m_RealPopUp.x - m_BodyOffset.x)/m_BodyScale;
		Real.z = (m_RealPopUp.y - m_BodyOffset.y)/m_BodyScale;
		Real.y = 0.0;

		if(m_pCurFrame)
		{
			int sel = m_pCurBody->InsertFrame(Real);
			if(sel>0) m_pCurFrame = m_pCurBody->m_Frame+sel;
			else      m_pCurFrame = NULL;
		}

		m_bResetglBody   = true;
		m_bResetglBody2D = true;
		pFrame->SetSaveState(false);
		UpdateView();
	}
	else if(m_FrameRect.PtInRect(m_ptPopUp) && m_pCurFrame)
	{
		m_pBodyCtrlBar->TakePicture();
		m_pBodyCtrlBar->StorePicture();

		Real.x = (m_RealPopUp.x - m_FrameOffset.x)/m_FrameScale;
		Real.y = (m_RealPopUp.y - m_FrameOffset.y)/m_FrameScale;
		Real.z = 0.0;

		 m_pCurBody->InsertPoint(Real);
		m_bResetglBody   = true;
		m_bResetglBody2D = true;
		pFrame->SetSaveState(false);
		UpdateView();
	}
}


void CMiarex::OnDeleteBodyPoint()
{
	int i,n;
	CVector Real;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CFrame *pBodyFrame;

	if(m_BodyLineRect.PtInRect(m_ptPopUp))
	{
		m_pBodyCtrlBar->TakePicture();
		m_pBodyCtrlBar->StorePicture();

		Real.x =  (m_RealPopUp.x - m_BodyOffset.x)/m_BodyScale;
		Real.y =  (m_RealPopUp.y - m_BodyOffset.y)/m_BodyScale;
		Real.z = 0.0;
		n =  m_pCurBody->IsFramePos(Real, m_BodyScale/m_BodyRefScale);
		if (n>=0) 
		{
			n = m_pCurBody->RemoveFrame(n);
			m_pBodyCtrlBar->FillFrameList();
			m_pBodyCtrlBar->SetFrame(n);
			m_bResetglBody   = true;
			m_bResetglBody2D = true;
			pFrame->SetSaveState(false);
		}
		
		UpdateView();
	}
	else if(m_FrameRect.PtInRect(m_ptPopUp) && m_pCurFrame)
	{
		m_pBodyCtrlBar->TakePicture();
		m_pBodyCtrlBar->StorePicture();

		Real.x =  (m_RealPopUp.x - m_FrameOffset.x)/m_FrameScale;
		Real.y =  (m_RealPopUp.y - m_FrameOffset.y)/m_FrameScale;
		Real.z = 0.0;

		n =   m_pCurFrame->IsPoint(Real, m_BodyScale/m_BodyRefScale);
		if (n>=0) 
		{
			for (i=0; i<m_pCurBody->m_NStations;i++)
			{
				pBodyFrame = m_pCurBody->m_Frame+i;
				pBodyFrame->RemovePoint(n);
			}
			m_pCurBody->m_NSideLines--;
			m_pCurBody->SetKnots();
			m_pBodyCtrlBar->SetFrame(m_pCurBody->m_iActiveFrame);
			m_bResetglBody   = true;
			m_bResetglBody2D = true;
			pFrame->SetSaveState(false);
		}

		UpdateView();
	}	
}


void CMiarex::OnDeleteCurBody()
{
	if(!m_pCurBody) return;
	int i;
	//Delete the currently selected body, if any
	m_bAnimate = false;

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString strong;
	CPlane *pOldPlane;
	
	bool bIsInUse = false;

	for (i=0; i<m_poaPlane->GetCount();i++)
	{
		pOldPlane = (CPlane*)m_poaPlane->GetAt(i);
		if(pOldPlane->m_bBody && pOldPlane->m_pBody == m_pCurBody)
		{
			bIsInUse = true;
			break;
		}
	}

	if(bIsInUse)
	{
		strong = "The body " +  m_pCurBody->m_BodyName +" is in use by a plane.\n Delete Anyhow?\n";
		if(IDYES != AfxMessageBox(strong, MB_YESNOCANCEL)) return;
	}
	else
	{
		strong = "Are you sure you want to delete the body :\n" +  m_pCurBody->m_BodyName +"?\n";
		if(IDYES != AfxMessageBox(strong, MB_YESNOCANCEL)) return;
	}

	if(bIsInUse)
	{
		for (i=0; i<m_poaPlane->GetCount();i++)
		{
			pOldPlane = (CPlane*)m_poaPlane->GetAt(i);
			if(pOldPlane->m_pBody == m_pCurBody)
			{
				pOldPlane->m_bBody = false;
				pOldPlane->m_pBody = NULL;
				pFrame->DeletePlane(pOldPlane, true);
			}
			m_bResetglGeom = true;
		}
	}

	pFrame->DeleteBody(m_pCurBody);
	m_pBodyCtrlBar->UpdateBodies();
	SetBody();
	UpdateView();
}

void CMiarex::OnDuplicateCurBody() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	//Duplicate the currently selected Body
	if(!m_pCurBody)	return;


	CBody* pNewBody= new CBody();
	pNewBody->Duplicate(m_pCurBody);

	if(!SetModBody(pNewBody)){
		delete pNewBody;
		UpdateView();
	}
	else {
		m_pCurBody = AddBody(pNewBody);
		m_pBodyCtrlBar->UpdateBodies();
		SetBody();
	}
}

void CMiarex::OnScaleFrame()
{
	if(!m_pCurBody || !m_pCurFrame) return;

	CBodyScaleDlg dlg(this);
	dlg.m_FrameID = m_pCurBody->m_iActiveFrame;
	dlg.m_bFrameOnly = true;

	if(dlg.DoModal()==IDOK)
	{
		m_pBodyCtrlBar->TakePicture();
		m_pBodyCtrlBar->StorePicture();
		m_pCurBody->Scale(dlg.m_XFactor,dlg.m_YFactor,dlg.m_ZFactor, true, m_pCurBody->m_iActiveFrame);
		m_bResetglBody     = true;
		m_bResetglBodyMesh = true;
		m_bResetglBody2D   = true;
		UpdateView();
	}
}

void CMiarex::OnScaleCurBody()
{
	if(!m_pCurBody) return;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CBodyScaleDlg dlg(this);

	dlg.m_FrameID = m_pCurBody->m_iActiveFrame;

	if(dlg.DoModal()==IDOK)
	{
		m_pBodyCtrlBar->TakePicture();
		m_pBodyCtrlBar->StorePicture();
		m_pCurBody->Scale(dlg.m_XFactor,dlg.m_YFactor,dlg.m_ZFactor, dlg.m_bFrameOnly, dlg.m_FrameID);
		m_bResetglBody     = true;
		m_bResetglBodyMesh = true;
		m_bResetglBody2D   = true;
		UpdateView();
	}
}

void CMiarex::OnExportCurBody()
{
	if(!m_pCurBody) return;
	m_pCurBody->Export(m_NXPoints, m_NHoopPoints);
}

void CMiarex::OnSaveCurBodyAsProject()
{
	if(!m_pCurBody) return;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pFrame->SaveBodyProject(m_pCurBody);
}

void CMiarex::OnRenameCurBody()
{
	if(!m_pCurBody) return;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	//Rename the currently selected body

	SetModBody(m_pCurBody);
	m_pBodyCtrlBar->UpdateBodies();
	SetBody();
	UpdateView();
}


CBody* CMiarex::AddBody(CBody *pBody)
{
	//adds the pBody to the m_oaBody array
	//places it in alphabetical order 
	bool bExists   = false;
	bool bInserted = false;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CBody *pOldBody;
	CString strong;
	int i,j;	

	if(pBody->m_BodyName.GetLength())
	{
		for (i=0; i<m_poaBody->GetSize(); i++)
		{
			pOldBody = (CBody*)m_poaBody->GetAt(i);
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
			for (j=0; j<m_poaBody->GetSize(); j++)
			{
				pOldBody = (CBody*)m_poaBody->GetAt(j);
				if (pBody->m_BodyName.CompareNoCase(pOldBody->m_BodyName)<0) {
					m_poaBody->InsertAt(j, pBody);
					bInserted = true;
					break;
				}
			}	
			if(!bInserted)	{
				m_poaBody->Add(pBody);
				bInserted = true;
			}
			pFrame->SetSaveState(false);
			return pBody;
		}
		else {
			//Ask for user intentions
			if(SetModBody(pBody)) 
			{
				m_poaBody->Add(pBody);
				return pBody;
			}
			else
			{
				delete pBody;
				return NULL;
			}

			bExists = false;
		}
	}
	return NULL;
}



bool CMiarex::SetModBody(CBody *pModBody)
{
	CMainFrame *pMainFrame = (CMainFrame*)m_pFrame;
	if(!pModBody) pModBody = m_pCurBody;
	CBody * pBody, *pOldBody;
	CPlane *pPlane;
	CString strong;

	bool bExists = true;
	int resp, k, l;

	CStringArray NameList;
	for(k=0; k<m_poaBody->GetSize(); k++)
	{
		pBody = (CBody*)m_poaBody->GetAt(k);
		NameList.Add(pBody->m_BodyName);
	}

	CNameDlg RDlg(this);
	RDlg.m_pStrArray = & NameList;
	RDlg.m_strQuestion = "Enter the new name for the Body :";
	RDlg.m_strName = pModBody->m_BodyName;

	while (bExists)
	{
		resp = (int)RDlg.DoModal();
		if(resp==IDOK)
		{
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaBody->GetSize(); k++){
				pBody = (CBody*)m_poaBody->GetAt(k);
				if (pBody->m_BodyName == RDlg.m_strName){
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
				for (l=0; l<m_poaBody->GetSize();l++){
					pBody = (CBody*)m_poaBody->GetAt(l);
					if(pBody == pModBody) {
						m_poaBody->RemoveAt(l);
						// but don't delete it !
						//and re-insert it
						for (l=0; l<m_poaBody->GetSize();l++){
							pBody = (CBody*)m_poaBody->GetAt(l);
							if(pBody->m_BodyName.CompareNoCase(pModBody->m_BodyName) >0) {
								//then insert before
								m_poaBody->InsertAt(l, pModBody);
								bInserted = true;
								break;
							}
						}
						if(!bInserted)	m_poaBody->Add(pModBody);
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
			pOldBody  = GetBody(RDlg.m_strName);
			if(pOldBody)
			{
				//first we check if this old body is used by one or more planes
				bool bIsInUse = false;
				resp = IDYES;
				for(k=0; k<m_poaPlane->GetSize(); k++)
				{
					pPlane = (CPlane*)m_poaPlane->GetAt(k);
					if(pPlane->m_bBody && pPlane->m_pBody==pOldBody)
					{
						bIsInUse = true;
						break;
					}
				}
				if(bIsInUse)
				{
					strong = "The body "+pOldBody->m_BodyName+" is used by one or more planes.\n Overwrite anyway ? (Results will be lost)";
					resp = AfxMessageBox(strong, MB_YESNOCANCEL);
				}
				if(resp==IDYES)
				{
					for (k=0; k<m_poaBody->GetSize(); k++)
					{
						pBody = (CBody*)m_poaBody->GetAt(k);
						if (pBody->m_BodyName == RDlg.m_strName)
						{
							m_poaBody->RemoveAt(k);
							if(pBody==m_pCurBody) 
							{
								m_pCurBody = NULL;
								m_pCurFrame = NULL;
							}
							delete pBody;
							bExists = false;
							break;
						}
					}
					for(k=0; k<m_poaPlane->GetSize(); k++)
					{
						pPlane = (CPlane*)m_poaPlane->GetAt(k);
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
		else {
			return false;//cancelled 
		}
	}
	return false ;//useless...
}


CBody * CMiarex::GetBody(CString BodyName)
{
	int i;
	CBody* pBody;
	for (i=0; i<m_poaBody->GetSize(); i++){
		pBody = (CBody*)m_poaBody->GetAt(i);
		if (pBody->m_BodyName == BodyName) return pBody;
	}
	return NULL;
}



void CMiarex::OnResetBodyScale()
{
	CPoint pt;
	GetCursorPos(&pt);
	m_pChildWnd->ScreenToClient( &pt);

	m_bIs3DScaleSet = false;
	SetBodyScale();
	Set3DScale();
//	UpdateBody();
	UpdateView();
}



void CMiarex::OnBodyGrid() 
{
	m_BodyGridDlg.m_pMiarex = this;
	m_BodyGridDlg.m_pFrame = m_pFrame;

	m_BodyGridDlg.DoModal();
	m_bResetglBody2D = true;
	UpdateView();
}



void CMiarex::OnCpLegend() 
{
	CCpScaleDlg dlg;
	dlg.m_CpMin = m_LegendMin;
	dlg.m_CpMax = m_LegendMax;
	dlg.m_bShowCpScale = m_bShowCpScale;
	dlg.m_bAutoCpScale = m_bAutoCpScale;

	if(dlg.DoModal() == IDOK)
	{
		m_LegendMin     = dlg.m_CpMin;
		m_LegendMax     = dlg.m_CpMax;
		m_bAutoCpScale  = dlg.m_bAutoCpScale;
		m_bShowCpScale  = dlg.m_bShowCpScale;
		m_bResetglOpp    = true;
		m_bResetglLegend = true;
		UpdateView();
	}
}

void CMiarex::GLCallViewLists()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);


	if (m_pCurWPolar && m_pCurWPolar->m_AnalysisType ==3 && m_pCurWPolar->m_bTiltedGeom 
		&& m_bWakePanels) 
		glCallList(WINGWAKEPANELS);

	if(m_bMoments && m_pCurWOpp)	
		glCallList(VLMMOMENTS);

	if (m_pCurWOpp
		&& m_bStream
		&& m_pCurWOpp->m_AnalysisType>=2
		&& !m_bResetglStream)
		glCallList(VLMSTREAMLINES);//streamlines are not rotated

	if(m_pCurWOpp && m_bSpeeds && m_pCurWOpp->m_AnalysisType>=2 && !m_bResetglFlow) 
		glCallList(SURFACESPEEDS);

	if(m_bglLight)	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	else {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}

	if (m_pCurWOpp) glRotated(m_pCurWOpp->m_Alpha, 0.0, 1.0, 0.0);

/*	glColor3d(1.0, 0.0, 0.5);
	glLineWidth(2.0);
	glBegin(GL_LINES);
	{
		glVertex3d(RA.x, RA.y, RA.z);
		glVertex3d(RB.x, RB.y, RB.z);
	}*/

	if(m_bSurfaces && m_pCurBody)		glCallList(BODYSURFACES);

	if(m_bSurfaces)	
	{
		if(m_pCurWing)  glCallList(WINGSURFACES);
		if(m_pCurWing2) glCallList(WING2SURFACES);
		if(m_pCurStab)  glCallList(STABSURFACES);
		if(m_pCurFin)   glCallList(FINSURFACES);
	}

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	if (m_pCurWPolar && m_pCurWPolar->m_AnalysisType ==3 && !m_pCurWPolar->m_bTiltedGeom 
		&& m_bWakePanels) 
		glCallList(WINGWAKEPANELS);

	if(m_bVLMPanels && m_pCurWing)
	{
		if(!(m_b3DVLMCl&&m_pCurWOpp) && !m_bSurfaces)//else panels will be filled by Cp color
			glCallList(MESHBACK);
		glCallList(MESHPANELS);
	}

	if(m_bVortices && m_pCurWing)
	{
		glCallList(VLMCTRLPTS);
		glCallList(VLMVORTICES);
	}
	if(m_b3DVLMCl && m_pCurWOpp && m_pCurWOpp->m_AnalysisType>=2)
		glCallList(PANELCP);

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
	if(m_b3DDownwash && m_pCurWOpp)
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
	if(m_bOutline)	
	{
		if(m_pCurWing)  glCallList(WINGOUTLINE);
		if(m_pCurWing2) glCallList(WING2OUTLINE);
		if(m_pCurStab)  glCallList(STABOUTLINE);
		if(m_pCurFin)   glCallList(FINOUTLINE);
		if(m_pCurBody)	glCallList(BODYGEOM);
	}
}

void CMiarex::GLRenderView(CDC *pDC)
{
	CW3DBar *pW3DBar = (CW3DBar*)m_pW3DBar;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CChildView *pChildView = (CChildView*)m_pChildWnd;
	wglMakeCurrent(pDC->m_hDC, pChildView->m_hRC);

	GLdouble pts[4];
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
		if(m_iView==3)
		{
			if (m_b3DVLMCl && m_pCurWOpp && m_pCurWOpp->m_AnalysisType>=2 && m_bShowCpScale)
				glCallList(WOPPCPLEGEND);
			if(m_pCurWing) 
				glCallList(WINGLEGEND);
			if(m_pCurWOpp) 
				glCallList(WOPPLEGEND);
		}

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
				COLORREF color;
				color = RGB((int)(m_GLLightDlg.m_Red  *255),
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
	}
	glPopMatrix();


	glDisable(GL_CLIP_PLANE1);
	glFinish();
	SwapBuffers(pDC->m_hDC);
	wglMakeCurrent(pDC->m_hDC, NULL);
}

void CMiarex::OnLButtonUp(UINT nFlags, CPoint point) 
{
//	if (GetCapture() == this) ReleaseCapture();	
	SetCursor(m_hcCross);
	if(m_iView==5 && m_pCurBody)
	{
		if(!m_bTrans)
		{
			if(point==m_PointDown)
			{
				m_bResetglBodyPoints = true;
			}
			else
			{
				CWaitCursor wait;
				int n1, n2;
				n1 = m_pCurBody->m_iActiveFrame;
				if(m_pCurFrame)	n2 = m_pCurFrame->m_iSelect;
				else			n2 = -10;

				if (m_BodyLineRect.PtInRect(point) && n1>=0 && n1<m_pCurBody->m_NStations)
				{	//the user has been dragging a point
					m_pBodyCtrlBar->FillFrameList();
					m_pBodyCtrlBar->FillPointList();
					m_pBodyCtrlBar->SetFrameSel(m_pCurBody->m_iActiveFrame);
					m_bResetglBody     = true;
					m_bResetglBodyMesh = true;
					m_bResetglBody2D   = true;
				}
				else if (m_FrameRect.PtInRect(point) && n2>=0 && n2<m_pCurFrame->m_NPoints)
				{	//the user has been dragging a point
					m_pBodyCtrlBar->FillFrameList();
					m_pBodyCtrlBar->FillPointList();
					m_pBodyCtrlBar->SetPointSel(m_pCurFrame->m_iSelect);
					m_bResetglBody     = true;
					m_bResetglBodyMesh = true;
					m_bResetglBody2D   = true;
				}
			}
		}
		else m_bResetglBody2D   = true;
		
		UpdateView();
	}
	m_bTrans = false;
	m_bDragPoint  = false;
	if(m_iView==3 || m_iView==5)
	{	
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
}

void CMiarex::OnBodyResolution()
{
	CBodyNURBSDlg dlg;
	dlg.m_pMainFrame = m_pFrame;
	dlg.m_NXPoints = m_NXPoints;
	dlg.m_NHoopPoints = m_NHoopPoints;
	if(dlg.DoModal()==IDOK)
	{
		m_NXPoints    = dlg.m_NXPoints;
		m_NHoopPoints = dlg.m_NHoopPoints;
		m_bResetglBody = true;
		UpdateView();
	}
}

void CMiarex::OnNewBody()
{
	//Define a Body from scratch
	//On exit, check if the Body's name is already used
	if(m_iView!=5) OnBodyDesign();

	CMainFrame *pMainFrame = (CMainFrame*)m_pFrame;
	CBody* pBody = new CBody();

	pMainFrame->SetSaveState(false);

	m_pCurBody = AddBody(pBody);
	m_pBodyCtrlBar->UpdateBodies();
	SetBody();

	m_bIs3DScaleSet = false;
	SetBodyScale();

	UpdateView();
}

void CMiarex::OnInterpolateBodyPoints()
{
	if(!m_pCurBody) return;
	CMainFrame *pMainFrame = (CMainFrame*)m_pFrame;
	CBody* pNewBody = new CBody();
	pNewBody->Duplicate(m_pCurBody);
	pNewBody->InterpolateSurface();
	pNewBody->m_LineType = 2;

	pMainFrame->SetSaveState(false);

	m_pCurBody = AddBody(pNewBody);
	m_pBodyCtrlBar->UpdateBodies();
	SetBody();

	m_bResetglBody2D = true;
	m_bResetglBody   = true;

	UpdateView();


}


bool CMiarex::UnlockCurBody() 
{
	if(!m_pCurBody) return false;

	if(!m_pCurBody->m_bLocked) return true;

	int i,resp;
	CString strong;
	CMainFrame *pMainFrame = (CMainFrame*)m_pFrame;
	CPlane *pPlane;
	//body is locked by results
	strong = "The body "+m_pCurBody->m_BodyName+" is used by one or more planes.\n Continue ? (Results will be lost1)";
	resp = AfxMessageBox(strong, MB_YESNOCANCEL);
	if(resp!=IDYES)
		return false;
	else
	{
		for(i=0; i<pMainFrame->m_oaPlane.GetSize(); i++)
		{
			pPlane = (CPlane*)pMainFrame->m_oaPlane.GetAt(i);
			if(pPlane->m_bBody && pPlane->m_pBody && pPlane->m_pBody==m_pCurBody)
				pMainFrame->DeletePlane(pPlane, true);
		}
		m_pCurBody->m_bLocked = false;
	}

	return true;
}


void CMiarex::OnShowOnlyActiveFrame()
{
	m_bCurFrameOnly = !m_bCurFrameOnly;
	m_bResetglBody2D = true;
	CheckMenus();
	UpdateView();
}


void * CMiarex::GetUFOPlrVariable(CWPolar *pWPolar, int iVar)
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
		default:
			pVar = &pWPolar->m_Alpha;
			break;
	}
	return pVar;
}


void CMiarex::SetWGraphTitles(Graph* pGraph, int iX, int iY)
{
	CMainFrame *pMainFrame = (CMainFrame*)m_pFrame;
	CString StrLength;
	CString StrSpeed;
	CString StrMoment;
	GetLengthUnit(StrLength, pMainFrame->m_LengthUnit);
	GetSpeedUnit(StrSpeed, pMainFrame->m_SpeedUnit);
	GetMomentUnit(StrMoment, pMainFrame->m_MomentUnit);

	switch (iX)
	{
		case 0:
			pGraph->SetXTitle("Alpha°");
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
			pGraph->SetXTitle("Gamma°");	
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
			pGraph->SetXTitle("Oswald");
			break;
		case 27:
			pGraph->SetXTitle("(XCp-XCG)/MAC(%)");
			break;
		case 28:
			pGraph->SetXTitle("ctrl");
			break;
		default:
			pGraph->SetXTitle("Alpha°");
			break;
	}

	switch (iY)
	{
		case 0:
			pGraph->SetYTitle("Alpha°");
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
			pGraph->SetYTitle("Gamma°");	
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
			pGraph->SetYTitle("Oswald");
			break;
		case 27:
			pGraph->SetYTitle("(XCp-XCG)/MAC(%)");
			break;
		case 28:
			pGraph->SetYTitle("ctrl var");
			break;
		default:
			pGraph->SetYTitle("Alpha°");
			break;
	}
} 


void CMiarex::GLInverseMatrix()
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
	
/*	//Step 2. negate the translation vector portion of the 4x4 matrix 
	//and then rotate it using the newly constructed 3x3 rotation matrix
	double vTmp[3], vTmp2[3];

	vTmp[0] = -MatIn[3][0];
	vTmp[1] = -MatIn[3][1];
	vTmp[2] = -MatIn[3][2];

	vTmp2[0] = vTmp[0]*MatOut[0][0] + vTmp[1]*MatOut[1][0] + vTmp[2]*MatOut[2][0];
	vTmp2[1] = vTmp[0]*MatOut[0][1] + vTmp[1]*MatOut[1][1] + vTmp[2]*MatOut[2][1];
	vTmp2[2] = vTmp[0]*MatOut[0][2] + vTmp[1]*MatOut[1][2] + vTmp[2]*MatOut[2][2];


	//Step 3. put the new translation vector into the new 4x4 matrix
	MatOut[3][0] = vTmp2[0];
	MatOut[3][1] = vTmp2[1];
	MatOut[3][2] = vTmp2[2];


	//Step 4. do some house cleaning so that the matrix can be used with OpenGL
	MatOut[0][3] = MatOut[1][3] = MatOut[2][3] = 0.0;
	MatOut[3][3] = 1.0;*/
}

bool CMiarex::Intersect(CVector const &LA, CVector const &LB, CVector const &TA, CVector const &TB, CVector const &Normal,
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

	if(abs(s)>0.0)
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


void CMiarex::Set3DRotationCenter()
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


void CMiarex::Set3DRotationCenter(CPoint point)
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

	if(m_iView==3)
	{
		for(p=0; p<m_MatSize; p++)
		{
			if(m_Panel[p].Intersect(AA, U, I, dist)) 
//			if(Intersect(m_Node[m_Panel[p].m_iLA], m_Node[m_Panel[p].m_iLB], m_Node[m_Panel[p].m_iTA], m_Node[m_Panel[p].m_iTB],
//				         m_Panel[p].Normal, AA, U, I, dist)) 
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
	else if(m_iView==5 && m_pCurBody)
	{
		if(m_pCurBody->m_LineType==2)
		{
			int nx, nh, inx, inh;
			CVector N, LATB, TALB, LA, LB, TA, TB;
			nx = min(500, m_NXPoints);
			nh = max(m_NHoopPoints, 3);

			//right side
			p = 0;
			for (inx=0; inx<nx; inx++)
			{
				p++;
				for (inh=1; inh<=nh; inh++)
				{
					LA.Set(m_T[p+1]);   LB.Set(m_T[p]);   TA.Set(m_T[p+nh+2]);   TB.Set(m_T[p+nh+1]);

					LATB = TB - LA;
					TALB = LB - TA;
					N = TALB * LATB;
					N.Normalize();

					if(Intersect(LA, LB, TA, TB, N, AA, U, I, dist)) 
					{
						if(dist < dmin) 
						{
							dmin = dist;
							PP.Set(I);
							bIntersect = true;
						}
					}

					N.y = -N.y;  LA.y = -LA.y;  LB.y = -LB.y ;  TA.y = -TA.y;  TB.y = -TB.y;
			
					if(Intersect(LA, LB, TA, TB, N, AA, U, I, dist)) 
					{
						if(dist < dmin) 
						{
							dmin = dist;
							PP.Set(I);
							bIntersect = true;
						}
					}

					p++;
				}
			}
		}
		else if(m_pCurBody->m_LineType==1)
		{
			int inh, inx;
			CVector Tj, Tjp1, LA, LB, TA, TB, TALB, LATB, N;
			for (inh=0; inh<m_pCurBody->m_NSideLines-1;inh++)
			{
				for (inx=0; inx<m_pCurBody->m_NStations-1;inx++)
				{
					Tj.Set(m_pCurBody->m_FramePosition[inx].x,     0.0, 0.0);
					Tjp1.Set(m_pCurBody->m_FramePosition[inx+1].x, 0.0, 0.0);

					LB = m_pCurBody->m_Frame[inx].m_Point[inh];			LB.x += Tj.x;
					TB = m_pCurBody->m_Frame[inx+1].m_Point[inh];		TB.x += Tjp1.x;
					TA = m_pCurBody->m_Frame[inx+1].m_Point[inh+1];		TA.x += Tjp1.x;
					LA = m_pCurBody->m_Frame[inx].m_Point[inh+1];		LA.x += Tj.x;

					TALB = LB - TA;
					LATB = TB - LA;
					N = TALB * LATB;
					N.Normalize();

					if(Intersect(LA, LB, TA, TB, N, AA, U, I, dist)) 
					{
						if(dist < dmin) 
						{
							dmin = dist;
							PP.Set(I);
							bIntersect = true;
						}
					}

					N.y = -N.y;  LA.y = -LA.y;  LB.y = -LB.y ;  TA.y = -TA.y;  TB.y = -TB.y;
			
					if(Intersect(LA, LB, TA, TB, N, AA, U, I, dist)) 
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
		}
	}

	m_iSel = p;
	RA = AA; 
	RB = BB;

	if(bIntersect)
	{
		if(m_pCurWOpp)
		{
			PP.RotateY(m_pCurWOpp->m_Alpha);
		}
//		instanteneous visual transition
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
//			Sleep(1);
		}	
	}
}


void CMiarex::OnControlAnalysis() 
{
	if(!m_pCurWing) return;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	int i,j;

	m_CtrlDlg.m_pPlane     = m_pCurPlane;
	m_CtrlDlg.m_pWing      = m_pCurWing;
	m_CtrlDlg.m_pWing2     = m_pCurWing2;
	m_CtrlDlg.m_pStab      = m_pCurStab;
	m_CtrlDlg.m_pFin       = m_pCurFin;
	m_CtrlDlg.m_pMainFrame = m_pFrame;
	m_CtrlDlg.m_poaXPolar  = &pFrame->m_oaWPolar;

	if(m_pCurPlane) 
		m_CtrlDlg.m_UFOName   = m_pCurPlane->m_PlaneName;
	else
		m_CtrlDlg.m_UFOName   = m_pCurWing->m_WingName;

	CWPolar* pCurWPolar       = new CWPolar(m_pFrame);
	if (m_pCurPlane) pCurWPolar->m_UFOName = m_pCurPlane->m_PlaneName;
	else             pCurWPolar->m_UFOName = m_pCurWing->m_WingName;
	pCurWPolar->m_WArea        = m_pCurWing->m_Area;
	pCurWPolar->m_WMAChord     = m_pCurWing->m_MAChord;
	pCurWPolar->m_WSpan        = m_pCurWing->m_Span;

	if (m_CtrlDlg.DoModal() == IDOK)
	{
		//Then add WPolar to array
		pCurWPolar->m_Type            = 5;
		pCurWPolar->m_QInf            = 0.0;
		pCurWPolar->m_Weight          = 0.0;
		pCurWPolar->m_XCmRef          = 0.0;
		pCurWPolar->m_ASpec           = 0.0;
		pCurWPolar->m_PlrName         = m_CtrlDlg.m_WPolarName;
		pCurWPolar->m_Density         = m_CtrlDlg.m_Density;
		pCurWPolar->m_Viscosity       = m_CtrlDlg.m_Viscosity;
		pCurWPolar->m_bVLM1           = true;
		pCurWPolar->m_bTiltedGeom     = false;
		pCurWPolar->m_bWakeRollUp     = false;
		pCurWPolar->m_bViscous        = false;
		pCurWPolar->m_AnalysisType    = 2; //vlm2
		pCurWPolar->m_bThinSurfaces   = false;
		pCurWPolar->m_bGround         = false;
		pCurWPolar->m_Height          = 0.0;
		pCurWPolar->m_TotalWakeLength = 10.0;
		pCurWPolar->m_WakePanelFactor = 1.1;
		pCurWPolar->m_NXWakePanels    = 1;

		pCurWPolar->m_nControls = m_CtrlDlg.m_nControls;
		for(i=0; i<m_CtrlDlg.m_nControls; i++)
		{
			pCurWPolar->m_bActiveControl[i] = m_CtrlDlg.m_bActiveControl[i];
			pCurWPolar->m_MinControl[i] = m_CtrlDlg.m_MinControl[i];
			pCurWPolar->m_MaxControl[i] = m_CtrlDlg.m_MaxControl[i];
		}

		pCurWPolar->m_bPolar = true;

		pCurWPolar->m_Color = pFrame->GetColor(4);
		CWPolar *pWPolar;
		bool bFound;
		for(i=0; i<30;i++)
		{
			bFound = false;
			for (j=0; j<m_poaWPolar->GetSize();j++)
			{
				pWPolar = (CWPolar*)m_poaWPolar->GetAt(j);
				if(pWPolar->m_Color == pFrame->m_crColors[i]) bFound = true;
			}
			if(!bFound) 
			{
				pCurWPolar->m_Color = pFrame->m_crColors[i];
				break;
			}
		}
		pCurWPolar->m_bIsVisible = true;

		pWPolar = GetWPolar(pCurWPolar->m_PlrName);
		pFrame->SetSaveState(false);
		if(pWPolar)
		{
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
		
		pFrame->UpdateWPlrs();
		SetWPlr();
		UpdateView();
		pFrame->m_WOperDlgBar.m_ctrlAnalyze.SetFocus();
	}
	else
	{
		delete pCurWPolar;
	}
}


void CMiarex::RotateGeomY(double const &Angle, CVector const &P)
{
	int n, p, pw, nw;
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

		if(m_Panel[p].m_iPos==-1)	m_Panel[p].Normal = TALB * LATB;
		else						m_Panel[p].Normal = LATB * TALB;

		m_Panel[p].Normal.Normalize();
		m_Panel[p].SetFrame(m_Node[iLA], m_Node[iLB], m_Node[iTA], m_Node[iTB]);
	}

	// the wake array is not rotated but translated to remain at the wing's trailing edge
	for (p=0; p< m_MatSize; p++)
	{
		if(m_Panel[p].m_bIsTrailing && (m_Panel[p].m_iPos ==-1 ||m_Panel[p].m_iPos==0))
		{
			Pt = m_Node[m_Panel[p].m_iTA];
			Pt.RotateY(P, Angle);
			//define the translation to be applied to the column's left points
			Trans = m_Node[m_Panel[p].m_iTA] - Pt;
			for(nw=0; nw<m_nWakeNodes; nw++)
			{
				if(abs(m_WakeNode[nw].y-m_Node[m_Panel[p].m_iTA].y)<0.0001 && abs(m_WakeNode[nw].z-m_Node[m_Panel[p].m_iTA].z)<0.0001) 
					m_WakeNode[nw] += Trans;
			}

			Pt = m_Node[m_Panel[p].m_iTB];
			Pt.RotateY(P, Angle);
			//define the translation to be applied to the column's left points
			Trans = m_Node[m_Panel[p].m_iTB] - Pt;
			for(nw=0; nw<m_nWakeNodes; nw++)
			{
				if(abs(m_WakeNode[nw].y-m_Node[m_Panel[p].m_iTB].y)<0.0001 && abs(m_WakeNode[nw].z-m_Node[m_Panel[p].m_iTB].z)<0.0001) 
					m_WakeNode[nw] += Trans;
			}
		}
	}


	//Reset panel frame : CollPt has been translated
	for (pw=0; pw< m_WakeSize; pw++)
	{
		iLA = m_WakePanel[pw].m_iLA; iLB = m_WakePanel[pw].m_iLB;
		iTA = m_WakePanel[pw].m_iTA; iTB = m_WakePanel[pw].m_iTB;
		m_WakePanel[pw].SetFrame(m_WakeNode[iLA], m_WakeNode[iLB], m_WakeNode[iTA], m_WakeNode[iTB]);
	}
}


bool CMiarex::RotateFlap(CWing *pWing, int const &nFlap, double const &Angle, CPanel *pPanel, CVector *pNode)
{
	//nFlap is the identification number of the flap to rotate (wing index)

	if(!pWing) return false;

	int j,k,l,p, iFlap;
	double alpha0;
	Quaternion Quat;
	CVector H, HA, HB, R, S;
	CSurface *pSurf;
	iFlap = 0;
	p     = 0;


	for (j=0; j<pWing->m_NSurfaces; j++)
	{
		pSurf = pWing->m_Surface+j;
		if(pSurf->m_pFoilA && pSurf->m_pFoilB)
		{
			if(iFlap==nFlap)
			{
				//get the approximate initial angle
				if(abs(pSurf->m_pFoilA->m_TEFlapAngle - pSurf->m_pFoilB->m_TEFlapAngle)>0.1) return false;
				alpha0 = (pSurf->m_pFoilA->m_TEFlapAngle + pSurf->m_pFoilB->m_TEFlapAngle)/2.0;
				//create a hinge unit vector
				pSurf->GetPoint(pSurf->m_posATE, pSurf->m_posBTE, 0.0, HA, 0);
				pSurf->GetPoint(pSurf->m_posATE, pSurf->m_posBTE, 1.0, HB, 0);
				H = HB-HA;
				H.Normalize();

				Quat.Set(Angle-alpha0, H);

				for (k=0; k<pSurf->m_nFlapNodes; k++)
				{
					R.x = pNode[pSurf->m_FlapNode[k]].x - HA.x;
					R.y = pNode[pSurf->m_FlapNode[k]].y - HA.y;
					R.z = pNode[pSurf->m_FlapNode[k]].z - HA.z;
					Quat.Conjugate(R,S);

					pNode[pSurf->m_FlapNode[k]].x = S.x + HA.x;
					pNode[pSurf->m_FlapNode[k]].y = S.y + HA.y;
					pNode[pSurf->m_FlapNode[k]].z = S.z + HA.z;
				}

				for(l=0; l<pSurf->m_nFlapPanels; l++)
				{
					m_Panel[pSurf->m_FlapPanel[l]].SetFrame(
						pNode[pPanel[pSurf->m_FlapPanel[l]].m_iLA],
						pNode[pPanel[pSurf->m_FlapPanel[l]].m_iLB],
						pNode[pPanel[pSurf->m_FlapPanel[l]].m_iTA],
						pNode[pPanel[pSurf->m_FlapPanel[l]].m_iTB]);
				}
			}
			else p+= pSurf->m_NYPanels * pSurf->m_NXPanels;

			++iFlap;
		}
		else p+= pSurf->m_NYPanels * pSurf->m_NXPanels;
	}
	return true;
}

















