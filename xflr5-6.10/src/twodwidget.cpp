/****************************************************************************

	TwoDWidget Class
	Copyright (C) 2009-2012 Andre Deperrois adeperrois@xflr5.com

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

#include <math.h>
#include <QMenu>
#include <QtDebug>
#include "mainframe.h"
#include "misc/Settings.h"
#include "graph/QGraph.h"
#include "graph/Curve.h"
#include "miarex/Miarex.h"
#include "xdirect/XDirect.h"
#include "design/AFoil.h"
#include "xinverse/XInverse.h"
#include "twodwidget.h"


/**
*The public constructor
*/
TwoDWidget::TwoDWidget(QWidget *parent)
	: QWidget(parent)
{
	m_pMainFrame = NULL;
	m_pXDirect   = NULL;
	m_pMiarex    = NULL;
	m_pAFoil     = NULL;

	setMouseTracking(true);
	setCursor(Qt::CrossCursor);
}


/**
*Overrides the keyPressEvent method of the base class.
*Dispatches the handling to the active child application.
*/
void TwoDWidget::keyPressEvent(QKeyEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(pMainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->keyPressEvent(event);
	}
	else if(pMainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->keyPressEvent(event);
	}
	else if(pMainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->keyPressEvent(event);
	}
	else if(pMainFrame->m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->keyPressEvent(event);
	}
}


/**
*Overrides the keyReleaseEvent method of the base class.
*Dispatches the handling to the active child application.
*/
void TwoDWidget::keyReleaseEvent(QKeyEvent *event)
{

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(pMainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->keyReleaseEvent(event);
	}
	else if(pMainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->keyReleaseEvent(event);
	}
	else if(pMainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->keyReleaseEvent(event);
	}
	else if(pMainFrame->m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->keyReleaseEvent(event);
	}
}



/**
*Overrides the mousePressEvent method of the base class.
*Dispatches the handling to the active child application.
*/
void TwoDWidget::mousePressEvent(QMouseEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(pMainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->mousePressEvent(event);
	}
	else if(pMainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->mousePressEvent(event);
	}
	else if(pMainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->mousePressEvent(event);
	}
	else if(pMainFrame->m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->mousePressEvent(event);
	}
}


/**
*Overrides the mouseReleaseEvent method of the base class.
*Dispatches the handling to the active child application.
*/
void TwoDWidget::mouseReleaseEvent(QMouseEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(pMainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->mouseReleaseEvent(event);
	}
	else if(pMainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->mouseReleaseEvent(event);
	}
	else if(pMainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->mouseReleaseEvent(event);
	}
	else if(pMainFrame->m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->mouseReleaseEvent(event);
	}
}


/**
*Overrides the mouseMoveEvent method of the base class.
*Dispatches the handling to the active child application.
*/
void TwoDWidget::mouseMoveEvent(QMouseEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(pMainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->mouseMoveEvent(event);
	}
	else if(pMainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->mouseMoveEvent(event);
	}
	else if(pMainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->mouseMoveEvent(event);
	}
	else if(pMainFrame->m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->mouseMoveEvent(event);
	}
}



/**
*Overrides the mouseDoubleClickEvent function of the base class.
*Dispatches the handling to the active child application.
*/
void TwoDWidget::mouseDoubleClickEvent ( QMouseEvent * event )
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	if(pMainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect *pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->doubleClickEvent(event->pos());
	}
	else if(pMainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->doubleClickEvent(event->pos());
	}
	else if(pMainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->doubleClickEvent(event->pos());
	}
	else if(pMainFrame->m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->doubleClickEvent(event->pos());
	}
}


/**
*Overrides the resizeEvent function of the base class.
*Dispatches the handling to the active child application.
*/
void TwoDWidget::resizeEvent(QResizeEvent *event)
{
//	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QXDirect  *pXDirect  = (QXDirect*)m_pXDirect;
	QMiarex   *pMiarex   = (QMiarex*)m_pMiarex;
	QAFoil    *pAFoil    = (QAFoil*)m_pAFoil;
	QXInverse *pXInverse = (QXInverse*)m_pXInverse;

	if(m_pXDirect)
	{
		pXDirect->SetFoilScale(rect());
		pXDirect->SetPolarLegendPos();
	}
	if(m_pMiarex)
	{
        pMiarex->m_bResetTextLegend = true;
        pMiarex->m_bIs2DScaleSet = false;
		pMiarex->Set2DScale();
		if(pMiarex->m_iView==WOPPVIEW)   pMiarex->SetWingLegendPos();
		if(pMiarex->m_iView==WPOLARVIEW) pMiarex->SetWPlrLegendPos();
	}
	if(m_pAFoil)
	{
		pAFoil->SetScale(rect());
	}
	if(m_pXInverse)
	{
		pXInverse->SetScale(rect());
	}
	event->accept();
}


