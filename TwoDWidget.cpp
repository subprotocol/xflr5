

#include <QtGui>
#include "MainFrame.h"
#include <math.h>
#include "Graph/QGraph.h"
#include "Graph/Curve.h"
#include "XDirect/XDirect.h"
#include "TwoDWidget.h"

TwoDWidget::TwoDWidget(QWidget *parent)
	: QWidget(parent)
{
	m_iView = XFOILANALYSIS;
	m_pXDirect = NULL;

	setMouseTracking(true);
}

TwoDWidget::~TwoDWidget()
{

}

void TwoDWidget::keyPressEvent(QKeyEvent *event)
{
	if(m_iView == XFOILANALYSIS && m_pXDirect)
	{
qDebug() << "TwoDWidget :: keypress event";
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->keyPressEvent(event);
	}
}

void TwoDWidget::mousePressEvent(QMouseEvent *event)
{
	if(m_iView == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->mousePressEvent(event);
	}
}


void TwoDWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if(m_iView == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->mouseReleaseEvent(event);
	}
}


void TwoDWidget::mouseMoveEvent(QMouseEvent *event)
{
	if(m_iView == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->mouseMoveEvent(event);
	}
}

void TwoDWidget::resizeEvent ( QResizeEvent * event )
{
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	pXDirect->m_rCltRect = rect();
	pXDirect->SetFoilScale();
}


void TwoDWidget::wheelEvent(QWheelEvent *event)
{
	if(m_iView == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->wheelEvent(event);
	}
}

void TwoDWidget::paintEvent(QPaintEvent *event)
{
	QXDirect* pXDirect = (QXDirect*)m_pXDirect;

	if(m_iView == XFOILANALYSIS && pXDirect)
	{
		QPainter painter(this);
//		painter.setBackgroundMode(Qt::TransparentMode);
		pXDirect->PaintView(painter);
	}
	else
	{
/*		QRect rect(30, 50, 600, 300);
		QGraph Graph;
		Graph.m_pParent = this;
		Graph.SetBkColor(QColor(75,12,0,127));
		Graph.SetAxisColor(QColor(20, 255,100));
		Graph.SetLabelColor(QColor(200,255,255));
		Graph.SetTitleColor(QColor(200,150,50));
		Graph.SetXTitle("abs");
		Graph.SetYTitle("ordonnee");
		Graph.SetMargin(20);
		CCurve *pCurve = Graph.AddCurve();
		for(int i=0; i<10; i++) pCurve->AddPoint((double)i/3000., sin(i)/50.);

		Graph.SetDrawRect(rect);
		Graph.DrawGraph();*/
	}
}




