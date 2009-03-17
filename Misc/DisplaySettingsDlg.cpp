/****************************************************************************

	QDipSettingsDlg Class
	Copyright (C) 2008-2008 Andre Deperrois xflr5@yahoo.com

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


#include <QtGui>
#include "DisplaySettingsDlg.h"
#include "../MainFrame.h"
#include "../Graph/GraphDlg.h"


DisplaySettingsDlg::DisplaySettingsDlg(void *pParent)
{
	m_pMainFrame = pParent;
	m_bIsGraphModified = false;

	SetupLayout();

	connect(m_pctrlStyles, SIGNAL(activated(const QString &)),this, SLOT(OnStyleChanged(const QString &)));

	connect(m_pctrlBackColor, SIGNAL(clicked()),this, SLOT(OnBackgroundColor()));
	connect(m_pctrlGraphSettings, SIGNAL(clicked()),this, SLOT(OnGraphSettings()));
	connect(m_pctrlTextColor, SIGNAL(clicked()),this, SLOT(OnTextColor()));
	connect(m_pctrlTextFont, SIGNAL(clicked()),this, SLOT(OnTextFont()));

	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
	connect(CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}


void DisplaySettingsDlg::SetupLayout()
{
	QVBoxLayout *MainLayout = new QVBoxLayout;

	m_pctrlStyles = new QComboBox;

	QRegExp regExp("Q(.*)Style");
	QString defaultStyle = QApplication::style()->metaObject()->className();

	if (defaultStyle == QLatin1String("QMacStyle"))
		defaultStyle = QLatin1String("Macintosh (Aqua)");
	else if (regExp.exactMatch(defaultStyle))
		defaultStyle = regExp.cap(1);

	m_pctrlStyles->addItems(QStyleFactory::keys());

	m_pctrlStyles->setCurrentIndex(m_pctrlStyles->findText(defaultStyle));

	QGridLayout *ColorButtons = new QGridLayout;
	m_pctrlBackColor      = new ColorButton;
	m_pctrlGraphSettings  = new QPushButton;
	m_pctrlTextFont       = new QPushButton;
	m_pctrlTextColor      = new ColorButton;
	m_pctrlBackColor->setMinimumWidth(120);
	m_pctrlGraphSettings->setMinimumWidth(120);
	m_pctrlTextFont->setMinimumWidth(120);
	m_pctrlTextColor->setMinimumWidth(120);
	QLabel *lab1 = new QLabel("View Background");
	QLabel *lab2 = new QLabel("All Graph Settings");
	QLabel *lab3 = new QLabel("Text Font");
	QLabel *lab4 = new QLabel("Text Color");
	ColorButtons->addWidget(lab1, 1,1);
	ColorButtons->addWidget(lab2, 1,2);
	ColorButtons->addWidget(lab3, 3,1);
	ColorButtons->addWidget(lab4, 3,2);
	ColorButtons->addWidget(m_pctrlBackColor,2,1);
	ColorButtons->addWidget(m_pctrlGraphSettings,2,2);
	ColorButtons->addWidget(m_pctrlTextFont,4,1);
	ColorButtons->addWidget(m_pctrlTextColor,4,2);

	QHBoxLayout *CommandButtons = new QHBoxLayout;
	OKButton = new QPushButton(tr("OK"));
	OKButton->setAutoDefault(false);
	CancelButton = new QPushButton(tr("Cancel"));
	CancelButton->setAutoDefault(false);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(OKButton);
	CommandButtons->addStretch(1);
	CommandButtons->addWidget(CancelButton);
	CommandButtons->addStretch(1);

	MainLayout->addStretch(1);
	MainLayout->addWidget(m_pctrlStyles);
	MainLayout->addStretch(1);
	MainLayout->addLayout(ColorButtons);
	MainLayout->addSpacing(20);
	MainLayout->addStretch(1);
	MainLayout->addLayout(CommandButtons);
	MainLayout->addStretch(1);

	setLayout(MainLayout);
}

void DisplaySettingsDlg::InitDialog()
{
	m_pctrlBackColor->SetColor(m_BackgroundColor);
	m_pctrlTextColor->SetColor(m_TextColor);
	m_pctrlTextFont->setText(m_TextFont.family());
	m_pctrlStyles->setCurrentIndex(m_pctrlStyles->findText(m_StyleName));

}

void DisplaySettingsDlg::OnStyleChanged(const QString &StyleName)
{
	qApp->setStyle(StyleName);

	m_StyleName = StyleName;
}


void DisplaySettingsDlg::OnBackgroundColor()
{
	bool bOK = true;
	QRgb rgba = m_BackgroundColor.rgba();
	rgba = QColorDialog::getRgba(rgba, &bOK);
	m_BackgroundColor = QColor::fromRgba(rgba);
	m_pctrlBackColor->SetColor(m_BackgroundColor);
}


void DisplaySettingsDlg::OnGraphSettings()
{
	if(!m_pRefGraph) return;

	GraphDlg dlg;
	QGraph graph;
	graph.CopySettings(m_pRefGraph);

	dlg.m_pMemGraph = m_pRefGraph;
	dlg.m_pGraph = &graph;

	dlg.SetParams();

	if(dlg.exec() == QDialog::Accepted)
	{
		m_pRefGraph->CopySettings(dlg.m_pGraph);
		m_bIsGraphModified = true;
	}
}


void DisplaySettingsDlg::OnTextColor()
{
	bool bOK = true;
	QRgb rgba = m_TextColor.rgba();
	rgba = QColorDialog::getRgba(rgba, &bOK);
	m_TextColor = QColor::fromRgba(rgba);
	m_pctrlBackColor->SetColor(m_TextColor);

}



void DisplaySettingsDlg::OnTextFont()
{
	bool ok;

	QFont font = QFontDialog::getFont(&ok, m_TextFont, this);

	if (ok)
	{
		m_TextFont = font;
		m_pctrlTextFont->setText(m_TextFont.family());
		m_pctrlTextFont->setFont(m_TextFont);
	}
}








