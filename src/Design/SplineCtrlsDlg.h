/****************************************************************************

	SplineCtrlsDlg
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

#ifndef SPLINECTRLSDLG_H
#define SPLINECTRLSDLG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include "../Misc/FloatEdit.h"
#include "../Objects/Sf.h"
#include <QTableView>
#include <QStandardItemModel>
#include <QCheckBox>
#include "../Misc/FloatEditDelegate.h"

class SplineCtrlsDlg : public QDialog
{
	Q_OBJECT
	friend class QAFoil;

public:
    SplineCtrlsDlg();
	void InitDialog();

private slots:
	void OnUpperCellChanged(QWidget *FloatEdit);
	void OnLowerCellChanged(QWidget *FloatEdit);
	void OnOK();
	void OnSymetric();
	void OnSelChangeDegree();
	void OnEditingFinished();

private:
	void SetuLayout();
	void showEvent(QShowEvent *event);
	void FillPointLists();
	void ReadData();

	FloatEdit	*m_pctrlOutExtrados;
	FloatEdit	*m_pctrlOutIntrados;
	QComboBox	*m_pctrlDegExtrados;
	QComboBox	*m_pctrlDegIntrados;
	QPushButton *OKButton, *CancelButton;
	QCheckBox *m_pctrlSymetric;

	QTableView *m_pctrlUpperList, *m_pctrlLowerList;
	QStandardItemModel *m_pUpperListModel,*m_pLowerListModel;
	FloatEditDelegate *m_pUpperFloatDelegate, *m_pLowerFloatDelegate;

protected:
	bool m_bSymetric;
	CSF *m_pSF;
	CSF m_SF; //work splines
};

#endif // SPLINECTRLSDLG_H
