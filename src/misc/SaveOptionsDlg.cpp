/****************************************************************************

	SaveOptionsDlg
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

#include "SaveOptionsDlg.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

SaveOptionsDlg::SaveOptionsDlg(QWidget *pParent) : QDialog(pParent)
{
	setWindowTitle(tr("Save Options"));
	m_bOpps = false;
	m_bWOpps = true;
	SetupLayout();
}

void SaveOptionsDlg::SetupLayout()
{
	QLabel *label = new QLabel(tr("Save:"));
	m_pctrlOpps  = new QCheckBox(tr("Foil Operating Points"));
	m_pctrlWOpps = new QCheckBox(tr("Wing and Plane Operating Points"));

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	QPushButton *OKButton = new QPushButton(tr("OK"));
	OKButton->setAutoDefault(false);
	QPushButton *CancelButton = new QPushButton(tr("Cancel"));
	CancelButton->setAutoDefault(false);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addWidget(label);
	MainLayout->addWidget(m_pctrlOpps);
	MainLayout->addWidget(m_pctrlWOpps);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	setLayout(MainLayout);
}

void SaveOptionsDlg::InitDialog(bool bOpps, bool bWOpps)
{
	m_bOpps  = bOpps;
	m_bWOpps = bWOpps;
	m_pctrlOpps->setChecked(m_bOpps);
	m_pctrlWOpps->setChecked(m_bWOpps);
}



void SaveOptionsDlg::OnOK()
{
	m_bOpps = m_pctrlOpps->isChecked();
	m_bWOpps = m_pctrlWOpps->isChecked();
	accept();
}