/**
*Overrides the wheelEvent function of the base class.
*Dispatches the handling to the active child application.
*/
void TwoDWidget::wheelEvent(QWheelEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	static double ZoomFactor=1.0;

	QPoint pt(event->x(), event->y()); //client coordinates

	if(event->delta()>0)
	{
		if(!Settings::s_bReverseZoom) ZoomFactor = 1./1.06;
		else                           ZoomFactor = 1.06;
	}
	else
	{
		if(!Settings::s_bReverseZoom) ZoomFactor = 1.06;
		else                           ZoomFactor = 1./1.06;
	}

	if(pMainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->zoomEvent(pt, ZoomFactor);
	}
	else if(pMainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->zoomEvent(pt, ZoomFactor);
	}
	else if(pMainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		pAFoil->zoomEvent(pt, ZoomFactor);
	}
	else if(pMainFrame->m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		pXInverse->zoomEvent(pt, ZoomFactor);
	}
}


/**
*Overrides the paintEvent function of the base class.
*Dispatches the handling to the active child application.
*/
void TwoDWidget::paintEvent(QPaintEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(pMainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{	
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		QPainter painter(this);
		pXDirect->PaintView(painter);
	}
	else if(pMainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		QPainter painter(this);
		pMiarex->PaintView(painter);
	}
	else if(pMainFrame->m_iApp == DIRECTDESIGN && m_pAFoil)
	{
		QAFoil *pAFoil= (QAFoil*)m_pAFoil;
		QPainter painter(this);
		pAFoil->PaintView(painter);
	}
	else if(pMainFrame->m_iApp == INVERSEDESIGN && m_pXInverse)
	{
		QXInverse *pXInverse= (QXInverse*)m_pXInverse;
		QPainter painter(this);
		pXInverse->PaintView(painter);
	}
	else
	{
		QPainter painter(this);
		painter.fillRect(rect(), Settings::s_BackgroundColor);
	}
	event->accept();
}


/**
*Overrides the contextMenuEvent function of the base class.
*Dispatches the handling to the active child application.
*/
void TwoDWidget::contextMenuEvent (QContextMenuEvent * event)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QPoint ScreenPt = event->globalPos();
	QPoint CltPt = event->pos();

	switch(pMainFrame->m_iApp)
	{
		case MIAREX:
		{
			QMiarex *pMiarex = (QMiarex*)m_pMiarex;
			if(event->reason()==QContextMenuEvent::Keyboard)
			{
				ScreenPt.rx() = pMiarex->m_LastPoint.x()+pMainFrame->pos().x()+geometry().x();
				ScreenPt.ry() = pMiarex->m_LastPoint.y()+pMainFrame->pos().y()+geometry().y();
			}

			pMiarex->m_pCurGraph = pMiarex->GetGraph(CltPt);
			if(pMiarex->m_iView==WOPPVIEW)            pMainFrame->WOppCtxMenu->exec(ScreenPt);
			else if (pMiarex->m_iView==WPOLARVIEW)    pMainFrame->WPlrCtxMenu->exec(ScreenPt);
			else if (pMiarex->m_iView==WCPVIEW)       pMainFrame->WCpCtxMenu->exec(ScreenPt);
			else if (pMiarex->m_iView==STABTIMEVIEW)  pMainFrame->WTimeCtxMenu->exec(ScreenPt);
			else if (pMiarex->m_iView==STABPOLARVIEW) pMainFrame->WPlrCtxMenu->exec(ScreenPt);

			break;
		}
		case XFOILANALYSIS:
		{
			QXDirect *pXDirect = (QXDirect*)m_pXDirect;
			pXDirect->m_pCurGraph = pXDirect->GetGraph(CltPt);
			if(pXDirect->m_bPolarView) pMainFrame->OperPolarCtxMenu->exec(ScreenPt);
			else                   pMainFrame->OperFoilCtxMenu->exec(ScreenPt);
			break;
		}
		case DIRECTDESIGN:
		{
			pMainFrame->AFoilCtxMenu->exec(ScreenPt);
			break;
		}
		case INVERSEDESIGN:
		{
			pMainFrame->InverseContextMenu->exec(ScreenPt);
			break;
		}
		default:
			break;
	}
}
