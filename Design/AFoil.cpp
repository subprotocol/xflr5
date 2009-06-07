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

//QSelectionModel *selections = tableView->selectionModel();
//QModelIndexList selected = selections->selectedIndexex();

#include <QtGui>

#include "../Globals.h"
#include "../MainFrame.h"
#include "../XDirect/NacaFoilDlg.h"
#include "../XDirect/CAddDlg.h"
#include "../XDirect/TwoDPanelDlg.h"
#include "../XDirect/TEGapDlg.h"
#include "../XDirect/LEDlg.h"
#include "../XDirect/FlapDlg.h"
#include "../XDirect/FoilCoordDlg.h"
#include "../XDirect/FoilGeomDlg.h"
#include "../XDirect/InterpolateFoilsDlg.h"
#include "../Misc/LinePickerDlg.h"
#include "AFoil.h"
#include "AFoilGridDlg.h"
#include "SplineCtrlsDlg.h"
#include "LECircleDlg.h"

QAFoil::~QAFoil()
{
//	delete m_pctrlFoilTable;
//	delete m_pFoilDelegate;
//	delete m_pFoilModel;
}


QAFoil::QAFoil(QWidget *parent)
	: QWidget(parent)
{
	m_hcArrow = QCursor(Qt::ArrowCursor);
	m_hcCross = QCursor(Qt::CrossCursor);
	m_hcMove  = QCursor(Qt::ClosedHandCursor);

	m_MousePos.x = 0.0;
	m_MousePos.y = 0.0;

	m_pCurFoil = NULL;
	m_poaFoil  = NULL;

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

	memset(&m_TmpPic,0, sizeof(Picture));

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

	m_pBufferFoil = new CFoil();

	SetupLayout();

	FoilTableDelegate::s_pAFoil = this;
}


