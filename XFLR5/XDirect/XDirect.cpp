/****************************************************************************

    CXDirect Class
    Copyright (C) 2003 André Deperrois xflr5@yahoo.com

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

#include "stdafx.h"
#include <afxpriv.h>
#include "../X-FLR5.h"
#include "../main/MainFrm.h"
#include "../main/ChildView.h"
#include "../XDirect/XFoilAdvDlg.h"
#include "../Graph/GraphOptions.h"
#include "../misc/PolarFilter.h"
#include "../misc/PrintOptionsDlg.h"
#include "../misc/EditPlrDlg.h"
#include "../misc/NameDlg.h"
#include "../misc/LinePickerDlg.h"
#include "../Graph/UserVarDlg.h"
#include "../Graph/GraphDlg.h"
#include "2DPanelDlg.h"
#include "CAddDlg.h"
#include "FoilClrDlg.h"
#include "FlapDlg.h"
#include "GeomDlg.h"
#include "InterpolateDlg.h"
#include "BatchDlg.h"
#include "TEGapDlg.h"
#include "LEDlg.h"
#include "EditFoilDlg.h"
#include "NacaDlg.h"
#include "XDirect.h"
#include ".\xdirect.h"


/////////////////////////////////////////////////////////////////////////////
// CXDirect


BEGIN_MESSAGE_MAP(CXDirect, CWnd)
	ON_COMMAND(IDM_XFLR5_ADVSETTINGS, OnAdvSettings)
	ON_COMMAND(IDM_EXPORTFOIL, OnExportFoil)
	ON_COMMAND(IDM_XFLR5_POLARS, OnPolars)
	ON_COMMAND(IDM_SHOWPANELS, OnShowPanels)
	ON_COMMAND(IDM_RENAMEFOIL, OnRenameFoil)
	ON_COMMAND(IDM_REMOVEFOIL, OnDeleteFoil)
	ON_COMMAND(IDM_XFLR5_CLCD, OnClCd)
	ON_COMMAND(IDM_XFLR5_CLALPHA, OnClAlpha)
	ON_COMMAND(IDM_XFLR5_CLXTR, OnClXtr)
	ON_COMMAND(IDM_XFLR5_CMALPHA, OnCmAlpha)
	ON_COMMAND(IDM_ALLFOUR, OnAllFour)
	ON_COMMAND(IDM_XFLR5_CPGRAPH, OnCpGraph)
	ON_COMMAND(IDM_XFLR5_QGRAPH, OnQGraph)
	ON_COMMAND(IDM_RESETGRAPH, OnResetGraph)
	ON_COMMAND(IDM_XFLR5_SINGLEANALYSIS, OnSingleAnalysis)
	ON_COMMAND(IDM_XFLR5_BATCHANALYSIS, OnBatchAnalysis)
	ON_COMMAND(IDM_XFLR5_RENAMEPLR, OnRenamePlr)
	ON_COMMAND(IDM_XFLR5_DELETEPLR, OnDeletePlr)
	ON_COMMAND(IDM_XFLR5_DELETEOPP, OnDeleteOpp)
	ON_COMMAND(IDM_XFLR5_EXPORTPLR, OnExportPlr)
	ON_COMMAND(IDM_DEFINEGRAPH, OnDefineGraph)
	ON_COMMAND(IDM_XFLR5_COUPLEGRAPH, OnCoupleGraph)
	ON_COMMAND(IDM_XFLR5_GRAPHOPTIONS, OnAllGraphOptions)
	ON_COMMAND(IDM_DELFOILPOLARS, OnDelFoilPolars)
	ON_COMMAND(IDM_DELCUROPP, OnDelCurOpp)
	ON_COMMAND(IDM_XFLR5_RESTDEFGRAPHS, OnRestDefGraphs)
	ON_COMMAND(IDM_XFLR5_GEOM, OnGeom)
	ON_COMMAND(IDM_XFLR5_PANEL, OnPanel)
	ON_COMMAND(IDM_XFLR5_CADD, OnCadd)
	ON_COMMAND(IDM_XFLR5_INTER, OnInterpolate)
	ON_COMMAND(IDM_FOILSTYLE, OnFoilStyle)
	ON_COMMAND(IDM_XFLR5_RESTOREDEF, OnRestoreDef)
	ON_COMMAND(IDM_XFLR5_READDEF, OnReadDef)
	ON_COMMAND(IDM_XFLR5_SAVEDEF, OnWriteDef)
	ON_COMMAND(IDM_XFLR5_POLARFILTER, OnPolarFilter)
	ON_COMMAND(IDT_SHOWBL, OnShowBL)
	ON_COMMAND(IDT_SHOWPRESSURE, OnShowPressure)
	ON_COMMAND(IDM_RESTOREVARIABLES, OnRestoreVariables)
	ON_COMMAND(IDM_XFLR5_GRAPHDLG, OnGraphOptions)
	ON_BN_CLICKED(IDC_ANALYZE, OnAnalyze)
	ON_COMMAND(IDM_XFLR5_OPER, OnOper)
	ON_COMMAND(IDM_RESETSCALE, OnResetScale)
	ON_COMMAND(IDM_PRINT, OnPrint)
	ON_COMMAND(IDT_LOAD, OnLoad)
	ON_COMMAND(IDM_XFLR5_EXPORTOPPOINTS, OnExportOpps)
	ON_COMMAND(IDM_INITXFOIL2, OnInitXFoil)
	ON_COMMAND(ID_APP_ABOUT, OnAbout)
	ON_COMMAND(IDM_PRESET, OnPolarReset)
	ON_COMMAND(IDM_XFLR5_SHOWUSERGRAPH, OnShowUserGraph)
	ON_COMMAND(IDM_XFLR5_USERGRAPH, OnUserGraph)
	ON_COMMAND(IDM_EDITPLR, OnEditPolar)
	ON_COMMAND(IDM_DUPLICATEFOIL, OnDuplicateFoil)
	ON_COMMAND(IDM_NORMALIZE, OnNormalize)
	ON_COMMAND(IDM_HIDEPOLARS, OnHidePolars)
	ON_COMMAND(IDM_SHOWPOLARS, OnShowPolars)
	ON_COMMAND(IDM_ROTATEFOIL, OnDeRotateFoil)
	ON_COMMAND(IDM_CPGRAPHDLG, OnCpGraphDlg)
	ON_COMMAND(IDM_HIDEALLPOLARS, OnHideAllPolars)
	ON_COMMAND(IDM_SHOWALLPOLARS, OnShowAllPolars)
	ON_COMMAND(IDM_SAVEPLRFILE, OnSavePlrFile)
	ON_COMMAND(IDM_ZOOMGRAPH, OnZoomGraph)
	ON_COMMAND(IDM_TEGAP, OnTEGap)
	ON_COMMAND(IDM_LERAD, OnLERad)
	ON_COMMAND(IDM_EDITCOORD, OnEditCoord)
	ON_COMMAND(IDM_SETFLAP, OnSetFlap)
	ON_COMMAND(IDM_RESETALLSCALES, OnResetAllScales)
	ON_COMMAND(IDM_EXPORTCUROPP, OnExportCurOpp)
	ON_COMMAND(IDM_NACAFOILS, OnNacaFoils)
	ON_COMMAND(IDM_HPLOT, OnHPlot)
	ON_COMMAND(IDM_DTPLOT, OnDtPlot)
	ON_COMMAND(IDM_DBPLOT, OnDbPlot)
	ON_COMMAND(IDM_UEPLOT, OnUePlot)
	ON_COMMAND(IDM_CFPLOT, OnCfPlot)
	ON_COMMAND(IDM_CDPLOT, OnCdPlot)
	ON_COMMAND(IDM_NPLOT, OnNPlot)
	ON_COMMAND(IDM_CTPLOT, OnCtPlot)
	ON_COMMAND(IDM_RTPLOT, OnRtPlot)
	ON_COMMAND(IDM_LOAD, OnLoad)
	ON_COMMAND(IDT_OPPS, OnOper)
	ON_COMMAND(IDT_POLARS, OnPolars)
	ON_COMMAND(IDM_SAVE, OnSavePolars)
	ON_COMMAND(IDM_RTLPLOT, OnRtLPlot)
	ON_COMMAND(ID_IMPORTPOLAR_XFOILPOLAR, OnImportXFoilPolar)
	ON_COMMAND(ID_IMPORTPOLAR_JAVAFOILPOLAR, OnImportJavaFoilPolar)
	ON_COMMAND(IDM_SHOWCUROPP, OnShowCurOpp)
	ON_COMMAND(IDM_SHOWOPPS, OnShowOpps)
	ON_COMMAND(IDM_HIDEOPPS, OnHideOpps)
	ON_COMMAND(IDM_SHOWFOILOPPS, OnShowFoilOpps)
	ON_COMMAND(IDM_HIDEFOILOPPS, OnHideFoilOpps)
	ON_COMMAND(IDM_CPI, OnCpi)
	ON_COMMAND(ID_CURRENTFOIL_MANAGEFOILS, OnManageFoils)
	ON_COMMAND(IDM_SHOWCPGRAPH, OnShowCpGraph)
	ON_COMMAND(IDM_EXPORTCURRESULTS, OnExportCurrentResults)
	ON_COMMAND(IDM_EXPORTGRAPHTOFILE, OnExportGraphToFile)
END_MESSAGE_MAP()


CXDirect::CXDirect(CWnd* pWnd)
{
	m_pChildWnd = pWnd;
//	m_hIcon = AfxGetApp()->LoadIcon(IDR_ICON1);
	m_hcArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_hcCross = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
	m_hcMove = AfxGetApp()->LoadCursor(IDC_HMOVE);

	pi = 3.141592654;

	m_bAnimate        = false;
	m_bAnimatePlus    = false;
	m_bBL             = false;
	m_bAutoInitBL     = true;
	m_bCpGraph        = true;
	m_bTransGraph     = false;
	m_bPolar          = false;
	m_bShowPanels     = false;
	m_bShowUserGraph  = true;

	m_bTrans          = false;
	m_bType1          = true;
	m_bType2          = true;
	m_bType3          = true;
	m_bType4          = true;
	m_bPressure       = false;
	m_bFromList       = false;
	m_bFromZero       = false;
	m_bShowTextOutput = true;
	m_bNeutralLine    = true;
	m_bShowInviscid   = false;
	m_bAlpha          = true;
	m_bInitBL         = true;
	m_bStoreOpp       = true;
	m_bViscous        = true;

	m_IterLim   = 100;

	m_iPlrGraph = 0;
	m_iPlrView  = 0;
	m_OppVar    = 0;

	m_PointDown.x = 0;
	m_PointDown.y = 0;

	m_FoilYPos = 0;
	m_iFoilStyle = 0;
	m_iFoilWidth = 0;
	
	m_XPolar     = 2;
	m_YPolar     = 1;
	m_XCz        = 0; 
	m_YCz        = 1;
	m_XCm        = 0;
	m_YCm        = 5;
	m_XTr        = 6;
	m_YTr        = 1;
	m_XUser      =  0;
	m_YUser      = 10;

	m_posAnimate = 0;

	m_pXFoil       = NULL;

	m_pCmGraph     = NULL;
	m_pCpGraph     = NULL;
	m_pCurGraph    = NULL;
	m_pCzGraph     = NULL;
	m_pPolarGraph  = NULL;
	m_pTrGraph     = NULL;
	m_pUserGraph   = NULL;
	m_pCurFoil     = NULL;
	m_pCurPolar    = NULL;
	m_pCurOpp      = NULL;

	m_FoilYPos = 140; // position of foil from bottom of screen
	
	m_ReMax = 150000.0;
	m_ReInc =  50000.0;
	
	m_pCpGraph    = new Graph();
	m_pPolarGraph = new Graph();
	m_pCmGraph    = new Graph();
	m_pCzGraph    = new Graph();
	m_pTrGraph    = new Graph();
	m_pUserGraph  = new Graph();

	SetDisplayDefault();

	m_pCpGraph->SetXTitle("X");
	m_pCpGraph->SetYTitle("Cp");
	m_pCpGraph->SetInverted(true);
	m_pCpGraph->SetXMin(0.0);
	m_pCpGraph->SetXMax(1.0);
	m_pCpGraph->SetYMin(-0.1);
	m_pCpGraph->SetYMax(0.1);


	m_pPolarGraph->SetXMin(0.0);
	m_pPolarGraph->SetXMax(0.1);
	m_pPolarGraph->SetYMin(-0.1);
	m_pPolarGraph->SetYMax(0.1);
	m_pPolarGraph->SetType(2);

	m_pCmGraph->SetXMin(0.0);
	m_pCmGraph->SetXMax(0.1);
	m_pCmGraph->SetYMin(-0.1);
	m_pCmGraph->SetYMax(0.1);
	m_pCmGraph->SetType(2);

	m_pCzGraph->SetXMin(0.0);
	m_pCzGraph->SetXMax(0.1);
	m_pCzGraph->SetYMin(-0.1);
	m_pCzGraph->SetYMax(0.1);
	m_pCzGraph->SetType(2);

	m_pTrGraph->SetXMin(0.0);
	m_pTrGraph->SetXMax(0.1);
	m_pTrGraph->SetYMin(-0.1);
	m_pTrGraph->SetYMax(0.1);
	m_pTrGraph->SetType(2);

	m_pUserGraph->SetXMin(0.0);
	m_pUserGraph->SetXMax(0.1);
	m_pUserGraph->SetYMin(-0.1);
	m_pUserGraph->SetYMax(0.1);
	m_pUserGraph->SetType(2);

	OnRestoreDef();


	m_posAnimate = 0; // no animation to start with


	SetTitles(m_pPolarGraph, m_XPolar,m_YPolar);
	SetTitles(m_pCzGraph, m_XCz, m_YCz);
	SetTitles(m_pCmGraph, m_XCm, m_YCm);
	SetTitles(m_pTrGraph, m_XTr, m_YTr);
	SetTitles(m_pUserGraph, m_XUser, m_YUser);

	memset(m_ReList, 0, sizeof(m_ReList));
	memset(m_NCritList, 0, sizeof(m_NCritList));
	memset(m_MachList, 0, sizeof(m_MachList));
	m_ReList[0]  =   40000.0;
	m_ReList[1]  =   60000.0;
	m_ReList[2]  =   80000.0;
	m_ReList[3]  =  100000.0;
	m_ReList[4]  =  130000.0;
	m_ReList[5]  =  160000.0;
	m_ReList[6]  =  200000.0;
	m_ReList[7]  =  250000.0;
	m_ReList[8]  =  300000.0;
	m_ReList[9]  =  500000.0;
	m_ReList[10] = 1000000.0;
	m_ReList[11] = 3000000.0;

	m_NCritList[0]  = 9.00;
	m_NCritList[1]  = 9.00;
	m_NCritList[2]  = 9.00;
	m_NCritList[3]  = 9.00;
	m_NCritList[4]  = 9.00;
	m_NCritList[5]  = 9.00;
	m_NCritList[6]  = 9.00;
	m_NCritList[7]  = 9.00;
	m_NCritList[8]  = 9.00;
	m_NCritList[9]  = 9.00;
	m_NCritList[10] = 9.00;
	m_NCritList[11] = 9.00;
	m_NRe = 12;

}

CXDirect::~CXDirect()
{

	delete m_pCpGraph;
	delete m_pCmGraph;
	delete m_pCzGraph;
	delete m_pPolarGraph;
	delete m_pTrGraph;
	delete m_pUserGraph;

}



/////////////////////////////////////////////////////////////////////////////
// CXDirect message handlers


void CXDirect::UpdateView(CDC *pDC) 
{		
	if(m_bAnimate) return;// painting is done elsewhere

	CChildView * pChildView = (CChildView*)m_pChildWnd;
	if(!pDC) pDC = pChildView->GetDC();
	if(!pDC) return;//nothing to draw on

	CDC memdc; 
	memdc.CreateCompatibleDC (pDC);

	CBitmap  bmp;
	bmp.CreateCompatibleBitmap(pDC, m_rCltRect.Width(), m_rCltRect.Height());
	CBitmap * pOldBmp = memdc.SelectObject(&bmp);

	memdc.SetBkColor(pChildView->m_crBackColor);
	memdc.FillSolidRect(&m_rCltRect,pChildView->m_crBackColor);

	// user has requested a polar plot
	if(m_bPolar)
	{
		if     (m_iPlrView == 0) PaintPolarGraphs(&memdc, &m_rCltRect);
		else if(m_iPlrView == 1) PaintSingleGraph(&memdc, &m_rSingleRect);
		else if(m_iPlrView == 2) PaintCoupleGraph(&memdc, &m_rCltRect);
	} 
	else
	{
		PaintOpPoint(&memdc, &m_rCltRect, &m_rDrawRect);
	}
	// Blit back
	pDC->BitBlt(m_rCltRect.left,    m_rCltRect.top,
				m_rCltRect.Width(), m_rCltRect.Height(),
				&memdc,0,0,SRCCOPY);

	pChildView->ReleaseDC(pDC);

	memdc.SelectObject(pOldBmp);
	memdc.DeleteDC();
	bmp.DeleteObject();
}


void CXDirect::SetFoilScale(CRect CltRect)
{
	m_rCltRect.CopyRect(&CltRect);
	m_rDrawRect.SetRect(40,10,CltRect.right-40,CltRect.bottom-250);

	int h  = CltRect.Height();
	int w  = CltRect.Width();
	int w2 = (int)(w/2);

	int w3 = (int)(0.35*w);
	int w23 = 2*w3;

//	CRect Rect1(0,0,w23,CltRect.bottom-00);

	m_rSingleRect.SetRect(0,0,w23,CltRect.bottom-00);
//	m_rSingleRect.SetRect(0,00,CltRect.right-300,CltRect.bottom-00);

	m_ptOffset.x = m_rDrawRect.left +(int)(1.0*m_pCpGraph->GetMargin());
	m_ptOffset.y = CltRect.bottom-m_FoilYPos;
	if(m_OppVar<2){
		m_fFoilScale = 1.0/(m_rDrawRect.Width()-2.0*m_pCpGraph->GetMargin());
	}
	else{
		double p0  = m_pCpGraph->xToClient(0.0);
		double p1  = m_pCpGraph->xToClient(1.0);
		m_fFoilScale = 1.0/(p1-p0);
	}
}

void CXDirect::OnResetScale() 
{
	m_ptOffset.x = m_rDrawRect.left +(int)(1.f*m_pCpGraph->GetMargin());
	m_ptOffset.y = m_rCltRect.bottom-m_FoilYPos;
	if(m_OppVar<2){
		m_fFoilScale = 1.f/(m_rDrawRect.Width()-2.f*m_pCpGraph->GetMargin());
	}
	else{
		double p0  = m_pCpGraph->xToClient(0.0);
		double p1  = m_pCpGraph->xToClient(1.0);
		m_fFoilScale = 1.f/(p1-p0);
	}
	if(!m_bAnimate) UpdateView();
}


OpPoint* CXDirect::GetCurOpPoint()
{
	OpPoint* pOpPoint;

	CMainFrame* pFrame =  (CMainFrame*)m_pFrame;
	CString str;
	int pos = pFrame->m_ctrlOpp.GetCurSel();
	if (pos == CB_ERR) return NULL;

	pFrame->m_ctrlOpp.GetLBText(pos, str);
	double point;
	int res = sscanf(str, "%lf", &point);
	if (res !=1) return NULL;


	for (int i=0; i<m_poaOpp->GetSize(); i++){
		if(!m_pCurPolar || !m_pCurFoil) return NULL;
		pOpPoint = (OpPoint*)m_poaOpp->GetAt(i);
		//since alphas are calculated at 1/100th
		if (pOpPoint->m_strFoilName == m_pCurFoil->m_FoilName){
			if (pOpPoint->m_strPlrName == m_pCurPolar->m_PlrName){
				if(m_pCurPolar->m_Type !=4){

					if(abs(pOpPoint->Alpha - point) <0.01){
						return pOpPoint;
					}
				}
				else{
					if(abs(pOpPoint->Reynolds - point) <0.1){
						return pOpPoint;
					}
				}
			}
		}
	}
	return NULL;// shouldn't ever get here, fortunately
}

OpPoint* CXDirect::GetOpPoint(double Alpha)
{
	OpPoint* pOpPoint;
	for (int i=0; i<m_poaOpp->GetSize(); i++){
		if(!m_pCurPolar) return NULL;
		pOpPoint = (OpPoint*)m_poaOpp->GetAt(i);
		//since alphas are calculated at 1/100th
		if (pOpPoint->m_strFoilName == m_pCurFoil->m_FoilName){
			if (pOpPoint->m_strPlrName == m_pCurPolar->m_PlrName){
				if(m_pCurPolar->m_Type !=4){
					if(abs(pOpPoint->Alpha - Alpha) <0.01){
						return pOpPoint;
					}
				}
				else{
					if(abs(pOpPoint->Reynolds - Alpha) <0.1){
						return pOpPoint;
					}
				}
			}
		}
	}
	return NULL;// shouldn't ever get here, fortunately
}

OpPoint* CXDirect::AddOpPoint(OpPoint *pNewPoint)
{
	// adds an Operating Point to the array from XFoil results
	CMainFrame*pFrame = (CMainFrame*)m_pFrame;
	int i;

	if(!pNewPoint) 
	{
		pNewPoint = new OpPoint();
		if(pNewPoint ==NULL) 
		{
			AfxMessageBox("Not enough memory to store the OpPoint\nOnly the polar point will be saved", MB_OK);
			return NULL;
		}
		else
		{
			if(!m_pXFoil->lvconv)
			{
				delete pNewPoint;
				return NULL;
			}
			pNewPoint->m_pXDirect = this;
			pNewPoint->Alpha = m_pXFoil->alfa * 180/pi;
			pNewPoint->m_Color = pFrame->GetColor(2);
			AddOpData(pNewPoint);
			pFrame->SetSaveState(false);
		}
	}

	if(m_bStoreOpp)
	{
		bool bIsInserted = false;
		OpPoint* pOpPoint;
		CPolar *pPolar = GetPolar(pNewPoint->m_strPlrName);
		m_pCurPolar = pPolar;
		if(!pPolar) 
		{
			delete pNewPoint;
			return NULL;
		}

		// first add the OpPoint to the OpPoint Array for the current FoilName
		for (i=0; i<m_poaOpp->GetSize(); i++)
		{
			pOpPoint = (OpPoint*)m_poaOpp->GetAt(i);
			if (pNewPoint->m_strFoilName.CompareNoCase(pOpPoint->m_strFoilName)<0)
			{
				//insert point
				m_poaOpp->InsertAt(i, pNewPoint);
				bIsInserted = true;
				i = (int)m_poaOpp->GetSize();// to break
			}
			else if (pNewPoint->m_strFoilName == pOpPoint->m_strFoilName)
			{
				if (pNewPoint->Reynolds < pOpPoint->Reynolds)
				{
					//insert point
					m_poaOpp->InsertAt(i, pNewPoint);
					bIsInserted = true;
					i = (int)m_poaOpp->GetSize();// to break
				}
				else if (abs(pNewPoint->Reynolds-pOpPoint->Reynolds)<1.0)
				{
					if(abs(pNewPoint->Alpha - pOpPoint->Alpha)<0.005)
					{
						//replace existing point
						m_poaOpp->RemoveAt(i);
						delete pOpPoint;
						m_poaOpp->InsertAt(i, pNewPoint);
						bIsInserted = true;
						i = (int)m_poaOpp->GetSize();// to break
					}
					else if (pNewPoint->Alpha < pOpPoint->Alpha)
					{
						//insert point
						m_poaOpp->InsertAt(i, pNewPoint);
						bIsInserted = true;
						i = (int)m_poaOpp->GetSize();// to break
					}
				}
			}
		}
		if (!bIsInserted) 	m_poaOpp->Add(pNewPoint);
	}

	// Now insert OpPoint in the current CPolar object

	if(m_pXFoil->lvconv && m_pCurPolar) 
	{
		if(m_pCurPolar->m_Type ==2 || m_pCurPolar->m_Type ==3)
		{
			if(pNewPoint && pNewPoint->Reynolds<1.00e7)
			{
				m_pCurPolar->AddData(pNewPoint);
			}
		}
		else
		{
			m_pCurPolar->AddData(pNewPoint);
		}
	}

	if(!m_bStoreOpp)
	{
		delete pNewPoint;
		pNewPoint = NULL;
	}
	m_pCurOpp = pNewPoint;

	if(m_bPolar)
	{
		CreatePolarCurves();
		UpdateView();
	}

	return pNewPoint;
}


CFoil* CXDirect::GetFoil(CString strFoilName)
{
	//returns a pointer to the foil with the corresponding name
	// returns NULL if not found
	if(!strFoilName.GetLength()) return NULL;

	CFoil* pFoil;
	for (int i=0; i<m_poaFoil->GetSize(); i++){
		pFoil = (CFoil*)m_poaFoil->GetAt(i);
		if (pFoil->m_FoilName == strFoilName) return pFoil;
	}
	return NULL;
}


void CXDirect::SetHingeMoments(OpPoint *pOpPoint)
{
	bool bFound;
	int i;
	double hmom, hfx, hfy;
	double dx, dy, xmid, ymid, pmid;
	double xof, yof;
	double ymin, ymax;
	xof = m_pCurFoil->m_TEXHinge/100.0;
	ymin = m_pCurFoil->GetBaseLowerY(xof);
	ymax = m_pCurFoil->GetBaseUpperY(xof);
	yof = ymin + (ymax-ymin) * m_pCurFoil->m_TEYHinge/100.0;

	if(m_pCurFoil->m_bTEFlap){
		hmom = 0.0;
		hfx  = 0.0;
		hfy  = 0.0;
		
		//---- integrate pressures on top and bottom sides of flap
		for (i=0;i<m_pCurFoil->n-1;i++){
			if (m_pCurFoil->x[i]>xof &&	m_pCurFoil->x[i+1]>xof) {
				dx = m_pCurFoil->x[i+1] - m_pCurFoil->x[i];
				dy = m_pCurFoil->y[i+1] - m_pCurFoil->y[i];
				xmid = 0.5*(m_pCurFoil->x[i+1]+m_pCurFoil->x[i]) - xof;
				ymid = 0.5*(m_pCurFoil->y[i+1]+m_pCurFoil->y[i]) - yof;
				
				if(pOpPoint->m_bVisc)	pmid = 0.5*(pOpPoint->Cpv[i+1] + pOpPoint->Cpv[i]);
				else					pmid = 0.5*(pOpPoint->Cpi[i+1] + pOpPoint->Cpi[i]);

//				TRACE("dx=%.3f  dy=%.3f  xmid=%.3f  ymid=%.3f  cpvi=%.3f  cpvi1=%.3f   pmid=%.5f\n",
//					   dx, dy, xmid, ymid, pOpPoint->Cpv[i], pOpPoint->Cpv[i+1], pmid);

				hmom += pmid * (xmid*dx + ymid*dy);
				hfx  -= pmid * dy;
				hfy  += pmid * dx;
			}
		}
		//Next add top chunk left out in the previous loop
		bFound = false;
/*		for (i=0;i<m_pCurFoil->n-1;i++){
			if(m_pCurFoil->x[i]>xof && m_pCurFoil->x[i+1]<xof){
				bFound =true;
				break;
			}
		}
		if(bFound) {
			dx = topx - x[i-1];
			dy = topy - y[i-1];
			xmid = 0.5*(topx+x[i-1]) - xof;
			ymid = 0.5*(topy+y[i-1]) - yof;
			if(s[i] != s[i-1]) frac = (tops-s[i-1])/(s[i]-s[i-1]);
			else	 frac = 0.0;

			if(lvisc) {
				topp = cpv[i]*frac + cpv[i-1]*(1.0-frac);
				pmid = 0.5*(topp+cpv[i-1]);
			}
			else{
				topp = cpi[i]*frac + cpi[i-1]*(1.0-frac);
				pmid = 0.5*(topp+cpi[i-1]);
			}
			hmom = hmom + pmid*(xmid*dx + ymid*dy);
			hfx  = hfx  - pmid* dy;
			hfy  = hfy  + pmid* dx;
		}*/
		//Then add bottom chunk left out
		

		//Next add internal hinge to top surface contribution

		//Next add internal hinge to bottom surface contribution

		//store the results
		pOpPoint->m_TEHMom = hmom;
		pOpPoint->XForce   = hfx;
		pOpPoint->YForce   = hfy;
	}
}


void CXDirect::AddOpData(OpPoint *pOpPoint)
{
	// Adds result of the XFoil Calculation to the OpPoint object
	int i, j, ibl, is, k;

	pOpPoint->m_strFoilName = m_pCurFoil->m_FoilName;
	pOpPoint->m_strPlrName = m_pCurPolar->m_PlrName;

	pOpPoint->n        = m_pXFoil->n;
	pOpPoint->Cd       = m_pXFoil->cd;
	pOpPoint->Cdp      = m_pXFoil->cdp;
	pOpPoint->Cl       = m_pXFoil->cl;
	pOpPoint->m_XCP    = m_pXFoil->xcp;
	pOpPoint->Cm       = m_pXFoil->cm;
	pOpPoint->Reynolds = m_pXFoil->reinf;
	pOpPoint->Mach     = m_pXFoil->minf;
	pOpPoint->ACrit    = m_pXFoil->acrit;
	pOpPoint->m_bTEFlap    = m_pCurFoil->m_bTEFlap;
	pOpPoint->m_bLEFlap    = m_pCurFoil->m_bLEFlap;

	pOpPoint->Cpmn   = m_pXFoil->cpmn;

	for (k=0; k<m_pXFoil->n; k++)
	{
		pOpPoint->x[k]   = m_pXFoil->x[k+1];
		pOpPoint->y[k]   = m_pXFoil->y[k+1];
//		pOpPoint->s[k]   = m_pXFoil->s[k+1];
		pOpPoint->Cpi[k] = m_pXFoil->cpi[k+1];
		pOpPoint->Qi[k]  = m_pXFoil->qgamm[k+1];
	}

	if(m_pXFoil->lvisc && m_pXFoil->lvconv)
	{
		pOpPoint->Xtr1 =m_pXFoil->xoctr[1];
		pOpPoint->Xtr2 =m_pXFoil->xoctr[2];
		pOpPoint->m_bVisc = true; 
		pOpPoint->m_bDispSurf = true;
		for (k=0; k<m_pXFoil->n; k++) 
		{
			pOpPoint->Cpv[k] = m_pXFoil->cpv[k+1];
			pOpPoint->Qv[k] = m_pXFoil->qvis[k+1];
		}
	}
	else
	{
		pOpPoint->m_bVisc = false;
	}

	if(pOpPoint->m_bTEFlap || pOpPoint->m_bLEFlap)
	{
		SetHingeMoments(pOpPoint);
//		pOpPoint->HMom = m_pXFoil->hmom;
//		pOpPoint->XForce = m_pXFoil->hfx;
//		pOpPoint->YForce = m_pXFoil->hfy;
	}

	if(!m_pXFoil->lvisc || !m_pXFoil->lvconv)	return;

//---- add boundary layer on both sides of airfoil 
	int nd1=0;
	int nd2=0;
	int nd3=0;
	for (is=1; is<=2; is++)
	{
		for ( ibl=2; ibl<=m_pXFoil->iblte[is];ibl++)
		{
			i = m_pXFoil->ipan[ibl][is];
			pOpPoint->xd1[i] = m_pXFoil->x[i] + m_pXFoil->nx[i]*m_pXFoil->dstr[ibl][is];
			pOpPoint->yd1[i] = m_pXFoil->y[i] + m_pXFoil->ny[i]*m_pXFoil->dstr[ibl][is];
			nd1++;
		}
	}

//---- set upper and lower wake dstar fractions based on first wake point
	is=2;
	double dstrte = m_pXFoil->dstr[m_pXFoil->iblte[is]+1][is];
	double dsf1, dsf2;
	if(dstrte!=0.0) 
	{
		dsf1 = (m_pXFoil->dstr[m_pXFoil->iblte[1]][1] + 0.5*m_pXFoil->ante) / dstrte;
		dsf2 = (m_pXFoil->dstr[m_pXFoil->iblte[2]][2] + 0.5*m_pXFoil->ante) / dstrte;
	}
	else
	{
		dsf1 = 0.5;
		dsf2 = 0.5;
	}

//---- plot upper wake displacement surface
	ibl = m_pXFoil->iblte[1];
	i = m_pXFoil->ipan[ibl][1];
	pOpPoint->xd2[0] = m_pXFoil->x[i] + m_pXFoil->nx[i]*m_pXFoil->dstr[ibl][1];
	pOpPoint->yd2[0] = m_pXFoil->y[i] + m_pXFoil->ny[i]*m_pXFoil->dstr[ibl][1];
	nd2++;

	j= m_pXFoil->ipan[m_pXFoil->iblte[is]+1][is]  -1;
	for (ibl=m_pXFoil->iblte[is]+1; ibl<=m_pXFoil->nbl[is]; ibl++)
	{
		i = m_pXFoil->ipan[ibl][is];
		pOpPoint->xd2[i-j] = m_pXFoil->x[i] - m_pXFoil->nx[i]*m_pXFoil->dstr[ibl][is]*dsf1;
		pOpPoint->yd2[i-j] = m_pXFoil->y[i] - m_pXFoil->ny[i]*m_pXFoil->dstr[ibl][is]*dsf1;
		nd2++;
	}
	
//---- plot lower wake displacement surface
	ibl = m_pXFoil->iblte[2];
	i = m_pXFoil->ipan[ibl][2];
	pOpPoint->xd3[0] = m_pXFoil->x[i] + m_pXFoil->nx[i]*m_pXFoil->dstr[ibl][2];
	pOpPoint->yd3[0] = m_pXFoil->y[i] + m_pXFoil->ny[i]*m_pXFoil->dstr[ibl][2];
	nd3++;

	j = m_pXFoil->ipan[m_pXFoil->iblte[is]+1][is]  -1;
	for (ibl=m_pXFoil->iblte[is]+1; ibl<=m_pXFoil->nbl[is]; ibl++)
	{
		i = m_pXFoil->ipan[ibl][is];
		pOpPoint->xd3[i-j] = m_pXFoil->x[i] + m_pXFoil->nx[i]*m_pXFoil->dstr[ibl][is]*dsf2;
		pOpPoint->yd3[i-j] = m_pXFoil->y[i] + m_pXFoil->ny[i]*m_pXFoil->dstr[ibl][is]*dsf2;
		nd3++;
	}
	pOpPoint->nd1 = nd1;
	pOpPoint->nd2 = nd2;
	pOpPoint->nd3 = nd3;
}




