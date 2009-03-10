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
#include "../Globals.h"
#include "../MainFrame.h"
#include "../Objects/Foil.h"
#include "../Graph/GraphDlg.h"

QXInverse::QXInverse(QWidget *parent)
	: QWidget(parent)
{
	m_bFullInverse = true;
	pi = 3.141592654;

	m_hcArrow = QCursor(Qt::ArrowCursor);
	m_hcCross = QCursor(Qt::CrossCursor);
	m_hcMove  = QCursor(Qt::ClosedHandCursor);
	
	m_pXFoil = NULL;

	m_bTransGraph    = false;
	m_bLoaded        = false;
	m_bSaved         = true;
	m_bZoomPlus      = false;
	m_bShowPoints    = false;
	m_bRefCurves     = false;
	m_bTangentSpline = true;
	m_bReflected     = false;

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

	m_SplineStyle = 0;
	m_SplineWidth = 1;
	m_SplineClr = QColor(170,120, 0);

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
	m_QGraph.SetXTitle("x/c");
	m_QGraph.SetYTitle("Q/Vinf");
	m_QGraph.SetXMin(0.0);
	m_QGraph.SetXMax(1.0);
	m_QGraph.SetYMin(-0.1);
	m_QGraph.SetYMax(0.1);
	m_pQCurve  = m_QGraph.AddCurve();
	m_pMCurve  = m_QGraph.AddCurve();
	m_pQVCurve = m_QGraph.AddCurve();
	m_pReflectedCurve = m_QGraph.AddCurve();
	m_pReflectedCurve->SetVisible(m_bReflected);

	SetupLayout();

	if(m_bFullInverse) m_pctrlStackedInv->setCurrentIndex(0);
	else               m_pctrlStackedInv->setCurrentIndex(1);
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


void QXInverse::CancelSmooth()
{
	m_bSmooth = false;
	m_bGetPos = false;
	m_pctrlSmooth->setChecked(false);
}
void QXInverse::CancelSpline()
{
	m_pctrlOutput->setText(" ");
//	pXInv->m_bSpline  = false;
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

void QXInverse::mouseMoveEvent(QMouseEvent *event)
{
//	SHORT shZ  = GetKeyState(90);
	double x1,y1, xmin, xmax, ymin,  ymax, xpt, ypt, scale, ux, uy, unorm, vx, vy, vnorm, scal;
	double xx0,xx1,xx2,yy0,yy1,yy2;
	int a, n, ipt;
	QPoint point = event->pos();
	if(m_bGetPos)
	{
		m_tmpPos = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x()));
	}
	else if(m_bZoomPlus && (event->buttons() & Qt::LeftButton))
	{
		m_ZoomRect.setRight(point.x());
		m_ZoomRect.setBottom(point.y());
	}
	else if(m_rCltRect.contains(point) && (event->buttons() & Qt::LeftButton) && m_bTrans)
	{
		QPoint pttmp(point.x(), point.y());
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
				ipt = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x()));
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
				ipt = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x()));
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
//				if(x1<m_Spline.m_Input[0].x)                        x1 = m_Spline.m_Input[0].x;
//				if(x1>m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x) x1 = m_Spline.m_Input[m_Spline.m_iCtrlPoints-1].x;

				m_Spline.m_Input[n].x = x1;
				m_Spline.m_Input[n].y = y1;
				m_Spline.SplineCurve();
				m_bSplined = false;
			}
		}
	}
	else if((event->buttons() & Qt::MidButton) /*||  (shZ & 0x8000)*/)
	{
		ReleaseZoom();
		QPoint pttmp(point.x(), point.y());
		if(m_QGraph.IsInDrawRect(pttmp))
		{
			//zoom graph

/*			SHORT shX = GetKeyState('X');
			SHORT shY = GetKeyState('Y');

			if (shX & 0x8000)
			{
				//zoom x scale
				m_QGraph.SetAutoX(false);
				if(point.x()-m_PointDown.x()<0) m_QGraph.Scalex(1.04);
				else                            m_QGraph.Scalex(1.0/1.04);
			}
			else if(shY & 0x8000)
			{
				//zoom y scale
				m_QGraph.SetAutoY(false);
				if(point.y()-m_PointDown.y()<0) m_QGraph.Scaley(1.04);
				else                            m_QGraph.Scaley(1.0/1.04);
			}
			else*/
			{
				//zoom both
				m_QGraph.SetAuto(false);
				if(point.y()-m_PointDown.y()<0) m_QGraph.Scale(1.06);
				else                            m_QGraph.Scale(1.0/1.06);
			}
		}
		else
		{
			scale = m_fScale;

			if(point.y()-m_PointDown.y()>0) m_fScale *= 1.06;
			else                            m_fScale /= 1.06;

			a = (int)((m_rCltRect.right()+m_rCltRect.left())/2);
			m_ptOffset.rx() = a + (int)((m_ptOffset.x()-a)*m_fScale/scale);
		}
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
		}
	}
	UpdateView();
}


