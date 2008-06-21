/****************************************************************************

    CMainFrame Class
	Copyright (C) 2005 André Deperrois xflr5@yahoo.com

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

// CMainFrame.cpp : implementation of the CChildView class
// The XFLR5 project uses the MainFrame/View architecture provided by VC++
// This class creates the application objects AFoil, XInverse, XFLR5 and Miarex
// It creates the various toolbars for all applications
// It stores the arrays for Foils, Wing, and Polars, and gives the Application objects
// their addresses in the corresponding pointers/
// It processes the Project commands (new, delete, etc.)
//

#include "stdafx.h"
#include "../X-FLR5.h"
#include "../misc/SettingsDlg.h"
#include "../Miarex/WingDlg.h"
#include "../Miarex/PlaneDlg.h"
#include "../misc/NameDlg.h"
#include "../misc/ToolBarDlg.h"
#include "../misc/EditPlrDlg.h"
#include "../misc/SaveOptionsDlg.h"
#include "../misc/UnitsDlg.h"
#include "../Miarex/POpp.h"
#include ".\MainFrm.h"
#include "../misc/MessageDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_COPYDATA()
	ON_WM_CONTEXTMENU()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(IDM_XFLR5, OnXDirect)
	ON_COMMAND(IDM_TOOLBARS, OnToolbars)
	ON_COMMAND(IDM_FULLINVERSE, OnFullInverse)
	ON_COMMAND(IDM_PLANEPREFS, OnPlanePrefs)
	ON_COMMAND(IDM_MIXEDINVERSE, OnMixedInverse)
	ON_COMMAND(IDM_MIAREX, OnMiarex)
	ON_COMMAND(IDM_LOADFILE, OnAppOpen)
	ON_COMMAND(IDT_LOAD, OnAppOpen)
	ON_COMMAND(ID_APP_OPEN, OnAppOpen)
	ON_COMMAND(IDM_UNITS, OnUnits)
	ON_COMMAND(IDM_NEWPROJECT, OnNewProject)
	ON_COMMAND(IDM_LOADPROJECT, OnLoadProject)
	ON_COMMAND(IDM_INSERTPROJECT, OnInsertProject)
	ON_COMMAND(IDM_SAVEPROJECT, OnSaveProject)
	ON_COMMAND(IDM_SAVEPROJECTAS, OnSaveProjectAs)
	ON_COMMAND(IDM_CLOSEPROJECT, OnCloseProject)
	ON_COMMAND(IDM_GUIDELINES, OnGuidelines)
	ON_COMMAND(IDM_LOGFILE, OnLogFile)
	ON_COMMAND(IDM_SAVEOPTIONS, OnSaveOptions)
	ON_COMMAND(IDM_FOILDIRECTDESIGN, OnFoilDirectDesign)
	ON_CBN_SELCHANGE(IDC_CBWING, OnSelChangeWing)
	ON_CBN_SELCHANGE(IDC_CBWPLR, OnSelChangeWPlr)
	ON_CBN_SELCHANGE(IDC_CBWOPP, OnSelChangeWOpp)
	ON_CBN_SELCHANGE(IDC_CBFOIL, OnSelChangeFoil)
	ON_CBN_SELCHANGE(IDC_CBPLR, OnSelChangePlr)
	ON_CBN_SELCHANGE(IDC_CBOPP, OnSelChangeOpp)
	ON_UPDATE_COMMAND_UI(IDC_ALPHAMAX, OnXDirectSequence)
	ON_UPDATE_COMMAND_UI(IDC_PARAM1, OnParam12)
	ON_UPDATE_COMMAND_UI(IDC_PARAM3, OnParam3)
	ON_UPDATE_COMMAND_UI(IDC_SEQUENCE, OnXDirectPolar)
	ON_UPDATE_COMMAND_UI(IDC_SHOWBL, OnXDirectDisplay)
	ON_UPDATE_COMMAND_UI(IDC_ANIMATE, OnAnimate)
	ON_UPDATE_COMMAND_UI(IDC_FOILCOLOR, OnFoilColor)
	ON_UPDATE_COMMAND_UI(IDC_AMAX, OnWAMax)
	ON_UPDATE_COMMAND_UI(IDC_INITCALC, OnInitCalc)
	ON_UPDATE_COMMAND_UI(IDC_DALPHA, OnDAlpha)
	ON_UPDATE_COMMAND_UI(IDC_HALFWING, OnHalfWing)
	ON_UPDATE_COMMAND_UI(IDC_3DOUTLINE, On3DOutline)
	ON_UPDATE_COMMAND_UI(IDC_3DSURFACES, On3DSurfaces)
	ON_UPDATE_COMMAND_UI(IDC_CL, OnMiarexW3DOppVLM)
	ON_UPDATE_COMMAND_UI(IDC_STREAM, OnStreamLines)
	ON_UPDATE_COMMAND_UI(IDC_SURFVELOCITIES, OnStreamLines)
	ON_UPDATE_COMMAND_UI(ID_INDPROJECT, OnIndicatorProject)
	ON_UPDATE_COMMAND_UI(IDC_CBWING, OnMiarexBar)
	ON_UPDATE_COMMAND_UI(IDC_CBWPLR, OnMiarexBar)
	ON_UPDATE_COMMAND_UI(IDC_CBWOPP, OnMiarexBar)
	ON_UPDATE_COMMAND_UI(IDT_UNDO, OnMiarexBar)
	ON_UPDATE_COMMAND_UI(IDT_REDO, OnMiarexBar)
	ON_UPDATE_COMMAND_UI(IDC_AMIN, OnMiarexWPolar)
	ON_UPDATE_COMMAND_UI(IDC_SHOWLIFT, OnWOpp)
	ON_UPDATE_COMMAND_UI(IDC_SHOWMOMENTS, OnWOpp)
	ON_UPDATE_COMMAND_UI(IDC_SHOWICD, OnWOpp)
	ON_UPDATE_COMMAND_UI(IDC_SHOWVCD, OnWOpp)
	ON_UPDATE_COMMAND_UI(IDC_3DAXES, On3DAxes)
	ON_UPDATE_COMMAND_UI(IDC_RENAME, OnAFoilCtrl)
	ON_UPDATE_COMMAND_UI(IDC_DELETE, OnAFoilCtrl)
	ON_UPDATE_COMMAND_UI(IDC_DUPLICATE, OnAFoilCtrl)
	ON_UPDATE_COMMAND_UI(IDC_QRESET, OnSpecInv)
	ON_UPDATE_COMMAND_UI(IDC_SPECINV, OnSpecFInv)
	ON_UPDATE_COMMAND_UI(IDC_MARK, OnSpecMInv)
	ON_UPDATE_COMMAND_UI(IDC_FILTERPARAM, OnSpecFInv)
	ON_UPDATE_COMMAND_UI(IDC_PARAM2, OnParam12)
	ON_UPDATE_COMMAND_UI(IDC_ANALYZE, OnXDirectPolar)
	ON_UPDATE_COMMAND_UI(IDC_VISCOUS, OnXDirectPolar)
	ON_UPDATE_COMMAND_UI(IDC_INITBL, OnXDirectPolar)
	ON_UPDATE_COMMAND_UI(IDC_STOREOPP, OnXDirectPolar)
	ON_UPDATE_COMMAND_UI(IDC_ALPHA, OnXDirectPolar)
	ON_UPDATE_COMMAND_UI(IDC_DELTAALPHA, OnXDirectSequence)
	ON_UPDATE_COMMAND_UI(IDC_SHOWPRESSURE, OnXDirectDisplay)
	ON_UPDATE_COMMAND_UI(IDC_SPEED, OnAnimate)
	ON_UPDATE_COMMAND_UI(IDC_SHOW, OnXMPolar)
	ON_UPDATE_COMMAND_UI(IDC_SHOWPOINTS, OnXMPolar)
	ON_UPDATE_COMMAND_UI(IDC_STYLE, OnXMPolar)
	ON_UPDATE_COMMAND_UI(IDC_WIDTH, OnXMPolar)
	ON_UPDATE_COMMAND_UI(IDC_COLORBTN, OnXMPolar)
	ON_UPDATE_COMMAND_UI(IDC_3DRESET, On3D)
	ON_UPDATE_COMMAND_UI(IDC_3DISO, On3D)
	ON_UPDATE_COMMAND_UI(IDC_3DLEFT, On3D)
	ON_UPDATE_COMMAND_UI(IDC_3DTOP, On3D)
	ON_UPDATE_COMMAND_UI(IDC_3DFRONT, On3D)
	ON_UPDATE_COMMAND_UI(IDC_PICKCENTER, On3D)
	ON_UPDATE_COMMAND_UI(IDC_DOWNWASH, OnWOpp)
	ON_UPDATE_COMMAND_UI(IDC_TRANSITIONS, OnWOpp)
	ON_UPDATE_COMMAND_UI(IDC_ALPHAPREC, OnMiarexWPolar)
	ON_UPDATE_COMMAND_UI(IDC_SPEC, OnMiarexWPolar)
	ON_UPDATE_COMMAND_UI(IDC_STOREWOPP, OnMiarexWPolar)
	ON_UPDATE_COMMAND_UI(IDC_KEEPOUTOPPS, OnMiarexWPolar)
	ON_UPDATE_COMMAND_UI(IDC_INITCALC, OnMiarexWPolar)
	ON_UPDATE_COMMAND_UI(IDC_WANALYSIS, OnMiarexWPolar)
	ON_UPDATE_COMMAND_UI(IDC_WSEQUENCE, OnMiarexWPolar)
	ON_UPDATE_COMMAND_UI(IDC_APPLYSPLINE, OnSpecInv)
	ON_UPDATE_COMMAND_UI(IDC_EXEC, OnSpecInv)
	ON_UPDATE_COMMAND_UI(IDC_SHOWSPLINE, OnSpecInv)
	ON_UPDATE_COMMAND_UI(IDC_NEWSPLINE, OnSpecInv)
	ON_UPDATE_COMMAND_UI(IDC_TANGENTSPLINE, OnSpecInv)
	ON_UPDATE_COMMAND_UI(IDC_SMOOTH, OnSpecInv)
	ON_UPDATE_COMMAND_UI(IDC_CPXX, OnSpecMInv)
	ON_UPDATE_COMMAND_UI(IDC_INVITER, OnSpecMInv)
	ON_UPDATE_COMMAND_UI(IDC_PERT, OnSpecFInv)
	ON_UPDATE_COMMAND_UI(IDC_SPECAL, OnSpecFInv)
	ON_UPDATE_COMMAND_UI(IDC_FILTER, OnSpecFInv)
	ON_UPDATE_COMMAND_UI(IDC_SPECCL, OnSpecFInv)
	ON_UPDATE_COMMAND_UI(IDC_TANGLE, OnSpecFInv)
	ON_UPDATE_COMMAND_UI(IDC_TGAPDX, OnSpecFInv)
	ON_UPDATE_COMMAND_UI(IDC_TGAPDY, OnSpecFInv)
	ON_UPDATE_COMMAND_UI(IDC_SYMM, OnSpecFInv)
	ON_UPDATE_COMMAND_UI(IDC_SPANSLIDE, OnSpanPos)
	ON_UPDATE_COMMAND_UI(IDC_SPANPOS, OnSpanPos)
	ON_UPDATE_COMMAND_UI(IDC_RESETCURVES, OnSpanPos)
	ON_UPDATE_COMMAND_UI(IDC_KEEPCURVE, OnSpanPos)
	ON_UPDATE_COMMAND_UI(IDC_BODYCOLOR, OnBodyCtrl)
	ON_UPDATE_COMMAND_UI(IDC_BODYLIST, OnBodyCtrl)
	ON_COMMAND(IDM_RECENTFILE1, OnRecentFile1)
	ON_COMMAND(IDM_RECENTFILE2, OnRecentFile2)
	ON_COMMAND(IDM_RECENTFILE3, OnRecentFile3)
	ON_COMMAND(IDM_RECENTFILE4, OnRecentFile4)
	ON_COMMAND(IDM_RECENTFILE5, OnRecentFile5)
	ON_COMMAND(IDM_RECENTFILE6, OnRecentFile6)
	ON_COMMAND(IDM_RECENTFILE7, OnRecentFile7)
	ON_COMMAND(IDM_RECENTFILE8, OnRecentFile8)
	END_MESSAGE_MAP()

static UINT XFLR5indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDPROJECT,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

static UINT indicators[] = // the extra indicator does not work on some PCs
{
	ID_SEPARATOR,           
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction


CMainFrame::CMainFrame()
{
	pi = 3.141592654;
	m_bSaved   = true;
	WINDOWPLACEMENT wndpl;
	
	m_wndView.m_pFrameWnd = this;
	wndpl.rcNormalPosition.left = 0; wndpl.rcNormalPosition.top = 0;
	wndpl.rcNormalPosition.right = 800;
	wndpl.rcNormalPosition.bottom = 768; 
	wndpl.showCmd = 1;

	m_VersionName = "XFLR5_v405_Beta";
	m_ProjectName = "";

	XDirect.m_pFrame    = this;
	XDirect.m_poaFoil   = &m_oaFoil;
	XDirect.m_poaPolar  = &m_oaPolar;
	XDirect.m_poaOpp    = &m_oaOpp;
	XDirect.m_pXFoil    = &m_XFoil;
	
	Miarex.m_pFrame     = this;
	Miarex.m_poaFoil    = &m_oaFoil;
	Miarex.m_poaPolar   = &m_oaPolar;
	Miarex.m_poaBody    = &m_oaBody;
	Miarex.m_poaWing    = &m_oaWing;
	Miarex.m_poaPlane   = &m_oaPlane;
	Miarex.m_poaWPolar  = &m_oaWPolar;
	Miarex.m_poaWOpp    = &m_oaWOpp;
	Miarex.m_poaPOpp    = &m_oaPOpp;
	Miarex.m_pW3DBar    = &m_W3DBar;
	Miarex.m_FlowLinesDlg.m_pMiarex   = &Miarex;
	Miarex.m_FlowLinesDlg.m_pChildWnd = &m_wndView;
	Miarex.m_FlowLinesDlg.m_pFrame    = this;
	Miarex.m_VLMDlg.m_pFrame    = this;
	Miarex.m_VLMDlg.m_pMiarex   = &Miarex;

	AFoil.m_pFrame = this;
	AFoil.m_pChildWnd = &m_wndView;
	AFoil.m_poaFoil = &m_oaFoil;
	AFoil.m_pXFoil  = &m_XFoil; 

	XInverse.m_pFrame =this;
	m_FInvCtrlBar.m_pXInverse = &XInverse;
	m_MInvCtrlBar.m_pXInverse = &XInverse;

	CPlaneDlg::s_poaWing = &m_oaWing;
	CPlaneDlg::s_poaBody  = &m_oaBody;
	CPlaneDlg::s_pMainFrame = this;
	CPlaneDlg::s_pMiarex = &Miarex;

	CPlane::s_pFrame = this;
	CPlane::s_pMiarex = &Miarex;
	CBody::s_pMainFrame = this;

	CWingDlg::s_pFrame   = this;
	CWingDlg::s_pMiarex  = &Miarex;
	CWing::s_pFrame      =  this;		//pointer to the Frame window
	CWing::s_pMiarex     = &Miarex;	//pointer to the Miarex Application window

	m_pCurFoil = NULL;

	m_W3DBar.m_pFrame = this;
	m_bSaveOpps       = false;
	m_bSaveWOpps      = true;

	m_LiftScale     = 0.7;
	m_DragScale     = 0.7;
	m_DownwashScale = 0.7;


// second line
	m_crColors[0] = RGB(255,0,0),
	m_crColors[1] = RGB(0,255,0),
	m_crColors[2] = RGB(0,0,255),
	m_crColors[3] = RGB(255,255,0),
	m_crColors[4] = RGB(255,0,255),
	m_crColors[5] = RGB(0,255,255),

// third line
	m_crColors[6]  = RGB(200,60,60),
	m_crColors[7]  = RGB(0,160,0),
	m_crColors[8]  = RGB(100,100,240),
	m_crColors[9]  = RGB(170,170,0),
	m_crColors[10] = RGB(130,0,130),
	m_crColors[11] = RGB(0,130,130),

// fourth line
	m_crColors[12] = RGB(255,128,128),
	m_crColors[13] = RGB(0,255,128),
	m_crColors[14] = RGB(64,200,255),
	m_crColors[15] = RGB(170,170,100),
	m_crColors[16] = RGB(190,100,190),
	m_crColors[17] = RGB(100,170,170),

// fifth line
	m_crColors[18] = RGB(228,150,70),
	m_crColors[19] = RGB(170,255,170),
	m_crColors[20] = RGB(120,120,255),
	m_crColors[21] = RGB(228,228,128),
	m_crColors[22] = RGB(255,170,255),
	m_crColors[23] = RGB(170,255,255),	

// first line
	m_crColors[24] = RGB(50,50,50),
	m_crColors[25] = RGB(90,90,90),
	m_crColors[26] = RGB(130,130,130),
	m_crColors[27] = RGB(170,170,170),
	m_crColors[28] = RGB(210,210,210),
	m_crColors[29] = RGB(255,255,255),

	m_iApp = 0;
	//Trace("CMainFrame::Object created");

	m_LeftMargin   = 15.0;//mm
	m_RightMargin  = 15.0;//mm
	m_TopMargin    = 15.0;//mm
	m_BottomMargin = 15.0;//mm

	m_LengthUnit  = 3;
	m_AreaUnit    = 3;
	m_WeightUnit  = 1;
	m_SpeedUnit   = 0;
	m_ForceUnit   = 0;
	m_MomentUnit  = 0;

	m_mtoUnit   = 1.0;
	m_mtoUnit   = 1.0;
	m_m2toUnit  = 1.0;
	m_kgtoUnit  = 1.0;
	m_mstoUnit  = 1.0;
	m_NtoUnit   = 1.0;
	m_NmtoUnit  = 1.0;

//	m_WingSurfaces = RGB(70,70,70);

	m_VLMStyle    = PS_SOLID;
	m_VLMWidth    = 1;
	m_VLMColor    = RGB(100,100,100);

	m_OutlineStyle   = PS_SOLID;
	m_OutlineWidth   = 1;
	m_OutlineColor   = RGB(120,120,120);
	m_XTopStyle      = PS_SOLID;
	m_XTopWidth      = 1;
	m_XTopColor      = RGB(200, 0, 255);
	m_XBotStyle      = PS_DASH;
	m_XBotWidth      = 1;
	m_XBotColor      = RGB(200, 0, 255);
	m_XCPStyle       = PS_SOLID;
	m_XCPWidth       = 1;
	m_XCPColor       = RGB(50, 150, 50);
	m_MomentStyle    = PS_SOLID;
	m_MomentWidth    = 1;
	m_MomentColor    = RGB(200, 100, 100);
	m_IDragStyle     = PS_SOLID;
	m_IDragWidth     = 1;
	m_IDragColor     = RGB(255,200,0);
	m_VDragStyle     = PS_SOLID;
	m_VDragWidth     = 1;
	m_VDragColor     = RGB(255,000,200);
	m_DownwashStyle  = PS_SOLID;
	m_DownwashWidth  = 1;
	m_DownwashColor  = RGB(255, 100, 100);
	m_WakeStyle      = PS_SOLID;
	m_WakeWidth      = 1;
	m_WakeColor      = RGB(0, 150, 200);
}

CMainFrame::~CMainFrame()
{
	m_XDirectMenu.DestroyMenu();
	m_MiarexMenu.DestroyMenu();
	m_AFoilMenu.DestroyMenu();
	m_XInverseMenu.DestroyMenu();

	CPlane* pPlane;
	int nIndex = (int)m_oaPlane.GetSize();
	for (int i=nIndex-1; i>=0;i--){
		pPlane = (CPlane*)m_oaPlane.GetAt(i);
		m_oaPlane.RemoveAt(i);//removes the pointer
		delete pPlane;// deletes the object
	}

	CPOpp *pPOpp;
	nIndex = (int)m_oaPOpp.GetSize();
	for (i=nIndex-1; i>=0;i--){
		pPOpp = (CPOpp*)m_oaPOpp.GetAt(i);
		m_oaPOpp.RemoveAt(i);//removes the pointer
		delete pPOpp;// deletes the object
	}

	CWing* pWing;
	nIndex = (int)m_oaWing.GetSize();
	for (i=nIndex-1; i>=0;i--){
		pWing = (CWing*)m_oaWing.GetAt(i);
		m_oaWing.RemoveAt(i);//removes the pointer
		delete pWing;// deletes the object
	}

	CWPolar* pWPolar;
	nIndex = (int)m_oaWPolar.GetSize();
	for (i=nIndex-1; i>=0;i--){
		pWPolar = (CWPolar*)m_oaWPolar.GetAt(i);
		m_oaWPolar.RemoveAt(i);//removes the pointer
		delete pWPolar;// deletes the object
	}

	CWOpp* pWOpp;
	nIndex = (int)m_oaWOpp.GetSize();
	for (i=nIndex-1; i>=0;i--){
		pWOpp = (CWOpp*)m_oaWOpp.GetAt(i);
		m_oaWOpp.RemoveAt(i);//removes the pointer
		delete pWOpp;// deletes the object
	}

	OpPoint* pOpPoint;
	nIndex = (int)m_oaOpp.GetSize();
	for (i=nIndex-1; i>=0;i--){
		pOpPoint = (OpPoint*)m_oaOpp.GetAt(i);
		m_oaOpp.RemoveAt(i);//removes the pointer
		delete pOpPoint;// deletes the object
	}

	CPolar *pPolar;
	nIndex=(int)m_oaPolar.GetSize();
	for(i = nIndex-1; i>=0; i--){
		pPolar = (CPolar*)m_oaPolar.GetAt(i);
		m_oaPolar.RemoveAt(i);//removes the pointer
		delete pPolar;// deletes the object
	}

}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	Trace("CMainFrame::OnCreate");

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		Trace("CMainFrame::OnCreate - Failed to create view window");
		return -1;
	}
		Trace("CMainFrame::OnCreate creating status bar");

	if (!m_wndStatusBar.Create(this)){
		Trace("CMainFrame::OnCreate - Failed to create status bar");
		return -1;      // fail to create
	}
	Trace("CMainFrame::OnCreate created status bar -1");

	if(!m_wndStatusBar.SetIndicators(XFLR5indicators, sizeof(XFLR5indicators)/sizeof(UINT)))
	{
//		TRACE("CMainFrame::OnCreate - Failed to set extended indicators\n");
		//try with the reduced indicator list
		if(!m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT))){
			Trace("CMainFrame::OnCreate - Failed to set std indicator list");
			return -1;
		}
		else 
			Trace("CMainFrame::OnCreate - no indicators are set");
	}
	Trace("CMainFrame::OnCreate Indicators are set");

	CreateAFoilBar();
	//Trace("CMainFrame::OnCreate - created AFoil Bar");
	CreateXInverseBar();
	//Trace("CMainFrame::OnCreate - created XInverse Bar");
	AFoil.m_pAFoilBar = &m_AFoilBar;
	CreateXDirectBar();
	//Trace("CMainFrame::OnCreate - created XDirect Bar");
	CreateMiarexBar();
	Trace("CMainFrame::OnCreate - created MIAREX Bar");

	m_OperDlgBar.m_pXDirect = &XDirect;
	m_OperDlgBar.m_pXFoil   = XDirect.m_pXFoil;
	if (!m_OperDlgBar.Create(this, IDD_OPERDLGBAR,	WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC , IDD_OPERDLGBAR))
	{ 
		//Trace("Failed to create Operbar\n");
		return -1;
		// fail to create
	}
	m_OperDlgBar.SetWindowText("Oper controls");
	//Trace("CMainFrame::OnCreate - created OPERDLGBAR Bar");

	if (!m_PolarDlgBar.Create(this, IDD_POLARBAR,	WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC , IDD_POLARBAR))
	{ 
		//Trace("Failed to create m_PolarDlgBar\n");
		return -1;
		// fail to create
	}
	m_PolarDlgBar.SetWindowText("Curve settings");
	m_PolarDlgBar.m_pParent = &XDirect;// just as a temporary necessity
	m_PolarDlgBar.m_idApp   = XFOILANALYSIS;
	m_PolarDlgBar.SetParams();
	m_PolarDlgBar.m_pFrame = this;

	//Trace("CMainFrame::OnCreate - created PolarDlgBar Bar");
	m_AFoilCtrlBar.m_pADlg = &AFoil;
	AFoil.m_pACtrl = &m_AFoilCtrlBar;

	if (!m_AFoilCtrlBar.Create(this, IDD_AFOILCTRLBAR,	WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC , IDD_AFOILCTRLBAR))
	{
		//Trace("Failed to create AFoilCtrlbar\n");
		return -1;
		// fail to create
	}
	m_AFoilCtrlBar.SetWindowText("Foil design controls");
	Trace("CMainFrame::OnCreate - created m_AFoilCtrlBar Bar");

	if (!m_WOperDlgBar.Create(this, IDD_WOPERDLGBAR,	WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC , IDD_WOPERDLGBAR))
	{
		//Trace("Failed to create WOperbar\n");
		return -1;
		// fail to create
	}

	m_WOperDlgBar.SetWindowText("Wing analysis controls");
	m_WOperDlgBar.m_pFrame = this;
	m_WOperDlgBar.m_pMiarex = &Miarex;
	//Trace("CMainFrame::OnCreate - created m_WOperDlgBar Bar");

	m_W3DBar.m_pFrame = this;
	m_W3DBar.m_pMiarex = &Miarex;
	if (!m_W3DBar.Create(this, IDD_W3DBAR,	WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC , IDD_W3DBAR))
	{
		//Trace("Failed to create W3DBar\n");
		return -1;
		// fail to create
	}
	m_W3DBar.SetWindowText("3D controls");

	//Trace("CMainFrame::OnCreate - created W3DBar Bar");

	m_ScaleOppBar.m_pFrame = this;
	m_ScaleOppBar.m_pChildView = &m_wndView;
	if (!m_ScaleOppBar.Create(this, IDD_SCALEOPPBAR,	WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC , IDD_SCALEOPPBAR))
	{
		//Trace("Failed to create W3DBar\n");
		return -1;
		// fail to create
	}
	m_ScaleOppBar.SetWindowText("Scale controls");

	m_SpanPosBar.m_pFrame = this;
	m_SpanPosBar.m_pMiarex = &Miarex;
	m_SpanPosBar.m_pChildView = &m_wndView;
	if (!m_SpanPosBar.Create(this, IDD_SPANPOSBAR,	WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC , IDD_SPANPOSBAR))
	{
		//Trace("Failed to create W3DBar\n");
		return -1;
		// fail to create
	}
	m_SpanPosBar.SetWindowText("Span Position");

	Trace("CMainFrame::OnCreate - created Span Position Bar");

	m_BodyCtrlBar.m_pMainFrame = this;
	m_BodyCtrlBar.m_pMiarex    = &Miarex;
	m_BodyCtrlBar.m_pChildView = &m_wndView;
	m_BodyCtrlBar.m_poaBody    = &m_oaBody;
	m_BodyCtrlBar.m_ppCurBody  = &Miarex.m_pCurBody;
	Miarex.m_pBodyCtrlBar = &m_BodyCtrlBar;
	if (!m_BodyCtrlBar.Create(this, IDD_BODYCTRLBAR,	WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC , IDD_BODYCTRLBAR))
	{
		//Trace("Failed to create W3DBar\n");
		return -1;
		// fail to create
	}
	m_BodyCtrlBar.SetWindowText("Body control Bar");


	if (!m_MInvCtrlBar.Create(this, IDD_MINVCTRLBAR,	WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC , IDD_MINVCTRLBAR))
	{
		//Trace("Failed to create MInvCtrlBar\n");
		return -1;
		// fail to create
	}

	//Trace("CMainFrame::OnCreate - created MInvCtrlBar Bar");

	m_MInvCtrlBar.SetWindowText("Mixed inverse controls");

	if (!m_FInvCtrlBar.Create(this, IDD_FINVCTRLBAR,	WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC , IDD_FINVCTRLBAR))
	{
		//Trace("Failed to create FInvCtrlbar\n");
		return -1;
		// fail to create
	}
	m_FInvCtrlBar.SetWindowText("Full inverse controls");
	Trace("CMainFrame::OnCreate - created MInvCtrlBar Bar");

	m_MiarexBar.EnableDocking(CBRS_ALIGN_ANY);
	m_XDirectBar.EnableDocking(CBRS_ALIGN_ANY);
	m_OperDlgBar.EnableDocking(CBRS_ALIGN_ANY);
	m_BodyCtrlBar.EnableDocking(CBRS_ALIGN_ANY);
	m_WOperDlgBar.EnableDocking(CBRS_ALIGN_ANY);
	m_W3DBar.EnableDocking(CBRS_ALIGN_ANY);
	m_ScaleOppBar.EnableDocking(CBRS_ALIGN_ANY);
	m_SpanPosBar.EnableDocking(CBRS_ALIGN_ANY);
	m_PolarDlgBar.EnableDocking(CBRS_ALIGN_ANY);
	m_AFoilBar.EnableDocking(CBRS_ALIGN_ANY);
	m_AFoilCtrlBar.EnableDocking(CBRS_ALIGN_ANY);
	m_XInverseBar.EnableDocking(CBRS_ALIGN_ANY);
	m_MInvCtrlBar.EnableDocking(CBRS_ALIGN_ANY);
	m_FInvCtrlBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);

	//Trace("CMainFrame::OnCreate - Docked Bars");

	HideBars();
	//Trace("CMainFrame::OnCreate - Hided Bars");

	m_wndView.m_pXDirect    = &XDirect;
	XDirect.m_pChildWnd     = &m_wndView;
//	XDirect.m_VersionName   = m_VersionName;

	m_wndView.m_pMiarex   = &Miarex;
	Miarex.m_pChildWnd    = &m_wndView;
//	Miarex.m_VersionName  = m_VersionName;

	m_wndView.m_pAFoil    = &AFoil;
	AFoil.m_pChildWnd     = &m_wndView;

	m_wndView.m_pXInverse = &XInverse;
	XInverse.m_pChildWnd  = &m_wndView;

	Trace("CMainFrame::OnCreate - Initializing XInverse");

	XInverse.m_pXFoil     = XDirect.m_pXFoil;
//	XInverse.Initialize();
	XInverse.m_pFInvCtrlBar = &m_FInvCtrlBar;
	XInverse.m_pMInvCtrlBar = &m_MInvCtrlBar;
	XInverse.m_pXInverseBar = &m_XInverseBar;

	m_XDirectMenu.LoadMenu(IDM_XDIRECT_MENU);
	m_MiarexMenu.LoadMenu(IDM_MIAREX_MENU);
	m_AFoilMenu.LoadMenu(IDM_AFOIL_MENU);
	m_XInverseMenu.LoadMenu(IDM_XINVERSE_MENU);


	//Trace("CMainFrame::OnCreate - Loading settings");
	LoadSettings();
	SetRecentFileMenu();
//	m_wndView.GLLoadFont();
	m_W3DBar.SetChecks();
	m_ScaleOppBar.SetPos();
	Trace("CMainFrame::OnCreate - pChildView=", &m_wndView);
	SetFocus();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	//Trace("CMainFrame::PreCreateWindow - Initializing");
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	//  the CREATESTRUCT cs
	cs.lpszName = "XFLR5 ";
//	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
//	cs.lpszClass = AfxRegisterWndClass(0);
	
	//Trace("CMainFrame::PreCreateWindow - Done");
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics
 
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))	return TRUE;

	// then let the current app do the job
	switch(m_iApp){
		case XFOILANALYSIS:
		{
			if (XDirect.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
				return TRUE;
			break;
		}
		case DIRECTDESIGN:{
			if (AFoil.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
				return TRUE;
			break;
		}
		case INVERSEDESIGN:{
			if (XInverse.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
				return TRUE;
			break;
		}
		case MIAREX:{
			if (Miarex.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
				return TRUE;
			break;
		}
	}
	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainFrame::OnXDirect() 
{
	if(m_iApp == XFOILANALYSIS) {
//		UpdateFoils();
		XDirect.UpdateView();
		return;
	}

	Miarex.m_bAnimate = false;

	HideBars();

	m_iApp = XFOILANALYSIS;
	m_PolarDlgBar.m_pParent = &XDirect;
	m_PolarDlgBar.m_idApp = m_iApp;
	m_PolarDlgBar.SetParams();

	m_XDirectBar.ShowWindow(SW_SHOW);
	DockControlBar(&m_XDirectBar);
	m_OperDlgBar.ShowWindow(SW_SHOW);
	DockControlBar(&m_OperDlgBar,AFX_IDW_DOCKBAR_RIGHT);

	CRect rectBar;
	CRect rectBar1;	
	RecalcLayout();	
	m_OperDlgBar.GetWindowRect(rectBar);
	m_PolarDlgBar.GetWindowRect(rectBar1);
	rectBar1.left   = rectBar.left;
	rectBar1.top    = rectBar.bottom+1;
	rectBar1.bottom = rectBar1.top+190;
	rectBar1.right  = rectBar.right ;
	DockControlBar(&m_PolarDlgBar, AFX_IDW_DOCKBAR_RIGHT, rectBar1); 
	m_PolarDlgBar.ShowWindow(SW_SHOW);

	m_wndView.SetScale();
	SetMenu(&m_XDirectMenu);
	SetRecentFileMenu();

	XDirect.SetFoil(m_pCurFoil);
	XDirect.SetParams();
	UpdateFoils();
	XDirect.UpdateView();
	m_wndView.SetFocus();
}


void CMainFrame::OnMiarex() 
{
	if(m_iApp == MIAREX) 
	{
		Miarex.UpdateView();
		return;
	}

	CPoint pt(500,100);

	XDirect.m_bAnimate  = false;

	HideBars();

	m_iApp = MIAREX;
	m_PolarDlgBar.m_pFrame  = this;
	m_PolarDlgBar.m_pParent = &Miarex;
	m_PolarDlgBar.m_idApp = m_iApp;
	m_PolarDlgBar.SetParams();

	m_MiarexBar.ShowWindow(SW_SHOW);
	DockControlBar(&m_MiarexBar);

	m_WOperDlgBar.ShowWindow(SW_SHOW);
	DockControlBar(&m_WOperDlgBar,AFX_IDW_DOCKBAR_RIGHT);

	if(Miarex.m_iView ==5)
	{
		m_BodyCtrlBar.ShowWindow(SW_SHOW);
		DockControlBar(&m_BodyCtrlBar,AFX_IDW_DOCKBAR_BOTTOM);
	}

	DockMiarexBars();

	SetMenu(&m_MiarexMenu);
	SetRecentFileMenu();

	Miarex.SetParams();
	UpdateUFOs();
	Miarex.SetUFO();

	m_wndView.SetScale();

	if(Miarex.m_pCurWing && Miarex.m_iView==1) Miarex.OnAdjustToWing();
	else if (Miarex.m_iView==2)                Miarex.CreateWPolarCurves();
	else if (Miarex.m_iView==5)
	{
		m_BodyCtrlBar.UpdateBodies();
		Miarex.SetBody();
		Miarex.m_bIs3DScaleSet = false;
		Miarex.SetBodyScale();
	}

	Miarex.UpdateView();
	m_wndView.SetFocus();
}


void CMainFrame::OnFoilDirectDesign()
{
//	if (m_iApp==DIRECTDESIGN ) return;

	XDirect.m_bAnimate  = false;
	Miarex.m_bAnimate = false;

	HideBars();

	m_iApp = DIRECTDESIGN;

	m_AFoilBar.ShowWindow(SW_SHOW);
	DockControlBar(&m_AFoilBar, AFX_IDW_DOCKBAR_TOP);

	CRect ViewRect;
	m_wndView.GetWindowRect(ViewRect);
	CRect rectBar;
	m_AFoilCtrlBar.GetWindowRect(rectBar);
	rectBar.right = ViewRect.right-10;

	m_AFoilCtrlBar.ShowWindow(SW_SHOW);
	DockControlBar(&m_AFoilCtrlBar, AFX_IDW_DOCKBAR_BOTTOM, rectBar);

	SetMenu(&m_AFoilMenu);	
	SetRecentFileMenu();

	AFoil.SetParams(m_pCurFoil);
	m_AFoilCtrlBar.SetParams(m_pCurFoil);

	m_wndView.SetFocus();
	AFoil.UpdateView();
}

void CMainFrame::DockMiarexBars() 
{
	CPoint pt(500,5);
	CRect CltRect, rectBar, rectBar1, rectBar2;;
	GetClientRect(&CltRect);
	
	RecalcLayout();

	if(m_iApp==MIAREX)
	{
		m_WOperDlgBar.GetWindowRect(rectBar);
		
		if(Miarex.m_iView ==1 || Miarex.m_iView ==2) m_PolarDlgBar.GetWindowRect(rectBar1);
		else if(Miarex.m_iView ==3)
		{
			m_W3DBar.GetWindowRect(rectBar1);
			m_ScaleOppBar.GetWindowRect(rectBar2);
		}
		else if(Miarex.m_iView ==4)
		{
			m_PolarDlgBar.GetWindowRect(rectBar1);
			m_SpanPosBar.GetWindowRect(rectBar2);
		}
		int h = rectBar1.Height();
		rectBar1.left   = rectBar.left;
		rectBar1.top    = rectBar.bottom+1;
		rectBar1.right  = rectBar.right ;
		rectBar2.left   = rectBar.left;
		rectBar2.top    = rectBar1.top+h+1;
		rectBar2.right  = rectBar.right ;

		if(Miarex.m_iView==1 || Miarex.m_iView==2)
		{
			DockControlBar(&m_WOperDlgBar, AFX_IDW_DOCKBAR_RIGHT, rectBar);
			m_WOperDlgBar.ShowWindow(SW_SHOW);
			DockControlBar(&m_PolarDlgBar, AFX_IDW_DOCKBAR_RIGHT, rectBar1);
			m_PolarDlgBar.ShowWindow(SW_SHOW);

			m_W3DBar.ShowWindow(SW_HIDE);
			m_SpanPosBar.ShowWindow(SW_HIDE);
			m_ScaleOppBar.ShowWindow(SW_HIDE);
			m_BodyCtrlBar.ShowWindow(SW_HIDE);
			FloatControlBar(&m_W3DBar, pt);
			FloatControlBar(&m_SpanPosBar, pt);
			FloatControlBar(&m_ScaleOppBar, pt);
			FloatControlBar(&m_BodyCtrlBar, pt);
		}
		else if(Miarex.m_iView==3)
		{
			DockControlBar(&m_WOperDlgBar, AFX_IDW_DOCKBAR_RIGHT, rectBar);
			m_WOperDlgBar.ShowWindow(SW_SHOW);
			DockControlBar(&m_W3DBar, AFX_IDW_DOCKBAR_RIGHT, rectBar1);
			m_W3DBar.ShowWindow(SW_SHOW);
			DockControlBar(&m_ScaleOppBar, AFX_IDW_DOCKBAR_RIGHT, rectBar2);
			m_ScaleOppBar.ShowWindow(SW_SHOW);

			m_PolarDlgBar.ShowWindow(SW_HIDE);
			m_SpanPosBar.ShowWindow(SW_HIDE);
			m_BodyCtrlBar.ShowWindow(SW_HIDE);
			FloatControlBar(&m_PolarDlgBar, pt);
			FloatControlBar(&m_SpanPosBar, pt);
			FloatControlBar(&m_BodyCtrlBar, pt);
		}
		else if(Miarex.m_iView==4)
		{
			DockControlBar(&m_WOperDlgBar, AFX_IDW_DOCKBAR_RIGHT, rectBar);
			m_WOperDlgBar.ShowWindow(SW_SHOW);
			DockControlBar(&m_PolarDlgBar, AFX_IDW_DOCKBAR_RIGHT, rectBar1);
			m_PolarDlgBar.ShowWindow(SW_SHOW);
			DockControlBar(&m_SpanPosBar, AFX_IDW_DOCKBAR_RIGHT, rectBar2);
			m_SpanPosBar.ShowWindow(SW_SHOW);

			m_W3DBar.ShowWindow(SW_HIDE);
			m_ScaleOppBar.ShowWindow(SW_HIDE);
			m_BodyCtrlBar.ShowWindow(SW_HIDE);
			FloatControlBar(&m_W3DBar, pt);
			FloatControlBar(&m_ScaleOppBar, pt);
			FloatControlBar(&m_BodyCtrlBar, pt);
		}
		else if(Miarex.m_iView==5)
		{
			m_WOperDlgBar.ShowWindow(SW_HIDE);
			m_ScaleOppBar.ShowWindow(SW_HIDE);
			m_SpanPosBar.ShowWindow(SW_HIDE);
			m_PolarDlgBar.ShowWindow(SW_HIDE);
			m_W3DBar.ShowWindow(SW_HIDE);

			FloatControlBar(&m_W3DBar, pt);
			FloatControlBar(&m_SpanPosBar, pt);
			FloatControlBar(&m_ScaleOppBar, pt);
			FloatControlBar(&m_PolarDlgBar, pt);

			
			DockControlBar(&m_BodyCtrlBar, AFX_IDW_DOCKBAR_BOTTOM);
			m_BodyCtrlBar.ShowWindow(SW_SHOW);
			RecalcLayout();
			m_BodyCtrlBar.GetWindowRect(rectBar1);
			m_W3DBar.GetWindowRect(rectBar2);

			int width  = abs(rectBar2.right - rectBar2.left);
			int height = abs(rectBar2.bottom - rectBar2.top);
			rectBar2.left   = rectBar1.right;
			rectBar2.top    = rectBar1.top;
			rectBar2.bottom = rectBar1.top+height;
			rectBar2.right  = rectBar2.left+width;

			DockControlBar(&m_W3DBar, AFX_IDW_DOCKBAR_BOTTOM, rectBar2);
			m_W3DBar.ShowWindow(SW_SHOW);
		}
	}
	RecalcLayout();

	m_wndView.SetFocus();
}


void CMainFrame::HideBars()
{
	CPoint pt(500,100);
	m_MiarexBar.ShowWindow(SW_HIDE);
	FloatControlBar(&m_MiarexBar, pt);

	m_XDirectBar.ShowWindow(SW_HIDE);
	FloatControlBar(&m_XDirectBar, pt);

	m_PolarDlgBar.ShowWindow(SW_HIDE);
	FloatControlBar(&m_PolarDlgBar, pt);

	m_OperDlgBar.ShowWindow(SW_HIDE);
	FloatControlBar(&m_OperDlgBar, pt);

	m_WOperDlgBar.ShowWindow(SW_HIDE);
	FloatControlBar(&m_WOperDlgBar, pt);

	m_BodyCtrlBar.ShowWindow(SW_HIDE);
	FloatControlBar(&m_BodyCtrlBar, pt);

	m_W3DBar.ShowWindow(SW_HIDE);
	FloatControlBar(&m_W3DBar, pt);

	m_ScaleOppBar.ShowWindow(SW_HIDE);
	FloatControlBar(&m_ScaleOppBar, pt);

	m_SpanPosBar.ShowWindow(SW_HIDE);
	FloatControlBar(&m_SpanPosBar, pt);

	m_AFoilBar.ShowWindow(SW_HIDE);
	FloatControlBar(&m_AFoilBar, pt);

	m_AFoilCtrlBar.ShowWindow(SW_HIDE);
	FloatControlBar(&m_AFoilCtrlBar, pt);

	m_XInverseBar.ShowWindow(SW_HIDE);
	FloatControlBar(&m_XInverseBar, pt);

	m_FInvCtrlBar.ShowWindow(SW_HIDE);
	FloatControlBar(&m_FInvCtrlBar, pt);

	m_MInvCtrlBar.ShowWindow(SW_HIDE);
	FloatControlBar(&m_MInvCtrlBar, pt);

	Miarex.m_FlowLinesDlg.ShowWindow(SW_HIDE);
}


void CMainFrame::OnToolbars() 
{
	CPoint pt(500,100);
	CToolBarDlg TDlg;
	
	if (m_MiarexBar.IsWindowVisible())		TDlg.m_BMiarexWingBar   = TRUE;
	if (m_WOperDlgBar.IsWindowVisible())	TDlg.m_BMiarexWAnaBar   = TRUE;
	if (m_W3DBar.IsWindowVisible())			TDlg.m_BMiarexW3DBar    = TRUE;
	if (m_ScaleOppBar.IsWindowVisible())	TDlg.m_BScaleOppBar     = TRUE;
	if (m_XDirectBar.IsWindowVisible())		TDlg.m_BXDirectFoilBar  = TRUE;
	if (m_OperDlgBar.IsWindowVisible())		TDlg.m_BXDirectAnaBar   = TRUE;
	if (m_PolarDlgBar.IsWindowVisible())	TDlg.m_BPolarBar        = TRUE;
	if (m_AFoilBar.IsWindowVisible())		TDlg.m_BAFoilBar        = TRUE;
	if (m_AFoilCtrlBar.IsWindowVisible())	TDlg.m_BACtrlBar        = TRUE;
	if (m_XInverseBar.IsWindowVisible())	TDlg.m_BXInverseBar     = TRUE;
	if (m_FInvCtrlBar.IsWindowVisible())	TDlg.m_BFInvCtrlBar     = TRUE;
	if (m_MInvCtrlBar.IsWindowVisible())	TDlg.m_BMInvCtrlBar     = TRUE;

	TDlg.m_iApp = m_iApp;
	TDlg.DoModal();

	if (TDlg.m_BXInverseBar == TRUE && !m_XInverseBar.IsWindowVisible()) {
		DockControlBar(&m_XInverseBar);
		m_XInverseBar.ShowWindow(SW_SHOW);
	}
	else if(TDlg.m_BXInverseBar == TRUE && m_XInverseBar.IsWindowVisible()) {
		// leave it
	}
	else {
		m_XInverseBar.ShowWindow(SW_HIDE);
		FloatControlBar(&m_XInverseBar, pt);
	}

	if (TDlg.m_BFInvCtrlBar == TRUE && !m_FInvCtrlBar.IsWindowVisible()) {
		DockControlBar(&m_FInvCtrlBar, AFX_IDW_DOCKBAR_RIGHT);
		m_FInvCtrlBar.ShowWindow(SW_SHOW);
	}
	else if(TDlg.m_BFInvCtrlBar == TRUE && m_FInvCtrlBar.IsWindowVisible()) {
		// leave it
	}
	else {
		m_FInvCtrlBar.ShowWindow(SW_HIDE);
		FloatControlBar(&m_FInvCtrlBar, pt);
	}

	if (TDlg.m_BMInvCtrlBar == TRUE && !m_MInvCtrlBar.IsWindowVisible()) {
		DockControlBar(&m_MInvCtrlBar, AFX_IDW_DOCKBAR_RIGHT);
		m_MInvCtrlBar.ShowWindow(SW_SHOW);
	}
	else if(TDlg.m_BMInvCtrlBar == TRUE && m_MInvCtrlBar.IsWindowVisible()) {
		// leave it
	}
	else {
		m_MInvCtrlBar.ShowWindow(SW_HIDE);
		FloatControlBar(&m_MInvCtrlBar, pt);
	}

	if (TDlg.m_BAFoilBar == TRUE && !m_AFoilBar.IsWindowVisible()) {
		DockControlBar(&m_AFoilBar, AFX_IDW_DOCKBAR_BOTTOM);
		m_AFoilBar.ShowWindow(SW_SHOW);
	}
	else if(TDlg.m_BAFoilBar == TRUE && m_AFoilBar.IsWindowVisible()) {
		// leave it
	}
	else {
		m_AFoilBar.ShowWindow(SW_HIDE);
		FloatControlBar(&m_AFoilBar, pt);
	}

	if (TDlg.m_BACtrlBar == TRUE && !m_AFoilCtrlBar.IsWindowVisible()) {
		DockControlBar(&m_AFoilCtrlBar, AFX_IDW_DOCKBAR_BOTTOM);
		m_AFoilCtrlBar.ShowWindow(SW_SHOW);
	}
	else if(TDlg.m_BACtrlBar == TRUE && m_AFoilCtrlBar.IsWindowVisible()) {
		// leave it
	}
	else {
		m_AFoilCtrlBar.ShowWindow(SW_HIDE);
		FloatControlBar(&m_AFoilCtrlBar, pt);
	}


	if (TDlg.m_BPolarBar == TRUE && !m_PolarDlgBar.IsWindowVisible()) {
		DockControlBar(&m_PolarDlgBar, AFX_IDW_DOCKBAR_RIGHT);
		m_PolarDlgBar.ShowWindow(SW_SHOW);
	}
	else if(TDlg.m_BPolarBar == TRUE && m_PolarDlgBar.IsWindowVisible()) {
		// leave it
	}
	else {
		m_PolarDlgBar.ShowWindow(SW_HIDE);
		FloatControlBar(&m_PolarDlgBar, pt);
	}

	if (TDlg.m_BXDirectAnaBar == TRUE && !m_OperDlgBar.IsWindowVisible()) {
		DockControlBar(&m_OperDlgBar, AFX_IDW_DOCKBAR_RIGHT);
		m_OperDlgBar.ShowWindow(SW_SHOW);
	}
	else if(TDlg.m_BXDirectAnaBar == TRUE && m_OperDlgBar.IsWindowVisible()) {
		// leave it
	}
	else {
		m_OperDlgBar.ShowWindow(SW_HIDE);
		FloatControlBar(&m_OperDlgBar, pt);
	}


	if (TDlg.m_BXDirectFoilBar == TRUE && !m_XDirectBar.IsWindowVisible()) {
		DockControlBar(&m_XDirectBar);
		m_XDirectBar.ShowWindow(SW_SHOW);
	}
	else if(TDlg.m_BXDirectFoilBar == TRUE && m_XDirectBar.IsWindowVisible()) {
		// leave it
	}
	else {
		m_XDirectBar.ShowWindow(SW_HIDE);
		FloatControlBar(&m_XDirectBar, pt);
	}


	if (TDlg.m_BMiarexWingBar == TRUE && !m_MiarexBar.IsWindowVisible()) {
		DockControlBar(&m_MiarexBar, AFX_IDW_DOCKBAR_TOP);
		m_MiarexBar.ShowWindow(SW_SHOW);
	}
	else if(TDlg.m_BMiarexWingBar == TRUE && m_MiarexBar.IsWindowVisible()) {
		// leave it
	}
	else {
		m_MiarexBar.ShowWindow(SW_HIDE);
		FloatControlBar(&m_MiarexBar, pt);
	}

	if (TDlg.m_BMiarexWAnaBar == TRUE && !m_WOperDlgBar.IsWindowVisible()) {
		DockControlBar(&m_WOperDlgBar, AFX_IDW_DOCKBAR_RIGHT);
		m_WOperDlgBar.ShowWindow(SW_SHOW);
	}
	else if(TDlg.m_BMiarexWAnaBar == TRUE && m_WOperDlgBar.IsWindowVisible()) {
		// leave it
	}
	else {
		m_WOperDlgBar.ShowWindow(SW_HIDE);
		FloatControlBar(&m_WOperDlgBar, pt);
	}

	if (TDlg.m_BMiarexW3DBar == TRUE && !m_W3DBar.IsWindowVisible()) {
		DockControlBar(&m_W3DBar, AFX_IDW_DOCKBAR_RIGHT);
		m_W3DBar.ShowWindow(SW_SHOW);
	}
	else if(TDlg.m_BMiarexW3DBar == TRUE && m_W3DBar.IsWindowVisible()) {
		// leave it
	}
	else {
		m_W3DBar.ShowWindow(SW_HIDE);
		FloatControlBar(&m_W3DBar, pt);
	}
	
	if (TDlg.m_BScaleOppBar == TRUE && !m_ScaleOppBar.IsWindowVisible()) {
		DockControlBar(&m_ScaleOppBar, AFX_IDW_DOCKBAR_RIGHT);
		m_ScaleOppBar.ShowWindow(SW_SHOW);
	}
	else if(TDlg.m_BScaleOppBar == TRUE && m_ScaleOppBar.IsWindowVisible()) {
		// leave it
	}
	else {
		m_ScaleOppBar.ShowWindow(SW_HIDE);
		FloatControlBar(&m_ScaleOppBar, pt);
	}
}


void CMainFrame::LoadOldPolarFile(CArchive &ar, int n)
{
	//XFLR5 version < 2.00
	
	CPolar *pPolar = NULL;
	CPolar *pOldPlr;
	CPolar *pOld2Plr;
	CString strong;
	
	try{
		for (int i=0;i<n; i++){
			pPolar = new CPolar(this);
			
			pPolar->m_Color = GetColor(1);
			pPolar->m_bIsVisible = true;
			pPolar->m_bShowPoints = false;
			pPolar->SerializeOldFormat(ar);
			if (!pPolar->m_FoilName.GetLength()){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			for (int l=0; l<m_oaPolar.GetSize(); l++){
				pOldPlr = (CPolar*)m_oaPolar[l];
				if (pOldPlr->m_FoilName == pPolar->m_FoilName &&
					pOldPlr->m_PlrName  == pPolar->m_PlrName){
					int p = 2;
					bool bFound = true;
					while(bFound){
						strong.Format(" (%d)", p);
						strong = pPolar->m_PlrName + strong;
						for (int k=0; k<m_oaPolar.GetSize(); k++){
							bFound = false;
							pOld2Plr = (CPolar*)m_oaPolar[k];
							if (pOld2Plr->m_FoilName == pPolar->m_FoilName &&
								pOld2Plr->m_PlrName  == strong){
								p++;
								bFound = true;
								break;
							}
						}
					}
					pPolar->m_PlrName = strong;
				}
			}
			pPolar = AddPolar(pPolar);
		}
	}
	catch (CArchiveException *ex){
		TCHAR   szCause[255];
		CString str;
		ex->GetErrorMessage(szCause, 255);
		str = _T("Error loading polar : ");
		str += szCause;
		AfxMessageBox(str);
		ex->Delete();
		if(pPolar) delete pPolar;
	}
	
}

void CMainFrame::WritePolars(CArchive &ar, CFoil *pFoil)
{
	try{
		if(!pFoil){
			ar << 100002;
			//100001 : transferred NCrit, XTopTr, XBotTr to polar file
			//100002 : means we are serializings opps in the new numbered format
			//first write foils
			ar << (int)m_oaFoil.GetSize();

			for (int i=0; i<m_oaFoil.GetSize(); i++){
				pFoil = (CFoil*)m_oaFoil.GetAt(i);
				pFoil->Serialize(ar);
			}

			//then write polars
			ar << (int)m_oaPolar.GetSize();
			CPolar * pPolar ;
			for (i=0; i<m_oaPolar.GetSize();i++){
				pPolar = (CPolar*)m_oaPolar.GetAt(i);
				pPolar->m_pFrame = this;
				pPolar->Serialize(ar);
			}
			if(m_bSaveOpps){
			ar << (int)m_oaOpp.GetSize();
				OpPoint * pOpp ;
				for (i=0; i<m_oaOpp.GetSize();i++){
					pOpp = (OpPoint*)m_oaOpp.GetAt(i);
//					pOpp->m_pParent = this;
					pOpp->SerializeOpp(ar,100002);
				}
			}
			else ar << 0;
		} 
		else{
			ar << 100002;
			//100001 : transferred NCrit, XTopTr, XBotTr to polar file
			//100002 : means we are serializings opps in the new numbered format
			//first write foil
			ar << 1;//only one this time
			pFoil->Serialize(ar);
			//count polars associated to the foil
			CPolar * pPolar ;
			int n=0;
			for (int i=0; i<m_oaPolar.GetSize();i++){
				pPolar = (CPolar*)m_oaPolar.GetAt(i);
				if (pPolar->m_FoilName == pFoil->m_FoilName) n++;
			}
			//then write polars
			ar << n;
			for (i=0; i<m_oaPolar.GetSize();i++){
				pPolar = (CPolar*)m_oaPolar.GetAt(i);
				pPolar->m_pFrame = this;
				if (pPolar->m_FoilName == pFoil->m_FoilName) pPolar->Serialize(ar);
			}
		}
	}
	catch (CException *ex){
		TCHAR   szCause[255];
		CString str;
		ex->GetErrorMessage(szCause, 255);
		str = _T("Error saving polar : ");
		str += szCause;
		AfxMessageBox(str);
		ex->Delete();
	}
}



bool CMainFrame::LoadPolarFile(CArchive &ar)
{
	CFoil *pFoil;
	CPolar *pPolar = NULL;
	CPolar *pOldPlr;
	CString strong;

	try{
		//first read all available foils
		int n;
		ar>>n;
		for (int i=0;i<n; i++){
			pFoil = new CFoil();
			
			pFoil->Serialize(ar);
			if (!pFoil->m_FoilName.GetLength()){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			AddFoil(pFoil);
		}
		
		//next read all available polars
		
		ar>>n;
		for (i=0;i<n; i++){
			pPolar = new CPolar(this);
			
			pPolar->m_Color = GetColor(1);
			pPolar->Serialize(ar);
			if (!pPolar->m_FoilName.GetLength()){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			for (int l=0; l<m_oaPolar.GetSize(); l++){
				pOldPlr = (CPolar*)m_oaPolar[l];
				if (pOldPlr->m_FoilName == pPolar->m_FoilName &&
					pOldPlr->m_PlrName  == pPolar->m_PlrName){
					//just overwrite...
					m_oaPolar.RemoveAt(l);
					delete pOldPlr;
					//... and continue to add
				}				
			}
			pPolar = AddPolar(pPolar);
		}
	}
	catch (CArchiveException *ex){
		TCHAR   szCause[255];
		CString str;
		ex->GetErrorMessage(szCause, 255);
		str = _T("Error loading polar : ");
		str += szCause;
		AfxMessageBox(str);
		ex->Delete();
		if(pPolar) delete pPolar;
		return false;
	}
	return true;
}

bool CMainFrame::LoadPolarFileV3(CArchive &ar, int ArchiveFormat)
{
	CFoil *pFoil;
	CPolar *pPolar = NULL;
	CPolar *pOldPlr;
	OpPoint *pOpp, *pOldOpp;
	CString strong;

		
	//first read all available foils
	int n;
	ar>>n;
	for (int i=0;i<n; i++){
		pFoil = new CFoil();
		
		pFoil->Serialize(ar);
		if (!pFoil->m_FoilName.GetLength()){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			delete pFoil;
			throw pfe;
		}
		if(pFoil->m_FoilColor==0){
			pFoil->m_FoilColor=GetColor(0);
		}
		AddFoil(pFoil);
	}
	
	//next read all available polars
	
	ar>>n;
	for (i=0;i<n; i++){
		pPolar = new CPolar(this);
		
		pPolar->m_Color = GetColor(1);
		pPolar->Serialize(ar);
		if (!pPolar->m_FoilName.GetLength()){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			delete pPolar;
			return false;
		}
		for (int l=0; l<m_oaPolar.GetSize(); l++){
			pOldPlr = (CPolar*)m_oaPolar[l];
			if (pOldPlr->m_FoilName == pPolar->m_FoilName &&
				pOldPlr->m_PlrName  == pPolar->m_PlrName){
				//just overwrite...
				m_oaPolar.RemoveAt(l);
				delete pOldPlr;
				//... and continue to add
			}
		}
		pPolar = AddPolar(pPolar);
	}
	//Last read all available operating points
	
	ar>>n;
	for (i=0;i<n; i++){
		pOpp = new OpPoint();
		if(!pOpp){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			pfe->m_strFileName = ar.m_strFileName;
			delete pOpp;
			return false;
		}
		pOpp->m_pXDirect = &XDirect;
		pOpp->m_Color = m_crColors[m_oaOpp.GetSize()%24];
		if(ArchiveFormat>=100002){
			if (!pOpp->SerializeOpp(ar,100002)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				delete pOpp;
				return false;
			}
			else{
				pFoil = GetFoil(pOpp->m_strFoilName);
				if(pFoil){
					memcpy(pOpp->x, pFoil->x, sizeof(pOpp->x));
					memcpy(pOpp->y, pFoil->y, sizeof(pOpp->y));
				}
				else {
					delete pOpp;
				}
			}
		}
		else{
			if (!pOpp->SerializeOpp(ar)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				delete pOpp;
				return false;
			}
			else{
				pFoil = GetFoil(pOpp->m_strFoilName);
				if(pFoil){
					memcpy(pOpp->x, pFoil->x, sizeof(pOpp->x));
					memcpy(pOpp->y, pFoil->y, sizeof(pOpp->y));
				}
				else {
					delete pOpp;
				}
			}
		}
		if(pOpp){
			for (int l=0; l<m_oaOpp.GetSize(); l++){
				pOldOpp = (OpPoint*)m_oaOpp.GetAt(l);
				if (pOldOpp->m_strFoilName == pOpp->m_strFoilName &&
					pOldOpp->m_strPlrName  == pOpp->m_strPlrName &&
					fabs(pOldOpp->Alpha-pOpp->Alpha)<0.001){
					//just overwrite...
					m_oaOpp.RemoveAt(l);
					delete pOldOpp;
					//... and continue to add
				}
			}
		}
		
		pOpp = XDirect.AddOpPoint(pOpp);
	}
	
	return true;
}


bool CMainFrame::ReadPolarFile(CString FileName)
{
	CFile fp;

	try{
		if (fp.Open(FileName, CFile::modeRead)) {
			CArchive ar(&fp, CArchive::load);
			int n;
			ar >> n;
			if(n<100000) {//old format
				LoadOldPolarFile(ar,n);
			}
			else if (n >=100000) {//new format XFLR5 v1.99+
				if(!LoadPolarFile(ar)) return false;
			}

			if(n>=100000){
				CPolar * pPolar;
				CFoil *pFoil;
				CString str;
				for (int j=0; j<m_oaPolar.GetSize(); j++){
					pPolar = (CPolar*)m_oaPolar.GetAt(j);
					for (int k=0; k<m_oaFoil.GetSize(); k++){
						pFoil = (CFoil*)m_oaFoil.GetAt(k);
						if (pFoil->m_FoilName==pPolar->m_FoilName){
							int pos = pPolar->m_PlrName.Find("_N",0);
							if(pos <= 0){
								pPolar->m_ACrit    = 9.0;
								pPolar->m_XTop     = 1.0;
								pPolar->m_XBot     = 1.0;
								str.Format("_N%.1f", 9.0);
								pPolar->m_PlrName += str;
								break;
							}
						}
					}
				}
			}

			ar.Close();
			fp.Close();
		}
	}
	catch (CException *ex){
		TCHAR   szCause[255];
		CString str;
		ex->GetErrorMessage(szCause, 255);
		str = _T("Error loading polar : ");
		str += szCause;
		AfxMessageBox(str);
		ex->Delete();
		return false;
	}
	return true;
}


void CMainFrame::OnFullInverse() 
{
	if(m_iApp == INVERSEDESIGN && XInverse.m_bFullInverse) return;

	XDirect.m_bAnimate  = false;
	Miarex.m_bAnimate = false;

	HideBars();

	m_iApp = INVERSEDESIGN;
	XInverse.m_bFullInverse = true;

	SetMenu(&m_XInverseMenu);	
	SetRecentFileMenu();

	XInverse.SetParams();

	m_XInverseBar.ShowWindow(SW_SHOW);
	DockControlBar(&m_XInverseBar, AFX_IDW_DOCKBAR_TOP);

	m_FInvCtrlBar.ShowWindow(SW_SHOW);
	DockControlBar(&m_FInvCtrlBar, AFX_IDW_DOCKBAR_RIGHT);

	m_wndView.SetFocus();
}


void CMainFrame::OnMixedInverse() 
{
	if(m_iApp == INVERSEDESIGN && !XInverse.m_bFullInverse) return;

	XDirect.m_bAnimate  = false;
	Miarex.m_bAnimate = false;

	HideBars();

	m_iApp = INVERSEDESIGN;
	XInverse.m_bFullInverse = false;

	SetMenu(&m_XInverseMenu);	
	SetRecentFileMenu();

	XInverse.SetParams();

	m_XInverseBar.ShowWindow(SW_SHOW);
	DockControlBar(&m_XInverseBar, AFX_IDW_DOCKBAR_TOP);

	m_MInvCtrlBar.ShowWindow(SW_SHOW);
	DockControlBar(&m_MInvCtrlBar, AFX_IDW_DOCKBAR_RIGHT);

	m_wndView.SetFocus();
}




void CMainFrame::OnSelChangeWing()
{
	// Gets the new selected wing name and notifies Miarex
	// then updates WPolar combobox
	// and selects either the current WPolar
	// or the first one in the list, if any
	Miarex.m_bAnimate = false;
	m_WOperDlgBar.m_ctrlAnimate.SetCheck(FALSE);
	CString strong;
	int sel = m_ctrlUFO.GetCurSel();
	if (sel !=CB_ERR) m_ctrlUFO.GetLBText(sel, strong);
	
	Miarex.SetUFO(strong);

	m_iApp = MIAREX;
	UpdateWPlrs();
	UpdateWOpps();
//	Miarex.SetWPlr(false);
	Miarex.UpdateView();
}


void CMainFrame::OnSelChangeWPlr()
{
	Miarex.m_bAnimate = false;
	m_WOperDlgBar.m_ctrlAnimate.SetCheck(FALSE);
	CString strong;
	int sel = m_ctrlWPlr.GetCurSel();
	if (sel !=CB_ERR) m_ctrlWPlr.GetLBText(sel, strong);
	m_iApp = MIAREX;
	Miarex.SetWPlr(false, strong);
//	Miarex.SetWOpp(true);
	Miarex.UpdateView();
}


void CMainFrame::OnSelChangeWOpp()
{
	Miarex.m_bAnimate = false;
	m_WOperDlgBar.m_ctrlAnimate.SetCheck(FALSE);
	// Gets the new selected WOpp name and notifies Miarex
	if(!m_ctrlWOpp.GetCount()){
		Miarex.m_pCurWOpp = NULL;
		if (Miarex.m_iView==1)     Miarex.CreateWOppCurves();
		else if(Miarex.m_iView==4) Miarex.CreateCpCurves();

		Miarex.UpdateView();
		return;
	}

	CString strong;
	int sel = m_ctrlWOpp.GetCurSel();
	if (sel !=CB_ERR) 
		m_ctrlWOpp.GetLBText(sel, strong);
	else {
		m_ctrlWOpp.SetCurSel(0);
		m_ctrlWOpp.GetLBText(0, strong);
	}

	if(!strong.IsEmpty()){
		double Alpha;
		int	res = sscanf(strong, "%lf", &Alpha);
		if(res) {
			m_iApp = MIAREX;
			Miarex.SetWOpp(false, Alpha);
			Miarex.UpdateView();
		}
	}
}


void CMainFrame::OnSelChangeFoil()
{
	// Gets the new selected foil name and notifies XDirect
	// then updates polar combobox
	// and selects either the current polar
	// or the first one in the list, if any
	
	XDirect.m_bAnimate = false;
	m_OperDlgBar.m_ctrlAnimate.SetCheck(FALSE);
	CString strong;
	int sel = m_ctrlFoil.GetCurSel();
	if (sel !=CB_ERR) m_ctrlFoil.GetLBText(sel, strong);
	
	m_pCurFoil = XDirect.SetFoil(strong);
//	XDirect.SetPolar();
	m_iApp = XFOILANALYSIS;
	UpdatePlrs();

	XDirect.UpdateView();
}


void CMainFrame::OnSelChangePlr()
{
	XDirect.m_bAnimate = false;
	m_OperDlgBar.m_ctrlAnimate.SetCheck(FALSE);
	// Gets the new selected polar name and notifies XDirect
	CString strong;
	int sel = m_ctrlPlr.GetCurSel();
	if (sel !=CB_ERR) m_ctrlPlr.GetLBText(sel, strong);
	m_iApp = XFOILANALYSIS;
	XDirect.SetPolar(strong);
	UpdateOpps();
//	UpdatePlrs();
//	XDirect.SetOpp();
	XDirect.UpdateView();
}


void CMainFrame::OnSelChangeOpp()
{
	XDirect.m_bAnimate = false;
	m_OperDlgBar.m_ctrlAnimate.SetCheck(FALSE);
	// Gets the new selected Opp name and notifies XDirect
	CString strong;
	int sel = m_ctrlOpp.GetCurSel();
	if (sel!=CB_ERR) m_ctrlOpp.GetLBText(sel, strong);
	m_iApp = XFOILANALYSIS;
	double Alpha;
	int	res = sscanf(strong, "%lf", &Alpha);
	if(res) {
		XDirect.OnOper();
		XDirect.SetOpp(Alpha);
		XDirect.CreateOppCurves();
		XDirect.UpdateView();
	}
	else {
		AfxMessageBox("Unknown Operating point");
		XDirect.m_pCurOpp = NULL;
		XDirect.SetOpp();
		XDirect.UpdateView();
	}
}



void CMainFrame::UpdateUFOs()
{
	// fills combobox with wing and plane names 
	// then selects the current wing or plane, if any
	// else selects the first, if any
	// else disables the combobox
	int i;
	CString strong;
	m_ctrlUFO.ResetContent();

	CWing *pWing, *pCurWing;
	CPlane *pPlane, *pCurPlane;
	pCurWing  = Miarex.m_pCurWing;
	pCurPlane = Miarex.m_pCurPlane;

	
	for (i=0; i<m_oaWing.GetSize(); i++){
		pWing = (CWing*)m_oaWing[i];
		m_ctrlUFO.AddString(pWing->m_WingName);
	}
	for (i=0; i<m_oaPlane.GetSize(); i++){
		pPlane = (CPlane*)m_oaPlane[i];
		m_ctrlUFO.AddString(pPlane->m_PlaneName);
	}
	if (m_ctrlUFO.GetCount()){
 		m_ctrlUFO.EnableWindow(true);
		//select the current Wing, if any...
		if(pCurPlane){
			int pos = m_ctrlUFO.FindStringExact(-1, pCurPlane->m_PlaneName);
			if(pos!=CB_ERR) {
				m_ctrlUFO.SetCurSel(pos);
//				Miarex.SetUFO();
			}
			else {// if error, select the first
				m_ctrlUFO.SetCurSel(0);
				m_ctrlUFO.GetLBText(0, strong);
//				Miarex.SetUFO(strong);
			}		
		}
		else if(pCurWing){
			int pos = m_ctrlUFO.FindStringExact(-1, pCurWing->m_WingName);
			if(pos!=CB_ERR) {
				m_ctrlUFO.SetCurSel(pos);
//				Miarex.SetUFO();
			}
			else {// if error, select the first
				m_ctrlUFO.SetCurSel(0);
				m_ctrlUFO.GetLBText(0, strong);
//				Miarex.SetUFO(strong);
			}		
		}
		//... else select the first
		else {
			m_ctrlUFO.SetCurSel(0);
			m_ctrlUFO.GetLBText(0, strong);
//			Miarex.SetUFO(strong);
		}
	}
 	else {
		m_ctrlUFO.ResetContent();
 		m_ctrlUFO.EnableWindow(false);

		Miarex.m_pCurPlane  = NULL;
		Miarex.m_pCurWing   = NULL;
		Miarex.m_pCurWPolar = NULL;
		Miarex.m_pCurWOpp   = NULL;
		Miarex.m_pCurPOpp   = NULL;
//		Miarex.SetUFO();
	}
	UpdateWPlrs();
}

void CMainFrame::UpdateWPlrs()
{
	// fills combobox with WPolar names associated to Miarex's current wing
	// then selects Miarex current WPolar if any, else selects the first, if any
	// else disables the combobox

 	CWPolar *pWPolar;
	CString strong, UFOName;
	m_ctrlWPlr.ResetContent();

	CPlane  *pCurPlane = Miarex.m_pCurPlane;
	CWing   *pCurWing  = Miarex.m_pCurWing;
	CWPolar *pCurWPlr  = Miarex.m_pCurWPolar;

	if(pCurPlane) UFOName = pCurPlane->m_PlaneName;
	else if(pCurWing)  UFOName = pCurWing->m_WingName;
	else UFOName = "";

	if(!UFOName.GetLength()){
		Miarex.m_pCurWPolar = NULL;
//		Miarex.SetWPlr();
 		m_ctrlWPlr.EnableWindow(false);
		UpdateWOpps();
		return;
	}

	int size = 0;
	//count the number of WPolars associated to the current Wing
 	for (int i=0; i<m_oaWPolar.GetSize(); i++){
 		pWPolar = (CWPolar*)m_oaWPolar[i];
 		if(pWPolar->m_UFOName == UFOName){
 			size++;
 		}
 	}
 	if (size){// if any
  		m_ctrlWPlr.EnableWindow(true);
		for (int i=0; i<m_oaWPolar.GetSize(); i++){
 			pWPolar = (CWPolar*)m_oaWPolar[i];
 			if(pWPolar->m_UFOName == UFOName){
 				m_ctrlWPlr.AddString(pWPolar->m_PlrName);
 			}
 		}
		//select the current Wing Polar, if any...
		if(pCurWPlr){
			int pos = m_ctrlWPlr.FindStringExact(-1, pCurWPlr->m_PlrName);
			if(pos!=CB_ERR) m_ctrlWPlr.SetCurSel(pos);
			else {// if error, select the first
				m_ctrlWPlr.SetCurSel(0);
				m_ctrlWPlr.GetLBText(0, strong);
//				Miarex.SetWPlr(false, strong);
			}		
		}
		//... else select the first
		else {
			m_ctrlWPlr.SetCurSel(0);
			m_ctrlWPlr.GetLBText(0, strong);
//			Miarex.SetWPlr(false, strong);
		}

	}
 	else {// otherwise disable control
 		m_ctrlWPlr.EnableWindow(false);
		Miarex.m_pCurWPolar = NULL;
		Miarex.m_pCurWOpp = NULL;
//		Miarex.SetWPlr();
	}
}



void CMainFrame::UpdateWOpps()
{
	// fills combobox with WOpp names associated to Miarex' current WPLr
	// then selects Miarex current WOpp if any, else selects the first, if any
	// else disables the combobox

 	CWOpp *pWOpp;
 	CPOpp *pPOpp;
	CString strong;
	m_ctrlWOpp.ResetContent();

	CPlane  *pCurPlane   = Miarex.m_pCurPlane;
	CWing   *pCurWing    = Miarex.m_pCurWing;
	CWPolar *pCurWPlr    = Miarex.m_pCurWPolar;

	if (!pCurWing || !pCurWing->m_WingName.GetLength() ||
		!pCurWPlr || !pCurWPlr->m_PlrName.GetLength()){
 		m_ctrlWOpp.EnableWindow(false);
		Miarex.m_pCurWOpp = NULL;
//		Miarex.SetWOpp(true);
		return;
	}

	if(pCurPlane){
		int size = 0;
		//count the number of POpps associated to the current Wing & WPolar
 		for (int i=0; i<m_oaPOpp.GetSize(); i++){
 			pPOpp = (CPOpp*)m_oaPOpp[i];
 			if (pPOpp->m_PlaneName == pCurPlane->m_PlaneName &&
				pPOpp->m_PlrName   == pCurWPlr->m_PlrName){
 				size++;
 			}
 		}
		CString str;
		if (size){// if any
  			m_ctrlWOpp.EnableWindow(true);
			for (int i=0; i<m_oaPOpp.GetSize(); i++){
 				pPOpp = (CPOpp*)m_oaPOpp[i];	
 				if (pPOpp->m_PlaneName == pCurPlane->m_PlaneName &&
					pPOpp->m_PlrName == pCurWPlr->m_PlrName){
					if(pCurWPlr->m_Type != 4) str.Format("%8.2f", pPOpp->m_Alpha);
					else                      str.Format("%8.2f", pPOpp->m_QInf);

 					m_ctrlWOpp.AddString(str);
 				}
 			}
//			Miarex.SetPOpp(true);
			if(Miarex.m_pCurPOpp){
				if(pCurWPlr->m_Type != 4) str.Format("%8.2f", Miarex.m_pCurPOpp->m_Alpha);
				else                      str.Format("%8.2f", Miarex.m_pCurPOpp->m_QInf);

				int pos = m_ctrlWOpp.FindStringExact(-1,str);
				if(pos != CB_ERR) {
					m_ctrlWOpp.SetCurSel(pos);
				}
				else {
					m_ctrlWOpp.SetCurSel(0);
				}
			}
			else{
				m_ctrlWOpp.SetCurSel(0);
			}

		}
 		else {// otherwise disable control
 			m_ctrlWOpp.EnableWindow(false);
			Miarex.m_pCurPOpp = NULL;
//			Miarex.SetPOpp(true);
		}
	}
	else {
		int size = 0;
		//count the number of WOpps associated to the current Wing & WPolar
 		for (int i=0; i<m_oaWOpp.GetSize(); i++){
 			pWOpp = (CWOpp*)m_oaWOpp[i];
 			if (pWOpp->m_WingName == pCurWing->m_WingName &&
				pWOpp->m_PlrName  == pCurWPlr->m_PlrName){
 				size++;
 			}
 		}
		CString str;
		if (size){// if any
  			m_ctrlWOpp.EnableWindow(true);
			for (int i=0; i<m_oaWOpp.GetSize(); i++){
 				pWOpp = (CWOpp*)m_oaWOpp[i];	
 				if (pWOpp->m_WingName == pCurWing->m_WingName &&
					pWOpp->m_PlrName == pCurWPlr->m_PlrName){
					if(pCurWPlr->m_Type != 4) str.Format("%8.2f", pWOpp->m_Alpha);
					else  str.Format("%8.2f", pWOpp->m_QInf);

 					m_ctrlWOpp.AddString(str);
 				}
 			}
//			Miarex.SetWOpp(true);
			if(Miarex.m_pCurWOpp){
				if(pCurWPlr->m_Type != 4) str.Format("%8.2f", Miarex.m_pCurWOpp->m_Alpha);
				else  str.Format("%8.2f", Miarex.m_pCurWOpp->m_QInf);

				int pos = m_ctrlWOpp.FindStringExact(-1,str);
				if(pos != CB_ERR) {
					m_ctrlWOpp.SetCurSel(pos);
				}
				else {
					m_ctrlWOpp.SetCurSel(0);
				}
			}
			else{
				m_ctrlWOpp.SetCurSel(0);
			}

		}
 		else {// otherwise disable control
 			m_ctrlWOpp.EnableWindow(false);
			Miarex.m_pCurWOpp = NULL;
//			Miarex.SetWOpp(true);
		}
	}
}


void CMainFrame::UpdateFoils()
{
	// fills combobox with foil names 
	// then selects XDirect current foil if any, else selects the first, if any
	// else disables the combobox

	CString strong;
	m_ctrlFoil.ResetContent();

	CFoil *pFoil;
	m_pCurFoil = XDirect.m_pCurFoil;

 	for (int i=0; i<m_oaFoil.GetSize(); i++){
 		pFoil = (CFoil*)m_oaFoil[i];
		m_ctrlFoil.AddString(pFoil->m_FoilName);
 	}

	if (m_ctrlFoil.GetCount()){
 		m_ctrlFoil.EnableWindow(true);
		//select the current foil, if any...
		if (m_pCurFoil){
			int pos = m_ctrlFoil.FindStringExact(-1, m_pCurFoil->m_FoilName);
			if (pos!=CB_ERR) m_ctrlFoil.SetCurSel(pos);
			else {// if error, select the first...
				m_ctrlFoil.SetCurSel(0);
				m_ctrlFoil.GetLBText(0, strong);
				//...and set it
				XDirect.SetFoil(strong);
			}
		}
		//... else select the first
		else {
			m_ctrlFoil.SetCurSel(0);
			m_ctrlFoil.GetLBText(0, strong);
			//...and set it
			if(!XDirect.SetFoil(strong)){

			}
		}
	}
 	else {
		m_ctrlFoil.ResetContent();
 		m_ctrlFoil.EnableWindow(false);
		XDirect.SetFoil();
	}
	UpdatePlrs();
}


void CMainFrame::UpdatePlrs()
{
	// fills combobox with polar names associated to XDirect' current foil
	// then selects XDirect current polar if any, else selects the first, if any
	// else disables the combobox
	// sets the polar in XDirect in all cases

 	CPolar *pPolar;
	CString strong;
	m_ctrlPlr.ResetContent();

	m_pCurFoil = XDirect.m_pCurFoil;

	if(!m_pCurFoil || !m_pCurFoil->m_FoilName.GetLength()){
 		m_ctrlPlr.EnableWindow(false);
		XDirect.SetPolar();
		UpdateOpps();
		return;
	}

	int size = 0;
	//count the number of polars associated to the current foil
 	for (int i=0; i<m_oaPolar.GetSize(); i++){
 		pPolar = (CPolar*)m_oaPolar[i];
 		if(pPolar->m_FoilName == m_pCurFoil->m_FoilName){
 			size++;
 		}
 	}
 	if (size){// if any
  		m_ctrlPlr.EnableWindow(true);
		for (int i=0; i<m_oaPolar.GetSize(); i++){
 			pPolar = (CPolar*)m_oaPolar[i];
 			if(pPolar->m_FoilName == m_pCurFoil->m_FoilName){
 				m_ctrlPlr.AddString(pPolar->m_PlrName);
 			}
 		}
		if(XDirect.m_pCurPolar){
			int pos = m_ctrlPlr.FindStringExact(-1, XDirect.m_pCurPolar->m_PlrName);
			if(pos!=CB_ERR) m_ctrlPlr.SetCurSel(pos);
			else{
				m_ctrlPlr.SetCurSel(0);
				m_ctrlPlr.GetLBText(0, strong);
				XDirect.SetPolar(strong);
			}
		}
		else{
			m_ctrlPlr.SetCurSel(0);
			m_ctrlPlr.GetLBText(0, strong);
			XDirect.SetPolar(strong);
		}
	}
 	else {// otherwise disable control
 		m_ctrlPlr.EnableWindow(false);
 		XDirect.SetPolar();
	}
	UpdateOpps();
}

void CMainFrame::UpdateOpps()
{
	// fills the combobox with the Opp names associated to XDirect's current foil
	// then selects XDirect current opp if any, else selects the first, if any
	// else disables the combobox

	OpPoint *pOpp;
	CString strong;
	m_ctrlOpp.ResetContent();
	
	m_pCurFoil = XDirect.m_pCurFoil;
	CPolar *pCurPlr    = XDirect.m_pCurPolar;
	
	if (!m_pCurFoil || !m_pCurFoil->m_FoilName.GetLength() ||
		!pCurPlr    || !pCurPlr->m_PlrName.GetLength()){
		m_ctrlOpp.ResetContent();
		m_ctrlOpp.EnableWindow(false);
		return;
	}

	int size = 0;
	//count the number of Opps associated to the current foil & polar
 	for (int i=0; i<m_oaOpp.GetSize(); i++){
 		pOpp = (OpPoint*)m_oaOpp[i];
 		if (pOpp->m_strFoilName == m_pCurFoil->m_FoilName &&
			pOpp->m_strPlrName  == pCurPlr->m_PlrName){
 			size++;
 		}
 	}
	CString str;
 	if (size){// if any
  		m_ctrlOpp.EnableWindow(true);
		for (int i=0; i<m_oaOpp.GetSize(); i++){
 			pOpp = (OpPoint*)m_oaOpp[i];	
 			if (pOpp->m_strFoilName == m_pCurFoil->m_FoilName &&
				pOpp->m_strPlrName  == pCurPlr->m_PlrName){
				if (pCurPlr->m_Type !=4){
					if(fabs(pOpp->Alpha)<0.0001f) pOpp->Alpha = 0.0001f;
					str.Format("%8.2f", pOpp->Alpha);
 					m_ctrlOpp.AddString(str);
				}
				else{
					str.Format("%8.0f", pOpp->Reynolds);
 					m_ctrlOpp.AddString(str);
				}
 			}
 		}
		if (XDirect.m_pCurOpp && 
			XDirect.m_pCurOpp->m_strFoilName==XDirect.m_pCurFoil->m_FoilName){//select it
			if (pCurPlr->m_Type !=4){
				str.Format("%8.2f", XDirect.m_pCurOpp->Alpha);
			}
			else{
				str.Format("%8.0f", XDirect.m_pCurOpp->Reynolds);
			}
			int pos = m_ctrlOpp.FindStringExact(-1,str);
			if(pos!=CB_ERR) m_ctrlOpp.SetCurSel(pos);
		}
		else {//select the first
			m_ctrlOpp.SetCurSel(0);
			m_ctrlOpp.GetLBText(0, str);
			double x;
			int res = sscanf(str, "%lf", &x);
			if(res == 1){
				XDirect.SetOpp(x);
			}
			else {
				m_ctrlOpp.EnableWindow(false);
				XDirect.m_pCurOpp = NULL;
			}
		}
	}
 	else {// otherwise disable control
 		m_ctrlOpp.EnableWindow(false);
		XDirect.m_pCurOpp = NULL;
		m_ctrlOpp.ResetContent();
	}
}


void CMainFrame::OnClose() 
{
	if(!m_bSaved){
		int resp = AfxMessageBox("Save project before exit ?", MB_YESNOCANCEL);
		if(resp == IDYES) {
			if (!m_ProjectName.GetLength() || m_ProjectName=="*") {
				if(!SaveProjectAs()) return; 
			}
			if(SaveProject(m_FileName)){
				SetSaveState(true);	
				m_wndStatusBar.SetWindowText("Project " + m_ProjectName + " has been saved");
			}
			else return;
		}
		else if (resp==IDCANCEL) return;
	}

	DeleteProject();

	SaveSettings();

	Miarex.m_FlowLinesDlg.ShowWindow(SW_HIDE);
	
	CFrameWnd::OnClose();
}


CFoil* CMainFrame::ReadFoilFile(CString FileName, bool bKeepExistingFoil)
{
	CStdioFile XFile;
	CFileException fe;
	CString FoilName;
	CFoil* pFoil = NULL;
	int res, line, i, ip;
	int pos=0;
	line = 0;
	double x,y, area;

	try{
		BOOL bOpen = XFile.Open(FileName, CFile::modeRead, &fe);

		if (bOpen){
			CString Strong, StrTemp;
			pFoil = new CFoil();

			BOOL bRead = TRUE;
			
			while(pos>=0 && bRead){
				bRead = XFile.ReadString(Strong);
				pos = Strong.Find("#",0);
			}
			
			if(bRead) {
				res = sscanf(Strong, "%lf%lf", &x,&y);
				if(res==2){
					//there isn't a name on the first line
					FoilName = "New Foil";
					Rewind1Line(&XFile,line, Strong);
				}
				else FoilName = Strong;

				if(!bRead){
					CFileException *pEx = new CFileException(CFileException::invalidFile);
					pEx->m_strFileName = FileName;
					throw(pEx);
				}
				// remove fore and aft spaces
				FoilName.TrimLeft();
				FoilName.TrimRight();
			}

			pFoil->n=0;
			while (bRead) {
				bRead = XFile.ReadString(Strong);
				pos = Strong.Find("#",0);

				if (bRead && pos<0) {
					res = sscanf(Strong, "%lf%lf", &x,&y);
					if(res==2){
						pFoil->xb[pFoil->nb] = x;
						pFoil->yb[pFoil->nb] = y;
						pFoil->nb++;
						if(pFoil->nb>IQX){
							CFileException *pEx = new CFileException(CFileException::invalidFile);
							pEx->m_strFileName = FileName;
							AfxMessageBox("Max number of nodes is exceeded", MB_OK);
							throw(pEx);
						}
					}
					else bRead = false;
				}
			}
			
			XFile.Close();

			pFoil->m_FoilName = FoilName;

// Check if the foil was written clockwise or counter-clockwise

			area = 0.0;
			for (i=0; i<pFoil->nb; i++){
				if(i==pFoil->nb-1)	ip = 0;
				else				ip = i+1;
				area +=  0.5*(pFoil->yb[i]+pFoil->yb[ip])*(pFoil->xb[i]-pFoil->xb[ip]);
			}

			if(area < 0.0){
				//reverse the points order
				double xtmp, ytmp;
				for (int i=0; i<pFoil->nb/2; i++){
					xtmp         = pFoil->xb[i];
					ytmp         = pFoil->yb[i];
					pFoil->xb[i] = pFoil->xb[pFoil->nb-i-1];
					pFoil->yb[i] = pFoil->yb[pFoil->nb-i-1];
					pFoil->xb[pFoil->nb-i-1] = xtmp;
					pFoil->yb[pFoil->nb-i-1] = ytmp;
				}
			}

			memcpy(pFoil->x, pFoil->xb, sizeof(pFoil->xb));
			memcpy(pFoil->y, pFoil->yb, sizeof(pFoil->yb));
			pFoil->n=pFoil->nb;

			pFoil->m_FoilColor = GetColor(0);

			if(!XDirect.InitXFoil(pFoil)){
				DeleteFoil(pFoil, false);
				return NULL;
			}
			else {
				pFoil = SetModFoil(pFoil, bKeepExistingFoil);
				return pFoil;
			}
		}
		return NULL;
	}
	catch (CFileException *ex){
		TCHAR   szCause[255];
		CString str;
		ex->GetErrorMessage(szCause, 255);
		str = _T("Error loading foil : ");
		str += szCause;
		AfxMessageBox(str);
		ex->Delete();
		if(pFoil) delete pFoil;
		return NULL;
	}
}

COLORREF CMainFrame::GetColor(int type)
{
	//type
	// 0=Foil
	// 1=Polar
	// 2=Opp
	// 3=Wing (unused)
	// 4=WPolar
	// 5=WOpp
	// 6=POpp
	COLORREF color = RGB(255,0,0);
	int i,j;
	bool bFound = false;
	switch (type){
		case 0:{
			CFoil *pFoil;
			for (j=0; j<24; j++){
				for (i=0; i<m_oaFoil.GetSize(); i++){
					pFoil = (CFoil*)m_oaFoil.GetAt(i);
					bFound = false;
					if(pFoil->m_FoilColor == m_crColors[j]){
						bFound = true;
						break;
					}
				}
				if(!bFound) return m_crColors[j];
			}
			return m_crColors[m_oaFoil.GetSize()%24];				
			break;
		}
		case 1:{
			CPolar *pPolar;
			for (j=0; j<24; j++){
				for (i=0; i<m_oaPolar.GetSize(); i++){
					pPolar = (CPolar*)m_oaPolar.GetAt(i);
					bFound = false;
					if(pPolar->m_Color == m_crColors[j]){
						bFound = true;
						break;
					}
				}
				if(!bFound) return m_crColors[j];
			}
			return m_crColors[m_oaPolar.GetSize()%24];
			break;
		}
		case 2:{
			OpPoint *pOpPoint;
			for (j=0; j<24; j++){
				for (i=0; i<m_oaOpp.GetSize(); i++){
					pOpPoint = (OpPoint*)m_oaOpp.GetAt(i);
					bFound = false;
					if(pOpPoint->m_Color == m_crColors[j]){
						bFound = true;
						break;
					}
				}
				if(!bFound) return m_crColors[j];
			}
			return m_crColors[m_oaOpp.GetSize()%24];
			break;
		}
		case 4:{
			CWPolar *pWPolar;
			for (j=0; j<24; j++){
				for (i=0; i<m_oaWPolar.GetSize(); i++){
					pWPolar = (CWPolar*)m_oaWPolar.GetAt(i);
					bFound = false;
					if(pWPolar->m_Color == m_crColors[j]){
						bFound = true;
						break;
					}
				}
				if(!bFound) 
					return m_crColors[j];
				
			}
			return m_crColors[m_oaWPolar.GetSize()%24];
			break;
		}
		case 5:{
			CWOpp *pWOpp;
			for (j=0; j<24; j++){
				for (i=0; i<m_oaWOpp.GetSize(); i++){
					pWOpp = (CWOpp*)m_oaWOpp.GetAt(i);
					bFound = false;
					if(pWOpp->m_Color == m_crColors[j]){
						bFound = true;
						break;
					}
				}
				if(!bFound) return m_crColors[j];
			}
			return m_crColors[m_oaWOpp.GetSize()%24];	
			break;
		}
		case 6:{
			CPOpp *pPOpp;
			for (j=0; j<24; j++){
				for (i=0; i<m_oaPOpp.GetSize(); i++){
					pPOpp = (CPOpp*)m_oaPOpp.GetAt(i);
					bFound = false;
					if(pPOpp->m_Color == m_crColors[j]){
						bFound = true;
						break;
					}
				}
				if(!bFound) 
					return m_crColors[j];
			}
			return m_crColors[m_oaPOpp.GetSize()%24];
			break;
		}

		default:{
			return RGB(255,0,0);
		}
	}
	return color;	
}

double CMainFrame::GetCl(CFoil *pFoil0, CFoil *pFoil1, 
					 double Re, double Alpha, double Tau, bool &bOutRe, bool &bError)
{

	double Cl0, Cl1;
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	if(!pFoil0) 
		Cl0 = 2.0*pi*(Alpha*pi/180.0);
	else 
		Cl0 = GetPlrPoint(pFoil0, Re, Alpha, 1, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;
	if(!pFoil1) 
		Cl1 = 2.0*pi*(Alpha*pi/180.0);
	else 
		Cl1 = GetPlrPoint(pFoil1, Re, Alpha, 1, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;

	return ((1-Tau) * Cl0 + Tau * Cl1);
}

void * CMainFrame::GetPlrVariable(CPolar *pPolar, int iVar)
{
	// returns a pointer to the variable array defined by its index iVar
	
	void * pVar;
	switch (iVar){
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

double CMainFrame::GetPlrPoint(CFoil *pFoil, double Re, double Alpha, 
						       int PlrVar, bool &bOutRe, bool &bError)
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

	CArray <double, double> *pX;	
	double amin, amax;
	CPolar *pPolar;

	bOutRe = false;
	bError = false;

	if(!pFoil) {
		bOutRe = true;
		bError = true;
		return 0.000;
	}


	int size;
	int n = 0;

	// Are there any Type 1 polars available for this foil ?
	for (int i = 0; i< m_oaPolar.GetSize(); i++){
		pPolar = (CPolar*)m_oaPolar.GetAt(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName)){
			n++;
			if(n>=2) break;
		}
	}

//more than one polar - interpolate between  - tough job

	//First Find the two polars with Reynolds number surrounding wanted Re
	CPolar * pPolar1 = NULL;
	CPolar * pPolar2 = NULL;
	int nPolars = (int)m_oaPolar.GetSize();
	//Type 1 Polars are sorted by crescending Re Number

	//if Re is less than that of the first polar, use this one
	for (i=0; i< nPolars; i++){
		pPolar = (CPolar*)m_oaPolar.GetAt(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName)){
			// we have found the first type 1 polar for this foil
			if (Re < pPolar->m_Reynolds) {
				bOutRe = true;
				//interpolate Alpha on this polar
				pX = (CArray <double, double> *) GetPlrVariable(pPolar, PlrVar);
				size = (int)pPolar->m_Alpha.GetSize();
				if(Alpha < pPolar->m_Alpha[0])      {
					return (*pX)[0];
				}
				if(Alpha > pPolar->m_Alpha[size-1])	{
					return (*pX)[size-1];
				}
				for (i=0; i<size-1; i++){
					if(pPolar->m_Alpha[i] <= Alpha && Alpha < pPolar->m_Alpha[i+1]){
						//interpolate
						if(pPolar->m_Alpha[i+1]-pPolar->m_Alpha[i] < 0.00001)//do not divide by zero
							return (*pX)[i];
						else {
							double u = (Alpha - pPolar->m_Alpha[i])
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
	for (i=0; i< nPolars; i++){
		pPolar = (CPolar*)m_oaPolar.GetAt(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName)){
			// we have found the first type 1 polar for this foil
			pPolar->GetAlphaLimits(amin, amax);
			if (pPolar->m_Reynolds <= Re) {
				if(amin <= Alpha && Alpha <= amax) {	
					pPolar1 = pPolar;
				}
			}
			else {
				if(amin <= Alpha && Alpha <= amax){	
					pPolar2 = pPolar;
					break;
				}
			}
		}
	}

	if (!pPolar2) {
		//then Re is greater than that of any polar
		// so use last polar and interpolate alphas on this polar
		bOutRe = true;
		if(!pPolar1){
			bError = true;
			return 0.000;
		}
		size = (int)pPolar1->m_Alpha.GetSize();
		if(!size) {
			bError = true;
			return 0.000;
		}

		pX = (CArray <double, double> *) GetPlrVariable(pPolar1, PlrVar);
		if(Alpha < pPolar1->m_Alpha[0])	     return (*pX)[0];
		if(Alpha > pPolar1->m_Alpha[size-1]) return (*pX)[size-1];
		for (i=0; i<size-1; i++){
			if(pPolar1->m_Alpha[i] <= Alpha && Alpha < pPolar1->m_Alpha[i+1]){
				//interpolate
				if(pPolar1->m_Alpha[i+1]-pPolar1->m_Alpha[i] < 0.00001){//do not divide by zero
					return (*pX)[i];
				}
				else{
					double u = (Alpha - pPolar1->m_Alpha[i])
							 /(pPolar1->m_Alpha[i+1]-pPolar1->m_Alpha[i]);
					return (*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]);
				}
			}
		}
		//Out in Re, out in alpha...
		return (*pX)[size-1] ;

	}
	else {
		// Re is between that of polars 1 and 2
		// so interpolate alphas for each
		double Var1, Var2;

		if(!pPolar1) {
			bOutRe = true; 
			bError = true;
			return 0.000;
		}
		size = (int)pPolar1->m_Alpha.GetSize();
		if(!size) {
			bOutRe = true;
			bError = true;
			return 0.000;
		}
		pX = (CArray <double, double> *) GetPlrVariable(pPolar1, PlrVar);
		if(Alpha < pPolar1->m_Alpha[0])	          Var1 = (*pX)[0];
		else if(Alpha > pPolar1->m_Alpha[size-1]) Var1 = (*pX)[size-1];
		else{
			for (i=0; i<size-1; i++){
				if(pPolar1->m_Alpha[i] <= Alpha && Alpha < pPolar1->m_Alpha[i+1]){
					//interpolate
					if(pPolar1->m_Alpha[i+1]-pPolar1->m_Alpha[i] < 0.00001)//do not divide by zero
						Var1 = (*pX)[i];
					else{
						double u = (Alpha - pPolar1->m_Alpha[i])
								 /(pPolar1->m_Alpha[i+1]-pPolar1->m_Alpha[i]);
						Var1 = (*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]);
					}
				}
			}
		}

		size = (int)pPolar2->m_Alpha.GetSize();
		if(!size) {
			bOutRe = true;
			bError = true;
			return 0.000;
		}
		pX = (CArray <double, double> *) GetPlrVariable(pPolar2, PlrVar);
		if(Alpha < pPolar2->m_Alpha[0]){
			bOutRe = true;
			bError = true;
			Var2 = (*pX)[0];
		}
		else if(Alpha > pPolar2->m_Alpha[size-1]) {
			bOutRe = true;
			bError = true;
			Var2 = (*pX)[size-1];
		}
		else{
			for (i=0; i<size-1; i++){
				if(pPolar2->m_Alpha[i] <= Alpha && Alpha < pPolar2->m_Alpha[i+1]){
					//interpolate
					pX = (CArray <double, double> *) GetPlrVariable(pPolar2, PlrVar);
					if(pPolar2->m_Alpha[i+1]-pPolar2->m_Alpha[i] < 0.00001)//do not divide by zero
						Var2 = (*pX)[i];
					else{
						double u = (Alpha - pPolar2->m_Alpha[i])
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



CFoil* CMainFrame::GetFoil(CString strFoilName)
{
	//returns a pointer to the foil with the corresponding name
	// returns NULL if not found
	if(!strFoilName.GetLength()) return NULL;

	CFoil* pFoil;
	for (int i=0; i<m_oaFoil.GetSize(); i++){
		pFoil = (CFoil*)m_oaFoil.GetAt(i);
		if (pFoil->m_FoilName == strFoilName) return pFoil;
	}
	return NULL;
}


CFoil* CMainFrame::AddFoil(CString strFoilName, double x[], double y[], int nf)
{
	CFoil *pOldFoil = GetFoil(strFoilName);
	if(!pOldFoil){
		CFoil *pFoil = new CFoil();
		pFoil->n = nf;
		for (int i=0; i<nf; i++){
			pFoil->x[i] = x[i];
			pFoil->y[i] = y[i];
		}
		pFoil->m_FoilColor  = m_crColors[m_oaFoil.GetSize()%24];
		pFoil->m_FoilName   = strFoilName;
		pFoil->InitFoil();
		bool IsInserted = false;
		for (i=0; i<m_oaFoil.GetSize(); i++){
			pOldFoil = (CFoil*)m_oaFoil.GetAt(i);
			if (pFoil->m_FoilName < pOldFoil->m_FoilName){
				m_oaFoil.InsertAt(i, pFoil);
				IsInserted = true;
				break;
			}
		}
		if (!IsInserted) m_oaFoil.Add(pFoil);

		return pFoil;
	}
	else return pOldFoil;
}


void CMainFrame::AddFoil(CFoil *pFoil)
{
	CFoil *pOldFoil = GetFoil(pFoil->m_FoilName);
	if(pOldFoil){
		DeleteFoil(pOldFoil, false);
	}
	
	bool IsInserted = false;

	for (int i=0; i<m_oaFoil.GetSize(); i++){
		pOldFoil = (CFoil*)m_oaFoil.GetAt(i);
		if (pFoil->m_FoilName < pOldFoil->m_FoilName){
			m_oaFoil.InsertAt(i, pFoil);
			IsInserted = true;
			break;
		}
	}
	if (!IsInserted) m_oaFoil.Add(pFoil);
//	if(pFoil->m_bFlap) XDirect.SetFoilFlap(pFoil);
//	else 
		pFoil->InitFoil();
	SetCurrentFoil(pFoil);
	SetSaveState(false);	
}

CFoil* CMainFrame::SetModFoil(CFoil* pNewFoil, bool bKeepExistingFoil)
{
	// Adds the buffer foil to the ObArray,
	// gives it a proper name, FoilName or another,
	// selects it ,
	// and initializes XFoil, comboboxes and everything.

	bool bExists = false;
	bool bNotFound = true;
	pNewFoil->m_bSaved = false;
	CFoil*pOldFoil;

	if(!pNewFoil->m_FoilName.GetLength()){
		bExists = true;
	}
	else {
		for (int j=0; j<m_oaFoil.GetSize(); j++){
			pOldFoil = (CFoil*)m_oaFoil.GetAt(j);
			if (pOldFoil->m_FoilName == pNewFoil->m_FoilName) {
				bExists = true;
				if(bKeepExistingFoil){
						delete pNewFoil;
						pNewFoil = NULL;
						m_pCurFoil = NULL;
						return pOldFoil;
				}
				break;
			}
		}
	}
	if(bExists){

		while(bNotFound){
			CFoil *pFoil;
			CStringArray NameList;
			for(int k=0; k<m_oaFoil.GetSize(); k++){
				pFoil = (CFoil*)m_oaFoil.GetAt(k);
				NameList.Add(pFoil->m_FoilName);
			}
			CNameDlg dlg(this);
			dlg.m_pStrArray = & NameList;
			dlg.m_strQuestion = "A foil of that name already exists\r\nPlease enter a new name";
			dlg.m_strName = pNewFoil->m_FoilName;

			bool exists = false;
			CString strong;
			int resp = (int)dlg.DoModal();
			strong = dlg.m_strName;
			if(IDOK == resp){
				for (int l=0; l<m_oaFoil.GetSize(); l++){
					pOldFoil = (CFoil*)m_oaFoil.GetAt(l);
					if(pOldFoil->m_FoilName == strong) exists = true;
				}
				if (!exists){
					bNotFound = false;// at last (users !...)
					// so create and add foil to array
					pNewFoil->m_FoilName = strong;

					AddFoil(pNewFoil);
				}
			}
			else if(resp==10){// user wants to overwrite existing airfoil
				//So delete any foil with that name
				for (int l=(int)m_oaFoil.GetSize()-1;l>=0; l--){
					pOldFoil = (CFoil*)m_oaFoil.GetAt(l);
					if(pOldFoil->m_FoilName == strong){
						m_oaFoil.RemoveAt(l);
						delete pOldFoil;
						if(m_pCurFoil == pOldFoil) m_pCurFoil = NULL;
						if(XDirect.m_pCurFoil == pOldFoil) XDirect.m_pCurFoil = NULL;
					}
				}
				// delete all associated OpPoints
				OpPoint * pOpPoint;
				for (l=(int)m_oaOpp.GetSize()-1;l>=0;l--){
					pOpPoint = (OpPoint*)m_oaOpp[l];
					if (pOpPoint->m_strFoilName == strong){
						m_oaOpp.RemoveAt(l);
						if(pOpPoint == XDirect.m_pCurOpp) XDirect.m_pCurOpp = NULL;
						delete pOpPoint;
					}
				}
				// delete all Polar results for that airfoil, but keep polar for analysis
				CPolar * pPolar;
				for (l=0; l <m_oaPolar.GetSize();l++){
					pPolar = (CPolar*) m_oaPolar.GetAt(l);
					if (pPolar->m_FoilName == strong){
						pPolar->ResetPolar();
					}
				}
				// finally add to array
				pNewFoil->m_FoilName = strong;
				AddFoil(pNewFoil);
				bNotFound = false;//exit loop
			}
			else {// Cancel so exit
				delete pNewFoil;
				pNewFoil = NULL;
				m_pCurFoil = NULL;
				return NULL;// foil not added
			}
		}
	}
	else{
		AddFoil(pNewFoil);
	}
	if(m_iApp == XFOILANALYSIS) XDirect.SetFoil(pNewFoil);
	m_pCurFoil = pNewFoil;
	return pNewFoil;// foil added
}


bool CMainFrame::RenameFoil(CFoil* pFoil)
{
	bool bNotFound = true;
	int i,l;
	CFoil*pOldFoil;
	CPolar * pPolar;
	OpPoint * pOpPoint;
	CString OldName = pFoil->m_FoilName;
	while(bNotFound){
		CStringArray NameList;
		for(int k=0; k<m_oaFoil.GetSize(); k++){
			pOldFoil = (CFoil*)m_oaFoil.GetAt(k);
			NameList.Add(pOldFoil->m_FoilName);
		}
		CNameDlg dlg(this);
		dlg.m_pStrArray = & NameList;
		dlg.m_strQuestion = "Enter the foil's new name";
		dlg.m_strName = OldName;
		bool exists = false;
		CString strong;
		int resp = (int)dlg.DoModal();
		strong = dlg.m_strName;
		if(IDOK == resp){
			if (strong==OldName) return true;
			for (l=0; l<m_oaFoil.GetSize(); l++){
				pOldFoil = (CFoil*)m_oaFoil.GetAt(l);
				if(pOldFoil->m_FoilName == strong) {
					exists = true;
					break;
				}
			}
			if (!exists){
				bNotFound = false;// at last (users !...)
				// so rename foil and associated polars and opps
				pFoil->m_FoilName = strong;
				bool bInserted = false;
				for(i=0;i<m_oaFoil.GetSize();i++){
					pOldFoil = (CFoil*)m_oaFoil.GetAt(i);
					if(pOldFoil == pFoil) {
						m_oaFoil.RemoveAt(i);
						//and re-insert
						for(l=0;l<m_oaFoil.GetSize();l++){
							pOldFoil = (CFoil*)m_oaFoil.GetAt(l);
							if(pFoil->m_FoilName.CompareNoCase(pOldFoil->m_FoilName)<0) {
								//then insert before
								m_oaFoil.InsertAt(l, pFoil);
								bInserted = true;
								break;
							}
						}
						if(!bInserted)	m_oaFoil.Add(pFoil);
						break;
					}
				}
				for (i=0; i<m_oaPolar.GetSize(); i++){
					pPolar = (CPolar*)m_oaPolar.GetAt(i);
					if(pPolar->m_FoilName == OldName){
						pPolar->m_FoilName = strong;
					}
				}
				for (i=0; i<m_oaOpp.GetSize(); i++){
					pOpPoint = (OpPoint*)m_oaOpp.GetAt(i);
					if(pOpPoint->m_strFoilName == OldName){
						pOpPoint->m_strFoilName = strong;
					}
				}
				SetSaveState(false);
			}
		}
		else if(resp==10){// user wants to overwrite an existing airfoil
			if (strong==OldName) return true;
			//So delete any foil with that name
			for (l=(int)m_oaFoil.GetSize()-1;l>=0; l--){
				pOldFoil = (CFoil*)m_oaFoil.GetAt(l);
				if(pOldFoil->m_FoilName == strong){
					if(m_pCurFoil == pOldFoil) m_pCurFoil = NULL;
					if(XDirect.m_pCurFoil == pOldFoil) XDirect.m_pCurFoil = NULL;
					m_oaFoil.RemoveAt(l);
					delete pOldFoil;
				}
			}
			// delete all associated OpPoints
			OpPoint * pOpPoint;
			for (l=(int)m_oaOpp.GetSize()-1;l>=0;l--){
				pOpPoint = (OpPoint*)m_oaOpp[l];
				if (pOpPoint->m_strFoilName == strong){
					if(pOpPoint == XDirect.m_pCurOpp) XDirect.m_pCurOpp = NULL;
					m_oaOpp.RemoveAt(l);
					delete pOpPoint;
				}
			}
			// delete all Polar results for that airfoil
			CPolar * pPolar;
			for (l=(int)m_oaPolar.GetSize()-1;l>=0;l--){
				pPolar = (CPolar*) m_oaPolar.GetAt(l);
				if (pPolar->m_FoilName == strong){
					if(XDirect.m_pCurPolar == pPolar) XDirect.m_pCurPolar = NULL;
					m_oaPolar.RemoveAt(l);
					delete pPolar;
				}
			}
			// finally add to array
			pFoil->m_FoilName = strong;
			for (i=0; i<m_oaPolar.GetSize(); i++){
				pPolar = (CPolar*)m_oaPolar.GetAt(i);
				if(pPolar->m_FoilName == OldName){
					pPolar->m_FoilName = strong;
				}
			}
			for (i=0; i<(int)m_oaOpp.GetSize(); i++){
				pOpPoint = (OpPoint*)m_oaOpp.GetAt(i);
				if(pOpPoint->m_strFoilName == OldName){
					pOpPoint->m_strFoilName = strong;
				}
			}
//			UpdateFoils();
			SetSaveState(false);
			bNotFound = false;//exit loop
		}
		else {// Cancel so exit
			bNotFound = false;//exit loop
			return NULL;// foil not added
		}
	}
	return true;// foil added
}



void CMainFrame::SetCurrentFoil(CFoil* pFoil)
{
	XDirect.SetFoil(pFoil);
	AFoil.SetCurFoil(pFoil);
	m_pCurFoil = pFoil;
}



CPolar* CMainFrame::AddPolar(CPolar *pPolar)
{
	if(!pPolar) return NULL;
	bool bExists   = false;
	bool bInserted = false;
	CPolar *pOldPlr;
	CPolar *pOld2Plr;
	CString strong;

	for (int i=0; i<m_oaPolar.GetSize(); i++){
		pOldPlr = (CPolar*)m_oaPolar.GetAt(i);
		if (pOldPlr->m_PlrName  == pPolar->m_PlrName &&
			pOldPlr->m_FoilName == pPolar->m_FoilName) bExists = true;
	}

	while(!bInserted){
		if(!bExists){ 
			for (int j=0; j<m_oaPolar.GetSize(); j++){
				pOldPlr = (CPolar*)m_oaPolar.GetAt(j);
//				if (pPolar->m_FoilName < pOldPlr->m_FoilName  ) {
				if (pPolar->m_FoilName.CompareNoCase(pOldPlr->m_FoilName)<0) {
					m_oaPolar.InsertAt(j, pPolar);
					bInserted = true;
					break;
				}
/*				else if (pPolar->m_FoilName == pOldPlr->m_FoilName &&
						 pPolar->m_PlrName < pOldPlr->m_PlrName ) {
					m_oaPolar.InsertAt(j, pPolar);
					bInserted = true;
					break;
				}*/
				else if (pPolar->m_FoilName == pOldPlr->m_FoilName){
					if(pPolar->m_Type < pOldPlr->m_Type){
						m_oaPolar.InsertAt(j, pPolar);
						bInserted = true;
						break;
					}
					else if(pPolar->m_Type == pOldPlr->m_Type){
						if (pPolar->m_Type !=4){//sort by re Nbr
							if(pPolar->m_Reynolds < pOldPlr->m_Reynolds){
								m_oaPolar.InsertAt(j, pPolar);
								bInserted = true;
								break;
							}
						}
						else {//Type 4, sort by Alphas
							if(pPolar->m_ASpec < pOldPlr->m_ASpec){
								m_oaPolar.InsertAt(j, pPolar);
								bInserted = true;
								break;
							}
						}
					}
				}
			}	
			if(!bInserted)	{
				m_oaPolar.Add(pPolar);
				bInserted = true;
			}
			return pPolar;
		}
		else {
			for (int l=0; l<m_oaPolar.GetSize(); l++){
				pOldPlr = (CPolar*)m_oaPolar.GetAt(l);
				if (pOldPlr->m_FoilName == pPolar->m_FoilName &&
					pOldPlr->m_PlrName  == pPolar->m_PlrName){
					int p = 2;
					bool bFound = true;
					while(bFound){
						strong.Format(" (%d)", p);
						strong = pPolar->m_PlrName + strong;
						for (int k=0; k<m_oaPolar.GetSize(); k++){
							bFound = false;
							pOld2Plr = (CPolar*)m_oaPolar.GetAt(k);
							if (pOld2Plr->m_FoilName == pPolar->m_FoilName &&
								pOld2Plr->m_PlrName  == strong){
								p++;
								bFound = true;
								break;
							}
						}
					}
					pPolar->m_PlrName = strong;
				}
			}
			bExists = false;
		}
	}
	return NULL;
}



