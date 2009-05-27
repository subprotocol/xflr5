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
#include <QDoubleValidator>
#include <math.h>
#include <QtDebug>


FloatEdit::FloatEdit(QWidget *pParent)
{
	setParent(pParent);
	m_Value = 0.0;

	m_fMin = -1.e10;
	m_fMax =  1.e10;
	m_iPrecision = 2;
	QDoubleValidator *v = new QDoubleValidator(this);
	setValidator(v);
	setAlignment(Qt::AlignRight);
}



FloatEdit::FloatEdit(double d, int precision)
{
	bool bOK;
	m_Value = d;

	m_fMin = -1.e10;
	m_fMax =  1.e10;
	m_iPrecision = precision;

	QDoubleValidator *v = new QDoubleValidator(this);
	setValidator(v);
	setAlignment(Qt::AlignRight);
}


bool FloatEdit::IsInBounds(double f)
{
    if ( m_Value<m_fMin || m_Value> m_fMax)
    {
	    return false;
    }
    return true;
}



void FloatEdit::focusInEvent ( QFocusEvent * event )
{
	selectAll();
}


void FloatEdit::focusOutEvent ( QFocusEvent * event )
{
	QString str;
	double f = ReadValue();
	if(IsInBounds(f))
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


double FloatEdit::GetValue()
{
	return m_Value;
}


bool FloatEdit::GetValue(double &f)
{
	f = m_Value;
    return true;
}


void FloatEdit::keyPressEvent(QKeyEvent *event)
{
	QString str;
	switch (event->key())
    {
		case Qt::Key_Return:
		{
			double f = ReadValue();
			if(IsInBounds(f))
			{
				m_Value = f;
			}
			FormatValue(m_Value, str);
			setText(str);
//			emit(editingFinished());
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
			if(IsInBounds(f)) m_Value = f;
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
    m_fMin = f;
}

void FloatEdit::SetMax(double f)
{
    m_fMax = f;
}


void FloatEdit::SetValue(double f)
{
	QString str;
	m_Value = f;
	FormatValue(m_Value,str);
	setText(str);
}


void FloatEdit::FormatValue(double const &f, QString &str)
{
	QString str1;
	int q, r, exp, i, pos, l;

	if ((f==0.0 || fabs(f)>=pow(10.0, -m_iPrecision)) && f <1000.0)
	{
		switch (m_iPrecision)
		{
			//there probably is a more elegant way to do this,
			case 0:
				str=QString("%1").arg(f);
				break;
			case 1:
				str=QString("%1").arg(f,0,'f',1);
				break;
			case 2:
				str=QString("%1").arg(f,0,'f',2);
				break;
			case 3:
				str=QString("%1").arg(f,0,'f',3);
				break;
			case 4:
				str=QString("%1").arg(f,0,'f',4);
				break;
			case 5:
				str=QString("%1").arg(f,0,'f',5);
				break;
			case 6:
				str=QString("%1").arg(f,0,'f',6);
				break;
			case 7:
				str=QString("%1").arg(f,0,'f',7);
				break;
			case 8:
				str=QString("%1").arg(f,0,'f',8);
				break;
			case 9:
				str=QString("%1").arg(f,0,'f',9);
				break;
			default:
				str=QString("%1").arg(f,0,'f',2);
		}
	}
	else if(f>=1000.0)
	{
		exp = (int)log10(f);
		r = exp%3;
		q = (exp-r)/3;

		QString strong;

		switch (m_iPrecision)
		{
			//there is probably is a more elegant way to do this,
			case 0:
				strong=QString("%1").arg(f,0,'f',0);
				break;
			case 1:
				strong=QString("%1").arg(f,0,'f',1);
				break;
			case 2:
				strong=QString("%1").arg(f,0,'f',2);
				break;
			case 3:
				strong=QString("%1").arg(f,0,'f',3);
				break;
			case 4:
				strong=QString("%1").arg(f,0,'f',4);
				break;
			case 5:
				strong=QString("%1").arg(f,0,'f',5);
				break;
			case 6:
				strong=QString("%1").arg(f,0,'f',6);
				break;
			case 7:
				strong=QString("%1").arg(f,0,'f',7);
				break;
			case 8:
				strong=QString("%1").arg(f,0,'f',8);
				break;
			case 9:
				strong=QString("%1").arg(f,0,'f',9);
				break;
			default:
				strong=QString("%1").arg(f,0,'f',2);
		}

		l = strong.length();
		pos = strong.indexOf(".");
		if(pos<0) pos = l;
		pos = l-pos;

		for (i=0; i<q; i++)
		{
			strong.insert(l-3*(i+1)-i-pos," ");
			l++;
		}

		str = strong;
	}
	else
	{
		exp  = (int)log10(f)-1;
		str1 = QString("e%1").arg( exp);

		double main = (float)(f/pow(10.0, exp)*1.000001);
		switch (m_iPrecision)
		{
			case 0:
				str=QString("%1").arg(main,0,'f',0);
				break;
			case 1:
				str=QString("%1").arg(main,0,'f',1);
				break;
			case 2:
				str=QString("%1").arg(main,0,'f',2);
				break;
			case 3:
				str=QString("%1").arg(main,0,'f',3);
				break;
			case 4:
				str=QString("%1").arg(main,0,'f',4);
				break;
			case 5:
				str=QString("%1").arg(main,0,'f',5);
				break;
			default:
				str=QString("%1").arg(main,0,'f',2);
		}
		str += str1;
	}
}


void FloatEdit::showEvent ( QShowEvent * event )
{
	QString str;
	FormatValue(m_Value, str);
	setText(str);
}