void QXInverse::mousePressEvent(QMouseEvent *event)
{
	bool bCtrl, bShift;
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
				setCursor(m_hcMove);
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
				setCursor(m_hcMove);
				m_bTrans = true;
				m_bZoomPlus = false;
			}
		}	
	}
}


void QXInverse::mouseReleaseEvent(QMouseEvent *event)
{
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	m_bTrans = false;

	int tmp, width, height;
	double x1,x2,w,h,xw,yh,xm,ym;
	double xmin, ymin, xmax, ymax;
	double ratio,x, y, ux, uy, xpt, ypt, norm;

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
			m_ZoomRect.setBottomRight(m_ZoomRect.topLeft());
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
			m_Pos1 = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x()));
		}
		if(m_nPos == 1)
		{
			m_Pos2 = m_pMCurve->GetClosestPoint(m_QGraph.ClientTox(point.x()));
		}
		m_nPos++;
		if(m_nPos == 2) 
		{
			if(m_bSmooth) 
			{
				m_pctrlOutput->setText(" ");
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
					m_pctrlOutput->setText(
						"Drag points to modify splines, Apply, and Execute to generate new geometry");
				}
				else
				{
					m_pctrlNewSpline->setChecked(0);
					m_pctrlOutput->setText(	
						"Drag points to modify splines, Apply, and Execute to generate new geometry");
				}
			}
			else if(m_bMark)
			{
				m_pctrlOutput->setText(" ");
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
		setCursor(m_hcCross);
	}
	else 
	{
		setCursor(m_hcArrow);
	}
	UpdateView();
}


void QXInverse::keyPressEvent(QKeyEvent *event)
{
	bool bCtrl;
	if(event->modifiers() & Qt::ControlModifier) bCtrl  = true;

	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	switch (event->key())
	{
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

			OnGraphOptions();
			return;
		}
	}
}


void QXInverse::keyReleaseEvent(QKeyEvent *event)
{
}


void QXInverse::OnGraphOptions()
{
	GraphDlg dlg;
	dlg.m_pGraph = &m_QGraph;

	if(QDialog::Accepted == dlg.exec())
	{
		UpdateView();
	}
}


void QXInverse::OnShowSpline()
{
	m_bSpline = m_pctrlShowSpline->isChecked();
	m_bSplined =   !m_bSpline;
	UpdateView();
}