void CXDirect::PaintSingleGraph(CDC *pDC, CRect *pDrawRect)
{
	if(pDrawRect->Width()<200 || pDrawRect->Height()<200) return;
	
//	int margin = 10;
	switch (m_iPlrGraph){
		case 1:{
			m_pCurGraph = m_pPolarGraph;
			break;
		}
		case 2:{
			m_pCurGraph = m_pCzGraph;
			break;
		}
		case 3:{
			m_pCurGraph = m_pTrGraph;
			break;
		}
		case 4:{
			m_pCurGraph = m_pCmGraph;
			break;
		}
		case 5:{
			m_pCurGraph = m_pUserGraph;
			break;
		}
	}

	if (m_pCurGraph) m_pCurGraph->DrawGraph(pDC, pDrawRect, false);

	CPoint Place(pDrawRect->right+5,pDrawRect->top);
	DrawPolarLegend(pDC, false, Place, m_rCltRect.bottom);
}

void CXDirect::PaintCoupleGraph(CDC *pDC, CRect *pDrawRect)
{
	//pDrawrect is the client rectangle
/*	int h  = pDrawRect->Height();
	int w  = pDrawRect->Width();
	if(w<400 || h<300) return;
	
	int margin = 10;
	int h34 = (int)(3*h/4);
	int w2  = (int)(w/2);

	CRect Rect1(0,0,w2,h34);
	CRect Rect2(w2,0,w,h34);*/


	int margin = 10;
	int h  = m_rCltRect.Height();
	int w  = m_rCltRect.Width();
	int w2 = (int)(w/2);
	int h23 = (int)(2*h/3);
	CRect Rect1(0,0,w2,h23);
	CRect Rect2(w2,0,w,h23);

	m_pPolarGraph->DrawGraph(pDC, &Rect1, false);
	m_pUserGraph->DrawGraph(pDC, &Rect2, false);

	CPoint Place(5, h23+margin);
	DrawPolarLegend(pDC, false, Place, m_rCltRect.bottom);
}

void CXDirect::OnPrint() 
{
	if(!m_bPolar && !m_pCurFoil)
		return;

	CMainFrame * pFrame = (CMainFrame*)m_pFrame;
	CString JobName;
	if(m_bPolar) 
		JobName = "XFLR5 - Polars";
	else 
		JobName = "XFLR5 - " + m_pCurFoil->m_FoilName;

	bool bStatus = IsBlackAndWhite();

	CDC dc;
	CPrintDialog dlgPrint(false);

	if(IDOK==dlgPrint.DoModal()){
		DOCINFO di;
		ZeroMemory(&di, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);
		di.lpszDocName = JobName;
		dc.Attach(dlgPrint.GetPrinterDC());
			
		// set landscape mode
		DEVMODE *pDevMode = dlgPrint.GetDevMode();
		int h, w;
		GetPaperSize(pDevMode, w, h);

		CPrintOptionsDlg dlg;
		dlg.m_bColor    = GetPrintColor();
		dlg.m_bWindowBk = GetWindowBk();
		dlg.m_bGraphBk  = GetGraphBk();
		dlg.m_Left   = pFrame->m_LeftMargin;
		dlg.m_Right  = pFrame->m_RightMargin;
		dlg.m_Top    = pFrame->m_TopMargin;
		dlg.m_Bottom = pFrame->m_BottomMargin;
		dlg.m_PenWidth = ((CXFLR5App*)AfxGetApp())->m_PenWidth;
		dlg.m_Width  = w;
		dlg.m_Height = h;
		dlg.m_PrinterName = dlgPrint.GetDeviceName();

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


void CXDirect::PrintAll(CDC *pDC, CRect *pCltRect)
{
	if(!m_bPolar && !m_pCurFoil){
		return;
	}

	CString str1, Result;

	CChildView *pChildView = (CChildView*)m_pChildWnd;
	pDC->SetMapMode(MM_HIMETRIC);
	if(GetWindowBk() && !IsBlackAndWhite()){
		pDC->SetBkMode(OPAQUE);

		CPen WhitePen;
		WhitePen.CreateStockObject(BLACK_PEN);
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

//	int w = pCltRect->Width();
//	int h = pCltRect->Height();

	CRect DrawRect;
	DrawRect.CopyRect(pCltRect);
	
	double FoilScale = 0.00006;

	OpPoint * pOpPoint = GetCurOpPoint();
	double alpha;
	if(pOpPoint) alpha = -pOpPoint->Alpha*pi/180.0;
	else		 alpha = 0.0;

	// user has requested a polar plot
	if(m_bPolar){
		if(m_iPlrView ==1){

			int w = pCltRect->Width();
			int w23 = int(2*w/3);

			CRect SingleRect(pCltRect->left, pCltRect->top,
							 pCltRect->left+w23,pCltRect->bottom);
			if(m_pCurGraph) m_pCurGraph->DrawGraph(pDC, &SingleRect, true);

			CPoint Place(SingleRect.right,-SingleRect.top);
			DrawPolarLegend(pDC, true, Place, pCltRect->bottom);
		}
		else if(m_iPlrView ==2){
			int margin = 00;
			int w = pCltRect->Width()-2*margin;
			int h = pCltRect->Height();
			int w2 = int(w/2);
			int h34 = int(3*h/4);

			CRect FirstRect(pCltRect->left,pCltRect->top, 
							pCltRect->left+w2, pCltRect->top+h34);
			m_pPolarGraph->DrawGraph(pDC, &FirstRect, true);

			CRect SecondRect(pCltRect->left+w2,pCltRect->top,
							 pCltRect->right, pCltRect->top+h34);
			m_pUserGraph->DrawGraph(pDC, &SecondRect, true);

			CPoint Place(pCltRect->left,-pCltRect->top-h34-500);
			DrawPolarLegend(pDC, true, Place, pCltRect->bottom);
		}
		else if(m_iPlrView ==0)
		{
			int w   = pCltRect->Width();
			int h   = pCltRect->Height() ;
			int h2  = (int)(h/2);
			int w10 = (int) (w/10);
			CRect Rect1(pCltRect->left,pCltRect->top,
						pCltRect->left+4*w10,pCltRect->top+h2);
			CRect Rect2(pCltRect->left+4*w10,pCltRect->top,
						pCltRect->left+7*w10,pCltRect->top+h2);
			CRect Rect3(pCltRect->left+4*w10,pCltRect->top+h2,
						pCltRect->left+7*w10,pCltRect->bottom);
			CRect Rect4(pCltRect->left+7*w10,pCltRect->top,
						pCltRect->right,pCltRect->top+h2);

			m_pPolarGraph->DrawGraph(pDC, &Rect1, true);
			m_pCzGraph->DrawGraph(pDC, &Rect2, true);
			m_pCmGraph->DrawGraph(pDC, &Rect3, true);
			m_pTrGraph->DrawGraph(pDC, &Rect4, true);

			if(m_bShowUserGraph){
				CRect Rect5(pCltRect->left+7*w10,pCltRect->top+h2,
							pCltRect->right, pCltRect->bottom);
				m_pUserGraph->DrawGraph(pDC, &Rect5, true);
			}

			CPoint Place(pCltRect->left, -pCltRect->top-h2-500);
			DrawPolarLegend(pDC, true, Place, pCltRect->bottom);
		}
	}
	else if (m_pCurFoil) {
	//user has requested a specific operating point

		int Offsetx     = DrawRect.left + 2000;
		CRect CpRect;
		CpRect.SetRect(Offsetx-1500,						//left
					   DrawRect.top,						//top
					   Offsetx+(int)(1.0/FoilScale)+1500,	//right
					   pCltRect->bottom-6500);				//bottom
		
		m_pCpGraph->DrawGraph(pDC, &CpRect, true);
		
		LOGFONT TempFont;
		memcpy(&TempFont, &pChildView->m_WndLogFont,sizeof(LOGFONT));
		TempFont.lfHeight = TempFont.lfHeight*30;

		CFont LegendFont;
		LegendFont.CreateFontIndirect(&TempFont);

		CFont * pOldFont = pDC->SelectObject(&LegendFont);
		CPoint Place(DrawRect.left+5000, -DrawRect.top-750);
		m_pCpGraph->DrawLegend(pDC, true, Place);
		int D = 5000;

		pDC->SetTextAlign(TA_RIGHT);
		if(IsBlackAndWhite())pDC->SetTextColor(RGB(0,0,0));
		else pDC->SetTextColor(pChildView->m_WndTextColor);
		int RightOffset = 500;
		pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, m_pCurFoil->m_FoilName);
		if(m_pCurFoil->m_bTEFlap){
			CString strong;
			strong="%";

			D+=500;
			str1.LoadString(IDS_XHINGE);
			Result.Format(str1, m_pCurFoil->m_TEXHinge);
			Result+=strong;
			pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);

			D+=500;
			str1.LoadString(IDS_YHINGE);
			Result.Format(str1, m_pCurFoil->m_TEYHinge);
			Result+=strong;
			pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);

			D+=500;
			str1.LoadString(IDS_FLAPANGLE);
			Result.Format(str1, m_pCurFoil->m_TEFlapAngle);
			pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);
		}

		D+=500;
		str1.LoadString(IDS_FORCEDUPPERTRANS);
		Result.Format(str1, m_pCurPolar->m_XTop);
		pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);

		D+=500;
		str1.LoadString(IDS_FORCEDLOWERTRANS);
		Result.Format(str1, m_pCurPolar->m_XBot);
		pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);

		D+=500;
		Result.Format("Ncr  = %7.2f", m_pCurPolar->m_ACrit);
		pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);
		D+=1000;

		DrawFoil(pDC, alpha, FoilScale, Offsetx, pCltRect->bottom-4000, true);	

		if (m_bNeutralLine && m_pCurFoil) {
			COLORREF color = m_crNeutralColor;
			int style = m_iNeutralStyle;
			int width = m_iNeutralWidth;
			if(IsBlackAndWhite()) GetBWColor(color, style, width);

			LOGBRUSH lb;
			lb.lbStyle = BS_SOLID;
			lb.lbColor = color;
			CPen NeutralPen(PS_GEOMETRIC | style, GetPenWidth(width, true), &lb);
			CPen *pOldPen = pDC->SelectObject(&NeutralPen);
			pDC->MoveTo(pCltRect->right, -pCltRect->bottom+4000);
			pDC->LineTo(pCltRect->left,  -pCltRect->bottom+4000);
			pDC->SelectObject(pOldPen);
		}

		if(m_pCurOpp){
			ReynoldsFormat(Result, m_pCurOpp->Reynolds);
			Result = "Re  = "+ Result;
			pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);
			if(m_pCurOpp->Mach>1.e-7){
				D+=500;
				Result.Format("Ma  = %.4f", m_pCurOpp->Mach);
				pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);
			}
			Result.Format("Alpha  = %8.2f°", m_pCurOpp->Alpha);
			D+=500;
			pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);
			Result.Format("Cl  = %9.3f", m_pCurOpp->Cl);
			D+=500;
			pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);
			Result.Format("Cd  = %9.4f", m_pCurOpp->Cd);
			D+=500;
			pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);
			Result.Format("Cm  = %9.4f", m_pCurOpp->Cm);
			D+=500;
			pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);
			Result.Format("L/D = %9.3f", m_pCurOpp->Cl/m_pCurOpp->Cd);
			D+=500;
			pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);

			str1.LoadString(IDS_UPPERTRANS);
			Result.Format(str1, m_pCurOpp->Xtr1);
			D+=500;
			pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);

			str1.LoadString(IDS_LOWERTRANS);
			Result.Format(str1, m_pCurOpp->Xtr2);
			D+=500;
			pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);

			if(m_pCurOpp->m_bTEFlap){
				str1.LoadString(IDS_TEHINGEMOMENT);
				Result.Format(str1, m_pCurOpp->m_TEHMom);
				D+=500;
				pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);
			}
			if(m_pCurOpp->m_bLEFlap){
				str1.LoadString(IDS_LEHINGEMOMENT);
				Result.Format(str1, m_pCurOpp->m_LEHMom);
				D+=500;
				pDC->TextOut(pCltRect->right-RightOffset,pCltRect->top-D, Result);
			}
			if(pOpPoint->m_bDispSurf  && m_bBL)
				DrawBL(pDC, m_pCurOpp, FoilScale, Offsetx, pCltRect->bottom-4000, true);
			if(m_bPressure)
				DrawCpv(pDC, m_pCurOpp, FoilScale, Offsetx, pCltRect->bottom-4000, true);
		}
		pDC->SelectObject(pOldFont);
		LegendFont.DeleteObject();
	}
	CFont VFont;
	pDC->SetTextColor(0);// black version name
	VFont.CreateFont(280,0,0,0,FW_NORMAL,FALSE,FALSE,0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, FF_SWISS+VARIABLE_PITCH, "Arial");

	CFont * pOldFont = pDC->SelectObject(&VFont);

	CMainFrame*pFrame = (CMainFrame*)m_pFrame;
	pDC->SetTextAlign(TA_RIGHT);
	pDC->TextOut(pCltRect->right,-pCltRect->bottom, pFrame->m_VersionName);

	pDC->SelectObject(pOldFont);
	VFont.DeleteObject();
}


LRESULT CXDirect::KickIdle()
{	
	if(m_bAnimate){
		if(!m_pCurFoil || !m_pCurPolar) return 0;
		CChildView * pChildView = (CChildView*)m_pChildWnd;

		int size = (int)m_poaOpp->GetSize();

		if(size<=1) return 0;

		OpPoint* pOpPoint;
		//find the current position to display

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

		if(m_posAnimate<0 || m_posAnimate>=m_poaOpp->GetSize()) return 0;
		pOpPoint = (OpPoint*)m_poaOpp->GetAt(m_posAnimate);
		if(!pOpPoint) return 0;
		
		if (pOpPoint->m_strPlrName  == m_pCurPolar->m_PlrName &&
			pOpPoint->m_strFoilName == m_pCurFoil->m_FoilName){

			CDC* pDC = pChildView->GetDC();
			if(!pDC) return 0;
		
			CDC dcMem;
			dcMem.CreateCompatibleDC(pDC);
			dcMem.SetBkMode(TRANSPARENT);
			
			dcMem.SetTextColor(pChildView->m_WndTextColor);
			CFont ReFont;
			ReFont.CreateFontIndirect(&pChildView->m_WndLogFont);
			CFont* pOldFont = dcMem.SelectObject(&ReFont);
			
			CRect dcMemRect(0,0,m_rCltRect.Width(), m_rCltRect.Height());
			CBitmap bmb;
			bmb.CreateCompatibleBitmap(pDC,m_rCltRect.Width(), m_rCltRect.Height());
			CBitmap *pOldBitmap = dcMem.SelectObject(&bmb);
			
			dcMem.SetBkColor(pChildView->m_crBackColor);
			dcMem.FillSolidRect(&m_rCltRect,pChildView->m_crBackColor);

			CreateOppCurves(pOpPoint);
			
			dcMem.SetTextAlign(TA_LEFT);
			dcMem.SetTextColor(m_pCpGraph->GetTitleColor());

			CString strong;
			strong.Format("Re = %.0f",pOpPoint->Reynolds);
			dcMem.TextOut(10,m_rCltRect.bottom-30, strong);
			strong.Format("Alpha = %.3f",pOpPoint->Alpha);
			dcMem.TextOut(10,m_rCltRect.bottom-20, strong);
			dcMem.SelectObject(pOldFont);

			double alpha     = -pOpPoint->Alpha/180.0*pi;
			DrawFoil(&dcMem, alpha, m_fFoilScale, m_ptOffset.x,m_ptOffset.y, false);

			if(pOpPoint->m_bDispSurf && m_bBL)
				DrawBL(&dcMem, pOpPoint, m_fFoilScale, m_ptOffset.x, m_ptOffset.y, false);
			if(m_bPressure)
				DrawCpv(&dcMem, pOpPoint, m_fFoilScale, m_ptOffset.x, m_ptOffset.y, false);
			if(m_bCpGraph)
				m_pCpGraph->DrawGraph(&dcMem, &m_rDrawRect, false);

			pDC->BitBlt(m_rCltRect.left, m_rCltRect.top/*+m_dz*/, m_rCltRect.Width(), m_rCltRect.Height(),
				&dcMem,0, 0, SRCCOPY);

			pChildView->ReleaseDC(pDC);
			dcMem.SelectObject(pOldBitmap);
			bmb.DeleteObject();

			CMainFrame *pFrame = (CMainFrame*)m_pFrame;
			int speed = pFrame->m_OperDlgBar.m_ctrlSpeed.GetPos();
			Sleep(500-speed);
		}
		return 1;
	}
	else return(0);
}


void CXDirect::DrawCpv(CDC* pDC, OpPoint* pOpPoint, double scale, int offx, int offy, bool bIsPrinting)
{
	CPoint offset;
	double scalex, scaley;
	COLORREF color;
	int style, width;
	if(!pOpPoint->m_bVisc ) return;
	if(!bIsPrinting){
		offset.x = offx;
		offset.y = offy;
		scalex  = scale;
		scaley  = scale;
	}
	else{
		offset.x =  offx;
		offset.y = -offy;
		scalex  =  scale;
		scaley  = -scale;
	}
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	color = m_crPressureColor;
	style = m_iPressureStyle;
	width = m_iPressureWidth;
	GetBWColor(color, style, width);
	lb.lbColor = color;

	CPen CpvPen(PS_GEOMETRIC | style, GetPenWidth(width, bIsPrinting), &lb);
	CPen* pOldPen = pDC->SelectObject(&CpvPen);

	int i;
	double alpha = -pOpPoint->Alpha*pi/180.0;
	double cosa = cos(alpha);
	double sina = sin(alpha);
	double x, y ,xs, ys, xe, ye, dx, dy, x1, x2, y1, y2, r2;
	double cp;

	for(i=0; i<m_pCurFoil->n;i++){
        if(pOpPoint->m_bVisc)	cp = pOpPoint->Cpv[i];
        else					cp = pOpPoint->Cpi[i];
		x = m_pCurFoil->x[i];
		y = m_pCurFoil->y[i];

		xs = (x-0.5)*cosa - y*sina + 0.5;
		ys = (x-0.5)*sina + y*cosa;

		if(cp>0){
			x += m_pCurFoil->nx[i] * cp * 0.05;
			y += m_pCurFoil->ny[i] * cp * 0.05;

			xe = (x-0.5)*cosa - y*sina + 0.5;
			ye = (x-0.5)*sina + y*cosa;
			pDC->MoveTo((int)( xs/scalex) + offset.x, (int)(-ys/scaley) + offset.y);
			pDC->LineTo((int)( xe/scalex) + offset.x, (int)(-ye/scaley) + offset.y);
			
			dx = xe - xs;
			dy = ye - ys;
			r2 = sqrt(dx*dx + dy*dy);
			dx = dx/r2;
			dy = dy/r2;

			x1 = xs + 0.0085*dx + 0.005*dy;
			y1 = ys + 0.0085*dy - 0.005*dx;
			x2 = xs + 0.0085*dx - 0.005*dy;
			y2 = ys + 0.0085*dy + 0.005*dx;

			pDC->MoveTo((int)( xs/scalex) + offset.x, (int)(-ys/scaley) + offset.y);
			pDC->LineTo((int)( x1/scalex) + offset.x, (int)(-y1/scaley) + offset.y);

			pDC->MoveTo((int)( xs/scalex) + offset.x, (int)(-ys/scaley) + offset.y);
			pDC->LineTo((int)( x2/scalex) + offset.x, (int)(-y2/scaley) + offset.y);
		}
		else {
			x += -m_pCurFoil->nx[i] * cp *0.05;
			y += -m_pCurFoil->ny[i] * cp *0.05;

			xe = (x-0.5)*cosa - y*sina+ 0.5;
			ye = (x-0.5)*sina + y*cosa;
			pDC->MoveTo((int)( xs/scalex) + offset.x, (int)(-ys/scaley) + offset.y);
			pDC->LineTo((int)( xe/scalex) + offset.x, (int)(-ye/scaley) + offset.y);

			dx = xe - xs;
			dy = ye - ys;
			r2 = sqrt(dx*dx + dy*dy);
			dx = -dx/r2;
			dy = -dy/r2;

			x1 = xe + 0.0085*dx + 0.005*dy;
			y1 = ye + 0.0085*dy - 0.005*dx;
			x2 = xe + 0.0085*dx - 0.005*dy;
			y2 = ye + 0.0085*dy + 0.005*dx;

			pDC->MoveTo((int)( xe/scalex) + offset.x, (int)(-ye/scaley) + offset.y);
			pDC->LineTo((int)( x1/scalex) + offset.x, (int)(-y1/scaley) + offset.y);

			pDC->MoveTo((int)( xe/scalex) + offset.x, (int)(-ye/scaley) + offset.y);
			pDC->LineTo((int)( x2/scalex) + offset.x, (int)(-y2/scaley) + offset.y);
		}
	}

	//last draw lift at XCP position
	CPen CpPen(PS_GEOMETRIC | m_iPressureStyle, GetPenWidth(m_iPressureWidth+1, bIsPrinting), &lb);
	pDC->SelectObject(&CpPen);

	xs =  (pOpPoint->m_XCP-0.5)*cosa  + 0.5;
	ys = -(pOpPoint->m_XCP-0.5)*sina ;

	xe = xs;
	ye = ys - pOpPoint->Cl/10.0;

	pDC->MoveTo((int)( xs/scalex) + offset.x, (int)(ys/scaley) + offset.y);
	pDC->LineTo((int)( xs/scalex) + offset.x, (int)(ye/scaley) + offset.y);

	dx = xe - xs;
	dy = ye - ys;
	r2 = sqrt(dx*dx + dy*dy);
	dx = -dx/r2;
	dy = -dy/r2;

	x1 = xe + 0.0085*dx + 0.005*dy;
	y1 = ye + 0.0085*dy - 0.005*dx;
	x2 = xe + 0.0085*dx - 0.005*dy;
	y2 = ye + 0.0085*dy + 0.005*dx;

	pDC->MoveTo((int)( xe/scalex) + offset.x, (int)(ye/scaley) + offset.y);
	pDC->LineTo((int)( x1/scalex) + offset.x, (int)(y1/scaley) + offset.y);

	pDC->MoveTo((int)( xe/scalex) + offset.x, (int)(ye/scaley) + offset.y);
	pDC->LineTo((int)( x2/scalex) + offset.x, (int)(y2/scaley) + offset.y);

	pDC->SelectObject(pOldPen);
}

void CXDirect::DrawBL(CDC *pDC, OpPoint* pOpPoint, double scale, int offx, int offy, bool bIsPrinting)
{
	CPoint offset;
	double scalex, scaley;

	if(!pOpPoint->m_bVisc || !pOpPoint->m_bDispSurf) return;

	if(!bIsPrinting)
	{
		offset.x = offx;
		offset.y = offy;
		scalex  = scale;
		scaley  = scale;
	}
	else
	{
		offset.x =  offx;
		offset.y = -offy;
		scalex  =  scale;
		scaley  = -scale;
	}

	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	COLORREF color = m_crBLColor;
	int style = m_iBLStyle;
	int width = m_iBLWidth;
	GetBWColor(color, style, width);

	lb.lbColor = color;

	CPen WakePen(PS_GEOMETRIC | style, GetPenWidth(width, bIsPrinting), &lb);

	CPen* pOldPen = pDC->SelectObject(&WakePen);

	int i;
	double alpha = -pOpPoint->Alpha*pi/180.0;
	double cosa = cos(alpha);
	double sina = sin(alpha);
	double x,y;

	x = (pOpPoint->xd1[1]-0.5)*cosa - pOpPoint->yd1[1]*sina + 0.5;
	y = (pOpPoint->xd1[1]-0.5)*sina + pOpPoint->yd1[1]*cosa;
	pDC->MoveTo((int)( x/scalex) + offset.x,
				(int)(-y/scaley) + offset.y);
	for (i=2; i<=pOpPoint->nd1; i++)
	{
		x = (pOpPoint->xd1[i]-0.5)*cosa - pOpPoint->yd1[i]*sina + 0.5;
		y = (pOpPoint->xd1[i]-0.5)*sina + pOpPoint->yd1[i]*cosa;
		pDC->LineTo((int)( x/scalex) + offset.x,
					(int)(-y/scaley) + offset.y);
	}
	x = (pOpPoint->xd2[0]-0.5)*cosa - pOpPoint->yd2[0]*sina + 0.5;
	y = (pOpPoint->xd2[0]-0.5)*sina + pOpPoint->yd2[0]*cosa;
	pDC->MoveTo((int)( x/scalex) + offset.x,
				(int)(-y/scaley) + offset.y);
	for (i=1; i<pOpPoint->nd2; i++)
	{
		x = (pOpPoint->xd2[i]-0.5)*cosa - pOpPoint->yd2[i]*sina + 0.5;
		y = (pOpPoint->xd2[i]-0.5)*sina + pOpPoint->yd2[i]*cosa;
		pDC->LineTo((int)( x/scalex) + offset.x,
					(int)(-y/scaley) + offset.y);
	}

	x = (pOpPoint->xd3[0]-0.5)*cosa - pOpPoint->yd3[0]*sina + 0.5;
	y = (pOpPoint->xd3[0]-0.5)*sina + pOpPoint->yd3[0]*cosa;
	pDC->MoveTo((int)( x/scalex) + offset.x,
				(int)(-y/scaley) + offset.y);
	for (i=1; i<pOpPoint->nd3; i++)
	{
		x = (pOpPoint->xd3[i]-0.5)*cosa - pOpPoint->yd3[i]*sina + 0.5;
		y = (pOpPoint->xd3[i]-0.5)*sina + pOpPoint->yd3[i]*cosa;
		pDC->LineTo((int)( x/scalex) + offset.x,
					(int)(-y/scaley) + offset.y);
	}

	pDC->SelectObject(pOldPen);
}


void CXDirect::DrawFoil(CDC *pDC, double alpha, double scale, int offx, int offy, 
						 bool bIsPrinting)
{
	int width, style;
	CPoint offset;
	double scalex, scaley;
	if(!bIsPrinting){
		offset.x = offx;
		offset.y = offy;
		scalex  = scale;
		scaley  = scale;
	}
	else{
		offset.x =  offx;
		offset.y = -offy;
		scalex   =  scale;
		scaley   = -scale;
	}
	
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	lb.lbColor = m_crFoilColor;
	width = m_iFoilWidth;
	style = m_iFoilStyle;

	CPen FoilPen(PS_GEOMETRIC | style, GetPenWidth(width, bIsPrinting), &lb);

	CPen *pOldPen = pDC->SelectObject(&FoilPen);
	CRect R;
	double x,y,sina,cosa;

	if(m_pCurOpp){// we rotate the airfoil
		cosa = cos(alpha);
		sina = sin(alpha);
	}
	else{
		cosa = 1.0;
		sina = 0.0;
	}
	x = (m_BufferFoil.x[0]-0.5)*cosa - m_BufferFoil.y[0]*sina + 0.5;
	y = (m_BufferFoil.x[0]-0.5)*sina + m_BufferFoil.y[0]*cosa;

	pDC->MoveTo((int)( x/scalex) + offset.x,
				(int)(-y/scaley) + offset.y);
	for (int i=0; i<m_BufferFoil.n;i++){
		x = (m_BufferFoil.x[i]-0.5)*cosa - m_BufferFoil.y[i]*sina+ 0.5;
		y = (m_BufferFoil.x[i]-0.5)*sina + m_BufferFoil.y[i]*cosa;
		if(m_bShowPanels){
			R.left   = (int)( x/scalex) + offset.x-2;
			R.right  = (int)( x/scalex) + offset.x+2;
			R.top    = (int)(-y/scaley) + offset.y -2; 
			R.bottom = (int)(-y/scaley) + offset.y +2; 
			if(bIsPrinting) R.InflateRect(40,40);
			pDC->Rectangle(&R);
		}
		if(m_bShowPanels && m_BufferFoil.m_iHighLight==i &&!bIsPrinting){
			CPen HighPen(PS_SOLID, 2, RGB(255,0,0));
			CPen *pOld1 = pDC->SelectObject(&HighPen);
			R.left   = (int)( x/scalex) + offset.x -2;
			R.right  = (int)( x/scalex) + offset.x +2;
			R.top    = (int)(-y/scaley) + offset.y -2; 
			R.bottom = (int)(-y/scaley) + offset.y +2;
			pDC->Rectangle(&R);
			pDC->SelectObject(pOld1);
		}
		pDC->LineTo((int)( x/scalex) + offset.x,
					(int)(-y/scaley) + offset.y);
	}

	pDC->SelectObject(pOldPen);
}



void CXDirect::PaintPolarGraphs(CDC *pDC, CRect* pCltRect)
{
	int margin = 10;
	int h  = pCltRect->Height();
	int w  = pCltRect->Width();
	int h2 = (int)(h/2);
	int w2 = (int)(w/2);
	int w3 = (int)(w/3);
	int w4 = (int)(w/4);

	if(w3>200 && h>250)
	{
		CRect Rect1(0,0,w2,h2);
		CRect Rect2(w2,0,3*w4,h2);
		CRect Rect3(w2, h2, 3*w4, pCltRect->bottom);
		CRect Rect4(3*w4,0,pCltRect->right,h2);

		m_pPolarGraph->DrawGraph(pDC, &Rect1, false);
		m_pCzGraph->DrawGraph(pDC, &Rect2, false);
		m_pCmGraph->DrawGraph(pDC, &Rect3, false);
		m_pTrGraph->DrawGraph(pDC, &Rect4, false);

		if(m_bShowUserGraph)
		{
			CRect Rect5(3*w4,h2,pCltRect->right-0,pCltRect->bottom-0);
			m_pUserGraph->DrawGraph(pDC, &Rect5, false);
		}

		CPoint Place(5, h2+2*margin);
		DrawPolarLegend(pDC, false, Place, pCltRect->bottom);
	}
}


