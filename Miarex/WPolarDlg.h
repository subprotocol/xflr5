/****************************************************************************

	WPolarDlg Class
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


#ifndef WPOLARDLG_H
#define WPOLARDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include "../Misc/FloatEdit.h"
#include "../Objects/Plane.h"

class WPolarDlg : public QDialog
{
	Q_OBJECT
	friend class QMiarex;
	friend class MainFrame;

public:
    WPolarDlg();

private:
	void keyPressEvent(QKeyEvent *event);

	void SetupLayout();
	void Connect();
	void SetWPolarName();
	void SetReynolds();
	void SetWingLoad();
	void ReadValues();
	void InitDialog();
	void SetDensity();
	void EnableControls();


private slots:
	void OnOK();
	void OnArea();
	void OnUnit();
	void OnMethod();
	void OnWPolarName();
	void OnAutoName();
	void OnWakeRollUp();
	void OnTiltedGeom();
	void OnViscous();
	void OnGroundEffect();
	void OnWPolarType();
	void OnEditingFinished();

private:

	void *m_pMainFrame;

	QList <void *> *m_poaWPolar;
	CPlane *m_pPlane;
	CWing *m_pWing;

	QString m_WPolarName;
	QString m_UFOName;

	int m_Type;
	int m_UnitType;//1= International, 2= English
	int m_AnalysisType; //0=LLT;1=VLM;2=Panel

	bool m_bAutoName;
	bool m_bVLM1; //true if Classic, false if Quendez
	bool m_bThinSurfaces;//true if Plane Panel calculation on middle surface, false if on top & bottom
	bool m_bWakeRollUp;//true if wake roll up is to be taken into account in calculation
	bool m_bTiltedGeom;//true if calculation is performed on the tilted geometry, at alpha=0.0
	bool m_bViscous;
	bool m_bGround;

	double m_QInf, m_Weight, m_Alpha, m_XCmRef;
	double m_Beta;
	double m_Density, m_Viscosity;
	double m_WingLoad;
	double m_Height;

	int m_NXWakePanels;
	double m_TotalWakeLength;//% mac
	double m_WakePanelFactor;

	int m_RefAreaType;

	FloatEdit *m_pctrlXCmRef;
	FloatEdit *m_pctrlDensity;
	FloatEdit *m_pctrlViscosity;
	FloatEdit *m_pctrlAlpha;
	FloatEdit *m_pctrlBeta;
	FloatEdit *m_pctrlWeight;
	FloatEdit *m_pctrlQInf;
	FloatEdit *m_pctrlHeight;
	QLineEdit *m_pctrlWPolarName;

	QCheckBox *m_pctrlGroundEffect;
	QCheckBox *m_pctrlViscous;
	QCheckBox *m_pctrlTiltGeom;
	QCheckBox *m_pctrlWakeRollUp;
	QCheckBox *m_pctrlAutoName;

	QRadioButton *m_pctrlType1,*m_pctrlType2,*m_pctrlType4;
	QRadioButton *m_pctrlMethod1, *m_pctrlMethod2, *m_pctrlMethod3, *m_pctrlMethod4;
	QRadioButton *m_pctrlUnit1, *m_pctrlUnit2;
	QRadioButton *m_pctrlArea1, *m_pctrlArea2;

	QLabel *m_pctrlSRe;
	QLabel *m_pctrlRRe;
	QLabel *m_pctrlQInfCl;
	QLabel *m_pctrlReTip;
	QLabel *m_pctrlReRoot;
	QLabel *m_pctrlWingName;
	QLabel *m_pctrlWingLoad;
	QLabel *m_pctrlSpeedUnit;
	QLabel *m_pctrlWeightUnit;

	QLabel *m_pctrlRho, *m_pctrlNu;
	QLabel *m_pctrlDensityUnit, *m_pctrlViscosityUnit;

	QLabel *m_pctrlLengthUnit1, *m_pctrlLengthUnit2;

	QPushButton *m_pctrlWakeParams;
	QPushButton	*OKButton, *CancelButton;
};

#endif // WPOLARDLG_H
