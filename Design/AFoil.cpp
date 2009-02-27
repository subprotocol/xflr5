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
#include "../XDirect/NacaFoilDlg.h"
#include "../Misc/LinePickerDlg.h"
#include "AFoil.h"


QAFoil::QAFoil(QWidget *parent)
	: QWidget(parent)
{
	m_hcArrow = QCursor(Qt::ArrowCursor);
	m_hcCross = QCursor(Qt::CrossCursor);
	m_hcMove  = QCursor(Qt::ClosedHandCursor);

	m_MousePos.x = 0.0;
	m_MousePos.y = 0.0;

	m_pCurFoil = NULL;
	m_FoilSelection = 0;

	m_pSF = new CSF();
	m_pSF->m_bModified = false;
	m_pSF->InitSplineFoil();

	m_pPF = new CPF();
	m_pPF->m_bModified = false;
	m_pPF->InitSplinedFoil();

	m_bZoomPlus    = false;
	m_bZoomYOnly   = false;
	m_bTrans       = false;
	m_bNeutralLine = false;
	m_bScale       = true;
	m_bCircle      = false;
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
	m_XMinorUnit = 0.01;
	m_XMinStyle  = 2;
	m_XMinWidth  = 1;
	m_XMinColor  = QColor(70,70,70);
	m_bYMinGrid  = false;
	m_YMinorUnit = 0.01;
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



void QAFoil::FillFoilTable()
{
	int i;
	m_pFoilModel->setRowCount(m_poaFoil->size());

	for(i=0; i<m_poaFoil->size(); i++)
	{
		FillTableRow(i);
	}
}

void QAFoil::FillTableRow(int row)
{
	QString str, strong;
	QModelIndex ind;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	CFoil *pFoil = (CFoil*)m_poaFoil->at(row);

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

}

void QAFoil::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
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
			m_bXDown = false;
			break;
		case Qt::Key_Y:
			m_bYDown = false;
			break;
		case Qt::Key_Z:
			m_bZDown = false;
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


void QAFoil::mouseMoveEvent(QMouseEvent *event)
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QPoint point = event->pos();
	m_MousePos = MousetoReal(point);
	CVector Real = m_MousePos;

/*	SHORT sh1  = GetKeyState(VK_LCONTROL);
	SHORT sh2  = GetKeyState(VK_RCONTROL);
	SHORT shZ  = GetKeyState('Z');
	SHORT shX  = GetKeyState('X');
	SHORT shY  = GetKeyState('Y');*/

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

	if(!m_bZoomPlus)
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
			UpdateView();
		}
	}
	if (event->modifiers() & Qt::MidButton && !m_bZoomPlus)
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

		UpdateView();
		return;
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
					else		                m_fScale /= 1.06;
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

		UpdateView();
		return;
	}
	UpdateView();
}


void QAFoil::mousePressEvent(QMouseEvent *event)
{
	QPoint point = event->pos();

	CVector Real = MousetoReal(point);

	// get a reference for mouse movements 
	m_PointDown.rx() = point.x();
	m_PointDown.ry() = point.y();

	if(m_bZoomPlus && m_rCltRect.contains(point))
	{
		m_ZoomRect.setTopLeft(point);
		m_ZoomRect.setBottomRight(point);
	}
	else if(!m_bZoomPlus)
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
					setCursor(m_hcMove);
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
					setCursor(m_hcMove);
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
//	m_pACtrl->UpdateFoil(-10);

	if(m_bZoomPlus && m_rCltRect.contains(point))
	{
		QRect ZRect = m_ZoomRect.normalized();
	
		if(!ZRect.isEmpty())
		{
			m_ZoomRect.setBottomRight(point);
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
		setCursor(m_hcCross);
	}

	UpdateView();
}


void QAFoil::OnFoilClicked(const QModelIndex& index)
{
	if(index.row()>=m_poaFoil->size())
	{
		return;
	}
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	QString FoilName = index.model()->data(index, Qt::EditRole).toString();
	CFoil *pFoil= pMainFrame->GetFoil(FoilName);
	SetFoil(pFoil);
}

void QAFoil::OnCenterLine()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(m_FoilSelection==0)
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
	else if(m_pCurFoil)
	{
		if(m_pctrlCenterLine->isChecked()) m_pCurFoil->m_bCenterLine = true;
		else                               m_pCurFoil->m_bCenterLine = false;
	}
	pMainFrame->SetSaveState(false);
	pMainFrame->UpdateView();
}



void QAFoil::OnFoilStyle()
{
	if(!m_pCurFoil) return;

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
	}
}


