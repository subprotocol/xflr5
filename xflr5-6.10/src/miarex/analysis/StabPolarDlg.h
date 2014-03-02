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
#include "./CtrlTableDelegate.h"
#include "../../misc/DoubleEdit.h"
#include "../../objects/Plane.h"




class CtrlTableModel: public QStandardItemModel
{
public:
    CtrlTableModel(QObject * parent=NULL) : QStandardItemModel(parent)  { }

    Qt::ItemFlags flags(const QModelIndex & index) const
    {
	    if (index.column() == 0)
		    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	    else
		    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
};


class StabPolarDlg : public QDialog
{
	Q_OBJECT
	friend class QMiarex;
	friend class MainFrame;

public:
	StabPolarDlg(QWidget *pParent=NULL);
	~StabPolarDlg();

	void InitDialog(Plane *pPlane, WPolar *pWPolar=NULL);

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
	void OnIgnoreBodyPanels();
	void OnUnit();
	void OnCellChanged(QWidget *);
	void OnMethod();
	void OnAutoInertia();


private:
	void keyPressEvent(QKeyEvent *event);
	void EnableControls();
	void FillControlList();
	void FillUFOInertia();
	void ReadCtrlData();
	void ReadParams();
	void SetDensity();
	void SetWPolarName();
	void SetViscous();

	QTableView *m_pctrlControlTable;
	CtrlTableModel *m_pControlModel;
	CtrlTableDelegate *m_pCtrlDelegate;

	DoubleEdit *m_pctrlDensity;
	DoubleEdit *m_pctrlViscosity;
	DoubleEdit *m_pctrlBeta;
	DoubleEdit *m_pctrlPhi;
	QLineEdit *m_pctrlWPolarName;

	QCheckBox *m_pctrlViscous;
	QCheckBox *m_pctrlAutoName;
	QCheckBox *m_pctrlPlaneInertia;
	QCheckBox *m_pctrlIgnoreBodyPanels;

	QRadioButton *m_pctrlUnit1, *m_pctrlUnit2;
	QRadioButton *m_pctrlArea1, *m_pctrlArea2, *m_pctrlArea3;

	DoubleEdit *m_pctrlRefChord, *m_pctrlRefArea, *m_pctrlRefSpan;

	QStackedWidget *m_pctrlAnalysisControls;
	QRadioButton *m_pctrlWingMethod2, *m_pctrlWingMethod3;
	QRadioButton *m_pctrlPanelMethod;

	QLabel *m_pctrlRho, *m_pctrlNu;
	QLabel *m_pctrlDensityUnit, *m_pctrlViscosityUnit;

	DoubleEdit *m_pctrlMass, *m_pctrlCoGx,*m_pctrlCoGz, *m_pctrlIxx, *m_pctrlIyy, *m_pctrlIzz, *m_pctrlIxz;

	QTabWidget *m_pTabWidget;
	QWidget  *m_pMethodPage, *m_pOptionPage, *m_pPlaneDataPage, *m_pControlPage;

	int  *m_precision;

	QPushButton *OKButton, *CancelButton;

	static WPolar s_StabPolar;

	Plane *m_pPlane;
	Wing *m_pWingList[MAXWINGS];         // pointers to the four wings of the currently selected plane


	bool m_bAutoName;
	int m_UnitType;//1= International, 2= Imperial
};




#endif // STABPOLARDLG_H
