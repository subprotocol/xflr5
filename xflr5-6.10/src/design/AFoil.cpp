/****************************************************************************

	AFoil Class
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

#include <QAction>
#include <QMenu>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QtDebug>


#include "AFoil.h"
#include "AFoilGridDlg.h"
#include "AFoilTableDlg.h"
#include "SplineCtrlsDlg.h"
#include "LECircleDlg.h"
#include "../globals.h"
#include "../mainframe.h"
#include "../misc/Settings.h"
#include "../misc/LinePickerDlg.h"
#include "../misc/RenameDlg.h"
#include "../xdirect/analysis/XFoil.h"
#include "../xdirect/geometry/NacaFoilDlg.h"
#include "../xdirect/geometry/CAddDlg.h"
#include "../xdirect/geometry/TwoDPanelDlg.h"
#include "../xdirect/geometry/TEGapDlg.h"
#include "../xdirect/geometry/LEDlg.h"
#include "../xdirect/geometry/FlapDlg.h"
#include "../xdirect/geometry/FoilCoordDlg.h"
#include "../xdirect/geometry/FoilGeomDlg.h"
#include "../xdirect/geometry/InterpolateFoilsDlg.h"



void *QAFoil::s_pMainFrame = NULL;
void *QAFoil::s_p2DWidget = NULL;

/**
 * The public constructor
 * @param parent a pointer to the MainFrame window
 */
QAFoil::QAFoil(QWidget *parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	m_hcCross = QCursor(Qt::CrossCursor);
	m_hcMove  = QCursor(Qt::ClosedHandCursor);

	m_StackPos = 0;

	m_MousePos.x = 0.0;
	m_MousePos.y = 0.0;

	m_poaFoil  = NULL;
	m_pctrlFoilTable = NULL;
	m_precision = NULL;

	m_pSF = new SplineFoil();
	m_pSF->m_bModified = false;
	m_pSF->InitSplineFoil();

	MainFrame *pMainFrame =(MainFrame*)parent;
	pMainFrame->UndoAFoilAct = pMainFrame->RedoAFoilAct = NULL;

	ClearStack();
	TakePicture();

	m_bZoomPlus    = false;
	m_bZoomYOnly   = false;
	m_bTrans       = false;
	m_bNeutralLine = true;
	m_bScale       = true;
	m_bLECircle      = false;
	m_bShowLegend  = true;
	m_bStored      = false;
	m_bXDown = m_bYDown = m_bZDown = false;
	m_bIsImageLoaded = false;

	m_LERad   = 1.0;

	m_bXGrid     = false;
	m_XGridUnit  = 0.05;
	m_XGridStyle = 1;
	m_XGridWidth = 1;
	m_XGridColor = QColor(150,150,150);

	m_bYGrid     = false;
	m_YGridUnit  = 0.05;
	m_YGridStyle = 1;
	m_YGridWidth = 1;
	m_YGridColor = QColor(150,150,150);

	m_bXMinGrid  = false;
	m_XMinUnit = 0.01;
	m_XMinStyle  = 2;
	m_XMinWidth  = 1;
	m_XMinColor  = QColor(70,70,70);

	m_bYMinGrid  = false;
	m_YMinUnit = 0.01;
	m_YMinStyle  = 2;
	m_YMinWidth  = 1;
	m_YMinColor  = QColor(70,70,70);

	m_NeutralStyle = 3;
	m_NeutralWidth = 1;
	m_NeutralColor = QColor(70,70,70);

	m_fScale    = 1.0;
	m_fRefScale = 1.0;

	m_fScaleY    = 1.0;
	m_ptOffset.rx() = 0;
	m_ptOffset.ry() = 0;

	m_ViewportTrans = QPoint(0,0);

	m_pBufferFoil = new Foil();

	m_StackPos = 0;

	SetupLayout();

	SplineCtrlsDlg::s_pAFoil    = this;
}


/**
 * The public destructor
 */
QAFoil::~QAFoil()
{
	Trace("Destroying AFoil");

	ClearStack(-1);
	if(m_pSF) delete m_pSF;
	if(m_pBufferFoil) delete m_pBufferFoil;
	if(m_precision) delete [] m_precision;
}


/**
 * Initializes the state of the button widgets and of the QAction objects.
 */
void QAFoil::SetControls()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	pMainFrame->AFoilDelete->setEnabled(Foil::curFoil());
	pMainFrame->AFoilRename->setEnabled(Foil::curFoil());
	pMainFrame->AFoilExport->setEnabled(Foil::curFoil());
	pMainFrame->pAFoilDuplicateFoil->setEnabled(Foil::curFoil());
	pMainFrame->ShowCurrentFoil->setEnabled(Foil::curFoil());
	pMainFrame->HideCurrentFoil->setEnabled(Foil::curFoil());

	pMainFrame->AFoilDerotateFoil->setEnabled(Foil::curFoil());
	pMainFrame->AFoilEditCoordsFoil->setEnabled(Foil::curFoil());
	pMainFrame->AFoilInterpolateFoils->setEnabled(Foil::curFoil());
	pMainFrame->AFoilNormalizeFoil->setEnabled(Foil::curFoil());
	pMainFrame->AFoilRefineGlobalFoil->setEnabled(Foil::curFoil());
	pMainFrame->AFoilRefineLocalFoil->setEnabled(Foil::curFoil());
	pMainFrame->AFoilScaleFoil->setEnabled(Foil::curFoil());
	pMainFrame->AFoilSetFlap->setEnabled(Foil::curFoil());
	pMainFrame->AFoilSetLERadius->setEnabled(Foil::curFoil());
	pMainFrame->AFoilSetTEGap->setEnabled(Foil::curFoil());

	pMainFrame->m_pShowLegend->setChecked(m_bShowLegend);

	pMainFrame->AFoilSplineMenu->setEnabled(!Foil::curFoil());
	pMainFrame->InsertSplinePt->setEnabled(!Foil::curFoil());
	pMainFrame->RemoveSplinePt->setEnabled(!Foil::curFoil());

	pMainFrame->UndoAFoilAct->setEnabled(m_StackPos>0);
	pMainFrame->RedoAFoilAct->setEnabled(m_StackPos<m_UndoStack.size()-1);
	pMainFrame->statusBar()->clearMessage();
}


/**
 * Draws the scale on the neutral line.
 * @param painter a reference to the QPainter object with which to draw
 * @param scalex
 */
void QAFoil::DrawScale(QPainter &painter, double scalex)
{
	int i;
	painter.save();

	painter.setFont(Settings::s_TextFont);

	QFontMetrics fm(Settings::s_TextFont);
	int dD = fm.height();
	int dW = fm.width("0.1");

	int TickSize, offy;

	TickSize = (int)(dD/2);
	offy = m_ptOffset.y();

	QPen TextPen(Settings::s_TextColor);
	painter.setPen(TextPen);

	double xo = 0.0;
	double xmin = 0.0;
	double xmax = 1.0;
//	double ymin = -0.2;
//	double ymax =  0.2;
	double XGridUnit = 0.1;
	double XHalfGridUnit = 0.05;
	double XMinGridUnit = 0.01;

	double xt  = xo-int((xo-xmin)*1.0001/XGridUnit)*XGridUnit;//one tick at the origin
	double xht = xo-int((xo-xmin)*1.0001/XHalfGridUnit)*XHalfGridUnit;//one tick at the origin
	double xmt = xo-int((xo-xmin)*1.0001/XMinGridUnit)*XMinGridUnit;//one tick at the origin

	painter.drawLine(int(xt*scalex) + m_ptOffset.x(), offy, int(xmax*scalex) + m_ptOffset.x(), offy);

	QString strLabel;

	while(xt<=xmax*1.001)
	{
		//Draw  ticks
		painter.drawLine(int(xt*scalex) + m_ptOffset.x(), offy, int(xt*scalex) + m_ptOffset.x(), offy+TickSize*2);
		strLabel = QString("%1").arg(xt,4,'f',1);
		painter.drawText(int(xt*scalex)+m_ptOffset.x()-dW/2, offy+dD*2, strLabel);
		xt += XGridUnit ;
	}

//	while(xht<=xmax*1.001)
	xht = 0;
	for(i=0;i<1/XHalfGridUnit;i++)
	{
		if(i%2!=0) painter.drawLine(int(xht*scalex) + m_ptOffset.x(), offy, int(xht*scalex) + m_ptOffset.x(), offy+TickSize*2);
		xht += XHalfGridUnit ;
	}

	xmt=0;
//	while(xmt<=xmax*1.001)
	for(i=0;i<1/XMinGridUnit;i++)
	{
		if(i%5!=0) painter.drawLine(int(xmt*scalex) + m_ptOffset.x(), offy,int(xmt*scalex) + m_ptOffset.x(), offy+TickSize);
		xmt += XMinGridUnit ;
	}

	painter.restore();
}


/**
 * Draws the X main grid.
 * @param painter a reference to the QPainter object with which to draw
 * @param scalex the scale factor in the x-direction
 * @param scaley the scale factor in the y-direction
 * @param Offset the Foil leading edge offset in the client area
 * @param dRect the drawing rectangle
 */
void QAFoil::DrawXGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect)
{
	painter.save();
	QPen GridPen(m_XGridColor);
	GridPen.setStyle(getStyle(m_XGridStyle));
	GridPen.setWidth(m_XGridWidth);
	painter.setPen(GridPen);

	double xo   =  0.0;
	double xmin =  0.0;
	double xmax =  1.0;
	double ymin = -0.2;
	double ymax =  0.2;

	int YMin = qMax(int(ymin*scaley)+ Offset.y(), dRect.top());
	int YMax = qMin(int(ymax*scaley)+ Offset.y(), dRect.bottom());

	double xt = xo-int((xo-xmin)*1.0001/m_XGridUnit)*m_XGridUnit;//one tick at the origin

	while(xt<=xmax*1.001)
	{
		//Draw  grid
		painter.drawLine(int(xt*scalex) + Offset.x(), YMin, int(xt*scalex) + Offset.x(), YMax);
		xt += m_XGridUnit ;
	}

	painter.restore();
}


/**
 * Draws the Y main grid.
 * @param painter a reference to the QPainter object with which to draw
 * @param scalex the scale factor in the x-direction
 * @param scaley the scale factor in the y-direction
 * @param Offset the Foil leading edge offset in the client area
 * @param dRect the drawing rectangle
 */
void QAFoil::DrawYGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect)
{
	painter.save();
	QPen GridPen(m_YGridColor);
	GridPen.setStyle(getStyle(m_YGridStyle));
	GridPen.setWidth(m_YGridWidth);
	painter.setPen(GridPen);

	double yo = 0.0;
	double ymin = -0.2;
	double ymax = 0.2;
	double xmin = 0.0;
	double xmax = 1.0;

	int XMin = max(int(xmin*scalex)+ Offset.x(), dRect.left());
	int XMax = min(int(xmax*scalex)+ Offset.x(), dRect.right());

	double yt = yo-int((yo-ymin)*1.0001/m_YGridUnit)*m_YGridUnit;//one tick at the origin

	while(yt<=ymax*1.0001)
	{

		painter.drawLine(XMin, (int)(yt*scaley) + Offset.y(), XMax, (int)(yt*scaley) + Offset.y());

		yt += m_YGridUnit ;
	}

	painter.restore();
}



/**
 * Draws the X minor grid.
 * @param painter a reference to the QPainter object with which to draw
 * @param scalex the scale factor in the x-direction
 * @param scaley the scale factor in the y-direction
 * @param Offset the Foil leading edge offset in the client area
 * @param dRect the drawing rectangle
 */