bool CXDirect::LoadSettings(CArchive &ar) 
{
	CMainFrame *pFrame = (CMainFrame*) m_pFrame;
	COLORREF cr;
	int k, l;

	try
	{
	//  we're reading/loading

		ar >> m_crFoilColor >> m_iFoilStyle >> m_iFoilWidth;
		if(m_crFoilColor <0 || m_crFoilColor> RGB(255,255,255)){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(m_iFoilStyle <0 || m_iFoilStyle> 1000){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(m_iFoilWidth <0 || m_iFoilWidth> 1000){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}

		ar >> m_crBLColor >> m_iBLStyle >> m_iBLWidth;
		if(m_crBLColor <0 || m_crBLColor> RGB(255,255,255))
		{
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(m_iBLStyle <0 || m_iBLStyle> 1000){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(m_iBLWidth <0 || m_iBLWidth> 1000){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}

		ar >> m_crPressureColor >> m_iPressureStyle >> m_iPressureWidth;
		if(m_crPressureColor <0 || m_crPressureColor> RGB(255,255,255)){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(m_iPressureStyle <0 || m_iPressureStyle> 1000){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(m_iPressureWidth <0 || m_iPressureWidth> 1000){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_bShowPanels = true; 	else m_bShowPanels = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}

		if(k) m_bNeutralLine = true;	else m_bNeutralLine = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}

		if(k) m_bCurOppOnly = true;	else m_bCurOppOnly = false;

		ar >> cr;
		if(cr <0 || cr> RGB(255,255,255)){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_crNeutralColor = cr;
		ar >> k;
		if(k <0 || k> 20){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_iNeutralStyle = k;
		ar >> k;
		if(k <0 || k> 5){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_iNeutralWidth = k;
		m_pCpGraph->Serialize(ar);

		if(m_OppVar == 0 || m_OppVar>=2) {
			m_pCpGraph->SetYTitle("Cp");
			m_pCpGraph->SetInverted(true);
		}
		else{
			m_pCpGraph->SetYTitle("Q");
			m_pCpGraph->SetInverted(false);
		}

		m_pPolarGraph->Serialize(ar);
		m_pCzGraph->Serialize(ar);
		m_pCmGraph->Serialize(ar);
		m_pTrGraph->Serialize(ar);
		m_pUserGraph->Serialize(ar);
		m_ViscDlg.m_RmsGraph.Serialize(ar);
		
		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if (k) m_bShowUserGraph = true;	else m_bShowUserGraph = false;

		ar >> k;
		if(k<0 || k>5){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_iPlrGraph = k;
//			if(m_iPlrGraph) GetMenu()->GetSubMenu(5)->EnableMenuItem(IDM_PLRGRAPHOPTIONS, MF_GRAYED);

		ar >> k;
		if(k<0 || k>3){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_iPlrView = k;
		if(m_iPlrView == 0 || m_iPlrView ==2) m_iPlrGraph = 0;


		ar >> k>>l;
		if (k<0 || l<0 || k>100 || l>100) {
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_XPolar= k; m_YPolar = l;

		ar >> k>>l;
		if (k<0 || l<0 || k>100 || l>100) {
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_XCz= k; m_YCz = l;

		ar >> k>>l;
		if (k<0 || l<0 || k>100 || l>100) {
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_XCm= k; m_YCm = l;

		ar >> k>>l;
		if (k<0 || l<0 || k>100 || l>100) {
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_XTr= k; m_YTr = l;

		ar >> k>>l;
		if (k<0 || l<0 || k>100 || l>100) {
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_XUser= k; m_YUser = l;

		SetTitles(m_pPolarGraph, m_XPolar,m_YPolar);
		SetTitles(m_pCzGraph, m_XCz, m_YCz);
		SetTitles(m_pCmGraph, m_XCm, m_YCm);
		SetTitles(m_pTrGraph, m_XTr, m_YTr);
		SetTitles(m_pUserGraph, m_XUser, m_YUser);

		ar >> k;
		if(k<0 || k>30) {
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		m_NRe = k;
		for (int l=0; l< m_NRe; l++){
			ar >> m_ReList[l]>> m_MachList[l] >> m_NCritList[l];
			if(m_ReList[l]<-1.0 || m_NCritList[l]<-1.0 || m_NCritList[l]>10000.0 || m_MachList[l]<0.0){
				memset(m_ReList, 0, sizeof(m_ReList));
				memset(m_MachList, 0, sizeof(m_MachList));
				memset(m_NCritList, 0, sizeof(m_NCritList));
				CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
				throw pfe;
			}
		}

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if (k) m_bFromList = true; else m_bFromList = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if (k) m_bFromZero = true; else m_bFromZero = false;

		ar >> m_Adlg.m_MachDef >> m_Adlg.m_ReDef >> m_Adlg.m_Type >> m_ReMax >> m_ReInc >> m_IterLim ;
		ar >> pFrame->m_OperDlgBar.m_Alpha;
		ar >> pFrame->m_OperDlgBar.m_AlphaMax;
		ar >> pFrame->m_OperDlgBar.m_DeltaAlpha;

		ar >> m_IterLim;


		ar >> k;
		if (k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_bShowInviscid = true; else m_bShowInviscid = false;

		ar >> k;
		if (k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) m_bStoreOpp = true; else m_bStoreOpp = false;

		ar >> k;
		if (k<0 || k>1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if(k) pFrame->m_OperDlgBar.m_bSequence = true; else pFrame->m_OperDlgBar.m_bSequence = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if (k) m_bAlpha = true; else m_bAlpha = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if (k) m_bViscous = true; else m_bViscous = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if (k) m_bPolar = true; else m_bPolar = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if (k) m_bPressure = true; else m_bPressure = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if (k) m_bBL = true; else m_bBL = false;

		ar >> m_Adlg.m_NCrit >> m_Adlg.m_XTopTr >> m_Adlg.m_XBotTr;

		if(m_Adlg.m_XTopTr <0.0 ||  m_Adlg.m_XTopTr >1.0001 ||m_Adlg.m_XBotTr <0.0 ||  m_Adlg.m_XBotTr >1.0001 ){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		ar >> m_Adlg.m_Reynolds >> m_Adlg.m_fMach;
		if (m_Adlg.m_Reynolds<=0.0 || m_Adlg.m_fMach<0.0){
			m_Adlg.m_Reynolds = 100000.0;
			m_Adlg.m_fMach    = 0.0;

			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		
		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if (k) m_bType1 = true; else m_bType1 = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if (k) m_bType2 = true; else m_bType2 = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if (k) m_bType3 = true; else m_bType3 = false;

		ar >> k;
		if(k!=0 && k!=1){
			CArchiveException *pfe = new CArchiveException(CArchiveException::badIndex);
			throw pfe;
		}
		if (k) m_bType4 = true; else m_bType4 = false;

		for (int i=0; i<20; i++){
			ar >> m_UFOdlg.m_ColSize[i];
		}
		ar >> pFrame->m_OperDlgBar.m_Alpha >> pFrame->m_OperDlgBar.m_AlphaMax >> pFrame->m_OperDlgBar.m_DeltaAlpha;
		ar >> pFrame->m_OperDlgBar.m_Cl >> pFrame->m_OperDlgBar.m_ClMax >> pFrame->m_OperDlgBar.m_DeltaCl;
		ar >> pFrame->m_OperDlgBar.m_Re >> pFrame->m_OperDlgBar.m_ReMax >> pFrame->m_OperDlgBar.m_DeltaRe;

		pFrame->m_OperDlgBar.SetParams();
		CheckMenu();

		return true;
//-------------	

	}
	catch (CArchiveException *ex){
//		AfxMessageBox("Failed to load settings",MB_OK);
		ex->Delete();
		SetDisplayDefault();
		return false;
	}
	catch (CException *ex){
//		AfxMessageBox("Failed to load settings",MB_OK);
		ex->Delete();
		SetDisplayDefault();
		return false;
	}
}


void CXDirect::SaveSettings(CArchive &ar) 
{
	CMainFrame *pFrame = (CMainFrame*) m_pFrame;

	int l;

	//  we're saving/storing
	ar << m_crFoilColor << m_iFoilStyle << m_iFoilWidth;
	ar << m_crBLColor << m_iBLStyle << m_iBLWidth;
	ar << m_crPressureColor << m_iPressureStyle << m_iPressureWidth;
	if(m_bShowPanels)  ar << 1; else ar << 0;
	if(m_bNeutralLine) ar << 1; else ar << 0;
	if(m_bCurOppOnly)  ar << 1; else ar << 0;
	ar << m_crNeutralColor << m_iNeutralStyle << m_iNeutralWidth;

	m_pCpGraph->Serialize(ar);
	m_pPolarGraph->Serialize(ar);
	m_pCzGraph->Serialize(ar);
	m_pCmGraph->Serialize(ar);
	m_pTrGraph->Serialize(ar);
	m_pUserGraph->Serialize(ar);
	m_ViscDlg.m_RmsGraph.Serialize(ar);

	if (m_bShowUserGraph) ar <<1; else ar <<0;

	ar << m_iPlrGraph;
	ar << m_iPlrView;

	ar << m_XPolar << m_YPolar << m_XCz << m_YCz;
	ar << m_XCm << m_YCm << m_XTr << m_YTr << m_XUser << m_YUser;


	ar << m_NRe;
	for (l=0; l< m_NRe; l++)
	{
		ar << m_ReList[l] << m_MachList[l] << m_NCritList[l];
	}

	if(m_bFromList) ar << 1; else ar <<0;
	if(m_bFromZero) ar << 1; else ar <<0;

	ar << m_Adlg.m_MachDef << m_Adlg.m_ReDef << m_Adlg.m_Type << m_ReMax << m_ReInc << m_IterLim ;
	ar << pFrame->m_OperDlgBar.m_Alpha;
	ar << pFrame->m_OperDlgBar.m_AlphaMax;
	ar << pFrame->m_OperDlgBar.m_DeltaAlpha;
	ar << m_IterLim;
	if (m_bShowInviscid)                  ar << 1; else ar <<0;
	if (m_bStoreOpp)                      ar << 1; else ar <<0;
	if(pFrame->m_OperDlgBar.m_bSequence)  ar << 1; else ar <<0;
	if(m_bAlpha)                          ar << 1; else ar <<0;
	if(m_bViscous)                        ar << 1; else ar <<0;
	if(m_bPolar)                          ar << 1; else ar <<0;
	if(m_bPressure)                       ar << 1; else ar <<0;
	if(m_bBL)							  ar << 1; else ar <<0;

	ar << m_Adlg.m_NCrit << m_Adlg.m_XTopTr << m_Adlg.m_XBotTr;
	ar << m_Adlg.m_Reynolds << m_Adlg.m_fMach;

	if(m_bType1)  ar << 1; else ar <<0;
	if(m_bType2)  ar << 1; else ar <<0;
	if(m_bType3)  ar << 1; else ar <<0;
	if(m_bType4)  ar << 1; else ar <<0;

	for (l=0; l<20; l++)
	{
		ar << m_UFOdlg.m_ColSize[l];
	}
	ar << pFrame->m_OperDlgBar.m_Alpha << pFrame->m_OperDlgBar.m_AlphaMax << pFrame->m_OperDlgBar.m_DeltaAlpha;
	ar << pFrame->m_OperDlgBar.m_Cl << pFrame->m_OperDlgBar.m_ClMax << pFrame->m_OperDlgBar.m_DeltaCl;
	ar << pFrame->m_OperDlgBar.m_Re << pFrame->m_OperDlgBar.m_ReMax << pFrame->m_OperDlgBar.m_DeltaRe;

}

void CXDirect::SetColors(Graph *pDefaultGraph)
{
	m_pCpGraph->CopySettings(pDefaultGraph);
	m_pPolarGraph->CopySettings(pDefaultGraph);
	m_pCmGraph->CopySettings(pDefaultGraph);
	m_pCzGraph->CopySettings(pDefaultGraph);
	m_pTrGraph->CopySettings(pDefaultGraph);
	m_pUserGraph->CopySettings(pDefaultGraph);
	m_ViscDlg.m_RmsGraph.CopySettings(pDefaultGraph);
}


void CXDirect::DiscardOpPoints()
{
	m_pCmGraph->DeleteCurves();	
	m_pTrGraph->DeleteCurves();	
	m_pPolarGraph->DeleteCurves();	
	
	int nIndex = (int)m_poaOpp->GetSize();
	for(int i=nIndex-1; i>=0; i--){
		delete m_poaOpp->GetAt(i);
	}
	m_poaOpp->RemoveAll();//removes the pointers
	

	UpdateOpps();
	m_pCurOpp = NULL;
	m_bPolar  = false;
	SetOpp();
	UpdateView();
}


CPolar* CXDirect::GetPolar(CString PlrName)
{
	if (!PlrName.GetLength()) return NULL;
  	CPolar *pPolar;
  	for (int i=0; i<m_poaPolar->GetSize(); i++){
 		pPolar = (CPolar*) m_poaPolar->GetAt(i);
 		if (pPolar->m_FoilName == m_pCurFoil->m_FoilName &&
			pPolar->m_PlrName == PlrName) return pPolar;
	}
  	return NULL;
}


void CXDirect::DrawPolarLegend(CDC *pDC, bool bIsPrinting, CPoint place, int bottom)
{
	CChildView *pChildView = (CChildView*)m_pChildWnd;
	int LegendSize, LegendWidth, ypos, x1;
	COLORREF color;
	int style, width;
	LOGFONT TempFont;

	memcpy(&TempFont, &pChildView->m_WndLogFont, sizeof(LOGFONT));

	if(bIsPrinting) {
		TempFont.lfHeight = TempFont.lfHeight*30;
		LegendSize = 1000;
		LegendWidth = 7000;
		ypos = -400;
//		bottom -= 1000;//margin
	}
	else{ 
		LegendSize = 30;
		LegendWidth = 210;
		ypos = 14;
	}
	CFont ThisFont;
	ThisFont.CreateFontIndirect(&TempFont);

	CStringArray str; // we need to make an inventory of foims
	CPolar * pPolar;
	CFoil *pFoil;
	for (int j=0; j<m_poaFoil->GetSize(); j++){
		pFoil = (CFoil*)m_poaFoil->GetAt(j);
		for (int i=0; i<m_poaPolar->GetSize(); i++){
			pPolar = (CPolar*)m_poaPolar->GetAt(i);
			if (pPolar->m_FoilName == pFoil->m_FoilName){
				str.Add(pFoil->m_FoilName);
				break;
			}
		}// finished inventory
	}

	int nFoils= (int)str.GetSize();

	pDC->SetTextAlign(TA_LEFT);
	pDC->SetBkMode(TRANSPARENT);
	if(IsBlackAndWhite()) pDC->SetTextColor(RGB(0,0,0));
	else				  pDC->SetTextColor(pChildView->m_WndTextColor);

	CFont* pOldFont = pDC->SelectObject(&ThisFont);
	CBrush LegendBrush(pChildView->m_crBackColor);
	CBrush* pOldBrush = pDC->SelectObject(&LegendBrush);

	int ny =0;
	for (int k = 0; k<nFoils; k++){
		int FoilPlrs = 0;
		for (int l=0; l < m_poaPolar->GetSize(); l++){
			pPolar = (CPolar*)m_poaPolar->GetAt(l);
			if (pPolar->m_Alpha.GetSize() &&
				pPolar->m_PlrName.GetLength() &&
				pPolar->m_bIsVisible &&
				pPolar->m_FoilName == str.GetAt(k) &&
				(pPolar->m_Type == 1 && m_bType1 ||
				 pPolar->m_Type == 2 && m_bType2 ||
				 pPolar->m_Type == 3 && m_bType3 ||
				 pPolar->m_Type == 4 && m_bType4))
					FoilPlrs++;
		}
		if (FoilPlrs) {
			int YPos = place.y+ (ny+FoilPlrs+2) * ypos;// bottom line of this foil's legend
			if(abs(bottom) > abs(YPos)){
				ny++;
			}
			else if (k>0) {
			// move rigth if outside screen
				place.x += LegendWidth;
				ny=1;
			}
			pDC->TextOut(place.x + (int)(0.5*LegendSize), place.y + ypos*ny-(int)(ypos/2),
						 str.GetAt(k));
//			pDC->TextOut(place.x , place.y + ypos*ny-(int)(ypos/2), str.GetAt(k));
		}

		for (int nc=0; nc < m_poaPolar->GetSize(); nc++){
			pPolar = (CPolar*)m_poaPolar->GetAt(nc);
			if(str.GetAt(k) == pPolar->m_FoilName){
				if (pPolar->m_Alpha.GetSize() &&
					pPolar->m_PlrName.GetLength() &&
					pPolar->m_bIsVisible &&
					(pPolar->m_Type == 1 && m_bType1 ||
					 pPolar->m_Type == 2 && m_bType2 ||
					 pPolar->m_Type == 3 && m_bType3 ||
					 pPolar->m_Type == 4 && m_bType4)){//is there anything to draw ?

					ny++ ;
					pPolar->GetBWStyle(color, style, width);

					LOGBRUSH lb;
					lb.lbStyle = BS_SOLID;
					lb.lbColor = color;

					CPen LegendPen(PS_GEOMETRIC | style,
								   GetPenWidth(width,bIsPrinting), &lb);

					CPen *pOldPen = pDC->SelectObject(&LegendPen);

					pDC->MoveTo(place.x + (int)(1.0*LegendSize),
								place.y + (int)(1.*ypos*ny));
					pDC->LineTo(place.x + (int)(2.0*LegendSize),
								place.y + (int)(1.*ypos*ny));
					if(pPolar->m_bShowPoints){
						x1 = place.x + (int)(1.5*LegendSize);

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

					pDC->TextOut(place.x + (int)(2.5*LegendSize),
								 place.y + (int)(1.*ypos*ny)-(int)(ypos/2),
								 pPolar->m_PlrName);
					pDC->SelectObject(pOldPen);
				}
			}
		}
		if (FoilPlrs) ny++;
	}
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldFont);
}


void CXDirect::RemoveOpPoint(bool bCurrent)
{
	int i;
	if(bCurrent)
	{
		// we remove only the current OpPoint
		OpPoint* pOpPoint = GetCurOpPoint();
		OpPoint* opt;
		for (i=0; i<m_poaOpp->GetSize(); i++)
		{
			opt =(OpPoint*)m_poaOpp->GetAt(i);
			if (pOpPoint == opt)
			{
				m_poaOpp->RemoveAt(i);
				delete pOpPoint;
				break;
			}
		}
	}
	else
	{
		// kill'em all
		OpPoint* pOpPoint;
		for (i=(int)m_poaOpp->GetSize()-1; i>=0;i--)
		{
			pOpPoint =(OpPoint*)m_poaOpp->GetAt(i);
			if (pOpPoint->m_strFoilName == m_pCurFoil->m_FoilName &&
				pOpPoint->m_strPlrName == m_pCurPolar->m_PlrName)
			{
				m_poaOpp->RemoveAt(i);
				delete pOpPoint;
			}
		}
	}
	m_pCurOpp = NULL;
}




void CXDirect::OnAdvSettings() 
{
	CXFoilAdvDlg	dlg;
	dlg.m_fVAccel  = m_pXFoil->vaccel;
	dlg.m_iIterLim = m_IterLim;
	dlg.m_bAutoInitBL = m_bAutoInitBL;

	if (IDOK == dlg.DoModal()){
		m_pXFoil->vaccel = dlg.m_fVAccel;
		m_IterLim        = dlg.m_iIterLim;
		m_bAutoInitBL    = dlg.m_bAutoInitBL;
	}	
}


void CXDirect::OnPanel() 
{
	if(!m_pCurFoil)	return;
	
	bool bState = m_bShowPanels;//save current view setting

	void* ptr = m_pCurOpp;
	m_pCurOpp = NULL;
	CreateOppCurves();
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->m_OperDlgBar.SetAnimate(false);	
	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	m_bPressure = false;
	m_bBL       = false;
	OnOper();

	C2DPanelDlg Pdlg;
	Pdlg.m_pChildView  = m_pChildWnd;
	Pdlg.m_pBufferFoil = &m_BufferFoil;
	Pdlg.m_pMemFoil    = m_pCurFoil;
	Pdlg.m_pXFoil      = m_pXFoil;
	Pdlg.Title         = "Refine "+ m_pCurFoil->m_FoilName;

	m_bShowPanels = true;
	UpdateView();


	if(IDOK == Pdlg.DoModal())
	{
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		m_pCurOpp = (OpPoint*)ptr;
		pFrame->SetModFoil(pNewFoil);
		UpdateFoils();
	}
	else
	{
		//reset everything
		m_pCurOpp = (OpPoint*)ptr;
		SetBufferFoil();
		InitXFoil();
	}

	m_bShowPanels = bState;//restore as it was
	m_bPressure   = bPressure;
	m_bBL         = bBL;
	UpdateView();
}

void CXDirect::OnEditCoord() 
{
	if(!m_pCurFoil)	return;
	
	bool bState = m_bShowPanels;//save current view setting

	void* ptr = m_pCurOpp;
	m_pCurOpp = NULL;
	CreateOppCurves();
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->m_OperDlgBar.SetAnimate(false);	
	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	m_bPressure = false;
	m_bBL       = false;
	OnOper();

	bool bFlap       = m_BufferFoil.m_bTEFlap;
	double FlapAngle = m_BufferFoil.m_TEFlapAngle;
	double Xh        = m_BufferFoil.m_TEXHinge;
	double Yh        = m_BufferFoil.m_TEXHinge;

	m_BufferFoil.m_bTEFlap = false;
//	SetFoilFlap(&m_BufferFoil);

	CEditFoilDlg Fdlg;
	Fdlg.m_pChildView  = m_pChildWnd;
	Fdlg.m_pBufferFoil = &m_BufferFoil;
	Fdlg.m_pMemFoil    = m_pCurFoil;

	m_bShowPanels = true;
	UpdateView();
	
	CString OldFoilName = m_pCurFoil->m_FoilName;

	if(IDOK == Fdlg.DoModal())
	{
		m_BufferFoil.m_bTEFlap = bFlap;
		m_BufferFoil.m_TEFlapAngle = FlapAngle;
		m_BufferFoil.m_TEXHinge = Xh;
		m_BufferFoil.m_TEYHinge = Yh;

		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		m_pCurOpp = (OpPoint*)ptr;
		pFrame->SetModFoil(pNewFoil);
		UpdateFoils();
	}

	else
	{
		//reset everything
		m_pCurOpp = (OpPoint*)ptr;
		m_BufferFoil.m_bTEFlap = bFlap;
		m_BufferFoil.m_TEFlapAngle = FlapAngle;
		m_BufferFoil.m_TEXHinge = Xh;
		m_BufferFoil.m_TEYHinge = Yh;
//		SetFoilFlap(&m_BufferFoil);
		SetBufferFoil();
		InitXFoil();
	}

	m_bShowPanels = bState;//restore as it was

	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();

}

void CXDirect::OnTEGap() 
{
	if(!m_pCurFoil)	return;

	void* ptr = m_pCurOpp;
	m_pCurOpp = NULL;
	CreateOppCurves();
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->m_OperDlgBar.SetAnimate(false);	
	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	bool bState = m_bShowPanels;
	m_bPressure = false;
	m_bBL       = false;
	OnOper();

	CTEGapDlg Gdlg;
	Gdlg.m_pChildView   = m_pChildWnd;
	Gdlg.m_pBufferFoil = &m_BufferFoil;
	Gdlg.m_pMemFoil    = m_pCurFoil;
	Gdlg.m_pXFoil      = m_pXFoil;
	Gdlg.m_Gap         = m_pCurFoil->m_Gap;
	

	if(IDOK == Gdlg.DoModal())
	{
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		m_pCurOpp = (OpPoint*)ptr;
		pFrame->SetModFoil(pNewFoil);
		UpdateFoils();
	}

	else
	{
		//reset everything
		m_pCurOpp = (OpPoint*)ptr;
		SetBufferFoil();
		InitXFoil();
	}

	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();
		
}


void CXDirect::OnLERad() 
{
	if(!m_pCurFoil)	return;

	void* ptr = m_pCurOpp;
	m_pCurOpp = NULL;
	CreateOppCurves();
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->m_OperDlgBar.SetAnimate(false);	
	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	bool bState = m_bShowPanels;
	m_bPressure = false;
	m_bBL       = false;
	OnOper();

	CLEDlg Ldlg(this);
	Ldlg.m_pBufferFoil = &m_BufferFoil;
	Ldlg.m_pMemFoil    = m_pCurFoil;
	Ldlg.m_pXFoil      = m_pXFoil;
	Ldlg.m_pChildView  = m_pChildWnd;
	
	if(IDOK == Ldlg.DoModal())
	{
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		m_pCurOpp = (OpPoint*)ptr;
		pFrame->SetModFoil(pNewFoil);
		UpdateFoils();
	}

	else
	{
		//reset everything
		m_pCurOpp = (OpPoint*)ptr;
		SetBufferFoil();
		InitXFoil();
	}

	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();
	
}



void CXDirect::OnInterpolate() 
{
	if(m_poaFoil->GetSize()<2)
	{
		AfxMessageBox("At least two foils are required", MB_OK);
		return;
	}
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->m_OperDlgBar.SetAnimate(false);	
	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	m_bPressure = false;
	m_bBL       = false;
	OnOper();

	CInterpolateDlg dlg(this);
	dlg.m_pChildView  = m_pChildWnd;
	dlg.m_poaFoil     = m_poaFoil;
	dlg.m_pXFoil      = m_pXFoil;
	dlg.m_pBufferFoil = &m_BufferFoil;// work on the buffer foil

	if(dlg.DoModal() ==IDOK)
	{
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilName = dlg.m_NewFoilName;
		pFrame->SetModFoil(pNewFoil);
		UpdateFoils();
	}
	else
	{
		SetBufferFoil();// restore buffer foil.. from current foil
		InitXFoil();
	}
	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();
}


void CXDirect::OnSetFlap() 
{
	if(!m_pCurFoil) return;

	void* ptr = m_pCurOpp;
	m_pCurOpp = NULL;
	CreateOppCurves();
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->m_OperDlgBar.SetAnimate(false);	
	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	m_bPressure = false;
	m_bBL       = false;
	OnOper();

	CFlapDlg dlg;
	dlg.m_pBufferFoil  = &m_BufferFoil;
	dlg.m_pMemFoil     = m_pCurFoil;
	dlg.m_pXFoil       = m_pXFoil;
	dlg.m_pChildView   = m_pChildWnd;

	if(IDOK == dlg.DoModal())
	{
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		m_pCurOpp = (OpPoint*)ptr;
		pFrame->SetModFoil(pNewFoil);
		UpdateFoils();
	}
	else
	{
		//reset everything
		m_pCurOpp = (OpPoint*)ptr;
		SetBufferFoil();
		InitXFoil();
	}

	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();
	
}


void CXDirect::OnCadd() 
{

	if(!m_pCurFoil){
		return;
	}
	
	void* ptr = m_pCurOpp;
	m_pCurOpp = NULL;
	CreateOppCurves();

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->m_OperDlgBar.SetAnimate(false);
	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	bool bState = m_bShowPanels;
	m_bPressure = false;
	m_bBL       = false;
	OnOper();
	
	CCAddDlg Adlg;
	Adlg.m_pChildView     = m_pChildWnd;
	Adlg.m_pBufferFoil = &m_BufferFoil;
	Adlg.m_pMemFoil    = m_pCurFoil;
	Adlg.m_pXFoil      = m_pXFoil;
	Adlg.Title         = "Refine "+ m_pCurFoil->m_FoilName;

	m_bShowPanels = true;
	UpdateView();

	if(IDOK == Adlg.DoModal())
	{
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		m_pCurOpp = (OpPoint*)ptr;
		pFrame->SetModFoil(pNewFoil);
		UpdateFoils();
	}
	else
	{
		m_pCurOpp = (OpPoint*)ptr;
		SetBufferFoil();
		InitXFoil();
	}

	m_bShowPanels = bState;

	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();
}


void CXDirect::OnDeRotateFoil() 
{
	if(!m_pCurFoil) return;
	CString str;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	double angle = m_pCurFoil->DeRotate();
	SetBufferFoil();
	pFrame->SetSaveState(false);
	str.Format("Foil has been de-rotated by %6.3f degrees", angle);
	pFrame->m_wndStatusBar.SetWindowText(str);

	UpdateView();
}


void CXDirect::OnExportFoil() 
{
	if(!m_pCurFoil)	return;

	CString FileName, DestFileName, OutString;
	CStdioFile DestFile;
	CFileException fe;

	FileName = m_pCurFoil->m_FoilName;
	FileName.Replace("/", " ");
	static TCHAR BASED_CODE szFilter[] = _T("Labeled Format (*.dat)|*.dat|") _T("VisuAero format (*.dat)|*.dat|") ;
	CFileDialog XFileDlg(false, "txt", FileName, OFN_OVERWRITEPROMPT, szFilter);

	if(IDOK==XFileDlg.DoModal()) 
	{
		DestFileName = XFileDlg.GetPathName();
		int FilterIndex = XFileDlg.m_ofn.nFilterIndex;
		CFoil *pFoil = GetFoil(m_pCurFoil->m_FoilName);
		if(pFoil) 
		{
			if(FilterIndex==1)	
				pFoil->ExportFoil(DestFileName);
			else 
			{
				//writing ViSuAero Format, special for... me
				BOOL bOpen2 = DestFile.Open(DestFileName, CFile::modeCreate | CFile::modeWrite, &fe);
				OutString ='"' ;
				OutString +=m_pCurFoil->m_FoilName;
				OutString +='"';
				OutString +="\n";
				DestFile.WriteString(OutString);

				OutString.Format("%.2f, %.2f, %.2f, %.2f, 0.0, 0.0\n",
								 m_pCurFoil->m_fThickness*100.0,
								 m_pCurFoil->m_fXThickness*100.0,
								 m_pCurFoil->m_fCamber*100.0,
								 m_pCurFoil->m_fXCamber*100.0);

				DestFile.WriteString(OutString);

				for (int i=0; i<m_pCurFoil->n; i++){
					OutString.Format("%8.4f, %8.4f\n", m_pCurFoil->x[i]*100.0, m_pCurFoil->y[i]*100.0); 
					DestFile.WriteString(OutString);
				}

				
				DestFile.Close();
			}
		}
	}
}


void CXDirect::OnAllFour() 
{
	m_bPolar = true;
	m_iPlrGraph = 0;
	m_iPlrView  = 0;
	CheckMenu();
	UpdateView();
}

void CXDirect::OnCoupleGraph() 
{
	m_bPolar = true;
	m_iPlrGraph = 0;
	m_iPlrView = 2;
	CheckMenu();
	UpdateView();
}

void CXDirect::OnClCd() 
{
	m_bPolar = true;
	m_pCurGraph = m_pPolarGraph;
	m_iPlrGraph = 1;
	m_iPlrView  = 1;
	m_XPolar     = 2;
	m_YPolar     = 1;
	SetTitles(m_pCurGraph, 2, 1);
	CreatePolarCurves();
	CheckMenu();
	UpdateView();
}

void CXDirect::OnClAlpha() 
{
	m_bPolar = true;
	m_pCurGraph = m_pCzGraph;
	m_iPlrGraph = 2;
	m_iPlrView  = 1;
	m_XCz        = 0; 
	m_YCz        = 1;
	SetTitles(m_pCurGraph, 0, 1);
	CreatePolarCurves();
	CheckMenu();
	UpdateView();
}

void CXDirect::OnClXtr() 
{
	m_bPolar = true;
	m_pCurGraph = m_pTrGraph;	
	m_iPlrGraph = 3;
	m_iPlrView  = 1;
	m_XTr        = 6;
	m_YTr        = 1;
	SetTitles(m_pCurGraph, 6, 1);
	CreatePolarCurves();
	CheckMenu();
	UpdateView();
}

void CXDirect::OnCmAlpha()
{
	m_bPolar = true;
	m_pCurGraph = m_pCmGraph;
	m_iPlrGraph = 4;
	m_iPlrView  = 1;
	m_XCm        = 0;
	m_YCm        = 5;
	SetTitles(m_pCurGraph, 0, 5);
	CreatePolarCurves();
	CheckMenu();
	UpdateView();
}


void CXDirect::OnUserGraph() 
{
	m_bPolar = true;
	m_pCurGraph = m_pUserGraph;
	m_iPlrGraph = 5;
	m_iPlrView  = 1;
	CreatePolarCurves();
	CheckMenu();
	UpdateView();
}



void CXDirect::OnContextMenu(CPoint ScreenPoint, CPoint ClientPoint) 
{
	m_pCurGraph = GetGraph(ClientPoint);

	if(!m_rCltRect.PtInRect(ClientPoint))return;
	if(m_bPolar){
		if (m_iPlrView==1){// we have only a single graph
//			if(m_pCurGraph && m_pCurGraph->IsInDrawRect(ClientPoint)){
				CMenu menu;
				if (menu.LoadMenu(IDR_CTXXDIRECTPLRMENU)){
					CMenu* pPopup = menu.GetSubMenu(0);
					ASSERT(pPopup != NULL);
					
					if (m_iPlrGraph == 1) pPopup->CheckMenuItem(IDM_XFLR5_CLCD,MF_BYCOMMAND | MF_CHECKED);
					if (m_iPlrGraph == 2) pPopup->CheckMenuItem(IDM_XFLR5_CLALPHA,MF_BYCOMMAND | MF_CHECKED);
					if (m_iPlrGraph == 3) pPopup->CheckMenuItem(IDM_XFLR5_CLXTR,MF_BYCOMMAND | MF_CHECKED);
					if (m_iPlrGraph == 4) pPopup->CheckMenuItem(IDM_XFLR5_CMALPHA,MF_BYCOMMAND | MF_CHECKED);
					if (m_iPlrGraph == 5) pPopup->CheckMenuItem(IDM_XFLR5_USERGRAPH,MF_BYCOMMAND | MF_CHECKED);
					if (m_bShowUserGraph) pPopup->CheckMenuItem(IDM_XFLR5_SHOWUSERGRAPH,MF_BYCOMMAND | MF_CHECKED);
					

					pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
						ScreenPoint.x, ScreenPoint.y, m_pFrame); // use main window for cmds
				}
//			}
		}
		else if (m_iPlrView==2){// we have two graphs
			bool bInGraph = false;
			if(m_pPolarGraph->IsInDrawRect(ClientPoint)){
				m_pCurGraph = m_pPolarGraph;
				bInGraph = true;
			}
			else if(m_pUserGraph->IsInDrawRect(ClientPoint)){
				m_pCurGraph = m_pUserGraph;
				bInGraph = true;
			}
			else m_pCurGraph = NULL;
//			if(bInGraph){
				CMenu menu;
				if (menu.LoadMenu(IDR_CTXXDIRECTPLRMENU)){
					CMenu* pPopup = menu.GetSubMenu(0);
					ASSERT(pPopup != NULL);

					pPopup->CheckMenuItem(IDM_XFLR5_COUPLEGRAPH,MF_BYCOMMAND | MF_CHECKED);					

					pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
						ScreenPoint.x, ScreenPoint.y, m_pFrame); // use main window for cmds
				}
//			}
		}
		else{// we have them all
			bool bInGraph = false;
			if(m_pPolarGraph->IsInDrawRect(ClientPoint)){
				m_pCurGraph = m_pPolarGraph;
				bInGraph = true;
			}
			else if(m_pCmGraph->IsInDrawRect(ClientPoint)){
				m_pCurGraph = m_pCmGraph;
				bInGraph = true;
			}
			else if(m_pCzGraph->IsInDrawRect(ClientPoint)){
				m_pCurGraph = m_pCzGraph;
				bInGraph = true;
			}
			else if(m_pTrGraph->IsInDrawRect(ClientPoint)){
				m_pCurGraph = m_pTrGraph;
				bInGraph = true;
			}
			else if(m_pUserGraph->IsInDrawRect(ClientPoint)){
				m_pCurGraph = m_pUserGraph;
				bInGraph = true;
			}
			else m_pCurGraph = NULL;
//			if(bInGraph){
				CMenu menu;
				if (menu.LoadMenu(IDR_CTXXDIRECTPLRMENU)){
					CMenu* pPopup = menu.GetSubMenu(0);
					ASSERT(pPopup != NULL);
					pPopup->CheckMenuItem(IDM_ALLFOUR,MF_BYCOMMAND | MF_CHECKED);

					if (m_bShowUserGraph) pPopup->CheckMenuItem(IDM_XFLR5_SHOWUSERGRAPH,MF_BYCOMMAND | MF_CHECKED);
					else pPopup->CheckMenuItem(IDM_XFLR5_SHOWUSERGRAPH, MF_BYCOMMAND | MF_UNCHECKED);
					
					pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
						ScreenPoint.x, ScreenPoint.y, m_pFrame); // use main window for cmds
				}
//			}
		}
	}
	else if (m_pCurGraph){
		CMenu menu;
		if (menu.LoadMenu(IDR_CTXXDIRECTFOILMENU)){
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);

			if(m_pCurGraph == m_pCpGraph){
				if(m_OppVar==0)	pPopup->CheckMenuItem(IDM_XFLR5_CPGRAPH, MF_BYCOMMAND | MF_CHECKED);
				else pPopup->CheckMenuItem(IDM_XFLR5_QGRAPH, MF_BYCOMMAND | MF_CHECKED);
			}
			
			if(m_bShowPanels){
				pPopup->CheckMenuItem(IDM_SHOWPANELS, MF_BYCOMMAND | MF_CHECKED);
			}
			if(!m_pCurPolar) {
				pPopup->EnableMenuItem(4,MF_BYPOSITION | MF_GRAYED);
			}

			if(m_bCurOppOnly) pPopup->CheckMenuItem(IDM_SHOWCUROPP, MF_BYCOMMAND | MF_CHECKED);
			
			if(m_bShowInviscid) pPopup->CheckMenuItem(IDM_CPI, MF_BYCOMMAND | MF_CHECKED);

			if(m_bCpGraph) pPopup->CheckMenuItem(IDM_SHOWCPGRAPH, MF_BYCOMMAND  | MF_CHECKED);

			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				ScreenPoint.x, ScreenPoint.y, m_pFrame); // use main window for cmds
		}
	}
}




void CXDirect::OnShowUserGraph() 
{
	m_bShowUserGraph = !m_bShowUserGraph;
	CheckMenu();
	UpdateView();
}


BOOL CXDirect::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
//	point is in client coordinates
//	if(m_rCltRect.PtInRect(pt)) SetFocus();
//	pt.y -= m_dz;

	m_pCurGraph = GetGraph(pt);
	if(m_pCurGraph && m_pCurGraph->IsInDrawRect(pt) && m_bCpGraph){
		SHORT shX = GetKeyState('X');
		SHORT shY = GetKeyState('Y');

		if (shX & 0x8000){
			//zoom x scale
			m_pCurGraph->SetAutoX(false);
			if(zDelta>0) m_pCurGraph->Scalex(1.06);
			else m_pCurGraph->Scalex(1.0/1.06);
		}
		else if(shY & 0x8000){
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
	
		if(!m_bAnimate) UpdateView();
	}
	else if(m_pCurFoil && !m_bPolar){
		double scale = m_fFoilScale;

		if(zDelta>0) m_fFoilScale *= 1.06;
		else         m_fFoilScale /= 1.06;

		int a = (int)((m_rDrawRect.right+m_rDrawRect.left)/2);

		m_ptOffset.x = a + (int)((m_ptOffset.x-a)*scale/m_fFoilScale);

		if(!m_bAnimate) UpdateView();
	}
	return TRUE;
}


void CXDirect::OnMouseMove(UINT nFlags, CPoint point) 
{

	SHORT shZ  = GetKeyState(90);
	if ((nFlags & MK_LBUTTON) && m_bTrans)
	{
		m_pCurGraph = GetGraph(point);
		if(m_pCurGraph && m_bTransGraph)
		{ 
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

			m_PointDown.x = point.x;
			m_PointDown.y = point.y;
			if(!m_bAnimate) UpdateView();
		}
		else if (m_pCurFoil  && !m_bPolar)
		{
			// we translate the airfoil
			
			m_ptOffset.x += point.x - m_PointDown.x;
			m_ptOffset.y += point.y - m_PointDown.y;

			m_PointDown.x = point.x;
			m_PointDown.y = point.y;
			if(!m_bAnimate) UpdateView();
		}
	}

	else if (m_pCurFoil && ((nFlags & MK_MBUTTON) || (shZ & 0x8000)))
	{// we zoom the graph or the foil		
		m_pCurGraph = GetGraph(point);
		if(m_pCurGraph && m_pCurGraph->IsInDrawRect(point) && m_bCpGraph)
		{ 
			//zoom graph

			SHORT shX = GetKeyState('X');
			SHORT shY = GetKeyState('Y');

			if (shX & 0x8000)
			{
				//zoom x scale
				m_pCurGraph->SetAutoX(false);
				m_pCurGraph->SetAutoX(false);
				if(point.x-m_PointDown.x<0) m_pCurGraph->Scalex(1.04);
				else						m_pCurGraph->Scalex(1.0/1.04);
			}
			else if(shY & 0x8000)
			{
				//zoom y scale
				m_pCurGraph->SetAutoY(false);
				m_pCurGraph->SetAutoY(false);
				if(point.y-m_PointDown.y<0) m_pCurGraph->Scaley(1.04);
				else						m_pCurGraph->Scaley(1.0/1.04);
			}
			else
			{
				//zoom both
				m_pCurGraph->SetAuto(false);
				if(point.y-m_PointDown.y<0) m_pCurGraph->Scale(1.06);
				else						m_pCurGraph->Scale(1.0/1.06);
			}

			if(!m_bAnimate) UpdateView();
		}
		else if(m_pCurFoil && !m_bPolar){//zoom foil
//			double scale = m_fFoilScale;

			if(point.y-m_PointDown.y<0) m_fFoilScale *= 1.06;
			else						m_fFoilScale /= 1.06;

//			int a = (int)((m_rDrawRect.right+m_rDrawRect.left)/2);
//
//			m_ptOffset.x = a + (int)((m_ptOffset.x-a)*scale/m_fFoilScale);

			if(!m_bAnimate) UpdateView();
		}
		m_PointDown = point;
	}
//	CDialog::OnMouseMove(nFlags, point);
}


void CXDirect::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_rCltRect.PtInRect(point)) m_pChildWnd->SetFocus();
	m_pCurGraph = GetGraph(point);

	if (m_pCurFoil || (m_pCurGraph && m_pCurGraph->IsInDrawRect(point) && m_bCpGraph)){
		m_PointDown.x = point.x;
		m_PointDown.y = point.y;

		m_bTrans = true;
		SetCursor(m_hcMove);
		if (m_pCurGraph && m_pCurGraph->IsInDrawRect(point))
			m_bTransGraph = true;
		else 
			m_bTransGraph = false;
		if(!m_bAnimate) UpdateView();
	}
}

void CXDirect::OnLButtonDblClk(UINT nFlags, CPoint point)
{
//	point is in client coordinates
	m_pCurGraph = GetGraph(point);

	if (m_pCurGraph && m_pCurGraph->IsInDrawRect(point) && m_bCpGraph){
		CGraphDlg dlg(m_pChildWnd);

		dlg.m_pGraph = m_pCurGraph;

		if(IDOK == dlg.DoModal()){
			UpdateView();
		}
	}	
}

void CXDirect::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (GetCapture() == this) ReleaseCapture();	
	m_bTrans = false;
	SetCursor(m_hcArrow);
	
//	CDialog::OnLButtonUp(nFlags, point);
}


Graph* CXDirect::GetGraph(CPoint &pt){
	//pt is in client coordinates
	if (m_bPolar)
	{

		if(m_iPlrView == 1) return m_pCurGraph;// no need for change
		if(m_iPlrView == 2)
		{
			if(m_pPolarGraph->IsInDrawRect(pt)){return m_pPolarGraph;}
			if(m_pUserGraph->IsInDrawRect(pt)){return m_pUserGraph;}
			return NULL;
		}

		if(m_pPolarGraph->IsInDrawRect(pt)){return m_pPolarGraph;}
		if(m_pCzGraph->IsInDrawRect(pt)){return m_pCzGraph;}
		if(m_pCmGraph->IsInDrawRect(pt)){return m_pCmGraph;}
		if(m_pTrGraph->IsInDrawRect(pt)){return m_pTrGraph;}
		if(m_pUserGraph->IsInDrawRect(pt)){return m_pUserGraph;}
	}
	else 
	{
		if(m_bCpGraph) return m_pCpGraph;
		else           return NULL;
	}
	return NULL;
}


void CXDirect::OnResetGraph() 
{
	if(m_pCurGraph == m_pCpGraph)
	{
		m_pCurGraph->SetAuto(true);
		m_pCpGraph->SetXTitle("X");
		m_pCpGraph->SetXMin(0.0);
		m_pCpGraph->SetXMax(1.0);
		m_pCpGraph->SetYMin(-0.1);
		m_pCpGraph->SetYMax(0.1);
		if(m_OppVar==0)
		{
			m_pCpGraph->SetYTitle("Cp");
			m_pCpGraph->SetInverted(true);
		}
		else
		{
			m_pCpGraph->SetInverted(false);
		}
		m_pCurGraph->Init();
		UpdateView();
	}
	else if(m_pCurGraph)
	{
		m_pCurGraph->SetAuto(true);
		m_pCurGraph->ResetXLimits();
		m_pCurGraph->ResetYLimits();
		UpdateView();
	}
	else return;
}

void CXDirect::OnResetAllScales() 
{
	m_pPolarGraph->SetAuto(true);
	m_pPolarGraph->ResetXLimits();
	m_pPolarGraph->ResetYLimits();
	m_pCzGraph->SetAuto(true);
	m_pCzGraph->ResetXLimits();
	m_pCzGraph->ResetYLimits();
	m_pTrGraph->SetAuto(true);
	m_pTrGraph->ResetXLimits();
	m_pTrGraph->ResetYLimits();
	m_pCmGraph->SetAuto(true);
	m_pCmGraph->ResetXLimits();
	m_pCmGraph->ResetYLimits();
	m_pUserGraph->SetAuto(true);
	m_pUserGraph->ResetXLimits();
	m_pUserGraph->ResetYLimits();
	UpdateView();		
}


void CXDirect::OnPolars() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	m_bPolar = true;
	pFrame->m_OperDlgBar.SetAnimate(false);
	pFrame->m_PolarDlgBar.SetParams();

	CreatePolarCurves();

	CToolBarCtrl *pTB = &(pFrame->m_XDirectBar.GetToolBarCtrl());
	pTB->PressButton(IDT_OPPS, !m_bPolar);
	pTB->PressButton(IDT_POLARS, m_bPolar);

	CMenu* pMenu = m_pFrame->GetMenu()->GetSubMenu(2);
	pMenu->CheckMenuItem(IDM_XFLR5_POLARS, MF_BYCOMMAND | MF_CHECKED);
	pMenu->CheckMenuItem(IDM_XFLR5_OPER,   MF_BYCOMMAND | MF_UNCHECKED);

	UpdateView();
}


void CXDirect::OnSingleAnalysis()
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	if(!m_pCurFoil) {
		AfxMessageBox("No foil available for analysis", MB_OK);	
		return;
	}

	m_Adlg.m_pParent = this;
	m_Adlg.m_FoilName = m_pCurFoil->m_FoilName;

	if (m_Adlg.DoModal() == IDOK){
		m_pCurPolar = new CPolar(m_pFrame);
		m_pCurPolar->m_FoilName = m_pCurFoil->m_FoilName;
		m_pCurPolar->m_PlrName = m_Adlg.m_PlrName;
		m_pCurPolar->m_bIsVisible = true;
		m_pCurPolar->m_Type = m_Adlg.m_Type;
		switch (m_pCurPolar->m_Type){
			case 1:
				m_pCurPolar->m_MaType = 1;
				m_pCurPolar->m_ReType = 1;
				break;
			case 2:
				m_pCurPolar->m_MaType = 2;
				m_pCurPolar->m_ReType = 2;
				break;
			case 3:
				m_pCurPolar->m_MaType = 1;
				m_pCurPolar->m_ReType = 3;
				break;
			case 4:
				m_pCurPolar->m_MaType = 1;
				m_pCurPolar->m_ReType = 1;
				break;
			default:
				m_pCurPolar->m_ReType = 1;
				m_pCurPolar->m_MaType = 1;
				break;
		}

		m_pCurPolar->m_Reynolds = m_Adlg.m_Reynolds;
		m_pCurPolar->m_Mach     = m_Adlg.m_fMach;
		m_pCurPolar->m_ASpec    = m_Adlg.m_ASpec;
		m_pCurPolar->m_ACrit    = m_Adlg.m_NCrit;
		m_pCurPolar->m_XTop     = m_Adlg.m_XTopTr;
		m_pCurPolar->m_XBot     = m_Adlg.m_XBotTr;
		m_pCurPolar->m_Color = pFrame->GetColor(1);

		m_pCurPolar = AddPolar(m_pCurPolar);


		CPolar *pPolar;
		bool bFound;
		for(int i=0; i<12;i++){
			bFound = false;
			for (int j=0; j<m_poaPolar->GetSize();j++){
				pPolar = (CPolar*)m_poaPolar->GetAt(j);
				if(pPolar->m_Color == pFrame->m_crColors[i]) bFound = true;
			}
			if(!bFound) {
				m_pCurPolar->m_Color = pFrame->m_crColors[i];
				break;
			}
		}
		
		SetPolar();
		UpdatePlrs();

		SetBufferFoil();

		UpdateView();

		CMainFrame* pFrame = (CMainFrame*)m_pFrame;
		pFrame->SetSaveState(false);
		pFrame->m_OperDlgBar.m_ctrlAnalyze.SetFocus();
	}	
}

void CXDirect::OnBatchAnalysis() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	if(!m_pCurFoil) {
		AfxMessageBox("No foil available for analysis", MB_OK);	
		return;
	}

	CBatchDlg BDlg(this);
	BDlg.m_pFrame    = m_pFrame;
	BDlg.m_pXFoil    = m_pXFoil;
	BDlg.m_pCurFoil  = m_pCurFoil;
	BDlg.m_Mach      = m_Adlg.m_MachDef;
	BDlg.m_ReMin     = m_Adlg.m_ReDef;
	BDlg.m_ReMax     = m_ReMax;
	BDlg.m_ReInc     = m_ReInc;
	BDlg.m_Type      = m_Adlg.m_Type;
	BDlg.m_IterLim   = m_IterLim;
	BDlg.m_FoilName  = m_pCurFoil->m_FoilName;
	BDlg.m_bAlpha    = m_bAlpha;
	BDlg.m_SpMin     = pFrame->m_OperDlgBar.m_Alpha;
	BDlg.m_SpMax     = pFrame->m_OperDlgBar.m_AlphaMax;
	BDlg.m_SpInc     = pFrame->m_OperDlgBar.m_DeltaAlpha;
	BDlg.m_NCrit     = m_Adlg.m_NCrit;
	BDlg.m_XTopTr    = m_Adlg.m_XTopTr;
	BDlg.m_XBotTr    = m_Adlg.m_XBotTr;
	BDlg.m_ReList    = m_ReList;
	BDlg.m_MachList  = m_MachList;
	BDlg.m_NCritList = m_NCritList;
	BDlg.m_NRe       = m_NRe;
	BDlg.m_bFromList = m_bFromList;
	BDlg.m_bFromZero = m_bFromZero;
	BDlg.m_bShowTextOutput = m_bShowTextOutput;
	BDlg.m_RmsGraph.CopySettings(&m_ViscDlg.m_RmsGraph);

	int resp = (int)BDlg.DoModal();
	pFrame->SetSaveState(false);

	m_Adlg.m_ReDef     = BDlg.m_ReMin;
	m_ReMax            = BDlg.m_ReMax;
	m_ReInc            = BDlg.m_ReInc;
	m_Adlg.m_MachDef   = BDlg.m_Mach;
	m_Adlg.m_Type      = BDlg.m_Type;
	m_Adlg.m_NCrit     = BDlg.m_NCrit;
	m_Adlg.m_XTopTr    = BDlg.m_XTopTr;
	m_Adlg.m_XBotTr    = BDlg.m_XBotTr;
	m_NRe              = BDlg.m_NRe;
	m_bAlpha           = BDlg.m_bAlpha;
	m_bFromList        = BDlg.m_bFromList;
	m_bFromZero        = BDlg.m_bFromZero;
	m_bShowTextOutput  = BDlg.m_bShowTextOutput;

	UpdatePlrs();
	if (resp !=IDCANCEL){
//		OnPolars();
		m_pCurOpp = NULL;
	}
	CreatePolarCurves();

	pFrame->m_OperDlgBar.m_Alpha      = BDlg.m_SpMin ;
	pFrame->m_OperDlgBar.m_AlphaMax   = BDlg.m_SpMax ;
	pFrame->m_OperDlgBar.m_DeltaAlpha = BDlg.m_SpInc ;
	SetPolar();
	if(m_pCurPolar){
		if(m_pCurPolar->m_Type ==4){
			pFrame->m_OperDlgBar.CheckRadioButton(IDC_PARAM1,IDC_PARAM3,IDC_PARAM3);
		}
		else{
			if(m_bAlpha) pFrame->m_OperDlgBar.CheckRadioButton(IDC_PARAM1,IDC_PARAM3,IDC_PARAM1);
			else         pFrame->m_OperDlgBar.CheckRadioButton(IDC_PARAM1,IDC_PARAM3,IDC_PARAM2);
		}
	}
	UpdateView();
}



void CXDirect::SetDisplayDefault()
{
	m_iFoilStyle = 0;
	m_iFoilWidth = 1;
	m_crFoilColor  = RGB(120,120,120);

	m_iBLStyle = PS_DASH;
	m_iBLWidth = 1;
	m_crBLColor = RGB(200,50,50);

	m_iPressureStyle = PS_SOLID;
	m_iPressureWidth = 1;
	m_crPressureColor = RGB(50,200,50);

	m_bNeutralLine = true;
	m_iNeutralStyle = PS_DASH;
	m_iNeutralWidth = 1;
	m_crNeutralColor = RGB(150,150,150);

	m_pCpGraph->SetDefaults();
	m_pCpGraph->SetInverted(true);
	m_pCpGraph->SetXMin(0.0);
	m_pCpGraph->SetXMax(1.0);
	m_pCpGraph->SetYMin(-0.1);
	m_pCpGraph->SetYMax(0.1);

	m_pPolarGraph->SetDefaults();
	m_pCmGraph->SetDefaults();
	m_pCzGraph->SetDefaults();
	m_pTrGraph->SetDefaults();
	m_pUserGraph->SetDefaults();
}


void CXDirect::DeleteFoil(bool bAsk)
{
	if(!m_pCurFoil || !m_pCurFoil->m_FoilName.GetLength()) return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	if(pFrame->DeleteFoil(m_pCurFoil, bAsk))
	{
		m_pCurOpp = NULL;
		m_pCurPolar = NULL;
		m_pCurFoil = NULL;
		if(m_bPolar) CreatePolarCurves();
		else         CreateOppCurves();
		UpdateFoils();
		UpdateView();
	}
}


void CXDirect::OnExportPlr() 
{
	if(!m_pCurPolar) return;
	CMainFrame *pMainFrame = (CMainFrame*)m_pFrame;
	CStdioFile XFile;
	CFileException fe;
	CString FileName;
	
	FileName = m_pCurPolar->m_PlrName;
	FileName.Replace("/", " ");

	static TCHAR BASED_CODE szFilter[] = _T("Text File (*.txt)|*.txt|") _T("CSV format (*.csv)|*.csv|") ;
	CFileDialog XFileDlg(false, "txt", FileName, OFN_OVERWRITEPROMPT, szFilter);
	XFileDlg.m_ofn.nFilterIndex = pMainFrame->m_TextFileFormat;

	if(IDOK == XFileDlg.DoModal())
	{
		pMainFrame->m_TextFileFormat = XFileDlg.m_ofn.nFilterIndex;
		FileName = XFileDlg.GetPathName();
		m_pCurPolar->Export(FileName , XFileDlg.m_ofn.nFilterIndex);
	}
}



void * CXDirect::GetVariable(CPolar *pPolar, int iVar){
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
			pVar = &pPolar->m_Cd;
			break;
		case 4:
			pVar = &pPolar->m_Cdp;
			break;
		case 5:
			pVar = &pPolar->m_Cm;
			break;
		case 6:
			pVar = &pPolar->m_XTr1;
			break;
		case 7:
			pVar = &pPolar->m_XTr2;
			break;
		case 8:
			pVar = &pPolar->m_HMom;
			break;
		case 9:
			pVar = &pPolar->m_Cpmn;
			break;
		case 10:
			pVar = &pPolar->m_ClCd;
			break;
		case 11:
			pVar = &pPolar->m_Cl32Cd;
			break;
		case 12:
			pVar = &pPolar->m_Cl;
			break;
		case 13:
			pVar = &pPolar->m_Re;
			break;
		case 14:
			pVar = &pPolar->m_XCp;
			break;
		default:
			pVar = &pPolar->m_Alpha;
			break;
	}
	return pVar;
}


void CXDirect::SetTitles(Graph* pGraph, int iX, int iY){
	switch (iX){
		case 0:
			pGraph->SetXTitle("Alpha");
			break;
		case 1:
			pGraph->SetXTitle("Cl");
			break;
		case 2:
			pGraph->SetXTitle("Cd");
			break;
		case 3:
			pGraph->SetXTitle("Cd x 10000");
			break;
		case 4:
			pGraph->SetXTitle("Cdp");
			break;
		case 5:
			pGraph->SetXTitle("Cm");
			break;
		case 6:
			pGraph->SetXTitle("Xtr1");
			break;
		case 7:
			pGraph->SetXTitle("Xtr2");
			break;
		case 8:
			pGraph->SetXTitle("HMom");
			break;
		case 9:
			pGraph->SetXTitle("Cpmin");
			break;
		case 10:
			pGraph->SetXTitle("Cl/Cd");
			break;
		case 11:
			pGraph->SetXTitle("|Cl|^(3/2)/Cd");
			break;
		case 12:
			pGraph->SetXTitle("1/Rt(Cl)");
			break;
		case 13:
			pGraph->SetXTitle("Re");
			break;
		case 14:
			pGraph->SetXTitle("XCp");
			break;
		default:
			pGraph->SetXTitle("Alpha");
			break;
	}
	switch (iY){
		case 0:
			pGraph->SetYTitle("Alpha");
			break;
		case 1:
			pGraph->SetYTitle("Cl");
			break;
		case 2:
			pGraph->SetYTitle("Cd");
			break;
		case 3:
			pGraph->SetYTitle("Cd x 10000");
			break;
		case 4:
			pGraph->SetYTitle("Cdp");
			break;
		case 5:
			pGraph->SetYTitle("Cm");
			break;
		case 6:
			pGraph->SetYTitle("Xtr1");
			break;
		case 7:
			pGraph->SetYTitle("Xtr2");
			break;
		case 8:
			pGraph->SetYTitle("HMom");
			break;
		case 9:
			pGraph->SetYTitle("Cpmin");
			break;
		case 10:
			pGraph->SetYTitle("Cl/Cd");
			break;
		case 11:
			pGraph->SetYTitle("|Cl|^(3/2)/Cd");
			break;
		case 12:
			pGraph->SetYTitle("1/Rt(Cl)");
			break;
		case 13:
			pGraph->SetYTitle("Re");
			break;
		case 14:
			pGraph->SetYTitle("XCp");
			break;
		default:
			pGraph->SetYTitle("Alpha");
			break;
	}
}



void CXDirect::OnDefineGraph() 
{
	CUserVarDlg dlg(this);
	dlg.m_bWing = false;

	if(!m_pCurGraph) return;
	if(m_pCurGraph == m_pCpGraph) {
		return;
	}

	if(m_pCurGraph == m_pPolarGraph){
		dlg.m_XSel = m_XPolar;
		dlg.m_YSel = m_YPolar;
	}
	else if(m_pCurGraph == m_pCmGraph){
		dlg.m_XSel = m_XCm;
		dlg.m_YSel = m_YCm;
	}
	else if(m_pCurGraph == m_pCzGraph){
		dlg.m_XSel = m_XCz;
		dlg.m_YSel = m_YCz;
	}
	else if(m_pCurGraph == m_pTrGraph){
		dlg.m_XSel = m_XTr;
		dlg.m_YSel = m_YTr;
	}
	else if(m_pCurGraph == m_pUserGraph){
		dlg.m_XSel = m_XUser;
		dlg.m_YSel = m_YUser;
	}

	if(IDOK == dlg.DoModal()){
		int iX = dlg.m_XSel;
		int iY = dlg.m_YSel;
		
		SetTitles(m_pCurGraph, iX, iY);

		if(m_pCurGraph == m_pPolarGraph){
			m_XPolar = iX;
			m_YPolar = iY;
		}
		if(m_pCurGraph == m_pCmGraph){
			m_XCm = iX;
			m_YCm = iY;
		}
		if(m_pCurGraph == m_pCzGraph){
			m_XCz = iX;
			m_YCz = iY;
		}
		if(m_pCurGraph == m_pTrGraph){
			m_XTr = iX;
			m_YTr = iY;
		}
		if(m_pCurGraph == m_pUserGraph){
			m_XUser = iX;
			m_YUser = iY;
		}

		if(iY == 4) m_pCurGraph->SetInverted(true); else m_pCurGraph->SetInverted(false);
		m_pCurGraph->ResetLimits();
		CreatePolarCurves();
		UpdateView();
	}	
}


void CXDirect::OnDefineUserGraph() 
{
	CUserVarDlg dlg(this);
	dlg.m_bWing = false;

	dlg.m_XSel = m_XUser;
	dlg.m_YSel = m_YUser;
	
	if(IDOK == dlg.DoModal()){
		m_XUser = dlg.m_XSel;
		m_YUser = dlg.m_YSel;
		
		SetTitles(m_pUserGraph, m_XUser, m_YUser);

		m_pUserGraph->ResetLimits();
		CreatePolarCurves();
		UpdateView();
	}	
}


void CXDirect::OnRestDefGraphs() 
{
	m_XPolar     = 2;
	m_YPolar     = 1;
	SetTitles(m_pPolarGraph, m_XPolar,m_YPolar);
	m_pPolarGraph->SetInverted(false);
	m_pPolarGraph->ResetLimits();

	m_XCz        = 0; 
	m_YCz        = 1;
	SetTitles(m_pCzGraph, m_XCz, m_YCz);
	m_pCzGraph->SetInverted(false);
	m_pCzGraph->ResetLimits();

	m_XCm        = 0;
	m_YCm        = 5;
	SetTitles(m_pCmGraph, m_XCm, m_YCm);
	m_pCmGraph->SetInverted(false);
	m_pCmGraph->ResetLimits();

	m_XTr        = 6;
	m_YTr        = 1;
	SetTitles(m_pTrGraph, m_XTr, m_YTr);
	m_pTrGraph->SetInverted(false);
	m_pTrGraph->ResetLimits();

	m_XUser      =  0;
	m_YUser      = 10;
	SetTitles(m_pUserGraph, m_XUser, m_YUser);
	m_pUserGraph->SetInverted(false);
	m_pUserGraph->ResetLimits();

	CreatePolarCurves();
	UpdateView();	
	
}

void CXDirect::OnRestoreVariables() 
{
	if(m_pCurGraph == m_pPolarGraph){
		m_XPolar     = 2;
		m_YPolar     = 1;
		SetTitles(m_pPolarGraph, m_XPolar,m_YPolar);
		m_pPolarGraph->SetInverted(false);
		m_pPolarGraph->ResetLimits();
	}
	if(m_pCurGraph == m_pCzGraph){
		m_XCz        = 0; 
		m_YCz        = 1;
		SetTitles(m_pCzGraph, m_XCz, m_YCz);
		m_pCzGraph->SetInverted(false);
		m_pCzGraph->ResetLimits();
	}
	if(m_pCurGraph == m_pCmGraph){
		m_XCm        = 0;
		m_YCm        = 5;
		SetTitles(m_pCmGraph, m_XCm, m_YCm);
		m_pCmGraph->SetInverted(true);
		m_pCmGraph->ResetLimits();
	}
	if(m_pCurGraph == m_pTrGraph){
		m_XTr        = 6;
		m_YTr        = 1;
		SetTitles(m_pTrGraph, m_XTr, m_YTr);
		m_pTrGraph->SetInverted(false);
		m_pTrGraph->ResetLimits();
	}
	if(m_pCurGraph == m_pUserGraph){
		m_XUser      =  0;
		m_YUser      = 10;
		SetTitles(m_pUserGraph, m_XUser, m_YUser);
		m_pUserGraph->SetInverted(false);
		m_pUserGraph->ResetLimits();
	}

	CreatePolarCurves();
	UpdateView();	
}



void CXDirect::OnAllGraphOptions() 
{

	CGraphOptions GOpts(this);
	LOGFONT lgft;

	GOpts.SetAxisData(m_pPolarGraph->GetAxisStyle(),
					  m_pPolarGraph->GetAxisWidth(),
					  m_pPolarGraph->GetAxisColor());
	bool b, ba; 
	int s, w;
	double f;
	COLORREF clr;
	m_pPolarGraph->GetXMajGrid(&b, &clr, &s, &w);
	GOpts.SetXMajData(b,s,w,clr);
	m_pPolarGraph->GetYMajGrid(&b, &clr, &s, &w);
	GOpts.SetYMajData(b,s,w,clr);

	m_pPolarGraph->GetXMinGrid(&b, &ba, &clr, &s, &w, &f);
	GOpts.SetXMinData(b,s,w,clr,f);
	m_pPolarGraph->GetYMinGrid(&b, &ba, &clr, &s, &w, &f);
	GOpts.SetYMinData(b,s,w,clr,f);

	m_pPolarGraph->GetTitleLogFont(&lgft);
	GOpts.SetTitleLogFont(&lgft);
	GOpts.SetTitleFontColor(m_pPolarGraph->GetTitleColor());

	m_pPolarGraph->GetLabelLogFont(&lgft);
	GOpts.SetLabelLogFont(&lgft);
	GOpts.SetLabelFontColor(m_pPolarGraph->GetLabelColor());

	m_pPolarGraph->GetLegendLogFont(&lgft);
	GOpts.SetLegendLogFont(&lgft);
	GOpts.SetLegendFontColor(m_pPolarGraph->GetLegendColor());

	GOpts.AMax    = m_pCmGraph->GetXMax();
	GOpts.AMin    = m_pCmGraph->GetXMin();
	GOpts.AOrigin = m_pCmGraph->GetX0();
	GOpts.AUnit   = m_pCmGraph->GetXUnit();

	GOpts.MMax    = m_pCmGraph->GetYMax();
	GOpts.MMin    = m_pCmGraph->GetYMin();
	GOpts.MOrigin = m_pCmGraph->GetY0();
	GOpts.MUnit   = m_pCmGraph->GetYUnit();

	GOpts.LMax    = m_pPolarGraph->GetYMax();
	GOpts.LMin    = m_pPolarGraph->GetYMin();
	GOpts.LOrigin = m_pPolarGraph->GetY0();
	GOpts.LUnit   = m_pPolarGraph->GetYUnit();

	GOpts.DMax    = m_pPolarGraph->GetXMax();
	GOpts.DMin    = m_pPolarGraph->GetXMin();
	GOpts.DOrigin = m_pPolarGraph->GetX0();
	GOpts.DUnit   = m_pPolarGraph->GetXUnit();

	GOpts.DoModal();// Settings applied from within GOpts if OK...
	
	UpdateView();	
}

void CXDirect::OnDeletePlr() 
{
	if(!m_pCurPolar) return;
	OpPoint *pOpPoint;
	int l;
	CString str;

	str = "Are you sure you want to delete polar :\n  "+m_pCurPolar->m_PlrName;
	str += "\n and all associated OpPoints ?";

	if (IDYES == AfxMessageBox(str, MB_YESNOCANCEL)){
		// start by removing all OpPoints
		for (l=(int)m_poaOpp->GetSize()-1; l>=0; l--){
			pOpPoint = (OpPoint*)m_poaOpp->GetAt(l);
			if (pOpPoint->m_strPlrName == m_pCurPolar->m_PlrName &&
				pOpPoint->m_strFoilName == m_pCurFoil->m_FoilName){
				m_poaOpp->RemoveAt(l);
				delete pOpPoint;
			}
		}
		// then remove CPolar and update views
		for (l=(int)m_poaPolar->GetSize()-1; l>=0; l--){
			if(m_pCurPolar == m_poaPolar->GetAt(l)){
				m_poaPolar->RemoveAt(l);
				delete m_pCurPolar;
			}
		}
		m_pCurOpp   = NULL;
		m_pCurPolar = NULL;
	}

	UpdatePlrs();
	SetPolar();

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(false);

	UpdateView();
}

void CXDirect::OnDelFoilPolars() {
	if(!m_pCurFoil) {
		return;
	}
	int l;
	OpPoint *pOpPoint;

	CString str;

	str = "Are you sure you want to delete polars and OpPoints\n";
	str +="associated to "+m_pCurFoil->m_FoilName  + " ?";

	if (IDYES == AfxMessageBox(str, MB_YESNOCANCEL)){
		// start by removing all OpPoints
		for (l=(int)m_poaOpp->GetSize()-1; l>=0; l--){
			pOpPoint = (OpPoint*)m_poaOpp->GetAt(l);
			if (pOpPoint->m_strFoilName == m_pCurFoil->m_FoilName){
				m_poaOpp->RemoveAt(l);
				delete pOpPoint;
			}
		}
		// then remove CPolar and update views
		CPolar* pPolar;
		for (l=(int)m_poaPolar->GetSize()-1; l>=0; l--){
			pPolar = (CPolar*)m_poaPolar->GetAt(l);
			if (pPolar->m_FoilName == m_pCurFoil->m_FoilName){
				m_poaPolar->RemoveAt(l);
				delete pPolar;
			}
		}
		m_pCurOpp = NULL;

	}
	m_pCurPolar = NULL;
	UpdatePlrs();
	SetPolar();

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(false);

	UpdateView();
}


void CXDirect::OnShowPanels() 
{
	m_bShowPanels = ! m_bShowPanels;
	CheckMenu();
	UpdateView();
}


void CXDirect::OnPolarFilter() 
{
	CPolarFilter dlg;
	dlg.m_bType1 = m_bType1;
	dlg.m_bType2 = m_bType2;
	dlg.m_bType3 = m_bType3;
	dlg.m_bType4 = m_bType4;
	dlg.m_bFoil=true;
	if(IDOK == dlg.DoModal())
	{
		if(dlg.m_bType1) m_bType1 = true; else m_bType1 = false;
		if(dlg.m_bType2) m_bType2 = true; else m_bType2 = false;
		if(dlg.m_bType3) m_bType3 = true; else m_bType3 = false;
		if(dlg.m_bType4) m_bType4 = true; else m_bType4 = false;
	}
	CreatePolarCurves();
	UpdateView();
}


void CXDirect::OnDeleteOpp() 
{
	if (!m_pCurPolar) return;
	if (AfxMessageBox("Are you sure you want to delete all Operating Point \n associated to the analysis ?",
		MB_YESNOCANCEL)== IDYES){
		CMainFrame* pFrame = (CMainFrame*)m_pFrame;
		pFrame->m_OperDlgBar.SetAnimate(false);
		RemoveOpPoint(false);
		UpdateOpps();
		SetOpp();
		UpdateView();
	}
}


void CXDirect::OnDelCurOpp() 
{
	OpPoint* pOpPoint = GetCurOpPoint();

	if (!pOpPoint) return;
	CString strong,str;
	strong = "Are you sure you want to delete the Operating Point \n";
	if(m_pCurPolar->m_Type !=4) str.Format("Alpha = %.2f", pOpPoint->Alpha);
	else str.Format("Reynolds = %.0f", pOpPoint->Reynolds);
	strong += str;
	strong += "  ?";

	if(AfxMessageBox(strong, MB_YESNOCANCEL)== IDYES){
		CMainFrame* pFrame = (CMainFrame*)m_pFrame;
		pFrame->m_OperDlgBar.SetAnimate(false);
		RemoveOpPoint(true);
		UpdateOpps();
		SetOpp();
		UpdateView();
	}
}


void CXDirect::CreatePolarCurves()
{
	// curves must be entirely reconstructed each time from the
	// operating points database, since user may have added
	// or deleted points & polars
	int k;
	CPolar *pPolar;
//	CArray <double, double> *pX;	
//	CArray <double, double> *pY;	

	m_pPolarGraph->DeleteCurves();
	m_pCmGraph->DeleteCurves();
	m_pCzGraph->DeleteCurves();
	m_pTrGraph->DeleteCurves();
	m_pUserGraph->DeleteCurves();

	for (k=0; k<m_poaPolar->GetSize(); k++){
		pPolar = (CPolar*)m_poaPolar->GetAt(k);

		if (pPolar->m_bIsVisible && pPolar->m_Alpha.GetSize()>0){
			if (pPolar->m_Type == 1 && m_bType1 ||
				pPolar->m_Type == 2 && m_bType2 ||
				pPolar->m_Type == 3 && m_bType3 ||
				pPolar->m_Type == 4 && m_bType4){
				CCurve* pPolarCurve = m_pPolarGraph->AddCurve();
				CCurve* pCmCurve    = m_pCmGraph->AddCurve();
				CCurve* pCzCurve    = m_pCzGraph->AddCurve();
				CCurve* pTr1Curve   = m_pTrGraph->AddCurve();
				CCurve* pTr2Curve = NULL;
				if(m_XTr == 6)	pTr2Curve   = m_pTrGraph->AddCurve();

				CCurve* pUserCurve  = m_pUserGraph->AddCurve();

				if(pPolar->m_bShowPoints){
					pPolarCurve->ShowPoints(true);
					pCmCurve->ShowPoints(true);
					pCzCurve->ShowPoints(true);
					pTr1Curve->ShowPoints(true);
					if(pTr2Curve) pTr2Curve->ShowPoints(true);
					pUserCurve->ShowPoints(true);
				}

				pPolarCurve->SetStyle(pPolar->m_Style);
				pCmCurve->SetStyle(pPolar->m_Style);
				pCzCurve->SetStyle(pPolar->m_Style);
				pTr1Curve->SetStyle(pPolar->m_Style);
				if(pTr2Curve) pTr2Curve->SetStyle(pPolar->m_Style);
				pUserCurve->SetStyle(pPolar->m_Style);

				pPolarCurve->SetColor(pPolar->m_Color);
				pCmCurve->SetColor(pPolar->m_Color);
				pCzCurve->SetColor(pPolar->m_Color);
				pTr1Curve->SetColor(pPolar->m_Color);
				if(pTr2Curve) pTr2Curve->SetColor(pPolar->m_Color);
				pUserCurve->SetColor(pPolar->m_Color);

				pPolarCurve->SetWidth(pPolar->m_Width);
				pCmCurve->SetWidth(pPolar->m_Width);
				pCzCurve->SetWidth(pPolar->m_Width);
				pTr1Curve->SetWidth(pPolar->m_Width);
				if(pTr2Curve) pTr2Curve->SetWidth(pPolar->m_Width);
				pUserCurve->SetWidth(pPolar->m_Width);

				FillPolarCurve(pPolarCurve, pPolar, m_XPolar, m_YPolar);
				FillPolarCurve(pCmCurve, pPolar, m_XCm, m_YCm);
				FillPolarCurve(pCzCurve, pPolar, m_XCz, m_YCz);
				FillPolarCurve(pTr1Curve, pPolar, m_XTr, m_YTr);
				if(pTr2Curve) FillPolarCurve(pTr2Curve, pPolar, 7, 1);
				FillPolarCurve(pUserCurve, pPolar, m_XUser, m_YUser);

				pPolarCurve->SetTitle(pPolar->m_PlrName);
				pCmCurve->SetTitle(pPolar->m_PlrName);
				pUserCurve->SetTitle(pPolar->m_PlrName);

				if(pTr2Curve){
					pTr1Curve->SetTitle(pPolar->m_PlrName + " / Xtr1");
					pTr2Curve->SetTitle(pPolar->m_PlrName + " / Xtr2");
				}
				else{
					pTr1Curve->SetTitle(pPolar->m_PlrName);
				}
			}
		}
	}
}


void CXDirect::FillPolarCurve(CCurve *pCurve, CPolar *pPolar, int XVar, int YVar)
{
	int i;
	CArray <double, double> *pX;	
	CArray <double, double> *pY;	

	pX = (CArray <double, double> *) GetVariable(pPolar, XVar);
	pY = (CArray <double, double> *) GetVariable(pPolar, YVar);
	double fx = 1.0;
	double fy = 1.0;
	if(XVar == 3) fx = 10000.0; 
	if(YVar == 3) fy = 10000.0; 
	for (i=0; i<pPolar->m_Alpha.GetSize(); i++)
	{
		if (XVar==12)
		{
			if((*pX)[i]>0.0)
			{
				if (YVar==12)
				{
					if((*pY)[i]>0.0)
					{
						pCurve->AddPoint(1.0/sqrt((*pX)[i]), 1.0/sqrt((*pY)[i]));
					}
				}
				else
				{
					pCurve->AddPoint(1.0/sqrt((*pX)[i]), (*pY)[i]*fy);
				}
			}
		}
		else{
			if (YVar==12)
			{
				if((*pY)[i]>0.0)
				{
					pCurve->AddPoint((*pX)[i]*fx, 1.0/sqrt((*pY)[i]));
				}
			}
			else
			{
				pCurve->AddPoint((*pX)[i]*fx, (*pY)[i]*fy);
			}
		}
	}
}



CFoil* CXDirect::SetFoil(CFoil* pFoil)
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	if(pFoil) m_pCurFoil = pFoil;
	SetBufferFoil();
	if(!InitXFoil()) 
	{
		DeleteFoil(false);
		pFrame->m_pCurFoil = NULL;
	}
	else
	{
		if(!m_pCurFoil)
		{
			m_pCurPolar = NULL;
			m_pCurOpp = NULL;
		}
		else if (m_pCurPolar && m_pCurPolar->m_FoilName !=m_pCurFoil->m_FoilName)
		{
//			m_pCurPolar = NULL;
//			m_pCurOpp = NULL;	
		}
		else if (m_pCurOpp && m_pCurOpp->m_strFoilName  !=m_pCurFoil->m_FoilName)
		{
//			m_pCurOpp = NULL;	
		}
		pFrame->m_pCurFoil = m_pCurFoil;
	}
	SetPolar();

//		if(m_bPolar) CreatePolarCurves();
//		else         CreateOppCurves();
	return m_pCurFoil;
}


CFoil* CXDirect::SetFoil(CString FoilName)
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	int j;
	if(FoilName.GetLength())
	{
		CFoil *pFoil;
		m_pCurFoil = NULL;
		for (j=0; j< m_poaFoil->GetSize(); j++)
		{
			pFoil = (CFoil*)m_poaFoil->GetAt(j);
			if(pFoil->m_FoilName == FoilName)
			{
				m_pCurFoil = pFoil;
				break;
			}
		}
	}
	else m_pCurFoil = NULL;

	if(m_pCurFoil)
	{
		SetBufferFoil();
		if(!InitXFoil()) 
		{
			pFrame->m_pCurFoil=NULL;
			DeleteFoil(false);
			SetPolar();
			return NULL;
		}
	}
	if(!m_pCurFoil)
	{
		m_pCurPolar = NULL;
		m_pCurOpp = NULL;
	}
	else if(m_pCurPolar && m_pCurPolar->m_FoilName !=m_pCurFoil->m_FoilName)
	{
//		m_pCurPolar = NULL;
//		m_pCurOpp = NULL;	
	}
	else if(m_pCurOpp && m_pCurOpp->m_strFoilName  !=m_pCurFoil->m_FoilName)
	{
//		m_pCurOpp = NULL;	
	}

	//are there 2 or more Opps associated to this foil
	int count = 0;

	OpPoint *pOpp;
	for (int i=0; i< m_poaOpp->GetSize(); i++)
	{
		pOpp = (OpPoint*)m_poaOpp->GetAt(i);
		if(pOpp->m_strFoilName == FoilName) count++;
		if(count>=2) break;
	}

	if (count<2) 
	{
		pFrame->m_OperDlgBar.SetAnimate(false);	
	}

	SetPolar();

	if(m_bPolar) CreatePolarCurves();
	else         CreateOppCurves();
	pFrame->m_pCurFoil=m_pCurFoil;
	return m_pCurFoil;
}

void CXDirect::SetPolar(CPolar *pPolar)
{
	// Finds the plr 
	// sets it as the active polar 
	// and initializes XFoil
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CString PlrName, strong;


	if(pPolar) m_pCurPolar = pPolar;

	if (!m_pCurFoil || !m_pCurPolar ||
		!m_pCurPolar->m_FoilName.GetLength() ||
		!m_pCurFoil->m_FoilName.GetLength()) {
		m_pCurPolar = NULL;
	}

	if(m_pCurPolar){
		if(m_pCurPolar->m_FoilName == m_pCurFoil->m_FoilName){
			InitXFoil2();
		}
		else {
			CPolar *pOldPolar;
			bool bFound = false;
			for (int i=0; i<m_poaPolar->GetSize(); i++){
				pOldPolar = (CPolar*)m_poaPolar->GetAt(i);
				if ((pOldPolar->m_FoilName == m_pCurFoil->m_FoilName) &&
					(pOldPolar->m_PlrName  == m_pCurPolar->m_PlrName)){
					m_pCurPolar = pOldPolar;
					InitXFoil2();
					bFound = true;
					break;
				}
			}
			if(!bFound){
				m_pCurPolar = NULL;
				m_pCurOpp   = NULL;
			}
		}
	}

	pFrame->m_PolarDlgBar.SetParams();
	pFrame->m_OperDlgBar.SetPolar();
	if (m_bPolar) CreatePolarCurves();
	SetOpp();
}


void CXDirect::SetPolar(CString PlrName)
{
	// Finds the plr with name PlrName
	// sets it as the active polar 
	// and initializes XFoil
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	if(!PlrName.GetLength()) {// try to read it
		if (!pFrame->m_ctrlPlr.GetCount()) {
			m_pCurPolar = NULL;
			m_pCurOpp   = NULL;
			return;//give up
		}
		//else 
		pFrame->m_ctrlPlr.GetLBText(0, PlrName);
	}

	m_pCurPolar = GetPolar(PlrName);

	if (!m_pCurPolar || !m_pCurFoil ||
		!m_pCurPolar->m_FoilName.GetLength() ||
		!m_pCurFoil->m_FoilName.GetLength()) {
		m_pCurPolar = NULL;
		m_pCurOpp   = NULL;
		return;
	}

	if(m_pCurPolar && (m_pCurPolar->m_FoilName == m_pCurFoil->m_FoilName)){
		InitXFoil2();
	}
	else {
		m_pCurPolar = NULL;
		m_pCurOpp = NULL;
	}
	
	if(m_pCurOpp){
		if (m_pCurOpp->m_strFoilName != m_pCurFoil->m_FoilName ||
			m_pCurOpp->m_strPlrName  != PlrName){			
//			does the Opp exist for these Foil/plrs ?
			OpPoint *pOpp;
			bool bFound = false;
			for (int l =0; l<m_poaOpp->GetSize(); l++){
				pOpp = (OpPoint*)m_poaOpp->GetAt(l);
				if (pOpp->m_strFoilName == m_pCurFoil->m_FoilName &&
					pOpp->m_strPlrName  == PlrName){
					if(abs(pOpp->Alpha-m_pCurOpp->Alpha)<0.0001){
						m_pCurOpp = pOpp;
						bFound = true;
						break;
					}
				}
			}
		
			if(!bFound) 
				m_pCurOpp = NULL;//give up
		}
	}

	pFrame->m_PolarDlgBar.SetParams();
	pFrame->m_OperDlgBar.SetPolar();
	SetOpp();
}

void CXDirect::SetOpp(double Alpha)
{
	CString strong;

	if(!m_pCurPolar) 
	{
		m_pCurOpp = NULL;
		if(!m_bPolar) CreateOppCurves();
		return;
	}
	if(m_pCurPolar->m_Type !=4)	strong.Format("%8.2f", Alpha);
	else strong.Format("%8.0f", Alpha);

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	int pos = pFrame->m_ctrlOpp.FindStringExact(-1, strong);
	if(pos !=CB_ERR) 
	{
		pFrame->m_ctrlOpp.SetCurSel(pos);
		m_pCurOpp = GetOpPoint(Alpha);
	}
	else 
	{
		pFrame->m_ctrlOpp.SetCurSel(0);
		m_pCurOpp = GetCurOpPoint();
	}
	CreateOppCurves();
	m_posAnimate = pFrame->m_ctrlOpp.GetCurSel();
	pFrame->m_PolarDlgBar.SetParams();
}

void CXDirect::SetOpp(OpPoint *pOpp)
{
	// set the opp, if valid
	// else set the current Opp, if any
	// else set the comboBox's first, if any
	// else set it to NULL
	
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CString strong;

	if(!pOpp) pOpp = m_pCurOpp;

	if(!pOpp)
	{
		//try to read it
		if(pFrame->m_ctrlOpp.GetCount())
		{
			double x;
			pFrame->m_ctrlOpp.SetCurSel(0);
			pFrame->m_ctrlOpp.GetLBText(0, strong);
			int res = sscanf(strong, "%lf", &x);
			if(res==1) 
			{
				pOpp = GetOpPoint(x);
			}
			else pOpp = NULL;
		}
		else
		{
			pOpp = NULL;
		}
	}
	else 
	{//set it
		strong.Format("%8.2f", m_pCurOpp->Alpha);
		int pos = pFrame->m_ctrlOpp.FindStringExact(-1, strong);
		if(pos!=CB_ERR)
		{
			pFrame->m_ctrlOpp.SetCurSel(pos);
		}
		else 
		{
			pFrame->m_ctrlOpp.SetCurSel(0);
		}
	}
	m_pCurOpp = pOpp;
	
	CreateOppCurves();

	m_posAnimate = pFrame->m_ctrlOpp.GetCurSel();
	pFrame->m_PolarDlgBar.SetParams();

}



void CXDirect::UpdateFoils()
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->UpdateFoils();

}
void CXDirect::UpdatePlrs()
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->UpdatePlrs();

}

void CXDirect::SetBufferFoil()
{
	if(!m_pCurFoil || !m_pCurFoil->m_FoilName.GetLength()) return;

	m_BufferFoil.CopyFoil(m_pCurFoil);
	m_BufferFoil.m_FoilName   = m_pCurFoil->m_FoilName;
	m_BufferFoil.m_FoilColor  = m_pCurFoil->m_FoilColor;
	m_BufferFoil.m_nFoilStyle = m_pCurFoil->m_nFoilStyle;
	m_BufferFoil.m_nFoilWidth = m_pCurFoil->m_nFoilWidth;
}


CPolar* CXDirect::AddPolar(CPolar *pPolar)
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	return pFrame->AddPolar(pPolar);
}



void CXDirect::OnFoilStyle() 
{
	CFoilClrDlg dlg;
	dlg.m_FoilColor = m_crFoilColor;
	dlg.m_FoilStyle = m_iFoilStyle;
	dlg.m_FoilWidth = m_iFoilWidth;
	dlg.m_BLColor = m_crBLColor;
	dlg.m_BLStyle = m_iBLStyle;
	dlg.m_BLWidth = m_iBLWidth;
	dlg.m_PressureColor = m_crPressureColor;
	dlg.m_PressureStyle = m_iPressureStyle;
	dlg.m_PressureWidth = m_iPressureWidth;
	dlg.m_NeutralColor  = m_crNeutralColor;
	dlg.m_NeutralStyle  = m_iNeutralStyle;
	dlg.m_NeutralWidth  = m_iNeutralWidth;
	dlg.m_bNeutral      = m_bNeutralLine;

	if(dlg.DoModal()==IDOK){
		m_crFoilColor = dlg.m_FoilColor;
		m_iFoilStyle  = dlg.m_FoilStyle;
		m_iFoilWidth  = dlg.m_FoilWidth;

		m_crBLColor = dlg.m_BLColor;
		m_iBLStyle  = dlg.m_BLStyle;
		m_iBLWidth  = dlg.m_BLWidth;

		m_crPressureColor = dlg.m_PressureColor;
		m_iPressureStyle  = dlg.m_PressureStyle;
		m_iPressureWidth  = dlg.m_PressureWidth;
		
		m_crNeutralColor  = dlg.m_NeutralColor;
		m_iNeutralStyle   = dlg.m_NeutralStyle;
		m_iNeutralWidth   = dlg.m_NeutralWidth;
		m_bNeutralLine = dlg.m_bNeutral ;
	}
	UpdateView();	

}

void CXDirect::OnRestoreDef() 
{
		//---- default paneling parameters (viscous)

	if (m_pXFoil){
		//intialize analysis parameter  until user changes them
		//---- default paneling parameters
		m_pXFoil->npan = 140;
		m_pXFoil->cvpar = 1.0f;
		m_pXFoil->cterat = 0.15f;
		m_pXFoil->ctrrat = 0.2f;
		
		//---- default paneling refinement zone x/c endpoints
		m_pXFoil->xsref1 = 1.0f;
		m_pXFoil->xsref2 = 1.0f;
		m_pXFoil->xpref1 = 1.0f;
		m_pXFoil->xpref2 = 1.0f;
		
		//---- drop tolerance for bl system solver
		m_pXFoil->vaccel = 0.01f;
	}

	//---- initialize freestream mach number to zero
	m_Adlg.m_MachDef   = 0.f;
	//---- default viscous parameters
	m_Adlg.m_ReDef    = 100000.0;
	m_Adlg.m_Reynolds = 100000.0;
	m_Adlg.m_MaTypDef  = 1;
	m_Adlg.m_ReTypDef  = 1;
	//initialize transition parameters until user changes them

	m_Adlg.Initialize();

	m_IterLim   = 100;
	m_bAlpha = true;
}


void CXDirect::OnReadDef() 
{
	CStdioFile XFile;
	CFileException fe;
	bool bb;
	int i;
	double a,b,c;
	CString FileName, strong;
	CFileDialog XFileDlg(true, "def", NULL, OFN_HIDEREADONLY, 
		_T("Xflr5 defaults (.def)|*.def|"));
	if(IDOK==XFileDlg.DoModal()) FileName = XFileDlg.GetPathName();
	
	BOOL bOpen = XFile.Open(FileName, CFile::modeRead, &fe);
	if (bOpen){
		int res;
		try{
			//  we're reading/loading
			//---- default paneling parameters (viscous)
			XFile.ReadString(strong);
			res = sscanf(strong, "%d%lf%lf%lf",
				&m_pXFoil->npan, &m_pXFoil->cvpar, &m_pXFoil->cterat, &m_pXFoil->ctrrat);
			if(res !=4)	{
				CFileException *pfe = new CFileException(CFileException::invalidFile);
				pfe->m_strFileName = FileName;
				throw pfe;
			}
			
			XFile.ReadString(strong);
			res = sscanf(strong, " %lf%lf%lf%lf",
				&m_pXFoil->xsref1, &m_pXFoil->xsref2, 
				&m_pXFoil->xpref1,  &m_pXFoil->xpref2);
			if(res !=4)	{
				CFileException *pfe = new CFileException(CFileException::invalidFile);
				pfe->m_strFileName = FileName;
				throw pfe;
			}
			
			//---- cp limits in  cp vs x  plot
			XFile.ReadString(strong);
			res = sscanf(strong, "%d%lf%lf%lf", &i, &a, &b, &c);
			if(res !=4)	{
				CFileException *pfe = new CFileException(CFileException::invalidFile);
				pfe->m_strFileName = FileName;
				throw pfe;
			}
			if (i) bb = true; else bb = false;
			m_pCpGraph->SetAuto(bb);
			m_pCpGraph->SetYMin(a);
			m_pCpGraph->SetYMax(b);
			m_pCpGraph->SetYUnit(c);
			
			
			//---- x-offset and scale factor for airfoil on cp vs x plot
			XFile.ReadString(strong);
			res = sscanf(strong, "%d",&i);
			if(res !=1)	{
				CFileException *pfe = new CFileException(CFileException::invalidFile);
				pfe->m_strFileName = FileName;
				throw pfe;
			}
			if (i) bb = true; else bb = false;
			m_pPolarGraph->SetAuto(bb);
			m_pCzGraph->SetAuto(bb);
			m_pCmGraph->SetAuto(bb);
			m_pTrGraph->SetAuto(bb);
			
			//---- polar plot cl,cd,alpha,cm  min,max,delta 
			//cl
			XFile.ReadString(strong);
			res = sscanf(strong, "%lf%lf%lf", &a,&b,&c);
			if(res !=3)	{
				CFileException *pfe = new CFileException(CFileException::invalidFile);
				pfe->m_strFileName = FileName;
				throw pfe;
			}
			m_pPolarGraph->SetYMin(a);
			m_pPolarGraph->SetYMax(b);
			m_pPolarGraph->SetYUnit(c); 
			
			m_pCzGraph->SetYMin(a);
			m_pCzGraph->SetYMax(b);
			m_pCzGraph->SetYUnit(c); 
			
			m_pTrGraph->SetYMin(a);
			m_pTrGraph->SetYMax(b);
			m_pTrGraph->SetYUnit(c); 
			
			//cd
			XFile.ReadString(strong);
			res = sscanf(strong, "%lf%lf%lf",&a,&b,&c);
			if(res !=3)	{
				CFileException *pfe = new CFileException(CFileException::invalidFile);
				pfe->m_strFileName = FileName;
				throw pfe;
			}
			m_pPolarGraph->SetXMin(a);
			m_pPolarGraph->SetXMax(b);
			m_pPolarGraph->SetXUnit(c); 
			
			//cm
			XFile.ReadString(strong);
			res = sscanf(strong, "%lf%lf%lf",&a,&b,&c);
			if(res !=3)	{
				CFileException *pfe = new CFileException(CFileException::invalidFile);
				pfe->m_strFileName = FileName;
				throw pfe;
			}
			m_pCmGraph->SetYMin(a);
			m_pCmGraph->SetYMax(b);
			m_pCmGraph->SetYUnit(c); 
			
			//Alpha
			XFile.ReadString(strong);
			res = sscanf(strong, "%lf%lf%lf",&a,&b,&c);
			if(res !=3)	{
				CFileException *pfe = new CFileException(CFileException::invalidFile);
				pfe->m_strFileName = FileName;
				throw pfe;
			}
			m_pCmGraph->SetXMin(a);
			m_pCmGraph->SetXMax(b);
			m_pCmGraph->SetXUnit(c); 
			
			m_pCzGraph->SetXMin(a);
			m_pCzGraph->SetXMax(b);
			m_pCzGraph->SetXUnit(c); 
			
			//--- default viscous parameters
			XFile.ReadString(strong);
			res = sscanf(strong, "%d%lf%lf", &i,  &a, &b);
			if(res !=3)	{
				CFileException *pfe = new CFileException(CFileException::invalidFile);
				pfe->m_strFileName = FileName;
				throw pfe;
			}
			m_Adlg.m_MaTypDef = i;
			m_Adlg.m_MachDef  = a;
			m_pXFoil->vaccel  = b;

			XFile.ReadString(strong);
			res = sscanf(strong, "%d%lf%lf", &i,&b,&c);
			if(res !=3)	{
				CFileException *pfe = new CFileException(CFileException::invalidFile);
				pfe->m_strFileName = FileName;
				throw pfe;
			}
			m_Adlg.m_ReTypDef = i;
			m_Adlg.m_ReDef    = b;
//			m_Adlg.m_NCritDef = c;

			XFile.ReadString(strong);
			res = sscanf(strong, "%lf%lf",&a, &b);
			if(res !=2)	{
				CFileException *pfe = new CFileException(CFileException::invalidFile);
				pfe->m_strFileName = FileName;
				throw pfe;
			}

			m_Adlg.Initialize();

			XFile.Close();	

		}
	
		catch (CFileException *ex){
			TCHAR   szCause[255];
			CString str;
			ex->GetErrorMessage(szCause, 255);
			str = _T("Error loading defaults : ");
			str += szCause;
			AfxMessageBox(str);
			ex->Delete();
		}
	}
	UpdateView();
}

void CXDirect::OnWriteDef() 
{
	CStdioFile XFile;
	CFileException fe;
	CString FileName, strong;
	CFileDialog XFileDlg(false, "def", NULL, OFN_OVERWRITEPROMPT, 
					 _T("Xflr5 defaults (.def)|*.def|"));
	if(IDOK==XFileDlg.DoModal()) FileName = XFileDlg.GetPathName();

	BOOL bOpen = XFile.Open(FileName, CFile::modeCreate |CFile::modeWrite, &fe);
	if (bOpen){
		//  we're saving/storing
		//---- default paneling parameters (viscous)
		strong.Format(" %d    %9.4f %9.4f %9.4f      | Npan    PPanel  TErat  REFrat\n",
			m_pXFoil->npan, m_pXFoil->cvpar, m_pXFoil->cterat, m_pXFoil->ctrrat);
		XFile.WriteString(strong);
		strong.Format(" %9.4f %9.4f %9.4f %9.4f   | XrefS1  XrefS2  XrefP1 XrefP2\n",
			m_pXFoil->xsref1, m_pXFoil->xsref2, m_pXFoil->xpref1,  m_pXFoil->xpref2);
		XFile.WriteString(strong);
		
		//---- cp limits in  cp vs x  plot
/*		if(m_pCpGraph->GetAuto()){
			strong.Format(" %d  %9.4f %9.4f %9.4f          | CPGraph : auto(1/0) cpmax   cpmin   cpdel\n",
				1, 0.f, 1.f, 0.1f);
		}
		else{*/
			strong.Format(" %d  %9.4f %9.4f %9.4f          | CPGraph : auto(1/0) cpmax   cpmin   cpdel\n",
				0, m_pCpGraph->GetYMin(), m_pCpGraph->GetYMax(), m_pCpGraph->GetYUnit());
//		}
		XFile.WriteString(strong);
		
		//---- x-offset and scale factor for airfoil on cp vs x plot
		strong.Format(" %d                                         | Polar graphs : auto(1/0) \n",
			0);
		XFile.WriteString(strong);
		
		//---- polar plot cl,cd,alpha,cm  min,max,delta 
/*		if(m_pPolarGraph->GetAuto()){
			//cl
			strong.Format(" %9.4f %9.4f %9.4f             |  Cl    : min   max   delta\n",
				0.f, 1.f, 0.1f); 
			XFile.WriteString(strong);
			//cd
			strong.Format(" %9.4f %9.4f %9.4f             |  Cd    : min   max   delta\n",
				0.f, 1.f, 0.1f); 
			XFile.WriteString(strong);
			//cm
			strong.Format(" %9.4f %9.4f %9.4f             |  Cm    : min   max   delta\n",
				0.f, 1.f, 0.1f); 
			XFile.WriteString(strong);
			//Alpha
			strong.Format(" %9.4f %9.4f %9.4f             |  Alpha : min   max   delta\n",
				0.f, 1.f, 0.1f); 
			XFile.WriteString(strong);
		}
		else{*/
			//cl
			strong.Format(" %9.4f %9.4f %9.4f             |  Cl    : min   max   delta\n",
				m_pPolarGraph->GetYMin(), m_pPolarGraph->GetYMax(), m_pPolarGraph->GetYUnit()); 
			XFile.WriteString(strong);
			//cd
			strong.Format(" %9.4f %9.4f %9.4f             |  Cd    : min   max   delta\n",
				m_pPolarGraph->GetXMin(), m_pPolarGraph->GetXMax(), m_pPolarGraph->GetXUnit()); 
			XFile.WriteString(strong);
			//cm
			strong.Format(" %9.4f %9.4f %9.4f             |  Cm    : min   max   delta\n",
				m_pCmGraph->GetYMin(), m_pCmGraph->GetYMax(), m_pCmGraph->GetYUnit()); 
			XFile.WriteString(strong);
			//Alpha
			strong.Format(" %9.4f %9.4f %9.4f             |  Alpha : min   max   delta\n",
				m_pCmGraph->GetXMin(), m_pCmGraph->GetXMax(), m_pCmGraph->GetXUnit()); 
			XFile.WriteString(strong);
//		}		
		int retyp, matyp;
		if (m_Adlg.m_Type == 1) {
			matyp=1;
			retyp=1;
		}
		else if (m_Adlg.m_Type == 2) {
			matyp=2;
			retyp=2;
		}
		else if (m_Adlg.m_Type == 3) {
			matyp=1;
			retyp=3;
		}
		else{
			matyp=1;
			retyp=1;
		}

		//--- default viscous parameters
		strong.Format(" %d %9.4f %9.4f                     | MAtype  Mach    Vaccel\n", 
			matyp, m_Adlg.m_fMach, m_pXFoil->vaccel);
		XFile.WriteString(strong);
//		strong.Format(" %d    %.0f %9.4f                     | REtype  Re   Ncrit\n", 
//			retyp, m_Adlg.m_Reynolds, m_Adlg.m_fNCrit);
//		XFile.WriteString(strong);
//		strong.Format(" %9.4f %9.4f                       | Xtript  Xtripb\n",
//			m_Adlg.m_fXTrip1, m_Adlg.m_fXTrip2);	
//		XFile.WriteString(strong);

		XFile.Close();	
	}
}

void CXDirect::OnGraphOptions() 
{
	if(m_pCurGraph){
		CGraphDlg dlg;
		dlg.m_pParent = m_pChildWnd;
		dlg.m_pGraph = m_pCurGraph;

		if(IDOK == dlg.DoModal()){
			m_pCurGraph->ResetLimits();

			UpdateView();
		}
	}	
}

void CXDirect::OnSavePolars() 
{
	if(!m_poaPolar->GetSize()) return;

	CFile fp;
	CString FileName;
	CFileDialog PolarDlg(false, "plr", NULL, OFN_OVERWRITEPROMPT, _T("Polar files (.plr)|*.plr|"));
	if(IDOK==PolarDlg.DoModal()){
		FileName = PolarDlg.GetPathName();
		CMainFrame *pFrame = (CMainFrame*)m_pFrame;
		pFrame->SavePolars(FileName);
	}
}

void CXDirect::OnSavePlrFile() 
{
	if(!m_pCurFoil || !m_poaPolar->GetSize()) {
		AfxMessageBox("No results to save", MB_OK);
		return;
	}

	CFile fp;
	CString FileName;
	FileName = m_pCurFoil->m_FoilName + ".plr";
	FileName.Replace("/", " ");
	CFileDialog PolarDlg(false, "plr", FileName, OFN_OVERWRITEPROMPT,
					     _T("Polar files (.plr)|*.plr|"));
	if(IDOK==PolarDlg.DoModal()){
		FileName = PolarDlg.GetPathName();
		CMainFrame *pFrame = (CMainFrame*)m_pFrame;
		pFrame->SavePolars(FileName, m_pCurFoil);
	}	
}

void CXDirect::OnRenameFoil() 
{
	if(!m_pCurFoil) return;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	if(pFrame->RenameFoil(m_pCurFoil)){
		SetFoil();
		UpdateFoils();
		UpdateView();
	}
}

void CXDirect::OnRenamePlr() 
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if(!m_pCurPolar) return;
	if(!m_pCurFoil) return;

	int resp, k,l;
	CPolar* pPolar;
	OpPoint * pOpp;
	CString OldName = m_pCurPolar->m_PlrName;

	CStringArray NameList;
	for(k=0; k<m_poaPolar->GetSize(); k++){
		pPolar = (CPolar*)m_poaPolar->GetAt(k);
		if(pPolar->m_FoilName == m_pCurFoil->m_FoilName)
			NameList.Add(pPolar->m_PlrName);
	}
	CNameDlg RDlg(this);
	RDlg.m_pStrArray = & NameList;
	RDlg.m_strQuestion = "Enter the new name for the foil polar :";
	RDlg.m_strName     = m_pCurPolar->m_PlrName;
//	RDlg.m_pFoil       = m_pCurFoil;
//	RDlg.m_pObArray    = m_poaPolar;
//	RDlg.m_Type        = 2;

	bool bExists = true;

	while (bExists){
		resp = (int)RDlg.DoModal();
		if(resp==IDOK){
			if (OldName == RDlg.m_strName) return;
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaPolar->GetSize(); k++){
				pPolar = (CPolar*)m_poaPolar->GetAt(k);
				if (pPolar->m_PlrName == RDlg.m_strName){
					bExists = true;
					break;
				}
			}
			if(!bExists){
				m_pCurPolar->m_PlrName = RDlg.m_strName;
				for (l=(int)m_poaOpp->GetSize()-1;l>=0; l--){
					pOpp = (OpPoint*)m_poaOpp->GetAt(l);
					if (pOpp->m_strPlrName == OldName &&
						pOpp->m_strFoilName == m_pCurFoil->m_FoilName){
						pOpp->m_strPlrName = RDlg.m_strName;
					}
				}
			}
			pFrame->SetSaveState(false);
		}
		else if(resp ==10){//user wants to overwrite
			if (OldName == RDlg.m_strName) return;
			for (k=0; k<m_poaPolar->GetSize(); k++){
				pPolar = (CPolar*)m_poaPolar->GetAt(k);
				if (pPolar->m_PlrName == RDlg.m_strName){
					bExists = true;
					break;
				}
			}
			for (l=(int)m_poaOpp->GetSize()-1;l>=0; l--){
				pOpp = (OpPoint*)m_poaOpp->GetAt(l);
				if (pOpp->m_strPlrName == pPolar->m_PlrName){
					m_poaOpp->RemoveAt(l);
					if(pOpp==m_pCurOpp) m_pCurOpp = NULL;
					delete pOpp;
				}
			}
			m_poaPolar->RemoveAt(k);
			if(pPolar==m_pCurPolar) m_pCurPolar = NULL;
			delete pPolar;

			//and rename everything
			m_pCurPolar->m_PlrName = RDlg.m_strName;

			for (l=(int)m_poaOpp->GetSize()-1;l>=0; l--){
				pOpp = (OpPoint*)m_poaOpp->GetAt(l);
				if (pOpp->m_strPlrName == OldName &&
					pOpp->m_strFoilName == m_pCurFoil->m_FoilName){
					pOpp->m_strPlrName = RDlg.m_strName;
				}
			}

			bExists = false;
			pFrame->SetSaveState(false);
		}
		else {
			return ;//cancelled 
		}
	}
	m_pCurPolar = NULL;
	m_pCurOpp   = NULL;
	SetPolar();
	UpdatePlrs();
	UpdateView();
}

void CXDirect::OnDeleteFoil() 
{
	DeleteFoil(true);
	UpdateFoils();

	if(m_bPolar) CreatePolarCurves();
	else         CreateOppCurves();
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->SetSaveState(false);

	UpdateView();
}

void CXDirect::OnShowBL() 
{
	m_bBL =  !m_bBL;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CToolBarCtrl *pTB = &(pFrame->m_XDirectBar.GetToolBarCtrl());
	if(m_bBL){
		m_bPressure = false;
		pTB->PressButton(IDT_SHOWPRESSURE, false);
		pTB->PressButton(IDT_SHOWBL, true);
	}
	else pTB->PressButton(IDT_SHOWBL, false);

	UpdateView();
}

void CXDirect::OnShowPressure() 
{
	m_bPressure = !m_bPressure;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CToolBarCtrl *pTB = &(pFrame->m_XDirectBar.GetToolBarCtrl());
	if(m_bPressure){
		m_bBL = false;
		pTB->PressButton(IDT_SHOWBL, false);
		pTB->PressButton(IDT_SHOWPRESSURE, true);
	}
	else pTB->PressButton(IDT_SHOWPRESSURE, false);

	UpdateView();
}


void CXDirect::UpdateOpps(){

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CString strong;
	OpPoint* pOpPoint;
//	double Alpha;

	pFrame->m_ctrlOpp.ResetContent();//start from scratch

	int size = 0;
	for (int l=0; l<m_poaOpp->GetSize(); l++){
		pOpPoint = (OpPoint*)m_poaOpp->GetAt(l);
		if (pOpPoint->m_strFoilName == m_pCurFoil->m_FoilName) size++;
	}

	if(size && m_pCurPolar){
		for (int i=0; i< m_poaOpp->GetSize(); i++){
			pOpPoint = (OpPoint*)m_poaOpp->GetAt(i);
			if (pOpPoint->m_strFoilName == m_pCurFoil->m_FoilName &&
				pOpPoint->m_strPlrName  == m_pCurPolar->m_PlrName){
				if(m_pCurPolar->m_Type !=4)	{
//					if(abs(pOpPoint->Alpha)<0.0001) pOpPoint->Alpha = 0.0001;
					strong.Format("%8.2f", pOpPoint->Alpha);
				}
				else strong.Format("%8.0f", pOpPoint->Reynolds);
				pFrame->m_ctrlOpp.AddString(strong);
				pFrame->m_ctrlOpp.EnableWindow(true);
			}
		}
	}
	else {
		m_pCurOpp = NULL;
		CreateOppCurves();
		UpdateView();
	}
	if(pFrame->m_ctrlOpp.GetCount()) pFrame->m_ctrlOpp.EnableWindow(true);
	else                             pFrame->m_ctrlOpp.EnableWindow(false);
	
}

void CXDirect::OnAnalyze() 
{
	CWaitCursor wait;

	bool bSequence;
	double Alpha, AlphaMax, DeltaAlpha;
	double Cl, ClMax, DeltaCl;
	double Re, ReMax, DeltaRe;

//first read parameters
	CString str;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	bSequence  = pFrame->m_OperDlgBar.m_bSequence;

	Alpha      = pFrame->m_OperDlgBar.m_Alpha;
	AlphaMax   = pFrame->m_OperDlgBar.m_AlphaMax;
	DeltaAlpha = pFrame->m_OperDlgBar.m_DeltaAlpha;

	Cl      = pFrame->m_OperDlgBar.m_Cl;
	ClMax   = pFrame->m_OperDlgBar.m_ClMax;
	DeltaCl = pFrame->m_OperDlgBar.m_DeltaCl;

	Re      = pFrame->m_OperDlgBar.m_Re;
	ReMax   = pFrame->m_OperDlgBar.m_ReMax;
	DeltaRe = pFrame->m_OperDlgBar.m_DeltaRe;

	if(m_pCurFoil)
	{
		m_pXFoil->m_bTrace = true;
		if(m_bViscous) 
		{
			m_pXFoil->lvisc = true;
			if(m_pCurPolar->m_Type!=4)
			{
				if(m_bAlpha)
				{
					if (bSequence)
					{
						if (abs(DeltaAlpha)<0.001)
						{
							AfxMessageBox("Increase Delta_Alpha", MB_OK);
							return;
						}
						else if(AlphaMax < Alpha)	DeltaAlpha = -abs(DeltaAlpha);
						else if(AlphaMax > Alpha)	DeltaAlpha =  abs(DeltaAlpha);
					}
					else AlphaMax = Alpha;
					AnalysisAlpha(Alpha, AlphaMax, DeltaAlpha, bSequence);
				}
				else
				{
					if (bSequence)
					{
						if (abs(DeltaCl)<0.001)
						{
							AfxMessageBox("Increase Delta_Cl", MB_OK);
							return;
						}
						else if(ClMax < Cl)	DeltaCl = -abs(DeltaCl);
						else if(ClMax > Cl)	DeltaCl =  abs(DeltaCl);
					}
					else ClMax = Cl;
					AnalysisCl(Cl, ClMax, DeltaCl, bSequence);
				}
			}
			else 
			{
				if (bSequence)
				{
					if (abs(DeltaRe) < 10.0)
					{
						AfxMessageBox("Increase Delta_Re", MB_OK);
						return;
					}
					else if(ReMax < Re)
						DeltaRe = -abs(DeltaRe);
					else if(ReMax > Re)
						DeltaRe = abs(DeltaRe);
				}
				else ReMax = Re;
				AnalysisRe(Re, ReMax, DeltaRe, bSequence);
			}
		}
		else 
		{
			m_pXFoil->lvisc = false;
			Analysis1(Alpha, AlphaMax, DeltaAlpha, bSequence);
		}

	// and update window

		m_bInitBL = !m_pXFoil->lblini;
		pFrame->m_OperDlgBar.m_ctrlInitBL.SetCheck(m_bInitBL);;

		m_pCurGraph = m_pCpGraph;

		UpdateOpps();
/*		if(m_pCurPolar->m_Type!=4){
			SetOpp(Alpha);
		}
		else{
			SetOpp(Re);
		}*/
		SetOpp();

		if(m_bPolar)CreatePolarCurves();
		UpdateView();
	}
}

void CXDirect::Analysis1(double Alpha, double AlphaMax, double DeltaAlpha, bool bSequence)
{
	// Inviscid analysis 
	int ia;
	double alfa;

	CString str;
	CString strAppDirectory;
	char    szAppPath[MAX_PATH] = "";
	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
	// Extract directory
	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.GetLength()-9);
	str =strAppDirectory + "XFLR5.log";
	m_XFile.Open(str, CFile::modeCreate | CFile::modeWrite);
	m_pXFoil->pXFile = &m_XFile;

	SetFileHeader();

	if (bSequence)
	{ //sequential analysis 
		for (ia=0; ia<=int((AlphaMax-Alpha)/DeltaAlpha); ia++)
		{
			if(m_bAlpha)
			{
				alfa = Alpha+ia*DeltaAlpha;

				str.Format("Alpha = %.3f",alfa);
				m_XFile.WriteString(str);

				m_pXFoil->alfa = alfa*pi/180.0;
				if (m_pXFoil->specal()) 
				{
					str = "... Converged\n\n";
					m_XFile.WriteString(str);
					m_pCurOpp = AddOpPoint();
				}
				else
				{
					OnCpCalcError();
				}
			}
			else
			{
				m_pXFoil->lalfa = true;
				m_pXFoil->alfa = 0.0f;
				m_pXFoil->qinf = 1.0f;
				double cl = Alpha+ia*DeltaAlpha;
				m_pXFoil->clspec = cl;
				if(m_pXFoil->speccl()) 
				{
					alfa = 	m_pXFoil->alfa*180/pi;
					str.Format("Cl = %.3f,     Alpha = %.3f\n\n", cl, alfa);
					m_XFile.WriteString(str);
					m_pCurOpp = AddOpPoint();
				}
				else{
					OnCpCalcError();
				}
			}
		}	
	}
	else 
	{ 
		//single point analysis
		if(m_bAlpha)
		{
			alfa = Alpha*pi/180.0;
			m_pXFoil->alfa = alfa;
			str.Format("Alpha = %.3f  ",alfa);
			m_XFile.WriteString(str);

			if (m_pXFoil->specal()) 
			{
				str = "... Converged\n\n";
				m_XFile.WriteString(str);
				m_pCurOpp = AddOpPoint();
			}
			else
			{
				OnCpCalcError();
			}
		}
		else
		{
			double cl = Alpha;
			m_pXFoil->lalfa = true;
			m_pXFoil->alfa = 0.0;
			m_pXFoil->qinf = 1.0;
			m_pXFoil->clspec = cl;
			if(m_pXFoil->speccl()) 
			{
				alfa = 	m_pXFoil->alfa*180/pi;
				Alpha = alfa;
				m_pCurOpp = AddOpPoint();
			}
			else
			{
				OnCpCalcError();
			}
		}
	}
	m_XFile.Close();
}


