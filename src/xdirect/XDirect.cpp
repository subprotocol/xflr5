/****************************************************************************

	QXDirect Class
	Copyright (C) 2008-2013 Andre Deperrois adeperrois@xflr5.com

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

#include <QCheckBox>
#include <QTimer>
#include <QAction>
#include <QMenu>
#include <QStatusBar>
#include <QMessageBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QGroupBox>
#include <QThread>
#include <math.h>
#include <QtDebug>

#include "../globals.h"
#include "../mainframe.h"
#include "XDirect.h"
#include "BatchThreadDlg.h"



void *QXDirect::s_pMainFrame;
void *QXDirect::s_p2DWidget;

/**
*The public constructor.
*/
QXDirect::QXDirect(QWidget *parent) : QWidget(parent)
{
	MainFrame*pMainFrame = (MainFrame*)s_pMainFrame;


	SetupLayout();

	m_pXFoil = NULL;
	m_pXFoil = new XFoil();

	m_pPolarFilterDlg   = new PolarFilterDlg(pMainFrame);
	m_pObjectPropsDlg   = new ObjectPropsDlg(pMainFrame);
	m_pGraphDlg         = new GraphDlg(pMainFrame);
	m_pRenameDlg        = new RenameDlg(pMainFrame);
	m_pEditPlrDlg       = new EditPlrDlg(pMainFrame);
	m_pXFoilAnalysisDlg = new XFoilAnalysisDlg(pMainFrame);
	m_pFPDlg            = new FoilPolarDlg(pMainFrame);
	m_pBDlg             = new BatchDlg(pMainFrame);
//	m_pBatchThreadDlg   = new BatchThreadDlg(pMainFrame);
	m_pXFoilAdvancedDlg = new XFoilAdvancedDlg(pMainFrame);
	m_pXDirectStyleDlg  = new XDirectStyleDlg(pMainFrame);
	m_pManageFoilsDlg   = new ManageFoilsDlg(pMainFrame);

	m_pTwoDPanelDlg     = new TwoDPanelDlg(pMainFrame);
	m_pIFDlg            = new InterpolateFoilsDlg(pMainFrame);
	m_pNacaFoilDlg      = new NacaFoilDlg(pMainFrame);
	m_pFoilCoordDlg     = new FoilCoordDlg(pMainFrame);
	m_pFoilGeomDlg      = new FoilGeomDlg(pMainFrame);
	m_pTEGapDlg         = new TEGapDlg(pMainFrame);
	m_pLEDlg            = new LEDlg(pMainFrame);
	m_pFlapDlg          = new FlapDlg(pMainFrame);
	m_pCAddDlg          = new CAddDlg(pMainFrame);


	m_CurveStyle = 0;
	m_CurveWidth = 1;
	m_CurveColor = QColor(0,0,0);

	m_pAnimateTimer = new QTimer(this);
	m_posAnimate = 0; // no animation to start with
	Connect();

	FillComboBoxes(false);

	m_bAnimate        = false;
	m_bAnimatePlus    = false;
	m_bAutoInitBL     = true;
	m_bCpGraph        = true;
	m_bTransGraph     = false;
	m_bShowPanels     = false;
	m_bShowUserGraph  = true;
	m_bSequence       = false;
	m_bHighlightOpp   = false;

	m_bXPressed = m_bYPressed = false;

	m_PolarType       = FIXEDSPEEDPOLAR;

	m_bTrans          = false;
	m_bType1          = true;
	m_bType2          = true;
	m_bType3          = true;
	m_bType4          = true;
	m_bPressure       = false;
	m_bBL             = false;
	m_bFromList       = false;
	m_bFromZero       = false;
	m_bShowTextOutput = true;
	m_bNeutralLine    = true;
	m_bShowInviscid   = false;
	m_bAlpha          = true;
	m_bInitBL         = true;
	m_bStoreOpp       = true;
	m_bViscous        = true;
	m_bCurOppOnly     = true;

	m_iNeutralStyle = 2;
	m_iNeutralWidth = 1;
	m_crNeutralColor = QColor(200,200,255);

	m_crBLColor = QColor(200,70,70);
	m_iBLStyle = 1;
	m_iBLWidth = 1;
	m_crPressureColor= QColor(0,255,0);
	m_iPressureStyle = 0;
	m_iPressureWidth = 1;

	m_IterLim   = 100;

	m_bPolarView          = true;
	m_iPlrGraph = 0;
	m_iPlrView  = ALLPOLARGRAPHS;
	m_XFoilVar  = 0;
	m_FoilYPos  = 150;

	m_PointDown.setX(0);
	m_PointDown.setY(0);

	m_posAnimate = 0;

	m_pCpGraph     = NULL;
	m_pCurGraph    = NULL;
	m_pCurPolar    = NULL;
	m_pCurOpp      = NULL;

	BatchDlg::s_bStoreOpp = false;

	m_NCrit = 9.0;
	m_XTopTr = 1.0;
	m_XBotTr = 1.0;
	m_Mach = 0.0;

	m_Alpha      = 0.0;
	m_AlphaMax   = 1.0;
	m_AlphaDelta = 0.5;
	m_Cl         = 0.0;
	m_ClMax      = 1.0;
	m_ClDelta    = 0.1;
	m_Reynolds      = 100000.0;
	m_ReynoldsMax   = 150000.0;
	m_ReynoldsDelta =  10000.0;

	m_ASpec = 0.0;

	m_pCpGraph    = new QGraph();

	m_pCpGraph->SetVariables(0,0);

	m_PlrGraph[0].SetVariables(2,1);
	m_PlrGraph[1].SetVariables(0,1);
	m_PlrGraph[2].SetVariables(0,5);
	m_PlrGraph[3].SetVariables(6,1);
	m_PlrGraph[4].SetVariables(0,10);

	m_pCpGraph->SetXTitle(tr("X"));
	m_pCpGraph->SetYTitle(tr("Cp"));
	m_pCpGraph->SetInverted(true);
	m_pCpGraph->SetXMin(0.0);
	m_pCpGraph->SetXMax(1.0);
	m_pCpGraph->SetYMin(-0.1);
	m_pCpGraph->SetYMax(0.1);
	m_pCpGraph->SetMargin(50);
	m_pCpGraph->SetBorderColor(QColor(200,200,200));
	m_pCpGraph->SetBorder(true);
	m_pCpGraph->SetBorderStyle(0);
	m_pCpGraph->SetGraphName(tr("Cp Graph"));

	m_PlrGraph[0].SetXMin(0.0);
	m_PlrGraph[0].SetXMax(0.1);
	m_PlrGraph[0].SetYMin(-0.1);
	m_PlrGraph[0].SetYMax(0.1);
	m_PlrGraph[0].SetType(2);
	m_PlrGraph[0].SetBorderColor(QColor(200,200,200));
	m_PlrGraph[0].SetBorder(true);
	m_PlrGraph[0].SetBorderStyle(0);
	m_PlrGraph[0].SetBorderWidth(3);
	m_PlrGraph[0].SetMargin(50);
	m_PlrGraph[0].SetGraphName(tr("Polar Graph"));

	m_PlrGraph[2].SetXMin(0.0);
	m_PlrGraph[2].SetXMax(0.1);
	m_PlrGraph[2].SetYMin(-0.1);
	m_PlrGraph[2].SetYMax(0.1);
	m_PlrGraph[2].SetType(2);
	m_PlrGraph[2].SetBorderColor(QColor(200,200,200));
	m_PlrGraph[2].SetBorder(true);
	m_PlrGraph[2].SetBorderStyle(0);
	m_PlrGraph[2].SetBorderWidth(3);
	m_PlrGraph[2].SetMargin(50);
	m_PlrGraph[2].SetGraphName(tr("Cm Graph"));

	m_PlrGraph[1].SetXMin(0.0);
	m_PlrGraph[1].SetXMax(0.1);
	m_PlrGraph[1].SetYMin(-0.1);
	m_PlrGraph[1].SetYMax(0.1);
	m_PlrGraph[1].SetType(2);
	m_PlrGraph[1].SetBorderColor(QColor(200,200,200));
	m_PlrGraph[1].SetBorder(true);
	m_PlrGraph[1].SetBorderStyle(0);
	m_PlrGraph[1].SetBorderWidth(3);
	m_PlrGraph[1].SetMargin(50);
	m_PlrGraph[1].SetGraphName(tr("Cz Graph"));

	m_PlrGraph[3].SetXMin(0.0);
	m_PlrGraph[3].SetXMax(0.1);
	m_PlrGraph[3].SetYMin(-0.1);
	m_PlrGraph[3].SetYMax(0.1);
	m_PlrGraph[3].SetType(2);
	m_PlrGraph[3].SetBorderColor(QColor(200,200,200));
	m_PlrGraph[3].SetBorder(true);
	m_PlrGraph[3].SetBorderStyle(0);
	m_PlrGraph[3].SetBorderWidth(3);
	m_PlrGraph[3].SetMargin(50);
	m_PlrGraph[3].SetGraphName(tr("Tr Graph"));

	m_PlrGraph[4].SetXMin(0.0);
	m_PlrGraph[4].SetXMax(0.1);
	m_PlrGraph[4].SetYMin(-0.1);
	m_PlrGraph[4].SetYMax(0.1);
	m_PlrGraph[4].SetType(2);
	m_PlrGraph[4].SetBorderColor(QColor(200,200,200));
	m_PlrGraph[4].SetBorder(true);
	m_PlrGraph[4].SetBorderStyle(0);
	m_PlrGraph[4].SetBorderWidth(3);
	m_PlrGraph[4].SetMargin(50);
	m_PlrGraph[4].SetGraphName(tr("User Graph"));

	for(int ig=0; ig<MAXPOLARGRAPHS; ig++)
		SetGraphTitles(m_PlrGraph+ig);

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

	m_FoilOffset.setX(0);
	m_FoilOffset.setY(400);
}


/**
 * The public destructor.
 *
 * Not really necessary. Qt does the clean-up pretty well by itself
 * Old C++ practice
 */
 QXDirect::~QXDirect()
{
	delete m_pCpGraph;

	delete m_pPolarFilterDlg;
	delete m_pObjectPropsDlg;
	delete m_pGraphDlg;
	delete m_pRenameDlg;
	delete m_pEditPlrDlg;
	delete m_pXFoilAnalysisDlg;
	delete m_pFPDlg;
	delete m_pBDlg;
//	delete m_pBatchThreadDlg;
	delete m_pXFoilAdvancedDlg;
	delete m_pTwoDPanelDlg;
	delete m_pIFDlg;
	delete m_pManageFoilsDlg;
	delete m_pNacaFoilDlg;
	delete m_pFoilCoordDlg;
	delete m_pFoilGeomDlg;
	delete m_pTEGapDlg;
	delete m_pLEDlg;
	delete m_pFlapDlg;
	delete m_pCAddDlg;
	delete m_pXDirectStyleDlg;
}

