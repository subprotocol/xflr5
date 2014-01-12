/****************************************************************************

	GraphWidget Class
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

#include "../misc/Settings.h"
#include "GraphWidget.h"
#include <QPen>
#include <QPainterPath>
#include <QPainter>
#include <QPaintEvent>

GraphWidget::GraphWidget()
{
	m_TitlePosition.setX(0);
	m_TitlePosition.setY(0);
	m_bDrawLegend = false;
	m_GraphTitle = "";
}




void GraphWidget::SetTitle(QString &Title, QPoint &Place)
{
	m_GraphTitle = Title;
	m_TitlePosition = Place;
}

void GraphWidget::paintEvent(QPaintEvent *  event )
{
	QPainter painter(this);
	painter.save();

	QBrush BackBrush(m_Graph.m_BkColor);
	painter.fillRect(event->rect(), BackBrush);

	painter.setBackgroundMode(Qt::OpaqueMode);
	painter.setBackground(BackBrush);

	QPen mypen;
	mypen.setColor(QColor(255,0,0));
	mypen.setWidth(1);
	mypen.setStyle(Qt::SolidLine);


	painter.setPen(mypen);
	painter.drawLine(39,78,200,302);
	m_Graph.DrawGraph(painter);

	QPen BorderPen;
	BorderPen.setColor(m_Graph.m_BorderColor);
	BorderPen.setWidth(3);
	painter.setPen(BorderPen);
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(QRect(0, 0, width() - 1, height() - 1));

	painter.drawText(m_TitlePosition, m_GraphTitle);

	if(m_bDrawLegend) m_Graph.DrawLegend(painter, m_LegendOrigin, Settings::s_TextFont, Settings::s_TextColor);

	painter.restore();
}



void GraphWidget::resizeEvent ( QResizeEvent * event )
{
	QRect r = rect();
	m_Graph.SetDrawRect(r);

	m_LegendOrigin = QPoint(r.center().x(), r.top()+20);

	event->accept();
}



void GraphWidget::showLegend(bool bShow)
{
	m_bDrawLegend = bShow;
}



void GraphWidget::setLegendPosition(QPoint pos)
{
	m_LegendOrigin = pos;
}










