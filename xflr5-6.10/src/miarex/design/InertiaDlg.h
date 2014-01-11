/****************************************************************************

	InertiaDlg Class
	Copyright (C) 2009-2013 Andre Deperrois adeperrois@xflr5.com

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

#ifndef INERTIADLG_H
#define INERTIADLG_H

#include <QDialog>
#include <QLabel>
#include <QStackedWidget>
#include <QRadioButton>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>

#include "../../params.h"
#include "../../misc/FloatEditDelegate.h"
#include "../../misc/DoubleEdit.h"
#include "../../objects/CVector.h"
#include "../../objects/Plane.h"

class InertiaDlg : public QDialog
{
	Q_OBJECT
	friend class GL3dWingDlg;
	friend class GL3dBodyDlg;
	friend class PlaneDlg;
	friend class QMiarex;
	friend class MainFrame;

public:
    InertiaDlg(QWidget *pParent);
	~InertiaDlg();

	void InitDialog();

private slots:
	void OnOK();
	void OnBodyInertia();
	void OnWingInertia();
	void OnWing2Inertia();
	void OnStabInertia();
	void OnFinInertia();
	void OnCellChanged(QWidget *);
	void OnExportToAVL();
	void OnInsertMassRow();
	void OnDeleteMassRow();
	void OnVolumeMass();

private:
	void contextMenuEvent(QContextMenuEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent *event);
	void showEvent(QShowEvent *event);
	void hideEvent(QHideEvent *event);

	void FillMassModel();
	void ClearPointMasses();
	void ComputeInertia();
	void ComputeBodyAxisInertia();
	void SetupLayout();
	void ReadData();

	//layout widget variables
	QStackedWidget *m_pctrlTopStack;
	QPushButton *m_pctrlWingInertia, *m_pctrlWing2Inertia, *m_pctrlStabInertia, *m_pctrlFinInertia, *m_pctrlBodyInertia;
	QPushButton *OKButton;
	QLabel *m_pctrlMassUnit, *m_pctrlMassUnit2, *m_pctrlLengthUnit, *m_pctrlLengthUnit2;
	QLabel *m_pctrlVolumeMassLabel, *m_pctrlTotalMassLabel;
	QLabel *m_pctrlInertiaUnit1, *m_pctrlInertiaUnit2;
	QTableView *m_pctrlMassTable;
	QStandardItemModel *m_pMassModel;
	FloatEditDelegate *m_pFloatDelegate;
	DoubleEdit *m_pctrlCoGIxx, *m_pctrlCoGIyy, *m_pctrlCoGIzz, *m_pctrlCoGIxz;
	DoubleEdit *m_pctrlXCoG,*m_pctrlYCoG,*m_pctrlZCoG;
	DoubleEdit *m_pctrlVolumeMass;

	DoubleEdit *m_pctrlTotalIxx, *m_pctrlTotalIyy, *m_pctrlTotalIzz, *m_pctrlTotalIxz;
	DoubleEdit *m_pctrlXTotalCoG,*m_pctrlYTotalCoG,*m_pctrlZTotalCoG;
	DoubleEdit *m_pctrlTotalMass;

	QMenu *m_pContextMenu;
	QAction *m_pInsertMassRow, *m_pDeleteMassRow;

	int  *m_precision;

	//member variables
	Body *m_pBody;
	Wing *m_pWing;
	Plane *m_pPlane;

	double m_VolumeMass;
	CVector m_VolumeCoG;

	double m_CoGIxx, m_CoGIyy, m_CoGIzz, m_CoGIxz;

	QList<PointMass*> m_PointMass;

	bool m_bChanged;

	static QSize  s_WindowSize;
	static bool s_bWindowMaximized;

public:
	static QPoint s_Position;
};

#endif // INERTIADLG_H