void QAFoil::DrawXMinGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect)
{
	painter.save();

	QPen GridPen(m_XMinColor);
	GridPen.setWidth(m_XMinWidth);
	GridPen.setStyle(getStyle(m_XMinStyle));

	painter.setPen(GridPen);


	double xo = 0.0;
	double xmin = 0.0;
	double xmax = 1.0;
	double ymin = -0.2;
	double ymax =  0.2;

	int YMin = qMax(int(ymin*scaley)+ Offset.y(), dRect.top());
	int YMax = qMin(int(ymax*scaley)+ Offset.y(), dRect.bottom());

	double xDelta = m_XMinUnit;
	int MinFreq = (int)(m_XGridUnit/m_XMinUnit);
	int k=0;
	double xt = xo-int((xo-xmin)*1.0001/m_XGridUnit)*m_XGridUnit;//one tick at the origin

	while(xt<=xmax*1.001)
	{
		if(k%(MinFreq)!=0)
		{
			// do not overwrite main grid
			if (xt>=xmin)
			{
				painter.drawLine(int(xt*scalex) + Offset.x(), YMin, int(xt*scalex) + Offset.x(), YMax);
			}
		}
		xt += xDelta;
		k++;
	}

	painter.restore();
}



/**
 * Draws the Y minor grid.
 * @param painter a reference to the QPainter object with which to draw
 * @param scalex the scale factor in the x-direction
 * @param scaley the scale factor in the y-direction
 * @param Offset the Foil leading edge offset in the client area
 * @param dRect the drawing rectangle
 */
void QAFoil::DrawYMinGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect)
{
	painter.save();

	QPen GridPen(m_YMinColor);
	GridPen.setWidth(m_YMinWidth);
	GridPen.setStyle(getStyle(m_YMinStyle));

	painter.setPen(GridPen);

	double yo = 0.0;
	double xmin = 0.0;
	double xmax = 1.0;
	double ymin = -0.2;
	double ymax =  0.2;

	int XMin = qMax(int(xmin*scalex)+ Offset.x(), dRect.left());
	int XMax = qMin(int(xmax*scalex)+ Offset.x(), dRect.right());

//	double yDelta = m_YGridUnit/(m_YMinFreq+1);
	double yDelta = m_YMinUnit;
	int MinFreq = (int)(m_YGridUnit/m_YMinUnit);
	int k=0;
	double yt = yo-int((yo-ymin)*1.0001/m_YGridUnit)*m_YGridUnit;//one tick at the origin

	while(yt<=ymax*1.001)
	{
		if(k%(MinFreq)!=0)
		{
		// do not overwrite main grid
			if (yt>=ymin)
			{
				painter.drawLine(XMin, (int)(yt*scaley) + Offset.y(), XMax, (int)(yt*scaley) + Offset.y());
			}
		}
		yt += yDelta;
		k++;
	}

	painter.restore();
}


/**
 * Fills the table with the data from the Foil objects.
 */
void QAFoil::FillFoilTable()
{
	int i;
	m_pFoilModel->setRowCount(m_poaFoil->size()+1);

	QString name;
	QModelIndex ind;

	double Thickness, xThickness, Camber, xCamber;
	Thickness = xThickness = Camber = xCamber = 0;
	int points = 0;

	if(m_pSF)
	{
		name = tr("Spline foil");
		Thickness  = m_pSF->m_fThickness;
		xThickness = m_pSF->m_fxThickMax;
		Camber     = m_pSF->m_fCamber;
		xCamber    = m_pSF->m_fxCambMax;
		points     = m_pSF->m_OutPoints;
	}

	ind = m_pFoilModel->index(0, 0, QModelIndex());
	m_pFoilModel->setData(ind,name);

	ind = m_pFoilModel->index(0, 1, QModelIndex());
	m_pFoilModel->setData(ind, Thickness);

	ind = m_pFoilModel->index(0, 2, QModelIndex());
	m_pFoilModel->setData(ind, xThickness);

	ind = m_pFoilModel->index(0, 3, QModelIndex());
	m_pFoilModel->setData(ind, Camber);

	ind = m_pFoilModel->index(0, 4, QModelIndex());
	m_pFoilModel->setData(ind,xCamber);

	ind = m_pFoilModel->index(0, 5, QModelIndex());
	m_pFoilModel->setData(ind, points);

/*	ind = m_pFoilModel->index(0,12, QModelIndex());
	if(m_pSF->m_bVisible) m_pFoilModel->setData(ind, Qt::Checked, Qt::CheckStateRole);
	else                  m_pFoilModel->setData(ind, Qt::Unchecked, Qt::CheckStateRole);

	QStandardItem *pItem = m_pFoilModel->item(0,12);
	pItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);


	ind = m_pFoilModel->index(0,13, QModelIndex());
	if(m_pSF->m_bOutPoints) m_pFoilModel->setData(ind, Qt::Checked, Qt::CheckStateRole);
	else                    m_pFoilModel->setData(ind, Qt::Unchecked, Qt::CheckStateRole);

	pItem = m_pFoilModel->item(0,13);
	pItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);


	ind = m_pFoilModel->index(0,14, QModelIndex());
	if(m_pSF->m_bCenterLine) m_pFoilModel->setData(ind, Qt::Checked,   Qt::CheckStateRole);
	else                     m_pFoilModel->setData(ind, Qt::Unchecked, Qt::CheckStateRole);

	pItem = m_pFoilModel->item(0,14);
	pItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
*/

	for(i=0; i<m_poaFoil->size(); i++)
	{
		FillTableRow(i+1);
	}
}


/**
 * Fills the data from a Foil object in the specified table row.
 * @param row the index of the row to be filled
 */
void QAFoil::FillTableRow(int row)
{
	QModelIndex ind;

	Foil *pFoil = (Foil*)m_poaFoil->at(row-1);

	ind = m_pFoilModel->index(row, 0, QModelIndex());
	m_pFoilModel->setData(ind,pFoil->m_FoilName);

	ind = m_pFoilModel->index(row, 1, QModelIndex());
	m_pFoilModel->setData(ind, pFoil->m_fThickness);

	ind = m_pFoilModel->index(row, 2, QModelIndex());
	m_pFoilModel->setData(ind, pFoil->m_fXThickness);

	ind = m_pFoilModel->index(row, 3, QModelIndex());
	m_pFoilModel->setData(ind, pFoil->m_fCamber);

	ind = m_pFoilModel->index(row, 4, QModelIndex());
	m_pFoilModel->setData(ind,pFoil->m_fXCamber);

	ind = m_pFoilModel->index(row, 5, QModelIndex());
	m_pFoilModel->setData(ind,pFoil->n);


	if(pFoil && pFoil->m_bTEFlap)
	{
		ind = m_pFoilModel->index(row, 6, QModelIndex());
		m_pFoilModel->setData(ind,pFoil->m_TEFlapAngle);

		ind = m_pFoilModel->index(row, 7, QModelIndex());
		m_pFoilModel->setData(ind,pFoil->m_TEXHinge/100.0);

		ind = m_pFoilModel->index(row, 8, QModelIndex());
		m_pFoilModel->setData(ind,pFoil->m_TEYHinge/100.0);

	}
	if(pFoil && pFoil->m_bLEFlap)
	{
		ind = m_pFoilModel->index(row, 9, QModelIndex());
		m_pFoilModel->setData(ind,pFoil->m_LEFlapAngle);

		ind = m_pFoilModel->index(row, 10, QModelIndex());
		m_pFoilModel->setData(ind,pFoil->m_LEXHinge/100.0);

		ind = m_pFoilModel->index(row, 11, QModelIndex());
		m_pFoilModel->setData(ind,pFoil->m_LEYHinge/100.0);
	}

/*	ind = m_pFoilModel->index(row, 12, QModelIndex());
	if(pFoil->m_bVisible) m_pFoilModel->setData(ind, Qt::Checked, Qt::CheckStateRole);
	else                  m_pFoilModel->setData(ind, Qt::Unchecked, Qt::CheckStateRole);
	pItem = m_pFoilModel->item(row,12);
	if(pItem) pItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	
	ind = m_pFoilModel->index(row, 13, QModelIndex());
	if(pFoil->m_bPoints) m_pFoilModel->setData(ind, Qt::Checked, Qt::CheckStateRole);
	else                 m_pFoilModel->setData(ind, Qt::Unchecked, Qt::CheckStateRole);
	pItem = m_pFoilModel->item(row,13);
	if(pItem) pItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);

	ind = m_pFoilModel->index(row, 14, QModelIndex());
	if(pFoil->m_bCenterLine) m_pFoilModel->setData(ind, Qt::Checked, Qt::CheckStateRole);
	else                     m_pFoilModel->setData(ind, Qt::Unchecked, Qt::CheckStateRole);
	pItem = m_pFoilModel->item(row,14);
	if(pItem) pItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);*/
}


/**
 * Overrides the QWidget's keyPressEvent method.
 * Dispatches the key press event
 * @param event the QKeyEvent
 */
void QAFoil::keyPressEvent(QKeyEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	bool bShift = false;
	if(event->modifiers() & Qt::ShiftModifier)   bShift =true;

	switch (event->key())
	{
		case Qt::Key_Escape:
		{
			if(m_bZoomPlus)
			{
				ReleaseZoom();
			}
			else if(m_bZoomYOnly)
			{
				pMainFrame->zoomYAct->setChecked(false);
				m_bZoomYOnly = false;
			}
			break;
		}
		case Qt::Key_F2:
		{
			OnRenameFoil();
			break;
		}
		case Qt::Key_F3:
		{
			if(Foil::curFoil())
			{
				if(bShift) OnAFoilCadd();
				else       OnAFoilPanels();
			}
			break;
		}
		case Qt::Key_F4:
		{
			OnStoreSplines();
			break;
		}
		case Qt::Key_F9:
		{
			OnAFoilFoilGeom();
			break;
		}
		case Qt::Key_F10:
		{
			OnAFoilSetFlap();
			break;
		}
		case Qt::Key_F11:
		{
			OnAFoilInterpolateFoils();
			break;
		}
		case Qt::Key_R:
			OnResetScales();
			break;
		case Qt::Key_X:
			m_bXDown = true;
			break;
		case Qt::Key_Y:
			m_bYDown = true;
			break;
		case Qt::Key_Z:
			m_bZDown = true;
			break;	
		case Qt::Key_I:
			if (event->modifiers().testFlag(Qt::ControlModifier) & event->modifiers().testFlag(Qt::ShiftModifier))
			{
				if(!m_bIsImageLoaded)
				{
					OnLoadBackImage();
				}
				else
				{
					OnClearBackImage();
				}
			}
			break;
		default:
			QWidget::keyPressEvent(event);
	}
}


/**
 * Overrides the QWidget's keyReleaseEvent method.
 * Dispatches the key press event
 * @param event the QKeyEvent
 */
void QAFoil::keyReleaseEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_X:
		{
			if(!event->isAutoRepeat()) m_bXDown = false;
			break;
		}
		case Qt::Key_Y:
			if(!event->isAutoRepeat()) m_bYDown = false;
			break;
		case Qt::Key_Z:
			if(!event->isAutoRepeat()) m_bZDown = false;
			break;	
		default:
			QWidget::keyReleaseEvent(event);
	}
}

/**
 * Converts screen coordinate to viewport coordinates
 * @param point the screen coordinates
 * @return the viewport coordinates
 */
CVector QAFoil::MousetoReal(QPoint &point)
{
	CVector Real;
	
	Real.x =  (point.x() - m_ptOffset.x())/m_fScale;
	Real.y = -(point.y() - m_ptOffset.y())/m_fScale/m_fScaleY;
	Real.z = 0.0;
	
	return Real;
}


/**
 * Loads the user's default settings from the application QSettings object.
 * @param pSettings a pointer to the QSettings object
 */
