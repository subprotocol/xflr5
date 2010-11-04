/****************************************************************************

	FloatEdit Class
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

#ifndef FLOATEDIT_H
#define FLOATEDIT_H

#include <QLineEdit>
#include <QString>
#include <QKeyEvent>
#include <QDoubleValidator>

class FloatEdit : public QLineEdit
{
public:

	FloatEdit(QWidget *pParent=NULL);
	FloatEdit(double d, int precision=2);

	void DefineValue(double f);
	double GetValue();
	bool GetValue(double &f);
	void SetMin(double f);
	void SetMax(double f);
	void SetValue(double f);
	void SetValueNoFormat(double f);
	void SetPrecision(int i);
	void SetFocusSelect(bool bFocusSelect);
	
private:
	bool IsInBounds(double f);
	void FormatValue(double const &f, QString &str);
	void focusOutEvent ( QFocusEvent * event );
	void keyPressEvent(QKeyEvent *event);
	void showEvent ( QShowEvent * event );
	double ReadValue();

	QDoubleValidator *v;

	bool m_bFocusSelect;
	double m_Value;
	int m_iPrecision;
};

#endif // FLOATEDIT_H
