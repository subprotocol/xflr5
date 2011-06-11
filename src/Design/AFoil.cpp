/****************************************************************************

	AFoil Class
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
 
#include <QtGui>

#include "../Globals.h"
#include "../MainFrame.h"
#include "../Misc/LinePickerDlg.h"
#include "AFoil.h"
#include "AFoilGridDlg.h"
#include "AFoilTableDlg.h"
#include "SplineCtrlsDlg.h"

extern CFoil *g_pCurFoil;



QAFoil::QAFoil(QWidget *parent)
	: QWidget(parent)
{
	m_hcCross = QCursor(Qt::CrossCursor);
	m_hcMove  = QCursor(Qt::ClosedHandCursor);

	m_StackPos = m_StackSize = 0;

	m_MousePos.x = 0.0;
	m_MousePos.y = 0.0;

	m_poaFoil  = NULL;
	m_pctrlFoilTable = NULL;

	m_pSF = new CSF();
	m_pSF->m_bModified = false;
	m_pSF->InitSplineFoil();

	m_pPF = new CPF();
	m_pPF->m_bModified = false;
	m_pPF->InitSplinedFoil();

	m_bSF          = true;
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

	m_BackImageWidth = m_BackImageHeight = 0;

	memset(&m_TmpPic,0, sizeof(Picture));
	memset(m_UndoPic, 0, MAXPICTURESIZE * sizeof(Picture));

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

	m_pBufferFoil = new CFoil();

	m_CurrentColumn = -1;
	m_StackPos = 0;
	m_StackSize = 0;
	SetupLayout();

	FoilTableDelegate::s_pAFoil = this;
}


void QAFoil::CheckButtons()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	pMainFrame->AFoilDelete->setEnabled(g_pCurFoil);
	pMainFrame->AFoilRename->setEnabled(g_pCurFoil);
	pMainFrame->AFoilExport->setEnabled(g_pCurFoil);
	pMainFrame->ShowCurrentFoil->setEnabled(g_pCurFoil);
	pMainFrame->HideCurrentFoil->setEnabled(g_pCurFoil);

	pMainFrame->AFoilDerotateFoil->setEnabled(g_pCurFoil);
	pMainFrame->AFoilEditCoordsFoil->setEnabled(g_pCurFoil);
	pMainFrame->AFoilInterpolateFoils->setEnabled(g_pCurFoil);
	pMainFrame->AFoilNormalizeFoil->setEnabled(g_pCurFoil);
	pMainFrame->AFoilRefineGlobalFoil->setEnabled(g_pCurFoil);
	pMainFrame->AFoilRefineLocalFoil->setEnabled(g_pCurFoil);
	pMainFrame->AFoilScaleFoil->setEnabled(g_pCurFoil);
	pMainFrame->AFoilSetFlap->setEnabled(g_pCurFoil);
	pMainFrame->AFoilSetLERadius->setEnabled(g_pCurFoil);
	pMainFrame->AFoilSetTEGap->setEnabled(g_pCurFoil);

	pMainFrame->m_pShowLegend->setChecked(m_bShowLegend);
	pMainFrame->SplinesAct->setChecked(m_bSF);
	pMainFrame->SplinedPointsAct->setChecked(!m_bSF);
}



void QAFoil::DrawScale(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect)
{
	int i;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	painter.save();

	painter.setFont(pMainFrame->m_TextFont);

	QFontMetrics fm(pMainFrame->m_TextFont);
	int dD = fm.height();
	int dW = fm.width("0.1");

	int TickSize, xTextOff, offy;

	TickSize = (int)(dD/2);
	xTextOff = 14;
	offy = m_ptOffset.y();

	QPen TextPen(pMainFrame->m_TextColor);
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



void QAFoil::DrawXGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect)
{
	painter.save();
	QPen GridPen(m_XGridColor);
	GridPen.setStyle(GetStyle(m_XGridStyle));
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




void QAFoil::DrawYGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect)
{
	painter.save();
	QPen GridPen(m_YGridColor);
	GridPen.setStyle(GetStyle(m_YGridStyle));
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


void QAFoil::DrawXMinGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect)
{
	painter.save();

	QPen GridPen(m_XMinColor);
	GridPen.setWidth(m_XMinWidth);
	GridPen.setStyle(GetStyle(m_XMinStyle));

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


void QAFoil::DrawYMinGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect)
{
	painter.save();

	QPen GridPen(m_YMinColor);
	GridPen.setWidth(m_YMinWidth);
	GridPen.setStyle(GetStyle(m_YMinStyle));

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



void QAFoil::FillFoilTable()
{
	int i;
	m_pFoilModel->setRowCount(m_poaFoil->size()+1);

	QString name;
	QModelIndex ind;

	double Thickness, xThickness, Camber, xCamber;
	Thickness = xThickness = Camber = xCamber = 0;
	int points = 0;

	if(m_bSF)
	{
		if(m_pSF) 
		{
			name = tr("Spline foil");
			Thickness  = m_pSF->m_fThickness;
			xThickness = m_pSF->m_fxThickMax;
			Camber     = m_pSF->m_fCamber;
			xCamber    = m_pSF->m_fxCambMax;
			points     = m_pSF->m_OutPoints;
		}
	}
	else
	{
		if(m_pPF) 
		{
			name = tr("Splined points foil");
			Thickness  = m_pPF->m_fThickness;
			xThickness = m_pPF->m_fxThickMax;
			Camber     = m_pPF->m_fCamber;
			xCamber    = m_pPF->m_fxCambMax;
			points     =  (m_pPF->m_Extrados.m_iPoints)*(m_pPF->m_Extrados.m_Freq-1)
						 +(m_pPF->m_Intrados.m_iPoints)*(m_pPF->m_Intrados.m_Freq-1);//+1;
		}
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

	ind = m_pFoilModel->index(0,12, QModelIndex());
	if(m_bSF) 
	{
		if(m_pSF->m_bVisible) m_pFoilModel->setData(ind, Qt::Checked, Qt::CheckStateRole);
		else                  m_pFoilModel->setData(ind, Qt::Unchecked, Qt::CheckStateRole);
	}
	else
	{
		if(m_pPF->m_bVisible) m_pFoilModel->setData(ind, Qt::Checked, Qt::CheckStateRole);
		else                  m_pFoilModel->setData(ind, Qt::Unchecked, Qt::CheckStateRole);
	}
	QStandardItem *pItem = m_pFoilModel->item(0,12);
	pItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);

	
	ind = m_pFoilModel->index(0,13, QModelIndex());
	if(m_bSF) 
	{
		if(m_pSF->m_bOutPoints) m_pFoilModel->setData(ind, Qt::Checked, Qt::CheckStateRole);
		else                    m_pFoilModel->setData(ind, Qt::Unchecked, Qt::CheckStateRole);
	}
	else
	{
		if(m_pPF->m_bOutPoints) m_pFoilModel->setData(ind, Qt::Checked, Qt::CheckStateRole);
		else                    m_pFoilModel->setData(ind, Qt::Unchecked, Qt::CheckStateRole);
	}
	pItem = m_pFoilModel->item(0,13);
	pItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);	
	
	
	ind = m_pFoilModel->index(0,14, QModelIndex());
	if(m_bSF) 
	{
		if(m_pSF->m_bCenterLine) m_pFoilModel->setData(ind, Qt::Checked,   Qt::CheckStateRole);
		else                     m_pFoilModel->setData(ind, Qt::Unchecked, Qt::CheckStateRole);
	}
	else 
	{
		if(m_pPF->m_bCenterLine) m_pFoilModel->setData(ind, Qt::Checked,   Qt::CheckStateRole);
		else                     m_pFoilModel->setData(ind, Qt::Unchecked, Qt::CheckStateRole);
	}
	pItem = m_pFoilModel->item(0,14);
	pItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);	


	for(i=0; i<m_poaFoil->size(); i++)
	{
		FillTableRow(i+1);
	}
}


void QAFoil::FillTableRow(int row)
{
	QModelIndex ind;
	QStandardItem *pItem;
	CFoil *pFoil = (CFoil*)m_poaFoil->at(row-1);

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

	ind = m_pFoilModel->index(row, 12, QModelIndex());
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
	if(pItem) pItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
}


void QAFoil::keyPressEvent(QKeyEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
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
				pMainFrame->m_pctrlZoomY->setChecked(false);
				m_bZoomYOnly = false;
			}
			break;
		}
		case Qt::Key_F2:
		{
			OnRenameFoil();
			break;
		}
		case Qt::Key_F4:
		{
			OnStoreSplines();
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


CVector QAFoil::MousetoReal(QPoint &point)
{
	CVector Real;
	
	Real.x =  (point.x() - m_ptOffset.x())/m_fScale;
	Real.y = -(point.y() - m_ptOffset.y())/m_fScale/m_fScaleY;
	Real.z = 0.0;
	
	return Real;
}


void QAFoil::LoadSettings(QSettings *pSettings)
{
	int r,g,b;
	int style, width;
	QColor color;

	pSettings->beginGroup("DirectDesign");
	{
		m_bSF         = pSettings->value("ShowSF").toBool();
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
		r = pSettings->value("XMajColorRed",34).toInt();
		g = pSettings->value("XMajColorGreen",177).toInt();
		b = pSettings->value("XMajColorBlue",234).toInt();
		m_XGridColor = QColor(r,g,b);
		r = pSettings->value("YMajColorRed").toInt();
		g = pSettings->value("YMajColorGreen").toInt();
		b = pSettings->value("YMajColorBlue").toInt();
		m_YGridColor = QColor(r,g,b);
		r = pSettings->value("XMinColorRed").toInt();
		g = pSettings->value("XMinColorGreen").toInt();
		b = pSettings->value("XMinColorBlue").toInt();
		m_XMinColor = QColor(r,g,b);
		r = pSettings->value("YMinColorRed").toInt();
		g = pSettings->value("YMinColorGreen").toInt();
		b = pSettings->value("YMinColorBlue").toInt();
		m_YMinColor = QColor(r,g,b);

		m_NeutralStyle  = pSettings->value("NeutralStyle").toInt();
		m_NeutralWidth  = pSettings->value("NeutralWidth").toInt();
		r = pSettings->value("NeutralColorRed").toInt();
		g = pSettings->value("NeutralColorGreen").toInt();
		b = pSettings->value("NeutralColorBlue").toInt();
		m_NeutralColor = QColor(r,g,b);
		m_bNeutralLine = pSettings->value("NeutralLine").toBool();
		
		style  = pSettings->value("SFStyle").toInt();
		width  = pSettings->value("SFWidth").toInt();
		r = pSettings->value("SFColorRed").toInt();
		g = pSettings->value("SFColorGreen").toInt();
		b = pSettings->value("SFColorBlue").toInt();
		color = QColor(r,g,b);
		m_pSF->SetCurveParams(style, width, color);

		style  = pSettings->value("PFStyle").toInt();
		width  = pSettings->value("PFWidth").toInt();
		r = pSettings->value("PFColorRed").toInt();
		g = pSettings->value("PFColorGreen").toInt();
		b = pSettings->value("PFColorBlue").toInt();
		color = QColor(r,g,b);
		m_pPF->SetCurveParams(style, width, color);

		m_pSF->m_bVisible    = pSettings->value("SFVisible").toBool();
		m_pSF->m_bOutPoints  = pSettings->value("SFOutPoints").toBool();
		m_pSF->m_bCenterLine = pSettings->value("SFCenterLine").toBool();
		m_pPF->m_bVisible    = pSettings->value("PFVisible").toBool();
		m_pPF->m_bOutPoints  = pSettings->value("PFOutPoints").toBool();
		m_pPF->m_bCenterLine = pSettings->value("PFCenterLine").toBool();
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


void QAFoil::mouseDoubleClickEvent (QMouseEvent * event)
{
	if(!hasFocus()) setFocus();

	QPoint point = event->pos();
	QPoint center;
	//translate
	center.rx() = (int)(m_rCltRect.width()/2);
	center.ry() = (int)(m_rCltRect.height()/2);

	m_ptOffset.rx() += -point.x() + center.x();
	m_ptOffset.ry() += -point.y() + center.y();
	m_ViewportTrans.rx() += -point.x() + center.x();
	m_ViewportTrans.ry() += -point.y() + center.y();

	UpdateView();
	return;
}


void QAFoil::mouseMoveEvent(QMouseEvent *event)
{
	if(!hasFocus()) setFocus();
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QPoint point = event->pos();
	m_MousePos = MousetoReal(point);
	CVector Real = m_MousePos;

	bool bCtrl = false;
	if(event->modifiers() & Qt::ControlModifier) bCtrl =true;

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
			if(m_bSF)
			{
				int n = m_pSF->m_Extrados.m_iSelect;
				if (n>=0 && n<=m_pSF->m_Extrados.m_iCtrlPoints) 
				{
					if(!m_bStored) StorePicture();//save for undo only the first time
//					if(n==1) m_MousePos.x = 0.0;// we can't move point 1 for vertical slope
					m_pSF->m_Extrados.m_Input[n].x = m_MousePos.x;
					m_pSF->m_Extrados.m_Input[n].y = m_MousePos.y;
					m_pSF->Update(true);
					if(m_pSF->m_bSymetric)
					{
						m_pSF->m_Intrados.m_Input[n].x = m_MousePos.x;
						m_pSF->m_Intrados.m_Input[n].y = -m_MousePos.y;
						m_pSF->Update(false);
					}
					m_pSF->m_bModified = true;
					pMainFrame->SetSaveState(false);
				}
				else
				{
					int n = m_pSF->m_Intrados.m_iSelect;
					if (n>=0 && n<=m_pSF->m_Intrados.m_iCtrlPoints)
					{
						if(!m_bStored) StorePicture();//save for undo only the first time

						m_pSF->m_Intrados.m_Input[n].x = m_MousePos.x;
						m_pSF->m_Intrados.m_Input[n].y = m_MousePos.y;
						m_pSF->Update(false);

						if(m_pSF->m_bSymetric)
						{
							m_pSF->m_Extrados.m_Input[n].x =  m_MousePos.x;
							m_pSF->m_Extrados.m_Input[n].y = -m_MousePos.y;
							m_pSF->Update(true);
						}
						m_pSF->m_bModified = true;
						pMainFrame->SetSaveState(false);
					}
				}
			}
			else
			{
				int n = m_pPF->m_Extrados.m_iSelect;
				if (n>=0 && n<m_pPF->m_Extrados.m_iPoints) 
				{
					if(!m_bStored) StorePicture();//save for undo only the first time
					m_pPF->m_Extrados.m_ctrlPoint[n].x = m_MousePos.x;
					m_pPF->m_Extrados.m_ctrlPoint[n].y = m_MousePos.y;
					if(m_pPF->m_bSymetric)
					{
						m_pPF->m_Intrados.m_ctrlPoint[n].x =  m_MousePos.x;
						m_pPF->m_Intrados.m_ctrlPoint[n].y = -m_MousePos.y;
					}
					m_pPF->m_bModified = true;
					pMainFrame->SetSaveState(false);
				}
				else
				{
					n = m_pPF->m_Intrados.m_iSelect;
					if (n>=0 && n<m_pPF->m_Intrados.m_iPoints)
					{
						if(!m_bStored) StorePicture();//save for undo only the first time
						m_pPF->m_Intrados.m_ctrlPoint[n].x = m_MousePos.x;
						m_pPF->m_Intrados.m_ctrlPoint[n].y = m_MousePos.y;
						m_pPF->m_bModified = true;
						pMainFrame->SetSaveState(false);
						if(m_pPF->m_bSymetric)
						{
							m_pPF->m_Extrados.m_ctrlPoint[n].x =  m_MousePos.x;
							m_pPF->m_Extrados.m_ctrlPoint[n].y = -m_MousePos.y;
						}
					}
				}
				if(n<0) 
				{
					//check for rear point
					if(m_pPF->m_Extrados.m_iSelect == -1)
					{
						if(!m_bStored) StorePicture();//save for undo only the first time
						m_pPF->m_Extrados.m_RearPoint.x = m_MousePos.x;
						m_pPF->m_Extrados.m_RearPoint.y = m_MousePos.y;
						if(m_pPF->m_bSymetric)
						{
							m_pPF->m_Intrados.m_RearPoint.x =  m_MousePos.x;
							m_pPF->m_Intrados.m_RearPoint.y = -m_MousePos.y;
						}
						m_pPF->m_bModified = true;
						pMainFrame->SetSaveState(false);
						m_pPF->Update();
					}
					else if(m_pPF->m_Intrados.m_iSelect == -1)
					{
						if(!m_bStored) StorePicture();//save for undo only the first time
						m_pPF->m_Intrados.m_RearPoint.x = m_MousePos.x;
						m_pPF->m_Intrados.m_RearPoint.y = m_MousePos.y;
						m_pPF->m_bModified = true;
						if(m_pPF->m_bSymetric)
						{
							m_pPF->m_Extrados.m_RearPoint.x =  m_MousePos.x;
							m_pPF->m_Extrados.m_RearPoint.y = -m_MousePos.y;
						}
												pMainFrame->SetSaveState(false);
						m_pPF->Update();
					}
				}
				m_pPF->CompMidLine();
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

		if(m_bSF && m_pSF->m_bVisible)
		{
			int n = m_pSF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale);
			if (n>0 && n<m_pSF->m_Extrados.m_iCtrlPoints)
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
			if (n>0 && n<m_pSF->m_Intrados.m_iCtrlPoints)
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
		else if (m_pPF->m_bVisible)
		{
			bool bFound = false;
			int n = m_pPF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale);
			if (n>0 && n<=m_pPF->m_Extrados.m_iPoints)
			{
				m_pPF->m_Extrados.m_iHighlight = n;
				bFound = true;
			}
			else if(m_pPF->m_Extrados.IsRearPoint(Real, m_fScale/m_fRefScale) == -1)
			{
				m_pPF->m_Extrados.m_iHighlight = -1;
				bFound = true;
			}

			n = m_pPF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale);
			if (n>0 && n<=m_pPF->m_Intrados.m_iPoints)
			{
				m_pPF->m_Intrados.m_iHighlight = n;
				bFound = true;
			}
			else if(m_pPF->m_Intrados.IsRearPoint(Real, m_fScale/m_fRefScale) == -1)
			{
				m_pPF->m_Intrados.m_iHighlight = -1;
				bFound = true;
			}
			if(!bFound)
			{
				m_pPF->m_Extrados.m_iHighlight = -10;
				m_pPF->m_Intrados.m_iHighlight = -10;
			}
		}
	}
	UpdateView();
}


void QAFoil::mousePressEvent(QMouseEvent *event)
{
	QPoint point = event->pos();
	TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;
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
		if(m_bSF)
		{
			if (event->modifiers() & Qt::ShiftModifier)
			{
				//shift --> removes the point
				TakePicture();
				StorePicture();
				int n =  m_pSF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale);
				if (n>=0) 
				{
					m_pSF->m_Extrados.RemovePoint(n);
					m_pSF->Update(true);
				}
				else 
				{
					int n=m_pSF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale);
					if (n>=0)
					{
						m_pSF->m_Intrados.RemovePoint(n);
						m_pSF->Update(false);
					}
				}
			}
			else if (event->modifiers() & Qt::ControlModifier) 
			{
				//Ctrl --> inserts a point
				TakePicture();
				StorePicture();

				if(Real.y>=0) 
				{
					m_pSF->m_Extrados.InsertPoint(Real.x,Real.y);
					m_pSF->Update(true);
				}
				else 
				{
					m_pSF->m_Intrados.InsertPoint(Real.x,Real.y);
					m_pSF->Update(false);
				}
			}
			else
			{ 
				//Selects the point
			
				m_pSF->m_Extrados.m_iSelect = m_pSF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale);
				m_pSF->m_Intrados.m_iSelect = m_pSF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale);
				if (m_pSF->m_Extrados.m_iSelect>=0 || m_pSF->m_Intrados.m_iSelect>=0)
				{
					TakePicture();
				}

				if(m_pSF->m_Extrados.m_iSelect ==-10 && m_pSF->m_Intrados.m_iSelect ==-10)
				{
					p2DWidget->setCursor(m_hcMove);
					m_bTrans = true;
				}
			}
		}
		else
		{
			if (event->modifiers() & Qt::ShiftModifier) 
			{
				//shift --> removes the point
				TakePicture();
				StorePicture();
				int n =  m_pPF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale);
				if (n>=0) 
				{
					m_pPF->m_Extrados.RemovePoint(n);
					m_pPF->Update();
				}
				else 
				{
					int n=m_pPF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale);
					if (n>=0) m_pPF->m_Intrados.RemovePoint(n);
					m_pPF->Update();
				}
			}
			else if (event->modifiers() & Qt::ControlModifier) 
			{
				//Ctrl --> inserts a point
				TakePicture();
				StorePicture();
				if(Real.y>=0) 
				{
					m_pPF->m_Extrados.InsertPoint(Real.x, Real.y);
					m_pPF->Update();
				}
				else 
				{
					m_pPF->m_Intrados.InsertPoint(Real.x, Real.y);
					m_pPF->Update();
				}
				
			}
			else
			{ 
				//Selects the point
				m_pPF->m_Extrados.m_iSelect = m_pPF->m_Extrados.IsControlPoint(Real, m_fScale/m_fRefScale) ;
				int n = m_pPF->m_Extrados.IsRearPoint(Real, m_fScale/m_fRefScale) ;
				if(n==-1) m_pPF->m_Extrados.m_iSelect = n;

				m_pPF->m_Intrados.m_iSelect = m_pPF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale) ;
				n = m_pPF->m_Intrados.IsRearPoint(Real, m_fScale/m_fRefScale) ;
				if(n==-1) m_pPF->m_Intrados.m_iSelect = n;

				TakePicture();

				if(m_pPF->m_Extrados.m_iSelect ==-10 && m_pPF->m_Intrados.m_iSelect ==-10)
				{
					p2DWidget->setCursor(m_hcMove);
					m_bTrans = true;
				}
			}
		}
	}
	UpdateView();	

}


void QAFoil::mouseReleaseEvent(QMouseEvent *event)
{
	m_bTrans = false;
	QPoint point = event->pos();

	TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;

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

	else 
	{
		if(m_bSF)
		{
			m_pSF->CompMidLine();
		}
		else
		{
			m_pPF->CompMidLine();
		}
		p2DWidget->setCursor(m_hcCross);
	}

	UpdateView();
}




void QAFoil::OnAFoilDerotateFoil()
{
	if(!g_pCurFoil) return;

	m_pBufferFoil->CopyFoil(g_pCurFoil);
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = g_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();

	double angle = m_pBufferFoil->DeRotate();
	QString str = QString(tr("Foil has been de-rotated by %1 degrees")).arg(angle,6,'f',3);
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->statusBar()->showMessage(str);

	//then duplicate the buffer foil and add it
	CFoil *pNewFoil = new CFoil();
	pNewFoil->CopyFoil(m_pBufferFoil);
	pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
	pNewFoil->m_nFoilStyle = 0;
	pNewFoil->m_nFoilWidth = 1;

	CFoil * pFoil = pMainFrame->SetModFoil(pNewFoil);
	FillFoilTable();
	SelectFoil(pFoil);

	m_pBufferFoil->m_bVisible = false;

	UpdateView();
}


void QAFoil::OnAFoilNormalizeFoil()
{
	if(!g_pCurFoil) return;
	double length = g_pCurFoil->NormalizeGeometry();
	g_pCurFoil->InitFoil();
	QString str = QString(tr("Foil has been normalized from %1  to 1.000")).arg(length,7,'f',3);

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->statusBar()->showMessage(str);

	UpdateView();

}

void QAFoil::OnAFoilCadd()
{
	if(!g_pCurFoil) return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pBufferFoil->CopyFoil(g_pCurFoil);
	m_pBufferFoil->m_FoilName   = g_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;
	m_pBufferFoil->m_bPoints    = true;
	m_pBufferFoil->m_bVisible   = true;

	UpdateView();

	m_CAddDlg.m_pBufferFoil = m_pBufferFoil;
	m_CAddDlg.m_pMemFoil    = g_pCurFoil;
	m_CAddDlg.m_pXDirect    = NULL;
	m_CAddDlg.m_pAFoil      = this;
	m_CAddDlg.move(pMainFrame->m_DlgPos);
	m_CAddDlg.InitDialog();

	if(QDialog::Accepted == m_CAddDlg.exec())
	{
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;


		CFoil * pFoil = pMainFrame->SetModFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pFoil);
	}
	else
	{
		FillFoilTable();
		SelectFoil(g_pCurFoil);
		m_pXFoil->m_FoilName ="";

	}
	pMainFrame->m_DlgPos = m_CAddDlg.pos();
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


void QAFoil::OnAFoilLECircle()
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_LECircleDlg.m_Radius      = m_LERad;
	m_LECircleDlg.m_bShowRadius = m_bLECircle;
	m_LECircleDlg.m_pAFoil      = this;
	m_LECircleDlg.move(pMainFrame->m_DlgPos);
	m_LECircleDlg.InitDialog();

	if(m_LECircleDlg.exec()==QDialog::Accepted)
	{
		m_LERad = m_LECircleDlg.m_Radius;
		m_bLECircle = m_LECircleDlg.m_bShowRadius;
	}
	pMainFrame->m_DlgPos = m_LECircleDlg.pos();
	UpdateView();
}


void QAFoil::OnAFoilPanels()
{
	if(!g_pCurFoil) return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pBufferFoil->CopyFoil(g_pCurFoil);
	m_pBufferFoil->m_FoilName  = g_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;
	m_pBufferFoil->m_bPoints   = true;
	m_pBufferFoil->m_bVisible  = true;

	UpdateView();

	m_TwoDPanelDlg.m_pBufferFoil = m_pBufferFoil;
	m_TwoDPanelDlg.m_pMemFoil    = g_pCurFoil;
	m_TwoDPanelDlg.m_pXDirect    = NULL;
	m_TwoDPanelDlg.m_pAFoil      = this;
	m_TwoDPanelDlg.move(pMainFrame->m_DlgPos);
	m_TwoDPanelDlg.InitDialog();

	if(QDialog::Accepted == m_TwoDPanelDlg.exec())
	{
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;

		CFoil * pFoil = pMainFrame->SetModFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pFoil);
	}
	else
	{
		FillFoilTable();
		SelectFoil(g_pCurFoil);
//		m_pXFoil->m_FoilName ="";

	}

	pMainFrame->m_DlgPos = m_TwoDPanelDlg.pos();
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


void QAFoil::OnAFoilFoilCoordinates()
{
	if(!g_pCurFoil) return;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pBufferFoil->CopyFoil(g_pCurFoil);
	m_pBufferFoil->m_bPoints  = true;
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = g_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();

	m_FoilCoordDlg.move(pMainFrame->m_DlgPos);
	m_FoilCoordDlg.m_pMemFoil    = g_pCurFoil;
	m_FoilCoordDlg.m_pBufferFoil = m_pBufferFoil;
	m_FoilCoordDlg.m_pXDirect    = NULL;
	m_FoilCoordDlg.m_pAFoil      = this;
	m_FoilCoordDlg.move(pMainFrame->m_DlgPos);
	m_FoilCoordDlg.InitDialog();

	if(QDialog::Accepted == m_FoilCoordDlg.exec())
	{
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_iHighLight = -1;

		CFoil * pFoil = pMainFrame->SetModFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pFoil);
	}
	else
	{
		FillFoilTable();
		SelectFoil(g_pCurFoil);
		m_pXFoil->m_FoilName ="";
	}
	pMainFrame->m_DlgPos = m_FoilCoordDlg.pos();
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


void QAFoil::OnAFoilFoilGeom()
{
	if(!g_pCurFoil) return;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pBufferFoil->CopyFoil(g_pCurFoil);
	m_pBufferFoil->m_bPoints  = true;
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = g_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();

	FoilGeomDlg dlg;
	dlg.m_pMemFoil    = g_pCurFoil;
	dlg.m_pBufferFoil = m_pBufferFoil;
	dlg.m_pAFoil      = this;
	dlg.m_pXDirect    = NULL;
	dlg.move(pMainFrame->m_DlgPos);
	dlg.InitDialog();

	if(QDialog::Accepted == dlg.exec())
	{
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;

		CFoil * pFoil = pMainFrame->SetModFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pFoil);
	}
	else
	{
		FillFoilTable();
		SelectFoil(g_pCurFoil);
//		m_pXFoil->m_FoilName ="";
	}
	pMainFrame->m_DlgPos = dlg.pos();
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}



void QAFoil::OnAFoilSetTEGap()
{
	if(!g_pCurFoil) return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pBufferFoil->CopyFoil(g_pCurFoil);
	m_pBufferFoil->m_bPoints    = false;
	m_pBufferFoil->m_bVisible   = true;
	m_pBufferFoil->m_FoilName   = g_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();

	m_TEGapDlg.m_pBufferFoil = m_pBufferFoil;
	m_TEGapDlg.m_pMemFoil    = g_pCurFoil;
	m_TEGapDlg.m_pXDirect    = NULL;
	m_TEGapDlg.m_pAFoil      = this;
	m_TEGapDlg.move(pMainFrame->m_DlgPos);
	m_TEGapDlg.InitDialog();

	if(QDialog::Accepted == m_TEGapDlg.exec())
	{
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints    = false;

		CFoil * pFoil = pMainFrame->SetModFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pFoil);
	}
	else
	{
		FillFoilTable();
		SelectFoil(g_pCurFoil);
//		m_pXFoil->m_FoilName ="";
		//to un-initialize XFoil in case user switches to XInverse
		//Thanks Jean-Marc !
	}

	pMainFrame->m_DlgPos = m_TEGapDlg.pos();
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


void QAFoil::OnAFoilSetLERadius()
{
	if(!g_pCurFoil) return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pBufferFoil->CopyFoil(g_pCurFoil);
	m_pBufferFoil->m_bVisible   = true;
	m_pBufferFoil->m_bPoints    = false;
	m_pBufferFoil->m_FoilName   = g_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();

	m_LEDlg.m_pBufferFoil = m_pBufferFoil;
	m_LEDlg.m_pMemFoil    = g_pCurFoil;
	m_LEDlg.m_pXDirect    = NULL;
	m_LEDlg.m_pAFoil      = this;
	m_LEDlg.move(pMainFrame->m_DlgPos);
	m_LEDlg.InitDialog();

	if(QDialog::Accepted == m_LEDlg.exec())
	{
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints    = false;

		CFoil * pFoil = pMainFrame->SetModFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pFoil);
	}
	else
	{
		FillFoilTable();
		SelectFoil(g_pCurFoil);
//		m_pXFoil->m_FoilName ="";

	}

	pMainFrame->m_DlgPos = m_LEDlg.pos();
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


void QAFoil::OnAFoilInterpolateFoils()
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	if(m_poaFoil->size()<2)
	{
		QMessageBox::warning(pMainFrame,tr("Warning"), tr("At least two foils are required"));
		return;
	}

	if(!g_pCurFoil) SelectFoil();
	if(!g_pCurFoil) return;
	m_pBufferFoil->CopyFoil(g_pCurFoil);
	m_pBufferFoil->m_FoilName  = g_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;
	m_pBufferFoil->m_bPoints   = false;
	m_pBufferFoil->m_bVisible  = true;

	UpdateView();

	m_InterpolateFoilsDlg.m_poaFoil = m_poaFoil;
	m_InterpolateFoilsDlg.m_pBufferFoil = m_pBufferFoil;
	m_InterpolateFoilsDlg.m_pMainFrame = m_pMainFrame;
	m_InterpolateFoilsDlg.m_pXDirect  = NULL;
	m_InterpolateFoilsDlg.m_pAFoil    = this;
	m_InterpolateFoilsDlg.move(pMainFrame->m_DlgPos);
	m_InterpolateFoilsDlg.InitDialog();

	if(QDialog::Accepted == m_InterpolateFoilsDlg.exec())
	{
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilName = m_InterpolateFoilsDlg.m_NewFoilName;

		CFoil * pFoil = pMainFrame->SetModFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pFoil);

	}

	else
	{
		FillFoilTable();
		SelectFoil(g_pCurFoil);
	}
	pMainFrame->m_DlgPos = m_InterpolateFoilsDlg.pos();
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


void QAFoil::OnAFoilNacaFoils()
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pBufferFoil->SetNaca009();
	m_pBufferFoil->m_bPoints  = true;
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = "Naca xxxx";
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();

	m_NacaFoilDlg.m_pBufferFoil = m_pBufferFoil;
	m_NacaFoilDlg.m_pXDirect = NULL;
	m_NacaFoilDlg.m_pAFoil = this;
	m_NacaFoilDlg.move(pMainFrame->m_DlgPos);

	if(QDialog::Accepted == m_NacaFoilDlg.exec())
	{
		//then duplicate the buffer foil and add it
		QString str;

		if(m_NacaFoilDlg.s_Digits>0 && log10((double)m_NacaFoilDlg.s_Digits)<4)
			str = QString("%1").arg(m_NacaFoilDlg.s_Digits,4,10,QChar('0'));
		else
			str = QString("%1").arg(m_NacaFoilDlg.s_Digits);
		str = "NACA "+ str;

		CFoil *pNewFoil    = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints    = false;
		pNewFoil->m_FoilName   = str;

		CFoil * pFoil = pMainFrame->SetModFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pFoil);
	}
	else
	{
		FillFoilTable();;
		if(g_pCurFoil) SelectFoil(g_pCurFoil);
		m_pXFoil->m_FoilName ="";

	}

	pMainFrame->m_DlgPos = m_NacaFoilDlg.pos();
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


void QAFoil::OnAFoilSetFlap()
{
	if(!g_pCurFoil) return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pBufferFoil->CopyFoil(g_pCurFoil);
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = g_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();

	m_FlapDlg.m_pAFoil      = this;
	m_FlapDlg.m_pXDirect    = NULL;
	m_FlapDlg.m_pXFoil      = m_pXFoil;
	m_FlapDlg.m_pMemFoil    = g_pCurFoil;
	m_FlapDlg.m_pBufferFoil = m_pBufferFoil;
	m_FlapDlg.move(pMainFrame->m_DlgPos);
	m_FlapDlg.InitDialog();

	if(QDialog::Accepted == m_FlapDlg.exec())
	{
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;

		CFoil * pFoil = pMainFrame->SetModFoil(pNewFoil);
		FillFoilTable();
		SelectFoil(pFoil);
	}
	else
	{
		FillFoilTable();
		SelectFoil(g_pCurFoil);
		m_pXFoil->m_FoilName ="";
	}
	pMainFrame->m_DlgPos = m_FlapDlg.pos();
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}



void QAFoil::OnDelete()
{
	if(!g_pCurFoil) return;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->DeleteFoil(g_pCurFoil);
	FillFoilTable();
	SelectFoil();
	UpdateView();
}


void QAFoil::OnDuplicate()
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	if(!g_pCurFoil) return;
	CFoil *pNewFoil = new CFoil;
	pNewFoil->CopyFoil(g_pCurFoil);
	pNewFoil->InitFoil();

	if(pMainFrame->SetModFoil(pNewFoil))
	{
		FillFoilTable();
		SelectFoil(pNewFoil);
	}
	else
	{
		FillFoilTable();
		SelectFoil(NULL);
	}
}


void QAFoil::OnExportCurFoil()
{
	if(!g_pCurFoil)	return;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString FileName;

	FileName = g_pCurFoil->m_FoilName;
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

	g_pCurFoil->ExportFoil(out);
	XFile.close();
}


void QAFoil::OnExportSplinesToFile()
{
	QString FoilName = tr("Spline Foil");
	QString FileName, strong;
	QString strOut;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	// deselect points so as not to interfere with other mouse commands
	m_pSF->m_Intrados.m_iSelect = -10;
	m_pSF->m_Extrados.m_iSelect = -10;
	m_pPF->m_Intrados.m_iSelect = -10;
	m_pPF->m_Extrados.m_iSelect = -10;

	//check that the number of output points is consistent with the array's size
	if(m_bSF)
	{
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
	}
	else
	{
		int size = m_pPF->m_Extrados.m_iPoints * (m_pPF->m_Extrados.m_Freq-1) ;//+ 1;
		if(size>IQX2)
		{
			strong = QString(tr("Too many output points on upper surface\n Max =%1")).arg(IQX2);
			QMessageBox::warning(pMainFrame, tr("Warning"), strong, QMessageBox::Ok);
			return;
		}
		size = m_pPF->m_Intrados.m_iPoints * (m_pPF->m_Intrados.m_Freq-1) ;//+ 1;
		if(size>IQX2)
		{
			strong = QString(tr("Too many output points on lower surface\n Max =%1")).arg(IQX2);
			QMessageBox::warning(pMainFrame, tr("Warning"), strong, QMessageBox::Ok);
			return;
		}
	}

	QFile DestFile;

	FileName.replace("/", " ");
	FileName = QFileDialog::getSaveFileName(this, tr("Export Splines"), 
				pMainFrame->m_LastDirName,
				tr("Text File (*.dat)"));

	if(!FileName.length()) return;
	int pos;
	pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);


	strOut = FoilName + "\n";
	out << strOut;
	if(m_bSF)
	{
//				XFile.WriteString("1\n");
		m_pSF->ExportToFile(out);
	}
	else
	{
//				XFile.WriteString("2\n");
		m_pPF->ExportToFile(out);
	}
	XFile.close();
}


void QAFoil::FoilVisibleClicked(const QModelIndex& index)
{
	if(index.row()>=m_poaFoil->size()+1) return;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QStandardItem *pItem = m_pFoilModel->item(index.row(),0);

	if(index.row()>0)
	{
		CFoil *pFoil= pMainFrame->GetFoil(pItem->text());
		g_pCurFoil = pFoil;
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
	else if(index.row()==0)
	{
		g_pCurFoil = NULL;
		if(index.column()==12)
		{
			if(m_bSF) m_pSF->m_bVisible = !m_pSF->m_bVisible;
			else      m_pPF->m_bVisible = !m_pPF->m_bVisible;

		}
		else if(index.column()==13)
		{
			if(m_bSF) m_pSF->m_bOutPoints = !m_pSF->m_bOutPoints;
			else     
			{
				m_pPF->m_bOutPoints = !m_pPF->m_bOutPoints;
				m_pPF->SetOutPoints(m_pPF->m_bOutPoints);
			}
		}
		else if(index.column()==14)
		{
			if(m_bSF) m_pSF->m_bCenterLine = !m_pSF->m_bCenterLine;
			else      m_pPF->m_bCenterLine = !m_pPF->m_bCenterLine;

		}
	}
	UpdateView();
}


void QAFoil::OnFoilClicked(const QModelIndex& index)
{
	if(index.row()>=m_poaFoil->size()+1) return;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QStandardItem *pItem = m_pFoilModel->item(index.row(),0);
	
	if(index.row()>0)
	{
		CFoil *pFoil= pMainFrame->GetFoil(pItem->text());
		g_pCurFoil = pFoil;
	}
	else if(index.row()==0)
	{
		g_pCurFoil = NULL;
	}
	if(index.column()==15) OnFoilStyle();
	CheckButtons();
}


void QAFoil::OnFoilStyle()
{
	if(!g_pCurFoil)
	{
		LinePickerDlg dlg;
		if(m_bSF) dlg.InitDialog(m_pSF->m_FoilStyle, m_pSF->m_FoilWidth, m_pSF->m_FoilColor);
		else      dlg.InitDialog(m_pPF->m_FoilStyle, m_pPF->m_FoilWidth, m_pPF->m_FoilColor);

		if(QDialog::Accepted==dlg.exec())
		{
			if(m_bSF)
			{
				m_pSF->SetCurveParams(dlg.GetStyle(), dlg.GetWidth(), dlg.GetColor());
			}
			else
			{
				m_pPF->SetCurveParams(dlg.GetStyle(), dlg.GetWidth(), dlg.GetColor());
			}
			UpdateView();
		}
	}
	else
	{
		LinePickerDlg dlg;
		dlg.InitDialog(g_pCurFoil->m_nFoilStyle, g_pCurFoil->m_nFoilWidth, g_pCurFoil->m_FoilColor);

		if(QDialog::Accepted==dlg.exec())
		{
			g_pCurFoil->m_nFoilStyle = dlg.GetStyle();
			g_pCurFoil->m_nFoilWidth = dlg.GetWidth();
			g_pCurFoil->m_FoilColor = dlg.GetColor();
			UpdateView();
		}
	}
}


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




void QAFoil::OnHideAllFoils()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->SetSaveState(false);
	CFoil*pFoil;
	for (int k=0; k<m_poaFoil->size(); k++)
	{
		pFoil = (CFoil*)m_poaFoil->at(k);
		pFoil->m_bVisible = false;
	}
	FillFoilTable();
	UpdateView();
}



void QAFoil::OnHideCurrentFoil()
{
	if(!g_pCurFoil) return;
	ShowFoil(g_pCurFoil, false);
	UpdateView();

}



void QAFoil::OnNewSplines()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(m_bSF) 
	{
		if(m_pSF->m_bModified)
		{
			if (QMessageBox::Yes != QMessageBox::question(pMainFrame, tr("Question"), tr("Discard changes to Splines ?"),
														  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))
			{
				return;
			}
		}
		m_pSF->InitSplineFoil();
		TakePicture();
		StorePicture();
	}
	else      
	{
		if(m_pPF->m_bModified)
		{
			if (QMessageBox::Yes != QMessageBox::question(pMainFrame, tr("Question"), tr("Discard changes to Splines ?"),
														  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))
			{
				return;
			}
		}
		m_pPF->InitSplinedFoil();
		TakePicture();
		StorePicture();
	}
	

	m_StackPos  = 0;
	m_StackSize = 0;

	pMainFrame->SetSaveState(false);
	UpdateView();
}



void QAFoil::OnRedo()
{
	if(m_StackPos<m_StackSize-1)
	{
		m_StackPos++;
		SetPicture();
	}
}


void QAFoil::OnRenameFoil()
{
	if(!g_pCurFoil) return;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->OnRenameCurFoil();
	FillFoilTable();
}


void QAFoil::OnShowAllFoils()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->SetSaveState(false);
	CFoil*pFoil;
	for (int k=0; k<m_poaFoil->size(); k++)
	{
		pFoil = (CFoil*)m_poaFoil->at(k);
		pFoil->m_bVisible = true;
	}
	FillFoilTable();
	UpdateView();
}



void QAFoil::OnShowCurrentFoil()
{
	if(!g_pCurFoil) return;
	ShowFoil(g_pCurFoil, true);
	UpdateView();

}



void QAFoil::OnShowLegend()
{
	m_bShowLegend = !m_bShowLegend;
	UpdateView();
	CheckButtons();
}



void QAFoil::OnStoreSplines()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(m_bSF)
	{
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


		CFoil *pNewFoil = new CFoil();
		m_pSF->ExportToBuffer(pNewFoil);

		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilName = "";
		if(pMainFrame->SetModFoil(pNewFoil))
		{
			g_pCurFoil = NULL;
			FillFoilTable();
			SelectFoil(pNewFoil);
		}
		else
		{
			FillFoilTable();
			SelectFoil();
		}
	}
	else
	{
		int size = m_pPF->m_Extrados.m_iPoints * (m_pPF->m_Extrados.m_Freq-1) ;//+ 1;
		if(size>IQX2)
		{
			QString strong = QString(tr("Too many output points on upper surface\n Max =%1")).arg(IQX2);
			QMessageBox::warning(pMainFrame, tr("Warning"), strong, QMessageBox::Ok);
			return;
		}

		size = m_pPF->m_Intrados.m_iPoints * (m_pPF->m_Intrados.m_Freq-1) ;//+ 1;
		if(size>IQX2)
		{
			QString strong = QString(tr("Too many output points on lower surface\n Max =%1")).arg(IQX2);
			QMessageBox::warning(pMainFrame, tr("Warning"), strong, QMessageBox::Ok);
			return;
		}

		CFoil *pNewFoil = new CFoil();
		m_pPF->ExportToBuffer(pNewFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilName = "";
		if(pMainFrame->SetModFoil(pNewFoil))
		{
			g_pCurFoil = NULL;
			FillFoilTable();
			SelectFoil(pNewFoil);
		}
/*		else
		{
			delete pNewFoil;
			pNewFoil = NULL;
			FillFoilTable();
			SelectFoil();
		}*/
	}
	UpdateView();
}


