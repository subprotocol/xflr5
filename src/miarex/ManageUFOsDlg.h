/****************************************************************************

	ManageUFOsDlg Class
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


#ifndef MANAGEUFOSDLG_H
#define MANAGEUFOSDLG_H

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include "UFOTableDelegate.h"
#include "../objects/Plane.h"



class ManageUFOsDlg : public QDialog
{
	Q_OBJECT
	friend class QMiarex;
	friend class MainFrame;

public:
    ManageUFOsDlg(QWidget *pParent);
	void InitDialog(QString &UFOName);

private slots:
	void OnDelete();
	void OnRename();
	void OnUFOClicked(QModelIndex index);
	void OnDoubleClickTable(const QModelIndex &index);
	void OnDescriptionChanged();

private:
	void resizeEvent(QResizeEvent *event);
	void keyPressEvent(QKeyEvent *event);

	void FillUFOTable();
	void FillWingRow(int row);
	void FillPlaneRow(int row, int n);

	void SetupLayout();
	void SelectUFO();

private:
	QPushButton *CloseButton;
	QPushButton *m_pctrlRename, *m_pctrlDelete;
	QTextEdit *m_pctrlDescription;
	QTableView *m_pctrlUFOTable;
	QStandardItemModel *m_pUFOModel;
	UFOTableDelegate *m_pUFODelegate;

	Wing *m_pWing;
	Plane *m_pPlane;
	static void *s_pMainFrame, *s_pMiarex;
};

#endif // MANAGEUFOSDLG_H
