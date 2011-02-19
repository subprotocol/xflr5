/****************************************************************************

	LineButton Class
	Copyright (C) 2009 Andre Deperrois XFLR5@yahoo.com

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

#include "LineButton.h"
#include <QPainter>
#include <QtDebug>
#include "../Globals.h"

LineButton::LineButton()
{
	m_Color = Qt::darkGray;
	m_Style = 0;
	m_Width = 1;
}

void LineButton::SetColor(QColor const & color)
{
	m_Color = color;
	update();
}

void LineButton::SetStyle(int const & style)
{
	m_Style = style;
	update();
}

void LineButton::SetWidth(int const & width)
{
	m_Width = width;
	update();
}

void LineButton::SetStyle(int const &style, int const &width, QColor const & color)
{
	m_Style = style;
	m_Width = width;
	m_Color = color;
	update();
}

QColor &LineButton::GetColor()
{
	return m_Color;
}

int &LineButton::GetStyle()
{
	return m_Style;
}

int &LineButton::GetWidth()
{
	return m_Width;
}


void LineButton::paintEvent ( QPaintEvent * event )
{
	QColor ContourColor = Qt::gray;

	if(isEnabled())
	{
		if(isDown())
		{
			ContourColor = Qt::darkGray;
		}
		else
		{
			ContourColor = Qt::gray;
		}
	}
	else
	{
		ContourColor = Qt::lightGray;
	}
	QRect r = rect();

	QPainter painter(this);
	painter.setBrush(Qt::NoBrush);
	painter.setBackgroundMode(Qt::TransparentMode);

	QPen LinePen(m_Color);
	LinePen.setStyle(::GetStyle(m_Style));
	LinePen.setWidth(m_Width);
	painter.setPen(LinePen);
	painter.drawLine(r.left()+5, r.height()/2, r.width()-10, r.height()/2);

	QPen ContourPen(ContourColor);
	painter.setPen(ContourPen);
	r.adjust(0,2,-1,-3);
	painter.drawRoundRect(r,5,40);

}














