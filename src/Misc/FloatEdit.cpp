/****************************************************************************

	AboutQ5 Class
	Copyright (C) 2008-2008 Andre Deperrois xflr5@yahoo.com

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
#include <QtDebug>

FloatEdit::FloatEdit(QWidget *pParent)
{
	setParent(pParent);
	m_Value = 0.0;
	m_iPrecision = 2;
	v = new QDoubleValidator(this);
	v->setRange(-1.e10, 1.e10, 1000);
	setValidator(v);
	setAlignment(Qt::AlignRight);
}



FloatEdit::FloatEdit(double d, int precision)
{
	m_Value = d;

	m_iPrecision = precision;

	v = new QDoubleValidator(this);
	v->setRange(-1.e10, 1.e10, 1000);
	setValidator(v);
	setAlignment(Qt::AlignRight);
}


bool FloatEdit::IsInBounds()
{
	int pos = 0;
	QString strange = text();
	strange.replace(" ", "");

	if (v->validate(strange, pos)==QValidator::Acceptable) return true;
	else                                                   return false;
}



void FloatEdit::focusOutEvent ( QFocusEvent * event )
{
	QString str;
	double f = ReadValue();

	if(IsInBounds())
	{
		m_Value = f;
		FormatValue(m_Value, str);
		setText(str);
		emit(editingFinished());
	}
	else
	{
		FormatValue(m_Value, str);
		setText(str);
	}
	QLineEdit::focusOutEvent(event);
}


void FloatEdit::focusInEvent(QFocusEvent * event)
{
/*	QString str;
	str = QString("%1").arg(m_Value,'g');
	str = str.trimmed();
	int ind = str.indexOf('.');
	if(ind<0 || str.right(str.length()-ind-1).length() < 2)
	{
		FormatValue(m_Value, str);
	}
	setText(str);*/

	QLineEdit::focusInEvent(event);
}


double FloatEdit::ReadValue()
{
	bool bOK;
	QString strange = text();
	strange.replace(" ", "");
	double f = strange.toDouble(&bOK);

	if(bOK) return f;
	else    return m_Value;
}


double FloatEdit::Value()
{
	return m_Value;
}



void FloatEdit::keyPressEvent(QKeyEvent *event)
{
	QString str;
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			double f = ReadValue();
			if(IsInBounds()) m_Value = f;

			FormatValue(m_Value, str);
			setText(str);

			QLineEdit::keyPressEvent(event);

			break;
		}
		case Qt::Key_Escape:
		{
			FormatValue(m_Value, str);
			setText(str);
			QLineEdit::keyPressEvent(event);
			break;
		}
		default:
		{
			QLineEdit::keyPressEvent(event);
			double f = ReadValue();
			if(IsInBounds()) m_Value = f;
			break;
		}
	}
}


void FloatEdit::SetPrecision(int i)
{
	m_iPrecision = i;
}


void FloatEdit::SetMin(double f)
{
	v->setBottom(f);
}


void FloatEdit::SetMax(double f)
{
	v->setTop(f);
}

void FloatEdit::DefineValue(double f)
{
	m_Value = f;
}

void FloatEdit::SetValue(double f)
{
	QString str;
	m_Value = f;
	FormatValue(m_Value,str);
	setText(str);
}

void FloatEdit::SetValueNoFormat(double f)
{
	m_Value = f;
}

void FloatEdit::FormatValue(double const &f, QString &str)
{
	if ((f==0.0 || fabs(f)>=pow(10.0, -m_iPrecision)) && f <10000000.0)
	{
		str=QString("%1").arg(f,0,'f', m_iPrecision);
	}
	else
	{
		str=QString("%1").arg(f,0,'g',m_iPrecision+1);
	}
}




