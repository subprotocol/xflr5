/****************************************************************************

    DoubleEdit Class
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

#include "DoubleEdit.h"



DoubleEdit::DoubleEdit(QWidget *pParent)
{
	setParent(pParent);
	m_Value = 0.0;
	m_pDV = new QDoubleValidator(this);
//	m_pDV->setNotation(QDoubleValidator::StandardNotation);
	m_pDV->setRange(-1.e10, 1.e10);
	m_pDV->setDecimals(2);
	setValidator(m_pDV);
	setAlignment(Qt::AlignRight);
}


DoubleEdit::DoubleEdit(double val, int decimals)
{
	m_Value = val;
	m_pDV = new QDoubleValidator(this);
//	m_pDV->setNotation(QDoubleValidator::StandardNotation);
	m_pDV->setRange(-1.e10, 1.e10);
	m_pDV->setDecimals(decimals);
	setValidator(m_pDV);
	setAlignment(Qt::AlignRight);
}




void DoubleEdit::focusOutEvent (QFocusEvent *event)
{
	ReadValue();
	FormatValue();
	emit(editingFinished());
	QLineEdit::focusOutEvent(event);
}


double DoubleEdit::ReadValue()
{
	bool bOK;
	double f = locale().toDouble(text().trimmed(), &bOK);
	if(bOK) m_Value = f;
	return m_Value;
//	DoubleEdit orig
//	if(bOK) return f;
//	else    return m_Value;
}


void DoubleEdit::SetValue(double val)
{
	m_Value = val;
	FormatValue();
}


void DoubleEdit::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			ReadValue();
			FormatValue();
			QLineEdit::keyPressEvent(event);

			break;
		}
		case Qt::Key_Escape:
		{
			FormatValue();
			QLineEdit::keyPressEvent(event);
			break;
		}
		default:
		{
			QLineEdit::keyPressEvent(event);
			ReadValue();
			break;
		}
	}
}



void DoubleEdit::FormatValue()
{
    setText(QString("%L1").arg(m_Value,0,'f', m_pDV->decimals()));
}



void DoubleEdit::SetValueNoFormat(double val)
{
	m_Value = val;
}


