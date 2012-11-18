/****************************************************************************

	WPolarDlg Class
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


#ifndef WPOLARDLG_H
#define WPOLARDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QRadioButton>
#include "../misc/FloatEdit.h"
#include "../objects/Plane.h"

class WPolarDlg : public QDialog
{
	Q_OBJECT
	friend class QMiarex;
	friend class MainFrame;

public:
    WPolarDlg(QWidget *pParent);

private:
	void keyPressEvent(QKeyEvent *event);

	void SetupLayout();
	void Connect();
	void SetWPolarName();
	void SetReynolds();
	void SetWingLoad();
	void ReadValues();
	void InitDialog(CPlane *pPlane, CWing *pWing, CWPolar *pWPolar=NULL);
	void SetDensity();
	void EnableControls();


private slots:
	void OnOK();
	void OnArea();
	void OnUnit();
	void OnMethod();
	void OnWPolarName();
	void OnAutoName();
	void OnTiltedGeom();
	void OnViscous();
	void OnGroundEffect();
	void OnWPolarType();
	void OnEditingFinished();
	void OnPlaneInertia();

private:
	static void *s_pMainFrame;
	static void *s_pMiarex;

//	QList <void *> *m_poaWPolar;
	CPlane *m_pPlane;
	CWing *m_pWing;

//	QString m_WPolarName;
//	QString m_UFOName;

	static CWPolar s_WPolar;
	bool m_bAutoName;
	int m_UnitType;//1= International, 2= English
	double m_WingLoad;
//	enumPolarType m_WPolarType; // fixed speed, fixed lift, fixed aoa

	QStackedWidget *m_pctrlAnalysisControls;

	FloatEdit *m_pctrlXCmRef, *m_pctrlZCmRef;
	FloatEdit *m_pctrlDensity;
	FloatEdit *m_pctrlViscosity;
	FloatEdit *m_pctrlAlpha;
	FloatEdit *m_pctrlBeta;
	FloatEdit *m_pctrlWeight;
	FloatEdit *m_pctrlQInf;
	FloatEdit *m_pctrlHeight;
	QLineEdit *m_pctrlWPolarName;

	QCheckBox *m_pctrlPlaneInertia;
	QCheckBox *m_pctrlGroundEffect;
	QCheckBox *m_pctrlViscous;
	QCheckBox *m_pctrlTiltGeom;
	QCheckBox *m_pctrlAutoName;

	QRadioButton *m_pctrlType1,*m_pctrlType2,*m_pctrlType4;
	QRadioButton *m_pctrlWingMethod1, *m_pctrlWingMethod2, *m_pctrlWingMethod3;
	QRadioButton *m_pctrlPanelMethod;
	QRadioButton *m_pctrlUnit1, *m_pctrlUnit2;
	QRadioButton *m_pctrlArea1, *m_pctrlArea2;

	QLabel *m_pctrlSRe;
	QLabel *m_pctrlRRe;
	QLabel *m_pctrlQInfCl;
	QLabel *m_pctrlUFOName;
	QLabel *m_pctrlWingLoad;
	QLabel *m_pctrlSpeedUnit;
	QLabel *m_pctrlWeightUnit;

	QLabel *m_pctrlRho, *m_pctrlNu;
	QLabel *m_pctrlDensityUnit, *m_pctrlViscosityUnit;

	QLabel *m_pctrlLengthUnit1, *m_pctrlLengthUnit2, *m_pctrlLengthUnit3;

	QPushButton	*OKButton, *CancelButton;
};

#endif // WPOLARDLG_H