void QAFoil::LoadSettings(QSettings *pSettings)
{
	int style, width;
	QColor color;

	pSettings->beginGroup("DirectDesign");
	{
		m_bXGrid      = pSettings->value("XMajGrid").toBool();
		m_bYGrid      = pSettings->value("YMajGrid").toBool();
		m_bXMinGrid   = pSettings->value("XMinGrid").toBool();
		m_bYMinGrid   = pSettings->value("YMinGrid").toBool();
		m_XGridStyle  = pSettings->value("XMajStyle").toInt();
		m_YGridStyle  = pSettings->value("YMajStyle").toInt();
		m_XGridWidth  = pSettings->value("XMajWidth").toInt();
		m_YGridWidth  = pSettings->value("YMajWidth").toInt();
		m_XMinStyle   = pSettings->value("XMinStyle").toInt();
		m_YMinStyle   = pSettings->value("YMinStyle").toInt();
		m_XMinWidth   = pSettings->value("XMinWidth").toInt();
		m_YMinWidth   = pSettings->value("YMinWidth").toInt();
		m_XGridUnit   = pSettings->value("XMajUnit").toDouble();
		m_YGridUnit   = pSettings->value("YMajUnit").toDouble();
		m_XMinUnit    = pSettings->value("XMinUnit").toDouble();
		m_YMinUnit    = pSettings->value("YMinUnit").toDouble();
		m_XGridColor  = pSettings->value("XMajColor", QColor(101,101,101)).value<QColor>();
		m_YGridColor  = pSettings->value("YMajColor", QColor(101,101,101)).value<QColor>();
		m_XMinColor   = pSettings->value("XMinColor", QColor(191,191,191)).value<QColor>();
		m_YMinColor   = pSettings->value("YMinColor", QColor(191,191,191)).value<QColor>();

		m_NeutralStyle = pSettings->value("NeutralStyle").toInt();
		m_NeutralWidth = pSettings->value("NeutralWidth").toInt();
		m_NeutralColor = pSettings->value("NeutralColor", QColor(131,131,131)).value<QColor>();
		m_bNeutralLine = pSettings->value("NeutralLine").toBool();
		
		style  = pSettings->value("SFStyle", 0).toInt();
		width  = pSettings->value("SFWidth", 1).toInt();
		color  = pSettings->value("SFColor",QColor(216,183,83)).value<QColor>();
		m_pSF->SetCurveParams(style, width, color);


		m_pSF->m_bVisible    = pSettings->value("SFVisible").toBool();
		m_pSF->m_bOutPoints  = pSettings->value("SFOutPoints").toBool();
		m_pSF->m_bCenterLine = pSettings->value("SFCenterLine").toBool();

		m_pSF->m_Intrados.m_iRes =  pSettings->value("LowerRes",79).toInt();
		m_pSF->m_Extrados.m_iRes =  pSettings->value("UpperRes",79).toInt();
		m_pSF->m_Extrados.SplineCurve();
		m_pSF->m_Intrados.SplineCurve();


		m_bLECircle          = pSettings->value("LECircle").toBool();
		m_bScale             = pSettings->value("Scale").toBool();
		m_bShowLegend        = pSettings->value("Legend").toBool();

		QString str;
		for(int i=0; i<16; i++)
		{
			str = QString("Column_%1").arg(i);
			m_pctrlFoilTable->setColumnWidth(i, pSettings->value(str,40).toInt());
			if(pSettings->value(str+"_hidden", false).toBool()) m_pctrlFoilTable->hideColumn(i);
		}
	}
	pSettings->endGroup();
}


/**
 * Overrides the QWidget's mouseDoubleClickEvent method.
 * Dispatches the key press event
 * @param event the QMouseEvent
 */
void QAFoil::doubleClickEvent(QPoint pos)
{
	if(!hasFocus()) setFocus();

	QPoint center;
	//translate
	center.rx() = (int)(m_rCltRect.width()/2);
	center.ry() = (int)(m_rCltRect.height()/2);

	m_ptOffset.rx() += -pos.x() + center.x();
	m_ptOffset.ry() += -pos.y() + center.y();
	m_ViewportTrans.rx() += -pos.x() + center.x();
	m_ViewportTrans.ry() += -pos.y() + center.y();

	UpdateView();
	return;
}


/**
 * Overrides the QWidget's mouseMoveEvent method.
 * Dispatches the key press event
 * @param event the QMouseEvent
 */
void QAFoil::mouseMoveEvent(QMouseEvent *event)
{
	if(!hasFocus()) setFocus();
	QPoint point = event->pos();
	m_MousePos = MousetoReal(point);
	CVector Real = m_MousePos;

	if(m_bZoomPlus && (event->buttons() & Qt::LeftButton))
	{
		// we're zooming in using the rectangle method
		m_ZoomRect.setBottomRight(point);
		UpdateView();
		return;
	}
	else if(m_rCltRect.contains(point) && (event->buttons() & Qt::LeftButton) && m_bTrans)
	{
		//translate
		m_ptOffset.rx() += point.x() - m_PointDown.x();
		m_ptOffset.ry() += point.y() - m_PointDown.y();
		m_ViewportTrans.rx() += point.x() - m_PointDown.x();
		m_ViewportTrans.ry() += point.y() - m_PointDown.y();

		m_PointDown.rx() = point.x();
		m_PointDown.ry() = point.y();
		m_MousePos = Real;


		UpdateView();
		return;
	}

	if (event->buttons() & Qt::LeftButton && !m_bZoomPlus)
	{
		// user is dragging the point
		if(m_rCltRect.contains(point))
		{
			int n = m_pSF->m_Extrados.m_iSelect;
			if (n>=0 && n<m_pSF->m_Extrados.m_CtrlPoint.size())
			{
//				if(n==1) m_MousePos.x = 0.0;// we can't move point 1 for vertical slope
				m_pSF->m_Extrados.m_CtrlPoint[n].x = m_MousePos.x;
				m_pSF->m_Extrados.m_CtrlPoint[n].y = m_MousePos.y;
				m_pSF->m_Extrados.SplineCurve();
				m_pSF->UpdateSplineFoil();
				if(m_pSF->m_bSymetric)
				{
					m_pSF->m_Intrados.m_CtrlPoint[n].x = m_MousePos.x;
					m_pSF->m_Intrados.m_CtrlPoint[n].y = -m_MousePos.y;
					m_pSF->m_Intrados.SplineCurve();
					m_pSF->UpdateSplineFoil();
				}
				m_pSF->m_bModified = true;
				emit projectModified();
			}
			else
			{
				int n = m_pSF->m_Intrados.m_iSelect;
				if (n>=0 && n<m_pSF->m_Intrados.m_CtrlPoint.size())
				{
					m_pSF->m_Intrados.m_CtrlPoint[n].x = m_MousePos.x;
					m_pSF->m_Intrados.m_CtrlPoint[n].y = m_MousePos.y;
					m_pSF->m_Intrados.SplineCurve();
					m_pSF->UpdateSplineFoil();

					if(m_pSF->m_bSymetric)
					{
						m_pSF->m_Extrados.m_CtrlPoint[n].x =  m_MousePos.x;
						m_pSF->m_Extrados.m_CtrlPoint[n].y = -m_MousePos.y;
						m_pSF->m_Extrados.SplineCurve();
						m_pSF->UpdateSplineFoil();
					}
					m_pSF->m_bModified = true;
					emit projectModified();
				}
			}

			FillFoilTable();
		}
	}

	else if ((event->buttons() & Qt::MidButton))
	{
		// user is zooming with mouse button down rather than with wheel
		if(m_rCltRect.contains(point))
		{		
			double scale = m_fScale;
			
			if(!m_bZoomYOnly)
			{
				if (m_bXDown)
				{
					if(point.y()-m_PointDown.y()>0)
					{
						m_fScale  *= 1.02;
						m_fScaleY /= 1.02;
					}
					else 
					{
						m_fScale  /= 1.02;
						m_fScaleY *= 1.02;
					}
				}
				else if (m_bYDown)
				{
					if(point.y()-m_PointDown.y()>0) m_fScaleY *= 1.02;
					else                            m_fScaleY /= 1.02;
				}
				else
				{
					if(point.y()-m_PointDown.y()>0) m_fScale *= 1.02;
					else		                    m_fScale /= 1.02;
				}

			}
			else
			{
				if(point.y()-m_PointDown.y()>0) m_fScaleY *= 1.02;
				else                            m_fScaleY /= 1.02;
			}

			m_PointDown = point;

			int a = (int)((m_rCltRect.right()+m_rCltRect.left())/2);
			m_ptOffset.rx() = a + (int)((m_ptOffset.x()-a)*m_fScale/scale);
		}
	}
	else if(!m_bZoomPlus)
	{
		//not zooming, check if mouse passes over control point and highlight

		if(m_pSF->m_bVisible)
		{
			int n = m_pSF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale);
			if (n>0 && n<m_pSF->m_Extrados.m_CtrlPoint.size())
			{
				m_pSF->m_Extrados.m_iHighlight = n;
			}
			else
			{
				if(m_pSF->m_Extrados.m_iHighlight>=0)
				{
					m_pSF->m_Extrados.m_iHighlight = -10;
				}
			}
			n = m_pSF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale);
			if (n>0 && n<m_pSF->m_Intrados.m_CtrlPoint.size())
			{
				m_pSF->m_Intrados.m_iHighlight = n;
			}
			else
			{
				if(m_pSF->m_Intrados.m_iHighlight>=0)
				{
					m_pSF->m_Intrados.m_iHighlight = -10;
				}
			}
			UpdateView();
		}
	}
	UpdateView();
}



/**
 * Overrides the QWidget's mousePressEvent method.
 * Dispatches the key press event
 * @param event the QMouseEvent
 */
void QAFoil::mousePressEvent(QMouseEvent *event)
{
	QPoint point = event->pos();
	TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;
	CVector Real = MousetoReal(point);

	// get a reference for mouse movements 
	m_PointDown.rx() = point.x();
	m_PointDown.ry() = point.y();

	if(m_bZoomPlus && m_rCltRect.contains(point))
	{
		m_ZoomRect.setTopLeft(point);
		m_ZoomRect.setBottomRight(point);
	}
	else if(!m_bZoomPlus && (event->buttons() & Qt::LeftButton))
	{
		if (event->modifiers() & Qt::ShiftModifier)
		{
			//shift --> inserts a point
			OnInsertCtrlPt();
		}
		else if (event->modifiers() & Qt::ControlModifier)
		{
			//Ctrl --> removes the point
			OnRemoveCtrlPt();
		}
		else
		{
			//Selects the point
			m_pSF->m_Extrados.m_iSelect = m_pSF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale);
			if(m_pSF->m_Extrados.m_iSelect<0) m_pSF->m_Intrados.m_iSelect = m_pSF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale);

			if(m_pSF->m_Extrados.m_iSelect ==-10 && m_pSF->m_Intrados.m_iSelect ==-10)
			{
				p2DWidget->setCursor(m_hcMove);
				m_bTrans = true;
			}
		}
	}
	UpdateView();	
}


/**
 * Overrides the QWidget's mouseReleaseEvent method.
 * Dispatches the key press event
 * @param event the QMouseEvent
 */
void QAFoil::mouseReleaseEvent(QMouseEvent *event)
{
	QPoint point = event->pos();

	TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;

	if(m_bZoomPlus && m_rCltRect.contains(point))
	{
		m_ZoomRect.setBottomRight(point);
		QRect ZRect = m_ZoomRect.normalized();
	
		if(!ZRect.isEmpty())
		{
			m_ZoomRect = ZRect;

			double ZoomFactor = qMin((double)m_rCltRect.width()  / (double)m_ZoomRect.width() , 
			                         (double)m_rCltRect.height() / (double)m_ZoomRect.height());

			double newScale = qMin(ZoomFactor*m_fScale, 32.0*m_fRefScale);

			ZoomFactor = qMin(ZoomFactor, newScale/m_fScale);

			m_fScale = ZoomFactor*m_fScale;
			int a = (int)((m_rCltRect.right() + m_rCltRect.left())/2);
			int b = (int)((m_rCltRect.top()   + m_rCltRect.bottom())/2);

			int aZoom = (int)((m_ZoomRect.right() + m_ZoomRect.left())/2);
			int bZoom = (int)((m_ZoomRect.top()   + m_ZoomRect.bottom())/2);

			//translate view
			m_ptOffset.rx() += (a - aZoom);
			m_ptOffset.ry() += (b - bZoom);
			//scale view
			m_ptOffset.rx() = (int)(ZoomFactor * (m_ptOffset.x()-a)+a);
			m_ptOffset.ry() = (int)(ZoomFactor * (m_ptOffset.y()-b)+b);

//			m_ZoomRect.setBottomRight(m_ZoomRect.topLeft());
			m_ZoomRect.setRight(m_ZoomRect.left()-1);
		}
		else 
		{
			m_ZoomRect.setBottomRight(m_ZoomRect.topLeft());
			ReleaseZoom();
		}
	}
	else if(m_bZoomPlus && !m_rCltRect.contains(point))
	{
		ReleaseZoom();
	}
	else if(m_bTrans)
	{
		// nothing to do
	}
	else 
	{
		//we're releasing a point drag
	   if(event->button()==Qt::LeftButton)
		{
			TakePicture();
			m_pSF->CompMidLine();
		}
	}

	p2DWidget->setCursor(m_hcCross);
	m_bTrans = false;
	UpdateView();
}


