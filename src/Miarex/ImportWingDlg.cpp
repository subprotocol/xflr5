/****************************************************************************

	ImportWingDlg Class
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

#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QLabel>
#include <QPushButton>
#include "../Objects/Wing.h"
#include "ImportWingDlg.h"


ImportWingDlg::ImportWingDlg()
{
	setWindowTitle(tr("Import Wing Dialog"));
	SetupLayout();
}


void ImportWingDlg::InitDialog()
{
	CWing *pWing;
	for(int i=0;i<m_poaWing->size(); i++)
	{
		pWing = (CWing*)m_poaWing->at(i);
		m_pctrlNameList->addItem(pWing->m_WingName);
	}
}


void ImportWingDlg::SetupLayout()
{
	QDesktopWidget desktop;
	QRect r = desktop.geometry();
	setMinimumHeight(r.height()/3);
	move(r.width()/3, r.height()/6);

	QVBoxLayout *MainLayout = new QVBoxLayout;

	QLabel *lab = new QLabel(tr("Select the wing to import"));

	m_pctrlNameList = new QListWidget;
	m_pctrlNameList->setMinimumHeight(300);
	connect(m_pctrlNameList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(OnOK()));

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

	MainLayout->addStretch(1);
	MainLayout->addWidget(lab);
	MainLayout->addWidget(m_pctrlNameList);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);
}



void ImportWingDlg::OnOK()
{
	QListWidgetItem *pItem =  m_pctrlNameList->currentItem();
	m_WingName = pItem->text();
	QDialog::accept();
}