void CMainFrame::SaveSettings()
{
	CFile fp;

	WINDOWPLACEMENT wndpl;
	GetWindowPlacement(&wndpl);

	CString str;
	CString strAppDirectory;
	char    szAppPath[MAX_PATH] = "";
	GetTempPath(MAX_PATH,szAppPath);
	strAppDirectory = szAppPath;
	str =strAppDirectory + "XFLR5.set";


	if(fp.Open(str ,CFile::modeCreate | CFile::modeWrite))
	{
		CArchive ar(&fp, CArchive::store);
		ar << 100320;
			//100320 : added window placement data
			//100304 at version 3.04

		ar << wndpl.rcNormalPosition.left <<wndpl.rcNormalPosition.top 
		   << wndpl.rcNormalPosition.right <<wndpl.rcNormalPosition.bottom;
		ar << wndpl.showCmd;
		// Proceed with order...
		ar << m_LengthUnit;
		ar << m_AreaUnit;
		ar << m_SpeedUnit;
		ar << m_WeightUnit;
		ar << m_ForceUnit;
		ar << m_MomentUnit;

		ar << m_VLMColor << m_VLMStyle << m_VLMWidth;
		ar << m_OutlineColor << m_OutlineStyle << m_OutlineWidth;
		ar << m_XTopStyle << m_XTopWidth << m_XTopColor;
		ar << m_XBotStyle << m_XBotWidth << m_XBotColor;
		ar << m_XCPStyle << m_XCPWidth << m_XCPColor;
		ar << m_MomentStyle << m_MomentWidth << m_MomentColor;
		ar << m_IDragStyle << m_IDragWidth << m_IDragColor;
		ar << m_VDragStyle << m_VDragWidth << m_VDragColor;
		ar << m_DownwashStyle << m_DownwashWidth << m_DownwashColor;
		ar << m_WakeStyle << m_WakeWidth << m_WakeColor;
		ar << m_LiftScale << m_DragScale << m_DownwashScale;

		ar << m_RecentFile[0] << m_RecentFile[1] << m_RecentFile[2] << m_RecentFile[3] ;
		ar << m_RecentFile[4] << m_RecentFile[5] << m_RecentFile[6] << m_RecentFile[7] ;
		
		if(m_bSaveOpps)           ar << 1; else ar<<0;
		if(m_bSaveWOpps)          ar << 1; else ar<<0;


		m_wndView.SaveSettings(ar);
		XDirect.SaveSettings(ar);
		Miarex.SaveSettings(ar);
		AFoil.SaveSettings(ar);
		XInverse.SaveSettings(ar);

		ar.Close();
		fp.Close();
	}
}
 
