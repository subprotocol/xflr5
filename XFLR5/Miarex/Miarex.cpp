/****************************************************************************

    Miarex Class
	Copyright (C) 2006 André Deperrois xflr5@yahoo.com

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
#include "../misc/ValueDlg.h"
#include "../Graph/GraphDlg.h"
#include "../Graph/UserVarDlg.h"
#include "ListPlrDlg.h"
#include "../main/ChildView.h"
#include "../main/MainFrm.h"
#include "WAdvDlg.h"
#include "WGraphVarDlg.h"
#include "WingScaleDlg.h"
#include "LLTThread.h"
#include "W3DBar.h"
#include <math.h>
#include <gl\glaux.h>
#include ".\miarex.h"


//Define the references for the OpenGL lists

#define VLMCL				1227
#define TOPTRANS			1228
#define BOTTRANS			1229

#define VLMWINGLIFT			1230
#define VLMWING2LIFT		1231
#define VLMLIFTFORCE		1232
#define VLMMOMENTS			1233
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
#define VLMFLOWS			1256

#define WINGSURFACES		1258
#define WINGOUTLINE			1259
#define WING2SURFACES		1260
#define WING2OUTLINE		1261

#define STABSURFACES		1265
#define STABOUTLINE			1266

#define FINSURFACES 		1270
#define FINOUTLINE			1271

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

#define AXES				1301

#define VLMPANELS			1302

 
/////////////////////////////////////////////////////////////////////////////
// CMiarex dialog

CMiarex::~CMiarex()
{
	//destroy evrything
	if(glIsList(VLMCL)) {
		glDeleteLists(VLMCL,3);
		m_GLList-=3;
	}	
	if(glIsList(VLMWINGLIFT)) {
		glDeleteLists(VLMWINGLIFT,2);
		m_GLList-=2;
	}	
	if(glIsList(VLMWING2LIFT)) {
		glDeleteLists(VLMWING2LIFT,2);
		m_GLList-=2;
	}	
	if(glIsList(VLMSTABLIFT)) {
		glDeleteLists(VLMSTABLIFT,1);
		m_GLList-=1;
	}	
	if(glIsList(VLMFINLIFT)) {
		glDeleteLists(VLMFINLIFT,1);
		m_GLList-=1;
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
	if(glIsList(AXES)) {
		glDeleteLists(AXES,1);
		m_GLList-=1;
	}
	if(glIsList(WINGSURFACES)) {
		glDeleteLists(WINGSURFACES,2);
		m_GLList-=3;
	}
	if(glIsList(WING2SURFACES)) {
		glDeleteLists(WING2SURFACES,2);
		m_GLList-=3;
	}
	if(glIsList(STABSURFACES)) {
		glDeleteLists(STABSURFACES,2);
		m_GLList-=3;
	}
	if(glIsList(FINSURFACES)) {
		glDeleteLists(FINSURFACES,2);
		m_GLList-=3;
	}
	if(glIsList(VLMPANELS)) {
		glDeleteLists(VLMPANELS,1);
		m_GLList-=1;
	}
	if(glIsList(WINGWAKEPANELS)) {
		glDeleteLists(WINGWAKEPANELS,1);
		m_GLList-=1;
	}
	if(glIsList(VLMSTREAMLINES)) {
		glDeleteLists(VLMSTREAMLINES,1);
		m_GLList-=1;
	}
	if(glIsList(VLMFLOWS)) {
		glDeleteLists(VLMFLOWS,1);
		m_GLList-=1;
	}
	if(glIsList(WOPPCPLEGEND)) {
		glDeleteLists(WOPPCPLEGEND,1);
		m_GLList-=1;
	}
	if(glIsList(WOPPLEGEND)) {
		glDeleteLists(WOPPLEGEND,1);
		m_GLList-=1;
	}
	if(glIsList(WINGLEGEND)) {
		glDeleteLists(WINGLEGEND,1);
		m_GLList-=1;
	}
}


CMiarex::CMiarex(CWnd* pParent /*=NULL*/)
{
	//construct evrything
	pi = 3.141592654;

	m_hcArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_hcCross = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
	m_hcMove  = AfxGetApp()->LoadCursor(IDC_HMOVE);

	m_pFrame     = NULL;
	m_pChildWnd  = NULL;
	m_pXFile     = NULL;

	m_pCurPlane = NULL;
	m_pCurWing2 = NULL;
	m_pCurStab  = NULL;
	m_pCurFin   = NULL;
	m_pCurPOpp  = NULL;

	m_NSurfaces =   0;
	m_NStation  =  20;
	m_Iter      = 100;
	m_CvPrec    =   0.01;
	m_Relax     =  20.0;

	m_Panel[0].m_VortexPos = 0.25;
	m_Panel[0].m_CtrlPos   = 0.75;
	m_WakePanelFactor = 1.10;
	m_FirstPanelSize  = 1.00;
	m_CoreSize        = 0.0;
	m_NXWakePanels    = 1;
	m_MaxWakeIter     = 5;
	m_WakeInterNodes  = 6;
	m_bResetWake      = true;
	m_bSetNewWake     = true;
	m_bVLMFinished    = true;

	m_nWakeNodes = 0;
	m_WakeSize   = 0;
	m_PointDown.SetPoint(0,0);
	m_WPlrLegendOffset.SetPoint(0,0);
	m_WingLegendOffset.SetPoint(0,0);
	m_rCltRect.SetRectEmpty();
	m_rDrawRect.SetRectEmpty();

	m_MinPanelSize = 0.001;//m

	m_LastWOpp   = 0.0;
	m_CurSpanPos = 0.0;
	m_WingScale  = 1.0;
	m_ptOffset.x = 0;
	m_ptOffset.y = 0;

	m_GLList = 0;
	m_bResetglGeom      = true;
	m_bResetglOpp       = true;
	m_bResetglLift      = true;
	m_bResetglDrag      = true;
	m_bResetglDownwash  = true;
	m_bResetglStream    = true;
	m_bResetglMesh      = true;
	m_bResetglWake      = true;
	m_bResetglFlow      = true;

	m_bShowLight        = false;
	m_bLogFile          = false;
	m_bHalfWing         = true;
	m_bTransGraph       = true;
	m_bIsPrinting       = false;
	m_bCurWOppOnly      = true;
	m_bStoreWOpp        = true;
	m_bKeepOutOpps      = false;
	m_bCheckPlanePanels = true;
	m_bTrans            = false;
	m_bType1            = true;
	m_bType2            = true;
	m_bType4            = true;
	m_bShowElliptic     = false;
	m_bShowWing2        = true;
	m_bShowStab         = true;
	m_bShowFin          = true;
	m_bIs2DScaleSet     = false;
	m_bIs3DScaleSet     = false;
	m_bAutoScales       = false;
	m_bAnimate          = false;
	m_bAnimatePlus      = true;
	m_posAnimate        = 0;

	m_pCurWPolar  = NULL;
	m_pCurWing    = NULL;
	m_pCurWOpp    = NULL;
	m_pCurGraph   = NULL;

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
	m_CpGraph.SetXTitle("x/c");
	m_CpGraph.SetYTitle("Cp");
	m_CpGraph.SetXMin(-0.0);
	m_CpGraph.SetXMax( 0.1);
	m_CpGraph.SetYMin(-0.01);
	m_CpGraph.SetYMax( 0.01);
	m_CpGraph.SetType(0);

	m_WOppVar1 = 0;
	m_WOppVar2 = 1;
	m_WOppVar3 = 2;
	m_WOppVar4 = 3;

//	m_Flow = 2;//1 is Cross Flow, 2 is FlowLines;

	m_XW1    =  4;
	m_YW1    =  1;
	m_XW2    =  0;
	m_YW2    =  1;
	m_XW3    =  0;
	m_YW3    = 12;
	m_XW4    =  0;
	m_YW4    = 11;

	m_iView     = 1;
	m_iWingView = 1;
	m_iWPlrView = 4;

	m_nNodes     = 0;
	m_VLMMatSize = 0;
//	m_NSurfaces  = 0;

	m_WingColor =  RGB(0,130,130);
	m_StabColor = 12477630;
	m_FinColor  =  RGB(60,60,180);

	m_VLMDlg.m_pRefPanels     = m_Panel;
	m_VLMDlg.m_pRefNodes      = m_Node;
	m_VLMDlg.m_pRefSurfaces   = m_Surface;

	m_VLMDlg.m_pWakeNode      = m_WakeNode;
	m_VLMDlg.m_pWakePanel     = m_WakePanel;

	m_PanelDlg.m_pPanel     = m_Panel;
	m_PanelDlg.m_pNode      = m_Node;
	m_PanelDlg.m_pWakePanel = m_WakePanel;
	m_PanelDlg.m_pWakeNode  = m_WakeNode;

	m_PanelDlg.m_pSurface   = m_Surface;

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
}


BEGIN_MESSAGE_MAP(CMiarex, CWnd)

	ON_COMMAND(IDM_RESET3DVIEW, OnReset3DView)
	ON_COMMAND(IDM_SINGLEGRAPH1, OnSingleGraph1)
	ON_COMMAND(IDM_SINGLEGRAPH2, OnSingleGraph2)
	ON_COMMAND(IDM_SINGLEGRAPH3, OnSingleGraph3)
	ON_COMMAND(IDM_SINGLEGRAPH4, OnSingleGraph4)
	ON_COMMAND(IDM_DEFINEPLANE, OnDefinePlane)
//	ON_COMMAND(IDM_EDITPLANE, OnEditPlane)
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
//	ON_COMMAND(IDM_EXIT, OnExit)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(IDM_WANALYSIS, OnWingAnalysis)
	ON_COMMAND(IDM_SHOWPOLAR, OnShowWPolar)
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
//	ON_COMMAND(IDM_SHOWPOINTS, OnShowPoints)
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
//	ON_COMMAND(IDM_SINGLEWINGGRAPH, OnSingleWingGraph)
	ON_COMMAND(IDM_FOURWINGGRAPHS, OnFourWingGraphs)
	ON_COMMAND(IDM_TWOWINGGRAPHS, OnTwoWingGraphs)
	ON_COMMAND(IDM_SHOWCURWOPP, OnShowCurWOpp)
	ON_COMMAND(IDM_SHOWALLWOPPS, OnShowAllWOpps)
	ON_COMMAND(IDM_DELALLPLRWOPPS, OnDelAllPlrWOpps)
	ON_COMMAND(IDM_ADVSETTINGS, OnAdvSettings)
	ON_COMMAND(IDM_SHOWXCMREF, OnShowXCmRef)
	ON_COMMAND(IDM_HIDEALLWPLRS, OnHideAllWPlrs)
	ON_COMMAND(IDM_SHOWALLWPLRS, OnShowAllWPlrs)
	ON_COMMAND(IDM_WNGHIDEALL, OnWngHideAll)
	ON_COMMAND(IDM_WNGSHOWALL, OnWngShowAll)
	ON_COMMAND(IDM_IMPORTWING, OnImportWing)
	ON_COMMAND(IDM_SHOWELLIPTIC, OnShowElliptic)
	ON_COMMAND(IDM_RESETWINGSCALE, OnResetWingScale)
	ON_COMMAND(IDM_3DVIEW, On3DView)
	ON_COMMAND(IDM_SCALEWING, OnScaleWing)
	ON_COMMAND(IDM_RESETWINGLEGEND, OnResetWingLegend)
	ON_COMMAND(IDM_RESETLEGEND, OnResetLegend)
	ON_COMMAND(IDM_RESETALLSCALES, OnResetAllScales)
	ON_COMMAND(IDM_HIDEALLWOPPS, OnHideAllWOpps)
	ON_COMMAND(IDM_HIDEWOPPS, OnHideWOpps)
	ON_COMMAND(IDM_SHOWWOPPS, OnShowWOpps)
	ON_COMMAND(IDM_DELALLWOPPS, OnDelAllWOpps)
	ON_COMMAND(IDM_HIDEWINGOPPS, OnHideWingOpps)
//	ON_COMMAND(IDT_SAVE, OnSaveProject)
	ON_COMMAND(IDT_WOPP, OnWOpp)
	ON_COMMAND(IDT_WPOLARS, OnShowWPolar)
	ON_COMMAND(IDM_SAVEPROJECTAS, OnSaveAs)
	ON_COMMAND(IDM_LOADPROJECT, OnLoadProject)
	ON_COMMAND(IDM_SHOWWINGOPPS, OnShowWingOpps)
