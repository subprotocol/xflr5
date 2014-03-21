/****************************************************************************

	IntEdit Class
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

#include "IntEdit.h"



IntEdit::IntEdit(QWidget *pParent)
{
	setParent(pParent);
	m_Value = 0.0;
	m_pDV = new QIntValidator(this);
	setValidator(m_pDV);
	setAlignment(Qt::AlignRight);
}


IntEdit::IntEdit(int val, QWidget *pParent)
{
	setParent(pParent);
	m_Value = val;
	m_pDV = new QIntValidator(this);
	setValidator(m_pDV);
	setAlignment(Qt::AlignRight);
	FormatValue();
}


void IntEdit::focusOutEvent (QFocusEvent *event)
{
	ReadValue();
	FormatValue();
	//	emit(editingFinished()); //is emitted by call to base class
	QLineEdit::focusOutEvent(event);
}


int IntEdit::ReadValue()
{
	bool bOK;
	int f = locale().toInt(text().trimmed(), &bOK);
	if(bOK) m_Value = f;
	return m_Value;
}


void IntEdit::SetValue(int val)
{
	m_Value = val;
	FormatValue();
}


void IntEdit::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
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



void IntEdit::FormatValue()
{
	setText(QString("%L1").arg(m_Value));
}



void IntEdit::SetValueNoFormat(int val)
{
	m_Value = val;
}


