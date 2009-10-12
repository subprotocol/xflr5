/****************************************************************************

	InertiaDlg Class
	Copyright (C) 2009 Andre Deperrois XFLR5@yahoo.com

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
#include <QRadioButton>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include "../Params.h"
#include "../Misc/FloatEditDelegate.h"
#include "../Misc/FloatEdit.h"
#include "../Objects/CVector.h"
#include "../Objects/Wing.h"
//#include "../Objects/Body.h"

class InertiaDlg : public QDialog
{
	Q_OBJECT
	friend class GL3dWingDlg;
	friend class GL3dBodyDlg;

public:
    InertiaDlg();
	void InitDialog();

private slots:
	void OnOK();
	void OnInputType();
	void OnCellChanged(QWidget *pWidget=NULL);

private:
	void FillMassModel();
	void ComputeInertia();
	void ComputeBodyInertia(double &Ixx, double &Iyy, double &Izz, double &Ixz);
	void SetupLayout();
	void ReadData();
	void keyPressEvent(QKeyEvent *event);

	CBody *m_pBody;
	CWing *m_pWing;
	void *m_pMainFrame;

	QRadioButton *m_pctrlAuto, *m_pctrlMan;
	QPushButton *OKButton;
	QLabel *m_pctrlMassUnit, *m_pctrlLengthUnit;
	QLabel *m_pctrlObjectMassLabel;
	QLabel *m_pctrlInertiaUnit1, *m_pctrlInertiaUnit2, *m_pctrlInertiaUnit3, *m_pctrlInertiaUnit4;
	QTableView *m_pctrlMassView;
	QStandardItemModel *m_pMassModel;
	FloatEditDelegate *m_pFloatDelegate;
	FloatEdit *m_pctrlIxx, *m_pctrlIyy, *m_pctrlIzz, *m_pctrlIxz;
	FloatEdit *m_pctrlXRef, *m_pctrlYRef, *m_pctrlZRef;

	FloatEdit *m_pctrlObjectMass;

	double m_XRef, m_YRef, m_ZRef;
	double m_Ixx, m_Iyy, m_Izz, m_Ixz;
	double m_MassValue[MAXMASSES];
	CVector m_MassPosition[MAXMASSES];
	int m_NMass;
	int m_iSelection;
};

#endif // INERTIADLG_H
