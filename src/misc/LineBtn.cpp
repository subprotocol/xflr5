#include <QStyleOption>
#include <QMouseEvent>
#include <QPen>
#include <QPainter>
#include <QDebug>

#include "../mainframe.h"
#include "LineBtn.h"
#include "../globals.h"


LineBtn::LineBtn(QWidget *parent)
	: QAbstractButton(parent)
{
	QFontMetrics fm(MainFrame::s_TextFont);
	setMinimumWidth(17*fm.averageCharWidth());
	setMinimumHeight(fm.height());

	m_Color = Qt::darkGray;
	m_Style = 0;
	m_Width = 1;
}



void LineBtn::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
			emit clicked();
	}
	else
		QWidget::mouseReleaseEvent(event);
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