/**
 * The user has requested that the foil be derotated.
 */
void QAFoil::OnAFoilDerotateFoil()
{
	if(!Foil::curFoil()) return;

	m_pBufferFoil->CopyFoil(Foil::curFoil());
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = Foil::curFoil()->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_FoilStyle = 1;
	m_pBufferFoil->m_FoilWidth = 1;

	UpdateView();

	double angle = m_pBufferFoil->DeRotate();
	QString str = QString(tr("Foil has been de-rotated by %1 degrees")).arg(angle,6,'f',3);
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->statusBar()->showMessage(str);

	//then duplicate the buffer foil and add it
	Foil *pNewFoil = new Foil();
	pNewFoil->CopyFoil(m_pBufferFoil);
	pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
	pNewFoil->m_FoilStyle = 0;
	pNewFoil->m_FoilWidth = 1;

	AddNewFoil(pNewFoil);
	FillFoilTable();
	SelectFoil(pNewFoil);

	m_pBufferFoil->m_bVisible = false;

	UpdateView();
}


/**
 * The user has requested that the Foil be normalized to unit length.
 */
void QAFoil::OnAFoilNormalizeFoil()
{
	if(!Foil::curFoil()) return;
	double length = Foil::curFoil()->NormalizeGeometry();
	Foil::curFoil()->InitFoil();
	QString str = QString(tr("Foil has been normalized from %1  to 1.000")).arg(length,7,'f',3);

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->statusBar()->showMessage(str);

	UpdateView();
}


/**
 * The user has requested a local refinement of the panels of the current Foil.
 */
void QAFoil::OnAFoilCadd()
{
	if(!Foil::curFoil()) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	m_pBufferFoil->CopyFoil(Foil::curFoil());
	m_pBufferFoil->m_FoilName   = Foil::curFoil()->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_FoilStyle = 1;
	m_pBufferFoil->m_FoilWidth = 1;
	m_pBufferFoil->m_bPoints    = true;
	m_pBufferFoil->m_bVisible   = true;

	UpdateView();

	CAddDlg caDlg(pMainFrame);
	caDlg.m_pBufferFoil = m_pBufferFoil;
	caDlg.m_pMemFoil    = Foil::curFoil();
	caDlg.InitDialog();

	if(QDialog::Accepted == caDlg.exec())
	{
		//then duplicate the buffer foil and add it
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_bPoints = false;

		AddNewFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pNewFoil);
	}
	else
	{
		FillFoilTable();
		SelectFoil(Foil::curFoil());
		((XFoil*)m_pXFoil)->m_FoilName ="";

	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}

/**
 * The user has requested the display of a circle at the L.E. position.
 */
void QAFoil::OnAFoilLECircle()
{
    LECircleDlg LECircleDlg(this);
	LECircleDlg.m_Radius      = m_LERad;
	LECircleDlg.m_bShowRadius = m_bLECircle;
	LECircleDlg.m_pAFoil      = this;
	LECircleDlg.InitDialog();

	if(LECircleDlg.exec()==QDialog::Accepted)
	{
		m_LERad = LECircleDlg.m_Radius;
		m_bLECircle = LECircleDlg.m_bShowRadius;
	}
	UpdateView();
}


/**
 * The user has requested the launch of the interface to refine globally the Foil.
*/
void QAFoil::OnAFoilPanels()
{
	if(!Foil::curFoil()) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	m_pBufferFoil->CopyFoil(Foil::curFoil());
	m_pBufferFoil->m_FoilName  = Foil::curFoil()->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_FoilStyle = 1;
	m_pBufferFoil->m_FoilWidth = 1;
	m_pBufferFoil->m_bPoints   = true;
	m_pBufferFoil->m_bVisible  = true;

	UpdateView();

    TwoDPanelDlg tdpDlg(pMainFrame);
    tdpDlg.m_pBufferFoil = m_pBufferFoil;
	tdpDlg.m_pMemFoil    = Foil::curFoil();
    tdpDlg.InitDialog();

    if(QDialog::Accepted == tdpDlg.exec())
	{
		//then duplicate the buffer foil and add it
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_bPoints = false;

		AddNewFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pNewFoil);
	}
	else
	{
		FillFoilTable();
		SelectFoil(Foil::curFoil());
//		m_pXFoil->m_FoilName ="";

	}

	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}

