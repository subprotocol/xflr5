/****************************************************************************

	XInverse Class
	Copyright (C) 2009 Andre Deperrois xflr5@yahoo.com

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

#include <QGroupBox>
#include <QGridLayout>
#include <QtGui>
 
#include "XInverse.h" 
#include "InverseOptionsDlg.h"
#include "FoilSelectionDlg.h"
#include "PertDlg.h"
#include "../Globals.h"
#include "../MainFrame.h"
#include "../Objects/Foil.h"
#include "../Graph/GraphDlg.h"


extern CFoil* g_pCurFoil;


QXInverse::QXInverse(QWidget *parent)
	: QWidget(parent)
{
	m_bFullInverse = false;

	m_pXFoil = NULL;
	m_pCurGraph = NULL;

	m_bTransGraph    = false;
	m_bLoaded        = false;
	m_bSaved         = true;
	m_bZoomPlus      = false;
	m_bZoomXOnly     = false;
	m_bZoomYOnly     = false;
	m_bShowPoints    = false;
	m_bRefCurves     = false;
	m_bTangentSpline = false;
	m_bReflected     = false;
	m_bMarked        = false;
	m_bTrans   = false;
	m_bSpline  = false;
	m_bSplined = true;
	m_bRefFoil = true;
	m_bModFoil = false;
	m_bGetPos  = false;
	m_bMark    = false;
	m_bMarked  = false;
	m_bSmooth  = false;
	m_bXPressed = m_bYPressed = false;

	m_pRefFoil = new CFoil();
	m_pModFoil = new CFoil();
	m_pRefFoil->m_FoilColor  = QColor(255,0,0);
	m_pRefFoil->m_nFoilStyle = 0;
	m_pRefFoil->m_nFoilWidth = 0;
	m_pModFoil->m_FoilColor  = QColor(0,0,255);
	m_pModFoil->m_nFoilStyle = 0;
	m_pModFoil->m_nFoilWidth = 1;

	m_Spline.InsertPoint(0.0,  0.0);
	m_Spline.InsertPoint(0.25, 0.0);
	m_Spline.InsertPoint(0.5,  0.0);
	m_Spline.InsertPoint(0.75, 0.0);
	m_Spline.InsertPoint(1.0,  0.0);
	m_Spline.SplineKnots();
	m_Spline.SplineCurve();

	m_Spline.SetStyle(0);
	m_Spline.SetWidth(1);
	m_Spline.SetColor(QColor(170,120, 0));

	m_ReflectedStyle = 1;
	m_ReflectedWidth = 1;
	m_ReflectedClr = QColor(170,120, 0);
	
	m_nPos    = 0;
	m_tmpPos  = -1;
	m_Pos1    = -1;
	m_Pos2    = -1;
	m_SplineLeftPos   = -1;
	m_SplineRightPos  = -1;

	m_QGraph.SetType(2);
	m_QGraph.SetDefaults();
	m_QGraph.SetXTitle(tr("x/c"));
	m_QGraph.SetYTitle(tr("Q/Vinf"));
	m_QGraph.SetXMin(0.0);
	m_QGraph.SetXMax(1.0);
	m_QGraph.SetYMin(-0.1);
	m_QGraph.SetYMax(0.1);
	m_QGraph.SetGraphName(tr("Q Graph"));
	m_pQCurve  = m_QGraph.AddCurve();
	m_pMCurve  = m_QGraph.AddCurve();
	m_pQVCurve = m_QGraph.AddCurve();
	m_pReflectedCurve = m_QGraph.AddCurve();
	m_pReflectedCurve->SetVisible(m_bReflected);

	SetupLayout();
	if(m_bFullInverse)
	{
		m_pctrlStackedInv->setCurrentIndex(0);
	}
	else
	{
		m_pctrlStackedInv->setCurrentIndex(1);
	}
}


void QXInverse::CancelMark()
{
	m_pctrlMark->setChecked(false);
	m_bGetPos = false;
	m_bMark   = false;
}

void QXInverse::CancelSmooth()
{
	m_bSmooth = false;
	m_bGetPos = false;
	m_pctrlSmooth->setChecked(false);
}


void QXInverse::CancelSpline()
{
	m_pctrlOutput->setPlainText(" ");
//	m_bSpline  = false;
	m_bSplined = false;
//	m_ctrlShowSpline.SetCheck(0);
	m_pctrlNewSpline->setChecked(false);
	m_bSmooth = false;
	m_bGetPos = false;
	m_nPos    = 0;
	m_tmpPos  = -1;
	m_Pos1    = -1;
	m_Pos2    = -1;
}




void QXInverse::CheckActions()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->InvQInitial->setChecked(m_pQCurve->IsVisible());
	pMainFrame->InvQSpec->setChecked(m_pMCurve->IsVisible());
	pMainFrame->InvQViscous->setChecked(m_pQVCurve->IsVisible());
	pMainFrame->InvQPoints->setChecked(m_bShowPoints);
	pMainFrame->InvQReflected->setChecked(m_bReflected);

	if(m_bFullInverse)
	{
		m_pctrlShowSpline->setChecked(m_bSpline);
		m_pctrlTangentSpline->setChecked(m_bTangentSpline);
	}
	else
	{
		XFoil *pXFoil = (XFoil*)m_pXFoil;
		m_pctrlMShowSpline->setChecked(m_bSpline);
		m_pctrlMTangentSpline->setChecked(m_bTangentSpline);
		m_pctrlCpxx->setChecked(pXFoil->lcpxx);
	}
}


void QXInverse::Clear()
{
	m_pRefFoil->n = 0;
	m_pRefFoil->m_FoilName = "";
	m_pModFoil->m_FoilName = "";
	m_bLoaded = false;
	m_pReflectedCurve->ResetCurve();
	m_pMCurve->ResetCurve();
	m_pQCurve->ResetCurve();
	m_pQVCurve->ResetCurve();
}



void QXInverse::Connect()
{
	connect(m_pctrlSpecAlpha,     SIGNAL(clicked()), this, SLOT(OnSpecal()));
	connect(m_pctrlSpecCl,        SIGNAL(clicked()), this, SLOT(OnSpecal()));
	connect(m_pctrlSpec,          SIGNAL(editingFinished()), this, SLOT(OnSpecInv()));
	connect(m_pctrlShowSpline,    SIGNAL(clicked()), this, SLOT(OnShowSpline()));
	connect(m_pctrlNewSpline,     SIGNAL(clicked()), this, SLOT(OnNewSpline()));
	connect(m_pctrlApplySpline,   SIGNAL(clicked()), this, SLOT(OnApplySpline()));
	connect(m_pctrlTangentSpline, SIGNAL(clicked()), this, SLOT(OnTangentSpline()));
	connect(m_pctrlResetQSpec,    SIGNAL(clicked()), this, SLOT(OnQReset()));
	connect(m_pctrlSmooth,        SIGNAL(clicked()), this, SLOT(OnSmooth()));
	connect(m_pctrlPert,          SIGNAL(clicked()), this, SLOT(OnPertubate()));
	connect(m_pctrlFilter,        SIGNAL(clicked()), this, SLOT(OnFilter()));
	connect(m_pctrlSymm,          SIGNAL(clicked()), this, SLOT(OnSymm()));
	connect(m_pctrlExec,          SIGNAL(clicked()), this, SLOT(OnExecute()));

	connect(m_pctrlMNewSpline,     SIGNAL(clicked()), this, SLOT(OnNewSpline()));
	connect(m_pctrlMark,           SIGNAL(clicked()), this, SLOT(OnMarkSegment()));
	connect(m_pctrlMApplySpline,   SIGNAL(clicked()), this, SLOT(OnApplySpline()));
	connect(m_pctrlMTangentSpline, SIGNAL(clicked()), this, SLOT(OnTangentSpline()));
	connect(m_pctrlMShowSpline,    SIGNAL(clicked()), this, SLOT(OnShowSpline()));
	connect(m_pctrlMResetQSpec,    SIGNAL(clicked()), this, SLOT(OnQReset()));
	connect(m_pctrlCpxx,           SIGNAL(clicked()), this, SLOT(OnCpxx()));
	connect(m_pctrlMExec,          SIGNAL(clicked()), this, SLOT(OnExecute()));
}



void QXInverse::CreateQCurve()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	double x,y;
	m_pQCurve->n = 0;

	int points;
	if(m_bFullInverse) points = 257;
	else points  = pXFoil->n;

	for (int i=1; i<=points; i++)
	{
		x = 1.0 - pXFoil->sspec[i];
		y = pXFoil->qcomp(pXFoil->qspec[1][i])/pXFoil->qinf;
		m_pQCurve->AddPoint(x,y);
	}
}


void QXInverse::CreateMCurve()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	int i, points;
	double x,y;
	m_pMCurve->n = 0;
	m_pReflectedCurve->n = 0;

	if(m_bFullInverse) points = 257;
	else               points = pXFoil->n;

	for (i=1; i<=points; i++)
	{
		x = 1.0 - pXFoil->sspec[i];
		y = pXFoil->qcomp(pXFoil->qspec[1][i])/pXFoil->qinf;
		m_pMCurve->AddPoint(x,y);
		m_pReflectedCurve->AddPoint(pXFoil->sspec[i],-y);
	}
}


void QXInverse::DrawGrid(QPainter &painter, double scale)
{
	painter.save();
	double scalex,scaley;
	int TickSize, xTextOff;

	MainFrame * pMainFrame = (MainFrame*)m_pMainFrame;

	TickSize = 5;
	scalex= scale;
	scaley= scale;
	xTextOff = 14;

	QPen TextPen(pMainFrame->m_TextColor);
	painter.setPen(TextPen);

	//neutral line first
//	QPen LinePen(pMainFrame->m_TextColor);
//	painter.setPen(LinePen);

	painter.drawLine(0, m_ptOffset.y(), m_rCltRect.right(), m_ptOffset.y());

	double xo            =  0.0;
	double xmin          =  0.0;
	double xmax          =  1.0;
//	double ymin          = -0.2;
//	double ymax          =  0.2;
	double XGridUnit     =  0.1;
	double XHalfGridUnit =  0.05;
	double XMinGridUnit  =  0.01;

	double xt  = xo-int((xo-xmin)*1.0001/XGridUnit)*XGridUnit;//one tick at the origin
	double xht = xo-int((xo-xmin)*1.0001/XHalfGridUnit)*XHalfGridUnit;//one tick at the origin
	double xmt = xo-int((xo-xmin)*1.0001/XMinGridUnit)*XMinGridUnit;//one tick at the origin


	QString strLabel;
	while(xt<=xmax*1.001)
	{
		//Draw  ticks
		painter.drawLine(int(xt*scalex) + m_ptOffset.x(), m_ptOffset.y(),
						 int(xt*scalex) + m_ptOffset.x(), m_ptOffset.y()+TickSize);
		strLabel = QString("%1").arg(xt,0,'f',1);
		painter.drawText(int(xt*scalex)+m_ptOffset.x()-5, m_ptOffset.y()+(int)(TickSize*5), strLabel);
		xt += XGridUnit ;
	}

	while(xht<=xmax*1.001)
	{
		//Draw  ticks
		painter.drawLine(int(xht*scalex) + m_ptOffset.x(), m_ptOffset.y(),
						 int(xht*scalex) + m_ptOffset.x(), m_ptOffset.y()+TickSize*2);
		xht += XHalfGridUnit ;
	}

	while(xmt<=xmax*1.001)
	{
		//Draw  ticks
		painter.drawLine(int(xmt*scalex) + m_ptOffset.x(), m_ptOffset.y(),
						 int(xmt*scalex) + m_ptOffset.x(), m_ptOffset.y()+TickSize);
		xmt += XMinGridUnit ;
	}

	painter.restore();

}


void QXInverse::ExecMDES()
{
//----- put modified info back into global arrays

	XFoil *pXFoil = (XFoil*)m_pXFoil;

	int i, isp;
	double qscom;
	for (i=1; i<= pXFoil->nsp; i++)
	{
		isp = pXFoil->nsp - i + 1;
		qscom =  pXFoil->qinf*m_pMCurve->y[i-1];
		pXFoil->qspec[1][i] = qincom(qscom, pXFoil->qinf, pXFoil->tklam);
	}
	pXFoil->ExecMDES();

	for(i=1; i<=pXFoil->nsp; i++)
	{
		m_pModFoil->x[i-1] = pXFoil->xb[i];
		m_pModFoil->y[i-1] = pXFoil->yb[i];
	}
	for(i=1; i<=pXFoil->nsp; i++)
	{
		m_pModFoil->xb[i-1] = pXFoil->xb[i];
		m_pModFoil->yb[i-1] = pXFoil->yb[i];
	}
	m_pModFoil->n  = pXFoil->nsp;
	m_pModFoil->nb = pXFoil->nsp;
	m_pModFoil->InitFoil();
	m_pModFoil->m_bSaved = false;

	m_bModFoil = true;
}


bool QXInverse::ExecQDES()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	int i;

	if(!m_bMarked)
	{
		 // || !pXFoil->liqset
		m_pctrlMOutput->setPlainText(tr("Must mark off target segment first"));
		return false;
	}

//----- put modified info back into global arrays
	int isp;
	double qscom;
	for (i=1; i<= pXFoil->nsp; i++)
	{
		isp = pXFoil->nsp - i + 1;
		qscom =  pXFoil->qinf*m_pMCurve->y[i-1];
		pXFoil->qspec[1][i] = qincom(qscom, pXFoil->qinf, pXFoil->tklam);
	}
	bool bRes =  pXFoil->ExecQDES();

	QString str;
	QString strong = "";
	strong = "   dNMax       dGMax\n";
	for(int l=1; l<=pXFoil->QMax; l++)
	{
		str = QString("%1e  %2\n").arg(pXFoil->dnTrace[l],7,'e',3).arg(pXFoil->dgTrace[l],7,'e',3);
		strong += str;
	}

	if(bRes)
	{
		strong += tr("Converged");
		m_pctrlMOutput->setPlainText(strong);
	}
	else
	{
		strong += tr("Unconverged");
		m_pctrlMOutput->setPlainText(strong);
	}

	for (i=1; i<=pXFoil->n; i++)
	{
		m_pModFoil->x[i-1] = pXFoil->x[i];
		m_pModFoil->y[i-1] = pXFoil->y[i];
	}
	for (i=1; i<=pXFoil->nb; i++)
	{
		m_pModFoil->xb[i-1] = pXFoil->x[i];
		m_pModFoil->yb[i-1] = pXFoil->y[i];
	}
	m_pModFoil->n  = pXFoil->n;
	m_pModFoil->nb = pXFoil->nb;

	m_pModFoil->InitFoil();
	m_bModFoil = true;

	return true;
}


bool QXInverse::InitXFoil(CFoil * pFoil)
{
	//loads pFoil in XFoil, calculates normal vectors, and sets results in current foil
	if(!pFoil) return  false;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	XFoil *pXFoil = (XFoil*)m_pXFoil;

	m_pModFoil->m_FoilName = pFoil->m_FoilName + tr(" Modified");
	pXFoil->m_FoilName = m_pRefFoil->m_FoilName ;

	pXFoil->Initialize();
	for(int i =0; i<pFoil->n; i++)
	{
		pXFoil->xb[i+1] = pFoil->x[i];
		pXFoil->yb[i+1] = pFoil->y[i];
	}
	pXFoil->nb     = pFoil->n;
	pXFoil->lflap  = false;
	pXFoil->lbflap = false;
	pXFoil->ddef   = 0.0;
	pXFoil->xbf    = 1.0;
	pXFoil->ybf    = 0.0;

	pXFoil->lqspec = false;
	pXFoil->lscini = false;

	if(pXFoil->Preprocess())
	{
		pXFoil->CheckAngles();

		for (int k=0; k<pXFoil->n;k++)
		{
			pFoil->nx[k] = pXFoil->nx[k+1];
			pFoil->ny[k] = pXFoil->ny[k+1];
		}
		pFoil->n = pXFoil->n;
		return true;
	}
	else
	{
		QMessageBox::warning(pMainFrame,tr("Warning"),tr("Unrecognized foil format"));
		return false;
	}
}


void QXInverse::keyPressEvent(QKeyEvent *event)
{
	bool bCtrl;
	if(event->modifiers() & Qt::ControlModifier) bCtrl  = true;
	switch (event->key())
	{
		case Qt::Key_X:
			m_bXPressed = true;
			break;
		case Qt::Key_Y:
			m_bYPressed = true;
			break;

		case Qt::Key_Escape:
		{
			if(m_bZoomPlus)
			{
				ReleaseZoom();
			}
			else if(m_bGetPos)
			{
				m_bGetPos = false;
				m_bSpline = false;
				m_bSmooth = false;
				if(m_bFullInverse)
				{
					CancelSpline();
					CancelSmooth();
				}
				else
				{
					CancelSpline();
					CancelSmooth();
				}
				UpdateView();
			}
			break;
		}
		case Qt::Key_Return:
		{
			if (m_bSmooth)
			{
				Smooth(-1);
				m_bGetPos = false;
			}
			else
			{
				if(m_bFullInverse)
				{
					   m_pctrlExec->setFocus();
				}
				else
				{
					   m_pctrlExec->setFocus();
				}
				return;
			}
			break;
		}

		case Qt::Key_Z:
		{
			return;//User is zooming with 'Z' key instead of mouse midle button
		}
		case Qt::Key_G:
		{
			OnGraphSettings();
			return;
		}
		case Qt::Key_R:
		{
			if(m_pCurGraph)
			{
				m_QGraph.SetAuto(true);
				UpdateView();
			}
			else OnResetFoilScale();
			break;
		}
		default:
			QWidget::keyPressEvent(event);
	}
}


void QXInverse::keyReleaseEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Escape:
		{
			if(m_bZoomPlus) ReleaseZoom();
			if(m_bZoomXOnly)
			{
				m_bZoomXOnly = false;
				MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
				pMainFrame->m_pctrlInvZoomX->setChecked(false);
			}
			if(m_bZoomYOnly)
			{
				m_bZoomYOnly = false;
				MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
				pMainFrame->m_pctrlInvZoomY->setChecked(false);
			}
			break;
		}
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



void QXInverse::LoadSettings(QSettings *pSettings)
{
	pSettings->beginGroup("XInverse");
	{
		m_bFullInverse = pSettings->value("FullInverse").toBool();

		m_Spline.m_Color = pSettings->value("SplineColor").value<QColor>();
		m_Spline.m_Style = pSettings->value("SplineStyle").toInt();
		m_Spline.m_Width = pSettings->value("SplineWdth").toInt();
		m_pRefFoil->m_FoilColor  = pSettings->value("BaseFoilColor").value<QColor>();
		m_pRefFoil->m_nFoilStyle = pSettings->value("BaseFoilStyle").toInt();
		m_pRefFoil->m_nFoilWidth = pSettings->value("BaseFoilWidth").toInt();
		m_pModFoil->m_FoilColor  = pSettings->value("ModFoilColor").value<QColor>();
		m_pModFoil->m_nFoilStyle = pSettings->value("ModFoilStyle").toInt();
		m_pModFoil->m_nFoilWidth = pSettings->value("ModFoilWidth").toInt();
	}
	pSettings->endGroup();
	m_QGraph.LoadSettings(pSettings);
}



void QXInverse::mouseDoubleClickEvent ( QMouseEvent * event )
{
	if (!m_QGraph.IsInDrawRect(event->pos())) return;

	OnGraphSettings();
}


void QXInverse::mouseMoveEvent(QMouseEvent *event)
{
	if(!hasFocus()) setFocus();//to catch keyboard input;
	static double x1,y1, xmin, xmax, ymin,  ymax, xpt, ypt, scale, ux, uy, unorm, vx, vy, vnorm, scal;
	static double xx0,xx1,xx2,yy0,yy1,yy2, dist;
	static int a, n, ipt;
	static QPoint point;
	point = event->pos();

	if(m_bGetPos)
	{
//		m_tmpPos = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x()));
		m_tmpPos = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x()), m_QGraph.ClientToy(point.y()), dist);
		UpdateView();
	}
	else if(m_bZoomPlus && (event->buttons() & Qt::LeftButton))
	{
		m_ZoomRect.setRight(point.x());
		m_ZoomRect.setBottom(point.y());
		UpdateView();
	}
	else if(m_rCltRect.contains(point) && (event->buttons() & Qt::LeftButton) && m_bTrans)
	{
		if(m_bTransGraph)
		{
			// we're dragging the graph
			x1 =  m_QGraph.ClientTox(m_PointDown.x()) ;
			y1 =  m_QGraph.ClientToy(m_PointDown.y()) ;
			
			xu = m_QGraph.ClientTox(point.x());
			yu = m_QGraph.ClientToy(point.y());

			xmin = m_QGraph.GetXMin() - xu+x1;
			xmax = m_QGraph.GetXMax() - xu+x1;
			ymin = m_QGraph.GetYMin() - yu+y1;
			ymax = m_QGraph.GetYMax() - yu+y1;

			m_QGraph.SetWindow(xmin, xmax, ymin, ymax);
		}
		else 
		{
			//we're dragging the foil
			m_ptOffset.rx() += point.x() - m_PointDown.x();
			m_ptOffset.ry() += point.y() - m_PointDown.y();
		}
		UpdateView();
		m_PointDown = point;
	}
	else if ((event->buttons() & Qt::LeftButton)  && !m_bZoomPlus && m_bSpline && m_Spline.m_iSelect>=0) 
	{
		// user is dragging the point
		x1 =  m_QGraph.ClientTox(point.x()) ;
		y1 =  m_QGraph.ClientToy(point.y()) ;
		if(m_rGraphRect.contains(point))
		{
			n = m_Spline.m_iSelect;

			if(n==0)
			{
				// user is dragging end point
				// find closest graph point
//				ipt = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x()));
				ipt = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x()), m_QGraph.ClientToy(point.y()), dist);
				m_SplineLeftPos = ipt;
				xpt = m_pMCurve->x[ipt];
				ypt = m_pMCurve->y[ipt];
				// check for inversion
				if(xpt> m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x)
				{
					m_Spline.m_Input[n].x = m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x;
					m_Spline.m_Input[n].y = m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].y;
					m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x = xpt;
					m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].y = ypt;
				}
				else
				{
					m_Spline.m_Input[n].x = xpt;
					m_Spline.m_Input[n].y = ypt;
				}
				m_bSplined = false;
				m_Spline.SplineCurve();
			}
			else if(n == m_Spline.m_iCtrlPoints-1)
			{
				// user is dragging end point
				// find closest graph point
//				ipt = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x()));
				ipt = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x()), m_QGraph.ClientToy(point.y()), dist);
				m_SplineRightPos = ipt;
				xpt = m_pMCurve->x[ipt];
				ypt = m_pMCurve->y[ipt];
				// check for inversion
				if(xpt< m_Spline.m_Input[0].x)
				{
					m_Spline.m_Input[n].x = m_Spline.m_Input[0].x;
					m_Spline.m_Input[n].y = m_Spline.m_Input[0].y;
					m_Spline.m_Input[0].x = xpt;
					m_Spline.m_Input[0].y = ypt;
				}
				else
				{
					m_Spline.m_Input[n].x = xpt;
					m_Spline.m_Input[n].y = ypt;
				}
				m_Spline.SplineCurve();
				m_bSplined = false;
			}
			else if (n==1 && m_bTangentSpline)
			{
				// Second point must remain on tangent to curve
				// difficulty is that we are working in non-normal coordinates

				tanpt = point;
				P0 = QPoint(m_QGraph.xToClient(m_pMCurve->x[m_SplineLeftPos-1]), m_QGraph.yToClient(m_pMCurve->y[m_SplineLeftPos-1]));
				P1 = QPoint(m_QGraph.xToClient(m_pMCurve->x[m_SplineLeftPos]),   m_QGraph.yToClient(m_pMCurve->y[m_SplineLeftPos]));
				P2 = QPoint(m_QGraph.xToClient(m_pMCurve->x[m_SplineLeftPos+1]), m_QGraph.yToClient(m_pMCurve->y[m_SplineLeftPos+1]));

				//v is the tangent to the curve in screen coordinates
				vx = (double)((P0.x()-P1.x())*(P0.x()-P2.x())*(P1.x()-P2.x())*(P2.x()-P0.x()));
				vy = (double)( P0.y() *(P1.x()-P2.x())            * (P1.x()-P2.x()) * (P2.x()-P0.x())         
				             - P1.y() *(2.0*P1.x()-P0.x()-P2.x()) * (P0.x()-P2.x()) * (P2.x()-P0.x())
				             - P2.y() *(P1.x()-P0.x())            * (P0.x()-P1.x()) * (P0.x()-P2.x()));
				vnorm = sqrt(vx*vx+vy*vy);
				vx/=vnorm;
				vy/=vnorm;
				scal = (double)(point.x()-P1.x())*vx + (double)(point.y()-P1.y())*vy;
				tanpt.rx() = P1.x() + (int)(vx * scal);
				tanpt.ry() = P1.y() + (int)(vy * scal);

				x1 =  m_QGraph.ClientTox(tanpt.x()) ;
				y1 =  m_QGraph.ClientToy(tanpt.y()) ;

				xx0 = m_pMCurve->x[m_SplineLeftPos-1];
				xx1 = m_pMCurve->x[m_SplineLeftPos];
				xx2 = m_pMCurve->x[m_SplineLeftPos+1];
				yy0 = m_pMCurve->y[m_SplineLeftPos-1];
				yy1 = m_pMCurve->y[m_SplineLeftPos];
				yy2 = m_pMCurve->y[m_SplineLeftPos+1];

				ux = (xx0-xx1)*(xx0-xx2)*(xx1-xx2)*(xx2-xx0);
				uy =	  yy0 *(xx1-xx2)         * (xx1-xx2) * (xx2-xx0)         
				        - yy1 *(2.0*xx1-xx0-xx2) * (xx0-xx2) * (xx2-xx0)
				        - yy2 *(xx1-xx0)         * (xx0-xx1) * (xx0-xx2);
			
//				unorm = sqrt(ux*ux*scx*scx+uy*uy*scy*scy)/scx/scy;
				unorm = sqrt(ux*ux+uy*uy);
				ux /= unorm;
				uy /= unorm;

				vx = x1-m_Spline.m_Input[n-1].x;
				vy = y1-m_Spline.m_Input[n-1].y;

				scal =  (ux*vx + uy*vy);
				m_Spline.m_Input[n].x = m_Spline.m_Input[0].x + scal * ux ;
				m_Spline.m_Input[n].y = m_Spline.m_Input[0].y + scal * uy ;
				m_Spline.SplineCurve();
				m_bSplined = false;
			}
			else if (n==m_Spline.m_iCtrlPoints-2 && m_bTangentSpline)
			{
				//penultimate point must remain on tangent to curve
				// difficulty is that we are working in non-normal coordinates
				tanpt = QPoint(point.x(), point.y());
				P0 = QPoint(m_QGraph.xToClient(m_pMCurve->x[m_SplineRightPos-1]), m_QGraph.yToClient(m_pMCurve->y[m_SplineRightPos-1]));
				P1 = QPoint(m_QGraph.xToClient(m_pMCurve->x[m_SplineRightPos]),   m_QGraph.yToClient(m_pMCurve->y[m_SplineRightPos]));
				P2 = QPoint(m_QGraph.xToClient(m_pMCurve->x[m_SplineRightPos+1]), m_QGraph.yToClient(m_pMCurve->y[m_SplineRightPos+1]));
				//v is the tangent to the curve in screen coordinates
				vx = (double)((P0.x()-P1.x())*(P0.x()-P2.x())*(P1.x()-P2.x())*(P2.x()-P0.x()));

				vy = (double)( P0.y() *(P1.x()-P2.x())            * (P1.x()-P2.x()) * (P2.x()-P0.x())
							 - P1.y() *(2.0*P1.x()-P0.x()-P2.x()) * (P0.x()-P2.x()) * (P2.x()-P0.x())
							 - P2.y() *(P1.x()-P0.x())            * (P0.x()-P1.x()) * (P0.x()-P2.x()));
				vnorm = sqrt(vx*vx+vy*vy);
				vx/=vnorm;
				vy/=vnorm;
				scal = (double)(point.x()-P1.x())*vx + (double)(point.y()-P1.y())*vy;
				tanpt.rx() = P1.x() + (int)(vx * scal);
				tanpt.ry() = P1.y() + (int)(vy * scal);

				x1 =  m_QGraph.ClientTox(tanpt.x()) ;
				y1 =  m_QGraph.ClientToy(tanpt.y()) ;

				xx0 = m_pMCurve->x[m_SplineRightPos-1];
				xx1 = m_pMCurve->x[m_SplineRightPos];
				xx2 = m_pMCurve->x[m_SplineRightPos+1];
				yy0 = m_pMCurve->y[m_SplineRightPos-1];
				yy1 = m_pMCurve->y[m_SplineRightPos];
				yy2 = m_pMCurve->y[m_SplineRightPos+1];

				ux = (xx0-xx1)*(xx0-xx2)*(xx1-xx2)*(xx2-xx0);
				uy =	  yy0 *(xx1-xx2)         * (xx1-xx2) * (xx2-xx0)         
				        - yy1 *(2.0*xx1-xx0-xx2) * (xx0-xx2) * (xx2-xx0)
				        - yy2 *(xx1-xx0)         * (xx0-xx1) * (xx0-xx2);

				unorm = sqrt(ux*ux+uy*uy);
				ux /= unorm;
				uy /= unorm;

				vx = x1-m_Spline.m_Input[n+1].x;
				vy = y1-m_Spline.m_Input[n+1].y;

				scal =  (ux*vx + uy*vy);
				m_Spline.m_Input[n].x = m_Spline.m_Input[n+1].x + scal * ux;
				m_Spline.m_Input[n].y = m_Spline.m_Input[n+1].y + scal * uy;
				m_Spline.SplineCurve();
				m_bSplined = false;
			}	
			else if (n>0 && n<m_Spline.m_iCtrlPoints-1)
			{
				m_Spline.m_Input[n].x = x1;
				m_Spline.m_Input[n].y = y1;
				m_Spline.SplineCurve();
				m_bSplined = false;
			}
			UpdateView();
		}
	}
	else if((event->buttons() & Qt::MidButton) /*||  (shZ & 0x8000)*/)
	{
		ReleaseZoom();
		QPoint pttmp(point.x(), point.y());
		if(m_QGraph.IsInDrawRect(pttmp))
		{
			//zoom graph
			m_QGraph.SetAuto(false);
			if(point.y()-m_PointDown.y()<0) m_QGraph.Scale(1.02);
			else                            m_QGraph.Scale(1.0/1.02);
		}
		else
		{
			scale = m_fScale;

			if(point.y()-m_PointDown.y()>0) m_fScale *= 1.02;
			else                            m_fScale /= 1.02;

			a = (int)((m_rCltRect.right()+m_rCltRect.left())/2);
			m_ptOffset.rx() = a + (int)((m_ptOffset.x()-a)*m_fScale/scale);
		}
		UpdateView();
		m_PointDown = point;
	}
	else
	{
		// highlight if mouse passe over a point
		if(m_bSpline)
		{
			x1 =  m_QGraph.ClientTox(point.x());
			y1 =  m_QGraph.ClientToy(point.y());
			n = m_Spline.IsControlPoint(x1,y1, m_QGraph.GetXScale(), m_QGraph.GetYScale());
			if (n>=0 && n<=m_Spline.m_iCtrlPoints)
			{
				m_Spline.m_iHighlight = n;
			}
			else m_Spline.m_iHighlight = -1;
			UpdateView();
		}
	}
	if(m_QGraph.IsInDrawRect(point))
	{
		MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
		pMainFrame->statusBar()->showMessage(QString("X = %1, Y = %2").arg(m_QGraph.ClientTox(event->x())).arg(m_QGraph.ClientToy(event->y())));
		m_pCurGraph = &m_QGraph;
	}
	else m_pCurGraph = NULL;
}