void QAFoil::OnModelPoints()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	if(m_FoilSelection==0)
	{
		if(m_bSF)
		{
			if(m_pctrlModelPoints->isChecked()) m_pSF->m_bOutPoints = true;
			else                                m_pSF->m_bOutPoints = false;
		}
		else
		{
			if(m_pctrlModelPoints->isChecked()) m_pPF->m_bOutPoints = true;
			else                                m_pPF->m_bOutPoints = false;
			m_pPF->SetOutPoints(m_pPF->m_bOutPoints);
		}
	}
	else if(m_pCurFoil)
	{
		if (m_pctrlModelPoints->isChecked()) m_pCurFoil->m_bPoints = true;
		else m_pCurFoil->m_bPoints = false;
	}

	pMainFrame->SetSaveState(false);
	UpdateView();
}


void QAFoil::OnVisible()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;

	if(m_FoilSelection==0)
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
	else if(m_pCurFoil)
	{
		if(m_pctrlVisible->isChecked()) ShowFoil(m_pCurFoil, true);
		else                            ShowFoil(m_pCurFoil, false);
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
	SetFoil();
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


void QAFoil::PaintLegend(QPainter &painter)
{
}

void QAFoil::PaintView(QPainter &painter)
{
	painter.save();

	TwoDWidget *p2DWidget = (TwoDWidget*)m_p2DWidget;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;

	painter.fillRect(m_rCltRect, pMainFrame->m_BackgroundColor);


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




void QAFoil::PaintGrids(QPainter &painter)
{
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

		CtrlPen.setStyle(Qt::SolidLine);
		CtrlPen.setColor(m_pPF->m_FoilColor);
		painter.setPen(CtrlPen);

		m_pPF->DrawCtrlPoints(painter, m_fScale,m_fScale*m_fScaleY, m_ptOffset);

		if (m_pPF->m_bCenterLine)
		{
			CenterPen.setColor(m_pSF->m_FoilColor);
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
	m_bZoomPlus = false;
	m_ZoomRect.setBottomRight(m_ZoomRect.topLeft());

	setCursor(m_hcCross);
}


void QAFoil::SetScale()
{//scale is set by user zooming
	m_fRefScale = (double)m_rCltRect.width()-150.0;

	m_fScale = m_fRefScale;

	double width  = (double)m_rCltRect.width();
	double height = (double)m_rCltRect.height();
//	double clippedh = height/width * 20.0;

	m_ptOffset.rx() = 75;
	m_ptOffset.ry() = (int)(m_rCltRect.height()/2);

	m_pSF->SetViewRect(m_rCltRect);
	m_pPF->SetViewRect(m_rCltRect);

}


void QAFoil::SetScale(QRect CltRect)
{//scale is set by ChildView
	m_rCltRect = CltRect;

	int width = m_rCltRect.width();

	SetScale();
}


void QAFoil::ShowFoil(CFoil* pFoil, bool bShow)
{
	if(!pFoil) return;
	MainFrame* pMainFrame = (MainFrame*)m_pMainFrame;
	m_pCurFoil->m_bVisible = bShow;
	SetFoil(pFoil);
	pMainFrame->SetSaveState(false);
}


void QAFoil::SetParams()
{
	m_pFoilModel = new QStandardItemModel;
	m_pFoilModel->setRowCount(10);//temporary
	m_pFoilModel->setColumnCount(10);

	m_pFoilModel->setHeaderData(0, Qt::Horizontal, "Name");
	m_pFoilModel->setHeaderData(1, Qt::Horizontal, "Camber");
	m_pFoilModel->setHeaderData(2, Qt::Horizontal, "at");
	m_pFoilModel->setHeaderData(3, Qt::Horizontal, "Thickness");
	m_pFoilModel->setHeaderData(4, Qt::Horizontal, "at");


	m_pctrlFoilTable->setModel(m_pFoilModel);
	m_pctrlFoilTable->setWindowTitle("Foils");

	m_pFoilDelegate = new FoilTableDelegate;
	m_pctrlFoilTable->setItemDelegate(m_pFoilDelegate);

	int  *precision = new int[10];
	precision[0] = 2;//five digits for x and y coordinates
	precision[1] = 2;
	precision[2] = 2;
	precision[3] = 1;
	precision[4] = 2;
	precision[5] = 1;
	precision[6] = 0;
	precision[7] = 0;
	precision[8] = 0;
	precision[9] = 0;
	m_pFoilDelegate->m_Precision = precision;

	FillFoilTable();

	m_pctrlFoilTable->setColumnWidth(0,70);
	m_pctrlFoilTable->setColumnWidth(1,70);
	m_pctrlFoilTable->setColumnWidth(2,70);
	m_pctrlFoilTable->setColumnWidth(3,70);
	m_pctrlFoilTable->setColumnWidth(4,70);
	m_pctrlFoilTable->setColumnWidth(5,130);
	m_pctrlFoilTable->setColumnWidth(6,50);
	m_pctrlFoilTable->setColumnWidth(7,80);
	m_pctrlFoilTable->setColumnWidth(8,50);
	m_pctrlFoilTable->setColumnWidth(9,80);
}


void QAFoil::SetSplineData()
{
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
				m_pctrlFoilTable->selectRow(i+1);
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
	CFoil * pOldFoil = NULL;
	int i;

	pMainFrame->m_pCurFoil = pFoil;
	m_pCurFoil = pFoil;

	if(!pFoil)
	{
		if(m_bSF)
		{
			m_pctrlVisible->setChecked(m_pSF->m_bVisible);
			m_pctrlCenterLine->setChecked(m_pSF->m_bCenterLine);
			m_pctrlModelPoints->setChecked(m_pSF->m_bOutPoints);
			m_pctrlFoilStyle->SetStyle(m_pSF->m_FoilStyle, m_pSF->m_FoilWidth, m_pSF->m_FoilColor);
		}
		else
		{
			m_pctrlVisible->setChecked(m_pPF->m_bVisible);
			m_pctrlCenterLine->setChecked(m_pPF->m_bCenterLine);
			m_pctrlModelPoints->setChecked(m_pPF->m_bOutPoints);
			m_pctrlFoilStyle->SetStyle(m_pSF->m_FoilStyle, m_pSF->m_FoilWidth, m_pSF->m_FoilColor);
		}
	}
	else
	{
		m_pctrlVisible->setChecked(pFoil->m_bVisible);
		m_pctrlCenterLine->setChecked(pFoil->m_bCenterLine);
		m_pctrlModelPoints->setChecked(pFoil->m_bPoints);
		m_pctrlFoilStyle->SetStyle(m_pCurFoil->m_nFoilStyle, m_pCurFoil->m_nFoilWidth, m_pCurFoil->m_FoilColor);
	}
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
//	m_pACtrl->SetGap(gap);
	UpdateView();
}


void QAFoil::SetupLayout()
{
	QDesktopWidget desktop;
	QRect r = desktop.geometry();
//	setMinimumHeight(r.height()/3);
	move(r.width()/3, r.height()/6);

	m_pctrlFoilTable   = new QTableView(this);

	m_pctrlFoilTable->setMinimumWidth(800);
	m_pctrlFoilTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pctrlFoilTable->setSelectionBehavior(QAbstractItemView::SelectRows);

	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::Expanding);
	m_pctrlFoilTable->setSizePolicy(szPolicyExpanding);

	m_pctrlFoilStyle   = new LineButton;
	m_pctrlVisible     = new QCheckBox("Show Foil");
	m_pctrlCenterLine  = new QCheckBox("Show Centerline");
	m_pctrlModelPoints = new QCheckBox("Show Points");
	m_pctrlRename      = new QPushButton("Rename");
	m_pctrlDelete      = new QPushButton("Delete");
	m_pctrlDuplicate   = new QPushButton("Duplicate");

	QVBoxLayout *RightControls = new QVBoxLayout;
	RightControls->addWidget(m_pctrlFoilStyle);
	RightControls->addWidget(m_pctrlRename);
	RightControls->addWidget(m_pctrlDelete);
	RightControls->addWidget(m_pctrlDuplicate);
	RightControls->addStretch(1);
	RightControls->addWidget(m_pctrlVisible);
	RightControls->addWidget(m_pctrlCenterLine);
	RightControls->addWidget(m_pctrlModelPoints);

	QHBoxLayout *MainLayout = new QHBoxLayout;
	MainLayout->addWidget(m_pctrlFoilTable);
	MainLayout->addLayout(RightControls);

	setLayout(MainLayout);

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	connect(m_pctrlFoilStyle, SIGNAL(clicked()), this, SLOT(OnFoilStyle()));
//	connect(m_pctrlRename, SIGNAL(clicked()), pMainFrame, SLOT(OnRenameCurFoil()));
	connect(m_pctrlDelete, SIGNAL(clicked()), this, SLOT(OnDelete()));
	connect(m_pctrlDuplicate, SIGNAL(clicked()), this, SLOT(OnDuplicate()));

	connect(m_pctrlFoilTable, SIGNAL(clicked(const QModelIndex &)), this, SLOT(OnFoilClicked(const QModelIndex&)));
	connect(m_pctrlFoilTable, SIGNAL(pressed(const QModelIndex &)), this, SLOT(OnFoilClicked(const QModelIndex&)));

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


void QAFoil::UpdateBlt()
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

