/****************************************************************************

	XFoilAdvancedDlg Class
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


#include "XFoilAdvancedDlg.h"

XFoilAdvancedDlg::XFoilAdvancedDlg()
{
	setWindowTitle("XFoil Settings");
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
	QHBoxLayout *VAccelBox = new QHBoxLayout;
	QLabel *lab1 = new QLabel("VAccel");
	lab1->setAlignment(Qt::AlignRight);
	m_pctrlVAccel = new FloatEdit;
	m_pctrlVAccel->setAlignment(Qt::AlignRight);
	VAccelBox->addStretch(1);
	VAccelBox->addWidget(lab1);
	VAccelBox->addWidget(m_pctrlVAccel);

	QHBoxLayout *IterBox = new QHBoxLayout;
	QLabel *lab2 = new QLabel("Iteration Limit");
	lab2->setAlignment(Qt::AlignRight);
	m_pctrlIterLimit = new QLineEdit;
	m_pctrlIterLimit->setAlignment(Qt::AlignRight);
	QValidator *IterValid = new QIntValidator(0, 100000000, this);
	m_pctrlIterLimit->setValidator(IterValid);
	IterBox->addStretch(1);
	IterBox->addWidget(lab2);
	IterBox->addWidget(m_pctrlIterLimit);

	m_pctrlInitBL = new QCheckBox("Re-initialize BLs after an unconverged iteration");
	m_pctrlFullReport = new QCheckBox("Show full log report for an XFoil analysis");

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton      = new QPushButton(tr("OK"));
	CancelButton  = new QPushButton(tr("Cancel"));
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);


	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addLayout(VAccelBox);
	MainLayout->addLayout(IterBox);
	MainLayout->addWidget(m_pctrlInitBL);
	MainLayout->addWidget(m_pctrlFullReport);
	MainLayout->addLayout(CommandButtons);

	setLayout(MainLayout);
}


void XFoilAdvancedDlg::InitDialog()
{
	m_pctrlVAccel->SetValue(m_VAccel);
	m_pctrlInitBL->setChecked(m_bInitBL);
	QString str = QString("%1").arg(m_IterLimit);
	m_pctrlIterLimit->setText(str);
	m_pctrlFullReport->setChecked(m_bFullReport);
}




void XFoilAdvancedDlg::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Return:
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
	QString str = m_pctrlIterLimit->text();
	m_IterLimit = str.toInt();
	m_VAccel = m_pctrlVAccel->GetValue();
	m_bInitBL = m_pctrlInitBL->isChecked();
	m_bFullReport = m_pctrlFullReport->isChecked();
	done(1);
}