void CXDirect::AnalysisAlpha(double AlphaMin, double AlphaMax, double DeltaAlpha, bool bSequence)
{
	//viscous analysis / Type 1-2-3

	m_ViscDlg.m_pXDirect = this;
	m_ViscDlg.m_pXFoil = m_pXFoil;
	m_ViscDlg.m_pFrame = m_pFrame;

	m_ViscDlg.m_bAlpha       = true;
	m_ViscDlg.m_IterLim      = m_IterLim;
	m_ViscDlg.m_FoilName     = m_pCurFoil->m_FoilName;

	m_ViscDlg.SetAlpha(AlphaMin,AlphaMax,DeltaAlpha);

	m_ViscDlg.DoModal();
}



void CXDirect::AnalysisCl(double ClMin, double ClMax, double DeltaCl, bool bSequence)
{
	//viscous analysis / Type 1-2-3

	m_ViscDlg.m_pXDirect = this;
	m_ViscDlg.m_pXFoil = m_pXFoil;
	m_ViscDlg.m_pFrame = m_pFrame;

	m_ViscDlg.m_bAlpha       = false;
	m_ViscDlg.m_IterLim      = m_IterLim;
	m_ViscDlg.m_FoilName     = m_pCurFoil->m_FoilName;

	m_ViscDlg.SetCl(ClMin, ClMax, DeltaCl);
	
	m_ViscDlg.DoModal();
}


