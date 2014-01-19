/****************************************************************************

	QXDirect Class
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
#include <QtDebug>
#include <math.h>

#include "../globals.h"
#include "../mainframe.h"
#include "../misc/Settings.h"
#include "XDirect.h"

#include "../graph/GraphDlg.h"

#include "../misc/PolarFilterDlg.h"
#include "../misc/ObjectPropsDlg.h"
#include "../misc/RenameDlg.h"
#include "../misc/EditPlrDlg.h"

#include "analysis/XFoilAnalysisDlg.h"
#include "analysis/XFoilAdvancedDlg.h"
#include "analysis/FoilPolarDlg.h"
#include "analysis/BatchThreadDlg.h"
#include "analysis/BatchDlg.h"

#include "geometry/TwoDPanelDlg.h"
#include "geometry/InterpolateFoilsDlg.h"
#include "geometry/NacaFoilDlg.h"
#include "geometry/FoilCoordDlg.h"
#include "geometry/FoilGeomDlg.h"
#include "geometry/TEGapDlg.h"
#include "geometry/LEDlg.h"
#include "geometry/FlapDlg.h"
#include "geometry/CAddDlg.h"

#include "XDirectStyleDlg.h"
#include "ManageFoilsDlg.h"

Polar QXDirect::s_refPolar;

QList<double> QXDirect::s_ReList;
QList<double> QXDirect::s_MachList;
QList<double> QXDirect::s_NCritList;

bool QXDirect::s_bViscous = true;
bool QXDirect::s_bAlpha = true;
bool QXDirect::s_bInitBL = true;
bool QXDirect::s_bKeepOpenErrors = true;
bool QXDirect::s_bFromZero = false;

int QXDirect::s_TimeUpdateInterval = 100;

void *QXDirect::s_pMainFrame;
void *QXDirect::s_p2DWidget;

/** @todo is there any use for a buffer foil at all ? */


/**
*The public constructor.
*/
QXDirect::QXDirect(QWidget *parent) : QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	SetupLayout();

	m_pXFoil = new XFoil();

	m_CurveStyle = 0;
	m_CurveWidth = 1;
	m_CurveColor = QColor(0,0,0);

	m_pAnimateTimer = new QTimer(this);
	m_posAnimate = 0; // no animation to start with
	Connect();

	FillComboBoxes(false);

	m_bAnimate        = false;
	m_bAnimatePlus    = false;
	m_bCpGraph        = true;
	m_bTransGraph     = false;
	m_bShowPanels     = false;
	m_bShowUserGraph  = true;
	m_bSequence       = false;
	m_bHighlightOpp   = false;

	m_bXPressed = m_bYPressed = false;

	m_bTrans          = false;
	m_bType1          = true;
	m_bType2          = true;
	m_bType3          = true;
	m_bType4          = true;
	m_bPressure       = false;
	m_bBL             = false;
	m_bFromList       = false;
	m_bShowTextOutput = true;
	m_bNeutralLine    = true;
	m_bShowInviscid   = false;
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

	m_bPolarView          = true;
	m_iPlrGraph = 0;
	m_iPlrView  = ALLGRAPHS;
	m_XFoilVar  = 0;
	m_FoilYPos  = 150;

	m_PointDown.setX(0);
	m_PointDown.setY(0);

	m_posAnimate = 0;

	m_pCurGraph    = NULL;
	Polar::setCurPolar(NULL);
	OpPoint::setCurOpp(NULL);



	m_Alpha      = 0.0;
	m_AlphaMax   = 1.0;
	m_AlphaDelta = 0.5;
	m_Cl         = 0.0;
	m_ClMax      = 1.0;
	m_ClDelta    = 0.1;
	m_Reynolds      = 100000.0;
	m_ReynoldsMax   = 150000.0;
	m_ReynoldsDelta =  10000.0;

	m_CpGraph.SetVariables(0,0);

	m_PlrGraph[0].SetVariables(2,1);
	m_PlrGraph[1].SetVariables(0,1);
	m_PlrGraph[2].SetVariables(0,5);
	m_PlrGraph[3].SetVariables(6,1);
	m_PlrGraph[4].SetVariables(0,10);

	m_CpGraph.SetXTitle(tr("X"));
	m_CpGraph.SetYTitle(tr("Cp"));
	m_CpGraph.SetInverted(true);
	m_CpGraph.SetXMin(0.0);
	m_CpGraph.SetXMax(1.0);
	m_CpGraph.SetYMin(-0.1);
	m_CpGraph.SetYMax(0.1);
	m_CpGraph.SetMargin(50);
	m_CpGraph.SetBorderColor(QColor(200,200,200));
	m_CpGraph.SetBorder(true);
	m_CpGraph.SetBorderStyle(0);
	m_CpGraph.SetGraphName("Cp_Graph");

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
	m_PlrGraph[0].SetGraphName("Polar_Graph");

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
	m_PlrGraph[2].SetGraphName("Cm_Graph");

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
	m_PlrGraph[1].SetGraphName("Cz_Graph");

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
	m_PlrGraph[3].SetGraphName("Tr_Graph");

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
	m_PlrGraph[4].SetGraphName("User_Graph");

	for(int ig=0; ig<MAXPOLARGRAPHS; ig++)
		SetGraphTitles(m_PlrGraph+ig);

	s_ReList.clear();
	s_MachList.clear();
	s_NCritList.clear();

	for(int iRe=0; iRe<12; iRe++)
	{
		s_ReList.append(0.0);
		s_MachList.append(0.0);
		s_NCritList.append(9.0);
	}

	s_ReList[0]  =   30000.0;
	s_ReList[1]  =   40000.0;
	s_ReList[2]  =   60000.0;
	s_ReList[3]  =   80000.0;
	s_ReList[4]  =  100000.0;
	s_ReList[5]  =  130000.0;
	s_ReList[6]  =  160000.0;
	s_ReList[7]  =  200000.0;
	s_ReList[8]  =  300000.0;
	s_ReList[9]  =  500000.0;
	s_ReList[10] = 1000000.0;
	s_ReList[11] = 3000000.0;


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
	qDebug()<<"Destroying XDirect";
}


/** Sets the state of the window's widgets i.a.w. the state of the active ojbects and views. */
void QXDirect::SetControls()
{
	setAttribute(Qt::WA_DeleteOnClose);

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	if(m_bPolarView) m_pctrlMiddleControls->setCurrentIndex(1);
	else             m_pctrlMiddleControls->setCurrentIndex(0);

	if(Polar::curPolar())
	{
		QString PolarProps;
		Polar::curPolar()->GetPolarProperties(PolarProps);
		m_pctrlPolarProps->setText(PolarProps);
	}
	else m_pctrlPolarProps->clear();

	pMainFrame->OpPointsAct->setChecked(!m_bPolarView);
	pMainFrame->PolarsAct->setChecked(m_bPolarView);

	pMainFrame->showPanels->setChecked(m_bShowPanels);
	pMainFrame->showNeutralLine->setChecked(m_bNeutralLine);
	pMainFrame->showInviscidCurve->setChecked(m_bShowInviscid);
	pMainFrame->showCurOppOnly->setChecked(m_bCurOppOnly);

	pMainFrame->setCpVarGraph->setChecked(m_CpGraph.GetYVariable()==0);
	pMainFrame->setQVarGraph->setChecked(m_CpGraph.GetYVariable()==1);

	for(int ig=0; ig<MAXPOLARGRAPHS; ig++)
		pMainFrame->PolarGraphAct[ig]->setChecked(m_iPlrView==ONEGRAPH && m_iPlrGraph== ig);

	pMainFrame->TwoPolarGraphsAct->setChecked(m_iPlrView==TWOGRAPHS);
	pMainFrame->AllPolarGraphsAct->setChecked(m_iPlrView==ALLGRAPHS);

	int OppVar = m_CpGraph.GetYVariable();
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

	m_pctrlShowPressure->setEnabled(!m_bPolarView && OpPoint::curOpp());
	m_pctrlShowBL->setEnabled(!m_bPolarView && OpPoint::curOpp());
	m_pctrlAnimate->setEnabled(!m_bPolarView && OpPoint::curOpp());
	m_pctrlAnimateSpeed->setEnabled(!m_bPolarView && OpPoint::curOpp());
//	m_pctrlHighlightOpp->setEnabled(m_bPolar);

	pMainFrame->currentFoilMenu->setEnabled(Foil::curFoil());
	pMainFrame->currentPolarMenu->setEnabled(Polar::curPolar());

	pMainFrame->renameCurFoil->setEnabled(Foil::curFoil());
	pMainFrame->pDirectDuplicateCurFoil->setEnabled(Foil::curFoil());
	pMainFrame->deleteCurFoil->setEnabled(Foil::curFoil());
	pMainFrame->exportCurFoil->setEnabled(Foil::curFoil());
	pMainFrame->renameCurFoil->setEnabled(Foil::curFoil());
	pMainFrame->setCurFoilStyle->setEnabled(Foil::curFoil());
	pMainFrame->definePolar->setEnabled(Foil::curFoil());
	pMainFrame->defineBatch->setEnabled(Foil::curFoil());
	pMainFrame->deleteFoilOpps->setEnabled(Foil::curFoil());
	pMainFrame->deleteFoilPolars->setEnabled(Foil::curFoil());

	pMainFrame->editCurPolar->setEnabled(Polar::curPolar());
	pMainFrame->deletePolar->setEnabled(Polar::curPolar());
	pMainFrame->exportCurPolar->setEnabled(Polar::curPolar());
	pMainFrame->hidePolarOpps->setEnabled(Polar::curPolar());
	pMainFrame->showPolarOpps->setEnabled(Polar::curPolar());
	pMainFrame->deletePolarOpps->setEnabled(Polar::curPolar());

	pMainFrame->DerotateFoil->setEnabled(Foil::curFoil());
	pMainFrame->NormalizeFoil->setEnabled(Foil::curFoil());
	pMainFrame->RefineLocalFoil->setEnabled(Foil::curFoil());
	pMainFrame->RefineGlobalFoil->setEnabled(Foil::curFoil());
	pMainFrame->EditCoordsFoil->setEnabled(Foil::curFoil());
	pMainFrame->ScaleFoil->setEnabled(Foil::curFoil());
	pMainFrame->SetLERadius->setEnabled(Foil::curFoil());
	pMainFrame->SetTEGap->setEnabled(Foil::curFoil());
	pMainFrame->SetFlap->setEnabled(Foil::curFoil());
	pMainFrame->InterpolateFoils->setEnabled(Foil::curFoil());

	pMainFrame->currentOppMenu->setEnabled(OpPoint::curOpp());
	pMainFrame->deleteCurOpp->setEnabled(OpPoint::curOpp());
	pMainFrame->exportCurOpp->setEnabled(OpPoint::curOpp());
}


/**
* Connects signals and slots
*/
void QXDirect::Connect()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	connect(this, SIGNAL(projectModified()), pMainFrame, SLOT(OnProjectModified()));
	connect(m_pctrlSpec1, SIGNAL(clicked()), this, SLOT(OnSpec()));
	connect(m_pctrlSpec2, SIGNAL(clicked()), this, SLOT(OnSpec()));
	connect(m_pctrlSpec3, SIGNAL(clicked()), this, SLOT(OnSpec()));
	connect(m_pctrlAnalyze, SIGNAL(clicked()), this, SLOT(OnAnalyze()));
	connect(m_pctrlAlphaMin, SIGNAL(editingFinished()), this, SLOT(OnInputChanged()));
	connect(m_pctrlAlphaMax, SIGNAL(editingFinished()), this, SLOT(OnInputChanged()));
	connect(m_pctrlAlphaDelta, SIGNAL(editingFinished()), this, SLOT(OnInputChanged()));
	connect(m_pctrlCurveStyle, SIGNAL(activated(int)), this, SLOT(OnCurveStyle(int)));
	connect(m_pctrlCurveWidth, SIGNAL(activated(int)), this, SLOT(OnCurveWidth(int)));
	connect(m_pctrlCurveColor, SIGNAL(clickedLB()), this, SLOT(OnCurveColor()));
	connect(m_pctrlSequence, SIGNAL(clicked()), this, SLOT(OnSequence()));
	connect(m_pctrlViscous, SIGNAL(clicked()), this, SLOT(OnViscous()));
	connect(m_pctrlShowBL, SIGNAL(clicked()), this, SLOT(OnShowBL()));
	connect(m_pctrlShowPressure, SIGNAL(clicked()), this, SLOT(OnShowPressure()));
	connect(m_pctrlStoreOpp, SIGNAL(clicked()), this, SLOT(OnStoreOpp()));
	connect(m_pctrlShowPoints, SIGNAL(clicked()), this, SLOT(OnShowCurvePoints()));
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
	if(pOpp) pOpPoint = pOpp; else pOpPoint = OpPoint::curOpp();

	Curve *pCurve1;
	QString str;
	int k;

	m_CpGraph.DeleteCurves();

	if(m_bCurOppOnly && pOpPoint)
	{
		if(!pOpPoint || !pOpPoint->m_bIsVisible) return;
		pCurve1    = m_CpGraph.AddCurve();
		if(pOpPoint->m_bShowPoints) pCurve1->ShowPoints(true);
		
		pCurve1->SetStyle(pOpPoint->m_Style);
		pCurve1->SetColor(pOpPoint->m_Color);
		pCurve1->SetWidth(pOpPoint->m_Width);

		str = QString("-Re=%1-Alpha=%2").arg(pOpPoint->Reynolds,8,'f',0).arg(pOpPoint->Alpha,5,'f',2);
		str = pOpPoint->m_strFoilName+str;
		pCurve1->SetTitle(str);

		FillOppCurve(pOpPoint, &m_CpGraph, pCurve1);

		if(m_bShowInviscid && pOpPoint)
		{
			Curve *pCpi = m_CpGraph.AddCurve();
			if(pOpPoint->m_bShowPoints) pCpi->ShowPoints(true);
			pCpi->SetStyle(1);
			pCpi->SetColor(pOpPoint->m_Color.darker(150));
			pCpi->SetWidth(pOpPoint->m_Width);
			str= QString("-Re=%1-Alpha=%2_Inviscid").arg(pOpPoint->Reynolds,8,'f',0).arg(pOpPoint->Alpha,5,'f',2);
			str = pOpPoint->m_strFoilName+str;
			pCpi->SetTitle(str);
			FillOppCurve(pOpPoint, &m_CpGraph, pCpi, true);
		}
	}
	else if(!m_bCurOppOnly)
	{
		for (k=0; k<m_poaOpp->size(); k++)
		{
			pOpp = (OpPoint*)m_poaOpp->at(k);
			if (pOpp && pOpp->m_bIsVisible)
			{
				pCurve1    = m_CpGraph.AddCurve();

				if(pOpp->m_bShowPoints) pCurve1->ShowPoints(true);

				pCurve1->SetStyle(pOpp->m_Style);
				pCurve1->SetColor(pOpp->m_Color);
				pCurve1->SetWidth(pOpp->m_Width);

				str= QString("-Re=%1-Alpha=%2").arg(pOpp->Reynolds,8,'f',0).arg(pOpp->Alpha,5,'f',2);
				str = pOpp->m_strFoilName+str;
				pCurve1->SetTitle(str);

				FillOppCurve(pOpp, &m_CpGraph, pCurve1);
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

	Foil *pOpFoil = Foil::foil(pOpp->m_strFoilName);

	switch(m_CpGraph.GetYVariable())
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
		if(OpPoint::curOpp() && m_bHighlightOpp)
		{
			if(qAbs(pPolar->m_Alpha[i]-OpPoint::curOpp()->Alpha)<0.0001)
			{
				if(pPolar->m_PlrName==OpPoint::curOpp()->m_strPlrName  && Foil::curFoil()->foilName()==pPolar->foilName())
				{

					pCurve->SetSelected(i);
				}
			}
		}
	}
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
		if(m_iPlrView == ONEGRAPH)
		{
			if(m_iPlrGraph>=0 && m_iPlrGraph<MAXPOLARGRAPHS) m_pCurGraph = m_PlrGraph + m_iPlrGraph;
			else                                             m_pCurGraph = NULL;

			if(m_pCurGraph->IsInDrawRect(pt)) return m_pCurGraph;
			else return NULL;
		}
		if(m_iPlrView == TWOGRAPHS)
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
		if(m_CpGraph.IsInDrawRect(pt))
		{
			if(m_bCpGraph) return &m_CpGraph;
			else           return NULL;
		}
		else return NULL;
	}
	return NULL;
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
	if(event->modifiers() & Qt::ShiftModifier)   bShift =true;

	switch (event->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
			if (event->modifiers().testFlag(Qt::AltModifier) & event->modifiers().testFlag(Qt::ShiftModifier))
			{
				OnOpPointProps();
				break;
			}
			else if (event->modifiers().testFlag(Qt::AltModifier))
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
			m_iPlrView  = ONEGRAPH;
			m_iPlrGraph = 0;
			if(m_bPolarView) SetPolarLegendPos();
			SetControls();
			UpdateView();
			break;
		case Qt::Key_2:
			m_iPlrView  = ONEGRAPH;
			m_iPlrGraph = 1;
			if(m_bPolarView) SetPolarLegendPos();
			SetControls();
			UpdateView();
			break;
		case Qt::Key_3:
			m_iPlrView  = ONEGRAPH;
			m_iPlrGraph = 2;
			if(m_bPolarView) SetPolarLegendPos();
			SetControls();
			UpdateView();
			break;
		case Qt::Key_4:
			m_iPlrView  = ONEGRAPH;
			m_iPlrGraph = 3;
			if(m_bPolarView) SetPolarLegendPos();
			SetControls();
			UpdateView();
			break;
		case Qt::Key_5:
			m_iPlrView  = ONEGRAPH;
			m_iPlrGraph = 4;
			if(m_bPolarView) SetPolarLegendPos();
			SetControls();
			UpdateView();
			break;
		case Qt::Key_A:
			m_iPlrView  = ALLGRAPHS;
			if(m_bPolarView) SetPolarLegendPos();
			SetControls();
			UpdateView();
			break;
		case Qt::Key_T:
			m_iPlrView  = TWOGRAPHS;
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
			OnRenameCurFoil();
			break;
		}
		case Qt::Key_F3:
		{
			if(bShift) OnCadd();
			else       OnRefinePanelsGlobally();
			break;
		}
		case Qt::Key_F5:
		{
			if(!m_bPolarView) return;
			OnOpPointView();
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
			OnPolarView();
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

	event->accept();
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

	event->accept();
}


/**
 * Loads the user's default settings from the application QSettings object
 * @param pSettings a pointer to the QSettings object
 */
void QXDirect::LoadSettings(QSettings *pSettings)
{
	QString str1, str2, str3;
	int b;

	pSettings->beginGroup("XDirect");
	{
		OpPoint::s_bStoreOpp       = pSettings->value("StoreOpp").toBool();
		s_bAlpha          = pSettings->value("AlphaSpec").toBool();
		s_bViscous        = pSettings->value("ViscousAnalysis").toBool();
		s_bInitBL         = pSettings->value("InitBL").toBool();
		m_bBL             = pSettings->value("BoundaryLayer").toBool();
		m_bPressure       = pSettings->value("Pressure").toBool();
		m_bPolarView      = pSettings->value("PolarView").toBool();
		m_bShowUserGraph  = pSettings->value("UserGraph").toBool();
		m_bShowPanels     = pSettings->value("ShowPanels").toBool();
		m_bType1          = pSettings->value("Type1").toBool();
		m_bType2          = pSettings->value("Type2").toBool();
		m_bType3          = pSettings->value("Type3").toBool();
		m_bType4          = pSettings->value("Type4").toBool();
		m_bFromList       = pSettings->value("FromList").toBool();
		s_bFromZero       = pSettings->value("FromZero").toBool();
		m_bShowTextOutput = pSettings->value("TextOutput").toBool();
		m_bNeutralLine    = pSettings->value("NeutralLine").toBool();
		m_bCurOppOnly     = pSettings->value("CurOppOnly").toBool();
		m_bShowInviscid   = pSettings->value("ShowInviscid", false).toBool();
		m_bCpGraph        = pSettings->value("ShowCpGraph", true).toBool();
		m_bSequence       = pSettings->value("Sequence", false).toBool();
		m_bHighlightOpp   = pSettings->value("HighlightOpp", false).toBool();
		m_bHighlightOpp = false;

		m_crBLColor = pSettings->value("BLColor").value<QColor>();
		m_iBLStyle = pSettings->value("BLStyle").toInt();
		m_iBLWidth = pSettings->value("BLWidth").toInt();

		m_crPressureColor = pSettings->value("PressureColor").value<QColor>();
		m_iPressureStyle = pSettings->value("PressureStyle").toInt();
		m_iPressureWidth = pSettings->value("PressureWidth").toInt();

		m_crNeutralColor = pSettings->value("NeutralColor").value<QColor>();
		m_iNeutralStyle = pSettings->value("NeutralStyle").toInt();
		m_iNeutralWidth = pSettings->value("NeutralWidth").toInt();

		m_XFoilVar       = pSettings->value("XFoilVar").toInt();
		s_TimeUpdateInterval = pSettings->value("TimeUpdateInterval",100).toInt();

		BatchThreadDlg::s_bUpdatePolarView = pSettings->value("BatchUpdatePolarView", false).toBool();


		m_iPlrGraph      = pSettings->value("PlrGraph").toInt();

		switch(pSettings->value("PlrView").toInt())
		{
			case 1:
				m_iPlrView = ONEGRAPH;
				break;
			case 2:
				m_iPlrView = TWOGRAPHS;
				break;
			default:
				m_iPlrView = ALLGRAPHS;
				break;
		}

		m_Alpha           = pSettings->value("AlphaMin").toDouble();
		m_AlphaMax        = pSettings->value("AlphaMax").toDouble();
		m_AlphaDelta      = pSettings->value("AlphaDelta").toDouble();
		m_Cl              = pSettings->value("ClMin").toDouble();
		m_ClMax           = pSettings->value("ClMax").toDouble();
		m_ClDelta         = pSettings->value("ClDelta").toDouble();
		m_Reynolds        = pSettings->value("ReynoldsMin").toDouble();
		m_ReynoldsMax     = pSettings->value("ReynoldsMax").toDouble();
		m_ReynoldsDelta   = pSettings->value("ReynolsDelta").toDouble();
		m_pXFoil->vaccel  = pSettings->value("VAccel").toDouble();
		s_bKeepOpenErrors = pSettings->value("KeepOpenErrors").toBool();

		XFoilTask::s_bAutoInitBL    = pSettings->value("AutoInitBL").toBool();
		XFoilTask::s_IterLim        = pSettings->value("IterLim", 100).toInt();

		XFoil::s_bFullReport = pSettings->value("FullReport").toBool();

		s_refPolar.m_ACrit    = pSettings->value("NCrit").toDouble();
		s_refPolar.m_XTop     = pSettings->value("XTopTr").toDouble();
		s_refPolar.m_XBot     = pSettings->value("XBotTr").toDouble();
		s_refPolar.m_Mach     = pSettings->value("Mach").toDouble();
		s_refPolar.m_ASpec    = pSettings->value("ASpec").toDouble();

		b = pSettings->value("Type").toInt();
		if(b==1)      s_refPolar.setPolarType(FIXEDSPEEDPOLAR);
		else if(b==2) s_refPolar.setPolarType(FIXEDLIFTPOLAR);
		else if(b==3) s_refPolar.setPolarType(RUBBERCHORDPOLAR);
		else if(b==4) s_refPolar.setPolarType(FIXEDAOAPOLAR);


		int NRe = pSettings->value("NReynolds").toInt();
		s_ReList.clear();
		s_MachList.clear();
		s_NCritList.clear();
		for (int i=0; i<NRe; i++)
		{
			str1 = QString("ReList%1").arg(i);
			str2 = QString("MaList%1").arg(i);
			str3 = QString("NcList%1").arg(i);
			s_ReList.append(pSettings->value(str1).toDouble());
			s_MachList.append(pSettings->value(str2).toDouble());
			s_NCritList.append(pSettings->value(str3).toDouble());
		}
	}
	pSettings->endGroup();

	m_PlrGraph[0].LoadSettings(pSettings);
	m_PlrGraph[1].LoadSettings(pSettings);
	m_PlrGraph[2].LoadSettings(pSettings);
	m_PlrGraph[3].LoadSettings(pSettings);
	m_PlrGraph[4].LoadSettings(pSettings);

	m_CpGraph.LoadSettings(pSettings);

	if(m_CpGraph.GetYVariable() == 0 || m_CpGraph.GetYVariable()>=2)
	{
		m_CpGraph.SetYTitle(tr("Cp"));
		m_CpGraph.SetInverted(true);
	}
	else
	{
		m_CpGraph.SetYTitle(tr("Q"));
		m_CpGraph.SetInverted(false);
	}

	for(int ig=0; ig<MAXPOLARGRAPHS; ig++) SetGraphTitles(m_PlrGraph+ig);
}


/**
 * Overrides the QWidget's mouseDoubleClickEvent method.
 * Dispatches the event
 * @param event the QMouseEvent
 */
void QXDirect::doubleClickEvent(QPoint pos)
{	
	if(!m_bPolarView)
	{
		if (!m_CpGraph.IsInDrawRect(pos)) return;
	}

	m_pCurGraph = GetGraph(pos);
	if(!m_pCurGraph) return;

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
		else if (Foil::curFoil()  && !m_bPolarView)
		{
			// we translate the airfoil
			
			m_FoilOffset.rx() += pt.x()-m_PointDown.x();
			m_FoilOffset.ry() += pt.y()-m_PointDown.y();
			if(!m_bAnimate) UpdateView();
		}
		m_PointDown = pt;
	}

	else if (Foil::curFoil() && ((event->buttons() & Qt::MidButton) || event->modifiers().testFlag(Qt::AltModifier)))
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
		else if(Foil::curFoil() && !m_bPolarView)
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

	event->accept();
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
		if (Foil::curFoil() || (m_pCurGraph && m_pCurGraph->IsInDrawRect(pt) && m_bCpGraph))
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

	event->accept();
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

	event->accept();
}