/**
 * The user has requested the launch of the interface to edit the Foil coordinates manually.
*/
void QAFoil::OnAFoilFoilCoordinates()
{
	if(!Foil::curFoil()) return;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	m_pBufferFoil->CopyFoil(Foil::curFoil());
	m_pBufferFoil->m_bPoints  = true;
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = Foil::curFoil()->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_FoilStyle = 1;
	m_pBufferFoil->m_FoilWidth = 1;

	UpdateView();

    FoilCoordDlg fcDlg(pMainFrame);
	fcDlg.m_pMemFoil    = Foil::curFoil();
    fcDlg.m_pBufferFoil = m_pBufferFoil;
	fcDlg.InitDialog();

    if(QDialog::Accepted == fcDlg.exec())
	{
		//then duplicate the buffer foil and add it
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_iHighLight = -1;

		AddNewFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pNewFoil);
	}
	else
	{
		FillFoilTable();
		SelectFoil(Foil::curFoil());
		((XFoil*)m_pXFoil)->m_FoilName ="";
	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


/**
 * The user has requested to perform an edition of the current foil's thickness and camber properties.
 */
void QAFoil::OnAFoilFoilGeom()
{
	if(!Foil::curFoil()) return;
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	m_pBufferFoil->CopyFoil(Foil::curFoil());
	m_pBufferFoil->m_bPoints  = true;
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = Foil::curFoil()->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_FoilStyle = 1;
	m_pBufferFoil->m_FoilWidth = 1;

	UpdateView();

    FoilGeomDlg fgeDlg(pMainFrame);
	fgeDlg.m_pMemFoil    = Foil::curFoil();
    fgeDlg.m_pBufferFoil = m_pBufferFoil;
    fgeDlg.InitDialog();

    if(QDialog::Accepted == fgeDlg.exec())
	{
		//then duplicate the buffer foil and add it
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_bPoints = false;

		AddNewFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pNewFoil);
	}
	else
	{
		FillFoilTable();
		SelectFoil(Foil::curFoil());
//		m_pXFoil->m_FoilName ="";
	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


/**
 * The user has requested the launch of the interface to modify the gap at the Foil's trailing edge.
 */
void QAFoil::OnAFoilSetTEGap()
{
	if(!Foil::curFoil()) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	m_pBufferFoil->CopyFoil(Foil::curFoil());
	m_pBufferFoil->m_bPoints    = false;
	m_pBufferFoil->m_bVisible   = true;
	m_pBufferFoil->m_FoilName   = Foil::curFoil()->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_FoilStyle = 1;
	m_pBufferFoil->m_FoilWidth = 1;

	UpdateView();

    TEGapDlg teDlg(pMainFrame);
    teDlg.m_pBufferFoil = m_pBufferFoil;
	teDlg.m_pMemFoil    = Foil::curFoil();
	teDlg.InitDialog();

    if(QDialog::Accepted == teDlg.exec())
	{
		//then duplicate the buffer foil and add it
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_bPoints    = false;

		AddNewFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pNewFoil);
	}
	else
	{
		FillFoilTable();
		SelectFoil(Foil::curFoil());
//		m_pXFoil->m_FoilName ="";
		//to un-initialize XFoil in case user switches to XInverse
		//Thanks Jean-Marc !
	}

	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


/**
 * The user has requested the launch of the interface to modify the radius of the Foil's leading edge.
 */
void QAFoil::OnAFoilSetLERadius()
{
	if(!Foil::curFoil()) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	m_pBufferFoil->CopyFoil(Foil::curFoil());
	m_pBufferFoil->m_bVisible   = true;
	m_pBufferFoil->m_bPoints    = false;
	m_pBufferFoil->m_FoilName   = Foil::curFoil()->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_FoilStyle = 1;
	m_pBufferFoil->m_FoilWidth = 1;

	UpdateView();

    LEDlg leDlg(pMainFrame);
    leDlg.m_pBufferFoil = m_pBufferFoil;
	leDlg.m_pMemFoil    = Foil::curFoil();
    leDlg.InitDialog();

    if(QDialog::Accepted == leDlg.exec())
	{
		//then duplicate the buffer foil and add it
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_bPoints    = false;

		AddNewFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pNewFoil);
	}
	else
	{
		FillFoilTable();
		SelectFoil(Foil::curFoil());
//		m_pXFoil->m_FoilName ="";

	}

	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


/**
 * The user has requested the launch of the interface to create a foil from the interpolation of two existing Foil objects.
 */
void QAFoil::OnAFoilInterpolateFoils()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	if(m_poaFoil->size()<2)
	{
		QMessageBox::warning(pMainFrame,tr("Warning"), tr("At least two foils are required"));
		return;
	}

	if(!Foil::curFoil()) SelectFoil();
	if(!Foil::curFoil()) return;
	m_pBufferFoil->CopyFoil(Foil::curFoil());
	m_pBufferFoil->m_FoilName  = Foil::curFoil()->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_FoilStyle = 1;
	m_pBufferFoil->m_FoilWidth = 1;
	m_pBufferFoil->m_bPoints   = false;
	m_pBufferFoil->m_bVisible  = true;

	UpdateView();

	InterpolateFoilsDlg ifDlg(pMainFrame);
    ifDlg.m_poaFoil = m_poaFoil;
    ifDlg.m_pBufferFoil = m_pBufferFoil;
    ifDlg.InitDialog();

    if(QDialog::Accepted == ifDlg.exec())
	{
		//then duplicate the buffer foil and add it
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_bPoints = false;
        pNewFoil->m_FoilName = ifDlg.m_NewFoilName;

		AddNewFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pNewFoil);

	}
	else
	{
		FillFoilTable();
		SelectFoil(Foil::curFoil());
	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


/**
 * The user has requested the launch of the interface used to create a NACA type Foil.
 */
void QAFoil::OnAFoilNacaFoils()
{
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	m_pBufferFoil->SetNaca009();
	m_pBufferFoil->m_bPoints  = true;
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = "Naca xxxx";
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_FoilStyle = 1;
	m_pBufferFoil->m_FoilWidth = 1;

	UpdateView();

    NacaFoilDlg nacaDlg(pMainFrame);
    nacaDlg.m_pBufferFoil = m_pBufferFoil;

    if(QDialog::Accepted == nacaDlg.exec())
	{
		//then duplicate the buffer foil and add it
		QString str;

        if(nacaDlg.s_Digits>0 && log10((double)nacaDlg.s_Digits)<4)
            str = QString("%1").arg(nacaDlg.s_Digits,4,10,QChar('0'));
		else
            str = QString("%1").arg(nacaDlg.s_Digits);
		str = "NACA "+ str;

		Foil *pNewFoil    = new Foil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;
		pNewFoil->m_bPoints    = false;
		pNewFoil->m_FoilName   = str;

		AddNewFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pNewFoil);
	}
	else
	{
		FillFoilTable();;
		if(Foil::curFoil()) SelectFoil(Foil::curFoil());
		((XFoil*)m_pXFoil)->m_FoilName ="";

	}

	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


/**
 * The user has requested the launch of the interface to define a L.E. or T.E. flap.
 */
void QAFoil::OnAFoilSetFlap()
{
	if(!Foil::curFoil()) return;

	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;

	m_pBufferFoil->CopyFoil(Foil::curFoil());
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = Foil::curFoil()->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_FoilStyle = 1;
	m_pBufferFoil->m_FoilWidth = 1;

	UpdateView();

    FlapDlg flDlg(pMainFrame);
	flDlg.m_pXFoil      = m_pXFoil;
	flDlg.m_pMemFoil    = Foil::curFoil();
    flDlg.m_pBufferFoil = m_pBufferFoil;
    flDlg.InitDialog();

    if(QDialog::Accepted == flDlg.exec())
	{
		//then duplicate the buffer foil and add it
		Foil *pNewFoil = new Foil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = MainFrame::GetColor(0);
		pNewFoil->m_FoilStyle = 0;
		pNewFoil->m_FoilWidth = 1;

		AddNewFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pNewFoil);
	}
	else
	{
		FillFoilTable();
		SelectFoil(Foil::curFoil());
		((XFoil*)m_pXFoil)->m_FoilName ="";
	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


/**
 * The user has requested the deletion of the current Foil.
 */
void QAFoil::OnDeleteCurFoil()
{
	if(!Foil::curFoil()) return;

	QString strong;
	strong = tr("Are you sure you want to delete")  +"\n"+ Foil::curFoil()->foilName() +"\n";
	strong+= tr("and all associated OpPoints and Polars ?");

	int resp = QMessageBox::question((MainFrame*)s_pMainFrame, tr("Question"), strong,  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
	if(resp != QMessageBox::Yes) return;

	Foil*pNextFoil = Foil::deleteFoil(Foil::curFoil());

	FillFoilTable();
	SelectFoil(pNextFoil);
	UpdateView();
}


/**
 * The user has requested the duplication of the current Foil.
 */
void QAFoil::OnDuplicate()
{
	if(!Foil::curFoil()) return;
	Foil *pNewFoil = new Foil;
	pNewFoil->CopyFoil(Foil::curFoil());
	pNewFoil->m_FoilColor = MainFrame::GetColor(0);
	pNewFoil->InitFoil();

	AddNewFoil(pNewFoil);
	FillFoilTable();
	SelectFoil(pNewFoil);
}


/**
 * The user has requested the export of the current Foil to a text file.
 */
void QAFoil::OnExportCurFoil()
{
	if(!Foil::curFoil())	return;

	QString FileName;

	FileName = Foil::curFoil()->m_FoilName;
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
 * The user has requested the export of the current SplineFoil to a text file.
 */
void QAFoil::OnExportSplinesToFile()
{
	QString FoilName = tr("Spline Foil");
	QString FileName, strong;
	QString strOut;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	// deselect points so as not to interfere with other mouse commands
	m_pSF->m_Intrados.m_iSelect = -10;
	m_pSF->m_Extrados.m_iSelect = -10;

	//check that the number of output points is consistent with the array's size

	if(m_pSF->m_Extrados.m_iRes>IQX2)
	{
		strong = QString(tr("Too many output points on upper surface\n Max =%1")).arg(IQX2);
		QMessageBox::warning(pMainFrame, tr("Warning"), strong, QMessageBox::Ok);
		return;
	}
	if(m_pSF->m_Intrados.m_iRes>IQX2)
	{
		strong = QString(tr("Too many output points on lower surface\n Max =%1")).arg(IQX2);
		QMessageBox::warning(pMainFrame, tr("Warning"), strong, QMessageBox::Ok);
		return;
	}


	FileName.replace("/", " ");
	FileName = QFileDialog::getSaveFileName(this, tr("Export Splines"), 
				Settings::s_LastDirName,
				tr("Text File (*.dat)"));

	if(!FileName.length()) return;
	int pos;
	pos = FileName.lastIndexOf("/");
	if(pos>0) Settings::s_LastDirName = FileName.left(pos);

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);


	strOut = FoilName + "\n";
	out << strOut;

	m_pSF->ExportToFile(out);
	XFile.close();
}


/**
 * A row has been clicked in the table of Foil objects.
 * @param index a QModelIndex object clicked in the table
 */
void QAFoil::OnFoilClicked(const QModelIndex& index)
{
	m_pctrlFoilTable->blockSignals(true);
	m_pFoilModel->blockSignals(true);

	if(index.row()>=m_poaFoil->size()+1) return;
	QStandardItem *pItem = m_pFoilModel->item(index.row(),0);
	if(!pItem) return;

	m_pctrlFoilTable->selectRow(index.row());

	if(index.row()==0)
	{
		Foil::setCurFoil(NULL);
		if(index.column()==12)
		{
			m_pSF->m_bVisible = !m_pSF->m_bVisible;
		}
		else if(index.column()==13)
		{
			m_pSF->m_bOutPoints = !m_pSF->m_bOutPoints;
		}
		else if(index.column()==14)
		{
			m_pSF->m_bCenterLine = !m_pSF->m_bCenterLine;
		}
		UpdateView();
	}
	else if(index.row()>0)
	{
		Foil *pFoil= Foil::foil(pItem->text());
		Foil::setCurFoil(pFoil);

		if(pFoil)
		{
			if(index.column()==12)
			{
				pFoil->m_bVisible = !pFoil->m_bVisible;
			}
			else if(index.column()==13)
			{
				pFoil->m_bPoints = !pFoil->m_bPoints;
			}
			else if(index.column()==14)
			{
				pFoil->m_bCenterLine = !pFoil->m_bCenterLine;
			}
		}
		emit projectModified();
		UpdateView();
	}

	if(index.column()==15) OnFoilStyle();

	SetControls();

	m_pctrlFoilTable->blockSignals(false);
	m_pFoilModel->blockSignals(false);
}



/**
 * The user has requested an edition of the style of the active Foil.
 */
void QAFoil::OnFoilStyle()
{
	if(!Foil::curFoil())
	{
        LinePickerDlg dlg(this);
		dlg.InitDialog(m_pSF->m_FoilStyle, m_pSF->m_FoilWidth, m_pSF->m_FoilColor);

		if(QDialog::Accepted==dlg.exec())
		{
			m_pSF->SetCurveParams(dlg.GetStyle(), dlg.GetWidth(), dlg.GetColor());
			UpdateView();
		}
	}
	else
	{
        LinePickerDlg dlg(this);
		dlg.InitDialog(Foil::curFoil()->m_FoilStyle, Foil::curFoil()->m_FoilWidth, Foil::curFoil()->m_FoilColor);

		if(QDialog::Accepted==dlg.exec())
		{
			emit projectModified();
			Foil::curFoil()->m_FoilStyle = dlg.GetStyle();
			Foil::curFoil()->m_FoilWidth = dlg.GetWidth();
			Foil::curFoil()->m_FoilColor = dlg.GetColor();
			UpdateView();
		}
	}
}



/**
 * The user has requested the launch the interface for the edition of the grid parameters.
 */
void QAFoil::OnGrid()
{
	AFoilGridDlg dlg(this);

	dlg.m_bScale       = m_bScale;
	dlg.m_bNeutralLine = m_bNeutralLine;
	dlg.m_NeutralStyle = m_NeutralStyle;
	dlg.m_NeutralWidth = m_NeutralWidth;
	dlg.m_NeutralColor = m_NeutralColor;

	dlg.m_bXGrid     = m_bXGrid;
	dlg.m_bXMinGrid  = m_bXMinGrid;
	dlg.m_XStyle     = m_XGridStyle;
	dlg.m_XWidth     = m_XGridWidth;
	dlg.m_XColor     = m_XGridColor;
	dlg.m_XUnit      = m_XGridUnit;
	dlg.m_XMinStyle  = m_XMinStyle;
	dlg.m_XMinWidth  = m_XMinWidth;
	dlg.m_XMinColor  = m_XMinColor;
	dlg.m_XMinUnit   = m_XMinUnit;

	dlg.m_bYGrid     = m_bYGrid;
	dlg.m_bYMinGrid  = m_bYMinGrid;
	dlg.m_YStyle     = m_YGridStyle;
	dlg.m_YWidth     = m_YGridWidth;
	dlg.m_YColor     = m_YGridColor;
	dlg.m_YUnit      = m_YGridUnit;
	dlg.m_YMinStyle  = m_YMinStyle;
	dlg.m_YMinWidth  = m_YMinWidth;
	dlg.m_YMinColor  = m_YMinColor;
	dlg.m_YMinUnit   = m_YMinUnit;

	dlg.InitDialog();

	if(dlg.exec() == QDialog::Accepted)
	{
		m_bScale       = dlg.m_bScale;
		m_bNeutralLine = dlg.m_bNeutralLine;
		m_NeutralStyle = dlg.m_NeutralStyle;
		m_NeutralWidth = dlg.m_NeutralWidth;
		m_NeutralColor = dlg.m_NeutralColor;

		m_bXGrid     = dlg.m_bXGrid;
		m_bXMinGrid  = dlg.m_bXMinGrid;
		m_XGridStyle = dlg.m_XStyle;
		m_XGridWidth = dlg.m_XWidth;
		m_XGridColor = dlg.m_XColor;
		m_XGridUnit  = dlg.m_XUnit;
		m_XMinStyle  = dlg.m_XMinStyle;
		m_XMinWidth  = dlg.m_XMinWidth;
		m_XMinColor  = dlg.m_XMinColor;
		m_XMinUnit   = dlg.m_XMinUnit;

		m_bYGrid     = dlg.m_bYGrid;
		m_bYMinGrid  = dlg.m_bYMinGrid;
		m_YGridStyle = dlg.m_YStyle;
		m_YGridWidth = dlg.m_YWidth;
		m_YGridColor = dlg.m_YColor;
		m_YGridUnit  = dlg.m_YUnit;
		m_YMinStyle  = dlg.m_YMinStyle;
		m_YMinWidth  = dlg.m_YMinWidth;
		m_YMinColor  = dlg.m_YMinColor;
		m_YMinUnit   = dlg.m_YMinUnit;
	}
	UpdateView();
}



/**
 * The user has requested that the visibility of all Foil objects be turned off.
 */
void QAFoil::OnHideAllFoils()
{
	emit projectModified();
	Foil*pFoil;
	for (int k=0; k<m_poaFoil->size(); k++)
	{
		pFoil = (Foil*)m_poaFoil->at(k);
		pFoil->m_bVisible = false;
	}
	FillFoilTable();
	UpdateView();
}



/**
 * The user has requested that the visibility of the active Foil object be turned off.
 */
void QAFoil::OnHideCurrentFoil()
{
	if(!Foil::curFoil()) return;
	ShowFoil(Foil::curFoil(), false);
	UpdateView();

}


/**
 * The user has requested to restore the default settings for the splines.
 */
void QAFoil::OnNewSplines()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(m_pSF->m_bModified)
	{
		if (QMessageBox::Yes != QMessageBox::question(pMainFrame, tr("Question"), tr("Discard changes to Splines ?"),
													  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))
		{
			return;
		}
	}
	m_pSF->InitSplineFoil();

	m_StackPos  = 0;
	ClearStack(0);
	TakePicture();

	emit projectModified();
	UpdateView();
}


/**
 * The user has requested to rename the Foil object
 */
void QAFoil::OnRenameFoil()
{
	if(!Foil::curFoil()) return;

	Foil*pOldFoil;

	QStringList NameList;
	for(int k=0; k<Foil::s_oaFoil.size(); k++)
	{
		pOldFoil = (Foil*)Foil::s_oaFoil.at(k);
		NameList.append(pOldFoil->m_FoilName);
	}

	RenameDlg renDlg(this);
	renDlg.InitDialog(&NameList, Foil::curFoil()->foilName(), tr("Enter the foil's new name"));

	if(renDlg.exec() !=QDialog::Rejected)
	{
		Foil::curFoil()->renameThisFoil(renDlg.newName());
	}

	FillFoilTable();
	UpdateView();
}


/**
 * The user has requested that the visibility of all Foil objects be turned on.
 */
void QAFoil::OnShowAllFoils()
{
	emit projectModified();
	Foil*pFoil;
	for (int k=0; k<m_poaFoil->size(); k++)
	{
		pFoil = (Foil*)m_poaFoil->at(k);
		pFoil->m_bVisible = true;
	}
	FillFoilTable();
	UpdateView();
}


/**
 * The user has requested that the visibility of the active Foil object be turned on.
 */
void QAFoil::OnShowCurrentFoil()
{
	if(!Foil::curFoil()) return;
	ShowFoil(Foil::curFoil(), true);
	UpdateView();

}


/**
 * The user has toggled the visibility of the legend
 */
void QAFoil::OnShowLegend()
{
	m_bShowLegend = !m_bShowLegend;
	UpdateView();
	SetControls();
}


/**
 * The user has requested to convert the SplineFoil object to a Foil, and to store it in the database.
 */
void QAFoil::OnStoreSplines()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(m_pSF->m_Extrados.m_iRes>IQX2)
	{
		QString strong = QString(tr("Too many output points on upper surface\n Max =%1")).arg(IQX2);
		QMessageBox::warning(pMainFrame, tr("Warning"), strong, QMessageBox::Ok);
		return;
	}
	if(m_pSF->m_Intrados.m_iRes>IQX2)
	{
		QString strong = QString(tr("Too many output points on lower surface\n Max =%1")).arg(IQX2);
		QMessageBox::warning(pMainFrame, tr("Warning"), strong, QMessageBox::Ok);
		return;
	}


	Foil *pNewFoil = new Foil();
	m_pSF->ExportToBuffer(pNewFoil);
	pNewFoil->m_FoilName = m_pSF->m_strFoilName;
	AddNewFoil(pNewFoil);
	FillFoilTable();
	SelectFoil(pNewFoil);

	UpdateView();
}


/**
 * The user has requested to reset the x-scale to its default value.
 */
void QAFoil::OnResetXScale()
{
	SetScale();
	ReleaseZoom();
	UpdateView();
}


/**
 * The user has requested to reset the y-scale to its default value.
 */
void QAFoil::OnResetYScale()
{
	m_fScaleY = 1.0;
	UpdateView();
}

/**
 * The user has requested to reset the scales to their default value.
 */
void QAFoil::OnResetScales()
{
	m_fScaleY = 1.0;
	SetScale();
	ReleaseZoom();
	UpdateView();
}


/**
 * The user has requested the launch of the interface to edit SplineFoil data.
 */
void QAFoil::OnSplineControls()
{
    SplineCtrlsDlg dlg(this);
	dlg.m_pSF = m_pSF;
	dlg.InitDialog();

	SplineFoil memSF;
	memSF.Copy(m_pSF);

	if(dlg.exec() == QDialog::Accepted)
	{
		TakePicture();
	}
	else m_pSF->Copy(&memSF);
}



/**
 * The user has requested to zoom in on the display by drawing a rectangle on the screen.
 */
void QAFoil::OnZoomIn()
{
	if(!m_bZoomPlus)
	{
		if(m_fScale/m_fRefScale <32.0)
		{
			m_bZoomPlus = true;
			MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
			pMainFrame->zoomInAct->setChecked(true);

		}
		else
		{
			ReleaseZoom();
		}
	}
	else
	{
		ReleaseZoom();
	}
}


/**
 * The user has requested to scale the y-axis only.
 */
void QAFoil::OnZoomYOnly()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	TwoDWidget *p2Dwidget = (TwoDWidget*)s_p2DWidget;
	m_bZoomYOnly = !m_bZoomYOnly;
	pMainFrame->zoomYAct->setChecked(m_bZoomYOnly);

	p2Dwidget->setFocus();
}


/**
 * The user has requested to zoom out.
 */
void QAFoil::OnZoomLess()
{
	// can't do two things at the same time can we ?
	ReleaseZoom();

	double ZoomFactor = 0.8;
	double newScale = qMax(ZoomFactor*m_fScale, m_fRefScale);

	ZoomFactor = qMax(ZoomFactor, newScale/m_fScale);

	m_fScale = ZoomFactor*m_fScale;
	int a = (int)((m_rCltRect.right()+m_rCltRect.left())/2);
	int b = (int)((m_rCltRect.top()+m_rCltRect.bottom())/2);

	//scale
	m_ptOffset.rx() = (int)(ZoomFactor*(m_ptOffset.x()-a)+a);
	m_ptOffset.ry() = (int)(ZoomFactor*(m_ptOffset.y()-b)+b);

	UpdateView();
}


/**
 * Draws the grids.
 * @param painter a reference to the QPainter object with which to draw.
 */
void QAFoil::PaintGrids(QPainter &painter)
{
	painter.save();

	if(m_bZoomPlus&& !m_ZoomRect.isEmpty())
	{
		QRect ZRect = m_ZoomRect.normalized();
		QPen ZoomPen(QColor(100,100,100));
		ZoomPen.setStyle(Qt::DashLine);
		painter.setPen(ZoomPen);
		painter.drawRect(ZRect);
	}

	if(m_bLECircle)
	{
		int rx = (int)(m_LERad/100.0 * m_fScale);
		int ry = (int)(m_LERad/100.0 * m_fScale * m_fScaleY);
		QRect rc(m_ptOffset.x(), m_ptOffset.y() - ry,  2*rx, 2*ry);

		QPen CirclePen(QColor(128,128,128));
		CirclePen.setStyle(Qt::DashLine);
		painter.setPen(CirclePen);
		painter.drawEllipse(rc);
	}
	
	if (m_bNeutralLine)
	{
		QPen NPen(m_NeutralColor);
		NPen.setStyle(getStyle(m_NeutralStyle));
		NPen.setWidth(m_NeutralWidth);
		painter.setPen(NPen);

		painter.drawLine(m_rCltRect.right(),m_ptOffset.y(), m_rCltRect.left(),m_ptOffset.y());
	}

	//draw grids
	if(m_bXGrid)	DrawXGrid(painter, m_fScale, m_fScale*m_fScaleY, m_ptOffset, m_rCltRect);
	if(m_bYGrid)	DrawYGrid(painter, m_fScale, m_fScale*m_fScaleY, m_ptOffset, m_rCltRect);
	if(m_bXMinGrid) DrawXMinGrid(painter, m_fScale, m_fScale*m_fScaleY, m_ptOffset, m_rCltRect);
	if(m_bYMinGrid) DrawYMinGrid(painter, m_fScale, m_fScale*m_fScaleY, m_ptOffset, m_rCltRect);

	if(m_bScale) DrawScale(painter, m_fScale);

	painter.restore();
}


/**
 * Draws the legend.
 * @param painter a reference to the QPainter object with which to draw.
 */
void QAFoil::PaintLegend(QPainter &painter)
{
	painter.save();

	painter.setFont(Settings::s_TextFont);

	if(m_bShowLegend)
	{
		Foil* pRefFoil;
		QString strong;
		QPoint Place(m_rCltRect.right()-250, 10);
		int LegendSize, ypos, x1, n, k, delta;

		LegendSize = 20;
		ypos = 15;
		delta = 5;

		painter.setBackgroundMode(Qt::TransparentMode);

		QPen TextPen(Settings::s_TextColor);
		painter.setPen(TextPen);
		QPen LegendPen;

		k=0;

		if(m_pSF->m_bVisible)
		{
			LegendPen.setColor(m_pSF->m_FoilColor);
			LegendPen.setStyle(getStyle(m_pSF->m_FoilStyle));
			LegendPen.setWidth(m_pSF->m_FoilWidth);

			painter.setPen(LegendPen);
			painter.drawLine(Place.x(), Place.y() + ypos*k, Place.x() + (int)(LegendSize), Place.y() + ypos*k);
			if(m_pSF->m_bOutPoints )
			{
//					x1 = Place.x + (int)(0.5*LegendSize);
//					pDC->Rectangle(x1-2, Place.y + ypos*k-2, x1+2, Place.y + ypos*k+2);
				x1 = Place.x() + (int)(0.5*LegendSize);
				painter.drawRect(x1-2, Place.y() + ypos*k-2, 4,4);
			}
			painter.setPen(TextPen);
			painter.drawText(Place.x() + (int)(1.5*LegendSize), Place.y() + ypos*k+delta, m_pSF->m_strFoilName);
		}

		k++;
		for (n=0; n < m_poaFoil->size(); n++)
		{
			pRefFoil = (Foil*)m_poaFoil->at(n);
			if(pRefFoil && pRefFoil->m_bVisible)
			{
				strong = pRefFoil->m_FoilName;
				if(strong.length())
				{
					LegendPen.setColor(pRefFoil->m_FoilColor);
					LegendPen.setStyle(getStyle(pRefFoil->m_FoilStyle));
					LegendPen.setWidth(pRefFoil->m_FoilWidth);

					painter.setPen(LegendPen);
					painter.drawLine(Place.x(), Place.y() + ypos*k, Place.x() + (int)(LegendSize), Place.y() + ypos*k);

					if(pRefFoil->m_bPoints)
					{
						x1 = Place.x() + (int)(0.5*LegendSize);
						painter.drawRect(x1-2, Place.y() + ypos*k-2, 4,4);
					}
					painter.setPen(TextPen);
					painter.drawText(Place.x() + (int)(1.5*LegendSize), Place.y() + ypos*k+delta, pRefFoil->m_FoilName);
					k++;
				}
			}
		}
	}
	painter.restore();
}


/**
 * Draws the view.
 * @param painter a reference to the QPainter object with which to draw.
 */
void QAFoil::PaintView(QPainter &painter)
{
	painter.save();

	double xscale = m_fScale          /m_fRefScale;
	double yscale = m_fScale*m_fScaleY/m_fRefScale;

	//zoom from the center of the viewport
	QPoint VCenter = QPoint((int)((m_rCltRect.right() + m_rCltRect.left()  )/2),
							(int)((m_rCltRect.top()   + m_rCltRect.bottom())/2));

	painter.fillRect(m_rCltRect, Settings::s_BackgroundColor);

	//draw the background image in the viewport
	if(m_bIsImageLoaded && !m_BackImage.isNull())
	{
		int w = (int)((double)m_BackImage.width()* xscale);
		int h = (int)((double)m_BackImage.height()* yscale);
		//the coordinates of the top left corner are measured from the center of the viewport
		double xtop = VCenter.x() + m_ViewportTrans.x() - (int)((double)m_BackImage.width()  /2.*xscale);
		double ytop = VCenter.y() + m_ViewportTrans.y() - (int)((double)m_BackImage.height() /2.*yscale);

		painter.drawPixmap(xtop, ytop, w,h, m_BackImage);
	}

//	m_ptOffset.rx() = VCenter.x() + m_ViewportTrans.x() - (int)(0.5 *m_fScale);
//	m_ptOffset.ry() = VCenter.y() + m_ViewportTrans.y() ;

	painter.setFont(Settings::s_TextFont);

	QPen TextPen(Settings::s_TextColor);
	painter.setPen(TextPen);

	PaintGrids(painter);
	PaintSplines(painter);
	PaintFoils(painter);
	PaintLegend(painter);

	QString str;

	str = QString(tr("X-Scale = %1")).arg(m_fScale/m_fRefScale,4,'f',1);
	painter.drawText(5,10, str);
	str = QString(tr("Y-Scale = %1")).arg(m_fScaleY*m_fScale/m_fRefScale,4,'f',1);
	painter.drawText(5,22, str);
	str = QString(tr("x  = %1")).arg(m_MousePos.x,7,'f',4);
	painter.drawText(5,34, str);
	str = QString(tr("y  = %1")).arg(m_MousePos.y,7,'f',4);
	painter.drawText(5,46, str);


	painter.restore();
}


/**
 * Draws the SplineFoil object.
 * @param painter a reference to the QPainter object with which to draw.
 */
void QAFoil::PaintSplines(QPainter &painter)
{
	painter.save();

	QPen CtrlPen;

	QBrush FillBrush(Settings::s_BackgroundColor);
	painter.setBrush(FillBrush);

	if(m_pSF->m_bVisible)
	{
		m_pSF->DrawFoil(painter, m_fScale,m_fScale*m_fScaleY, m_ptOffset);

		CtrlPen.setStyle(Qt::SolidLine);
		CtrlPen.setColor(m_pSF->m_FoilColor);
		painter.setPen(CtrlPen);

		m_pSF->DrawCtrlPoints(painter, m_fScale,m_fScale*m_fScaleY, m_ptOffset);

		if (m_pSF->m_bCenterLine)
		{
			m_pSF->DrawMidLine(painter, m_fScale,m_fScale*m_fScaleY, m_ptOffset);
		}
		if (m_pSF->m_bOutPoints)
		{
			m_pSF->DrawOutPoints(painter, m_fScale,m_fScale*m_fScaleY, m_ptOffset);
		}
	}

	painter.restore();
}


/**
 * Draws the visible Foil objects.
 * @param painter a reference to the QPainter object with which to draw.
 */
void QAFoil::PaintFoils(QPainter &painter)
{
	painter.save();
	int k;
	Foil *pFoil;

	QPen FoilPen, CenterPen, CtrlPen;

	QBrush FillBrush(Settings::s_BackgroundColor);
	painter.setBrush(FillBrush);

	for (k=0; k< m_poaFoil->size(); k++)
	{
		pFoil = (Foil*)m_poaFoil->at(k);
		if (pFoil->m_bVisible)
		{
			FoilPen.setStyle(getStyle(pFoil->m_FoilStyle));
			FoilPen.setWidth(pFoil->m_FoilWidth);
			FoilPen.setColor(pFoil->m_FoilColor);
			painter.setPen(FoilPen);

			pFoil->DrawFoil(painter, 0.0, m_fScale, m_fScale*m_fScaleY,m_ptOffset);

			if (pFoil->m_bCenterLine)
			{
				CenterPen.setColor(pFoil->m_FoilColor);
				CenterPen.setStyle(Qt::DashLine);
				painter.setPen(CenterPen);
				pFoil->DrawMidLine(painter, m_fScale, m_fScale*m_fScaleY, m_ptOffset);
			}
			if (pFoil->m_bPoints)
			{
				CtrlPen.setColor(pFoil->m_FoilColor);
				painter.setPen(CtrlPen);
				pFoil->DrawPoints(painter, m_fScale,m_fScale*m_fScaleY, m_ptOffset);
			}
		}
	}
	if (m_pBufferFoil->m_bVisible)
	{
		m_pBufferFoil->DrawFoil(painter, 0.0, m_fScale, m_fScale*m_fScaleY,m_ptOffset);

		if (m_pBufferFoil->m_bCenterLine)
		{
			CenterPen.setColor(m_pBufferFoil->m_FoilColor);
			CenterPen.setStyle(Qt::DashLine);
			painter.setPen(CenterPen);
			m_pBufferFoil->DrawMidLine(painter, m_fScale, m_fScale*m_fScaleY, m_ptOffset);
		}
		if (m_pBufferFoil->m_bPoints)
		{
			CtrlPen.setColor(m_pBufferFoil->m_FoilColor);
			painter.setPen(CtrlPen);
			m_pBufferFoil->DrawPoints(painter, m_fScale,m_fScale*m_fScaleY, m_ptOffset);
		}
	}
	painter.restore();
}


/**
 * Ends the zoom-in action.
 */
void QAFoil::ReleaseZoom()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->zoomInAct->setChecked(false);
	TwoDWidget *p2DWidget = (TwoDWidget*)s_p2DWidget;
	m_bZoomPlus = false;

	m_ZoomRect.setRight(m_ZoomRect.left()-1);
	m_ZoomRect.setTop(m_ZoomRect.bottom()+1);
	p2DWidget->setCursor(m_hcCross);
}