void CXDirect::AnalysisRe(double ReMin, double ReMax, double DeltaRe, bool bSequence)
{
	//Type 4 viscous analysis
	m_pXFoil->alfa = m_pCurPolar->m_ASpec * pi/180.0;

	m_ViscDlg.m_pXDirect = this;
	m_ViscDlg.m_pXFoil = m_pXFoil;
	m_ViscDlg.m_pFrame = m_pFrame;

	m_ViscDlg.m_bAlpha       = false;
	m_ViscDlg.m_IterLim      = m_IterLim;
	m_ViscDlg.m_FoilName     = m_pCurFoil->m_FoilName;
	if(m_pCurPolar->m_Type == 4) m_ViscDlg.m_bType4 = true;

	m_ViscDlg.SetRe(ReMin,ReMax,DeltaRe);
	
	m_ViscDlg.DoModal();

}

void CXDirect::SetFileHeader()
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	m_XFile.WriteString("\n");
	m_XFile.WriteString(pFrame->m_VersionName);
	m_XFile.WriteString("\n");
	m_XFile.WriteString(m_pCurFoil->m_FoilName);
	m_XFile.WriteString("\n");
	m_XFile.WriteString(m_pCurPolar->m_PlrName);
	m_XFile.WriteString("\n");

	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString str, strong;
	switch (tm.wMonth){
		case 1:{
			strong = "January";
			break;
		}
		case 2:{
			strong = "February";
			break;
		}
		case 3:{
			strong = "March";
			break;
		}
		case 4:{
			strong = "April";
			break;
		}
		case 5:{
			strong = "May";
			break;
		}
		case 6:{
			strong = "Jun";
			break;
		}
		case 7:{
			strong = "July";
			break;
		}
		case 8:{
			strong = "August";
			break;
		}
		case 9:{
			strong = "September";
			break;
		}
		case 10:{
			strong = "October";
			break;
		}
		case 11:{
			strong = "November";
			break;
		}
		case 12:{
			strong = "December";
			break;
		}
	}
	str.Format(" %02d, %d  at  %02d:%02d:%02d \n\n",
		 tm.wDay, tm.wYear,
		tm.wHour, tm.wMinute, tm.wSecond);

	m_XFile.WriteString(strong + str);
	m_XFile.WriteString("\n___________________________________\n\n");
	m_XFile.WriteString("Inviscis analysis \n\n");
}

