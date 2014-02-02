/****************************************************************************

	StabViewDlg Class
	Copyright (C) 200-2010 Andre Deperrois adeperrois@xflr5.com

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


#ifndef STABVIEWDLG_H
#define STABVIEWDLG_H

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QDial>
#include <QRadioButton>
#include <QPushButton>
#include <QComboBox>
#include <QStackedWidget>
#include <QGroupBox>
#include <QTableView>
#include <QStandardItemModel>
#include "../../misc/DoubleEdit.h"
#include "../../graph/Curve.h"
#include "../../misc/FloatEditDelegate.h"


class StabViewDlg : public QWidget
{
	Q_OBJECT

	friend class QMiarex;
	friend class MainFrame;

public:
	StabViewDlg(QWidget *parent);
	~StabViewDlg();


private slots:
	void OnAnimate();
	void OnAnimateRestart();
	void OnAnimationSpeed(int val);
	void OnAnimationAmplitude(int val);
	void OnCellChanged(QWidget *);
	void OnModeSelection();
	void OnPlotStabilityGraph();
	void OnReadData();
	void OnResponseType();
	void OnAddCurve();
	void OnDeleteCurve();
	void OnRenameCurve();
	void OnSelChangeCurve(int sel);
	
private:
	void keyPressEvent(QKeyEvent *event);
	
	void AddCurve();
	void Connect();
//	void FillControlNames();
	void FillEigenThings();
	void FillCurveList();
	double GetControlInput(const double &time);
	void SetMode(int iMode=-1);
	void SetupLayout();
	void SetControls();
	void SetTimeCurveStyle(QColor const &Color, int const&Style, int const &Width, bool const& bCurve, bool const& bPoints);
	void ReadControlModelData();
	void UpdateControlModelData();

	static void *s_pMiarex;

	QRadioButton *m_pctrlLongDynamics,*m_pctrlLatDynamics;
	QRadioButton *m_pctrlRLMode1,*m_pctrlRLMode2,*m_pctrlRLMode3,*m_pctrlRLMode4;
	QRadioButton *m_pctrlTimeMode1,*m_pctrlTimeMode2,*m_pctrlTimeMode3,*m_pctrlTimeMode4;
	QLineEdit *m_pctrlEigenValue, *m_pctrlEigenVector1, *m_pctrlEigenVector2, *m_pctrlEigenVector3, *m_pctrlEigenVector4;

	QLabel *m_pctrlModeProperties;
//	QSlider *m_pctrlAnimationSpeed, *m_pctrlAnimationAmplitude;
	QDial *m_pctrlAnimationSpeed, *m_pctrlAnimationAmplitude;
	QPushButton *m_pctrlAnimate, *m_pctrlAnimateRestart;
	DoubleEdit *m_pctrlModeStep;
	
	QLabel *m_pctrlStabLabel1, *m_pctrlStabLabel2, *m_pctrlStabLabel3;
	DoubleEdit  *m_pctrlStabVar1, *m_pctrlStabVar2, *m_pctrlStabVar3;
	DoubleEdit *m_pctrlTotalTime, *m_pctrlDeltat;
	QPushButton *m_pctrlPlotStabGraph;
	QPushButton *m_pctrlAddCurve, *m_pctrlDeleteCurve, *m_pctrlRenameCurve;
	QComboBox *m_pctrlCurveList;

	QLabel *m_pctrlUnit1, *m_pctrlUnit2, *m_pctrlUnit3;
	DoubleEdit *m_pctrlFreqN, *m_pctrlFreq1, *m_pctrlDsi;
	QStackedWidget *m_pctrlStackWidget, *m_pctrlInitialConditionsWidget, *m_pctrlModeViewType;


	QRadioButton *m_pctrlModalResponse, *m_pctrlInitCondResponse, *m_pctrlForcedResponse;

	QTableView *m_pctrlControlTable;
	QStandardItemModel *m_pControlModel;
	FloatEditDelegate *m_pCtrlDelegate;

	int m_ModeInterval;
	double m_ModeAmplitude;
	double m_vabs[4], m_phi[4];

	int  *m_precision;

public:
	int m_iCurrentMode;	
	Curve *m_pCurve;
	double m_Time[20], m_Amplitude[20];
};

#endif // STABVIEWDLG_H