void QAFoil::DrawXGrid(QPainter &painter, double scalex, double scaley, QPoint Offset, QRect dRect)
{
	painter.save();
	QPen GridPen(m_XGridColor);
	GridPen.setStyle(GetStyle(m_XGridStyle));
	GridPen.setWidth(m_XGridWidth);
	painter.setPen(GridPen);

	double xo = 0.0;
	double xmin = 0.0;
	double xmax = 1.0;
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

	QString str, name;
	QModelIndex ind;

	double Thickness, xThickness, Camber, xCamber;
	Thickness = xThickness = Camber = xCamber = 0;
	int points = 0;

	if(m_bSF)
	{
		if(m_pSF) 
		{
			name = "Spline foil";
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
			name = "Splined points foil";
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


	for(i=0; i<m_poaFoil->size(); i++)
	{
		FillTableRow(i+1);
	}
}


void QAFoil::FillTableRow(int row)
{
	QString str, strong;
	QModelIndex ind;
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

/*	ind = m_pFoilModel->index(row, 12, QModelIndex());
	if(pFoil->m_bVisible) m_pFoilModel->setData(ind, Qt::Checked, Qt::CheckStateRole);
	else                  m_pFoilModel->setData(ind, Qt::Unchecked, Qt::CheckStateRole);*/
}


void QAFoil::keyPressEvent(QKeyEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	switch (event->key())
	{
		case Qt::Key_Escape:
		{
//			TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;
//			p2DWidget->setCursor(m_hcCross);
			if(m_bZoomPlus)
			{
				ReleaseZoom();
				break;
			}
			else if(m_bZoomYOnly)
			{
				pMainFrame->m_pctrlZoomY->setChecked(false);
				m_bZoomYOnly = false;
				break;
			}
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
		case Qt::Key_X:
			m_bXDown = true;
			break;
		case Qt::Key_Y:
			m_bYDown = true;
			break;
		case Qt::Key_Z:
			m_bZDown = true;
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


void QAFoil::LoadSettings(QDataStream &ar)
{
	ar >> m_bSF;
	ar >> m_bXGrid >>m_bYGrid >> m_bXMinGrid >> m_bYMinGrid >> m_XGridStyle >> m_YGridStyle;
	ar >> m_XGridWidth >> m_YGridWidth >> m_XMinStyle >> m_YMinStyle >> m_XMinWidth >> m_YMinWidth;
	ar >> m_XGridUnit >> m_YGridUnit >> m_XMinUnit >> m_YMinUnit;
	ar >> m_XGridColor >>m_YGridColor >> m_XMinColor >>m_YMinColor;

	ar >> m_NeutralStyle >> m_NeutralWidth >> m_NeutralColor;

	int style, width;
	QColor color;
	ar >> style >> width >> color;
	m_pSF->SetCurveParams(style, width, color);
	ar >> style >> width >> color;
	m_pPF->SetCurveParams(style, width, color);
//	ar >> m_pSF->m_FoilStyle >> m_pSF->m_FoilWidth  >> m_pSF->m_FoilColor;
//	ar >> m_pPF->m_FoilStyle >> m_pPF->m_FoilWidth  >> m_pPF->m_FoilColor;
	ar >> m_pSF->m_bVisible  >> m_pSF->m_bOutPoints >> m_pSF->m_bCenterLine;
	ar >> m_pPF->m_bVisible  >> m_pPF->m_bOutPoints >> m_pPF->m_bCenterLine;

	ar >> m_bLECircle >> m_bNeutralLine >> m_bScale >> m_bShowLegend;

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
//					m_pSF->m_Extrados.SplineCurve();
					m_pSF->m_bModified = true;
					pMainFrame->SetSaveState(false);
					m_pSF->Update(true);
				}
				else
				{
					int n = m_pSF->m_Intrados.m_iSelect;
					if (n>=0 && n<=m_pSF->m_Intrados.m_iCtrlPoints)
					{
						if(!m_bStored) StorePicture();//save for undo only the first time
//						if(n==1) m_MousePos.x = 0.0;// we can't move point 1 for vertical slope
						m_pSF->m_Intrados.m_Input[n].x = m_MousePos.x;
						m_pSF->m_Intrados.m_Input[n].y = m_MousePos.y;
//						m_pSF->m_Intrados.SplineCurve();
						m_pSF->m_bModified = true;
						pMainFrame->SetSaveState(false);
						m_pSF->Update(false);
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
						m_pPF->m_bModified = true;
						pMainFrame->SetSaveState(false);
						m_pPF->Update(true);
					}
					else if(m_pPF->m_Intrados.m_iSelect == -1)
					{
						if(!m_bStored) StorePicture();//save for undo only the first time
						m_pPF->m_Intrados.m_RearPoint.x = m_MousePos.x;
						m_pPF->m_Intrados.m_RearPoint.y = m_MousePos.y;
						m_pPF->m_bModified = true;
						pMainFrame->SetSaveState(false);
						m_pPF->Update(false);
					}
				}
				m_pPF->CompMidLine();
			}
			FillFoilTable();
		}
	}
	else if (((event->modifiers() & Qt::ControlModifier) )  && !bCtrl)
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
						m_fScale  *= 1.06;
						m_fScaleY /= 1.06;
					}
					else 
					{
						m_fScale  /= 1.06;
						m_fScaleY *= 1.06;
					}
				}
				else if (m_bYDown)
				{
					if(point.y()-m_PointDown.y()>0) m_fScaleY *= 1.06;
					else                            m_fScaleY /= 1.06;
				}
				else
				{
					if(point.y()-m_PointDown.y()>0) m_fScale *= 1.06;
					else		                    m_fScale /= 1.06;
				}

			}
			else
			{
				if(point.y()-m_PointDown.y()>0) m_fScaleY *= 1.06;
				else                            m_fScaleY /= 1.06;
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
					m_pPF->Update(true);
				}
				else 
				{
					int n=m_pPF->m_Intrados.IsControlPoint(Real, m_fScale/m_fRefScale);
					if (n>=0) m_pPF->m_Intrados.RemovePoint(n);
					m_pPF->Update(false);
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
					m_pPF->Update(true);
				}
				else 
				{
					m_pPF->m_Intrados.InsertPoint(Real.x, Real.y);
					m_pPF->Update(false);
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
//	UpdateFoil(-10);
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

			m_ZoomRect.setBottomRight(m_ZoomRect.topLeft());
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


void QAFoil::OnAFoilSetFlap()
{
	if(!m_pCurFoil) return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pBufferFoil->CopyFoil(m_pCurFoil);
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = m_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();

	FlapDlg dlg;
	dlg.m_pAFoil      = this;
	dlg.m_pXDirect    = NULL;
	dlg.m_pXFoil      = m_pXFoil;
	dlg.m_pMemFoil    = m_pCurFoil;
	dlg.m_pBufferFoil = m_pBufferFoil;
	dlg.InitDialog();

	if(QDialog::Accepted == dlg.exec())
	{
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		if(pMainFrame->SetModFoil(pNewFoil))
		{
			m_pCurFoil = NULL;
			FillFoilTable();
			SetFoil(pNewFoil);
			SelectFoil(pNewFoil);
		}
		else
		{
//			pNewFoil = NULL;
			FillFoilTable();
			SelectFoil(m_pCurFoil);
		}
	}
	else
	{
		FillFoilTable();
		SelectFoil(m_pCurFoil);
		m_pXFoil->m_FoilName ="";
	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}



void QAFoil::OnAFoilDerotateFoil()
{
	if(!m_pCurFoil) return;

	double angle = m_pCurFoil->DeRotate();

	QString str = QString("Foil has been de-rotated by %1 degrees").arg(angle,6,'f',3);

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->statusBar()->showMessage(str);
	UpdateView();
}


void QAFoil::OnAFoilNormalizeFoil()
{
	if(!m_pCurFoil) return;
	double length = m_pCurFoil->NormalizeGeometry();

	QString str = QString("Foil has been normalized from %1  to 1.000").arg(length,7,'f',3);

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->statusBar()->showMessage(str);

	UpdateView();

}

void QAFoil::OnAFoilCadd()
{
	if(!m_pCurFoil) return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pBufferFoil->CopyFoil(m_pCurFoil);
	m_pBufferFoil->m_FoilName   = m_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;
	m_pBufferFoil->m_bPoints    = true;
	m_pBufferFoil->m_bVisible   = true;

	UpdateView();

	CAddDlg Adlg;
	Adlg.m_pBufferFoil = m_pBufferFoil;
	Adlg.m_pMemFoil    = m_pCurFoil;
	Adlg.m_pXDirect    = NULL;
	Adlg.m_pAFoil      = this;
	Adlg.InitDialog();

	if(QDialog::Accepted == Adlg.exec())
	{
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;

		if(pMainFrame->SetModFoil(pNewFoil))
		{
			m_pCurFoil = NULL;
			FillFoilTable();
			SelectFoil(pNewFoil);
		}
		else
		{
//			pNewFoil = NULL;
			FillFoilTable();
			SelectFoil(m_pCurFoil);
		}
	}

	else
	{
		FillFoilTable();
		SelectFoil(m_pCurFoil);
		m_pXFoil->m_FoilName ="";

	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


void QAFoil::OnAFoilLECircle()
{
	LECircleDlg dlg;
	dlg.m_Radius      = m_LERad;
	dlg.m_bShowRadius = m_bLECircle;
	dlg.m_pAFoil      = this;
	dlg.InitDialog();

	if(dlg.exec()==QDialog::Accepted)
	{
		m_LERad = dlg.m_Radius;
		m_bLECircle = dlg.m_bShowRadius;
	}
	UpdateView();
}


void QAFoil::OnAFoilPanels()
{
	if(!m_pCurFoil) return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pBufferFoil->CopyFoil(m_pCurFoil);
	m_pBufferFoil->m_FoilName  = m_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;
	m_pBufferFoil->m_bPoints   = true;
	m_pBufferFoil->m_bVisible  = true;

	UpdateView();

	TwoDPanelDlg Pdlg;
	Pdlg.m_pBufferFoil = m_pBufferFoil;
	Pdlg.m_pMemFoil    = m_pCurFoil;
	Pdlg.m_pXDirect    = NULL;
	Pdlg.m_pAFoil      = this;
	Pdlg.InitDialog();

	if(QDialog::Accepted == Pdlg.exec())
	{
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		if(pMainFrame->SetModFoil(pNewFoil))
		{
			m_pCurFoil = NULL;
			FillFoilTable();
			SelectFoil(pNewFoil);
		}
		else
		{
//			pNewFoil = NULL;
			FillFoilTable();
			SelectFoil(m_pCurFoil);
		}
	}

	else
	{
		FillFoilTable();
		SelectFoil(m_pCurFoil);
//		m_pXFoil->m_FoilName ="";

	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


void QAFoil::OnAFoilFoilCoordinates()
{
	if(!m_pCurFoil) return;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pBufferFoil->CopyFoil(m_pCurFoil);
	m_pBufferFoil->m_bPoints  = true;
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = m_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();

	FoilCoordDlg dlg;
	dlg.m_pMemFoil    = m_pCurFoil;
	dlg.m_pBufferFoil = m_pBufferFoil;
	dlg.m_pXDirect    = NULL;
	dlg.m_pAFoil      = this;

	if(QDialog::Accepted == dlg.exec())
	{
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_iHighLight = -1;

		if(pMainFrame->SetModFoil(pNewFoil))
		{
			m_pCurFoil = NULL;
			FillFoilTable();
			SelectFoil(pNewFoil);
		}
		else
		{
//			pNewFoil = NULL;
			FillFoilTable();
			SelectFoil(m_pCurFoil);
		}
	}
	else
	{
		FillFoilTable();
		SelectFoil(m_pCurFoil);
		m_pXFoil->m_FoilName ="";
	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


void QAFoil::OnAFoilFoilGeom()
{
	if(!m_pCurFoil) return;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pBufferFoil->CopyFoil(m_pCurFoil);
	m_pBufferFoil->m_bPoints  = true;
	m_pBufferFoil->m_bVisible = true;
	m_pBufferFoil->m_FoilName = m_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();

	FoilGeomDlg dlg;
	dlg.m_pMemFoil    = m_pCurFoil;
	dlg.m_pBufferFoil = m_pBufferFoil;
	dlg.m_pAFoil      = this;
	dlg.m_pXDirect    = NULL;
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

		if(pMainFrame->SetModFoil(pNewFoil))
		{
			m_pCurFoil = NULL;
			FillFoilTable();
			SelectFoil(pNewFoil);
		}
		else
		{
//			pNewFoil = NULL;
			FillFoilTable();
			SelectFoil(m_pCurFoil);
		}
	}

	else
	{
		FillFoilTable();
		SelectFoil(m_pCurFoil);
//		m_pXFoil->m_FoilName ="";
	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}



void QAFoil::OnAFoilSetTEGap()
{
	if(!m_pCurFoil) return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pBufferFoil->CopyFoil(m_pCurFoil);
	m_pBufferFoil->m_bPoints    = false;
	m_pBufferFoil->m_bVisible   = true;
	m_pBufferFoil->m_FoilName   = m_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();

	TEGapDlg Gdlg;
	Gdlg.m_pBufferFoil = m_pBufferFoil;
	Gdlg.m_pMemFoil    = m_pCurFoil;
	Gdlg.m_pXDirect    = NULL;
	Gdlg.m_pAFoil      = this;
	Gdlg.InitDialog();

	if(QDialog::Accepted == Gdlg.exec())
	{
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints    = false;

		if(pMainFrame->SetModFoil(pNewFoil))
		{
			m_pCurFoil = NULL;
			FillFoilTable();
			SelectFoil(pNewFoil);
		}
		else
		{
//			pNewFoil = NULL;
			FillFoilTable();
			SelectFoil(m_pCurFoil);
		}
	}
	else
	{
		FillFoilTable();
		SelectFoil(m_pCurFoil);
//		m_pXFoil->m_FoilName ="";
		//to un-initialize XFoil in case user switches to XInverse
		//Thanks Jean-Marc !
	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


void QAFoil::OnAFoilSetLERadius()
{
	if(!m_pCurFoil) return;

	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	m_pBufferFoil->CopyFoil(m_pCurFoil);
	m_pBufferFoil->m_bVisible   = true;
	m_pBufferFoil->m_bPoints    = false;
	m_pBufferFoil->m_FoilName   = m_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;

	UpdateView();

	LEDlg Ldlg;
	Ldlg.m_pBufferFoil = m_pBufferFoil;
	Ldlg.m_pMemFoil    = m_pCurFoil;
	Ldlg.m_pXDirect    = NULL;
	Ldlg.m_pAFoil      = this;
	Ldlg.InitDialog();

	if(QDialog::Accepted == Ldlg.exec())
	{
		//then duplicate the buffer foil and add it
		CFoil *pNewFoil = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints    = false;

		if(pMainFrame->SetModFoil(pNewFoil))
		{
			m_pCurFoil = NULL;
			FillFoilTable();
			SelectFoil(pNewFoil);
		}
		else
		{
//			pNewFoil = NULL;
			FillFoilTable();
			SelectFoil(m_pCurFoil);
		}
	}
	else
	{
		FillFoilTable();
		SelectFoil(m_pCurFoil);
//		m_pXFoil->m_FoilName ="";

	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


void QAFoil::OnAFoilInterpolateFoils()
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	if(m_poaFoil->size()<2)
	{
		QMessageBox::warning(pMainFrame,"QFLR5","At least two foils are required");
		return;
	}

	if(!m_pCurFoil) SetFoil();
	if(!m_pCurFoil) return;
	m_pBufferFoil->CopyFoil(m_pCurFoil);
	m_pBufferFoil->m_FoilName  = m_pCurFoil->m_FoilName;
	m_pBufferFoil->m_FoilColor  = QColor(160,160,160);
	m_pBufferFoil->m_nFoilStyle = 1;
	m_pBufferFoil->m_nFoilWidth = 1;
	m_pBufferFoil->m_bPoints   = false;
	m_pBufferFoil->m_bVisible  = true;

	UpdateView();

	InterpolateFoilsDlg dlg;
	dlg.m_poaFoil = m_poaFoil;
	dlg.m_pBufferFoil = m_pBufferFoil;
	dlg.m_pMainFrame = m_pMainFrame;
	dlg.m_pXDirect  = NULL;
	dlg.m_pAFoil    = this;
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
		pNewFoil->m_FoilName = dlg.m_NewFoilName;

		if(pMainFrame->SetModFoil(pNewFoil))
		{
			m_pCurFoil = NULL;
			FillFoilTable();
			SelectFoil(pNewFoil);
		}
		else
		{
//			pNewFoil = NULL;
			FillFoilTable();
			SelectFoil(m_pCurFoil);
		}
	}

	else
	{
		FillFoilTable();
		SelectFoil(m_pCurFoil);
//		dlg.m_pXFoil->m_FoilName ="";

	}
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

	NacaFoilDlg dlg;
	dlg.m_pBufferFoil = m_pBufferFoil;
	dlg.m_pXDirect = NULL;
	dlg.m_pAFoil = this;

	if(QDialog::Accepted == dlg.exec())
	{
		//then duplicate the buffer foil and add it
		QString str;

		if(dlg.m_Digits>0 && log10((double)dlg.m_Digits)<4)
			str = QString("%1").arg(dlg.m_Digits,4,10,QChar('0'));
		else
			str = QString("%1").arg(dlg.m_Digits);
		str = "NACA "+ str;

		CFoil *pNewFoil    = new CFoil();
		pNewFoil->CopyFoil(m_pBufferFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints    = false;
		pNewFoil->m_FoilName   = str;
		if(pMainFrame->SetModFoil(pNewFoil))
		{
			m_pCurFoil = NULL;
			FillFoilTable();
			SelectFoil(pNewFoil);
		}
		else
		{
//			pNewFoil = NULL;
			FillFoilTable();
			SelectFoil(m_pCurFoil);
		}
	}

	else
	{
		FillFoilTable();;
		if(m_pCurFoil) SelectFoil(m_pCurFoil);
		m_pXFoil->m_FoilName ="";

	}
	m_pBufferFoil->m_bVisible = false;
	UpdateView();
}


void QAFoil::OnCenterLine()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(m_pCurFoil)
	{
		if(m_pctrlCenterLine->isChecked()) m_pCurFoil->m_bCenterLine = true;
		else                               m_pCurFoil->m_bCenterLine = false;
	}
	else
	{
		if(m_bSF)
		{
			if(m_pctrlCenterLine->isChecked()) m_pSF->m_bCenterLine = true;
			else                               m_pSF->m_bCenterLine = false;
		}
		else
		{
			if(m_pctrlCenterLine->isChecked()) m_pPF->m_bCenterLine = true;
			else                               m_pPF->m_bCenterLine = false;
		}
	}

	pMainFrame->SetSaveState(false);
	UpdateView();
}



void QAFoil::OnDelete()
{
	if(!m_pCurFoil) return;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->DeleteFoil(m_pCurFoil);
	FillFoilTable();
	SelectFoil();
	UpdateView();
}


void QAFoil::OnDuplicate()
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	if(!m_pCurFoil) return;
	CFoil *pNewFoil = new CFoil;
	pNewFoil->CopyFoil(m_pCurFoil);
	pNewFoil->InitFoil();

	if(pMainFrame->SetModFoil(pNewFoil))
	{
		FillFoilTable();
		SetFoil(pNewFoil);
	}
	else
	{
		pNewFoil = NULL;
		FillFoilTable();
		SetFoil(pNewFoil);
	}
}


void QAFoil::OnExportCurFoil()
{
	if(!m_pCurFoil)	return;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QString FileName, DestFileName, OutString;
	QFile DestFile;

	FileName = m_pCurFoil->m_FoilName;
	FileName.replace("/", " ");

	FileName = QFileDialog::getSaveFileName(this, "Export Foil",
											pMainFrame->m_LastDirName,
											"Foil File (*.dat)");
	if(!FileName.length()) return;
	int pos = FileName.lastIndexOf("/");
	if(pos>0) pMainFrame->m_LastDirName = FileName.left(pos);

	QFile XFile(FileName);

	if (!XFile.open(QIODevice::WriteOnly | QIODevice::Text)) return ;

	QTextStream out(&XFile);

	m_pCurFoil->ExportFoil(out);
	XFile.close();
}


void QAFoil::OnExportSplinesToFile()
{
	QString FoilName = "Spline Foil";
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
			strong = QString("Too many output points on upper surface\n Max =%1").arg(IQX2);
			QMessageBox::warning(pMainFrame, "QFLR5", strong, QMessageBox::Ok);
			return;
		}
		if(m_pSF->m_Intrados.m_iRes>IQX2)
		{
			strong = QString("Too many output points on lower surface\n Max =%1").arg(IQX2);
			QMessageBox::warning(pMainFrame, "QFLR5", strong, QMessageBox::Ok);
			return;
		}
	}
	else
	{
		int size = m_pPF->m_Extrados.m_iPoints * (m_pPF->m_Extrados.m_Freq-1) ;//+ 1;
		if(size>IQX2)
		{
			strong = QString("Too many output points on upper surface\n Max =%1").arg(IQX2);
			QMessageBox::warning(pMainFrame, "QFLR5", strong, QMessageBox::Ok);
			return;
		}
		size = m_pPF->m_Intrados.m_iPoints * (m_pPF->m_Intrados.m_Freq-1) ;//+ 1;
		if(size>IQX2)
		{
			strong = QString("Too many output points on lower surface\n Max =%1").arg(IQX2);
			QMessageBox::warning(pMainFrame, "QFLR5", strong, QMessageBox::Ok);
			return;
		}
	}

	QFile DestFile;

	FileName.replace("/", " ");
	FileName = QFileDialog::getSaveFileName(this, "Export Splines", pMainFrame->m_LastDirName,
											"Text File (*.dat)");

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




void QAFoil::OnFoilClicked(const QModelIndex& index)
{
	if(index.row()>=m_poaFoil->size()+1) return;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	QStandardItem *pItem = m_pFoilModel->item(index.row(),0);
	QString FoilName =pItem->text();

	if(index.row()>0)
	{
		CFoil *pFoil= pMainFrame->GetFoil(FoilName);
		SetFoil(pFoil);
	}
	else if(index.row()==0)
	{
		SetFoil();
	}
}


void QAFoil::OnFoilStyle()
{
	if(!m_pCurFoil)
	{
		LinePickerDlg dlg;
		if(m_bSF) dlg.InitDialog(m_pSF->m_FoilStyle, m_pSF->m_FoilWidth, m_pSF->m_FoilColor);
		else      dlg.InitDialog(m_pPF->m_FoilStyle, m_pPF->m_FoilWidth, m_pPF->m_FoilColor);

		if(QDialog::Accepted==dlg.exec())
		{
			m_pctrlFoilStyle->SetStyle(dlg.GetStyle());
			m_pctrlFoilStyle->SetWidth(dlg.GetWidth());
			m_pctrlFoilStyle->SetColor(dlg.GetColor());

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
		dlg.InitDialog(m_pCurFoil->m_nFoilStyle, m_pCurFoil->m_nFoilWidth, m_pCurFoil->m_FoilColor);

		if(QDialog::Accepted==dlg.exec())
		{
			m_pctrlFoilStyle->SetStyle(dlg.GetStyle());
			m_pctrlFoilStyle->SetWidth(dlg.GetWidth());
			m_pctrlFoilStyle->SetColor(dlg.GetColor());

			m_pCurFoil->m_nFoilStyle = dlg.GetStyle();
			m_pCurFoil->m_nFoilWidth = dlg.GetWidth();
			m_pCurFoil->m_FoilColor = dlg.GetColor();
			UpdateView();
		}
	}
}

void QAFoil::OnGrid()
{
	AFoilGridDlg dlg(this);

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
	m_pctrlVisible->setChecked(false);
	UpdateView();
}



void QAFoil::OnHideCurrentFoil()
{
	if(!m_pCurFoil) return;
	ShowFoil(m_pCurFoil, false);
	UpdateView();

}



void QAFoil::OnNewSplines()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(m_bSF) SFNew();
	else      PFNew();

	m_StackPos  = 0;
	m_StackSize = 0;

	pMainFrame->SetSaveState(false);
	UpdateView();
}


void QAFoil::OnPoints()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	if(m_pCurFoil)
	{
		if (m_pctrlFoilPoints->isChecked()) m_pCurFoil->m_bPoints = true;
		else m_pCurFoil->m_bPoints = false;
	}
	else
	{
		if(m_bSF)
		{
			if(m_pctrlFoilPoints->isChecked()) m_pSF->m_bOutPoints = true;
			else                                m_pSF->m_bOutPoints = false;
		}
		else
		{
			if(m_pctrlFoilPoints->isChecked()) m_pPF->m_bOutPoints = true;
			else                                m_pPF->m_bOutPoints = false;
			m_pPF->SetOutPoints(m_pPF->m_bOutPoints);
		}
	}

	pMainFrame->SetSaveState(false);
	UpdateView();
}


void QAFoil::OnRedo()
{
	if(m_StackPos<m_StackSize-1)
	{
		m_StackPos++;
		SetPicture();
//		CToolBarCtrl *pTB = &(m_pAFoilBar->GetToolBarCtrl());
//		pTB->EnableButton(IDT_UNDO, true);
//		if(m_StackPos==m_StackSize-1) pTB->EnableButton(IDT_REDO, false);
	}
}


void QAFoil::OnRenameFoil()
{
	if(!m_pCurFoil) return;
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
	m_pctrlVisible->setChecked(true);
	UpdateView();
}



void QAFoil::OnShowCurrentFoil()
{
	if(!m_pCurFoil) return;
	ShowFoil(m_pCurFoil, true);
	UpdateView();

}




void QAFoil::OnStoreSplines()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(m_bSF)
	{
		if(m_pSF->m_Extrados.m_iRes>IQX2)
		{
			QString strong = QString("Too many output points on upper surface\n Max =%1").arg(IQX2);
			QMessageBox::warning(pMainFrame, "QFLR5", strong, QMessageBox::Ok);
			return;
		}
		if(m_pSF->m_Intrados.m_iRes>IQX2)
		{
			QString strong = QString("Too many output points on lower surface\n Max =%1").arg(IQX2);
			QMessageBox::warning(pMainFrame, "QFLR5", strong, QMessageBox::Ok);
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
			m_pCurFoil = NULL;
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
			QString strong = QString("Too many output points on upper surface\n Max =%1").arg(IQX2);
			QMessageBox::warning(pMainFrame, "QFLR5", strong, QMessageBox::Ok);
			return;
		}

		size = m_pPF->m_Intrados.m_iPoints * (m_pPF->m_Intrados.m_Freq-1) ;//+ 1;
		if(size>IQX2)
		{
			QString strong = QString("Too many output points on lower surface\n Max =%1").arg(IQX2);
			QMessageBox::warning(pMainFrame, "QFLR5", strong, QMessageBox::Ok);
			return;
		}
//		Trace("m_iPts_Int=",size);

		CFoil *pNewFoil = new CFoil();
		m_pPF->ExportToBuffer(pNewFoil);
		pNewFoil->m_FoilColor  = pMainFrame->GetColor(0);
		pNewFoil->m_nFoilStyle = 0;
		pNewFoil->m_nFoilWidth = 1;
		pNewFoil->m_bPoints = false;
		pNewFoil->m_FoilName = "";
		if(pMainFrame->SetModFoil(pNewFoil))
		{
			m_pCurFoil = NULL;
			FillFoilTable();
			SelectFoil(pNewFoil);
		}
		else
		{
			delete pNewFoil;
			pNewFoil = NULL;
			FillFoilTable();
			SelectFoil();
		}
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
		if(m_bSF)
		{
			m_pSF->m_Extrados.m_iDegree = dlg.m_pctrlDegExtrados->currentIndex()+2;
			m_pSF->m_Intrados.m_iDegree = dlg.m_pctrlDegIntrados->currentIndex()+2;
		}
		if(m_bSF)
		{
			int n = dlg.m_pctrlOutExtrados->GetValue();
			if(n<1) dlg.m_pctrlOutExtrados->SetValue(1);
			if(n>IQX2-10) dlg.m_pctrlOutExtrados->SetValue(IQX2-10);
			m_pSF->m_Extrados.m_iRes = dlg.m_pctrlOutExtrados->GetValue();
			m_pSF->Update(true);
		}
		else
		{
			m_pPF->m_Extrados.m_Freq = dlg.m_pctrlOutExtrados->GetValue();
			m_pPF->m_Intrados.m_Freq = dlg.m_pctrlOutIntrados->GetValue();
		}
	}
}



void QAFoil::OnSplineType()
{
	m_bSF = m_pctrlSF->isChecked();
	SetFoil(NULL);
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
//		CToolBarCtrl *pTB = &(m_pAFoilBar->GetToolBarCtrl());
//		if(m_StackPos==0) pTB->EnableButton(IDT_UNDO, false);
//		pTB->EnableButton(IDT_REDO, true);
	}
	else
	{
		m_StackPos = 0;
	}
}



void QAFoil::OnVisible()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(m_pCurFoil)
	{
		if(m_pctrlVisible->isChecked()) ShowFoil(m_pCurFoil, true);
		else                            ShowFoil(m_pCurFoil, false);
	}
	else
	{
		if(m_bSF)
		{
			if(m_pctrlVisible->isChecked()) m_pSF->m_bVisible = true;
			else                            m_pSF->m_bVisible = false;
		}
		else {
			if(m_pctrlVisible->isChecked()) m_pPF->m_bVisible = true;
			else                            m_pPF->m_bVisible = false;
		}
	}

	pMainFrame->SetSaveState(false);
	UpdateView();
}



void QAFoil::OnZoomIn()
{
	// can't do two things at the same time can we ?
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

	if(m_bZoomPlus)
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

/*	pDC->SetTextColor(pChildView->m_WndTextColor);
	CFont RFont;
	RFont.CreateFontIndirect(&pChildView->m_WndLogFont);
	CFont *pOldFont = pDC->SelectObject(&RFont);

	if(m_bScale) DrawScale(pDC, &m_rDrawRect, m_fScale, m_ptOffset,false);*/

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
		QPoint Place(m_rCltRect.right()-350, 20);

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

//	TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	painter.fillRect(m_rCltRect, pMainFrame->m_BackgroundColor);

	painter.setFont(pMainFrame->m_TextFont);

	QPen TextPen(pMainFrame->m_TextColor);
	painter.setPen(TextPen);

	PaintGrids(painter);
	PaintSplines(painter);
	PaintFoils(painter);
	PaintLegend(painter);

	QString str;

	str = QString("X-Scale = %1").arg(m_fScale/m_fRefScale,4,'f',1);
	painter.drawText(5,10, str);
	str = QString("Y-Scale = %1").arg(m_fScaleY*m_fScale/m_fRefScale,4,'f',1);
	painter.drawText(5,22, str);
	str = QString("x  = %1").arg(m_MousePos.x,7,'f',4);
	painter.drawText(5,34, str);
	str = QString("y  = %1").arg(m_MousePos.y,7,'f',4);
	painter.drawText(5,46, str);

	painter.restore();
}




void QAFoil::PaintSplines(QPainter &painter)
{
	painter.save();
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	QPen SplinePen, CenterPen, CtrlPen, OutPen;

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


void QAFoil::PFNew()
{
	bool bLoadNew = true;
	if(m_pPF->m_bModified)
	{
		if (QMessageBox::Yes != QMessageBox::question(this, "QFLR5", "Discard changes to Splines ?",
													  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))
		{
			bLoadNew = false;
		}
	}
	if(bLoadNew) m_pPF->InitSplinedFoil();
}


void QAFoil::ReleaseZoom()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	pMainFrame->m_pctrlZoomIn->setChecked(false);
	TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;
	m_bZoomPlus = false;
	m_ZoomRect.setBottomRight(m_ZoomRect.topLeft());
	p2DWidget->setCursor(m_hcCross);
}


void QAFoil::SaveSettings(QDataStream &ar)
{
	ar << m_bSF;
	ar << m_bXGrid <<m_bYGrid << m_bXMinGrid << m_bYMinGrid << m_XGridStyle << m_YGridStyle;
	ar << m_XGridWidth << m_YGridWidth << m_XMinStyle << m_YMinStyle << m_XMinWidth << m_YMinWidth;
	ar << m_XGridUnit << m_YGridUnit << m_XMinUnit << m_YMinUnit;
	ar << m_XGridColor <<m_YGridColor << m_XMinColor <<m_YMinColor;

	ar << m_NeutralStyle << m_NeutralWidth << m_NeutralColor;

	ar << m_pSF->m_FoilStyle << m_pSF->m_FoilWidth << m_pSF->m_FoilColor;
	ar << m_pPF->m_FoilStyle << m_pPF->m_FoilWidth << m_pPF->m_FoilColor;
	ar << m_pSF->m_bVisible << m_pSF->m_bOutPoints << m_pSF->m_bCenterLine;
	ar << m_pPF->m_bVisible << m_pPF->m_bOutPoints << m_pPF->m_bCenterLine;

	ar << m_bLECircle << m_bNeutralLine << m_bScale << m_bShowLegend;
}




void QAFoil::SetScale()
{
	//scale is set by user zooming
	m_fRefScale = (double)m_rCltRect.width()-150.0;

	m_fScale = m_fRefScale;

//	double width  = (double)m_rCltRect.width();
//	double height = (double)m_rCltRect.height();
//	double clippedh = height/width * 20.0;

	m_ptOffset.rx() = 75;
	m_ptOffset.ry() = (int)(m_rCltRect.height()/2);

	m_pSF->SetViewRect(m_rCltRect);
	m_pPF->SetViewRect(m_rCltRect);

}


void QAFoil::SetScale(QRect CltRect)
{
	//scale is set by ChildView
	m_rCltRect = CltRect;

//	int width = m_rCltRect.width();

	SetScale();
}

void QAFoil::SetPicture()
{
	double gap;
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

		gap =   m_pSF->m_Extrados.m_Input[m_pSF->m_Extrados.m_iCtrlPoints].y
			  - m_pSF->m_Intrados.m_Input[m_pSF->m_Intrados.m_iCtrlPoints].y;

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
		gap =   m_pPF->m_Extrados.m_ctrlPoint[m_pPF->m_Extrados.m_iPoints].y
			  - m_pPF->m_Intrados.m_ctrlPoint[m_pPF->m_Intrados.m_iPoints].y;

		m_pPF->Update(true);
		m_pPF->Update(false);
	}
//	SetGap(gap);
	UpdateView();
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

	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Expanding);
	m_pctrlFoilTable->setSizePolicy(szPolicyExpanding);

	m_pctrlFoilStyle   = new LineButton;
	m_pctrlVisible     = new QCheckBox("Show Foil");
	m_pctrlCenterLine  = new QCheckBox("Show Centerline");
	m_pctrlFoilPoints  = new QCheckBox("Show Points");

	m_pctrlRename      = new QPushButton("Rename");
	m_pctrlDelete      = new QPushButton("Delete");
	m_pctrlDuplicate   = new QPushButton("Duplicate");
	m_pctrlExport      = new QPushButton("Export");

	QGridLayout *FoilCommands = new QGridLayout;
	FoilCommands->addWidget(m_pctrlRename,1,1);
	FoilCommands->addWidget(m_pctrlDelete,1,2);
	FoilCommands->addWidget(m_pctrlDuplicate,2,1);
	FoilCommands->addWidget(m_pctrlExport,2,2);

	QHBoxLayout *SplineOption = new QHBoxLayout;
	m_pctrlSF = new QRadioButton("Splines");
	m_pctrlPF = new QRadioButton("Splined points");
	SplineOption->addWidget(m_pctrlSF);
	SplineOption->addWidget(m_pctrlPF);

	QVBoxLayout *RightControls = new QVBoxLayout;
	RightControls->addLayout(SplineOption);
	RightControls->addWidget(m_pctrlFoilStyle);
	RightControls->addLayout(FoilCommands);
	RightControls->addStretch(1);
	RightControls->addWidget(m_pctrlVisible);
	RightControls->addWidget(m_pctrlCenterLine);
	RightControls->addWidget(m_pctrlFoilPoints);

	QHBoxLayout *MainLayout = new QHBoxLayout;
	MainLayout->addWidget(m_pctrlFoilTable);
	MainLayout->addLayout(RightControls);

	setLayout(MainLayout);

	connect(m_pctrlSF, SIGNAL(clicked()), this, SLOT(OnSplineType()));
	connect(m_pctrlPF, SIGNAL(clicked()), this, SLOT(OnSplineType()));

	connect(m_pctrlFoilStyle, SIGNAL(clicked()), this, SLOT(OnFoilStyle()));
	connect(m_pctrlRename, SIGNAL(clicked()), this, SLOT(OnRenameFoil()));
	connect(m_pctrlDelete, SIGNAL(clicked()), this, SLOT(OnDelete()));
	connect(m_pctrlDuplicate, SIGNAL(clicked()), this, SLOT(OnDuplicate()));
	connect(m_pctrlExport, SIGNAL(clicked()), this, SLOT(OnExportCurFoil()));
	connect(m_pctrlFoilPoints, SIGNAL(clicked()), this, SLOT(OnPoints()));
	connect(m_pctrlVisible, SIGNAL(clicked()), this, SLOT(OnVisible()));
	connect(m_pctrlCenterLine, SIGNAL(clicked()), this, SLOT(OnCenterLine()));

	connect(m_pctrlFoilTable, SIGNAL(clicked(const QModelIndex &)), this, SLOT(OnFoilClicked(const QModelIndex&)));
	connect(m_pctrlFoilTable, SIGNAL(pressed(const QModelIndex &)), this, SLOT(OnFoilClicked(const QModelIndex&)));


	m_pFoilModel = new QStandardItemModel;
	m_pFoilModel->setRowCount(10);//temporary
	m_pFoilModel->setColumnCount(12);

	m_pFoilModel->setHeaderData(0, Qt::Horizontal, "Name");
	m_pFoilModel->setHeaderData(1, Qt::Horizontal, "Thickness (%)");
	m_pFoilModel->setHeaderData(2, Qt::Horizontal, "at (%)");
	m_pFoilModel->setHeaderData(3, Qt::Horizontal, "Camber (%)");
	m_pFoilModel->setHeaderData(4, Qt::Horizontal, "at (%)");
	m_pFoilModel->setHeaderData(5, Qt::Horizontal, "Points");
	m_pFoilModel->setHeaderData(6, Qt::Horizontal, "T.E. Flap (deg)");
	m_pFoilModel->setHeaderData(7, Qt::Horizontal, "T.E. XHinge");
	m_pFoilModel->setHeaderData(8, Qt::Horizontal, "T.E. YHinge");
	m_pFoilModel->setHeaderData(9, Qt::Horizontal, "L.E. Flap (deg)");
	m_pFoilModel->setHeaderData(10, Qt::Horizontal, "L.E. XHinge");
	m_pFoilModel->setHeaderData(11, Qt::Horizontal, "L.E. YHinge");
//	m_pFoilModel->setHeaderData(12, Qt::Horizontal, "Visible");

	m_pctrlFoilTable->setModel(m_pFoilModel);
	m_pctrlFoilTable->setWindowTitle("Foils");

	m_pFoilDelegate = new FoilTableDelegate;
	m_pctrlFoilTable->setItemDelegate(m_pFoilDelegate);
	m_pFoilDelegate->m_pFoilModel = m_pFoilModel;

//	QHeaderView *pHeader = new QHeaderView(Qt::Horizontal);
//	m_pctrlFoilTable->setHorizontalHeader(pHeader);

	int  *precision = new int[12];
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

	m_pFoilDelegate->m_Precision = precision;
//	connect(m_pFoilDelegate,  SIGNAL(closeEditor(QWidget *)), this, SLOT(OnCellChanged(QWidget *)));
}


void QAFoil::resizeEvent(QResizeEvent *event)
{
	int w = m_pctrlFoilTable->width();
	int w12 = (int)((double)w/12.0);
	int w14 = (int)((double)w/14.0);

	m_pctrlFoilTable->setColumnWidth(1,w12);
	m_pctrlFoilTable->setColumnWidth(2,w12);
	m_pctrlFoilTable->setColumnWidth(3,w12);
	m_pctrlFoilTable->setColumnWidth(4,w12);
	m_pctrlFoilTable->setColumnWidth(5,w14);//points

	m_pctrlFoilTable->setColumnWidth(6,w14);//TE Flap
	m_pctrlFoilTable->setColumnWidth(7,w12);//TE XHinge
	m_pctrlFoilTable->setColumnWidth(8,w12);//TE YHinge

	m_pctrlFoilTable->setColumnWidth(9, w14);//LE Flap
	m_pctrlFoilTable->setColumnWidth(10,w12);//LE XHinge
	m_pctrlFoilTable->setColumnWidth(11,w12);//LE YHinge

	m_pctrlFoilTable->setColumnWidth(0,w-8*w12-3*w14-20);
}



void QAFoil::SelectFoil(CFoil* pFoil)
{
	int i;

	if(pFoil)
	{
		QModelIndex ind;
		QString FoilName;
		SetFoil(pFoil);

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
		SetFoil(NULL);
		m_pctrlFoilTable->selectRow(0);
	}
}


void QAFoil::SetFoil(CFoil *pFoil)
{
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	pMainFrame->m_pCurFoil = pFoil;
	m_pCurFoil = pFoil;

	if(!pFoil)
	{
		if(m_bSF)
		{
			m_pctrlVisible->setChecked(m_pSF->m_bVisible);
			m_pctrlCenterLine->setChecked(m_pSF->m_bCenterLine);
			m_pctrlFoilPoints->setChecked(m_pSF->m_bOutPoints);
			m_pctrlFoilStyle->SetStyle(m_pSF->m_FoilStyle, m_pSF->m_FoilWidth, m_pSF->m_FoilColor);
		}
		else
		{
			m_pctrlVisible->setChecked(m_pPF->m_bVisible);
			m_pctrlCenterLine->setChecked(m_pPF->m_bCenterLine);
			m_pctrlFoilPoints->setChecked(m_pPF->m_bOutPoints);
			m_pctrlFoilStyle->SetStyle(m_pPF->m_FoilStyle, m_pPF->m_FoilWidth, m_pPF->m_FoilColor);
		}
	}
	else
	{
		m_pctrlVisible->setChecked(pFoil->m_bVisible);
		m_pctrlCenterLine->setChecked(pFoil->m_bCenterLine);
		m_pctrlFoilPoints->setChecked(pFoil->m_bPoints);
		m_pctrlFoilStyle->SetStyle(m_pCurFoil->m_nFoilStyle, m_pCurFoil->m_nFoilWidth, m_pCurFoil->m_FoilColor);
	}
}


void QAFoil::SetParams()
{
	if(m_bSF) m_pctrlSF->setChecked(true);
	else      m_pctrlPF->setChecked(true);

	FillFoilTable();
//	m_pctrlFoilTable->adjustSize();

	SelectFoil(m_pCurFoil);
}

void QAFoil::SFNew()
{
	bool bLoadNew = true;
	if(m_pSF->m_bModified)
	{
		if (QMessageBox::Yes != QMessageBox::question(this, "QFLR5", "Discard changes to Splines ?",
													  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))
		{
			bLoadNew = false;
		}
	}
	if(bLoadNew) m_pSF->InitSplineFoil();
}



void QAFoil::ShowFoil(CFoil* pFoil, bool bShow)
{
	if(!pFoil) return;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	m_pCurFoil->m_bVisible = bShow;
	SetFoil(pFoil);
	pMainFrame->SetSaveState(false);
}

void QAFoil::StorePicture()
{
	if(m_StackPos>=50)
	{
		for (int i=1; i<50; i++)
		{
			memcpy(&m_UndoPic[i-1],&m_UndoPic[i], sizeof(Picture));
		}
		m_StackPos = 49;
		m_StackSize = 49;
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



void QAFoil::UpdateFoil(int iFoil)
{
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
	double scale = m_fScale;

	if(!m_bZoomYOnly)
	{
		if (m_bXDown)
		{
			if(event->delta()<0)
			{
				m_fScale  *= 1.06;
				m_fScaleY /= 1.06;
			}
			else
			{
				m_fScale  /= 1.06;
				m_fScaleY *= 1.06;
			}
		}
		else if (m_bYDown)
		{
			if(event->delta()<0) m_fScaleY *= 1.06;
			else                 m_fScaleY /= 1.06;
		}
		else 
		{
			if(event->delta()<0) m_fScale *= 1.06;
			else                 m_fScale /= 1.06;
		}
	}
	else
	{

		if(event->delta()<0) m_fScaleY *= 1.06;
		else                 m_fScaleY /= 1.06;
	}

	int a = (int)((m_rCltRect.right() + m_rCltRect.left())/2);
	m_ptOffset.rx() = a + (int)((m_ptOffset.x()-a)*m_fScale/scale);

	UpdateView();
}











