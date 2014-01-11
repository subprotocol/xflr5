/****************************************************************************

	XFoilAdvancedDlg Class
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

#include <QLabel>
#include "../XDirect.h"
#include "XFoilAdvancedDlg.h"

XFoilAdvancedDlg::XFoilAdvancedDlg(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("XFoil Settings"));
	SetupLayout();

	m_IterLimit = 100;
	m_VAccel = 0.001;
	m_bInitBL = true;
	m_bFullReport = false;

	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}


void XFoilAdvancedDlg::SetupLayout()
{
	QHBoxLayout *VAccelBoxLayout = new QHBoxLayout;
	{
		QLabel *lab1 = new QLabel(tr("VAccel"));
		lab1->setAlignment(Qt::AlignRight);
		m_pctrlVAccel = new DoubleEdit;
		m_pctrlVAccel->setAlignment(Qt::AlignRight);
		VAccelBoxLayout->addStretch(1);
		VAccelBoxLayout->addWidget(lab1);
		VAccelBoxLayout->addWidget(m_pctrlVAccel);
	}

	QHBoxLayout *IterBoxLayout = new QHBoxLayout;
	{
		QLabel *lab2 = new QLabel(tr("Iteration Limit"));
		lab2->setAlignment(Qt::AlignRight);
		m_pctrlIterLimit = new IntEdit;

		IterBoxLayout->addStretch(1);
		IterBoxLayout->addWidget(lab2);
		IterBoxLayout->addWidget(m_pctrlIterLimit);
	}

	m_pctrlInitBL = new QCheckBox(tr("Re-initialize BLs after an unconverged iteration"));
	m_pctrlFullReport = new QCheckBox(tr("Show full log report for an XFoil analysis"));
	m_pctrlKeepErrorsOpen = new QCheckBox(tr("Keep Xfoil interface open if analysis errors"));

	QHBoxLayout *CommandButtonsLayout = new QHBoxLayout;
	{
		OKButton      = new QPushButton(tr("OK"));
		CancelButton  = new QPushButton(tr("Cancel"));
		CommandButtonsLayout->addStretch(1);
		CommandButtonsLayout->addWidget(OKButton);
		CommandButtonsLayout->addStretch(1);
		CommandButtonsLayout->addWidget(CancelButton);
		CommandButtonsLayout->addStretch(1);
	}

	QVBoxLayout *MainLayout = new QVBoxLayout;
	{
		MainLayout->addLayout(VAccelBoxLayout);
		MainLayout->addLayout(IterBoxLayout);
		MainLayout->addWidget(m_pctrlInitBL);
		MainLayout->addWidget(m_pctrlFullReport);
		MainLayout->addWidget(m_pctrlKeepErrorsOpen);
		MainLayout->addLayout(CommandButtonsLayout);
	}

	setLayout(MainLayout);
}


void XFoilAdvancedDlg::InitDialog()
{
	m_pctrlVAccel->SetValue(m_VAccel);
	m_pctrlInitBL->setChecked(m_bInitBL);
	m_pctrlIterLimit->SetValue(m_IterLimit);
	m_pctrlFullReport->setChecked(m_bFullReport);
	m_pctrlKeepErrorsOpen->setChecked(QXDirect::s_bKeepOpenErrors);
}




void XFoilAdvancedDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				OKButton->setFocus();
			}
			else if (OKButton->hasFocus())
			{
				OnOK();
			}
			break;
		}
		case Qt::Key_Escape:
		{
			reject();
			return;
		}
		default:
			event->ignore();
	}
}


void XFoilAdvancedDlg::OnOK()
{
	m_IterLimit = m_pctrlIterLimit->Value();
	m_VAccel = m_pctrlVAccel->Value();
	m_bInitBL = m_pctrlInitBL->isChecked();
	m_bFullReport = m_pctrlFullReport->isChecked();
	QXDirect::s_bKeepOpenErrors = m_pctrlKeepErrorsOpen->isChecked();
	done(1);
}