void QXInverse::mousePressEvent(QMouseEvent *event)
{
	TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;
	bool bCtrl, bShift;
	bCtrl = bShift = false;
	if(event->modifiers() & Qt::ControlModifier) bCtrl  = true;
	if(event->modifiers() & Qt::ShiftModifier)   bShift = true;

	int CtrlPt;
	QPoint pttmp;
	QPoint point = event->pos();
	if((event->buttons() & Qt::LeftButton))
	{
		if(!m_bGetPos)
		{
			m_PointDown.rx() = point.x();
			m_PointDown.ry() = point.y();
			pttmp = QPoint(point.x(), point.y());

			if(m_QGraph.IsInDrawRect(pttmp))
			{
				m_bTransGraph = true;
				p2DWidget->setCursor(Qt::ClosedHandCursor);
				xd = m_QGraph.ClientTox(point.x());
				yd = m_QGraph.ClientToy(point.y());
				if(m_bSpline)
				{
					CtrlPt = m_Spline.IsControlPoint(xd, yd, m_QGraph.GetXScale(), m_QGraph.GetYScale());
					if(CtrlPt<0) m_Spline.m_iSelect = -1;
					else 
					{
						m_Spline.m_iSelect = CtrlPt;
//						return;
					}
					if (bCtrl)
					{
						if(CtrlPt>=0) 
						{
							if (m_Spline.m_iSelect>=0) 
							{
								m_Spline.RemovePoint(m_Spline.m_iSelect);
								m_Spline.SplineKnots();
								m_Spline.SplineCurve();
							}
						}
					}
					else if (bShift) 
					{
						m_Spline.InsertPoint(xd,yd);
						m_Spline.SplineKnots();
						m_Spline.SplineCurve();
					}
					if(CtrlPt>=0) return;
				}
			}
			else m_bTransGraph = false;

			if(m_bZoomPlus && m_QGraph.IsInDrawRect(point))
			{
				m_ZoomRect.setLeft(point.x());
				m_ZoomRect.setTop(point.y());
				m_ZoomRect.setRight(point.x());
				m_ZoomRect.setBottom(point.y());
				return;
			}
			else if(m_bZoomPlus && !m_QGraph.IsInDrawRect(point))
			{
				ReleaseZoom();
			}
			else
			{
				p2DWidget->setCursor(Qt::ClosedHandCursor);
				m_bTrans = true;
				m_bZoomPlus = false;
			}
		}	
	}
	else if((event->buttons() & Qt::RightButton))
	{
		m_ptPopUp = event->pos();
	}
}