void QXInverse::OnTangentSpline()
{
	m_bTangentSpline = m_pctrlTangentSpline->isChecked();
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
		QPen SplinePen(m_SplineClr);
		SplinePen.setStyle(GetStyle(m_SplineStyle));
		SplinePen.setWidth(m_SplineWidth);
		
		painter.setPen(SplinePen);
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


//	draw the scale/grid
	if(m_bModFoil || m_bRefFoil)
	{
//		DrawGrid(painter, &m_rCltRect, m_fScale, m_ptOffset, false);
	}

//draw the reference and modified foils  
	XFoil *pXFoil = (XFoil*)m_pXFoil;
	double alpha = pXFoil->alqsp[1];

	if(m_bRefFoil && m_bLoaded)
	{
		QPen FoilPen(m_pRefFoil->m_FoilColor);
		FoilPen.setStyle(GetStyle(m_pRefFoil->m_nFoilStyle));
		FoilPen.setWidth(m_pRefFoil->m_nFoilWidth);
		painter.setPen(FoilPen);

		m_pRefFoil->DrawFoil(painter, -alpha, m_fScale, m_fScale, m_ptOffset);
		painter.drawLine(50, m_rCltRect.bottom()-180, 80, m_rCltRect.bottom()-180);
		painter.drawText(90, m_rCltRect.bottom()-186, m_pRefFoil->m_FoilName);
	}

	if(m_bModFoil && m_bLoaded) 
	{
		QPen ModPen(m_pModFoil->m_FoilColor);
		ModPen.setStyle(GetStyle(m_pModFoil->m_nFoilStyle));
		ModPen.setWidth(m_pModFoil->m_nFoilWidth);
		painter.setPen(ModPen);

		m_pModFoil->DrawFoil(painter, -alpha, m_fScale, m_fScale, m_ptOffset);
		painter.drawLine(50, m_rCltRect.bottom()-165, 80, m_rCltRect.bottom()-165);
		painter.drawText(90, m_rCltRect.bottom()-171, m_pModFoil->m_FoilName);
	}

	if (m_pRefFoil->m_bPoints)
	{
		QPen CtrlPen(m_pRefFoil->m_FoilColor);
		CtrlPen.setStyle(GetStyle(m_pRefFoil->m_nFoilStyle));
		CtrlPen.setWidth(m_pRefFoil->m_nFoilWidth);
		painter.setPen(CtrlPen);

		m_pRefFoil->DrawPoints(painter, 1.0,  1.0, m_ptOffset);
	}

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



void QXInverse::ReleaseZoom()
{
	m_bZoomPlus = false;
	m_ZoomRect.setRight(m_ZoomRect.left());
	m_ZoomRect.setTop(m_ZoomRect.bottom());
//	CToolBarCtrl *pTB = &(m_pXInverseBar->GetToolBarCtrl());
//	pTB->PressButton(IDT_ZOOMIN, false);
}


void QXInverse::SetupLayout()
{
	QDesktopWidget desktop;
	QRect r = desktop.geometry();
//	setMinimumHeight(r.height()/3);
	move(r.width()/3, r.height()/6);

	QGridLayout *SpecLayout = new QGridLayout;
	m_pctrlSpecAlpha = new QRadioButton("Alpha");
	m_pctrlSpecCl = new QRadioButton("Cl");
	m_pctrlSpecif = new QLabel("Alpha = ");
	m_pctrlSpecif->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	m_pctrlSpec   = new FloatEdit("1.23");
	SpecLayout->addWidget(m_pctrlSpecAlpha,1,1);
	SpecLayout->addWidget(m_pctrlSpecCl,1,2);
	SpecLayout->addWidget(m_pctrlSpecif,2,1);
	SpecLayout->addWidget(m_pctrlSpec,2,2);
	QGroupBox *SpecBox = new QGroupBox("Specification");
	SpecBox->setLayout(SpecLayout);

	QGridLayout *ModLayout = new QGridLayout;
	m_pctrlShowSpline    = new QCheckBox("ShowSpline");
	m_pctrlTangentSpline = new QCheckBox("Tangent Spline");
	m_pctrlNewSpline     = new QPushButton("New Spline");
	m_pctrlApplySpline   = new QPushButton("Apply Spline");
	m_pctrlResetQSpec    = new QPushButton("Reset QSpec");
	m_pctrlPert          = new QPushButton("Pert");
	ModLayout->addWidget(m_pctrlShowSpline,1,1);
	ModLayout->addWidget(m_pctrlTangentSpline,1,2);
	ModLayout->addWidget(m_pctrlNewSpline,2,1);
	ModLayout->addWidget(m_pctrlApplySpline,2,2);
	ModLayout->addWidget(m_pctrlResetQSpec,3,1);
	ModLayout->addWidget(m_pctrlPert,3,2);
	QGroupBox *ModBox = new QGroupBox("Modification");
	ModBox->setLayout(ModLayout);

	QGridLayout *SmoothLayout = new QGridLayout;
	m_pctrlSmooth = new QPushButton("Smooth QSpec");
	m_pctrlFilter = new QPushButton("Hannig Filter");
	QLabel *lab0 = new QLabel("Filter parameter");
	lab0->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	m_pctrlFilterParam = new FloatEdit("2.34");
	SmoothLayout->addWidget(m_pctrlSmooth,1,1);
	SmoothLayout->addWidget(m_pctrlFilter,1,2);
	SmoothLayout->addWidget(lab0,2,1);
	SmoothLayout->addWidget(m_pctrlFilterParam,2,2);
	QGroupBox *SmoothBox = new QGroupBox;
	SmoothBox->setLayout(SmoothLayout);

	QGridLayout *TELayout = new QGridLayout;
	QLabel *lab1 = new QLabel("T.E. Angle");
	QLabel *lab2 = new QLabel("T.E. Gap dx/c");
	QLabel *lab3 = new QLabel("T.E. Gap dy/c");
	lab1->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	lab2->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	lab3->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	m_pctrlTAngle = new FloatEdit("3.681");
	m_pctrlTGapx  = new FloatEdit("0.001");
	m_pctrlTGapy  = new FloatEdit("0.002");
	m_pctrlTAngle->SetPrecision(3);
	m_pctrlTGapx->SetPrecision(3);
	m_pctrlTGapy->SetPrecision(3);
	TELayout->addWidget(lab1,1,1);
	TELayout->addWidget(lab2,2,1);
	TELayout->addWidget(lab3,3,1);
	TELayout->addWidget(m_pctrlTAngle,1,2);
	TELayout->addWidget(m_pctrlTGapx,2,2);
	TELayout->addWidget(m_pctrlTGapy,3,2);
	m_pctrlSymm = new QCheckBox("Symmetric foil");
	QVBoxLayout *ConstraintsLayout = new QVBoxLayout;
	ConstraintsLayout->addLayout(TELayout);
	ConstraintsLayout->addWidget(m_pctrlSymm);
	QGroupBox *ConstraintsBox = new QGroupBox("Constraints");
	ConstraintsBox->setLayout(ConstraintsLayout);

	m_pctrlExec = new QPushButton("Execute");
	m_pctrlOutput = new QTextEdit("Output here");

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
	m_pctrlMSpec          = new QTextEdit("Alpha = Cl =");
	m_pctrlIter           = new FloatEdit("11");
	m_pctrlMark           = new QPushButton("Mark for modification");
	m_pctrlCpxx           = new QCheckBox("End Point Constraint");
	m_pctrlMExec          = new QPushButton("Execute");
	m_pctrlMOutput        = new QTextEdit("Output here");
	m_pctrlMShowSpline    = new QCheckBox("ShowSpline");
	m_pctrlMTangentSpline = new QCheckBox("Tangent Spline");
	m_pctrlMNewSpline     = new QPushButton("New Spline");
	m_pctrlMApplySpline   = new QPushButton("Apply Spline");
	m_pctrlMSmooth        = new QPushButton("Smooth");
	m_pctrlMResetQSpec    = new QPushButton("ResetQSpec");

//	m_pctrlMSpec->setMaximumHeight((int)((rect().height()/8)));

	QGridLayout *MSplineslayout = new QGridLayout;
	MSplineslayout->addWidget(m_pctrlMShowSpline,1,1);
	MSplineslayout->addWidget(m_pctrlMTangentSpline,1,2);
	MSplineslayout->addWidget(m_pctrlMNewSpline,2,1);
	MSplineslayout->addWidget(m_pctrlMApplySpline,2,2);
	MSplineslayout->addWidget(m_pctrlMark,3,1,1,2);
	MSplineslayout->addWidget(m_pctrlMSmooth,4,1);
	MSplineslayout->addWidget(m_pctrlMResetQSpec,4,2);
	QGroupBox *MSplinesBox = new QGroupBox("Modification");
	MSplinesBox->setLayout(MSplineslayout);

	QVBoxLayout *FoilLayout = new QVBoxLayout;
	FoilLayout->addWidget(m_pctrlCpxx);
	FoilLayout->addWidget(m_pctrlMExec);
	QHBoxLayout *MaxIter = new QHBoxLayout;
	QLabel *lab10 = new QLabel("Max Iterations");
	MaxIter->addWidget(lab10);
	MaxIter->addWidget(m_pctrlIter);
	FoilLayout->addLayout(MaxIter);
	QGroupBox *FoilBox = new QGroupBox("Foil");

	QVBoxLayout *MInvLayout = new QVBoxLayout;
	MInvLayout->addWidget(m_pctrlMSpec);
	MInvLayout->addWidget(MSplinesBox);
	MInvLayout->addWidget(FoilBox);
	MInvLayout->addWidget(m_pctrlMOutput);
	MInvLayout->addStretch(1);
	m_pctrlMInvWidget = new QWidget(this);
	m_pctrlMInvWidget->setLayout(MInvLayout);

	m_pctrlStackedInv = new QStackedWidget;
	m_pctrlStackedInv->addWidget(m_pctrlFInvWidget);
	m_pctrlStackedInv->addWidget(m_pctrlMInvWidget);
}


void QXInverse::SetScale(QRect CltRect)
{
	m_rCltRect = CltRect;

	int h = CltRect.height();
	int h8 = (int)(h/8.0);
	int w = CltRect.width();
	int w20 = (int)(w/20);
	m_rGraphRect = QRect(w20, 10, + m_rCltRect.width()-2*w20, m_rCltRect.height()-h8);
	m_QGraph.SetDrawRect(m_rGraphRect);


	m_fRefScale  = m_rGraphRect.width()-2.0*m_QGraph.GetMargin();
	m_fScale     = m_fRefScale;


	m_ptOffset.rx() = m_rGraphRect.left() +(int)(1.0*m_QGraph.GetMargin());
	m_ptOffset.ry() = m_rCltRect.bottom()-h8/2.0;

}


void QXInverse::showEvent(QShowEvent *event)
{
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
//		SHORT shX = GetKeyState('X');
//		SHORT shY = GetKeyState('Y');

/*		if (shX & 0x8000)
		{
			//zoom x scale
			m_QGraph.SetAutoX(false);
			if(zDelta>0) m_QGraph.Scalex(1.06);
			else         m_QGraph.Scalex(1.0/1.06);
		}
		else if(shY & 0x8000)
		{
			//zoom y scale
			m_QGraph.SetAutoY(false);
			if(zDelta>0) m_QGraph.Scaley(1.06);
			else         m_QGraph.Scaley(1.0/1.06);
		}
		else */
		{
			//zoom both
			m_QGraph.SetAuto(false);
			if(event->delta()>0) m_QGraph.Scale(1.06);
			else               m_QGraph.Scale(1.0/1.06);
		}
	}
	else
	{
		double scale = m_fScale;

		if(event->delta()<0) m_fScale *= 1.06;
		else         m_fScale /= 1.06;

		int a = (int)((m_rCltRect.right() + m_rCltRect.left())/2);
		m_ptOffset.rx() = a + (int)((m_ptOffset.x()-a)*m_fScale/scale);
	}
	UpdateView();
}





