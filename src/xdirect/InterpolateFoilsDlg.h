/****************************************************************************

	InterpolateFoilsDlg Class
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



#ifndef INTERPOLATEFOILSDLG_H
#define INTERPOLATEFOILSDLG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QSlider>
#include "../objects/Foil.h"
#include "../misc/DoubleEdit.h"

class InterpolateFoilsDlg : public QDialog
{
	Q_OBJECT

private slots:
	void OnSelChangeFoil1(int i);
	void OnSelChangeFoil2(int i);
	void OnFrac();
	void OnOK();
	void OnVScroll(int val);

	void keyPressEvent(QKeyEvent *event);

public:
	InterpolateFoilsDlg(QWidget *pParent=NULL);
	void InitDialog();
	void SetupLayout();
	void Update();

	static void *s_pXFoil;

	Foil* m_pBufferFoil;
	QList<void *> *m_poaFoil;

	QComboBox *m_pctrlFoil1, *m_pctrlFoil2;
	QLabel *m_pctrlCamb1, *m_pctrlCamb2, *m_pctrlThick1, *m_pctrlThick2;
	QLabel *m_pctrlCamb3, *m_pctrlThick3;
	QSlider *m_pctrlSlider;
	DoubleEdit *m_pctrlFrac;
	QPushButton *OKButton, *CancelButton;

	void *m_pXDirect;
	void *m_pAFoil;
	double m_Frac;
	QString m_NewFoilName;
};

#endif // INTERPOLATEFOILSDLG_H