/**
* Adds the results of the XFoil Calculation to the OpPoint object
* @param pOpPoint a pointer to the instance of the OpPoint to be filled with the data from the XFoil object.
*/
void QXDirect::AddOpData(OpPoint *pOpPoint)
{
	int i, j, ibl, is, k;

	pOpPoint->n        = m_pXFoil->n;
	pOpPoint->Cd       = m_pXFoil->cd;
	pOpPoint->Cdp      = m_pXFoil->cdp;
	pOpPoint->Cl       = m_pXFoil->cl;
	pOpPoint->m_XCP    = m_pXFoil->xcp;
	pOpPoint->Cm       = m_pXFoil->cm;
	pOpPoint->Reynolds = m_pXFoil->reinf;
	pOpPoint->Mach     = m_pXFoil->minf;
	pOpPoint->ACrit    = m_pXFoil->acrit;
	pOpPoint->m_bTEFlap    = MainFrame::s_pCurFoil->m_bTEFlap;
	pOpPoint->m_bLEFlap    = MainFrame::s_pCurFoil->m_bLEFlap;

	pOpPoint->Cpmn   = m_pXFoil->cpmn;

	for (k=0; k<m_pXFoil->n; k++)
	{
//		pOpPoint->x[k]   = m_pXFoil->x[k+1];
//		pOpPoint->y[k]   = m_pXFoil->y[k+1];
//		pOpPoint->s[k]   = m_pXFoil->s[k+1];
		pOpPoint->Cpi[k] = m_pXFoil->cpi[k+1];
		pOpPoint->Qi[k]  = m_pXFoil->qgamm[k+1];
	}

	if(m_pXFoil->lvisc && m_pXFoil->lvconv)
	{
		pOpPoint->Xtr1 =m_pXFoil->xoctr[1];
		pOpPoint->Xtr2 =m_pXFoil->xoctr[2];
		pOpPoint->m_bViscResults = true;
		pOpPoint->m_bBL = true;
		for (k=0; k<m_pXFoil->n; k++) 
		{
			pOpPoint->Cpv[k] = m_pXFoil->cpv[k+1];
			pOpPoint->Qv[k] = m_pXFoil->qvis[k+1];
		}
	}
	else
	{
		pOpPoint->m_bViscResults = false;
	}

	if(pOpPoint->m_bTEFlap || pOpPoint->m_bLEFlap)
	{
		SetHingeMoments(pOpPoint);
/*		pOpPoint->m_TEHMom = m_pXFoil->hmom;
		pOpPoint->XForce   = m_pXFoil->hfx;
		pOpPoint->YForce   = m_pXFoil->hfy;*/
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


/**
* Creates a new instance of an OpPoint.
* Loads the data from the XFoil object in that OpPoint
* Adds the data to the active Polar object
* @param pPolar a point to the parent Polar object to which the OpPoint should be attached.
* @param bStoreOpp true if the OpPoint should be stored
* @return a pointer to the OpPoint which has been created, or NULL if it wasn't stored.
*/
OpPoint* QXDirect::AddOpPoint(Polar *pPolar, bool bStoreOpp)
{
	MainFrame*pMainFrame = (MainFrame*)s_pMainFrame;

	if(!pPolar) pPolar = m_pCurPolar;

	OpPoint *pNewPoint = new OpPoint();
	if(pNewPoint ==NULL)
	{
		return NULL;
	}
	else
	{
		if(!m_pXFoil->lvconv)
		{
			delete pNewPoint;
			return NULL;
		}
		pNewPoint->Alpha = m_pXFoil->alfa * 180/PI;
		pNewPoint->m_Color = pMainFrame->GetColor(2);
		pNewPoint->m_strFoilName = MainFrame::s_pCurFoil->m_FoilName;
		pNewPoint->m_strPlrName  = pPolar->m_PlrName;

		AddOpData(pNewPoint);
		pMainFrame->SetSaveState(false);
	}

	if(bStoreOpp)
	{
		//insert the OpPoint in the Operating points array
		InsertOpPoint(pNewPoint);
	}

	// Now insert OpPoint in the current CPolar object

	if(m_pXFoil->lvconv && pPolar)
	{
		if(pPolar->m_PolarType==FIXEDLIFTPOLAR || pPolar->m_PolarType==RUBBERCHORDPOLAR)
		{
			if(pNewPoint && pNewPoint->Reynolds<1.00e8)
			{
				pPolar->AddData(pNewPoint);
			}
		}
		else
		{
			pPolar->AddData(pNewPoint);
		}
	}

	if(!m_bStoreOpp)
	{
		delete pNewPoint;
		pNewPoint = NULL;
	}
	m_pCurOpp = pNewPoint;

	if(m_bPolarView)
	{
		CreatePolarCurves();
	}
	else
	{
		SetOpp(-1.e10);
	}

	pMainFrame->SetSaveState(false);
	return pNewPoint;
}

/** Sets the state of the window's widgets i.a.w. the state of the active ojbects and views. */
void QXDirect::SetControls()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	if(m_bPolarView) m_pctrlMiddleControls->setCurrentIndex(1);
	else         m_pctrlMiddleControls->setCurrentIndex(0);

	if(m_pCurPolar)
	{
		QString PolarProps;
		m_pCurPolar->GetPolarProperties(PolarProps);
		m_pctrlPolarProps->setText(PolarProps);
	}
	else m_pctrlPolarProps->clear();

	pMainFrame->m_pctrlOppView->setChecked(!m_bPolarView);
	pMainFrame->m_pctrlPolarView->setChecked(m_bPolarView);

	pMainFrame->OpPointsAct->setChecked(!m_bPolarView);
	pMainFrame->PolarsAct->setChecked(m_bPolarView);

	pMainFrame->showPanels->setChecked(m_bShowPanels);
	pMainFrame->showNeutralLine->setChecked(m_bNeutralLine);
	pMainFrame->showInviscidCurve->setChecked(m_bShowInviscid);
	pMainFrame->showCurOppOnly->setChecked(m_bCurOppOnly);

	pMainFrame->setCpVarGraph->setChecked(m_pCpGraph->GetYVariable()==0);
	pMainFrame->setQVarGraph->setChecked(m_pCpGraph->GetYVariable()==1);

	for(int ig=0; ig<MAXPOLARGRAPHS; ig++)
		pMainFrame->PolarGraphAct[ig]->setChecked(m_iPlrView==ONEPOLARGRAPH && m_iPlrGraph== ig);

	pMainFrame->TwoPolarGraphsAct->setChecked(m_iPlrView==TWOPOLARGRAPHS);
	pMainFrame->AllPolarGraphsAct->setChecked(m_iPlrView==ALLPOLARGRAPHS);

	int OppVar = m_pCpGraph->GetYVariable();
	pMainFrame->CurXFoilCtPlot->setChecked(!m_bPolarView  && OppVar==2 && m_XFoilVar ==1);
	pMainFrame->CurXFoilDbPlot->setChecked(!m_bPolarView  && OppVar==2 && m_XFoilVar ==2);
	pMainFrame->CurXFoilDtPlot->setChecked(!m_bPolarView  && OppVar==2 && m_XFoilVar ==3);
	pMainFrame->CurXFoilRtLPlot->setChecked(!m_bPolarView && OppVar==2 && m_XFoilVar ==4);
	pMainFrame->CurXFoilRtPlot->setChecked(!m_bPolarView  && OppVar==2 && m_XFoilVar ==5);
	pMainFrame->CurXFoilNPlot->setChecked(!m_bPolarView   && OppVar==2 && m_XFoilVar ==6);
	pMainFrame->CurXFoilCdPlot->setChecked(!m_bPolarView  && OppVar==2 && m_XFoilVar ==7);
	pMainFrame->CurXFoilCfPlot->setChecked(!m_bPolarView  && OppVar==2 && m_XFoilVar ==8);
	pMainFrame->CurXFoilUePlot->setChecked(!m_bPolarView  && OppVar==2 && m_XFoilVar ==9);
	pMainFrame->CurXFoilHPlot->setChecked(!m_bPolarView   && OppVar==2 && m_XFoilVar ==10);

	m_pctrlShowPressure->setEnabled(!m_bPolarView && m_pCurOpp);
	m_pctrlShowBL->setEnabled(!m_bPolarView && m_pCurOpp);
	m_pctrlAnimate->setEnabled(!m_bPolarView && m_pCurOpp);
	m_pctrlAnimateSpeed->setEnabled(!m_bPolarView && m_pCurOpp);
//	m_pctrlHighlightOpp->setEnabled(m_bPolar);

	pMainFrame->currentFoilMenu->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->currentPolarMenu->setEnabled(m_pCurPolar);

	pMainFrame->renameCurFoil->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->DuplicateFoil->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->deleteCurFoil->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->exportCurFoil->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->renameCurFoil->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->setCurFoilStyle->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->definePolar->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->defineBatch->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->deleteFoilOpps->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->deleteFoilPolars->setEnabled(MainFrame::s_pCurFoil);

	pMainFrame->editCurPolar->setEnabled(m_pCurPolar);
	pMainFrame->deletePolar->setEnabled(m_pCurPolar);
	pMainFrame->exportCurPolar->setEnabled(m_pCurPolar);
	pMainFrame->hidePolarOpps->setEnabled(m_pCurPolar);
	pMainFrame->showPolarOpps->setEnabled(m_pCurPolar);
	pMainFrame->deletePolarOpps->setEnabled(m_pCurPolar);

	pMainFrame->DerotateFoil->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->NormalizeFoil->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->RefineLocalFoil->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->RefineGlobalFoil->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->EditCoordsFoil->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->ScaleFoil->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->SetLERadius->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->SetTEGap->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->SetFlap->setEnabled(MainFrame::s_pCurFoil);
	pMainFrame->InterpolateFoils->setEnabled(MainFrame::s_pCurFoil);

	pMainFrame->currentOppMenu->setEnabled(m_pCurOpp);
	pMainFrame->deleteCurOpp->setEnabled(m_pCurOpp);
	pMainFrame->exportCurOpp->setEnabled(m_pCurOpp);
}

/**
* Connects signals and slots
*/
void QXDirect::Connect()
{
	connect(m_pctrlSpec1, SIGNAL(clicked()), this, SLOT(OnSpec()));
	connect(m_pctrlSpec2, SIGNAL(clicked()), this, SLOT(OnSpec()));
	connect(m_pctrlSpec3, SIGNAL(clicked()), this, SLOT(OnSpec()));
	connect(m_pctrlAnalyze, SIGNAL(clicked()), this, SLOT(OnAnalyze()));
	connect(m_pctrlAlphaMin, SIGNAL(editingFinished()), this, SLOT(OnInputChanged()));
	connect(m_pctrlAlphaMax, SIGNAL(editingFinished()), this, SLOT(OnInputChanged()));
	connect(m_pctrlAlphaDelta, SIGNAL(editingFinished()), this, SLOT(OnInputChanged()));
	connect(m_pctrlCurveStyle, SIGNAL(activated(int)), this, SLOT(OnCurveStyle(int)));
	connect(m_pctrlCurveWidth, SIGNAL(activated(int)), this, SLOT(OnCurveWidth(int)));
	connect(m_pctrlCurveColor, SIGNAL(clicked()), this, SLOT(OnCurveColor()));
	connect(m_pctrlSequence, SIGNAL(clicked()), this, SLOT(OnSequence()));
	connect(m_pctrlViscous, SIGNAL(clicked()), this, SLOT(OnViscous()));
	connect(m_pctrlInitBL, SIGNAL(clicked()), this, SLOT(OnInitBL()));
	connect(m_pctrlShowBL, SIGNAL(clicked()), this, SLOT(OnShowBL()));
	connect(m_pctrlShowPressure, SIGNAL(clicked()), this, SLOT(OnShowPressure()));
	connect(m_pctrlStoreOpp, SIGNAL(clicked()), this, SLOT(OnStoreOpp()));
	connect(m_pctrlShowPoints, SIGNAL(clicked()), this, SLOT(OnShowPoints()));
	connect(m_pctrlShowCurve, SIGNAL(clicked()), this, SLOT(OnShowCurve()));
//	connect(m_pctrlHighlightOpp, SIGNAL(clicked()), this, SLOT(OnHighlightOpp()));

	connect(m_pctrlAnimate, SIGNAL(clicked(bool)), this, SLOT(OnAnimate(bool)));
	connect(m_pctrlAnimateSpeed, SIGNAL(sliderMoved(int)), this, SLOT(OnAnimateSpeed(int)));
	connect(m_pAnimateTimer, SIGNAL(timeout()), this, SLOT(OnAnimateSingle()));
}


/**
* Creates a curve of the Cp graph for a specified OpPoint instance, or for all the instances of OpPoint.
* @param pOpp a pointer to the instance of the operating point, the data of which is used to build the CCurve objects
*/
void QXDirect::CreateOppCurves(OpPoint *pOpp)
{
	OpPoint *pOpPoint = NULL;
	if(pOpp) pOpPoint = pOpp; else pOpPoint = m_pCurOpp;

	Curve *pCurve1;
	QString str;
	int k;

	m_pCpGraph->DeleteCurves();

	if(m_bCurOppOnly && pOpPoint)
	{
		if(!pOpPoint || !pOpPoint->m_bIsVisible) return;
		pCurve1    = m_pCpGraph->AddCurve();
		if(pOpPoint->m_bShowPoints) pCurve1->ShowPoints(true);
		
		pCurve1->SetStyle(pOpPoint->m_Style);
		pCurve1->SetColor(pOpPoint->m_Color);
		pCurve1->SetWidth(pOpPoint->m_Width);

		str = QString("-Re=%1-Alpha=%2").arg(pOpPoint->Reynolds,8,'f',0).arg(pOpPoint->Alpha,5,'f',2);
		str = pOpPoint->m_strFoilName+str;
		pCurve1->SetTitle(str);

		FillOppCurve(pOpPoint, m_pCpGraph, pCurve1);

		if(m_bShowInviscid && pOpPoint)
		{
			Curve *pCpi = m_pCpGraph->AddCurve();
			if(pOpPoint->m_bShowPoints) pCpi->ShowPoints(true);
			pCpi->SetStyle(1);
			pCpi->SetColor(pOpPoint->m_Color.darker(150));
			pCpi->SetWidth(pOpPoint->m_Width);
			str= QString("-Re=%1-Alpha=%2_Inviscid").arg(pOpPoint->Reynolds,8,'f',0).arg(pOpPoint->Alpha,5,'f',2);
			str = pOpPoint->m_strFoilName+str;
			pCpi->SetTitle(str);
			FillOppCurve(pOpPoint, m_pCpGraph, pCpi, true);
		}
	}
	else if(!m_bCurOppOnly)
	{
		for (k=0; k<m_poaOpp->size(); k++)
		{
			pOpp = (OpPoint*)m_poaOpp->at(k);
			if (pOpp && pOpp->m_bIsVisible)
			{
				pCurve1    = m_pCpGraph->AddCurve();

				if(pOpp->m_bShowPoints) pCurve1->ShowPoints(true);

				pCurve1->SetStyle(pOpp->m_Style);
				pCurve1->SetColor(pOpp->m_Color);
				pCurve1->SetWidth(pOpp->m_Width);

				str= QString("-Re=%1-Alpha=%2").arg(pOpp->Reynolds,8,'f',0).arg(pOpp->Alpha,5,'f',2);
				str = pOpp->m_strFoilName+str;
				pCurve1->SetTitle(str);

				FillOppCurve(pOpp, m_pCpGraph, pCurve1);
			}
		}
	}
}


/**
*Creates the curves of the graphs for all the visible polars.
*/
void QXDirect::CreatePolarCurves()
{
	// curves must be entirely reconstructed each time from the
	// operating points database, since user may have added
	// or deleted points & polars
	int k;
	Polar *pPolar;
	QString str;

	m_PlrGraph[0].DeleteCurves();
	m_PlrGraph[2].DeleteCurves();
	m_PlrGraph[1].DeleteCurves();
	m_PlrGraph[3].DeleteCurves();
	m_PlrGraph[4].DeleteCurves();

	for (k=0; k<m_poaPolar->size(); k++)
	{
		pPolar = (Polar*)m_poaPolar->at(k);

		if (pPolar->m_bIsVisible && pPolar->m_Alpha.size()>0)
		{
						if (	(pPolar->m_PolarType==FIXEDSPEEDPOLAR  && m_bType1) ||
								(pPolar->m_PolarType==FIXEDLIFTPOLAR   && m_bType2) ||
								(pPolar->m_PolarType==RUBBERCHORDPOLAR && m_bType3) ||
								(pPolar->m_PolarType==FIXEDAOAPOLAR    && m_bType4))
			{
				Curve* pPolarCurve = m_PlrGraph[0].AddCurve();
				Curve* pCmCurve    = m_PlrGraph[2].AddCurve();
				Curve* pCzCurve    = m_PlrGraph[1].AddCurve();
				Curve* pTr1Curve   = m_PlrGraph[3].AddCurve();
				Curve* pTr2Curve = NULL;
				if(m_PlrGraph[3].GetYVariable() == 6)	pTr2Curve   = m_PlrGraph[3].AddCurve();

				Curve* pUserCurve  = m_PlrGraph[4].AddCurve();

				if(pPolar->m_bShowPoints)
				{
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

				FillPolarCurve(pPolarCurve, pPolar, m_PlrGraph[0].GetXVariable(), m_PlrGraph[0].GetYVariable());
				FillPolarCurve(pCmCurve, pPolar, m_PlrGraph[2].GetXVariable(), m_PlrGraph[2].GetYVariable());
				FillPolarCurve(pCzCurve, pPolar, m_PlrGraph[1].GetXVariable(), m_PlrGraph[1].GetYVariable());
				FillPolarCurve(pTr1Curve, pPolar, m_PlrGraph[3].GetXVariable(), m_PlrGraph[3].GetYVariable());
				if(pTr2Curve) FillPolarCurve(pTr2Curve, pPolar, 7, 1);
				FillPolarCurve(pUserCurve, pPolar, m_PlrGraph[4].GetXVariable(), m_PlrGraph[4].GetYVariable());

				pPolarCurve->SetTitle(pPolar->m_PlrName);
				pCmCurve->SetTitle(pPolar->m_PlrName);
				pUserCurve->SetTitle(pPolar->m_PlrName);

				if(pTr2Curve)
				{
					str = pPolar->m_PlrName + " / Xtr1";
					pTr1Curve->SetTitle(str);
					str = pPolar->m_PlrName + " / Xtr2";
					pTr2Curve->SetTitle(str);
				}
				else
				{
					pTr1Curve->SetTitle(pPolar->m_PlrName);
				}
			}
		}
	}
}


/**
* Deletes instance of the active Foil object, and the associated results. 
* @param bAsk true if confirmation should be requested from the user priori to deletion
*/
void QXDirect::DeleteFoil(bool bAsk)
{
	if(!MainFrame::s_pCurFoil || !MainFrame::s_pCurFoil->m_FoilName.length()) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	if(pMainFrame->DeleteFoil(MainFrame::s_pCurFoil, bAsk))
	{
		m_pCurOpp = NULL;
		m_pCurPolar = NULL;
		MainFrame::s_pCurFoil = NULL;
	}
}


/** Deletes the current OpPoint, or all of them, depending on the input flag 
*
* @param bCurrent true if only the current OpPoint should be deleted, false if all should be deleted.
*/
void QXDirect::DeleteOpPoint(bool bCurrent)
{
	int i;
	if(bCurrent)
	{
		// we remove only the current OpPoint
		
		OpPoint* opt;
		for (i=0; i<m_poaOpp->size(); i++)
		{
			opt =(OpPoint*)m_poaOpp->at(i);
			if (opt == m_pCurOpp)
			{
				m_poaOpp->removeAt(i);
				delete m_pCurOpp;
				m_pCurOpp = NULL;
				break;
			}
		}
	}
	else
	{
		// kill'em all
		OpPoint* pOpPoint;
		for (i=m_poaOpp->size()-1; i>=0;i--)
		{
			pOpPoint =(OpPoint*)m_poaOpp->at(i);
			if (pOpPoint->m_strFoilName == MainFrame::s_pCurFoil->m_FoilName &&
				pOpPoint->m_strPlrName == m_pCurPolar->m_PlrName)
			{
				m_poaOpp->removeAt(i);
				delete pOpPoint;
			}
		}
	}
	m_pCurOpp = NULL;
}


/**
* Initializes the comboboxes with the active OpPoint or Polar line style
* @param bEnable true if the comboboxes should be enable as a result
*/
void QXDirect::FillComboBoxes(bool bEnable)
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
	m_pctrlCurveWidth->setCurrentIndex(m_CurveWidth-1);
}


/**
* Fills the Cp graph curve with the data from the OpPoint.
*@param pOpp a pointer to the OpPoint for which the curve is drawn
*@param pGraph a pointer to the Graph to which the curve belongs
*@param pCurve a pointer to the CCurve which will be filled with the data from the OpPoint
*@param bInviscid true if the inviscid resutls should be displayed, false if the viscous results should be displayed
*/
void QXDirect::FillOppCurve(OpPoint *pOpp, Graph *pGraph, Curve *pCurve, bool bInviscid)
{
	int j;

	Foil *pOpFoil = ((MainFrame*)s_pMainFrame)->GetFoil(pOpp->m_strFoilName);

	switch(m_pCpGraph->GetYVariable())
	{
		case 0:
		{
			for (j=0; j<pOpp->n; j++)
			{
				if(!bInviscid)
				{
					if(pOpp->m_bViscResults) pCurve->AppendPoint(pOpFoil->x[j], pOpp->Cpv[j]);
				}
				else
				{
					pCurve->AppendPoint(pOpFoil->x[j], pOpp->Cpi[j]);
				}
			}
			pGraph->SetYTitle(tr("Cp"));
			break;
		}
		case 1:
		{
			for (j=0; j<pOpp->n; j++)
			{
				if(!bInviscid)
				{
					if(pOpp->m_bViscResults) pCurve->AppendPoint(pOpFoil->x[j], pOpp->Qv[j]);
				}
				else
				{
					pCurve->AppendPoint(pOpFoil->x[j], pOpp->Qi[j]);
				}
			}
			pGraph->SetYTitle(tr("Q"));
			break;
		}
		default:
		{
			for (j=0; j<pOpp->n; j++)
			{
				if(!bInviscid)
				{
					if(pOpp->m_bViscResults) pCurve->AppendPoint(pOpFoil->x[j], pOpp->Cpv[j]);
				}
				else{
					pCurve->AppendPoint(pOpFoil->x[j], pOpp->Cpi[j]);
				}
			}
			pGraph->SetYTitle(tr("Cp"));
			break;
		}
	}
}


/**
*Fills a CCurve object with data from a Polar object
* @param pCurve a pointer to the CCurve object to be filled with the data from the Polar object
* @param pPolar a pointer to the Polar object from which the data will extracted
* @param XVar the index of the variable for the curve's x-axis
* @param YVar the index of the variable for the curve's y-axis
*/
void QXDirect::FillPolarCurve(Curve *pCurve, Polar *pPolar, int XVar, int YVar)
{
	int i;
	QList <double> *pX;
	QList <double> *pY;

	pX = (QList <double> *) GetVariable(pPolar, XVar);
	pY = (QList <double> *) GetVariable(pPolar, YVar);
	double fx = 1.0;
	double fy = 1.0;

	pCurve->SetSelected(-1);

	if(XVar == 3) fx = 10000.0;
	if(YVar == 3) fy = 10000.0;

	for (i=0; i<pPolar->m_Alpha.size(); i++)
	{
		if (XVar==12)
		{
			if((*pX)[i]>0.0)
			{
				if (YVar==12)
				{
					if((*pY)[i]>0.0)
					{
						pCurve->AppendPoint(1.0/sqrt((*pX)[i]), 1.0/sqrt((*pY)[i]));
					}
				}
				else
				{
					pCurve->AppendPoint(1.0/sqrt((*pX)[i]), (*pY)[i]*fy);
				}
			}
		}
		else{
			if (YVar==12)
			{
				if((*pY)[i]>0.0)
				{
					pCurve->AppendPoint((*pX)[i]*fx, 1.0/sqrt((*pY)[i]));
				}
			}
			else
			{
				pCurve->AppendPoint((*pX)[i]*fx, (*pY)[i]*fy);
			}
		}
		if(m_pCurOpp && m_bHighlightOpp)
		{
			if(fabs(pPolar->m_Alpha[i]-m_pCurOpp->Alpha)<0.0001)
			{
				if(pPolar->m_PlrName==m_pCurOpp->m_strPlrName  && MainFrame::s_pCurFoil->m_FoilName==pPolar->m_FoilName)
				{

					pCurve->SetSelected(i);
				}
			}
		}
	}
}


/**
*Returns a pointer to the OpPoint with the specified aoa and which is attached to the active Foil and Polar.
*@param Alpha the input aoa 
*@return a pointer to the OpPoint, or NULL if none has been found for this set of (Foil, Polar, aoa);
*/
OpPoint* QXDirect::GetOpPoint(double Alpha)
{
	OpPoint* pOpPoint;
	for (int i=0; i<m_poaOpp->size(); i++)
	{
		if(!m_pCurPolar) return NULL;
		pOpPoint = (OpPoint*)m_poaOpp->at(i);

		//since alphas are calculated at 1/100th
		if (pOpPoint->m_strFoilName == MainFrame::s_pCurFoil->m_FoilName)
		{
			if (pOpPoint->m_strPlrName == m_pCurPolar->m_PlrName)
			{
				if(m_pCurPolar->m_PolarType!=FIXEDAOAPOLAR)
				{
					if(fabs(pOpPoint->Alpha - Alpha) <0.01)
					{
						return pOpPoint;
					}
				}
				else
				{
					if(fabs(pOpPoint->Reynolds - Alpha) <0.1)
					{
						return pOpPoint;
					}
				}
			}
		}
	}
	return NULL;// shouldn't ever get here, fortunately
}


/**
* Returns a void pointer to the array of the specified variable of the input Polar
* @param pPolar a pointer to the Polar object
* @param iVar the index of the variable for which a pointer is requested
* @return a pointer to the array of the requested variable
*/
void * QXDirect::GetVariable(Polar *pPolar, int iVar)
{
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


/**
 * Returns a pointer to the graph to which the input client point belongs
 * @param pt the client point, in this case the position of the mouse
 * @return  a pointer to the graph
 */
QGraph* QXDirect::GetGraph(QPoint &pt)
{
	//pt is in client coordinates
	if (m_bPolarView)
	{
		if(m_iPlrView == ONEPOLARGRAPH)
		{
			if(m_iPlrGraph>=0 && m_iPlrGraph<MAXPOLARGRAPHS) m_pCurGraph = m_PlrGraph + m_iPlrGraph;
			else                                             m_pCurGraph = NULL;

			if(m_pCurGraph->IsInDrawRect(pt)) return m_pCurGraph;
			else return NULL;
		}
		if(m_iPlrView == TWOPOLARGRAPHS)
		{
			if(m_PlrGraph[0].IsInDrawRect(pt)){return m_PlrGraph;}
			if(m_PlrGraph[4].IsInDrawRect(pt)){return m_PlrGraph+4;}
			return NULL;
		}
		else
		{
			QRect r;
			m_PlrGraph[2].GetClientRect(r);
			if(m_PlrGraph[0].IsInDrawRect(pt)){return m_PlrGraph;}
			if(m_PlrGraph[1].IsInDrawRect(pt)){return m_PlrGraph+1;}
			if(m_PlrGraph[2].IsInDrawRect(pt)){return m_PlrGraph+2;}
			if(m_PlrGraph[3].IsInDrawRect(pt)){return m_PlrGraph+3;}
			if(m_PlrGraph[4].IsInDrawRect(pt)){return m_PlrGraph+4;}
		}
	}
	else
	{
		if(m_pCpGraph->IsInDrawRect(pt))
		{
			if(m_bCpGraph) return m_pCpGraph;
			else           return NULL;
		}
		else return NULL;
	}
	return NULL;
}


/**
 * Inserts a new OpPoint in the array. The OpPoints are sorted by FoilName first, then by Re number, then by aoa.
 * If an OpPoint already exists with the same combination of (FoilName, Re, aoa), it is overwritten.
 * @param pNewPoint
 */
void QXDirect::InsertOpPoint(OpPoint *pNewPoint)
{
	if(!pNewPoint) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	bool bIsInserted = false;
	OpPoint* pOpPoint;
	Polar *pPolar = pMainFrame->GetPolar(pNewPoint->m_strFoilName, pNewPoint->m_strPlrName);

	if(!pPolar)
	{
		delete pNewPoint;
		return ;
	}

	// first add the OpPoint to the OpPoint Array for the current FoilName
	for (int i=0; i<m_poaOpp->size(); i++)
	{
		pOpPoint = (OpPoint*)m_poaOpp->at(i);
		if (pNewPoint->m_strFoilName.compare(pOpPoint->m_strFoilName)<0)
		{
			//insert point
			m_poaOpp->insert(i, pNewPoint);
			bIsInserted = true;
			i = m_poaOpp->size();// to break
		}
		else if (pNewPoint->m_strFoilName == pOpPoint->m_strFoilName)
		{
			if (pNewPoint->Reynolds < pOpPoint->Reynolds)
			{
				//insert point
				m_poaOpp->insert(i, pNewPoint);
				bIsInserted = true;
				i = m_poaOpp->size();// to break
			}
			else if (fabs(pNewPoint->Reynolds-pOpPoint->Reynolds)<1.0)
			{
				if(fabs(pNewPoint->Alpha - pOpPoint->Alpha)<0.005)
				{
					//replace existing point
					m_poaOpp->removeAt(i);
					delete pOpPoint;
					m_poaOpp->insert(i, pNewPoint);
					bIsInserted = true;
					i = m_poaOpp->size();// to break
				}
				else if (pNewPoint->Alpha > pOpPoint->Alpha)
				{
					//insert point
					m_poaOpp->insert(i, pNewPoint);
					bIsInserted = true;
					i = m_poaOpp->size();// to break
				}
			}
		}
	}
	if (!bIsInserted) 	m_poaOpp->append(pNewPoint);
}


/**
 * Overrides the QWidget's keyPressEvent method.
 * Dispatches the key press event
 * @param event the QKeyEvent
 */
void QXDirect::keyPressEvent(QKeyEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	bool bShift = false;
	bool bCtrl  = false;
	if(event->modifiers() & Qt::ShiftModifier)   bShift =true;
	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;

	switch (event->key())
	{
		case Qt::Key_Return:
			if (event->modifiers().testFlag(Qt::AltModifier))
			{
				OnPolarProps();
				break;
			}

			ReadParams();
			if(m_pctrlAnalyze->hasFocus())  OnAnalyze();
			else
			{
				activateWindow();
				m_pctrlAnalyze->setFocus();
			}
			break;
		case Qt::Key_Tab:
			ReadParams();
			break;
		case Qt::Key_Escape:
			StopAnimate();
			UpdateView();
			break;
		case Qt::Key_H:
		{
			if(m_bPolarView && event->modifiers().testFlag(Qt::ControlModifier))
			{
				OnHighlightOpp();
			}
			break;
		}
		case Qt::Key_L:
			pMainFrame->OnLogFile();
			break;
		case Qt::Key_X:
			m_bXPressed = true;
			break;
		case Qt::Key_Y:
			m_bYPressed = true;
			break;
		case Qt::Key_1:
			m_iPlrView  = ONEPOLARGRAPH;
			m_iPlrGraph = 0;
			if(m_bPolarView) SetPolarLegendPos();
			SetControls();
			UpdateView();
			break;
		case Qt::Key_2:
			m_iPlrView  = ONEPOLARGRAPH;
			m_iPlrGraph = 1;
			if(m_bPolarView) SetPolarLegendPos();
			SetControls();
			UpdateView();
			break;
		case Qt::Key_3:
			m_iPlrView  = ONEPOLARGRAPH;
			m_iPlrGraph = 2;
			if(m_bPolarView) SetPolarLegendPos();
			SetControls();
			UpdateView();
			break;
		case Qt::Key_4:
			m_iPlrView  = ONEPOLARGRAPH;
			m_iPlrGraph = 3;
			if(m_bPolarView) SetPolarLegendPos();
			SetControls();
			UpdateView();
			break;
		case Qt::Key_5:
			m_iPlrView  = ONEPOLARGRAPH;
			m_iPlrGraph = 4;
			if(m_bPolarView) SetPolarLegendPos();
			SetControls();
			UpdateView();
			break;
		case Qt::Key_A:
			m_iPlrView  = ALLPOLARGRAPHS;
			if(m_bPolarView) SetPolarLegendPos();
			SetControls();
			UpdateView();
			break;
		case Qt::Key_T:
			m_iPlrView  = TWOPOLARGRAPHS;
			if(m_bPolarView) SetPolarLegendPos();
			SetControls();
			UpdateView();
			break;
		case Qt::Key_G:
			if(m_pCurGraph) OnGraphSettings();
			break;
		case Qt::Key_R:
			if(m_pCurGraph)
			{
				m_pCurGraph->SetAuto(true);
				UpdateView();
			}
			else if(!m_bPolarView)
			{
				OnResetFoilScale();
			}
			else
			{
				SetPolarLegendPos();
				UpdateView();
			}
			break;
		case Qt::Key_V:
			if(m_bPolarView && m_pCurGraph)
			{
				GraphDlg::s_ActivePage=0;
				OnGraphSettings();
			}
			break;
		case Qt::Key_F2:
		{
			OnRenameFoil();
			break;
		}
		case Qt::Key_F3:
		{
			if(bShift) OnCadd();
			else       OnPanels();
			break;
		}
		case Qt::Key_F5:
		{
			if(!m_bPolarView) return;
			OnOpPoints();
			break;
		}
		case Qt::Key_F6:
		{
			if (event->modifiers().testFlag(Qt::ShiftModifier))        OnBatchAnalysis();
			else if (event->modifiers().testFlag(Qt::ControlModifier)) OnMultiThreadedBatchAnalysis();
			else                                                       OnDefinePolar();
			break;
		}
		case Qt::Key_F8:
		{
			if(m_bPolarView) return;
			OnPolars();
			break;
		}
		case Qt::Key_F9:
		{
			OnFoilGeom();
			break;
		}
		case Qt::Key_F10:
		{
			OnSetFlap();
			break;
		}
		case Qt::Key_F11:
		{
			OnInterpolateFoils();
			break;
		}
		default:
			QWidget::keyPressEvent(event);
	}
}



/**
 * Overrides the QWidget's keyReleaseEvent method.
 * Dispatches the key release event
 * @param event the QKeyEvent
 */
void QXDirect::keyReleaseEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_X:
			if(!event->isAutoRepeat()) m_bXPressed = false;
			break;
		case Qt::Key_Y:
			if(!event->isAutoRepeat()) m_bYPressed = false;
			break;
		default:
			QWidget::keyReleaseEvent(event);
	}
}

/**
 * Loads the user's default settings from the application QSettings object
 * @param pSettings a pointer to the QSettings object
 */
void QXDirect::LoadSettings(QSettings *pSettings)
{
	QString str1, str2, str3;
	int r,g,b;

	pSettings->beginGroup("XDirect");
	{
		m_bAlpha          = pSettings->value("AlphaSpec").toBool();
		m_bStoreOpp       = pSettings->value("StoreOpp").toBool();
		m_bViscous        = pSettings->value("ViscousAnalysis").toBool();
		m_bInitBL         = pSettings->value("InitBL").toBool();
		m_bBL             = pSettings->value("BoundaryLayer").toBool();
		m_bPressure       = pSettings->value("Pressure").toBool();
		m_bPolarView          = pSettings->value("PolarView").toBool();
		m_bShowUserGraph  = pSettings->value("UserGraph").toBool();
		m_bShowPanels     = pSettings->value("ShowPanels").toBool();
		m_bType1          = pSettings->value("Type1").toBool();
		m_bType2          = pSettings->value("Type2").toBool();
		m_bType3          = pSettings->value("Type3").toBool();
		m_bType4          = pSettings->value("Type4").toBool();
		m_bAutoInitBL     = pSettings->value("AutoInitBL").toBool();
		m_bFromList       = pSettings->value("FromList").toBool();
		m_bFromZero       = pSettings->value("FromZero").toBool();
		m_bShowTextOutput = pSettings->value("TextOutput").toBool();
		m_bNeutralLine    = pSettings->value("NeutralLine").toBool();
		m_bCurOppOnly     = pSettings->value("CurOppOnly").toBool();
		m_bShowInviscid   = pSettings->value("ShowInviscid").toBool();
		m_bCpGraph        = pSettings->value("ShowCpGraph").toBool();
		m_bSequence       = pSettings->value("Sequence").toBool();
		m_bHighlightOpp   = pSettings->value("HighlightOpp").toBool();
		m_bHighlightOpp = false;

		r = pSettings->value("BLColorRed").toInt();
		g = pSettings->value("BLColorGreen").toInt();
		b = pSettings->value("BLColorBlue").toInt();
		m_crBLColor = QColor(r,g,b);
		m_iBLStyle = pSettings->value("BLStyle").toInt();
		m_iBLWidth = pSettings->value("BLWidth").toInt();
		r = pSettings->value("PressureColorRed").toInt();
		g = pSettings->value("PressureColorGreen").toInt();
		b = pSettings->value("PressureColorBlue").toInt();
		m_crPressureColor = QColor(r,g,b);
		m_iPressureStyle = pSettings->value("PressureStyle").toInt();
		m_iPressureWidth = pSettings->value("PressureWidth").toInt();
		r = pSettings->value("NeutralColorRed").toInt();
		g = pSettings->value("NeutralColorGreen").toInt();
		b = pSettings->value("NeutralColorBlue").toInt();
		m_crNeutralColor = QColor(r,g,b);
		m_iNeutralStyle = pSettings->value("NeutralStyle").toInt();
		m_iNeutralWidth = pSettings->value("NeutralWidth").toInt();

		m_XFoilVar       = pSettings->value("XFoilVar").toInt();
		m_IterLim        = pSettings->value("IterLim").toInt();
		m_iPlrGraph      = pSettings->value("PlrGraph").toInt();

		switch(pSettings->value("PlrView").toInt())
		{
			case 1:
				m_iPlrView = ONEPOLARGRAPH;
				break;
			case 2:
				m_iPlrView = TWOPOLARGRAPHS;
				break;
			default:
				m_iPlrView = ALLPOLARGRAPHS;
				break;
		}

		m_Alpha          = pSettings->value("AlphaMin").toDouble();
		m_AlphaMax       = pSettings->value("AlphaMax").toDouble();
		m_AlphaDelta     = pSettings->value("AlphaDelta").toDouble();
		m_Cl             = pSettings->value("ClMin").toDouble();
		m_ClMax          = pSettings->value("ClMax").toDouble();
		m_ClDelta        = pSettings->value("ClDelta").toDouble();
		m_Reynolds       = pSettings->value("ReynoldsMin").toDouble();
		m_ReynoldsMax    = pSettings->value("ReynoldsMax").toDouble();
		m_ReynoldsDelta  = pSettings->value("ReynolsDelta").toDouble();
		m_NCrit          = pSettings->value("NCrit").toDouble();
		m_XTopTr         = pSettings->value("XTopTr").toDouble();
		m_XBotTr         = pSettings->value("XBotTr").toDouble();
		m_Mach           = pSettings->value("Mach").toDouble();
		m_ASpec          = pSettings->value("ASpec").toDouble();
		m_pXFoil->vaccel = pSettings->value("VAccel").toDouble();
		m_bAutoInitBL    = pSettings->value("AutoInitBL").toBool();
		m_NRe            = pSettings->value("NReynolds").toInt();
		m_pXFoil->m_bFullReport = pSettings->value("FullReport").toBool();

		b = pSettings->value("Type").toInt();
		if(b==1) m_PolarType = FIXEDSPEEDPOLAR;
		else if(b==2) m_PolarType = FIXEDLIFTPOLAR;
		else if(b==3) m_PolarType = RUBBERCHORDPOLAR;
		else if(b==4) m_PolarType = FIXEDAOAPOLAR;

		for (int i=0; i<m_NRe; i++)
		{
			str1 = QString("ReList%1").arg(i);
			str2 = QString("MaList%1").arg(i);
			str3 = QString("NcList%1").arg(i);
			m_ReList[i] = pSettings->value(str1).toDouble();
			m_MachList[i] = pSettings->value(str2).toDouble();
			m_NCritList[i] = pSettings->value(str3).toDouble();
		}
	}
	pSettings->endGroup();

	m_PlrGraph[0].LoadSettings(pSettings);
	m_PlrGraph[2].LoadSettings(pSettings);
	m_PlrGraph[1].LoadSettings(pSettings);
	m_PlrGraph[3].LoadSettings(pSettings);
	m_PlrGraph[4].LoadSettings(pSettings);
	m_pCpGraph->LoadSettings(pSettings);

	if(m_pCpGraph->GetYVariable() == 0 || m_pCpGraph->GetYVariable()>=2)
	{
		m_pCpGraph->SetYTitle(tr("Cp"));
		m_pCpGraph->SetInverted(true);
	}
	else
	{
		m_pCpGraph->SetYTitle(tr("Q"));
		m_pCpGraph->SetInverted(false);
	}

	for(int ig=0; ig<MAXPOLARGRAPHS; ig++) SetGraphTitles(m_PlrGraph+ig);
}


/**
 * Overrides the QWidget's mouseDoubleClickEvent method.
 * Dispatches the event
 * @param event the QMouseEvent
 */
void QXDirect::mouseDoubleClickEvent (QMouseEvent * event)
{
	if(!m_bPolarView)
	{
		if (!m_pCpGraph->IsInDrawRect(event->pos())) return;
	}
	else if(!m_pCurGraph) return;

	OnGraphSettings();
}



/**
 * Overrides the QWidget's mouseMoveEvent method.
 * Dispatches the event
 * @param event the QMouseEvent
 */
void QXDirect::mouseMoveEvent(QMouseEvent *event)
{
	static QPoint pt;
	static double xu, yu, x1, y1, xmin, ymin, xmax, ymax, scale;
	static int a;
	static MainFrame* pMainFrame;
	pMainFrame = (MainFrame*)s_pMainFrame;

	pt.setX(event->x());
	pt.setY(event->y()); //client coordinates
	m_pCurGraph = GetGraph(pt);
	if(!hasFocus()) setFocus();//to catch keyboard input;

	if ((event->buttons() & Qt::LeftButton) && m_bTrans)
	{
		if(m_pCurGraph && m_bTransGraph)
		{
			// we translate the curves inside the graph
			m_pCurGraph->SetAuto(false);
			x1 =  m_pCurGraph->ClientTox(m_PointDown.x()) ;
			y1 =  m_pCurGraph->ClientToy(m_PointDown.y()) ;

			xu = m_pCurGraph->ClientTox(pt.x());
			yu = m_pCurGraph->ClientToy(pt.y());

			xmin = m_pCurGraph->GetXMin() - xu+x1;
			xmax = m_pCurGraph->GetXMax() - xu+x1;
			ymin = m_pCurGraph->GetYMin() - yu+y1;
			ymax = m_pCurGraph->GetYMax() - yu+y1;

			m_pCurGraph->SetWindow(xmin, xmax, ymin, ymax);

			m_PointDown.setX(pt.x());
			m_PointDown.setY(pt.y());
			if(!m_bAnimate) UpdateView();
		}
		else if(m_bPolarView)
		{
			m_PolarLegendOffset.rx() += pt.x()-m_PointDown.x();
			m_PolarLegendOffset.ry() += pt.y()-m_PointDown.y();
			UpdateView();
		}
		else if (MainFrame::s_pCurFoil  && !m_bPolarView)
		{
			// we translate the airfoil
			
			m_FoilOffset.rx() += pt.x()-m_PointDown.x();
			m_FoilOffset.ry() += pt.y()-m_PointDown.y();
			if(!m_bAnimate) UpdateView();
		}
		m_PointDown = pt;
	}

	else if (MainFrame::s_pCurFoil && ((event->buttons() & Qt::MidButton) || event->modifiers().testFlag(Qt::AltModifier)))
	{
		// we zoom the graph or the foil		
		if(m_pCurGraph && m_pCurGraph->IsInDrawRect(pt) && m_bCpGraph)
		{ 
			//zoom graph

			m_pCurGraph->SetAuto(false);
			if(pt.y()-m_PointDown.y()<0) m_pCurGraph->Scale(1.02);
			else                         m_pCurGraph->Scale(1.0/1.02);


			if(!m_bAnimate) UpdateView();
		}
		else if(MainFrame::s_pCurFoil && !m_bPolarView)
		{
			//zoom the foil
			scale = m_fFoilScale;

			if(pt.y()-m_PointDown.y()<0) m_fFoilScale *= 1.02;
			else                         m_fFoilScale /= 1.02;

			a = (int)((m_rCltRect.right()+m_rCltRect.left())/2);

			m_FoilOffset.rx() = a + (int)((m_FoilOffset.x()-a)/scale*m_fFoilScale);

			if(!m_bAnimate) UpdateView();

		}
		m_PointDown = pt;
	}
	else if(m_pCurGraph && m_pCurGraph->IsInDrawRect(pt))
	{
		x1 = m_pCurGraph->ClientTox(event->x()) ;
		y1 = m_pCurGraph->ClientToy(event->y()) ;
		pMainFrame->statusBar()->showMessage(QString("X = %1, Y = %2").arg(x1).arg(y1));
	}
	else
	{
		pMainFrame->statusBar()->clearMessage();
	}
}



