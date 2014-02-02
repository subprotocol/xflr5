/****************************************************************************

	XInverse Class
	Copyright (C) 2009-2014 Andre Deperrois adeperrois@xflr5.com

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
#include <QAction>
#include <QMessageBox>
#include <QStatusBar>
#include <QtDebug>

#include "XInverse.h" 
#include "FoilSelectionDlg.h"
#include "PertDlg.h"
#include "InverseOptionsDlg.h"
#include "../globals.h"
#include "../mainframe.h"
#include "../misc/Settings.h"
#include "../objects/Foil.h"
#include "../misc/RenameDlg.h"
#include "../graph/GraphDlg.h"
#include "../xdirect/analysis/XFoil.h"


void *QXInverse::s_pMainFrame;
void *QXInverse::s_p2DWidget;

/** The public contructor */
QXInverse::QXInverse(QWidget *parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	m_bFullInverse = false;

	m_pXFoil = NULL;
	m_pCurGraph = NULL;

	m_bTransGraph    = false;
	m_bLoaded        = false;
	m_bZoomPlus      = false;
	m_bZoomXOnly     = false;
	m_bZoomYOnly     = false;
	m_bShowPoints    = false;
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

	m_pRefFoil = new Foil();
	m_pModFoil = new Foil();
	m_pRefFoil->m_FoilColor  = QColor(255,0,0);
	m_pRefFoil->m_FoilStyle = 0;
	m_pRefFoil->m_FoilWidth = 0;
	m_pModFoil->m_FoilColor  = QColor(0,0,255);
	m_pModFoil->m_FoilStyle = 0;
	m_pModFoil->m_FoilWidth = 1;

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

/**
 * The public destructor
 */
QXInverse::~QXInverse()
{
	Trace("Destroying XInverse");
	delete m_pModFoil;
	delete m_pRefFoil;
//	delete m_pPertDlg;
//	delete m_pGraphDlg;
//	delete m_pXInverseStyleDlg;
}

/**
 * Cancels the existing modification limits on the velocity curve
 */
void QXInverse::CancelMark()
{
	m_pctrlMark->setChecked(false);
	m_bGetPos = false;
	m_bMark   = false;
}


/**
 * Cancels the existing smoothing limits on the velocity curve
 */
void QXInverse::CancelSmooth()
{
	m_bSmooth = false;
	m_bGetPos = false;
	m_pctrlSmooth->setChecked(false);
}

/**
 * Cancels the spline definition process
 */
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


/**
 * Initializes the widgets and actions with the current data
 */
void QXInverse::CheckActions()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
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

/**
 * Clears the data associated to the loaded Foil
 */
void QXInverse::Clear()
{
	m_pRefFoil->n = 0;
	m_pRefFoil->m_FoilName = "";
	m_pModFoil->m_FoilName = "";
	m_bLoaded = false;
	m_pReflectedCurve->clear();
	m_pMCurve->clear();
	m_pQCurve->clear();
	m_pQVCurve->clear();
}


/**
 * Performs the connections between SIGNALS and SLOTS
 */
void QXInverse::Connect()
{
	connect(this, SIGNAL(projectModified()), (MainFrame*)s_pMainFrame, SLOT(OnProjectModified()));

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


/**
 * Creates the velocity curve
 */
void QXInverse::CreateQCurve()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	double x,y;
	m_pQCurve->clear();

	int points;
	if(m_bFullInverse) points = 257;
	else points  = pXFoil->n;

	for (int i=1; i<=points; i++)
	{
		x = 1.0 - pXFoil->sspec[i];
		y = pXFoil->qcomp(pXFoil->qspec[1][i])/pXFoil->qinf;
		m_pQCurve->AppendPoint(x,y);
	}
}

/**
 * Creates the modified velocity specification curve
 */
void QXInverse::CreateMCurve()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	int i, points;
	double x,y;
	m_pMCurve->clear();
	m_pReflectedCurve->clear();

	if(m_bFullInverse) points = 257;
	else               points = pXFoil->n;

	for (i=1; i<=points; i++)
	{
		x = 1.0 - pXFoil->sspec[i];
		y = pXFoil->qcomp(pXFoil->qspec[1][i])/pXFoil->qinf;
		m_pMCurve->AppendPoint(x,y);
		m_pReflectedCurve->AppendPoint(pXFoil->sspec[i],-y);
	}
}

/**
 * Draws the grid underneath the Foil display
 * @param painter the instance of the QPainter object on which to draw
 * @param scale the scaling factor for drawing
 */