void CMainFrame::LoadSettings()
{
	CFile fp;
	WINDOWPLACEMENT wndpl;
	int k;

	CString str;
	CString strAppDirectory;
	char    szAppPath[MAX_PATH] = "";
	GetTempPath(MAX_PATH,szAppPath);
	strAppDirectory = szAppPath;
	str =strAppDirectory + "XFLR5.set";

	try
	{
		if(fp.Open(str,CFile::modeRead)){
			CArchive ar(&fp, CArchive::load);
			ar >> k;
			if(k!=100320){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			ar >> wndpl.rcNormalPosition.left  >> wndpl.rcNormalPosition.top; //used in XFLR5App
			ar >> wndpl.rcNormalPosition.right >> wndpl.rcNormalPosition.bottom;//used in XFLR5App
			ar >> wndpl.showCmd;//used in XFLR5App
			//  we're reading/loading
			ar >> m_LengthUnit;
			ar >> m_AreaUnit;
			ar >> m_SpeedUnit;
			ar >> m_WeightUnit;
			ar >> m_ForceUnit;
			ar >> m_MomentUnit;
			if(m_MomentUnit<0 || m_MomentUnit>10){
				m_MomentUnit = 0;

				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}

			SetUnits(m_LengthUnit, m_AreaUnit, m_SpeedUnit, m_WeightUnit, m_ForceUnit, m_MomentUnit,
					 m_mtoUnit, m_m2toUnit, m_mstoUnit, m_kgtoUnit, m_NtoUnit, m_NmtoUnit);

			ar >> k;
			if(k<0 || k>RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			m_VLMColor = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_VLMStyle  = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_VLMWidth  = k;
			
			ar >> k;
			if(k<0 || k>RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			m_OutlineColor = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_OutlineStyle  = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_OutlineWidth  = k;
			
			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_XTopStyle  = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_XTopWidth  = k;
			
			ar >> k;
			if(k<0 || k>RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			m_XTopColor = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_XBotStyle  = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_XBotWidth  = k;
			
			ar >> k;
			if(k<0 || k>RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			m_XBotColor = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_XCPStyle  = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_XCPWidth  = k;
			
			ar >> k;
			if(k<0 || k>RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			m_XCPColor = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_MomentStyle  = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_MomentWidth  = k;
			
			ar >> k;
			if(k<0 || k>RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			m_MomentColor = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_IDragStyle  = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_IDragWidth  = k;
			
			ar >> k;
			if(k<0 || k>RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			m_IDragColor = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_VDragStyle  = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_VDragWidth  = k;
			
			ar >> k;
			if(k<0 || k>RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			m_VDragColor = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_DownwashStyle  = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_DownwashWidth  = k;
			
			ar >> k;
			if(k<0 || k>RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			m_DownwashColor = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_WakeStyle  = k;

			ar >> k;
			if(k<0 || k>10){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}			
			m_WakeWidth  = k;
			
			ar >> k;
			if(k<0 || k>RGB(255,255,255)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			m_WakeColor = k;

			ar >> m_LiftScale ;
			ar >> m_DragScale ;
			ar >> m_DownwashScale ;

			m_ScaleOppBar.m_LiftScale     = m_LiftScale;
			m_ScaleOppBar.m_DragScale     = m_DragScale;
			m_ScaleOppBar.m_DownwashScale = m_DownwashScale;

			ar >> m_RecentFile[0] >> m_RecentFile[1] >> m_RecentFile[2] >> m_RecentFile[3] ;
			ar >> m_RecentFile[4] >> m_RecentFile[5] >> m_RecentFile[6] >> m_RecentFile[7] ;

		
			ar >> k;
			if(k<0 || k>1){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			if(k) m_bSaveOpps = true; else m_bSaveOpps = false;

			ar >> k;
			if(k<0 || k>1){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = ar.m_strFileName;
				throw pfe;
			}
			if(k) m_bSaveWOpps = true; else m_bSaveWOpps = false;

			if(!m_wndView.LoadSettings(ar)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}

			if(!XDirect.LoadSettings(ar)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if(!Miarex.LoadSettings(ar)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if(!AFoil.LoadSettings(ar)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}
			if(!XInverse.LoadSettings(ar)){
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				pfe->m_strFileName = str;
				throw pfe;
			}

			ar.Close();
			fp.Close();
		}
	}
	catch (CArchiveException *ex){
		ex->Delete();
	}
	catch (CException *ex){
		ex->Delete();
	}
}

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;	
//	return CFrameWnd::OnEraseBkgnd(pDC);
}

void CMainFrame::ShowOperBar(bool bShow)
{
	if(bShow) m_OperDlgBar.ShowWindow(SW_SHOW);
	else m_OperDlgBar.ShowWindow(SW_HIDE);
}

void CMainFrame::CreateAFoilBar()
{

	m_AFoilBar.Create(this);
	m_AFoilBar.LoadToolBar(IDR_AFOILBAR);
	m_AFoilBar.SetBarStyle(CBRS_TOP	| CBRS_GRIPPER | CBRS_TOOLTIPS | 
						   CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_FLOAT_MULTI);

	m_AFoilBar.SetWindowText("Direct Design Controls");
	m_AFoilBar.SetHeight(32);
}

void CMainFrame::CreateXInverseBar()
{
	m_XInverseBar.Create(this);
	m_XInverseBar.LoadToolBar(IDR_XINVERSEBAR);
	m_XInverseBar.SetBarStyle(CBRS_TOP	| CBRS_GRIPPER | CBRS_TOOLTIPS | 
							  CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_FLOAT_MULTI);

	m_XInverseBar.SetWindowText("Inverse Design Controls");
	m_XInverseBar.SetHeight(32);
}


void CMainFrame::CreateMiarexBar()
{
	int cx, nWidth, nHeight, index;
	CRect rect;
	if(m_wndpl.showCmd == SW_SHOWMAXIMIZED){
		cx = GetSystemMetrics(SM_CXSCREEN);
	}
	else{
		cx = m_wndpl.rcNormalPosition.right - m_wndpl.rcNormalPosition.left;
	}

	nWidth = (int)((cx-400)/2);
	nWidth = min(nWidth,500);
	nWidth = max(150, nWidth);
	nHeight = 600;

	m_MiarexBar.Create(this);
	m_MiarexBar.LoadToolBar(IDR_MIAREXBAR);
	m_MiarexBar.SetBarStyle(CBRS_TOP	| CBRS_GRIPPER | CBRS_TOOLTIPS | 
						   CBRS_FLYBY | CBRS_SIZE_DYNAMIC| CBRS_FLOAT_MULTI);

	m_MiarexBar.SetWindowText("Wing selection");
	m_MiarexBar.SetHeight(34);

	index = 0;
    while(m_MiarexBar.GetItemID(index)!=IDC_CBWING && index <100) index++;
	m_MiarexBar.SetButtonInfo(index, IDC_CBWING, TBBS_SEPARATOR, nWidth);
	m_MiarexBar.GetItemRect(index, &rect);
	rect.bottom = rect.top + nHeight;
	m_ctrlUFO.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_SORT,
		              rect, &m_MiarexBar, IDC_CBWING);
	m_ctrlUFO.SetFont(&m_FFont);

//	nWidth = 270;
	index = 0;
    while(m_MiarexBar.GetItemID(index)!=IDC_CBWPLR && index <100) index++;
	m_MiarexBar.SetButtonInfo(index, IDC_CBWPLR, TBBS_SEPARATOR, nWidth);
	m_MiarexBar.GetItemRect(index, &rect);
	rect.bottom = rect.top + nHeight;
	m_ctrlWPlr.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_SORT,
		              rect, &m_MiarexBar, IDC_CBWPLR);
	m_ctrlWPlr.SetFont(&m_FFont);

	index = 0;
    while(m_MiarexBar.GetItemID(index)!=IDC_CBWOPP && index <100) index++;
	m_MiarexBar.SetButtonInfo(index, IDC_CBWOPP, TBBS_SEPARATOR, 80);
	m_MiarexBar.GetItemRect(index, &rect);
	rect.bottom = rect.top + nHeight;

	m_ctrlWOpp.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
		              rect, &m_MiarexBar, IDC_CBWOPP);
	m_ctrlWOpp.SetFont(&m_FFont);

	m_ctrlUFO.EnableWindow(false);
	m_ctrlWPlr.EnableWindow(false);
	m_ctrlWOpp.EnableWindow(false);
}

void CMainFrame::CreateXDirectBar()
{
	int cx, nWidth, nHeight, index;
	CRect rect;
	if(m_wndpl.showCmd == SW_SHOWMAXIMIZED){
		cx = GetSystemMetrics(SM_CXSCREEN);
	}
	else{
		cx = m_wndpl.rcNormalPosition.right - m_wndpl.rcNormalPosition.left;
	}
	nWidth = (int)((cx-240-80)/2);
	nWidth = min(nWidth,500);
	nWidth = max(150, nWidth);

	CClientDC dc(this);
	int LogPixelsY = dc.GetDeviceCaps(LOGPIXELSY);
	LOGFONT LgFt; 
	ZeroMemory(&LgFt, sizeof(LOGFONT));
	LgFt.lfHeight = - MulDiv(8, LogPixelsY, 72);
	LgFt.lfWeight = FW_NORMAL;
	strcpy(LgFt.lfFaceName,"Arial"); 
	m_FFont.CreateFontIndirect(&LgFt);

	m_XDirectBar.Create(this);
	m_XDirectBar.LoadToolBar(IDR_XFLR5BAR);
	m_XDirectBar.SetBarStyle(CBRS_TOP	| CBRS_GRIPPER | CBRS_TOOLTIPS | 
						   CBRS_FLYBY | CBRS_SIZE_DYNAMIC| CBRS_FLOAT_MULTI);

	m_XDirectBar.SetWindowText("Foil selection");


	nHeight = 600;

	m_XDirectBar.SetHeight(34);

	index = 0;
    while(m_XDirectBar.GetItemID(index)!=IDC_CBFOIL && index <100) index++;
	m_XDirectBar.SetButtonInfo(index, IDC_CBFOIL, TBBS_SEPARATOR, nWidth);
	m_XDirectBar.GetItemRect(index, &rect);
	rect.bottom = rect.top + nHeight;

	m_ctrlFoil.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_SORT,
		              rect, &m_XDirectBar, IDC_CBFOIL);
	m_ctrlFoil.SetFont(&m_FFont);

	index = 0;
    while(m_XDirectBar.GetItemID(index)!=IDC_CBPLR && index <100) index++;
	m_XDirectBar.SetButtonInfo(index, IDC_CBPLR, TBBS_SEPARATOR, nWidth);
	m_XDirectBar.GetItemRect(index, &rect);
	rect.bottom = rect.top + nHeight;
	m_ctrlPlr.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_SORT,
		              rect, &m_XDirectBar, IDC_CBPLR);
	m_ctrlPlr.SetFont(&m_FFont);

	index = 0;
    while(m_XDirectBar.GetItemID(index)!=IDC_CBOPP && index <100) index++;
	m_XDirectBar.SetButtonInfo(index, IDC_CBOPP, TBBS_SEPARATOR, 80);
	m_XDirectBar.GetItemRect(index, &rect);
	rect.bottom = rect.top + nHeight;
	m_ctrlOpp.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST ,
		              rect, &m_XDirectBar, IDC_CBOPP);
	m_ctrlOpp.SetFont(&m_FFont);

	m_ctrlFoil.EnableWindow(false);
	m_ctrlPlr.EnableWindow(false);
	m_ctrlOpp.EnableWindow(false);
}

//XOperDlgBar controls


void CMainFrame::OnXDirectPolar(CCmdUI* pCmdUI)
{
	if(m_iApp==XFOILANALYSIS && XDirect.m_pCurPolar) pCmdUI->Enable(true);
	else  pCmdUI->Enable(false);
}

void CMainFrame::OnXDirectSequence(CCmdUI* pCmdUI)
{
	if(m_iApp==XFOILANALYSIS && XDirect.m_pCurPolar && m_OperDlgBar.m_bSequence) pCmdUI->Enable(true);
	else  pCmdUI->Enable(false);
}

void CMainFrame::OnXDirectDisplay(CCmdUI* pCmdUI)
{
	if(m_iApp==XFOILANALYSIS && XDirect.m_pCurOpp) pCmdUI->Enable(true);
	else  pCmdUI->Enable(false);
}
		
void CMainFrame::OnAnimate(CCmdUI* pCmdUI)
{
	if (m_iApp==XFOILANALYSIS && m_ctrlOpp.GetCount()>1)
		pCmdUI->Enable(true);
	else if(m_iApp==MIAREX && m_ctrlWOpp.GetCount()>1 && Miarex.m_iView!=2)
		pCmdUI->Enable(true);
	else
		pCmdUI->Enable(false);
}

void CMainFrame::OnParam12(CCmdUI* pCmdUI)
{
	if (m_iApp==XFOILANALYSIS &&
		XDirect.m_pCurPolar &&
		XDirect.m_pCurPolar->m_Type !=4) pCmdUI->Enable(true);
	else  pCmdUI->Enable(false);
}

void CMainFrame::OnParam3(CCmdUI* pCmdUI)
{
	if (m_iApp==XFOILANALYSIS &&
		XDirect.m_pCurPolar &&
		XDirect.m_pCurPolar->m_Type ==4) pCmdUI->Enable(true);
	else  pCmdUI->Enable(false);
}

// end OperDlgBar controls

//PolarDlgBar controls

void CMainFrame::OnXMPolar(CCmdUI* pCmdUI)
{
	if (m_iApp ==XFOILANALYSIS){
		if ( XDirect.m_pCurPolar &&  XDirect.m_bPolar||
			(XDirect.m_pCurOpp   && !XDirect.m_bPolar))
			pCmdUI->Enable(true);
		else
			pCmdUI->Enable(false);
	}
	else if(m_iApp ==MIAREX){
		if ((Miarex.m_pCurWPolar && Miarex.m_iView==2) ||
			(Miarex.m_pCurWOpp   && Miarex.m_iView!=2)) 
			pCmdUI->Enable(true);
		else
			pCmdUI->Enable(false);
	}
}

// End PolarDlgBar controls


// ?InvCtrlBar controls

void CMainFrame::OnSpecInv(CCmdUI* pCmdUI)
{
	if (m_iApp == INVERSEDESIGN && XInverse.m_bLoaded)
		pCmdUI->Enable(true);
	else  
		pCmdUI->Enable(false);
}

void CMainFrame::OnSpecFInv(CCmdUI* pCmdUI)
{
	if (m_iApp == INVERSEDESIGN && XInverse.m_bFullInverse && XInverse.m_bLoaded)
		pCmdUI->Enable(true);
	else 
		pCmdUI->Enable(false);
}

void CMainFrame::OnSpecMInv(CCmdUI* pCmdUI)
{
	if (m_iApp == INVERSEDESIGN && !XInverse.m_bFullInverse && XInverse.m_bLoaded)
		pCmdUI->Enable(true);
	else  
		pCmdUI->Enable(false);
}

//W3DBar controls

void CMainFrame::On3D(CCmdUI* pCmdUI)
{
	if(m_iApp==MIAREX){
		if(Miarex.m_iView==3 || Miarex.m_iView==5) pCmdUI->Enable(true);
		else                                       pCmdUI->Enable(false);
	}
}


void CMainFrame::On3DAxes(CCmdUI* pCmdUI)
{
	if(Miarex.m_bAxes)   pCmdUI->SetCheck(true);
	else                 pCmdUI->SetCheck(false);
}

void CMainFrame::OnglLight(CCmdUI* pCmdUI)
{
	if(Miarex.m_bglLight)   pCmdUI->SetCheck(true);
	else					pCmdUI->SetCheck(false);
}

void CMainFrame::OnMiarexBar(CCmdUI* pCmdUI)
{
	if(m_iApp==MIAREX)
	{
		if(Miarex.m_iView==5)
		{
			if(pCmdUI->m_nID==IDT_UNDO || pCmdUI->m_nID==IDT_REDO)
			{
				pCmdUI->Enable(true);
			}			
			else pCmdUI->Enable(false);
		}
		else  
		{
			if(pCmdUI->m_nID==IDC_CBWING)
			{
				if(m_ctrlUFO.GetCount()) pCmdUI->Enable(true);
				else                   pCmdUI->Enable(false);
			}
			if(pCmdUI->m_nID==IDC_CBWPLR)
			{
				if(m_ctrlWPlr.GetCount()) pCmdUI->Enable(true);
				else                     pCmdUI->Enable(false);
			}
			if(pCmdUI->m_nID==IDC_CBWOPP)
			{
				if(m_ctrlWOpp.GetCount()) pCmdUI->Enable(true);
				else                                         pCmdUI->Enable(false);
			}
			if(pCmdUI->m_nID==IDT_UNDO || pCmdUI->m_nID==IDT_REDO)
			{
				pCmdUI->Enable(false);
			}			
		}
	}
}

void CMainFrame::OnHalfWing(CCmdUI* pCmdUI)
{
	if(m_iApp==MIAREX){
		if(Miarex.m_iView==2 || Miarex.m_iView==3 )
			pCmdUI->Enable(false);
		else if(Miarex.m_iView==1 && Miarex.m_pCurWing){
			pCmdUI->Enable(true);
		}
		else  
			pCmdUI->Enable(false);
	}
}

void CMainFrame::On3DSurfaces(CCmdUI* pCmdUI)
{
	if(Miarex.m_bSurfaces)   pCmdUI->SetCheck(true);
	else                     pCmdUI->SetCheck(false);
}

void CMainFrame::On3DOutline(CCmdUI* pCmdUI)
{
	if(Miarex.m_bOutline)   pCmdUI->SetCheck(true);
	else                    pCmdUI->SetCheck(false);
}


void CMainFrame::OnStreamLines(CCmdUI* pCmdUI)
{
	if(m_iApp==MIAREX){
		if (Miarex.m_iView==3 && Miarex.m_pCurWOpp && Miarex.m_pCurWOpp->m_AnalysisType!=1)
												  pCmdUI->Enable(true);
		else  									  pCmdUI->Enable(false);
	}
	else pCmdUI->Enable(false);
} 

void CMainFrame::OnWOpp(CCmdUI* pCmdUI)
{
	if(m_iApp==MIAREX){
		if(Miarex.m_iView==2)
			pCmdUI->Enable(false);
		else if(Miarex.m_iView==3 && Miarex.m_pCurWOpp)
			pCmdUI->Enable(true);
		else if(Miarex.m_iView!=2 && Miarex.m_pCurWOpp){
			if(pCmdUI->m_nID==IDC_DOWNWASH) pCmdUI->Enable(false);
			else                            pCmdUI->Enable(true);
		}
		else  
			pCmdUI->Enable(false);
	}
}

void CMainFrame::OnMiarexW3DOppVLM(CCmdUI* pCmdUI)
{
	if(m_iApp==MIAREX){
		if (Miarex.m_iView==3 && Miarex.m_pCurWOpp &&Miarex.m_pCurWOpp->m_AnalysisType!=1)
												  pCmdUI->Enable(true);
		else  									  pCmdUI->Enable(false);

		if (Miarex.m_iView==3 && Miarex.m_bSurfaces)		pCmdUI->SetCheck(false);
		else if (Miarex.m_iView==3 && Miarex.m_b3DVLMCl)    pCmdUI->SetCheck(true);
	}
	else pCmdUI->Enable(false);
} 



//WOperDlgBar controls

void CMainFrame::OnMiarexWPolar(CCmdUI* pCmdUI)
{
	if(m_iApp==MIAREX){
		if(Miarex.m_pCurWPolar) pCmdUI->Enable(true);
		else					pCmdUI->Enable(false);
	}
	else  pCmdUI->Enable(false);
}

void CMainFrame::OnWAMax(CCmdUI* pCmdUI)
{
	if(m_iApp == MIAREX){
		if (Miarex.m_pCurWPolar && m_WOperDlgBar.m_bSequence)	pCmdUI->Enable(true);
		else													pCmdUI->Enable(false);
	}
	else														pCmdUI->Enable(false);
}

void CMainFrame::OnInitCalc(CCmdUI* pCmdUI)
{
	if (m_iApp==MIAREX &&
		Miarex.m_pCurWPolar &&
		(Miarex.m_pCurWPolar->m_AnalysisType==1 || Miarex.m_pCurWPolar->m_bWakeRollUp))
		pCmdUI->Enable(true);
	else
		pCmdUI->Enable(false);
}

void CMainFrame::OnDAlpha(CCmdUI* pCmdUI)
{
	if(m_iApp==MIAREX){
		if (Miarex.m_pCurWPolar && m_WOperDlgBar.m_bSequence)	pCmdUI->Enable(true);
		else													pCmdUI->Enable(false);
	}
	else														pCmdUI->Enable(false);
}
//End WOperDlgBar controls
//AFoilCtrlBar controls
void CMainFrame::OnFoilColor(CCmdUI* pCmdUI)
{
	if (m_iApp==DIRECTDESIGN)pCmdUI->Enable(true);
}
void CMainFrame::OnMFC(CCmdUI* pCmdUI)
{
	if((m_iApp ==DIRECTDESIGN)) pCmdUI->Enable(true);
	else  {
		pCmdUI->Enable(false);
	}
}
void CMainFrame::OnAFoilCtrl(CCmdUI* pCmdUI)
{
	if((m_iApp ==DIRECTDESIGN && m_oaFoil.GetSize())) {
		pCmdUI->Enable(true);
	}
	else  {
		pCmdUI->Enable(false);
	}
}
//End AFoilCtrlBar controls

void CMainFrame::OnSpanPos(CCmdUI* pCmdUI)
{
	if(m_iApp ==MIAREX && Miarex.m_pCurWOpp && Miarex.m_iView==4) {
		pCmdUI->Enable(true);
	}
	else  {
		pCmdUI->Enable(false);
	}
}

//BodyCtrlBar controls

void CMainFrame::OnBodyCtrl(CCmdUI* pCmdUI)
{
	if(m_iApp ==MIAREX) 
	{
		if(m_oaBody.GetCount()) pCmdUI->Enable(true);
		else				   pCmdUI->Enable(false);
	
	}
	else  
		pCmdUI->Enable(false);
	
}
//End BodyCtrlBar controls


//Status Bar
void CMainFrame::OnIndicatorProject(CCmdUI* pCmdUI)
{
	pCmdUI->SetText(m_ProjectName);
//	Trace(m_ProjectName);
}



BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F10 )
	{
		if(m_iApp==MIAREX)	Miarex.OnBodyDesign();
		return true;
	}
	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam != VK_LCONTROL && pMsg->wParam != VK_RCONTROL) Miarex.m_bArcball = false; 

		SHORT sh1 = GetKeyState(VK_LCONTROL);
		SHORT sh2 = GetKeyState(VK_RCONTROL);
		if (pMsg->wParam == 'O' && 	((sh1 & 0x8000)||(sh2 & 0x8000))) 
		{ 
			if(m_iApp==MIAREX && (Miarex.m_iView==3 || Miarex.m_iView==5))
			{
				Miarex.m_bArcball = false;
				Miarex.UpdateView();
			}
			OnAppOpen();
			return true;
		} 
		if (pMsg->wParam == 'W' && 
			( (sh1 & 0x8000)||(sh2 & 0x8000) )) { 
			OnCloseProject();
			return true;
		} 
		if (pMsg->wParam == 'S' && 
			( (sh1 & 0x8000)||(sh2 & 0x8000) )) { 
			OnSaveProject();
			return true;
		} 
		if (pMsg->wParam == 'N' && 
			((sh1 & 0x8000)||(sh2 & 0x8000) )) { 
			OnNewProject();
			return true;
		} 
		if (pMsg->wParam == 'L' && 
			((sh1 & 0x8000)||(sh2 & 0x8000) )) { 
			OnLoadProject();
			return true;
		} 
		if (pMsg->wParam == 'I' && 
			((sh1 & 0x8000)||(sh2 & 0x8000) )) { 
			OnInsertProject();
			return true;
		} 
		if (pMsg->wParam == '1' && 
			((sh1 & 0x8000)||(sh2 & 0x8000) )) {
			if(m_iApp==DIRECTDESIGN && AFoil.m_bSF) return true;
			AFoil.m_bSF = true;
//			AFoil.SetParams(NULL);
			OnFoilDirectDesign();
			return true;
		} 

		if (pMsg->wParam == '2' && 
			((sh1 & 0x8000)||(sh2 & 0x8000) )) { 
			if(m_iApp==DIRECTDESIGN && !AFoil.m_bSF) return true;
			AFoil.m_bSF = false;
//			AFoil.SetParams(NULL);
			OnFoilDirectDesign();
			return true;
		} 

		if (pMsg->wParam == VK_F12)
			return true;
		
		switch (m_iApp){
			case XFOILANALYSIS:{
				if(XDirect.PreTranslateMessage(pMsg)) return TRUE;
				break;
			}
			case DIRECTDESIGN:{
				if(AFoil.PreTranslateMessage(pMsg)) return TRUE;
				break;
			}
			case INVERSEDESIGN:{
				if(XInverse.PreTranslateMessage(pMsg)) return TRUE;
				break;
			}
			case MIAREX:{
				if(Miarex.PreTranslateMessage(pMsg)) return TRUE;
				break;
			}
		}	
	}
	
	return CFrameWnd::PreTranslateMessage(pMsg);
}


bool CMainFrame::ReadProject(CString FileName)
{
	CFile fp;
	try{
		if (fp.Open(FileName, CFile::modeRead))
		{
			CArchive ar(&fp, CArchive::load);
			if(!SerializeProject(ar))
			{
				DeleteProject();
				return false;
			}
			ar.Close();
			fp.Close();
			return true;
		}
		else return false;
	}
	catch (CException *ex){
		TCHAR   szCause[255];
		CString str;
		ex->GetErrorMessage(szCause, 255);
		str = _T("Error encountered while loading project ");
		str += szCause;
		AfxMessageBox(str);
		ex->Delete();
		return false;
	}
}
void CMainFrame::OnSaveProjectAs() 
{
	SaveProjectAs();
	UpdateView();
}


bool CMainFrame::SaveProjectAs()
{
	CFile fp;
	CString FileName = m_ProjectName;

	int len = FileName.GetLength();
	CString str = FileName.Right(1);
	if(FileName.Right(1)=="*"){
		FileName = FileName.Left(len-1);
	}

	FileName.Replace("/", " ");

	CFileDialog WProjectDlg(false, "wpa", FileName, OFN_OVERWRITEPROMPT, _T("XFLR5 Project (.wpa)|*.wpa|"));
	if(IDOK==WProjectDlg.DoModal()){
		FileName = WProjectDlg.GetFileName();

		if(SaveProject(FileName)){
			m_FileName    = WProjectDlg.GetPathName();
			SetProjectName(m_FileName);
			AddRecentFile(m_FileName);
			m_wndStatusBar.SetWindowText("The project " + m_ProjectName + " has been saved");
			SetSaveState(true);	
			return true;
		}
		else return false;
	}
	else return false;
}


bool CMainFrame::SaveProject(CString FileName)
{
	CWaitCursor wait;
	CFileException fe;
	CString strong;
	CFile fp;
	BOOL bOpen = fp.Open(FileName,  CFile::modeCreate | CFile::modeWrite, &fe);
	try{
		if (bOpen) {
			CArchive ar(&fp, CArchive::store);
			SerializeProject(ar);
			ar.Close();
			fp.Close();
			return true;
		}
		else{
//			pfe->m_strFileName = FileName;
			throw &fe;
		}
	}
	catch (CFileException *ex){
		TCHAR   szCause[255];
		CString str;
		ex->GetErrorMessage(szCause, 255);
		str = _T("Error saving project : ");
		str += szCause;
		AfxMessageBox(str);
		return false;
	}
}
bool CMainFrame::SaveBodyProject(CBody *pBody)
{
	CString strong;
	if(!pBody) 		AfxMessageBox("Nothing to save", MB_OK);

	CFileException fe;
	CFile fp;
	CFileDialog XFileDlg(false, "wpa", strong, OFN_OVERWRITEPROMPT, _T("XFLR5 file (.wpa)|*.wpa|"));

	if(IDOK==XFileDlg.DoModal()) {

		BOOL bOpen = fp.Open(XFileDlg.GetFileName(),  CFile::modeCreate | CFile::modeWrite, &fe);
		try{
			if (bOpen) {
				CArchive ar(&fp, CArchive::store);
				CWaitCursor Wait;

				CString strong;
				if (ar.IsStoring())
				{	// storing code
					ar << 100011;					// 100011 : Added Body serialization
					ar << m_LengthUnit;
					ar << m_AreaUnit;
					ar << m_WeightUnit;
					ar << m_SpeedUnit;
					ar << m_ForceUnit;
					ar << m_MomentUnit;
					ar << Miarex.m_WngAnalysis.m_Type;
					ar << (float)Miarex.m_WngAnalysis.m_Weight;
					ar << (float)Miarex.m_WngAnalysis.m_QInf;
					ar << (float)Miarex.m_WngAnalysis.m_XCmRef;
					ar << (float)Miarex.m_WngAnalysis.m_Density;
					ar << (float)Miarex.m_WngAnalysis.m_Viscosity;
					ar << (float)Miarex.m_WngAnalysis.m_Alpha;
					ar << Miarex.m_WngAnalysis.m_AnalysisType;

					if (Miarex.m_WngAnalysis.m_bVLM1)   ar << 1;
					else								ar << 0;
					ar <<1;//		if (Miarex.m_WngAnalysis.m_bMiddle) ar << 1; else ar << 0;

					if (Miarex.m_WngAnalysis.m_bTiltedGeom) ar << 1;
					else									ar << 0;
					if (Miarex.m_WngAnalysis.m_bWakeRollUp) ar << 1;
					else									ar << 0;
					ar << 0; //number of wings
					ar << 0; // store all the WPolars
					ar << 0; // next store all the WOpps
				
					ar << 100001;//unused 
					ar << 0;//write the number of foils
					
					ar << 0;//then write polars
					ar << 0;//no Opps... keep it simple

					if(pBody)
					{
						ar << 1;
						pBody->SerializeBody(ar);
					}
					else ar<<0;

					ar << 0; // last write the planes...
					ar << 0; // not forgetting their POpps


					AFoil.m_pSF->Serialize(ar);
					AFoil.m_pPF->Serialize(ar);

				}
				ar.Close();
				fp.Close();

				return true;
			}
			else{
				throw &fe;
			}
		}
		catch (CFileException *ex){
			TCHAR   szCause[255];
			CString str;
			ex->GetErrorMessage(szCause, 255);
			str = _T("Error saving project : ");
			str += szCause;
			AfxMessageBox(str);
			return false;
		}
	}
	else return false;
}

bool CMainFrame::SaveUFOProject()
{
	CString strong;
	if(Miarex.m_pCurPlane) strong = Miarex.m_pCurPlane->m_PlaneName;
	else if(Miarex.m_pCurWing) strong = Miarex.m_pCurWing->m_WingName;
	else {
		AfxMessageBox("Nothing to save", MB_OK);
		return false;
	}

	CFileException fe;
	CFile fp;
//	CString FileName;
	CFileDialog XFileDlg(false, "wpa", strong, OFN_OVERWRITEPROMPT, _T("XFLR5 file (.wpa)|*.wpa|"));

	if(IDOK==XFileDlg.DoModal()) {

		BOOL bOpen = fp.Open(XFileDlg.GetFileName(),  CFile::modeCreate | CFile::modeWrite, &fe);
		try{
			if (bOpen) {
				CArchive ar(&fp, CArchive::store);
				SerializeUFOProject(ar, strong);
				ar.Close();
				fp.Close();
				return true;
			}
			else{
	//			pfe->m_strFileName = FileName;
				throw &fe;
			}
		}
		catch (CFileException *ex){
			TCHAR   szCause[255];
			CString str;
			ex->GetErrorMessage(szCause, 255);
			str = _T("Error saving project : ");
			str += szCause;
			AfxMessageBox(str);
			return false;
		}
	}
	else return false;
}
/*
*/
bool CMainFrame::SerializeUFOProject(CArchive &ar, CString UFOName)
{
	CWaitCursor Wait;

	//saves only the UFO with its associated plrs, foils and other results

	CPlane *pPlane   = Miarex.m_pCurPlane;
	CWing *pWing     = Miarex.m_pCurWing;
	CWing *pStab     = Miarex.m_pCurStab;
	CWing *pFin      = Miarex.m_pCurFin;
	CWPolar *pWPolar = NULL;
	CWOpp *pWOpp     = NULL;
	CFoil *pFoil  = NULL;
	CPolar * pPolar  = NULL;

	int i,j;
	CString strong;

	// storing code
	ar << 100011;
	// 100011 : Added Body serialization
	// 100010 : Converted to I.S. units
	// 100009 : added serialization of opps in numbered format
	// 100008 : Added m_WngAnalysis.m_bTiltedGeom, m_WngAnalysis.m_bWakeRollUp
	// 100006 : version 2.99Beta001 format
	// 100005 : version 2.00 format 
	//Archive format introduced in Beta V22
	// Write units //added in Beta v16
	// Save analysis settings // added in Beta v22
	ar << m_LengthUnit;
	ar << m_AreaUnit;
	ar << m_WeightUnit;
	ar << m_SpeedUnit;
	ar << m_ForceUnit;
	ar << m_MomentUnit;
	ar << Miarex.m_WngAnalysis.m_Type;
	ar << (float)Miarex.m_WngAnalysis.m_Weight;
	ar << (float)Miarex.m_WngAnalysis.m_QInf;
	ar << (float)Miarex.m_WngAnalysis.m_XCmRef;
	ar << (float)Miarex.m_WngAnalysis.m_Density;
	ar << (float)Miarex.m_WngAnalysis.m_Viscosity;
	ar << (float)Miarex.m_WngAnalysis.m_Alpha;
	ar << Miarex.m_WngAnalysis.m_AnalysisType;

	if (Miarex.m_WngAnalysis.m_bVLM1)   ar << 1;
	else								ar << 0;
//		if (Miarex.m_WngAnalysis.m_bMiddle) ar << 1; else ar << 0;
	ar <<1;
	if (Miarex.m_WngAnalysis.m_bTiltedGeom) ar << 1;
	else									ar << 0;
	if (Miarex.m_WngAnalysis.m_bWakeRollUp) ar << 1;
	else									ar << 0;

	// Store the wing, if any
	if(!pPlane){
		ar << 1;//just one wing
		pWing->SerializeWing(ar);
	}
	else ar <<0;

	// now store all the WPolars associated to the UFO
	//start by listing the number
	int size = 0;
	for (i=0; i<m_oaWPolar.GetSize();i++){
		pWPolar = (CWPolar*)m_oaWPolar.GetAt(i);
		if(pWPolar->m_UFOName == UFOName) size++;
	}

	ar << size;
	for (i=0; i<m_oaWPolar.GetSize();i++){
		pWPolar = (CWPolar*)m_oaWPolar.GetAt(i);
		pWPolar->m_pParent = this;
		if(pWPolar->m_UFOName == UFOName) pWPolar->SerializeWPlr(ar);
	}

	ar << 0;//no WOpps

	// then the foils,  polars and Opps
	//list the foils to be saved
	CStringArray FoilList;
	bool bFound = false;
	if(pWing){
		for (i=0; i<=pWing->m_NPanel; i++){
			pFoil = GetFoil(pWing->m_RFoil[i]);
			bFound = false;
			for(j=0;j<FoilList.GetSize();j++){
				if(pFoil->m_FoilName == FoilList.GetAt(j)){
					bFound = true;
					break;
				}
			}
			if(!bFound) FoilList.Add(pFoil->m_FoilName);
		}
		if(!pWing->m_bSymetric){
			for (i=0; i<=pWing->m_NPanel; i++){
				pFoil = GetFoil(pWing->m_LFoil[i]);
				bFound = false;
				for(j=0;j<FoilList.GetSize();j++){
					if(pFoil->m_FoilName == FoilList.GetAt(j)){
						bFound = true;
						break;
					}
				}
				if(!bFound) FoilList.Add(pFoil->m_FoilName);
			}
		}
	}
	if(pStab){
		for (i=0; i<=pStab->m_NPanel; i++){
			pFoil = GetFoil(pStab->m_RFoil[i]);
			bFound = false;
			for(j=0;j<FoilList.GetSize();j++){
				if(pFoil->m_FoilName == FoilList.GetAt(j)){
					bFound = true;
					break;
				}
			}
			if(!bFound) FoilList.Add(pFoil->m_FoilName);
		}
		if(!pStab->m_bSymetric){
			for (i=0; i<=pStab->m_NPanel; i++){
				pFoil = GetFoil(pStab->m_LFoil[i]);
				bFound = false;
				for(j=0;j<FoilList.GetSize();j++){
					if(pFoil->m_FoilName == FoilList.GetAt(j)){
						bFound = true;
						break;
					}
				}
				if(!bFound) FoilList.Add(pFoil->m_FoilName);
			}
		}
	}
	if(pFin){
		for (i=0; i<=pFin->m_NPanel; i++){
			pFoil = GetFoil(pFin->m_RFoil[i]);
			bFound = false;
			for(j=0;j<FoilList.GetSize();j++){
				if(pFoil->m_FoilName == FoilList.GetAt(j)){
					bFound = true;
					break;
				}
			}
			if(!bFound) FoilList.Add(pFoil->m_FoilName);
		}
		if(!pFin->m_bSymetric){
			for (i=0; i<=pFin->m_NPanel; i++){
				pFoil = GetFoil(pFin->m_LFoil[i]);
				bFound = false;
				for(j=0;j<FoilList.GetSize();j++){
					if(pFoil->m_FoilName == FoilList.GetAt(j)){
						bFound = true;
						break;
					}
				}
				if(!bFound) FoilList.Add(pFoil->m_FoilName);
			}
		}
	}

	//write the number of foils
	ar << 100001;//unused 
	ar << (int)FoilList.GetSize();
	for (i=0; i<(int)FoilList.GetSize(); i++){
		pFoil = GetFoil(FoilList[i]);
		pFoil->Serialize(ar);
	}
	int n = 0;
	for (i=0; i<(int)FoilList.GetSize(); i++){
		pFoil = GetFoil(FoilList[i]);
		//count polars associated to the foil
		for (int i=0; i<m_oaPolar.GetSize();i++){
			pPolar = (CPolar*)m_oaPolar.GetAt(i);
			if (pPolar->m_FoilName == pFoil->m_FoilName) 
				n++;
		}
	}
	//then write polars
	ar << n;
	for (i=0; i<FoilList.GetSize(); i++){
		pFoil = GetFoil(FoilList[i]);
		for (j=0; j<m_oaPolar.GetSize();j++){
			pPolar = (CPolar*)m_oaPolar.GetAt(j);
			pPolar->m_pFrame = this;
			if (pPolar->m_FoilName == pFoil->m_FoilName) {
				pPolar->Serialize(ar);
			}
		}
	}

	ar<<0;//no Opps

	// next the bodies

	if(pPlane && pPlane->m_bBody && pPlane->m_pBody)
	{
		ar << 1;
		pPlane->m_pBody->SerializeBody(ar);
	}
	else ar <<0; //no plane

	// last write the plane...

	if(pPlane) 
	{
		ar <<1;
		pPlane->SerializePlane(ar);
	}
	else ar <<0;

	ar << 0;//no POpps

	AFoil.m_pSF->Serialize(ar);
	AFoil.m_pPF->Serialize(ar);

	return true;
}


bool CMainFrame::SerializeProject(CArchive &ar)
{
	CWaitCursor Wait;

	CWing *pWing     = NULL;
	CWPolar *pWPolar = NULL;
	CWPolar *pPPolar = NULL;
	CWOpp *pWOpp     = NULL;
	CPOpp *pPOpp     = NULL;
	CPlane *pPlane   = NULL;
	CBody *pBody     = NULL;;

	int i;
	CString strong;
	if (ar.IsStoring())
	{	
		// storing code
		ar << 100011;
		// 100011 : Added Body serialization
		// 100010 : Converted to I.S. units
		// 100009 : added serialization of opps in numbered format
		// 100008 : Added m_WngAnalysis.m_bTiltedGeom, m_WngAnalysis.m_bWakeRollUp
		// 100006 : version 2.99Beta001 format
		// 100005 : version 2.00 format 
		//Archive format introduced in Beta V22
		// Write units //added in Beta v16
		// Save analysis settings // added in Beta v22
		ar << m_LengthUnit;
		ar << m_AreaUnit;
		ar << m_WeightUnit;
		ar << m_SpeedUnit;
		ar << m_ForceUnit;
		ar << m_MomentUnit;
		ar << Miarex.m_WngAnalysis.m_Type;
		ar << (float)Miarex.m_WngAnalysis.m_Weight;
		ar << (float)Miarex.m_WngAnalysis.m_QInf;
		ar << (float)Miarex.m_WngAnalysis.m_XCmRef;
		ar << (float)Miarex.m_WngAnalysis.m_Density;
		ar << (float)Miarex.m_WngAnalysis.m_Viscosity;
		ar << (float)Miarex.m_WngAnalysis.m_Alpha;
		ar << Miarex.m_WngAnalysis.m_AnalysisType;

		if (Miarex.m_WngAnalysis.m_bVLM1)   ar << 1;
		else								ar << 0;
//		if (Miarex.m_WngAnalysis.m_bMiddle) ar << 1; else ar << 0;
		ar <<1;
		if (Miarex.m_WngAnalysis.m_bTiltedGeom) ar << 1;
		else									ar << 0;
		if (Miarex.m_WngAnalysis.m_bWakeRollUp) ar << 1;
		else									ar << 0;

		ar << (int)m_oaWing.GetSize() ;//number of wings
		// Store the wings
		for (i=0; i<m_oaWing.GetSize();i++){
			pWing = (CWing*)m_oaWing.GetAt(i);
			pWing->SerializeWing(ar);
		}

		// now store all the WPolars
		ar << (int)m_oaWPolar.GetSize();
		for (i=0; i<m_oaWPolar.GetSize();i++){
			pWPolar = (CWPolar*)m_oaWPolar.GetAt(i);
			pWPolar->m_pParent = this;
			pWPolar->SerializeWPlr(ar);
		}

		// next store all the WOpps
		if(m_bSaveWOpps){
			ar << (int)m_oaWOpp.GetSize();
			for (i=0; i<m_oaWOpp.GetSize();i++){
				pWOpp = (CWOpp*)m_oaWOpp.GetAt(i);
				pWOpp->SerializeWOpp(ar);
			}
		}
		else ar << 0;

		// then the foils,  polars and Opps
		WritePolars(ar);

		// next the bodies
		ar << (int)m_oaBody.GetSize();
		for (i=0; i<m_oaBody.GetSize();i++)
		{
			pBody = (CBody*)m_oaBody.GetAt(i);
			pBody->SerializeBody(ar);
		}

		// last write the planes...
		ar << (int)m_oaPlane.GetSize();
		for (i=0; i<m_oaPlane.GetSize();i++)
		{
			pPlane = (CPlane*)m_oaPlane.GetAt(i);
			pPlane->SerializePlane(ar);
		}

		if(m_bSaveWOpps){
			// not forgetting their POpps
			ar << (int)m_oaPOpp.GetSize();
			for (i=0; i<m_oaPOpp.GetSize();i++){
				pPOpp = (CPOpp*)m_oaPOpp.GetAt(i);
				pPOpp->SerializePOpp(ar);
			}
		}
		else ar << 0;

		AFoil.m_pSF->Serialize(ar);
		AFoil.m_pPF->Serialize(ar);

		return true;
	}
	else {	// LOADING CODE
		int n, k;
		try{
			int ArchiveFormat;
			ar >> n;
			if(n<100000){
			// then n is the number of wings to load
			// up to v1.99beta15
				ArchiveFormat = 100000;
			}
			else{
				float f;
			// then n is the ArchiveFormat number
				ArchiveFormat = n;
				ar >> m_LengthUnit;
				ar >> m_AreaUnit;
				ar >> m_WeightUnit;
				ar >> m_SpeedUnit;
				ar >> m_ForceUnit;
				if(ArchiveFormat>=100005){
					ar >> m_MomentUnit;
				}
				SetUnits(m_LengthUnit, m_AreaUnit, m_SpeedUnit, m_WeightUnit, m_ForceUnit, m_MomentUnit,
						 m_mtoUnit, m_m2toUnit, m_mstoUnit, m_kgtoUnit, m_NtoUnit, m_NmtoUnit);

				if(ArchiveFormat>=100004){
					ar >> Miarex.m_WngAnalysis.m_Type;
					ar >> f; Miarex.m_WngAnalysis.m_Weight=f;
					ar >> f; Miarex.m_WngAnalysis.m_QInf=f;
					ar >> f; Miarex.m_WngAnalysis.m_XCmRef=f;
					if(ArchiveFormat<100010) Miarex.m_WngAnalysis.m_XCmRef=f/1000.0;
					ar >> f; Miarex.m_WngAnalysis.m_Density=f;
					ar >> f; Miarex.m_WngAnalysis.m_Viscosity=f;
					ar >> f; Miarex.m_WngAnalysis.m_Alpha=f;

					ar >> Miarex.m_WngAnalysis.m_AnalysisType;

				}
				if(ArchiveFormat>=100006){
					ar >> k;
					if (k) Miarex.m_WngAnalysis.m_bVLM1 = true;
					else   Miarex.m_WngAnalysis.m_bVLM1 = false;

					ar >> k;
//					if (k) Miarex.m_WngAnalysis.m_bMiddle = true;
//					else   Miarex.m_WngAnalysis.m_bMiddle = false;
				}
			
				if(ArchiveFormat>=100008){
					ar >> k;
					if (k) Miarex.m_WngAnalysis.m_bTiltedGeom = true;
					else   Miarex.m_WngAnalysis.m_bTiltedGeom = false;

					ar >> k;
					if (k) Miarex.m_WngAnalysis.m_bWakeRollUp = true;
					else   Miarex.m_WngAnalysis.m_bWakeRollUp = false;
				}
				// and read n again
				ar >> n;
			}
			
			// WINGS FIRST

			for (i=0;i<n; i++)
			{
				pWing = new CWing();
								
				if (!pWing->SerializeWing(ar))
				{
					CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
					pfe->m_strFileName = ar.m_strFileName;
					if(pWing) delete pWing;
					throw pfe;
				}
				if(pWing){
//					pWing->ComputeGeometry();
					pWing = Miarex.AddWing(pWing);
				}
				else{
					CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
					pfe->m_strFileName = ar.m_strFileName;
					if(pWing) delete pWing;
					throw pfe;
				}
			}

			//THEN WPOLARS
			ar >> n;// number of WPolars to load
			for (i=0;i<n; i++)
			{
				pWPolar = new CWPolar(this);
				
				if (!pWPolar->SerializeWPlr(ar)){
					CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
					pfe->m_strFileName = ar.m_strFileName;
					if(pWPolar) 
						delete pWPolar;
					throw pfe;
				}
				if(!pWPolar->m_AnalysisType==1 && ArchiveFormat <100003) 
					pWPolar->ResetWPlr();//former VLM version was flawed
				pWPolar = Miarex.AddWPolar(pWPolar);
			}

			//THEN WOPPS
			ar >> n;// number of WOpps to load
			for (i=0;i<n; i++){
				pWOpp = new CWOpp();
				bool bWOppOK;
				if (ArchiveFormat<=100001){
					CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
					AfxMessageBox("Old format, not supported any more");
					throw pfe;
				}
				else {
					bWOppOK = pWOpp->SerializeWOpp(ar);
					if(pWOpp && bWOppOK){
						pWing = Miarex.GetWing(pWOpp->m_WingName);
						if(pWing) pWOpp->m_MAChord = pWing->m_MAChord;
						else bWOppOK = false;
					}
				}
				if (!bWOppOK){
					CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
					pfe->m_strFileName = ar.m_strFileName;
					if(pWOpp) 
						delete pWOpp;
					throw pfe;
				}
				if(pWOpp->m_AnalysisType==1 || ArchiveFormat >=100006)//former VLM version was flawed
					Miarex.InsertWOpp(pWOpp);
				else delete pWOpp;
			}

			ar >> n;
			//=100000 ... unused

			//THEN FOILS, POLARS and OPPS
			if(ArchiveFormat>=100009){
				if(!LoadPolarFileV3(ar,100002)){
					CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
					pfe->m_strFileName = ar.m_strFileName;
					throw pfe;
					return false;
				}
			}
			else {
				if(ArchiveFormat>=100006){
					if(!LoadPolarFileV3(ar)){
						CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
						pfe->m_strFileName = ar.m_strFileName;
						throw pfe;
						return false;
					}
				}
				else {
					if(!LoadPolarFile(ar)){
						CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
						pfe->m_strFileName = ar.m_strFileName;
						throw pfe;
						return false;
					}
				}
			}

			if(n==100000){
				CPolar * pPolar;
				CFoil *pFoil;
				CString str;
				for (int j=0; j<m_oaPolar.GetSize(); j++){
					pPolar = (CPolar*)m_oaPolar.GetAt(j);
					for (int k=0; k<m_oaFoil.GetSize(); k++){
						pFoil = (CFoil*)m_oaFoil.GetAt(k);
						if(pFoil->m_FoilName==pPolar->m_FoilName){
							pPolar->m_ACrit    = 9.0;
							pPolar->m_XTop     = 1.0;
							pPolar->m_XBot     = 1.0;
							str.Format("_N%.1f", 9.0);
							pPolar->m_PlrName += str;
							break;
						}
					}
				}
			} 

			if(ArchiveFormat>=100011)
			{
				ar >> n;// number of Bodies to load
				for (i=0;i<n; i++){
					pBody = new CBody();
					
					if (!pBody->SerializeBody(ar)){
						CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
						pfe->m_strFileName = ar.m_strFileName;
						if(pPOpp) delete pPOpp;
						throw pfe;
					}
					Miarex.AddBody(pBody);
				}
			}

			if(ArchiveFormat>=100006){ //read the planes
				ar >> n;
				// last read the planes
				for (i=0; i<n;i++){
					pPlane = Miarex.CreatePlane();
					if(pPlane){
						if(pPlane->SerializePlane(ar))		Miarex.AddPlane(pPlane);
						else
						{
							delete pPlane;
							CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
							pfe->m_strFileName = ar.m_strFileName;
							if(pWPolar) delete pWPolar;
							throw pfe;

						}
					}
				}
				//and their pPolars
				
				if(ArchiveFormat <100007){
					ar >> n;// number of PPolars to load
					for (i=0;i<n; i++){
						pWPolar = new CWPolar(this);
						
						if (!pWPolar->SerializeWPlr(ar)){
							CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
							pfe->m_strFileName = ar.m_strFileName;
							if(pWPolar) delete pWPolar;
							throw pfe;
						}
						if(!pWPolar->m_AnalysisType==1 && ArchiveFormat <100003) 
							pWPolar->ResetWPlr();
						Miarex.AddWPolar(pWPolar);
					}
				}
				ar >> n;// number of POpps to load
				for (i=0;i<n; i++){
					pPOpp = new CPOpp();
					
					if (!pPOpp->SerializePOpp(ar)){
						CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
						pfe->m_strFileName = ar.m_strFileName;
						if(pPOpp) delete pPOpp;
						throw pfe;
					}
					Miarex.AddPOpp(false, NULL, NULL, NULL, pPOpp);
				}
				//lock all bodies with results
				for(i=0;i<m_oaWPolar.GetSize(); i++)
				{
					pWPolar = (CWPolar*)m_oaWPolar.GetAt(i);
					pPlane  = Miarex.GetPlane(pWPolar->m_UFOName);
					if(pPlane && pWPolar->m_Alpha.GetSize())
					{
						if(pPlane->m_bBody && pPlane->m_pBody) 
							pPlane->m_pBody->m_bLocked = true;
					}
				}
			}

			Miarex.m_pCurPOpp = NULL;

			AFoil.m_pSF->Serialize(ar);
			AFoil.m_pPF->Serialize(ar);

			for (i=0; i<m_oaWing.GetSize();i++)
			{
				pWing = (CWing*)m_oaWing[i];
				pWing->ComputeGeometry();
			}

			return true;
		}
		catch (CArchiveException *ex)
		{
			TCHAR   szCause[255];
			CString str;
			ex->GetErrorMessage(szCause, 255);
			str = _T("Error loading Project : ");
			str += szCause;
			AfxMessageBox(str);
			ex->Delete();
			return false;
		}
	}
}



int CMainFrame::LoadFile(CString FileName, CString PathName)
{
	if(!FileName.GetLength()) return 0;

	CString end;
	end = PathName.Right(4);
	end.MakeLower();

	if(end==".plr"){
		ReadPolarFile(PathName);
		XDirect.m_pCurPolar = NULL;
		XDirect.m_pCurOpp   = NULL;
		if(m_oaPolar.GetSize()){
			XDirect.m_bPolar = true;
			XDirect.CreatePolarCurves();
		}
		SetSaveState(false);
		return XFOILANALYSIS;
	}
	else {

		if(end==".dat"){
			CFoil *pFoil = ReadFoilFile(PathName);
			if(pFoil){
				m_pCurFoil = pFoil;
				XDirect.m_pCurFoil = pFoil;
				XDirect.m_pCurPolar = NULL;
				XDirect.m_pCurOpp   = NULL;
				AFoil.SetFoils(m_pCurFoil);
				SetSaveState(false);
				return XFOILANALYSIS;
			}
		}
		else{

			if(end==".wpa"){
				if(!m_bSaved){
					int resp = AfxMessageBox("Save the current project ?", MB_YESNOCANCEL);
					if (IDCANCEL == resp) return -1;
					if (IDYES == resp) OnSaveProject();
				}

				DeleteProject();
				SetProjectName(PathName);
				if(ReadProject(PathName)){
					SetSaveState(true);
					AddRecentFile(PathName);
					if(m_oaWing.GetSize() || m_oaPlane.GetSize())	return MIAREX;
					else if (m_oaFoil.GetSize())                    return XFOILANALYSIS;
					else return 0;
				}
			}
		}
	}
	return -1;
}

BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	CString FileName;
	if (pCopyDataStruct->dwData == XFLR5_DATA) {
		
		FileName = (LPCTSTR)pCopyDataStruct->lpData;
		FileName = FileName.Left(pCopyDataStruct->cbData);
		
		if(LoadFile(FileName, FileName)>0) {
			
			int pos1 = FileName.Find(".plr");
			int pos2 = FileName.Find(".PLR");
			if (pos1>0 || pos2>0) {
				OnXDirect();
			}
			else {
				pos1 = FileName.Find(".dat");
				pos2 = FileName.Find(".DAT");
				if(pos1>0 || pos2>0) {
					OnXDirect();
				}
				else{
					pos1 = FileName.Find(".wpa");
					pos2 = FileName.Find(".WPA");
					if(pos1>0 || pos2>0) {
						AddRecentFile(FileName);
						if(m_iApp==MIAREX)
						{
							UpdateUFOs();
							Miarex.SetUFO();
							Miarex.SetBody();
							OnMiarex();
						}
						else if(m_iApp==XFOILANALYSIS) {
							UpdateFoils();
							XDirect.SetFoil();
							OnXDirect();
						}
						else if(m_oaWing.GetSize() || m_oaPlane.GetSize())
						{
							UpdateUFOs();
							Miarex.SetUFO();
							Miarex.SetBody();
							OnMiarex();
						}
						else if(m_oaFoil.GetSize()) {
							UpdateFoils();
							XDirect.SetFoil();
							OnXDirect();
						}
					}
				}
			}
		}
	}
	return TRUE;
}


void CMainFrame::SavePolars(CString FileName, CFoil *pFoil)
{
	CFile fp;
	if (fp.Open(FileName, CFile::modeCreate | CFile::modeWrite)) {
		CArchive ar(&fp, CArchive::store);
		WritePolars(ar, pFoil);
		ar.Close();
		fp.Close();
	}
//	XDirect.m_bSaved = true;
}

void CMainFrame::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	//point is in screen coordinates...
	// so convert to client and send to app
	CPoint CltPt = point;
	ScreenToClient(&CltPt);

	CMenu menu;
	if (menu.LoadMenu(IDR_CTXMAINMENU)){
		CMenu* pPopup = menu.GetSubMenu(0);
		if(m_wndStatusBar.IsWindowVisible()){
			menu.CheckMenuItem(ID_VIEW_STATUS_BAR, MF_BYCOMMAND | MF_CHECKED);
		}
		
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			CltPt.x, CltPt.y, this); // use main window for cmds
	}
}

