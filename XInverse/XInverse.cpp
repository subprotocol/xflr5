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

#include "../Globals.h"
#include "../MainFrame.h"
#include "XInverse.h"
#include "../Objects/Foil.h"


QXInverse::QXInverse(QWidget *parent)
	: QWidget(parent)
{
	m_hcArrow = QCursor(Qt::ArrowCursor);
	m_hcCross = QCursor(Qt::CrossCursor);
	m_hcMove  = QCursor(Qt::ClosedHandCursor);
	
	m_pXFoil = NULL;

	m_bTransGraph = false;
	m_bFullInverse = true;
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

	if(m_bFullInverse) setLayout(m_pctrlFInvLayout);
	else               setLayout(m_pctrlMInvLayout);

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
	m_pctrlSpec   = new FloatEdit("1.23");
	SpecLayout->addWidget(m_pctrlSpecAlpha,1,1);
	SpecLayout->addWidget(m_pctrlSpecCl,1,2);
	SpecLayout->addWidget(m_pctrlSpecif,2,1);
	SpecLayout->addWidget(m_pctrlSpec,2,2);
	QGroupBox *SpecBox = new QGroupBox("Specification");
	SpecBox->setLayout(SpecLayout);

	QVBoxLayout *ModLayout = new QVBoxLayout;
	m_pctrlShowSpline = new QCheckBox("ShowSpline");
	m_pctrlTangentSpline = new QCheckBox("Spline tangent to curve");
	m_pctrlNewSpline   = new QPushButton("New Spline");
	m_pctrlApplySpline = new QPushButton("Apply Spline");
	m_pctrlResetQSpec  = new QPushButton("Reset QSpec");
	m_pctrlPert        = new QPushButton("Pert");
	ModLayout->addWidget(m_pctrlShowSpline);
	ModLayout->addWidget(m_pctrlTangentSpline);
	ModLayout->addWidget(m_pctrlNewSpline);
	ModLayout->addWidget(m_pctrlApplySpline);
	ModLayout->addWidget(m_pctrlResetQSpec);
	ModLayout->addWidget(m_pctrlPert);
	QGroupBox *ModBox = new QGroupBox("Modification");
	ModBox->setLayout(ModLayout);

	QVBoxLayout *SmoothLayout = new QVBoxLayout;
	m_pctrlSmooth = new QPushButton("Smooth QSpec");
	m_pctrlFilter = new QPushButton("Hannig Filter");
	QHBoxLayout *FilterLayout = new QHBoxLayout;
	QLabel *lab0 = new QLabel("Filter parameter");
	m_pctrlFilterParam = new FloatEdit("2.34");
	FilterLayout->addWidget(lab0);
	FilterLayout->addWidget(m_pctrlFilter);
	SmoothLayout->addWidget(m_pctrlSmooth);
	SmoothLayout->addWidget(m_pctrlFilter);
	SmoothLayout->addLayout(FilterLayout);
	QGroupBox *SmoothBox = new QGroupBox;
	SmoothBox->setLayout(SmoothLayout);

	QGridLayout *TELayout = new QGridLayout;
	QLabel *lab1 = new QLabel("T.E. Angle");
	QLabel *lab2 = new QLabel("T.E. Gap dx/c");
	QLabel *lab3 = new QLabel("T.E. Gap dy/c");
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

	m_pctrlFInvLayout = new QVBoxLayout;
	m_pctrlFInvLayout->addWidget(SpecBox);
	m_pctrlFInvLayout->addWidget(ModBox);
	m_pctrlFInvLayout->addWidget(SmoothBox);
	m_pctrlFInvLayout->addWidget(ConstraintsBox);
	m_pctrlFInvLayout->addWidget(m_pctrlExec);
	m_pctrlFInvLayout->addWidget(m_pctrlOutput);
	m_pctrlFInvLayout->addStretch(1);

	//specific MInv Controls
	m_pctrlMSpec = new QTextEdit("Alpha = \nCl = \n");
	m_pctrlIter = new FloatEdit("11");
	m_pctrlMark = new QPushButton("Mark for modification");
	m_pctrlCpxx = new QCheckBox("End Point Constraint");

	QVBoxLayout *MSplineslayout = new QVBoxLayout;
	MSplineslayout->addWidget(m_pctrlShowSpline);
	MSplineslayout->addWidget(m_pctrlTangentSpline);
	MSplineslayout->addWidget(m_pctrlNewSpline);
	MSplineslayout->addWidget(m_pctrlApplySpline);
	MSplineslayout->addWidget(m_pctrlMark);
	MSplineslayout->addWidget(m_pctrlSmooth);
	MSplineslayout->addWidget(m_pctrlResetQSpec);
	QGroupBox *MSplinesBox = new QGroupBox("Modification");
	MSplinesBox->setLayout(MSplineslayout);

	QVBoxLayout *FoilLayout = new QVBoxLayout;
	FoilLayout->addWidget(m_pctrlCpxx);
	FoilLayout->addWidget(m_pctrlExec);
	QHBoxLayout *MaxIter = new QHBoxLayout;
	QLabel *lab10 = new QLabel("Max Iterations");
	MaxIter->addWidget(lab10);
	MaxIter->addWidget(m_pctrlIter);
	FoilLayout->addLayout(MaxIter);
	QGroupBox *FoilBox = new QGroupBox("Foil");

	m_pctrlMInvLayout = new QVBoxLayout;
	m_pctrlMInvLayout->addWidget(m_pctrlMSpec);
	m_pctrlMInvLayout->addWidget(MSplinesBox);
	m_pctrlMInvLayout->addWidget(FoilBox);
	m_pctrlMInvLayout->addWidget(m_pctrlOutput);
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
}


void QXInverse::mouseMoveEvent(QMouseEvent *event)
{
}


void QXInverse::mousePressEvent(QMouseEvent *event)
{
}


void QXInverse::mouseReleaseEvent(QMouseEvent *event)
{
}


void QXInverse::keyPressEvent(QKeyEvent *event)
{
}


void QXInverse::keyReleaseEvent(QKeyEvent *event)
{
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
/*		QRect ZRect(m_ZoomRect.left()   - m_rCltRect.left(),
					m_ZoomRect.top()    - m_rCltRect.top(),
					m_ZoomRect.right()  - m_rCltRect.left(),
					m_ZoomRect.bottom() - m_rCltRect.top());
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

		m_Spline.DrawSpline(painter, 1.0/m_QGraph.GetXScale(), -1.0/m_QGraph.GetYScale(), m_QGraph.GetOffset());
		m_Spline.DrawCtrlPoints(painter, 1.0/m_QGraph.GetXScale(), -1.0/m_QGraph.GetYScale(), m_QGraph.GetOffset());
		
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