void QAFoil::OnResetXScale()
{
	SetScale();
	ReleaseZoom();
	UpdateView();
}


void QAFoil::OnResetYScale()
{
	m_fScaleY = 1.0;
	UpdateView();
}


void QAFoil::OnResetScales()
{
	m_fScaleY = 1.0;
	SetScale();
	ReleaseZoom();
	UpdateView();
}

void QAFoil::OnSplineControls()
{
	SplineCtrlsDlg dlg;
	dlg.m_pSF = m_pSF;
	dlg.m_pPF = m_pPF;
	dlg.m_bSF = m_bSF;
	dlg.InitDialog();

	if(dlg.exec() == QDialog::Accepted)
	{
		TakePicture();
		StorePicture();

		if(m_bSF) m_pSF->Copy(&dlg.m_SF);
		else      m_pPF->Copy(&dlg.m_PF);
	}
}



void QAFoil::OnSplines()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	m_bSF = true;
	m_StackSize = 0;
	m_StackPos = 0;
	TakePicture();
	StorePicture();
	pMainFrame->SplinesAct->setChecked(m_bSF);
	pMainFrame->SplinedPointsAct->setChecked(!m_bSF);
	FillFoilTable();
	UpdateView();
}


void QAFoil::OnSplinedPoints()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	m_bSF = false;
	m_StackSize = 0;
	m_StackPos = 0;
	TakePicture();
	StorePicture();
	pMainFrame->SplinesAct->setChecked(m_bSF);
	pMainFrame->SplinedPointsAct->setChecked(!m_bSF);
	FillFoilTable();
	UpdateView();
}