void CMainFrame::OnUnits() 
{
	CUnitsDlg Dlg;

	Dlg.m_Length    = m_LengthUnit;
	Dlg.m_Area      = m_AreaUnit;
	Dlg.m_Speed     = m_SpeedUnit;
	Dlg.m_Weight    = m_WeightUnit;
	Dlg.m_Force     = m_ForceUnit;
	Dlg.m_Moment    = m_MomentUnit;

	if(Dlg.DoModal()==IDOK){
		m_LengthUnit  = Dlg.m_Length;
		m_AreaUnit    = Dlg.m_Area;
		m_SpeedUnit   = Dlg.m_Speed;
		m_WeightUnit  = Dlg.m_Weight;
		m_ForceUnit   = Dlg.m_Force;
		m_MomentUnit  = Dlg.m_Moment;

		SetUnits(m_LengthUnit, m_AreaUnit, m_SpeedUnit, m_WeightUnit, m_ForceUnit, m_MomentUnit,
				 m_mtoUnit, m_m2toUnit, m_mstoUnit, m_kgtoUnit, m_NtoUnit, m_NmtoUnit);

		SetSaveState(false);

		if(m_iApp==MIAREX){
			Miarex.UpdateUnits();
		}
	}
}

void CMainFrame::OnNewProject() 
{
	if(!m_bSaved){
		int resp = AfxMessageBox("Save the current project ?", MB_YESNOCANCEL);
		if (IDCANCEL == resp) return;
		if (IDYES == resp) OnSaveProject();
	}
	DeleteProject();
	m_wndView.Invalidate();
	SetProjectName("");
}