/**
 * Overrides the QWidget's mousePressEvent method.
 * Dispatches the event
 * @param event the QMouseEvent
 */
void QXDirect::mousePressEvent(QMouseEvent *event)
{
	QPoint pt(event->x(), event->y()); //client coordinates

	m_pCurGraph = GetGraph(pt);

	TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;
	p2DWidget->setFocus();

	if(event->buttons() & Qt::LeftButton)
	{
		if (MainFrame::s_pCurFoil || (m_pCurGraph && m_pCurGraph->IsInDrawRect(pt) && m_bCpGraph))
		{
			m_PointDown.setX(pt.x());
			m_PointDown.setY(pt.y());

			m_bTrans = true;
			p2DWidget->setCursor(Qt::ClosedHandCursor);
			if (m_pCurGraph && m_pCurGraph->IsInDrawRect(pt)) m_bTransGraph = true;
			else                                              m_bTransGraph = false;
			if(!m_bAnimate) UpdateView();
		}
	}
}


/**
 * Overrides the QWidget's mouseReleasEvent method.
 * Dispatches the event
 * @param event the QMouseEvent
 */
void QXDirect::mouseReleaseEvent(QMouseEvent *event)
{
	TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;

	m_bTrans = false;
	p2DWidget->setCursor(Qt::CrossCursor);
}



/**
 * The user has requested a display of all polar graphs
 */
void QXDirect::OnAllPolarGraphs()
{
	m_iPlrView  = ALLPOLARGRAPHS;
	m_bPolarView = true;
	SetPolarLegendPos();
	SetControls();
	UpdateView();
}



/**
 * The user has requested an edition of all polar graphs settings
 */
void QXDirect::OnAllPolarGraphsSetting()
{
	QGraph graph;
	graph.CopySettings(m_PlrGraph);
	m_pGraphDlg->m_pMemGraph = &graph;
	m_pGraphDlg->m_pGraph    = m_PlrGraph;
	m_pGraphDlg->m_GraphArray[0] = m_PlrGraph;
	m_pGraphDlg->m_GraphArray[1] = m_PlrGraph+1;
	m_pGraphDlg->m_GraphArray[2] = m_PlrGraph+2;
	m_pGraphDlg->m_GraphArray[3] = m_PlrGraph+3;
	m_pGraphDlg->m_GraphArray[4] = m_PlrGraph+4;
	m_pGraphDlg->m_NGraph = 5;
	m_pGraphDlg->SetParams();

	if(m_pGraphDlg->exec() == QDialog::Accepted)
	{
	}
	else
	{
		m_PlrGraph[0].CopySettings(&graph);
		m_PlrGraph[2].CopySettings(&graph);
		m_PlrGraph[1].CopySettings(&graph);
		m_PlrGraph[3].CopySettings(&graph);
		m_PlrGraph[4].CopySettings(&graph);
	}
	UpdateView();
}


/**
 * The user has changed one of the analysis parameters. Reads all the data and maps it.
 */
void QXDirect::OnInputChanged()
{
	ReadParams();
}


/**
 * The user has clicked the animate checkcbox
 * @param bChecked the new state of the checkbox
 */
void QXDirect::OnAnimate(bool bChecked)
{
	if(!MainFrame::s_pCurFoil || !m_pCurPolar)
	{
		m_bAnimate = false;
		return;
	}

	OpPoint* pOpPoint;
	int l;

	if(bChecked)
	{
		for (l=0; l< m_poaOpp->size(); l++)
		{
			pOpPoint = (OpPoint*)m_poaOpp->at(l);

			if (pOpPoint &&
				pOpPoint->m_strPlrName  == m_pCurPolar->m_PlrName &&
				pOpPoint->m_strFoilName == MainFrame::s_pCurFoil->m_FoilName)
			{
					if(fabs(m_pCurOpp->Alpha - pOpPoint->Alpha)<0.0001)
						m_posAnimate = l-1;
			}
		}
		m_bAnimate  = true;
		int speed = m_pctrlAnimateSpeed->value();
		m_pAnimateTimer->setInterval(800-speed);
		m_pAnimateTimer->start();
	}
	else
	{
		m_pAnimateTimer->stop();
		m_bAnimate = false;
		if(m_posAnimate<0 || m_posAnimate>=m_poaOpp->size()) return;
		OpPoint* pOpPoint = (OpPoint*)m_poaOpp->at(m_posAnimate);
		if(pOpPoint) SetOpp(pOpPoint->Alpha);
//		UpdateView();
		return;
	}
}


/**
 * Called by the animation timer. Updates the display with the data of the next OpPoint.
 */
void QXDirect::OnAnimateSingle()
{
	static int indexCbBox;
	static QString str;
	bool bIsValid = false;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	OpPoint* pOpPoint;

	if(m_poaOpp->size()<=1) return;

	// find the next oppoint related to this foil and polar pair
	while(!bIsValid)
	{
		if(m_bAnimatePlus)
		{
			m_posAnimate++;
			if (m_posAnimate >= m_poaOpp->size())
			{
				m_posAnimate = m_poaOpp->size()-2;
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
		if(m_posAnimate<0 || m_posAnimate>=m_poaOpp->size()) return;

		pOpPoint = (OpPoint*)m_poaOpp->at(m_posAnimate);

		if (pOpPoint &&
			pOpPoint->m_strPlrName  == m_pCurPolar->m_PlrName &&
			pOpPoint->m_strFoilName == MainFrame::s_pCurFoil->m_FoilName &&
			pOpPoint != m_pCurOpp)
		{
			bIsValid = true;
			CreateOppCurves(pOpPoint);
			m_pCurOpp = pOpPoint;

			//select current OpPoint in Combobox
			if(m_pCurPolar->m_PolarType!=FIXEDAOAPOLAR) str = QString("%1").arg(m_pCurOpp->Alpha,8,'f',2);
			else                                        str = QString("%1").arg(m_pCurOpp->Reynolds,8,'f',2);
			indexCbBox = pMainFrame->m_pctrlOpPoint->findText(str);
			if(indexCbBox>=0) pMainFrame->m_pctrlOpPoint->setCurrentIndex(indexCbBox);

			UpdateView();
		}
	}
}


void QXDirect::OnAnimateSpeed(int val)
{
	if(m_pAnimateTimer->isActive())
	{
		m_pAnimateTimer->setInterval(800-val);
	}
}


void QXDirect::OnAnalyze()
{
	if(!MainFrame::s_pCurFoil || !m_pCurPolar) return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	m_pXFoil->lvisc = m_bViscous;

	ReadParams();

	bool bHigh = m_bHighlightOpp;
	m_bHighlightOpp = false;


	m_pXFoilAnalysisDlg->SetAlpha(m_Alpha, m_AlphaMax, m_AlphaDelta);
	m_pXFoilAnalysisDlg->SetCl(m_Cl, m_ClMax, m_ClDelta);
	m_pXFoilAnalysisDlg->SetRe(m_Reynolds, m_ReynoldsMax, m_ReynoldsDelta);

	m_pXFoilAnalysisDlg->m_bSequence = m_bSequence;
	m_pXFoilAnalysisDlg->m_bAlpha = m_bAlpha;
	m_pXFoilAnalysisDlg->m_bType4 = (m_pCurPolar->m_PolarType==FIXEDAOAPOLAR);

	m_pXFoilAnalysisDlg->m_FoilName = MainFrame::s_pCurFoil->m_FoilName;
	m_pXFoilAnalysisDlg->m_IterLim = m_IterLim;
	m_pXFoilAnalysisDlg->m_pXFoil = m_pXFoil;
	m_pXFoilAnalysisDlg->InitDialog();
	m_pXFoilAnalysisDlg->show();
	m_pXFoilAnalysisDlg->StartAnalysis();
	m_pXFoilAnalysisDlg->hide();
	m_pXFoilAnalysisDlg->move(m_pXFoilAnalysisDlg->x(), m_pXFoilAnalysisDlg->y());
// and update window

	m_bInitBL = !m_pXFoil->lblini;
	m_pctrlInitBL->setChecked(m_bInitBL);;

	m_pCurGraph = m_pCpGraph;

	pMainFrame->UpdateOpps();

	SetOpp();

	m_bHighlightOpp = bHigh;

	if(m_bPolarView) CreatePolarCurves();

	SetControls();
	UpdateView();
}


void QXDirect::OnBatchAnalysis()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(!MainFrame::s_pCurFoil) 		return;

	m_bPolarView = true;
	OnPolars();
	UpdateView();


//	m_pBDlg->move(pMainFrame->m_DlgPos);
	m_pBDlg->m_pFoil     = MainFrame::s_pCurFoil;
	m_pBDlg->m_Mach      = 0.0;
	m_pBDlg->m_ReMin     = m_Reynolds;
	m_pBDlg->m_ReMax     = m_ReynoldsMax;
	m_pBDlg->m_ReInc     = m_ReynoldsDelta;
	m_pBDlg->m_PolarType = m_PolarType;
	m_pBDlg->m_IterLim   = m_IterLim;
	m_pBDlg->m_bAlpha    = true;
	m_pBDlg->m_SpMin     = m_Alpha;
	m_pBDlg->m_SpMax     = m_AlphaMax;
	m_pBDlg->m_SpInc     = m_AlphaDelta;
	m_pBDlg->m_AlphaMin  = m_Alpha;
	m_pBDlg->m_AlphaMax  = m_AlphaMax;
	m_pBDlg->m_AlphaInc  = m_AlphaDelta;
	m_pBDlg->m_ClMin     = m_Cl;
	m_pBDlg->m_ClMax     = m_ClMax;
	m_pBDlg->m_ClInc     = m_ClDelta;
	m_pBDlg->m_NCrit     = m_NCrit;
	m_pBDlg->m_XTopTr    = m_XTopTr;
	m_pBDlg->m_XBotTr    = m_XBotTr;
	m_pBDlg->m_ReList    = m_ReList;
	m_pBDlg->m_MachList  = m_MachList;
	m_pBDlg->m_NCritList = m_NCritList;
	m_pBDlg->m_NRe       = m_NRe;
	m_pBDlg->m_bFromList = m_bFromList;
	m_pBDlg->m_bFromZero = m_bFromZero;
	m_pBDlg->InitDialog();

	if(m_pBDlg->exec()==QDialog::Accepted) pMainFrame->SetSaveState(false);
//	pMainFrame->m_DlgPos = m_pBDlg->pos();

	m_Reynolds         = m_pBDlg->m_ReMin;
	m_ReynoldsMax      = m_pBDlg->m_ReMax;
	m_ReynoldsDelta    = m_pBDlg->m_ReInc;
	m_Alpha            = m_pBDlg->m_AlphaMin;
	m_AlphaMax         = m_pBDlg->m_AlphaMax;
	m_AlphaDelta       = m_pBDlg->m_AlphaInc;
	m_Cl               = m_pBDlg->m_ClMin;
	m_ClMax            = m_pBDlg->m_ClMax;
	m_ClDelta          = m_pBDlg->m_ClInc;
	m_Mach             = m_pBDlg->m_Mach;
	m_PolarType        = m_pBDlg->m_PolarType;
	m_NCrit            = m_pBDlg->m_NCrit;
	m_XTopTr           = m_pBDlg->m_XTopTr;
	m_XBotTr           = m_pBDlg->m_XBotTr;
	m_NRe              = m_pBDlg->m_NRe;
	m_bAlpha           = m_pBDlg->m_bAlpha;
	m_bFromList        = m_pBDlg->m_bFromList;
	m_bFromZero        = m_pBDlg->m_bFromZero;

	SetPolar();
	pMainFrame->UpdatePolars();

	m_pCurOpp = NULL;

	SetControls();
	UpdateView();
}


void QXDirect::OnMultiThreadedBatchAnalysis()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(!MainFrame::s_pCurFoil) 		return;

	if(QThread::idealThreadCount()<2)
	{
		QString strange = tr("Not enough threads available for multithreading");
		QMessageBox::warning(pMainFrame, tr("Warning"), strange);
		return;
	}

	m_bPolarView = true;
	OnPolars();
	UpdateView();

	BatchThreadDlg *m_pBatchThreadDlg   = new BatchThreadDlg(pMainFrame);

	m_pBatchThreadDlg->m_pCurFoil  = MainFrame::s_pCurFoil;
	m_pBatchThreadDlg->m_Mach      = 0.0;
	m_pBatchThreadDlg->m_ReMin     = m_Reynolds;
	m_pBatchThreadDlg->m_ReMax     = m_ReynoldsMax;
	m_pBatchThreadDlg->m_ReInc     = m_ReynoldsDelta;
	m_pBatchThreadDlg->m_PolarType = FIXEDSPEEDPOLAR;
	m_pBatchThreadDlg->m_IterLim   = m_IterLim;
	m_pBatchThreadDlg->m_bAlpha    = true;
	m_pBatchThreadDlg->m_AlphaMin  = m_Alpha;
	m_pBatchThreadDlg->m_AlphaMax  = m_AlphaMax;
	m_pBatchThreadDlg->m_AlphaInc  = m_AlphaDelta;
	m_pBatchThreadDlg->m_ClMin     = m_Cl;
	m_pBatchThreadDlg->m_ClMax     = m_ClMax;
	m_pBatchThreadDlg->m_ClInc     = m_ClDelta;
	m_pBatchThreadDlg->m_NCrit     = m_NCrit;
	m_pBatchThreadDlg->m_XTopTr    = m_XTopTr;
	m_pBatchThreadDlg->m_XBotTr    = m_XBotTr;
	m_pBatchThreadDlg->m_ReList    = m_ReList;
	m_pBatchThreadDlg->m_MachList  = m_MachList;
	m_pBatchThreadDlg->m_NCritList = m_NCritList;
	m_pBatchThreadDlg->m_NRe       = m_NRe;
	m_pBatchThreadDlg->m_bFromList = m_bFromList;
	m_pBatchThreadDlg->m_bFromZero = m_bFromZero;
	m_pBatchThreadDlg->InitDialog();

	if(m_pBatchThreadDlg->exec()==QDialog::Accepted) pMainFrame->SetSaveState(false);

	m_Reynolds         = m_pBatchThreadDlg->m_ReMin;
	m_ReynoldsMax      = m_pBatchThreadDlg->m_ReMax;
	m_ReynoldsDelta    = m_pBatchThreadDlg->m_ReInc;
	m_Alpha            = m_pBatchThreadDlg->m_AlphaMin;
	m_AlphaMax         = m_pBatchThreadDlg->m_AlphaMax;
	m_AlphaDelta       = m_pBatchThreadDlg->m_AlphaInc;
	m_Cl               = m_pBatchThreadDlg->m_ClMin;
	m_ClMax            = m_pBatchThreadDlg->m_ClMax;
	m_ClDelta          = m_pBatchThreadDlg->m_ClInc;
	m_Mach             = m_pBatchThreadDlg->m_Mach;
	m_PolarType        = m_pBatchThreadDlg->m_PolarType;
	m_NCrit            = m_pBatchThreadDlg->m_NCrit;
	m_XTopTr           = m_pBatchThreadDlg->m_XTopTr;
	m_XBotTr           = m_pBatchThreadDlg->m_XBotTr;
	m_NRe              = m_pBatchThreadDlg->m_NRe;
	m_bAlpha           = m_pBatchThreadDlg->m_bAlpha;
	m_bFromList        = m_pBatchThreadDlg->m_bFromList;
	m_bFromZero        = m_pBatchThreadDlg->m_bFromZero;

	SetPolar();
	pMainFrame->UpdatePolars();

	m_pCurOpp = NULL;

	SetControls();
	UpdateView();
}


void QXDirect::OnCadd()
{
	StopAnimate();
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(!MainFrame::s_pCurFoil)		return;

	void* ptr = m_pCurOpp;
	m_pCurOpp = NULL;
	CreateOppCurves();

	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	bool bState = m_bShowPanels;
	m_bPressure = false;
	m_bBL       = false;
	OnOpPoints();


	m_pCAddDlg->m_pBufferFoil = &m_BufferFoil;
	m_pCAddDlg->m_pMemFoil    = MainFrame::s_pCurFoil;
	m_pCAddDlg->m_pXDirect    = this;
	m_pCAddDlg->m_pAFoil      = NULL;
	m_pCAddDlg->InitDialog();



	m_bShowPanels = true;
	UpdateView();

	if(QDialog::Accepted == m_pCAddDlg->exec())
	{
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 1;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		m_pCurOpp = (OpPoint*)ptr;
		if(!pMainFrame->SetModFoil(pNewFoil))	SetBufferFoil();
		pMainFrame->UpdateFoils();
	}
	else
	{
		m_pCurOpp = (OpPoint*)ptr;
		SetBufferFoil();
		m_pXFoil->InitXFoilGeometry(MainFrame::s_pCurFoil);
	}

	m_bShowPanels = bState;

	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();
}


void QXDirect::OnCfPlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;
	double x[IVX][3],y[IVX][3];
	int nside1, nside2, ibl;

	m_pCpGraph->SetYVariable(2);
	m_XFoilVar = 8;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle(tr("Cf"));
	Curve * pTopCurve = m_pCpGraph->AddCurve();
	Curve * pBotCurve = m_pCpGraph->AddCurve();
	pTopCurve->SetTitle(tr("Top"));
	pBotCurve->SetTitle(tr("Bot"));

	double que = 0.5*m_pXFoil->qinf*m_pXFoil->qinf;

	m_pXFoil->CreateXBL(x, nside1, nside2);
	//---- fill compressible ue arrays
	for (ibl=2; ibl<= nside1;ibl++)
	{
		y[ibl][1] = m_pXFoil->tau[ibl][1] / que;
	}
	for ( ibl=2; ibl<= nside2;ibl++)
	{
		y[ibl][2] = m_pXFoil->tau[ibl][2] / que;
	}

	for (i=2; i<=nside1-1; i++)
	{
		pTopCurve->AppendPoint(x[i][1], y[i][1]);
	}
	for (i=2; i<=nside2-1; i++)
	{
		pBotCurve->AppendPoint(x[i][2], y[i][2]);
	}
	m_pCpGraph->SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}


void QXDirect::OnCdPlot()
{
	if(!m_pXFoil->lvconv) return;
	double x[IVX][3],y[IVX][3];
	int nside1, nside2, ibl;
	int i;

	m_pCpGraph->SetYVariable(2);
	m_XFoilVar = 7;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle(tr("Cd'"));
	Curve * pTopCurve = m_pCpGraph->AddCurve();
	Curve * pBotCurve = m_pCpGraph->AddCurve();
	pTopCurve->SetTitle(tr("Top"));
	pBotCurve->SetTitle(tr("Bot"));

	double qrf = m_pXFoil->qinf;

	m_pXFoil->CreateXBL(x, nside1, nside2);
	//---- fill compressible ue arrays
	for (ibl=2; ibl<= nside1;ibl++)
	{
		y[ibl][1] = m_pXFoil->dis[ibl][1] / qrf/ qrf/ qrf;
	}
	for ( ibl=2; ibl<= nside2;ibl++)
	{
		y[ibl][2] = m_pXFoil->dis[ibl][2] / qrf/ qrf/ qrf;
	}

	for (i=2; i<=nside1-1; i++)
	{
		pTopCurve->AppendPoint(x[i][1], y[i][1]);
	}
	for (i=2; i<=nside2-1; i++)
	{
		pBotCurve->AppendPoint(x[i][2], y[i][2]);
	}
	m_pCpGraph->SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}


void QXDirect::OnCouplePolarGraphs()
{
	m_iPlrView  = TWOPOLARGRAPHS;
	m_bPolarView = true;
	SetControls();
	UpdateView();
}



void QXDirect::OnCpGraph()
{
	m_bPolarView = false;
	if(m_pCpGraph->GetYVariable()!=0)
	{
//		m_pCpGraph->ResetLimits();
		m_pCpGraph->SetAuto(true);
		m_pCpGraph->SetYVariable(0);
	}
	m_pCpGraph->SetYVariable(0);
	m_pCpGraph->SetInverted(true);
	CreateOppCurves();
	m_pCpGraph->SetYTitle(tr("Cp"));

	SetControls();
	m_pCpGraph->SetXScale();
	SetFoilScale();
	UpdateView();
}



void QXDirect::OnCpGraphSettings()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	m_pGraphDlg->move(pMainFrame->m_DlgPos);

	QGraph graph;
	graph.CopySettings(m_pCpGraph);
	m_pGraphDlg->m_pMemGraph = m_pCurGraph;
	m_pGraphDlg->m_pGraph = &graph;
	m_pGraphDlg->SetParams();

	if(m_pGraphDlg->exec() == QDialog::Accepted)
	{
		m_pCpGraph->CopySettings(&graph);
	}
	pMainFrame->m_DlgPos = m_pGraphDlg->pos();
	UpdateView();
}


void QXDirect::OnCpi()
{
	m_bShowInviscid = !m_bShowInviscid;

	CreateOppCurves();
	SetControls();
	UpdateView();
}

void QXDirect::OnCtPlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_pCpGraph->SetYVariable(2);
	m_XFoilVar=1;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle(tr("Max Shear"));
	Curve * pCurve0 = m_pCpGraph->AddCurve();
	Curve * pCurve1 = m_pCpGraph->AddCurve();
	Curve * pCurve2 = m_pCpGraph->AddCurve();
	Curve * pCurve3 = m_pCpGraph->AddCurve();
	pCurve0->SetTitle(tr("Top Shear"));
	pCurve1->SetTitle(tr("Top Shear eq"));
	pCurve2->SetTitle(tr("Bot Shear"));
	pCurve3->SetTitle(tr("Bot Shear eq"));

	double x[IVX][3];
	int nside1, nside2;

	m_pXFoil->CreateXBL(x, nside1, nside2);

	int it1 = m_pXFoil->itran[1];
	int it2 = m_pXFoil->itran[2];

	for (i=it1; i<=nside1-1; i++)	pCurve0->AppendPoint(x[i][1], m_pXFoil->ctau[i][1]);
	for (i=2; i<=nside1-1; i++)		pCurve1->AppendPoint(x[i][1], m_pXFoil->ctq[i][1]);

	for (i=it2; i<=nside2-1; i++)	pCurve2->AppendPoint(x[i][2], m_pXFoil->ctau[i][2]);
	for (i=2; i<=nside2-1; i++)		pCurve3->AppendPoint(x[i][2], m_pXFoil->ctq[i][2]);

	m_pCpGraph->SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();

}

void QXDirect::OnCurOppOnly()
{
	m_bCurOppOnly = !m_bCurOppOnly;
	((MainFrame*)s_pMainFrame)->showCurOppOnly->setChecked(m_bCurOppOnly);

	if(m_pCurOpp) m_pCurOpp->m_bIsVisible = true;
	CreateOppCurves();
	SetAnalysisParams();
	UpdateView();
}


void QXDirect::OnCurveColor()
{
	QColor Color = QColorDialog::getColor(m_CurveColor);
	if(Color.isValid()) m_CurveColor = Color;

	FillComboBoxes();
	UpdateCurve();
}


void QXDirect::OnCurveStyle(int index)
{
	m_CurveStyle = index;
	FillComboBoxes();
	UpdateCurve();
}


void QXDirect::OnCurveWidth(int index)
{
	m_CurveWidth = index+1;
	FillComboBoxes();
	UpdateCurve();
}


void QXDirect::OnDefinePolar()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	int i,j;
	Polar *pPolar;
	bool bFound;
	if(!MainFrame::s_pCurFoil) return;

	m_pFPDlg->m_NCrit     = m_NCrit;
	m_pFPDlg->m_XBotTr    = m_XBotTr;
	m_pFPDlg->m_XTopTr    = m_XTopTr;
	m_pFPDlg->m_Mach      = m_Mach;
	m_pFPDlg->m_Reynolds  = m_Reynolds;
	m_pFPDlg->m_PolarType = m_PolarType;
	m_pFPDlg->m_ASpec     = m_ASpec;

	m_pFPDlg->InitDialog();

	int res = m_pFPDlg->exec();
	if (res == QDialog::Accepted)
	{
		m_pCurPolar = new Polar();

		m_pCurPolar->m_FoilName = MainFrame::s_pCurFoil->m_FoilName;
		m_pCurPolar->m_PlrName = m_pFPDlg->m_PlrName;
		m_pCurPolar->m_bIsVisible = true;
		m_pCurPolar->m_PolarType = m_pFPDlg->m_PolarType;

		switch (m_pCurPolar->m_PolarType)
		{
			case FIXEDSPEEDPOLAR:
				m_pCurPolar->m_MaType = 1;
				m_pCurPolar->m_ReType = 1;
				break;
			case FIXEDLIFTPOLAR:
				m_pCurPolar->m_MaType = 2;
				m_pCurPolar->m_ReType = 2;
				break;
			case RUBBERCHORDPOLAR:
				m_pCurPolar->m_MaType = 1;
				m_pCurPolar->m_ReType = 3;
				break;
			case FIXEDAOAPOLAR:
				m_pCurPolar->m_MaType = 1;
				m_pCurPolar->m_ReType = 1;
				break;
			default:
				m_pCurPolar->m_ReType = 1;
				m_pCurPolar->m_MaType = 1;
				break;
		}

		m_PolarType = m_pFPDlg->m_PolarType;
		m_NCrit     = m_pFPDlg->m_NCrit;
		m_XBotTr    = m_pFPDlg->m_XBotTr;
		m_XTopTr    = m_pFPDlg->m_XTopTr;
		m_Mach      = m_pFPDlg->m_Mach;
		m_Reynolds  = m_pFPDlg->m_Reynolds;
		m_ASpec     = m_pFPDlg->m_ASpec;

		m_pCurPolar->m_Reynolds = m_pFPDlg->m_Reynolds;
		m_pCurPolar->m_Mach     = m_pFPDlg->m_Mach;
		m_pCurPolar->m_ASpec    = m_pFPDlg->m_ASpec;
		m_pCurPolar->m_ACrit    = m_pFPDlg->m_NCrit;
		m_pCurPolar->m_XTop     = m_pFPDlg->m_XTopTr;
		m_pCurPolar->m_XBot     = m_pFPDlg->m_XBotTr;
		m_pCurPolar->m_Color = pMainFrame->GetColor(1);

		m_pCurPolar = pMainFrame->AddPolar(m_pCurPolar);

		for(i=0; i<12;i++)
		{
			bFound = false;
			for (j=0; j<m_poaPolar->size();j++)
			{
			pPolar = (Polar*)m_poaPolar->at(j);
			if(pPolar->m_Color == pMainFrame->m_ColorList[i]) bFound = true;
			}
			if(!bFound)
			{
			m_pCurPolar->m_Color = pMainFrame->m_ColorList[i];
			break;
			}
		}

		SetPolar(m_pCurPolar);
		pMainFrame->UpdatePolars();
		SetBufferFoil();
		UpdateView();
		pMainFrame->SetSaveState(false);
	}
	SetControls();
}