/**
 * The user has requested a display of all polar graphs
 */
void QXDirect::OnAllPolarGraphs()
{
	m_iPlrView  = ALLGRAPHS;
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

	GraphDlg grDlg((MainFrame*)s_pMainFrame);

	grDlg.m_pMemGraph = &graph;
	grDlg.m_pGraph    = m_PlrGraph;
	grDlg.m_GraphArray[0] = m_PlrGraph;
	grDlg.m_GraphArray[1] = m_PlrGraph+1;
	grDlg.m_GraphArray[2] = m_PlrGraph+2;
	grDlg.m_GraphArray[3] = m_PlrGraph+3;
	grDlg.m_GraphArray[4] = m_PlrGraph+4;
	grDlg.m_NGraph = 5;
	grDlg.SetParams();

	if(grDlg.exec() == QDialog::Accepted)
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
	if(!Foil::curFoil() || !Polar::curPolar())
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
				pOpPoint->m_strPlrName  == Polar::curPolar()->polarName() &&
				pOpPoint->m_strFoilName == Foil::curFoil()->foilName())
			{
					if(qAbs(OpPoint::curOpp()->Alpha - pOpPoint->Alpha)<0.0001)
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
 * Called by the animation timer.
 * Updates the display with the data of the next OpPoint.
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
			pOpPoint->m_strPlrName  == Polar::curPolar()->polarName() &&
			pOpPoint->m_strFoilName == Foil::curFoil()->foilName() &&
			pOpPoint != OpPoint::curOpp())
		{
			bIsValid = true;
			CreateOppCurves(pOpPoint);
			OpPoint::setCurOpp(pOpPoint);

			//select current OpPoint in Combobox
			if(Polar::curPolar()->polarType()!=FIXEDAOAPOLAR) str = QString("%1").arg(OpPoint::curOpp()->Alpha,8,'f',2);
			else                                              str = QString("%1").arg(OpPoint::curOpp()->Reynolds,8,'f',2);
			indexCbBox = pMainFrame->m_pctrlOpPoint->findText(str);
			if(indexCbBox>=0) pMainFrame->m_pctrlOpPoint->setCurrentIndex(indexCbBox);

			UpdateView();
		}
	}
}


/**
 * the user has moved the slider which defines the animation speed
 * @param val the slider's new position
 */
void QXDirect::OnAnimateSpeed(int val)
{
	if(m_pAnimateTimer->isActive())
	{
		m_pAnimateTimer->setInterval(1000-val);
	}
}



/**
 * The user has clicked the analyze button.
 *
 * Reads the input parameters, initializes the analysis dialog box, and starts the analysis.
 */
void QXDirect::OnAnalyze()
{
	if(!Foil::curFoil() || !Polar::curPolar()) return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	ReadParams();

	m_pctrlAnalyze->setEnabled(false);

	bool bHigh = m_bHighlightOpp;
	m_bHighlightOpp = false;

	XFoilAnalysisDlg *pXFADlg = new XFoilAnalysisDlg;

	if(m_bSequence)
	{
		pXFADlg->SetAlpha(m_Alpha, m_AlphaMax, m_AlphaDelta);
		pXFADlg->SetCl(m_Cl, m_ClMax, m_ClDelta);
		pXFADlg->SetRe(m_Reynolds, m_ReynoldsMax, m_ReynoldsDelta);
	}
	else
	{
		pXFADlg->SetAlpha(m_Alpha, m_Alpha, m_AlphaDelta);
		pXFADlg->SetCl(m_Cl, m_Cl, m_ClDelta);
		pXFADlg->SetRe(m_Reynolds, m_Reynolds, m_ReynoldsDelta);
	}

	pXFADlg->m_bAlpha = s_bAlpha;

	pXFADlg->InitDialog();
	pXFADlg->show();
	pXFADlg->Analyze();

	if(!s_bKeepOpenErrors || !pXFADlg->m_bErrors) pXFADlg->hide();

	delete pXFADlg;

	// and update window
	emit projectModified();

	m_pctrlAnalyze->setEnabled(true);

	s_bInitBL = !m_pXFoil->lblini;
	m_pctrlInitBL->setChecked(s_bInitBL);;

	m_pCurGraph = &m_CpGraph;

	pMainFrame->UpdateOppListBox();

	SetOpp();

	m_bHighlightOpp = bHigh;

	if(m_bPolarView) CreatePolarCurves();

	emit projectModified();

	SetControls();
	UpdateView();
}



/**
 * Launches a single-threaded batch analysis
 */
void QXDirect::OnBatchAnalysis()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(!Foil::curFoil()) return;

	m_bPolarView = true;
	OnPolarView();
	UpdateView();

	m_pctrlAnalyze->setEnabled(false);

	BatchDlg *pBatchDlg = new BatchDlg;
	pBatchDlg->m_pFoil     = Foil::curFoil();
	pBatchDlg->m_bAlpha    = true;

	pBatchDlg->m_SpMin     = m_Alpha;
	pBatchDlg->m_SpMax     = m_AlphaMax;
	pBatchDlg->m_SpInc     = m_AlphaDelta;
	pBatchDlg->m_AlphaMin  = m_Alpha;
	pBatchDlg->m_AlphaMax  = m_AlphaMax;
	pBatchDlg->m_AlphaInc  = m_AlphaDelta;
	pBatchDlg->m_ClMin     = m_Cl;
	pBatchDlg->m_ClMax     = m_ClMax;
	pBatchDlg->m_ClInc     = m_ClDelta;
	pBatchDlg->m_ReMin     = m_Reynolds;
	pBatchDlg->m_ReMax     = m_ReynoldsMax;
	pBatchDlg->m_ReInc     = m_ReynoldsDelta;

	pBatchDlg->m_bFromList = m_bFromList;
	pBatchDlg->m_bFromZero = s_bFromZero;
	pBatchDlg->InitDialog();

	if(pBatchDlg->exec()==QDialog::Accepted) emit projectModified();

	m_Reynolds         = pBatchDlg->m_ReMin;
	m_ReynoldsMax      = pBatchDlg->m_ReMax;
	m_ReynoldsDelta    = pBatchDlg->m_ReInc;
	m_Alpha            = pBatchDlg->m_AlphaMin;
	m_AlphaMax         = pBatchDlg->m_AlphaMax;
	m_AlphaDelta       = pBatchDlg->m_AlphaInc;
	m_Cl               = pBatchDlg->m_ClMin;
	m_ClMax            = pBatchDlg->m_ClMax;
	m_ClDelta          = pBatchDlg->m_ClInc;
	s_bAlpha           = pBatchDlg->m_bAlpha;
	m_bFromList        = pBatchDlg->m_bFromList;
	s_bFromZero        = pBatchDlg->m_bFromZero;

	delete pBatchDlg;

	SetPolar();
	pMainFrame->UpdatePolarListBox();

	m_pctrlAnalyze->setEnabled(true);

	SetOpp();

	emit projectModified();

	SetControls();
	UpdateView();
}


/**
 * Launches a multi-threaded batch analysis
 *
 */
void QXDirect::OnMultiThreadedBatchAnalysis()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(!Foil::curFoil()) 		return;

	if(QThread::idealThreadCount()<2)
	{
		QString strange = tr("Not enough threads available for multithreading");
		QMessageBox::warning(pMainFrame, tr("Warning"), strange);
		return;
	}

	m_bPolarView = true;
	OnPolarView();
	UpdateView();

	m_pctrlAnalyze->setEnabled(false);

	BatchThreadDlg *pBatchThreadDlg   = new BatchThreadDlg;

	pBatchThreadDlg->m_pCurFoil  = Foil::curFoil();

	pBatchThreadDlg->m_bAlpha    = true;
	pBatchThreadDlg->m_AlphaMin  = m_Alpha;
	pBatchThreadDlg->m_AlphaMax  = m_AlphaMax;
	pBatchThreadDlg->m_AlphaInc  = m_AlphaDelta;
	pBatchThreadDlg->m_ClMin     = m_Cl;
	pBatchThreadDlg->m_ClMax     = m_ClMax;
	pBatchThreadDlg->m_ClInc     = m_ClDelta;
	pBatchThreadDlg->m_ReMin     = m_Reynolds;
	pBatchThreadDlg->m_ReMax     = m_ReynoldsMax;
	pBatchThreadDlg->m_ReInc     = m_ReynoldsDelta;

	pBatchThreadDlg->m_bFromList = m_bFromList;
	pBatchThreadDlg->m_bFromZero = s_bFromZero;
	pBatchThreadDlg->InitDialog();

	pBatchThreadDlg->exec();

	m_Reynolds         = pBatchThreadDlg->m_ReMin;
	m_ReynoldsMax      = pBatchThreadDlg->m_ReMax;
	m_ReynoldsDelta    = pBatchThreadDlg->m_ReInc;
	m_Alpha            = pBatchThreadDlg->m_AlphaMin;
	m_AlphaMax         = pBatchThreadDlg->m_AlphaMax;
	m_AlphaDelta       = pBatchThreadDlg->m_AlphaInc;
	m_Cl               = pBatchThreadDlg->m_ClMin;
	m_ClMax            = pBatchThreadDlg->m_ClMax;
	m_ClDelta          = pBatchThreadDlg->m_ClInc;
	s_bAlpha           = pBatchThreadDlg->m_bAlpha;
	m_bFromList        = pBatchThreadDlg->m_bFromList;
	s_bFromZero        = pBatchThreadDlg->m_bFromZero;

	delete pBatchThreadDlg;

	SetPolar();
	pMainFrame->UpdatePolarListBox();

	m_pctrlAnalyze->setEnabled(true);


	pMainFrame->UpdateOppListBox();

	SetOpp();


	SetControls();
	UpdateView();

	emit projectModified();

}



/**
 * The user has requested the plot of the Cf variable using current XFoil results.
 */
void QXDirect::OnCfPlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;
	double x[IVX][3],y[IVX][3];
	int nside1, nside2, ibl;

	m_CpGraph.SetYVariable(2);
	m_XFoilVar = 8;
	m_CpGraph.DeleteCurves();
	m_CpGraph.ResetLimits();
	m_CpGraph.SetAuto(true);
	m_CpGraph.SetInverted(false);
	m_CpGraph.SetYTitle(tr("Cf"));
	Curve * pTopCurve = m_CpGraph.AddCurve();
	Curve * pBotCurve = m_CpGraph.AddCurve();
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
	m_CpGraph.SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}



/**
 * The user has requested the plot of the Ct variable using current XFoil results.
 */
void QXDirect::OnCtPlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_CpGraph.SetYVariable(2);
	m_XFoilVar=1;
	m_CpGraph.DeleteCurves();
	m_CpGraph.ResetLimits();
	m_CpGraph.SetAuto(true);
	m_CpGraph.SetInverted(false);
	m_CpGraph.SetYTitle(tr("Max Shear"));
	Curve * pCurve0 = m_CpGraph.AddCurve();
	Curve * pCurve1 = m_CpGraph.AddCurve();
	Curve * pCurve2 = m_CpGraph.AddCurve();
	Curve * pCurve3 = m_CpGraph.AddCurve();
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

	m_CpGraph.SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();

}


