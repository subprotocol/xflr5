

#include <QtGui>
#include "MainFrame.h"
#include <math.h>
#include "Graph/QGraph.h"
#include "Graph/Curve.h"
#include "Miarex/Miarex.h"
#include "XDirect/XDirect.h"
#include "TwoDWidget.h"

TwoDWidget::TwoDWidget(QWidget *parent)
	: QWidget(parent)
{
	m_pMainFrame = NULL;
	m_pXDirect   = NULL;
	m_pMiarex    = NULL;

	setMouseTracking(true);
	setCursor(Qt::CrossCursor);
}

TwoDWidget::~TwoDWidget()
{
}

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
}

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
}


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
}


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
}

void TwoDWidget::resizeEvent(QResizeEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QXDirect *pXDirect = (QXDirect*)m_pXDirect;
	QMiarex* pMiarex = (QMiarex*)m_pMiarex;
	if(pXDirect) pXDirect->m_rCltRect = rect();
	if(pMiarex)  pMiarex->m_rCltRect = rect();

	if(pMainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		pXDirect->SetFoilScale(rect());
	}
	else  if(pMainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		pMiarex->m_bIs2DScaleSet = false;
		pMiarex->SetScale(rect());
	}
}


void TwoDWidget::wheelEvent(QWheelEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(pMainFrame->m_iApp == XFOILANALYSIS && m_pXDirect)
	{
		QXDirect* pXDirect = (QXDirect*)m_pXDirect;
		pXDirect->wheelEvent(event);
	}
	else if(pMainFrame->m_iApp == MIAREX && m_pMiarex)
	{
		QMiarex* pMiarex = (QMiarex*)m_pMiarex;
		pMiarex->wheelEvent(event);
	}
}

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
	else
	{
		QPainter painter(this);
		painter.fillRect(rect(), pMainFrame->m_BackgroundColor);
	}
}




