/****************************************************************************

	InterpolateFoilsDlg Class
	Copyright (C) 2008 Andre Deperrois XFLR5@yahoo.com

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
#include <QtGui>
#include "../Objects/Foil.h"
#include "../Misc/FloatEdit.h"

class InterpolateFoilsDlg : public QDialog
{
	Q_OBJECT

private slots:
	void OnSelChangeFoil1(int i);
	void OnSelChangeFoil2(int i);
	void OnFrac();
	void OnOK();
	void OnVScroll(int val);

public:
	InterpolateFoilsDlg(void *pParent = NULL);
	void InitDialog();
	void SetupLayout();
	void Update();

	CFoil* m_pBufferFoil;
	QList<void *> *m_poaFoil;

	QComboBox *m_pctrlFoil1, *m_pctrlFoil2;
	QLabel *m_pctrlCamb1, *m_pctrlCamb2, *m_pctrlThick1, *m_pctrlThick2;
	QLabel *m_pctrlCamb3, *m_pctrlThick3;
	QLineEdit *m_pctrlNewFoilName;
	QSlider *m_pctrlSlider;
	FloatEdit *m_pctrlFrac;
	QPushButton *OKButton, *CancelButton;

	void *m_pXFoil;
	void* m_pXDirect;
	void* m_pMainFrame;
	double m_Frac;
	QString m_NewFoilName;
};

#endif // INTERPOLATEFOILSDLG_H
