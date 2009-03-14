/****************************************************************************

	GL3DScales Class
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

#ifndef GL3DSCALES_H
#define GL3DSCALES_H

#include <QWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>
#include "Miarex.h"
#include "../Misc/FloatEdit.h"

class GL3DScales : public QWidget
{
	Q_OBJECT
	friend class MainFrame;
	friend class QMiarex;


public:
	GL3DScales(QWidget *parent = NULL);
	void InitDialog();

private slots:
	void OnCpScale();
	void OnApply();
	void OnLiftScale(int pos);
	void OnDragScale(int pos);
	void OnVelocityScale(int pos);


private:
	void showEvent(QShowEvent *event);
	void hideEvent(QHideEvent *event);

	void SetupLayout();
	void ReadStreamParams();

	bool LoadSettings(QDataStream &ar);
	bool SaveSettings(QDataStream &ar);

	QSlider *m_pctrlLiftScaleSlider, *m_pctrlDragScaleSlider, *m_pctrlVelocityScaleSlider;
	QPushButton *ApplyButton;
	QCheckBox *m_pctrlAutoCpScale;
	FloatEdit	*m_pctrlLegendMin, *m_pctrlLegendMax;

	FloatEdit *m_pctrlNXPoint, *m_pctrlDeltaL, *m_pctrlXFactor, *m_pctrlXOffset, *m_pctrlZOffset;
	QRadioButton *m_pctrlLE, *m_pctrlTE, *m_pctrlLine;

	QLabel *m_pctrlLengthUnit1, *m_pctrlLengthUnit2, *m_pctrlLengthUnit3;


	void *m_pMiarex;
	void *m_pMainFrame;

	int m_pos;
	int m_NX;
	double m_DeltaL;
	double m_XFactor;
	double m_XOffset, m_ZOffset;
};

#endif // GL3DSCALES_H
