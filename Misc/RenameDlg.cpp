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
	setWindowTitle("Rename");
	m_pMainFrame =pParent;
	m_bEnableOverwrite = true;
	SetupLayout();
}


void RenameDlg::SetupLayout()
{
	QVBoxLayout *MainLayout = new QVBoxLayout;

	m_pctrlMessage = new QLabel("A Message here");

	m_pctrlNote = new QLabel(tr("Enter the new name"));
	m_pctrlNote->setMinimumWidth(350);

	m_pctrlName = new QLineEdit("");
	QLabel* NameListLabel = new QLabel(tr("Existing Names:"));
	m_pctrlNameList = new QListWidget;
//	m_pctrlNameList->setMinimumHeight(300);
	connect(m_pctrlNameList, SIGNAL(currentRowChanged(int)), this, SLOT(OnSelChangeList(int)));
	connect(m_pctrlNameList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(OnDoubleClickList(QListWidgetItem *)));

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

	QLabel *LabelNote = new QLabel;
	LabelNote->setText("Note : Overwrite will delete Opps and reset polars");

	MainLayout->setStretchFactor(m_pctrlMessage, 1);
	MainLayout->setStretchFactor(m_pctrlName, 1);
	MainLayout->setStretchFactor(NameListLabel, 1);
	MainLayout->setStretchFactor(m_pctrlNameList, 5);
	MainLayout->setStretchFactor(CommandButtons, 1);
	MainLayout->setStretchFactor(LabelNote, 1);

	MainLayout->addWidget(m_pctrlMessage);
	MainLayout->addWidget(m_pctrlName);
	MainLayout->addStretch(1);
	MainLayout->addWidget(NameListLabel);
	MainLayout->addWidget(m_pctrlNameList);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addWidget(LabelNote);

	MainLayout->setStretchFactor(m_pctrlMessage, 1);
	MainLayout->setStretchFactor(m_pctrlName, 1);
	MainLayout->setStretchFactor(NameListLabel, 1);
	MainLayout->setStretchFactor(m_pctrlNameList, 5);
	MainLayout->setStretchFactor(CommandButtons, 1);
	MainLayout->setStretchFactor(LabelNote, 1);


	setLayout(MainLayout);
}


void RenameDlg::InitDialog()
{
	m_pctrlNote->setText(m_Note);

	if(!m_bEnableOverwrite) OverwriteButton->setEnabled(false);

	if(m_strName.length())
	{
		m_pctrlMessage->setText(m_strQuestion);
	}
	else
	{
		m_pctrlMessage->setText("Enter a name");
	}
	m_StartName = m_strName;
	m_pctrlName->setText(m_StartName);
	m_pctrlName->setFocus();
	m_pctrlName->selectAll();

	for (int i=0; i<m_pstrArray->size(); i++)
	{
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
				OnOK();
			}
			return;
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
		QMessageBox::warning(this, tr("Warning"), tr("Must enter a name"));
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
			if (QMessageBox::Yes == QMessageBox::question(window(), "Question", str,
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


void RenameDlg::OnSelChangeList(int item)
{
	QListWidgetItem *pItem =  m_pctrlNameList->currentItem();

	QString str = pItem->text();
	m_pctrlName->setText(str);
	m_pctrlName->selectAll();
}




void RenameDlg::OnDoubleClickList(QListWidgetItem * pItem)
{
//	QListWidgetItem *pItem =  m_pctrlNameList->currentItem();

	QString str = pItem->text();
	m_pctrlName->setText(str);
	OnOK();
}