/**
 * The user has requested the plot of the Dt variable using current XFoil results.
 */
void QXDirect::OnDtPlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_CpGraph.SetYVariable(2);
	m_XFoilVar=3;
	m_CpGraph.DeleteCurves();
	m_CpGraph.ResetLimits();
	m_CpGraph.SetAuto(true);
	m_CpGraph.SetInverted(false);
	m_CpGraph.SetYTitle(" ");


	double x[IVX][3];
	int nside1, nside2;

	Curve * pCurve1 = m_CpGraph.AddCurve();
	Curve * pCurve2 = m_CpGraph.AddCurve();

	pCurve1->SetTitle("D*");
	pCurve2->SetTitle("Theta");
	m_pXFoil->CreateXBL(x, nside1, nside2);

	for (i=2; i<nside1; i++){
		pCurve1->AppendPoint(x[i][1], m_pXFoil->dstr[i][1]);
		pCurve2->AppendPoint(x[i][1], m_pXFoil->thet[i][1]);
	}

	m_CpGraph.SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}


/**
 * The user has requested the plot of the Db variable using current XFoil results.
 */
void QXDirect::OnDbPlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_CpGraph.SetYVariable(2);
	m_XFoilVar = 2;
	m_CpGraph.DeleteCurves();
	m_CpGraph.ResetLimits();
	m_CpGraph.SetAuto(true);
	m_CpGraph.SetInverted(false);
	m_CpGraph.SetYTitle(" ");


	double x[IVX][3];
	int nside1, nside2;

	Curve * pCurve1 = m_CpGraph.AddCurve();
	Curve * pCurve2 = m_CpGraph.AddCurve();

	pCurve1->SetTitle("D*");
	pCurve2->SetTitle("Theta");
	m_pXFoil->CreateXBL(x, nside1, nside2);

	for (i=2; i<nside2; i++)
	{
		pCurve1->AppendPoint(x[i][2], m_pXFoil->dstr[i][2]);
		pCurve2->AppendPoint(x[i][2], m_pXFoil->thet[i][2]);
	}

	m_CpGraph.SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}


/**
 * The user has requested the plot of the Cd variable using current XFoil results.
 */
void QXDirect::OnCdPlot()
{
	if(!m_pXFoil->lvconv) return;
	double x[IVX][3],y[IVX][3];
	int nside1, nside2, ibl;
	int i;

	m_CpGraph.SetYVariable(2);
	m_XFoilVar = 7;
	m_CpGraph.DeleteCurves();
	m_CpGraph.ResetLimits();
	m_CpGraph.SetAuto(true);
	m_CpGraph.SetInverted(false);
	m_CpGraph.SetYTitle(tr("Cd'"));
	Curve * pTopCurve = m_CpGraph.AddCurve();
	Curve * pBotCurve = m_CpGraph.AddCurve();
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
	m_CpGraph.SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}


/**
 * The user has requested the plot of the Hk variable using current XFoil results.
 */
void QXDirect::OnHPlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_CpGraph.SetYVariable(2);
	m_XFoilVar = 10;
	m_CpGraph.DeleteCurves();
	m_CpGraph.ResetLimits();
	m_CpGraph.SetAuto(true);
	m_CpGraph.SetInverted(false);
	m_CpGraph.SetYTitle("Hk");
	Curve * pTopCurve = m_CpGraph.AddCurve();
	Curve * pBotCurve = m_CpGraph.AddCurve();
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

	m_CpGraph.SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}


/**
 * The user has requested the plot of the Rt variable using current XFoil results.
 */
void QXDirect::OnRtPlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_CpGraph.SetYVariable(2);
	m_XFoilVar=5;
	m_CpGraph.DeleteCurves();
	m_CpGraph.ResetLimits();
	m_CpGraph.SetAuto(true);
	m_CpGraph.SetInverted(false);
	m_CpGraph.SetYTitle("Re_Theta");
	Curve * pTopCurve = m_CpGraph.AddCurve();
	Curve * pBotCurve = m_CpGraph.AddCurve();
	pTopCurve->SetTitle(tr("Top"));
	pBotCurve->SetTitle(tr("Bot"));

	double x[IVX][3],y[IVX][3];
	int nside1, nside2;

	m_pXFoil->CreateXBL(x, nside1, nside2);
	m_pXFoil->FillRTheta(y, nside1, nside2);

	for (i=2; i<=nside1-1; i++)	pTopCurve->AppendPoint(x[i][1], y[i][1]);
	for (i=2; i<=nside2-1; i++) pBotCurve->AppendPoint(x[i][2], y[i][2]);

	m_CpGraph.SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}


/**
 * The user has requested the plot of the RtL variable using current XFoil results.
 */
void QXDirect::OnRtLPlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;

	m_CpGraph.SetYVariable(2);
	m_XFoilVar=4;
	m_CpGraph.DeleteCurves();
	m_CpGraph.ResetLimits();
	m_CpGraph.SetAuto(true);
	m_CpGraph.SetInverted(false);
	m_CpGraph.SetYTitle("Re_Theta");
	Curve * pTopCurve = m_CpGraph.AddCurve();
	Curve * pBotCurve = m_CpGraph.AddCurve();
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
	m_CpGraph.SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}


/**
 * The user has requested the plot of the Ue variable using current XFoil results.
 */
void QXDirect::OnUePlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;
	double x[IVX][3],y[IVX][3];
	double uei;
	int nside1, nside2, ibl;

	m_CpGraph.SetYVariable(2);
	m_XFoilVar = 9;
	m_CpGraph.DeleteCurves();
	m_CpGraph.ResetLimits();
	m_CpGraph.SetAuto(true);
	m_CpGraph.SetInverted(false);
	m_CpGraph.SetYTitle("Ue/Vinf");
	Curve * pTopCurve = m_CpGraph.AddCurve();
	Curve * pBotCurve = m_CpGraph.AddCurve();
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
	m_CpGraph.SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}


/**
 * The user has requested to switch to the two polar graph view
 */
void QXDirect::OnCouplePolarGraphs()
{
	m_iPlrView  = TWOGRAPHS;
	m_bPolarView = true;
	SetControls();
	UpdateView();
}


/**
 * The user has requested to switch to the Cp graph view
 */
void QXDirect::OnCpGraph()
{
	m_bPolarView = false;
	if(m_CpGraph.GetYVariable()!=0)
	{
//		m_pCpGraph->ResetLimits();
		m_CpGraph.SetAuto(true);
		m_CpGraph.SetYVariable(0);
	}

	m_CpGraph.SetInverted(true);
	CreateOppCurves();
	m_CpGraph.SetYTitle(tr("Cp"));

	SetControls();
	m_CpGraph.SetXScale();
	SetFoilScale();
	UpdateView();
}


/**
 * The user has requested an edition of the settings of the Cp graph
 */
void QXDirect::OnCpGraphSettings()
{
	QGraph graph;
	graph.CopySettings(&m_CpGraph);
	GraphDlg grDlg((MainFrame*)s_pMainFrame);
	grDlg.m_pMemGraph = m_pCurGraph;
	grDlg.m_pGraph = &graph;
	grDlg.SetParams();

	if(grDlg.exec() == QDialog::Accepted)
	{
		m_CpGraph.CopySettings(&graph);
	}

	UpdateView();
}


/**
 * The user has toggled the request for the display of the inviscid Cp curve
 */
void QXDirect::OnCpi()
{
	m_bShowInviscid = !m_bShowInviscid;

	CreateOppCurves();
	SetControls();
	UpdateView();
}


/**
 * The user has toggled the switch for the display of the current OpPoint only
 */
void QXDirect::OnCurOppOnly()
{
	m_bCurOppOnly = !m_bCurOppOnly;
	((MainFrame*)s_pMainFrame)->showCurOppOnly->setChecked(m_bCurOppOnly);

	if(OpPoint::curOpp()) OpPoint::curOpp()->m_bIsVisible = true;
	CreateOppCurves();
	SetAnalysisParams();
	UpdateView();
}


/**
 * The user has changed the color of the current curve
 */
void QXDirect::OnCurveColor()
{
	QColor Color = QColorDialog::getColor(m_CurveColor);
	if(Color.isValid()) m_CurveColor = Color;

	FillComboBoxes();
	UpdateCurveStyle();
}


/**
 * The user has changed the style of the current curve
 */
void QXDirect::OnCurveStyle(int index)
{
	m_CurveStyle = index;
	FillComboBoxes();
	UpdateCurveStyle();
}


/**
 * The user has changed the width of the current curve
 */
void QXDirect::OnCurveWidth(int index)
{
	m_CurveWidth = index+1;
	FillComboBoxes();
	UpdateCurveStyle();
}


/**
 * The user has requested to define a new polar
 */
void QXDirect::OnDefinePolar()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(!Foil::curFoil()) return;

	FoilPolarDlg fpDlg(pMainFrame);

	fpDlg.InitDialog();

	int res = fpDlg.exec();
	if (res == QDialog::Accepted)
	{
		Polar::setCurPolar(new Polar());
		Polar::curPolar()->m_FoilName = Foil::curFoil()->foilName();
		Polar::curPolar()->m_PlrName = fpDlg.m_PlrName;
		Polar::curPolar()->m_bIsVisible = true;
		Polar::curPolar()->CopySpecification(&s_refPolar);

		Polar::curPolar()->setPolarType(fpDlg.m_PolarType);

		Polar::addPolar(Polar::curPolar());
		SetPolar(Polar::curPolar());

		pMainFrame->UpdatePolarListBox();
		SetBufferFoil();
		UpdateView();
		emit projectModified();
	}
	SetControls();
}


/**
 * The user has requested the deletion of the current Foil.
 * Deletes the Foil, and selects the next one in the array, if any.
 */
void QXDirect::OnDeleteCurFoil()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	QString strong;
	strong = tr("Are you sure you want to delete")  +"\n"+ Foil::curFoil()->foilName() +"\n";
	strong+= tr("and all associated OpPoints and Polars ?");

	int resp = QMessageBox::question(pMainFrame, tr("Question"), strong,  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
	if(resp != QMessageBox::Yes) return;


	Foil*pNextFoil = Foil::deleteFoil(Foil::curFoil());
	Foil::setCurFoil(pNextFoil);
	OpPoint::setCurOpp(NULL);
	Polar::setCurPolar(NULL);

	pMainFrame->UpdateFoilListBox();

	if(m_bPolarView) CreatePolarCurves();
	else         CreateOppCurves();
	emit projectModified();

	SetControls();
	UpdateView();
}



/**
 * The user has requested the deletion of the current OpPoint.
 */
void QXDirect::OnDelCurOpp()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	OpPoint* pOpPoint = OpPoint::curOpp();
	StopAnimate();

	if (!pOpPoint) return;
	QString strong,str;
	strong = tr("Are you sure you want to delete the Operating Point\n");
	if(Polar::curPolar()->polarType()!=FIXEDAOAPOLAR) str = QString("Alpha = %1").arg(pOpPoint->Alpha,0,'f',2);
	else                                        str = QString("Reynolds = %1").arg(pOpPoint->Reynolds,0,'f',0);
	strong += str;
	strong += "  ?";

	if (QMessageBox::Yes == QMessageBox::question(pMainFrame, tr("Question"), strong,
		QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))
	{
		OpPoint::deleteOpp(OpPoint::curOpp());
		pMainFrame->UpdateOppListBox();
		SetOpp();
		UpdateView();
	}
	SetControls();
}


/**
 * The user has requested the deletion of the current Polar.
 */
void QXDirect::OnDeleteCurPolar()
{
	if(!Polar::curPolar()) return;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	OpPoint *pOpPoint;
	int l;
	QString str;

	str = tr("Are you sure you want to delete the polar :\n  ") + Polar::curPolar()->polarName();
	str += tr("\n and all the associated OpPoints ?");

	if (QMessageBox::Yes == QMessageBox::question(pMainFrame, tr("Question"), str,
		QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))
	{
		// start by removing all OpPoints
		for (l=m_poaOpp->size()-1; l>=0; l--)
		{
			pOpPoint = (OpPoint*)m_poaOpp->at(l);
			if (pOpPoint->m_strPlrName  == Polar::curPolar()->polarName() &&
				pOpPoint->m_strFoilName == Foil::curFoil()->foilName())
			{
				m_poaOpp->removeAt(l);
				delete pOpPoint;
			}
		}
		// then remove the CPolar and update views
		for (l=m_poaPolar->size()-1; l>=0; l--)
		{
			if(Polar::curPolar() == m_poaPolar->at(l))
			{
				m_poaPolar->removeAt(l);
				delete Polar::curPolar();
				break;
			}
		}
		OpPoint::setCurOpp(NULL);
		Polar::setCurPolar(NULL);
	}

	pMainFrame->UpdatePolarListBox();
	SetPolar();

	emit projectModified();
	UpdateView();
}


/**
 * The user has requested the deletion of the OpPoints associated to the current Polar.
 */
void QXDirect::OnDeletePolarOpps()
{
	if(!Foil::curFoil() || !Polar::curPolar()) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	OpPoint *pOpp;

	for(int i=m_poaOpp->size()-1; i>=0; i--)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		if(pOpp->m_strFoilName==Foil::curFoil()->foilName() && pOpp->m_strPlrName==Polar::curPolar()->polarName())
		{
			m_poaOpp->removeAt(i);
			delete pOpp;
		}
	}

	OpPoint::setCurOpp(NULL);
	emit projectModified();

	pMainFrame->UpdateOppListBox();
	if(!m_bPolarView) CreateOppCurves();
	SetCurveParams();
	SetControls();
	UpdateView();
}


/**
 * The user has requested the deletion of the OpPoints associated to the current Foil.
 */
void QXDirect::OnDeleteFoilOpps()
{
	if(!Foil::curFoil() || !Polar::curPolar()) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	OpPoint *pOpp;

	for(int i=m_poaOpp->size()-1; i>=0; i--)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		if(pOpp->m_strFoilName==Foil::curFoil()->foilName())
		{
			m_poaOpp->removeAt(i);
			delete pOpp;
		}
	}
	OpPoint::setCurOpp(NULL);

	pMainFrame->UpdateOppListBox();
	if(!m_bPolarView) CreateOppCurves();
	SetCurveParams();
	SetControls();
	UpdateView();

	emit projectModified();
}


/**
 * The user has requested the deletion of the Polars associated to the current Foil.
 */
void QXDirect::OnDeleteFoilPolars()
{
	if(!Foil::curFoil()) return;
	MainFrame * pMainFrame = (MainFrame*) s_pMainFrame;
	int l;
	OpPoint *pOpPoint;
	StopAnimate();

	QString strong;

	strong = tr("Are you sure you want to delete polars and OpPoints\n");
	strong +=tr("associated to ")+Foil::curFoil()->foilName()  + " ?";
	if (QMessageBox::Yes == QMessageBox::question(pMainFrame, tr("Question"), strong,
		QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))

	{
		// start by removing all OpPoints
		for (l=m_poaOpp->size()-1; l>=0; l--)
		{
			pOpPoint = (OpPoint*)m_poaOpp->at(l);
			if (pOpPoint->m_strFoilName == Foil::curFoil()->foilName())
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
			if (pPolar->foilName() == Foil::curFoil()->foilName())
			{
				m_poaPolar->removeAt(l);
				delete pPolar;
			}
		}
		OpPoint::setCurOpp(NULL);

	}
	Polar::setCurPolar(NULL);
	SetPolar();
	if(m_bPolarView) CreatePolarCurves();
	else             CreateOppCurves();

	pMainFrame->UpdatePolarListBox();

	emit projectModified();

	SetControls();
	UpdateView();
}


/**
 * The user has requested a local refinement of the panels of the current foil
 */
void QXDirect::OnCadd()
{
	StopAnimate();
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(!Foil::curFoil())		return;

	void* ptr = OpPoint::curOpp();
	OpPoint::setCurOpp(NULL);
	CreateOppCurves();

	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	bool bState = m_bShowPanels;
	m_bPressure = false;
	m_bBL       = false;
	OnOpPointView();

	CAddDlg caDlg(pMainFrame);
	caDlg.m_pBufferFoil = &m_BufferFoil;
	caDlg.m_pMemFoil    = Foil::curFoil();
	caDlg.InitDialog();


	m_bShowPanels = true;
	UpdateView();

	if(QDialog::Accepted == caDlg.exec())
	{
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 1;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_bPoints = false;
		OpPoint::setCurOpp((OpPoint*)ptr);
		if(!AddNewFoil(pNewFoil))	SetBufferFoil();
		pMainFrame->UpdateFoilListBox();
	}
	else
	{
		OpPoint::setCurOpp((OpPoint*)ptr);
		SetBufferFoil();
		m_pXFoil->InitXFoilGeometry(Foil::curFoil());
	}

	m_bShowPanels = bState;

	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();
}


/**
 * The user has requested that the foil be derotated
 */
void QXDirect::OnDerotateFoil()
{
	if(!Foil::curFoil()) return;
	QString str;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	StopAnimate();

	Foil *pNewFoil = new Foil;
	pNewFoil->CopyFoil(Foil::curFoil());
	pNewFoil->m_FoilStyle = 0;
	pNewFoil->m_FoilWidth = 1;
	pNewFoil->m_bPoints = false;
	pNewFoil->m_FoilColor  = MainFrame::GetColor(0);

	double angle = pNewFoil->DeRotate();
	str = QString(tr("The foil has been de-rotated by %1 degrees")).arg(angle,6,'f',3);
	pMainFrame->statusBar()->showMessage(str);

	if(!AddNewFoil(pNewFoil))	SetBufferFoil();
	SetFoil(pNewFoil);
	pMainFrame->UpdateFoilListBox();

	emit projectModified();
	UpdateView();
}


/**
 * The user has requested to modify the parameters of the active polar
 */
void QXDirect::OnEditCurPolar()
{
	if (!Polar::curPolar()) return;

	Polar *pMemPolar = new Polar;
	pMemPolar->CopyPolar(Polar::curPolar());

	EditPlrDlg epDlg((MainFrame*)s_pMainFrame);
	epDlg.move(EditPlrDlg::s_Position);
	epDlg.resize(EditPlrDlg::s_WindowSize);
	if(EditPlrDlg::s_bWindowMaximized) epDlg.setWindowState(Qt::WindowMaximized);

	epDlg.InitDialog(this, Polar::curPolar(), NULL, NULL);

	bool bPoints = Polar::curPolar()->m_bShowPoints;
	Polar::curPolar()->m_bShowPoints = true;

	CreatePolarCurves();
	UpdateView();

	if(epDlg.exec() == QDialog::Accepted)
	{
		emit projectModified();
	}
	else
	{
		Polar::curPolar()->CopyPolar(pMemPolar);
	}
	Polar::curPolar()->m_bShowPoints = bPoints;
	CreatePolarCurves();
	UpdateView();

	delete pMemPolar;
}