void QXInverse::mouseReleaseEvent(QMouseEvent *event)
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;
	m_bTrans = false;

	static int tmp, width, height;
	static double x1,x2,w,h,xw,yh,xm,ym, dist;
	static double xmin, ymin, xmax, ymax;
	static double ratio,x, y, ux, uy, xpt, ypt, norm;

	QPoint point = event->pos();

	if(m_bZoomPlus && m_rCltRect.contains(point))
	{
		QRect ZRect = m_ZoomRect.normalized();
		if (!ZRect.isEmpty() )
		{
			xu = m_QGraph.ClientTox(point.x());
			yu = m_QGraph.ClientToy(point.y());

			width  = abs(m_PointDown.x()-point.x());
			height = abs(m_PointDown.y()-point.y());
			//preserve ratio
			w = fabs(xu-xd);
			h = fabs(yu-yd);
			xw = 	m_QGraph.GetXMax() - m_QGraph.GetXMin();
			yh = 	m_QGraph.GetYMax() - m_QGraph.GetYMin();
			xm = (xu+xd)/2.0;
			ym = (yu+yd)/2.0;

			if(width>=height)
			{
				xmin  = xm - w/2.0;
				xmax  = xm + w/2.0;
				ratio = w/xw;

				ymin  = ym - ratio*yh/2.0;
				ymax  = ym + ratio*yh/2.0;
			}
			else 
			{
				ymin  = ym - h/2.0;
				ymax  = ym + h/2.0;
				ratio = h/yh;
				xmin  = xm - ratio * xw/2.0;
				xmax  = xm + ratio * xw/2.0;
			}
			if (m_QGraph.IsInDrawRect(ZRect.left(), ZRect.top()) &&
				m_QGraph.IsInDrawRect(ZRect.right(), ZRect.bottom()))
			{
				m_QGraph.SetWindow(xmin, xmax, ymin, ymax);
			}
			m_ZoomRect.setRight(m_ZoomRect.left()-1);
			m_ZoomRect.setTop(m_ZoomRect.bottom()+1);
		}
		else 
		{
			ReleaseZoom();
		}
	}
	else if(m_bZoomPlus && !m_rCltRect.contains(point))
	{
			ReleaseZoom();
	}
	else if(m_bGetPos && m_rCltRect.contains(point))
	{
		if(m_nPos == 0)
		{
//			m_Pos1 = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x()));
			m_Pos1 = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x()), m_QGraph.ClientToy(point.y()), dist);
		}
		if(m_nPos == 1)
		{
//			m_Pos2 = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x()));
			m_Pos2 = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x()), m_QGraph.ClientToy(point.y()), dist);
		}
		m_nPos++;
		if(m_nPos == 2) 
		{
			if(m_bSmooth) 
			{
				m_pctrlOutput->setPlainText(" ");
				Smooth(m_Pos1, m_Pos2);
			}
			else if(m_bSpline) 
			{
				x1 = m_pMCurve->x[m_Pos1];
				x2 = m_pMCurve->x[m_Pos2];
				if(fabs(x2-x1)<0.00001) return;
				if(x2<x1)
				{
					tmp    = m_Pos2;
					m_Pos2 = m_Pos1;
					m_Pos1 = tmp;
				}

				m_SplineLeftPos  = m_Pos1;
				m_SplineRightPos = m_Pos2;

				m_Spline.m_iCtrlPoints = 0;
				m_Spline.InsertPoint(m_pMCurve->x[m_Pos1], m_pMCurve->y[m_Pos1]);
				m_Spline.InsertPoint(m_pMCurve->x[m_Pos2], m_pMCurve->y[m_Pos2]);

				x = (3.0*m_pMCurve->x[m_Pos1] + m_pMCurve->x[m_Pos2])/4.0;
				y = (3.0*m_pMCurve->y[m_Pos1] + m_pMCurve->y[m_Pos2])/4.0;
				m_Spline.InsertPoint(x,y);

				x = (m_pMCurve->x[m_Pos1] + m_pMCurve->x[m_Pos2])/2.0;
				y = (m_pMCurve->y[m_Pos1] + m_pMCurve->y[m_Pos2])/2.0;
				m_Spline.InsertPoint(x,y);

				x = (m_pMCurve->x[m_Pos1] + 3.0*m_pMCurve->x[m_Pos2])/4.0;
				y = (m_pMCurve->y[m_Pos1] + 3.0*m_pMCurve->y[m_Pos2])/4.0;
				m_Spline.InsertPoint(x,y);


				if (m_bTangentSpline)
				{
					//Second point must remain on tangent to curve
					ux = m_pMCurve->x[m_Pos1+1] - m_pMCurve->x[m_Pos1];
					uy = m_pMCurve->y[m_Pos1+1] - m_pMCurve->y[m_Pos1];
					norm = sqrt(ux*ux+uy*uy);
					ux /= norm;
					uy /= norm;
					xpt = m_Spline.m_Input[1].x - m_Spline.m_Input[0].x;
					ypt = m_Spline.m_Input[1].y - m_Spline.m_Input[0].y;
				
					m_Spline.m_Input[1].x = m_Spline.m_Input[0].x + (ux*xpt + uy*ypt) * ux;
					m_Spline.m_Input[1].y = m_Spline.m_Input[0].y + (ux*xpt + uy*ypt) * uy;
				

					//penultimate point must remain on tangent to curve
					ux = m_pMCurve->x[m_Pos2] - m_pMCurve->x[m_Pos2-1];
					uy = m_pMCurve->y[m_Pos2] - m_pMCurve->y[m_Pos2-1];
					norm = sqrt(ux*ux+uy*uy);
					ux /= norm;
					uy /= norm;

					xpt = m_Spline.m_Input[m_Spline.m_iCtrlPoints-2].x - m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x;
					ypt = m_Spline.m_Input[m_Spline.m_iCtrlPoints-2].y - m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].y;
					
					m_Spline.m_Input[m_Spline.m_iCtrlPoints-2].x = m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x + (ux*xpt + uy*ypt) * ux;
					m_Spline.m_Input[m_Spline.m_iCtrlPoints-2].y = m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].y + (ux*xpt + uy*ypt) * uy;
				}	

				m_Spline.SplineKnots();
				m_Spline.SplineCurve();
				if(m_bFullInverse)
				{
					m_pctrlNewSpline->setChecked(0);
					m_pctrlOutput->setPlainText(
						tr("Drag points to modify splines, Apply, and Execute to generate the new geometry"));
				}
				else
				{
					m_pctrlMNewSpline->setChecked(0);
					m_pctrlMOutput->setPlainText(
						tr("Drag points to modify splines, Apply, and Execute to generate the new geometry"));
				}
			}
			else if(m_bMark)
			{
				m_pctrlOutput->setPlainText(" ");
				if (m_Pos1 == m_Pos2) return;

				m_Mk1 = m_Pos1;
				m_Mk2 = m_Pos2;
				pXFoil->iq1 = min(m_Pos1, m_Pos2)+1;
				pXFoil->iq2 = max(m_Pos1, m_Pos2)+1;
				if(pXFoil->iq1<=1) pXFoil->iq1 = 2;
				if(pXFoil->iq2>=pXFoil->n) pXFoil->iq2 = pXFoil->n-1;
				pXFoil->liqset = true;

				m_bMarked = true;
				m_bMark   = false;
				m_pctrlMark->setChecked(false);
			}
			m_bGetPos = false;

		}
	}

	if(m_QGraph.IsInDrawRect(point))
	{
		p2DWidget->setCursor(Qt::CrossCursor);
	}
	else 
	{
		p2DWidget->setCursor(Qt::ArrowCursor);
	}
	UpdateView();
}


