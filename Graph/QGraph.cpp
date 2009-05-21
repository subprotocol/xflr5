/****************************************************************************

	QGraph Classes
	Copyright (C) 2008-2009 Andre Deperrois xflr5@yahoo.com

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


// Graph.cpp: implementation of the Graph class.
//
//////////////////////////////////////////////////////////////////////

#include "QGraph.h"
#include "../Globals.h"
#include <math.h>
#include <QPainter>
#include <QtDebug>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


QGraph::QGraph()
{
	//Type is used to determine automatic scales
	m_Type = 1;

	xmin         =  9999999.0;
	xmax         = -9999999.0;
	ymin         =  9999999.0;
	ymax         = -9999999.0;
	xunit        = 0.1;
	yunit        = 0.1;
	xo           = 0.0;
	yo           = 0.0;
	m_XMinorUnit = 0.01;
	m_YMinorUnit = 0.01;

	m_scalex     = 0.1;
	m_scaley     = 0.1;
	Cxmin        = 0.0;
	Cxmax        = 1.0;
	Cymin        = 0.0;
	Cymax        = 1.0;

	exp_x = 0;
	exp_y = 0;

	m_bYInverted  = false;
	m_bAutoX        = true;
	m_bAutoY        = true;
	m_bXAutoMinGrid = true;
	m_bYAutoMinGrid = true;
	m_bBorder       = true;
	m_bIsPrinting   = false;

	m_ptoffset.setX(0);
	m_ptoffset.setY(0);

	m_CurveColors[0] = QColor(255,   0,   0);
	m_CurveColors[1] = QColor(  0,   0, 255);
	m_CurveColors[2] = QColor(  0, 255,   0);
	m_CurveColors[3] = QColor(255, 255,   0);
	m_CurveColors[4] = QColor(  0, 255, 255);
	m_CurveColors[5] = QColor(255,   0, 255);
	m_CurveColors[6] = QColor(255, 125,  70);
	m_CurveColors[7] = QColor( 70, 125, 255);
	m_CurveColors[8] = QColor(125, 255,  70);
	m_CurveColors[9] = QColor(255, 70,  200);

	m_CurveStyles[0] = Qt::SolidLine;	// A plain line.
	m_CurveStyles[1] = Qt::DashLine;	// Dashes separated by a few pixels. 
	m_CurveStyles[2] = Qt::DotLine;		// Dots separated by a few pixels. 
	m_CurveStyles[3] = Qt::DashDotLine;	// Alternate dots and dashes. 
	m_CurveStyles[4] = Qt::DashDotDotLine;	// One dash, two dots, one dash, two dots.

	m_iMargin = 10;
	m_h       =  0;
	m_w       =  0;
	m_rDrawRect.SetRectEmpty();

	m_rCltRect.setRect(0,0, 200, 300);

	SetDefaults();
}


QGraph::~QGraph()
{
	DeleteCurves();
}


void QGraph::DrawGraph(QRect const &rect, QPainter &painter)
{
	m_rCltRect = rect;
	DrawGraph(painter);
}


void QGraph::DrawGraph(QPainter &painter)
{
	QColor color;
	painter.save();

//	Paint background
	QBrush bg(m_BkColor);
	painter.setBackground(bg);

//	Draw Border
	if(m_bBorder) color = m_BorderColor;
	else          color = m_BkColor;
	QPen BorderPen(color);
	BorderPen.setStyle(GetStyle(m_BorderStyle));
	BorderPen.setWidth(m_BorderWidth);

	painter.setPen(BorderPen);
	painter.fillRect(m_rCltRect, m_BkColor);
	painter.drawRect(m_rCltRect);
	Init();

	painter.setClipRect(m_rCltRect);

	if(m_bXMinGrid) DrawXMinGrid(painter);
	if(m_bYMinGrid) DrawYMinGrid(painter);
	if(m_bXMajGrid) DrawXMajGrid(painter);
	if(m_bYMajGrid) DrawYMajGrid(painter);

	DrawAxes(painter);

	DrawXTicks(painter);

	DrawYTicks(painter);

	for (int nc=0; nc < m_oaCurves.size(); nc++)	DrawCurve(nc,painter);

	DrawTitles(painter);

	painter.setClipping(false);
	painter.restore();
}

void QGraph::DrawCurve(int nIndex,QPainter &painter)
{
	painter.save();
	double scaley = m_scaley;
	int i;
	int ptside = 2;

	CCurve* pCurve = GetCurve(nIndex);

	QColor color;

	QBrush FillBrush(m_BkColor);
	painter.setBrush(FillBrush);

	QPen CurvePen(pCurve->GetColor());
	CurvePen.setStyle(GetStyle(pCurve->GetStyle()));
	CurvePen.setWidth((int)pCurve->GetWidth());
	painter.setPen(CurvePen);

	QPoint From, To;
	QPoint Min(int(xmin/m_scalex) +m_ptoffset.x(), int(ymin/scaley) +m_ptoffset.y());
	QPoint Max(int(xmax/m_scalex) +m_ptoffset.x(), int(ymax/scaley) +m_ptoffset.y());
	QRect rViewRect(Min, Max);

	if(pCurve->n>1 && pCurve->IsVisible())
	{
		From.setX(int(pCurve->x[0]/m_scalex+m_ptoffset.x()));
		From.setY(int(pCurve->y[0]/scaley+m_ptoffset.y()));

		if(pCurve->PointsVisible()) painter.drawRect(From.x()-ptside,From.y()-ptside, 2*ptside,2*ptside);

		for (i=1; i<pCurve->n;i++)
		{
			To.setX(int(pCurve->x[i]/m_scalex+m_ptoffset.x()));
			To.setY(int(pCurve->y[i]/scaley+m_ptoffset.y()));
			painter.drawLine(From, To);
			if(pCurve->PointsVisible())	painter.drawRect(To.x()-ptside,To.y()-ptside, 2*ptside,2*ptside);

			From = To;
		}
		if(pCurve->PointsVisible())
		{
			for (i=0; i<pCurve->n;i++)
			{
				painter.drawRect(int(pCurve->x[i]/m_scalex+m_ptoffset.x())-ptside,
								 int(pCurve->y[i]/  scaley+m_ptoffset.y())-ptside,
								 2*ptside,2*ptside);
			}
		}
	}
	else if(pCurve->n == 1 && pCurve->IsVisible() && pCurve->PointsVisible())
	{
		To.setX(int(pCurve->x[0]/m_scalex+m_ptoffset.x()));
		To.setY(int(pCurve->y[0]/scaley+m_ptoffset.y()));
		painter.drawRect(From.x()-ptside,From.y()-ptside, 2*ptside,2*ptside);
	}
	painter.restore();
}


void QGraph::DrawAxes(QPainter &painter)
{	
	double scaley = m_scaley;
	painter.save();

	QPen AxesPen(m_AxisColor);
	AxesPen.setStyle(GetStyle(m_nStyle));
	AxesPen.setWidth(m_Width);
	painter.setPen(AxesPen);


	//vertical axis
	double xp;
	if(xo>=xmin && xo<=xmax) xp = xo;
	else if(xo>xmax)         xp = xmax;
	else                     xp = xmin;

	painter.drawLine((int)(xp/m_scalex) + m_ptoffset.x(), (int)(ymin/scaley) + m_ptoffset.y(),
					 (int)(xp/m_scalex) + m_ptoffset.x(), (int)(ymax/scaley) + m_ptoffset.y());

	//horizontal axis
	double yp;
	if(yo>=ymin && yo<=ymax)	yp = yo;
	else if(yo>ymax)		yp = ymax;
	else				yp = ymin;


	painter.drawLine((int)(xmin/m_scalex) +m_ptoffset.x(), (int)(yp/scaley) + m_ptoffset.y(),
					 (int)(xmax/m_scalex) +m_ptoffset.x(), (int)( yp/scaley) + m_ptoffset.y());

	painter.restore();
}


void QGraph::DrawTitles(QPainter &painter)
{
	double scaley = m_scaley;
	//draws the x & y axis name
	int XPosXTitle, YPosXTitle;
	int XPosYTitle, YPosYTitle;

	painter.save();
	XPosXTitle = 5;
	YPosXTitle = -10;
	XPosYTitle = -5;
	YPosYTitle =  5;

	double xp;
	if(xo>=xmin && xo<=xmax) xp = xo;
	else if(xo>xmax)         xp = xmax;
	else					 xp = xmin;

	double yp;
	if(yo>=ymin && yo<=ymax) yp = yo;
	else if(yo>ymax)         yp = ymax;
	else                     yp = ymin;

	painter.setFont(m_TitleLogFont);

	QPen TitlePen(m_TitleColor);
	painter.setPen(TitlePen);

	painter.drawText(  (int)(xmax/m_scalex) + m_ptoffset.x() + XPosXTitle,
					   (int)(yp  /scaley)   + m_ptoffset.y() + YPosXTitle, m_XTitle);

	painter.drawText(  m_ptoffset.x() + (int)(xp/m_scalex)   + XPosYTitle,
					   m_rCltRect.top() + m_iMargin - YPosYTitle, m_YTitle);
	painter.restore();
}


void QGraph::DrawXTicks(QPainter &painter)
{
	double scaley = m_scaley;
	if(fabs(xunit)<0.00000001) return;
	if(fabs(xmax-xmin)/xunit>30.0) return;

	painter.save();
	QString strLabel, strLabelExp;

	painter.setFont(m_LabelLogFont);

	int exp;
	double main;
	int TickSize;
	int xExpOff, yExpOff, xMainOff;

	TickSize = 5;
	xExpOff  = 24;
	yExpOff  = 3;
	xMainOff = 14;

	QPen LabelPen(m_AxisColor);

	LabelPen.setStyle(GetStyle(m_nStyle));
	LabelPen.setWidth(m_Width);
	painter.setPen(LabelPen);

	double xt = xo-(xo-xmin);//one tick at the origin
	int  nx = (int)((xo-xmin)/xunit);
	xt = xo - nx*xunit;
//qDebug() <<"start" <<xt << xmin <<xo;

	double yp;
	if(yo>=ymin && yo<=ymax) yp = yo;
	else if(yo>ymax)         yp = ymax;
	else                     yp = ymin;

	while(xt<=xmax*1.0001)
	{
		//Draw ticks
		if(xt>=xmin)
		{
			painter.setPen(LabelPen);
			painter.drawLine(int(xt/m_scalex) + m_ptoffset.x(),int(yp/scaley) +TickSize + m_ptoffset.y(),
							 int(xt/m_scalex) + m_ptoffset.x(),int(yp/scaley)           + m_ptoffset.y());
			painter.setPen(m_LabelColor);

			if(exp_x>=4 || exp_x<=-4)
			{
				main = xt;
				ExpFormat(main, exp);

				strLabel = QString("%1 10").arg(main,5,'f',1);
				painter.drawText(int(xt/m_scalex) +xMainOff          +m_ptoffset.x(),
								 int(yp/scaley) +int(TickSize*1.5) +m_ptoffset.y(),
								 strLabel);
				strLabel = QString("%1").arg(exp);

				painter.drawText(int(xt/m_scalex) + xExpOff                     +m_ptoffset.x(),
								 int(yp/scaley) - yExpOff + int(TickSize*1.5) +m_ptoffset.y(),
								 strLabel);

			}
			else
			{
				if(exp_x>=0)        strLabel = QString("%1").arg(xt,6,'f',0);
				else if (exp_x>=-1) strLabel = QString("%1").arg(xt,6,'f',1);
				else if (exp_x>=-2) strLabel = QString("%1").arg(xt,6,'f',2);
				else if (exp_x>=-3) strLabel = QString("%1").arg(xt,6,'f',3);
				painter.drawText((int)(xt/m_scalex) - 20 + m_ptoffset.x(),
								 (int)(yp/scaley)   + 20 + m_ptoffset.y(),
								 strLabel);
			}
		}
		xt += xunit ;
	}
	painter.restore();
}




void QGraph::DrawYTicks(QPainter &painter)
{
	double scaley = m_scaley;
	if(fabs(xunit)<0.00000001) return;
	if(fabs(ymax-ymin)/yunit>30.0) return;

	painter.save();
	QString strLabel, strLabelExp;
	int exp;//TODO : exp used uninitialized
	double main;
	int TickSize;
	int xExpOff, yExpOff;

	painter.setFont(m_LabelLogFont);

	TickSize = 5;
	xExpOff  = 7;
	yExpOff  = 2;

	QPen LabelPen(m_AxisColor);
	LabelPen.setStyle(GetStyle(m_nStyle));
	LabelPen.setWidth(m_Width);

	double yt = ymin;
	double xp;

	if(xo>=xmin && xo<=xmax) xp = xo;
	else if(xo>xmax)         xp = xmax;
	else                     xp = xmin;

	while(yt<=ymax*1.0001)
	{
		//Draw ticks
		if(yt>=ymin)
		{
			painter.setPen(LabelPen);
			painter.drawLine((int)(xp/m_scalex)          +m_ptoffset.x(),(int)(yt/scaley)          +m_ptoffset.y(),
							 (int)(xp/m_scalex)-TickSize +m_ptoffset.x(),(int)(yt/scaley)          +m_ptoffset.y());

			painter.setPen(m_LabelColor);

			if(exp_y>=3 || exp_y<=-3)
			{
				main = yt;
				ExpFormat(main, exp);

				if(fabs(yt)<1.e-7)
				{
					main = 0.0;
					exp  = 0;
				}

				strLabel = QString("%1 10").arg(main,4,'f',1);
				strLabelExp= QString("%1").arg(exp);

				painter.drawText((int)(xp/m_scalex) - 50 + m_ptoffset.x(),
								 (int)(yt/scaley)   + 5 + m_ptoffset.y(),
								 strLabel);

				if(exp_y>=3)
				{
					painter.drawText(int(xp/m_scalex) - 25 + xExpOff + m_ptoffset.x(),
									 int(yt/scaley)   -  2 - yExpOff + m_ptoffset.y(),
									 strLabelExp);
				}
				else
				{
					painter.drawText(int(xp/m_scalex) - 25 + xExpOff + 2 + m_ptoffset.x(),
									 int(yt/scaley)   -  2 - yExpOff + m_ptoffset.y(),
									 strLabelExp);
				}
			}
			else
			{
				if(exp_y>=0)        strLabel = QString("%1").arg(yt,6,'f',0);
				else if (exp_y>=-1) strLabel = QString("%1").arg(yt,6,'f',1);
				else if (exp_y>=-2) strLabel = QString("%1").arg(yt,6,'f',2);
				else if (exp_y>=-3) strLabel = QString("%1").arg(yt,6,'f',3);

				painter.drawText((int)(xp/m_scalex) - 40 +m_ptoffset.x(),
								 (int)(yt/scaley)   +  5 +m_ptoffset.y(),
								 strLabel);

			}
		}
		yt += yunit ;

	}
	painter.restore();
}


void QGraph::DrawXMajGrid(QPainter &painter)
{
	double scaley = m_scaley;
	if(fabs(xunit)<0.00000001)     return;
	if(fabs(xmax-xmin)/xunit>30.0) return;

	painter.save();
	int YMin, YMax;

	QPen GridPen(m_XMajClr);

	GridPen.setStyle(GetStyle(m_XMajStyle));
	GridPen.setWidth(m_XMajWidth);
	painter.setPen(GridPen);

	YMin = (int)(ymin/scaley) + m_ptoffset.y();
	YMax = (int)(ymax/scaley) + m_ptoffset.y();


	double xt = xo-int((xo-xmin)*1.0001/xunit)*xunit;//one tick at the origin
	while(xt<=xmax*1.001)
	{
		if(xt>=xmin)
		{
			painter.drawLine(int(xt/m_scalex) + m_ptoffset.x(), YMin, int(xt/m_scalex) + m_ptoffset.x(), YMax);
		}
		xt += xunit ;
	}
	painter.restore();
}


void QGraph::DrawYMajGrid(QPainter &painter)
{
	double scaley = m_scaley;
	if(fabs(yunit)<0.00000001) return;
	if(fabs(ymax-ymin)/yunit>30.0) return;

	painter.save();
	int width;
	if(m_YMajWidth<=1) width = 1;

	QPen GridPen(m_YMajClr);

	GridPen.setStyle(GetStyle(m_YMajStyle));
	GridPen.setWidth(width);
	painter.setPen(GridPen);

	double yt = yo-int((yo-ymin)*1.0001/yunit)*yunit;//one tick at the origin

	int XMin = qMax((int)(xmin/m_scalex + m_ptoffset.x()), m_rCltRect.left());
	int XMax = qMin((int)(xmax/m_scalex + m_ptoffset.x()), m_rCltRect.right());

	while(yt<=ymax*1.0001)
	{
		if(yt>=ymin)
		{
			painter.drawLine(XMin, (int)(yt/scaley)   + m_ptoffset.y(), XMax, (int)(yt/scaley)   + m_ptoffset.y());
		}
		yt += yunit ;
	}
	painter.restore();
}

void QGraph::DrawXMinGrid(QPainter &painter)
{
	double scaley = m_scaley;
	if(fabs(xunit)<0.00000001) return;
	if(fabs(m_XMinorUnit)<0.00000001) return;
	if(fabs(xmax-xmin)/xunit>30.0) return;
	if(fabs(xmax-xmin)/m_XMinorUnit>100.0) return;
	int YMin, YMax;

	painter.save();
	QPen GridPen(m_XMinClr);
	GridPen.setStyle(GetStyle(m_XMinStyle));
	GridPen.setWidth(m_XMinWidth);
	painter.setPen(GridPen);


	YMin = (int)(ymin/scaley)+ m_ptoffset.y();
	YMax = (int)(ymax/scaley)+ m_ptoffset.y();

	double xDelta = m_XMinorUnit;
	double xt = xmin;//one tick at the origin

	while(xt<=xmax*1.001)
	{
		if(xt>=xmin)
		{
			painter.drawLine(int(xt/m_scalex) + m_ptoffset.x(), YMin, int(xt/m_scalex) + m_ptoffset.x(), YMax);
		}
		xt += xDelta;
	}
	painter.restore();
}

void QGraph::DrawYMinGrid(QPainter &painter)
{
	double scaley = m_scaley;
	if(fabs(yunit)<0.00000001) return;
	if(fabs(m_YMinorUnit)<0.00000001) return;
	if(fabs(ymax-ymin)/yunit>30.0) return;
	if(fabs(ymax-ymin)/m_YMinorUnit>100.0) return;

	painter.save();
	QPen GridPen(m_YMinClr);
	GridPen.setStyle(GetStyle(m_YMinStyle));
	GridPen.setWidth(m_YMinWidth);
	painter.setPen(GridPen);

	double yDelta = m_YMinorUnit;
	double yt = yo-int((yo-ymin)*1.0001/yDelta)*yDelta;//one tick at the origin
	int XMin = qMax((int)(xmin/m_scalex + m_ptoffset.x()), m_rCltRect.left());
	int XMax = qMin((int)(xmax/m_scalex + m_ptoffset.x()), m_rCltRect.right());

	while(yt<=ymax*1.0001)
	{
		if(yt>=ymin)
		{
			painter.drawLine(XMin, (int)(yt/scaley)   + m_ptoffset.y(), XMax, (int)(yt/scaley)   + m_ptoffset.y());
		}
		yt += yDelta ;
	}
	painter.restore();
}


void QGraph::DrawLegend(QPainter &painter, QPoint &Place)
{
	painter.save();
	int LegendSize, ypos;
	QString strong, strLabel;

	LegendSize = 30;
	ypos = 12;

	painter.setFont(m_LegendLogFont);

	CCurve* pCurve;

	QPen TextPen(m_LegendColor);
	QPen LegendPen(Qt::gray);

	int npos = 0;
	for (int nc=0; nc< m_oaCurves.size(); nc++)
	{
		pCurve = (CCurve*) m_oaCurves[nc];
		if(pCurve->IsVisible())
		{
			pCurve->GetTitle(strong);
			if(pCurve->n>0 && strong.length())//is there anything to draw ?
			{

				LegendPen.setColor(pCurve->GetColor());
				LegendPen.setStyle(GetStyle(pCurve->GetStyle()));
				LegendPen.setWidth(pCurve->GetWidth());

				painter.setPen(LegendPen);

				painter.drawLine(Place.x(),                     Place.y() + ypos*npos,
								 Place.x() + (int)(LegendSize), Place.y() + ypos*npos);

				painter.setPen(TextPen);
				painter.drawText(Place.x() + (int)(1.5*LegendSize),    Place.y()  + ypos*npos+(int)(ypos/2),
								 strong);

				npos++;
			}
		}
	}

	painter.restore();

}

void QGraph::ExpFormat(double &f, int &exp)
{

	if (f==0.0)
	{
		exp = 0;
		f = 0.0;
		return;
	}
	double f1 = fabs(f);
	if(f1<1)
		exp = (int)log10(f1)-1;
	else
		exp = (int)log10(f1);

	f = f/pow(10.0,exp);

}


void QGraph::ExportToFile(QFile &XFile, int FileType)
{
	int i,j;
	CCurve *pCurve;
	QString strong;
	QTextStream out(&XFile);

	for(i=0; i<(int)m_oaCurves.size(); i++)
	{
		pCurve = GetCurve(i);
		if(pCurve && pCurve->n>0)
		{
			pCurve->GetTitle(strong);

			if(FileType==1)	out << "       x               "+strong+" \n";
			else            out << "x,"+strong+"\n";

			for(j=0; j<pCurve->n; j++)
			{
				if(FileType==1)	strong= QString("%1     %2\n")
												.arg(pCurve->x[j],14,'e',5)
												.arg(pCurve->y[j],14,'e',5);
				else            strong= QString("%1, %2\n")
												.arg(pCurve->x[j],14,'e',5)
												.arg(pCurve->y[j],14,'e',5);

				out << strong;
			}
			out << "\n\n";
		}
	}
	XFile.close();
}

QPoint QGraph::GetOffset()
{
	return m_ptoffset;
}


void QGraph::Highlight(QPainter &painter, CCurve *pCurve, int ref)
{
	painter.save();
	int x = int(pCurve->x[ref]/m_scalex)  +m_ptoffset.x();
	int y = int(pCurve->y[ref]/m_scaley)  +m_ptoffset.y();

	QPen HighlightPen(QColor(255,100,100));
	HighlightPen.setWidth(2);
	painter.setPen(HighlightPen);
	QRect r(x-3,y-3,6,6);;
	painter.drawRect(r);
	painter.restore();
}


bool QGraph::Serialize(QDataStream &ar, bool bIsStoring)
{
	int k;
	QColor cr, cr1, cr2;
	QString str;
	QFont lgft;
	bool bs, ba;
	int s,w;
	int r,g,b;
	double f;

	if(bIsStoring)
	{
		//write variables
		ar << GetAxisColor().red()<< GetAxisColor().green() << GetAxisColor().blue();
		ar << GetAxisStyle();
		ar << GetAxisWidth();

		ar << GetTitleColor().red()  << GetTitleColor().green()  << GetTitleColor().blue();
		ar << GetLabelColor().red()  << GetLabelColor().green()  << GetLabelColor().blue();
		ar << GetLegendColor().red() << GetLegendColor().green() << GetLegendColor().blue();

		GetTitleLogFont(&lgft);
		ar << lgft;

		GetLabelLogFont(&lgft);
		ar << lgft;

		GetLegendLogFont(&lgft);
		ar << lgft;

		GetXMajGrid(bs,cr,s,w);
		ar << bs << cr.red() << cr.green() << cr.blue() << s << w;

		GetYMajGrid(bs,cr,s,w);
		ar << bs << cr.red() << cr.green() << cr.blue() << s << w;

		GetXMinGrid(bs,ba,cr,s,w,f);
		ar << bs << ba << cr.red() << cr.green() << cr.blue() << s << w << f;

		GetYMinGrid(bs,ba,cr,s,w,f);
		ar << bs << ba << cr.red() << cr.green() << cr.blue() << s << w << f;

		ar << m_bBorder << m_BorderColor.red() << m_BorderColor.green() << m_BorderColor.blue() << m_BorderStyle << m_BorderWidth;
		ar << m_BkColor.red() << m_BkColor.green() << m_BkColor.blue();

		ar << m_bYInverted;
	}
	else
	{
		//read variables
		ar >>  r>>g>>b;
		SetAxisColor(QColor(r,g,b));

		ar >> k;
		SetAxisStyle(k);
		ar >> k;
		SetAxisWidth(k);

		ar >>  r>>g>>b;
		SetTitleColor(QColor(r,g,b));
		ar >>  r>>g>>b;
		SetLabelColor(QColor(r,g,b));
		ar >>  r>>g>>b;
		SetLegendColor(QColor(r,g,b));

		ar >> lgft;
		SetTitleLogFont(&lgft);

		ar >> lgft;
		SetLabelLogFont(&lgft);

		ar >> lgft;
		SetLegendLogFont(&lgft);

		ar >> bs >> r>>g>>b >> s >> w;

		if(s <0 || s> 10)            return false;
		if(w <0 || w> 10)            return false;
		SetXMajGrid(bs,QColor(r,g,b),s,w);

		ar >> bs >> r>>g>>b >> s >> w;
		if(s <0 || s> 10)            return false;
		if(w <0 || w> 10)            return false;
		SetYMajGrid(bs,QColor(r,g,b),s,w);

		ar >> bs >> ba >> r>>g>>b >> s >> w >> f;
		if(s <0 || s> 10)            return false;
		if(w <0 || w> 10)            return false;
		if(f <0.0)            return false;
		SetXMinGrid(bs,ba,QColor(r,g,b),s,w,f);

		ar >> bs >> ba >> r>>g>>b >> s >> w >> f;
		if(s <0 || s> 10)            return false;
		if(w <0 || w> 10)            return false;
		if(f <0.0)                   return false;
		SetYMinGrid(bs,ba,QColor(r,g,b),s,w,f);

		ar >> m_bBorder >> r>>g>>b >> s >> w;
		if(s <0 || s> 10)            return false;
		if(w <0 || w> 10)            return false;
		SetBorderColor(QColor(r,g,b));

		SetBorderStyle(s);
		SetBorderWidth(w);

		ar >> r>>g>>b;
		SetBkColor(QColor(r,g,b));

		ar >> m_bYInverted ;
	}
	return true;
}