/**
 * The user has requested the export of the current results stored in the XFoil object to a text file
 */
void QXDirect::OnExportCurXFoilResults()
{
	if(!m_pXFoil->lvconv) return;
	if(!Foil::curFoil())		  return;

	QString FileName,  OutString, strong;

	double x[IVX][3],Hk[IVX][3],UeVinf[IVX][3], Cf[IVX][3], Cd[IVX][3], AA0[IVX][3];
	double RTheta[IVX][3], DStar[IVX][3], Theta[IVX][3];
	double uei;
	double que = 0.5*m_pXFoil->qinf*m_pXFoil->qinf;
	double qrf = m_pXFoil->qinf;
	int nside1, nside2, ibl;
	int type = 1;

	FileName = Foil::curFoil()->foilName();
	FileName.replace("/", " ");

	FileName = QFileDialog::getSaveFileName(this, tr("Export Current XFoil Results"),
											Settings::s_LastDirName,
											tr("Text File (*.txt);;Comma Separated Values (*.csv)"));

	if(!FileName.length()) return;
	int pos = FileName.lastIndexOf("/");
	if(pos>0) Settings::s_LastDirName = FileName.left(pos);

	pos  = FileName.lastIndexOf(".csv");
	if(pos>0) type = 2;

	QFile DestFile(FileName);

	if (!DestFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&DestFile);

	out << VERSIONNAME;
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


/**
 * The user has requested the export of all polars to text files
 */
void QXDirect::OnExportAllPolars()
{
	QString FileName, DirName;
	QFile XFile;
	QTextStream out(&XFile);

	//select the directory for output
	DirName = QFileDialog::getExistingDirectory(this,  tr("Export Directory"), Settings::s_LastDirName);

	Polar *pPolar;
	for(int l=0; l<m_poaPolar->size(); l++)
	{
		pPolar = (Polar*)m_poaPolar->at(l);
		FileName = DirName + "/" + pPolar->foilName() + "_" + pPolar->m_PlrName;
		if(Settings::s_ExportFileType==TXT) FileName += ".txt";
		else                                 FileName += ".csv";

		XFile.setFileName(FileName);
		if (XFile.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			pPolar->ExportPolar(out, Settings::s_ExportFileType);
			XFile.close();
		}
	}
}


/**
 * The user has requested the export of the current foil to a text file
 */
void QXDirect::OnExportCurFoil()
{
	if(!Foil::curFoil())	return;

	QString FileName;

	FileName = Foil::curFoil()->foilName();
	FileName.replace("/", " ");

	FileName = QFileDialog::getSaveFileName(this, tr("Export Foil"),
											Settings::s_LastDirName+"/"+FileName+".dat",
											tr("Foil File (*.dat)"));

	if(!FileName.length()) return;
	int pos = FileName.lastIndexOf("/");
	if(pos>0) Settings::s_LastDirName = FileName.left(pos);

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);

	Foil::curFoil()->ExportFoil(out);
	XFile.close();
}


/**
 * The user has requested the export of the current OpPoint to a text file
 */
void QXDirect::OnExportCurOpp()
{
	if(!Foil::curFoil() || !Polar::curPolar() || !OpPoint::curOpp())	return;

	QString FileName;

	QString filter;
	if(Settings::s_ExportFileType==TXT) filter = "Text File (*.txt)";
	else                                  filter = "Comma Separated Values (*.csv)";

	FileName = QFileDialog::getSaveFileName(this, tr("Export OpPoint"),
											Settings::s_LastDirName ,
											tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
											&filter);
	if(!FileName.length()) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) Settings::s_LastDirName = FileName.left(pos);
	pos = FileName.lastIndexOf(".csv");
	if (pos>0) Settings::s_ExportFileType = CSV;
	else       Settings::s_ExportFileType = TXT;

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);

	OpPoint::curOpp()->ExportOpp(out, VERSIONNAME, Settings::s_ExportFileType);
	XFile.close();
}


/**
 * The user has requested the export of the OpPoints associated to the current Polar to a text file
 */
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
	if(Settings::s_ExportFileType==TXT) filter = "Text File (*.txt)";
	else                                  filter = "Comma Separated Values (*.csv)";

	FileName = QFileDialog::getSaveFileName(this, tr("Export OpPoint"),
											Settings::s_LastDirName ,
											tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
											&filter);

	if(!FileName.length()) return;
	if(!FileName.length()) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) Settings::s_LastDirName = FileName.left(pos);
	pos = FileName.lastIndexOf(".csv");
	if (pos>0) Settings::s_ExportFileType = CSV;
	else       Settings::s_ExportFileType = TXT;

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);


	QString Header, strong;
	out<<VERSIONNAME;
	out<<"\n\n";
	strong = Foil::curFoil()->foilName() + "\n";
	out << strong;

	OpPoint *pOpPoint;

	for (i=0; i<m_poaOpp->size(); i++)
	{
		pOpPoint = (OpPoint*)m_poaOpp->at(i);
		if(pOpPoint->m_strFoilName == Polar::curPolar()->foilName() && pOpPoint->m_strPlrName == Polar::curPolar()->polarName() )
		{
			if(Settings::s_ExportFileType==TXT)
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
			if(Settings::s_ExportFileType==1) Header = QString("  Alpha        Cd        Cl        Cm        XTr1      XTr2   TEHMom    Cpmn\n");
			else        Header = QString("Alpha,Cd,Cl,Cm,XTr1,XTr2,TEHMom,Cpmn\n");
			out<<Header;

			if(Settings::s_ExportFileType==TXT)
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
			if(Settings::s_ExportFileType==TXT) out<< " Cpi          Cpv\n-----------------\n";
			else                                  out << "Cpi,Cpv\n";

			for (j=0; j<pOpPoint->n; j++)
			{
				if(pOpPoint->m_bViscResults)
				{
					if(Settings::s_ExportFileType==TXT) strong = QString("%1   %2\n").arg(pOpPoint->Cpi[j], 7,'f',4).arg(pOpPoint->Cpv[j], 7, 'f',4);
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


/**
 * The user has requested the export of the current Polar to a text file
 */
void QXDirect::OnExportCurPolar()
{
	if(!Foil::curFoil() || !Polar::curPolar())	return;

	QString FileName, filter;

	if(Settings::s_ExportFileType==TXT) filter = "Text File (*.txt)";
	else                                  filter = "Comma Separated Values (*.csv)";

	FileName = Polar::curPolar()->polarName();
	FileName.replace("/", " ");
	FileName = QFileDialog::getSaveFileName(this, tr("Export Polar"),
											Settings::s_LastDirName + "/"+FileName,
											tr("Text File (*.txt);;Comma Separated Values (*.csv)"),
											&filter);
	if(!FileName.length()) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) Settings::s_LastDirName = FileName.left(pos);
	pos = FileName.lastIndexOf(".csv");
	if (pos>0) Settings::s_ExportFileType = CSV;
	else       Settings::s_ExportFileType = TXT;

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);

	Polar::curPolar()->ExportPolar(out, Settings::s_ExportFileType);
	XFile.close();
}


/**
 * The user has requested an edition of the current foil coordinates
 */
void QXDirect::OnFoilCoordinates()
{
	if(!Foil::curFoil())	return;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	StopAnimate();

	bool bState = m_bShowPanels;//save current view setting

	void* ptr = OpPoint::curOpp();
	OpPoint::setCurOpp(NULL);
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

	FoilCoordDlg fcoDlg(pMainFrame);
	fcoDlg.m_pMemFoil    = Foil::curFoil();
	fcoDlg.m_pBufferFoil = &m_BufferFoil;
	fcoDlg.InitDialog();

	m_bShowPanels = true;
	UpdateView();

	int res = fcoDlg.exec();

	if(QDialog::Accepted == res)
	{
		m_BufferFoil.m_bTEFlap = bFlap;
		m_BufferFoil.m_TEFlapAngle = FlapAngle;
		m_BufferFoil.m_TEXHinge = Xh;
		m_BufferFoil.m_TEYHinge = Yh;

		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_bPoints = false;
		OpPoint::setCurOpp((OpPoint*)ptr);

		if(!AddNewFoil(pNewFoil))	SetBufferFoil();
		SetFoil(pNewFoil);
		pMainFrame->UpdateFoilListBox();
		emit projectModified();
	}
	else
	{
		//reset everything
		OpPoint::setCurOpp((OpPoint*)ptr);
		m_BufferFoil.m_bTEFlap = bFlap;
		m_BufferFoil.m_TEFlapAngle = FlapAngle;
		m_BufferFoil.m_TEXHinge = Xh;
		m_BufferFoil.m_TEYHinge = Yh;
//		SetFoilFlap(&m_BufferFoil);
		SetBufferFoil();
		m_pXFoil->InitXFoilGeometry(Foil::curFoil());
	}

	m_BufferFoil.m_iHighLight = -1;
	m_bShowPanels = bState;//restore as it was
	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();
}


/**
 * The user has requested to perform an edition of the current foil's thickness and camber properties.
 */
void QXDirect::OnFoilGeom()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	if(!Foil::curFoil())	return;

	StopAnimate();
	m_bPolarView   = false;

	void* ptr = OpPoint::curOpp();
	OpPoint::setCurOpp(NULL);
	CreateOppCurves();
	UpdateView();

	FoilGeomDlg fgeDlg(pMainFrame);
	fgeDlg.m_pMemFoil = Foil::curFoil();
	fgeDlg.m_pBufferFoil = &m_BufferFoil;
	fgeDlg.InitDialog();

	if(fgeDlg.exec() == QDialog::Accepted)
	{
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_bPoints = false;
		OpPoint::setCurOpp((OpPoint*)ptr);
		if(!AddNewFoil(pNewFoil))	SetBufferFoil();
		SetFoil(pNewFoil);
		pMainFrame->UpdateFoilListBox();
		emit projectModified();
	}
	else
	{
		OpPoint::setCurOpp((OpPoint*)ptr);
		SetBufferFoil();
		m_pXFoil->InitXFoilGeometry(Foil::curFoil());
	}

	UpdateView();
}


/**
 * The user has requested an edition of the current graph's settings
 */
void QXDirect::OnGraphSettings()
{
	QGraph *pGraph = NULL;

	pGraph = m_pCurGraph;
	if(!pGraph) return;

	GraphDlg grDlg((MainFrame*)s_pMainFrame);
	if(!m_bPolarView)
	{
		grDlg.m_iGraphType = 51;
	}
	else
	{
		grDlg.m_iGraphType = 52;
	}

	QGraph graph;
	graph.CopySettings(pGraph);
	grDlg.m_pMemGraph = &graph;
	grDlg.m_pGraph = pGraph;

	grDlg.SetParams();
	int res = grDlg.exec();

	if(res == QDialog::Accepted)
	{
		if(!m_bPolarView)
		{
			if(m_CpGraph.GetYVariable() == 0 || m_CpGraph.GetYVariable()>=2)
			{
				m_CpGraph.SetYTitle(tr("Cp"));
				m_CpGraph.SetInverted(true);
			}
			else
			{
				m_CpGraph.SetYTitle(tr("Q"));
				m_CpGraph.SetInverted(false);
			}
			m_CpGraph.ResetYLimits();
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
			if(grDlg.m_bVariableChanged)
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


/**
 * The user has requested the highlighting of the current OpPoint on the polar curves
 */
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


/**
 * The user has requested to hide all OpPoints
 */
void QXDirect::OnHideAllOpps()
{
	OpPoint *pOpp;
	for (int i=0; i<m_poaOpp->size(); i++)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		pOpp->m_bIsVisible = false;
	}
	emit projectModified();
	if(!m_bPolarView) CreateOppCurves();
	SetAnalysisParams();
	UpdateView();
}


/**
 * The user has requested to hide all polar curves
 */
void QXDirect::OnHideAllPolars()
{
	Polar *pPolar;
	for (int i=0; i<m_poaPolar->size(); i++)
	{
		pPolar = (Polar*)m_poaPolar->at(i);
		pPolar->m_bIsVisible = false;
	}
	emit projectModified();
	CreatePolarCurves();
	SetCurveParams();
	UpdateView();
}


/**
 * The user has requested to hide all polar curves associated to the current Foil
 */
void QXDirect::OnHideFoilPolars()
{
	if(!Foil::curFoil()) return;
	Polar *pPolar;
	for (int i=0; i<m_poaPolar->size(); i++)
	{
		pPolar = (Polar*)m_poaPolar->at(i);
		if(pPolar->foilName() == Foil::curFoil()->foilName())
		{
			pPolar->m_bIsVisible = false;
		}
	}
	emit projectModified();
	CreatePolarCurves();
	SetCurveParams();
	UpdateView();
}


/**
 * The user has requested to hide all OpPoint curves associated to the current Foil
 */
void QXDirect::OnHideFoilOpps()
{
	if(!Foil::curFoil() || !Polar::curPolar()) return;

	OpPoint *pOpp;

	for(int i=0; i<m_poaOpp->size(); i++)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		if(pOpp->m_strFoilName==Foil::curFoil()->foilName())
			pOpp->m_bIsVisible = false;
	}
	emit projectModified();
	if(!m_bPolarView) CreateOppCurves();
	SetCurveParams();
	UpdateView();
}


/**
 * The user has requested to hide all OpPoint curves associated to the current Polar
 */
void QXDirect::OnHidePolarOpps()
{
	if(!Foil::curFoil() || !Polar::curPolar()) return;

	OpPoint *pOpp;

	for(int i=0; i<m_poaOpp->size(); i++)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		if(pOpp->m_strFoilName==Foil::curFoil()->foilName() && pOpp->m_strPlrName==Polar::curPolar()->polarName())
			pOpp->m_bIsVisible = false;
	}
	emit projectModified();
	if(!m_bPolarView) CreateOppCurves();
	SetCurveParams();
	UpdateView();
}


/**
 * The user has requested to import a polar from a text file.
 * The Polar will be added to the array only if a Foil with the parent name exists.
 */
void QXDirect::OnImportXFoilPolar()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	Polar *pPolar = new Polar;
	double Re, alpha, CL, CD, CDp, CM, Xt, Xb,Cpmn, HMom;
	QString FoilName, PathName, strong, str;

	PathName = QFileDialog::getOpenFileName(pMainFrame, tr("Open File"),
											Settings::s_LastDirName,
											tr("XFoil Polar Format (*.*)"));
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
	int Line;
	bool bOK, bOK2, bRead;
	Line = 0;

	bRead  = ReadAVLString(in, Line, strong);// XFoil or XFLR5 version
	bRead  = ReadAVLString(in, Line, strong);// Foil Name

	FoilName = strong.right(strong.length()-22);
	FoilName = FoilName.trimmed();

	if(!Foil::foil(FoilName))
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
//				textline = strong.toLatin1();
//				text = textline.constData();
//				res = sscanf(text, "%lf%lf%lf%lf%lf%lf%lf%lf%lf", &alpha, &CL, &CD, &CDp, &CM, &Xt, &Xb, &Cpmn, &HMom);

				//Do this the C++ way
				QStringList values = strong.split(" ", QString::SkipEmptyParts);

				if(values.length()>=7)
				{
					alpha  = values.at(0).toDouble();
					CL     = values.at(1).toDouble();
					CD     = values.at(2).toDouble();
					CDp    = values.at(3).toDouble();
					CM     = values.at(4).toDouble();
					Xt     = values.at(5).toDouble();
					Xb     = values.at(6).toDouble();

					if(values.length() >= 9)
					{
						Cpmn    = values.at(7).toDouble();
						HMom    = values.at(8).toDouble();
						pPolar->AddPoint(alpha, CD, CDp, CL, CM, Xt, Xb, Cpmn, HMom,Re,0.0);
					}
					else
					{
						pPolar->AddPoint(alpha, CD, CDp, CL, CM, Xt, Xb, 0.0, 0.0,Re,0.0);

					}
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

	pPolar->m_Color = MainFrame::GetColor(1);

	Polar::addPolar(pPolar);

	OpPoint::setCurOpp(NULL);
	SetPolar(pPolar);
	pMainFrame->UpdatePolarListBox();
	UpdateView();
	emit projectModified();
}


/**
 * The user has requested to import a polar from a text file in JavaFoil format
 * The Polar will be added to the array only if a Foil with the parent name exists.
 *  @todo Note: this option has not been tested in years... the JavaFoil format may have changed since
 */
void QXDirect::OnImportJavaFoilPolar()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	QString FoilName;
	QString strong, str;

	QString PathName;
	bool bOK;
	QByteArray textline;
	const char *text;

	PathName = QFileDialog::getOpenFileName(pMainFrame, tr("Open File"),
											Settings::s_LastDirName,
											tr("JavaFoil Polar Format (*.*)"));
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

	bool bIsReading = true;
	int res, Line;
	int NPolars = 0;
	double Re;

	double alpha, CL, CD, CM, Xt,  Xb;

	Line = 0;
	if(!ReadAVLString(in, Line, FoilName)) return;


	FoilName = FoilName.trimmed();

	if(!Foil::foil(FoilName))
	{
		str = tr("No Foil with the name ")+FoilName;
		str+= tr("\ncould be found. The polar(s) will not be stored");
		QMessageBox::warning(pMainFrame, tr("Warning"), str);
		return;
	}
	if(!ReadAVLString(in, Line, strong)) return; //blank line

	while(bIsReading)
	{
		if(!ReadAVLString(in, Line, strong)) break; //Re number

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

			pPolar->m_Color = MainFrame::GetColor(1);
			Polar::addPolar(pPolar);
			Polar::setCurPolar(pPolar);
			NPolars++;

			if(!ReadAVLString(in, Line, strong)) break;//?	Cl	Cd	Cm 0.25	TU	TL	SU	SL	L/D
			if(!ReadAVLString(in, Line, strong)) break;//[?]	[-]	[-]	[-]	[-]	[-]	[-]	[-]	[-]

			res = 6;
			while(res==6)
			{
				bIsReading  = ReadAVLString(in, Line, strong);//values
				if(!bIsReading) break;
				strong = strong.trimmed();
				if(strong.length())
				{
					strong.replace(',', '.');

					textline = strong.toLatin1();
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
		OpPoint::setCurOpp(NULL);
		SetPolar();
		pMainFrame->UpdatePolarListBox();
		UpdateView();
		emit projectModified();
	}
}



/**
 * The user has requested the launch of the interface to create a foil from the interpolation of two existing Foil objects.
 */
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
	OnOpPointView();

	InterpolateFoilsDlg ifDlg(pMainFrame);
	ifDlg.m_poaFoil     = m_poaFoil;
	ifDlg.m_pBufferFoil = &m_BufferFoil;// work on the buffer foil
	ifDlg.InitDialog();

	if(ifDlg.exec() == QDialog::Accepted)
	{
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilName = ifDlg.m_NewFoilName;
		if(!AddNewFoil(pNewFoil))	SetBufferFoil();
		SetFoil(pNewFoil);
		pMainFrame->UpdateFoilListBox();
		emit projectModified();
	}
	else
	{
		SetBufferFoil();// restore buffer foil.. from current foil
		m_pXFoil->InitXFoilGeometry(Foil::curFoil());
	}

	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();
}


/**
 * The user has requested the launch of the interface to manage Foil objects.
 */
void QXDirect::OnManageFoils()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	ManageFoilsDlg mfDlg(pMainFrame);

	QString FoilName = "";
	if(Foil::curFoil()) FoilName = Foil::curFoil()->foilName();
	mfDlg.InitDialog(FoilName);
	mfDlg.exec();

	if(mfDlg.m_bChanged) emit projectModified();

	if(mfDlg.m_pFoil) SetFoil(mfDlg.m_pFoil);
	else                           SetFoil();

	pMainFrame->UpdateFoilListBox();
	emit projectModified();
	SetControls();
	UpdateView();
}