/**
 * Saves the user-defined settings.
 * @param pSettings a pointer to the QSetting object.
 */
void QAFoil::SaveSettings(QSettings *pSettings)
{
	pSettings->beginGroup("DirectDesign");
	{
		pSettings->setValue("XMajGrid", m_bXGrid);
		pSettings->setValue("YMajGrid", m_bYGrid);
		pSettings->setValue("XMinGrid", m_bXMinGrid);
		pSettings->setValue("YMinGrid", m_bYMinGrid);
		pSettings->setValue("XMajStyle", m_XGridStyle);
		pSettings->setValue("YMajStyle", m_YGridStyle);
		pSettings->setValue("XMajWidth", m_XGridWidth);
		pSettings->setValue("YMajWidth", m_YGridWidth);
		pSettings->setValue("XMinStyle", m_XMinStyle);
		pSettings->setValue("YMinStyle", m_YMinStyle);
		pSettings->setValue("XMinWidth", m_XMinWidth);
		pSettings->setValue("YMinWidth", m_YMinWidth);
		pSettings->setValue("XMajUnit", m_XGridUnit);
		pSettings->setValue("YMajUnit", m_YGridUnit);
		pSettings->setValue("XMinUnit", m_XMinUnit);
		pSettings->setValue("YMinUnit", m_YMinUnit);

		pSettings->setValue("XMajColor",m_XGridColor);
		pSettings->setValue("YMajColor", m_YGridColor);
		pSettings->setValue("XMinColor", m_XMinColor);
		pSettings->setValue("YMinColor", m_YMinColor);

		pSettings->setValue("NeutralLine", m_bNeutralLine);
		pSettings->setValue("NeutralStyle", m_NeutralStyle);
		pSettings->setValue("NeutralWidth", m_NeutralWidth);
		pSettings->setValue("NeutralColor", m_NeutralColor);

		pSettings->setValue("SFStyle", m_pSF->m_FoilStyle);
		pSettings->setValue("SFWidth", m_pSF->m_FoilWidth);
		pSettings->setValue("SFColor", m_pSF->m_FoilColor);

		pSettings->setValue("SFVisible", m_pSF->m_bVisible);
		pSettings->setValue("SFOutPoints", m_pSF->m_bOutPoints);
		pSettings->setValue("SFCenterLine", m_pSF->m_bCenterLine);

		pSettings->setValue("LowerRes", m_pSF->m_Intrados.m_iRes);
		pSettings->setValue("UpperRes", m_pSF->m_Extrados.m_iRes);

		pSettings->setValue("LECircle", m_bLECircle);
		pSettings->setValue("Scale", m_bScale);
		pSettings->setValue("Legend", m_bShowLegend );
		
		QString str;
		for(int i=0; i<16; i++)
		{
			str = QString("Column_%1").arg(i);
			pSettings->setValue(str,m_pctrlFoilTable->columnWidth(i));
		}
		for(int i=0; i<16; i++)
		{
			str = QString("Column_%1").arg(i);
			pSettings->setValue(str+"_hidden", m_pctrlFoilTable->isColumnHidden(i));
		}

	}
	pSettings->endGroup();
}