void QXInverse::OnApplySpline()
{
	if(!m_bSplined)
	{
		XFoil *pXFoil = (XFoil*)m_pXFoil;
		int i, isp;
		double qscom, xx;
		for (i=1; i<m_pMCurve->n-1; i++)
		{
			xx = m_pMCurve->x[i];
			if (xx > m_Spline.m_Input[0].x &&
				xx < m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x )
			{
				//interpolate spline at xx
				m_pMCurve->y[i] = m_Spline.GetY(xx);
			}
		}

		for (i=1; i<m_pMCurve->n-1; i++)
		{
			m_pReflectedCurve->y[i] = -m_pMCurve->y[i];
		}

		m_bSplined = true;
		for (i=1; i<= pXFoil->nsp; i++)
		{
			isp = pXFoil->nsp - i + 1;
			qscom =  pXFoil->qinf*m_pMCurve->y[i-1];
			pXFoil->qspec[1][i] = qincom(qscom,pXFoil->qinf,pXFoil->tklam);
		}

		pXFoil->lqspec = false;

		UpdateView();
	}
	if(m_bZoomPlus) ReleaseZoom();
//	m_bSpline = false;
	m_nPos    = 0;
	m_tmpPos  = -1;
	m_Pos1    = -1;
	m_Pos2    = -1;

	UpdateView();
//	m_ctrlShowSpline.Invalidate();
}


