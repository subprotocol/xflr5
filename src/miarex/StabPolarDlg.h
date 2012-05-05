/****************************************************************************

	StabPolarDlg Class
	Copyright (C) 2010 Andre Deperrois adeperrois@xflr5.com

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

 
#ifndef STABPOLARDLG_H
#define STABPOLARDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QStackedWidget>
#include <QTableView>
#include <QStandardItemModel>
#include "CtrlTableDelegate.h"
#include "../misc/FloatEdit.h"
#include "../objects/Plane.h"
//#include "../objects/WPolar.h"



class StabPolarDlg : public QDialog
{
	Q_OBJECT
	friend class QMiarex;
	friend class MainFrame;

public:
	StabPolarDlg();
	void InitDialog(CPlane *pPlane, CWing *pWing, CWPolar *pWPolar);

private:
	void SetupLayout();
	void Connect();
	

private slots:
	void OnOK();
	void OnAutoName();
	void OnWPolarName();
	void OnArea();
	void OnEditingFinished();
	void OnViscous();
	void OnUnit();
	void OnCellChanged(QWidget *pWidget);
	void OnMethod();
	void OnAutoInertia();


private:
	void keyPressEvent(QKeyEvent *event);
	void showEvent(QShowEvent *event);
	void ReadCtrlData();
	void FillControlList();
	void FillUFOInertia();
	void ReadParams();
	void SetDensity();
	void SetWPolarName();
	void SetViscous();

	QTableView *m_pctrlControlTable;
	QStandardItemModel *m_pControlModel;
	CtrlTableDelegate *m_pCtrlDelegate;

	FloatEdit *m_pctrlDensity;
	FloatEdit *m_pctrlViscosity;
	FloatEdit *m_pctrlBeta;
	FloatEdit *m_pctrlPhi;
	QLineEdit *m_pctrlWPolarName;

	QCheckBox *m_pctrlViscous;
	QCheckBox *m_pctrlAutoName;
	QCheckBox *m_pctrlPlaneInertia;
//	QCheckBox *m_pctrlAVLControls;

	QRadioButton *m_pctrlUnit1, *m_pctrlUnit2;
	QRadioButton *m_pctrlArea1, *m_pctrlArea2;


	QStackedWidget *m_pctrlAnalysisControls;
	QRadioButton *m_pctrlWingMethod2, *m_pctrlWingMethod3;
	QRadioButton *m_pctrlPanelMethod;

	QLabel *m_pctrlUFOName;

	QLabel *m_pctrlRho, *m_pctrlNu;
	QLabel *m_pctrlDensityUnit, *m_pctrlViscosityUnit;
	QLabel *m_pctrlLab299,*m_pctrlLab300,*m_pctrlLab301,*m_pctrlLab302,*m_pctrlLab303,*m_pctrlLab304,*m_pctrlLab305;

	FloatEdit *m_pctrlMass, *m_pctrlCoGx,*m_pctrlCoGz, *m_pctrlIxx, *m_pctrlIyy, *m_pctrlIzz, *m_pctrlIxz;

	QPushButton *OKButton, *CancelButton;

	static void *s_pMainFrame;
	static void *s_pMiarex;
	static CWPolar s_StabPolar;

	CPlane *m_pPlane;
	CWing *m_pWingList[MAXWINGS];         // pointers to the four wings of the currently selected plane

	bool m_bAutoName;
	int m_UnitType;//1= International, 2= Imperial

/*	bool m_bThinSurfaces;
	double m_Density, m_Viscosity;
	double m_SideSlip, m_BankAngle;
	double m_WingTilt;
	double m_StabTilt;
	double m_Mass;
	bool m_bAutoInertia;
	CVector m_CoG;
	double m_CoGIxx,m_CoGIyy,m_CoGIzz,m_CoGIxz;
	int m_nControls;
	enumPolarType m_WPolarType;
	int m_RefAreaType;
	double m_MinControl[MAXWINGS*MAXCONTROLS], m_MaxControl[MAXWINGS*MAXCONTROLS];//4 wings altogether, MAXCONTROLS on each
	bool m_bActiveControl[MAXWINGS*MAXCONTROLS];
	bool m_bViscous;
	bool m_bAVLControls;*/

};

#endif // STABPOLARDLG_H
