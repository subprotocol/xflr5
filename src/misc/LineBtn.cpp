/****************************************************************************

    LineBtn Class
    Copyright (C) 2013 Andre Deperrois adeperrois@xflr5.com

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


#include <QStyleOption>
#include <QMouseEvent>
#include <QPen>
#include <QPainter>

#include "../mainframe.h"
#include "LineBtn.h"
#include "../globals.h"


LineBtn::LineBtn(QWidget *parent)
    : QAbstractButton(parent)
{
	QSizePolicy szPolicyExpanding;
	szPolicyExpanding.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
	szPolicyExpanding.setVerticalPolicy(QSizePolicy::Minimum);
	setSizePolicy(szPolicyExpanding);

	m_Color = Qt::darkGray;
	m_Style = 0;
	m_Width = 1;
}



void LineBtn::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
            emit clickedLB();
	}
	else
		QWidget::mouseReleaseEvent(event);
}


QSize LineBtn::sizeHint() const
{
	QFontMetrics fm(MainFrame::s_TextFont);
	int w = 13 * fm.averageCharWidth();
	int h = fm.height();
	return QSize(w, h);
}


void LineBtn::SetColor(QColor const & color)
{
	m_Color = color;
	update();
}


void LineBtn::SetStyle(int const & style)
{
	m_Style = style;
	update();
}


void LineBtn::SetWidth(int const & width)
{
	m_Width = width;
	update();
}


void LineBtn::SetStyle(int const &style, int const &width, QColor const & color)
{
	m_Style = style;
	m_Width = width;
	m_Color = color;
	update();
}


QColor &LineBtn::GetColor()
{
	return m_Color;
}


int &LineBtn::GetStyle()
{
	return m_Style;
}


int &LineBtn::GetWidth()
{
	return m_Width;
}


void LineBtn::paintEvent(QPaintEvent *event)
{
	QRect r = rect();

    QStyleOption opt;
	opt.init(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

	painter.save();

	painter.setBrush(Qt::DiagCrossPattern);
	painter.setBackgroundMode(Qt::TransparentMode);

	QPen LinePen(m_Color);
	LinePen.setStyle(::GetStyle(m_Style));
	LinePen.setWidth(m_Width);
	painter.setPen(LinePen);
	painter.drawLine(r.left()+5, r.height()/2, r.width()-10, r.height()/2);


/*	QColor ContourColor = Qt::gray;

	if(isEnabled())
	{
		if(isDown())
		{
			ContourColor = Qt::red;
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
	QPen ContourPen(ContourColor);
	painter.setPen(ContourPen);
	r.adjust(0,2,-1,-3);
	painter.drawRoundRect(r,5,40);*/

	painter.restore();
}
