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
#include "../Objects/Plane.h"
//#include "../Objects/Body.h"

class InertiaDlg : public QDialog
{
	Q_OBJECT
	friend class GL3dWingDlg;
	friend class GL3dBodyDlg;
	friend class PlaneDlg;
	friend class QMiarex;

public:
    InertiaDlg();
	void InitDialog();

private slots:
	void OnOK();
	void OnCellChanged(QWidget *pWidget=NULL);
	void OnExportToAVL();

private:
	void resizeEvent(QResizeEvent *event);
	void showEvent(QShowEvent *event);
	void FillMassModel();
	void ComputeInertia();
	void ComputeBodyInertia(double &Ixx, double &Iyy, double &Izz, double &Ixz);
	void SetupLayout();
	void ReadData();
	void keyPressEvent(QKeyEvent *event);

	CBody *m_pBody;
	CWing *m_pWing;
	CPlane *m_pPlane;
	void *m_pMainFrame;

	QPushButton *OKButton;
	QLabel *m_pctrlMassUnit, *m_pctrlMassUnit2, *m_pctrlLengthUnit, *m_pctrlLengthUnit2;
	QLabel *m_pctrlVolumeMassLabel, *m_pctrlTotalMassLabel;
	QLabel *m_pctrlInertiaUnit1, *m_pctrlInertiaUnit2;
	QTableView *m_pctrlMassView;
	QStandardItemModel *m_pMassModel;
	FloatEditDelegate *m_pFloatDelegate;
	FloatEdit *m_pctrlCoGIxx, *m_pctrlCoGIyy, *m_pctrlCoGIzz, *m_pctrlCoGIxz;
	FloatEdit *m_pctrlXCoG,*m_pctrlYCoG,*m_pctrlZCoG;
	FloatEdit *m_pctrlVolumeMass;
	
	FloatEdit *m_pctrlTotalIxx, *m_pctrlTotalIyy, *m_pctrlTotalIzz, *m_pctrlTotalIxz;
	FloatEdit *m_pctrlXTotalCoG,*m_pctrlYTotalCoG,*m_pctrlZTotalCoG;
	FloatEdit *m_pctrlTotalMass;

	double m_VolumeMass;
	CVector m_VolumeCoG;

	CVector m_PtRef;
	double m_CoGIxx, m_CoGIyy, m_CoGIzz, m_CoGIxz;
	double m_MassValue[MAXMASSES];
	CVector m_MassPosition[MAXMASSES];
	QString m_MassTag[MAXMASSES];
	int m_NMass;

	bool m_bChanged;
};

#endif // INERTIADLG_H