void CMainFrame::OnCloseProject() 
{
	if(!m_bSaved){
		int resp = AfxMessageBox("Save the current project ?", MB_YESNOCANCEL);
		if (IDCANCEL == resp) return;
		if (IDYES == resp) OnSaveProject();
	}
	DeleteProject();
	
	m_wndView.Invalidate();
}


void CMainFrame::OnInsertProject() 
{
	CFile fp;
	CString strong;

	//load new project
	CFileDialog WPlrDlg(true, "wpa", NULL, OFN_HIDEREADONLY, _T("XFLR5 Project (.wpa)|*.wpa|"));
	if(IDOK==WPlrDlg.DoModal()){
		CString FileName = WPlrDlg.GetFileName();
		ReadProject(FileName);
		if(m_iApp == MIAREX)
		{
			UpdateUFOs();
			Miarex.SetUFO();

			if(Miarex.m_iView==2)      Miarex.CreateWPolarCurves();
			else if(Miarex.m_iView==1) Miarex.CreateWOppCurves();
			else if(Miarex.m_iView==4) Miarex.CreateCpCurves();
			else if(Miarex.m_iView==5)
			{
				m_BodyCtrlBar.UpdateBodies();
				Miarex.SetBody();
			}
		}
		else if(m_iApp == XFOILANALYSIS){
			if(XDirect.m_bPolar) XDirect.CreatePolarCurves();
			else			   XDirect.CreateOppCurves();
			UpdateFoils();
		}
		else if(m_iApp == DIRECTDESIGN){
			AFoil.SetFoils();
		}
		m_wndView.Invalidate();
	}	
}