/**
 * Sets the default scale for the Foil display.
 */
void QAFoil::SetScale()
{
	//scale is set by user zooming
	m_fRefScale = (double)m_rCltRect.width()-150.0;
	m_fScale = m_fRefScale;

	m_ptOffset.rx() = 75;
	m_ptOffset.ry() = (int)(m_rCltRect.height()/2);

	m_ViewportTrans = QPoint(0,0);
}


/**
 * Overloaded function.
 * Sets the default scale for the Foil display based on a specified client rectangle.
 */
void QAFoil::SetScale(QRect CltRect)
{
	m_rCltRect = CltRect;
	SetScale();
}



/**
 * The user has requested the context menu associated to the Foil table.
 * @param position the right-click positon
 */
void QAFoil::OnFoilTableCtxMenu(const QPoint &)
{
//	m_CurrentColumn = m_pctrlFoilTable->columnAt(position.x());
	MainFrame* pMainFrame = (MainFrame*)s_pMainFrame;
	pMainFrame->AFoilTableCtxMenu->exec(cursor().pos());
}


/**
 * Sets up the GUI.
 */
void QAFoil::SetupLayout()
{
	m_pctrlFoilTable   = new QTableView(this);
	m_pctrlFoilTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pctrlFoilTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pctrlFoilTable->setContextMenuPolicy(Qt::CustomContextMenu);
	m_pctrlFoilTable->setWordWrap(false);
	m_pctrlFoilTable->setFont(Settings::s_TableFont);
	m_pctrlFoilTable->horizontalHeader()->setFont(Settings::s_TableFont);

//	connect(m_pctrlFoilTable, SIGNAL(pressed(const QModelIndex &)), this, SLOT(OnFoilClicked(const QModelIndex&)));
	connect(m_pctrlFoilTable, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(OnFoilTableCtxMenu(const QPoint &)));

	QHBoxLayout *MainLayout = new QHBoxLayout;
	MainLayout->addWidget(m_pctrlFoilTable);
	setLayout(MainLayout);


	m_pFoilModel = new QStandardItemModel(this);
	m_pFoilModel->setRowCount(10);//temporary
	m_pFoilModel->setColumnCount(16);

	m_pFoilModel->setHeaderData(0,  Qt::Horizontal, tr("Name"));
	m_pFoilModel->setHeaderData(1,  Qt::Horizontal, tr("Thickness (%)"));
	m_pFoilModel->setHeaderData(2,  Qt::Horizontal, tr("at (%)"));
	m_pFoilModel->setHeaderData(3,  Qt::Horizontal, tr("Camber (%)"));
	m_pFoilModel->setHeaderData(4,  Qt::Horizontal, tr("at (%)"));
	m_pFoilModel->setHeaderData(5,  Qt::Horizontal, tr("Points"));
	m_pFoilModel->setHeaderData(6,  Qt::Horizontal, tr("TE Flap (")+QString::fromUtf8("°")+")");
	m_pFoilModel->setHeaderData(7,  Qt::Horizontal, tr("TE XHinge"));
	m_pFoilModel->setHeaderData(8,  Qt::Horizontal, tr("TE YHinge"));
	m_pFoilModel->setHeaderData(9,  Qt::Horizontal, tr("LE Flap (")+QString::fromUtf8("°")+")");
	m_pFoilModel->setHeaderData(10, Qt::Horizontal, tr("LE XHinge"));
	m_pFoilModel->setHeaderData(11, Qt::Horizontal, tr("LE YHinge"));
	m_pFoilModel->setHeaderData(12, Qt::Horizontal, tr("Show"));
	m_pFoilModel->setHeaderData(13, Qt::Horizontal, tr("Points"));
	m_pFoilModel->setHeaderData(14, Qt::Horizontal, tr("Centerline"));
	m_pFoilModel->setHeaderData(15, Qt::Horizontal, tr("Style"));
	m_pctrlFoilTable->setModel(m_pFoilModel);
	m_pctrlFoilTable->setWindowTitle(tr("Foils"));
	m_pctrlFoilTable->horizontalHeader()->setStretchLastSection(true);

	m_pFoilDelegate = new FoilTableDelegate(this);
	m_pFoilDelegate->m_pAFoil = this;
	m_pctrlFoilTable->setItemDelegate(m_pFoilDelegate);
	m_pFoilDelegate->m_pFoilModel = m_pFoilModel;

/*	int unitwidth = (int)(750.0/16.0);
	m_pctrlFoilTable->setColumnWidth(0, 3*unitwidth);
	for(int i=1; i<16; i++)		m_pctrlFoilTable->setColumnWidth(i, unitwidth);*/
	m_pctrlFoilTable->setColumnHidden(9, true);
	m_pctrlFoilTable->setColumnHidden(10, true);
	m_pctrlFoilTable->setColumnHidden(11, true);


	m_precision = new int[16];
	m_precision[0]  = 2;
	m_precision[1]  = 2;
	m_precision[2]  = 2;
	m_precision[3]  = 2;
	m_precision[4]  = 2;
	m_precision[5]  = 0;
	m_precision[6]  = 2;
	m_precision[7]  = 2;
	m_precision[8]  = 2;
	m_precision[9]  = 2;
	m_precision[10] = 2;
	m_precision[11] = 2;
	m_precision[12] = 2;
	m_precision[13] = 2;
	m_precision[14] = 2;
	m_precision[15] = 2;

	m_pFoilDelegate->m_Precision = m_precision;
//	connect(m_pFoilDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged(QWidget *)));

	connect(this, SIGNAL(projectModified()), (MainFrame*)s_pMainFrame, SLOT(OnProjectModified()));
}


