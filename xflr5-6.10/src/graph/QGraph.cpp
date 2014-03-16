/****************************************************************************

	QGraph Classes
	Copyright (C) 2008-2014 Andre Deperrois adeperrois@xflr5.com

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



#include "QGraph.h"
#include "../globals.h"
#include <math.h>
#include <QPainter>
#include <QFontMetrics>
#include <QtDebug>

QGraph::QGraph()
{
	m_bHighlightPoint = false;

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
	static QColor color;
	painter.save();

//	Draw Border
	if(m_bBorder) color = m_BorderColor;
	else          color = m_BkColor;
	QPen BorderPen(color);
	BorderPen.setStyle(getStyle(m_BorderStyle));
	BorderPen.setWidth(m_BorderWidth);

	painter.setPen(BorderPen);
	painter.fillRect(m_rCltRect, m_BkColor);
	painter.drawRect(m_rCltRect);
	Init();

	painter.setClipRect(m_rCltRect);

	painter.setBackgroundMode(Qt::TransparentMode);

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

void QGraph::DrawCurve(int nIndex, QPainter &painter)
{
	painter.save();
	static double scaley;
	static int i, ptside;
	static QPoint From, To, Min, Max;
	static QRect rViewRect;

	ptside = 2;
	Curve* pCurve = GetCurve(nIndex);

	scaley = m_scaley;

	QBrush FillBrush(m_BkColor);
	painter.setBrush(FillBrush);

	QPen CurvePen(pCurve->color());
	CurvePen.setStyle(getStyle(pCurve->style()));
	CurvePen.setWidth((int)pCurve->width());
	painter.setPen(CurvePen);

	Min.setX(int(xmin/m_scalex) +m_ptoffset.x());
	Min.setY(int(ymin/scaley) +m_ptoffset.y());
	Max.setX(int(xmax/m_scalex) +m_ptoffset.x());
	Max.setY(int(ymax/scaley) +m_ptoffset.y());
	rViewRect.setTopLeft(Min);
	rViewRect.setBottomRight(Max);

	if(pCurve->size()>=1)
	{
		From.setX(int(pCurve->x[0]/m_scalex+m_ptoffset.x()));
		From.setY(int(pCurve->y[0]/scaley  +m_ptoffset.y()));

		if(pCurve->IsVisible())
		{
			for (i=1; i<pCurve->size();i++)
			{
				To.setX(int(pCurve->x[i]/m_scalex+m_ptoffset.x()));
				To.setY(int(pCurve->y[i]/scaley  +m_ptoffset.y()));
				painter.drawLine(From, To);

				From = To;
			}
		}

		if(pCurve->PointsVisible())
		{
			for (i=0; i<pCurve->size();i++)
			{
				if(pCurve->selected() !=i)
					painter.drawRect(int(pCurve->x[i]/m_scalex+m_ptoffset.x())-ptside,
									 int(pCurve->y[i]/  scaley+m_ptoffset.y())-ptside,
									 2*ptside,2*ptside);
			}
		}
	}

	if(m_bHighlightPoint)
	{
		int point = pCurve->selected();
		if(point>=0)
		{
			//highlight
			QColor HighColor(0,40, 150);
			CurvePen.setWidth((int)pCurve->width());
			CurvePen.setColor(HighColor);
			painter.setPen(CurvePen);
			To.setX(int(pCurve->x[point]/m_scalex+m_ptoffset.x()));
			To.setY(int(pCurve->y[point]/scaley  +m_ptoffset.y()));
			painter.drawRect(To.x()-ptside-1,To.y()-ptside-1, 2*(ptside+1),2*(ptside+1));
		}
	}
	painter.restore();
}


void QGraph::DrawAxes(QPainter &painter)
{
	static double xp, yp, scaley;
	static QPen AxesPen;
	scaley = m_scaley;
	painter.save();

	AxesPen.setColor(m_AxisColor);
	AxesPen.setStyle(getStyle(m_AxisStyle));
	AxesPen.setWidth(m_AxisWidth);
	painter.setPen(AxesPen);

	//vertical axis
	if(xo>=xmin && xo<=xmax) xp = xo;
	else if(xo>xmax)         xp = xmax;
	else                     xp = xmin;

	painter.drawLine((int)(xp/m_scalex) + m_ptoffset.x(), (int)(ymin/scaley) + m_ptoffset.y(),
					 (int)(xp/m_scalex) + m_ptoffset.x(), (int)(ymax/scaley) + m_ptoffset.y());

	//horizontal axis
	if(yo>=ymin && yo<=ymax) yp = yo;
	else if(yo>ymax)         yp = ymax;
	else                     yp = ymin;


	painter.drawLine((int)(xmin/m_scalex) +m_ptoffset.x(), (int)(yp/scaley) + m_ptoffset.y(),
					 (int)(xmax/m_scalex) +m_ptoffset.x(), (int)( yp/scaley) + m_ptoffset.y());

	painter.restore();
}


void QGraph::DrawTitles(QPainter &painter)
{
	//draws the x & y axis name
	static double scaley;
	static int XPosXTitle, YPosXTitle, XPosYTitle, YPosYTitle;
	static double xp, yp;

	scaley = m_scaley;
	painter.save();
	XPosXTitle = 5;
	YPosXTitle = -10;
	XPosYTitle = -5;
	YPosYTitle =  5;

	if(xo>=xmin && xo<=xmax) xp = xo;
	else if(xo>xmax)         xp = xmax;
	else					 xp = xmin;

	if(yo>=ymin && yo<=ymax) yp = yo;
	else if(yo>ymax)         yp = ymax;
	else                     yp = ymin;

	painter.setFont(m_TitleFont);

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
	static double main, scaley, xt, yp;
	static int exp, TickSize, height, yExpOff, nx;

	exp=0;

	if(qAbs(xunit)<0.00000001) return;
	if(qAbs(xmax-xmin)/xunit>30.0) return;

	scaley = m_scaley;
	painter.save();
	QString strLabel, strLabelExp;

	QFontMetrics fm(m_LabelFont);

	painter.setFont(m_LabelFont);


	TickSize = 5;
	height  = fm.height()/2;
	yExpOff = height/2;


	QPen LabelPen(m_AxisColor);

	LabelPen.setStyle(getStyle(m_AxisStyle));
	LabelPen.setWidth(m_AxisWidth);
	painter.setPen(LabelPen);
	xt = xo-(xo-xmin);//one tick at the origin
	nx = (int)((xo-xmin)/xunit);
	xt = xo - nx*xunit;

	if(yo>=ymin && yo<=ymax) yp = yo;
	else if(yo>ymax)         yp = ymax;
	else                     yp = ymin;

	int iTick = 0;

	while(fabs(xunit)>0.0 && xt<=xmax*1.0001 && iTick<100)
	{
		//Draw ticks
		if(xt>=xmin)
		{
			painter.setPen(LabelPen);
			painter.drawLine(int(xt/m_scalex) + m_ptoffset.x(),int(yp/scaley) +TickSize + m_ptoffset.y(),
						  int(xt/m_scalex) + m_ptoffset.x(),int(yp/scaley)           + m_ptoffset.y());
			painter.setPen(m_LabelColor);

			if(fabs(xt)<PRECISION)
			{

				strLabel = "0";
				painter.drawText((int)(xt/m_scalex) - fm.width(strLabel)/2 + m_ptoffset.x(),
							  (int)(yp/scaley)   + TickSize*2 +height   + m_ptoffset.y(),
							  strLabel);
			}
			else if(exp_x>=4 || exp_x<=-4)
			{
				main = xt;
				ExpFormat(main, exp);

				strLabel = QString("%1 10").arg(main,5,'f',1);
				painter.drawText(int(xt/m_scalex) - fm.width(strLabel)/2  +m_ptoffset.x(),
								 int(yp/scaley)   + TickSize*2 +height    +m_ptoffset.y(),
								 strLabel);
				strLabelExp = QString("%1").arg(exp);

				painter.drawText(int(xt/m_scalex) + fm.width(strLabel)/2       +m_ptoffset.x(),
								 int(yp/scaley)   + TickSize*2 +height-yExpOff +m_ptoffset.y(),
								 strLabelExp);
			}
			else
			{
				if(exp_x>0)         strLabel = QString("%1").arg(xt,0,'f',0);
				else if (exp_x>=-1) strLabel = QString("%1").arg(xt,6,'f',1);
				else if (exp_x>=-2) strLabel = QString("%1").arg(xt,6,'f',2);
				else if (exp_x>=-3) strLabel = QString("%1").arg(xt,6,'f',3);
				painter.drawText((int)(xt/m_scalex) - fm.width(strLabel)/2 + m_ptoffset.x(),
								 (int)(yp/scaley)   + TickSize*2 +height   + m_ptoffset.y(),
								 strLabel);
			}
		}
		xt += xunit ;
		iTick++;
	}
	painter.restore();
}




void QGraph::DrawYTicks(QPainter &painter)
{
	static double scaley, xp, main, yt;
	static int TickSize, fmheight, fmheight4, exp;
	if(qAbs(xunit)<0.00000001) return;
	if(qAbs(ymax-ymin)/yunit>30.0) return;
	scaley = m_scaley;
	painter.save();
	QString strLabel, strLabelExp;
	exp = 0;
	QFontMetrics fm(m_LabelFont);
	painter.setFont(m_LabelFont);

	fmheight  = fm.height();
	fmheight4 = (int)((double)fmheight/4.0);

	TickSize = 5;

	QPen LabelPen(m_AxisColor);
	LabelPen.setStyle(getStyle(m_AxisStyle));
	LabelPen.setWidth(m_AxisWidth);


	if(xo>=xmin && xo<=xmax) xp = xo;
	else if(xo>xmax)         xp = xmax;
	else                     xp = xmin;

	yt = yo-int((yo-ymin)*1.0001/yunit)*yunit;//one tick at the origin

	int iTick=0;

	while(fabs(yunit)>PRECISION && yt<=ymax*1.0001 && iTick<100)
	{
		//Draw ticks
		if(yt>=ymin)
		{
			painter.setPen(LabelPen);
			painter.drawLine((int)(xp/m_scalex)          + m_ptoffset.x(), (int)(yt/scaley) + m_ptoffset.y(),
					    (int)(xp/m_scalex)-TickSize + m_ptoffset.x(), (int)(yt/scaley) + m_ptoffset.y());

			painter.setPen(m_LabelColor);


			if(fabs(yt)<PRECISION)
			{
				strLabel = "0";
				painter.drawText((int)(xp/m_scalex) - fm.width(strLabel)-TickSize*2 +m_ptoffset.x(),
							  (int)(yt/scaley)   + fmheight4 +m_ptoffset.y(),
							  strLabel);
			}
			else if(abs(exp_y)>=4)
			{
				main = yt;
				ExpFormat(main, exp);

				strLabel    = QString("%1 10").arg(main,5,'f',1);
				strLabelExp = QString("%1").arg(exp);

				painter.drawText((int)(xp/m_scalex)   - fm.width(strLabel)-TickSize*3 + m_ptoffset.x(),
								 (int)(yt/scaley) + fmheight4                     + m_ptoffset.y(),
								 strLabel);

				if(exp_y>=4)
				{
					painter.drawText(int(xp/m_scalex)   - TickSize*3 + m_ptoffset.x(),
									 int(yt/scaley)              + m_ptoffset.y(),
									 strLabelExp);
				}
				else
				{
					painter.drawText(int(xp/m_scalex)   - TickSize*3 + 2 + m_ptoffset.x(),
									 int(yt/scaley)                  + m_ptoffset.y(),
									 strLabelExp);
				}
			}
			else
			{
				if(exp_y>=0)        strLabel = QString("%1").arg(yt,6,'f',0);
				else if (exp_y>=-1) strLabel = QString("%1").arg(yt,6,'f',1);
				else if (exp_y>=-2) strLabel = QString("%1").arg(yt,6,'f',2);
				else if (exp_y>=-3) strLabel = QString("%1").arg(yt,6,'f',3);

				painter.drawText((int)(xp/m_scalex)   - fm.width(strLabel)-TickSize*2 +m_ptoffset.x(),
								 (int)(yt/scaley) + fmheight4 +m_ptoffset.y(),
								 strLabel);
			}
		}
		yt += yunit ;
		iTick++;
	}
	painter.restore();
}


void QGraph::DrawXMajGrid(QPainter &painter)
{
	double scaley = m_scaley;
	if(qAbs(xunit)<0.00000001)     return;
	if(qAbs(xmax-xmin)/xunit>30.0) return;

	painter.save();
	int YMin, YMax;

	QPen GridPen(m_XMajClr);

	GridPen.setStyle(getStyle(m_XMajStyle));
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
	if(qAbs(yunit)<0.00000001) return;
	if(qAbs(ymax-ymin)/yunit>30.0) return;

	painter.save();
	int width;
	if(m_YMajWidth<=1) width = 1;

	QPen GridPen(m_YMajClr);

	GridPen.setStyle(getStyle(m_YMajStyle));
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
	if(qAbs(xunit)<0.00000001) return;
	if(qAbs(m_XMinorUnit)<0.00000001) return;
	if(qAbs(xmax-xmin)/xunit>30.0) return;
	if(qAbs(xmax-xmin)/m_XMinorUnit>100.0) return;
	int YMin, YMax;

	painter.save();
	QPen GridPen(m_XMinClr);
	GridPen.setStyle(getStyle(m_XMinStyle));
	GridPen.setWidth(m_XMinWidth);
	painter.setPen(GridPen);


	YMin = (int)(ymin/scaley)+ m_ptoffset.y();
	YMax = (int)(ymax/scaley)+ m_ptoffset.y();

	double xDelta = m_XMinorUnit;
	double xt = xo-int((xo-xmin)*1.0001/xDelta)*xDelta;//one tick at the origin


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
	if(qAbs(yunit)<0.00000001) return;
	if(qAbs(m_YMinorUnit)<0.00000001) return;
	if(qAbs(ymax-ymin)/yunit>30.0) return;
	if(qAbs(ymax-ymin)/m_YMinorUnit>100.0) return;

	painter.save();
	QPen GridPen(m_YMinClr);
	GridPen.setStyle(getStyle(m_YMinStyle));
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


void QGraph::DrawLegend(QPainter &painter, QPoint &Place, QFont &LegendFont, QColor &LegendColor)
{
	painter.save();
	int LegendSize, ypos;
	QString strong;

	LegendSize = 30;
	ypos = 12;

	painter.setFont(LegendFont);

	Curve* pCurve;

	QPen TextPen(LegendColor);
	QPen LegendPen(Qt::gray);

	int npos = 0;
	for (int nc=0; nc< m_oaCurves.size(); nc++)
	{
		pCurve = (Curve*) m_oaCurves[nc];
		if(pCurve->IsVisible())
		{
			pCurve->title(strong);
			if(pCurve->size()>0 && strong.length())//is there anything to draw ?
			{

				LegendPen.setColor(pCurve->color());
				LegendPen.setStyle(getStyle(pCurve->style()));
				LegendPen.setWidth(pCurve->width());

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
	double f1 = qAbs(f);
	if(f1<1)
		exp = (int)log10(f1)-1;
	else
		exp = (int)log10(f1);

	f = f/pow(10.0,exp);

}


void QGraph::ExportToFile(QFile &XFile, enumTextFileType FileType)
{
	int i,j, maxpoints;
	Curve *pCurve;
	QString strong;
	QTextStream out(&XFile);

	maxpoints = 0;
	for(i=0; i<m_oaCurves.size(); i++)
	{
		pCurve = GetCurve(i);
		if(pCurve)
		{
			maxpoints = qMax(maxpoints,pCurve->size());

			pCurve->title(strong);
			if(FileType==TXT) out << "     "<<m_XTitle<<"       "<< strong <<"    ";
			else              out << m_XTitle<<","<< strong << ", , ";

		}
	}
	out<<"\n"; //end of title line

	for(j=0; j<maxpoints; j++)
	{
		for(i=0; i<m_oaCurves.size(); i++)
		{
			pCurve = GetCurve(i);
			if(pCurve && j<pCurve->size())
			{
				if(FileType==TXT) strong= QString("%1     %2  ")
												.arg(pCurve->x[j],13,'g',7).arg(pCurve->y[j],13,'g',7);
				else              strong= QString("%1, %2, , ")
												.arg(pCurve->x[j],13,'g',7).arg(pCurve->y[j],13,'g',7);
			}
			else
			{
				if(FileType==TXT) strong= "                                 ";
				else              strong= ", , , ";
			}
			out << strong;
		}
		out<<"\n"; //end of data line
	}
	out<<"\n"; //end of file
	XFile.close();
}



QPoint QGraph::GetOffset()
{
	return m_ptoffset;
}


void QGraph::Highlight(QPainter &painter, Curve *pCurve, int ref)
{
	if(!pCurve) return;
	if(ref<0 || ref>pCurve->size()-1) return;

	painter.save();
	int x = int(pCurve->x[ref]/m_scalex)  +m_ptoffset.x();
	int y = int(pCurve->y[ref]/m_scaley)  +m_ptoffset.y();

	QPen HighlightPen(QColor(255,100,100));
	HighlightPen.setWidth(2);
	painter.setPen(HighlightPen);
	QRect r(x-3,y-3,6,6);
	painter.drawRect(r);
	painter.restore();
}


void QGraph::SaveSettings(QSettings *pSettings)
{
	QFont lgft;
	QColor clr;
	int k,s,w;
	bool ba, bs;
	double f;

	pSettings->beginGroup(m_GraphName);
	{
		//read variables
		clr = GetAxisColor();
		pSettings->setValue("AxisColor", clr);
		k = GetAxisStyle();
		pSettings->setValue("AxisStyle", k);
		k = GetAxisWidth();
		pSettings->setValue("AxisWidth", k);

		clr = GetTitleColor();
		pSettings->setValue("TitleColor", clr);

		clr = GetLabelColor();
		pSettings->setValue("LabelColor", clr);

		GetTitleFont(lgft);
		pSettings->setValue("TitleFontName", lgft.family());
		pSettings->setValue("TitleFontSize", lgft.pointSize());
		pSettings->setValue("TitleFontItalic", lgft.italic());
		pSettings->setValue("TitleFontBold", lgft.bold());

		GetLabelFont(lgft);
		pSettings->setValue("LabelFontName", lgft.family());
		pSettings->setValue("LabelFontSize", lgft.pointSize());
		pSettings->setValue("LabelFontItalic", lgft.italic());
		pSettings->setValue("LabelFontBold", lgft.bold());

		GetXMajGrid(bs,clr,s,w);
		pSettings->setValue("XMajGridColor", clr);
		pSettings->setValue("XMajGridShow",bs);
		pSettings->setValue("XMajGridStyle",s);
		pSettings->setValue("XMajGridWidth",w);

		GetYMajGrid(bs,clr,s,w);
		pSettings->setValue("YMajGridColor", clr);
		pSettings->setValue("YMajGridShow",bs);
		pSettings->setValue("YMajGridStyle",s);
		pSettings->setValue("YMajGridWidth",w);

		GetXMinGrid(bs,ba,clr,s,w,f);
		pSettings->setValue("XMinGridColor", clr);
		pSettings->setValue("XMinGridAuto",ba);
		pSettings->setValue("XMinGridShow",bs);
		pSettings->setValue("XMinGridStyle",s);
		pSettings->setValue("XMinGridWidth",w);
		pSettings->setValue("XMinGridUnit",f);

		GetYMinGrid(bs,ba,clr,s,w,f);
		pSettings->setValue("YMinGridColor", clr);
		pSettings->setValue("YMinGridAuto",ba);
		pSettings->setValue("YMinGridShow",bs);
		pSettings->setValue("YMinGridStyle",s);
		pSettings->setValue("YMinGridWidth",w);
		pSettings->setValue("YMinGridUnit",f);

		clr = GetBorderColor();
		s   = GetBorderStyle();
		w   = GetBorderWidth();
		pSettings->setValue("BorderColor", clr);
		pSettings->setValue("BorderStyle", s);
		pSettings->setValue("BorderWidth", w);
		pSettings->setValue("BorderShow", m_bBorder);

		clr = GetBackColor();
		pSettings->setValue("BackgroundColor", clr);

		pSettings->setValue("margin", m_iMargin);

		pSettings->setValue("Inverted", m_bYInverted);

		pSettings->setValue("XVariable", m_X);
		pSettings->setValue("YVariable", m_Y);
	}
	pSettings->endGroup();
}


void QGraph::LoadSettings(QSettings *pSettings)
{
	QFont lgft;
	bool bs, ba;
	int s,w;

	double f;
	QColor clr;

	pSettings->beginGroup(m_GraphName);
	{
		//read variables
		clr = pSettings->value("AxisColor", QColor(255,255,255)).value<QColor>();
		s = pSettings->value("AxisStyle",0).toInt();
		w = pSettings->value("AxisWidth",1).toInt();
		SetAxisData(s,w,clr);

		clr = pSettings->value("TitleColor", QColor(255,255,255)).value<QColor>();
		SetTitleColor(clr);
		clr = pSettings->value("LabelColor", QColor(255,255,255)).value<QColor>();
		SetLabelColor(clr);

		lgft = QFont(pSettings->value("TitleFontName","Comic Sans MS").toString());
		int size = pSettings->value("TitleFontSize",8).toInt();
		if(size>0) lgft.setPointSize(size);
		lgft.setItalic(pSettings->value("TitleFontItalic", false).toBool());
		lgft.setBold(pSettings->value("TitleFontBold", false).toBool());
		SetTitleFont(lgft);

		lgft = QFont(pSettings->value("LabelFontName","Comic Sans MS").toString());
		size = pSettings->value("LabelFontSize",8).toInt();
		if(size>0) lgft.setPointSize(size);
		lgft.setItalic(pSettings->value("LabelFontItalic", false).toBool());
		lgft.setBold(pSettings->value("LabelFontBold", false).toBool());
		SetLabelFont(lgft);

		clr  = pSettings->value("XMajGridColor", QColor(90,90,90)).value<QColor>();
		bs = pSettings->value("XMajGridShow",true).toBool();
		s  = pSettings->value("XMajGridStyle",1).toInt();
		w  = pSettings->value("XMajGridWidth",1).toInt();
		SetXMajGrid(bs,clr,s,w);

		clr  = pSettings->value("YMajGridColor", QColor(90,90,90)).value<QColor>();
		bs = pSettings->value("YMajGridShow",true).toBool();
		s  = pSettings->value("YMajGridStyle",1).toInt();
		w  = pSettings->value("YMajGridWidth",1).toInt();
		SetYMajGrid(bs,clr,s,w);

		clr  = pSettings->value("XMinGridColor", QColor(90,90,90)).value<QColor>();
		ba = pSettings->value("XMinGridAuto",true).toBool();
		bs = pSettings->value("XMinGridShow",false).toBool();
		s  = pSettings->value("XMinGridStyle",2).toInt();
		w  = pSettings->value("XMinGridWidth",1).toInt();
		f  = pSettings->value("XMinGridUnit", 0.01).toDouble();
		SetXMinGrid(bs,ba,clr,s,w,f);

		clr  = pSettings->value("YMinGridColor", QColor(90,90,90)).value<QColor>();
		ba = pSettings->value("YMinGridAuto",true).toBool();
		bs = pSettings->value("YMinGridShow",false).toBool();
		s  = pSettings->value("YMinGridStyle",2).toInt();
		w  = pSettings->value("YMinGridWidth",1).toInt();
		f  = pSettings->value("YMinGridUnit",0.01).toDouble();
		SetYMinGrid(bs,ba,clr,s,w,f);

		clr  = pSettings->value("BorderColor", QColor(200,200,200)).value<QColor>();
		s  = pSettings->value("BorderStyle",0).toInt();
		w  = pSettings->value("BorderWidth",2).toInt();
		m_bBorder = pSettings->value("BorderShow", true).toBool();
		SetBorderColor(clr);
		SetBorderStyle(s);
		SetBorderWidth(w);

		clr  = pSettings->value("BackgroundColor", QColor(15,19,20)).value<QColor>();
		SetBkColor(clr);

		m_iMargin = pSettings->value("margin", 61).toInt();

		m_bYInverted = pSettings->value("Inverted", false).toBool();

		m_X  = pSettings->value("XVariable",0).toInt();
		m_Y  = pSettings->value("YVariable",0).toInt();
	}
	pSettings->endGroup();
}


void QGraph::SetLabelFont(QFont &font)
{
	m_LabelFont = font;
}


void QGraph::SetTitleFont(QFont &font)
{
	m_TitleFont = font;
}


void QGraph::CopySettings(QGraph *pGraph, bool bScales)
{
	Graph::CopySettings(pGraph, bScales);
	m_LabelFont     = pGraph->m_LabelFont;
	m_TitleFont     = pGraph->m_TitleFont;
}



void QGraph::GetTitleFont(QFont &titleFont)
{
	titleFont = m_TitleFont;
}
void QGraph::GetLabelFont(QFont &labelFont)
{
	labelFont = m_LabelFont;
}