void QXDirect::OnDeleteCurFoil()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	MainFrame::s_pCurFoil = pMainFrame->DeleteFoil(MainFrame::s_pCurFoil,true);
	m_pCurOpp = NULL;
	m_pCurPolar = NULL;

	pMainFrame->UpdateFoils();

	if(m_bPolarView) CreatePolarCurves();
	else         CreateOppCurves();
	pMainFrame->SetSaveState(false);

	SetControls();
	UpdateView();
}


void QXDirect::OnDelCurOpp()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	OpPoint* pOpPoint = m_pCurOpp;
	StopAnimate();

	if (!pOpPoint) return;
	QString strong,str;
	strong = tr("Are you sure you want to delete the Operating Point\n");
	if(m_pCurPolar->m_PolarType!=FIXEDAOAPOLAR) str = QString("Alpha = %1").arg(pOpPoint->Alpha,0,'f',2);
	else                                        str = QString("Reynolds = %1").arg(pOpPoint->Reynolds,0,'f',0);
	strong += str;
	strong += "  ?";

	if (QMessageBox::Yes == QMessageBox::question(pMainFrame, tr("Question"), strong,
		QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))
	{
		pMainFrame->RemoveOpPoint(true);
		pMainFrame->UpdateOpps();
		SetOpp();
		UpdateView();
	}
	SetControls();
}



void QXDirect::OnDeleteCurPolar()
{
	if(!m_pCurPolar) return;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	OpPoint *pOpPoint;
	int l;
	QString str;

	str = tr("Are you sure you want to delete the polar :\n  ") + m_pCurPolar->m_PlrName;
	str += tr("\n and all the associated OpPoints ?");

	if (QMessageBox::Yes == QMessageBox::question(pMainFrame, tr("Question"), str,
		QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))
	{
		// start by removing all OpPoints
		for (l=m_poaOpp->size()-1; l>=0; l--)
		{
			pOpPoint = (OpPoint*)m_poaOpp->at(l);
			if (pOpPoint->m_strPlrName  == m_pCurPolar->m_PlrName &&
				pOpPoint->m_strFoilName == MainFrame::s_pCurFoil->m_FoilName)
			{
				m_poaOpp->removeAt(l);
				delete pOpPoint;
			}
		}
		// then remove the CPolar and update views
		for (l=m_poaPolar->size()-1; l>=0; l--)
		{
			if(m_pCurPolar == m_poaPolar->at(l))
			{
				m_poaPolar->removeAt(l);
				delete m_pCurPolar;
				break;
			}
		}
		m_pCurOpp   = NULL;
		m_pCurPolar = NULL;
	}

	pMainFrame->UpdatePolars();
	SetPolar();

	pMainFrame->SetSaveState(false);
	UpdateView();
}


void QXDirect::OnDeletePolarOpps()
{
	if(!MainFrame::s_pCurFoil || !m_pCurPolar) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	OpPoint *pOpp;

	for(int i=m_poaOpp->size()-1; i>=0; i--)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		if(pOpp->m_strFoilName==MainFrame::s_pCurFoil->m_FoilName && pOpp->m_strPlrName==m_pCurPolar->m_PlrName)
		{
			m_poaOpp->removeAt(i);
			delete pOpp;
		}
	}

	m_pCurOpp=NULL;
	pMainFrame->SetSaveState(false);
	pMainFrame->UpdateOpps();
	if(!m_bPolarView) CreateOppCurves();
	SetCurveParams();
	SetControls();
	UpdateView();
}



void QXDirect::OnDeleteFoilOpps()
{
	if(!MainFrame::s_pCurFoil || !m_pCurPolar) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	OpPoint *pOpp;

	for(int i=m_poaOpp->size()-1; i>=0; i--)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		if(pOpp->m_strFoilName==MainFrame::s_pCurFoil->m_FoilName)
		{
			m_poaOpp->removeAt(i);
			delete pOpp;
		}
	}
	m_pCurOpp = NULL;
	pMainFrame->SetSaveState(false);
	pMainFrame->UpdateOpps();
	if(!m_bPolarView) CreateOppCurves();
	SetCurveParams();
	SetControls();
	UpdateView();
}


void QXDirect::OnDeleteFoilPolars()
{
	if(!MainFrame::s_pCurFoil) return;
	MainFrame * pMainFrame = (MainFrame*) s_pMainFrame;
	int l;
	OpPoint *pOpPoint;
	StopAnimate();

	QString strong;

	strong = tr("Are you sure you want to delete polars and OpPoints\n");
	strong +=tr("associated to ")+MainFrame::s_pCurFoil->m_FoilName  + " ?";
	if (QMessageBox::Yes == QMessageBox::question(pMainFrame, tr("Question"), strong,
		QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))

	{
		// start by removing all OpPoints
		for (l=m_poaOpp->size()-1; l>=0; l--)
		{
			pOpPoint = (OpPoint*)m_poaOpp->at(l);
			if (pOpPoint->m_strFoilName == MainFrame::s_pCurFoil->m_FoilName)
			{
				m_poaOpp->removeAt(l);
				delete pOpPoint;
			}
		}
		// then remove CPolar and update views
		Polar* pPolar;
		for (l=m_poaPolar->size()-1; l>=0; l--)
		{
			pPolar = (Polar*)m_poaPolar->at(l);
			if (pPolar->m_FoilName == MainFrame::s_pCurFoil->m_FoilName)
			{
				m_poaPolar->removeAt(l);
				delete pPolar;
			}
		}
		m_pCurOpp = NULL;

	}
	m_pCurPolar = NULL;
	SetPolar();
	pMainFrame->UpdatePolars();
	pMainFrame->SetSaveState(false);

	SetControls();
	UpdateView();
}


void QXDirect::OnDerotateFoil()
{
	if(!MainFrame::s_pCurFoil) return;
	QString str;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	StopAnimate();

	Foil *pNewFoil = new Foil;
	pNewFoil->CopyFoil(MainFrame::s_pCurFoil);

	double angle = pNewFoil->DeRotate();
	str = QString(tr("The foil has been de-rotated by %1 degrees")).arg(angle,6,'f',3);
	pMainFrame->statusBar()->showMessage(str);

	pNewFoil->m_nFoilStyle = 0;
	pNewFoil->m_nFoilWidth = 1;
	pNewFoil->m_bPoints = false;
	pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);

	if(!pMainFrame->SetModFoil(pNewFoil))	SetBufferFoil();
	pMainFrame->UpdateFoils();
	pMainFrame->SetSaveState(false);

	UpdateView();
}


void QXDirect::OnDtPlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_pCpGraph->SetYVariable(2);
	m_XFoilVar=3;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle(" ");


	double x[IVX][3];
	int nside1, nside2;

	Curve * pCurve1 = m_pCpGraph->AddCurve();
	Curve * pCurve2 = m_pCpGraph->AddCurve();

	pCurve1->SetTitle("D*");
	pCurve2->SetTitle("Theta");
	m_pXFoil->CreateXBL(x, nside1, nside2);

	for (i=2; i<nside1; i++){
		pCurve1->AppendPoint(x[i][1], m_pXFoil->dstr[i][1]);
		pCurve2->AppendPoint(x[i][1], m_pXFoil->thet[i][1]);
	}

	m_pCpGraph->SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}

void QXDirect::OnDbPlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_pCpGraph->SetYVariable(2);
	m_XFoilVar = 2;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle(" ");


	double x[IVX][3];
	int nside1, nside2;

	Curve * pCurve1 = m_pCpGraph->AddCurve();
	Curve * pCurve2 = m_pCpGraph->AddCurve();

	pCurve1->SetTitle("D*");
	pCurve2->SetTitle("Theta");
	m_pXFoil->CreateXBL(x, nside1, nside2);

	for (i=2; i<nside2; i++)
	{
		pCurve1->AppendPoint(x[i][2], m_pXFoil->dstr[i][2]);
		pCurve2->AppendPoint(x[i][2], m_pXFoil->thet[i][2]);
	}

	m_pCpGraph->SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}




void QXDirect::OnEditCurPolar()
{
	if (!m_pCurPolar) return;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	Polar MemPolar;
	MemPolar.Copy(m_pCurPolar);

	m_pEditPlrDlg->m_pPolar = m_pCurPolar;
	m_pEditPlrDlg->m_pXDirect = this;
	m_pEditPlrDlg->move(EditPlrDlg::s_WindowPos);
	m_pEditPlrDlg->resize(EditPlrDlg::s_WindowSize);
	if(EditPlrDlg::s_bWindowMaximized) m_pEditPlrDlg->setWindowState(Qt::WindowMaximized);

	m_pEditPlrDlg->InitDialog();

	bool bPoints = m_pCurPolar->m_bShowPoints;
	m_pCurPolar->m_bShowPoints = true;

	CreatePolarCurves();
	UpdateView();

	if(m_pEditPlrDlg->exec() == QDialog::Accepted)
	{
		pMainFrame->SetSaveState(false);
	}
	else
	{
		m_pCurPolar->Copy(&MemPolar);
	}
	m_pCurPolar->m_bShowPoints = bPoints;
	CreatePolarCurves();
	UpdateView();
}



void QXDirect::OnExportCurXFoilResults()
{
	if(!m_pXFoil->lvconv) return;
	if(!MainFrame::s_pCurFoil)		  return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString FileName,  OutString, strong;

	double x[IVX][3],Hk[IVX][3],UeVinf[IVX][3], Cf[IVX][3], Cd[IVX][3], AA0[IVX][3];
	double RTheta[IVX][3], DStar[IVX][3], Theta[IVX][3];
	double uei;
	double que = 0.5*m_pXFoil->qinf*m_pXFoil->qinf;
	double qrf = m_pXFoil->qinf;
	int nside1, nside2, ibl;
	int type = 1;

	FileName = MainFrame::s_pCurFoil->m_FoilName;
	FileName.replace("/", " ");

	FileName = QFileDialog::getSaveFileName(this, tr("Export Current XFoil Results"),
											pMainFrame->m_LastDirName,
											tr("Text File (*.txt);;Comma Separated Values (*.csv)"));

	if(!FileName.length()) return;
	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);

	pos  = FileName.lastIndexOf(".csv");
	if(pos>0) type = 2;

	QFile DestFile(FileName);

	if (!DestFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&DestFile);

	out << (pMainFrame->m_VersionName);
	out << ("\n");
	strong = m_pXFoil->m_FoilName+ "\n";
	out << (strong);

	if(type==1)
		strong = QString("Alpha = %1,  Re = %2,  Ma= %3,  ACrit=%4\n\n")
						 .arg(m_pXFoil->alfa*180./PI, 5, 'f',1)
						 .arg(m_pXFoil->reinf1, 8, 'f',0)
						 .arg(m_pXFoil->minf1, 6, 'f',4)
						 .arg(m_pXFoil->acrit, 4, 'f',1);
	else
		strong = QString("Alpha =, %1,Re =, %3,Ma=, %3,ACrit =,%4\n\n")
						 .arg(m_pXFoil->alfa*180./PI, 5, 'f',1)
						 .arg(m_pXFoil->reinf1, 8, 'f',0)
						 .arg(m_pXFoil->minf1, 6, 'f',4)
						 .arg(m_pXFoil->acrit, 4, 'f',1);	out << (strong);

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

	out << tr("\nTop Side\n");
	if(type==1) OutString = QString(tr("    x         Hk     Ue/Vinf      Cf        Cd     A/A0       D*       Theta      CTq\n"));
	else        OutString = QString(tr("x,Hk,Ue/Vinf,Cf,Cd,A/A0,D*,Theta,CTq\n"));
	out << (OutString);
	for (ibl=2; ibl<nside1; ibl++)
	{
		if(type==1)
			OutString = QString("%1  %2  %3  %4 %5 %6  %7  %8  %9\n")
							.arg(x[ibl][1])
							.arg(Hk[ibl][1],8,'f',5)
							.arg(UeVinf[ibl][1],8,'f',5)
							.arg(Cf[ibl][1],8,'f',5)
							.arg(Cd[ibl][1],8,'f',5)
							.arg(AA0[ibl][1],8,'f',5)
							.arg(DStar[ibl][1],8,'f',5)
							.arg(Theta[ibl][1],8,'f',5)
							.arg(m_pXFoil->ctq[ibl][1],8,'f',5);
		else
			OutString = QString("%1, %2, %3, %4, %5, %6, %7, %8, %9\n")
							.arg(x[ibl][1])
							.arg(Hk[ibl][1],8,'f',5)
							.arg(UeVinf[ibl][1],8,'f',5)
							.arg(Cf[ibl][1],8,'f',5)
							.arg(Cd[ibl][1],8,'f',5)
							.arg(AA0[ibl][1],8,'f',5)
							.arg(DStar[ibl][1],8,'f',5)
							.arg(Theta[ibl][1],8,'f',5)
							.arg(m_pXFoil->ctq[ibl][1],8,'f',5);
		out << (OutString);
	}
	out << tr("\n\nBottom Side\n");
	if(type==1) OutString = QString(tr("    x         Hk     Ue/Vinf      Cf        Cd     A/A0       D*       Theta      CTq\n"));
	else        OutString = QString(tr("x,Hk,Ue/Vinf,Cf,Cd,A/A0,D*,Theta,CTq\n"));
	out << (OutString);
	for (ibl=2; ibl<nside2; ibl++)
	{
		if(type==1)
			OutString = QString("%1  %2  %3  %4 %5 %6  %7  %8  %9\n")
							.arg(x[ibl][2])
							.arg(Hk[ibl][2],8,'f',5)
							.arg(UeVinf[ibl][2],8,'f',5)
							.arg(Cf[ibl][2],8,'f',5)
							.arg(Cd[ibl][2],8,'f',5)
							.arg(AA0[ibl][2],8,'f',5)
							.arg(DStar[ibl][2],8,'f',5)
							.arg(Theta[ibl][2],8,'f',5)
							.arg(m_pXFoil->ctq[ibl][2],8,'f',5);
		else
			OutString = QString("%1, %2, %3, %4, %5, %6, %7, %8, %9\n")
							.arg(x[ibl][2])
							.arg(Hk[ibl][2],8,'f',5)
							.arg(UeVinf[ibl][2],8,'f',5)
							.arg(Cf[ibl][2],8,'f',5)
							.arg(Cd[ibl][2],8,'f',5)
							.arg(AA0[ibl][2],8,'f',5)
							.arg(DStar[ibl][2],8,'f',5)
							.arg(Theta[ibl][2],8,'f',5)
							.arg(m_pXFoil->ctq[ibl][2],8,'f',5);
		out << (OutString);
	}

	DestFile.close();
}



void QXDirect::OnExportAllPolars()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString FileName, DirName;
	QFile XFile;
	QTextStream out(&XFile);

	//select the directory for output
	DirName = QFileDialog::getExistingDirectory(this,  tr("Export Directory"), pMainFrame->m_LastDirName);

	Polar *pPolar;
	for(int l=0; l<m_poaPolar->size(); l++)
	{
		pPolar = (Polar*)m_poaPolar->at(l);
		FileName = DirName + "/" + pPolar->m_FoilName + "_" + pPolar->m_PlrName;
		if(pMainFrame->m_ExportFileType==TXT) FileName += ".txt";
		else                                  FileName += ".csv";

		XFile.setFileName(FileName);
		if (XFile.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			pPolar->ExportPolar(out, pMainFrame->m_ExportFileType);
			XFile.close();
		}
	}
}



void QXDirect::OnExportCurFoil()
{
	if(!MainFrame::s_pCurFoil)	return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString FileName;

	FileName = MainFrame::s_pCurFoil->m_FoilName;
	FileName.replace("/", " ");

	FileName = QFileDialog::getSaveFileName(this, tr("Export Foil"),
											pMainFrame->m_LastDirName+"/"+FileName+".dat",
											tr("Foil File (*.dat)"));

	if(!FileName.length()) return;
	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);

	MainFrame::s_pCurFoil->ExportFoil(out);
	XFile.close();
}


void QXDirect::OnExportCurOpp()
{
	if(!MainFrame::s_pCurFoil || !m_pCurPolar || !m_pCurOpp)	return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString FileName;

	QString filter;
	if(pMainFrame->m_ExportFileType==TXT) filter = "Text File (*.txt)";
	else                                  filter = "Comma Separated Values (*.csv)";

	FileName = QFileDialog::getSaveFileName(this, tr("Export OpPoint"),
											pMainFrame->m_LastDirName ,
											tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
											&filter);
	if(!FileName.length()) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);
	pos = FileName.lastIndexOf(".csv");
	if (pos>0) pMainFrame->m_ExportFileType = CSV;
	else       pMainFrame->m_ExportFileType = TXT;

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);

	m_pCurOpp->ExportOpp(out, pMainFrame->m_VersionName, pMainFrame->m_ExportFileType);
	XFile.close();
}


void QXDirect::OnExportPolarOpps()
{
	MainFrame * pMainFrame = (MainFrame*)s_pMainFrame;
	if(!m_poaPolar->size())
	{
		QMessageBox::warning(pMainFrame, tr("Warning"), "No Operating Points to export to file");
		return;
	}

	int i,j;
	QString FileName;

	QString filter;
	if(pMainFrame->m_ExportFileType==TXT) filter = "Text File (*.txt)";
	else                                  filter = "Comma Separated Values (*.csv)";

	FileName = QFileDialog::getSaveFileName(this, tr("Export OpPoint"),
											pMainFrame->m_LastDirName ,
											tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
											&filter);

	if(!FileName.length()) return;
	if(!FileName.length()) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);
	pos = FileName.lastIndexOf(".csv");
	if (pos>0) pMainFrame->m_ExportFileType = CSV;
	else       pMainFrame->m_ExportFileType = TXT;

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);


	QString Header, strong;
	out<<pMainFrame->m_VersionName;
	out<<"\n\n";
	strong = MainFrame::s_pCurFoil->m_FoilName + "\n";
	out << strong;

	OpPoint *pOpPoint;

	for (i=0; i<m_poaOpp->size(); i++)
	{
		pOpPoint = (OpPoint*)m_poaOpp->at(i);
		if(pOpPoint->m_strFoilName == m_pCurPolar->m_FoilName && pOpPoint->m_strPlrName == m_pCurPolar->m_PlrName )
		{
			if(pMainFrame->m_ExportFileType==TXT)
				strong = QString("Reynolds = %1   Mach = %2  NCrit = %3\n")
									.arg(pOpPoint->Reynolds, 7, 'f', 0)
									.arg(pOpPoint->Mach, 4,'f',0)
									.arg(pOpPoint->ACrit, 3, 'f',1);
			else
				strong = QString("Reynolds =, %1,Mach =, %2,NCrit =, %3\n")
						.arg(pOpPoint->Reynolds, 7, 'f', 0)
						.arg(pOpPoint->Mach, 4,'f',0)
						.arg(pOpPoint->ACrit, 3, 'f',1);

			out<<strong;
			if(pMainFrame->m_ExportFileType==1) Header = QString("  Alpha        Cd        Cl        Cm        XTr1      XTr2   TEHMom    Cpmn\n");
			else        Header = QString("Alpha,Cd,Cl,Cm,XTr1,XTr2,TEHMom,Cpmn\n");
			out<<Header;

			if(pMainFrame->m_ExportFileType==TXT)
				strong = QString("%1   %2   %3   %4   %5   %6   %7  %8\n")
					.arg(pOpPoint->Alpha,7,'f',3)
					.arg(pOpPoint->Cd,9,'f',3)
					.arg(pOpPoint->Cl,7,'f',3)
					.arg(pOpPoint->Cm,7,'f',3)
					.arg(pOpPoint->Xtr1,7,'f',3)
					.arg(pOpPoint->Xtr2,7,'f',3)
					.arg(pOpPoint->m_TEHMom,7,'f',4)
					.arg(pOpPoint->Cpmn,7,'f',4);
			else
				strong = QString("%1,%2,%3,%4,%5,%6,%7,%8\n")
				.arg(pOpPoint->Alpha,7,'f',3)
				.arg(pOpPoint->Cd,9,'f',3)
				.arg(pOpPoint->Cl,7,'f',3)
				.arg(pOpPoint->Cm,7,'f',3)
				.arg(pOpPoint->Xtr1,7,'f',3)
				.arg(pOpPoint->Xtr2,7,'f',3)
				.arg(pOpPoint->m_TEHMom,7,'f',4)
				.arg(pOpPoint->Cpmn,7,'f',4);

			out<<strong;
			if(pMainFrame->m_ExportFileType==TXT) out<< " Cpi          Cpv\n-----------------\n";
			else                                  out << "Cpi,Cpv\n";

			for (j=0; j<pOpPoint->n; j++)
			{
				if(pOpPoint->m_bViscResults)
				{
					if(pMainFrame->m_ExportFileType==TXT) strong = QString("%1   %2\n").arg(pOpPoint->Cpi[j], 7,'f',4).arg(pOpPoint->Cpv[j], 7, 'f',4);
					else                                  strong = QString("%1,%2\n").arg(pOpPoint->Cpi[j], 7,'f',4).arg(pOpPoint->Cpv[j], 7, 'f',4);
				}
				else
				{
					strong=QString("%1\n").arg(pOpPoint->Cpi[j],7,'f',4);
				}

				out << strong;
			}
			out << "\n\n";
		}
	}
	XFile.close();


}


void QXDirect::OnExportCurPolar()
{
	if(!MainFrame::s_pCurFoil || !m_pCurPolar)	return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString FileName, filter;

	if(pMainFrame->m_ExportFileType==TXT) filter = "Text File (*.txt)";
	else                                  filter = "Comma Separated Values (*.csv)";

	FileName = m_pCurPolar->m_PlrName;
	FileName.replace("/", " ");
	FileName = QFileDialog::getSaveFileName(this, tr("Export Polar"),
											pMainFrame->m_LastDirName + "/"+FileName,
											tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
											&filter);
	if(!FileName.length()) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);
	pos = FileName.lastIndexOf(".csv");
	if (pos>0) pMainFrame->m_ExportFileType = CSV;
	else       pMainFrame->m_ExportFileType = TXT;

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);

	m_pCurPolar->ExportPolar(out, pMainFrame->m_ExportFileType);
	XFile.close();
}


void QXDirect::OnFoilCoordinates()
{
	if(!MainFrame::s_pCurFoil)	return;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	StopAnimate();

	bool bState = m_bShowPanels;//save current view setting

	void* ptr = m_pCurOpp;
	m_pCurOpp = NULL;
	CreateOppCurves();

	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	m_bPressure = false;
	m_bBL       = false;
	m_bPolarView    = false;
	UpdateView();

	bool bFlap       = m_BufferFoil.m_bTEFlap;
	double FlapAngle = m_BufferFoil.m_TEFlapAngle;
	double Xh        = m_BufferFoil.m_TEXHinge;
	double Yh        = m_BufferFoil.m_TEXHinge;

	m_BufferFoil.m_bTEFlap = false;
//	SetFoilFlap(&m_BufferFoil);

	m_pFoilCoordDlg->m_pXDirect = this;
	m_pFoilCoordDlg->m_pAFoil = NULL;
	m_pFoilCoordDlg->m_pMemFoil    = MainFrame::s_pCurFoil;
	m_pFoilCoordDlg->m_pBufferFoil = &m_BufferFoil;
	m_pFoilCoordDlg->InitDialog();

	m_bShowPanels = true;
	UpdateView();

	int res = m_pFoilCoordDlg->exec();

	if(QDialog::Accepted == res)
	{
		m_BufferFoil.m_bTEFlap = bFlap;
		m_BufferFoil.m_TEFlapAngle = FlapAngle;
		m_BufferFoil.m_TEXHinge = Xh;
		m_BufferFoil.m_TEYHinge = Yh;

		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		m_pCurOpp = (OpPoint*)ptr;
		if(!pMainFrame->SetModFoil(pNewFoil))	SetBufferFoil();
		pMainFrame->UpdateFoils();
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
		m_pXFoil->InitXFoilGeometry(MainFrame::s_pCurFoil);
	}

	m_BufferFoil.m_iHighLight = -1;
	m_bShowPanels = bState;//restore as it was
	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();
}


void QXDirect::OnFoilGeom()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	if(!MainFrame::s_pCurFoil)	return;

	StopAnimate();
	m_bPolarView   = false;

	void* ptr = m_pCurOpp;
	m_pCurOpp = NULL;
	CreateOppCurves();
	UpdateView();

	m_pFoilGeomDlg->m_pMemFoil = MainFrame::s_pCurFoil;
	m_pFoilGeomDlg->m_pBufferFoil = &m_BufferFoil;
	m_pFoilGeomDlg->m_pXDirect = this;
	m_pFoilGeomDlg->m_pAFoil   = NULL;
	m_pFoilGeomDlg->InitDialog();

	if(m_pFoilGeomDlg->exec() == QDialog::Accepted)
	{
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		m_pCurOpp = (OpPoint*)ptr;
		if(!pMainFrame->SetModFoil(pNewFoil))	SetBufferFoil();
		pMainFrame->UpdateFoils();
	}
	else
	{
		m_pCurOpp = (OpPoint*)ptr;
		SetBufferFoil();
		m_pXFoil->InitXFoilGeometry(MainFrame::s_pCurFoil);
	}

	UpdateView();
}