void CMainFrame::OnSaveProject() 
{
	if (!m_ProjectName.GetLength() || m_ProjectName=="*") {
		OnSaveProjectAs();
		return; 
	}
	if(SaveProject(m_FileName)){
		SetSaveState(true);	
		m_wndStatusBar.SetWindowText("The project " + m_ProjectName + " has been saved");
	}
}


void CMainFrame::DeleteProject()
{
	// clear everything
	int i;
	CObject *pObj;

	for (i=(int)m_oaPlane.GetSize()-1; i>=0; i--){
		pObj = m_oaPlane.GetAt(i);
		m_oaPlane.RemoveAt(i);
		delete pObj;
	}
	for (i=(int)m_oaPOpp.GetSize()-1; i>=0; i--){
		pObj = m_oaPOpp.GetAt(i);
		m_oaPOpp.RemoveAt(i);
		delete pObj;
	}
	for (i=(int)m_oaWing.GetSize()-1; i>=0; i--){
		pObj = m_oaWing.GetAt(i);
		m_oaWing.RemoveAt(i);
		delete pObj;
	}
	for (i=(int)m_oaWPolar.GetSize()-1; i>=0; i--){
		pObj = m_oaWPolar.GetAt(i);
		m_oaWPolar.RemoveAt(i);
		delete pObj;
	}
	for (i=(int)m_oaWOpp.GetSize()-1; i>=0; i--){
		pObj = m_oaWOpp.GetAt(i);
		m_oaWOpp.RemoveAt(i);
		delete pObj;
	}
	for (i=(int)m_oaFoil.GetSize()-1; i>=0; i--){
		pObj = m_oaFoil.GetAt(i);
		m_oaFoil.RemoveAt(i);
		delete pObj;
	}
	for (i=(int)m_oaPolar.GetSize()-1; i>=0; i--){
		pObj = m_oaPolar.GetAt(i);
		m_oaPolar.RemoveAt(i);
		delete pObj;
	}
	for (i=(int)m_oaOpp.GetSize()-1; i>=0; i--){
		pObj = m_oaOpp.GetAt(i);
		m_oaOpp.RemoveAt(i);
		delete pObj;
	}
	for (i=(int)m_oaBody.GetSize()-1; i>=0; i--){
		pObj = m_oaBody.GetAt(i);
		m_oaBody.RemoveAt(i);
		delete pObj;
	}

	Miarex.m_pCurPlane  = NULL;
	Miarex.m_pCurPOpp   = NULL;
	Miarex.m_pCurWing   = NULL;
	Miarex.m_pCurWPolar = NULL;
	Miarex.m_pCurWOpp   = NULL;
	Miarex.m_pCurBody   = NULL;
	Miarex.m_pCurFrame  = NULL;
	Miarex.CreateWPolarCurves();
	Miarex.CreateWOppCurves();
	Miarex.CreateCpCurves();

	XDirect.m_pXFoil->m_FoilName = "";
	XDirect.m_pCurFoil  = NULL;
	XDirect.m_pCurPolar = NULL;
	XDirect.m_pCurOpp   = NULL;
	XDirect.SetFoil();

	if(m_iApp == MIAREX)
	{
		UpdateUFOs();
		Miarex.SetUFO();
		Miarex.SetBody();
	}
	else if (m_iApp==XFOILANALYSIS) UpdateFoils();

	m_AFoilCtrlBar.m_pRefFoil = NULL;
	if(m_iApp==DIRECTDESIGN) 
{
		m_AFoilCtrlBar.FillFoilList();
		m_AFoilCtrlBar.SelectFoil();
	}

	XInverse.Clear();
	SetProjectName("");
	SetSaveState(true);
}