void QXInverse::OnCpxx()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	CancelSpline();
	CancelSmooth();
	CancelMark();
	if (m_bZoomPlus) ReleaseZoom();
	pXFoil->lcpxx = m_pctrlCpxx->isChecked();
}



void QXInverse::OnExecute()
{
	if (m_bZoomPlus) ReleaseZoom();
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	CancelSpline();
	CancelSmooth();
	CancelMark();

	if(m_bFullInverse)
	{
		SetTAngle(m_pctrlTAngle->GetValue());
		SetTGap(m_pctrlTGapx->GetValue(),m_pctrlTGapy->GetValue());
		m_pctrlOutput->setPlainText(" ");
		ExecMDES();
	}
	else
	{
		pXFoil->niterq = m_pctrlIter->GetValue();
		m_pctrlMOutput->setPlainText(" ");
		ExecQDES();
	}
	UpdateView();
}

void QXInverse::OnExtractFoil()
{
	//Extracts a foil from the database for display and modification

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	FoilSelectionDlg dlg;
	dlg.move(pMainFrame->m_DlgPos);
	dlg.m_poaFoil = m_poaFoil;
	dlg.InitDialog();

	if(m_bLoaded)
	{
		dlg.m_FoilName = m_pRefFoil->m_FoilName;
	}

	if(QDialog::Accepted == dlg.exec())
	{
		m_bMark = false;
		m_bMarked = false;
		m_bSpline = false;
		m_bSplined  = true;
		CFoil *pFoil;
		pFoil = pMainFrame->GetFoil(dlg.m_FoilName);
		pMainFrame->SetCurrentFoil(pFoil);
		m_pRefFoil->CopyFoil(pFoil);

		m_pModFoil->m_FoilName = m_pRefFoil->m_FoilName + tr(" Modified");
		InitXFoil(m_pRefFoil);
		SetFoil();
		UpdateView();
	}
	pMainFrame->m_DlgPos = dlg.pos();
}

void QXInverse::OnFilter()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	CancelSpline();
	if (m_bZoomPlus) ReleaseZoom();

	double filt = m_pctrlFilterParam->GetValue();
	pXFoil->Filter(filt);
	CreateMCurve();
	UpdateView();
}




void QXInverse::OnGraphSettings()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	GraphDlg dlg;
	dlg.move(pMainFrame->m_DlgPos);

	dlg.m_iGraphType = 31;
	dlg.m_XSel = 0;
	dlg.m_YSel = 0;
	dlg.m_pGraph = &m_QGraph;


	QGraph graph;
	graph.CopySettings(&m_QGraph);
	dlg.m_pMemGraph = &m_QGraph;
	dlg.m_pGraph = &m_QGraph;
	dlg.SetParams();

	if(dlg.exec() == QDialog::Accepted)
	{
	}
	else
	{
		m_QGraph.CopySettings(&graph);
	}
	pMainFrame->m_DlgPos = dlg.pos();
	UpdateView();
}



void QXInverse::OnInsertCtrlPt()
{
	double xd = m_QGraph.ClientTox(m_ptPopUp.x());
	double yd = m_QGraph.ClientToy(m_ptPopUp.y());

	if(xd < m_Spline.m_Input[0].x) return;
	if(xd > m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x) return;

	m_Spline.InsertPoint(xd,yd);
	m_Spline.SplineKnots();
	m_Spline.SplineCurve();
	UpdateView();
}



void QXInverse::OnInverseApp()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	m_bFullInverse = pMainFrame->m_pctrlFullInverse->isChecked();

	if(m_bFullInverse)
	{
		m_pctrlStackedInv->setCurrentIndex(0);
	}
	else
	{
		m_pctrlStackedInv->setCurrentIndex(1);
	}
	SetFoil();
	UpdateView();
}


void QXInverse::OnInverseStyles()
{
	InverseOptionsDlg dlg;
	dlg.m_pXInverse = this;
	dlg.InitDialog();
	dlg.exec();
}


void QXInverse::OnMarkSegment()
{
	CancelSpline();
	CancelSmooth();

	if (m_bZoomPlus) ReleaseZoom();

	if(m_pctrlMark->isChecked()) m_pctrlMOutput->setPlainText(tr("Mark target segment for modification"));

	m_tmpPos  = -1;
	m_bMark   = true;
	m_bMarked = false;
	m_bSpline = false;
	m_bGetPos = true;
	m_nPos    = 0;

	m_pctrlMNewSpline->setChecked(false);
	m_pctrlMShowSpline->setChecked(false);

	UpdateView();
}


void QXInverse::OnNewSpline()
{
	ReleaseZoom();
	if((m_bFullInverse && m_pctrlNewSpline->isChecked()) || (!m_bFullInverse && m_pctrlMNewSpline->isChecked()))
	{
		CancelSmooth();
		CancelMark();
		m_pctrlOutput->setPlainText(tr("Mark spline endpoints"));
		m_bSpline = true;
		m_bSplined = false;
		if(m_bFullInverse) m_pctrlShowSpline->setChecked(true);
		else               m_pctrlMShowSpline->setChecked(true);
		m_bSmooth = false;
		m_bGetPos = true;
		m_nPos    = 0;
		m_tmpPos  = -1;
		m_Pos1    = -1;
		m_Pos2    = -1;
	}
	else
	{
		CancelSpline();
	}
	UpdateView();
}



void QXInverse::OnPertubate()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	int m;
	pXFoil->pert_init(1);

	PertDlg dlg;
	for (m=0; m<=qMin(32, pXFoil->nc); m++)
	{
		dlg.m_cnr[m] = (double)real(pXFoil->cn[m]);
		dlg.m_cni[m] = (double)imag(pXFoil->cn[m]);
	}
	dlg.m_nc = qMin(32, pXFoil->nc);
	dlg.InitDialog();

	if(dlg.exec() == QDialog::Accepted)
	{
		for (m=0; m<=qMin(32, pXFoil->nc); m++)
		{
			pXFoil->cn[m] = complex<double>(dlg.m_cnr[m],dlg.m_cni[m]);
		}

		pXFoil->pert_process(1);
		CreateMCurve();
		m_pMCurve->SetVisible(true);
		UpdateView();
	}
}



void QXInverse::OnQInitial()
{
	m_pQCurve->SetVisible(!m_pQCurve->IsVisible());
	CheckActions();
	UpdateView();
}


void QXInverse::OnQSpec()
{
	m_pMCurve->SetVisible(!m_pMCurve->IsVisible());
	CheckActions();
	UpdateView();
}


void QXInverse::OnQViscous()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	if(pXFoil->lvisc)
	{
		m_pQVCurve->SetVisible(!m_pQVCurve->IsVisible());
		UpdateView();
	}
	CheckActions();
}


void QXInverse::OnQPoints()
{
	m_bShowPoints = !m_bShowPoints;
	m_pQCurve->ShowPoints(m_bShowPoints);
	m_pMCurve->ShowPoints(m_bShowPoints);
	CheckActions();
	UpdateView();
}


void QXInverse::OnQReflected()
{
	m_bReflected = !m_bReflected;
	m_pReflectedCurve->SetVisible(m_bReflected);
	CheckActions();
	UpdateView();
}



void QXInverse::OnQReset()
{
	CancelSpline();
	CancelSmooth();
	CancelMark();
	ReleaseZoom();
	if(m_bFullInverse) ResetQ();
	else               ResetMixedQ();
	UpdateView();
}


void QXInverse::OnRemoveCtrlPt()
{
	if (m_Spline.m_iHighlight>=0) m_Spline.RemovePoint(m_Spline.m_iHighlight);
	m_Spline.SplineCurve();
	UpdateView();
}


void QXInverse::OnResetFoilScale()
{
	ReleaseZoom();
	ResetScale();
	UpdateView();
}



void QXInverse::OnSpecal()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;

	if(m_pctrlSpecAlpha->isChecked())
	{
		m_pctrlSpecif->setText(tr("Alpha = "));
		m_pctrlSpec->SetPrecision(2);
		m_pctrlSpec->SetValue(pXFoil->alqsp[1]*180.0/PI);
	}
	else
	{
		m_pctrlSpecif->setText(tr("Cl = "));
		m_pctrlSpec->SetPrecision(3);
		m_pctrlSpec->SetValue(pXFoil->clqsp[1]);
	}
}


void QXInverse::OnSpecInv()
{
	if (m_bZoomPlus) ReleaseZoom();
	XFoil *pXFoil = (XFoil*)m_pXFoil;

	if(m_pctrlSpecAlpha->isChecked())
	{
		pXFoil->alqsp[1] = m_pctrlSpec->GetValue()*PI/180.0;
		pXFoil->iacqsp = 1;
		pXFoil->qspcir();
	}
	else if(m_pctrlSpecCl->isChecked())
	{
		pXFoil->clqsp[1] = m_pctrlSpec->GetValue();
		pXFoil->iacqsp = 2;
		pXFoil->qspcir();
	}
	CreateQCurve();
	CreateMCurve();
	UpdateView();
}


void QXInverse::OnShowSpline()
{
	if(m_bFullInverse) m_bSpline = m_pctrlShowSpline->isChecked();
	else               m_bSpline = m_pctrlMShowSpline->isChecked();
	m_bSplined =   !m_bSpline;
	UpdateView();
}


void QXInverse::OnSmooth()
{
	CancelSpline();
	if(m_pctrlSmooth->isChecked())
	{
		m_pctrlOutput->setPlainText(tr("Mark target segment for smoothing, or type 'Return' to smooth the entire distribution"));

		m_bSpline = false;
		m_bSmooth = true;
		UpdateView();
		m_bGetPos = true;
		m_nPos    = 0;
	}
	else CancelSmooth();
}


