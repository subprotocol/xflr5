/****************************************************************************

	GraphVariableDlg  Classes
	Copyright (C) 2009 Andre Deperrois xflr5@yahoo.com

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

#ifndef GRAPHVARIABLEDLG_H
#define GRAPHVARIABLEDLG_H

#include <QListWidget>
#include <QDialog>

class GraphVariableDlg : public QDialog
{
	Q_OBJECT
public:
	GraphVariableDlg();
	void InitDialog(int iGraphType=1);
	void GetSelection(int &XSel, int &YSel);
	void SetSelection(int const &XSel, int const &YSel);

private slots:
	void OnOK();

private:
	void SetupLayout();
	void FillLists(int iGraphType);

	QListWidget *m_pctrlXSel, *m_pctrlYSel;
	QPushButton *OKButton, *CancelButton;
public:
	int m_XSel, m_YSel;

};

#endif // GRAPHVARIABLEDLG_H
