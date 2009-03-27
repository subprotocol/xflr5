/****************************************************************************

	PlaneDlg Class
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

#ifndef PLANEDLG_H
#define PLANEDLG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include "../Objects/Plane.h"
#include "../Misc/FloatEdit.h"

class PlaneDlg : public QDialog
{
	Q_OBJECT
	friend class QMiarex;
	friend class MainFrame;

public:
    PlaneDlg();
	void InitDialog();

private slots:
	void OnOK();
	void OnFin(int state);
	void OnStab(int state);
	void OnBodyCheck();
	void OnDefineWing();
	void OnDefineFin();
	void OnDefineStab();
	void OnChanged();
	void OnImportWing();
	void OnPlaneName();
	void OnSymFin(int state);
	void OnDoubleFin(int state);
	void OnExportWing();
	void OnBiplane();
	void OnDefineWing2();
	void OnImportWing2();
	void OnExportWing2();
	void OnEditBody();
	void OnSelChangeBodyList(int pos = 0);

private:
	void SetupLayout();
	void ComputePlane(void);
	void SetResults();
	void ReadParams();
	void SetParams();

private:
	static QList <void*> *s_poaWing;
	static QList <void*> *s_poaBody;

	static void * s_pMiarex;
	static void * s_pMainFrame;
	CPlane * m_pPlane;
	bool m_bChanged;
	bool m_bAcceptName;
	
	double pi;

private:
	QLabel *m_pctrlSurf1;
	QLabel *m_pctrlSurf2;
	QLabel *m_pctrlSurf3;
	QLabel *m_pctrlVolume;
	QLabel *m_pctrlLen1;
	QLabel *m_pctrlLen2;
	QLabel *m_pctrlLen3;
	QLabel *m_pctrlLen4;
	QLabel *m_pctrlLen5;
	QLabel *m_pctrlLen6;
	QLabel *m_pctrlLen7;
	QLabel *m_pctrlLen8;
	QLabel *m_pctrlLen9;
	QLabel *m_pctrlLen10;
	QLabel *m_pctrlLen11;
	QLabel *m_pctrlLen12;
	QLabel *m_pctrlLen13;
	QLabel *m_pctrlWingSpan;
	QLabel *m_pctrlWingSurface;
	QLabel *m_pctrlStabVolume;
	QLabel *m_pctrlFinSurface;
	QLabel *m_pctrlStabLeverArm;
	QLabel *m_pctrlStabSurface;
	QLabel *m_pctrlPlaneVolume;
	QLabel *m_pctrlVLMTotalPanels;
	FloatEdit  *m_pctrlXBody;
	FloatEdit  *m_pctrlZBody;
	FloatEdit  *m_pctrlXLEFin;
	FloatEdit  *m_pctrlYLEFin;
	FloatEdit  *m_pctrlZLEFin;
	FloatEdit  *m_pctrlZLEStab;
	FloatEdit  *m_pctrlXLEStab;
	FloatEdit  *m_pctrlXLEWing;
	FloatEdit  *m_pctrlZLEWing;
	FloatEdit  *m_pctrlXLEWing2;
	FloatEdit  *m_pctrlZLEWing2;
	FloatEdit  *m_pctrlStabTilt;
	FloatEdit  *m_pctrlFinTilt;
	FloatEdit  *m_pctrlWingTilt;
	FloatEdit  *m_pctrlWingTilt2;
	QLineEdit *m_pctrlPlaneName;
	QComboBox *m_pctrlBodyList;
	QCheckBox *m_pctrlBiplane;
	QCheckBox *m_pctrlBody;
	QCheckBox *m_pctrlStabCheck;
	QCheckBox *m_pctrlFinCheck;
	QCheckBox *m_pctrlDoubleFin;
	QCheckBox	*m_pctrlSymFin;
	QPushButton *m_pctrlDefineWing;
	QPushButton *m_pctrlImportWing;
	QPushButton *m_pctrlExportWing;
	QPushButton *m_pctrlDefineWing2;
	QPushButton *m_pctrlImportWing2;
	QPushButton *m_pctrlExportWing2;
	QPushButton	*m_pctrlDefineFin;
	QPushButton	*m_pctrlVTail;
	QPushButton	*m_pctrlDefineStab;
	QPushButton *m_pctrlEditBody;
	QPushButton *OKButton;
	QPushButton *CancelButton;

};

#endif // PLANEDLG_H