/**
 * Selects the specified foil in the table of Foil objects. This will highlight the corresponding row.
 * @param pFoil
 */
void QAFoil::SelectFoil(Foil* pFoil)
{
	int i;

	if(pFoil)
	{
		QModelIndex ind;
		QString FoilName;

		for(i=0; i< m_pFoilModel->rowCount(); i++)
		{
			ind = m_pFoilModel->index(i, 0, QModelIndex());
			FoilName = ind.model()->data(ind, Qt::EditRole).toString();

			if(FoilName == pFoil->m_FoilName)
			{
				m_pctrlFoilTable->selectRow(i);
				break;
			}
		}
	}
	else
	{
		m_pctrlFoilTable->selectRow(0);
	}
	Foil::setCurFoil(pFoil);
}




/**
 * Initializes the Foil table, the QWidget and the QAction objects from the data.
 * Selects the current foil in the table
 */
void QAFoil::SetAFoilParams()
{
	SetTableFont();
	FillFoilTable();

	SelectFoil(Foil::curFoil());
	SetControls();
}

/**
 * Turns on or off the display of the current Foil object.
 * @param pFoil a pointer to the Foil object to show
 * @param bShow the new visibility status of the Foil
 */
void QAFoil::ShowFoil(Foil* pFoil, bool bShow)
{
	if(!pFoil) return;
	Foil::curFoil()->m_bVisible = bShow;
	emit projectModified();
}


/**
 * Copies the current SplineFoil object to a new SplineFoil object and pushes it on the stack.
 */
void QAFoil::TakePicture()
{
	//clear the downstream part of the stack which becomes obsolete
	ClearStack(m_StackPos);

	// append a copy of the current object
	m_UndoStack.append(SplineFoil(m_pSF));

	// the new current position is the top of the stack
	m_StackPos = m_UndoStack.size()-1;

	m_bStored = true;

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(s_pMainFrame && pMainFrame->UndoAFoilAct && pMainFrame->RedoAFoilAct)
	{
		pMainFrame->UndoAFoilAct->setEnabled(m_StackPos>0);
		pMainFrame->RedoAFoilAct->setEnabled(m_StackPos<m_UndoStack.size()-1);
	}
}



/**
 * Restores a SplineFoil definition from the current position in the stack.
 */
void QAFoil::SetPicture()
{
	SplineFoil SF = m_UndoStack.at(m_StackPos);
	m_pSF->Copy(&SF);
	m_pSF->m_Intrados.SplineKnots();
	m_pSF->m_Intrados.SplineCurve();
	m_pSF->m_Extrados.SplineKnots();
	m_pSF->m_Extrados.SplineCurve();
	m_pSF->UpdateSplineFoil();

	UpdateView();
}


/**
 * The user has requested to Undo the last modification to the SplineFoil object.
 */
void QAFoil::OnUndo()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	if(m_StackPos>0)
	{
		m_StackPos--;
		SetPicture();
		pMainFrame->UndoAFoilAct->setEnabled(m_StackPos>0);
		pMainFrame->RedoAFoilAct->setEnabled(m_StackPos<m_UndoStack.size()-1);
	}
	else
	{
		//nothing to restore
	}
}


/**
 *The user has requested a Redo operation after an undo.
 */
void QAFoil::OnRedo()
{
	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;
	if(m_StackPos<m_UndoStack.size()-1)
	{
		m_StackPos++;
		SetPicture();
		pMainFrame->UndoAFoilAct->setEnabled(m_StackPos>0);
		pMainFrame->RedoAFoilAct->setEnabled(m_StackPos<m_UndoStack.size()-1);
	}
}


/**
  * Clears the stack starting at a given position.
  * @param the first stack element to remove
  */
void QAFoil::ClearStack(int pos)
{
	for(int il=m_UndoStack.size()-1; il>pos; il--)
	{
		m_UndoStack.removeAt(il);     // remove from the stack
	}
	m_StackPos = m_UndoStack.size()-1;
}


/**
 * Refreshes the view.
 */
void QAFoil::UpdateView()
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
void QAFoil::zoomEvent(QPoint pt, double zoomFactor)
{
	if(! m_rCltRect.contains(pt)) return;

	m_ZoomRect.setBottomRight(m_ZoomRect.topLeft());
	ReleaseZoom();

	static double  scale;
	scale = m_fScale;

	if(!m_bZoomYOnly)
	{
		if (m_bXDown)
		{
			m_fScale  *= zoomFactor;
			m_fScaleY *= 1./zoomFactor;
		}
		else if (m_bYDown) m_fScaleY *= zoomFactor;
		else  m_fScale *= zoomFactor;
	}
	else m_fScaleY *= zoomFactor;


	int a = (int)((m_rCltRect.right() + m_rCltRect.left())/2);
	m_ptOffset.rx() = a + (int)((m_ptOffset.x()-a)*m_fScale/scale);
	m_ViewportTrans.rx() = (int)((m_ViewportTrans.x())*m_fScale /scale);
	m_ViewportTrans.ry() = (int)((m_ViewportTrans.y())*m_fScale /scale);

	UpdateView();
}


/**
 * The user has requested that the width of the columns of the Foil table be reset to default values.
 */
void QAFoil::OnResetColumnWidths()
{
	int unitwidth = (int)((double)m_pctrlFoilTable->width()/16.0);
	m_pctrlFoilTable->setColumnWidth(0, 3*unitwidth);
	for(int i=1; i<16; i++) m_pctrlFoilTable->setColumnWidth(i, unitwidth);
	m_pctrlFoilTable->setColumnHidden(9, true);
	m_pctrlFoilTable->setColumnHidden(10, true);
	m_pctrlFoilTable->setColumnHidden(11, true);
}

/**
 * The user has requested the lanuch of the interface to show or hide the columns of the Foil table.
 */
void QAFoil::OnAFoilTableColumns()
{
	AFoilTableDlg dlg((MainFrame*)s_pMainFrame);

	dlg.m_bFoilName    = !m_pctrlFoilTable->isColumnHidden(0);
	dlg.m_bThickness   = !m_pctrlFoilTable->isColumnHidden(1);
	dlg.m_bThicknessAt = !m_pctrlFoilTable->isColumnHidden(2);
	dlg.m_bCamber      = !m_pctrlFoilTable->isColumnHidden(3);
	dlg.m_bCamberAt    = !m_pctrlFoilTable->isColumnHidden(4);
	dlg.m_bPoints      = !m_pctrlFoilTable->isColumnHidden(5);
	dlg.m_bTEFlapAngle = !m_pctrlFoilTable->isColumnHidden(6);
	dlg.m_bTEXHinge    = !m_pctrlFoilTable->isColumnHidden(7);
	dlg.m_bTEYHinge    = !m_pctrlFoilTable->isColumnHidden(8);
	dlg.m_bLEFlapAngle = !m_pctrlFoilTable->isColumnHidden(9);
	dlg.m_bLEXHinge    = !m_pctrlFoilTable->isColumnHidden(10);
	dlg.m_bLEYHinge    = !m_pctrlFoilTable->isColumnHidden(11);

	dlg.InitDialog();

	if(dlg.exec()==QDialog::Accepted)
	{
		m_pctrlFoilTable->setColumnHidden(0,  !dlg.m_bFoilName);
		m_pctrlFoilTable->setColumnHidden(1,  !dlg.m_bThickness);
		m_pctrlFoilTable->setColumnHidden(2,  !dlg.m_bThicknessAt);
		m_pctrlFoilTable->setColumnHidden(3,  !dlg.m_bCamber);
		m_pctrlFoilTable->setColumnHidden(4,  !dlg.m_bCamberAt);
		m_pctrlFoilTable->setColumnHidden(5,  !dlg.m_bPoints);
		m_pctrlFoilTable->setColumnHidden(6,  !dlg.m_bTEFlapAngle);
		m_pctrlFoilTable->setColumnHidden(7,  !dlg.m_bTEXHinge);
		m_pctrlFoilTable->setColumnHidden(8,  !dlg.m_bTEYHinge);
		m_pctrlFoilTable->setColumnHidden(9,  !dlg.m_bLEFlapAngle);
		m_pctrlFoilTable->setColumnHidden(10, !dlg.m_bLEXHinge);
		m_pctrlFoilTable->setColumnHidden(11, !dlg.m_bLEYHinge);
	}
}


/**
 * The user has requested to load a background image in the view.
 */
void QAFoil::OnLoadBackImage()
{
	QString PathName;
	PathName = QFileDialog::getOpenFileName(this, tr("Open Image File"),
											Settings::s_LastDirName,
											"Image files (*.png *.jpg *.bmp)");
	m_bIsImageLoaded = m_BackImage.load(PathName);

	UpdateView();
}


/**
 * The user has requested to clear the background image.
 */
void QAFoil::OnClearBackImage()
{
	m_bIsImageLoaded = false;
	UpdateView();
}


/**
 * The client area has been resized. Update the column widths.
 * @param event the QResizeEvent.
 */
void QAFoil::resizeEvent(QResizeEvent *event)
{
	int ncol = m_pctrlFoilTable->horizontalHeader()->count() - m_pctrlFoilTable->horizontalHeader()->hiddenSectionCount();
	//add 1 to get double width for the name
	ncol++;

	//get column width and spare 10% for horizontal header
	int unitwidth = (int)((double)(m_pctrlFoilTable->width())/(double)ncol/1.1);

	m_pctrlFoilTable->setColumnWidth(0, 2*unitwidth);
	for(int i=1; i<16; i++)	m_pctrlFoilTable->setColumnWidth(i, unitwidth);
	event->accept();
}


/**
 * The user has requested the insertion of a control point in the SplineFoil at the location of the mouse
 */
void QAFoil::OnInsertCtrlPt()
{
	if(Foil::curFoil()) return; // Action can be performed only if the spline foil is selected

	CVector Real = MousetoReal(m_PointDown);

	if(Real.y>=0)
	{
		m_pSF->m_Extrados.InsertPoint(Real.x,Real.y);
		m_pSF->m_Extrados.SplineKnots();
		m_pSF->m_Extrados.SplineCurve();
		m_pSF->UpdateSplineFoil();
	}
	else
	{
		m_pSF->m_Intrados.InsertPoint(Real.x,Real.y);
		m_pSF->m_Intrados.SplineKnots();
		m_pSF->m_Intrados.SplineCurve();
		m_pSF->UpdateSplineFoil();
	}

//	TakePicture();
}


/**
 * The user has requested the deletion of a control point in the SplineFoil at the location of the mouse.
 */
void QAFoil::OnRemoveCtrlPt()
{
	//Removes a point in the spline
	if(Foil::curFoil()) return; // Action can be performed only if the spline foil is selected

	MainFrame *pMainFrame = (MainFrame*)s_pMainFrame;

	CVector Real = MousetoReal(m_PointDown);

	int n =  m_pSF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale);
	if (n>=0)
	{
		if(!m_pSF->m_Extrados.RemovePoint(n))
		{
			QMessageBox::warning(pMainFrame,tr("Warning"), tr("The minimum number of control points has been reached for this spline degree"));
			return;
		}
		m_pSF->m_Extrados.SplineKnots();
		m_pSF->m_Extrados.SplineCurve();
		m_pSF->UpdateSplineFoil();
	}
	else
	{
		int n=m_pSF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale);
		if (n>=0)
		{
			if(!m_pSF->m_Intrados.RemovePoint(n))
			{
				QMessageBox::warning(pMainFrame,tr("Warning"), tr("The minimum number of control points has been reached for this spline degree"));
				return;
			}
			m_pSF->m_Intrados.SplineKnots();
			m_pSF->m_Intrados.SplineCurve();
			m_pSF->UpdateSplineFoil();
		}
	}

//	TakePicture();
}



/** Sets the display font for the Foil table using the default defined in the MainFrame class/ */
void QAFoil::SetTableFont()
{
	m_pctrlFoilTable->setFont(Settings::s_TableFont);
}



Foil* QAFoil::AddNewFoil(Foil *pFoil)
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
		emit projectModified();
		return pFoil;
	}
	return NULL;
}