//	ON_COMMAND(IDM_XFLOW, OnStreamLines)
	ON_COMMAND(IDM_GLLIGHT, OnGLLight)
	ON_COMMAND(IDM_STREAMOPTIONS, OnStreamOptions)
	ON_COMMAND(IDM_MANAGEWINGS, OnManageUFO)
	ON_COMMAND(IDM_EXPORTVLMPANELS, OnExportPanels)
	ON_COMMAND(IDM_CPVIEW, OnCpView)
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
	bool bExists   = false;
	bool bInserted = false;
	CWing *pOldWing;
	CString strong;
	
	if(pWing->m_WingName.GetLength()){
		for (int i=0; i<m_poaWing->GetSize(); i++){
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
			for (int j=0; j<m_poaWing->GetSize(); j++){
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
 	bool bExists   = false;
 	bool bInserted = false;
 	CWPolar *pOldWPlr;
 	CWPolar *pOld2WPlr;
 	CString strong;

 	for (int i=0; i<m_poaWPolar->GetSize(); i++){
 		pOldWPlr = (CWPolar*)m_poaWPolar->GetAt(i);
 		if (pOldWPlr->m_PlrName == pWPolar->m_PlrName &&
			pOldWPlr->m_UFOName == pWPolar->m_UFOName) {
			bExists = true;
			break;
		}
 	}

 	while(!bInserted){
 		if(!bExists){ 
 			for (int j=0; j<m_poaWPolar->GetSize(); j++){
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
	if(m_pCurPlane) UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return NULL;
  	for (int i=0; i<m_poaWPolar->GetSize(); i++){
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
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CWPolar *pWPolar = NULL;
	CWPolar *pOldWPolar = NULL;
	CString UFOName;
	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return;

	if(bCurrent) pWPolar = m_pCurWPolar;
	else {
  		for (int i=0; i<m_poaWPolar->GetSize(); i++){
 			pOldWPolar = (CWPolar*) m_poaWPolar->GetAt(i);
 			if (pOldWPolar->m_UFOName == UFOName &&
				pOldWPolar->m_PlrName == WPlrName) {
				pWPolar = pOldWPolar;
				break;
			}
		}
	}
	if(pWPolar && pWPolar == m_pCurWPolar) {
		SetWOpp(true);
	}
	else {
		m_pCurPOpp = NULL;
		m_pCurWOpp = NULL;
		m_bResetglOpp  = true;
		m_bResetglMesh = true;
		m_bResetglWake = true;
	
		if(!m_pCurWing) {
			pWPolar = NULL;
			m_pCurWPolar = NULL;
			pFrame->m_WOperDlgBar.EnableAnalysis(false);
		}
		if(pWPolar) m_pCurWPolar = pWPolar;
		else if(!m_pCurWPolar && m_pCurWing){// try to set the first in the list, if any
			CString strong;
			if (pFrame->m_ctrlWPlr.GetCount()){
				pFrame->m_ctrlWPlr.GetLBText(0, strong);
				m_pCurWPolar = GetWPolar(strong);
			}
			else {
				m_pCurWPolar = NULL;
				pFrame->m_WOperDlgBar.EnableAnalysis(false);
			}
		}
	}
	if(m_pCurWPolar && m_pCurWPolar->m_UFOName==UFOName){
		m_pCurWing->ComputeChords();
		m_pCurWing->m_Type         = m_pCurWPolar->m_Type;
		m_pCurWing->m_Alpha        = m_pCurWPolar->m_ASpec;//in case its a Type 4 polar
		m_pCurWing->m_QInf         = m_pCurWPolar->m_QInf;
		m_pCurWing->m_Weight       = m_pCurWPolar->m_Weight;
		m_pCurWing->m_XCmRef       = m_pCurWPolar->m_XCmRef;
		m_pCurWing->m_Density      = m_pCurWPolar->m_Density;
		m_pCurWing->m_Viscosity    = m_pCurWPolar->m_Viscosity;
		m_pCurWing->m_bVLM1        = m_pCurWPolar->m_bVLM1;
		m_pCurWing->m_bMiddle      = m_pCurWPolar->m_bMiddle;
		m_pCurWing->m_bVLMSymetric = m_pCurWing->m_bSymetric;

		if(m_pCurWing2) {
			m_pCurWing2->ComputeChords();
			m_pCurWing2->m_Type         = m_pCurWPolar->m_Type;
			m_pCurWing2->m_Alpha        = m_pCurWPolar->m_ASpec;//in case its a Type 4 polar
			m_pCurWing2->m_QInf         = m_pCurWPolar->m_QInf;
			m_pCurWing2->m_Weight       = m_pCurWPolar->m_Weight;
			m_pCurWing2->m_XCmRef       = m_pCurWPolar->m_XCmRef;
			m_pCurWing2->m_Density      = m_pCurWPolar->m_Density;
			m_pCurWing2->m_Viscosity    = m_pCurWPolar->m_Viscosity;
			m_pCurWing2->m_bVLM1        = m_pCurWPolar->m_bVLM1;
			m_pCurWing2->m_bMiddle      = m_pCurWPolar->m_bMiddle;
			m_pCurWing2->m_bVLMSymetric = m_pCurWing2->m_bSymetric;
		}

		if(m_pCurStab) {
			m_pCurStab->ComputeChords();
			m_pCurStab->m_Type         = m_pCurWPolar->m_Type;
			m_pCurStab->m_Alpha        = m_pCurWPolar->m_ASpec;//in case its a Type 4 polar
			m_pCurStab->m_QInf         = m_pCurWPolar->m_QInf;
			m_pCurStab->m_Weight       = m_pCurWPolar->m_Weight;
			m_pCurStab->m_XCmRef       = m_pCurWPolar->m_XCmRef;
			m_pCurStab->m_Density      = m_pCurWPolar->m_Density;
			m_pCurStab->m_Viscosity    = m_pCurWPolar->m_Viscosity;
			m_pCurStab->m_bVLM1        = m_pCurWPolar->m_bVLM1;
			m_pCurStab->m_bMiddle      = m_pCurWPolar->m_bMiddle;
			m_pCurStab->m_bVLMSymetric = m_pCurStab->m_bSymetric;
		}

		if(m_pCurFin) {
			m_pCurFin->ComputeChords();
			m_pCurFin->m_Type         = m_pCurWPolar->m_Type;
			m_pCurFin->m_Alpha        = m_pCurWPolar->m_ASpec;//in case its a Type 4 polar
			m_pCurFin->m_QInf         = m_pCurWPolar->m_QInf;
			m_pCurFin->m_Weight       = m_pCurWPolar->m_Weight;
			m_pCurFin->m_XCmRef       = m_pCurWPolar->m_XCmRef;
			m_pCurFin->m_Density      = m_pCurWPolar->m_Density;
			m_pCurFin->m_Viscosity    = m_pCurWPolar->m_Viscosity;
			m_pCurFin->m_bVLM1        = m_pCurWPolar->m_bVLM1;
			m_pCurFin->m_bMiddle      = m_pCurWPolar->m_bMiddle;
			m_pCurFin->m_bVLMSymetric = m_pCurFin->m_bSymetric;
		}

		if(m_pCurWPolar->m_AnalysisType==1){
			m_pCurWing->m_NStation  = m_NStation;
			m_pCurWing->m_bLLT      = true;
		}

		pFrame->m_WOperDlgBar.EnableAnalysis(true);
		UpdateWOpps();
		if (m_pCurWOpp){
			// try to set the same as the existing WOpp... Special for Marc
			if(!SetWOpp(false, m_pCurWOpp->m_Alpha))
				SetWOpp(true);// try again from scratch
		}
		else 
			SetWOpp(true);

//		if(m_pCurWPolar->m_bWakeRollUp && !m_pCurWOpp) CreateWakeElems(m_pCurWOpp);
	}
	else {
		m_pCurWPolar = NULL;
		m_pCurPOpp = NULL;
		m_pCurWOpp = NULL;
		pFrame->m_WOperDlgBar.EnableAnalysis(false);
	}

	VLMInitializePanels();


	memcpy(m_VLMDlg.m_pPanel, m_VLMDlg.m_pRefPanels,      sizeof(m_VLMDlg.m_pPanel));
	memcpy(m_VLMDlg.m_pNode,  m_VLMDlg.m_pRefNodes,       sizeof(m_VLMDlg.m_pNode));
	for (int l=0; l<m_VLMDlg.m_NSurfaces; l++)
		m_VLMDlg.m_pSurface[l].Copy(m_VLMDlg.m_pRefSurfaces[l]);

	if(m_pCurWing){
		m_pCurWing->m_CvPrec    = m_CvPrec;
		m_pCurWing->m_RelaxMax  = m_Relax;
	}

	pFrame->m_WOperDlgBar.SetParams(m_pCurWPolar);
	pFrame->m_PolarDlgBar.SetParams();
	CreateWPolarCurves();
	m_VLMDlg.m_pWPolar  = m_pCurWPolar;
}


void CMiarex::UpdateUFOs()
{
	//Update the UFO Combobox with the names in the array's list
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pFrame->UpdateUFOs();
}

void CMiarex::UpdateWPlrs()
{
	//Update the WPolar's Combobox with the names in the array's list
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pFrame->UpdateWPlrs();
}

void CMiarex::UpdateWOpps()
{
	//Update the WOpp's Combobox with the names in the array's list
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pFrame->UpdateWOpps();
}

void CMiarex::AddWOpp(bool bPointOut, double *Gamma)
{
	// creates a new WOpp, and if successful
	// fills the WOpp's variables with the current analysis results
	// adds the WOpp to the array
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	int j;
	if(!m_bKeepOutOpps && bPointOut) return;

	CWOpp *pWOpp;	
	CPOpp *pPOpp;	
	CWOpp * pNewPoint;
	pNewPoint = new CWOpp();
	if(pNewPoint == NULL) {
		AfxMessageBox("Not enough memory to store the OpPoint\n", MB_OK);
		return;
	}
	else{//load WOpp with data
		pNewPoint->m_Color = pFrame->GetColor(5);
		bool bFound;
		for(int i=0; i<30;i++){
			bFound = false;
			for (j=0; j<m_poaWOpp->GetSize();j++){
				pWOpp = (CWOpp*)m_poaWOpp->GetAt(j);
				if(pWOpp->m_Color == pFrame->m_crColors[i]) bFound = true;
			}
			for (j=0; j<m_poaPOpp->GetSize();j++){
				pPOpp = (CPOpp*)m_poaPOpp->GetAt(j);
				if(pPOpp->m_Color == pFrame->m_crColors[i]) bFound = true;
			}
			if(!bFound) {
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
		pNewPoint->m_bMiddle             = m_pCurWPolar->m_bMiddle;
		pNewPoint->m_Type                = m_pCurWPolar->m_Type;
		pNewPoint->m_Weight              = m_pCurWPolar->m_Weight;


		double Cb =0.0;

		if(m_pCurWPolar->m_AnalysisType==1){
			pNewPoint->m_Alpha               = m_pCurWing->m_Alpha;
			pNewPoint->m_QInf                = m_pCurWing->m_QInf;
			pNewPoint->m_NVLMPanels          = m_pCurWing->m_VLMMatSize;
			pNewPoint->m_bOut                = m_pCurWing->m_bWingOut;
			pNewPoint->m_CL                  = m_pCurWing->m_CL;
			pNewPoint->m_InducedDrag         = m_pCurWing->m_InducedDrag;
			pNewPoint->m_VCm                 = m_pCurWing->m_VCm;
			pNewPoint->m_GCm                 = m_pCurWing->m_GCm;
			pNewPoint->m_PitchingMoment      = m_pCurWing->m_PitchingMoment;
			pNewPoint->m_InducedYawingMoment = m_pCurWing->m_IYm;
			pNewPoint->m_XCP                 = m_pCurWing->m_XCP;
			pNewPoint->m_YCP                 = m_pCurWing->m_YCP;
			pNewPoint->m_ViscousDrag         = m_pCurWing->m_ViscousDrag;
			pNewPoint->m_RollingMoment       = m_pCurWing->m_RollingMoment;
			pNewPoint->m_GYm                 = m_pCurWing->m_GYm;

			for (int l=1; l<m_pCurWing->m_NStation; l++){
				pNewPoint->m_SpanPos[l] = -m_pCurWing->m_SpanPos[l];
				pNewPoint->m_Ai[l]      =  m_pCurWing->m_Ai[m_NStation-l];
				pNewPoint->m_Cl[l]      = m_pCurWing->m_Cl[m_NStation-l];
				pNewPoint->m_PCd[l]     = m_pCurWing->m_PCd[m_NStation-l];
				pNewPoint->m_ICd[l]     = m_pCurWing->m_ICd[m_NStation-l];
				pNewPoint->m_Cm[l]      = m_pCurWing->m_Cm[m_NStation-l];
				pNewPoint->m_CmAirf[l]  = m_pCurWing->m_CmAirf[m_NStation-l];
				pNewPoint->m_CmGeom[l]  = m_pCurWing->m_CmGeom[m_NStation-l];
				pNewPoint->m_XCPSpanRel[l] = m_pCurWing->m_XCPSpanRel[m_NStation-l];
				pNewPoint->m_XCPSpanAbs[l] = m_pCurWing->m_XCPSpanAbs[m_NStation-l];
				pNewPoint->m_Re[l]      = m_pCurWing->m_Re[m_NStation-l];
				pNewPoint->m_Chord[l]   = m_pCurWing->m_Chord[m_NStation-l];
				pNewPoint->m_Twist[l]   = m_pCurWing->m_Twist[m_NStation-l];
				pNewPoint->m_XTrTop[l]  = m_pCurWing->m_XTrTop[m_NStation-l];
				pNewPoint->m_XTrBot[l]  = m_pCurWing->m_XTrBot[m_NStation-l];
				pNewPoint->m_BendingMoment[l] = m_pCurWing->m_BendingMoment[m_NStation-l];
				memset(pNewPoint->m_Cp, 0, sizeof(pNewPoint->m_Cp));
				if(abs(m_pCurWing->m_BendingMoment[l])>abs(Cb))	Cb = m_pCurWing->m_BendingMoment[l];
			}
		}
		else if(m_pCurWPolar->m_AnalysisType==2){
			pNewPoint->m_Alpha               = m_VLMDlg.m_AlphaCalc;
			pNewPoint->m_QInf                = m_VLMDlg.m_QInf;
			pNewPoint->m_NVLMPanels          = m_VLMDlg.m_VLMMatSize;
			pNewPoint->m_bOut                = m_VLMDlg.m_bPointOut;
			pNewPoint->m_CL                  = m_VLMDlg.m_CL;
			pNewPoint->m_InducedDrag         = m_VLMDlg.m_InducedDrag;
			pNewPoint->m_ViscousDrag         = m_VLMDlg.m_ViscousDrag;
			pNewPoint->m_RollingMoment       = m_VLMDlg.m_Rm;
			pNewPoint->m_GYm                 = m_VLMDlg.m_GYm;
			pNewPoint->m_VCm                 = m_VLMDlg.m_VCm;
			pNewPoint->m_GCm                 = m_VLMDlg.m_GCm;
			pNewPoint->m_PitchingMoment      = m_VLMDlg.m_TCm;
			pNewPoint->m_InducedYawingMoment = m_VLMDlg.m_IYm;
			pNewPoint->m_XCP                 = m_VLMDlg.m_XCP;
			pNewPoint->m_YCP                 = m_VLMDlg.m_YCP;
			for (int l=0; l<m_pCurWing->m_NStation; l++){
				pNewPoint->m_SpanPos[l] = m_pCurWing->m_SpanPos[l];
				if(abs(m_pCurWing->m_BendingMoment[l])>abs(Cb))	Cb = m_pCurWing->m_BendingMoment[l];
			}
			memcpy(pNewPoint->m_Cl, m_pCurWing->m_Cl, sizeof(m_pCurWing->m_Cl));
			memcpy(pNewPoint->m_PCd, m_pCurWing->m_PCd, sizeof(m_pCurWing->m_PCd));
			memcpy(pNewPoint->m_Cm, m_pCurWing->m_Cm, sizeof(m_pCurWing->m_Cm));
			memcpy(pNewPoint->m_CmAirf, m_pCurWing->m_CmAirf, sizeof(m_pCurWing->m_CmAirf));
			memcpy(pNewPoint->m_CmGeom, m_pCurWing->m_CmGeom, sizeof(m_pCurWing->m_CmGeom));
			memcpy(pNewPoint->m_XCPSpanRel, m_pCurWing->m_XCPSpanRel, sizeof(m_pCurWing->m_XCPSpanRel));
			memcpy(pNewPoint->m_XCPSpanAbs, m_pCurWing->m_XCPSpanAbs, sizeof(m_pCurWing->m_XCPSpanAbs));
			memcpy(pNewPoint->m_Re, m_pCurWing->m_Re, sizeof(m_pCurWing->m_Re));
			memcpy(pNewPoint->m_Chord, m_pCurWing->m_Chord, sizeof(m_pCurWing->m_Chord));
			memcpy(pNewPoint->m_Twist, m_pCurWing->m_Twist, sizeof(m_pCurWing->m_Twist));
			memcpy(pNewPoint->m_XTrTop, m_pCurWing->m_XTrTop, sizeof(m_pCurWing->m_XTrTop));
			memcpy(pNewPoint->m_XTrBot, m_pCurWing->m_XTrBot, sizeof(m_pCurWing->m_XTrBot));
			memcpy(pNewPoint->m_BendingMoment, m_pCurWing->m_BendingMoment, sizeof(m_pCurWing->m_BendingMoment));
			memcpy(pNewPoint->m_Vd, m_pCurWing->m_Vd, sizeof(pNewPoint->m_Vd));

			memcpy(pNewPoint->m_F, m_pCurWing->m_F,  sizeof(pNewPoint->m_F));
			memcpy(pNewPoint->m_Ai, m_VLMDlg.m_Ai,   sizeof(pNewPoint->m_Ai));
			memcpy(pNewPoint->m_ICd, m_VLMDlg.m_ICd, sizeof(pNewPoint->m_ICd));
			memcpy(pNewPoint->m_Cp, m_VLMDlg.m_Cp,   sizeof(pNewPoint->m_Cp));

			memcpy(pNewPoint->m_G, Gamma, sizeof(pNewPoint->m_G));

			if(m_pCurWPolar->m_bWakeRollUp){
				pNewPoint->m_nWakeNodes     = m_nWakeNodes;
				pNewPoint->m_NXWakePanels   = m_NXWakePanels;
				pNewPoint->m_FirstWakePanel = m_FirstPanelSize;
				pNewPoint->m_WakeFactor     = m_WakePanelFactor;
			}
			for (i=0; i<m_pCurWing->m_nFlaps; i++){
				pNewPoint->m_FlapMoment[i] = m_pCurWing->m_FlapMoment[i];
			}
			pNewPoint->m_nFlaps = m_pCurWing->m_nFlaps;
		}
		else if(m_pCurWPolar->m_AnalysisType==3){
			pNewPoint->m_Alpha               = m_PanelDlg.m_AlphaCalc;
			pNewPoint->m_QInf                = m_PanelDlg.m_QInf;
			pNewPoint->m_NVLMPanels          = m_PanelDlg.m_MatSize;
			pNewPoint->m_bOut                = m_PanelDlg.m_bPointOut;
			pNewPoint->m_CL                  = m_PanelDlg.m_CL;
			pNewPoint->m_InducedDrag         = m_PanelDlg.m_InducedDrag;
			pNewPoint->m_ViscousDrag         = m_PanelDlg.m_ViscousDrag;
			pNewPoint->m_RollingMoment       = m_PanelDlg.m_Rm;
			pNewPoint->m_GYm                 = m_PanelDlg.m_GYm;
			pNewPoint->m_VCm                 = m_PanelDlg.m_VCm;
			pNewPoint->m_GCm                 = m_PanelDlg.m_GCm;
			pNewPoint->m_PitchingMoment      = m_PanelDlg.m_TCm;
			pNewPoint->m_InducedYawingMoment = m_PanelDlg.m_IYm;
			pNewPoint->m_XCP                 = m_PanelDlg.m_XCP;
			pNewPoint->m_YCP                 = m_PanelDlg.m_YCP;
			for (int l=0; l<m_pCurWing->m_NStation; l++){
				pNewPoint->m_SpanPos[l] = m_pCurWing->m_SpanPos[l];
				if(abs(m_pCurWing->m_BendingMoment[l])>abs(Cb))	Cb = m_pCurWing->m_BendingMoment[l];
			}
			memcpy(pNewPoint->m_Cl, m_pCurWing->m_Cl, sizeof(m_pCurWing->m_Cl));
			memcpy(pNewPoint->m_PCd, m_pCurWing->m_PCd, sizeof(m_pCurWing->m_PCd));
			memcpy(pNewPoint->m_Cm, m_pCurWing->m_Cm, sizeof(m_pCurWing->m_Cm));
			memcpy(pNewPoint->m_CmAirf, m_pCurWing->m_CmAirf, sizeof(m_pCurWing->m_CmAirf));
			memcpy(pNewPoint->m_CmGeom, m_pCurWing->m_CmGeom, sizeof(m_pCurWing->m_CmGeom));
			memcpy(pNewPoint->m_XCPSpanRel, m_pCurWing->m_XCPSpanRel, sizeof(m_pCurWing->m_XCPSpanRel));
			memcpy(pNewPoint->m_XCPSpanAbs, m_pCurWing->m_XCPSpanAbs, sizeof(m_pCurWing->m_XCPSpanAbs));
			memcpy(pNewPoint->m_Re, m_pCurWing->m_Re, sizeof(m_pCurWing->m_Re));
			memcpy(pNewPoint->m_Chord, m_pCurWing->m_Chord, sizeof(m_pCurWing->m_Chord));
			memcpy(pNewPoint->m_Twist, m_pCurWing->m_Twist, sizeof(m_pCurWing->m_Twist));
			memcpy(pNewPoint->m_XTrTop, m_pCurWing->m_XTrTop, sizeof(m_pCurWing->m_XTrTop));
			memcpy(pNewPoint->m_XTrBot, m_pCurWing->m_XTrBot, sizeof(m_pCurWing->m_XTrBot));
			memcpy(pNewPoint->m_BendingMoment, m_pCurWing->m_BendingMoment, sizeof(m_pCurWing->m_BendingMoment));
			memcpy(pNewPoint->m_Vd, m_pCurWing->m_Vd, sizeof(pNewPoint->m_Vd));

			memcpy(pNewPoint->m_F, m_pCurWing->m_F,  sizeof(pNewPoint->m_F));
			memcpy(pNewPoint->m_Ai, m_PanelDlg.m_Ai,   sizeof(pNewPoint->m_Ai));
			memcpy(pNewPoint->m_ICd, m_PanelDlg.m_ICd, sizeof(pNewPoint->m_ICd));
			memcpy(pNewPoint->m_Cp, m_PanelDlg.m_Cp,   sizeof(pNewPoint->m_Cp));

			memcpy(pNewPoint->m_G, m_PanelDlg.m_Mu, sizeof(pNewPoint->m_G));
			memcpy(pNewPoint->m_Sigma, m_PanelDlg.m_Sigma, sizeof(pNewPoint->m_G));

			if(m_pCurWPolar->m_bWakeRollUp){
				pNewPoint->m_nWakeNodes     = m_nWakeNodes;
				pNewPoint->m_NXWakePanels   = m_NXWakePanels;
				pNewPoint->m_FirstWakePanel = m_FirstPanelSize;
				pNewPoint->m_WakeFactor     = m_WakePanelFactor;
			}
		}
		pNewPoint->m_MaxBending = Cb;
	}

	m_pCurWPolar->AddPoint(pNewPoint);
	if(m_iView==2) {
		CreateWPolarCurves();
		UpdateView();
	}

	pFrame->SetSaveState(false);

	bool bIsInserted = false;

	// add the WOpPoint to the WOpPoint Array for the current WingName
	if(m_bStoreWOpp){
		for (int i=0; i<m_poaWOpp->GetSize(); i++){
			pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
			if (pNewPoint->m_WingName == pWOpp->m_WingName){
				if (pNewPoint->m_PlrName == pWOpp->m_PlrName){

					if(pNewPoint->m_Type !=4){
						if(fabs(pNewPoint->m_Alpha - pWOpp->m_Alpha)<0.005){
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
					else{
						if(fabs(pNewPoint->m_QInf - pWOpp->m_QInf)<0.005){
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
	else {
		delete pNewPoint;
	}
	Trace("Added WOpp");

}

void CMiarex::AddWOpp(CWOpp *pNewPoint)
{
	// loads a WOpPoint from a file	
	// adds it to the WOpp array

	bool bIsInserted = false;
	CWOpp* pWOpp;

	// add the WOpPoint to the WOpPoint Array for the current WingName
	for (int i=0; i<m_poaWOpp->GetSize(); i++){
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		if (pNewPoint->m_WingName == pWOpp->m_WingName){
			if (pNewPoint->m_PlrName == pWOpp->m_PlrName){

				if(pNewPoint->m_Type !=4){
					if(fabs(pNewPoint->m_Alpha - pWOpp->m_Alpha)<0.005f){
						//replace existing point
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
				else{
					if(fabs(pNewPoint->m_QInf - pWOpp->m_QInf)<0.005f){
						//replace existing point
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
} 


CWOpp* CMiarex::GetWOpp(double Alpha)
{
	// returns a pointer to the WOpp corresponding to aoa Alpha,
	// and with the name of the current wing and current WPolar
	if(!m_pCurWing || !m_pCurWPolar) return NULL;

	CWOpp* pWOpp;

	for (int i=0; i<m_poaWOpp->GetSize(); i++){
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		if ((pWOpp->m_WingName == m_pCurWing->m_WingName) &&
			(pWOpp->m_PlrName == m_pCurWPolar->m_PlrName)){
			if(m_pCurWPolar->m_Type!=4 && fabs(pWOpp->m_Alpha -Alpha)<0.001)
				return pWOpp;
			if(m_pCurWPolar->m_Type==4 && fabs(pWOpp->m_QInf - Alpha)<0.001)
				return pWOpp;
		}
	}
	return NULL;
}

CPOpp* CMiarex::GetPOpp(double Alpha)
{
	// returns a pointer to the WOpp corresponding to aoa Alpha,
	// and with the name of the current plane and current WPolar

	if(!m_pCurPlane || !m_pCurWPolar) return NULL;

	CPOpp* pPOpp;

	for (int i=0; i<m_poaPOpp->GetSize(); i++){
		pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
		if ((pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName) &&
			(pPOpp->m_PlrName   == m_pCurWPolar->m_PlrName)){
			if(m_pCurWPolar->m_Type!=4 && fabs(pPOpp->m_Alpha -Alpha)<0.001)
				return pPOpp;
			if(m_pCurWPolar->m_Type==4 && fabs(pPOpp->m_QInf - Alpha)<0.001)
				return pPOpp;
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

	if(!pPOpp) {
		if(m_pCurWPolar)
			pPOpp = GetPOpp(m_pCurWPolar->m_AMem);
		else 
			pPOpp = GetPOpp(m_LastWOpp);
	}

	if(!pPOpp) pPOpp = m_pCurPOpp;
	

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
	}

	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();
	if(!m_pCurPOpp) return false;
	else            return true;
}


bool CMiarex::SetWOpp(bool bCurrent, double Alpha)
{
	// set the WOpp, if valid
	// else set the current WOpp, if any
	// else set the comboBox's first, if any
	// else set it to NULL
	if(!m_pCurWing) return false;
	if(m_pCurPlane)	return SetPOpp(bCurrent, Alpha);

	CWOpp *pWOpp = NULL;
	if(bCurrent) pWOpp = m_pCurWOpp;
	else         pWOpp = GetWOpp(Alpha);
	
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CString strong;
	m_bResetglOpp    = true;
	m_bResetglWake   = true;
	m_bResetglStream = true;
	m_bResetglFlow   = true;

	if(!pWOpp) {
	//first try to set the last alpha which has been selected
		if(m_pCurWPolar)
			pWOpp = GetWOpp(m_pCurWPolar->m_AMem);
		else 
			pWOpp = GetWOpp(m_LastWOpp);
	}

	if(!pWOpp) pWOpp = m_pCurWOpp;

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
	if(m_pCurWOpp) {
		m_LastWOpp = m_pCurWOpp->m_Alpha;
		m_pCurWPolar->m_AMem = m_pCurWOpp->m_Alpha;
//		m_nWakeNodes   = m_pCurWOpp->m_nWakeNodes;
//		m_NXWakePanels = m_pCurWOpp->m_NXWakePanels;
//		m_FirstPanelSize  =		m_pCurWOpp->m_FirstWakePanel;
//		m_WakePanelFactor =		m_pCurWOpp->m_WakeFactor;
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

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	int LegendSize, LegendWidth, ypos;
	int x1;
	int i,k,l,nc;
	COLORREF color;
	int style, width;
	CString strong, str1, str2, str3, str4;
	CChildView * pChildView = (CChildView*)m_pChildWnd;

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
	CWOpp * pWOpp;
	CPOpp *pPOpp;

	for (i=0; i<m_poaWOpp->GetSize(); i++){
		bFound = false;
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		for (int j=0; j<str.GetSize(); j++){
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
		for (int j=0; j<str.GetSize(); j++){
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

	if(m_bCurWOppOnly){
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
		str3.Format("-a=%5.2f", m_pCurWOpp->m_Alpha);
		if(m_pCurWOpp->m_AnalysisType==1)  str4="-LLT";
		else{
			if(m_pCurWOpp->m_bVLM1)	str4="-VLM1";
			else 					str4="-VLM2";
			if(!m_pCurWOpp->m_bMiddle)	str4+="-TB";
		}
		if(m_pCurWOpp->m_bOut) str4+=" (Out)";
		pDC->TextOut(place.x + (int)(3*LegendSize),
					 place.y + (int)(1.*ypos*ny)-(int)(ypos/2),
					 str1+str2+str3+str4);
		pDC->SelectObject(pOldPen);			
	}
	else {
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
						str3.Format("-a=%5.2f", pWOpp->m_Alpha);
						if(pWOpp->m_AnalysisType==1) str4 ="-LLT";
						else {
							if(pWOpp->m_bVLM1)       str4 ="-VLM1";
							else                     str4 ="-VLM2";
							if(!pWOpp->m_bMiddle)    str4+="-TB";
						}
						if(pWOpp->m_bOut)            str4+=" (Out)";
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
						if(pPOpp->m_bVLM1)      str4 ="-VLM1";
						else                    str4 ="-VLM2";
						if(!pPOpp->m_bMiddle)	str4+="-TB";
						if(pPOpp->m_bOut)           str4+=" (Out)";
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


void CMiarex::DrawWPolarLegend(CDC *pDC, bool bIsPrinting, CPoint place, int bottom)
{
	//draws the WPolar legend to the device context,
	// bottom is the lower limit not to exceed for the legend

	int LegendSize, LegendWidth, ypos;
	int x1;
	int i, j;
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
				 pWPolar->m_Type == 4 && m_bType4))

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
						 pWPolar->m_Type == 4 && m_bType4)){//is there anything to draw ?

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
	if(m_iView==1){
		if(m_iWingView==1) {
			m_pCurWingGraph = m_pCurGraph;
			return m_pCurGraph;
		}
		else if (m_iWingView==2){
			if(m_WingGraph1.IsInDrawRect(pt)){return &m_WingGraph1;}
			if(m_WingGraph2.IsInDrawRect(pt)){return &m_WingGraph2;}
			return NULL;
		}
		else {
			if(m_WingGraph1.IsInDrawRect(pt)){return &m_WingGraph1;}
			if(m_WingGraph2.IsInDrawRect(pt)){return &m_WingGraph2;}
			if(m_WingGraph3.IsInDrawRect(pt)){return &m_WingGraph3;}
			if(m_WingGraph4.IsInDrawRect(pt)){return &m_WingGraph4;}
		}
	}
	else if(m_iView==2){
		if(m_iWPlrView==1) {
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
	else if(m_iView==4){
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

	m_pCurGraph = GetGraph(ClientPoint);

	if(m_iView==3){
		CMenu menu;
		if (menu.LoadMenu(IDR_CTXWING3DMENU)){
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);

			pPopup->CheckMenuItem(IDM_3DVIEW,     MF_BYCOMMAND | MF_CHECKED);
			if (pFrame->m_bXCmRef) pPopup->CheckMenuItem(IDM_SHOWXCMREF, MF_BYCOMMAND | MF_CHECKED);

			
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				ScreenPoint.x, ScreenPoint.y, m_pFrame); // use main window for cmds
		}
	}
	else if(m_iView==1 || m_iView==4){
		CMenu menu;
		if (menu.LoadMenu(IDR_CTXWINGOPPMENU)){
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);

			if(m_bShowWing2)	pPopup->CheckMenuItem(IDM_SHOWWING2, MF_BYCOMMAND | MF_CHECKED);
			if(m_bShowStab)		pPopup->CheckMenuItem(IDM_SHOWSTAB, MF_BYCOMMAND | MF_CHECKED);
			if(m_bShowFin)		pPopup->CheckMenuItem(IDM_SHOWFIN, MF_BYCOMMAND | MF_CHECKED);

			if (pFrame->m_bXCmRef) pPopup->CheckMenuItem(IDM_SHOWXCMREF, MF_BYCOMMAND | MF_CHECKED);

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
	else {
//		if(m_pCurGraph && m_pCurGraph->IsInDrawRect(ClientPoint)){
			CMenu menu;
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
//	}
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

	ar << m_WakePanelFactor << m_FirstPanelSize << m_NXWakePanels;
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
	if(m_bCheckPlanePanels) ar << 1; else ar<<0;
	if(m_bResetWake)        ar << 1; else ar<<0;
	ar << m_WngAnalysis.m_UnitType;
	ar << m_WingColor << m_StabColor << m_FinColor;

	ar << m_FlowLinesDlg.m_NX      << m_FlowLinesDlg.m_DeltaL;
	ar << m_FlowLinesDlg.m_XFactor << m_FlowLinesDlg.m_XOffset << m_FlowLinesDlg.m_ZOffset ;
	if(m_FlowLinesDlg.m_bLE) ar <<1; else ar <<0;
	
	m_GLLightDlg.SaveSettings(ar);

	ar << pFrame->m_WOperDlgBar.m_Alpha0;
	ar << pFrame->m_WOperDlgBar.m_AlphaMax;
	ar << pFrame->m_WOperDlgBar.m_DeltaAlpha;
	ar << pFrame->m_WOperDlgBar.m_DeltaQInf;

	if(pFrame->m_WOperDlgBar.m_bSequence)   ar<<1; else ar <<0;
	if(m_bStoreWOpp)                        ar<<1; else ar <<0;
	if(m_bKeepOutOpps)                      ar<<1; else ar <<0;

	for (int i=0; i<20; i++){
		ar << m_UFOdlg.m_ColSize[i];
	}
}


bool CMiarex::LoadSettings(CArchive &ar)
{
	//loads the Miarex settings from the archive ar
	CFile fp;
	int k;
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
		pFrame->SetWGraphTitles(&m_WPlrGraph1, m_XW1,m_YW1);
		pFrame->SetWGraphTitles(&m_WPlrGraph2, m_XW2,m_YW2);
		pFrame->SetWGraphTitles(&m_WPlrGraph3, m_XW3,m_YW3);
		pFrame->SetWGraphTitles(&m_WPlrGraph4, m_XW4,m_YW4);

		ar >> m_iView >> m_iWingView >> m_iWPlrView;
		if(m_iView <0 || m_iView>3 || m_iWingView<0|| m_iWingView>4|| m_iWPlrView<0|| m_iWPlrView>4){
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

		ar >> m_WakePanelFactor >> m_FirstPanelSize >> m_NXWakePanels;
		ar >> m_MaxWakeIter     >> m_WakeInterNodes >> m_CoreSize;
		if (m_WakePanelFactor<0.0 || m_FirstPanelSize<0.0 || m_WakeInterNodes<0 ||
			m_NXWakePanels <0  || m_MaxWakeIter <0 || m_CoreSize<0){
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
		if (k) m_bCheckPlanePanels = true; else m_bCheckPlanePanels = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_bResetWake = true; else m_bResetWake = false;

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
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		if (k) m_FlowLinesDlg.m_bLE = true; else m_FlowLinesDlg.m_bLE = false;

		m_FlowLinesDlg.SetParams();

		if(!m_GLLightDlg.LoadSettings(ar)){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			throw pfe;
		}
		ar >> pFrame->m_WOperDlgBar.m_Alpha0;
		ar >> pFrame->m_WOperDlgBar.m_AlphaMax;
		ar >> pFrame->m_WOperDlgBar.m_DeltaAlpha;
		ar >> pFrame->m_WOperDlgBar.m_DeltaQInf;

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

		for (int i=0; i<20; i++){
			ar >> m_UFOdlg.m_ColSize[i];
		}

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

	if(m_iView==3){
		pMenu->CheckMenuItem(IDM_WOPP,      MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_SHOWPOLAR, MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_3DVIEW,    MF_BYCOMMAND | MF_CHECKED);
		pMenu->CheckMenuItem(IDM_CPVIEW,    MF_BYCOMMAND | MF_UNCHECKED);
	}
	else if(m_iView==2){
		pMenu->CheckMenuItem(IDM_3DVIEW,    MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_WOPP,      MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_SHOWPOLAR, MF_BYCOMMAND | MF_CHECKED);
		pMenu->CheckMenuItem(IDM_CPVIEW,    MF_BYCOMMAND | MF_UNCHECKED);
	}

	else if(m_iView==1){
		pMenu->CheckMenuItem(IDM_3DVIEW,    MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_WOPP,      MF_BYCOMMAND | MF_CHECKED);
		pMenu->CheckMenuItem(IDM_SHOWPOLAR, MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_CPVIEW,    MF_BYCOMMAND | MF_UNCHECKED);
	}

	else if(m_iView==4){
		pMenu->CheckMenuItem(IDM_3DVIEW,    MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_WOPP,      MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_SHOWPOLAR, MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_CPVIEW,    MF_BYCOMMAND | MF_CHECKED);
	}

	if(m_iView==1){
		pMenu = m_pFrame->GetMenu()->GetSubMenu(5);
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
		if(m_bShowWing2)	pMenu->CheckMenuItem(IDM_SHOWWING2, MF_BYCOMMAND | MF_CHECKED);
		else				pMenu->CheckMenuItem(IDM_SHOWWING2, MF_BYCOMMAND | MF_UNCHECKED);
		if(m_bShowStab)		pMenu->CheckMenuItem(IDM_SHOWSTAB, MF_BYCOMMAND | MF_CHECKED);
		else				pMenu->CheckMenuItem(IDM_SHOWSTAB, MF_BYCOMMAND | MF_UNCHECKED);
		if(m_bShowFin)		pMenu->CheckMenuItem(IDM_SHOWFIN, MF_BYCOMMAND | MF_CHECKED);
		else				pMenu->CheckMenuItem(IDM_SHOWFIN, MF_BYCOMMAND | MF_UNCHECKED);
		if(m_bShowElliptic)	pMenu->CheckMenuItem(IDM_SHOWELLIPTIC, MF_BYCOMMAND | MF_CHECKED);
		else				pMenu->CheckMenuItem(IDM_SHOWELLIPTIC, MF_BYCOMMAND | MF_UNCHECKED);
	}
	else if(m_iView==2){
		pMenu = m_pFrame->GetMenu()->GetSubMenu(4);
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
		else if (m_iWPlrView==2){
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH1, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH2, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH3, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH4, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_TWOGRAPHS,   MF_BYCOMMAND | MF_CHECKED);
			pMenu->CheckMenuItem(IDM_FOURGRAPHS,  MF_BYCOMMAND | MF_UNCHECKED);
		}
		else if (m_iWPlrView==4){
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH1, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH2, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH3, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_SINGLEGRAPH4, MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_TWOGRAPHS,   MF_BYCOMMAND | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_FOURGRAPHS,  MF_BYCOMMAND | MF_CHECKED);
		}
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
		m_pCurGraph == &m_WingGraph4){
		
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
	if(m_pCurGraph == &m_WPlrGraph1){
		UVDlg.m_XSel = m_XW1;
		UVDlg.m_YSel = m_YW1;
		if(IDOK == UVDlg.DoModal()){
			m_XW1 = UVDlg.m_XSel;
			m_YW1 = UVDlg.m_YSel;
			pFrame->SetWGraphTitles(&m_WPlrGraph1,m_XW1, m_YW1);
			m_WPlrGraph1.SetAuto(true);
			m_WPlrGraph1.SetAutoYMinUnit(true);
		}
	}
	else if(m_pCurGraph == &m_WPlrGraph2){
		UVDlg.m_XSel = m_XW2;
		UVDlg.m_YSel = m_YW2;
		if(IDOK == UVDlg.DoModal()){
			m_XW2 = UVDlg.m_XSel;
			m_YW2 = UVDlg.m_YSel;
			pFrame->SetWGraphTitles(&m_WPlrGraph2,m_XW2, m_YW2);
			m_WPlrGraph2.SetAuto(true);
			m_WPlrGraph2.SetAutoYMinUnit(true);
		}
	}
	else if(m_pCurGraph == &m_WPlrGraph3){
		UVDlg.m_XSel = m_XW3;
		UVDlg.m_YSel = m_YW3;
		if(IDOK == UVDlg.DoModal()){
			m_XW3 = UVDlg.m_XSel;
			m_YW3 = UVDlg.m_YSel;
			pFrame->SetWGraphTitles(&m_WPlrGraph3,m_XW3, m_YW3);
			m_WPlrGraph3.SetAuto(true);
			m_WPlrGraph3.SetAutoYMinUnit(true);
		}
	}
	else if(m_pCurGraph == &m_WPlrGraph4){
		UVDlg.m_XSel = m_XW4;
		UVDlg.m_YSel = m_YW4;
		if(IDOK == UVDlg.DoModal()){
			m_XW4 = UVDlg.m_XSel;
			m_YW4 = UVDlg.m_YSel;
			pFrame->SetWGraphTitles(&m_WPlrGraph4,m_XW4, m_YW4);
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
			 m_bType4 && pWPolar->m_Type == 4 )){

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
	pX = (CArray <double, double> *) pFrame->GetUFOPlrVariable(pWPolar, XVar);
	pY = (CArray <double, double> *) pFrame->GetUFOPlrVariable(pWPolar, YVar);

	for (i=0; i<pWPolar->m_Alpha.GetSize(); i++){
		bAdd = true;
		x = (*pX)[i];
		y = (*pY)[i];

		if((XVar==13 || XVar==16 || XVar==17) && x<0) bAdd = false;
		if((YVar==13 || YVar==16 || YVar==17) && y<0) bAdd = false;

		if(XVar==14 || XVar==15)              x *= pFrame->m_NtoUnit; //force
		if(YVar==14 || YVar==15)              y *= pFrame->m_NtoUnit; //force
		if(XVar==16 || XVar==17 || XVar==18)  x *= pFrame->m_mstoUnit;//speed
		if(YVar==16 || YVar==17 || YVar==18)  y *= pFrame->m_mstoUnit;//speed

		if(XVar==20 || XVar==21 || XVar==22)  x *= pFrame->m_NmtoUnit;//moment
		if(YVar==20 || YVar==21 || YVar==22)  y *= pFrame->m_NmtoUnit;//moment

		if(XVar==23 || XVar==24)              x *= pFrame->m_mtoUnit;//force
		if(YVar==23 || YVar==24)              y *= pFrame->m_mtoUnit;//force

		if(XVar==25)                          x *= pFrame->m_NmtoUnit;//moment
		if(YVar==25)                          y *= pFrame->m_NmtoUnit;//moment

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
	m_pCurGraph = GetGraph(point);
	if(m_rCltRect.PtInRect(point)) m_pChildWnd->SetFocus();

	if (m_pCurWing || (m_pCurGraph && m_pCurGraph->IsInDrawRect(point))){
		m_PointDown.x = point.x;
		m_PointDown.y = point.y;

		m_hcMove = AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_HMOVE));
		m_bTrans = true;
		SetCursor(m_hcMove);
		m_bTransGraph = true;
		if(m_pCurGraph && m_pCurGraph->IsInDrawRect(point))
			m_bTransGraph = true;		
		else 
			m_bTransGraph = false;
	}	
}


void CMiarex::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (GetCapture() == this) ReleaseCapture();	
	m_bTrans = false;
	SetCursor(m_hcArrow);
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
	bool bCtrl = false;
	SHORT sh1  = GetKeyState(VK_LCONTROL);
	SHORT sh2  = GetKeyState(VK_RCONTROL);
	SHORT shZ  = GetKeyState(90);
	SHORT shX = GetKeyState('X');
	SHORT shY = GetKeyState('Y');

	if ((sh1 & 0x8000)||(sh2 & 0x8000)) bCtrl =true;

	if ((nFlags & MK_LBUTTON) && m_bTrans && m_iView!=3){
		m_pCurGraph = GetGraph(point);
		if(m_pCurGraph && m_bTransGraph){ 
			// we translate the curves inside the graph
			double xu, yu;
			m_pCurGraph->SetAuto(false);
			double x1 =  m_pCurGraph->ClientTox(m_PointDown.x) ;
			double y1 =  m_pCurGraph->ClientToy(m_PointDown.y) ;
			
			xu = m_pCurGraph->ClientTox(point.x);
			yu = m_pCurGraph->ClientToy(point.y);

			double xmin = m_pCurGraph->GetXMin() - xu+x1;
			double xmax = m_pCurGraph->GetXMax() - xu+x1;
			double ymin = m_pCurGraph->GetYMin() - yu+y1;
			double ymax = m_pCurGraph->GetYMax() - yu+y1;

			m_pCurGraph->SetWindow(xmin, xmax, ymin, ymax);
			UpdateView();

		}
		else if (m_pCurWing  && m_iView!=2 && m_iWingView ==1){
			// we translate the wing
			
			m_ptOffset.x += point.x - m_PointDown.x;
			m_ptOffset.y += point.y - m_PointDown.y;
			UpdateView();

		}
		else if (m_pCurWing  && m_iView!=2 && m_iWingView !=1){
			// we translate the legend
			//horizontally only
			m_WingLegendOffset.x += point.x - m_PointDown.x;
			UpdateView();
		}
		else if (m_iView==2){
			// we translate the legend
			if(m_iWPlrView==1 || m_iWPlrView==4){
				m_WPlrLegendOffset.y += point.y - m_PointDown.y;
				UpdateView();
			}
			else if(m_iWPlrView==2){
				m_WPlrLegendOffset.x += point.x - m_PointDown.x;
				UpdateView();
			}

		}
	}
	else if (nFlags & MK_LBUTTON && m_iView==3) {
		if(bCtrl){//rotate
			CRect CenterRect;
			CenterRect.CopyRect(m_rCltRect);
			CenterRect.DeflateRect(000, 100);
			int dx = m_PointDown.x - point.x;
			int dy = m_PointDown.y - point.y;
			if(CenterRect.PtInRect(point)){
				if(abs(dy)>abs(dx))	pW3DBar->m_glXRotatef -= (GLfloat)(dy / 7.0);
				else				pW3DBar->m_glYRotatef -= (GLfloat)(dx / 7.0);
			}
			else{
				if(abs(dy)>abs(dx))	pW3DBar->m_glZRotatef -= (GLfloat)(dy / 7.0);
				else				pW3DBar->m_glZRotatef -= (GLfloat)(dx / 7.0);

			}
			UpdateView();
		}
		else if(m_bTrans){//translate
			pW3DBar->m_glXTransf += (GLfloat)(m_PointDown.x - point.x);
			pW3DBar->m_glYTransf += (GLfloat)(m_PointDown.y - point.y);
			UpdateView();
		}
	}

	else if ((nFlags & MK_MBUTTON) || (shZ & 0x8000)) {
		// we zoom the graph or the wing	
		if(m_iView !=3){
			m_pCurGraph = GetGraph(point);
			if(m_pCurGraph && m_pCurGraph->IsInDrawRect(point)){ 
				//zoom graph

				if (shX & 0x8000){
					//zoom x scale
					m_pCurGraph->SetAutoX(false);
					m_pCurGraph->SetAutoX(false);
					if(point.x-m_PointDown.x<0) m_pCurGraph->Scalex(1.04);
					else						m_pCurGraph->Scalex(1.0/1.04);
				}
				else if(shY & 0x8000){
					//zoom y scale
					m_pCurGraph->SetAutoY(false);
					m_pCurGraph->SetAutoY(false);
					if(point.y-m_PointDown.y<0) m_pCurGraph->Scaley(1.04);
					else						m_pCurGraph->Scaley(1.0/1.04);
				}
				else{
					//zoom both
					m_pCurGraph->SetAuto(false);
					if(point.y-m_PointDown.y<0) m_pCurGraph->Scale(1.06);
					else						m_pCurGraph->Scale(1.0/1.06);
				}


				UpdateView();
			}
			else if(m_pCurWing && m_iView!=2){
				//zoom wing
				if(point.y-m_PointDown.y<0) m_WingScale /=1.06;
				else                        m_WingScale *= 1.06;
				UpdateView();
			}
		}
		else{
			if(m_pCurWing){//zoom 3D wing
				if(point.y-m_PointDown.y>0) 
					pW3DBar->m_glScalef *= (GLfloat)1.06;
				else	
					pW3DBar->m_glScalef /= (GLfloat)1.06;

				UpdateView();
			}
		}
	}

	m_PointDown = point;
	
//	CWnd::OnMouseMove(nFlags, point);
}


BOOL CMiarex::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	//The mouse button has been wheeled
	//Process the message
//	point is in client coordinates
	if(m_iView !=3){
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
		else if(m_pCurWing && m_iView!=2){

			if(zDelta<0) m_WingScale *= 1.06;
			else m_WingScale /= 1.06;

			UpdateView();
		}
	}
	else{
		CW3DBar* pW3DBar = (CW3DBar*)m_pW3DBar;
		if(m_pCurWing){//zoom wing
			
			if(zDelta<0) pW3DBar->m_glScalef *=(GLfloat)1.06;
			else         pW3DBar->m_glScalef /= (GLfloat)1.06;
			
			UpdateView();
		}
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
	if(m_iView==2){

		if(m_iWPlrView ==1){
			CRect SingleRect(pCltRect->left,
							 pCltRect->top,
							 pCltRect->left+pCltRect->Width()-10000,
							 pCltRect->left+pCltRect->Height());
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
	else if(m_pCurWOpp){

		if (m_iWingView == 1 && m_pCurWing)      PrintSingleWingGraph(pDC, pCltRect);
		else if (m_iWingView == 2 && m_pCurWing) PrintTwoWingGraph(pDC, pCltRect);
		else if (m_iWingView == 4 && m_pCurWing) PrintFourWingGraph(pDC, pCltRect);

	}
	if(m_iView==2 || m_pCurWOpp){
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

	
/*	CRect Rect1(margin, margin,
				w2+margin,h23);
	CRect Rect2(w2+margin, margin,
				w+margin, h23);*/
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
		if (pFrame->m_bXCP)     PaintXCP(pDC, ORef, PtLegend, scale, true);
	}
	if (pFrame->m_bXCmRef)  PaintXCmRef(pDC, ORef, scale, true);
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
		Result.Format("Pitching Moment coef. = %8.4f", pWOpp->m_PitchingMoment);
		pDC->TextOut(RightPos, ZPos-D, Result);

		D+=400;
		Result.Format("Rolling Moment coef. = %8.4f", pWOpp->m_RollingMoment);
		pDC->TextOut(RightPos, ZPos-D, Result);

		D+=400;
		Result.Format("Airfoil Yawing Moment coef. = %8.4f", pWOpp->m_GYm);
		pDC->TextOut(RightPos, ZPos-D, Result);

		D+=400;
		Result.Format("Induced Yawing Moment coef. = %8.4f", pWOpp->m_InducedYawingMoment);
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

	CWing *pOldWing;
	CWing* pWing = new CWing(m_pFrame);
	pWing->m_pMiarex = this;

	pWing->m_bCheckName= true;

	if(IDOK == pWing->DoModal()){
		CMainFrame *pFrame = (CMainFrame*)m_pFrame;
		pFrame->SetSaveState(false);
		bool bExists = false;
		for(int i=0; i<m_poaWing->GetSize(); i++){
			pOldWing = (CWing*)m_poaWing->GetAt(i);
			if(pWing->m_WingName == pOldWing->m_WingName){
				bExists = true;
				AfxMessageBox("The name "+pWing->m_WingName+" already exists", MB_OK);
				break;
			}
		}

		if(bExists){
			if(!SetModWing(pWing)){
				delete pWing;
				return;
			}
		}

		m_pCurWing = AddWing(pWing);
		SetUFO();
		if(m_iView==1){
			SetScale();
			OnAdjustToWing();
		}
		SetScale();
		SetWGraphScale();
	
		UpdateUFOs();
		UpdateView();
	}
	else {
		delete pWing;
	}
}


void CMiarex::On3DView() 
{
	// The user has requested a 3D view
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CToolBarCtrl *pTB = &(pFrame->m_MiarexBar.GetToolBarCtrl());
	pTB->PressButton(IDT_WOPP, false);
	pTB->PressButton(IDT_WPOLARS, false);
	pTB->PressButton(IDM_3DVIEW, true);
//	pTB->PressButton(IDM_CHORDCP, false);

	if(m_iView==3) {
		UpdateView();
		return;
	}
	m_iView =3;

	pFrame->DockW3DBar();
	pFrame->m_W3DBar.SetChecks();

	UpdateView();
//	OnStreamLines();

	CheckMenus();
}


void CMiarex::PaintWing(CDC *pDC, CPoint ORef, double scale, bool bIsPrinting)
{
	//Draws the wing on the 2D WOpp view
	if(!m_pCurWing)	return;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
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
	for (int i=0; i<m_pCurWing->m_NPanel;i++){
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
	if(!m_bHalfWing){
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

	if (pFrame->m_bXTop){// top transition

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
	if (pFrame->m_bXBot){// Bottom transition

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
	if(!m_pCurWing)	return;
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
	int YCm = O.y + (int)(m_pCurWing->m_XCmRef*scaley);
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
			for (int p=0; p<m_VLMMatSize; p++){
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
	for (int i=0; i<m_WingGraph1.GetCurveCount();i++){
		pCurve = m_WingGraph1.GetCurve(i);
		pCurve->m_bShowPoints = !pCurve->m_bShowPoints;
	}
	UpdateView();
}*/

void CMiarex::OnShowXCmRef() 
{
	//Show the Moment reference point
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pFrame->m_bXCmRef = !pFrame->m_bXCmRef;
	CMenu *pMenu = pFrame->GetMenu();
	if(pFrame->m_bXCmRef)	pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWXCMREF, MF_BYCOMMAND | MF_CHECKED);
	else			pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWXCMREF, MF_BYCOMMAND | MF_UNCHECKED);

	UpdateView();
}

void CMiarex::OnEditWing() 
{
	//Edit the currently selected wing
	if(!m_pCurWing)	return;
	if(m_pCurPlane){
		OnEditPlane();
		return;
	}
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CWPolar *pWPolar;
	CWOpp* pWOpp;
	bool bHasResults = false;
	for (int i=0; i< m_poaWPolar->GetSize(); i++){
		pWPolar = (CWPolar*)m_poaWPolar->GetAt(i);
		if (pWPolar->m_Alpha.GetSize() && pWPolar->m_UFOName == m_pCurWing->m_WingName){
			bHasResults = true;
			break;
		}
	}

	for (i=0; i< m_poaWOpp->GetSize(); i++){
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		if(pWOpp->m_WingName == m_pCurWing->m_WingName){
			bHasResults = true;
			break;
		}
	}

	//save a copy just in case user changes his mind (users !...)
	CWing* pSaveWing= new CWing(m_pFrame);
	pSaveWing->m_pMiarex = this;
	pSaveWing->Duplicate(m_pCurWing);

	m_pCurWing->m_bCheckName= false;

	if(IDOK == m_pCurWing->DoModal()){
		if(m_pCurWing->m_bChanged){
			if(bHasResults){
				CModDlg dlg;
				dlg.m_Question = "The modification will erase all results associated to this Wing.\nContinue ?";
				int Ans = (int)dlg.DoModal();			
				if (Ans == IDCANCEL) {

					//restore geometry for initial wing
					m_pCurWing->Duplicate(pSaveWing);
					delete pSaveWing; // clean up	
					return;
				}
				else if(Ans==20){
					//create new wing
					CWing* pNewWing= new CWing(m_pFrame);
					pNewWing->m_pFrame = this;
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
					UpdateUFOs();
					UpdateView();
					return;
				}
			}

			//then modifications are automatically recorded
			pFrame->DeleteWing(m_pCurWing, true);// will also set new surface and Aerochord in WPolars
			if(m_iView==3) {
				m_bResetglGeom = true;
				m_bResetglMesh = true;
				m_bResetglOpp  = true;
			}
			else{
				if(m_iView==2)		CreateWPolarCurves();
				else if(m_iView==1)	CreateWOppCurves();
				else if(m_iView==4)	CreateCpCurves();
			}
		}
		SetUFO();	
		UpdateWPlrs();
		SetScale();
		SetWGraphScale();
		OnAdjustToWing();
	}
	else { // restore original
		m_pCurWing->Duplicate(pSaveWing);
//		SetWing();
		UpdateView();
	}
	delete pSaveWing; // clean up
}


void CMiarex::OnDuplicateWing() 
{
	//Duplicate the currently selected wing
	if(!m_pCurWing)	return;
	
	if(m_pCurPlane) {
		DuplicatePlane();
		return;
	}

	CString UFOName;
	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return;

	CWing* pNewWing= new CWing(m_pFrame);
	pNewWing->m_pMiarex = this;
	pNewWing->Duplicate(m_pCurWing);


	if(!SetModWing(pNewWing)){
		delete pNewWing;
		UpdateView();
	}
	else {
		m_pCurWing = AddWing(pNewWing);
		SetUFO();
		UpdateUFOs();
		OnEditWing();
	}
}

void CMiarex::DuplicatePlane() 
{
	//Duplicate the currently selected plane, if any
	if(!m_pCurPlane) return;

	CPlane* pNewPlane= new CPlane(m_pFrame);
	pNewPlane->m_pMiarex = this;
	pNewPlane->Duplicate(m_pCurPlane);

	if(!SetModPlane(pNewPlane)){
		delete pNewPlane;
		SetUFO();
		UpdateUFOs();
		UpdateView();
	}
	else {
		m_pCurPlane = AddPlane(pNewPlane);
		SetUFO();
		UpdateUFOs();
		UpdateView();
		OnEditPlane();
	}
}

void CMiarex::OnScaleWing() 
{
	//Scale the currently selected wing, if any
	if(!m_pCurWing)	return;
	if(m_pCurPlane){
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

	if(IDOK == dlg.DoModal()){
		if (dlg.m_bScaleSpan || dlg.m_bScaleChord || dlg.m_bScaleSweep || dlg.m_bScaleTwist) {
			CWing* pNewWing= new CWing(m_pFrame);
			pNewWing->m_pMiarex = this;
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
			SetUFO();
			UpdateUFOs();
		}
		UpdateView();
	}
}

void CMiarex::SetUFO(CString UFOName)
{
	//Set the UFO specified by UFOName as the current UFO

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(!UFOName.GetLength()) {
		if(m_pCurPlane) UFOName = m_pCurPlane->m_PlaneName;
		else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
		else {
			m_pCurPlane = NULL;
			m_pCurWing  = NULL;
			m_pCurWing2 = NULL;
			m_pCurStab  = NULL;
			m_pCurFin   = NULL;
			return;
		}
	}
	int pos = pFrame->m_ctrlUFO.FindStringExact(0, UFOName);
	if (pos!=CB_ERR){
		m_pCurPlane = GetPlane(UFOName);
		m_pCurWing  = GetWing(UFOName);
	}
	
	if(!m_pCurWing && !m_pCurPlane) return;

	if(m_pCurPlane){
		m_pCurWing = &m_pCurPlane->m_Wing;
		if(m_pCurPlane->m_bBiplane) m_pCurWing2 = &m_pCurPlane->m_Wing2;
		else						m_pCurWing2 = NULL;
		if(m_pCurPlane->m_bStab)	m_pCurStab = &m_pCurPlane->m_Stab;
		else						m_pCurStab = NULL;
		if(m_pCurPlane->m_bFin) {
			m_pCurFin  = &m_pCurPlane->m_Fin;
			m_pCurFin->m_bDoubleSymFin = m_pCurPlane->m_bDoubleSymFin;
		}
		else                     m_pCurFin  = NULL;
	}
	else {
		m_pCurWing2 = NULL;
		m_pCurStab  = NULL;
		m_pCurFin   = NULL;
	}
	
	m_bResetglGeom = true;
	m_bResetglMesh = true;

	double WingTilt = 0.0;
	COLORREF WingColor = m_WingColor;
	CVector T(0.0,0.0,0.0);
	m_NSurfaces = 0;

	if(m_pCurPlane){
		WingTilt  = m_pCurPlane->m_WingTilt;
		WingColor = m_pCurWing->m_WingColor;
	}

	if(m_pCurWing) {
		CreateSurfaces(m_pCurWing, T,0.0,WingTilt,WingColor);
	}

	if(m_pCurWing2) {
		CreateSurfaces(m_pCurWing2,
					   m_pCurPlane->m_LEWing2,
					   0.0,
					   m_pCurPlane->m_WingTilt2,
					   m_pCurWing2->m_WingColor);
	}

	if(m_pCurStab) {
		CreateSurfaces(m_pCurStab,
					   m_pCurPlane->m_LEStab,
					   0.0,
					   m_pCurPlane->m_StabTilt,
					   m_pCurStab->m_WingColor);
	}

	if(m_pCurFin) {
		CreateSurfaces(m_pCurFin,
					   m_pCurPlane->m_LEFin,
					   90.0,
					   m_pCurPlane->m_FinTilt,
					   m_pCurFin->m_WingColor);
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
	UpdateUFOs();
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
		pFrame->SetWGraphTitles(&m_WPlrGraph1, m_XW1,m_YW1);
		pFrame->SetWGraphTitles(&m_WPlrGraph2, m_XW2,m_YW2);
		pFrame->SetWGraphTitles(&m_WPlrGraph3, m_XW3,m_YW3);
		pFrame->SetWGraphTitles(&m_WPlrGraph4, m_XW4,m_YW4);
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

	if(m_iView==3)		GLDraw3D(pDC);
	else{
		CDC memdc; 

		memdc.CreateCompatibleDC (pDC);

		CBitmap  bmp;
		bmp.CreateCompatibleBitmap(pDC, m_rCltRect.Width(), m_rCltRect.Height());
		CBitmap * pOldBmp = memdc.SelectObject(&bmp);

		memdc.FillSolidRect(&m_rCltRect,pChildView->m_crBackColor);
		memdc.SetBkMode(TRANSPARENT);


		if (m_iView==2){
			if     (m_iWPlrView == 1) PaintWSingleGraph(&memdc, &m_rCltRect, &m_rDrawRect);
			else if(m_iWPlrView == 2) PaintWCoupleGraphs(&memdc, &m_rCltRect, &m_rDrawRect);
			else if(m_iWPlrView == 4) PaintWFourGraphs(&memdc, &m_rCltRect, &m_rDrawRect);
		} 
		else if (m_iView==1) PaintWOpp(&memdc, &m_rCltRect, &m_rDrawRect);
		else if (m_iView==4) PaintCp(&memdc, &m_rCltRect, &m_rDrawRect);
		

		// Blit back
		pDC->BitBlt(m_rCltRect.left,m_rCltRect.top,m_rCltRect.Width(),m_rCltRect.Height(),
				 &memdc,0,0,SRCCOPY);


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
	if (pMsg->message == WM_KEYDOWN){
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
		if (pMsg->wParam == VK_RETURN){
			if (pWnd != &pFrame->m_ctrlUFO &&
				pWnd != &pFrame->m_ctrlWPlr &&
				pWnd != &pFrame->m_ctrlWOpp) {
				if(pFrame->m_WOperDlgBar.IsWindowVisible()) 
					pFrame->m_WOperDlgBar.m_ctrlAnalyze.SetFocus();
				return true;
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
		if (pMsg->wParam == VK_F7 ){
			OnManageUFO();
			return true;
		}
		if (pMsg->wParam == VK_F3){
			if((sh1 & 0x8000)||(sh2 & 0x8000)) {
				OnDefinePlane();
				return true;
			}
			if(!(sh & 0x8000))	OnDefineWing();
			else OnEditWing();
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
		if (pMsg->wParam == VK_F9){
			OnCpView();
			return true;
		}
		if (pMsg->wParam == VK_F6){
			OnWingAnalysis();
			return true;
		}

		if (pMsg->wParam == VK_F8){
			OnShowWPolar();
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

	if(pFrame->m_bXTop)	pMenu->GetSubMenu(4)->CheckMenuItem(IDM_TOPTRANS, MF_BYCOMMAND | MF_CHECKED);
	else                pMenu->GetSubMenu(4)->CheckMenuItem(IDM_TOPTRANS, MF_BYCOMMAND | MF_UNCHECKED);

	if(pFrame->m_bXBot)	pMenu->GetSubMenu(4)->CheckMenuItem(IDM_BOTTRANS, MF_BYCOMMAND | MF_CHECKED);
	else                pMenu->GetSubMenu(4)->CheckMenuItem(IDM_BOTTRANS, MF_BYCOMMAND | MF_UNCHECKED);

	if(pFrame->m_bXCP)	pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWXCP, MF_BYCOMMAND | MF_CHECKED);
	else                pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWXCP, MF_BYCOMMAND | MF_UNCHECKED);

	if(pFrame->m_bXCmRef)
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
	if(m_iView==3){
		pTB->PressButton(IDM_3DVIEW, true);
		pTB->PressButton(IDT_WOPP, false);
		pTB->PressButton(IDT_WPOLARS, false);
	}
	else {
		pTB->PressButton(IDT_WOPP, m_iView==1);
		pTB->PressButton(IDT_WPOLARS, m_iView==2);
		pTB->PressButton(IDM_3DVIEW, false);
	}
	CheckMenus();

	pFrame->SetWGraphTitles(&m_WPlrGraph1, m_XW1,m_YW1);
	pFrame->SetWGraphTitles(&m_WPlrGraph2, m_XW2,m_YW2);
	pFrame->SetWGraphTitles(&m_WPlrGraph3, m_XW3,m_YW3);
	pFrame->SetWGraphTitles(&m_WPlrGraph4, m_XW4,m_YW4);

	pFrame->m_bStream = false;
	m_FlowLinesDlg.m_pChildWnd = m_pChildWnd;
	m_FlowLinesDlg.SetParams();

	GLSetupLight();

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
	SetWPlr();
	UpdateWPlrs();
	UpdateView();
}

void CMiarex::OnRenameUFO()
{
	//Rename the currently selected UFO
	if(!m_pCurWing)	return;
	if(m_pCurPlane)		 RenameUFO(m_pCurPlane->m_PlaneName);
	else if (m_pCurWing) RenameUFO(m_pCurWing->m_WingName);
	UpdateUFOs();
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
	if(m_pCurPOpp){
		CPOpp* pPOpp;
		for (int i = (int)m_poaPOpp->GetSize()-1; i>=0; i--){
			pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
			if(pPOpp == m_pCurPOpp){
				m_poaPOpp->RemoveAt(i);
				delete pPOpp;
				break;
			}
		}
		m_pCurPOpp = NULL;
		m_pCurWOpp = NULL;
		UpdateWOpps();
		SetWOpp(true);
		pFrame->SetSaveState(false);
		if (m_iView==1)     CreateWOppCurves();
		else if(m_iView==4)	CreateCpCurves();

		UpdateView();	
	}

	else if(m_pCurWOpp) {
		CWOpp* pWOpp;
		for (int i = (int)m_poaWOpp->GetSize()-1; i>=0; i--){
			pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
			if(pWOpp == m_pCurWOpp){
				m_poaWOpp->RemoveAt(i);
				delete pWOpp;
				m_pCurWOpp = NULL;
				break;
			}
		}
		UpdateWOpps();
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

	for (int i = (int)m_poaPOpp->GetSize()-1; i>=0; i--){
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
	UpdateWOpps();
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

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(false);
	CWOpp* pWOpp;
	CPOpp* pPOpp;

	if(m_pCurPlane){
		for (int i = (int)m_poaPOpp->GetSize()-1; i>=0; i--){
			pPOpp = (CPOpp*) m_poaPOpp->GetAt(i);
			if (pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName &&
				pPOpp->m_PlrName  == m_pCurWPolar->m_PlrName){
				m_poaPOpp->RemoveAt(i);
				delete pPOpp;
			}
		}
	}
	else if (m_pCurWing){
		for (int i = (int)m_poaWOpp->GetSize()-1; i>=0; i--){
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
	UpdateWOpps();
	SetWOpp(true);
	if (m_iView==1)     CreateWOppCurves();
	else if(m_iView==4)	CreateCpCurves();
	UpdateView();
}


void CMiarex::OnDelAllWOpps() 
{
	//Delete all the WOpps or POpps
	if(!m_pCurPlane && !m_pCurWing) return;
	if(!m_pCurWPolar) return;
	if(!m_pCurWPolar) return;
	m_bAnimate = false;

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(false);
	CWOpp* pWOpp;
	CPOpp* pPOpp;
	if(m_pCurPlane){
		for (int i = (int)m_poaPOpp->GetSize()-1; i>=0; i--){
			pPOpp = (CPOpp*) m_poaPOpp->GetAt(i);
			if (pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName){
				m_poaPOpp->RemoveAt(i);
				delete pPOpp;
			}
		}
	}
	else if (m_pCurWing){
		for (int i = (int)m_poaWOpp->GetSize()-1; i>=0; i--){
			pWOpp = (CWOpp*) m_poaWOpp->GetAt(i);
			if (pWOpp->m_WingName == m_pCurWing->m_WingName){
				m_poaWOpp->RemoveAt(i);
				delete pWOpp;
			}
		}
	}
	m_pCurPOpp = NULL;
	m_pCurWOpp = NULL;
	UpdateWOpps();
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

	//delete all WPolars associated to the current wing/plane
	if(!m_pCurWing)		return;
	
	int i;

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
	UpdateWPlrs();
	UpdateView();
}


void CMiarex::OnDeleteWPolar() 
{
	//delete the current WPolar
	m_bAnimate = false;
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
	for (int i=(int)m_poaWOpp->GetSize()-1; i>=0; i--){
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		if (pWOpp->m_PlrName  == m_pCurWPolar->m_PlrName &&
			pWOpp->m_WingName == UFOName){
			m_poaWOpp->RemoveAt(i);
			delete pWOpp;
		}
	}
	//next remove all the POpps associated to the Wing Polar
	CPOpp * pPOpp;
	for (i=(int)m_poaPOpp->GetSize()-1; i>=0; i--){
		pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
		if (pPOpp->m_PlrName  == m_pCurWPolar->m_PlrName &&
			pPOpp->m_PlaneName == UFOName){
			m_poaPOpp->RemoveAt(i);
			delete pPOpp;
		}
	}
	//next remove the WPolar
	CWPolar* pWPolar;
	for (i=(int)m_poaWPolar->GetSize()-1;i>=0; i--){
		pWPolar = (CWPolar*)m_poaWPolar->GetAt(i);
		if (pWPolar == m_pCurWPolar){
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
	UpdateWPlrs();
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


	else{//m_iView=3
		if(m_bTrans) {
			SetCursor(m_hcMove);
			return TRUE;
		}
		else{
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
		strong = m_pCurWOpp->m_WingName + "\n";
		XFile.WriteString(strong);
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

		strong.Format("Cm    = %9.4f     GCm   = %9.4f     PCm   = %9.4f\n",
			m_pCurWOpp->m_GCm+m_pCurWOpp->m_VCm, m_pCurWOpp->m_GCm,	m_pCurWOpp->m_VCm);
		XFile.WriteString(strong);

		strong.Format("Rm    = %9.4f\n",	m_pCurWOpp->m_RollingMoment);
		XFile.WriteString(strong);

		strong.Format("IYm   = %9.4f     PYm   = %9.4f \n",
			m_pCurWOpp->m_InducedYawingMoment, m_pCurWOpp->m_GYm);
		XFile.WriteString(strong);

		strong.Format("XCP   = %9.4f     YCP   = %9.4f \n",
			m_pCurWOpp->m_XCP, m_pCurWOpp->m_YCP);
		XFile.WriteString(strong);

		strong.Format("Bend. = %9.4f\n\n",m_pCurWOpp->m_MaxBending);
		XFile.WriteString(strong);
		
		XFile.WriteString("\nWing Data\n");
		for (l=0; l<m_pCurWOpp->m_nFlaps; l++){
	 		strong.Format("Flap %d moment = %9.4f",l+1, m_pCurWOpp->m_FlapMoment[l]*pFrame->m_NmtoUnit);
			GetMomentUnit(str, pFrame->m_MomentUnit);
			strong += str +"\n";
			XFile.WriteString(strong);      
		}
		XFile.WriteString("\n\n");
		m_pCurWOpp->Export(&XFile);

		if(m_pCurPOpp) {
			XFile.WriteString("\n\nElevator Data\n");
			for (l=0; l<m_pCurPOpp->m_Wing2WOpp.m_nFlaps; l++){
	 			strong.Format("Flap %d moment = %9.4f ",l+1, m_pCurPOpp->m_Wing2WOpp.m_FlapMoment[l]*pFrame->m_NmtoUnit);
				GetMomentUnit(str, pFrame->m_MomentUnit);
				strong += str +"\n";
				XFile.WriteString(strong);      
			}
			XFile.WriteString("\n\n");
			m_pCurPOpp->m_Wing2WOpp.Export(&XFile);
		}

		if(m_pCurStab) {
			XFile.WriteString("\n\nElevator Data\n");
			for (l=0; l<m_pCurPOpp->m_StabWOpp.m_nFlaps; l++){
	 			strong.Format("Flap %d moment = %9.4f ",l+1, m_pCurPOpp->m_StabWOpp.m_FlapMoment[l]*pFrame->m_NmtoUnit);
				GetMomentUnit(str, pFrame->m_MomentUnit);
				strong += str +"\n";
				XFile.WriteString(strong);      
			}
			XFile.WriteString("\n\n");
			m_pCurPOpp->m_StabWOpp.Export(&XFile);
		}

		if(m_pCurFin) {
			XFile.WriteString("\n\nFin Data\n");
			for (l=0; l<m_pCurPOpp->m_FinWOpp.m_nFlaps; l++){
	 			strong.Format("Flap %d moment = %9.4f",l+1, m_pCurPOpp->m_FinWOpp.m_FlapMoment[l]*pFrame->m_NmtoUnit);
				GetMomentUnit(str, pFrame->m_MomentUnit);
				strong += str +"\n";
				XFile.WriteString(strong);      
			}
			XFile.WriteString("\n\n");
			m_pCurPOpp->m_FinWOpp.Export(&XFile);
		}


		XFile.WriteString("Panel          Cp\n");
		for(p=0; p<m_VLMMatSize; p++){
			if(m_pCurPOpp)
				strong.Format("%4d     %12.5f\n", p, m_pCurPOpp->m_Cp[p]);
			else if(m_pCurWOpp)
				strong.Format("%4d     %12.5f\n", p, m_pCurWOpp->m_Cp[p]);
			
			XFile.WriteString(strong);

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
	UpdateWOpps();
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
	//define the curernt WPolar grpah's vaiable
	if(!m_pCurGraph && m_iView!=2) return;
	CWGraphVarDlg dlg;
	int Var = m_WOppVar1;
	if     (m_pCurGraph==&m_WingGraph1) Var = m_WOppVar1;
	else if(m_pCurGraph==&m_WingGraph2) Var = m_WOppVar2;
	else if(m_pCurGraph==&m_WingGraph3) Var = m_WOppVar3;
	else if(m_pCurGraph==&m_WingGraph4) Var = m_WOppVar4;

	dlg.m_iWVar = Var;
	if(dlg.DoModal() == IDOK){
		for (int i=m_WingGraph1.GetCurveCount()-1; i>0; i--){//0 is curretn curve
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

// Write wing data
	pDC->SetTextColor(pChildView->m_WndTextColor);
	CString Result, str, strong;

	CFont LegendFont;
	LegendFont.CreateFontIndirect(&pChildView->m_WndLogFont);

	CFont* pOldFont = pDC->SelectObject(&LegendFont);
	pDC->SetTextAlign(TA_LEFT);

	int D = 0;
	int LeftPos = margin;
	int ZPos    = pCltRect->Height()-10*12;

	if(m_pCurWing){
		if(m_pCurWPolar) ZPos-=24;
		if(m_iView!=2) {
			PaintWing(pDC, m_ptOffset, m_WingScale, false);
			if(m_pCurWOpp && m_pCurWOpp->m_bIsVisible) {
				CPoint PtLegend;
				PtLegend.x = (int)(m_rCltRect.Width()/2);
				PtLegend.y = m_rCltRect.bottom;
				PaintXTr(pDC, m_ptOffset, PtLegend, m_WingScale, false);
				if (pFrame->m_bXCP) PaintXCP(pDC, m_ptOffset,PtLegend, m_WingScale, false);
				if (pFrame->m_bXCmRef) PaintXCmRef(pDC, m_ptOffset, m_WingScale, false);
			}
			pDC->TextOut(LeftPos,ZPos, m_pCurWing->m_WingName);
			D+=12;
			CString length, surface;
			GetLengthUnit(length,pFrame->m_LengthUnit);
			GetAreaUnit(surface,pFrame->m_AreaUnit);

			Result.Format("Wing span    = %7.2f "+length, m_pCurWing->m_Span*pFrame->m_mtoUnit);
			pDC->TextOut(LeftPos,ZPos+D, Result);
			D+=12;
			Result.Format("Wing area    = %7.2f "+surface, m_pCurWing->m_Area * pFrame->m_m2toUnit);
			pDC->TextOut(LeftPos,ZPos+D, Result);
			D+=12;
			if(m_pCurWPolar){
				GetWeightUnit(str, pFrame->m_WeightUnit);
				Result.Format("Plane weight = %7.2f ",  m_pCurWing->m_Weight*pFrame->m_kgtoUnit);
				Result += str;
				pDC->TextOut(LeftPos,ZPos+D, Result);
				D+=12;
				GetAreaUnit(strong, pFrame->m_AreaUnit);
				Result.Format("Wing load    = %7.3f ",
							  m_pCurWing->m_Weight*pFrame->m_kgtoUnit/m_pCurWing->m_Area/pFrame->m_m2toUnit);
				Result += str + "/" + strong;
				pDC->TextOut(LeftPos,ZPos+D, Result);
				D+=12;
			}
			Result.Format("Root chord   = %7.2f "+length, m_pCurWing->m_TChord[0]*pFrame->m_mtoUnit);
			pDC->TextOut(LeftPos,ZPos+D, Result);
			D+=12;
			Result.Format("M.A.C.       = %7.2f "+length, m_pCurWing->m_MAChord*pFrame->m_mtoUnit);
			pDC->TextOut(LeftPos,ZPos+D, Result);
			D+=12;				
			Result.Format("Twist at tip = %7.1f °", m_pCurWing->m_TTwist[m_pCurWing->m_NPanel]);
			pDC->TextOut(LeftPos,ZPos+D, Result);
			D+=12;
			Result.Format("Aspect Ratio = %7.1f", m_pCurWing->m_AR);
			pDC->TextOut(LeftPos,ZPos+D, Result);
			D+=12;
			Result.Format("Taper Ratio  = %7.1f", m_pCurWing->m_TR);
			pDC->TextOut(LeftPos,ZPos+D, Result);
			D+=12;
			Result.Format("Rt-Tip sweep = %7.1f °", m_pCurWing->GetAverageSweep());
			pDC->TextOut(LeftPos,ZPos+D, Result);
		}
	}

	//END Write wing data
	if (m_pCurWing && w2>200 && h>250){
		if(pDrawRect->Width()<200 || pDrawRect->Height()<200) return;
		m_CpGraph.DrawGraph(pDC, pDrawRect, false);
		CPoint Place(pDrawRect->left+10, pDrawRect->top +30);
		DrawWOppLegend(pDC, false, Place, pCltRect->bottom);
	}

	pDC->SetTextColor(pChildView->m_WndTextColor);
	pDC->SetTextAlign(TA_RIGHT);

	D = 0;
	int RightPos = pCltRect->Width()-margin;
	ZPos	 = pCltRect->Height()-13*12;
	if(m_pCurWOpp && m_pCurWOpp->m_bOut) ZPos -= 12;

	if(m_pCurWOpp && m_pCurWOpp->m_bIsVisible){
		GetSpeedUnit(str, pFrame->m_SpeedUnit);
		int l = str.GetLength();
		if(l==2) Result.Format("QInf = %6.2f ", m_pCurWOpp->m_QInf*pFrame->m_mstoUnit);
		else if(l==3) Result.Format("QInf = %5.1f ", m_pCurWOpp->m_QInf*pFrame->m_mstoUnit);
		else if(l==4) Result.Format("QInf = %4.1f ", m_pCurWOpp->m_QInf*pFrame->m_mstoUnit);

		Result += str;
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		Result.Format("Alpha = %7.3f° ", m_pCurWOpp->m_Alpha);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		if(m_pCurWOpp->m_bOut){
			Result = "Point is out of the flight envelope";
			D+=12;
			pDC->TextOut(RightPos,ZPos+D, Result);
		}
		Result.Format("Lift coef. = %8.4f ", m_pCurWOpp->m_CL);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		Result.Format("Drag coef. = %8.4f ", m_pCurWOpp->m_ViscousDrag+m_pCurWOpp->m_InducedDrag);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		/*		oswald=CZ^2/CXi/pi/allongement;*/
		double cxielli=m_pCurWOpp->m_CL*m_pCurWOpp->m_CL/pi/m_pCurWing->m_AR;
		Result.Format("Oswald = %8.4f ", cxielli/m_pCurWOpp->m_InducedDrag);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		Result.Format("Cl/Cd = %8.4f ", 
			m_pCurWOpp->m_CL/(m_pCurWOpp->m_InducedDrag+m_pCurWOpp->m_ViscousDrag));
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		GetLengthUnit(str, pFrame->m_LengthUnit);
		l = str.GetLength();
		if (l==1)     Result.Format("XCP = %7.3f ", 	m_pCurWOpp->m_XCP);
		else if(l==2) Result.Format("XCP = %6.2f ", 	m_pCurWOpp->m_XCP);
		else if(l>=3) Result.Format("XCP = %5.2f ", 	m_pCurWOpp->m_XCP);
		Result += str;
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		Result.Format("Cm = %8.4f ", 	m_pCurWOpp->m_PitchingMoment);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);


		Result.Format("Rolling Moment coef. = %8.4f ", m_pCurWOpp->m_RollingMoment);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		Result.Format("Induced yawing moment coef. = %8.4f ", m_pCurWOpp->m_InducedYawingMoment);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		Result.Format("Airfoil yawing moment coef. = %8.4f ", m_pCurWOpp->m_GYm);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

	}
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
				if (pFrame->m_bXCP) PaintXCP(pDC, m_ptOffset,PtLegend, m_WingScale, false);
				if (pFrame->m_bXCmRef) PaintXCmRef(pDC, m_ptOffset, m_WingScale, false);
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

		Result.Format("Cm = %9.4f ", 	m_pCurWOpp->m_PitchingMoment);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		str1.LoadString(IDS_ROLLINGMOMENT);
		Result.Format(str1, m_pCurWOpp->m_RollingMoment);
		D+=12;
		pDC->TextOut(RightPos,ZPos+D, Result);

		str1.LoadString(IDS_INDUCEDMOMENT);
		Result.Format(str1, m_pCurWOpp->m_InducedYawingMoment);
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

		for(int i=0; i<m_pCurWOpp->m_nFlaps; i++){
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

	int k;
	
	m_WingGraph1.DeleteCurves();
	m_WingGraph2.DeleteCurves();
	m_WingGraph3.DeleteCurves();
	m_WingGraph4.DeleteCurves();

	if(m_bCurWOppOnly){
		if(!m_pCurWOpp || !m_pCurWOpp->m_bIsVisible) return;
		pCurve1    = m_WingGraph1.AddCurve();
		pCurve2    = m_WingGraph2.AddCurve();
		pCurve3    = m_WingGraph3.AddCurve();
		pCurve4    = m_WingGraph4.AddCurve();
		if(m_pCurWOpp->m_bShowPoints){
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
			for (int i=nStart; i<m_pCurWOpp->m_NStation; i++){
				x = m_pCurWOpp->m_SpanPos[i];
				y = maxlift*sqrt(1.0-x*x/m_pCurWOpp->m_Span/m_pCurWOpp->m_Span*4.0);
				pCurve->AddPoint(x*pFrame->m_mtoUnit,y);
			}
		}
		if(m_WOppVar2==3){
			CCurve *pCurve = m_WingGraph2.AddCurve();
			pCurve->SetStyle(PS_DASH);
			pCurve->SetColor(RGB(150, 150, 150));
			for (int i=nStart; i<m_pCurWOpp->m_NStation; i++){
				x = m_pCurWOpp->m_SpanPos[i];
				y = maxlift*sqrt(1.0-x*x/m_pCurWOpp->m_Span/m_pCurWOpp->m_Span*4.0);
				pCurve->AddPoint(x*pFrame->m_mtoUnit,y);
			}
		}
		if(m_WOppVar3==3){
			CCurve *pCurve = m_WingGraph3.AddCurve();
			pCurve->SetStyle(PS_DASH);
			pCurve->SetColor(RGB(150, 150, 150));
			for (int i=nStart; i<m_pCurWOpp->m_NStation; i++){
				x = m_pCurWOpp->m_SpanPos[i];
				y = maxlift*sqrt(1.0-x*x/m_pCurWOpp->m_Span/m_pCurWOpp->m_Span*4.0);
				pCurve->AddPoint(x*pFrame->m_mtoUnit,y);
			}
		}
		if(m_WOppVar4==3){
			CCurve *pCurve = m_WingGraph4.AddCurve();
			pCurve->SetStyle(PS_DASH);
			pCurve->SetColor(RGB(150, 150, 150));
			for (int i=nStart; i<m_pCurWOpp->m_NStation; i++){
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
	int nStart;
	if(pWOpp->m_AnalysisType==1) nStart = 1;
	else nStart = 0;
	
	switch(Var){
		case 0:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_Ai[i]);
			}
			pGraph->SetYTitle("Induced Angle");
			break;
		}
		case 1:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, 
					pWOpp->m_Alpha + pWOpp->m_Ai[i] + pWOpp->m_Twist[i]);
			}
			pGraph->SetYTitle("Total Angle");
			break;
		}
		case 2:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_Cl[i]);
			}
			pGraph->SetYTitle("Cl");
			break;
		}
		case 3:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_Cl[i] * pWOpp->m_Chord[i]/pWOpp->m_MAChord);
			}
			pGraph->SetYTitle("Local lift");
			break;
		}
		case 4:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_PCd[i]);
			}
			pGraph->SetYTitle("Airfoil drag");
			break;
		}
		case 5:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_ICd[i]);
			}
			pGraph->SetYTitle("Induced drag");
			break;
		}
		case 6:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_PCd[i]+ pWOpp->m_ICd[i]);
			}
			pGraph->SetYTitle("Total drag");
			break;
		}
		case 7:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, (pWOpp->m_PCd[i]+ pWOpp->m_ICd[i])* pWOpp->m_Chord[i]/pWOpp->m_MAChord);
			}
			pGraph->SetYTitle("Local drag");
			break;
		}
		case 8:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_CmAirf[i]);
			}
			pGraph->SetYTitle("Cm Airfoil");
			break;
		}
		case 9:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_CmGeom[i]);
			}
			pGraph->SetYTitle("Cm Geom.");
			break;
		}
		case 10:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_Cm[i]);
			}
			pGraph->SetYTitle("Cm");
			break;
		}
		case 11:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_Re[i]);
			}
			pGraph->SetYTitle("Re");
			break;
		}
		case 12:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_XTrTop[i]);
			}
			pGraph->SetYTitle("Top Trans x-Pos %");
			break;
		}
		case 13:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_XTrBot[i]);
			}
			pGraph->SetYTitle("Bot Trans x-Pos %");
			break;
		}
		case 14:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit, pWOpp->m_XCPSpanRel[i]);
			}
			pGraph->SetYTitle("CP x-Pos %");
			break;
		}
		case 15:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
				pCurve->AddPoint(pWOpp->m_SpanPos[i]*pFrame->m_mtoUnit,
					pWOpp->m_BendingMoment[i] * pFrame->m_NmtoUnit);
			}
			CString str;
			GetMomentUnit(str, pFrame->m_MomentUnit);
			pGraph->SetYTitle("BM (" + str + ")" );
			break;
		}
		default:{
			for (int i=nStart; i<pWOpp->m_NStation; i++){
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
	//Switch all WOpps view to on
	m_bCurWOppOnly = false;		
	CMenu *pMenu = m_pFrame->GetMenu();
	pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWCURWOPP, MF_BYCOMMAND | MF_UNCHECKED);
	CWOpp *pWOpp;
	CPOpp *pPOpp;
	for (int i=0; i< m_poaWOpp->GetSize(); i++){
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
	CreateWOppCurves();
	UpdateView();	
}

void CMiarex::OnShowAllWOpps() 
{
	//Switch all WOpps view to on for the current UFO and WPolar
	m_bCurWOppOnly = false;		
	CMenu *pMenu = m_pFrame->GetMenu();
	pMenu->GetSubMenu(4)->CheckMenuItem(IDM_SHOWCURWOPP, MF_BYCOMMAND | MF_UNCHECKED);
	CPOpp *pPOpp;
	CWOpp *pWOpp;
	for (int i=0; i< m_poaWOpp->GetSize(); i++){
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
	CreateWOppCurves();
	UpdateView();
}



void CMiarex::OnHideWingOpps() 
{
	//Switch all WOpps view to off for the current UFO 
	CPOpp *pPOpp;
	CWOpp *pWOpp;
	for (int i=0; i< m_poaWOpp->GetSize(); i++){
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
	CreateWOppCurves();
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
	for (int i=0; i< m_poaWOpp->GetSize(); i++){
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
	CreateWOppCurves();
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(false);
	pFrame->m_PolarDlgBar.SetParams();
	UpdateView();	
}


void CMiarex::OnHideWOpps() 
{
	CPOpp *pPOpp;
	CWOpp *pWOpp;
	for (int i=0; i< m_poaWOpp->GetSize(); i++){
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(i);
		pWOpp->m_bIsVisible = false;
	}
	for (i=0; i< m_poaPOpp->GetSize(); i++){
		pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
		pPOpp->m_bIsVisible = false;
	}
	CreateWOppCurves();
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(false);
	pFrame->m_PolarDlgBar.SetParams();
	UpdateView();
	
}


void CMiarex::OnHideAllWOpps() 
{
	CPOpp *pPOpp;
	CWOpp *pWOpp;
	for (int i=0; i< m_poaWOpp->GetSize(); i++){
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
	CreateWOppCurves();
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
	if(m_iView==1) CreateWOppCurves();
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
//	pFrame->SetSaveState(false);
	pFrame->m_PolarDlgBar.SetParams();
	UpdateView();
}



void CMiarex::OnAdvSettings() 
{
	CWAdvDlg dlg;
	dlg.m_AlphaPrec       = m_CvPrec;
	dlg.m_Iter            = m_Iter;
	dlg.m_Relax           = m_Relax;
	dlg.m_NStation        = m_NStation;
	dlg.m_WakePanelFactor = m_WakePanelFactor;
	dlg.m_WakePanels      = m_NXWakePanels;
	dlg.m_FirstPanelSize  = m_FirstPanelSize;
	dlg.m_MaxWakeIter     = m_MaxWakeIter;
	dlg.m_CoreSize        = m_CoreSize;
	dlg.m_bResetWake      = m_bResetWake;
	dlg.m_WakeInterNodes  = m_WakeInterNodes;
	dlg.m_BLogFile        = m_bLogFile;
	dlg.m_MinPanelSize    = m_MinPanelSize;
	dlg.m_ControlPos      = m_Panel[0].m_CtrlPos;
	dlg.m_VortexPos       = m_Panel[0].m_VortexPos;
	dlg.m_pFrame          = m_pFrame;
	dlg.m_pMiarex         = this;

	if(dlg.DoModal() == IDOK){
		m_CvPrec          = dlg.m_AlphaPrec;
		m_Iter            = dlg.m_Iter;
		m_Relax           = dlg.m_Relax;
		m_NStation        = dlg.m_NStation;
		m_WakePanelFactor = dlg.m_WakePanelFactor;
		m_NXWakePanels    = dlg.m_WakePanels;
		m_FirstPanelSize  = dlg.m_FirstPanelSize;
		m_MaxWakeIter     = dlg.m_MaxWakeIter;
		m_CoreSize        = dlg.m_CoreSize;
		m_bResetWake      = dlg.m_bResetWake;
		m_WakeInterNodes  = dlg.m_WakeInterNodes;
		m_MinPanelSize    = dlg.m_MinPanelSize;
		m_Panel[0].m_CtrlPos   = dlg.m_ControlPos;
		m_Panel[0].m_VortexPos = dlg.m_VortexPos;

		if(dlg.m_BLogFile) m_bLogFile = true; else m_bLogFile = false;
		m_bResetglWake    = true;
		SetUFO();
		UpdateView();
	}
}


void CMiarex::OnHideAllWPlrs() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CWPolar *pWPolar;
	for (int i=0; i<m_poaWPolar->GetSize(); i++){
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
	for (int i=0; i<m_poaWPolar->GetSize(); i++){
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

	CString UFOName;
	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CWPolar *pWPolar;
	for (int i=0; i<m_poaWPolar->GetSize(); i++){
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

	CString UFOName;
	if(m_pCurPlane)     UFOName = m_pCurPlane->m_PlaneName;
	else if(m_pCurWing) UFOName = m_pCurWing->m_WingName;
	else return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CWPolar *pWPolar;
	for (int i=0; i<m_poaWPolar->GetSize(); i++){
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
	Set2DScale();
	Set3DScale();
}

void CMiarex::Set2DScale()
{
//	AfxMessageBox("2D scale");
	if(m_bIs2DScaleSet && !m_bAutoScales) return;
	CW3DBar* pW3DBar = (CW3DBar*)m_pW3DBar;
	
	if(m_bHalfWing) m_ptOffset.x = m_rDrawRect.left + m_WingGraph1.GetMargin();
	else            m_ptOffset.x = (int)(m_rCltRect.Width()/2.0);
	m_ptOffset.y = m_rCltRect.bottom-185;


	if(m_pCurWing){
		m_WingScale = (m_rDrawRect.Width()-2*m_WingGraph1.GetMargin())/m_pCurWing->m_Span;
		if(m_bHalfWing) 	m_WingScale *= 2.0;
		m_bIs2DScaleSet = true;
	}
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
	int res, i, j;
	bool bSymetric;
	double a;

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
			for (int l=0; l<m_poaWing->GetSize(); l++){
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
					pWing->m_pMiarex = this;
					pWing->m_bSymetric = bSymetric;
					bRead = AVLReadSurface(&XFile, Line, pWing, NSpan, NChord, Sspace);
					if (bRead) {
						for(int i=0; i<=pWing->m_NPanel;i++){
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
							for(int i=0; i<pWing->m_NPanel;i++){
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

								for (int m=0; m<=pWing->m_NPanel; m++){
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
			int vlmpanels = 0;
			for (l=0; l<m_pCurWing->m_NPanel; l++){
				spanpos += m_pCurWing->m_NYPanels[l];
				vlmpanels +=m_pCurWing->m_NXPanels[l]*m_pCurWing->m_NYPanels[l];
			}

			if(spanpos*2>=MAXSTATIONS || vlmpanels*2 >=VLMMATSIZE){
				//repanel
				m_pCurWing->VLMSetAutoMesh();				
			}
			
			CMainFrame *pFrame = (CMainFrame*)m_pFrame;
			pFrame->SetSaveState(false);
			SetUFO();
			UpdateUFOs();
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
	int res;
	int index;
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
		for (int i=0; i<=pWing->m_NPanel; i++){
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
	if(!m_pCurWOpp || !m_pCurWPolar) {
		m_bAnimate = false;
		return;
	}

	CWOpp* pWOpp;

	if(bAnimate) {
		for (int l=0; l< m_poaWOpp->GetSize(); l++){
			pWOpp = (CWOpp*)m_poaWOpp->GetAt(l);
			if (pWOpp->m_PlrName  == m_pCurWPolar->m_PlrName &&
				pWOpp->m_WingName == m_pCurWing->m_WingName) {
					if(m_pCurWOpp->m_Alpha - pWOpp->m_Alpha<0.0001) 
						m_posAnimate = l-1;
			}
		}
		m_bAnimate  = bAnimate;
	}
	else  {
		m_bAnimate = bAnimate;
		if(m_posAnimate<0 || m_posAnimate>=m_poaWOpp->GetSize()) return;
		pWOpp = (CWOpp*)m_poaWOpp->GetAt(m_posAnimate);
		SetWOpp(false, pWOpp->m_Alpha);
		UpdateWOpps();
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
	SetWOpp(false, pWOpp->m_Alpha);

	if(m_pCurWOpp){
		CString str;
		if(m_pCurWPolar->m_Type != 4) str.Format("%8.2f", m_pCurWOpp->m_Alpha);
		else  str.Format("%8.2f", m_pCurWOpp->m_QInf);

		int pos = pFrame->m_ctrlWOpp.FindStringExact(-1,str);
		if(pos != CB_ERR) {
			pFrame->m_ctrlWOpp.SetCurSel(pos);
		}
		else {
			pFrame->m_ctrlWOpp.SetCurSel(0);
		}
	}
	else{
		pFrame->m_ctrlWOpp.SetCurSel(0);
	}
}

LRESULT CMiarex::KickIdle()
{	
	if(m_bAnimate){
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

		if(m_bAnimatePlus){
			m_posAnimate++;
			if (m_posAnimate >= size){
				m_posAnimate = size-2;
				m_bAnimatePlus = false;
			}
		}
		else{
			m_posAnimate--;
			if (m_posAnimate <0){
				m_posAnimate = 1;
				m_bAnimatePlus = true;
			}
		}

		if(m_posAnimate<0 || m_posAnimate>=size) return 0;
		if(m_pCurPlane){
			pPOpp = (CPOpp*)m_poaPOpp->GetAt(m_posAnimate);
			if(!pPOpp) return 0;
		}
		else{
			pWOpp = (CWOpp*)m_poaWOpp->GetAt(m_posAnimate);
			if(!pWOpp) return 0;
		}

		if(m_pCurPlane) 
			IsValid =(pPOpp->m_PlrName   == m_pCurWPolar->m_PlrName &&
					  pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName);
		else 
			IsValid =(pWOpp->m_PlrName  == m_pCurWPolar->m_PlrName &&
					  pWOpp->m_WingName == m_pCurWing->m_WingName);

		if (IsValid){
			
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
			CreateWOppCurves();

			CDC *pDC = pChildView->GetDC();
			if(!pDC) return 0;
			if(m_iView==3){
				m_bResetglOpp      = true;
				m_bResetglDownwash = true;
				m_bResetglLift     = true;
				m_bResetglDrag     = true;
				m_bResetglWake     = true;
				GLDraw3D(pDC);
			}
			else {
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
//	double angle;//radian
//	double endx, endz, dx, dz,xae, zae;

	double r,g,b;
	CW3DBar *pW3DBar   = (CW3DBar*)m_pW3DBar;

	glNewList(VLMLIFTFORCE, GL_COMPILE);
		m_GLList++;
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		color = pFrame->m_XCPColor;
		style = pFrame->m_XCPStyle;
		width = pFrame->m_XCPWidth;
		GetBWColor(color, style, width);

		if(style == PS_DOT) 
			glLineStipple (1, 0x0101);
		else if(style == PS_DASH) 
			glLineStipple (1, 0x00FF);
		else if(style == PS_DASHDOT) 
			glLineStipple (1, 0x1C47);
		else  
			glLineStipple (1, 0xFFFF);// Solid

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);
		glLineWidth((GLfloat)GetPenWidth(pFrame->m_XCPWidth, false));

		//Resulting force vector
		glLineWidth((GLfloat)(GetPenWidth(width, false)*2.0));

		double force = 0.5*m_pCurWPolar->m_Density * m_pCurWing->m_Area
						  *m_pCurWOpp->m_QInf*m_pCurWOpp->m_QInf
						  *m_pCurWOpp->m_CL;
	
		force *= pFrame->m_LiftScale/100.0;

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
			glVertex3d(glx+forcex+0.008, gly+0.008, forcez-0.012*sign*0.01/pW3DBar->m_glScalef);
		glEnd();

		glBegin(GL_LINES);
			glVertex3d(glx+forcex, gly, forcez);
			glVertex3d(glx+forcex-0.008, gly-0.008, forcez-0.012*sign*0.01/pW3DBar->m_glScalef);
		glEnd();

	glEndList();
}

void CMiarex::GLCreateMoments()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	int style, width;
	COLORREF color;

	double sign, amp, radius;
	double angle;//radian
	double endx, endy, endz, dx, dy, dz,xae, yae, zae;
	double factor = 10.0;
	double r,g,b;
	CW3DBar *pW3DBar   = (CW3DBar*)m_pW3DBar;

	glNewList(VLMMOMENTS, GL_COMPILE);
		m_GLList++;
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		color = pFrame->m_MomentColor;
		style = pFrame->m_MomentStyle;
		width = pFrame->m_MomentWidth;
		GetBWColor(color, style, width);

		if(style == PS_DOT) 
			glLineStipple (1, 0x0101);
		else if(style == PS_DASH) 
			glLineStipple (1, 0x00FF);
		else if(style == PS_DASHDOT) 
			glLineStipple (1, 0x1C47);
		else  
			glLineStipple (1, 0xFFFF);// Solid

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);
		glLineWidth((GLfloat)GetPenWidth(pFrame->m_XCPWidth, false));
		//Resulting Pitching Moment Arc vector
		glLineWidth((GLfloat)(GetPenWidth(width, false)*2.0));

		amp = 0.5*m_pCurWPolar->m_Density * m_pCurWing->m_Area
						*m_pCurWOpp->m_QInf*m_pCurWOpp->m_QInf
						*m_pCurWOpp->m_PitchingMoment;
	
		amp *= pFrame->m_LiftScale*factor;
		
		radius= m_pCurWing->m_Span/4.0;

		if (amp>0.0) sign = -1.0; else sign = +1.0;

		glBegin(GL_LINE_STRIP);
			for (int i=0; i<=int(abs(amp)); i++){
				angle = sign*(double)i/500.0*pi;
				glVertex3d(radius*cos(angle),0.0,radius*sin(angle));
			}
		glEnd();

		endx = radius*cos(angle);
		endz = radius*sin(angle);

		dx = 0.03;
		dz = 0.03*sign;

		xae = (radius-dx)*cos(angle) +dz *sin(angle);
		zae = (radius-dx)*sin(angle) -dz *cos(angle);
		glBegin(GL_LINES);
			glVertex3d(endx, 0.0, endz);
			glVertex3d(xae,  0.0, zae);
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
						*m_pCurWOpp->m_RollingMoment;
	
		amp *= pFrame->m_LiftScale*factor;
		
		if (amp>0.0) sign = -1.0; else sign = 1.0;

		glBegin(GL_LINE_STRIP);
			for (int i=0; i<=int(abs(amp)); i++){
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
						*(m_pCurWOpp->m_GYm+m_pCurWOpp->m_InducedYawingMoment);
	
		amp *= pFrame->m_LiftScale*factor;
		
		if (amp>0.0) sign = 1.0; else sign = -1.0;

		glBegin(GL_LINE_STRIP);
			for (int i=0; i<=int(abs(amp)); i++){
				angle = sign*(double)i/500.0*pi;
				glVertex3d(-radius*cos(angle),-radius*sin(angle),0.0);
			}
		glEnd();

		endx = -radius*cos(angle);
		endy = -radius*sin(angle);

		dx =   0.03;
		dy =  -0.03*sign;

		xae = (-radius+dx)*cos(angle) +dy *sin(angle);
		yae = (-radius+dx)*sin(angle) -dy *cos(angle);
		glBegin(GL_LINES);
			glVertex3d(endx, endy, 0.0);
			glVertex3d(xae,  yae, 0.0);
		glEnd();

		xae = (-radius-dx)*cos(angle) +dy *sin(angle);
		yae = (-radius-dx)*sin(angle) -dy *cos(angle);
		glBegin(GL_LINES);
			glVertex3d(endx, endy, 0.0);
			glVertex3d(xae,  yae, 0.0);
		glEnd();
	glEndList();
}

void CMiarex::GLCreateLift(CWing *pWing, CWOpp *pWOpp, UINT List)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	int i,j,k;
	CVector C;

	int style, width;
	COLORREF color;
	
	double r,g,b;
	double amp;
	double yob, xt, yt, zt, dih;
	double cosa =  cos(pWOpp->m_Alpha * pi/180.0);
	double sina = -sin(pWOpp->m_Alpha * pi/180.0);

	//LIFTLINE
	glNewList(List,GL_COMPILE);
		m_GLList++;
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		color = pFrame->m_XCPColor;
		style = pFrame->m_XCPStyle;
		width = pFrame->m_XCPWidth;
		GetBWColor(color, style, width);

		if(style == PS_DOT) 
			glLineStipple (1, 0x0101);
		else if(style == PS_DASH) 
			glLineStipple (1, 0x00FF);
		else if(style == PS_DASHDOT) 
			glLineStipple (1, 0x1C47);
		else  
			glLineStipple (1, 0xFFFF);// Solid

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);
		glLineWidth((GLfloat)GetPenWidth(width, false));

		double q0 = 0.5*m_pCurWPolar->m_Density*m_pCurWing->m_Area*pWOpp->m_QInf*pWOpp->m_QInf;

		if(pWOpp){
			if(pWOpp->m_AnalysisType==1){
				for (i=1; i<pWOpp->m_NStation; i++){
					yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
					xt = pWing->GetOffset(yob) + pWOpp->m_XCPSpanRel[i]*pWOpp->m_Chord[i];
					pWing->GetViewYZPos(pWOpp->m_XCPSpanRel[i], pWOpp->m_SpanPos[i],
										yt,zt,0);
					dih = -pWing->GetDihedral(yob)*pi/180.0;
					amp = q0*pWOpp->m_Cl[i]*pWOpp->m_Chord[i]/pWOpp->m_MAChord;
					amp *= pFrame->m_LiftScale/100.0;

					glBegin(GL_LINES);
						glVertex3d(xt, yt, zt);
						glVertex3d((xt + amp * cos(dih)*sina),
									yt + amp * sin(dih),
									zt + amp * cos(dih)*cosa);
					glEnd();
				}
				glBegin(GL_LINE_STRIP);
					for (i=1; i<pWOpp->m_NStation; i++){
						yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
						xt = pWing->GetOffset(yob) + pWOpp->m_XCPSpanRel[i]*pWOpp->m_Chord[i];
						pWing->GetViewYZPos(pWOpp->m_XCPSpanRel[i], pWOpp->m_SpanPos[i],
										yt,zt,0);
		
						dih = -pWing->GetDihedral(yob)*pi/180.0;
						amp = q0*pWOpp->m_Cl[i]*pWOpp->m_Chord[i]/pWOpp->m_MAChord;
						amp *= pFrame->m_LiftScale/100.0;

						glVertex3d(xt + amp * cos(dih)*sina,
								   yt + amp * sin(dih),
								   zt + amp * cos(dih)*cosa);
					}
				glEnd();
			}
			else{
				i = 0;
				for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
					for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
						pWing->m_Surface[j].GetLeadingPt(C, k);
						amp = q0*pWOpp->m_Chord[i]/m_pCurWing->m_MAChord * pFrame->m_LiftScale/100.0;
						C.x = pWOpp->m_XCPSpanAbs[i];
						glBegin(GL_LINES);
							glVertex3d(C.x, C.y, C.z);
							glVertex3d(C.x + pWOpp->m_F[i].x*amp*cosa + pWOpp->m_F[i].z*amp*sina,
									   C.y + pWOpp->m_F[i].y*amp,
									   C.z + pWOpp->m_F[i].x*amp*sina + pWOpp->m_F[i].z*amp*cosa);
						glEnd();
						i++;
					}
				}
				if(!pWing->m_bIsFin){
					glBegin(GL_LINE_STRIP);
						i = 0;
						for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
							for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
								pWing->m_Surface[j].GetLeadingPt(C, k);
								amp = q0*pWOpp->m_Chord[i]/m_pCurWing->m_MAChord * pFrame->m_LiftScale/100.0;
								C.x = pWOpp->m_XCPSpanAbs[i];
								glVertex3d(C.x + pWOpp->m_F[i].x*amp*cosa + pWOpp->m_F[i].z*amp*sina,
										   C.y + pWOpp->m_F[i].y*amp,
										   C.z + pWOpp->m_F[i].x*amp*sina + pWOpp->m_F[i].z*amp*cosa);
								i++;
							}
						}
					glEnd();
				}
				else {
					i = 0;
					for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
						glBegin(GL_LINE_STRIP);
							for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
								pWing->m_Surface[j].GetLeadingPt(C, k);
								amp = q0*pWOpp->m_Chord[i]/m_pCurWing->m_MAChord * pFrame->m_LiftScale/100.0;
								C.x = pWOpp->m_XCPSpanAbs[i];
								glVertex3d(C.x + pWOpp->m_F[i].x*amp*cosa + pWOpp->m_F[i].z*amp*sina,
										   C.y + pWOpp->m_F[i].y*amp,
										   C.z + pWOpp->m_F[i].x*amp*sina + pWOpp->m_F[i].z*amp*cosa);
								i++;
							}
						glEnd();
					}
				}
			}
		}
		glDisable (GL_LINE_STIPPLE);
	glEndList();
}


void CMiarex::GLCreateDrag(CWing *pWing, CWOpp *pWOpp, UINT List)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	int i,j,k;
	CVector C;

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

	if(Istyle == PS_DOT)			IDash = 0x0101;
	else if(Istyle == PS_DASH)		IDash = 0x00FF;
	else if(Istyle == PS_DASHDOT) 	IDash = 0x1C47;
	else							IDash = 0xFFFF;// Solid

	if(Vstyle == PS_DOT)			VDash = 0x0101;
	else if(Vstyle == PS_DASH) 		VDash = 0x00FF;
	else if(Vstyle == PS_DASHDOT)	VDash = 0x1C47;
	else							VDash = 0xFFFF;// Solid

	DecompRGB(Icolor,Ir,Ig,Ib);
	DecompRGB(Vcolor,Vr,Vg,Vb);

	glLineStipple (1, IDash);// Solid
	glLineWidth((GLfloat)GetPenWidth(Iwidth, false));

	//DRAGLINE
	glNewList(List,GL_COMPILE);
		m_GLList++;
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);


		double q0 = 0.5*m_pCurWPolar->m_Density*m_pCurWing->m_Area*pWOpp->m_QInf*pWOpp->m_QInf;

		if(pWOpp){
			if(pWOpp->m_AnalysisType==1){
				for (i=1; i<pWOpp->m_NStation; i++){
					yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
					xt = pWing->GetOffset(yob) + pWOpp->m_Chord[i];
					pWing->GetViewYZPos(1.0, pWOpp->m_SpanPos[i],yt,zt,0);
					dih = pWing->GetDihedral(yob)*pi/180.0;
					amp1 = q0*pWOpp->m_ICd[i]*pWOpp->m_Chord[i]/pWOpp->m_MAChord*pFrame->m_DragScale/5.0;
					amp2 = q0*pWOpp->m_PCd[i]*pWOpp->m_Chord[i]/pWOpp->m_MAChord*pFrame->m_DragScale/5.0;
					if(pFrame->m_bICd){
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
					if(pFrame->m_bVCd){
						glColor3f((GLfloat)Vr,(GLfloat)Vg,(GLfloat)Vb);
						glLineStipple (1, VDash);// Solid
						glLineWidth((GLfloat)GetPenWidth(Vwidth, false));
						glBegin(GL_LINES);
							if(!pFrame->m_bICd){
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
				if(pFrame->m_bICd){
					glColor3f((GLfloat)Ir,(GLfloat)Ig,(GLfloat)Ib);
					glLineStipple (1, IDash);// Solid
					glLineWidth((GLfloat)GetPenWidth(Iwidth, false));
					glBegin(GL_LINE_STRIP);
						for (i=1; i<pWOpp->m_NStation; i++){
							yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
							xt = pWing->GetOffset(yob) + pWOpp->m_Chord[i];
							pWing->GetViewYZPos(1.0, pWOpp->m_SpanPos[i],yt,zt,0);
			
							dih = pWing->GetDihedral(yob)*pi/180.0;

							amp  = q0*pWOpp->m_ICd[i]*pWOpp->m_Chord[i]/pWOpp->m_MAChord;
							amp *= pFrame->m_DragScale/5.0;

							glVertex3d(xt + amp * cos(dih)*cosa,
									   yt,
									   zt - amp * cos(dih)*sina);
						}
					glEnd();
				}
				if(pFrame->m_bVCd){
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
							if(pFrame->m_bICd) amp+=pWOpp->m_ICd[i];
							amp +=pWOpp->m_PCd[i];
							amp *= q0*pWOpp->m_Chord[i]/pWOpp->m_MAChord;
							amp *= pFrame->m_DragScale/5.0;

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
						pWing->m_Surface[j].GetTrailingPt(C, k);
						amp1 = q0*pWOpp->m_ICd[i]*pWOpp->m_Chord[i]/m_pCurWing->m_MAChord*pFrame->m_DragScale/3.0;
						amp2 = q0*pWOpp->m_PCd[i]*pWOpp->m_Chord[i]/m_pCurWing->m_MAChord*pFrame->m_DragScale/3.0;
						if(pFrame->m_bICd){
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
						if(pFrame->m_bVCd){
							glColor3f((GLfloat)Vr,(GLfloat)Vg,(GLfloat)Vb);
							glLineStipple (1, VDash);// Solid
							glLineWidth((GLfloat)GetPenWidth(Vwidth, false));
							glBegin(GL_LINES);
								if(!pFrame->m_bICd){
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
					if(pFrame->m_bICd){
						glColor3f((GLfloat)Ir,(GLfloat)Ig,(GLfloat)Ib);
						glLineStipple (1, IDash);// Solid
						glLineWidth((GLfloat)GetPenWidth(Iwidth, false));
						glBegin(GL_LINE_STRIP);
							i = 0;
							for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
								for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
									pWing->m_Surface[j].GetTrailingPt(C, k);
									amp = q0*(pWOpp->m_ICd[i])*pWOpp->m_Chord[i]/m_pCurWing->m_MAChord;
									amp *= pFrame->m_DragScale/3.0;
									glVertex3d(C.x + amp*cosa,
											   C.y,
											   C.z - amp*sina);
									i++;
								}
							}
						glEnd();
					}
					if(pFrame->m_bVCd){
						glColor3f((GLfloat)Vr,(GLfloat)Vg,(GLfloat)Vb);
						glLineStipple (1, VDash);// Solid
						glLineWidth((GLfloat)GetPenWidth(Vwidth, false));
						glBegin(GL_LINE_STRIP);
							i = 0;
							for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
								for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
									pWing->m_Surface[j].GetTrailingPt(C, k);
									amp=0.0;
									if(pFrame->m_bICd) amp+=pWOpp->m_ICd[i];
									amp +=pWOpp->m_PCd[i];
									amp *= q0*pWOpp->m_Chord[i]/m_pCurWing->m_MAChord;
									amp *= pFrame->m_DragScale/3.0;

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
					if(pFrame->m_bICd){
						glColor3f((GLfloat)Ir,(GLfloat)Ig,(GLfloat)Ib);
						glLineStipple (1, IDash);// Solid
						glLineWidth((GLfloat)GetPenWidth(Iwidth, false));
						i = 0;
						for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
							glBegin(GL_LINE_STRIP);
								for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
									pWing->m_Surface[j].GetTrailingPt(C, k);
									amp = q0*(pWOpp->m_ICd[i])*pWOpp->m_Chord[i]/m_pCurWing->m_MAChord;
									amp *= pFrame->m_DragScale/3.0;
									glVertex3d(C.x + amp*cosa,
											   C.y,
											   C.z - amp*sina);
									i++;
								}
							glEnd();
						}
					}
					if(pFrame->m_bVCd){
						glColor3f((GLfloat)Vr,(GLfloat)Vg,(GLfloat)Vb);
						glLineStipple (1, VDash);// Solid
						glLineWidth((GLfloat)GetPenWidth(Vwidth, false));
						i = 0;
						for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
							glBegin(GL_LINE_STRIP);
								for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
									pWing->m_Surface[j].GetTrailingPt(C, k);
									amp=0.0;
									if(pFrame->m_bICd) amp+=pWOpp->m_ICd[i];
									amp +=pWOpp->m_PCd[i];
									amp *= q0*pWOpp->m_Chord[i]/m_pCurWing->m_MAChord;
									amp *= pFrame->m_DragScale/3.0;

									glVertex3d(C.x + amp*cosa,
											   C.y,
											   C.z - amp*sina);
									i++;
								}
							glEnd();
						}
					}
				}
			}
		}
		glDisable (GL_LINE_STIPPLE);
	glEndList();
}
/*
void CMiarex::GLCreateXFlow()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	int i,k;
	COLORREF color;
	int style, width;
	int p;
	double x1, x2, y1, y2, z1, z2, xe, ye, ze;
	CVector C, V, VT;
	CVector VInf;
	double factor;
	double r,g,b;
	CW3DBar *pW3DBar   = (CW3DBar*)m_pW3DBar;
	double x0, y0,z0;
	double dx, dz, dlx, dlz;
	double length, sinT, cosT, beta;
	double *Gamma;

	if(m_pCurPOpp)        Gamma = m_pCurPOpp->m_G;
	else if (m_pCurWOpp)  Gamma = m_pCurWOpp->m_G;
	else                  Gamma = NULL;

	//DOWNWASH
	glNewList(VLMSTREAMLINES,GL_COMPILE);
		m_GLList++;

		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		color = pFrame->m_WakeColor;
		style = pFrame->m_WakeStyle;
		width = pFrame->m_WakeWidth;

		glLineWidth((GLfloat)GetPenWidth(width, false));
		color = RGB(125,200,255);
		style = 0;
		width = 1;

		GetBWColor(color, style, width);

		if(style == PS_DOT) 
			glLineStipple (1, 0x0101);
		else if(style == PS_DASH) 
			glLineStipple (1, 0x00FF);
		else if(style == PS_DASHDOT) 
			glLineStipple (1, 0x1C47);
		else  
			glLineStipple (1, 0xFFFF);// Solid

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		factor = 1.0;
		if(Gamma){
			VInf.Set(m_pCurWOpp->m_QInf,0.0,0.0);
		
			x0  = m_XFlowDlg.m_xmin;
			y0  =    0.0;
			z0  = m_XFlowDlg.m_zmin;
			dx  = abs((m_XFlowDlg.m_xmax-m_XFlowDlg.m_xmin) /  m_XFlowDlg.m_nx);
			dz  = abs((m_XFlowDlg.m_zmax-m_XFlowDlg.m_zmin) /  m_XFlowDlg.m_nz);

			C.y = m_XFlowDlg.m_ypos*m_pCurWing->m_Span/2.0;

			for (i=0; i<m_XFlowDlg.m_nx; i++){
				C.x = x0 + i*dx;
				for (k=0; k< m_XFlowDlg.m_nz; k++){
					VT.Set(0.0,0.0,0.0);
					C.z = z0 + k*dz;

					for (p=0; p<m_VLMMatSize;p++){
						m_VLMDlg.VLMGetVortexInfluence(p, C, V);
						V  *= Gamma[p];
						VT += V;
					}
					
					VT += VInf;
					length = VT.VAbs()*factor;
					xe     = C.x+factor*VT.x;
					ye     = C.y+factor*VT.y;
					ze     = C.z+factor*VT.z;
					if(length>0.0){
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
				}
			}	
		}
		glDisable (GL_LINE_STIPPLE);
	glEndList();
}*/

void CMiarex::GLCreateSurfFlow()
{
	if(!m_pCurWOpp || m_pCurWOpp->m_AnalysisType==1) {
		glNewList(VLMFLOWS, GL_COMPILE);
		glEndList();
		return;
	}
	CWaitCursor wait;
	CMessageDlg dlg;
	dlg.Create(IDD_MESSAGEDLG,this);
	dlg.ShowWindow(SW_SHOW);
	dlg.SetMessage("Calculating surface flows...\nPlease wait");

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	COLORREF color;
	int style, width;
	int p, pp;
	double x1, x2, y1, y2, z1, z2, xe, ye, ze;
	CVector C, V, VT;

	CVector RefPoint(0.0,0.0,0.0);

	double factor;
	double r,g,b;
	CW3DBar *pW3DBar   = (CW3DBar*)m_pW3DBar;
	double dlx, dlz;
	double length, sinT, cosT, beta;
	double *Gamma;

	if(m_pCurPOpp)        Gamma = m_pCurPOpp->m_G;
	else if (m_pCurWOpp)  Gamma = m_pCurWOpp->m_G;
	else                  Gamma = NULL;

	glNewList(VLMFLOWS, GL_COMPILE);
		m_GLList++;

		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		color = pFrame->m_WakeColor;
		style = pFrame->m_WakeStyle;
		width = pFrame->m_WakeWidth;

		glLineWidth((GLfloat)GetPenWidth(width, false));
		GetBWColor(color, style, width);

		if(style == PS_DOT)          glLineStipple (1, 0x0101);
		else if(style == PS_DASH)    glLineStipple (1, 0x00FF);
		else if(style == PS_DASHDOT) glLineStipple (1, 0x1C47);
		else						 glLineStipple (1, 0xFFFF);// Solid

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		factor = 2.0;
		if(Gamma){
		
			for (p=0; p<m_VLMMatSize; p++){

				VT.Set(m_pCurWOpp->m_QInf,0.0,0.0);

				if(m_pCurWPolar->m_AnalysisType==2)	{
					C.Copy(m_Panel[p].CtrlPt);
					if(m_pCurWPolar->m_bTiltedGeom)	C.RotateY(RefPoint, m_pCurWOpp->m_Alpha);
					for (pp=0; pp<m_VLMMatSize;pp++){
						m_VLMDlg.VLMGetVortexInfluence(pp, C, V, true);
						V  *= Gamma[pp];
						VT += V;
					}
					VT *= pFrame->m_DownwashScale/100.0;
					if(!m_pCurWPolar->m_bTiltedGeom)	C.RotateY(RefPoint, m_pCurWOpp->m_Alpha);
				}
				else if (m_pCurWPolar->m_AnalysisType==3){
					CVector Vt;
					C.Copy(m_Panel[p].CollPt);
					C.RotateY(RefPoint, m_pCurWOpp->m_Alpha);

					VT = m_PanelDlg.m_Speed[p]*pFrame->m_DownwashScale/100.0;
				}
			
				length = VT.VAbs()*factor;
				xe     = C.x+factor*VT.x;
				ye     = C.y+factor*VT.y;
				ze     = C.z+factor*VT.z;
				if(length>0.0){
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
			}	
		}
		glDisable (GL_LINE_STIPPLE);
	glEndList();
}


void CMiarex::GLCreateStreamLines()
{
	CWaitCursor wait;
	CMessageDlg dlg;
	dlg.Create(IDD_MESSAGEDLG,this);
	dlg.m_bCancel = false;
	dlg.ShowWindow(SW_SHOW);
	dlg.SetMessage("Calculating Streamlines...\nPlease wait");
	dlg.SetProgress(0);

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	bool bFound;
	int i, p, style, width;
	int pp=0;
	int method = 3;
	double r,g,b, dx, *Gamma, *Mu, *Sigma;
	double Sn, Sn1, Sn2;
	COLORREF color;

	CVector C, V, VT, VInf;
	CVector RefPoint(0.0,0.0,0.0);
	CW3DBar *pW3DBar   = (CW3DBar*)m_pW3DBar;
	CVector t, Tn, Tn1, Tn2;
	CVector Rn, Rnp1;
	CVector RP;

	if(m_pCurPOpp)      {
		Gamma = m_pCurPOpp->m_G;
	}
	else if (m_pCurWOpp)  {
		Gamma = m_pCurWOpp->m_G;
		Mu    = m_pCurWOpp->m_G;
		Sigma = m_pCurWOpp->m_Sigma;
	}
	else {
		Gamma = NULL;
		Mu    = NULL;
		Sigma = NULL;
	}

	glNewList(VLMSTREAMLINES,GL_COMPILE);
		m_GLList++;

		glEnable (GL_LINE_STIPPLE);

		color = pFrame->m_WakeColor;
		style = pFrame->m_WakeStyle;
		width = pFrame->m_WakeWidth;

		GetBWColor(color, style, width);
		glLineWidth((GLfloat)GetPenWidth(width, false));

		if(style == PS_DOT) 			glLineStipple (1, 0x0101);
		else if(style == PS_DASH) 		glLineStipple (1, 0x00FF);
		else if(style == PS_DASHDOT) 	glLineStipple (1, 0x1C47);
		else  							glLineStipple (1, 0xFFFF);// Solid
		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		if(Gamma){
			VInf.Set(m_pCurWOpp->m_QInf,0.0,0.0);

			memcpy(m_VLMDlg.m_pPanel,   m_VLMDlg.m_pRefPanels,   sizeof(m_VLMDlg.m_pPanel));
			memcpy(m_VLMDlg.m_pNode,    m_VLMDlg.m_pRefNodes,    sizeof(m_VLMDlg.m_pNode));
//			memcpy(m_VLMDlg.m_pSurface, m_VLMDlg.m_pRefSurfaces, sizeof(m_VLMDlg.m_pSurface));
			for (int l=0; l<m_VLMDlg.m_NSurfaces; l++)
				m_VLMDlg.m_pSurface[l].Copy(m_VLMDlg.m_pRefSurfaces[l]);

			m_VLMDlg.VLMRotateGeomY(m_pCurWOpp->m_Alpha, RefPoint);

			for (p=0; p<m_VLMMatSize; p++){
				bFound = false;
				if(m_FlowLinesDlg.m_bLE && m_Panel[p].m_bIsLeading){
					C.x =(m_Node[m_Panel[p].m_iLA].x + m_Node[m_Panel[p].m_iLB].x)/2.0;
					C.y =(m_Node[m_Panel[p].m_iLA].y + m_Node[m_Panel[p].m_iLB].y)/2.0;
					C.z =(m_Node[m_Panel[p].m_iLA].z + m_Node[m_Panel[p].m_iLB].z)/2.0;
					bFound = true;
				}
				else if(!m_FlowLinesDlg.m_bLE && m_Panel[p].m_bIsTrailing){
					C.x =(m_Node[m_Panel[p].m_iTA].x + m_Node[m_Panel[p].m_iTB].x)/2.0;
					if(m_pCurWPolar->m_AnalysisType==3){
						C.x += m_Panel[p].Size/10.0;
					}
					C.y =(m_Node[m_Panel[p].m_iTA].y + m_Node[m_Panel[p].m_iTB].y)/2.0;
					C.z =(m_Node[m_Panel[p].m_iTA].z + m_Node[m_Panel[p].m_iTB].z)/2.0;
					bFound = true;
				}
				if(bFound){
					C.x += m_FlowLinesDlg.m_XOffset;
//					C.z += m_FlowLinesDlg.m_ZOffset;
					C.x += m_Panel[p].Normal.x *  m_FlowLinesDlg.m_ZOffset;
					C.y += m_Panel[p].Normal.y *  m_FlowLinesDlg.m_ZOffset;
					C.z += m_Panel[p].Normal.z *  m_FlowLinesDlg.m_ZOffset;
					C.RotateY(RefPoint, m_pCurWOpp->m_Alpha);
					dx = m_FlowLinesDlg.m_DeltaL;
					switch(method){
						case 1:{
							glBegin(GL_LINE_STRIP);
								glVertex3d(C.x, C.y, C.z);

								for (i=0; i< m_FlowLinesDlg.m_NX ;i++){
									VT.Set(0.0,0.0,0.0);
									for (pp=0; pp<m_VLMMatSize;pp++){
										m_VLMDlg.VLMGetVortexInfluence(pp, C, V, true);
										VT += V* Gamma[pp];
									}
									
									VT += VInf;
									VT.Normalize();
									C   += VT* dx;
									glVertex3d(C.x, C.y, C.z);
									dx *=m_FlowLinesDlg.m_XFactor;
								}

							glEnd();
							break;
						}
						case 2:{
							glBegin(GL_LINE_STRIP);
								glVertex3d(C.x, C.y, C.z);
								
								for (i=0; i< m_FlowLinesDlg.m_NX ;i++){
									if(i==0){//initialize sequence
										VT = m_VLMDlg.GetSpeedVector(C, Gamma);
										VT += VInf;
										VT.Normalize();
										C   += VT* dx;
										Rnp1 = C;
										Tn   = VT; Tn1  = Tn;
										Sn   = dx; Sn1  = Sn;
										glVertex3d(C.x, C.y, C.z);

									}
									else {
										Rn  = Rnp1;
										Tn2 = Tn1; Tn1 = Tn;
										Sn2 = Sn1; Sn1 = Sn; Sn=dx;

										t  = (Tn1 - Tn2) * (Sn+Sn1)/(Sn1+Sn2) + Tn1;
										RP = Rn + t * 0.5 * Sn;

										Tn = m_VLMDlg.GetSpeedVector(RP, Gamma);				
										Tn += VInf;
										Tn.Normalize();

										Rnp1 = Rn + Tn * Sn;

										glVertex3d(Rnp1 .x, Rnp1 .y, Rnp1 .z);
									}
									dx *=m_FlowLinesDlg.m_XFactor;
								}

							glEnd();
							break;
						}
						case 3:{
							glBegin(GL_LINE_STRIP);
								glVertex3d(C.x, C.y, C.z);
								
								for (i=0; i< m_FlowLinesDlg.m_NX ;i++){
									if(i==0){//initialize sequence
										if(m_pCurWPolar->m_AnalysisType==2)
											VT = m_VLMDlg.GetSpeedVector(C, Gamma);
										else if(m_pCurWPolar->m_AnalysisType==3)
											VT = m_PanelDlg.GetSpeedVector(C, Mu, Sigma);
										VT += VInf;
										VT.Normalize();
										C   += VT* dx;
										Rnp1 = C;
										Tn   = VT; Tn1  = Tn;
										Sn   = dx; Sn1  = Sn;
										glVertex3d(C.x, C.y, C.z);

									}
									else {
										Rn  = Rnp1;
										Rnp1 = Rn + Tn *dx;
										C = (Rn+Rnp1)/2.0;
										Tn = m_VLMDlg.GetSpeedVector(C, Gamma);				
										if(m_pCurWPolar->m_AnalysisType==2)
											Tn = m_VLMDlg.GetSpeedVector(C, Gamma);
										else if(m_pCurWPolar->m_AnalysisType==3)
											Tn = m_PanelDlg.GetSpeedVector(C, Mu, Sigma);
										Tn += VInf;
										Tn.Normalize();
										Rnp1 = Rn + Tn *dx;

										TRACE("%10.5e    %10.5e    %10.5e    %10.5e\n",Rnp1.x, Tn.x, Tn.y, Tn.z);
										glVertex3d(Rnp1.x, Rnp1.y, Rnp1.z);
									}
									dx *=m_FlowLinesDlg.m_XFactor;
								}
							glEnd();
							break;
						}
					}
					dlg.SetProgress((int)(100*((double)p/(double)m_VLMMatSize)));
					if(dlg.m_bCancel) break;
					TRACE("\n\n");
				}
			}	
		}
		glDisable (GL_LINE_STIPPLE);
	glEndList();
	dlg.ShowWindow(SW_HIDE);
}


void CMiarex::GLCreateDownwash(CWing *pWing, CWOpp *pWOpp, UINT List, int surf0)
{
	// pWing is either the Wing, the stab, or the fin
	// pWOpp is related to the pWing
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	int i,j,k;
	COLORREF color;
	int style, width;
	int p;
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
		m_GLList++;

		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glLineWidth((GLfloat)GetPenWidth(pFrame->m_DownwashWidth, false));

		color = pFrame->m_DownwashColor;
		style = pFrame->m_DownwashStyle;
		width = pFrame->m_DownwashWidth;
		GetBWColor(color, style, width);

		if(style == PS_DOT) 
			glLineStipple (1, 0x0101);
		else if(style == PS_DASH) 
			glLineStipple (1, 0x00FF);
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
				glEnd();
			}
			else{
				factor = pFrame->m_DownwashScale*pWOpp->m_QInf/100.0;

				p = 0;
				i = 0;
				for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
					for (k=0; k< pWing->m_Surface[j].m_NYPanels; k++){
						m_Surface[j+surf0].GetTrailingPt(C, k);
						if (pWOpp->m_Vd[i].z>0) sign = 1.0; else sign = -1.0;
						glBegin(GL_LINES);
							glVertex3d(C.x, C.y, C.z);
							glVertex3d(C.x+factor*pWOpp->m_Vd[i].z * sina,
									   C.y+factor*pWOpp->m_Vd[i].y,
									   C.z+factor*pWOpp->m_Vd[i].z * cosa);
	 
						glEnd();
						xs = C.x+factor*pWOpp->m_Vd[i].z*sina;
						ys = C.y+factor*pWOpp->m_Vd[i].y;
						zs = C.z+factor*pWOpp->m_Vd[i].z*cosa;
						y1 = ys - 0.085*factor*pWOpp->m_Vd[i].y      + 0.05*factor*pWOpp->m_Vd[i].z*cosa;
						z1 = zs - 0.085*factor*pWOpp->m_Vd[i].z*cosa - 0.05*factor*pWOpp->m_Vd[i].y;
						y2 = ys - 0.085*factor*pWOpp->m_Vd[i].y      - 0.05*factor*pWOpp->m_Vd[i].z*cosa;
						z2 = zs - 0.085*factor*pWOpp->m_Vd[i].z*cosa + 0.05*factor*pWOpp->m_Vd[i].y;
			
						glBegin(GL_LINES);
							glVertex3d(xs, ys, zs);
							glVertex3d(xs, y1, z1);

						glEnd();
						glBegin(GL_LINES);
							glVertex3d(xs, ys, zs);
							glVertex3d(xs, y2, z2);
						glEnd();

						i++;
					}
					p++;
				}
			}
		}
		glDisable (GL_LINE_STIPPLE);
	glEndList();
}

void CMiarex::GLCreateCp()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	int i,p,pp, average, NodeA, NodeB;
	int nPanels;

	CVector LA,LB,TA,TB, Pt;
	CVector C,N;
	double color, lmin, lmax, range;
	double *tab;

	glNewList(VLMCL,GL_COMPILE);
		m_GLList++;
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		if(m_pCurWOpp){
			if(m_pCurPOpp){
				tab = m_pCurPOpp->m_Cp;
				nPanels = m_pCurPOpp->m_NVLMPanels;
				if(pFrame->m_bAutoCpScale){
					pFrame->m_LegendMin =  10000.0;
					pFrame->m_LegendMax = -10000.0;
					for (i=0; i< nPanels; i++){
						pFrame->m_LegendMin = min(pFrame->m_LegendMin, tab[i]);
						pFrame->m_LegendMax = max(pFrame->m_LegendMax, tab[i]);
					}
				}
			}
			else{
				tab = m_pCurWOpp->m_Cp;
				nPanels = m_pCurWOpp->m_NVLMPanels;
				if(pFrame->m_bAutoCpScale){
					pFrame->m_LegendMin =  10000.0;
					pFrame->m_LegendMax = -10000.0;
					for (i=0; i< nPanels; i++){
						pFrame->m_LegendMin = min(pFrame->m_LegendMin, tab[i]);
						pFrame->m_LegendMax = max(pFrame->m_LegendMax, tab[i]);
					}
				}
			}
			lmin = pFrame->m_LegendMin;
			lmax = pFrame->m_LegendMax;
			range = lmax - lmin;

			glLineWidth(1.0);
			p=0;
			for (p=0; p<m_VLMMatSize; p++){
				glBegin(GL_QUADS);
					TA.Copy(m_Node[m_Panel[p].m_iTA]);
					TB.Copy(m_Node[m_Panel[p].m_iTB]);
					LA.Copy(m_Node[m_Panel[p].m_iLA]);
					LB.Copy(m_Node[m_Panel[p].m_iLB]);

					NodeA = IsNode(LA);
					color = 0.0;
					average = 0;
					for (pp=0; pp< nPanels; pp++){
						if (m_Panel[pp].m_iLA==NodeA ||	
							m_Panel[pp].m_iLB==NodeA ||
							m_Panel[pp].m_iTA==NodeA ||
							m_Panel[pp].m_iTB==NodeA){
							if(VLMIsSameSide(p,pp)){
								color +=tab[pp];
								average++;
							}
						}
					}
					
					color = (color/average-lmin)/range;
					if(IsBlackAndWhite())	glColor3d(color,color,color);
					else					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
					glVertex3d(LA.x, LA.y, LA.z);

					NodeB = IsNode(LB);
					color = 0.0;
					average = 0;
					for (pp=0; pp< nPanels; pp++){
						if (m_Panel[pp].m_iLA==NodeB ||
							m_Panel[pp].m_iLB==NodeB ||
							m_Panel[pp].m_iTA==NodeB ||
							m_Panel[pp].m_iTB==NodeB){
							if(VLMIsSameSide(p,pp)){
								color +=tab[pp];
								average++;
							}
						}
					}
					color = (color/average-lmin)/range;
					if(IsBlackAndWhite())	glColor3d(color,color,color);
					else					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
					glVertex3d(LB.x, LB.y, LB.z);
					
					NodeA = IsNode(TB);
					color = 0.0;
					average = 0;
					for (pp=0; pp< nPanels; pp++){
						if (m_Panel[pp].m_iLA==NodeA ||	
							m_Panel[pp].m_iLB==NodeA ||
							m_Panel[pp].m_iTA==NodeA ||
							m_Panel[pp].m_iTB==NodeA){
							if(VLMIsSameSide(p,pp)){
								color +=tab[pp];
								average++;
							}
						}
					}
					
					color = (color/average-lmin)/range;
					if(IsBlackAndWhite())	glColor3d(color,color,color);
					else					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
					glVertex3d(TB.x, TB.y, TB.z);

					NodeB = IsNode(TA);
					color = 0.0;
					average = 0;
					for (pp=0; pp< nPanels; pp++){
						if (m_Panel[pp].m_iLA==NodeB ||
							m_Panel[pp].m_iLB==NodeB ||
							m_Panel[pp].m_iTA==NodeB ||
							m_Panel[pp].m_iTB==NodeB){
							if(VLMIsSameSide(p,pp)){
								color +=tab[pp];
								average++;
							}
						}
					}
					color = (color/average-lmin)/range;
					if(IsBlackAndWhite())	glColor3d(color,color,color);
					else					glColor3d(GLGetRed(color),GLGetGreen(color),GLGetBlue(color));
					glVertex3d(TA.x, TA.y, TA.z);
				glEnd();
			}
		}
	glEndList();
}


void CMiarex::GLCreateTrans(CWing *pWing, CWOpp *pWOpp, UINT List)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	int i,j,k;
	double r,g,b, yrel, xt, yt, zt, yob, dih;
	CVector A,B, Pt;
	CVector C,N;

	//TOP TRANSITION
	glNewList(List,GL_COMPILE);
		m_GLList++;
		//Trace("GL lists = %5d", m_GLList);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glLineWidth((GLfloat)GetPenWidth(pFrame->m_XTopWidth, false));
		if(pFrame->m_XTopStyle == PS_DOT) 
			glLineStipple (1, 0x0101);
		else if(pFrame->m_XTopStyle == PS_DASH) 
			glLineStipple (1, 0x00FF);
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
			if(pWOpp->m_AnalysisType==1){
				glBegin(GL_LINE_STRIP);
					for (i=1; i<pWOpp->m_NStation; i++){
						yob = 2.0*pWOpp->m_SpanPos[i]/pWOpp->m_Span;
						xt = pWing->GetOffset(yob) + pWOpp->m_XTrTop[i]*pWOpp->m_Chord[i];
						pWing->GetViewYZPos(pWOpp->m_XTrTop[i], pWOpp->m_SpanPos[i],yt,zt,0);
		
						dih = pWing->GetDihedral(yob)*pi/180.0;
						glVertex3d(xt,yt,zt);
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
								pWing->m_Surface[j].GetPoint(pWOpp->m_XTrTop[m],pWOpp->m_XTrTop[m],yrel,Pt,1);
								glVertex3d(Pt.x, Pt.y, Pt.z);
								m++;
							}
						}
					glEnd();
				}
				else {
					int m = 0;
					for(j=0; j<pWing->m_NSurfaces; j++){
						glBegin(GL_LINE_STRIP);
							for(k=0; k<pWing->m_Surface[j].m_NYPanels; k++){
								yrel = pWing->Getyrel(pWOpp->m_SpanPos[m]);
								pWing->m_Surface[j].GetPoint(pWOpp->m_XTrTop[m],pWOpp->m_XTrTop[m],yrel,Pt,1);
								glVertex3d(Pt.x, Pt.y, Pt.z);

								m++;
							}
						glEnd();
					}
				}
			}
		}
		glDisable (GL_LINE_STIPPLE);
	glEndList();

	//BOTTOM TRANSITION
	glNewList(List+1,GL_COMPILE);
		m_GLList++;
		//Trace("GL lists = %5d", m_GLList);
		glEnable (GL_LINE_STIPPLE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glLineWidth((GLfloat)GetPenWidth(pFrame->m_XBotWidth, false));
		if(pFrame->m_XBotStyle == PS_DOT) 
			glLineStipple (1, 0x0101);
		else if(pFrame->m_XBotStyle == PS_DASH) 
			glLineStipple (1, 0x00FF);
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
	glEndList();
}


void CMiarex::GLDraw3D(CDC* pDC)
{
	if (!m_pCurWing) {
		m_bResetglGeom = true;
		m_bResetglMesh = true;
		m_bResetglOpp  = true;
	}

	CChildView * pChildView = (CChildView*)m_pChildWnd;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

    wglMakeCurrent(pDC->m_hDC, pChildView->m_hRC);
	double r,g,b;
	DecompRGB(pChildView->m_crBackColor,r,g,b);
	glClearColor((GLfloat)r,(GLfloat)g,(GLfloat)b,0.0);

	if(m_bResetglGeom) {
		if(glIsList(AXES)) {
			glDeleteLists(AXES,1);
			m_GLList--;
		}
		GLCreateAxes();
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

		if(glIsList(WINGLEGEND)) {
			glDeleteLists(WINGLEGEND,1);
			m_GLList--;
		}
		GLCreateWingLegend();
		m_bResetglGeom = false;
	}
	if(m_bResetglMesh) {
		if(glIsList(VLMPANELS)) {
			glDeleteLists(VLMPANELS,1);
			m_GLList-=1;
		}
		GLCreateVLMMesh();
		if(glIsList(VLMCTRLPTS)) {
			glDeleteLists(VLMCTRLPTS,1);
			m_GLList-=1;
		}
		GLCreateCtrlPts();
		if(glIsList(VLMVORTICES)) {
			glDeleteLists(VLMVORTICES,1);
			m_GLList-=1;
		}
		GLCreateVortices();
		m_bResetglMesh = false;
	}
	if(m_bResetglWake) {
		if(glIsList(WINGWAKEPANELS)) {
			glDeleteLists(WINGWAKEPANELS,1);
			m_GLList-=1;
		}
		if (m_pCurWPolar && m_pCurWPolar->m_bWakeRollUp){
			if(m_bVLMFinished) GLCreateWakePanels(WINGWAKEPANELS, m_pCurWOpp);
			else               GLCreateWakePanels(WINGWAKEPANELS, NULL);
		}
		m_bResetglWake = false;
	}

	if(m_bResetglLift || m_bResetglOpp) {

		if(glIsList(VLMWINGLIFT)) {
			glDeleteLists(VLMWINGLIFT,1);
			m_GLList -=1;
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
		if (m_pCurWOpp || (m_pCurPOpp &&m_pCurWOpp) ) {
			GLCreateLift(m_pCurWing, m_pCurWOpp, VLMWINGLIFT);
			GLCreateTrans(m_pCurWing, m_pCurWOpp, VLMWINGTOPTRANS);
			int surf = m_pCurWing->m_NSurfaces;
			if(m_pCurPOpp){
				if(m_pCurWing2) {
					GLCreateLift(m_pCurWing2, &m_pCurPOpp->m_Wing2WOpp, VLMWING2LIFT);
					GLCreateTrans(m_pCurWing2, &m_pCurPOpp->m_Wing2WOpp, VLMWING2TOPTRANS);
					surf += m_pCurWing2->m_NSurfaces;
				}
				if(m_pCurStab) {
					GLCreateLift(m_pCurStab, &m_pCurPOpp->m_StabWOpp, VLMSTABLIFT);
					GLCreateTrans(m_pCurStab, &m_pCurPOpp->m_StabWOpp, VLMSTABTOPTRANS);
					surf += m_pCurStab->m_NSurfaces;
				}
				if(m_pCurFin) {
					GLCreateLift(m_pCurFin, &m_pCurPOpp->m_FinWOpp, VLMFINLIFT);
					GLCreateTrans(m_pCurFin, &m_pCurPOpp->m_FinWOpp, VLMFINTOPTRANS);
				}
			}
			GLCreateLiftForce();
			GLCreateMoments();
		}
		m_bResetglLift = false;
	}
	if(m_bResetglDrag || m_bResetglOpp) {
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

		if (m_pCurWOpp || (m_pCurPOpp &&m_pCurWOpp)) {
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
	if(m_bResetglDownwash || m_bResetglOpp) {
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
		if (m_pCurWOpp){
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
	if(m_bResetglOpp) {
		if(glIsList(VLMCL)) {
			glDeleteLists(VLMCL,3);
			m_GLList-=3;
		}
		if (m_pCurWOpp){
			GLCreateCp();
			if(glIsList(WOPPCPLEGEND)) {
				glDeleteLists(WOPPCPLEGEND,1);
				m_GLList--;
			}
			GLCreateCpLegend();
			if(glIsList(WOPPLEGEND)) {
				glDeleteLists(WOPPLEGEND,1);
				m_GLList--;
			}
			GLCreateWOppLegend();
		}

		m_bResetglOpp = false;
	}

	GLRenderView();

	glFinish();
	SwapBuffers(pDC->m_hDC);

	// draw once without streamlines to update the screen
	// then draw with streamlines
	if(m_bResetglStream || m_bResetglOpp) {
		if(pFrame->m_bStream){//no need to recalculate if not showing
			if(m_pCurWOpp && m_pCurWOpp->m_AnalysisType>=2){

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
/*	if(m_bResetglFlow || m_bResetglOpp) {

		if(pFrame->m_bFlow){
			if(m_pCurWOpp && m_pCurWOpp->m_AnalysisType>=2){

				if(glIsList(VLMFLOWS)) {
					glDeleteLists(VLMFLOWS,1);
					m_GLList -=1;
				}
				GLCreateSurfFlow();

				m_bResetglFlow = false;
				UpdateView();
			}
		}
	}*/

	wglMakeCurrent(pDC->m_hDC, NULL);
}



void CMiarex::Set3DScale()
{
	if(m_bIs3DScaleSet && !m_bAutoScales) return;
	CW3DBar* pW3DBar = (CW3DBar*)m_pW3DBar;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	
	double width  = (double)m_rCltRect.Width();
	double height = (double)m_rCltRect.Height();
	double gh = (double)height/(double)width *pFrame->m_GLScale;
	if(m_pCurWing) {
		//wing along X axis will take 3/4 of the screen
		pW3DBar->m_glScalef = (GLfloat)(3./4.*2.0*pFrame->m_GLScale/m_pCurWing->m_Span);
		pW3DBar->m_glXTransf = 0.0;
		pW3DBar->m_glYTransf = 0.0;
		m_bIs3DScaleSet = true;
	}
//	pW3DBar->m_glYOffset = (GLfloat)(-1.0*pFrame->m_GLScale+clippedh/2.0);
	pW3DBar->m_glYOffset = (GLfloat)(gh - pFrame->m_GLScale);
	m_bResetglOpp = true;

/*	m_bResetglGeom = true;
	m_bResetglOpp  = true;
	m_bResetglMesh = true;
	m_bResetglWake = true;*/
}


void CMiarex::GLSetupLight()
{
	CW3DBar *pW3DBar = (CW3DBar*)m_pW3DBar;

	float fLightAmbient0[4];
	float fLightDiffuse0[4];
	float fLightSpecular0[4];
	float fLightPosition0[4];

	float LightFactor = 1.0f;
	if(m_pCurWing) LightFactor = 15.0f/(float)pow(m_pCurWing->m_Span,1.5);
	if(LightFactor>1.0) 
		LightFactor = 1.0;

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

	fLightPosition0[0] = m_GLLightDlg.m_XLight ; // X
	fLightPosition0[1] = pW3DBar->m_glYOffset + m_GLLightDlg.m_YLight ; // Y
	fLightPosition0[2] = m_GLLightDlg.m_ZLight ; // Z
	fLightPosition0[3] = 1.0; // W (positional light)


	// Enable the basic light
	glLightfv(GL_LIGHT0,GL_AMBIENT,fLightAmbient0);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,fLightDiffuse0);
	glLightfv(GL_LIGHT0,GL_SPECULAR,fLightSpecular0);
	glLightfv(GL_LIGHT0,GL_POSITION,fLightPosition0);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);    // the ambient light
	glDisable(GL_LIGHT1);	
	glDisable(GL_LIGHT2);	
	glDisable(GL_LIGHT3);	
	glDisable(GL_LIGHT4);
	glDisable(GL_LIGHT5);
	glDisable(GL_LIGHT6);
	glDisable(GL_LIGHT7);

	if(m_GLLightDlg.m_bColorMaterial)  {
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
		float fMatAmbient[4]   = {0.0f,0.0f,0.0f,1.0f};
		float fMatSpecular[4]  = {0.0f,0.0f,0.0f,1.0f};
		float fMatDiffuse[4]   = {0.0f,0.0f,0.0f,1.0f};
		float fMatShininess[4] = {0.0f,0.0f,0.0f,1.0f};
		float fMatEmission[4]  = {0.0f,0.0f,0.0f,1.0f};

		glMaterialfv(GL_FRONT, GL_SPECULAR,  fMatSpecular);
		glMaterialfv(GL_FRONT, GL_AMBIENT,   fMatAmbient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE,   fMatDiffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, fMatShininess);
		glMaterialfv(GL_FRONT, GL_EMISSION,  fMatEmission);
	//	glColorMaterial(GL_FRONT, GL_AMBIENT |  GL_DIFFUSE | GL_SPECULAR | GL_EMISSION);
	//	glColorMaterial(GL_FRONT, GL_SPECULAR);
	}
	else{
		glDisable(GL_COLOR_MATERIAL);
		float fMatAmbient[4]   = {m_GLLightDlg.m_MatAmbient,m_GLLightDlg.m_MatAmbient,m_GLLightDlg.m_MatAmbient,1.0f};
		float fMatSpecular[4]  = {m_GLLightDlg.m_MatSpecular,m_GLLightDlg.m_MatSpecular,m_GLLightDlg.m_MatSpecular,1.0f};
		float fMatDiffuse[4]   = {m_GLLightDlg.m_MatDiffuse,m_GLLightDlg.m_MatDiffuse,m_GLLightDlg.m_MatDiffuse,1.0f};
		float fMatShininess[4] = {m_GLLightDlg.m_MatShininess,m_GLLightDlg.m_MatShininess,m_GLLightDlg.m_MatShininess,1.0f};
		float fMatEmission[4]  = {m_GLLightDlg.m_MatEmission,m_GLLightDlg.m_MatEmission,m_GLLightDlg.m_MatEmission,1.0f};

		glMaterialfv(GL_FRONT, GL_SPECULAR,  fMatSpecular);
		glMaterialfv(GL_FRONT, GL_AMBIENT,   fMatAmbient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE,   fMatDiffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, fMatShininess);
		glMaterialfv(GL_FRONT, GL_EMISSION,  fMatEmission);
	}

/*	if(m_GLLightDlg.m_AlphaBlend<1.0){
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else glDisable(GL_BLEND);*/

    if(m_GLLightDlg.m_bDepthTest)  glEnable(GL_DEPTH_TEST);     else glDisable(GL_DEPTH_TEST);
	if(m_GLLightDlg.m_bCullFaces)  glEnable(GL_CULL_FACE);      else glDisable(GL_CULL_FACE);
	if(m_GLLightDlg.m_bSmooth)     glEnable(GL_POLYGON_SMOOTH); else glDisable(GL_POLYGON_SMOOTH);
	if(m_GLLightDlg.m_bShade)      glShadeModel(GL_SMOOTH);     else glShadeModel(GL_FLAT);

	if(m_GLLightDlg.m_bLocalView) glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,0);
	else                          glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER ,1);
//	if(m_GLLightDlg.m_bFog)       {
//		glEnable(GL_FOG);
//		glFogf(GL_FOG_DENSITY,0.2f);
//		glFogi(GL_FOG_MODE, GL_LINEAR);
//		glFogi(GL_FOG_MODE, GL_EXP);
//		glFogi(GL_FOG_MODE, GL_EXP2);
//		float fFog[4] = {0.5f,0.5f,0.5f,1.0f};
//		glFogfv(GL_FOG_COLOR,fFog);
//	}
//	else
		glDisable(GL_FOG);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);
}


void CMiarex::GLRenderView()
{
	CW3DBar *pW3DBar = (CW3DBar*)m_pW3DBar;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	
	// Clear the viewport
	glFlush();
    glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	int width = m_rCltRect.Width();

	glPushMatrix();
		if (pFrame->m_b3DVLMCl && m_pCurWOpp && m_pCurWOpp->m_AnalysisType>=2 && pFrame->m_bShowCpScale)
			glCallList(WOPPCPLEGEND);
		
		if(m_pCurWing) 
			glCallList(WINGLEGEND);
		if(m_pCurWOpp) 
			glCallList(WOPPLEGEND);

		GLSetupLight();
		if(m_bShowLight){
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			glPushMatrix();
				glTranslated( m_GLLightDlg.m_XLight*pFrame->m_GLScale, 
							 (m_GLLightDlg.m_YLight+ pW3DBar->m_glYOffset)*pFrame->m_GLScale,
							  m_GLLightDlg.m_ZLight*pFrame->m_GLScale);
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				double radius = (m_GLLightDlg.m_ZLight+2.0)/40.0*pFrame->m_GLScale;
				COLORREF color;
				color = RGB((int)(m_GLLightDlg.m_Red  *255),
							(int)(m_GLLightDlg.m_Green*255),
							(int)(m_GLLightDlg.m_Blue *255));
				GLRenderSphere(color,radius);
			glPopMatrix();
		}

		glTranslated(-pW3DBar->m_glXTransf*2.0*pFrame->m_GLScale/width, 
					  pW3DBar->m_glYOffset + pW3DBar->m_glYTransf*2.0*pFrame->m_GLScale/width,
					  0.0);
		glPushMatrix();
			glRotatef(pW3DBar->m_glXRotatef, 1.0, 0.0, 0.0);
			glRotatef(pW3DBar->m_glYRotatef, 0.0, 1.0, 0.0);
			glRotatef(pW3DBar->m_glZRotatef, 0.0, 0.0, 1.0);

			glScalef(pW3DBar->m_glScalef, pW3DBar->m_glScalef, pW3DBar->m_glScalef);

			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			if(pW3DBar->m_bAxes)             
				glCallList(AXES);

			if(m_pCurWing){
				if (m_pCurWOpp
					&& pFrame->m_bStream
					&& m_pCurWOpp->m_AnalysisType>=2
					&& !m_bResetglStream)
					glCallList(VLMSTREAMLINES);//streamlines are not rotated
				if(pFrame->m_bMoments && m_pCurWOpp){
					glCallList(VLMMOMENTS);
				}
				
				if(pW3DBar->m_bglLight)	{
					glEnable(GL_LIGHTING);
					glEnable(GL_LIGHT0);
				}
				else {
					glDisable(GL_LIGHTING);
					glDisable(GL_LIGHT0);
				}

				if(m_pCurWOpp && pFrame->m_bStream && m_pCurWOpp->m_AnalysisType>=2 && !m_bResetglFlow) 
					glCallList(VLMFLOWS);

				if (m_pCurWOpp) glRotated(m_pCurWOpp->m_Alpha, 0.0, 1.0, 0.0);


				if(pW3DBar->m_bSurfaces)	{
					glCallList(WINGSURFACES);
					if(m_pCurWing2) glCallList(WING2SURFACES);
					if(m_pCurStab)  glCallList(STABSURFACES);
					if(m_pCurFin)   glCallList(FINSURFACES);
				}

				glDisable(GL_LIGHTING);
				glDisable(GL_LIGHT0);


				if(pW3DBar->m_bVLMPanels){
					glCallList(VLMPANELS);
				}
				
				if(pFrame->m_bVortices){
					glCallList(VLMCTRLPTS);
					glCallList(VLMVORTICES);
				}
				if(pFrame->m_b3DVLMCl && m_pCurWOpp && m_pCurWOpp->m_AnalysisType>=2)
					glCallList(VLMCL);

				if(pFrame->m_bXCP && m_pCurWOpp){
					glCallList(VLMWINGLIFT);
					if(m_pCurPOpp){
						if(m_pCurWing2) 
							glCallList(VLMWING2LIFT);
						if(m_pCurStab) 
							glCallList(VLMSTABLIFT);
						if(m_pCurFin) 
							glCallList(VLMFINLIFT);
					}
					glCallList(VLMLIFTFORCE);
				}

				if((pFrame->m_bICd ||pFrame->m_bVCd) && m_pCurWOpp ){
					glCallList(VLMWINGDRAG);
					if(m_pCurPOpp){
						if(m_pCurWing2) 
							glCallList(VLMWING2DRAG);
						if(m_pCurStab) 
							glCallList(VLMSTABDRAG);
						if(m_pCurFin) 
							glCallList(VLMFINDRAG);
					}
				}
				if(pFrame->m_b3DDownwash && m_pCurWOpp){
					glCallList(VLMWINGWASH);
					if(m_pCurPOpp){
						if(m_pCurWing2) 
							glCallList(VLMWING2WASH);
						if(m_pCurStab) 
							glCallList(VLMSTABWASH);
						if(m_pCurFin) 
							glCallList(VLMFINWASH);
					}
				}
				if(pFrame->m_bXTop && m_pCurWOpp){
					glCallList(VLMWINGTOPTRANS);
					if(m_pCurPOpp){
						if(m_pCurWing2) 
							glCallList(VLMWING2TOPTRANS);
						if(m_pCurStab) 
							glCallList(VLMSTABTOPTRANS);
						if(m_pCurFin) 
							glCallList(VLMFINTOPTRANS);
					}
					glCallList(TOPTRANS);
				}
				if(pFrame->m_bXBot && m_pCurWOpp){
					glCallList(VLMWINGBOTTRANS);
					if(m_pCurPOpp){
						if(m_pCurWing2) 
							glCallList(VLMWING2BOTTRANS);
						if(m_pCurStab) 
							glCallList(VLMSTABBOTTRANS);
						if(m_pCurFin) 
							glCallList(VLMFINBOTTRANS);
					}
					glCallList(TOPTRANS);
				}
				if(pW3DBar->m_bOutline)	{
					glCallList(WINGOUTLINE);
					if(m_pCurWing2) glCallList(WING2OUTLINE);
					if(m_pCurStab)  glCallList(STABOUTLINE);
					if(m_pCurFin)   glCallList(FINOUTLINE);
				}

				if (m_pCurWPolar && m_pCurWPolar->m_bWakeRollUp && pW3DBar->m_bVLMPanels) 
					glCallList(WINGWAKEPANELS);
			}
		glPopMatrix();

	glPopMatrix();
}

void CMiarex::GLRenderSphere(COLORREF cr, double radius)
{
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_TRIANGLES);

	glColor3ub(GetRValue(cr),GetGValue(cr),GetBValue(cr));

	double start_lat = -90;
	double start_lon = 0.0;
	double R = radius;

	const int NumLatitudes = 18;
	const int NumLongitudes = 36;

	double lat_incr = 180.0 / NumLatitudes;
	double lon_incr = 360.0 / NumLongitudes;

	double phi1, phi2, theta1, theta2;
	GLdouble u[3], v[3], w[3], n[3];

	int row, col;

	for (col = 0; col < NumLongitudes; col++){
		phi1 = (start_lon + col * lon_incr) * pi/180.0;
		phi2 = (start_lon + (col + 1) * lon_incr) * pi/180.0;

		for (row = 0; row < NumLatitudes; row++){
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
			glVertex3dv(v);
			glVertex3dv(w);
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

	double dD      = 12.0/(double)width*2.0*pFrame->m_GLScale;
	double LeftPos = -0.98*pFrame->m_GLScale;
	double ZPos = -pFrame->m_GLScale + 8.0*dD +0.02*pFrame->m_GLScale;

	glNewList(WINGLEGEND,GL_COMPILE);
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

	glEndList();

	if(pW3DBar->m_bglLight){
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
}

void CMiarex::GLCreateCpLegend()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString strong;
	CW3DBar *pW3DBar = (CW3DBar*)m_pW3DBar;
	CChildView * pChildView = (CChildView*)(m_pChildWnd);
	int i;
	double r,g,b, f, fi;
	double color = 0.0;
	double range, delta;
	glNewList(WOPPCPLEGEND,GL_COMPILE);
		m_GLList++;
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		if(IsBlackAndWhite())	DecompRGB(0, r, g, b);
		else					DecompRGB(pChildView->m_WndTextColor, r,g,b);
		glColor3d(r,g,b);
		glListBase(GLF_START_LIST); 
		// Draw the labels
		double width  = (double)pChildView->m_rCltRect.Width();
		double height = (double)pChildView->m_rCltRect.Height();
		int maxlength = 7;
		double gh = height/width * pFrame->m_GLScale;
		double dD      = 12.0/width*2.0*pFrame->m_GLScale;
		double ZPos    = -pFrame->m_GLScale + gh/2.0;
		double dz      = gh*1.2/20.0;
		double Right1  = pFrame->m_GLScale*.94;
		double Right2  = pFrame->m_GLScale*.98;
		double RightLetter = pFrame->m_GLScale 
			- (double)(maxlength*pChildView->m_LetterWidth) *2.0*pFrame->m_GLScale/(double)width
			- 0.07*pFrame->m_GLScale;

		range = (pFrame->m_LegendMax-pFrame->m_LegendMin);
		delta = range / 20;
		for (i=0; i<=20; i ++){
			f = pFrame->m_LegendMin + (double)i * delta;
			fi = (double)i*dz;
			strong.Format("%7.2f",f);
			glRasterPos2d(RightLetter,ZPos+fi);
			glCallLists(maxlength, GL_UNSIGNED_BYTE, strong);
		}
		glRasterPos2d(Right1,ZPos+21.0*dz);
		glCallLists(2, GL_UNSIGNED_BYTE, "Cp");

		glBegin(GL_QUAD_STRIP);
			for (i=0; i<=20; i++){
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
	double r,g,b;
	int l, len;

	glListBase(GLF_START_LIST); 

// Write wing data
	CString Result, str, strong, str1;
	double width  = m_rCltRect.Width();
	int height = m_rCltRect.Height();
	int maxlength = 45;

	double dD      = 12.0/(double)width*2.0*pFrame->m_GLScale;
	double ZPos = -pFrame->m_GLScale + 11.0*dD +0.02*pFrame->m_GLScale;
	double XPos = pFrame->m_GLScale 
		- (double)(maxlength*pChildView->m_LetterWidth)*2.0*pFrame->m_GLScale/(double)width-0.02*pFrame->m_GLScale;

	glNewList(WOPPLEGEND,GL_COMPILE);
		m_GLList++;
		if(IsBlackAndWhite())	DecompRGB(0, r,g,b);
		else					DecompRGB(pChildView->m_WndTextColor, r,g,b);
		glColor3d(r,g,b);
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		if(m_pCurWOpp){
			ZPos += m_pCurWOpp->m_nFlaps * dD;
			if(m_pCurWOpp->m_bOut) {
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

			/*		oswald=CZ^2/CXi/pi/allongement;*/
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

			Result.Format("Cm = %9.4f ", 	m_pCurWOpp->m_PitchingMoment);
			ZPos -=dD;
			len = Result.GetLength();
			for(l=0; l<maxlength-len; l++) Result =" "+Result;
			glRasterPos2d(XPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);

			str1.LoadString(IDS_ROLLINGMOMENT);
			Result.Format(str1, m_pCurWOpp->m_RollingMoment);
			ZPos -=dD;
			len = Result.GetLength();
			for(l=0; l<maxlength-len; l++) Result =" "+Result;
			glRasterPos2d(XPos,ZPos);
			glCallLists(Result.GetLength(), GL_UNSIGNED_BYTE, Result);

			str1.LoadString(IDS_INDUCEDMOMENT);
			Result.Format(str1, m_pCurWOpp->m_InducedYawingMoment);
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

			for(int i=0; i<m_pCurWOpp->m_nFlaps; i++){
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
	glEndList();

	if(pW3DBar->m_bglLight){
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
}


void CMiarex::GLCreateCtrlPts()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	glNewList(VLMCTRLPTS,GL_COMPILE);
		m_GLList++;
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glLineWidth(1.0);
		glColor3f(0.0,1.0,0.0);
		for (int p=0; p<m_VLMMatSize; p++){//All panels
			glBegin(GL_LINES);
				glVertex3d(m_Panel[p].CtrlPt.x, m_Panel[p].CtrlPt.y, m_Panel[p].CtrlPt.z);

				glVertex3d((m_Panel[p].CtrlPt.x + m_Panel[p].Normal.x * 0.04),
						   (m_Panel[p].CtrlPt.y + m_Panel[p].Normal.y * 0.04),
						   (m_Panel[p].CtrlPt.z + m_Panel[p].Normal.z * 0.04));
			glEnd();
		}
	glEndList();
}

void CMiarex::GLCreateVortices()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	CVector A, B, C, D, AC, BD;
	glEnable (GL_LINE_STIPPLE);
	glLineStipple (1, 0xFFFF);

	glNewList(VLMVORTICES,GL_COMPILE);
		m_GLList++;

		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glLineWidth(1.0);
		glColor3d(0.7,0.0,0.0);
		for (int p=0; p<m_VLMMatSize; p++){//All panels
			if(!m_Panel[p].m_bIsTrailing){
				if(m_Panel[p].m_iPos<=0){
					A = m_Panel[p].A;
					B = m_Panel[p].B;
					C = m_Panel[p-1].B;
					D = m_Panel[p-1].A;
				}
				else {
					A = m_Panel[p].A;
					B = m_Panel[p].B;
					C = m_Panel[p+1].B;
					D = m_Panel[p+1].A;
				}
			}
			else {
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
			AC.x = C.x-A.x;
			AC.y = C.y-A.y;
			AC.z = C.z-A.z;
			BD.x = D.x-B.x;
			BD.y = D.y-B.y;
			BD.z = D.z-B.z;

			AC *= 0.03;
			A  += AC;
			C  -= AC; 
			BD *= 0.03;
			B  += BD;
			D  -= BD; 

			if(m_pCurWPolar && m_pCurWPolar->m_bVLM1){
				glLineStipple (1, 0xFFFF);
				glBegin(GL_LINES);
					glVertex3d(A.x, A.y, A.z);
					glVertex3d(B.x, B.y, B.z);
				glEnd();
				glLineStipple (1, 0x00FF);
				glBegin(GL_LINES);
					glVertex3d(A.x, A.y, A.z);
					glVertex3d(D.x, D.y, D.z);
				glEnd();
				glBegin(GL_LINES);
					glVertex3d(B.x, B.y, B.z);
					glVertex3d(C.x, C.y, C.z);
				glEnd();
			}
			else if(!m_pCurWPolar || (m_pCurWPolar && !m_pCurWPolar->m_bVLM1)){
				glBegin(GL_LINE_STRIP);
					glVertex3d(A.x, A.y, A.z);
					glVertex3d(B.x, B.y, B.z);
					glVertex3d(C.x, C.y, C.z);
					glVertex3d(D.x, D.y, D.z);
					glVertex3d(A.x, A.y, A.z);
				glEnd();
			}
		}
	glEndList();
}


void CMiarex::GLCreateGeom(CWing *pWing, UINT List, COLORREF wingcolor)
{
//	GLCreateCtrlPts();
	int j,l;
	CVector Pt;
	CVector A,B,C,D,N, BD,AC;
	double r,g,b;
	COLORREF color;
	int style, width;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;	
	
	N.Set(0.0, 0.0, 0.0);

	glNewList(List,GL_COMPILE);
		m_GLList++;
		glLineWidth(1.0);

		color = wingcolor;
		style = 0;
		width = 0;
		GetBWColor(color, style, width);
		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

		//top surface
		for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
			for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++){
				glBegin(GL_QUADS);
					pWing->m_Surface[j].GetPanel(0,l, N, 1);

					glNormal3d( N.x,  N.y,  N.z);
					glVertex3d(pWing->m_Surface[j].TA.x, 
							   pWing->m_Surface[j].TA.y,
							   pWing->m_Surface[j].TA.z);	
					glVertex3d(pWing->m_Surface[j].LA.x, 
							   pWing->m_Surface[j].LA.y, 
							   pWing->m_Surface[j].LA.z);

					pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1,l, N, 1);
					glVertex3d(pWing->m_Surface[j].LB.x,
							   pWing->m_Surface[j].LB.y,
							   pWing->m_Surface[j].LB.z);
					glVertex3d(pWing->m_Surface[j].TB.x,
							   pWing->m_Surface[j].TB.y,
							   pWing->m_Surface[j].TB.z);

				glEnd();
			}
		}

		//bottom surface
		for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
			for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++){
				glBegin(GL_QUADS);
					pWing->m_Surface[j].GetPanel(0,l, N, -1);

					glNormal3d(N.x, N.y, N.z);
					glVertex3d( pWing->m_Surface[j].LA.x, 
								pWing->m_Surface[j].LA.y, 
								pWing->m_Surface[j].LA.z);
					glVertex3d( pWing->m_Surface[j].TA.x, 
								pWing->m_Surface[j].TA.y,
								pWing->m_Surface[j].TA.z);	

					pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1,l, N, -1);
					glVertex3d( pWing->m_Surface[j].TB.x,
								pWing->m_Surface[j].TB.y,
								pWing->m_Surface[j].TB.z);
					glVertex3d( pWing->m_Surface[j].LB.x,
								pWing->m_Surface[j].LB.y,
								pWing->m_Surface[j].LB.z);
				glEnd();
			}
		}
		for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
			//Left surface
			glBegin(GL_QUAD_STRIP);
				
				pWing->m_Surface[j].GetPanel(0,0, N,1);
				A. Copy(pWing->m_Surface[0].TA);
				B. Copy(pWing->m_Surface[0].LA);
				pWing->m_Surface[j].GetPanel(0,0, N,-1);
				C. Copy(pWing->m_Surface[0].LA);
				D. Copy(pWing->m_Surface[0].TA);

				BD = B-D;
				AC = A-C;
				N  = AC*BD;
				N.Normalize();
				glNormal3d( N.x,  N.y,  N.z);

				glVertex3d(C.x,C.y,C.z);
				glVertex3d(A.x,A.y, A.z);
				for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++){
					pWing->m_Surface[j].GetPanel(0,l, N,-1);
					glVertex3d(pWing->m_Surface[0].LA.x,
							   pWing->m_Surface[0].LA.y,
							   pWing->m_Surface[0].LA.z);
					pWing->m_Surface[j].GetPanel(0,l, N,1);
					glVertex3d(pWing->m_Surface[0].LA.x,
							   pWing->m_Surface[0].LA.y,
							   pWing->m_Surface[0].LA.z);
				}
			glEnd();
			//right surface
			glBegin(GL_QUAD_STRIP);
				glNormal3d(-N.x, -N.y, -N.z);
				pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1,0, N,1);
				glVertex3d(pWing->m_Surface[j].TB.x,
						   pWing->m_Surface[j].TB.y,
						   pWing->m_Surface[j].TB.z);
				pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1,0, N,-1);
				glVertex3d(pWing->m_Surface[j].TB.x,
						   pWing->m_Surface[j].TB.y,
						   pWing->m_Surface[j].TB.z);
				for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++){
					pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1, l, N,-1);
					glVertex3d(pWing->m_Surface[j].LB.x,
							   pWing->m_Surface[j].LB.y,
							   pWing->m_Surface[j].LB.z);
					pWing->m_Surface[j].GetPanel(pWing->m_Surface[j].m_NYPanels-1, l, N,1);
					glVertex3d(pWing->m_Surface[j].LB.x,
							   pWing->m_Surface[j].LB.y,
							   pWing->m_Surface[j].LB.z);
				}
			glEnd();
		}
	glEndList();

	//OUTLINE
	glNewList(List+1,GL_COMPILE);
		m_GLList++;

		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glEnable (GL_LINE_STIPPLE);
		glLineWidth((GLfloat)GetPenWidth(pFrame->m_OutlineWidth, false));

		color = pFrame->m_OutlineColor;
		style = pFrame->m_OutlineStyle;
		width = pFrame->m_OutlineWidth;
		GetBWColor(color, style, width);

		if     (style == PS_DOT) 		glLineStipple (1, 0x0101);
		else if(style == PS_DASH) 		glLineStipple (1, 0x00FF);
		else if(style == PS_DASHDOT) 	glLineStipple (1, 0x1C47);
		else							glLineStipple (1, 0xFFFF);// Solid

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);
		glLineWidth((GLfloat)GetPenWidth(width, false));

		//TOP PANELS
		for (j=0; j<pWing->m_NSurfaces; j++){
			glBegin(GL_LINE_STRIP);
				for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++){
					pWing->m_Surface[j].GetPanel(0,l, N,1);
					glVertex3d(pWing->m_Surface[j].TA.x,
							   pWing->m_Surface[j].TA.y,
							   pWing->m_Surface[j].TA.z);
				}
		
				glVertex3d(pWing->m_Surface[j].LA.x,
						   pWing->m_Surface[j].LA.y,
						   pWing->m_Surface[j].LA.z);
			glEnd();

			glBegin(GL_LINE_STRIP);
				for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++){
					pWing->m_Surface[j].GetPanel(
											pWing->m_Surface[j].m_NYPanels-1,
											l, N, 1);
					glVertex3d(pWing->m_Surface[j].TB.x,
							   pWing->m_Surface[j].TB.y,
							   pWing->m_Surface[j].TB.z);
				}

				glVertex3d(pWing->m_Surface[j].LB.x,
						   pWing->m_Surface[j].LB.y,
						   pWing->m_Surface[j].LB.z);
			glEnd();
		}

		//BOTTOM PANELS
		for (j=0; j<pWing->m_NSurfaces; j++){
			glBegin(GL_LINE_STRIP);
				for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++){
					pWing->m_Surface[j].GetPanel(0,l, N,-1);
					glVertex3d(pWing->m_Surface[j].TA.x,
							   pWing->m_Surface[j].TA.y,
							   pWing->m_Surface[j].TA.z);
				}
				glVertex3d(pWing->m_Surface[j].LA.x,
						   pWing->m_Surface[j].LA.y,
						   pWing->m_Surface[j].LA.z);
			glEnd();
			glBegin(GL_LINE_STRIP);
				for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++){
					pWing->m_Surface[j].GetPanel(
											pWing->m_Surface[j].m_NYPanels-1,
											l, N,-1);
					glVertex3d(pWing->m_Surface[j].TB.x,
							   pWing->m_Surface[j].TB.y,
							   pWing->m_Surface[j].TB.z);
				}

				glVertex3d(pWing->m_Surface[j].LB.x,
						   pWing->m_Surface[j].LB.y,
						   pWing->m_Surface[j].LB.z);
			glEnd();
		}
		//WingContour
		//Leading edge outline
		for (j=0; j<pWing->m_NSurfaces; j++){
			glBegin(GL_LINES);
				pWing->m_Surface[j].GetPanel(0,pWing->m_Surface[j].m_NXPanels-1, N,0);
				glVertex3d(pWing->m_Surface[j].LA.x,
						   pWing->m_Surface[j].LA.y,
						   pWing->m_Surface[j].LA.z);
				pWing->m_Surface[j].GetPanel( pWing->m_Surface[j].m_NYPanels-1,pWing->m_Surface[j].m_NXPanels-1, N,0);
				glVertex3d(pWing->m_Surface[j].LB.x,
						   pWing->m_Surface[j].LB.y,
						   pWing->m_Surface[j].LB.z);
			glEnd();
		}
		//Trailing edge outline
		for (j=0; j<pWing->m_NSurfaces; j++){
			glBegin(GL_LINES);
				pWing->m_Surface[j].GetPanel(0,0, N,0);
				glVertex3d(pWing->m_Surface[j].TA.x,
						   pWing->m_Surface[j].TA.y,
						   pWing->m_Surface[j].TA.z);
				pWing->m_Surface[j].GetPanel( pWing->m_Surface[j].m_NYPanels-1,0, N,0);
				glVertex3d(pWing->m_Surface[j].TB.x,
						   pWing->m_Surface[j].TB.y,
						   pWing->m_Surface[j].TB.z);
			glEnd();
		}
		//flap outline....
		for (j=0; j<pWing->m_NSurfaces; j++){
			if(pWing->m_Surface[j].m_pFoilA->m_bTEFlap && pWing->m_Surface[j].m_pFoilB->m_bTEFlap) {
				glBegin(GL_LINES);
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
				glEnd();
				glBegin(GL_LINES);
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
				glEnd();
			}
		}
		for (j=0; j<pWing->m_NSurfaces; j++){
			if(pWing->m_Surface[j].m_pFoilA->m_bLEFlap && pWing->m_Surface[j].m_pFoilB->m_bLEFlap) {
				glBegin(GL_LINES);
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
				glEnd();
				glBegin(GL_LINES);
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
				glEnd();
			}
		}
	glEndList();
}


void CMiarex::GLCreateVLMMesh()
{
	double r,g,b;
	COLORREF color;
	int style, width;
	
	CVector A, B, N;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;	
	
	N.Set(0.0, 0.0, 0.0);
	//VLM Mesh
	glNewList(VLMPANELS,GL_COMPILE);
		m_GLList++;

		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		color = pFrame->m_VLMColor;
		style = pFrame->m_VLMStyle;
		width = pFrame->m_VLMWidth;
		GetBWColor(color, style, width);

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		glLineWidth((GLfloat)GetPenWidth(width, false));
		glEnable (GL_LINE_STIPPLE);
		if     (style == PS_DOT) 		glLineStipple (1, 0x0101);
		else if(style == PS_DASH) 		glLineStipple (1, 0x00FF);
		else if(style == PS_DASHDOT) 	glLineStipple (1, 0x1C47);
		else							glLineStipple (1, 0xFFFF);// Solid

		int iLA, iLB, iTA, iTB;
		//middle surface
		for (int p=0; p<m_VLMMatSize; p++){//All surfaces
				glBegin(GL_QUADS);
/*					if(m_Panel[p].m_bIsTrailing) glColor3d(1.0,00.,0.0);
					else if(m_Panel[p].m_bIsLeading) glColor3d(0.0,1.,0.0);
					else glColor3d(r,g,b);*/

					iLA = m_Panel[p].m_iLA;
					iLB = m_Panel[p].m_iLB;
					iTA = m_Panel[p].m_iTA;
					iTB = m_Panel[p].m_iTB;

					glVertex3d(m_Node[iLA].x, m_Node[iLA].y, m_Node[iLA].z);
					glVertex3d(m_Node[iTA].x, m_Node[iTA].y, m_Node[iTA].z);
					glVertex3d(m_Node[iTB].x, m_Node[iTB].y, m_Node[iTB].z);
					glVertex3d(m_Node[iLB].x, m_Node[iLB].y, m_Node[iLB].z);
					glNormal3d(m_Panel[p].Normal.x, m_Panel[p].Normal.y, m_Panel[p].Normal.z);

				glEnd();
		}

		//contour/outline
/*		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

		color = pFrame->m_OutlineColor;
		style = pFrame->m_OutlineStyle;
		width = pFrame->m_OutlineWidth;
		GetBWColor(color, style, width);

		DecompRGB(color,r,g,b);
		glColor3d(r,g,b);

		glLineWidth((GLfloat)GetPenWidth(width, false));
		glEnable (GL_LINE_STIPPLE);
		if     (style == PS_DOT) 		glLineStipple (1, 0x0101);
		else if(style == PS_DASH) 		glLineStipple (1, 0x00FF);
		else if(style == PS_DASHDOT) 	glLineStipple (1, 0x1C47);
		else							glLineStipple (1, 0xFFFF);// Solid


		//middle surface
		for (int p=0; p<m_VLMMatSize; p++){//All surfaces
				glBegin(GL_QUADS);

					iLA = m_Panel[p].m_iLA;
					iLB = m_Panel[p].m_iLB;
					iTA = m_Panel[p].m_iTA;
					iTB = m_Panel[p].m_iTB;

					glVertex3d(m_Node[iLA].x, m_Node[iLA].y, m_Node[iLA].z);
					glVertex3d(m_Node[iTA].x, m_Node[iTA].y, m_Node[iTA].z);
					glVertex3d(m_Node[iTB].x, m_Node[iTB].y, m_Node[iTB].z);
					glVertex3d(m_Node[iLB].x, m_Node[iLB].y, m_Node[iLB].z);
					glNormal3d(m_Panel[p].Normal.x, m_Panel[p].Normal.y, m_Panel[p].Normal.z);

				glEnd();
		}*/

	glEndList();
}

void CMiarex::GLCreateWakePanels(int LIST, CWOpp *pWOpp)
{
	if(!m_pCurWPolar || !m_pCurWPolar->m_bWakeRollUp) return;
	if(m_nWakeNodes==0 || m_WakeSize==0) return;
	int j,k,lw, NXWakePanels;
	int iWLA, iWLB, iWTA, iWTB;
	double r,g,b;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;


	if(pWOpp) NXWakePanels = pWOpp->m_NXWakePanels;
	else           NXWakePanels = m_NXWakePanels;
	//VLM WAKE PANELS
	glNewList(LIST,GL_COMPILE);
		m_GLList++;
		glLineWidth(1.0);

		glLineWidth((GLfloat)GetPenWidth(1,false));
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		DecompRGB(pFrame->m_WakeColor,r,g,b);
		glColor3d(r*.8,g*.8,b*.8);
		glEnable (GL_LINE_STIPPLE);
		glLineWidth((GLfloat)GetPenWidth(pFrame->m_WakeWidth, false));
		if(pFrame->m_WakeStyle == PS_DOT) 
			glLineStipple (1, 0x0101);
		else if(pFrame->m_WakeStyle == PS_DASH) 
			glLineStipple (1, 0x00FF);
		else if(pFrame->m_WakeStyle == PS_DASHDOT) 
			glLineStipple (1, 0x1C47);
		else  
			glLineStipple (1, 0xFFFF);// Solid

		int mw = 0;
		for (j=0; j<m_pCurWing->m_NSurfaces; j++){
			for (k=0; k<m_Surface[j].m_NYPanels; k++){
				for (lw=0; lw<NXWakePanels; lw++){
					iWLA = m_WakePanel[mw].m_iLA;
					iWTA = m_WakePanel[mw].m_iTA;
					iWLB = m_WakePanel[mw].m_iLB;
					iWTB = m_WakePanel[mw].m_iTB;
					glBegin(GL_QUADS);
						glVertex3d(m_WakeNode[iWLA].x, 
									m_WakeNode[iWLA].y, 
									m_WakeNode[iWLA].z);
						glVertex3d(m_WakeNode[iWLB].x, 
									m_WakeNode[iWLB].y, 
									m_WakeNode[iWLB].z);
						glVertex3d(m_WakeNode[iWTB].x, 
									m_WakeNode[iWTB].y, 
									m_WakeNode[iWTB].z);
						glVertex3d(m_WakeNode[iWTA].x, 
									m_WakeNode[iWTA].y, 
									m_WakeNode[iWTA].z);
						mw++;

					glEnd();
				}
			}
		}
	glEndList();
}

void CMiarex::GLCreateWakeLines(int LIST)
{
	if(!m_pCurWPolar || !m_pCurWPolar->m_bWakeRollUp) return;
	int j,k,lw;
	int iWLA, iWLB, iWTA, iWTB;
	double r,g,b;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	//VLM WAKE PANELS
	glNewList(LIST,GL_COMPILE);
		m_GLList++;
		glLineWidth(1.0);

		glLineWidth((GLfloat)GetPenWidth(1,false));
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		DecompRGB(pFrame->m_OutlineColor,r,g,b);
		glColor3d(r*.8,g*.8,b*.8);
		glEnable (GL_LINE_STIPPLE);
		glLineWidth((GLfloat)GetPenWidth(pFrame->m_OutlineWidth, false));
		if(pFrame->m_OutlineStyle == PS_DOT) 
			glLineStipple (1, 0x0101);
		else if(pFrame->m_OutlineStyle == PS_DASH) 
			glLineStipple (1, 0x00FF);
		else if(pFrame->m_OutlineStyle == PS_DASHDOT) 
			glLineStipple (1, 0x1C47);
		else  
			glLineStipple (1, 0xFFFF);// Solid

		int mw = 0;
		for (j=0; j<m_pCurWing->m_NSurfaces; j++){
			for (k=0; k<m_pCurWing->m_Surface[j].m_NYPanels; k++){
				for (lw=0; lw<m_NXWakePanels; lw++){
					iWLA = m_WakePanel[mw].m_iLA;
					iWTA = m_WakePanel[mw].m_iTA;
					glBegin(GL_LINE_STRIP);
						glVertex3d(m_WakeNode[iWLA].x, m_WakeNode[iWLA].y, m_WakeNode[iWLA].z);
						glVertex3d(m_WakeNode[iWTA].x, m_WakeNode[iWTA].y, m_WakeNode[iWTA].z);
						mw++;

					glEnd();
				}
			}
			mw -= m_NXWakePanels;
			for (lw=0; lw<m_NXWakePanels; lw++){
				iWLB = m_WakePanel[mw].m_iLB;
				iWTB = m_WakePanel[mw].m_iTB;
				glBegin(GL_LINE_STRIP);
					glVertex3d(m_WakeNode[iWLB].x, m_WakeNode[iWLB].y, m_WakeNode[iWLB].z);
					glVertex3d(m_WakeNode[iWTB].x, m_WakeNode[iWTB].y, m_WakeNode[iWTB].z);
					mw++;

				glEnd();
			}

		}
	glEndList();
}

void CMiarex::GLCreateWakeSurfaces(int LIST)
{
	if(!m_pCurWPolar || !m_pCurWPolar->m_bWakeRollUp) return;
	int j,k,lw;
	int iWLA, iWLB, iWTA, iWTB;
	double r,g,b;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	//VLM WAKE PANELS
	glNewList(LIST,GL_COMPILE);
		m_GLList++;
		glLineWidth(1.0);

		glLineWidth((GLfloat)GetPenWidth(1,false));
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		DecompRGB(pFrame->m_OutlineColor,r,g,b);
		glColor3d(r*.8,g*.8,b*.8);
		glEnable (GL_LINE_STIPPLE);
		glLineWidth((GLfloat)GetPenWidth(pFrame->m_OutlineWidth, false));
		if(pFrame->m_OutlineStyle == PS_DOT) 
			glLineStipple (1, 0x0101);
		else if(pFrame->m_OutlineStyle == PS_DASH) 
			glLineStipple (1, 0x00FF);
		else if(pFrame->m_OutlineStyle == PS_DASHDOT) 
			glLineStipple (1, 0x1C47);
		else  
			glLineStipple (1, 0xFFFF);// Solid

		int mw = 0;
		for (j=0; j<m_NSurfaces; j++){
			for (k=0; k<m_Surface[j].m_NYPanels; k++){
				for (lw=0; lw<m_NXWakePanels; lw++){
					iWLA = m_WakePanel[mw].m_iLA;
					iWTA = m_WakePanel[mw].m_iTA;
					iWLB = m_WakePanel[mw].m_iLB;
					iWTB = m_WakePanel[mw].m_iTB;
					glBegin(GL_QUADS);
						glVertex3d(m_WakeNode[iWLA].x, m_WakeNode[iWLA].y, m_WakeNode[iWLA].z);
						glVertex3d(m_WakeNode[iWLB].x, m_WakeNode[iWLB].y, m_WakeNode[iWLB].z);
						glVertex3d(m_WakeNode[iWTB].x, m_WakeNode[iWTB].y, m_WakeNode[iWTB].z);
						glVertex3d(m_WakeNode[iWTA].x, m_WakeNode[iWTA].y, m_WakeNode[iWTA].z);
						mw++;

					glEnd();
				}
			}
		}
	glEndList();
}

void CMiarex::GLCreateAxes()
{
	int i;
	double r,g,b;
	CChildView * pChildView = (CChildView*)m_pChildWnd;
	// Finally, the coordinate axes display list
	glNewList(AXES,GL_COMPILE);
		m_GLList++;
		//Trace("GL lists = %5d", m_GLList);

		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glLineWidth(1.0);
		DecompRGB(pChildView->m_WndTextColor,r,g,b);
		glColor3d(r,g,b);

// X axis____________
		glEnable (GL_LINE_STIPPLE);
		glLineStipple (1, 0x1C47);// Dash-Dot

		glVertex3d(1000.0, 0.0, 0.0);
		glBegin(GL_LINE_STRIP);
			for(i=-9; i<=10; i++){
				glVertex3d(0.1*(double)i, 0.0, 0.0);
			}
		glEnd();
		//Arrow
		glBegin(GL_LINES); 
			glVertex3d(1.0, 0.0,  0.0);
			glVertex3d( 0.98,0.015, 0.015);
		glEnd();
		glBegin(GL_LINES); 
			glVertex3d(1.0, 0.0,   0.0);
			glVertex3d( 0.98,-0.015,-0.015);
		glEnd();
		glDisable (GL_LINE_STIPPLE);
		//XLabel
		glBegin(GL_LINES); 
			glVertex3d(1.0, -0.050, -0.020);
			glVertex3d(1.0, -0.020, -0.050);
		glEnd();
		glBegin(GL_LINES); 
			glVertex3d(1.0, -0.020, -0.020);
			glVertex3d(1.0, -0.050, -0.050);
		glEnd();

// Y axis____________
		glEnable (GL_LINE_STIPPLE);
		glLineStipple (1, 0x1C47);// Dash-Dot
		glBegin(GL_LINES); 
			glVertex3d(0.0,-1.0, 0.0);
		glEnd();

		
		glVertex3d(0.0, 1.0, 0.0);
		glBegin(GL_LINE_STRIP);
			for(i=-9; i<=10; i++){
				glVertex3d(0.0, 0.1*(double)i, 0.0);
			}
		glEnd();
		
		//Arrow
		glBegin(GL_LINES);
			glVertex3d( 0.0, 1.0,  0.0);
			glVertex3d(0.015,  0.98, 0.015);
		glEnd();
		glBegin(GL_LINES);
			glVertex3d(  0.0, 1.0,  0.0);
			glVertex3d(-0.015,  0.98,-0.015);
		glEnd();
		glDisable (GL_LINE_STIPPLE);
		//Y Label
		glBegin(GL_LINES); 
			glVertex3d(-0.020,  1.0,-0.020);
			glVertex3d(-0.050, 1.0,-0.050);
		glEnd();
		glBegin(GL_LINES); 
			glVertex3d(-0.050, 1.0,-0.020);
			glVertex3d(-0.035, 1.0,-0.035);
		glEnd();

// Z axis____________
		glEnable (GL_LINE_STIPPLE);
		glLineStipple (1, 0x1C47);// Dash-Dot
		glVertex3d(0.0, 0.0, 1000.0);
		glBegin(GL_LINE_STRIP);
			for(i=-9; i<=10; i++){
				glVertex3d(0.0, 0.0, 0.1*(double)i);
			}
		glEnd();

		//Arrow
		glBegin(GL_LINES); 
			glVertex3d(  0.0,   0.0, 1.0);
			glVertex3d( 0.015,  0.015,  0.98);
		glEnd();
		glBegin(GL_LINES); 
			glVertex3d( 0.0,    0.0, 1.0);
			glVertex3d(-0.015, -0.015,  0.98);
		glEnd();
		glDisable (GL_LINE_STIPPLE);
		//ZLabel
		glBegin(GL_LINE_STRIP); 
			glVertex3d(-0.050, -0.050,1.0);
			glVertex3d(-0.050, -0.020,1.0);

			glVertex3d(-0.020, -0.050,1.0);
			glVertex3d(-0.020, -0.020,1.0);
		glEnd();
		glDisable (GL_LINE_STIPPLE);

	glEndList(); // AXES
}



void CMiarex::OnWPolarExport()
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->ExportWPlr(m_pCurWPolar);
}

void CMiarex::CreateCpCurves()
{
	if(!m_pCurWOpp) return;
	int p;
	CVector N;
	
	m_CpGraph.DeleteCurves();
	CCurve *pCurve = m_CpGraph.AddCurve();
	pCurve->SetColor(m_pCurWOpp->m_Color);
	pCurve->SetStyle(m_pCurWOpp->m_Style);
	pCurve->SetWidth(m_pCurWOpp->m_Width);
	pCurve->ShowPoints(m_pCurWOpp->m_bShowPoints);
	pCurve->SetTitle("Cp");

	for (p=0; p<m_VLMMatSize; p++){
		if (m_Node[m_Panel[p].m_iLA].y <= m_CurSpanPos*m_pCurWing->m_Span/2.0 &&
			m_Node[m_Panel[p].m_iLB].y >  m_CurSpanPos*m_pCurWing->m_Span/2.0){
			
			pCurve->AddPoint(m_Panel[p].CollPt.x, -m_pCurWOpp->m_Cp[p]);
		}
	}
}

void CMiarex::OnCpView() 
{
	if(!m_pCurWing || !m_pCurWOpp || !m_pCurWPolar || m_pCurWPolar->m_AnalysisType != 3 ) return;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	CToolBarCtrl *pTB = &(pFrame->m_MiarexBar.GetToolBarCtrl());
	pTB->PressButton(IDT_WOPP,    true);
	pTB->PressButton(IDT_WPOLARS, false);
	pTB->PressButton(IDM_3DVIEW,  false);
//	pTB->PressButton(IDM_CHORDCP, false);

	CValueDlg dlg;
	dlg.m_WindowText = "Span Position Dlg";
	dlg.m_Text1 = "y/Span =";
	dlg.m_Text2 = " ";
	dlg.m_Value = m_CurSpanPos;

	if(dlg.DoModal())	{
		m_CurSpanPos = dlg.m_Value;
		m_CurSpanPos = min(m_CurSpanPos,  1.0);
		m_CurSpanPos = max(m_CurSpanPos, -1.0);
	}
	CreateCpCurves();
	if(m_iView==4) {
		UpdateView();
		return;
	}

	m_pCurGraph = &m_CpGraph;
	m_iView=4;
	pFrame->DockW3DBar();
	pFrame->m_PolarDlgBar.SetParams();
	m_FlowLinesDlg.ShowWindow(SW_HIDE);
	CheckMenus();

	UpdateView();
}

void CMiarex::OnWOpp() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CToolBarCtrl *pTB = &(pFrame->m_MiarexBar.GetToolBarCtrl());
	pTB->PressButton(IDT_WOPP,    true);
	pTB->PressButton(IDT_WPOLARS, false);
	pTB->PressButton(IDM_3DVIEW,  false);
//	pTB->PressButton(IDM_CHORDCP, false);

	if(m_iView==1) {
		UpdateView();
		return;
	}

	m_pCurGraph = m_pCurWingGraph;
	m_iView=1;
	pFrame->DockW3DBar();
	pFrame->m_PolarDlgBar.SetParams();
	m_FlowLinesDlg.ShowWindow(SW_HIDE);
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



void CMiarex::OnShowWPolar() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	if (m_bAnimate) {
		StopAnimate();
	}

	CToolBarCtrl *pTB = &(pFrame->m_MiarexBar.GetToolBarCtrl());
	pTB->PressButton(IDT_WOPP,    false);
	pTB->PressButton(IDT_WPOLARS, true);
	pTB->PressButton(IDM_3DVIEW,  false);
//	pTB->PressButton(IDM_CHORDCP, false);

	if(m_iView==2) {
		UpdateView();
		return;
	}

	m_iView=2;
	m_pCurGraph = m_pCurWPlrGraph;
	pFrame->DockW3DBar();
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

	if(m_iView!=2) OnShowWPolar();
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
	if(m_iView!=2) OnShowWPolar();
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
	if(m_iView!=2) OnShowWPolar();
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
	if(m_iView!=2) OnShowWPolar();
	else {
		SetWPlrLegendPos();
		CheckMenus();
		UpdateView();	
	}
}


void CMiarex::CreateSurfaces(CWing *pWing, CVector T, double XTilt, double YTilt, COLORREF color)
{
	pWing->CreateSurfaces(T, XTilt, YTilt);

	for (int i=0; i<pWing->m_NSurfaces; i++){
		m_Surface[m_NSurfaces].Copy(pWing->m_Surface[i]);
//		m_Surface[m_NSurfaces].m_Color = color;
		m_NSurfaces++;
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
	UpdateWOpps();
	SetWOpp(true);
	UpdateView();
}


bool CMiarex::VLMInitializePanels()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	m_VLMMatSize  = 0;
	for (int j=0; j<m_NSurfaces; j++){
		m_VLMMatSize    += m_Surface[j].GetNPanels();
	}
	if(m_pCurWPolar && !m_pCurWPolar->m_bMiddle){
		m_VLMMatSize *=2;
	}
	if(m_VLMMatSize>VLMMATSIZE){
		CString strong;
		strong.Format("Totalnumber of panels =%d\n Max Number =%d\nA reduction of the number of panels is required",m_VLMMatSize, VLMMATSIZE);
		AfxMessageBox(strong, MB_OK);
		return false;
	}

	
	m_VLMMatSize = 0;
	m_nNodes     = 0;
	m_nWakeNodes = 0;
	m_WakeSize   = 0;

	memset(m_Panel, 0, sizeof(m_Panel));
	memset(m_Node, 0, sizeof(m_Node));

	CPanel *ptr = m_Panel;
	CreateElements(m_pCurWing, m_Panel);
	ptr = m_Panel+m_pCurWing->m_VLMMatSize;
	if(m_pCurWing2) {
		CreateElements(m_pCurWing2, ptr);
		ptr = ptr + m_pCurWing2->m_VLMMatSize;
	}
	if(m_pCurStab) {
		CreateElements(m_pCurStab,ptr);
		ptr += m_pCurStab->m_VLMMatSize;
	}
	if(m_pCurFin) {
		CreateElements(m_pCurFin,ptr);
	}
	

	m_VLMDlg.m_pPlane         = m_pCurPlane;
	m_VLMDlg.m_pWing          = m_pCurWing;
	m_VLMDlg.m_pWing2         = m_pCurWing2;
	m_VLMDlg.m_pStab          = m_pCurStab;
	m_VLMDlg.m_pFin           = m_pCurFin;

	m_VLMDlg.m_VLMMatSize     = m_VLMMatSize;
	m_VLMDlg.m_nNodes         = m_nNodes;
	m_VLMDlg.m_NSurfaces      = m_NSurfaces;

	return true;
}

void CMiarex::ResetElements()
{
	for (int p=0; p<VLMMATSIZE; p++){
		m_Panel[p].Reset();
		m_WakePanel[p].Reset();
	}
}


bool CMiarex::CreateElements(CWing *pWing, CPanel *pPanel)
{
	int j,k,l;
	int n0, n1, n2, n3;
	int nNode = 0;
	int m  = 0;		// number of wing panels
	int NWakeColumn = 0;	// number of wake columns

	int side;
	CVector N, LATB, TALB;
	CVector LA, LB, TA, TB;

	ResetElements();

	pWing->m_pPanel = pPanel;
	pWing->m_pNode  = m_Node;

	for (j=0; j<pWing->m_NSurfaces; j++){//All surfaces
		for (k=0; k<pWing->m_Surface[j].m_NYPanels; k++){
			if(!m_pCurWPolar) side = 0;
			else if(m_pCurWPolar && m_pCurWPolar->m_AnalysisType==1) side =0;
			else if(m_pCurWPolar){
				if(m_pCurWPolar->m_AnalysisType==2 && m_pCurWPolar->m_bMiddle)    
					side = 0;
				else if (m_pCurWPolar->m_AnalysisType==3 ||	!m_pCurWPolar->m_bMiddle)
					side =-1; //start with lower surf, as recommended by K&P 
			}
			//from T.E. to L.E.
			for (l=0; l<pWing->m_Surface[j].m_NXPanels; l++){
				pWing->m_Surface[j].GetPanel(k,l,N,side);
				n0 = IsNode(pWing->m_Surface[j].LA);
				n1 = IsNode(pWing->m_Surface[j].TA);
				n2 = IsNode(pWing->m_Surface[j].LB);
				n3 = IsNode(pWing->m_Surface[j].TB);

				if(l==0)									pPanel[m].m_bIsTrailing = true;
				if(l==pWing->m_Surface[j].m_NXPanels-1) 	pPanel[m].m_bIsLeading  = true;
				
				if(n0>=0) {
					pPanel[m].m_iLA = n0;
				}
				else {
					pPanel[m].m_iLA = m_nNodes;
					m_Node[m_nNodes].Copy(pWing->m_Surface[j].LA);
					m_nNodes++;
				}

				if(n1>=0) {
					pPanel[m].m_iTA = n1;
				}
				else {
					pPanel[m].m_iTA = m_nNodes;
					m_Node[m_nNodes].Copy(pWing->m_Surface[j].TA);
					m_nNodes++;
				}

				if(n2>=0) {
					pPanel[m].m_iLB = n2;
				}
				else {
					pPanel[m].m_iLB = m_nNodes;
					m_Node[m_nNodes].Copy(pWing->m_Surface[j].LB);
					m_nNodes++;
				}

				if(n3 >=0) {
					pPanel[m].m_iTB = n3;
				}
				else {
					pPanel[m].m_iTB = m_nNodes;
					m_Node[m_nNodes].Copy(pWing->m_Surface[j].TB);
					m_nNodes++;
				}

				LATB.x = pWing->m_Surface[j].TB.x - pWing->m_Surface[j].LA.x;
				LATB.y = pWing->m_Surface[j].TB.y - pWing->m_Surface[j].LA.y;
				LATB.z = pWing->m_Surface[j].TB.z - pWing->m_Surface[j].LA.z;
				TALB.x = pWing->m_Surface[j].LB.x - pWing->m_Surface[j].TA.x;
				TALB.y = pWing->m_Surface[j].LB.y - pWing->m_Surface[j].TA.y;
				TALB.z = pWing->m_Surface[j].LB.z - pWing->m_Surface[j].TA.z;

				if(m_pCurWPolar && !m_pCurWPolar->m_bMiddle)
					pPanel[m].Normal = TALB * LATB;
				else 
					pPanel[m].Normal = LATB * TALB;

				pPanel[m].m_iPos = side;
				pPanel[m].Area =  pPanel[m].Normal.VAbs()/2.0;
				pPanel[m].Normal.Normalize();
				pPanel[m].SetVortex(      pWing->m_Surface[j].LA, pWing->m_Surface[j].LB, pWing->m_Surface[j].TA, pWing->m_Surface[j].TB);
				pPanel[m].SetFrame(pWing->m_Surface[j].LA, pWing->m_Surface[j].LB, pWing->m_Surface[j].TA, pWing->m_Surface[j].TB);

				// set neighbour panels
				// valid only for Panel 2-sided Analysis
				// we are on the bottom or middle surface
				pPanel[m].m_iPD = m-1;
				pPanel[m].m_iPU = m+1;
				if(l==0)								pPanel[m].m_iPD = -1;// no panel downstream
				if(l==pWing->m_Surface[j].m_NXPanels-1)	pPanel[m].m_iPU = -1;// no panel upstream
				
				pPanel[m].m_iPL = m - 2*pWing->m_Surface[j].m_NXPanels;//assuming all wing panels have same chordwise distribution
				pPanel[m].m_iPR = m + 2*pWing->m_Surface[j].m_NXPanels;//assuming all wing panels have same chordwise distribution
				if(pPanel[m].m_iPL<0)												pPanel[m].m_iPL = -1;
				if(k==pWing->m_Surface[j].m_NYPanels-1 && j== pWing->m_NSurfaces-1)	pPanel[m].m_iPR = -1;

				if(m_pCurWPolar && pPanel[m].m_bIsTrailing && m_pCurWPolar->m_AnalysisType==3){
					pPanel[m].m_iWake = m_WakeSize;//next wake element
					pPanel[m].m_iWakeColumn = NWakeColumn;
					CreateWakeElems(m);
				}
				m++;
			}

			if(m_pCurWPolar && (m_pCurWPolar->m_AnalysisType==3 || !m_pCurWPolar->m_bMiddle)){
				side = 1; //next upper surf, as recommended by K&P 
				//from L.E. to T.E.
				for (l=pWing->m_Surface[j].m_NXPanels-1;l>=0; l--){
					pWing->m_Surface[j].GetPanel(k,l,N,side);
					n0 = IsNode(pWing->m_Surface[j].LA);
					n1 = IsNode(pWing->m_Surface[j].TA);
					n2 = IsNode(pWing->m_Surface[j].LB);
					n3 = IsNode(pWing->m_Surface[j].TB);

					if(l==0)								pPanel[m].m_bIsTrailing = true;	
					if(l==pWing->m_Surface[j].m_NXPanels-1) pPanel[m].m_bIsLeading  = true;

					if(n0>=0) 
						pPanel[m].m_iLA = n0;
					else {
						pPanel[m].m_iLA = m_nNodes;
						m_Node[m_nNodes].Copy(pWing->m_Surface[j].LA);
						m_nNodes++;
					}

					if(n1>=0) 
						pPanel[m].m_iTA = n1;
					else {
						pPanel[m].m_iTA = m_nNodes;
						m_Node[m_nNodes].Copy(pWing->m_Surface[j].TA);
						m_nNodes++;
					}

					if(n2>=0) 
						pPanel[m].m_iLB = n2;
					else {
						pPanel[m].m_iLB = m_nNodes;
						m_Node[m_nNodes].Copy(pWing->m_Surface[j].LB);
						m_nNodes++;
					}

					if(n3 >=0)
						pPanel[m].m_iTB = n3;
					else {
						pPanel[m].m_iTB = m_nNodes;
						m_Node[m_nNodes].Copy(pWing->m_Surface[j].TB);
						m_nNodes++;
					}
			
					LATB.x = pWing->m_Surface[j].TB.x - pWing->m_Surface[j].LA.x;
					LATB.y = pWing->m_Surface[j].TB.y - pWing->m_Surface[j].LA.y;
					LATB.z = pWing->m_Surface[j].TB.z - pWing->m_Surface[j].LA.z;
					TALB.x = pWing->m_Surface[j].LB.x - pWing->m_Surface[j].TA.x;
					TALB.y = pWing->m_Surface[j].LB.y - pWing->m_Surface[j].TA.y;
					TALB.z = pWing->m_Surface[j].LB.z - pWing->m_Surface[j].TA.z;

					pPanel[m].Normal = LATB * TALB;
					pPanel[m].m_iPos = side;
					pPanel[m].Area =  pPanel[m].Normal.VAbs()/2.0;
					pPanel[m].Normal.Normalize();
					pPanel[m].SetVortex(      pWing->m_Surface[j].LA, pWing->m_Surface[j].LB, pWing->m_Surface[j].TA, pWing->m_Surface[j].TB);
					pPanel[m].SetFrame(pWing->m_Surface[j].LA, pWing->m_Surface[j].LB, pWing->m_Surface[j].TA, pWing->m_Surface[j].TB);
					// set neighbour panels
					// valid only for Panel 2-sided Analysis
					// we are on the top surface
					pPanel[m].m_iPD = m+1;
					pPanel[m].m_iPU = m-1;
					if(l==0)								pPanel[m].m_iPD = -1;// no panel downstream
					if(l==pWing->m_Surface[j].m_NXPanels-1)	pPanel[m].m_iPU = -1;// no panel upstream
					
					pPanel[m].m_iPL = m - 2*pWing->m_Surface[j].m_NXPanels;//assuming all wing panels have same chordwise distribution
					pPanel[m].m_iPR = m + 2*pWing->m_Surface[j].m_NXPanels;//assuming all wing panels have same chordwise distribution
					if(k==0 && j==0)													pPanel[m].m_iPL = -1;
					if(k==pWing->m_Surface[j].m_NYPanels-1 && j== pWing->m_NSurfaces-1)	pPanel[m].m_iPR = -1;

					if(m_pCurWPolar && pPanel[m].m_bIsTrailing && m_pCurWPolar->m_AnalysisType==3){
						pPanel[m].m_iWake = m_WakeSize;//next wake element
						pPanel[m].m_iWakeColumn = NWakeColumn;
//						CreateWakeElems(m);
					}	
					m++;
				}
				NWakeColumn++;
			}
		}
	}


	if(m_pCurWPolar && m_pCurWPolar->m_AnalysisType==3){
		//then add side panels
		//left surface
		for (l=0; l< pWing->m_Surface[0].m_NXPanels; l++){
			
			pPanel[m].m_bIsTrailing = false;
			pPanel[m].m_bIsLeading  = false;

			pWing->m_Surface[0].GetPanel(0,l,N,-1);
			LA.Copy(pWing->m_Surface[0].LA);
			TA.Copy(pWing->m_Surface[0].TA);
			pWing->m_Surface[0].GetPanel(0,l,N,+1);
			LB.Copy(pWing->m_Surface[0].LA);
			TB.Copy(pWing->m_Surface[0].TA);

			n0 = IsNode(LA);
			if(n0>=0) 	pPanel[m].m_iLA = n0;
			else {
				pPanel[m].m_iLA = m_nNodes;
				m_Node[m_nNodes].Copy(LA);
				m_nNodes++;
			}

			n1 = IsNode(TA);
			if(n1>=0) 	pPanel[m].m_iTA = n1;
			else {
				pPanel[m].m_iTA = m_nNodes;
				m_Node[m_nNodes].Copy(TA);
				m_nNodes++;
			}

			n2 = IsNode(LB);
			if(n2>=0) 	pPanel[m].m_iLB = n2;
			else {
				pPanel[m].m_iLB = m_nNodes;
				m_Node[m_nNodes].Copy(LB);
				m_nNodes++;
			}

			n3 = IsNode(TB); 
			if(n3>=0) 	pPanel[m].m_iTB = n3;
			else {
				pPanel[m].m_iTB = m_nNodes;
				m_Node[m_nNodes].Copy(TB);
				m_nNodes++;
			}
			
			LATB.x = TB.x - LA.x;
			LATB.y = TB.y - LA.y;
			LATB.z = TB.z - LA.z;
			TALB.x = LB.x - TA.x;
			TALB.y = LB.y - TA.y;
			TALB.z = LB.z - TA.z;

			pPanel[m].Normal = LATB * TALB;

			pPanel[m].m_iPos = 1;
			pPanel[m].Area =  pPanel[m].Normal.VAbs()/2.0;
			pPanel[m].Normal.Normalize();
			pPanel[m].SetVortex( LA, LB, TA, TB);
			pPanel[m].SetFrame(LA, LB, TA, TB);
			pPanel[m].m_iWake = -1;

			m++;
		}
	}

	if(m_pCurWPolar && m_pCurWPolar->m_AnalysisType==3){
		//right surface
		j = pWing->m_NSurfaces-1;
		k = pWing->m_Surface[j].m_NYPanels-1;
		for (l=0; l< pWing->m_Surface[j].m_NXPanels; l++){

//			if(l==0)							pPanel[m].m_bIsTrailing = true;
//			if(l==m_Surface[j].m_NXPanels-1) 	pPanel[m].m_bIsLeading  = true;
			pPanel[m].m_bIsTrailing = false;
			pPanel[m].m_bIsLeading  = false;

			pWing->m_Surface[j].GetPanel(k,l,N,1);
			LA.Copy(pWing->m_Surface[j].LB);
			TA.Copy(pWing->m_Surface[j].TB);
//			if(!pPanel[m].m_bIsLeading)	TA.z = LA.z;//no skewing
//			else						LA.z = TA.z;//no skewing
			pWing->m_Surface[j].GetPanel(k,l,N,-1);
			LB.Copy(pWing->m_Surface[j].LB);
			TB.Copy(pWing->m_Surface[j].TB);
//			if(!pPanel[m].m_bIsLeading)	TB.z = LB.z;//no skewing
//			else						LB.z = TB.z;//no skewing

			n0 = IsNode(LA);//answer should be yes 
			if(n0>=0) 				pPanel[m].m_iLA = n0;
			else {
				pPanel[m].m_iLA = m_nNodes;
				m_Node[m_nNodes].Copy(LA);
				m_nNodes++;
			}
			n1 = IsNode(TA);//answer should be yes 
			if(n1>=0) 				pPanel[m].m_iTA = n1;
			else {
				pPanel[m].m_iTA = m_nNodes;
				m_Node[m_nNodes].Copy(TA);
				m_nNodes++;
			}
			n2 = IsNode(LB);//answer should be yes 
			if(n2>=0) 				pPanel[m].m_iLB = n2;
			else {
				pPanel[m].m_iLB = m_nNodes;
				m_Node[m_nNodes].Copy(LB);
				m_nNodes++;
			}
			n3 = IsNode(TB);//answer should be yes 		
			if(n3>=0)				pPanel[m].m_iTB = n3;
			else {
				pPanel[m].m_iTB = m_nNodes;
				m_Node[m_nNodes].Copy(TB);
				m_nNodes++;
			}

			LATB.x = TB.x - LA.x;
			LATB.y = TB.y - LA.y;
			LATB.z = TB.z - LA.z;
			TALB.x = LB.x - TA.x;
			TALB.y = LB.y - TA.y;
			TALB.z = LB.z - TA.z;

			pPanel[m].Normal =   LATB*TALB;

			pPanel[m].m_iPos = 1;
			pPanel[m].Area =  pPanel[m].Normal.VAbs()/2.0;
			pPanel[m].Normal.Normalize();
			pPanel[m].SetVortex( LA, LB, TA, TB);
			pPanel[m].SetFrame(LA, LB, TA, TB);
			pPanel[m].m_iWake = -1;

			m++;
		}
	}

	pWing->m_VLMMatSize  = m;
	m_VLMMatSize +=m;
	m_NWakeColumn = NWakeColumn;

	return true;
}


bool CMiarex::CreateWakeElems(int pw)
{
	if(!m_pCurWPolar) return false;
	if(!m_Panel[pw].m_bIsTrailing) return false;
	//creates elements trailing panel p
	int n0, n1,n2, n3;
	double dx;
	int mw = m_WakeSize;// number of wake panels
	CVector N, LATB, TALB;
	CVector LA, LB, TA,TB;//wake panel's corner points
	
	dx = m_FirstPanelSize*m_pCurWing->m_MAChord;
	TA = m_Node[m_Panel[pw].m_iTA];
	TB = m_Node[m_Panel[pw].m_iTB];

	for (int l=0; l<m_NXWakePanels; l++){

		LA = TA;
		LB = TB;
		TA.x += dx;
		TB.x += dx;
		dx *= m_WakePanelFactor;

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
		m_WakePanel[mw].SetVortex(LA, LB, TA, TB);
		m_WakePanel[mw].SetFrame(LA,LB, TA, TB);

		if(l==0)					m_WakePanel[mw].m_iPD = -1;// no panel downstream
		else if(l==m_NXWakePanels)	m_WakePanel[mw].m_iPU = -1;// no panel upstream
		else						m_WakePanel[mw].m_iPD = mw+1;
		
		m_WakePanel[mw].m_iPL = -1;
		m_WakePanel[mw].m_iPR = -1;
		
		mw++;
	}

	m_WakeSize = mw;

	return true;
}

int CMiarex::IsWakeNode(CVector Pt)
{
/*	for (int i=0; i<m_nWakeNodes; i++){
		if(Pt.IsSame(m_WakeNode[i])) return i;
	}*/
	for (int i=0; i<m_nWakeNodes; i++){
		if(Pt.IsSame(m_WakeNode[i])) return i;
	}
	return -1;
}

bool CMiarex::VLMIsSameSide(int p, int pp)
{
	if(m_Panel[p].m_iPos==m_Panel[pp].m_iPos) return true;
	return false;
}

int CMiarex::IsNode(CVector Pt)
{
	for (int i=0; i<m_nNodes; i++){
		if(Pt.IsSame(m_Node[i])) return i;
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

	if(m_pCurStab){
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
	CWing* pWing;
	for (int i=0; i<m_poaWing->GetSize(); i++){
		pWing = (CWing*)m_poaWing->GetAt(i);
		if (pWing->m_WingName == WingName) return pWing;
	}
	return NULL;
}


CPlane * CMiarex::GetPlane(CString PlaneName)
{
	CPlane* pPlane;
	for (int i=0; i<m_poaPlane->GetSize(); i++){
		pPlane = (CPlane*)m_poaPlane->GetAt(i);
		if (pPlane->m_PlaneName == PlaneName) return pPlane;
	}
	return NULL;
}
 
CPlane * CMiarex::CreatePlane()
{
	CPlane* pPlane = new CPlane(m_pFrame);
	if(!pPlane){
		AfxMessageBox("Not enough memory available to create the plane", MB_OK);
		return NULL;
	}
	pPlane->m_pMiarex = this;
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	pPlane->m_Wing.m_pFrame   = m_pFrame;
	pPlane->m_Wing.m_pMiarex  = &(pFrame->Miarex);
	pPlane->m_Wing2.m_pFrame  = m_pFrame;
	pPlane->m_Wing2.m_pMiarex = &(pFrame->Miarex);
	pPlane->m_Stab.m_pFrame   = m_pFrame;
	pPlane->m_Stab.m_pMiarex  = &(pFrame->Miarex);
	pPlane->m_Fin.m_pFrame    = m_pFrame;
	pPlane->m_Fin.m_pMiarex   = &(pFrame->Miarex);
	pPlane->m_poaWing         = m_poaWing;
	return pPlane;
}


CPlane* CMiarex::AddPlane(CPlane *pPlane)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CPlane *pOldPlane;
	CString strong;
	bool bExists   = false;
	bool bInserted = false;

	for (int i=0; i<m_poaPlane->GetSize(); i++){
		pOldPlane = (CPlane*)m_poaPlane->GetAt(i);
		if (pOldPlane->m_PlaneName == pPlane->m_PlaneName) {
			bExists = true;
			break;
		}
	}

	while(!bInserted){
		if(!bExists){ 
			for (int j=0; j<m_poaPlane->GetSize(); j++){
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
		else {
			//overwrite it all
			for (int l=0; l<m_poaPlane->GetSize(); l++){
				pOldPlane = (CPlane*)m_poaPlane->GetAt(l);
				if (pOldPlane->m_PlaneName == pPlane->m_PlaneName){
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
	CPlane* pPlane = CreatePlane();
	CPlane* pOldPlane = NULL;
	pPlane->m_poaWing = m_poaWing;
	pPlane->m_poaPlane = m_poaPlane;
	pPlane->m_bCheckName = true;
	pPlane->m_bCheckPanels = m_bCheckPlanePanels;
	pPlane->m_Wing.m_WingColor = m_WingColor;
	pPlane->m_Stab.m_WingColor = m_StabColor;
	pPlane->m_Fin.m_WingColor  = m_FinColor;

	if(IDOK == pPlane->DoModal()){
		m_WingColor = pPlane->m_Wing.m_WingColor ;
		m_StabColor = pPlane->m_Stab.m_WingColor;
		m_FinColor  = pPlane->m_Fin.m_WingColor;
		m_bCheckPlanePanels = pPlane->m_bCheckPanels;

		CMainFrame *pFrame = (CMainFrame*)m_pFrame;
		pFrame->SetSaveState(false);

		bool bExists = false;
		for(int i=0; i<m_poaPlane->GetSize(); i++){
			pOldPlane = (CPlane*)m_poaPlane->GetAt(i);
			if(pPlane->m_PlaneName == pOldPlane->m_PlaneName){
				bExists = true;
				AfxMessageBox("The name "+pPlane->m_PlaneName+" already exists", MB_OK);
				break;
			}
		}

		if(bExists){
			if(!SetModPlane(pPlane)){
				delete pPlane;
				return;
			}
		}

		m_pCurPlane = AddPlane(pPlane);
		SetUFO();

		UpdateUFOs();
	}
	else {
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
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(!m_pCurPlane)	return;
	CWPolar *pWPolar;
	CPOpp* pPOpp;
	bool bHasResults = false;
	for (int i=0; i< m_poaWPolar->GetSize(); i++){
		pWPolar = (CWPolar*)m_poaWPolar->GetAt(i);
		if(pWPolar->m_Alpha.GetSize() && pWPolar->m_UFOName == m_pCurPlane->m_PlaneName){
			bHasResults = true;
			break;
		}
	}

	for (i=0; i< m_poaPOpp->GetSize(); i++){
		pPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
		if(pPOpp->m_PlaneName == m_pCurPlane->m_PlaneName){
			bHasResults = true;
			break;
		}
	}
 
	//save a copy just in case user changes his mind (users !...)
	CPlane* pSavePlane= new CPlane(m_pFrame);
	pSavePlane->m_pMiarex = this;
	pSavePlane->Duplicate(m_pCurPlane);
	m_pCurPlane->m_bCheckName = false;

	if(IDOK == m_pCurPlane->DoModal()){
		if(m_pCurPlane->m_bChanged){
			if(bHasResults){
				CModDlg dlg;
				dlg.m_Question = "The modification will erase all results associated to this Plane.\nContinue ?";
				int Ans = (int)dlg.DoModal();			
				if (Ans == IDCANCEL) {
					//restore geometry
					m_pCurPlane->Duplicate(pSavePlane);
					delete pSavePlane; // clean up	
					return;
				}
				else if(Ans==20){
					CPlane* pNewPlane= new CPlane(m_pFrame);
					pNewPlane->m_pMiarex = this;
					pNewPlane->Duplicate(m_pCurPlane);
					if(!SetModPlane(pNewPlane)){
						delete pNewPlane;
					}
					else {
						m_pCurPlane = AddPlane(pNewPlane);
					}
					SetUFO();
					UpdateUFOs();
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
		UpdateWPlrs();
		OnAdjustToWing();

	}
	else { // restore original
		m_pCurPlane->Duplicate(pSavePlane);
	}
	delete pSavePlane; // clean up
}


void CMiarex::AddPOpp(double *Gamma, CPOpp *pPOpp)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CPOpp *pOldPOpp;
	CWOpp *pWOpp;
	int i,j,p;
	double Cb = 0.0;

	if(!pPOpp){
		// loads PAnalysis results in the POpPoint
		// else we're adding from serialization
		if(!m_VLMDlg.m_bConverged) return;
		if(!m_bKeepOutOpps && m_VLMDlg.m_bPointOut) return;

		pPOpp = new CPOpp();
		if(pPOpp == NULL) {
			AfxMessageBox("Not enough memory to store the OpPoint\n", MB_OK);
			return;
		}

		pPOpp->m_Color = pFrame->GetColor(6);
		bool bFound;
		for(i=0; i<30;i++){
			bFound = false;
			for (j=0; j<m_poaWOpp->GetSize();j++){
				pWOpp = (CWOpp*)m_poaWOpp->GetAt(j);
				if(pWOpp->m_Color == pFrame->m_crColors[i]) bFound = true;
			}
			for (j=0; j<m_poaPOpp->GetSize();j++){
				pOldPOpp = (CPOpp*)m_poaPOpp->GetAt(j);
				if(pOldPOpp->m_Color == pFrame->m_crColors[i]) bFound = true;
			}
			if(!bFound) {
				pPOpp->m_Color = pFrame->m_crColors[i];
				break;
			}
		}


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
		pPOpp->m_bMiddle             = m_pCurWPolar->m_bMiddle;
		pPOpp->m_PlrName             = m_pCurWPolar->m_PlrName;
		pPOpp->m_NStation            = m_pCurWing->m_NStation;
		pPOpp->m_NVLMPanels          = m_VLMDlg.m_VLMMatSize;
		pPOpp->m_Alpha               = m_VLMDlg.m_AlphaCalc;
		pPOpp->m_QInf                = m_VLMDlg.m_QInf;
		pPOpp->m_Type                = m_pCurWPolar->m_Type;

		pWOpp->m_Type                = m_pCurWPolar->m_Type;
		pWOpp->m_Alpha               = m_VLMDlg.m_Alpha;
		pWOpp->m_QInf                = m_VLMDlg.m_QInf;
		pWOpp->m_CL                  = m_VLMDlg.m_CL;
		pWOpp->m_InducedDrag         = m_VLMDlg.m_InducedDrag;
		pWOpp->m_ViscousDrag         = m_VLMDlg.m_ViscousDrag;
		pWOpp->m_InducedYawingMoment = m_VLMDlg.m_IYm;
		pWOpp->m_GYm                 = m_VLMDlg.m_GYm;
		pWOpp->m_PitchingMoment      = m_VLMDlg.m_TCm;
		pWOpp->m_VCm                 = m_VLMDlg.m_VCm;
		pWOpp->m_GCm                 = m_VLMDlg.m_GCm;
		pWOpp->m_RollingMoment       = m_VLMDlg.m_Rm;
		pWOpp->m_XCP                 = m_VLMDlg.m_XCP;
		pWOpp->m_YCP                 = m_VLMDlg.m_YCP;

		pWOpp->m_nFlaps = m_pCurWing->m_nFlaps;
		for (i=0; i<m_pCurWing->m_nFlaps; i++){
			pWOpp->m_FlapMoment[i] = m_pCurWing->m_FlapMoment[i];
		}

		memcpy(pPOpp->m_Cp, m_VLMDlg.m_Cp,  sizeof(pPOpp->m_Cp));
		if(Gamma)	memcpy(pPOpp->m_G,  Gamma,   sizeof(pPOpp->m_G));

		for (int l=0; l<m_pCurWing->m_NStation; l++){
			Cb = __max(Cb, m_pCurWing->m_BendingMoment[l]);
		}
		pWOpp->m_MaxBending = Cb;

		p = 0;
		for(i=0; i<m_pCurWing->m_VLMMatSize; i++){
			pPOpp->m_WingWOpp.m_Cp[p] = pPOpp->m_Cp[p];
			p++;
		}
		if(m_pCurWing2){
			for(i=0; i<m_pCurWing2->m_VLMMatSize; i++){
				pPOpp->m_Wing2WOpp.m_Cp[i] = pPOpp->m_Cp[p];
				p++;
			}
		}
		if(m_pCurStab){
			for(i=0; i<m_pCurStab->m_VLMMatSize; i++){
				pPOpp->m_StabWOpp.m_Cp[i] = pPOpp->m_Cp[p];
				p++;
			}
		}
		if(m_pCurFin){
			for(i=0; i<m_pCurFin->m_VLMMatSize; i++){
				pPOpp->m_FinWOpp.m_Cp[i] = pPOpp->m_Cp[p];
				p++;
			}
		}
		memcpy(pPOpp->m_WingWOpp.m_ICd, m_VLMDlg.m_ICd, sizeof(pPOpp->m_WingWOpp.m_ICd));
		memcpy(pPOpp->m_WingWOpp.m_Ai,  m_VLMDlg.m_Ai,  sizeof(pPOpp->m_WingWOpp.m_Ai));

		m_pCurWPolar->AddPoint(pPOpp);

		m_pCurWOpp = &pPOpp->m_WingWOpp;
		if(m_iView==2) {
			CreateWPolarCurves();
			UpdateView();
		}
	}
	pFrame->SetSaveState(false);

	bool bIsInserted = false;
	
	// add the POpPoint to the POpPoint Array for the current PlaneName
	if(m_bStoreWOpp){
		for (int i=0; i<m_poaPOpp->GetSize(); i++){
			pOldPOpp = (CPOpp*)m_poaPOpp->GetAt(i);
			if (pPOpp->m_PlaneName == pOldPOpp->m_PlaneName){
				if (pPOpp->m_PlrName == pOldPOpp->m_PlrName){
					if(pPOpp->m_Type !=4){
						if(fabs(pPOpp->m_Alpha - pOldPOpp->m_Alpha)<0.001){
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
						else if (pPOpp->m_Alpha > pOldPOpp->m_Alpha){
							//insert point
							m_poaPOpp->InsertAt(i, pPOpp);
							bIsInserted = true;
							i = (int)m_poaPOpp->GetSize();// to break
						}
					}
					else{
						if(fabs(pPOpp->m_QInf - pOldPOpp->m_QInf)<0.001){
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
		m_pCurPOpp = pPOpp;
	}
	else {
		delete pPOpp;
		m_pCurPOpp = NULL;
		m_pCurWOpp = NULL;
	}
}


void CMiarex::CreateWOpp(CWOpp *pWOpp, CWing *pWing)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CWOpp *pOldWOpp = NULL;
	bool bFound;
	for(int i=0; i<30;i++){
		bFound = false;
		for (int j=0; j<m_poaWOpp->GetSize();j++){
			pOldWOpp = (CWOpp*)m_poaWOpp->GetAt(j);
			if(pOldWOpp->m_Color == pFrame->m_crColors[i]) bFound = true;
		}
		if(!bFound) {
			pWOpp->m_Color = pFrame->m_crColors[i];
			break;
		}
	}
	pWOpp->m_WingName            = pWing->m_WingName;
	pWOpp->m_PlrName             = m_pCurWPolar->m_PlrName;
	pWOpp->m_bMiddle             = m_pCurWPolar->m_bMiddle;
	pWOpp->m_Type                = m_pCurWPolar->m_Type;
	pWOpp->m_bVLM1               = m_pCurWPolar->m_bVLM1;
	pWOpp->m_NStation            = pWing->m_NStation;
	pWOpp->m_NVLMPanels          = pWing->m_VLMMatSize;
	pWOpp->m_bOut                = m_VLMDlg.m_bPointOut;
//	pWOpp->m_bLLT                = false;
	pWOpp->m_AnalysisType        = m_pCurWPolar->m_AnalysisType;
	pWOpp->m_Alpha               = m_VLMDlg.m_Alpha;
	pWOpp->m_QInf                = m_VLMDlg.m_QInf;
	pWOpp->m_Weight              = m_pCurWPolar->m_Weight;
	pWOpp->m_Span                = pWing->m_Span;
	pWOpp->m_MAChord             = pWing->m_MAChord;
	pWOpp->m_CL                  = pWing->m_CL;
	pWOpp->m_InducedDrag         = pWing->m_InducedDrag;
	pWOpp->m_ViscousDrag         = pWing->m_ViscousDrag;
	pWOpp->m_PitchingMoment      = pWing->m_PitchingMoment;
	pWOpp->m_VCm                 = pWing->m_VCm;
	pWOpp->m_GCm                 = pWing->m_GCm;
	pWOpp->m_GYm                 = pWing->m_GYm;
	pWOpp->m_InducedYawingMoment = pWing->m_IYm;
	pWOpp->m_RollingMoment       = pWing->m_RollingMoment;
	pWOpp->m_XCP                 = pWing->m_XCP;
	pWOpp->m_YCP                 = pWing->m_YCP;

	memcpy(pWOpp->m_Ai,            pWing->m_Ai,            sizeof(pWing->m_Ai));
	memcpy(pWOpp->m_Cl,            pWing->m_Cl,            sizeof(pWing->m_Cl));
	memcpy(pWOpp->m_PCd,           pWing->m_PCd,           sizeof(pWing->m_PCd));
	memcpy(pWOpp->m_ICd,           pWing->m_ICd,           sizeof(pWing->m_ICd));
	memcpy(pWOpp->m_Cm,            pWing->m_Cm,            sizeof(pWing->m_Cm));
	memcpy(pWOpp->m_CmAirf,        pWing->m_CmAirf,        sizeof(pWing->m_CmAirf));
	memcpy(pWOpp->m_CmGeom,        pWing->m_CmGeom,        sizeof(pWing->m_CmGeom));
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
//	memcpy(pWOpp->m_Cp,            pWing->m_Cp,            sizeof(pWing->m_Cp));

	double Cb =0.0;

	for (int l=0; l<pWing->m_NStation; l++){
		pWOpp->m_SpanPos[l] = pWing->m_SpanPos[l];
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
	CreateWOppCurves();
	UpdateView();
}

void CMiarex::OnShowElevator() 
{
	m_bShowStab = !m_bShowStab;
	CheckMenus();
	CreateWOppCurves();
	UpdateView();
}

void CMiarex::OnShowFin() 
{
	m_bShowFin = !m_bShowFin;
	CheckMenus();
	CreateWOppCurves();
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
	else if(m_iView==1){
		SetWGraphScale();

	}
	else if(m_iView==4){
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
	m_FlowLinesDlg.ShowWindow(SW_SHOW);
}

void CMiarex::OnStreamLines() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(!m_pCurWOpp ||
	   (m_pCurWOpp && m_pCurWOpp->m_AnalysisType==1) ||
	   !pFrame->m_bStream){
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

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	m_PanelDlg.m_pMiarex = this;
	m_PanelDlg.m_pFrame  = m_pFrame;
	m_PanelDlg.m_bSequence      = bSequence;
	m_PanelDlg.m_pWPolar        = m_pCurWPolar;

	if(m_pCurWPolar->m_Type!=4){
		m_PanelDlg.m_QInf       = m_pCurWPolar->m_QInf;
		m_PanelDlg.m_Alpha      = V0;
		m_PanelDlg.m_AlphaMax   = VMax;
		m_PanelDlg.m_DeltaAlpha = VDelta;
	}
	else{
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
	m_PanelDlg.m_MatSize = m_VLMMatSize;
	m_PanelDlg.m_NWakeColumn  = m_NWakeColumn;
	m_PanelDlg.m_NXWakePanels = m_NXWakePanels;

	m_PanelDlg.m_VersionName = pFrame->m_VersionName;

	m_PanelDlg.DoModal();

	if(m_iView!=2) SetWOpp(false, V0);
 
	else CreateWPolarCurves();
	UpdateWOpps();
	SetWOpp(true);
	UpdateView();
}

void CMiarex::VLMAnalyze(double V0, double VMax, double VDelta, bool bSequence, bool bInitCalc)
{
	if(!m_pCurWing || !m_pCurWPolar) return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	m_bVLMFinished = false;
	if(m_pCurWPolar->m_bWakeRollUp && (m_bResetWake||bInitCalc)){
//		VLMCreateWakeElems(NULL);
		m_VLMDlg.m_NXWakePanels   = m_NXWakePanels;
		m_bSetNewWake  = false;
		m_bResetglWake = true;
		UpdateView();
	}
	else if(m_pCurWOpp){
		m_VLMDlg.m_NXWakePanels   = m_pCurWOpp->m_NXWakePanels;
		m_NXWakePanels            = m_pCurWOpp->m_NXWakePanels;
	}

	m_VLMDlg.m_pMiarex = this;
	m_VLMDlg.m_pFrame  = m_pFrame;
	m_VLMDlg.m_bWakeRollUp    = m_pCurWPolar->m_bWakeRollUp;
	m_VLMDlg.m_MaxWakeIter    = m_MaxWakeIter;
	m_VLMDlg.m_WakeInterNodes = m_WakeInterNodes;
	m_VLMDlg.m_CoreSize       = m_CoreSize;
	m_VLMDlg.m_NXWakePanels   = m_NXWakePanels;
	m_VLMDlg.m_bSequence      = bSequence;
	m_VLMDlg.m_pWPolar        = m_pCurWPolar;

	if(m_pCurWPolar->m_Type!=4){
		m_VLMDlg.m_QInf       = m_pCurWPolar->m_QInf;
		m_VLMDlg.m_Alpha      = V0;
		m_VLMDlg.m_AlphaMax   = VMax;
		m_VLMDlg.m_DeltaAlpha = VDelta;
	}
	else{
		m_VLMDlg.m_Alpha      = m_pCurWPolar->m_ASpec;
		m_VLMDlg.m_QInf       = V0;
		m_VLMDlg.m_QInfMax    = VMax;
		m_VLMDlg.m_DeltaQInf  = VDelta;
	}

	m_VLMDlg.DoModal();

	if(m_iView!=2) SetWOpp(false, V0);
 
	else CreateWPolarCurves();
	UpdateWOpps();
	SetWOpp(true);
	UpdateView();
}

void CMiarex::OnWingAnalysis() 
{
	if(!m_pCurWing) return;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	m_WngAnalysis.m_pWing       = m_pCurWing;
	m_WngAnalysis.m_pPlane      = m_pCurPlane;
	m_WngAnalysis.m_pParent     = m_pFrame;
	m_WngAnalysis.m_poaXPolar   = &pFrame->m_oaWPolar;

	if(m_pCurPlane) 
		m_WngAnalysis.m_UFOName   = m_pCurPlane->m_PlaneName;
	else
		m_WngAnalysis.m_UFOName   = m_pCurWing->m_WingName;


	CWPolar* pCurWPolar       = new CWPolar(m_pFrame);
	if (m_pCurPlane) pCurWPolar->m_UFOName = m_pCurPlane->m_PlaneName;
	else             pCurWPolar->m_UFOName = m_pCurWing->m_WingName;
	pCurWPolar->m_WArea        = m_pCurWing->m_Area;
	pCurWPolar->m_WMAChord     = m_pCurWing->m_MAChord;
	pCurWPolar->m_WSpan        = m_pCurWing->m_Span;
	m_WngAnalysis.m_pWPolar    = pCurWPolar;


	if (m_WngAnalysis.DoModal() == IDOK){
		//Then add WPolar to array

		pCurWPolar->m_Type         = m_WngAnalysis.m_Type;
		pCurWPolar->m_QInf         = m_WngAnalysis.m_QInf;
		pCurWPolar->m_Weight       = m_WngAnalysis.m_Weight;
		pCurWPolar->m_XCmRef       = m_WngAnalysis.m_XCmRef;
		pCurWPolar->m_ASpec        = m_WngAnalysis.m_Alpha;
		pCurWPolar->m_PlrName      = m_WngAnalysis.m_WPolarName;
		pCurWPolar->m_Density      = m_WngAnalysis.m_Density;
		pCurWPolar->m_Viscosity    = m_WngAnalysis.m_Viscosity;
		pCurWPolar->m_bVLM1        = m_WngAnalysis.m_bVLM1;
		pCurWPolar->m_bTiltedGeom  = m_WngAnalysis.m_bTiltedGeom;
		pCurWPolar->m_bWakeRollUp  = m_WngAnalysis.m_bWakeRollUp;
		pCurWPolar->m_bViscous     = m_WngAnalysis.m_bViscous;
		pCurWPolar->m_AnalysisType = m_WngAnalysis.m_AnalysisType;
		pCurWPolar->m_bMiddle      = m_WngAnalysis.m_bMiddle;
		pCurWPolar->m_bGround      = m_WngAnalysis.m_bGround;
		pCurWPolar->m_Height       = m_WngAnalysis.m_Height;
		
		pCurWPolar->m_bPolar       = true;

		pCurWPolar->m_Color = pFrame->GetColor(4);
		CWPolar *pWPolar;
		bool bFound;
		for(int i=0; i<30;i++){
			bFound = false;
			for (int j=0; j<m_poaWPolar->GetSize();j++){
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
		
		SetWPlr();
		UpdateWPlrs();
		UpdateView();
		pFrame->m_WOperDlgBar.m_ctrlAnalyze.SetFocus();
	}
	else {
		delete pCurWPolar;
	}
}