void QXInverse::OnStoreFoil()
{
	if(!m_bLoaded) return;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	CFoil* pFoil = new CFoil();
	pFoil->CopyFoil(m_pModFoil);
	pFoil->m_nFoilStyle = 0;
	pFoil->m_nFoilWidth = 1;
	memcpy(pFoil->xb, m_pModFoil->x, sizeof(m_pModFoil->x));
	memcpy(pFoil->yb, m_pModFoil->y, sizeof(m_pModFoil->y));
	pFoil->nb = m_pModFoil->n;
	pFoil->m_FoilName = m_pRefFoil->m_FoilName;
	pMainFrame->SetModFoil(pFoil);
}


void QXInverse::OnSymm()
{
	CancelSpline();

	if (m_bZoomPlus) ReleaseZoom();

	XFoil *pXFoil = (XFoil*)m_pXFoil;
	pXFoil->lqsym = m_pctrlSymm->isChecked();
	pXFoil->lqspec = false;
}

void QXInverse::OnZoomIn()
{
	if(!m_bZoomPlus)
	{
		if(m_fScale/m_fRefScale <32.0)
		{
			m_bZoomPlus = true;
			MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
			pMainFrame->m_pctrlInvZoomIn->setChecked(true);
		}
		else
		{
			ReleaseZoom();
		}
	}
	else {
		ReleaseZoom();
	}
}


void QXInverse::OnZoomX()
{
	ReleaseZoom();
	m_bZoomYOnly = false;
	m_bZoomXOnly = !m_bZoomXOnly;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->m_pctrlInvZoomX->setChecked(m_bZoomXOnly);
	pMainFrame->m_pctrlInvZoomY->setChecked(m_bZoomYOnly);
}


void QXInverse::OnZoomY()
{
	ReleaseZoom();
	m_bZoomXOnly = false;
	m_bZoomYOnly = !m_bZoomYOnly;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->m_pctrlInvZoomX->setChecked(m_bZoomXOnly);
	pMainFrame->m_pctrlInvZoomY->setChecked(m_bZoomYOnly);
}


void QXInverse::OnTangentSpline()
{
	if(m_bFullInverse) m_bTangentSpline = m_pctrlTangentSpline->isChecked();
	else               m_bTangentSpline = m_pctrlMTangentSpline->isChecked();
	m_pctrlTangentSpline->setChecked(m_bTangentSpline);
	m_pctrlMTangentSpline->setChecked(m_bTangentSpline);
}


void QXInverse::PaintGraph(QPainter &painter)
{
	painter.save();

//  draw  the graph	
	if(m_rGraphRect.width()>200 && m_rGraphRect.height()>150)
	{
		m_QGraph.DrawGraph(painter);
		QPoint Place((int)(m_rGraphRect.right()-300), m_rGraphRect.top()+12);
		m_QGraph.DrawLegend(painter, Place);
	}

// draw the zoom rectangle, if relevant
	QRect ZRect = m_ZoomRect.normalized();

	if(m_bZoomPlus && !ZRect.isEmpty()) 
	{
/*		QRect ZRect(m_ZoomRect.left()   - m_rCltRect.left(), m_ZoomRect.top()    - m_rCltRect.top(),
		            m_ZoomRect.right()  - m_rCltRect.left(), m_ZoomRect.bottom() - m_rCltRect.top());
		ZRect.NormalizeRect();*/
		QPen ZoomPen(QColor(100,100,100));
		ZoomPen.setStyle(Qt::DashLine);
		painter.setPen(ZoomPen);
		painter.drawRect(ZRect);

	}

//Draw spline, if any
	if(m_bSpline && !m_bGetPos)
	{

		QPoint pt = m_QGraph.GetOffset();

		m_Spline.DrawSpline(painter, 1.0/m_QGraph.GetXScale(), -1.0/m_QGraph.GetYScale(), pt);
		m_Spline.DrawCtrlPoints(painter, 1.0/m_QGraph.GetXScale(), -1.0/m_QGraph.GetYScale(), pt);
		
	}

// Highlight selected points, if any
	if(m_bGetPos)
	{
		QPoint pt;
		//QRect r;
		m_QGraph.Highlight(painter, m_pMCurve,m_tmpPos);
		if(m_nPos>=1) m_QGraph.Highlight(painter, m_pMCurve, m_Pos1);
		if(m_nPos>=2) m_QGraph.Highlight(painter, m_pMCurve, m_Pos2);
		
	}
// Show marked segment if mixed-inverse design
	if(m_bMarked)
	{
		QPoint ptl, ptr;
		ptl.rx() = m_QGraph.xToClient(m_pMCurve->x[m_Mk1]);
		ptr.rx() = m_QGraph.xToClient(m_pMCurve->x[m_Mk2]);
		ptl.ry() = m_QGraph.yToClient(m_pMCurve->y[m_Mk1]);
		ptr.ry() = m_QGraph.yToClient(m_pMCurve->y[m_Mk2]);

		QPen MarkPen(QColor(175,30,30));
		MarkPen.setStyle(Qt::SolidLine);
		MarkPen.setWidth(2);
		painter.setPen(MarkPen);

		if(m_rGraphRect.contains(ptl))
		{
			painter.drawLine(ptl.x(), ptl.y()-20, ptl.x(), ptl.y()+20);
		}
		if(m_rGraphRect.contains(ptr))
		{
			painter.drawLine(ptr.x(), ptr.y()-20, ptr.x(), ptr.y()+20);
		}
	}

	painter.restore();
}


void QXInverse::PaintFoil(QPainter &painter)
{
	painter.save();
	QString str,str1,str2;

//	draw the scale/grid
	if(m_bModFoil || m_bRefFoil)
	{
		DrawGrid(painter, m_fScale);
	}

//draw the reference and modified foils  
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	double alpha = pXFoil->alqsp[1]*180./PI;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	QPen TextPen(pMainFrame->m_TextColor);

	if(m_bRefFoil && m_bLoaded)
	{
		QPen FoilPen(m_pRefFoil->m_FoilColor);
		FoilPen.setStyle(GetStyle(m_pRefFoil->m_nFoilStyle));
		FoilPen.setWidth(m_pRefFoil->m_nFoilWidth);
		painter.setPen(FoilPen);

		m_pRefFoil->DrawFoil(painter, -alpha, m_fScale, m_fScale, m_ptOffset);
		painter.drawLine(20, m_rGraphRect.bottom()+20, 40, m_rGraphRect.bottom()+20);
		painter.setPen(TextPen);
		painter.drawText(50, m_rGraphRect.bottom()+25, m_pRefFoil->m_FoilName);
	}

	if(m_bModFoil && m_bLoaded) 
	{
		QPen ModPen(m_pModFoil->m_FoilColor);
		ModPen.setStyle(GetStyle(m_pModFoil->m_nFoilStyle));
		ModPen.setWidth(m_pModFoil->m_nFoilWidth);
		painter.setPen(ModPen);

		m_pModFoil->DrawFoil(painter, -alpha, m_fScale, m_fScale, m_ptOffset);
		painter.drawLine(20, m_rGraphRect.bottom()+35, 40, m_rGraphRect.bottom()+35);
		painter.setPen(TextPen);
		painter.drawText(50, m_rGraphRect.bottom()+40, m_pModFoil->m_FoilName);
	}

	if (m_pRefFoil->m_bPoints)
	{
		QPen CtrlPen(m_pRefFoil->m_FoilColor);
		CtrlPen.setStyle(GetStyle(m_pRefFoil->m_nFoilStyle));
		CtrlPen.setWidth(m_pRefFoil->m_nFoilWidth);
		painter.setPen(CtrlPen);

		m_pRefFoil->DrawPoints(painter, 1.0,  1.0, m_ptOffset);
	}

	painter.setFont(pMainFrame->m_TextFont);
	QFontMetrics fm(pMainFrame->m_TextFont);
	int dD = fm.height();
	int Back = 4;
	int LeftPos = m_rCltRect.left()+10;
	int ZPos = m_rCltRect.bottom() - 10 - Back*dD;

	int D = 0;

	str = tr("                     Base");
	if(m_bModFoil && m_bLoaded)  str +=tr("       Mod.");

	painter.drawText(LeftPos,ZPos+D, str);
	D += dD;

	str1 = QString(tr("Thickness        = %1%")).arg(m_pRefFoil->m_fThickness*100.0, 6, 'f', 2);
	if(m_bModFoil && m_bLoaded)  str2 = QString("    %1%").arg(m_pModFoil->m_fThickness*100.0, 6, 'f', 2);
	else str2 = "";
	painter.drawText(LeftPos,ZPos+D, str1+str2);
	D += dD;

	str1 = QString(tr("Max.Thick.pos.   = %1%")).arg(m_pRefFoil->m_fXThickness*100.0, 6, 'f', 2);
	if(m_bModFoil && m_bLoaded)  str2 = QString("    %1%").arg(m_pModFoil->m_fXThickness*100.0, 6, 'f', 2);
	else str2 = "";
	painter.drawText(LeftPos,ZPos+D, str1+str2);
	D += dD;

	str1 = QString(tr("Max. Camber      = %1%")).arg( m_pRefFoil->m_fCamber*100.0, 6, 'f', 2);
	if(m_bModFoil && m_bLoaded)  str2 = QString("    %1%").arg(m_pModFoil->m_fCamber*100.0, 6, 'f', 2);
	else str2 = "";
	painter.drawText(LeftPos,ZPos+D, str1+str2);
	D += dD;

	str1 = QString(tr("Max.Thick.pos.   = %1%")).arg(m_pRefFoil->m_fXCamber*100.0, 6, 'f', 2);
	if(m_bModFoil && m_bLoaded)  str2 = QString("    %1%").arg(m_pModFoil->m_fXCamber*100.0, 6, 'f', 2);
	else str2 = "";
	painter.drawText(LeftPos,ZPos+D, str1+str2);
	D += dD;

	painter.restore();
}



void QXInverse::PaintView(QPainter &painter)
{
	painter.save();

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	painter.fillRect(m_rCltRect, pMainFrame->m_BackgroundColor);
	PaintGraph(painter);
	PaintFoil(painter);

	painter.restore();
}


double QXInverse::qincom(double qc, double qinf, double tklam)
{
//-------------------------------------
//     sets incompressible speed from
//     karman-tsien compressible speed
//-------------------------------------

	if(tklam<1.0e-4 || fabs(qc)<1.0e-4)
	{
//----- for nearly incompressible case or very small speed, use asymptotic
//      expansion of singular quadratic formula to avoid numerical problems
		return( qc/(1.0 - tklam));
	}
	else
	{
//----- use quadratic formula for typical case
		double tmp = 0.5*(1.0 - tklam)*qinf/(qc*tklam);
		return (qinf*tmp*((double)sqrt(1.0 + 1.0/(tklam*tmp*tmp)) - 1.0));
	}
}




void QXInverse::ReleaseZoom()
{
	m_bZoomPlus  = false;
	m_ZoomRect.setRight(m_ZoomRect.left()-1);
	m_ZoomRect.setTop(m_ZoomRect.bottom()+1);
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->m_pctrlInvZoomIn->setChecked(false);
}