void QXDirect::OnGraphSettings()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	QGraph *pGraph = NULL;
	m_pGraphDlg->move(pMainFrame->m_DlgPos);

	pGraph = m_pCurGraph;
	if(!pGraph) return;

	if(!m_bPolarView)
	{
		m_pGraphDlg->m_iGraphType = 51;
	}
	else
	{
		m_pGraphDlg->m_iGraphType = 52;
	}

	QGraph graph;
	graph.CopySettings(pGraph);
	m_pGraphDlg->m_pMemGraph = &graph;
	m_pGraphDlg->m_pGraph = pGraph;

	m_pGraphDlg->SetParams();
	int res = m_pGraphDlg->exec();
	pMainFrame->m_DlgPos = m_pGraphDlg->pos();

	if(res == QDialog::Accepted)
	{
		if(!m_bPolarView)
		{
			if(m_pCpGraph->GetYVariable() == 0 || m_pCpGraph->GetYVariable()>=2)
			{
				m_pCpGraph->SetYTitle(tr("Cp"));
				m_pCpGraph->SetInverted(true);
			}
			else
			{
				m_pCpGraph->SetYTitle(tr("Q"));
				m_pCpGraph->SetInverted(false);
			}
			m_pCpGraph->ResetYLimits();
			CreateOppCurves();
		}
		else
		{
			if(m_PlrGraph == pGraph)
			{
				SetGraphTitles(m_PlrGraph);
			}
			else if(m_PlrGraph+1 == pGraph)
			{
				SetGraphTitles(m_PlrGraph+1);
			}
			else if(m_PlrGraph+2 == pGraph)
			{
				SetGraphTitles(m_PlrGraph+2);
			}
			else if(m_PlrGraph+3 == pGraph)
			{
				SetGraphTitles(m_PlrGraph+3);
			}
			else if(m_PlrGraph+4 == pGraph)
			{
				SetGraphTitles(m_PlrGraph+4);
			}
			if(m_pGraphDlg->m_bVariableChanged)
			{
				pGraph->SetAuto(true);
				pGraph->SetAutoYMinUnit(true);
			}
			CreatePolarCurves();
		}
	}
	else
	{
		pGraph->CopySettings(&graph);
	}
	UpdateView();
}


void QXDirect::OnHighlightOpp()
{
	if(!m_bPolarView) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	m_bHighlightOpp = !m_bHighlightOpp;

	pMainFrame->highlightOppAct->setChecked(m_bHighlightOpp);
	m_PlrGraph[0].m_bHighlightPoint = m_bHighlightOpp;
	m_PlrGraph[2].m_bHighlightPoint = m_bHighlightOpp;
	m_PlrGraph[1].m_bHighlightPoint = m_bHighlightOpp;
	m_PlrGraph[3].m_bHighlightPoint = m_bHighlightOpp;
	m_PlrGraph[4].m_bHighlightPoint = m_bHighlightOpp;

	CreatePolarCurves();
	UpdateView();
}


void QXDirect::OnHPlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_pCpGraph->SetYVariable(2);
	m_XFoilVar = 10;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle("Hk");
	Curve * pTopCurve = m_pCpGraph->AddCurve();
	Curve * pBotCurve = m_pCpGraph->AddCurve();
	pTopCurve->SetTitle(tr("Top"));
	pBotCurve->SetTitle(tr("Bot"));

	double x[IVX][3],y[IVX][3];
	int nside1, nside2;

	m_pXFoil->CreateXBL(x, nside1, nside2);
	m_pXFoil->FillHk(y, nside1, nside2);

	for (i=2; i<=nside1-1; i++)
	{
		pTopCurve->AppendPoint(x[i][1], y[i][1]);
	}
	for (i=2; i<=nside2-1; i++)
	{
		pBotCurve->AppendPoint(x[i][2], y[i][2]);
	}

	m_pCpGraph->SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}


void QXDirect::OnHideAllOpps()
{
	MainFrame * pMainFrame = (MainFrame*) s_pMainFrame;
	OpPoint *pOpp;
	for (int i=0; i<m_poaOpp->size(); i++)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		pOpp->m_bIsVisible = false;
	}
	pMainFrame->SetSaveState(false);
	if(!m_bPolarView) CreateOppCurves();
	SetAnalysisParams();
	UpdateView();
}


void QXDirect::OnHideAllPolars()
{
	MainFrame * pMainFrame = (MainFrame*) s_pMainFrame;
	Polar *pPolar;
	for (int i=0; i<m_poaPolar->size(); i++)
	{
		pPolar = (Polar*)m_poaPolar->at(i);
		pPolar->m_bIsVisible = false;
	}
	pMainFrame->SetSaveState(false);
	CreatePolarCurves();
	SetCurveParams();
	UpdateView();
}


void QXDirect::OnHideFoilPolars()
{
	if(!MainFrame::s_pCurFoil) return;
	MainFrame * pMainFrame = (MainFrame*) s_pMainFrame;
	Polar *pPolar;
	for (int i=0; i<m_poaPolar->size(); i++)
	{
		pPolar = (Polar*)m_poaPolar->at(i);
		if(pPolar->m_FoilName == MainFrame::s_pCurFoil->m_FoilName)
		{
			pPolar->m_bIsVisible = false;
		}
	}
	pMainFrame->SetSaveState(false);
	CreatePolarCurves();
	SetCurveParams();
	UpdateView();
}


void QXDirect::OnHideFoilOpps()
{
	if(!MainFrame::s_pCurFoil || !m_pCurPolar) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	OpPoint *pOpp;

	for(int i=0; i<m_poaOpp->size(); i++)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		if(pOpp->m_strFoilName==MainFrame::s_pCurFoil->m_FoilName)
			pOpp->m_bIsVisible = false;
	}
	pMainFrame->SetSaveState(false);
	if(!m_bPolarView) CreateOppCurves();
	SetCurveParams();
	UpdateView();
}



void QXDirect::OnHidePolarOpps()
{
	if(!MainFrame::s_pCurFoil || !m_pCurPolar) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	OpPoint *pOpp;

	for(int i=0; i<m_poaOpp->size(); i++)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		if(pOpp->m_strFoilName==MainFrame::s_pCurFoil->m_FoilName && pOpp->m_strPlrName==m_pCurPolar->m_PlrName)
			pOpp->m_bIsVisible = false;
	}
	pMainFrame->SetSaveState(false);
	if(!m_bPolarView) CreateOppCurves();
	SetCurveParams();
	UpdateView();
}


void QXDirect::OnImportXFoilPolar()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	Polar *pPolar = new Polar;
	double Re, alpha, CL, CD, CDp, CM, Xt, Xb,Cpmn, HMom;
	QString FoilName, PathName, strong, str;

	QByteArray textline;
	const char *text;

	PathName = QFileDialog::getOpenFileName(pMainFrame, tr("Open File"),
											pMainFrame->m_LastDirName,
											tr("XFoil Polar Format (*.*)"));
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
	bool bOK, bOK2, bRead;
	Line = 0;

	bRead  = ReadAVLString(in, Line, strong);// XFoil or XFLR5 version
	bRead  = ReadAVLString(in, Line, strong);// Foil Name

	FoilName = strong.right(strong.length()-22);
	FoilName = FoilName.trimmed();

	if(!pMainFrame->GetFoil(FoilName))
	{
		str = tr("No Foil with the name ")+FoilName;
		str+= tr("\ncould be found. The polar(s) will not be stored");
		delete pPolar;
		QMessageBox::warning(pMainFrame, tr("Warning"), str);
		return;
	}
	pPolar->m_FoilName = FoilName;

	bRead  = ReadAVLString(in, Line, strong);// analysis type

	pPolar->m_ReType = strong.mid(0,2).toInt(&bOK);
	pPolar->m_MaType= strong.mid(2,2).toInt(&bOK2);
	if(!bOK || !bOK2)
	{
		str = QString("Error reading line %1: Unrecognized Mach and Reynolds type.\nThe polar(s) will not be stored").arg(Line);
		delete pPolar;
		QMessageBox::warning(pMainFrame, tr("Warning"), str);
		return;
	}
	if     (pPolar->m_ReType ==1 && pPolar->m_MaType ==1) pPolar->m_PolarType = FIXEDSPEEDPOLAR;
	else if(pPolar->m_ReType ==2 && pPolar->m_MaType ==2) pPolar->m_PolarType = FIXEDLIFTPOLAR;
	else if(pPolar->m_ReType ==3 && pPolar->m_MaType ==1) pPolar->m_PolarType = RUBBERCHORDPOLAR;
	else                                                  pPolar->m_PolarType = FIXEDSPEEDPOLAR;


	bRead  = ReadAVLString(in, Line, strong);
	if(strong.length() < 34)
	{
		str = QString("Error reading line %1. The polar(s) will not be stored").arg(Line);
		delete pPolar;
		QMessageBox::warning(pMainFrame, tr("Warning"), str);
		return;
	}

	pPolar->m_XTop = strong.mid(9,6).toDouble(&bOK);
	if(!bOK)
	{
		str = QString("Error reading Bottom Transition value at line %1. The polar(s) will not be stored").arg(Line);
		delete pPolar;
		QMessageBox::warning(pMainFrame, tr("Warning"), str);
		return;

	}

	pPolar->m_XTop = strong.mid(28,6).toDouble(&bOK);
	if(!bOK)
	{
		str = QString("Error reading Top Transition value at line %1. The polar(s) will not be stored").arg(Line);
		delete pPolar;
		QMessageBox::warning(pMainFrame, tr("Warning"), str);
		return;
	}

	// Mach     Re     NCrit
	bRead  = ReadAVLString(in, Line, strong);// blank line
	if(strong.length() < 50)
	{
		str = QString("Error reading line %1. The polar(s) will not be stored").arg(Line);
		delete pPolar;
		QMessageBox::warning(pMainFrame, tr("Warning"), str);
		return;
	}

	pPolar->m_Mach = strong.mid(8,6).toDouble(&bOK);
	if(!bOK)
	{
		str = QString("Error reading Mach Number at line %1. The polar(s) will not be stored").arg(Line);
		delete pPolar;
		QMessageBox::warning(pMainFrame, tr("Warning"), str);
		return;
	}

	Re = strong.mid(24,10).toDouble(&bOK);
	if(!bOK)
	{
		str = QString("Error reading Reynolds Number at line %1. The polar(s) will not be stored").arg(Line);
		delete pPolar;
		QMessageBox::warning(pMainFrame, tr("Warning"), str);
		return;
	}
	Re *=1000000.0;

	pPolar->m_ACrit = strong.mid(52,8).toDouble(&bOK);
	if(!bOK)
	{
		str = QString("Error reading NCrit at line %1. The polar(s) will not be stored").arg(Line);
		delete pPolar;
		QMessageBox::warning(pMainFrame, tr("Warning"), str);
		return;
	}
	pPolar->m_Reynolds = Re;

	bRead  = ReadAVLString(in, Line, strong);// column titles
	bRead  = ReadAVLString(in, Line, strong);// underscores

	while( bRead)
	{
		bRead  = ReadAVLString(in, Line, strong);// polar data
		if(bRead)
		{
			if(strong.length())
			{
				textline = strong.toAscii();
				text = textline.constData();
				res = sscanf(text, "%lf%lf%lf%lf%lf%lf%lf%lf%lf", &alpha, &CL, &CD, &CDp, &CM, &Xt, &Xb, &Cpmn, &HMom);

				if (res == 7)
				{
					pPolar->AddPoint(alpha, CD, CDp, CL, CM, Xt, Xb, 0.0, 0.0,Re,0.0);
				}
				else if(res == 9)
				{
					pPolar->AddPoint(alpha, CD, CDp, CL, CM, Xt, Xb, Cpmn, HMom,Re,0.0);
				}
				else
				{
					bRead = false;
				}
			}
		}
	}

	Re = pPolar->m_Reynolds/1000000.0;
	pPolar->m_PlrName = QString("T%1_Re%2_M%3")
						.arg(pPolar->m_PolarType)
						.arg(Re,0,'f',2)
						.arg(pPolar->m_Mach,0,'f',2);
	str = QString("_N%1").arg(pPolar->m_ACrit,0,'f',1);
	pPolar->m_PlrName += str + "_Imported";

	pPolar->m_Color = pMainFrame->GetColor(1);

	m_pCurPolar = pMainFrame->AddPolar(pPolar);

	m_pCurOpp = NULL;
	SetPolar();
	pMainFrame->UpdatePolars();
	UpdateView();
	pMainFrame->SetSaveState(false);
}


void QXDirect::OnImportJavaFoilPolar()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString FileName, PolarName, FoilName;
	QString strong, str;

	QString PathName;
	bool bOK;
	QByteArray textline;
	const char *text;

	PathName = QFileDialog::getOpenFileName(pMainFrame, tr("Open File"),
											pMainFrame->m_LastDirName,
											tr("JavaFoil Polar Format (*.*)"));
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

	bool bIsReading = true;
	int res, Line;
	int NPolars = 0;
	double Re;

	double alpha, CL, CD, CM, Xt,  Xb;
	bool bRead;

	Line = 0;
	bRead  = ReadAVLString(in, Line, FoilName);

	FoilName = FoilName.trimmed();

	if(!pMainFrame->GetFoil(FoilName))
	{
		str = tr("No Foil with the name ")+FoilName;
		str+= tr("\ncould be found. The polar(s) will not be stored");
		QMessageBox::warning(pMainFrame, tr("Warning"), str);
		return;
	}
	bRead  = ReadAVLString(in, Line, strong);//blank line

	while(bIsReading)
	{
		bRead  = ReadAVLString(in, Line, strong);//Re number

		strong = strong.right(strong.length()-4);
		Re = strong.toDouble(&bOK);
		if(!bOK)
		{
			bIsReading = false;
		}
		else
		{
			Polar *pPolar = new Polar();
			pPolar->m_FoilName = FoilName;
			pPolar->m_Reynolds = Re;;
			pPolar->m_PlrName = QString("T%1_Re2_M3_JavaFoil")
								.arg(pPolar->m_PolarType)
								.arg(pPolar->m_Reynolds/1000000.0,0,'f',2)
								.arg(pPolar->m_Mach,0,'f',2);

			pPolar->m_Color = pMainFrame->GetColor(1);
			m_pCurPolar = pMainFrame->AddPolar(pPolar);
			NPolars++;

			bRead  = ReadAVLString(in, Line, strong);//?	Cl	Cd	Cm 0.25	TU	TL	SU	SL	L/D
			bRead  = ReadAVLString(in, Line, strong);//[?]	[-]	[-]	[-]	[-]	[-]	[-]	[-]	[-]

			res = 6;
			while(res==6)
			{
				bRead  = ReadAVLString(in, Line, strong);//values
				strong = strong.trimmed();
				if(strong.length())
				{
					strong.replace(',', '.');

					textline = strong.toAscii();
					text = textline.constData();

					res = sscanf(text, "%lf%lf%lf%lf%lf%lf",&alpha, &CL, &CD, &CM, &Xt, &Xb);
					if (res == 6) 	pPolar->AddPoint(alpha, CD, 0.0, CL, CM, Xt, Xb, 0.0, 0.0, Re,0.0);
				}
				else
				{
					res = 0;
				}
			}
		}
		m_pCurOpp = NULL;
		SetPolar();
		pMainFrame->UpdatePolars();
		UpdateView();
		pMainFrame->SetSaveState(false);
	}
}


void QXDirect::OnInitBL()
{
	if(!m_pXFoil) return;
	if (m_pctrlInitBL->isChecked())
	{
		m_pXFoil->lblini = false;
		m_pXFoil->lipan  = false;
	}
	else
	{
		m_pXFoil->lblini = true;
	}
}


void QXDirect::OnInterpolateFoils()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(m_poaFoil->size()<2)
	{
		QMessageBox::warning(pMainFrame, tr("Warning"), tr("At least two foils are required"));
		return;
	}

	StopAnimate();

	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	m_bPressure = false;
	m_bBL       = false;
	OnOpPoints();

	m_pIFDlg->m_pMainFrame = s_pMainFrame;
	m_pIFDlg->m_poaFoil     = m_poaFoil;
	m_pIFDlg->m_pBufferFoil = &m_BufferFoil;// work on the buffer foil
	m_pIFDlg->m_pXDirect = this;
	m_pIFDlg->m_pAFoil   = NULL;
	m_pIFDlg->InitDialog();

	if(m_pIFDlg->exec() == QDialog::Accepted)
	{
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilName = m_pIFDlg->m_NewFoilName;
		pMainFrame->SetModFoil(pNewFoil);
		pMainFrame->UpdateFoils();
	}
	else
	{
		SetBufferFoil();// restore buffer foil.. from current foil
		m_pXFoil->InitXFoilGeometry(MainFrame::s_pCurFoil);
	}

	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();
}


void QXDirect::OnManageFoils()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	m_pManageFoilsDlg->m_pMainFrame  = s_pMainFrame;

	QString FoilName = "";
	if(MainFrame::s_pCurFoil) FoilName = MainFrame::s_pCurFoil->m_FoilName;
	m_pManageFoilsDlg->InitDialog(FoilName);
	m_pManageFoilsDlg->exec();

	if(m_pManageFoilsDlg->m_pFoil) SetFoil(m_pManageFoilsDlg->m_pFoil);
	else                           SetFoil();

	pMainFrame->UpdateFoils();
	pMainFrame->SetSaveState(false);
	SetControls();
	UpdateView();
}


void QXDirect::OnNacaFoils()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StopAnimate();
	m_bPolarView   = false;

	void* ptr0 = MainFrame::s_pCurFoil;
	void* ptr  = m_pCurOpp;
	MainFrame::s_pCurFoil = NULL;
	m_pCurOpp = NULL;

	CreateOppCurves();
	UpdateView();

	m_pNacaFoilDlg->m_pBufferFoil = &m_BufferFoil;
	m_pNacaFoilDlg->m_pXDirect    = this;
	m_pNacaFoilDlg->m_pAFoil      = NULL;

	if (m_pNacaFoilDlg->exec() == QDialog::Accepted)
	{
		QString str;
		if(m_pNacaFoilDlg->s_Digits>0 && log10((double)m_pNacaFoilDlg->s_Digits)<4)
			str = QString("%1").arg(m_pNacaFoilDlg->s_Digits,4,10,QChar('0'));
		else
			str = QString("%1").arg(m_pNacaFoilDlg->s_Digits);
		str = "NACA "+ str;

		Foil *pNewFoil = new Foil;
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilName = str;
		m_pCurOpp = (OpPoint*)ptr;
		Foil *pOldFoil = pMainFrame->GetFoil(str);
		if(pOldFoil) pMainFrame->SetModFoil(pNewFoil);
		else         pMainFrame->AddFoil(pNewFoil);
		SetFoil(pNewFoil);
		pMainFrame->UpdateFoils();
		pMainFrame->SetSaveState(false);
	}
	else
	{
		MainFrame::s_pCurFoil = (Foil*)ptr0;
		m_pCurOpp  = (OpPoint*)ptr;
		SetBufferFoil();
		m_pXFoil->InitXFoilGeometry(MainFrame::s_pCurFoil);
	}
	SetControls();
	UpdateView();
}


void QXDirect::OnNormalizeFoil()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(!MainFrame::s_pCurFoil) return;
	QString str;
	StopAnimate();


	double length = MainFrame::s_pCurFoil->NormalizeGeometry();
	m_pXFoil->InitXFoilGeometry(MainFrame::s_pCurFoil);
	SetBufferFoil();
	str = QString(tr("The foil has been normalized from %1  to 1.000")).arg(length,7,'f',3);
	pMainFrame->SetSaveState(false);
	pMainFrame->statusBar()->showMessage(str);

	UpdateView();
}



void QXDirect::OnNPlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;
	int nside1, nside2, ibl;

	m_pCpGraph->SetYVariable(2);
	m_XFoilVar=6;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle("A/A0");
	Curve * pTopCurve = m_pCpGraph->AddCurve();
	Curve * pBotCurve = m_pCpGraph->AddCurve();
	pTopCurve->SetTitle(tr("Top"));
	pBotCurve->SetTitle(tr("Bot"));

	double x[IVX][3],y[IVX][3];

	m_pXFoil->CreateXBL(x, nside1, nside2);

	for (ibl=2; ibl< nside1;ibl++)
	{
		y[ibl][1] = m_pXFoil->ctau[ibl][1];
	}
	for ( ibl=2; ibl< nside2;ibl++)
	{
		y[ibl][2] = m_pXFoil->ctau[ibl][2];
	}

	for (i=2; i<=m_pXFoil->itran[1]-2; i++)
	{
		pTopCurve->AppendPoint(x[i][1], y[i][1]);
	}
	for (i=2; i<=m_pXFoil->itran[2]-2; i++)
	{
		pBotCurve->AppendPoint(x[i][2], y[i][2]);
	}
	m_pCpGraph->SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}



void QXDirect::OnOpPoints()
{
	if(!m_bPolarView) return;
//	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_bPolarView = false;
	CreateOppCurves();
	SetFoilScale();
	SetCurveParams();
	SetAnalysisParams();
	SetControls();
	UpdateView();
}


void QXDirect::OnPolars()
{
	if(m_bPolarView) return;
//	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	m_bPolarView = true;
	CreatePolarCurves();
	SetCurveParams();
	SetAnalysisParams();
	SetControls();
	UpdateView();
}


void QXDirect::OnPolarFilter()
{
	m_pPolarFilterDlg->m_bMiarex = false;
	m_pPolarFilterDlg->m_bType1 = m_bType1;
	m_pPolarFilterDlg->m_bType2 = m_bType2;
	m_pPolarFilterDlg->m_bType3 = m_bType3;
	m_pPolarFilterDlg->m_bType4 = m_bType4;
	m_pPolarFilterDlg->InitDialog();

	if(m_pPolarFilterDlg->exec()==QDialog::Accepted)
	{
		m_bType1 = m_pPolarFilterDlg->m_bType1;
		m_bType2 = m_pPolarFilterDlg->m_bType2;
		m_bType3 = m_pPolarFilterDlg->m_bType3;
		m_bType4 = m_pPolarFilterDlg->m_bType4;
		if(m_bPolarView)
		{
			CreatePolarCurves();
			UpdateView();
		}
	}
}


void QXDirect::OnPanels()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(!MainFrame::s_pCurFoil)	return;
	StopAnimate();

	bool bState = m_bShowPanels;//save current view setting

	void* ptr = m_pCurOpp;
	m_pCurOpp = NULL;
	CreateOppCurves();

	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	m_bPressure = false;
	m_bBL       = false;
	OnOpPoints();

	m_pTwoDPanelDlg->move(pMainFrame->m_DlgPos);
	m_pTwoDPanelDlg->m_pXDirect = this;
	m_pTwoDPanelDlg->m_pAFoil   = NULL;
	m_pTwoDPanelDlg->m_pBufferFoil = &m_BufferFoil;
	m_pTwoDPanelDlg->m_pMemFoil    = MainFrame::s_pCurFoil;

	m_bShowPanels = true;
	UpdateView();

	m_pTwoDPanelDlg->InitDialog();

	if(QDialog::Accepted == m_pTwoDPanelDlg->exec())
	{
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = true;
		m_pCurOpp = (OpPoint*)ptr;
		if(!pMainFrame->SetModFoil(pNewFoil))	SetBufferFoil();
		pMainFrame->UpdateFoils();
	}
	else
	{
		//reset everything
		m_pCurOpp = (OpPoint*)ptr;
		SetBufferFoil();
		m_pXFoil->InitXFoilGeometry(MainFrame::s_pCurFoil);
	}
	pMainFrame->m_DlgPos = m_pTwoDPanelDlg->pos();

	m_bShowPanels = bState;//restore as it was
	m_bPressure   = bPressure;
	m_bBL         = bBL;
	UpdateView();
}


void QXDirect::OnQGraph()
{
	m_bPolarView = false;
	if(m_pCpGraph->GetYVariable()!=1)
	{
		m_pCpGraph->ResetLimits();
		m_pCpGraph->SetAuto(true);
		m_pCpGraph->SetYVariable(1);
	}
	m_pCpGraph->SetXVariable(1);
	m_pCpGraph->SetInverted(false);
	CreateOppCurves();
	m_pCpGraph->SetYTitle(tr("Q"));

	SetControls();

	m_pCpGraph->SetXScale();
	SetFoilScale();
	UpdateView();
}


void QXDirect::OnRenamePolar()
{
	if(!m_pCurPolar) return;
	if(!MainFrame::s_pCurFoil) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	int resp, k,l;
	Polar* pPolar = NULL;
	OpPoint * pOpp;
	QString OldName = m_pCurPolar->m_PlrName;

	QStringList NameList;
	for(k=0; k<m_poaPolar->size(); k++)
	{
		pPolar = (Polar*)m_poaPolar->at(k);
		if(pPolar->m_FoilName == MainFrame::s_pCurFoil->m_FoilName)
			NameList.append(pPolar->m_PlrName);
	}
	m_pRenameDlg->move(pMainFrame->m_DlgPos);
	m_pRenameDlg->m_pstrArray = & NameList;
	m_pRenameDlg->m_strQuestion = tr("Enter the new name for the foil polar :");
	m_pRenameDlg->m_strName     = m_pCurPolar->m_PlrName;
	m_pRenameDlg->InitDialog();

	bool bExists = true;

	while (bExists)
	{
		resp = m_pRenameDlg->exec();
		pMainFrame->m_DlgPos = m_pRenameDlg->pos();
		if(resp==QDialog::Accepted)
		{
			if (OldName == m_pRenameDlg->m_strName) return;
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaPolar->size(); k++)
			{
				pPolar = (Polar*)m_poaPolar->at(k);
				if ((pPolar->m_FoilName==MainFrame::s_pCurFoil->m_FoilName) && (pPolar->m_PlrName == m_pRenameDlg->m_strName))
				{
					bExists = true;
					break;
				}
			}
			if(!bExists)
			{
				m_pCurPolar->m_PlrName = m_pRenameDlg->m_strName;
				for (l=(int)m_poaOpp->size()-1;l>=0; l--)
				{
					pOpp = (OpPoint*)m_poaOpp->at(l);
					if (pOpp->m_strPlrName == OldName &&
						pOpp->m_strFoilName == MainFrame::s_pCurFoil->m_FoilName)
					{
						pOpp->m_strPlrName = m_pRenameDlg->m_strName;
					}
				}
			}
			pMainFrame->SetSaveState(false);
		}
		else if(resp ==10)
		{//user wants to overwrite
			if (OldName == m_pRenameDlg->m_strName) return;
			for (k=0; k<m_poaPolar->size(); k++)
			{
				pPolar = (Polar*)m_poaPolar->at(k);
				if (pPolar->m_PlrName == m_pRenameDlg->m_strName)
				{
					bExists = true;
					break;
				}
			}
			for (l=m_poaOpp->size()-1;l>=0; l--)
			{
				pOpp = (OpPoint*)m_poaOpp->at(l);
				if (pOpp->m_strPlrName == pPolar->m_PlrName)
				{
					m_poaOpp->removeAt(l);
					if(pOpp==m_pCurOpp) m_pCurOpp = NULL;
					delete pOpp;
				}
			}
			m_poaPolar->removeAt(k);
			if(pPolar==m_pCurPolar) m_pCurPolar = NULL;
			delete pPolar;

			//and rename everything
			m_pCurPolar->m_PlrName = m_pRenameDlg->m_strName;

			for (l=m_poaOpp->size()-1;l>=0; l--)
			{
				pOpp = (OpPoint*)m_poaOpp->at(l);
				if (pOpp->m_strPlrName == OldName &&
					pOpp->m_strFoilName == MainFrame::s_pCurFoil->m_FoilName)
				{
					pOpp->m_strPlrName = m_pRenameDlg->m_strName;
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
	m_pCurPolar = NULL;
	m_pCurOpp   = NULL;
	SetPolar();
	pMainFrame->UpdatePolars();
	UpdateView();
}


void QXDirect::OnResetAllPolarGraphsScales()
{
	m_PlrGraph[0].SetAuto(true);
	m_PlrGraph[0].ResetXLimits();
	m_PlrGraph[0].ResetYLimits();

	m_PlrGraph[2].SetAuto(true);
	m_PlrGraph[2].ResetXLimits();
	m_PlrGraph[2].ResetYLimits();

	m_PlrGraph[1].SetAuto(true);
	m_PlrGraph[1].ResetXLimits();
	m_PlrGraph[1].ResetYLimits();

	m_PlrGraph[3].SetAuto(true);
	m_PlrGraph[3].ResetXLimits();
	m_PlrGraph[3].ResetYLimits();

	m_PlrGraph[4].SetAuto(true);
	m_PlrGraph[4].ResetXLimits();
	m_PlrGraph[4].ResetYLimits();

	UpdateView();
}


void QXDirect::OnResetFoilScale()
{
	SetFoilScale();
	if(!m_bAnimate) UpdateView();

}


void QXDirect::OnResetGraphLegend()
{
	SetPolarLegendPos();
	UpdateView();
}


void QXDirect::OnResetCurPolar()
{
	if(!m_pCurPolar) return;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	m_pCurPolar->ResetPolar();

	OpPoint*pOpp;
	for(int i=m_poaOpp->size()-1;i>=0;i--)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		if(pOpp->m_strFoilName==MainFrame::s_pCurFoil->m_FoilName && pOpp->m_strPlrName==m_pCurPolar->m_PlrName)
		{
			m_poaOpp->removeAt(i);
			delete pOpp;
		}
	}
	m_pCurOpp = NULL;

	pMainFrame->UpdateOpps();

	if(m_bPolarView) CreatePolarCurves();
	else         CreateOppCurves();
	UpdateView();
}


void QXDirect::OnResetXFoil()
{
	m_pXFoil->Initialize();
	m_pXFoil->lblini = false;
	m_pXFoil->lipan  = false;
	m_pXFoil->InitXFoilGeometry(MainFrame::s_pCurFoil);
	m_pXFoil->InitXFoilAnalysis(m_pCurPolar);
	m_bInitBL = true;
	m_pctrlInitBL->setChecked(m_bInitBL);
}



void QXDirect::OnRtPlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_pCpGraph->SetYVariable(2);
	m_XFoilVar=5;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle("Re_Theta");
	Curve * pTopCurve = m_pCpGraph->AddCurve();
	Curve * pBotCurve = m_pCpGraph->AddCurve();
	pTopCurve->SetTitle(tr("Top"));
	pBotCurve->SetTitle(tr("Bot"));

	double x[IVX][3],y[IVX][3];
	int nside1, nside2;

	m_pXFoil->CreateXBL(x, nside1, nside2);
	m_pXFoil->FillRTheta(y, nside1, nside2);

	for (i=2; i<=nside1-1; i++)	pTopCurve->AppendPoint(x[i][1], y[i][1]);
	for (i=2; i<=nside2-1; i++) pBotCurve->AppendPoint(x[i][2], y[i][2]);

	m_pCpGraph->SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}


void QXDirect::OnRtLPlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_pCpGraph->SetYVariable(2);
	m_XFoilVar=4;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle("Re_Theta");
	Curve * pTopCurve = m_pCpGraph->AddCurve();
	Curve * pBotCurve = m_pCpGraph->AddCurve();
	pTopCurve->SetTitle(tr("Top"));
	pBotCurve->SetTitle(tr("Bot"));

	double x[IVX][3],y[IVX][3];
	int nside1, nside2;

	m_pXFoil->CreateXBL(x, nside1, nside2);
	m_pXFoil->FillRTheta(y, nside1, nside2);

	for (i=2; i<=nside1-1; i++){
		if (y[i][1]>0.0) y[i][1] = log10( y[i][1] );
		else             y[i][1] = 0.0;
		pTopCurve->AppendPoint(x[i][1], y[i][1]);
	}
	for (i=2; i<=nside2-1; i++){
		if (y[i][2]>0.0) y[i][2] = log10( y[i][2] );
		else             y[i][2] = 0.0;
		pBotCurve->AppendPoint(x[i][2], y[i][2]);
	}
	m_pCpGraph->SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}


