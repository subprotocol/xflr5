/****************************************************************************

    AboutQ5 Class
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
#include "AboutQ5.h"
#include "../MainFrame.h"
#include <QBitmap>

AboutQ5::AboutQ5(void *parent)
{
	setWindowTitle(tr("About QFLR5"));

	m_pMainFrame = parent;

	SetupLayout();
	connect(OKButton, SIGNAL(clicked()),this, SLOT(accept()));
}

AboutQ5::~AboutQ5()
{
}

void AboutQ5::SetupLayout()
{
	MainFrame *pMainFrame = (MainFrame*)m_pMainFrame;
	QHBoxLayout *LogoLayout = new QHBoxLayout;
	m_IconQ5 = new QLabel;
	m_IconQ5->setObjectName("iconQ5");
	m_IconQ5->setGeometry(QRect(20, 0, 61, 71));
	m_IconQ5->setPixmap(QPixmap(QString::fromUtf8(":/images/Q5.png")));
	QLabel *lab1  = new QLabel(pMainFrame->m_VersionName);
	lab1->setAlignment(Qt::AlignLeft);
	LogoLayout->addWidget(m_IconQ5);
	LogoLayout->addStretch(1);
	LogoLayout->addWidget(lab1);
	LogoLayout->addStretch(4);


	QLabel *lab2  = new QLabel(tr("Copyright (C) M. Drela and H. Youngren 2000 - XFoil v6.94"));
	QLabel *lab3  = new QLabel(tr("Copyright (C) Matthieu Scherrer 2004 - Miarex v1.00"));
	QLabel *lab4  = new QLabel(tr("Copyright (C) Andre Deperrois 2003-2009"));
	QLabel *lab5  = new QLabel(tr("This program is distributed in the hope that it will be useful,"));
	QLabel *lab6  = new QLabel(tr("but WITHOUT ANY WARRANTY; without even the implied warranty of"));
	QLabel *lab7  = new QLabel(tr("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."));
	QLabel *lab8  = new QLabel(tr("This program has been developped exclusively for the analysis of model aircraft"));
	QLabel *lab9  = new QLabel(tr("Any other usage is strongly disapproved"));
	QLabel *lab10 = new QLabel(tr("Program distributed  under the terms of the GNU General Public License"));

	QPushButton *AboutQt = new QPushButton(tr("About Qt"));
	connect(AboutQt, SIGNAL(clicked()), qApp, SLOT(aboutQt()));

	m_pXFLR5Link = new QLabel;
	m_pXFLR5Link->setText("<a href=http://xflr5.sourceforge.net/xflr5.htm>http://xflr5.sourceforge.net/xflr5.htm</a>");
	m_pXFLR5Link->setOpenExternalLinks(true);
	m_pXFLR5Link->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse);

	QHBoxLayout *LinkLayout = new QHBoxLayout;
	LinkLayout->addWidget(m_pXFLR5Link);
	LinkLayout->addStretch(1);
	LinkLayout->addWidget(AboutQt);

	OKButton = new QPushButton(tr("OK"));
	QHBoxLayout *OKLayout = new QHBoxLayout;
	OKLayout->addStretch(1);
	OKLayout->addWidget(OKButton);
	OKLayout->addStretch(1);

	QVBoxLayout *MainLayout = new QVBoxLayout;
	MainLayout->addLayout(LogoLayout);
	MainLayout->addStretch(1);
	MainLayout->addWidget(lab2);
	MainLayout->addWidget(lab3);
	MainLayout->addWidget(lab4);
	MainLayout->addStretch(1);
	MainLayout->addWidget(lab5);
	MainLayout->addWidget(lab6);
	MainLayout->addWidget(lab7);
	MainLayout->addStretch(1);
	MainLayout->addWidget(lab8);
	MainLayout->addWidget(lab9);
	MainLayout->addStretch(1);
	MainLayout->addWidget(lab10);
	MainLayout->addStretch(1);
	MainLayout->addLayout(LinkLayout);
	MainLayout->addStretch(1);
	MainLayout->addLayout(OKLayout);
	setLayout(MainLayout);
	setMinimumHeight(400);
}