void CXDirect::OnOper() 
{
	m_bPolar = false;
	m_pCurOpp = GetCurOpPoint();

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CToolBarCtrl *pTB = &(pFrame->m_XDirectBar.GetToolBarCtrl());
	pTB->PressButton(IDT_OPPS, !m_bPolar);
	pTB->PressButton(IDT_POLARS, m_bPolar);
	pFrame->m_PolarDlgBar.SetParams();

	CMenu* pMenu = m_pFrame->GetMenu()->GetSubMenu(2);
	pMenu->CheckMenuItem(IDM_XFLR5_POLARS,MF_BYCOMMAND | MF_UNCHECKED);
	pMenu->CheckMenuItem(IDM_XFLR5_OPER,MF_BYCOMMAND | MF_CHECKED);

	m_pCurGraph = m_pCpGraph;
	UpdateView();
}



void CXDirect::Animate(bool bAnimate)
{
	if(!m_pCurFoil || !m_pCurPolar) {
		m_bAnimate = false;
		return;
	}

	OpPoint* pOpPoint;
	int l;

	if(bAnimate) 
	{
		for (l=0; l< m_poaOpp->GetSize(); l++)
		{
			pOpPoint = (OpPoint*)m_poaOpp->GetAt(l);

			if (pOpPoint &&
				pOpPoint->m_strPlrName  == m_pCurPolar->m_PlrName &&
				pOpPoint->m_strFoilName == m_pCurFoil->m_FoilName) 
			{
					if(m_pCurOpp->Alpha - pOpPoint->Alpha<0.0001) 
						m_posAnimate = l-1;
			}
		}
		m_bAnimate  = true;
	}
	else  
	{
		m_bAnimate = false;
		if(m_posAnimate<0 || m_posAnimate>=m_poaOpp->GetSize()) return;
		OpPoint* pOpPoint = (OpPoint*)m_poaOpp->GetAt(m_posAnimate);
		if(pOpPoint) SetOpp(pOpPoint->Alpha);
		UpdateView();
	}
}


void CXDirect::OnLoad() 
{
	CString FileName, PathName;
	CFileDialog XFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, 
		_T("XFLR5 file (.dat; .plr; .wpa)|*.dat; *.plr; *.wpa|"));

	if(IDOK==XFileDlg.DoModal()) 
	{
		CMainFrame*pFrame = (CMainFrame*)m_pFrame;
		pFrame->LoadFile(XFileDlg.GetFileName(), XFileDlg.GetPathName());
		SetFoil();
		UpdateFoils();
		UpdateView();
	}
}

void CXDirect::SetLogPixels(int LPY)
{
	m_pCpGraph->SetLogPixelsY(LPY);
	m_pPolarGraph->SetLogPixelsY(LPY);
	m_pCzGraph->SetLogPixelsY(LPY);
	m_pCmGraph->SetLogPixelsY(LPY);
	m_pTrGraph->SetLogPixelsY(LPY);
	m_pUserGraph->SetLogPixelsY(LPY);
}

BOOL CXDirect::PreTranslateMessage(MSG* pMsg)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	if (pMsg->message == WM_KEYDOWN)
	{
		CWnd* pWnd = GetFocus();
		if (pMsg->wParam == VK_RETURN)
		{
			if (pWnd != &pFrame->m_ctrlFoil &&
				pWnd != &pFrame->m_ctrlPlr &&
				pWnd != &pFrame->m_ctrlOpp) 
			{
				if(pFrame->m_OperDlgBar.IsWindowVisible()) pFrame->m_OperDlgBar.m_ctrlAnalyze.SetFocus();
				return true;
			}
		}
		if (pMsg->wParam == VK_ESCAPE)
		{
			if (m_bAnimate) 
			{
				m_bAnimate = false;
				if(pFrame->m_OperDlgBar.IsWindowVisible()) 
					pFrame->m_OperDlgBar.m_ctrlAnimate.SetCheck(false);
				if(m_posAnimate<0 || m_posAnimate>=m_poaOpp->GetSize()) return true;
				OpPoint* pOpPoint = (OpPoint*)m_poaOpp->GetAt(m_posAnimate);
				SetOpp(pOpPoint->Alpha);
				UpdateView();
				return true;
			}
		}
		if (pMsg->wParam == VK_TAB)
		{
			if (pWnd == &pFrame->m_ctrlFoil) 
			{
				pFrame->m_ctrlPlr.SetFocus();
				return true;
			}
			else if (pWnd == &pFrame->m_ctrlPlr) 
			{
				pFrame->m_ctrlOpp.SetFocus();
				return true;
			}
			else if (pWnd == &pFrame->m_ctrlOpp) 
			{
				pFrame->m_ctrlFoil.SetFocus();
				return true;
			}
		}
		if (pMsg->wParam == VK_F2)
		{
			OnRenameFoil();
			return true;
		}
		if (pMsg->wParam == VK_F5)
		{
			OnOper();
			return true;
		}
		if (pMsg->wParam == VK_F7)
		{
			OnManageFoils();
			return true;
		}
		SHORT sh = GetKeyState(VK_SHIFT);
		if (pMsg->wParam == VK_F6 && (sh & 0x8000))
		{
			OnBatchAnalysis();
			return true;
		}
		if (pMsg->wParam == VK_F6)
		{
			OnSingleAnalysis();
			return true;
		}
		if (pMsg->wParam == VK_F8)
		{
			OnPolars();
			return true;
		}
		SHORT sh1 = GetKeyState(VK_LCONTROL);
		SHORT sh2 = GetKeyState(VK_RCONTROL);
		if (pMsg->wParam == 'O' && ((sh1 & 0x8000)||(sh2 & 0x8000))) 
		{ 
			OnLoad();
			return true;
		} 
		if (pMsg->wParam == 'E' && ((sh1 & 0x8000)||(sh2 & 0x8000))) 
		{ 
			OnSavePolars();
			return true;
		} 
		if (pMsg->wParam == 'P' && ((sh1 & 0x8000)||(sh2 & 0x8000))) 
		{ 
			OnPrint();
			return true;
		} 
		if (pMsg->wParam == 'G' && pWnd==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))) 
		{ 
			CPoint pt;
			GetCursorPos(&pt);
			m_pChildWnd->ScreenToClient( &pt);
			m_pCurGraph = GetGraph(pt);
			if(m_pCurGraph) OnGraphOptions();
			return true;
		} 
		if (pMsg->wParam == 'V' && pWnd==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))) 
		{ 
			CPoint pt;
			GetCursorPos(&pt);
			m_pChildWnd->ScreenToClient( &pt);
			m_pCurGraph = GetGraph(pt);
			if(m_pCurGraph) OnDefineGraph();
			return true;
		} 

		if (pMsg->wParam == 'Z')
		{ 
			return true;//User is zooming with 'Z' key instead of mouse midle button
		} 
		if (pMsg->wParam == 'T' && pWnd==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))) 
		{ 
			if(m_bPolar) {
				m_iPlrView = 2;
				UpdateView();
				return true;
			}
		} 
		if (pMsg->wParam == 'A' && pWnd==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))) { 
			if(m_bPolar) {
				m_iPlrView = 0;
				UpdateView();
				return true;
			}

		}
		if (pMsg->wParam == 'N' && !((sh1 & 0x8000)||(sh2 & 0x8000))) { 
			OnAnalyze();
			return true;
		}

		if ((pMsg->wParam == '1' || pMsg->wParam == 97)&&
			pWnd==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))) { 
			if(m_bPolar) {
				m_iPlrView  = 1;
				m_iPlrGraph = 1;
				UpdateView();
				return true;
			}
		}
		if ((pMsg->wParam == '2' || pMsg->wParam == 98)&&
			pWnd==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))) { 
			if(m_bPolar) {
				m_iPlrView  = 1;
				m_iPlrGraph = 2;
				UpdateView();
				return true;
			}
		}
		if ((pMsg->wParam == '3' || pMsg->wParam == 99)&&
			pWnd==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))) { 
			if(m_bPolar) {
				m_iPlrView  = 1;
				m_iPlrGraph = 3;
				UpdateView();
				return true;
			}
		}
		if ((pMsg->wParam == '4' || pMsg->wParam == 100)&&
			pWnd==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))) { 
			if(m_bPolar) {
				m_iPlrView  = 1;
				m_iPlrGraph = 4;
				UpdateView();
				return true;
			}
		}
		if ((pMsg->wParam == '5' || pMsg->wParam == 101)&&
			pWnd==m_pChildWnd && !((sh1 & 0x8000)||(sh2 & 0x8000))) { 
			if(m_bPolar) {
				m_iPlrView  = 1;
				m_iPlrGraph = 5;
				UpdateView();
				return true;
			}
		}
	}
	return CWnd::PreTranslateMessage(pMsg);
}

void CXDirect::OnExportOpps() 
{
	if(!m_poaPolar->GetSize()) 
	{
		AfxMessageBox("No Operating Points to export to file", MB_OK);	
		return;
	}

	int i,j, type;
	CMainFrame * pMainFrame = (CMainFrame*)m_pFrame;
	CString FileName;
			
	static TCHAR BASED_CODE szFilter[] = _T("Text File (*.txt)|*.txt|") _T("CSV format (*.csv)|*.csv|") ;
	CFileDialog XFileDlg(false, "txt", NULL, OFN_OVERWRITEPROMPT, szFilter);
	XFileDlg.m_ofn.nFilterIndex = pMainFrame->m_TextFileFormat;

	if(IDOK == XFileDlg.DoModal())
	{
		pMainFrame->m_TextFileFormat = XFileDlg.m_ofn.nFilterIndex;
		type = XFileDlg.m_ofn.nFilterIndex;
		FileName = XFileDlg.GetPathName();
		CStdioFile XFile;
		OpPoint* pOpPoint;
		CFileException fe;
		BOOL bOpen = XFile.Open(FileName, CFile::modeCreate | CFile::modeWrite, &fe);//erase and write

		try
		{
			if(!bOpen)
			{
				fe.m_strFileName = FileName;
				throw &fe;
			}
			else
			{
				CString strOut;
				CString Header, strong;
				CMainFrame* pFrame = (CMainFrame*)m_pFrame;
				XFile.WriteString(pFrame->m_VersionName);
				XFile.WriteString("\n\n");
				strong = m_pCurFoil->m_FoilName + "\n";
				XFile.WriteString(strong);
				for (i=0; i<m_poaOpp->GetSize(); i++)
				{
					pOpPoint = (OpPoint*)m_poaOpp->GetAt(i);
					if(pOpPoint->m_strFoilName == m_pCurPolar->m_FoilName && pOpPoint->m_strPlrName == m_pCurPolar->m_PlrName )
					{
						if(type==1)
							strong.Format("Reynolds = %.0f   Mach = %.4f  NCrit = %.2f\n",	pOpPoint->Reynolds, pOpPoint->Mach, pOpPoint->ACrit);
						else
							strong.Format("Reynolds =, %.0f,Mach =, %.4f,NCrit =, %.2f\n",	pOpPoint->Reynolds, pOpPoint->Mach, pOpPoint->ACrit);

						XFile.WriteString(strong);
						if(type==1) Header.Format("  Alpha        Cd        Cl        Cm        XTr1      XTr2   TEHMom    Cpmn\n");
						else        Header.Format("Alpha,Cd,Cl,Cm,XTr1,XTr2,TEHMom,Cpmn\n");
						XFile.WriteString(Header);

						if(type==1)
							strong.Format("%7.3f   %9.3f   %7.3f   %7.3f   %7.3f   %7.3f   %7.4f  %7.4f\n",
								pOpPoint->Alpha, pOpPoint->Cd,
								pOpPoint->Cl,    pOpPoint->Cm,
								pOpPoint->Xtr1,  pOpPoint->Xtr2,
								pOpPoint->m_TEHMom,  pOpPoint->Cpmn);
						else
							strong.Format("%7.3f,%9.3f,%7.3f,%7.3f,%7.3f,%7.3f,%7.4f,%7.4f\n",
								pOpPoint->Alpha, pOpPoint->Cd,
								pOpPoint->Cl,    pOpPoint->Cm,
								pOpPoint->Xtr1,  pOpPoint->Xtr2,
								pOpPoint->m_TEHMom,  pOpPoint->Cpmn);

						XFile.WriteString(strong);
						if(type==1) XFile.WriteString(" Cpi          Cpv\n-----------------\n");
						else        XFile.WriteString("Cpi,Cpv\n");
						
						for (j=0; j<pOpPoint->n; j++)
						{
							if(pOpPoint->m_bVisc)
							{
								if(type==1) strong.Format("%7.4f   %7.4f\n",pOpPoint->Cpi[j], pOpPoint->Cpv[j]);
								else        strong.Format("%7.4f,%7.4f\n",pOpPoint->Cpi[j], pOpPoint->Cpv[j]);
							}
							else
							{
								strong.Format("% 7.4f\n", pOpPoint->Cpi[j]);
							}
							
							XFile.WriteString(strong);
						}
						XFile.WriteString("\n\n");
					}
				}
				XFile.Close();
			}
		}
		catch (CFileException *ex)
		{
			TCHAR   szCause[255];
			CString str;
			ex->GetErrorMessage(szCause, 255);
			str = _T("Error writing analysis : ");
			str += szCause;
			AfxMessageBox(str);
		}
	}
	
}

void CXDirect::OnInitXFoil() 
{
	m_pXFoil->Initialize();
	m_pXFoil->lblini = false;
	m_pXFoil->lipan  = false;
	InitXFoil();
	InitXFoil2();	
}

void CXDirect::SetParams()
{
	CheckMenu();

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CToolBarCtrl *pTB = &(pFrame->m_XDirectBar.GetToolBarCtrl());
	pTB->PressButton(IDT_OPPS, !m_bPolar);
	pTB->PressButton(IDT_POLARS, m_bPolar);

}