/**
 * The user has requested the launch of the interface used to create a NACA type foil.
 */
void QXDirect::OnNacaFoils()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	StopAnimate();
	m_bPolarView   = false;

	void* ptr0 = Foil::curFoil();
	void* ptr  = OpPoint::curOpp();
	Foil::setCurFoil(NULL);
	OpPoint::setCurOpp(NULL);

	CreateOppCurves();
	UpdateView();

	NacaFoilDlg nacaDlg(pMainFrame);
	nacaDlg.m_pBufferFoil = &m_BufferFoil;

	if (nacaDlg.exec() == QDialog::Accepted)
	{
		QString str;
		if(nacaDlg.s_Digits>0 && log10((double)nacaDlg.s_Digits)<4)
			str = QString("%1").arg(nacaDlg.s_Digits,4,10,QChar('0'));
		else
			str = QString("%1").arg(nacaDlg.s_Digits);
		str = "NACA "+ str;

		Foil *pNewFoil = new Foil;
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilName = str;

		OpPoint::setCurOpp((OpPoint*)ptr);

		if(!AddNewFoil(pNewFoil))	SetBufferFoil();

		SetFoil(pNewFoil);
		pMainFrame->UpdateFoilListBox();
		emit projectModified();
	}
	else
	{
		Foil::setCurFoil((Foil*)ptr0);
		OpPoint::setCurOpp((OpPoint*)ptr);
		SetBufferFoil();
		m_pXFoil->InitXFoilGeometry(Foil::curFoil());
	}
	SetControls();
	UpdateView();
}


/**
 * The user has requested that the length of the current foil be normalized to 1.
 */
void QXDirect::OnNormalizeFoil()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(!Foil::curFoil()) return;
	QString str;
	StopAnimate();


	double length = Foil::curFoil()->NormalizeGeometry();
	m_pXFoil->InitXFoilGeometry(Foil::curFoil());
	SetBufferFoil();
	str = QString(tr("The foil has been normalized from %1  to 1.000")).arg(length,7,'f',3);
	emit projectModified();
	pMainFrame->statusBar()->showMessage(str);

	UpdateView();
}


/**
 * The user has requested a  plot of the A/A0 variable using current XFoil results.
 */
void QXDirect::OnNPlot()
{
	if(!m_pXFoil->lvconv) return;
	int i;
	int nside1, nside2, ibl;

	m_CpGraph.SetYVariable(2);
	m_XFoilVar=6;
	m_CpGraph.DeleteCurves();
	m_CpGraph.ResetLimits();
	m_CpGraph.SetAuto(true);
	m_CpGraph.SetInverted(false);
	m_CpGraph.SetYTitle("A/A0");
	Curve * pTopCurve = m_CpGraph.AddCurve();
	Curve * pBotCurve = m_CpGraph.AddCurve();
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
	m_CpGraph.SetXScale();
	SetFoilScale();
	SetControls();
	UpdateView();
}


/**
 * The user has requested to switch to the OpPoint view
 */
void QXDirect::OnOpPointView()
{
	if(!m_bPolarView) return;

	m_bPolarView = false;
	CreateOppCurves();
	SetFoilScale();
	SetCurveParams();
	SetAnalysisParams();
	SetControls();
	UpdateView();
}


/**
 * The user has requested to switch to the Polar view
 */
void QXDirect::OnPolarView()
{
	if(m_bPolarView) return;
	m_bPolarView = true;
	CreatePolarCurves();
	SetCurveParams();
	SetAnalysisParams();
	SetControls();
	UpdateView();
}

/**
 * The user has requested the launch of the interface used to filter the type of polars to be displayed.
 */
void QXDirect::OnPolarFilter()
{
	PolarFilterDlg pfDlg((MainFrame*)s_pMainFrame);
	pfDlg.m_bMiarex = false;
	pfDlg.m_bType1 = m_bType1;
	pfDlg.m_bType2 = m_bType2;
	pfDlg.m_bType3 = m_bType3;
	pfDlg.m_bType4 = m_bType4;
	pfDlg.InitDialog();

	if(pfDlg.exec()==QDialog::Accepted)
	{
		m_bType1 = pfDlg.m_bType1;
		m_bType2 = pfDlg.m_bType2;
		m_bType3 = pfDlg.m_bType3;
		m_bType4 = pfDlg.m_bType4;
		if(m_bPolarView)
		{
			CreatePolarCurves();
			UpdateView();
		}
	}
}


/**
 * The user has requested the launch of the interface to refine globally the Foil
*/
void QXDirect::OnRefinePanelsGlobally()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(!Foil::curFoil())	return;
	StopAnimate();

	bool bState = m_bShowPanels;//save current view setting

	void* ptr = OpPoint::curOpp();
	OpPoint::setCurOpp(NULL);
	CreateOppCurves();

	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	m_bPressure = false;
	m_bBL       = false;
	OnOpPointView();

	TwoDPanelDlg tdpDlg(pMainFrame);
	tdpDlg.m_pBufferFoil = &m_BufferFoil;
	tdpDlg.m_pMemFoil    = Foil::curFoil();

	m_bShowPanels = true;
	UpdateView();

	tdpDlg.InitDialog();

	if(QDialog::Accepted == tdpDlg.exec())
	{
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_bPoints = true;
		OpPoint::setCurOpp((OpPoint*)ptr);
		if(!AddNewFoil(pNewFoil))	SetBufferFoil();
		SetFoil(pNewFoil);
		pMainFrame->UpdateFoilListBox();
		emit projectModified();
	}
	else
	{
		//reset everything
		OpPoint::setCurOpp((OpPoint*)ptr);
		SetBufferFoil();
		m_pXFoil->InitXFoilGeometry(Foil::curFoil());
	}

	m_bShowPanels = bState;//restore as it was
	m_bPressure   = bPressure;
	m_bBL         = bBL;
	UpdateView();
}


/**
 * The user has requested the display of the velocity in the Cp graph.
 */
void QXDirect::OnQGraph()
{
	m_bPolarView = false;
	if(m_CpGraph.GetYVariable()!=1)
	{
		m_CpGraph.ResetLimits();
		m_CpGraph.SetAuto(true);
		m_CpGraph.SetYVariable(1);
	}
	m_CpGraph.SetInverted(false);
	CreateOppCurves();
	m_CpGraph.SetYTitle(tr("Q"));

	SetControls();

	m_CpGraph.SetXScale();
	SetFoilScale();
	UpdateView();
}

/**
 * The user has requested to rename the Polar
 */
void QXDirect::OnRenamePolar()
{
	if(!Polar::curPolar()) return;
	if(!Foil::curFoil()) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	int resp, k,l;
	Polar* pPolar = NULL;
	OpPoint * pOpp;
	QString OldName = Polar::curPolar()->polarName();

	QStringList NameList;
	for(k=0; k<m_poaPolar->size(); k++)
	{
		pPolar = (Polar*)m_poaPolar->at(k);
		if(pPolar->m_FoilName == Foil::curFoil()->foilName())
			NameList.append(pPolar->m_PlrName);
	}

	RenameDlg renDlg(pMainFrame);
	renDlg.InitDialog(&NameList, Polar::curPolar()->polarName(), tr("Enter the new name for the foil polar :"));

	bool bExists = true;

	while (bExists)
	{
		resp = renDlg.exec();
		if(resp==QDialog::Accepted)
		{
			if (OldName == renDlg.newName()) return;
			//Is the new name already used ?
			bExists = false;
			for (k=0; k<m_poaPolar->size(); k++)
			{
				pPolar = (Polar*)m_poaPolar->at(k);
				if ((pPolar->m_FoilName==Foil::curFoil()->foilName()) && (pPolar->m_PlrName == renDlg.newName()))
				{
					bExists = true;
					break;
				}
			}
			if(!bExists)
			{
				for (l=(int)m_poaOpp->size()-1;l>=0; l--)
				{
					pOpp = (OpPoint*)m_poaOpp->at(l);
					if (pOpp->m_strPlrName == OldName &&
						pOpp->m_strFoilName == Foil::curFoil()->foilName())
					{
						pOpp->m_strPlrName = renDlg.newName();
					}
				}
				Polar::curPolar()->setPolarName(renDlg.newName());
			}
			emit projectModified();
		}
		else if(resp ==10)
		{//user wants to overwrite
			if (OldName == renDlg.newName()) return;
			for (k=0; k<m_poaPolar->size(); k++)
			{
				pPolar = (Polar*)m_poaPolar->at(k);
				if (pPolar->m_PlrName == renDlg.newName())
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
					if(pOpp==OpPoint::curOpp()) OpPoint::setCurOpp(NULL);
					delete pOpp;
				}
			}
			m_poaPolar->removeAt(k);
			if(pPolar==Polar::curPolar()) Polar::setCurPolar(NULL);
			delete pPolar;

			//and rename everything
			Polar::curPolar()->polarName() = renDlg.newName();

			for (l=m_poaOpp->size()-1;l>=0; l--)
			{
				pOpp = (OpPoint*)m_poaOpp->at(l);
				if (pOpp->m_strPlrName == OldName &&
					pOpp->m_strFoilName == Foil::curFoil()->foilName())
				{
					pOpp->m_strPlrName = renDlg.newName();
				}
			}

			bExists = false;
			emit projectModified();
		}
		else
		{
			return ;//cancelled
		}
	}
//	Polar::setCurPolar(NULL);
//	OpPoint::setCurOpp(NULL);
//	SetPolar();
	pMainFrame->UpdatePolarListBox();
	UpdateView();
}


/**
 *The user has requested the display of the detailed properties of the active OpPoint object.
 */
void QXDirect::OnOpPointProps()
{
	if(!Polar::curPolar()) return;
	ObjectPropsDlg opDlg((MainFrame*)s_pMainFrame);
	opDlg.m_pXDirect = this;
	opDlg.m_pOpp = OpPoint::curOpp();
	opDlg.m_pPolar = NULL;
	opDlg.m_pMiarex = NULL;
	opDlg.m_pWOpp = NULL;
	opDlg.m_pWPolar = NULL;
	opDlg.InitDialog();
	opDlg.exec();
}




/**
 *The user has requested the display of the detailed properties of the active Polar object.
 */
void QXDirect::OnPolarProps()
{
	if(!Polar::curPolar()) return;
	ObjectPropsDlg opDlg((MainFrame*)s_pMainFrame);
	opDlg.m_pXDirect = this;
	opDlg.m_pOpp = NULL;
	opDlg.m_pPolar = Polar::curPolar();
	opDlg.m_pMiarex = NULL;
	opDlg.m_pWOpp = NULL;
	opDlg.m_pWPolar = NULL;
	opDlg.InitDialog();
	opDlg.exec();
}




/**
 * The user has requested to reset all polar graph scales to their automatic default value
 */
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


/**
 * The user has requested to reset the scale of the foil to its automatic default value
 */
void QXDirect::OnResetFoilScale()
{
	SetFoilScale();
	if(!m_bAnimate) UpdateView();

}


/**
 * The user has requested to reset the polar legend to its default position
 */
void QXDirect::OnResetGraphLegend()
{
	SetPolarLegendPos();
	UpdateView();
}


/**
 * The user has requested the deletion of the dataof the current Polar.
 * The associated OpPoint objects will be deleted too.
 */
void QXDirect::OnResetCurPolar()
{
	if(!Polar::curPolar()) return;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	Polar::curPolar()->ResetPolar();

	OpPoint*pOpp;
	for(int i=m_poaOpp->size()-1;i>=0;i--)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		if(pOpp->m_strFoilName==Foil::curFoil()->foilName() && pOpp->m_strPlrName==Polar::curPolar()->polarName())
		{
			m_poaOpp->removeAt(i);
			delete pOpp;
		}
	}
	OpPoint::setCurOpp(NULL);

	pMainFrame->UpdateOppListBox();

	if(m_bPolarView) CreatePolarCurves();
	else         CreateOppCurves();
	UpdateView();

	emit projectModified();
}


/**
 * The user has requested the creation of a .plr file with the Polars of the active Foil object.
 */
void QXDirect::OnSavePolars()
{
	if(!Foil::curFoil() || !m_poaPolar->size()) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	QString FileName;
	FileName = Foil::curFoil()->foilName() + ".plr";
	FileName.replace("/", " ");

	FileName = QFileDialog::getSaveFileName(this, tr("Polar File"), Settings::s_LastDirName+"/"+FileName, tr("Polar File (*.plr)"));
	if(!FileName.length()) return;

	QString strong = FileName.right(4);
	if(strong !=".plr" && strong !=".PLR") FileName += ".plr";

	QFile XFile(FileName);
	if (!XFile.open(QIODevice::WriteOnly)) return;

	int pos = FileName.lastIndexOf("/");
	if(pos>0) Settings::s_LastDirName = FileName.left(pos);

	QDataStream ar(&XFile);
#if QT_VERSION >= 0x040500
	ar.setVersion(QDataStream::Qt_4_5);
#endif
	ar.setByteOrder(QDataStream::LittleEndian);

	pMainFrame->WritePolars(ar, Foil::curFoil());

	XFile.close();
}


/**
 * The user has toggled the switch for a sequential analysis.
 */
void QXDirect::OnSequence()
{
	m_bSequence = m_pctrlSequence->isChecked();
	SetOpPointSequence();
}


/**
 * The user has requested the launch of the interface to define a L.E. or T.E. flap.
 */
void QXDirect::OnSetFlap()
{
	if(!Foil::curFoil()) return;
	StopAnimate();

	void* ptr = OpPoint::curOpp();
	OpPoint::setCurOpp(NULL);
	CreateOppCurves();
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	m_bPressure = false;
	m_bBL       = false;
	OnOpPointView();

	FlapDlg flpDlg(pMainFrame);
	flpDlg.m_pBufferFoil  = &m_BufferFoil;
	flpDlg.m_pMemFoil     = Foil::curFoil();
	flpDlg.m_pXFoil       = m_pXFoil;
	flpDlg.InitDialog();

	if(QDialog::Accepted == flpDlg.exec())
	{
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_bPoints = false;
		OpPoint::setCurOpp((OpPoint*)ptr);
		if(!AddNewFoil(pNewFoil))	SetBufferFoil();
		SetFoil(pNewFoil);
		pMainFrame->UpdateFoilListBox();
		emit projectModified();
	}
	else
	{
		//reset everything
		OpPoint::setCurOpp((OpPoint*)ptr);
		SetBufferFoil();
		m_pXFoil->InitXFoilGeometry(Foil::curFoil());
	}

	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();
}


/**
 * The user has requested the launch of the interface to modify the radius of the Foil's leading edge.
 */
void QXDirect::OnSetLERadius()
{
	if(!Foil::curFoil())	return;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	StopAnimate();

	void* ptr = OpPoint::curOpp();
	OpPoint::setCurOpp(NULL);
	CreateOppCurves();
	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	m_bPressure = false;
	m_bBL       = false;
	OnOpPointView();

	LEDlg lDlg(pMainFrame);
	lDlg.m_pBufferFoil = &m_BufferFoil;
	lDlg.m_pMemFoil    = Foil::curFoil();
	lDlg.InitDialog();

	if(QDialog::Accepted == lDlg.exec())
	{
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_bPoints = false;
		OpPoint::setCurOpp((OpPoint*)ptr);
		if(!AddNewFoil(pNewFoil))	SetBufferFoil();
		SetFoil(pNewFoil);
		pMainFrame->UpdateFoilListBox();
		emit projectModified();
	}
	else
	{
		//reset everything
		OpPoint::setCurOpp((OpPoint*)ptr);
		SetBufferFoil();
		m_pXFoil->InitXFoilGeometry(Foil::curFoil());
	}

	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();
}


/**
 * The user has requested the launch of the interface to modify the gap at the Foil's trailing edge.
 */
void QXDirect::OnSetTEGap()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(!Foil::curFoil())	return;
	StopAnimate();

	void* ptr = OpPoint::curOpp();
	OpPoint::setCurOpp(NULL);
	CreateOppCurves();

	bool bPressure = m_bPressure;
	bool bBL       = m_bBL;
	m_bPressure = false;
	m_bBL       = false;
	OnOpPointView();

	TEGapDlg tegDlg(pMainFrame);
	tegDlg.m_pBufferFoil = &m_BufferFoil;
	tegDlg.m_pMemFoil    = Foil::curFoil();
	tegDlg.m_Gap         = Foil::curFoil()->m_Gap;
	tegDlg.InitDialog();

	if(QDialog::Accepted == tegDlg.exec())
	{
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(&m_BufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_bPoints = false;
		OpPoint::setCurOpp((OpPoint*)ptr);
		if(!AddNewFoil(pNewFoil))	SetBufferFoil();
		SetFoil(pNewFoil);
		pMainFrame->UpdateFoilListBox();
		emit projectModified();
	}
	else
	{
		//reset everything
		OpPoint::setCurOpp((OpPoint*)ptr);
		SetBufferFoil();
		m_pXFoil->InitXFoilGeometry(Foil::curFoil());
	}

	m_bPressure = bPressure;
	m_bBL       = bBL;
	UpdateView();

}


