/****************************************************************************

	SplineCtrlsDlg
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

#ifndef SPLINECTRLSDLG_H
#define SPLINECTRLSDLG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include "../Misc/FloatEdit.h"
#include "../Objects/Sf.h"
#include "../Objects/Pf.h"


class SplineCtrlsDlg : public QDialog
{
	Q_OBJECT
	friend class QAFoil;

public:
    SplineCtrlsDlg();
	void InitDialog();

private:
	void SetuLayout();

	FloatEdit	*m_pctrlOutExtrados;
	FloatEdit	*m_pctrlOutIntrados;
	QComboBox	*m_pctrlDegExtrados;
	QComboBox	*m_pctrlDegIntrados;
	QPushButton *OKButton, *CancelButton;
//	QButton *m_pctrlOutText;


protected:

	bool m_bSF;
	CSF *m_pSF;
	CPF *m_pPF;

};

#endif // SPLINECTRLSDLG_H
