/****************************************************************************

	CtrlPolarDlg Class
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

 
#ifndef CTRLPOLARDLG_H
#define CTRLPOLARDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QTableView>
#include <QStandardItemModel>
#include "CtrlTableDelegate.h"
#include "../Misc/FloatEdit.h"
#include "../Objects/Plane.h"



class CtrlPolarDlg : public QDialog
{
	Q_OBJECT
	friend class QMiarex;
	friend class MainFrame;

public:
	CtrlPolarDlg();
	void InitDialog();

private:
	void SetupLayout();
	void Connect();
	

private slots:
	void OnOK();
	void OnArea();
	void OnEditingFinished();
	void OnViscous();
	void OnType();
	void OnUnit();
	void OnCellChanged(QWidget *pWidget);

private:
	void resizeEvent(QResizeEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void ReadCtrlData();
	void FillControlList();
	void ReadParams();
	void SetDensity();
	void SetWPolarName();

	QTableView *m_pctrlControlTable;
	QStandardItemModel *m_pControlModel;
	CtrlTableDelegate *m_pCtrlDelegate;

	FloatEdit *m_pctrlDensity;
	FloatEdit *m_pctrlViscosity;
	FloatEdit *m_pctrlWeight;
	FloatEdit *m_pctrlQInf;
	QLineEdit *m_pctrlWPolarName;

	QCheckBox *m_pctrlViscous;
	QCheckBox *m_pctrlAutoName;

	QRadioButton *m_pctrlType5,*m_pctrlType6;
	QRadioButton *m_pctrlUnit1, *m_pctrlUnit2;
	QRadioButton *m_pctrlArea1, *m_pctrlArea2;

	QLabel *m_pctrlUFOName;
	QLabel *m_pctrlSpeedUnit;
	QLabel *m_pctrlWeightUnit;

	QLabel *m_pctrlRho, *m_pctrlNu;
	QLabel *m_pctrlDensityUnit, *m_pctrlViscosityUnit;


	QPushButton *OKButton, *CancelButton;

	void *m_pMainFrame;
	void *m_pMiarex;

	QList <void*> *m_poaXPolar;

	QString m_WPolarName, m_UFOName;
	bool m_bAutoName;

	double m_Density, m_Viscosity;

	double m_XCmRef;
	double m_WingTilt;
	double m_StabTilt;

	CPlane *m_pPlane;
	CWing *m_pWing;
	CWing *m_pWing2;
	CWing *m_pStab;
	CWing *m_pFin;

	int m_UnitType;//1= International, 2= Imperial
	int m_nControls;
	int m_Type;
	int m_RefAreaType;

	double m_MinControl[4*MAXCONTROLS], m_MaxControl[4*MAXCONTROLS];//4 wings altogether, MAXCONTROLS on each
	bool m_bActiveControl[4*MAXCONTROLS];
	bool m_bViscous;
	double m_QInf;
	double m_Mass;
};

#endif // CTRLPOLARDLG_H