void QXInverse::DrawGrid(QPainter &painter, double scale)
{
	painter.save();
	double scalex;
	int TickSize;

	TickSize = 5;
	scalex= scale;

	QPen TextPen(Settings::s_TextColor);
	painter.setPen(TextPen);

	//neutral line first
//	QPen LinePen(MainFrame::m_TextColor);
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


/**
 * Executes a full inverse design analysis
 * Updates the geometry of the modified Foil
 */
void QXInverse::ExecMDES()
{
//----- put modified info back into global arrays

	XFoil *pXFoil = (XFoil*)m_pXFoil;

	int i;
	double qscom;
	for (i=1; i<= pXFoil->nsp; i++)
	{
//		isp = pXFoil->nsp - i + 1;
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


/**
 * Executes a mixed inverse design analysis
 * Updates the geometry of the modified Foil
 *@return true unless the modifications points were not marked
 */
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
//	int isp;
	double qscom;
	for (i=1; i<= pXFoil->nsp; i++)
	{
//		isp = pXFoil->nsp - i + 1;
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


/**
 * Initializes XFoil with the data from the input Foil object
 * @param pFoil a pointer to the Foil object with which to initialize the XFoil object
 * @true if the initialization has been sucessful
 */
bool QXInverse::InitXFoil(Foil * pFoil)
{
	//loads pFoil in XFoil, calculates normal vectors, and sets results in current foil
	if(!pFoil) return  false;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
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


/**
 * Overrides the QWidget's keyPressEvent method.
 * Dispatches the key press event
 * @param event the QKeyEvent
 */
void QXInverse::keyPressEvent(QKeyEvent *event)
{
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
		case Qt::Key_Enter:
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


/**
 * Overrides the QWidget's keyReleaseEvent method.
 * Dispatches the key release event
 * @param event the QKeyEvent
 */
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
				MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
				pMainFrame->InverseZoomX->setChecked(false);
			}
			if(m_bZoomYOnly)
			{
				m_bZoomYOnly = false;
				MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
				pMainFrame->InverseZoomY->setChecked(false);
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


/**
 * Loads the user's default settings from the application QSettings object
 * @param pSettings a pointer to the QSettings object
 */
void QXInverse::LoadSettings(QSettings *pSettings)
{
	pSettings->beginGroup("XInverse");
	{
		m_bFullInverse = pSettings->value("FullInverse").toBool();

		m_Spline.SetColor(pSettings->value("SplineColor").value<QColor>());
		m_Spline.SetStyle(pSettings->value("SplineStyle").toInt());
		m_Spline.SetWidth(pSettings->value("SplineWdth").toInt());
		m_pRefFoil->m_FoilColor  = pSettings->value("BaseFoilColor").value<QColor>();
		m_pRefFoil->m_FoilStyle = pSettings->value("BaseFoilStyle").toInt();
		m_pRefFoil->m_FoilWidth = pSettings->value("BaseFoilWidth").toInt();
		m_pModFoil->m_FoilColor  = pSettings->value("ModFoilColor").value<QColor>();
		m_pModFoil->m_FoilStyle = pSettings->value("ModFoilStyle").toInt();
		m_pModFoil->m_FoilWidth = pSettings->value("ModFoilWidth").toInt();
	}
	pSettings->endGroup();
	m_QGraph.LoadSettings(pSettings);
}


/**
 * Overrides the QWidget's mouseDoubleClickEvent method.
 * Dispatches the event
 * @param event the QMouseEvent
 */
void QXInverse::doubleClickEvent(QPoint pos)
{
	if (!m_QGraph.IsInDrawRect(pos)) return;

	OnGraphSettings();
}


/**
 * Overrides the QWidget's mouseMoveEvent method.
 * Dispatches the event
 * @param event the QMouseEvent
 */
void QXInverse::mouseMoveEvent(QMouseEvent *event)
{
//	if(!hasFocus()) setFocus();
	static double x1,y1, xmin, xmax, ymin,  ymax, xpt, ypt, scale, ux, uy, unorm, vx, vy, vnorm, scal;
	static double xx0,xx1,xx2,yy0,yy1,yy2, dist;
	static int a, n, ipt;
	static QPoint point;
	point = event->pos();

	if(m_bGetPos)
	{
		m_tmpPos = m_pMCurve->closestPoint(m_QGraph.ClientTox(point.x()), m_QGraph.ClientToy(point.y()), dist);
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
				ipt = m_pMCurve->closestPoint(m_QGraph.ClientTox(point.x()), m_QGraph.ClientToy(point.y()), dist);
				m_SplineLeftPos = ipt;
				xpt = m_pMCurve->x[ipt];
				ypt = m_pMCurve->y[ipt];
				// check for inversion
				if(xpt> m_Spline.m_CtrlPoint.last().x)
				{
					m_Spline.m_CtrlPoint[n].x = m_Spline.m_CtrlPoint.last().x;
					m_Spline.m_CtrlPoint[n].y = m_Spline.m_CtrlPoint.last().y;
					m_Spline.m_CtrlPoint.last().x = xpt;
					m_Spline.m_CtrlPoint.last().y = ypt;
				}
				else
				{
					m_Spline.m_CtrlPoint[n].x = xpt;
					m_Spline.m_CtrlPoint[n].y = ypt;
				}
				m_bSplined = false;
				m_Spline.SplineCurve();
			}
			else if(n == m_Spline.m_CtrlPoint.size()-1)
			{
				// user is dragging end point
				// find closest graph point
				ipt = m_pMCurve->closestPoint(m_QGraph.ClientTox(point.x()), m_QGraph.ClientToy(point.y()), dist);
				m_SplineRightPos = ipt;
				xpt = m_pMCurve->x[ipt];
				ypt = m_pMCurve->y[ipt];
				// check for inversion
				if(xpt< m_Spline.m_CtrlPoint[0].x)
				{
					m_Spline.m_CtrlPoint[n].x = m_Spline.m_CtrlPoint[0].x;
					m_Spline.m_CtrlPoint[n].y = m_Spline.m_CtrlPoint[0].y;
					m_Spline.m_CtrlPoint[0].x = xpt;
					m_Spline.m_CtrlPoint[0].y = ypt;
				}
				else
				{
					m_Spline.m_CtrlPoint[n].x = xpt;
					m_Spline.m_CtrlPoint[n].y = ypt;
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

				vx = x1-m_Spline.m_CtrlPoint[n-1].x;
				vy = y1-m_Spline.m_CtrlPoint[n-1].y;

				scal =  (ux*vx + uy*vy);
				m_Spline.m_CtrlPoint[n].x = m_Spline.m_CtrlPoint[0].x + scal * ux ;
				m_Spline.m_CtrlPoint[n].y = m_Spline.m_CtrlPoint[0].y + scal * uy ;
				m_Spline.SplineCurve();
				m_bSplined = false;
			}
			else if (n==m_Spline.m_CtrlPoint.size()-2 && m_bTangentSpline)
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

				vx = x1-m_Spline.m_CtrlPoint[n+1].x;
				vy = y1-m_Spline.m_CtrlPoint[n+1].y;

				scal =  (ux*vx + uy*vy);
				m_Spline.m_CtrlPoint[n].x = m_Spline.m_CtrlPoint[n+1].x + scal * ux;
				m_Spline.m_CtrlPoint[n].y = m_Spline.m_CtrlPoint[n+1].y + scal * uy;
				m_Spline.SplineCurve();
				m_bSplined = false;
			}	
			else if (n>0 && n<m_Spline.m_CtrlPoint.size()-1)
			{
				m_Spline.m_CtrlPoint[n].x = x1;
				m_Spline.m_CtrlPoint[n].y = y1;
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
			if (n>=0 && n<m_Spline.m_CtrlPoint.size())
			{
				m_Spline.m_iHighlight = n;
			}
			else m_Spline.m_iHighlight = -1;
			UpdateView();
		}
	}
	if(m_QGraph.IsInDrawRect(point))
	{
		MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
		pMainFrame->statusBar()->showMessage(QString("X = %1, Y = %2").arg(m_QGraph.ClientTox(event->x())).arg(m_QGraph.ClientToy(event->y())));
		m_pCurGraph = &m_QGraph;
	}
	else m_pCurGraph = NULL;
}





/**
 * Overrides the QWidget's mousePressEvent method.
 * Dispatches the event
 * @param event the QMouseEvent
 */
void QXInverse::mousePressEvent(QMouseEvent *event)
{
	TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;
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
								if(!m_Spline.RemovePoint(m_Spline.m_iSelect))
								{
									QMessageBox::warning(p2DWidget,tr("Warning"), tr("The minimum number of control points has been reached for this spline degree"));
									return;
								}
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


/**
 * Overrides the QWidget's mouseReleaseEvent method.
 * Dispatches the event
 * @param event the QMouseEvent
 */
void QXInverse::mouseReleaseEvent(QMouseEvent *event)
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;
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
			w = qAbs(xu-xd);
			h = qAbs(yu-yd);
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
			m_Pos1 = m_pMCurve->closestPoint(m_QGraph.ClientTox(point.x()), m_QGraph.ClientToy(point.y()), dist);
		}
		if(m_nPos == 1)
		{
			m_Pos2 = m_pMCurve->closestPoint(m_QGraph.ClientTox(point.x()), m_QGraph.ClientToy(point.y()), dist);
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
				if(qAbs(x2-x1)<0.00001) return;
				if(x2<x1)
				{
					tmp    = m_Pos2;
					m_Pos2 = m_Pos1;
					m_Pos1 = tmp;
				}

				m_SplineLeftPos  = m_Pos1;
				m_SplineRightPos = m_Pos2;

				m_Spline.m_CtrlPoint.clear();
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
					xpt = m_Spline.m_CtrlPoint[1].x - m_Spline.m_CtrlPoint[0].x;
					ypt = m_Spline.m_CtrlPoint[1].y - m_Spline.m_CtrlPoint[0].y;
				
					m_Spline.m_CtrlPoint[1].x = m_Spline.m_CtrlPoint[0].x + (ux*xpt + uy*ypt) * ux;
					m_Spline.m_CtrlPoint[1].y = m_Spline.m_CtrlPoint[0].y + (ux*xpt + uy*ypt) * uy;
				

					//penultimate point must remain on tangent to curve
					ux = m_pMCurve->x[m_Pos2] - m_pMCurve->x[m_Pos2-1];
					uy = m_pMCurve->y[m_Pos2] - m_pMCurve->y[m_Pos2-1];
					norm = sqrt(ux*ux+uy*uy);
					ux /= norm;
					uy /= norm;

					xpt = m_Spline.m_CtrlPoint[m_Spline.m_CtrlPoint.size()-2].x - m_Spline.m_CtrlPoint[m_Spline.m_CtrlPoint.size()-1].x;
					ypt = m_Spline.m_CtrlPoint[m_Spline.m_CtrlPoint.size()-2].y - m_Spline.m_CtrlPoint[m_Spline.m_CtrlPoint.size()-1].y;
					
					m_Spline.m_CtrlPoint[m_Spline.m_CtrlPoint.size()-2].x = m_Spline.m_CtrlPoint[m_Spline.m_CtrlPoint.size()-1].x + (ux*xpt + uy*ypt) * ux;
					m_Spline.m_CtrlPoint[m_Spline.m_CtrlPoint.size()-2].y = m_Spline.m_CtrlPoint[m_Spline.m_CtrlPoint.size()-1].y + (ux*xpt + uy*ypt) * uy;
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

	p2DWidget->setCursor(Qt::CrossCursor);
	UpdateView();
}



/**
 * The user has requested to apply the spline to the velocity curve
 */
void QXInverse::OnApplySpline()
{
	if(!m_bSplined)
	{
		XFoil *pXFoil = (XFoil*)m_pXFoil;
		int i;
		double qscom, xx;
		for (i=1; i<m_pMCurve->size()-1; i++)
		{
			xx = m_pMCurve->x[i];
			if (xx > m_Spline.m_CtrlPoint.first().x &&
				xx < m_Spline.m_CtrlPoint.last().x )
			{
				//interpolate spline at xx
				m_pMCurve->y[i] = m_Spline.GetY(xx);
			}
		}

		for (i=1; i<m_pMCurve->size()-1; i++)
		{
			m_pReflectedCurve->y[i] = -m_pMCurve->y[i];
		}

		m_bSplined = true;
		for (i=1; i<= pXFoil->nsp; i++)
		{
//			isp = pXFoil->nsp - i + 1;
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
	emit projectModified();
}


/**
 * Not sure - refer to XFoil documentation
 */
void QXInverse::OnCpxx()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	CancelSpline();
	CancelSmooth();
	CancelMark();
	if (m_bZoomPlus) ReleaseZoom();
	pXFoil->lcpxx = m_pctrlCpxx->isChecked();
}


/**
 * The user has requested the execution of the modification
 */
void QXInverse::OnExecute()
{
	if (m_bZoomPlus) ReleaseZoom();
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	CancelSpline();
	CancelSmooth();
	CancelMark();

	if(m_bFullInverse)
	{
		SetTAngle(m_pctrlTAngle->Value());
		SetTGap(m_pctrlTGapx->Value(), m_pctrlTGapy->Value());
		m_pctrlOutput->setPlainText(" ");
		ExecMDES();
	}
	else
	{
		pXFoil->niterq = m_pctrlIter->Value();
		m_pctrlMOutput->setPlainText(" ");
		ExecQDES();
	}
	UpdateView();
	emit projectModified();
}


/**
 * 	Extracts a Foil from the database for display and modification
*/
void QXInverse::OnExtractFoil()
{
    FoilSelectionDlg dlg(this);
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
		Foil *pFoil;
		pFoil = Foil::foil(dlg.m_FoilName);
		Foil::setCurFoil(pFoil);

		m_pRefFoil->CopyFoil(pFoil);

		m_pModFoil->m_FoilName = m_pRefFoil->m_FoilName + tr(" Modified");
		InitXFoil(m_pRefFoil);
		SetFoil();
		UpdateView();
	}
}


/**
 * Applies a Hanning type filter to the velocity curve
 */
void QXInverse::OnFilter()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	CancelSpline();
	if (m_bZoomPlus) ReleaseZoom();

	double filt = m_pctrlFilterParam->Value();
	pXFoil->Filter(filt);
	CreateMCurve();

	UpdateView();
}


/**
 *The user has requested an edition of the graph settings
*/
void QXInverse::OnGraphSettings()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	GraphDlg *m_pGraphDlg = new GraphDlg(pMainFrame);

	m_pGraphDlg->m_iGraphType = 31;
	m_pGraphDlg->m_XSel = 0;
	m_pGraphDlg->m_YSel = 0;
	m_pGraphDlg->m_pGraph = &m_QGraph;

	QGraph graph;
	graph.CopySettings(&m_QGraph);
	m_pGraphDlg->m_pMemGraph = &m_QGraph;
	m_pGraphDlg->m_pGraph = &m_QGraph;
	m_pGraphDlg->SetParams();

	if(m_pGraphDlg->exec() == QDialog::Accepted)
	{
	}
	else
	{
		m_QGraph.CopySettings(&graph);
	}

	UpdateView();
}


/**
 * The user has requested the insertion of a new control point in the Spline object at the mouse position
 */
void QXInverse::OnInsertCtrlPt()
{
	double xd = m_QGraph.ClientTox(m_ptPopUp.x());
	double yd = m_QGraph.ClientToy(m_ptPopUp.y());

	if(xd < m_Spline.m_CtrlPoint.first().x) return;
	if(xd > m_Spline.m_CtrlPoint.last().x) return;

	m_Spline.InsertPoint(xd,yd);
	m_Spline.SplineKnots();
	m_Spline.SplineCurve();
	UpdateView();
}


/**
 * The user has toggled between full-inverse and mixed-inverse applications
 */
void QXInverse::OnInverseApp()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
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


/**
 * The user has requested a modification of the styles of the curves
 */
void QXInverse::OnInverseStyles()
{
	InverseOptionsDlg *m_pXInverseStyleDlg = new InverseOptionsDlg((MainFrame*)s_pMainFrame);
	m_pXInverseStyleDlg->m_pXInverse = this;
	m_pXInverseStyleDlg->InitDialog();
	m_pXInverseStyleDlg->exec();
}


/**
 * The user has requested to mark a segment for modification on the curve
 */
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


/**
 * The user has requested the creation of a new spline to define the modification of the velocity curve
 */
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


/**
 * @todo check The user has requested the launch of the interface to define the perturbation to the curve
 */
void QXInverse::OnPertubate()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	int m;
	pXFoil->pert_init(1);

	PertDlg PerturbDlg((MainFrame*)s_pMainFrame);

	for (m=0; m<=qMin(32, pXFoil->nc); m++)
	{
		PerturbDlg.m_cnr[m] = (double)real(pXFoil->cn[m]);
		PerturbDlg.m_cni[m] = (double)imag(pXFoil->cn[m]);
	}
	PerturbDlg.m_nc = qMin(32, pXFoil->nc);
	PerturbDlg.InitDialog();

	if(PerturbDlg.exec() == QDialog::Accepted)
	{
		for (m=0; m<=qMin(32, pXFoil->nc); m++)
		{
			pXFoil->cn[m] = complex<double>(PerturbDlg.m_cnr[m], PerturbDlg.m_cni[m]);
		}

		pXFoil->pert_process(1);
		CreateMCurve();
		m_pMCurve->SetVisible(true);
		UpdateView();
	}
}


/** Toggles the visibility of the reference curve */
void QXInverse::OnQInitial()
{
	m_pQCurve->SetVisible(!m_pQCurve->IsVisible());
	CheckActions();
	UpdateView();
}

/** Toggles the visibility of the specification curve */
void QXInverse::OnQSpec()
{
	m_pMCurve->SetVisible(!m_pMCurve->IsVisible());
	CheckActions();
	UpdateView();
}

/** Toggles the visibility of the viscous curve */
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

/** Toggles the visibility of the curve's points */
void QXInverse::OnQPoints()
{
	m_bShowPoints = !m_bShowPoints;
	m_pQCurve->ShowPoints(m_bShowPoints);
	m_pMCurve->ShowPoints(m_bShowPoints);
	CheckActions();
	UpdateView();
}

/** Toggles the visibility of the reflected curve */
void QXInverse::OnQReflected()
{
	m_bReflected = !m_bReflected;
	m_pReflectedCurve->SetVisible(m_bReflected);
	CheckActions();
	UpdateView();
}


/** Resets the specification curve */
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

/**
 * The user has requested to remove the selected control point from the spline
 */
void QXInverse::OnRemoveCtrlPt()
{
	if (m_Spline.m_iHighlight>=0)
	{
		if(!m_Spline.RemovePoint(m_Spline.m_iHighlight))
		{
			MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
			QMessageBox::warning(pMainFrame,tr("Warning"), tr("The minimum number of control points has been reached for this spline degree"));
			return;
		}
	}
	m_Spline.SplineCurve();
	UpdateView();
}

/** The user has requested a reset of the Foil scale */
void QXInverse::OnResetFoilScale()
{
	ReleaseZoom();
	ResetScale();
	UpdateView();
}


/**
 * The user has toggled between aoa and lift coefficient as the input parameter
 */
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

/**
 * The user has modified the value of the aoa or lift coefficient
 */
void QXInverse::OnSpecInv()
{
	if (m_bZoomPlus) ReleaseZoom();
	XFoil *pXFoil = (XFoil*)m_pXFoil;

	if(m_pctrlSpecAlpha->isChecked())
	{
		pXFoil->alqsp[1] = m_pctrlSpec->Value()*PI/180.0;
		pXFoil->iacqsp = 1;
		pXFoil->qspcir();
	}
	else if(m_pctrlSpecCl->isChecked())
	{
		pXFoil->clqsp[1] = m_pctrlSpec->Value();
		pXFoil->iacqsp = 2;
		pXFoil->qspcir();
	}
	CreateQCurve();
	CreateMCurve();
	UpdateView();
}


/** The user has toggled the spline display */
void QXInverse::OnShowSpline()
{
	if(m_bFullInverse) m_bSpline = m_pctrlShowSpline->isChecked();
	else               m_bSpline = m_pctrlMShowSpline->isChecked();
	m_bSplined =   !m_bSpline;
	UpdateView();
}

/** The user has requested a smoothing operation on the curve */
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


/**
 * The user has requested the storage of the modified Foil in the database
 */
void QXInverse::OnStoreFoil()
{
	if(!m_bLoaded) return;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	Foil* pFoil = new Foil();
	pFoil->CopyFoil(m_pModFoil);
	pFoil->m_FoilStyle = 0;
	pFoil->m_FoilWidth = 1;
	memcpy(pFoil->xb, m_pModFoil->x, sizeof(m_pModFoil->x));
	memcpy(pFoil->yb, m_pModFoil->y, sizeof(m_pModFoil->y));
	pFoil->nb = m_pModFoil->n;
	pFoil->m_FoilName = m_pRefFoil->m_FoilName;

	QStringList NameList;
	for(int k=0; k<Foil::s_oaFoil.size(); k++)
	{
		Foil *pOldFoil = (Foil*)Foil::s_oaFoil.at(k);
		NameList.append(pOldFoil->m_FoilName);
	}

	RenameDlg renDlg(pMainFrame);
	renDlg.InitDialog(&NameList, Foil::curFoil()->foilName(), tr("Enter the foil's new name"));

	if(renDlg.exec() !=QDialog::Rejected)
	{
		Foil::curFoil()->insertThisFoil();
	}
}


/** The user has toggled the symetric requirement for the foil*/
void QXInverse::OnSymm()
{
	CancelSpline();

	if (m_bZoomPlus) ReleaseZoom();

	XFoil *pXFoil = (XFoil*)m_pXFoil;
	pXFoil->lqsym = m_pctrlSymm->isChecked();
	pXFoil->lqspec = false;
}


/** The user has requested to zoom in on a part of the display */
void QXInverse::OnZoomIn()
{
	if(!m_bZoomPlus)
	{
		if(m_fScale/m_fRefScale <32.0)
		{
			m_bZoomPlus = true;
			MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
			pMainFrame->InverseZoomIn->setChecked(true);
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


/** The user has requested to zoom the x-axis only */
void QXInverse::OnZoomX()
{
	ReleaseZoom();
	m_bZoomYOnly = false;
	m_bZoomXOnly = !m_bZoomXOnly;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->InverseZoomX->setChecked(m_bZoomXOnly);
	pMainFrame->InverseZoomY->setChecked(m_bZoomYOnly);
}


/** The user has requested to zoom the y-axis only */
void QXInverse::OnZoomY()
{
	ReleaseZoom();
	m_bZoomXOnly = false;
	m_bZoomYOnly = !m_bZoomYOnly;
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->InverseZoomX->setChecked(m_bZoomXOnly);
	pMainFrame->InverseZoomY->setChecked(m_bZoomYOnly);
}


/** The user has toggled the requirement for a spline tangent to the specification curve */
void QXInverse::OnTangentSpline()
{
	if(m_bFullInverse) m_bTangentSpline = m_pctrlTangentSpline->isChecked();
	else               m_bTangentSpline = m_pctrlMTangentSpline->isChecked();
	m_pctrlTangentSpline->setChecked(m_bTangentSpline);
	m_pctrlMTangentSpline->setChecked(m_bTangentSpline);
}


/**
 * Draws the graph
 * @param painter a reference to the QPainter object with which to draw
 */
void QXInverse::PaintGraph(QPainter &painter)
{
	painter.save();

//  draw  the graph	
	if(m_rGraphRect.width()>200 && m_rGraphRect.height()>150)
	{
		m_QGraph.DrawGraph(painter);
		QPoint Place((int)(m_rGraphRect.right()-300), m_rGraphRect.top()+12);
		m_QGraph.DrawLegend(painter, Place, Settings::s_TextFont, Settings::s_TextColor);
	}

// draw the zoom rectangle, if relevant
	QRect ZRect = m_ZoomRect.normalized();

	if(m_bZoomPlus && !ZRect.isEmpty()) 
	{
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



/**
 * Draws the Foil
 * @param painter a reference to the QPainter object with which to draw
 */
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

	QPen TextPen(Settings::s_TextColor);

	if(m_bRefFoil && m_bLoaded)
	{
		QPen FoilPen(m_pRefFoil->m_FoilColor);
		FoilPen.setStyle(getStyle(m_pRefFoil->m_FoilStyle));
		FoilPen.setWidth(m_pRefFoil->m_FoilWidth);
		painter.setPen(FoilPen);

		m_pRefFoil->DrawFoil(painter, -alpha, m_fScale, m_fScale, m_ptOffset);
		painter.drawLine(20, m_rGraphRect.bottom()+20, 40, m_rGraphRect.bottom()+20);
		painter.setPen(TextPen);
		painter.drawText(50, m_rGraphRect.bottom()+25, m_pRefFoil->m_FoilName);
	}

	if(m_bModFoil && m_bLoaded) 
	{
		QPen ModPen(m_pModFoil->m_FoilColor);
		ModPen.setStyle(getStyle(m_pModFoil->m_FoilStyle));
		ModPen.setWidth(m_pModFoil->m_FoilWidth);
		painter.setPen(ModPen);

		m_pModFoil->DrawFoil(painter, -alpha, m_fScale, m_fScale, m_ptOffset);
		painter.drawLine(20, m_rGraphRect.bottom()+35, 40, m_rGraphRect.bottom()+35);
		painter.setPen(TextPen);
		painter.drawText(50, m_rGraphRect.bottom()+40, m_pModFoil->m_FoilName);
	}

	if (m_pRefFoil->m_bPoints)
	{
		QPen CtrlPen(m_pRefFoil->m_FoilColor);
		CtrlPen.setStyle(getStyle(m_pRefFoil->m_FoilStyle));
		CtrlPen.setWidth(m_pRefFoil->m_FoilWidth);
		painter.setPen(CtrlPen);

		m_pRefFoil->DrawPoints(painter, 1.0,  1.0, m_ptOffset);
	}

	painter.setFont(Settings::s_TextFont);
	QFontMetrics fm(Settings::s_TextFont);
	int dD = fm.height();
	int Back = 4;
	int LeftPos = m_rCltRect.left()+10;
	int ZPos = m_rCltRect.bottom() - 10 - Back*dD;

	int D = 0;

	str = tr("                     Base");
	if(m_bModFoil && m_bLoaded)  str +=tr("       Mod.");

	painter.setPen(TextPen);

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


/** Paints the view
 * @param painter a reference to the QPainter object with which to draw
 */
void QXInverse::PaintView(QPainter &painter)
{
	painter.save();

	painter.fillRect(m_rCltRect, Settings::s_BackgroundColor);
	PaintGraph(painter);
	PaintFoil(painter);

	painter.restore();
}

/**
 * -------------------------------------
 * XFoil source code: sets incompressible speed from karman-tsien compressible speed
 *-------------------------------------
 */
double QXInverse::qincom(double qc, double qinf, double tklam)
{
//-------------------------------------
//     sets incompressible speed from
//     karman-tsien compressible speed
//-------------------------------------

	if(tklam<1.0e-4 || qAbs(qc)<1.0e-4)
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

/**
 * Ends the zoom-in process
 */
void QXInverse::ReleaseZoom()
{
	m_bZoomPlus  = false;
	m_ZoomRect.setRight(m_ZoomRect.left()-1);
	m_ZoomRect.setTop(m_ZoomRect.bottom()+1);
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->InverseZoomIn->setChecked(false);
}


/**
 * Resets the mixed inverse specification curve
 */
void QXInverse::ResetMixedQ()
{
	m_pMCurve->clear();
	for (int i=0; i<=m_pQCurve->size(); i++)
	{
		m_pMCurve->AppendPoint(m_pQCurve->x[i], m_pQCurve->y[i]);
	}

//	m_pXFoil->gamqsp(1);
//	CreateMCurve();
}


/**
 * Resets the reference velocity curve
 */
void QXInverse::ResetQ()
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	pXFoil->cncalc(pXFoil->qgamm,false);
	pXFoil->qspcir();
	CreateMCurve();
	UpdateView();
}


/**
 * Resets the Foil scale
 */
void QXInverse::ResetScale()
{
	int h4 = m_rCltRect.height()/4;
	m_ptOffset.rx() = m_rGraphRect.left() +(int)(1.0*m_QGraph.GetMargin());
	m_fRefScale  = m_rGraphRect.width()-2.0*m_QGraph.GetMargin();

	m_ptOffset.ry() = m_rCltRect.bottom()-h4/2;
	m_fScale = m_fRefScale;
}


/**
 * Saves the user settings
 * @param pSettings a pointer to the QSetting object.
 */
void QXInverse::SaveSettings(QSettings *pSettings)
{
	pSettings->beginGroup("XInverse");
	{
		pSettings->setValue("FullInverse", m_bFullInverse);
		pSettings->setValue("SplineColor", m_Spline.color());
		pSettings->setValue("SplineStyle", m_Spline.style());
		pSettings->setValue("SplineWdth", m_Spline.width());
		pSettings->setValue("BaseFoilColor", m_pRefFoil->m_FoilColor);
		pSettings->setValue("BaseFoilStyle", m_pRefFoil->m_FoilStyle);
		pSettings->setValue("BaseFoilWidth", m_pRefFoil->m_FoilWidth);
		pSettings->setValue("ModFoilColor", m_pModFoil->m_FoilColor);
		pSettings->setValue("ModFoilStyle", m_pModFoil->m_FoilStyle);
		pSettings->setValue("ModFoilWidth", m_pModFoil->m_FoilWidth);
	}
	pSettings->endGroup();

	m_QGraph.SaveSettings(pSettings);
}


/**
 * Initializes the interface with the selected foil
 */
void QXInverse::SetFoil()
{
	int i;
	QFile *pXFile=NULL;
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
		QString FileName = QDir::tempPath() + "/XFLR5.log";
		pXFile = new QFile(FileName);
		if (!pXFile->open(QIODevice::WriteOnly | QIODevice::Text)) pXFile = NULL;

		pXFoil->m_OutStream.setDevice(pXFile);

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

		m_pQVCurve->clear();

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
			m_pQVCurve->AppendPoint(x,y);
			x = 1.0 - sp2;
			y = qv2/pXFoil->qinf;
			m_pQVCurve->AppendPoint(x,y);
		}
		m_pQVCurve->SetVisible(true);
	}
	else
	{
		m_pQVCurve->SetVisible(false);
	}

	m_bLoaded = true;

	if(pXFile)
	{
		pXFile->close();
		delete pXFile;
	}
}

/**
 * Initializes the widgets with the active data
 * @return
 */
bool QXInverse::SetParams()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	Foil*pFoil;

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
	m_pQCurve->SetStyle(m_pRefFoil->m_FoilStyle);
	m_pQCurve->SetWidth(m_pRefFoil->m_FoilWidth);
	m_pMCurve->SetColor(m_pModFoil->m_FoilColor);
	m_pMCurve->SetStyle(m_pModFoil->m_FoilStyle);
	m_pMCurve->SetWidth(m_pModFoil->m_FoilWidth);
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
	if (Foil::curFoil() && pXFoil->m_FoilName==Foil::curFoil()->m_FoilName && pXFoil->lqspec)
	{
		m_pRefFoil->CopyFoil(Foil::curFoil());
		m_pRefFoil->m_FoilColor = m_pQCurve->color();
//		m_pXFoil->m_FoilName    = m_pRefFoil->m_FoilName ;
//		InitXFoil(m_pRefFoil);

	}
	else if(!pXFoil->m_FoilName.length())
	{
		// XFoil is not initialized
		//is there anything in the database ?
		if(m_poaFoil->size())
		{
			pFoil = (Foil*)m_poaFoil->at(0);
			m_pRefFoil->CopyFoil(pFoil);
			m_pRefFoil->m_FoilColor = m_pQCurve->color();
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


/**
 * Sets the scale for the Foil and QGraph display
 * @param CltRect the client area
 */
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



/**
 * Sets the angle at the trailing edge
 * @param a the angle in degrees
 */
void QXInverse::SetTAngle(double a)
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	pXFoil->agte = a/180.0;
}


/**
 * Sets the trailing edge gap.
 */
void QXInverse::SetTGap(double tr, double ti)
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	pXFoil->dzte = complex<double>(tr,ti);
}

/**
 * Sets up the interface
 */
void QXInverse::SetupLayout()
{
	QGroupBox *pSpecBox = new QGroupBox(tr("Specification"));
	{
		QGridLayout *pSpecLayout = new QGridLayout;
		{
			m_pctrlSpecAlpha = new QRadioButton(tr("Alpha"));
			m_pctrlSpecCl = new QRadioButton(tr("Cl"));
			m_pctrlSpecif = new QLabel(tr("Alpha = "));
			m_pctrlSpecif->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
			m_pctrlSpec   = new DoubleEdit(1.23);
			pSpecLayout->addWidget(m_pctrlSpecAlpha,1,1);
			pSpecLayout->addWidget(m_pctrlSpecCl,1,2);
			pSpecLayout->addWidget(m_pctrlSpecif,2,1);
			pSpecLayout->addWidget(m_pctrlSpec,2,2);
			pSpecBox->setLayout(pSpecLayout);
		}
	}

	QGroupBox *pModBox = new QGroupBox(tr("Modification"));
	{
		QGridLayout *pModLayout = new QGridLayout;
		{
			m_pctrlShowSpline    = new QCheckBox(tr("ShowSpline"));
			m_pctrlTangentSpline = new QCheckBox(tr("Tangent Spline"));
			m_pctrlNewSpline     = new QPushButton(tr("New Spline"));
			m_pctrlApplySpline   = new QPushButton(tr("Apply Spline"));
			m_pctrlResetQSpec    = new QPushButton(tr("Reset QSpec"));
			m_pctrlPert          = new QPushButton(tr("Pert"));
			m_pctrlNewSpline->setCheckable(true);
			pModLayout->addWidget(m_pctrlShowSpline,1,1);
			pModLayout->addWidget(m_pctrlTangentSpline,1,2);
			pModLayout->addWidget(m_pctrlNewSpline,2,1);
			pModLayout->addWidget(m_pctrlApplySpline,2,2);
			pModLayout->addWidget(m_pctrlResetQSpec,3,1);
			pModLayout->addWidget(m_pctrlPert,3,2);
		}
		pModBox->setLayout(pModLayout);
	}

	QGroupBox *pSmoothBox = new QGroupBox(tr("Smoothing"));
	{
		QGridLayout *pSmoothLayout = new QGridLayout;
		{
			m_pctrlSmooth = new QPushButton(tr("Smooth QSpec"));
			m_pctrlFilter = new QPushButton(tr("Hanning Filter"));
			QLabel *lab0 = new QLabel(tr("Filter parameter"));
			lab0->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
			m_pctrlFilterParam = new DoubleEdit(0.1,3);
			pSmoothLayout->addWidget(m_pctrlSmooth,1,1);
			pSmoothLayout->addWidget(m_pctrlFilter,1,2);
			pSmoothLayout->addWidget(lab0,2,1);
			pSmoothLayout->addWidget(m_pctrlFilterParam,2,2);
		}
		pSmoothBox->setLayout(pSmoothLayout);
	}

	QGroupBox *pConstraintsBox = new QGroupBox(tr("Constraints"));
	{
		QGridLayout *pTELayout = new QGridLayout;
		{
			QLabel *lab1 = new QLabel(tr("T.E. Angle"));
			QLabel *lab2 = new QLabel(tr("T.E. Gap dx/c"));
			QLabel *lab3 = new QLabel(tr("T.E. Gap dy/c"));
			lab1->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
			lab2->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
			lab3->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
			m_pctrlTAngle = new DoubleEdit(0.000, 3);
			m_pctrlTGapx  = new DoubleEdit(0.000, 3);
			m_pctrlTGapy  = new DoubleEdit(0.000, 3);
			pTELayout->addWidget(lab1,1,1);
			pTELayout->addWidget(lab2,2,1);
			pTELayout->addWidget(lab3,3,1);
			pTELayout->addWidget(m_pctrlTAngle,1, 2);
			pTELayout->addWidget(m_pctrlTGapx, 2, 2);
			pTELayout->addWidget(m_pctrlTGapy, 3, 2);
			m_pctrlSymm = new QCheckBox(tr("Symmetric foil"));
		}
		QVBoxLayout *pConstraintsLayout = new QVBoxLayout;
		{
			pConstraintsLayout->addLayout(pTELayout);
			pConstraintsLayout->addWidget(m_pctrlSymm);
		}
		pConstraintsBox->setLayout(pConstraintsLayout);
	}

	m_pctrlFInvWidget = new QWidget(this);
	{
		QVBoxLayout *pFInvLayout = new QVBoxLayout;
		{
			m_pctrlExec = new QPushButton(tr("Execute"));

			m_pctrlOutput = new MinTextEdit(this);
			m_pctrlOutput->setEnabled(false);

			pFInvLayout->addWidget(pSpecBox);
			pFInvLayout->addWidget(pModBox);
			pFInvLayout->addWidget(pSmoothBox);
			pFInvLayout->addWidget(pConstraintsBox);
			pFInvLayout->addWidget(m_pctrlExec);
			pFInvLayout->addWidget(m_pctrlOutput);
			pFInvLayout->addStretch(1);
		}
		m_pctrlFInvWidget->setLayout(pFInvLayout);
	}

	//specific MInv Controls
	QHBoxLayout *pMSpecLayout = new QHBoxLayout;
	{
		m_pctrlMAlphaSpec     = new QLineEdit(tr("Alpha = "));
		m_pctrlMClSpec        = new QLineEdit(tr("Cl ="));
		m_pctrlMAlphaSpec->setEnabled(false);
		m_pctrlMClSpec->setEnabled(false);
		pMSpecLayout->addWidget(m_pctrlMAlphaSpec);
		pMSpecLayout->addWidget(m_pctrlMClSpec);
	}

	QGroupBox *pMSplinesBox = new QGroupBox(tr("Modification"));
	{
		QGridLayout *pMSplineslayout = new QGridLayout;
		{
			m_pctrlMark           = new QPushButton(tr("Mark for modification"));
			m_pctrlMShowSpline    = new QCheckBox(tr("ShowSpline"));
			m_pctrlMTangentSpline = new QCheckBox(tr("Tangent Spline"));
			m_pctrlMNewSpline     = new QPushButton(tr("New Spline"));
			m_pctrlMApplySpline   = new QPushButton(tr("Apply Spline"));
			m_pctrlMSmooth        = new QPushButton(tr("Smooth"));
			m_pctrlMResetQSpec    = new QPushButton(tr("Reset QSpec"));
			m_pctrlMNewSpline->setCheckable(true);
			m_pctrlMark->setCheckable(true);
			pMSplineslayout->addWidget(m_pctrlMShowSpline,1,1);
			pMSplineslayout->addWidget(m_pctrlMTangentSpline,1,2);
			pMSplineslayout->addWidget(m_pctrlMNewSpline,2,1);
			pMSplineslayout->addWidget(m_pctrlMApplySpline,2,2);
			pMSplineslayout->addWidget(m_pctrlMark,3,1,1,2);
			pMSplineslayout->addWidget(m_pctrlMSmooth,4,1);
			pMSplineslayout->addWidget(m_pctrlMResetQSpec,4,2);
		}
		pMSplinesBox->setLayout(pMSplineslayout);
	}

	QGroupBox *pFoilBox = new QGroupBox(tr("Foil"));
	{
		QVBoxLayout *pFoilLayout = new QVBoxLayout;
		{
			m_pctrlCpxx           = new QCheckBox(tr("End Point Constraint"));
			m_pctrlMExec          = new QPushButton(tr("Execute"));
			pFoilLayout->addWidget(m_pctrlCpxx);
			pFoilLayout->addWidget(m_pctrlMExec);
			QHBoxLayout *pMaxIterLayout = new QHBoxLayout;
			{
				pMaxIterLayout->addStretch();
				QLabel *lab10 = new QLabel(tr("Max Iterations"));
				m_pctrlIter = new IntEdit(this);
				pMaxIterLayout->addWidget(lab10);
				pMaxIterLayout->addWidget(m_pctrlIter);
			}
			pFoilLayout->addLayout(pMaxIterLayout);
			pFoilLayout->addStretch();
		}
		pFoilBox->setLayout(pFoilLayout);
	}

	m_pctrlMInvWidget = new QWidget(this);
	{
		QVBoxLayout *pMInvLayout = new QVBoxLayout;
		{
			m_pctrlMOutput = new MinTextEdit(this);
			m_pctrlMOutput->setEnabled(false);

			pMInvLayout->addLayout(pMSpecLayout);
			pMInvLayout->addWidget(pMSplinesBox);
			pMInvLayout->addWidget(pFoilBox);
			pMInvLayout->addWidget(m_pctrlMOutput);
//			pMInvLayout->addStretch(1);
		}
		m_pctrlMInvWidget->setLayout(pMInvLayout);
	}

	m_pctrlStackedInv = new QStackedWidget;
	{
		m_pctrlStackedInv->addWidget(m_pctrlFInvWidget);
		m_pctrlStackedInv->addWidget(m_pctrlMInvWidget);
	}

	QVBoxLayout *pMainLayout = new QVBoxLayout;
	{
		pMainLayout->addWidget(m_pctrlStackedInv);
		pMainLayout->addStretch(1);
	}
	setLayout(pMainLayout);

	Connect();
}



/**
 * Performs a smoothing operation of the specification cuve between two end points
 * @param Pos1 the first end point
 * @param Pos2 the seconf end point
 */
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

/**
 * Refreshes the display
 */
void QXInverse::UpdateView()
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
 * @param event the QMouseEvent
 */
void QXInverse::zoomEvent(QPoint pos, double zoomFactor)
{
	ReleaseZoom();

	if(m_QGraph.IsInDrawRect(pos))
	{
		if (m_bXPressed || m_bZoomXOnly)
		{
			//zoom x scale
			m_QGraph.SetAutoX(false);
			m_QGraph.Scalex(1.0/zoomFactor);
		}
		else if(m_bYPressed || m_bZoomYOnly)
		{
			//zoom y scale
			m_QGraph.SetAutoY(false);
			m_QGraph.Scaley(1.0/zoomFactor);
		}
		else
		{
			//zoom both
			m_QGraph.SetAuto(false);
			m_QGraph.Scale(1.0/zoomFactor);
		}
		m_QGraph.SetAutoXUnit();
		m_QGraph.SetAutoYUnit();
	}
	else
	{
		double scale = m_fScale;

		m_fScale *= zoomFactor;


		int a = (int)((m_rCltRect.right() + m_rCltRect.left())/2);
		m_ptOffset.rx() = a + (int)((m_ptOffset.x()-a)*m_fScale/scale);
	}
	UpdateView();
}