void QXInverse::SetFoil()
{
	int i;
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
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
//		ResetQ();

		m_pctrlSpec->SetValue(pXFoil->alqsp[1]*180.0/pi);
		m_pctrlTAngle->SetValue(pXFoil->agte*180.0);//agte expressed in pi units:!?!?
		m_pctrlTGapx->SetValue(real(pXFoil->dzte));
		m_pctrlTGapy->SetValue(imag(pXFoil->dzte));
	}
	else
	{
		// Mixed Inverse
		pXFoil->InitQDES();
		CreateQCurve();
		CreateMCurve();
//		ResetMixedQ();
		strong = QString("Alpha = %1 \r\n      Cl = %2")
								.arg(pXFoil->algam/pXFoil->dtor,0,'f',3).arg(pXFoil->clgam,0,'f',3);
		m_pctrlMSpec->setText(strong);
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


void QXInverse::ResetScale()
{
	m_ptOffset.rx() = m_rGraphRect.left() +(int)(1.0*m_QGraph.GetMargin());
	m_fRefScale  = m_rGraphRect.width()-2.0*m_QGraph.GetMargin();

	m_ptOffset.ry() = m_rCltRect.bottom()-100;
	m_fScale = m_fRefScale;
}


void QXInverse::SetRect(QRect CltRect)
{
	m_rCltRect = CltRect;
	m_rGraphRect = m_rCltRect;
	m_rGraphRect.adjust(-20,-20,-20,-200);

	m_Spline.m_rViewRect = m_rGraphRect;
	ResetScale();
}