void CMainFrame::SetProjectName(CString PathName)
{
	m_FileName = PathName;
	int pos = PathName.ReverseFind(92);
	if (pos>0) m_ProjectName = PathName.Right(PathName.GetLength()-pos-1);
	else m_ProjectName = PathName;
	if(m_ProjectName.GetLength()>4){
		m_ProjectName = m_ProjectName.Left(m_ProjectName.GetLength()-4);
	}
}

void CMainFrame::SetSaveState(bool bSave)
{
	m_bSaved = bSave;

	int len = m_ProjectName.GetLength();
	if(m_ProjectName.Right(1)=="*") m_ProjectName = m_ProjectName.Left(len-1);
	if (!bSave){
		m_ProjectName += "*";
	}
}

bool CMainFrame::DeleteFoil(CFoil *pFoil, bool bAsk)
{
	if(!pFoil || !pFoil->m_FoilName.GetLength()) return false;
	CString strong;
	CFoil *pOldFoil;
	OpPoint * pOpPoint;
	CPolar* pPolar;
	int j;

	bool bDelete = true;

	if(bAsk){
		strong = "Are you sure you want to delete \n" + pFoil->m_FoilName ;
		strong+= "\nand all associated OpPoints and Polars ?";
		if(IDYES == AfxMessageBox(strong, MB_YESNOCANCEL)) bDelete = true;
		else return false;
	}

	if (bDelete){
		for ( j= (int)m_oaOpp.GetSize()-1; j>=0; j--){
			pOpPoint = (OpPoint*)m_oaOpp[j];
			if(pOpPoint->m_strFoilName == pFoil->m_FoilName) {
				m_oaOpp.RemoveAt(j);
				delete pOpPoint;
			}
		}
		for ( j= (int)m_oaPolar.GetSize()-1;j>=0; j--){
			pPolar = (CPolar*)m_oaPolar.GetAt(j);
			if(pPolar->m_FoilName == pFoil->m_FoilName) {
				m_oaPolar.RemoveAt(j);
				delete pPolar;
			}
		}
		for (j=0; j< m_oaFoil.GetSize(); j++){
			pOldFoil = (CFoil*)m_oaFoil.GetAt(j);
			if (pOldFoil == pFoil) {
				m_oaFoil.RemoveAt(j);
				delete pOldFoil;
				if(m_pCurFoil == pOldFoil) m_pCurFoil = NULL;
				if(XDirect.m_pCurFoil == pOldFoil) XDirect.m_pCurFoil = NULL;
				break;
			}
		}
		XDirect.m_pCurOpp = NULL;
		XDirect.m_pCurPolar = NULL;
		XDirect.m_pCurFoil = NULL;
		SetSaveState(false);
		return true;
	}
	return false;
}


void CMainFrame::DeletePlane(CPlane *pPlane, bool bResultsOnly) 
{
	if(!pPlane || !pPlane->m_PlaneName.GetLength()) return ;
	CWPolar* pWPolar;
	CPOpp * pPOpp;
	CPlane *pOldPlane;
	SetSaveState(false);
	int i;
	//first remove all POpps associated to the plane
	for (i=(int)m_oaPOpp.GetSize()-1; i>=0; i--){
		pPOpp = (CPOpp*)m_oaPOpp.GetAt(i);
		if(pPOpp->m_PlaneName == pPlane->m_PlaneName){
			m_oaPOpp.RemoveAt(i);
			delete pPOpp;
		}
	}

	Miarex.m_pCurPOpp = NULL;
	Miarex.m_pCurWOpp = NULL;
	//next remove all PPolars associated to the plane
	for (i=(int)m_oaWPolar.GetSize()-1; i>=0; i--){
		pWPolar = (CWPolar*)m_oaWPolar.GetAt(i);
		if (pWPolar->m_UFOName == pPlane->m_PlaneName){
			if(!bResultsOnly){
				m_oaWPolar.RemoveAt(i);
				if(pWPolar == Miarex.m_pCurWPolar){
					Miarex.m_pCurWPolar = NULL;
					m_WOperDlgBar.EnableAnalysis(false);
				}
				delete pWPolar;
			}
			else {
				pWPolar->ResetWPlr();
				pWPolar->m_WArea  = Miarex.m_pCurWing->m_Area;
				pWPolar->m_WMAChord  = Miarex.m_pCurWing->m_MAChord;
				pWPolar->m_WSpan     = Miarex.m_pCurWing->m_Span; 
			}
		}
	}	

	if(bResultsOnly) return;


	for (i=(int)m_oaPlane.GetSize()-1; i>=0; i--){
		pOldPlane = (CPlane*)m_oaPlane.GetAt(i);
		if (pOldPlane == pPlane){
			m_oaPlane.RemoveAt(i);
			delete pPlane;
			if(pPlane == Miarex.m_pCurPlane)	{
				Miarex.m_pCurPlane = NULL;
				Miarex.m_pCurWing  = NULL;
				Miarex.m_pCurStab  = NULL;
				Miarex.m_pCurFin   = NULL;
				Miarex.m_pCurBody  = NULL;
			}
			break;
		}
	}
}


void CMainFrame::RecalcLayout(BOOL bNotify) 
{
	CFrameWnd::RecalcLayout(bNotify);

	if (m_iApp == MIAREX) 
	{
		Miarex.m_bIs2DScaleSet=false;
		Miarex.m_bIs3DScaleSet=false;
		Miarex.SetWPlrLegendPos();
	}
	else if (m_iApp==DIRECTDESIGN){
		CRect ViewRect;
		m_wndView.GetWindowRect(ViewRect);
		CRect rectBar;
		m_AFoilCtrlBar.GetWindowRect(rectBar);
		rectBar.right = ViewRect.right-10;
		rectBar.left  = ViewRect.left;

		if(m_AFoilCtrlBar.IsWindowVisible()){
			m_AFoilCtrlBar.ShowWindow(SW_SHOW);
			DockControlBar(&m_AFoilCtrlBar, AFX_IDW_DOCKBAR_BOTTOM, rectBar);
		}
	}
}


void CMainFrame::OnGuidelines() 
{
	HINSTANCE hInstReturn;

	CString strAppDirectory;
	char    szAppPath[MAX_PATH] = "";
	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
	// Extract directory
	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.GetLength()-9);

	CString strText(strAppDirectory + "Guidelines.pdf");

	hInstReturn = ShellExecute(GetSafeHwnd(),
		_T("open"),
		strText,
		NULL,
		NULL,
		SW_SHOWNORMAL);
}


void CMainFrame::UpdateView()
{
	m_wndView.Invalidate();
}



void CMainFrame::OnLogFile() 
{
	CFile lf;
	CString str;
	CString strAppDirectory;
	char    szAppPath[MAX_PATH] = "";

	GetTempPath(MAX_PATH,szAppPath);
	strAppDirectory = szAppPath;
	str = strAppDirectory + "XFLR5.log";


	if(lf.Open(str, CFile::modeRead)){// file exists (there should be a better way to do this)
		lf.Close();
		
		ShellExecute(GetSafeHwnd(),
			_T("open"),
			str,
			NULL,
			NULL,
			SW_SHOWNORMAL);
		
	}
	else AfxMessageBox("Log File not found", MB_OK);
	
}

