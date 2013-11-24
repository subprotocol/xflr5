/****************************************************************************

	AboutQ5 Class
	Copyright (C) 2008-2008 Andre Deperrois adeperrois@xflr5.com

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

#include "FloatEdit.h"
#include <math.h>


FloatEdit::FloatEdit(QWidget *pParent)
{
	setParent(pParent);
	m_Value = 0.0;

	v = new QDoubleValidator(this);
	v->setRange(-1.e10, 1.e10, 1000);
    v->setDecimals(2);
	setValidator(v);
	setAlignment(Qt::AlignRight);
}



FloatEdit::FloatEdit(double d, int precision)
{
	m_Value = d;

	v = new QDoubleValidator(this);
	v->setRange(-1.e10, 1.e10, 1000);
    v->setDecimals(3);
	setValidator(v);
	setAlignment(Qt::AlignRight);
}



void FloatEdit::focusOutEvent (QFocusEvent *event)
{
	QString str;
    m_Value = ReadValue();
    FormatValue(m_Value);
    emit(editingFinished());

    QLineEdit::focusOutEvent(event);
}



double FloatEdit::ReadValue()
{
	bool bOK;
    double f = locale().toDouble(text(),&bOK);

	if(bOK) return f;
	else    return m_Value;
}

void FloatEdit::SetPrecision(int i){v->setDecimals(i);}

void FloatEdit::SetMin(double min){v->setBottom(min); }
void FloatEdit::SetMax(double max){v->setTop(max); }
double FloatEdit::Value(){ return m_Value;}

void FloatEdit::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Return:
		{
            m_Value = ReadValue();
            FormatValue(m_Value);
			QLineEdit::keyPressEvent(event);

			break;
		}
		case Qt::Key_Escape:
		{
            FormatValue(m_Value);
			QLineEdit::keyPressEvent(event);
			break;
		}
		default:
		{
			QLineEdit::keyPressEvent(event);
            m_Value = ReadValue();
			break;
		}
	}
}



void FloatEdit::SetValue(double f)
{
	QString str;
	m_Value = f;
    FormatValue(m_Value);
}



void FloatEdit::FormatValue(double const &f)
{
    QString str=QString("%L1").arg(f,0,'f', v->decimals());
    setText(str);
}