void CXDirect::CheckMenu()
{
	CMenu* pMenu = m_pFrame->GetMenu()->GetSubMenu(6);
	if(pMenu){
		if(m_OppVar==0) {
			pMenu->CheckMenuItem(IDM_XFLR5_CPGRAPH, MF_BYCOMMAND | MF_CHECKED);
			pMenu->CheckMenuItem(IDM_XFLR5_QGRAPH, MF_BYCOMMAND  | MF_UNCHECKED);
		}
		else if(m_OppVar==1) {
			pMenu->CheckMenuItem(IDM_XFLR5_QGRAPH, MF_BYCOMMAND  | MF_CHECKED);
			pMenu->CheckMenuItem(IDM_XFLR5_CPGRAPH, MF_BYCOMMAND | MF_UNCHECKED);
		}
		else  {
			pMenu->CheckMenuItem(IDM_XFLR5_QGRAPH, MF_BYCOMMAND  | MF_UNCHECKED);
			pMenu->CheckMenuItem(IDM_XFLR5_CPGRAPH, MF_BYCOMMAND | MF_UNCHECKED);
		}
		if(m_bShowInviscid) pMenu->CheckMenuItem(IDM_CPI, MF_BYCOMMAND  | MF_CHECKED);
		else                pMenu->CheckMenuItem(IDM_CPI, MF_BYCOMMAND  | MF_UNCHECKED);

		if(m_bCurOppOnly) pMenu->CheckMenuItem(IDM_SHOWCUROPP, MF_BYCOMMAND  | MF_CHECKED);
		else              pMenu->CheckMenuItem(IDM_SHOWCUROPP, MF_BYCOMMAND  | MF_UNCHECKED);

		if(m_bCpGraph) pMenu->CheckMenuItem(IDM_SHOWCPGRAPH, MF_BYCOMMAND  | MF_CHECKED);
		else           pMenu->CheckMenuItem(IDM_SHOWCPGRAPH, MF_BYCOMMAND  | MF_UNCHECKED);
	}

	pMenu = m_pFrame->GetMenu()->GetSubMenu(5);
	if(pMenu){
		if(m_bShowUserGraph)  pMenu->CheckMenuItem(IDM_XFLR5_SHOWUSERGRAPH,MF_BYCOMMAND | MF_CHECKED);
		else                  pMenu->CheckMenuItem(IDM_XFLR5_SHOWUSERGRAPH,MF_BYCOMMAND | MF_UNCHECKED);
//		if (m_iPlrView == 0) pMenu->CheckMenuItem(IDM_XFLR5_ALLFOUR, MF_BYCOMMAND | MF_CHECKED);
//		else 
		if (m_iPlrView == 1){
			switch (m_iPlrGraph){
				case 1:{
					pMenu->CheckMenuItem(IDM_XFLR5_CLCD, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
				case 2:{
					pMenu->CheckMenuItem(IDM_XFLR5_CLALPHA, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
				case 3:{
					pMenu->CheckMenuItem(IDM_XFLR5_CLXTR, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
				case 4:{
					pMenu->CheckMenuItem(IDM_XFLR5_CMALPHA, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
				case 5:{
					pMenu->CheckMenuItem(IDM_XFLR5_USERGRAPH, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
			}
		}
		if (m_iPlrView == 2) pMenu->CheckMenuItem(IDM_XFLR5_COUPLEGRAPH, MF_BYCOMMAND | MF_CHECKED);
	}

	pMenu = m_pFrame->GetMenu()->GetSubMenu(2);
	if(pMenu){
		if(m_bShowPanels) 	pMenu->CheckMenuItem(IDM_SHOWPANELS,MF_BYCOMMAND | MF_CHECKED);
		else                pMenu->CheckMenuItem(IDM_SHOWPANELS,MF_BYCOMMAND | MF_UNCHECKED);
		if(m_bCpGraph	) 	pMenu->CheckMenuItem(IDM_SHOWCPGRAPH,MF_BYCOMMAND | MF_CHECKED);
		else                pMenu->CheckMenuItem(IDM_SHOWCPGRAPH,MF_BYCOMMAND | MF_UNCHECKED);
		if(m_bPolar)   pMenu->CheckMenuItem(IDM_XFLR5_POLARS,MF_BYCOMMAND | MF_CHECKED);
		else           pMenu->CheckMenuItem(IDM_XFLR5_OPER,MF_BYCOMMAND | MF_CHECKED);
	}
	
	pMenu = m_pFrame->GetMenu()->GetSubMenu(1);
	if(pMenu){

		pMenu->CheckMenuItem(IDM_BSPLINES,      MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_SPLINEDPOINTS, MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_MIXEDINVERSE,  MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_FULLINVERSE,   MF_BYCOMMAND | MF_UNCHECKED);
		pMenu->CheckMenuItem(IDM_XFLR5,         MF_BYCOMMAND | MF_CHECKED);
		pMenu->CheckMenuItem(IDM_MIAREX,        MF_BYCOMMAND | MF_UNCHECKED);
	}
}

 




bool CXDirect::SetXFLR5Cursor(CWnd* pWnd, CPoint ptMouse, UINT message) 
{
	if (pWnd !=m_pChildWnd) return false;

	if(m_bTrans){
		if  (!m_bPolar || 
			( m_bPolar && (m_pPolarGraph->IsInDrawRect(ptMouse) ||
						   m_pCzGraph->IsInDrawRect(ptMouse)    ||
						   m_pCmGraph->IsInDrawRect(ptMouse)    ||
						   m_pTrGraph->IsInDrawRect(ptMouse) ))){
			SetCursor(m_hcMove);
			return TRUE;
		}
	}

	if (m_bPolar){
		bool bZ = false;
			
		if(m_iPlrGraph>0) {
			if(m_pCurGraph && m_pCurGraph->IsInDrawRect(ptMouse)) 
				bZ = true;
		}
		else {
			if(m_iPlrView == 2){
				if (m_pPolarGraph->IsInDrawRect(ptMouse) ||
					 m_pUserGraph->IsInDrawRect(ptMouse) ){
						bZ = true;
				}
			}
			else if (m_iPlrView == 0){
				if (m_pPolarGraph->IsInDrawRect(ptMouse) ||
					 m_pCzGraph->IsInDrawRect(ptMouse)    ||
					 m_pCmGraph->IsInDrawRect(ptMouse)    ||
					 m_pTrGraph->IsInDrawRect(ptMouse)    ||
					 (m_bShowUserGraph && m_pUserGraph->IsInDrawRect(ptMouse))){
						bZ = true;
				}
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
	else {
		if(m_pCpGraph->IsInDrawRect(ptMouse)){
			SetCursor(m_hcCross);
			return TRUE;
		}
		else {
			SetCursor(m_hcArrow);
			return TRUE;
		}
	}
	return true;
}

void CXDirect::OnAbout() 
{
	((CXFLR5App*)AfxGetApp())->OnAppAbout();
}

CFoil* CXDirect::AddBufferFoil()
{
	// Adds the buffer foil to the array for further use 
	int i;
	CFoil *pOldFoil;
	CFoil *pFoil;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFoil = NULL;

	if(!GetFoil(m_BufferFoil.m_FoilName))
	{
		pFoil = new CFoil();
		if(!pFoil) return NULL;
		pFoil->CopyFoil(&m_BufferFoil);
		pFoil->m_FoilColor = pFrame->m_crColors[m_poaFoil->GetSize()%24];
		pFoil->InitFoil();
		bool IsInserted = false;
		for (i=0; i<m_poaFoil->GetSize(); i++)
		{
			pOldFoil = (CFoil*)m_poaFoil->GetAt(i);
			if (pFoil->m_FoilName < pOldFoil->m_FoilName)
			{
				m_poaFoil->InsertAt(i, pFoil);
				IsInserted = true;
				break;
			}
		}
		if (!IsInserted) 
		{
			m_poaFoil->Add(pFoil);
		}
	}
	return pFoil;
}


bool CXDirect::InitXFoil(CFoil * pFoil)
{
	//loads pFoil in XFoil, 
	//calculates normal vectors, 
	//and sets results in current foil
	
	if(!pFoil) pFoil = m_pCurFoil;
	if(!pFoil) return  false;
	m_pXFoil->m_FoilName = pFoil->m_FoilName;
	int i, k;

	for (i =0; i<pFoil->n; i++)
	{
		m_pXFoil->xb[i+1] = pFoil->x[i];
		m_pXFoil->yb[i+1] = pFoil->y[i];
	}

	m_pXFoil->nb = pFoil->n;
//	if(pFoil->m_bTEFlap){
//		m_pXFoil->lflap  = true;
//		m_pXFoil->lbflap = true;
//	}
//	else {
		m_pXFoil->lflap  = false;
		m_pXFoil->lbflap = false;
//	}

	m_pXFoil->ddef = 0.0;
	m_pXFoil->xbf  = 1.0;
	m_pXFoil->ybf  = 0.0;

	m_pXFoil->lscini = false;
	m_pXFoil->lqspec = false;
	m_pXFoil->lvisc  = false;
//	m_pXFoil->acrit      = pFoil->m_NCrit;
//	m_pXFoil->xstrip[1]  = pFoil->m_XTopTr;
//	m_pXFoil->xstrip[2]  = pFoil->m_XBotTr;

	if(m_pCurPolar)
	{
		m_pXFoil->acrit      = m_pCurPolar->m_ACrit;
		m_pXFoil->xstrip[1]  = m_pCurPolar->m_XTop;
		m_pXFoil->xstrip[2]  = m_pCurPolar->m_XBot;
	}

	if(m_pXFoil->Preprocess())
	{
		m_pXFoil->CheckAngles();
		for (k=0; k<m_pXFoil->n;k++)
		{
			pFoil->nx[k] = m_pXFoil->nx[k+1];
			pFoil->ny[k] = m_pXFoil->ny[k+1];
		}
		pFoil->n = m_pXFoil->n;
		return true;
	}
	else 
	{
		AfxMessageBox("Unrecognized foil format", MB_OK);
		return false;
	}
}

void CXDirect::InitXFoil2()
{
	//Sets Analysis parameters in XFoil
	if(!m_pCurPolar) return;

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	m_pXFoil->lblini = false;
	m_pXFoil->lipan = false;
	m_bInitBL = !m_pXFoil->lblini;
	pFrame->m_OperDlgBar.m_ctrlInitBL.SetCheck(m_bInitBL);;

	m_pXFoil->m_bTrace = false;

	m_pXFoil->reinf1 = m_pCurPolar->m_Reynolds;
	if (m_pCurPolar->m_Type == 4) m_pXFoil->alfa = m_pCurPolar->m_ASpec*pi/180.0;

	m_pXFoil->minf1  = m_pCurPolar->m_Mach;
	
	m_pXFoil->retyp  = m_pCurPolar->m_ReType;
	m_pXFoil->matyp  = m_pCurPolar->m_MaType;

	if(m_pCurPolar)
	{
		m_pXFoil->acrit      = m_pCurPolar->m_ACrit;
		m_pXFoil->xstrip[1]  = m_pCurPolar->m_XTop;
		m_pXFoil->xstrip[2]  = m_pCurPolar->m_XBot;
	}

	m_pXFoil->lalfa = true;
	m_pXFoil->qinf  = 1.0;

	if (m_pCurPolar->m_Mach > 0.000001)
	{
		if(!m_pXFoil->SetMach())
		{
			CString str;
			str.Format("... Invalid Analysis Settings\nCpCalc: local speed too large \n Compressibility corrections invalid ");
			AfxMessageBox(str, MB_OK);
		}
	}
}



CFoil* CXDirect::AddFoil(CString strFoilName, double x[], double y[], 
					   int nf, double thickness, double camber)
{
	// Adds a foil in the array for further use should the user
	// want to visualize graphs for this foil
	int i;
	CFoil *pOldFoil;
	CFoil *pFoil;
	pFoil = NULL;
	if(!GetFoil(strFoilName)){
		pFoil = new CFoil();
		pFoil->n = nf;
		for (i=0; i<nf; i++){
			pFoil->x[i] = x[i];
			pFoil->y[i] = y[i];
		}
		pFoil->m_fCamber = camber;
		pFoil->m_fThickness = thickness;
		pFoil->m_FoilName = strFoilName;
		pFoil->InitFoil();
		bool IsInserted = false;
		for (i=0; i<m_poaFoil->GetSize(); i++){
			pOldFoil = (CFoil*)m_poaFoil->GetAt(i);
//			if (pFoil->m_FoilName < pOldFoil->m_FoilName){
			if (pFoil->m_FoilName.CompareNoCase(pOldFoil->m_FoilName)<0){
				m_poaFoil->InsertAt(i, pFoil);
				IsInserted = true;
				break;
			}
		}
		if (!IsInserted) {
			m_poaFoil->Add(pFoil);
		}
	}
	return pFoil;
}


void CXDirect::OnPolarReset() 
{
	if(m_pCurPolar) {
		if(IDYES==AfxMessageBox("Reset polar content ?", MB_YESNO)){
			m_pCurPolar->ResetPolar();
			CreatePolarCurves();
			if(m_bPolar) UpdateView();
		}
	}
}

void CXDirect::OnEditPolar() 
{
	if (m_pCurPolar == NULL) return;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;

	CPolar MemPolar;
	MemPolar.Copy(m_pCurPolar);

	CEditPlrDlg dlg;
	dlg.m_pPolar = m_pCurPolar;
	dlg.m_pParent = this;

	bool bPoints = m_pCurPolar->m_bShowPoints;
	m_pCurPolar->m_bShowPoints = true;

	CreatePolarCurves();
	UpdateView();

	if(dlg.DoModal() == IDOK)
	{
		pFrame->SetSaveState(false);
	}
	else
	{
		m_pCurPolar->Copy(&MemPolar);
	}
	m_pCurPolar->m_bShowPoints = bPoints;
	CreatePolarCurves();
	UpdateView();
}

void CXDirect::OnDuplicateFoil() 
{
	if(!m_pCurFoil) return;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	CFoil *pNewFoil = new CFoil();
	pNewFoil->CopyFoil(m_pCurFoil);
	pNewFoil->InitFoil();

	m_BufferFoil.CopyFoil(pNewFoil);
	if(pFrame->SetModFoil(pNewFoil))
	{
		SetFoil();
		UpdateFoils();
		UpdateView();
		CMainFrame* pFrame = (CMainFrame*)m_pFrame;
		pFrame->SetSaveState(false);

	}
	delete pNewFoil;	
}



void CXDirect::PaintOpPoint(CDC *pDC, CRect *pCltRect, CRect *pDrawRect)
{
	COLORREF color;
	int style, width;
	CString Result, str, str1;
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;

	CChildView * pChildView = (CChildView*)m_pChildWnd;

	if (pDrawRect->Width()<100 || pDrawRect->Height()<100) return;

	if (m_bNeutralLine && m_pCurFoil) 
	{
		color = m_crNeutralColor;
		style = m_iNeutralStyle;
		width = m_iNeutralWidth;
		if(IsBlackAndWhite()) GetBWColor(color, style, width);
		lb.lbColor = color;
		CPen NeutralPen(PS_GEOMETRIC | style, GetPenWidth(width, false), &lb);
		CPen *pOldPen = pDC->SelectObject(&NeutralPen);
		pDC->MoveTo(pCltRect->right,m_ptOffset.y);
		pDC->LineTo(pCltRect->left,m_ptOffset.y);
		pDC->SelectObject(pOldPen);
	}


	if (!m_pCurFoil || !m_pCurFoil->m_FoilName.GetLength()) 
		return;

	if(m_pCurOpp)
	{
		DrawFoil(pDC, -m_pCurOpp->Alpha*pi/180.0,
			m_fFoilScale, m_ptOffset.x, m_ptOffset.y, false);
		if(m_pCurOpp->m_bDispSurf && m_bBL)
			DrawBL(pDC, m_pCurOpp, m_fFoilScale, m_ptOffset.x, m_ptOffset.y, false);
		else if(m_bPressure)
			DrawCpv(pDC, m_pCurOpp, m_fFoilScale, m_ptOffset.x, m_ptOffset.y, false);
	}
	else 
		DrawFoil(pDC, 0.0, m_fFoilScale, m_ptOffset.x, m_ptOffset.y, false);

	if(m_bCpGraph)
	{
		//Draw Cp Graph
		m_pCpGraph->DrawGraph(pDC, pDrawRect, false);
		CPoint Place(pCltRect->right-330, pCltRect->top + 20);
		m_pCpGraph->DrawLegend(pDC, false, Place);//Graph::DrawLegend uses graph's legend font and color
	}

	// Write Titles and results
	pDC->SetTextAlign(TA_RIGHT);

	CFont LegendFont;
	LegendFont.CreateFontIndirect(&pChildView->m_WndLogFont);
	CFont* pOldFont = pDC->SelectObject(&LegendFont);
	int D = 0;
	int ZPos = m_rCltRect.bottom;
	int XPos = m_rCltRect.right-10;
	pDC->SetTextColor(m_pCpGraph->GetTitleColor());

	//last write the foil's properties
	pDC->SetTextColor(pChildView->m_WndTextColor);
	CString strong;
	
	int Back = 5;

	if(m_BufferFoil.m_bTEFlap) Back +=3;

	pDC->SetTextAlign(TA_LEFT);
	int LeftPos = m_rCltRect.left+10;
	ZPos = m_rCltRect.bottom - 10 - Back*12;
	
	D = 0;
	str = "%";
	str1.LoadString(IDS_THICKNESS);
	Result.Format(str1, m_BufferFoil.m_fThickness*100.0);
	pDC->TextOut(LeftPos,ZPos+D, Result+str);
	D+=12;

	str1.LoadString(IDS_MAXTHICKNESSPOS);
	Result.Format(str1, m_BufferFoil.m_fXThickness*100.0);
	pDC->TextOut(LeftPos,ZPos+D, Result+str);
	D+=12;

	str1.LoadString(IDS_MAXCAMBER);
	Result.Format(str1, m_BufferFoil.m_fCamber*100.0);
	pDC->TextOut(LeftPos,ZPos+D, Result+str);
	D+=12;

	str1.LoadString(IDS_MAXCAMBERPOS);
	Result.Format(str1, m_BufferFoil.m_fXCamber*100.0);
	pDC->TextOut(LeftPos,ZPos+D, Result+str);
	D+=12;

	str1.LoadString(IDS_TOTALPANELS);
	Result.Format(str1, m_BufferFoil.n);
	pDC->TextOut(LeftPos,ZPos+D, Result);
	D+=12;

	if(m_BufferFoil.m_bTEFlap)
	{
		str1.LoadString(IDS_FLAPANGLE);
		strong.Format(str1, m_BufferFoil.m_TEFlapAngle);
		pDC->TextOut(LeftPos,ZPos+D, strong);
		D+=12;

		str1.LoadString(IDS_XHINGE);
		strong.Format(str1, m_BufferFoil.m_TEXHinge);
		strong+="%";
		pDC->TextOut(LeftPos,ZPos+D, strong);
		D+=12;

		str1.LoadString(IDS_YHINGE);
		strong.Format(str1, m_BufferFoil.m_TEYHinge);
		strong+="%";
		pDC->TextOut(LeftPos,ZPos+D, strong);
		D+=12;
	}
		

	pDC->SetTextAlign(TA_RIGHT);

//	ZPos = pDrawRect->top+10;
//	XPos = pDrawRect->right-20;
	D = 0;
	Back = 6;

	ZPos = m_rCltRect.bottom-Back*12 - 10;
	XPos = m_rCltRect.right-10;
	D=0;

	if(m_pCurPolar)
	{
		str1.LoadString(IDS_POLARTYPE);
		strong.Format(str1, m_pCurPolar->m_Type);
		pDC->TextOut(XPos,ZPos, strong);
		D+=12;
		if(m_pCurPolar->m_Type ==1)
		{
			ReynoldsFormat(strong, m_pCurPolar->m_Reynolds );
			strong ="Reynolds = " + strong;
			pDC->TextOut(XPos,ZPos+D, strong);
			D+=12;
			strong.Format("Mach = %9.3f", m_pCurPolar->m_Mach);
			pDC->TextOut(XPos,ZPos+D, strong);
			D+=12;
		}
		if(m_pCurPolar->m_Type ==2)
		{
			ReynoldsFormat(strong, m_pCurPolar->m_Reynolds );
			strong ="Re.sqrt(Cl) = " + strong;
			pDC->TextOut(XPos,ZPos+D, strong);
			D+=12;
			strong.Format("M.sqrt(Cl) = %9.3f", m_pCurPolar->m_Mach);
			pDC->TextOut(XPos,ZPos+D, strong);
			D+=12;
		}
		if(m_pCurPolar->m_Type ==3)
		{
			ReynoldsFormat(strong, m_pCurPolar->m_Reynolds );
			strong ="Re.sqrt(Cl) = " + strong;
			pDC->TextOut(XPos,ZPos+D, strong);
			D+=12;

			strong.Format("Mach = %9.3f", m_pCurPolar->m_Mach);
			pDC->TextOut(XPos,ZPos+D, strong);
			D+=12;
		}
		if(m_pCurPolar->m_Type ==4)
		{
			strong.Format("Alpha = %8.2f°", m_pCurPolar->m_ASpec);
			pDC->TextOut(XPos,ZPos+D, strong);
			D+=12;
			strong.Format("Mach = %9.3f", m_pCurPolar->m_Mach);
			pDC->TextOut(XPos,ZPos+D, strong);
			D+=12;
		}

		strong.Format("NCrit = %9.2f", m_pCurPolar->m_ACrit);
		pDC->TextOut(XPos,ZPos+D, strong);
		D+=12;

		str1.LoadString(IDS_FORCEDUPPERTRANS);
		strong.Format(str1, m_pCurPolar->m_XTop);
		pDC->TextOut(XPos,ZPos+D, strong);
		D+=12;
		str1.LoadString(IDS_FORCEDLOWERTRANS);
		strong.Format(str1, m_pCurPolar->m_XBot);
		pDC->TextOut(XPos,ZPos+D, strong);
		D+=12;
	}

	if(m_pCurOpp)  
	{
		Back =6;
		if(m_pCurOpp->m_bTEFlap) Back++;
		if(m_pCurOpp->m_bLEFlap) Back++;
		if(m_pCurOpp->m_bVisc && abs(m_pCurOpp->Cd)>0.0) Back++;
		if(m_pCurPolar->m_Type==2 ) Back++;
		if(m_pCurPolar->m_Type!=1 && m_pCurPolar->m_Type!=4) Back++;

		ZPos = m_rCltRect.bottom-Back*12 - 10;
		XPos = (int)((m_rCltRect.right+m_rCltRect.left)/2.0)+75;
		D=0;

		if(m_pCurPolar->m_Type!=1)
		{
			ReynoldsFormat(Result, m_pCurOpp->Reynolds);
			Result = "Re = "+ Result;
			pDC->TextOut(XPos,ZPos+D, Result);
			D+=12;
		}
		if(m_pCurPolar->m_Type==2)
		{
			Result.Format("Ma = %9.4f", m_pCurOpp->Mach);
			pDC->TextOut(XPos,ZPos+D, Result);
			D+=12;
		}
		if(m_pCurPolar->m_Type!=4)
		{
			Result.Format("Alpha = %8.2f°", m_pCurOpp->Alpha);
			pDC->TextOut(XPos,ZPos+D, Result);
			D+=12;
		}
		Result.Format("Cl = %9.3f", m_pCurOpp->Cl);
		pDC->TextOut(XPos,ZPos+D, Result);
		D+=12;

		Result.Format("Cm = %9.3f", m_pCurOpp->Cm);
		pDC->TextOut(XPos,ZPos+D, Result);
		D+=12;

		Result.Format("Cd = %9.4f", m_pCurOpp->Cd);
		pDC->TextOut(XPos,ZPos+D, Result);
		D+=12;

		if(m_pCurOpp->m_bVisc && abs(m_pCurOpp->Cd)>0.0)
		{
			Result.Format("L/D = %9.2f", m_pCurOpp->Cl/m_pCurOpp->Cd);
			pDC->TextOut(XPos,ZPos+D, Result);
			D+=12;
		}

		str1.LoadString(IDS_UPPERTRANS);
		Result.Format(str1, m_pCurOpp->Xtr1);
		pDC->TextOut(XPos,ZPos+D, Result);
		D+=12;

		str1.LoadString(IDS_LOWERTRANS);
		Result.Format(str1, m_pCurOpp->Xtr2);
		pDC->TextOut(XPos,ZPos+D, Result);
		D+=12;

		if(m_pCurOpp->m_bTEFlap)
		{
			str1.LoadString(IDS_TEHINGEMOMENT);
			Result.Format(str1, m_pCurOpp->m_TEHMom);
			pDC->TextOut(XPos,ZPos+D, Result);
			D+=12;
		}

		if(m_pCurOpp->m_bLEFlap)
		{
			str1.LoadString(IDS_LEHINGEMOMENT);
			Result.Format(str1, m_pCurOpp->m_LEHMom);
			pDC->TextOut(XPos,ZPos+D, Result);
			D+=12;
		}
	}
	pDC->SelectObject(pOldFont);
}


void CXDirect::OnNormalize() 
{
	if(!m_pCurFoil) return;
	CString str, str1;
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	double length = m_pCurFoil->NormalizeGeometry();
	InitXFoil();
	SetBufferFoil();
	str1.LoadString(IDS_FOILNORMALIZATION);
	str.Format(str1, length);
	pFrame->SetSaveState(false);
	pFrame->m_wndStatusBar.SetWindowText(str);

	UpdateView();
}


void CXDirect::OnHidePolars() 
{
	if(!m_pCurFoil) return;
	CMainFrame * pFrame = (CMainFrame*) m_pFrame;
	CPolar *pPolar;
	for (int i=0; i<m_poaPolar->GetSize(); i++){
		pPolar = (CPolar*)m_poaPolar->GetAt(i);
		if(pPolar->m_FoilName == m_pCurFoil->m_FoilName){
			pPolar->m_bIsVisible = false;
		}
	}
	CreatePolarCurves();
	pFrame->m_PolarDlgBar.SetParams();
	UpdateView();
}


void CXDirect::OnShowPolars() 
{
	if(!m_pCurFoil) return;
	int i;
	CMainFrame * pFrame = (CMainFrame*) m_pFrame;
	CPolar *pPolar;
	for (i=0; i<m_poaPolar->GetSize(); i++)
	{
		pPolar = (CPolar*)m_poaPolar->GetAt(i);
		if(pPolar->m_FoilName == m_pCurFoil->m_FoilName)
		{
			pPolar->m_bIsVisible = true;
		}
	}
	CreatePolarCurves();
	pFrame->m_PolarDlgBar.SetParams();
	pFrame->SetSaveState(false);
	UpdateView();
	
}



void CXDirect::OnCpGraphDlg() 
{
	m_pCurGraph = m_pCpGraph;
	OnGraphOptions();
}

void CXDirect::OnHideAllPolars() 
{
	CPolar *pPolar;
	for (int i=0; i<m_poaPolar->GetSize(); i++){
		pPolar = (CPolar*)m_poaPolar->GetAt(i);
		pPolar->m_bIsVisible = false;
	}
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->m_PolarDlgBar.SetParams();
	pFrame->SetSaveState(false);
	CreatePolarCurves();
	UpdateView();
}

void CXDirect::OnShowAllPolars() 
{
	CPolar *pPolar;
	for (int i=0; i<m_poaPolar->GetSize(); i++){
		pPolar = (CPolar*)m_poaPolar->GetAt(i);
		pPolar->m_bIsVisible = true;
	}
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->m_PolarDlgBar.SetParams();
	pFrame->SetSaveState(false);
	CreatePolarCurves();
	UpdateView();
}


void CXDirect::OnCpCalcError()
{
	CString str;
	str.Format("... Invalid Analysis Settings\nCpCalc: local speed too large \n Compressibility corrections invalid ");
	AfxMessageBox(str, MB_OK);
	OnInitXFoil();
}

void CXDirect::OnZoomGraph() 
{
	m_iPlrView = 1;
	if(m_pCurGraph == m_pPolarGraph)	m_iPlrGraph=1;
	if(m_pCurGraph == m_pCzGraph)		m_iPlrGraph=2;
	if(m_pCurGraph == m_pTrGraph)		m_iPlrGraph=3;
	if(m_pCurGraph == m_pCmGraph)		m_iPlrGraph=4;
	if(m_pCurGraph == m_pUserGraph)		m_iPlrGraph=5;
	UpdateView();
}





void CXDirect::OnExportCurOpp() 
{
	if(!m_pCurOpp) return ;// is there anything to export ?

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString FileName;

	static TCHAR BASED_CODE szFilter[] = _T("Text File (*.txt)|*.txt|") _T("CSV format (*.csv)|*.csv|") ;
	CFileDialog XFileDlg(false, "txt", NULL, OFN_OVERWRITEPROMPT, szFilter);
	XFileDlg.m_ofn.nFilterIndex = pFrame->m_TextFileFormat;

	if(IDOK == XFileDlg.DoModal())
	{
		pFrame->m_TextFileFormat = XFileDlg.m_ofn.nFilterIndex;
		FileName = XFileDlg.GetPathName();
		m_pCurOpp->Export(FileName, pFrame->m_VersionName,  XFileDlg.m_ofn.nFilterIndex);
	}
}

void CXDirect::OnGeom() 
{
	if(!m_pCurFoil)	return;

	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->m_OperDlgBar.SetAnimate(false);
	m_bPolar   = false;
	
	void* ptr = m_pCurOpp;
	m_pCurOpp = NULL;
	CreateOppCurves();
	UpdateView();

	CGeomDlg dlg(this);
	dlg.m_pXFoil      = m_pXFoil;
	dlg.m_pMemFoil    = m_pCurFoil;
	dlg.m_pBufferFoil = &m_BufferFoil;
	dlg.m_pChildView  = m_pChildWnd;
	

	if(dlg.DoModal() == IDOK)
	{
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		m_pCurOpp = (OpPoint*)ptr;
		pFrame->SetModFoil(pNewFoil);
		UpdateFoils();
	}
	else
	{
		m_pCurOpp = (OpPoint*)ptr;
		SetBufferFoil();
		InitXFoil();
	}
	UpdateView();
}

void CXDirect::OnNacaFoils() 
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	pFrame->m_OperDlgBar.SetAnimate(false);
	m_bPolar   = false;
	
	void* ptr0 = m_pCurFoil;
	void* ptr  = m_pCurOpp;
	m_pCurFoil = NULL;
	m_pCurOpp = NULL;

	CreateOppCurves();
	UpdateView();

	CNacaDlg dlg(this);

	dlg.m_pXFoil      = m_pXFoil;
	dlg.m_pBufferFoil = &m_BufferFoil;
	dlg.m_pChildView  = m_pChildWnd;

	if(dlg.DoModal() == IDOK)
	{
		CString str;
		if(dlg.m_Digits>0 && log10((double)dlg.m_Digits)<4)	str.Format("%04d", dlg.m_Digits);
		else												str.Format("%d", dlg.m_Digits);
		str = "NACA "+ str;

		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = PS_SOLID;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilName = str;
		m_pCurOpp = (OpPoint*)ptr;
		pFrame->SetModFoil(pNewFoil);
		UpdateFoils();
	}
	else
	{
		m_pCurFoil = (CFoil*)ptr0;
		m_pCurOpp  = (OpPoint*)ptr;
		SetBufferFoil();
		InitXFoil();
	}
	UpdateView();

}

void CXDirect::OnCpGraph() 
{
	m_bPolar = false;
	if(m_OppVar!=0) {
		m_pCpGraph->ResetLimits();
		m_pCpGraph->SetAuto(true);
	}
	m_OppVar = 0;
	m_pCpGraph->SetInverted(true);
	CreateOppCurves();
	m_pCpGraph->SetYTitle("Cp");

	CheckMenu();

	m_pCpGraph->SetXScale();
	SetFoilScale(m_rCltRect);
	UpdateView();
}


void CXDirect::OnQGraph()
{
	m_bPolar = false;
	if(m_OppVar!=1) {
		m_pCpGraph->ResetLimits();
		m_pCpGraph->SetAuto(true);
	}
	m_OppVar = 1;
	m_pCpGraph->SetInverted(false);
	CreateOppCurves();
	m_pCpGraph->SetYTitle("Q");

	CheckMenu();
	m_pCpGraph->SetXScale();
	SetFoilScale(m_rCltRect);
	UpdateView();
}


void CXDirect::OnHPlot() 
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_OppVar = 2;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle("Hk");
	CCurve * pTopCurve = m_pCpGraph->AddCurve();
	CCurve * pBotCurve = m_pCpGraph->AddCurve();
	pTopCurve->SetTitle("Top");
	pBotCurve->SetTitle("Bot");	

	double x[IVX][3],y[IVX][3];
	int nside1, nside2;

	m_pXFoil->CreateXBL(x, nside1, nside2);
	m_pXFoil->FillHk(y, nside1, nside2);

	for (i=2; i<=nside1-1; i++)
	{
		pTopCurve->AddPoint(x[i][1], y[i][1]);
	}
	for (i=2; i<=nside2-1; i++)
	{
		pBotCurve->AddPoint(x[i][2], y[i][2]);
	}

	m_pCpGraph->SetXScale();
	SetFoilScale(m_rCltRect);
	CheckMenu();
	UpdateView();
}



void CXDirect::OnUePlot() 
{
	if(!m_pXFoil->lvconv) return;
	int i;
	double x[IVX][3],y[IVX][3];
	double uei;
	int nside1, nside2, ibl;

	m_OppVar = 2;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle("Ue/Vinf");
	CCurve * pTopCurve = m_pCpGraph->AddCurve();
	CCurve * pBotCurve = m_pCpGraph->AddCurve();
	pTopCurve->SetTitle("Top");
	pBotCurve->SetTitle("Bot");	


	m_pXFoil->CreateXBL(x, nside1, nside2);
	//---- fill compressible ue arrays
	for (ibl=2; ibl<= nside1;ibl++){
		uei = m_pXFoil->uedg[ibl][1];
		y[ibl][1] = uei * (1.0-m_pXFoil->tklam) 
						/ (1.0-m_pXFoil->tklam*(uei/m_pXFoil->qinf)*(uei/m_pXFoil->qinf));
	}
	for (ibl=2; ibl<= nside2;ibl++){
		uei = m_pXFoil->uedg[ibl][2];
		y[ibl][2] = uei * (1.0-m_pXFoil->tklam) 
						/ (1.0-m_pXFoil->tklam*(uei/m_pXFoil->qinf)*(uei/m_pXFoil->qinf));
	}

	for (i=2; i<=nside1-1; i++){
		pTopCurve->AddPoint(x[i][1], y[i][1]);
	}
	for (i=2; i<=nside2-1; i++){
		pBotCurve->AddPoint(x[i][2], y[i][2]);
	}
	m_pCpGraph->SetXScale();
	SetFoilScale(m_rCltRect);
	CheckMenu();
	UpdateView();
	
}

void CXDirect::OnCfPlot() 
{
	if(!m_pXFoil->lvconv) return;
	int i;
	double x[IVX][3],y[IVX][3];
	int nside1, nside2, ibl;

	m_OppVar = 2;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle("Cf");
	CCurve * pTopCurve = m_pCpGraph->AddCurve();
	CCurve * pBotCurve = m_pCpGraph->AddCurve();
	pTopCurve->SetTitle("Top");
	pBotCurve->SetTitle("Bot");	

	double que = 0.5*m_pXFoil->qinf*m_pXFoil->qinf;

	m_pXFoil->CreateXBL(x, nside1, nside2);
	//---- fill compressible ue arrays
	for (ibl=2; ibl<= nside1;ibl++){
		y[ibl][1] = m_pXFoil->tau[ibl][1] / que;
	}
	for ( ibl=2; ibl<= nside2;ibl++){
		y[ibl][2] = m_pXFoil->tau[ibl][2] / que;
	}


	for (i=2; i<=nside1-1; i++){
		pTopCurve->AddPoint(x[i][1], y[i][1]);
	}
	for (i=2; i<=nside2-1; i++){
		pBotCurve->AddPoint(x[i][2], y[i][2]);
	}
	m_pCpGraph->SetXScale();
	SetFoilScale(m_rCltRect);
	CheckMenu();
	UpdateView();
		
}
 

void CXDirect::OnCdPlot() 
{
	if(!m_pXFoil->lvconv) return;
	double x[IVX][3],y[IVX][3];
	int nside1, nside2, ibl;
	int i;

	m_OppVar = 2;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle("Cd'");
	CCurve * pTopCurve = m_pCpGraph->AddCurve();
	CCurve * pBotCurve = m_pCpGraph->AddCurve();
	pTopCurve->SetTitle("Top");
	pBotCurve->SetTitle("Bot");	

	double qrf = m_pXFoil->qinf;

	m_pXFoil->CreateXBL(x, nside1, nside2);
	//---- fill compressible ue arrays
	for (ibl=2; ibl<= nside1;ibl++){
		y[ibl][1] = m_pXFoil->dis[ibl][1] / qrf/ qrf/ qrf;
	}
	for ( ibl=2; ibl<= nside2;ibl++){
		y[ibl][2] = m_pXFoil->dis[ibl][2] / qrf/ qrf/ qrf;
	}


	for (i=2; i<=nside1-1; i++){
		pTopCurve->AddPoint(x[i][1], y[i][1]);
	}
	for (i=2; i<=nside2-1; i++){
		pBotCurve->AddPoint(x[i][2], y[i][2]);
	}
	m_pCpGraph->SetXScale();
	SetFoilScale(m_rCltRect);
	CheckMenu();
	UpdateView();	
}


void CXDirect::OnNPlot() 
{
	if(!m_pXFoil->lvconv) return;
	int i;
	int nside1, nside2, ibl;

	m_OppVar = 2;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle("A/A0");
	CCurve * pTopCurve = m_pCpGraph->AddCurve();
	CCurve * pBotCurve = m_pCpGraph->AddCurve();
	pTopCurve->SetTitle("Top");
	pBotCurve->SetTitle("Bot");	

	double x[IVX][3],y[IVX][3];

	m_pXFoil->CreateXBL(x, nside1, nside2);

	for (ibl=2; ibl< nside1;ibl++){
		y[ibl][1] = m_pXFoil->ctau[ibl][1];
	}
	for ( ibl=2; ibl< nside2;ibl++){
		y[ibl][2] = m_pXFoil->ctau[ibl][2];
	}

	for (i=2; i<=m_pXFoil->itran[1]-2; i++){
		pTopCurve->AddPoint(x[i][1], y[i][1]);
	}
	for (i=2; i<=m_pXFoil->itran[2]-2; i++){
		pBotCurve->AddPoint(x[i][2], y[i][2]);
	}
	m_pCpGraph->SetXScale();
	SetFoilScale(m_rCltRect);
	CheckMenu();
	UpdateView();	
}


void CXDirect::OnRtPlot() 
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_OppVar = 2;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle("Re_Theta");
	CCurve * pTopCurve = m_pCpGraph->AddCurve();
	CCurve * pBotCurve = m_pCpGraph->AddCurve();
	pTopCurve->SetTitle("Top");
	pBotCurve->SetTitle("Bot");	

	double x[IVX][3],y[IVX][3];
	int nside1, nside2;

	m_pXFoil->CreateXBL(x, nside1, nside2);
	m_pXFoil->FillRTheta(y, nside1, nside2);

	for (i=2; i<=nside1-1; i++)	pTopCurve->AddPoint(x[i][1], y[i][1]);
	for (i=2; i<=nside2-1; i++) pBotCurve->AddPoint(x[i][2], y[i][2]);
	
	m_pCpGraph->SetXScale();
	SetFoilScale(m_rCltRect);
	CheckMenu();
	UpdateView();
}


void CXDirect::OnRtLPlot() 
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_OppVar = 2;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle("Re_Theta");
	CCurve * pTopCurve = m_pCpGraph->AddCurve();
	CCurve * pBotCurve = m_pCpGraph->AddCurve();
	pTopCurve->SetTitle("Top");
	pBotCurve->SetTitle("Bot");	

	double x[IVX][3],y[IVX][3];
	int nside1, nside2;

	m_pXFoil->CreateXBL(x, nside1, nside2);
	m_pXFoil->FillRTheta(y, nside1, nside2);

	for (i=2; i<=nside1-1; i++){
		if (y[i][1]>0.0) y[i][1] = log10( y[i][1] );
		else             y[i][1] = 0.0;
		pTopCurve->AddPoint(x[i][1], y[i][1]);
	}
	for (i=2; i<=nside2-1; i++){
		if (y[i][2]>0.0) y[i][2] = log10( y[i][2] );
		else             y[i][2] = 0.0;
		pBotCurve->AddPoint(x[i][2], y[i][2]);
	}
	m_pCpGraph->SetXScale();
	SetFoilScale(m_rCltRect);
	CheckMenu();
	UpdateView();
}


void CXDirect::OnDtPlot() 
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_OppVar = 2;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle(" ");
	

	double x[IVX][3];
	int nside1, nside2;

	CCurve * pCurve1 = m_pCpGraph->AddCurve();
	CCurve * pCurve2 = m_pCpGraph->AddCurve();

	pCurve1->SetTitle("D*");
	pCurve2->SetTitle("Theta");
	m_pXFoil->CreateXBL(x, nside1, nside2);

	for (i=2; i<nside1; i++){
		pCurve1->AddPoint(x[i][1], m_pXFoil->dstr[i][1]);
		pCurve2->AddPoint(x[i][1], m_pXFoil->thet[i][1]);
	}

	m_pCpGraph->SetXScale();
	SetFoilScale(m_rCltRect);
	CheckMenu();
	UpdateView();
}

void CXDirect::OnDbPlot() 
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_OppVar = 2;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle(" ");
	

	double x[IVX][3];
	int nside1, nside2;

	CCurve * pCurve1 = m_pCpGraph->AddCurve();
	CCurve * pCurve2 = m_pCpGraph->AddCurve();

	pCurve1->SetTitle("D*");
	pCurve2->SetTitle("Theta");
	m_pXFoil->CreateXBL(x, nside1, nside2);

	for (i=2; i<nside2; i++)
	{
		pCurve1->AddPoint(x[i][2], m_pXFoil->dstr[i][2]);
		pCurve2->AddPoint(x[i][2], m_pXFoil->thet[i][2]);
	}

	m_pCpGraph->SetXScale();
	SetFoilScale(m_rCltRect);
	CheckMenu();
	UpdateView();	
}


void CXDirect::OnExportCurrentResults()
{
	if(!m_pXFoil->lvconv) return;
	if(!m_pCurFoil)		  return;

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CString FileName, DestFileName, OutString, strong;
	CStdioFile DestFile;
	CFileException fe;

	double x[IVX][3],Hk[IVX][3],UeVinf[IVX][3], Cf[IVX][3], Cd[IVX][3], AA0[IVX][3];
	double RTheta[IVX][3], DStar[IVX][3], Theta[IVX][3];
	double uei;
	double que = 0.5*m_pXFoil->qinf*m_pXFoil->qinf;
	double qrf = m_pXFoil->qinf;
	int nside1, nside2, ibl, type;

	FileName = m_pCurFoil->m_FoilName;
	FileName.Replace("/", " ");

	static TCHAR BASED_CODE szFilter[] = _T("Text File (*.txt)|*.txt|") _T("CSV format (*.csv)|*.csv|") ;
	CFileDialog XFileDlg(false, "txt", FileName, OFN_OVERWRITEPROMPT, szFilter);
	XFileDlg.m_ofn.nFilterIndex = pFrame->m_TextFileFormat;

	if(IDOK==XFileDlg.DoModal()) 
	{
		pFrame->m_TextFileFormat = XFileDlg.m_ofn.nFilterIndex;
		DestFileName = XFileDlg.GetPathName();
		BOOL bOpen = DestFile.Open(DestFileName, CFile::modeCreate | CFile::modeWrite, &fe);
		if(bOpen)
		{
			type = XFileDlg.m_ofn.nFilterIndex;
			DestFile.WriteString(pFrame->m_VersionName);
			DestFile.WriteString("\n");
			strong = m_pXFoil->m_FoilName+ "\n";
			DestFile.WriteString(strong);

			if(type==1) strong.Format("Alpha = %5.1f,  Re = %5.1f,  Ma= %6.4f,  ACrit=%4.1f \n\n",	m_pXFoil->alfa*180./pi, m_pXFoil->reinf1, m_pXFoil->minf1, m_pXFoil->acrit);
			else        strong.Format("Alpha =, %5.1f,Re =, %5.1f,Ma=, %6.4f,ACrit =,%4.1f \n\n",	m_pXFoil->alfa*180./pi, m_pXFoil->reinf1, m_pXFoil->minf1, m_pXFoil->acrit);
			DestFile.WriteString(strong);

			m_pXFoil->CreateXBL(x, nside1, nside2);
			//write top first
			m_pXFoil->FillHk(Hk, nside1, nside2);
			for (ibl=2; ibl<= nside1;ibl++)
			{
				uei = m_pXFoil->uedg[ibl][1];
				UeVinf[ibl][1] = uei * (1.0-m_pXFoil->tklam) 
								/ (1.0-m_pXFoil->tklam*(uei/m_pXFoil->qinf)*(uei/m_pXFoil->qinf));
			}
			for (ibl=2; ibl<= nside2;ibl++)
			{
				uei = m_pXFoil->uedg[ibl][2];
				UeVinf[ibl][2] = uei * (1.0-m_pXFoil->tklam) 
								/ (1.0-m_pXFoil->tklam*(uei/m_pXFoil->qinf)*(uei/m_pXFoil->qinf));
			}
			//---- fill compressible ue arrays
			for (ibl=2; ibl<= nside1;ibl++)	Cf[ibl][1] = m_pXFoil->tau[ibl][1] / que;
			for (ibl=2; ibl<= nside2;ibl++)	Cf[ibl][2] = m_pXFoil->tau[ibl][2] / que;
			
			//---- fill compressible ue arrays
			for (ibl=2; ibl<= nside1;ibl++)	Cd[ibl][1] = m_pXFoil->dis[ibl][1] / qrf/ qrf/ qrf;
			for (ibl=2; ibl<= nside2;ibl++)	Cd[ibl][2] = m_pXFoil->dis[ibl][2] / qrf/ qrf/ qrf;
			//NPlot
			for (ibl=2; ibl< nside1;ibl++)	AA0[ibl][1] = m_pXFoil->ctau[ibl][1];
			for (ibl=2; ibl< nside2;ibl++)	AA0[ibl][2] = m_pXFoil->ctau[ibl][2];

			m_pXFoil->FillRTheta(RTheta, nside1, nside2);
			for (ibl=2; ibl<= nside1; ibl++)
			{
				DStar[ibl][1] = m_pXFoil->dstr[ibl][1];
				Theta[ibl][1] = m_pXFoil->thet[ibl][1];
			}
			for (ibl=2; ibl<= nside2; ibl++)
			{
				DStar[ibl][2] = m_pXFoil->dstr[ibl][2];
				Theta[ibl][2] = m_pXFoil->thet[ibl][2];
			}

			DestFile.WriteString("\nTop Side\n");
			if(type==1) OutString.Format("    x         Hk     Ue/Vinf      Cf        Cd     A/A0       D*       Theta      CTq\n");
			else        OutString.Format("x,Hk,Ue/Vinf,Cf,Cd,A/A0,D*,Theta,CTq\n");
			DestFile.WriteString(OutString);
			for (ibl=2; ibl<nside1; ibl++)
			{
				if(type==1) 
					OutString.Format("%8.5f  %8.5f  %8.5f  %8.5f  %8.5f %8.5f  %8.5f  %8.5f  %8.5f\n",
					x[ibl][1], Hk[ibl][1],  UeVinf[ibl][1], Cf[ibl][1], Cd[ibl][1], AA0[ibl][1], DStar[ibl][1],  Theta[ibl][1],  m_pXFoil->ctq[ibl][1]);
				else 
					OutString.Format("%8.5f,%8.5f,%8.5f,%8.5f,%8.5f,%8.5f,%8.5f,%8.5f,%8.5f\n",
					x[ibl][1], Hk[ibl][1],  UeVinf[ibl][1], Cf[ibl][1], Cd[ibl][1], AA0[ibl][1], DStar[ibl][1],  Theta[ibl][1],  m_pXFoil->ctq[ibl][1]);
				DestFile.WriteString(OutString);
			}
			DestFile.WriteString("\n\nBottom Side\n");
			if(type==1) OutString.Format("    x         Hk     Ue/Vinf      Cf        Cd     A/A0       D*       Theta      CTq\n");
			else        OutString.Format("x,Hk,Ue/Vinf,Cf,Cd,A/A0,D*,Theta,CTq\n");
			DestFile.WriteString(OutString);
			for (ibl=2; ibl<nside2; ibl++)
			{
				if(type==1) 
					OutString.Format("%8.5f  %8.5f  %8.5f  %8.5f  %8.5f %8.5f  %8.5f  %8.5f  %8.5f\n",
					x[ibl][2], Hk[ibl][2],  UeVinf[ibl][2], Cf[ibl][2], Cd[ibl][2], AA0[ibl][2], DStar[ibl][2],  Theta[ibl][2],  m_pXFoil->ctq[ibl][2]);
				else 
					OutString.Format("%8.5f,%8.5f, %8.5f,%8.5f,%8.5f,%8.5f,%8.5f,%8.5f,%8.5f\n",
					x[ibl][2], Hk[ibl][2],  UeVinf[ibl][2], Cf[ibl][2], Cd[ibl][2], AA0[ibl][2], DStar[ibl][2],  Theta[ibl][2],  m_pXFoil->ctq[ibl][2]);
				DestFile.WriteString(OutString);
			}
		}
		DestFile.Close();
	}
}


void CXDirect::OnCtPlot() 
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_OppVar = 2;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle("Max Shear");
	CCurve * pCurve0 = m_pCpGraph->AddCurve();
	CCurve * pCurve1 = m_pCpGraph->AddCurve();
	CCurve * pCurve2 = m_pCpGraph->AddCurve();
	CCurve * pCurve3 = m_pCpGraph->AddCurve();
	pCurve0->SetTitle("Top Shear");
	pCurve1->SetTitle("Top Shear eq");	
	pCurve2->SetTitle("Bot Shear");
	pCurve3->SetTitle("Bot Shear eq");	

	double x[IVX][3];
	int nside1, nside2;

 	m_pXFoil->CreateXBL(x, nside1, nside2);

	int it1 = m_pXFoil->itran[1];
	int it2 = m_pXFoil->itran[2];

	for (i=it1; i<=nside1-1; i++)	pCurve0->AddPoint(x[i][1], m_pXFoil->ctau[i][1]);
	for (i=2; i<=nside1-1; i++)		pCurve1->AddPoint(x[i][1], m_pXFoil->ctq[i][1]);
	
	for (i=it2; i<=nside2-1; i++)	pCurve2->AddPoint(x[i][2], m_pXFoil->ctau[i][2]);
	for (i=2; i<=nside2-1; i++)		pCurve3->AddPoint(x[i][2], m_pXFoil->ctq[i][2]);
	
	m_pCpGraph->SetXScale();
	SetFoilScale(m_rCltRect);
	CheckMenu();
	UpdateView();	
	
}
void CXDirect::OnImportXFoilPolar()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CStdioFile XFile;
	CFileException fe;
	CString FileName, PolarName, FoilName;
	CPolar *pPolar = new CPolar(m_pFrame);
	CString strong, str;
	double Re;
	int res;
	double alpha,CL, CD, CDp, CM, Xt,  Xb, Cpmn, HMom;

	CFileDialog XFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY,  
					 _T("XFoil Polar Format |*.*|"));
	if(IDOK==XFileDlg.DoModal()) {
		FileName = XFileDlg.GetPathName();
//	need to select the foil to which the polars will be associated		
		BOOL bOpen = XFile.Open(FileName, CFile::modeRead, &fe);
		if (bOpen){
			try{
				BOOL bRead = XFile.ReadString(strong);// blank line
				bRead = XFile.ReadString(strong);     // XFoil or XFLR5 version
				bRead = XFile.ReadString(strong);     // blank line
				bRead = XFile.ReadString(strong);     // Foil Name
				FoilName = strong.Right(strong.GetLength()-22);
				FoilName.TrimLeft();	FoilName.TrimRight();
				if(!GetFoil(FoilName)){
					str = "No Foil with the name "+FoilName; 
					str+= "\ncould be found. The polar(s) will not be stored";
					AfxMessageBox(str, MB_OK);
					delete pPolar;
					return;
				}
				pPolar->m_FoilName = FoilName;

				bRead = XFile.ReadString(strong);     // blank line
				bRead = XFile.ReadString(strong);     // analysis type
				res = sscanf(strong, "%d%d", &pPolar->m_ReType,&pPolar->m_MaType);			
				if(res !=2) {
					CFileException *pfe = new CFileException(CFileException::invalidFile);
					pfe->m_strFileName = FileName;
					throw pfe;
				}

				if     (pPolar->m_ReType ==1 && pPolar->m_MaType ==1) pPolar->m_Type = 1;
				else if(pPolar->m_ReType ==2 && pPolar->m_MaType ==2) pPolar->m_Type = 2;
				else if(pPolar->m_ReType ==3 && pPolar->m_MaType ==1) pPolar->m_Type = 3;
				else                                                  pPolar->m_Type = 1;

				bRead = XFile.ReadString(strong);     // blank line

				bRead = XFile.ReadString(strong);
				if(strong.GetLength() < 34){
					CFileException *pfe = new CFileException(CFileException::invalidFile);
					pfe->m_strFileName = FileName;
					throw pfe;
				}
				res  = sscanf(strong.Mid(9,6), "%lf", &pPolar->m_XTop);   
				if(res !=1) {
					CFileException *pfe = new CFileException(CFileException::invalidFile);
					pfe->m_strFileName = FileName;
					throw pfe;
				}
				res += sscanf(strong.Mid(28,6), "%lf", &pPolar->m_XBot);
				if(res !=2) {
					CFileException *pfe = new CFileException(CFileException::invalidFile);
					pfe->m_strFileName = FileName;
					throw pfe;
				}

				// Mach     Re     NCrit
				bRead = XFile.ReadString(strong);     
				if(strong.GetLength() < 58){
					CFileException *pfe = new CFileException(CFileException::invalidFile);
					pfe->m_strFileName = FileName;
					throw pfe;
				}
				res  = sscanf(strong.Mid(8,6), "%lf", &pPolar->m_Mach);// Mach 
				if(res!=1){
					CFileException *pfe = new CFileException(CFileException::invalidFile);
					pfe->m_strFileName = FileName;
					throw pfe;
				}

				res += sscanf(strong.Mid(24,18), "%lf", &Re);// Re 
				if(res!=2){
					CFileException *pfe = new CFileException(CFileException::invalidFile);
					pfe->m_strFileName = FileName;
					throw pfe;
				}
				res += sscanf(strong.Mid(52,8), "%lf", &pPolar->m_ACrit);// NCrit 
				Re = Re*1000000.0;
				if(res !=3) {
					CFileException *pfe = new CFileException(CFileException::invalidFile);
					pfe->m_strFileName = FileName;
					throw pfe;
				}
				pPolar->m_Reynolds = Re;

				bRead = XFile.ReadString(strong);     // blank line
				bRead = XFile.ReadString(strong);     // column titles
				bRead = XFile.ReadString(strong);     // underscores

				while( bRead){
					bRead = XFile.ReadString(strong);// polar data
					if(bRead){
						if(strong.GetLength()){
							res = sscanf(strong, "%lf%lf%lf%lf%lf%lf%lf%lf%lf", &alpha, &CL, &CD, &CDp, &CM, &Xt, &Xb, &Cpmn, &HMom);

							if (res == 7) {
								pPolar->AddPoint(alpha, CD, CDp, CL, CM, Xt, Xb, 0.0, 0.0,Re,0.0);
							}
							else if(res == 9){
								pPolar->AddPoint(alpha, CD, CDp, CL, CM, Xt, Xb, Cpmn, HMom,Re,0.0);
							}
							else{
								bRead = false;
							}
						}
					}				
				}
				XFile.Close();

				Re = pPolar->m_Reynolds/1000000.0;
				pPolar->m_PlrName.Format("T%d_Re%.2f_M%.2f", pPolar->m_Type, Re, pPolar->m_Mach);
				str.Format("_N%.1f", pPolar->m_ACrit);
				pPolar->m_PlrName += str;

				pPolar->m_Color = pFrame->GetColor(1);

				m_pCurPolar = AddPolar(pPolar);

				m_pCurOpp = NULL;
				UpdatePlrs();
				SetPolar();
				OnPolars();
				
			}
			catch (CException *ex){
				TCHAR   szCause[255];
				CString str;
				ex->GetErrorMessage(szCause, 255);
				str = _T("Error loading polar : ");
				str += szCause;
				AfxMessageBox(str);
				if(pPolar) delete pPolar;
				ex->Delete();
			}
		}
	}
}


void CXDirect::OnImportJavaFoilPolar()
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	CStdioFile XFile;
	CFileException fe;
	CString FileName, PolarName, FoilName;
	CString strong, str;
	bool bResult = true;
	bool bIsReading = true;
	int res;
	int NPolars = 0;
	double Re;
	CFileDialog XFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY,  
					 _T("JavaFoil Polar Format |*.*|"));
	if(IDOK==XFileDlg.DoModal()) {
		FileName = XFileDlg.GetPathName();
//	need to select the foil to which the polars will be associated		

		try{
			BOOL bOpen = XFile.Open(FileName, CFile::modeRead, &fe);
			if (bOpen){
			
				double alpha,CL, CD, CM, Xt,  Xb;
				BOOL bRead = XFile.ReadString(FoilName);// Foil Name
				FoilName.TrimLeft();
				FoilName.TrimRight();
				if(!GetFoil(FoilName)){
					str = "No Foil with the name "+FoilName; 
					str+= "\ncould be found. The polar(s) will not be stored";
					AfxMessageBox(str, MB_OK);
					return;
				}
				bRead = XFile.ReadString(strong);//blank line

				while(bIsReading){

					bRead = XFile.ReadString(strong);//Re number
					strong = strong.Right(strong.GetLength()-4);
					res = sscanf(strong, "%lf",&Re);
					if(res !=1){
						bIsReading = false;
					}
					else{
						CPolar *pPolar = new CPolar(m_pFrame);
						pPolar->m_FoilName = FoilName;
						pPolar->m_Reynolds = Re;;
						pPolar->m_PlrName.Format("T%d_Re%.2f_M%.2f_JavaFoil", 
							pPolar->m_Type, 
							pPolar->m_Reynolds/1000000.0,
							pPolar->m_Mach);

						pPolar->m_Color = pFrame->GetColor(1);
						m_pCurPolar = AddPolar(pPolar);
						NPolars++;

						bRead = XFile.ReadString(strong);//?	Cl	Cd	Cm 0.25	TU	TL	SU	SL	L/D
						bRead = XFile.ReadString(strong);//[°]	[-]	[-]	[-]	[-]	[-]	[-]	[-]	[-]
						res = 6;
						while(res==6){
							bRead = XFile.ReadString(strong);//values
							strong.TrimLeft();
							strong.TrimRight();
							if(strong.GetLength()){
								strong.Replace(',', '.');
								res = sscanf(strong, "%lf%lf%lf%lf%lf%lf",&alpha, &CL, &CD, &CM, &Xt, &Xb);
								if (res == 6) 	pPolar->AddPoint(alpha, CD, 0.0, CL, CM, Xt, Xb, 0.0, 0.0, Re,0.0);
							}
							else {
								res = 0;
							}
						}
					}
				}
			}
 
			m_pCurOpp = NULL;
			UpdatePlrs();
			SetPolar();
			OnPolars();
		}
		catch (CException *ex){
			TCHAR   szCause[255];
			CString str;
			ex->GetErrorMessage(szCause, 255);
			str = _T("Error loading polar : ");
			str += szCause;
			AfxMessageBox(str);
			ex->Delete();
		}
	}
}




void CXDirect::OnCpi()
{
	m_bShowInviscid = !m_bShowInviscid;
	CheckMenu();
	CreateOppCurves();
	UpdateView();
}

void CXDirect::OnShowCurOpp()
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	m_bCurOppOnly = !m_bCurOppOnly;	
	CMenu *pMenu = m_pFrame->GetMenu();
	if(m_bCurOppOnly) pMenu->GetSubMenu(5)->CheckMenuItem(IDM_SHOWCUROPP, MF_BYCOMMAND | MF_CHECKED);
	else              pMenu->GetSubMenu(5)->CheckMenuItem(IDM_SHOWCUROPP, MF_BYCOMMAND | MF_UNCHECKED);
	if(m_pCurOpp) m_pCurOpp->m_bIsVisible = true;
	CreateOppCurves();
//	pFrame->SetSaveState(false);
	pFrame->m_PolarDlgBar.SetParams();
	UpdateView();
}

void CXDirect::OnShowOpps()
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	OpPoint *pOpp;
	for(int i=0; i<m_poaOpp->GetSize(); i++){
		pOpp = (OpPoint*)m_poaOpp->GetAt(i);
		pOpp->m_bIsVisible = true;
	}
	CreateOppCurves();
	pFrame->m_PolarDlgBar.SetParams();
	UpdateView();
}