void QXDirect::OnSavePolars()
{
	if(!MainFrame::s_pCurFoil || !m_poaPolar->size()) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	QString FileName, FoilName;
	FileName = MainFrame::s_pCurFoil->m_FoilName + ".plr";
	FileName.replace("/", " ");

	FileName = QFileDialog::getSaveFileName(this, tr("Polar File"), pMainFrame->m_LastDirName+"/"+FileName, tr("Polar File (*.plr)"));
	if(!FileName.length()) return;

	QString strong = FileName.right(4);
	if(strong !=".plr" && strong !=".PLR") FileName += ".plr";

	QFile XFile(FileName);
	if (!XFile.open(QIODevice::WriteOnly)) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);

	QDataStream ar(&XFile);
#if QT_VERSION >= 0x040500
	ar.setVersion(QDataStream::Qt_4_5);
#endif
	ar.setByteOrder(QDataStream::LittleEndian);

	pMainFrame->WritePolars(ar, MainFrame::s_pCurFoil);

	XFile.close();
}


void QXDirect::OnSequence()
{
	m_bSequence = m_pctrlSequence->isChecked();
	SetOpPointSequence();
}


void QXDirect::OnSetFlap()
{
	if(!MainFrame::s_pCurFoil) return;
	StopAnimate();

	void* ptr = m_pCurOpp;
	m_pCurOpp = NULL;
	CreateOppCurves();
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	m_bPressure = false;
	m_bBL       = false;
	OnOpPoints();

	m_pFlapDlg->m_pBufferFoil  = &m_BufferFoil;
	m_pFlapDlg->m_pMemFoil     = MainFrame::s_pCurFoil;
	m_pFlapDlg->m_pXFoil       = m_pXFoil;
	m_pFlapDlg->m_pXDirect     = this;
	m_pFlapDlg->m_pAFoil       = NULL;
	m_pFlapDlg->InitDialog();

	if(QDialog::Accepted == m_pFlapDlg->exec())
	{
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		m_pCurOpp = (OpPoint*)ptr;
		if(!pMainFrame->SetModFoil(pNewFoil))	SetBufferFoil();
		pMainFrame->UpdateFoils();
	}
	else
	{
		//reset everything
		m_pCurOpp = (OpPoint*)ptr;
		SetBufferFoil();
		m_pXFoil->InitXFoilGeometry(MainFrame::s_pCurFoil);
	}

	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();
}


void QXDirect::OnSetLERadius()
{
	if(!MainFrame::s_pCurFoil)	return;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	StopAnimate();

	void* ptr = m_pCurOpp;
	m_pCurOpp = NULL;
	CreateOppCurves();
	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	m_bPressure = false;
	m_bBL       = false;
	OnOpPoints();

	m_pLEDlg->m_pBufferFoil = &m_BufferFoil;
	m_pLEDlg->m_pMemFoil    = MainFrame::s_pCurFoil;
	m_pLEDlg->m_pXDirect   = this;
	m_pLEDlg->m_pAFoil     = NULL;
	m_pLEDlg->InitDialog();

	if(QDialog::Accepted == m_pLEDlg->exec())
	{
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		m_pCurOpp = (OpPoint*)ptr;
		if(!pMainFrame->SetModFoil(pNewFoil))	SetBufferFoil();
		pMainFrame->UpdateFoils();
	}
	else
	{
		//reset everything
		m_pCurOpp = (OpPoint*)ptr;
		SetBufferFoil();
		m_pXFoil->InitXFoilGeometry(MainFrame::s_pCurFoil);
	}

	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();
}



void QXDirect::OnSetTEGap()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(!MainFrame::s_pCurFoil)	return;
	StopAnimate();

	void* ptr = m_pCurOpp;
	m_pCurOpp = NULL;
	CreateOppCurves();

	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	m_bPressure = false;
	m_bBL       = false;
	OnOpPoints();


	m_pTEGapDlg->m_pXDirect   = this;
	m_pTEGapDlg->m_pAFoil     = NULL;
	m_pTEGapDlg->m_pBufferFoil = &m_BufferFoil;
	m_pTEGapDlg->m_pMemFoil    = MainFrame::s_pCurFoil;
	m_pTEGapDlg->m_Gap         = MainFrame::s_pCurFoil->m_Gap;
	m_pTEGapDlg->InitDialog();

	if(QDialog::Accepted == m_pTEGapDlg->exec())
	{
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		m_pCurOpp = (OpPoint*)ptr;
		if(!pMainFrame->SetModFoil(pNewFoil))	SetBufferFoil();
		pMainFrame->UpdateFoils();
	}

	else
	{
		//reset everything
		m_pCurOpp = (OpPoint*)ptr;
		SetBufferFoil();
		m_pXFoil->InitXFoilGeometry(MainFrame::s_pCurFoil);
	}
	pMainFrame->m_DlgPos = m_pTEGapDlg->pos();

	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();

}



void QXDirect::OnShowAllOpps()
{
	OpPoint *pOpp;

	m_bCurOppOnly = false;
	((MainFrame*)s_pMainFrame)->showCurOppOnly->setChecked(m_bCurOppOnly);

	for (int i=0; i<m_poaOpp->size(); i++)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		pOpp->m_bIsVisible = true;
	}
	((MainFrame*)s_pMainFrame)->SetSaveState(false);
	if(!m_bPolarView) CreateOppCurves();
	SetCurveParams();
	UpdateView();
}



void QXDirect::OnShowAllPolars()
{
	MainFrame * pMainFrame = (MainFrame*) s_pMainFrame;
	Polar *pPolar;
	for (int i=0; i<m_poaPolar->size(); i++)
	{
		pPolar = (Polar*)m_poaPolar->at(i);
		pPolar->m_bIsVisible = true;
	}
	pMainFrame->SetSaveState(false);
	CreatePolarCurves();
	SetCurveParams();
	UpdateView();
}



void QXDirect::OnShowBL()
{
	if(m_pctrlShowBL->isChecked())
	{
		if(m_bPolarView) OnOpPoints();
		m_pctrlShowPressure->setChecked(false);
		m_bBL = true;
		m_bPressure = false;
	}
	else
	{
		m_bBL = false;
	}
	if(!m_bAnimate)	UpdateView();
}


void QXDirect::OnShowCurve()
{
	//user has toggled visible switch
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(m_bPolarView)
	{
		if (m_pCurPolar)
		{
			m_pCurPolar->m_bIsVisible = m_pctrlShowCurve->isChecked();

		}
		CreatePolarCurves();
	}
	else if (m_pCurOpp)
	{
		m_pCurOpp->m_bIsVisible = m_pctrlShowCurve->isChecked();
		CreateOppCurves();
	}
	pMainFrame->SetSaveState(false);
	UpdateView();
}


void QXDirect::OnShowFoilPolarsOnly()
{
	if(!MainFrame::s_pCurFoil) return;
	MainFrame * pMainFrame = (MainFrame*) s_pMainFrame;
	Polar *pPolar;
	for (int i=0; i<m_poaPolar->size(); i++)
	{
		pPolar = (Polar*)m_poaPolar->at(i);
		pPolar->m_bIsVisible = (pPolar->m_FoilName == MainFrame::s_pCurFoil->m_FoilName);
	}
	pMainFrame->SetSaveState(false);
	CreatePolarCurves();
	SetCurveParams();
	UpdateView();
}




void QXDirect::OnShowFoilPolars()
{
	if(!MainFrame::s_pCurFoil) return;
	MainFrame * pMainFrame = (MainFrame*) s_pMainFrame;
	Polar *pPolar;
	for (int i=0; i<m_poaPolar->size(); i++)
	{
		pPolar = (Polar*)m_poaPolar->at(i);
		if(pPolar->m_FoilName == MainFrame::s_pCurFoil->m_FoilName)
		{
			pPolar->m_bIsVisible = true;
		}
	}
	pMainFrame->SetSaveState(false);
	CreatePolarCurves();
	SetCurveParams();
	UpdateView();
}



void QXDirect::OnShowFoilOpps()
{
	if(!MainFrame::s_pCurFoil || !m_pCurPolar) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	OpPoint *pOpp;

	m_bCurOppOnly = false;
	((MainFrame*)s_pMainFrame)->showCurOppOnly->setChecked(m_bCurOppOnly);

	for(int i=0; i<m_poaOpp->size(); i++)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		if(pOpp->m_strFoilName==MainFrame::s_pCurFoil->m_FoilName)
			pOpp->m_bIsVisible = true;
	}
	pMainFrame->SetSaveState(false);
	if(!m_bPolarView) CreateOppCurves();
	SetCurveParams();

	UpdateView();
}



void QXDirect::OnShowNeutralLine()
{
	m_bNeutralLine = !m_bNeutralLine;
	if(!m_bPolarView) UpdateView();
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->showNeutralLine->setChecked(m_bNeutralLine);
}


void QXDirect::OnShowPanels()
{
	m_bShowPanels = !m_bShowPanels;
	if(!m_bPolarView)	UpdateView();
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->showPanels->setChecked(m_bShowPanels);
}


void QXDirect::OnShowPoints()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(m_bPolarView)
	{
		if (m_pCurPolar)
		{ 
			m_pCurPolar->m_bShowPoints = m_pctrlShowPoints->isChecked();
		}
		CreatePolarCurves(); 
	}
	else if (m_pCurOpp)
	{
		m_pCurOpp->m_bShowPoints = m_pctrlShowPoints->isChecked();
		CreateOppCurves(); 
	}
	
	pMainFrame->SetSaveState(false);
	UpdateView();	
}



void QXDirect::OnShowPolarOpps()
{
	if(!MainFrame::s_pCurFoil || !m_pCurPolar) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	OpPoint *pOpp;

	m_bCurOppOnly = false;
	((MainFrame*)s_pMainFrame)->showCurOppOnly->setChecked(m_bCurOppOnly);

	for(int i=0; i<m_poaOpp->size(); i++)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		if(pOpp->m_strFoilName==MainFrame::s_pCurFoil->m_FoilName && pOpp->m_strPlrName==m_pCurPolar->m_PlrName)
			pOpp->m_bIsVisible = true;
	}
	pMainFrame->SetSaveState(false);
	if(!m_bPolarView) CreateOppCurves();
	SetCurveParams();
	UpdateView();
}



void QXDirect::OnShowPressure()
{
	if(m_pctrlShowPressure->isChecked())
	{
		if(m_bPolarView) OnOpPoints();
		m_pctrlShowBL->setChecked(false);
		m_bBL = false;
		m_bPressure = true;
	}
	else
	{
		m_bPressure = false;
	}
	if(!m_bAnimate)		UpdateView();
}



void QXDirect::OnSinglePolarGraph()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action) return;

	m_iPlrView  = ONEPOLARGRAPH;
	m_iPlrGraph = action->data().toInt();
	m_bPolarView = true;
	SetPolarLegendPos();
	SetControls();
	UpdateView();
}



void QXDirect::OnSpec()
{
	if      (m_pctrlSpec1->isChecked()) m_bAlpha = true;
	else if (m_pctrlSpec2->isChecked()) m_bAlpha = false;
	else if (m_pctrlSpec3->isChecked()) m_bAlpha = false;
}



void QXDirect::OnStoreOpp()
{
	m_bStoreOpp = m_pctrlStoreOpp->isChecked();
}



void QXDirect::OnUePlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;
	double x[IVX][3],y[IVX][3];
	double uei;
	int nside1, nside2, ibl;

	m_pCpGraph->SetYVariable(2);
	m_XFoilVar = 9;
	m_pCpGraph->DeleteCurves();
	m_pCpGraph->ResetLimits();
	m_pCpGraph->SetAuto(true);
	m_pCpGraph->SetInverted(false);
	m_pCpGraph->SetYTitle("Ue/Vinf");
	Curve * pTopCurve = m_pCpGraph->AddCurve();
	Curve * pBotCurve = m_pCpGraph->AddCurve();
	pTopCurve->SetTitle(tr("Top"));
	pBotCurve->SetTitle(tr("Bot"));

	m_pXFoil->CreateXBL(x, nside1, nside2);
	//---- fill compressible ue arrays
	for (ibl=2; ibl<= nside1;ibl++)
	{
		uei = m_pXFoil->uedg[ibl][1];
		y[ibl][1] = uei * (1.0-m_pXFoil->tklam)
						/ (1.0-m_pXFoil->tklam*(uei/m_pXFoil->qinf)*(uei/m_pXFoil->qinf));
	}
	for (ibl=2; ibl<= nside2;ibl++)
	{
		uei = m_pXFoil->uedg[ibl][2];
		y[ibl][2] = uei * (1.0-m_pXFoil->tklam)
						/ (1.0-m_pXFoil->tklam*(uei/m_pXFoil->qinf)*(uei/m_pXFoil->qinf));
	}

	for (i=2; i<=nside1-1; i++)
	{
		pTopCurve->AppendPoint(x[i][1], y[i][1]);
	}
	for (i=2; i<=nside2-1; i++)
	{
		pBotCurve->AppendPoint(x[i][2], y[i][2]);
	}
	m_pCpGraph->SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}


void QXDirect::OnViscous()
{
	m_bViscous = m_pctrlViscous->isChecked();
}


void QXDirect::OnXDirectStyle()
{
	m_pXDirectStyleDlg->m_pXDirect = this;
	m_pXDirectStyleDlg->m_iBLStyle         = m_iBLStyle;
	m_pXDirectStyleDlg->m_iBLWidth         = m_iBLWidth;
	m_pXDirectStyleDlg->m_crBLColor        = m_crBLColor;
	m_pXDirectStyleDlg->m_iPressureStyle   = m_iPressureStyle;
	m_pXDirectStyleDlg->m_iPressureWidth   = m_iPressureWidth;
	m_pXDirectStyleDlg->m_crPressureColor  = m_crPressureColor;
	m_pXDirectStyleDlg->m_iNeutralStyle    = m_iNeutralStyle;
	m_pXDirectStyleDlg->m_iNeutralWidth    = m_iNeutralWidth;
	m_pXDirectStyleDlg->m_crNeutralColor   = m_crNeutralColor;
	m_pXDirectStyleDlg->InitDialog();
	if(m_pXDirectStyleDlg->exec() == QDialog::Accepted)
	{
		m_iBLStyle         = m_pXDirectStyleDlg->m_iBLStyle;
		m_iBLWidth         = m_pXDirectStyleDlg->m_iBLWidth;
		m_crBLColor        = m_pXDirectStyleDlg->m_crBLColor;
		m_iPressureStyle   = m_pXDirectStyleDlg->m_iPressureStyle;
		m_iPressureWidth   = m_pXDirectStyleDlg->m_iPressureWidth;
		m_crPressureColor  = m_pXDirectStyleDlg->m_crPressureColor;
		m_iNeutralStyle    = m_pXDirectStyleDlg->m_iNeutralStyle;
		m_iNeutralWidth    = m_pXDirectStyleDlg->m_iNeutralWidth;
		m_crNeutralColor   = m_pXDirectStyleDlg->m_crNeutralColor;
	}
	UpdateView();
}



void QXDirect::OnXFoilAdvanced()
{
	m_pXFoilAdvancedDlg->m_IterLimit = m_IterLim;
	m_pXFoilAdvancedDlg->m_VAccel  = m_pXFoil->vaccel;
	m_pXFoilAdvancedDlg->m_bInitBL = m_bAutoInitBL;
	m_pXFoilAdvancedDlg->m_bFullReport    = m_pXFoil->m_bFullReport;
	m_pXFoilAdvancedDlg->InitDialog();

	if (QDialog::Accepted == m_pXFoilAdvancedDlg->exec())
	{
		m_pXFoil->vaccel = m_pXFoilAdvancedDlg->m_VAccel;
		m_IterLim        = m_pXFoilAdvancedDlg->m_IterLimit;
		m_bAutoInitBL    = m_pXFoilAdvancedDlg->m_bInitBL;
		m_pXFoil->m_bFullReport    = m_pXFoilAdvancedDlg->m_bFullReport;
	}
}


void QXDirect::PaintBL(QPainter &painter, OpPoint* pOpPoint, double scale)
{
	QPoint offset, From, To;
	double x,y;
	int i;
	double alpha = -pOpPoint->Alpha*PI/180.0;
	double cosa = cos(alpha);
	double sina = sin(alpha);

	if(!pOpPoint->m_bViscResults || !pOpPoint->m_bBL) return;

	offset = m_FoilOffset;
	scale  = scale;
	scale  = scale;

	QPen WakePen(m_crBLColor);
	WakePen.setStyle(GetStyle(m_iBLStyle));
	WakePen.setWidth(m_iBLWidth);

	painter.setPen(WakePen);

	x = (pOpPoint->xd1[1]-0.5)*cosa - pOpPoint->yd1[1]*sina + 0.5;
	y = (pOpPoint->xd1[1]-0.5)*sina + pOpPoint->yd1[1]*cosa;
	From.rx() = (int)( x*scale) + offset.x();
	From.ry() = (int)(-y*scale) + offset.y();
	for (i=2; i<=pOpPoint->nd1; i++)
	{
		x = (pOpPoint->xd1[i]-0.5)*cosa - pOpPoint->yd1[i]*sina + 0.5;
		y = (pOpPoint->xd1[i]-0.5)*sina + pOpPoint->yd1[i]*cosa;
		To.rx() = (int)( x*scale) + offset.x();
		To.ry() = (int)(-y*scale) + offset.y();
		painter.drawLine(From, To);
		From = To;
	}

	x = (pOpPoint->xd2[0]-0.5)*cosa - pOpPoint->yd2[0]*sina + 0.5;
	y = (pOpPoint->xd2[0]-0.5)*sina + pOpPoint->yd2[0]*cosa;
	From.rx() = (int)( x*scale) + offset.x();
	From.ry() = (int)(-y*scale) + offset.y();
	for (i=1; i<pOpPoint->nd2; i++)
	{
		x = (pOpPoint->xd2[i]-0.5)*cosa - pOpPoint->yd2[i]*sina + 0.5;
		y = (pOpPoint->xd2[i]-0.5)*sina + pOpPoint->yd2[i]*cosa;
		To.rx() = (int)( x*scale) + offset.x();
		To.ry() = (int)(-y*scale) + offset.y();
		painter.drawLine(From, To);
		From = To;
	}

	x = (pOpPoint->xd3[0]-0.5)*cosa - pOpPoint->yd3[0]*sina + 0.5;
	y = (pOpPoint->xd3[0]-0.5)*sina + pOpPoint->yd3[0]*cosa;
	From.rx() = (int)( x*scale) + offset.x();
	From.ry() = (int)(-y*scale) + offset.y();
	for (i=1; i<pOpPoint->nd3; i++)
	{
		x = (pOpPoint->xd3[i]-0.5)*cosa - pOpPoint->yd3[i]*sina + 0.5;
		y = (pOpPoint->xd3[i]-0.5)*sina + pOpPoint->yd3[i]*cosa;
		To.rx() = (int)( x*scale) + offset.x();
		To.ry() = (int)(-y*scale) + offset.y();
		painter.drawLine(From, To);
		From = To;
	}
}


void QXDirect::PaintPressure(QPainter &painter, OpPoint* pOpPoint, double scale)
{
	if(!pOpPoint->m_bViscResults ) return;

	int i;
	double alpha = -pOpPoint->Alpha*PI/180.0;
	double cosa = cos(alpha);
	double sina = sin(alpha);
	double x, y ,xs, ys, xe, ye, dx, dy, x1, x2, y1, y2, r2;
	double cp;
	QPoint offset = m_FoilOffset;

	QPen CpvPen(m_crPressureColor);
	CpvPen.setStyle(GetStyle(m_iPressureStyle));
	CpvPen.setWidth(m_iPressureWidth);
	painter.setPen(CpvPen);


	for(i=0; i<m_BufferFoil.n; i++)
	{
		if(pOpPoint->m_bViscResults)	cp = pOpPoint->Cpv[i];
		else					cp = pOpPoint->Cpi[i];
		x = m_BufferFoil.x[i];
		y = m_BufferFoil.y[i];

		xs = (x-0.5)*cosa - y*sina + 0.5;
		ys = (x-0.5)*sina + y*cosa;

		if(cp>0)
		{
			x += m_BufferFoil.nx[i] * cp * 0.05;
			y += m_BufferFoil.ny[i] * cp * 0.05;

			xe = (x-0.5)*cosa - y*sina + 0.5;
			ye = (x-0.5)*sina + y*cosa;
			painter.drawLine((int)( xs*scale) + offset.x(), (int)(-ys*scale) + offset.y(),
							 (int)( xe*scale) + offset.x(), (int)(-ye*scale) + offset.y());

			dx = xe - xs;
			dy = ye - ys;
			r2 = sqrt(dx*dx + dy*dy);
			if(r2!=0.0) //you can never be sure...
			{
				dx = dx/r2;
				dy = dy/r2;
			}

			x1 = xs + 0.0085*dx + 0.005*dy;
			y1 = ys + 0.0085*dy - 0.005*dx;
			x2 = xs + 0.0085*dx - 0.005*dy;
			y2 = ys + 0.0085*dy + 0.005*dx;

			painter.drawLine((int)( xs*scale) + offset.x(), (int)(-ys*scale) + offset.y(),
							 (int)( x1*scale) + offset.x(), (int)(-y1*scale) + offset.y());

			painter.drawLine((int)( xs*scale) + offset.x(), (int)(-ys*scale) + offset.y(),
							 (int)( x2*scale) + offset.x(), (int)(-y2*scale) + offset.y());
		}
		else
		{

			x += -m_BufferFoil.nx[i] * cp *0.05;
			y += -m_BufferFoil.ny[i] * cp *0.05;

			xe = (x-0.5)*cosa - y*sina+ 0.5;
			ye = (x-0.5)*sina + y*cosa;
			painter.drawLine((int)( xs*scale) + offset.x(), (int)(-ys*scale) + offset.y(),
							 (int)( xe*scale) + offset.x(), (int)(-ye*scale) + offset.y());

			dx = xe - xs;
			dy = ye - ys;
			r2 = sqrt(dx*dx + dy*dy);
			if(r2!=0.0) //you can never be sure...
			{
				dx = -dx/r2;
				dy = -dy/r2;
			}

			x1 = xe + 0.0085*dx + 0.005*dy;
			y1 = ye + 0.0085*dy - 0.005*dx;
			x2 = xe + 0.0085*dx - 0.005*dy;
			y2 = ye + 0.0085*dy + 0.005*dx;

			painter.drawLine((int)( xe*scale) + offset.x(), (int)(-ye*scale) + offset.y(),
							 (int)( x1*scale) + offset.x(), (int)(-y1*scale) + offset.y());

			painter.drawLine((int)( xe*scale) + offset.x(), (int)(-ye*scale) + offset.y(),
							 (int)( x2*scale) + offset.x(), (int)(-y2*scale) + offset.y());
		}
	}
	//last draw lift at XCP position
	QPen LiftPen(m_crPressureColor);
	LiftPen.setStyle(GetStyle(m_iPressureStyle));
	LiftPen.setWidth(m_iPressureWidth+1);
	painter.setPen(LiftPen);

	xs =  (pOpPoint->m_XCP-0.5)*cosa  + 0.5;
	ys = -(pOpPoint->m_XCP-0.5)*sina ;

	xe = xs;
	ye = ys - pOpPoint->Cl/10.0;

	painter.drawLine((int)( xs*scale) + offset.x(), (int)(ys*scale) + offset.y(),
					 (int)( xs*scale) + offset.x(), (int)(ye*scale) + offset.y());

	dx = xe - xs;
	dy = ye - ys;
	r2 = sqrt(dx*dx + dy*dy);
	dx = -dx/r2;
	dy = -dy/r2;

	x1 = xe + 0.0085*dx + 0.005*dy;
	y1 = ye + 0.0085*dy - 0.005*dx;
	x2 = xe + 0.0085*dx - 0.005*dy;
	y2 = ye + 0.0085*dy + 0.005*dx;

	painter.drawLine((int)( xe*scale) + offset.x(), (int)(ye*scale) + offset.y(),
					 (int)( x1*scale) + offset.x(), (int)(y1*scale) + offset.y());

	painter.drawLine((int)( xe*scale) + offset.x(), (int)(ye*scale) + offset.y(),
					 (int)( x2*scale) + offset.x(), (int)(y2*scale) + offset.y());
}


