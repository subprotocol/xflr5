/****************************************************************************

	FloatEdit Class
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


/**
 *@file This file implements the FloatEdit class
 *
*/
#ifndef FLOATEDIT_H
#define FLOATEDIT_H

#include <QLineEdit>
#include <QString>
#include <QKeyEvent>
#include <QDoubleValidator>


/**
 *This class defines a line edit widget for the specific input of floating point numbers.
 *It goes back to the very first MFC versions of this program, at a time when no such default widget was available.
 *Could be advantageously replaced with a QLineEdit + a validator, and be able to read numbers i.a.w. regional options.
 *@todo replace with QLineEdit + a validator.
 */
class FloatEdit : public QLineEdit
{
public:

	FloatEdit(QWidget *pParent=NULL);
	FloatEdit(double d, int precision=2);

	void DefineValue(double f);
	double Value();
	void SetMin(double f);
	void SetMax(double f);
	void SetValue(double f);
	void SetValueNoFormat(double f);
	void SetPrecision(int i);
	
private:
	bool IsInBounds();
	void FormatValue(double const &f, QString &str);

	void focusInEvent ( QFocusEvent * event );
	void focusOutEvent ( QFocusEvent * event );
	void keyPressEvent(QKeyEvent *event);

	double ReadValue();

	QDoubleValidator *v;

	double m_Value;
	int m_iPrecision;
};

#endif // FLOATEDIT_H
