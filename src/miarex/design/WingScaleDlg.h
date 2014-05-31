/****************************************************************************

	WingScaleDlg Class
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

#ifndef WINGSCALEDLG_H
#define WINGSCALEDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include "../../misc/DoubleEdit.h"


class WingScaleDlg : public QDialog
{
	Q_OBJECT
	friend class QMiarex;
	friend class MainFrame;
public:
    WingScaleDlg(QWidget *pParent);
	void InitDialog(double const &RefSpan, double const &RefChord, double const &RefSweep, double const &RefTwist, const double &RefArea, const double &RefAR);
private:
	void SetupLayout();
	void ReadData();
	void SetResults();
	void EnableControls();

private slots:
	void OnOK();
	void OnClickedCheckBox();
	void OnEditingFinished();


public:

	QCheckBox *m_pctrlSpan, *m_pctrlChord, *m_pctrlSweep, *m_pctrlTwist;
	QCheckBox *m_pctrlScaleArea, *m_pctrlScaleAR;
	DoubleEdit  *m_pctrlNewSpan, *m_pctrlNewChord, *m_pctrlNewSweep, *m_pctrlNewTwist;
	DoubleEdit *m_pctrlNewArea, *m_pctrlNewAR;
	QLabel *m_pctrlRefSpan, *m_pctrlRefChord, *m_pctrlRefSweep, *m_pctrlRefTwist;
	QLabel *m_pctrlRefArea,*m_pctrlRefAR;
	QLabel *m_pctrlSpanRatio, *m_pctrlChordRatio, *m_pctrlSweepRatio, *m_pctrlTwistRatio;
	QLabel *m_pctrlAreaRatio,*m_pctrlARRatio;

	bool m_bSweep, m_bSpan, m_bChord, m_bTwist;
	bool m_bArea, m_bAR;

	double m_NewSweep, m_NewChord, m_NewTwist, m_NewSpan, m_NewArea, m_NewAR;
	double m_RefSweep, m_RefChord, m_RefTwist, m_RefSpan, m_RefArea, m_RefAR;
};

#endif // WINGSCALEDLG_H