void QAFoil::OnUndo()
{
	if(m_StackPos>0)
	{
		if(m_StackPos == m_StackSize)
		{
			//if we're at the first undo command, save current state
			TakePicture();
			StorePicture();//in case we redo
			m_StackPos--;
		}
		m_StackPos--;
		SetPicture();
	}
	else
	{
		m_StackPos = 0;
	}
}



void QAFoil::OnZoomIn()
{
	if(!m_bZoomPlus)
	{
		if(m_fScale/m_fRefScale <32.0)
		{
			m_bZoomPlus = true;
			MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
			pMainFrame->m_pctrlZoomIn->setChecked(true);

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


void QAFoil::OnZoomYOnly()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	TwoDWidget *p2Dwidget = (TwoDWidget*)m_p2DWidget;
	m_bZoomYOnly = !m_bZoomYOnly;
	if(m_bZoomYOnly)	pMainFrame->m_pctrlZoomY->setChecked(true);
	else				pMainFrame->m_pctrlZoomY->setChecked(false);
	p2Dwidget->setFocus();
}



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


void QAFoil::PaintGrids(QPainter &painter)
{
	painter.save();
//	TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;
	QColor color;
	int style, width;

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
		color = m_NeutralColor;
		style = m_NeutralStyle;
		width = m_NeutralWidth;
		QPen NPen(m_NeutralColor);
		NPen.setStyle(GetStyle(m_NeutralStyle));
		NPen.setWidth(m_NeutralWidth);
		painter.setPen(NPen);

		painter.drawLine(m_rCltRect.right(),m_ptOffset.y(), m_rCltRect.left(),m_ptOffset.y());
	}

	//draw grids
	if(m_bXGrid)	DrawXGrid(painter, m_fScale, m_fScale*m_fScaleY, m_ptOffset, m_rCltRect);
	if(m_bYGrid)	DrawYGrid(painter, m_fScale, m_fScale*m_fScaleY, m_ptOffset, m_rCltRect);
	if(m_bXMinGrid) DrawXMinGrid(painter, m_fScale, m_fScale*m_fScaleY, m_ptOffset, m_rCltRect);
	if(m_bYMinGrid) DrawYMinGrid(painter, m_fScale, m_fScale*m_fScaleY, m_ptOffset, m_rCltRect);

	if(m_bScale) DrawScale(painter, m_fScale, m_fScale*m_fScaleY, m_ptOffset, m_rCltRect);

	painter.restore();
}



void QAFoil::PaintLegend(QPainter &painter)
{
	painter.save();
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	painter.setFont(pMainFrame->m_TextFont);

	if(m_bShowLegend)
	{
		CFoil* pRefFoil;
		QString strong;
		QPoint Place(m_rCltRect.right()-250, 10);
		int LegendSize, ypos, x1, n, k, delta;

		LegendSize = 20;
		ypos = 15;
		delta = 5;

		painter.setBackgroundMode(Qt::TransparentMode);

		QPen TextPen(pMainFrame->m_TextColor);
		painter.setPen(TextPen);
		QPen LegendPen;

		k=0;
		if(m_bSF)
		{
			if(m_pSF->m_bVisible)
			{
				LegendPen.setColor(m_pSF->m_FoilColor);
				LegendPen.setStyle(GetStyle(m_pSF->m_FoilStyle));
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
		}
		else
		{
			if(m_pPF->m_bVisible)
			{
				LegendPen.setColor(m_pPF->m_FoilColor);
				LegendPen.setStyle(GetStyle(m_pPF->m_FoilStyle));
				LegendPen.setWidth(m_pPF->m_FoilWidth);

				painter.setPen(LegendPen);
				painter.drawLine(Place.x(), Place.y() + ypos*k, Place.x() + (int)(LegendSize), Place.y() + ypos*k);

				if(m_pPF->m_bOutPoints)
				{
//					x1 = Place.x + (int)(0.5*LegendSize);
//					pDC->Rectangle(x1-2, Place.y + ypos*k-2, x1+2, Place.y + ypos*k+2);
					x1 = Place.x() + (int)(0.5*LegendSize);
					painter.drawRect(x1-2, Place.y() + ypos*k-2, 4,4);
				}
				painter.setPen(TextPen);
				painter.drawText(Place.x() + (int)(1.5*LegendSize), Place.y() + ypos*k+delta, m_pPF->m_strFoilName);
			}
		}
		k++;
		for (n=0; n < m_poaFoil->size(); n++)
		{
			pRefFoil = (CFoil*)m_poaFoil->at(n);
			if(pRefFoil && pRefFoil->m_bVisible)
			{
				strong = pRefFoil->m_FoilName;
				if(strong.length())
				{
					LegendPen.setColor(pRefFoil->m_FoilColor);
					LegendPen.setStyle(GetStyle(pRefFoil->m_nFoilStyle));
					LegendPen.setWidth(pRefFoil->m_nFoilWidth);

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


void QAFoil::PaintView(QPainter &painter)
{
	painter.save();

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	double xscale = m_fScale          /m_fRefScale;
	double yscale = m_fScale*m_fScaleY/m_fRefScale;

	//zoom from the center of the viewport
	QPoint VCenter = QPoint((int)((m_rCltRect.right() + m_rCltRect.left()  )/2),
					    (int)((m_rCltRect.top()   + m_rCltRect.bottom())/2));

	painter.fillRect(m_rCltRect, pMainFrame->m_BackgroundColor);

	//draw the background image in the viewport
	if(m_bIsImageLoaded)
	{
		int w = (int)((double)m_BackImageWidth  * xscale);
		int h = (int)((double)m_BackImageHeight * yscale);
		//the coordinates of the top left corner are measured from the center of the viewport
		double xtop = VCenter.x() + m_ViewportTrans.x() - (int)((double)m_BackImageWidth  /2.*xscale);
		double ytop = VCenter.y() + m_ViewportTrans.y() - (int)((double)m_BackImageHeight /2.*yscale);

		painter.drawPixmap(xtop, ytop, w,h, m_BackImage);
	}

	m_ptOffset.rx() = VCenter.x() + m_ViewportTrans.x() - (int)(0.5 *m_fScale);
	m_ptOffset.ry() = VCenter.y() + m_ViewportTrans.y() ;

	painter.setFont(pMainFrame->m_TextFont);

	QPen TextPen(pMainFrame->m_TextColor);
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


void QAFoil::PaintSplines(QPainter &painter)
{
	painter.save();
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	QPen SplinePen, CenterPen, CtrlPen;

	QBrush FillBrush(pMainFrame->m_BackgroundColor);
	painter.setBrush(FillBrush);

	if (m_bSF)
	{
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
	}
	else if(m_pPF->m_bVisible)
	{
		SplinePen.setStyle(GetStyle(m_pPF->m_FoilStyle));
		SplinePen.setWidth(m_pPF->m_FoilWidth);
		SplinePen.setColor(m_pPF->m_FoilColor);
		painter.setPen(SplinePen);

		m_pPF->DrawFoil(painter, m_fScale, m_fScale*m_fScaleY, m_ptOffset);

/*		CtrlPen.setStyle(Qt::SolidLine);
		CtrlPen.setColor(m_pPF->m_FoilColor);
		painter.setPen(CtrlPen);*/

		m_pPF->DrawCtrlPoints(painter, m_fScale,m_fScale*m_fScaleY, m_ptOffset);


		if (m_pPF->m_bCenterLine)
		{
			CenterPen.setColor(m_pPF->m_FoilColor);
			CenterPen.setStyle(Qt::DashLine);
			painter.setPen(CenterPen);

			m_pPF->DrawMidLine(painter, m_fScale,m_fScale*m_fScaleY, m_ptOffset);
		}
	}
	painter.restore();
}



void QAFoil::PaintFoils(QPainter &painter)
{
	painter.save();
	int k;
	CFoil *pFoil;
	QColor color;
	int style, width;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	QPen FoilPen, CenterPen, CtrlPen;

	QBrush FillBrush(pMainFrame->m_BackgroundColor);
	painter.setBrush(FillBrush);

	for (k=0; k< m_poaFoil->size(); k++)
	{
		pFoil = (CFoil*)m_poaFoil->at(k);
		if (pFoil->m_bVisible)
		{
			FoilPen.setStyle(GetStyle(pFoil->m_nFoilStyle));
			FoilPen.setWidth(pFoil->m_nFoilWidth);
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
		color = m_pBufferFoil->m_FoilColor;
		style = m_pBufferFoil->m_nFoilStyle;
		width = m_pBufferFoil->m_nFoilWidth;



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



void QAFoil::ReleaseZoom()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->m_pctrlZoomIn->setChecked(false);
	TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;
	m_bZoomPlus = false;

	m_ZoomRect.setRight(m_ZoomRect.left()-1);
	m_ZoomRect.setTop(m_ZoomRect.bottom()+1);
	p2DWidget->setCursor(m_hcCross);
}


void QAFoil::SaveSettings(QSettings *pSettings)
{
	pSettings->beginGroup("DirectDesign");
	{
		pSettings->setValue("ShowSF", m_bSF);
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
		pSettings->setValue("XMajUnit", m_YGridUnit);
		pSettings->setValue("YMajUnit", m_YGridUnit);
		pSettings->setValue("XMinUnit", m_XMinUnit);
		pSettings->setValue("YMinUnit", m_YMinUnit);

		pSettings->setValue("XMajColorRed",m_XGridColor.red());
		pSettings->setValue("XMajColorGreen",m_XGridColor.green());
		pSettings->setValue("XMajColorBlue",m_XGridColor.blue());
		pSettings->setValue("YMajColorRed", m_YGridColor.red());
		pSettings->setValue("YMajColorGreen", m_YGridColor.green());
		pSettings->setValue("YMajColorBlue", m_YGridColor.blue());

		pSettings->setValue("XMinColorRed", m_XMinColor.red());
		pSettings->setValue("XMinColorGreen", m_XMinColor.green());
		pSettings->setValue("XMinColorBlue", m_XMinColor.blue());

		pSettings->setValue("YMinColorRed", m_YMinColor.red());
		pSettings->setValue("YMinColorGreen", m_YMinColor.green());
		pSettings->setValue("YMinColorBlue", m_YMinColor.blue());

		pSettings->setValue("NeutralStyle", m_NeutralStyle);
		pSettings->setValue("NeutralWidth", m_NeutralWidth);
		pSettings->setValue("NeutralColorRed", m_NeutralColor.red());
		pSettings->setValue("NeutralColorGreen", m_NeutralColor.green());
		pSettings->setValue("NeutralColorBlue", m_NeutralColor.blue());

		pSettings->setValue("SFStyle", m_pSF->m_FoilStyle);
		pSettings->setValue("SFWidth", m_pSF->m_FoilWidth);
		pSettings->setValue("SFColorRed", m_pSF->m_FoilColor.red());
		pSettings->setValue("SFColorGreen", m_pSF->m_FoilColor.green());
		pSettings->setValue("SFColorBlue", m_pSF->m_FoilColor.blue());

		pSettings->setValue("PFStyle", m_pPF->m_FoilStyle);
		pSettings->setValue("PFWidth", m_pPF->m_FoilWidth);
		pSettings->setValue("PFColorRed", m_pPF->m_FoilColor.red());
		pSettings->setValue("PFColorGreen", m_pPF->m_FoilColor.green());
		pSettings->setValue("PFColorBlue", m_pPF->m_FoilColor.blue());
	
		pSettings->setValue("SFVisible", m_pSF->m_bVisible);
		pSettings->setValue("SFOutPoints", m_pSF->m_bOutPoints);
		pSettings->setValue("SFCenterLine", m_pSF->m_bCenterLine);
		pSettings->setValue("PFVisible", m_pPF->m_bVisible);
		pSettings->setValue("PFOutPoints", m_pPF->m_bOutPoints);
		pSettings->setValue("PFCenterLine", m_pPF->m_bCenterLine);
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


void QAFoil::SetScale()
{
	//scale is set by user zooming
	m_fRefScale = (double)m_rCltRect.width()-150.0;

	m_fScale = m_fRefScale;

	m_ptOffset.rx() = 75;
	m_ptOffset.ry() = (int)(m_rCltRect.height()/2);

	m_pSF->SetViewRect(m_rCltRect);
	m_pPF->SetViewRect(m_rCltRect);

	m_ViewportTrans = QPoint(0,0);
}


void QAFoil::SetScale(QRect CltRect)
{
	//scale is set by ChildView
	m_rCltRect = CltRect;

	SetScale();
}


void QAFoil::SetPicture()
{
//	double gap;
	int i;
	if(m_bSF)
	{
		 m_pSF->m_Extrados.m_iCtrlPoints = m_UndoPic[m_StackPos].m_iExt;
		 m_pSF->m_Intrados.m_iCtrlPoints = m_UndoPic[m_StackPos].m_iInt;
		for (i=0; i<=m_UndoPic[m_StackPos].m_iExt; i++)
		{
			m_pSF->m_Extrados.m_Input[i].x = m_UndoPic[m_StackPos].xExt[i];
			m_pSF->m_Extrados.m_Input[i].y = m_UndoPic[m_StackPos].yExt[i];
		}
		for (i=0; i<=m_UndoPic[m_StackPos].m_iInt; i++)
		{
			m_pSF->m_Intrados.m_Input[i].x = m_UndoPic[m_StackPos].xInt[i];
			m_pSF->m_Intrados.m_Input[i].y = m_UndoPic[m_StackPos].yInt[i];
		}

//		gap =   m_pSF->m_Extrados.m_Input[m_pSF->m_Extrados.m_iCtrlPoints].y
//			  - m_pSF->m_Intrados.m_Input[m_pSF->m_Intrados.m_iCtrlPoints].y;

		m_pSF->UpdateKnots();
		m_pSF->Update(true);
		m_pSF->Update(false);
	}
	else
	{
		 m_pPF->m_Extrados.m_iPoints = m_UndoPic[m_StackPos].m_iExt;
		 m_pPF->m_Intrados.m_iPoints = m_UndoPic[m_StackPos].m_iInt;

		 m_pPF->m_Extrados.m_RearPoint.x = m_UndoPic[m_StackPos].ExtRearPt.x;
		 m_pPF->m_Extrados.m_RearPoint.y = m_UndoPic[m_StackPos].ExtRearPt.y;
		 m_pPF->m_Intrados.m_RearPoint.x = m_UndoPic[m_StackPos].IntRearPt.x;
		 m_pPF->m_Intrados.m_RearPoint.y = m_UndoPic[m_StackPos].IntRearPt.y;

		for (i=0; i<=m_UndoPic[m_StackPos].m_iExt; i++)
		{
			m_pPF->m_Extrados.m_ctrlPoint[i].x = m_UndoPic[m_StackPos].xExt[i];
			m_pPF->m_Extrados.m_ctrlPoint[i].y = m_UndoPic[m_StackPos].yExt[i];
		}
		for (i=0; i<=m_UndoPic[m_StackPos].m_iInt; i++)
		{
			m_pPF->m_Intrados.m_ctrlPoint[i].x = m_UndoPic[m_StackPos].xInt[i];
			m_pPF->m_Intrados.m_ctrlPoint[i].y = m_UndoPic[m_StackPos].yInt[i];
		}
//		gap =   m_pPF->m_Extrados.m_ctrlPoint[m_pPF->m_Extrados.m_iPoints].y
//			  - m_pPF->m_Intrados.m_ctrlPoint[m_pPF->m_Intrados.m_iPoints].y;

		m_pPF->Update();
		m_pPF->Update();
	}
//	SetGap(gap);
	UpdateView();
}


void QAFoil::OnFoilTableCtxMenu(const QPoint & position)
{
	m_CurrentColumn = m_pctrlFoilTable->columnAt(position.x());
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->AFoilTableCtxMenu->exec(cursor().pos());
}



void QAFoil::SetupLayout()
{
	QDesktopWidget desktop;
	QRect r = desktop.geometry();
//	setMinimumHeight(r.height()/3);
	move(r.width()/3, r.height()/6);

	m_pctrlFoilTable   = new QTableView(this);

//	m_pctrlFoilTable->setMinimumWidth(800);
	m_pctrlFoilTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pctrlFoilTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pctrlFoilTable->setContextMenuPolicy(Qt::CustomContextMenu);
	
	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);
	m_pctrlFoilTable->setSizePolicy(szPolicyExpanding);


	QHBoxLayout *MainLayout = new QHBoxLayout;
	MainLayout->addWidget(m_pctrlFoilTable);
	setLayout(MainLayout);

//	connect(m_pctrlFoilTable, SIGNAL(clicked(const QModelIndex &)), this, SLOT(OnFoilClicked(const QModelIndex&)));
	connect(m_pctrlFoilTable, SIGNAL(pressed(const QModelIndex &)), this, SLOT(OnFoilClicked(const QModelIndex&)));
	connect(m_pctrlFoilTable, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(OnFoilTableCtxMenu(const QPoint &)));

	m_pFoilModel = new QStandardItemModel;
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

	m_pFoilDelegate = new FoilTableDelegate;
	m_pctrlFoilTable->setItemDelegate(m_pFoilDelegate);
	m_pFoilDelegate->m_pFoilModel = m_pFoilModel;

	int unitwidth = (int)(750.0/16.0);
	m_pctrlFoilTable->setColumnWidth(0, 3*unitwidth);
	for(int i=1; i<16; i++)		m_pctrlFoilTable->setColumnWidth(i, unitwidth);
	m_pctrlFoilTable->setColumnHidden(9, true);
	m_pctrlFoilTable->setColumnHidden(10, true);
	m_pctrlFoilTable->setColumnHidden(11, true);


	int  *precision = new int[16];
	precision[0]  = 2;
	precision[1]  = 2;
	precision[2]  = 2;
	precision[3]  = 2;
	precision[4]  = 2;
	precision[5]  = 0;
	precision[6]  = 2;
	precision[7]  = 2;
	precision[8]  = 2;
	precision[9]  = 2;
	precision[10] = 2;
	precision[11] = 2;
	precision[12] = 2;
	precision[13] = 2;
	precision[14] = 2;
	precision[15] = 2;

	m_pFoilDelegate->m_Precision = precision;
//	connect(m_pFoilDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged(QWidget *)));
}


void QAFoil::SelectFoil(CFoil* pFoil)
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
	g_pCurFoil = pFoil;
}


void QAFoil::SetParams()
{
	FillFoilTable();

	SelectFoil(g_pCurFoil);
	CheckButtons();
}


void QAFoil::ShowFoil(CFoil* pFoil, bool bShow)
{
	if(!pFoil) return;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	g_pCurFoil->m_bVisible = bShow;
	pMainFrame->SetSaveState(false);
}


void QAFoil::StorePicture()
{
	if(m_StackPos>=50)
	{
		for (int i=1; i<MAXSTACKPOS; i++)
		{
			memcpy(&m_UndoPic[i-1],&m_UndoPic[i], sizeof(Picture));
		}
		m_StackPos  = MAXSTACKPOS-1;
		m_StackSize = MAXSTACKPOS-1;
	}
	memcpy(&m_UndoPic[m_StackPos], &m_TmpPic, sizeof(Picture));
	m_bStored = true;
	m_StackPos++;
	m_StackSize = m_StackPos;
}


void QAFoil::TakePicture()
{
	int i;
	m_bStored = false;
	if(m_bSF)
	{
		m_TmpPic.m_iExt = m_pSF->m_Extrados.m_iCtrlPoints;
		for (i=0; i<=m_TmpPic.m_iExt; i++)
		{
			m_TmpPic.xExt[i] = m_pSF->m_Extrados.m_Input[i].x;
			m_TmpPic.yExt[i] = m_pSF->m_Extrados.m_Input[i].y;
		}
		m_TmpPic.m_iInt = m_pSF->m_Intrados.m_iCtrlPoints;
		for (i=0; i<=m_TmpPic.m_iInt; i++)
		{
			m_TmpPic.xInt[i] = m_pSF->m_Intrados.m_Input[i].x;
			m_TmpPic.yInt[i] = m_pSF->m_Intrados.m_Input[i].y;
		}
	}
	else
	{
		m_TmpPic.m_iExt = m_pPF->m_Extrados.m_iPoints;
		for (i=0; i<=m_TmpPic.m_iExt; i++)
		{
			m_TmpPic.xExt[i] = m_pPF->m_Extrados.m_ctrlPoint[i].x;
			m_TmpPic.yExt[i] = m_pPF->m_Extrados.m_ctrlPoint[i].y;
		}
		m_TmpPic.ExtRearPt.x = m_pPF->m_Extrados.m_RearPoint.x;
		m_TmpPic.ExtRearPt.y = m_pPF->m_Extrados.m_RearPoint.y;

		m_TmpPic.m_iInt = m_pPF->m_Intrados.m_iPoints;
		for (i=0; i<=m_TmpPic.m_iInt; i++)
		{
			m_TmpPic.xInt[i] = m_pPF->m_Intrados.m_ctrlPoint[i].x;
			m_TmpPic.yInt[i] = m_pPF->m_Intrados.m_ctrlPoint[i].y;
		}
		m_TmpPic.IntRearPt.x = m_pPF->m_Intrados.m_RearPoint.x;
		m_TmpPic.IntRearPt.y = m_pPF->m_Intrados.m_RearPoint.y;
	}
}



void QAFoil::UpdateView()
{
	TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;

	if(m_p2DWidget)
	{
		p2DWidget->update();
	}
}




void QAFoil::wheelEvent(QWheelEvent *event)
{
	if(! m_rCltRect.contains(event->pos())) return;

	m_ZoomRect.setBottomRight(m_ZoomRect.topLeft());
	ReleaseZoom();

	MainFrame * pMainFrame = (MainFrame*)m_pMainFrame;
	static double ZoomFactor, scale;
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

	scale = m_fScale;

	if(!m_bZoomYOnly)
	{
		if (m_bXDown)
		{
			m_fScale  *= ZoomFactor;
			m_fScaleY *= 1./ZoomFactor;
		}
		else if (m_bYDown) m_fScaleY *= ZoomFactor;
		else  m_fScale *= ZoomFactor;
	}
	else m_fScaleY *= ZoomFactor;


	int a = (int)((m_rCltRect.right() + m_rCltRect.left()  )/2);
	m_ptOffset.rx() = a + (int)((m_ptOffset.x()-a)*m_fScale/scale);
	m_ViewportTrans.rx() = (int)((m_ViewportTrans.x())*m_fScale           /scale);
	m_ViewportTrans.ry() = (int)((m_ViewportTrans.y())*m_fScale /scale);

	UpdateView();
}



void QAFoil::OnColumnWidths()
{
	int unitwidth = (int)((double)m_pctrlFoilTable->width()/16.0);
	m_pctrlFoilTable->setColumnWidth(0, 3*unitwidth);
	for(int i=1; i<16; i++)		m_pctrlFoilTable->setColumnWidth(i, unitwidth);
	m_pctrlFoilTable->setColumnHidden(9, true);
	m_pctrlFoilTable->setColumnHidden(10, true);
	m_pctrlFoilTable->setColumnHidden(11, true);
}


void QAFoil::OnAFoilTableColumns()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	AFoilTableDlg dlg;
	dlg.move(pMainFrame->m_DlgPos);

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
	pMainFrame->m_DlgPos = dlg.pos();
}


void QAFoil::OnLoadBackImage()
{
	MainFrame*pMainFrame = (MainFrame*)m_pMainFrame;
	QString PathName;
	PathName = QFileDialog::getOpenFileName(this, tr("Open Image File"),
											pMainFrame->m_LastDirName,
											"Image files (*.png *.jpg *.bmp)");
	m_bIsImageLoaded = m_BackImage.load(PathName);
	if(m_bIsImageLoaded)
	{
		m_BackImageWidth = m_BackImage.width();
		m_BackImageHeight = m_BackImage.height();
	}

	UpdateView();
}


void QAFoil::OnClearBackImage()
{
	m_bIsImageLoaded = false;
	UpdateView();
}








