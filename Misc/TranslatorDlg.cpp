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
#include <QLabel>
#include <QStringList>
#include <QListWidgetItem>
#include <QMutableStringListIterator>
#include <QDir>
#include <QMessageBox>
#include <QTranslator>
#include <QtDebug>

TranslatorDlg::TranslatorDlg(void *pParent)
{
	setWindowTitle(tr("Language settings"));
	QString LanguageName = tr("English");// will be translated in the ts & qm files and this will be used to fill the QListWidget

	SetupLayout();
	InitDialog();
}


void TranslatorDlg::OnOK()
{
	//read user language selection and exit
	QListWidgetItem *pItem =  m_pctrlLanguageList->currentItem();
	if(pItem)
	{
		if(pItem->text()=="English") m_LanguageFilePath = "";
		else m_LanguageFilePath = qmFileForLanguage[pItem->text()];
	}
	else
	{
		m_LanguageFilePath = "";
	}

	QMessageBox::warning(this, tr("Warning"), tr("The change will take effect at the next session"));

	accept();
}



void TranslatorDlg::SetupLayout()
{
	QLabel *lab = new QLabel(tr("Select the application's default language:"));
	m_pctrlLanguageList = new QListWidget;
	m_pctrlLanguageList->setMinimumHeight(300);
	connect(m_pctrlLanguageList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(OnOK()));

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	QPushButton *OKButton = new QPushButton(tr("OK"));
	QPushButton *CancelButton = new QPushButton(tr("Cancel"));
	OKButton->setAutoDefault(true);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);
	connect(OKButton, SIGNAL(clicked()),this, SLOT(OnOK()));
	connect(CancelButton, SIGNAL(clicked()),this, SLOT(reject()));

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addWidget(lab);
	MainLayout->addStretch(1);
	MainLayout->addWidget(m_pctrlLanguageList);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);
}


void TranslatorDlg::InitDialog()
{
	QStringList qmFiles = findQmFiles();
	qmFiles.sort();
	qmFileForLanguage.insert("English", "English");
	m_pctrlLanguageList->addItem("English (default)");
	for (int i = 0; i < qmFiles.size(); ++i)
	{
		qmFileForLanguage.insert(languageName(qmFiles[i]), qmFiles[i]);
		m_pctrlLanguageList->addItem(languageName(qmFiles[i]));
	}
}


QStringList TranslatorDlg::findQmFiles()
{
	QDir dir;
	dir.setPath( QDir::current().path()+"/translations");
	if(!dir.exists())
	{
		QMessageBox::warning(this, tr("Warning"), tr("The directory ")+dir.path()+tr("does not exist"));
	}

	QStringList fileNames = dir.entryList(QStringList("*.qm"), QDir::Files,
										  QDir::Name);
	QMutableStringListIterator i(fileNames);
	while (i.hasNext())
	{
		i.next();
		i.setValue(dir.filePath(i.value()));
	}

	return fileNames;
}


QString TranslatorDlg::languageName(const QString &qmFile)
{
	QTranslator translator;
	translator.load(qmFile);

	return translator.translate("TranslatorDlg", "English");
}