void CXDirect::OnHideOpps()
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	OpPoint *pOpp;
	for(int i=0; i<m_poaOpp->GetSize(); i++){
		pOpp = (OpPoint*)m_poaOpp->GetAt(i);
		pOpp->m_bIsVisible = false;
	}
	CreateOppCurves();
	pFrame->m_PolarDlgBar.SetParams();
	UpdateView();
}

void CXDirect::OnShowFoilOpps()
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	if(!m_pCurFoil) return;
	OpPoint *pOpp;
	for(int i=0; i<m_poaOpp->GetSize(); i++){
		pOpp = (OpPoint*)m_poaOpp->GetAt(i);
		if(pOpp->m_strFoilName == m_pCurFoil->m_FoilName)
			pOpp->m_bIsVisible = true;
	}
	CreateOppCurves();
	pFrame->m_PolarDlgBar.SetParams();
	UpdateView();
}

void CXDirect::OnHideFoilOpps()
{
	CMainFrame* pFrame = (CMainFrame*)m_pFrame;
	if(!m_pCurFoil) return;
	OpPoint *pOpp;
	for(int i=0; i<m_poaOpp->GetSize(); i++){
		pOpp = (OpPoint*)m_poaOpp->GetAt(i);
		if(pOpp->m_strFoilName == m_pCurFoil->m_FoilName)
			pOpp->m_bIsVisible = false;
	}
	CreateOppCurves();
	pFrame->m_PolarDlgBar.SetParams();
	UpdateView();
}

void CXDirect::CreateOppCurves(OpPoint *pOpp)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;
	OpPoint *pOpPoint = NULL;;
	if(pOpp) pOpPoint = pOpp; else pOpPoint = m_pCurOpp;
	CCurve *pCurve1;
	CString str;
	int k;
	
	m_pCpGraph->DeleteCurves();

	if(m_bCurOppOnly || pOpp){
		if(!pOpPoint || !pOpPoint->m_bIsVisible) return;
		pCurve1    = m_pCpGraph->AddCurve();
		if(pOpPoint->m_bShowPoints) pCurve1->ShowPoints(true);
		
		pCurve1->SetStyle(pOpPoint->m_Style);
		pCurve1->SetColor(pOpPoint->m_Color);
		pCurve1->SetWidth(pOpPoint->m_Width);
		str.Format("-Re=%8.0f-Alpha=%5.2f", pOpPoint->Reynolds,pOpPoint->Alpha);
		str = pOpPoint->m_strFoilName+str;
		pCurve1->SetTitle(str);

		FillOppCurve(pOpPoint, m_pCpGraph, pCurve1);
	}
	else{
		for (k=0; k<m_poaOpp->GetSize(); k++){
			pOpp = (OpPoint*)m_poaOpp->GetAt(k);
			if (pOpp->m_bIsVisible){
				pCurve1    = m_pCpGraph->AddCurve();

				if(pOpp->m_bShowPoints) pCurve1->ShowPoints(true);

				pCurve1->SetStyle(pOpp->m_Style);
				pCurve1->SetColor(pOpp->m_Color);
				pCurve1->SetWidth(pOpp->m_Width);

				str.Format("-Re=%8.0f-Alpha=%5.2f", pOpp->Reynolds,pOpp->Alpha);
				str = pOpp->m_strFoilName+str;
				pCurve1->SetTitle(str);

				FillOppCurve(pOpp, m_pCpGraph, pCurve1);
			}
		}
	}
	if(m_bShowInviscid && pOpPoint){		
		CCurve *pCpi = m_pCpGraph->AddCurve();
		if(pOpPoint->m_bShowPoints) pCpi->ShowPoints(true);
		pCpi->SetStyle(PS_DASH);
		pCpi->SetColor(pOpPoint->m_Color);
		pCpi->SetWidth(pOpPoint->m_Width);
		str.Format("-Re=%8.0f-Alpha=%5.2f_Inviscid", pOpPoint->Reynolds,pOpPoint->Alpha);
		str = pOpPoint->m_strFoilName+str;
		pCpi->SetTitle(str);
		FillOppCurve(pOpPoint, m_pCpGraph, pCpi, true);
	}
}


void CXDirect::FillOppCurve(OpPoint *pOpp, Graph *pGraph, CCurve *pCurve, bool bInviscid)
{
	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	switch(m_OppVar){
		case 0:{
			for (int j=0; j<pOpp->n; j++){
				if(!bInviscid){
					if(pOpp->m_bVisc)
						pCurve->AddPoint(pOpp->x[j], pOpp->Cpv[j]);
				}
				else{
					pCurve->AddPoint(pOpp->x[j], pOpp->Cpi[j]);
				}
			}
			pGraph->SetYTitle("Cp");
			break;

		}
		case 1:{
			for (int j=0; j<pOpp->n; j++){
				if(!bInviscid){
					if(pOpp->m_bVisc)
						pCurve->AddPoint(pOpp->x[j], pOpp->Qv[j]);
				}
				else{
					pCurve->AddPoint(pOpp->x[j], pOpp->Qi[j]);
				}
			}
			pGraph->SetYTitle("Q");
			break;
		}
		default:{
			for (int j=0; j<pOpp->n; j++){
				if(!bInviscid){
					if(pOpp->m_bVisc)
						pCurve->AddPoint(pOpp->x[j], pOpp->Cpv[j]);
				}
				else{
					pCurve->AddPoint(pOpp->x[j], pOpp->Cpi[j]);
				}
			}
			pGraph->SetYTitle("Cp");
			break;
		}
	}
}


void CXDirect::OnManageFoils()
{
	m_UFOdlg.m_pFrame    = m_pFrame;
	m_UFOdlg.m_pXDirect    = this;
	m_UFOdlg.m_poaFoil   = m_poaFoil;
	m_UFOdlg.m_bWing     = false;

	m_UFOdlg.DoModal();
	UpdateView();
}

void CXDirect::OnShowCpGraph()
{
	m_bCpGraph = ! m_bCpGraph;
	CheckMenu();
	UpdateView();
}



void CXDirect::PaintImage(ATL::CImage *pImage, CString &FileName, int FileType)
{
	//Refresh the active view
	if(m_bAnimate) return;// painting is performed elsewhere

	HRESULT hResult;

	CDC *pDC;
	CChildView * pChildView = (CChildView*)m_pChildWnd;
	pDC = m_pChildWnd->GetDC();
	if(!pDC) return;

	CDC memdc; 
	memdc.CreateCompatibleDC(pDC);

	CBitmap  bmp;
	bmp.CreateCompatibleBitmap(pDC, m_rCltRect.Width(), m_rCltRect.Height());
	CBitmap * pOldBmp = memdc.SelectObject(&bmp);

	memdc.FillSolidRect(&m_rCltRect,pChildView->m_crBackColor);
	memdc.SetBkMode(TRANSPARENT);

	if(m_bPolar)
	{
		if     (m_iPlrView == 0) PaintPolarGraphs(&memdc, &m_rCltRect);
		else if(m_iPlrView == 1) PaintSingleGraph(&memdc, &m_rSingleRect);
		else if(m_iPlrView == 2) PaintCoupleGraph(&memdc, &m_rCltRect);
	} 
	else
	{
		PaintOpPoint(&memdc, &m_rCltRect, &m_rDrawRect);
	}

	memdc.SelectObject(pOldBmp);
	memdc.DeleteDC();

	HBITMAP hBmp = (HBITMAP)bmp;
	pImage->Attach(hBmp);

	if(FileType==1)      hResult = pImage->Save(_T(FileName), Gdiplus::ImageFormatBMP);
	else if(FileType==2) hResult = pImage->Save(_T(FileName), Gdiplus::ImageFormatJPEG);
	else if(FileType==3) hResult = pImage->Save(_T(FileName), Gdiplus::ImageFormatGIF);
	else if(FileType==4) hResult = pImage->Save(_T(FileName), Gdiplus::ImageFormatPNG);

	if (FAILED(hResult)) 
	{
		CString fmt;
		fmt.Format("Save image failed:\n%x ", hResult);
		AfxMessageBox(fmt);
		return;
	}

	bmp.DeleteObject();
	
	pChildView->ReleaseDC(pDC);
}



void CXDirect::OnExportGraphToFile()
{
	if(!m_pCurGraph) return;

	CString FileName, str;
	CStdioFile XFile;

	CMainFrame *pFrame = (CMainFrame*)m_pFrame;

	CFileException fe;
	CString strong;

	m_pCurGraph->GetYTitle(FileName);
	FileName.Replace("/", " ");

	static TCHAR BASED_CODE szFilter[] = _T("Text File (*.txt)|*.txt|") _T("CSV format (*.csv)|*.csv|") ;
	CFileDialog XFileDlg(false, "txt", FileName, OFN_OVERWRITEPROMPT, szFilter);
	XFileDlg.m_ofn.nFilterIndex = pFrame->m_TextFileFormat;

	if(IDOK==XFileDlg.DoModal()) 
	{
		pFrame->m_TextFileFormat = XFileDlg.m_ofn.nFilterIndex;

		FileName = XFileDlg.GetPathName();
		BOOL bOpen = XFile.Open(FileName, CFile::modeCreate | CFile::modeWrite, &fe);

		if(bOpen)
		{
			m_pCurGraph->ExportToFile(XFile, XFileDlg.m_ofn.nFilterIndex);
		}
	}
}