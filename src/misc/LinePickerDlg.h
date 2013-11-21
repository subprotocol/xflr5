/****************************************************************************

	LinePicker Class
	Copyright (C) 2009 Andre Deperrois adeperrois@xflr5.com

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

#ifndef LINEPICKERDLG_H
#define LINEPICKERDLG_H

#include <QDialog>
#include <QComboBox>
#include "LineBtn.h"
#include "LineCbBox.h"
#include "LineDelegate.h"

class LinePickerDlg : public QDialog
{
	Q_OBJECT

public:
    LinePickerDlg(QWidget *pParent);

	void InitDialog();
	void InitDialog(int style, int width, QColor color);

	void keyPressEvent(QKeyEvent *event);

	int &GetWidth();
	int &GetStyle();
	QColor &GetColor();

	void SetColor(QColor color);
	void SetWidth(int width);
	void SetStyle(int style);

	void FillBoxes();
	void SetupLayout();


protected:
	LineBtn *m_pctrlColor;
	LineCbBox *m_pctrlWidth, *m_pctrlStyle;
	QPushButton *OKButton, *CancelButton;

private:
	int m_Style;
	int m_Width;
	QColor m_Color;
	void* m_pParent;
	LineDelegate *m_pStyleDelegate, *m_pWidthDelegate;


private slots:
	void OnWidth(int val);
	void OnStyle(int val);
	void OnColor();

};

#endif // LINEPICKERDLG_H