void QXInverse::ResetMixedQ()
{
	for (int i=0; i<=m_pQCurve->n; i++)
	{
		m_pMCurve->x[i] = m_pQCurve->x[i] ;
		m_pMCurve->y[i] = m_pQCurve->y[i] ;
	}
	m_pMCurve->n = m_pQCurve->n;

//	m_pXFoil->gamqsp(1);
//	CreateMCurve();
}

void QXInverse::ResetQ()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	pXFoil->cncalc(pXFoil->qgamm,false);
	pXFoil->qspcir();
	CreateMCurve();
	UpdateView();
}


void QXInverse::ResetScale()
{
	int h4 = m_rCltRect.height()/4;
	m_ptOffset.rx() = m_rGraphRect.left() +(int)(1.0*m_QGraph.GetMargin());
	m_fRefScale  = m_rGraphRect.width()-2.0*m_QGraph.GetMargin();

	m_ptOffset.ry() = m_rCltRect.bottom()-h4/2;
	m_fScale = m_fRefScale;
}


void QXInverse::SaveSettings(QSettings *pSettings)
{
	pSettings->beginGroup("XInverse");
	{
		pSettings->setValue("FullInverse", m_bFullInverse);
		pSettings->setValue("SplineColor", m_Spline.m_Color);
		pSettings->setValue("SplineStyle", m_Spline.m_Style);
		pSettings->setValue("SplineWdth", m_Spline.m_Width);
		pSettings->setValue("BaseFoilColor", m_pRefFoil->m_FoilColor);
		pSettings->setValue("BaseFoilStyle", m_pRefFoil->m_nFoilStyle);
		pSettings->setValue("BaseFoilWidth", m_pRefFoil->m_nFoilWidth);
		pSettings->setValue("ModFoilColor", m_pModFoil->m_FoilColor);
		pSettings->setValue("ModFoilStyle", m_pModFoil->m_nFoilStyle);
		pSettings->setValue("ModFoilWidth", m_pModFoil->m_nFoilWidth);
	}
	pSettings->endGroup();

	m_QGraph.SaveSettings(pSettings);
}




void QXInverse::SetFoil()
{
	int i;

	XFoil *pXFoil = (XFoil*)m_pXFoil;

	QString strong;
	for(i=1; i<=pXFoil->n; i++)
	{
		m_pModFoil->x[i-1] = pXFoil->x[i];
		m_pModFoil->y[i-1] = pXFoil->y[i];
	}
	m_pModFoil->n = pXFoil->n;

	if(m_bFullInverse)
	{
		pXFoil->InitMDES();
		CreateQCurve();
		CreateMCurve();

		m_pctrlSpec->SetValue(pXFoil->alqsp[1]*180.0/PI);
		m_pctrlTAngle->SetValue(pXFoil->agte*180.0);//agte expressed in PI units:!?!?
		m_pctrlTGapx->SetValue(real(pXFoil->dzte));
		m_pctrlTGapy->SetValue(imag(pXFoil->dzte));
	}
	else
	{
		// Mixed Inverse
		pXFoil->InitQDES();
		CreateQCurve();
		CreateMCurve();
		strong = QString(tr("Alpha = %1")).arg(pXFoil->algam/pXFoil->dtor,0,'f',3);
		m_pctrlMAlphaSpec->setText(strong);
		strong = QString(tr("Cl = %1")).arg(pXFoil->clgam,0,'f',3);
		m_pctrlMClSpec->setText(strong);
		m_pctrlIter->SetValue(pXFoil->niterq);
	}

	if(pXFoil->lvisc)
	{
		//a previous xfoil calculation is still active, so add the associated viscous curve
		double x,y;
		double dsp, dqv, sp1, sp2, qv1, qv2;
		m_pQVCurve->n = 0;
		for(i=2; i<= pXFoil->n; i++)
		{
			dsp = pXFoil->s[i] - pXFoil->s[i-1];
			dqv = pXFoil->qcomp(pXFoil->qvis[i]) - pXFoil->qcomp(pXFoil->qvis[i-1]);
			sp1 = (pXFoil->s[i-1] + 0.25*dsp)/pXFoil->s[pXFoil->n];
			sp2 = (pXFoil->s[i]   - 0.25*dsp)/pXFoil->s[pXFoil->n];
			qv1 = pXFoil->qcomp(pXFoil->qvis[i-1]) + 0.25*dqv;
			qv2 = pXFoil->qcomp(pXFoil->qvis[i]  ) - 0.25*dqv;
			x = 1.0 - sp1;
			y = qv1/pXFoil->qinf;
			m_pQVCurve->AddPoint(x,y);
			x = 1.0 - sp2;
			y = qv2/pXFoil->qinf;
			m_pQVCurve->AddPoint(x,y);
		}
		m_pQVCurve->SetVisible(true);
	}
	else
	{
		m_pQVCurve->SetVisible(false);
	}

	m_bLoaded = true;
}


bool QXInverse::SetParams()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	CFoil*pFoil;

	if(m_bFullInverse)
	{
		pMainFrame->m_pctrlFullInverse->setChecked(true);
		pMainFrame->m_pctrlMixedInverse->setChecked(false);
		m_pctrlStackedInv->setCurrentIndex(0);
	}
	else
	{
		pMainFrame->m_pctrlFullInverse->setChecked(false);
		pMainFrame->m_pctrlMixedInverse->setChecked(true);
		m_pctrlStackedInv->setCurrentIndex(1);
	}

	m_pQCurve->SetColor(m_pRefFoil->m_FoilColor);
	m_pQCurve->SetStyle(m_pRefFoil->m_nFoilStyle);
	m_pQCurve->SetWidth(m_pRefFoil->m_nFoilWidth);
	m_pMCurve->SetColor(m_pModFoil->m_FoilColor);
	m_pMCurve->SetStyle(m_pModFoil->m_nFoilStyle);
	m_pMCurve->SetWidth(m_pModFoil->m_nFoilWidth);
	m_pQCurve->SetTitle(tr("Q - Reference"));
	m_pMCurve->SetTitle(tr("Q - Specification"));
	m_pQVCurve->SetTitle(tr("Q - Viscous"));
	m_pQVCurve->SetColor(QColor(50,170,0));
	m_pQVCurve->SetStyle(0);

	m_pReflectedCurve->SetColor(m_ReflectedClr);
	m_pReflectedCurve->SetStyle(m_ReflectedStyle);
	m_pReflectedCurve->SetWidth(m_ReflectedWidth);
	m_pReflectedCurve->SetTitle(tr("Reflected"));

	m_bTrans   = false;
	m_bSpline  = false;
	m_bSplined = true;
	m_bRefFoil = true;
	m_bModFoil = false;
	m_bGetPos  = false;
	m_bMark    = false;
	m_bMarked  = false;
	m_bSmooth  = false;

	m_QGraph.SetDrawRect(m_rGraphRect);
	m_QGraph.Init();
	m_pQCurve->SetVisible(true);
	m_pctrlSpecAlpha->setChecked(true);

	OnSpecal();
	//is a foil set as current in the mainframe ?
	if (g_pCurFoil && pXFoil->m_FoilName==g_pCurFoil->m_FoilName && pXFoil->lqspec)
	{
		m_pRefFoil->CopyFoil(g_pCurFoil);
		m_pRefFoil->m_FoilColor = m_pQCurve->GetColor();
//		m_pXFoil->m_FoilName    = m_pRefFoil->m_FoilName ;
//		InitXFoil(m_pRefFoil);

	}
	else if(!pXFoil->m_FoilName.length())
	{
		// XFoil is not initialized
		//is there anything in the database ?
		if(m_poaFoil->size())
		{
			pFoil = (CFoil*)m_poaFoil->at(0);
			m_pRefFoil->CopyFoil(pFoil);
			m_pRefFoil->m_FoilColor = m_pQCurve->GetColor();
			pXFoil->m_FoilName      = m_pRefFoil->m_FoilName ;
			InitXFoil(m_pRefFoil);
		}
		else
		{
			//nothing to initialize
			if(m_bFullInverse)
			{
				m_pctrlSpec->SetValue(0.0);
				m_pctrlTAngle->SetValue(0.0);
				m_pctrlTGapx->SetValue(0.0);
				m_pctrlTGapy->SetValue(0.0);
			}
			else
			{
				m_pctrlIter->SetValue(pXFoil->niterq);
			}

			Clear();
			CheckActions();
			return false;
		}
	}


	//XFOIL has already been initialized so retrieve the foil
	for (int i=1; i<=pXFoil->n; i++)
	{
		m_pRefFoil->x[i-1]  = pXFoil->x[i];
		m_pRefFoil->y[i-1]  = pXFoil->y[i];
		m_pRefFoil->xb[i-1] = pXFoil->x[i];
		m_pRefFoil->yb[i-1] = pXFoil->y[i];
	}

	m_pRefFoil->n          = pXFoil->n;
	m_pRefFoil->nb         = pXFoil->n;
	m_pRefFoil->m_FoilName = pXFoil->m_FoilName;
	m_pRefFoil->InitFoil();
	m_pModFoil->m_FoilName = pXFoil->m_FoilName + tr(" Modified");

	SetFoil();
	CheckActions();
	return true;
}



void QXInverse::SetScale(QRect CltRect)
{
	m_rCltRect = CltRect;

	int h = CltRect.height();
	int h4 = (int)(h/3.0);
	int w = CltRect.width();
	int w20 = (int)(w/20);
	m_rGraphRect = QRect(w20, 10, + m_rCltRect.width()-2*w20, m_rCltRect.height()-h4);
	m_QGraph.SetMargin(50);
	m_QGraph.SetDrawRect(m_rGraphRect);

	ResetScale();
}




void QXInverse::SetTAngle(double a)
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	pXFoil->agte = a/180.0;
}


void QXInverse::SetTGap(double tr, double ti)
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	pXFoil->dzte = complex<double>(tr,ti);
}