/**
 * The user has requested the display of all OpPoint curves.
 */
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
    emit projectModified();
	if(!m_bPolarView) CreateOppCurves();
	SetCurveParams();
	UpdateView();
}


/**
 * The user has requested the display of all Polar curves.
 */
void QXDirect::OnShowAllPolars()
{
	Polar *pPolar;
	for (int i=0; i<m_poaPolar->size(); i++)
	{
		pPolar = (Polar*)m_poaPolar->at(i);
		pPolar->m_bIsVisible = true;
	}
	emit projectModified();
	CreatePolarCurves();
	SetCurveParams();
	UpdateView();
}


/**
 * The user has toggled the display of the boundary layer
 */
void QXDirect::OnShowBL()
{
	if(m_pctrlShowBL->isChecked())
	{
		if(m_bPolarView) OnOpPointView();
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

/**
 * The user has toggled the display of the curve of the active object
 */
void QXDirect::OnShowCurve()
{
	//user has toggled visible switch

	if(m_bPolarView)
	{
		if (Polar::curPolar())
		{
			Polar::curPolar()->m_bIsVisible = m_pctrlShowCurve->isChecked();

		}
		CreatePolarCurves();
	}
	else if (OpPoint::curOpp())
	{
		OpPoint::curOpp()->m_bIsVisible = m_pctrlShowCurve->isChecked();
		CreateOppCurves();
	}
	emit projectModified();
	UpdateView();
}


/**
 * The user has requested the display of only the Polar curves associated to the active Foil
 */
void QXDirect::OnShowFoilPolarsOnly()
{
	if(!Foil::curFoil()) return;

	Polar *pPolar;
	for (int i=0; i<m_poaPolar->size(); i++)
	{
		pPolar = (Polar*)m_poaPolar->at(i);
		pPolar->m_bIsVisible = (pPolar->foilName() == Foil::curFoil()->foilName());
	}
	emit projectModified();
	CreatePolarCurves();
	SetCurveParams();
	UpdateView();
}


/**
 * The user has requested the display of the Polar curves associated to the active Foil
 */
void QXDirect::OnShowFoilPolars()
{
	if(!Foil::curFoil()) return;
	Polar *pPolar;
	for (int i=0; i<m_poaPolar->size(); i++)
	{
		pPolar = (Polar*)m_poaPolar->at(i);
		if(pPolar->m_FoilName == Foil::curFoil()->foilName())
		{
			pPolar->m_bIsVisible = true;
		}
	}
	emit projectModified();
	CreatePolarCurves();
	SetCurveParams();
	UpdateView();
}


/**
 * The user has requested the display of the OpPoint curves associated to the active Foil
 */
void QXDirect::OnShowFoilOpps()
{
	if(!Foil::curFoil() || !Polar::curPolar()) return;

	OpPoint *pOpp;

	m_bCurOppOnly = false;
	((MainFrame*)s_pMainFrame)->showCurOppOnly->setChecked(m_bCurOppOnly);

	for(int i=0; i<m_poaOpp->size(); i++)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		if(pOpp->m_strFoilName==Foil::curFoil()->foilName())
			pOpp->m_bIsVisible = true;
	}
	emit projectModified();
	if(!m_bPolarView) CreateOppCurves();
	SetCurveParams();

	UpdateView();
}


/**
 * The user has toggled the display of the neutral line y=0.
 */
void QXDirect::OnShowNeutralLine()
{
	m_bNeutralLine = !m_bNeutralLine;
	if(!m_bPolarView) UpdateView();
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->showNeutralLine->setChecked(m_bNeutralLine);
}


/**
 * The user has toggled the display of the Foil's panels.
 */
void QXDirect::OnShowPanels()
{
	m_bShowPanels = !m_bShowPanels;
	if(!m_bPolarView)	UpdateView();
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->showPanels->setChecked(m_bShowPanels);
}


/**
 * The user has toggled the display of the curve's points
 */
void QXDirect::OnShowCurvePoints()
{
	if(m_bPolarView)
	{
		if (Polar::curPolar())
		{ 
			Polar::curPolar()->m_bShowPoints = m_pctrlShowPoints->isChecked();
		}
		CreatePolarCurves(); 
	}
	else if (OpPoint::curOpp())
	{
		OpPoint::curOpp()->m_bShowPoints = m_pctrlShowPoints->isChecked();
		CreateOppCurves(); 
	}
	
	emit projectModified();
	UpdateView();	
}


/**
 * The user has requested the display of the curves of all OpPoint objects associated to the active Polar.
 */
void QXDirect::OnShowPolarOpps()
{
	if(!Foil::curFoil() || !Polar::curPolar()) return;

	OpPoint *pOpp;

	m_bCurOppOnly = false;
	((MainFrame*)s_pMainFrame)->showCurOppOnly->setChecked(m_bCurOppOnly);

	for(int i=0; i<m_poaOpp->size(); i++)
	{
		pOpp = (OpPoint*)m_poaOpp->at(i);
		if(pOpp->m_strFoilName==Foil::curFoil()->foilName() && pOpp->m_strPlrName==Polar::curPolar()->polarName())
			pOpp->m_bIsVisible = true;
	}
	emit projectModified();
	if(!m_bPolarView) CreateOppCurves();
	SetCurveParams();
	UpdateView();
}


/**
 * The user has toggled the display of the pressure arrows
 */
void QXDirect::OnShowPressure()
{
	if(m_pctrlShowPressure->isChecked())
	{
		if(m_bPolarView) OnOpPointView();
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


/**
 * The user has requested the display of a single polar graph
 */
void QXDirect::OnSinglePolarGraph()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (!action) return;

	m_iPlrView  = ONEGRAPH;
	m_iPlrGraph = action->data().toInt();
	m_bPolarView = true;
	SetPolarLegendPos();
	SetControls();
	UpdateView();
}


/**
 * The user has toggled the switch used to define the type of input parameter bewteen aoa, Cl, and Re
 */
void QXDirect::OnSpec()
{
	if      (m_pctrlSpec1->isChecked()) s_bAlpha = true;
	else if (m_pctrlSpec2->isChecked()) s_bAlpha = false;
	else if (m_pctrlSpec3->isChecked()) s_bAlpha = false;
}


/**
 * The user has toggled the switch which defines if OpPoints should be stored at the end of the analysis
 */
void QXDirect::OnStoreOpp()
{
	OpPoint::s_bStoreOpp = m_pctrlStoreOpp->isChecked();
}


/**
 * The user has toggled the switch which defines if the analysis will be viscous or inviscid
 */
void QXDirect::OnViscous()
{
	s_bViscous = m_pctrlViscous->isChecked();
}


/**
 * The user has requested the launch of the interface used to define the display style of the Foil
 */
void QXDirect::OnXDirectStyle()
{
	XDirectStyleDlg xdsDlg((MainFrame*)s_pMainFrame);
	xdsDlg.m_pXDirect = this;
	xdsDlg.m_iBLStyle         = m_iBLStyle;
	xdsDlg.m_iBLWidth         = m_iBLWidth;
	xdsDlg.m_crBLColor        = m_crBLColor;
	xdsDlg.m_iPressureStyle   = m_iPressureStyle;
	xdsDlg.m_iPressureWidth   = m_iPressureWidth;
	xdsDlg.m_crPressureColor  = m_crPressureColor;
	xdsDlg.m_iNeutralStyle    = m_iNeutralStyle;
	xdsDlg.m_iNeutralWidth    = m_iNeutralWidth;
	xdsDlg.m_crNeutralColor   = m_crNeutralColor;
	xdsDlg.InitDialog();
	if(xdsDlg.exec() == QDialog::Accepted)
	{
		m_iBLStyle         = xdsDlg.m_iBLStyle;
		m_iBLWidth         = xdsDlg.m_iBLWidth;
		m_crBLColor        = xdsDlg.m_crBLColor;
		m_iPressureStyle   = xdsDlg.m_iPressureStyle;
		m_iPressureWidth   = xdsDlg.m_iPressureWidth;
		m_crPressureColor  = xdsDlg.m_crPressureColor;
		m_iNeutralStyle    = xdsDlg.m_iNeutralStyle;
		m_iNeutralWidth    = xdsDlg.m_iNeutralWidth;
		m_crNeutralColor   = xdsDlg.m_crNeutralColor;
	}
	UpdateView();
}



/**
 * The user has requested the launch of the interface used to define advanced settings for the XFoil analysis
 */
void QXDirect::OnXFoilAdvanced()
{
	XFoilAdvancedDlg xfaDlg((MainFrame*)s_pMainFrame);
	xfaDlg.m_IterLimit = XFoilTask::s_IterLim;
	xfaDlg.m_bInitBL   = XFoilTask::s_bAutoInitBL;
	xfaDlg.m_VAccel      = XFoil::vaccel;
	xfaDlg.m_bFullReport = XFoil::s_bFullReport;
	xfaDlg.InitDialog();

	if (QDialog::Accepted == xfaDlg.exec())
	{
		XFoil::vaccel             = xfaDlg.m_VAccel;
		XFoil::s_bFullReport   = xfaDlg.m_bFullReport;
		XFoilTask::s_bAutoInitBL  = xfaDlg.m_bInitBL;
		XFoilTask::s_IterLim      = xfaDlg.m_IterLimit;
	}
}

/**
 * The method which draws the boundary layer in the OpPoint view.
 * @param painter a reference to the QPainter object with which to draw
 * @param pOpPoint the OpPoint object to draw
 * @param scale the scale of the view
 */
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
	WakePen.setStyle(getStyle(m_iBLStyle));
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


/**
 * The method which draws the pressure arrows in the OpPoint view.
 * @param painter a reference to the QPainter object with which to draw
 * @param pOpPoint the OpPoint object to draw
 * @param scale the scale of the view
 */
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
	CpvPen.setStyle(getStyle(m_iPressureStyle));
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
	LiftPen.setStyle(getStyle(m_iPressureStyle));
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


/**
 * Draws a pair of polar graphs
 * @param painter a reference to the QPainter object with which to draw
 */
void QXDirect::PaintCoupleGraphs(QPainter &painter)
{
	int h  = m_rCltRect.height();
	int w  = m_rCltRect.width();
	int w2 = (int)(w/2);
	int h23 = (int)(2*h/3);

	QRect Rect1(0,0,w2,h23);
	QRect Rect2(w2,0,w2,h23);
	QRect Rect3(0, h23, w,h-h23);
	painter.fillRect(Rect3, Settings::s_BackgroundColor);

	m_PlrGraph[0].DrawGraph(Rect1, painter);
	m_PlrGraph[4].DrawGraph(Rect2, painter);

	PaintPolarLegend(m_PolarLegendOffset, h, painter);
}


/**
 * Draws the Cp Graph and the foil
 * @param painter a reference to the QPainter object with which to draw
 */
void QXDirect::PaintOpPoint(QPainter &painter)
{
	static double Alpha, FoilScale;
	FoilScale = m_fFoilScale;
	QString Result, str, str1;
	painter.fillRect(m_rCltRect, Settings::s_BackgroundColor);

	if (m_rCltRect.width()<150 || m_rCltRect.height()<150) return;


	if(m_bCpGraph)
	{
		//Draw Cp Graph
		m_CpGraph.DrawGraph(painter);
		QPoint Place(m_rCltRect.right()/2, m_rCltRect.top() + 20);
		m_CpGraph.DrawLegend(painter, Place, Settings::s_TextFont, Settings::s_TextColor);//Graph::DrawLegend uses graph's legend font and color
	}

	if(m_bNeutralLine)
	{
		QPen NeutralPen(m_crNeutralColor);
		NeutralPen.setStyle(getStyle(m_iNeutralStyle));
		NeutralPen.setWidth(m_iNeutralWidth);
		painter.setPen(NeutralPen);
		painter.drawLine(m_rCltRect.left(),m_FoilOffset.y(),
						 m_rCltRect.right(),m_FoilOffset.y());
	}

	if (!Foil::curFoil() || !Foil::curFoil()->foilName().length())
		return;

	if(Foil::curFoil())
	{
		Alpha = 0.0;
		if(OpPoint::curOpp()) Alpha = OpPoint::curOpp()->Alpha;

		m_BufferFoil.m_bPoints = m_bShowPanels;
		m_BufferFoil.DrawFoil(painter, -Alpha, m_fFoilScale, FoilScale, m_FoilOffset);

		if(m_bPressure && OpPoint::curOpp()) PaintPressure(painter, OpPoint::curOpp(), m_fFoilScale);
		if(m_bBL && OpPoint::curOpp())       PaintBL(painter, OpPoint::curOpp(), m_fFoilScale);
	}


	// Write Titles and results
	QString strong;

	painter.setFont(Settings::s_TextFont);
	int D = 0;
	int ZPos = m_rCltRect.bottom();
	int XPos = m_rCltRect.right()-10;
	QPen WritePen(Settings::s_TextColor);
	painter.setPen(WritePen);

	QFontMetrics fm(Settings::s_TextFont);
	int dD = fm.height();

	//write the foil's properties

	int Back = 5;

	if(m_BufferFoil.m_bTEFlap) Back +=3;

	int LeftPos = m_rCltRect.left()+10;
	ZPos = m_rCltRect.bottom() - 10 - Back*dD;

	D = 0;
	str1 = m_BufferFoil.m_FoilName;
	painter.drawText(LeftPos,ZPos+D, str1+str);
	D += dD;

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

	if(OpPoint::curOpp())
	{
		Back = 12;
		if(OpPoint::curOpp()->m_bTEFlap) Back++;
		if(OpPoint::curOpp()->m_bLEFlap) Back++;
		if(OpPoint::curOpp()->m_bViscResults && qAbs(OpPoint::curOpp()->Cd)>0.0) Back++;
		if(Polar::curPolar()->polarType()==FIXEDLIFTPOLAR) Back++;
		if(Polar::curPolar()->polarType()!=FIXEDSPEEDPOLAR && Polar::curPolar()->polarType()!=FIXEDAOAPOLAR) Back++;
	}

	int dwidth = fm.width(tr("TE Hinge Moment/span = 0123456789"));

	ZPos = m_rCltRect.bottom()-Back*dD - 10;
	XPos = m_rCltRect.right()-dwidth-20;
	D=0;


	if(Polar::curPolar())
	{
		if(Polar::curPolar()->polarType()==FIXEDSPEEDPOLAR)       str1 = tr("Fixed speed polar");
		else if(Polar::curPolar()->polarType()==FIXEDLIFTPOLAR)   str1 = tr("Fixed lift polar");
		else if(Polar::curPolar()->polarType()==RUBBERCHORDPOLAR) str1 = tr("Rubber chord polar");
		else if(Polar::curPolar()->polarType()==FIXEDAOAPOLAR)    str1 = tr("Fixed a.o.a. polar");

		painter.drawText(XPos,ZPos, dwidth, dD, Qt::AlignRight | Qt::AlignTop, str1);
		D += dD;
		if(Polar::curPolar()->polarType() ==FIXEDSPEEDPOLAR)
		{
			ReynoldsFormat(strong, Polar::curPolar()->m_Reynolds);
			strong ="Reynolds = " + strong;
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
			D += dD;
			strong = QString("Mach = %1").arg( Polar::curPolar()->m_Mach,9,'f',3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
			D += dD;
		}
		else if(Polar::curPolar()->polarType()==FIXEDLIFTPOLAR)
		{
			ReynoldsFormat(strong, Polar::curPolar()->m_Reynolds);
			strong = tr("Re.sqrt(Cl) = ") + strong;
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
			D += dD;

			strong = QString(tr("M.sqrt(Cl) = %1")).arg(Polar::curPolar()->m_Mach,9,'f',3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
			D += dD;
		}
		else if(Polar::curPolar()->polarType()==RUBBERCHORDPOLAR)
		{
			ReynoldsFormat(strong, Polar::curPolar()->m_Reynolds);
			strong = tr("Re.sqrt(Cl) = ") + strong;
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
			D += dD;

			strong = QString("Mach = %1").arg(Polar::curPolar()->m_Mach,9,'f',3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
			D += dD;
		}
		else if(Polar::curPolar()->polarType()==FIXEDAOAPOLAR)
		{
			strong = QString("Alpha = %1 "+QString::fromUtf8("°")).arg(Polar::curPolar()->m_ASpec,10,'f',2);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
			D += dD;
			strong = QString("Mach = %1").arg(Polar::curPolar()->m_Mach,9,'f',3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
			D += dD;
		}

		strong = QString("NCrit = %1").arg(Polar::curPolar()->m_ACrit,9,'f',3);
		painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
		D += dD;

		strong = QString(tr("Forced Upper Trans. = %1")).arg(Polar::curPolar()->m_XTop,9,'f',3);
		painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
		D += dD;
		strong = QString(tr("Forced Lower Trans. = %1")).arg(Polar::curPolar()->m_XBot, 9, 'f', 3);
		painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, strong);
		D += dD;

		if(OpPoint::curOpp())
		{
			if(Polar::curPolar()->polarType()!=FIXEDSPEEDPOLAR)
			{
				ReynoldsFormat(Result, OpPoint::curOpp()->Reynolds);
				Result = "Re = "+ Result;
				painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
				D += dD;
			}
			if(Polar::curPolar()->polarType()==FIXEDLIFTPOLAR)
			{
				Result = QString("Ma = %1").arg(OpPoint::curOpp()->Mach, 9, 'f', 4);
				painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
				D += dD;
			}
			if(Polar::curPolar()->polarType()!=FIXEDAOAPOLAR)
			{
				Result = QString(tr("Alpha = %1")+QString::fromUtf8("°")).arg(OpPoint::curOpp()->Alpha, 8, 'f', 2);
				painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
				D += dD;
			}
			Result = QString(tr("Cl = %1")).arg(OpPoint::curOpp()->Cl, 9, 'f', 3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
			D += dD;

			Result = QString(tr("Cm = %1")).arg(OpPoint::curOpp()->Cm, 9, 'f', 3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
			D += dD;

			Result = QString(tr("Cd = %1")).arg(OpPoint::curOpp()->Cd, 9, 'f', 3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
			D += dD;

			if(OpPoint::curOpp()->m_bViscResults && qAbs(OpPoint::curOpp()->Cd)>0.0)
			{
				Result = QString(tr("L/D = %1")).arg(OpPoint::curOpp()->Cl/OpPoint::curOpp()->Cd, 9, 'f', 3);
				painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
				D += dD;
			}

			Result = QString(tr("Upper Trans. = %1")).arg(OpPoint::curOpp()->Xtr1, 9, 'f', 3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
			D += dD;

			Result = QString(tr("Lower Trans. = %1")).arg(OpPoint::curOpp()->Xtr2, 9, 'f', 3);
			painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
			D += dD;

			if(OpPoint::curOpp()->m_bTEFlap)
			{
				Result = QString(tr("TE Hinge Moment/span = %1")).arg(OpPoint::curOpp()->m_TEHMom, 9, 'e', 2);
				painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
				D += dD;
			}

			if(OpPoint::curOpp()->m_bLEFlap)
			{
				Result = QString(tr("LE Hinge Moment/span = %1")).arg(OpPoint::curOpp()->m_LEHMom, 9, 'e', 2);
				painter.drawText(XPos,ZPos+D, dwidth, dD, Qt::AlignRight | Qt::AlignTop, Result);
				D += dD;
			}
		}
	}
}


/**
 * Paints all polar graphs
 * @param painter a reference to the QPainter object with which to draw
 */
void QXDirect::PaintPolarGraphs(QPainter &painter)
{
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

	painter.fillRect(Rect5, Settings::s_BackgroundColor);

	m_PlrGraph[0].DrawGraph(Rect1, painter);
	m_PlrGraph[1].DrawGraph(Rect2, painter);
	m_PlrGraph[2].DrawGraph(Rect3, painter);
	m_PlrGraph[3].DrawGraph(Rect4, painter);

	if(m_bShowUserGraph)	m_PlrGraph[4].DrawGraph(Rect6, painter);
	else				 	painter.fillRect(Rect6, Settings::s_BackgroundColor);

	PaintPolarLegend(m_PolarLegendOffset,  h, painter);
}


/**
 * Paints the legend of the polar graphs
 * @param place the top-left point where the legend will be placed
 * @param bottom the number of pixels to the bottom of the client area
 * @param painter a reference to the QPainter object with which to draw
 *@todo position improvement required for the two graph display
 */
void QXDirect::PaintPolarLegend(QPoint place, int bottom, QPainter &painter)
{
	int LegendSize, LegendWidth, legendHeight, x1;
	int i,j,k,l,nc,ny,nFoils;

	LegendSize = 30;
	LegendWidth = 240;

	painter.setFont(Settings::s_TextFont);

	QFont fnt(Settings::s_TextFont); //two step to shut valgrind up
	QFontMetrics fm(fnt);
	legendHeight = fm.height()+1;

	QPen TextPen(Settings::s_TextColor);
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
			if (pPolar->foilName() == pFoil->foilName() && pPolar->m_Alpha.size() && pPolar->isVisible())
			{
				str.append(pFoil->foilName());
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
				pPolar->foilName() == str.at(k) &&
								((pPolar->m_PolarType==FIXEDSPEEDPOLAR  && m_bType1) ||
								 (pPolar->m_PolarType==FIXEDLIFTPOLAR   && m_bType2) ||
								 (pPolar->m_PolarType==RUBBERCHORDPOLAR && m_bType3) ||
								 (pPolar->m_PolarType==FIXEDAOAPOLAR    && m_bType4)))
					FoilPlrs++;
		}
		if (FoilPlrs)
		{
			int YBotPos = place.y() + (ny+FoilPlrs+2) * legendHeight;// bottom line of this foil's legend
			if(abs(bottom) > abs(YBotPos))
			{
				ny++;
			}
			else if (k>0)
			{
			// move rigth if less than client bottom area
				place.rx() += LegendWidth;
				ny=1;
			}
			else
			{
				ny=1;
			}
			painter.setPen(TextPen);
			painter.drawText(place.x() + (int)(0.5*LegendSize), place.y() + legendHeight*ny-(int)(legendHeight/2),
							 str.at(k));
		}
		for (nc=0; nc<m_poaPolar->size(); nc++)
		{
			pPolar = (Polar*)m_poaPolar->at(nc);
			if(str.at(k) == pPolar->foilName())
			{
				if (pPolar->m_Alpha.size() && pPolar->m_PlrName.length() && pPolar->m_bIsVisible &&
										((pPolar->m_PolarType==FIXEDSPEEDPOLAR  && m_bType1) ||
										 (pPolar->m_PolarType==FIXEDLIFTPOLAR   && m_bType2) ||
										 (pPolar->m_PolarType==RUBBERCHORDPOLAR && m_bType3) ||
										 (pPolar->m_PolarType==FIXEDAOAPOLAR    && m_bType4)))
				{
					//is there anything to draw ?
					LegendPen.setColor(pPolar->m_Color);
					LegendPen.setStyle(getStyle(pPolar->m_Style));
					LegendPen.setWidth(pPolar->m_Width);
					painter.setPen(LegendPen);

					painter.drawLine(place.x() + (int)(1.0*LegendSize), place.y() + (int)(1.*legendHeight*ny)+2,
									 place.x() + (int)(2.0*LegendSize), place.y() + (int)(1.*legendHeight*ny)+2);
					if(pPolar->m_bShowPoints)
					{
						x1 = place.x() + (int)(1.5*LegendSize);
						painter.drawRect(x1-2, place.y() + (int)(1.*legendHeight*ny), 4, 4);
					}

					painter.setPen(TextPen);
					painter.drawText(place.x() + (int)(2.5*LegendSize), place.y() + (int)(1.*legendHeight*ny)+(int)(legendHeight/3),
									 pPolar->m_PlrName);
					ny++ ;
				}
			}
		}
		if (FoilPlrs) ny++;
	}
//	painter.setBackgroundMode(Qt::OpaqueMode);

}


/**
 * Paints a single polar graph, referenced by it index m_iPlrGraph
 * @param painter a reference to the QPainter object with which to draw
 */
void QXDirect::PaintSingleGraph(QPainter &painter)
{
	int h   = m_rCltRect.height();
	int w   = m_rCltRect.width();
	int w3  = (int)(w/3);

	QRect Rect1(0,0,2*w3,h);
	QRect Rect2(2*w3, 0, w-2*w3,h);
	painter.fillRect(Rect2, Settings::s_BackgroundColor);

	if(m_iPlrGraph>=0 && m_iPlrGraph<MAXPOLARGRAPHS) m_pCurGraph = m_PlrGraph + m_iPlrGraph;
	else                                             m_pCurGraph = NULL;

	if (m_pCurGraph) m_pCurGraph->DrawGraph(Rect1, painter);
	PaintPolarLegend(m_PolarLegendOffset, h, painter);
}


/**
 * Paints the view. Calls either the painting of the OpPoint or the polar view.
 * @param painter a reference to the QPainter object with which to draw
 */
void QXDirect::PaintView(QPainter &painter)
{
	if(Foil::curFoil() && !m_bPolarView)
	{
		PaintOpPoint(painter);
	}
	else if (m_bPolarView)
	{
		if(m_iPlrView==ALLGRAPHS)      PaintPolarGraphs(painter);
		else if(m_iPlrView==ONEGRAPH)  PaintSingleGraph(painter);
		else if(m_iPlrView==TWOGRAPHS) PaintCoupleGraphs(painter);
	}
	else
	{
		painter.fillRect(m_rCltRect, Settings::s_BackgroundColor);
	}
}


/**
 * Reads the analysis parameters from the widgets.
 */
void QXDirect::ReadParams()
{
	if(!Polar::curPolar()) return;

	if      (m_pctrlSpec1->isChecked()) s_bAlpha = true;
	else if (m_pctrlSpec2->isChecked()) s_bAlpha = false;
	else if (m_pctrlSpec3->isChecked()) s_bAlpha = false;


	if(Polar::curPolar()->polarType()!=FIXEDAOAPOLAR)
	{
		if(s_bAlpha)
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
	s_bInitBL   = m_pctrlInitBL->isChecked();
	s_bViscous  = m_pctrlViscous->isChecked();
	OpPoint::s_bStoreOpp = m_pctrlStoreOpp->isChecked();
}


/**
 * Saves the user-defined settings
 * @param pSettings a pointer to the QSetting object.
 */
void QXDirect::SaveSettings(QSettings *pSettings)
{
	QString str1, str2, str3;
	pSettings->beginGroup("XDirect");
	{
		pSettings->setValue("AlphaSpec", s_bAlpha);
		pSettings->setValue("StoreOpp", OpPoint::s_bStoreOpp);
		pSettings->setValue("ViscousAnalysis", s_bViscous);
		pSettings->setValue("InitBL", s_bInitBL);
		pSettings->setValue("BoundaryLayer", m_bBL);
		pSettings->setValue("Pressure", m_bPressure);
		pSettings->setValue("PolarView", m_bPolarView);
		pSettings->setValue("UserGraph", m_bShowUserGraph);
		pSettings->setValue("ShowPanels", m_bShowPanels);
		pSettings->setValue("Type1", m_bType1);
		pSettings->setValue("Type2", m_bType2);
		pSettings->setValue("Type3", m_bType3);
		pSettings->setValue("Type4", m_bType4);
		pSettings->setValue("FromList", m_bFromList);
		pSettings->setValue("FromZero", s_bFromZero);
		pSettings->setValue("TextOutput", m_bShowTextOutput);
		pSettings->setValue("NeutralLine", m_bNeutralLine);
		pSettings->setValue("CurOppOnly", m_bCurOppOnly);
		pSettings->setValue("ShowInviscid", m_bShowInviscid);
		pSettings->setValue("ShowCpGraph", m_bCpGraph);
		pSettings->setValue("Sequence", m_bSequence);
		pSettings->setValue("HighlightOpp",m_bHighlightOpp);
		pSettings->setValue("BLColor", m_crBLColor);
		pSettings->setValue("BLStyle", m_iBLStyle);
		pSettings->setValue("BLWidth", m_iBLWidth);
		pSettings->setValue("PressureColor", m_crPressureColor);
		pSettings->setValue("PressureStyle", m_iPressureStyle);
		pSettings->setValue("PressureWidth", m_iPressureWidth);
		pSettings->setValue("NeutralColor", m_crNeutralColor);
		pSettings->setValue("NeutralStyle", m_iNeutralStyle);
		pSettings->setValue("NeutralWidth", m_iNeutralWidth);
		pSettings->setValue("XFoilVar", m_XFoilVar);
		pSettings->setValue("TimeUpdateInterval", s_TimeUpdateInterval);
		pSettings->setValue("BatchUpdatePolarView", BatchThreadDlg::s_bUpdatePolarView);
		pSettings->setValue("PlrGraph", m_iPlrGraph);

		switch(m_iPlrView)
		{
			case ONEGRAPH:
				pSettings->setValue("PlrView", 1);
				break;
			case TWOGRAPHS:
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

		pSettings->setValue("AutoInitBL", XFoilTask::s_bAutoInitBL);
		pSettings->setValue("IterLim", XFoilTask::s_IterLim);
		pSettings->setValue("FullReport", XFoil::s_bFullReport);


		pSettings->setValue("VAccel", m_pXFoil->vaccel);
		pSettings->setValue("KeepOpenErrors", s_bKeepOpenErrors);
		pSettings->setValue("NCrit", s_refPolar.m_ACrit);
		pSettings->setValue("XTopTr", s_refPolar.m_XTop);
		pSettings->setValue("XBotTr", s_refPolar.m_XBot);
		pSettings->setValue("Mach", s_refPolar.m_Mach);
		pSettings->setValue("ASpec", s_refPolar.m_ASpec);

		if(s_refPolar.polarType()==FIXEDSPEEDPOLAR)      pSettings->setValue("Type", 1);
		else if(s_refPolar.polarType()==FIXEDSPEEDPOLAR) pSettings->setValue("Type", 2);
		else if(s_refPolar.polarType()==FIXEDAOAPOLAR)   pSettings->setValue("Type", 4);
		else if(s_refPolar.polarType()==STABILITYPOLAR)  pSettings->setValue("Type", 7);

		pSettings->setValue("NReynolds", s_ReList.count());
		for (int i=0; i<s_ReList.count(); i++)
		{
			str1 = QString("ReList%1").arg(i);
			str2 = QString("MaList%1").arg(i);
			str3 = QString("NcList%1").arg(i);
			pSettings->setValue(str1, s_ReList[i]);
			pSettings->setValue(str2, s_MachList[i]);
			pSettings->setValue(str3, s_NCritList[i]);
		}
	}
	pSettings->endGroup();

	m_PlrGraph[0].SaveSettings(pSettings);
	m_PlrGraph[1].SaveSettings(pSettings);
	m_PlrGraph[2].SaveSettings(pSettings);
	m_PlrGraph[3].SaveSettings(pSettings);
	m_PlrGraph[4].SaveSettings(pSettings);
	m_CpGraph.SaveSettings(pSettings);
}



/**
 * Initializes the widget values, depending on the type of Polar
 */
void QXDirect::SetAnalysisParams()
{
	m_pctrlViscous->setChecked(s_bViscous);
	m_pctrlInitBL->setChecked(s_bInitBL);
	m_pctrlStoreOpp->setChecked(OpPoint::s_bStoreOpp);
	m_pctrlShowPressure->setChecked(m_bPressure);
	m_pctrlShowBL->setChecked(m_bBL);

	if(Polar::curPolar())
	{
		if(Polar::curPolar()->polarType()!=FIXEDAOAPOLAR)
		{
			m_pctrlAlphaMin->SetPrecision(2);
			m_pctrlAlphaMax->SetPrecision(2);
			m_pctrlAlphaDelta->SetPrecision(2);
			if(s_bAlpha) m_pctrlSpec1->setChecked(true);
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
		if(s_bAlpha) m_pctrlSpec1->setChecked(true);
		else         m_pctrlSpec2->setChecked(true);
		m_pctrlSpec3->setEnabled(false);
	}
	SetOpPointSequence();

	SetCurveParams();
}


/**
 * Sets the buffer Foil as a copy of the active Foil.
 * All geometric modifications are made on the buffer foil.
 * The buffer foil is the one displayed in the OpPoint view.
 */
void QXDirect::SetBufferFoil()
{
	if(!Foil::curFoil() || !Foil::curFoil()->foilName().length()) return;

	m_BufferFoil.CopyFoil(Foil::curFoil());

	m_BufferFoil.m_FoilName   = Foil::curFoil()->foilName();
	m_BufferFoil.m_FoilColor  = Foil::curFoil()->m_FoilColor;
	m_BufferFoil.m_FoilStyle = Foil::curFoil()->m_FoilStyle;
	m_BufferFoil.m_FoilWidth = Foil::curFoil()->m_FoilWidth;
}


/**
 * Updates the combobox widgets with the curve data from the active OpPoint or Polar, depending on the active view.
 */
void QXDirect::SetCurveParams()
{
	if(m_bPolarView)
	{
		if(Polar::curPolar())
		{
			if(Polar::curPolar()->m_bIsVisible)  m_pctrlShowCurve->setChecked(true);  else  m_pctrlShowCurve->setChecked(false);
			if(Polar::curPolar()->m_bShowPoints) m_pctrlShowPoints->setChecked(true); else  m_pctrlShowPoints->setChecked(false);

			m_CurveColor = Polar::curPolar()->m_Color;
			m_CurveStyle = Polar::curPolar()->m_Style;
			m_CurveWidth = Polar::curPolar()->m_Width;
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
		if(OpPoint::curOpp())
		{
			if(OpPoint::curOpp()->m_bIsVisible)  m_pctrlShowCurve->setChecked(true);  else  m_pctrlShowCurve->setChecked(false);
			if(OpPoint::curOpp()->m_bShowPoints) m_pctrlShowPoints->setChecked(true); else  m_pctrlShowPoints->setChecked(false);

			m_CurveColor = OpPoint::curOpp()->m_Color;
			m_CurveStyle = OpPoint::curOpp()->m_Style;
			m_CurveWidth = OpPoint::curOpp()->m_Width;
			FillComboBoxes();
		}
		else
		{
			FillComboBoxes(false);
		}
	}
	if(Polar::curPolar())
	{
		if(Polar::curPolar()->polarType()!=FIXEDAOAPOLAR)
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


/**
 * Initializes QXDirect with the data of the input Foil object.
 * If no Foil is proposed in input,sets the first stock Foil in alphabetical order.
 * Sets the first Polar object belonging to this Foil, if any.
 * Sets the first OpPoint object belonging to this Polar, if any.
 * @param pFoil a pointer to the active Foil object, or NULL if a stock Foil should be used.
 * @return a pointer to the Foil object which has been set.
 */
Foil* QXDirect::SetFoil(Foil* pFoil)
{
	StopAnimate();

	Foil::setCurFoil(pFoil);

	if(!Foil::curFoil())
	{
		//take the first in the array, if any
		if(m_poaFoil->size())
		{
			Foil::setCurFoil((Foil*)m_poaFoil->at(0));
		}
	}

	if(Foil::curFoil() && !m_pXFoil->InitXFoilGeometry(Foil::curFoil()))
	{
		Foil::setCurFoil(NULL);
	}
	else
	{
		if(!Foil::curFoil())
		{
			Polar::setCurPolar(NULL);
			OpPoint::setCurOpp(NULL);
		}
		else if (Polar::curPolar() && Polar::curPolar()->foilName() !=Foil::curFoil()->foilName())
		{
//			Polar::setCurPolar(NULL);
//			OpPoint::setCurOpp(NULL);
		}
		else if (OpPoint::curOpp() && OpPoint::curOpp()->m_strFoilName  !=Foil::curFoil()->foilName())
		{
//			OpPoint::setCurOpp(NULL);
		}
	}

	SetBufferFoil();

	SetPolar();

	return Foil::curFoil();
}


/**
 * Sets the Foil scale in the OpPoint view.
 * @param CltRect the size of the client rectangle
 */
void QXDirect::SetFoilScale(QRect CltRect)
{
	m_rCltRect = CltRect;

	int w23 = (int)(2./3.*(double)m_rCltRect.width());
	m_PolarLegendOffset = QPoint(w23+10, 10);

	SetFoilScale();
}


/**
 * Sets the Foil scale in the OpPoint view.
 */
void QXDirect::SetFoilScale()
{
	QRect rect(10, 10, + m_rCltRect.width()-20, m_rCltRect.height()-2*m_FoilYPos);
	m_CpGraph.SetDrawRect(rect);

	m_FoilOffset.rx() = rect.left() +(int)(1.0*m_CpGraph.GetMargin());
	m_FoilOffset.ry() = m_rCltRect.bottom()-m_FoilYPos;

	m_fFoilScale = (rect.width()-2.0*m_CpGraph.GetMargin());
	if(m_CpGraph.GetYVariable()>=2)
	{
		double p0  = m_CpGraph.xToClient(0.0);
		double p1  = m_CpGraph.xToClient(1.0);
		m_fFoilScale =  (p1-p0);
	}
}



/**
 * Initializes QXDirect with the specified Polar object.
 * If the specified polar is not valid, a stock polar associated to the current foil will be set.
 * Sets the first OpPoint object belonging to this Polar, if any.
 * Initializes the XFoil object with the Polar's data.
 * @param pPolar a pointer to the Polar object to set. If NULL, a stock polar associated to the current foil will be set.
 * @return a pointer to the Polar object which has been set.
 */
Polar * QXDirect::SetPolar(Polar *pPolar)
{
	StopAnimate();

	if(!Foil::curFoil()|| !Foil::curFoil()->foilName().length())
	{
		Polar::setCurPolar(NULL);
		SetAnalysisParams();
		return NULL;
	}

	if(pPolar) Polar::setCurPolar(pPolar);

	if(!Polar::curPolar())
	{
		//try to get one from the object array
		for(int i=0; i<m_poaPolar->size(); i++)
		{
			pPolar = (Polar*)m_poaPolar->at(i);
			if(pPolar && pPolar->foilName()==Foil::curFoil()->foilName())
			{
				//set this one
				Polar::setCurPolar(pPolar);
				break;
			}
		}
	}

	if(Polar::curPolar())
	{
		if(Polar::curPolar()->foilName() != Foil::curFoil()->foilName())
		{
			Polar *pOldPolar;
			bool bFound = false;
			for (int i=0; i<m_poaPolar->size(); i++)
			{
				pOldPolar = (Polar*)m_poaPolar->at(i);
				if ((pOldPolar->foilName() == Foil::curFoil()->foilName()) &&
					(pOldPolar->m_PlrName == Polar::curPolar()->polarName()))
				{
					Polar::setCurPolar(pOldPolar);
					bFound = true;
					break;
				}
			}
			if(!bFound)
			{
				Polar::setCurPolar(NULL);
				OpPoint::setCurOpp(NULL);
			}
		}
		s_bInitBL = true;
		m_pctrlInitBL->setChecked(s_bInitBL);
	}

//	m_pXFoil->InitXFoilAnalysis(Polar::curPolar(), s_bViscous); //useless, will be done in XFoilTask
	if (m_bPolarView) CreatePolarCurves();
	SetAnalysisParams();
	SetOpp();

	return Polar::curPolar();
}


/**
 * Initializes QXDirect with the OpPoint with the specified aoa.
 * If the OpPoint cannot be found for the active Foil and Polar, a stock OpPoint associated to the current foil and polar will be set.
 * @param Alpha the aoa of the OpPoint to ser
 * @return a pointer to the OpPoint object which has been set.
 */
OpPoint * QXDirect::SetOpp(double Alpha)
{
	OpPoint * pOpp = NULL;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	if(!Foil::curFoil())  return NULL;
	if(!Polar::curPolar()) return NULL;

	if(Alpha < -1234567.0) //the default
	{
		if(OpPoint::curOpp() && OpPoint::curOpp()->m_strFoilName == Foil::curFoil()->foilName())
			pOpp = OpPoint::curOpp();
		else
		{
			//try to get one from the array
			for(int iOpp=0; iOpp<OpPoint::s_oaOpp.count(); iOpp++)
			{
				pOpp = (OpPoint*)OpPoint::s_oaOpp.at(iOpp);
				if(pOpp->foilName()==Foil::curFoil()->foilName() && pOpp->polarName()==Polar::curPolar()->polarName())
				{
					break;
				}
			}
		}
	}
	else
	{
		pOpp = OpPoint::getOpp(Alpha);
	}


	if(pOpp) 
	{
		pMainFrame->SelectOpPoint(pOpp);
	}
	OpPoint::setCurOpp(pOpp);

	if(!m_bPolarView) CreateOppCurves();
	else
	{
		if(m_bHighlightOpp) CreatePolarCurves();
	}

	SetControls();
	SetCurveParams();

	return OpPoint::curOpp();
}


/**
 * Initializes the widgets with the sequence parameters for the type of the active Polar object.
 */
void QXDirect::SetOpPointSequence()
{
	m_pctrlSequence->setEnabled(Polar::curPolar());
	m_pctrlAlphaMin->setEnabled(Polar::curPolar());
	m_pctrlAnalyze->setEnabled(Polar::curPolar());
	m_pctrlViscous->setEnabled(Polar::curPolar());
	m_pctrlInitBL->setEnabled(Polar::curPolar());
	m_pctrlStoreOpp->setEnabled(Polar::curPolar());

	if(m_bSequence && Polar::curPolar())
	{
		m_pctrlSequence->setCheckState(Qt::Checked);
		m_pctrlAlphaMax->setEnabled(true);
		m_pctrlAlphaDelta->setEnabled(true);
	}
	else if (Polar::curPolar())
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


	if(Polar::curPolar() && Polar::curPolar()->polarType()!=FIXEDAOAPOLAR)
	{
		if(m_pctrlSpec3->isChecked())
		{
			m_pctrlSpec1->setChecked(true);
			s_bAlpha = true;
		}

		if(s_bAlpha)
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
	else if(Polar::curPolar() && Polar::curPolar()->polarType()==FIXEDAOAPOLAR)
	{
		m_pctrlSpec3->setChecked(true);
		s_bAlpha = true;		// no choice with type 4 polars
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


/**
 * Sets the axis titles for the specified graph
 * @param pGraph a pointer to the Graph object for which the titles will be set
 */
void QXDirect::SetGraphTitles(Graph* pGraph)
{
	if(!pGraph) return;

	QString Title;
	Title = Polar::variableName(pGraph->GetXVariable());
	pGraph->SetXTitle(Title);

	Title = Polar::variableName(pGraph->GetYVariable());
	pGraph->SetYTitle(Title);

}


/**
 * Sets the position of the polar graph legend, depending on the number of requested graphs
 */
void QXDirect::SetPolarLegendPos()
{
	int h   = m_rCltRect.height();
	int w   = m_rCltRect.width();
	int h2  = (int)(h/2);
	int h23 = (int)(2*h/3);
	int w3  = (int)(w/3);
	int w23 = 2*w3;
	int margin = 10;

	if(m_iPlrView == ONEGRAPH)
	{
		m_PolarLegendOffset.rx() = w23+margin;
		m_PolarLegendOffset.ry() = margin;
	}
	else if (m_iPlrView == TWOGRAPHS)
	{
		m_PolarLegendOffset.rx() = margin;
		m_PolarLegendOffset.ry() = h23+margin;
	}
	else if	(m_iPlrView == ALLGRAPHS)
	{
		m_PolarLegendOffset.rx() = margin;
		m_PolarLegendOffset.ry() = h2+30;
	}
}


/**
 * Creates the GUI associated to the toolbar.
 */
void QXDirect::SetupLayout()
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


	QGroupBox *AnalysisBox = new QGroupBox(tr("Analysis settings"));
	{
		QVBoxLayout *AnalysisGroup = new QVBoxLayout;
		{
			m_pctrlSequence = new QCheckBox(tr("Sequence"));
			m_pctrlStoreOpp = new QCheckBox(tr("Store Opp"));
			m_pctrlAnalyze  = new QPushButton(tr("Analyze"));

			QHBoxLayout *SpecVarsLayout = new QHBoxLayout;
			{
				m_pctrlSpec1 = new QRadioButton("a");
				m_pctrlSpec2 = new QRadioButton(tr("Cl"));
				m_pctrlSpec3 = new QRadioButton(tr("Re"));
				m_pctrlSpec1->setFont(QFont("Symbol"));
				SpecVarsLayout->addWidget(m_pctrlSpec1);
				SpecVarsLayout->addWidget(m_pctrlSpec2);
				SpecVarsLayout->addWidget(m_pctrlSpec3);
			}

			QGridLayout *SequenceGroupLayout = new QGridLayout;
			{
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

				m_pctrlAlphaMin   = new DoubleEdit();
				m_pctrlAlphaMax   = new DoubleEdit();
				m_pctrlAlphaDelta = new DoubleEdit();
				m_pctrlAlphaMin->setMinimumHeight(20);
				m_pctrlAlphaMax->setMinimumHeight(20);
				m_pctrlAlphaDelta->setMinimumHeight(20);
				m_pctrlAlphaMin->setAlignment(Qt::AlignRight);
				m_pctrlAlphaMax->setAlignment(Qt::AlignRight);
				m_pctrlAlphaDelta->setAlignment(Qt::AlignRight);
				SequenceGroupLayout->addWidget(AlphaMinLab,1,1);
				SequenceGroupLayout->addWidget(AlphaMaxLab,2,1);
				SequenceGroupLayout->addWidget(DeltaAlphaLab,3,1);
				SequenceGroupLayout->addWidget(m_pctrlAlphaMin,1,2);
				SequenceGroupLayout->addWidget(m_pctrlAlphaMax,2,2);
				SequenceGroupLayout->addWidget(m_pctrlAlphaDelta,3,2);
				SequenceGroupLayout->addWidget(m_pctrlUnit1,1,3);
				SequenceGroupLayout->addWidget(m_pctrlUnit2,2,3);
				SequenceGroupLayout->addWidget(m_pctrlUnit3,3,3);
			}

			QHBoxLayout *AnalysisSettings = new QHBoxLayout;
			{
				m_pctrlViscous  = new QCheckBox(tr("Viscous"));
				m_pctrlInitBL   = new QCheckBox(tr("Init BL"));
				AnalysisSettings->addWidget(m_pctrlViscous);
				AnalysisSettings->addWidget(m_pctrlInitBL);
			}

			AnalysisGroup->addLayout(SpecVarsLayout);
			AnalysisGroup->addStretch(1);
			AnalysisGroup->addWidget(m_pctrlSequence);
			AnalysisGroup->addLayout(SequenceGroupLayout);
			AnalysisGroup->addStretch(1);
			AnalysisGroup->addLayout(AnalysisSettings);
			AnalysisGroup->addWidget(m_pctrlStoreOpp);
			AnalysisGroup->addWidget(m_pctrlAnalyze);
		}
		AnalysisBox->setLayout(AnalysisGroup);

	}


	QGroupBox *DisplayBox = new QGroupBox(tr("Display"));
	{
		QVBoxLayout *DisplayGroup = new QVBoxLayout;
		{
			m_pctrlShowBL        = new QCheckBox(tr("Show BL"));
			m_pctrlShowPressure  = new QCheckBox(tr("Show Pressure"));
			m_pctrlAnimate       = new QCheckBox(tr("Animate"));
			m_pctrlAnimateSpeed  = new QSlider(Qt::Horizontal);
			m_pctrlAnimateSpeed->setMinimum(0);
			m_pctrlAnimateSpeed->setMaximum(1000);
			m_pctrlAnimateSpeed->setSliderPosition(500);
			m_pctrlAnimateSpeed->setTickInterval(50);
			m_pctrlAnimateSpeed->setTickPosition(QSlider::TicksBelow);
			DisplayGroup->addWidget(m_pctrlShowBL);
			DisplayGroup->addWidget(m_pctrlShowPressure);
			DisplayGroup->addWidget(m_pctrlAnimate);
			DisplayGroup->addWidget(m_pctrlAnimateSpeed);
			DisplayGroup->addStretch(1);
		//	DisplayGroup->addWidget(m_pctrlHighlightOpp);
		}
		DisplayBox->setLayout(DisplayGroup);
		DisplayBox->setSizePolicy(szPolicyExpanding);
	}
	QGroupBox *pPolarPropsBox = new QGroupBox(tr("Polar properties"));
	{
		m_pctrlPolarProps = new QLabel;
		m_pctrlPolarProps->setSizePolicy(szPolicyExpanding);

	//	m_pctrlPolarProps->setReadOnly(true);
	//	m_pctrlPolarProps->setWordWrapMode(QTextOption::NoWrap);
		QHBoxLayout *pPolarPropsLayout = new QHBoxLayout;
		{
			pPolarPropsLayout->addWidget(m_pctrlPolarProps);
		}
		pPolarPropsBox->setLayout(pPolarPropsLayout);
	}

	QGroupBox *pCurveBox = new QGroupBox(tr("Graph Curve Settings"));
	{
		QVBoxLayout *pCurveGroup = new QVBoxLayout;
		{
			QHBoxLayout *pCurveDisplay = new QHBoxLayout;
			{
				m_pctrlShowCurve  = new QCheckBox(tr("Curve"));
				m_pctrlShowPoints = new QCheckBox(tr("Points"));
				pCurveDisplay->addWidget(m_pctrlShowCurve);
				pCurveDisplay->addWidget(m_pctrlShowPoints);
			}

			m_pctrlCurveStyle = new LineCbBox(this);
			m_pctrlCurveWidth = new LineCbBox(this);
			m_pctrlCurveColor = new LineBtn(this);
			m_pctrlCurveColor->setMinimumHeight(m_pctrlCurveStyle->minimumSizeHint().height());

			for (int i=0; i<5; i++)
			{
				m_pctrlCurveStyle->addItem("item");
				m_pctrlCurveWidth->addItem("item");
			}
			m_pStyleDelegate = new LineDelegate(this);
			m_pWidthDelegate = new LineDelegate(this);
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

			pCurveGroup->addLayout(pCurveDisplay);
			pCurveGroup->addLayout(CurveStyleLayout);
			pCurveGroup->addStretch(1);
		}
		pCurveBox->setLayout(pCurveGroup);
	}


	QVBoxLayout *mainLayout = new QVBoxLayout;
	{
		m_pctrlMiddleControls = new QStackedWidget;
		m_pctrlMiddleControls->addWidget(DisplayBox);
		m_pctrlMiddleControls->addWidget(pPolarPropsBox);

		mainLayout->addWidget(AnalysisBox);
		mainLayout->addStretch(1);
		mainLayout->addWidget(m_pctrlMiddleControls);
		mainLayout->addStretch(1);
		mainLayout->addWidget(pCurveBox);
		mainLayout->addStretch(1);
	}

	setLayout(mainLayout);

	setAttribute(Qt::WA_AlwaysShowToolTips);

	setSizePolicy(szPolicyExpanding);
}


/**
 * Interrupts the OpPoint animation
 */
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


/**
 * Updates the curve's style based on the selection in the comboboxes.
 */
void QXDirect::UpdateCurveStyle()
{
	if(m_bPolarView && Polar::curPolar())
	{
		Polar::curPolar()->m_Color = m_CurveColor;
		Polar::curPolar()->m_Style = m_CurveStyle;
		Polar::curPolar()->m_Width = (int)m_CurveWidth;
		CreatePolarCurves();
	}
	else if (!m_bPolarView && OpPoint::curOpp())
	{
		OpPoint::curOpp()->m_Color = m_CurveColor;
		OpPoint::curOpp()->m_Style = m_CurveStyle;
		OpPoint::curOpp()->m_Width = (int)m_CurveWidth;
		CreateOppCurves();
	}

	UpdateView();
	emit projectModified();
}


/**
 * Refreshes the 2d central display.
 */
void QXDirect::UpdateView()
{
	TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;

	if(s_p2DWidget)
	{
		p2DWidget->update();
	}
}



/**
 * Overrides the QWidget's wheelEvent method.
 * Dispatches the event
 * @param event the QWheelEvent
 */
void QXDirect::zoomEvent(QPoint pos, double zoomFactor)
{
	m_pCurGraph = GetGraph(pos);

	if(m_pCurGraph && m_pCurGraph->IsInDrawRect(pos) && m_bCpGraph)
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

		if(!m_bAnimate) UpdateView();
	}
	else if(Foil::curFoil() && !m_bPolarView)
	{
		double scale = m_fFoilScale;

		m_fFoilScale *= zoomFactor;

		int a = (int)((m_rCltRect.right()+m_rCltRect.left())/2);

		m_FoilOffset.rx() = a + (int)((m_FoilOffset.x()-a)/scale*m_fFoilScale);

		if(!m_bAnimate) UpdateView();
	}
}


/**
 * The user has requested the duplication of the current Foil.
 */
void QXDirect::OnDuplicateFoil()
{
	if(!Foil::curFoil()) return;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	Foil *pNewFoil = new Foil;
	pNewFoil->CopyFoil(Foil::curFoil());

	if(AddNewFoil(pNewFoil))
	{
		pMainFrame->UpdateFoilListBox();
		SetFoil(pNewFoil);
		emit projectModified();
	}
}

/**
 * The user has requested to rename the current Foil.
 */
void QXDirect::OnRenameCurFoil()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;


	RenameFoil(Foil::curFoil());
	pMainFrame->UpdateFoilListBox();
	SetFoil(Foil::curFoil());
	emit projectModified();
}


/**
 * Adds a new Foil to the array.
 * Requests a name, and overwrites any former Foil with that name.
 * @param pFoil a pointer to the Foil to be added to the array
 * @return a pointer to the input Foil, or NULL if the operation was user-cancelled. @todo what's the point ?
 */
Foil* QXDirect::AddNewFoil(Foil *pFoil)
{
	if(!pFoil) return NULL;
	QStringList NameList;
	for(int k=0; k<Foil::s_oaFoil.size(); k++)
	{
		Foil*pOldFoil = (Foil*)Foil::s_oaFoil.at(k);
		NameList.append(pOldFoil->m_FoilName);
	}

	RenameDlg renDlg((MainFrame*)s_pMainFrame);
	renDlg.InitDialog(&NameList, pFoil->foilName(), tr("Enter the foil's new name"));

	if(renDlg.exec() != QDialog::Rejected)
	{
		pFoil->setFoilName(renDlg.newName());
		pFoil->insertThisFoil();
		return pFoil;
	}
	return NULL;
}



/**
 * Renames the current Foil.
 * Requests a name, and overwrites any former Foil with that name.
 * @param pFoil a pointer to the Foil to be renamed.
 */
void QXDirect::RenameFoil(Foil *pFoil)
{
	if(!pFoil) return;
	QStringList NameList;
	for(int k=0; k<Foil::s_oaFoil.size(); k++)
	{
		Foil*pOldFoil = (Foil*)Foil::s_oaFoil.at(k);
		NameList.append(pOldFoil->m_FoilName);
	}

	RenameDlg renDlg((MainFrame*)s_pMainFrame);
	renDlg.InitDialog(&NameList, pFoil->foilName(), tr("Enter the foil's new name"));

	if(renDlg.exec() != QDialog::Rejected)
	{
		pFoil->renameThisFoil(renDlg.newName());
	}
}



