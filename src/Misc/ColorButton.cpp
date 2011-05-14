/****************************************************************************

	ColorButton Class
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

#include "ColorButton.h"
#include <QPainter>

ColorButton::ColorButton()
{
	m_Color = Qt::darkGray;
}

void ColorButton::SetColor(QColor const & color)
{
	m_Color = color;
	update();
}

QColor &ColorButton::GetColor()
{
	return m_Color;
}

void ColorButton::paintEvent ( QPaintEvent * event )
{
	QColor paintcolor;

	if(isEnabled()) paintcolor = m_Color;
	else
	{
		if(isDown()) paintcolor = m_Color.lighter(150);
		else         paintcolor = Qt::lightGray;
	}


	QPainter painter(this);
	painter.setBackgroundMode(Qt::TransparentMode);
	QRect r = rect();

	QPen GreyPen(QColor(70,70,70), 1, Qt::DotLine);
	QBrush colorbrush(paintcolor);
	painter.setBrush(colorbrush);
	r.adjust(3,3,-3,-3);
	painter.setPen(Qt::NoPen);
//	painter.drawRoundedRect(r, 10, 50, Qt::RelativeSize);
	painter.drawRect(r);
	painter.setPen(GreyPen);
	painter.setBrush(Qt::NoBrush);
	r.adjust(-1,-1,0,0);
//	painter.drawRoundedRect(r, 10, 50, Qt::RelativeSize);
	painter.drawRect(r);

}