void QXInverse::SetupLayout()
{
//	QDesktopWidget desktop;
//	QRect r = desktop.geometry();
//	setMinimumHeight(r.height()/3);
//	move(r.width()/3, r.height()/6);

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

	QGridLayout *SpecLayout = new QGridLayout;
	m_pctrlSpecAlpha = new QRadioButton(tr("Alpha"));
	m_pctrlSpecCl = new QRadioButton(tr("Cl"));
	m_pctrlSpecif = new QLabel(tr("Alpha = "));
	m_pctrlSpecif->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	m_pctrlSpec   = new FloatEdit(1.23);
	SpecLayout->addWidget(m_pctrlSpecAlpha,1,1);
	SpecLayout->addWidget(m_pctrlSpecCl,1,2);
	SpecLayout->addWidget(m_pctrlSpecif,2,1);
	SpecLayout->addWidget(m_pctrlSpec,2,2);
	QGroupBox *SpecBox = new QGroupBox(tr("Specification"));
	SpecBox->setLayout(SpecLayout);

	QGridLayout *ModLayout = new QGridLayout;
	m_pctrlShowSpline    = new QCheckBox(tr("ShowSpline"));
	m_pctrlTangentSpline = new QCheckBox(tr("Tangent Spline"));
	m_pctrlNewSpline     = new QPushButton(tr("New Spline"));
	m_pctrlApplySpline   = new QPushButton(tr("Apply Spline"));
	m_pctrlResetQSpec    = new QPushButton(tr("Reset QSpec"));
	m_pctrlPert          = new QPushButton(tr("Pert"));
	m_pctrlNewSpline->setCheckable(true);
	ModLayout->addWidget(m_pctrlShowSpline,1,1);
	ModLayout->addWidget(m_pctrlTangentSpline,1,2);
	ModLayout->addWidget(m_pctrlNewSpline,2,1);
	ModLayout->addWidget(m_pctrlApplySpline,2,2);
	ModLayout->addWidget(m_pctrlResetQSpec,3,1);
	ModLayout->addWidget(m_pctrlPert,3,2);
	QGroupBox *ModBox = new QGroupBox(tr("Modification"));
	ModBox->setLayout(ModLayout);

	QGridLayout *SmoothLayout = new QGridLayout;
	m_pctrlSmooth = new QPushButton(tr("Smooth QSpec"));
	m_pctrlFilter = new QPushButton(tr("Hannig Filter"));
	QLabel *lab0 = new QLabel(tr("Filter parameter"));
	lab0->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	m_pctrlFilterParam = new FloatEdit(2.34);
	SmoothLayout->addWidget(m_pctrlSmooth,1,1);
	SmoothLayout->addWidget(m_pctrlFilter,1,2);
	SmoothLayout->addWidget(lab0,2,1);
	SmoothLayout->addWidget(m_pctrlFilterParam,2,2);
	QGroupBox *SmoothBox = new QGroupBox(tr("Smoothing"));
	SmoothBox->setLayout(SmoothLayout);

	QGridLayout *TELayout = new QGridLayout;
	QLabel *lab1 = new QLabel(tr("T.E. Angle"));
	QLabel *lab2 = new QLabel(tr("T.E. Gap dx/c"));
	QLabel *lab3 = new QLabel(tr("T.E. Gap dy/c"));
	lab1->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	lab2->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	lab3->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	m_pctrlTAngle = new FloatEdit(3.681);
	m_pctrlTGapx  = new FloatEdit(0.001);
	m_pctrlTGapy  = new FloatEdit(0.002);
	m_pctrlTAngle->SetPrecision(3);
	m_pctrlTGapx->SetPrecision(3);
	m_pctrlTGapy->SetPrecision(3);
	TELayout->addWidget(lab1,1,1);
	TELayout->addWidget(lab2,2,1);
	TELayout->addWidget(lab3,3,1);
	TELayout->addWidget(m_pctrlTAngle,1,2);
	TELayout->addWidget(m_pctrlTGapx,2,2);
	TELayout->addWidget(m_pctrlTGapy,3,2);
	m_pctrlSymm = new QCheckBox(tr("Symmetric foil"));
	QVBoxLayout *ConstraintsLayout = new QVBoxLayout;
	ConstraintsLayout->addLayout(TELayout);
	ConstraintsLayout->addWidget(m_pctrlSymm);
	QGroupBox *ConstraintsBox = new QGroupBox(tr("Constraints"));
	ConstraintsBox->setLayout(ConstraintsLayout);

	m_pctrlExec = new QPushButton(tr("Execute"));
	m_pctrlOutput = new QTextEdit(" ");
	m_pctrlOutput->setEnabled(false);

	QVBoxLayout *FInvLayout = new QVBoxLayout;
	FInvLayout->addWidget(SpecBox);
	FInvLayout->addWidget(ModBox);
	FInvLayout->addWidget(SmoothBox);
	FInvLayout->addWidget(ConstraintsBox);
	FInvLayout->addWidget(m_pctrlExec);
	FInvLayout->addWidget(m_pctrlOutput);
	FInvLayout->addStretch(1);
	m_pctrlFInvWidget = new QWidget(this);
	m_pctrlFInvWidget->setLayout(FInvLayout);

	//specific MInv Controls
	m_pctrlMAlphaSpec     = new QLineEdit(tr("Alpha = "));
	m_pctrlMClSpec        = new QLineEdit(tr("Cl ="));
	m_pctrlMAlphaSpec->setEnabled(false);
	m_pctrlMClSpec->setEnabled(false);
	QHBoxLayout *MSpecLayout = new QHBoxLayout;
	MSpecLayout->addWidget(m_pctrlMAlphaSpec);
	MSpecLayout->addWidget(m_pctrlMClSpec);

	m_pctrlIter           = new FloatEdit(11);
	m_pctrlMark           = new QPushButton(tr("Mark for modification"));
	m_pctrlCpxx           = new QCheckBox(tr("End Point Constraint"));
	m_pctrlMExec          = new QPushButton(tr("Execute"));
	m_pctrlMOutput        = new QTextEdit(tr(" "));
	m_pctrlMShowSpline    = new QCheckBox(tr("ShowSpline"));
	m_pctrlMTangentSpline = new QCheckBox(tr("Tangent Spline"));
	m_pctrlMNewSpline     = new QPushButton(tr("New Spline"));
	m_pctrlMApplySpline   = new QPushButton(tr("Apply Spline"));
	m_pctrlMSmooth        = new QPushButton(tr("Smooth"));
	m_pctrlMResetQSpec    = new QPushButton(tr("Reset QSpec"));
	m_pctrlMNewSpline->setCheckable(true);
	m_pctrlMark->setCheckable(true);
	m_pctrlMOutput->setEnabled(false);

	QGridLayout *MSplineslayout = new QGridLayout;
	MSplineslayout->addWidget(m_pctrlMShowSpline,1,1);
	MSplineslayout->addWidget(m_pctrlMTangentSpline,1,2);
	MSplineslayout->addWidget(m_pctrlMNewSpline,2,1);
	MSplineslayout->addWidget(m_pctrlMApplySpline,2,2);
	MSplineslayout->addWidget(m_pctrlMark,3,1,1,2);
	MSplineslayout->addWidget(m_pctrlMSmooth,4,1);
	MSplineslayout->addWidget(m_pctrlMResetQSpec,4,2);
	QGroupBox *MSplinesBox = new QGroupBox(tr("Modification"));
	MSplinesBox->setLayout(MSplineslayout);

	QVBoxLayout *FoilLayout = new QVBoxLayout;
	FoilLayout->addWidget(m_pctrlCpxx);
	FoilLayout->addWidget(m_pctrlMExec);
	QHBoxLayout *MaxIter = new QHBoxLayout;
	QLabel *lab10 = new QLabel(tr("Max Iterations"));
	MaxIter->addWidget(lab10);
	MaxIter->addWidget(m_pctrlIter);
	FoilLayout->addLayout(MaxIter);
	QGroupBox *FoilBox = new QGroupBox(tr("Foil"));
	FoilBox->setLayout(FoilLayout);

	QVBoxLayout *MInvLayout = new QVBoxLayout;
	MInvLayout->addLayout(MSpecLayout);
	MInvLayout->addWidget(MSplinesBox);
	MInvLayout->addWidget(FoilBox);
	MInvLayout->addWidget(m_pctrlMOutput);
	MInvLayout->addStretch(1);
	m_pctrlMInvWidget = new QWidget(this);
	m_pctrlMInvWidget->setLayout(MInvLayout);

	m_pctrlStackedInv = new QStackedWidget;
	m_pctrlStackedInv->addWidget(m_pctrlFInvWidget);
	m_pctrlStackedInv->addWidget(m_pctrlMInvWidget);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addWidget(m_pctrlStackedInv);
	MainLayout->addStretch(1);
	setLayout(MainLayout);
	Connect();
}


void QXInverse::Smooth(int Pos1, int Pos2)
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;

	if(Pos1 ==-1)
	{
		//smooth it all
		Pos1 = 1;
		Pos2 = pXFoil->nsp;
	}
	m_bGetPos = false;
	if (abs(Pos2-Pos1)<=2) return;
	if (Pos1>Pos2)
	{
		int tmp = Pos2;
		Pos2  = Pos1;
		Pos1  = tmp;
	}
	if(m_bFullInverse)
	{
		pXFoil->smooq(Pos1,Pos2,1);
		pXFoil->cncalc(pXFoil->qspec[1], false);
		pXFoil->qspcir();
		pXFoil->lqspec = true;
	}
	else
	{
		// added v1.17, i.e. different sequence as for Full Inverse
		pXFoil->smooq(Pos1,Pos2,1);
		pXFoil->splqsp(1);
		pXFoil->clcalc(pXFoil->xcmref,pXFoil->ycmref);
//		pXFoil->lqspec = true; ?? should we
	}
	CreateMCurve();
	m_bSmooth = false;
	CancelSmooth();
	UpdateView();
}


void QXInverse::UpdateView()
{
	TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;

	if(m_p2DWidget)
	{
		p2DWidget->update();
	}
}


void QXInverse::wheelEvent(QWheelEvent *event)
{
	ReleaseZoom();
	QPoint pttmp(event->pos().x(), event->pos().y());
	if(m_QGraph.IsInDrawRect(pttmp))
	{
		if (m_bXPressed || m_bZoomXOnly)
		{
			//zoom x scale
			m_QGraph.SetAutoX(false);
			if(event->delta()>0) m_QGraph.Scalex(1.06);
			else                 m_QGraph.Scalex(1.0/1.06);
		}
		else if(m_bYPressed || m_bZoomYOnly)
		{
			//zoom y scale
			m_QGraph.SetAutoY(false);
			if(event->delta()>0) m_QGraph.Scaley(1.06);
			else                 m_QGraph.Scaley(1.0/1.06);
		}
		else
		{
			//zoom both
			m_QGraph.SetAuto(false);
			if(event->delta()>0) m_QGraph.Scale(1.06);
			else                 m_QGraph.Scale(1.0/1.06);
		}
		m_QGraph.SetAutoXUnit();
		m_QGraph.SetAutoYUnit();
	}
	else
	{
		double scale = m_fScale;

		if(event->delta()<0) m_fScale *= 1.06;
		else                 m_fScale /= 1.06;

		int a = (int)((m_rCltRect.right() + m_rCltRect.left())/2);
		m_ptOffset.rx() = a + (int)((m_ptOffset.x()-a)*m_fScale/scale);
	}
	UpdateView();
}


