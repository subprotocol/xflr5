/****************************************************************************

	WAdvancedDlg Class
	Copyright (C) 2009-2010 Andre Deperrois XFLR5@yahoo.com

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

#ifndef WADVANCEDDLG_H
#define WADVANCEDDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include "../Misc/FloatEdit.h"

class WAdvancedDlg : public QDialog
{
	Q_OBJECT
	friend class QMiarex;


public:
    WAdvancedDlg();
	void InitDialog();

private slots:
	void OnOK();
	void OnResetDefaults();

private:
	void keyPressEvent(QKeyEvent *event);
	void ReadParams();
	void SetParams();
	void SetupLayout();

	QLabel *m_pctrlAStat, *m_pctrlLength, *m_pctrlLength2;
	QPushButton *OKButton, *CancelButton;
	QCheckBox *m_pctrlLogFile;
	QCheckBox *m_pctrlResetWake;
	QCheckBox *m_pctrlKeepOutOpps;
	QRadioButton *m_pctrlDirichlet, *m_pCtrlNeumann;
	FloatEdit *m_pctrlInterNodes;
	FloatEdit *m_pctrlRelax;
	FloatEdit *m_pctrlAlphaPrec;
	FloatEdit *m_pctrlMinPanelSize;
	FloatEdit *m_pctrlNStation;
	FloatEdit *m_pctrlIterMax;
	FloatEdit *m_pctrlMaxWakeIter;
	FloatEdit *m_pctrlCoreSize;
	FloatEdit *m_pctrlVortexPos;
	FloatEdit *m_pctrlControlPos;

	bool m_bLogFile;
	bool m_bDirichlet;
	bool m_bTrefftz;
	bool m_bKeepOutOpps;
	bool m_bResetWake;

	int m_Iter;
	int m_NStation;
	int m_WakeInterNodes;
	int m_MaxWakeIter;
	int m_InducedDragPoint;

	double m_ControlPos, m_VortexPos;
	double m_Relax, m_AlphaPrec;
	double m_CoreSize;
	double m_MinPanelSize;

	void* m_pMainFrame;
};

#endif // WADVANCEDDLG_H
