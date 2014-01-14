/****************************************************************************

	FoilPolarDlg Class
	Copyright (C) 2008 Andre Deperrois adeperrois@xflr5.com

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


#ifndef FOILPOLARDLG_H
#define FOILPOLARDLG_H

#include <QDialog>
#include <QRadioButton>
#include <QLabel>
#include <QString>
#include "../../misc/DoubleEdit.h"
#include "../../objects/Polar.h"


class FoilPolarDlg : public QDialog
{
    friend class QOperWidget;

    Q_OBJECT

public:
	FoilPolarDlg(QWidget *pParent=NULL);

	void ReadParams();
	void keyPressEvent(QKeyEvent *event);
	void InitDialog();
	void SetPlrName();
	void SetupLayout();
	void SetDensity();


	QRadioButton *m_pctrlAuto1;
	QRadioButton *m_pctrlAuto2;

	QLabel *m_pctrlReLabel;
	QLabel *m_pctrlMachLabel;

	QLineEdit *m_pctrlAnalysisName;
	QRadioButton *m_rbtype1;
	QRadioButton *m_rbtype2;
	QRadioButton *m_rbtype3;
	QRadioButton *m_rbtype4;

    DoubleEdit *m_pctrlReynolds;
    DoubleEdit *m_pctrlMach;

    DoubleEdit *m_pctrlChord, *m_pctrlMass, *m_pctrlSpan;
	QLabel *m_pctrlLengthUnit1, *m_pctrlLengthUnit2, *m_pctrlMassUnit;

	QRadioButton *m_pctrlUnit1, *m_pctrlUnit2;
	QLabel *m_pctrlRho, *m_pctrlNu, *m_pctrlViscosityUnit, *m_pctrlDensityUnit;
    DoubleEdit *m_pctrlDensity, *m_pctrlViscosity;

	QPushButton *OKButton;
	QPushButton *CancelButton;

    DoubleEdit *m_pctrlNCrit;
    DoubleEdit *m_pctrlTopTrans;
    DoubleEdit *m_pctrlBotTrans;

	bool  m_bAutoName;
	enumPolarType m_PolarType;
	int m_MaTypDef, m_ReTypDef;

	static int s_UnitType;
	static double s_Viscosity, s_Density;
	static double s_Chord, s_Span, s_Mass;

	double m_Reynolds;
	double m_Mach;
	double m_ReDef;
	double m_ASpec;
	double m_XTop, m_XBot;
	double m_NCrit;
	QString m_FoilName;
	QString m_PlrName;

public slots:
	void OnAutoName();
	void OnOK();
	void OnPolarType();
	void OnNameChanged();
	void EditingFinished();
	void OnUnit();
	void OnEditingFinished();
};

#endif // FOILPOLARDLG_H
