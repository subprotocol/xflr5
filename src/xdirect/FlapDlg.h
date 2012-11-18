/****************************************************************************

	FlapDlg class
	Copyright (C) 2004-2009 Andre Deperrois adeperrois@xflr5.com

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

#ifndef FLAPDLG_H
#define FLAPDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QPushButton>
#include "../objects/Foil.h"
#include "../misc/FloatEdit.h"


class FlapDlg : public QDialog
{
	Q_OBJECT

public:
    FlapDlg(QWidget *pParent);

	friend class QAFoil;
	friend class QXDirect;


private:
	void EnableTEFlap(bool bEnable);
	void EnableLEFlap(bool bEnable);
	void ReadParams();
	void SetupLayout();

	FloatEdit	*m_pctrlLEYHinge;
	FloatEdit	*m_pctrlLEXHinge;
	FloatEdit	*m_pctrlLEFlapAngle;
	FloatEdit	*m_pctrlTEYHinge;
	FloatEdit	*m_pctrlTEXHinge;
	FloatEdit	*m_pctrlTEFlapAngle;

	QCheckBox *m_pctrlLEFlapCheck;
	QCheckBox *m_pctrlTEFlapCheck;
	QPushButton	*OKButton, *CancelButton, *ApplyButton;

protected:
	void keyPressEvent(QKeyEvent *event);
	void InitDialog();

private slots:
	void OnApply();
	void OnLEFlapCheck(int state);
	void OnTEFlapCheck(int state);
	void OnChanged();
	virtual void OnOK();

private:
	bool m_bTEFlap;
	bool m_bLEFlap;
	bool m_bApplied;
	bool m_bModified;

	double m_LEXHinge, m_LEYHinge, m_LEFlapAngle;
	double m_TEXHinge, m_TEYHinge, m_TEFlapAngle;
	CFoil *m_pMemFoil, *m_pBufferFoil;

	void *m_pXFoil;
	void *m_pXDirect;
	void *m_pAFoil;

};

#endif // FLAPDLG_H
