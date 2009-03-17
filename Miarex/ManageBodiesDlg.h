/****************************************************************************

	ManageBodiesDlg  Classes
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


#ifndef MANAGEBODIESDLG_H
#define MANAGEBODIESDLG_H


#include "../Objects/Body.h"

#include <QDialog>
#include <QListWidget>
#include <QPushButton>


class ManageBodiesDlg : public QDialog
{
	Q_OBJECT

	friend class CBody;
	friend class QMiarex;

public:
    ManageBodiesDlg();
	void InitDialog();

private slots:
	void OnNameList(QListWidgetItem *pItem);
	void OnRename();
	void OnDelete();
	void OnDuplicate();
	void OnEdit();
	void OnNew();

private:
	void SetBody(CBody *pBody = NULL);
	void SetupLayout();
	void UpdateBodyList();

	void *m_pMainFrame;
	void *m_pMiarex;
	void *m_pGL3dBodyDlg;

	QList <void*> *m_poaBody;
	QList <void*> *m_poaPlane;

	CBody *m_pBody;
	QPushButton *m_pctrlNew, *m_pctrlEdit, *m_pctrlRename, *m_pctrlDelete, *m_pctrlDuplicate;
	QListWidget *m_pctrlNameList;

};



#endif // MANAGEBODIESDLG_H



