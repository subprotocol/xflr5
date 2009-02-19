/****************************************************************************

	RenameDlg Class
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



#include "RenameDlg.h"

RenameDlg::RenameDlg(void *pParent)
{
	m_pMainFrame =pParent;
	m_bEnableOverwrite = true;
	SetupLayout();
}


void RenameDlg::SetupLayout()
{
	QDesktopWidget desktop;
	QRect r = desktop.geometry();
	setMinimumHeight(r.height()/3);
	move(r.width()/3, r.height()/6);

	QVBoxLayout *MainLayout = new QVBoxLayout;

	m_pctrlMessage = new QLabel("A Message here");

	m_pctrlNote = new QLabel("Enter the new name");
	m_pctrlNote->setMinimumWidth(350);

	m_pctrlName = new QLineEdit("");
	m_pctrlNameList = new QListWidget;
	m_pctrlNameList->setMinimumHeight(300);
	connect(m_pctrlNameList,    SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(OnSelChangeList(QListWidgetItem *)));

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton = new QPushButton(tr("OK"));
	OKButton->setAutoDefault(false);
	CancelButton = new QPushButton(tr("Cancel"));
	CancelButton->setAutoDefault(false);
	OverwriteButton = new QPushButton(tr("Overwrite"));
	OverwriteButton->setAutoDefault(false);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OverwriteButton);
	CommandButtons->addStretch(1);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(OverwriteButton, SIGNAL(clicked()), this, SLOT(OnOverwrite()));

	QLabel *m_pctrlNote = new QLabel;
	m_pctrlNote->setText("Note : Overwrite will delete Opps and reset polars");

	MainLayout->addStretch(1);
	MainLayout->addWidget(m_pctrlMessage);
	MainLayout->addWidget(m_pctrlName);
	MainLayout->addStretch(1);
	MainLayout->addWidget(m_pctrlNameList);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addWidget(m_pctrlNote);
	MainLayout->addStretch(1);

	setLayout(MainLayout);
}


void RenameDlg::InitDialog()
{
	m_pctrlNote->setText(m_Note);

	if(!m_bEnableOverwrite) OverwriteButton->setEnabled(false);
	m_StartName = m_strName;
	m_pctrlMessage->setText(m_strQuestion);
	m_pctrlName->setText(m_StartName);
	m_pctrlName->setFocus();
	m_pctrlName->selectAll();

	for (int i=0; i<m_pstrArray->size(); i++){
		m_pctrlNameList->addItem(m_pstrArray->at(i));
	}
}




void RenameDlg::keyPressEvent(QKeyEvent *event)
{
	// Prevent Return Key from closing App
	switch (event->key())
	{
		case Qt::Key_Return:
		{
			if(!OKButton->hasFocus() && !CancelButton->hasFocus())
			{
				OKButton->setFocus();
			}
			else
			{
				QDialog::accept();
			}
			break;
		}
		default:
		QDialog::keyPressEvent(event);
	}

	QDialog::keyPressEvent(event);
}



void RenameDlg::OnOverwrite()
{
	m_strName = m_pctrlName->text();
	done(10);
}


void RenameDlg::OnOK()
{
	m_strName = m_pctrlName->text();
	if (!m_strName.length())
	{
		QMessageBox::warning(this, "QFLR5", "Must enter a name");
		m_pctrlName->setFocus();
		return;
	}

	QString strong;

	//exists ?
	for (int l=0; l<m_pstrArray->size(); l++)
	{
		strong = m_pstrArray->at(l);
		if(strong == m_strName)
		{
			QString str = "Do you wish to overwrite "+m_strName + " ?";
			if (QMessageBox::Yes == QMessageBox::question(window(), "QFLR5", str,
														  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel))
			{
				done(10);
				return;
			}
			else return;
		}
	}

	QDialog::accept();
}


void RenameDlg::OnSelChangeList(QListWidgetItem * pItem)
{
//	QListWidgetItem *pItem =  m_pctrlNameList->currentItem();

	QString str = pItem->text();
	m_pctrlName->setText(str);
	m_pctrlName->selectAll();
}













