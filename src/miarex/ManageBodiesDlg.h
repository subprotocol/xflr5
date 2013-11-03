/****************************************************************************

	ManageBodiesDlg  Classes
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


#ifndef MANAGEBODIESDLG_H
#define MANAGEBODIESDLG_H


#include "../objects/Body.h"

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>

class ManageBodiesDlg : public QDialog
{
	Q_OBJECT

	friend class Body;
	friend class MainFrame;
	friend class QMiarex;

public:
    ManageBodiesDlg(QWidget *pParent);
	void InitDialog();

private slots:
	void OnNameList(QListWidgetItem *pItem);
	void OnRename();
	void OnDelete();
	void OnDuplicate();
	void OnEdit();
	void OnNew();
	void OnDescriptionChanged();
	void OnExportDefinition();
	void OnExportGeometry();
	void OnDoubleClickTable(const QModelIndex &index);

private:
	void SetBody(Body *pBody = NULL);
	void SetupLayout();
	void UpdateBodyList();
	bool IsInUse(Body *pBody);

	static void *s_pMainFrame;
	static void *s_pMiarex;

	void *m_pGL3dBodyDlg;

	QList <void*> *m_poaBody;
	QList <void*> *m_poaPlane;

	Body *m_pBody;
	QPushButton *m_pctrlNew, *m_pctrlEdit, *m_pctrlRename, *m_pctrlDelete, *m_pctrlDuplicate;
	QPushButton *m_pctrlExportDef, *m_pctrlExportGeom;
	QListWidget *m_pctrlNameList;
	QTextEdit *m_pctrlDescription;
};



#endif // MANAGEBODIESDLG_H



