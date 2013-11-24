#include "DoubleEdit.h"
#include <QLocale>
#include <QtDebug>



DoubleEdit::DoubleEdit(QWidget *pParent)
{
    setParent(pParent);
    m_Value = 0.0;
    m_pDV = new QDoubleValidator(this);
    m_pDV->setNotation(QDoubleValidator::StandardNotation);
    m_pDV->setDecimals(2);
    setValidator(m_pDV);
    setAlignment(Qt::AlignRight);
}

DoubleEdit::DoubleEdit(double val, int decimals)
{
    m_Value = val;
    m_pDV = new QDoubleValidator(this);
    m_pDV->setDecimals(decimals);
    m_pDV->setNotation(QDoubleValidator::StandardNotation);
    setValidator(m_pDV);
    setAlignment(Qt::AlignRight);
}

DoubleEdit::DoubleEdit(int decimals)
{
    m_Value = 0.0;
    m_pDV = new QDoubleValidator(this);
    m_pDV->setDecimals(decimals);
    m_pDV->setNotation(QDoubleValidator::StandardNotation);
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

