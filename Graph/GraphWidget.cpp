/****************************************************************************

	GraphWidget Class
        Copyright (C) 2008 Andre Deperrois XFLR5@yahoo.com

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

#include "GraphWidget.h"
#include <QPen>
#include <QPainterPath>
#include <QPainter>
#include <QPaintEvent>

GraphWidget::GraphWidget()
{
}


void GraphWidget::paintEvent(QPaintEvent *  event )
{
	if(!m_pGraph) return;
	QPainter painter(this);
	painter.save();

	QBrush Bck(m_pGraph->m_BkColor);
	painter.fillRect(event->rect(), Bck);

	painter.setBackgroundMode(Qt::OpaqueMode);
	painter.setBackground(Bck);
	QRect rect(10, 20, 80, 60);

	QPen mypen;
	mypen.setColor(QColor(255,0,0));
	mypen.setWidth(1);
	mypen.setStyle(Qt::SolidLine);


	painter.setPen(mypen);
	painter.drawLine(39,78,200,302);
	m_pGraph->DrawGraph(painter);

	QPen BorderPen;
	BorderPen.setColor(m_pGraph->m_BorderColor);
	BorderPen.setWidth(3);
	painter.setPen(BorderPen);
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(QRect(0, 0, width() - 1, height() - 1));

	painter.restore();
}



void GraphWidget::resizeEvent ( QResizeEvent * event )
{
	QRect r = rect();
	if(m_pGraph) m_pGraph->SetDrawRect(r);

}














