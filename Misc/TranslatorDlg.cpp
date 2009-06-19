/****************************************************************************

	TranslatorDlg Class
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

#include "TranslatorDlg.h"
#include <QHBoxLayout>

TranslatorDlg::TranslatorDlg()
{
	setWindowTitle("Language settings");
	SetupLayout();
	InitDialog();
}


void TranslatorDlg::InitDialog()
{
	//list all ts files...
	//read headers
	//fill QListWidget for user selection
}


void TranslatorDlg::SetupLayout()
{
	m_pctrlLanguageList = new QListWidget;
	m_pctrlLanguageList->setMinimumHeight(300);
	connect(m_pctrlLanguageList, SIGNAL(itemClicked(QListWidgetItem *)),       this, SLOT(OnSelChangeList(QListWidgetItem *)));
	connect(m_pctrlLanguageList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(OnDoubleClickList(QListWidgetItem *)));

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton = new QPushButton(tr("Close"));
	OKButton->setAutoDefault(true);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addStretch(1);
	MainLayout->addWidget(m_pctrlLanguageList);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);
}



void TranslatorDlg::OnOK()
{
	//read user language selection and exit
	QListWidgetItem *pItem =  m_pctrlLanguageList->currentItem();
	QString str = pItem->text();
	accept();
}




