/****************************************************************************

	RenameDlg Classes
        Copyright (C) 2003-2008 Andre Deperrois adeperrois@xflr5.com

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


#ifndef RENAMEDLG_H
#define RENAMEDLG_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QListWidget>

class RenameDlg : public QDialog
{
	Q_OBJECT

	friend class MainFrame;
	friend class QXDirect;
	friend class QAFoil;
	friend class QMiarex;
	friend class StabViewDlg;

private:
	void InitDialog();

private slots:
	void OnOverwrite();
	void OnOK();
	void OnSelChangeList(int iItem);
	void OnDoubleClickList(QListWidgetItem * pItem);

public:
	RenameDlg(QWidget *pParent=NULL);
	void keyPressEvent(QKeyEvent *event);
	void SetupLayout();

	QLabel      *m_pctrlMessage;
	QLineEdit   *m_pctrlName;
	QListWidget *m_pctrlNameList;
	QPushButton *OverwriteButton;
	QPushButton *OKButton;
	QPushButton *CancelButton;
	QLabel *m_pctrlNote;

	QStringList *m_pstrArray;
	QString m_StartName;
	QString m_strName;
	QString m_strQuestion;
	QString m_Note;
	bool m_bEnableOverwrite;
	bool m_bExists;

};

#endif // RENAMEDLG_H
