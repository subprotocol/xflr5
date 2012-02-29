/****************************************************************************

	LineCbBox Class
	Copyright (C) 2009 Andre Deperrois adeperrois@xflr5.com

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

#include "../Globals.h"
#include "LineCbBox.h"
#include <QPainter>
#include <QPaintEvent>

LineCbBox::LineCbBox(void *pParent)
{
	m_Style = 0;
	m_Width = 1;
	m_Color = QColor(255,100,50);
}

LineCbBox::LineCbBox(int const &style, int const &width, QColor const &color)
{
	m_Style = style;
	m_Width = width;
	m_Color = color;
}

void LineCbBox::SetLine(int const &style, int const &width, QColor const &color)
{
	m_Style = style;
	m_Width = width;
	m_Color = color;
}

void LineCbBox::paintEvent (QPaintEvent *event)
{

	QPainter painter(this);
	painter.save();

	QColor ContourColor = Qt::gray;

	if(isEnabled()) ContourColor = Qt::gray;
	else            ContourColor = Qt::lightGray;

	QRect r = event->rect();

	painter.setBrush(Qt::NoBrush);
	painter.setBackgroundMode(Qt::TransparentMode);

	QPen LinePen(m_Color);
	LinePen.setStyle(GetStyle(m_Style));
	LinePen.setWidth(m_Width);
	painter.setPen(LinePen);
	painter.drawLine(r.left()+5, r.height()/2, r.width()-10, r.height()/2);

	QPen ContourPen(ContourColor);
	painter.setPen(ContourPen);
	r.adjust(0,2,-1,-3);
	painter.drawRoundRect(r,5,40);

	painter.restore();
}