void QXDirect::PaintCoupleGraphs(QPainter &painter)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	int h  = m_rCltRect.height();
	int w  = m_rCltRect.width();
	int w2 = (int)(w/2);
	int h23 = (int)(2*h/3);

	QRect Rect1(0,0,w2,h23);
	QRect Rect2(w2,0,w2,h23);
	QRect Rect3(0, h23, w,h-h23);
	painter.fillRect(Rect3, pMainFrame->m_BackgroundColor);

	m_PlrGraph[0].DrawGraph(Rect1, painter);
	m_PlrGraph[4].DrawGraph(Rect2, painter);

	PaintPolarLegend(m_PolarLegendOffset, h, painter);
}


void QXDirect::PaintOpPoint(QPainter &painter)
{
	static double Alpha, FoilScale;
	FoilScale = m_fFoilScale;
	QString Result, str, str1;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	painter.fillRect(m_rCltRect, pMainFrame->m_BackgroundColor);

	if (m_rCltRect.width()<150 || m_rCltRect.height()<150) return;


	if(m_bCpGraph)
	{
		//Draw Cp Graph
		m_pCpGraph->DrawGraph(painter);
		QPoint Place(m_rCltRect.right()/2, m_rCltRect.top() + 20);
		m_pCpGraph->DrawLegend(painter, Place, pMainFrame->m_TextFont, pMainFrame->m_TextColor);//Graph::DrawLegend uses graph's legend font and color
	}

	if(m_bNeutralLine)
	{
		QPen NeutralPen(m_crNeutralColor);
		NeutralPen.setStyle(GetStyle(m_iNeutralStyle));
		NeutralPen.setWidth(m_iNeutralWidth);
		painter.setPen(NeutralPen);
		painter.drawLine(m_rCltRect.left(),m_FoilOffset.y(),
						 m_rCltRect.right(),m_FoilOffset.y());
	}

	if (!MainFrame::s_pCurFoil || !MainFrame::s_pCurFoil->m_FoilName.length())
		return;

	if(MainFrame::s_pCurFoil)
	{
		Alpha = 0.0;
		if(m_pCurOpp) Alpha = m_pCurOpp->Alpha;

		m_BufferFoil.m_bPoints = m_bShowPanels;
		m_BufferFoil.DrawFoil(painter, -Alpha, m_fFoilScale, FoilScale, m_FoilOffset);

		if(m_bPressure && m_pCurOpp) PaintPressure(painter, m_pCurOpp, m_fFoilScale);
		if(m_bBL && m_pCurOpp)       PaintBL(painter, m_pCurOpp, m_fFoilScale);
	}


	// Write Titles and results
	QString strong;

	painter.setFont(pMainFrame->m_TextFont);
	int D = 0;
	int ZPos = m_rCltRect.bottom();
	int XPos = m_rCltRect.right()-10;
	QPen WritePen(pMainFrame->m_TextColor);
	painter.setPen(WritePen);

	QFontMetrics fm(pMainFrame->m_TextFont);
	int dD = fm.height();

	//write the foil's properties

	int Back = 4;

	if(m_BufferFoil.m_bTEFlap) Back +=3;

	int LeftPos = m_rCltRect.left()+10;
	ZPos = m_rCltRect.bottom() - 10 - Back*dD;

	D = 0;
	str = "%";
	str1 = QString(tr("Thickness         = %1")).arg(m_BufferFoil.m_fThickness*100.0, 6, 'f', 2);
	painter.drawText(LeftPos,ZPos+D, str1+str);
	D += dD;

	str1 = QString(tr("Max. Thick.pos.   = %1")).arg(m_BufferFoil.m_fXThickness*100.0, 6, 'f', 2);
	painter.drawText(LeftPos,ZPos+D, str1+str);
	D += dD;

	str1 = QString(tr("Max. Camber       = %1")).arg( m_BufferFoil.m_fCamber*100.0, 6, 'f', 2);
	painter.drawText(LeftPos,ZPos+D, str1+str);
	D += dD;

	str1 = QString(tr("Max. Camber pos.  = %1")).arg(m_BufferFoil.m_fXCamber*100.0, 6, 'f', 2);
	painter.drawText(LeftPos,ZPos+D, str1+str);
	D += dD;

	str1 = QString(tr("Number of Panels  =  %1")).arg( m_BufferFoil.n);
	painter.drawText(LeftPos,ZPos+D, str1);
	D += dD;

	if(m_BufferFoil.m_bTEFlap)
	{
		str1 = QString(tr("Flap Angle = %1")+QString::fromUtf8("°")).arg( m_BufferFoil.m_TEFlapAngle, 7, 'f', 2);
		painter.drawText(LeftPos,ZPos+D, str1);
		D += dD;

		str1 = QString(tr("XHinge     = %1")).arg( m_BufferFoil.m_TEXHinge, 6, 'f', 1);
		strong="%";
		painter.drawText(LeftPos,ZPos+D, str1+strong);
		D += dD;

		str1 = QString(tr("YHinge     = %1")).arg( m_BufferFoil.m_TEYHinge, 6, 'f', 1);
		strong="%";
		painter.drawText(LeftPos,ZPos+D, str1+strong);
		D += dD;
	}


	D = 0;
	Back = 6;

	if(m_pCurOpp)
	{
		Back = 12;
		if(m_pCurOpp->m_bTEFlap) Back++;
		if(m_pCurOpp->m_bLEFlap) Back++;
		if(m_pCurOpp->m_bViscResults && fabs(m_pCurOpp->Cd)>0.0) Back++;
		if(m_pCurPolar->m_PolarType==FIXEDLIFTPOLAR) Back++;
		if(m_pCurPolar->m_PolarType!=FIXEDSPEEDPOLAR && m_pCurPolar->m_PolarType!=FIXEDAOAPOLAR) Back++;
	}

	int dwidth = fm.width(tr("TE Hinge Moment/span = 123456789"));

	ZPos = m_rCltRect.bottom()-Back*dD - 10;
	XPos = m_rCltRect.right()-dwidth-20;
	D=0;


	if(m_pCurPolar)
	{
		str1 = QString(tr("Polar Type =         %1")).arg( m_pCurPolar->m_PolarType);
		painter.drawText(XPos,ZPos, dwidth, dD, Qt::AlignRight | Qt::AlignTop, str1);
		D += dD;
		if(m_pCurPolar->m_PolarType ==1)
		{
			ReynoldsFormat(strong, m_pCurPolar->m_Reynolds );
			strong ="Reynolds = " + strong;
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
			D += dD;
			strong = QString("Mach = %1").arg( m_pCurPolar->m_Mach,9,'f',3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
			D += dD;
		}
		if(m_pCurPolar->m_PolarType==FIXEDLIFTPOLAR)
		{
			ReynoldsFormat(strong, m_pCurPolar->m_Reynolds );
			strong = tr("Re.sqrt(Cl) = ") + strong;
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
			D += dD;

			strong = QString(tr("M.sqrt(Cl) = %1")).arg(m_pCurPolar->m_Mach,9,'f',3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
			D += dD;
		}
		if(m_pCurPolar->m_PolarType==RUBBERCHORDPOLAR)
		{
			ReynoldsFormat(strong, m_pCurPolar->m_Reynolds );
			strong = tr("Re.sqrt(Cl) = ") + strong;
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
			D += dD;

			strong = QString("Mach = %1").arg(m_pCurPolar->m_Mach,9,'f',3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
			D += dD;
		}
		if(m_pCurPolar->m_PolarType==FIXEDAOAPOLAR)
		{
			strong = QString("Alpha = %1 "+QString::fromUtf8("°")).arg(m_pCurPolar->m_ASpec,10,'f',2);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
			D += dD;
			strong = QString("Mach = %1").arg(m_pCurPolar->m_Mach,9,'f',3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
			D += dD;
		}

		strong = QString("NCrit = %1").arg(m_pCurPolar->m_ACrit,9,'f',3);
		painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
		D += dD;

		strong = QString(tr("Forced Upper Trans. = %1")).arg(m_pCurPolar->m_XTop,9,'f',3);
		painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
		D += dD;
		strong = QString(tr("Forced Lower Trans. = %1")).arg(m_pCurPolar->m_XBot, 9, 'f', 3);
		painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
		D += dD;

		if(m_pCurOpp)
		{
			if(m_pCurPolar->m_PolarType!=FIXEDSPEEDPOLAR)
			{
				ReynoldsFormat(Result, m_pCurOpp->Reynolds);
				Result = "Re = "+ Result;
				painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
				D += dD;
			}
			if(m_pCurPolar->m_PolarType==FIXEDLIFTPOLAR)
			{
				Result = QString("Ma = %1").arg(m_pCurOpp->Mach, 9, 'f', 4);
				painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
				D += dD;
			}
			if(m_pCurPolar->m_PolarType!=FIXEDAOAPOLAR)
			{
				Result = QString(tr("       Alpha = %1 ")+QString::fromUtf8("°")).arg(m_pCurOpp->Alpha, 7, 'f', 2);
				painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
				D += dD;
			}
			Result = QString(tr("          Cl = %1")).arg(m_pCurOpp->Cl, 9, 'f', 3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
			D += dD;

			Result = QString(tr("          Cm = %1")).arg(m_pCurOpp->Cm, 9, 'f', 3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
			D += dD;

			Result = QString(tr("          Cd = %1")).arg(m_pCurOpp->Cd, 9, 'f', 3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
			D += dD;

			if(m_pCurOpp->m_bViscResults && fabs(m_pCurOpp->Cd)>0.0)
			{
				Result = QString(tr("         L/D = %1")).arg(m_pCurOpp->Cl/m_pCurOpp->Cd, 9, 'f', 3);
				painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
				D += dD;
			}

			Result = QString(tr("Upper Trans. = %1")).arg(m_pCurOpp->Xtr1, 9, 'f', 3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
			D += dD;

			Result = QString(tr("Lower Trans. = %1")).arg(m_pCurOpp->Xtr2, 9, 'f', 3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
			D += dD;

			if(m_pCurOpp->m_bTEFlap)
			{
				Result = QString(tr("TE Hinge Moment/span = %1")).arg(m_pCurOpp->m_TEHMom, 9, 'e', 3);
				painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
				D += dD;
			}

			if(m_pCurOpp->m_bLEFlap)
			{
				Result = QString(tr("LE Hinge Moment/span = %1")).arg(m_pCurOpp->m_LEHMom, 9, 'e', 3);
				painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
				D += dD;
			}
		}
	}
}



void QXDirect::PaintPolarGraphs(QPainter &painter)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int h  = m_rCltRect.height();
	int w  = m_rCltRect.width();
	int h2 = (int)(h/2);
	int w2 = (int)(w/2);
	int w4 = (int)(w/4);

	QRect Rect1(0,0,w2,h2);
	QRect Rect2(w2,0,w4,h2);
	QRect Rect3(w2, h2, w4, h2);
	QRect Rect4(3*w4,0,w4,h2);
	QRect Rect5(0,h2, w2,h-h2);
	QRect Rect6(3*w4,h2,w4,h2);

	painter.fillRect(Rect5, pMainFrame->m_BackgroundColor);

	m_PlrGraph[0].DrawGraph(Rect1, painter);
	m_PlrGraph[1].DrawGraph(Rect2, painter);
	m_PlrGraph[2].DrawGraph(Rect3, painter);
	m_PlrGraph[3].DrawGraph(Rect4, painter);

	if(m_bShowUserGraph)	m_PlrGraph[4].DrawGraph(Rect6, painter);
	else				 	painter.fillRect(Rect6, pMainFrame->m_BackgroundColor);

	PaintPolarLegend(m_PolarLegendOffset,  h, painter);
}


void QXDirect::PaintPolarLegend(QPoint place, int bottom, QPainter &painter)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	int LegendSize, LegendWidth, ypos, x1;
	int i,j,k,l,nc,ny,nFoils;

	LegendSize = 30;
	LegendWidth = 240;

	painter.setFont(pMainFrame->m_TextFont);

	QFontMetrics fm(pMainFrame->m_TextFont);
	ypos = fm.height();

	QPen TextPen(pMainFrame->m_TextColor);
	painter.setPen(TextPen);
	TextPen.setWidth(1);

	QStringList str; // we need to make an inventory of foils
	Polar * pPolar;
	Foil *pFoil;
	for (j=0; j<m_poaFoil->size(); j++)
	{
		pFoil = (Foil*)m_poaFoil->at(j);
		for (i=0; i<m_poaPolar->size(); i++)
		{
			pPolar = (Polar*)m_poaPolar->at(i);
			if (pPolar->m_FoilName == pFoil->m_FoilName && pPolar->m_Alpha.size())
			{
				str.append(pFoil->m_FoilName);
				break;
			}
		}// finished inventory
	}

	nFoils= str.size();

	painter.setBackgroundMode(Qt::TransparentMode);

	QPen LegendPen;
	LegendPen.setWidth(1);

	ny =0;
	for (k=0; k<nFoils; k++)
	{
		int FoilPlrs = 0;
		for (l=0; l < m_poaPolar->size(); l++)
		{
			pPolar = (Polar*)m_poaPolar->at(l);
			if (pPolar->m_Alpha.size() &&
				pPolar->m_PlrName.length() &&
				pPolar->m_bIsVisible &&
				pPolar->m_FoilName == str.at(k) &&
								((pPolar->m_PolarType==FIXEDSPEEDPOLAR  && m_bType1) ||
								 (pPolar->m_PolarType==FIXEDLIFTPOLAR   && m_bType2) ||
								 (pPolar->m_PolarType==RUBBERCHORDPOLAR && m_bType3) ||
								 (pPolar->m_PolarType==FIXEDAOAPOLAR    && m_bType4)))
					FoilPlrs++;
		}
		if (FoilPlrs)
		{
			int YPos = place.y() + (ny+FoilPlrs+2) * ypos;// bottom line of this foil's legend
			if(abs(bottom) > abs(YPos))
			{
				ny++;
			}
			else if (k>0)
			{
			// move rigth if less than client bottom area
				place.rx() += LegendWidth;
				ny=1;
			}
			painter.setPen(TextPen);
			painter.drawText(place.x() + (int)(0.5*LegendSize), place.y() + ypos*ny-(int)(ypos/2),
							 str.at(k));
		}
		for (nc=0; nc<m_poaPolar->size(); nc++)
		{
			pPolar = (Polar*)m_poaPolar->at(nc);
			if(str.at(k) == pPolar->m_FoilName)
			{
				if (pPolar->m_Alpha.size() && pPolar->m_PlrName.length() && pPolar->m_bIsVisible &&
										((pPolar->m_PolarType==FIXEDSPEEDPOLAR  && m_bType1) ||
										 (pPolar->m_PolarType==FIXEDLIFTPOLAR   && m_bType2) ||
										 (pPolar->m_PolarType==RUBBERCHORDPOLAR && m_bType3) ||
										 (pPolar->m_PolarType==FIXEDAOAPOLAR    && m_bType4)))
				{
					//is there anything to draw ?
					LegendPen.setColor(pPolar->m_Color);
					LegendPen.setStyle(GetStyle(pPolar->m_Style));
					LegendPen.setWidth(pPolar->m_Width);
					painter.setPen(LegendPen);

					painter.drawLine(place.x() + (int)(1.0*LegendSize), place.y() + (int)(1.*ypos*ny)+2,
									 place.x() + (int)(2.0*LegendSize), place.y() + (int)(1.*ypos*ny)+2);
					if(pPolar->m_bShowPoints)
					{
						x1 = place.x() + (int)(1.5*LegendSize);
						painter.drawRect(x1-2, place.y() + (int)(1.*ypos*ny), 4, 4);
					}

					painter.setPen(TextPen);
					painter.drawText(place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.*ypos*ny)+(int)(ypos/3),
									 pPolar->m_PlrName);
					ny++ ;
				}
			}
		}
		if (FoilPlrs) ny++;
	}
//	painter.setBackgroundMode(Qt::OpaqueMode);

}


void QXDirect::PaintSingleGraph(QPainter &painter)
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	int h   = m_rCltRect.height();
	int w   = m_rCltRect.width();
	int w3  = (int)(w/3);

	QRect Rect1(0,0,2*w3,h);
	QRect Rect2(2*w3, 0, w-2*w3,h);
	painter.fillRect(Rect2, pMainFrame->m_BackgroundColor);

	if(m_iPlrGraph>=0 && m_iPlrGraph<MAXPOLARGRAPHS) m_pCurGraph = m_PlrGraph + m_iPlrGraph;
	else                                             m_pCurGraph = NULL;

	if (m_pCurGraph) m_pCurGraph->DrawGraph(Rect1, painter);
	PaintPolarLegend(m_PolarLegendOffset, h, painter);
}



void QXDirect::PaintView(QPainter &painter)
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(MainFrame::s_pCurFoil && !m_bPolarView)
	{
		PaintOpPoint(painter);
	}
	else if (m_bPolarView)
	{
		if(m_iPlrView==ALLPOLARGRAPHS)      PaintPolarGraphs(painter);
		else if(m_iPlrView==ONEPOLARGRAPH)  PaintSingleGraph(painter);
		else if(m_iPlrView==TWOPOLARGRAPHS) PaintCoupleGraphs(painter);
	}
	else
	{
		painter.fillRect(m_rCltRect, pMainFrame->m_BackgroundColor);
	}
}


void QXDirect::ReadParams()
{
	if(!m_pCurPolar) return;

	if      (m_pctrlSpec1->isChecked()) m_bAlpha = true;
	else if (m_pctrlSpec2->isChecked()) m_bAlpha = false;
	else if (m_pctrlSpec3->isChecked()) m_bAlpha = false;


	if(m_pCurPolar->m_PolarType!=FIXEDAOAPOLAR)
	{
		if(m_bAlpha)
		{
			m_Alpha      = m_pctrlAlphaMin->Value();
			m_AlphaMax   = m_pctrlAlphaMax->Value();
			m_AlphaDelta = m_pctrlAlphaDelta->Value();
		}
		else
		{
			m_Cl      = m_pctrlAlphaMin->Value();
			m_ClMax   = m_pctrlAlphaMax->Value();
			m_ClDelta = m_pctrlAlphaDelta->Value();
		}
	}
	else
	{
		m_Reynolds      = m_pctrlAlphaMin->Value();
		m_ReynoldsMax   = m_pctrlAlphaMax->Value();
		m_ReynoldsDelta = m_pctrlAlphaDelta->Value();
	}
	m_bSequence = m_pctrlSequence->isChecked();
	m_bInitBL   = m_pctrlInitBL->isChecked();
	m_bViscous  = m_pctrlViscous->isChecked();
	m_bStoreOpp = m_pctrlStoreOpp->isChecked();
}


void QXDirect::SaveSettings(QSettings *pSettings)
{
	QString str1, str2, str3;
	pSettings->beginGroup("XDirect");
	{
		pSettings->setValue("AlphaSpec", m_bAlpha);
		pSettings->setValue("StoreOpp", m_bStoreOpp);
		pSettings->setValue("ViscousAnalysis", m_bViscous);
		pSettings->setValue("InitBL", m_bInitBL);
		pSettings->setValue("BoundaryLayer", m_bBL);
		pSettings->setValue("Pressure", m_bPressure);
		pSettings->setValue("PolarView", m_bPolarView);
		pSettings->setValue("UserGraph", m_bShowUserGraph);
		pSettings->setValue("ShowPanels", m_bShowPanels);
		pSettings->setValue("Type1", m_bType1);
		pSettings->setValue("Type2", m_bType2);
		pSettings->setValue("Type3", m_bType3);
		pSettings->setValue("Type4", m_bType4);
		pSettings->setValue("AutoInitBL", m_bAutoInitBL);
		pSettings->setValue("FromList", m_bFromList);
		pSettings->setValue("FromZero", m_bFromZero);
		pSettings->setValue("TextOutput", m_bShowTextOutput);
		pSettings->setValue("NeutralLine", m_bNeutralLine);
		pSettings->setValue("CurOppOnly", m_bCurOppOnly);
		pSettings->setValue("ShowInviscid", m_bShowInviscid);
		pSettings->setValue("ShowCpGraph", m_bCpGraph);
		pSettings->setValue("Sequence", m_bSequence);
		pSettings->setValue("HighlightOpp",m_bHighlightOpp);
		pSettings->setValue("BLColorRed", m_crBLColor.red());
		pSettings->setValue("BLColorGreen",m_crBLColor.green());
		pSettings->setValue("BLColorBlue", m_crBLColor.blue());
		pSettings->setValue("BLStyle", m_iBLStyle);
		pSettings->setValue("BLWidth", m_iBLWidth);
		pSettings->setValue("PressureColorRed", m_crPressureColor.red());
		pSettings->setValue("PressureColorGreen",m_crPressureColor.green());
		pSettings->setValue("PressureColorBlue", m_crPressureColor.blue());
		pSettings->setValue("PressureStyle", m_iPressureStyle);
		pSettings->setValue("PressureWidth", m_iPressureWidth);
		pSettings->setValue("NeutralColorRed", m_crNeutralColor.red());
		pSettings->setValue("NeutralColorGreen",m_crNeutralColor.green());
		pSettings->setValue("NeutralColorBlue", m_crNeutralColor.blue());
		pSettings->setValue("NeutralStyle", m_iNeutralStyle);
		pSettings->setValue("NeutralWidth", m_iNeutralWidth);
		pSettings->setValue("XFoilVar", m_XFoilVar);
		pSettings->setValue("IterLim", m_IterLim);
		pSettings->setValue("PlrGraph", m_iPlrGraph);

		switch(m_iPlrView)
		{
			case ONEPOLARGRAPH:
				pSettings->setValue("PlrView", 1);
				break;
			case TWOPOLARGRAPHS:
				pSettings->setValue("PlrView", 2);
				break;
			default:
				pSettings->setValue("PlrView", 0);
				break;
		}

		pSettings->setValue("AlphaMin", m_Alpha);
		pSettings->setValue("AlphaMax", m_AlphaMax);
		pSettings->setValue("AlphaDelta", m_AlphaDelta);
		pSettings->setValue("ClMin", m_Cl);
		pSettings->setValue("ClMax", m_ClMax);
		pSettings->setValue("ClDelta", m_ClDelta);
		pSettings->setValue("ReynoldsMin", m_Reynolds);
		pSettings->setValue("ReynoldsMax", m_ReynoldsMax);
		pSettings->setValue("ReynolsDelta", m_ReynoldsDelta);
		pSettings->setValue("NCrit", m_NCrit);
		pSettings->setValue("XTopTr", m_XTopTr);
		pSettings->setValue("XBotTr", m_XBotTr);
		pSettings->setValue("Mach", m_Mach);
		pSettings->setValue("ASpec", m_ASpec);
		pSettings->setValue("VAccel", m_pXFoil->vaccel);
		pSettings->setValue("AutoInitBL", m_bAutoInitBL);
		pSettings->setValue("FullReport", m_pXFoil->m_bFullReport);
		pSettings->setValue("NReynolds", m_NRe);

		if(m_PolarType==FIXEDSPEEDPOLAR)      pSettings->setValue("Type", 1);
		else if(m_PolarType==FIXEDSPEEDPOLAR) pSettings->setValue("Type", 2);
		else if(m_PolarType==FIXEDAOAPOLAR)   pSettings->setValue("Type", 4);
		else if(m_PolarType==STABILITYPOLAR)  pSettings->setValue("Type", 7);

		for (int i=0; i<m_NRe; i++)
		{
			str1 = QString("ReList%1").arg(i);
			str2 = QString("MaList%1").arg(i);
			str3 = QString("NcList%1").arg(i);
			pSettings->setValue(str1, m_ReList[i]);
			pSettings->setValue(str2, m_MachList[i]);
			pSettings->setValue(str3, m_NCritList[i]);
		}
	}
	pSettings->endGroup();

	m_PlrGraph[0].SaveSettings(pSettings);
	m_PlrGraph[2].SaveSettings(pSettings);
	m_PlrGraph[1].SaveSettings(pSettings);
	m_PlrGraph[3].SaveSettings(pSettings);
	m_PlrGraph[4].SaveSettings(pSettings);
	m_pCpGraph->SaveSettings(pSettings);
}


void QXDirect::SetAnalysisParams()
{
	m_pctrlViscous->setChecked(m_bViscous);
	m_pctrlInitBL->setChecked(m_bInitBL);
	m_pctrlStoreOpp->setChecked(m_bStoreOpp);
	m_pctrlShowPressure->setChecked(m_bPressure);
	m_pctrlShowBL->setChecked(m_bBL);

	if(m_pCurPolar)
	{
		if(m_pCurPolar->m_PolarType!=FIXEDAOAPOLAR)
		{
			m_pctrlAlphaMin->SetPrecision(2);
			m_pctrlAlphaMax->SetPrecision(2);
			m_pctrlAlphaDelta->SetPrecision(2);
			if(m_bAlpha) m_pctrlSpec1->setChecked(true);
			else         m_pctrlSpec2->setChecked(true);
			m_pctrlSpec3->setEnabled(false);
		}
		else
		{
			m_pctrlSpec3->setChecked(true);
			m_pctrlSpec3->setEnabled(true);
			m_pctrlAlphaMin->SetPrecision(0);
			m_pctrlAlphaMax->SetPrecision(0);
			m_pctrlAlphaDelta->SetPrecision(0);
		}
	}
	else
	{
		if(m_bAlpha) m_pctrlSpec1->setChecked(true);
		else         m_pctrlSpec2->setChecked(true);
		m_pctrlSpec3->setEnabled(false);
	}
	SetOpPointSequence();

	SetCurveParams();
}


void QXDirect::SetBufferFoil()
{
	if(!MainFrame::s_pCurFoil || !MainFrame::s_pCurFoil->m_FoilName.length()) return;

	m_BufferFoil.CopyFoil(MainFrame::s_pCurFoil);

	m_BufferFoil.m_FoilName   = MainFrame::s_pCurFoil->m_FoilName;
	m_BufferFoil.m_FoilColor  = MainFrame::s_pCurFoil->m_FoilColor;
	m_BufferFoil.m_nFoilStyle = MainFrame::s_pCurFoil->m_nFoilStyle;
	m_BufferFoil.m_nFoilWidth = MainFrame::s_pCurFoil->m_nFoilWidth;
}


void QXDirect::SetCurveParams()
{
	if(m_bPolarView)
	{
		if(m_pCurPolar)
		{
			if(m_pCurPolar->m_bIsVisible)  m_pctrlShowCurve->setChecked(true);  else  m_pctrlShowCurve->setChecked(false);
			if(m_pCurPolar->m_bShowPoints) m_pctrlShowPoints->setChecked(true); else  m_pctrlShowPoints->setChecked(false);

			m_CurveColor = m_pCurPolar->m_Color;
			m_CurveStyle = m_pCurPolar->m_Style;
			m_CurveWidth = m_pCurPolar->m_Width;
			FillComboBoxes();
		}
		else
		{
			FillComboBoxes(false);
		}
	}
	else
	{
		//set Opoint params
		if(m_pCurOpp)
		{
			if(m_pCurOpp->m_bIsVisible)  m_pctrlShowCurve->setChecked(true);  else  m_pctrlShowCurve->setChecked(false);
			if(m_pCurOpp->m_bShowPoints) m_pctrlShowPoints->setChecked(true); else  m_pctrlShowPoints->setChecked(false);

			m_CurveColor = m_pCurOpp->m_Color;
			m_CurveStyle = m_pCurOpp->m_Style;
			m_CurveWidth = m_pCurOpp->m_Width;
			FillComboBoxes();
		}
		else
		{
			FillComboBoxes(false);
		}
	}
	if(m_pCurPolar)
	{
		if(m_pCurPolar->m_PolarType!=FIXEDAOAPOLAR)
		{
			m_pctrlUnit1->setText(QString::fromUtf8("°"));
			m_pctrlUnit2->setText(QString::fromUtf8("°"));
			m_pctrlUnit3->setText(QString::fromUtf8("°"));
		}
		else
		{
			m_pctrlUnit1->setText("");
			m_pctrlUnit2->setText("");
			m_pctrlUnit3->setText("");
		}
	}
}



Foil* QXDirect::SetFoil(Foil* pFoil)
{
//	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	StopAnimate();

	MainFrame::s_pCurFoil = pFoil;

	if(!MainFrame::s_pCurFoil)
	{
		//take the first in the array, if any
		if(m_poaFoil->size())
		{
			MainFrame::s_pCurFoil = (Foil*)m_poaFoil->at(0);
		}
	}

	if(MainFrame::s_pCurFoil && !m_pXFoil->InitXFoilGeometry(MainFrame::s_pCurFoil))
	{
		DeleteFoil(false);
		MainFrame::s_pCurFoil = NULL;
	}
	else
	{
		if(!MainFrame::s_pCurFoil)
		{
			m_pCurPolar = NULL;
			m_pCurOpp = NULL;
		}
		else if (m_pCurPolar && m_pCurPolar->m_FoilName !=MainFrame::s_pCurFoil->m_FoilName)
		{
//			m_pCurPolar = NULL;
//			m_pCurOpp = NULL;
		}
		else if (m_pCurOpp && m_pCurOpp->m_strFoilName  !=MainFrame::s_pCurFoil->m_FoilName)
		{
//			m_pCurOpp = NULL;
		}
	}

	SetBufferFoil();

	SetPolar();

	return MainFrame::s_pCurFoil;
}


Foil* QXDirect::SetFoil(QString FoilName)
{
//	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	int j;
	StopAnimate();

	if(FoilName.length())
	{
		Foil *pFoil;
		MainFrame::s_pCurFoil = NULL;
		for (j=0; j< m_poaFoil->size(); j++)
		{
			pFoil = (Foil*)m_poaFoil->at(j);
			if(pFoil->m_FoilName == FoilName)
			{
				MainFrame::s_pCurFoil = pFoil;
				break;
			}
		}
	}
	else MainFrame::s_pCurFoil = NULL;

	if(MainFrame::s_pCurFoil)
	{
		if(!m_pXFoil->InitXFoilGeometry(MainFrame::s_pCurFoil))
		{
			MainFrame::s_pCurFoil=NULL;
			DeleteFoil(false);
			SetPolar();
			return NULL;
		}
	}
	if(!MainFrame::s_pCurFoil)
	{
		m_pCurPolar = NULL;
		m_pCurOpp = NULL;
	}
	else if(m_pCurPolar && m_pCurPolar->m_FoilName !=MainFrame::s_pCurFoil->m_FoilName)
	{
//		m_pCurPolar = NULL;
//		m_pCurOpp = NULL;
	}
	else if(m_pCurOpp && m_pCurOpp->m_strFoilName  !=MainFrame::s_pCurFoil->m_FoilName)
	{
//		m_pCurOpp = NULL;
	}

	//are there 2 or more Opps associated to this foil
	int count = 0;

	OpPoint *pOpp;
	for (int i=0; i< m_poaOpp->size(); i++)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		if(pOpp->m_strFoilName == FoilName) count++;
		if(count>=2) break;
	}


	SetBufferFoil();
	SetPolar();

	return MainFrame::s_pCurFoil;
}




void QXDirect::SetFoilScale(QRect CltRect)
{
	m_rCltRect = CltRect;

	int w23 = (int)(2./3.*(double)m_rCltRect.width());
	m_PolarLegendOffset = QPoint(w23+10, 10);

	SetFoilScale();
}


void QXDirect::SetFoilScale()
{
	QRect rect(10, 10, + m_rCltRect.width()-20, m_rCltRect.height()-2*m_FoilYPos);
	m_pCpGraph->SetDrawRect(rect);

	m_FoilOffset.rx() = rect.left() +(int)(1.0*m_pCpGraph->GetMargin());
	m_FoilOffset.ry() = m_rCltRect.bottom()-m_FoilYPos;

	m_fFoilScale = (rect.width()-2.0*m_pCpGraph->GetMargin());
	if(m_pCpGraph->GetYVariable()>=2)
	{
		double p0  = m_pCpGraph->xToClient(0.0);
		double p1  = m_pCpGraph->xToClient(1.0);
		m_fFoilScale =  (p1-p0);
	}
}


void QXDirect::SetHingeMoments(OpPoint *pOpPoint)
{
//	bool bFound;
	int i;
	double hmom, hfx, hfy;
	double dx, dy, xmid, ymid, pmid;
	double xof, yof;
	double ymin, ymax;
	xof = MainFrame::s_pCurFoil->m_TEXHinge/100.0;
	ymin = MainFrame::s_pCurFoil->GetBaseLowerY(xof);
	ymax = MainFrame::s_pCurFoil->GetBaseUpperY(xof);
	yof = ymin + (ymax-ymin) * MainFrame::s_pCurFoil->m_TEYHinge/100.0;

	if(MainFrame::s_pCurFoil->m_bTEFlap)
	{
		hmom = 0.0;
		hfx  = 0.0;
		hfy  = 0.0;

		//---- integrate pressures on top and bottom sides of flap
		for (i=0;i<MainFrame::s_pCurFoil->n-1;i++)
		{
			if (MainFrame::s_pCurFoil->x[i]>xof &&	MainFrame::s_pCurFoil->x[i+1]>xof)
			{
				dx = MainFrame::s_pCurFoil->x[i+1] - MainFrame::s_pCurFoil->x[i];
				dy = MainFrame::s_pCurFoil->y[i+1] - MainFrame::s_pCurFoil->y[i];
				xmid = 0.5*(MainFrame::s_pCurFoil->x[i+1]+MainFrame::s_pCurFoil->x[i]) - xof;
				ymid = 0.5*(MainFrame::s_pCurFoil->y[i+1]+MainFrame::s_pCurFoil->y[i]) - yof;

				if(pOpPoint->m_bViscResults)	pmid = 0.5*(pOpPoint->Cpv[i+1] + pOpPoint->Cpv[i]);
				else					pmid = 0.5*(pOpPoint->Cpi[i+1] + pOpPoint->Cpi[i]);


				hmom += pmid * (xmid*dx + ymid*dy);
				hfx  -= pmid * dy;
				hfy  += pmid * dx;
			}
		}
		//Next add top chunk left out in the previous loop
/*		bFound = false;
		for (i=0;i<MainFrame::g_ppCurFoil->n-1;i++){
			if(MainFrame::g_ppCurFoil->x[i]>xof && MainFrame::g_ppCurFoil->x[i+1]<xof){
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

Polar * QXDirect::SetPolar(Polar *pPolar)
{
	// Finds the plr
	// sets it as the active polar
	// and initializes XFoil
	StopAnimate();


	if(!MainFrame::s_pCurFoil|| !MainFrame::s_pCurFoil->m_FoilName.length())
	{
		m_pCurPolar = NULL;
		SetAnalysisParams();
		return NULL;
	}

	if(pPolar) m_pCurPolar = pPolar;

	if(!m_pCurPolar)
	{
		//try to get one from the object array
		for(int i=0; i<m_poaPolar->size(); i++)
		{
			pPolar = (Polar*)m_poaPolar->at(i);
			if(pPolar && pPolar->m_FoilName==MainFrame::s_pCurFoil->m_FoilName)
			{
				//set this one
				m_pCurPolar = pPolar;
				break;
			}
		}
	}

	if(m_pCurPolar)
	{
		if(m_pCurPolar->m_FoilName != MainFrame::s_pCurFoil->m_FoilName)
		{
			Polar *pOldPolar;
			bool bFound = false;
			for (int i=0; i<m_poaPolar->size(); i++)
			{
				pOldPolar = (Polar*)m_poaPolar->at(i);
				if ((pOldPolar->m_FoilName == MainFrame::s_pCurFoil->m_FoilName) &&
					(pOldPolar->m_PlrName == m_pCurPolar->m_PlrName))
				{
					m_pCurPolar = pOldPolar;
					bFound = true;
					break;
				}
			}
			if(!bFound)
			{
				m_pCurPolar = NULL;
				m_pCurOpp   = NULL;
			}
		}
		m_bInitBL = true;
		m_pctrlInitBL->setChecked(m_bInitBL);
	}

	m_pXFoil->InitXFoilAnalysis(m_pCurPolar);
	if (m_bPolarView) CreatePolarCurves();
	SetAnalysisParams();
	SetOpp();

	return m_pCurPolar;
}


Polar * QXDirect::SetPolar(QString PlrName)
{
	// Finds the plr with name PlrName
	// sets it as the active polar
	// and initializes XFoil
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	if(!MainFrame::s_pCurFoil) return NULL;

	if(!PlrName.length())
	{// try to read it
		if (!pMainFrame->m_pctrlPolar->count())
		{
			m_pCurPolar = NULL;
			m_pCurOpp   = NULL;
			return NULL;//give up
		}
		//else
		PlrName = pMainFrame->m_pctrlPolar->currentText();
	}
	m_pCurPolar = pMainFrame->GetPolar(MainFrame::s_pCurFoil->m_FoilName, PlrName);

	if (!m_pCurPolar || !MainFrame::s_pCurFoil ||
		!m_pCurPolar->m_FoilName.length() ||
		!MainFrame::s_pCurFoil->m_FoilName.length())
	{
		m_pCurPolar = NULL;
		m_pCurOpp   = NULL;
		return NULL;
	}

	if(m_pCurPolar && (m_pCurPolar->m_FoilName == MainFrame::s_pCurFoil->m_FoilName))
	{
		m_pXFoil->InitXFoilAnalysis(m_pCurPolar);
	}
	else
	{
		m_pCurPolar = NULL;
		m_pCurOpp = NULL;
	}
	m_bInitBL = true;
	m_pctrlInitBL->setChecked(m_bInitBL);

	if(m_pCurOpp)
	{
		if (m_pCurOpp->m_strFoilName != MainFrame::s_pCurFoil->m_FoilName || m_pCurOpp->m_strPlrName  != PlrName)
		{
//			does the Opp exist for these Foil/plrs ?
			OpPoint *pOpp;
			bool bFound = false;
			for (int l =0; l<m_poaOpp->size(); l++)
			{
				pOpp = (OpPoint*)m_poaOpp->at(l);
				if (pOpp->m_strFoilName == MainFrame::s_pCurFoil->m_FoilName &&
					pOpp->m_strPlrName  == PlrName)
				{
					if(fabs(pOpp->Alpha-m_pCurOpp->Alpha)<0.0001)
					{
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

	SetAnalysisParams();
	SetOpp();

	return m_pCurPolar;
}



OpPoint * QXDirect::SetOpp(double Alpha)
{
	// set the opp, if valid
	// else set the current Opp, if any
	// else set the comboBox's first, if any
	// else set it to NULL
	OpPoint * pOpp = NULL;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	QString strong;

	if(!MainFrame::s_pCurFoil)  return NULL;
	if(!m_pCurPolar) return NULL;

	if(Alpha < -1234567.0) //the default
	{
		if(m_pCurOpp && m_pCurOpp->m_strFoilName == MainFrame::s_pCurFoil->m_FoilName)	pOpp = m_pCurOpp;
		else
		{
			//try to get one from the combobox
			if(pMainFrame->m_pctrlOpPoint->count())
			{
				QString strong;
				bool bOK;
				int sel = pMainFrame->m_pctrlOpPoint->currentIndex();
				if (sel>=0) strong = pMainFrame->m_pctrlOpPoint->itemText(sel);
				Alpha = strong.toFloat(&bOK);
				if(!bOK) pOpp = NULL;
				else     pOpp = pMainFrame->GetOpp(Alpha);
			}
			else pOpp = NULL;
		}
	}
	else
	{
		pOpp = pMainFrame->GetOpp(Alpha);
	}


	if(pOpp) 
	{
		//set it
		pMainFrame->SelectOpPoint(pOpp);//to Combobox
	}
	m_pCurOpp = pOpp;

	if(!m_bPolarView) CreateOppCurves();
	else
	{
		if(m_bHighlightOpp) CreatePolarCurves();
	}


	SetCurveParams();

	return m_pCurOpp;
}



void QXDirect::SetOpPointSequence()
{
	m_pctrlSequence->setEnabled(m_pCurPolar);
	m_pctrlAlphaMin->setEnabled(m_pCurPolar);
	m_pctrlAnalyze->setEnabled(m_pCurPolar);
	m_pctrlViscous->setEnabled(m_pCurPolar);
	m_pctrlInitBL->setEnabled(m_pCurPolar);
	m_pctrlStoreOpp->setEnabled(m_pCurPolar);

	if(m_bSequence && m_pCurPolar)
	{
		m_pctrlSequence->setCheckState(Qt::Checked);
		m_pctrlAlphaMax->setEnabled(true);
		m_pctrlAlphaDelta->setEnabled(true);
	}
	else if (m_pCurPolar)
	{
		m_pctrlSequence->setCheckState(Qt::Unchecked);
		m_pctrlAlphaMax->setEnabled(false);
		m_pctrlAlphaDelta->setEnabled(false);
	}
	else
	{
		m_pctrlAlphaMax->setEnabled(false);
		m_pctrlAlphaDelta->setEnabled(false);
	}


	if(m_pCurPolar && m_pCurPolar->m_PolarType!=FIXEDAOAPOLAR)
	{
		if(m_pctrlSpec3->isChecked())
		{
			m_pctrlSpec1->setChecked(true);
			m_bAlpha = true;
		}

		if(m_bAlpha)
		{
			m_pctrlAlphaMin->SetValue(m_Alpha);
			m_pctrlAlphaMax->SetValue(m_AlphaMax);
			m_pctrlAlphaDelta->SetValue(m_AlphaDelta);
		}
		else
		{
			m_pctrlAlphaMin->SetValue(m_Cl);
			m_pctrlAlphaMax->SetValue(m_ClMax);
			m_pctrlAlphaDelta->SetValue(m_ClDelta);
		}
		m_pctrlSpec1->setEnabled(true);
		m_pctrlSpec2->setEnabled(true);
		m_pctrlSpec3->setEnabled(false);
	}
	else if(m_pCurPolar && m_pCurPolar->m_PolarType==FIXEDAOAPOLAR)
	{
		m_pctrlSpec3->setChecked(true);
		m_bAlpha = true;		// no choice with type 4 polars
		m_pctrlAlphaMin->SetValue(m_Reynolds);
		m_pctrlAlphaMax->SetValue(m_ReynoldsMax);
		m_pctrlAlphaDelta->SetValue(m_ReynoldsDelta);
		m_pctrlSpec1->setEnabled(false);
		m_pctrlSpec2->setEnabled(false);
		m_pctrlSpec3->setEnabled(true);
	}
	else
	{
		m_pctrlSpec1->setEnabled(false);
		m_pctrlSpec2->setEnabled(false);
		m_pctrlSpec3->setEnabled(false);
	}
}



void QXDirect::SetGraphTitles(Graph* pGraph)
{
	if(!pGraph) return;

	QString Title;
	Polar::GetPlrVariableName(pGraph->GetXVariable(),Title);
	pGraph->SetXTitle(Title);

	Polar::GetPlrVariableName(pGraph->GetYVariable(), Title);
	pGraph->SetYTitle(Title);

}



void QXDirect::SetPolarLegendPos()
{
	int h   = m_rCltRect.height();
	int w   = m_rCltRect.width();
	int h2  = (int)(h/2);
	int h23 = (int)(2*h/3);
	int w3  = (int)(w/3);
	int w23 = 2*w3;
	int margin = 10;

	if(m_iPlrView == ONEPOLARGRAPH)
	{
		m_PolarLegendOffset.rx() = w23+margin;
		m_PolarLegendOffset.ry() = margin;
	}
	else if (m_iPlrView == TWOPOLARGRAPHS)
	{
		m_PolarLegendOffset.rx() = margin;
		m_PolarLegendOffset.ry() = h23+margin;
	}
	else if	(m_iPlrView == ALLPOLARGRAPHS)
	{
		m_PolarLegendOffset.rx() = margin;
		m_PolarLegendOffset.ry() = h2+30;
	}
}


void QXDirect::SetupLayout()
{
	setAttribute(Qt::WA_AlwaysShowToolTips);
	QHBoxLayout *SpecVarsBox = new QHBoxLayout;

	m_pctrlSpec1 = new QRadioButton("a");
	m_pctrlSpec2 = new QRadioButton(tr("Cl"));
	m_pctrlSpec3 = new QRadioButton(tr("Re"));
	m_pctrlSpec1->setFont(QFont("Symbol"));
	SpecVarsBox->addWidget(m_pctrlSpec1);
	SpecVarsBox->addWidget(m_pctrlSpec2);
	SpecVarsBox->addWidget(m_pctrlSpec3);

	m_pctrlSequence = new QCheckBox(tr("Sequence"));

	QGridLayout *SequenceGroup = new QGridLayout;
	QLabel *AlphaMinLab   = new QLabel(tr("Start="));
	QLabel *AlphaMaxLab   = new QLabel(tr("End="));
	QLabel *DeltaAlphaLab = new QLabel(tr("D="));
	DeltaAlphaLab->setFont(QFont("Symbol"));
	DeltaAlphaLab->setAlignment(Qt::AlignRight);
	AlphaMinLab->setAlignment(Qt::AlignRight);
	AlphaMaxLab->setAlignment(Qt::AlignRight);

	m_pctrlUnit1 = new QLabel(QString::fromUtf8("°"));
	m_pctrlUnit2 = new QLabel(QString::fromUtf8("°"));
	m_pctrlUnit3 = new QLabel(QString::fromUtf8("°"));


	m_pctrlAlphaMin     = new FloatEdit();
	m_pctrlAlphaMax     = new FloatEdit();
	m_pctrlAlphaDelta   = new FloatEdit();
	m_pctrlAlphaMin->setMinimumHeight(20);
	m_pctrlAlphaMax->setMinimumHeight(20);
	m_pctrlAlphaDelta->setMinimumHeight(20);
	m_pctrlAlphaMin->setAlignment(Qt::AlignRight);
	m_pctrlAlphaMax->setAlignment(Qt::AlignRight);
	m_pctrlAlphaDelta->setAlignment(Qt::AlignRight);
	SequenceGroup->addWidget(AlphaMinLab,1,1);
	SequenceGroup->addWidget(AlphaMaxLab,2,1);
	SequenceGroup->addWidget(DeltaAlphaLab,3,1);
	SequenceGroup->addWidget(m_pctrlAlphaMin,1,2);
	SequenceGroup->addWidget(m_pctrlAlphaMax,2,2);
	SequenceGroup->addWidget(m_pctrlAlphaDelta,3,2);
	SequenceGroup->addWidget(m_pctrlUnit1,1,3);
	SequenceGroup->addWidget(m_pctrlUnit2,2,3);
	SequenceGroup->addWidget(m_pctrlUnit3,3,3);

	QHBoxLayout *AnalysisSettings = new QHBoxLayout;
	m_pctrlViscous  = new QCheckBox(tr("Viscous"));
	m_pctrlInitBL   = new QCheckBox(tr("Init BL"));
	AnalysisSettings->addWidget(m_pctrlViscous);
	AnalysisSettings->addWidget(m_pctrlInitBL);

	m_pctrlStoreOpp = new QCheckBox(tr("Store Opp"));
	m_pctrlAnalyze  = new QPushButton(tr("Analyze"));

	QVBoxLayout *AnalysisGroup = new QVBoxLayout;
	AnalysisGroup->addLayout(SpecVarsBox);
	AnalysisGroup->addStretch(1);
	AnalysisGroup->addWidget(m_pctrlSequence);
	AnalysisGroup->addLayout(SequenceGroup);
	AnalysisGroup->addStretch(1);
	AnalysisGroup->addLayout(AnalysisSettings);
	AnalysisGroup->addWidget(m_pctrlStoreOpp);
	AnalysisGroup->addWidget(m_pctrlAnalyze);

	QGroupBox *AnalysisBox = new QGroupBox(tr("Analysis settings"));
	AnalysisBox->setLayout(AnalysisGroup);


	QVBoxLayout *DisplayGroup = new QVBoxLayout;
	m_pctrlShowBL        = new QCheckBox(tr("Show BL"));
	m_pctrlShowPressure  = new QCheckBox(tr("Show Pressure"));
//	m_pctrlHighlightOpp  = new QCheckBox(tr("Highlight Current OpPoint"));
//	m_pctrlHighlightOpp->setToolTip(tr("Highlights the currently selected OpPoint, if any, on the currently selected polar curve"));
	m_pctrlAnimate       = new QCheckBox(tr("Animate"));
	m_pctrlAnimateSpeed  = new QSlider(Qt::Horizontal);
	m_pctrlAnimateSpeed->setMinimum(0);
	m_pctrlAnimateSpeed->setMaximum(500);
	m_pctrlAnimateSpeed->setSliderPosition(250);
	m_pctrlAnimateSpeed->setTickInterval(25);
	m_pctrlAnimateSpeed->setTickPosition(QSlider::TicksBelow);
	DisplayGroup->addWidget(m_pctrlShowBL);
	DisplayGroup->addWidget(m_pctrlShowPressure);
	DisplayGroup->addWidget(m_pctrlAnimate);
	DisplayGroup->addWidget(m_pctrlAnimateSpeed);
//	DisplayGroup->addWidget(m_pctrlHighlightOpp);
	QGroupBox *DisplayBox = new QGroupBox(tr("Display"));
	DisplayBox->setLayout(DisplayGroup);

	QGroupBox *PolarPropsBox = new QGroupBox(tr("Polar properties"));
	m_pctrlPolarProps = new QLabel;
//	m_pctrlPolarProps->setReadOnly(true);
//	m_pctrlPolarProps->setWordWrapMode(QTextOption::NoWrap);
	QHBoxLayout *PolarPropsLayout = new QHBoxLayout;
	PolarPropsLayout->addWidget(m_pctrlPolarProps);
	PolarPropsBox->setLayout(PolarPropsLayout);

	QHBoxLayout *CurveDisplay = new QHBoxLayout;
	m_pctrlShowCurve  = new QCheckBox(tr("Curve"));
	m_pctrlShowPoints = new QCheckBox(tr("Points"));
	CurveDisplay->addWidget(m_pctrlShowCurve);
	CurveDisplay->addWidget(m_pctrlShowPoints);
	QVBoxLayout *CurveGroup = new QVBoxLayout;
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


	CurveGroup->addLayout(CurveDisplay);
	CurveGroup->addLayout(CurveStyleLayout);
	CurveGroup->addStretch(1);

	QGroupBox *CurveBox = new QGroupBox(tr("Graph Curve Settings"));
	CurveBox->setLayout(CurveGroup);

	m_pctrlMiddleControls = new QStackedWidget;
	m_pctrlMiddleControls->addWidget(DisplayBox);
	m_pctrlMiddleControls->addWidget(PolarPropsBox);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addStretch(1);
	mainLayout->addWidget(AnalysisBox);
	mainLayout->addStretch(1);
	mainLayout->addWidget(m_pctrlMiddleControls);
	mainLayout->addStretch(1);
	mainLayout->addWidget(CurveBox);
	mainLayout->addStretch(1);

	setLayout(mainLayout);

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
}


void QXDirect::StopAnimate()
{
	if(m_bAnimate)
	{
		m_pAnimateTimer->stop();
		m_bAnimate = false;
		m_pctrlAnimate->setChecked(false);
		SetOpp();
	}
}


void QXDirect::UpdateCurve()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(m_bPolarView && m_pCurPolar)
	{
		m_pCurPolar->m_Color = m_CurveColor;
		m_pCurPolar->m_Style = m_CurveStyle;
		m_pCurPolar->m_Width = (int)m_CurveWidth;
		CreatePolarCurves();
	}
	else if (!m_bPolarView && m_pCurOpp)
	{
		m_pCurOpp->m_Color = m_CurveColor;
		m_pCurOpp->m_Style = m_CurveStyle;
		m_pCurOpp->m_Width = (int)m_CurveWidth;
		CreateOppCurves();
	}

	UpdateView();
	pMainFrame->SetSaveState(false);
}


void QXDirect::UpdateView()
{
	TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;

	if(s_p2DWidget)
	{
		p2DWidget->update();
	}
}


void QXDirect::wheelEvent (QWheelEvent *event )
{
//	point is in client coordinates
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
		else                            ZoomFactor = 1./1.06;
	}

	m_pCurGraph = GetGraph(pt);

	if(m_pCurGraph && m_pCurGraph->IsInDrawRect(pt) && m_bCpGraph)
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

		if(!m_bAnimate) UpdateView();
	}
	else if(MainFrame::s_pCurFoil && !m_bPolarView)
	{
		double scale = m_fFoilScale;

		m_fFoilScale *= ZoomFactor;

		int a = (int)((m_rCltRect.right()+m_rCltRect.left())/2);

		m_FoilOffset.rx() = a + (int)((m_FoilOffset.x()-a)/scale*m_fFoilScale);

		if(!m_bAnimate) UpdateView();
	}
}




void QXDirect::OnOpPointProps()
{
	if(!m_pCurPolar) return;
	m_pObjectPropsDlg->m_pXDirect = this;
	m_pObjectPropsDlg->m_pOpp = m_pCurOpp;
	m_pObjectPropsDlg->m_pPolar = NULL;
	m_pObjectPropsDlg->m_pMiarex = NULL;
	m_pObjectPropsDlg->m_pWOpp = NULL;
	m_pObjectPropsDlg->m_pWPolar = NULL;
	m_pObjectPropsDlg->InitDialog();
	m_pObjectPropsDlg->exec();
}



void QXDirect::OnPolarProps()
{
	if(!m_pCurPolar) return;
	m_pObjectPropsDlg->m_pXDirect = this;
	m_pObjectPropsDlg->m_pOpp = NULL;
	m_pObjectPropsDlg->m_pPolar = m_pCurPolar;
	m_pObjectPropsDlg->m_pMiarex = NULL;
	m_pObjectPropsDlg->m_pWOpp = NULL;
	m_pObjectPropsDlg->m_pWPolar = NULL;
	m_pObjectPropsDlg->InitDialog();
	m_pObjectPropsDlg->exec();
}




void QXDirect::OnRenameFoil()
{
	if(!MainFrame::s_pCurFoil) return;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->OnRenameCurFoil();
}