double CMainFrame::VLMGetVar(int nVar, CFoil *pFoil0, CFoil *pFoil1,
						 double Re, double Cl, double Tau, bool &bOutRe, bool &bError)
{
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	double Var0, Var1;
	if(!pFoil0) {
		Cl = 0.0;
		Var0 = 0.0;
	}
	else Var0 = VLMGetPlrPoint(pFoil0, Re, Cl,nVar, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;
	if(!pFoil1) {
		Cl = 0.0;
		Var1 = 0.0;
	}
	else Var1 = VLMGetPlrPoint(pFoil1, Re, Cl,nVar, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;
	return ((1-Tau) * Var0 + Tau * Var1);
}


double CMainFrame::VLMGetPlrPoint(CFoil *pFoil, double Re,
							  double Cl, int PlrVar, bool &bOutRe, bool &bError)
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
*/

	CArray <double, double> *pX;	
	double Clmin, Clmax;
	CPolar *pPolar;

	bOutRe = false;
	bError = false;

	if(!pFoil) {
		bOutRe = true;
		bError = true;
		return 0.000;
	}


	int size;
	int n = 0;

	// Are there any Type 1 polars available for this foil ?
	for (int i = 0; i< m_oaPolar.GetSize(); i++){
		pPolar = (CPolar*)m_oaPolar.GetAt(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName)){
			n++;
			if(n>=2) break;
		}
	}

//more than one polar - interpolate between  - tough job

	//First Find the two polars with Reynolds number surrounding wanted Re
	CPolar * pPolar1 = NULL;
	CPolar * pPolar2 = NULL;
	int nPolars = (int)m_oaPolar.GetSize();
	//Type 1 Polars are sorted by crescending Re Number

	//if Re is less than that of the first polar, use this one
	for (i=0; i< nPolars; i++){
		pPolar = (CPolar*)m_oaPolar.GetAt(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName)){
			// we have found the first type 1 polar for this foil
			if (Re < pPolar->m_Reynolds) {
				bOutRe = true;
				//interpolate Cl on this polar
				pX = (CArray <double, double> *) GetPlrVariable(pPolar, PlrVar);
				size = (int)pPolar->m_Cl.GetSize();
				if(Cl < pPolar->m_Cl[0])      {
					return (*pX)[0];
				}
				if(Cl > pPolar->m_Cl[size-1])	{
					return (*pX)[size-1];
				}
				for (i=0; i<size-1; i++){
					if(pPolar->m_Cl[i] <= Cl && Cl < pPolar->m_Cl[i+1]){
						//interpolate
						if(pPolar->m_Cl[i+1]-pPolar->m_Cl[i] < 0.00001)//do not divide by zero
							return (*pX)[i];
						else {
							double u = (Cl - pPolar->m_Cl[i])
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
	for (i=0; i< nPolars; i++){
		pPolar = (CPolar*)m_oaPolar.GetAt(i);
		if((pPolar->m_Type == 1) && (pPolar->m_FoilName == pFoil->m_FoilName)){
			// we have found the first type 1 polar for this foil
			pPolar->GetClLimits(Clmin, Clmax);
			if (pPolar->m_Reynolds <= Re) {
				if(Clmin <= Cl && Cl <= Clmax) {	
					pPolar1 = pPolar;
				}
			}
			else {
				if(Clmin <= Cl && Cl <= Clmax){	
					pPolar2 = pPolar;
					break;
				}
			}
		}
	}


	if (!pPolar2) {
		//then Re is greater than that of any polar
		// so use last polar and interpolate Cls on this polar
		bOutRe = true;
		if(!pPolar1){
			bOutRe = true; 
			bError = true;
			return 0.000;
		}
		size = (int)pPolar1->m_Cl.GetSize();
		if(!size) {
			bOutRe = true; 
			bError = true;
			return 0.000;
		}

		pX = (CArray <double, double> *) GetPlrVariable(pPolar1, PlrVar);
		if(Cl < pPolar1->m_Cl[0])	   return (*pX)[0];
		if(Cl > pPolar1->m_Cl[size-1]) return (*pX)[size-1];
		for (i=0; i<size-1; i++){
			if(pPolar1->m_Cl[i] <= Cl && Cl < pPolar1->m_Cl[i+1]){
				//interpolate
				if(pPolar1->m_Cl[i+1]-pPolar1->m_Cl[i] < 0.00001){//do not divide by zero
					return (*pX)[i];
				}
				else {
					double u = (Cl - pPolar1->m_Cl[i])
							 /(pPolar1->m_Cl[i+1]-pPolar1->m_Cl[i]);
					return ((*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]));
				}
			}
		}
		//Out in Re, out in Cl...
		return (*pX)[size-1];
	}
	else {
		// Re is between that of polars 1 and 2
		// so interpolate Cls for each
		double Var1, Var2;

		if(!pPolar1) {
			bOutRe = true;
			bError = true;
			return 0.000;
		}
		size = (int)pPolar1->m_Cl.GetSize();
		if(!size) {
			bOutRe = true;
			bError = true;
			return 0.000;
		}

		pX = (CArray <double, double> *) GetPlrVariable(pPolar1, PlrVar);
		pPolar1->GetClLimits(Clmin, Clmax);

		if(Cl < Clmin){	         
			Var1 = (*pX)[0];
			bOutRe = true;
		}
		else if(Cl > Clmax) {
			Var1 = (*pX)[size-1];
			bOutRe = true;
		}
		else{
			//first Find the point closest to Cl=0
			int pt = 0;
			double dist = fabs(pPolar1->m_Cl[0]);
			for (i=1; i<size;i++){
				if (fabs(pPolar1->m_Cl[i])< dist){
					dist = fabs(pPolar1->m_Cl[i]);
					pt = i;
				}
			}
			if(Cl<pPolar1->m_Cl[pt]){
				for (i=pt; i>0; i--){
					if(Cl<= pPolar1->m_Cl[i] && Cl > pPolar1->m_Cl[i-1]){
						//interpolate
						if(fabs(pPolar1->m_Cl[i]-pPolar1->m_Cl[i-1]) < 0.00001){//do not divide by zero
							Var1 = (*pX)[i];
							break;
						}
						else{
							double u = (Cl - pPolar1->m_Cl[i-1])
									 /(pPolar1->m_Cl[i]-pPolar1->m_Cl[i-1]);
							Var1 = (*pX)[i-1] + u * ((*pX)[i]-(*pX)[i-1]);
							break;
						}
					}
				}
			}
			else {
				for (i=pt; i<size-1; i++){
					if(pPolar1->m_Cl[i] <=Cl && Cl < pPolar1->m_Cl[i+1]){
						//interpolate
						if(fabs(pPolar1->m_Cl[i+1]-pPolar1->m_Cl[i]) < 0.00001){//do not divide by zero
							Var1 = (*pX)[i];
							break;
						}
						else{
							double u = (Cl - pPolar1->m_Cl[i])
									 /(pPolar1->m_Cl[i+1]-pPolar1->m_Cl[i]);
							Var1 = (*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]);
							break;
						}
					}
				}
			}
		} 

		size = (int)pPolar2->m_Cl.GetSize();
		if(!size) {
			bOutRe = true;
			bError = true;
			return 0.000;
		}

		pX = (CArray <double, double> *) GetPlrVariable(pPolar2, PlrVar);
		pPolar2->GetClLimits(Clmin, Clmax);

		if(Cl < Clmin){	         
			Var2 = (*pX)[0];
			bOutRe = true;
		}
		else if(Cl > Clmax) {
			Var2 = (*pX)[size-1];
			bOutRe = true;
		}
		else{
			//first Find the point closest to Cl=0
			int pt = 0;
			double dist = fabs(pPolar2->m_Cl[0]);
			for (i=1; i<size;i++){
				if (fabs(pPolar2->m_Cl[i])< dist){
					dist = fabs(pPolar2->m_Cl[i]);
					pt = i;
				}
			}
			if(Cl<pPolar2->m_Cl[pt]){
				for (i=pt; i>0; i--){
					if(Cl<= pPolar2->m_Cl[i] && Cl > pPolar2->m_Cl[i-1]){
						//interpolate
						if(fabs(pPolar2->m_Cl[i]-pPolar2->m_Cl[i-1]) < 0.00001){//do not divide by zero
							Var2 = (*pX)[i];
							break;
						}
						else{
							double u = (Cl - pPolar2->m_Cl[i-1])
									 /(pPolar2->m_Cl[i]-pPolar2->m_Cl[i-1]);
							Var2 = (*pX)[i-1] + u * ((*pX)[i]-(*pX)[i-1]);
							break;
						}
					}
				}
			}
			else {
				for (i=pt; i<size-1; i++){
					if(pPolar2->m_Cl[i] <=Cl && Cl < pPolar2->m_Cl[i+1]){
						//interpolate
						if(fabs(pPolar2->m_Cl[i+1]-pPolar2->m_Cl[i]) < 0.00001){//do not divide by zero
							Var2 = (*pX)[i];
							break;
						}
						else{
							double u = (Cl - pPolar2->m_Cl[i])
									 /(pPolar2->m_Cl[i+1]-pPolar2->m_Cl[i]);
							Var2 = (*pX)[i] + u * ((*pX)[i+1]-(*pX)[i]);
							break;
						}
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


double CMainFrame::GetCm0(CFoil *pFoil0, CFoil *pFoil1,
					  double Re, double Tau, bool &bOutRe, bool &bError)
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
	for (int i=-10; i<10; i++){
		Alpha = (double)i;
		Cl1 = GetCl(pFoil0, pFoil1, Re, Alpha, Tau, IsOutRe, IsError);
		if(Cl1>0.0) {
			if(IsOutRe) bOutRe = true;
			if(IsError) bError = true;
			break;
		}
		Cl0 = Cl1;
	}
	if(Cl0>0.0) {
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

 

double CMainFrame::GetCm(CFoil *pFoil0, CFoil *pFoil1,
					 double Re, double Alpha, double Tau, bool &bOutRe, bool &bError)
{
	double Cm0, Cm1;
	bool IsOutRe = false;
	bool IsError  = false;
	bOutRe = false;
	bError = false;

	if(!pFoil0) 
		Cm0 = 0.0;
	else 
		Cm0 = GetPlrPoint(pFoil0, Re, Alpha, 4, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if(!pFoil1) 
		Cm1 = 0.0;
	else 
		Cm1 = GetPlrPoint(pFoil1, Re, Alpha, 4, IsOutRe, IsError);
	if(IsOutRe) bOutRe = true;
	if(IsError) bError = true;

	if (Tau<0.0) Tau = 0.0;
	if (Tau>1.0) Tau = 1.0;
	return ((1-Tau) * Cm0 + Tau * Cm1);
}



void CMainFrame::ExportWPlr(CWPolar * pWPolar)
{
	if (!pWPolar) return;
	CStdioFile XFile;
	CString FileName;
	CString strong;
	if(pWPolar->m_PlrName.Find(".",0) >=0) strong ="";
	else strong = pWPolar->m_PlrName;
	
	strong.Replace("/", " ");
	CFileDialog XFileDlg(false, "txt", strong,
		OFN_OVERWRITEPROMPT,
		_T("Text Format (.txt)|*.txt|"));

	if(IDOK == XFileDlg.DoModal()){
		FileName = XFileDlg.GetFileName();
		pWPolar->Export(FileName);
	}
	UpdateView();
}



void CMainFrame::OnSaveOptions() 
{
	CSaveOptionsDlg dlg;	
	dlg.m_BOpps = m_bSaveOpps;
	dlg.m_BWOpps = m_bSaveWOpps;

	if(IDOK==dlg.DoModal()){
		if(dlg.m_BOpps)  m_bSaveOpps = true; else m_bSaveOpps = false;
		if(dlg.m_BWOpps) m_bSaveWOpps = true; else m_bSaveWOpps = false;
	}
}


void CMainFrame::DeleteWing(CWing *pThisWing, bool bResultsOnly) 
{
	if(!pThisWing){
		return;
	}
	SetSaveState(false);
	int i;

	//first remove all WOpps associated to the wing
	CWOpp * pWOpp;
	for (i=(int)m_oaWOpp.GetSize()-1; i>=0; i--){
		pWOpp = (CWOpp*)m_oaWOpp.GetAt(i);
		if(pWOpp->m_WingName == pThisWing->m_WingName){
			m_oaWOpp.RemoveAt(i);
			if(pWOpp == Miarex.m_pCurWOpp) Miarex.m_pCurWOpp = NULL;
			delete pWOpp;
		}
		Miarex.m_pCurWOpp = NULL;
	}

	//next remove all WPolars associated to the wing
	CWPolar* pWPolar;
	for (i=(int)m_oaWPolar.GetSize()-1; i>=0; i--){
		pWPolar = (CWPolar*)m_oaWPolar.GetAt(i);
		if (pWPolar->m_UFOName == pThisWing->m_WingName){
			if(!bResultsOnly){
				m_oaWPolar.RemoveAt(i);
				if(pWPolar == Miarex.m_pCurWPolar) {
					m_WOperDlgBar.EnableAnalysis(false);
					Miarex.m_pCurWPolar = NULL;
				}
				delete pWPolar;
			}
			else {
				pWPolar->ResetWPlr();
				pWPolar->m_WArea     = Miarex.m_pCurWing->m_Area;
				pWPolar->m_WMAChord  = Miarex.m_pCurWing->m_MAChord;
				pWPolar->m_WSpan     = Miarex.m_pCurWing->m_Span; 
			}
		}
	}	

	if(bResultsOnly) return;// that's enough

	// ... Find the wing in the object array and remove it...
	CWing* pWing;
	for (i=(int)m_oaWing.GetSize()-1; i>=0; i--){
		pWing = (CWing*)m_oaWing.GetAt(i);
		if (pWing == pThisWing){
			m_oaWing.RemoveAt(i);
			delete pWing;
			if(pWing == Miarex.m_pCurWing)	Miarex.m_pCurWing = NULL;
			break;
		}
	}
}

void CMainFrame::OnPlanePrefs() 
{
	C3DColorDlg SDlg(this);
	SDlg.m_bWakePanels    = Miarex.m_bWakePanels;
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

	if(SDlg.DoModal() == IDOK){
		Miarex.m_bWakePanels    = SDlg.m_bWakePanels;
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
		Miarex.m_bResetglWake = true;
		Miarex.m_bResetglBody = true;
		Miarex.m_bResetglGeom = true;
		Miarex.m_bResetglMesh = true;
		Miarex.m_bResetglOpp  = true;

		UpdateView();
	}		
}
	
void CMainFrame::OnLoadProject() 
{
	CWaitCursor wait;
	CFile fp;
	CString strong;

	if(!m_bSaved){
		int resp = AfxMessageBox("Save the current project ?", MB_YESNOCANCEL);
		if (IDCANCEL == resp) return;
		if (IDYES == resp) OnSaveProject();
	}

	//load new project
	CFileDialog XFileDlg(true, "wpa", NULL, OFN_HIDEREADONLY, _T("XFLR5 Project (.wpa)|*.wpa|"));
	if(IDOK==XFileDlg.DoModal()){
		if(LoadProject(XFileDlg.GetPathName()))
			AddRecentFile(XFileDlg.GetPathName());
	}	
}

bool CMainFrame::LoadProject(CString PathName) 
{
	CWaitCursor wait;
	CString strong;
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	int pos = PathName.ReverseFind(92);
	if (pos>0) strong = PathName.Right(PathName.GetLength()-pos-1);
	else       strong = PathName;

	strong = "Loading the Project \n"+ strong;
	CMessageDlg dlg;
	dlg.Create(IDD_MESSAGEDLG,this);
	dlg.SetWindowPos(this, (int)(cx/2)-150,(int)(cy/2)-100,0,0,SWP_NOSIZE);
	dlg.m_bCancel = false;
	dlg.ShowWindow(SW_SHOW);
	dlg.SetMessage(strong);

	DeleteProject();

	UpdateView();
	SetProjectName(PathName);

	if(ReadProject(m_FileName))
	{
		SetSaveState(true);
		if( m_oaWing.GetSize() || m_oaPlane.GetSize() || m_oaBody.GetSize()){
			if(m_iApp==0)
			{
				OnMiarex();
				return TRUE;
			}
			else if(m_iApp==MIAREX) 
			{
				UpdateUFOs();
				m_BodyCtrlBar.UpdateBodies();
				Miarex.SetUFO();
				
				if(Miarex.m_iView==5) 
				{
					Miarex.m_bIs3DScaleSet = false;
					Miarex.SetBody();
				}
				Miarex.UpdateView();
				return TRUE;
			}
		}
		if(m_oaFoil.GetSize())  
		{
			if(m_iApp==0)
				OnXDirect();
			else if(m_iApp==XFOILANALYSIS) 
			{
				UpdateFoils();
				XDirect.SetFoil();
				XDirect.UpdateView();
			}
			else if(m_iApp==DIRECTDESIGN)
			{
				AFoil.SetFoils();
			}
		}
		return TRUE;
	}
	else 
	{
		DeleteProject();
		return FALSE;
	}
}



void CMainFrame::OnAppOpen() 
{
//	CString FileName, PathName;
	CFileDialog XFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("XFLR5 file (.dat; .plr; .wpa)|*.dat; *.plr; *.wpa|"));

	if(IDOK==XFileDlg.DoModal()) 
	{
		int app = LoadFile(XFileDlg.GetFileName(),XFileDlg.GetPathName());
//		if(app>=0) AddRecentFile(XFileDlg.GetPathName());

		UpdateView();
		if(m_iApp==0)
		{
			if (app == XFOILANALYSIS) OnXDirect();
			else if(app==MIAREX)      OnMiarex();
		}
		else if(m_iApp==XFOILANALYSIS)
		{
			UpdateFoils();
			XDirect.SetFoil();
			XDirect.UpdateView();
		}
		else if(m_iApp==MIAREX)
		{
			UpdateUFOs();
			Miarex.SetUFO();
			Miarex.SetBody();
			Miarex.UpdateView();
		}
		else if(m_iApp==DIRECTDESIGN)
		{
			AFoil.SetFoils();
		}
	}
}



void CMainFrame::AddRecentFile(CString FileName)
{
	for(int j=0; j<8;j++){
		if(m_RecentFile[j]==FileName){
			for (int l=j; l<7; l++){
				m_RecentFile[l]=m_RecentFile[l+1];
			}
			break;
		}
	}

	m_RecentFile[7] = m_RecentFile[6];
	m_RecentFile[6] = m_RecentFile[5];
	m_RecentFile[5] = m_RecentFile[4];
	m_RecentFile[4] = m_RecentFile[3];
	m_RecentFile[3] = m_RecentFile[2];
	m_RecentFile[2] = m_RecentFile[1];
	m_RecentFile[1] = m_RecentFile[0];
	m_RecentFile[0] = FileName;
	SetRecentFileMenu();
}

void CMainFrame::SetRecentFileMenu()
{
	CString strong;
	CMenu* pMenu = GetMenu()->GetSubMenu(0);
	if(pMenu){
		pMenu->RemoveMenu(IDM_RECENTFILE1, MF_BYCOMMAND);
		pMenu->RemoveMenu(IDM_RECENTFILE2, MF_BYCOMMAND);
		pMenu->RemoveMenu(IDM_RECENTFILE3, MF_BYCOMMAND);
		pMenu->RemoveMenu(IDM_RECENTFILE4, MF_BYCOMMAND);
		pMenu->RemoveMenu(IDM_RECENTFILE5, MF_BYCOMMAND);
		pMenu->RemoveMenu(IDM_RECENTFILE6, MF_BYCOMMAND);
		pMenu->RemoveMenu(IDM_RECENTFILE7, MF_BYCOMMAND);
		pMenu->RemoveMenu(IDM_RECENTFILE8, MF_BYCOMMAND);

		if(m_RecentFile[7].GetLength()) {
			strong = m_RecentFile[7];
			ShortenFileName(strong);
			pMenu->InsertMenu(10,MF_BYPOSITION,IDM_RECENTFILE8,"8 "+strong);
		}
		if(m_RecentFile[6].GetLength()) {
			strong = m_RecentFile[6];
			ShortenFileName(strong);
			pMenu->InsertMenu(10,MF_BYPOSITION,IDM_RECENTFILE7,"7 "+strong);
		}
		if(m_RecentFile[5].GetLength()) {
			strong = m_RecentFile[5];
			ShortenFileName(strong);
			pMenu->InsertMenu(10,MF_BYPOSITION,IDM_RECENTFILE6,"6 "+strong);
		}
		if(m_RecentFile[4].GetLength()) {
			strong = m_RecentFile[4];
			ShortenFileName(strong);
			pMenu->InsertMenu(10,MF_BYPOSITION,IDM_RECENTFILE5,"5 "+strong);
		}
		if(m_RecentFile[3].GetLength()) {
			strong = m_RecentFile[3];
			ShortenFileName(strong);
			pMenu->InsertMenu(10,MF_BYPOSITION,IDM_RECENTFILE4,"4 "+strong);
		}
		if(m_RecentFile[2].GetLength()) {
			strong = m_RecentFile[2];
			ShortenFileName(strong);
			pMenu->InsertMenu(10,MF_BYPOSITION,IDM_RECENTFILE3,"3 "+strong);
		}
		if(m_RecentFile[1].GetLength()) {
			strong = m_RecentFile[1];
			ShortenFileName(strong);
			pMenu->InsertMenu(10,MF_BYPOSITION,IDM_RECENTFILE2,"2 "+strong);
		}
		if(m_RecentFile[0].GetLength()) {
			strong = m_RecentFile[0];
			ShortenFileName(strong);
			pMenu->InsertMenu(10,MF_BYPOSITION,IDM_RECENTFILE1,"1 "+strong);
		}

		DrawMenuBar();
	}
}
void CMainFrame::ShortenFileName(CString &PathName)
{
	CString strong, strange;
	if(PathName.GetLength()>40){
		int pos = PathName.ReverseFind(92);
		if (pos>0) {
			strong = char(92)+PathName.Right(PathName.GetLength()-pos-1);
			pos = PathName.Find(92);
			if(pos>0) strange = PathName.Left(pos)+char(92)+"...";
			strong = strange+strong;
		}
		else strong = PathName.Left(40);

	}
	else strong = PathName;
	PathName = strong;
}

void CMainFrame::OnRecentFile1()
{
	if(!m_bSaved){
		int resp = AfxMessageBox("Save the current project ?", MB_YESNOCANCEL);
		if (IDCANCEL == resp) return;
		if (IDYES == resp) OnSaveProject();
	}
	if(!LoadProject(m_RecentFile[0])){
		AfxMessageBox("Could not read the file\n"+m_RecentFile[0]);
		m_RecentFile[0] = m_RecentFile[1];
		m_RecentFile[1] = m_RecentFile[2];
		m_RecentFile[2] = m_RecentFile[3];
		m_RecentFile[3] = m_RecentFile[4];
		m_RecentFile[4] = m_RecentFile[5];
		m_RecentFile[5] = m_RecentFile[6];
		m_RecentFile[6] = m_RecentFile[7];
		m_RecentFile[7].Empty();
		SetRecentFileMenu();
	}
}

void CMainFrame::OnRecentFile2()
{
	CString strong;
	if(!m_bSaved)
	{
		int resp = AfxMessageBox("Save the current project ?", MB_YESNOCANCEL);
		if (IDCANCEL == resp) return;
		if (IDYES == resp) OnSaveProject();
	}
	if(!LoadProject(m_RecentFile[1]))
	{
		AfxMessageBox("Could not read the file\n"+m_RecentFile[1]);
		m_RecentFile[1] = m_RecentFile[2];
		m_RecentFile[2] = m_RecentFile[3];
		m_RecentFile[3] = m_RecentFile[4];
		m_RecentFile[4] = m_RecentFile[5];
		m_RecentFile[5] = m_RecentFile[6];
		m_RecentFile[6] = m_RecentFile[7];
		m_RecentFile[7].Empty();
	}
	else{
		strong = m_RecentFile[1];
		m_RecentFile[1] = m_RecentFile[0];
		m_RecentFile[0] = strong;
	}
	SetRecentFileMenu();
}

void CMainFrame::OnRecentFile3()
{
	CString strong;
	if(!m_bSaved){
		int resp = AfxMessageBox("Save the current project ?", MB_YESNOCANCEL);
		if (IDCANCEL == resp) return;
		if (IDYES == resp) OnSaveProject();
	}
	if(!LoadProject(m_RecentFile[2])){
		AfxMessageBox("Could not read the file\n"+m_RecentFile[2]);
		m_RecentFile[2] = m_RecentFile[3];
		m_RecentFile[3] = m_RecentFile[4];
		m_RecentFile[4] = m_RecentFile[5];
		m_RecentFile[5] = m_RecentFile[6];
		m_RecentFile[6] = m_RecentFile[7];
		m_RecentFile[7].Empty();
	}
	else{
		strong = m_RecentFile[2];
		m_RecentFile[2] = m_RecentFile[1];
		m_RecentFile[1] = m_RecentFile[0];
		m_RecentFile[0] = strong;
	}
	SetRecentFileMenu();
}

void CMainFrame::OnRecentFile4()
{
	CString strong;
	if(!m_bSaved){
		int resp = AfxMessageBox("Save the current project ?", MB_YESNOCANCEL);
		if (IDCANCEL == resp) return;
		if (IDYES == resp) OnSaveProject();
	}
	if(!LoadProject(m_RecentFile[3])){
		AfxMessageBox("Could not read the file\n"+m_RecentFile[3]);
		m_RecentFile[3] = m_RecentFile[4];
		m_RecentFile[4] = m_RecentFile[5];
		m_RecentFile[5] = m_RecentFile[6];
		m_RecentFile[6] = m_RecentFile[7];
		m_RecentFile[7].Empty();
	}
	else{
		strong = m_RecentFile[3];
		m_RecentFile[3] = m_RecentFile[2];
		m_RecentFile[2] = m_RecentFile[1];
		m_RecentFile[1] = m_RecentFile[0];
		m_RecentFile[0] = strong;
	}
	SetRecentFileMenu();
}

void CMainFrame::OnRecentFile5()
{
	CString strong;
	if(!m_bSaved){
		int resp = AfxMessageBox("Save the current project ?", MB_YESNOCANCEL);
		if (IDCANCEL == resp) return;
		if (IDYES == resp) OnSaveProject();
	}
	if(!LoadProject(m_RecentFile[4])){
		AfxMessageBox("Could not read the file\n"+m_RecentFile[4]);
		m_RecentFile[4] = m_RecentFile[5];
		m_RecentFile[5] = m_RecentFile[6];
		m_RecentFile[6] = m_RecentFile[7];
		m_RecentFile[7].Empty();
	}
	else{
		strong = m_RecentFile[4];
		m_RecentFile[4] = m_RecentFile[3];
		m_RecentFile[3] = m_RecentFile[2];
		m_RecentFile[2] = m_RecentFile[1];
		m_RecentFile[1] = m_RecentFile[0];
		m_RecentFile[0] = strong;
	}
	SetRecentFileMenu();
}


void CMainFrame::OnRecentFile6()
{
	CString strong;
	if(!m_bSaved){
		int resp = AfxMessageBox("Save the current project ?", MB_YESNOCANCEL);
		if (IDCANCEL == resp) return;
		if (IDYES == resp) OnSaveProject();
	}
	if(!LoadProject(m_RecentFile[5])){
		AfxMessageBox("Could not read the file\n"+m_RecentFile[5]);
		m_RecentFile[5] = m_RecentFile[6];
		m_RecentFile[6] = m_RecentFile[7];
		m_RecentFile[7].Empty();
	}
	else{
		strong = m_RecentFile[5];
		m_RecentFile[5] = m_RecentFile[4];
		m_RecentFile[4] = m_RecentFile[3];
		m_RecentFile[3] = m_RecentFile[2];
		m_RecentFile[2] = m_RecentFile[1];
		m_RecentFile[1] = m_RecentFile[0];
		m_RecentFile[0] = strong;
	}
	SetRecentFileMenu();
}

void CMainFrame::OnRecentFile7()
{
	CString strong;
	if(!m_bSaved){
		int resp = AfxMessageBox("Save the current project ?", MB_YESNOCANCEL);
		if (IDCANCEL == resp) return;
		if (IDYES == resp) OnSaveProject();
	}
	if(!LoadProject(m_RecentFile[6])){
		AfxMessageBox("Could not read the file\n"+m_RecentFile[6]);
		m_RecentFile[6] = m_RecentFile[7];
		m_RecentFile[7].Empty();
	}
	else{
		strong = m_RecentFile[6];
		m_RecentFile[6] = m_RecentFile[5];
		m_RecentFile[5] = m_RecentFile[4];
		m_RecentFile[4] = m_RecentFile[3];
		m_RecentFile[3] = m_RecentFile[2];
		m_RecentFile[2] = m_RecentFile[1];
		m_RecentFile[1] = m_RecentFile[0];
		m_RecentFile[0] = strong;
	}
	SetRecentFileMenu();
}

void CMainFrame::OnRecentFile8()
{
	CString strong;
	if(!m_bSaved){
		int resp = AfxMessageBox("Save the current project ?", MB_YESNOCANCEL);
		if (IDCANCEL == resp) return;
		if (IDYES == resp) OnSaveProject();
	}
	if(!LoadProject(m_RecentFile[7])){
		AfxMessageBox("Could not read the file\n"+m_RecentFile[7]);
		m_RecentFile[7].Empty();
	}
	else{
		strong = m_RecentFile[7];
		m_RecentFile[7] = m_RecentFile[6];
		m_RecentFile[6] = m_RecentFile[5];
		m_RecentFile[5] = m_RecentFile[4];
		m_RecentFile[4] = m_RecentFile[3];
		m_RecentFile[3] = m_RecentFile[2];
		m_RecentFile[2] = m_RecentFile[1];
		m_RecentFile[1] = m_RecentFile[0];
		m_RecentFile[0] = strong;
	}
	SetRecentFileMenu();
}

void CMainFrame::DeleteBody(CBody *pThisBody)
{
	if(!pThisBody){
		return;
	}
	SetSaveState(false);
	int i;


	// ... Find the Body in the object array and remove it...
	CBody* pBody;
	for (i=(int)m_oaBody.GetSize()-1; i>=0; i--){
		pBody = (CBody*)m_oaBody.GetAt(i);
		if (pBody == pThisBody){
			m_oaBody.RemoveAt(i);
			delete pBody;
			if(pBody == Miarex.m_pCurBody)	
			{
				Miarex.m_pCurBody = NULL;
				Miarex.m_pCurFrame = NULL;
			}
			break;
		}
	}
}